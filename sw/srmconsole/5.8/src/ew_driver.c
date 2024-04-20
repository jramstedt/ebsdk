/* file:	ew_driver.c
 *
 * Copyright (C) 1993, 1996 by
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
 *      Console Firmware
 *
 *  ABSTRACT:	PCI Ethernet port driver.
 *
 *	Port driver for the TULIP board.
 *
 *  AUTHORS:
 *
 *	John DeNisco
 *
 *  CREATION DATE:
 *  
 *      08-Apr-1993
 *
 *  MODIFICATIONS
 *
 *
 *  MAR 10-Feb-2000     Report invalid setmode if a user enter 
 *			an invalid mode
 *
 *  MAR	 6-May-1999	Fix FastFD mode for DE500-BA.
 *
 *  TNA 11-Nov-1998     Conditionalized for YUKONA
 *
 *  DTR 6-Oct-1998	Add monet support for the 21143 chip - different the
 *			de500 series devices.
 *
 *  SKA 17-Jul-98	tu_read() - add CORTEX/YUKON directive to use original 
 *			callback_timeout value for VxWorks to work.			
 *
 *  JWJ 12-MAY-98	Change value for 21040 csr14 for twisted pair to enable
 *			collision detection csr14 =0xffffffff (TP) or 
 *			csr14 = 0xfffffffd (TPFD).  This setup is according to
 *			the 21040 HRM.  Having collision detection disabled was 
 *			affecting performance of network downloads.
 *
 *  MAR 26-Mar-98       Change default values to twisted except for de500-aa,
 *			de500-ba
 *  MAR 13-Feb-98       1) tu_read() - add takara/k2 directive to use original
 *                      callback_timeout value for VxWorks to work.
 *			2) tx_rx - change process priority to 6 for OVMS
 *			~20% performance with cluster booting and with 
 *			using system code debugger
 *
 *  MAR 10-Dec-97	1)Add callback check in tu_auto_negotiate() to avoid
 * 			having to re-auto-negotiate if a callback is in progress.
 *			If a callback is in progress then we have already
 *			auto-negotiated.2) change de500ba sequence in 
 *			tu_auto_negotiate()
 *
 *  MAR 10-Dec-97	Improve timing in tu_read() when invoked by a callback
 *			OpenVMS reported considerable delay with their
 *			Source Code Debugger. This is what prompted the change.
 *
 *  MAR 03-Dec-97	Update csr15 default for DE500-FA
 *
 *  MAR 13-Nov-97	Add DE450-CA support for  external loopback mode
 *
 *  JWJ 22-Oct-97	remove medulla specific enable_interrupt() and disable_interrupt()
 *
 *  MAR 16-Sept-97	Add support for the DE500-FA                         
 *
 *  MAR 08-Sept-97	Add the ability to send a new frame with a multicast 
 *			address. This change was based on an OpenVMS request 
 *			for the ability do get the hardware addr of a LAN
 *			adapter that is OpenVMS's secondary boot device.
 *			See tu_open() for details. See also Boot.c
 *
 *  JWJ 25-JUL-1997	Set Medulla's embedded tulip to default of AUI and
 *  			incorporate support of the ewx0_mode environment 
 *			variable 
 *
 *  MAR 27-Jun-1997	Reduce counter to optimal value in tu_auto_negotiate()
 *  
 *  MAR	28-May-1997	Assign global variables for tu_auto_negotiate(), 
 *			change the OR value in tu_interrupt().
 *  MAR	29-Apr-1997	Fix LED values for de500-ba in twisted pair mode, reduce
 *                      auto-negotiation timing loop values.
 *
 *
 *  MAR	25-Apr-1997	Init CFDD register to clear bit<31> which is getting set
 *                      by PCI Reset.
 *
 *  MAR	31-MAR-1997	More changes for DE500-ba to work on modular platform.
 *
 *  MAR	11-MAR-1997	Adjust csr bits for twisted and twisted-fd mode.
 *                      The symptom was that the DUnix console displayed
 *                      random link status in either mode.
 *  MAR	10-MAR-1997	Fix mop/bootp bug with DE500-BA & init csr12 for 21143
 *  MAR	 4-MAR-1997	Add support for the DE500-BA
 *
 *--
 */

/*Include files*/

#include "cp$src:platform.h"
#include "cp$src:common.h"		/*Common definitions*/
#include "cp$src:platform_cpu.h"
#include "cp$src:kernel_def.h"		/*Kernel definitions*/
#include "cp$src:msg_def.h"		/*Message definitions*/
#include "cp$src:prdef.h"		/*Processor definitions*/
#include "cp$src:dynamic_def.h"		/*Memory allocation defs*/
#include "cp$src:pb_def.h"		/*Port block definitions*/
#include "cp$src:ni_dl_def.h"		/*Datalink definitions*/
#include "cp$src:ni_env.h"		/*NI Environment variables*/
#include "cp$src:ev_def.h"		/*Environment vars*/
#include "cp$src:mop_counters64.h"	/*MOP counters*/
#include "cp$src:tu.h"			/*Tulip register definitions*/
#include "cp$src:tupb.h"		/*Tulip port block definitions*/
#include "cp$src:probe_io_def.h"
#include "cp$inc:platform_io.h"
#include "cp$src:ctype.h"
#include "cp$src:if_tureg.h"
#include "cp$inc:kernel_entry.h"
#include "cp$inc:prototypes.h"

#if MODULAR
#undef ew_mode_table
#endif

#if MONET
extern int tt_reprompt;
extern void halt_reprompt();
#endif

#define EISA_SUPPORT SABLE || MIKASA || ALCOR

#define APS AVANTI || K2 || MTU

#if APS || EISA_SUPPORT
#include "cp$src:eisa.h"
#endif

#if APS || EB164 || TAKARA
#include "cp$src:pci_size_config.h"
#endif

#if ( STARTSHUT || DRIVERSHUT )
#include "cp$src:mem_def.h"		/*Need the device structure*/
#define msg_ez_err_nisa_bad_chk 99
#define msg_ez_err_nisa_n_rpt 999
#define msg_ez_err_bad_patt 9999
#define msg_ez_err_nisa_null 99999
#define tu_window_base 0x0
#else
#define tu_window_base ((u_long)io_get_window_base(tupb))
#endif

#define ALLOCATE_PKT(j,i) ((u_long)ndl_allocate_pkt(j,i) + tu_window_base)

#if ( STARTSHUT || DRIVERSHUT )
extern struct EVNODE evlist;		/* header of the list of variables */
extern struct window_head config;
extern int net_ref_counter;

char *ew_modes[] = {			/* */
  "INTERRUPT", 				/* set to interrupt mode */
  "POLLED", 				/* set to polled mode  */
  "STOP", 				/* set to stopped */
  "START", 				/* set to start */
  "ASSIGN", 				/* perform system startup */
  "LOOPBACK_INTERNAL", 			/* device internal loopback */
  "LOOPBACK_EXTERNAL", 			/* device external loopback */
  "NOLOOPBACK", 			/* no loopback */
  "OPEN", 				/* file open */
  "CLOSE"				/* file close */
};
#endif

#if MODULAR
#define DEBUG 0
#else
#define DEBUG 0
#endif

/* Debug routines */

/*
 * debug flag
 * 1 = top level
 * 2 = malloc and free
 * 4 = packets
 * 8 = interrupts & port_s
 * 10 = dumps
 * 40 = raw packets
 * 100 = qprintf
 */

int tulip_debug = 0;

#if DEBUG
#include "cp$src:print_var.h"
#define dqprintf _dqprintf
#define dprintf(fmt, args)	\
    _dprintf(fmt, args)
#define d2printf(fmt, args)	\
    _d2printf(fmt, args)
#define d4printf(fmt, args)	\
    _d4printf(fmt, args)
#define d8printf(fmt, args)	\
    _d8printf(fmt, args)
#define d10printf(fmt, args)	\
    _d10printf(fmt, args)
#else
#define dqprintf qprintf
#define dprintf(fmt, args)
#define d2printf(fmt, args)
#define d4printf(fmt, args)
#define d8printf(fmt, args)
#define d10printf(fmt, args)
#endif

#if DEBUG
static void _dprintf(fmt, a, b, c, d, e, f, g, h, i, j)
{
    if ((tulip_debug & 1) != 0) {
	if ((tulip_debug & 0x100) == 0)
	    pprintf(fmt, a, b, c, d, e, f, g, h, i, j);
	else
	    qprintf(fmt, a, b, c, d, e, f, g, h, i, j);
    }
}
static void _d2printf(fmt, a, b, c, d, e, f, g, h, i, j)
{
    if ((tulip_debug & 2) != 0) {
	if ((tulip_debug & 0x100) == 0)
	    pprintf(fmt, a, b, c, d, e, f, g, h, i, j);
	else
	    qprintf(fmt, a, b, c, d, e, f, g, h, i, j);
    }
}
static void _d4printf(fmt, a, b, c, d, e, f, g, h, i, j)
{
    if ((tulip_debug & 4) != 0) {
	if ((tulip_debug & 0x100) == 0)
	    pprintf(fmt, a, b, c, d, e, f, g, h, i, j);
	else
	    qprintf(fmt, a, b, c, d, e, f, g, h, i, j);
    }
}
static void _d8printf(fmt, a, b, c, d, e, f, g, h, i, j)
{
    if ((tulip_debug & 8) != 0) {
	if ((tulip_debug & 0x100) == 0)
	    pprintf(fmt, a, b, c, d, e, f, g, h, i, j);
	else
	    qprintf(fmt, a, b, c, d, e, f, g, h, i, j);
    }
}
static void _d10printf(fmt, a, b, c, d, e, f, g, h, i, j)
{
    if ((tulip_debug & 0x10) != 0) {
	if ((tulip_debug & 0x100) == 0)
	    pprintf(fmt, a, b, c, d, e, f, g, h, i, j);
	else
	    qprintf(fmt, a, b, c, d, e, f, g, h, i, j);
    }
}
static void _dqprintf(fmt, a, b, c, d, e, f, g, h, i, j)
{
    if (tulip_debug)
	_dprintf(fmt, a, b, c, d, e, f, g, h, i, j);
    else
	qprintf(fmt, a, b, c, d, e, f, g, h, i, j);
}
#endif

/*Local definitions*/

/*External references*/

extern int cbip;
extern int cb_ref;			/*Call backs flag*/
extern int cb_fp[];
extern struct FILE *el_fp;
extern struct ZONE *memzone;		/*Heap header   */
extern int spl_kernel;			/*Kernel spinlock       */
extern struct QUEUE pollq;		/*POLLQ                 */
extern int null_procedure();
extern unsigned int crc32(unsigned char *m, unsigned int n, unsigned int s,
  unsigned int crc);

/*Platform specific externals and prototypes*/

#if MEDULLA || CORTEX || YUKONA
#include "cp$src:hwrpb_def.h"
extern int cpip;
#endif

/*Platform specific ENET_ID_ADDR*/

#if SABLE || MIKASA || ALCOR
#define ENET_ID_ADDR 0x0500
#endif

#if APS
#define ENET_ID_EISA_ADDR 0x048
#endif

/*Function prototypes*/

int turbo_tu_setmode(int mode, struct device *dev,...);
int ew_init();
void tu_check_mode(struct TUPB *tubp);
int tu_open(struct FILE *fp);
#if !( STARTSHUT || DRIVERSHUT )
int tu_read(struct FILE *fp, int size, int number, unsigned char *c);
#endif
void tu_free_rcv_pkts(struct INODE *ip);
void tu_build_perfect(struct TUPB *tupb, char *setup_frame);
int tu_change_mode(struct FILE *fp, int mode);
void tu_platform_init(struct TUPB *tupb);
int tu_check_nirom(struct TUPB *tupb, unsigned char *ni);
int tu_driver_init(struct TUPB *tupb);
void tu_dump_csrs(struct NI_GBL *np);
int tu_ev_write(char *name, struct EVNODE *ev);
void tu_get_nirom(struct TUPB *tupb, unsigned char *ni);
void tu_get_tulip_nirom(struct TUPB *tupb, unsigned char *ni);
int tu_hardware_init(struct INODE *ip);
int tu_init_device(struct TUPB *tupb);
void tu_init_rx(struct INODE *ip, struct TUPB *tupb);
void tu_init_st(struct INODE *ip, struct TUPB *tupb);
void tu_init_tx(struct TUPB *tupb);
void tu_interrupt(struct NI_GBL *np);
int tu_nirom_checksum(unsigned char *ni);
int tu_out(struct TUPB *tupb, char *buf, int len, int setup);
void tu_poll(struct NI_GBL *np);
void tu_process_rx(struct FILE *fp, struct INODE *ip, struct TUPB *tupb);
void tu_process_tx(struct TUPB *tupb);
void tu_read_mode(struct TUPB *tubp);
int tu_restart_driver(struct INODE *ip);
void tu_rx(struct INODE *ip);
int tu_send(struct INODE *ip, struct TUPB *tupb, char *buf, int len,
int setup);
int tu_setmode(struct TUPB *tupb, int mode);
int tu_switch_con(struct TUPB *tupb, int mode_auto);
void tu_init_media(struct TUPB *tupb);
void tu_show(struct NI_GBL *np);
int tu_software_init(struct INODE *ip);
int tu_stop_driver(struct INODE *ip);
void tu_tx_count(struct TUPB *tupb, struct _TU_RING *tdes);
void tu_rx_count(struct TUPB *tupb, struct _TU_RING *rdes);
int tu_write(struct FILE *fp, int item_size, int number, char *buf);
int microwire_nirom(struct TUPB *tupb, unsigned char *ni);
int tu_check_micro(struct TUPB *tupb, unsigned char *ni);
/* added for DE500 Support */
unsigned short tu_auto_negotiate(struct TUPB *tupb);
unsigned long tu_getfrom_mii(struct TUPB *pb, unsigned long command);
void tu_sendto_mii(struct TUPB *pb, unsigned long command, unsigned long data);
unsigned long tu_mii_rdata(struct TUPB *pb);
void tu_mii_wdata(struct TUPB *pb, unsigned long data, unsigned long len);
unsigned long tu_mii_swap(unsigned long data, unsigned long len);
void tu_mii_turnaround(struct TUPB *pb, unsigned long rw);
unsigned short tu_mii_rd(struct TUPB *pb, unsigned char phyreg);
void tu_mii_wr(struct TUPB *pb, unsigned char phyreg, unsigned long data);
void tu_flags_to_ability(struct TUPB *pb);
void tu_ability_to_flags(struct TUPB *pb , unsigned char abilities );

unsigned long abnormal_intr_error_flag;

#if ( STARTSHUT )
struct tupb *turbo_ew_init_port(struct device *dev);
struct PBQ *findpbe(struct pb *pb);
#endif
#if ( DRIVERSHUT )
struct tupb *turbo_ew_init_port(struct pb *pb);
struct PBQ *findpbe(struct pb *pb);
#endif

/*Global definitions*/

#if ( STARTSHUT || DRIVERSHUT )
struct PBQ _align(QUADWORD) tupblist = {&tupblist, &tupblist, 0};
					/*Port block list*/
#endif

#if ( STARTSHUT || DRIVERSHUT )
char *mode_str[] = {"Interrupt", 	/* 0 */
  "Polled", 				/* 1 */
  "Stop", 				/* 2 */
  "Start", 				/* 3 */
  "Assign", 				/* 4 */
  "Loopback_Internal", 			/* 5 */
  "Loopback_external", 			/* 6 */
  "Noloopback"				/* 7 */
  };
#endif

#if (RAWHIDE)
#define AUTO_SENSE  1
#else
#define AUTO_SENSE  0
#endif

char tu_card_name[10]={0,0,0,0,0,0,0,0,0,0};   /* for P2SE+ */

char ew_not_used[] = "Not-Used";
char ew_auto_sensing[] = "Auto-Sensing";
char ew_aui[] = "AUI";
char ew_fast[] = "Fast";
char ew_twisted_pair[] = "Twisted-Pair";
char ew_twisted_pair_fd[] = "Full Duplex, Twisted-Pair";
char ew_bnc[] = "BNC";
char ew_fastfd[] = "FastFD (Full Duplex)";
char ew_auto_negotiate[] = "Auto-Negotiate";

struct set_param_table ew_mode_table[] = {
#if AUTO_SENSE
  {ew_auto_sensing, 0}, 		/* */
#endif
  {ew_twisted_pair, 0}, 		/* */
  {ew_twisted_pair_fd, 0}, 		/* */
  {ew_aui, 0}, 				/* */
  {ew_bnc, 0}, 				/* */
  {ew_fast, 0}, 			/* */
  {ew_fastfd, 0}, 			/* */
  {ew_auto_negotiate, 0},		/* */
  {0}					/* */
  };

static int cbl_ev_sev_init(struct env_table *et, char *value)
{
    return ev_sev_init(et, value);
}

struct env_table ew_mode_ev = {		/* */
  0, 
#if AUTO_SENSE
  ew_auto_sensing, 
#else
  ew_aui, 
#endif
  EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
  ew_mode_table, tu_ev_write, 0, cbl_ev_sev_init};

/*Driver database definition*/

#if ( STARTSHUT || DRIVERSHUT )
extern struct DDB *ew_ddb_ptr;
#else

struct DDB ew_ddb = {			/*                              */
  "ew", 				/* Calling name                 */
  tu_read, 				/* read routine                 */
  tu_write, 				/* write routine                */
  tu_open, 				/* open routine                 */
  null_procedure, 			/* close routine                */
  null_procedure, 			/* name expansion routine       */
  null_procedure, 			/* delete routine               */
  null_procedure, 			/* create routine               */
  null_procedure,			/* setmode                      */
  null_procedure, 			/* validation routine           */
  0, 					/* class specific use           */
  1, 					/* allows information           */
  0, 					/* must be stacked              */
  0, 					/* is a flash update driver     */
  0, 					/* is a block device            */
  0, 					/* not seekable                 */
  1, 					/* is an ethernet device        */
  0 					/* is a filesystem driver       */
};
#endif

/*Globals to be used as environment variables*/

long int tu_ser_com = TU_SER_COM_VAL;	/*Serial command information    */
long int tu_xmt_buf_no = TU_XMT_BUF_CNT;
					/*Number of transmit buffers   */
long int tu_rcv_buf_no = TU_RCV_BUF_CNT;
					/*Number of Receive buffers    */
long int tu_xmt_mod = TU_XMT_MOD;	/*XMT Descriptor alignment modulus*/
long int tu_xmt_rem = TU_XMT_REM;	/*XMT Descriptor alignment remainder*/
long int tu_rcv_mod = TU_RCV_MOD;	/*RCV Descriptor alignment modulus*/
long int tu_rcv_rem = TU_RCV_REM;	/*RCV Descriptor alignment remainder*/
long int tu_driver_flags = 0;		/*Driver flags                  */
long int tu_msg_buf_size = TU_MSG_BUF_SIZE;
					/*Message buffer size       */
long int tu_msg_mod = TU_MSG_MOD;	/*Message Modulus               */
long int tu_msg_rem = TU_MSG_REM;	/*Message Remainder             */

/* Used for Auto Negotiation */
unsigned short ability;
int fast_speed = 0,full_duplex = 0;

/*Register access macros*/
/*
 * read a byte or long from a tulip csr.
 *      arguments
 *          p       Pointer to the port block (as defined in TUPB.h)
 *          r       Register as defined in tu.h
 * return value - register data
 * 
 * Note, some platforms use memory space and some use I/O space.  The decision
 * of which space to use must be matched with the address that is stored in 
 * pb->csr (i.e. memory space or I/O space). 
 */

#if EISA_SUPPORT
#define RD_TULIP_CSR_BYTE(p,r)  (p->pb.type==TYPE_EISA?inportb(p,p->pb.csr+(r)*p->rm):inmemb(p,p->pb.csr+(r)*p->rm))
#define RD_TULIP_CSR_LONG(p,r)  (p->pb.type==TYPE_EISA?inportl(p,p->pb.csr+(r)*p->rm):inmeml(p,p->pb.csr+(r)*p->rm))
#else
#define RD_TULIP_CSR_BYTE(p,r)  (inmemb(p,p->pb.csr|(r)))
#define RD_TULIP_CSR_LONG(p,r)  (inmeml(p,p->pb.csr|(r)))
#endif

/*
 * Write a byte or long to a tulip csr.
 *      arguments
 *          p       Pointer to the port block (as defined in TUPH.h)
 *          r       Register as defined in tu.h
 *          v       Value to be written
 * return value - None
 */

#if EISA_SUPPORT
#define WRT_TULIP_CSR_BYTE(p,r,v)  (p->pb.type==TYPE_EISA?outportb(p,p->pb.csr+(r)*p->rm,v):outmemb(p,p->pb.csr+(r)*p->rm,v))
#define WRT_TULIP_CSR_LONG(p,r,v)  (p->pb.type==TYPE_EISA?outportl(p,p->pb.csr+(r)*p->rm,v):outmeml(p,p->pb.csr+(r)*p->rm,v))
#else
#define WRT_TULIP_CSR_BYTE(p,r,v)  (outmemb(p,p->pb.csr|(r),v))
#define WRT_TULIP_CSR_LONG(p,r,v)  (outmeml(p,p->pb.csr|(r),v))
#endif

#define TULIP_ENABLE_INTERRUPT(p,v) io_enable_interrupts(p,v)
#define TULIP_DISABLE_INTERRUPT(p,v) io_disable_interrupts(p,v)

/*
 * Medulla is "special".  Thanks, Joe.
 */
#if MEDULLA
#define RD_TULIP_CSR_BYTE(p,r)  inportb(p,p->pb.csr|(r))
#define RD_TULIP_CSR_LONG(p,r)  inportl(p,p->pb.csr|(r))
#define WRT_TULIP_CSR_BYTE(p,r,v)  outportb(p, p->pb.csr|(r), v)
#define WRT_TULIP_CSR_LONG(p,r,v)  outportl(p, p->pb.csr|(r), v)
#endif

/*+
 * ============================================================================
 * = ew_init - Initialization routine for the port driver                     =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine sniffs the eisa slots and then calls the per
 *	slot initialization routine.
 *
 * FORM OF CALL:
 *
 *	ew_init (); 
 *
 * RETURNS:
 *
 *	int Status - msg_success or FALIURE.
 *
 * ARGUMENTS:
 *
 *      None
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int ew_init()
{
#if !( STARTSHUT || DRIVERSHUT )
    void ew_init_pb();

/*Find all Tulips*/

    find_pb("ew", sizeof(struct TUPB), ew_init_pb);
#endif

    return (msg_success);
}

/*+
 * ============================================================================
 * = ew_init_pb - Per slot initialization routine 		              =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine is called by a callback in the find_pb function
 *	to actually perform the call to the per slot device inuit.
 *
 * FORM OF CALL:
 *
 *	ew_init_pb (); 
 *
 * RETURNS:
 *
 *	int Status - msg_success or FALIURE.
 *
 * ARGUMENTS:
 *
 *      None
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
void ew_init_pb(struct TUPB *pb)
{
    char name[16];
    int subtype;

    log_driver_init(pb);

    set_io_prefix(pb, name, "_mode");
    ew_mode_ev.ev_name = name;

    pb->pci_type = 0;
    if (pb->pb.type != TYPE_EISA) {
	pb->pci_type = incfgl(pb, 0);
	subtype = incfgl(pb, 0x2c);
    }

/* Default to twisted-pair */
	ew_mode_ev.ev_value = ew_twisted_pair; 

/* Default to 100BaseTx - fast for DE500-FA */
	if ( subtype = 0x500f1011 )
	ew_mode_ev.ev_value = ew_fast; 

/* default Medulla's embedded tulip to AUI */
#if MEDULLA
    if (pb->pb.hose==0 && pb->pb.bus==0 && pb->pb.slot==1){
	ew_mode_ev.ev_value = ew_aui;
    }
#endif

/* default CORTEX's embedded tulip to twisted pair */
#if CORTEX || YUKONA
    if (pb->pb.hose==0 && pb->pb.bus==0 && pb->pb.slot==1) {
	ew_mode_ev.ev_value = ew_twisted_pair;
    }
#endif

    ev_init_ev(&ew_mode_ev, 0);

/* For Cortex, leave ew_mode_ev.ev_value at twisted pair for subsequent
 * tulips, since any pmc tulip cards will be tp or fast-tp.
 */
#if CORTEX || YUKONA
/* put it back to twisted-pair incase there are any others after this one */

    ew_mode_ev.ev_value = ew_twisted_pair;
#else
/* all other platforms default to AUI. */
    ew_mode_ev.ev_value = ew_aui;
#endif


    pb->pb.mode = DDB$K_INTERRUPT;
    set_io_name(pb->pb.name, 0, 0, 0, pb);
    set_io_alias(pb->pb.alias, 0, 0, pb);

/*Init the module*/

    tu_driver_init(pb);
}

