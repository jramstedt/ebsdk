/*
 * Copyright (C) 1995, 1997 by
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
 * Abstract:	IDE driver
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *	jwj	13-Dec-1999	Add a check for disk size of newer disks in dq_reset, 
 *				adjust cylinder count if necessary.  This accomodates IDE
 *				disks larger than 8.455GB.
 *	jwj	30-Aug-1999	Increase the waiting time for the drive to assert the
 *				DRQ bit.  This resolves a problem with Fujitsu disks
 *				when running exer at the console.
 *	dtr	4-Mar-1999	Correct potential problem with multi channels.
 *				IDE can only support one active command and the
 *				way the code was written did not take that in to
 *				account.  Added a spin lock at the command
 *				level.
 *
 *	dtr	5-Feb-1999	Added the necessary setup for zip drives.  They
 *				use 512 byte blocks an not 2048.
 *
 *	dtr	25-Aug-1998	Allocated a local buffer for the inq command,
 *				The scsi code did a request for inquiry data and
 *				a byte count of 1.
 *
 *	er	16-May-1997	Fixed atapi_do_send to process interrupt
 *				reasons 0 - 3.
 *
 *	er	23-Apr-1997	Fixed ATAPI support and shorten timeouts.
 *				Conditionalized for STARTSHUT/DRIVERSHUT model.
 *
 *	er	14-Jan-1997	Added ATAPI support based on Don Rice's
 *				IDE/ATAPI driver.
 *
 *	sfs	02-Oct-1995	Initial entry.
 */

#define rb(x) inportb((struct pb *)pb,pb->csr+x)
#define rb2(x) inportb((struct pb *)pb,pb->csr2+x)
#define rw(x) inportw((struct pb *)pb,pb->csr+x)
#define wb(x,y) outportb((struct pb *)pb,pb->csr+x,y)
#define ww(x,y) outportw((struct pb *)pb,pb->csr+x,y)
#define prefix(x) set_io_prefix(pb,name,x)

#include "cp$src:platform.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:msg_def.h"
#include "cp$src:common.h"
#include "cp$inc:prototypes.h"
#include "cp$src:eisa.h"
#include "cp$src:scsi_def.h"
#include "cp$src:pb_def.h"
#include "cp$src:dq_pb_def.h"
#include "cp$src:sb_def.h"
#include "cp$src:ub_def.h"
#include "cp$src:probe_io_def.h"
#include "cp$inc:platform_io.h"
#include "cp$inc:kernel_entry.h"
#if MODULAR
#include "cp$src:mem_def.h"
#endif

#if M1543C || M1535C
#include "cp$src:nt_types.h"
#define IDE_DEVICE_ID		MAKEVENID(PCI_VENDORID_ACER,PCI_DEVICEID_ACER_IDE)
#else
#define IDE_DEVICE_ID		0xC6931080
#endif

#define DQ_DEBUG 0

#if DQ_DEBUG
#define ddprintf(x)			qprintf x
#else
#define ddprintf(x)
#endif

#define ATAPI_MAGIC_1	0x14
#define ATAPI_MAGIC_2	0xEB

#define COMMAND_DEVICE_RESET 0x08
#define COMMAND_READ_SECTOR 0x20
#define COMMAND_WRITE_SECTOR 0x30
#define COMMAND_PACKET 0xA0
#define COMMAND_PIDENTIFY 0xA1
#define COMMAND_IDENTIFY 0xEC

#define STATUS_ERR 0x01
#define STATUS_IDX 0x02
#define STATUS_CORR 0x04
#define STATUS_DRQ 0x08
#define STATUS_SERV 0x10
#define STATUS_DF 0x20
#define STATUS_DRDY 0x40
#define STATUS_IDLE 0x50
#define STATUS_BSY 0x80

#define DATA 0x0
#define ERROR 0x1
#define FEATURES 0x1
#define SECTOR_COUNT 0x2
#define SECTOR 0x3
#define CYLINDER_LO 0x4
#define CYLINDER_HI 0x5
#define DRIVE_HEAD 0x6
#define STATUS 0x7
#define COMMAND 0x7
#define ALT_STATUS 0x2
#define DEVICE_CONTROL 0x2

#define CAPABILITY_DMA 0x100
#define CAPABILITY_LBA 0x200

#define MAX_CYLINDERS	16383
#define	MAX_SECTORS	63
#define MAX_HEADS	16
#define MAX_BLOCKS_PER_CYLINDER 1008

/*	 
** Define the SCSI sense codes that need to looked at and generated.
*/	 

#define scsi_k_no_sense 0
#define scsi_k_recovered_error 1
#define scsi_k_not_ready 2
#define scsi_k_medium_error 3
#define scsi_k_hardware_error 4
#define scsi_k_illegal_request 5
#define scsi_k_unit_attention 6
#define scsi_k_data_protect 7
#define scsi_k_blank_check 8
#define scsi_k_unique 9
#define scsi_k_copy_aborted 10
#define scsi_k_aborted_command 11
#define scsi_k_equal 12
#define scsi_k_vol_overflow 13
#define scsi_k_miscompare 14
#define scsi_k_reserved 15

struct SEMAPHORE ide_lock;
unsigned char dq_buf[2048];
int dq_buf_lbn;
int dq_buf_sb;

extern null_procedure( );
extern struct FILE *el_fp;
#if !( STARTSHUT || DRIVERSHUT )
extern int scsi_poll;
#endif

#if MODULAR
extern int diagnostic_mode_flag;
extern struct DDB *dq_ddb_ptr;
#endif

#if !( STARTSHUT || DRIVERSHUT )
int dk_read( );
int dk_write( );
int dq_open( );
int dk_close( );

struct DDB dq_ddb = {
	"dq",			/* how this routine wants to be called	*/
	dk_read,		/* read routine				*/
	dk_write,		/* write routine			*/
	dq_open,		/* open routine				*/
	dk_close,		/* close routine			*/
	null_procedure,		/* name expansion routine		*/
	null_procedure,		/* delete routine			*/
	null_procedure,		/* create routine			*/
	null_procedure,		/* setmode				*/
	null_procedure,		/* validation routine			*/
	0,			/* class specific use			*/
	1,			/* allows information			*/
	0,			/* must be stacked			*/
	0,			/* is a flash update driver		*/
	1,			/* is a block device			*/
	0,			/* not seekable				*/
	0,			/* is an ethernet device		*/
	0,			/* is a filesystem driver		*/
};
#endif

void dq_send_inquiry( struct ata_sb *sb );
int dq_identify( struct dq_pb *pb, unsigned int unit, unsigned short *id );
int dq_wait_on_busy( struct dq_pb *pb );
int dq_long_wait_on_busy( struct dq_pb *pb );
int dq_wait_for_drq( struct dq_pb *pb );

