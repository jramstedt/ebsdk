/* file:	de205_driver.c
 *
 * Copyright (C) 1992 by
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
 * FACILITY:
 *
 *      Console Firmware
 *
 * ABSTRACT:	DE205 (LEMAC) Ethernet driver
 *
 *	This driver uses the LEMAC chip in the 
 *	programed I/O mode.  Note that in the programmed I/O mode care 
 *	must be taken that once the page and offset  registers are set no 
 *	other process touches the chip. If not things will be unpredictable
 *	Also care must be used whan dealing with the queues as putting 
 *	bad data on the queues causes strange behavior but no obvious errors
 *	Currently only MOP and BOOTP downloads are tested, 
 *
 *	The receive process handles all the incoming packets. The send 
 *	process doesn't do much. Note that the MOP boot doesn't use the 
 *	normal driver read routines but goes direct to the process.
 *
 * OVERVIEW:
 *
 *	The driver requires that an entry into the isacfg table be entered.
 *	Required values are iobase address, membase address and irq value.
 *	IRQs 5 10 15 (5 a f) are the only valid irqs. The irq and iobase
 *	entered in isacfg MUST match those entered via the ewrk3_config
 *	command. If IRQ0 in the isacfg entry is at the defaule of 80000000,
 *      then polled mode is done for that module. Membase is taken from the
 *      isacfg information and sets it up for a 2k window regardless of the
 *      on board settings. The isacfg string for a DE205 set for the defaults
 *      is:
 *
 * isacfg -slot ? dev 0 -mk -handle DE200-LE -irq0 5 -membase0 d0000 -etyp 1
 *	  -iobase0 300 
 *
 *	EWRK3_CONFIG is a utility that sets up the DE205 on board EEROM. It 
 * 	is similar to the isacfg utility. This utility is required as the 
 * 	only existing method for changing it is a msdos utility. In order 
 *	to use multiple boards it is required that boards be inserted one at 
 *      a time, and at a minimum their IO address be changed.  This is 
 *      required to avoid address conflicts.  The command for changing a 
 *	board from its default address to address 0x340 for example is:
 *	ewrk3_config -curaddr 300 -ioaddr 340
 *	Followed by a power cycle. A power cycle is required for the new
 *	IO address to take affect.
 *
 *	Invoking "ewrk3_config" with no parameters searches the entire
 *	address space for DE205 modules and displays the addresses of all
 *	modules found. 
 *	
 *	The parameters to ewrk3_config are as follows:
 *
 *	<none>   Find all DE205 boards and diplay their IO address
 *	-curaddr The current hex IO address of the module being changed 
 *      -ioaddr  The hex IO address to change the current module to
 *	-irq	 The decimal irq line to be used 5, 10 or 15
 *	-fbus	 Fast bus enable
 *	-ena16	 16 bit bus enable 
 *	-memaddr Memory address the board should respond to
 *	-bufsize Buffer size of the memory address 2K, 32K or 64K
 *	-swflag	 Software flag field
 *	-netmgt  Net management field
 *	-default Set the board back to the default settings
 *	-show    Displays the entire EEROM for the module
 *
 *	See the "EtherWORKS3 Configuration & EEPROM specifcation" for more
 *	information about the EEPROM configuration.
 *
 *  AUTHORS:
 *
 *  CREATION DATE:
 *  
 *
 *  MODIFICATION HISTORY:
 *
 *	dpm	20-jul-1994	Fixed device name to DE200-LN and controller
 *				name to "en" Put a timeout on the write so 
 *				an unconnected controllor doesn't hang
 *
 *
 *	dpm	23-aug-1994	A few changes for callbacks. Added LeMACII 
 *				Parameters in ewrk3_config
 *
 *	sew	10-oct-1995	Fixed de205_restart_driver(): polled mode
 *				functionality so it works.
 *
 *--
 */

/*Include files*/
#include "cp$src:platform.h"
#include "cp$src:kernel_def.h"			/*Kernel definitions*/
#include "cp$src:msg_def.h"			/*Message definitions*/
#include "cp$src:prdef.h"			/*Processor definitions*/
#include "cp$src:common.h"			/*Common definitions*/
#include "cp$inc:prototypes.h"
#include "cp$src:dynamic_def.h"			/*Memory allocation defs*/
#include "cp$src:ni_env.h"			/*NI Environment variables*/
#include "cp$src:ev_def.h"			/*Environment vars*/
#include "cp$src:de205.h"			/*DE205 definitions*/
#include "cp$src:ni_dl_def.h"			/*Datalink definitions*/
#include "cp$src:pb_def.h"			/*Port block definitions*/
#include "cp$src:de205_pb_def.h"		/*DE205 Port block definitions*/
#include "cp$src:mop_counters.h"		/*MOP counters*/
#include "cp$src:eisa.h"			/*Eisa bus definitions*/
#include "cp$inc:platform_io.h"			/*System bus definitions*/
#include "cp$src:isacfg.h"
#include "cp$src:parse_def.h"
#include "cp$src:parser.h"
#include "cp$src:platform_cpu.h"		/*System definitions*/
#include "cp$inc:kernel_entry.h"

#define _addq_v_u(x,y) addq_v_u(&(x),y)

#define malloc(x) dyn$_malloc(x,DYN$K_FLOOD|DYN$K_SYNC|DYN$K_NOOWN)
#define free(x) dyn$_free(x,DYN$K_SYNC|DYN$K_NOOWN)

/*External references*/
extern int en_controllers;		/*Used for controller letters*/
extern int cb_ref;			/*Call backs flag*/
extern int cb_fp[];
extern struct QUEUE pollq;		/*POLLQ			*/
extern int spl_kernel;			/*Kernel spinlock	*/
extern int null_procedure ();
extern int isa_count;

/*External routine definitions*/
extern unsigned char inportb (struct pb *pb, unsigned __int64 addr);
extern void outportb (struct pb *pb, unsigned __int64 addr,unsigned char data);
extern void outmemb (struct pb *pb, unsigned __int64 addr,unsigned char data);
extern unsigned char inmemb (struct pb *pb, unsigned __int64 addr);



/*Routine definitions*/
int de205_callback_write(struct FILE *fp,int item_size,int number,char *buf);
void de205_control_t(struct NI_GBL *np);
int de205_check_nirom(struct DE205_PB *pb,unsigned char *ni);
int de205_change_mode(struct FILE *fp,int mode);
void de205_free_rcv_pkts(struct INODE *ip);
void de205_get_nirom (struct DE205_PB *pb,unsigned char *ni);
int de205_init ();
int de205_init_driver (struct INODE *ip);
int de205_init_I (char *file_name,struct DE205_PB *pb);
int de205_init_tx(struct INODE *ip);
void de205_interrupt (struct NI_GBL *np);
int de205_open (struct FILE*);
int de205_out (struct INODE *ip,char *buf,int len);
void de205_poll (struct NI_GBL *np);
int de205_restart_driver(struct INODE *ip);
int de205_write(struct FILE *fp,int item_size,int number,char *buf);
void de205_msg_rcvd(struct FILE *fp,struct DE205_PB *pb,struct MOP$_V4_COUNTERS *mc,
						char *msg,unsigned short size);
int de205_process_rx(struct DE205_PB *pb,struct MOP$_V4_COUNTERS *mc,
						struct NI_ENV *ev,
						unsigned char **rbuf,
						int	*size);
int de205_read (struct FILE *fp,int size,int number,unsigned char *c);
void de205_read_env (struct NI_ENV *ev,char *name);
void de205_reinit(struct FILE *pf);
int de205_rx(struct INODE *ip);
int de205_setmode (int mode);
void de205_set_env (char *name);
int de205_stop_driver(struct INODE *ip);
void de205_tx(struct INODE *ip);
void de205_eewrite(unsigned char wdata1, unsigned char wdata2, int rindx,
                struct de205_regs *rp);
void de205_get_rom (struct DE205_PB *pb);


/*Globals*/
struct PBQ _align (QUADWORD) de205pb;		/*Port block list*/

int	de205_write  ();
int	de205_open  ();
int	de205_close  ();
int	de205_setmode  ();

int	de205_controllers;

struct DDB de205_ddb = {
	"en",			/* how this routine wants to be called	*/
	de205_read,		/* read routine				*/
	de205_write,		/* write routine			*/
	de205_open,		/* open routine				*/
	null_procedure,		/* close routine			*/
	null_procedure,		/* name expansion routine		*/
	null_procedure,		/* delete routine			*/
	null_procedure,		/* create routine			*/
	de205_setmode,		/* setmode				*/
	null_procedure,		/* validation routine			*/
	0,			/* class specific use			*/
	0,			/* allows information			*/
	0,			/* must be stacked			*/
	0,			/* is a flash update driver		*/
	0,			/* is a block device			*/
	0,			/* not seekable				*/
	1,			/* is an ethernet device		*/
	0,			/* is a filesystem driver		*/
};

#define DSTRING "%xcuraddr %xioaddr %xmemaddr %dirq %xfbus %xena16 %xswflag %xnetmgt0 %dbufsize %xzws %xra %xwb show default"
#define DCURADDR 0
#define DIOADDR 1
#define DMEMADDR 2
#define DIRQ 3
#define DFBUS 4
#define DENA16 5
#define DSWFLAG 6
#define DNETMGT 7
#define DBUFSIZE 8
#define DZWS 9
#define DRA 10
#define DWB 11
#define DSHOW 12
#define DDEFAULT 13
#define DMAX 14

/*+
 * ============================================================================
 * = de205_init - Initialization routine for the port driver                     =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routines will initialize the LEMAC port. find_pb will locate 
 *      all the de205 modules and create the PBs for them. DE205_init_pb is
 *	pased to it an will be called for each device found.  
 *
 * FORM OF CALL:
 *
 *	de205_init (); 
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

int de205_init ()
{
    int de205_init_pb( );

    de205_controllers=0;
/*Init the port queue list*/
    de205pb.flink = &de205pb;
    de205pb.blink = &de205pb;

/* make sure all de205 boards are found */
    find_pb("DE200-LE",sizeof(struct DE205_PB),de205_init_pb);
    return (msg_success);
}



