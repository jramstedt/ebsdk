/*
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
 * Abstract:	SCSI port driver for the PCI KZPSA FWD SCSI module.
 *
 *		The KZPSA is a Digital developed PCI FWD SCSI controller.
 *		The KZPSA uses SIMport architecture.
 *
 * Author:	Judith E. Gold
 *
 * Creation Date: 20-May-1994
 *
 * Modifications:
 *
 * 	jeg	06-Sep-1994	Add support for reading/writing ev's into
 *				on-board FEPROM.	
 *
 * 	jrk	09-Nov-1994	Remove sparse space force
 *
 *
 *      dpm	5-Dec-1994	Added windows base support for mikasa/avanti
 *				everywhere the board would access memory the
 *				windows base register is or'ed in. If the 
 *				cpu accesses it the windows base is anded out. 
 *				This is done via the pks_phys macro, to "or" it
 *				in. and the sim_phys macro to "and" it out
 *	
 *                              The pks_interrupt was changed. It uses a flag
 * 				to determine if the interrupt was expected. If
 *				it was, it cleared it on the board and returned
 *				If it was not expected it never cleared it. 
 *				This caused an infinte loop when an unexpected
 *				interrupt occured. It now clears the interrupt
 *				whether or not we were expecting it. 
 *
 *				The timeout was changed from 60000 to 2. As 2
 *				is more realistic. Outcfgl was changed to type 
 *				void as it returns nothing. The interrupts are 
 *				not enabled until a handler has been created.
 *				The BIST is not run except for a SABLE. The APS
 *				systems can't do a pci bus reset.
 *				
 *	dtr	1-jun-1995	Changed the call in pks_poll_int routine to call
 *				sim_poll1 which does not have the half second
 *				delay in it.  It was the cleanest way of
 *				handling polling for interrupts without changing
 *				a bunch of code.
 (
 */
                     
#include "cp$src:platform.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:ev_def.h"
#include "cp$src:msg_def.h"
#include "cp$src:common.h"
#include "cp$src:stddef.h"
#include "cp$src:pb_def.h"
#include "cp$src:sim_pb_def.h"
#include "cp$src:pks_flash_def.h"
#include "cp$src:pks_pb_def.h"
#include "cp$src:simport.h"
#include "cp$src:cam.h"
#include "cp$src:scsi_def.h"
#include "cp$src:sb_def.h"
#include "cp$src:ub_def.h"
#include "cp$src:hwrpb_def.h"
#include "cp$src:eisa.h"
#include "cp$inc:platform_io.h"
#include "cp$src:mem_def.h"
#include "cp$src:probe_io_def.h"
#include "cp$inc:kernel_entry.h"
#include "cp$inc:prototypes.h"

#define prefix(x) set_io_prefix(pb,name,x)
#define malloc(x) dyn$_malloc(x,DYN$K_SYNC|DYN$K_NOOWN)
#define free(x) dyn$_free(x,DYN$K_SYNC|DYN$K_NOOWN)

#if RAWHIDE
#define DRIVERSHUT 1
#endif

#define FLASH_DEBUG 0
#if 0   /* DEBUG */
#define qprintf pprintf 
#define err_printf pprintf 
#endif

#define pks_phys(x) ((int)x|io_get_window_base(pb))
#define msg_timeout 2

/* function prototypes */
unsigned int pks_read_long_csr( struct pks_pb *pb, unsigned __int64 csr);
void pks_write_long_csr( struct pks_pb *pb, unsigned __int64 csr,
		unsigned int value);
void pks_reset( struct pks_pb *pb );
int pks_init_kzpsa(struct pks_pb *pb);
void pks_interrupt(struct pks_pb *pb);
extern struct FILE *el_fp;
extern struct QUEUE pollq;
extern struct SEMAPHORE pbs_lock;
extern struct pb **pbs;
extern unsigned int num_pbs;
extern unsigned int num_pbs_all;
unsigned int CalculateChecksum( unsigned int *AddrPtr, unsigned int Length);
void pks_flash_param_read ( unsigned char *ParamStruct, unsigned int size, 
unsigned char BankOffset, unsigned int *FlashPtr, struct pks_pb *pb);
unsigned char pks_flash_param_write ( unsigned char *ParamStruct, 
       unsigned char BankOffset, unsigned int *FlashStat, struct pks_pb *pb);


extern int scsi_reset;
extern int scsi_poll;
extern int ev_initing;

#if !DRIVERSHUT
#define PKS_TRACE 0
extern unsigned char incfgb (struct pb *pb, unsigned __int64 addr);
extern void outcfgb (struct pb *pb, unsigned __int64 addr, unsigned char data);
extern unsigned short incfgw (struct pb *pb, unsigned __int64 addr);
extern ev_init_ev (struct env_table *et, struct pks_pb *pb);
extern sim_interrupt();
extern sim_proc_int();
extern sim_poll();
#endif

void pks_ev_read_rom();
int pks_ev_init();
int pks_ev_write();

#if DRIVERSHUT
int pks_ev_write_ev();
#define PKS_TRACE 0
extern struct FILE *el_fp;
extern int boot_retry_counter;
int kzpsa_process();
int kzpsa_proc_int();
extern int null_procedure();
extern struct EVNODE evlist;
extern match_controller();
#define DEBUG 0
#if !DEBUG
#define pprintf
#endif
#endif

#if !DRIVERSHUT
struct env_table kzpsa_host_id_ev = {
	0, 7,
	EV$K_NODELETE | EV$K_INTEGER | EV$K_NONVOLATILE,
	0, pks_ev_write, 0, pks_ev_init
};

struct env_table kzpsa_termpwr_ev = {
	0, 1,
	EV$K_NODELETE | EV$K_INTEGER | EV$K_NONVOLATILE,
	0, pks_ev_write, 0, pks_ev_init
};

struct env_table kzpsa_fast_ev = {
	0, 1,
	EV$K_NODELETE | EV$K_INTEGER | EV$K_NONVOLATILE,
	0, pks_ev_write, 0, pks_ev_init
};
#endif


#if DRIVERSHUT
struct env_table kzpsa_host_id_ev = {
    0, 7,
    EV$K_NODELETE | EV$K_INTEGER,
    0, pks_ev_write_ev, 0, pks_ev_init};

struct env_table kzpsa_termpwr_ev = {
    0, 1,
    EV$K_NODELETE | EV$K_INTEGER,
    0, pks_ev_write_ev, 0, pks_ev_init};

struct env_table kzpsa_fast_ev = {
    0, 1,
    EV$K_NODELETE | EV$K_INTEGER,
    0, pks_ev_write_ev, 0, pks_ev_init};

#endif

#if !DRIVERSHUT
/*+
 * ============================================================================
 * = pks_init - initialize port driver                                        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine initializes the port driver data structures and the
 *	port hardware.  Port blocks (PBs) contain low-level, port-specific
 *	information; any routine which accesses hardware must reference the
 *	PB.
 *  
 * FORM OF CALL:
 *  
 *	pks_init( )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 * SIDE EFFECTS:
 *
 *	A port block is allocated and initialized for each instance of port
 *	hardware of the appropriate type.
 *
-*/
int pks_init( )
    {
    int pks_init_pb( );
    trace("pks_init\n",PKS_TRACE);

    find_pb( "pks", sizeof( struct pks_pb ), pks_init_pb );
    return( msg_success );
    }