int dq_command( struct ata_sb *sb, int lun, unsigned char *cmd, int cmd_len,
	unsigned char *dat_out, int dat_out_len,
	unsigned char *dat_in, int dat_in_len, unsigned char *sts );


#if STARTSHUT

/*+
 * ============================================================================
 * = dq_setmode - change driver state				              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * 	This routine changes the state of the port driver.  The following
 * 	states are relevant:
 *
 *	    DDB$K_STOP -
 *			Suspends the  port driver.  dq_poll stops checking
 *			things in this state, and interrupts from the
 *			device are dismissed.
 *
 *	    DDB$K_RESTARTABLE -
 *			Same as DDB$K_STOP, except the port driver may be
 *			restarted dynamically by the class driver.
 *
 *	    DDB$K_START -
 *			Changes the   port   driver   to   interrupt  mode.
 *			dq_poll is once again enabled, as well as device
 *			interrupt handling.
 *
 *	    DDB$K_INTERRUPT -
 *			Same as DDB$K_START.
 *  
 *	    DDB$K_ASSIGN  -	
 *			Perform system startup functions.
 *
 * FORM OF CALL:
 *  
 *	dq_setmode(mode, dev)
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
 *	struct device *dev - pointer to device information
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/
int dq_setmode(int mode, struct device *dev)
{
    int i, j, k, t;
    struct dq_pb *pb;
    struct ata_sb *sb;

    switch (mode) {
	case DDB$K_ASSIGN: 
	    break;

	case DDB$K_START: 

	case DDB$K_INTERRUPT: 
	    pb = (struct dq_pb *) dev->devdep.io_device.devspecific;

	    if (pb) {
		pb->pb.ref++;
		return msg_success;
	    }

	    if (dev->flags)
		return msg_failure;

	    krn$_set_console_mode(0);

	    break;	    

	case DDB$K_STOP: 
	    pb = (struct dq_pb *) dev->devdep.io_device.devspecific;

	    if (!pb)
		return msg_failure;

	    pb->pb.ref--;
	    if (pb->pb.ref != 0)
		return msg_success;


	    dq_reset(pb);

/* Free all vectors */

	    int_vector_clear(pb->pb.vector);

/* Free all semaphores */

	    krn$_semrelease( &pb->owner_s );

/* Free all dynamic memory */

	    for( i = 0; i < 2; i++ )
		if (pb->pb.sb[i])
		    free(pb->pb.sb[i]);
	    free(pb->pb.sb);
	    free(pb);

	    dev->devdep.io_device.devspecific = 0;

	    break;

    }					/* switch */

    return msg_success;

}					/* dq_setmode */

/*+
 * ============================================================================
 * = dq_poll_units -							      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *  
 * FORM OF CALL:
 *  
 *	dq_poll_units(dev, verbose)
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct device *dev	- Pointer to device structure of adapter.
 *	int verbose		- if set, print node information.
 *
 * SIDE EFFECTS:
 *
 *	Files will be created.
 *
-*/
int dq_poll_units(struct device *dev, int verbose)
{
    if (dq_initialize(dev, TRUE) != msg_success)
	return msg_failure;
}

/*+
 * ============================================================================
 * = dq_establish_connection - establish a connection to given unit           =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine establishes a connection to a given unit.
 *  
 * FORM OF CALL:
 *  
 *	dq_establish_connection( inode, dev, class )
 *  
 * RETURNS:
 *
 *	success or failure.
 *       
 * ARGUMENTS:
 *
 *	struct INODE *inode	- pointer to inode.
 *	struct device *dev	- pointer to device block
 *	int class		- class of connection
 *
 * SIDE EFFECTS:
 *
 *	Connection established to given unit.
 *
-*/
int dq_establish_connection(struct INODE *inode, struct device *dev, int class)
{

/* diagnostic mode, node already configured */

    if (diagnostic_mode_flag)
	return msg_success;

/* if device not in file system, fail */

    if (!inode->inuse) {
	return msg_failure;
    }

    if (dq_initialize(dev, FALSE) != msg_success)
	return msg_failure;
}

int dq_initialize(struct device *dev, int verbose )
    {

    struct dq_pb *pb;

    pb = malloc_noown(sizeof(*pb));

    /* load pb into device */

    dev->devdep.io_device.devspecific = (void *) pb;

    /* Load the vector from device to pb */

    pb->pb.vector = 0;

#if 0
    pb->pb.vector = pci_get_vector(dev);
#endif

    /* Load the base address from the device to pb */

    pb->pb.hose = dev->hose;
    pb->pb.slot = dev->slot;
    pb->pb.bus = dev->bus;
    pb->pb.function = dev->function;
    pb->pb.channel = dev->channel;
    pb->pb.controller = dev->devdep.io_device.controller;
    pb->pb.config_device = dev;
    pb->pb.protocol = dq_ddb_ptr->name; 

    if ( pb->pb.bus == 1 )
	pb->pb.type = TYPE_EISA;
    else
	pb->pb.type = TYPE_PCI;

    /*
     * Using the state filed in the pb to turn on printing of the inode.
     */

    if ( verbose )
	pb->pb.state = TRUE;

    
    io_disable_interrupts( pb, pb->pb.vector );

    if ( platform() == ISP_MODEL ) {
	pprintf("IDE controller is not emulated\n");
	return msg_failure;
    }

    pb->pb.ref++;

    dq_init_port( pb );
    return msg_success;
    }
#endif

#if !( STARTSHUT || DRIVERSHUT )
int dq_init( )
    {
    int dq_init_pb( );

    krn$_seminit( &ide_lock, 1, "dq_owner" );

    find_pb( "dq", sizeof( struct dq_pb ), dq_init_pb );

    return( msg_success );
    }

void dq_init_pb( struct dq_pb *pb )
    {
    int dq_init_port( );

    log_driver_init( pb );
    krn$_create( dq_init_port, null_procedure, 0, 5, 0, 0,
	    "dq_init", "nl", "r", "nl", "w", "nl", "w", ( struct pb * )pb, NULL, NULL, NULL );
    }
#endif

