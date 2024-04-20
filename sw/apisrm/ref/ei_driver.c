/* file:	ei_driver.c
 *
 * Copyright (C) 1998 by
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
 *	Port driver for the Intel 82558 fast ethernet chip & 
 *	EtherExpress Pro/100+
 *
 *  AUTHORS:
 *
 *	David Golden
 *
 *  CREATION DATE:
 *  
 *      29-Jun-1998
 *
 *  MODIFICATIONS
 *
 *
 *--
 */

/*Include files*/

#include "cp$src:platform.h"
#include "cp$src:common.h"		/*Common definitions*/
/* #include "cp$src:platform_cpu.h" */
#include "cp$src:kernel_def.h"		/*Kernel definitions*/
#include "cp$src:msg_def.h"		/*Message definitions*/
#include "cp$src:prdef.h"		/*Processor definitions*/
#include "cp$src:dynamic_def.h"		/*Memory allocation defs*/
#include "cp$src:pb_def.h"		/*Port block definitions*/
#include "cp$src:ni_dl_def.h"		/*Datalink definitions*/
#include "cp$src:ni_env.h"		/*NI Environment variables*/
#include "cp$src:ev_def.h"		/*Environment vars*/
#include "cp$src:mop_counters64.h"	/*MOP counters*/
#include "cp$src:i82558.h"
#include "cp$src:i82558_pb_def.h"
#include "cp$src:probe_io_def.h"
#include "cp$inc:platform_io.h"
#include "cp$src:ctype.h"
#include "cp$inc:kernel_entry.h"
#if RAWHIDE
#include "cp$src:rawhide.h"
#endif
#if ( STARTSHUT || DRIVERSHUT )
#include "cp$src:mem_def.h"		/* get the device structure */
#endif
#include "cp$inc:prototypes.h"		/* MUST BE LAST */

#if MODULAR
#undef ei_init
#undef ei_ev_write
#undef ei_mode_table
#undef ei_read
#endif

#define DEBUG	0

/* Debug routines */

/*
 * debug flag
 * 1 = top level
 */

int xp_debug = 0;

#if DEBUG
#include "cp$src:print_var.h"
#define dqprintf _dqprintf
#define dprintf(fmt, args)	\
    _dprintf(fmt, args)
#else
#define dqprintf qprintf
#define dprintf(fmt, args)
#endif

#if DEBUG

static void _dprintf(fmt, a, b, c, d, e, f, g, h, i, j)
{
    if ((xp_debug & 1) != 0) {
	if ((xp_debug & 0x100) == 0)
	    pprintf(fmt, a, b, c, d, e, f, g, h, i, j);
	else
	    qprintf(fmt, a, b, c, d, e, f, g, h, i, j);
    }
}
static void _dqprintf(fmt, a, b, c, d, e, f, g, h, i, j)
{
    if (xp_debug)
	_dprintf(fmt, a, b, c, d, e, f, g, h, i, j);
    else
	qprintf(fmt, a, b, c, d, e, f, g, h, i, j);
}
#endif

/*Local definitions*/

#define malloc(x) dyn$_malloc(x,DYN$K_FLOOD|DYN$K_SYNC|DYN$K_NOOWN)
#define XP_CSR_READ_BYTE(p, o)	inportb((p), ((p)->pb.csr) | (o))
#define XP_CSR_READ_WORD(p, o)	inportw((p), ((p)->pb.csr) | (o))
#define XP_CSR_READ_LONG(p, o)	inportl((p), ((p)->pb.csr) | (o))
#define XP_CSR_WRITE_BYTE(p, o, v) outportb((p), ((p)->pb.csr) | (o), (v))
#define XP_CSR_WRITE_WORD(p, o, v) outportw((p), ((p)->pb.csr) | (o), (v))
#define XP_CSR_WRITE_LONG(p, o, v) outportl((p), ((p)->pb.csr) | (o), (v))

#if ( STARTSHUT || DRIVERSHUT )
#if TURBO
#define WINDOW_BASE(basepb)		((u_long)io_get_window_base(basepb))
#else
#define WINDOW_BASE(basepb)		(0)
#endif
#else
#define WINDOW_BASE(basepb)		((u_long)io_get_window_base(basepb))
#endif

#if ( STARTSHUT || DRIVERSHUT )
extern struct EVNODE evlist;		/* header of the list of variables */
extern struct window_head config;
extern int net_ref_counter;

#if DEBUG
char *ei_modes[] = {			/* */
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
#endif

/*Globals to be used as environment variables */

long int ei_xmt_buf_no = EI_XMT_BUF_CNT;	/* Number of transmit buffers */
long int ei_rcv_buf_no = EI_RCV_BUF_CNT;	/* Number of receive buffers */
long int ei_xmt_mod = EI_XMT_MOD;	/* XMT Descriptor alignment modulus*/
long int ei_xmt_rem = EI_XMT_REM;	/* XMT Descriptor alignment remainder */
long int ei_rcv_mod = EI_RCV_MOD;	/* RCV Descriptor alignment modulus */
long int ei_rcv_rem = EI_RCV_REM;	/* RCV Descriptor alignment remiander */
long int ei_driver_flags = 0;		/* driver flags */
long int ei_msg_buf_size = EI_MSG_BUF_SIZE;	/* message buffer size */
long int ei_msg_mod = EI_MSG_MOD;	/* Message modulus */
long int ei_msg_rem = EI_MSG_REM;	/* Message Remainder */

/* string used to format the eia0_mode environment var (over and over) */

static const char EI_MODE_FMT_STR[] = "%4.4s_mode";
static const char EI_ADDR_FMT_STR[] = "%02X-%02X-%02X-%02X-%02X-%02X";

/*External references*/

extern int cb_ref;			/*Call backs flag*/
extern int spl_kernel;			/*Kernel spinlock       */
extern struct QUEUE pollq;		/*POLLQ                 */
extern int null_procedure();

#if !MODULAR
extern int ev_sev_init();
#endif

#if !MODULAR || RAWHIDE	    /* need protos for rawhide */
/* function prototypes */

int ei_init();
void ei_init_pb(struct I82558PB *pb);
int ei_driver_init(struct I82558PB *xppb);
void ei_software_init(struct INODE *ip);
int ei_hardware_init(struct INODE *ip);
int ei_init_device(struct I82558PB *xppb);
int ei_get_nirom(struct I82558PB *xppb, unsigned char *ni);
void ei_ev_write(char *name, struct EVNODE *ev);
void ei_read_mode(struct I82558PB *xppb);
int ei_init_media(struct I82558PB *xppb);
int ei_mdi_read(struct I82558PB *xppb, unsigned int addr, unsigned int offset);
int ei_mdi_write(struct I82558PB *xppb, unsigned int addr, unsigned int offset, unsigned short data);
int ei_setup_phy(struct I82558PB *xppb, int phy_num);
void ei_interrupt(struct NI_GBL *np);
#if !( STARTSHUT || DRIVERSHUT )
int ei_read();
int ei_setmode();
#endif
int ei_write();
int ei_open();
void ei_poll(struct NI_GBL *np);
int ei_rx();
int ei_show();
int ei_change_mode();
int ei_dump_csrs();
void ei_dumppb(struct I82558PB *xppb);
#endif

#if ( STARTSHUT )
struct I82558PB *ei_init_port(struct device *dev);
struct PBQ *findpbe(struct pb *pb);
#endif

/* local stuff */

char ei_not_used[] = "Not-Used";
char ei_auto_sensing[] = "Auto-Sensing";
char ei_aui[] = "AUI";
char ei_fast[] = "Fast";
char ei_twisted_pair[] = "Twisted-Pair";
char ei_twisted_pair_fd[] = "Full Duplex, Twisted-Pair";
char ei_bnc[] = "BNC";
char ei_fastfd[] = "FastFD (Full Duplex)";
char ei_auto_negotiate[] = "Auto-Negotiate";

struct set_param_table ei_mode_table[] = {
    {ei_auto_sensing, 0}, 		/* */
    {ei_twisted_pair, 0}, 		/* */
    {ei_twisted_pair_fd, 0}, 		/* */
    {ei_aui, 0}, 			/* */
    {ei_bnc, 0}, 			/* */
    {ei_fast, 0}, 			/* */
    {ei_fastfd, 0}, 			/* */
    {ei_auto_negotiate, 0}, 		/* */
    {0}					/* */
};

static int cbl_ev_sev_init(struct env_table *et, char *value)
{
    return ev_sev_init(et, value);
}

struct env_table ei_mode_ev = {		/* */
  0,
  ei_twisted_pair,
  EV$K_NODELETE | EV$K_STRING | EV$K_NONVOLATILE,
  ei_mode_table, ei_ev_write, 0, cbl_ev_sev_init};

#if ( STARTSHUT || DRIVERSHUT )
extern struct DDB *ei_ddb_ptr;
#else

struct DDB ei_ddb = {			/*                              */
  "ei", 				/* Calling name                 */
  ei_read, 				/* read routine                 */
  ei_write, 				/* write routine                */
  ei_open, 				/* open routine                 */
  null_procedure, 			/* close routine                */
  null_procedure, 			/* name expansion routine       */
  null_procedure, 			/* delete routine               */
  null_procedure, 			/* create routine               */
  ei_setmode, 				/* setmode                      */
  null_procedure, 			/* validation routine           */
  0, 					/* class specific use           */
  1, 					/* allows information           */
  0, 					/* must be stacked              */
  0, 					/* is a flash update driver     */
  0, 					/* is a block device            */
  0, 					/* not seekable                 */
  1, 					/* is an ethernet device        */
  0					/* is a filesystem driver       */
};
#endif

struct PBQ _align(QUADWORD) xppblist = {&xppblist, &xppblist, 0};
					/* port block list */

#if !( STARTSHUT || DRIVERSHUT )
/*+
 * ============================================================================
 * = ei_init - Initialization routine for the port driver                     =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine sniffs all slots and then calls the per
 *	slot initialization routine. This version is used for
 *	MONOLITHIC, non-DRIVERSHUT kernels.
 *
 * FORM OF CALL:
 *
 *	ei_init (); 
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
int ei_init()
{

    void ei_init_pb();

/*Init the port queue list*/

    xppblist.flink = &xppblist;
    xppblist.blink = &xppblist;

/*Find all 82558 chips */

    find_pb("ei", sizeof(struct I82558PB), ei_init_pb);

    return (msg_success);
}
#endif

/*+
 * ============================================================================
 * = ei_init_pb - Per slot initialization routine 		              =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine is called by a callback in the find_pb function
 *	to actually perform the call to the per slot device init.
 *
 * FORM OF CALL:
 *
 *	ei_init_pb (); 
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
void ei_init_pb(struct I82558PB *pb)
{
    char name[16];

    log_driver_init(pb);
    sprintf(name, "%s%c0_mode", pb->pb.protocol, pb->pb.controller + 'a');
    ei_mode_ev.ev_name = name;

/* Default to twisted-pair */
    ei_mode_ev.ev_value = ei_twisted_pair;

    ev_init_ev(&ei_mode_ev);

    pb->pb.mode = DDB$K_INTERRUPT;
    set_io_name(pb->pb.name, 0, 0, 0, pb);
    sprintf(pb->pb.alias, "__%c0", pb->pb.controller + 'A');
    pb->pb.alias[0] = toupper(pb->pb.protocol[0]);
    pb->pb.alias[1] = toupper(pb->pb.protocol[1]);

/*Init the module*/

    ei_driver_init(pb);
}

/*+
 * ============================================================================
 * = ei_driver_init - Initialization routine for I82558 driver                =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will initialize the I82558 driver. It calls routines
 *	that first initialize the sofware data structures, the hardware,
 *	and finally the upper driver layers.
 *
 * FORM OF CALL:
 *
 *	ei_driver_init (struct I82558PB xppb); 
 *
 * RETURNS:
 *
 *	int Status - msg_success or FALIURE.
 *
 * ARGUMENTS:
 *
 *      struct I82558PB *xppb - I82558 port block.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int ei_driver_init(struct I82558PB *xppb)
{
    int i;
    int ei_pid;
    struct INODE *ip;			/* Pointer to the INODE */
    struct NI_GBL *np;			/* Pointer to the NI global database */
    struct PORT_CALLBACK *cb;		/* Callback structure*/
    char name[32];			/*General holder for a name*/

    dprintf("I found an i82558 port name: %s %08x %08x\n",
      p_args3(xppb->pb.name, xppb->pb.csr, xppb->pb.vector));
    dprintf("slot %d hose: %d bus: %d channel: %d controller: %d\n",
      p_args5(xppb->pb.slot, xppb->pb.hose, xppb->pb.bus, xppb->pb.channel,
      xppb->pb.controller));

/*Allocate the Port Block*/

    np = (struct NI_GBL *) dyn$_malloc(sizeof(struct NI_GBL),
      DYN$K_FLOOD | DYN$K_SYNC | DYN$K_NOOWN);
    np->pbp = xppb;

/* Create an inode entry, thus making the device visible as a file. */

    allocinode(xppb->pb.name, 1, &ip);
#if ( STARTSHUT || DRIVERSHUT )
    ip->dva = ei_ddb_ptr;
#else
    ip->dva = &ei_ddb;
#endif
    ip->dev = xppb->pb.config_device;
    ip->attr = ATTR$M_READ | ATTR$M_WRITE;
    ip->loc = 0;
    ip->misc = (int *) np;
    INODE_UNLOCK(ip);

#if ( STARTSHUT || DRIVERSHUT )
    xppb->ip = ip;
#endif

/*Initialize the software data structures*/

    ei_software_init(ip);

/*Initialize the i82558 hardware*/

    if (ei_hardware_init(ip) != msg_success)
	return (msg_failure);

/*Setup the callback structure*/

    cb = (struct PORT_CALLBACK *) malloc(sizeof(struct PORT_CALLBACK));
    cb->st = ei_show;
    cb->cm = ei_change_mode;
    cb->ri = null_procedure;
    cb->re = null_procedure;
    cb->dc = ei_dump_csrs;

/*Fill in the station address and insert the port block*/

    sprintf(xppb->pb.info, EI_ADDR_FMT_STR, xppb->sa[0],
      xppb->sa[1], xppb->sa[2], xppb->sa[3], xppb->sa[4], xppb->sa[5]);

#if ( STARTSHUT || DRIVERSHUT )
    sprintf(xppb->pb.string, "%-18s %-8s  %20s", xppb->pb.name,
      xppb->pb.alias, xppb->pb.info);
#else
    sprintf(xppb->pb.string, "%-24s   %-8s   %24s", xppb->pb.name,
      xppb->pb.alias, xppb->pb.info);
#endif

/*Initialize the datalink*/

    ndl_init(ip, cb, xppb->sa, (int *) &(xppb->lrp));

/*Set the state flag*/

    xppb->state = XP_K_STARTED;

#if ( STARTSHUT || DRIVERSHUT )

/*Initialize receive process*/

    sprintf(name, "rx_isr_%4.4s", ip->name);
    krn$_seminit(&(xppb->rx_isr), 0, name);

    sprintf(name, "rx_%4.4s", ip->name);
    krn$_seminit(&(xppb->cmplt_rx_s), 0, "ei_cmplt_rx");

    dprintf("creating %s\n", p_args1(name));

    ei_pid = krn$_create(		/* */
      ei_rx, 				/* address of process */
      null_procedure, 			/* startup routine */
      &xppb->cmplt_rx_s, 		/* completion semaphore */
      5, 				/* process priority */
      1 << whoami(), 			/* cpu affinity mask */
      1024 * 4, 			/* stack size */
      name, 				/* process name */
      "tt", "r", 			/* stdin */
      "tt", "w", 			/* stdout */
      "tt", "w", 			/* stderr */
      ip);				/* argument */

    krn$_wait(&xppb->ei_ready_s);

    dprintf("%s pid = %x\n", p_args2(name, ei_pid));

    krn$_wait(&xppb->ei_port_s);
    xppb->pb.ref--;
    dprintf("[2] ref=%d fcount=%d\n", p_args2(xppb->pb.ref, xppb->fcount));
    krn$_post(&xppb->ei_port_s);
#endif

    return (msg_success);
}

/*+
 * ============================================================================
 * = ei_software_init - Initialization routine for i82558 driver data structures =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routines will initialize the software data structures needed
 *	by the i82558 driver.
 *
 * FORM OF CALL:
 *
 *	ei_software_init (ip) 
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

void ei_software_init(struct INODE *ip)
{
    struct PBQ *pbe;			/* Pointer to a port block element*/
    struct NI_GBL *np;			/* Pointer to the NI global database */
    struct I82558PB *xppb;		/* Pointer to the port block */
    struct NI_ENV *ev;			/*Pointer to the environment variables*/
    char name[32];			/*General holder for a name*/
    int i, ei_media;
    char var[EV_NAME_LEN];

    dprintf("ei_software_init\n", p_args0);

/*Allocate some memory and get some pointers*/

    np = (struct NI_GBL *) ip->misc;
    np->dlp = (int *) dyn$_malloc(sizeof(struct NI_DL),
      DYN$K_SYNC | DYN$K_NOOWN | DYN$K_FLOOD | DYN$K_ALIGN, 32, 0);
    np->enp = (int *) dyn$_malloc(sizeof(struct NI_ENV),
      DYN$K_SYNC | DYN$K_NOOWN | DYN$K_FLOOD | DYN$K_ALIGN, 32, 0);
    np->mcp = (int *) dyn$_malloc(sizeof(struct MOP$_V4_COUNTERS),
      DYN$K_SYNC | DYN$K_NOOWN | DYN$K_FLOOD | DYN$K_ALIGN, 32, 0);
    xppb = (struct I82558PB *) np->pbp;
    ev = (struct NI_ENV *) np->enp;
    xppb->mc = (struct MOP$_V4_COUNTERS *) np->mcp;

