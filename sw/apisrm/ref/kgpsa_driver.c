/*
 * Copyright (C) 1997 by
 * Digital Equipment Corporation, Maynard, Massachusetts.
 * All rights reserved.
 *
 * This software is furnished under a license and may be used and copied
 * only  in  accordance  of  the  terms  of  such  license  and with the
 * inclusion of the above copyright notice. This software or  any  other
 * copies thereof may not be provided or otherwise made available to any
 * other person.  No title to and  ownership of the  software is  hereby
 * transferred.
 *
 * The information in this software is  subject to change without notice
 * and  should  not  be  construed  as a commitment by Digital Equipment
 * Corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by Digital.
 *
 *
 * Abstract:	KGPSA port driver
 *
 * Author:	Dennis Mazur
 *
 * Modifications:
 *
 *	dm	28-May-1996	Initial entry.
 */


#include "cp$src:platform.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:ev_def.h"
#include "cp$src:msg_def.h"
#include "cp$src:scsi_def.h"
#include "cp$src:common.h"
#include "cp$src:stddef.h"
#include "cp$src:pb_def.h"
#include "cp$src:sb_def.h"
#include "cp$src:ub_def.h"
#include "cp$inc:platform_io.h"
#include "cp$src:print_var.h"
#include "cp$src:ctype.h"
#include "cp$src:wwid.h"
#include "cp$src:mem_def.h"
#include "cp$src:kgpsa_emx.h"
#include "cp$src:fc.h"
#include "cp$src:kgpsa_def.h"
#include "cp$src:kgpsa_pb_def.h"
#include "cp$src:kgpsa_literals.h"
#include "cp$inc:kernel_entry.h"


#if KGPSA_POLLED
extern struct QUEUE pollq;
#endif

/*
 * These defines control different builds and the compilation of optional
 * routines.
 */
#define ABORTS 0
#define FC_ABTS 1
#define XRI_SUPPORT      0
#define FABRIC_SUPPORT   1
#define CT_EXTRA         0
#define DONT_RETRY       0            /* don't retry ELS. symbol for readability */
#define DONT_WAIT        0            /* Dont wait for command completion */
#define WAIT             1            /* Wait for command completion.  Some
                                         ELSes wait.  FCP always wait     */
#define NO_CONT_IOCB     0            /* Passed to indicate only one IOCB
                                         is in the arguement list to be 
                                         issue.                           */
#define IOCB_CMD         1            /* manipulate IOCB cmd queue        */
#define IOCB_RSP         2            /* manipulate IOCB rsp queue        */
#define SKIP_IT          0
#define DO_IT            1            /* print and debug control flag     */
#define BASE0         0x10            /* PCI bar */
#define BASE1         0x18            /* PCI bar */
#define BASE2         0x20            /* PCI bar */
#define ACTIVE_QUEUE     2            /* memory resident IOCB q           */
#define IOCB_OWNER_OFFSET   7*4       /* Easy access to the Owner bit     */
#define PAYLOAD_SIZE       2048
#define FCP_CMD_PAYLOAD_SIZE 32
#define ELS_CMD_PAYLOAD_SIZE  ( sizeof(SERV_PARM) + sizeof (int) )
#define ELS_RSP_PAYLOAD_SIZE  128
#define MAX_ELS_BUF 256

/*
 * These are error codes returned from the fcp_cmd routine to the
 * kgpsa_command routine.  I want to start their values above the
 * values for msg_success(0) and msg_failure(1).  
 */
#define fc_sts_retry             2
#define fc_sts_loop_open         3
#define fc_sts_link_down         4
#define fc_sts_seq_timeout       5
#define fc_sts_incarnerr         6
#define fc_sts_fcp_cmd_field_err 7

#define SCSI_TARGETS_SZ  sizeof(int)*FC_MAX_NPORTS
#define CT_BUF_SZ        sizeof(CT_HDR)+SCSI_TARGETS_SZ
/*
 * Malloc short hand redefinitions
 */
/*
 * #define LEAK_WATCH (MKLOAD || SBLOAD || PKLOAD || RAWHIDE_DEBUG || TURBO_DEBUG)
 */

#define LEAK_WATCH 0

#if LEAK_WATCH
#  define fc_malloc( x, y )  lw_malloc( pb, x, y )
#  define fc_free( x, y )    lw_free( pb, x, y )
#else
#  define fc_malloc( x, y )  kgpsa_malloc(x)
#  define fc_free( x, y )    kgpsa_free(x)
#endif


#define malloc(x,label) fc_malloc(x,label)
#define free(x,label)   fc_free(x,label)
#define rs(x)           kgpsa_read_slim(pb,x)
#define ws(x,y)         kgpsa_write_slim(pb,x,y)
#define rl(x)           kgpsa_read_csr(pb,x)
#define wl(x,y)         kgpsa_write_csr(pb,x,y)


/*
 * Routines to help with Big Endian / Little Endian translation
 */
#define SWAP_SHORT(x)   ((((x) & 0xFF) << 8) | ((x) >> 8))
#define SWAP_LONG(x)    ((((x) & 0xFF)<<24) | (((x) & 0xFF00)<<8) | \
                         (((x) & 0xFF0000)>>8) | (((x) & 0xFF000000)>>24))


#if ( RAWHIDE || TURBO )
#define $MAP_FACTOR( x )  0
#else
#define $MAP_FACTOR( x ) io_get_window_base( x )
#endif

/*
 * These globals that are for use with command line routine calling.
 * They are only ever used referenced in command line called routines and
 * only from debug functions.
 * 
 */
#if KGPSA_DEBUG
/*
 *  Could use pg_controllers counter to malloc the correct array size.
 */
int kgpsa_pbs[10] = {0,0,0,0,0,0,0,0,0,0};
int num_kgpsa = 0;
#endif

/*
 *  Array of all 126 valid AL_PA's (excluding FL_PORT AL_PA 0)
 */

uchar staticAlpaArray[] =
   {
   0x01, 0x02, 0x04, 0x08, 0x0F, 0x10, 0x17, 0x18, 0x1B, 0x1D,
   0x1E, 0x1F, 0x23, 0x25, 0x26, 0x27, 0x29, 0x2A, 0x2B, 0x2C,
   0x2D, 0x2E, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x39, 0x3A,
   0x3C, 0x43, 0x45, 0x46, 0x47, 0x49, 0x4A, 0x4B, 0x4C, 0x4D,
   0x4E, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x59, 0x5A, 0x5C,
   0x63, 0x65, 0x66, 0x67, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E,
   0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x79, 0x7A, 0x7C, 0x80,
   0x81, 0x82, 0x84, 0x88, 0x8F, 0x90, 0x97, 0x98, 0x9B, 0x9D,
   0x9E, 0x9F, 0xA3, 0xA5, 0xA6, 0xA7, 0xA9, 0xAA, 0xAB, 0xAC,
   0xAD, 0xAE, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB9, 0xBA,
   0xBC, 0xC3, 0xC5, 0xC6, 0xC7, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD,
   0xCE, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD9, 0xDA, 0xDC,
   0xE0, 0xE1, 0xE2, 0xE4, 0xE8, 0xEF
   };



/*
 * The following are always tailorable with a deposit
 */
volatile int kgpsa_topology = -1;   
                            /* -1 Not used; use nvram value  */
                            /* 2  FLAGS_TOPOLOGY_MODE_PT_PT  */
                            /* 4  FLAGS_TOPOLOGY_MODE_LOOP   */
/*
 * Tailorable constants
 */
#define TAILORABLE_AT_RUNTIME 0 | KGPSA_DEBUG

                                      
/************************ CONTROLE KNOBS **********************************/
/*                                                                        */
#if TAILORABLE_AT_RUNTIME                   
volatile int kgpsa_trace_iocb_doit =       0     ;
volatile int kgpsa_no_autologin =          0     ;
volatile int kgpsa_test_zeroalpa =         1     ;
volatile int kgpsa_write_ring_sleep_time = 500   ;
volatile int kgpsa_flood_byte =            0     ;
volatile int kgpsa_rcv_bufs =              10    ;
volatile int kgpsa_probing_timer =         10000 ;
volatile int kgpsa_els_full_timer =        1000  ;
volatile int reset_hold_delay =            1     ;
volatile int kgpsa_fcp_timeout =           20    ;
volatile int kgpsa_mbxtimeout =            10000 ;
volatile int kgpsa_ratov_time =            2     ;
volatile int kgpsa_edtov_time =            1000  ;
volatile int kgpsa_latt_sleep =            1000  ;
volatile int kgpsa_latt_timer =            1000  ;
volatile int kgpsa_read_alpa_sleep =       200   ;
volatile int kgpsa_link_down_timer =       60000 ;
volatile int kgpsa_abort_retry_timer =     10000 ;
volatile int kgpsa_status_sleep =          0000  ;
volatile int kgpsa_iocb_timer =            35000 ;
volatile int kgpsa_fcp_retries =           5     ;
volatile int kgpsa_fcp_retry_delay =       5000  ;
volatile int kgpsa_down_int_cnt =          5     ;
volatile int CT_late_int_cnt =             3     ;
volatile int fc_wake_interval =            5000  ;
volatile int kgthrottle =                  10    ;
volatile int fc_els_retries =              3     ;

#else

#define kgpsa_trace_iocb_doit              0     
#define kgpsa_no_autologin                 0     
#define kgpsa_test_zeroalpa                1     
#define kgpsa_write_ring_sleep_time        500   
#define kgpsa_flood_byte                   0     
#define kgpsa_rcv_bufs                     10    
#define kgpsa_probing_timer                10000 
#define kgpsa_els_full_timer               1000  
#define reset_hold_delay                   1     
#define kgpsa_fcp_timeout                  20    
#define kgpsa_mbxtimeout                   10000 
#define kgpsa_ratov_time                   2     
#define kgpsa_edtov_time                   1000  
#define kgpsa_latt_sleep                   1000  
#define kgpsa_latt_timer                   1000  
#define kgpsa_read_alpa_sleep              200   
#define kgpsa_link_down_timer              60000 
#define kgpsa_abort_retry_timer            10000 
#define kgpsa_status_sleep                 0000  
#define kgpsa_iocb_timer                   35000 
#define kgpsa_fcp_retries                  5     
#define kgpsa_fcp_retry_delay              5000  
#define kgpsa_down_int_cnt                 5     
#define CT_late_int_cnt                    3
#define fc_wake_interval                   5000  
#define kgthrottle                         10    
#define fc_els_retries                     3     

#endif


#if KGPSA_DEBUG
/*
 * referenced in both DEBUG and non-DEBUG mode   
 */
volatile int kgpsa_stress_cnt = 100;
volatile int kgpsa_seagate_alpa = 0;
volatile int kgpsa_new_seagate_id = 0x00;
volatile int kgpsa_debug_flag = 0;
volatile int kgpsa_wait = 0;           /* used to make ELSs wait for comp */
volatile int kgpsa_queue_verbose = 0;
volatile int kgpsa_trace_filter = 0;
volatile int kgpsa_readla_dump = 0;
/* 
 * below variables only referenced in DEBUG mode 
 */
volatile int big_read_xfer_len = 0x300000;
volatile int send_flow_cnt=0;
volatile int send_flow_cnt_enable = 0;
volatile int kgpsa_byhand_els     = 0; /*value used in ratov */
volatile int kgpsa_big_trace      = 0;

#if 1
volatile int iocbbuf             = 20;        /* # of 100 byte iocb records */
#else
volatile int iocbbuf             = 500;        /* # of 100 byte iocb records */
#endif

#else

#define kgpsa_debug_flag     0
#define kgpsa_wait           DONT_WAIT
#define kgpsa_queue_verbose  0
#define kgpsa_trace_filter   0
#define kgpsa_readla_dump    0
#define kgpsa_big_trace      0
#define iocbbuf             20          /* # of 100 byte iocb records */

#endif


#if !MODULAR
extern struct kgpsa_pb *find_pb();
extern register_wwid();
#endif


/*
 * Externals
 */
extern int diagnostic_mode_flag;
extern int null_procedure();
extern struct LOCK spl_kernel;
extern __int64 cbip;
extern int scsi_poll;
extern struct ZONE _align (QUADWORD) defzone;
extern int wwidmgr_mode;

extern struct registered_portname **portnames;
extern unsigned int num_portnames;
extern unsigned int portname_ev_mask;


/*
 * Prototypes or routines referenced as address data
 */
unsigned int kgpsa_read_csr(struct kgpsa_pb *pb, int offset);
kgpsa_write_csr(struct kgpsa_pb *pb, int offset, unsigned int data);
unsigned int kgpsa_read_slim ( struct kgpsa_pb *pb, int offset );



#if KGPSA_DEBUG

void dprintf(fmt, a, b, c, d, e, f, g, h, i, j )
{
if ((kgpsa_debug_flag&1) != 0)
  pprintf(fmt, a, b, c, d, e, f, g, h, i, j);
}
void d2printf(fmt, a, b, c, d, e, f, g, h, i, j )
{
if ((kgpsa_debug_flag & 0x2) != 0)
        pprintf(fmt, a, b, c, d, e, f, g, h, i, j);
}
void d4printf(fmt, a, b, c, d, e, f, g, h, i, j )
{
if ((kgpsa_debug_flag & 0x4) != 0)
        pprintf(fmt, a, b, c, d, e, f, g, h, i, j);
}
void d8printf(fmt, a, b, c, d, e, f, g, h, i, j )
{
if ((kgpsa_debug_flag & 0x8) != 0)
        pprintf(fmt, a, b, c, d, e, f, g, h, i, j);
}

#else

#define dprintf(fmt, args)
#define d2printf(fmt, args)
#define d4printf(fmt, args)
#define d8printf(fmt, args)

#endif







/*
 *********************************************************************
 *                                                                   *
 *  Start of DEBUG routines                                          *
 *                                                                   *
 *********************************************************************
 */
#if !KGPSA_DEBUG 
#  define kgpsa_dump_blk( x, y, z )
#  define kgpsa_dump_mbx( x , y, z )
#  define kgpsa_dump_sparm( x, y )
#  define kgpsa_dump_iocb( x, y, z )
#  define kgpsa_dump_iocb_pair( x, y, z, a )
#  define kgpsa_dump_csrs( x, y )
#  define kgpsa_dump_ct( x, y )
#  define kgpsa_dump_active_q( x, y )
#  define kgpsa_dump_active_q_brief( x, y ) 
#  define kgpsa_dump_owner( x, y )
#  define kgpsa_iocb_failure_display( x, y, z )
#  define kgpsa_check_owner( x, y, z, a )
#  define kgpsa_sanity_check_rings( x )
#if 0
#  define kgpsa_pollx( )
#endif
#  define kgpsa_dump_ip( x )
#  define kgpsadt( x )
#  define kgpsadtgo()
#  define kgpsa_ntix( x )
#  define kgpsa_event_checker()
#  define dump_sb( x )
#  define KGPSA_DUMP_COMMAND()
#  define kgpsa_dump_pb( x )
#  define kgpsa_dump_rings( x )

void kgpsa_dump_state()
{
  /* This space intentionally left blank */
}

#else

void kgpsa_dump_blk( char *label,  int *blk, int do_it )
{
   int j;

   if (do_it) {

     pprintf("\n %s  (%x)\n",label,blk);
     for (j=0; j<31; j=j+4){
       pprintf("%3d:%08x  %3d:%08x %3d:%08x  %3d:%08x\n",
         j*4,blk[j],(j+1)*4,blk[j+1],(j+2)*4,blk[j+2],(j+3)*4,blk[j+3]);
     }
   }
}


void kgpsa_dump_mbx( char *label, int *mbx, int do_it )
{
   int j;

   if (do_it) {

     pprintf("\n %s  (%x)\n",label,mbx);
     for (j=0; j<8; j=j+4){
       pprintf("%3d:%08x  %3d:%08x %3d:%08x  %3d:%08x\n",
         j*4,mbx[j],(j+1)*4,mbx[j+1],(j+2)*4,mbx[j+2],(j+3)*4,mbx[j+3]);
     }
   }
}


void kgpsa_dump_sparm(int *buf, int doit)
{
 if (doit) {
  printf("Common Service Parameters: %x %x %x %x\n",buf[0],buf[1],buf[2],buf[3]);
  printf("Port Name: %x %x \n", buf[4], buf[5]);
  printf("Node / Fabric Name: %x %x\n", buf[6], buf[7]);
  printf("Class 1 Service Params: %x  %x %x %x\n",buf[8],buf[9],buf[10],buf[11]);
  printf("Class 2 Service Params: %x  %x %x %x\n",buf[12],buf[13],buf[14],buf[15]);
  printf("Class 3 Service Params: %x  %x %x %x\n",buf[16],buf[17],buf[18],buf[19]);
  printf("Reserved: %x  %x %x %x\n",buf[20],buf[21],buf[22],buf[23]);
  printf("Vendor Version Level: %x  %x %x %x\n",buf[24],buf[25],buf[26],buf[27]);
 }
}


void kgpsa_dump_iocb( char *label, int *blk, int do_it )
{
 int j;
 int *cmd,*rsp;
 if (!do_it) return;
 pprintf("%s",label);

 pprintf(" %x", blk[0] );
 if (blk[0]) {
   cmd = blk[0] - $MAP_FACTOR(kgpsa_pbs[pbx]);
   pprintf("/%x", *cmd );
 }

 pprintf(" %x", blk[1] );

 pprintf(" %x", blk[2] );
 if (blk[2]) {
   rsp = blk[2] - $MAP_FACTOR(kgpsa_pbs[0]);
   pprintf("/%x", *rsp );
 }

 pprintf(" %x", blk[3] );
 pprintf(" %x", blk[4] );
 pprintf(" %x", blk[5] );
 pprintf(" %x", blk[6] );
 pprintf(" %x", blk[7] );

 pprintf("\n");
}


void kgpsa_dump_iocb_pair( char *label, int *cmd, int *rsp, int do_it )
{
 int j;
 if (!do_it) return;
 pprintf("%s : \n",label);
 for (j=0; j<8; j++)
   {
   pprintf(" %08x",cmd[j] );
   }
 pprintf("\n");
 for (j=0; j<8; j++)
   {
   pprintf(" %08x",rsp[j] );
   }
 pprintf("\n");
}


void kgpsa_dump_csrs( int pbx, int do_it )
{
 struct kgpsa_pb *pb;
 int ha, ca, hs, hc;
 pb = kgpsa_pbs[pbx];
 if (!do_it) return;
 ha = rl(HA);
 ca = rl(CA);
 hs = rl(HS);
 hc = rl(HC);
 pprintf("HA:%08x CA:%08x HS:%08x HC:%08x  BIUCFG:%x  BIUSTAT:%x\n",
            p_args6(ha, ca, hs, hc, rl(HA+0x10),rl(HA+0x14)) );

#define $HA(y,z) if(ha & y) pprintf("\t y %s\n", z);
#define $CA(y,z) if(ca & y) pprintf("\t y %s\n", z);
#define $HS(y,z) if(hs & y) pprintf("\t y %s\n", z);
#define $HC(y,z) if(hc & y) pprintf("\t y %s\n", z);


 pprintf("\nHA:%08X \n", ha);
 $HA(HA_R0RE_REQ  ,   "0x00000001  /* Bit  0 */" );
 $HA(HA_R0CE_RSP  ,   "0x00000002  /* Bit  1 */" );
 $HA(HA_R0ATT     ,   "0x00000008  /* Bit  3 */" );
 $HA(HA_R1RE_REQ  ,   "0x00000010  /* Bit  4 */" );
 $HA(HA_R1CE_RSP  ,   "0x00000020  /* Bit  5 */" );
 $HA(HA_R1ATT     ,   "0x00000080  /* Bit  7 */" );
 $HA(HA_R2RE_REQ  ,   "0x00000100  /* Bit  8 */" );
 $HA(HA_R2CE_RSP  ,   "0x00000200  /* Bit  9 */" );
 $HA(HA_R2ATT     ,   "0x00000800  /* Bit 11 */" );
 $HA(HA_R3RE_REQ  ,   "0x00001000  /* Bit 12 */" );
 $HA(HA_R3CE_RSP  ,   "0x00002000  /* Bit 13 */" );
 $HA(HA_R3ATT     ,   "0x00008000  /* Bit 15 */" );
 $HA(HA_LATT      ,   "0x20000000  /* Bit 29 */" );
 $HA(HA_MBATT     ,   "0x40000000  /* Bit 30 */" );
 $HA(HA_ERATT     ,   "0x80000000  /* Bit 31 */" );

 pprintf("\nCA:%08X \n", ca);
 $CA(CA_R0CE_REQ  ,   "0x00000001  /* Bit  0 */" );
 $CA(CA_R0RE_RSP  ,   "0x00000002  /* Bit  1 */" );
 $CA(CA_R0ATT     ,   "0x00000008  /* Bit  3 */" );
 $CA(CA_R1CE_REQ  ,   "0x00000010  /* Bit  4 */" );
 $CA(CA_R1RE_RSP  ,   "0x00000020  /* Bit  5 */" );
 $CA(CA_R1ATT     ,   "0x00000080  /* Bit  7 */" );
 $CA(CA_R2CE_REQ  ,   "0x00000100  /* Bit  8 */" );
 $CA(CA_R2RE_RSP  ,   "0x00000200  /* Bit  9 */" );
 $CA(CA_R2ATT     ,   "0x00000800  /* Bit 11 */" );
 $CA(CA_R3CE_REQ  ,   "0x00001000  /* Bit 12 */" );
 $CA(CA_R3RE_RSP  ,   "0x00002000  /* Bit 13 */" );
 $CA(CA_R3ATT     ,   "0x00008000  /* Bit 15 */" );
 $CA(CA_MBATT     ,   "0x40000000  /* Bit 30 */" );


 pprintf("\nHS:%08X \n", hs);
 $HS(HS_MBRDY     ,   "0x00400000  /* Bit 22 */" );
 $HS(HS_FFRDY     ,   "0x00800000  /* Bit 23 */" );
 $HS(HS_FFER8     ,   "0x01000000  /* Bit 24 */" );
 $HS(HS_FFER7     ,   "0x02000000  /* Bit 25 */" );
 $HS(HS_FFER6     ,   "0x04000000  /* Bit 26 */" );
 $HS(HS_FFER5     ,   "0x08000000  /* Bit 27 */" );
 $HS(HS_FFER4     ,   "0x10000000  /* Bit 28 */" );
 $HS(HS_FFER3     ,   "0x20000000  /* Bit 29 */" );
 $HS(HS_FFER2     ,   "0x40000000  /* Bit 30 */" );
 $HS(HS_FFER1     ,   "0x80000000  /* Bit 31 */" );
 $HS(HS_FFERM     ,   "0xFF000000  /* Mask for error bits 31:24 */" );



 pprintf("\nHC:%08X \n", hc);
 $HC(HC_MBINT_ENA ,   "0x00000001  /* Bit  0 */" );
 $HC(HC_R0INT_ENA ,   "0x00000002  /* Bit  1 */" );
 $HC(HC_R1INT_ENA ,   "0x00000004  /* Bit  2 */" );
 $HC(HC_R2INT_ENA ,   "0x00000008  /* Bit  3 */" );
 $HC(HC_R3INT_ENA ,   "0x00000010  /* Bit  4 */" );
 $HC(HC_INITHBI   ,   "0x02000000  /* Bit 25 */" );
 $HC(HC_INITMB    ,   "0x04000000  /* Bit 26 */" );
 $HC(HC_INITFF    ,   "0x08000000  /* Bit 27 */" );
 $HC(HC_LAINT_ENA ,   "0x20000000  /* Bit 29 */" );
 $HC(HC_ERINT_ENA ,   "0x80000000  /* Bit 31 */" );


}

void kgpsa_dump_ct( CT_HDR *ct_hdr, int do_it )
{
if( do_it )
{
 pprintf("ct_hdr->uId.bits.inId: %x\n", ct_hdr->uId.bits.inId );
 pprintf("ct_hdr->uId.bits.ctRev: %x\n", ct_hdr->uId.bits.ctRev );
 pprintf("ct_hdr->fcsType: %x\n", ct_hdr->fcsType ); 
 pprintf("ct_hdr->fcsSubType: %x\n", ct_hdr->fcsSubType );
 pprintf("ct_hdr->options: %x \n", ct_hdr->options );
 pprintf("ct_hdr->rsvd1: %x \n", ct_hdr->rsvd1 );
 pprintf("ct_hdr->usc.bits.size: %x\n", ct_hdr->usc.bits.size );
 pprintf("ct_hdr->usc.bits.cmsRsp: %x \n", ct_hdr->usc.bits.cmdRsp );
 pprintf("ct_hdr->rsvd2: %x\n", ct_hdr->rsvd2 );
 pprintf("ct_hdr->reasonCode: %x\n", ct_hdr->reasonCode );
 pprintf("ct_hdr->explanation: %x\n", ct_hdr->explanation );
 pprintf("ct_hdr->vendorUnique: %x\n", ct_hdr->vendorUnique );
}
}

void kgpsa_dump_active_q ( int pbx, int ringno )
{
  struct kgpsa_pb *pb;
  struct QUEUE active_q;
  struct QUEUE *qp;
  IOCB_HDR *t;
  FC_BRD_INFO *binfo;
  RING *rp;

  pb = kgpsa_pbs[pbx];
  binfo = &pb->binfo;
  rp = &binfo->fc_ring[ringno];
  qp = rp->active_q.flink;
  while (qp != &rp->active_q.flink) {
    t = (int)qp - offsetof(IOCB_HDR, iocb_q);
    kgpsa_dump_iocbhdr( t );
    qp = qp->flink;
  }
}



void kgpsa_dump_active_q_brief ( int pbx, int ringno )
{
  struct kgpsa_pb *pb;
  struct QUEUE active_q;
  struct QUEUE *qp;
  IOCB_HDR *t;
  FC_BRD_INFO *binfo;
  RING *rp;

  pb = kgpsa_pbs[pbx];
  binfo = &pb->binfo;
  rp = &binfo->fc_ring[ringno];
  qp = rp->active_q.flink;
  while (qp != &rp->active_q.flink) {
    t = (int)qp - offsetof(IOCB_HDR, iocb_q);
    pprintf( "%x ", t->iotag );
    qp = qp->flink;
  }
  pprintf("\n");

  rp = &binfo->fc_ring[ringno];
  qp = rp->active_q.flink;
  while (qp != &rp->active_q.flink) {
    t = (int)qp - offsetof(IOCB_HDR, iocb_q);
    pprintf( "    LINKUP:%x RSCN:%x: ", t->LINKUPeventTag, t->RSCNeventTag, t->iotag );
    kgpsa_dump_iocb( "", &t->cmd_iocb, DO_IT );
    qp = qp->flink;
  }


}


void kgpsa_dump_received_slim( int pbx, int ringno )
{
 int i;
 int offset;
 struct kgpsa_pb *pb = kgpsa_pbs[pbx];
 FC_BRD_INFO *binfo;
 RING	*rp;
 IOCB riocb, *rsp=&riocb;
 int status;
 char cidx[4];
 int ringidx;
 binfo = &pb->binfo;
 rp = &binfo->fc_ring[ringno];

 kgpsa_read_ring( pb, IOCB_RSP, ringno, rp->fc_rspidx, rsp );
 ringidx = rp->fc_rspidx; 
 
 while (rsp->ulpOwner == OWN_HOST) {
   sprintf( &cidx, "    %x:", ringidx );
   kgpsa_dump_iocb( &cidx, rsp, DO_IT );
   ringidx++;
   if (ringidx >= rp->fc_numRiocb) {
     ringidx = 0;
   }
   if(ringidx == rp->fc_rspidx) break;
   kgpsa_read_ring( pb, IOCB_RSP, ringno, ringidx, rsp );
 } /* while */

}




void kgpsa_dump_done_q ( int pbx, int ringno )
{
  struct kgpsa_pb *pb;
  struct QUEUE done_q;
  struct QUEUE *qp;
  IOCB_HDR *t;
  FC_BRD_INFO *binfo;
  RING *rp;

  pb = kgpsa_pbs[pbx];
  binfo = &pb->binfo;
  rp = &binfo->fc_ring[ringno];
  qp = rp->done_q.flink;
  while (qp != &rp->done_q.flink) {
    t = (int)qp - offsetof(IOCB_HDR, iocb_q);
    kgpsa_dump_iocbhdr( t );
    qp = qp->flink;
  }
}


void kgpsa_dump_iocbhdr( IOCB_HDR *t )
{
#if 0
#          define      IOCB_NOSTATE   0x00
#          define      IOCB_PENDING   0x01    /* waiting to be sent to adapter */
#          define      IOCB_ACTIVE    0x02    /* active in adapter */
#          define      IOCB_DONE      0x03    /* complete */
#endif
 int *p;
 int i;
 pprintf("IOCB_HDR (%x)\n",t);
 pprintf("  (%x)struct QUEUE iocb_q; %x\n", &t->iocb_q, t->iocb_q );
 pprintf("  (%x)int bde1_bufptr; %x\n", &t->bde1_bufptr, t->bde1_bufptr );
 p = t->bde1_bufptr;
 if (p) {
    pprintf("---");
    for (i=0; i<8; i++)
      pprintf("%x ", p[i] );
    pprintf("\n");
 }
 pprintf("  (%x)int bde2_bufptr; %x\n", &t->bde2_bufptr, t->bde2_bufptr );
 p = t->bde2_bufptr;
 if (p) {
    pprintf("---");
    for (i=0; i<8; i++)
      pprintf("%x ", p[i] );
    pprintf("\n");
 }
 pprintf("  (%x)int bde3_bufptr; %x\n", &t->bde3_bufptr, t->bde3_bufptr );
 pprintf("  (%x)(*callback) ; %x\n", &t->callback, t->callback );
 pprintf("  (%x)*callback_arg; %x\n", &t->callback_arg, t->callback_arg );
 pprintf("  (%x)int LINKUPeventTag; %x\n", &t->LINKUPeventTag, t->LINKUPeventTag );
 pprintf("  (%x)int RSCNeventTag; %x\n", &t->RSCNeventTag, t->RSCNeventTag );
 pprintf("  (%x)int iotag; %x\n", &t->iotag, t->iotag );
 pprintf("  (%x)iocb_state; %x %s\n", &t->iocb_state, t->iocb_state, 
                                            iocb_state_text[t->iocb_state] );
 pprintf("  (%x)int did; %x\n", &t->did, t->did );
 pprintf("  (%x)int wait_for_completion; %x  %s\n", &t->wait_for_completion,
                   t->wait_for_completion, wait_text[t->wait_for_completion] );
 pprintf("  (%x)int iocb_q_state; %x\n", &t->iocb_q_state, t->iocb_q_state );
 pprintf("  (%x)struct SEMAPHORE rsp; %x\n", &t->rsp, t->rsp );
 pprintf("  (%x)struct TIMREQ xrsp_t; %x\n", &t->xrsp_t, t->xrsp_t );
 pprintf("  (%x)IOCB cmd_iocb; %x\n", &t->cmd_iocb, t->cmd_iocb );
 kgpsa_dump_iocb( "---", &t->cmd_iocb, 1 );
 pprintf("  (%x)int cmd_slim_idx; %x (%x)\n", &t->cmd_slim_idx,
                                t->cmd_slim_idx,t->cmd_slim_idx*sizeof(IOCB));
 pprintf("  (%x)IOCB cont_iocb; %x\n", &t->cont_iocb, t->cont_iocb );
 kgpsa_dump_iocb( "---", &t->cont_iocb, 1 );
 pprintf("  (%x)int cont_slim_idx; %x (%x)\n", &t->cont_slim_idx,
                                t->cont_slim_idx,t->cont_slim_idx*sizeof(IOCB));
 pprintf("  (%x)IOCB rsp_iocb; %x\n", &t->rsp_iocb, t->rsp_iocb );
 kgpsa_dump_iocb("---", &t->rsp_iocb, 1 );
 pprintf("  (%x)int rsp_slim_idx; %x (%x)\n", &t->rsp_slim_idx,
                                t->rsp_slim_idx,t->rsp_slim_idx*sizeof(IOCB));
 pprintf("  (%x)struct kgpsa_sb *sb; %x\n", &t->sb, t->sb );
 return;
}


int kgpsa_check_owner( struct kgpsa_pb *pb, int type, int ringno, int ringidx )
{
 int i;
 int offset;
 FC_BRD_INFO *binfo;
 RING	*rp;
 int ringaddr;
 int value;
 binfo = &pb->binfo;
 rp = &binfo->fc_ring[ringno];
 if (type == IOCB_CMD) {
   ringaddr = rp->fc_cmdringaddr;
 } else if (type == IOCB_RSP) {
   ringaddr = rp->fc_rspringaddr;
 } else {
   dprintf("Bad iocb type\n", p_args0);
 }
 offset = ringaddr + (sizeof(IOCB) * ringidx) + IOCB_OWNER_OFFSET;
 value = rs( offset ) & OWN_CHIP;
 return value;
}



int kgpsa_sanity_check_rings( int pbx )
{
 int i,status,cmdidx,rspidx;
 struct kgpsa_pb *pb;
 MAILBOX mb, *lmb=&lmb;
 FC_BRD_INFO *binfo;
 RING	*rp;
 IOCB *cmd,*rsp,cmd_iocb,rsp_iocb;
 pb = kgpsa_pbs[pbx];
 binfo = &pb->binfo;
 for (i=0;i<4;i++)
   {
    if ( (i==1) || (i==3) ) continue;
    status = kgpsa_read_rconfig( pb, i, lmb );
    if (status != msg_success) return status;
      cmdidx = (lmb->un.varRdRConfig.nextCmdOffset  -
                         lmb->un.varRdRConfig.cmdRingOffset  ) / sizeof(IOCB);

    rspidx = (lmb->un.varRdRConfig.nextRspOffset  -
                         lmb->un.varRdRConfig.rspRingOffset  ) / sizeof(IOCB);

    rp = &binfo->fc_ring[i];

    dprintf("CMD ring %x  host/adapter  %x/%x \n", p_args3(i,rp->fc_cmdidx,cmdidx) );
    dprintf("RSP ring %x  host/adapter  %x/%x \n", p_args3(i,rp->fc_rspidx,rspidx) );
    if (rp->fc_cmdidx != cmdidx)
      {
       dprintf("Sanity check error: CMD ring %x  host/adapter  %x/%x \n",
                                             p_args3(i,rp->fc_cmdidx,cmdidx) );
      }
    if (rp->fc_rspidx != rspidx)
      {
       dprintf("Sanity check error: RSP ring %x  host/adapter  %x/%x \n",
                                             p_args3(i,rp->fc_rspidx,rspidx) );
      }

    if ( kgpsa_check_owner( pb, IOCB_RSP, i, rp->fc_rspidx) == OWN_HOST )
      {
       dprintf("ERROR: next RSP buffer owned by host\n", p_args0 );
      }
   }
}

#endif

#if 1
void kgpsa_pollx(int pb)
{
  kgpsa_poll( pb, 1, NO_VERBOSE );
  pprintf("polling pb: %x\n", pb );
}
#endif

#if KGPSA_DEBUG
void kgpsa_dump_ip( char *fname )
{
  struct FILE *fp;
  fp = fopen (fname, "r");
  pprintf("%s ub:%x", fp->ip->name, fp->ip->misc );
}



void dump_sb( struct kgpsa_sb *sb )
{
 pprintf("dump_sb - Not yet implemented\n");
}




#define KGPSA_DUMP_COMMAND()
#if 0
 \
                             \
    pprintf("kgpsa_command:\n",p_args0); \
    pprintf("\t int lun:%x\n", p_args1(lun) ); \
    pprintf("\t unsigned char *cmd:%x\n", p_args1(cmd) ); \
    pprintf("\t int cmd_len:%x\n", p_args1(cmd_len) ); \
    pprintf("\t unsigned char *dat_out:%x\n", p_args1(dat_out) ); \
    pprintf("\t int dat_out_len:%x\n", p_args1(dat_out_len) ); \
    pprintf("\t unsigned char *dat_in:%x\n", p_args1(dat_in) ); \
    pprintf("\t int dat_in_len:%x\n", p_args1(dat_in_len) ); \
    pprintf("\t unsigned char *sts:%x\n", p_args1(sts) ); \
    pprintf("\t unsigned char *sense_dat_in:%x\n", p_args1(sense_dat_in) ); \
    pprintf("\t int *sense_dat_in_len:%x\n", p_args1(sense_dat_in_len) ); \
    pprintf("KGPSA COMMAND BLOCK %x %x\n", command->pb->command, command); \
    pprintf("	entry type %x\n", command->entry_type); \
    pprintf("	entry count %x\n", command->entry_count); \
    pprintf("	entry unused %x\n", command->entry_unused); \
    pprintf("	entry flags %x\n", command->entry_flags); \
    pprintf("	handle %x\n", command->handle); \
    pprintf("	lun %x\n", command->lun); \
    pprintf("	target %x\n", command->target); \
    pprintf("	cdb length %x\n", command->cdb_length); \
    pprintf("	control flags  %x\n", command->control_flags); \
    pprintf("	reserved %x\n", command->reserved); \
    pprintf("	timeout %x\n", command->timeout); \
    pprintf("	data count %x\n", command->data_count); \
    pprintf("	data address %x\n", command->data_address); \
    pprintf("	data length %x\n", command->data_length); \
    for (i = 0; i < command->cdb_length; i++) \
	    pprintf("	cdb[%d] %x\n", i, command->cdb[i]);     \

#endif


void kgpsa_dump_pb( struct kgpsa_pb *pb )
{
 int i;
 pprintf(" (%x)   struct pb pb;  %x \n", &pb->pb, pb->pb);
 pprintf(" (%x)   int (*setmode)(); %x\n", &pb->pb.setmode, pb->pb.setmode );
 pprintf(" (%x)   unsigned long int ref; %x \n", &pb->pb.ref, pb->pb.ref );
 pprintf(" (%x)   unsigned long int state; %x \n", &pb->pb.state , pb->pb.state );
 pprintf(" (%x)   unsigned long int mode; %x \n", &pb->pb.mode, pb->pb.mode);
 pprintf(" (%x)   unsigned long int desired_mode; %x \n", &pb->pb.desired_mode, pb->pb.desired_mode);
 pprintf(" (%x)   unsigned long int hose; %x \n", &pb->pb.hose, pb->pb.hose);
 pprintf(" (%x)   unsigned long int bus; %x \n", &pb->pb.bus, pb->pb.bus);
 pprintf(" (%x)   unsigned long int slot; %x \n", &pb->pb.slot, pb->pb.slot);
 pprintf(" (%x)   unsigned long int function; %x \n", &pb->pb.function, pb->pb.function);
 pprintf(" (%x)   unsigned long int channel; %x \n", &pb->pb.channel, pb->pb.channel);
 pprintf(" (%x)   unsigned char *protocol; %s \n", &pb->pb.protocol, pb->pb.protocol);
 pprintf(" (%x)   unsigned long int controller; %x \n", &pb->pb.controller, pb->pb.controller);
#if !RAWHIDE
 pprintf("    unsigned char controller_id [4]; %4c \n", pb->pb.controller_id);
#endif
 pprintf(" (%x)   unsigned char port_name [32]; %s \n", &pb->pb.port_name, pb->pb.port_name);
 pprintf(" (%x)   unsigned long int type; %x \n", &pb->pb.type, pb->pb.type);
 pprintf(" (%x)   unsigned long int csr; %x \n", &pb->pb.csr, pb->pb.csr);
 pprintf(" (%x)   unsigned long int vector; %x \n", &pb->pb.vector, pb->pb.vector);
 pprintf(" (%x)   unsigned char *device; %s \n", &pb->pb.device, pb->pb.device);
 pprintf(" (%x)   unsigned long int *letter; %c \n", &pb->pb.letter, pb->pb.letter);
 pprintf(" (%x)   void *misc; %x \n", &pb->pb.misc, pb->pb.misc);
 pprintf(" (%x)   void *pdm; %x \n", &pb->pb.pdm, pb->pb.pdm);
 pprintf(" (%x)   void *config_device; %x \n", &pb->pb.config_device, pb->pb.config_device);
 pprintf(" (%x)   unsigned long int order; %x \n", &pb->pb.order, pb->pb.order);
 pprintf(" (%x)   struct sb **sb; %x \n", &pb->pb.sb, pb->pb.sb);
 pprintf(" (%x)   unsigned long int num_sb; %x \n", &pb->pb.num_sb, pb->pb.num_sb);
 pprintf(" (%x)   unsigned char name [32]; %s \n", &pb->pb.name, pb->pb.name);
 pprintf(" (%x)   unsigned char alias [8]; %s \n", &pb->pb.alias, pb->pb.alias);
 pprintf(" (%x)   unsigned char info [24]; %s \n", &pb->pb.info, pb->pb.info);
 pprintf(" (%x)   unsigned char version [16]; %s \n", &pb->pb.version, pb->pb.version);
 pprintf(" (%x)   unsigned char string [80]; %s \n", &pb->pb.string, pb->pb.string);
 pprintf(" (%x)   unsigned char id [20]; %s \n", &pb->pb.id, pb->pb.id);
 pprintf(" (%x)   unsigned char *hardware_name; %s \n", &pb->pb.hardware_name, pb->pb.hardware_name);
 pprintf(" (%x)   E_MEMQ e_memq; %x \n", &pb->pb.e_memq, pb->pb.e_memq);
 pprintf(" (%x)   int status; %x \n", &pb->pb.status, pb->pb.status);
 pprintf(" (%x)   struct POLLQ pq; %x \n", &pb->pb.pq, pb->pb.pq);
 pprintf(" (%x)   struct pb *parent; %x \n", &pb->pb.parent, pb->pb.parent);
#if DRIVERSHUT
 pprintf(" (%x)   struct io_device *tbl; %x \n", &pb->pb.tbl, pb->pb.tbl);
 pprintf(" (%x)   struct pb *dpb; %x \n", &pb->pb.dpb, pb->pb.dpb);
#endif


 pprintf(" (%x)   int pbx; %x\n", &pb->pbx, pb->pbx);
 pprintf(" (%x)   struct  PBQ	pbq; %x\n", &pb->pbq, pb->pbq);
 pprintf(" (%x)   unsigned long int node_id; %x\n", &pb->node_id, pb->node_id);
 pprintf(" (%x)   unsigned __int64 slim_base; %x\n", &pb->slim_base, pb->slim_base);
 pprintf(" (%x)   unsigned __int64 csr_mem_base; %x\n", &pb->csr_mem_base, pb->csr_mem_base);
 pprintf(" (%x)   unsigned __int64 csr_io_base; %x\n", &pb->csr_io_base, pb->csr_io_base);
 pprintf(" (%x)   int	prefetch; %x\n", &pb->prefetch, pb->prefetch);
 pprintf(" (%x)   int node_count; %x\n", &pb->node_count, pb->node_count);
 pprintf(" (%x)   FC_BRD_INFO binfo; %x\n", &pb->binfo, pb->binfo);
 pprintf(" (%x)   struct  QUEUE iocbhdr_q; %x\n", &pb->iocbhdr_q, pb->iocbhdr_q);
 pprintf(" (%x)   int	    iocbhdr_q_cnt; %x\n", &pb->iocbhdr_q_cnt, pb->iocbhdr_q_cnt);
 pprintf(" (%x)   struct  QUEUE els_ring_buf_q; %x\n", &pb->els_ring_buf_q, pb->els_ring_buf_q);
 pprintf(" (%x)   struct SEMAPHORE els_owner_s; %x\n", &pb->els_owner_s, pb->els_owner_s);
 pprintf(" (%x)   struct SEMAPHORE fcp_owner_s; %x\n", &pb->fcp_owner_s, pb->fcp_owner_s);
 pprintf(" (%x)   struct SEMAPHORE ring_s; %x\n", &pb->ring_s, pb->ring_s);
 pprintf(" (%x)   struct LOCK mbx_l; %x\n", &pb->mbx_l, pb->mbx_l);
 pprintf(" (%x)   struct LOCK record_l; %x\n", &pb->record_l, pb->record_l);
 pprintf(" (%x)   struct TIMERQ poll_t; %x\n", &pb->poll_t, pb->poll_t);
 pprintf(" (%x)   volatile int els_suspend; %x\n", &pb->els_suspend, pb->els_suspend);
 pprintf(" (%x)   volatile int fcp_suspend; %x\n", &pb->fcp_suspend, pb->fcp_suspend);
 pprintf(" (%x)   volatile unsigned long devState; %x\n", &pb->devState, pb->devState);
 pprintf(" (%x)   unsigned long int attType; %x\n", &pb->attType, pb->attType);
 pprintf(" (%x)   unsigned long int topology; %x\n", &pb->topology, pb->topology);
 pprintf(" (%x)   unsigned long int LINKUPeventTag; %x\n", &pb->LINKUPeventTag, pb->LINKUPeventTag);
 pprintf(" (%x)   unsigned long int DiscLINKUPeventTag; %x\n", &pb->DiscLINKUPeventTag, pb->DiscLINKUPeventTag);
 pprintf(" (%x)   unsigned long int RSCNeventTag; %x\n", &pb->RSCNeventTag, pb->RSCNeventTag);
 pprintf(" (%x)   unsigned long int DiscRSCNeventTag; %x\n", &pb->DiscRSCNeventTag, pb->DiscRSCNeventTag);
 pprintf(" (%x)   unsigned short int elscmdcnt;   %x\n", &pb->elscmdcnt, pb->elscmdcnt);
 pprintf(" (%x)   unsigned short int rsvd2; %x\n", &pb->rsvd2, pb->rsvd2);
 pprintf(" (%x)   unsigned short int fcpcmdcnt; %x\n", &pb->fcpcmdcnt, pb->fcpcmdcnt);
 pprintf(" (%x)   unsigned short int rsvd3; %x\n", &pb->rsvd3, pb->rsvd3);
 pprintf(" (%x)   unsigned short int scsicmdcnt; %x\n", &pb->scsicmdcnt, pb->scsicmdcnt);
 pprintf(" (%x)   unsigned short int rsvd4; %x\n", &pb->rsvd4, pb->rsvd4);
 pprintf(" (%x)   int sb_cnt; %x\n", &pb->sb_cnt, pb->sb_cnt);
 pprintf(" (%x)   volatile int discovery_index; %x\n", &pb->discovery_index, pb->discovery_index);
 pprintf(" (%x)   int *scsi_targets; %x\n", &pb->scsi_targets, pb->scsi_targets);
 pprintf(" (%x)   int scsi_targets_cnt; %x\n", &pb->scsi_targets_cnt , pb->scsi_targets_cnt );
 for( i=0; i<pb->scsi_targets_cnt; i++)
   {
     pprintf(" (%x)      scsi_targets[%d]: %x\n", &pb->scsi_targets[i], i, pb->scsi_targets[i] );
   }
 pprintf(" (%x)   int last_read_iocb[8]; %x \n", &pb->last_read_iocb, pb->last_read_iocb);
 pprintf(" (%x)   volatile int nprobe_state[FC_MAX_NPORTS]; \n", &pb->nprobe_state, pb->nprobe_state);
#     define	PROBE_IN_PROGRESS 1
#     define	PROBE_DONE        2

 for( i=0; i<pb->scsi_targets_cnt; i++)
   {
     pprintf(" (%x)      nprobe_state[%d]: %x\n", &pb->nprobe_state[i], i, pb->nprobe_state[i] );
   }
 pprintf(" (%x)   unsigned long int poll_active; %x\n", &pb->poll_active, pb->poll_active);
 pprintf(" (%x)   int begin_used_bytes; %x\n", &pb->begin_used_bytes, pb->begin_used_bytes);
 pprintf(" (%x)   int end_used_bytes; %x\n", &pb->end_used_bytes, pb->end_used_bytes);
 pprintf(" (%x)   struct TASK fc; %x\n", &pb->fc, pb->fc);
 pprintf(" (%x)   struct TASK poll; %x\n", &pb->poll, pb->poll);

 pprintf(" (%x)   volatile int slim_trace_enable;	 %x\n", &pb->slim_trace_enable, pb->slim_trace_enable);
 pprintf(" (%x)   volatile int slim_trace_entries; %x\n", &pb->slim_trace_entries, pb->slim_trace_entries);
 pprintf(" (%x)   volatile int slim_trace_currentry; %x\n", &pb->slim_trace_currentry, pb->slim_trace_currentry);
 pprintf(" (%x)   volatile int slim_trace_cnt; %x\n", &pb->slim_trace_cnt, pb->slim_trace_cnt);
 pprintf(" (%x)   volatile int *slim_trace_start; %x\n", &pb->slim_trace_start, pb->slim_trace_start);

 pprintf(" (%x)   volatile int iocb_trace_enable; %x\n", &pb->iocb_trace_enable, pb->iocb_trace_enable);
 pprintf(" (%x)   volatile int iocb_trace_entries; %x\n", &pb->iocb_trace_entries, pb->iocb_trace_entries);
 pprintf(" (%x)   volatile int iocb_trace_currentry; %x\n", &pb->iocb_trace_currentry, pb->iocb_trace_currentry);
 pprintf(" (%x)   volatile int iocb_trace_cnt; %x\n", &pb->iocb_trace_cnt, pb->iocb_trace_cnt);
 pprintf(" (%x)   volatile int *iocb_trace_start; %x\n", &pb->iocb_trace_start, pb->iocb_trace_start);
 


}



void kgpsa_dump_rings( struct kgpsa_pb *pb )
{
 FC_BRD_INFO *binfo;
 RING *rp;
 int i, j, ring;
 binfo = &pb->binfo;


   for ( i=0; i < binfo->fc_ffnumrings; i++ ) {
     rp = &binfo->fc_ring[i];
     pprintf("Ring %d \n", i );

pprintf("    struct QUEUE rcv_buf_q; %x\n", rp-> rcv_buf_q  );
pprintf("    int rcv_buf_cnt; %x\n", rp->rcv_buf_cnt  );
pprintf("    int rcv_buf_highwater; %x\n", rp->rcv_buf_highwater  );
pprintf("    char *rcv_blk; %x\n", rp->rcv_blk  );
pprintf("    struct QUEUE active_q; %x\n", rp->active_q  );
pprintf("    struct QUEUE done_q; %x\n", rp->done_q  );
pprintf("    struct QUEUE receive_q; %x\n", rp->receive_q  );
pprintf("    struct SEMAPHORE pending_s; %x\n", rp->pending_s  );
pprintf("    struct SEMAPHORE active_s; %x\n", rp->active_s  );
pprintf("    struct SEMAPHORE done_s; %x\n", rp->done_s  );
pprintf("    struct SEMAPHORE receive_s; %x\n", rp->receive_s  );
pprintf("    uchar	fc_numCiocb; %x\n", rp->fc_numCiocb  );
pprintf("    uchar	fc_numRiocb; %x\n", rp->fc_numRiocb  );
pprintf("    uchar	fc_rspidx; %x\n", rp->fc_rspidx  );
pprintf("    uchar	fc_cmdidx; %x\n", rp->fc_cmdidx  );
pprintf("    ulong	fc_cmdringaddr; %x\n", rp->fc_cmdringaddr  );
pprintf("    ulong	fc_rspringaddr; %x\n", rp->fc_rspringaddr  );
pprintf("    ushort	fc_iotag; %x\n", rp->fc_iotag  );
pprintf("    ushort	fc_ringno; %x\n", rp->fc_ringno  );
   }
}



void kgpsa_dump_state()
{
#if KGPSA_DEBUG
  struct kgpsa_pb *pb;
  pb = kgpsa_pbs[0];
  pprintf(" --- kgpsa_dump_state --- \n");
  pprintf("ELS active_q: ");
  kgpsa_dump_active_q_brief( pb, FC_ELS_RING );
  pprintf("ELS received in slim: \n");
  kgpsa_dump_received_slim( pb, FC_ELS_RING );

  pprintf("FCP active_q: ");
  kgpsa_dump_active_q_brief( pb, FC_FCP_RING );
  pprintf("FCP received in slim: \n");
  kgpsa_dump_received_slim( pb, FC_FCP_RING );
#endif
}



#endif



/*
 *********************************************************************
 *                                                                   *
 *  End of DEBUG routines                                            *
 *                                                                   *
 *********************************************************************
 */




void kgpsa_dump_slim ( struct kgpsa_pb *pb, int start, int len )
{
 int i,j,offset;
 unsigned int x[8];
 i = 0;
 while (i < len)
  {
   for (j=0; j<8; j++)
    {
      offset = (i+j)*4;
      /*
       * Use rs() when you want the trace to have these reads.  If
       * that becomes annoying, then make this an indmeml
       */
    if (pb->prefetch)
      x[j] = indmeml(pb, pb->slim_base+start+offset);
    else
      x[j] = inmeml(pb, pb->slim_base+start+offset);
    }
   pprintf("%04x %8x %8x %8x %8x %8x %8x %8x %8x\n",
                  start+(i*4),x[0],x[1],x[2],x[3],x[4],x[5],x[6],x[7]);
   i = i + 8;
  }
 return;
}





/*
 *********************************************************************
 *                                                                   *
 *  RECORDING    routines                                            *
 *                                                                   *
 *********************************************************************
 */
volatile int kgpsa_put_record_doit = 1;

#define TRACE_PACKET_SIZE 27
struct kgpsa_record_head {
    int	    type;   
    int	    entrynum;
};
struct kgpsa_iocb_record {
    int	    type;   
    int	    entrynum;
    int	    offset;
    int	    iocb[8];
    int	    atbde[14]; 
    int     linkupevtag;
    int     rscnevtag;
};
struct kgpsa_latt_record {
    int	    type;   
    int	    entrynum;
    int	    latt_type;
    int	    linkupevtag;
    int	    rscnevtag;
};
struct kgpsa_mbx_record {
    int	    type;
    int	    entrynum;
    int	    mbx[8];
};

struct kgpsa_dyn_record {
    int	    type;   
    int	    entrynum;
    int	    *leakq_adr;
    int	    *adr;
    int	    size;
    char    label[20];
};

struct kgpsa_probe_record {
    int	    type;   
    int	    entrynum;
    int	    did;
    int	    oldstate;
    int	    newstate;
};

struct kgpsa_record {
   union {
     int word[TRACE_PACKET_SIZE];
     struct kgpsa_iocb_record  iocb_r;
     struct kgpsa_latt_record  latt_r;
     struct kgpsa_mbx_record   mbx_r;
     struct kgpsa_dyn_record   dyn_r;
     struct kgpsa_probe_record probe_r;
   } un;
};

#        define   KREC_TYPE_READ        1     /* iocb read  */
#        define   KREC_TYPE_WRITE       2     /* iocb write */
#	 define	  KREC_TYPE_LATT        3     /* latt interrupt */
#        define   KREC_TYPE_MBX_R       4     /* mbx read   */
#	 define	  KREC_TYPE_MBX_W       5     /* mbx write  */
#	 define   KREC_TYPE_DYN_MALLOC  6     /* malloc */
#	 define   KREC_TYPE_DYN_FREE    7     /* free */
#	 define   KREC_TYPE_PROBE_STATE 8     /* probe state */

#define RECORDING (MKLOAD || SBLOAD || PKLOAD || \
                   RAWHIDE_DEBUG || TURBO_DEBUG )


#if !RECORDING
#define kgpsa_record_iocb( x, y, z, a)
#define kgpsa_record_latt( x, y, z, a )
#define kgpsa_record_mbx( x, y, z )
#define kgpsa_record_dyn( x, y, z, a, b, c )
#define kgpsa_record_probe( x, y, a, b )
#define kgpsa_himem_put_record( x, y )
#define kgpsa_dump_iocb_trace( x, y, z, a )
#define kgpsa_record_iocb_setup( x, y, z )

#else


#define BIG_RECORDING 0

#if !BIG_RECORDING
#define kgpsa_record_dyn( x, y, z, a, b, c )
#define kgpsa_record_probe( x, y, a, b )
#endif


void kgpsa_record_iocb( struct kgpsa_pb *pb, int type, int offset, int *i_iocb)
{
  int i,j;
  struct kgpsa_record krec;

  for(i=0;i<TRACE_PACKET_SIZE;i++) {
    krec.un.word[i] = 0;
  }
  krec.un.iocb_r.type   = type;   
  krec.un.iocb_r.entrynum = pb->iocb_trace_cnt;
  krec.un.iocb_r.offset = offset;
  for ( i=0; i<IOCB_WSIZE; i++ ) {
    krec.un.iocb_r.iocb[i] = i_iocb[i];
    if ( (i==0) || (i==2) ){
      if (i==0) 
        j = 0;
      else
        j = 7;
      if (i_iocb[i] != 0 ) {
        krec.un.iocb_r.atbde[j]   = *(int*)(i_iocb[i] - $MAP_FACTOR(pb)     );
        krec.un.iocb_r.atbde[j+1] = *(int*)(i_iocb[i] - $MAP_FACTOR(pb) + 4 );
        krec.un.iocb_r.atbde[j+2] = *(int*)(i_iocb[i] - $MAP_FACTOR(pb) + 8 );
        krec.un.iocb_r.atbde[j+3] = *(int*)(i_iocb[i] - $MAP_FACTOR(pb) + 12);
        krec.un.iocb_r.atbde[j+4] = *(int*)(i_iocb[i] - $MAP_FACTOR(pb) + 16);
        krec.un.iocb_r.atbde[j+5] = *(int*)(i_iocb[i] - $MAP_FACTOR(pb) + 20);
        krec.un.iocb_r.atbde[j+6] = *(int*)(i_iocb[i] - $MAP_FACTOR(pb) + 24);
      } else {
        krec.un.iocb_r.atbde[j]   = 0;
        krec.un.iocb_r.atbde[j+1] = 0;
        krec.un.iocb_r.atbde[j+2] = 0;
        krec.un.iocb_r.atbde[j+3] = 0;
        krec.un.iocb_r.atbde[j+4] = 0;
        krec.un.iocb_r.atbde[j+5] = 0;
        krec.un.iocb_r.atbde[j+6] = 0;
      }
    }
  }
  krec.un.iocb_r.linkupevtag = pb->DiscLINKUPeventTag;
  krec.un.iocb_r.rscnevtag = pb->DiscRSCNeventTag;
  kgpsa_himem_put_record( pb, &krec );

}

void kgpsa_record_latt( struct kgpsa_pb *pb, int type, int linkup, int rscn )
{
 int i;
 struct kgpsa_record krec;

 for(i=0;i<TRACE_PACKET_SIZE;i++) {
   krec.un.word[i] = 0;
 }
 krec.un.latt_r.type = KREC_TYPE_LATT;
 krec.un.latt_r.entrynum = pb->iocb_trace_cnt;
 krec.un.latt_r.latt_type = type;
 krec.un.latt_r.linkupevtag = linkup;
 krec.un.latt_r.rscnevtag = rscn;
 kgpsa_himem_put_record( pb, &krec );
}

void kgpsa_record_mbx( struct kgpsa_pb *pb, int type, int *i_mbx )
{
 int i;
 struct kgpsa_record krec;

 for(i=0;i<TRACE_PACKET_SIZE;i++) {
   krec.un.word[i] = 0;
 }
 krec.un.mbx_r.type = type;
 krec.un.mbx_r.entrynum = pb->iocb_trace_cnt;
 for(i=0;i<8;i++){
   krec.un.mbx_r.mbx[i] = i_mbx[i];
 }
 kgpsa_himem_put_record( pb, &krec );
}

#if BIG_RECORDING
void kgpsa_record_dyn( struct kgpsa_pb *pb, int type, int *leakq_adr, 
                                              int *adr, char *label, int size )
{
 int i;
 struct kgpsa_record krec;

 for(i=0;i<TRACE_PACKET_SIZE;i++) {
   krec.un.word[i] = 0;
 }
 krec.un.dyn_r.type = type;
 krec.un.dyn_r.entrynum = pb->iocb_trace_cnt;
 krec.un.dyn_r.leakq_adr = leakq_adr;
 krec.un.dyn_r.adr = adr;
 krec.un.dyn_r.size = size;
 strcpy( &krec.un.dyn_r.label, label );
 kgpsa_himem_put_record( pb, &krec );
}
#endif


#if BIG_RECORDING
void kgpsa_record_probe( struct kgpsa_pb *pb, int did, int oldstate, int newstate )
{
 int i;
 struct kgpsa_record krec;

 for(i=0;i<TRACE_PACKET_SIZE;i++) {
   krec.un.word[i] = 0;
 }
 krec.un.probe_r.type = KREC_TYPE_PROBE_STATE;
 krec.un.probe_r.entrynum = pb->iocb_trace_cnt;
 krec.un.probe_r.did = did;
 krec.un.probe_r.oldstate = oldstate;
 krec.un.probe_r.newstate = newstate;
 kgpsa_himem_put_record( pb, &krec );
}
#endif


void kgpsa_himem_put_record( struct kgpsa_pb *pb, struct kgpsa_record *b )
{
  int i;
  struct kgpsa_record *kbase, *k;
  struct kgpsa_record_head *kh;

  if(pb==0) {
     pprintf("kgpsa_put_record - zero pb\n");
     return;
  }

  if( kgpsa_put_record_doit == 0 )
    return;

  spinlock(&pb->record_l);
  kh = b;
  kh->entrynum = pb->iocb_trace_cnt;
  kbase = pb->iocb_trace_start;
  if ( pb->iocb_trace_enable ) {
    pb->iocb_trace_currentry = pb->iocb_trace_cnt % pb->iocb_trace_entries;
    k = &kbase[pb->iocb_trace_currentry];
    for(i=0;i<TRACE_PACKET_SIZE;i++) {
      k->un.word[i] = b->un.word[i];
    }

    pb->iocb_trace_cnt++;
  }
  spinunlock(&pb->record_l);
}

void kgpsa_dump_iocb_trace( struct kgpsa_pb *pb, int index, int cnt, 
                                                              int dump_style )
{
#define DUMP_STYLE_SHORT 1
#define DUMP_STYLE_LONG  2

  int i,j, start,type;
  int *b;
  int end;
  struct kgpsa_record *kbase, *krec;
  char *rw[] = {"0","R","W","L","MR", "MW", "MALLOC", "FREE", "PS" };
  char *probetxt[] = { "INVALID - 0", "PROBE_IN_PROGRESS", "PROBE_DONE" };

  if(pb==0) {
     pprintf("kgpsa_dump_iocb_trace - zero pb\n");
     do_bpt();
  }

  /*
   * Need this because Xdelta ;z cant take more than 3 parameters
   */
  if( (dump_style != DUMP_STYLE_SHORT) && (dump_style != DUMP_STYLE_SHORT) )
         dump_style = DUMP_STYLE_SHORT;


  end = min( index+cnt, pb->iocb_trace_cnt );
  kbase = pb->iocb_trace_start;
  for(i=index; i<end; i++) {
    krec = &kbase[i];
    pprintf("%x (%x) %x %s ", i, krec, krec->un.word[1], rw[krec->un.word[0]]);
    switch(krec->un.word[0]) {
      case KREC_TYPE_READ:
      case KREC_TYPE_WRITE:
        if (dump_style == DUMP_STYLE_LONG)
	{
#if BIG_RECORDING
	pprintf("%x %x %x %x %x %x %x %x %x LINKUP: %x RSCN:%x \n",
	   krec->un.iocb_r.offset,
	   krec->un.iocb_r.iocb[0],
	   krec->un.iocb_r.iocb[1],
	   krec->un.iocb_r.iocb[2],
	   krec->un.iocb_r.iocb[3],
	   krec->un.iocb_r.iocb[4],
	   krec->un.iocb_r.iocb[5],
	   krec->un.iocb_r.iocb[6],
	   krec->un.iocb_r.iocb[7],
           krec->un.iocb_r.linkupevtag,
           krec->un.iocb_r.rscnevtag);
	pprintf("            %x/ %x %x %x %x %x %x %x\n",
	   krec->un.iocb_r.iocb[0], krec->un.iocb_r.atbde[0],
                                    krec->un.iocb_r.atbde[1],
                                    krec->un.iocb_r.atbde[2], 
                                    krec->un.iocb_r.atbde[3], 
                                    krec->un.iocb_r.atbde[4], 
                                    krec->un.iocb_r.atbde[5], 
                                    krec->un.iocb_r.atbde[6]);

	pprintf("            %x/ %x %x %x %x %x %x %x\n",
	   krec->un.iocb_r.iocb[2], krec->un.iocb_r.atbde[7], 
                                    krec->un.iocb_r.atbde[8],
                                    krec->un.iocb_r.atbde[9],
                                    krec->un.iocb_r.atbde[10],
                                    krec->un.iocb_r.atbde[11],
                                    krec->un.iocb_r.atbde[12],
                                    krec->un.iocb_r.atbde[13]);

#endif
	} else if (dump_style == DUMP_STYLE_SHORT) {
	pprintf("%x %x/%x %x %x/%x %x %x %x %x %x LINKUP:%x RSCN:%x\n",
	   krec->un.iocb_r.offset,
	   krec->un.iocb_r.iocb[0], krec->un.iocb_r.atbde[0], 
	   krec->un.iocb_r.iocb[1],
	   krec->un.iocb_r.iocb[2], krec->un.iocb_r.atbde[7],
	   krec->un.iocb_r.iocb[3],
	   krec->un.iocb_r.iocb[4],
	   krec->un.iocb_r.iocb[5],
	   krec->un.iocb_r.iocb[6],
	   krec->un.iocb_r.iocb[7],
           krec->un.iocb_r.linkupevtag,
           krec->un.iocb_r.rscnevtag);
	}
	break;
      case KREC_TYPE_LATT:
	pprintf("%s LINKUP:%x  RSCN: %x\n", p_args3(latt_type[krec->un.latt_r.latt_type],
                     krec->un.latt_r.linkupevtag, krec->un.latt_r.rscnevtag) );
	break;
      case KREC_TYPE_MBX_R:
      case KREC_TYPE_MBX_W:
	for(j=0;j<8;j++){
           pprintf("%x ", krec->un.mbx_r.mbx[j] ); 
	};
	pprintf("\n");
	break;

#if BIG_RECORDING
      case KREC_TYPE_DYN_MALLOC:
      case KREC_TYPE_DYN_FREE:
          pprintf("l:%x (%x size:%d %s)\n", krec->un.dyn_r.leakq_adr,
                                   krec->un.dyn_r.adr, 
                                   krec->un.dyn_r.size,
                                   &krec->un.dyn_r.label );
	  break;

      case KREC_TYPE_PROBE_STATE:
          pprintf("DID: %x  from %s to %s \n",
                          krec->un.probe_r.did,
			  probetxt[krec->un.probe_r.oldstate],
			  probetxt[krec->un.probe_r.newstate] );
	break;
#endif

    }
  }

}

/*
 * Trace recording setup.   Either code can issue this or from the command
 * line.   When setting to enabled (=1), then entries number of records
 * will be allocated in a contiguous buffer.  When setting enabled off (=0)
 * then the buffer is freed and no tracing will occur.   This buffer stays
 * around over driver starting and stopping.
 *
 * TODO: (optional) set up an option to allow mallocing from memzone
 *
 */

void kgpsa_record_iocb_setup( struct kgpsa_pb *pb, int enable, int entries )
{

  if (kgpsa_big_trace) {
     pb->iocb_trace_enable = 1;
     pb->iocb_trace_entries = 1000000;
     pb->iocb_trace_currentry = 0;
     pb->iocb_trace_cnt    = 0;
     pb->iocb_trace_start  = 0xa00000;
     goto EXIT;
  }

  if(enable)
    {
     pb->iocb_trace_enable = enable;
     pb->iocb_trace_entries = entries;
     pb->iocb_trace_currentry = 0;
     pb->iocb_trace_cnt    = 0;
     pb->iocb_trace_start  = malloc( entries*sizeof(struct kgpsa_record), 
                                                                 "iocbtrace" );
    }
  else
    {
     pb->iocb_trace_enable = enable;
     pb->iocb_trace_entries = 0;
     pb->iocb_trace_currentry = 0;
     pb->iocb_trace_cnt    = 0;
     free( pb->iocb_trace_start, "iocbtrace" );
    }
EXIT:
#if 0
   kgpsa_dump_iocb_trace_setup( pb );
#endif
   return;
}

void kgpsa_dump_iocb_trace_setup( struct kgpsa_pb *pb )
{

   pprintf("pb: %x \n", pb );
   pprintf("pb->iocb_trace_enable (%x) = %x \n", &pb->iocb_trace_enable, pb->iocb_trace_enable );
   pprintf("pb->iocb_trace_entries (%x) = %x\n", &pb->iocb_trace_entries, pb->iocb_trace_entries );
   pprintf("pb->iocb_trace_currentry (%x) = %x\n", &pb->iocb_trace_currentry, pb->iocb_trace_currentry );
   pprintf("pb->iocb_trace_cnt (%x) = %x \n", &pb->iocb_trace_cnt, pb->iocb_trace_cnt );
   pprintf("pb->iocb_trace_start (%x) = %x\n", &pb->iocb_trace_start, pb->iocb_trace_start );
   pprintf("iocb trace buffer size = %d (dec) bytes\n", 
                        pb->iocb_trace_entries*sizeof(struct kgpsa_record)  );
}

void kgdumptrace( struct kgpsa_pb *pb)
{

 kgpsa_dump_iocb_trace( pb, 0, pb->iocb_trace_currentry, 1);

}

#endif




/*
 * IO primative tracing
 */


#define READ_CSR    1
#define WRITE_CSR   2
#define READ_SLIM   3
#define WRITE_SLIM  4
struct kgpsa_slim_trace_rec {
    int type;	
    int	entrynum;
    int offset;
    int data;
    int repcnt;
};


#if !RECORDING
#define kgpsa_slim_record( x, y, z, a )
#define kgpsa_dump_slim_trace( x, y, z )
#define kgpsa_record_slim_setup( x, y, z )
#define kgpsa_dump_slim_setup( x )


#else

void kgpsa_slim_record( struct kgpsa_pb *pb, int type, int offset, int data ) 
{
 struct kgpsa_slim_trace_rec *kstr = pb->slim_trace_start;
 if (!kgpsa_slim_record) return;

 if(pb==0) {
   pprintf("kgpsa_slim_record - zero_pb\n");
   do_bpt();
 }

 if (
     (kstr[pb->slim_trace_currentry].type   == type)   &&
     (kstr[pb->slim_trace_currentry].offset == offset) &&
     (kstr[pb->slim_trace_currentry].data   == data)   
    ) 
 {
   kstr[pb->slim_trace_currentry].repcnt++;
 }
 else 
 {
   pb->slim_trace_cnt++;
   pb->slim_trace_currentry = pb->slim_trace_cnt % pb->slim_trace_entries;
   kstr[pb->slim_trace_currentry].type     = type;
   kstr[pb->slim_trace_currentry].entrynum = pb->slim_trace_cnt;
   kstr[pb->slim_trace_currentry].offset   = offset;
   kstr[pb->slim_trace_currentry].data     = data;
   kstr[pb->slim_trace_currentry].repcnt   = 0;
 }
}

void kgpsa_dump_slim_trace( struct kgpsa_pb *pb, int index, int cnt )
{
  int i;
  int end;
  struct kgpsa_slim_trace_rec *kbase, *k;
  char *atype[] = { "0", "READ_CSR  ", 
                         "WRITE_CSR ", 
                         "READ_SLIM ", 
                         "WRITE_SLIM" };

  end = min( index+cnt, pb->slim_trace_cnt );
  kbase = pb->slim_trace_start;
  for ( i = index; i<end; i++ ) {
     k = &kbase[i];
     pprintf("%x (%x) %x %s %x %x ", i, k, k->entrynum, atype[k->type], 
                                                          k->offset, k->data); 
     if (k->repcnt ) {
        pprintf("\t reps: %x", k->repcnt );
     }
     pprintf("\n");
  }
}


/*
 * Trace recording setup.   Either code can issue this or from the command
 * line.   When setting to enabled (=1), then entries number of records
 * will be allocated in a contiguous buffer.  When setting enabled off (=0)
 * then the buffer is freed and no tracing will occur.   This buffer stays
 * around over driver starting and stopping.
 *
 * TODO: (optional) set up an option to allow mallocing from memzone
 *
 */

void kgpsa_record_slim_setup( struct kgpsa_pb *pb, int enable, int entries )
{
  if(enable)
    {
     pb->slim_trace_enable = enable;
     pb->slim_trace_entries = entries;
     pb->slim_trace_currentry = 0;
     pb->slim_trace_cnt    = 0;
     pb->slim_trace_start  = malloc( entries*sizeof(struct kgpsa_slim_trace_rec), 
                                                                 "slimtrace" );
    }
  else
    {
     pb->slim_trace_enable = enable;
     pb->slim_trace_entries = 0;
     pb->slim_trace_currentry = 0;
     pb->slim_trace_cnt    = 0;
     free( pb->slim_trace_start, "slimtrace" );
    }
#if DEBUG
   kgpsa_dump_slim_trace_setup( pb );
#endif
}

void kgpsa_dump_slim_trace_setup( struct kgpsa_pb *pb )
{

   pprintf("pb->slim_trace_enable (%x) = %x \n", &pb->slim_trace_enable, pb->slim_trace_enable );
   pprintf("pb->slim_trace_entries (%x) = %x\n", &pb->slim_trace_entries, pb->slim_trace_entries );
   pprintf("pb->slim_trace_currentry (%x) = %x\n", &pb->slim_trace_currentry, pb->slim_trace_currentry );
   pprintf("pb->slim_trace_cnt (%x) = %x \n", &pb->slim_trace_cnt, pb->slim_trace_cnt );
   pprintf("pb->slim_trace_start (%x) = %x\n", &pb->slim_trace_start, pb->slim_trace_start );
   pprintf("slim trace buffer size = %d (dec) bytes\n", 
                        pb->slim_trace_entries*sizeof(struct kgpsa_slim_trace_rec)  );
}
#endif






/*
 *********************************************************************
 *                                                                   *
 *  End of RECORDING routines                                        *
 *                                                                   *
 *********************************************************************
 */





/*
 *********************************************************************
 *                                                                   *
 *  MEMORY LEAK  routines                                            *
 *                                                                   *
 *********************************************************************
 */

struct QUEUE kgpsa_leak_q;

struct LEAKDATA {
    struct QUEUE q;
    int adr;
    int callerpc;
    char *label;
    int size;
};

#define MALLOC 1
#define FREE   2


#if !LEAK_WATCH

#define kgpsa_dyn_debug 0
#define kgpsa_leak_watcher(t, a, l, c, s )
#define kgpsa_dump_leak_queue()
#define kgpsa_dyn$_status( z, h ) 0 


#else

volatile int kgpsa_dyn_debug = 0;

/* TODO: (optional) NEED TO MAKE LEAK_LOCK a per/pb struct */
struct SEMAPHORE kgpsa_leak_lock;

void kgpsa_leak_watcher(struct kgpsa_pb *pb, int type, int adr, char *label, 
                                                       int callerpc, int size )
{
  int found;
  struct LEAKDATA *l;
  switch(type) {
    case MALLOC:
        dprintf("MALLOC ", p_args0);
        l = kgpsa_malloc( sizeof (struct LEAKDATA) );
	l->adr      = adr;
	l->callerpc = callerpc;
	l->label    = label;
	l->size     = size;
	krn$_wait( &kgpsa_leak_lock );
	insq( &l->q.flink, kgpsa_leak_q.blink );
        kgpsa_record_dyn( pb, KREC_TYPE_DYN_MALLOC, l, adr, label, size );
	krn$_post( &kgpsa_leak_lock );
	dprintf( "inserting %x onto leak watch queue |%s| size:%d\n", 
                                                p_args3( adr, label, size ) );
	break;
    case FREE:
        dprintf("FREE ", p_args0);

	found = 0;
	krn$_wait( &kgpsa_leak_lock );
	l = kgpsa_leak_q.flink;
	while (l != &kgpsa_leak_q.flink) {
	  if ( adr != l->adr ) {
  	    l = l->q.flink;
	  } else {
	    remq( l );
	    found = 1;
	    break;
	  }
	}
	if (found) {
	  dprintf("(%x) found element on leak queue |%s| size:%d dec\n", 
                                              p_args3(adr, label, l->size ) );
	  kgpsa_record_dyn( pb, KREC_TYPE_DYN_FREE, l, adr, l->label, l->size );
	  krn$_post( &kgpsa_leak_lock );
	  kgpsa_free( l );

	} else {
	  krn$_post( &kgpsa_leak_lock );
          pprintf("(%x) %s not found on allocation queue. pb: %x\n", 
                                                            adr, label, pb );
 	  do_bpt();
	  return;
	}
        break;
    default:
        pprintf("kgpsa_leak_watcher default\n");
	break;
  }
}


void kgpsa_dump_leak_queue()
{
  struct LEAKDATA *l;
  pprintf("kgpsa_dump_leak_queue\n");
  l = kgpsa_leak_q.flink;
  while (l != &kgpsa_leak_q.flink) {
    pprintf("(%x) found element on leak queue |%s| size:%d  callerpc:%x\n", 
                                    l->adr, l->label, l->size, l->callerpc );
    l = l->q.flink;
  }  
}





/*
 * Helper routine for DYNAMIC that prints out a usage summary for a zone
 */
int header=0;  
int kgpsa_dyn$_status (struct ZONE *zp, int *header) {
	int	i;
	struct DYNAMIC	*p;
        int	free_bytes, used_bytes;
	int	free_blocks, used_blocks;

	free_bytes = used_bytes = 0;
	free_blocks = used_blocks = 0;
	krn$_wait (&zp->sync);
	p = (void *) &zp->header.adj_fl;

	/*
	 * walk down the list
	 */
	do {
	    p = (void *) ((int) p - adjacent_bias);
	    if (p->bsize > 0) {
		free_bytes += p->bsize;
		free_blocks++;
	    } else {
		used_bytes += abs (p->bsize);
		used_blocks++;
	    }
	    p = p->adj_fl;
	    if (killpending ()) break;
	} while (p != (void *) &zp->header.adj_fl);
	krn$_post (&zp->sync);

#if 0
	if (*header == 0) {
printf ("%s%s%s",
"zone     zone       used    used       free    free       utili-  high\n",
"address  size       blocks  bytes      blocks  bytes      zation  water\n",
"-------- ---------- ------- ---------- ------- ---------- ------- ----------\n"
);
	}

	printf ("%08X %-10d %-7d %-10d %-7d %-10d %3d %%   %-10d\n",
	    zp, zp->size,
	    used_blocks, used_bytes,
	    free_blocks, free_bytes,
	    percentage (used_bytes, free_bytes + used_bytes),
	    zp->hiwater
	);

	*header = 1; 
#endif

    return used_bytes;
}



int lw_malloc( struct kgpsa_pb *pb, int x, char *label )
{
  int *buf;
  if (kgpsa_dyn_debug) dprintf("malloc: %d %s (pc:%x)\n",
                                          p_args3( x, label, callers_pc() ) );
#if 0
  buf = dyn$_malloc(x,DYN$K_SYNC|DYN$K_NOOWN);
#endif
  buf = kgpsa_malloc(x);
  memset ( buf, kgpsa_flood_byte, x );  /* flood with something bad to help find bugs */
  kgpsa_leak_watcher( pb, MALLOC, buf, label, callers_pc(), x ); 
  return buf;
}

void lw_free( struct kgpsa_pb *pb, int x, char *label )
{
  if (kgpsa_dyn_debug) dprintf("free: %d %s (pc:%x)\n", 
                                            p_args3(x, label, callers_pc() ) );
  kgpsa_leak_watcher( pb, FREE, x, label, callers_pc(), 0 );
#if 0
  dyn$_free(x,DYN$K_SYNC|DYN$K_NOOWN);
#endif
  kgpsa_free(x);
}

#endif


/*
 *********************************************************************
 *                                                                   *
 *  End of MEMORY LEAK routines                                      *
 *                                                                   *
 *********************************************************************
 */



/*
 *********************************************************************
 *                                                                   *
 *  SEMAPHORE routines                                               *
 *                                                                   *
 *********************************************************************
 */

/*
 *  Below are routines that are done in a long form.  These routines 
 *  are to help in debug, easy to break point at.  For instance, if 
 *  breakpointing on krn$_wait, we would hit everybodies krn$_wait.
 *  This way we can easily pick off our own.   This feature is most useful
 *  the startshut model when shutting down.
 */

#define SEMAPHORE_DEBUG 0
#if !SEMAPHORE_DEBUG


#define kgpsa_krn$_wait( s, p, l)  krn$_wait( s )
#define kgpsa_krn$_post( s, p, l)  krn$_post( s, 0 )
#define kgpsa_krn$_bpost( s, p, l) krn$_bpost( s, 0 )
#define kgpsa_krn$_post_wait( s1, s2, p, l)  krn$_post_wait( s1, s2 )

#else

volatile int kgpsa_semaphore_debug = 0;
volatile struct SEMAPHORE *kgpsa_semaphore_filter;

#if KGPSA_DEBUG
volatile int fb_wait_name;
volatile int fb_wait_callerspc;
volatile char fb_label[50];
volatile int fa_wait_name;
volatile int fa_wait_callerspc;
volatile char fa_label[50];
#endif

int kgpsa_krn$_wait(struct SEMAPHORE *s, int pbx, char *label)
{
 int retval;
 struct kgpsa_pb *pb = kgpsa_pbs[pbx];
 struct PCB *pcb = getpcb();
 if (kgpsa_semaphore_debug && (kgpsa_semaphore_filter==s) ) {
   pprintf("%s semaphore:%x process:%s WAIT %s  callers_pc:%x\n",
              p_args5( s->name, s, pcb->pcb$b_name, label, callers_pc() ) );
 }

#if KGPSA_DEBUG
 if( strcmp( "fc_ring", s->name ) == 0 )
   {
    fb_wait_name = pcb->pcb$b_name;
    fb_wait_callerspc = callers_pc();
    strcpy( &fb_label, label );
   }
#endif

 retval = krn$_wait( s );

#if KGPSA_DEBUG
 if( strcmp( "fc_ring", s->name ) == 0 )
   {
    fa_wait_name = pcb->pcb$b_name;
    fa_wait_callerspc = callers_pc();
    strcpy( &fa_label, label );
   }
#endif
 if (kgpsa_semaphore_debug && (kgpsa_semaphore_filter==s) ) {
   pprintf("%s WAIT satified: %s for %s\n", p_args3(pcb->pcb$b_name,label,s->name) );
 }

 return retval;
}


void kgpsa_krn$_post(struct SEMAPHORE *s, int pbx, char *label)
{
 struct kgpsa_pb *pb = kgpsa_pbs[pbx];
 struct PCB *pcb = getpcb();
 if (kgpsa_semaphore_debug && (kgpsa_semaphore_filter==s) ) {
   pprintf("%s semaphore:%x process:%s POST %s  callers_pc:%x\n",
              p_args5( s->name, s, pcb->pcb$b_name, label, callers_pc() ) );
 }
 krn$_post( s, 0 );
}


void kgpsa_krn$_bpost(struct SEMAPHORE *s, int pbx, char *label)
{
 struct kgpsa_pb *pb = kgpsa_pbs[pbx];
 struct PCB *pcb = getpcb();
 if (kgpsa_semaphore_debug && (kgpsa_semaphore_filter==s) ) {
   pprintf("%s semaphore:%x process:%s BPOST %s  callers_pc:%x\n",
              p_args5( s->name, s, pcb->pcb$b_name, label, callers_pc() ) );
 }
 krn$_bpost( s , 0 );
}


void kgpsa_krn$_post_wait(struct SEMAPHORE *s1, struct SEMAPHORE *s2,
                                                       int pbx, char *label)
{
 struct kgpsa_pb *pb = kgpsa_pbs[pbx];
 struct PCB *pcb = getpcb();
 if (kgpsa_semaphore_debug && 
    ( (kgpsa_semaphore_filter==s1) || (kgpsa_semaphore_filter==s2) ) ) {
   pprintf("%s|%s semaphore:%x|%x process:%s POSTWAIT %s  callers_pc:%x\n",
    p_args7( s1->name, s1, s2->name, s2, pcb->pcb$b_name, label, callers_pc() ) );
 }
 krn$_post_wait( s1, s2 );
}


#endif

/*
 *********************************************************************
 *                                                                   *
 *  End of SEMAPHORE routines                                        *
 *                                                                   *
 *********************************************************************
 */




/*
 *********************************************************************
 *                                                                   *
 *  Other DEBUG assist routines                                      *
 *                                                                   *
 *********************************************************************
 *
 *  set_rpi
 *  set_incarnation
 *  kgpsa_set_nstate
 *  kgpsa_set_prli_state
 *  kgpsa_trace_iocb
 *  kgpsa_dump_kgpsa_sb
 *  kgpsa_dump_nodes
 *  dn
 *  
 *
 */

/*
 *  This routine is DEBUG capable.  RPI management was an area of work
 *  requiring this debug feature.  But to save space, it is not enabled.
 */
#if !KGPSA_DEBUG

#define set_rpi( x, y )   x->rpi = y

#else 

void set_rpi( struct kgpsa_sb *sb , int rpi )
{
  dprintf("Setting sb:%x rpi from %x to %x  (did:%x) \n", 
                                       p_args4(sb, sb->rpi, rpi, sb->did) );
  sb->rpi = rpi;
}

#endif

#if !KGPSA_DEBUG

#define set_incarnation( x, y )   x->LINKUPeventTag = y->LINKUPeventTag; \
                                  x->RSCNeventTag   = y->RSCNeventTag;

#else 

void set_incarnation( struct kgpsa_sb *sb, IOCB_HDR *iocbhdr )
{
  dprintf("set_incarnation sb:%x LINKUP from %x to %x"
          " RSCN from %x to %x  (did:%x/rpi:%x) \n", 
     p_args7(sb, sb->LINKUPeventTag, iocbhdr->LINKUPeventTag, 
             sb->RSCNeventTag, iocbhdr->RSCNeventTag,
             sb->did, sb->rpi) );
  sb->LINKUPeventTag = iocbhdr->LINKUPeventTag;
  sb->RSCNeventTag = iocbhdr->RSCNeventTag;
}

#endif


#if !KGPSA_DEBUG

#define kgpsa_set_nstate( x, y )  x->nstate = y

#else

void kgpsa_set_nstate( struct kgpsa_sb *sb, int val )
{
 if (1)
   dprintf("Setting did: %x from %s to %s\n", 
              p_args3(sb->did, nstate_text[sb->nstate], nstate_text[val] ) );
 sb->nstate = val;

 if (kgpsa_debug_flag&1) 
   kgpsa_dump_kgpsa_sb( sb, 1, DO_IT );
}

#endif


#if !KGPSA_DEBUG

#define kgpsa_set_prli_state( x, y )  x->prli_state = y

#else

void kgpsa_set_prli_state( struct kgpsa_sb *sb, int val )
{
 if (kgpsa_debug_flag&1)
   dprintf("Setting did: %x from %s to %s\n", 
     p_args3(sb->did, prli_state_text[sb->prli_state], prli_state_text[val]) );
 sb->prli_state = val;
}

#endif


#if !KGPSA_DEBUG

#define kgpsa_trace_iocb( x, y, z, a )

#else

void kgpsa_trace_iocb( char *label, int offset, int *i_iocb, int do_it )
{
 int j;
 char trace[132];
 if (!do_it) return;
 if ( (kgpsa_trace_filter == 0) || (i_iocb[5] == kgpsa_trace_filter) ) {
   sprintf(trace,"%s %x %x %x %x %x %x %x %x %x\n",
      label, offset, i_iocb[0], i_iocb[1], i_iocb[2], i_iocb[3],
                           i_iocb[4], i_iocb[5], i_iocb[6], i_iocb[7]  );
   pprintf( "%s", trace );
 }
}

#endif


#if KGPSA_DEBUG

void dump_scsi_targets( struct kgpsa_pb *pb)
{
int i, aidx;
pprintf("scsi_targets_cnt: %d\n", pb->scsi_targets_cnt );
for(i=0; i<pb->scsi_targets_cnt; i++) {
  aidx = alpaidx(pb,pb->scsi_targets[i]);
  pprintf("scsi_targets[%d] did:%x  alpaidx:%d %s\n", i, pb->scsi_targets[i], 
               aidx, nprobe_state_text[pb->nprobe_state[aidx]] );
}
}

#endif


void kgpsa_dump_kgpsa_sb ( struct kgpsa_sb *sb, int do_header, int doit)
{
 int i,j,offset;
 unsigned int x[8];
 struct kgpsa_pb *pb;
 int probing;
 pb = sb->pb;


 if (do_header)
      pprintf(
"  sb       portname           al_pa rpi    nstate/prli  valid  probe  LINKUP/RSCN\n");

 probing = pb->nprobe_state[alpaidx(pb,sb->did)];

 pprintf( "%6x %s      %3x  %3x  %8s/%7s %3x    %2.2x     %3.3x/%3.3x\n",
	  sb,
	  sb->portname,
	  sb->did,
	  sb->rpi,
	  nstate_text[sb->nstate],
	  prli_state_text[sb->prli_state],
	  sb->valid,
          probing,
          sb->LINKUPeventTag, sb->RSCNeventTag );

 return;
}


void kgpsa_dump_nodes( struct kgpsa_pb *pb, int do_it)
{
  int i;
  struct kgpsa_sb *sb;
  pprintf("Node count: %x   pb: %x devState:%s  LINKUP: %x  RSCN: %x\n", 
        pb->node_count, pb, devnstate_text[pb->devState], pb->LINKUPeventTag,
                                                             pb->RSCNeventTag );
  for(i = 0; i < pb->node_count; i++){
    sb = pb->pb.sb[i];
    if ( !sb ) {
      pprintf("ZERO sb!!!!!  Should not happen. index:%x\n", i );
    }
    kgpsa_dump_kgpsa_sb( sb, (i==0), do_it );  /* do_header first time */
  }
}


void dn( struct kgpsa_pb *pb)
{
 kgpsa_dump_nodes( pb, 1 );
}


/*
 *********************************************************************
 *                                                                   *
 *  End of Other DEBUG assist routines                               *
 *                                                                   *
 *********************************************************************
 */






/*
 ************************************************************************
 *                                                                      *
 *                Start of non-debug code                               *
 *                                                                      *
 ************************************************************************
 */



/*+
 * ============================================================================
 * = kgpsa_prefix - derive the string for this adapter                        =
 * ============================================================================
 *
-*/
int kgpsa_prefix( struct kgpsa_pb *pb, char *name, char *string )
 {
   sprintf( name, "pg%c0%s", pb->pb.controller + 'a', string );
   return( name );
 }



/*+
 * ============================================================================
 * = wwn_sprintf - format a WWN into the common print format                  =
 * ============================================================================
 *
-*/
void wwn_sprintf( unsigned char *s, NAME_TYPE *nt )
{
 int i;
 unsigned char *p = nt;
 static char digits[] = "0123456789abcdef";

    for (i = 0; i < 8; i++) {
        *s++ = digits[*p >> 4];
        *s++ = digits[*p++ & 0xf];
        if( !((i+1)%2) ) *s++ = '-';
    }
    *--s = 0;


}



/*+
 * ============================================================================
 * = alpaidx - find the alpa or did in the transient list                     =
 * ============================================================================
 * 
 * A list of targets is used which changes from probe to probe.  This list
 * is temporary, and rebuilt each time the Name Server supplies its list
 * of targets.  The handle used by the console is the did or alpa.  
 * The mapping of a did/alpa to the target list is done with alpaidx.
 *
 * In loop mode, for example, this list will be equivalent to the
 * StaticAlpaArray data.  Due to the need for running disparity, the alpa
 * handles are not contiguous.  We only keep an array of 126, when their
 * values range from 1 to 255. 
 *
-*/
int alpaidx( struct kgpsa_pb *pb, int alpa )
{
 int i = -1;
 for ( i = 0; i<pb->scsi_targets_cnt; i++ ) {
   if (pb->scsi_targets[i] == alpa) return i;
 }
}



/*+
 * ============================================================================
 * = subtract - entries in one list are removed from another list             =
 * ============================================================================
 * 
-*/
void subtract( int *l1, int l1cnt, int *l2, int l2cnt, int *dstlst, int *dstcnt )
{
  int i, j;
  int max;
  int newcnt;

  max = *dstcnt;
  newcnt = l1cnt;
  memcpy( dstlst, l1, l1cnt*sizeof(int) );

  /*
   * Remove l2 entries from l1
   */
  for(j=0; j<l2cnt; j++)
    {
     for(i=0; i<newcnt; i++)
       {
        if( dstlst[i] == l2[j] )
          {
           dstlst[i] = dstlst[--newcnt];
           break;
          }
       }
    }

  *dstcnt = newcnt;
}




/*+
 * ============================================================================
 * = kgpsa_malloc - special error resistant malloc rtn                        =
 * ============================================================================
 *
 * To help avoid those cases where console runs out of heap in big configs,
 * this code is a feeble attempt to try and not sysfault when there is
 * no heap left.
 *
 * The policy this driver uses is:
 *
 *         - Don't take any heap if < 10000 left
 *         - retry in 1 second for 3 retries
 * 
 * Unfortunately, the rest of the driver does not check for malloc failures
 * so the driver is likely to fail in some catastrophic way anyhow.  But you
 * can see the potential to do better if the problem needs to be solved in
 * the future.   There is no heap space problem at this time (3-Nov-98).
 *
-*/
int kgpsa_malloc( int x )
{
  int *buf = 0;
  int retry = 3;
  struct ZONE	*zp = &defzone;
  int heapleft;
  
  /*
   * Skip the wait/post of zp->sync.  Shouldn't be that critical
   */
  heapleft = zp->size - zp->allocated;
  while( (heapleft < 10000) && (retry > 0) ) {
    pprintf("heapleft: %d  (retry:%x) \n", heapleft, retry );
    retry--;
    krn$_sleep(1000);
    heapleft = zp->size - zp->allocated;
  }
  if ( retry == 0 ) {
    pprintf("kgpsa_malloc: allocation failure of %d (retry:%x)\n", x, retry );
    return 0; 
  }
  
  retry = 3;
  while ( (buf==0) && (retry > 0 ) ) {
    buf = dyn$_malloc(x,DYN$K_SYNC|DYN$K_NOOWN|DYN$K_NOWAIT);
    if ( buf == 0 ) {
      pprintf("kgpsa_malloc: allocation failure of %d (retry:%x)\n", x, retry );
      retry--;
      krn$_sleep(1000);
    }
  }
  return buf;
}

void kgpsa_free( int x )
{
  dyn$_free(x,DYN$K_SYNC|DYN$K_NOOWN);
}


/***************************************************************************** 

General discussion on the driver: 
-----------------------------------
This section will have general discussion of things that go on in this driver.
It is a dumping ground for thoughts and description of how things were done.

The port driver code is logically partitioned into the major ways to       
command the adapter.  There is an MBX, an ELS, and an FCP section.         
ELSes and FCPs both use the IOCB communication mechanism.  MBXes are       
used in adapter setup and related to link initializations.                
                                                                           
There are 2 processes created.  One (kgpsa_fc_process) handles the adapter 
responses, adapter interrupts, and periodic checking. The other is the 
poll process, kgpsa_poll_process, which gets posted to do scsi_inquiries 
on the sb's. 

With the exception of shutdown, all ELSes are sent and received by the 
fc_process.  FCPs are issued in the context of user processes that are
reading and writing disks, the poll process which is issueing scsi inquiries,
and in one situation by the poll process which is issueing a request to the 
name server.  

FCPs, are serialized the fcp_owner_s semaphore.   

ELSes can be issued concurrently.  Access to data structures, like the sb,
are synchronized by the els_owner_s semaphore.  

The adapter supports up to 4 iocb rings.  After working with the adapter,
we have come to realize that there is not much benefit to these separate
rings.  Its almost a "what's the point."  But this code is implemented so it
uses 2 of the 4 rings since the code was originally written that way after 
being borrowed from the Emulex NT mini port driver.  The attribute of
separate rings which I do use is that buffers are allocated to rings.  I
allocate 256 byte buffers to the ELS ring and no buffers to the FCP ring.
However, when the FCP needs a buffer, it will notify the port driver which
can get it a buffer within the 2 second time out limit.   Receive buffer
notification is not set for the ELS ring.  If more than 10 ELS buffers are
consumed, the adapter will throw away ELSes.

The rings I use are ring 0 and ring 2.  This stems from the NT driver
that was referenced when designing this code.  Also in my discussion of 
rings throughout this code and comments, I often use ELS ring synonymously
with ring 0, and the FCP ring synonymous with ring 2.  This is not precisely
accurate.  The Name Server transactions are not FCP type, yet they are 
issued to ring 2 (resolving a buffer allocation issue).  Also, some iocb's
which are just adapter management iocb's, like aborts, can be issued on
any ring.

Although I talk about abort iocb's in this comment, they are never used by
this port driver.  They are a little tricky, they did not work in early revs
of adapter firmware, and I can facilitate what they do by using the mailbox
command UNREG_LOGIN.  The abort code lives in this port driver but is 
conditionally compiled out.

The port driver tracks the iocb's issued to the rings by putting an IOCB_HDR
structure on a queue for each outstanding iocb.   There are separate 
IOCB_HDR queues tracking the ELS ring and the FCP ring.  Separate rings
were done since the lookup into the queue is by iotag+Ring.  In order to
keep ELS iotag 25 distinct from FCP iotag 25, the live on separate queues.
Late in the development, it was discovered that the adapter does not separate
iotags in rings.  That is, if the port driver issues an abort on the ELS ring
for iotag 25, if that iotag is on the FCP ring, it would get aborted (and
vise versa).  So no matter what ring an abort is issued, it would work on 
an iotag on any ring.  When UNIX found this out, the decided it was pointless
to manage separate rings and in fact, they only have a 1 ring implementation.
In UNIX, the ring mask that filters packets is set to allow ELS, FCP, NAME
SERVER, all on the same ring.  I did not track this UNIX change in my driver
so separate rings still are used.  And it came in handy when allocating that
Name Server buffer (which I discuss below).

The XMIT_SEQ to the switch's NAME SERVER was sort of an after thought.  
It is a hybrid of sorts between an ELS and an FCP.  It has its own
receive and transmit masks.  But rings can have several masks of the types
of packets they will accept.  However, this driver puts this request
on the FCP iocb ring.  That is done because I need to allocate a larger
buffer to the ring.  The ELS ring only has 256 byte buffers currently, but
allocating a 4K buffer to it does not help because the adapter does not 
try to fit the data, instead it would fill up the 256 byte buffer and send
me a bunch of them.  I do not want to put in the code to concatenate 
continuation iocbs.   So therefore I decided I would issue the NAME SERVER
request on the FCP ring in the adapter where the name server buffer would
be the only buffer in around to use.  Since a NAME SERVER request is issued
from the fc_process, it is guarded by the els_owner_s semaphore.  

Now here is a little complexity and how it is managed by a little trick.
Because this drivers outstanding iocb's are stored in separate queues for
each ring, they are allowed to have iocb tags that are the same.  The adapter
does not care about the iotag (except when issued a command to abort a 
specific iotag which this driver currently (04-Nov-98) does not implement).
I am getting to what this is all about, hang in there reader. The iotag counter
for FCPs are incremented under fcp_owner_s by user processes, poll processes,
etc...  The ELS iotag counter is incremented under the els_owner_s semaphore.
But now we have a situation, with this NAME SERVER request, where the 
fc_process (until NAME SERVER it was strictly ELS) is putting an iocb
on the FCP ring (which runs iotag under fcp_owner_s).  I could either have
the fc_process take out the fcp_owner_s or have the fcp processes take out
the els_owner_s.  Or, Stephen's idea is to run the ELS iotags as even numbers
and the FCP iotags as odd numbers, and then I just issue the NAME SERVER
request with an ELS iotag and that guarantees no FCP would have the same tag.
Although elegant, I wonder if the maintainer of the code after I am dead and
gone would ever figure that one out.   

Back to that Name Server buffer for a minute.  I allocate it to the FCP ring
when notified from the adapter that it needs one.   The NAME SERVER data
comes back asynchronously and wants a large buffer.   Because of the 
asynchronous nature of this data, I was afraid of preallocating buffers
every time I issued a CT request because if a link or fabric transition
occurred, the CT data might not come back ever, and then my buffer would be
lost forever.   I saw this happening on rev 1.4b switch firmware.  The switch
was bouncing its ports.  I would issue CT's and the data would not come back.
The other way to manage this problem is to allocate the buffer to the 
exchange.  This is what UNIX and VMS do.   But the console is only implemented
with by_did or by_rpi iocb commands.   To add the iocb command set to work
by_xri would have added a bunch of code I did not want to qual nor to consume
valuable ROM space.   The code to operate the CT requests by_xri is present
in this driver, but conditionally compiled out.   What happens in the
by_xri mechanism, the buffer is allocated to an xri.  If a link transition
occurs, the exchange (xri) gets closed by the driver and frees the associated
resources.


There are only two places where the ELS and FCP originating processes
cross over.  That is, in this implementation a process that is generating 
ELSes does not issue anything to the FCP ring, and vice versa.  The ELS and
FCP functionality are different enough to keep this distinction - except
in the case of the XMIT_SEQ to the name server.  The XMIT_SEQ is called from
the kgpsa_els_response flow, and issues to the FCP/CT ring.  The name server
information comes back on the FCP/CT ring.

******************************************************************************/



/*
 *########################################################################
 *#########                    MBX                                ########
 *########################################################################
 */

/*
   The mailboxes the console needs are handled by kgpsa_mbx,
   kgpsa_reg_login, and kgpsa_unreg_login.  The kgpsa_reg_login
   and kgpsa_unreg_login are in their own routine since they
   frequently need a breakpoint during debug.  It is easier to
   set a breakpoint at a routine address then at a case of the
   switch statement.   And that is how the macros below came to
   be.  Each mailbox used to be a routine, but then during a code
   size reduction effort I got rid of the separate routines.
*/

#define kgpsa_part_slim( x, y )        kgpsa_mbx( x, y, MBX_PART_SLIM,   0 )
#define kgpsa_config_rings( x, y )     kgpsa_mbx( x, y, MBX_CONFIG_RING, 0 )
#define kgpsa_config_link( x, y )      kgpsa_mbx( x, y, MBX_CONFIG_LINK, 0 )
#define kgpsa_init_link( x, y, wait )  kgpsa_mbx( x, y, MBX_INIT_LINK,   wait )
#define kgpsa_read_la( x, y )          kgpsa_mbx( x, y, MBX_READ_LA,     0 )
#define kgpsa_read_sparm( x, y, buf )  kgpsa_mbx( x, y, MBX_READ_SPARM,  buf )
#define kgpsa_reset_ring( x, y, ring ) kgpsa_mbx( x, y, MBX_RESET_RING,  ring )
#define kgpsa_clear_la( x, y, etag )   kgpsa_mbx( x, y, MBX_CLEAR_LA,    etag )
#define kgpsa_down_link( x, y )        kgpsa_mbx( x, y, MBX_DOWN_LINK,   0 )
#define kgpsa_read_nv( x, y )          kgpsa_mbx( x, y, MBX_READ_NV,     0 )
#define kgpsa_dump_cfg_mbx( x, y )     kgpsa_mbx( x, y, MBX_DUMP_CFG,    0 )
#define kgpsa_update_cfg_mbx( x, y )   kgpsa_mbx( x, y, MBX_UPDATE_CFG,  0 )
#define kgpsa_load_sm( x, y )          kgpsa_mbx( x, y, MBX_LOAD_SM,     0 )

int kgpsa_mbx( struct kgpsa_pb *pb, MAILBOX *mbx, int mbxtype, int arg )
{

 ULONG status;
 FC_BRD_INFO *binfo;
 RING *rp;
 int i, j, ring;
 binfo = &pb->binfo;

 /*
  * Clear out the mailbox buffer except in the case of the NV Write.
  * For all except the UPDATE_CFG operation, all work is done in this routine.
  * But the UPDATE_CFG passes in data in the mailbox buffer provided.
  */
 if (mbxtype != MBX_UPDATE_CFG )
   memset( mbx, 0, MAILBOX_CMD_BSIZE );

 mbx->mbxCommand = mbxtype;

 switch (mbxtype) {

   case MBX_PART_SLIM:
     mbx->un.varSlim.numRing = binfo->fc_ffnumrings;
     for (i = 0; i < binfo->fc_ffnumrings; i++)
       {
        rp = &binfo->fc_ring[i];
        /* Setup number of cmd and rsp iocb entries for each ring */
        mbx->un.varSlim.ringdef[i].numCiocb = rp->fc_numCiocb;
        mbx->un.varSlim.ringdef[i].numRiocb = rp->fc_numRiocb;
       }
     status = kgpsa_issue_mbx(pb,mbx);
     break;

   case MBX_CONFIG_RING:
     for (ring = 0; ring < binfo->fc_ffnumrings; ring++) {
       mbx->un.varCfgRing.ring = ring;
       mbx->un.varCfgRing.profile = 0;
       mbx->un.varCfgRing.maxOrigXchg = 0;
       mbx->un.varCfgRing.maxRespXchg = 0;

     /*
      *  We do not use recvNotify for ELSes.  This functionality would notify the host
      *  when the adapter needed a buffer.  I used to have this on and then
      *  in kgpsa_els_response, I would allocate a buffer for it when it
      *  wanted one.  But the adapter can get greedy and then the host would
      *  have to come up with a policy for managing these buffers.  If
      *  we do not have this notification, then the adapter will give up
      *  right away and ABTS the incoming iocb.  Initially I give the adapter
      *  some preallocated rcv buffers, and that is all its gonna get.
      */
      mbx->un.varCfgRing.recvNotify = 0;  /* Notify me if I am out of buffers */
    
      /*
       *  This is used when CT data comes in.  Instead of allocating a buffer
       *  for the request, we wait until the response needs the buffer.  This
       *  helps keep down the lost buffers that preallocation would have.
       *  Prior to this I was allocating a buffer with the GP_ID3 command.
       *  But the data comes back asynchronous to the command, and if it got
       *  aborted somehow (as seen in the switchEnable test), then the buffer
       *  would remain in the adapter.
       */
      if(ring==FC_FCP_RING)
         mbx->un.varCfgRing.recvNotify = 1;  /* Notify me if I am out of buffers */


      mbx->un.varCfgRing.numMask = binfo->fc_nummask[ring];

      j = 0;
      for (i = 0; i < ring; i++) j += binfo->fc_nummask[i];
      for (i = 0; i < binfo->fc_nummask[ring]; i++) {
        mbx->un.varCfgRing.rrRegs[i].rval = binfo->fc_rval[j + i];
        mbx->un.varCfgRing.rrRegs[i].rmask = 0xff;
        mbx->un.varCfgRing.rrRegs[i].tval = binfo->fc_tval[j + i];
        mbx->un.varCfgRing.rrRegs[i].tmask = 0xff;
      }
   
      status = kgpsa_issue_mbx( pb, mbx );
      if (status != MBX_SUCCESS) {
        dprintf("config_ring failure\n",p_args0);
        break;
      } else {
      }
     } /* for rings */

     break;

   case MBX_CONFIG_LINK:
     mbx->un.varCfgLnk.myId = binfo->fc_myDID;
     if(kgpsa_edtov_time) binfo->fc_edtov = kgpsa_edtov_time;
     mbx->un.varCfgLnk.edtov = binfo->fc_edtov;
     mbx->un.varCfgLnk.arbtov = binfo->fc_arbtov;
     if(kgpsa_ratov_time) binfo->fc_ratov = kgpsa_ratov_time;
     mbx->un.varCfgLnk.ratov = binfo->fc_ratov;
     mbx->un.varCfgLnk.rttov = binfo->fc_rttov;
     mbx->un.varCfgLnk.altov = binfo->fc_altov;
     mbx->un.varCfgLnk.crtov = binfo->fc_crtov;
     mbx->un.varCfgLnk.citov = binfo->fc_citov;
     status = kgpsa_issue_mbx( pb, mbx );
     break;



   case MBX_INIT_LINK:
#if KGPSA_DEBUG
     if(kgpsa_topology == -1)
       {
         if (pb->topology == FLAGS_TOPOLOGY_MODE_LOOP) 
           {
            pprintf("%s TOPOLOGY: LOOP (pb:%x) [pb->topology]\n", pb->pb.name, pb );
           }
         else if (pb->topology == FLAGS_TOPOLOGY_MODE_PT_PT) 
           {
            pprintf("%s TOPOLOGY: PT_PT (for Switch) (pb:%x) [pb->topology]\n", pb->pb.name, pb );
           }
       }
     else 
       {
        if(kgpsa_topology==FLAGS_TOPOLOGY_MODE_LOOP) 
         {
          pprintf("%s TOPOLOGY: LOOP (pb:%x) [kgpsa_topology]\n", pb->pb.name, pb );
         } 
        else if (kgpsa_topology == FLAGS_TOPOLOGY_MODE_PT_PT) 
         {
          pprintf("%s TOPOLOGY: PT_PT (for Switch) (pb:%x) [kgpsa_topology]\n", pb->pb.name, pb );
         }
       }
#endif
     if (kgpsa_topology == -1 ) {
       mbx->un.varInitLnk.link_flags = pb->topology;
     } else {
       mbx->un.varInitLnk.link_flags = kgpsa_topology;
     }

#if LOOP_NOT_SUPPORTED
     if( mbx->un.varInitLnk.link_flags == FLAGS_TOPOLOGY_MODE_LOOP )
        {
          return msg_success;
        }
#endif

     status = kgpsa_issue_mbx( pb, mbx );
     /*
      * If WAIT is requested, we come here to wait for the link
      * event to happen before going on.  This should
      * only be requested when interrupts are disabled.  When interrupts are
      * enabled, we compete with the ISR which will try also to clear HA_LATT.
      *
      * If a more general solution should ever be required, one that needs 
      * to wait in interrupt mode, then this could check the link event 
      * number instead.
      */
     /*
      * TODO: (medium) (7-Nov-98 - soak time needed)
      *    If nobody is calling this routine with WAIT, then this code can
      *    be deleted.
      */
     if( arg == WAIT )
     {
        int status = msg_success;
        struct TIMERQ *tq, tqe;
        tq = &tqe;
        start_timeout(kgpsa_latt_timer,tq);
        while ( ( (rl(HA) & HA_LATT) == 0) && (tq->sem.count==0)  ) {
          krn$_sleep(10);
        }
        if (tq->sem.count) {
          status = msg_failure;
          dprintf("LATT timeout\n",p_args0);
        }
        stop_timeout(tq);
        /* clear LATT */
        if (status==msg_success){
          dprintf("wait_for_latt found Link Attention:%x\n", p_args1( rl(HA) ) );
          wl(HA,HA_LATT);
        }
     } /* if arg == WAIT */
     break;

   case MBX_READ_LA:
     {
     mbx->un.varReadLA.lilpBde.bdeAddress = (int)binfo->alpa_map | $MAP_FACTOR( pb );
     if (kgpsa_test_zeroalpa) {
       mbx->un.varReadLA.lilpBde.bdeSize = 0;
     } else {
       mbx->un.varReadLA.lilpBde.bdeSize = 128;
     }
     memset( binfo->alpa_map, 0, mbx->un.varReadLA.lilpBde.bdeSize );
     status = kgpsa_issue_mbx( pb, mbx );
     dprintf("kgpsa_read_la - eventTag:%x\n", mbx->un.varReadLA.eventTag );
     kgpsa_dump_blk( "ALPA_MAP:", binfo->alpa_map, kgpsa_debug_flag&1 );
     break;
     }

   case MBX_READ_SPARM:
     mbx->un.varRdSparm.sp.bdeAddress = arg | $MAP_FACTOR( pb );
     mbx->un.varRdSparm.sp.bdeSize = sizeof( SERV_PARM );
     status = kgpsa_issue_mbx( pb, mbx );
     kgpsa_dump_sparm( arg, kgpsa_debug_flag&1 );
     break;

   case MBX_RESET_RING:
     mbx->un.varRstRing.ring_no = arg;
     status = kgpsa_issue_mbx( pb, mbx );
     rp = &binfo->fc_ring[arg];
     rp->fc_cmdidx = 0;
     rp->fc_rspidx = 0;
     break;

   case MBX_CLEAR_LA:
     mbx->un.varClearLA.eventTag = arg;
     dprintf("Clearing LA event %x  (current IPL:%x)\n",
                                  p_args2( arg, mfpr_ipl() ) );
     status = kgpsa_issue_mbx( pb, mbx );
     dprintf("clear_la status:%x\n", p_args1( mbx->mbxStatus ) );
     break;

   case MBX_DOWN_LINK:
   case MBX_READ_NV:
     status = kgpsa_issue_mbx( pb, mbx ); 
     break;

   case MBX_DUMP_CFG:
     mbx->un.varWords[0] = 2;  /* type = CONFIGURATION DATA */
     mbx->un.varWords[1] = KGPSA_NVRAM_REGION;  /* offset=0,region=3 */
     mbx->un.varWords[2] = 0;
     mbx->un.varWords[3] = sizeof(struct kgpsa_nvram_region)/4; /* Word count */
     mbx->un.varWords[4] = 0;
     status = kgpsa_issue_mbx( pb, mbx ); 
     break;

   case MBX_UPDATE_CFG:
     mbx->un.varWords[0] = 2;  /* type = CONFIGURATION DATA */
     mbx->un.varWords[1] = (sizeof(struct kgpsa_nvram_region) << 16) |
                            KGPSA_NVRAM_REGION; /* entry len in bytes, region */
     mbx->un.varWords[2] = 0;  /* R: response info */
     mbx->un.varWords[3] = sizeof(struct kgpsa_nvram_region);  /* byte count */
     status = kgpsa_issue_mbx( pb, mbx ); 
     break;

   case MBX_LOAD_SM:
     mbx->un.varWords[0] = 0x0000000A;  /* from Krueger email */
     mbx->un.varWords[1] = KGPSA_REGION3_BASE;  /* region 3 first word address */
     mbx->un.varWords[2] = 4;  /* length in bytes */
     mbx->un.varWords[3] = 0x14;  /* mbx offset of data (pts to WORD5 below) */
     mbx->un.varWords[4] = 0x00000000;  /* data to write */
     status = kgpsa_issue_mbx( pb, mbx ); 
     break;

   default:
     break;

 } /* switch */

 return status;
}



/*+
 * ============================================================================
 * = kgpsa_reg_login - Login                                                  =
 * ============================================================================
 *
-*/
int kgpsa_reg_login( struct kgpsa_sb *sb, MAILBOX *mbx,  int d_id )
{
 int status;
 struct kgpsa_pb *pb;
#if KGPSA_DEBUG
 if ( (kgpsa_trace_filter == 0) || (kgpsa_trace_filter == d_id) )
    dprintf("reg_login of did: %x", p_args1(d_id) );
#endif
 dprintf("reg_login of did: %x", p_args1(d_id) );
 pb = sb->pb;
 memset(mbx, 0, MAILBOX_CMD_BSIZE );
 mbx->un.varRegLogin.rpi = 0;
 mbx->un.varRegLogin.did = d_id;
 mbx->un.varRegLogin.sp.bdeSize = sizeof(SERV_PARM);
 /* PLOGI resultant data goes into bde buffer */
 mbx->un.varRegLogin.sp.bdeAddress = (int)&sb->serv_parm | $MAP_FACTOR( pb );
 mbx->mbxCommand = MBX_REG_LOGIN;
 status = kgpsa_issue_mbx(pb,mbx);
 dprintf(" as rpi: %x \n", p_args1(mbx->un.varRegLogin.rpi) );
 return status;
}



/*+
 * ============================================================================
 * = kgpsa_unreg_login - UNREG Login                                          =
 * ============================================================================
 *
-*/
int kgpsa_unreg_login( struct kgpsa_pb *pb, int rpi )
{
 MAILBOX lmb, *mbx = &lmb;
 int status;

 dprintf("unreg_login of rpi: %x \n", p_args1(rpi) );

 memset( mbx, 0, MAILBOX_CMD_BSIZE );
 mbx->un.varRegLogin.rpi = rpi;
 mbx->mbxCommand = MBX_UNREG_LOGIN;
 status = kgpsa_issue_mbx(pb,mbx);
 return status;
}




#if KGPSA_DEBUG
/*+
 * ============================================================================
 * = kgpsa_read_rev - read rev info                                           =
 * ============================================================================
 *
 * TODO: (optional)
 * Spice this up and make it a real routine.  Put results in the
 * event log.
 *
-*/
void kgpsa_read_rev( struct kgpsa_pb *pb)
{
 int i;
 MAILBOX lmb, *mbx=&lmb;
 memset( mbx, 0, MAILBOX_CMD_BSIZE );
 mbx->mbxCommand = MBX_READ_REV;
 kgpsa_issue_mbx(pb,mbx);
}
#endif


/*+
 * ============================================================================
 * = kgpsa_read_rpi - inquire the adapter about rpi's                         =
 * ============================================================================
 *
 *  Given a did, inquire the adapter about the associated rpi.  In most cases
 *  the console knows the rpi from a reg_login, or in the iocb.  But there
 *  are some cases, like when dealing with a did that we chose not to reg_login,
 *  or that may have changed, where we need to ask the adapter about the
 *  did<->rpi mapping.
 *
-*/
int kgpsa_read_rpi( struct kgpsa_pb *pb, int did )
{
 int i;
 int rpi;
 MAILBOX lmb, *mb=&lmb;
 SERV_PARM sp;
 memset( mb, 0, MAILBOX_CMD_BSIZE );
 mb->mbxCommand = MBX_READ_RPI;
 mb->un.varRdRPI.reqRpi = 0xFFFF;
 mb->un.varRdRPI.sp.bdeAddress = (int)(&sp) | $MAP_FACTOR( pb );
 mb->un.varRdRPI.sp.bdeSize    = sizeof(SERV_PARM);
 while (mb->un.varRdRPI.nextRpi != 0xFFFF) {
   kgpsa_issue_mbx(pb,mb);
   if ( did )
   {
     if ( mb->un.varRdRPI.DID == did )
       {
       dprintf("Did %x found as rpi %x\n", p_args2(did, mb->un.varRdRPI.reqRpi) );
       return mb->un.varRdRPI.reqRpi;
       }
   }
   else
   {
     pprintf("read_rpi - rpi:%x  did:%x  next:%x\n", mb->un.varRdRPI.reqRpi, 
                             mb->un.varRdRPI.DID, mb->un.varRdRPI.nextRpi );
   }
   mb->un.varRdRPI.reqRpi = mb->un.varRdRPI.nextRpi;
 }
 return 0;
}




/*+
 * ============================================================================
 * = kgpsa_read_rconfig - read the ring configuration                         =
 * ============================================================================
 *
-*/
#if KGPSA_DEBUG
int kgpsa_read_rconfig( struct kgpsa_pb *pb, int ring, MAILBOX *mb)
{
 int status;
 memset( mb, 0, MAILBOX_CMD_BSIZE );
 mb->un.varRdRConfig.ring = ring;
 mb->mbxCommand = MBX_READ_RCONFIG;
 status = kgpsa_issue_mbx( pb, mb );
 if (status != MBX_SUCCESS) {
   dprintf("read_rconfig failure\n",p_args0);
 }
 return status;
}
#endif


/****************************************************************************
 * The routines:
 *
 *               kgpsa_dump_cfg
 *               kgpsa_update_cfg
 *               kgpsa_erase_nv
 *		 kgpsa_read_compaq_nvdata
 *               kgpsa_write_compaq_nvdata
 *               kgpsa_init_compaq_nvdata
 *
 * were added (summer 1999) to support the setting of the topology
 * into the KGPSA's non-volatile memory.
 *
 */

int kgpsa_dump_cfg( struct kgpsa_pb *pb, unsigned char *buf, int *buflen )
{
  int i,status;
  MAILBOX mbx, *mb=&mbx;
  unsigned int *ibuf = &mbx;
  kgpsa_dump_cfg_mbx( pb, mb );
#if 0
  printf("kgpsa_dump_cfg\n");
  for(i=0;i<31;i++)
    {
      printf("[%d] %08x \n", i, ibuf[i] );
    }
#endif
  if(mb->mbxStatus != MBX_SUCCESS)
    {
      printf("%s - Nvram read failed.\n", pb->pb.name );
      status = msg_failure;
    }
  else
    {
      memcpy( buf, &mbx.un.varDump.data, *buflen );
      status = msg_success;
    }
  *buflen = mbx.un.varDump.blen;
  return status;
}

int kgpsa_update_cfg( struct kgpsa_pb *pb, unsigned char *buf, int buflen )
{
  int i, status;
  MAILBOX mbx, *mb=&mbx;
  unsigned int *ibuf = &mbx;
  
  memset( mb, 0, sizeof(MAILBOX) );
  memcpy( &mbx.un.varUpdateCfg.data, buf, buflen );
  kgpsa_update_cfg_mbx( pb, mb );
#if 0
  printf("kgpsa_update_cfg:\n");
  for(i=0;i<31;i++)
    {
     if(i==3)
       printf("[%d] %08x (%d) \n", i, ibuf[i], ibuf[i] );
     else
       printf("[%d] %08x \n", i, ibuf[i] );
    }
#endif
  if(mb->mbxStatus != MBX_SUCCESS)
    {
      printf("%s - Nvram update failed.\n", pb->pb.name );
      status = msg_failure;
    }
  else
    {
      status = msg_success;
    }
  return status;
}

#if KGPSA_DEBUG
void kgpsa_erase_nv( struct kgpsa_pb *pb )
{
  MAILBOX mbx, *mb=&mbx;
  kgpsa_load_sm( pb, mb );
}
#endif



int kgpsa_read_compaq_nvdata( struct kgpsa_pb * pb, struct kgpsa_nvram_region *nvr )
{
  int i, status;
  int lwlen;
  int data_len;
  unsigned int chksum = 0;
  unsigned int *ibuf = nvr; 

  data_len =  sizeof(struct kgpsa_nvram_region);
  status = kgpsa_dump_cfg( pb, nvr, &data_len );
  if( status == msg_failure )
    {
     return msg_failure;
    }

#if 0
  printf("read_compaq_nvdata - vendor_id: %x\n", nvr->vendor_id );
  printf("                      version:   %x\n", nvr->version );
  printf("                      checksum: %x\n", nvr->checksum );
  printf("                      topology: %x\n", nvr->topology );
  printf("                      alpa: %x\n", nvr->alpa );
  printf("                      fill1: %x\n", nvr->fill1 );
  printf("                      fill2: %x\n", nvr->fill2 );
  printf("                      rsvd1: %x\n", nvr->rsvd1);
  printf("                      rsvd1: %x\n", nvr->rsvd2);
  printf("                      rsvd1: %x\n", nvr->rsvd3);
#endif


  /*
   * Validate the checksum
   */
  lwlen = sizeof(struct kgpsa_nvram_region) / sizeof( int );
  for( i=0; i < lwlen; i++ )
     {
      chksum += ibuf[i];  /* data starts at offset 5 in mbx */
     }
   if( (chksum != 0xFFFFFFFF) || (nvr->version != KGPSA_NVRAM_VERSION ) 
           || (data_len != sizeof(struct kgpsa_nvram_region) ) )
     {
     printf("%s - Nvram format incorrect.\n", pb->pb.name );
     return msg_failure;
     }
   else
     {
     return msg_success;
     }
}

int kgpsa_write_compaq_nvdata( struct kgpsa_pb *pb, struct kgpsa_nvram_region *nvr )
{
 int i, status;
 int lwlen;
 unsigned int *ibuf;
 unsigned int chksum = 0;
 lwlen = sizeof(struct kgpsa_nvram_region) / sizeof( int );
 /*
  * Formulate the checksum
  */
 ibuf = nvr;
 nvr->checksum = 0;
 for( i=0; i < lwlen; i++ )
   {
    chksum += ibuf[i];
   }
  nvr->checksum = ~chksum;
#if 0
  printf("write_compaq_nvdata - vendor_id: %x\n", nvr->vendor_id );
  printf("                      version:   %x\n", nvr->version );
  printf("                      checksum: %x\n", nvr->checksum );
  printf("                      topology: %x\n", nvr->topology );
  printf("                      alpa: %x\n", nvr->alpa );
  printf("                      fill1: %x\n", nvr->fill1 );
  printf("                      fill2: %x\n", nvr->fill2 );
  printf("                      rsvd1: %x\n", nvr->rsvd1);
  printf("                      rsvd1: %x\n", nvr->rsvd2);
  printf("                      rsvd1: %x\n", nvr->rsvd3);
#endif
  status = kgpsa_update_cfg( pb, nvr, sizeof(struct kgpsa_nvram_region) );
  if(status != msg_success)
     printf("%s - Nvram update failed.\n", pb->pb.name );
  return status;
}

int kgpsa_init_compaq_nvdata( struct kgpsa_pb *pb, struct kgpsa_nvram_region *nvr )
{
 int status;
 memset( nvr, 0, sizeof(struct kgpsa_nvram_region) );
 nvr->vendor_id = COMPAQ_VENDOR_ID;
 nvr->version =  KGPSA_NVRAM_VERSION;
 nvr->checksum = 0xa5a5a5a5;
 nvr->topology = FLAGS_TOPOLOGY_MODE_PT_PT;
 nvr->alpa     = 0x69;
 status = kgpsa_write_compaq_nvdata( pb, nvr );
 return status;
}


#if KGPSA_DEBUG
/*
 * Test routine to screw up the nvram.
 */
void kgpsa_test_compaq_nvdata( struct kgpsa_pb *pb, struct kgpsa_nvram_region *nvr )
{
 nvr->vendor_id = COMPAQ_VENDOR_ID;
 nvr->version =  KGPSA_NVRAM_VERSION+1;
 nvr->checksum = 0xa5a5a5a5;
 nvr->topology = FLAGS_TOPOLOGY_MODE_PT_PT;
 nvr->alpa     = 0x69;
 kgpsa_write_compaq_nvdata( pb, nvr );
}
#endif
 



#if KGPSA_DEBUG
/*+
 * ============================================================================
 * = kgpsa_nvram - read the nvram                                             =
 * ============================================================================
 * This command reads the non-volatile FireFly parameters (saved in the
 * FLASH).  The command must be issued before the FireFly Chipset is configured
 * (PART_SLIM and CONFIG_RING Mailbox commands), since the first 1028 bytes
 * of the SLIM is used to store parameter information.
 *
 * This routine leaves the adapter in an unintialized state.
 *
-*/
int kgpsa_nvram( struct kgpsa_pb *pb, unsigned char *buf )
{
 int i;
 int status;
 int *ibuf = buf;

 /*
  * Get the adapter into the required unconfigured state by
  * stopping the driver and then resetting the adapter.
  */
 kgpsa_setmode( pb, DDB$K_STOP );
 kgpsa_make_FF_ready(pb);
 /*
  *  Clear the mbxhc and owner bits so we don't get confused when
  *  trying to write the first mailbox;  It might have powered up
  *  to all F's
  */
 ws(0,0);

 kgpsa_set_devState( pb, DEV_UNINITIALIZED, "NVRAM" );

 status = kgpsa_read_nv( pb, buf );

 for(i=0; i<10; i++)
   pprintf("ibuf[%x]: %x\n", i, ibuf[i] );

 return status;
}
#endif



/****************************************************************************/


#if KGPSA_DEBUG
/*+
 * ============================================================================
 * = kgpsa_init_link_reset - Initialize the link with selective reset         =
 * ============================================================================
 *
 * This is just a handy routine that is callable from the command line.
 *
-*/
int kgpsa_init_link_reset( struct kgpsa_pb *pb, MAILBOX *mbx, int al_pa )
{
 kgpsa_init_link( pb, mbx, DONT_WAIT );
 return msg_success;
}
#endif


/*+
 * ============================================================================
 * = kgpsa_issue_mbx - issue a mailbox command                                =
 * ============================================================================
 *
 * This is the mailbox writing primative.  There are no mailbox commands
 * which take too long to wait for, so this routine runs in a spinlock
 * waiting for the mailbox to complete.
 *
 * Since the ISR, kgpsa_interrupt, needs to issue mailboxes and runs at 
 * high ipl, we cannot let this routine get interrupted.  The mbx_l spinlock
 * is taken out.
 * 
 * For the most part, this generic primative does not care which mailbox
 * command it is issuing.  But the RESTART is an oddball and we do special
 * code.  Also, the hardware operation CLEAR_LA and setting the port drivers 
 * state really need to be an atomic operation, and it is best done here
 * under the spinlock.
 *
-*/
int kgpsa_issue_mbx( struct kgpsa_pb *pb, MAILBOX *mbx)
{
 int status = msg_success;
 int i,word0;
 int *i_mbx = (int*)mbx;
 int mbxsize = MAILBOX_CMD_BSIZE / sizeof( int );

 spinlock(&pb->mbx_l);

 if ( ( rs(0) & (M_MBXOWNER | M_MBXHC) ) != 0) {
   pprintf("*** MBX not ready ***\n");
   status = msg_failure;
   goto EXIT;
 }
 ws(0,M_MBXHC);

 /*
  * If this is a READ_NV or a WRITE_NV then the mailbox is 1028 bytes.
  */
 if ( (mbx->mbxCommand == MBX_READ_NV) || (mbx->mbxCommand == MBX_WRITE_NV) )
   {
    pprintf("Mailbox size upped to 1028 bytes\n");
    mbxsize = NVPARMS_CMD_BSIZE / sizeof( int );
   } 

 /*
  * Write the mailbox to the SLIM, but do it in reverse order
  * so OWN gets written last.  There is always one trouble maker exception
  * to ugly up the code - MBX_RESTART.
  */
 if ( mbx->mbxCommand == MBX_RESTART ) {
   mbx->mbxOwner = OWN_HOST;                /* the one exception */
 } else {
   mbx->mbxOwner = OWN_CHIP;
 }
 mbx->mbxHc = 1; 
 for ( i = mbxsize-1; i >= 0; i-- ){
   ws( i*sizeof(int), i_mbx[i] );
 }
 kgpsa_record_mbx( pb, KREC_TYPE_MBX_W, i_mbx );

 /*
  * wait for completion
  */
 for ( i = 0; i < kgpsa_mbxtimeout; i++) {
   word0 = rs(0);
   if ( ( word0 & M_MBXOWNER ) == OWN_HOST) {
#if KGPSA_DEBUG
      int cmd = (word0 >> 8) & 0xFF;
      dprintf("mbx done : i=%d   word0:%x  %s\n",
                                        p_args3(i,word0,mbx_txt[cmd]) );
#endif
      break;
   } else {
      krn$_micro_delay(100);     /* Could be called from elevated ipl */
   }
 }
 if (i == kgpsa_mbxtimeout) {
    dprintf("mbx timeout\n",p_args0);
    status = msg_failure;
    goto EXIT;
 }


 /*
  * retrieve the results from the SLIM back into the local mbx
  */
 for ( i = mbxsize-1; i >= 0; i-- ){
   i_mbx[i] = rs( i*sizeof(int) );
 }
 kgpsa_record_mbx( pb, KREC_TYPE_MBX_R, i_mbx );

EXIT:
 /*
  * Give up the Host Control bit and allow other mailboxes in.
  * Except in the case of a MBX_RESTART because we have to hang on
  * to host control.
  */
 if ( mbx->mbxCommand != MBX_RESTART ) {
  word0 = rs(0);
  word0 &= ~M_MBXHC;
  ws(0,word0);
 }

/*
 *  If CLEAR_LA succeeded, then set_DevState here while we have the
 *  spinlock out.
 */

  if( status==msg_success )
    switch( mbx->mbxCommand )
      {
      case MBX_CLEAR_LA:
	 if( (word0>>16) != MBXERR_NOT_LAST_LINK_ATTN )
            kgpsa_set_devState( pb, DEV_READY, "CLEAR_LA" );
         else 
            status = msg_failure;
         break;
      default:
         break;
      }


 /*
  * Re-enable the Mailbox Attention interrupt;  
  */
  wl( HA, HA_MBATT );

 spinunlock(&pb->mbx_l);

 return status;

}




/*+
 * ============================================================================
 * = kgpsa_init - init the driver                                             =
 * ============================================================================
 *
-*/
#if !(STARTSHUT||DRIVERSHUT)
/*+
 * ============================================================================
 * = kgpsa_init -                                                             =
 * ============================================================================
 *
-*/
int kgpsa_init( int phase )
{
 int status;
 int i;
 int null_procedure();
 int kgpsa_init_pb( );


#if 0
 pprintf("size of MAILBOX: %d (decimal) \n",sizeof(MAILBOX) );
 pprintf("size of MAILBOX_CMD_BSIZE %d (decimal) \n", MAILBOX_CMD_BSIZE );
 pprintf("size of MAILVARIATNS: %d (decimal) \n", sizeof(MAILVARIANTS) );

 pprintf("size of LOAD_SM_VAR %d \n", sizeof(LOAD_SM_VAR) );
 pprintf("size of READ_NV_VAR %d \n", sizeof(READ_NV_VAR) );
 pprintf("size of WRITE_NV_VAR %d \n", sizeof(WRITE_NV_VAR) );
 pprintf("size of BIU_DIAG_VAR %d \n", sizeof(BIU_DIAG_VAR) );
 pprintf("size of INIT_LINK_VAR %d \n", sizeof(INIT_LINK_VAR) );
 pprintf("size of DOWN_LINK_VAR %d \n", sizeof(DOWN_LINK_VAR) );
 pprintf("size of CONFIG_LINK %d \n", sizeof(CONFIG_LINK) );
 pprintf("size of PART_SLIM_VAR %d \n", sizeof(PART_SLIM_VAR) );
 pprintf("size of CONFIG_RING_VAR %d \n", sizeof(CONFIG_RING_VAR) );
 pprintf("size of RESET_RING_VAR %d \n", sizeof(RESET_RING_VAR) );
 pprintf("size of READ_CONFIG_VAR %d \n", sizeof(READ_CONFIG_VAR) );
 pprintf("size of READ_RCONF_VAR %d \n", sizeof(READ_RCONF_VAR) );
 pprintf("size of READ_SPARM_VAR %d \n", sizeof(READ_SPARM_VAR) );
 pprintf("size of READ_STATUS_VAR %d \n", sizeof(READ_STATUS_VAR) );
 pprintf("size of READ_RPI_VAR %d \n", sizeof(READ_RPI_VAR) );
 pprintf("size of READ_XRI_VAR %d \n", sizeof(READ_XRI_VAR) );
 pprintf("size of READ_REV_VAR %d \n", sizeof(READ_REV_VAR) );
 pprintf("size of READ_LNK_VAR %d \n", sizeof(READ_LNK_VAR) );
 pprintf("size of REG_LOGIN_VAR %d \n", sizeof(REG_LOGIN_VAR) );
 pprintf("size of UNREG_LOGIN_VAR %d \n", sizeof(UNREG_LOGIN_VAR) );
 pprintf("size of READ_LA_VAR %d \n", sizeof(READ_LA_VAR) );
 pprintf("size of CLEAR_LA_VAR %d \n", sizeof(CLEAR_LA_VAR) );
 pprintf("size of DUMP_VAR %d \n", sizeof(DUMP_VAR) );
 pprintf("size of UPDATE_CFG_VAR %d \n", sizeof(UPDATE_CFG_VAR) );

 pprintf("size of SERV_PARM %d \n", sizeof(SERV_PARM) );
 pprintf("size of command %d\n", sizeof(struct command) );
 pprintf("size of IOCBQ %d\n", sizeof( IOCBQ ) );
 pprintf("size of TIMERQ %d\n", sizeof( struct TIMERQ ) );
 pprintf("size of generic pb %d\n", sizeof(struct pb) );
 pprintf("size of kgpsa_pb %d \n", sizeof(struct kgpsa_pb) );
 pprintf("size of generic sb %d \n", sizeof(struct sb) );
 pprintf("size of scsi_sb %d \n", sizeof(struct scsi_sb) );
 pprintf("size of scsi3_sb %d \n", sizeof(struct scsi3_sb) );
 pprintf("size of kgpsa_sb %d \n", sizeof(struct kgpsa_sb) );
 pprintf("size of ub %d\n", sizeof( struct ub ) );
 pprintf("size of inode %d\n", sizeof( struct INODE ) );

 pprintf("size of NAME_TYPE %d \n", sizeof( NAME_TYPE) );
 pprintf("size of SEMAPHORE %d \n", sizeof(struct SEMAPHORE) );
 pprintf("size of RING %d \n", sizeof(RING) );
 pprintf("size of FC_BRD_INFO %d\n", sizeof(FC_BRD_INFO) );
 pprintf("size of LOCK %d \n", sizeof(struct LOCK) );

 pprintf("size of registered_wwid %d\n", sizeof(struct registered_wwid) );
 pprintf("size of registered_portname %d\n", sizeof( struct registered_portname ) );
 pprintf("size of wwid_t %d\n", sizeof( wwid_t ) );
 pprintf("MAX_INQ_SZ %d \n", MAX_INQ_SZ );


 pprintf("MAX_ELBUF %d\n", MAX_ELBUF );

#endif

kgpsa_ev_init();

#if LEAK_WATCH
  krn$_seminit( &kgpsa_leak_lock, 1, "leak_lock" );
  kgpsa_leak_q.flink = &kgpsa_leak_q;
  kgpsa_leak_q.blink = &kgpsa_leak_q;
#endif


#if KGPSA_DEBUG
 for (i=0; i<10; i++) { kgpsa_pbs[i] =  0; };
 num_kgpsa = 0;
#endif

 find_pb( "kgpsa", sizeof( struct kgpsa_pb ), kgpsa_init_pb );

 status = msg_success;
 return status;
}
#endif /* !( STARTSHUT || DRIVERSHUT ) */




#if STARTSHUT||DRIVERSHUT
/*+
 * ============================================================================
 * = kgpsa_init -                                                             =
 * ============================================================================
 *
-*/
int kgpsa_init( struct device *kgpsa_dev )
{
 int status;
 int i;
 struct pb *pb=0;          /* init'ed to zero because lw_malloc will trace
                            * mallocs associated with a pb.  If pb=0, no
			    * tracing done.  Since we do not have a pb on
			    * the malloc of the pb, just make sure it is 0.
			    */



#if 0
 pprintf("size of MAILBOX: %d (decimal) \n",sizeof(MAILBOX) );
 pprintf("size of MAILBOX_CMD_BSIZE %d (decimal) \n", MAILBOX_CMD_BSIZE );
 pprintf("size of MAILVARIATNS: %d (decimal) \n", sizeof(MAILVARIANTS) );

 pprintf("size of LOAD_SM_VAR %d \n", sizeof(LOAD_SM_VAR) );
 pprintf("size of READ_NV_VAR %d \n", sizeof(READ_NV_VAR) );
 pprintf("size of WRITE_NV_VAR %d \n", sizeof(WRITE_NV_VAR) );
 pprintf("size of BIU_DIAG_VAR %d \n", sizeof(BIU_DIAG_VAR) );
 pprintf("size of INIT_LINK_VAR %d \n", sizeof(INIT_LINK_VAR) );
 pprintf("size of DOWN_LINK_VAR %d \n", sizeof(DOWN_LINK_VAR) );
 pprintf("size of CONFIG_LINK %d \n", sizeof(CONFIG_LINK) );
 pprintf("size of PART_SLIM_VAR %d \n", sizeof(PART_SLIM_VAR) );
 pprintf("size of CONFIG_RING_VAR %d \n", sizeof(CONFIG_RING_VAR) );
 pprintf("size of RESET_RING_VAR %d \n", sizeof(RESET_RING_VAR) );
 pprintf("size of READ_CONFIG_VAR %d \n", sizeof(READ_CONFIG_VAR) );
 pprintf("size of READ_RCONF_VAR %d \n", sizeof(READ_RCONF_VAR) );
 pprintf("size of READ_SPARM_VAR %d \n", sizeof(READ_SPARM_VAR) );
 pprintf("size of READ_STATUS_VAR %d \n", sizeof(READ_STATUS_VAR) );
 pprintf("size of READ_RPI_VAR %d \n", sizeof(READ_RPI_VAR) );
 pprintf("size of READ_XRI_VAR %d \n", sizeof(READ_XRI_VAR) );
 pprintf("size of READ_REV_VAR %d \n", sizeof(READ_REV_VAR) );
 pprintf("size of READ_LNK_VAR %d \n", sizeof(READ_LNK_VAR) );
 pprintf("size of REG_LOGIN_VAR %d \n", sizeof(REG_LOGIN_VAR) );
 pprintf("size of UNREG_LOGIN_VAR %d \n", sizeof(UNREG_LOGIN_VAR) );
 pprintf("size of READ_LA_VAR %d \n", sizeof(READ_LA_VAR) );
 pprintf("size of CLEAR_LA_VAR %d \n", sizeof(CLEAR_LA_VAR) );

 pprintf("size of SERV_PARM %d \n", sizeof(SERV_PARM) );
 pprintf("size of command %d\n", sizeof(struct command) );
 pprintf("size of IOCBQ %d\n", sizeof( IOCBQ ) );
 pprintf("size of TIMERQ %d\n", sizeof( struct TIMERQ ) );
 pprintf("size of generic pb %d\n", sizeof(struct pb) );
 pprintf("size of kgpsa_pb %d \n", sizeof(struct kgpsa_pb) );
 pprintf("size of generic sb %d \n", sizeof(struct sb) );
 pprintf("size of scsi_sb %d \n", sizeof(struct scsi_sb) );
 pprintf("size of scsi3_sb %d \n", sizeof(struct scsi3_sb) );
 pprintf("size of kgpsa_sb %d \n", sizeof(struct kgpsa_sb) );
 pprintf("size of ub %d\n", sizeof( struct ub ) );
 pprintf("size of inode %d\n", sizeof( struct INODE ) );

 pprintf("size of NAME_TYPE %d \n", sizeof( NAME_TYPE) );
 pprintf("size of SEMAPHORE %d \n", sizeof(struct SEMAPHORE) );
 pprintf("size of RING %d \n", sizeof(RING) );
 pprintf("size of FC_BRD_INFO %d\n", sizeof(FC_BRD_INFO) );
 pprintf("size of LOCK %d \n", sizeof(struct LOCK) );

 pprintf("size of registered_wwid %d\n", sizeof(struct registered_wwid) );
 pprintf("size of registered_portname %d\n", sizeof( struct registered_portname ) );
 pprintf("size of wwid_t %d\n", sizeof( wwid_t ) );
 pprintf("MAX_INQ_SZ %d \n", MAX_INQ_SZ );


 pprintf("MAX_ELBUF %d\n", MAX_ELBUF );
#endif


 pb = malloc(sizeof(struct kgpsa_pb), "pb");

 pb->hose          = kgpsa_dev->hose;
 pb->slot          = kgpsa_dev->slot;
 pb->function      = kgpsa_dev->function;
 pb->bus           = kgpsa_dev->bus;
 pb->mode          = DDB$K_STOP;
 pb->vector        = pci_get_vector(kgpsa_dev);
 pb->config_device = kgpsa_dev;
 pb->hardware_name = kgpsa_dev->tbl->name;
 pb->controller    = kgpsa_dev->devdep.io_device.controller;
 pb->protocol      = "pg";
 dprintf("pb->hardware_name: %s\n", p_args1(pb->hardware_name) );

 kgpsa_dev->devdep.io_device.vector[0] = pb->vector;
 kgpsa_dev->devdep.io_device.devspecific = 0;
 kgpsa_dev->devdep.io_device.devspecific2 = (int)pb;

 kgpsa_init_pb(pb);

 status = msg_success;
 return status;
}
#endif



/*+
 * ============================================================================
 * = kgpsa_init_pb - init the pb                                              =
 * ============================================================================
 *
-*/
int kgpsa_init_pb( struct kgpsa_pb *pb )
{
    int i, t;
    unsigned int base0[2],base1[2],base2[2];
    struct kgpsa_nvram_region nvstack, *nvr = &nvstack;

#if !RAWHIDE
    log_driver_init(pb);
#endif

    if (pb->pb.name[0] == 0) {
       dprintf("first time\n", p_args0);
       dprintf("pb: %x \n", p_args1(pb) );
    } else {
       dprintf("%s already initialized\n", p_args1(pb->pb.name));
       goto EXIT;
    }

#if KGPSA_DEBUG
    kgpsa_pbs[num_kgpsa] =  pb;
    pb->pbx = num_kgpsa;
    num_kgpsa++;
#endif

    base0[0] = incfgl(pb, BASE0);
    if ( base0[0] & 0x8 ) {
       pb->prefetch = 1;
    }
    base0[0] = base0[0] & 0xFFFFFFF0;
    base0[1] = incfgl(pb, BASE0+4);
    base1[0] = incfgl(pb, BASE1) & 0xFFFFFFF0;
    base1[1] = incfgl(pb, BASE1+4);
    base2[0] = incfgl(pb, BASE2) & 0xFFFFFFF0;
    base2[1] = incfgl(pb, BASE2+4);
    pb->slim_base     =  *(unsigned __int64*)base0;
    pb->csr_mem_base  =  *(unsigned __int64*)base1;
    pb->csr_io_base   =  *(unsigned __int64*)base2;
    pb->elscmdcnt = 0;
    pb->fcpcmdcnt = 0;
    pb->topology      =  FLAGS_TOPOLOGY_MODE_LOOP;

    /*
     * Default the topology to PT2PT, then try and
     * get a stored value out of the modules nvram.
     */
    pb->topology      =  FLAGS_TOPOLOGY_MODE_PT_PT;
    /*
     * Set up name now for early error messages.  It will not have the
     * correct node_id (hardcoded to zero here) for the remote id field, 
     * but that is no biggie.
     */
#if !(STARTSHUT||DRIVERSHUT)
    set_io_name( pb->pb.name, 0, 0, 0, pb );
#else
    set_io_name( pb->pb.name, "kgpsa", 0, 0, pb );
#endif
    if(kgpsa_read_compaq_nvdata( pb, nvr ) == msg_success )
      {
       pb->topology = nvr->topology;
      }

    if(pb->topology == FLAGS_TOPOLOGY_MODE_LOOP)
       printf("%s - in LOOP mode. LOOP not supported by console\n", pb->pb.name);

    kgpsa_record_iocb_setup( pb, 1, iocbbuf );      /* 1 means ENABLE */
    kgpsa_record_slim_setup( pb, 1, 0x100 );     /* 1 means ENABLE */
  
    t = kgpsa_init_port( pb );
  
EXIT:
    return t;
}




/*+
 * ============================================================================
 * = kgpsa_init_port - init the port                                          =
 * ============================================================================
 *
 * This routine is only called once for a Monolithic console, but it is
 * called each startup of the STARTSHUT/DRIVERSHUT console.
 *
 *
-*/
int kgpsa_init_port( struct kgpsa_pb *pb )
{
 int i;
 int status;
 RING	*rp;
 FC_BRD_INFO *binfo;
 char name[32];
 int kgpsa_setmode( );
 int kgpsa_poll_process( );
 int kgpsa_interrupt( );
 int kgpsa_fc_process( );
 int kgpsa_poll_for_interrupt();
 struct kgpsa_sb *sb;

 krn$_seminit( &pb->els_owner_s, 1, "els_owner" );
 krn$_seminit( &pb->fcp_owner_s, 1, "fcp_owner" );
 krn$_seminit( &pb->poll_done_s, 0, "poll_done" );

 /*
  * The ring_s semaphore guards reading and writing the rings.  It may
  * be superfluous and redundant with the els_owner_s and fcp_owner_s 
  * It is initialized to 0 because
  * in kgpsa_init_kgpsa (called from setmode START and initial startup), we
  * post this semaphore after the rings are initialize.  
  *
  */
 krn$_seminit( &pb->ring_s, 0,  "fc_ring" );
 pb->mbx_l.req_ipl = IPL_SYNC;
 pb->record_l.req_ipl = IPL_SYNC;

 /*
  * Allocate the buffer used to store the current list of scsi targets.
  * In loop mode, this will just be a replica of the alpa map.  In switch
  * mode, this contains the results of the query to the Name Server.
  * It is then used to drive the probing of the ports.
  */
 pb->scsi_targets = malloc(CT_BUF_SZ, "scsi_targets" );  

/*
 * HACK
 */
#if KGPSA_POLLED
 pb->pb.vector = 0;
 pprintf("KGPSA in POLLED mode\n");
#endif

 pb->node_id = 0;
 pb->pb.setmode = kgpsa_setmode;
 if( pb->pb.vector ) {
   pb->pb.mode = DDB$K_INTERRUPT;
   pb->pb.desired_mode = DDB$K_INTERRUPT;
   int_vector_set( pb->pb.vector, kgpsa_interrupt, pb );
 } else {
#if KGPSA_POLLED
   pprintf("pb.mode set to DDB$K_POLLED\n");
   pb->pb.mode = DDB$K_POLLED;
   pb->pb.desired_mode = DDB$K_POLLED;
   pb->pb.pq.routine = kgpsa_poll_for_interrupt;
   pb->pb.pq.param = pb;
#endif  /* KGPSA_POLLED */
 }

 pb->linkdownintvcnt = kgpsa_down_int_cnt;

 binfo = &pb->binfo;
 for (i=0; i < MAX_RINGS; i++)
   {
    rp = &binfo->fc_ring[i];
    rp->active_q.flink  = &rp->active_q;
    rp->active_q.blink  = &rp->active_q;
   }

 sprintf( pb->pb.alias, "PG%c0", pb->pb.controller + 'A' );
#if !(STARTSHUT||DRIVERSHUT)
 set_io_name( pb->pb.name, 0, 0, pb->node_id, pb );
#else
 set_io_name( pb->pb.name, "kgpsa", 0, pb->node_id, pb );
#endif


 krn$_seminit( &pb->fc.wake_t.sem, 0, "kgpfcwake" );
 krn$_init_timer( &pb->fc.wake_t );
 krn$_seminit( &pb->fc.comp_s, 0, "kgfccomp");

 pb->fc.pid =
   krn$_create(kgpsa_fc_process,
                                        /* address of process   */
      null_procedure,                   /* startup routine      */
      &pb->fc.comp_s,		        /* completion semaphore */
      6,                                /* process priority     */
      1 << whoami(),                    /* cpu affinity mask    */
      0,                                /* stack size           */
      set_io_prefix( pb, name, "_fcint"), /* process name         */
      0, 0, 0, 0, 0, 0,                 /* i/o                  */
      pb);



 krn$_seminit( &pb->poll.wake_s, 0, "kgppollwake" );
 krn$_seminit( &pb->poll.comp_s, 0, "kgpollcomp");
 pb->poll.pid =
   krn$_create(kgpsa_poll_process,
                                        /* address of process   */
      null_procedure,                   /* startup routine      */
      &pb->poll.comp_s,		        /* completion semaphore */
      6,                                /* process priority     */
      1 << whoami(),                    /* cpu affinity mask    */
      0,                                /* stack size           */
      set_io_prefix( pb, name, "__poll"), /* process name         */
      0, 0, 0, 0, 0, 0,                 /* i/o                  */
      pb);


 /*
  * TODO: (future release) 
  * Should I move to an insert_sb mechanism to make for an unlimited 
  * number of sb's.
  */

 pb->pb.sb = malloc( FC_MAX_NPORTS * sizeof( struct kgpsa_sb * ), "fc_max_nports*(*sb)" );
 pb->pb.num_sb = FC_MAX_NPORTS;
 pb->node_count = 0;

 kgpsa_krn$_wait( &pb->fcp_owner_s, pb->pbx, "kgpsa_reset" );

 /*
  *
  * Initialize the KGPSA hardware.
  * 
  */
 status = kgpsa_init_kgpsa( pb );
 if (status != msg_success)  {
   pb->pb.mode = DDB$K_STOP;
   err_printf(msg_ex_init_fail, pb->pb.name);
 }

 kgpsa_krn$_post( &pb->fcp_owner_s, pb->pbx, "kgpsa_reset" );

 sb = pb->pb.sb[0];
 sprintf( pb->pb.info, "WWN %s", sb->nodename );
 sprintf( pb->pb.string, "%-24s   %-8s   %24s",
	    pb->pb.name, pb->pb.alias, pb->pb.info );


 return status;
}



/*+
 * ============================================================================
 * = kgpsa_init_kgpsa - initialize the kgpsa hardware                         =
 * ============================================================================
 *
 * This routine is called every time the driver is started.  On a
 * monolithic console, that means via at poweron, and during a callback
 * open, or a ">>> start -d".
 *
 *  1. Init the chip
 *  2. Part Slim
 *  3. Config Ring
 *  4. Config Link
 *  5. Init Link
 *
-*/
int kgpsa_init_kgpsa( struct kgpsa_pb *pb )
{
 int         i,j;
 int         status;
 int         status2;
 int	     al_pa;
 int	     rpi;
 int	     kgpsa_poll();
 char	     name[16];
 struct	     kgpsa_sb *sb;
 MAILBOX     lmb, *mb;
 FC_BRD_INFO *binfo;
 RING        *rp;
 SERV_PARM   *sp;

 status = rl(HS);
 dprintf("FireFlyInitialize: status = %x\n", p_args1(status));
 binfo = &pb->binfo;
 mb = &lmb;		
 memset( &lmb, 0, MAILBOX_CMD_BSIZE );

 /*
  * I found that I had to do the reset on the FF every time even if
  * these bits said it was ready.  Over START/STOP, it needed to be
  * hit.
  */

  kgpsa_make_FF_ready(pb);


  /*
   *  Clear the mbxhc and owner bits so we don't get confused when
   *  trying to write the first mailbox;  It might have powered up
   *  to all F's
   */
  ws(0,0);


 /*
  * For now Clear ALL interrupt enable conditions
  */
 wl(HC,0);

 /*
  * Clear Host Attention register
  */
 wl(HA,0xFFFFFFFF);


 /* Initialize configuration parameters */
 binfo->fc_ffnumrings = MAX_CONFIGURED_RINGS; /* number of rings */

 /* Ring 0 - ELS */
 binfo->fc_nummask[0] = 1;
 binfo->fc_ring[0].fc_numCiocb = IOCB_CMD_R0_ENTRIES;
 binfo->fc_ring[0].fc_numRiocb = IOCB_RSP_R0_ENTRIES;

 binfo->fc_rval[0] = FC_ELS_REQ;     /* ELS request */
 binfo->fc_tval[0] = FC_ELS_DATA;    /* ELS */

 /* Ring 1 - IP */
 binfo->fc_nummask[1] = 1;
 binfo->fc_ring[1].fc_numCiocb = IOCB_CMD_R1_ENTRIES;
 binfo->fc_ring[1].fc_numRiocb = IOCB_RSP_R1_ENTRIES;
 binfo->fc_rval[1] = FC_DEV_DATA;    /* Device Data */
 binfo->fc_tval[1] = FC_LLC_SNAP;    /* LLC/SNAP */

 /* Ring 2 - FCP */
 binfo->fc_nummask[2] = 2;
 binfo->fc_ring[2].fc_numCiocb = IOCB_CMD_R2_ENTRIES;
 binfo->fc_ring[2].fc_numRiocb = IOCB_RSP_R2_ENTRIES;
 binfo->fc_rval[2] = 0x06;    /* */
 binfo->fc_tval[2] = 0x08;    /* */
 binfo->fc_rval[3] = 0x03;	     /* Name server */
 binfo->fc_tval[3] = 0x20;	     /* Name server */


 /*
  * Issue PART_SLIM Mailbox command
  */
 status = kgpsa_part_slim( pb, &lmb );
 if (status != msg_success) {
   pprintf(" part_slim failed\n",p_args0);
 }


 /* Initialize cmd/rsp ring pointers */
 for (i = 0; i < binfo->fc_ffnumrings; i++) {
   rp = &binfo->fc_ring[i];
   /* offsets are from the beginning of SLIM */
   rp->fc_cmdringaddr = (ulong)(mb->un.varSlim.ringdef[i].offCiocb);
   rp->fc_rspringaddr = (ulong)(mb->un.varSlim.ringdef[i].offRiocb);
   dprintf("Partslim 0x%x 0x%x 0x%x 0x%x\n", p_args4(
            (mb->un.varSlim.ringdef[i].offCiocb),
            (mb->un.varSlim.ringdef[i].offRiocb),
            (mb->un.varSlim.ringdef[i].numCiocb),
            (mb->un.varSlim.ringdef[i].numRiocb)    ) );
 }

 /*
  * CONFIG_RING - configure all the rings
  */
 kgpsa_config_rings( pb, &lmb );

 /*
  * kgpsa_reset_ring needed to initialize console data structure
  * counters.  The kgpsa is already reset by the above, but the
  * additional reset does not hurt anything.
  */
 kgpsa_reset_ring ( pb, &lmb, FC_ELS_RING );
 kgpsa_reset_ring ( pb, &lmb, 1 );
 kgpsa_reset_ring ( pb, &lmb, FC_FCP_RING );

 /*
  * Config Link.  Reset myDid.  myDid may be non-zero if the driver had 
  * been running already.
  */
 binfo->fc_myDID = 0;
 kgpsa_config_link( pb, &lmb );

 /*
  * Init the Link
  */
 /*
  * TODO: (medium) (7-Nov-98 - needs soak time)
  *      For most of the life of this code, the init link used to wait
  *      for a LATT to occur before proceding.   But Since kgpsa_read_alpa_map
  *      waits for the link to be up, what's the point.  At this time, 
  *      all callers of kgpsa_init_link are DONT_WAIT.  The wait for latt
  *      code could be removed from the kgpsa_init_link if no reason for it
  *      arises.
  */
 kgpsa_init_link( pb, &lmb, DONT_WAIT ); 

 /*
  * Wait for the link to come up.  Find out myDid.  Get the alpamap.
  */
 status2 = kgpsa_read_alpa_map( pb, &lmb, SECONDS_2 );
 if (status2 == msg_success) {
   kgpsa_dump_mbx( "READ_LA[1]:", &lmb, (kgpsa_debug_flag&1) | kgpsa_readla_dump);
   kgpsa_dump_blk( "ALPA_MAP[1]:", binfo->alpa_map, (kgpsa_debug_flag&1) | kgpsa_readla_dump);
 } else {
   dprintf("kgpsa_init_kgpsa - kgpsa_read_alpa_map failed \n", p_args0 );
 }

 /*
  * Read the Service Parameters
  */
 status = kgpsa_read_sparm(pb, &lmb, &binfo->fc_sparam);

 sb = kgpsa_new_node( pb, binfo->fc_myDID, &binfo->fc_sparam );
 kgpsa_set_nstate( sb, NLP_ADAPTER );

 /*
  * Allow use of the rings.  Enable this now so that we can allocate
  * receive buffers to the adapter via the rings.
  */
 kgpsa_krn$_post( &pb->ring_s, pb->pbx, "kgpsa_init_kgpsa" );

 /*
  * Allocate some buffers for unsolicited receive messages.
  * See the comments in kgpsa_def.h for complete details.
  */
 kgpsa_alloc_rcv_bufs( pb, kgpsa_rcv_bufs );

#if !KGPSA_POLLED

 io_enable_interrupts( pb, pb->pb.vector );

#else  /* KGPSA_POLLED */
 pprintf("inserting onto pollq.\n");
 insq_lock( &pb->pb.pq.flink, &pollq );

#endif

 wl(HA,HA_R0ATT);
 wl(HC,HC_R0INT_ENA);


 /*
  * Clear outstanding interrupts and  Enable interrupts
  */
 wl(HA,HA_LATT | HA_R0ATT      | HA_R2ATT    | 
                 HA_R0RE_REQ   | HA_R2RE_REQ |
                 HA_R0CE_RSP   | HA_R2CE_RSP );

 kgpsa_set_devState( pb, DEV_INITIAL_DISCOVER, "INIT_KGPSA" );


 wl( CA, CA_R0CE_REQ|CA_R0RE_RSP|CA_R2CE_REQ|CA_R2RE_RSP);
 wl( CA, CA_R0ATT|CA_R2ATT);
 wl( HC, HC_MBINT_ENA | HC_LAINT_ENA | HC_R0INT_ENA | HC_R2INT_ENA );

 /*
  * Start up a timer that will wake up the fc_process in a few seconds.
  * fc_process will restart the timer and wake up ever fc_wake_interval
  * time period.  
  */
 krn$_start_timer( &pb->fc.wake_t, fc_wake_interval );

 return status;

}


/*+
 * ============================================================================
 * = kgpsa_alloc_rcv_bufs - allocate buffers and give them to the adapter     =
 * ============================================================================
 *
 *  Pre-queue the adapter n buffers for unsolicited payloads that come off the
 *  wire.  When the adapter runs out of buffers, the adapter will reject
 *  any unsolicited incoming.  It will be up to the remote n_port to retry.
 *  This is only done at adapter initialization.
 *
 * aug 27, 97   malloc 256 byte buffers.  No ELS should be bigger than that
 * and we don't accept unsolicited SCSIs (no SCSI target mode support)
-*/
void kgpsa_alloc_rcv_bufs( struct kgpsa_pb *pb, int n ) {
  int i;
  FC_BRD_INFO *binfo;
  RING *rp;

  binfo = &pb->binfo;
  rp = &binfo->fc_ring[FC_ELS_RING];
  rp->rcv_blk = malloc( n*MAX_ELS_BUF, "rcv_blk" );    
  for(i = 0; i < n; i++) {
    kgpsa_que_ring_buf( pb, FC_ELS_RING, &rp->rcv_blk[i*MAX_ELS_BUF], MAX_ELS_BUF );
  }
}

/*+
 * ============================================================================
 * = kgpsa_alloc_rcv_bufs - deallocate buffers previously give to the adapter =
 * ============================================================================
 *
 * This is the partner routine for kgpsa_alloc_rcv_bufs.
 *
-*/
void kgpsa_dealloc_rcv_bufs( struct kgpsa_pb *pb, int ringno ){
  FC_BRD_INFO *binfo;
  RING *rp;
  char *rcv_blk;

  binfo = &pb->binfo;
  rp = &binfo->fc_ring[ringno];       /* better be the FC_ELS_RING */
  free( rp->rcv_blk, "rcv_blk" );
}




/************************************************************************
 * Find routines                                                        *
 *                                                                      *
 * We have to look thru our list of Nports in several                   *
 * ways.                                                                *
 *                                                                      *
 ************************************************************************/

#define FIND_DID 1
#define FIND_RPI 2
#define BY_PORNAME 1
#define BY_NODE    2

#define kgpsa_find_did( x, y )  kgpsa_find_did_rpi( x, y, FIND_DID )
#define kgpsa_find_rpi( x, y )  kgpsa_find_did_rpi( x, y, FIND_RPI )

struct kgpsa_sb *kgpsa_find_did_rpi( struct kgpsa_pb *pb, int value, int type )
{
 int i;
 struct kgpsa_sb *sb;
 for( i = 0; i < pb->node_count; i++ )
   {
    if (!pb->pb.sb[i]) continue;
    sb = pb->pb.sb[i];

    if( sb->nstate == NLP_ADAPTER ) continue;

    switch (type) {
      case FIND_DID:
        if( (sb->did == value)  && (sb->valid) ) {
           dprintf("did %x found as sb: %x node_id:%x\n", 
                                           p_args3(value, sb, sb->node_id) );
           return (sb);
        }
	break;
      case FIND_RPI:
        if( (sb->rpi == value) && (sb->valid) ) {
           dprintf("rpi %x found as sb: %x node_id:%x\n", 
                                           p_args3(value, sb, sb->node_id) );
           return (sb);
        }
        break;
    } /* switch */

  } /* for */

  return (NULL);

}



struct kgpsa_sb *kgpsa_find_by_portname( struct kgpsa_pb *pb, NAME_TYPE *portname )
{
 int i;
 int status;
 struct kgpsa_sb *sb;
 int found = FALSE;
 for ( i=0; i < FC_MAX_NPORTS; i++) {
   sb = pb->pb.sb[i];
   if( sb )
    if (sb->valid)
      if ( kgpsa_cmp_name( &sb->serv_parm.portName, portname ) == 0 ) {
        found = TRUE;
	break;
      }
 }
 if ( found == FALSE ) {
   sb = NULL;
 }
 return sb;


}

/*+
 * ============================================================================
 * = kgpsa_find_sb - lookup an sb in the pb's list of sb's                    =
 * ============================================================================
 *
 * sb's are unique and 1-to-1 with the portname.  This routine looks returns
 * an sb from the pb list of sb's that matches the portname arguement.
 * Also, the portname:did relationship is 1-to-1.
 *
 * There is an exception to the 1-to-1'ness.  The switch will have several
 * dids all off of one portname.  The console makes separate sb's for each did.
 * For the switch service dids which all share the same portname.   For did's
 * that are over 0xFFFFF0, we use the did information to uniquely identify
 * the sb. 
 * ASSUMPTION: We will never see a did < 0xFFFFF0 on a port that is ever
 *             associated with a did > 0xFFFFF0.
 *
-*/
struct kgpsa_sb *kgpsa_find_sb( struct kgpsa_pb *pb, SERV_PARM *s, int did )
{
 int i;
 int status;
 struct kgpsa_sb *sb;
 int found = FALSE;
 for ( i=0; i < FC_MAX_NPORTS; i++) {
   sb = pb->pb.sb[i];
   if( sb )
    {
      if ( kgpsa_cmp_name( &sb->serv_parm.portName, &s->portName ) == 0 ) {
	if( did < 0xFFFFF0 ) 
           {
            found = TRUE;
	    break;
           }
        else
           {
	    if( did == sb->did )
              {
	        found = TRUE;
		break;
	      }
           }
      }
    }
 }

 if ( found == FALSE ) {
   sb = NULL;
 }

 return sb;

}

/************************************************************************
 * End of Find routines                                                 *
 ************************************************************************/








/*+
 * ============================================================================
 * = kgpsa_logout_all - logout from n_ports that we are logged into           =
 * ============================================================================
 * 
 * Called from the stop driver context.  Therefore, issueing ELSes will
 * require the els_owner_s.  This routine is a courtesy.  We could just shut
 * down the adapter without issueing logouts.   That is why there is no need
 * retry the LOGO/PRLO if they fail.
 *
-*/
void kgpsa_logout_all( struct kgpsa_pb *pb )
{
 int status;
 int i;
 int al_pa;
 struct kgpsa_sb *sb;

 kgpsa_krn$_wait( &pb->els_owner_s, pb->pbx, "els_cmd");

 status = kgpsa_unreg_login ( pb, 0xffff ); /* all nodes */
 if (status == msg_success) {
   for (i = 0; i < pb->node_count; i++ ) {
     sb = pb->pb.sb[i];
     if (!sb) {
       dprintf("SANITY ERROR: null sb found in kgpsa_logout_all: %x\n",
                                              p_args1(i) );
     }
     if (sb->nstate != NLP_LOGIN) continue;
     dprintf("Logging out al_pa: %x \n", p_args1(sb->did) );
     status = kgpsa_els_cmd( sb->pb, sb->did, ELS_CMD_FCP_PRLO, WAIT, DONT_RETRY);
     status = kgpsa_els_cmd( sb->pb, sb->did, ELS_CMD_LOGO, WAIT, DONT_RETRY );
     kgpsa_set_nstate( sb, NLP_LOGOUT );
   }
 }

 kgpsa_krn$_post( &pb->els_owner_s, pb->pbx, "els_cmd");

}




/*+
 * ============================================================================
 * = kgpsa_new_node - verify service params and make a new sb when necessary  =
 * ============================================================================
 *
-*/
struct kgpsa_sb *kgpsa_new_node( struct kgpsa_pb *pb, int did, SERV_PARM *sp )
{
  int status;
  int port_index;
  char portname[20], nodename[20];
  struct kgpsa_sb *sb, *sb1;
  struct registered_portname *pn;

  wwn_sprintf( &portname, &sp->portName );
  wwn_sprintf( &nodename, &sp->nodeName );
  dprintf("kgpsa_new_node did:%x as %s\n", p_args2(did, portname) );


  /*
   *  See if the did has been already put in the list.
   */
  sb1 = kgpsa_find_did( pb, did );


  /*
   * Still looking for an sb.
   */
  if ( sb=kgpsa_find_sb( pb, sp, did ) ){
     dprintf("found a match for %s as did: %x\n", 
                                      p_args2(portname, did) );
     if ( sb->did != did ) {
       dprintf("FYI: did changed from %x to %x\n", 
                                      p_args2(sb->did, did) );
     }
  } else {
     /*
      * Create an sb
      */
     dprintf("match Not found - creating an sb\n", p_args0 );
     sb = kgpsa_alloc_sb( pb );
     /*
      *  Is this a registered port?   If so, the evnum is returned
      *  and that is a small integer (<10)
      */
     pn = fc_find_portname(&sp->portName, &port_index, FC_SWAP_BYTES);
     if( pn && (pn->evnum != -1) )
        sb->node_id = pn->evnum;   /* Asssume: evnum < 10  */
     else
        sb->node_id = pb->node_count + 10;  /* start these above ev defined*/
     pb->pb.sb[pb->node_count++] = sb;     /* install it for as a real node */
     dprintf("Node_id %x  for sb %x \n", p_args2( sb->node_id, sb ) );  
  }


  /*  
   * See if this DID was stolen from somebody else.  If so then
   * invalidate the older sb.
   * TESTING:  This path is nicely tested in the switch configuration.
   *           Move the host connection from one port to the other and
   *           the 0xFFFFFx did's pick up new portnames.
   */
   if (sb1){
     if ( sb != sb1 ) {
        dprintf("Duplicate did. invalidated older.\n", p_args0 );
        dprintf("    %x  %s  New \n", p_args2(did, portname));
        dprintf("    %x  %s  INVALIDATED \n", p_args2(sb1->did, sb1->portname));
        sb1->valid = 0;
        sb1->rpi = 0;
        kgpsa_set_nstate( sb1, NLP_LOGOUT );
        kgpsa_set_prli_state( sb1, PRLI_NG );

     }
   }
  memcpy( &sb->serv_parm, sp, sizeof(SERV_PARM) );
  strcpy( &sb->nodename, nodename );
  strcpy( &sb->portname, portname );
  set_io_name( sb->name, 0, 0, sb->node_id, pb );
  sb->did = did;
  sb->valid = 1;
  return sb;
 }



/*+
 * ============================================================================
 * = kgpsa_alloc_sb - allocate an sb buffer and fill in constructs            =
 * ============================================================================
 *
-*/
struct kgpsa_sb *kgpsa_alloc_sb( struct kgpsa_pb *pb )
{
    struct kgpsa_sb *sb;
    int kgpsa_command ( );
    void kgpsa_rundown_send_inquiry ( );

    pb->sb_cnt++;
    dprintf("kgpsa_alloc_sb - %d\n", p_args1(pb->sb_cnt) );
    sb = malloc( sizeof( struct kgpsa_sb ), "sb" );
    sb->pb = pb;
    sb->ub.flink = &sb->ub.flink;
    sb->ub.blink = &sb->ub.flink;
    sb->command = kgpsa_command;
    sb->send_inquiry = kgpsa_rundown_send_inquiry;
    sb->ok = 1;
    sb->valid = 0;
    kgpsa_set_nstate( sb, NLP_UNUSED );
    kgpsa_set_prli_state( sb, PRLI_NG );
#if MODULAR || STARTSHUT
    krn$_seminit(&sb->ready_s, 0, "kgpsa_sb_ready");
#endif
    return sb;
}


/*+
 * ============================================================================
 * = kgpsa_dealloc_sb - Free the memory associated with an sb                 =
 * ============================================================================
 *
-*/
void kgpsa_dealloc_sb( struct kgpsa_sb *sb )
{
  struct kgpsa_pb *pb;
  if (sb->ub.flink != &sb->ub.flink){
    dprintf("ERROR: Trying to delete an sb while units on its queue\n", p_args0);
    dprintf("sb->ub.flink:%x &sb->ub.flink:%x\n",
                                       p_args2(sb->ub.flink, &sb->ub.flink) );
  }
#if MODULAR || STARTSHUT
  krn$_semrelease(&sb->ready_s);
#endif
  pb = sb->pb;
  pb->sb_cnt--;
  dprintf("kgpsa_dealloc_sb - %d  did:%x\n", p_args2(pb->sb_cnt, sb->did) );
  free( sb, "sb" );
}



/*+
 * ============================================================================
 * = kgpsa_cmp_name - compare two portnames                                   =
 * ============================================================================
 *
-*/
int kgpsa_cmp_name( NAME_TYPE *n1, NAME_TYPE *n2 )
{
 int status;
 status = memcmp (n1, n2, sizeof(NAME_TYPE) );
 return status;
}






/*+
 * ============================================================================
 * = kgpsa_setmode - change driver state                                      =
 * ============================================================================
 *
-*/
#if !(STARTSHUT||DRIVERSHUT)
void kgpsa_setmode( struct kgpsa_pb *pb, int mode )
{
 int i;
 int status;
 struct kgpsa_sb *sb;
 MAILBOX mbx;

 switch( mode )
   {
   case DDB$K_STOP:
     if( pb->pb.mode != DDB$K_STOP ) {
	kgpsa_krn$_wait( &pb->fcp_owner_s, pb->pbx, "setmode STOP" );


       /*
	* Stop the periodic wake up of fc_process.
	*/
        krn$_stop_timer( &pb->fc.wake_t );


	/*
	 * Step thru the sb's skipping the 0th sb which is our adapter
	 */
        kgpsa_logout_all( pb );
 
	/* 
	 * should we wait for the logouts to be done?
	 * Wait R_A_TOV.
	 * should set the mask for no receive
	 */
	
	kgpsa_krn$_wait( &pb->els_owner_s, pb->pbx, "setmode STOP" );

	/*
	 * Take the lock on the adapter rings.  Without this I found that
	 * when the fc_process was woken up by the timer, it was still
	 * looking into the rings on the adapter.  On an uninitialized
	 * or initializing adapter it could find garbage.   I was okay
	 * before the periodic timer mechanism was added to fc_process.
	 * But with that new feature, I need to synchronize that process 
	 * with this start process.  
	 *
	 */
	kgpsa_krn$_wait( &pb->ring_s, pb->pbx, "setmode STOP" );

         /*
	  * Disable interrupts
	  */

	 if( pb->pb.mode == DDB$K_INTERRUPT ) 
             {
              io_disable_interrupts( pb, pb->pb.vector );
	      dprintf("interrupts have been disabled\n", p_args0);
             }
#if KGPSA_POLLED
         else
             {
	      pprintf("Removing from poll queue.\n");
	      remq_lock( &pb->pb.pq.flink );
             }
#endif
         io_issue_eoi( pb, pb->pb.vector );

         pb->pb.mode = DDB$K_STOP;

	 kgpsa_down_link( pb, &mbx );

	 /* 
	  * The adapter is not receiving anything now.  
	  * All the resources are back available to the port
	  * driver.
	  *
	  * Nobody should be sending anything out since we are STOP'ed
	  * Make sure of this.  I think I want those processes quieted
	  * down and not suspended over the stop-start.
	  *
	  * We have the FCP lock, so none of those are outstanding.
	  * No more ELSes are being issued since we do not issue anything
	  * to the adapter since kgpsa_ok_to_send check driver state and
	  * wont send anything out if we are DDB$K_STOPed.
	  *
	  * TODO: (optional - no problem seen as of 7-Nov-98)
	  * Dismiss anything pending on the active queue?  Or will
	  * the poll process take care of this for us.
	  * Do I need to find FCPs & ELSes that did not complete and
	  * deallocate them?
	  */

	/*
	 * Deallocate the buffers we gave the adapter
	 */
        kgpsa_reset_ring ( pb, &mbx, FC_ELS_RING );
        kgpsa_reset_ring ( pb, &mbx, 1 );
        kgpsa_reset_ring ( pb, &mbx, FC_FCP_RING );

        kgpsa_dealloc_rcv_bufs( pb, FC_ELS_RING );
     }

     kgpsa_dump_leak_queue();
     status = msg_success;
     break;

   case DDB$K_START:
     if( pb->pb.mode == DDB$K_STOP ) {
	pb->pb.mode = pb->pb.desired_mode;
	status = kgpsa_init_kgpsa( pb );
	if ( status != msg_success ) {
          pb->pb.mode = DDB$K_STOP;
        } 
	kgpsa_krn$_post( &pb->els_owner_s, pb->pbx, "setmode START" );
	kgpsa_krn$_post( &pb->fcp_owner_s, pb->pbx, "setmode_START" );
     }
     break;

   case DDB$K_READY:
     break;

 } /* switch */

 

}
#endif


#if (STARTSHUT||DRIVERSHUT)
int kgpsa_setmode( int mode, struct device *dev )
{
 int i;
 int status = msg_success;
 struct kgpsa_pb *pb;
 struct kgpsa_sb *sb;
 MAILBOX mbx;

 switch( mode )
   {
   case DDB$K_ASSIGN:
    dprintf("KGPSA_SETMODE ASSIGN  dev: %x\n", p_args1(dev) );
    kgpsa_ev_init();
    break;

   case DDB$K_STOP:
     dprintf("KGPSA_SETMODE STOP  dev: %x\n", p_args1(dev));
     pb = dev->devdep.io_device.devspecific2;
     if (!pb) 
        return msg_failure;

     /* If it was already shutdown, return */

     if (pb->pb.ref == 0)
	return msg_success;

     pb->pb.ref--;
     if (pb->pb.ref != 0)
	return msg_success;

     if( pb->pb.mode != DDB$K_STOP ) {
	/*
	 * The poll process may be running.  Wait for that to finish.
	 */

	krn$_delete(pb->poll.pid);
	krn$_bpost(&pb->poll.wake_s);
	krn$_wait(&pb->poll.comp_s);
	krn$_semrelease(&pb->poll.wake_s);
	krn$_semrelease(&pb->poll.comp_s);

	kgpsa_krn$_wait( &pb->fcp_owner_s, pb->pbx, "setmode STOP" );
	dprintf("kgpsa_setmode STOP - pb->node_count : %x \n", 
                                                p_args1(pb->node_count) );
	/*
	 * Step thru the sb's skipping the 0th sb which is our adapter
	 */
	for (i=1; i<pb->node_count; i++){
	  sb = pb->pb.sb[i];
          scsi_break_vc( sb );
        }
        kgpsa_logout_all( pb );

        /* 
         * should we wait for the logouts to be done?
         * Wait R_A_TOV.
         */
        
        /*
	 * Kill off the LATT process.  It might be in the middle of
	 * its probing and we cannot turn off interrupts while it is
	 * still issuing iocb or there would never be any response for 
	 * them.
	 */


	if( pb->pb.mode == DDB$K_INTERRUPT ) 
           {
            io_disable_interrupts( pb, pb->pb.vector );
           }
#if KGPSA_POLLED
        else
           {
            pprintf("Removing from poll queue.\n");
            remq_lock( &pb->pb.pq.flink );
           }
#endif
         io_issue_eoi( pb, pb->pb.vector );
  	
         pb->pb.mode = DDB$K_STOP;

         kgpsa_down_link( pb, &mbx );


	 /* 
	  * The adapter is not receiving anything now.  
	  * All the resources are back available to the port
	  * driver.
	  *
	  * Nobody should be sending anything out since we are STOP'ed
	  * Make sure of this.  I think I want those processes quieted
	  * down and not suspended over the stop-start.
	  *
	  * We have the FCP lock, so none of those are outstanding.
	  * No more ELSes are being issued since we do not issue anything
	  * to the adapter since kgpsa_ok_to_send check driver state and
	  * wont send anything out if we are DDB$K_STOPed.
	  *
	  * TODO: (optional - no problem seen as of 7-Nov-98)
	  * Dismiss anything pending on the active queue?  Or will
	  * the poll process take care of this for us.
	  * Do I need to find FCPs & ELSes that did not complete and
	  * deallocate them?
	  */



	/*
	 * Deallocate the buffers we gave the adapter
	 */
        kgpsa_reset_ring ( pb, &mbx, FC_ELS_RING );
        kgpsa_reset_ring ( pb, &mbx, 1 );
        kgpsa_reset_ring ( pb, &mbx, FC_FCP_RING );

        kgpsa_dealloc_rcv_bufs( pb, FC_ELS_RING );

	/*
	 * Delete the sb's
	 */
	 for (i=0; i<pb->node_count; i++) {
	   kgpsa_dealloc_sb( pb->pb.sb[i] );
	 }
	 pb->node_count = 0;
	 free(pb->pb.sb, "126*(sb)" );
         free(pb->scsi_targets, "scsi_targets");


	/*
	 * Kill off the supporting tasks
	 */

	krn$_delete(pb->fc.pid);
	krn$_post(&pb->fc.wake_t.sem);
	krn$_wait(&pb->fc.comp_s);
	krn$_stop_timer(&pb->fc.wake_t);
	krn$_release_timer(&pb->fc.wake_t);
	krn$_semrelease(&pb->fc.wake_t.sem);
	krn$_semrelease(&pb->fc.comp_s);

        int_vector_clear(pb->pb.vector);

        krn$_semrelease(&pb->els_owner_s);
        krn$_semrelease(&pb->fcp_owner_s);
        krn$_semrelease(&pb->ring_s);
        krn$_semrelease(&pb->poll_done_s);

#if RECORDING
	free( pb->slim_trace_start, "slimtrace" );
	if( pb->iocb_trace_start != 0xa00000 )
	    free( pb->iocb_trace_start, "iocbtrace" );
#endif


#if KGPSA_DEBUG
        pb->end_used_bytes = kgpsa_dyn$_status(&defzone, &header);

        pprintf("used bytes:  %d-%d = %d \n", pb->begin_used_bytes, 
                 pb->end_used_bytes, pb->end_used_bytes-pb->begin_used_bytes);
#endif

	free( pb,"pb" );
        (void *)dev->devdep.io_device.devspecific2 = 0;


#if LEAK_WATCH
        kgpsa_dump_leak_queue();
        krn$_semrelease( &kgpsa_leak_lock ); 
#endif

     }
     status = msg_success;
     break;

   case DDB$K_START:
     dprintf("KGPSA_SETMODE START  dev: %x\n", p_args1(dev) );

     if ( (void *)dev->devdep.io_device.devspecific2!=0){
       pb = dev->devdep.io_device.devspecific2;
       if ( pb && (pb->pb.ref != 0) ) {
         dprintf("kgpsa_setmode -  already STARTED\n", p_args0);
	 pb->pb.ref++;
         return msg_success;
       } 
     }

#if KGPSA_DEBUG
     pb->begin_used_bytes = kgpsa_dyn$_status(&defzone, &header);
#endif

     krn$_set_console_mode(INTERRUPT_MODE);

     scsi_shared_adr = ovly_load("SCSI");
     if (!scsi_shared_adr)
        return msg_failure;



#if LEAK_WATCH
#if RAWHIDE_APR16 || 1
  krn$_seminit( &kgpsa_leak_lock, 1, "leak_lock" );
  kgpsa_leak_q.flink = &kgpsa_leak_q;
  kgpsa_leak_q.blink = &kgpsa_leak_q;
#endif
#endif

     if ( (void *)dev->devdep.io_device.devspecific2==0){
       kgpsa_init(dev);
       pb = dev->devdep.io_device.devspecific2;
     } else {
       pb = dev->devdep.io_device.devspecific2;
       kgpsa_init_port( pb );
     }
     pb->pb.ref = 1;

     status = msg_success;
     break;

   case DDB$K_READY:
     pprintf("KGPSA_SETMODE READY  dev: %x\n", p_args1(dev) );
     pb = dev->devdep.io_device.devspecific2;
     if( pb->pb.mode != DDB$K_STOP ) {
	while( pb->poll_active )   krn$_sleep( 100 );
     }
     status = msg_success;
     break;

 } /* switch */


 return status;
}
#endif



void kgpsa_poll_process( struct kgpsa_pb *pb, int poll )
{
 struct PCB  *pcb = getpcb();
    while(1) 
      {
        kgpsa_krn$_wait( &pb->poll.wake_s, pb->pbx, "poll_process" );
        if (pcb->pcb$l_killpending & SIGNAL_KILL) {
          dprintf("poll process -- ahhhhh - I'm dying.....\n", p_args0);
          return;
        }
#if (STARTSHUT||DRIVERSHUT)
	if(diagnostic_mode_flag)
#endif
           kgpsa_poll( pb, poll, VERBOSE );
      }
}

/*+
 * ============================================================================
 * = kgpsa_poll - poll                                                        =
 * ============================================================================
 *
-*/
void kgpsa_poll( struct kgpsa_pb *pb, int poll, int verbose )
{
 int i;
 char portname[20];
 struct kgpsa_sb *sb;
 FC_BRD_INFO *binfo;

 binfo = &pb->binfo;

 dprintf("kgpsa_poll - node count: %x  LINKUP: %x  RSCN: %x\n",
              p_args3(pb->node_count, pb->LINKUPeventTag, pb->RSCNeventTag) );
 for( i = 0; i < pb->node_count; i++ )
   {
    if (!pb->pb.sb[i]) continue;
    sb = pb->pb.sb[i];
    /*    REMEMBER TO NOT POLL MYSELF   */
    if( sb->did  == binfo->fc_myDID ) {
      continue;
    }

    /*
     * See FCLOG 99.0, Jan 13, 98.
     * This code was removed because we want device to be removed from
     * the show dev list if it is no longer available.   I wanted to put
     * this code here to save on the erroring out of the scsi inquiry
     * commands at the port level, but we really need to go thru scsi_break_vc.
     * The kgpsa_command (and down) is the correct place to do this type of
     * check, because the failure from kgpsa_command can trickle back up
     * to result in a break_vc.
     *
     * TODO: (medium)
     * Unfortunately we will continue to poll nports that have gone away.
     * Nothing will go out on the wire since sb->valid will be zero for 
     * nports that have gone away, but currently (7-nov-98) we would wait
     * in kgpsa_command for it to become valid.   This policy could be 
     * improved.  
     *
     *if (sb->nstate != NLP_LOGIN) {
     * dprintf(" %s %s  skipping\n", 
     *                 p_args2(sb->portname, nstate_text[sb->nstate]) );
     * continue;
     *}
     */

    /*
     * The PRLI information indicated to us if the n_port supported 
     * scsi_target mode.  This check is relevant during wwidmgr_mode
     * because we are probing all sb's that we log into.   When we are
     * not in wwidmgr_mode, then our sb list only contains n_ports that
     * have been pre-registered by prior wwidmgr setup.  The user would
     * only put scsi targets in the list.
     *
     * TODO: (medium priority / 09-apr-99)
     *   If an sb changes its capability, that is, if it used to be a
     *   scsi target, but then changes to not be a scsi target, then
     *   we would need some way to get rid of any disks that might have
     *   been associated with that sb.  Typically we get rid of scsi
     *   disk when a scsi_break_vc is called, but the path to having
     *   that routine called has scsi commands issued to the sb.  We do
     *   not want to have scsi commands issued to an sb that does not
     *   have scsi capability.
     *
     *   Here is the case.  If UNIX/VMS is up and running and reports
     *   that it is a scsi target, and if they report serving disks (which
     *   they do not do today), then when it goes back into console mode,
     *   console would report it is not scsi target capable.  Another console
     *   on the link running wwidmgr would then skip doing the polling to
     *   the target, and would not eliminate the disks that used to be
     *   served under the os.  
     *
     *   Also, wwidmgr would issue scsi commands directed at stale disks
     *   on targets that no longer support scsi commands.  
     *   
     *      System A                         System B
     *    --------------------           ------------------
     *     wwidmgr mode                           OS - serves dga2
     *       (thinks B is scsi capable)
     *                                    now goes into wwidmgr mode
     *     sb->scsi_target set to 0        and is no longer a scsi target
     *       but only checked at inquiry
     *
     *     no inquiry done, dga2 is thought
     *       to still be valid.
     *
     *     another wwidmgr cmd issued to dga2    This is a protocol violation!
     *     would send scsi cmd to B.             Console used to crash but
     *                                           no rejects scsi command. 
     *     dga2 would go away with the failure   However, we want to avoid
     *       of scsi commands to it.             this.
     *
     *  RECOMMENDATION:
     *    Need to have the kgpsa_fcp_cmd code check the current status
     *    of sb->scsi_target and fail scsi commands directed at sb's
     *    that are no longer targets.   This would allow us to remove this
     *    check below where we avoid scsi_send_inquiry.   In the new
     *    method we would always call scsi_send_inquiry, but we would end
     *    up failing the scsi command at the kgpsa_command level.
     *    My only concern is that we would be doing a lot of retries 
     *    initiated from the scsi class driver.  If they happen quickly
     *    then that may be acceptable.  If they have delays, then we
     *    should find some other way to break the vc's and void the stale
     *    disks.
     */
    if (!sb->scsi_target) {
      dprintf("kgpsa_poll: skipping %x; does not do scsi target mode\n",
                                                          p_args1(sb->did) );
      continue;
    }


#if (STARTSHUT||DRIVERSHUT)
    if( !wwidmgr_mode )
      sb->verbose = verbose;
    else
      sb->verbose = NO_VERBOSE;
#endif

    dprintf("kgpsa polling sb %x\n", p_args1( sb ) );

    if( poll || scsi_poll ) scsi_send_inquiry( sb );

  } /* for */

  pb->poll_active = 0;
  kgpsa_krn$_bpost( &pb->poll_done_s, pb->pbx, "poller" );
}



/*+
 * ============================================================================
 * = kgpsa_connection_helper -                                                =
 * ============================================================================
 *
-*/
int kgpsa_connection_helper( struct kgpsa_pb *pb , int verbose )
{
 int status;
 struct TIMERQ *tq, tqe;
 tq = &tqe;

#if LOOP_NOT_SUPPORTED
 if( pb->topology == FLAGS_TOPOLOGY_MODE_LOOP )
    return msg_success; 
#endif


 /*
  * If the link has been down for a while, give up right away.
  */
 if( pb->linkdownintvcnt <= 0 )
    {
    pprintf("Link is down.\n", p_args0 );
    return msg_failure;
    }
  /*
   * Wait up to 5 seconds for link to come up
   */
 status = msg_success;
 start_timeout(5000,tq);   
 while ( ( pb->attType != AT_LINK_UP ) && (tq->sem.count==0)  ) {
   krn$_sleep(10);
 }
 if (tq->sem.count) {
     status = msg_failure;
     pprintf("Link is down.\n",p_args0);
 }
 stop_timeout(tq);
 if (status!=msg_success){
     return msg_failure;
 }

  /*
   * Wait up to 20 seconds for probe to complete
   */
 status = msg_success;
 start_timeout(20000,tq);   
 while ( ( pb->devState != DEV_READY ) && (tq->sem.count==0)  ) {
   krn$_sleep(10);
 }
 if (tq->sem.count) {
     status = msg_failure;
     pprintf("Probing timeout.\n",p_args0);
 }
 stop_timeout(tq);
 if (status==msg_success){

#if (STARTSHUT || DRIVERSHUT)
   kgpsa_poll(pb,1, verbose);
#else
   pb->poll_done_s.count = 0;
   krn$_post( &pb->poll.wake_s );
   krn$_wait( &pb->poll_done_s );
#endif
 }

 return msg_success;

}


#if (STARTSHUT || DRIVERSHUT)

void kgpsa_poll_units( struct device *dev, int verbose )
{
  int status;
  int local_verbose;
  struct kgpsa_pb *pb;

  /*
   * In diagnostic mode, the poll process is enabled to 
   * display devices.   There is a small problem with this.
   * The prompt comes back before the devices are displayed.
   * There is a sort of nice side effect to running the poll
   * process with verbose on, and that is, as new device come
   * online, they get displayed automatically.
   *
   * If there is pushback to how this operates then a semaphore
   * will have to be created to synchronize polling operation.
   * The root of the problem is that sb->verbose is not synchro-
   * nized and the kgpsa_poll_units and the poll process change it.
   * Another thing that could change is the way polling is done.
   * This is a non-conventional driver in that it runs a poll
   * process.   It does not have to do that, but it will have the
   * limitations that all STARTSHUT drivers do that have a
   * diagnostic mode.
   *
   * However, if we are in wwidmgr_mode, then we are managing probes
   * by hand (at least at wwidmgr initialization time), then we procede
   * here.   
   *
   */
  local_verbose = VERBOSE;
  if( wwidmgr_mode )
     local_verbose = NO_VERBOSE;
  else
     if(diagnostic_mode_flag)
        return;


  pb = dev->devdep.io_device.devspecific2;

  printf("%s\n", pb->pb.string );

  kgpsa_connection_helper( pb, local_verbose );


}




/*+
 * ============================================================================
 * = kgpsa_establish_connection -                                             =
 * ============================================================================
 *
-*/
int kgpsa_establish_connection(struct INODE *inode, struct device *dev, int class)
{

  int status;
  struct kgpsa_pb *pb;
  struct TIMERQ *tq, tqe;
  tq = &tqe;
  pb = dev->devdep.io_device.devspecific2;

  status = kgpsa_connection_helper( pb, NO_VERBOSE );

  return status;
}
#endif



#if KGPSA_POLLED
/*+
 * ============================================================================
 * = kgpsa_poll_for_interrupt - poll for port interrupts                      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      This routine polls for port interrupts.  If an interrupt is pending,
 *      the ISR is called.
 *  
 * FORM OF CALL:
 *  
 *      kgpsa_poll_for_interrupt( pb )
 *  
 * RETURNS:
 *
 *      Nothing.
 *       
 * ARGUMENTS:
 *
 *      struct kgpsa_pb *pb              - port block
 *
 * SIDE EFFECTS:
 *
 *      The ISR may be called.
 *
-*/
void kgpsa_poll_for_interrupt( struct kgpsa_pb *pb )
    {
    if( pb->pb.mode == DDB$K_POLLED )
        if( rl(HA) & (HA_LATT|HA_R0ATT|HA_R2ATT|HA_MBATT) )
            kgpsa_interrupt( pb );
    }
#endif  /* KGPSA_POLLED */



/*+
 * ============================================================================
 * = kgpsa_interrupt - handle the interrupts                                  =
 * ============================================================================
 *
-*/
void kgpsa_interrupt( struct kgpsa_pb *pb )
{
 int i;
 ulong   ha_copy, hc_copy;
 MAILBOX     lmb, *mb=&lmb;
 FC_BRD_INFO *binfo;
 struct kgpsa_sb *sb;
 int do_post = 0;

 ha_copy = rl(HA);
 hc_copy = rl(HC);
 dprintf("kgpsa_interrupt - HA:%x  HC:%x\n", p_args2(ha_copy, hc_copy) );

 wl(HC, hc_copy&( ~(HC_LAINT_ENA|HC_R0INT_ENA|HC_R2INT_ENA) ) ); 


 /*
  * Allow interrupt to come in again
  */
 if( pb->pb.mode == DDB$K_INTERRUPT )
     io_issue_eoi( pb, pb->pb.vector );


 /*
  *  MBX interrupt
  *
  * if its a mailbox interrupt, clear it out and reenable it
  */
  if (ha_copy & HA_MBATT){
    wl(HA,HA_MBATT);
    
  }


 /*
  *  LATT interrupt
  *  I put in the check for the HC_LAINT_ENA because at initial discovery
  *  while ELS's are interrupting, we sometimes came in here and handled
  *  a LATT that we did not want to handle yet.  Maybe I can change this
  *  to be goverened by devState.
  */

  if (ha_copy & HA_LATT) {
    wl(HA,HA_LATT);  /* Clear LATT interrupt */
    /*
     * Since this console port driver implementation does not support PDISC,
     * all connections and all outstanding iocbs will get aborted via the
     * unreg_login.  FCPs will get bounce with a link error until a clear_la.
     */
    kgpsa_unreg_login( pb, 0xFFFF );
    kgpsa_read_la ( pb, mb );
    dprintf("HA_LATT interrupt ", p_args0);
    kgpsa_dump_csrs( pb->pbx, (kgpsa_debug_flag&1) );
    kgpsa_set_devState( pb, DEV_PROBE_NEEDED, "ISR" );
    dprintf("Link event %x\n", p_args1(mb->un.varReadLA.eventTag) );
    dprintf("[1]%s %s  originating L_PORT:%x\n",
               p_args3(   latt_type[mb->un.varReadLA.attType],
                          lip_type[mb->un.varReadLA.lipType],
			  mb->un.varReadLA.lipAlPs
                      )  );
    pb->attType = mb->un.varReadLA.attType;

    kgpsa_record_latt( pb, pb->attType, mb->un.varReadLA.eventTag, pb->RSCNeventTag );

    if(mb->un.varReadLA.eventTag == 0) {
      dprintf("Result of an INIT_LINK\n", p_args0);
      /*
       * The eventTag returned from the adapter is initialized 
       * by the init_link, so port driver tracking must reinitialize
       */
       pb->LINKUPeventTag = 0;
    }

   
  if ( pb->attType == AT_LINK_UP ) {
     pb->LINKUPeventTag = mb->un.varReadLA.eventTag;
     binfo = &pb->binfo;
     if (binfo->fc_myDID != mb->un.varReadLA.granted_AL_PA) {
       dprintf("MY ALPA changed from %x to %x \n",
                   p_args2(binfo->fc_myDID, mb->un.varReadLA.granted_AL_PA) );
       binfo->fc_myDID = mb->un.varReadLA.granted_AL_PA;
       sb = pb->pb.sb[0];
       sb->did = binfo->fc_myDID;
       kgpsa_config_link( pb, mb ); 
     }
     pb->discovery_index = 0; /* reset the loop discovery index */
#if KGPSA_DEBUG
     dprintf("kgpsa_interrupt - LINK UP event %x RSCN:%x - post fc process\n", 
                      p_args2(pb->LINKUPeventTag, pb->RSCNeventTag) );
#endif
     do_post = 1;

  } else {

     pb->LINKDOWNeventTag = mb->un.varReadLA.eventTag;
#if KGPSA_DEBUG
     dprintf("kgpsa_interrupt - LINK UP event %x"
             " LINK DOWN: %x RSCN:%x - post fc process\n", 
        p_args3(pb->LINKUPeventTag, pb->LINKDOWNeventTag, pb->RSCNeventTag) );
#endif
  } /* if pb->attType */

} /* LATT */



 /*
  *  Ring 0 is an ELS
  */
  if (ha_copy & HA_R0ATT){
    dprintf("HA_R0ATT interrupt\n",p_args0);
    wl(HA,HA_R0ATT);  /*  Need to do something about this occurance */
    do_post = 1;
  } /* R0ATT */


 /*
  *  Ring 2 is an FCP
  */
  if (ha_copy & HA_R2ATT){
    wl(HA,HA_R2ATT);  /*  Need to do something about this occurance */
    dprintf("HA_R2ATT interrupt\n",p_args0);
    do_post = 1;
  } /* R2ATT */

  if(do_post)
     kgpsa_krn$_bpost( &pb->fc.wake_t.sem, pb->pbx, "intr rtn" );

  wl(HC, hc_copy); 

 return;
}




/*+
 * ============================================================================
 * = kgpsa_fc_process - process the received iocbs                            =
 * ============================================================================
 *
 * Wait for the ISR to post the receive semaphore.  This indicates
 * that a new response is available.  This process will look at the
 * ELS receive ring and the FCP receive ring.  
 *
 * There is no in-memory receive_q (analygous to pending_q).  The 
 * IOCB rsp ring is used to store the receive iocbs.  One IOCB rsp
 * is read off the adapter at a time (as opposed to sucking in 
 * all available responses).   The adapter can store quite a few IOCBs,
 * and for a console implementation, there should not be a backlog
 * of incoming requests that would overrun the adapter and cause an
 * ABTS.  Certainly not in the case of FCP operations, where console is
 * only an initiator and can control the flow to a single SCSI command.
 *
 * This routine get posted either because and ELS was received, or an
 * FCP was received, or both.  The code does not try to determine which
 * one of those it was.  It just goes out to both receive rings and will
 * handle iocb's if they are there, and if not then nothing.   The only
 * annoyance with that is when tracing, it clutters up the trace to read
 * an iocb that we did not have to.
 *
 * The ELSes get the priority.  There can be many received ELS iocbs,
 * but only one FCP response at this time.  So I do all the ELSes first.
 *
 * A nice enhancement I would like to make someday is to get rid of the 
 * latt_process and run its duties out of here. 
 *
-*/

void kgpsa_fc_process( struct kgpsa_pb *pb )
{
 int i;
 int doffset;
 int status;
 int semval;
 IOCB *iocb_rsp;
 struct kgpsa_sb *sb;
 MAILBOX mbx, *mb=&mbx;

 struct PCB  *pcb = getpcb();

 int lattcnt=0, r0cnt=0, r2cnt=0;

 iocb_rsp = malloc(sizeof(IOCB), "iocb_rsp");


 while( 1 )
   {
     semval = kgpsa_krn$_wait( &pb->fc.wake_t.sem, pb->pbx, "fc_process" );

     if(semval & TIMEOUT$K_SEM_VAL) 
       {
         krn$_start_timer( &pb->fc.wake_t, fc_wake_interval );
         if( pb->attType == AT_LINK_DOWN )
           {
	     --pb->linkdownintvcnt;
	     dprintf("pb->linkdownintvcnt: %x\n", p_args1(pb->linkdownintvcnt) );
	     if( pb->linkdownintvcnt == 0 )
               {
                printf("%s Link is down.\n", pb->pb.name );

                 /*
                  * Void out all sb's and kick off a device probe to 
                  * make the devices invalid and removed from show dev.
                  */
                 for(i=0; i<pb->node_count; i++)
		    {
                     if (!pb->pb.sb[i]) continue;
                     sb = pb->pb.sb[i];
		     sb->valid = 0;
		    }
                 kgpsa_krn$_bpost( &pb->poll.wake_s, pb->pbx, "poll rtn" );
	       }
	   } 
         else 
           {
             pb->linkdownintvcnt = kgpsa_down_int_cnt;
	   }


       /*
        * TODO:  (optional)
	*	 Here is where the active_q of IOCBs can be scanned and
	*        each IOCBHDR can have a counter field which can be
	*        incremented.  After so many increments, we can call out   
        *        a lost iocb. 
        */

       /*
        *        Also, we can time an outstanding CT.  Since
        *        the CT data comes back asynchronously, it may never come
	*        back at all.  So the state machine would never advance.
        *        Its okay for a CT to never come back if a link transition
	*        occurred and a new CT was issued.  In order to know if
	*        a CT got lost and should be reissued,  a counter runs
	*        while a CT is outstanding.
	*/


       --pb->CT_late_intvcnt;
       if( pb->CT_late_intvcnt == 0 )
         { 
	  err_printf("%s - lost CT\n.", pb->pb.name );
	  kgpsa_init_link( pb, mb, DONT_WAIT );
         }

       }

    lattcnt = r0cnt = r2cnt = 0;


    if (pcb->pcb$l_killpending & SIGNAL_KILL) {
      dprintf("fc process -- ahhhhh - I'm dying.....\n", p_args0);
      free( iocb_rsp, "iocb_rsp" );
      return;
    }



   kgpsa_krn$_wait( &pb->els_owner_s, pb->pbx, "els_cmd");

/*
 *  LOOP: This is good for the loop
 */
  if(pb->topology==FLAGS_TOPOLOGY_MODE_LOOP)
    {

#if LOOP_NOT_SUPPORTED
        {
	  /* Do nothing */
        }
#else


    if ( (pb->LINKUPeventTag > pb->DiscLINKUPeventTag) ||
         (pb->RSCNeventTag   > pb->DiscRSCNeventTag  ) ){
      /*
       * TODO: (optional)
       * Link may be down.  If the link went down/up/down the LINKupeventTag
       * would be greater than the DiscStarteventTag, and we would start
       * probing even if the link were down.  But do we really need to do
       * anything about it since the link could go down at any moment, and
       * I would expect the ELS probing to just trickle through.  We could
       * check with this latest read_la info the up/down state of the link.
       */
      kgpsa_set_DiscState( pb, DISCOVERY_NEEDED, "fc_process (loop)" );
      pb->DiscLINKUPeventTag = pb->LINKUPeventTag;
      dprintf("fc_process - DiscLINKUPeventTag: %x \n", 
                                          p_args1( pb->DiscLINKUPeventTag ) );
      memset( pb->scsi_targets, 0, CT_BUF_SZ );
      pb->scsi_targets_cnt = sizeof(staticAlpaArray);
      for( i=0; i<sizeof(staticAlpaArray); i++)
         {
	   pb->scsi_targets[i] = staticAlpaArray[i];
         }

	

      probe_state( pb, PROBE_SET, PROBE_IN_PROGRESS, ALL_DID );
      kgpsa_set_DiscState( pb, DISCOVERY_IN_PROGRESS, "fc_process" );

      lattcnt++;
      pb->discovery_index = 0;
      for(i=0; i < kgthrottle; i++)
        {
        kgpsa_els_cmd( pb, pb->scsi_targets[i], ELS_CMD_PLOGI, kgpsa_wait, fc_els_retries );
        }
    }

#endif

  }


/*
 * END LOOP: 
 */


/*
 * FABRIC:
 */
  if(pb->topology==FLAGS_TOPOLOGY_MODE_PT_PT)
    {
      if ( pb->LINKUPeventTag > pb->DiscLINKUPeventTag ){
        pb->DiscLINKUPeventTag = pb->LINKUPeventTag;
        pb->DiscRSCNeventTag   = pb->RSCNeventTag;
#if KGPSA_DEBUG
        dprintf("fc_process - PT_PT - RSCN: %x/%x  LINKUP: %x/%x \n",
               p_args4(pb->RSCNeventTag, pb->DiscRSCNeventTag, 
                            pb->LINKUPeventTag, pb->DiscLINKUPeventTag ) );
#endif
  
        kgpsa_set_DiscState( pb, DISCOVERY_NEEDED, "fc_process (pt2pt)" );
        probe_state( pb, PROBE_SET, PROBE_IN_PROGRESS, ALL_DID );
        kgpsa_fabric_login(pb);
      }
    }


/*
 * END FABRIC:
 */

    status = kgpsa_receive( pb,  FC_ELS_RING, iocb_rsp, &doffset );
    while ( status == msg_success ) {
      r0cnt++;
      kgpsa_els_response( pb, iocb_rsp, doffset );
      status = kgpsa_receive( pb, FC_ELS_RING, iocb_rsp, &doffset );
    }


    /*
     * TODO: (optional) 
     * Need a timeout  ???? Hmmm.  If an ELS does not ever return
     * then probe_state will never get to PROBE_DONE.  This may be too harsh.
     * How can I get back into this process if a probing timer went out and
     * I wish to continue anyway.   
     */


    if ( 
         ( probe_state(pb, PROBE_GET, 0, ALL_DID) == PROBE_DONE ) &&
	 ( pb->DiscState == DISCOVERY_IN_PROGRESS )               && 
         ( pb->DiscLINKUPeventTag == pb->LINKUPeventTag )         &&
         ( pb->DiscRSCNeventTag == pb->RSCNeventTag )             &&
         ( pb->LINKUPeventTag > pb->LINKDOWNeventTag )
       )
    {
    dprintf("fc_process - probing done for LINKUP: %x  RSCN: %x\n", 
                      p_args2(pb->DiscLINKUPeventTag, pb->DiscRSCNeventTag ) );

    /*dn();*/

    kgpsa_set_DiscState( pb, DISCOVERY_DONE, "fc_process" );

    /*
     * clear_la now sets DEV_STATE 
     */
    status = kgpsa_clear_la ( pb, mb, pb->DiscLINKUPeventTag ); 

    /*
     * At this point, we are done with any log in attempts to any n_ports.
     * If the n_ports did not respond to any of our PLOGIs during discovery
     * then they wont be seen, nor their devices probed.   Even if they
     * come in late, after this port driver is done probing, they will not
     * get used by this driver because this implementation does not accepT
     * PLOGIs that are initiated from a remote n_port.  This implementation
     * only accepts PLOGIs that is has initiated.  (This is the restriction
     * the NT driver made, so it is good enough for me.)  If a slow pokey
     * n_port starts participating long after the LIP or RSCN, it may send
     * out a PLOGI long after my probing is done.  But I will not service
     * that PLOGI.  This may sound harsh, but I have never seen this occur
     * in any testing.  It wouldn't be all that hard to put in code to handle
     * this case - when a PLOGI is receive when DevState is READY, then
     * initiate a PLOGI to it.   When we get logged in, then a mechanism would
     * have to be invented to kick off a scsi poll of that n_port.   But
     * this code does not do that at this time (9-Nov-98).
     *
     *
     * A new node may have entered the fabric or loop this past probe.
     * We need to probe all its devices.   No harm probing the whole world
     * again so I start up a kgpsa_poll.
     */
    if( status == msg_success )
      {
       dprintf("Posting the poll process\n", p_args0);
       kgpsa_krn$_bpost( &pb->poll.wake_s, pb->pbx, "poll rtn" );
      }
    }


    status = kgpsa_receive( pb, FC_FCP_RING, iocb_rsp, &doffset );
    while (status == msg_success) {
      r2cnt++;
      kgpsa_fcp_response( pb, iocb_rsp, doffset );
      status = kgpsa_receive( pb, FC_FCP_RING, iocb_rsp, &doffset );
   }

   if ( lattcnt+r0cnt+r2cnt == 0 ) 
     {
       dprintf("Stacked posts of fc_process\n", p_args0);
     }

      kgpsa_krn$_post( &pb->els_owner_s, pb->pbx, "els_cmd");

 } /* while(1) */


}



/*+
 * ============================================================================
 * = els_cmd - send an ELS from the command line                              =
 * ============================================================================
 *
 *  This is a debug routine.  But I like it so much I am keeping it around
 *  even in the optimized builds.  From the command line, you can issue
 *  an ELS, say a PLOGI, by
 *
 *               >>> call els_cmd 123450 4 3
 *                                ^      ^ ^
 *                                |      | |
 *                                |      | +--- PLOGI
 *                                |      +--- did 
 *                                +--- pb
-*/
void els_cmd( struct kgpsa_pb *pb, int did, int type, int retries )
{
  pprintf("Issueing els iotag: %x \n", pb->elscmdcnt );
  kgpsa_els_cmd( pb, did, type, DONT_WAIT, retries ); /* no retry */
}



/*+
 * ============================================================================
 * = kgpsa_els_response - handle a received ELS                               =
 * ============================================================================
 *
 * Process the iocb's coming off the iocb rsp queue in the slim.
 *
 * Some of these are generated by remote nodes requesting things.
 *   CMD_RCV_ELS_REQ_CX:
 *      Link service commands initiated by the remote N_PORT come in
 *      as this type.  They can be PDISC/PLOGI/LOGO/PRLI/PRLO.
 *
 * Some are generated by the local adapter giving information or buf request
 *   CMD_RCV_SEQUENCE_CX
 *      If the adapter receives a buffer off the wire and has no queue
 *      buffer to recieve the data, the CMD_RCV_SEQUENCE_CX is generated
 *      and time is allowed for the adapter to respond by queueing a buffer.
 *
 * Some are responses to requests sent out by the port driver link services.
 *    CMD_ELS_REQUEST_CX
 *       Responses to port driver commands of PDISC/PLOGI/PRLI/LOGO/PRLO
 *          
 * INCARNERR - The iocb_state is set to INCARNERR when the iocb_hdr
 *   indicates that the iocb was issued prior to a loop initialization.
 *   If the loop has initialized since we sent out the ELS, then the results
 *   that come back may be erroneous.   For example, if a LIP occurs during
 *   loop probing, and PLOGI response is in the adapter yet unread from 
 *   before the LIP, it may indicate a DID-to-WWN relationship that is no
 *   longer valid.   This was a particularly prickly situation when the console
 *   used to support PDISCs and PLOGIs from remote n_ports.  The incarnation
 *   solved problems there.   Now that console does not support PDISCs and
 *   received PLOGIs, a new probe will always occur after a LIP, and all
 *   the latest parameters are gathered for a LIP, and no harm would be
 *   be done processing stale received iocbs.   I leave the code in since
 *   it still shortcuts some work, and mostly because the PDISC and remote
 *   PLOGI requirements may be back some day.
-*/

void kgpsa_els_response( struct kgpsa_pb *pb, IOCB *iocb_rsp, int doffset )
{
 IOCB_HDR *iocbhdr;
 struct kgpsa_sb *sb;

 kgpsa_trace_iocb( "R ELS  R ", doffset, iocb_rsp,  (kgpsa_trace_iocb_doit&1) );
 /*
  * For commands that return an IOTAG, do the work of finding it
  * up front.
  */
 if ( (iocb_rsp->ulpCommand == CMD_XMIT_ELS_RSP_CX) ||
      (iocb_rsp->ulpCommand == CMD_ELS_REQUEST_CX ) ||
      (iocb_rsp->ulpCommand == CMD_ELS_REQUEST_CR ) ||
      (iocb_rsp->ulpCommand == CMD_ABORT_XRI_CN   ) ||
      (iocb_rsp->ulpCommand == CMD_CLOSE_XRI_CX   ) ||
      (iocb_rsp->ulpCommand == CMD_ABORT_XRI_CX   ) ||
      (iocb_rsp->ulpCommand == CMD_XMIT_SEQUENCE_CX)||
      (iocb_rsp->ulpCommand == CMD_GET_RPI_CR     ) ){
   iocbhdr = kgpsa_find_iocbhdr( pb, IOCB_ACTIVE, FC_ELS_RING,
                                             iocb_rsp->un1.t1.ulpIoTag, 1);
   if (iocbhdr) {
     memcpy( &iocbhdr->rsp_iocb, iocb_rsp, sizeof(IOCB) );
     remq_lock(&iocbhdr->iocb_q.flink);
     if ( (iocbhdr->LINKUPeventTag != pb->LINKUPeventTag ) ||
          (iocbhdr->RSCNeventTag   != pb->RSCNeventTag   ) ){
#if KGPSA_DEBUG
	d2printf("INCARNERR: iocbhdr->LINKUPeventTag: %x"
                          "  pb->LINKUPeventTag: %x"
			  "  iocbhdr->RSCNeventTag: %x"
			  "  pb->RSCNeventTag: %x"
                          " iotag:%x\n",
        p_args5(iocbhdr->LINKUPeventTag, pb->LINKUPeventTag,
             iocbhdr->RSCNeventTag, pb->RSCNeventTag, 
              iocb_rsp->un1.t1.ulpIoTag) );
#endif
        iocbhdr->iocb_state = IOCB_INCARNERR;
     }
     if ((iocbhdr->cmd_iocb.un.cont[0].bdeAddress !=
                               iocbhdr->rsp_iocb.un.cont[0].bdeAddress) ||
         (iocbhdr->cmd_iocb.un.cont[1].bdeAddress !=
                               iocbhdr->rsp_iocb.un.cont[1].bdeAddress) ){
        pprintf("HERE  iocbhdr:%x  iocb_rsp:%x\n", p_args2(iocbhdr, iocb_rsp));
        do_bpt();
     }
   }
 } else {
   iocbhdr = 0;
 }

 /*
  * We don't handle continuation iocb's.  They should never happen.
  * If it does, disregard it.
  */
 if( !iocb_rsp->ulpLe ) {
   pprintf("\nELS ulpLe set  - disregarding \n");
   kgpsa_dump_iocb_trace( pb, pb->iocb_trace_currentry-3, 4 , 1);
   goto EXIT;
 }

 switch (iocb_rsp->ulpCommand)  {
   /*
    *  Unsolicited requests.  These wont have an associated iocbhdr
    *  on the active queue, so there is no need to delete a header.
    *  However, they will have buffers allocated off the pool and those
    *  should be replaced.
    *     CMD_RCV_SEQUENCE_CX 
    *     CMD_RCV_ELS_REQ_CX
    */
   case CMD_RCV_SEQUENCE_CX:
     /*
      * No long supported.  See older console if this needs to be resurrected.
      * The only thing we ever saw from here was the RCV_BUFFER_WAITING.
      */
     break;
   case CMD_RCV_ELS_REQ_CX:
     kgpsa_els_rcv( pb, iocb_rsp );
     break; 


   /*
    *  The cases below are received as responses to something
    *  that the port driver has sent out.  The buffers the adapter
    *  used were allocated along with the command (as bde's) in
    *  the originally issued IOCB.
    *     CMD_XMIT_ELS_RSP_CX
    *     CMD_ELS_REQUEST_CR
    *     CMD_ELS_REQUEST_CX
    *     CMD_ABORT_XRI_CX
    *     CMD_CLOSE_XRI_CX
    *     CMD_GET_RPI_CR
    *     CMD_ABORT_XRI_CN
    *     CMD_CLOSE_XRI_CR
    */

    /*
     *  CMD_XMIT_ELS_RSP_CX - This command is used to send the ELS response
     *  payload to an ELS request received with an RCV_ELS_REQ_CX IOCB response.
     *  These are the rsp IOCBs to the commands issued by kgpsa_els_rsp.
     *  Responses to requests I satisfied.  These all should have 
     *  a iocbhdr on the active queue.   The _done routines clean
     *  up the one BDE buffer that was malloced by kgpsa_els_rsp.
     */
    case CMD_XMIT_ELS_RSP_CX:
      dprintf("received on ELS - CMD_XMIT_ELS_RSP_CX\n", p_args0);
      if (iocbhdr != NULL) {
        kgpsa_els_rsp_done( pb, iocbhdr );
      } else {
        dprintf("No header found for XMIT_ELS_RSP_CX\n", p_args0);
      }
      break;


    /*
     *  The CMD_ELS_REQUEST_CX are process generated.  We post
     *  the waiting semaphore notifying that process.  The iocbhdr
     *  is removed from the active queue, but it is not ours to delete.
     *  The notified process will do that.  The CMD_ELS_REQUEST_CR is
     *  always a failure case where the adapter is spitting back the
     *  request without taking any action.  The _CR should always 
     *  have IOSTAT_LOCAL_REJECT.  And I have seen that with 
     *  IOERR_NO_RESOURCES.
     */
    case CMD_ELS_REQUEST_CR:               
    case CMD_ELS_REQUEST_CX:
      if (iocbhdr != NULL) {
        if (iocbhdr->wait_for_completion) {
          kgpsa_krn$_post( &iocbhdr->rsp, pb->pbx, "found IOCB" );
        } else {
	  kgpsa_els_cmd_done( pb, iocbhdr );
	}
      } else {
        dprintf("Unexpected ELS_REQUEST IOCB received.  For now throw it away.\n", p_args0 );
        kgpsa_trace_iocb( "UNEXP ELS  R ", 99, iocb_rsp, DO_IT );
        /*dump_els();*/
      }
      break;



    case CMD_ABORT_XRI_CX:
      if ( iocbhdr != NULL ) {
        kgpsa_els_abort_done( pb, iocbhdr );
      } else {
        dprintf("No header found for ABORT_XRI_CX\n", p_args0);
      }
      break;

#if ABORTS
    /*
     * We worked on these at one time, but we do not support these
     */
    case CMD_CLOSE_XRI_CX:
      if ( iocbhdr != NULL ) {
        kgpsa_els_abort_done( pb, iocbhdr );
      } else {
        dprintf("No header found for ABORT_XRI_CX\n", p_args0);
      }
      break;

    case CMD_GET_RPI_CR:
      dprintf("received on ELS - CMD_GET_RPI_CR\n", p_args0);
      if (iocbhdr != NULL) {
        /*kgpsa_get_rpi_done( pb, iocbhdr ); */
      } else {
        dprintf("No header found for XMIT_ELS_RSP_CX\n", p_args0);
      }
      break;

     case CMD_ABORT_XRI_CN:
     case CMD_CLOSE_XRI_CR:
       dprintf("ABORT FAILED\n", p_args0 );
       break;
#endif

     default:
       d2printf("ELS rcv default.  command = %x\n", p_args1(iocb_rsp->ulpCommand) );
       kgpsa_trace_iocb( "ELS rcv ", 99, iocb_rsp, SKIP_IT );
       if(kgpsa_debug_flag&0x4)
         kgpsa_dump_iocb_trace( pb, pb->iocb_trace_currentry-1, 2 , 1);
       if (iocbhdr != NULL) {
         /* check first to see if anybody is waiting on the semaphore
          * or is there some more cleanup needed from this
          * routine.
          *
          * TODO: (no harm done; 7-Nov-98)
          * We may not be waiting on a semaphore.
          * Need to put in checks.   But it doesn't hurt to post the semaphore
          * as long as the semaphore is initialized.  But if I
	  * make ELS waiting optional and then do not malloc a
	  * semaphore, this would be wrong.
          */
         kgpsa_krn$_post( &iocbhdr->rsp, pb->pbx, "found IOCB" );
       }
       break;
   } /*  switch (iocb_rsp->ulpCommand)  */

EXIT:
   return;
}

/*+
 * ============================================================================
 * = kgpsa_els_rcv - handle an unsolicited incoming sequence                  =
 * ============================================================================
 *
 * Handle an unsolicited incoming sequence.   There is the posibility
 * that the iocb's that come off the receive queue are stale.  That is,
 * they may have come in before a LATT, and therefore they may have 
 * wrong info.  If this does occur, the situation is short lived since
 * we will reprobe after a LATT, and the results from the reprobe will
 * supercede anything we do with this stale receive.  Since the console has
 * gone to rejecting PLOGI's, the ramifications from a stale receive are 
 * minimal if any.
 *
 * Called by: kgpsa_els_response
 *
 *                          POSTERITY NOTE
 * In earlier versions of this port driver, console used to accept incoming
 * PLOGI commands.  Console made the simplification with the assumption 
 * (approved by James Smart) that storage n_ports do not issue PLOGIs and that
 * they can always be LS_RJTed.   Older versions of this driver also supported
 * the PDISC transactions.  If we ever need to reinstate this functionality,
 * somewhere in the library we can find code to start with.  There is a more
 * elaborate state table developed my James Smart and Dennis Mazur.  It is
 * quite a complex table but it covers a lot of situations.
 *
 *
-*/
void kgpsa_els_rcv( struct kgpsa_pb *pb, IOCB *iocb_rcv )
{
 int rpi;
 int xri;
 int did;
 int *rcv_payload;
 struct kgpsa_sb *sb;
 MAILBOX mbx, *mb = &mbx;
 ELS_PKT *els; 

 /*                  ACTION TABLE
  *
  *             current state:     LOGIN           LOGOUT
  * event:                       ------------------------------
  *   rcv PLOGI                   send LS_RJT[1]   send LS_RJT
  *   rcv LOGO                    send ACC         send ACC
  *   rcv PDISC match             send LS_RJT      send LS_RJT
  *   rcv PDISC mismatch          send LS_RJT      send LS_RJT
  *   rcv PRLI                    send ACC         send LOGO
  *   rcv PRLO                    send ACC         send LOGO
  *    
  *     UL - unreg_login
  *     [1]we assume we do not receive unsolicited PLOGIs from
  *        storage devices.
  *
  */


  if (iocb_rcv->ulpStatus != IOSTAT_SUCCESS) {
     if( iocb_rcv->ulpBdeCount == 0 ) {
       return;
     } else {
       goto EXIT;
     }
  }

  els = iocb_rcv->un.rcvels.elsReq[0].bdeAddress - $MAP_FACTOR(pb) ;
  rcv_payload = els;
  did = iocb_rcv->un.rcvels.remoteID;
  xri = iocb_rcv->un1.t1.ulpContext;
  rpi = iocb_rcv->un1.t1.ulpIoTag;

  dprintf("elsCode: %x\n", p_args1(els->elsCode) );
  switch (els->elsCode) {


    /*********************************************
     *  rcv PLOGI                                *
     *********************************************/
    case ELS_CMD_PLOGI:
    case ELS_CMD_FLOGI:
      /*
       * PLOGIs are not accepted.   They have to be initiated by this driver.
       * This has the small deficiency that if an n_port fails to respond
       * during the probing time window, it will not be seen.  See the 
       * comment on fc_process (2-Nov-98) regarding this.   I have never
       * seen a 'slow' responder to a PLOGI and think this is a non-issue.
       *
       */
      kgpsa_els_rsp( pb, ELS_CMD_LS_RJT, xri, did, rpi, PLOGI_LS_RJT_SENT );
      break;


    /*********************************************
     *  rcv LOGO                                 *
     *********************************************/
    case ELS_CMD_LOGO:
      dprintf("rcv LOGO from did: %x\n", p_args1(did) );
      /*
       * TODO:
       *   Do we need to check the parameters of the LOGO?
       */
      kgpsa_els_rsp( pb, ELS_CMD_ACC, xri, did, rpi, LOGO_ACC_SENT );
      break;



    /*********************************************
     *  rcv PDISC                                *
     *********************************************/
    case ELS_CMD_PDISC:
      /*
       * No PDISC support
       */
      kgpsa_els_rsp( pb, ELS_CMD_LS_RJT, xri, did, rpi, PDISC_LS_RJT_SENT );
      break;


    /*********************************************
     *  rcv PRLI                                 *
     *********************************************/
    case ELS_CODE_PRLI:
      sb = kgpsa_find_did( pb, did );
      if(sb!=NULL) {
        /* TODO: Make sure we are PLOGIed */
	dprintf("ACCepting PRLI\n", p_args0);
        kgpsa_els_rsp( pb, ELS_CMD_FCP_ACC, xri, did, rpi, PRLI_ACC_SENT );
        kgpsa_set_prli_state( sb, PRLI_OK );
 	probe_state( pb, PROBE_SET, PROBE_DONE, sb->did);
	sb->scsi_target = els->un.prli.targetFunc;
      } else {
        pprintf("PRLI has a null sb - Issueing LOGO - did: %x\n", 
                                                                p_args1(did) );
        kgpsa_els_cmd( pb, did, ELS_CMD_LOGO, DONT_WAIT, DONT_RETRY );
      }
      break;



    /*********************************************
     *  rcv PRLO                                 *
     *********************************************/
    case ELS_CODE_PRLO: 
      sb = kgpsa_find_did( pb, did );
      if(sb!=NULL) {
	/* TODO: Make sure we are PLOGIed */
        dprintf("ACCepting PRLO\n", p_args0);
        kgpsa_els_rsp( pb, ELS_CMD_FCP_ACC, xri, did, rpi, PRLO_ACC_SENT );
        kgpsa_set_prli_state( sb, PRLI_NG );
      } else {
        pprintf("PRLO has a null sb - Issueing LOGO - did: %x\n", 
                                                                p_args1(did) );
        kgpsa_els_cmd( pb, did, ELS_CMD_LOGO, DONT_WAIT, DONT_RETRY );
      }
      break;

#if FABRIC_SUPPORT
    /*********************************************
     *  rcv RSCN                                 *
     *********************************************/
    case ELS_CMD_RSCN:
      dprintf("Received an RSCN\n", p_args0 );
      /*
       * FABRIC STATE CHANGE THREAD STARTING WITH RSCN
       *   already logged into fabric
       * --> You are here
       *
       * --> 1. Receive notification of a state change
       * --> 2. Accept the RSCN
       * --> 3. suspend FCP outgoing queue
       * --> 4. log into name server
       *     5. Receive login response
       *     6. give the ring a buffer
       *        some problems here I think.  Do we want to queue to an XRI
       *        instead?  What happens to the XRI queued buffer when the 
       *        exchange goes away.  - A RET_XRI_QUE_BUF returns the unused
       *        buffer.
       *        Do we just want to queue this to the FCP ring and have it there
       *        always and replenish it upon every receipt?
       *     7. Request the latest list of nodes with GP_ID3
       *     8. Receive the SCR completed response
       *     9. Receive asynchronously the data from the SCR
       *    10. Logout from the name server
       *    11. Probe the node list
       */

      dprintf("* --> 1. Receive notification of a state change\n", p_args0);
      dprintf("* --> 2. Accept the RSCN\n", p_args0);

      /*
       * Accept the RSCN.  Handle further activity in the accept routine.
       */
      kgpsa_set_devState( pb, DEV_PROBE_NEEDED, "RSCN" );
      kgpsa_els_rsp( pb, ELS_CMD_ACC, xri, did, rpi, RSCN_ACC_SENT );

      break;

#endif


    default:
      dprintf("default CMD_RCV_ELS_REQ_CX  \n", p_args0 );
      break;
  } /* switch elscode */



EXIT:
  /*
   * Give the buffer back to the adapter.  It is one of the group that
   * we allocated it for receiving unsolicited ELSes.
   */
  kgpsa_que_ring_buf( pb, FC_ELS_RING, els, MAX_ELS_BUF );

  return;

}


/*+
 * ============================================================================
 * = kgpsa_fcp_response - handle a received FCP                               =
 * ============================================================================
-*/
void kgpsa_fcp_response( struct kgpsa_pb *pb, IOCB *iocb_rsp, int doffset )
{
 IOCB_HDR *iocbhdr;
 IOCB *rsp; 
 int *buf;

 kgpsa_trace_iocb( "R FCP  R ", doffset, iocb_rsp, (kgpsa_trace_iocb_doit&2) );

 switch (iocb_rsp->ulpCommand)  {
   case CMD_RCV_SEQUENCE_CX:
     dprintf("received on FCP - CMD_RCV_SEQUENCE\n", p_args0);


         if ( (iocb_rsp->ulpStatus  == IOSTAT_LOCAL_REJECT ) &&
              (iocb_rsp->un.ulpWord[4] == IOERR_RCV_BUFFER_WAITING) ){
              dprintf("IOERR_RCV_BUFFER_WAITING\n", p_args0 );
              /*
               */
              buf = malloc(CT_BUF_SZ, "GP_ID3 buf" );
              kgpsa_que_ring_buf( pb, FC_FCP_RING, buf, CT_BUF_SZ );
              break;
            } 

         if ( iocb_rsp->ulpStatus  == IOSTAT_LOCAL_REJECT ) {
              d2printf("RCV_SEQ IOERR : %x\n", p_args1(iocb_rsp->un.ulpWord[4]) );
	      break;
            }
   

#if FABRIC_SUPPORT
      /*
       * FABRIC STATE CHANGE THREAD
       * --> You are here
       *
       *     1. Receive notification of a state change
       *     2. Accept the RSCN
       *     3. suspend FCP outgoing queue
       *     4. log into name server
       *     5. Receive login response
       *     6. give the ring a buffer
       *        some problems here I think.  Do we want to queue to an XRI
       *        instead?  What happens to the XRI queued buffer when the 
       *        exchange goes away.  - A RET_XRI_QUE_BUF returns the unused
       *        buffer.
       *        Do we just want to queue this to the FCP ring and have it there
       *        always and replenish it upon every receipt?
       *     7. Request the latest list of nodes with GP_ID3
       *     8. Receive the SCR completed response
       * --> 9. Receive asynchronously the data from the SCR
       * -->10. Logout from the name server
       * -->11. Probe the node list
       */      


{
 int did;
 int i,j;
 int *s;
 int xri;
 int nodelist_cnt;
 int *nodelist;
 struct kgpsa_sb *sb;
 WORD5 word5;
 CT_HDR *ct_hdr;

     dprintf("* --> 9. Receive asynchronously the data from the SCR\n", p_args0);
     dprintf("* -->10. Logout from the name server\n", p_args0);


 ct_hdr = iocb_rsp->un.rcvels.elsReq[0].bdeAddress - $MAP_FACTOR(pb) ;

 kgpsa_dump_ct(ct_hdr, (kgpsa_debug_flag&1) );
 /*
  *  Check iocb word 5 for the type code.  Then check to see if
  *  the packet is accepted.
  */
 word5.reserved = iocb_rsp->un.ulpWord[5];
 
 if( (word5.hcsw.Rctl!=CT_R_CTL_RESPONSE) || (word5.hcsw.Type!=CT_ULP_TYPE) )
  {
    free(ct_hdr, "GP_ID3 buf");
    xri = iocb_rsp->un1.t1.ulpContext;
    kgpsa_abort_xri_cx( pb, xri );
    return;
  }
 else
  {
  }

 pb->CT_late_intvcnt = 0; 
 kgpsa_els_cmd( pb, NAME_SERVER, ELS_CMD_LOGO, DONT_WAIT, DONT_RETRY );

/* TODO: (strong - retry) Check to see if iocb is has a good status */
 if (iocb_rsp->ulpStatus != IOSTAT_SUCCESS) {
   pprintf("ct iocb failed - do something\n");
   free(ct_hdr, "GP_ID3 buf");
   return;
 }
 memcpy(pb->scsi_targets, ct_hdr, CT_BUF_SZ);
 free(ct_hdr, "GP_ID3 buf");

 if (iocb_rsp->un.cont[0].bdeSize == sizeof(CT_HDR) )
    {
      dprintf("bde is size of header - no data\n", p_args0);
      dprintf("reasonCode: %x\n", p_args1(ct_hdr->reasonCode) );
      dprintf("explanation: %x\n", p_args1(ct_hdr->explanation) );
    }
 else
    {
       pb->scsi_targets_cnt = 0;
       s = pb->scsi_targets+4;
       for(i=0;;i++)
         {
           did = SWAP_LONG(*s) & 0x00ffffff;
#if KGPSA_DEBUG
	   dprintf("%x\n", p_args1(did) );
#endif
           if(did==0)
             {
              pprintf("s:%x did is 0  (scsi_targets: %x)\n", s, pb->scsi_targets);
	      pprintf("iocb_rsp: %x\n", &iocb_rsp );
	      do_bpt();
             }
	   pb->scsi_targets[pb->scsi_targets_cnt++] = did;  /* recycle the buffer */
	   if (*s & CT_NS_PORT_ID_LAST_IND) break;
	   s++;
         } 
    }


#if 1
/*
 * Fab up a list of nodes we know to exist
 */
   nodelist = malloc( 4*pb->node_count, "temp node count" );
   nodelist_cnt = 0;
   for(i=0; i< pb->node_count; i++)
     {
       struct kgpsa_sb *sb; 
       FC_BRD_INFO *binfo;
       binfo = &pb->binfo;
       sb = pb->pb.sb[i];
       dprintf("sb->did: %x\n", p_args1(sb->did) );
       if( (sb->did < 0xFFFFF0) && (sb->did != binfo->fc_myDID) && sb->valid )
         {
         nodelist[nodelist_cnt++] = sb->did;
	 dprintf("setting it to nodelist[%d]\n", p_args1(nodelist_cnt) );
         }
     }


   for( i=0; i<nodelist_cnt; i++ )
     {
       dprintf("Nodelist[%d]: %x\n", p_args2(i, nodelist[i]) );
     }

   subtract( nodelist, nodelist_cnt, pb->scsi_targets, pb->scsi_targets_cnt,
             nodelist, &nodelist_cnt );

   for( i=0; i<nodelist_cnt; i++ )
     {
       d4printf("Dropped off: %x\n", p_args1(nodelist[i]) );
       kgpsa_void_did( pb, nodelist[i] );
     }

   free( nodelist, "temp node count" );
#endif


   probe_state( pb, PROBE_SET, PROBE_IN_PROGRESS, ALL_DID );

   kgpsa_set_DiscState( pb, DISCOVERY_IN_PROGRESS, "scsi_targets" );
   
   if ( pb->scsi_targets_cnt )
     {
      dprintf("* -->11. Probe the node list\n", p_args0);
      pb->discovery_index = 0;  /* Get past header of data struct */
      kgpsa_els_cmd( pb, pb->scsi_targets[pb->discovery_index] , ELS_CMD_PLOGI, DONT_WAIT, fc_els_retries );
     }

}

#endif
     break;

   /*
    * This response is to a command we sent to the name server.
    */
#if FABRIC_SUPPORT
   case CMD_XMIT_SEQUENCE_CX:
     dprintf("received on FCP - CMD_XMIT_SEQUENCE_CX\n", p_args0);
      /*
       * FABRIC STATE CHANGE THREAD
       * --> You are here
       *
       *     1. Receive notification of a state change
       *     2. Accept the RSCN
       *     3. suspend FCP outgoing queue
       *     4. log into name server
       *     5. Receive login response
       *     6. give the ring a buffer
       *        some problems here I think.  Do we want to queue to an XRI
       *        instead?  What happens to the XRI queued buffer when the 
       *        exchange goes away.  - A RET_XRI_QUE_BUF returns the unused
       *        buffer.
       *        Do we just want to queue this to the FCP ring and have it there
       *        always and replenish it upon every receipt?
       *     7. Request the latest list of nodes with GP_ID3
       * --> 8. Receive the SCR completed response
       *     9. Receive asynchronously the data from the SCR
       *    10. Logout from the name server
       *    11. Probe the node list
       */      
     iocbhdr = kgpsa_find_iocbhdr( pb, IOCB_ACTIVE, FC_FCP_RING, 
                                                iocb_rsp->un1.t1.ulpIoTag, 1 );
     if (iocbhdr != NULL) {
       memcpy ( &iocbhdr->rsp_iocb, iocb_rsp, 32);
       remq_lock(&iocbhdr->iocb_q.flink);
       if (iocbhdr->wait_for_completion) {
         kgpsa_krn$_post( &iocbhdr->rsp, pb->pbx, "found IOCB" );
       } else {
         kgpsa_xmit_seq_done( pb, iocbhdr );
       }
     } else {
       dprintf("No header found for CMD_XMIT_SEQUENCE_CX\n", p_args0);
     }
     break;
#endif

   case CMD_FCP_IWRITE_CR:
   case CMD_FCP_IREAD_CR:
   case CMD_FCP_ICMND_CR:
   case CMD_FCP_IWRITE_CX:
   case CMD_FCP_IREAD_CX:
   case CMD_FCP_ICMND_CX:
     iocbhdr = kgpsa_find_iocbhdr( pb, IOCB_ACTIVE, FC_FCP_RING, 
                                           iocb_rsp->un1.t1.ulpIoTag, 1);
     if (iocbhdr != NULL) {
        memcpy ( &iocbhdr->rsp_iocb, iocb_rsp, 32);
        remq_lock(&iocbhdr->iocb_q.flink);
        /*
         * Check for some errors that may require some ELS
         * activity.  That will be done from this response process.
         */
        rsp = &iocbhdr->rsp_iocb;
        if( (rsp->ulpStatus == IOSTAT_LOCAL_REJECT) &&
           ( (rsp->un.fcpi.fcpi_parm == IOERR_INVALID_RPI)       ||
             (rsp->un.fcpi.fcpi_parm == IOERR_SEQUENCE_TIMEOUT)  ||
             (rsp->un.fcpi.fcpi_parm == IOERR_LOOP_OPEN_FAILURE) )  )
          {
	   kgpsa_els_cmd( pb, iocbhdr->sb->did, ELS_CMD_PLOGI, DONT_WAIT, fc_els_retries );
     	  }
        iocbhdr->iocb_state = IOCB_FCP_DONE;
        kgpsa_krn$_post( &iocbhdr->xrsp_t.sem, pb->pbx, "found IOCB" );
     } else {
       dprintf("Huh?  Could not find iocbhdr for FCP rcv'ed IOCB\n", p_args0);
     }
     break;
   default:
     d2printf("FCP ring default case \n", p_args0);
     break;
   } /* switch (iocb_rsp->ulpCommand) */
}

/*+
 * ============================================================================
 * = kgpsa_find_iocbhdr - look thru a queue for a given iotag                 =
 * ============================================================================
 *
 * Look on a rings queue of iocbhdr's and find a matching iotag.   The
 * ELSes and FCPs have different rings, so they are allowed to have the same
 * iotags.  
 *
 *                       POSTERITY NOTE
 * Although this driver runs the iotags for the ELS and FCP iocb's 
 * independently, that would be an issue if this driver was ever to issue
 * abort commands.  An abort command keys off RPI+IOTAG, and has no knowledge
 * of ring.  Aborts that are issued to the ELS ring can kill off FCPs and
 * vice versa, so the ring type concept is a blur when it comes to aborts.
 * In fact, UNIX, which does implement aborts, has decided to only have one
 * ring with both ELS and FCP types, since it really doesn't matter.  When
 * we first wrote our drivers (console and UNIX), we had separate rings in
 * the style of the drivers provided by Emulex (NT, AIX).   I stayed with that
 * but UNIX converted to one ring to guarantee the abort command would issue
 * after the iocb it was trying to abort.   That is, in a 2 ring situation
 * an FCP issued on the FCP ring, and then an ABORT command of that FCP issued
 * on the ELS ring may not get aborted.  The ABORT command may be worked on
 * in the adapter prior to the adapter reading the FCP command.  So in order
 * to coordinate the two rings for the abort, UNIX decided to go to 1 ring
 * which guaratees, if the adapter reads the ABORT command, it had previous
 * read the FCP that should be aborted.  How does console get around this?
 * Instead of using the ABORT command, I just unreg_login to abort everything,
 * since I do not need the fine resolution that UNIX uses to optimize 
 * performance.  Also, the console has only 1 FCP outstanding at a time, which
 * makes finding that FCP and killing it off, and the need to kill it off
 * a much less frequent and easier situation to manage.
 * 
 *
 * This is a generalized iocbhdr lookup routine. It only has one queue
 * type in the switch statement below, but that could change.  At one point
 * there were more queues (like a pending queue) in this driver, but those
 * were unnecessary.  Rather than remove the construct to allow for more 
 * queues, I leave the code this way.
 *
-*/

IOCB_HDR *kgpsa_find_iocbhdr( struct kgpsa_pb *pb, int queue, int ringno,
                                                      int iotag, int verbose )
{
  RING	*rp;
  FC_BRD_INFO *binfo;
  IOCB_HDR *t;
  struct QUEUE *qp;
  int found = FALSE;
  int *hdrflink;
  binfo = &pb->binfo;
  rp = &binfo->fc_ring[ringno];

  switch( queue ) {
    case IOCB_ACTIVE:
        qp = rp->active_q.flink;
	hdrflink = &rp->active_q.flink;
	break;
  }

  while ( (qp != hdrflink) && (found==FALSE) ){
    t = (int)qp - offsetof(IOCB_HDR, iocb_q);
    if (t->cmd_iocb.un1.t1.ulpIoTag == iotag) {
      found = TRUE;
    }
    qp = qp->flink;
  }
  if (found==FALSE){
    if(verbose)
       pprintf("pb: %x  - No iotag %x found\n", p_args2(pb,iotag) );
    t = NULL;
  }

#if KGPSA_DEBUG
  /*
   * check for duplicate iotags from a lost els
   */
  {
  IOCB_HDR *t2;
  qp = rp->active_q.flink;
  hdrflink = &rp->active_q.flink;
  found = FALSE;
  while (qp != hdrflink) {
    t2 = (int)qp - offsetof(IOCB_HDR, iocb_q);
    if (t2->cmd_iocb.un1.t1.ulpIoTag == iotag) {
      if( found == TRUE ) {
        dprintf("\n\n****************************************\n", p_args0);
        pprintf("Duplicate Iotag: %x ", iotag );
        do_bpt();
      }
      found = TRUE;
    }
    qp = qp->flink;
  }
  }
#endif

  return t;
}

/*+
 * ============================================================================
 * = kgpsa_read_alpa_map - gather loop and link information                   =
 * ============================================================================
 *
 *   The difference between kgpsa_read_la and kgpsa_read_alpa_map is that
 *   the kgpsa_read_alpa_map is waits for the link to be up, so that the
 *   alpa map is valid.   The kgpsa_read_la routine is used to find out
 *   if the link is up or down and the eventtag, contrasting that to a 
 *   minute delay waiting for the link to come up for a valid alpa_map.
 *
 *   Since this driver no longer uses an alpa_map from the loop initialization
 *   phase, this routine is only useful for the waiting for the link to be
 *   up and eventTag and myDid information.
 *
 *
 *                     POSTERITY NOTE
 *   The alpa map was a nice idea, but it is not guaranteed to work.  It is
 *   not a mandatory functionality of every n_port on the loop, and if one 
 *   n_port does not support it, no alpa map is generated.  Guess what, the
 *   Tachyn chip which the HSG80 uses does not go thru the alpa map phase.
 *   Seeing how the HSG80 is the primary focus of the fibre configs we are
 *   to offer, any code involving alpa map management would be seldom if
 *   ever used.  Now the console just polls all possible loop alpas.  It was
 *   a surprise when we found out the Tachyn did not provide an alpa map since
 *   we had been working with well behaved Seagates.   Tachyn chips are 
 *   very popular in the world (including the Compaq Jaguar adapter), so
 *   alpa maps will hardly ever be generated.
 *
 */
int kgpsa_read_alpa_map( struct kgpsa_pb *pb, MAILBOX *mb, int timeout )
 {
  int i=0;
  int status = msg_success;
  volatile struct TIMERQ *tq, tqe;
  FC_BRD_INFO *binfo;
  int sleep_time = kgpsa_read_alpa_sleep;

#if LOOP_NOT_SUPPORTED
     /*
      * Get out without waiting for link to come up and do not report
      * a problem if link does not come up.
      */
     if( pb->topology == FLAGS_TOPOLOGY_MODE_LOOP )
        {
          return msg_success;
        }
#endif

  tq = &tqe;
  binfo = &pb->binfo;
  start_timeout( timeout, tq);
  kgpsa_read_la ( pb, mb );
  while ( ( mb->un.varReadLA.attType == AT_LINK_DOWN ) &&
           ( tq->sem.count == 0 )                      ){
     if ( (i%10) == 0 ) {
       dprintf("LINK DOWN : elapsed downtime: %d (decimal)\n", 
                                                  p_args1(i*sleep_time) );
     }
     kgpsa_read_la ( pb, mb );
     krn$_sleep(sleep_time);
     i++;
  } /* while */
  if (tq->sem.count) {
     status = msg_failure;
     pprintf("LINK_DOWN timeout\n", p_args0);
  }
 stop_timeout(tq);
 pb->attType = mb->un.varReadLA.attType;
 if (status == msg_success) {
   pb->LINKUPeventTag = mb->un.varReadLA.eventTag;
   binfo->fc_myDID = mb->un.varReadLA.granted_AL_PA;
   dprintf("LINK UP : elapsed downtime: %d (decimal)\n", 
                                                      p_args1(i*sleep_time) );
   dprintf("myDID:%x\n", p_args1(binfo->fc_myDID) );
 }

  return status;
 }




/*+
 * ============================================================================
 * = kgpsa_make_FF_ready - Initialize the adapter                             =
 * ============================================================================
 *
 *  Initialize the adapter
 *
-*/
int kgpsa_make_FF_ready( struct kgpsa_pb *pb )
{
 ULONG   i, status;
 MAILBOX lmb, *mbx=&lmb;

 dprintf("About to do hard_reset\n", p_args0);


  /*
   *  Clear the mbxhc and owner bits so we don't get confused when
   *  trying to write the first mailbox
   */
  ws(0,0);

/*
 *
 * The wait period is actually 10 clocks @40Mhz or 2.5 microseconds, though
 * I believe in some of our drivers (for superstitious reasons) we wait 50us.
 *
 * The POST code / OFC synchronization ( fiber GLM w/OFC) can take up to
 * 10-15 seconds to complete, while the POST + copper GLM should complete
 * in 2-5 seconds.
 *
 */
 memset( mbx, 0, MAILBOX_CMD_BSIZE );
 mbx->mbxCommand = MBX_RESTART;
 /*mbx->mbxOwner   = OWN_HOST;   this is done in the kgpsa_issue_mbx rtn */
 mbx->mbxHc = 1;
 status = kgpsa_issue_mbx( pb, mbx );
 if (status != msg_success) {
   printf("Mailbox failed trying to RESTART\n",p_args0 );
   goto EXIT;
  }
 wl( HC, HC_INITFF );

 pb->LINKUPeventTag = 0;
 pb->RSCNeventTag = 0;
 pb->DiscLINKUPeventTag = 0;
 pb->DiscRSCNeventTag = 0;
 pb->LINKDOWNeventTag = 0;


/*
 * Reset all sb event counters.
 */
 for(i=0; i<pb->node_count; i++)
   {
     struct kgpsa_sb *sb;  
     sb = pb->pb.sb[i];
     sb->LINKUPeventTag = 0;
     sb->RSCNeventTag = 0;
   }


#define RESET_HOLD_DELAY 1000
 krn$_sleep( reset_hold_delay );
 /*
  * If we are a BIU1, the driver needs to clear INITFF.
  */
   wl( HC, 0 );
 krn$_sleep( 4000 );
 status = rl( HS );
 i = 0;
 /* Check status register to see what current state is */
 while ((status & (HS_FFRDY | HS_MBRDY)) != (HS_FFRDY | HS_MBRDY)) {
   /* Check every 100ms for 50 retries. */
   if (i++ > 50) {
    pprintf("HARD_RESTART failed\n",p_args0);
     return msg_failure;
   }
   /* Check to see if any errors occurred during init */
   if (status & HS_FFERM) {
     /* ERROR: During chipset initialization */
     pprintf("RESTART chip errors\n",p_args0);
     status = msg_failure;
     break;
   }
   krn$_sleep(100);
   status = rl(HS);
 } /* while */

EXIT:
 return status;

}

/* 
 * TODO :  Make a different version of this for non KGPSA_DEBUG mode
 */
void kgpsa_set_devState( struct kgpsa_pb *pb, int value, char *label )
{
 dprintf("\n *** Setting devState from %s to %s (%s)\n\n",
   p_args3( devnstate_text[pb->devState], devnstate_text[value], label ));
 pb->devState = value;
}


void kgpsa_set_DiscState( struct kgpsa_pb *pb, int value, char *label )
{
 dprintf("\n *** Setting DiscState from %s to %s (%s)\n\n",
   p_args3( Discstate_text[pb->DiscState], Discstate_text[value], label ));
 pb->DiscState = value;
}


/*+
 * ============================================================================
 * = kgpsa_queue_ring_buf - Give the adapter  buffer for unsolicited receives =
 * ============================================================================
 *
 * Give the adapter some buffers in main memory where it can store
 * payloads of from unsolicited packets that come in off the wire.  This
 * routine is called to initially give the adapter some buffers to work
 * with, and it is called after each unsolicited packet is processed by
 * the port driver giving the packet back to the adapter to recycle the buffer.
 *
-*/
int kgpsa_que_ring_buf( struct kgpsa_pb *pb, int ringno, int buffer, int bufsiz )
  {
   int status;
   IOCB *piocb;
   IOCB_HDR *iocb_hdr;
   int iotag;


   iotag = 0xabcd;
   iocb_hdr = kgpsa_alloc_iocbhdr( pb, iotag, "QUEBUF" );
   piocb = &iocb_hdr->cmd_iocb;

   memset( buffer, 0, bufsiz);

   piocb->un.cont[0].bdeAddress = (int)buffer | $MAP_FACTOR( pb );
   piocb->un.cont[0].bdeSize    = bufsiz;
   piocb->un.ulpWord[6]         = iotag;     /* IO_TAG */

   /*
    * Save on code generated by just using a longword constant.
    *
    * piocb->ulpOwner              = OWN_CHIP;
    * piocb->ulpLe                 = 1;
    * piocb->ulpBdeCount           = 1;
    * piocb->ulpCommand            = CMD_QUE_RING_BUF_CN;
    * piocb->ulpClass              = CLASS3;
    */
   piocb->un.ulpWord[7]         = 0x20607;

   status = kgpsa_send_iocb( pb, ringno, iocb_hdr );
   return status;
}


/*+
 * ============================================================================
 * = kgpsa_que_ring_buf_done - command completion action for que_ring_buf     =
 * ============================================================================
 *
 *  The QUE_RING_BUF command has no response.  So this routine is
 *  called after we are assured the IOCB has been written to the adapter
 *  and therefore the in-memory IOCB resource can be taken off the
 *  active queue freed.
-*/
void kgpsa_que_ring_buf_done( IOCB_HDR *iocb_hdr )
{
   /*
    *  Get it off the active queue since there is no response.
    */
   remq_lock(&iocb_hdr->iocb_q.flink);
   kgpsa_dealloc_iocbhdr( iocb_hdr );
}


/*+
 * ============================================================================
 * = kgpsa_command - called by class driver to request SCSI services          =
 * ============================================================================
 *
-*/
int kgpsa_command( struct kgpsa_sb *sb, int lun, unsigned char *cmd, int cmd_len,
        unsigned char *dat_out, int dat_out_len,
        unsigned char *dat_in, int dat_in_len, unsigned char *sts,
        unsigned char *sense_dat_in, int *sense_dat_in_len )
{
 int *ldat_in = dat_in;
 struct kgpsa_pb *pb;
 int status;
 struct srb srb, *psrb = &srb;
 int retry;
 int wait_cnt;

 pb = sb->pb;
 kgpsa_krn$_wait( &pb->fcp_owner_s, pb->pbx, "kgpsa_command" );
 psrb->sb               = sb;
 psrb->lun              = lun;
 psrb->cmd              = cmd;
 psrb->cmd_len          = cmd_len;
 psrb->dat_out          = dat_out;
 psrb->dat_out_len      = dat_out_len;
 psrb->dat_in           = dat_in;
 psrb->dat_in_len       = dat_in_len;
 psrb->sts              = sts;
 psrb->sense_dat_in     = sense_dat_in;
 psrb->sense_dat_in_len = sense_dat_in_len;


 sts[0] = scsi_k_sts_good;

retry = kgpsa_fcp_retries;
while ( retry ) {

 KGPSA_DUMP_COMMAND();
 if (dat_out_len) {
   status =  kgpsa_fcp_cmd( pb, CMD_FCP_IWRITE_CR, psrb);
 } else if (dat_in_len) {
   status =  kgpsa_fcp_cmd( pb, CMD_FCP_IREAD_CR, psrb );
 } else {
   status =  kgpsa_fcp_cmd( pb, CMD_FCP_ICMND_CR, psrb );
 }

 switch( status )
 {
 case msg_success:
   retry = 0;
   pb->fcp_retries_allowed = 3;
   sb->retries = 5;    
   break;
 case fc_sts_incarnerr:
   --sb->retries;
   retry = ( sb->retries > 0 );
   d2printf("fc_sts_incarnerr retry: %d \n", p_args1(sb->retries) );
   if( retry ) 
     {
      krn$_sleep(2000);
    }
   break;
 case fc_sts_retry:
   d2printf("fc_sts_retry  retry: %d \n", p_args1(retry) );
   krn$_sleep(kgpsa_fcp_retry_delay);
   retry--;
   break;
 /*
  * fc_sts_seq_timeout - if an FCP is not responded to, then something is
  * really wrong.  Give up.  This is the switch equivalent of LOOP_OPEN.
  * fc_sts_loop_open - if the node is not there, give up.
  */
 case fc_sts_seq_timeout:
   retry--;
   krn$_sleep(kgpsa_fcp_retry_delay);
   d2printf("     fc_sts_seq_timeout done sleeping; retry: %d \n", p_args1(retry) );
   break;
 case fc_sts_loop_open:
   retry = 0;
   d2printf("     fc_sts_loop_open - retry: %d \n", p_args1(retry) );
   break;
 /*
  * fc_sts_link_down - a global counter is used after we find the link down.
  * This allows us to give some retry, without allowing every FCP to do all
  * these retries.  We like to fail future FCPs faster once we have concluded
  * the link is down for the long term.   If the link ever comes up, then
  * the fcp_retries_allowed is reset by a successful fcp in "case msg_success".
  */
 case fc_sts_link_down:
   --pb->fcp_retries_allowed;
   retry = max( 0, pb->fcp_retries_allowed );
   d2printf("fc_sts_link_down  retry: %d %s %s\n", 
     p_args3(retry, Discstate_text[pb->DiscState], devnstate_text[pb->devState])  );
   if( retry )
      krn$_sleep(3000);
   break;
 case fc_sts_fcp_cmd_field_err:
 default:
   retry = 0;
   break;
 } /* switch status */

} /* while retry */


EXIT:
 kgpsa_krn$_post( &pb->fcp_owner_s, pb->pbx, "kgpsa_command" );
 if ( sts[0] ) {
   dprintf("kgpsa_command failure:\n", p_args0);
   dprintf("\t sts[0]:%x\n",p_args1(sts[0]) );
   kgpsa_dump_blk("sense data:", sense_dat_in, (kgpsa_debug_flag&1) );
 }

 if (status != msg_success) {
   dprintf("kgpsa_command fcp status %x\n", p_args1( status ) );
 }

#if 0
/*
 * HACK FOR TESTING
 *
 * This is useful for injecting a failure and checking out the upper 
 * scsi layer.
 */
if(kgdevid)
  if( cmd[0] == scsi_k_cmd_rd_devid )
    {
      sts[0] = scsi_k_sts_check;
      sense_dat_in[0]  = 0x70;
      sense_dat_in[2]  = 0x5;
      sense_dat_in[12] = 0x20;
      sense_dat_in[13] = 0x00;
      *sense_dat_in_len = 25;
    }
#endif


#if 0
/*
 * HACK FOR TESTING
 *
 * This is useful for injecting a failure and checking out the upper 
 * scsi layer.
 */
if(kgrepluns)
  if( cmd[0] == scsi_k_cmd_report_luns ) 
    {
      sts[0] = scsi_k_sts_check;
      sense_dat_in[0]  = 0x70;
      sense_dat_in[2]  = 0x5;
      sense_dat_in[12] = 0x20;
      sense_dat_in[13] = 0x00;
      *sense_dat_in_len = 25;
    }

#endif



 return status;
}



/*+
 * ============================================================================
 * = kgpsa_rundown_send_inquiry -                                             =
 * ============================================================================
 *
-*/
void kgpsa_rundown_send_inquiry( struct kgpsa_sb *sb )
{
  scsi_send_inquiry(sb);
}



/*+
 * ============================================================================
 * = kgpsa_alloc_iocbhdr - allocate buffer for iocb and related constructs    =
 * ============================================================================
 *
 * Allocate space for an iocb.  Fill in some of the fields.  The most
 * interesting of these is the incarnation field.   I debated whether
 * this should be LINKUPeventTag or DiscStarteventTag.  I think either
 * way would result in correct behavior.  Using DiscStarteventTag throws
 * away more ELS responses.
 *
-*/
IOCB_HDR *kgpsa_alloc_iocbhdr( struct kgpsa_pb *pb, int iotag, char *label )
{
 IOCB_HDR *iocb_hdr;
 char semname[20];
 iocb_hdr = malloc(sizeof(IOCB_HDR), "iocb_hdr");
 iocb_hdr->pb = pb;
 iocb_hdr->iotag = iotag;
 iocb_hdr->cmd_iocb.un1.t1.ulpIoTag = iotag;
 iocb_hdr->cont_iocb.un1.t1.ulpIoTag = iotag;
 iocb_hdr->RSCNeventTag = pb->DiscRSCNeventTag;
 iocb_hdr->LINKUPeventTag = pb->DiscLINKUPeventTag;
 sprintf(&semname,"%s:%x",label, iotag);
 krn$_seminit( &iocb_hdr->rsp, 0, &semname );
 return iocb_hdr;
}

/*+
 * ============================================================================
 * = kgpsa_dealloc_iocbhdr - Free up an iocbhdr                               =
 * ============================================================================
 *
-*/
void kgpsa_dealloc_iocbhdr( IOCB_HDR *iocb_hdr )
{
 struct kgpsa_pb *pb = iocb_hdr->pb;
 krn$_semrelease( &iocb_hdr->rsp );
 free(iocb_hdr, "iocb_hdr");
}


void kgpsa_void_did( struct kgpsa_pb *pb, int did )
{
  struct kgpsa_sb *sb;
  sb = kgpsa_find_did( pb, did );
  if (sb!=NULL) {
     qprintf("%s - A valid node has dropped off link: %x\n", p_args2(pb->pb.name, did) );
     d2printf("%s - A valid node has dropped off link: %x\n", p_args2(pb->pb.name, did) );
     kgpsa_set_nstate( sb, NLP_LOGOUT );
     sb->valid = 0;
     sb->rpi = 0;
  } else {
     dprintf("Null sb- PLOGI - did : %x\n", p_args1( did ) );
  }
  probe_state( pb, PROBE_SET, PROBE_DONE, did);

}


/*
 *########################################################################
 *#########                    ELS                                ########
 *########################################################################
 */

void dump_els( struct kgpsa_pb *pb )
{
  int save_ipl;
  kgpsa_dump_slim(pb, 0x100, (IOCB_CMD_R0_ENTRIES+IOCB_RSP_R0_ENTRIES)*8 );
}

void dump_fcp( struct kgpsa_pb *pb)
{
  kgpsa_dump_slim(pb ,0x100 + ((IOCB_CMD_R0_ENTRIES+IOCB_RSP_R0_ENTRIES)*32) +
                    ((IOCB_CMD_R1_ENTRIES+IOCB_RSP_R1_ENTRIES)*32),
            (IOCB_CMD_R2_ENTRIES+IOCB_RSP_R2_ENTRIES)*8 );
}

/*+
 * ============================================================================
 * = kgpsa_els_cmd - Originate an ELS command to a did                        =
 * ============================================================================
 *
-*/
int kgpsa_els_cmd( struct kgpsa_pb *pb, int did, int elstype, 
                                      int wait_for_completion, int retries )
{
 int status = msg_success;
 int *cmd_payload;
 int *rsp_payload;
 int nstate;
 struct kgpsa_sb *sb;
 IOCB *icmd, *irsp;
 MAILBOX mbx;
 IOCB_HDR *iocb_hdr;
 FC_BRD_INFO *binfo;
 SERV_PARM   *sp;
 int iotag;
 int al_pa;
 ELS_PKT *els;
 int elscode;

 int cmdsize;
 /*
  * I tried to be thrifty and used the max ELS size for the outgoing
  * payload, but some n_ports were strict and rejected or failed a
  * size inconsistancy.  e.g. the Brocade switch required the LOGO to 
  * be 16 bytes.  Therefore I added cmdsize and issue an accurately sized
  * payload for each command.  Just an observation that I thought I would
  * note here for posterity.
  */


 pb->elscmdcnt++;
 iotag = pb->elscmdcnt;

 dprintf("\nels cmd iotag: %x  elstype: %s to did: %x\n\n", 
                       p_args3(iotag, elstype_text[elstype&0xFF], did) );

 iocb_hdr = kgpsa_alloc_iocbhdr( pb, iotag, elstype_text[elstype&0xFF] );
 iocb_hdr->did = did;
 iocb_hdr->wait_for_completion = wait_for_completion;
 iocb_hdr->retries = retries;
 iocb_hdr->pb = pb;
 iocb_hdr->sb = kgpsa_find_did( pb, did );  /* may not be valid at this point*/

 binfo = &pb->binfo;
 icmd = &iocb_hdr->cmd_iocb;
 irsp = &iocb_hdr->rsp_iocb;
 cmd_payload = malloc( ELS_CMD_PAYLOAD_SIZE, "payload" );
 iocb_hdr->bde1_bufptr = cmd_payload;
 rsp_payload = malloc( ELS_RSP_PAYLOAD_SIZE, "payload" );
 iocb_hdr->bde2_bufptr = rsp_payload;

 cmd_payload[0] = elstype;
 els = cmd_payload;
 elscode = els->elsCode;

 switch(elscode) {

   case ELS_CMD_PDISC:
   case ELS_CMD_PLOGI:             /* FC-PH Table 71 */
   case ELS_CMD_FLOGI:
     memcpy ( &cmd_payload[1], &binfo->fc_sparam, sizeof(SERV_PARM) );
     cmdsize = sizeof( SERV_PARM );
     break;

   case ELS_CODE_PRLI:                    /* FC-PH-2 Table 114  */
     /*
      *  Format PRLI request payload.  To save on code size, I avoid
      *  the bit manipulation and just set longword constants.  The
      *  data structure (and bit positions labelled below) are BIG ENDIAN.
      *
      *  For example, look at estabImagePair at bit 13
      *
      *  BITS 7:0  |  BITS 15:8 |  BITS 23:16 |  BITS 24-31
      *            | estabImage |             |  prliType 0x8
      *            |  bit 13    |             |
      *            |    111111  |             |
      *            |    54321098|             |
      *            |bin 00100000|             |
      *            |hex 20      |             |
      *            |            |             |
      *
      * elsCode = ELS_CMD_PRLI;
      * elsByte1 = PRLX_PAGE_LEN;
      * elsByte2 = 0;
      * elsByte3 = PRLX_PAGE_LEN + 4;
      *    cmd_payload[0] = ELS_CMD_FCP_PRLI  0x14001020 
      *
      * prli.prliType = PRLI_FCP_TYPE (8);  bits 24:31
      * prli.estabImagePair  = 1;           bit 13 
      *    cmd_payload[1] = 0x00200008
      *
      * prli.readXferRdyDis  = 1;           bit  1
      * prli.initiatorFunc   = 1;           bit  5
      *    cmd_payload[4] = 0x22000000           
      */
     cmd_payload[1] = 0x00200008; 
     cmd_payload[4] = 0x22000000;
     cmdsize = sizeof( PRLI );
     break;

   case ELS_CODE_PRLO:               /* FC-PH-2 Table 119 */
     cmd_payload[1] = 0x00200008;
     cmd_payload[4] = 0x22000000;
     cmdsize = sizeof( PRLO );
     break;

   case ELS_CMD_LOGO:               /* FC-PH Table 73 */
     cmd_payload[1] = SWAP_LONG(binfo->fc_myDID);
     memcpy( &cmd_payload[2], &binfo->fc_sparam.portName.IEEE, sizeof(NAME_TYPE) );
     cmdsize = sizeof( LOGO );
     break;

   case ELS_CMD_SCR:               /* FLA */
     dprintf("Sending State Change Registration \n", p_args0);
     cmd_payload[1] = SWAP_LONG(RSCN_RTYPE_ALL_EVENTS);
     cmdsize = sizeof( SCR );
     break;

   default:
     dprintf("ELS command code %x not supported\n", p_args1(elstype) );
     break;

 }


 icmd->un.cont[0].bdeAddress = (int)cmd_payload | $MAP_FACTOR( pb );
 icmd->un.cont[0].bdeSize    = cmdsize + sizeof( int );
 icmd->un.cont[1].bdeAddress = (int)rsp_payload | $MAP_FACTOR( pb );
 icmd->un.cont[1].bdeSize    = ELS_RSP_PAYLOAD_SIZE;

 icmd->un.ulpWord[4]         = binfo->fc_myDID;   /* my id */
 icmd->un.ulpWord[5]         = did;
 icmd->un.ulpWord[6]         = iotag;

   /*
    * Save on code generated by just using a longword constant.
    *
    * icmd->ulpOwner              = OWN_CHIP;
    * icmd->ulpLe                 = 1;
    * icmd->ulpBdeCount           = 2;
    * icmd->ulpCommand            = CMD_ELS_REQUEST_CR;
    * icmd->ulpClass              = CLASS3;
    */
 icmd->un.ulpWord[7]         = 0x20a0b;


 status = kgpsa_send_iocb( pb, FC_ELS_RING, iocb_hdr );

 if ( status != msg_success ) {
   /*
    * did not get the iocb issued
    */
    dprintf("kgpsa_els_cmd: iocb not issued\n", p_args0);
    remq_lock(&iocb_hdr->iocb_q.flink);
    free( cmd_payload, "payload" );
    free( rsp_payload, "payload" );
    kgpsa_dealloc_iocbhdr( iocb_hdr );
    goto EXIT;
 }
 
 if (wait_for_completion) {
   kgpsa_krn$_post_wait( &pb->els_owner_s, &iocb_hdr->rsp , pb->pbx, "waiting for rsp" );
   kgpsa_krn$_wait( &pb->els_owner_s, pb->pbx, "waiting on ELS owner" );
   kgpsa_els_cmd_done( pb, iocb_hdr );
 }


EXIT:
 return status;
}



/*+
 * ============================================================================
 * = kgpsa_els_cmd_done - handle the result of an originated ELS command      =
 * ============================================================================
 *
-*/

int kgpsa_els_cmd_done( struct kgpsa_pb *pb, IOCB_HDR *iocb_hdr )
{
 int status = msg_success;
 int not_a_retryable_error = 0;
 int *cmd_payload;
 int *rsp_payload;
 int nstate;
 int port_index;
 IOCB *icmd, *irsp;           /* els_request_cr 0xA, els_request_cx 0xB */
 struct kgpsa_sb *sb;
 MAILBOX lmb, *mb=&lmb;
 FC_BRD_INFO *binfo;
 SERV_PARM   *sp;
 ELS_PKT    *elsreq, *elsrsp;
 int iotag;
 int elstype;
 int word4, word5;
 int did, word5did;
 struct PCB *pcb = getpcb();

 binfo       = &pb->binfo;
 icmd        = &iocb_hdr->cmd_iocb;
 irsp        = &iocb_hdr->rsp_iocb;
 iotag       = iocb_hdr->iotag;
 cmd_payload = iocb_hdr->bde1_bufptr;
 rsp_payload = iocb_hdr->bde2_bufptr;
 word4       = irsp->un.ulpWord[4];  /* error parameter */
 did         = iocb_hdr->did;
 word5did    = (irsp->un.ulpWord[5] & 0x00FFFFFF);

    elsreq = irsp->un.elsreq.elsReq.bdeAddress - $MAP_FACTOR(pb);
    elsrsp = irsp->un.elsreq.elsRsp.bdeAddress - $MAP_FACTOR(pb);
    dprintf("cmd_done Req elsCode : %x  Rsp elsCode : %x \n", 
                                p_args2(elsreq->elsCode, elsrsp->elsCode) );

    /*
     * In this implementation, the ELSes are always issued with a TAG_TYPE=0,
     * which means word5 of the IOCB contains a DID.  There is some bug in the
     * EMULEX adapter which does not allow having 2 outstanding ELSes to a
     * DID before it is logged in.   I do not think I have that situation, but
     * if a descrepancy occurs, shout out something.
     *
     */
    if( did != word5did ) {
      pprintf("kgpsa_els_cmd_done - did:%x != word5did:%x \n", did, word5did ); 
      dump_els( pb );
    }

    if (iocb_hdr->iocb_state == IOCB_INCARNERR) {
      switch(elsreq->elsCode) {
         case ELS_CMD_PDISC:
         case ELS_CMD_PLOGI:
         case ELS_CODE_PRLI:
	   /*
	    * 1-Dec-98
	    *
	    * This probe_state is being removed.  It was necessary in the
	    * old style of probing when the latt_process waited on the probe
	    * state to be done.   The new single threaded way we are not
            * waiting on the probe state.  (See FCLOG 126.*)
	    *
	    * Although it looks harmless to set the probe state to done 
	    * upon an incarnation failure, it is not right.  An new probe
	    * could have been kicked off, and a new scsi_targets list found
	    * and the setting of this probe state would not apply to the
	    * new incarnation of probing.  
	    *
	    * After some soak time, we can remove this comment.
	    *
            * probe_state( pb, PROBE_SET, PROBE_DONE, did);
            */
	   dprintf("INCARNERR did: %x \n", p_args1(did) );
           dprintf("cmd_done Req elsCode : %x  Rsp elsCode : %x \n", 
                                p_args2(elsreq->elsCode, elsrsp->elsCode) );
           goto EXIT;
	   break;
         case ELS_CMD_FLOGI:
         case ELS_CODE_PRLO:
         case ELS_CMD_LOGO:
         case ELS_CMD_RLS:
           break;
      } /* switch */
    }


    switch(elsreq->elsCode) {

      case ELS_CMD_PDISC:
        /*
	 * No longer supported.  See older versions of the kgpsa_driver.c
	 * if the support needs to be reinstated. (26-Jun-98)
	 */
        break;

      case ELS_CMD_FLOGI:             
      /*
       * FABRIC STATE CHANGE THREAD STARTING WITH LINK UP
       * --> You are here
       *
       *     1. Link up                               
       *     2. suspend FCP outgoing queue
       *     3. Log into Fabric
       * -->3a. Register with Fabric Controller for state change notification
       * --> 4. log into name server
       *     5. Receive login response
       *     6. give the ring a buffer
       *        some problems here I think.  Do we want to queue to an XRI
       *        instead?  What happens to the XRI queued buffer when the 
       *        exchange goes away.  - A RET_XRI_QUE_BUF returns the unused
       *        buffer.
       *        Do we just want to queue this to the FCP ring and have it there
       *        always and replenish it upon every receipt?
       *     7. Request the latest list of nodes with GP_ID3
       *     8. Receive the SCR completed response
       *     9. Receive asynchronously the data from the SCR
       *    10. Logout from the name server
       *    11. Probe the node list
       */      
	/*
	 * The switch gives us back our DID.  We use this from now on,
	 * but also, we have to tell the adapter about it via the
	 * CONFIG_LINK mailbox.
	 */
        if ( ( irsp->ulpStatus == IOSTAT_SUCCESS) &&
            ( elsrsp->elsCode == ELS_CMD_ACC)    ){
	  struct kgpsa_sb *sb2;
 	  binfo->fc_myDID = irsp->un.ulpWord[4];
	  sb = kgpsa_new_node( pb, did , rsp_payload+1 );
	  
          /*
           *
           * TODO: (strong)
           * Do I need to change pb->sb[0]->did to new fc_myDID.?
           *
           */
          sb2 = pb->pb.sb[0];
	  sb2->did = binfo->fc_myDID;

	  kgpsa_set_nstate( sb, NLP_LOGIN );
          kgpsa_config_link( pb, &lmb );
          kgpsa_reg_login( sb, mb, did );
          set_rpi( sb, mb->un.varRegLogin.rpi);
	  set_incarnation( sb, iocb_hdr );
          kgpsa_set_nstate( sb, NLP_LOGIN );
	  kgpsa_set_prli_state( sb, PRLI_NG );



      /*
       * Register with the Fabric controller (0xFFFFFD) that we want to
       * know about state changes.
       */
      dprintf("* -->3a. Register with Fabric Controller for state change notification\n", p_args0);
      kgpsa_els_cmd( pb, FABRIC_CONTROLLER, ELS_CMD_SCR, DONT_WAIT, DONT_RETRY );



    /*
     * Before we clear_la, we can talk to the fabrics name server and 
     * find out the other ports that are logged in.  This is permissable
     * by a new bit as of V2.00a Emulex firmware, which allows issueing an
     * XMIT_SEQUENCE prior to a clear_la.  Per Brocade Fabric
     * Programming Spec V1.3, "Once all the name server queries are done, the
     * Nx_Port must logout from the Name Server."
     */ 

      dprintf("* --> 4. log into name server\n", p_args0);
      kgpsa_els_cmd( pb, NAME_SERVER, ELS_CMD_PLOGI, DONT_WAIT, fc_els_retries ); 


        } else {
	 /*
          * TODO: (medium) (9-Nov-98)
          * Put some retry code in here.  This is the error flow.
          */
	}
	break;

      case ELS_CMD_PLOGI:
        if ( ( irsp->ulpStatus == IOSTAT_SUCCESS) &&
             ( elsrsp->elsCode == ELS_CMD_ACC)    ){
	  /*
	   * (Used to be under BEFORE_APRIL1)
	   * Only allow known ports unless we are in wwidmgr_mode
	   */
	  sp = rsp_payload+1;
	  if ( wwidmgr_mode==0 ) 
           {
           if ( (did != NAME_SERVER) && (did != FABRIC_CONTROLLER) ) 
            {
	    /*
	     * restrict login
	     */
            if ( fc_find_portname( &sp->portName, &port_index, FC_SWAP_BYTES ) 
                                                                        == 0 ) 
              {
	      dprintf("did %x portname not registered\n", p_args1(did) );
              kgpsa_els_cmd( pb, did, ELS_CMD_LOGO, DONT_WAIT, DONT_RETRY );
              probe_state( pb, PROBE_SET, PROBE_DONE, did);
              probe_next( pb );
              break;
              } /* if fc_find_portname */
             } /* if Name_server? */
           else 
             {
              dprintf("Fabric or Name_server should never be Fabric - did:%x\n",p_args1(did) );
	      dprintf("  always allow login\n", p_args0);
	     } /* else Name_server */
	  } /* wwidmgr_mode */




	  sb = kgpsa_new_node( pb, did , rsp_payload+1 );
          kgpsa_reg_login( sb, mb, did );
          set_rpi( sb, mb->un.varRegLogin.rpi);
	  set_incarnation( sb, iocb_hdr );
          kgpsa_set_nstate( sb, NLP_LOGIN );
	  kgpsa_set_prli_state( sb, PRLI_NG );
	  /*
	   * Successful PLOGI has occurred.  Now send off a PRLI to
	   * allow for FCP exchanges.  If we are talking to the Fabric
	   * services, like the NAME_SERVER (0xFFFFFC),
           * then there is no need to PRLI.
	   */
	  if( (did != NAME_SERVER) && (did != FABRIC_CONTROLLER) ) {
            kgpsa_els_cmd( pb, did, ELS_CMD_FCP_PRLI, DONT_WAIT, DONT_RETRY );
	  }



#if FABRIC_SUPPORT
      /*
       * FABRIC STATE CHANGE THREAD  (either after LINK UP or RSCN )
       * --> You are here
       *
       *     1. Receive notification of a state change
       *     2. Accept the RSCN
       *     3. suspend FCP outgoing queue
       *     4. log into name server
       * --> 5. Receive login response
       * --> 6. give the ring a buffer
       *        some problems here I think.  Do we want to queue to an XRI
       *        instead?  What happens to the XRI queued buffer when the 
       *        exchange goes away.  - A RET_XRI_QUE_BUF returns the unused
       *        buffer.
       *        Do we just want to queue this to the FCP ring and have it there
       *        always and replenish it upon every receipt?
       * --> 7. Request the latest list of nodes with GP_ID3
       *     8. Receive the SCR completed response
       *     9. Receive asynchronously the data from the SCR
       *    10. Logout from the name server
       *    11. Probe the node list
       */      
      if( did == NAME_SERVER)
         {
	  int *buf;
          sb = kgpsa_find_did( pb, NAME_SERVER );
          dprintf("* --> 5. Receive login response\n", p_args0);
          dprintf("* --> 6. give the ring a buffer\n", p_args0);
          dprintf("* --> 7. Request the latest list of nodes with GP_ID3\n", p_args0);
          kgpsa_xmit_seq( pb, sb->rpi, GP_ID3, DONT_WAIT );
          break;
         }
#endif FABRIC_SUPPORT


        } else {


          /*
           * IOCB indicates an error of this PLOGI.  Some errors should be
	   * retried.  Since this implementation issues the ELSes in class 3,
	   * they could get dropped by the wire or hub or switch or target.
	   * Some errors give a status indicating that we should not bother
	   * retrying.   A LOOP_OPEN_FAILURE means that the target is not
	   * present on the loop - we dont retry that.   And if we find an
	   * error due to link down, then when the link comes up we will 
	   * kick off this probing again, so we can back out of this attempt
	   * which would be of an old incarnation anyway.
           */
	  dprintf("PLOGI failed to %x \n", p_args1(did) );
          not_a_retryable_error = 
            ( 
              ( irsp->ulpStatus == IOSTAT_LS_RJT )              ||

              ( ( irsp->ulpStatus == IOSTAT_LOCAL_REJECT)         &&
                (
                  ( irsp->un.fcpi.fcpi_parm == IOERR_LOOP_OPEN_FAILURE)  ||
                  ( irsp->un.fcpi.fcpi_parm == IOERR_LINK_DOWN)          
                )   
              )

            );

          if( (iocb_hdr->retries > 0) && (!not_a_retryable_error) )
	        {
		  d2printf("pb: %x retry iocb %d on did:%x ulpStatus: %x  parm: %x\n", 
                       p_args5(pb, iocb_hdr->retries, did, irsp->ulpStatus, irsp->un.fcpi.fcpi_parm) );
		  iocb_hdr->retries--;
                  kgpsa_els_cmd( pb, did, ELS_CMD_PLOGI, DONT_WAIT, iocb_hdr->retries );
		} 
              else
                {
          if(1) {
  	      sb = kgpsa_find_did( pb, did );
              if (sb!=NULL) {
	        d2printf("A valid node has dropped off link: %x\n", 
                                                           p_args1(did) );
	        kgpsa_set_nstate( sb, NLP_LOGOUT );
	        sb->valid = 0;
              } else {
	        dprintf("Null sb- PLOGI - did : %x\n", p_args1( did ) );
	      }
	   }
          probe_state( pb, PROBE_SET, PROBE_DONE, did);

          probe_next(pb);

	}

        }
        break;

      case ELS_CODE_PRLI:
        if ( ( irsp->ulpStatus == IOSTAT_SUCCESS) &&
             ( elsrsp->elsCode == ELS_CMD_ACC)    ){
	   sb = kgpsa_find_did( pb, did );
           if (sb!=NULL) {
             kgpsa_set_prli_state( sb, PRLI_OK );
	     sb->scsi_target = elsrsp->un.prli.targetFunc;
	   } else {
	     pprintf("Null sb - PRLI - did: %x\n", p_args1(did) );
	   }
        } else  {
	  /*
	   * TODO: (optional)
           * We could enhance this code by retrying a PRLI if it fails.
           * Instead however, I choose the less code solution and do nothing.
	   * If the PRLI does not succede, then when an FCP gets issued,
	   * the target node will PRLO me and that will kick off the retry.
	   * Or if the FCP just timed out, we would kick off a retry.  
	   * The retry of the PRLI is left to an on-demand condition, that is
	   * when the FCP fails.
	   */
	  dprintf("PRLI failure to did: %x ulpStatus:%x elsCode:%x\n", 
                            p_args3( did, irsp->ulpStatus, elsrsp->elsCode ) );
        }
        probe_state( pb, PROBE_SET, PROBE_DONE, did);

        probe_next( pb );

        break;

      case ELS_CODE_PRLO:
        /*
	 * No error check is made.   I do not think there is anything
	 * significant about console erroneously thinking it is PRLOed.
	 */
	sb = kgpsa_find_did( pb, did );
        if (sb!=NULL) {
           kgpsa_set_prli_state( sb, PRLI_NG );
	} else {
	   pprintf("Null sb - PRLO - did: %x\n", p_args1(did) );
	}
        break;

      case ELS_CMD_LOGO:
        /*
	 * No error check is made.   I do not think there is anything
	 * significant about console erroneously thinking its LOGO got thru.
	 */
	sb = kgpsa_find_did( pb, did );
        if (sb!=NULL) {
	   /*
	    * TODO: Do I want to do this unreg_login stuff with the
	    * results of the LS_RJT too?
	    */

	   /*
            * Check sb->valid bit before using the sb->rpi
            * Suppose a PLOGI comes in from some node I have an N* ev
	    * allowing.   I would LOGO it.  My sb info may have a old
	    * stale sb->rpi.  So when the logo completes, I need to
	    * get the right RPI to unreg_login.
	    */
	   int rpi;
	   rpi = kgpsa_read_rpi( pb, did );
	   if( rpi )
 	     kgpsa_unreg_login( pb, rpi );  
  	   kgpsa_set_nstate( sb, NLP_LOGOUT );
           kgpsa_set_prli_state( sb, PRLI_NG );
	} else {
	   int rpi;
	   dprintf("NULL sb - LOGO - did: %x\n", p_args1(did) );
	   rpi = kgpsa_read_rpi( pb, did );
	   if( rpi )
 	     kgpsa_unreg_login( pb, rpi );  
	}
        break;

      case ELS_CMD_SCR:               /* FLA */
	dprintf("SCR done\n", p_args0);
        break;

      case ELS_CMD_RLS:
        break;
    } /* switch */

 goto EXIT;


EXIT:
 free( cmd_payload, "payload" );
 free( rsp_payload, "payload" );
 kgpsa_dealloc_iocbhdr( iocb_hdr );
 return status;

}


/*+
 * ============================================================================
 * = kgpsa_els_rsp - respond to an ELS that originated from a remote n_port   =
 * ============================================================================
 *
 *  Respond to an ELS that came in off the wire from a remote n_port.
 *  No state change or action is taken until the kgpsa_els_rsp_done, when
 *  we either know or assume the command has completed.
 *
-*/
int kgpsa_els_rsp( struct kgpsa_pb *pb, int elstype, int xri, int did,
                                                        int rpi, int state )
{

 int status = msg_success;
 int *cmd_payload;
 int *rsp_payload;
 IOCB *icmd, *irsp;
 IOCB_HDR *iocb_hdr;
 FC_BRD_INFO *binfo;
 SERV_PARM   *sp;
 ELS_PKT *els;
 int idx;
 int iotag;
 int elscode;
 int cmdsize;

 dprintf("kgpsa_els_rsp\n", p_args0);


 pb->elscmdcnt++;
 iotag = pb->elscmdcnt;
 dprintf("els rsp iotag: %x  elstype: %s\n", 
                p_args2(iotag, elstype_text[elstype&0xFF]) );

 iocb_hdr = kgpsa_alloc_iocbhdr( pb, iotag, elstype_text[elstype&0xFF] );
 iocb_hdr->did = did;
 iocb_hdr->rpi = rpi;
 iocb_hdr->iocb_state = state;
 iocb_hdr->pb = pb;
 iocb_hdr->sb = kgpsa_find_did( pb, did );   /* May be zero */

 binfo = &pb->binfo;

 icmd = &iocb_hdr->cmd_iocb;
 irsp = &iocb_hdr->rsp_iocb;
 cmd_payload = malloc( ELS_CMD_PAYLOAD_SIZE, "payload" );
 els = cmd_payload;
 iocb_hdr->bde1_bufptr = cmd_payload;

 cmd_payload[0] = elstype;
 elscode = els->elsCode;
 switch(elscode) {
   case ELS_CMD_LS_RJT:                      /* FC-PH 21.5.2 */
     dprintf("issueing a LS_RJT\n", p_args0);
     cmdsize = sizeof( LS_RJT );
     switch ( state ) {
       case PLOGI_LS_RJT_SENT:
       case PDISC_LS_RJT_SENT:
         cmd_payload[1]   = LSRJT_CMD_UNSUPPORTED<<8;
         cmd_payload[2]   = 0;
         break;
       default:
         cmd_payload[1]	  = 0x0;
         cmd_payload[2]   = 0;
         break;
     };
   break;

   case ELS_CMD_ACC:
     /*
      * A PLOGI ACC returns the 0x70 bytes of service parameters + 4 bytes
      * A LOGO ACC just returns 4 bytes
      * A PDISC ACC returns the 0x70 bytes of service parameters + 4 bytes
      */
     switch ( state ) {

       case LOGO_ACC_SENT:		    /* FC-PH Table 74 */
	 cmdsize = 0;
         break;

       case PLOGI_ACC_SENT:                 /* FC-PH Table 72 */
	 /*
	  * This case is obsolete since we took up the policy of always 
	  * being the PLOGI initiator.  At this time, no storage targets
	  * initiate with a PLOGI, so this is an acceptable policy.  Should
	  * this policy change, go back to older consoles, before April 1, 98,
	  * and the code could be found there but some work is needed.
          * Because, currently, kgpsa_els_rcv will only send an LS_RJT to
	  * a PLOGI, we should never get here.
	  */
         pprintf("kgpsa_els_rsp - PLOGI_ACC - should not get here!\n");
         break;

       case PDISC_ACC_SENT:                 /* FC-PH-2 21.19.1 */
	 /*
	  *  No longer supported.  See older versions of this code if we
	  *  ever need to support it.  (like before April 98 ).
	  */
         break;

       case PRLI_ACC_SENT:                  /* FC-PH-2 Table 116  */
       case PRLO_ACC_SENT:                  /* FC-PH-2 Table 120  */
         dprintf("issueing an PRLI/PRLO ACCEPT\n", p_args0);
         cmdsize         = PRLX_PAGE_LEN;
         cmd_payload[1]  = 0x00200008;
         cmd_payload[4]  = 0x22000000;
         break;

       case RSCN_ACC_SENT:
	 cmdsize = 0;
	 dprintf("NYI - issueing a RSCN ACCEPT \n", p_args0);
         break;

     }
     break;

  default:
    dprintf("default - Not yet implemented - sending back garbage.\n", p_args0);
    break;
 }
 icmd->un.cont[0].bdeAddress = (int)cmd_payload | $MAP_FACTOR( pb );
 icmd->un.cont[0].bdeSize    = cmdsize + sizeof( int );
 icmd->un.ulpWord[6]         = (xri<<16) | iotag;

   /*
    * Save on code generated by just using a longword constant.
    *
    * icmd->ulpOwner              = OWN_CHIP;
    * icmd->ulpLe                 = 1;
    * icmd->ulpBdeCount           = 1;
    * icmd->ulpCommand            = CMD_XMIT_ELS_RSP_CX;
    * icmd->ulpClass              = CLASS3;
    */
 icmd->un.ulpWord[7]         = 0x21507;

 status = kgpsa_send_iocb( pb, FC_ELS_RING, iocb_hdr );

 if ( status != msg_success ) {
   /*
    * did not get the iocb issued
    */
    dprintf("kgpsa_els_rsp: iocb not issued\n", p_args0);
    remq_lock(&iocb_hdr->iocb_q.flink);
    goto EXIT;
 }


EXIT:
 return status;
}


/*+
 * ============================================================================
 * = kgpsa_els_rsp_done - handle the ELS response when it is done             =
 * ============================================================================
 *
 *
 *
 *
-*/
void kgpsa_els_rsp_done( struct kgpsa_pb *pb, IOCB_HDR *iocb_hdr )
{
 int *buf;
 int status;
 MAILBOX     lmb, *mb=&lmb;
 struct kgpsa_sb *sb;
 IOCB *rspcmd;

 dprintf("kgpsa_els_rsp_done\n", p_args0);
 dprintf("els_rsp_done - %s ", p_args1(iocb_state_text[iocb_hdr->iocb_state]) );
 dprintf(" rsp status: %x\n", p_args1(iocb_hdr->rsp_iocb.ulpStatus) );

 rspcmd = &iocb_hdr->cmd_iocb;
 switch( iocb_hdr->iocb_state ) {
   case PLOGI_ACC_SENT:
     /*
      * The code to handle this was removed on 28-Jun-98.  If we ever need
      * to support PLOGIs that are initiated from a remote n_port, we can
      * put that code back.  But the latest policy is that this n_port must
      * initiate the PLOGI.   Since kgpsa_els_rcv only sends an LS_RJT
      * to an PLOGI recieved, we should never get here.
      */
     pprintf("kgpsa_els_rsp_done - PLOGI_ACC - should never get here.\n");
     break;

   case PRLO_ACC_SENT:
	 /*
	  * (9-Nov-98)
          * We could do an unreg_login here.  Think about it.  We are only
	  * doing FCP PRLIs to storage devices.  If a PRLO came in, it
	  * most likely means that we sent out and FCP to a device that 
	  * somehow did not think we did the PRLI.  Either the PRLI got
	  * lost or failed (PRLI errors are not retried).   The FCP would
	  * eventually time out, which takes too long.  If we do clobber 
	  * the whole login state here, that is our fastest way out.
          *
          * see FCLOG 228.2 for further discussion.
          *
          */
   case PLOGI_LS_RJT_SENT:
   case LOGO_ACC_SENT:
   case LOGO_LS_RJT_SENT:
     /*
      * We shouldn't really care about success or fail status on the 
      * response.  Behave as if it succeeded.
      */
     sb = kgpsa_find_did( pb, iocb_hdr->did );
     if(!sb){
        dprintf("RJT/LOGO_ACC no sb found for did: %x\n", p_args1(iocb_hdr->did) );
     }
     if (sb) {
       kgpsa_set_prli_state( sb, PRLI_NG );
       kgpsa_set_nstate( sb, NLP_LOGOUT );
       if ( iocb_hdr->rpi != sb->rpi ) {
	 dprintf("PANIC: LOGO rpi's inconsistent. iocb_hdr->rpi:%x sb->rpi:%x\n",
                             p_args2(iocb_hdr->rpi, sb->rpi) );
         if (iocb_hdr->rsp_iocb.ulpStatus != IOSTAT_SUCCESS) {
	   pprintf("...on a failed response.  \n");
	 }
       }

     }
     /*
      * TODO:  Should we avoid this if we have a failed response.  I think so
      *        because what does that mean about the rpi?
      *        28-jun-98 look into this some more.  I suppose the rpi could
      *        be reused and we might unreg_login erroneously.  Is there
      *        some safer way.
      */
     kgpsa_unreg_login( pb, iocb_hdr->rpi ); 
     break;


    case RSCN_ACC_SENT:

      /*
       * Up the RSCNeventTag.  That will suspend the FCP queues.
       */
      dprintf("* --> 3. suspend FCP outgoing queue\n", p_args0);
      pb->RSCNeventTag++;
      dprintf("New RSCNeventTag: %x\n", p_args1(pb->RSCNeventTag) );
      
      pb->DiscLINKUPeventTag = pb->LINKUPeventTag;
      pb->DiscRSCNeventTag   = pb->RSCNeventTag;
      kgpsa_set_DiscState( pb, DISCOVERY_NEEDED, "RSCN" );
      kgpsa_unreg_login( pb, 0xFFFF);
      /*
       *  We do not FLOGI again to the Fabric controller.  That would
       *  just cause everybody more SCR notifications and it would result
       *  in endless probing.
       */
      sb = kgpsa_find_did( pb, FABRIC_F_PORT );
      kgpsa_reg_login( sb, mb, FABRIC_F_PORT );
      probe_state( pb, PROBE_SET, PROBE_IN_PROGRESS, ALL_DID );

      /*
       * Log into name server;  Am I already logged in?  Does it matter if
       * we are already logged in?
       */
      dprintf("* --> 4. log into name server\n", p_args0);
      kgpsa_els_cmd( pb, NAME_SERVER, ELS_CMD_PLOGI, DONT_WAIT, fc_els_retries ); 
      break;


    case PDISC_ACC_SENT:
    case LS_RJT_SENT:
    case PDISC_LS_RJT_SENT:
    case PRLI_ACC_SENT:
    case PRLI_LS_RJT_SENT:
    case PRLO_LS_RJT_SENT:
    case IOCB_ABORTED:
    case IOCB_INCARNERR:
    default:
      break;

 } /* switch */


EXIT:

 if (rspcmd->un.cont[0].bdeAddress) {
   buf = rspcmd->un.cont[0].bdeAddress - $MAP_FACTOR(pb) ; 
   dprintf("els_rsp_done freeing %x\n", p_args1(buf) );
   free( buf, "buf" );
 } else {
   dprintf("***** zero buffer condition [2] *****\n", p_args0);
 }
 kgpsa_dealloc_iocbhdr( iocb_hdr );
}


#if FABRIC_SUPPORT
/*+
 * ============================================================================
 * = kgpsa_xmit_seq -                                                         =
 * ============================================================================
 *
 * This is a general service which is only used by the console to talk to
 * the fabric name server.  Therefore the feature of issueing this command
 * before a CLEAR_LA is enabled by setting Bit 3 in Word 5.  This feature
 * is available in Emulex adapter firmware from V2.0 on.
 *
 * Note that the caller must have the els_owner_s.
 *
 * At this time (Nov 98), this routine is only called from the 
 * kgpsa_els_response routine which waits on the els_owner_s.
 *
 * Note that this iocb will be issued to the FCP ring.   There is a long 
 * discussion of this in a comment up above in the section called 
 * "General discussion on the driver:".  
 * 
-*/
int kgpsa_xmit_seq( struct kgpsa_pb *pb, int rpi, int xmittype, int wait_for_completion )
{
 int status = msg_success;
 int *cmd_payload;
 int *rsp_payload;
 int nstate;
 struct kgpsa_sb *sb;
 IOCB *icmd, *irsp;
 MAILBOX mbx;
 IOCB_HDR *iocb_hdr;
 FC_BRD_INFO *binfo;
 SERV_PARM   *sp;
 int iotag;
 int al_pa;
 ELS_PKT *els;
 int elscode;


 /*
  * We tag this request with an FCP counter because it is going onto the
  * the FCP ring.  We do not have the fcp_owner_s.  That is because 
  * we are already under els_owner_s, and the fcpcmdcnt is only incremented
  * under els_owner_s.   Then when we issue the iocb, we do have the
  * ring_s semaphore for the fcp ring.
  */
 pb->fcpcmdcnt++; 
 iotag = pb->fcpcmdcnt;

 iocb_hdr = kgpsa_alloc_iocbhdr( pb, iotag, "XMIT" );
 iocb_hdr->rpi                 = rpi;
 iocb_hdr->wait_for_completion = wait_for_completion;
 iocb_hdr->pb                  = pb;
 iocb_hdr->sb = kgpsa_find_did( pb, rpi );
 sb = iocb_hdr->sb;
 iocb_hdr->did = sb->did;

 binfo = &pb->binfo;
 icmd        = &iocb_hdr->cmd_iocb;
 irsp        = &iocb_hdr->rsp_iocb;
 cmd_payload = malloc( ELS_CMD_PAYLOAD_SIZE, "payload" );
 els         = cmd_payload;
 iocb_hdr->bde1_bufptr = cmd_payload;
 rsp_payload = malloc( PAYLOAD_SIZE, "payload" );
 iocb_hdr->bde2_bufptr = rsp_payload;


 switch(xmittype) {

#if CT_EXTRA
    /* 
     * GPN Get port name given its address identifier
     */
    case GPN:    
       cmd_payload[0]     = SWAP_LONG(0x01000000);
       cmd_payload[1]     = SWAP_LONG(0xFC020000);
       cmd_payload[2]     = SWAP_LONG(0x01120000);
       cmd_payload[4]     = SWAP_LONG(0x00200413);
      break;

    /* 
     * GCoS Get Class of Service
     */
    case GCoS:    
       cmd_payload[0]     = SWAP_LONG(0x01000000);
       cmd_payload[1]     = SWAP_LONG(0xFC020000);
       cmd_payload[2]     = SWAP_LONG(0x01140000);
       cmd_payload[4]     = SWAP_LONG(0x00200413);
      break;


    /* 
     * GPT Get port type
     */
    case GPT:    
       cmd_payload[0]     = SWAP_LONG(0x01000000);
       cmd_payload[1]     = SWAP_LONG(0xFC020000);
       cmd_payload[2]     = SWAP_LONG(0x011A0000);
       cmd_payload[4]     = SWAP_LONG(0x00200413);
      break;


    /* 
     * GP_ID2 Get address identifier given a NodeName
     */
    case GP_ID2:
       cmd_payload[0]     = SWAP_LONG(0x01000000);
       cmd_payload[1]     = SWAP_LONG(0xFC020000);
       cmd_payload[2]     = SWAP_LONG(0x01310000);

/*TODO:*/       cmd_payload[4]     = SWAP_LONG(0x00000008);
      break;


#endif /* CT_EXTRA */


    /* 
     * GP_ID3 Get address identifier given and FC-4 type value
     */
    case GP_ID3:    
       cmd_payload[0]     = SWAP_LONG(0x01000000);
       cmd_payload[1]     = SWAP_LONG(0xFC020000);
       cmd_payload[2]     = SWAP_LONG(0x01710000);
       cmd_payload[4]     = SWAP_LONG(0x00000008);
      break;

    default:
      dprintf("XMIT command code %x not supported\n", p_args1(xmittype) );
      break;
 }


 icmd->un.cont[0].bdeAddress = (int)cmd_payload | $MAP_FACTOR( pb );
 icmd->un.cont[0].bdeSize    = 5*4;
 icmd->un.cont[1].bdeAddress = (int)rsp_payload | $MAP_FACTOR( pb );
 icmd->un.cont[1].bdeSize    = PAYLOAD_SIZE;
 icmd->un.ulpWord[5]         = 0x0220000C;  /* r_ctl  = 0x02
                                               type   = 0x20
                                               df_ctl = 0x00
                                               f_ctl  = 0x0C comes from  
                                                        0x04 (SI) from 0x290000
                                                        OR'ed with 0x8 to 
                                                        allow before CLEAR_LA
					     */
 icmd->un.ulpWord[6]         = (rpi<<16) | iotag;
   /*
    * Save on code generated by just using a longword constant.
    *
    * icmd->ulpOwner              = OWN_CHIP; 
    * icmd->ulpLe                 = 1;
    * icmd->ulpBdeCount           = 2;
    * icmd->ulpCommand            = CMD_XMIT_SEQUENCE_CR;
    * icmd->ulpClass              = CLASS3;
    */
 icmd->un.ulpWord[7]         = 0x20207;

 status = kgpsa_send_iocb( pb, FC_FCP_RING, iocb_hdr );  /* issue to ring 2 */

 if ( status != msg_success ) {
   /*
    * did not get the iocb issued
    */
    dprintf("kgpsa_els_cmd: iocb not issued\n", p_args0 );
    remq_lock(&iocb_hdr->iocb_q.flink);
    free( cmd_payload, "payload" );
    free( rsp_payload, "payload" );
    kgpsa_dealloc_iocbhdr( iocb_hdr );
    goto EXIT;
 }

 /*
  * Set a counter of fc_process intervals to wait for the CT to come back.
  * If it does not come back in time, we will init the link.
  */
 pb->CT_late_intvcnt = CT_late_int_cnt; 
 
 if (wait_for_completion) {
   kgpsa_krn$_post_wait( &pb->els_owner_s, &iocb_hdr->rsp , pb->pbx, "waiting for rsp" );
   kgpsa_krn$_wait( &pb->els_owner_s, pb->pbx, "waiting on ELS owner" );
   kgpsa_els_cmd_done( pb, iocb_hdr );
 }

EXIT:
 return status;
}

int kgpsa_xmit_seq_done( struct kgpsa_pb *pb, IOCB_HDR *iocb_hdr )
{
 int status = msg_success;
 int *cmd_payload;
 int *rsp_payload;

 cmd_payload = iocb_hdr->bde1_bufptr;
 rsp_payload = iocb_hdr->bde2_bufptr;

 /*
  * Check out error conditions
  */
  
      dprintf("* --> 8. Receive the SCR completed response\n", p_args0);

EXIT:
 free( cmd_payload, "payload" );
 free( rsp_payload, "payload" );
 kgpsa_dealloc_iocbhdr( iocb_hdr );
 return status;

}

#endif  /* FABRIC_SUPPORT */


void kgpsa_els_abort_done( struct kgpsa_pb *pb, IOCB_HDR *iocb_hdr)
{
 dprintf("kgpsa_els_abort_done\n", p_args0);
 kgpsa_dealloc_iocbhdr( iocb_hdr );
 /* dealloc the BDE buffers too */
}


/*
 * kgpsa_abort_xri_cx
 *
 * caller must wait on els_owner_s 
 */
int kgpsa_abort_xri_cx( struct kgpsa_pb *pb,  unsigned int xri )
{
 int status = msg_success;
 IOCB *icmd, *irsp;
 IOCB_HDR *iocb_hdr;
 int iotag;

 pb->elscmdcnt++;
 iotag = pb->elscmdcnt;
 iocb_hdr = kgpsa_alloc_iocbhdr( pb, iotag, "ABTXRI" );
 iocb_hdr->wait_for_completion = DONT_WAIT;


 /*
  *  A CMD_ABORT_XRI_CN will fail if the link is down and ABTS (word3=1)
  *  is specified.  Also, since this is being used when the link is
  *  down, it has to be issued on the ELS ring.   The FCP ring is stalled
  *  until the CLEAR_LA.
  */

 icmd = &iocb_hdr->cmd_iocb;
 irsp = &iocb_hdr->rsp_iocb;
 icmd->un.ulpWord[3]         = FC_ABTS;     /* NO ABTS (when link is down) */
 icmd->un.ulpWord[6]         = (xri<<16)|iotag;
 icmd->ulpOwner              = OWN_CHIP;
 icmd->ulpLe                 = 1;
 icmd->ulpBdeCount           = 0;
 icmd->ulpCommand            = CMD_ABORT_XRI_CX;
 icmd->ulpClass              = CLASS3;

 status = kgpsa_send_iocb( pb, FC_ELS_RING, iocb_hdr );

 if ( status != msg_success ) {
   /*
    * did not get the iocb issued
    */
    dprintf("kgpsa_abort_xri_cn: iocb not issued\n", p_args0);
    remq_lock(&iocb_hdr->iocb_q.flink);
    goto EXIT;
 }


EXIT:
 return status;
}

#if ABORTS
volatile int abts = FC_ABTS;

int kgpsa_abort_xri_cn( struct kgpsa_pb *pb, int ring, unsigned int word5 )
{
 int status = msg_success;
 IOCB *icmd, *irsp;
 IOCB_HDR *iocb_hdr;
 int iotag;



 iotag = 0xbeef;
 iocb_hdr = kgpsa_alloc_iocbhdr( pb, iotag, "ABTXRI" );
 iocb_hdr->wait_for_completion = DONT_WAIT;


 /*
  *  A CMD_ABORT_XRI_CN will fail if the link is down and ABTS (word3=1)
  *  is specified.  Also, since this is being used when the link is
  *  down, it has to be issued on the ELS ring.   The FCP ring is stalled
  *  until the CLEAR_LA.
  */

 icmd = &iocb_hdr->cmd_iocb;
 irsp = &iocb_hdr->rsp_iocb;
 icmd->un.ulpWord[3]         = abts;     /* NO ABTS (when link is down) */
 icmd->un.ulpWord[4]         = 0;
 icmd->un.ulpWord[5]         = word5;       /* rpi,,iotag */
 icmd->un.ulpWord[6]         = iotag;
 icmd->ulpOwner              = OWN_CHIP;
 icmd->ulpLe                 = 1;
 icmd->ulpBdeCount           = 0;
 icmd->ulpCommand            = CMD_ABORT_XRI_CN;
 icmd->ulpClass              = CLASS3;

 status = kgpsa_send_iocb( pb, ring, iocb_hdr );

 if ( status != msg_success ) {
   /*
    * did not get the iocb issued
    */
    dprintf("kgpsa_abort_xri_cn: iocb not issued\n", p_args0);
    remq_lock(&iocb_hdr->iocb_q.flink);
    goto EXIT;
 }


EXIT:
 return status;
}

int kgpsa_close_xri_cr( struct kgpsa_pb *pb, int ring, unsigned int word5 )
{
 int status = msg_success;
 IOCB *icmd, *irsp;
 IOCB_HDR *iocb_hdr;
 int iotag;



 iotag = 0xdead;
 iocb_hdr = kgpsa_alloc_iocbhdr( pb, iotag, "CLOSEXRI" );
 iocb_hdr->wait_for_completion = DONT_WAIT;

 icmd = &iocb_hdr->cmd_iocb;
 irsp = &iocb_hdr->rsp_iocb;
 icmd->un.ulpWord[3]         = 0;
 icmd->un.ulpWord[4]         = 0;
 icmd->un.ulpWord[5]         = word5;       /* rpi,,iotag */
 icmd->un.ulpWord[6]         = iotag;
 icmd->ulpOwner              = OWN_CHIP;
 icmd->ulpLe                 = 1;
 icmd->ulpBdeCount           = 0;
 icmd->ulpCommand            = CMD_CLOSE_XRI_CR;
 icmd->ulpClass              = 0;

 status = kgpsa_send_iocb( pb, ring, iocb_hdr );

 if ( status != msg_success ) {
   /*
    * did not get the iocb issued
    */
    dprintf("kgpsa_close_xri_cn: iocb not issued\n", p_args0);
    remq_lock(&iocb_hdr->iocb_q.flink);
    goto EXIT;
 }


EXIT:
 return status;
}

int kgpsa_close_xri_cx( struct kgpsa_pb *pb, unsigned int xri )
{
 int status = msg_success;
 IOCB *icmd, *irsp;
 IOCB_HDR *iocb_hdr;
 int iotag;



 iotag = 0xdead;
 iocb_hdr = kgpsa_alloc_iocbhdr( pb, iotag, "CLOSEXRI" );
 iocb_hdr->wait_for_completion = DONT_WAIT;

 icmd = &iocb_hdr->cmd_iocb;
 irsp = &iocb_hdr->rsp_iocb;
 icmd->un.ulpWord[6]         = (xri<<16)|iotag;
 icmd->ulpOwner              = OWN_CHIP;
 icmd->ulpLe                 = 1;
 icmd->ulpBdeCount           = 0;
 icmd->ulpCommand            = CMD_CLOSE_XRI_CX;
 icmd->ulpClass              = 0;

 status = kgpsa_send_iocb( pb, FC_ELS_RING, iocb_hdr );

 if ( status != msg_success ) {
   /*
    * did not get the iocb issued
    */
    dprintf("kgpsa_close_xri_cn: iocb not issued\n", p_args0);
    remq_lock(&iocb_hdr->iocb_q.flink);
    goto EXIT;
 }


EXIT:
 return status;
}


#endif



/*
 *########################################################################
 *#########                    FCP                                ########
 *########################################################################
 */



/*+
 * ============================================================================
 * = kgpsa_fcp_cmd - construct iocb and issue FCP command                     =
 * ============================================================================
 *
-*/
int kgpsa_fcp_cmd( struct kgpsa_pb *pb, int fcptype, struct srb *psrb )
{
 int i, status = msg_success;
 int *cmd_payload;
 int *rsp_payload;
 int ulpLe;
 int rpi;
 int iotag;
 unsigned char *scsi_cmd;
 struct kgpsa_sb *sb;
 IOCB *icmd, *icont, *irsp;
 IOCB_HDR *iocb_hdr;


 sb = psrb->sb;

 /*
  * FCP IOCBs are RPI based.  Briefly grab the els_owner semaphore since there
  * may be some probing going on.  The sb->rpi and its sb->incarnation have
  * to be read together.
  */
 kgpsa_krn$_wait( &pb->els_owner_s, pb->pbx, "FCP bld iocb" );
 pb->fcpcmdcnt++;
 iotag = pb->fcpcmdcnt;
 iocb_hdr = kgpsa_alloc_iocbhdr( pb, iotag, "SCSI" );
 rpi = psrb->sb->rpi;
 iocb_hdr->sb = psrb->sb;
 iocb_hdr->did = psrb->sb->did;
 iocb_hdr->LINKUPeventTag = sb->LINKUPeventTag;
 iocb_hdr->RSCNeventTag = sb->RSCNeventTag;
 kgpsa_krn$_post( &pb->els_owner_s, pb->pbx, "FCP bld iocb");

 icmd = &iocb_hdr->cmd_iocb;
 icont = &iocb_hdr->cont_iocb;
 irsp = &iocb_hdr->rsp_iocb;

 cmd_payload = malloc( FCP_CMD_PAYLOAD_SIZE, "payload" );
 rsp_payload = malloc( PAYLOAD_SIZE, "payload" );
 scsi_cmd    = &cmd_payload[3];
 memset( psrb->dat_in, 0, psrb->dat_in_len );             /* fcp data payload */

 /* 
  * The cmd_payload is the integer manipulation of the FCP_CMD
  * structure.  This code manipulates it as ints to save on code
  * generated.
  */

 cmd_payload[0]    = psrb->lun<<8;               /* lun low  */
 cmd_payload[1]    = 0;                          /* lun high */


 switch(fcptype) {
   case CMD_FCP_ICMND_CR:
     ulpLe = 1;
     cmd_payload[2]    = UNTAGGED<<8;            /* fcp_cntl field */
     break;
   case CMD_FCP_IREAD_CR:
     ulpLe = 0;
     cmd_payload[2]    = (READ_DATA<<24)|(UNTAGGED<<8); /* fcp_cntl field */
     cmd_payload[7]    = SWAP_LONG(psrb->dat_in_len);
     icont->un.cont[0].bdeAddress = (int)psrb->dat_in | $MAP_FACTOR( pb );   /* fcp cmnd payload */
     icont->un.cont[0].bdeSize    = psrb->dat_in_len;
     break;
   case CMD_FCP_IWRITE_CR:
     ulpLe = 0;
     cmd_payload[2]    = (WRITE_DATA<<24)|(UNTAGGED<<8); /* fcp_cntl field */
     cmd_payload[7]    = SWAP_LONG(psrb->dat_out_len);
     icont->un.cont[0].bdeAddress = (int)psrb->dat_out | $MAP_FACTOR( pb );   /* fcp cmnd payload */
     icont->un.cont[0].bdeSize    = psrb->dat_out_len;
     break;
 }/* switch */

 for (i=0; i < 12; i++) {
   scsi_cmd[i] = psrb->cmd[i];
 }

 icmd->un.cont[0].bdeAddress = (int)cmd_payload | $MAP_FACTOR( pb );   /* fcp cmnd payload */
 icmd->un.cont[0].bdeSize    = FCP_CMD_PAYLOAD_SIZE;
 icmd->un.cont[1].bdeAddress = (int)rsp_payload | $MAP_FACTOR( pb );
 icmd->un.cont[1].bdeSize    = PAYLOAD_SIZE;
 icmd->un.ulpWord[6]         = (rpi << 16) | iotag;
 icmd->ulpOwner              = OWN_CHIP;
 icmd->ulpLe                 = ulpLe;
 icmd->ulpBdeCount           = 2;
 icmd->ulpCommand            = fcptype;
 icmd->ulpClass              = CLASS3;

 icmd->ulpRsvdByte           = kgpsa_fcp_timeout;  /* timeout in seconds */

 if ( !ulpLe ) {
   icont->un.ulpWord[6]         = iotag;
   /*
    * Save on code generated by just using a longword constant.
    *
    * icont->ulpOwner              = OWN_CHIP;
    * icont->ulpLe                 = 1;
    * icont->ulpBdeCount           = 1;
    * icont->ulpCommand            = CMD_IOCB_CONTINUE_CN;
    * icont->ulpClass              = CLASS3;
    */
   icont->un.ulpWord[7]         = 0x20807;
 }


 krn$_seminit( &iocb_hdr->xrsp_t.sem, 0, "fcp_isr" );
 krn$_init_timer( &iocb_hdr->xrsp_t );
 krn$_start_timer( &iocb_hdr->xrsp_t, 30000 );
  
 status = kgpsa_send_iocb( pb, FC_FCP_RING, iocb_hdr );

 if ( status != msg_success ) {
   /*
    * did not get the iocb issued
    */
    dprintf("kgpsa_fcp_cmd: iocb not issued. iotag:%x\n", 
                                                  p_args1(iocb_hdr->iotag) );
    remq_lock(&iocb_hdr->iocb_q.flink);
    psrb->sts[0] = scsi_k_sts_busy;
    status = fc_sts_incarnerr;
    goto EXIT;
 }


 kgpsa_krn$_wait( &iocb_hdr->xrsp_t.sem, pb->pbx, "waiting for FCP" );

 /*
  *  if the timeout occurred, then remove the iocb_hdr from the
  *  queue and error out.   We do not retry because if the adapter
  *  lost the rsp, something is bad.    The Emulex is supposed to
  *  always return something.
  */
  
 if ( iocb_hdr->iocb_state != IOCB_FCP_DONE ) {
   /*
    * did not get the iocb returned
    */
    dprintf("kgpsa_fcp_cmd: timeout iotag:%x\n", 
                                                  p_args1(iocb_hdr->iotag) );
    remq_lock(&iocb_hdr->iocb_q.flink);
    kgpsa_unreg_login(pb,0xFFFF);   /* Overkill, but okay since something
                                     * is really wrong.
                                     */

    status = msg_failure;
    goto EXIT;
 }

 if ( irsp->ulpStatus == IOSTAT_LOCAL_REJECT ) {
   d2printf("pb:%x Local failure for did: %x  iotag: %x  word4: %x\n", 
                 p_args4(pb, iocb_hdr->did, iotag, irsp->un.fcpi.fcpi_parm) );
   switch( irsp->un.fcpi.fcpi_parm )
   {
     case IOERR_SEQUENCE_TIMEOUT:
       psrb->sts[0] = scsi_k_sts_check;
       status = fc_sts_seq_timeout;
       break;
     case IOERR_LOOP_OPEN_FAILURE:
       psrb->sts[0] = scsi_k_sts_check;
       status = fc_sts_loop_open;
       break;
     case IOERR_LINK_DOWN:
       status = fc_sts_link_down;
       break;
     default:
       psrb->sts[0] = scsi_k_sts_busy;
       status = fc_sts_retry;
   }
 }

 /*
  * Extract the scsi status and sense data.  Console only uses up to 
  * MAX_SENSE_SZ, so we throw away anything after that.
  */
 if (irsp->ulpStatus == IOSTAT_FCP_RSP_ERROR )
   {
     int RspLen = 0;
     FCP_RSP *fcpRsp;
     fcpRsp = rsp_payload;
     status = msg_success;                     /* assume we will report back
                                                  good status for now */

     if(fcpRsp->rspStatus2 & RSP_LEN_VALID) 
       {
       RspLen = SWAP_LONG(fcpRsp->rspRspLen);
       }
     psrb->sts[0] = fcpRsp->rspStatus3;
     if( psrb->sts[0] == scsi_k_sts_check )
       {
       if( fcpRsp->rspStatus2 & SNS_LEN_VALID ) 
         {
         memcpy( psrb->sense_dat_in, ((uchar *)&fcpRsp->rspInfo0) + RspLen, 
                                                                MAX_SENSE_SZ);
         *psrb->sense_dat_in_len = SWAP_LONG(fcpRsp->rspSnsLen);
         if( *psrb->sense_dat_in_len > MAX_SENSE_SZ ) 
           {
           dprintf("sense data %d longer than %d bytes (%x)\n", 
                 p_args3( *psrb->sense_dat_in_len, MAX_SENSE_SZ, &fcpRsp->rspInfo0 ) );
           *psrb->sense_dat_in_len = MAX_SENSE_SZ;
           }
         }
       } 

     if(fcpRsp->rspStatus2 & RSP_LEN_VALID) 
       {
        /*
         *  0, 4, or 8 info bytes will be returned(FCP) 
         * for now - only pay attention to info[3] w/ RSP_CODE
         */
        switch (fcpRsp->rspInfo3) {
        case RSP_DATA_BURST_ERR:
        case RSP_RO_MISMATCH_ERR:
        case RSP_TM_NOT_SUPPORTED:
        case RSP_TM_NOT_COMPLETED:
                break;
        case RSP_CMD_FIELD_ERR:
                /*
                 * I saw this errro when I was issuing FCP commands to UNIX
                 * with tag
                 */
                dprintf("CMD_FIELD_ERR\n", p_args0 );
		status = fc_sts_fcp_cmd_field_err;
                break;
        case RSP_NO_FAILURE:
                /* 
                 * Everything is ok - just ignore 
                 */
                break;
        }
       }

   }


 if ( irsp->ulpStatus == IOSTAT_SUCCESS )
   {
      status = msg_success;
   }


EXIT:
 krn$_stop_timer( &iocb_hdr->xrsp_t );
 krn$_semrelease( &iocb_hdr->xrsp_t.sem );
 krn$_release_timer( &iocb_hdr->xrsp_t );
 free( cmd_payload, "payload" );
 free( rsp_payload, "payload" );
 kgpsa_dealloc_iocbhdr( iocb_hdr );
 return status;
}





/*
 *########################################################################
 *#########                 IO Primatives                         ########
 *########################################################################
 */



/*+
 * ============================================================================
 * = kgpsa_read_csr - read a csr                                              =
 * ============================================================================
 *
-*/
unsigned int kgpsa_read_csr ( struct kgpsa_pb *pb, int offset )
{
 unsigned int retval;
 dprintf("pb->csr_mem_base:%x\n",p_args1(pb->csr_mem_base));
 dprintf("reading %s\n",p_args1(ffcsrtbl[offset/4]));
 if (pb->prefetch)
   retval = indmeml(pb, pb->csr_mem_base+offset);
 else
   retval = inmeml(pb, pb->csr_mem_base+offset);

 if (pb->slim_trace_enable) 
    kgpsa_slim_record( pb, READ_CSR, pb->csr_mem_base+offset, retval);
 return retval;
}


/*+
 * ============================================================================
 * = kgpsa_write_csr - write a csr                                            =
 * ============================================================================
 *
-*/
void kgpsa_write_csr( struct kgpsa_pb *pb, int offset, unsigned int data )
{
 dprintf("writing %s with %x\n",p_args2(ffcsrtbl[offset/4], data));
 if (pb->slim_trace_enable) 
    kgpsa_slim_record( pb, WRITE_CSR, pb->csr_mem_base+offset, data );
 if (pb->prefetch)
     outdmeml(pb, pb->csr_mem_base+offset, data);
 else
     outmeml(pb, pb->csr_mem_base+offset, data);
 
}



/*+
 * ============================================================================
 * = kgpsa_read_slim - read slim                                              =
 * ============================================================================
 *
-*/
unsigned int kgpsa_read_slim ( struct kgpsa_pb *pb, int offset )
{
 unsigned int retval;
 dprintf(" read_slim - offset: %x \n", p_args1(offset) );
 if (pb->prefetch)
    retval = indmeml(pb, pb->slim_base+offset);
 else
    retval = inmeml(pb, pb->slim_base+offset);
   
 if (pb->slim_trace_enable) 
    kgpsa_slim_record( pb, READ_SLIM, pb->slim_base+offset, retval);
 return retval;
}


/*+
 * ============================================================================
 * = kgpsa_write_slim - write a slim                                          =
 * ============================================================================
 *
-*/
void kgpsa_write_slim( struct kgpsa_pb *pb, int offset, unsigned int data )
{
 dprintf(" write_slim: %x <-- %x \n", p_args2(pb->slim_base+offset , data) );
 if (pb->slim_trace_enable) 
    kgpsa_slim_record( pb, WRITE_SLIM, pb->slim_base+offset, data );
 if (pb->prefetch)
    outdmeml(pb, pb->slim_base+offset, data);
 else
    outmeml(pb, pb->slim_base+offset, data);
}



/*
 *########################################################################
 *#########            IOCB & Ring Management                     ########
 *########################################################################
 */


/*+
 * ============================================================================
 * = kgpsa_read_ring - read an iocb ring                                      =
 * ============================================================================
 *
 * Read either the ELS or FCP response ring at the specified index into 
 * the ring.
 *
-*/
void kgpsa_read_ring( struct kgpsa_pb *pb, int type,
                                             int ringno, int idx, int *i_iocb)
{
 int i;
 int offset;
 FC_BRD_INFO *binfo;
 RING	*rp;
 int ringaddr;
 IOCB *iocb = i_iocb;
 int payload;

 binfo = &pb->binfo;


 rp = &binfo->fc_ring[ringno];
 if (type == IOCB_CMD) {
   ringaddr = rp->fc_cmdringaddr;
   pprintf("Reading the IOCB_CMD ring\n");
 } else if (type == IOCB_RSP) {
   ringaddr = rp->fc_rspringaddr;
 } else {
   dprintf("Bad iocb type\n", p_args0);
 }
 offset = ringaddr + ( sizeof(IOCB) * idx );
 for ( i=IOCB_WSIZE-1; i>=0; i-- ) {
   i_iocb[i] = rs( offset+(i*4) );   /* read slim */
 }


 /*
  *  This is just for tracing.  Later when we crash, this info will be
  *  useful.
  */

 for ( i=0; i<IOCB_WSIZE; i++ ) {
   pb->last_read_iocb[i] = i_iocb[i];   
 }

 
 /*
  * From here down is just code useful in tracing and debug.
  */

 if ( (type==IOCB_RSP) && (pb->iocb_trace_enable) && 
                                       ( (i_iocb[7]&1) == OWN_HOST) ) {
    kgpsa_record_iocb( pb, KREC_TYPE_READ, offset, i_iocb );


    /*
     * Check to see if the buffer has accidentally been freed.
     */
    if ( (i==0) || (i==2) ){
      if (i_iocb[i] != 0 ) {
      payload = *(int*)(i_iocb[i] - $MAP_FACTOR(pb));
        if ( ( (payload == 0xefefefef) || (payload == 0xa5a5a5a5) ) &&
           (iocb->ulpStatus == IOSTAT_SUCCESS)                    ) {
          if (  ( (i==0) && (iocb->ulpBdeCount>0)  )   ||
             ( (i==2) && (iocb->ulpBdeCount>=2) )   ){
	         pprintf("Payload already freed\n");
                 dump_els( pb );
                 do_bpt();
          }
        }
      }
    }

 }


}


/*+
 * ============================================================================
 * = kgpsa_write_ring - write an iocb ring                                    =
 * ============================================================================
 *
 * Write either the ELS or FCP command ring at the specified index into the
 * ring.
 *
 * Called by: kgpsa_send_iocb
 *
-*/
int kgpsa_write_ring( struct kgpsa_pb *pb, int type,
                                             int ringno, int idx, int *i_iocb)
{
 int i, status;
 int offset;
 FC_BRD_INFO *binfo;
 RING	*rp;
 int ringaddr;
 volatile unsigned int word7;
 volatile struct TIMERQ  tqe, *tq = &tqe;
 int sleep_time = kgpsa_write_ring_sleep_time;

 binfo = &pb->binfo;
 rp = &binfo->fc_ring[ringno];
 if (type == IOCB_CMD) {
   ringaddr = rp->fc_cmdringaddr;
 } else if (type == IOCB_RSP) {
   ringaddr = rp->fc_rspringaddr;
   pprintf("Writing the IOCB_RSP ring.\n");
 } else {
   dprintf("Bad iocb type\n", p_args0);
 }
 offset = ringaddr + ( sizeof(IOCB) * idx );

 /*
  *  First make our final check to see if we can send off an IOCB.
  *  There is no good way to clean up an iocb issued while the link
  *  is down and we will end up hanging.   This leaves a small window
  *  that the problem can still occur.   A request has been made to
  *  Emulex to address this problem (27-Mar-97).
  *  
  */

 status = msg_success;
 start_timeout(kgpsa_els_full_timer,tq);
 word7 = rs(offset+(7*4));
 /*
  * Wait for the next ring position to be freed by the adapter.  With
  * kgthrottle=10, we never see this.  So
  * I found it more meaningful to error out catastophically whenever a 
  * ring full condition occurs.  It usually means the adapter is wedged and
  * will never come back.
  *
  * Also, consider the FCP case.  Since only one FCP is issued at a time,
  * and there are 5 FCP cmd ring entries, the FCP cmd ring should never
  * see the full condition.
  */
 while ( (word7 & OWN_CHIP) && (tq->sem.count==0) ) {
   pprintf("RING %d FULL\n", ringno);
   pprintf("word7 (%x) = %x  (offset=%x) \n", pb->slim_base+offset, word7, 
                                                                      offset );
   if (ringno == FC_ELS_RING) {
     dump_els( pb );
   } else {
     pprintf("FCP ring full. How can this happen?\n");
     dump_fcp( pb );
   }
#if KGPSA_DEBUG
   do_bpt();
#endif
   krn$_sleep(sleep_time); 
   word7 = rs(offset+(7*4));
 }
 if (tq->sem.count) {
     status = msg_failure;
     pprintf("ELS stalled for too long \n",p_args0);
 }
 stop_timeout(tq);

 if ( status == msg_success ) {
   if ( (type==IOCB_CMD) && (pb->iocb_trace_enable)) {
     kgpsa_record_iocb( pb, KREC_TYPE_WRITE, offset, i_iocb );
   }
   for ( i=0; i<IOCB_WSIZE; i++ ) {
     ws( offset+(i*4), i_iocb[i] );  /* write slim */
   }
   status = msg_success;
 }

 return status;
}


/*+
 * ============================================================================
 * = kgpsa_ok_to_send - check validity of iocb before issueing                =
 * ============================================================================
 *
-*/
int kgpsa_ok_to_send( struct kgpsa_pb *pb, int ringno, IOCB_HDR *iocb_hdr )
{
IOCB *iocb;

iocb = &iocb_hdr->cmd_iocb;

 if ( iocb->ulpCommand == CMD_QUE_RING_BUF_CN ) {
   return TRUE;
 }

 if ( iocb->ulpCommand == CMD_ABORT_XRI_CN ) {
   return TRUE;
 }
 
 if ( pb->pb.mode == DDB$K_STOP ) {
   d2printf("kgpsa_ok_to_send - FAILED due to DDB$K_STOP \n", p_args0 );
   return FALSE;
 }

 if ( iocb->ulpCommand == CMD_XMIT_SEQUENCE_CR ) {
   return TRUE;
 }

 if( ringno == FC_FCP_RING ) 
  {
    if ( ( iocb_hdr->LINKUPeventTag < pb->LINKUPeventTag ) ||
         ( iocb_hdr->RSCNeventTag < pb->RSCNeventTag )     )
    {
     d2printf("kgpsa_ok_to_send - FAILED due to %x < %x || %x < %x\n", 
        p_args4( iocb_hdr->LINKUPeventTag, pb->LINKUPeventTag,
                iocb_hdr->RSCNeventTag, pb->RSCNeventTag) );
     return FALSE;
    }
  }

 return TRUE;
}

/*+
 * ============================================================================
 * = kgpsa_send_iocb - issue an iocb to the adapter                           =
 * ============================================================================
 *
-*/
int kgpsa_send_iocb( struct kgpsa_pb *pb, int ringno, IOCB_HDR *iocb_hdr)
{
 int i, status;
 int offset;
 FC_BRD_INFO *binfo;
 RING	*rp;
 IOCB *cmd;

 binfo = &pb->binfo;
 rp = &binfo->fc_ring[ringno];

 /* with queue locked */
 kgpsa_krn$_wait( &pb->ring_s, pb->pbx, "write_next" );

 /*
  * Put the iocb onto the queue prior to writing the adapter.  We want
  * to guarantee it is found there when the response interrupt comes in,
  * and the kgpsa_fc_process does a find_iocbhdr.
  */
#if KGPSA_DEBUG
{
 IOCB_HDR *iocb_hdr2;
 if( ringno == FC_ELS_RING ) 
 {
  iocb_hdr2 = kgpsa_find_iocbhdr( pb, IOCB_ACTIVE, FC_ELS_RING, 
                                                    iocb_hdr->iotag, 0 );
  if( iocb_hdr2 )
    {
      pprintf("kgpsa_send_iocb - duplicate iotag: %x \n", iocb_hdr->iotag ); 
      do_bpt();
    }
 }
}
#endif
 insq_lock(&iocb_hdr->iocb_q.flink, rp->active_q.blink);
 iocb_hdr->cmd_slim_idx = rp->fc_cmdidx;   /* debug informational only */

 /*
  * Trace code
  */
 if (ringno == FC_ELS_RING) {
   kgpsa_trace_iocb( "S ELS  W ", rp->fc_cmdringaddr + (sizeof(IOCB)*rp->fc_cmdidx), 
                        (int*)&iocb_hdr->cmd_iocb, (kgpsa_trace_iocb_doit&1) );
 } else {
   kgpsa_trace_iocb( "S FCP  W ", rp->fc_cmdringaddr + (sizeof(IOCB)*rp->fc_cmdidx), 
                       (int*)&iocb_hdr->cmd_iocb,  (kgpsa_trace_iocb_doit&2) );
 }

 /*
  * This is our last chance to make sure nothing has changed since the
  * iocb was formed.  There is a very small window and low probability
  * case (if not a zero probability case) that the destination rpi could
  * have change by the time we get here.   
  */
 if ( !kgpsa_ok_to_send( pb, ringno, iocb_hdr ) ) {
   status = msg_failure;
   goto exit;
 }
 /*
  * Issue the iocb by writing the adapters command ring.
  */
 status = kgpsa_write_ring( pb, IOCB_CMD, ringno, rp->fc_cmdidx,
                                                    (int)&iocb_hdr->cmd_iocb);
 if ( status==msg_failure ){
    dprintf("kgpsa_send: failure.  ring:%d  iotag:%x\n", 
                                         p_args2( ringno, iocb_hdr->iotag ) );
    goto exit;
 }
 /*
  * Bump the ring index to the next iocb.
  */
 rp->fc_cmdidx++;
 if (rp->fc_cmdidx >= rp->fc_numCiocb) {
   rp->fc_cmdidx  = 0;
 }


 /*
  *  FCP reads and writes use a continuation IOCB.  Issue the continuation
  *  iocb if necessary.
  */

 if(!iocb_hdr->cmd_iocb.ulpLe) {
    iocb_hdr->cont_slim_idx = rp->fc_cmdidx;  /* debug informational only */
    kgpsa_trace_iocb( "CONT W ", rp->fc_cmdringaddr + (sizeof(IOCB)*rp->fc_cmdidx), 
                             (int*)&iocb_hdr->cont_iocb,  (kgpsa_trace_iocb_doit&2)  );
    status = kgpsa_write_ring( pb, IOCB_CMD, ringno, rp->fc_cmdidx,
                                                  (int)&iocb_hdr->cont_iocb);
    if ( status==msg_failure ) {
       dprintf("kgpsa_send: Writing continuation IOCB failed\n", p_args0 );
       goto exit;
    }
    /*
     * Bump the ring index to the next iocb.
     */
    rp->fc_cmdidx++;
    if (rp->fc_cmdidx >= rp->fc_numCiocb) {
      rp->fc_cmdidx  = 0;
    }
 }

 /*
  * Handle Immediate post-processing for commands that do not
  * generate responses.
  */
 switch (iocb_hdr->cmd_iocb.ulpCommand) {
   case CMD_QUE_RING_BUF_CN :
        kgpsa_que_ring_buf_done( iocb_hdr );
        break;
   default:
        break;
 }


exit:

 kgpsa_krn$_post( &pb->ring_s, pb->pbx, "write_next" );
 if ( status != msg_success ) 
    dprintf("kgpsa_send_iocb - failed to send \n", p_args0 );
 return status;
}


/*+
 * ============================================================================
 * = kgpsa_receive - grab next available iocb from receive ring               =
 * ============================================================================
 *
-*/
int kgpsa_receive( struct kgpsa_pb *pb, int ringno, int *i_iocb, int *doffset )
{
 int i;
 int offset;
 int  value;
 FC_BRD_INFO *binfo;
 RING	*rp;
 IOCB *rsp;
 int status;

 binfo = &pb->binfo;
 rp = &binfo->fc_ring[ringno];
 rsp = i_iocb;

 kgpsa_krn$_wait( &pb->ring_s, pb->pbx, "kgpsa_receive" );
 kgpsa_read_ring( pb, IOCB_RSP, ringno, rp->fc_rspidx, rsp );
 if (rsp->ulpOwner == OWN_CHIP) {
   kgpsa_krn$_post( &pb->ring_s, pb->pbx, "kgpsa_receive" );
   return msg_failure;
 } else {
   rsp->ulpOwner = OWN_CHIP;  /* TODO:???  (7-nov-98 why?  I don't see any good reason for this) */    
   /* 
    * Give the IOCB back to the adapter 
    */
   *doffset = rp->fc_rspringaddr + (sizeof(IOCB) * rp->fc_rspidx);
   offset = rp->fc_rspringaddr + (sizeof(IOCB) * rp->fc_rspidx) + IOCB_OWNER_OFFSET;
   value = rs( offset );
   ws( offset, value|OWN_CHIP );
   /*
    * Bump the ring index to the next iocb.
    */
   rp->fc_rspidx++;
   if (rp->fc_rspidx >= rp->fc_numRiocb) {
     rp->fc_rspidx  = 0;
   }
 }
 kgpsa_krn$_post( &pb->ring_s, pb->pbx, "read_next" );
 return msg_success;
}



/**********************************************************************
 * Only used in debug
 */
#if KGPSA_DEBUG
void kgpsa_find_iocb_tag( struct kgpsa_pb *pb, int iotag)
{
#define WORD6_OFFSET 6*4
 int i;
 unsigned int piocb;
 unsigned int word6;
 int t;


 for (i=0; i <= 119; i++) {
   piocb = ( 256 + (i*32) );
   word6 = rs( piocb+WORD6_OFFSET );
   t = word6 & 0xFFFF;
   if (t == iotag) {
     kgpsa_dump_slim(pb, piocb, 8);
   }
 }
 return;
}
#endif



/*+
 * ============================================================================
 * = probe_state -                                                            =
 * ============================================================================
 *
-*/
/*
 * The probe state routine is a loop-only implementation right now
 */
int probe_state( struct kgpsa_pb *pb, int func, int set_value, int did )
{
/*
 * regarding alpaidx -There is (maybe) potential to feed alpaidx a non-existant
 * did (say the scsi_targets array is changed or a did comes in that is
 * not a scsi_target or somehow not in that list).
 */

  int i;
  int aidx;
  int first,last;
  int retval;
  if (did == ALL_DID) {
    first = 0;
    last  = pb->scsi_targets_cnt-1;
  } else {
    aidx = alpaidx( pb, did );
    if( aidx == -1 )
       return PROBE_NON_EXISTANT_DID;
    first = aidx;
    last  = aidx;
  }



  switch( func ) {
    case PROBE_SET:
      if( first == last ) 
         dprintf("old: %s ", p_args1(nprobe_state_text[pb->nprobe_state[first]]) ); 

      dprintf("SETTING probe_state: %s alpa:%x to %x\n", 
         p_args3(nprobe_state_text[set_value], pb->scsi_targets[first], pb->scsi_targets[last]) );

      for( i=first; i<=last; i++ ) {
        kgpsa_record_probe( pb, pb->scsi_targets[i], 
                                            pb->nprobe_state[i], set_value );
        pb->nprobe_state[i] = set_value;
      }
      break;

   case PROBE_GET:
      retval = PROBE_DONE;
      for( i=first; i<=last; i++ ) {
        if ( pb->nprobe_state[i] == PROBE_IN_PROGRESS ) { 
	  dprintf("STILL NOT PROBED: i: %d  alpa:%x\n", 
                             p_args2(i, pb->scsi_targets[i]) );
	  retval = PROBE_IN_PROGRESS;
	  break;
	}
      }
      break;
#if KGPSA_DEBUG 
    case PROBE_DISPLAY_BOTH:
      for( i=first; i<=last; i++ ) {
	printf("idx:%3d  did: %3x ", i, pb->scsi_targets[i] );
        switch(pb->nprobe_state[i]) {
	  case PROBE_IN_PROGRESS:
	    printf(" PROBE_IN_PROGRESS\n");
	    break;
          case PROBE_DONE:
	    printf(" PROBE_DONE\n");
	    break;
	  default:
	    printf(" INVALID PROBE STATE\n");
	    break;
	} /* switch */
      } /* for */
      break;

    case PROBE_DISPLAY_DONE:
      printf("PROBE_DONE: ");
      for( i=first; i<=last; i++ ) {
	if(pb->nprobe_state[i] == PROBE_DONE)
	  printf("%x ", pb->scsi_targets[i] );
      } /* for */
      break;

    case PROBE_DISPLAY_INPROG:
      printf("PROBE_IN_PROGRESS: ");
      for( i=first; i<=last; i++ ) {
	if(pb->nprobe_state[i] == PROBE_IN_PROGRESS)
  	  printf("%x ", pb->scsi_targets[i] );
      } /* for */
      break;
      
#endif

  } /* switch */


  return retval;
}

/*+
 * ============================================================================
 * = probe_next -                                                             =
 * ============================================================================
 *
-*/
void probe_next( struct kgpsa_pb *pb )
{                               
   int next_did;                                             
   pb->discovery_index++;                                                 
   if( pb->discovery_index < pb->scsi_targets_cnt )        
      {                                                         
        next_did =  pb->scsi_targets[pb->discovery_index];
        kgpsa_els_cmd( pb, next_did , ELS_CMD_PLOGI, DONT_WAIT, fc_els_retries );
      }                                                         
}                




#if KGPSA_DEBUG
void set_probing( int pbx, int state )
{
  probe_state( kgpsa_pbs[pbx], PROBE_SET, state, 0 );

  if (state==PROBE_IN_PROGRESS) {
     printf("probe state set to PROBE_IN_PROGRESS (%x)\n", state );
  } else if (state==PROBE_DONE) {
     printf("probe state set to PROBE_DONE (%x)\n", state );
  } else {
     printf("probe state - unknown state set (%x)\n", state );
  }

}
#endif





#if KGPSA_DEBUG
int kgpsa_big_read()
{
 int i, status = msg_success;
 struct kgpsa_pb *pb;
 struct kgpsa_sb *sb;
 int *cmd_payload;
 int *rsp_payload;
 int ulpLe;
 int iotag;
 unsigned char *scsi_cmd;
 unsigned char scsi_hack[12] = {0x28,0x00,0x00,0x00,
                                0x00,0x00,0x00,0x00,
                                0x00,0x00,0x00,0x00};
 unsigned short *scsi_transfer_len;
 IOCB *icmd, *icont, *irsp;
 IOCB_HDR *iocb_hdr;
 struct PCB *pcb;
 int rpi;

 pb = kgpsa_pbs[0];
 sb = kgpsa_find_did( pb, kgpsa_seagate_alpa );

 iotag = 0xfade;
 iocb_hdr = kgpsa_alloc_iocbhdr( pb, iotag, "BIGREAD" );
 iocb_hdr->sb = sb;
 iocb_hdr->did = kgpsa_seagate_alpa;
 icmd = &iocb_hdr->cmd_iocb;
 icont = &iocb_hdr->cont_iocb;
 irsp = &iocb_hdr->rsp_iocb;
 cmd_payload = malloc( FCP_CMD_PAYLOAD_SIZE, "payload" );
 rsp_payload = malloc( PAYLOAD_SIZE, "payload" );

 scsi_cmd    = &cmd_payload[3];

 cmd_payload[0]    = 0;                          /* lun low  */
 cmd_payload[1]    = 0;                          /* lun high */
 cmd_payload[2]    = 0x02000000;
 cmd_payload[7]    = SWAP_LONG(big_read_xfer_len);

 for (i=0; i < 12; i++) {
   scsi_cmd[i] = scsi_hack[i];
 }

 scsi_transfer_len = &scsi_cmd[7];
 *scsi_transfer_len = SWAP_SHORT(big_read_xfer_len/512);

 icmd->un.cont[0].bdeAddress = (int)cmd_payload | $MAP_FACTOR( pb );   /* fcp cmnd payload */
 icmd->un.cont[0].bdeSize    = FCP_CMD_PAYLOAD_SIZE;
 icmd->un.cont[1].bdeAddress = (int)rsp_payload | $MAP_FACTOR( pb );
 icmd->un.cont[1].bdeSize    = PAYLOAD_SIZE;
 icmd->un.ulpWord[4]         = 0;
 icmd->un.ulpWord[5]         = 0;
 icmd->un.ulpWord[6]         = (sb->rpi << 16) | iotag; /* rpi */
 icmd->ulpOwner              = OWN_CHIP;
 icmd->ulpLe                 = 0;
 icmd->ulpBdeCount           = 2;
 icmd->ulpCommand            = CMD_FCP_IREAD_CR;
 icmd->ulpClass              = CLASS3;
 icmd->ulpRsvdByte           = kgpsa_fcp_timeout;  /* timeout in seconds */

 icont->un.cont[0].bdeAddress = (int)0xc00000 | $MAP_FACTOR( pb );
 icont->un.cont[0].bdeSize    = big_read_xfer_len/3;
 icont->un.cont[1].bdeAddress = (int)0xd00000 | $MAP_FACTOR( pb );
 icont->un.cont[1].bdeSize    = big_read_xfer_len/3;
 icont->un.cont[2].bdeAddress = (int)0xe00000 | $MAP_FACTOR( pb );
 icont->un.cont[2].bdeSize    = (big_read_xfer_len/3)+3;
 icont->un.ulpWord[6]         = iotag;
 icont->ulpOwner              = OWN_CHIP;
 icont->ulpLe                 = 1;
 icont->ulpBdeCount           = 3;
 icont->ulpCommand            = CMD_IOCB_CONTINUE_CN;
 icont->ulpClass              = CLASS3;

 status = kgpsa_send_iocb( pb, FC_FCP_RING, iocb_hdr );
   if ( status != msg_success ) {
     /*
      * did not get the iocb issued
      */
      pprintf("kgpsa_big_read: iocb not issued\n");
      remq_lock(&iocb_hdr->iocb_q.flink);
      goto EXIT;
   }

 kgpsa_krn$_wait( &iocb_hdr->rsp, pb->pbx, "waiting for FCP" );


EXIT:
 free( cmd_payload, "payload" );
 free( rsp_payload, "payload" );
 kgpsa_dealloc_iocbhdr( iocb_hdr );

 return status;

}
#endif





#if 0
void kgpsa_abort_rpi( struct kgpsa_pb *pb, int ringno, IOCB *rsp )
{
  struct QUEUE active_q;
  struct QUEUE *qp;
  IOCB_HDR *t;
  FC_BRD_INFO *binfo;
  RING *rp;
  int al_pa;
  int word5;
  struct kgpsa_sb *sb;

  al_pa = rsp->un.ulpWord[5];
  binfo = &pb->binfo;
  rp = &binfo->fc_ring[ringno];
  qp = rp->active_q.flink;
  while (qp != &rp->active_q.flink) {
    t = (int)qp - offsetof(IOCB_HDR, iocb_q);
    if ( al_pa == t->did ) {
       word5 = (rsp->un.ulpWord[6] << 16) | t->iotag;
       sb = kgpsa_find_did( pb, al_pa );
       if (sb == NULL) {
         printf("sb not found - panic\n");
       }
       kgpsa_abort_xri_cn( sb, ringno, word5 );
    }
    qp = qp->flink;
  }

}
#endif

/*
 * This routine is used in conjunction with unreg_login.  When an
 * unreg_login occurs, the active exchanges are killed off, and
 * iocbs already in the adapter may be delivered correctly after
 * the exchange is killed off.  There is no problem with unsolicited
 * ELS iocbs that arrive.  These require an acknowledgement and 
 * the ACC would fail.  Since we do not change any state until we
 * know we have sent out the ACC, this failure will prevent us from
 * changing state on due to something stale in the receive queue.
 * 
 * But if we get a failure because of a stale and invalid response,
 * then we would have a problem as state would change.  
 *
 * Suppose for instance, there is a PLOGI ACC in the slim waiting
 * for us to read it out.  Then a link init occurs and the ALPA
 * on that remote port changes, or the myDID changes.  If we act
 * upon the PLOGI ACC, node state changes would falsely happen.
 *
 * (Another way to do this might be to keep a link_event incarnation
 *  with each IOCB_HDR and throw away those that are not current).
 *
 */
void kgpsa_terminate_outstanding_ELS( struct kgpsa_pb *pb, int rpi )
{
#if 0
  struct QUEUE active_q;
  struct QUEUE *qp;
  IOCB_HDR *t;
  FC_BRD_INFO *binfo;
  RING *rp;
  int al_pa;
  int word5;
  struct kgpsa_sb *sb;

  al_pa = rsp->un.ulpWord[5];
  binfo = &pb->binfo;
  rp = &binfo->fc_ring[ringno];
  qp = rp->active_q.flink;
  while (qp != &rp->active_q.flink) {
    t = (int)qp - offsetof(IOCB_HDR, iocb_q);
    if ( al_pa == t->did ) {
       word5 = (rsp->un.ulpWord[6] << 16) | t->iotag;
       sb = kgpsa_find_did( pb, al_pa );
       if (sb == NULL) {
         printf("sb not found - panic\n");
       }
       kgpsa_abort_xri_cn( sb, ringno, word5 );
    }
    qp = qp->flink;
  }
#endif
}






/*
 * These are some CLI helpers to help in the qual. 
 */
#if KGPSA_DEBUG

volatile int kgpsa_new_id = 12;


void kgpsa_stress_ring( int pbx, int did )
{
  int i;
  struct kgpsa_pb *pb = kgpsa_pbs[pbx];
  struct kgpsa_sb *sb;
  for (i=0; i<kgpsa_stress_cnt; i++) {
    kgpsa_els_cmd( pb, did, ELS_CMD_PDISC, kgpsa_wait, DONT_RETRY );
  }
}

void kgpsa_stress_latt( int pbx, int cnt, int interval )
{
  int i;
  for (i=0;i<cnt;i++) {
    kgpsa_init_link( kgpsa_pbs[pbx], 0x900000, DONT_WAIT );
    krn$_sleep( interval );
  }
}

void kgpsa_stress_elscmd( int pbx, int elscmd, int did, int cnt, int interval )
{
  int i;
  for (i=0;i<cnt;i++) {
    kgpsa_els_cmd( kgpsa_pbs[pbx], did, elscmd, DONT_WAIT, DONT_RETRY );
    krn$_sleep( interval );
  }
}

void kgpsa_stress_flurry( int pbx, int elscmd, int did, int cnt, int flcnt, int interval )
{
  int i,j;
  for (i=0;i<cnt;i++) {
    for (j=0; j<flcnt; j++) {
      kgpsa_els_cmd( kgpsa_pbs[pbx], did, elscmd, DONT_WAIT, DONT_RETRY );
    }
    krn$_sleep( interval );
  }
}

void kgpsa_stress_servparm( struct kgpsa_pb *pb, char id )
{
  struct kgpsa_sb *sb;
  char portname[20];
  FC_BRD_INFO *binfo;

  binfo = &pb->binfo;
  wwn_sprintf( &portname, &binfo->fc_sparam.portName );
  pprintf("kgpsa_stress_servparm from %s", portname );
  binfo->fc_sparam.portName.IEEE[0] = id;
  wwn_sprintf( &portname, &binfo->fc_sparam.portName );
  pprintf(" to %s \n",  portname );
  sb = pb->pb.sb[0];
  strcpy( sb->portname, portname );
  kgpsa_new_id = id;     /* back fill for next time driver starts */
}

/*  
 * On every lip, modify the serv parameters we get from kgpsa_seagate_alpa.
 * This will make the drive keep switching controllers.
 *
 */
void kgpsa_stress_seagate_servparm( int alpa, SERV_PARM *sp )
{
  char portname[20];
  if( alpa != kgpsa_seagate_alpa) return;
  wwn_sprintf( &portname, &sp->portName );
  pprintf("kgpsa_stress_seagate_servparm from %s", portname );
  sp->portName.IEEE[0] = kgpsa_new_seagate_id;
  wwn_sprintf( &portname, &sp->portName );
  pprintf(" to %s \n",  portname );
}


void kgpsa_stress_downtime( struct kgpsa_pb *pb, int seconds )
{
  MAILBOX mbx;
  ws( 0, 0x601 );  /* Down Link */
  krn$_sleep( seconds * 1000 );
  kgpsa_init_link_reset( pb, &mbx, 0 );
}


#endif



#if LEAK_WATCH
void kgpsa_leak_init()
{
  krn$_seminit( &kgpsa_leak_lock, 1, "leak_lock" );
  kgpsa_leak_q.flink = &kgpsa_leak_q;
  kgpsa_leak_q.blink = &kgpsa_leak_q;
}
#endif


int kg_scsi_send( struct ub *ub, unsigned char *cmd, int cmd_len,
	unsigned char *dat_out, int *dat_out_len,
	unsigned char *dat_in, int *dat_in_len )
{
  int t = msg_failure;
  if(ub)
    {
     t = scsi_send( ub, cmd, cmd_len, 0, 0, dat_in, dat_in_len );
    }
  return t;
}

int kg_get_udid( struct ub *ub, int *udid )
{
  int t = msg_failure;
  if(ub)
    {
     t = scsi3_get_udid( ub, udid );
    }
  return t;
}

void kgpsa_fabric_login( struct kgpsa_pb *pb )
{
#if FABRIC_SUPPORT
      /*
       * FABRIC STATE CHANGE THREAD STARTING WITH LINK UP
       * --> You are here
       *
       * --> 1. Link up                               
       * --> 2. suspend FCP outgoing queue
       * --> 3. Log into Fabric
       *    3a. Register with Fabric Controller for state change notification
       *     4. log into name server
       *     5. Receive login response
       *     6. give the ring a buffer
       *        some problems here I think.  Do we want to queue to an XRI
       *        instead?  What happens to the XRI queued buffer when the 
       *        exchange goes away.  - A RET_XRI_QUE_BUF returns the unused
       *        buffer.
       *        Do we just want to queue this to the FCP ring and have it there
       *        always and replenish it upon every receipt?
       *     7. Request the latest list of nodes with GP_ID3
       *     8. Receive the SCR completed response
       *     9. Receive asynchronously the data from the SCR
       *    10. Logout from the name server
       *    11. Probe the node list
       */      

      dprintf("* --> 1. Link up                               \n", p_args0);
      dprintf("* --> 2. suspend FCP outgoing queue\n", p_args0);
      dprintf("* --> 3. Log into Fabric\n", p_args0);
      kgpsa_els_cmd( pb, FABRIC_F_PORT, ELS_CMD_FLOGI, DONT_WAIT, fc_els_retries );


#endif
}

#if 0
void kgevent( struct kgpsa_pb *pb )
{
  int prbstate;
  MAILBOX mbx, *mb=&mbx;
  kgpsa_mbx( pb, mb, MBX_READ_LA, 0);
  pprintf("READ_LA: %x   pb->DiscLINKUPeventTag: %x  pb->LINKUPeventTag: %x\n", 
      mb->un.varReadLA.eventTag, pb->DiscLINKUPeventTag, pb->LINKUPeventTag );
  pprintf("pb->DiscRSCNeventTag: %x  pb->RSCNeventTag: %x \n",
                                     pb->DiscRSCNeventTag, pb->RSCNeventTag );

  pprintf("pb->DiscState: %s\n",Discstate_text[pb->DiscState]);
  pprintf("pb->DevState: %s \n",devnstate_text[pb->devState] );
  prbstate = probe_state(pb, PROBE_GET, 0, ALL_DID);
  dprintf("probe state : %s \n", 
                   p_args1( nprobe_state_text[pb->nprobe_state[prbstate]]) );
}
#endif

#if 0
void kgeasyfind()
{

}
#endif



#if XRI_SUPPORT
/*
 * At this time (7-Nov-98), there is no requirement to support the xri
 * commands.  The xri code is in good shape should it ever be needed.  
 * The xri mechanisms came close to being used for the Name Server request.
 * It was the only way to attach a buffer to an exchange.   But instead,
 * I wait for a rcv_notification after an xmit_sequence_cr.
 *
 */

int create_xri_cr( struct kgpsa_pb *pb, int rpi )
  {
 int status = msg_success;
 int nstate;
 struct kgpsa_sb *sb;
 IOCB *icmd, *irsp;
 MAILBOX mbx;
 IOCB_HDR *iocb_hdr;
 FC_BRD_INFO *binfo;
 SERV_PARM   *sp;
 int iotag;
 int al_pa;
 ELS_PKT *els;
 int elscode;

 int cmdsize;

 pb->elscmdcnt++;
 iotag = pb->elscmdcnt;

 iocb_hdr = kgpsa_alloc_iocbhdr( pb, iotag, "CREATE_XRI_CR" );
 iocb_hdr->wait_for_completion = DONT_WAIT;
 iocb_hdr->pb = pb;
 iocb_hdr->sb = kgpsa_find_rpi( pb, rpi );  /* may not be valid at this point*/
 iocb_hdr->did = sb->did;

 binfo = &pb->binfo;
 icmd = &iocb_hdr->cmd_iocb;
 irsp = &iocb_hdr->rsp_iocb;


 icmd->un.ulpWord[6]         = (rpi<<16)|iotag;

   /*
    * Save on code generated by just using a longword constant.
    *
*/
     icmd->ulpOwner              = OWN_CHIP;
     icmd->ulpLe                 = 1;
     icmd->ulpBdeCount           = 0;
     icmd->ulpCommand            = CMD_CREATE_XRI_CR;
     icmd->ulpClass              = CLASS3;
/*
    */
#if 0
 icmd->un.ulpWord[7]         = 0x20a0b;
#endif

 status = kgpsa_send_iocb( pb, FC_ELS_RING, iocb_hdr );

 if ( status != msg_success ) {
   /*
    * did not get the iocb issued
    */
    dprintf("kgpsa_els_cmd: iocb not issued\n", p_args0);
    remq_lock(&iocb_hdr->iocb_q.flink);
    kgpsa_dealloc_iocbhdr( iocb_hdr );
    goto EXIT;
 }
 

EXIT:
 return status;
  
  }



int que_xri_buf_cx( struct kgpsa_pb *pb, int xri, int buffer, int bufsiz )
{

   int status;
   IOCB *piocb;
   IOCB_HDR *iocb_hdr;
   int iotag;


   iotag = 0xabcd;
   iocb_hdr = kgpsa_alloc_iocbhdr( pb, iotag, "QUEBUF" );
   piocb = &iocb_hdr->cmd_iocb;

   piocb->un.cont[0].bdeAddress = (int)buffer | $MAP_FACTOR( pb );
   piocb->un.cont[0].bdeSize    = bufsiz;
   piocb->un.ulpWord[6]         = (xri<<16)|iotag;     /* XRI,,IO_TAG */

   /*
    * Save on code generated by just using a longword constant.
    *
    * piocb->ulpOwner              = OWN_CHIP;
    * piocb->ulpLe                 = 1;
    * piocb->ulpBdeCount           = 1;
    * piocb->ulpCommand            = CMD_QUE_XRI_BUF_CX;
    * piocb->ulpClass              = CLASS3;
    */
   piocb->un.ulpWord[7]         = 0x20707;

   status = kgpsa_send_iocb( pb, FC_FCP_RING, iocb_hdr );
   return status;

}


int kgpsa_xmit_seq_cx( struct kgpsa_pb *pb, int rpi, int xri, int xmittype, int wait_for_completion )
{
 int status = msg_success;
 int *cmd_payload;
 int *rsp_payload;
 int nstate;
 struct kgpsa_sb *sb;
 IOCB *icmd, *irsp;
 MAILBOX mbx;
 IOCB_HDR *iocb_hdr;
 FC_BRD_INFO *binfo;
 SERV_PARM   *sp;
 int iotag;
 int al_pa;
 ELS_PKT *els;
 int elscode;



 pb->elscmdcnt++;

 iotag = pb->elscmdcnt;

 iocb_hdr = kgpsa_alloc_iocbhdr( pb, iotag, "XMIT" );
 iocb_hdr->rpi                 = rpi;
 iocb_hdr->wait_for_completion = wait_for_completion;
 iocb_hdr->pb                  = pb;
 iocb_hdr->sb = kgpsa_find_did( pb, rpi );
 sb = iocb_hdr->sb;
 iocb_hdr->did = sb->did;

 binfo = &pb->binfo;
 icmd        = &iocb_hdr->cmd_iocb;
 irsp        = &iocb_hdr->rsp_iocb;
 cmd_payload = malloc( ELS_CMD_PAYLOAD_SIZE, "payload" );
 els         = cmd_payload;
 iocb_hdr->bde1_bufptr = cmd_payload;
 rsp_payload = malloc( PAYLOAD_SIZE, "payload" );
 iocb_hdr->bde2_bufptr = rsp_payload;


 switch(xmittype) {

    /* 
     * GPN Get port name given its address identifier
     */
    case GPN:    
       cmd_payload[0]     = SWAP_LONG(0x01000000);
       cmd_payload[1]     = SWAP_LONG(0xFC020000);
       cmd_payload[2]     = SWAP_LONG(0x01120000);
       cmd_payload[4]     = SWAP_LONG(0x00200413);
      break;

    /* 
     * GCoS Get Class of Service
     */
    case GCoS:    
       cmd_payload[0]     = SWAP_LONG(0x01000000);
       cmd_payload[1]     = SWAP_LONG(0xFC020000);
       cmd_payload[2]     = SWAP_LONG(0x01140000);
       cmd_payload[4]     = SWAP_LONG(0x00200413);
      break;


    /* 
     * GPT Get port type
     */
    case GPT:    
       cmd_payload[0]     = SWAP_LONG(0x01000000);
       cmd_payload[1]     = SWAP_LONG(0xFC020000);
       cmd_payload[2]     = SWAP_LONG(0x011A0000);
       cmd_payload[4]     = SWAP_LONG(0x00200413);
      break;


    /* 
     * GP_ID2 Get address identifier given a NodeName
     */
    case GP_ID2:
       cmd_payload[0]     = SWAP_LONG(0x01000000);
       cmd_payload[1]     = SWAP_LONG(0xFC020000);
       cmd_payload[2]     = SWAP_LONG(0x01310000);

/*TODO:*/       cmd_payload[4]     = SWAP_LONG(0x00000008);
      break;

    /* 
     * GP_ID3 Get address identifier given and FC-4 type value
     */
    case GP_ID3:    
       cmd_payload[0]     = SWAP_LONG(0x01000000);
       cmd_payload[1]     = SWAP_LONG(0xFC020000);
       cmd_payload[2]     = SWAP_LONG(0x01710000);
       cmd_payload[4]     = SWAP_LONG(0x00000008);
      break;

    default:
      dprintf("XMIT command code %x not supported\n", p_args1(xmittype) );
      break;
 }


 icmd->un.cont[0].bdeAddress = (int)cmd_payload | $MAP_FACTOR( pb );
 icmd->un.cont[0].bdeSize    = 5*4;
 icmd->un.cont[1].bdeAddress = (int)rsp_payload | $MAP_FACTOR( pb );
 icmd->un.cont[1].bdeSize    = PAYLOAD_SIZE;
 icmd->un.ulpWord[5]         = 0x0220000C;  /* r_ctl  = 0x02
                                               type   = 0x20
                                               df_ctl = 0x00
                                               f_ctl  = 0x0C comes from  
                                                        0x04 (SI) from 0x290000
                                                        OR'ed with 0x8 to 
                                                        allow before CLEAR_LA
					     */
 icmd->un.ulpWord[6]         = (xri<<16) | iotag;
   /*
    * Save on code generated by just using a longword constant.
    *
    * icmd->ulpOwner              = OWN_CHIP; 
    * icmd->ulpLe                 = 1;
    * icmd->ulpBdeCount           = 2;
    * icmd->ulpCommand            = CMD_XMIT_SEQUENCE_CX;
    * icmd->ulpClass              = CLASS3;
    */
 icmd->un.ulpWord[7]         = 0x20307;

 status = kgpsa_send_iocb( pb, FC_FCP_RING, iocb_hdr );  /* issue to ring 2 */

 if ( status != msg_success ) {
   /*
    * did not get the iocb issued
    */
    dprintf("kgpsa_els_cmd: iocb not issued\n", p_args0 );
    remq_lock(&iocb_hdr->iocb_q.flink);
    free( cmd_payload, "payload" );
    free( rsp_payload, "payload" );
    kgpsa_dealloc_iocbhdr( iocb_hdr );
    goto EXIT;
 }
 
 if (wait_for_completion) {
   kgpsa_krn$_post_wait( &pb->els_owner_s, &iocb_hdr->rsp , pb->pbx, "waiting for rsp" );
   kgpsa_krn$_wait( &pb->els_owner_s, pb->pbx, "waiting on ELS owner" );
   kgpsa_els_cmd_done( pb, iocb_hdr );
 }

EXIT:
 return status;
}

#endif /* XRI_SUPPORT */


extern struct EVNODE evlist;  /* header of the list of variables */
extern int ev_initing;
int ev_wr_fc( );
int ev_rd_fc( );
int ev_fc_init( );
extern struct registered_wwid **wwids;
extern unsigned int num_wwids;
extern unsigned int wwid_ev_mask;
extern struct registered_portname **portnames;
extern unsigned int portname_ev_mask;

/*
 * Currently only 4 wwid's and 4 N ev's are created.  The number of these
 * must agree with the definitions in WWID.H :
 *
 *          #define MAX_WWID_ALIAS_EV          4
 *          #define MAX_PORTNAME_ALIAS_EV      4
 *
 */

struct env_table fc_wwidx_ev = {
"wwidx","",
        EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_wr_fc, ev_rd_fc, ev_fc_init

};

struct env_table fc_Nx_ev = {
"Nx","",
        EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
	0, ev_wr_fc, ev_rd_fc, ev_fc_init
};


int ev_wr_fc( char *name, struct EVNODE *ev )
    {
    int i;
    char *srm_wwid;
    int portname_len;

    if( ev_initing )
	return( msg_success );

    if(ev->name[0] == 'w') {
       i = atoi(&name[4]);
       srm_wwid = strstr( ev->value.string, "WWID:" );
       if (srm_wwid) {
          wwid_ev_mask |= 1 << i;
          if( ( ev->attributes & EV$M_VOLATILE ) == EV$K_NONVOLATILE )
  	    return( ev_sev( name, ev ) );
          return( msg_success );
       } else {
          wwid_ev_mask = wwid_ev_mask & ~(1 << wwids[i]->evnum );
          ev->value.string[0] = '\0';
          if( ( ev->attributes & EV$M_VOLATILE ) == EV$K_NONVOLATILE )
     	     return( ev_sev( name, ev ) );
       }
    } else {
       /* must be a N ev */
       /*pprintf("ev->name  %s\n", ev->name);*/

       i = atoi(&name[1]);
       if (strlen(ev->value.string)==16) {
          portname_ev_mask |= 1 << i;
          if( ( ev->attributes & EV$M_VOLATILE ) == EV$K_NONVOLATILE )
  	    return( ev_sev( name, ev ) );
          return( msg_success );
       } else {
          portname_ev_mask = portname_ev_mask & ~(1 << portnames[i]->evnum );
          ev->value.string[0] = '\0';
          if( ( ev->attributes & EV$M_VOLATILE ) == EV$K_NONVOLATILE )
     	     return( ev_sev( name, ev ) );
       }

    }

   }

int ev_rd_fc( char *name, struct EVNODE *ev, struct EVNODE *ev_copy )
    {
    int i;
    if( ev_initing )
	return( msg_success );
    return( msg_success );
    }             


int ev_fc_init( struct env_table *et, char *value )
{
 int i,t;           
 char *pWWID;
 struct registered_wwid *w;   
 struct registered_portname *p;
 extern int num_portnames;

 *value = 0;
 ev_sev_init( et, value );
 if(*value==0)
    goto EXIT;

 ev_fc_init_sub( et->ev_name, value );

EXIT:
 return( value );
} 


int ev_fc_init_sub( char *name, char *value )
{
 int i,t;           
 char *pWWID;
 struct registered_wwid *w;   
 struct registered_portname *p;
 extern int num_portnames;

 if(*value==0)
    goto EXIT;

 if(name[0] == 'w') 
   {
    i = atoi(&name[4]);
    w = dyn$_malloc( sizeof(struct registered_wwid), DYN$K_SYNC | DYN$K_NOOWN );
    pWWID = wwev2wwid( value, &w->unit, &w->collision, &w->wwid );
    if (pWWID) {
       wwid_ev_mask |= 1 << i;
       w->evnum = i; 
       w->udid = -1; 
       wwids[num_wwids++] = w;
    } else {
       dyn$_free(w, DYN$K_SYNC);
    }
   } else {
     /* must be a N ev */

    i = atoi(&name[1]);
    p = dyn$_malloc( sizeof(struct registered_portname), DYN$K_SYNC | DYN$K_NOOWN );
    p->sb_allowed = 1;
    if ( strlen(value)==16 ) {
       memcpy(p->portname,value,16);
       portname_ev_mask |= 1 << i;
       p->evnum = i; 
       portnames[num_portnames++] = p;
    } else {
       dyn$_free(p, DYN$K_SYNC);
    }

   }
EXIT:
 return( value );
} 


/*
 * kgpsa_ev_init -
 *
 *
 * On the monolithic consoles, this flow is straightforward - that is -
 * if the ev does not exist, then create it in this driver starup flow.
 *
 * On a Galaxy Turbo, we have to do some checking.  Any ev in non-volatile
 * ram is created at startup, without any action routine or special init.
 * Then this code is called during the DDB$K_ASSIGN flow from config_adapt
 * at powerup.   So some wwid<> and N<> ev's may already exist if they were
 * found in the nvram.  The ones that were not found get created, and the ones
 * that were found have to have their action routines plugged in and we have
 * to build up the wwids[] and portnames[] arrays at this time too.   Also
 * we have to know which state of init the ev has seen, the powerup state,
 * the ddb$k_assign state, or the fully init'ed state.   
 *
 * Let me put it another way, since this is confusing.
 *   Suppose :
 *         2 KGPSAs in the system
 *         NVRAM has stored in it wwid0
 *
 *    a. Powerup flow ev_init creates wwid0
 *
 *    b. config_adapt runs KGPSA0 Assign rtn -
 *               semi initialized wwid0 found -> install action routine; 
 *                                               wwids[0] set to value
 *                                               num_wwids=1
 *               wwid1 not found -> create ev; 
 *               wwid2 not found -> create ev; 
 *               wwid3 not found -> create ev; 
 *       all wwid<n> ev's are now fully initialized.
 *
 *    c. config_adapt runs KGPSA1 Assign rtn -
 *               fully init'ed wwid0 found -> do nothing
 *               fully init'ed wwid1 found -> do nothing
 *               fully init'ed wwid2 found -> do nothing
 *               fully init'ed wwid3 found -> do nothing
 *       The way we determine fully init'ed is if the ev has the write
 *       action routine set.
 *
 */
void kgpsa_ev_init( )
{
 int i,j, wrrtn, rdrtn;
 struct EVNODE *evp;

#if (STARTSHUT||DRIVERSHUT)
 rdrtn = ovly_call_save("KGPSA", ev_rd_fc_$offset);
 wrrtn = ovly_call_save("KGPSA", ev_wr_fc_$offset);
#endif

 for(j=0;j<MAX_WWID_ALIAS_EV;j++)
 {
   sprintf(fc_wwidx_ev.ev_name,"wwid%x",j);
   if( (ev_locate(&evlist, fc_wwidx_ev.ev_name, &evp)) != msg_success ) {
      ev_init_ev(&fc_wwidx_ev);
#if (STARTSHUT||DRIVERSHUT)
      ev_install_action (fc_wwidx_ev.ev_name, wrrtn, 0 );

   } else {
      if( evp->wr_action == 0 ) {
        ev_install_action (fc_wwidx_ev.ev_name, wrrtn, 0 );
        ev_fc_init_sub( evp->name, evp->value.string );
      }
#endif
   }
 }

 for(j=1;j<=MAX_PORTNAME_ALIAS_EV;j++)
 {
   sprintf(fc_Nx_ev.ev_name,"N%x",j);
   if( (ev_locate(&evlist, fc_Nx_ev.ev_name, &evp)) != msg_success ) {
      ev_init_ev(&fc_Nx_ev);
#if (STARTSHUT||DRIVERSHUT)
      ev_install_action (fc_Nx_ev.ev_name, wrrtn, 0 );
   } else {
      if( evp->wr_action == 0 ) {
        ev_install_action (fc_Nx_ev.ev_name, wrrtn, 0 );
        ev_fc_init_sub( evp->name, evp->value.string );
      }
#endif
   }
 }

}

