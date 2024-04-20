/* file:    dw_driver.c
 *
 * Copyright (C) 1992, 1995 by
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
 * and  should  not  be  construed  as a commitment by digital equipment
 * corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by digital.
 */

/*
 *++
 *  FACILITY:
 *
 *      Mikasa Firmware
 *
 * ABSTRACT:	Port driver for the Pci/Eisa token ring adaptors
 *
 *  Functionally, this driver accepts fully formatted ethernet or IEEE 802.3
 *  frames and translates them to IEEE 802.5 Token Ring frames and then
 *  transmits them on the token ring.  Ethernet multicast addresses are
 *  translated to Token Ring functional addresses whenever possible.
 *
 *  The driver also receives token ring frames and translates them to the
 *  appropriate Ethernet or IEEE 802.3 frame format, using empirical tests to
 *  decide which format is appropriate, and passes them to a higher level 
 *  received message handler. (Note: the driver does not currently translate 
 *  received token ring functional addresses to the corresponding ethernet 
 *  multicast addresses, partly because there is currently no requirement 
 *  to accept functional/multicast messages.)
 *
 *  Token Ring Source Routing is optionally supported, and is controlled
 *  by an envirnment variable settable from the console. It defaults to 
 *  source routing disabled.
 *
 * OVERVIEW:
 *
 *  NOTE: This token ring driver has the following unique options which
 *        can be set from the console:
 *
 *        Enable / Disable (default = Disable) Source Routing for Token Ring
 *        <short_device_name>_source_route = 1 (Enable) or 0 (Disable)
 *
 *        example:  >>> set dwa0_source_route 1
 *
 *  AUTHORS:
 *
 *  Lowell Kopp
 *
 *  CREATION DATE:
 *
 *      14-Oct-1994
 *
 *  MODIFICATION HISTORY:
 *
 *  dwn 12-Sep-1995 Fixed station address printing. 
 *
 *  dwn 21-Aug-1995 Changed "show dev" displaying of LAN Address to cannonical 
 *		    format.
 *
 *  dwn 10-Jul-1995 Added Pci TR support.  Verified overall functionality.
 *		    Verified/Corrected net/nettest compatibility.  
 *
 *  swt	25-Jun-1995 add Noritake platform.
 *
 *  lek 25-Jan 1995 Successful MOP and BOOTP boots, driver basically functional
 *
 *  lek 14-Oct 1994 Initial entry.  Derived partly from ER_DRIVER.C and other
 *                  and other sources.
 *--
*/

#include "cp$src:platform.h"
#include "cp$src:kernel_def.h"      /* Kernel definitions */
#include "cp$src:msg_def.h"         /* Message definitions */
#include "cp$src:prdef.h"           /* Processor definitions */
#include "cp$src:common.h"          /* Common definitions */
#include "cp$inc:prototypes.h"
#include "cp$src:dynamic_def.h"     /* Memory allocation defs */
#include "cp$src:ni_env.h"          /* NI Environment variables */
#include "cp$src:ev_def.h"          /* Environment vars */
#include "cp$src:ni_dl_def.h"       /* Datalink definitions */
#include "cp$src:pb_def.h"          /* Port block definitions */
#include "cp$src:mop_counters.h"    /* MOP counters */
#include "cp$src:eisa.h"            /* Eisa bus definitions */
#include "cp$inc:platform_io.h"     /* System bus definitions */
#include "cp$src:probe_io_def.h"    /* */

#include "cp$src:dw300.h"           /* Driver specific definitions */
#include "cp$src:tms380.h"          /* TMS380 definitions */
#include "cp$src:tms380_def.h"      /* TMS380 struct definitions */
#include "cp$src:dw_pb.h"           /* dw_driver port block definition */

/* see TMS380FW.C for definition of TMS380_Firmware [] */

#include "cp$inc:kernel_entry.h"

#define window_base io_get_window_base(pb)

#define DW_OUT_USE_PPRINTF 0

#define DW_TRACE      0		/* enables trace messages to log */
#define DW_TXRX       0 	/* enables TX RX messages to log */
#define DW_DEBUG      0 	/* enables various debug messages to log */
#define DW_DEBUG_INT  0		/* enables pprintf msg in interrupt handler */

#define DW_FYI        0		/* enables some optional info fns */

/* -- NOTE --- 
/*	use either  DW_DEBUG_CONVERT or 
/*	(exclusive) DW_DEBUG_PROTOCOL 
*/
#define DW_DEBUG_CONVERT  0 /* enables pprint_pkt before & after conversion */
#define DW_DEBUG_PROTOCOL 0 /* enables printing 1st 60 bytes of pkt 
    			    /* to/from higher levels (Ethernet frame only) 
    			    */


#define DW_DEBUG_INIT 	  0 /* enables msgs related to initialization */

/*
 * These are present in virtually all alpha firmware drivers ...
 * copy 'em here and decide whether to use them later --- lek
 */


#if TRUE	/* apparently not used */
/*Some debug stuff*/
#if SABLE || MIKASA || ALCOR
#define     DW_POLLED 0 
#else
#define     DW_POLLED 1
#endif
#endif		/* apparently not used */

#define _addq_v_u(x,y) addq_v_u(&(x),y)
#define malloc(x) dyn$_malloc(x,DYN$K_FLOOD|DYN$K_SYNC|DYN$K_NOOWN)
#define free(x)   dyn$_free(x,DYN$K_SYNC|DYN$K_NOOWN)

/* packing / unpacking macros
*/
#define MAKEWORD( hi, lo ) ((hi << 8) | (lo))
#define MAKELONG( hi, lo ) ((hi << 16) | (lo))
#define HIBYTE16( x ) (x >> 8)
#define LOBYTE16( x ) (x & 0xff)
#define HIWORD32( x ) (x >> 16)
#define LOWORD32( x ) (x & 0xffff)

 
/* External references 
*/
extern int dw_controllers;      /*Used for controller letters*/
extern int cb_ref;              /*Call backs flag*/
extern int cb_fp[];
extern struct QUEUE pollq;      /*POLLQ         */
extern int spl_kernel;          /*Kernel spinlock   */
extern USHORT TMS380_Firmware [];

#if TRUE  /* USE MINIMAL FORWARD DECLARATIONS */ 
/* Use minimal forward declarations --- the system code has so many
 * inappropriate function argument definitions -- it would take
 * a lot of casting to make this compile clean with full ANSI function
 * prototypes, and there's no time to do that.  Also be careful: many
 * functions are called with an inappropriate number of arguments --
 * dyn$_malloc() normally called with 2 args, sometimes 4 but expecting 5;
 * krn$_post(), krn$_bpost() normally called with 1 arg, but expect 2;
 * -- are only a few examples.  Some functions, especially those involving
 * the system port block are called with a pointer to the extended
 * driver port block, in which the system port block is the first
 * member -- those references should be pb->pb, not just pb (which
 * works, but is very bad practice). */

#define _MB() mb()
extern void mb();

extern int err_printf();
extern int pprintf();
extern int sprintf();
extern void * find_pb();
extern int dyn$_malloc();
extern int dyn$_free();
extern int krn$_seminit();
extern int krn$_post();
extern int krn$_bpost();
extern int krn$_wait();
extern int krn$_semrelease();
extern int insq();
extern int remq();
extern int insq_lock();
extern int remq_lock();
extern void *ndl_allocate_pkt();
extern void ndl_deallocate_pkt();
extern int ndl_init();
extern void init_mop_counters( struct MOP$_V4_COUNTERS *mc );
extern void pprint_pkt();
extern char *memset();
extern char *memcpy();
extern char *strcpy();
extern char *strncpy();
extern int allocinode();
extern int ilist_lock();
extern int ilist_unlock();
extern int null_procedure();
extern int set_io_name();
extern int krn$_create();
extern void spinlock();
extern void spinunlock();
extern void krn$_micro_delay();
extern int krn$_sleep();
extern int krn$_init_timer();
extern int krn$_release_timer();
extern int krn$_start_timer();
extern int krn$_stop_timer();
extern int use_eisa_ecu_data();
extern int io_issue_eoi();
extern int addq_v_u();
extern int mop_set_env();
extern int int_vector_set();
extern int fopen();
extern int ev_write();
extern int ev_read();
extern int ni_error();
extern int printf();
extern int set_timer();
extern int wait_msg();

#else
/* printf.c */
extern int err_printf( char *format, int va_alist );
extern int pprintf( char *format, int va_alist );
extern int sprintf( char *buf, char *format, int va_alist );

/* mikasa.c */
extern void * find_pb( char *device, int size, int (*callback)(struct pb *pb) );

/* dynamic.c -- mem allocation:  (addr % modulus) = remainder */
#if TRUE
/* all the mem allocations in this driver ignore the modulus, remainder
 * and zone args (as do most of the drivers) */
extern int dyn$_malloc( unsigned int size, unsigned int option );
#else
extern int dyn$_malloc( unsigned int size, unsigned int option, unsigned int modulus,
                 unsigned int remainder, struct ZONE zone );
#endif
extern int dyn$_free( void *p, int option );

#if FALSE
/* kernel.c  !!! */
extern int dyn$_malloc( int size, int type, int modulus, 
                        int address, int foo );
#endif
 
/* semaphore.c 
*/
extern int krn$_seminit( struct SEMAPHORE *s, int init_count, char *name );
extern int krn$_post( struct SEMAPHORE *s, int val );
extern int krn$_bpost( struct SEMAPHORE *s, int val );
extern int krn$_wait( struct SEMAPHORE *s );
extern int krn$_semrelease( struct SEMAPHORE *s );

/* queue.c 
*/
extern int insq( struct QUEUE *entry, struct QUEUE *header );
extern int remq( struct QUEUE *header );
extern int insq_lock( struct QUEUE *entry, struct QUEUE *header );
extern int remq_lock( struct QUEUE *header );

/* ni_datalink.c 
*/
extern void *ndl_allocate_pkt( struct INODE *ip, int rem_mask );
extern void ndl_deallocate_pkt( struct INODE *ip, void *pkt );
extern int ndl_init( struct INODE *ip, struct PORT_CALLBACK *cb, 
              unsigned char *sa, int *rp );
extern void init_mop_counters( struct MOP$_V4_COUNTERS *mc );
extern void pprint_pkt( void *pkt, int size );

/* memset.c 
*/
extern char *memset( void *dst, unsigned char val, int length );
extern char *memcpy( char *dst, char *src, int length );

/* strfunc.c 
*/
extern char *strcpy( char *s1, char *s2 );
extern char *strncpy( char *s1, char *s2, size_t n );

/* filesys.c 
*/
extern int allocinode( char *name, int can_create, struct INODE **ip  );
extern int ilist_lock( );
extern int ilist_unlock( );

/* kernel.c 
*/
extern int null_procedure( void );

/* kernel_support.c 
*/
extern int set_io_name( char *name, char *prefix, int unit, 
                        int node_id, struct pb *pb );

/* mtpr_mfpr_call.mar 
*/
#define _MB() mb()
extern void mb();

/* process.c 
*/
extern int krn$_create( int pc, int startup, struct SEMAPHORE *sem,
        int pri, int affinity, int actual_stack, char *name, char *in,
        char *in_mode, char *out, char *out_mode, char *err,
        char *err_mode, int a0, int a1, int a2, int a3 );

/* sport_routines.c -- actual implementation is in kernel.mar,
 * this c file is not used in mikasa 
*/
extern void spinlock( struct LOCK *lock );
extern void spinunlock( struct LOCK *lock );

/* timer.c 
*/
extern void krn$_micro_delay( int us );
extern int krn$_sleep( int ms );
extern int krn$_init_timer( struct TIMERQ *tqe );
extern int krn$_release_timer( struct TIMERQ *tqe );
extern int krn$_start_timer( struct TIMERQ *tqe, int ms );
extern int krn$_stop_timer( struct TIMERQ *tqe );

/* TO BE VERIFIED 
*/
extern int use_eisa_ecu_data( struct pb *pb );
extern int io_issue_eoi( );
extern int addq_v_u( );
extern int mop_set_env( );
extern int int_vector_set( );

#endif	/* USE MINIMAL FORWARD DECLARATIONS */ 

/*External routine definitions
*/
extern UBYTE  inportb  (struct pb *port_block, unsigned __int64 addr);
extern USHORT inportw  (struct pb *port_block, unsigned __int64 addr);
extern ULONG  inportl  (struct pb *port_block, unsigned __int64 addr);
extern void   outportb (struct pb *port_block, unsigned __int64 addr, UBYTE  data);
extern void   outportw (struct pb *port_block, unsigned __int64 addr, USHORT data);
extern void   outportl (struct pb *port_block, unsigned __int64 addr, ULONG  data);

extern void io_enable_interrupts  (struct pb *port_block, int vector );
extern void io_disable_interrupts (struct pb *port_block, int vector );

/* DW300 / TMS380 CSR ACCESS MACROS 
*/
#define SIFACL (pb->a_sifacl)
#define SIFADX (pb->a_sifadx)
#define SIFADR (pb->a_sifadr)
#define SIFINC (pb->a_sifinc)
#define SIFCMDSTS (pb->a_sifcmdsts)
#define SIFSTS_BYTE SIFCMDSTS		/* little endian */ 
#define SIFDAT (pb->a_sifdat)
#define SIFDMALEN (pb->a_sifdmalen)

#define ZC86 ( (ULONG) pb->regp+0x0c86 )
#define ZC87 ( (ULONG) pb->regp+0x0c87 )
#define MMR  ( (ULONG) pb->regp+0x0080 )

/* important Ethernet multicast and Token Ring Functional addresses 
*/
UBYTE mop_dl_multicast [] = { 0xab, 0x00, 0x00, 0x01, 0x00, 0x00 };
UBYTE mop_dl_func_addr [] = { 0xc0, 0x00, 0x40, 0x00, 0x00, 0x00 };
UBYTE mop_rc_multicast [] = { 0xab, 0x00, 0x00, 0x02, 0x00, 0x00 };
UBYTE mop_rc_func_addr [] = { 0xc0, 0x00, 0x20, 0x00, 0x00, 0x00 };
UBYTE broadcast_addr []   = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
UBYTE broadcast_addr2 []  = { 0xc0, 0x00, 0xff, 0xff, 0xff, 0xff };

/* INIT RESULT RETURN VALUES 
*/
#define MSG_BUD_FAILED      2
#define MSG_INIT_FAILED     1
#define MSG_INIT_PASSED     0

/* EBCDIC constants used in adapter software level encoding 
*/
#define ebcdic_c  0xc3
#define ebcdic_e  0xc5
#define ebcdic_l  0xcc
#define ebcdic_m  0xcd




/* function prototypes 
*/
/* --------- Initialization ------------------ */
int dw_init ();
int dw_init_pb( struct DW_PB *pb );
int dw_init_I( char *file_name, struct DW_PB *pb );
int dw_init_driver( struct INODE *ip );
int dw_init_adapter( struct DW_PB *pb );
int dw_alloc_scb_ssb_plist( struct DW_PB *pb );
int dw_dealloc_scb_ssb_plist( struct DW_PB *pb );

/* --------- Open / Close -------------------- */
int dw_open( struct FILE *fp );
int dw_open_adapter( struct DW_PB *pb );
int dw_close( struct FILE *fp );

/* --------- RCV processing ------------------ */
int dw_issue_receive_cmd( struct DW_PB *pb );
int dw_init_rx_plists( struct DW_PB *pb );
int dw_rx_proc( struct INODE *ip );
void dw_msg_rcvd( struct FILE *fp, struct DW_PB *pb,
                     struct MOP$_V4_COUNTERS *mc, 
                     ETHERNET_FRAME *msg, USHORT size );
int dw_read( struct FILE *fp, int size, int number, UBYTE *c);
void dw_free_rcv_pkts( struct INODE *ip );
int dw_process_rx_plist( struct DW_PB *pb, struct NI_ENV *ev, 
                     struct MOP$_V4_COUNTERS *mc,
                     PARAM_LIST *plist, ETHERNET_FRAME *enetf );
PARAM_LIST *dw_release_rcv_plist( struct DW_PB *pb, PARAM_LIST *plist );
int convert_tokenring_to_enet( struct DW_PB *pb, ETHERNET_FRAME *enf, 
                                  TOKEN_RING_HEADER *trf, int len );
/* --------- XMT processing ------------------ */
int dw_init_tx_plists( struct DW_PB *pb );
void dw_tx_proc( struct INODE *ip );
int dw_process_tx_plist( struct INODE *ip, PARAM_LIST *plist );
int dw_write( struct FILE *fp, int item_size, int number, ETHERNET_FRAME *buf );
int dw_callback_write( struct FILE *fp, int item_size, int number, char *buf );
int dw_out( struct DW_PB *pb, PARAM_LIST *plist, int len, USHORT flags );
PARAM_LIST *dw_get_next_xmt_plist( struct DW_PB *pb );
void dw_release_xmt_plist( struct DW_PB *pb, PARAM_LIST *plist );
int convert_enet_to_tokenring( struct DW_PB *pb, 
                                  TOKEN_RING_HEADER *trf,
                                  ETHERNET_FRAME *enf, int len );
int dw_filter_dest_addr( SA *a_tr, SA *a_enet  );
int dw_match_addr( SA *addr1, SA *addr2 );

int dw_setmode( int mode );
int dw_stop_driver( struct INODE *ip );
int dw_restart_driver( struct INODE *ip );

/* --------- Interrupt processing ------------------ */
int dw_poll( struct NI_GBL *np );
void dw_interrupt ( struct NI_GBL *np );
void dw_handle_cmd_reject( struct DW_PB *pb, USHORT cmd_id, USHORT cmd_status );

/* --------- NI environment processing ------------------ */
void dw_set_env( char *name );
void dw_read_env( struct NI_ENV *ev, char *name );
                           
/* --------- misc ------------------ */
void dw_control_t( struct NI_GBL *np );
int dw_change_mode( struct FILE *fp, int mode );

int dw_read_error_log( struct DW_PB *pb );
int dw_update_error_counters( struct DW_PB *pb );


void dw_dump_csrs(struct NI_GBL *np );
USHORT dw_read_csr( struct DW_PB *pb, ULONG a_csr );
UBYTE dw_read_byte_csr( struct DW_PB *pb, ULONG a_csr );
void dw_write_csr( struct DW_PB *pb, ULONG a_csr, USHORT value );
void dw_write_byte_csr( struct DW_PB *pb, ULONG a_csr, UBYTE value );
                 
/* --------- command synchroniztion ----------------- */                 
SCB *dw_get_scb( struct DW_PB *pb );
int dw_release_scb( struct DW_PB *pb );
int dw_issue_next_cmd( struct DW_PB *pb );
int dw_write_cmd( struct DW_PB *pb );

    /* to be verified */

/* OPTIONAL DEBUG FUNCTIONS */

#if DW_DEBUG
#endif

#if DW_DEBUG_INIT
void show_plists( struct DW_PB *pb, PARAM_LIST *a_plist, int plist_cnt );
void show_plist( PARAM_LIST *plist );
#endif

/* OPTIONAL INFO FUNCTIONS */

#if DW_FYI
void print_sw_level_info( struct DW_PB *pb );
void dw_show_ring_status( struct DW_PB *pb );
void print_scb_ring( struct DW_PB *pb );
#endif



struct PBQ _align (QUADWORD) dw_pb_list;       /* See KERNEL_DEF.H */


/*
 *  NOTE: obvious port driver name "tr" is already in use by
 *  protocol driver (translate).  We will need a two letter
 *  device name, like "tr", for port identifier. Instead,
 *  use "dw" ... then "dwa0" etc
 *
 *  NOTE: integrating this driver into the MIKASA console
 *  requires:
 *  1.  entries in mikasa.c: io_device []
 * { TYPE_EISA, 0x02604f42, "dw", 0, "dw", &dw_controllers },   / * PRO6002 * /
 * { TYPE_EISA, 0x01604f42, "dw", 0, "dw", &dw_controllers },   / * PRO6001 * /
 * { TYPE_EISA, 0x00604f42, "dw", 0, "dw", &dw_controllers },   / * PRO6000 * /
 *      to allow EISA buf-sniffer to recognise the device (any of three 
 *      different adapter ids may be found) 
 *      also, declare "int dw_controllers;" and init to 0;
 *
 *  2.  an entry in hwrpb.c: fd_table []
 * { "dw", "dw", 0, "MOP", " 6 0" },
 * { "dw", "dw", 1, "BOOTP", " 6 0" }, 
 *      to make dw device usable as a boot device
 *
 *  3. inet_driver.c has been modified to test for device "dw" in bootp_open()
 *     in order to override the hard-wired ethernet assumption and provide
 *     token ring (IEEE 802.5) hardware type id to the BOOTP and ARP protocol
 *     handlers.  If this device name changes (e.g. to tra0) edit the
 *     statement if ( strncomp_nocase( next, "dw", 2 ) == 0 )
 *
*/
struct DDB dw_ddb = {
    "dw",               /* how this routine wants to be called  */
    dw_read,            /* read routine                         */
    dw_write,           /* write routine                        */
    dw_open,            /* open routine                         */
    dw_close,           /* close routine                        */
    null_procedure,     /* name expansion routine               */
    null_procedure,     /* delete routine                       */
    null_procedure,     /* create routine                       */
    dw_setmode,         /* setmode                              */
    null_procedure,     /* validation routine                   */
    0,                  /* class specific use                   */
    0,                  /* allows information                   */
    0,                  /* must be stacked                      */
    0,                  /* is a flash update driver             */
    0,                  /* is a block device                    */
    0,                  /* not seekable                         */
    3,                  /* ddb_ptr->net_device */
                        /* 1==ENET, 2=FDDI,3=TOKENRING */
    0,                  /* is a filesystem driver               */
};

/* ?????????????????????????????????????
 *
 *  NOTE: TOKENRING network device is not defined for system ...
 *  should we define it (in kernel_def.h) ?
 *  #define DDB$K_TOKENRING 3
 *
 * ????????????????????????????????????? */