/*Get the memory for the Command and Receive Lists */

/* Transmit Command Blocks */

    if (ei_xmt_mod || ei_xmt_rem) {

	xppb->cbl_base = 
	  (struct xp_txcb *) dyn$_malloc(sizeof(struct xp_txcb) * ei_xmt_buf_no,
	  DYN$K_SYNC | DYN$K_NOOWN | DYN$K_FLOOD | DYN$K_ALIGN, ei_xmt_mod,
	  ei_xmt_rem);

    } else {

	xppb->cbl_base = 
	  (struct xp_txcb *) malloc(sizeof(struct xp_txcb) * ei_xmt_buf_no);

    }

/* Receive Frame Descriptors */

    if (ei_rcv_mod || ei_rcv_rem) {

	xppb->rfd_base = 
	  (struct xp_rfd *) dyn$_malloc(sizeof(struct xp_rfd) * ei_rcv_buf_no,
	  DYN$K_SYNC | DYN$K_NOOWN | DYN$K_FLOOD | DYN$K_ALIGN, ei_rcv_mod,
	  ei_rcv_rem);

    } else {

	xppb->rfd_base = 
	  (struct xp_rfd *) malloc(sizeof(struct xp_rfd) * ei_rcv_buf_no);

    }

/* Receive Buffer Descriptors */

    if (ei_rcv_mod || ei_rcv_rem) {

	xppb->rbd_base = 
	  (struct xp_rbd *) dyn$_malloc(sizeof(struct xp_rbd) * ei_rcv_buf_no,
	  DYN$K_SYNC | DYN$K_NOOWN | DYN$K_FLOOD | DYN$K_ALIGN, ei_rcv_mod,
	  ei_rcv_rem);

    } else {

	xppb->rbd_base = 
	  (struct xp_rbd *) malloc(sizeof(struct xp_rbd) * ei_rcv_buf_no);

    }

/* Initialize the TX timers */

    xppb->tx_isr = 
      (struct TIMERQ *) malloc(sizeof(struct TIMERQ) * ei_xmt_buf_no);
    for (i = 0; i < ei_xmt_buf_no; i++) {
	sprintf(name, "txisr_%08x", i);
	krn$_seminit(&(xppb->tx_isr[i].sem), 0, name);
	krn$_init_timer(&(xppb->tx_isr[i]));
    }

/* init the tx mutex semaphore */
    sprintf(name, "txwait_%4.4s", ip->name);
    krn$_seminit(&(xppb->tx_wait), 1, name);

/*Initialize the port spinlock*/

    xppb->spl_port.value = 0;
    xppb->spl_port.req_ipl = IPL_SYNC;
    xppb->spl_port.sav_ipl = 0;
    xppb->spl_port.attempts = 0;
    xppb->spl_port.retries = 0;
    xppb->spl_port.owner = 0;
    xppb->spl_port.count = 0;

/*Put the port block (inode) on the port block list*/

    pbe = (struct PBQ *) malloc(sizeof(struct PBQ));
    pbe->pb = (int *) ip;
#if ( STARTSHUT || DRIVERSHUT )
    pbe->pb = (int *) xppb;
#endif
    insq_lock(pbe, xppblist.blink);

/*Initialize the msg received list, added for callback support */

    spinlock(&spl_kernel);
    xppb->rcv_msg_cnt = 0;
    xppb->rqh.flink = &(xppb->rqh);
    xppb->rqh.blink = &(xppb->rqh);
    spinunlock(&spl_kernel);

/*Initialize the mop counters*/

    init_mop_counters(xppb->mc);

/*Init Local routine pointer*/

    xppb->lrp = 0;

/*Initialize some flags and counts*/

    xppb->state = XP_K_UNINITIALIZED;

/*Get the user's desired mode from the EV */

    ei_read_mode(xppb);

/*Check for polled or not*/

    if (xppb->pb.vector == 0) {

/*Set up the poll queue*/

#if ( STARTSHUT || DRIVERSHUT )
	err_printf("No vector found port %s is running polled!\n",
	  xppb->pb.name);
#endif
	xppb->pqp.flink = &xppb->pqp.flink;
	xppb->pqp.blink = &xppb->pqp.flink;
	xppb->pqp.routine = ei_poll;
	xppb->pqp.param = np;
	insq_lock(&xppb->pqp.flink, &pollq);
    } else {
#if DEBUG
	dprintf("Interrupt vector: %x.\n", p_args1(xppb->pb.vector));
#endif
	int_vector_set(xppb->pb.vector, ei_interrupt, np);
	io_enable_interrupts(xppb, xppb->pb.vector);
    }

#if 0
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

#endif

/* Initialize the MOP environment variables */

    mop_set_env(ip->name);
    ev->driver_flags = ei_driver_flags;
    ev->msg_buf_size = ei_msg_buf_size;
    ev->msg_mod = ei_msg_mod;
    ev->msg_rem = ei_msg_rem;
    ev->msg_prefix_size = 0;
    ev->mode = -1;			/* mode isn't kept here, used by nettest */

/* initialize some counters */
    xppb->ru_restarts = 0;
    xppb->cu_restarts = 0;
    xppb->cb_highwater = 0;
    xppb->cu_timeouts = 0;

#if !( STARTSHUT || DRIVERSHUT )

/*Initialize receive process*/

    sprintf(name, "rx_isr_%4.4s", ip->name);
    krn$_seminit(&(xppb->rx_isr), 0, name);
    sprintf(name, "rx_%4.4s", ip->name);
    krn$_create(ei_rx, null_procedure, 0, 5, -1, 1024 * 4, name, "tt", "r",
      "tt", "w", "tt", "w", ip);
#endif
}

/*+
 * ============================================================================
 * = ei_hardware_init - Initialization routine for the i82558 hardware        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routines will initialize the i82558 (EtherExpress Pro/100+) hardware.
 *
 * FORM OF CALL:
 *
 *	ei_hardware_init (struct INODE *ip); 
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

int ei_hardware_init(struct INODE *ip)
{
    struct NI_GBL *np;			/* Pointer to the NI global database */
    struct I82558PB *xppb;		/* Pointer to the port block */
    unsigned char ee_rom[XP_EEPROM_SIZE];	/* Copy of the eeprom */
    struct eeprom_fmt *eep = ee_rom;
    int i, status, status_message;

#if DEBUG
    char *subsys;

#endif

    dprintf("ei_hardware_init\n", p_args0);

/*Get some pointers*/

    np = (struct NI_GBL *) ip->misc;
    xppb = (struct I82558PB *) np->pbp;

/* read the eerom */

    status = ei_get_nirom(xppb, ee_rom);
    if (status != msg_success)
	return status;

/*Set up the Station address*/

    for (i = 0; i < 6; i++)
	xppb->sa[i] = eep->ia[i];

/* stash some of the info that we need in the PB */

    xppb->connectors = eep->connectors;	/* the connector list */
    xppb->phy_adr[0] = eep->phy1_adr;	/* the first PHY's address */
    xppb->phy_adr[1] = eep->phy2_adr;	/* the second PHY's address */
    xppb->phy_dev[0] = eep->phy1_dev;	/* the first PHY's type & flag */
    xppb->phy_dev[1] = eep->phy2_dev;	/* the second PHY's type & flag */

    /* Since even the Intel driver doesn't trust the eeprom... */

    /* If either of the phy_dev's are empty, scan all of the mii */

    /* addresses looking for a phy. Search 1 first, then 0-31. */

    /* Skip mii addresses that are already known. */

    if ((xppb->phy_dev[0] == 0) || (xppb->phy_dev[1] == 0)) {
	for (i = 0; i <= XP_PHY_MAX_ADDR; i++) {
	    int mii_addr;
	    unsigned int mdi_control, mdi_status;

	    if (i == 0)			/* swap address 0 & 1 */
		mii_addr = 1;
	    else if (i == 1)
		mii_addr = 0;
	    else
		mii_addr = i;

	    if (xppb->phy_dev[0] && (xppb->phy_adr[0] == mii_addr))
		continue;		/* already used */

	    mdi_control = ei_mdi_read(xppb, mii_addr, XP_PHY_CR) & 0xffff;
	    mdi_status = ei_mdi_read(xppb, mii_addr, XP_PHY_SR);
	    mdi_status = ei_mdi_read(xppb, mii_addr, XP_PHY_SR);

		/* check for a valid phy at this address */

	    if (!((mdi_control == 0xffff) || 
	      ((mdi_status == 0) && (mdi_control == 0)))) {
		dprintf("Found a valid PHY at address: %x\n", p_args1(mii_addr));

		if (xppb->phy_dev[0] == 0) {	/* set up primary phy */
		    xppb->phy_dev[0] = 0xff;
		    xppb->phy_adr[0] = mii_addr;
		} else {		/* set up second phy */
		    xppb->phy_dev[1] = 0xff;
		    xppb->phy_adr[1] = mii_addr;
		    break;		/* filled in both entries */
		}
	    }
	}
    }

/*Make sure the device is stopped*/

    status = ei_init_device(xppb);
    if (status != msg_success)
	return status;

/*Set the device to the correct media port*/

    ei_init_media(xppb);

#if DEBUG

    dprintf(EI_ADDR_FMT_STR,
      p_args6(xppb->sa[0], xppb->sa[1], xppb->sa[2],
      xppb->sa[3], xppb->sa[4], xppb->sa[5]));

    dprintf("\nConnectors: %02x\n", xppb->connectors);
    dprintf("PHY0: %02x, addr: %02x\n",
      p_args2(xppb->phy_dev[0], xppb->phy_adr[0]));
    dprintf("PHY1: %02x, addr: %02x\n",
      p_args2(xppb->phy_dev[1], xppb->phy_adr[1]));
    dprintf("OEM flag: %02x\n", eep->compat1);
    dprintf("Subsystem ID: %04x\n", eep->subsys_id);
    dprintf("Subsystem Vendor: %04x\n", eep->vendor_id);

    /* decode compaq subsys IDs */

    switch (eep->subsys_id) {
	case XP_SUBSYS_ID_NC3121: 
	    subsys = "Curveball";
	    break;
	case XP_SUBSYS_ID_NC3123:
	    subsys = "Hascal";
	    break;
	case XP_SUBSYS_ID_NC3131: 
	    subsys = "Tornado";
	    break;
	case XP_SUBSYS_ID_NC3132: 
	    subsys = "Thunder";
	    break;
	case XP_SUBSYS_ID_NC3133: 
	    subsys = "El Nino";
	    break;
	default: 
	    subsys = NULL;
    }

    if (subsys != NULL)
	dprintf("Adapter type: %s\n", subsys);
#endif

/*The hardware is now initialized*/

    xppb->state = XP_K_HARDWARE_INIT;

/*Initialize the tx descriptors*/

    ei_init_tx(xppb);

/*Initialize the rx descriptors*/

    ei_init_rx(ip, xppb);

/*Send the configure command */

    ei_configure_cmd(ip, xppb);

/* Start the receive process */

    ei_exec_cmd(xppb, XP_SCB_CMD_RU_START, xppb->rfd_base);

/* Start the transmit process */

    ei_exec_cmd(xppb, XP_SCB_CMD_CU_START, xppb->cbl_base);

    return (msg_success);
}

/*+
 * ============================================================================
 * = ei_init_device - Initialize the i82558.                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will initialize the i82558 device.
 *
 * FORM OF CALL:                
 *
 *	ei_init_device (xppb);    
 *
 * RETURNS:
 *
 *	int Status - msg_success or FAILURE.
 *
 * ARGUMENTS:
 *
 *	struct I82558PB *xppb - Pointer to the xp port block.
 *
 * SIDE EFFECTS:          
 *
 *      None                    
 *
-*/

int ei_init_device(struct I82558PB *xppb)
{
    unsigned long rev;
    unsigned long scratch;
    unsigned int str[6];
    unsigned int *selftest_ptr;
    unsigned int selftest_limit = 1000;

    dprintf("ei_init_device\n", p_args0);

    scratch = incfgl(xppb, 0x0);
    dprintf("i82558 device vendor id: %08x\n", p_args1(scratch));

    scratch = incfgl(xppb, 0x10);
    dprintf("i82558 memory base address: %08x\n", p_args1(scratch));

    scratch = incfgl(xppb, 0x14);
    dprintf("i82558 I/O base address: %08x\n", p_args1(scratch));

    scratch = incfgl(xppb, 0x4);
    dprintf("i82558 PCI command/status reg: %08x\n", p_args1(scratch));

    /* Reset the chip with a selective reset */

    XP_CSR_WRITE_LONG(xppb, XP_PORT, XP_PORT_SEL_RST);
    krn$_sleep(15);

    /* Then a software reset */

    XP_CSR_WRITE_LONG(xppb, XP_PORT, XP_PORT_SOFTW_RST);
    krn$_sleep(15);

    /* Now, a selftest */

    /* the results pointer has to be aligned on a 16 byte boundary */

    selftest_ptr = ((((unsigned int) str) + 15) & ~0xf);
    selftest_ptr[0] = 0;
    selftest_ptr[1] = -1;
    XP_CSR_WRITE_LONG(xppb, XP_PORT,
      XP_PORT_SELFTEST | (int) selftest_ptr | WINDOW_BASE(xppb));

    do {
	krn$_sleep(10);
    } while ((selftest_ptr[1] == -1) && (selftest_limit--));

    dprintf("i82558 Selftest Results: %08x, %08x\n",
      p_args2(selftest_ptr[0], selftest_ptr[1]));

    if ((selftest_ptr[0] == 0) || (selftest_ptr[1] != 0)) {
	ni_error(xppb->pb.name, "i82558 Selftest Failed");
	return msg_failure;
    }

/*Write the interrupt mask register*/
/*Enable all the interrupts, unless running polled*/

    if (xppb->pb.vector) {
	XP_CSR_WRITE_BYTE(xppb, XP_SCB_CMD + 1, 0);
    } else {
	XP_CSR_WRITE_BYTE(xppb, XP_SCB_CMD + 1,
	  XP_SCB_CMD_I_MASK);
    }

    /* init the CU & RU base registers */

    /* *most* data structure addresses are offset by this value */

    ei_exec_cmd(xppb, XP_SCB_CMD_CU_BASE, WINDOW_BASE(xppb));
    ei_exec_cmd(xppb, XP_SCB_CMD_RU_BASE, WINDOW_BASE(xppb));

    return (msg_success);
}

/*+                      
 * ============================================================================
 * = ei_get_nirom - Retreive the onboard eeprom data. 		      	      =
 * ============================================================================
 *               
 * OVERVIEW:                                                                  
 *  
 *	Load the contents of the onboard eeprom via the chip CSRs.
 *           
 * FORM OF CALL:                                                 
 *  
 *  	ei_get_nirom (xppb,ni);
 *              
 * RETURNS:
 *
 *	int Status - msg_success or FAILURE.
 *                    
 * ARGUMENTS:                           
 *
 *      struct I82558PB *xppb - Pointer to the port block.
 *      unsigned char *ni - Pointer to a buffer to filled.
 *                          
-*/
int ei_get_nirom(struct I82558PB *xppb, unsigned char *ni)
{
    unsigned short sum;
    unsigned short data;
    unsigned short *dp = (unsigned short *) ni;
    int i;

    ei_get_eeprom_size(xppb);	/* init rom size */
    sum = 0;
    for (i = 0; i < (1 << (xppb->rom_bits)); i++) {
	data = ei_read_eeprom(xppb, i);
	sum += data;
	dprintf("  eeprom word: %04x\n", p_args1(data));
	if (i < (XP_EEPROM_SIZE / 2))	/* only store this much */
	    *dp++ = data;
    }

    if (sum != 0xbaba) {		/* checksum must be 0xbaba */
	ni_error(xppb->pb.name, "Warning, Bad Checksum on eeprom");
	dprintf("  checksum: %04x\n", p_args1(sum));
	return(msg_failure);
    }
    return msg_success;
}