struct identify {
    U_INT_16	config;			/* Configuration information */
    U_INT_16	cylinders;		/* Number of cylinders */
    U_INT_16	rsvd2;			/* Reserved word */
    U_INT_16	heads;			/* Number of heads */
    U_INT_16	ubytes_track;		/* Unformatted bytes/track */
    U_INT_16	ubytes_sector;		/* Unformatted bytes/sector */
    U_INT_16	sectors;		/* Number of sectors */
    U_INT_16	unique7[3];		/* Vendor unique */
    char	serial_number[20];	/* ASCII serial number */
    U_INT_16	buffer_type;		/* Buffer type */
    U_INT_16	buffer_size_blocks;	/* Buffer size (in blocks) */
    U_INT_16	ecc_bytes;		/* Number of ECC bytes/sector */
    char	fw_version[8];		/* ASCII firmware revision */
    char	model_number[40];	/* ASCII drive model */
    U_INT_8	rw_multiple;		/* Number of sectors/interrupt */
    U_INT_8	unique47;    		/* Vendor unique */
    U_INT_16	dblword_io;		/* Doubleword I/O flag */
    U_INT_16	capabilities;		/* Capabilities */
    U_INT_16	rsvd50;			/* Reserved */
    U_INT_16	pio_cycle;		/* Programmed I/O cycle times */
    U_INT_16	dma_cycle;		/* DMA I/O cycle times */
    U_INT_16	valid54_58;		/* Valid bit for next 4 fields */
    U_INT_16	curr_cyls;		/* 1) Current cylinder count */
    U_INT_16	curr_heads;		/* 2) Current head count */
    U_INT_16	curr_sectors;		/* 3) Current sector count */
    int		max_sectors;		/* 4) Maximum sector number */
    U_INT_16	multiple_sectors;	/* Current sectors/interrupt setting */
    int		lba_maxblock;		/* LBA mode maximum block number */
    U_INT_16	single_word_dma;	/* Single word DMA info */
    U_INT_16	multi_word_dma;		/* Multi word DMA info */
    U_INT_8	rsvd64[64];		/* Reserved */
    U_INT_8	unique128[32];		/* Vendor unique */
    U_INT_8	rsvd160[96];		/* Reserved */
    U_INT_8	fill[192];
    } ;


void dq_interrupt( struct dq_pb *pb )
    {
    io_disable_interrupts( ( struct pb * )pb, pb->pb.vector );
    }

/*+
 * ============================================================================
 * = dq_init_port - initialize a given port                                   =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine initializes the port driver data structures and the
 *	port hardware of a specific, given port.
 *  
 * FORM OF CALL:
 *  
 *	dq_init_port ( pb );
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct dq_pb *pb    - port block
 *
 * SIDE EFFECTS:
 *
 *	A port block is initialized for each instance of port
 *	hardware of the appropriate type.
 *
-*/
void dq_init_port( struct dq_pb *pb )
    {
    char name[32];
#if !( STARTSHUT || DRIVERSHUT )
    int dq_setmode( );
    int dq_poll( );
#endif
    int dq_interrupt( );

#if M1543C || M1535C
	M5229Initialize(pb);
#endif

#if ( STARTSHUT || DRIVERSHUT )
    pb->pb.ref++;
#endif    

#if !( STARTSHUT || DRIVERSHUT )
    pb->pb.setmode = dq_setmode;
#endif
    if( pb->pb.vector )
	{
	pb->pb.mode = DDB$K_INTERRUPT;
	pb->pb.desired_mode = DDB$K_INTERRUPT;
	int_vector_set( pb->pb.vector, dq_interrupt, pb, 0 );
	}
    else
	{
	pb->pb.mode = DDB$K_POLLED;
	pb->pb.desired_mode = DDB$K_POLLED;
	}
    set_io_name( pb->pb.name, 0, 0, 0, ( struct pb * )pb );
    strcpy( pb->pb.name + 3, pb->pb.name + 4 );
    pb->pb.sb = ( struct ata_sb ** )malloc_noown( 2 * sizeof( struct ata_sb * ) );
    pb->pb.num_sb = 2;
    krn$_seminit( &pb->owner_s, 1, "dq_owner" );

    dq_reset( pb );
    fprintf( el_fp, "port %s initialized\n", pb->pb.name );
    pb->poll_active = 1;
#if !( STARTSHUT || DRIVERSHUT )
    krn$_create( dq_poll, 0, 0, 5, 0, 4096,
	    prefix( "_poll" ), "nl", "r", "nl", "w", "nl", "w", pb, 1 );
#endif
    }

/*+
 * ============================================================================
 * = dq_reset - reset the port hardware                                       =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine resets the port hardware.  It initializes it completely
 *	so that further operations may take place.
 *  
 * FORM OF CALL:
 *  
 *	dq_reset( pb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct dq_pb *pb		- port block
 *
 * SIDE EFFECTS:
 *
 *	The port hardware may be initialized and made ready for action.
 *
-*/
void dq_reset( struct dq_pb *pb )
{
    unsigned int i;
    struct ata_sb *sb;
    struct identify *id;

    if( pb->pb.type == TYPE_PCI )
	dq_init_ide( pb );
    else
	{
	pb->csr = pb->pb.csr;
	pb->csr2 = pb->pb.csr + 0x204;
	}
    /*
     *  Statically allocate an SB for each possible node connected to this
     *  port.  Fill in each SB.
     */
    for ( i = 0; i < pb->pb.num_sb; i++ ) {
	sb = ( struct ata_sb * )malloc_noown( sizeof( *sb ) );
	pb->pb.sb[i] = sb;
	sb->pb = pb;
	sb->ub.flink = &sb->ub.flink;
	sb->ub.blink = &sb->ub.flink;
	sb->command = dq_command;
	sb->send_inquiry = dq_send_inquiry;
	sb->ok = 1;
	sb->node_id = i;
	sb->atapi = 0;
	set_io_name( sb->name, 0, 0, sb->node_id, pb );
	sb->identify = ( struct identify * )malloc_noown( sizeof( struct identify ) );

#if DQ_DEBUG
	ddprintf(("Indentifying unit %d\n",sb->node_id));
#endif

	if ( dq_identify( pb, sb->node_id, ( unsigned short * )sb->identify ) != msg_success ) {
#if DQ_DEBUG
	    ddprintf(("Units not found\n"));
#endif
	    free( sb->identify );   
	    sb->identify = ( void * )0;
	}
	else
	{
#if DQ_DEBUG
	    ddprintf(("Units found\n"));
#endif
#if M1543C || M1535C
	    M5229Configure(pb,pb->pb.channel,sb->node_id,sb->identify);
#endif
#if DQ_DEBUG
	    hd_helper( el_fp, 0, ( unsigned int )sb->identify, sizeof( struct identify ), 2, 0 );
#endif
	}
	id = ( struct identify * )sb->identify;
	if( id->cylinders == MAX_CYLINDERS
	   && id->heads == MAX_HEADS
	   && id->sectors == MAX_SECTORS
	   && id->lba_maxblock != 0 ){
           /* if cylinders = 16383, sectors = 63, heads = 16,*/
	   /* and lba_maxblock is non-zero, use new method for calculating */
           /* size and adjust it here */
	   id->cylinders = (id->lba_maxblock/MAX_BLOCKS_PER_CYLINDER) + ((id->lba_maxblock%MAX_BLOCKS_PER_CYLINDER)!= 0);
	 }  

    }
}