int pks_init_pb( struct pks_pb *pb )
    {
    int hver[1];
    int fver[2];
    char name[16];
    int pks_int_poll( );
    int pks_poll( );
    int pks_interrupt( );

    trace("pks_init_pb\n",PKS_TRACE);
    log_driver_init( pb );
    pks_init_port( pb );
    pb->reinit = 0;

    /* read the on-board firmware version number */    
    hver[0] = incfgl( pb, 0x40 );
    fver[0] = pks_read_long_csr( pb, (int)pb->flp + 0x10000 );
    fver[1] = pks_read_long_csr( pb, (int)pb->flp + 0x10004 );
    sprintf( pb->pb.pb.version, "%4.4s %8.8s", hver, fver );

    if( pks_init_kzpsa( pb ) == msg_failure)
     {
	qprintf( "can't initialize KZPSA SCSI adapter\n" );
	return msg_failure;
     }

    /* these routines had to be moved from init_port since the host id field */
    /* was not filled in until after initializing the adapter. */
    set_io_name( pb->pb.pb.name, 0, 0, pb->node_id, pb );
    sprintf( pb->pb.pb.info, "SCSI Bus ID %d", pb->node_id );
    sprintf( pb->pb.pb.string, "%-24s   %-8s   %24s  %s",
	    pb->pb.pb.name, pb->pb.pb.alias, pb->pb.pb.info, pb->pb.pb.version );

#if 0
    spinlock( &pb->owner_l );
#endif
    /* set up ev's */
    pks_ev_read_rom(pb);  
    kzpsa_host_id_ev.ev_name = prefix( "_host_id" );
    ev_init_ev( &kzpsa_host_id_ev, pb );
    kzpsa_termpwr_ev.ev_name = prefix( "_termpwr" );
    ev_init_ev( &kzpsa_termpwr_ev, pb );
    kzpsa_fast_ev.ev_name = prefix( "_fast" );
    ev_init_ev( &kzpsa_fast_ev, pb );
#if 0
    spinunlock( &pb->owner_l );
#endif

    if( pb->pb.pb.vector )
	{
	pb->pb.pb.mode = DDB$K_INTERRUPT;
	pb->pb.pb.desired_mode = DDB$K_INTERRUPT;
    	krn$_create( sim_proc_int, 0, 0, 6, -1, 4096,
	    prefix( "_proc" ), "nl", "r", "nl", "w", "nl", "w", pb );
	int_vector_set( pb->pb.pb.vector, pks_interrupt, pb );
	io_enable_interrupts( pb, pb->pb.pb.vector );
	}
    else
	{
	pb->pb.pb.mode = DDB$K_POLLED;
	pb->pb.pb.desired_mode = DDB$K_POLLED;
    	pb->pb.pb.pq.routine = pks_int_poll;
    	pb->pb.pb.pq.param = pb;
	insq_lock( &pb->pb.pb.pq.flink, &pollq );
	}

    if (scsi_reset)
        {
        krn$_wait( &pb->owner_s );
        pks_bus_reset( pb );  
        krn$_post( &pb->owner_s );
        }

    pb->poll_active = 1;
    /* polls nodes for devices */
    krn$_create( pks_poll, 0, 0, 5, 0, 4096,
	    prefix( "_poll" ), "nl", "r", "nl", "w", "nl", "w", pb, 1 );
    }

/*+
 * ============================================================================
 * = pks_init_port - initialize a given port                                  =
 * ============================================================================
 *
 * OVERVIEW:                  
 *  
 *	This routine initializes the port driver data structures and the
 *	port hardware of a specific, given port.
 *  
 * FORM OF CALL:
 *  
 *	pks_init_port( pb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct pks_pb *pb		- port block
 *
 * SIDE EFFECTS:
 *
 *	A port block is allocated and initialized for each instance of port
 *	hardware of the appropriate type.
 *
-*/
void pks_init_port( struct pks_pb *pb )
    {
    int pks_setmode();

    trace("pks_init_port\n",PKS_TRACE);
    pb->pb.pb.setmode = pks_setmode;
    set_io_alias( pb->pb.pb.alias, 0, 0, pb );
    pb->pb.pb.sb = malloc( 16 * sizeof( struct scsi_sb * ) );
    pb->pb.pb.num_sb = 16;
    pb->owner_l.req_ipl = IPL_SYNC;
    pb->rp = (struct sim_regs *)(incfgl( pb, 0x10 ) & ~15);
    pb->flp = (void *)(incfgl( pb, 0x1c ) & ~15);
    pb->pb.sim_regs = pb->rp;

    /* device specific parameters for Set Parameters Command */
    pb->pb.set_params = malloc(sizeof(struct SPO_SetParam));
    pb->pb.set_params->ntLUNs = 0;	/* no target mode (for pass 1 anyway) */
    pb->pb.set_params->flags.enable_counters = 1;  /* counters are supported */
    pb->pb.set_params->int_holdoff = 0;  /* not supported on KZPSA */
    pb->pb.set_params->rp_timer = 20;    /* arbitrary number for now */

    pb->pb.set_params->host_sg_BSD.BufPtrLo.BP = 0; /* BSD not used by KZPSA */
    pb->pb.set_params->host_sg_BSD.ByteCount = 0;

    /* so the simport layer knows where to find these */

    pb->pb.reset = pks_init_kzpsa;
    pb->pb.interrupt = pks_interrupt;
    pb->pb.adap_pb = pb;
    pb->pb.owner_s = &pb->owner_s;
    /* init the simport queue lock */
    pb->pb.q_lock.req_ipl = IPL_SYNC;
    /* init the simport rx q semaphore */
    krn$_seminit(&pb->pb.rx_s, 0, "pks_rx_s");
    krn$_seminit( &pb->owner_s, 1, "pks_owner" );
    pks_reset( pb );  
    }
#endif

/*+
 * ============================================================================
 * = pks_init_kzpsa - initialize the KZPSA hardware                             =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine initializes the KZPSA hardware.  It walks through the
 *	necessary steps to bring a SCSI port online.
 *  
 * FORM OF CALL:
 *    
 *	pks_init_kzpsa( )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	none
 *                                                       
 * SIDE EFFECTS:
 *
 *	The KZPSA is now ready for operation.
 *                  
-*/
int pks_init_kzpsa(struct pks_pb *pb)
    {
	unsigned char status;
	int retries = 0;
        int reset_status;
	int print_flag;		/* only print msg once in while loop */

      trace("pks_init_kzpsa\n",PKS_TRACE);
#if SABLE || RAWHIDE
      if(!pb->reinit)    	/* only do this the first time through */
       {
	outcfgb(pb, 0x0f, 0x40);	/* start BIST */
	krn$_sleep(2000);		/* wait for module to finish BIST */

/* read the completion code */

	status = incfgb(pb, 0x0f);
	status &= ~0x80;

	if(status) {
	    err_printf("*** KZPSA BIST failed, %02x error. ***\n", status);
	    return msg_failure;
	 }  /* end if status*/
	pb->reinit = 1;
        }   /* end if !reinit */
#endif
	 /* give it a few tries */
	 print_flag = 1;
	 do
	  {
		if((reset_status = sim_init(pb,0,print_flag)) == NO_ASR_ERRORS)
		 {			
			pks_write_long_csr(pb,(int)&pb->rp->reset_start,1); 
		 }
		print_flag = 0;
	  }
	 while((retries++ < SIM_INIT_RETRIES) && (reset_status != msg_success));

	 /* adapter init failed */
	 if((reset_status == msg_failure) || (retries >= SIM_INIT_RETRIES))
	  {
		return msg_failure;
	  }

	 /* init the channel.  There is only one channel for this device. */
	 if(sim_init(pb,1, 0) == msg_failure)
	  {
		qprintf( "can't initialize KZPSA SCSI channel\n" );
		return msg_failure;
	  }
	/* fill in adapter id, read from channel_state_set response*/
    	 pb->node_id = pb->pb.id;	

	 if(pb->pb.pb.vector)
	  {
		pks_write_long_csr( pb, &pb->rp->amcsr_lo, AMCSR$M_IE );
	  }

	 return msg_success;
	
    }

/*+                         
 * ============================================================================
 * = pks_reset - reset the port hardware                                      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine resets the port hardware.  It initializes it completely
 *	so that further operations may take place.
 *  
 * FORM OF CALL:
 *  
 *	pks_reset( pb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct pks_pb *pb		- port block
 *
 * SIDE EFFECTS:
 *
 *	The port hardware may be initialized and made ready for action.
 *
-*/
void pks_reset( struct pks_pb *pb )
    {
    int i;
    int pks_command( );
    struct scsi_sb *sb;

    /*
     *  Statically allocate an SB for each possible node connected to this
     *  port.  The KZPSA supports up to sixteen attached nodes (the KZPSA 
     *  counts as one but for simplicity we ignore this fact).  Fill in each
     *  SB.
     */
    for( i = 0; i < 16; i++ )
	{
	sb = malloc( sizeof( *sb ) );
	pb->pb.pb.sb[i] = sb;
	sb->pb = pb;
	sb->ub.flink = &sb->ub.flink;
	sb->ub.blink = &sb->ub.flink;
	sb->command = pks_command;
	sb->ok = 1;
	sb->node_id = i;
	if( pb->node_id == i )
	    sb->local = 1;
	set_io_name( sb->name, 0, 0, sb->node_id, pb );
#if DRIVERSHUT
	krn$_seminit(&sb->ready_s, 0, "scsi_sb_ready");
#endif
	}
    }