/*+
 * ============================================================================
 * = ei_read_eeprom - Read a word from the serial eeprom.		      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This function goes through the operations required to read a
 *	16 bit word from the serial EEPROM.
 *
 * FORM OF CALL:
 *
 *	ei_read_eeprom (xppb, offset)
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *      struct I82558PB *xppb - Pointer to the port block.
 *	int offset - word # to read
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
unsigned short ei_read_eeprom(struct I82558PB *xppb, int offset)
{
    unsigned short reg, data;
    int i, x;

    /* chip select */

    XP_CSR_WRITE_WORD(xppb, XP_EEPROM_CTL, XP_EEPROM_EECS);

    /* shift in read opcode */

    for (x = 3; x > 0; x--) {
	if (XP_EEPROM_OPC_READ & (1 << (x - 1))) {
	    reg = XP_EEPROM_EECS | XP_EEPROM_EEDI;
	} else {
	    reg = XP_EEPROM_EECS;
	}
	XP_CSR_WRITE_WORD(xppb, XP_EEPROM_CTL, reg);
	XP_CSR_WRITE_WORD(xppb, XP_EEPROM_CTL,
	  reg | XP_EEPROM_EESK);
	krn$_micro_delay(1);
	XP_CSR_WRITE_WORD(xppb, XP_EEPROM_CTL, reg);
	krn$_micro_delay(1);
    }

    /* shift in address */

    for (x = xppb->rom_bits; x > 0; x--) {
	if (offset & (1 << (x - 1))) {
	    reg = XP_EEPROM_EECS | XP_EEPROM_EEDI;
	} else {
	    reg = XP_EEPROM_EECS;
	}
	XP_CSR_WRITE_WORD(xppb, XP_EEPROM_CTL, reg);
	XP_CSR_WRITE_WORD(xppb, XP_EEPROM_CTL,
	  reg | XP_EEPROM_EESK);
	krn$_micro_delay(1);
	XP_CSR_WRITE_WORD(xppb, XP_EEPROM_CTL, reg);
	krn$_micro_delay(1);
    }

    reg = XP_EEPROM_EECS;
    data = 0;

    /* shift out data */

    for (x = 16; x > 0; x--) {
	XP_CSR_WRITE_WORD(xppb, XP_EEPROM_CTL,
	  reg | XP_EEPROM_EESK);
	krn$_micro_delay(1);
	if (XP_CSR_READ_WORD(xppb, XP_EEPROM_CTL) & XP_EEPROM_EEDO) {
	    data |= (1 << (x - 1));
	}
	XP_CSR_WRITE_WORD(xppb, XP_EEPROM_CTL, reg);
	krn$_micro_delay(1);
    }
    XP_CSR_WRITE_WORD(xppb, XP_EEPROM_CTL, 0);
    krn$_micro_delay(1);

    return (data);
}

/*+
 * ============================================================================
 * = ei_get_eeprom_size - Determine the size of the serial eeprom.	      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This function communicates with the serial eeprom to determine
 *	the number of bits to send in the read address.
 *
 * FORM OF CALL:
 *
 *	ei_get_eeprom_size (xppb)
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *      struct I82558PB *xppb - Pointer to the port block.
 *
 * SIDE EFFECTS:
 *
 *      xppb->rom_bits is updated with the address field size
 *
-*/
int ei_get_eeprom_size(struct I82558PB *xppb)
{
    unsigned short reg, data;
    int i, x;
    int bit_count;

    /* chip select */

    XP_CSR_WRITE_WORD(xppb, XP_EEPROM_CTL, XP_EEPROM_EECS);

    /* shift in read opcode */

    for (x = 3; x > 0; x--) {
	if (XP_EEPROM_OPC_READ & (1 << (x - 1))) {
	    reg = XP_EEPROM_EECS | XP_EEPROM_EEDI;
	} else {
	    reg = XP_EEPROM_EECS;
	}
	XP_CSR_WRITE_WORD(xppb, XP_EEPROM_CTL, reg);
	XP_CSR_WRITE_WORD(xppb, XP_EEPROM_CTL,
	  reg | XP_EEPROM_EESK);
	krn$_micro_delay(1);
	XP_CSR_WRITE_WORD(xppb, XP_EEPROM_CTL, reg);
	krn$_micro_delay(1);
    }

    /* shift in address */

    for (bit_count = 1; ; bit_count++) {
	reg = XP_EEPROM_EECS;
	XP_CSR_WRITE_WORD(xppb, XP_EEPROM_CTL, reg);
	XP_CSR_WRITE_WORD(xppb, XP_EEPROM_CTL,
	  reg | XP_EEPROM_EESK);
	krn$_micro_delay(1);
	XP_CSR_WRITE_WORD(xppb, XP_EEPROM_CTL, reg);
	krn$_micro_delay(1);
	if ((XP_CSR_READ_WORD(xppb, XP_EEPROM_CTL) & XP_EEPROM_EEDO) == 0)
	    break;  /* found the dummy zero bit */
    }

    dprintf("EEprom address is %d bits.\n", p_args1(bit_count));

    reg = XP_EEPROM_EECS;
    data = 0;

    /* shift out data */

    for (x = 16; x > 0; x--) {
	XP_CSR_WRITE_WORD(xppb, XP_EEPROM_CTL,
	  reg | XP_EEPROM_EESK);
	krn$_micro_delay(1);
	if (XP_CSR_READ_WORD(xppb, XP_EEPROM_CTL) & XP_EEPROM_EEDO) {
	    data |= (1 << (x - 1));
	}
	XP_CSR_WRITE_WORD(xppb, XP_EEPROM_CTL, reg);
	krn$_micro_delay(1);
    }
    XP_CSR_WRITE_WORD(xppb, XP_EEPROM_CTL, 0);
    krn$_micro_delay(1);

    xppb->rom_bits = bit_count;
    return(bit_count);
}

/*+
 * ============================================================================
 * = ei_ev_write - Action routine for the environment variables.              =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This function is the action routine used by the ei
 *	environment variables. 
 *
 * FORM OF CALL:
 *
 *	ei_ev_write (name,ev)
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

void ei_ev_write(char *name, struct EVNODE *ev)
{
    struct FILE *fp;			/*File pointer*/
    struct NI_GBL *np;			/*Pointer to the NI global database*/
    struct I82558PB *xppb;		/*Pointer to the port block*/
    char fname[8];			/*Name of the file*/
    char c;
    int status;

/*Get the port block*/

    strncpy(fname, name, 4);
    fname[4] = 0;
    fp = fopen(fname, "rs");
    if (!fp)
	return;
    np = (struct NI_GBL *) fp->ip->misc;
    xppb = (struct I82558PB *) np->pbp;

/*Read the new mode*/

    c = ev->value.string[2];
    if (strncmp_nocase(ev->value.string, ei_auto_negotiate, 8) == 0)
	xppb->mode = XP_K_AUTO_NEGOTIATE;
    else if (c == 'i')
	xppb->mode = XP_K_TWISTED;
    else if (c == 'l')
	xppb->mode = XP_K_TWISTED_FD;
    else if (c == 'C')
	xppb->mode = XP_K_BNC;
    else if (c == 'I')
	xppb->mode = XP_K_AUI;
    else if (strncmp_nocase(ev->value.string, ei_fastfd, 6) == 0)
	xppb->mode = XP_K_FASTFD;
    else if (strncmp_nocase(ev->value.string, ei_fast, 4) == 0)
	xppb->mode = XP_K_FAST;
    else
	xppb->mode = XP_K_TWISTED;

/* Change the mode */

    err_printf("Changing to selected mode.\n");
    status = ei_init_media(xppb);
    if (status != msg_success) {
	err_printf("The selected mode is illegal or incompatible.\n");
    } else {
#if 0
	if (tupb->pb.type == TYPE_PCI)
	    outcfgl(tupb, TU_PCFDA_ADDR, ew_media);
#endif

/*Put the new value in nvram*/

	ev_sev(name, ev);
    }

/*Close the file*/

    fclose(fp);
}

/*+
 * ============================================================================
 * = ei_read_mode - read the i82558 mode (Twisted Pair or AUI).                =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This function will read the i82558 mode environment variable
 *	and convert it to the correct integer.
 *
 * FORM OF CALL:
 *
 *	ei_read_mode (xppb)
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	struct I82558PB *xppb - Pointer to the xp port block.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void ei_read_mode(struct I82558PB *xppb)
{
    struct EVNODE *evp, evn;		/*Pointer to the evnode,evnode*/
    char var[EV_NAME_LEN];		/*General holder for a name*/
    char c;

/*Read the environment variables*/

    evp = &evn;
    sprintf(var, EI_MODE_FMT_STR, xppb->pb.name);
    if (ev_read(var, &evp, 0) != msg_success) {
	xppb->mode = XP_K_TWISTED;
	return;
    }

/*Now check the value*/

    c = evp->value.string[2];
    if (strncmp_nocase(evp->value.string, ei_auto_negotiate, 8) == 0)
	xppb->mode = XP_K_AUTO_NEGOTIATE;
    else if (c == 'i')
	xppb->mode = XP_K_TWISTED;
    else if (c == 'l')
	xppb->mode = XP_K_TWISTED_FD;
    else if (c == 'C')
	xppb->mode = XP_K_BNC;
    else if (c == 'I')
	xppb->mode = XP_K_AUI;
    else if (strncmp_nocase(evp->value.string, ei_fastfd, 6) == 0)
	xppb->mode = XP_K_FASTFD;
    else if (strncmp_nocase(evp->value.string, ei_fast, 4) == 0)
	xppb->mode = XP_K_FAST;
    else
	xppb->mode = XP_K_TWISTED;
}

/*+
 * ============================================================================
 * = ei_init_media - Set the i82558 to use the correct media port.            =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will set the network connection type and speed.
 *	Depending on what is available for a PHY and what the user has
 *	set the eiax_mode environment variable to, select a connection.
 *
 * FORM OF CALL:                
 *
 *	ei_init_media (xppb);    
 *
 * RETURNS:
 *
 *	int Status - msg_success or FAILURE.
 *
 * ARGUMENTS:
 *
 *	struct I82558PB *xppb - Pointer to the xp port block.
 *
 * SIDE EFFECTS:          
 *
 *      None                    
 *
-*/

int ei_init_media(struct I82558PB *xppb)
{
    unsigned short capabilities[2];
    int i;
    int phy_num = -1;
    int phy_count = 0;

    dprintf("ei_init_media\n", p_args0);
    dprintf("xppb-mode = %d\n", p_args1(xppb->mode));

    /* get PHY capabilities (0 == not present) */

    for (i = 0; i < EI_MAX_PHY; i++) {
	if (xppb->phy_dev[i]) {
	    capabilities[i] = ei_mdi_read(xppb, xppb->phy_adr[i], XP_PHY_SR);
	    dprintf("PHY %d capabilities: %04x\n", p_args2(i, capabilities[i]));
	    phy_count++;
	} else {
	    capabilities[i] = 0;
	}
    }

    /* now, see how these stack up against what the user wants */

    for (i = 0; i < EI_MAX_PHY; i++) {
	switch (xppb->mode) {
	    case XP_K_TWISTED: 
		if (capabilities[i] & XP_PHY_SR_10HD)
		    phy_num = i;
		break;

	    case XP_K_AUI: 
	    case XP_K_BNC: 
		break;			/* how do we match up this connector to a PHY ? */

	    case XP_K_TWISTED_FD: 
		if (capabilities[i] & XP_PHY_SR_10FD)
		    phy_num = i;
		break;

	    case XP_K_FAST: 
		if (capabilities[i] & XP_PHY_SR_TXHD)
		    phy_num = i;
		break;

	    case XP_K_FASTFD: 
		if (capabilities[i] & XP_PHY_SR_TXFD)
		    phy_num = i;
		break;

	    case XP_K_AUTO: 		/* see if we have a link or this is the last PHY */
		if (capabilities[i] & XP_PHY_SR_AUTO) {
		    phy_num = i;
		}
		break;

	    case XP_K_AUTO_NEGOTIATE: 
		if (capabilities[i] & XP_PHY_SR_AUTO)
		    phy_num = i;
		break;

	    default: 
		break;
	}
	if ((phy_num && (capabilities[i] & XP_PHY_SR_LINK)) || 
	  (i == phy_count - 1)) {
	    phy_num = i;
	    break;			/* a PHY was selected */
	}
    }

    /* if we matched a PHY, then set it to the desired mode */

    /* otherwise, do an auto select on the first available */

    if (phy_num != -1) {
	dprintf("Selecting PHY %d\n", phy_num);
	xppb->selected_phy = phy_num;
	return (ei_setup_phy(xppb, phy_num));
    } else {
	dprintf("Can't locate a PHY compatible with mode %d\n", xppb->mode);
	xppb->mode = XP_K_TWISTED;
	return (msg_failure);
    }
}

/*+
 * ============================================================================
 * = ei_mdi_read - Read a word from an MDI register.                          =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will return the data from the MII device at the
 *	address and offset specified using the 82558 MDI control register.
 *
 * FORM OF CALL:                
 *
 *	ei_mdi_read (xppb, addr, offset);    
 *
 * RETURNS:
 *
 *	unsigned short data - data received
 *
 * ARGUMENTS:
 *
 *	struct I82558PB *xppb - Pointer to the xp port block.
 *
 *	unsigned int addr - PHY address.
 *
 *	unsigned int offset - MDI register offset.
 *
 * SIDE EFFECTS:          
 *
 *      None                    
 *
-*/

int ei_mdi_read(struct I82558PB *xppb, unsigned int addr, unsigned int offset)
{
    unsigned int mdi_reg;
    int phy_retry = 100;
    int i;

    /* set up a read command with the PHY address and register num */

    mdi_reg = XP_MDI_OP_READ | 
      ((addr << 21) & XP_MDI_PHY_ADR_MASK) | 
      ((offset << 16) & XP_MDI_PHY_REG_MASK);

    /* send it to the PHY via the 82558's MDI control reg */

    XP_CSR_WRITE_LONG(xppb, XP_MDI_CTL, mdi_reg);

    /* wait for "ready" */

    for (i = 0; i < phy_retry; i++) {
	if ((mdi_reg = XP_CSR_READ_LONG(xppb, XP_MDI_CTL)) & XP_MDI_READY_MASK)
	    break;
	krn$_sleep(20);
    }

    if (i >= phy_retry) {
	dprintf("Timeout reading MDI addr %02x, offset %02x\n",
	  p_args2(addr, offset));
    }

    return (mdi_reg & 0xffff);
}

/*+
 * ============================================================================
 * = ei_mdi_write - Write a word to an MDI register.                          =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will write data to the MII device at the
 *	address and offset specified using the 82558 MDI control register.
 *
 * FORM OF CALL:                
 *
 *	ei_mdi_write (xppb, addr, offset, data);
 *
 * RETURNS:
 *
 *	int Status - msg_success or FAILURE
 *
 * ARGUMENTS:
 *
 *	struct I82558PB *xppb - Pointer to the xp port block.
 *
 *	unsigned int addr - PHY address.
 *
 *	unsigned int offset - MDI register offset.
 *
 *	unsigned short data - data to write
 *
 * SIDE EFFECTS:          
 *
 *      None                    
 *
-*/

int ei_mdi_write(struct I82558PB *xppb, unsigned int addr,
  unsigned int offset, unsigned short data)
{
    unsigned int mdi_reg;
    int phy_retry = 100;
    int i;

    /* set up a write command with the PHY address and register num */

    mdi_reg = XP_MDI_OP_WRITE | 
      ((addr << 21) & XP_MDI_PHY_ADR_MASK) | 
      ((offset << 16) & XP_MDI_PHY_REG_MASK) | 
      data;

    /* send it to the PHY via the 82558's MDI control reg */

    XP_CSR_WRITE_LONG(xppb, XP_MDI_CTL, mdi_reg);

    /* wait for "ready" */

    for (i = 0; i < phy_retry; i++) {
	if ((mdi_reg = XP_CSR_READ_LONG(xppb, XP_MDI_CTL)) & XP_MDI_READY_MASK)
	    break;
	krn$_sleep(20);
    }

    if (i >= phy_retry) {
	dprintf("Timeout writing MDI addr %02x, offset %02x\n",
	  p_args2(addr, offset));
	return (0);
    }

    return (msg_success);
}

/*+
 * ============================================================================
 * = ei_setup_phy - Configure a PHY on the MII port.                          =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will set up one PHY according to the current mode
 *	of the driver. If the speed and duplex are specified, they are used,
 *	else an auto negotiation process occurs.
 *
 * FORM OF CALL:                
 *
 *	ei_setup_phy (xppb, phy_num);
 *
 * RETURNS:
 *
 *	int Status - msg_success or FAILURE.
 *
 * ARGUMENTS:
 *
 *	struct I82558PB *xppb - Pointer to the xp port block.
 *
 *	int phy_num - index as to which PHY (0 or 1) in the port block
 *		      is being configured.
 *
 * SIDE EFFECTS:          
 *
 *      Any other PHY device will be disabled.
 *
-*/