/*+
 * ============================================================================
 * = dq_init_ide - initialize the IDE hardware                                =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine initializes the IDE hardware.  It walks through the
 *	necessary steps to bring an IDE port online.
 *  
 * FORM OF CALL:
 *  
 *	dq_init_ide( pb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct dq_pb *pb		- port block
 *
 * SIDE EFFECTS:
 *
 *	The IDE controller is now ready for operation.
 *
-*/
void dq_init_ide( struct dq_pb *pb )
{
    unsigned int id;
    unsigned char progif;
    unsigned short class;

/*
** Read the programming interface field of the class code register,
** located in the PCI configuration header, to determine PCI native 
** or legacy mode operation.
*/
#if TINOSA
	progif = 0x04;
#else
    progif = incfgb( ( struct pb * )pb, 0x09 );
#endif
/*
** Channel is used to distinguish between primary (pb->channel = 0)
** and secondary (pb->channel = 1) IDE interfaces.
*/
    if ( pb->pb.channel ) {
	if ( progif & 0x04 ) {
	    pb->csr = incfgl( ( struct pb * )pb, 0x18 ) & ~3; 
	    pb->csr2 = incfgl( ( struct pb * )pb, 0x1C ) & ~3; 
	}
	else {
	    pb->csr = 0x170;
	    pb->csr2 = 0x374;
	}
    }    
    else {
	if ( progif & 0x01 ) {
	    pb->csr = incfgl( ( struct pb * )pb, 0x10 ) & ~3;
	    pb->csr2 = incfgl( ( struct pb * )pb, 0x14 ) & ~3;
	}
	else {
	    pb->csr = 0x1F0;
	    pb->csr2 = 0x3F4;
	}
    }
/*
** Force Cypress CY82C693/ACER IDE functions to work in ISA legacy mode.  
** Note that the Cypress PCI IDE Interrupt Request Routing Registers 
** (Register 21 at offset 0x4B and Register 22 at offset 0x4C in PCI
** config space) must be programmed to 0x80, disabling interrupt routing.
*/
    id = incfgl( ( struct pb * )pb, 0x00 );
    class = incfgw( ( struct pb * )pb, 0x0A );
    if ( ( (int)id == IDE_DEVICE_ID ) && ( class == 0x0101 ) ) {
	if ( pb->pb.function == 1 ) {
	    pb->csr = 0x1F0;
	    pb->csr2 = 0x3F4;
	    outcfgl( ( struct pb * )pb, 0x10, 0x1F0 );
	    outcfgl( ( struct pb * )pb, 0x14, 0x3F6 );
	}
	else if ( pb->pb.function == 2 ) {
	    pb->csr = 0x170;
	    pb->csr2 = 0x374;
	    outcfgl( ( struct pb * )pb, 0x10, 0x170 );
	    outcfgl( ( struct pb * )pb, 0x14, 0x376 );
	}
    }
}

/*+
 * ============================================================================
 * = dq_new_unit - make a unit known to the system                            =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds the data structures necessary to make an IDE
 *	unit known to the system.
 *  
 * FORM OF CALL:
 *  
 *	dq_new_unit( pb, lun, inq )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct dq_pb *pb		- port block
 *	int lun				- LUN
 *	unsigned char *inq		- pointer to device data
 *
 * SIDE EFFECTS:
 *
 *	A unit block is allocated, and an INODE is initialized.
 *
-*/
void dq_new_unit( struct ata_sb *sb, int lun, unsigned char *inq )
{
    char controller[MAX_NAME];
    char name[32];
    char *np;
    int inq_len;
    int i;
    int t;
    struct DDB *driver;
    struct pb *pb;
    struct ub *ub;
    struct INODE *inode;

    pb = ( struct pb * )sb->pb;
    if( lun )
	return;
    if( sb->state == sb_k_stall )
	return;
    /*
     *  Flag that we've successfully talked to this node.
     */
    sb->state = sb_k_open;
    if ( scsi_lun_map( sb, GET_LUN, lun ) ) 
      return;
    scsi_lun_map( sb, SET_LUN, lun );
    /*
     *  See if this device is accessible.  We will accept 0 (device currently
     *  connected) only.  We could accept 1 (target capable of supporting
     *  device) but there's no real reason to do so, and in additon, the rest
     *  of the inquiry data is bogus in this case.
     */
    t = inq[0] & 0xe0;
    if( t )
	return;
    /*
     *  Check the device type field.  We will accept 0 (random access),
     *  1 (sequential access), 4 (write-once), 5 (read-only), or 7 (optical).
     */
    t = inq[0] & 0x1f;
    switch( t )
	{
	/*
	 *  Sequential access devices use MK; all others use DK.
	 */
	case 0:
	case 4:
	case 5:
	case 7:
	    driver = &dq_ddb;
	    sb->ok = 1;
	    break;

	default:
	    if( sb->ok )
		fprintf( el_fp, msg_scsi_ill_dev_typ, t, sb->name );
	    sb->ok = 0;
	    return;
	}
    inq_len = inq[4] + 5;
    /*
     *  Create a new "file".
     */
    set_io_name( name, driver->name, sb->node_id, sb->node_id, pb );
    /*
     * If we fail to get a name, then the port driver does not want
     * an inode made out of this information.  Think about if I want it
     * out of the lun map or not.  The below if statement may not be
     * needed at all.  
     */
    if ( name[0] == 0 ) {
      scsi_lun_map( sb, CLEAR_LUN, lun ); 
      return;
    }
    t = allocinode( name, 1, &inode );
    inode->dva = driver;
    inode->attr = ATTR$M_READ;
    inode->bs = 512;
    /*
     *  If the file already exists, then a unit block is already allocated;
     *  otherwise, we must allocate one.
     */
    if( t == 1 )
	ub = inode->misc;
    else
	{
	ub = malloc_noown( sizeof( *ub ) );
	ub->inq = malloc_noown( MAX_INQ_SZ );
	}
    ub->inq_len = umin( inq_len, MAX_INQ_SZ );
    memcpy( ub->inq, inq, ub->inq_len );
    ub->sb = sb;
    ub->pb = pb;
    ub->incarn = krn$_unique_number( );
    ub->unit = lun;
    ub->flags = ( inq[1]<<8 ) + inq[0] + 65536;
    /*
     *  Convert the space-padded vendor ID and product ID fields into
     *  a normal 0-terminated string.
     */
    np = ub->info;
    if( memcmp( &inq[8], "Compaq ", 7 ) == 0 ) {
	memcpy( np, &inq[8], 7 );
	np += 7;
	*np++ = ' ';
	memcpy( np, &inq[15], 17 );
	np += 16;
	for( i = 0; i < 16; i++, np-- )
	   if( np[-1] != ' ' )
	      break;
    } else {
    if( memcmp( &inq[8], "DEC ", 4 ) == 0 ) {
	memcpy( np, &inq[16], 16 );
	for( i = 0; i < 16; i++, np++ )
	   if( np[0] == ' ' )
	      break;
    } else {
	memcpy( np, &inq[8], 8 );
	np += 8;
	for ( i = 0; i < 8; i++, np-- )
	   if( np[-1] != ' ' )
	      break;	   
	if( inq[16] != ' ' )
	   *np++ = ' ';
	memcpy( np, &inq[16], 16 );
	np += 16;
	for( i = 0; i < 16; i++, np-- )
	   if( np[-1] != ' ' )
	      break;
	}
    }
    *np++ = 0;
    sprintf( ub->version, "%8.8s", &inq[32] );
    set_io_alias( ub->alias, driver->name, sb->node_id, pb );
    /*
     *  Set up required pointers and back-pointers.  The INODE points to the
     *  UB, the UB points to the SB, the SB points to the UB.
     */
    inode->misc = ub;
    ub->inode = inode;
    if( !sb->first_ub )
	sb->first_ub = ub;
    insq( ub, sb->ub.blink );
    INODE_UNLOCK( inode );
#if ( STARTSHUT || DRIVERSHUT )
    sprintf( ub->string, "%-18s %-24s %5s  %s",
	    name, ub->alias, ub->info, ub->version );
#else
    sprintf( ub->string, "%-24s   %-8s   %24s  %s",
	    name, ub->alias, ub->info, ub->version );
#endif
    insert_ub( ub );
#if !( STARTSHUT || DRIVERSHUT )
    fprintf( el_fp, msg_dev_found, inode->name, ub->info, sb->name );
#endif
    ub->flags -= 65536;
#if ( STARTSHUT || DRIVERSHUT )
    if( sb->verbose )
	printf( "%-18s %-24s %5s %5s\n", ub->inode->name, ub->alias, ub->info, ub->version );
#endif
}