/*+
 * ============================================================================
 * = tu_driver_init - Initialization routine for tulip driver                 =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will initialize the Tulip driver it calls routines
 *	that first initialize the sofware data structures, the hardware,
 *	and finally the upper driver layers.
 *
 * FORM OF CALL:
 *
 *	tu_driver_init (struct TUPB *tupb); 
 *
 * RETURNS:
 *
 *	int Status - msg_success or FALIURE.
 *
 * ARGUMENTS:
 *
 *      struct TUPB *tupb - Tulip port block.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int tu_driver_init(struct TUPB *tupb)
{
    int i;
    int ew_pid;
    struct INODE *ip;			/* Pointer to the INODE */
    struct NI_GBL *np;			/* Pointer to the NI global database */
    struct PORT_CALLBACK *cb;		/* Callback structure*/
    char name[32];			/*General holder for a name*/

    dprintf("I found a tulip port name: %s %08x %08x\n",
	p_args3(tupb->pb.name, tupb->pb.csr, tupb->pb.vector));
    dprintf("slot %d hose: %d bus: %d channel: %d controller: %d\n",
      p_args5(tupb->pb.slot, tupb->pb.hose, tupb->pb.bus, tupb->pb.channel,
      tupb->pb.controller));

/*Allocate the Port Block*/

    np = (struct NI_GBL *) dyn$_malloc(sizeof(struct NI_GBL),
      DYN$K_FLOOD | DYN$K_SYNC | DYN$K_NOOWN);
    np->pbp = tupb;

/* Create an inode entry, thus making the device visible as a file. */

    allocinode(tupb->pb.name, 1, &ip);
#if ( STARTSHUT || DRIVERSHUT )
    ip->dva = ew_ddb_ptr;
#else
    ip->dva = &ew_ddb;
#endif
    ip->dev = tupb->pb.config_device;
    ip->attr = ATTR$M_READ | ATTR$M_WRITE;
    ip->loc = 0;
    ip->misc = (int *) np;
    INODE_UNLOCK(ip);

    tupb->ip = ip;
    tupb->pb.setmode = tu_setmode;

/*Initialize the software data structures*/

    tu_software_init(ip);

/*Initialize the tulip hardware*/

    if (tu_hardware_init(ip) != msg_success)
	return (msg_failure);

/*Setup the callback structure*/

    cb = (struct PORT_CALLBACK *) malloc_noown(sizeof(struct PORT_CALLBACK));
    cb->st = tu_show;
    cb->cm = tu_change_mode;
    cb->ri = null_procedure;
    cb->re = null_procedure;
    cb->dc = tu_dump_csrs;

/*Fill in the station address and insert the port block*/

    sprintf(tupb->pb.info, "%02X-%02X-%02X-%02X-%02X-%02X", tupb->sa[0],
      tupb->sa[1], tupb->sa[2], tupb->sa[3], tupb->sa[4], tupb->sa[5]);

#if ( STARTSHUT || DRIVERSHUT )
    sprintf(tupb->pb.string, "%-18s %-8s  %20s", tupb->pb.name,
      tupb->pb.alias, tupb->pb.info);
#else
    sprintf(tupb->pb.string, "%-24s   %-8s   %24s", tupb->pb.name,
      tupb->pb.alias, tupb->pb.info);
#endif

/*Initialize the datalink*/

    ndl_init(ip, cb, tupb->sa, (int *) &(tupb->lrp));

/*Set the state flag*/

    tupb->state = TU_K_STARTED;

#if ( STARTSHUT || DRIVERSHUT )

/*Initialize receive process*/

    sprintf(name, "rx_isr_%4.4s", ip->name);
    krn$_seminit(&(tupb->rx_isr), 0, name);

    sprintf(name, "rx_%4.4s", ip->name);
    krn$_seminit(&(tupb->cmplt_rx_s), 0, "ew_cmplt_rx");

    dprintf("creating %s\n", p_args1(name));

    ew_pid = krn$_create(	/* */
	tu_rx, 			/* address of process */
	null_procedure, 	/* startup routine */
	&tupb->cmplt_rx_s, 	/* completion semaphore */
	5, 			/* process priority */
	1 << whoami(),		/* cpu affinity mask */
	1024 * 4, 		/* stack size */
	name, 			/* process name */
	"tt", "r",		/* stdin */
	"tt", "w",		/* stdout */
	"tt", "w",		/* stderr */
	ip);			/* argument */

    krn$_wait(&tupb->tu_ready_s);

    dprintf("%s pid = %x\n", p_args2(name, ew_pid));

    krn$_wait(&tupb->tu_port_s);
    tupb->pb.ref--;
    dprintf("[2] ref=%d fcount=%d\n", p_args2(tupb->pb.ref, tupb->fcount));
    krn$_post(&tupb->tu_port_s);
#endif

    return (msg_success);
}

/*+
 * ============================================================================
 * = tu_software_init - Initialization routine for tulip driver data structures =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routines will initialize the software data structures needed
 *	by the tulip driver.
 *
 * FORM OF CALL:
 *
 *	tu_software_init (ip) 
 *
 * RETURNS:
 *
 *	int Status - msg_success or FALIURE.
 *
 * ARGUMENTS:
 *
 *      struct INODE *ip - pointer the inode.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void tu_software_init(struct INODE *ip)
{
    struct PBQ *pbe;			/* Pointer to a port block element*/
    struct NI_GBL *np;			/* Pointer to the NI global database */
    struct TUPB *tupb;			/* Pointer to the port block */
    struct NI_ENV *ev;			/*Pointer to the environment
					 * variables*/
    char name[32];			/*General holder for a name*/
    int i, ew_media;
    char var[EV_NAME_LEN];

    dprintf("tu_software_init\n", p_args0);

/*Allocate some memory and get some pointers*/

    np = (struct NI_GBL *) ip->misc;
    np->dlp = (int *) dyn$_malloc(sizeof(struct NI_DL),
      DYN$K_SYNC | DYN$K_NOOWN | DYN$K_FLOOD | DYN$K_ALIGN, 32, 0);
    np->enp = (int *) dyn$_malloc(sizeof(struct NI_ENV),
      DYN$K_SYNC | DYN$K_NOOWN | DYN$K_FLOOD | DYN$K_ALIGN, 32, 0);
    np->mcp = (int *) dyn$_malloc(sizeof(struct MOP$_V4_COUNTERS),
      DYN$K_SYNC | DYN$K_NOOWN | DYN$K_FLOOD | DYN$K_ALIGN, 32, 0);
    tupb = (struct TUPB *) np->pbp;
    ev = (struct NI_ENV *) np->enp;
    tupb->mc = (struct MOP$_V4_COUNTERS *) np->mcp;

/*Get the memory for the descriptors*/
/*Transmit*/

    if (tu_xmt_mod || tu_xmt_rem)

/*Use alignment*/

	tupb->tdes =
	  (PTU_RING) dyn$_malloc(sizeof(TU_RING) * tu_xmt_buf_no,
	  DYN$K_SYNC | DYN$K_NOOWN | DYN$K_FLOOD | DYN$K_ALIGN, tu_xmt_mod,
	  tu_xmt_rem);
    else

/*Don't use alignment*/

	tupb->tdes = (PTU_RING) malloc_noown(sizeof(TU_RING) * tu_xmt_buf_no);

/*Receive*/

    if (tu_rcv_mod || tu_rcv_rem)

/*Use alignment*/

	tupb->rdes =
	  (PTU_RING) dyn$_malloc(sizeof(TU_RING) * tu_rcv_buf_no,
	  DYN$K_SYNC | DYN$K_NOOWN | DYN$K_FLOOD | DYN$K_ALIGN, tu_rcv_mod,
	  tu_rcv_rem);
    else

/*Don't use alignment*/

	tupb->rdes = (PTU_RING) malloc_noown(sizeof(TU_RING) * tu_rcv_buf_no);

/*Allocate the timers*/

    tupb->tx_isr =
      (struct TIMERQ *) malloc_noown(sizeof(struct TIMERQ) * tu_xmt_buf_no);
    for (i = 0; i < tu_xmt_buf_no; i++) {
	sprintf(name, "txisr_%08x", (tupb->tdes)[i]);
	krn$_seminit(&(tupb->tx_isr[i].sem), 0, name);
	krn$_init_timer(&(tupb->tx_isr[i]));
    }

/*Initialize the port spinlock*/

    tupb->spl_port.value = 0;
    tupb->spl_port.req_ipl = IPL_SYNC;
    tupb->spl_port.sav_ipl = 0;
    tupb->spl_port.attempts = 0;
    tupb->spl_port.retries = 0;
    tupb->spl_port.owner = 0;
    tupb->spl_port.count = 0;

#if ( STARTSHUT || DRIVERSHUT )
/*Put the port block (inode) on the port block list*/

    pbe = (struct PBQ *) malloc_noown(sizeof(struct PBQ));
    pbe->pb = (int *) ip;
    pbe->pb = (int *) tupb;
    insq_lock(pbe, tupblist.blink);
#endif

/*Initialize the msg received list, added for callback support */

    spinlock(&spl_kernel);
    tupb->rcv_msg_cnt = 0;
    tupb->rqh.flink = &(tupb->rqh);
    tupb->rqh.blink = &(tupb->rqh);
    spinunlock(&spl_kernel);

/*Initialize the mop counters*/

    init_mop_counters(tupb->mc);

/*Init Local routine pointer*/

    tupb->lrp = 0;

/*Initialize register offset multiplier*/

    tupb->rm = 1;

/*Initialize some flags and counts*/

    tupb->state = TU_K_UNINITIALIZED;
    tupb->tu_flags_saved = 0;

/*Make sure the mode is read*/

    tu_read_mode(tupb);

/*Platform specific software init*/
/*Check for polled or not*/


    if ((tupb->pb.vector == 0) || ((IPL_RUN >= 0) && (IPL_RUN > 20))) {

/*Set up the poll queue*/

#if ( STARTSHUT || DRIVERSHUT )
	err_printf("No vector found port %s is running polled!\n",
	  tupb->pb.name);
#endif
	tupb->pqp.flink = &tupb->pqp.flink;
	tupb->pqp.blink = &tupb->pqp.flink;
	tupb->pqp.routine = tu_poll;
	tupb->pqp.param = np;
	insq_lock(&tupb->pqp.flink, &pollq);
    } else {
#if APS

/* Make sure the device is stopped  */
/* In case someone halts while OS ew */

	tu_init_device(tupb);
#endif
	int_vector_set(tupb->pb.vector, tu_interrupt, np);
	TULIP_ENABLE_INTERRUPT(tupb, tupb->pb.vector);
    }

#if !AUTO_SENSE

/* if a leftover auto mode is set change it to AUI */

    if (tupb->mode == TU_K_AUTO) {
	tupb->mode = TU_K_AUI;
	sprintf(var, "%4.4s_mode", ip->name);
	ev_write(var, "AUI", EV$K_STRING);
    }
#endif

/*Set to aui or twisted pair*/

    if (tupb->mode == TU_K_AUTO_NEGOTIATE) {
        ew_media = TU_K_OS_AN;
    } else if (tupb->mode == TU_K_TWISTED) {
	ew_media = TU_K_OS_TWISTED;
    } else if (tupb->mode == TU_K_TWISTED_FD) {
	ew_media = TU_K_OS_TWISTED_FD;
    } else if (tupb->mode == TU_K_AUI) {
	ew_media = TU_K_OS_AUI;
    } else if (tupb->mode == TU_K_BNC) {
	ew_media = TU_K_OS_BNC;
    } else if (tupb->mode == TU_K_FAST) {
	if (tupb->controller_type == DE500FA) 
		ew_media = TU_K_OS_FX;
	else
		ew_media = TU_K_OS_F;
    } else if (tupb->mode == TU_K_FASTFD) {
	if (tupb->controller_type == DE500FA) 
		ew_media = TU_K_OS_FFX;
	else
		ew_media = TU_K_OS_FFD;
    } else {
	ew_media = TU_K_OS_NOT_USED;
    }

    if (tupb->pb.type == TYPE_PCI)
	outcfgl(tupb, TU_PCFDA_ADDR, ew_media);


/*Initialize the environment variables*/

    mop_set_env(ip->name);
    ev->driver_flags = tu_driver_flags;
    ev->msg_buf_size = tu_msg_buf_size;
    ev->msg_mod = tu_msg_mod;
    ev->msg_rem = tu_msg_rem;
    ev->msg_prefix_size = 0;

/*Debug counts*/

    tupb->ti = tupb->tps = tupb->tu = tupb->tjt = 0;
    tupb->unf = tupb->ri = tupb->ru = tupb->rwt = 0;
    tupb->at = tupb->fd = tupb->lnf = tupb->se = tupb->tbf = 0;
    tupb->tto = tupb->lkf = tupb->ato = tupb->anc = tupb->aoc = 0;

/*Initialize the filter*/

    tupb->fcnt = 0;
    if (tu_driver_flags & NDL$M_BROADCAST)
	memset(tupb->filter[(tupb->fcnt)++], 0xff, 6);

#if !( STARTSHUT || DRIVERSHUT )

/*Initialize receive process*/

    sprintf(name, "rx_isr_%4.4s", ip->name);
    krn$_seminit(&(tupb->rx_isr), 0, name);
    sprintf(name, "rx_%4.4s", ip->name);
    krn$_create(tu_rx, null_procedure, 0, 6, -1, 1024 * 4, name, "tt", "r",
      "tt", "w", "tt", "w", ip);
#endif
}

/*+
 * ============================================================================
 * = tu_hardware_init - Initialization routine for the tulip hardware         =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routines will initialize the Tulip evaluation board hardware.
 *
 * FORM OF CALL:
 *
 *	tu_hardware_init (struct INODE *ip); 
 *
 * RETURNS:
 *
 *	int Status - msg_success or FALIURE.
 *
 * ARGUMENTS:
 *
 *	struct INODE *ip - INODE pointer.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int tu_hardware_init(struct INODE *ip)
{
    struct NI_GBL *np;			/* Pointer to the NI global database */
    struct TUPB *tupb;			/* Pointer to the port block */
    unsigned char nisa_rom[32];		/* Copy of the station address rom */
    unsigned long csr6;
    int i, status_message;

    dprintf("tu_hardware_init\n", p_args0);

/*Get some pointers*/

    np = (struct NI_GBL *) ip->misc;
    tupb = (struct TUPB *) np->pbp;
    tupb->controller_type = TU_21040;

/* 21140/21140A */
    if (tupb->pci_type == 0x91011 )  	   /* Use the de500-xa as default. */
       tupb->controller_type = DE500XA ; /* Differentiate later in microwire_rom */

    else if (tupb->pci_type == 0x191011 ) {
       tupb->nway_supported = 1; 
       tupb->fast_speed = 1; 
       tupb->full_duplex = 1;
       tupb->controller_type = TU_21143 ;
    }
    else if (tupb->pci_type == 0x21011 ) 	/* 21040 */
       tupb->controller_type = TU_21040 ;

    else if (tupb->pci_type == 0x141011 ) 	/* 21041 */
       tupb->controller_type = TU_21041 ;

    if ( (tupb->controller_type != TU_21040) && (tupb->controller_type != TU_EISA) )  {
       microwire_nirom(tupb, nisa_rom);
       status_message = tu_check_micro(tupb, nisa_rom);

       if (status_message != msg_success) {
          ni_error((char *)tupb->pb.alias, (char *)status_message);
          return (status_message);
       }
    }
    else {
/*Get the station address rom*/

       tu_get_nirom(tupb, nisa_rom);

/*Check the station address rom*/

       status_message = tu_check_nirom(tupb, nisa_rom);
    }


/*Put Station Address in HWRPB*/
#if YUKONA
if(tupb->controller_type != DE500XA)
    tu_get_nirom(tupb, nisa_rom);      
#endif

/*Configure the module*/

    tu_platform_init(tupb);

/*Make sure the device is stopped*/

    tu_init_device(tupb);

/*Set the device to the correct media port*/

    tu_init_media(tupb);

/*Set up the Station address*/

    for (i = 0; i < 6; i++)
	tupb->sa[i] = nisa_rom[i];
#if MEDULLA
    pprintf("Station Address: %02x-%02x-%02x-%02x-%02x-%02x\n", tupb->sa[0],
      tupb->sa[1], tupb->sa[2], tupb->sa[3], tupb->sa[4], tupb->sa[5]);
#endif

    dprintf("Station Address: %02x-%02x-%02x-%02x-%02x-%02x\n",
	p_args6(tupb->sa[0], tupb->sa[1], tupb->sa[2],
	tupb->sa[3], tupb->sa[4], tupb->sa[5]));

/*Add our address to the filter*/

    memcpy(tupb->filter[(tupb->fcnt)++], tupb->sa, 6);

/*The hardware is now initialized*/

    tupb->state = TU_K_HARDWARE_INITIALIZED;

/*Initialize the tx descriptors*/

    tu_init_tx(tupb);

/*Initialize the rx descriptors*/

    tu_init_rx(ip, tupb);

/*Send the setup frame and reset the link fail count*/

    tu_init_st(ip, tupb);

/*Start the receive process*/

    spinlock(&spl_kernel);
    csr6 = RD_TULIP_CSR_LONG(tupb, TU_CSR6_ADDR);
    WRT_TULIP_CSR_LONG(tupb, TU_CSR6_ADDR, csr6 | TU_CSR6_SR);
    spinunlock(&spl_kernel);

    return (msg_success);
}

/*+
 * ============================================================================
 * = tu_dump_csrs - Dump the tulip csrs.                                      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      Dump All the TULIP evaluation board csrs.
 *      
 * Arguments:
 *
 *	struct NI_GBL *np - Pointer to the NI global database.
 *
 * Return Value
 *      None
 */

void tu_dump_csrs(struct NI_GBL *np)
{
    struct TUPB *tupb;			/* Pointer to the port block */

/*Get some pointers*/

    tupb = (struct TUPB *) np->pbp;

/*Dump the Tulip evaluation board registers*/

    printf("TULIP CSRS:\n");
    printf("  Register Base: %08x\n\n", tupb->pb.csr);

/*EISA Configuration Registers*/

#if EISA_SUPPORT
#if DEBUG
    if (tupb->pb.type == TYPE_EISA) {

/*Change the register multiplier for a while*/

	tupb->rm = 1;

	printf("  EISA ID: %08x\n\n",
	  RD_TULIP_CSR_LONG(tupb, TU_EISA_ID_ADDR));

	printf("  EBC: %02x REG0: %02x REG1: %02x REG2: %02x REG3: %02x\n\n",
	  RD_TULIP_CSR_BYTE(tupb, TU_EBC_ADDR),
	  RD_TULIP_CSR_BYTE(tupb, TU_REG0_ADDR),
	  RD_TULIP_CSR_BYTE(tupb, TU_REG1_ADDR), RD_TULIP_CSR_BYTE(tupb,
	  TU_REG2_ADDR), RD_TULIP_CSR_BYTE(tupb, TU_REG3_ADDR));

	/*PCI Configuration registers*/

	printf("  CFID: %08x CFCS: %08x CFRV: %08x\n",
	  RD_TULIP_CSR_LONG(tupb, TU_CFID_ADDR), RD_TULIP_CSR_LONG(tupb,
	  TU_CFCS_ADDR), RD_TULIP_CSR_LONG(tupb, TU_CFRV_ADDR));
	printf("  CFLT: %08x CBIO: %08x\n\n", RD_TULIP_CSR_LONG(tupb,
	  TU_CFLT_ADDR), RD_TULIP_CSR_LONG(tupb, TU_CBIO_ADDR));

/*Change the register multiplier back*/

	tupb->rm = 2;
    }
#endif
#endif

/*Tulip registers*/

    printf("  CSR0: %08x CSR1: %08x CSR2: %08x\n",
      RD_TULIP_CSR_LONG(tupb, TU_CSR0_ADDR), RD_TULIP_CSR_LONG(tupb,
      TU_CSR1_ADDR), RD_TULIP_CSR_LONG(tupb, TU_CSR2_ADDR));
    printf("  CSR3: %08x CSR4: %08x CSR5: %08x\n",
      RD_TULIP_CSR_LONG(tupb, TU_CSR3_ADDR), RD_TULIP_CSR_LONG(tupb,
      TU_CSR4_ADDR), RD_TULIP_CSR_LONG(tupb, TU_CSR5_ADDR));
    printf("  CSR6: %08x CSR7: %08x CSR8: %08x\n",
      RD_TULIP_CSR_LONG(tupb, TU_CSR6_ADDR), RD_TULIP_CSR_LONG(tupb,
      TU_CSR7_ADDR), RD_TULIP_CSR_LONG(tupb, TU_CSR8_ADDR));
    printf("  CSR9: %08x CSR10:%08x CSR11:%08x\n",
      RD_TULIP_CSR_LONG(tupb, TU_CSR9_ADDR), RD_TULIP_CSR_LONG(tupb,
      TU_CSR10_ADDR), RD_TULIP_CSR_LONG(tupb, TU_CSR11_ADDR));
    printf("  CSR12:%08x CSR13:%08x CSR14:%08x\n",
      RD_TULIP_CSR_LONG(tupb, TU_CSR12_ADDR), RD_TULIP_CSR_LONG(tupb,
      TU_CSR13_ADDR), RD_TULIP_CSR_LONG(tupb, TU_CSR14_ADDR));
    printf("  CSR15:%08x\n", RD_TULIP_CSR_LONG(tupb, TU_CSR15_ADDR));
}

/*+
 * ============================================================================
 * = tu_platform_init - Platform hardware init.                               =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      This function is meant for platform specific init.
 *      
 * Arguments:
 *
 *      struct TUPB *tupb - Pointer to the port block.
 *
 * Return Value
 *      None
 */

void tu_platform_init(struct TUPB *tupb)
{
#if EISA_SUPPORT

/*If we are on the eisa bus init the module and change*/
/*the register multiplier*/

    if (tupb->pb.type == TYPE_EISA) {
     
/*If we have a vector use the ECU data*/
/*If not make up our own*/

	if (tupb->pb.vector) {

/*Enable the module*/

	    WRT_TULIP_CSR_BYTE(tupb, TU_EBC_ADDR, TU_EBC_ENA);
	    WRT_TULIP_CSR_LONG(tupb, TU_CFLT_ADDR, TU_CFLT_VALUE);
	    WRT_TULIP_CSR_LONG(tupb, TU_CBIO_ADDR, tupb->pb.csr | TU_CBIO_MIO);

	    use_eisa_ecu_data(&(tupb->pb));
	} else {
	    WRT_TULIP_CSR_BYTE(tupb, TU_REG0_ADDR, TU_EREG0_ITP |
	      TU_EREG0_IRQ3 | TU_EREG0_PR | TU_EREG0_BM | TU_EREG0_SIO);
	    WRT_TULIP_CSR_BYTE(tupb, TU_REG1_ADDR,
	      TU_EREG1_IDIS | TU_EREG1_US0);
	    krn$_sleep(500);
	    WRT_TULIP_CSR_BYTE(tupb, TU_REG1_ADDR, TU_EREG1_IDIS);
	    WRT_TULIP_CSR_BYTE(tupb, TU_REG2_ADDR, 0);
	    WRT_TULIP_CSR_BYTE(tupb, TU_REG3_ADDR, TU_EREG3_BR | TU_EREG3_BW);
	}

/*Enable the module*/

	WRT_TULIP_CSR_LONG(tupb, TU_CFCS_ADDR, TU_CFCS_IOA | TU_CFCS_MO);
	WRT_TULIP_CSR_BYTE(tupb, TU_EBC_ADDR, TU_EBC_ENA);
	WRT_TULIP_CSR_LONG(tupb, TU_CFLT_ADDR, TU_CFLT_VALUE);
	WRT_TULIP_CSR_LONG(tupb, TU_CBIO_ADDR, tupb->pb.csr | TU_CBIO_MIO);

/*Change the register multiplier*/

	tupb->rm = 2;
    }
#endif
}

/*+                      
 * ============================================================================
 * = tu_get_nirom - Provide a pointer to nirom. 		      	      =
 * ============================================================================
 *               
 * OVERVIEW:                                                                  
 *  
 *      This routine will perform platform specific functions to
 *	provide a pointer to the nirom.  
 *           
 * FORM OF CALL:                                                 
 *  
 *  	tu_get_nirom (tupb,ni);
 *              
 * RETURNS:
 *
 *	None
 *                    
 * ARGUMENTS:                           
 *
 *      struct TUPB *tupb - Pointer to the port block.
 *      unsigned char *ni - Pointer to a buffer to filled.
 *                          
-*/

#if ( STARTSHUT || DRIVERSHUT )
void tu_get_nirom(struct TUPB *tupb, unsigned char *ni)
{
    int i;
    unsigned char mod_id;

    dprintf("tu_get_nirom\n", p_args0);

/* Assume PCI adapter */

/* get the serial ROM contents via csr9 */

    tu_get_tulip_nirom(tupb, ni);

/*Tell the OS that its the serial ROM*/

    outcfgl(tupb, TU_CFDA_ADDR, 0);
}
#endif

#if MEDULLA 
void tu_get_nirom(struct TUPB *tupb, unsigned char *ni)
{
    int i;
    unsigned char mod_id;
    struct HWRPB *rpb = 0x2000;
    struct CONFIG *config;

    dprintf("tu_get_nirom\n", p_args0);

/* read in the entire e-net rom */

    mod_id = ((inportb(tupb, 0x2800)) >> 3) & 0x3;
    if ((mod_id == 0) && (tupb->pb.slot == TULIP_SLOT)) {
	for (i = 0; i < 32; i++)
	    ni[i] = inportb(tupb, ENET_ID_BASE + i * 4);
    } else {

/* get the serial ROM contents via csr9 */

	tu_get_tulip_nirom(tupb, ni);
    }

/* fill the SA into the HWRPB for OS to use during boot */

    config = (unsigned int) rpb + rpb->CONFIG_OFFSET[0];
    for (i = 0; i < 6; i++)
	config->SA[i] = ni[i];
}
#endif