/*+
 * ============================================================================
 * = de205_init_pb - Initialization PB for the port driver                    =
 * ============================================================================
 *
 * OVERVIEW:
 *      The port block gets initialized with the hose, slot ect. the 
 *	enviornment varibles get set as do the  MOP varibles. Initialization
 *	continues in de205_init_I
 *
 *
 * FORM OF CALL:
 *
 *	de205_init_pb (pb); 
 *
 * RETURNS:
 *
 *	int Status - msg_success or FALIURE.
 *
 * ARGUMENTS:
 *
 *      The port block to initalize
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/


de205_init_pb( struct DE205_PB *pb )
{ 
    char c;
    char pname[16];			/*Port name*/

	/*Do the first stage of the driver init*/
        de205_get_rom(pb);
	pb->pb.hose = EISA_HOSE;
	pb->pb.protocol = "en";
        pb->pb.device = malloc(sizeof("en"));
	pb->pb.controller = en_controllers;

	strcpy(pb->pb.device,"er");

        c = en_controllers++ + 'a'; 

	sprintf(pname,"en%c0",c);  
	/*Set environment variables*/
	de205_set_env(pname);
	mop_set_env(pname);                     


	sprintf(pname,"en%c0.0.0.%d.1",c,pb->pb.slot);

	de205_init_I(pname,pb);
        if(pb->pb.mode == DDB$K_INTERRUPT)
	  {
     outportb(pb, ((unsigned __int64)&(pb->rp->de205_b_icr)),pb->irqval|0x80);
          }
        else
          outportb(pb, ((unsigned __int64)&(pb->rp->de205_b_icr)),pb->irqval);

        outportb(pb, ((unsigned __int64)&(pb->rp->de205_b_csr)),0);

}

/*+
 * ============================================================================
 * = de205_init_I - Initialization routine for the LEMAC port driver          =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routines will initialize the LEMAC port driver. This routine
 *	allocates the inode, sets up the driver data base. The NIROM is read 
 *	and checked.
 *	It also calls the higher level initialization routines.
 *	This is called from the generic de205_init_pb routine.
 *
 * FORM OF CALL:
 *
 *	de205_init_I (file_name,pb); 
 *      
 * RETURNS:
 *   
 *	int Status - msg_success or FAILURE.
 *
 * ARGUMENTS:
 *
 *      char *file_name - Filename of the device to be opened.
 *	struct DE205_PB *pb - pointer to port block.
 *
 * SIDE EFFECTS:               
 *      
 *      None
 *   
-*/                            

int de205_init_I (char *file_name,struct DE205_PB *pb)
{
struct NI_GBL *np;		/* Pointer to the NI global database */
struct INODE *ip;		/* Pointer to the INODE */
struct NI_DL *dl;		/* Pointer to the datalink*/	
struct PBQ *pbe;		/* Pointer to a port block element*/
int i;
unsigned char nisa_rom[32];
unsigned int raw_id;
unsigned short nicsr0;
unsigned short mbrval;



/* Create an inode entry, thus making the device visible as a file. */
        allocinode (file_name, 1, &ip);

	ip->dva = &de205_ddb;
	ip->attr = ATTR$M_READ | ATTR$M_WRITE;
	ip->loc = 0;
     	ip->misc = (int*)dyn$_malloc(sizeof(struct NI_GBL),
		    			DYN$K_FLOOD|DYN$K_SYNC|DYN$K_NOOWN);
	INODE_UNLOCK (ip);

/*Get the pointer to the NI global block*/
	np = (struct NI_GBL*)ip->misc;
/* fill in pointer to the port block*/
	np->pbp = pb;                           
/*Initialize the names*/
	strcpy(pb->name,file_name);
	strncpy(pb->short_name,file_name,4);

/*Initialize the port spinlock*/
	pb->spl_port.value = 0;	
	pb->spl_port.req_ipl = IPL_SYNC;	
	pb->spl_port.sav_ipl = 0;	
	pb->spl_port.attempts =	0;
	pb->spl_port.retries = 0;	
	pb->spl_port.owner = 0;	
	pb->spl_port.count = 0;	

/*Put the port block (inode) on the port block list*/
	pbe = (struct PBQ*)malloc(sizeof(struct PBQ));
	pbe -> pb = (int*)ip;
	insq_lock(pbe,de205pb.blink);

/*Initialize the msg received list*/
	spinlock(&spl_kernel);
	pb->rcv_msg_cnt = 0;
	pb->rqh.flink = &(pb->rqh);
	pb->rqh.blink = &(pb->rqh);
	spinunlock(&spl_kernel);

/*Init critical datalink stuff*/
	np->dlp = (int*)malloc(sizeof(struct NI_DL));

/*Initialize some flags*/
	pb->state = DE205_K_UNINITIALIZED;

        outportb(pb, ((unsigned __int64)&(pb->rp->de205_b_iopr)),0xc0);
#if K2 || TAKARA
	krn$_sleep(10);	/* marginal timing error on batch of DE205s used */
			/* in customer config */
#else
        krn$_sleep(1);
#endif
        outportb(pb, ((unsigned __int64)&(pb->rp->de205_b_pir2)),0);
        outportb(pb, ((unsigned __int64)&(pb->rp->de205_b_pir1)),0);


/*Set up the interrupt vector*/
	if(pb->pb.mode == DDB$K_INTERRUPT)
	{
		pb->vector = pb->pb.vector;	
		int_vector_set (pb->vector,de205_interrupt,np);
		io_enable_interrupts (pb, pb->vector);
	}


/*Get the station address rom*/                     
	de205_get_nirom (pb,nisa_rom);
/*Check the station address rom*/                     
	if(de205_check_nirom(pb,nisa_rom) != msg_success)
	    return(msg_failure);

        outportb(pb, ((unsigned __int64)&(pb->rp->de205_b_csr)),03); 

/*Set up the Station address*/
	for(i=0; i<6; i++)
	 {
	    pb->sa[i] = nisa_rom[i];
      outportb(pb, ((unsigned __int64)&(pb->rp->de205_b_par0)+i),nisa_rom[i]);
         }
/* set up the memory address from the isacfg table in 2k mode */
  mbrval=(unsigned short)(((unsigned int)(pb->eisa_addr)-0x80000) >>11)&0x00ff;

  outportb(pb, ((unsigned __int64)&(pb->rp->de205_b_mbr)),mbrval);

    for (i=1;i<=63;i++)/* set free pages count */
      outportb(pb, ((unsigned __int64)&(pb->rp->de205_b_fmq)),i);

  outportb(pb, ((unsigned __int64)&(pb->rp->de205_b_csr)),0);

  outportb(pb, ((unsigned __int64)&(pb->rp->de205_b_pir2)),0);
  outportb(pb, ((unsigned __int64)&(pb->rp->de205_b_pir1)),0);
  outportb(pb, ((unsigned __int64)&(pb->rp->de205_b_csr)),3);




/*Set up some configuration info*/
	pb->pb.setmode = 0;
	pb->pb.channel = 0;
	strcpy( pb->pb.name, ip->name );
	sprintf( pb->pb.alias, "EN%c0",
		pb->pb.controller + 'A' );
	sprintf( pb->pb.info, "%02X-%02X-%02X-%02X-%02X-%02X",
		pb->sa[0], pb->sa[1], pb->sa[2],
		pb->sa[3], pb->sa[4], pb->sa[5] );
	pb->pb.sb = 0;
	pb->pb.num_sb = 0;
	sprintf( pb->pb.id, "DE200-LN");
	sprintf( pb->pb.string, "%-24s   %-8s   %24s",
		pb->pb.name, pb->pb.alias, pb->pb.info );

/*Start the next stage of initilization*/
	de205_init_driver (ip);

/*Return a success*/  
	return(msg_success);
}                          

/*+
 * ============================================================================
 * = de205_init_driver - Init the LEMAC port driver                           =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This process will initialize the sgec port driver. In doing so it
 *	start the transmit and receive processes,initialize the mop
 *	counters, receive and transmit descriptors, the lemac, and filters.
 *
 * FORM OF CALL:
 *
 *	de205_init_driver (ip); 
 *
 * RETURNS:
 *
 *   	int Status - msg_success or FAILURE.
 *   
 * ARGUMENTS:
 *
 *	struct INODE *ip - Pointer to the INODE.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int de205_init_driver (struct INODE *ip)
{
struct NI_GBL *np;     		/*Pointer to the NI global database*/
struct NI_ENV *ev;		/*Pointer to the environment variables*/	
struct DE205_PB *pb;		/*Pointer to the port block*/	
struct MOP$_V4_COUNTERS *mc;	/*Pointer to the MOP counters*/	
struct PORT_CALLBACK *cb;	/*Callback structure*/
char name[20];			/*General holder for a name*/


/*Get the pointer to the NI global block*/
	np = (struct NI_GBL*)ip->misc;

/*Get the pointer to the environment variables*/
	np->enp = (int*)malloc(sizeof(struct NI_ENV));
	ev = (struct NI_ENV*)np->enp;

/*Set the environment variables*/
	de205_read_env(ev,ip->name);

/*Get the pointers to the port block*/                  
	pb = (struct DE205_PB*)np->pbp;

/*Get a pointer to the MOP counters*/
     	np->mcp = (int*)malloc(sizeof(struct MOP$_V4_COUNTERS));
	mc = (struct MOP$_V4_COUNTERS*)np->mcp;

/*Init Local routine pointer*/
	pb->lrp = 0;
               
/*Initialize init timer*/
	pb->it = 0;

/*Initialize interrupt semaphores*/
	sprintf(name,"%4.4s_isr_tx",ip->name);
     	krn$_seminit( &(pb->de205_isr_tx.sem),0,name);
        krn$_init_timer(&pb->de205_isr_tx);
     	sprintf(name,"%4.4s_isr_rx",ip->name);
	krn$_seminit( &(pb->de205_isr_rx.sem),0,name);
        krn$_init_timer(&pb->de205_isr_rx);
/*Get a pointer to the transmit information packets*/
	pb->ti = (struct DE205_TX_INFO*)malloc(sizeof(struct DE205_TX_INFO)*
							ev->xmt_buf_no);

/*Initialize some counters*/
	pb->ti_cnt = pb->ri_cnt = pb->tx_bue_cnt = pb->rx_bue_cnt = 0;
	pb->tbf_cnt = pb->babl_cnt = pb->cerr_cnt = pb->miss_cnt = 0;
	pb->merr_cnt = pb->bp_cnt = 0;

/*Initialize the mop counters*/
	init_mop_counters(mc);