/*+
 * ============================================================================
 * = dq_open - open a file                                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine opens a file.  A "file" is a raw device.  "Files" are 
 *	created and deleted as device units come and go.  Reference counts 
 *	are kept in the unit block (UB) in order to prevent UB deletion while 
 *	files are still open.  This routine may return failure if the underlying 
 *	unit is no longer valid.
 *  
 * FORM OF CALL:
 *  
 *	dq_open( fp )
 *  
 * RETURN CODES:
 *
 *	msg_success			- success
 *	msg_bad_inode			- the INODE is not in use now
 *       
 * ARGUMENTS:
 *
 *	struct FILE *fp			- pointer to the file to be opened
 *
 * SIDE EFFECTS:
 *
 *	Fields in various structures are modified.
 *
-*/
int dq_open( struct FILE *fp, char *info, char *next, char *mode )
    {
    dq_buf_lbn = -1;
    dq_buf_sb = -1;
    return( dk_open( fp, info, next, mode ) );
    }

int dq_send_read_write( struct ub *ub, unsigned char *c, int len, int lbn,
	int command )
    {
    int t;

    while( len > 0 )
	{
	t = dq_send_read_write_sub( ub, c, lbn, command );
	if( t != msg_success )
	    return( t );
	c += 512;
	len -= 512;
	lbn++;
	}
    return( msg_success );
    }

int dq_send_read_write_sub( struct ub *ub, unsigned char *c, int lbn, int command )
    {
    unsigned short *p;
    int i;
    int t;
    int sector;
    int head;
    int cylinder;
    struct ata_sb *sb;
    struct dq_pb *pb;
    struct identify *id;

    sb = ( struct ata_sb * )ub->sb;
    pb = ( struct dq_pb * )sb->pb;
    id = ( struct identify * )sb->identify;
    if( id->capabilities & CAPABILITY_LBA )
	{
	sector = ( lbn >> 0 ) & 0xff;
	cylinder = ( lbn >> 8 ) & 0xffff;
	head = ( ( lbn >> 24 ) & 0xf ) + 0x40;
	}
    else
	{
	sector = lbn % id->sectors + 1;
	t = lbn / id->sectors;
	cylinder = t / id->heads;
	head = t % id->heads;
	}
    t = msg_failure;
    if ( dq_wait_on_busy( pb ) == msg_success )
	{
	wb( DRIVE_HEAD, 0xA0 | head | ( sb->node_id << 4 ) );
	if ( dq_wait_on_busy( pb ) == msg_success ) 
	    {
	    wb( FEATURES, 0 );
	    wb( SECTOR_COUNT, 1 );
	    wb( SECTOR, sector );
	    wb( CYLINDER_LO, ( cylinder >> 0 ) & 0xff );
	    wb( CYLINDER_HI, ( cylinder >> 8 ) & 0xff );
	    p = ( unsigned short * )c;
	    wb( COMMAND, command );
	    if ( dq_wait_for_drq( pb ) == msg_success )
		{
		for( i = 0; i < 512; i += 2 )
		if( command == COMMAND_WRITE_SECTOR )
		{
		    ww( DATA, *p++ );
		    krn$_micro_delay( 1 );
		}
		else
		    *p++ = rw( DATA );
		if ( dq_wait_on_busy( pb ) == msg_success )
		    {
		    t = msg_success;
		    }
		}
	    }
	}
    return( t );
    }

#if !( STARTSHUT || DRIVERSHUT )
/*+
 * ============================================================================
 * = dq_poll - poll to see which nodes exist                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine performs polling, in order to get an accurate picture
 *	of which nodes exist and which don't.  Polling periodically is a way
 *	of detecting nodes coming and going.
 *  
 * FORM OF CALL:
 *  
 *	dq_poll( pb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct dq_pb *pb		- port block
 *
 * SIDE EFFECTS:
 *
 *	Nodes may be found.  Nodes may be lost.
 *
-*/
void dq_poll( struct dq_pb *pb, int poll )
{
    char name[32];
    unsigned int i;

    /*
     *  Poll all nodes as a group, pause for a while and repeat forever.
     */
    for ( i = 0; i < pb->pb.num_sb; i++ ) {
	if( pb->pb.mode == DDB$K_STOP )
	    continue;
	if ( poll || scsi_poll )
	    dq_send_inquiry( pb->pb.sb[i] );
    }
    pb->poll_active = 0;
    krn$_create_delayed( 30000, 0, dq_poll, 0, 0, 5, 0, 4096,
	    prefix( "_poll" ), pb, 0 );
}