#if CORTEX || YUKONA 
void tu_get_nirom (struct TUPB *tupb,unsigned char *ni)
{
 int i;
 struct HWRPB *rpb = 0x2000;
 struct CONFIG *config;


    dprintf("tu_get_nirom\n", p_args0);

#if !YUKONA
    tu_get_tulip_nirom (tupb, ni);
#endif


    /* fill the SA into the HWRPB for OS to use during boot */    
    config = (unsigned int)rpb + rpb->CONFIG_OFFSET[0];
    for (i = 0; i < 6; i++)
	config->SA[i] = ni[i];

}
#endif

#if SABLE || MIKASA || ALCOR
void tu_get_nirom(struct TUPB *tupb, unsigned char *ni)
{
    int i, j;
    unsigned char match_pattern[8];
    unsigned char nirom[64];

    dprintf("tu_get_nirom\n", p_args0);

/*Get the correct station address*/

    if (tupb->pb.type == TYPE_EISA) {

/*It's the eisa tulip card*/
/*Make the pattern we're looking for*/

	match_pattern[0] = 0xff;
	match_pattern[1] = 0x00;
	match_pattern[2] = 0x55;
	match_pattern[3] = 0xaa;
	match_pattern[4] = 0xff;
	match_pattern[5] = 0x00;
	match_pattern[6] = 0x55;
	match_pattern[7] = 0xaa;

/*Find the start of the ROM*/
/*Check up to 32 bytes worth (size of network address ROM)*/

	for (i = 0; i < 64; i++)
	    nirom[i] =
	      inportb(tupb, (unsigned __int64)(tupb->pb.csr + TU_SA_ADDR));

	for (i = 0; i < 32; i++) {
	    for (j = 0; j < 8; j++)
		if (nirom[i + j] != match_pattern[j])
		    break;
	    if (j == 8)
		break;
	}
	memcpy(ni, nirom + i + 8, 32);

    } else if (tupb->pb.slot == 0) {

/*It's the stdio TULIP*/

/*Try the serial rom if it's not there get the parallel rom*/

	tu_get_tulip_nirom(tupb, ni);
	if (tu_check_nirom(tupb, ni) == msg_success) {

/*Tell the OS that its the serial ROM*/

	} else {

/*Tell the OS that its the Parallel ROM*/
/*Get the address*/

	    for (i = 0; i < 32; i++)
		ni[i] = inportb(tupb, ENET_ID_ADDR + i);
	}
    } else {

/*It's the PCI TULIP card*/

	tu_get_tulip_nirom(tupb, ni);
    }
}
#endif

#if APS
void tu_get_nirom(struct TUPB *tupb, unsigned char *ni)
{
    int i;
    unsigned int rom_data;
    int timeout_value;

    unsigned char match_pattern[8];

    dprintf("tu_get_nirom\n", p_args0);

/*Get the correct station address*/

/*Make the pattern we're looking for*/

    match_pattern[0] = 0xff;
    match_pattern[1] = 0x00;
    match_pattern[2] = 0x55;
    match_pattern[3] = 0xaa;
    match_pattern[4] = 0xff;
    match_pattern[5] = 0x00;
    match_pattern[6] = 0x55;
    match_pattern[7] = 0xaa;

/* do a dummy write to reset the serial ROM */

    outmemb(tupb, tupb->pb.csr + ENET_ID_EISA_ADDR, 0);

/*Copy the ROM*/

    for (i = 0; i < 32; i += 1) {
	timeout_value = 500;

	do {
	    rom_data = inmeml(tupb, tupb->pb.csr + ENET_ID_EISA_ADDR);
	} while ((rom_data & 0x80000000) != 0 && timeout_value--);
	ni[i] = (char) (rom_data & 0xff);
    }
}
#endif

#if APC_PLATFORM || TAKARA || REGATTA || WILDFIRE || MONET || EIGER || MARVEL
void tu_get_nirom(struct TUPB *tupb, unsigned char *ni)
{
    dprintf("tu_get_nirom\n", p_args0);

    tu_get_tulip_nirom(tupb, ni);
#if 0
    outcfgl(tupb, TU_PCFDA_ADDR, 0);
#endif
}
#endif

/*+                      
 * ============================================================================
 * = tu_get_tulip_nirom - Provide a pointer using the tulip serial rom.	      =
 * ============================================================================
 *               
 * OVERVIEW:                                                                  
 *  
 *      This routine will get the ni rom using the tulip's csr9.
 *           
 * FORM OF CALL:                                                 
 *  
 *  	tu_get_tulip_nirom (tupb,ni);
 *              
 * RETURNS:
 *
 *	None
 *                    
 * ARGUMENTS:                           
 *
 *      struct TUPB *tupb - Pointer to the port block.
 *      unsigned char *ni - Pointer to a buffer to filled.
 *                          
-*/

void tu_get_tulip_nirom(struct TUPB *tupb, unsigned char *ni)
{
    int i, j;
    unsigned long csr9;

/*Start at the begining of the ROM*/

    WRT_TULIP_CSR_LONG(tupb, TU_CSR9_ADDR, 0);
    krn$_sleep(10);

/* Read in the entire e-net rom while checking data not valid*/

    for (i = 0; i < 32; i++)
	for (j = 0; j < TU_CSR9_DV_MAX; j++) {
	    csr9 = (unsigned char) RD_TULIP_CSR_LONG(tupb, TU_CSR9_ADDR);
	    krn$_sleep(10);
	    if (!(csr9 & TU_CSR9_DN)) {
		ni[i] = (unsigned char) csr9;
		break;
	    }
	    krn$_sleep(TU_CSR9_DV_WT);
	}
}

/*+                      
 * ============================================================================
 * = microwire_nirom - Provide a pointer using the tulip serial rom.	      =
 * ============================================================================
 *               
 * OVERVIEW:                                                                  
 *  
 *      This routine will get the ni rom using the tulip's csr9.
 *           
 * FORM OF CALL:                                                 
 *  
 *  	tu_get_tulip_nirom (tupb,ni);
 *              
 * RETURNS:
 *
 *	None
 *                    
 * ARGUMENTS:                           
 *
 *      struct TUPB *tupb - Pointer to the port block.
 *      unsigned char *ni - Pointer to a buffer to filled.
 *                          
-*/

int microwire_nirom(struct TUPB *tupb, unsigned char *ni)
{
    int i, j, k,l,d,w;
    unsigned char lni[128];
    unsigned long datain, dinbit, raddr, addrb, chksum;
    char nic_name[9];

    chksum = 0;
    k = 0;
    l=0;

    tupb->fast_speed = tupb->full_duplex = tupb->nway_supported = 0;

/* rajuls method */
	WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02800);
	krn$_micro_delay(1);
        for(i=0;i<25;i++)
         {
           WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02803);
	   krn$_micro_delay(1);
	   WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02801);
	   krn$_micro_delay(1);
         }
	WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02800);
	krn$_micro_delay(1);

/* output the command */

	for (i=0,j=0;j<20;j++)
         {
          for (k=0; k<2;k++)
            {
	     WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02805);
	     WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02807);
	     krn$_micro_delay(1);
	     WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02805);
	     krn$_micro_delay(1);
            }
          WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02801);
          WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02803);
          krn$_micro_delay(1);
          WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02801);
          krn$_micro_delay(1);

/* output the word address */
          for (k=0; k<6; k++)
            {
             d= ((j>> (5-k)) & 1) << 2;
	     WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02801 | d);
	     WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02803 | d);
	     krn$_micro_delay(1);
	     WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02801 | d);
	     krn$_micro_delay(1);
            }             
  
 /* input word of data */
          for (w=0,k=0; k<16; k++)
           {
	     WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02803);
	     krn$_micro_delay(1);
             dinbit=RD_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR) & 0x08;
             w|= ((dinbit >>3) & 1) << (15-k);
	     WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02801);
	     krn$_micro_delay(1);
           }


        if ((j>=10)&&(j<=12))
         {
	   ni[l++] = (unsigned char)(w&0x0ff);
	   ni[l++] = (unsigned char)((w&0x0ff00)>>8);
	 }

           lni[i++] = w&0xff;
           lni[i++] = w>>8;
          

	   WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02800);
	   krn$_micro_delay(1);
         } /* end for i */

	/* Store the the Adapter Name e.g. DE500-BA */
        for (w=0,k=29; w<8; w++,k++)
              nic_name[w] = lni[k];
        nic_name[w] = 0;

        if (strcmp(nic_name, "DE500-AA") == 0) {
	   tupb->controller_type = DE500AA;
           tupb->full_duplex=1; tupb->fast_speed=1; tupb->nway_supported=1;
	}
        if (strcmp(nic_name, "DE500-BA") == 0) {
	   tupb->controller_type = DE500BA;	/* DE500-BA and -FA use 21143 chip */
           tupb->full_duplex=1; tupb->fast_speed=1; tupb->nway_supported=1;
	}
        if (strcmp(nic_name, "DE500-FA") == 0) {
	   tupb->controller_type = DE500FA;	
           tupb->full_duplex=1; tupb->fast_speed=1;
	}
        if (strcmp(nic_name, "DE500-XA") == 0) {
           tupb->full_duplex=1; tupb->fast_speed=1; 
	}
	if ( strncmp((char *)&lni[29], "P2SEPLUS", 8) == 0) {
           tupb->controller_type = P2SEPLUS ;
	   tupb->full_duplex=1; tupb->fast_speed=1; 
	}
	if (tupb->controller_type == TU_21143)
	{   /* because the 21143 supports this and we haven't set it up */
	    tupb->full_duplex=1; tupb->fast_speed=1; tupb->nway_supported=1;
	}


return(msg_success);

}
#if 0 
/************************************************************************
 * This is the  method of reading the microwire EEROM from the specs.   *
 * This seems to have problems reading the EEROM on some of the DE450   *
 * and DE500 boards. I changed the algorithm used to be the same as     *
 * that used by  DEC UNIX (Rajul Shah) He got the code from DEC Israel. *
 ************************************************************************/
int microwire_nirom(struct TUPB *tupb, unsigned char *ni)
{
    int i, j, k,l,d,w;
    unsigned char lni[128];
    unsigned long datain, dinbit, raddr, addrb, chksum;
    char nic_name[9];

    chksum = 0;
    k = 0;
    l=0;

    tupb->fast_speed = tupb->full_duplex = tupb->nway_supported = 0;

    for (i = 0; i < 64; i++) {

/*Setup*/

	WRT_TULIP_CSR_LONG(tupb, TU_CSR9_ADDR, 0x04800);
	krn$_sleep(1);
	WRT_TULIP_CSR_LONG(tupb, TU_CSR9_ADDR, 0x04801);
	krn$_sleep(1);
	WRT_TULIP_CSR_LONG(tupb, TU_CSR9_ADDR, 0x04803);
	krn$_sleep(1);
	WRT_TULIP_CSR_LONG(tupb, TU_CSR9_ADDR, 0x04801);
	krn$_sleep(1);

/*command*/

	WRT_TULIP_CSR_LONG(tupb, TU_CSR9_ADDR, 0x04805);
	krn$_sleep(1);
	WRT_TULIP_CSR_LONG(tupb, TU_CSR9_ADDR, 0x04807);
	krn$_sleep(1);
	WRT_TULIP_CSR_LONG(tupb, TU_CSR9_ADDR, 0x04805);
	krn$_sleep(1);
	WRT_TULIP_CSR_LONG(tupb, TU_CSR9_ADDR, 0x04807);
	krn$_sleep(1);
	WRT_TULIP_CSR_LONG(tupb, TU_CSR9_ADDR, 0x04805);
	krn$_sleep(1);
	WRT_TULIP_CSR_LONG(tupb, TU_CSR9_ADDR, 0x04801);
	krn$_sleep(1);
	WRT_TULIP_CSR_LONG(tupb, TU_CSR9_ADDR, 0x04803);
	krn$_sleep(1);
	WRT_TULIP_CSR_LONG(tupb, TU_CSR9_ADDR, 0x04801);
	krn$_sleep(1);

/* address */

	raddr = (unsigned long) i;
	for (j = 5; j >= 0; j--) {
	    addrb = ((raddr >> j) & 0x0001) << 2;
	    WRT_TULIP_CSR_LONG(tupb, TU_CSR9_ADDR, (0x04801 | addrb));
	    krn$_sleep(1);
	    WRT_TULIP_CSR_LONG(tupb, TU_CSR9_ADDR, (0x04803 | addrb));
	    krn$_sleep(1);
	    WRT_TULIP_CSR_LONG(tupb, TU_CSR9_ADDR, (0x04801 | addrb));
	    krn$_sleep(1);
	}

/* data read */

	datain = 0;
	for (j = 15; j >= 0; j--) {
	    WRT_TULIP_CSR_LONG(tupb, TU_CSR9_ADDR, 0x04803);
	    krn$_sleep(1);

	    dinbit = RD_TULIP_CSR_LONG(tupb, TU_CSR9_ADDR);
	    datain = (datain << 1) | (dinbit & 0x08) >> 3;

	    krn$_sleep(1);
	    WRT_TULIP_CSR_LONG(tupb, TU_CSR9_ADDR, 0x04801);
	    krn$_sleep(1);
	}

	WRT_TULIP_CSR_LONG(tupb, TU_CSR9_ADDR, 0x04800);
	krn$_sleep(1);

	lni[i * 2] = (unsigned char) (datain & 0x0ff);
	lni[i * 2 + 1] = (unsigned char) ((datain & 0x0ff00) >> 8);
	if ((i >= 10) && (i <= 12)) {
	    ni[k++] = (unsigned char) (datain & 0x0ff);
	    ni[k++] = (unsigned char) ((datain & 0x0ff00) >> 8);
	}
    }
    return (msg_success);
}
#endif

/*+
 * ============================================================================
 * = tu_check_nirom - Tests the nirom.                                        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will test the ni rom. 
 *
 * FORM OF CALL:
 *
 *	tu_check_nirom (tupb,ni); 
 *
 * RETURNS:
 *
 *	int Status - msg_success or FAILURE.
 *
 * ARGUMENTS:
 *
 *      struct TUPB *tupb - Port block.
 *	unsigned char *ni - Pointer to the the ni rom.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int tu_check_nirom(struct TUPB *tupb, unsigned char *ni)
{
    int i, j, k;			/* index*/
    unsigned short t1;			/*Temporary locations*/
    unsigned char t2[4];
    unsigned long *t4;

    dprintf("tu_check_nirom\n", p_args0);

/*Verify that we don't have a null address*/

    t1 = 0;
    for (i = 0; i < 6; i++)
	t1 += (int) ni[i];
    if (!t1)
	return (msg_ez_err_nisa_null);

/*Verify the address checksum*/

    if (tu_nirom_checksum(ni) != msg_success)
	return (msg_ez_err_nisa_bad_chk);

/*Verify that the address is repeated again backwards*/

    for (i = 0, j = 15, k = 16; i < 8; i++, j--, k++)
	if (ni[j] != ni[k])
	    return (msg_ez_err_nisa_n_rpt);

/*Verify that the pattern ff,00,55,aa,ff,00,55,aa exists*/
/*Initialize some variables*/

    t4 = (unsigned long *) t2;
    for (i = 0, j = 24; i < 2; i++, j += 4) {
	t2[3] = ni[j];
	t2[2] = ni[j + 1];
	t2[1] = ni[j + 2];
	t2[0] = ni[j + 3];
	if ((int)*t4 != 0xff0055aa)
	    return (msg_ez_err_bad_patt);
    }
    return (msg_success);
}

/*+
 * ============================================================================
 * = tu_check_micro - Tests the microwire de500 nirom.                        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will test the ni rom. 
 *
 * FORM OF CALL:
 *
 *	tu_check_micro (tupb,ni); 
 *
 * RETURNS:
 *
 *	int Status - msg_success or FAILURE.
 *
 * ARGUMENTS:
 *
 *      struct TUPB *tupb - Port block.
 *	unsigned char *ni - Pointer to the the ni rom.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
 *	the format of the microwire eerom for the de500 is as follows:
 *
 *  byte offset  data                      description
 *---------------------------------------------------------------------------
 *    00          00 00 00 00 00 00 	reserved
 *                00 00 00 00 00 00
 *                00 00 00 00 00 00
 *    18	  01	                srom format version
 *    19	  01	                adapter count
 *    20          xx xx xx xx xx xx     IEEE network address
 *    
 *    26          00			adapter 0 device number      
 *    27	  41 00			adapter 0 leaf offset
 *    29	  44 45 35 30 30 2D     Adapter name DE500-XA (ABG info)
 *	          58 41 
 *    37          00 09			status (ABG info)
 *    39          YY YY YY YY YY YY     serial number in ascii (ABG info)
 *                YY YY YY YY
 *    49	  00 00 00 00 00 00	Reserves for future ABG use
 *		  00 00 00 00 00 00
 *		  00 00 00 00 00 00
 *		  00 00 00 00
 *    65	  00 08			Selected connection type (autosense)
 *    67	  0F			General purpose control
 *    68          04			media count
 *
 *    69(media#1) 00			Media code - tp (10Mb)
 *
 *    70          08			GP port data
 *
 *    71          9E 00			command
 *
 *
 *    73(media#2) 03			Media code - SYM_SCR(100MBaseTX)
 *
 *    74          09			GP port data
 *
 *    75          ED 00			command
 *
 *
 *    77(media#3) 05			Media code - SYM_SCR Full Duplex
 *
 *    78          01			GP port data
 *
 *    79          ED 80			command
 *
 *
 *    81(media#4) 04			Media code - TP Full Duplex
 *
 *    82          08			GP port data
 *
 *    83          9E 80			command
 *
 *    85          
 *    to
 *   125	  00...			Unused
-*/

int tu_check_micro(struct TUPB *tupb, unsigned char *ni)
{
    int i, j, k;			/* index*/
    unsigned short t1;			/*Temporary locations*/
    unsigned char t2[4];
    unsigned long *t4;

    dprintf("tu_check_micro\n", p_args0);

/*Verify that we don't have a null address*/

    t1 = 0;
    for (i = 0; i < 6; i++)
	t1 += (int) ni[i];
    if (!t1)
	return (msg_ez_err_nisa_null);

    return (msg_success);
}

/*+
 * ============================================================================
 * = tu_nirom_checksum - Looks at the nirom checksum.                         =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This function will calculate and test the nirom's checksum. 
 *
 * FORM OF CALL:
 *
 *	tu_nirom_checksum (ni); 
 *
 * RETURNS:
 *
 *	int Status - msg_success or FAILURE.
 *
 * ARGUMENTS:
 *
 *	unsigned char *ni - Pointer to the the ni rom.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int tu_nirom_checksum(unsigned char *ni)
{
    int i;
    unsigned long chksm;
    unsigned short *t3;
    unsigned char t2[4];

/*Verify the address checksum*/
/*Initialize some variables*/

    t3 = (unsigned short *) t2;
    chksm = 0;

/*Compute the partial address checksum*/

    for (i = 0; i < 6; i += 2) {

/*Advance the checksum*/

	chksm += chksm;

/*Make the 2 bytes a word*/

	t2[1] = ni[i];
	t2[0] = ni[i + 1];

/*Add it in*/

	chksm += *t3;
    }

/* Reduce SUM modulo 65535*/

    while (chksm >= 65535)
	chksm -= 65535;

/*Compare the computed checksum with loaded checksum*/
/*Get the loaded checksum*/

    t2[1] = ni[6];
    t2[0] = ni[6 + 1];
    if (chksm != *t3)
	return (msg_ez_err_nisa_bad_chk);
    else
	return (msg_success);
}

/*+
 * ============================================================================
 * = tu_init_device - Initialize tulip.                                       =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will initialize the tulip device.
 *
 * FORM OF CALL:                
 *
 *	tu_init_device (tupb,mode);    
 *
 * RETURNS:
 *
 *	int Status - msg_success or FAILURE.
 *
 * ARGUMENTS:
 *
 *	struct TUPB *tupb - Pointer to the tu port block.
 *
 * SIDE EFFECTS:          
 *
 *      None                    
 *
-*/

int tu_init_device(struct TUPB *tupb)
{
    unsigned long rev;
    unsigned long scratch;

    dprintf("tu_init_device\n", p_args0);

/*Reset the tulip*/

    if (tupb->controller_type == DE500BA || tupb->controller_type == TU_21143) {
    spinlock(&spl_kernel);
    WRT_TULIP_CSR_LONG(tupb, TU_CSR0_ADDR, TU_CSR0_SWR);
    spinunlock(&spl_kernel);
    krn$_sleep(5);
    }

    if (tupb->controller_type == DE500AA ) /* Required for DE500-AA */
	WRT_TULIP_CSR_LONG(tupb, TU_CSR6_ADDR, TU_CSR6_PS);

    if (tupb->pb.type != TYPE_EISA) {
	rev = incfgl(tupb, TU_PCFRV_ADDR);
	if ((tupb->pci_type != 0x00021011) || ((rev & TU_CFRV_ID) >= 0x20)) {
	    WRT_TULIP_CSR_LONG(tupb, TU_CSR0_ADDR, TU_CSR0_SWR);
	    krn$_sleep(5);
	}

	scratch = incfgl(tupb, 0x0);
	dprintf("Tulip device vendor id: %08x\n", p_args1(scratch));

	scratch = incfgl(tupb, 0x10);
	dprintf("Tulip I/O base address: %08x\n", p_args1(scratch));

	scratch = incfgl(tupb, 0x14);
	dprintf("Tulip memory base address: %08x\n", p_args1(scratch));

	scratch = incfgl(tupb, 0x4);
	dprintf("Tulip command/status reg: %08x\n", p_args1(scratch));
    }

    WRT_TULIP_CSR_LONG(tupb, TU_CSR0_ADDR, TU_CSR0_PBL8 | TU_CSR0_CAL1);

    WRT_TULIP_CSR_LONG(tupb, TU_CSR13_ADDR, tupb->csr13);

/*Write the serial command register*/

    WRT_TULIP_CSR_LONG(tupb, TU_CSR6_ADDR, tu_ser_com);

/*Write the interrupt mask register*/
/*Enable all the interrupts, unless running polled*/

    if (tupb->pb.vector) {
	WRT_TULIP_CSR_LONG(tupb, TU_CSR7_ADDR, 0xc01ffff);
    } else {
	WRT_TULIP_CSR_LONG(tupb, TU_CSR7_ADDR, 0xc007fff);
    }

    return (msg_success);
}

/*+
 * ============================================================================
 * = tu_init_tx - Initialization routine for TX descriptors.                  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will init the transmit descriptors.
 *
 * FORM OF CALL:
 *
 *	tu_init_tx (tupb); 
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	struct TUPB *tupb - Pointer to the tu port block.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void tu_init_tx(struct TUPB *tupb)
{
    char name[20];			/*General holder for a name*/
    int i;
    char *tdes_dma_ptr;

/*Initialize the indices*/

    tupb->tx_index_in = tupb->tx_index_out = 0;

/*Fill in the descriptors*/

    for (i = 0; i < tu_xmt_buf_no; i++) {

/*Descriptors*/

	(tupb->tdes)[i].tu_flags = 0;
	(tupb->tdes)[i].tu_info = 0;
	(tupb->tdes)[i].tu_bfaddr1 = 0;
	(tupb->tdes)[i].tu_bfaddr2 = 0;
    }

/*Tell the tulip where they are*/

    tdes_dma_ptr = (char *) ((u_long) tupb->tdes + tu_window_base);
    WRT_TULIP_CSR_LONG(tupb, TU_CSR4_ADDR, tdes_dma_ptr);

    dprintf("xmt descriptor: %08x\n", p_args1(tupb->tdes));
    dprintf("xmt_buf_no: %d\n", p_args1(tu_xmt_buf_no));
    dprintf("xmt_mod: %08x xmt_rem: %08x\n", p_args2(tu_xmt_mod, tu_xmt_rem));
}

/*+
 * ============================================================================
 * = tu_init_rx - Initialization routine for RX descriptors.                  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will init the receive descriptors.
 *
 * FORM OF CALL:
 *
 *	tu_init_rx (tupb); 
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	struct TUPB *tupb - Pointer to the tu port block.
 *	struct INODE *ip - INODE pointer.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void tu_init_rx(struct INODE *ip, struct TUPB *tupb)
{
    int i;
    char *rdes_dma_ptr;

/*Initialize the indices*/

    tupb->rx_index = 0;

/*Fill in the descriptors and tx info packets*/

    for (i = 0; i < (tu_rcv_buf_no - 1); i++) {

/*Descriptors*/

	(tupb->rdes)[i].tu_flags = TU_OWN;
	(tupb->rdes)[i].tu_info = NDL$K_MAX_PACK_LENGTH;
	if (!((tupb->rdes)[i].tu_bfaddr1))
	    (tupb->rdes)[i].tu_bfaddr1 = ALLOCATE_PKT(ip, 1);
	(tupb->rdes)[i].tu_bfaddr2 = 0;
    }

/*Fill in the chain descriptor*/

    (tupb->rdes)[i].tu_flags = TU_OWN;
    (tupb->rdes)[i].tu_info = TU_RER | NDL$K_MAX_PACK_LENGTH;
    if (!((tupb->rdes)[i].tu_bfaddr1))
	(tupb->rdes)[i].tu_bfaddr1 = ALLOCATE_PKT(ip, 1);
    (tupb->rdes)[i].tu_bfaddr2 = 0;