/*+
 * ============================================================================
 * = dw_init - Initialization routine for the port driver                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  This routines will initialize the "dw300" port.
 *
 * RETURNS:
 *  Status - msg_success or FALIURE.
 *
 * ARGUMENTS:
 *      None
-*/
int dw_init () {

#if DW_TRACE
    err_printf( "dw_init\n", 0 );
#endif

    /* Initialize the port block list 
    */
    dw_pb_list.flink = &dw_pb_list;
    dw_pb_list.blink = &dw_pb_list;

    /* find the port block or create and initialize a new one 
    */
    find_pb( "dw", sizeof( struct DW_PB ), dw_init_pb );

    return (msg_success);
    }


/*+
 * ============================================================================
 * = dw_init_pb - =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * RETURNS:
 *
 * ARGUMENTS:
 *
-*/
int dw_init_pb( struct DW_PB *pb ) {
    char pname[32];         /*Port name*/

#if DW_TRACE
    err_printf( "dw_init_pb %s\n", pb->pb.name );
#endif

    set_io_prefix( pb, pname, "" );
    dw_set_env( pname );
    mop_set_env( pname );

    /* Do the first stage of the driver init
    */
    set_io_name( pname, 0, 0, 0, pb );
    dw_init_I( pname, pb );

    return (msg_success);   
    }



/*+
 * ============================================================================
 * = dw_init_I - Inode initialization routine for the "dw300" port driver  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *  This routines will initialize the TMS380 port driver. It allocates the
 *  inode, sets up the driver data base, and calls the higher level
 *  initialization routines.
 *
 * RETURNS:
 *  int Status - msg_success or FAILURE.
 *
 * ARGUMENTS:
 *  *file_name - Filename of the device to be opened.
 *  *pb        - pointer to port block.
 *
-*/
int dw_init_I( char *file_name, struct DW_PB *pb ) {
    struct NI_GBL *np;      /* Pointer to the NI global database */
    struct INODE *ip;       /* Pointer to the INODE */
    struct NI_DL *dl;       /* Pointer to the datalink*/
    struct PBQ *pbe;        /* See KERNEL_DEF.H */

    /* Create an inode entry, thus making the device visible as a file. 
    */
    allocinode( file_name, 1, &ip );

    ip->dva  = &dw_ddb;
    ip->attr = ATTR$M_READ | ATTR$M_WRITE;
    ip->loc  = 0;
    ip->misc = (int*)malloc( sizeof(struct NI_GBL) );
    INODE_UNLOCK (ip);

    np      = (struct NI_GBL*) ip->misc;
    np->pbp = (int *) pb;

    strcpy( pb->pb.name, file_name );  /* ??? is this necessary? name is set later */

#if DW_TRACE
    err_printf( "dw_init_I %s\n", pb->pb.name );
#endif

    /* Initialize the port spinlock 
    */
    pb->spl_port.value    = 0;
    pb->spl_port.req_ipl  = IPL_SYNC;
    pb->spl_port.sav_ipl  = 0;
    pb->spl_port.attempts = 0;
    pb->spl_port.retries  = 0;
    pb->spl_port.owner    = 0;
    pb->spl_port.count    = 0;

    /* Driver Startup/ShutDown Information:
    /*   Put this Inode on the TokenRing Inode queue.
    /*   This queue saves the Inode pointer of all TokenRing port drivers 
    /*   started.  It will be used later to either shutdown/startup them via
    /*   dw_setmode().
    */
    pbe     = (struct PBQ*) malloc( sizeof(struct PBQ) );
    pbe->pb = (int*) ip;  

    insq_lock( pbe, dw_pb_list.blink );

    /* Initialize the msg received list 
    */
    spinlock( &spl_kernel );
    pb->rcv_msg_cnt      = 0;
    pb->rcv_msg_qh.flink = &(pb->rcv_msg_qh);
    pb->rcv_msg_qh.blink = &(pb->rcv_msg_qh);
    spinunlock( &spl_kernel );

    /* Init critical datalink stuff 
    */
    np->dlp = (int*) malloc( sizeof(struct NI_DL) );

    /* Setup adapter csr addresses 
    */
    if (pb->pb.type == TYPE_EISA)
    	pb->regp = (struct DW_REGS*) ((pb->pb.slot<<12)|CSR_BASE);
    else
        pb->regp = (struct DW_REGS*)(incfgl(pb, 0x10) & 0xfffffff0);

    pb->a_sifacl    = (ULONG) &pb->regp->dw_w_sifacl;
    pb->a_sifadx    = (ULONG) &pb->regp->dw_w_sifadx;
    pb->a_sifadr    = (ULONG) &pb->regp->dw_w_sifadr;
    pb->a_sifinc    = (ULONG) &pb->regp->dw_w_sifinc;
    pb->a_sifcmdsts = (ULONG) &pb->regp->dw_w_sifcmdsts;
    pb->a_sifdat    = (ULONG) &pb->regp->dw_w_sifdat;
    pb->a_sifdmalen = (ULONG) &pb->regp->dw_w_sifdmalen;

    /* Initialize some flags 
    */
    pb->state = DW_K_ADPTR_UNINITIALIZED;

    /* get EISA config data 
    */
    if (pb->pb.type == TYPE_EISA)
        use_eisa_ecu_data( pb );

    /* respect DW_POLLED configuration 
    */
    if (DW_POLLED == 1)
        pb->pb.vector = 0;

    /* Set up the interrupt vector 
    */
    if (pb->pb.vector)
        {
        int_vector_set( pb->pb.vector, dw_interrupt, np );
        io_enable_interrupts (pb, pb->pb.vector);
        }

    /* if using ECU data 
    */
    if (pb->pb.type == TYPE_EISA)
        if (pb->pb.e_memq.flink)
    	    {
            pb->eisa_addr = (UBYTE*) pb->pb.e_memq.flink->addr;
            pb->eisa_size = pb->pb.e_memq.flink->size;
    	    }
        else
    	    {
            if (pb->pb.controller > 0)
                return(msg_failure);

            pb->eisa_addr = (UBYTE *) DW_K_EISA_ADDR;
            pb->eisa_size = DW_K_EISA_SIZE;
            }
   else
        {
        pb->eisa_addr = (UBYTE *)(incfgl(pb, 0x14) & 0xfffffff0);
        pb->eisa_size = 0;
        }


#if DW_DEBUG_INIT
    err_printf("\ndw_init_I: vector = %08x, eisa_addr = %08x, eisa_size = %08x\n",
        pb->pb.vector, pb->eisa_addr, pb->eisa_size );
#endif

    /* A drivers setmode() routine, can be called by either:
    /*	ddb->setmode(mode) or pb->pb.setmode(pb, mode)
    /*
    /* In either case, only ONE (1) or NONE can be defined.
    /* This dw_driver.c uses the ddb->setmode(mode) method therefore
    /* pb->pb.setmode is NULLed.  See filesys.c
    */
    pb->pb.setmode = 0;

    if (DW_POLLED == 1)
        pb->pb.mode = DDB$K_POLLED;  
    else
        pb->pb.mode = DDB$K_INTERRUPT;

    pb->pb.channel = 0;

    strcpy( pb->pb.name, ip->name );

    set_io_alias( pb->pb.alias, 0, 0, pb );

    pb->pb.sb     = 0;
    pb->pb.num_sb = 0;

    /* Start the next stage of initilization 
    */
    if (dw_init_driver( ip ) )
        {
        err_printf("\n%s Init Driver Error \n", ip->name );
        return msg_failure;
        }

    return(msg_success);
    }


/*+
 * ============================================================================
 * = dw_init_driver - Init the TMS380 port driver                           =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *  This process will initialize the port driver. In doing so it
 *  start the transmit and receive processes, initializes the mop
 *  counters, receive and transmit descriptors, the TMS380, and filters.
 *
 * RETURNS:
 *      int Status - msg_success or FAILURE.
 *
 * ARGUMENTS:
 *  *ip - Pointer to the INODE.
 *
-*/
int dw_init_driver( struct INODE *ip ) {
    struct MB *mbp;              /*Pointer to the MOP block*/
    struct NI_GBL *np;           /*Pointer to the NI global database*/
    struct NI_ENV *ev;           /*Pointer to the environment variables*/
    struct DW_PB *pb;            /*Pointer to the port block*/
    struct MOP$_V4_COUNTERS *mc; /*Pointer to the MOP counters*/
    struct PORT_CALLBACK *cb;    /*Callback structure*/
    char   name[32];             /*General holder for a name*/
    char *st,*s;

    int    retrycnt;             /* adapter init retry counter */
    int    msg;                  /* return value from adapter init */

    USHORT copy;                 /* temp csr copy */

    unsigned char
    	k, i, j;

    unsigned char 
    	tmp,
    	rslt[6];    

#if DW_TRACE
    err_printf( "\ndw_init_driver %s\n", ip->name);
#endif

    np = (struct NI_GBL*) ip->misc;
    np->enp = (int*)malloc(sizeof(struct NI_ENV));
    ev = (struct NI_ENV*) np->enp;
    pb = (struct DW_PB*) np->pbp;
    mbp = (struct MB*)np->mbp;

    /*	Replaced these ev-> variables with pb->
    */
    pb->xmt_buf_no   = DW_K_XMT_BUF_CNT;
    pb->rcv_buf_no   = DW_K_RCV_BUF_CNT;
    pb->driver_flags = NDL$K_DRIVER_FLAGS_DEF|NDL$M_ENA_BROADCAST;

    /* ---------- Init some debug vars -------------
    */
    pb->opn_cntr            = 0;
    pb->close_cntr          = 0;
    pb->pend_adptr_opn_cntr = 0;

    np->mcp = (int*) malloc( sizeof(struct MOP$_V4_COUNTERS) );
    mc      = (struct MOP$_V4_COUNTERS*) np->mcp;

    pb->lrp = 0;

    pb->it = 0;	    /*Initialize init timer*/

    /* Initialize interrupt semaphores
    */
    sprintf( name, "%4.4s_isr_tx", ip->name );
    krn$_seminit( &(pb->dw_isr_tx_sem), 0, name );

    sprintf( name, "%4.4s_isr_rx", ip->name );
    krn$_seminit( &(pb->dw_isr_rx_sem), 0, name );

    /* Get a pointer to the transmit information packets
    */
    pb->ti = (struct DW_TX_INFO*) malloc( sizeof(struct DW_TX_INFO)* pb->xmt_buf_no);

    /* Initialize some counters 
    */
    pb->cntr_w_rcv_int = 0;
    pb->cntr_w_xmt_int = 0;

    /* adapter error log counters 
    */
    pb->cntr_w_line_error   = 0;
    pb->cntr_w_burst_error  = 0;
    pb->cntr_w_arifci_error = 0;
    pb->cntr_w_lostframe    = 0;
    pb->cntr_w_rcv_congest  = 0;
    pb->cntr_w_framecopy    = 0;
    pb->cntr_w_token_error  = 0;
    pb->cntr_w_dmabuserror  = 0;
    pb->cntr_w_dmaparerror  = 0; 
    pb->cntr_w_signal_loss  = 0;
    pb->cntr_w_hard_err     = 0;
    pb->cntr_w_softerr      = 0;
    pb->cntr_w_xmt_beacon   = 0;
    pb->cntr_w_lobe         = 0;
    pb->cntr_w_autoremoval_err = 0;
    pb->cntr_w_remove_stn      = 0;
    pb->cntr_w_single_station  = 0;
    pb->cntr_w_ring_recovery   = 0;
    
    /* Initialize the mop counters
    */
    init_mop_counters( mc );

    /* Setup the callback structure
    */
    cb     = (struct PORT_CALLBACK*) malloc( sizeof(struct PORT_CALLBACK) );
    cb->st = dw_control_t;
    cb->cm = dw_change_mode;
    cb->ri = null_procedure;
    cb->re = null_procedure;
    cb->dc = dw_dump_csrs;

    /* stop the adapter -- MUST LOAD ADAPTER CODE AFTER THIS 
    */
    dw_write_csr( pb, SIFACL, ACL_M_ARESET );
    krn$_micro_delay( 140 );     		


    /* Set up the poll queue, if we're not using interrupts 
    */
    if (!pb->pb.vector)
        {
        pb->pb.pq.flink   = & pb->pb.pq;
        pb->pb.pq.blink   = & pb->pb.pq;
        pb->pb.pq.routine = dw_poll;
        pb->pb.pq.param   = (int) np;
        insq_lock (&pb->pb.pq, &pollq);
        }

    /* allocate SCB, SSB, xmt & rcv parameter lists for adapter 
    */
    if (dw_alloc_scb_ssb_plist( pb ) != msg_success)
        return (msg_failure);   /* insufficient memory */

    dw_init_tx_plists( pb );		/* Init the transmit ring */
    dw_init_rx_plists( pb );		/*Init the receive ring */

    /* Start the transmit receive process
    */
    sprintf( name,"rx_%4.4s",ip->name );
    krn$_create( dw_rx_proc, null_procedure, 0, 5,-1, 1024 * 4,
                name, "tt", "r", "tt", "w", "tt", "w", ip );

    sprintf(name,"tx_%4.4s",ip->name);
    krn$_create( dw_tx_proc, null_procedure, 0, 5,-1, 1024 * 4,
                name, "tt", "r", "tt", "w", "tt", "w", ip );

    /* initialize the adapter - TI recommends that you retry the TMS380        
    /* initialization up to three times from the initial adapter reset step.   
    /* BUD (built-in-diagnostics are retried up to three times internally in   
    /* the initialization procedure                                            
    */
    retrycnt = 0;

    do

        msg = dw_init_adapter( pb );

    while ( msg == MSG_INIT_FAILED && ++retrycnt < 3 );

    if (msg != MSG_INIT_PASSED)
        return (msg_failure);

    /* get station address from the adapter BIA (burned-in-address) MSB to LSB 
    */
    dw_write_csr( pb, SIFADX, INIT_C_AXBASE );
    dw_write_csr( pb, SIFADR, pb->dw_mem.init_w_bia );_MB();

    copy      = dw_read_csr( pb, SIFINC );
    pb->sa[0] = HIBYTE16( copy );
    pb->sa[1] = LOBYTE16( copy ); _MB();

    copy      = dw_read_csr( pb, SIFINC );
    pb->sa[2] = HIBYTE16( copy );
    pb->sa[3] = LOBYTE16( copy ); _MB();

    copy      = dw_read_csr( pb, SIFINC );
    pb->sa[4] = HIBYTE16( copy );
    pb->sa[5] = LOBYTE16( copy );

    if (pb->sa[0] == 0xff && pb->sa[1] == 0xff)
        return (msg_failure);  /* BIA was zero or gave bad checksum */


    /* Typically we display all LAN addresses in "cannanical format", 
    /* while still needing to send the adapter the station address in 
    /* "bit reversed" order.
    /*
    /* pb->pb.info done for ">>> show dev dwa0"
    /* See setshow.c.  >>> show device
    */
    for (i=0; i<6; i++)
    	{
    	tmp     = pb->sa[i];     
    	rslt[i] = 0;

        for (j=0,k=7; j<8; j++,k--)
	    rslt[i] |= (( (tmp >> j) & 0x1) << k);
    	}

    sprintf( (char *) pb->pb.info, "%02X-%02X-%02X-%02X-%02X-%02X",
        rslt[0], rslt[1], rslt[2], rslt[3], rslt[4], rslt[5] );

    sprintf( (char *) pb->pb.string, "%-24s   %-8s   %24s",
        pb->pb.name, pb->pb.alias, pb->pb.info );

#if DW_DEBUG_INIT
    err_printf("\npb.name[%s], pb.alias[%s], pb.info[%s]\n",
       pb->pb.name, pb->pb.alias, pb->pb.info );
#endif


    /* Initialize the NI_DataLink, see NI_DATALINK.C
    /*  ndl_init() will init
    /*          *rp = (int)ndl_receive_msg
    /*
    /* Give ndl_init() the "bit reversed" formatted station address.  
    /* Internally we'll represent the station address in "bit reversed"
    /* format, excepting pb->pb.info.  file2dev() will look for token
    /* ring device type and display the cannonical format.
    */
    ndl_init( ip, cb, pb->sa, (int*)&(pb->lrp) );

#if DW_DEBUG
    {
    /* type==1==BOOTP
    /* type==0==MOP
    */
    char dname[EV_VALUE_LEN+1];
    file2dev( ip->name, dname, 1 );
    pprintf ("\nENV VAR NAME: %s\t", dname);
    }
#endif

    return (msg_success);
    }



/*+
 * ============================================================================
 * = dw_alloc_scb_ssb_plist - Allocate scb, ssb, and plists                   =
 * = dw_dealloc_scb_ssb_plist - Deallocate scb, ssb, and plists               =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *  Allocate SCB, SSB, and parameter lists in contiguous memory.  If
 *  successful, the physical addresses are computed for each item.
 *
 * INPUTS:
 *  pb = addr of dw300 extended port block
 *
 * OUTPUTS:
 *  lsb$a_mem contains pointer to memory (virtual address)
 *      pointers/phyaddress computations are stored for;
 *          scb, ssb, opn parameters, transmit list, receive list, and
 *          scratch space.
 *
 * RETURNS:
 *  msg_success - Success
 *
-*/
int dw_alloc_scb_ssb_plist( struct DW_PB *pb ) {
    struct NI_GBL *np;      /*Pointer to the NI global database*/
    struct NI_ENV *ev;      /*Pointer to the environment variables*/
    int i;
    SCB_LIST *a_scblist;
    char *a_last_alloc;

#if DW_TRACE
    err_printf( "dw_alloc_scb_ssb_plist %s\n", pb->pb.name );
#endif

    /* the dw300 supports only a single command at any time, so only a single
     * scb or ssb is required - note Proteon's IBM PC sample driver fragments  
     * copied ssbs to a ring buffer for temp storage (ring held 20 ssbs for
     * "performance reasons" Alpha is so much faster than a PC that we probably
     * don't need a ssb ring buffer 
    */


    /* Allocate the host SCB 
    */
    pb->a_scb = (SCB *) malloc( sizeof(SCB) );

    /* allocate a block of memory and subdivide it into SCB_LISTs 
    */
    pb->a_scblist_ring = (SCB_LIST *) malloc( sizeof( SCB_LIST ) * DW_K_SCBLISTS );

    /* Initialize the ring 
    */
    for (i=0, a_scblist = pb->a_scblist_ring; 
    	    i < DW_K_SCBLISTS; 
    	        i++, a_scblist++ )
        {    
        a_scblist->a_next_list = a_scblist + 1;
    	a_scblist->scb.scb_w_cmd = 0;
        }    

    /* Close the ring 
    */
    --a_scblist; 			/* backup to last scb list */
    a_scblist->a_next_list = pb->a_scblist_ring;

    /* Init the FIFO pointers and entry counter 
    */
    pb->no_free_scblists = DW_K_SCBLISTS;
    pb->a_next_free_scblist = pb->a_next_cmd_scblist = pb->a_scblist_ring;

#if DW_DEBUG_INIT
    print_scb_ring( pb );
#endif

    /* SSBs 
    */
    pb->a_ssb = (SSB *) malloc( sizeof(SSB) );

    /* ADAPTER CHECK_FRAME 
    */
    pb->a_check_frame = (CHECK_FRAME *) malloc( sizeof(CHECK_FRAME) );

    /* OPEN CMD BUFFER 
    */
    pb->a_opn_cmd_buf = (CMD_OPEN *) malloc( sizeof(CMD_OPEN) );

    /* ERROR LOG 
    */
    pb->a_error_log = (ERROR_LOG *) malloc( sizeof(ERROR_LOG) );

    /* NO USER SPECIFIED ALLIGNMENT 
    */
    /* Get the memory for the transmit parameter list(s)  
    /* Get the memory for the transmit translation buffers 
    /* Get the memory for the receive parameter list 
    /* Get the memory for the transmit translation buffers 
    */
    pb->a_xmtplist_ring = (PARAM_LIST *)        malloc( sizeof(PARAM_LIST) * pb->xmt_buf_no );
    pb->a_xmtbuf        = (TOKEN_RING_HEADER *) malloc( DW_K_XMT_BUF_SIZ   * pb->xmt_buf_no );
    pb->a_rcvplist_ring = (PARAM_LIST *)        malloc( sizeof(PARAM_LIST) * pb->rcv_buf_no );
    pb->a_rcvbuf        = (TOKEN_RING_HEADER *) malloc( DW_K_RCV_BUF_SIZ   * pb->rcv_buf_no );


#if DW_DEBUG_INIT
    /* ??? DUMMY malloc to define end of mem allocations */
    a_last_alloc = (char *)malloc( 1 );
    err_printf("\ndummy alloc addr = %08x\n", a_last_alloc );
#endif


    return (msg_success);   
    }



/*+
 * ============================================================================
 * = dw_dealloc_scb_ssb_plist - Deallocate scb, ssb, and plists               =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * ARGUMENTS:
 *
 * RETURNS:
 *
-*/
int dw_dealloc_scb_ssb_list( struct DW_PB *pb ) {
    struct NI_GBL *np;      /*Pointer to the NI global database*/
    struct NI_ENV *ev;      /*Pointer to the environment variables*/

#if DW_TRACE
    err_printf( "dw_dealloc_scb_ssb_list %s\n", pb->pb.name );
#endif

    /* SCBs 
    */
    free( pb->a_scb );
    pb->a_scb = (SCB *) 0; 
    free( pb->a_scblist_ring );
    pb->a_scblist_ring = (SCB_LIST *) 0;
    pb->no_free_scblists = 0;

    /* SSBs 
    */
    free( pb->a_ssb );
    pb->a_ssb = (SSB *) 0;

    /* OPEN CMD BUFFER 
    */
    free( pb->a_opn_cmd_buf );
    pb->a_opn_cmd_buf = (CMD_OPEN *) 0;

    /* ADAPTER CHECK_FRAME 
    */
    free( pb->a_check_frame );
    pb->a_check_frame = (CHECK_FRAME *) 0;

    /* ERROR LOG 
    */
    free( pb->a_error_log );
    pb->a_error_log = (ERROR_LOG *) 0;

    /* memory for the transmit parameter list(s)  
    */
    free( pb->a_xmtplist_ring );
    pb->a_xmtplist_ring = (PARAM_LIST *) 0;

    /* memory for the transmit translation buffers 
    */
    free( pb->a_xmtbuf );
    pb->a_xmtbuf = (TOKEN_RING_HEADER *) 0;

    /* memory for the receive parameter list 
    */
    free( pb->a_rcvplist_ring );
    pb->a_rcvplist_ring = (PARAM_LIST *) 0;

    /* memory for the transmit translation buffers 
    */
    free( pb->a_rcvbuf );
    pb->a_rcvbuf = (TOKEN_RING_HEADER *) 0;

    return (msg_success);   
    }