void dq_unix_reset(struct dq_pb *pb)
{

    krn$_wait( &ide_lock );
    wb( DRIVE_HEAD, 0xB0);   /* drive select */
    wb( COMMAND, 00);        /* nop to the slave.  Should force it  */
    krn$_micro_delay(3000);         /* just in case */
    wb( DRIVE_HEAD, 0xA0);    /* master */
    wb( COMMAND, COMMAND_DEVICE_RESET);
    krn$_micro_delay(3000);
    krn$_post( &ide_lock );

#if M1535C || M1543C
	dq_wait_for_drq( pb );				/* Wait for the device to finish resetting. */
#endif

}


void dq_setmode( struct dq_pb *pb, int mode )
    {
    switch( mode )
	{
	case DDB$K_STOP:
	    if( pb->pb.mode != DDB$K_STOP )
		{
		krn$_wait( &pb->owner_s );
		if( pb->pb.mode == DDB$K_INTERRUPT )
		    io_disable_interrupts( pb, pb->pb.vector );
		( void )rb( STATUS );
		pb->pb.mode = DDB$K_STOP;
		}
	    break;

	case DDB$K_START:
	    if( pb->pb.mode == DDB$K_STOP )
		{
		pb->pb.mode = pb->pb.desired_mode;
#if 1
		/*	 
		**   this is required to allow the unix driver to work properly.
		**   the post was moved up to allow the reset to work.
		*/	 
		dq_unix_reset( pb );
#endif
		( void )rb( STATUS );
		krn$_post( &pb->owner_s );
		}
	    break;

	case DDB$K_READY:
	    if( pb->pb.mode != DDB$K_STOP )
		{
		while( pb->poll_active )
		    krn$_sleep( 100 );
		}
	    break;	    

#if 0
	case DDB$K_POLLED:
	    if( pb->pb.mode == DDB$K_STOP )
		{
		pb->pb.mode = DDB$K_POLLED;
		krn$_post( &pb->owner_s );
		dq_reset( pb );
		}
	    break;
#endif
	}
    }
#endif

int dq_wait_on_busy( struct dq_pb *pb )
    {
    unsigned int i;

    for ( i = 0; i < 5000; i++ ) {
	if ( rb2( ALT_STATUS ) & STATUS_BSY ) {
	    krn$_micro_delay( 100 );
	    continue;
	}
	else {
	    return( msg_success );
	}
    }
    return( msg_failure );
    }

int dq_wait_for_drq( struct dq_pb *pb )
    {
    unsigned int i;
    unsigned char t;

    for ( i = 0; i < 5000; i++ ) {
	t = rb2( ALT_STATUS );
	if ( t & STATUS_BSY ) {
	    krn$_micro_delay( 100 );
	}
	else if ( t & STATUS_DRQ ) {
	    return( msg_success );
	}
	else {
	    krn$_micro_delay( 200 );
	}
    }
    return( msg_failure );
    }

int dq_long_wait_on_busy( struct dq_pb *pb )
    {
    unsigned int i;

    for ( i = 0; i < 310000; i++ ) {
	if ( rb2( ALT_STATUS ) & STATUS_BSY ) {
	    krn$_micro_delay( 100 );
	    continue;
	}
	else {
	    return( msg_success );
	}
    }
    return( msg_failure );
    }

void map_error( unsigned char *status, unsigned char error )
{
    switch ( error >> 4 ) {
    	case scsi_k_no_sense:
	case scsi_k_not_ready:
	case scsi_k_medium_error:
	case scsi_k_hardware_error:
	case scsi_k_illegal_request:
	case scsi_k_unit_attention:
	case scsi_k_data_protect:
	case scsi_k_blank_check:
	case scsi_k_aborted_command:
	    dq_buf_lbn = -1;
	    dq_buf_sb = -1;
	    status[0] = scsi_k_sts_check;
	    break;
	case scsi_k_recovered_error:
	    status[0] = scsi_k_sts_good;
	    break;
    	default:
	    status[0] = scsi_k_sts_good;
    }
}

/*+
 * ============================================================================
 * = atapi_do_send - send an ATAPI packet command                             =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine sends an ATAPI packet command to a remote node.  
 *  
 * FORM OF CALL:
 *  
 *	atapi_do_send( sb, cmd, cmd_len, dat_out, dat_out_len,
 *		       dat_in, dat_in_len, sts )
 *  
 * RETURN CODES:
 *
 *	msg_success			- success
 *	msg_failure			- failed to send the command
 *
 * ARGUMENTS:
 *
 *	struct ata_sb *sb		- system block
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
 *	A command may be sent to an ATAPI device.
 *
-*/
int atapi_do_send( struct ata_sb *sb, unsigned char *cmd, int cmd_len,
		   unsigned char *dat_out, int dat_out_len,
		   unsigned char *dat_in, int dat_in_len, unsigned char *sts )
{
    struct dq_pb *pb;
    unsigned short *p;
    unsigned int i, j, nbytes;
    unsigned char reason;

    pb = ( struct dq_pb * )sb->pb;

    if ( dq_wait_on_busy( pb ) == msg_success ) {
/*
** Select the target device.
*/
	wb( DRIVE_HEAD, 0xA0 | ( sb->node_id << 4 ) );
	if ( dq_wait_on_busy( pb ) == msg_success ) {
	    wb( FEATURES, 0 );
	    if ( dat_in_len ) {
		wb( CYLINDER_HI, ( dat_in_len >> 8 ) & 0xFF );
		wb( CYLINDER_LO, dat_in_len & 0xFF );
	    }
	    else if ( dat_out_len ) {
		wb( CYLINDER_HI, ( dat_out_len >> 8 ) & 0xFF );
		wb( CYLINDER_LO, dat_out_len & 0xFF );
	    }
/*
** Write the PACKET command code to the Command register
** and wait for the device to accept the command.
*/
	    wb( COMMAND, COMMAND_PACKET );
	    krn$_sleep( 10 );
	    for ( j = 0;  j < 100;  j++ ) {
		if ( dq_wait_on_busy( pb ) == msg_success ) {
/*
** Some devices assert INTRQ at this point so read the
** Status register to clear the interrupt.
*/
		    ( void )rb( STATUS );
		    if ( ( reason = rb( SECTOR_COUNT ) & 0x03 ) == 0x3 ) {
/*
** Interrupt reason 3 signals command completion.
*/
			if ( rb( STATUS ) & 1 ) {
			    map_error( sts, rb( ERROR ) );
			}
			return( msg_success );
		    }
		    else if ( dq_wait_for_drq( pb ) == msg_success ) {
			nbytes = ( unsigned int )( 
				    ( rb( CYLINDER_HI ) << 8 ) | 
				      rb( CYLINDER_LO ) );
			switch ( reason ) {
			    case 0x0:
				p = ( unsigned short * )dat_out;
				for ( i = 0;  i < nbytes;  i += 2 ) {
				    ww( DATA, *p++ );
				    krn$_micro_delay( 1 );
				}
				break;
			    case 0x1:
				p = ( unsigned short * )cmd;
				for ( i = 0;  i < 12;  i += 2 ) {
				    if( i < cmd_len ) {
					ww( DATA, *p++ );
				    } else {
					ww( DATA, 0 );
				    }
				}
				krn$_sleep( 1 );
				break;
			    case 0x2:
				p = ( unsigned short * )dat_in;
				for ( i = 0;  i < nbytes;  i += 2 ) {
				    *p++ = rw( DATA );
				}
				break;
			    default:
				fprintf( el_fp, "atapi_do_send: Unknown interrupt reason %d\n", reason ); 
				break;
			}
		    }
		    else {
#if DQ_DEBUG
			fprintf( el_fp, "atapi_do_send: Failed, status = %2x, error =%2x\n", 
				 rb2( ALT_STATUS ), rb( ERROR ) );
#endif
			break;
		    }
		}
	    }
	}
    }
    return( msg_failure );
}