/*Tell the tulip where they are*/

    rdes_dma_ptr = (char *) ((u_long) tupb->rdes + tu_window_base);
    WRT_TULIP_CSR_LONG(tupb, TU_CSR3_ADDR, rdes_dma_ptr);

    dprintf("rcv descriptor: %08x\n", p_args1(tupb->rdes));
    dprintf("rcv_buf_no: %d\n", p_args1(tu_rcv_buf_no));
    dprintf("rcv_mod: %08x rcv_rem: %08x\n", p_args2(tu_rcv_mod, tu_rcv_rem));
}


/* ============================================================================
 * = tu_init_st - Initialization routine for the setup frame.                 =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will setup and send the setup frame.
 *
 * FORM OF CALL:
 *
 *	tu_init_st (tupb); 
 *
 * RETURNS:
 *
 *	int Status - msg_success or FAILURE.
 *
 * ARGUMENTS:
 *
 *	struct TUPB *tupb - Pointer to the tu port block.
 *	struct INODE *ip - INODE pointer.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void tu_init_st(struct INODE *ip, struct TUPB *tupb)
{
    unsigned char *setup_frame, *setup_frame_dma;
					/*Setup frame*/
    unsigned long csr6;
    int i;

/*Get a pointer to the setup frame*/

    setup_frame_dma = ALLOCATE_PKT(ip, 1);
    setup_frame = (char *) ((u_long) setup_frame_dma - tu_window_base);

/*Build a perfect filter*/

    tu_build_perfect(tupb, setup_frame);

#if DEBUG
    d10printf("Filter:\n", p_args0);
    for (i = 0; i < 16 * 3; i++)
	d10printf("%08x %08x\n", p_args2(&(setup_frame[i * 4]), *(unsigned long *) &(setup_frame[i * 4])));
#endif

/*Start the transmit process*/

    spinlock(&spl_kernel);
    csr6 = RD_TULIP_CSR_LONG(tupb, TU_CSR6_ADDR);
    WRT_TULIP_CSR_LONG(tupb, TU_CSR6_ADDR, csr6 | TU_CSR6_ST);
    spinunlock(&spl_kernel);

/*Send the setup frame*/

    tu_send(ip, tupb, setup_frame_dma, TU_SETUP_FRAME_LEN, 1);
}

/*+
 * ============================================================================
 * = tu_build_perfect - Builds a setup frame for a perfect filter.            =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will build the setup for a perfect filter. 
 *
 * FORM OF CALL:
 *
 *	tu_build_perfect (tupb,setup_frame,driver_flags); 
 *
 * RETURNS:
 *
 *	int size - Size of the frame built.
 *
 * ARGUMENTS:
 *
 *	struct TUPB *tupb - Pointer to the tu port block.
 *	char *setup_frame - Pointer to the setup frame to be filled in.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void tu_build_perfect(struct TUPB *tupb, char *setup_frame)
{
    int i, j, k;

/*Clear the frame*/

    memset(setup_frame, 0, TU_PERFECT_MAX * (4 * 3));

/*Build the frame*/

    for (i = 0; i < tupb->fcnt; i++)
	for (j = 0; j < 6; j += 2)
	    for (k = 0; k < 2; k++)
		setup_frame[(i * 12) + (j * 2) + k] = tupb->filter[i][j + k];

/*Fill the reset in with a valid address*/

    for (; i < TU_PERFECT_MAX; i++)
	for (j = 0; j < 6; j += 2)
	    for (k = 0; k < 2; k++)
		setup_frame[(i * 12) + (j * 2) + k] = tupb->filter[0][j + k];
}

/*+                      
 * ============================================================================
 * = tu_poll - tulip polling routine.                          	              =
 * ============================================================================
 *               
 * OVERVIEW:                                                                  
 *  
 *      This routine is the polling routine for the tulip driver.
 *	It is called from the timer task. It will simply check for
 *	an interrupt and call the isr if there is one.
 *           
 * FORM OF CALL:                                                 
 *  
 *  	tu_poll (tupb);
 *              
 * RETURNS:
 *
 *		None
 *                    
 * ARGUMENTS:                           
 *
 *	struct NI_GBL *np - pointer to ni block.
 *                          
-*/

void tu_poll(struct NI_GBL *np)
{
    struct TUPB *tupb;			/*Pointer to the port block*/

/*Get some pointers*/

    tupb = (struct TUPB *) np->pbp;

/*If the hardware is still not initialized return*/

    if (tupb->state == TU_K_UNINITIALIZED)
	return;

    if (tupb->state == TU_K_STOPPED)
	return;

/*If we have an interrupt call the isr*/

    if (RD_TULIP_CSR_LONG(tupb, TU_CSR5_ADDR) & (TU_CSR5_NIS | TU_CSR5_AIS))
	tu_interrupt(np);
}

/*+
 * ============================================================================
 * = tu_interrupt - tu interrupt routine.                                     =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine processes the tulip interrupts. 
 *
 * FORM OF CALL:   
 *
 *	tu_interrupt (np); 
 *
 * RETURNS:
 *
 *	None
 *                 
 * ARGUMENTS:
 *
 *	struct NI_GBL *np - pointer to ni block.
 *
 * SIDE EFFECTS:
 *
 *      None       
 *
-*/

void tu_interrupt(struct NI_GBL *np)
{
    struct TUPB *tupb;			/*Pointer to the port block*/
    unsigned long csr5, csr6;
    unsigned long clear_all = 0x0017aa;

    d8printf("tu_interrupt\n", p_args0);

/*Get some pointers*/

    tupb = (struct TUPB *) np->pbp;

    /*Look at the type of interrupt*/

    csr5 = RD_TULIP_CSR_LONG(tupb, TU_CSR5_ADDR);

    while (csr5 & (TU_CSR5_NIS | TU_CSR5_AIS)) {

    /*Clear the interrupts*/

	WRT_TULIP_CSR_LONG(tupb, TU_CSR5_ADDR, ( csr5 | clear_all ) );

#if 1

/*Handle Abnormal interrupts*/
/* Report only a fatal bus error then issue a software reset */

	    if (csr5 & (TU_CSR5_SE) ) {
		pprintf("TU PCI Bus Error :CSR5: %08x\n", csr5);
                WRT_TULIP_CSR_LONG(tupb, TU_CSR0_ADDR, TU_CSR0_SWR);
                abnormal_intr_error_flag = 1;
	    }
#endif

/*Test the specific interrupts*/

	if (csr5 & TU_CSR5_TI) {

/*Process the transmit descriptors*/

	    tupb->ti++;
	    tu_process_tx(tupb);
	}
	if (csr5 & TU_CSR5_RI) {

/*Post the receive semaphore*/

	    tupb->ri++;
	    krn$_bpost(&(tupb->rx_isr));
	}
	if (csr5 & TU_CSR5_TPS)
	    tupb->tps++;
	if (csr5 & TU_CSR5_TU)
	    tupb->tu++;
	if (csr5 & TU_CSR5_TJT)
	    tupb->tjt++;
	if (csr5 & TU_CSR5_UNF)
	    tupb->unf++;
	if (csr5 & TU_CSR5_RU)
	    tupb->ru++;
	if (csr5 & TU_CSR5_RPS)
	    tupb->rps++;
	if (csr5 & TU_CSR5_RWT)
	    tupb->rwt++;
	if (csr5 & TU_CSR5_AT)
	    tupb->at++;
	if (csr5 & TU_CSR5_FD)
	    tupb->fd++;
	if (csr5 & TU_CSR5_LNF)
	    tupb->lnf++;
	if (csr5 & TU_CSR5_SE)
	    tupb->se++;

/*Check for interrupts again*/

	csr5 = RD_TULIP_CSR_LONG(tupb, TU_CSR5_ADDR);
    }

#if SABLE || AVANTI || MIKASA || ALCOR || MTU || M1543C || M1535C
    if (tupb->pb.vector != 0)
	io_issue_eoi(tupb, tupb->pb.vector);
#endif
}

/*+
 * ============================================================================
 * = tu_show - Tulips show routine.                                           =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine is used to display the tulip's information.
 *
 * FORM OF CALL:
 *
 *	tu_show (np); 
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	struct NI_GBL *np - Pointer to the NI global database.
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/

void tu_show(struct NI_GBL *np)
{
    struct TUPB *tupb;			/*Pointer to the port block*/

/*Get some pointers*/

    tupb = (struct TUPB *) np->pbp;

/*Display some status*/

    printf("Status counts:\n");
    printf("ti: %d tps: %d tu: %d tjt: %d unf: %d ri: %d ru: %d\n", tupb->ti,
      tupb->tps, tupb->tu, tupb->tjt, tupb->unf, tupb->ri, tupb->ru);
    printf("rps: %d rwt: %d at: %d fd: %d lnf: %d se: %d tbf: %d\n", tupb->rps,
      tupb->rwt, tupb->at, tupb->fd, tupb->lnf, tupb->se, tupb->tbf);
    printf("tto: %d lkf: %d ato: %d nc: %d oc: %d\n", tupb->tto, tupb->lkf,
      tupb->ato, tupb->anc, tupb->aoc);
}

/*+
 * ============================================================================
 * = tu_send - Send a message.			                              =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine sends a packet. It will handle chaining, deallocating
 *	of the packet, callbacks and calling of the out routine.
 *
 * FORM OF CALL:
 *
 *	tu_send (ip,tupb,buf,len,setup); 
 *
 * RETURNS:
 *
 *	int size - the number of bytes transmitted.
 *
 * ARGUMENTS:
 *
 *	struct INODE *ip - INODE pointer.
 *      struct TUPB *tupb - Pointer to the port block.
 *	char *buf - Buffer to be sent. 
 *	int len - Number of bytes to be sent. 
 *	int setup - Send a setup frame or not
 *			1 = This is a setup frame.
 *			0 = This is not a setup frame.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int tu_send(struct INODE *ip, struct TUPB *tupb, char *buf, int len, int setup)
{
    uint64 sc;
    int bytes;
    u_long scratch;

    dprintf("SEND: buf: %08x len: %08x setup: %d\n", p_args3(buf, len, setup));

/*Save the current short circuit count*/

    sc = tupb->mc->MOP_V4CNT_TX_FAIL_SHRT_CIRCUIT;

/*Transmit the packet*/

    bytes = tu_out(tupb, buf, len, setup);

/*Check our network connection*/

#if AUTO_SENSE
    if (!setup)
	tu_check_mode(tupb);
#endif

/*Deallocate packet*/

    if (!cb_ref) {
	scratch = (u_long) buf - tu_window_base;
	ndl_deallocate_pkt(ip, (char *) scratch);
    }

/*Return the number of bytes sent*/

    return (bytes);
}

#if MEDULLA
/*+
 * ============================================================================
 * = tu_out - Add a packet to the descriptor transmit list.                   =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine sends a packet. It will put the message on the transmit
 *	descriptor list set the ownership and return when the transmit is
 *	complete. 
 *
 * FORM OF CALL:
 *
 *	int tu_out (tupb,buf,len,setup); 
 *
 * RETURNS:
 *
 *	int size - the number of bytes transmitted.
 *
 * ARGUMENTS:
 *
 *      struct TUPB *tupb - Pointer to the port block.
 *	char *buf         - Buffer to be sent. 
 *	int len           - Number of bytes to be sent. 
 *	int setup         - Send a setup frame or not.
 *                             1 = This is a setup frame.
 *                             0 = This is not a setup frame.
 *
 * NOTES:
 *
 *    'tx_index_in' is the next free entry in a RING BUFFER.
 *                  When we send something out, we use this
 *                  as an index to store the request.
 *
 *    'tx_index_out' is the index into the RING BUFFER that
 *                  is next to be transmit.
 *
 *    'tu_xmt_buf_no' is the size in elements of the RING BUFFER.
 *
 *    Therefore:
 *
 *            if ( in == out ) the RING BUFFER is empty
 *            - or -
 *            if ( (in+1)%size != out ) the RING BUFFER can hold
 *                                      at least one more entry
 *            - or -
 *            the RING BUFFER is full.
 *
 *
 * SIDE EFFECTS:
 *
 *      None.
 *
-*/

int tu_out(struct TUPB *tupb, char *buf, int len, int setup)
{
    unsigned long csr6;
    PTU_RING tdes;
    unsigned int index;
    int i;
    int status;
    int avail;				/* true if a descriptor is available */
    int retry;				/* true if we should retry           */

    dprintf("Out: buf: %08x len: %08x setup: %d\n", p_args3(buf, len, setup));
#if DEBUG
    if (tulip_debug & 4)
	pprint_pkt(buf, len);
#endif

/* set retry count to 1 */

    retry = 1;
loop: 

/* Take out the spinlock */

    spinlock(&(tupb->spl_port));

/* See if any descriptors are available */

    if (tupb->tx_index_out == tupb->tx_index_in)
	avail = 1;
    else if ((tupb->tx_index_in + 1) % tu_xmt_buf_no != tupb->tx_index_out)
	avail = 1;
    else
	avail = 0;

    if (!avail) {

/* Bump the counts */

	tupb->mc->MOP_V4CNT_NO_USER_BUFFER++;
	tupb->tbf++;

/* release the spin lock */

	spinunlock(&(tupb->spl_port));

/* console log the event */

	dqprintf("tu_out: No descriptors available, cb_ref = %d\n", cb_ref);
	dqprintf("  tx_index_in = %x\n", tupb->tx_index_in);
	dqprintf("  tx_index_out = %x\n", tupb->tx_index_out);
	for (i = 0; i <= tu_xmt_buf_no; i++) {
	    tdes = &(tupb->tdes[i]);
	    dqprintf("  status tdes%d:%08x\n", i, tdes->tu_flags);
	}

/* should we retry ??? */

	if (retry-- > 0) {

/* yes - retry */

	    tu_process_tx(tupb);	/* try and clear out some descriptors
					 */
	    goto loop;			/* and retry
					 *                          */
	} else {

/* no - no retry - return error */

	    return (0);
	}
    }

/* Start the timer */

    tupb->tx_isr[tupb->tx_index_in].sem.count = 0;
    tupb->tx_isr[tupb->tx_index_in].alt = FALSE;
    krn$_start_timer(&(tupb->tx_isr[tupb->tx_index_in]), 1000);

/* Get the descriptor pointer,and index */

    tdes = &(tupb->tdes[tupb->tx_index_in]);
    index = tupb->tx_index_in;

/* Add the message to the list */

    if (setup)
	tdes->tu_info = len | TU_SET | TU_TDES_FS | TU_TDES_LS | TU_IC;
    else
	tdes->tu_info = len | TU_TDES_FS | TU_TDES_LS | TU_IC;
    tdes->tu_bfaddr1 = buf;
    tdes->tu_bfaddr2 = 0;
    mb();
    tdes->tu_flags = TU_OWN;

/* Point to the next descriptor and release the spinlock */

    if (tupb->tx_index_in == (tu_xmt_buf_no - 1))
	tdes->tu_info |= TU_TER;
    tupb->tx_index_in++;
    tupb->tx_index_in %= tu_xmt_buf_no;
    spinunlock(&(tupb->spl_port));

    dprintf("Transmit queued %08x: %08x %08x %08x %08x\n",
	p_args5(tdes, tdes->tu_flags, tdes->tu_info, tdes->tu_bfaddr1, tdes->tu_bfaddr2));

/* Set the transmit poll */

    WRT_TULIP_CSR_LONG(tupb, TU_CSR1_ADDR, 1);

/* Wait for the packet to be processed */

    krn$_wait(&(tupb->tx_isr[index].sem));

    if (!tupb->tx_isr[index].alt) {
	krn$_stop_timer(&(tupb->tx_isr[index]));
	tupb->tto++;
	tupb->ato++;
	len = 0;
    }

/* Return the length */

    return (len);
}
#else					/* MEDULLA */

/*+
 * ============================================================================
 * = tu_out - Add a packet to the descriptor list.                            =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine sends a packet. It will put the message on the transmit
 *	descriptor list set the ownership and return when the transmit is
 *	complete. 
 *
 * FORM OF CALL:
 *
 *	tu_out (tupb,buf,len,setup); 
 *
 * RETURNS:
 *
 *	int size - the number of bytes transmitted.
 *
 * ARGUMENTS:
 *
 *      struct TUPB *tupb - Pointer to the port block.
 *	char *buf - Buffer to be sent. 
 *	int len - Number of bytes to be sent. 
 *	int setup - Send a setup frame or not.
 *			1 = This is a setup frame.
 *			0 = This is not a setup frame.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int tu_out(struct TUPB *tupb, char *buf, int len, int setup)
{
    unsigned long csr6;
    PTU_RING tdes;
    unsigned int index;
    int i, status;

    dprintf("Out: buf: %08x len: %08x setup: %d\n", p_args3(buf, len, setup));
#if DEBUG
    if (tulip_debug & 4)
	pprint_pkt(buf, len);
#endif

/*Take out the spinlock*/

    spinlock(&(tupb->spl_port));

/*See if any descriptors are available*/

    if (((tupb->tx_index_in - tupb->tx_index_out) % tu_xmt_buf_no) >
      (tu_xmt_buf_no - 2)) {

/*Bump the counts*/

	tupb->mc->MOP_V4CNT_NO_USER_BUFFER++;
	tupb->tbf++;

#if 0
	tu_process_tx(tupb);		/* try and clear out some descriptors
					 */
	if (((tupb->tx_index_in - tupb->tx_index_out) % tu_xmt_buf_no) >
	  (tu_xmt_buf_no - 2)) {

/*Bump the counts*/

	    spinunlock(&(tupb->spl_port));
	    dqprintf("No descriptors available\n");
	    dqprintf("tx_index_in = %x\n", tupb->tx_index_in);
	    dqprintf("tx_index_out = %x\n", tupb->tx_index_out);
	    for (i = 0; i <= tu_xmt_buf_no; i++) {
		tdes = &(tupb->tdes[i]);
		dqprintf("status tdes%d:%08x\n", i, tdes->tu_flags);
	    }

/*Return a failure*/

	    return (0);
	}
#else

/*Return a failure*/

	spinunlock(&(tupb->spl_port));
	return (0);
#endif
    }

/*Start the timer*/

    tupb->tx_isr[tupb->tx_index_in].sem.count = 0;
    tupb->tx_isr[tupb->tx_index_in].alt = FALSE;
    krn$_start_timer(&(tupb->tx_isr[tupb->tx_index_in]), 1000);

/*Get the descriptor pointer,and index*/

    tdes = &(tupb->tdes[tupb->tx_index_in]);
    index = tupb->tx_index_in;

/*Add the message to the list*/

    if (setup)
	tdes->tu_info = len | TU_SET | TU_TDES_FS | TU_TDES_LS | TU_IC;
    else
	tdes->tu_info = len | TU_TDES_FS | TU_TDES_LS | TU_IC;
    tdes->tu_bfaddr1 = buf;
    tdes->tu_bfaddr2 = 0;
    mb();
    tdes->tu_flags = TU_OWN;

/*Point to the next descriptor and release the spinlock*/

    if (tupb->tx_index_in == (tu_xmt_buf_no - 1))
	tdes->tu_info |= TU_TER;
    tupb->tx_index_in++;
    tupb->tx_index_in %= tu_xmt_buf_no;
    spinunlock(&(tupb->spl_port));

    dprintf("Transmit queued %08x: %08x %08x %08x %08x\n",
	p_args5(tdes, tdes->tu_flags, tdes->tu_info, tdes->tu_bfaddr1, tdes->tu_bfaddr2));

/*Set the transmit poll*/

    WRT_TULIP_CSR_LONG(tupb, TU_CSR1_ADDR, 1);

/*Wait for the packet to be processed*/

    krn$_wait(&(tupb->tx_isr[index].sem));

    if (!tupb->tx_isr[index].alt) {
	krn$_stop_timer(&(tupb->tx_isr[index]));
	tupb->tto++;
	tupb->ato++;
	len = 0;
    }

/*Return the length*/

    return (len);
}
#endif					/* MEDULLA */

/*+
 * ============================================================================
 * = tu_open - Open the EW (Tulip aka dc287) device.                                =
 * ============================================================================
 *
 * OVERVIEW:         
 *  
 *      Open the EW (Tulip aka dc287) device.
 *  	8/22/97 Add the ability to send a new frame with a multicast address
 *   
 * FORM OF CALL:
 *  
 *	ew_open (fp, multicast-string)
 *  
 * RETURNS:
 *
 *	msg_success - Success
 *       
 * ARGUMENTS:
 *
 *	struct FILE *fp - File to be opened.
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/

int tu_open(struct FILE *fp, char *multicast_str)
{
    int valid;
    unsigned char *multicast_addr;
    struct INODE  *ip;	                   
    struct TUPB   *tupb;		/* Pointer to the port block */
    struct NI_GBL *np;			/* Pointer to the NI global database */



    if (multicast_str && (multicast_str[0] != '\0')) {
    	ip = (struct INODE *)  fp->ip;
        np = (struct NI_GBL *) ip->misc;
        tupb = (struct TUPB *) np->pbp;

        multicast_addr = tupb->filter[(tupb->fcnt)++];  
        valid = convert_enet_address(multicast_addr,multicast_str);

        /* send a setup frame if a valid multicast address exists  */

        if ( (valid == msg_success) && (multicast_addr[0] & 0x01) ) {
	   pprintf("Display converted mc address: %02x-%02x-%02x-%02x-%02x-%02x \n", 
            multicast_addr[0],multicast_addr[1],multicast_addr[2],
            multicast_addr[3],multicast_addr[4],multicast_addr[5]);

           tu_init_st(ip, tupb);
        }
    }
    return (msg_success);
}

/*+
 * ============================================================================
 * = tu_read - ew port read routine.                                          =
 * ============================================================================
 *
 * OVERVIEW:         
 *  
 *      Used primarily for ew port callbacks.
 *  
 * FORM OF CALL:
 *  
 *	tu_read (fp,size,number,c);
 *  
 * RETURNS:
 *
 *	number of bytes read
 *      0 - If EOF or error (fp->status = failure message)
 *                          (fp->code_entry = boot start address)
 *       
 * ARGUMENTS:
 *
 *	struct FILE *fp - Pointer to a file.
 *	int size - Used to test for buffer size.
 *	int number - Used to test for buffer size.
 *	unsigned char *c - For MOP load the base of where this is to be loaded.
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/

int tu_read(struct FILE *fp, int size, int number, unsigned char *c)
{
    struct NI_GBL *np;			/*Pointer to NI global database*/
    struct TUPB *tupb;			/*Pointer to the port block*/
    struct EW_MSG *mr;			/*Pointer to the message received*/
    int len, retries;
    int callback_timeout = 100;
    int callback_wait = 10;


    dprintf("tu_read: size: %d number: %d\n", p_args2(size, number));

/*Get the pointer to the port block*/

    np = (struct NI_GBL *) fp->ip->misc;
    tupb = (struct TUPB *) np->pbp;

/*See if there is a message available*/

    if ( cb_ref ) 
       callback_timeout = 2;

#if    TAKARA | K2 | CORTEX | YUKONA
       callback_timeout = 100;
#endif

    for (retries = 0; retries < callback_timeout; retries++) {
	spinlock(&spl_kernel);
	if (tupb->rqh.flink != (struct EW_MSG *) &(tupb->rqh.flink)) {

/*Get the message*/

	    mr = (struct EW_MSG *) remq(tupb->rqh.flink);
	    tupb->rcv_msg_cnt--;
	    spinunlock(&spl_kernel);
	    break;
	} else

/*There isn't so just unlock*/

	    spinunlock(&spl_kernel);

/*Wait a little*/

	krn$_sleep(callback_wait);
    }

/*If there is no message available return 0*/

    if (retries == callback_timeout)
	return (0);

/*Copy the info*/

    len = min(mr->size, size * number);
    memcpy(c, mr->msg, len);

/*Free the holder and packet*/

    ndl_deallocate_pkt(fp->ip, mr->msg);
    free(mr);

/*Return the size*/

    dprintf("Callback message received.\n", p_args0);
#if DEBUG
    if (tulip_debug & 4)
	pprint_pkt(c, len);
#endif
    return (len);
}

/*+
 * ============================================================================
 * = tu_rx - Receive process.                                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine is actually a process that is created at init time.
 *	It processes receive descriptors. 
 *
 * FORM OF CALL:
 *
 *	tu_rx (ip); 
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	struct INODE *ip - Pointer to the Device's inode.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void tu_rx(struct INODE *ip)
{
    struct NI_GBL *np;			/*Pointer to the NI global database*/
    struct TUPB *tupb;			/*Pointer to the port block*/
    struct FILE *fp;			/*Pointer to the file being used*/

/*Get a file pointer for this device*/
/*This file stays open forever*/

    fp = (struct FILE *) fopen(ip->name, "r+");
    if (!fp)
	return;

/*Get some pointers*/

    np = (struct NI_GBL *) ip->misc;	/* NI global block */
    tupb = (struct TUPB *) np->pbp;	/* port block */

#if ( STARTSHUT || DRIVERSHUT )
    krn$_post(&tupb->tu_ready_s);
#endif

/*Wait here for the first interrupt*/

    krn$_wait(&(tupb->rx_isr));

/*Do this forever*/