#if DW_FYI
/*+
 * ============================================================================
 * = print_scb_ring - .          =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * RETURNS:
 *
 * ARGUMENTS:
 *
-*/
void print_scb_ring( struct DW_PB *pb ) {
    int i;
    SCB_LIST *asl;

    asl = pb->a_scblist_ring;

    for (i=0; i < DW_K_SCBLISTS; i++)
        {
    	err_printf("\n%d: a_scblist: %08x; a_next: %08x\n", i, asl, asl->a_next_list);
    	err_printf("\n    cmd: %04x, param_0: %04x, param_1: %04x\n",
           asl->scb.scb_w_cmd, asl->scb.scb_w_param_0, asl->scb.scb_w_param_1 ); 
        asl = asl->a_next_list;
        }
    }
#endif


/*+
 * ============================================================================
 * = dw_init_adapter - Initialization routine for the TMS380 chip.          =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *  This routine will init the TMS380 chip a make it ready for transmitting and
 *  recieving. It is called after adpator's BUD (Bring-Up-Diagnostics) have
 *  completed successfully. This routine assumes the SCBs, SSBs and descriptors
 *  have been allocated and initialized.
 *
 * RETURNS:
 *
 *     int Status - msg_success or FAILURE.
 *
 * ARGUMENTS:
 *
 *     *pb - address of extended port block
 *
-*/
int dw_init_adapter( struct DW_PB *pb )	{
    USHORT chap, addr, count;  /* TMS380 download params */
    USHORT copy;               /* temp csr value */
    USHORT *fw;                 /* ptr to MAC code image */
    USHORT intcode;
    USHORT status;

    int 
    	pollcnt, 
    	retrycnt, 
    	bud_done, 
    	init_done,  /* misc cntrl vars */
    	i;

#if DW_TRACE
    err_printf( "dw_init_adapter %s\n", pb->pb.name );
#endif

    if (pb->state == DW_K_ADPTR_UNINITIALIZED)
    	{
    	/* Hardware Reset... ( Set ARESET in SIFACL Register)
    	/*	Refer to:
    	/*	    TMS380 Second Gerneration TokenRing User's Guide
    	/*		page 4-37
    	*/
    	dw_write_csr( pb, SIFACL, ACL_M_ARESET );
        krn$_micro_delay( 14 );     

        /* get parity and CPU strappings 
    	*/
        copy = dw_read_csr( pb, SIFACL );

        /* clear adapter reset, and halt CPU for boot load 
    	*/
        copy &= ~ACL_M_ARESET;
        copy !=  ACL_M_CPHALT;

        dw_write_csr( pb, SIFACL, copy );
        krn$_micro_delay( 14 );     /* 14 microseconds */

#if DW_TRACE
        err_printf( "Loading MAC code %s \n", pb->pb.name );
#endif

        /* load the MAC code into the TMS380 
        */

        /* The firmware is linked to the image at the symbol 'TMS380_Firmware'.  */
        /* The firmware is divided into segments.                                */
        /*                                                                       */
        /* Segment descriptors are 6 bytes (3 shorts)                            */
        /*                                                                       */
        /*     [    Chapter Address    ]   segment address in TMS380 memory      */
        /*     [    Offset             ]   offset in segment                     */
        /*     [    Word Count         ]   word (16-bit) count of segment data   */

        fw = TMS380_Firmware;

        for (chap=*fw; chap != 0xffff; )
    	    {
            chap  = *fw++;     /* segment addr */
            addr  = *fw++;     /* offset       */
            count = *fw++;     /* word count   */

            dw_write_csr( pb, SIFADX, chap ); _MB();
            dw_write_csr( pb, SIFADR, addr ); _MB();

            for( i=0; i < count; i++)
    	        {
                copy = *fw++;
                dw_write_csr( pb, SIFINC, copy ); _MB();
    	        }
    	    }


        if (pb->pb.type == TYPE_EISA)
            {
    	    /* set DMA address for gigabyte window in host memory
    	     * for Mikasa, this begins at 0x40000000, and is required because the
    	     * TMS380 dma only supports 30 bits of address. The dw300 adapter has
    	     * additional logic to hold and set the required address states for
    	     * the two msbits of the 32-bit address. 
    	    */
    	    copy = 0;
    	    if (window_base & 0x80000000)
    	        copy |= DW300_M_BIT31;
    	    if (window_base & 0x40000000)
    	        copy |= DW300_M_BIT30;
    	    dw_write_byte_csr( pb, MMR, LOBYTE16( copy ) );
            }


    	/* start the Bring-Up-Diagnostics (BUD) execution 
    	*/
        copy  = dw_read_csr( pb, SIFACL );
        copy &= ~ACL_M_CPHALT;
        dw_write_csr( pb, SIFACL, copy );

        /* poll for BUD completion 
    	/*
        /* retry BUD up to 3 times 
    	*/
        for (retrycnt=0, bud_done=FALSE; !bud_done && retrycnt < 3; retrycnt++)
            {
            for (pollcnt = 0; !bud_done && pollcnt < 6; pollcnt++)
    	        {
    	        krn$_sleep( 500 );
    	        status = dw_read_csr( pb, SIFCMDSTS );

    	        if (status == 0x0040)
    	            bud_done = TRUE;
    	        }

            /* do sw reset if we're going to retry 
    	    */
            if (!bud_done && retrycnt < 2)
                dw_write_csr( pb, SIFCMDSTS, 0xff00 );  /* sw reset */
    	    }

        if (!bud_done)   /* adapter hardware failure */
    	    {
            intcode = dw_read_csr( pb, SIFCMDSTS ) & 0x000f;

            switch (intcode)
    	        {
                case 0: pprintf( "DW300: Initial Test Error\n" ); break;
                case 1: pprintf( "DW300: Adapter Software Checksum Error\n" ); break;
                case 2: pprintf( "DW300: Adapter RAM error\n" ); break;
                case 3: pprintf( "DW300: Instruction Test Error\n" ); break;
                case 4: pprintf( "DW300: Context/Interrupt Test Error\n" ); break;
                case 5: pprintf( "DW300: Protocol Handler/RI Hardware Error\n" ); break;
                case 6: pprintf( "DW300: System Interface Register Error\n" );
    	        }

            /* BUD test failed - abort adapter initialization 
    	    */
#if DW_TRACE
            err_printf( "BUD Failed \n", 0 );
#endif
            return (MSG_BUD_FAILED);
    	    }

#if DW_TRACE
        err_printf( "BUD passed\n", 0 );
#endif

    	}  /* pb->state == DW_K_ADPTR_UNITIALIZED */




    /*	Init the adapter: DMA is not yet working...
    /*	Write the INIT BLOCK using direct IO
    */
    dw_write_csr( pb, SIFADX, INIT_AX );
    dw_write_csr( pb, SIFADR, INIT_A ); _MB();
    dw_write_csr( pb, SIFINC, INIT_M_RESERVED | DMA_OPTS ); _MB();
    dw_write_csr( pb, SIFINC, CMDXMT_VEC ); _MB();
    dw_write_csr( pb, SIFINC, RCVSTS_VEC ); _MB();
    dw_write_csr( pb, SIFINC, SCBCHK_VEC ); _MB();
    dw_write_csr( pb, SIFINC, DMA_RCV ); _MB();
    dw_write_csr( pb, SIFINC, DMA_XMT ); _MB();
    dw_write_csr( pb, SIFINC, INIT_C_DATH ); _MB();

    dw_write_csr( pb, SIFINC, HIWORD32( ((ULONG) pb->a_scb | window_base )) ); _MB();
    dw_write_csr( pb, SIFINC, LOWORD32(  (ULONG) pb->a_scb ) ); 	       _MB();

    dw_write_csr( pb, SIFINC, HIWORD32( ((ULONG) pb->a_ssb | window_base )) ); _MB();
    dw_write_csr( pb, SIFINC, LOWORD32(  (ULONG) pb->a_ssb ) ); 		_MB();

    dw_write_csr( pb, SIFCMDSTS, (CMD_M_INTADP|CMD_M_EXECUTE|CMD_M_SYSINT) ); _MB();

    /* poll for initialization complete, max 11 sec 
    */
    for (init_done=FALSE, pollcnt=0; !init_done && pollcnt < 24; pollcnt++)
        {
        krn$_sleep( 500 );   /* ms */
        status = dw_read_csr( pb, SIFCMDSTS ) & 0x00ff;

        if ((status & (STS_M_INIT | STS_M_TEST | STS_M_ERROR)) == 0)
           init_done = TRUE;
        else 
    	    if (status & STS_M_ERROR)
    	        {
                pprintf( "\nInitialization error %x", status & 0x000f );
                break;
    	        }
        }

    /* If init done successfully, 
    /* then verify test patterns in scb, ssb 
    */
    if (!init_done)
    	{
#if DW_DEBUG
        pprintf( "\nInitialization failed %x", status );
#endif
    	return (MSG_INIT_FAILED); /* ABORT - SHOULD RETRY FROM ADPTR RESET */
    	}
    else  
        {
        if ( pb->a_scb->scb_w_cmd != SCB_W_TEST1	|| 
    	     pb->a_scb->scb_w_param_0 != SCB_W_TEST2	|| 
    	     pb->a_scb->scb_w_param_1 != SCB_W_TEST3    || 
    	     pb->a_ssb->ssb_w_cmd     != SSB_W_TEST1    || 
    	     pb->a_ssb->ssb_w_param_0 != SSB_W_TEST2    || 
    	     pb->a_ssb->ssb_w_param_1 != SSB_W_TEST3    || 
    	     pb->a_ssb->ssb_w_param_2 != SSB_W_TEST4)

    	    {
#if DW_DEBUG
    	    pprintf( "\nBud DMA Failed");
#endif
    	    return (MSG_INIT_FAILED); /* ABORT - SHLD RETRY FROM ADPTR RESET */
    	    }
        }

    /* now read the TMS memory pointers - this must be done BEFORE issuing an 
    /* OPEN command                                                           
    */
    dw_write_csr( pb, SIFADX, INIT_C_AXBASE );
    dw_write_csr( pb, SIFADR, INIT_C_ABASE );_MB();

    pb->dw_mem.init_w_bia   = dw_read_csr( pb, SIFINC ); _MB();
    pb->dw_mem.init_w_slev  = dw_read_csr( pb, SIFINC ); _MB();
    pb->dw_mem.init_w_addrs = dw_read_csr( pb, SIFINC ); _MB();
    pb->dw_mem.init_w_param = dw_read_csr( pb, SIFINC ); _MB();
    pb->dw_mem.init_w_mac   = dw_read_csr( pb, SIFINC ); _MB();
    pb->dw_mem.init_w_llc   = dw_read_csr( pb, SIFINC ); _MB();
    pb->dw_mem.init_w_speed = dw_read_csr( pb, SIFINC ); _MB();
    pb->dw_mem.init_w_ram   = dw_read_csr( pb, SIFINC ); _MB();

    /*
    /* Get Software Level numbers. (5 shorts)
    /*
    /*    vv vv vv tt cc ss ss mm mm mm
    /*
    /*    v = vendor code (TI = 0)
    /*    t = microcode type (ebcdic C=CAF (Copy All Frames),
    /*    M=MAC (Media Access Control), L=LLC (Logical Link Control)
    /*    c = chip (ebcdic E=TMS38016, F=TMS38025)
    /*    s = Silicon Rev Level (ebcdic)
    /*    m = Microcode Rev Level (ebcdic)
    /*
    /* At the time of this writing, DETRA's string is;
    /*    00 00   00 C3   C5 F3   F1 F2   F2 F2
    /*    TI,  CAF, E, Sil Rev 3.1, ucode rev 2.22
    */
    dw_write_csr( pb, SIFADX, 1 );
    dw_write_csr( pb, SIFADR, pb->dw_mem.init_w_slev ); _MB();

    pb->dw_slevel.w_1 = dw_read_csr( pb, SIFINC ); _MB(); /* 1 */
    pb->dw_slevel.w_2 = dw_read_csr( pb, SIFINC ); _MB(); /* 2 */
    pb->dw_slevel.w_3 = dw_read_csr( pb, SIFINC ); _MB(); /* 3 */
    pb->dw_slevel.w_4 = dw_read_csr( pb, SIFINC ); _MB(); /* 4 */
    pb->dw_slevel.w_5 = dw_read_csr( pb, SIFINC ); _MB(); /* 5 */

#if DW_FYI
    print_sw_level_info( pb );
#endif

    /* enable system interrupts 
    /* ??? should this be conditioned by pb->pb.vector??? 
    */
    copy  = dw_read_csr( pb, SIFACL );
    copy |= ACL_M_SINTEN;

    dw_write_csr( pb, SIFACL, copy );  /* enable system int */
    dw_write_byte_csr( pb, SIFSTS_BYTE, 0);  /* clear pending int bit */

    /* open the adapter, insert into ring
    */
    if (dw_open_adapter( pb ))
        {
        err_printf( "%s Open Adapter Failed \n", pb->pb.name );
        return msg_failure;
        }
 
    return (MSG_INIT_PASSED);
    }


#if DW_FYI
/*+
 * ============================================================================
 * = print_sw_level_info - dump adapter sw/chip type and rev level info       =
 * ============================================================================
 *
 * OVERVIEW:
 *  Print the software and TMS380 chip type and revision/step level, for
 *  debug purposes.
 *
 * RETURNS:
 *  none
 *
 * ARGUMENTS:
 *  *pb  - pointer to device port block
 *
-*/
void print_sw_level_info( struct DW_PB *pb ) {

    unsigned int 
    	vc, 
    	uc_type, 
    	uc_rev_major, 
    	uc_rev_minor,
    	c_type, 
    	c_step_major, 
    	c_step_minor, 
    	tmp;

    char *sz_uc_type [] = { "CAF",      "MAC", "LLC", "?" };
    char *sz_c_type []  = { "TMS38016", "TMS38025",   "?" };
    
    /* vendor code 
    */
    vc = (pb->dw_slevel.w_1 << 8) | ((pb->dw_slevel.w_2 & 0xff00) >> 8);

    /* microcode type 
    */
    tmp = (pb->dw_slevel.w_2 &0x00ff);

    switch (tmp)
        {
        case ebcdic_c: /* CAF = COPY ALL FRAMES */ uc_type = 0; break;
        case ebcdic_l: /* LLC = LOGICAL LINK OONTROL */ uc_type = 2; break;
        case ebcdic_m: /* MAC = MEDIA ACCESS CONTROL */ uc_type = 1; break;
        default: uc_type = 3; break;  /* unknown */
        }

    pb->uc_type = uc_type;

    /* microcode rev 
    */
    uc_rev_major = pb->dw_slevel.w_4 & 0x000f;
    uc_rev_minor = ((pb->dw_slevel.w_5 & 0x0f00) >>4) | (pb->dw_slevel.w_5 & 0x000f);

    /* chip type 
    */
    tmp = (pb->dw_slevel.w_3 & 0xff00)>>8;

    switch (tmp)
    	{
    	/* 38016 
    	*/ 
        case ebcdic_c: 
    	    c_type = 0; break;

    	/* 38025 
    	*/ 
        case ebcdic_e: 
    	    c_type = 1; break;

    	/* unknown 
    	*/
        default: 
    	    c_type = 2;  
        }

    /* chip step level 
    */
    c_step_major = pb->dw_slevel.w_3 & 0x000f;
    c_step_minor = (pb->dw_slevel.w_4 & 0x0f00) >>8;

    err_printf( "Vendor code: %c; uCode: %s rev %x.%02x; Chip: %s step %x.%x\n",
              vc, sz_uc_type [uc_type], uc_rev_major, uc_rev_minor,
              sz_c_type [c_type], c_step_major, c_step_minor );
    }
#endif /* DW_FYI */


/*+
 * ============================================================================
 * = dw_open - Open the TMS380 device.                                        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      Open establishes a file pointer to the TokenRing Device.
 *
 * RETURNS:
 *  	msg_success - Success: station has entered the ring.
 *
 * ARGUMENTS:
 *  *fp - File to be opened.
 *
-*/
int dw_open( struct FILE *fp ) {
    struct NI_GBL *np;          
    struct DW_PB  *pb;            

    np = (struct NI_GBL*) fp->ip->misc;
    pb = (struct DW_PB*)  np->pbp;

    pb->opn_cntr++;

#if DW_TRACE
    err_printf( "dw_open %s \n", fp->ip->name );
#endif

    return (msg_success);
    }


/*+
 * ============================================================================
 * = dw_open_adapter - open command for tms380                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * Issues the OPEN command to the TMS380.  This is done as normal operating
 * procedure for startup, changing the physical address (also called name
 * changing), and for error/restart/recovery.
 *
 * INPUTS:
 *  pb - pointer to the extended port block
 *
 * OUTPUTS:
 *  Open command is issued to the TMS380
 *
 * CALLER:
 *  DEV$CHNGPRO
 *  DEV$TIMER (for a name change transition)
 *
 * USAGE OF OTHER ROUTINES:
 *
-*/
int dw_open_adapter( struct DW_PB *pb ) {
    CMD_OPEN *a_cmd;
    SCB *a_scb;
    int i ;
    USHORT copy;

#if DW_TRACE
    err_printf("  Opening %s adapter, pending opens = %d\n",
                pb->pb.name,  pb->pend_adptr_opn_cntr );
#endif

#if DW_DEBUG
    pb->pend_adptr_opn_cntr++;
#endif

    /* something relevant for the Product ID 
    */
    sprintf( pb->prod_id, "Digital -- DW300 ", 0 );

    a_cmd = pb->a_opn_cmd_buf;
 
    /* clear result so we can see open completion - interrupt handler will
    /* copy non-zero command result into this variable 
    */
    pb->opn_result     = 0;   
    pb->rcv_cmd_active = FALSE;

    /* initialize the open parameter list 
    */
    a_cmd->opn_w_options = (OPN_M_DISHARD     /* no hard error ints */
                            | OPN_M_DISSOFT   /* no soft error ints */
                            | OPN_M_FRAMEHOLD /* wait for entire frame before dma */
                            | OPN_M_ETR       /* early token release for 16 mbs */
                            | OPN_M_CONTENDER /* enable monitor contender */
                           );

    /* use adapter BIA for now 
    */
    for (i=0; i<3; i++)   
        a_cmd->opn_w_node_address [i] = 0;

    a_cmd->opn_l_group_address      = 0;     	/* no group address */
    a_cmd->opn_l_functional_address = 0;    	/* no functional address */
    a_cmd->opn_w_rcv_list_size      = bswp16(8 + (6 * PL_C_BUFFADDR));
    a_cmd->opn_w_xmt_list_size      = bswp16(8 + (6 * PL_C_BUFFADDR));
    a_cmd->opn_w_buffer_size        = bswp16( 1024 );  /* multiple of 1024 */
    a_cmd->opn_b_xmt_buf_min        = 1;
    a_cmd->opn_b_xmt_buf_max        = 8;

    a_cmd->opn_l_product_id_address = cnv32( ((ULONG)pb->prod_id | window_base) );
    
    if ((a_scb = dw_get_scb( pb )) == (SCB *) 0 )
        return msg_failure;

    a_scb->scb_w_cmd     = CMD_C_OPEN;
    a_scb->scb_w_param_0 = bswp16( HIWORD32( ((ULONG)a_cmd | window_base)) );
    a_scb->scb_w_param_1 = bswp16( LOWORD32( (ULONG)a_cmd ) );

    /* initiate the adapter open (ring insertion) 
    */
    while (dw_issue_next_cmd( pb ) == msg_failure)
        krn$_micro_delay( 100 );    /* us - waiting for adptr int flag to clear
                                     * so command can be written */

    /* Give this command some time to succeed  - note: state DW_K_STARTED is
    /* set in the CMD OPEN COMPLETE interrupt handler 
    */
    for (i=0; i < (DW_K_OPEN_TIMEOUT_SECS * 5) && (pb->opn_result == 0); i++)
        krn$_sleep( 200 );    

    if ((pb->opn_result & OPN_M_SUCCESS) == 0)
        {
#if DW_DEBUG_INIT
        err_printf("open command failed\n", 0 );
#endif
        return (msg_failure);
        }

#if DW_FYI
    dw_show_ring_status( pb );
#endif

    pb->rcv_cmd_active = FALSE;  /* interrupt handler will set this true */
    dw_issue_receive_cmd( pb );

    return (msg_success);  
    }


/*+
 * ============================================================================
 * = dw_close - Close the TMS380 device.
 * ============================================================================
 *
 * OVERVIEW:
 *      This routine closes the Eisa Port Driver, closes the adapter
 * 	and (leave ring).
 *
 * RETURNS:
 *  msg_success
 *
 * ARGUMENTS:
 *  *fp -   frx0" Port Driver File to close
 *
-*/
int dw_close (struct FILE *fp) {
    struct NI_GBL *np;          /*Pointer to NI global database*/
    struct DW_PB *pb;        /*Pointer to the port block*/
    int  i;
    SCB *a_scb;

    np = (struct NI_GBL*)fp->ip->misc;
    pb = (struct DW_PB*)np->pbp;                                     

    pb->close_cntr++;

#if DW_TRACE
    err_printf( "\ndw_close %s #%d, net = %d\n", 
             fp->ip->name, pb->close_cntr, 
             (USHORT)(pb->opn_cntr - pb->close_cntr) );
#endif

    return (msg_success);
    }