/*+
 * ============================================================================
 * = dq_command - send a command to a remote node                             =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine sends a command to a remote node.
 *  
 * FORM OF CALL:
 *  
 *	dq_command( sb, lun, cmd, cmd_len, dat_out, dat_out_len,
 *		    dat_in, dat_in_len, sts )
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
 *	A command may be sent to a device.
 *
-*/
int dq_command( struct ata_sb *sb, int lun, unsigned char *cmd, int cmd_len,
	unsigned char *dat_out, int dat_out_len,
	unsigned char *dat_in, int dat_in_len, unsigned char *sts )
{
    int i;
    struct dq_pb *pb;
    int status = msg_failure;
    struct identify *id;
    int start_lbn = 0;
    int lbn = 0;
    int	block_size = 0;
    char new_cmd[12];
    unsigned int nbytes;

    memcpy( new_cmd, cmd, cmd_len );
    sts[0] = 0;
    pb = ( struct dq_pb * )sb->pb;
    if ( lun )
	return( msg_failure );
    krn$_wait( &pb->owner_s );
    krn$_wait( &ide_lock );
    switch ( cmd[0] ) {
	case scsi_k_cmd_test_unit_ready:
	    if ( sb->atapi )
		status = atapi_do_send( sb, cmd, cmd_len, ( unsigned char * )0, 0, ( unsigned char * )0, 0, sts );
	    else
		status = msg_success;
	    break;
	case scsi_k_cmd_request_sense:
	    if ( sb->atapi )
		status = atapi_do_send( sb, cmd, cmd_len, ( unsigned char * )0, 0, dat_in, dat_in_len, sts );
	    break;
	case scsi_k_cmd_inquiry:
	    {
		unsigned char local_dat_in[MAX_INQ_SZ+1];

		id = ( struct identify * )sb->identify;

		if ( !id )
		    status = msg_failure;
		else {
		    if ( id->config & 0x8000 )
		    {
			/* if we are atapi then the device information in the */
			/* identify command is valid other wise we force it to */
			/* direct access */

			local_dat_in[0] = ( id->config & 0x1F00 ) >> 8;
			sb->atapi = 1;
		    }
		    else
		    {
			local_dat_in[0] = 0;
		    }
		    for ( i = 1;  i < MAX_INQ_SZ+1;  i++ ) {
			local_dat_in[i] = 0;
		    }

		    if ( dat_in_len <= MAX_INQ_SZ ) {
			local_dat_in[4] = dat_in_len - 5;
		    }
		    else {
			local_dat_in[4] = MAX_INQ_SZ - 5;
		    }
		    local_dat_in[3] = 2;
	    /*
	    ** Fill in the vendor and product ID fields
	    */
		    for ( i = 0;  i < 24;  i += 2 ) {
			local_dat_in[8+i] = id->model_number[i+1];
			local_dat_in[9+i] = id->model_number[i];
		    }
		    for ( i = 0;  i < 8;  i += 2 ) {
			local_dat_in[32+i] = id->fw_version[i+1];
			local_dat_in[33+i] = id->fw_version[i];
		    }
		    for (i=0;  i<dat_in_len;  i++)
		    {
		    	dat_in[i] = local_dat_in[i];
		    }
#if DQ_DEBUG
	    hd_helper( el_fp, 0, &dat_in[0], dat_in_len, 2, 0 );
#endif
		    status = msg_success;
		}
	    }
	    break;
	case scsi_k_cmd_mode_select:
	    status = msg_success;
	    break;
	case scsi_k_cmd_start_unit:
	    status = msg_success;
	    break;
	case scsi_k_cmd_read_capacity:
	    if ( sb->atapi ) {
		status = atapi_do_send( sb, cmd, cmd_len, ( unsigned char * )0,
		     0, dat_in, dat_in_len, sts );			

		lbn = ( dat_in[0] << 24 |
			dat_in[1] << 16 |
			dat_in[2] << 8	|
			dat_in[3] );

		/*	 
		**  we need to pick up the ral blcok size in the event that the
		**  device supports something other then 2048 byte blocks
		*/	 
		block_size = ( dat_in[4] << 24 |
			dat_in[5] << 16 |
			dat_in[6] << 8	|
			dat_in[7] );


		/*	 
		** this funny operation compensates for the 2048 blocks as
		** well as any other size.  The scsi class driver will add
		** one to the size to get the last block on the disk.
		*/	 

		lbn = (lbn + 1) * block_size;
		lbn = (lbn/512) -1;
		sb->block_size = block_size;	/* needed for furture	    */
						/* operations */
	    }
	    else {
		id = ( struct identify * )sb->identify;
		lbn = id->cylinders * id->heads * id->sectors;
	    }
	    dat_in[7] = 0x00;
	    dat_in[6] = 0x02;
	    dat_in[5] = 0x00;
	    dat_in[4] = 0x00;
	    dat_in[3] = ( unsigned char )( lbn & 0xFF );
	    dat_in[2] = ( unsigned char )( ( lbn >> 8  ) & 0xFF );
	    dat_in[1] = ( unsigned char )( ( lbn >> 16 ) & 0xFF );
	    dat_in[0] = ( unsigned char )( ( lbn >> 24 ) & 0xFF );
	    status = msg_success;
	    break;
	case scsi_k_cmd_dk_write:
	    nbytes = ( unsigned int )( ( cmd[7] << 8 | cmd[8] ) * 512 );
	    new_cmd[8] = 1;
	    start_lbn = ( cmd[2] << 24 | cmd[3] << 16 | cmd[4] << 8 | cmd[5] );
	    if ( sb->atapi ) 
		status = msg_failure;
	    else 
		status = 
		    dq_send_read_write( 
			sb->first_ub, 
			dat_out, 
			nbytes, 
			start_lbn, 
			COMMAND_WRITE_SECTOR 
		    );
	    break;
	case scsi_k_cmd_dk_read:
	    nbytes = ( unsigned int )( ( cmd[7] << 8 | cmd[8] ) * 512 );
	    new_cmd[8] = 1;
	    start_lbn = ( cmd[2] << 24 | cmd[3] << 16 | cmd[4] << 8 | cmd[5] );
	    if ( sb->atapi )
	    {
		int local_count =0;
		unsigned char *local_temp_ptr;

		while ( nbytes ) 
		{
		    int blocking = (sb->block_size/512);

		    lbn = start_lbn / blocking;
		    local_count = sb->block_size;

		    new_cmd[5] = ( unsigned char )( lbn & 0xFF );
		    new_cmd[4] = ( unsigned char )( ( lbn >>  8 ) & 0xFF );
		    new_cmd[3] = ( unsigned char )( ( lbn >> 16 ) & 0xFF );
		    new_cmd[2] = ( unsigned char )( ( lbn >> 24 ) & 0xFF );

		    if ( lbn != dq_buf_lbn || sb != dq_buf_sb ) {
			sts[0] = 0;
			dq_buf_lbn = -1;
			dq_buf_sb = -1;
			status = atapi_do_send( sb, new_cmd, cmd_len, ( unsigned char * )0, 0, &dq_buf[0], sb->block_size, sts );

			if ( status != msg_success || sts[0] ) {
			    krn$_post( &ide_lock );
			    krn$_post( &pb->owner_s );
			    return( status );
			}
			dq_buf_lbn = lbn;
			dq_buf_sb = sb;
		    }
		    memcpy( dat_in, &dq_buf[( start_lbn % blocking ) * 512], 512 );
		    start_lbn++;
		    nbytes -= 512;
		    dat_in += 512;

		}
		status = msg_success;
	    }
	    else {
		status = 
		    dq_send_read_write( 
			sb->first_ub, 
			dat_in, 
			nbytes, 
			start_lbn, 
			COMMAND_READ_SECTOR 
		    );
	    }
	    break;
	case scsi_k_cmd_rewind:
	case scsi_k_cmd_mk_read:
	case scsi_k_cmd_mk_write:
	case scsi_k_cmd_mk_write_filemarks:
	default:
#if DQ_DEBUG
	    fprintf( el_fp, "dq_command: Unsupported command %d\n", cmd[0] );
#endif
	    status = msg_failure;
    }
    krn$_post( &ide_lock );
    krn$_post( &pb->owner_s );
    return( status );
}