#if !( STARTSHUT || DRIVERSHUT )

    while (1) {
	while (tupb->rdes[tupb->rx_index].tu_flags & TU_OWN)
	    krn$_wait(&(tupb->rx_isr));

	/*Process the Receive descriptors*/
	tu_process_rx(fp, ip, tupb);
    }
#endif

#if ( STARTSHUT || DRIVERSHUT )

    while (1) {

	while ((tupb->rdes[tupb->rx_index].tu_flags & TU_OWN) &&
	  (tupb->pb.state != DDB$K_STOP))
	    krn$_wait(&(tupb->rx_isr));

	if (tupb->pb.state == DDB$K_STOP) {	/* YES: kill this process. */
	    dprintf("shutting down tu_rx...\n", p_args0);
	    fclose(fp);
	    dprintf("tu_rx: closed\n", p_args0);
	    return;
	}

/*Process the Receive descriptors*/

	tu_process_rx(fp, ip, tupb);
    }					/* while */
#endif
}

/*+
 * ============================================================================
 * = tu_process_rx - Process RX descriptors and packets.                      =
 * ============================================================================
 *
 * OVERVIEW:                    
 *
 *	This routine will process receive packets and descriptors. It processes
 *	all the descriptors that are currently owned by the host. This routine
 *	is responsible for getting the outstanding message and calling the
 *	receive routine.
 *
 * FORM OF CALL:
 *
 *	tu_process_rx (fp,ip,tupb); 
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *  struct FILE *fp - Pointer to the receive port file.
 *	struct INODE *ip - Pointer to the Device's inode.
 *	struct TUPB *tupb - Pointer to the tu port block.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void tu_process_rx(struct FILE *fp, struct INODE *ip, struct TUPB *tupb)
{
    PTU_RING rdes;
    unsigned char *pkt;
    unsigned long crc1, crc2, c;
    U_INT_16 len;
    int i, j, k;
    int badcrc = 0;
    struct EW_MSG *mh;			/*Received message holder*/

/*Wait for the chip to clear the ownership bit in the descriptors*/

    for (i=0; i<20; i++) {
	if (!(tupb->rdes[tupb->rx_index].tu_flags & TU_OWN))
	    break;
	else
	    krn$_micro_delay (1);
    }

/*Process all the descriptors that we own*/

    while (!(tupb->rdes[tupb->rx_index].tu_flags & TU_OWN)) {

/*Get the descriptor pointer*/

	rdes = &(tupb->rdes[tupb->rx_index]);

	dprintf("Processing Receive %08x: %08x %08x %08x %08x\n",
	  p_args5(rdes, rdes->tu_flags, rdes->tu_info, rdes->tu_bfaddr1, rdes->tu_bfaddr2));

/*Count the receive frames*/

	tu_rx_count(tupb, rdes);

/*Look at the descriptor*/
/*Check for errors*/

	if ((rdes->tu_flags & TU_RDES_ES) ||
	  (!(rdes->tu_flags & TU_RDES_LS))) {

/*Give the ownership back*/

	    dprintf("Receive error RDES %08x: %08x %08x %08x %08x\n",
		p_args5(rdes, rdes->tu_flags, rdes->tu_info, rdes->tu_bfaddr1, rdes->tu_bfaddr2));

	    mb();
	    rdes->tu_flags = TU_OWN;

	} else {

/*Valid message*/
/*Get a new buffer and give ownership back to the XGEC*/

	    pkt = (unsigned char *) (rdes->tu_bfaddr1 - tu_window_base);
	    len = (rdes->tu_flags & TU_RDES_FL_MSK) >> TU_RDES_FL_SFT;

/*Check the crc*/

	    badcrc = 0;

#if 0
/* This chunk of code will CRC the packet in system memory and compare the 
 * calculated CRC with the CRC off the wire, thus verifying the DMA 
 * transaction. A mismatch in the CRC will cause the packet to be disregarded
 * and an error message to be pprintf'ed.
 * The class level protocol should ask for it again. (MOP and BOOTP/TFTP do 
 * this).
 */
	    crc1 = 0xffffffff;
	    crc1 = crc32(pkt, len - 4, 1, crc1);

/*Reverse into r0 and complement*/

	    for (c = 0, j = 0, k = 31; j < 32; j++, k--)
		c |= ((crc1 >> j) & 1) << k;
	    crc1 = ~c;
	    crc2 = *(unsigned long *) (pkt + len - 4);
	    if (crc1 != crc2) {
		dprintf("BAD CRC\n", p_args0);
		dprintf("rdes: %08x pkt: %08x len: %08x\n", p_args3(rdes,pkt,len));
		mb();
		rdes->tu_flags = TU_OWN;
		badcrc = 1;
	    }
#endif

	    if (!badcrc)		/* workaround for bad DMA */
	      {
		rdes->tu_bfaddr1 = ALLOCATE_PKT(ip, 1);
		mb();
		rdes->tu_flags = TU_OWN;

/*If Callbacks are enabled pass the raw packet*/

		if (cb_ref) {

/*If there are already too many messages cleanup the list*/

		    if (tupb->rcv_msg_cnt > EW_MAX_RCV_MSGS)
			tu_free_rcv_pkts(ip);

/*Get a message holder*/

		    mh = (struct EW_MSG *) malloc_noown(sizeof(struct EW_MSG));

/*Fill in the message holder*/

		    mh->msg = pkt;
		    mh->size = len;

/*Insert it onto the list*/

		    spinlock(&spl_kernel);
		    insq(mh, tupb->rqh.blink);
		    tupb->rcv_msg_cnt++;
		    spinunlock(&spl_kernel);

		} else {

/*Strip the crc*/

		    len -= 4;

/*Call the datalink routine*/

		    if (tupb->lrp)
			(*(tupb->lrp))(fp, pkt, len);

		}			/* end of if (cb_ref) else */
	    }				/* end of if (!badcrc) */
	}				/* end of valid message handling else */

/*Get the next descriptor*/

	spinlock(&(tupb->spl_port));
	tupb->rx_index++;
	tupb->rx_index %= tu_rcv_buf_no;
	spinunlock(&(tupb->spl_port));
    }					/* end of while loop */
}

/*+
 * ============================================================================
 * = ez_free_rcv_pkts - Frees packets from the receive list.                  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will free the packets and memory used for
 *	the received packet list.
 *
 * FORM OF CALL:
 *
 *	ez_free_rcv_pkts (ip); 
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	struct INODE *ip - Inode of the current port.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void tu_free_rcv_pkts(struct INODE *ip)
{
    struct NI_GBL *np;			/*Pointer to NI global database*/
    struct TUPB *tupb;			/*Pointer to the port block*/
    struct EW_MSG *mr;			/*Pointer to the message received*/

/*Get the pointers to the datalink info*/

    np = (struct NI_GBL *) ip->misc;
    tupb = (struct TUPB *) np->pbp;

/*Free all the messages on the preallocated list*/

    spinlock(&spl_kernel);
    while (tupb->rqh.flink != (struct EW_MSG *) &(tupb->rqh.flink)) {

/*Remove the packet*/

	mr = (struct EW_MSG *) remq(tupb->rqh.flink);
	tupb->rcv_msg_cnt--;
	spinunlock(&spl_kernel);

/*Free the packet and memory*/

	ndl_deallocate_pkt(ip, mr->msg);
	free(mr);

/*Syncronize next access*/

	spinlock(&spl_kernel);
    }
    spinunlock(&spl_kernel);
}

/*+
 * ============================================================================
 * = tu_process_tx - Process TX descriptors and packets.                      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will process transmit packets and descriptors. It processes
 *	all the descriptors that have been used owned by the host. This
 *	routine is responsible for counting transmit errors and clearing
 *	the descriptors.
 *
 * FORM OF CALL:
 *
 *	tu_process_tx (tupb); 
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	struct TUPB *tupb - Pointer to the tu port block.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void tu_process_tx(struct TUPB *tupb)
{
    int i, process;
    PTU_RING tdes;

/*Wait for the chip to clear the ownership bit in the descriptors*/

    spinlock(&(tupb->spl_port));
    for (i=0; i<20; i++) {
	if (process = (!(tupb->tdes[tupb->tx_index_out].tu_flags & TU_OWN)) &&
		    (tupb->tx_index_in != tupb->tx_index_out))
	    break;
	else
	    krn$_micro_delay (1);
    }
    spinunlock(&(tupb->spl_port));

/*Process the descriptors*/

/*Check to see if we should process this descriptor*/

    while (process) {

/*Get the descriptor pointer*/

	tdes = &(tupb->tdes[tupb->tx_index_out]);

	dprintf("Processing transmit %08x: %08x %08x %08x %08x\n",
	  p_args5(tdes, tdes->tu_flags, tdes->tu_info, tdes->tu_bfaddr1, tdes->tu_bfaddr2));

/*Count*/

	tu_tx_count(tupb, tdes);

/*Post the waiter*/

	tupb->tx_isr[tupb->tx_index_out].alt = TRUE;
	krn$_post(&(tupb->tx_isr[tupb->tx_index_out].sem));

/*Get the next descriptor*/

	spinlock(&(tupb->spl_port));
	tupb->tx_index_out++;
	tupb->tx_index_out %= tu_xmt_buf_no;
	process = (!(tupb->tdes[tupb->tx_index_out].tu_flags & TU_OWN)) &&
	  (tupb->tx_index_in != tupb->tx_index_out);
	spinunlock(&(tupb->spl_port));
    }
}

/*+
 * ============================================================================
 * = tu_write - Driver write routine.                                         =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine does a write to the tulip port. It is called from
 *	the file system
 *
 * FORM OF CALL:
 *
 *	tu_write (fp, item_size, number, buf); 
 *                    
 * RETURNS:
 *
 *	int len - The number of bytes written.
 *
 * ARGUMENTS:
 *
 *	struct FILE *fp - Pointer to the file to be written.
 *	int item_size	- size of item
 *	int number	- number of items
 *	char *buf - Buffer to be written. 
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int tu_write(struct FILE *fp, int item_size, int number, char *buf)
{
    struct INODE *ip;			/* Pointer to the INODE */
    struct NI_GBL *np;			/* Pointer to the NI global database */
    struct TUPB *tupb;			/* Pointer to the port block */

/*Get some pointers*/

    ip = fp->ip;
    np = (struct NI_GBL *) ip->misc;
    tupb = (struct TUPB *) np->pbp;

    if (cb_ref)
	memcpy(buf + 6, tupb->sa, 6);

/*Return the length*/

/* add offset to normal space pointer */

    buf = (char *) ((u_long) buf + tu_window_base);
    return (tu_send(ip, tupb, buf, item_size * number, 0));
}

/*+
 * ============================================================================
 * = tu_setmode - Set mode                                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Set mode.
 *  
 * FORM OF CALL:
 *  
 *	tu_setmode ()
 *  
 * RETURNS:
 *
 *	msg_success - Normal completion.
 *       
 * ARGUMENTS:
 *
 *	int mode - Mode the driver is to be changed to.
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/

void tu_setmode(struct TUPB *tupb, int mode)
{
/*Stop if we're asked to stop and are not already stopped*/

    if (mode == DDB$K_STOP)

/*Execute the stop routine*/

	tu_stop_driver(tupb->ip);

    else if (mode == DDB$K_START)

	tu_restart_driver(tupb->ip);
}

/*+
 * ============================================================================
 * = tu_stop_driver - Stops the driver.                                       =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will stop the TULIP driver.
 *
 * FORM OF CALL:
 *
 *	tu_stop_driver (ip); 
 *
 * RETURNS:
 *
 *	int Status - msg_success or FAILURE.
 *
 * ARGUMENTS:
 *
 *	struct INODE *ip - Pointer to the Device's inode.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int tu_stop_driver(struct INODE *ip)
{
    struct NI_GBL *np;			/*Pointer to the NI global database*/
    struct TUPB *tupb;			/*Pointer to the port block*/

    dprintf("tu_stop_driver: %s\n", p_args1(ip->name));

/*Get some pointers*/

    np = (struct NI_GBL *) ip->misc;
    tupb = (struct TUPB *) np->pbp;

/*If we are already stopped don't stop again*/

    if (tupb->state == TU_K_STOPPED)
	return (msg_success);

/*If we are unitialized return a failure*/

    if (tupb->state != TU_K_STARTED)
	return (msg_failure);

/*Set the state flag*/

    tupb->state = TU_K_STOPPED;

/*Make sure the device is stopped*/

    tu_init_device(tupb);

/*Check for polled or not*/

    if (tupb->pb.vector != 0)
	TULIP_DISABLE_INTERRUPT(tupb, tupb->pb.vector);
}

/*+
 * ============================================================================
 * = tu_change_mode - Change the tulip's mode.                                =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will change the tgec modes. It uses the
 *	specified constant for the mode to be changed to.
 *
 * FORM OF CALL:
 *
 *	tu_change_mode (fp,mode); 
 *                            
 * RETURNS:
 *
 *	int Status - msg_success or FAILURE.
 *
 * ARGUMENTS:
 *
 *	struct FILE *fp - File pointing to the port.
 *	int mode - The mode the sgec will be changed to.
 *
 *		NDL$K_NORMAL_OM:	Normal operating mode.
 *		NDL$K_INT: 		Internal loopback.
 *		NDL$K_EXT: 		Internal loopback.
 *		NDL$K_TP: 		Twisted pair.
 *		NDL$K_TP_FD: 		Full Duplex, Twisted pair.
 *		NDL$K_AUI: 		AUI.
 *		NDL$K_BNC: 		BNC.
 *
 * SIDE EFFECTS:
 *   
 *      None
 *
-*/

int tu_change_mode(struct FILE *fp, int mode)
{
    struct NI_GBL *np;			/*Pointer to NI global database*/
    struct TUPB *tupb;			/*Pointer to the port block*/
    unsigned long csr6;
    unsigned long csr14;
    char var[EV_NAME_LEN];

/*Get the pointer to the port block*/

    np = (struct NI_GBL *) fp->ip->misc;
    tupb = (struct EZ_PB *) np->pbp;

/*Change the mode*/

    switch (mode) {

	case NDL$K_NORMAL_OM: 
#if MEDULLA || CORTEX || YUKONA
	    if (!cpip)
		dqprintf("Change to Normal Operating Mode.\n");
#else
	    dqprintf("Change to Normal Operating Mode.\n");
#endif
	    spinlock(&spl_kernel);
	    csr6 = RD_TULIP_CSR_LONG(tupb, TU_CSR6_ADDR);
	    csr6 &= ~TU_CSR6_OM;
	    WRT_TULIP_CSR_LONG(tupb, TU_CSR6_ADDR, csr6);
	    spinunlock(&spl_kernel);
	    break;

	case NDL$K_INT: 
#if MEDULLA || CORTEX || YUKONA
	    if (!cpip)
		dqprintf("Change mode to Internal loopback.\n");
#else
	    dqprintf("Change mode to Internal loopback.\n");
#endif
	    spinlock(&spl_kernel);
	    csr6 = RD_TULIP_CSR_LONG(tupb, TU_CSR6_ADDR);
	    csr6 &= ~TU_CSR6_OM;
	    csr6 |= TU_OM6_INL;
	    WRT_TULIP_CSR_LONG(tupb, TU_CSR6_ADDR, csr6);
	    spinunlock(&spl_kernel);
	    break;

	case NDL$K_EXT: 
	    dqprintf("Change mode to External loopback.\n");
	    spinlock(&spl_kernel);
	    csr6 = RD_TULIP_CSR_LONG(tupb, TU_CSR6_ADDR);
	    csr6 &= ~TU_CSR6_OM;
	    csr6 |= TU_OM6_EXL;
	    WRT_TULIP_CSR_LONG(tupb, TU_CSR6_ADDR, csr6);
            if ( tupb->controller_type == 21040 || tupb->controller_type == 21041 ) {
               csr14 = RD_TULIP_CSR_LONG(tupb, TU_CSR14_ADDR);
               if ( (int)csr14 == 0xFFFF7F3F || (int)csr14 == 0xFFFF7F3d )  /* verify twisted pair */
	          WRT_TULIP_CSR_LONG(tupb, TU_CSR14_ADDR, 0xFFFF7B3D); /* set ext. loopback mode */
            }
	    spinunlock(&spl_kernel);
	    break;

	case NDL$K_TP: 
	    sprintf(var, "%4.4s_mode", fp->ip->name);
	    ev_write(var, "Twisted-Pair", EV$K_STRING);
	    break;

	case NDL$K_TP_FD: 
	    sprintf(var, "%4.4s_mode", fp->ip->name);
	    ev_write(var, "Full Duplex, Twisted-Pair", EV$K_STRING);
	    break;

	case NDL$K_AUI: 
	    sprintf(var, "%4.4s_mode", fp->ip->name);
	    ev_write(var, "AUI", EV$K_STRING);
	    break;

	case NDL$K_BNC: 
	    sprintf(var, "%4.4s_mode", fp->ip->name);
	    ev_write(var, "BNC", EV$K_STRING);
	    break;

	default: 
	    ni_error(tupb->pb.name, "Invalid mode\n");
    }
    return (msg_success);
}

/*+
 * ============================================================================
 * = tu_restart_driver - restarts the driver.                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will restart the driver.
 *
 * FORM OF CALL:
 *
 *	tu_restart_driver (ip); 
 *
 * RETURNS:
 *
 *	int Status - msg_success or FAILURE.
 *
 * ARGUMENTS:
 *
 *	struct INODE *ip - Pointer to the Device's inode.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int tu_restart_driver(struct INODE *ip)
{
    struct NI_GBL *np;			/*Pointer to the NI global database*/
    struct TUPB *tupb;			/*Pointer to the port block*/
    unsigned long csr6;
    int ew_media;

    dprintf("tu_restart_driver: %s\n", p_args1(ip->name));

/*Get some pointers*/

    np = (struct NI_GBL *) ip->misc;
    tupb = (struct TUPB *) np->pbp;

/*If we are already started don't start again*/

    if (tupb->state == TU_K_STARTED)
	return (msg_success);

/*If we are unitialized return a failure*/

    if (tupb->state != TU_K_STOPPED)
	return (msg_failure);

/*Make sure the mode is read*/

    tu_read_mode(tupb);

/*Set to the proper mode. */
/* Ensure the CFDD register is set to the proper value.
 * A PCI Reset will set CFDD<31> the "sleep mode bit" in the 
 * 21143 chip which causes the system to hang when rebooting or 
 * restarting OpenVMS.
*/

    if (tupb->pb.type == TYPE_PCI) {

       if (tupb->mode == TU_K_TWISTED) {
	   ew_media = TU_K_OS_TWISTED;
       } 
       else if (tupb->mode == TU_K_TWISTED_FD) {
	  ew_media = TU_K_OS_TWISTED_FD;
       } 
       else if (tupb->mode == TU_K_AUI) {
	  ew_media = TU_K_OS_AUI;
       } 
       else if (tupb->mode == TU_K_BNC) {
	  ew_media = TU_K_OS_BNC;
       } 
       else if (tupb->mode == TU_K_FAST) {
	  ew_media = TU_K_OS_F;
       } 
       else if (tupb->mode == TU_K_FASTFD) {
	  ew_media = TU_K_OS_FFD;
       } 
       else if (tupb->mode == TU_K_AUTO_NEGOTIATE) {
	  ew_media = TU_K_OS_AN;    
       } 
       else {
	  ew_media = TU_K_OS_NOT_USED;
       }
       outcfgl(tupb, TU_PCFDA_ADDR, ew_media);
    }

/*Configure the module*/

    tu_platform_init(tupb);

/*Make sure the device is stopped*/

    tu_init_device(tupb);

/*Set the device to the correct media port*/

    tu_init_media(tupb);

/* Enable the interrupts, Check for polled or not */

    if (tupb->pb.vector != 0)
	TULIP_ENABLE_INTERRUPT(tupb, tupb->pb.vector);

/*Initialize the tx descriptors*/

    tu_init_tx(tupb);

/*Initialize the rx descriptors*/

    tu_init_rx(ip, tupb);

/*Send the setup frame*/

    tu_init_st(ip, tupb);

/*Start the receive process*/

    spinlock(&spl_kernel);
    csr6 = RD_TULIP_CSR_LONG(tupb, TU_CSR6_ADDR);
    WRT_TULIP_CSR_LONG(tupb, TU_CSR6_ADDR, csr6 | TU_CSR6_SR);
    spinunlock(&spl_kernel);

/*Set the state flag*/

    tupb->state = TU_K_STARTED;

    return (msg_success);
}

/*+
 * ============================================================================
 * = tu_tx_count - Count the transmit.                                        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This handle the transmit mop counters.
 *
 * FORM OF CALL:
 *
 *	tu_tx_count (tupb,tdes); 
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	struct TUPB *tupb - Pointer to the tu port block.
 *	PTU_RING tdes - Pointer to the transmit descriptor;
 *
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void tu_tx_count(struct TUPB *tupb, struct _TU_RING *tdes)
{
    int bs, cc;

/*If its a setup frame don't count it*/

    if (tdes->tu_info & TU_SET)
	return;

/*Handle some normal counts*/

    if (tdes->tu_flags & TU_DE)
	tupb->mc->MOP_V4CNT_TX_INIT_DEFERRED++;
    cc = (tdes->tu_flags & TU_CC_MSK) >> TU_CC_SFT;
    if (cc == 1)
	tupb->mc->MOP_V4CNT_TX_ONE_COLLISION++;
    if (cc > 1)
	tupb->mc->MOP_V4CNT_TX_MULTI_COLLISION++;
    if (tdes->tu_flags & TU_HF)
	tupb->mc->MOP_V4CNT_FAIL_COLLIS_DETECT++;

/*If it's an error count the error if not count the packet*/

    if (tdes->tu_flags & TU_TDES_ES) {

	dprintf("Processing transmit ERROR %08x: %08x %08x %08x %08x\n",
	  p_args5(tdes, tdes->tu_flags, tdes->tu_info, tdes->tu_bfaddr1, tdes->tu_bfaddr2));

/*Count the errors*/

	if (tdes->tu_flags & TU_EC)
	    tupb->mc->MOP_V4CNT_TX_FAIL_EXCESS_COLLS++;
	if ((tdes->tu_flags & TU_LC) && (tdes->tu_flags & TU_LO))
	    tupb->mc->MOP_V4CNT_TX_FAIL_CARRIER_CHECK++;
	if (tdes->tu_flags & TU_NC) {
	    tupb->mc->MOP_V4CNT_TX_FAIL_SHRT_CIRCUIT++;
	    tupb->anc++;
	}
	if ((tdes->tu_flags & TU_EC) && (tupb->tu_flags_saved & TU_EC)) {
	    tupb->mc->MOP_V4CNT_TX_FAIL_OPEN_CIRCUIT++;
	    tupb->aoc++;
	}
	if (tdes->tu_flags & TU_LC)
	    tupb->mc->MOP_V4CNT_TX_FAIL_REMOTE_DEFER++;
    } else {

/*Count the successfully transmitted frames*/

	if (tdes->tu_info & TU_TDES_LS)
	    tupb->mc->MOP_V4CNT_TX_FRAMES++;

/*And the number of bytes*/

	bs = (tdes->tu_info & TU_RDES_BS1_MSK) +
	  ((tdes->tu_info & TU_RDES_BS2_MSK) >> TU_RDES_BS2_SFT);
	if (bs < TU_MIN_PKT_SIZE + TU_CRC_SIZE)
	    tupb->mc->MOP_V4CNT_TX_BYTES += TU_MIN_PKT_SIZE + TU_CRC_SIZE;
	else
	    tupb->mc->MOP_V4CNT_TX_BYTES += bs;

/*Reset the local error counts*/

	tupb->lkf = tupb->ato = tupb->anc = tupb->aoc = 0;
    }

/*Save this descriptor's flags*/

    tupb->tu_flags_saved = tdes->tu_flags;
}

/*+
 * ============================================================================
 * = tu_read_mode - read the tulip mode (Twisted Pair or AUI).                =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This function will read the tulip mode environment variable
 *	and convert it to the correct integer.
 *
 * FORM OF CALL:
 *
 *	tu_read_mode (tupb)
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	struct TUPB *tupb - Pointer to the tu port block.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void tu_read_mode(struct TUPB *tupb)
{
    struct EVNODE *evp, evn;		/*Pointer to the evnode,evnode*/
    char var[EV_NAME_LEN];		/*General holder for a name*/
    char c;

/*Read the environment variables*/

    evp = &evn;
    sprintf(var, "%4.4s_mode", tupb->pb.name);
    if (ev_read(var, &evp, 0) != msg_success) {
#if AUTO_SENSE
	tupb->mode = TU_K_AUTO;
#else
	tupb->mode = TU_K_TWISTED;
#endif
	return;
    }

/*Now check the value*/

#if AUTO_SENSE
    c = evp->value.string[2];
    if (strncmp_nocase(evp->value.string, "Auto-Neg", 8) == 0)
	tupb->mode = TU_K_AUTO_NEGOTIATE;
    else if (c == 'i')
	tupb->mode = TU_K_TWISTED;
    else if (c == 'l')
	tupb->mode = TU_K_TWISTED_FD;
    else if (c == 'C')
	tupb->mode = TU_K_BNC;
    else if (c == 'I')
	tupb->mode = TU_K_AUI;
    else if (strncmp_nocase(evp->value.string, "FastFD", 6) == 0)
	tupb->mode = TU_K_FASTFD;
    else if (strncmp_nocase(evp->value.string, "Fast", 4) == 0)
	tupb->mode = TU_K_FAST;
    else
	tupb->mode = TU_K_AUTO;