/*+
 * ============================================================================
 * = dw_setmode - Set mode                                                    =
 * ============================================================================
 *
 * OVERVIEW:
 * 	This routine is called by driver_setmode() to walk down the list 
 *	of ALL TokenRing Inodes in order to change the running state 
 *	to either DDB$K_STOP or DDB$K_START.
 *  
 * 	The Drivers Port Block saves the state of the Driver and are 
 *	as follows:
 *   		DW_K_ADPTR_UNINITIALIZED	0
 *		    The driver is set to UNINITIALIZED during dw_init_adapter()
 *   		DW_K_STOPPED	1
 *		    The driver is set to STOPPED by this routine.
 *   		DW_K_STARTED	2
 *		    The driver is set to STARTED during dw_interrupt()
 *  
 * RETURNS:
 *	msg_success or msg_failure 
 *       
 * ARGUMENTS:
 *	mode - DDB$K_STOP	   DDB$K_START
 *
 *	DDB$K_INTERRUPT         0       set to interrupt mode            
 *	DDB$K_POLLED            1       set to polled mode               
 *	DDB$K_STOP              2       set to stopped                   
 *	DDB$K_START             3       set to start                     
 *	DDB$K_ASSIGN            4       assign controller letters        
 *	DDB$K_LOOPBACK_INTERNAL 5       device internal loopback         
 *	DDB$K_LOOPBACK_EXTERNAL 6       device external loopback         
 *	DDB$K_NOLOOPBACK        7       no loopback                      
 *	DDB$K_OPEN              8       file open                        
 *	DDB$K_CLOSE             9       file close                       
 *	DDB$K_READY            10       get ready to boot                
 *	DDB$K_ETHERNET          1       Ethernet device                  
 *	DDB$K_FDDI              2       FDDI device
 *	
-*/
int dw_setmode( int mode ) {
    struct PBQ    *pbe;        /* See KERNEL_DEF.H */
    struct INODE  *ip;
    struct NI_GBL *np;
    struct DW_PB  *pb;

    /* Do the setmode for all the dw300 ports 
    */
    pbe = dw_pb_list.flink;

    while (pbe != &(dw_pb_list))
        {
        ip = (struct INODE *) pbe->pb;
        np = (struct NI_GBL *)ip->misc;
        pb = (struct DW_PB *) np->pbp;                                     

#if DW_TRACE
        err_printf( "dw_setmode: %s mode=%x\n", pb->pb.name, mode );
#endif

	switch (mode)
            {
            case DDB$K_STOP:
                dw_stop_driver( ip );
                break;
            case DDB$K_START:
                dw_restart_driver( ip );
                break;
            case DDB$K_INTERRUPT:
                pb->pb.mode = DDB$K_INTERRUPT;
                int_vector_set( pb->pb.vector, dw_interrupt, np );
    	        io_enable_interrupts (pb, pb->pb.vector);
                remq_lock( &pollq );
                break;
            case DDB$K_POLLED:
                pb->pb.mode = DDB$K_POLLED;
    	        io_disable_interrupts (pb, pb->pb.vector);
                pb->pb.vector = 0;

                /* insert our poll routing into poll queue 
    		/* use pb->pb.pq instead of pb->pqp 
    		*/
                pb->pb.pq.flink = & pb->pb.pq;
                pb->pb.pq.blink = & pb->pb.pq;
                pb->pb.pq.routine = dw_poll;
                pb->pb.pq.param = (int) np;
                insq_lock (&pb->pb.pq, &pollq);

    		break;
    	    }

        pbe = pbe->flink;	/* Get the next port*/
        }

    return (msg_success);
    }



/*+
 * ============================================================================
 * = dw_stop_driver - Stops the dw300 driver.                              =
 * ============================================================================
 *
 * OVERVIEW:
 *  This routine will stop the dw300 driver.
 *
 * RETURNS:
 *  int Status - msg_success or FAILURE.
 *
 * ARGUMENTS:
 *  *ip - Pointer to the Device's inode.
 *
-*/
int dw_stop_driver(struct INODE *ip) {
    struct NI_GBL *np;      /*Pointer to the NI global database*/
    struct DW_PB *pb;        /*Pointer to the port block*/

#if DW_TRACE
    err_printf( "dw_stop_driver %s\n", ip->name );
#endif

    np = (struct NI_GBL*) ip->misc;
    pb = (struct DW_PB*) np->pbp;

    if (pb->pb.vector)
        io_disable_interrupts (pb, pb->pb.vector);

    /* If we are already stopped don't stop again 
    */
    if (pb->state == DW_K_STOPPED)
        return (msg_success);

    /* If we are unitialized return a failure 
    */
    if (pb->state == DW_K_ADPTR_UNINITIALIZED)
        return (msg_failure);

    /* stop the adapter -- MUST LOAD ADAPTER CODE AFTER THIS 
    */
    dw_write_csr( pb, SIFACL, ACL_M_ARESET );
    krn$_micro_delay( 140 );     		

    pb->state = DW_K_ADPTR_UNINITIALIZED;

    pb->xmt_cmd_active = FALSE;

    /* Cleanup the msg rcved list 
    */
    dw_free_rcv_pkts(ip);

    return (msg_success);
    }


/*+
 * ============================================================================
 * = dw_restart_driver - restarts the driver.                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *  This routine will restart the driver.
 *
 * RETURNS:
 *  int Status - msg_success or FAILURE.
 *
 * ARGUMENTS:
 *  *ip - Pointer to the Device's inode.
 *
-*/
int dw_restart_driver(struct INODE *ip) {
    struct NI_GBL *np;      /* Pointer to the NI global database */
    struct NI_ENV *ev;      /* Pointer to the environment variables */
    struct DW_PB *pb;       /* Pointer to the port block */
    USHORT sifsts;

    unsigned int 
    	msg,	
    	retrycnt=0;

#if DW_TRACE
    err_printf( "dw_restart_driver %s \n", ip->name );
#endif

    np = (struct NI_GBL*) ip->misc;
    pb = (struct DW_PB*)  np->pbp;
    ev = (struct NI_ENV*) np->enp;

    /* If we are already started don't start again 
    */
    if (pb->state == DW_K_STARTED)
        return (msg_success);


    /* reconfig the EISA slot 
    */
    if (pb->pb.type == TYPE_EISA)
        use_eisa_ecu_data( pb );

    /* Set up the interrupt vector 
    */
    if (pb->pb.vector)
        {
        int_vector_set( pb->pb.vector, dw_interrupt, np );
        io_enable_interrupts (pb, pb->pb.vector);
        }

    dw_init_tx_plists( pb );		/* Init the transmit ring */
    dw_init_rx_plists( pb );		/* Init the receive ring */

    /* Empty the SCB_LIST FIFO 
    */
    while( dw_release_scb( pb ) == msg_success )
       ; /* do nothing */


    /* Initialize the adapter - TI recommends that you retry the TMS380        
    /* initialization up to three times from the initial adapter reset step.   
    /* BUD (built-in-diagnostics are retried up to three times internally in   
    /* the initialization procedure                                            
    */
    do

        msg = dw_init_adapter( pb );

    while ( msg == MSG_INIT_FAILED && ++retrycnt < 3 );

    if (msg != MSG_INIT_PASSED)
    	{
        err_printf("\n%s Init Adapter Error \n", pb->pb.name );
        return (msg_failure);
    	}

    return (msg_success);
    }


/*+
 * ============================================================================
 * = dw_poll - dw300 polling routine       .                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *  This routine is the polling routine for the dw300 driver.
 *  It is called from the timer task. It will simply check for
 *  an interrupt and call the isr if there is one.
 *
 * RETURNS:
 *      int msg_success     - a dummy return to keep system happy
 *
 * ARGUMENTS:
 *  *np - pointer to ni block.
 *
-*/
int dw_poll (struct NI_GBL *np) {
    struct DW_PB *pb;                /*Pointer to the port block*/
    USHORT sifsts;
    int poll_msg;

    pb = (struct DW_PB*) np->pbp;

#if DW_TRACE
    err_printf( "dw_poll %s\n", pb->pb.name );
#endif

    /* Call our isr routine is the adapter is flagging an interrupt
    */
    if (dw_read_csr( pb, SIFCMDSTS ) & STS_M_SYSINT)
        dw_interrupt( np );   

    return (msg_success);   
    }


/*+
 * ============================================================================
 * = dw_interrupt - TMS380 interrupt routine.                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *  This routine processes the TMS380 interrupts.
 *
 * RETURNS:
 *  None
 *
 * ARGUMENTS:
 *  *np - pointer to ni block.
 *
 * IMPLEMENTATION NOTES:
 *
 *  Re-enabling the adapter-to-system interrupts should be done as soon
 *  as possible after entering the interrupt handler, according to the
 *  TI manual - so we should perform a byte write to the low order byte
 *  of the SIFCMDSTS  (SIFSTS) register (little endian here, use the
 *  smaller byte address) - but this doesn't work.  The interrupts can
 *  be re-enabled by doing a WORD write to SIFCMDSTS with the low order
 *  byte = 0 -- is there a problem in the EISA outportb() routine? So we're
 *  re-enabling the adapter-to-system interrupts at the END of our 
 *  interrupt handling, at the time we notify the adapter that the SSB
 *  is clear.
-*/
void dw_interrupt( struct NI_GBL *np ) {
    struct DW_PB *pb;               /* Pointer to the port block */
    struct MOP$_V4_COUNTERS *mc;    /* Pointer to the MOP counters */
    USHORT sifsts;
    USHORT int_reason;
    USHORT w_temp;

    int pollcnt;	/* debug aid */

    pb = (struct DW_PB*) np->pbp;
    mc = (struct MOP$_V4_COUNTERS*) np->mcp;

    /* read the adapter status to get the reason for the interrupt 
    */
    sifsts     = dw_read_csr( pb, SIFCMDSTS );
    int_reason = sifsts & STS_M_CODE; 		/* get the interrupt id */

#if DW_DEBUG_INT
    pprintf("\n### INTERRUPT %04x %s ", sifsts, pb->pb.name );
#endif


    /* Handle the interrupt 
    */
    if (pb->state == DW_K_STARTED)
        {
        switch (int_reason)
            {
    	    /* Unrecoverable adapter error, 
    	    /* DMA is not available now, so use direct IO  
    	    */
            case STS_C_ADPCHK:   
    		pprintf("\n-- adapter check error\n", 0 );

                dw_write_csr( pb, SIFADX, ADAPCHK_AX ); _MB();
                dw_write_csr( pb, SIFADR, ADAPCHK_A ); _MB();

                pb->a_check_frame->w_adp_chk_sts    = dw_read_csr( pb, SIFINC );
                pb->a_check_frame->w_adp_chk_parm_0 = dw_read_csr( pb, SIFINC );
                pb->a_check_frame->w_adp_chk_parm_1 = dw_read_csr( pb, SIFINC );
                pb->a_check_frame->w_adp_chk_parm_2 = dw_read_csr( pb, SIFINC );
                pb->state = DW_K_ADPTR_UNINITIALIZED;
#if DW_DEBUG_INT
    		pprintf("\n#### dw adapter check: %04x ####\n", pb->a_check_frame->w_adp_chk_sts );
#endif            
                break;

    	    /* SSB contains Ring Status Interrupt 
    	    */
            case STS_C_RNGSTS:   
                /* If counter overflow, request read error log 
                /* count various ring status changes 
    		*/
                w_temp = pb->a_ssb->ssb_w_param_0;
#if DW_DEBUG_INT
    		pprintf("\n#### dw ring status: %04x ####\n", w_temp );
#endif            
                if (w_temp & RSTS_M_SIGLOSS)
                    pb->cntr_w_signal_loss++;     /* detected signal loss on ring */
            
                if (w_temp & RSTS_M_HARDERR)
                    pb->cntr_w_hard_err++;        /* adptr is xmtng / rcvng beacon frames */
            
                if (w_temp & RSTS_M_SOFTERR)
                    pb->cntr_w_softerr++;         /* adptr has xmtd a report error MAC frame */
            
                if (w_temp & RSTS_M_XMTBEA)
                    pb->cntr_w_xmt_beacon++;      /* adptr is xmtng beacon frames to ring */
            
                if (w_temp & RSTS_M_LOBE)
                    pb->cntr_w_lobe++;            /* open or short wiring fault between adptr and concentrator */

                if (w_temp & RSTS_M_ARERR)      /* auto removal error */
                    pb->cntr_w_autoremoval_err++; /* failed lobe wrap test resulting from beacon auto-removal process */

                if (w_temp & RSTS_M_RRECV)      /* rcvd MAC frame requesting ring station removal */
                    pb->cntr_w_remove_stn++;

                if (w_temp & RSTS_M_CNTOVR)     /* an adapter internal error counter has overflowed (254 to 255) */
                    dw_read_error_log( pb );
                
                if (w_temp & RSTS_M_SS)         /* adaptr has sensed it is the only station on the ring */            
                    pb->cntr_w_single_station = 1;
                else
                    pb->cntr_w_single_station = 0;
            
                if (w_temp & RSTS_M_RR)         /* adptr has observed claim token MAC frames on ring */
                    pb->cntr_w_ring_recovery = 1;
                else
                    pb->cntr_w_ring_recovery = 0; /* ring purge frame has been rcvd / xmtd */

                break;

    	    /* SCB has been copied to adapter 
    	    */
            case STS_C_SCBCLR:                  
#if DW_DEBUG_INT_x
    		pprintf("\n#### dw scb clear ####\n", 0);
#endif            
    	        /* copy the next command, in any, 
    		/* to the scb and signal the adapter 
    		*/
                dw_issue_next_cmd( pb );     

                break;



    	    /* Command is complete in SSB 
    	    */
            case STS_C_CMD:       
                switch (pb->a_ssb->ssb_w_cmd)
    	            {
    		    /* Ring Status changed 
                    /* this case should be handled above by int_reason 
    		    */
                    case CMD_C_RNGSTS:        
#if DW_DEBUG_INT 
    			pprintf("\n#### Unexpected CMD_C_RNGSTS interrupt ####\n", 0);
#endif
                        break;

    		    /* Command Reject 
    		    */
                    case CMD_C_CMDREJ:        
#if DW_DEBUG_INT
    			pprintf("\n#### dw cmd reject: P1 %04x P0 %04x ####\n", 
        			bswp16(pb->a_ssb->ssb_w_param_1), bswp16(pb->a_ssb->ssb_w_param_0));
#endif            
                        dw_handle_cmd_reject( pb, pb->a_ssb->ssb_w_param_1, pb->a_ssb->ssb_w_param_0);

                        break;

    		    /* Open 
    		    */
                    case CMD_C_OPEN:          
#if DW_DEBUG_INT
    			pprintf("\n#### dw cmd open: P0 %04x ####\n", bswp16(pb->a_ssb->ssb_w_param_0));
#endif            
    			if ((pb->opn_result = pb->a_ssb->ssb_w_param_0) & OPN_M_SUCCESS)
    			    pb->state = DW_K_STARTED;
                       
                        break;
                    
    		    /* Close 
    		    */
                    case CMD_C_CLOSE:         
                        pb->close_result = pb->a_ssb->ssb_w_param_0;
#if DW_DEBUG_INT
    			pprintf("\n#### dw cmd close: P0 %04x ####\n", 
    		           bswp16(pb->a_ssb->ssb_w_param_0));
#endif            
                        break;
                    
    		    /* Transmit 
    		    */
                    case CMD_C_XMIT:          
                        /* this case should be handled above by int_reason */
#if DW_DEBUG_INT  
    			pprintf("\n#### Unexpected CMD_C_XMIT interrupt ####\n", 0);
#endif
                        break;
                    
    		    /* Receive 
    		    */
                    case CMD_C_RECV:          
    	                /* this case should be handled above by int_reason */
#if DW_DEBUG_INT 
    			pprintf("\n#### Unexpected CMD_C_RECV interrupt ####\n", 0);
#endif
                        break;

    		    /* Read Error Log 
    		    */
                    case CMD_C_RDERR:         
                        pb->errlog_result = pb->a_ssb->ssb_w_param_0;

                        break;

#if FALSE
    		    /* These commands are not implemented here 
    		    */
                    case CMD_C_XMITHALT:      /* Transmit Halt */
                    case CMD_C_GROUP:         /* Set group address (last 32 bits) */       
                    case CMD_C_FUNC:          /* Set Functional address */
                    case CMD_C_RDADP:         /* Read Adapter */
                    case CMD_C_MDOPEN:        /* Modify Open parameters */
                    case CMD_C_RSOPEN:        /* Restore Open parameters */
                    case CMD_C_F16GRP:        /* Set group address (first 16 bits) */
                    case CMD_C_BRIGDE:        /* Set Bridge params */
                    case CMD_C_CBRIDGE:       /* Configure Bridge */
#endif
                    default:
                        break;    
    	            }

                break;



    	    /* RCV frame is complete in SSB 
    	    */
            case STS_C_RCV:       
                (pb->cntr_w_rcv_int)++;    /* Count it */
            
    	        /* save the addr of the plist for the last (most recent) 
    		/* msg received -- a rcv interrupt may signal receipt of 
    		/* more than one frame 
    		*/    
                pb->a_last_rcvplist = 
                     (PARAM_LIST *) ((bswp16(pb->a_ssb->ssb_w_param_1)<<16) + 
                                      bswp16(pb->a_ssb->ssb_w_param_2));

                /*  This is done because the Adapter returns in ssb_w_param_1, ssb_w_param_2, the address
                /*  of the XMT_PARM_LIST.  For Pci adapters, the window_base is or'ed in and must be
                /*  or'ed out.
                */
    		(ULONG)pb->a_last_rcvplist &= ~window_base;
            
#if DW_DEBUG_INT
    		w_temp = bswp16( pb->a_last_rcvplist->pl_w_size );

    		pprintf("\n#### rcv pkt: status = %04x, plist @%08x, buf @%08x, pkt size = %d ####\n", 
    			pb->a_ssb->ssb_w_param_0,
    			pb->a_last_rcvplist, 
    			pb->a_last_rcvplist->pl_a_buf, 
    			w_temp );

    		/* limit the print out 
    		*/
    		if (w_temp > 80)
    	             w_temp = 81;

#endif            
    	        /* Activate the rcv process 
    		*/
                krn$_bpost( &(pb->dw_isr_rx_sem) );

                break;

    	    /* XMT is complete in SSB 
    	    */
            case STS_C_XMT:       
                (pb->cntr_w_xmt_int)++;		/* Count it */

    	        /* Pick up the xmt plist, and save the xmt status in it 
    		*/
                pb->a_next_status_xmtplist = 
    		    (PARAM_LIST *) ((bswp16(pb->a_ssb->ssb_w_param_1)<<16) + 
                          bswp16(pb->a_ssb->ssb_w_param_2));


                /*  This is done because the Adapter returns in ssb_w_param_1, ssb_w_param_2, the address
                /*  of the XMT_PARM_LIST.  For Pci adapters, the window_base is or'ed in and must be
                /*  or'ed out.
                */
    		(ULONG)pb->a_next_status_xmtplist &= ~window_base;

                /* status bit masks are already byte swapped to save the 
    		/* runtime byte swap 
    		*/
                pb->a_next_status_xmtplist->ssb_w_status = pb->a_ssb->ssb_w_param_0; 
            
#if DW_DEBUG_INT
    		pprintf("\n#### xmt pkt: %s status = %04x, plist @%08x ####\n", 
    			pb->pb.name,
    			pb->a_ssb->ssb_w_param_0,
    			pb->a_next_status_xmtplist );
#endif            
                /* XMT command completed - need a new one to send more 
    		*/
                if (pb->a_ssb->ssb_w_param_0 & TS_M_COMPLETE)
                    pb->xmt_cmd_active = FALSE;

    	        /* Activate the xmt process 
    		*/
                krn$_bpost( &(pb->dw_isr_tx_sem) );

                break;

            default:
                break;
            }
        }

    else /* if (pb->state == DW_K_ADPTR_INITIALIZED) */
        {
        switch (int_reason)
            {
#if FALSE
            case STS_C_ADPCHK:
    		pprintf("-- adapter check error\n", 0 );

                dw_write_csr( pb, SIFADX, ADAPCHK_AX ); _MB();
                dw_write_csr( pb, SIFADR, ADAPCHK_A ); _MB();

                pb->a_check_frame->w_adp_chk_sts = dw_read_csr( pb, SIFINC );
                pb->a_check_frame->w_adp_chk_parm_0 = dw_read_csr( pb, SIFINC );
                pb->a_check_frame->w_adp_chk_parm_1 = dw_read_csr( pb, SIFINC );
                pb->a_check_frame->w_adp_chk_parm_2 = dw_read_csr( pb, SIFINC );

                pb->state = DW_K_ADPTR_UNINITIALIZED;
#endif
#if DW_DEBUG_INT
    		pprintf("\n$$$$ dw adapter check (while not started): %04x ####\n", 
    	            pb->a_check_frame->w_adp_chk_sts );
#endif            
                break;

    	    /* Command is complete in SSB 
    	    */
            case STS_C_CMD:       
                switch (pb->a_ssb->ssb_w_cmd )
    	            {
    	            case CMD_C_OPEN:
#if DW_DEBUG_INT
    			pprintf("\n$$$$ dw cmd open: P0 %04x ####\n", 
    			    bswp16(pb->a_ssb->ssb_w_param_0));
#endif            
    			if ((pb->opn_result = pb->a_ssb->ssb_w_param_0) & OPN_M_SUCCESS)
                            pb->state = DW_K_STARTED;

                        break;
            
    	            default:
    	                break;
    	            }

                break;

            default:
                break;

            }    /* end switch */

        }    /* end if-then-else


    /* Enable interrupts again 
    */
    if (pb->pb.vector)
        io_issue_eoi( pb, pb->pb.vector );

    /* Wait for the adapter to be ready to receive an interrupt 
    */
    for ( pollcnt = 10000; pollcnt && sifsts & STS_M_INTADP; pollcnt--) 
        {
    	sifsts = dw_read_csr( pb, SIFCMDSTS );
    	krn$_micro_delay( 100 );   /* don't lock out everything else! */
        } 

#if DW_DEBUG_INT
    if (!pollcnt)
        pprintf("dw_int: adapter is not responding\n", 0); 
#endif

    /* Tell the adapter that the SSB is available again, & reset sysint 
    */     
    dw_write_csr( pb, SIFCMDSTS, (CMD_M_INTADP|CMD_M_SSBCLR) );
    }