/*Setup the callback structudere*/
	cb = (struct PORT_CALLBACK*)malloc(sizeof(struct PORT_CALLBACK)); 	
	cb->st = de205_control_t;
     	cb->cm = de205_change_mode;
	cb->ri = de205_reinit;
	cb->re = de205_read_env;
	

	if(pb->pb.mode == DDB$K_POLLED)
	{
/*Set up the poll queue*/
	pb->pqp.flink = & pb->pqp.flink;
	pb->pqp.blink = & pb->pqp.flink;
	pb->pqp.routine = de205_poll;
	pb->pqp.param = np;
	insq_lock (&pb->pqp.flink, &pollq);
	}


/*Init the transmit ring*/
	de205_init_tx (ip);


/*Start the transmit receive process*/   
	sprintf(name,"rx_%4.4s",ip->name);
	krn$_create (de205_rx,null_procedure,0, 5,-1, 1024 * 4,
				name, "tt", "r", "tt", "w", "tt", "w", ip);
	sprintf(name,"tx_%4.4s",ip->name);
	krn$_create (de205_tx,null_procedure,0, 5,-1, 1024 * 4,
				name, "tt", "r", "tt", "w", "tt", "w", ip);


/*Set the state flag*/
	pb->state = DE205_K_STARTED;

/*Initialize the datalink*/
	ndl_init(ip,cb,pb->sa,(int*)&(pb->lrp));

/*Return a success*/
	return(msg_success);
}

/*+
 * ============================================================================
 * = de205_init_tx - Initialization routine for the transmit ring.            =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will init the initialization the transmit ring.
 *
 * FORM OF CALL:
 *
 *	de205_init_tx (ip); 
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

int de205_init_tx (struct INODE *ip)
{
struct NI_GBL *np;		/*Pointer to the NI global database*/
struct DE205_PB *pb;		/*Pointer to the port block*/	
struct NI_ENV *ev;		/*Pointer to the environment variables*/	




/*Get the pointer to the NI global block*/
	np = (struct NI_GBL*)ip->misc;
/*Get the pointer to the port block*/
	pb = (struct DE205_PB*)np->pbp;
/*Get the pointer to the environment variables*/
	ev = (struct NI_ENV*)np->enp;


/*Initialize the indices*/
	pb->tx_index_in = pb->tx_index_out = 0;

/*Return a success*/
	return(msg_success);
}

/*+
 * ============================================================================
 * = de205_interrupt - Lemac interrupt routine.                               =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine processes the lemac interrupts. 
 *
 * FORM OF CALL:   
 *
 *	de205_interrupt (np); 
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

void de205_interrupt (struct NI_GBL *np)
{
struct DE205_PB	*pb;		/*Pointer to the port block*/	
struct MOP$_V4_COUNTERS *mc;	/*Pointer to the MOP counters*/	
unsigned short nicsr0;

/*Get some pointers*/
	pb = (struct DE205_PB*)np->pbp;
	mc = (struct MOP$_V4_COUNTERS*)np->mcp;
/*read the status*/
	nicsr0 = inportb(pb, ((unsigned __int64)&(pb->rp->de205_b_csr)));



/*Check for recieve int*/
	    if(nicsr0 & 4)
	    {
	    /*Count it*/
		(pb->ri_cnt)++;
	    /*Activate the rcv process*/
        if(pb->pb.mode == DDB$K_INTERRUPT)
outportb(pb, ((unsigned __int64)&(pb->rp->de205_b_icr)),(pb->irqval|0x80)&0xfb);
        else
           outportb(pb, ((unsigned __int64)&(pb->rp->de205_b_icr)),pb->irqval);

		krn$_bpost(&(pb->de205_isr_rx.sem), 1);
	    }

/*Check for transmit int*/
	    if(nicsr0 & 8)
	    {
	    /*Count it*/
		(pb->ti_cnt)++;
        if(pb->pb.mode == DDB$K_INTERRUPT)
outportb(pb, ((unsigned __int64)&(pb->rp->de205_b_icr)),(pb->irqval|0x80)&0xf7);
        else
outportb(pb, ((unsigned __int64)&(pb->rp->de205_b_icr)),pb->irqval);

	    /*Activate the rcv process*/
		krn$_bpost(&(pb->de205_isr_tx.sem), 1);
	    }

/*Enable interrupts again*/
	if(pb->pb.mode==DDB$K_INTERRUPT)
	{
		io_issue_eoi (pb, pb->pb.vector);
	}

}


/*+
 * ============================================================================
 * = de205_open - Open the ES (LEMAC) device.                                 =
 * ============================================================================
 *
 * OVERVIEW:         
 *  
 *      Open the ES (SGEC/TGEC) device.
 *  
 * FORM OF CALL:
 *  
 *	de205_open (fp)
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

int de205_open (struct FILE *fp) 
{
struct NI_GBL *np;			/*Pointer to NI global database*/	
struct DE205_PB *pb;			/*Pointer to the port block*/
int i;


/*Get the pointer to the port block*/
	np = (struct NI_GBL*)fp->ip->misc;
	pb = (struct DE205_PB*)np->pbp;

/*Wait a while and see if the prot initializes*/
	for(i=0; i<(DE205_K_OPEN_TIMEOUT_SECS*4); i++)
	{
	    if(pb->state == DE205_K_STARTED)
		break;		
	    krn$_sleep((1000/4));
	}

/*If we are unitialized return a failure*/
	if(pb->state == DE205_K_UNINITIALIZED)
	    return(msg_failure);

/*Return a success*/
	return(msg_success);
}


/*+
 * ============================================================================
 * = de205_write - Driver write routine.                                         =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine does a writes to the er(LeMAC) port.
 *
 * FORM OF CALL:
 *
 *	de205_write (fp, item_size, number, buf); 
 *                    
 * RETURNS:
 *
 *	int Status - msg_success or FAILURE.
 *
 * ARGUMENTS:
 *
 *      struct FILE *fp - Pointer to the file to be written.
 *	int item_size	- size of item
 *	int number	- number of items
 *	char *buf - Buffer to be written. 
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int de205_write (struct FILE *fp,int item_size,int number,char *buf)
{
struct NI_GBL *np;			/*Pointer to NI global database*/	
struct NI_ENV *ev;			/*Pointer to environment variables*/
struct DE205_PB *pb;			/*Pointer to the port block*/

int index,size,fi;			/*Keeps track of size*/ 
unsigned char *b;			/*Buffer pointer*/
int i,len;				/*Length for padded buffers*/


/*If call backs are needed use callback writes*/
	if(cb_ref)
	    return de205_callback_write (fp,item_size,number,buf);

/*Get the pointer to the port block*/
	np = (struct NI_GBL*)fp->ip->misc;
	pb = (struct DE205_PB*)np->pbp;
	ev = (struct NI_ENV*)np->enp;
/*Get the length*/
	len = item_size*number;

/*Just dump the message if the driver is stopped*/
	if((pb->state == DE205_K_STOPPED) || 
	   (pb->state == DE205_K_UNINITIALIZED))

	{
	    ndl_deallocate_pkt(fp->ip,buf);
	    return(0);
	}

	/*Pad the buffer if its needed*/
	    if(len < DE205_K_MIN_PKT_SIZE)
	    {
		b = ndl_allocate_pkt(fp->ip,0);
		memcpy(b,buf,len);
 		ndl_deallocate_pkt(fp->ip,buf);
		while(len<DE205_K_MIN_PKT_SIZE)
		    b[len++] = 0;
	    }
	    else
		b = buf;


	/*Set the transmit flags*/
	    (pb->ti)[pb->tx_index_in].sync_flag = DE205_K_TX_ASYNC;

      	    size = de205_out(fp->ip,b,len);



	/*Issue a poll*/
	    pb->msg_post_cnt++;
	    pb->msg_post_cnt %= ev->xmt_msg_post;	
	/*Return the size and deallocate the buffer*/
	    ndl_deallocate_pkt(fp->ip,b);
	    return(size);
}

/*+
 * ============================================================================
 * = de205_out - Send the packet.                                             =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine sends a packet.
 *
 * FORM OF CALL:
 *
 *	de205_out (pb,ev,buf,len); 
 *
 * RETURNS:
 *
 *	int Status - msg_success or FAILURE.
 *
 * ARGUMENTS:
 *
 *      struct INODE *ip - Pointer to the inode.
 *	char *buf - Buffer to be sent. 
 *	int len - Number of bytes to be sent. 
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int de205_out (struct INODE *ip,char *buf,int len)
{
struct NI_GBL *np;			/*Pointer to NI global database*/	
struct NI_ENV *ev;			/*Pointer to environment variables*/
struct DE205_PB *pb;			/*Pointer to the port block*/


unsigned char *tbuf;			/*Lance buffer*/
 int index,size,fi;			/*Keeps track of size*/ 
unsigned char *b, *bufaddr;			/*Buffer pointer*/
int i, timeoutval;			/*Length for padded buffers*/
unsigned int page;


/*Get the pointer to the port block*/
	np = (struct NI_GBL*)ip->misc;
	pb = (struct DE205_PB*)np->pbp;
	ev = (struct NI_ENV*)np->enp;



  i=  inportb(pb, ((unsigned __int64)&(pb->rp->de205_b_fmqc)));
  if (i == 0) 
  { 
    return(0);
  }				
/* get next free page number */
  page = inportb(pb, ((unsigned __int64)&(pb->rp->de205_b_fmq)));


  size=len;

/* set up access to that page */
  outportb(pb, ((unsigned __int64)&(pb->rp->de205_b_mpr)),page);

  bufaddr=pb->eisa_addr;
/* set ISA (insert source address) bit to 0 set frame check to 1 */

  outmemb(pb, ((unsigned __int64) (bufaddr++)),2);
  outmemb(pb, ((unsigned __int64) (bufaddr++)),(size)&0xff);
  outmemb(pb, ((unsigned __int64) (bufaddr++)),((size)&0xff00)>>8);


/* start of pdu is 5 bytes, consisting of, xmit ctrl, len hi, len low,*/
/* xmit index (the byte being written  and unused                     */
  outmemb(pb, ((unsigned __int64) (bufaddr++)),5);
  outmemb(pb, ((unsigned __int64) (bufaddr++)),0);

/* write the rest of the packet */
  for (i=0;i<size;i++)
    outmemb(pb, (unsigned __int64) (bufaddr+i),*(buf+i));