int ei_setup_phy(struct I82558PB *xppb, int phy_num)
{
    int other_phy;
    unsigned int phy_address, phy_device;
    unsigned short phy_data;
    unsigned int phy_id;
    int retry_count;

    /* get the selected PHY's address */

    phy_address = xppb->phy_adr[phy_num];

    /* disable any other PHY on this adapter */

    for (other_phy = 0; other_phy < EI_MAX_PHY; other_phy++) {
	if (other_phy == phy_num)
	    continue;

	if (xppb->phy_dev[other_phy]) {	/* other PHY valid ? */
	    phy_data = ei_mdi_read(xppb, xppb->phy_adr[other_phy], XP_PHY_CR);
	    phy_data |= XP_PHY_CR_ISOLATE;	/* take him off the MII bus */
	    ei_mdi_write(xppb, xppb->phy_adr[other_phy], XP_PHY_CR, phy_data);
	}
    }

    phy_data = ei_mdi_read(xppb, phy_address, XP_PHY_CR);
    phy_data |= XP_PHY_CR_ISOLATE;	/* put him on the bus */
    ei_mdi_write(xppb, phy_address, XP_PHY_CR, phy_data);

    phy_id = ei_mdi_read(xppb, phy_address, XP_PHY_IR1);
    phy_id |= (ei_mdi_read(xppb, phy_address, XP_PHY_IR2) << 16);
    dprintf("Phy ID Reg: %08x\n", p_args1(phy_id));

    /* Handle the LED initialization for the 100Base-FX PHY */

    if ((phy_id & XP_PHY_MODEL_REVID_MASK) == XP_LXT970_PHY) {
	unsigned int lxt970_cr;

	dprintf("Enabling lxt970 activity led\n", p_args0);
	lxt970_cr = ei_mdi_read(xppb, phy_address, XP_LXT970_CR);
	lxt970_cr &= ~XP_LXT970_CR_LEDCMASK;
	lxt970_cr |= XP_LXT970_CR_LEDCACTIV;	/* led = activity */
	ei_mdi_write(xppb, phy_address, XP_LXT970_CR, lxt970_cr);
    }

    /* perform mode-specific initialization on this PHY */

    switch (xppb->mode) {
	case XP_K_TWISTED: 
	    phy_data &= ~(XP_PHY_CR_SPEED_100M | 	/* 10 mbit */
	      XP_PHY_CR_AUTOEN | 	/* no autoselect */
	      XP_PHY_CR_ISOLATE | 	/* enabled */
	      XP_PHY_CR_FULLDUP);	/* half duplex */
	    ei_mdi_write(xppb, phy_address, XP_PHY_CR, phy_data);
	    break;

	case XP_K_TWISTED_FD: 
	    phy_data &= ~(XP_PHY_CR_SPEED_100M | 	/* 10 mbit */
	      XP_PHY_CR_AUTOEN | 	/* no autoselect */
	      XP_PHY_CR_ISOLATE);	/* enabled */

	    phy_data |= XP_PHY_CR_FULLDUP;	/* full duplex */
	    ei_mdi_write(xppb, phy_address, XP_PHY_CR, phy_data);
	    break;

	case XP_K_FAST: 
	    phy_data &= ~(XP_PHY_CR_AUTOEN | 	/* no autoselect */
	      XP_PHY_CR_ISOLATE | 	/* enabled */
	      XP_PHY_CR_FULLDUP);	/* half duplex */
	    phy_data |= XP_PHY_CR_SPEED_100M;	/* 100 mbit */
	    ei_mdi_write(xppb, phy_address, XP_PHY_CR, phy_data);
	    break;

	case XP_K_FASTFD: 
	    phy_data &= ~(XP_PHY_CR_AUTOEN | 	/* no autoselect */
	      XP_PHY_CR_ISOLATE);	/* enabled */
	    phy_data |= (XP_PHY_CR_SPEED_100M | 	/* 100 mbit */
	      XP_PHY_CR_FULLDUP);	/* full duplex */
	    ei_mdi_write(xppb, phy_address, XP_PHY_CR, phy_data);
	    break;

	case XP_K_AUTO: 		/* restart auto-negotiation */
	case XP_K_AUTO_NEGOTIATE: 
	    phy_data |= (XP_PHY_CR_RESET | 
	      XP_PHY_CR_AUTOEN | 
	      XP_PHY_CR_RSTAUTO);
	    ei_mdi_write(xppb, phy_address, XP_PHY_CR, phy_data);
	    break;

	case XP_K_AUI: 
	case XP_K_BNC: 
	default: 
	    ni_error(xppb->pb.name, "Illegal interface mode value.");
	    return (msg_failure);
	    break;
    }

    /* if we used auto mode, check the result */

    if ((xppb->mode == XP_K_AUTO) || (xppb->mode == XP_K_AUTO_NEGOTIATE)) {
	char *speed, *duplex;
	static char fast_s[] = "100BaseTX", slow_s[] = "10BaseT",
	  full_s[] = "full", half_s[] = "half";

	for (retry_count = 35; retry_count > 0; retry_count--) {

	    /* check the link status (have to read it twice to clear old error)
	     */

	    phy_data = ei_mdi_read(xppb, phy_address, XP_PHY_SR);
	    phy_data = ei_mdi_read(xppb, phy_address, XP_PHY_SR);
	    if ((phy_data & XP_PHY_SR_LINK) || (phy_data & XP_PHY_SR_AUTOCPLT))
		break;
	    krn$_sleep(100);		/* wait 100 ms */
	}

	/* Read the status register again to clear out any old link errors */

	phy_data = ei_mdi_read(xppb, phy_address, XP_PHY_SR);
	if ((phy_data & XP_PHY_SR_LINK) == 0) {
	    ni_error(xppb->pb.name, "No link");
	}

	/* make sure auto negotiation completed */

	if ((phy_data & XP_PHY_SR_AUTOCPLT) == 0) {
	    ni_error(xppb->pb.name, "Auto Negotiation did not complete.");
	    return (msg_failure);
	}

	phy_data = ei_mdi_read(xppb, phy_address, XP_PHY_ER);
	dprintf("XP_PHY_ER: %04x\n", phy_data);

	/* get the speed and duplex information */

	phy_device = (xppb->phy_dev[phy_num] & XP_EEPROM_PHY_DMASK);
	switch (phy_device) {
	    case XP_PHY_82555: 
	    case XP_PHY_82553A: 
	    case XP_PHY_82553C: 

		/* use the extended status and control reg. */

		phy_data = ei_mdi_read(xppb, phy_address, XP_PHY100_SCR);
		if (phy_data & XP_PHY100_SCR_FAST)
		    speed = fast_s;
		else
		    speed = slow_s;

		if (phy_data & XP_PHY100_SCR_FD)
		    duplex = full_s;
		else
		    duplex = half_s;
		break;

	    default: 			/* try to discern from the MDI regs */
		phy_data = ei_mdi_read(xppb, phy_address, XP_PHY_ER);

		/* was the partner capable of auto negotiation ? */

		if (phy_data & XP_PHY_ER_LPAUTO) {
		    int our_caps, other_caps;

		    /* yes, what did we agree on */

		    our_caps = ei_mdi_read(xppb, phy_address, XP_PHY_AR);
		    other_caps = ei_mdi_read(xppb, phy_address, XP_PHY_LP);

		    our_caps &= other_caps & XP_PHY_LP_TAMASK;

		    if (our_caps & (XP_PHY_AR_100BASET4 | 
		      XP_PHY_AR_FAST | 
		      XP_PHY_AR_FASTFD)) {
			speed = fast_s;
		    } else {
			speed = slow_s;
		    }

		    if (our_caps & (XP_PHY_AR_FASTFD | 
		      XP_PHY_AR_FD)) {
			duplex = full_s;
		    } else {
			duplex = half_s;
		    }

		} else {
		    return (msg_success);	/* we have no idea */
		}

	}
	dprintf("Auto-negotiated: %s Mbps, %s duplex\n",
	  p_args2(speed, duplex));

	printf("ei%c0: link %s %s: %s duplex\n", xppb->pb.controller + 'a',
	  "up : Negotiated ",
	  speed,
	  duplex);

    }

    return (msg_success);
}

/*+
 * ============================================================================
 * = ei_init_tx - Initialize the transmit descriptor data structures.         =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will initialize all of the command block list
 *	structures used for transmit commands.
 *
 * FORM OF CALL:                
 *
 *	ei_init_tx (xppb);    
 *
 * RETURNS:
 *
 *	int Status - msg_success or FAILURE.
 *
 * ARGUMENTS:
 *
 *	struct I82558PB *xppb - Pointer to the xp port block.
 *
 * SIDE EFFECTS:          
 *
 *      None                    
 *
-*/

void ei_init_tx(struct I82558PB *xppb)
{
    struct xp_txcb *txcb_p;
    int i;

    txcb_p = xppb->cbl_base;

    for (i = 0; i < ei_xmt_buf_no; i++) {
	txcb_p[i].cb_status = 0;
	txcb_p[i].cb_command = XP_CB_CMD_EL;	/* mark all as end o list */
	txcb_p[i].index = i;		/* used to index the timers */

	if (i == (ei_xmt_buf_no - 1))
	    txcb_p[i].cb_link = txcb_p;	/* make list circular */
	else
	    txcb_p[i].cb_link = &(txcb_p[i + 1]);	/* link to next */
    }
    xppb->cbl_first = xppb->cbl_last = xppb->cbl_base;

    /* Init the first command block so that when the tx process is */

    /* started, a NOP will be executed and the CU will be in the */

    /* suspended state. Then, additional transmits will be able */

    /* to be added in a consistent manner. */

    xppb->cbl_first->cb_command = XP_CB_CMD_NOP | XP_CB_CMD_S;
    xppb->cbl_first->cb_status = 0;
    xppb->cb_active = 1;

    dprintf("Command Block base: %08x\n", xppb->cbl_base);
}

/*+
 * ============================================================================
 * = ei_init_rx - Initialize the receive descriptor data structures.          =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will initialize the Receive Frame Descriptors and
 *	Receive Buffer Descriptors allocated by ei_software_init.
 *
 * FORM OF CALL:                
 *
 *	ei_init_rx (ip, xppb);    
 *
 * RETURNS:
 *
 *	int Status - msg_success or FAILURE.
 *
 * ARGUMENTS:
 *
 *	struct INODE *ip - INODE pointer
 *	struct I82558PB *xppb - Pointer to the xp port block.
 *
 * SIDE EFFECTS:          
 *
 *      None                    
 *
-*/

void ei_init_rx(struct INODE *ip, struct I82558PB *xppb)
{
    struct xp_rfd *rfd_p;
    struct xp_rbd *rbd_p;
    int i;

    rfd_p = xppb->rfd_base;
    rbd_p = xppb->rbd_base;

    rfd_p[0].rfd_status = 0;
    rfd_p[0].rfd_control = XP_RFD_CONTROL_SF;	/* flexible mode */
    rfd_p[0].actual_size = 0;
    rfd_p[0].size = 0;
    rfd_p[0].rbd_ptr = rbd_p;		/* the first RFD points to the RBD list */
    rbd_p[0].actual_size = 0;
    if (!(rbd_p[0].rcv_buf_adr))
	rbd_p[0].rcv_buf_adr = ndl_allocate_pkt(ip, 1) + WINDOW_BASE(xppb);
    rbd_p[0].size = NDL$K_MAX_PACK_LENGTH;

    for (i = 1; i < ei_rcv_buf_no; i++) {

	/* initialize all fields in the RFD */

	/* and link the circular list */

	rfd_p[i].rfd_status = 0;
	rfd_p[i].rfd_control = XP_RFD_CONTROL_SF;	/* flexible mode */
	rfd_p[i - 1].link_addr = &(rfd_p[i]);	/* previous points to this one */
	rfd_p[i].rbd_ptr = I82558_NULL;
	rfd_p[i].actual_size = 0;
	rfd_p[i].size = 0;

	/* initialize the corresponding RBD, and link them */

	rbd_p[i].actual_size = 0;
	rbd_p[i - 1].next_rbd_adr = &(rbd_p[i]);
	if (!(rbd_p[i].rcv_buf_adr))
	    rbd_p[i].rcv_buf_adr = ndl_allocate_pkt(ip, 1) + WINDOW_BASE(xppb);
	rbd_p[i].size = NDL$K_MAX_PACK_LENGTH;
    }
    rfd_p[ei_rcv_buf_no - 1].link_addr = &(rfd_p[0]);	/* circular ring */
    rfd_p[ei_rcv_buf_no - 1].rfd_control |= XP_RFD_CONTROL_EL;	/* end o list */
    rbd_p[ei_rcv_buf_no - 1].next_rbd_adr = &(rbd_p[0]);
    rbd_p[ei_rcv_buf_no - 1].size |= XP_RBD_SIZE_EL;	/* end of list */
    xppb->rfd_first = xppb->rfd_base;	/* track start RFD */
    xppb->rbd_first = xppb->rbd_base;	/* track start RBD */
    xppb->rfd_last = &(rfd_p[ei_rcv_buf_no - 1]);	/* track end RFD */
    xppb->rbd_last = &(rbd_p[ei_rcv_buf_no - 1]);	/* track end RBD */

    dprintf("RFD list: %08x, %08x\n", p_args2(xppb->rfd_base, xppb->rfd_last));
    dprintf("RBD list: %08x, %08x\n", p_args2(xppb->rbd_base, xppb->rbd_last));
}

/*+
 * ============================================================================
 * = ei_read - ei port read routine.                                          =
 * ============================================================================
 *
 * OVERVIEW:         
 *  
 *      Used primarily for ei port callbacks.
 *  
 * FORM OF CALL:
 *  
 *	ei_read (fp,size,number,c);
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

int ei_read(struct FILE *fp, int size, int number, unsigned char *c)
{
    struct NI_GBL *np;			/*Pointer to NI global database*/
    struct I82558PB *xppb;		/*Pointer to the port block*/
    struct EI_MSG *mr;			/*Pointer to the message received*/
    int len, retries;
    int callback_timeout = 100;
    int callback_wait = 10;

    dprintf("ei_read: size: %d number: %d\n", p_args2(size, number));

/*Get the pointer to the port block*/

    np = (struct NI_GBL *) fp->ip->misc;
    xppb = (struct I82558PB *) np->pbp;

/*See if there is a message available*/

    if (cb_ref)
	callback_timeout = 2;

    for (retries = 0; retries < callback_timeout; retries++) {
	spinlock(&spl_kernel);
	if (xppb->rqh.flink != (struct EI_MSG *) &(xppb->rqh.flink)) {

/*Get the message*/

	    mr = (struct EI_MSG *) remq(xppb->rqh.flink);
	    xppb->rcv_msg_cnt--;
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
    if (xp_debug & 4)
	pprint_pkt(c, len);
#endif
    return (len);
}

/*+
 * ============================================================================
 * = ei_rx - Receive process.                                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine is actually a process that is created at init time.
 *	It processes receive frame descriptors. 
 *
 * FORM OF CALL:
 *
 *	ei_rx (ip); 
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

void ei_rx(struct INODE *ip)
{
    struct NI_GBL *np;			/*Pointer to the NI global database*/
    struct I82558PB *xppb;		/*Pointer to the port block*/
    struct FILE *fp;			/*Pointer to the file being used*/

/*Get a file pointer for this device*/
/*This file stays open forever*/

    fp = (struct FILE *) fopen(ip->name, "r+");
    if (!fp)
	return;

/*Get some pointers*/

    np = (struct NI_GBL *) fp->ip->misc;	/* NI global block */
    xppb = (struct I82558PB *) np->pbp;	/* port block */

#if ( STARTSHUT || DRIVERSHUT )
    krn$_post(&xppb->ei_ready_s);
#endif

/*Wait here for the first interrupt*/

    krn$_wait(&(xppb->rx_isr));

/*Do this forever*/

#if !( STARTSHUT || DRIVERSHUT )

    while (1) {
	while (!(xppb->rfd_first->rfd_status & XP_RFD_STAT_CPLT)) {
	    krn$_wait(&(xppb->rx_isr));
	}

	/*Process the Receive descriptors*/

	ei_process_rx(fp, ip, xppb);
    }
#endif

#if ( STARTSHUT || DRIVERSHUT )

    while (1) {

	while ((!(xppb->rfd_first->rfd_status & XP_RFD_STAT_CPLT)) && 
	  (xppb->pb.state != DDB$K_STOP))
	    krn$_wait(&(xppb->rx_isr));

	if (xppb->pb.state == DDB$K_STOP) {	/* YES: kill this process. */
	    dprintf("shutting down ei_rx...\n", p_args0);
	    fclose(fp);
	    dprintf("ei_rx: closed\n", p_args0);
	    return;
	}

/*Process the Receive descriptors*/

	ei_process_rx(fp, ip, xppb);
    }					/* while */
#endif
}

/*+
 * ============================================================================
 * = ei_configure_cmd - Send a configure & IAS command to the i82558.         =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will take care of the last few details to start up
 *	the device, including loading the offset registers, sending
 *	the configure command and setting the station address. The 
 *	commands will be executed in a polled mode.
 *
 * FORM OF CALL:                
 *
 *	ei_configure_cmd (ip, xppb);    
 *
 * RETURNS:
 *
 *	int Status - msg_success or FAILURE.
 *
 * ARGUMENTS:
 *
 *	struct INODE *ip - Pointer to the INODE
 *	struct I82558PB *xppb - Pointer to the xp port block.
 *
 * SIDE EFFECTS:          
 *
 *      None                    
 *
-*/