/*+
 * ============================================================================
 * = pks_command - send a SCSI command to a remote node                       =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine sends a SCSI command to a remote node, allowing 60
 *	seconds for the remote node to complete the command.  Since only
 *	one command may be in progress at a time, the PB owner semaphore
 *	is used as a hardware lock.
 *  
 * FORM OF CALL:
 *  
 *	pks_command( sb, lun, cmd, cmd_len, dat_out, dat_out_len,
 *		dat_in, dat_in_len, sts )
 *  
 * RETURN CODES:
 *
 *	msg_success			- success
 *	msg_failure			- failed to send the command
 *
 * ARGUMENTS:
 *
 *	struct scsi_sb *sb		- system block
 *	int lun				- LUN
 *	unsigned char *cmd		- pointer to Command bytes
 *	int cmd_len			- length of Command bytes
 *	unsigned char *dat_out		- pointer to Data Out bytes
 *	int dat_out_len			- length of Data Out bytes
 *	unsigned char *dat_in		- pointer to Data In bytes
 *	int dat_in_len			- length of Data In bytes
 *	unsigned char *sts		- pointer to Status byte
 *
 * SIDE EFFECTS:
 *
 *	A command may be sent to a SCSI device.
 *
-*/
int pks_command( struct scsi_sb *sb, int lun, unsigned char *cmd, int cmd_len,
	unsigned char *dat_out, int dat_out_len,
	unsigned char *dat_in, int dat_in_len, unsigned char *sts )
    {
    int t;
    int pri;
    struct PCB *pcb;
    struct pks_pb *pb;
    void *ccb;

    trace("pks_command\n",PKS_TRACE);

    /*
     *  Bump up the priority of this thread for the duration.
     */
    pcb = getpcb( );
    pri = pcb->pcb$l_pri;
    pcb->pcb$l_pri = 5;
    /*
     *  Acquire ownership of the port hardware.
     */
    pb = sb->pb;
#if DEBUG
    qprintf( "attempting to send to %s\n",sb->name );  
#endif
    krn$_wait( &pb->owner_s );
    ccb = sim_alloc_ccb( pb );
    /*
     *  Send the command.
     */
    t = pks_do_send( sb, lun, cmd, cmd_len, dat_out, dat_out_len,
	    dat_in, dat_in_len, sts, ccb );
    if( t == msg_timeout )
	{
	pks_bus_reset( pb );
	t = pks_do_send( sb, lun, cmd, cmd_len, dat_out, dat_out_len,
		dat_in, dat_in_len, sts, ccb );
	if( t == msg_timeout )
	    {
	    if( pks_init_kzpsa( pb ) == msg_failure )
		qprintf( "can't initialize KZPSA SCSI adapter\n" );
	    else
		pks_bus_reset( pb );
	    }
	}
    else
	sim_free( ccb );
    /*
     *  Release ownership of the port hardware, and return.
     */
    krn$_post( &pb->owner_s );

    if( ( t != msg_success ) && ( sb->lun_map & ( 1 << lun ) ) )
	qprintf( "failed to send to %s\n", sb->name ); 
    /*
     *  Restore the old priority.
     */
    pcb->pcb$l_pri = pri;
    return( t );
    }
/*+
 * ============================================================================
 * = pks_do_send - build the CCB to send a SCSI command                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *  
 * FORM OF CALL:
 *  
 *	pks_do_send( sb, lun, cmd, cmd_len, dat_out, dat_out_len,
 *		dat_in, dat_in_len, sts, cmd_ctrl_blk )  
 *  
 * RETURN CODES:
 *
 *	msg_success			- success
 *	msg_failure			- failed to send the command
 *
 * ARGUMENTS:
 *
 *	struct scsi_sb *sb		- system block
 *	int lun				- LUN
 *	unsigned char *cmd		- pointer to Command bytes
 *	int cmd_len			- length of Command bytes
 *	unsigned char *dat_out		- pointer to Data Out bytes
 *	int dat_out_len			- length of Data Out bytes
 *	unsigned char *dat_in		- pointer to Data In bytes
 *	int dat_in_len			- length of Data In bytes
 *	unsigned char *sts		- pointer to Status byte
 *	void *cmd_ctrl_blk		- pointer to command control block
 *
 * SIDE EFFECTS:
 *
 *	A command may be sent to a SCSI device.
 *
-*/
int pks_do_send( struct scsi_sb *sb, int lun, unsigned char *cmd, int cmd_len,
	unsigned char *dat_out, int dat_out_len,
	unsigned char *dat_in, int dat_in_len, unsigned char *sts,
	void *cmd_ctrl_blk )
{
  struct pks_pb *pb;
  struct CAMHeader *camh;	
  struct EXECUTE_SCSI_IO *ccb;
  struct PrivateDataArea *priv;

  trace("pks_do_send\n",PKS_TRACE);

  pb = sb->pb;

  /* init pointers */
  camh = (struct CAMHeader *)cmd_ctrl_blk;

  ccb = (struct EXECUTE_SCSI_IO *)((unsigned int)cmd_ctrl_blk + 
			sizeof(struct CAMHeader));

  priv = (struct  PrivateDataArea *)((unsigned int)ccb + 
		sizeof(struct EXECUTE_SCSI_IO));

  /* fill in command specific fields of CAM header */
  camh->function = XPT_EXECUTE_SCSI_IO;
  camh->CCB_len = sizeof(struct CAMHeader) + 
			sizeof(struct EXECUTE_SCSI_IO) + SIM_PRIV;
  camh->conn_id.channel = 0; /* was pb->pb.pb.controller - 1 port number */
  camh->conn_id.target = sb->node_id;    	      /* target id */
  camh->conn_id.lun = lun; 	  /* lun */
  camh->cam_flags.dis_autos = 1;  /* disable autosensing */
  camh->cam_flags.simq_fz_dis = 1;/* disable sim q freeze */

  /* fill in the CCB */
  ccb->cdb_len = cmd_len;		   
  memcpy(ccb->cdb,cmd,cmd_len);	/* fill in the cmd bytes */
  ccb->timeout = 30;
  camh->cam_flags.dir = DIR_NO_XFER;	/* set this as the default */
					/* overwrite if necessary below */
  if(dat_in_len)
   {
	/* length of data buffer */
	priv->start_data_BSD.ByteCount = ccb->xfer_len = dat_in_len;    
	camh->cam_flags.dir = DIR_IN;

	/* pointer to data buffer */

	priv->start_data_BSD.BufPtrLo.b.BPBits = (u_int)pks_phys(dat_in);
	priv->start_data_BSD.BufPtrHi = (u_int)pks_phys(dat_in) >> 30;

	/* for now assume data read is 64 K or less. */
	/* So we set the second data buffer explictly to 0. */

	priv->end_data_BSD.BufPtrLo.BP = 0;
	priv->end_data_BSD.ByteCount = 0;
   }	
  if(dat_out_len)
   {
	/* length of data buffer */
	priv->start_data_BSD.ByteCount = ccb->xfer_len = dat_out_len;    
	camh->cam_flags.dir = DIR_OUT;

	/* pointer to data buffer */

	priv->start_data_BSD.BufPtrLo.b.BPBits = (u_int)pks_phys(dat_out);
	priv->start_data_BSD.BufPtrHi = (u_int)pks_phys(dat_out) >> 30;

	/* for now assume data written is 64 K or less. */
	/* So we set the second data buffer explictly to 0. */

	priv->end_data_BSD.BufPtrLo.BP = 0;
	priv->end_data_BSD.ByteCount = 0;
   }	

/* prepare for the interrupt */
  spinlock( &pb->owner_l );
  pb->int_pending = 1;
  pb->pb.asr = 0;
  pb->pb.afar_lo = 0;
  pb->pb.afar_hi = 0;
  pb->pb.afpr = 0;
  spinunlock( &pb->owner_l );

  if(sim_execute_scsi_io(&pb->pb,camh) == msg_failure)
	return msg_timeout;

  switch(camh->status & ERR_MASK)
   {
	case REQ_DONE_NO_ERR:
	case REQ_DONE_ERR:
		sts[0] = priv->scsi_stat;
		return msg_success;

	case SCSI_BUS_RESET:
		sts[0] = scsi_k_sts_busy;
		return msg_success;

	case TGT_SEL_TO:
		return msg_failure;

	default:
		qprintf("error on %s, cmd = %x, sts = %x, camh->status = %x\n", sb->name, cmd[0], sts[0], camh->status);
		return msg_failure;
   }

}