/* put it on the transmit queue */
  outportb(pb, ((unsigned __int64)&(pb->rp->de205_b_tq)),page);

  timeoutval=1000;
	/* wait 'til done, then put it back */
  pb->de205_isr_tx.sem.count=0;
  krn$_start_timer(&pb->de205_isr_tx,10000);

  if (krn$_wait(&(pb->de205_isr_tx.sem))==0)
   {
   err_printf("timeout occured in de205 driver \n");
   size=0;
   }
  krn$_stop_timer(&pb->de205_isr_tx);
  outportb(pb, ((unsigned __int64)&(pb->rp->de205_b_fmq)),
               (inportb(pb, (unsigned __int64)&(pb->rp->de205_b_tdq))));

  /*Point to the next descriptor*/
  (pb->tx_index_out)++;
   pb->tx_index_out %= ev->xmt_buf_no;
    if(pb->pb.mode == DDB$K_INTERRUPT)
       outportb(pb, ((unsigned __int64)&(pb->rp->de205_b_icr)),
           inportb(pb, (unsigned __int64)&(pb->rp->de205_b_icr))|0x08);
    else
       outportb(pb, ((unsigned __int64)&(pb->rp->de205_b_icr)),pb->irqval);



  return(size);


}


/*+
 * ============================================================================
 * = de205_setmode - Set mode                                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Set mode.
 *  
 * FORM OF CALL:
 *  
 *	de205_setmode ()
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

int de205_setmode (int mode)
{                           
struct PBQ *pbe;		/*Port block list element*/


/*Do the setmode for all the de205 ports*/
	pbe = de205pb.flink;
	while(pbe != &(de205pb))
	{
	/*Stop if we're asked to*/
	    if(mode == DDB$K_STOP) 
	    /*Execute the stop routine*/
		de205_stop_driver((struct INODE *)pbe->pb);	    	    	
	    else if(mode == DDB$K_START) 
		de205_restart_driver((struct INODE *)pbe->pb);	    	    	

	/*Get the next port*/ 
	    pbe = pbe->flink;
	}
	return( msg_success );
}


/*+
 * ============================================================================
 * = de205_set_env - Sets ES environment variables.                              =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will create and set the ES environment variables.
 *
 * FORM OF CALL:
 *
 *	de205_set_env (name); 
 *
 * RETURNS:
 *
 *	int Status - msg_success or FAILURE.
 *
 * ARGUMENTS:
 *
 *	char *name - Pointer to a name of the port.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void de205_set_env (char *name)
{                             
char var[EV_NAME_LEN];			/*General holder for a name*/

/*Write the environment variables with defaults*/
	sprintf(var,"%4.4s_mode",name);
	ev_write(var,DE205_K_MODE_NORMAL,EV$K_INTEGER);
	sprintf(var,"%4.4s_xmt_buf_no",name);
	ev_write(var,DE205_K_XMT_BUF_CNT,EV$K_INTEGER);
	sprintf(var,"%4.4s_xmt_mod",name);
	ev_write(var,DE205_K_XMT_MOD,EV$K_INTEGER);
	sprintf(var,"%4.4s_xmt_rem",name);
	ev_write(var,DE205_K_XMT_REM,EV$K_INTEGER);
	sprintf(var,"%4.4s_xmt_max_size",name);
	ev_write(var,DE205_K_XMT_MAX_SIZE,EV$K_INTEGER);
	sprintf(var,"%4.4s_xmt_msg_post",name);
	ev_write(var,DE205_K_XMT_MSG_POST,EV$K_INTEGER);
	sprintf(var,"%4.4s_rcv_buf_no",name);
	ev_write(var,DE205_K_RCV_BUF_CNT,EV$K_INTEGER);
	sprintf(var,"%4.4s_rcv_mod",name);
	ev_write(var,DE205_K_RCV_MOD,EV$K_INTEGER);
	sprintf(var,"%4.4s_rcv_rem",name);
	ev_write(var,DE205_K_RCV_REM,EV$K_INTEGER);
	sprintf(var,"%4.4s_msg_buf_size",name);
	ev_write(var,DE205_K_MSG_BUF_SIZE,EV$K_INTEGER);
	sprintf(var,"%4.4s_msg_mod",name);
	ev_write(var,DE205_K_MSG_MOD,EV$K_INTEGER);
	sprintf(var,"%4.4s_msg_rem",name);
	ev_write(var,DE205_K_MSG_REM,EV$K_INTEGER);
	sprintf(var,"%4.4s_driver_flags",name);
	ev_write(var,NDL$K_DRIVER_FLAGS_DEF|NDL$M_ENA_BROADCAST,EV$K_INTEGER);
}


/*+
 * ============================================================================
 * = de205_read_env - Reads the ES environment variables.                        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will read the ES environment variables.
 *
 * FORM OF CALL:
 *
 *	de205_read_env (ev,name); 
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	struct NI_ENV *ev - Pointer to the environment variables.
 *	char *name - Name of the unique part of the environment variable
 *			(Should be the name of the port).
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void de205_read_env (struct NI_ENV *ev,char *name)
{
struct EVNODE *evp,evn;		/*Pointer to the evnode,evnode*/
char var[EV_NAME_LEN];		/*General holder for a name*/

/*Get a pointer to the evnode*/
	evp = &evn;

/*Read the environment variables*/
/*Setup the mode*/
	sprintf(var,"%4.4s_mode",name);
	if(ev_read(var,&evp,0) == msg_success)
	    ev->mode = evp->value.integer;
	else
	{
	    ev->mode = DE205_K_MODE_NORMAL;
	    ni_error("",msg_ez_err_n_env_var,var);
	}

        sprintf(var,"%4.4s_driver_flags",name);
        if(ev_read(var,&evp,0) == msg_success)
	{
	    ev->driver_flags = evp->value.integer;
	}
	else
	{
	    ev->driver_flags = NDL$K_DRIVER_FLAGS_DEF|NDL$M_ENA_BROADCAST;
	    ni_error("",msg_ez_err_n_env_var,var);
	}

/*Now lets setup some tx info*/
	sprintf(var,"%4.4s_xmt_buf_no",name);
	if(ev_read(var,&evp,0) == msg_success)
	    ev->xmt_buf_no = evp->value.integer;
	else
	{
	    ev->xmt_buf_no = DE205_K_XMT_BUF_CNT;
	    ni_error("",msg_ez_err_n_env_var,var);
	}
        sprintf(var,"%4.4s_xmt_mod",name);
        if(ev_read(var,&evp,0) == msg_success)
	    ev->xmt_mod = evp->value.integer;
	else
	{
	    ev->xmt_mod = DE205_K_XMT_MOD;
	    ni_error("",msg_ez_err_n_env_var,var);
	}
        sprintf(var,"%4.4s_xmt_rem",name);
        if(ev_read(var,&evp,0) == msg_success)
	    ev->xmt_rem = evp->value.integer;
	else
	{
	    ev->xmt_rem = DE205_K_XMT_REM;
	    ni_error("",msg_ez_err_n_env_var,var);
	}
	sprintf(var,"%4.4s_xmt_max_size",name);
	if(ev_read(var,&evp,0) == msg_success)
	    ev->xmt_max_size = evp->value.integer;
	else
	{
	    ev->xmt_max_size = DE205_K_XMT_MAX_SIZE;
	    ni_error("",msg_ez_err_n_env_var,var);
	}
	sprintf(var,"%4.4s_xmt_msg_post",name);
	if(ev_read(var,&evp,0) == msg_success)
	    ev->xmt_msg_post = evp->value.integer;
	else
	{
	    ev->xmt_msg_post = DE205_K_XMT_MSG_POST;
	    ni_error("",msg_ez_err_n_env_var,var);
	}


/*First lets setup receive info*/
	sprintf(var,"%4.4s_rcv_buf_no",name);
	if(ev_read(var,&evp,0) == msg_success)
	    ev->rcv_buf_no = evp->value.integer;
	else
	{
	    ev->rcv_buf_no = DE205_K_RCV_BUF_CNT;
	    ni_error("",msg_ez_err_n_env_var,var);
	}
        sprintf(var,"%4.4s_msg_buf_size",name);
        if(ev_read(var,&evp,0) == msg_success)
	    ev->msg_buf_size = evp->value.integer;
	else
	{
	    ev->msg_buf_size = DE205_K_MSG_BUF_SIZE;
	    ni_error("",msg_ez_err_n_env_var,var);
	}
        sprintf(var,"%4.4s_rcv_mod",name);
        if(ev_read(var,&evp,0) == msg_success)
	    ev->rcv_mod = evp->value.integer;
	else
	{
	    ev->rcv_mod = DE205_K_RCV_MOD;
	    ni_error("",msg_ez_err_n_env_var,var);
	}
        sprintf(var,"%4.4s_rcv_rem",name);
        if(ev_read(var,&evp,0) == msg_success)
	    ev->rcv_rem = evp->value.integer;
	else
	{
	    ev->rcv_rem = DE205_K_RCV_REM;
	    ni_error("",msg_ez_err_n_env_var,var);
	}

/*Some message info*/
        sprintf(var,"%4.4s_msg_mod",name);
        if(ev_read(var,&evp,0) == msg_success)
	    ev->msg_mod = evp->value.integer;
	else
	{
	    ev->msg_mod = DE205_K_MSG_MOD;
	    ni_error("",msg_ez_err_n_env_var,var);
	}
        sprintf(var,"%4.4s_msg_rem",name);
        if(ev_read(var,&evp,0) == msg_success)
	    ev->msg_rem = evp->value.integer;
	else
	{
	    ev->msg_rem = DE205_K_MSG_REM;
	    ni_error("",msg_ez_err_n_env_var,var);
	}

	ev->msg_prefix_size = 0;

}


/*+
 * ============================================================================
 * = de205_check_nirom - Tests the nirom.                                        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will test the ni rom. 
 *
 * FORM OF CALL:
 *
 *	de205_check_nirom (pb,ni); 
 *
 * RETURNS:
 *
 *	int Status - msg_success or FAILURE.
 *
 * ARGUMENTS:
 *
 *      struct DE205_PB *pb - Port block.
 *	unsigned char *ni - Pointer to the the ni rom.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int de205_check_nirom (struct DE205_PB *pb,unsigned char *ni)
{
unsigned long chksm;		/*Computed checksum*/
int i,j,k;			/* index*/
unsigned short t1,*t3;		/*Temporary locations*/
unsigned char t2[4];
unsigned long *t4;


/*Verify that we don't have a null address*/
	t1 = 0;
	for(i=0; i<6; i++)
	    t1 += (int)ni[i];
	if(!t1)
	{
	    ni_error(pb->short_name,msg_ez_err_nisa_null);
	    return(msg_ez_err_nisa_null);
	}