void ei_configure_cmd(struct INODE *ip, struct I82558PB *xppb)
{
    struct xp_config_cb config_cb;
    struct xp_ias_cb ias_cb;
    int i;

    dprintf("ei_configure_cmd\n", p_args0);

    config_cb.cb_status = 0;
    config_cb.cb_command = XP_CB_CMD_CONFIG | XP_CB_CMD_EL;
    config_cb.cb_link = I82558_NULL;

    config_cb.config_data[0] = XP_CFIG_DEFAULT_BYTE0;
    config_cb.config_data[1] = XP_CFIG_DEFAULT_BYTE1;
    config_cb.config_data[2] = XP_CFIG_DEFAULT_BYTE2;
    config_cb.config_data[3] = XP_CFIG_DEFAULT_BYTE3;
    config_cb.config_data[4] = XP_CFIG_DEFAULT_BYTE4;
    config_cb.config_data[5] = XP_CFIG_DEFAULT_BYTE5;
    config_cb.config_data[6] = XP_CFIG_DEFAULT_BYTE6;
    config_cb.config_data[7] = XP_CFIG_DEFAULT_BYTE7;
    config_cb.config_data[8] = XP_CFIG_DEFAULT_BYTE8;
    config_cb.config_data[9] = XP_CFIG_DEFAULT_BYTE9;
    config_cb.config_data[10] = XP_CFIG_DEFAULT_BYTE10;
    config_cb.config_data[11] = XP_CFIG_DEFAULT_BYTE11;
    config_cb.config_data[12] = XP_CFIG_DEFAULT_BYTE12;
    config_cb.config_data[13] = XP_CFIG_DEFAULT_BYTE13;
    config_cb.config_data[14] = XP_CFIG_DEFAULT_BYTE14;
    config_cb.config_data[15] = XP_CFIG_DEFAULT_BYTE15;
    config_cb.config_data[16] = XP_CFIG_DEFAULT_BYTE16;
    config_cb.config_data[17] = XP_CFIG_DEFAULT_BYTE17;
    config_cb.config_data[18] = XP_CFIG_DEFAULT_BYTE18;

    /* for external PHYs, the duplex must be forced in the controller */

    if (xppb->phy_adr[xppb->selected_phy] != 1) {
	if ((xppb->mode == XP_K_TWISTED_FD) || 
	  (xppb->mode == XP_K_FASTFD)) {
	    config_cb.config_data[19] = XP_CFIG_ALT_BYTE19;
	} else {
	    config_cb.config_data[19] = 0;
	}
    } else {
	config_cb.config_data[19] = XP_CFIG_DEFAULT_BYTE19;
    }

    config_cb.config_data[20] = XP_CFIG_DEFAULT_BYTE20;
    config_cb.config_data[21] = XP_CFIG_DEFAULT_BYTE21;

    if (xppb->loopback == 1)		/* internal loopback (in CSMA unit) */
	config_cb.config_data[10] = XP_CFIG_ILOOPBACK_BYTE10;
    if (xppb->loopback == 2)		/* "external" loopback (in PHY) */
	config_cb.config_data[10] = XP_CFIG_XLOOPBACK_BYTE10;

    ei_send_polled(xppb, &config_cb);
    dprintf("ei_configure_cmd: configure done\n", p_args0);

	/* execute the individual address setup command */

    ias_cb.cb_status = 0;
    ias_cb.cb_command = XP_CB_CMD_IAS | XP_CB_CMD_EL;
    ias_cb.cb_link = I82558_NULL;
    for (i = 0; i < 6; i++)
	ias_cb.iaddr[i] = xppb->sa[i];

    ei_send_polled(xppb, &ias_cb);
    dprintf("ei_configure_cmd: ias done\n", p_args0);

}

/*+
 * ============================================================================
 * = ei_send_polled - Send a command to the i82558 and wait for completion.   =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will issue a non-transmit command to the i82558
 *	and wait for its final status. This routine verifies that the CU
 *	and RU are idle.
 *
 * FORM OF CALL:                
 *
 *	ei_send_polled (xppb, cmd);    
 *
 * RETURNS:
 *
 *	int Status - msg_success or FAILURE.
 *
 * ARGUMENTS:
 *
 *	struct I82558PB *xppb - Pointer to the xp port block.
 *	struct xp_cb *cmd - Pointer to the command block.
 *
 * SIDE EFFECTS:          
 *
 *      None                    
 *
-*/

int ei_send_polled(struct I82558PB *xppb, struct xp_cb *cmd)
{
    unsigned int gen_ptr;
    unsigned short scb_status;
    int i;

    dprintf("ei_send_polled: send command: %04x\n", p_args1(cmd->cb_command));

    scb_status = XP_CSR_READ_WORD(xppb, XP_SCB_STATUS);
    if (((scb_status & XP_SCB_CUS_MASK) != XP_SCB_CUS_IDLE) && 
      ((scb_status & XP_SCB_CUS_MASK) != XP_SCB_CUS_SUSPENDED)) {
	err_printf("EI Driver: Controller must be idle or suspended for a polled command.\n");
	return (msg_failure);
    }

    cmd->cb_status = 0;
    cmd->cb_command |= XP_CB_CMD_EL;	/* end -o- list */

    /* Start execution with this command at the head of the list */

    if (ei_exec_cmd(xppb, XP_SCB_CMD_CU_START, cmd) == msg_failure) {
	scb_status = XP_CSR_READ_WORD(xppb, XP_SCB_STATUS);
	if (scb_status & XP_SCB_STATACK_MASK) {
	    dprintf("scb_status: %04x\n", p_args1(scb_status));
	    XP_CSR_WRITE_WORD(xppb, XP_SCB_STATUS, XP_SCB_STATACK_MASK);
	}
	return (msg_failure);
    }

    /* Wait for the command to be accepted */

    if (ei_wait_scb(xppb) == msg_failure) {
	scb_status = XP_CSR_READ_WORD(xppb, XP_SCB_STATUS);
	if (scb_status & XP_SCB_STATACK_MASK) {
	    dprintf("scb_status: %04x\n", p_args1(scb_status));
	    XP_CSR_WRITE_WORD(xppb, XP_SCB_STATUS, XP_SCB_STATACK_MASK);
	}
	return (msg_failure);
    }

    dprintf("ei_exec_cmd: Waiting for command completion.\n", p_args0);

    /* Wait for some status (up to 100 ms) */

    for (i = 100; i > 0; i--) {
	krn$_micro_delay(1000);

	if (cmd->cb_status & XP_CB_STATUS_C) {
	    dprintf("Command Complete\n", p_args0);
	    break;
	}
    }

    if (i <= 0) {
	ni_error(xppb->pb.name, "Command timeout.");
    }

    /* ack any pending interrupts */

    scb_status = XP_CSR_READ_WORD(xppb, XP_SCB_STATUS);
    if (scb_status & XP_SCB_STATACK_MASK) {
	XP_CSR_WRITE_WORD(xppb, XP_SCB_STATUS, XP_SCB_STATACK_MASK);
    }

    if (i > 0)
	return (msg_success);
    else
	return (msg_failure);
}

/*+
 * ============================================================================
 * = ei_wait_scb - Wait for a command to be accepted by the i82558.           =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will poll the SCB command register until the command
 *	fields are clear, signifying that it is ready for a new command.
 *
 * FORM OF CALL:                
 *
 *	ei_wait_scb (xppb);    
 *
 * RETURNS:
 *
 *	int Status - msg_success or FAILURE.
 *
 * ARGUMENTS:
 *
 *	struct I82558PB *xppb - Pointer to the xp port block.
 *
 * SIDE EFFECTS:          
 *
 *      None                    
 *
-*/

int ei_wait_scb(struct I82558PB *xppb)
{
    unsigned short scb_cmd;
    int retry = 30;

    dprintf("ei_wait_scb\n", p_args0);

    do {
	scb_cmd = XP_CSR_READ_WORD(xppb, XP_SCB_CMD);
	if ((scb_cmd & (XP_SCB_CMD_CU_MASK | XP_SCB_CMD_RU_MASK)) == 0)
	    break;
	krn$_micro_delay(1000);
    } while (--retry);

    if (retry == 0) {
	ni_error(xppb->pb.name, "Command Unit Timeout.");
	return (msg_failure);
    }

    return (msg_success);
}

/*+
 * ============================================================================
 * = ei_exec_cmd - Issue an i82558 SCB command.                               =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will issue the specified command to the SCB.
 *
 * FORM OF CALL:                
 *
 *	ei_exec_cmd (xppb, cmd, ptr);    
 *
 * RETURNS:
 *
 *	int Status - msg_success or FAILURE.
 *
 * ARGUMENTS:
 *
 *	struct I82558PB *xppb - Pointer to the xp port block.
 *	unsigned char cmd - CU/RU command to issue
 *	unsigned int ptr - general pointer value
 *
 * SIDE EFFECTS:          
 *
 *      None                    
 *
-*/

int ei_exec_cmd(struct I82558PB *xppb, unsigned char cmd, unsigned int ptr)
{
    dprintf("ei_exec_cmd: %x, %08x\n", p_args2(cmd, ptr));

    if (ei_wait_scb(xppb) == msg_failure)
	return (msg_failure);

    mb(); /* just in case */
    XP_CSR_WRITE_LONG(xppb, XP_SCB_GENPTR, ptr);
    XP_CSR_WRITE_BYTE(xppb, XP_SCB_CMD, cmd);

    return (msg_success);
}

/*+
 * ============================================================================
 * =  ei_display_state - format and print the i82558 RU and CU state.         =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will format and print the scb_status CSR.
 *
 * FORM OF CALL:                
 *
 *	ei_display_state (scb_status, debug);
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	unsigned short scb_status - i82558 status word.
 *	int debug - flag indicating to use d8printf or printf.
 *
 * SIDE EFFECTS:          
 *
 *      None                    
 *
-*/

void ei_display_state(unsigned short scb_status, int debug)
{
    static const char *cu_state[] = {"Idle", "Susp", "Act", "?"};

    static const char *ru_state[] = {"Idle", "Susp", "NoRsrc", "?",
      "Rdy", "?", "?", "?",
      "?", "Susp/noRBD", "NoRsrc/noRBD", "?",
      "Rdy/noRBD", "?", "?", "?"};

    /* these are in order, bit 8 to bit 15, so they can be done in a loop */

    static const char *err_bits[] = {
      " Flow-ctl-pause",
      " Early-rx",
      " SW-int",
      " MDI-rw",
      " RU-not-ready",
      " CU-not-active",
      " Frame-rx",
      " Cmdblk-cplt"};

    static char hdr_fmt[] = "  RU State: %s, CU State: %s\n";
    char *rus, *cus;
    int i;

    rus = ru_state[(scb_status & XP_SCB_RUS_MASK) >> 2];

    cus = cu_state[(scb_status & XP_SCB_CUS_MASK) >> 6];

    if (debug)
	dprintf(hdr_fmt, p_args2(rus, cus));
    else
	printf(hdr_fmt, rus, cus);

    for (i = 8; i < 16; i++) {
	if (scb_status & (1 << i)) {
	    if (debug)
		dprintf(err_bits[i - 8], p_args0);
	    else
		printf(err_bits[i - 8]);
	}
    }

    if (debug)
	dprintf("\n", p_args0);
    else
	printf("\n");
}

/*+                      
 * ============================================================================
 * = ei_poll - i82558 polling routine.                                        =
 * ============================================================================
 *               
 * OVERVIEW:                                                                  
 *  
 *      This routine is the polling routine for the i82558 driver.
 *	It is called from the timer task. It will simply check for
 *	an interrupt and call the isr if there is one.
 *           
 * FORM OF CALL:                                                 
 *  
 *  	ei_poll (xppb);
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

void ei_poll(struct NI_GBL *np)
{
    struct I82558PB *xppb;		/*Pointer to the port block*/

/*Get some pointers*/

    xppb = (struct I82558PB *) np->pbp;

/*If the hardware is still not initialized return*/

    if (xppb->state == XP_K_UNINITIALIZED)
	return;

    if (xppb->state == XP_K_STOPPED)
	return;

/*If we have an interrupt call the isr*/

    if (XP_CSR_READ_WORD(xppb, XP_SCB_STATUS) & (XP_SCB_STATACK_MASK))
	ei_interrupt(np);
}

/*+
 * ============================================================================
 * = ei_interrupt - i82558 interrupt handler.                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will service interrupts from the i82558 chip.
 *
 * FORM OF CALL:                
 *
 *	ei_interrupt (struct NI_GBL *np);    
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

void ei_interrupt(struct NI_GBL *np)
{
    struct I82558PB *xppb;
    unsigned short scb_status;

    dprintf("ei_interrupt\n", p_args0);

    /* retrieve our port block pointer */

    xppb = np->pbp;

    if (xppb->cb_active > xppb->cb_highwater)
	xppb->cb_highwater = xppb->cb_active;

    /* decode the type of interrupt */

    scb_status = XP_CSR_READ_WORD(xppb, XP_SCB_STATUS);

#if DEBUG
    dprintf("ei_interrupt: scb_status: %04x\n", scb_status);
    if (xp_debug & 0x8)
	ei_display_state(scb_status, 1);
#endif

    /* ack any pending interrupts */

    while (scb_status & XP_SCB_STATACK_MASK) {

	xppb->last_scb_status = scb_status;
	XP_CSR_WRITE_WORD(xppb, XP_SCB_STATUS, XP_SCB_STATACK_MASK);

	/* end of command list or command complete, process tx */

	if (scb_status & (XP_SCB_STATACK_CNA | XP_SCB_STATACK_CX))
	    ei_process_tx(xppb);

	/* frame rx complete, post rx semaphore */

	if (scb_status & (XP_SCB_STATACK_FR | XP_SCB_STATACK_RNR))
	    krn$_bpost(&(xppb->rx_isr));

	/* check again */

	scb_status = XP_CSR_READ_WORD(xppb, XP_SCB_STATUS);
    }
}

/*+
 * ============================================================================
 * = ei_open - Open the EI (i82558) device.				      =
 * ============================================================================
 *
 * OVERVIEW:         
 *  
 *      Open the i82558 device for callback use. If the optional "multi"
 *	parameter is passed with a multicast address as a string, it is
 *	added to the filter.
 *   
 * FORM OF CALL:
 *  
 *	ei_open (fp, multi)
 *  
 * RETURNS:
 *
 *	msg_success - Success
 *       
 * ARGUMENTS:
 *
 *	struct FILE *fp - File to be opened.
 *	char *multi - multicast address string.
 *
 * SIDE EFFECTS:
 *
 *	None
 *
 *  NOTE: The filter on the i82558 is NOT PERFECT like the tulip.
 *  any address which hashes to the same value will pass.
 * 
-*/

int ei_open(struct FILE *fp, char *multi)
{
    struct INODE *ip;
    struct I82558PB *xppb;		/* Pointer to the port block */
    struct NI_GBL *np;			/* Pointer to the NI global database */
    struct xp_mcs_cb *ptr;
    struct xp_txcb *cbp;
    unsigned char multicast_addr[6];
    int valid;
    int delay;

    ip = (struct INODE *) fp->ip;
    np = (struct NI_GBL *) ip->misc;
    xppb = (struct I82558PB *) np->pbp;

    if (multi && (multi[0] != '\0')) {
	ip = (struct INODE *) fp->ip;
	np = (struct NI_GBL *) ip->misc;
	xppb = (struct I82558PB *) np->pbp;

	dprintf("ei_open: Multicast: %s\n", multi);

	/* validate the address */

	valid = convert_enet_address(multicast_addr, multi);

	/* send a setup frame if a valid multicast address exists  */

	if ((valid == msg_success) && (multicast_addr[0] & 0x01)) {
	    pprintf("Display converted mc address: %02x-%02x-%02x-%02x-%02x-%02x \n",
	      multicast_addr[0], multicast_addr[1], multicast_addr[2],
	      multicast_addr[3], multicast_addr[4], multicast_addr[5]);
	} else {
	    return msg_success;
	}

	/* get the next tx cmd buffer & cast to a multicast setup cmd */

	/* take out the spinlock */

	spinlock(&(xppb->spl_port));

	/* find the first available Command Block */

	if (xppb->cb_active < ei_xmt_buf_no) {
	    cbp = xppb->cbl_last->cb_link;

	} else {
	    spinunlock(&(xppb->spl_port));	/* release */
	    ni_error(xppb->pb.name, "No free command blocks.");

	    /* Bump the count */

	    xppb->mc->MOP_V4CNT_NO_USER_BUFFER++;

	    return (msg_failure);
	}

	ptr = (struct xp_mcs_cb *) cbp;

	/* fill in the mc setup command */

	ptr->cb_status = 0;
	ptr->cb_command = XP_CB_CMD_MCAS | XP_CB_CMD_S;
	ptr->mc_count = 6;
	ptr->mc_addr[0] = multicast_addr[0];
	ptr->mc_addr[1] = multicast_addr[1];
	ptr->mc_addr[2] = multicast_addr[2];
	ptr->mc_addr[3] = multicast_addr[3];
	ptr->mc_addr[4] = multicast_addr[4];
	ptr->mc_addr[5] = multicast_addr[5];

	/* kick off the cmd */

	xppb->cbl_last->cb_command &= ~XP_CB_CMD_S;	/* reset the prev. "S" bit */

	xppb->cbl_last = cbp;		/* this is the last command */
	if (xppb->cb_active == 0)
	    xppb->cbl_first = cbp;		/* this is also the first */
	xppb->cb_active++;

	/* release the spinlock */

	spinunlock(&(xppb->spl_port));

	dprintf("ei_open: Queueing command block: %08x, cb_active: %d\n",
	  p_args2(cbp, xppb->cb_active));

	/* resume the transmit */

	ei_exec_cmd(xppb, XP_SCB_CMD_CU_RESUME, 0);

	/* wait for completion */

	delay = 0;
	while (!(cbp->cb_status & XP_CB_STATUS_C)) {
	    krn$_sleep(1);
	    delay++;
	    if (delay > 1000) {
	    }
	    ni_error(xppb->pb.name,
	      "TX timeout - cb %08x, active %d",
	      (unsigned int) cbp, xppb->cb_active);
	}

    } else
	dprintf("ei_open: No Multicast, xppb: %08x\n", p_args1(xppb));

    return (msg_success);
}