/*+                      
 * ============================================================================
 * = pks_int_poll - kzpsa polling routine, checks for interrupts            =
 * ============================================================================
 *               
 * OVERVIEW:                                                                  
 *  
 *      This routine is the polling routine for the pks driver.
 *	It is called from the timer task. It will simply check for
 *	an interrupt and call the isr if there is one.
 *           
 * FORM OF CALL:                                                 
 *  
 *  	pks_int_poll (pb);
 *              
 *                    
 * ARGUMENTS:                           
 *             
 *  	struct pks_pb - pointer to pks port block.
 *                          
-*/             
void pks_int_poll (struct pks_pb *pb)
{
    trace("pks_int_poll\n",PKS_TRACE);
    sim_poll1(pb);
}

#if !DRIVERSHUT
/*+
 * ============================================================================
 * = pks_poll - poll to see which nodes exist                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine performs polling, in order to get an accurate picture
 *	of which nodes exist and which don't.  Polling periodically is a way
 *	of detecting nodes coming and going.  This routine is careful never to
 *	poll the port's own node number.
 *  
 * FORM OF CALL:
 *  
 *	pks_poll( pb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct pks_pb *pb		- port block
 *
 * SIDE EFFECTS:
 *
 *	Nodes may be found.  Nodes may be lost.
 *
-*/
void pks_poll( struct pks_pb *pb, int poll )
    {
    char name[16];
    int i;

    /*
     *  Poll all sixteen nodes as a group, pause for a while and repeat forever.
     */
    for( i = 0; i < 16; i++ )   
	{
	if( pb->pb.pb.mode == DDB$K_STOP )
	    continue;
	if( i == pb->node_id )
	    continue;
	if( poll || scsi_poll )
	    scsi_send_inquiry( pb->pb.pb.sb[i] );
	}
    pb->poll_active = 0;
    krn$_create_delayed( 30000, 0, pks_poll, 0, 0, 5, 0, 4096,
	    prefix( "_poll" ), pb, 0 );
    }

/*+
 * ============================================================================
 * = pks_setmode - stop or start the port driver                              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine deals with requests to stop or start the port driver.
 *  
 * FORM OF CALL:
 *  
 *	pks_setmode( pb, mode )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct pks_pb *pb		- port block
 *	int mode			- the new mode
 *
 * SIDE EFFECTS:
 *
 *	The driver may be stopped or started.
 *
-*/
void pks_setmode( struct pks_pb *pb, int mode )
    {
    unsigned __int64 amcsr;
    unsigned __int64 asr;

    trace("pks_setmode\n",PKS_TRACE);

    switch( mode )
 	{
 	case DDB$K_STOP:
	    if( pb->pb.pb.mode != DDB$K_STOP )
		{
		krn$_wait( &pb->owner_s );
		sim_reset( pb );
		sim_cleanup( pb );
		if( pb->pb.pb.mode == DDB$K_INTERRUPT )
		{
		    io_disable_interrupts( pb, pb->pb.pb.vector );
		}
		else
		{
		    remq_lock( &pb->pb.pb.pq.flink );
		}
		pb->pb.pb.mode = DDB$K_STOP;
		}
	    break;	    
 
 	case DDB$K_START:
 	    if( pb->pb.pb.mode == DDB$K_STOP )
 		{
 		pks_init_kzpsa( pb );
                if (scsi_reset)
		    pks_bus_reset( pb );
		pb->pb.pb.mode = pb->pb.pb.desired_mode;
		if( pb->pb.pb.desired_mode == DDB$K_INTERRUPT )
		    io_enable_interrupts( pb, pb->pb.pb.vector );
		else
		    insq_lock( &pb->pb.pb.pq.flink, &pollq );
#if 0
		pb->poll_active = 1;
#endif
		krn$_post( &pb->owner_s );
		}
 	    break;

	case DDB$K_READY:
	    if( pb->pb.pb.mode != DDB$K_STOP )
		{
		while( pb->poll_active )
		    krn$_sleep( 100 );
		}
	    break;

#if 0
 	case DDB$K_POLLED:
 	    if( pb->pb.pb.mode == DDB$K_STOP )
 		{
		pb->pb.pb.mode = DDB$K_POLLED;
		insq_lock( &pb->pb.pb.pq.flink, &pollq );
 		pks_init_kzpsa( pb );
                if (scsi_reset)
		    pks_bus_reset( pb );
		krn$_post( &pb->owner_s );
 		}
 	    break;
#endif
 	}
    }

#endif


/*+
 * ============================================================================
 * = pks_interrupt - service a port interrupt                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine services port interrupts.  It posts the ISR semaphore,
 *	and then acknowledges the port's interrupt, allowing the port to
 *	interrupt again as needed.
 *  
 * FORM OF CALL:
 *  
 *	pks_interrupt( pb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct pks_pb *pb		- port block
 *
 * SIDE EFFECTS:
 *
 *	Waiting tasks may be awoken.
 *
-*/
void pks_interrupt( struct pks_pb *pb )
 {
    unsigned int asr;
    unsigned int afar_lo;
    unsigned int afar_hi;
    unsigned int afpr;

    spinlock( &pb->owner_l );

    asr = pks_read_long_csr(pb,(int)&pb->rp->asr_lo);
    afar_lo = pks_read_long_csr(pb,(int)&pb->rp->afar_lo);
    afar_hi = pks_read_long_csr(pb,(int)&pb->rp->afar_hi);
    afpr = pks_read_long_csr(pb,(int)&pb->rp->afpr_lo);

    if(asr)
     {
	pb->pb.asr |= asr;
	pb->pb.afar_lo = afar_lo;
	pb->pb.afar_hi = afar_hi;
	pb->pb.afpr = afpr;
    	if(pb->int_pending)
     	 {            
		krn$_post(&pb->pb.rx_s);
		pb->int_pending = 0; 
	 }
	pks_write_long_csr(pb,(int)&pb->rp->clrint,1);
     }		       
    spinunlock( &pb->owner_l );
    if(pb->pb.pb.vector)
	io_issue_eoi(pb,pb->pb.pb.vector);
 }


/***********************************************************************/
/* routines to read/write the segment in on-board FEROM which contains */
/* parameters used as console environment variables.                   */
/***********************************************************************/


/*+
 * ============================================================================
 * = pks_ev_init - ev init action routine for pks ev's                        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	
 *  
 * FORM OF CALL:
 *  
 *	pks_ev_init( )
 *  
 * RETURNS:
 *
 *	the value of the parameter as read from the pb
 *       
 * ARGUMENTS:
 *
 *	char *name - ev name
 *	struct EVNODE *ev - pointer to ev struct
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/
int pks_ev_init(struct env_table *et, int value, struct pks_pb *pb)
 {
	char c;
	int t;

	c = et->ev_name[5];
	if(c == 'h')
      	 {
		t = pb->HBAConfig.Chan.SCSIID;
		if(t > 9)
			t += 6;
	 }

	if(c == 't')
      	 {
		t = pb->HBAConfig.Chan.u.ChanParams.TermPwr;
	 }

	if(c == 'f')
      	 {
		t = pb->HBAConfig.Chan.u.ChanParams.Fast;
	 }
	return(t);			
 }


#if DRIVERSHUT
int pks_ev_write_ev( char *name, struct EVNODE *ev ) {
    struct pb *gpb = 0;
    int status = msg_failure;
    int c[4];

    if ( !ev_initing ) {
	c[0] = name[2]; c[1] = 0;	
	find_pb( "pks", 0, match_controller, &gpb, controller_id_to_num( &c[0] ));
	if ( gpb ) {
	    pks_setmode( DDB$K_START, gpb );
	    ev->misc = gpb->dpb;
	    status = pks_ev_write( name, ev );
	    pks_setmode( DDB$K_STOP, gpb );
	    ev->misc = 0;
	}
     }
    return ( status );
}    
#endif

/*+
 * ============================================================================
 * = pks_ev_write - write action routine for pks ev's                        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Writes the value of the ev specified in name to on-board flash rom.
 *  
 * FORM OF CALL:
 *  
 *	pks_ev_write( )
 *  
 * RETURNS:
 *
 *	None.
 *       
 * ARGUMENTS:
 *
 *	char *name - ev name
 *	struct EVNODE *ev - pointer to ev struct
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/
int pks_ev_write(char *name, struct EVNODE *ev)
 {
	char c;
	unsigned int id;
	int flash_stat;
	int write_rom = 0;
	struct pks_pb *pb;

	if(ev_initing)
	    return(msg_success);
	pb = ev->misc;
	c = name[5];

	if(c == 'h')
      	 {
		id = ev->value.integer;
		if(id != pb->HBAConfig.Chan.SCSIID)
		 {
			if(id > 15)
				id -= 6;
			if(id > 15)
			 {
				ev->value.integer = pb->HBAConfig.Chan.SCSIID;
				err_printf(msg_ev_badvalue,name);
				return(msg_failure);
			 }
			pb->HBAConfig.Chan.SCSIID = id;
			if(id > 9)
				ev->value.integer = id + 6;
			else
				ev->value.integer = id;
			write_rom = 1;
		 }
         }

	if(c == 't')
      	 {
		if(ev->value.integer != pb->HBAConfig.Chan.u.ChanParams.TermPwr)
		 {
			pb->HBAConfig.Chan.u.ChanParams.TermPwr = ev->value.integer & 1;
			write_rom = 1;
		 }

         }

	if(c == 'f')
      	 {
		if(ev->value.integer != pb->HBAConfig.Chan.u.ChanParams.Fast)
		 {
			pb->HBAConfig.Chan.u.ChanParams.Fast = ev->value.integer & 1;
			write_rom = 1;
		 }
 	 }

	/* write the flash rom if necessary */
	if(write_rom)
	 {
		
	        /* recalculate checksum */
                pb->HBAConfig.Header.Checksum = CalculateChecksum (
                 (unsigned int*)&(pb->HBAConfig.Header.ClassType),
                 ((PSDL_LENGTH_4 - sizeof(unsigned int)) / sizeof(unsigned int)));
		/* write the flash */
		if(pks_flash_param_write(&pb->HBAConfig,FLASH_BANK_V_CNFG,
					&flash_stat,pb) != msg_success)
	  		pprintf("pks on-board flash not written\n");
			return(msg_failure);
	 }

	return(msg_success);
 }