/*+
 * ============================================================================
 * = dq_identify - Identify ATA/ATAPI devices                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine identifies ATA/ATAPI devices connected to
 *	the specified port.
 *  
 * FORM OF CALL:
 *  
 *	dq_identify( pb, unit, inq )
 *  
 * RETURN CODES:
 *
 *	msg_success
 *      msg_failure
 *
 * ARGUMENTS:
 *
 *	struct dq_pb *pb		- port block
 *	int unit			- LUN
 *	unsigned short *id		- pointer to identify data
 *
 * SIDE EFFECTS:
 *
 *	A command may be sent to a device.
 *
-*/
int dq_identify( struct dq_pb *pb, unsigned int unit, unsigned short *id )
{
    unsigned int i;
    int t = msg_failure;
    unsigned char status;
    unsigned char error;

    krn$_wait( &pb->owner_s );
    krn$_wait( &ide_lock );
    ( void )dq_wait_on_busy( pb );
    wb( DRIVE_HEAD, 0xA0 | ( unit << 4 ) );
    krn$_sleep( 1 );

    /* see if driver there.  We first do this by writing to the */
    /* cylinder low register and verifying that we get back what we */
    /* wrote */
    wb( CYLINDER_LO, 0xAA);

    if ( ((rb (CYLINDER_LO)) == 0xAA) && ( t = dq_wait_on_busy( pb ) ) == msg_success ) {
	wb( COMMAND, COMMAND_IDENTIFY );
	krn$_sleep( 1 );
	( void )dq_wait_on_busy( pb );
	status = rb( STATUS );
	error = rb( ERROR );
	if ( status & STATUS_ERR ) {
	    if ( error & 0x04 ) {
		if ( ( rb( CYLINDER_HI ) == ATAPI_MAGIC_2 ) && 
		     ( rb( CYLINDER_LO ) == ATAPI_MAGIC_1 ) ) {
		    wb( COMMAND, COMMAND_PIDENTIFY );
		    krn$_sleep( 1 );
		    ( void )dq_wait_on_busy( pb );
		    status = rb( STATUS );
		    error = rb( ERROR );
		}
	    }
	}
	if ( ( t = dq_wait_for_drq( pb ) ) == msg_success ) {
	    for ( i = 0; i < sizeof( struct identify ); i += 2 ) {
		*id++ = rw( DATA );
	    }
	}
    } else {
	wb( DRIVE_HEAD, 0xA0 );
	krn$_sleep( 1 );
    }
    krn$_post( &ide_lock );
    krn$_post( &pb->owner_s );
    return( t );
}

/*+
 * ============================================================================
 * = dq_send_inquiry - send an Inquiry command                                =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds and sends an Inquiry command.  This routine
 *	is normally called by the polling routine in the port driver.
 *  
 * FORM OF CALL:
 *  
 *	dq_send_inquiry( sb )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct ata_sb *sb		- system block
 *
 * SIDE EFFECTS:
 *
 *	A new unit may be discovered.
 *
-*/
void dq_send_inquiry( struct ata_sb *sb )
{
    unsigned char cmd[6];
    unsigned char dat_in[MAX_INQ_SZ];
    int cmd_len;
    int dat_in_len;
    int i;
    int t;
    struct ub *ub;

    /*
     *  Fill in the Command Out bytes.
     */
    cmd_len = sizeof( cmd );
    cmd[0] = scsi_k_cmd_inquiry;
    cmd[1] = 0;
    cmd[2] = 0;
    cmd[3] = 0;
    cmd[4] = sizeof( dat_in );
    cmd[5] = 0;
    /*
     *  Get a UB for use.
     */
    ub = ( struct ub * )malloc_noown( sizeof( *ub ) );
    ub->sb = sb;
    ub->pb = sb->pb;
    /*
     *  Send the command.  If successful, say we have discovered a new unit.
     */
    ub->unit = 0;
    dat_in_len = sizeof( dat_in );
    t = scsi_send( ub, cmd, cmd_len, 0, 0, dat_in, &dat_in_len );
    if ( t == msg_success )
	dq_new_unit( sb, 0, dat_in );
    /*
     * Get rid of the UB.
     */
    free( ub );
}