/*+
 * ============================================================================
 * = dw_handle_cmd_reject - . =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * RETURNS:
 *
 * ARGUMENTS:
 *
-*/
void dw_handle_cmd_reject( struct DW_PB *pb, USHORT cmd_id, USHORT cmd_status ) {
#if DW_DEBUG_INT
    pprintf("Unexpected CMD_C_REJECT: id=%04x, sts=%04x\n", 
    	bswp16(cmd_id), bswp16(cmd_status) );
#endif
    switch (cmd_id)
        {
        case CMD_C_OPEN:          /* Open */
            pb->opn_result = cmd_status;
            break;

        case CMD_C_CLOSE:         /* Close */
            pb->close_result = cmd_status;
            break;

        case CMD_C_XMIT:          /* Transmit */
            pb->xmt_result = cmd_status;
            break;

        case CMD_C_RECV:          /* Receive */
            pb->rcv_result = cmd_status;
            break;

        case CMD_C_RDERR:         /* Read Error Log */
            pb->errlog_result = cmd_status;
            break;
#if FALSE
        /* These commands are not implemented here 
    	*/
        case CMD_C_XMITHALT:      /* Transmit Halt */
        case CMD_C_GROUP:         /* Set group address (last 32 bits) */       
        case CMD_C_FUNC:          /* Set Functional address */
        case CMD_C_RDADP:         /* Read Adapter */
        case CMD_C_MDOPEN:        /* Modify Open parameters */
        case CMD_C_RSOPEN:        /* Restore Open parameters */
        case CMD_C_F16GRP:        /* Set group address (first 16 bits) */
        case CMD_C_BRIGDE:        /* Set Bridge params */
        case CMD_C_CBRIDGE:       /* Configure Bridge */
#endif
        default:
            break;
        }
    }                    


#if DW_FYI
/*+
 * ============================================================================
 * = dw_show_ring_status - . =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * RETURNS:
 *
 * ARGUMENTS:
 *
-*/
void dw_show_ring_status( struct DW_PB *pb ) {
    err_printf("Ring status: SigLoss=%04d,  HardErr=%04d, SoftErr=%04d\n",
                pb->cntr_w_signal_loss,
                pb->cntr_w_hard_err,
                pb->cntr_w_softerr );
    err_printf("             XmtBeac=%04d,  LobeFlt=%04d, AutoRem=%04d\n",
                pb->cntr_w_xmt_beacon,
                pb->cntr_w_lobe,
                pb->cntr_w_autoremoval_err );
    err_printf("             RemvStn=%04d,  SnglStn=%04d, RingRec=%04d\n",
                pb->cntr_w_remove_stn,
                pb->cntr_w_single_station,
                pb->cntr_w_ring_recovery );
    }
#endif


/* ============================================================================
 *                  R E C E I V E    P R O C E S S I N G
 * ============================================================================
 */
 
/*+
 * ============================================================================
 * = dw_init_rx_plists - Initialization routine for the receive ring.         =
 * ============================================================================
 *
 * OVERVIEW:
 *  This routine will init the receive ring.
 *
 * RETURNS:
 *  int Status - msg_success or FAILURE.
 *
 * ARGUMENTS:
 *  *pb - ptr to extended port block
 *
-*/
int dw_init_rx_plists( struct DW_PB *pb ) {
    PARAM_LIST *plist;  /* parameter list address */
    char *buf; 		/* Buffer address -- USE (char *) to force byte pointer addressing */
    int i;

#if DW_TRACE
    err_printf( "dw_init_rx_plists %s \n", pb->pb.name );
#endif

    /* Go through and init the rx ring
    */
    for (i=0, buf = (char *)pb->a_rcvbuf, plist = pb->a_rcvplist_ring; 
             i < pb->rcv_buf_no;
                 i++, buf += DW_K_PKT_ALLOC, plist++)
        {
        /* save the address of the next plist, and a byte swapped copy 
    	*/
        plist->pl_a_next = plist + 1;

        plist->pl_l_next = cnv32( ((ULONG) plist->pl_a_next | window_base ));

        /* save the address of the translation buffer, and a byte swapped copy 
    	/* BOTH POINT TO THE SAME pb->a_xmtbuf ADDRESS...
    	*/
        plist->pl_a_buf             = (TOKEN_RING_HEADER *)buf;
        plist->pl_buf[0].pl_l_addr  = cnv32( ((ULONG) buf | window_base ));
        plist->pl_buf[0].pl_w_count = bswp16( DW_K_PKT_ALLOC ); /* max size buf */

        /* zero out the status, setting adapter ownership, interrupt on frame  	
    	*/
        plist->pl_w_cstat = RC_M_INTER | RC_M_VALID;

        /* zero out the size field 
    	*/
        plist->pl_w_size = 0;
        }

    /* close the ring 
    */
    --plist; 				    /* backup to last plist */
    plist->pl_a_next = pb->a_rcvplist_ring;

    plist->pl_l_next = cnv32( ((ULONG) plist->pl_a_next | window_base ) );

    /* init the FIFO pointers 
    */
    pb->a_next_rcvplist = pb->a_last_rcvplist = pb->a_rcvplist_ring;

#if DW_DEBUG_INIT
    err_printf("\nrcv_plists: (buffer size: %d (0x%04x) %d plists\n", 
              DW_K_PKT_ALLOC, DW_K_PKT_ALLOC, pb->rcv_buf_no);
    show_plists( pb, pb->a_rcvplist_ring, pb->rcv_buf_no );
#endif

    return (msg_success);
    }



/*+
 * ============================================================================
 * = dw_read - dw300 port read routine.                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      Used primarily for dw300 port callbacks.
 *
 * RETURNS:
 *  number of bytes read
 *      0 - If EOF or error (fp->status = failure message)
 *                          (fp->code_entry = boot start address)
 *
 * ARGUMENTS:
 *  *fp     - Pointer to a file.
 *  size    - Used to test for buffer size.
 *  number  - Used to test for buffer size.
 *  *c      - For MOP load the base of where this is to be loaded.
-*/
int dw_read( struct FILE *fp, int size, int number, UBYTE *c ) {
    struct NI_GBL *np;          /*Pointer to NI global database*/
    struct DW_PB *pb;        /*Pointer to the port block*/
    struct DW_MSG *mr;       /*Pointer to the message recieved*/
    int 
    	len, 
    	retries;

    np = (struct NI_GBL*) fp->ip->misc;
    pb = (struct DW_PB*) np->pbp;

#if DW_TRACE
    err_printf( "\ndw_read: %s size: %d number: %d\n", pb->pb.name, size, number );
#endif

    /* See if there is a message available 
    */
    for (retries=0; retries < DW_CALLBACK_TIMEOUT; retries++)
        {
        spinlock( &spl_kernel );

        if (pb->rcv_msg_qh.flink != (struct DW_MSG*) &(pb->rcv_msg_qh.flink))
            {
            /* Get the message 
    	    */
            mr = (struct DW_MSG*) remq( pb->rcv_msg_qh.flink );
            pb->rcv_msg_cnt--;
            spinunlock( &spl_kernel );

            break;
            }
        else
            spinunlock( &spl_kernel );	/* There isn't so just unlock */

        krn$_sleep( DW_CALLBACK_WAIT );
        }

    /* If there is no message available return 0 
    */
    if (retries == DW_CALLBACK_TIMEOUT)
        return (0);

    /* Copy the info 
    */
    len = min( mr->size, size * number );
    memcpy( c, mr->msg, len );

    /* Free the holder and packet 
    */
    ndl_deallocate_pkt( fp->ip, mr->msg );
    free( mr );

#if DW_DEBUG
    err_printf( "\ndw_read: Callback message received.\n", 0 );
    pprint_pkt( c, len );
#endif

    return (len);
    }


/*+
 * ============================================================================
 * = dw_issue_receive_cmd - start receiving messages                          =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *   Command the adapter to begin receiving messages.
 *
 * RETURNS:
 *   msg_success if cmd queued/sent else msg_failure (scb not available)
 *
 * ARGUMENTS:
 *  *pb - ptr to device port block
 *
-*/
int dw_issue_receive_cmd( struct DW_PB *pb ) {
    SCB *a_scb;

#if DW_TRACE
    err_printf( "\ndw_issue_receive_cmd %s \n", pb->pb.name );
#endif    

    if ((a_scb = dw_get_scb( pb )) == (SCB *) 0 )
        return msg_failure;   /* no room in cmd FIFO */

    a_scb->scb_w_cmd     =  CMD_C_RECV;
    a_scb->scb_w_param_0 = bswp16( HIWORD32( ((ULONG)pb->a_rcvplist_ring | window_base)) );
    a_scb->scb_w_param_1 = bswp16( LOWORD32(  (ULONG)pb->a_rcvplist_ring ) );
    
    dw_issue_next_cmd( pb );

    return msg_success;
    }


/*+
 * ============================================================================
 * = dw_rx_proc - Receive process.                                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *  This routine is actually a process that is created by dw_init_driver.
 *  It processes receive descriptors. Its main purpose is to keep
 *  going through the descriptors and processing them.
 *
 * RETURNS:
 *  int Status - msg_success or FAILURE.
 *
 * ARGUMENTS:
 *  *ip - Pointer to the Device's inode.
 *
-*/
int dw_rx_proc( struct INODE *ip ) {
    struct FILE *fp;            /* Pointer to the file being used */
    struct NI_GBL *np;          /* Pointer to the NI global database */
    struct DW_PB *pb;           /* Pointer to the port block */
    struct MOP$_V4_COUNTERS *mc;/* Pointer to the MOP counters */
    struct NI_ENV *ev;          /* Pointer to the environment variables */
    ETHERNET_FRAME *enetf;
    int size;

    /* Get a file pointer for this device 
    /* This file stays open forever 
    */
    fp = (struct FILE *) fopen( ip->name, "r+" );

    if (!fp)
        return (msg_failure);

    np = (struct NI_GBL*) ip->misc;
    pb = (struct DW_PB*) np->pbp;
    mc = (struct MOP$_V4_COUNTERS*) np->mcp;
    ev = (struct NI_ENV*) np->enp;

    enetf = (ETHERNET_FRAME *) 0;  /* no pkt buffer allocated */

    /* DO FOREVER 
    */
    while (TRUE)    
        {
        /* process any available receive plists */
#if TRUE
        /* while we own the starting plist 
    	*/
        while (!(pb->a_next_rcvplist->pl_w_cstat & RC_M_VALID))
#else
        while (pb->a_next_rcvplist->pl_w_cstat & (RC_M_VALID | RC_M_COMPLETE)
                   == RC_M_COMPLETE )  /* _VALID cleared and _COMPLETE */
#endif
            {
            /* Get a packet buffer to receive the frame 
    	    */
            if (enetf == (ETHERNET_FRAME *) 0)
                /* we need another pkt buffer 
    		*/
                enetf = (ETHERNET_FRAME *) ndl_allocate_pkt( ip, 1 );

    	        /* translate the token ring frame to 
    		/* ethernet & reuse plist(s) 
    		*/
    		size = dw_process_rx_plist( pb, ev, mc, pb->a_next_rcvplist, enetf );

    	        /* pass msg to read callback, or enqueue it for dw_read 
    		*/
                if (size > 0)
    	            {
                    dw_msg_rcvd( fp, pb, mc, enetf, size );
                    enetf = (ETHERNET_FRAME *) 0;  /* we used the current one */
    	            }
    	        }

        /* wait for next rcv interrupt 
    	*/
        krn$_wait( &(pb->dw_isr_rx_sem) );

#if DW_TXRX 
    	err_printf("\n-- waking up proc_rx: %s @ next_rcvplist = %08x, cstat = %04x, last_rcvplist %08x\n",  
    		pb->pb.name, 
    		pb->a_next_rcvplist,  
    		bswp16(pb->a_next_rcvplist->pl_w_cstat),  
    		pb->a_last_rcvplist );
#endif
        }    /* end while true */

    return msg_success;   
    }



/*+
 * ============================================================================
 * = dw_msg_rcvd - Handle a received message.                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *  Depending if CALLBACKS are Enabled, this routine will EITHER:
 *      - Give the Received Packet to the MOP/NI_DATALINK Protocols 
 *        by calling the class driver receive routine  - or -
 *      - Give it to the Callback via the pdq->rqh queue.  fr_read()
 *        will remove the Packets from pdq->rqh as called by the OS.
 *
 * RETURNS:
 *  None
 *
 * ARGUMENTS:
 *  *fp   - Pointer to the receive process port file.
 *  *pb   - Pointer to the port block.
 *  *mc   - Pointer to the counters.
 *  *msg  - Pointer to the ndl_message received.
 *  size  -
 *
-*/
void dw_msg_rcvd( struct FILE *fp, 
    		  struct DW_PB *pb, 
    		  struct MOP$_V4_COUNTERS *mc, 
    		  ETHERNET_FRAME *msg, 
    		  USHORT size ) {

    struct DW_MSG *mh;               /* Recieved message holder */
    int w_temp;

#if DW_TRACE
    err_printf( "\ndw_msg_rcvd: %s @%08x, size: %d\n", pb->pb.name, msg, size );
#endif

#if DW_DEBUG_CONVERT
    print_pkt( msg, size );
#endif

#if DW_DEBUG_PROTOCOL
    /* LIMIT PRINT TO 60 BYTES MAX 
    */
    w_temp = size;

    if (w_temp > 60)
    	w_temp = 60;

    print_pkt( msg, w_temp );
#endif

    /* If callbacks are ENABLED:
     *  dw_read() is called via the OS and will return these raw
     *  network Packets to the caller
     *
     * If callbacks are DISABLED:
     *  We'll return these network Packets to the MOP/NI_DATALINK Class
     *  drivers, ie via the ni_datalink.c routines.
     */
    if (cb_ref)  /* callbacks are ENABLED */
        {
        /*   Purge the Ports Receive Msg queue, if the Host fails to read them
         *   before the queue fills. 
    	*/
        if (pb->rcv_msg_cnt > DW_MAX_RCV_MSGS)
            dw_free_rcv_pkts( fp->ip );

        /* Get a message holder, and fill it 
    	*/
        mh       = (struct DW_MSG*) malloc( sizeof(struct DW_MSG) );
        mh->msg  = (UBYTE *) msg;
        mh->size = size;

        /* Insert all received packets into the callBack Receive_Msg_queue.
         * This queue is primarily used for callback reads.  See dw_read().
         * These received packets are removed from this queue by either
         * fr_read() or fr_free_rcv_pkts(). 
    	*/

        spinlock( &spl_kernel );
        insq( mh, pb->rcv_msg_qh.blink );
        pb->rcv_msg_cnt++;
        spinunlock( &spl_kernel );

        return;
        }
        
    /* CALLBACKS ARE DISABLED.
     *  SO WE MUST GIVE THIS RECEIVED PACKET TO THE UPPER LEVEL
     *  MOP/NI_DATALINK PROTOCOLS AND NOT INTO pdq->rqh. 
    */
        
    /* Give the upper level protocols this packet by
     * calling the Class Drivers "Receive Message Routine".
     *      pb->lrp == *rp = (int) ndl_receive_msg 
    */

    if (pb->lrp)
        (*(pb->lrp))( fp, (char *) msg, size );    /* MAGIC.... CALL class driver */
    else
        {
        /* There is no receive routine: bump the count, 
    	/* dump the msg and continue 
    	*/
        _addq_v_u(( mc->MOP_V4CNT_UNKNOWN_DESTINATION.l), 1 );
        ndl_deallocate_pkt( fp->ip,msg );
        }
    }



/*+
 * ============================================================================
 * = dw_process_rx_plist - Process RX plists and packets.                     =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *  This routine will process receive packets and descriptors. Processing
 *  includes TMS380 and descriptor error checking. If the TMS380 still owns
 *  the descriptor we will just return wait for the receive interrupt.
 *
 * RETURNS:
 *  int size of received packet, or 0 if frame dropped, or error  
 *
 * ARGUMENTS:
 *  *pb    - Pointer to the es port block.
 *  *ev    - pointer to NI environment
 *  *mc    - pointer to MOP counters
 *  *plist - received frame parameter list
 *  *enetf - ptr to pkt buffer to receive translated frame
 *
-*/
int dw_process_rx_plist( struct DW_PB *pb, struct NI_ENV *ev, 
                         struct MOP$_V4_COUNTERS *mc, 
                         PARAM_LIST *plist, ETHERNET_FRAME *enetf ) {

    int enet_size;   /* enet packet size */
    SA  *da;         /* station address */
    int i;
    unsigned int w_size, w_temp, ri_len;

    IEEE_LLC_HEADER *trf_llc;

#if DW_TRACE
    err_printf("\ndw_process_rx_plist: %s @%0x8\n", pb->pb.name, plist );
#endif

    if (plist->pl_w_cstat & RC_M_START)
        {
        if (plist->pl_w_cstat & RC_M_END)  /* we have the whole frame in plist */
            {
            /* be certain that pl_w_size is valid before processing plist 
    	    */
            while (plist->pl_w_size == 0)
               krn$_micro_delay( 500 );     /* us */

            w_size = bswp16( plist->pl_w_size );
#if DW_TXRX 
    	    pprintf(" --- rcv pkt: cstat = %04x, plist @%08x, buf @%08x, pkt size = %d ####\n", 
                      bswp16( plist->pl_w_cstat ),
                      plist, plist->pl_a_buf, w_size );
#if FALSE
    	    /* show the whole pkt 
    	    */
    	    pprint_pkt( pb->a_next_rcvplist->pl_a_buf, w_size );
#endif
#endif            

#if DW_DEBUG_CONVERT
    	    /* show the whole pkt 
    	    */
    	    pprintf("\ndw_process_rx_plist: %s\n", pb->pb.name);
    	    pprint_pkt( pb->a_next_rcvplist->pl_a_buf, w_size );
#endif

    	    /* Check for our own source address - this occurs in reception of
            /* our own broadcast msg - DISCARD IT 
    	    */
            if (dw_match_addr( &(plist->pl_a_buf->source_addr), &(pb->sa) ))
                {
                /* release the plist and point to the next one 
    		*/
                pb->a_next_rcvplist = dw_release_rcv_plist( pb, plist );

                return 0;
                }

            /* check for broadcast msg -- same addr in token ring and ethernet 
    	    */
            if ((pb->driver_flags & NDL$M_BROADCAST) == NDL$M_DIS_BROADCAST)
                {
                da = (SA *) plist->pl_a_buf->dest_addr;

                if (dw_match_addr( da, (SA *) broadcast_addr ) || 
    		    dw_match_addr( da, (SA *) broadcast_addr2 ))
                    {
                    /* release the plist and point to the next one 
    		    */
                    pb->a_next_rcvplist = dw_release_rcv_plist( pb, plist );

                    return 0;
                    }
                }

            /* translate tr frame into enet buffer 
    	    */
            enet_size = convert_tokenring_to_enet( pb, enetf, plist->pl_a_buf,
                                       (int) w_size );

            /* update some MOP counters 
    	    */
#if FALSE
            /* trust receive addr filter: check for ethernet multi-cast addr 
    	    */
            if (enetf->dest_addr [0] & 1)
#else
            /* don't trust receive addr filter: check for tr functional addr 
    	    */
            if (((*da) [0] & 0xc0) == 0xc0)
#endif
                {
                /* it's a multi-cast address (or token ring functional addr) 
    		*/
                _addq_v_u( (mc->MOP_V4CNT_RX_MCAST_BYTES.l), enet_size );
                _addq_v_u( (mc->MOP_V4CNT_RX_MCAST_FRAMES.l), 1 );
                }
            else
                {
                /* it's a physical address 
    		*/
                _addq_v_u( (mc->MOP_V4CNT_RX_BYTES.l), enet_size );
                _addq_v_u( (mc->MOP_V4CNT_RX_FRAMES.l), 1 );
                }

            /* reuse this plist, and point to the next one 
    	    */
            pb->a_next_rcvplist = dw_release_rcv_plist( pb, plist );

            return (enet_size);    /* success or failuer */
            }
        else
            {
#if DW_TXRX 
    	   err_printf("\ndw_process_rx_plist: %s Unexpected multi-seg frame: size %d\n",
    	       pb->pb.name,
               bswp16( plist->pl_w_size ));
#endif             
            /* release plists up to the next one with RC_M_END 
    	    */
            for (i=0; i<pb->rcv_buf_no && !(plist->pl_w_cstat & RC_M_END); i++)
                {
                /* release the plist and point to the next one 
    		*/
                plist = pb->a_next_rcvplist = dw_release_rcv_plist( pb, plist );
                }

            /* release the one with RC_M_END set, too, and point to the next 
    	    */
    	    plist = pb->a_next_rcvplist = dw_release_rcv_plist( pb, plist );
    
            return 0;   /* indicate failure */
            }
        }
    else
        {
#if DW_DEBUG
        err_printf( "\ndw_process_rx_plist: %s no START of frame\n", pb->pb.name );

        err_printf("\n -- plist @%08x, cstat = %04x, len = %d, buf @%0x8\n",
           plist, bswp16( plist->pl_w_cstat ), bswp16( plist->pl_w_size ), 
           plist->pl_a_buf );
#endif
        /* release the plist and point to the next one 
    	*/
        plist = pb->a_next_rcvplist = dw_release_rcv_plist( pb, plist );

        return 0;   /* indicate failure */
        }
    }