/*+
 * ============================================================================
 * = pks_ev_read_rom - read ev's in ROM  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine reads the contents of the on-board ROM to determine the
 *	values of the environmental variables.  If the ROM is unreadable,
 *	default values are used.  The values are stored in the pb.
 *  
 * FORM OF CALL:
 *  
 *	pks_ev_read_rom( )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct pks_pb *pb - pks port block
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/
void pks_ev_read_rom(struct pks_pb *pb)
 {
	unsigned int *flash_address;
	int i;
	int flash_stat;

	
	flash_address = incfgl( pb, 0x1c ) & ~15;
    	pks_flash_param_read((unsigned char*)&pb->HBAConfig, 
			sizeof(PSDLClassType4), FLASH_BANK_V_CNFG, 
			flash_address, pb);
#if FLASH_DEBUG
	pprintf("pks flash contents from pb:\n");
	for(i=0;i<sizeof(PSDLClassType4);i=i+4)
		pprintf("%08x\n",*(unsigned int *)(
			(unsigned char *)&pb->HBAConfig.Header + i));
#endif

    /*
    ** If FLASH is not intact, use the configuration defaults.
    */

#if FLASH_DEBUG 
	pprintf("pb->HBAConfig.Header.Checksum = %08x\n",
		pb->HBAConfig.Header.Checksum);
#endif
    	if (pb->HBAConfig.Header.Checksum != CalculateChecksum(
	  (unsigned int*)&(pb->HBAConfig.Header.ClassType),
	  ((PSDL_LENGTH_4 - sizeof(unsigned int)) / sizeof(unsigned int ))))   
    	 {                                                    
#if FLASH_DEBUG 
	    pprintf("CalculateChecksum = %08x\n",
		CalculateChecksum(
	  (unsigned int*)&(pb->HBAConfig.Header.ClassType),
	  ((PSDL_LENGTH_4 - sizeof(unsigned int)) / sizeof(unsigned int ))));
#endif
            pb->HBAConfig.Header.ClassType      = DEFAULT_CLASSTYPE;
            pb->HBAConfig.Header.SubClassVersion= DEFAULT_SUB_CLASS_VERSION;
            pb->HBAConfig.Header.length         = DEFAULT_LENGTH;
            pb->HBAConfig.Chan.SCSIID           = DEFAULT_SCSIID;
            pb->HBAConfig.Chan.stime0           = DEFAULT_STIME0;
            pb->HBAConfig.Chan.CacheBurstDisable= DEFAULT_CACHE_BURST_DISABLE;
            pb->HBAConfig.Chan.BurstLength      = DEFAULT_BURST_LENGTH;
	    pb->HBAConfig.Chan.u.ChanParams.BusReset = DEFAULT_BUSRESET;
	    pb->HBAConfig.Chan.u.ChanParams.TermPwr = DEFAULT_TERMPWR;
	    pb->HBAConfig.Chan.u.ChanParams.Fast = DEFAULT_FAST;
            pb->HBAConfig.KeepAlive             = DEFAULT_KEEPALIVE;
            pb->HBAConfig.CmdTO                 = DEFAULT_CMDTO;
            pb->HBAConfig.AbortSelTORetries     = DEFAULT_ABORT_SEL_TO_RETRIES;
            pb->HBAConfig.DMAEntriesPerQPS      = DEFAULT_DMA_ENTRIES_PER_QPS;
            pb->HBAConfig.NumFQE                = DEFAULT_NUM_FQE;
            pb->HBAConfig.NumFreeQ              = DEFAULT_NUM_FREEQ;
            pb->HBAConfig.NumIPS                = DEFAULT_NUM_IPS;
            pb->HBAConfig.rsvd                  = DEFAULT_RSVD;
            pb->HBAConfig.AbortPhaseCount       = DEFAULT_ABORT_PHASE_COUNT;
            pb->HBAConfig.PRCBACIM              = DEFAULT_PRCB_ACIM;
            pb->HBAConfig.PRCBFCW               = DEFAULT_PRCB_FCW;
            pb->HBAConfig.PRCBICCW              = DEFAULT_PRCB_ICCW;
            pb->HBAConfig.PRCBRCCW              = DEFAULT_PRCB_RCCW;
            
            /*  fill in the per channel default device config params */
            for (i = 0; i < 16; i++)
                pb->HBAConfig.DevCnfg[i] = DEFAULT_DEVICE_CNFG;
    
            pb->HBAConfig.Header.Checksum = CalculateChecksum (
             (unsigned int*)&(pb->HBAConfig.Header.ClassType),
             ((PSDL_LENGTH_4 - sizeof(unsigned int)) / sizeof(unsigned int)));
	    /* try to write the flash anyway */
	    if(pks_flash_param_write(&pb->HBAConfig,FLASH_BANK_V_CNFG,
					&flash_stat,pb) != msg_success)
		pprintf("pks on-board flash not written\n");
    	 }
#if FLASH_DEBUG
	pprintf("pks flash contents from pb:\n");
	for(i=0;i<sizeof(PSDLClassType4);i=i+4)
		pprintf("%08x\n",*(unsigned int *)(
			(unsigned char *)&pb->HBAConfig.Header + i));
#endif
 }


/*+
 * ============================================================================
 * = pks_flash_write - write KZPSA flash ROM                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Write the KZPSA flash and return status.
 *  
 * FORM OF CALL:
 *  
 *	pks_flash_write( )
 *  
 * RETURNS:
 *
 *	msg_success - flash successfully written
 *	msg_failure - flash not successfully written
 *       
 * ARGUMENTS:
 *   	
 *	volatile unsigned int *FAddr - flash address 
 *	unsigned int FData - data to write
 *	unsigned int *FStat - 
 *	unsigned int mask -
 *	struct pks_pb *pb - pointer to pks port block
 *
 * SIDE EFFECTS:
 *
 *	KZPSA Flash ROM may be written
 *
-*/
unsigned char pks_flash_write (volatile unsigned int* FAddr,unsigned int FData, 
                  unsigned int* FStat, unsigned int mask, struct pks_pb *pb)
{
    register unsigned int FlashStat ;
    register volatile unsigned int* FBase = 
	 	(unsigned int *)((unsigned int)FAddr & FLASH_ADDR_MASK) ;
    register unsigned char status = msg_success ;
    register int Retries = MAX_FLASH_RETRY ;
    register unsigned int Timer;
    unsigned int temp;
    int i,j=8;

    do
    {
        /*
        **  NOTE - mask is ANDed with constants by definition for PZA
        */

        pks_write_long_csr(pb,(unsigned __int64)FAddr, FLASH_SETUP_PROGRAM);
        pks_write_long_csr(pb,(unsigned __int64)FAddr, FData & mask);
        pks_write_long_csr(pb,(unsigned __int64)FBase, FLASH_READ_STATUS);

        Timer = MAX_FLASH_BYTE_PROGRAM ; /* should be 5 seconds */
        do
        {
            FlashStat = pks_read_long_csr(pb, (unsigned __int64)FBase);
        } while ((FLASH_OP_CMP != (FlashStat & FLASH_OP_CMP)) && (--Timer));

        if ((Timer==0) || (FlashStat & (FLASH_VPP_ERROR + FLASH_PROG_ERROR)))
        {
                pks_write_long_csr(pb,FStat,FlashStat);
                status = msg_failure ;
                --Retries;
        }
        else
        {
            status = msg_success ;
        };

        pks_write_long_csr(pb,(unsigned __int64)FBase, FLASH_CLEAR_STATUS);
    } while ((status == msg_failure) && (Retries)) ;

    pks_write_long_csr(pb,(unsigned __int64)FBase, FLASH_READ_ARRAY);
    return(status) ;
};