/*Verify the address checksum*/
/*Initialize some variables*/	
	t3 = (unsigned short*)t2;
	chksm = 0;

/*Compute the partial address checksum*/
	for(i=0; i<6; i+=2)
	{
	/*Advance the checksum*/
	    chksm += chksm;
	/*Make the 2 bytes a word*/
 	    t2[1] = ni[i];		
 	    t2[0] = ni[i+1];		
	/*Add it in*/
	    chksm += *t3;
	}

/* Reduce SUM modulo 65535*/
	while(chksm >= 65535)
	    chksm -= 65535;

    

/*Compare the computed checksum with loaded checksum*/
/*Get the loaded checksum*/
	t2[1] = ni[6];		
	t2[0] = ni[6+1];		
	if(chksm != *t3)
	{
	    ni_error(pb->short_name,msg_ez_err_nisa_bad_chk);
	    return(msg_ez_err_nisa_bad_chk);
	}

/*Verify that the address is repeated again backwards*/
	for(i=0,j=15,k=16; i<8; i++,j--,k++)
	    if(ni[j] != ni[k])
	    {
		ni_error(pb->short_name,msg_ez_err_nisa_n_rpt);
		return(msg_ez_err_nisa_n_rpt);
	    }

/*Verify that the pattern ff,00,55,aa,ff,00,55,aa exists*/
/*Initialize some variables*/	
	t4 = (unsigned long*)t2;
	for(i=0,j=24; i<2; i++,j+=4)
	{
	    t2[3] = ni[j];		
	    t2[2] = ni[j+1];		
	    t2[1] = ni[j+2];		
	    t2[0] = ni[j+3];		
	    if(*t4 != 0xff0055aa)
	    {
	        ni_error(pb->short_name,msg_ez_err_bad_patt);
	        return(msg_ez_err_bad_patt);
	    }
	}

/*Return a success*/
	return(msg_success);
}


/*+
 * ============================================================================
 * = de205_tx - Transmit process.                                                =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine is actually a process that is created by de205_init_driver.
 *	It processes used transmit descriptors.
 *
 * FORM OF CALL:
 *
 *	de205_tx (ip); 
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

void de205_tx (struct INODE *ip)
{
struct NI_GBL *np;		/*Pointer to the NI global database*/
struct DE205_PB *pb;		/*Pointer to the port block*/	
struct MOP$_V4_COUNTERS *mc;	/*Pointer to the MOP counters*/	
struct NI_ENV *ev;		/*Pointer to the environment variables*/
int cont;			/*Flag to continue processing*/

/*Get the pointer to the NI global block*/
	np = (struct NI_GBL*)ip->misc;
/*Get the pointer to the port block*/
	pb = (struct DE205_PB*)np->pbp;
/*Get a pointer to mop counters*/
	mc = (struct MOP$_V4_COUNTERS*)np->mcp;
/*Get a pointer to the environment variables*/
	ev = (struct NI_ENV*)np->enp;




}



/*+
 * ============================================================================
 * = de205_rx - Recieve process.                                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine is actually a process that is created by de205_init_driver.
 *	It processes receive descriptors. Its main purpose is to keep
 *	going through the descriptors and processing them.
 *
 * FORM OF CALL:
 *
 *	de205_rx (ip); 
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

int de205_rx (struct INODE *ip)
{
struct NI_GBL *np;		/*Pointer to the NI global database*/
struct DE205_PB *pb;		/*Pointer to the port block*/	
struct MOP$_V4_COUNTERS *mc;	/*Pointer to the MOP counters*/	
struct NI_ENV *ev;		/*Pointer to the environment variables*/	
struct FILE *fp;		/*Pointer to the file being used*/

unsigned char *msg,*rbuf;	/*Pointer to the packets*/
int len, retries,i,size,status;
unsigned int page,next;


/*Get a file pointer for this device*/
/*This file stays open forever*/
	fp = fopen(ip->name,"r+");
	if(!fp)
	    return(msg_failure);
/*Get the pointer to the NI global block*/
	np = (struct NI_GBL*)ip->misc;
/*Get the pointer to the port block*/
	pb = (struct DE205_PB*)np->pbp;
/*Get a pointer to the MOP counters*/
	mc = (struct MOP$_V4_COUNTERS*)np->mcp;
/*Get the pointer to the environment variables*/
	ev = (struct NI_ENV*)np->enp;
/*Do this forever*/
	while(1)
	{

	    /*Wait for the receive interrupt*/
	        krn$_wait(&(pb->de205_isr_rx.sem));
	    /*Process the descriptor*/
		msg = ndl_allocate_pkt(ip,1);
		status = de205_process_rx(pb,mc,ev,&msg,&size);

	    /*If we've got a good packet handle it*/
		if(status == msg_success)
		{
		 /*Handle the received message*/
	 	 de205_msg_rcvd(fp,pb,mc,msg,size);
		}
                else
                {
                 ndl_deallocate_pkt(ip,msg);
                }
    if(pb->pb.mode == DDB$K_INTERRUPT)
               outportb(pb, ((unsigned __int64)&(pb->rp->de205_b_icr)),
               (inportb(pb, (unsigned __int64)&(pb->rp->de205_b_icr))|0x04));
    else
       outportb(pb, ((unsigned __int64)&(pb->rp->de205_b_icr)),pb->irqval);


	}

}



/*+
 * ============================================================================
 * = de205_msg_rcvd - Handle a received message.                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will handle a received message its major function is to
 *	call the class driver recieve routine.
 *
 * FORM OF CALL:
 *
 *	de205_msg_rcvd (fp,pb,mc,msg,size); 
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	struct FILE *fp - Pointer to the receive process port file.
 *	struct DE205_PB *pb - Pointer to the port block.
 *	struct MOP$_V4_COUNTERS *mc - Pointer to the counters.
 *	char *msg - Pointer to the message received.
 *
 *
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void de205_msg_rcvd(struct FILE *fp,struct DE205_PB *pb,struct MOP$_V4_COUNTERS *mc,
						 char *msg,unsigned short size)
{
struct DE205_MSG *mh;				/*Recieved message holder*/



/*If Callbacks are enabled pass the raw packet*/
	if(cb_ref)
	{
	/*If there are already too many messages cleanup the list*/
	    if(pb->rcv_msg_cnt > DE205_MAX_RCV_MSGS)
		de205_free_rcv_pkts(fp->ip);

	/*Get a message holder*/
	    mh = (struct DE205_MSG*)malloc(sizeof(struct DE205_MSG));
                                    
	/*Fill in the message holder*/
	    mh->msg = msg;
	    mh->size = size;
                            
	/*Insert it onto the list*/
	    spinlock(&spl_kernel);
	    insq(mh,pb->rqh.blink);                                
	    pb->rcv_msg_cnt++;
	    spinunlock(&spl_kernel);
            return;
	}


/*Call the class driver receive routine*/

	if(pb->lrp)
         {
	/*Call the class driver*/
	    (*(pb->lrp))(fp,msg,size);
         }
	else
	{
	/*There is no receive routine bump the count and continue*/
	    _addq_v_u((mc->MOP_V4CNT_UNKNOWN_DESTINATION.l),1); 
	/*Give the buffer back*/
	    ndl_deallocate_pkt(fp->ip,msg);
	}
}


/*+
 * ============================================================================
 * = de205_process_rx - Process RX descriptors and packets.                      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will process receive packets and descriptors. Processing
 *	includes LEMAC and descriptor error checking. If the lemac still owns
 *	the descriptor we will just return wait for the receive interrupt.
 *
 * FORM OF CALL:
 *
 *	de205_process_rx (pb,mc,ev,rdes,rbuf); 
 *
 * RETURNS:
 *
 *	int Status - msg_success or FAILURE.
 *
 * ARGUMENTS:
 *
 *	struct DE205_PB *pb - Pointer to the es port block.
 *	struct MOP$_V4_COUNTERS *mc - Pointer to the MOP counter block.
 *	struct NI_ENV *ev - Pointer to the environment variables.
 *	struct rbd *rdes - Pointer to a copy of the receive descriptor.
 *	unsigned char **rbuf - Pointer to the message.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int de205_process_rx (struct DE205_PB *pb,struct MOP$_V4_COUNTERS *mc,
					struct NI_ENV *ev,
					unsigned char **rbuf,
					int *size)
{
int retries,i;
unsigned char xpage,xstatus,xnext;
unsigned char *c,*bufaddr;
int j,k;
int foundbad = 0;

c=*rbuf;
	spinlock(&(pb->spl_port));

  do {				
   if (inportb(pb, ((unsigned __int64)&(pb->rp->de205_b_rqc)))==0)
     {
      spinunlock(&(pb->spl_port));
      return (msg_failure);
     }
    xpage = inportb(pb, ((unsigned __int64)&(pb->rp->de205_b_rq)));
	/* get page index of new packet */ 
  outportb(pb, ((unsigned __int64)&(pb->rp->de205_b_mpr)),xpage);
  bufaddr=pb->eisa_addr;

    xstatus = inmemb(pb, ((unsigned __int64) (bufaddr++)));

    if ((xstatus&0x80)!=0x80)	/* if bogus packet then */
      outportb(pb, ((unsigned __int64)&(pb->rp->de205_b_fmq)),xpage);

  } while ((xstatus&0x80)!=0x80);/* do while packets present */


				/* get size of packet */
  *size = (inmemb(pb, (unsigned __int64) (bufaddr++))|
         inmemb(pb, (unsigned __int64) (bufaddr++))<<8);


  xnext = inmemb(pb, ((unsigned __int64) (bufaddr++)));

  *size=*size-4; 	/* since status, size*2 and filler has been read */

  
  for (i=0;i<*size;i++)
   {
/* load packet into users buffer */
    *(c+i)=inmemb(pb, ((unsigned __int64) (bufaddr++)));
   }

	spinunlock(&(pb->spl_port));

		/*Its a physical address*/
		    _addq_v_u((mc->MOP_V4CNT_RX_FRAMES.l),1); 
  outportb(pb, ((unsigned __int64)&(pb->rp->de205_b_fmq)),xpage);
/*Return success*/
	return(msg_success);
}


/*+
 * ============================================================================
 * = de205_control_t - DE205's control T routine.                             =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine is used to display DE205 status (counters etc.) in real time.
 *
 * FORM OF CALL:
 *
 *	de205_control_t (np); 
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

void de205_control_t(struct NI_GBL *np)
{
struct DE205_PB	*pb;		/*Pointer to the port block*/	