#else
    c = evp->value.string[2];
    if (strncmp_nocase(evp->value.string, "Auto-Neg", 8) == 0)
	tupb->mode = TU_K_AUTO_NEGOTIATE;
    else  if (c == 'i')
	tupb->mode = TU_K_TWISTED;
    else if (c == 'l')
	tupb->mode = TU_K_TWISTED_FD;
    else if (c == 'C')
	tupb->mode = TU_K_BNC;
    else if (c == 't')
	tupb->mode = TU_K_AUTO;
    else if (strncmp_nocase(evp->value.string, "FastFD", 6) == 0)
	tupb->mode = TU_K_FASTFD;
    else if (strncmp_nocase(evp->value.string, "Fast", 4) == 0)
	tupb->mode = TU_K_FAST;

    else
	tupb->mode = TU_K_AUI;
#endif
}

/*+
 * ============================================================================
 * = tu_ev_write - Action routine for the environment variables.              =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This function is the action routine used by the ew
 *	environment variables. 
 *
 * FORM OF CALL:
 *
 *	tu_ev_write (name,ev)
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	char *name - Pointer to the environment variable.
 *	struct EVNODE *ev - Pointer to ev node.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void tu_ev_write(char *name, struct EVNODE *ev)
{
    struct FILE *fp;			/*File pointer*/
    struct NI_GBL *np;			/*Pointer to the NI global database*/
    struct TUPB *tupb;			/*Pointer to the port block*/
    int ew_media;
    char fname[8];			/*Name of the file*/
    char c;

/*Get the port block*/

    strncpy(fname, name, 4);
    fname[4] = 0;
    fp = fopen(fname, "rs");
    if (!fp)
	return;
    np = (struct NI_GBL *) fp->ip->misc;
    tupb = (struct TUPB *) np->pbp;

/*Read the new mode*/

#if AUTO_SENSE
    c = ev->value.string[2];
    if (strncmp_nocase(ev->value.string, "Auto-Neg", 8) == 0)
	tupb->mode = TU_K_AUTO_NEGOTIATE;
    else  if (c == 'i')
	tupb->mode = TU_K_TWISTED;
    else if (c == 'l')
	tupb->mode = TU_K_TWISTED_FD;
    else if (c == 'C')
	tupb->mode = TU_K_BNC;
    else if (c == 'I')
	tupb->mode = TU_K_AUI;
    else if (strncmp_nocase(ev->value.string, "FastFD", 6) == 0)
	tupb->mode = TU_K_FASTFD;
    else if (strncmp_nocase(ev->value.string, "Fast", 4) == 0)
	tupb->mode = TU_K_FAST;
    else
	tupb->mode = TU_K_AUTO;
#else
    c = ev->value.string[2];
    if (strncmp_nocase(ev->value.string, "Auto-Neg", 8) == 0)
	tupb->mode = TU_K_AUTO_NEGOTIATE;
    else if (c == 'i')
	tupb->mode = TU_K_TWISTED;
    else if (c == 'l')
	tupb->mode = TU_K_TWISTED_FD;
    else if (c == 'C')
	tupb->mode = TU_K_BNC;
    else if (c == 'I')
	tupb->mode = TU_K_AUI;
    else if (strncmp_nocase(ev->value.string, "FastFD", 6) == 0)
	tupb->mode = TU_K_FASTFD;
    else if (strncmp_nocase(ev->value.string, "Fast", 4) == 0)
	tupb->mode = TU_K_FAST;
#endif

/*Change the mode if we need to*/

    if (tupb->mode == TU_K_AUTO_NEGOTIATE ) {
 	if (tupb->nway_supported) {
	  ew_media = TU_K_OS_AN;
	err_printf("Change mode to auto negotiate \n");
	}
	else {
	     if ((tupb->fast_speed)&&(tupb->full_duplex)) {
	         ew_media = TU_K_OS_FFD ;
		 tupb->mode = TU_K_FASTFD;
		 strcpy(ev->value.string,"FastFD");
	     }
	     else {
		 tupb->mode = TU_K_TWISTED;
	         ew_media = TU_K_OS_TWISTED ;
		 strcpy(ev->value.string,"Twisted-Pair");
	     }
        err_printf("Auto Negotiation not supported. ");
	}
	tu_switch_con(tupb,0);
    }

    if ((tupb->mode == TU_K_TWISTED) || (tupb->mode == TU_K_TWISTED_FD)) {
	if (tupb->controller_type == DE500FA) {
	    err_printf("Unsupported mode. ");
	    tupb->mode = TU_K_FAST;
	    strcpy(ev->value.string,"Fast");
        }
        else {
		
            err_printf("Change mode to ");

            if (tupb->mode == TU_K_TWISTED_FD) {
            	err_printf("Full Duplex, ");
		ew_media = TU_K_OS_TWISTED_FD;
            } 
            else
            	ew_media = TU_K_OS_TWISTED;

	    err_printf("Twisted Pair.\n");
	    tu_switch_con(tupb, 0);
        }
    }
    if ((tupb->mode == TU_K_AUI) || (tupb->mode == TU_K_BNC)) {
	if ((tupb->controller_type == DE500FA) || (tupb->controller_type == DE500BA) ||
            (tupb->controller_type == DE500AA) || (tupb->controller_type == DE500XA)) {
	    err_printf("Unsupported mode. ");
	    tupb->mode = TU_K_FAST;
	    strcpy(ev->value.string,"Fast");
        }
        else {
	   err_printf("Change mode to ");
	   if (tupb->mode == TU_K_BNC) {
	      err_printf("BNC.\n");
	      ew_media = TU_K_OS_BNC;
	   } 
           else {
	      err_printf("AUI.\n");
	      ew_media = TU_K_OS_AUI;
           }
	tu_switch_con(tupb, 0);
	}
    }

    if (tupb->mode == TU_K_FAST) {
	err_printf("Change mode to ");	/* 6 */
	err_printf("FAST.\n");
	if (tupb->controller_type == DE500FA) 
	   ew_media = TU_K_OS_FX;
	else	
           ew_media = TU_K_OS_F;
	tu_switch_con(tupb, 0);
    }

    if (tupb->mode == TU_K_FASTFD) {	/* 7 */
	err_printf("Change mode to ");
	err_printf("FAST Full Duplex.\n");
	if (tupb->controller_type == DE500FA) 
	   ew_media = TU_K_OS_FFX;
	else
   	   ew_media = TU_K_OS_FFD;
	tu_switch_con(tupb, 0);
    }

#if AUTO_SENSE
    if (tupb->mode == TU_K_AUTO) {	/* 0 */
	err_printf("Change mode to ");
	err_printf("Auto-Sensing.\n");
	ew_media = TU_K_AUTO;
	tu_switch_con(tupb, 0);
    }
#endif

    if (tupb->pb.type == TYPE_PCI)
	outcfgl(tupb, TU_PCFDA_ADDR, ew_media);

/*Put the new value in nvram*/

    ev_sev(name, ev);

/*Close the file*/

    fclose(fp);
}

/*+
 * ============================================================================
 * = tu_check_mode - Check the tulip mode (Twisted Pair or AUI).              =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This function will check the tulip mode and change it
 *	appropriately. The valid modes are auto*sense twisted*pair or
 *	AUI.
 *
 * FORM OF CALL:
 *
 *	tu_check_mode (tupb)
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	struct TUPB *tupb - Pointer to the tu port block.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

#if AUTO_SENSE
void tu_check_mode(struct TUPB *tupb)
{
    unsigned long csr;

/*Check internal loopback*/

    csr = RD_TULIP_CSR_LONG(tupb, TU_CSR6_ADDR);
    if (csr & TU_OM6_INL)
	return;

/*Check link fail*/

    csr = RD_TULIP_CSR_LONG(tupb, TU_CSR12_ADDR);
    if (csr & TU_CSR12_LKF)
	tupb->lkf++;

    dprintf("Check Mode: %d lkf: %d ato: %d nc: %d oc: %d\n",
	p_args5(tupb->mode, tupb->lkf, tupb->ato, tupb->anc, tupb->aoc));

/*If we're not in auto configure mode just return*/

    if (tupb->mode != TU_K_AUTO)
	return;

/*If we have no important errors just return*/

    if (!(tupb->lkf | tupb->ato | tupb->anc | tupb->aoc))
	return;

/*Check our current mode and then handle the error*/

    tupb->csr13 = RD_TULIP_CSR_LONG(tupb, TU_CSR13_ADDR);
    if (tupb->csr13 & TU_CSR13_AUI) {

/*If we got no carrier or timed out twice switch the mode*/

	if ((tupb->ato > 1) || (tupb->anc > 1) || (tupb->aoc > 1))

/*Switch the mode*/

	    tu_switch_con(tupb, 1);
    } else {

/*If the link failed twice switch the mode*/

	if (tupb->lkf > 1)

/*Switch the mode*/

	    tu_switch_con(tupb, 1);
    }
}
#endif

/*+
 * ============================================================================
 * = tu_switch_con - Switch the network connection.                           =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This function will switch the network connection from
 *	AUI to twisted pair or from twisted pair to AUI.
 *
 * FORM OF CALL:
 *
 *	tu_switch_con (tupb, mode_auto)
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	struct TUPB *tupb - Pointer to the tu port block.
 *	int mode_auto     - uses round robin AUI->BNC->Twp->AUI...
 *		   	    non mode_auto uses the current mode
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int tu_switch_con(struct TUPB *tupb, int mode_auto)
{
    unsigned long csr5;
    unsigned long csr12;
    unsigned long csr13;
    unsigned long csr14;
    unsigned long csr15;
    unsigned long cmdcsr;
    unsigned long tu_mode;
    unsigned short status;

    tu_mode = tupb->mode;

    abnormal_intr_error_flag = 0;

/* default the mode to AUI for auto-sense */
/* except for the devices which use the 21143 chip */

    if (tu_mode == TU_K_AUTO) {
	if (tupb->controller_type == DE500FA) 
             tu_mode = TU_K_FASTFD;	
	else if (tupb->controller_type == DE500BA) 
		tu_mode = TU_K_TWISTED;
    }

#if AUTO_SENSE
    if (mode_auto) {

/*AUI BNC to Twisted pair*/

	if (tupb->last_auto_mode == TU_K_BNC) {
	    err_printf("Switching network %s to Twisted pair.\n", tupb->pb.name);
	    tu_mode = TU_K_TWISTED;
	}

/*Twisted pair to Twisted pair FD*/

	if (tupb->last_auto_mode == TU_K_TWISTED) {
	    err_printf("Switching network %s to Twisted Pair Full Duplex.\n",
	      tupb->pb.name);
	    tu_mode = TU_K_TWISTED_FD;
	}

/*Twisted pair FD to AUI thick*/

	if (tupb->last_auto_mode == TU_K_TWISTED_FD) {
	    err_printf("Switching network %s to AUI Thick.\n", tupb->pb.name);
	    tu_mode = TU_K_AUI;
	}

/*AUI thick to AUI BNC and not 21040 */

	if (tupb->last_auto_mode == TU_K_AUI && tupb->pci_type != 0x21011) {
	    err_printf("Switching network %s to AUI BNC.\n", tupb->pb.name);
	    tu_mode = TU_K_BNC;
	}

/* AUI thick to Twisted Pair and 21040 */

	if (tupb->last_auto_mode == TU_K_AUI && tupb->pci_type == 0x21011) {
	    err_printf("Switching network %s to Twisted pair.\n", tupb->pb.name);
	    tu_mode = TU_K_TWISTED;
	}
    }
#endif

/* 21140A CSR0,6 is different than 21140 */
/* 21140 modes are different than the others */
                                                                                            

/* stop the receive and transmit */

    if (tupb->pci_type == 0x91011) {
	WRT_TULIP_CSR_LONG(tupb, TU_CSR13_ADDR, 0);
	WRT_TULIP_CSR_LONG(tupb, TU_CSR12_ADDR, TU_CSR12_PINS_INIT);
	spinlock(&spl_kernel);
        cmdcsr = RD_TULIP_CSR_LONG(tupb, TU_CSR6_ADDR);
	WRT_TULIP_CSR_LONG(tupb, TU_CSR6_ADDR, cmdcsr & TU_CSR6_STOP);
	spinunlock(&spl_kernel);
    } 

    if (tupb->pci_type == 0x191011) {
	csr5 = RD_TULIP_CSR_LONG(tupb, TU_CSR5_ADDR);
	spinlock(&spl_kernel);
	cmdcsr = RD_TULIP_CSR_LONG(tupb, TU_CSR6_ADDR);
	WRT_TULIP_CSR_LONG(tupb, TU_CSR6_ADDR, cmdcsr & TU_CSR6_STOP);
	spinunlock(&spl_kernel);
	csr5 = RD_TULIP_CSR_LONG(tupb, TU_CSR5_ADDR);
    }

    if (tu_mode == TU_K_AUTO_NEGOTIATE) {

       status = tu_auto_negotiate(tupb);

       if (tupb->fast_speed) {
          if (tupb->full_duplex)
             tu_mode = TU_K_FASTFD;
          else
             tu_mode = TU_K_FAST;
       }
       else {
          if  (tupb->full_duplex)
             tu_mode = TU_K_TWISTED_FD;
          else
             tu_mode = TU_K_TWISTED;
        }
    }
    if (tu_mode == TU_K_FAST) {
       switch (tupb->controller_type) {
          case DE500AA :
             cmdcsr = TU_CSR6_FASTA | TU_CSR6_INIT | TU_CSR6_SR | TU_CSR6_ST;
             csr12  =  TU_MII_CR_100;
             break;

	  case DE500FA  :
          case DE500BA :
          case TU_21143 :
             cmdcsr = TU_CSR6_FAST | TU_CSR6_MBO | TU_CSR6_INIT | TU_CSR6_SR | TU_CSR6_ST ;
             csr13  = 0x00000001;
             csr14  = 0x00000000;
             csr15  = 0x00050008;
	     break;

          case P2SEPLUS :
          case DE500XA :
             cmdcsr = TU_CSR6_FAST | TU_CSR6_INIT | TU_CSR6_SR | TU_CSR6_ST ;
             csr12  = TU_CSR12_INIT_FAST_HD;
             break;
          default:
             err_printf("Invalid mode\n");
             break;
       }
    }
    if (tu_mode == TU_K_FASTFD) {
       switch (tupb->controller_type) {
          case DE500AA :
             cmdcsr = TU_CSR6_FASTA | TU_CSR6_FD | TU_CSR6_INIT | TU_CSR6_SR | TU_CSR6_ST;
             csr12  = TU_MII_CR_100 | TU_MII_CR_FDX;
             break;

          case DE500FA :
          case DE500BA :
          case TU_21143 :
             cmdcsr = TU_CSR6_PS | TU_CSR6_HBD | TU_CSR6_PCS | TU_CSR6_SCR | TU_CSR6_FD | TU_CSR6_MBO | TU_CSR6_INIT | TU_CSR6_SR | TU_CSR6_ST;
	     csr13  = 0x00000001;
	     csr14  = 0x00000000;
	     csr15  = 0x00050008;
	     break;

          case DE500XA :
          case P2SEPLUS :
             cmdcsr = TU_CSR6_FAST | TU_CSR6_FD | TU_CSR6_INIT | TU_CSR6_SR | TU_CSR6_ST;
             csr12  = TU_CSR12_INIT_FAST_FD;
             break;

          default:
             err_printf("Invalid mode\n");
             break;
       }
    }
    if (tu_mode == TU_K_TWISTED_FD) {
       switch ( tupb->controller_type ) {
          case DE500AA :
             cmdcsr = TU_CSR6_SLOWA | TU_CSR6_FD | TU_CSR6_INIT | TU_CSR6_SR |TU_CSR6_ST;
             csr12  =  TU_MII_CR_10 | TU_MII_CR_FDX;
             break;

          case DE500XA :        
          case P2SEPLUS :
             cmdcsr = TU_CSR6_SLOW | TU_CSR6_FD | TU_CSR6_INIT | TU_CSR6_SR |TU_CSR6_ST;
             csr12  = TU_CSR12_INIT_SLOW_FD;
             break;

          case DE500BA :
          case TU_21143 :
             cmdcsr = TU_CSR6_SLOW | TU_CSR6_FD | TU_CSR6_INIT | TU_CSR6_SR |TU_CSR6_ST;
             csr13  = 0x0001;
             csr14  = 0x7f3d;
             csr15  = 0x0008;
             break;

          case TU_21142 :
             csr12  = TU_CSR12_INIT_SLOW_FD;
             cmdcsr = TU_CSR6_SLOW | TU_CSR6_FD | TU_CSR6_INIT | TU_CSR6_SR |TU_CSR6_ST;
          case TU_21040 :
             csr13 = 0xffff0f01;
             csr14 = 0xfffffffd;
             csr15 = 0xffff0000;
             break;

          case TU_21041 :
             csr15 = TU_CSR15_UTP_INIT_PG3;
             csr13 = TU_CSR13_UTP_INIT_PG3;
             csr14 = TU_CSR14_FDX_INIT_PG3;
             break;
       }
    }
    if (tu_mode == TU_K_TWISTED)  {
       switch (tupb->controller_type ) {
          case DE500AA :
             cmdcsr = TU_CSR6_SLOWA | TU_CSR6_INIT | TU_CSR6_SR |TU_CSR6_ST;
             csr12  = TU_MII_CR_10;
             break;

          case P2SEPLUS :
          case DE500XA :
             cmdcsr = TU_CSR6_SLOW | TU_CSR6_INIT | TU_CSR6_SR |TU_CSR6_ST;
             csr12  = TU_CSR12_INIT_SLOW_HD;
             break;

          case DE500BA :
          case TU_21143 :
             cmdcsr = TU_CSR6_SLOW | TU_CSR6_INIT | TU_CSR6_SR |TU_CSR6_ST;
             csr13 = 0x0001;
             csr14 = 0x7f3f;
             csr15 = 0x0008;
             break;

          case TU_21040 :
             csr13 = 0xffff8f01;
             csr14 = 0xffffffff;
             csr15 = 0xffff0000;
             break;

          case TU_21041 :
             csr15 = TU_CSR15_UTP_INIT_PG3;
             csr13 = TU_CSR13_UTP_INIT_PG3;
             csr14 = TU_CSR14_UTP_INIT_PG3;
             break; 
       }
    }
    if (tu_mode == TU_K_AUI) {
       switch (tupb->controller_type ) {
          case DE500AA :
             cmdcsr = TU_CSR6_SLOWA | TU_CSR6_INIT | TU_CSR6_SR |TU_CSR6_ST;
             csr12  = TU_MII_CR_10;
             break;

          case P2SEPLUS :
          case DE500XA :
             cmdcsr = TU_CSR6_SLOW | TU_CSR6_INIT | TU_CSR6_SR |TU_CSR6_ST;
             csr12  = TU_CSR12_INIT_SLOW_HD;
             break;

          case DE500BA :
          case TU_21143 :
          case TU_21040 :
             csr13 = 0xffff8f09;
             csr14 = 0xffff0705;
             csr15 = 0xffff000e;
             break;

          case TU_21041 :
             csr15 = TU_CSR15_AUI_INIT_PG3;
             csr13 = TU_CSR13_AUI_INIT_PG3;
             csr14 = TU_CSR14_AUI_INIT_PG3;
             break;
       }
    }
    if (tu_mode == TU_K_BNC) {
       switch (tupb->controller_type ) {
          case DE500AA :
             cmdcsr = TU_CSR6_SLOWA | TU_CSR6_INIT | TU_CSR6_SR |TU_CSR6_ST;
             csr12  = TU_MII_CR_10;
             break;

          case P2SEPLUS :
             cmdcsr = TU_CSR6_SLOW | TU_CSR6_INIT | TU_CSR6_SR | TU_CSR6_ST;
             /* The p2se+ uses bit 4 of gpp (csr12) for using bnc mode */
             csr12  = TU_CSR12_INIT_BNC; 
             break;

          case DE500XA :
             cmdcsr = TU_CSR6_SLOW | TU_CSR6_INIT | TU_CSR6_SR | TU_CSR6_ST;
             csr12  = TU_CSR12_INIT_SLOW_HD;
             break;

          case TU_21040 :
          case TU_21041 :
          case DE500BA :
          case TU_21143 :

#if MTU
             /* MTU uses an external SIA for BNC (Thin-wire) */
             csr13 = TU_CSR13_OE13|TU_CSR13_IE|TU_CSR13_SIM|TU_CSR13_SRL;  /* 0x3041 */    
             csr14 = 0xffff0000;
             csr15 = 0xffff0006;
#else
             csr13 = 0xffffef09;
             csr14 = 0xffff0705;
             csr15 = 0xffff000e;
#endif
             break;
       }
    }

    switch (tupb->controller_type ) {
          case DE500AA  :
             krn$_sleep(10);
             WRT_TULIP_CSR_LONG(tupb, TU_CSR6_ADDR, cmdcsr);
             tu_mii_wr(tupb, TU_MII_CR, csr12);
             break;

          case DE500XA  :
          case P2SEPLUS :
             WRT_TULIP_CSR_LONG(tupb, TU_CSR6_ADDR, cmdcsr);
             WRT_TULIP_CSR_LONG(tupb, TU_CSR12_ADDR, csr12);
             break;

          case DE500FA :
	     cmdcsr = cmdcsr & (~TU_CSR6_SCR);
          case DE500BA :
          case TU_21143 :
             WRT_TULIP_CSR_LONG(tupb, TU_CSR13_ADDR, 0x00000000);
             WRT_TULIP_CSR_LONG(tupb, TU_CSR6_ADDR, cmdcsr);
             tu_mii_wr(tupb, TU_MII_CR, csr12);
	     WRT_TULIP_CSR_LONG(tupb, TU_CSR15_ADDR, 0x08a50000);
             WRT_TULIP_CSR_LONG(tupb, TU_CSR15_ADDR, 0x00000000);
             WRT_TULIP_CSR_LONG(tupb, TU_CSR15_ADDR, 0x00050000);
             WRT_TULIP_CSR_LONG(tupb, TU_CSR15_ADDR, csr15);
             WRT_TULIP_CSR_LONG(tupb, TU_CSR14_ADDR, csr14);
             WRT_TULIP_CSR_LONG(tupb, TU_CSR13_ADDR, csr13);
             break;

          case TU_21040 :
          case TU_21041 :
		WRT_TULIP_CSR_LONG(tupb, TU_CSR13_ADDR, 0x00000000);
		WRT_TULIP_CSR_LONG(tupb, TU_CSR14_ADDR, csr14);
		WRT_TULIP_CSR_LONG(tupb, TU_CSR15_ADDR, csr15);
		WRT_TULIP_CSR_LONG(tupb, TU_CSR13_ADDR, csr13);
		break;

    }
/*Save the new value of csr13*/

	tupb->csr13 = csr13;

/*Save the last auto sense mode */

    tupb->last_auto_mode = tu_mode;

/*Reset the local error counts*/
                         
    tupb->lkf = tupb->ato = tupb->anc = tupb->aoc = 0;
   return (msg_success);
}