/*+
 * ============================================================================
 * = pks_flash_erase - erase a portion of the KZPSA flash ROM                 =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Erase the KZPSA flash and return status.
 *  
 * FORM OF CALL:
 *  
 *	pks_flash_erase( )
 *  
 * RETURNS:
 *
 *	msg_success - flash successfully written
 *	msg_failure - flash not successfully written
 *       
 * ARGUMENTS:
 *   	
 *	volatile unsigned int *FAddr - flash address
 *	unsigned int *FStat - 
 *	unsigned int mask -
 *	struct pks_pb *pb - pointer to pks port block
 *
 * SIDE EFFECTS:
 *
 *	KZPSA Flash ROM may be erased
 *
-*/
unsigned char pks_flash_erase (volatile unsigned int* FAddr, 
		unsigned int* FStat, unsigned int mask, struct pks_pb *pb)
{
    register unsigned int FlashStat ;
    register unsigned char status = msg_success ;
    register Retries = MAX_FLASH_RETRY ;
    register unsigned int Timer;
    unsigned int temp;	
    int i,j=8;

    do
    {
        /*
        **  NOTE - mask is ANDed with constants by definition for PZA
        */

        pks_write_long_csr(pb,(unsigned __int64)FAddr, FLASH_SETUP_ERASE);
        pks_write_long_csr(pb,(unsigned __int64)FAddr, FLASH_ERASE);
        pks_write_long_csr(pb,(unsigned __int64)FAddr, FLASH_READ_STATUS);
        Timer = MAX_FLASH_ERASE ;  /* should be 25 seconds */
        do
        {
            	FlashStat = pks_read_long_csr(pb,(unsigned __int64)FAddr) ;
        } while ( (FLASH_OP_CMP !=(FlashStat & FLASH_OP_CMP)) && (--Timer) );

        if ( (Timer==0) ||
             (FLASH_VPP_ERROR == (FlashStat & FLASH_VPP_ERROR)) ||
             (FLASH_CMD_SEQ_ERROR == (FlashStat & FLASH_CMD_SEQ_ERROR)) ||
             (FLASH_ERASE_ERROR == (FlashStat & FLASH_ERASE_ERROR)) )
        {
                *FStat = FlashStat;
                status = msg_failure ;
                --Retries;
        }
        else
        {
            status = msg_success ;
        };

        pks_write_long_csr(pb,(unsigned __int64)FAddr, FLASH_CLEAR_STATUS);
    } while ((status == msg_failure) && (Retries)) ;

    pks_write_long_csr(pb,(unsigned __int64)FAddr, FLASH_READ_ARRAY);
    return(status) ;
};

/*+
 * ============================================================================
 * = pks_flash_param_write - write parameter block to the KZPSA flash ROM    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Erase the KZPSA flash and write the param region of ROM.
 *  
 * FORM OF CALL:
 *  
 *	pks_flash_param_write( )
 *  
 * RETURNS:
 *
 *	msg_success - flash successfully written
 *	msg_failure - flash not successfully written
 *       
 * ARGUMENTS:
 *   	
 *	unsigned char *ParamStruct - pointer to parameter struct in memory
 *	unsigned char BankOffset - byte lane in ROM to write
 *	unsigned int *FlashStat - return status 
 *	struct pks_pb *pb - pointer to pks port block
 *
 * SIDE EFFECTS:
 *
 *	KZPSA Flash ROM parameters may be written
 *
-*/
unsigned char pks_flash_param_write ( unsigned char *ParamStruct, 
                        unsigned char BankOffset, 
                        unsigned int *FlashStat, struct pks_pb *pb)
{
unsigned int mask;
unsigned int data;
unsigned int length;
unsigned int i,j=8,temp;
unsigned char status;
unsigned int *FlashPtr;


	
    /* Set flash ptr to parameter region in flash block 2 */    
    FlashPtr = FLASH_BLOCK_2 + (unsigned char *)pb->flp;

     mask = 0xFF << BankOffset;

    /* unlock the flash first */
    pks_write_long_csr(pb,(unsigned __int64)&(pb->rp->flash_lock),0xC0DEC0DE);	
    /* verify the lock */
    for(i=0;i<5000;i++)  /* delay */
	i = j+i;
    temp = pks_read_long_csr(pb,(unsigned __int64)&(pb->rp->flash_lock));
    if(temp != (unsigned int)0xbeefbeef)
     {
	qprintf("KZPSA flash unlock confirmation timeout...\n");
	qprintf("continuing update anyway.");
     }
     /* Erase the region                                  */
     status = pks_flash_erase(FlashPtr, FlashStat, mask,pb);

     if (msg_success == status)
     {
         /* Write each byte into the region                                   */
         length = ((PSDLHeaderType*)ParamStruct)->length;
#if FLASH_DEBUG
	 pprintf("length = %08x\n",length);
#endif
         for (i=0; i<length; i++)
         {
#if FLASH_DEBUG
 	    pprintf("FlashPtr = %08x\n",FlashPtr);
#endif
            data = (*(ParamStruct++) << BankOffset) & mask;
#if FLASH_DEBUG
	    pprintf("data: %08x\n",data);
#endif
            status = pks_flash_write(FlashPtr, data, FlashStat, mask,pb);
            FlashPtr++;
         }
     }
    /* lock the flash */
    pks_write_long_csr(pb,(unsigned __int64)&(pb->rp->flash_lock),0x0);	
    for(i=0;i<5000;i++)  /* delay */
	i = i+j;
    /* verify the lock */
    if(pks_read_long_csr(pb,(unsigned __int64)&(pb->rp->flash_lock)) != 
		(unsigned int)0xbeefbeef)
     {
	qprintf("KZPSA flash lock confirmation timeout");
     }
     return (status);
}

/*+
 * ============================================================================
 * = pks_flash_param_read - read parameter block from the KZPSA flash ROM     =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	The parameter block is read from KZPSA on-board ROM.
 *  
 * FORM OF CALL:
 *  
 *	pks_flash_param_read( )
 *  
 * RETURNS:
 *
 *	msg_success - flash successfully written
 *	msg_failure - flash not successfully written
 *       
 * ARGUMENTS:
 *   	
 *	unsigned char *ParamStruct - pointer to parameter struct in memory
 *	unsigned int size - number of longwords to read
 *	unsigned char BankOffset - byte lane in ROM to write
 *	unsigned int *FlashPtr - flash address 
 *	struct pks_pb *pb - pointer to pks port block
 *
 * SIDE EFFECTS:
 *
 *	KZPSA Flash ROM parameters may be written
 *
-*/
void pks_flash_param_read ( unsigned char *ParamStruct, unsigned int size, 
	unsigned char BankOffset, unsigned int *FlashPtr, struct pks_pb *pb)
{
unsigned int mask;
unsigned int temp;
unsigned int i;
unsigned char *StartPtr;

    /* Set flash ptr to parameter region in flash block 2 */    
    FlashPtr = FLASH_BLOCK_2 + (unsigned char *)pb->flp;
    
     StartPtr = ParamStruct;
     mask = 0xFF << BankOffset;
     /* Load the structure. */
     for (i=0; i<size; i++)
     {
        temp = pks_read_long_csr(pb,(unsigned __int64)FlashPtr++) & mask;
        *(ParamStruct++) = (unsigned char) (temp >> BankOffset);
     }
}


/*+
 * ============================================================================
 * = CalculateChecksum - on-board ROM checksum calculation                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *  	This routine calculates the checksum using the adapted Rigel-style
 *  	algorithm described in the Product Support Data Logging Standards & 
 * 	Guidelines, revision 1.2.
 *  
 * FORM OF CALL:
 *  
 *	CalculateChecksum( )
 *  
 * RETURNS:
 *
 * 	CheckSum - The checksum calculated for region.
 *       
 * ARGUMENTS:
 * 
 * 	unsigned int *AddrPtr - Pointer to start of region to be checksummed.
 *	unsigned int Length  - The number of longwords in region.
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
unsigned int CalculateChecksum( unsigned int *AddrPtr,
                         unsigned int Length)
{
unsigned int i;
unsigned int CheckSum = 0;
unsigned int CheckSumTemp = 0;

        
        for (i = 0; i < Length; i++) 
        {
            CheckSum += AddrPtr[i];

            if ((CheckSum < CheckSumTemp) || (CheckSum < AddrPtr[i])) 
            { 
                if (!CheckSum++)  
                    CheckSum ++;
            }
    
            CheckSumTemp = CheckSum >> 1;
            if (CheckSum ^ (CheckSumTemp << 1))
            {
                CheckSumTemp += 0x80000000; /* BIT31 */
            }

            CheckSum = CheckSumTemp;
        } /* EndFor */

        /* return the complement of the CheckSum                              */
        return( ~CheckSum);
}  
  