/*+
 * ============================================================================
 * = dw_release_rcv_plist - clean up rcv plist and give it back to adapter    =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *  This routine resets the control fields of the receive parameter list
 *  to the values expected for an unused plist.
 *
 * RETURNS:
 *  address of next parameter list in the ring
 *
 * ARGUMENTS:
 * *pb    - ptr to extended device port block
 * *plist - ptr to rcv parameter list to clean and return to
 *                      adapter
 *
 * IMPLEMENTATION NOTES:
 *
 *  The plist pointers are unchanged (each plist has an associated 
 *  translation buffer, and is part of a ring of plists.  The pl_w_size
 *  field must be set to zero so the received frame routine can detect 
 *  a valid size in the field. (The driver may acquire plist ownership before
 *  the size field has been updated.)
 *
-*/
PARAM_LIST *dw_release_rcv_plist( struct DW_PB *pb, PARAM_LIST *plist ) {

    /* the only processing needed here is to reset the plist flags and
    /* parameters to their default (free) values.  The adapter fills
    /* and returns rcv plists for processing and manages its own ptrs 
    */

#if DW_TRACE
    err_printf("\n release_rcv_plist %s \n", pb->pb.name );
#endif     

    /* clear VALID, COMPLETE flags - keep "interrupt on frame completion" 
    */
    spinlock( &(pb->spl_port) );

    plist->pl_w_cstat            = RC_M_INTER | RC_M_VALID;
    plist->pl_w_size             = 0;   		/* so we can detect valid frame size field */
    plist->pl_buf [0].pl_w_count = bswp16( DW_K_RCV_BUF_SIZ );
    plist->ssb_w_status          = 0;

    spinunlock( &(pb->spl_port) );

    /* tell the adapter we've returned the plist 
    */
    dw_write_csr( pb, SIFCMDSTS, (CMD_M_INTADP|CMD_M_RCVVAL|CMD_M_SYSINT) );

    return plist->pl_a_next;
    }



/*+
 * ============================================================================
 * = dw_free_rcv_pkts - Frees packets from the recieve list.               =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *  This routine will free the packets and memory used for
 *  the received packet list.
 *
 * RETURNS:
 *  None
 *
 * ARGUMENTS:
 *  *ip - Inode of the current port.
 *
-*/
void dw_free_rcv_pkts( struct INODE *ip ) {
    struct NI_GBL *np;      /*Pointer to NI global database*/
    struct DW_PB *pb;       /*Pointer to the port block*/
    struct DW_MSG *mr;      /*Pointer to the message recieved*/

    np = (struct NI_GBL*) ip->misc;
    pb = (struct DW_PB*) np->pbp;

#if DW_TRACE
    err_printf( "dw_free_rcv_pkts %s\n", pb->pb.name );
#endif

    /* Free all the messages on the preallocated list
    */
    spinlock( &spl_kernel );

    while ( pb->rcv_msg_qh.flink != (struct DW_MSG*) &(pb->rcv_msg_qh.flink) )
        {
        /*Remove the packet
    	*/
        mr = (struct DW_MSG*) remq( pb->rcv_msg_qh.flink );
        pb->rcv_msg_cnt--;
        spinunlock( &spl_kernel );

        /* Free the packet and memory
    	*/
        ndl_deallocate_pkt( ip, mr->msg );
        free( mr );

        /*Synchronize next access
    	*/
        spinlock( &spl_kernel );
        }

    spinunlock( &spl_kernel );
    }


/* ============================================================================
 *                  T R A N S M I T    P R O C E S S I N G
 * ============================================================================
 */
 
/*+
 * ============================================================================
 * = dw_init_tx_plists - Initialization routine for the transmit ring.        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *  This routine will init the initialization the transmit ring.
 *
 * RETURNS:
 *  int Status - msg_success or FAILURE.
 *
 * ARGUMENTS:
 *   *pb - ptr to device's extended port block.
 *
-*/
int dw_init_tx_plists( struct DW_PB *pb ) {
    PARAM_LIST *plist;      /* parameter list address */
    char *buf;              /* Buffer address*/
    int   i;

#if DW_TRACE
    err_printf( "\ndw_init_tx_plists %s \n", pb->pb.name );
#endif

    /* Go through and init the tx ring 	
    */
    for (i=0, buf = (char *)pb->a_xmtbuf, plist = pb->a_xmtplist_ring; 
             i < pb->xmt_buf_no;
                i++, buf += DW_K_PKT_ALLOC, plist++)
        {
        /* save the address of the next plist, and a byte swapped copy 
    	*/
        plist->pl_a_next = plist + 1;

    	plist->pl_l_next = cnv32( ((ULONG)plist->pl_a_next | window_base ) );

        /* #define cnv32(l)   ( (bswp16(l)<<16) | (bswp16((l)>>16)) )
        /* #define bswp16(w)  ( ((w&0xff)<<8) | ((w>>8)&0xff) )
        */
        /* save the address of the translation buffer, and a byte swapped copy 
    	/* BOTH POINT TO THE SAME pb->a_xmtbuf ADDRESS...
    	*/
        plist->pl_a_buf              = (TOKEN_RING_HEADER *) buf;
        plist->pl_buf [0].pl_l_addr  = cnv32( ((ULONG) buf | window_base ));
        plist->pl_buf [0].pl_w_count = 0;   		/* no data in it */

        /* zero out the status, clearing the TC_M_VALID bit 
    	*/
        plist->pl_w_cstat = 0;

        /* zero out the size field 
    	*/
        plist->pl_w_size = 0;
        }

    /* close the ring 
    */
    --plist;  			/* backup to last plist */
    plist->pl_a_next = pb->a_xmtplist_ring;

    plist->pl_l_next = cnv32( ((ULONG) plist->pl_a_next | window_base ) );

    /* init the FIFO pointers and entry counter 
    */
    pb->a_next_free_xmtplist = pb->a_next_sent_xmtplist = pb->a_xmtplist_ring;
    pb->no_free_xmt_plist = pb->xmt_buf_no;

    /* initialize the indices 
    */
    pb->tx_index_in = pb->tx_index_out = 0;

#if DW_DEBUG_INIT
    err_printf("\nxmt_plists: %d plists\n", pb->xmt_buf_no );
    show_plists( pb, pb->a_xmtplist_ring, pb->xmt_buf_no );
#endif

    return (msg_success);
    }


#if DW_DEBUG_INIT
/*+
 * ============================================================================
 * = show_plists - . =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * RETURNS:
 *
 * ARGUMENTS:
-*/
void show_plists( struct DW_PB *pb, PARAM_LIST *a_plist, int plist_cnt ) {
    int i;

    for (i=0; i < plist_cnt; i++)
        {
        err_printf("\n#%d: a_plist: %08x, a_next: %08x, a_buf: %08x \n",
            i, a_plist, a_plist->pl_a_next, a_plist->pl_a_buf );
            a_plist = a_plist->pl_a_next;
        }
    }
#endif /* DW_DEBUG_INIT */


/*+
 * ============================================================================
 * = dw_write - Driver write routine.                                         =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *  This routine writes a packet to the dw300 port.  Note: this routine
 *  expects a fully formatted ethernet (MOP v3) or IEEE 802.3 (MOV v4) frame to
 *  send.  It is necessary to filter the frame to adjust it to the IEEE 802.5
 *  with or without the source routing field format.
 *
 * the er_driver tested for loopback mode and packet
 * exceeding the max loopback packet in addition to state = driver started
 *
 * RETURNS:
 *  int size of packet written or 0 if failure
 *
 * ARGUMENTS:
 *  *fp 	- Pointer to the file to be written.
 *  item_size   - size of item
 *  number 	- number of items
 *  *buf 	- Buffer to be written.
 *
 * IMPLEMENTATION NOTES:
 *   returned size is adjusted ethernet packet size, not token ring
 *
-*/
int dw_write( struct FILE *fp, int item_size, int number, ETHERNET_FRAME *buf ) {
    struct NI_GBL *np;          /* Pointer to NI global database */
    struct NI_ENV *ev;          /* Pointer to environment variables */
    struct DW_PB *pb;           /* Pointer to the port block */
    UBYTE *b;                   /* Buffer pointer */

    int 
    	index, 
    	size, 	
    	fi,		/* Keeps track of size */
    	i,
    	len, 
    	w_temp,
    	return_size;     /* Length for padded buffers */

    PARAM_LIST *plist;

#if DW_DEBUG
    IEEE_FRAME *ieeef;
#endif

    /* If callbacks are needed use callback writes 
    */
    if (cb_ref)
        return dw_callback_write( fp, item_size, number, buf );

    np = (struct NI_GBL*)fp->ip->misc;
    pb = (struct DW_PB*)np->pbp;
    ev = (struct NI_ENV*)np->enp;

#if DW_TRACE
    err_printf( "\ndw_write %s \n", pb->pb.name );
#endif

    return_size = len = item_size * number;	/* Get the length */

#if DW_DEBUG_CONVERT
    err_printf("\ndw_write %s [ENET / 802.3]: @buf: %08x, item_size: %d, #: %d, len: %d\n", 
          pb->pb.name, buf, item_size, number, len );
    pprint_pkt( buf, len );
#endif

#if DW_DEBUG_PROTOCOL
    err_printf("\ndw_write %s [ENET / 802.3]: @buf: %08x, item_size: %d, #: %d, len: %d\n", 
          pb->pb.name, buf, item_size, number, len );

    w_temp = len;

    if (w_temp > 60)
        w_temp = 60;

    pprint_pkt( buf, w_temp );
#endif

    /* Just dump the message if the driver is stopped
    /*    if ((pb->state == DW_K_STOPPED) || (pb->state == DW_K_ADPTR_UNINITIALIZED)); 
    */
    if (pb->state != DW_K_STARTED)
        {
        ndl_deallocate_pkt( fp->ip, buf );
#if DW_DEBUG
        err_printf( "\ndw_write: driver STOPPED or UNIT'D\n",0 );
#endif
        return (0);
        }

    /* If we can send this message in a single frame we do it 
    */
    if (len <= DW_K_XMT_MAX_SIZE)
        {
        /* get an xmt parameter list and associated buffer: we can't use 
        /* ndl_allocate() because the token ring frame created from the enet 
        /* or IEEE 802.3 frame can be longer than the max frame allocated by 
        /* ndl_allocate() 
    	*/
        if ( (plist = dw_get_next_xmt_plist( pb )) == NULL )
            {
#if DW_DEBUG
    	    err_printf( "\ndw_write: no xmt plist available\n",0 );
#endif
            ndl_deallocate_pkt( fp->ip, buf );

            return 0;  /* no descriptors available */
            }

        len = convert_enet_to_tokenring( pb, plist->pl_a_buf, buf, len );

#if DW_DEBUG_CONVERT
        err_printf("\ndw_write [TOKEN RING]: len = %d\n", len );
        pprint_pkt( plist->pl_a_buf, len );
#endif

    	/* Send the packet 
    	*/
        spinlock( &(pb->spl_port) ); /* Protect the write */
    
        /* Set the transmit flags 
    	*/
        (pb->ti)[pb->tx_index_in].sync_flag = DW_K_TX_ASYNC;

        size = dw_out( pb, plist, len, TC_M_START | TC_M_END );
    
        spinunlock( &(pb->spl_port) );
        ndl_deallocate_pkt( fp->ip, buf );

        return (return_size);  /* size of enet | IEEE 802.3 frame */
        }
    else  
    	/* Write in multiple segments 
    	*/
        {
        /* this should not happen: other drivers containing similar code
         * never copy the packet header to the extra frames, (dw_write
         * expects a fully formatted ethernet or IEEE 802.3 frame for
         * transmission), so multi-segment writes CAN'T work as currently
         * implemented in virtually all the other network drivers. Since
         * the drivers work, the multi-segment write can't be being used,
         * so we'll stub it out with an error message here 
    	*/

#if DW_DEBUG
        err_printf( "\ndw_write: Unsupported multi-segment write\n", 0 );
#endif
        return 0;   /* error */
        }
    }



/*+
 * ============================================================================
 * = dw_callback_write - dw300 callback write routine.                        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *  This routine does a callback write.  It is ASYNCHRONOUS. It expects a 
 *  properly formatted ethernet or IEEE 802.3 frame, and the frame is
 *  transparently translated to the token ring format.
 *
 * ARGUMENTS:
 *  *fp       - Pointer to the file to be written.
 *  item_size - size of item
 *  number    - number of items
 *  *buf      - Buffer to be written.
 *
 * IMPLEMENTATION NOTES:
 *   returned size is adjusted ethernet packet size, not token ring
 *
-*/
int dw_callback_write(  
    struct FILE *fp, int item_size, int number, char *buf ) {

    struct NI_GBL *np;          
    struct DW_PB  *pb;   
    PARAM_LIST    *plist;

    int 
    	tx_index_in, 
    	return_size,
    	len;

    np = (struct NI_GBL*)fp->ip->misc;
    pb = (struct DW_PB*) np->pbp;

#if DW_DEBUG
    err_printf( "\ndw_callback_write %s \n", pb->pb.name );
#endif

    if (pb->state != DW_K_STARTED)
        return 0;

    /* Copy our source address 
    /* We save the station address in "bit reversed" format.
    */
    memcpy( buf+6, pb->sa, 6 );   

#if DW_DEBUG_CONVERT
    pprint_pkt( buf, item_size * number );
#endif

    /* Get the length 
    */
    return_size = len = item_size * number;

    /* If we can send this message in a single frame we do it 
    */
    if (len <= DW_K_XMT_MAX_SIZE)
        {
        /* Get an xmt parameter list and associated buffer: we can't use 
        /* ndl_allocate() because the token ring frame created from the enet 
        /* or IEEE 802.3 frame can be longer than the max frame allocated by 
        /* ndl_allocate() 
    	*/
        if ( (plist = dw_get_next_xmt_plist( pb )) == NULL )
            {
#if DW_DEBUG
    	    err_printf( "dw_write: no xmt plist available\n",0 );
#endif
            ndl_deallocate_pkt( fp->ip, buf );

            return 0;  
            }

        len = convert_enet_to_tokenring( pb, plist->pl_a_buf, buf, len );

#if DW_DEBUG_CONVERT
        err_printf("\ndw_write [TOKEN RING]: len = %d\n", len );
        pprint_pkt( plist->pl_a_buf, len );
#endif

        spinlock( &(pb->spl_port) ); 
    
        (pb->ti)[pb->tx_index_in].sync_flag = DW_K_TX_ASYNC;

    	if (dw_out(pb, plist, len, TC_M_START|TC_M_END) != len)
    	    return_size = 0;

        spinunlock( &(pb->spl_port) );

        ndl_deallocate_pkt( fp->ip, buf );

        return return_size;  
        }
    else  
    	/* Write in multiple segments 
    	/*
        /* this should not happen: other drivers containing similar code
    	/* never copy the packet header to the extra frames, (dw_write
    	/* expects a fully formatted ethernet or IEEE 802.3 frame for
    	/* transmission), so multi-segment writes CAN'T work as currently
    	/* implemented in virtually all the other network drivers. Since
    	/* the drivers work, the multi-segment write can't be being used,
    	/* so we'll stub it out with an error message here 
    	*/
        {
        err_printf( "dw_write: Unsupported multi-segment write\n", 0 );

        return 0;   
        }
    }


/*+
 * ============================================================================
 * = dw_out - Send the frame.                                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *  This routine sends a frame.
 *
 * RETURNS:
 *  int size of packet (no failure from this fn)
 *
 * ARGUMENTS:
 *  *pb     - ptr to extended port block
 *  *plist  - ptr to plist containing ptr to frame buffer
 *  len     - Number of bytes to be sent.
 *  flags   - control flags for multi-frame write.
 *
-*/
int dw_out( struct DW_PB *pb, PARAM_LIST *plist, int len, USHORT flags ) {

    SCB *a_scb;
    
#if DW_TRACE
    pprintf( "dw_out %s \n", pb->pb.name );
#endif

    /* fill in frame length data and clear the valid flag 
    */
    plist->pl_w_size 	  	 = bswp16( len );
    plist->pl_buf [0].pl_w_count = bswp16( len );
    plist->pl_w_cstat 		 = flags; 	  /* assert TCM_M_VALID clear */

    /* if the xmt plist processing is not running, start it 
    */
    if (flags & TC_M_START)   /* issue the transmit command */
    	{
    	/* pb->xmt_msg_plist is the FIRST plist of possible 
    	/* multi-plist frame.  Save it so we can set TC_M_VALID.
    	*/
    	pb->xmt_msg_plist = plist; 

        if (!pb->xmt_cmd_active)
            {
            /* isssue an xmt command - valid bit is clear so list processing
            /* waits for valid bit to be set, and valid interrupt to be issued 
    	    */
            if ((a_scb = dw_get_scb( pb )) == (SCB *) 0 )
                {
#if DW_DEBUG
    		pprintf( "dw_out: no room in cmd FIFO\n", 0 );
#endif
    	        return 0; 		/* no room in cmd FIFO */
    		}

    	    a_scb->scb_w_cmd     = CMD_C_XMIT;
    	    a_scb->scb_w_param_0 = bswp16( HIWORD32( ((ULONG) plist | window_base)) );
    	    a_scb->scb_w_param_1 = bswp16( LOWORD32(  (ULONG) plist ) );

            dw_issue_next_cmd( pb );

            pb->xmt_cmd_active = TRUE;
    	    }
    	}

    /* set the valid bit and tell the adpater there's a frame to send 
    */
    if (flags & TC_M_END)
        {
        /* always resume xmt parmeter list processing on TC_M_END - give
         * ownership of plist to adapter, and request interrupt on frame
         * completion 
    	*/
        pb->xmt_msg_plist->pl_w_cstat |= (TC_M_VALID | TC_M_INTER);
        dw_write_csr( pb, SIFCMDSTS, (CMD_M_INTADP|CMD_M_XMTVAL|CMD_M_SYSINT) );
        }

    return (len);	/* return without waiting for xmt status */    
    }


#if DW_DEBUG
/*+
 * ============================================================================
 * = show_plist - . =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * RETURNS:
 *
 * ARGUMENTS:
 *
-*/
void show_plist( PARAM_LIST *plist ) {
#if DW_OUT_USE_PPRINTF
    pprintf("dw_out plist: @%08x: cstat:%04x size:%d count:%d &buf:%08x\n",
        plist, bswp16(plist->pl_w_cstat), bswp16(plist->pl_w_size),
        bswp16(plist->pl_buf [0].pl_w_count), plist->pl_a_buf );
#else
    err_printf("\ndw_out plist: @%08x: cstat:%04x size:%d count:%d &buf:%08x\n",
        plist, bswp16(plist->pl_w_cstat), bswp16(plist->pl_w_size),
        bswp16(plist->pl_buf [0].pl_w_count), plist->pl_a_buf );
#endif
    }
#endif


/*+
 * ============================================================================
 * = dw_tx_proc - Transmit process.                                                =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *  This routine is actually a process that is created by dw_init_driver.
 *  It processes used transmit parameter lists.
 *
 * RETURNS:
 *  None
 *
 * ARGUMENTS:
 *  *ip - Pointer to the Device's inode.
 *
-*/
void dw_tx_proc( struct INODE *ip ) {
    struct NI_GBL *np;           /*Pointer to the NI global database*/
    struct DW_PB *pb;            /*Pointer to the port block*/

    np = (struct NI_GBL*)ip->misc;
    pb = (struct DW_PB*)np->pbp;
                  
    /* DO "forever" 
    */
    while ( TRUE )  
        {                  
        /* ordinarily, _next_status_xmtplist should point to the same
         * plist as _next_sent_xmtplist.  If we ever start xmting multiple
         * frames without setting the interrupt on frame complete, then
         * these ptrs will not be equal 
    	*/

        /* we don't need to test here - we still have to process plist(s)
         * even if the frame is not complete (i.e. command complete) 
    	*/
        dw_process_tx_plist( ip, pb->a_next_sent_xmtplist );
        
        krn$_wait( &(pb->dw_isr_tx_sem) );

#if DW_TXRX 
    	pprintf("*** dw_tx_proc waking up: %s plist @%08x ssb_w_status: %04x\n",
    		pb->pb.name, 
    		pb->a_next_sent_xmtplist, 
    		pb->a_next_sent_xmtplist->ssb_w_status );
#endif
        }
    }                  



/*+
 * ============================================================================
 * = dw_process_tx_plist - Process TX parameter lists.                        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *  This routine will process used transmit parameter lists. Plists
 *  for the beginning of a frame are checked for errors, and if 
 *  error free, are counted and summed. The plists are then released.
 *
 * RETURNS:
 *  int Status - msg_success
 *
 * ARGUMENTS:
 *  *ip   - Pointer to the Device's inode.
 *  plist - Pointer to an xmt plist.
 *
 * IMPLEMENTATION NOTES:
 *
 *   This implementation makes no provision for handling multi-plist
 *   frames - which means basically that we ignore the TC_M_START and
 *   TC_M_END flags, since both are assumed to be set in each plist 
 *   we receive.
-*/
int dw_process_tx_plist( struct INODE *ip, PARAM_LIST *plist ) {
    struct NI_GBL *np;              /*Pointer to the NI global database*/
    struct DW_PB *pb;            /*Pointer to the port block*/
    struct MOP$_V4_COUNTERS *mc;    /*Pointer to the MOP counters*/
    PARAM_LIST *next_plist;

    np = (struct NI_GBL*) ip->misc;
    pb = (struct DW_PB*) np->pbp;
    mc = (struct MOP$_V4_COUNTERS*) np->mcp;

#if DW_TRACE
    err_printf( "dw_process_tx_plist %s \n", pb->pb.name );
#endif

    /* In case there's more than one plist to process 
    */
    while (plist->pl_w_cstat & TC_M_COMPLETE)
        {
        /* If no errors, update the MOP frame and byte counters 
    	*/
        if (!(plist->pl_w_cstat & TC_M_XERR))
            {
            /* Count the frame */    
            _addq_v_u((mc->MOP_V4CNT_TX_FRAMES.l),1); 
    
            /* Accumulate the frame size*/
            _addq_v_u((mc->MOP_V4CNT_TX_BYTES.l), (USHORT) bswp16( plist->pl_w_size ));
            }

        /* else, the adapter counts the errors in the ERROR LOG 
        /* and they don't correspond to any of the MOP error 
        /* counters 
    	*/              
         
        /* If SYNC post the semaphore saying we're done with it
    	*/
        if ((pb->ti)[pb->tx_index_out].sync_flag == DW_K_TX_SYNC)
            {
            ((pb->ti)[pb->tx_index_out].tx_timer)->alt = TRUE;
            krn$_post(&(((pb->ti)[pb->tx_index_out].tx_timer)->sem));
            }

        /* release the plist and keep the ptr to the next plist 
    	/* to process current 
    	*/
        pb->a_next_sent_xmtplist = plist->pl_a_next;
        dw_release_xmt_plist( pb, plist );
        plist = pb->a_next_sent_xmtplist;
        } 

    return (msg_success);
    }