/*+
 * ============================================================================
 * = tu_init_media - Set the tulip to use the correct media port
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This function will set the network connection to use the
 *	correct media port.
 *
 * FORM OF CALL:
 *
 *	tu_init_media (tupb)
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	struct TUPB *tupb - Pointer to the tu port block.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void tu_init_media(struct TUPB *tupb)
{
    unsigned long csr12;
    unsigned long csr13;
    unsigned long csr14;
    unsigned long csr15;
    unsigned long cmdcsr;
    unsigned long tu_mode;
    unsigned short status;

    dprintf("tu_init_media\n", p_args0);
    dprintf("tupb-mode = %d\n", p_args1(tupb->mode));

    tu_mode = tupb->mode;

/* default the mode to AUI for auto-sense */

    if (tu_mode == TU_K_AUTO) {
       if (tupb->pci_type == 0x91011) 
	tu_mode = TU_K_TWISTED;
       else
	tu_mode = TU_K_AUI;
    }
	/* 21140A CSR0,6 is different than 21140 */
	/* 21140 modes are different than the others */

	cmdcsr = RD_TULIP_CSR_LONG(tupb, TU_CSR6_ADDR);
    if (tupb->pci_type == 0x91011) {
	WRT_TULIP_CSR_LONG(tupb, TU_CSR13_ADDR, 0);
	WRT_TULIP_CSR_LONG(tupb, TU_CSR12_ADDR, TU_CSR12_PINS_INIT);
    }
    if (tu_mode == TU_K_AUTO_NEGOTIATE) {

       if (tupb->nway_supported)
          status = tu_auto_negotiate(tupb);

       if (tupb->fast_speed) 
          if (tupb->full_duplex)
             tu_mode = TU_K_FASTFD;
          else
             tu_mode = TU_K_FAST;
       else {
          if  (tupb->full_duplex)
             tu_mode = TU_K_TWISTED_FD;
          else
             tu_mode = TU_K_TWISTED;
       }
    }

    if (tu_mode == TU_K_FAST) {
       switch (tupb->controller_type) {
          case DE500AA :
             cmdcsr = TU_CSR6_FASTA | TU_CSR6_INIT ;
             csr12  = TU_MII_CR_100;
             break;

	  case DE500FA :
	     outcfgl((struct pb *)tupb, TU_PCFDA_ADDR, TU_K_OS_FX);

          case DE500BA :
          case TU_21143 :
             cmdcsr = TU_CSR6_FAST | TU_CSR6_MBO | TU_CSR6_INIT ;
             csr13  = 0x00000001;
             csr14  = 0x00000000;
             csr15  = 0x00050008;
             break;


          case P2SEPLUS :
          case DE500XA :
             cmdcsr = TU_CSR6_FAST | TU_CSR6_INIT ;
             csr12  = TU_CSR12_INIT_FAST_HD;
             break;

          default:
             err_printf("Invalid mode\n");
             break;
	}
    }
    if (tu_mode == TU_K_FASTFD) {
       switch (tupb->controller_type) {
          case DE500AA :
             cmdcsr = TU_CSR6_FASTA | TU_CSR6_FD | TU_CSR6_INIT ;
             csr12  = TU_MII_CR_100 | TU_MII_CR_FDX;
             break;

	  case DE500FA  :
	     outcfgl((struct pb *)tupb, TU_PCFDA_ADDR, TU_K_OS_FFX);

          case DE500BA :
          case TU_21143 :
             cmdcsr = TU_CSR6_PS | TU_CSR6_HBD | TU_CSR6_PCS | TU_CSR6_SCR | TU_CSR6_FD | TU_CSR6_MBO | TU_CSR6_INIT ;
/*             cmdcsr = TU_CSR6_FAST | TU_CSR6_FD | TU_CSR6_MBO | TU_CSR6_INIT ;*/
             csr13  = 0x00000001;
             csr14  = 0x00000000;
             csr15  = 0x00050008;
             break;

          case P2SEPLUS :
          case DE500XA :
             cmdcsr = TU_CSR6_FAST | TU_CSR6_FD | TU_CSR6_INIT ;
             csr12  = TU_CSR12_INIT_FAST_FD;
             break;

          default:
             err_printf("Invalid mode\n");
             break;
       }
    }
    if (tu_mode == TU_K_TWISTED_FD) {
       switch ( tupb->controller_type )     {
          case DE500AA :
             cmdcsr = TU_CSR6_SLOWA | TU_CSR6_FD | TU_CSR6_INIT ;
             csr12  =  TU_MII_CR_10 | TU_MII_CR_FDX;
             break;

          case DE500XA :		
          case P2SEPLUS :
             cmdcsr = TU_CSR6_SLOW | TU_CSR6_FD | TU_CSR6_INIT ;
             csr12  = TU_CSR12_INIT_SLOW_FD;
             break;

          case DE500BA :
          case TU_21143 :
             cmdcsr = TU_CSR6_SLOW | TU_CSR6_FD | TU_CSR6_INIT ;
             csr13 = 0xffff0001;
             csr14 = 0x00007f3d;
             csr15 = 0xffff0008;
             break;

          case TU_21040 :
             csr13 = 0xffff0f01;
             csr14 = 0xfffffffd;
             csr15 = 0xffff0000;
             break;

          case TU_21041 :
             csr15 = TU_CSR15_UTP_INIT_PG3;
             csr13 = TU_CSR13_UTP_INIT_PG3;
             csr14 = TU_CSR14_FDX_INIT_PG3;
             break;
       }
    }
    if (tu_mode == TU_K_TWISTED)  {
       switch (tupb->controller_type ) {
          case DE500AA :
             cmdcsr = TU_CSR6_SLOWA | TU_CSR6_INIT ;
             csr12  = TU_MII_CR_10;
             break;

          case P2SEPLUS :
          case DE500XA :
             cmdcsr = TU_CSR6_SLOW | TU_CSR6_INIT ;
             csr12  = TU_CSR12_INIT_SLOW_HD;
             break;

          case DE500BA :
          case TU_21143 :
             cmdcsr = TU_CSR6_SLOW | TU_CSR6_INIT ;
             csr13 = 0x0001;
             csr14 = 0x7f3f;
             csr15 = 0x0008;
             break;

          case TU_21040 :
             csr13 = 0xffff8f01;
             csr14 = 0xfffffffd;
             csr15 = 0xffff0000;
             break;

          case TU_21041 :
             csr15 = TU_CSR15_UTP_INIT_PG3;
             csr13 = TU_CSR13_UTP_INIT_PG3;
             csr14 = TU_CSR14_UTP_INIT_PG3;
             break; 
       }
    }
    if (tu_mode == TU_K_AUI) {
       switch (tupb->controller_type ) {
          case DE500AA :
             cmdcsr = TU_CSR6_SLOWA | TU_CSR6_INIT ;
             csr12  = TU_MII_CR_10;
             break;

          case P2SEPLUS :
          case DE500XA :
             cmdcsr = TU_CSR6_SLOW | TU_CSR6_INIT ;
             csr12  = TU_CSR12_INIT_SLOW_HD;
             break;

          case DE500BA :
          case TU_21143 :
          case TU_21040 :
             cmdcsr = TU_CSR6_SLOW | TU_CSR6_INIT ;
             csr13 = 0xffff8f09;
             csr14 = 0xffff0705;
             csr15 = 0xffff000e;
             break;

          case TU_21041 :
             csr15 = TU_CSR15_AUI_INIT_PG3;
             csr13 = TU_CSR13_AUI_INIT_PG3;
             csr14 = TU_CSR14_AUI_INIT_PG3;
             break;
       }
    }
    if (tu_mode == TU_K_BNC) {
       switch (tupb->controller_type ) {
          case DE500AA :
             cmdcsr = TU_CSR6_SLOWA | TU_CSR6_INIT ;
             csr12  = TU_MII_CR_10;
             break;

          case P2SEPLUS :
             cmdcsr = TU_CSR6_SLOW | TU_CSR6_INIT ;
             /* The p2se+ uses bit 4 of gpp (csr12) for using bnc mode */
             csr12  = TU_CSR12_INIT_BNC; 
             break;

          case DE500XA :
             cmdcsr = TU_CSR6_SLOW | TU_CSR6_INIT ;
             csr12  = TU_CSR12_INIT_SLOW_HD;
             break;

          case TU_21040 :
          case TU_21041 :
          case DE500BA :
          case TU_21143 :
#if MTU
             /* MTU uses an external SIA for BNC (Thin-wire) */
             csr13 = TU_CSR13_OE13|TU_CSR13_IE|TU_CSR13_SIM|TU_CSR13_SRL;  /* 0x3041 */	
             csr14 = 0xffff0000;
             csr15 = 0xffff0006;
#else
             csr13 = 0xffffef09;
             csr14 = 0xffff0705;
             csr15 = 0xffff0006;
#endif
       }
    }

    switch (tupb->controller_type ) {
	  case DE500AA  :
             krn$_sleep(10);
             WRT_TULIP_CSR_LONG(tupb, TU_CSR6_ADDR, cmdcsr);
             tu_mii_wr(tupb, TU_MII_CR, csr12);
             break;

	  case DE500XA  :
	  case P2SEPLUS :
             WRT_TULIP_CSR_LONG(tupb, TU_CSR6_ADDR, cmdcsr);
             WRT_TULIP_CSR_LONG(tupb, TU_CSR12_ADDR, csr12);
             break;

	  case DE500FA :
             cmdcsr = cmdcsr & (~TU_CSR6_SCR);
	  case DE500BA :
          case TU_21143 :
             WRT_TULIP_CSR_LONG(tupb, TU_CSR13_ADDR, 0x00000000);
             WRT_TULIP_CSR_LONG(tupb, TU_CSR6_ADDR, cmdcsr);
             tu_mii_wr(tupb, TU_MII_CR, csr12);
	     WRT_TULIP_CSR_LONG(tupb, TU_CSR15_ADDR, 0x08a50000);
             WRT_TULIP_CSR_LONG(tupb, TU_CSR15_ADDR, 0x00000000);
             WRT_TULIP_CSR_LONG(tupb, TU_CSR15_ADDR, 0x00050000);
             WRT_TULIP_CSR_LONG(tupb, TU_CSR15_ADDR, csr15);
             WRT_TULIP_CSR_LONG(tupb, TU_CSR14_ADDR, csr14);
             WRT_TULIP_CSR_LONG(tupb, TU_CSR13_ADDR, csr13);
             break;

	  case TU_21040 :
	  case TU_21041 :
	     WRT_TULIP_CSR_LONG(tupb, TU_CSR13_ADDR, 0x00000000);
             WRT_TULIP_CSR_LONG(tupb, TU_CSR14_ADDR, csr14);
	     WRT_TULIP_CSR_LONG(tupb, TU_CSR15_ADDR, csr15);
	     WRT_TULIP_CSR_LONG(tupb, TU_CSR13_ADDR, csr13);
	     break;
    }

/*Save the new value of csr13*/

      tupb->csr13 = csr13;

/*Save the last auto sense mode */

      tupb->last_auto_mode = tu_mode;

/*Reset the local error counts*/

     tupb->lkf = tupb->ato = tupb->anc = tupb->aoc = 0;
}

/*+
 * ============================================================================
 * = tu_rx_count - Count the receive.                                         =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This handle the receive mop counters.
 *
 * FORM OF CALL:
 *
 *	tu_rx_count (tupb,rdes); 
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	struct TUPB *tupb - Pointer to the tu port block.
 *	PTU_RING rdes - Pointer to the receive descriptor;
 *
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void tu_rx_count(struct TUPB *tupb, struct _TU_RING *rdes)
{
    unsigned long csr8;

/*Count the missed frames*/

    csr8 = RD_TULIP_CSR_LONG(tupb, TU_CSR8_ADDR);
    tupb->mc->MOP_V4CNT_NO_SYSTEM_BUFFER += csr8 & TU_CSR8_MFC;

/*If it's an error count the error if not count the packet*/
/*Be sure that this packet has vaild info by checking for the LS bit*/

    if (rdes->tu_flags & TU_RDES_LS) {
	if (rdes->tu_flags & TU_RDES_ES) {

/*Count the errors*/

	    if ((rdes->tu_flags & TU_CE) && (!(rdes->tu_flags & TU_DB)))
		tupb->mc->MOP_V4CNT_RX_FAIL_BLOCK_CHECK++;
	    if ((rdes->tu_flags & TU_CE) && (rdes->tu_flags & TU_DB))
		tupb->mc->MOP_V4CNT_RX_FAIL_FRAMING_ERR++;
	    if (rdes->tu_flags & TU_TL)
		tupb->mc->MOP_V4CNT_RX_FAIL_LONG_FRAME++;
	    if (rdes->tu_flags & TU_OF)
		tupb->mc->MOP_V4CNT_DATA_OVERRUN++;
	} else {

/*Check for multicast*/

	    if (rdes->tu_flags & TU_MF) {

/*Count the number of bytes*/

		tupb->mc->MOP_V4CNT_RX_MCAST_BYTES +=
		  (rdes->tu_flags & TU_RDES_FL_MSK) >> TU_RDES_FL_SFT;

/*Count the frames*/

		if (rdes->tu_info & TU_RDES_LS)
		    tupb->mc->MOP_V4CNT_RX_MCAST_FRAMES++;
	    } else {

/*Count the number of bytes*/

		tupb->mc->MOP_V4CNT_RX_BYTES +=
		  (rdes->tu_flags & TU_RDES_FL_MSK) >> TU_RDES_FL_SFT;

/*Count the frames*/

		if (rdes->tu_info & TU_RDES_LS)
		    tupb->mc->MOP_V4CNT_RX_FRAMES++;
	    }
	}
    }
}

#if STARTSHUT
/*+
 * ============================================================================
 * = tu_config - Find units and report them                                   =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine simply displays the device (unit) name and hardware address
 *	of the given DEMNA, if the verbose flag is set.
 *  
 * FORM OF CALL:
 *  
 *	tu_config(dev, verbose)
 *  
 * RETURNS:
 *
 *	msg_success - success
 *	msg_failure - failure
 *       
 * ARGUMENTS:
 *
 *	struct device *dev - Pointer to config table device entry.
 *	int verbose - verbose flag, controls printing of unit information.
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/

char *tu_modes[] = {			/* */
  ew_not_used,				/* 0 */
  ew_auto_sensing, 			/* 1 */
  ew_twisted_pair, 			/* 2 */
  ew_aui, 				/* 3 */
  ew_bnc, 				/* 4 */
  ew_twisted_pair_fd, 			/* 5 */
  ew_fast, 				/* 6 */
  ew_fastfd, 				/* 7 */
  ew_auto_negotiate			/* 8 */
};

int tu_config(struct device *dev, int verbose)
{
    struct TUPB *pb;

    pb = (void *) dev->devdep.io_device.devspecific;
    if (pb) {
	printf("%-18s %02X-%02X-%02X-%02X-%02X-%02X", pb->ip->name, pb->sa[0],
	  pb->sa[1], pb->sa[2], pb->sa[3], pb->sa[4], pb->sa[5]);
	printf("\t%s\n", tu_modes[pb->mode]);
    }
    return msg_success;
}
#endif

#if STARTSHUT
/*+
 * ============================================================================
 * = turbo_tu_setmode - change driver state				      =
 * ============================================================================
 *
 * OVERVIEW:                           
 *  
 * 	This routine  changes  the  state  of  the  port driver.  The following
 * 	states are relevant:
 *
 * 	    DDB$K_ASSIGN -  Initialize the port driver.
 *
 * 	    DDB$K_STOP -    Shuts down the port driver.
 *
 * 	    DDB$K_START -   Changes the port driver to interrupt mode.
 *
 *	    DDB$K_INTERRUPT - Same as DDB$K_START.
 *  
 * FORM OF CALL:
 *  
 *	turbo_tu_setmode(mode)
 *  
 * RETURNS:
 *
 *	msg_success - success
 *	msg_failure - failure
 *       
 * ARGUMENTS:
 *
 *	int mode    - Desired mode for port driver.
 *	struct device *dev - Pointer to config table device structure for the port.
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/

int turbo_tu_setmode(int mode, struct device *dev, struct FILE *fp, char *info)
{
    struct TUPB *pb;
    struct PCB *pcb;
    unsigned long csr6;

    pcb = getpcb();

#if DEBUG
    pb = (void *) dev->devdep.io_device.devspecific;
    dprintf("TULIP setmode: pb = %x, mode = %s, pid = %x, %s\n",
	p_args4(pb, ew_modes[mode], pcb->pcb$l_pid, pcb->pcb$b_name));
    if (pb)
	dprintf("  ref = %d, fcount = %d\n", p_args2(pb->pb.ref, pb->fcount));
#endif

    switch (mode) {

	case DDB$K_ASSIGN: 
	    turbo_tu_setmode(DDB$K_START, dev,0 ,0);
	    turbo_tu_setmode(DDB$K_STOP, dev,0 ,0);
	    break;

	case DDB$K_LOOPBACK_INTERNAL: 
	case DDB$K_LOOPBACK_EXTERNAL: 

	    if (dev->devdep.io_device.state == DDB$K_INTERRUPT) {
		err_printf("TULIP cannot enable loopback - port is in use.\n");
		return msg_failure;
	    }

	    if (turbo_tu_setmode(DDB$K_START, dev,0 ,0) != msg_success)
		return msg_failure;

	    pb = (void *) dev->devdep.io_device.devspecific;

/* set the loopback mode */

	    spinlock(&spl_kernel);
	    csr6 = RD_TULIP_CSR_LONG(pb, TU_CSR6_ADDR);
	    csr6 &= ~TU_CSR6_OM;
	    if (mode == DDB$K_LOOPBACK_INTERNAL)
		csr6 |= TU_OM6_INL;
	    else
		csr6 |= TU_OM6_EXL;
	    WRT_TULIP_CSR_LONG(pb, TU_CSR6_ADDR, csr6);
	    spinunlock(&spl_kernel);

	    dev->devdep.io_device.state = DDB$K_INTERRUPT;
	    dprintf("device state = %x\n", p_args1(dev->devdep.io_device.state));
	    dprintf("[1b] ref=%x fcount = %x\n", p_args2(pb->pb.ref, pb->fcount));

	    break;

	case DDB$K_OPEN: 
	case DDB$K_START: 
	case DDB$K_INTERRUPT: 

	    pb = (void *) dev->devdep.io_device.devspecific;
	    if (pb == 0) {
		dprintf("Starting TULIP...\n", p_args0);
		
/* Initialize the class driver */

#if RAWHIDE
		if (platform() == ISP_MODEL) {
		    pprintf("Tulip is not emulated\n");
		    return msg_failure;
		}
#endif
		ether_shared_adr = ovly_load("ETHER");
		if (!ether_shared_adr)
		    return msg_failure;

		net_shared_adr = ovly_load("NET");
		if (!net_shared_adr)
		    return msg_failure;
		net_ref_counter++;

		pb = turbo_ew_init_port(dev);
		if (!pb)
		    return msg_failure;

		tu_driver_init(pb);

		krn$_wait(&pb->tu_port_s);
		pb->pb.mode = DDB$K_INTERRUPT;
		dev->devdep.io_device.state = DDB$K_INTERRUPT;
		pb->pb.ref++;
		pb->fcount++;
		dprintf("[3] ref=%d fcount = %d\n", p_args2(pb->pb.ref, pb->fcount));
		krn$_post(&pb->tu_port_s);

	    } else {

		krn$_wait(&pb->tu_port_s);
		pb->pb.ref++;
		dprintf("device state = %x\n", p_args1(dev->devdep.io_device.state));
		if (dev->devdep.io_device.state == DDB$K_INTERRUPT) {
		    pb->fcount++;
		    dprintf("fcount=%d\n", p_args1(pb->fcount));
		}
		dprintf("[4] ref=%d fcount = %d\n", p_args2(pb->pb.ref, pb->fcount));
		krn$_post(&pb->tu_port_s);
	    }
	    break;

	case DDB$K_CLOSE: 
	case DDB$K_STOP: 

	    pb = (void *) dev->devdep.io_device.devspecific;
	    if (!pb) {
		dprintf("TULIP Setmode called without pb setup\n", p_args0);
		return msg_failure;
	    }

	    krn$_wait(&pb->tu_port_s);
	    pb->pb.ref--;
	    dprintf("device state = %x\n", p_args1(dev->devdep.io_device.state));
	    if (dev->devdep.io_device.state == DDB$K_INTERRUPT)
		pb->fcount--;
	    dprintf("[5] ref=%d fcount = %d\n", p_args2(pb->pb.ref, pb->fcount));

	    if ((pb->pb.ref == 1) && (pb->fcount == 0) && (dev->devdep.io_device.state == DDB$K_INTERRUPT)) {

		dprintf("Stopping TULIP...\n", p_args0);
		dev->devdep.io_device.state = DDB$K_STOP;
		dprintf("device state = %x\n", p_args1(dev->devdep.io_device.state));

		krn$_post(&pb->tu_port_s);

		ew_shutdown(pb);

		dev->devdep.io_device.devspecific = 0;

		net_ref_counter--;
		if (net_ref_counter == 0)
		    ovly_remove("NET");
	    } else {
		krn$_post(&pb->tu_port_s);
	    }
	    break;

        case DDB$K_MULTICAST:

            tu_open(fp,info);
            break;

	case DDB$K_POLLED: 
	default: 
	    err_printf("TULIP Error: setmode - illegal mode.\n");
	    return msg_failure;
	    break;
    }					/* switch */

#if DEBUG
    dprintf("Completed setmode: mode = %s, pid = %x, %s\n",
      p_args3(ew_modes[mode], pcb->pcb$l_pid, pcb->pcb$b_name));
    pb = (void *) dev->devdep.io_device.devspecific;
    if (pb)
	dprintf("  pb = %x, ref = %d, fcount = %d\n", p_args3(pb, pb->pb.ref,pb->fcount));
#endif

    return msg_success;
}
#endif

#if STARTSHUT
int turbo_ew_init(struct device *dev)
{
/*Init the port queue list*/

    tupblist.flink = &tupblist;
    tupblist.blink = &tupblist;

    return (msg_success);
}
#endif

#if STARTSHUT

struct tupb *turbo_ew_init_port(struct device *dev)
{
    int i, j;
    struct TUPB *pb;
    int vector_index;
    int int_pin;
    int subtype;
    char name[16];
    char value[32];
    struct EVNODE *evp;

/* Insure that we are in interrupt mode: */

    krn$_set_console_mode(INTERRUPT_MODE);

    pb = dev->devdep.io_device.devspecific;

    if (!pb) {
	pb = dyn$_malloc(sizeof(*pb), DYN$K_SYNC | DYN$K_NOOWN);
	dev->devdep.io_device.devspecific = pb;
    }

/* Initialize port semaphore: */

#if 0
    sprintf(name, "%s_port", pb->pb.port_name);
    krn$_seminit(&(pb->tu_port_s), 0, name);
    sprintf(name, "%s_ready", pb->pb.port_name);
    krn$_seminit(&(pb->tu_ready_s), 0, name);
#else
    krn$_seminit(&(pb->tu_port_s), 0, "tu_port_s");
    krn$_seminit(&(pb->tu_ready_s), 0, "tu_ready_s");
#endif

    pb->pb.ref = 1;			/* Init reference count */
    pb->fcount = 0;			/* Init open file count */

/* Load the base address from the device to pb */

    pb->pb.state = DDB$K_START;
    pb->pb.hose = dev->hose;
    pb->pb.slot = dev->slot;
    pb->pb.bus = dev->bus;
    pb->pb.type = TYPE_PCI;
    pb->pci_type = dev->dtype;
    pb->pb.function = dev->function;
    pb->pb.channel = dev->channel;
    pb->pb.controller = dev->devdep.io_device.controller;
    controller_num_to_id(pb->pb.controller, pb->pb.controller_id);
    pb->pb.config_device = dev;

/*    io_disable_interrupts( pb, pb->pb.vector ); */

    if (pci_get_base_address(dev, 1, &pb->pb.csr) != msg_success)
	return 0;

    pb->lwp = tu_send;

    sprintf(pb->pb.name, "ew%c0.0.%d.%d.%d", pb->pb.controller + 'a',
      pb->pb.channel,
      pb->pb.bus * 1000 + pb->pb.function * 100 + pb->pb.slot, pb->pb.hose);

    sprintf(pb->pb.alias, "EW%c0", pb->pb.controller + 'A');

/* setup ev table */

    sprintf(name, "ew%c0_mode", pb->pb.controller + 'a');
    ew_mode_ev.ev_name = name;

#if AUTO_SENSE
    ew_mode_ev.ev_value = ew_auto_sensing;
#else
    ew_mode_ev.ev_value = ew_twisted_pair;
#endif

#if TURBO
/* ITIOP tulips are always a twisted pair. */

    if ((dev->bridge->bridge->tbl->dtype == 0x2020) && (dev->hose % 4 == 0)) {
	ew_mode_ev.ev_value = ew_twisted_pair;
    }
#endif

/* Create mode ev if it doesn't exist */
/* if it does, setup table & write if they are zero */

    if ((ev_locate(&evlist, ew_mode_ev.ev_name, &evp)) == msg_success)
    {						/* found */
	if (evp->wr_action == 0) {
	    strncpy(value, evp->value.string, 31);
	    value[31] = '\0';
	    i = ovly_call_save("TULIP", tu_ev_write_$offset);
	    j = ovly_call_save("TULIP", ew_mode_table_$offset);
	    ew_mode_ev.ev_wr_action = i;
	    ew_mode_ev.validate = j;
	    ew_mode_ev.ev_value = value;
	    ev_init_ev(&ew_mode_ev, pb);
	}
    } else {					/* not found */
	i = ovly_call_save("TULIP", tu_ev_write_$offset);
	j = ovly_call_save("TULIP", ew_mode_table_$offset);
	ew_mode_ev.ev_wr_action = i;
	ew_mode_ev.validate = j;
	ev_init_ev(&ew_mode_ev, pb);
    }

/* Load the vector from device to pb */

    pb->pb.vector = pci_get_vector(dev);

    insert_pb(pb);

    krn$_post(&pb->tu_port_s);		/* Ready to access port */

    return pb;
}
#endif

#if ( STARTSHUT || DRIVERSHUT )