/*Get some pointers*/
	pb = (struct DE205_PB*)np->pbp;

/*Display some status*/
	printf(msg_ez_inf_dev_spec);
	printf("TI: %d RI: %d BABL: %d CERR: %d MISS: %d MERR: %d\n",
						pb->ti_cnt,pb->ri_cnt,
						pb->babl_cnt,pb->cerr_cnt,
						pb->miss_cnt,pb->merr_cnt);
	printf("TX FULL: %d BRDCST: %d TX BUE: %d RX BUE: %d\n",pb->tbf_cnt,
							pb->bp_cnt,
							pb->tx_bue_cnt,
							pb->rx_bue_cnt);
}





/*+
 * ============================================================================
 * = de205_change_mode - Change the LEMAC mode.                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will change the LEMAC operating mode. It uses the
 *	specified constant for the mode to be changed to.
 *
 * FORM OF CALL:
 *
 *	de205_change_mode (fp,mode); 
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
 *		NDL$K_NORMAL_FILTER:	Normal filter mode.
 *		NDL$K_PROM: 		Promiscious.
 *		NDL$K_MULTICAST:	All multicast frames passed.
 *		NDL$K_INT_PROM:		Internal loopback and promiscious.
 *		NDL$K_INT_FC: 		Internal loopback and force collision.
 *		NDL$K_NO_FC: 		No force collision.
 *		NDL$K_DEFAULT: 		Use the environment variable.
 *
 * SIDE EFFECTS:
 *   
 *      None
 *
-*/

int de205_change_mode(struct FILE *fp,int mode)
{

	pprintf("DE205 change mode not implemented....................\n");


/*Return a success*/
	return(msg_success);
}

                                        
/*+
 * ============================================================================
 * = de205_reinit - Reinitialize the es port.                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will reinitialize the es ports recieve and transmit
 *	buffers and descriptors.
 *
 * FORM OF CALL:
 *
 *	de205_reinit (pf); 
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	struct FILE *pf - Pointer to the port file.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void de205_reinit (struct FILE *pf)
{
	pprintf("DE205 reinit not implemented....................\n");
}


/*+                      
 * ============================================================================
 * = de205_get_rom - Gets the de205 eerom . 	         	      	      =
 * ============================================================================
 *               
 * OVERVIEW:                                                                  
 *  
 *	This routine will get the LEMAC EEROM and stuff the PB with the 
 *	correct values. It will also check that the values in the EEROM 
 *	agree with the isa_cfg and update as needed.
 *           
 * FORM OF CALL:                                                 
 *  
 *  	de205_get_rom (pb);
 *              
 * RETURNS:
 *
 *	pd - Pointer to the port block
 *                    
 * ARGUMENTS:                           
 *
 *      struct DE205_PB *pb - Pointer to the port block.
 *                          
-*/             

void de205_get_rom (struct DE205_PB *pb)
{
int status;
struct TABLE_ENTRY temp_entry;   /*a place to store isacfg stuff*/
int found,i;                           
unsigned long isa_dma_val, isa_irq_val, le_dma_val, le_irq_val;
unsigned __int64 isa_io_addr, isa_mem_addr, temp_addr, le_mem_addr;
char de_eerom[32];

   de205_controllers++;
   temp_addr=1;
   temp_addr<<=32;
   found = isacfg_get_device("DE200-LE",isa_count,&temp_entry);
   if (found != 0)
    {
      pb->pb.slot = (temp_entry.isa_slot|(temp_entry.dev_num <<4));
      isa_irq_val = temp_entry.irq0_assignment;
      if (isa_irq_val==5)
        pb->irqval=0x0c;
      if (isa_irq_val==10)
        pb->irqval=0x2c;
      if (isa_irq_val==15)
        pb->irqval=0x6c;


      if (isa_irq_val==0x80000000 || IPL_RUN > 19)
	{
        pb->irqval=0x00;
        pb->pb.mode=DDB$K_POLLED;
	}
      else
        pb->pb.mode=DDB$K_INTERRUPT;

      pb->pb.vector=isa_irq_val+0x80;
      isa_io_addr = temp_entry.io_base_addr0;
      isa_mem_addr = temp_entry.mem0_base_addr;
      pb->eisa_addr=isa_mem_addr;
      pb->rp = (struct de205_regs*)(isa_io_addr);    
      
    }



}


/*+                      
 * ============================================================================
 * = ewrk3_config - Change the DE205 configuration, find addresses of modules.=
 * ============================================================================
 *               
 * OVERVIEW:                                                                  
 *  
 *      This routine will perform lemac specific changes to the de250 
 *	module.
 *
 *   COMMAND FORM:
 *  
 *     ewrk3_config ([-curaddr <addr#>][-ioaddr <hex value>][-irq <3,5,9,10,15>]
 *                   [-fbus <0,1>][-ena16 <0,1>][-memaddr <hex>]
 *                   [-bufsize <2,32,64>][-swflag <hexvalue>]
 *                   [-netmgt <hex value>][-zws <0,1>][-ra<0,1>][-wb<0,1>]
 *                   [-default])
 *
 *   COMMAND FMT: ewrk3_CONFIG 0 0 0 ewrk3_CONFIG
 *
 *
 *   COMMAND TAG: ewrk3_config 0 RXB ewrk3_config
 *              
 * FORM OF CALL:                                                 
 *  
 *      ewrk3_config(argc, argv)
 *
 *           
 * RETURNS:
 *
 *   	Nothing.
 *                    
 * ARGUMENTS:                           
 *
 *                          
-*/             
ewrk3_config(int argc, char *argv[])
{
unsigned char de_eerom[32];
unsigned char de_chksum,tempval,tempirq,chiprev;
unsigned int i, new_addr;
unsigned char de205_chksum();
char status;
int update,bufsize;
int dontwrt,newaddr;

struct de205_regs *rp;
unsigned long  le_dma_val, le_irq_val;
 /* config_lemac */
struct QSTRUCT qual[DMAX];
      if (argc == 1)
       {
        find_lemac();
       }
      else
       {
        status = qscan(&argc,argv,"-",DSTRING,qual);
        if (status != msg_success)
        {
	    err_printf (status);
            return;
        }
      if (qual[DCURADDR].present)
        {
         rp=(struct de205_regs*)(qual[DCURADDR].value.integer);
        }
      else
        {
         err_printf("no current address givin \n");
         return;
        }
/* read the de205 eerom */
        for(i=0; i<16; i++)
         {
          spinlock(&spl_kernel);
          outportb(0, ((unsigned __int64)&(rp->de205_b_pir1)),i);
          outportb(0, ((unsigned __int64)&(rp->de205_b_iopr)),0xe0);
	  krn$_sleep(2);
          de_eerom[i*2] = inportb(0, (unsigned __int64)&(rp->de205_b_edr1));
      de_eerom[(i*2)+1] = inportb(0, (unsigned __int64)&(rp->de205_b_edr2));
          spinunlock(&spl_kernel);
         }

/* we now have the DE205 EEROM in the de_rom array, check that it agrees */
/* with the isacfg table don't bother with the ioaddr0 as if it's not    */
/* right, we cant read the thing anyway					 */
       chiprev=de_eerom[22]&0x0f; /* a value of 2 is a LeMACII */
       if (qual[DSHOW].present)
         for(i=0; i<16; i++)
           {
 err_printf("EErom[%d] dreg2=%x dreg1=%x\n",i,de_eerom[(i*2)+1],de_eerom[i*2]); 
           }   
       de_chksum=de205_chksum(de_eerom);

      update=0;
      if (qual[DIOADDR].present)
        {
         new_addr=qual[DIOADDR].value.integer;
	 if ((new_addr <= 0x3b0) && (new_addr >= 0x100))
	   {
            de_eerom[1] =(char)(new_addr >> 5);
            de205_eewrite(de_eerom[0],de_eerom[1],0,rp);
   	    update=1;
	   }
         else
           err_printf("address must be in the range (100h < addr < 3b0h) \n");
        }
      if (qual[DMEMADDR].present||qual[DBUFSIZE].present)
        {
	 dontwrt=0;
         tempval=de_eerom[0];
         if (qual[DBUFSIZE].present)
          {
           bufsize=qual[DBUFSIZE].value.integer;
           if ((bufsize != 2)&&(bufsize != 32)&&(bufsize != 64))
	    {
             err_printf("Invalid buffer size %d\n",bufsize);
             dontwrt=1;
            }
          }
         else
          {
           if ((tempval>=0x0a)&&(tempval<=0xf))
            bufsize=64;
           if ((tempval>=0x14)&&(tempval<=0x1f))
            bufsize=32;
           if ((tempval>=0x40)&&(tempval<=0xff))
            bufsize=2;
          }
         if (qual[DMEMADDR].present)
          {
          newaddr=qual[DMEMADDR].value.integer;
          }
         else
          {
           if ((tempval>=0x0a)&&(tempval<=0x0f))
             newaddr=tempval*0x10000;
           if ((tempval>=0x14)&&(tempval<=0x1f))
             newaddr=tempval*0x8000;
           if ((tempval>=0x40)&&(tempval<=0xff))
             newaddr=tempval*0x800+0x80000;
          }

	 if(bufsize==2)
            tempval=(newaddr-0x80000)/0x800;
         if(bufsize==32)
            tempval=newaddr/0x8000;
	 if(bufsize==64)
            tempval=newaddr/0x10000;

         de_eerom[0]=(unsigned char)(tempval);
         if(dontwrt==0)
          {
           de205_eewrite(de_eerom[0],de_eerom[1],0,rp);
	   update=1;
          }
        }
      dontwrt=0;
      if ((qual[DIRQ].present)||(qual[DFBUS].present)||(qual[DENA16].present)||
          (qual[DZWS].present)||(qual[DRA].present)||(qual[DWB].present))
        {
        tempval=de_eerom[6];

/* its a LeMAC I chip */
        if (qual[DIRQ].present && chiprev < 2)
         {
          tempirq=(unsigned char)(qual[DIRQ].value.integer); 
	   if (tempirq==5)
            de_eerom[6]=de_eerom[6]&0x9f;
           else if (tempirq==10)
            {
             de_eerom[6]=de_eerom[6]&0x9f;
             de_eerom[6]=de_eerom[6]|0x020; 
            }
           else if (tempirq==15)
            {
             de_eerom[6]=de_eerom[6]&0x9f;
             de_eerom[6]=de_eerom[6]|0x060; 
            }
           else
            {
              err_printf("Invalid IRQ on LeMAC Rev. %x \n",chiprev); 
              dontwrt=1;
            }
         }

/* if its a LeMACII chip the IRQ's are different */

        if (qual[DIRQ].present && chiprev == 2)
         {
          tempirq=(unsigned char)(qual[DIRQ].value.integer); 
	   if (tempirq==5)
            de_eerom[6]=de_eerom[6]&0x8f;
           else if (tempirq==10)
            {
             de_eerom[6]=de_eerom[6]&0x8f;
             de_eerom[6]=de_eerom[6]|0x020; 
            }
           else if (tempirq==3)
            {
             de_eerom[6]=de_eerom[6]&0x8f;
             de_eerom[6]=de_eerom[6]|0x030; 
            }
           else if (tempirq==9)
            {
             de_eerom[6]=de_eerom[6]&0x8f;
             de_eerom[6]=de_eerom[6]|0x050; 
            }
           else if (tempirq==15)
            {
             de_eerom[6]=de_eerom[6]&0x8f;
             de_eerom[6]=de_eerom[6]|0x060; 
            }
           else
            {
              err_printf("Invalid IRQ \n"); 
              dontwrt=1;
            }
         }


        if (qual[DFBUS].present)
         {
          if((qual[DFBUS].value.integer) == 0)
           {
             de_eerom[6]=de_eerom[6]&0xf7;
	   }

          else if((qual[DFBUS].value.integer) == 1)
           {
             de_eerom[6]=de_eerom[6]|0x08;
	   }
          else
           {
            dontwrt=1;
            err_printf("Invalid FAST_BUS \n"); 
           }
	 }          


        if (qual[DENA16].present)
         {
          if((qual[DENA16].value.integer) == 0)
           {
             de_eerom[6]=de_eerom[6]&0xfb;
	   }

          else if((qual[DENA16].value.integer) == 1)
           {
             de_eerom[6]=de_eerom[6]|0x04;
	   }
          else
           {
            dontwrt=1;
            err_printf("Invalid 16_ENA \n");
           }
	 }         

/* DZWS DRA DWB bits are only avalible on the LeMAC II chip */
        if ((qual[DZWS].present || qual[DRA].present || qual[DWB].present)&&chiprev<2)
	  {
	    dontwrt=1;
            err_printf("Option invalid on LeMAC REV %x\n",chiprev); 
	  }
        if (qual[DZWS].present && chiprev==2)
         {
          if((qual[DZWS].value.integer) == 0)
           {
             de_eerom[6]=de_eerom[6]&0xfe;
	   }

          else if((qual[DZWS].value.integer) == 1)
           {
             de_eerom[6]=de_eerom[6]|0x01;
	   }
          else
           {
            dontwrt=1;
            err_printf("Invalid ZWS \n"); 
           }
	 }          

        if (qual[DRA].present && chiprev==2)
         {
          if((qual[DRA].value.integer) == 0)
           {
             de_eerom[6]=de_eerom[6]&0x7f;
	   }

          else if((qual[DRA].value.integer) == 1)
           {
             de_eerom[6]=de_eerom[6]|0x80;
	   }
          else
           {
            dontwrt=1;
            err_printf("Invalid read ahead \n"); 
           }
	 }          
        if (qual[DWB].present && chiprev==2)
         {
          if((qual[DWB].value.integer) == 0)
           {
             de_eerom[6]=de_eerom[6]&0xfd;
	   }
          else if((qual[DWB].value.integer) == 1)
           {
             de_eerom[6]=de_eerom[6]|0x02;
	   }
          else
           {
            dontwrt=1;
            err_printf("Invalid write behind \n"); 
           }
	 }          

 
         if (dontwrt==0)
          {
           de205_eewrite(de_eerom[6],de_eerom[7],3,rp);
	   update=1;
          }
        }

      if (qual[DSWFLAG].present)
        {
         de_eerom[16]=(unsigned char)(qual[DSWFLAG].value.integer);
         de205_eewrite(de_eerom[16],de_eerom[17],8,rp);
	 update=1;
        }

      if (qual[DNETMGT].present)
        {
         de_eerom[20]=(unsigned char)(qual[DNETMGT].value.integer);
         de205_eewrite(de_eerom[20],de_eerom[21],10,rp);
	 update=1;
        }
      if (qual[DDEFAULT].present)
        {
         de_eerom[0] =0xa0;
         de_eerom[1] =0x18;
         de205_eewrite(de_eerom[0],de_eerom[1],0,rp);
         de_eerom[6]=0x04;
         de205_eewrite(de_eerom[6],de_eerom[7],3,rp);
         de_eerom[16]=0x12;
         de205_eewrite(de_eerom[16],de_eerom[17],8,rp);
         de_eerom[20]=0x01;
         de205_eewrite(de_eerom[20],de_eerom[21],10,rp);
	 update=1;
        }

       de_eerom[31]=de205_chksum(de_eerom);

        if (update==1)
         {
         de205_eewrite(de_eerom[30],de_eerom[31],15,rp); 
err_printf("Unit must be power cycled for these changes to take affect\n");
         }

       }
  }