/*+
 * ============================================================================
 * = ei_write - Driver write routine.                                         =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine does a write to the i82558 port. It is called from
 *	the file system
 *
 * FORM OF CALL:
 *
 *	ei_write (fp, item_size, number, buf); 
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

int ei_write(struct FILE *fp, int item_size, int number, char *buf)
{
    struct INODE *ip;			/* Pointer to the INODE */
    struct NI_GBL *np;			/* Pointer to the NI global database */
    struct I82558PB *xppb;		/* Pointer to the port block */

/*Get some pointers*/

    ip = fp->ip;
    np = (struct NI_GBL *) ip->misc;
    xppb = (struct I82558PB *) np->pbp;

    if (xppb->state != XP_K_STARTED) {
	if (!cb_ref) {		/* deallocate the packet */
	    ndl_deallocate_pkt(ip, buf);
	}
	return (0);			/* no bytes sent */
    }

    if (cb_ref)
	memcpy(buf + 6, xppb->sa, 6);	/* set source address for callbacks */

/*Return the length*/

    return (ei_send(ip, xppb, buf, item_size * number, 0));
}

/*+
 * ============================================================================
 * = ei_send - Send a message via the command block list.		      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will build up a transmit command block.
 *
 * FORM OF CALL:                
 *
 *	ei_send (ip, xppb, buf, len, setup);
 *
 * RETURNS:
 *
 *	int size - the number of bytes transmitted.
 *
 * ARGUMENTS:
 *
 *	struct INODE *ip - INODE pointer
 *	struct I82558PB *xppb - Pointer to the xp port block.
 *	char *buf - Buffer to be sent.
 *	int len - number of bytes to be sent.
 *	int setup - Send a non-tx frame or not
 *		    1 = this is a non-tx frame
 *		    0 = this is a tx frame
 *
 * SIDE EFFECTS:          
 *
 *	none
 *
-*/

int ei_send(struct INODE *ip, struct I82558PB *xppb, char *buf, int len, int setup)
{
    struct xp_txcb *cbp;

    dprintf("ei_send: buf: %08x len: %04x setup: %d\n", p_args3(buf, len, setup));

    if (xppb->cb_active > xppb->cb_highwater)
	xppb->cb_highwater = xppb->cb_active;

/* I. transmit the packet (follow the flow in section 10.2 of the manual */

    krn$_wait(&(xppb->tx_wait));	/* serialize this */

    /* take out the spinlock */

    spinlock(&(xppb->spl_port));

    /* find the first available Command Block */

    while (1) {
	if (xppb->cb_active < ei_xmt_buf_no) {
	    cbp = xppb->cbl_last->cb_link;
	    break;

	} else {
	    spinunlock(&(xppb->spl_port));	/* release */
	    ni_error(xppb->pb.name, "No free command blocks.");

#if DEBUG
	    ei_dumppb(xppb);
	    ei_dump_csrs(ip->misc);
	    if (xp_debug & 0x10)
		do_bpt();
#endif
	    /* Bump the count */

	    xppb->mc->MOP_V4CNT_NO_USER_BUFFER++;

	    if (!cb_ref) {		/* deallocate the packet */
		ndl_deallocate_pkt(ip, buf);
	    }
	    return (0);			/* no bytes sent */
	}
    }

    /* fill it in */

    /* xppb->tx_isr[cbp->index].sem.count = 0; */
    xppb->tx_isr[cbp->index].alt = FALSE;

    cbp->cb_status = 0;
    cbp->cb_command = XP_CB_CMD_XMIT | 	/* transmit */
      XP_CB_CMD_S | 			/* (current) end of list */
      XP_CB_CMD_I |			/* interrupt when done */
      XP_CB_CMD_SF;			/* flexible mode */

    cbp->cb_tbd_array_ptr = &(cbp->cb_txb0_addr);	/* use built in TBD */
    cbp->cb_txcb_cnt = 0;
    cbp->cb_txthresh = 0;
    cbp->cb_tbd_num = 1;
    cbp->cb_txb0_addr = buf + WINDOW_BASE(xppb);	/* needs offset added */
    cbp->cb_txb0_size = len | 0x8000;	/* end of list */

    xppb->cbl_last->cb_command &= ~XP_CB_CMD_S;	/* reset the prev. "S" bit */

    xppb->cbl_last = cbp;		/* this is the last command */
    if (xppb->cb_active == 0)
	xppb->cbl_first = cbp;		/* this is also the first */
    xppb->cb_active++;
    if (xppb->cb_active > xppb->cb_highwater)
	xppb->cb_highwater = xppb->cb_active;

    /* resume the transmit */

    xppb->last_cb_out = cbp;
    ei_exec_cmd(xppb, XP_SCB_CMD_CU_RESUME, 0);

    dprintf("ei_send: Queued command block: %08x, index: %d, cb_active: %d\n",
      p_args3(cbp, cbp->index, xppb->cb_active));

    /* start a timer */

#if DEBUG
    if (xppb->tx_isr[cbp->index].sem.count) {
	pprintf("ei_send: index %x has timer semaphore value: %d\n",
	    cbp->index, xppb->tx_isr[cbp->index].sem.count);
	if (xp_debug & 0x10) {
	    ei_dumppb(xppb);
	    spinunlock(&(xppb->spl_port));
	    ei_dump_csrs(ip->misc);
	    ei_show(ip->misc);
	    do_bpt();
	}
    }
#endif
    krn$_start_timer(&(xppb->tx_isr[cbp->index]), 500);

    /* release the spinlock */

    spinunlock(&(xppb->spl_port));

    /* wait for completion */

    dprintf("ei_send: waiting on timer: %d, %d\n", 
	p_args2(cbp->index, &(xppb->tx_isr[cbp->index])));
    krn$_wait(&(xppb->tx_isr[cbp->index].sem));
    dprintf("ei_send: done waiting timer: %d\n", p_args1(cbp->index));
    krn$_stop_timer(&(xppb->tx_isr[cbp->index]));
    dprintf("ei_send: stopped timer: %d\n", p_args1(cbp->index));

    if (!xppb->tx_isr[cbp->index].alt) {
	/* only declare a timeout if it didn't finish */
	/* but count it anyway */
	if ( !(cbp->cb_status & XP_CB_STATUS_C)) {
	    ni_error(xppb->pb.name,
	      "TX timeout - index %d, cb %08x, sem %08x, active %d",
	      cbp->index, (unsigned int) cbp, 
	      &(xppb->tx_isr[cbp->index]), xppb->cb_active);
#if DEBUG
	    if (xp_debug & 0x10) {
		ei_dumppb(xppb);
		ei_dump_csrs(ip->misc);
		ei_show(ip->misc);
		do_bpt();
	    }
#endif

	    len = 0;
	} else {
	    ei_process_tx(xppb);		    /* force process */
	    xppb->tx_isr[cbp->index].sem.count = 0; /* clean up */
	}
	xppb->last_to_index = cbp->index;
	xppb->cu_timeouts++;
    }

    krn$_post(&(xppb->tx_wait));	/* let in one waiter */

/* II. deallocate the packet */

    if (!cb_ref) {
	ndl_deallocate_pkt(ip, buf);
    }

/* III. return the number of bytes sent */

    dprintf("ei_send: exit\n", p_args0);
    return (len);
}

/*+
 * ============================================================================
 * = ei_process_tx - Complete tx processing after an interrupt.               =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will complete the transmit frame processing.
 *	All completed command blocks will be reclaimed.
 *	Note that the command list is also used to send Multicast Setup
 *	commands which are not strictly xmits, but they still get
 *	completed here.
 *
 * FORM OF CALL:                
 *
 *	ei_process_tx (xppb);    
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	struct I82558PB *xppb - Pointer to the xp port block.
 *
 * SIDE EFFECTS:          
 *
 *      None                    
 *
-*/

void ei_process_tx(struct I82558PB *xppb)
{
    struct xp_cb *cbp;
    int index;

    dprintf("ei_process_tx\n", p_args0);

    spinlock(&(xppb->spl_port));

    cbp = xppb->cbl_first;
#if DEBUG
    if (!(cbp->cb_status & XP_CB_STATUS_C))
	pprintf("ei_process_tx: command block not complete.\n");
#endif

    while (cbp->cb_status & XP_CB_STATUS_C) {

	/* we have a completed command */

	dprintf("ei_process_tx: completed CB %08x\n", p_args1(cbp));
	xppb->last_cb_in = cbp;

	/* decrement the outstanding count */

	xppb->cb_active--;
	if ((cbp == xppb->cbl_first) && (xppb->cbl_first != xppb->cbl_last))
	    xppb->cbl_first = cbp->cb_link;

	/* clear the complete and OK status flags for this command block */

	cbp->cb_status &= ~(XP_CB_STATUS_OK | XP_CB_STATUS_C);

	if ((cbp->cb_command & XP_CB_CMD_MASK) == XP_CB_CMD_XMIT) {
	    xppb->mc->MOP_V4CNT_TX_FRAMES++;
	    xppb->mc->MOP_V4CNT_TX_BYTES += 
	      (((struct xp_txcb *) cbp)->cb_txb0_size & 0x7fff);

	    /* post the waiter */

	    index = ((struct xp_txcb *) cbp)->index;
	    xppb->last_index = index;
	    dprintf("ei_process_tx: posting tqe %d\n", p_args1(index));
#if DEBUG
	    if ((index < 0) || (index > ( ei_xmt_buf_no-1 )))
		pprintf("ei_process_tx: bogus index value: %x\n",
		    index);
#endif
	    xppb->tx_isr[index].alt = TRUE;
	    krn$_post(&(xppb->tx_isr[index].sem));

	} else {
#if DEBUG
	    pprintf("ei_process_tx: Not an xmit cmd\n");
#endif
	}

	cbp = cbp->cb_link;
    }

    spinunlock(&(xppb->spl_port));

    /* call ei_restart_cu to handle any nasty transitions to the idle state */

    ei_restart_cu(xppb);
}

/*+
 * ============================================================================
 * = ei_process_rx - Receive Frame Descriptor processing.                     =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will traverse the RFD list and send off the
 *	received frames. All completed buffers on the list will be
 *	handled and the list will be repopulated with clean buffers.
 *
 *	Note: When we're done, we check the current state of the RU.
 *	If it's not ready (out of RFD's/RBD's) we restart it.
 *
 * FORM OF CALL:                
 *
 *	ei_process_rx (fp, ip, xppb);
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	struct FILE *fp - Pointer to the receive port file.
 *	struct INODE *ip - Pointer to the device's inode.
 *	struct I82558PB *xppb - Pointer to the xp port block.
 *
 * SIDE EFFECTS:          
 *
 *      None                    
 *
-*/

void ei_process_rx(struct FILE *fp, struct INODE *ip, struct I82558PB *xppb)
{
    struct xp_rfd *rfdp;
    struct xp_rbd *rbdp;
    unsigned short status;
    unsigned char *pkt;
    struct EI_MSG *mh;
    int len;

    dprintf("ei_process_rx\n", p_args0);
    rfdp = xppb->rfd_first;

    while (rfdp->rfd_status & XP_RFD_STAT_CPLT) {

	/* we have a completed packet */

	dprintf("ei_process_rx: completed RFD: %08x, status: %04x\n",
	  p_args2(rfdp, rfdp->rfd_status));

	/* check for errors */

	status = rfdp->rfd_status & 
	  ~(XP_RFD_STAT_CPLT | XP_RFD_STAT_OK | 
	  XP_RFD_STAT_TYPELEN | XP_RFD_STAT_IAMATCH);

	if (status) {
	    ni_error(xppb->pb.name, "Rx packet error.");
	    pkt = NULL;
	    len = 0;

	} else {

	    /* find the corresponding RBD */

	    /* it should be the one in the list pointed to by "rbd_first" */

	    rbdp = xppb->rbd_first;

	    if ((rbdp->actual_size & XP_RBD_COUNT_F) == 0) {
		err_printf("EI Driver: No data for rx frame.\n");
	    }

	    if ((rbdp->actual_size & XP_RBD_COUNT_EOF) == 0) {
		err_printf("EI Driver: Frame doesn't fit in one rx buf.\n");
	    }

	    /* convert the windowed pointer back */

	    pkt = rbdp->rcv_buf_adr - WINDOW_BASE(xppb);

	    /* strip off the flags from the length count */

	    len = rbdp->actual_size & ~(XP_RFD_COUNT_F | XP_RFD_COUNT_EOF);
	}

	ei_rx_count(xppb, rfdp->rfd_status, len);

	/* set this packet up to be reused */

	if (pkt) {			/* only if we got a data packet */

	    /* new buffer on the RBD */

	    rbdp->rcv_buf_adr = ndl_allocate_pkt(ip, 1) + WINDOW_BASE(xppb);
	    rbdp->actual_size = 0;
	    rbdp->size |= XP_RBD_SIZE_EL;	/* new end-o-list */
	}

	spinlock(&(xppb->spl_port));

	/* clear the flags out of the RFD */

	rfdp->rfd_status = 0;
	rfdp->rfd_control = XP_RFD_CONTROL_SF | 
	  XP_RFD_CONTROL_EL;		/* new end-o-list */

	/* update the port block's RFD and RBD lists */

	xppb->rfd_first = rfdp->link_addr;
	xppb->rfd_last->rfd_control &= ~XP_RFD_CONTROL_EL;	/* clear end */
	xppb->rfd_last = rfdp;

	if (pkt) {			/* only if we got a data packet */
	    xppb->rbd_first = rbdp->next_rbd_adr;
	    xppb->rbd_last->size &= ~XP_RBD_SIZE_EL;	/* clear end */
	    xppb->rbd_last = rbdp;
	}

	spinunlock(&(xppb->spl_port));

	/* get the data back to the user */

	if (pkt) {
#if DEBUG
	    if (xp_debug & 4)
		pprint_pkt(pkt, len);
#endif
	    if (cb_ref) {		/* callback request */

		if (xppb->rcv_msg_cnt > EI_MAX_RCV_MSGS) {

		    /* free up messages on the callback list */

		    ei_free_rcv_pkts(xppb);
		}

		mh = (struct EI_MSG *) malloc(sizeof(struct EI_MSG));

		mh->msg = pkt;
		mh->size = len;

		/* insert onto the save message queue */

		spinlock(&spl_kernel);
		insq(mh, xppb->rqh.blink);
		xppb->rcv_msg_cnt++;
		spinunlock(&spl_kernel);

	    } else {			/* filesystem */
		dprintf("ei_process_rx: calling datalink.\n", p_args0);
		if (xppb->lrp)
		    (*(xppb->lrp))(fp, pkt, len);
		dprintf("ei_process_rx: back from datalink.\n", p_args0);
	    }
	}				/* end of if (pkt) */

	rfdp = rfdp->link_addr;
    }					/* end of while there are complete packets */

    /* determine the current RU state */

    /* If it's not ready (and we're note stopped), restart it */

    ei_restart_ru(xppb);

}

/*+
 * ============================================================================
 * = ei_rx_count - Count the receive.                                         =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This handle the receive mop counters.
 *	NOTE: If the i82558 is not configured in "Save Bad Frames" mode,
 *	we should never see many of the interrupt bits set.
 *
 * FORM OF CALL:
 *
 *	ei_rx_count (xppb,rfd_status,len); 
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	struct I82558PB *xppb - Pointer to the xp port block.
 *	unsigned short rfd_status - status field from the RFD
 *	unsigned short len - length of rx packet
 *
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void ei_rx_count(struct I82558PB *xppb, unsigned short rfd_status,
  unsigned short len)
{

/*Count the missed frames*/

    if (rfd_status & XP_RFD_STAT_RNR)
	xppb->mc->MOP_V4CNT_NO_SYSTEM_BUFFER++;

/*If it's an error count the error if not count the packet*/
/*Be sure that this packet has vaild info by checking for the LS bit*/

    if (rfd_status & 
      ~(XP_RFD_STAT_CPLT | XP_RFD_STAT_OK | 
      XP_RFD_STAT_TYPELEN | XP_RFD_STAT_IAMATCH)) {

/*Count the errors*/

	if (rfd_status & XP_RFD_STAT_CRC)
	    xppb->mc->MOP_V4CNT_RX_FAIL_BLOCK_CHECK++;
	if (rfd_status & XP_RFD_STAT_ALIGN)
	    xppb->mc->MOP_V4CNT_RX_FAIL_FRAMING_ERR++;
	if (rfd_status & XP_RFD_STAT_OVERRUN)
	    xppb->mc->MOP_V4CNT_DATA_OVERRUN++;
    } else {

/*Check for multicast (IAMATCH = 1 indicates broadcast or multicast) */

	if (rfd_status & XP_RFD_STAT_IAMATCH) {

/*Count the number of bytes*/

	    xppb->mc->MOP_V4CNT_RX_MCAST_BYTES += len;

/*Count the frames*/

	    xppb->mc->MOP_V4CNT_RX_MCAST_FRAMES++;
	} else {

/*Count the number of bytes*/

	    xppb->mc->MOP_V4CNT_RX_BYTES += len;

/*Count the frames*/

	    xppb->mc->MOP_V4CNT_RX_FRAMES++;
	}
    }
}

/*+
 * ============================================================================
 * = ei_free_rcv_pkts - Free packets from the callback rx queue.              =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will deallocate (and flush) all saved rx packets
 *	on the receive queue. This is normally done on a receive overrun
 *	when callback reads are not coming fast enough.
 *
 * FORM OF CALL:                
 *
 *	ei_free_rcv_pkts (xppb);    
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	struct I82558PB *xppb - Pointer to the xp port block.
 *
 * SIDE EFFECTS:          
 *
 *      None                    
 *
-*/