int ew_shutdown(struct TUPB *pb)
{
    int i;
    struct PBQ *pbe;			/* Pointer to a port block element*/
    struct NI_GBL *np;			/* Pointer to the NI global database */

/* Post the receive process so it can kill itself: */

/*Check for polled or not*/

    if (pb->pb.vector != 0) {
	io_disable_interrupts(pb, pb->pb.vector);
	int_vector_clear(pb->pb.vector);
    }
    WRT_TULIP_CSR_LONG(pb, TU_CSR0_ADDR, TU_CSR0_SWR);
    krn$_sleep(5);

/* Shutdown the datalink: */

    ndl_shutdown(pb->ip);

    pb->pb.state = DDB$K_STOP;

    krn$_bpost(&pb->rx_isr);

    krn$_post(&pb->tu_port_s);

    dprintf("waiting for cmplt_rx_s\n", p_args0);
    krn$_wait(&pb->cmplt_rx_s);

/* remove que if polled mode */

    if (pb->pb.vector == 0)
	remq_lock(&pb->pqp.flink);

    krn$_wait(&pb->tu_port_s);

/* Remove the port block from the queue: */

    pbe = findpbe(pb);

    if (pbe == 0) {
	dprintf("pbe not found for pb:%08x\n", p_args1(pb));
	return (msg_failure);
    } else {
	remq_lock(pbe);
	free(pb->tdes);

/* remove the tx info packets*/

	for (i = 0; i < tu_rcv_buf_no; i++) {
	    if ((pb->rdes)[i].tu_bfaddr1)
		free((pb->rdes)[i].tu_bfaddr1);
	}

	free(pb->rdes);

	for (i = 0; i < tu_xmt_buf_no; i++) {
	    krn$_stop_timer(&(pb->tx_isr[i]));
	    krn$_semrelease(&(pb->tx_isr[i].sem));
	    krn$_release_timer(&(pb->tx_isr[i]));
	}

	free(pb->tx_isr);
	free(pbe);
    }

/* Free structures allocated by the port driver: */

    np = (struct NI_GBL *) pb->ip->misc;
    free(np->enp);

    INODE_LOCK(pb->ip);

    pb->ip->misc = NULL;

    INODE_UNLOCK(pb->ip);

    if (np->dlp)
	free(np->dlp);

    free(np->mcp);
    free(np);

    krn$_semrelease(&pb->tu_port_s);
    krn$_semrelease(&pb->tu_ready_s);
    krn$_semrelease(&pb->cmplt_rx_s);
    krn$_semrelease(&pb->rx_isr);

#if !DRIVERSHUT
    remove_pb(pb);
#endif
    free(pb);

    dprintf("Completed shutdown\n", p_args0);
    return msg_success;
}
#endif

#if ( STARTSHUT || DRIVERSHUT )

struct PBQ *findpbe(struct pb *pb)
{
    struct PBQ *pbe;

    pbe = tupblist.flink;
    while (pbe != &(tupblist)) {
	dprintf("pbe:%08x  pbe->pb:%08x pb:%08x\n", p_args3(pbe, pbe->pb, pb));
	if (pbe->pb == pb)
	    return (pbe);
	pbe = pbe->flink;
    }
    return 0;
}
#endif

#if 0
/*
 * This routine is a quick start mode to get the ev's and setup the tulips
 * It is called by setmode assign.
*/

int turbo_ew_quick_start(struct device *dev)
{
    struct device *rbalist[20 * 12];
    int devcnt, i;
    struct TUPB *pb;
    int ew_media;

    pb = turbo_ew_init_port(dev);

/*Make sure the mode is read*/

    tu_read_mode(pb);

/*Set to the proper mode. */

    if (pb->mode == TU_K_TWISTED) {
	ew_media = TU_K_OS_TWISTED;
    } else if (pb->mode == TU_K_TWISTED_FD) {
	ew_media = TU_K_OS_TWISTED_FD;
    } else if (pb->mode == TU_K_AUI) {
	ew_media = TU_K_OS_AUI;
    } else if (pb->mode == TU_K_BNC) {
	ew_media = TU_K_OS_BNC;
    } else if (pb->mode == TU_K_FAST) {
	ew_media = TU_K_OS_F;
    } else if (pb->mode == TU_K_FASTFD) {
	ew_media = TU_K_OS_FFD;
    } else if (pb->mode == TU_K_AUTO_NEGOTIATE) {
	ew_media = TU_K_OS_AN;    
    } else {
	ew_media = TU_K_OS_NOT_USED;
    }
    outcfgl(pb, TU_PCFDA_ADDR, ew_media);

    krn$_semrelease(&pb->tu_port_s);

    krn$_semrelease(&pb->tu_ready_s);

    pb->pb.ref = 0;			/* not fully started */
    return 0;
}
#endif

unsigned long tu_getfrom_mii(struct TUPB *pb, unsigned long command)
{
	WRT_TULIP_CSR_LONG(pb, TU_CSR9_ADDR, command);
	krn$_micro_delay(1);
	WRT_TULIP_CSR_LONG(pb, TU_CSR9_ADDR, command | TU_MII_MDC);
	krn$_micro_delay(1);
	return ((RD_TULIP_CSR_LONG(pb, TU_CSR9_ADDR) >> 19 ) & 1);
}

void tu_sendto_mii(struct TUPB *pb , unsigned long command, unsigned long data)
{
        unsigned long j;

        j = (data & 1) << 17;
	WRT_TULIP_CSR_LONG(pb, TU_CSR9_ADDR, command | j);
	krn$_micro_delay(1);
	WRT_TULIP_CSR_LONG(pb, TU_CSR9_ADDR, command | TU_MII_MDC | j);
	krn$_micro_delay(1);
}

unsigned long tu_mii_rdata(struct TUPB *pb)
{
        unsigned long i, tmp = 0;

        for (i=0; i<16; i++) {
                tmp <<= 1;
                tmp |= tu_getfrom_mii(pb, TU_MII_MRD | TU_MII_RD);
        }

        return tmp;
}

void tu_mii_wdata(struct TUPB *pb, unsigned long data, unsigned long len)
{
        unsigned long i;

        for (i=0; i<len; i++) {
                tu_sendto_mii(pb, TU_MII_MWR | TU_MII_WR, data);
                data >>= 1;
        }
}

unsigned long tu_mii_swap(unsigned long data, unsigned long len)
{
        unsigned long i, tmp = 0;

        for (i=0; i<len; i++) {
                tmp <<= 1;
                tmp |= (data & 1);
                data >>= 1;
        }

        return tmp;
}

void tu_mii_turnaround(struct TUPB *pb, unsigned long rw)
{
        if (rw == TU_MII_STARTWR) {
                tu_sendto_mii(pb, TU_MII_MWR | TU_MII_WR, 1);
                tu_sendto_mii(pb, TU_MII_MWR | TU_MII_WR, 0);
        } else {
                tu_getfrom_mii(pb, TU_MII_MRD | TU_MII_RD);
        }
}
                                                                                
unsigned short tu_mii_rd(struct TUPB *pb, unsigned char phyreg)
{
unsigned char phyaddr = 5;
        tu_mii_wdata(pb, TU_MII_PREAMBLE, 2);
        tu_mii_wdata(pb, TU_MII_PREAMBLE, 32);
        tu_mii_wdata(pb, TU_MII_STARTRD, 4);
        tu_mii_wdata(pb, tu_mii_swap(phyaddr, 5), 5);
        tu_mii_wdata(pb, tu_mii_swap(phyreg, 5), 5);
        tu_mii_turnaround(pb, TU_MII_STARTRD);
        return (unsigned short)tu_mii_rdata(pb);
}

void tu_mii_wr(struct TUPB *pb, unsigned char phyreg, unsigned long data)
{
unsigned char phyaddr = 5;
        tu_mii_wdata(pb, TU_MII_PREAMBLE, 2);
        tu_mii_wdata(pb, TU_MII_PREAMBLE, 32);
        tu_mii_wdata(pb, TU_MII_STARTWR, 4);
        tu_mii_wdata(pb, tu_mii_swap(phyaddr, 5), 5);
        tu_mii_wdata(pb, tu_mii_swap(phyreg, 5), 5);
        tu_mii_turnaround(pb, TU_MII_STARTWR);
        tu_mii_wdata(pb, tu_mii_swap(data, 16), 16);
}
                                                            


void tu_flags_to_ability(struct TUPB *pb)
{

        ability = 0xf;

        if (!fast_speed)
                ability >>= 2;

        if (!full_duplex)
                ability >>= 1;
}

void tu_ability_to_flags(struct TUPB *pb , unsigned char abilities )
{
	fast_speed = 0; full_duplex = 0;
        if (abilities & 8) {
                fast_speed = 1;
                full_duplex = 1;
        }  
        if (abilities & 4)
           fast_speed = 1;
        if (abilities & 2)
           full_duplex = 1;
}



/* Initiates  auto negotiation  and determines common operation mode with */
/* remote  partner.							  */

unsigned short tu_auto_negotiate(struct TUPB *pb)
{
   volatile unsigned short mii_cr, mii_sr, mii_ar, mii_pr;
   unsigned short common_ability, count, status;
   unsigned long csr5, csr12, csr14, cmdcsr;

   status = 1;
   if (cbip)		/*If in callbacks don't do auto neg. It has already been done*/
      return msg_success;

    dprintf("\nStarting Auto-Negotiate: controller = %x\n", p_args1(pb->controller_type));

   /* try to auto negotiate 3 times */

   if ( pb->controller_type == DE500BA || pb->controller_type == TU_21143 ) {

	dprintf("DE500-BA or TU_21143\n", p_args0);

	    csr14 = 0x0003ffff;
	    cmdcsr = TU_CSR6_FD ;
	    csr5 = TU_CSR5_ANC ;

	    WRT_TULIP_CSR_LONG(pb, TU_CSR13_ADDR, 0x00000000);
	    WRT_TULIP_CSR_LONG(pb, TU_CSR15_ADDR, 0x08a50000);
	    WRT_TULIP_CSR_LONG(pb, TU_CSR15_ADDR, 0x00000000);
	    WRT_TULIP_CSR_LONG(pb, TU_CSR15_ADDR, 0x00050000);
	    WRT_TULIP_CSR_LONG(pb, TU_CSR5_ADDR, csr5);
	    WRT_TULIP_CSR_LONG(pb, TU_CSR6_ADDR, cmdcsr);
	    WRT_TULIP_CSR_LONG(pb, TU_CSR14_ADDR, csr14);
	    WRT_TULIP_CSR_LONG(pb, TU_CSR13_ADDR, 0x00000001);			

    dprintf("tu_auto_negotiate: sleep\n", p_args0);

	krn$_sleep(5000);

    dprintf("tu_auto_negotiate: after sleep\n", p_args0);

      count = 0;
      while (count < 6) {
         csr12 = RD_TULIP_CSR_LONG(pb, TU_CSR12_ADDR);
         if (csr12 &  0xD000) { 
            if (csr12 & 0x01000000) {		/* 100BaseTx FD */
               pb->fast_speed  = 1; 
               pb->full_duplex = 1; 
            }
            else if (csr12 & 0x00800000) {   	/* 100BaseTx HD */
               pb->fast_speed  = 1; 
               pb->full_duplex = 0;
            }
            else if (csr12 & 0x00400000) {	 /* 10BaseT FD */
               pb->fast_speed  = 0; 
               pb->full_duplex = 1; 
            }
            else if (csr12 & 0x00200000) {	/*  10BaseT HD */
               pb->fast_speed  = 0; 
               pb->full_duplex = 0;
            }
            else {				/* unknown abilities */
               status = 1; 
               count = 10; 
               break; 
            }
	    csr5 = RD_TULIP_CSR_LONG(pb, TU_CSR5_ADDR);
            WRT_TULIP_CSR_LONG(pb, TU_CSR5_ADDR, csr5 & TU_CSR5_ANC);
            status = 0;
            break;
         } else {
            count++;
	 }
         pprintf(".");
	 dprintf("tu_auto_negotiate: sleep 2000\n", p_args0);
         krn$_sleep(2000);
	 dprintf("tu_auto_negotiate: after sleep 2000\n", p_args0);
      }
      pprintf("\new%s0: link %s %s: %s duplex\n", pb->pb.controller_id,
         (status == 0) ? "up : Negotiated " : "failed : Using ",
         pb->fast_speed ? "100BaseTX" : "10BaseT",
         pb->full_duplex ? "full" : "half");
#if MONET
    if (!tt_reprompt)
	halt_reprompt();
#endif
      return (status);	
   } 					/* end of de500-ba if-statement */

    dprintf("DE500-AA\n", p_args0);

   while ((status < 4)&&(status != 0))   {
      fast_speed = pb->fast_speed ; 
      full_duplex = pb->full_duplex;

      while (1)     {
	 /* Reset the MII control register   */

         tu_mii_wr(pb, TU_MII_CR, TU_MII_CR_RST);
	 count = 0;
         while (count < 2) {
            krn$_sleep(512);
	    if( tu_mii_rd(pb, TU_MII_CR) & TU_MII_CR_RST)
	       count++; 
            else 
               break; 
	 }
	 
	 if (count >= 2) {
	    pprintf("PHY reset failed\n");
	    status ++; break; 
	 }

         /* Set the adapter abilities */

	dprintf("set adapter abilities\n", p_args0);

	 tu_flags_to_ability(pb);

	 /* Put the ability into the Ability Register */

         tu_mii_wr(pb, TU_MII_AR, (ability << 5) | 1);

	 /* Enable auto negotiation */

	dprintf("enable nway\n", p_args0);

         tu_mii_wr(pb, TU_MII_CR, TU_MII_CR_ANE | TU_MII_CR_RAN);

         krn$_sleep(512);
	 count = 0;
	 while (count < 4) {
	    krn$_sleep(512);
	    if (tu_mii_rd(pb, TU_MII_CR) & TU_MII_CR_RAN) 
	       count++;
	    else 
	       break;
	 }
	 if (count >=4) {
	    status ++; 
            break; 
	 }
	 count = 0;
	 while (count < 4) {
	    krn$_sleep(512);
	    mii_sr = tu_mii_rd(pb,TU_MII_SR);
	    if (mii_sr & TU_MII_SR_ANC) 
	       break;
	    else 
	       count++;
	 }
	dprintf("nway completed\n", p_args0);

	 if (count >=4) {
	    pprintf("Auto Negotiation restart failed\n");
	    status ++; 
            break; 
	 }
	 krn$_sleep(512);
         mii_sr = tu_mii_rd(pb, TU_MII_SR);
         mii_pr = tu_mii_rd(pb, TU_MII_PR);

         /*
          * If link-down, or remote-fault, or partner's ability is 0 or
          * not 802.3, then this completion isn't acceptable. Restart.
          */
         if (!(mii_sr & TU_MII_SR_LKS)||(mii_sr & TU_MII_SR_RFD) ||
          ((mii_pr & 0x1E0) == 0) || ((mii_pr & 0x1F) != 0x01)) {
	    pprintf ("Auto Negotiation error\n");
	    status ++; 
            break;
	 }	
         common_ability = ability & ((mii_pr & 0x01E0) >> 5) ;
         if (common_ability) {
            tu_ability_to_flags(pb, common_ability);
         }
         else {
	    pprintf("No Common Abilities with partner\n");
	    status = 4; 
            break;
         }
	 status  = 0 ;
	 break;

      } /* End of while (1)  loop */

   } /* End of while status loop */

   pprintf("\new%s0: link %s %s: %s duplex\n",
	  pb->pb.controller_id,
	  (status == 0) ? "up : Negotiated " : "failed : Using ",
          fast_speed ? "100BaseTX" : "10BaseT",
          full_duplex ? "full" : "half");
#if MONET
    if (!tt_reprompt)
	halt_reprompt();
#endif

    pb->fast_speed = fast_speed; 
    pb->full_duplex = full_duplex;
    return (status);
}

/* Read the SROM  words into an an array 

    Inputs:
	struct TUPB *tupb
	start		 - word count to start reading at
	count		 - # of words to read
	ni		 - pointer to an array big enough to hold the data

*/
#if DRIVERSHUT
/*+
 * ============================================================================
 * = tu_config - Find units and report them                                   =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine simply displays the device (unit) name and hardware address
 *	of the given DEMNA, if the verbose flag is set.
 *  
 * FORM OF CALL:
 *  
 *	tu_config(pb, verbose)
 *  
 * RETURNS:
 *
 *	msg_success - success
 *	msg_failure - failure
 *       
 * ARGUMENTS:
 *
 *	struct device *dev - Pointer to config table device entry.
 *	int verbose - verbose flag, controls printing of unit information.
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/

char *tu_modes[] = {			/* */
  ew_not_used,				/* 0 */
  ew_auto_sensing, 			/* 1 */
  ew_twisted_pair, 			/* 2 */
  ew_aui, 				/* 3 */
  ew_bnc, 				/* 4 */
  ew_twisted_pair_fd, 			/* 5 */
  ew_fast, 				/* 6 */
  ew_fastfd, 				/* 7 */
  ew_auto_negotiate			/* 8 */
};

int tu_config(struct pb *gpb, int verbose)
{
    struct TUPB *pb;

    pb = gpb->dpb;
    if (pb) {
	printf("%-18s %02X-%02X-%02X-%02X-%02X-%02X", pb->ip->name, pb->sa[0],
	  pb->sa[1], pb->sa[2], pb->sa[3], pb->sa[4], pb->sa[5]);
	printf("\t%s\n", tu_modes[pb->mode]);
    }
    return msg_success;
}

/*+
 * ============================================================================
 * = turbo_tu_setmode - change driver state				      =
 * ============================================================================
 *
 * OVERVIEW:                           
 *  
 * 	This routine  changes  the  state  of  the  port driver.  The following
 * 	states are relevant:
 *
 * 	    DDB$K_ASSIGN -  Initialize the port driver.
 *
 * 	    DDB$K_STOP -    Shuts down the port driver.
 *
 * 	    DDB$K_START -   Changes the port driver to interrupt mode.
 *
 *	    DDB$K_INTERRUPT - Same as DDB$K_START.
 *  
 * FORM OF CALL:
 *  
 *	turbo_tu_setmode(mode)
 *  
 * RETURNS:
 *
 *	msg_success - success
 *	msg_failure - failure
 *       
 * ARGUMENTS:
 *
 *	int mode    - Desired mode for port driver.
 *	struct device *dev - Pointer to config table device structure for the port.
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/

int turbo_tu_setmode(int mode, struct pb *gpb)
{
    struct TUPB *pb;
    struct PCB *pcb;
    unsigned long csr6;

    pcb = getpcb();

#if DEBUG
    dprintf("TULIP setmode: pb = %x, mode = %s, pid = %x, %s\n",
	p_args4(pb, ew_modes[mode], pcb->pcb$l_pid, pcb->pcb$b_name));
    if (pb)
	dprintf("  ref = %d, fcount = %d\n", p_args2(pb->pb.ref, pb->fcount));
#endif

    switch (mode) {

	case DDB$K_ASSIGN: 
	    turbo_tu_setmode(DDB$K_START, gpb);
	    turbo_tu_setmode(DDB$K_STOP, gpb);
	    break;

	case DDB$K_LOOPBACK_INTERNAL: 
	case DDB$K_LOOPBACK_EXTERNAL: 

	    if (pb->pb.mode == DDB$K_INTERRUPT) {
		err_printf("TULIP cannot enable loopback - port is in use.\n");
		return msg_failure;
	    }

	    if (turbo_tu_setmode(DDB$K_START, pb) != msg_success)
		return msg_failure;

/* set the loopback mode */

	    spinlock(&spl_kernel);
	    csr6 = RD_TULIP_CSR_LONG(pb, TU_CSR6_ADDR);
	    csr6 &= ~TU_CSR6_OM;
	    if (mode == DDB$K_LOOPBACK_INTERNAL)
		csr6 |= TU_OM6_INL;
	    else
		csr6 |= TU_OM6_EXL;
	    WRT_TULIP_CSR_LONG(pb, TU_CSR6_ADDR, csr6);
	    spinunlock(&spl_kernel);

	    pb->pb.mode = DDB$K_INTERRUPT;
	    dprintf("[1b] ref=%x fcount = %x\n", p_args2(pb->pb.ref, pb->fcount));
	    break;

	case DDB$K_OPEN: 
	case DDB$K_START: 
	case DDB$K_INTERRUPT: 

	    if ( !gpb->dpb ) {
		dprintf("Starting TULIP...\n", p_args0);
		
/* Initialize the class driver */

		ether_shared_adr = ovly_load("ETHER");
		if (!ether_shared_adr)
		    return msg_failure;

		net_shared_adr = ovly_load("NET");
		if (!net_shared_adr)
		    return msg_failure;
		net_ref_counter++;

		pb = turbo_ew_init_port(gpb);
		if (!pb)
		    return msg_failure;
		pb->pb.config_device = DDB$K_STOP;
		tu_driver_init(pb);

		krn$_wait(&pb->tu_port_s);
		pb->pb.config_device = DDB$K_INTERRUPT;
		pb->pb.ref++;
		pb->fcount++;
		dprintf("[3] ref=%d fcount = %d\n", p_args2(pb->pb.ref, pb->fcount));
		krn$_post(&pb->tu_port_s);

	    } else {

		pb = gpb->dpb;
		krn$_wait(&pb->tu_port_s);
		gpb->dpb->ref++;
		if (pb->pb.config_device == DDB$K_INTERRUPT) {
		    pb->fcount++;
		    dprintf("fcount=%d\n", p_args1(pb->fcount));
		}
		dprintf("[4] ref=%d fcount = %d\n", p_args2(pb->pb.ref, pb->fcount));
		krn$_post(&pb->tu_port_s);
	    }
	    break;

	case DDB$K_CLOSE: 
	case DDB$K_STOP: 

	    pb = gpb->dpb;

	    if (!pb) {
		dprintf("TULIP Setmode called without pb setup\n", p_args0);
		return msg_failure;
	    }
	    krn$_wait(&pb->tu_port_s);
	    pb->pb.ref--;
	    dprintf("device state = %x\n", p_args1(pb->pb.config_device));
	    if (pb->pb.config_device == DDB$K_INTERRUPT)
		pb->fcount--;
	    dprintf("[5] ref=%d fcount = %d\n", p_args2(pb->pb.ref, pb->fcount));

	    if ((pb->pb.ref == 1) && (pb->fcount == 0) && (pb->pb.config_device == DDB$K_INTERRUPT)) {

		dprintf("Stopping TULIP...\n", p_args0);
		pb->pb.config_device = DDB$K_STOP;
		dprintf("device state = %x\n", p_args1(pb->pb.config_device));

		krn$_post(&pb->tu_port_s);

		ew_shutdown(pb);

		gpb->dpb = 0;

		net_ref_counter--;
		if (net_ref_counter == 0)
		    ovly_remove("NET");
	    } else {
		krn$_post(&pb->tu_port_s);
	    }
	    break;

	case DDB$K_POLLED: 
	default: 
	    err_printf("TULIP Error: setmode - illegal mode.\n");
	    return msg_failure;
	    break;
    }					/* switch */

#if DEBUG
    dprintf("Completed setmode: mode = %s, pid = %x, %s\n",
      p_args3(ew_modes[mode], pcb->pcb$l_pid, pcb->pcb$b_name));
    if (pb)
	dprintf("  pb = %x, ref = %d, fcount = %d\n", p_args3(pb, pb->pb.ref,pb->fcount));
#endif

    return msg_success;
}

int turbo_ew_init()
{
/*Init the port queue list*/

    tupblist.flink = &tupblist;
    tupblist.blink = &tupblist;

    return (msg_success);
}

struct tupb *turbo_ew_init_port(struct pb *gpb)
{
    struct TUPB *pb;
    struct EVNODE *evp;
    int i, j;
    int int_pin;
    char name[16];
    char value[32];

/* Insure that we are in interrupt mode: */

    krn$_set_console_mode(INTERRUPT_MODE);

    if ( !gpb->dpb ) {
	alloc_dpb( gpb, &pb, sizeof( struct TUPB ));
    }

/* Initialize port semaphore: */

    krn$_seminit(&(pb->tu_port_s), 0, "tu_port_s");
    krn$_seminit(&(pb->tu_ready_s), 0, "tu_ready_s");

    pb->pb.ref = 1;			/* Init reference count */
    pb->fcount = 0;			/* Init open file count */

/* Load the base address from the device to pb */

    pb->pb.state = DDB$K_START;
    pb->pci_type = 0;
    pb->pci_type = incfgl(pb, 0);
    subtype = incfgl(pb, 0x2c);

/*    io_disable_interrupts( pb, pb->pb.vector ); */

    pb->lwp = tu_send;

    sprintf(pb->pb.name, "ew%c0.0.%d.%d.%d", pb->pb.controller + 'a',
      pb->pb.channel,
      pb->pb.bus * 1000 + pb->pb.function * 100 + pb->pb.slot, pb->pb.hose);

    sprintf(pb->pb.alias, "EW%c0", pb->pb.controller + 'A');

/* setup ev table */

    sprintf(name, "ew%c0_mode", pb->pb.controller + 'a');
    ew_mode_ev.ev_name = name;

#if AUTO_SENSE
    ew_mode_ev.ev_value = ew_auto_sensing;
#else
    ew_mode_ev.ev_value = ew_twisted_pair;
#endif

/* Default to 100BaseTx - fast for DE500-FA */
    if ( subtype = 0x500f1011 )
	ew_mode_ev.ev_value = ew_fast; 

/* Create mode ev if it doesn't exist */
/* if it does, setup table & write if they are zero */

    if ((ev_locate(&evlist, ew_mode_ev.ev_name, &evp)) == msg_success)
    {						/* found */
	if (evp->wr_action == 0) {
	    strncpy(value, evp->value.string, 31);
	    value[31] = '\0';
	    i = ovly_call_save("TULIP", tu_ev_write_$offset);
	    j = ovly_call_save("TULIP", ew_mode_table_$offset);
	    ew_mode_ev.ev_wr_action = i;
	    ew_mode_ev.validate = j;
	    ew_mode_ev.ev_value = value;
	    ev_init_ev(&ew_mode_ev, pb);
	}
    } else {					/* not found */
	i = ovly_call_save("TULIP", tu_ev_write_$offset);
	j = ovly_call_save("TULIP", ew_mode_table_$offset);
	ew_mode_ev.ev_wr_action = i;
	ew_mode_ev.validate = j;
	ev_init_ev(&ew_mode_ev, pb);
    }

    krn$_post(&pb->tu_port_s);		/* Ready to access port */

    return pb;
}
#endif