/*+                      
 * ============================================================================
 * = de205_eewrite - Write the the LEMAC eerom   	                             =
 * ============================================================================
 *               
 * OVERVIEW:                                                                  
 *  
 *           
 * FORM OF CALL:                                                 
 *  
 *              
 * RETURNS:
 *
 *                    
 * ARGUMENTS:                           
 *
 *                          
-*/             

void de205_eewrite(unsigned char wdata1, unsigned char wdata2, int rindx,
                struct de205_regs *rp)
 {
           spinlock(&spl_kernel);
           outportb(0, ((unsigned __int64)&(rp->de205_b_iopr)),0xc8);
	   krn$_sleep(1);
           outportb(0, (unsigned __int64)&(rp->de205_b_edr1),wdata1);
           outportb(0, (unsigned __int64)&(rp->de205_b_edr2),wdata2);
           outportb(0, ((unsigned __int64)&(rp->de205_b_pir1)),rindx);
           outportb(0, ((unsigned __int64)&(rp->de205_b_iopr)),0xd0);
	   krn$_sleep(15);
           outportb(0, ((unsigned __int64)&(rp->de205_b_iopr)),0xd8);
	   krn$_sleep(1);

           spinunlock(&spl_kernel);

 } 



/*+                      
 * ============================================================================
 * = de205_chksum - calculate the checksum on the LEMAC eerom 	              =
 * ============================================================================
 *               
 * OVERVIEW:                                                                  
 *  
 *           
 * FORM OF CALL:                                                 
 *  
 *              
 * RETURNS:
 *
 *                    
 * ARGUMENTS:                           
 *
 *                          
-*/             

unsigned char de205_chksum(unsigned char *rom_img)
 {
  int i;
  unsigned char chksum;

   chksum=0;
   for (i=0;i<31;i++)
    {
     chksum=chksum + *rom_img++;
    }
  chksum=0x100-chksum;
  return (chksum);
 }

/*+                      
 * ============================================================================
 * = find_de205 - Find  LEMAC in the system              	              =
 * ============================================================================
 *               
 * OVERVIEW:                                                                  
 *  
 *           
 * FORM OF CALL:                                                 
 *  
 *              
 * RETURNS:
 *
 *                    
 * ARGUMENTS:                           
 *
 *                          
-*/             

void find_lemac()
 {
  int i,j, found;
  unsigned int start_addr, end_addr, addr_inc, test_addr;
  struct de205_regs *rp;
  unsigned char ni[40];
  char name[7];
  char tst_name[] = "DE200-LE";
  unsigned char test_data[8]={0xff,0x00,0x55,0xaa,0xff,0x00,0x55,0xaa};

  start_addr=0x100;
  end_addr=0x3b0;
  addr_inc=0x20;
  found=0;
  
  

  for(test_addr=start_addr; test_addr <= end_addr; test_addr+=addr_inc)
   {
    rp=(struct de205_regs*)(test_addr);
    
     /*Find the start of the ROM*/
    j=0;
    i=0;
    while((i<39)&&(j!=8))
      {
       ni[i] = inportb(0, (unsigned __int64)&(rp->de205_b_ap));
       if (ni[i++] == test_data[j])
	{
	 j++;
	}
       else
	 j=0;
      }

    if (j==8)
     {
      /* found a candidate */
     for (j=0;j<3;j++)
      { 
       spinlock(&spl_kernel);
       outportb(0, ((unsigned __int64)&(rp->de205_b_pir1)),j+4);
       outportb(0, ((unsigned __int64)&(rp->de205_b_iopr)),0xe0);
       krn$_sleep(2);
       name[j*2] = inportb(0, (unsigned __int64)&(rp->de205_b_edr1));
       name[(j*2)+1] = inportb(0, (unsigned __int64)&(rp->de205_b_edr2));
       spinunlock(&spl_kernel);
      }
      name[5]=0;
      if (strcmp(name,"DE205")==0)
       {
       err_printf("found board addr=%x \n",test_addr);
       found=1;
       }
     }
   }   
 }

/*+                      
 * ============================================================================
 * = de205_get_nirom - Provide a pointer to nirom. 		      	      =
 * ============================================================================
 *               
 * OVERVIEW:                                                                  
 *  
 *      This routine will perform platform specific functions to
 *	provide a pointer to the nirom.  
 *           
 * FORM OF CALL:                                                 
 *  
 *  	de205_get_nirom (pb,ni);
 *              
 * RETURNS:
 *
 *   	ptr - Pointer to a copy of the ni station address rom.
 *                    
 * ARGUMENTS:                           
 *
 *      struct DE205_PB *pb - Pointer to the port block.
 *      unsigned char *ni - Pointer to a buffer to filled.
 *                          
-*/             

void de205_get_nirom (struct DE205_PB *pb,unsigned char *ni)
{
int i,j;
unsigned char match_pattern[8];
unsigned char nirom[64];

/*Make the pattern we're looking for*/
  match_pattern[0]=0xff;match_pattern[1]=0x00;match_pattern[2]=0x55;
  match_pattern[3]=0xaa;match_pattern[4]=0xff;match_pattern[5]=0x00;
  match_pattern[6]=0x55;match_pattern[7]=0xaa;


/* set up access to the nirom */
  outportb(pb, ((unsigned __int64)&(pb->rp->de205_b_pir2)),0);
  outportb(pb, ((unsigned __int64)&(pb->rp->de205_b_pir1)),0);
  outportb(pb, ((unsigned __int64)&(pb->rp->de205_b_csr)),3);

/*Find the start of the ROM*/
    for(i=0;i<64;i++)
	nirom[i] = inportb(pb, (unsigned __int64)&(pb->rp->de205_b_ap));

    for(i=0;i<32;i++)
	{
	for(j=0;j<8;j++)
	    if(nirom[i+j]!=match_pattern[j])
		break;
	if(j==8)
	    break;
	}
    memcpy(ni,nirom+i+8,32);
}