/*+
 * ============================================================================
 * = dw_get_next_xmt_plist - get next xmt parameter list                      =
 * = dw_release_xmt_plist - release a xmt parameter list                      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *   This pair of routines manage the xmt parameter list ring, and pointers
 *   used in TX_INFO entries.
 *
 * RETURNS:
 *  dw_get_next_xmt_plist: ptr to plist, or NULL
 *  dw_release_xmt_plist: none
 *
 * ARGUMENTS: 
 *  dw_get_next_xmt_plist:
 *     struct DW300_PB *pb; - ptr to device port block.
 *
 *  dw_release_xmt_plist:
 *     struct DW300_PB *pb; - ptr to device port block.
 *     PARAM_LIST *plist    - ptr to xmt plist.
 *
-*/
PARAM_LIST *dw_get_next_xmt_plist( struct DW_PB *pb ) {
    PARAM_LIST *rtn_val;

#if DW_TRACE
    err_printf( "dw_get_next_xmt_plist %s \n", pb->pb.name );
#endif

    if (pb->no_free_xmt_plist--)
        {
        rtn_val = pb->a_next_free_xmtplist;
        pb->a_next_free_xmtplist = (pb->a_next_free_xmtplist)->pl_a_next;

        /* keep the TX_INFO ptrs synchronized 
    	*/   
        (pb->tx_index_in)++;
        pb->tx_index_in %= pb->xmt_buf_no;
        }
    else
        {
        pb->no_free_xmt_plist++;
        rtn_val = NULL;
        }

    return (rtn_val);
    }


/*+
 * ============================================================================
 * = dw_release_xmt_plist - release a xmt parameter list                      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * RETURNS:
 *
 * ARGUMENTS: 
 *
-*/
void dw_release_xmt_plist( struct DW_PB *pb, PARAM_LIST *plist ) {


#if DW_TRACE
    err_printf( "dw_release_xmt_plist( %s \n", pb->pb.name );
#endif

    /* clear xmt control bits 
    */
    plist->pl_w_cstat = 0;  
    plist->ssb_w_status = 0;

    /* since the adapter consumes xmt descripters in the same order
     * they were produced (the xmt process is single threaded, single
     * priority level), the plist is the oldest plist in use
     * so we only need to increment the xmt free plist count */
    /* keep the TX_INFO ptrs synchronized */   

    (pb->tx_index_out)++;
    pb->tx_index_out %= pb->xmt_buf_no;
    
    pb->no_free_xmt_plist++;
    }



/*+
 * ============================================================================
 * = dw_set_env - Sets environment variables.                              =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *  This routine will create and set the DW300 environment variables.
 *
 * RETURNS:
 *  none
 *
 * ARGUMENTS:
 *  *name - Pointer to a name of the port.
 *
-*/
void dw_set_env (char *name) {
    char var[EV_NAME_LEN];          /*General holder for a name*/

    }


/*+
 * ============================================================================
 * = dw_read_env - Reads the environment variables.                        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *  This routine will read the environment variables.
 *
 * RETURNS:
 *  None
 *
 * ARGUMENTS:
 *  *ev   - Pointer to the environment variables.
 *  *name - Name of the unique part of the environment variable
 *
-*/
void dw_read_env( struct NI_ENV *ev, char *name ) {
    struct EVNODE *evp, evn;    /*Pointer to the evnode,evnode*/
    char var[EV_NAME_LEN];      /*General holder for a name*/

    /* Get a pointer to the evnode
    */
    evp = &evn;

    }

/*+
 * ============================================================================
 * = dw_control_t - dw300's control T routine.                             =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *  This routine is used to display dw300 status (counters etc.) in real time.
 *
 * RETURNS:
 *  None
 *
 * ARGUMENTS:
 *  *np - Pointer to the NI global database.
 *
-*/
void dw_control_t (struct NI_GBL *np) {

    struct DW_PB *pb;        /*Pointer to the port block*/

    pb = (struct DW_PB*)np->pbp;

#if DW_TRACE
    err_printf( "dw_control_t %s \n", pb->pb.name );
#endif

    /* Display some status
    */
#if FALSE
    printf(msg_ez_inf_dev_spec);
    printf("TxI: %d RxI: %d BABL: %d CERR: %d MISS: %d MERR: %d\n",
                        pb->cntr_w_xmt_int,pb->cntr_w_rcv_int,
                        pb->babl_cnt,pb->cerr_cnt,
                        pb->miss_cnt,pb->merr_cnt);
    printf("TX FULL: %d BRDCST: %d TX BUE: %d RX BUE: %d\n",pb->tbf_cnt,
                            pb->bp_cnt,
                            pb->tx_bue_cnt,
                            pb->rx_bue_cnt);
#endif
    }


/*+
 * ============================================================================
 * = dw_change_mode - Change the TMS380 mode.                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *  This routine will change the TMS380 operating mode. It uses the
 *  specified constant for the mode to be changed to.
 *
 * RETURNS:
 *  int Status - msg_success
 *
 * ARGUMENTS:
 *  *fp  - File pointing to the port.
 *  mode - The mode the TMS380 will be changed to.
 *
 *      NDL$K_NORMAL_OM:    Normal operating mode.
 *      NDL$K_INT:          Internal loopback.
 *      NDL$K_EXT:          External loopback.
 *      NDL$K_NORMAL_FILTER: Normal filter mode.
 *      NDL$K_PROM:         Promiscious.
 *      NDL$K_MULTICAST:    All multicast frames passed.
 *      NDL$K_INT_PROM:     Internal loopback and promiscious.
 *      NDL$K_INT_FC:       Internal loopback and force collision.
 *      NDL$K_NO_FC:        No force collision.
 *      NDL$K_DEFAULT:      Use the environment variable.
 *
-*/
int dw_change_mode (struct FILE *fp, int mode) {

#if DW_TRACE
    err_printf( "dw_change_mode %s \n", fp->ip->name );
#endif

    return (msg_success);
    }



/*+
 * ============================================================================
 * = dw_dump_csrs - Display the DW300 and TMS380 csrs.                     =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *  This routine will display the contents of the DW300 and TMS380 csrs.
 *  It should be used for debug purposes.
 *
 * RETURNS:
 *  None
 *
 * ARGUMENTS:
 *  *np -Pointer to the NI global database
 *
-*/
void dw_dump_csrs (struct NI_GBL *np) {
#if DW_DEBUG
    struct DW_PB *pb;        /*Pointer to the port block*/

    pb = (struct DW_PB*)np->pbp;

    if (pb->pb.type == TYPE_PCI)
        {
        printf("\nPCI CONFIG CSRS:\n");
        printf("\tVEND_DEV_ID %x: \t%x\n", VEND_DEV_ID, incfgl(pb, VEND_DEV_ID));
        printf("\tCOM_STAT    %x: \t%x\n", COM_STAT,    incfgl(pb, COM_STAT));
        printf("\tREV_ID      %x: \t%x\n", REV_ID,      incfgl(pb, REV_ID));
        printf("\tBASE_ADDR0  %x: %x\n", BASE_ADDR0,  incfgl(pb, BASE_ADDR0));
        printf("\tBASE_ADDR1  %x: %x\n", BASE_ADDR1,  incfgl(pb, BASE_ADDR1));
        printf("\tBASE_ADDR2  %x: %x\n", BASE_ADDR2,  incfgl(pb, BASE_ADDR2));
        printf("\tINT_LINE    %x: %x\n", INT_LINE,    incfgl(pb, INT_LINE));
        }

    printf("\nTMS380 CSRS:\n");
    printf("\tSIFDAT:    %04x \n", dw_read_csr( pb, SIFDAT ) );
    printf("\tSIFADR:    %04x \n", dw_read_csr( pb, SIFADR) );
    printf("\tSIFCMDSTS: %04x \n", dw_read_csr( pb, SIFCMDSTS ) );
    printf("\tSIFACL:    %04x \n", dw_read_csr( pb, SIFACL ) );
    printf("\tSIFADX:    %04x \n", dw_read_csr( pb, SIFADX ) );
    printf("\tSIFDMALEN: %04x \n", dw_read_csr( pb, SIFDMALEN ) );

    if (pb->pb.type == TYPE_EISA)
        {
        printf("\nDW300 CSRS:\n");
        printf("  ZC86: %04x   ZC87: %04x\n",
    		dw_read_byte_csr( pb, ZC86 ),
    		dw_read_byte_csr( pb, ZC87 ));
        printf("   MMR: %04x\n", 
    		dw_read_byte_csr( pb, MMR ));
    	}

#endif
    }


/*+
 * ============================================================================
 * = dw_read_csr - Read a dw300 csr.                                          =
 * = dw_read_byte_csr - Read a byte wide dw300 csr.                                          =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *  This routine will read a dw300 csr.
 *
 * RETURNS:
 *  USHORT csr_value - csr value read.
 *  UBYTE  csr value
 *
 * ARGUMENTS:
 *  *pb - Pointer to DW00 port block
 *  csr - Csr to be read.
 *
-*/
USHORT dw_read_csr ( struct DW_PB *pb, ULONG csr ) {
    return ( inportw( (struct pb *) pb, csr ) );
    }

UBYTE dw_read_byte_csr ( struct DW_PB *pb, ULONG csr ) {
    return ( inportb( (struct pb *) pb, csr ) );
    }


/*+
 * ============================================================================
 * = dw_write_csr - Write an dw300 csr.                                       =
 * = dw_write_byte_csr - Write a byte wide dw300 csr                          =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *  This routine will write an dw300 csr.
 *
 * RETURNS:
 *  none
 *
 * ARGUMENTS:
 *  csr   - Csr to be written.
 *  value - Value to be written.
 *  *pb   - device port block
-*/
void dw_write_csr( struct DW_PB *pb, ULONG csr, USHORT value ) {
#if DW_DEBUG_CSR
    err_printf( "\nwrite csr %08x val: %08x\n", csr, value );
#endif

    /*Write the address register*/
    outportw( (struct pb *) pb, csr, value );
    }

void dw_write_byte_csr( struct DW_PB *pb, ULONG csr, UBYTE value ) {
#if DW_DEBUG_CSR
    err_printf( "\nwrite csr %08x val: %08x\n", csr, value );
#endif

    /*Write the address register*/
    outportb( (struct pb *) pb, csr, value );
    }


/*+
 * ============================================================================
 * = convert_enet_to_tokenring - translate enet/IEEE 802.3 frame to 802.5     =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *  Converts an ethernet or csma/cd (IEEE 802.3) frame to token ring
 *  (IEEE 802.5) formatted frame.  MOP multicast addresses are translated to
 *  corresponding token ring MOP function addresses.  For ethernet, LLC fields
 *  are defined.  The optional source routing field is inserted, as required.
 *
 * RETURNS:
 *  len of token ring frame
 *
 * ARGUMENTS:
 *  *pb  - device port block - contains relevant control data
 *  *trf - pointer to buffer to contain the whole frame
 *  *enf - pointer to source enet/IEEE 802.3 frame
 *  len  - length of ethernet or 802.3 frame
 *
 * SIDE EFFECTS:
 *  Frame type is identified (ENET vs IEEE) and saved.  Received
 *  frames are filtered using the same frame type sent.
 *
 * IMPLEMENTATION NOTES:
 *
 *  This implementation only requests routing info for MOP multicasts,
 *  and only saves the routing info for the first response received.
 *  This will probably have to be changed to support a list of known
 *  destinations and the corresponding routing info.  And we will probably
 *  have to request routing info for any destination not in the list.
 *
 *  Conversion of frame lengths:
 *    Enet header: 16 + 0 LLC + data  (excl CRC-16?)
 *    IEEE 802.3 header: 14 + 8 LLC + data (excl CRC-32?)
 *    TR 802.5 header: 14 + x RIF + 8 LLC + data
 *
 *    tr_len = enet_len -2 + rif_len + llc_len = enet_len + 6 + rif_len
 *    tr_len = ieee_len + rif_len
 *
 *    enet_info_len = enet_len - 16
 *    ieee_info_len = ieee_len - 22
 *    tr_info_len = tr_len - 22 - rif_len
-*/
int convert_enet_to_tokenring( struct DW_PB *pb, 
                               TOKEN_RING_HEADER *trf,
                               ETHERNET_FRAME *enf, 
    			       int len ) {

    IEEE_LLC_HEADER *trf_llc;
    IEEE_FRAME      *ieeef;

    int i;
    int adj_len;
    USHORT protocol;

    struct EVNODE *evp, evn;    /*Pointer to the evnode,evnode*/
    char var [20];

#if DW_TRACE
    err_printf( "\nconvert_enet_to_tokenring %s \n", pb->pb.name );
#endif

    adj_len = len;

    /* determine whether frame is ethernet or IEEE 802.3 by testing
     * ethernet protocol field: ethernet value for MOP protocols is
     * 0x6000, 0x6001 or 0x9000, while IEEE has data length in same
     * location (<= 1500).  The magic value 1500 in this test is
     * inherited from the fr_driver. */

    protocol = (enf->protocol_type_0 << 8) | (enf->protocol_type_1);

    if ( protocol > 1500)
        pb->frame_format = ENET; /* Ethernet Frame format */        
    else
        pb->frame_format = IEEE;    /* IEEE Frame format */

    trf->ac = 0x10;          /* priority 0, frame */
    trf->fc = 0x40;          /* non-MAC control frame, no special handling */

    /* translate multicast destination addr to token ring function address 
    */
#if DW_DEBUG
    err_printf( "\nbefore filter: da = %02x-%02x-%02x-%02x-%02x-%02x ",
        enf->dest_addr[0], enf->dest_addr[1],
        enf->dest_addr[2], enf->dest_addr[3],
        enf->dest_addr[4], enf->dest_addr[5] );
    err_printf( ", sa = %02x-%02x-%02x-%02x-%02x-%02x\n",
        enf->source_addr[0], enf->source_addr[1],
        enf->source_addr[2], enf->source_addr[3],
        enf->source_addr[4], enf->source_addr[5] );
#endif

    if (dw_filter_dest_addr( (SA *) trf->dest_addr, (SA *) enf->dest_addr ))
        pb->need_routing_info = TRUE;

#if DW_DEBUG
    err_printf( "\nafter filter: da = %02x-%02x-%02x-%02x-%02x-%02x ",
        trf->dest_addr[0], trf->dest_addr[1],
        trf->dest_addr[2], trf->dest_addr[3],
        trf->dest_addr[4], trf->dest_addr[5] );
#endif

    /* setup source address, and source routing flag */
    memcpy( trf->source_addr, enf->source_addr, 6 );

    trf_llc = (IEEE_LLC_HEADER *) trf->ri;  /* presume location of LLC header */

    /* Moved from dw_open(), fixes hangs during >>> show dwa* 
    */
    evp = &evn;
    sprintf( var, "%4.4s_source_route", pb->pb.name );

    if ( ev_read( var, &evp,0) == msg_success )
        {
        pb->use_src_routing   = atoi( evp->value.string );
        pb->need_routing_info = TRUE;
        }
    else
        {
        pb->use_src_routing   = FALSE;      /* 0 */
        pb->need_routing_info = FALSE;
        }


    if (pb->use_src_routing)
        {
        trf->source_addr [0] |= 0x80;   /* set source routing field present flag */
        if (pb->need_routing_info)
            {
            pb->routing_len = 2;
            trf->ri [0] = 0xe2;
            trf->ri [1] = 0x70;
            for (i=2; i < pb->routing_len; i++)
                trf->ri [i] = 0;
            }
        else
            {
            memcpy( trf->ri, pb->routing_info, pb->routing_len );
#if TRUE
            trf->ri [1] ^= 0x80;    /* toggle direction flag */
#else
            trf->ri [1] &= 0x7f;    /* mark info outbound (from us) */
#endif
            }
        }
    else
        {
        trf->source_addr[0] &= 0x7f;   /* clr source routing field present flag */
        pb->routing_len = 0;
        }

    adj_len += pb->routing_len;

    /* fix up LLC stuff */

    trf_llc = (IEEE_LLC_HEADER *)((char *) trf_llc + pb->routing_len);
    if (pb->frame_format == ENET)
        {
        trf_llc->dsap = 0xaa;
        trf_llc->ssap = 0xaa;
        trf_llc->ui = 0x03;
        trf_llc->oui [0] = 0;
        trf_llc->oui [1] = 0;
        trf_llc->oui [2] = 0;
        trf_llc->pid [0] = enf->protocol_type_0;
        trf_llc->pid [1] = enf->protocol_type_1;

        adj_len += 8; /* (LLC field len: 8) + (AC,FC: 2) 
                       * - (protocol: 2) = 8 */

        /* MIMIC FR_DRIVER.C - OMITS LEN FIELD, ASSUMES DATA STARTS IMMEDIATELY
        /* AFTER PROTOCOL FIELD 
        /* copy data to buffer - data begins AT enet length 
        */
        memcpy( trf_llc->data, enf->len, len-ENET_HDR_SIZE+2 );
        }
    else  /* using IEEE 802.3 */
        {
        ieeef = (IEEE_FRAME *) enf;
        memcpy( &(trf_llc->dsap), &(ieeef->dsap), len-IEEE_HDR_SIZE );
        }

    return (adj_len); /* actual token ring frame length, for plist */
    }


/*+
 * ============================================================================
 * = convert_tokenring_to_enet - translate 802.5 to enet/IEEE 802.3 frame     =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *  Converts a token ring frame (IEEE 802.5) to ethernet or csma/cd (IEEE 802.5)
 *  formatted frame. The optional source routing field is saved for use
 *  in outgoing frames, as required.
 *
 * RETURNS:
 *  length of resulting ethernet or IEEE 802.3 frame
 *
 * ARGUMENTS:
 *  *pb  - device port block - contains relevant control data
 *  *trf - pointer to source the token ring frame
 *  *enf - pointer to buffer to contain enet/IEEE 802.3 frame
 *  len  - token ring frame length
 *
 * SIDE EFFECTS:
 *
 *  Frame is translated using saved type of most recent outgoing frame
 *     (ENET vs IEEE) and saved.
 *
-*/
int convert_tokenring_to_enet( struct DW_PB *pb, ETHERNET_FRAME *enf, 
                                   TOKEN_RING_HEADER *trf, int len ) {
    IEEE_LLC_HEADER *trf_llc;
    IEEE_FRAME *ieeef;
    int  adj_len;

#if DW_TRACE
    err_printf( "\nconvert_tokenring_to_enet %s \n", pb->pb.name );
#endif

    /* copy destination and source addresses to buffer
     * we shouldn't need to do any address filtering 
    */
    adj_len = len;

    memcpy( &(enf->dest_addr), &(trf->dest_addr), 12 );
    enf->source_addr [0] &= 0x7f;  /* guarantee no source routing flag */
    trf_llc = (IEEE_LLC_HEADER *) trf->ri;      /* presume no source routing */

    if (trf->source_addr [0] & 0x80)  /* if frame has source routing info */
        {
        pb->routing_len = trf->ri [0] & 0x1f;
        trf_llc = (IEEE_LLC_HEADER *)((char *) trf_llc + pb->routing_len);
        adj_len -= pb->routing_len;

        /* if we need routing info, and ri field has it, then save it
         *  and corresponding server address 
    	*/
        if (pb->need_routing_info && (trf->ri [0] & 0xe0)== 0) /* no broadcast */
            {
            memcpy( pb->routing_info, trf->ri, pb->routing_len );
            memcpy( pb->server_addr, trf->source_addr, 6 ); /* include RIF flag */

            pb->need_routing_info = FALSE;
            /* ??? do we talk with only a single station, or do we need
             * to save routing info from multiple destinations */
            }
        }

/* MIMIC FR_DRIVER.C - ASSUME FRAME TYPE ALWAYS HAS THE SNAP SAP LLC FIELDS
 * AND THAT FOR SOME REASON, FRAMES THAT SHOULD BE PASSED UPWARD AS IEEE
 * 802.3 FRAMES HAVE SOME NON-ZERO BYTES IN THE PID_0, _1 OR _2 BYTES 
 *
 * THIS IS AN ODD ASSUMPTION, BECAUSE IEEE 802.3 FRAMES ORIGINATING HERE
 * MAY HAVE 0s IN THOSE (PID 0..2) BYTES.  FURTHERMORE, IN ATTEMPTING
 * TO BOOT WITH BOOTP, WITH HARDWARE TYPE SET TO TOKEN RING, THE SERVER
 * SENDS IEEE 802.2 XID COMMAND FRAMES THAT NULL DSAP AND SSAP FIELDS
 * INSTEAD OF SNAP/SAP (THE BYTES EQUIV TO PID_0 .. _2 DO CONTAIN NON-ZERO
 * VALUES, SO THIS PART IS VALID FOR XID FRAMES) */

    if ( trf_llc->oui [0] == 0 && 
    	 trf_llc->oui [1] == 0 && 
    	 trf_llc->oui [2] == 0 )
        {
        enf->protocol_type_0 = trf_llc->pid [0];
        enf->protocol_type_1 = trf_llc->pid [1];

    	/* TOKEN RING FRAME LENGTH (REPORTED BY ADAPTER) = DATA LENGTH + 22 */
    	/* ETHERNET FRAME LENGTH TO REPORT = DATA LENGTH + 14 */
    	/* IEEE 8802.3 FRAME LENGTH TO REPORT = DATA LENGTH + 22 
    	*/

#if FALSE
    	/* omit length field */
        enf->len [0] = HIBYTE16( adj_len );
        enf->len [1] = LOBYTE16( adj_len );
#endif

        memcpy( &(enf->len), &(trf_llc->data), 
              adj_len-(TR_HDR_SIZE+IEEE_LLC_SIZE) );

        adj_len -= 8;   /* -8:LLC fields +2:enet_protocol -2:AC,FC */
        }
    else
        {
        ieeef          = (IEEE_FRAME *) enf;
        ieeef->len [0] = HIBYTE16( adj_len );
        ieeef->len [1] = LOBYTE16( adj_len );

        memcpy( &(ieeef->dsap), &(trf_llc->dsap), adj_len-TR_HDR_SIZE );
        }

    return (adj_len);  /* length of resulting ethernet or 802.3 frame */
    }