void pks_write_long_csr(struct pks_pb *pb, unsigned __int64 csr, 
		      unsigned int value)
 {
#if DEBUG
	pprintf("pks w %x %x\n", csr, value);
#endif
	outmeml(pb,csr,value);
 }

unsigned int pks_read_long_csr(struct pks_pb *pb, unsigned __int64 csr)
 {
	unsigned __int64 value;

	value = inmeml(pb,csr);
#if DEBUG
	pprintf("pks r %x %x\n", csr, value);
#endif
	return(value);
 }              

void pks_bus_reset( struct pks_pb *pb )
    {
    fprintf( el_fp, "resetting the SCSI bus on %s\n", pb->pb.pb.name );
    sim_reset_scsi_bus( &pb->pb );
    krn$_sleep( 250 );
    }

#if DRIVERSHUT

/*+
 * ============================================================================
 * = kzpsa_setmode - change driver state				      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * 	This routine changes the state of the port driver.  The following
 * 	states are relevant:
 *
 *	    DDB$K_STOP -
 *			Shuts the port driver down.
 *
 *	    DDB$K_START -
 *			Starts up the port driver.
 *
 *	    DDB$K_INTERRUPT -
 *			Same as DDB$K_START.
 *  
 * FORM OF CALL:
 *  
 *	kzpsa_setmode(mode, pb)
 *  
 * RETURNS:
 *
 *	msg_success - success
 *	msg_failure - failure
 *	msg_port_state_running - port running
 *       
 * ARGUMENTS:
 *
 *	int mode    	   - Desired mode for port driver.
 *	struct pb *gpb     - pointer to port block information
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/

int pks_setmode(int mode, struct pb *gpb)
{
    int pid;
    int i, j, k, t;
    struct pks_pb *pb;
    struct scsi_sb *sb;

    switch (mode) {

	case DDB$K_ASSIGN: 
	    pks_setmode(DDB$K_START, gpb);
	    pks_setmode(DDB$K_STOP, gpb);
	    break;

	case DDB$K_START: 
	case DDB$K_INTERRUPT: 

	    if ( gpb->dpb ) {
		gpb->dpb->ref++;
		return msg_success;
	    } else {
		alloc_dpb( gpb, &pb, sizeof( struct pks_pb ));
	    }

/* has device already failed ? */

#if 0
	    if (dev->flags)
		return msg_failure;
#endif

/* initialize the class driver */

	    scsi_shared_adr = ovly_load("SCSI");
	    if (!scsi_shared_adr)
		return msg_failure;

	    simport_shared_adr = ovly_load("SIMPORT");
	    if (!simport_shared_adr)
		return msg_failure;

	    krn$_seminit(&pb->init, 0, "kzpsa_i");
	    krn$_seminit(&pb->shut, 0, "kzpsa_s");
	    krn$_seminit(&pb->process, 0, "kzpsa_process");

	    pb->pb.pb.ref = 1;		/* initial start */

	    krn$_set_console_mode(INTERRUPT_MODE);

/* create process for adapter */

	      pid = krn$_create(	/* */
	      kzpsa_process, 		/* address of process   */
	      null_procedure, 		/* startup routine      */
	      &pb->process, 		/* completion semaphore */
	      6, 			/* process priority     */
	      1 << whoami(), 		/* cpu affinity mask    */
	      0x3000, 			/* stack size           */
	      "kzpsa", 			/* process name         */
	      0, 0, 0, 0, 0, 0,		/* io                   */
	      pb);			/* args                 */

	    krn$_wait(&pb->init);

	    if (pb->status != msg_success) {
		err_printf("KZPSA: setmode - start, failed, status = %m\n",
		  pb->status);
		return pb->status;
	    }

	    break;

	case DDB$K_STOP: 

	    pb = gpb->dpb;

	    if (pb == 0) {
		return msg_failure;
	    }

	    /* if it was already shutdown(diag shutdown), return */
	    if (pb->pb.pb.ref == 0)
		return msg_success;

	    pb->pb.pb.ref--;
	    if (pb->pb.pb.ref != 0)
		return msg_success;

	    kzpsa_shutdown(pb);

	    krn$_post(&pb->shut);

	    krn$_wait(&pb->process);

	    krn$_semrelease(&pb->init);
	    krn$_semrelease(&pb->shut);
	    krn$_semrelease(&pb->process);
	    ovly_remove("SIMPORT");

	    free(pb);

	    gpb->dpb = 0;

	    break;

	case DDB$K_POLLED: 
	default: 
	    err_printf("KZPSA: setmode error - illegal mode\n");
	    return msg_failure;
	    break;
    }					/* switch */
    return msg_success;
}					/* kzpsa_setmode */

/*+
 * ============================================================================
 * = kzpsa_process - initialize the KZPSA				      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine initializes the KZPSA.
 *  
 * FORM OF CALL:
 *  
 *	kzpsa_process( pb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct pks_pb *pb - port block pointer
 *
 * SIDE EFFECTS:
 *
-*/

int kzpsa_process(struct pks_pb *pb )
{
    int status;

    status = kzpsa_init( pb );

    pb->status = status;
    krn$_post(&pb->init);

    if (status != msg_success) {
	return ( status );
    }

/* Wait for shutdown */

    krn$_wait(&pb->shut);

}

/*+
 * ============================================================================
 * = kzpsa_shutdown - Stop the KZPSA port				      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine handles Port shutdown.
 *  
 * FORM OF CALL:
 *  
 *	kzpsa_shutdown_port( pb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct pks_pb *pb - port block
 *
 * SIDE EFFECTS:
 *
 *	All dynamic memory block are freed.
 *
-*/

void kzpsa_shutdown(struct pks_pb *pb)
{
    int i;
    struct scsi_sb *sb;

/* Break all virtual connections */

    for (i = 0; i < 16; i++) {
	sb = pb->pb.pb.sb[i];
	if (sb->state == sb_k_open) {
	    scsi_break_vc(sb);
	}
	krn$_semrelease(&sb->ready_s);
    }

/* stop all processes */

    pb->pb.pb.state = DDB$K_STOP;
    krn$_post(&pb->pb.rx_s);
    krn$_wait(&pb->pb.done);

/* reset the adapter */

    pks_write_long_csr(pb, &pb->rp->reset_start, 1);
    krn$_sleep(1000);

    sim_shutdown(pb);

/* Free all vectors */

    int_vector_clear(pb->pb.pb.vector);

    sim_shutdown(pb);

/* Free all semaphores */

    krn$_release_timer(&pb->isr_t);
    krn$_semrelease(&pb->owner_s);
    krn$_semrelease(&pb->isr_t.sem);
    krn$_semrelease(&pb->pb.rx_s);
    krn$_semrelease(&pb->pb.ready);
    krn$_semrelease(&pb->pb.done);

    free( pb->pb.set_params );
    for( i = 0; i < 16; i++ )
	free( pb->pb.pb.sb[i] );
    free( pb->pb.pb.sb );

}

/*+
 * ============================================================================
 * = kzpsa_poll_units -							      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *  
 * FORM OF CALL:
 *  
 *	kzpsa_poll_units( gpb, verbose)
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct pb *gpb		- Pointer to port block structure of adapter.
 *	int verbose		- if set, print node information.
 *
 * SIDE EFFECTS:
 *
 *	Files will be created.
 *
-*/

int pks_poll_units(struct pb *gpb, int verbose)
{
    int i;
    struct pks_pb *pb;
    struct scsi_sb *sb;

    pb = gpb->dpb;

    if (verbose)
	printf("%s\n", pb->pb.pb.string);

/*  Poll all sixteen nodes. */

    for (i = 0; i < 16; i++) {

	if (i == pb->node_id)
	    continue;

	sb = pb->pb.pb.sb[i];
	sb->verbose = 1;

	scsi_send_inquiry(sb);

	if (killpending())
	    return ( msg_success );
    }
}