/*+                      
 * ============================================================================
 * = de205_poll - de205 polling routine.                          	      =
 * ============================================================================
 *               
 * OVERVIEW:                                                                  
 *  
 *      This routine is the polling routine for the de205 driver.
 *	It is called from the timer task. It will simply check for
 *	an interrupt and call the isr if there is one.
 *           
 * FORM OF CALL:                                                 
 *  
 *  	de205_poll (pb);
 *              
 * RETURNS:
 *
 *   	ptr - Pointer to a copy of the ni station address rom.
 *                    
 * ARGUMENTS:                           
 *
 *	struct NI_GBL *np - pointer to ni block.
 *                          
-*/             

void de205_poll (struct NI_GBL *np)
{
struct DE205_PB	*pb;				/*Pointer to the port block*/	
unsigned short nicsr0;

/*Get some pointers*/
	pb = (struct DE205_PB*)np->pbp;
/*Read csr0*/
	nicsr0 = inportb(pb, ((unsigned __int64)&(pb->rp->de205_b_csr)));
/*If we have an interrupt call the isr*/
	if((nicsr0 & 4)||(nicsr0 & 8))
	{
	    de205_interrupt(np);
	}
}




/*+
 * ============================================================================
 * = de205_stop_driver - Stops the DE205 driver.                              =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will stop the DE205 driver.
 *
 * FORM OF CALL:
 *
 *	de205_stop_driver (ip); 
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

int de205_stop_driver(struct INODE *ip)
{
struct NI_GBL *np;		/*Pointer to the NI global database*/
struct DE205_PB *pb;		/*Pointer to the port block*/	


/*Get some pointers*/
	np = (struct NI_GBL*)ip->misc;
	pb = (struct DE205_PB*)np->pbp;

	if(pb->pb.mode==DDB$K_INTERRUPT) 
	{
  outportb(pb, ((unsigned __int64)&(pb->rp->de205_b_icr)),pb->irqval);
	io_disable_interrupts (pb, pb->pb.vector);
	}

/*If we are already stopped don't stop again*/
	if(pb->state == DE205_K_STOPPED)
	    return(msg_success);

/*If we are unitialized return a failure*/
	if(pb->state == DE205_K_UNINITIALIZED)
	    return(msg_failure);

/*Set the state flag*/
	pb->state = DE205_K_STOPPED;

/*Stop the lemac*/
  outportb(pb, ((unsigned __int64)&(pb->rp->de205_b_csr)),3);

/*Cleanup the msg rcved list*/
	de205_free_rcv_pkts(ip);

/*Return a success*/
	return(msg_success);
}


/*+
 * ============================================================================
 * = de205_restart_driver - restarts the driver.                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will restart the driver.
 *
 * FORM OF CALL:
 *
 *	de205_restart_driver (ip); 
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

int de205_restart_driver(struct INODE *ip)
{
struct NI_GBL *np;		/*Pointer to the NI global database*/
struct NI_ENV *ev;		/*Pointer to the environment variables*/
struct DE205_PB *pb;		/*Pointer to the port block*/	
unsigned short nicsr0;

/*Get some pointers*/
	np = (struct NI_GBL*)ip->misc;
	pb = (struct DE205_PB*)np->pbp;
	ev = (struct NI_ENV*)np->enp;

/*If we are already started don't start again*/
	if(pb->state == DE205_K_STARTED)
	    return(msg_success);

/*If we are unitialized return a failure*/
	if(pb->state == DE205_K_UNINITIALIZED)
	    return(msg_failure);

/*Set the environment variables*/
	de205_read_env(ev,ip->name);

/*Stop the lemac*/
        outportb(pb, ((unsigned __int64)&(pb->rp->de205_b_csr)),3);

	if(pb->pb.mode== DDB$K_INTERRUPT) /* just enable ints don't change irq */
	{
  outportb(pb, ((unsigned __int64)&(pb->rp->de205_b_csr)),0);
  outportb(pb, ((unsigned __int64)&(pb->rp->de205_b_icr)),0x80|pb->irqval);
		io_enable_interrupts (pb, pb->pb.vector);
	}
	else
	{
         outportb(pb, ((unsigned __int64)&(pb->rp->de205_b_csr)),0);
         outportb(pb, ((unsigned __int64)&(pb->rp->de205_b_icr)),pb->irqval);
	}

/*Init the transmit ring*/
	de205_init_tx (ip);


/*Set the state flag*/
	pb->state = DE205_K_STARTED;

/*Return a success*/
	return(msg_success);
}


/*+
 * ============================================================================
 * = de205_callback_write - DE205 callback write routine.                           =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine does a callback write it is a synchronous.
 *
 * FORM OF CALL:
 *
 *	de205_callback_write (fp, item_size, number, buf); 
 *                    
 * RETURNS:
 *
 *	int Status - msg_success or FAILURE.
 *
 * ARGUMENTS:
 *
 *      struct FILE *fp - Pointer to the file to be written.
 *	int item_size	- size of item
 *	int number	- number of items
 *	char *buf - Buffer to be written. 
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int de205_callback_write (struct FILE *fp,int item_size,int number,char *buf)
{
struct NI_GBL *np;			/*Pointer to NI global database*/	
struct DE205_PB *pb;			/*Pointer to the port block*/
struct NI_ENV *ev;			/*Pointer to environment variables*/
struct MOP$_V4_COUNTERS *mc;		/*Pointer to the MOP counters*/	
struct TIMERQ *ctt;			/*Callback frame timer*/
int i,size,status,tx_index_in,len, return_size;


/*Get the pointer to the port block*/
	np = (struct NI_GBL*)fp->ip->misc;
	pb = (struct DE205_PB*)np->pbp;
	ev = (struct NI_ENV*)np->enp;
	mc = (struct MOP$_V4_COUNTERS*)np->mcp;
/*Just dump the message if the driver is stopped*/
	if((pb->state == DE205_K_STOPPED) || (pb->state == DE205_K_UNINITIALIZED))
	    return(0);

/*Copy our source address*/
	memcpy(buf+6,pb->sa,6);


/*Get the length*/
	len = item_size*number;

/*Pad the buffer if its needed*/
	if(len < DE205_K_MIN_PKT_SIZE){
	    memset(buf+len,DE205_K_MIN_PKT_SIZE-len,0);
	    len = DE205_K_MIN_PKT_SIZE;
	}

/*First set the timer*/
        ctt = (struct TIMERQ*)malloc(sizeof(struct TIMERQ));
        set_timer(ctt,DE205_K_XMT_TIMEOUT);

/*Set the transmit flags*/
	tx_index_in = pb->tx_index_in;
	(pb->ti)[tx_index_in].sync_flag = DE205_K_TX_SYNC;
	(pb->ti)[tx_index_in].tx_timer = ctt;
/* do the write */
      	    size = de205_out(fp->ip,buf,len);

/*Send the packet*/
         return_size=size;
		/* success, but don't include extra padding in return size */
	 if (size!=0)
	    return_size = item_size*number;


/*Return the size*/
	return( return_size);
}

/*+
 * ============================================================================
 * = de205_free_rcv_pkts - Frees packets from the recieve list.               =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will free the packets and memory used for
 *	the received packet list.
 *
 * FORM OF CALL:
 *
 *	de205_free_rcv_pkts (ip); 
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

void de205_free_rcv_pkts(struct INODE *ip)
{
struct NI_GBL *np;		/*Pointer to NI global database*/	
struct DE205_PB *pb;		/*Pointer to the port block*/	
struct DE205_MSG *mr;		/*Pointer to the message recieved*/

/*Get the pointers to the datalink info*/
	np = (struct NI_GBL*)ip->misc;
	pb = (struct DE205_PB*)np->pbp;

/*Free all the messages on the preallocated list*/
	while(pb->rqh.flink != (struct DE205_MSG*)&(pb->rqh.flink))
	{
	/*Remove the packet*/
            spinlock(&spl_kernel);
	    mr = (struct DE205_MSG*)remq(pb->rqh.flink);
	    pb->rcv_msg_cnt--;
	    spinunlock(&spl_kernel);

	/*Free the packet and memory*/
	    ndl_deallocate_pkt(ip,mr->msg);
	    free(mr);

	}

}

/*+
 * ============================================================================
 * = de205_read - de205 port read routine.                                    =
 * ============================================================================
 *
 * OVERVIEW:         
 *  
 *      Used primarily for de205 port callbacks.
 *  
 * FORM OF CALL:
 *  
 *	de205_read (fp,size,number,c);
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

int de205_read (struct FILE *fp,int size,int number,unsigned char *c)
{
struct NI_GBL *np;			/*Pointer to NI global database*/	
struct DE205_PB *pb;			/*Pointer to the port block*/
struct DE205_MSG *mr;			/*Pointer to the message recieved*/
int len,retries;


/*Get the pointer to the port block*/
	np = (struct NI_GBL*)fp->ip->misc;
	pb = (struct DE205_PB*)np->pbp;

/*See if there is a message available*/
	for(retries=0; retries<DE205_CALLBACK_TIMEOUT; retries++)
	{
	    spinlock(&spl_kernel);
	    if(pb->rqh.flink != (struct DE205_MSG*)&(pb->rqh.flink))
	    {
	    /*Get the message*/
		mr = (struct DE205_MSG*)remq(pb->rqh.flink);
		pb->rcv_msg_cnt--;
		spinunlock(&spl_kernel);
		break;
	    }
	    else
	    /*There isn't so just unlock*/ 
		spinunlock(&spl_kernel);

	/*Wait a little*/
	    krn$_sleep(DE205_CALLBACK_WAIT);
	}
/*If there is no message available return 0*/
	if(retries == DE205_CALLBACK_TIMEOUT)
	    return (0);

/*Copy the info*/
	len = min(mr->size,size*number);
	memcpy(c,mr->msg,len);

/*Free the holder and packet*/
	ndl_deallocate_pkt(fp->ip,mr->msg);
	free(mr);

/*Return the size*/
	return(len);
}