void ei_free_rcv_pkts(struct INODE *ip)
{
    struct NI_GBL *np;			/* pointer to NI global database */
    struct I82558PB *xppb;		/* pointer to the port block */
    struct EI_MSG *mr;			/* pointer to a received message */

    /* initialize pointers */

    np = (struct NI_GBL *) ip->misc;
    xppb = np->pbp;

    spinlock(&spl_kernel);
    while (xppb->rqh.flink != (struct EI_MSG *) &(xppb->rqh.flink)) {

	/* remove the packet */

	mr = (struct EI_MSG *) remq(xppb->rqh.flink);
	xppb->rcv_msg_cnt--;
	spinunlock(&spl_kernel);

	/* free the packet */

	ndl_deallocate_pkt(ip, mr->msg);
	free(mr);

	/* synchronize next access */

	spinlock(&spl_kernel);
    }

    spinunlock(&spl_kernel);
}

/*+
 * ============================================================================
 * = ei_show - Dump i82558 information.					      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine is used to display the i82558 statistics counters.
 *
 * FORM OF CALL:                
 *
 *	ei_show (np);    
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	struct NI_GBL *np - Pointer to the NI global database.
 *
 * SIDE EFFECTS:          
 *
 *      None                    
 *
-*/

void ei_show(struct NI_GBL *np)
{
    struct I82558PB *xppb;		/* pointer to the port block */
    struct xp_stats *xps;
    int i;
    unsigned int gen_ptr;

    xppb = (struct I82558PB *) np->pbp;

    xps = (struct xp_stats *) malloc(sizeof(struct xp_stats));
    dprintf("ei_show: stat buffer: %08x\n", p_args1(xps));
    if (xps == NULL)
	return;

    /* get a statistics dump */

    gen_ptr = ((unsigned int) xps) + WINDOW_BASE(xppb);
    ei_exec_cmd(xppb, XP_SCB_CMD_CU_DUMP_ADR, gen_ptr);
    xps->completion_status = 0;
    ei_exec_cmd(xppb, XP_SCB_CMD_CU_DUMP, xps);

    for (i = 0; i < 20; i++) {
	if (xps->completion_status)
	    break;
	krn$_sleep(100);
    }

    if (xps->completion_status == 0) {
	err_printf("EI Driver: Timeout obtaining i82558 stats.\n");
	return;
    }

    printf("i82558 Statistics:\n");
    printf(" TX:\n");
    printf(" Good Frames: %d, Max Collisions: %d, Late Collisions: %d\n",
      xps->tx_good, xps->tx_maxcols, xps->tx_latecols);
    printf(" Underruns: %d, Lost CRS: %d, Deferred: %d\n",
      xps->tx_underruns, xps->tx_lostcrs, xps->tx_deferred);
    printf(" Single Coll.: %d, Multiple Coll.: %d, Total Coll.: %d\n",
      xps->tx_single_collisions, xps->tx_multiple_collisions,
      xps->tx_total_collisions);

    printf(" RX:\n");
    printf(" Good Frames: %d, CRC errors: %d, Align errors: %d\n",
      xps->rx_good, xps->rx_crc_errors, xps->rx_alignment_errors);
    printf(" Rx not ready: %d, Overrun: %d, Coll Detect: %d, Short Frames: %d\n",
      xps->rx_rnr_errors, xps->rx_overrun_errors,
      xps->rx_cdt_errors, xps->rx_shortframes);

    printf("\n RU Restarts: %d, CU Restarts: %d, CU Timeouts: %d\n",
      xppb->ru_restarts, xppb->cu_restarts, xppb->cu_timeouts);

    free(xps);
}

/*+
 * ============================================================================
 * = ei_dump_csrs - Print out the i82558 SCB registers.                       =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will dump out the CSR registers in the SCB.
 *
 * FORM OF CALL:                
 *
 *	ei_dump_csrs (np);    
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	struct NI_GBL *np - Pointer to the NI global database.
 *
 * SIDE EFFECTS:          
 *
 *      None                    
 *
-*/

void ei_dump_csrs(struct NI_GBL *np)
{
    struct I82558PB *xppb;
    unsigned short scb_command, scb_status;
    unsigned int scb_ptr;
    unsigned char *dump_ptr;
    int retry_cnt;

    xppb = np->pbp;

    scb_status = XP_CSR_READ_WORD(xppb, XP_SCB_STATUS);
    scb_command = XP_CSR_READ_WORD(xppb, XP_SCB_CMD);
    scb_ptr = XP_CSR_READ_LONG(xppb, XP_SCB_GENPTR);

    printf("i82558 CSRs:\n");
    printf("  Register Base: %08x\n", xppb->pb.csr);
    printf("  scb_command: %04x, scb_status: %04x\n",
      scb_command, scb_status);
    printf("  scb_general_ptr: %08x\n", scb_ptr);

    ei_display_state(scb_status, 0);

    printf("  CU_Base: %08x, RU_Base: %08x\n",
      WINDOW_BASE(xppb), WINDOW_BASE(xppb));

#if DEBUG

    /* Dump all the internal registers -- for debug */

    /* Note: this is dependent on the setting of xp_debug */

    /* since it seems to send the chip into la-la land. */

    if (xp_debug & 0x1000) {
	dump_ptr = malloc(1024);
	if (dump_ptr == NULL)
	    return;

	/* the dump pointer has to be aligned on a 16 byte boundary */

	dump_ptr = ((((unsigned int) dump_ptr) + 15) & ~0xf);
	dprintf("Dumping internal state at: %08x\n", p_args1(dump_ptr));

	dump_ptr[596] = 0;
	dump_ptr[597] = 0;
	retry_cnt = 1000;
	XP_CSR_WRITE_LONG(xppb, XP_PORT,
	  XP_PORT_DUMP | (int) dump_ptr | WINDOW_BASE(xppb));

	do {
	    krn$_sleep(100);
	} while ((dump_ptr[596] == 0) && (retry_cnt--));

	dprintf("i82558 Dump Results: %02x%02x\n",
	  p_args2(dump_ptr[597], dump_ptr[596]));
    }
#endif
}

#if (! STARTSHUT)
/*+
 * ============================================================================
 * = ei_setmode - Set mode                                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Set driver mode.
 *  
 * FORM OF CALL:
 *  
 *	ei_setmode (mode)
 *  
 * RETURNS:
 *
 *	msg_success - Normal completion.
 *       
 * ARGUMENTS:
 *
 *	int mode - Mode the driver is to be changed to.
 *		    DDB$K_START
 *		    DDB$K_STOP
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/

void ei_setmode(int mode)
{
    struct PBQ *pbe;			/*Port block list element*/
    struct INODE *ip;

    pbe = xppblist.flink;

    while (pbe != &(xppblist)) {

/*Stop if we're asked to stop and are not already stopped*/

	if (mode == DDB$K_STOP) {

	    /*Execute the stop routine*/

	    ei_stop_driver((struct INODE *) pbe->pb);

	} else if (mode == DDB$K_START) {

	    /* Restart */

	    ei_restart_driver((struct INODE *) pbe->pb);
	}

	/*Get the next element*/

	pbe = pbe->flink;
    }
}
#endif

/*+
 * ============================================================================
 * = ei_stop_driver - Stops the driver.                                       =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will stop the i82558 driver.
 *
 * FORM OF CALL:
 *
 *	ei_stop_driver (ip); 
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

int ei_stop_driver(struct INODE *ip)
{
    struct NI_GBL *np;			/*Pointer to the NI global database*/
    struct I82558PB *xppb;		/*Pointer to the port block*/

    dprintf("ei_stop_driver: %s\n", p_args1(ip->name));

/*Get some pointers*/

    np = (struct NI_GBL *) ip->misc;
    xppb = (struct I82558PB *) np->pbp;

/*If we are already stopped don't stop again*/

    if (xppb->state == XP_K_STOPPED)
	return (msg_success);

/*If we are unitialized return a failure*/

    if (xppb->state != XP_K_STARTED)
	return (msg_failure);

/*Set the state flag*/

    xppb->state = XP_K_STOPPED;

/*Make sure the device is stopped*/

    ei_init_device(xppb);

/*Check for polled or not*/

    if (xppb->pb.vector != 0)
	io_disable_interrupts(xppb, xppb->pb.vector);
}

/*+
 * ============================================================================
 * = ei_change_mode - Change the i82558's mode.                                =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will change the i82558 modes. It uses the
 *	specified constant for the mode to be changed to.
 *
 * FORM OF CALL:
 *
 *	ei_change_mode (fp,mode); 
 *                            
 * RETURNS:
 *
 *	int Status - msg_success or FAILURE.
 *
 * ARGUMENTS:
 *
 *	struct FILE *fp - File pointing to the port.
 *	int mode - The mode the controller will be changed to.
 *		    (note: not all are supported)
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

int ei_change_mode(struct FILE *fp, int mode)
{
    struct NI_GBL *np;			/*Pointer to NI global database*/
    struct I82558PB *xppb;		/*Pointer to the port block*/
    char var[EV_NAME_LEN];
    char *pstr = NULL;
    int status = -1;
    unsigned int scb_status;

/*Get the pointer to the port block*/

    np = (struct NI_GBL *) fp->ip->misc;
    xppb = (struct I82558PB *) np->pbp;

/* Prepare the ev name */

    sprintf(var, EI_MODE_FMT_STR, fp->ip->name);

/*Change the mode*/

    switch (mode) {			/* loopback modes */

	case NDL$K_NORMAL_OM: 
	    pstr = "Normal Operating Mode";
	    break;

	case NDL$K_INT: 
	    pstr = "Internal loopback";
	    break;

	case NDL$K_EXT: 
	    pstr = "External loopback";
	    break;

    }
    if (pstr != NULL) {
	dqprintf("Change to %s.\n", pstr);
    }

    switch (mode) {			/* loopback modes */

	case NDL$K_NORMAL_OM: 
	case NDL$K_INT: 
	case NDL$K_EXT: 

	    /* fetch the latest and greatest scb_status */

	    scb_status = XP_CSR_READ_WORD(xppb, XP_SCB_STATUS);

	    /* check the current CU state since we've got to reset things, */

	    if ((xppb->state != XP_K_STARTED) || 
	      ((scb_status & XP_SCB_CUS_MASK) == XP_SCB_CUS_IDLE) || 
	      ((scb_status & XP_SCB_CUS_MASK) == XP_SCB_CUS_SUSPENDED)) {

		if (mode == NDL$K_NORMAL_OM)
		    xppb->loopback = 0;
		if (mode == NDL$K_INT)
		    xppb->loopback = 1;
		if (mode == NDL$K_EXT)
		    xppb->loopback = 2;

		if (xppb->state == XP_K_STARTED) {
		    xppb->state = XP_K_STOPPED;	/* mark the dev stopped */
		    ei_init_tx(xppb);	/* re-init the command blocks */

		    ei_configure_cmd(fp->ip, xppb);	/* re-config loopback */

		    /* Re-Start the transmit process */

		    ei_exec_cmd(xppb, XP_SCB_CMD_CU_START, xppb->cbl_first);
		    xppb->state = XP_K_STARTED;	/* back up and running */
		}

		status = msg_success;
	    } else {
		status = msg_failure;
	    }

	    break;

    }
    if (status == msg_success) {
	return (msg_success);
    } else if (status == msg_failure) {
	ni_error(xppb->pb.name, "Driver must be stopped to change mode.");
	return (msg_failure);
    }

    switch (mode) {			/* media modes */
	case NDL$K_TP: 
	    pstr = ei_twisted_pair;
	    break;

	case NDL$K_TP_FD: 
	    pstr = ei_twisted_pair_fd;
	    break;

	case NDL$K_AUI: 
	    pstr = ei_aui;
	    break;

	case NDL$K_BNC: 
	    pstr = ei_bnc;
	    break;

	default: 
	    pstr = NULL;
    }
    if (pstr != NULL) {
	ev_write(var, pstr, EV$K_STRING);
    } else {
	ni_error(xppb->pb.name, "Invalid mode");
    }
    return (msg_success);
}

/*+
 * ============================================================================
 * = ei_restart_driver - restarts the driver.                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will restart the driver.
 *
 * FORM OF CALL:
 *
 *	ei_restart_driver (ip); 
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

int ei_restart_driver(struct INODE *ip)
{
    struct NI_GBL *np;			/*Pointer to the NI global database*/
    struct I82558PB *xppb;		/*Pointer to the port block*/
    int ei_media;

    dprintf("ei_restart_driver: %s\n", p_args1(ip->name));

/*Get some pointers*/

    np = (struct NI_GBL *) ip->misc;
    xppb = (struct I82558PB *) np->pbp;

/*If we are already started don't start again*/

    if (xppb->state == XP_K_STARTED)
	return (msg_success);

/*If we are unitialized return a failure*/

    if (xppb->state != XP_K_STOPPED)
	return (msg_failure);

/*Make sure the mode is read*/

    ei_read_mode(xppb);

#if 0
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

#endif

/*Make sure the device is stopped*/

    ei_init_device(xppb);

/*Set the device to the correct media port*/

    ei_init_media(xppb);

/*Enable the interrupts*/

/*Check for polled or not*/

    if (xppb->pb.vector != 0)
	io_enable_interrupts(xppb, xppb->pb.vector);

/*Initialize the tx descriptors*/

    ei_init_tx(xppb);

/*Initialize the rx descriptors*/

    ei_init_rx(ip, xppb);

/*Send the configure command */

    ei_configure_cmd(ip, xppb);

/*Start the receive process*/

/* Start the receive process */

    ei_exec_cmd(xppb, XP_SCB_CMD_RU_START, xppb->rfd_base);

/* Start the transmit process */

    ei_exec_cmd(xppb, XP_SCB_CMD_CU_START, xppb->cbl_base);

/*Set the state flag*/

    xppb->state = XP_K_STARTED;

    return (msg_success);
}

/*+
 * ============================================================================
 * =  ei_restart_ru - Restart the i82558 Receive Unit.                        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine handle the case where the RU goes out of the Ready
 *	state unexpectedly. It does its best to get the RU going with
 *	a list of buffers.
 *
 *	Note: This routine will have unpredictable results if the RFA
 *	has not already been cleaned up by ei_process_rx.
 *
 * FORM OF CALL:                
 *
 *	ei_restart_ru (xppb);    
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	struct I82558PB *xppb - Pointer to the xp port block.
 *
 * SIDE EFFECTS:          
 *
 *      None                    
 *
-*/

void ei_restart_ru(struct I82558PB *xppb)
{
    unsigned short scb_status;

    dprintf("ei_restart_ru\n", p_args0);

    /* if the driver isn't in the started state, return */

    if (xppb->state != XP_K_STARTED)
	return;

    /* fetch the latest and greatest scb_status */

    scb_status = XP_CSR_READ_WORD(xppb, XP_SCB_STATUS);

    /* if the RU is in the ready state, return */

    if ((scb_status & XP_SCB_RUS_MASK) == XP_SCB_RUS_READY)
	return;

    dprintf(" RU is in an unexpected state.\n", p_args0);
#if DEBUG
    ei_display_state(scb_status, 1);
#endif

    /* handle the remaining states */

    /* Ready with no rbd's means we somehow have a free rfd with */

    /* no corresponding RBD. We'll abort the RU, get a not-ready */

    /* interrupt, and return here to clean things up.            */

    if ((scb_status & XP_SCB_RUS_MASK) == XP_SCB_RUS_READY_NORBDS) {
	ei_exec_cmd(xppb, XP_SCB_CMD_RU_ABORT, 0);
	return;
    }

    /* For all others, we're going to start the RU again */

    /* set up the rbd pointer in the first avail rfd */

    xppb->ru_restarts++;
    xppb->rfd_first->rbd_ptr = xppb->rbd_first;

    /* start up the RU */

    ei_exec_cmd(xppb, XP_SCB_CMD_RU_START, xppb->rfd_first);

}

/*+
 * ============================================================================
 * =  ei_restart_cu - Restart the i82558 Command Unit.                        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine handles the case where the CU goes into the idle state
 *	unexpectedly. It follows the same sequence as a normal CU startup
 *	by executing a NOP command to get into the suspended state.
 *
 *	Note: This routine will have unpredictable results if the CU block
 *	list has not been cleaned up by ei_process_tx beforehand.
 *
 * FORM OF CALL:                
 *
 *	ei_restart_cu (xppb);
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	struct I82558PB *xppb - Pointer to the xp port block.
 *
 * SIDE EFFECTS:          
 *
 *      None                    
 *
-*/