/*+
 * ============================================================================
 * = kzpsa_establish_connection - establish a connection to given unit        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine establishes a connection to a given unit.
 *  
 * FORM OF CALL:
 *  
 *	kzpsa_establish_connection( inode, pb, class )
 *  
 * RETURNS:
 *
 *	success or failure.
 *       
 * ARGUMENTS:
 *
 *	struct INODE *inode	- pointer to inode.
 *	struct pb *gpb		- pointer to port block
 *	int class		- class of connection
 *
 * SIDE EFFECTS:
 *
 *	Connection established to given unit.
 *
-*/

int pks_establish_connection(struct INODE *inode, struct pb *gpb, int class)
{
    int unit;
    int node;
    int channel;
    struct scsi_sb *sb;
    struct pks_pb *pb;

    pb = gpb->dpb;

/* if device not in file system, fail */

    if (!inode->inuse) {
	return msg_failure;
    }

/* get device particulars */

    if (explode_devname(inode->name, 0, 0, &unit, &node, &channel, 0, 0 ) !=
      msg_success) {
	boot_retry_counter = 0;
	return msg_failure;
    }

    if ((node >= 0) && (node < 16))
	sb = pb->pb.pb.sb[node];
    else
	return msg_failure;

/* if sb in use, device already configured */

    if (sb->ref != 0)
	return msg_success;

    sb->ref++;

    sb->verbose = 0;

    scsi_send_inquiry(sb);
    return msg_success;
}

int kzpsa_init(struct pks_pb *pb )
{
    int status;
    int hver;
    int fver[2];
    unsigned int flash_offset = 0x10000;

    io_disable_interrupts(pb, pb->pb.pb.vector);

    pprintf("KZPSA h/s/b/f/c/config_device/vec %d/%d/%d/%d/%d/%x/%x\n",
      pb->pb.pb.hose, pb->pb.pb.slot, pb->pb.pb.bus, pb->pb.pb.function,
      pb->pb.pb.controller, pb->pb.pb.config_device, pb->pb.pb.vector);

    pb->pb.pb.csr = pb->rp = (struct sim_regs *) (incfgl(pb, 0x10) & ~15);

    pprintf("init_pb: csr base  = %x\n", pb->pb.pb.csr);

    pb->pb.sim_regs = pb->rp;

    pb->flp = (void *) (incfgl(pb, 0x1c) & ~15);

    pprintf("rp = %08x flp = %08x\n", pb->rp, pb->flp);

/* read the on-board version numbers */

    hver = incfgl(pb, 0x40);

    fver[0] = pks_read_long_csr(pb, (unsigned char *) pb->flp + flash_offset);
    fver[1] = pks_read_long_csr(pb, (unsigned char *) pb->flp + flash_offset + 4);

    sprintf(pb->pb.pb.version, "%4.4s %8.8s", &hver, &fver[0]);

/* device specific parameters for Set Parameters Command */

    pb->pb.set_params = malloc(sizeof(struct SPO_SetParam));
    pb->pb.set_params->ntLUNs = 0;	/* no target mode (for pass 1 anyway)
					 */
    pb->pb.set_params->flags.enable_counters = 1;
					/* counters are supported */
    pb->pb.set_params->int_holdoff = 0;	/* not supported on KZPSA */
    pb->pb.set_params->rp_timer = 20;	/* arbitrary number for now */

    pb->pb.set_params->host_sg_BSD.BufPtrLo.BP = 0; /* BSD not used by KZPSA */
    pb->pb.set_params->host_sg_BSD.ByteCount = 0;

/* init the timer semaphore */

    krn$_seminit(&pb->isr_t.sem, 0, "kzpsa_isr");
    krn$_init_timer(&pb->isr_t);

/* so the simport layer knows where to find these */

    pb->pb.reset = pks_init_kzpsa;
    pb->pb.sem = &pb->isr_t.sem;
    pb->pb.interrupt = pks_interrupt;
    pb->pb.adap_pb = pb;
    pb->pb.owner_s = &pb->owner_s;

/* init the simport queue lock */

    pb->pb.q_lock.req_ipl = IPL_SYNC;

/* init the simport semaphores */

    krn$_seminit(&pb->pb.rx_s, 0, "kzpsa_rx_s");
    krn$_seminit(&pb->pb.ready, 0, "kzpsa_ready_s");
    krn$_seminit(&pb->pb.done, 0, "kzpsa_done_s");

/* init the owner semaphore */

    krn$_seminit(&pb->owner_s, 1, "kzpsa_owner");

    pb->owner_l.req_ipl = IPL_SYNC;

/* initialize the port */

    pb->pb.pb.state = DDB$K_START;

    status = pks_init_port(pb);
    if (status != msg_success)
	return status;

    return msg_success;
}

int pks_init_port(struct pks_pb *pb)
{
    int i, j;
    int pid;
    struct device *dev;
    struct EVNODE *evp;
    char controller[MAX_NAME];
    char name[16];
    int pks_interrupt();

/* bring it on-line */

    if (pks_init_kzpsa(pb) != msg_success) {
	err_printf("KZPSA: can't initialize SCSI adapter\n");
	return msg_failure;
    }

    pb->pb.pb.sb = malloc( 16 * sizeof( struct scsi_sb * ) );
    pb->pb.pb.num_sb = 16;

/* reset the port hardware */

    pks_reset(pb);

/* set up ev's */

    spinlock(&pb->owner_l);

    pks_ev_read_rom(pb);

/* Create host_id ev if it doesn't exist */

    kzpsa_host_id_ev.ev_name = prefix("_host_id");
    if ((ev_locate(&evlist, kzpsa_host_id_ev.ev_name, &evp)) != msg_success) {
	i = ovly_call_save("PKS", pks_ev_write_ev_$offset);
	kzpsa_host_id_ev.ev_wr_action = i;
	ev_init_ev(&kzpsa_host_id_ev, pb);
    }

/* Create termpwr ev if it doesn't exist */

    kzpsa_termpwr_ev.ev_name = prefix("_termpwr");
    if ((ev_locate(&evlist, kzpsa_termpwr_ev.ev_name, &evp)) != msg_success) {
	kzpsa_termpwr_ev.ev_wr_action = i;
	ev_init_ev(&kzpsa_termpwr_ev, pb);
    }

/* Create fast ev if it doesn't exist */

    kzpsa_fast_ev.ev_name = prefix("_fast");
    if ((ev_locate(&evlist, kzpsa_fast_ev.ev_name, &evp)) != msg_success) {
	kzpsa_fast_ev.ev_wr_action = i;
	ev_init_ev(&kzpsa_fast_ev, pb);
    }

    spinunlock(&pb->owner_l);

    controller_num_to_id(pb->pb.pb.controller, controller);

    sprintf(pb->pb.pb.name, "%s%s.%d.%d.%d.%d", pb->pb.pb.protocol, controller,
      pb->node_id, pb->pb.pb.channel, pb->pb.pb.bus * 1000 +
      pb->pb.pb.function * 100 + pb->pb.pb.slot, pb->pb.pb.hose);

    sprintf(pb->pb.pb.alias, "dk%s", controller);

    sprintf(pb->pb.pb.info, "TPwr %d Fast %d Bus ID %d",
      pb->HBAConfig.Chan.u.ChanParams.TermPwr,
      pb->HBAConfig.Chan.u.ChanParams.Fast, pb->node_id);

    sprintf(pb->pb.pb.string, "%-18s %-7s %s  %s", pb->pb.pb.name,
      pb->pb.pb.alias, pb->pb.pb.info, pb->pb.pb.version);

/* enable interrupts */

    pb->pb.pb.mode = DDB$K_INTERRUPT;
    pb->pb.pb.desired_mode = DDB$K_INTERRUPT;

    int_vector_set(pb->pb.pb.vector, pks_interrupt, pb);
    io_enable_interrupts(pb, pb->pb.pb.vector);

/* processes entries from the adrq */

    pid = krn$_create(			/* */
      kzpsa_proc_int, 			/* address of process   */
      null_procedure, 			/* startup routine      */
      &(pb->pb.done), 			/* completion seamphore */
      6, 				/* process priority     */
      1 << whoami(), 			/* cpu affinity mask    */
      0x3000, 				/* stack size           */
      prefix("_proc_int"), 		/* process name         */
      0, 0, 0, 0, 0, 0, 		/* io                   */
      pb);				/* argument             */

    krn$_wait(&pb->pb.ready);

    return msg_success;
}

int kzpsa_proc_int(int *pb)
{
    return sim_proc_int(pb);
}

#endif