/*+
 * ============================================================================
 * = dw_filter_dest_addr - translates multicast to function addresses         =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *  This routine copies src_addr to the dest_addr, and if the src address
 *  matched a known MOP ethernet or IEEE 802.3 multicast addresses it
 *  substitutes the corresponging token ring MOP function address.
 *
 * RETURNS:
 *   if address recognized as multicast return TRUE, else return FALSE
 *
 * ARGUMENTS:
 *   *a_tr   -  token ring dest address to be written
 *   *a_enet -  source ethernet or IEEE 802.3 address to be tested
 *
-*/
int dw_filter_dest_addr( SA *a_tr, SA *a_enet  ) {

    int filtered = TRUE;   /* presume we find multicast addr */

    if (dw_match_addr( a_enet, (SA *) mop_dl_multicast ))
        {
        /* use token ring mop dump/load function address */
        memcpy( a_tr, mop_dl_func_addr, 6 );
        }
    else 
    	if (dw_match_addr( a_enet, (SA *) mop_rc_multicast ))
    	    {
    	    /* use token ring mop remote console function address */
            memcpy( a_tr, (SA *) mop_rc_func_addr, 6 );
    	    }
#if FALSE    /* need mop loopback functional address for token ring */
        else 
    	    if (dw_match_addr( a_enet, (SA *)  mop_lp_multicast ))
    		{
    		/* use token ring mop loopback function address */
    	        memcpy( a_tr, mop_lp_func_addr, 6 );
    		}
#endif
        else
    	    {
    	    /* pass-thru dest_addr */
            memcpy( a_tr, a_enet, 6 );
            filtered = FALSE;       /* it wasn't a recognised multicast */
    	    }

    /* if (!filtered)
     *     search known destination address list here, and set filtered=TRUE
     * if dest addr is found not found on list (to force request for routing
     * info) 
    */
    return (filtered);
    }



/*+
 * ============================================================================
 * = dw_match_addr - compare two network addresses for equality               =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *   Compare two network addresses for equality.
 *
 * RETURNS:
 *  TRUE if addresses match, else FALSE
 *
 * ARGUMENTS:
 *
 *  *addr1  - pointer to 6 byte network address
 *  *addr2  - pointer to 6 byte network address
 *
 * IMPLEMENTATION NOTES:
 *
 *  note use of (char *) and casting to force correct byte ptr arithmetic
 *  otherwise, we would be incrementing by sizeof( SA )!
-*/
int dw_match_addr( SA *addr1, SA *addr2 ) {
    char *a1;
    char *a2;
    int i;

    a1 = (char *) addr1;
    a2 = (char *) addr2;

    /* logically correct, but probably could be faster 
    */
    for (i=0; i<6; i++)
       if (*a1++ != *a2++)
           return FALSE;   /* no match */

    return TRUE;
    }


/*+
 * ============================================================================
 * = dw_read_error_log - read adapter error log                               =
 * = dw_update_error_counters - update counters                               =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *   Read the current values for error counters maintained by adapter, and
 *   add them to corresponding counters maintained in the driver.
 *
 * RETURNS:
 *   msg_success or msg_failure;
 *
 * ARGUMENTS:
 *   *pb - pointer to port block where driver counters are saved
 *
-*/
int dw_read_error_log( struct DW_PB *pb ) {
    SCB *a_scb;
    
    /* Error and die, if no room in cmd FIFO 
    */
    if ((a_scb = dw_get_scb( pb )) == (SCB *) 0 )
        return msg_failure;  

    a_scb->scb_w_cmd     = CMD_C_RDERR;
    a_scb->scb_w_param_0 = bswp16( HIWORD32( ((ULONG) pb->a_error_log | window_base)) );
    a_scb->scb_w_param_1 = bswp16( LOWORD32( (ULONG) pb->a_error_log ) );

    dw_issue_next_cmd( pb );

    return msg_success;
    }
  

/*+
 * ============================================================================
 * = dw_update_error_counters - . 	=
 * ============================================================================
 *
 * OVERVIEW:
 *
 * RETURNS:
 *
 * ARGUMENTS:
 *
-*/
int dw_update_error_counters( struct DW_PB *pb ) {
    ERROR_LOG errs;

    /* check command status 
    */
    if (!(pb->errlog_result & CMD_C_SUCCESS))
        return msg_failure;  /* command failed */

    /* update the port block counters 
    */
    pb->cntr_w_line_error   += (int) errs.erl_b_line_error;
    pb->cntr_w_burst_error  += (int) errs.erl_b_burst_error;
    pb->cntr_w_arifci_error += (int) errs.erl_b_arifci_error;
    pb->cntr_w_lostframe    += (int) errs.erl_b_lostframe;
    pb->cntr_w_rcv_congest  += (int) errs.erl_b_rcv_congest;
    pb->cntr_w_framecopy    += (int) errs.erl_b_framecopy;
    pb->cntr_w_token_error  += (int) errs.erl_b_tokenerr;
    pb->cntr_w_dmabuserror  += (int) errs.erl_b_dmabuserr;
    pb->cntr_w_dmaparerror  += (int) errs.erl_b_dmaparerr;

    return msg_success;  
    }


/*+
 * ============================================================================
 * = dw_get_scb - get next scb in next free scblist                           =
 * = dw_release scb - release the least recently used scb/scblist             =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *   These routines manage the SCB_LIST ring, which implements an SCB FIFO, 
 *   feeding the dedicated adapter SCB. A FIFO is needed to buffer commands
 *   for the adapter, because the adapter may not have copied the most recently
 *   issued command SCB into its internal memory, and the system SCB may not
 *   be overwritten until that's done.
 *   
 * RETURNS:
 *   dw_get_scb returns a ptr to SCB or NULL;
 *   TRUE on success, else FALSE;
 *
 * ARGUMENTS:
 *   *pb - pointer to port block where driver counters are saved
 *
-*/
SCB *dw_get_scb( struct DW_PB *pb ) {   
    SCB *a_scb;

    if (pb->no_free_scblists-- >0)
        {
        a_scb = &(pb->a_next_free_scblist->scb);
        pb->a_next_free_scblist = pb->a_next_free_scblist->a_next_list; 

        return (a_scb);
        }
    else
        {
        pb->no_free_scblists++;

        return (SCB *) 0;
        }    
    }


/*+
 * ============================================================================
 * = dw_release_scb - =
 * ============================================================================
 *
 * OVERVIEW:
 *   
 * RETURNS:
 *
 * ARGUMENTS:
-*/
int dw_release_scb( struct DW_PB *pb ) {    
    /* SCB lists are implemented as a FIFO, using a ring buffer 
    */
    if (++pb->no_free_scblists <= DW_K_SCBLISTS)
        {
        pb->a_next_cmd_scblist->scb.scb_w_cmd = 0;  
        pb->a_next_cmd_scblist = pb->a_next_cmd_scblist->a_next_list;

        return msg_success;
        }
    else
        {
        pb->no_free_scblists--;

        return msg_failure;
        }
    }


/*+
 * ============================================================================
 * = dw_issue_next_cmd - send the next command off the SCB FIFO               =
 * = dw_write_cmd - write a command to the adapter                            =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *   These routines manage sending commands to the adapter.  dw_issue_next_cmd
 *   copies the next SCB from the SCB FIFO into the adapter SCB, where
 *   dw_write_cmd signals the adapter that a new command is ready, after
 *   testing for the adapter's readyness to accept a new command.
 *  
 * RETURNS:
 *   msg_success if command is sent to adapter, else msg_failure
 *
 * ARGUMENTS:
 *   *pb - pointer to port block where driver counters are saved
 *
-*/
int dw_issue_next_cmd( struct DW_PB *pb ) {                        

    /* If there's a command ready in the dedicated adapter SCB, try to send it.
    /* Else if there's a command in the SCB_LIST FIFO, copy it to the adapter 
    /* SCB and attempt to send it 
    */
    if (pb->a_scb->scb_w_cmd != 0)
        return dw_write_cmd( pb );
    else 
    	if (pb->a_next_cmd_scblist->scb.scb_w_cmd != 0)
    	    { 
            memcpy( pb->a_scb, &(pb->a_next_cmd_scblist->scb), sizeof(SCB));
            dw_release_scb( pb ); 

            return dw_write_cmd( pb );
    	    }
        else
    	    /* else nothing to do 
    	    */    
            return msg_failure;
    }


/*+
 * ============================================================================
 * = dw_write_cmd - =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * RETURNS:
 *
 * ARGUMENTS:
-*/
int dw_write_cmd( struct DW_PB *pb ) {
    USHORT sifsts;
    
    if ((sifsts = dw_read_csr( pb, SIFCMDSTS )) & STS_M_INTADP)
        return msg_failure;   /* adptr has not consumed last scb, not ready for new one */
    else
        { 
        /* request int on scb clr, keep CMD_M_SYSINT = 1 so we don't 
    	/* accidentally clear an incoming interrupt.
    	*/
        dw_write_csr( pb, SIFCMDSTS, 
                 (CMD_M_INTADP|CMD_M_EXECUTE|CMD_M_SCBREQ|CMD_M_SYSINT) );

        return msg_success;
        }
    }

/*+
 * ============================================================================
 * = net.hlp - Help text to Access a network port 			      =
 * ============================================================================
 *
 * GENERAL OVERVIEW:
 *	Using the consoles >>> net command to perform network device specific 
 *	maintenance operations.
 *  
 *   FORM:
 *      net ( [-s]   [-sa]    [-ri]    [-ic]   [-id] 
 *	      [-l0]  [-l1]    [-rb]    [-csr]  [-els] 
 *	      [-kls] [-start] [-stop]  
 *	      [-cm <mode_string>] 
 *	      [-da <node_address>] 
 *	      [-l  <file_name>] 
 *	      [-lw <wait_in_secs>]
 *	      [-sv <mop_version>] 
 *	      <port_name> ) 
 *  
 *   ARGUMENT(S):
 *	<port_name> - Specifies the Networdk port on which to operate.
 *
 *   OPTION(S):
 *	-s     - Display port status information including MOP counters.
 *	-sa    - Display the port's Ethernet station address.
 *	-ri    - Reinitialize the port drivers.
 *	-start - Start the port drivers.
 *	-stop  - Stop the port drivers.
 *	-ic    - Initialize the MOP counters.
 *	-id    - Send a MOP Request ID to the specified node.
 *		 Uses -da to specify the destination address.
 *	-l0    - Send an Ethernet loopback to the specified node.
 *		 Uses -da to specify the destination address.
 *	-l1    - Do a MOP loopback requester.
 *	-rb    - Send a MOP V4 boot message to remote boot a node.
 *	 	 Uses -da to specify the destination address.
 *	-csr   - Displays the values of the Ethernet port CSRs.
 *	-els   - Enable the extended DVT loop service.
 *	-kls   - Kill the extended DVT loop service.
 *
 *	-cm <mode_string> - Change the mode of the port device.
 *		The mode string may be any one of the following abbreviations.
 *		      o	nm =  Normal mode
 *		      o	in =  Internal Loopback
 *		      o	ex =  External Loopback
 *		      o	nf =  Normal Filter
 *		      o	pr =  Promiscious
 *		      o	mc =  Multicast
 *		      o	ip =  Internal Loopback and Promiscious
 *		      o	fc =  Force collisions
 *		      o	nofc = Don't Force collisions
 *		      o	df   = Default 
 *		      o	tp   =  Twisted pair
 *		      o	aui  = AUI
 *	-da <node_address> - Specifies the destination address of a node
 *			     to be used with -l0, -id, or -rb, options.
 *	-l  <file_name>    - Attempt a MOP load of the file name.
 *	-lw <wait_in_secs> - Wait the number of seconds specified for the 
 *			     loop messages from the -l1 option to return.  
 *			     If the messages don't return in the time period, 
 *			     an error message is generated.
 *	-sv <mop_version>  - Set the prefered MOP version number for operations.
 *		             Legitimate values are 3 or 4.
 *
 *   EXAMPLE(S):
 *	Display the local Ethernet ports' station address.
 *      >>>net -sa
 *      -fra0:  08-00-2b-1d-02-91
 *      >>>net -sa frb0
 *      -frb0:  08-00-2b-1d-02-92
 *
 *	Display the fra0 port status, including the MOP counters.
 *      >>>net -s
-*/


/*+                                                                  
 * ============================================================================
 * = nettest.hlp - Help text to test the network ports using MOP loopback     =
 * ============================================================================
 *                                                
 * GENERAL OVERVIEW:                                            
 *                                                                           
 *      Nettest contains the basic options to allow the user to run MOP
 *	loopback tests.  It is assumed that nettest will be included in a
 *	script for most applications.  Many environment variables can be
 *	set to customize nettest.  These may be set/show, from the console 
 *	before nettest is started, ie.
 *	>>>
 *	>>>show fra*
 *	dwa0_loop_count         3e8
 *	dwa0_loop_inc           a
 *	dwa0_loop_patt          ffffffff
 *	dwa0_loop_size          2e
 *	dwa0_lp_msg_node        1
 *	>>>
 *	>>>set dwa0_loop_count 200
 *	>>>set dwa0_loop_inc   10
 *
 *   ENVIRONMENT VARIABLE(S):
 *	To show/set port environment variable perform the following console
 *	command line:
 *	    >>> show dwa*
 *	    >>> set  dwa0_loop_count %d100000
 *
 *	dwa0_lp_msg_node _
 *		This qualifier specifies the number of unique frames queued to 
 *		each port.  The destination of the frame is determined in the 
 *		file lp_nodes_dwa0. To set this qualifier perform the following 
 *		console command line, the default is 1 frame.
 *		    >>> set  dwa0_lp_msg_node %d50
 *	dwa0_loop_count  - 
 *		This qualifier specifies the number of times to ReTransmit 
 *		each frame to the node specified in lp_nodes_dwa0.To set this 
 *		qualifier perform the following console command line, the 
 *		default is %d1000:
 *		    >>> set  dwa0_loop_count %d100000
 *	dwa0_loop_size   - 
 *		The initial byte size of each LoopBack frame is specified by
 *		this qualifier.  The byte size of the frames will grow by 
 *		dwa0_loop_inc bytes for each ReTransmission of the frame. 
 *		To set this qualifier perform the following console command 
 *		line, the default is %d46
 *		    >>> set  dwa0_loop_size %d1500
 *	dwa0_loop_inc    - 
 *		Specifies the growth of each ReTransmitted frame to a maximum
 *		of %d1500 bytes for both ENET/IEEE/FDDI. To set this qualifier 
 *		perform the following console command line, the default is %d10
 *		    >>> set  dwa0_loop_inc  %d20
 *	- dwa0_loop_patt   
 *		Specifies the frame data pattern.  A value of F's selects
 *		all possible patterns.  If not F's, a fixed pattern is selected.
 *		If the value is F's,  each time a frame is ReTransmitted, the 
 *		next dwa0_loop_patt is selected, and the frame size is
 *		incremented by dwa0_loop_inc.  To set this qualifier perform 
 *		the following console command line, the default is %d10
 *		    >>> set  dwa0_loop_patt 5.
 *		Patterns to select from are as following:
 *
 *		o 0 : all zeros
 *		o 1 : all ones
 *		o 2 : all fives
 *		o 3 : all 0xAs
 *		o 4 : incrementing data
 *		o 5 : decrementing data
 *		o ffffffff : all patterns
 *
 * CREATING lp_nodes_dwa0
 *	To create <lp_nodes_dwa0> from the console perform the following 
 *	command lines:
 *	>>>
 *	>>>net -sa dwa0
 *	-dwa0:  08-00-2b-a1-71-7a
 *	>> 
 *	>>> net -sa dwa0 > ndbr/lp_nodes_dwa0
 *	>>>
 *	>>> cat ndbr/lp_nodes_dwa0
 *	Node: 08-00-2b-a1-71-7a
 *	>>>
 *	>>> echo "-file:  08-00-2b-a1-71-7a" >  ndbr/lp_nodes_all
 *	>>> echo "-file:  08-00-2b-a1-96-4b" >> ndbr/lp_nodes_all
 *	>>>
 *	>>>cat ndbr/lp_nodes_all
 *	Node: 08-00-2b-a1-71-7a
 *	Node: 08-00-2b-a1-96-4b
 *
 * DETAILED OVERVIEW:                                            
 *  	The routine MOP_LOOP_REQUESTER() is called to queue any number of frames
 *	to the NetWork Port specified by the file lp_nodes_dwa0. The qualifier 
 *	dwa0_lp_msg_node specifies the number of unique frames queued to the
 *	SOURCE port.  The destination of the LoopBack frame is determined in 
 *	the file lp_nodes_dwa0. For each dwa0_lp_msg_node frame ReTransmitted 
 *	the:
 *	    - next dwa0_loop_patt is selected
 *	    - next dwa0_loop_size is incremented by dwa0_loop_inc
 *
 *	Once lp_nodes_dwa0 receives the LoopBack frame, it FORWARDs it back 
 *	to Source port as a LoopBack REPLY.  When the Source port receives the 
 *	REPLY, it counts the number of times received and will either:
 *	   - ReTransmit it, if dwa0_loop_count is not reached
 *	   - Stop ReTransmission once dwa0_loop_count is reached
 *
 *	Once MOP_LOOP_REQUESTER() has queued dwa0_lp_msg_node frames to
 *	each lp_nodes_dwa0 ports, MOPLP_WAIT() is called.  This routine
 *	waits for each dwa0_lp_msg_node to be ReTransmitted for
 * 	dwa0_loop_count times.  MOPLP_WAIT() will continue waiting for
 *	each counter to be reached for -to xx seconds.  An error will be 
 *	reported, along with the number of frames currently received and 
 *	frames expected, if the timeout expired.
 *
 *   FORM:                                             
 *                                                  
 *	>>> nettest ( [-f <file>] [-mode <port_mode>] [-p <pass_count>]
 *                [-sv <mop_version>] [-to <loop_time>] [-w <wait_time>] 
 *		  [<port>] )
 *                                                                           
 *   TAG: nettest 0 RXBZ nettest
 *                                               
 *   ARGUMENT(S):     
 *                                                                 
 *	<port> - 
 *		Specifies the SOURCE Network Port on which to run this test.
 *                                             
 *   OPTION(S):                                                
 *                                                             
 *     -f <lp_nodes_dwa0> - 
 *		A file containing the list of DESTINATION network station
 *		addresses to loop messages to.  The default file name is 
 *		lp_nodes_dwa0 for port dwa0.  The files by default have their 
 *		own station address.		
 *	
 *     -p <pass_count> - 
 *		Specifies the number of times to run the test.  Use 0 to run
 *		test forever.  The default is 1.  Note: This is the number of 
 *		passes for the diagnostic.  Each pass will send the number of 
 *		loop messages as set by the environment variable, 
 *		dwa0_loop_count.  
 *                                                     
 *     -sv <mop_version> - 
 *		Specifies which MOP version protocol to use.
 *		If 3, then MOP V3 (DECNET Phase IV) packet format is used.
 *		If 4, then MOP V4 (DECNET Phase V IEEE 802.3) format is used.
 *                     
 *     -to <seconds>   - 
 *		Specifies the time in seconds allowed for the loop
 *		messages to be returned.  The default is 2 seconds.	
 *		                                 
 *     -w <seconds> - 
 *		Specifies the time in seconds to wait between passes
 *		of the test.  The default is 0 (no delay).  The network 
 *		device can be very CPU intensive.  This option will allow 
 *		other processes to run.		
 *		                                 
 *     -mode <TGEC_mode> - 
 *		Specifies the mode to set the port adapter (TGEC). The 
 *		default is 'ex' (external loopback).  This qualifier is 
 *		unsupported for other network device.
 *
 *		Allowed values are:
 *
 *		o df : default, use environment variable values
 *		o ex : external loopback
 *		o in : internal loopback
 *	       	o nm : normal mode         
 *		o nf : normal filter
 *		o pr : promiscious
 *		o mc : multicast                    
 *		o ip : internal loopback and promiscious
 *    		o fc : force collisions
 *		o nofc : do not force collisions
 *		o nc : do not change mode
 *
 *   COMMAND EXAMPLE(S):                                 
 *                                                               
 *	>>>nettest		   - internal loopback test on port dwa0
 *	>>>nettest dw*		   - internal loopback test on ports dwa0/dwb0
 *	>>>nettest -mode ex 	   - external loopback test on port dwa0
 *	>>>nettest -mode ex -w 10  - external loopback test on port dwa0 wait 10
 *				     seconds between tests	
 *	>>>nettest -f lp_nodes_dwa0 dwb0 -to 20 & 
 *	>>>nettest -f lp_nodes_dwb0 dwa0 -to 20 & 
 *
 * ADDITIONAL HELP:
 *	As nettest is running as a background process, the user can view
 *	it runtime status by executing the following console command:
 *	>>> show_status
 *	 ID       Program      Device       Pass  Hard/Soft Bytes Written  Bytes Read
 *	-------- ------------ ------------ ------ --------- ------------- -------------
 *	0000006b      nettest dwb0.0.0.5.1     23    0    0       1058068       1058000
 *	0000006e      nettest dwa0.0.0.4.1     19    0    0        874068        874000
 *
*/