void ei_restart_cu(struct I82558PB *xppb)
{
    unsigned short scb_status;

    dprintf("ei_restart_cu\n", p_args0);

    /* if the driver isn't in the started state, return */

    if (xppb->state != XP_K_STARTED)
	return;

    /* fetch the latest and greatest scb_status */

    scb_status = XP_CSR_READ_WORD(xppb, XP_SCB_STATUS);

    /* if the CU is not in the idle state, return */

    if ((scb_status & XP_SCB_CUS_MASK) != XP_SCB_CUS_IDLE)
	return;

    dprintf(" CU is in an unexpected state.\n", p_args0);
    xppb->cu_restarts++;

#if DEBUG
    ei_display_state(scb_status, 1);
#endif

    if (xppb->cb_active) {

    /* there is one or more active commands sitting in the CU list */

    /* start the CU working on the first command */

    } else {

	/* build a NOP command in the first available slot */

	/* Init the first command block so that when the tx process is */

	/* started, a NOP will be executed and the CU will be in the */

	/* suspended state. Then, additional transmits will be able */

	/* to be added in a consistent manner. */

	xppb->cbl_first->cb_command = XP_CB_CMD_NOP | XP_CB_CMD_S;
	xppb->cbl_first->cb_status = 0;
	xppb->cbl_last = xppb->cbl_first;	/* just for safety */
	xppb->cb_active = 1;

    }

/* Start the transmit process */

    ei_exec_cmd(xppb, XP_SCB_CMD_CU_START, xppb->cbl_first);

}

#if STARTSHUT
/*+
 * ============================================================================
 * = ei_config - Find units and report them                                   =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine prints out the adapter name, its physical address
 *	and mode.
 *  
 * FORM OF CALL:
 *  
 *	ei_config(dev, verbose)
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

char *xp_modes[] = {			/* */
  ei_not_used, 				/* 0 */
  ei_auto_sensing, 			/* 1 */
  ei_twisted_pair, 			/* 2 */
  ei_aui, 				/* 3 */
  ei_bnc, 				/* 4 */
  ei_twisted_pair_fd, 			/* 5 */
  ei_fast, 				/* 6 */
  ei_fastfd, 				/* 7 */
  ei_auto_negotiate			/* 8 */
};

int ei_config(struct device *dev, int verbose)
{
    struct I82558PB *pb;

    pb = (void *) dev->devdep.io_device.devspecific;
    if (pb) {
	printf("%-18s %02X-%02X-%02X-%02X-%02X-%02X", pb->ip->name, pb->sa[0],
	  pb->sa[1], pb->sa[2], pb->sa[3], pb->sa[4], pb->sa[5]);
	printf("\t%s\n", xp_modes[pb->mode]);
    }
    return msg_success;
}
#endif

#if STARTSHUT
/*+
 * ============================================================================
 * = ei_setmode - change driver state				      =
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
 *	ei_setmode(mode)
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

int ei_setmode(int mode, struct device *dev, struct FILE *fp, char *info)
{
    struct I82558PB *pb;
    struct PCB *pcb;
    int status;

    pcb = getpcb();

#if DEBUG
    pb = (void *) dev->devdep.io_device.devspecific;
    dprintf("i82558 setmode: pb = %x, mode = %s, pid = %x, %s\n",
      p_args4(pb, ei_modes[mode], pcb->pcb$l_pid, pcb->pcb$b_name));
    if (pb)
	dprintf("  ref = %d, fcount = %d\n", p_args2(pb->pb.ref, pb->fcount));
#endif

    switch (mode) {

	case DDB$K_ASSIGN: 
	    ei_setmode(DDB$K_START, dev, 0, 0);
	    ei_setmode(DDB$K_STOP, dev, 0, 0);
	    break;

	case DDB$K_LOOPBACK_INTERNAL: 
	case DDB$K_LOOPBACK_EXTERNAL: 

	    if (dev->devdep.io_device.state == DDB$K_INTERRUPT) {
		err_printf("i82558 cannot enable loopback - port is in use.\n");
		goto setmode_failure;
	    }

	    if (ei_setmode(DDB$K_START, dev, 0, 0) != msg_success)
		goto setmode_failure;

	    pb = (void *) dev->devdep.io_device.devspecific;

/* set the loopback mode */

	    dev->devdep.io_device.state = DDB$K_INTERRUPT;
	    dprintf("device state = %x\n", p_args1(dev->devdep.io_device.state));
	    dprintf("[1b] ref=%x fcount = %x\n", p_args2(pb->pb.ref, pb->fcount));

	    break;

	case DDB$K_OPEN: 
	case DDB$K_START: 
	case DDB$K_INTERRUPT: 

	    pb = (void *) dev->devdep.io_device.devspecific;
	    if (pb == 0) {
		dprintf("Starting i82558...\n", p_args0);

/* Initialize the class driver */

		ether_shared_adr = ovly_load("ETHER");
		if (!ether_shared_adr)
		    goto setmode_failure;

		net_shared_adr = ovly_load("NET");
		if (!net_shared_adr)
		    goto setmode_failure;
		net_ref_counter++;

		pb = ei_init_port(dev);
		if (!pb)
		    goto setmode_failure;

		status = ei_driver_init(pb);
		if (status != msg_success) {
		    goto setmode_failure;
		}

		krn$_wait(&pb->ei_port_s);
		pb->pb.mode = DDB$K_INTERRUPT;
		dev->devdep.io_device.state = DDB$K_INTERRUPT;
		pb->pb.ref++;
		pb->fcount++;
		dprintf("[3] ref=%d fcount = %d\n", p_args2(pb->pb.ref, pb->fcount));
		krn$_post(&pb->ei_port_s);

	    } else {

		krn$_wait(&pb->ei_port_s);
		pb->pb.ref++;
		dprintf("device state = %x\n", p_args1(dev->devdep.io_device.state));
		if (dev->devdep.io_device.state == DDB$K_INTERRUPT) {
		    pb->fcount++;
		    dprintf("fcount=%d\n", p_args1(pb->fcount));
		}
		dprintf("[4] ref=%d fcount = %d\n", p_args2(pb->pb.ref, pb->fcount));
		krn$_post(&pb->ei_port_s);
	    }
	    break;

	case DDB$K_CLOSE: 
	case DDB$K_STOP: 

	    pb = (void *) dev->devdep.io_device.devspecific;
	    if (!pb) {
		dprintf("i82558 Setmode called without pb setup\n", p_args0);
		goto setmode_failure;
	    }

	    krn$_wait(&pb->ei_port_s);
	    pb->pb.ref--;
	    dprintf("device state = %x\n", p_args1(dev->devdep.io_device.state));
	    if (dev->devdep.io_device.state == DDB$K_INTERRUPT)
		pb->fcount--;
	    dprintf("[5] ref=%d fcount = %d\n", p_args2(pb->pb.ref, pb->fcount));

	    if ((pb->pb.ref == 1) && (pb->fcount == 0) && (dev->devdep.io_device.state == DDB$K_INTERRUPT)) {

		dprintf("Stopping i82558...\n", p_args0);
		dev->devdep.io_device.state = DDB$K_STOP;
		dprintf("device state = %x\n", p_args1(dev->devdep.io_device.state));

		krn$_post(&pb->ei_port_s);

		ei_shutdown(pb);

		dev->devdep.io_device.devspecific = 0;

		net_ref_counter--;
		if (net_ref_counter == 0)
		    ovly_remove("NET");
	    } else {
		krn$_post(&pb->ei_port_s);
	    }
	    break;

	case DDB$K_MULTICAST: 

	    ei_open(fp, info);
	    break;

	case DDB$K_POLLED: 
	default: 
	    err_printf("i82558 Error: setmode - illegal mode.\n");
	    return msg_failure;
	    break;
    }					/* switch */

#if DEBUG
    dprintf("Completed setmode: mode = %s, pid = %x, %s\n",
      p_args3(ei_modes[mode], pcb->pcb$l_pid, pcb->pcb$b_name));
    pb = (void *) dev->devdep.io_device.devspecific;
    if (pb)
	dprintf("  pb = %x, ref = %d, fcount = %d\n", p_args3(pb, pb->pb.ref, pb->fcount));
#endif
    return msg_success;

setmode_failure:
    printf("I82558 setmode failed: pid = %x, %s\n", pcb->pcb$l_pid, pcb->pcb$b_name);
    return msg_failure;
}
#endif

#if STARTSHUT
/*+
 * ============================================================================
 * =  ei_init - i82558 driver init for the MODULAR STARTSHUT driver model.    =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will initalize the port queue list.
 *
 * FORM OF CALL:                
 *
 *	ei_init (dev);    
 *
 * RETURNS:
 *
 *	int Status - msg_success or FAILURE.
 *
 * ARGUMENTS:
 *
 *	struct device *dev - device structure ptr.
 *
 * SIDE EFFECTS:          
 *
 *      None                    
 *
-*/
int ei_init(struct device *dev)
{
/*Init the port queue list*/

    xppblist.flink = &xppblist;
    xppblist.blink = &xppblist;

    return (msg_success);
}
#endif

#if STARTSHUT
/*+
 * ============================================================================
 * =  ei_init_port - Init the i82558 port driver for the STARTSHUT model.     =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will allocate and initialize the port block structure.
 *
 * FORM OF CALL:                
 *
 *	ei_init_port (xppb);    
 *
 * RETURNS:
 *
 *	int Status - msg_success or FAILURE.
 *
 * ARGUMENTS:
 *
 *	struct device *dev - Pointer to the xp device structure.
 *
 * SIDE EFFECTS:          
 *
 *      None                    
 *
-*/

struct I82558PB *ei_init_port(struct device *dev)
{
    int i, j;
    struct I82558PB *pb;
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

    krn$_seminit(&(pb->ei_port_s), 0, "ei_port_s");
    krn$_seminit(&(pb->ei_ready_s), 0, "ei_ready_s");

    pb->pb.ref = 1;			/* Init reference count */
    pb->fcount = 0;			/* Init open file count */

/* Load the base address from the device to pb */

    pb->pb.state = DDB$K_START;
    pb->pb.hose = dev->hose;
    pb->pb.slot = dev->slot;
    pb->pb.bus = dev->bus;
    pb->pb.type = TYPE_PCI;
    pb->pb.function = dev->function;
    pb->pb.channel = dev->channel;
    pb->pb.controller = dev->devdep.io_device.controller;
    pb->pb.config_device = dev;

/*    io_disable_interrupts( pb, pb->pb.vector ); */

    if (pci_get_base_address(dev, 1, &pb->pb.csr) != msg_success)
	return 0;

    pb->lwp = ei_send;

    sprintf(pb->pb.name, "ei%c0.0.%d.%d.%d", pb->pb.controller + 'a',
      pb->pb.channel,
      pb->pb.bus * 1000 + pb->pb.function * 100 + pb->pb.slot, pb->pb.hose);

    sprintf(pb->pb.alias, "EI%c0", pb->pb.controller + 'A');

/* setup ev table */

    sprintf(name, "ei%c0_mode", pb->pb.controller + 'a');
    ei_mode_ev.ev_name = name;

    ei_mode_ev.ev_value = ei_twisted_pair;

/* Create mode ev if it doesn't exist */
/* if it does, setup table & write if they are zero */

    if ((ev_locate(&evlist, ei_mode_ev.ev_name, &evp)) == msg_success)
      {					/* found */
	if (evp->wr_action == 0) {
	    strncpy(value, evp->value.string, 31);
	    value[31] = '\0';
	    i = ovly_call_save("I82558", ei_ev_write_$offset);
	    j = ovly_call_save("I82558", ei_mode_table_$offset);
	    ei_mode_ev.ev_wr_action = i;
	    ei_mode_ev.validate = j;
	    ei_mode_ev.ev_value = value;
	    ev_init_ev(&ei_mode_ev, pb);
	}
    } else {				/* not found */
	i = ovly_call_save("I82558", ei_ev_write_$offset);
	j = ovly_call_save("I82558", ei_mode_table_$offset);
	ei_mode_ev.ev_wr_action = i;
	ei_mode_ev.validate = j;
	ev_init_ev(&ei_mode_ev, pb);
    }

/* Load the vector from device to pb */

    pb->pb.vector = pci_get_vector(dev);

    insert_pb(pb);

    krn$_post(&pb->ei_port_s);		/* Ready to access port */

    return pb;
}
#endif

#if ( STARTSHUT || DRIVERSHUT )
/*+
 * ============================================================================
 * =  ei_shutdown - Shutdown the i82558 port driver (STARTSHUT model).        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will shutdown the driver, reset the device and dealocate
 *	all dynamic resources.
 *
 * FORM OF CALL:                
 *
 *	ei_shutdown (xppb);    
 *
 * RETURNS:
 *
 *	int Status - msg_success or FAILURE.
 *
 * ARGUMENTS:
 *
 *	struct I82558PB *xppb - Pointer to the xp port block.
 *
 * SIDE EFFECTS:          
 *
 *      None                    
 *
-*/

int ei_shutdown(struct I82558PB *pb)
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
    krn$_sleep(5);

/* software reset of chip */

    /* Reset the chip with a selective reset */

    XP_CSR_WRITE_LONG(pb, XP_PORT, XP_PORT_SEL_RST);
    krn$_sleep(15);

    /* Then a software reset */

    XP_CSR_WRITE_LONG(pb, XP_PORT, XP_PORT_SOFTW_RST);
    krn$_sleep(15);

/* Shutdown the datalink: */

    ndl_shutdown(pb->ip);

    pb->pb.state = DDB$K_STOP;

    krn$_bpost(&pb->rx_isr);

    krn$_post(&pb->ei_port_s);

    dprintf("waiting for cmplt_rx_s\n", p_args0);
    krn$_wait(&pb->cmplt_rx_s);

/* remove que if polled mode */

    if (pb->pb.vector == 0)
	remq_lock(&pb->pqp.flink);

    krn$_wait(&pb->ei_port_s);

/* Remove the port block from the queue: */

    pbe = findpbe(pb);

    if (pbe == 0) {
	dprintf("pbe not found for pb:%08x\n", p_args1(pb));
	return (msg_failure);
    } else {
	remq_lock(pbe);

/* remove the Command Blocks */

	free(pb->cbl_base);

/* free the receive buffers */

	for (i = 0; i < ei_rcv_buf_no; i++) {
	    ndl_deallocate_pkt(pb->ip, pb->rbd_base[i].rcv_buf_adr);
	}

/* remove the RFDs */

	free(pb->rfd_base);

/* free the RBDs */

	free(pb->rbd_base);

	for (i = 0; i < ei_xmt_buf_no; i++) {
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

    krn$_semrelease(&pb->ei_port_s);
    krn$_semrelease(&pb->ei_ready_s);
    krn$_semrelease(&pb->cmplt_rx_s);
    krn$_semrelease(&pb->rx_isr);
    krn$_semrelease(&pb->tx_wait);

#if !DRIVERSHUT
    remove_pb(pb);
#endif
    free(pb);

    dprintf("Completed shutdown\n", p_args0);
    return msg_success;
}
#endif

#if ( STARTSHUT || DRIVERSHUT )
/*+
 * ============================================================================
 * =  findpbe - Search the port block list for this device.                   =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will search the port block list for the structure
 *	corresponding to this device.
 *
 * FORM OF CALL:                
 *
 *	ei_findpbe (pb);    
 *
 * RETURNS:
 *
 *	int Status - msg_success or FAILURE.
 *
 * ARGUMENTS:
 *
 *	struct PB *pb - Pointer to the generic port block.
 *
 * SIDE EFFECTS:          
 *
 *      None                    
 *
-*/
struct PBQ *findpbe(struct pb *pb)
{
    struct PBQ *pbe;

    pbe = xppblist.flink;
    while (pbe != &(xppblist)) {
	dprintf("pbe:%08x  pbe->pb:%08x pb:%08x\n", p_args3(pbe, pbe->pb, pb));
	if (pbe->pb == pb)
	    return (pbe);
	pbe = pbe->flink;
    }
    return 0;
}
#endif

#if DEBUG
/*+
 * ============================================================================
 * =  ei_dumppb - dump the i82558 port block info.    			      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will format an i82558 port block.
 *
 * FORM OF CALL:                
 *
 *	ei_dumppb (xppb);    
 *
 * RETURNS:
 *
 *	none
 *
 * ARGUMENTS:
 *
 *	struct I82558PB *xppb - Pointer to the xp port block.
 *
 * SIDE EFFECTS:          
 *
 *      None                    
 *
-*/

void ei_dumppb(struct I82558PB *xppb)
{
    int i;
    struct xp_txcb * p_txcb;

    pprintf("ei pb: %08x\n", xppb);
    pprintf("rx_isr: %08x tx_isr: %08x\n", 
	&(xppb->rx_isr), &(xppb->tx_isr));
    pprintf("cbl_base: %08x, cbl_first: %08x, cbl_last: %08x\n", 
	xppb->cbl_base, xppb->cbl_first, xppb->cbl_last);
    pprintf("cb_active: %x\n", xppb->cb_active);
    pprintf("rfd_base: %08x, rfd_first: %08x, rfd_last: %08x\n", 
	xppb->rfd_base, xppb->rfd_first, xppb->rfd_last);
    pprintf("last_scb_status: %08x, last_index: %08x, last_to_index: %08x\n",
	xppb->last_scb_status, xppb->last_index, xppb->last_to_index);
    pprintf("last_cb_in/out: %08x/%08x, high water: %d\n",
	xppb->last_cb_in, 
	xppb->last_cb_out, 
	xppb->cb_highwater);

    pprintf("command block list:\n");
    p_txcb = xppb->cbl_base;
    for (i = 0; i < ei_xmt_buf_no; i++) {
	pprintf("cb[%d], %08x, index: %d, cmd: %x, status: %x link: %08x\n",
	    i, &(p_txcb[i]),
	    p_txcb[i].index,
	    p_txcb[i].cb_command, p_txcb[i].cb_status,
	    p_txcb[i].cb_link);
    }
}
#endif
