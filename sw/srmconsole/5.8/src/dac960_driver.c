/*
 * Copyright (C) 1993 by
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
 * Abstract:	DAC960 driver
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *	sfs	14-Oct-1993	Initial entry.
 *
 *	dtr	9-Sep-1994	Added support for those systems that have pci
 *				window support.  All references that set up the
 *				mailbox pointers have the addresses modified to
 *				or in the window_base address defined by the
 *				specific platform_io.h.
 */

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$src:prototypes.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:msg_def.h"
#include "cp$src:eisa.h"
#include "cp$src:scsi_def.h"
#include "cp$src:pb_def.h"
#include "cp$src:dac960_pb_def.h"
#include "cp$src:sb_def.h"
#include "cp$src:ub_def.h"
#include "cp$src:probe_io_def.h"
#include "cp$inc:platform_io.h"
#include "cp$src:platform_cpu.h"
#include "cp$inc:kernel_entry.h"
#if MODULAR
#include "cp$src:mem_def.h"
#endif

#define rb(x) pb->inb(pb,pb->csr+pb->x)
#define rl(x) pb->inl(pb,pb->csr+pb->x)
#define wb(x,y) pb->outb(pb,pb->csr+pb->x,y)
#define wl(x,y) pb->outl(pb,pb->csr+pb->x,y)

#define CMD_SCSI_PT 0x2
#define CMD_SCSI 0x4
#define CMD_IOCTL 0x20

#define IOCTL_GETCONTROLLERINFO 0x1
#define IOCTL_GETLOGDEVINFOVALID 0x3
#define IOCTL_GETDEVCONFINFOVALID 0xcb

#define OPCODE_READ_OLD 0x02
#define OPCODE_WRITE_OLD 0x03
#define OPCODE_DCDB 0x04
#define OPCODE_ENQUIRY_OLD 0x05
#define OPCODE_DEVICE_STATE_OLD 0x14
#define OPCODE_READ 0x36
#define OPCODE_WRITE 0x37
#define OPCODE_NVRAM_CONFIG_OLD 0x38
#define OPCODE_NVRAM_CONFIG 0x4e
#define OPCODE_DEVICE_STATE 0x50
#define OPCODE_ENQUIRY 0x53

#define LDB_MBX_IN_USE 0x01
#define LDB_MBX_DONE 0x02
#define SDB_MBX_DONE 0x01
#define SDB_MSG_READY 0x04
#define SDB_SYNCING 0x08

#define NUM_CHANNELS 5
#define NUM_CHANNELS_OLD 5
#define NUM_TARGETS 16
#define NUM_TARGETS_OLD 7
#define NUM_DRIVES 32
#define NUM_DRIVES_OLD 8
#define NUM_ARMS 8
#define NUM_DISKS 4
#define NUM_SPANS 4
#define NUM_DEAD_DRIVES 21
#define NUM_SG_ENTRIES 17
#define BLOCK_SIZE 512

extern null_procedure( );
extern common_driver_open( );
extern unsigned char inportb( );
extern unsigned long inportl( );
extern void outportb( );
extern void outportl( );
extern unsigned char inmemb( );
extern unsigned long inmeml( );
extern void outmemb( );
extern void outmeml( );

extern struct FILE *el_fp;

#if MODULAR
extern int diagnostic_mode_flag;
extern struct DDB *dr_ddb_ptr;
#endif

#if RAWHIDE
#include "cp$src:rawhide_common.h"
#define malloc_noown(x) dyn$_malloc(x,DYN$K_SYNC|DYN$K_NOOWN)
#endif

#if !( STARTSHUT || DRIVERSHUT )
int dac960_read( );
int dac960_write( );

struct DDB dac960_ddb = {
	"dac960",		/* how this routine wants to be called	*/
	dac960_read,		/* read routine				*/
	dac960_write,		/* write routine			*/
	common_driver_open,	/* open routine				*/
	null_procedure,		/* close routine			*/
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

#if STARTSHUT

/*+
 * ============================================================================
 * = dac960_setmode - change driver state				      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * 	This routine changes the state of the port driver.  The following
 * 	states are relevant:
 *
 *	    DDB$K_STOP -
 *			Suspends the  port driver.  dac960_poll stops checking
 *			things in this state, and interrupts from the
 *			device are dismissed.
 *
 *	    DDB$K_RESTARTABLE -
 *			Same as DDB$K_STOP, except the port driver may be
 *			restarted dynamically by the class driver.
 *
 *	    DDB$K_START -
 *			Changes the   port   driver   to   interrupt  mode.
 *			dac960_poll is once again enabled, as well as device
 *			interrupt handling.
 *
 *	    DDB$K_INTERRUPT -
 *			Same as DDB$K_START.
 *  
 *	    DDB$K_ASSIGN  -	
 *			Assigns controller device letters.
 *
 * FORM OF CALL:
 *  
 *	dac960_setmode(mode, dev)
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
int dac960_setmode(int mode, struct device *dev)
{
    int i, j, k, t;
    struct dac960_pb *pb;
    struct sb *sb;
    struct ub *ub;

    switch (mode) {
	case DDB$K_ASSIGN: 
	    break;

	case DDB$K_START: 

	case DDB$K_INTERRUPT: 
	    pb = (struct dac960_pb *) dev->devdep.io_device.devspecific;

	    if (pb) {
		pb->pb.ref++;
		return msg_success;
	    }

	    if (dev->flags)
		return msg_failure;

	    krn$_set_console_mode(0);

	    break;	    

	case DDB$K_STOP: 
	    pb = (struct dac960_pb *) dev->devdep.io_device.devspecific;

	    if (!pb)
		return msg_failure;

	    if (pb->pb.ref == 0)
		return msg_success;

	    pb->pb.ref--;
	    if (pb->pb.ref != 0)
		return msg_success;

	    dac960_clear_controller(pb);

/* Free all vectors */

	    int_vector_clear(pb->pb.vector);

/* Free all semaphores */

	    krn$_semrelease( &pb->owner_s );

/* Free all dynamic memory */

	    for (i = 0; i < pb->pb.num_sb; i++) {
		if (pb->pb.sb[i]) {
		    sb = pb->pb.sb[i];
		    ub = sb->first_ub;
		    if( ub ) {
			remove_ub(ub);		/* Free all unit blocks */
			free(ub);
		    }
		    free(pb->pb.sb[i]);
		}
	    }
	    free(pb->pb.sb);

	    if (diagnostic_mode_flag)
		remove_pb( pb );

	    free(pb);

	    dev->devdep.io_device.devspecific = 0;

	    break;

    }					/* switch */

    return msg_success;

}					/* dac960_setmode */

/*+
 * ============================================================================
 * = dac960_poll_units -							      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *  
 * FORM OF CALL:
 *  
 *	dac960_poll_units(dev, verbose)
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
int dac960_poll_units(struct device *dev, int verbose)
{
    if (dac960_initialize(dev, TRUE) != msg_success)
	return msg_failure;
}

/*+
 * ============================================================================
 * = dac960_establish_connection - establish a connection to given unit        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine establishes a connection to a given unit.
 *  
 * FORM OF CALL:
 *  
 *	dac960_establish_connection( inode, dev, class )
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
int dac960_establish_connection(struct INODE *inode, struct device *dev, int class)
{

/* diagnostic mode, node already configured */

    if (diagnostic_mode_flag)
	return msg_success;

/* if device not in file system, fail */

    if (!inode->inuse) {
	return msg_failure;
    }

    if (dac960_initialize(dev, FALSE) != msg_success)
	return msg_failure;
}

int dac960_initialize(struct device *dev, int verbose )
    {

    struct dac960_pb *pb;

    pb = malloc_noown(sizeof(*pb));

    pb = (struct dac960_pb *) dev->devdep.io_device.devspecific;

    if ( !pb ) {
	pb = malloc(sizeof(*pb));
	pb->pb.ref++;
	if (diagnostic_mode_flag)
	    insert_pb( pb );
    } else {
	return msg_success;
    }

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
    pb->pb.protocol = dr_ddb_ptr->name; 
    pb->pb.device = "dac960";
    pb->pb.hardware_name = dev->tbl->name;

    if ( pb->pb.bus == 1 )
	pb->pb.type = TYPE_EISA;
    else
	pb->pb.type = TYPE_PCI;

    /*
     * Using the state filed in the pb to turn on printing of the inode.
     */

    if ( verbose )
	pb->pb.state = TRUE;
    
#if 0
    if (pci_get_base_address(dev, dac960_CSR_BASE, &pb->pb.csr) != msg_success)
	return msg_failure;
#endif

    if ( platform() == ISP_MODEL ) {
	pprintf("RAID controller is not emulated\n");
	return msg_failure;
    }

    pb->pb.ref++;

    dac960_init_port( pb );
    return msg_success;
    }
#endif


#if !( STARTSHUT || DRIVERSHUT )
int dac960_init( )
    {
    int dac960_init_pb( );

    find_pb( "dac960", sizeof( struct dac960_pb ), dac960_init_pb );
    return( msg_success );
    }

void dac960_init_pb( struct dac960_pb *pb )
    {
    int dac960_init_port( );

    log_driver_init( pb );
    krn$_create( dac960_init_port, null_procedure, 0, 5, 0, 0,
	    "dr_init", "nl", "r", "nl", "w", "nl", "w", pb );
    }
#endif

struct scsi {
    uint16 cmd_id;
    uint8 cmd_opcode;
    uint8 control;
    uint32 data_size;
    uint64 request_sense_pointer;
    uint8 lun;
    uint8 target;
    uint8 channel;
    uint8 timeout;
    uint8 request_sense_size;
    uint8 cdb_size;
    uint8 cdb [10];
    uint64 data_pointer_1;
    uint64 data_size_1;
    uint64 data_pointer_2;
    uint64 data_size_2;
    } ;

struct ioctl {
    uint16 cmd_id;
    uint8 cmd_opcode;
    uint8 control;
    uint32 data_size;
    uint64 request_sense_pointer;
    uint16 device;
    uint8 channel;
    uint8 timeout;
    uint8 request_sense_size;
    uint8 ioctl_opcode;
    uint8 fill22_31 [10];
    uint64 data_pointer_1;
    uint64 data_size_1;
    uint64 data_pointer_2;
    uint64 data_size_2;
    } ;

struct controller_info {
    uint8 controller_number;
    uint8 bus_interface_type;
    uint8 controller_type;
    uint8 controller_type_exp;
    uint16 bus_interface_speed;
    uint8 bus_interface_width;
    uint8 fill7_15 [9];
    uint8 bus_interface_name [16];
    uint8 controller_name [16];
    uint8 fill48_63 [16];
    uint8 fw_ver_major;
    uint8 fw_ver_minor;
    uint8 fw_turn;
    uint8 fw_build;
    uint8 fw_rel_day;
    uint8 fw_rel_month;
    uint8 fw_rel_year_ms2;
    uint8 fw_rel_year_ls2;
    uint8 hw_version;
    uint8 fill73_75 [3];
    uint8 hw_rel_day;
    uint8 hw_rel_month;
    uint8 hw_rel_year_ms2;
    uint8 hw_rel_year_ls2;
    uint8 hw_batch;
    uint8 fill81_81 [1];
    uint8 hw_plant;
    uint8 fill83_83 [1];
    uint8 hw_mfg_day;
    uint8 hw_mfg_month;
    uint8 hw_mfg_year_ms2;
    uint8 hw_mfg_year_ls2;
    uint8 max_pd_per_xld;
    uint8 max_ild_per_xld;
    uint8 max_xld;
    uint16 nvram_size;
    uint8 fill92_95 [3];
    uint8 serial_number [16];
    uint8 fill112_127 [16];
    uint8 fill128_130 [3];
    uint8 oem_info;
    uint8 vendor_name [16];
    uint8 hw_flags;
    uint8 fill149_151 [3];
    uint8 pd_scan_active;
    uint8 pd_scan_channel;
    uint8 pd_scan_target;
    uint8 pd_scan_lun;
    uint16 max_data_size;
    uint16 max_sg_entries;
    uint16 num_drives;
    uint16 num_critical;
    uint16 num_offline;
    uint16 num_targets;
    uint16 num_phys_drives;
    uint16 num_phys_critical;
    uint16 num_phys_offline;
    uint16 num_cmds;
    uint8 num_phys_channels;
    uint8 num_virt_channels;
    uint8 max_phys_channels;
    uint8 max_virt_channels;
    uint8 max_targets [16];
    uint8 fill196_207 [12];
    uint16 memory_size;
    uint16 cache_size;
    uint32 valid_cache_size;
    uint32 dirty_cache_size;
    uint16 memory_speed;
    uint8 memory_width;
    uint8 cache_type;
    uint8 cache_name [16];
    uint8 fill240_271 [32];
    uint16 cpu_speed;
    uint8 cpu_type;
    uint8 num_cpu;
    uint8 fill274_285 [12];
    uint8 cpu_name [16];
    uint8 fill304_335 [32];
    uint8 fill336_511 [176];
    } ;

struct phys_device_config_info {
    uint8 raid_type;
    uint8 state;
    uint16 device;
    uint32 capacity;
    uint8 controller;
    uint8 channel;
    uint8 target;
    uint8 lun;
    uint32 start_sector;
    } ;

struct raid_device_config_info {
    uint8 fill0_0 [1];
    uint8 state;
    uint16 device;
    uint32 start_sector;
    } ;

struct device_config_info {
    uint8 raid_type;
    uint8 state;
    uint16 device;
    uint32 capacity;
    uint8 members;
    uint8 stripe_size;
    uint8 cache_line_size;
    uint8 control;
    uint32 member_capacity;
    struct raid_device_config_info raid_device_config_info [16];
    } ;

struct mbx_1 {
    unsigned char opcode;
    unsigned char cmd_id;
    unsigned char count;
    unsigned char block [4];
    unsigned char unit;
    unsigned long pointer;
    unsigned char sg_type;
    unsigned char rsp_id;
    unsigned short status;
    } ;

struct mbx_2 {
    unsigned char opcode;
    unsigned char cmd_id;
    unsigned char channel;
    unsigned char target;
    unsigned char state;
    unsigned char unused5_7 [3];
    unsigned long pointer;
    unsigned char unused12_12 [1];
    unsigned char rsp_id;
    unsigned short status;
    } ;

struct mbx_3 {
    unsigned char opcode;
    unsigned char cmd_id;
    unsigned char unused2_7 [6];
    unsigned long pointer;
    unsigned char unused12_12 [1];
    unsigned char rsp_id;
    unsigned short status;
    } ;

struct mbx_5 {
    unsigned char opcode;
    unsigned char cmd_id;
    unsigned char param;
    unsigned char unused3_7 [5];
    unsigned long pointer;
    unsigned char unused12_12 [1];
    unsigned char rsp_id;
    unsigned short status;
    } ;

struct mbx_6 {
    unsigned char opcode;
    unsigned char cmd_id;
    unsigned short count : 11;
    unsigned short unit : 5;
    unsigned long block;
    unsigned long pointer;
    unsigned char unused12_12;
    unsigned char rsp_id;
    unsigned short status;
    } ;

struct dead_drive {
    unsigned char target;
    unsigned char channel;
    } ;

struct enquiry {
    unsigned char num_drives;
    unsigned char fill1_3 [3];
    unsigned long capacity [NUM_DRIVES];
    unsigned char fill132_133 [2];
    unsigned char status_flags;
    unsigned char fill135_135 [1];
    unsigned char fw_ver_minor;
    unsigned char fw_ver_major;
    unsigned char rebuild_flag;
    unsigned char num_cmds;
    unsigned char num_offline;
    unsigned char fill141_141 [1];
    unsigned short event_log_seq_num;
    unsigned char num_critical;
    unsigned char fill145_147 [3];
    unsigned char num_dead;
    unsigned char fill149_149 [1];
    unsigned char rebuild_count;
    unsigned char misc_flags;
    struct dead_drive dead_drive [NUM_DEAD_DRIVES];
    } ;

struct enquiry_old {
    unsigned char num_drives;
    unsigned char fill1_3 [3];
    unsigned long capacity [NUM_DRIVES_OLD];
    unsigned short num_flash_prog;
    unsigned char status_flags;
    unsigned char fill39_39 [1];
    unsigned char fw_ver_minor;
    unsigned char fw_ver_major;
    unsigned char rebuild_flag;
    unsigned char num_cmds;
    unsigned char num_offline;
    unsigned char fill45_47 [3];
    unsigned char num_critical;
    unsigned char fill49_51 [3];
    unsigned char num_dead;
    unsigned char fill53_53 [1];
    unsigned char rebuild_count;
    unsigned char misc_flags;
    struct dead_drive dead_drive [NUM_DEAD_DRIVES];
    unsigned char fill98_99 [2];
    } ;

struct arm {
    unsigned target : 4;
    unsigned channel : 4;
    } ;

struct span {
    unsigned long start_sector;
    unsigned long num_sectors;
    struct arm arm [NUM_ARMS];
    } ;

struct system_drive {
    unsigned char status;
    unsigned char extended_status;
    unsigned char modifier1;
    unsigned char modifier2;
    unsigned char raid;
    unsigned char arms;
    unsigned char spans;
    unsigned char fill7_7 [1];
    struct span span [NUM_SPANS];
    } ;

struct device_info {
    unsigned char present;
    unsigned char type;
    unsigned char status;
    unsigned char fill3_3 [1];
    unsigned char period;
    unsigned char offset;
    unsigned long num_sectors;
    } ;

struct nvram_config {
    unsigned char num_drives;
    unsigned char fill1_3 [3];
    struct system_drive system_drive [NUM_DRIVES];
    struct device_info device_info [NUM_CHANNELS] [NUM_TARGETS];
    } ;

struct disk_old {
    unsigned char channel;
    unsigned char target;
    unsigned char fill2_3 [2];
    unsigned long start_sector;
    unsigned long num_sectors;
    } ;

struct arm_old {
    unsigned char disks;
    unsigned char fill1_3 [3];
    struct disk_old disk [NUM_DISKS];
    } ;

struct system_drive_old {
    unsigned char status;
    unsigned char raid;
    unsigned char arms;
    unsigned char fill3_3 [1];
    struct arm_old arm [NUM_ARMS];
    } ;

struct device_info_old {
    unsigned char present;
    unsigned char params;
    unsigned char config_state;
    unsigned char status;
    unsigned char fill4_4 [1];
    unsigned char sync_tp;
    unsigned char sync_mo;
    unsigned char fill7_7 [1];
    unsigned long num_sectors;
    } ;

struct nvram_config_old {
    unsigned char num_drives;
    unsigned char fill1_3 [3];
    struct system_drive_old system_drive [NUM_DRIVES_OLD];
    struct device_info_old device_info [NUM_CHANNELS_OLD] [NUM_TARGETS_OLD];
    } ;

struct dcdb {
    unsigned char address;
    unsigned char control;
    unsigned short dat_in_out_len;
    unsigned long dat_in_out;
    unsigned char cdb_len;
    unsigned char sense_len;
    unsigned char cdb [12];
    unsigned char sense [64];
    unsigned char sts;
    unsigned char fill87_87 [1];
    } ;

void dac960_interrupt( struct dac960_pb *pb )
    {
    io_disable_interrupts( pb, pb->pb.vector );
    }

int dac960_issue_mbx( struct dac960_pb *pb, unsigned char *mbx )
    {
    int i;
    int mbx_status;

    mbx_status = 0xffff;
    for( i = 0; i < 100; i++ )
	if( ( rb( LDB ) ^ pb->ldb_xor ) & LDB_MBX_IN_USE )
	    krn$_sleep( 10 );
	else
	    break;
    if( i == 100 )
	return( msg_failure );
    if( pb->new )
	wl( MBX, (unsigned long)mbx | io_get_window_base( pb ) );
    else
	for( i = 0; i < 13; i++ )
	    wb( MBX + i, mbx[i] );
    wb( LDB, LDB_MBX_IN_USE );
    for( i = 0; i < 6000; i++ )
	if( rb( SDB ) & SDB_MBX_DONE )
	    break;
	else
	    krn$_sleep( 10 );
    if( i == 6000 )
	return( msg_failure );
    mbx_status = ( rb( MBX_STATUS + 0 ) << 0 ) | ( rb( MBX_STATUS + 1 ) << 8 );
    wb( SDB, SDB_MBX_DONE );
    wb( LDB, LDB_MBX_DONE );
    return( mbx_status );
    }

void dac960_init_port( struct dac960_pb *pb )
    {
    int i;
    int j;
    int nd;
    int no;
    int nc;
    int channel;
    int target;
    int disks;
    int dac960_setmode( );
    int dac960_interrupt( );
    int mbx_status;
    struct ioctl ioctl;
    struct controller_info *controller_info;
    struct device_config_info *device_config_info;
    struct phys_device_config_info phys_device_config_info;
    struct system_drive system_drive;
    struct mbx_5 mbx;
    struct enquiry *enquiry;
    struct enquiry_old *enquiry_old;
    struct nvram_config *nvram_config;
    struct nvram_config_old *nvram_config_old;

    enquiry = malloc_noown( sizeof( *enquiry ) );
    nvram_config = malloc_noown( sizeof( *nvram_config ) );
    if( pb->pb.type == TYPE_EISA )
	{
	use_eisa_ecu_data( pb );
	pb->csr = pb->pb.csr;
	pb->inb = inportb;
	pb->inl = inportl;
	pb->outb = outportb;
	pb->outl = outportl;
	pb->ldb_xor = 0;
	pb->LDB = 0xc8d;
	pb->SDB = 0xc8f;
	pb->MBX = 0xc90;
	pb->MBX_STATUS = 0xc9e;
	pb->TGT = 0xc98;
	pb->CHN = 0xc99;
	}
    else
	{
	pb->csr = incfgl( pb, 0x10 ) & ~15;
	switch( incfgb( pb, 0x2e ) )
	    {
	    case 0x10:
		pb->inb = inmemb;
		pb->inl = inmeml;
		pb->outb = outmemb;
		pb->outl = outmeml;
		pb->ldb_xor = 0;
		pb->LDB = 0x20;
		pb->SDB = 0x2c;
		pb->MBX = 0x1000;
		pb->MBX_STATUS = 0x101a;
		break;
	    case 0x20:
		pb->inb = inmemb;
		pb->inl = inmeml;
		pb->outb = outmemb;
		pb->outl = outmeml;
		pb->ldb_xor = LDB_MBX_DONE | LDB_MBX_IN_USE;
		pb->LDB = 0x60;
		pb->SDB = 0x61;
		pb->MBX = 0x50;
		pb->MBX_STATUS = 0x5e;
		pb->INT = 0x34;
		break;
	    case 0x40:
		pb->new = 1;
		pb->inb = inmemb;
		pb->inl = inmeml;
		pb->outb = outmemb;
		pb->outl = outmeml;
		pb->ldb_xor = LDB_MBX_DONE | LDB_MBX_IN_USE;
		pb->LDB = 0x60;
		pb->SDB = 0x61;
		pb->MBX = 0x50;
		pb->MBX_STATUS = 0x5a;
		pb->INT = 0x34;
		break;
	    default:
		pb->inb = inportb;
		pb->inl = inportl;
		pb->outb = outportb;
		pb->outl = outportl;
		pb->ldb_xor = 0;
		pb->LDB = 0x40;
		pb->SDB = 0x41;
		pb->MBX = 0x0;
		pb->MBX_STATUS = 0xe;
		pb->TGT = 0x8;
		pb->CHN = 0x9;
		break;
	    }
	}
    pb->pb.setmode = dac960_setmode;
    if( pb->pb.vector )
	{
	pb->pb.mode = DDB$K_INTERRUPT;
	pb->pb.desired_mode = DDB$K_INTERRUPT;
	int_vector_set( pb->pb.vector, dac960_interrupt, pb );
	}
    else
	{
	pb->pb.mode = DDB$K_POLLED;
	pb->pb.desired_mode = DDB$K_POLLED;
	}
    set_io_name( pb->pb.name, 0, 0, 0, pb );
    strcpy( pb->pb.name + 3, pb->pb.name + 4 );
    krn$_seminit( &pb->owner_s, 1, "dr_owner" );
    krn$_wait( &pb->owner_s );
    pb->board_status = dac960_clear_controller( pb );
    if( pb->board_status == msg_success )
	{
	if( pb->new )
	    {
#if DEBUG
	    fprintf( el_fp, "issuing GETCONTROLLERINFO on %s\n", pb->pb.name );
#endif
	    controller_info = malloc_noown( sizeof( *controller_info ) );
	    device_config_info = malloc_noown( sizeof( *device_config_info ) );
	    memset( &ioctl, 0, sizeof( ioctl ) );
	    ioctl.cmd_id = 1;
	    ioctl.cmd_opcode = CMD_IOCTL;
	    ioctl.control = 0x10;
	    ioctl.data_size = sizeof( *controller_info );
	    ioctl.ioctl_opcode = IOCTL_GETCONTROLLERINFO;
	    ioctl.data_pointer_1 = (unsigned long)controller_info | io_get_window_base( pb );
	    ioctl.data_size_1 = sizeof( *controller_info );
	    mbx_status = dac960_issue_mbx( pb, &ioctl );
	    if( mbx_status )
		fprintf( el_fp, "GETCONTROLLERINFO failed with status %04x on %s\n", mbx_status, pb->pb.name );
	    else
		{
#if DEBUG
		hd_helper( el_fp, 0, controller_info, sizeof( *controller_info ), 1, 0 );
#endif
		nd = controller_info->num_drives;
		no = controller_info->num_offline;
		nc = controller_info->num_critical;
		fprintf( el_fp, "%s firmware version V%d.%d\n",
			pb->pb.name, controller_info->fw_ver_major, controller_info->fw_ver_minor );
		fprintf( el_fp, "%s drives = %d, optimal = %d, degraded = %d, failed = %d\n",
			pb->pb.name, nd, nd - nc - no, nc, no );
		pb->pb.sb = malloc_noown( nd * sizeof( struct sb * ) );
		pb->pb.num_sb = nd;
		pb->max_channels = controller_info->max_phys_channels;
		for( i = 0, j = 0; i < controller_info->max_xld && j < nd; i++, j++ )
		    {
#if DEBUG
		    fprintf( el_fp, "issuing GETDEVCONFINFOVALID for device %d on %s\n", i, pb->pb.name );
#endif
		    memset( &ioctl, 0, sizeof( ioctl ) );
		    ioctl.cmd_id = 1;
		    ioctl.cmd_opcode = CMD_IOCTL;
		    ioctl.control = 0x10;
		    ioctl.data_size = sizeof( *device_config_info );
		    ioctl.device = i;
		    ioctl.ioctl_opcode = IOCTL_GETDEVCONFINFOVALID;
		    ioctl.data_pointer_1 = (unsigned long)device_config_info | io_get_window_base( pb );
		    ioctl.data_size_1 = sizeof( *device_config_info );
		    mbx_status = dac960_issue_mbx( pb, &ioctl );
		    if( mbx_status )
			fprintf( el_fp, "GETDEVCONFINFOVALID failed with status %04x on %s\n", mbx_status, pb->pb.name );
		    else
			{
			i = device_config_info->device;
			switch( device_config_info->state )
			    {
			    case 1:
				system_drive.status = 3;
				break;

			    case 8:
				system_drive.status = 255;
				break;

			    case 9:
			    case 11:
				system_drive.status = 4;
				break;

			    default:
				system_drive.status = 0;
				break;
			    }				
			system_drive.raid = device_config_info->raid_type & 0xf;
			system_drive.arms = 1;
			system_drive.spans = device_config_info->members;
			dac960_new_unit( pb, j, i, device_config_info->capacity, &system_drive );
			}
		    }
		}
	    free( controller_info );
	    free( device_config_info );
	    }
	else
	    {
#if DEBUG
	    fprintf( el_fp, "issuing ENQUIRY on %s\n", pb->pb.name );
#endif
	    mbx.opcode = OPCODE_ENQUIRY;
	    mbx.pointer = (unsigned long)enquiry | io_get_window_base( pb );
	    mbx_status = dac960_issue_mbx( pb, &mbx );
	    if( mbx_status == 0x0104 )
		{
		enquiry_old = malloc_noown( sizeof( *enquiry_old ) );
		mbx.opcode = OPCODE_ENQUIRY_OLD;
		mbx.pointer = (unsigned long)enquiry_old | io_get_window_base( pb );
		mbx_status = dac960_issue_mbx( pb, &mbx );
		enquiry->num_drives = enquiry_old->num_drives;
		memcpy( enquiry->capacity, enquiry_old->capacity, sizeof( enquiry_old->capacity ) );
		enquiry->fw_ver_major = enquiry_old->fw_ver_major;
		enquiry->fw_ver_minor = enquiry_old->fw_ver_minor;
		enquiry->num_cmds = enquiry_old->num_cmds;
		enquiry->num_offline = enquiry_old->num_offline;
		enquiry->num_critical = enquiry_old->num_critical;
		enquiry->num_dead = enquiry_old->num_dead;
		memcpy( enquiry->dead_drive, enquiry_old->dead_drive, sizeof( enquiry_old->dead_drive ) );
		free( enquiry_old );
		pb->old = 1;
		pb->num_drives = NUM_DRIVES_OLD;
		}
	    else
		{
		pb->old = 0;
		pb->num_drives = NUM_DRIVES;
		}
	    if( mbx_status )
		fprintf( el_fp, "ENQUIRY failed with status %04x on %s\n", mbx_status, pb->pb.name );
	    else
		{
#if DEBUG
		hd_helper( el_fp, 0, enquiry, sizeof( *enquiry ), 1, 0 );
#endif
		nd = 0;
		for( i = 0; i < pb->num_drives; i++ )
		    if( enquiry->capacity[i] )
			nd++;
		no = enquiry->num_offline;
		nc = enquiry->num_critical;
		fprintf( el_fp, "%s firmware version V%d.%d\n",
			pb->pb.name, enquiry->fw_ver_major, enquiry->fw_ver_minor );
		fprintf( el_fp, "%s drives = %d, optimal = %d, degraded = %d, failed = %d\n",
			pb->pb.name, nd, nd - nc - no, nc, no );
		if( enquiry->num_dead )
		    for( i = 0; i < NUM_DEAD_DRIVES; i++ )
			{
			channel = enquiry->dead_drive[i].channel;
			target = enquiry->dead_drive[i].target;
			if( ( channel == 0xff ) && ( target == 0xff ) )
			    break;
			fprintf( el_fp, "SCSI drive at channel %d, target %d dead on %s\n",
				channel, target, pb->pb.name );
			}
#if DEBUG
		fprintf( el_fp, "issuing NVRAM_CONFIG on %s\n", pb->pb.name );
#endif
		if( pb->old )
		    {
		    nvram_config_old = malloc_noown( sizeof( *nvram_config_old ) );
		    mbx.opcode = OPCODE_NVRAM_CONFIG_OLD;
		    mbx.pointer = (unsigned long)nvram_config_old | io_get_window_base( pb );
		    mbx_status = dac960_issue_mbx( pb, &mbx );
		    for( i = 0; i < NUM_DRIVES_OLD; i++ )
			{
			disks = 0;
			for( j = 0; j < nvram_config_old->system_drive[i].arms; j++ )
			    disks += nvram_config_old->system_drive[i].arm[j].disks;
			nvram_config->system_drive[i].status = nvram_config_old->system_drive[i].status;
			nvram_config->system_drive[i].raid = nvram_config_old->system_drive[i].raid;
			nvram_config->system_drive[i].arms = disks;
			nvram_config->system_drive[i].spans = 1;
			}
		    free( nvram_config_old );
		    }
		else
		    {
		    mbx.opcode = OPCODE_NVRAM_CONFIG;
		    mbx.pointer = (unsigned long)nvram_config | io_get_window_base( pb );
		    mbx_status = dac960_issue_mbx( pb, &mbx );
		    }
		if( mbx_status )
		    fprintf( el_fp, "NVRAM_CONFIG failed with status %04x on %s\n", mbx_status, pb->pb.name );
		else
		    {
#if DEBUG
		    hd_helper( el_fp, 0, nvram_config, sizeof( *nvram_config ), 1, 0 );
#endif
		    pb->pb.sb = malloc_noown( nd * sizeof( struct sb * ) );
		    pb->pb.num_sb = nd;
		    nd = 0;
		    for( i = 0; i < pb->num_drives; i++ )
			if( enquiry->capacity[i] )
			    dac960_new_unit( pb, nd++, i, enquiry->capacity[i],
				    &nvram_config->system_drive[i] );
		    }
		}
	    }
#if DAC960_DISKS_AND_TAPES
	dac960_do_inquiries( pb );
#endif
	}
    krn$_post( &pb->owner_s );
    free( enquiry );
    free( nvram_config );
    }

int dac960_clear_controller( struct dac960_pb *pb )
    {
    unsigned char sdb;
    int i;
    int fatal;

    if( pb->INT )
	wb( INT, 0xff );
    wl( MBX + 0, 0 );
    wl( MBX + 4, 0 );
    wl( MBX + 8, 0 );
    wl( MBX + 12, 0 );
    fatal = 0;
    wb( LDB, LDB_MBX_DONE );
    krn$_sleep( 100 );
    for( i = 0; i < 600; i++ )
	{
	if( ( rb( LDB ) ^ pb->ldb_xor ) & LDB_MBX_DONE )
	    {
	    if( pb->old )
		{
		sdb = rb( SDB );
		if( sdb & SDB_MSG_READY )
		    {
		    switch( ( sdb >> 4 ) & 0x0f )
			{
			case 15:
			    fatal = 1;
			    qprintf( "memory parity error on %s\n", pb->pb.name );
			    break;

			case 8:
			    fatal = 1;
			    qprintf( "memory error on %s\n", pb->pb.name );
			    break;

			case 4:
			    fatal = 1;
			    qprintf( "BMIC error on %s\n", pb->pb.name );
			    break;

			case 2:
			    fatal = 1;
			    qprintf( "firmware checksum error on %s\n", pb->pb.name );
			    break;

			case 3:
			    fatal = 1;
			    qprintf( "configuration checksum error on %s\n", pb->pb.name );
			    break;

			case 5:
			    fatal = 1;
			    qprintf( "configuration mismatch error on %s\n", pb->pb.name );
			    break;

			case 0:
			    qprintf( "SCSI drive at channel %d, target %d not responding on %s\n", rb( CHN ), rb( TGT ), pb->pb.name );
			    break;

			case 9:
			    qprintf( "SCSI drive at channel %d, target %d misconnected on %s\n", rb( CHN ), rb( TGT ), pb->pb.name );
			    break;

			case 10:
			case 7:
			case 6:
			case 11:
			    break;

			default:
			    fatal = 1;
			    qprintf( "hardware error on %s\n", pb->pb.name );
			    break;
			}
		    wb( SDB, sdb );
		    i = 0;
		    if( fatal )
			return( msg_failure );
		    }
		else if( sdb & SDB_SYNCING )
		    {
		    qprintf( "waiting for drives to spin up on %s\n", pb->pb.name );
		    wb( SDB, sdb );
		    i = 0;
		    }
		}
	    }
	else
	    {
	    for( i = 0; i < 300; i++ )
		if( rb( SDB ) & SDB_MBX_DONE )
		    {
		    wb( SDB, SDB_MBX_DONE );
		    wb( LDB, LDB_MBX_DONE );
		    krn$_sleep( 100 );
		    }
		else
		    return( msg_success );
	    break;
	    }
	krn$_sleep( 1000 );
	}
    qprintf( "adapter not responding on %s\n", pb->pb.name );
    return( msg_failure );
    }

void dac960_new_unit( struct dac960_pb *pb, int index, int unit, int capacity,
	struct system_drive *system_drive )
    {
    char *status;
    char *raid;
    char name[32];
    int i;
    struct sb *sb;
    struct ub *ub;
    struct INODE *inode;

    sb = malloc_noown( sizeof( *sb ) );
    pb->pb.sb[index] = sb;
    sb->pb = pb;
    strcpy( sb->name, pb->pb.name );
    ub = malloc_noown( sizeof( *ub ) );
    ub->sb = sb;
    ub->pb = pb;
    ub->unit = unit;
    /*
     *  Create a new "file".
     */
    set_io_name( name, 0, ub->unit, 0, pb );
    allocinode( name, 1, &inode );
#if ( STARTSHUT || DRIVERSHUT )
    inode->dva = dr_ddb_ptr;
#else
    inode->dva = &dac960_ddb;
#endif
    inode->attr = ATTR$M_READ;
    *(INT *)inode->len = (INT)capacity * BLOCK_SIZE;
    inode->bs = BLOCK_SIZE;
    /*
     *  Set up required pointers and back-pointers.  The INODE points to the
     *  UB, the UB points to the SB, the SB points to the UB.
     */
    inode->misc = ub;
    ub->inode = inode;
    sb->first_ub = ub;
    INODE_UNLOCK( inode );
    switch( system_drive->raid & 0x7f )
	{
	case 0:
	    raid = "RAID 0";
	    break;

	case 1:
	    raid = "RAID 1";
	    break;

	case 5:
	    raid = "RAID 5";
	    break;

	case 6:
	    raid = "RAID 0+1";
	    break;

	case 7:
	    raid = "JBOD";
	    break;

	default:
	    raid = "RAID";
	    break;
	}
    switch( system_drive->status )
	{
	case 3:
	    status = "";
	    break;

	case 4:
	    status = "Degraded";
	    break;

	case 255:
	    status = "Failed";
	    break;

	default:
	    status = "Unknown";
	    break;
	}
    strcpy( ub->version, status );
    set_io_alias( ub->alias, 0, ub->unit, pb );
    sprintf( ub->info, "%d Member %s",
	    system_drive->arms * system_drive->spans,
	    raid );
#if ( STARTSHUT || DRIVERSHUT )
    sprintf( ub->string, "%-18s %-8s  %20s  %5s",
	    name, ub->alias, ub->info, ub->version );
#else
    sprintf( ub->string, "%-24s   %-8s   %24s  %s",
	    name, ub->alias, ub->info, ub->version );
#endif
    insert_ub( ub );
#if DEBUG
    fprintf( el_fp, msg_dev_found, inode->name, ub->info, sb->name );
#endif

#if ( STARTSHUT || DRIVERSHUT )
    if (pb->pb.state == TRUE)
	printf( "%-18s %-8s  %20s  %5s\n", ub->inode->name, ub->alias, ub->info, ub->version );
#endif

    if( system_drive->status != 3 )
	{
	if( system_drive->status == 4 )
	    status = "degraded";
	else if( system_drive->status == 255 )
	    status = "failed";
	else
	    {
	    sprintf( name, "unknown (0x%02x)", system_drive->status );
	    status = name;
	    }
	fprintf( el_fp, "device %s status is %s\n", inode->name, status );
	}
    }

#if DAC960_DISKS_AND_TAPES
int dac960_command( struct scsi_sb *sb, int lun, unsigned char *cmd, int cmd_len,
	unsigned char *dat_out, int dat_out_len,
	unsigned char *dat_in, int dat_in_len, unsigned char *sts )
    {
    int i;
    int j;
    int mbx_status;
    struct mbx_3 mbx;
    struct dcdb dcdb;
    struct dac960_pb *pb;

    if( lun )
	return( msg_failure );
    pb = sb->pb;
    i = sb->node_id / 100;
    j = sb->node_id % 100;
    mbx.opcode = OPCODE_DCDB;
    mbx.pointer = (unsigned long)&dcdb | io_get_window_base( pb );
    dcdb.address = ( i << 4 ) + j;
    dcdb.control = 0xe0;
    if( dat_out_len )
	{
	dcdb.control = 0xe2;
	dcdb.dat_in_out_len = dat_out_len;
	dcdb.dat_in_out = (unsigned long)dat_out | io_get_window_base( pb );
	}
    if( dat_in_len )
	{
	dcdb.control = 0xe1;
	dcdb.dat_in_out_len = dat_in_len;
	dcdb.dat_in_out = (unsigned long)dat_in | io_get_window_base( pb );
	}
    dcdb.cdb_len = cmd_len;
    memcpy( dcdb.cdb, cmd, cmd_len );
    krn$_wait( &pb->owner_s );
    mbx_status = dac960_issue_mbx( pb, &mbx );
    krn$_post( &pb->owner_s );
    if( mbx_status )
	{
	fprintf( el_fp, "DCDB (%02x) to channel %d, target %d failed with status %04x (%02x) on %s\n",
		cmd[0], i, j, mbx_status, dcdb.sts, pb->pb.name );
	return( msg_failure );
	}
    else
	{
	*sts = dcdb.sts;
	return( msg_success );
	}
    }

void dac960_do_inquiries( struct dac960_pb *pb )
    {
    unsigned char dat_in[MAX_INQ_SZ];
    int i;
    int j;
    int mbx_status;
    struct mbx_3 mbx;
    struct dcdb dcdb;

    for( i = 0; i < NUM_CHANNELS; i++ )
	for( j = 0; j < NUM_TARGETS; j++ )
	    {
	    mbx.opcode = OPCODE_DCDB;
	    mbx.pointer = (unsigned long)&dcdb | io_get_window_base( pb );
	    dcdb.address = ( i << 4 ) + j;
	    dcdb.control = 0x91;
	    dcdb.dat_in_out_len = sizeof( dat_in );
	    dcdb.dat_in_out = (unsigned long)dat_in | io_get_window_base( pb );
	    dcdb.cdb_len = 6;
	    dcdb.cdb[0] = scsi_k_cmd_inquiry;
	    dcdb.cdb[1] = 0;
	    dcdb.cdb[2] = 0;
	    dcdb.cdb[3] = 0;
	    dcdb.cdb[4] = sizeof( dat_in );
	    dcdb.cdb[5] = 0;
	    mbx_status = dac960_issue_mbx( pb, &mbx );
	    if( mbx_status )
		if( ( mbx_status == 0x000e ) || ( mbx_status == 0x0105 ) )
		    ;
		else
		    fprintf( el_fp, "DCDB (INQUIRY) to channel %d, target %d failed with status %04x on %s\n",
			    i, j, mbx_status, pb->pb.name );
	    else
		{
		fprintf( el_fp, "DCDB (INQUIRY) to channel %d, target %d succeeded on %s\n",
			i, j, pb->pb.name );
		hd_helper( el_fp, 0, dat_in, dcdb.dat_in_out_len, 1, 1 );
		dac960_device_state( pb, i, j );
		}
	    }
    }

void dac960_device_state( struct dac960_pb *pb, int channel, int target )
    {
    unsigned char device_state[12];
    int mbx_status;
    struct scsi_sb *sb;
    struct mbx_2 mbx;

    if( pb->old )
	mbx.opcode = OPCODE_DEVICE_STATE_OLD;
    else
	mbx.opcode = OPCODE_DEVICE_STATE;
    mbx.channel = channel;
    mbx.target = target;
    mbx.pointer = (unsigned long)device_state | io_get_window_base( pb );
    mbx_status = dac960_issue_mbx( pb, &mbx );
    if( mbx_status )
	fprintf( el_fp, "DEVICE_STATE to channel %d, target %d failed with status %04x on %s\n",
		channel, target, mbx_status, pb->pb.name );
    else
	{
	if( device_state[0] == 0 )
	    {
	    fprintf( el_fp, "device at channel %d, target %d not part of a RAID set on %s\n",
		    channel, target, pb->pb.name );
	    sb = malloc_noown( sizeof( *sb ) );
	    sb->pb = pb;
	    sb->ub.flink = &sb->ub.flink;
	    sb->ub.blink = &sb->ub.flink;
	    sb->command = dac960_command;
	    sb->ok = 1;
	    sb->node_id = channel * 100 + target;
	    sb->sdtr = 0;
	    set_io_name( sb->name, 0, 0, sb->node_id, pb );
	    strcpy( sb->name + 3, sb->name + 4 );
	    krn$_post( &pb->owner_s );
	    scsi_send_inquiry( sb );
	    krn$_wait( &pb->owner_s );
	    }
	}
    }
#endif

int dac960_read( struct FILE *fp, int ilen, int inum, unsigned char *c )
    {
    return( dac960_transfer( fp, ilen * inum, c, 1 ) );
    }

int dac960_write( struct FILE *fp, int ilen, int inum, unsigned char *c )
    {
    return( dac960_transfer( fp, ilen * inum, c, 0 ) );
    }

int dac960_transfer( struct FILE *fp, int len, unsigned char *c, int opcode )
    {
    int lbn;
    int t;
    struct INODE *inode;
    struct ub *ub;

    /*
     *  The file is open.  The MISC field in the INODE structure is used
     *  to hold a UB pointer.
     */
    lbn = *(INT *)fp->offset / BLOCK_SIZE;
    inode = fp->ip;
    ub = inode->misc;
    /*
     *  Don't let transfers go past the end of the unit.
     */
    if( *(INT *)fp->offset + len > *(INT *)inode->len )
	{
	fp->status = msg_eof;
	fp->count = 0;
	t = 0;
	}
    /*
     *  Make sure that the transfer respects block boundaries.
     */
    else if( ( ( len & 511 ) == 0 ) && ( ( *(INT *)fp->offset & 511 ) == 0 ) )
	{
	t = dac960_send_read_write( ub, c, len, lbn, opcode );
	fp->status = t;
	fp->count = len;
	if( t == msg_success )
	    t = len;
	else
	    t = 0;
	/*
	 *  Update the file offset.
	 */
	*(INT *)fp->offset += t;
	}
    else
	{
	err_printf( msg_bad_request, inode->name );
	fp->status = msg_bad_request;
	fp->count = 0;
	t = 0;
	}
    /*
     *  At this point we have kept track of the number of bytes successfully
     *  transferred.  Return that number.
     */
    return( t );
    }

int dac960_send_read_write( struct ub *ub, unsigned char *c, int len, int lbn,
	int opcode )
    {
    void *mbx_ptr;
    int t;
    int mbx_status;
    struct scsi scsi;
    struct mbx_1 mbx_old;
    struct mbx_6 mbx;
    struct dac960_pb *pb;

    pb = ub->sb->pb;
    if( pb->new )
	{
	memset( &scsi, 0, sizeof( scsi ) );
	scsi.cmd_id = 1;
	scsi.cmd_opcode = CMD_SCSI;
	scsi.control = opcode ? 0x10 : 0x00;
	scsi.data_size = len;
	scsi.target = ub->unit % 16;
	scsi.channel = pb->max_channels + ( ub->unit / 16 );
	scsi.cdb_size = 10;
	scsi.cdb[0] = opcode ? scsi_k_cmd_dk_read : scsi_k_cmd_dk_write;
	t = lbn;
	scsi_swap_data( &scsi.cdb[2], &t, 4 );
	t = len / BLOCK_SIZE;
	scsi_swap_data( &scsi.cdb[7], &t, 2 );
	scsi.data_pointer_1 = (unsigned long)c | io_get_window_base( pb );
	scsi.data_size_1 = len;
	mbx_ptr = &scsi;
	}
    else if( pb->old )
	{
	mbx_old.opcode = opcode ? OPCODE_READ_OLD : OPCODE_WRITE_OLD;
	mbx_old.count = len / BLOCK_SIZE;
	mbx_old.block[0] = ( lbn >> 18 ) & 0xc0;
	mbx_old.block[1] = ( lbn >> 0 ) & 0xff;
	mbx_old.block[2] = ( lbn >> 8 ) & 0xff;
	mbx_old.block[3] = ( lbn >> 16 ) & 0xff;
	mbx_old.unit = ub->unit;
	mbx_old.pointer = (unsigned long)c | io_get_window_base( pb );
	mbx_ptr = &mbx_old;
	}
    else
	{
	mbx.opcode = opcode ? OPCODE_READ : OPCODE_WRITE;
	mbx.count = len / BLOCK_SIZE;
	mbx.block = lbn;
	mbx.unit = ub->unit;
	mbx.pointer = (unsigned long)c | io_get_window_base( pb );
	mbx_ptr = &mbx;
	}
    krn$_wait( &pb->owner_s );
    mbx_status = dac960_issue_mbx( pb, mbx_ptr );
    krn$_post( &pb->owner_s );
    if( mbx_status )
	{
	err_printf( "READ/WRITE failed with status %04x on %s\n", mbx_status, pb->pb.name );
	return( msg_failure );
	}
    return( msg_success );
    }

#if !( STARTSHUT || DRIVERSHUT )
void dac960_setmode( struct dac960_pb *pb, int mode )
    {
    switch( mode )
	{
	case DDB$K_STOP:
	    if( pb->pb.mode != DDB$K_STOP )
		{
		krn$_wait( &pb->owner_s );
		if( pb->pb.mode == DDB$K_INTERRUPT )
		    io_disable_interrupts( pb, pb->pb.vector );
		if( pb->old )
		    {
		    wb( MBX + 0, 0x00 );
		    wl( MBX + 8, 0x40080000 );
		    }
		pb->pb.mode = DDB$K_STOP;
		}
	    break;

	case DDB$K_START:
	    if( pb->pb.mode == DDB$K_STOP )
		{
		pb->pb.mode = pb->pb.desired_mode;
		if( pb->pb.type == TYPE_EISA )
		    use_eisa_ecu_data( pb );
		if( pb->board_status == msg_success )
		    pb->board_status = dac960_clear_controller( pb );
		krn$_post( &pb->owner_s );
		}
	    break;

	case DDB$K_READY:
	    if( pb->pb.mode != DDB$K_STOP )
		{
		krn$_wait( &pb->owner_s );
		krn$_post( &pb->owner_s );
		}
	    break;	    

#if 0
	case DDB$K_POLLED:
	    if( pb->pb.mode == DDB$K_STOP )
		{
		pb->pb.mode = DDB$K_POLLED;
		if( pb->pb.type == TYPE_EISA )
		    use_eisa_ecu_data( pb );
		if( pb->board_status == msg_success )
		    pb->board_status = dac960_clear_controller( pb );
		krn$_post( &pb->owner_s );
		}
	    break;
#endif
	}
    }
#endif

#if SHOW_RAID || EB164LOAD || LX164 || SX164 || RAWHIDE

extern struct SEMAPHORE pbs_lock;
extern struct pb **pbs;
extern unsigned int num_pbs;
       
int dac960_show( int argc, char *argv[] )
    {
    int i;
    int j;
    int flag;
    int strcmp_pb( );
    struct dac960_pb *pb;

    if( argc < 3 )
	{
	argc = 3;
	argv[2] = "";
	}
    krn$_wait( &pbs_lock );
    shellsort( pbs, num_pbs, strcmp_pb );
    flag = 0;
    for( i = 2; i < argc; i++ )
	{
	for( j = 0; j < num_pbs; j++ )
	    {
	    if( killpending( ) )
		break;
	    pb = pbs[j];
	    if( ( strcmp( pb->pb.device, "dac960" ) == 0 )
		    && substr_nocase( argv[i], pb->pb.name ) )
		{
		if( flag )
		    printf( "\n\n" );
		printf( "%s, %s at Hose %d, Bus %d, Slot %d (%s)\n",
			pb->pb.name, pb->pb.hardware_name,
			pb->pb.hose, pb->pb.bus, pb->pb.slot,
			pb->pb.type == TYPE_EISA ? "EISA" : "PCI" );
		if( pb->board_status == msg_success )
		    if( pb->new )
			dac960_show_sub_new( pb );
		    else if( pb->old )
			dac960_show_sub_old( pb );
		    else
			dac960_show_sub( pb );
		flag = 1;
		}
	    }
	}
    krn$_post( &pbs_lock );
    return( msg_success );
    }

void dac960_show_sub( struct dac960_pb *pb )
    {
    char info[8+1+16+1];
    char version[4+1];
    char *status;
    int i;
    int j;
    int k;
    int nd;
    int no;
    int nc;
    int mbx_status;
    struct mbx_5 mbx;
    struct enquiry *enquiry;
    struct nvram_config *nvram_config;
    struct system_drive *system_drive;
    struct span *span;
    struct arm *arm;
    struct device_info *device_info;
    struct sb *sb;
    struct ub *ub;

    printf( "\n" );
    enquiry = malloc_noown( sizeof( *enquiry ) );
    nvram_config = malloc_noown( sizeof( *nvram_config ) );
    krn$_wait( &pb->owner_s );
    mbx.opcode = OPCODE_ENQUIRY;
    mbx.pointer = (unsigned long)enquiry | io_get_window_base( pb );
    mbx_status = dac960_issue_mbx( pb, &mbx );
    if( mbx_status )
	printf( "ENQUIRY failed with status %04x\n", mbx_status );
    else
	{
	nd = 0;
	for( i = 0; i < pb->num_drives; i++ )
	    if( enquiry->capacity[i] )
		nd++;
	no = enquiry->num_offline;
	nc = enquiry->num_critical;
	printf( "Firmware Version V%d.%d\n",
		enquiry->fw_ver_major, enquiry->fw_ver_minor );
	printf( "Number of Drives = %d (Optimal = %d, Degraded = %d, Failed = %d)\n",
		nd, nd - nc - no, nc, no );
	printf( "\n" );
	mbx.opcode = OPCODE_NVRAM_CONFIG;
	mbx.pointer = (unsigned long)nvram_config | io_get_window_base( pb );
	mbx_status = dac960_issue_mbx( pb, &mbx );
	if( mbx_status )
	    printf( "NVRAM_CONFIG failed with status %04x\n", mbx_status );
	else
	    {
	    for( i = 0; i < NUM_CHANNELS; i++ )
		for( j = 0; j < NUM_TARGETS; j++ )
		    {
		    if( killpending( ) )
			break;
		    device_info = nvram_config->device_info[i] + j;
		    switch( device_info->status )
			{
			case 0:
			    status = "Dead";
			    break;

			case 2:
			    status = "Write-Only";
			    break;

			case 3:
			    status = "Online";
			    break;

			case 16:
			    status = "Standby";
			    break;

			default:
			    status = "Unknown";
			    break;
			}
		    strcpy( info, "" );
		    strcpy( version, "" );
		    if( dac960_show_inquiry( pb, i, j, 0, info, version ) || device_info->present )
			printf( "Channel %d, Target %d\t\t%24s  %4s  %s\n",
				i, j, info, version, status );
		    }
	    for( i = 0; i < pb->pb.num_sb; i++ )
		{
		if( killpending( ) )
		    break;
		sb = pb->pb.sb[i];
		if( sb )
		    {
		    ub = sb->first_ub;
		    if( ub )
			{
			printf( "\n" );
			printf( "%s\n", ub->string );
			system_drive = nvram_config->system_drive + ub->unit;
			for( j = 0; j < system_drive->spans; j++ )
			    {
			    span = system_drive->span + j;
			    for( k = 0; k < system_drive->arms; k++ )
				{
				arm = span->arm + k;
				printf( "    Channel %d, Target %d, %d MB (LBNs %d to %d)\n",
					arm->channel, arm->target,
					span->num_sectors / 2048, span->start_sector,
    					span->start_sector + span->num_sectors - 1 );
				}
			    }
			}
		    }
		}
	    }
	}
    krn$_post( &pb->owner_s );
    free( enquiry );
    free( nvram_config );
    }

void dac960_show_sub_old( struct dac960_pb *pb )
    {
    char info[8+1+16+1];
    char version[4+1];
    char *status;
    int i;
    int j;
    int k;
    int nd;
    int no;
    int nc;
    int mbx_status;
    struct mbx_5 mbx;
    struct enquiry_old *enquiry;
    struct nvram_config_old *nvram_config;
    struct system_drive_old *system_drive;
    struct arm_old *arm;
    struct disk_old *disk;
    struct device_info_old *device_info;
    struct sb *sb;
    struct ub *ub;

    printf( "\n" );
    enquiry = malloc_noown( sizeof( *enquiry ) );
    nvram_config = malloc_noown( sizeof( *nvram_config ) );
    krn$_wait( &pb->owner_s );
    mbx.opcode = OPCODE_ENQUIRY_OLD;
    mbx.pointer = (unsigned long)enquiry | io_get_window_base( pb );
    mbx_status = dac960_issue_mbx( pb, &mbx );
    if( mbx_status )
	printf( "ENQUIRY failed with status %04x\n", mbx_status );
    else
	{
	nd = 0;
	for( i = 0; i < pb->num_drives; i++ )
	    if( enquiry->capacity[i] )
		nd++;
	no = enquiry->num_offline;
	nc = enquiry->num_critical;
	printf( "Firmware Version V%d.%d\n",
		enquiry->fw_ver_major, enquiry->fw_ver_minor );
	printf( "Number of Drives = %d (Optimal = %d, Degraded = %d, Failed = %d)\n",
		nd, nd - nc - no, nc, no );
	printf( "\n" );
	mbx.opcode = OPCODE_NVRAM_CONFIG_OLD;
	mbx.pointer = (unsigned long)nvram_config | io_get_window_base( pb );
	mbx_status = dac960_issue_mbx( pb, &mbx );
	if( mbx_status )
	    printf( "NVRAM_CONFIG failed with status %04x\n", mbx_status );
	else
	    {
	    for( i = 0; i < NUM_CHANNELS_OLD; i++ )
		for( j = 0; j < NUM_TARGETS_OLD; j++ )
		    {
		    if( killpending( ) )
			break;
		    device_info = nvram_config->device_info[i] + j;
		    switch( device_info->status )
			{
			case 0:
			    status = "Dead";
			    break;

			case 3:
			    status = "Online";
			    break;

			case 16:
			    status = "Standby";
			    break;

			default:
			    status = "Unknown";
			    break;
			}
		    strcpy( info, "" );
		    strcpy( version, "" );
		    if( dac960_show_inquiry( pb, i, j, 0, info, version ) || device_info->present )
			printf( "Channel %d, Target %d\t\t%24s  %4s  %s\n",
				i, j, info, version, status );
		    }
	    for( i = 0; i < pb->pb.num_sb; i++ )
		{
		if( killpending( ) )
		    break;
		sb = pb->pb.sb[i];
		if( sb )
		    {
		    ub = sb->first_ub;
		    if( ub )
			{
			printf( "\n" );
			printf( "%s\n", ub->string );
			system_drive = nvram_config->system_drive + ub->unit;
			for( j = 0; j < system_drive->arms; j++ )
			    {
			    arm = system_drive->arm + j;
			    for( k = 0; k < arm->disks; k++ )
				{
				disk = arm->disk + k;
				printf( "    Channel %d, Target %d, %d MB (LBNs %d to %d)\n",
					disk->channel, disk->target,
					disk->num_sectors / 2048, disk->start_sector,
    					disk->start_sector + disk->num_sectors - 1 );
				}
			    }
			}
		    }
		}
	    }
	}
    krn$_post( &pb->owner_s );
    free( enquiry );
    free( nvram_config );
    }

void dac960_show_sub_new( struct dac960_pb *pb )
    {
    char info[8+1+16+1];
    char version[4+1];
    char *status;
    int i;
    int j;
    int k;
    int nd;
    int no;
    int nc;
    int sc;
    int st;
    int sl;
    int mc;
    int ss;
    int mbx_status;
    struct ioctl ioctl;
    struct controller_info *controller_info;
    struct device_config_info *device_config_info;
    struct phys_device_config_info phys_device_config_info;
    struct phys_device_config_info *saved_phys_device_config_info;
    struct sb *sb;
    struct ub *ub;

    printf( "\n" );
    controller_info = malloc_noown( sizeof( *controller_info ) );
    device_config_info = malloc_noown( sizeof( *device_config_info ) );
    saved_phys_device_config_info = malloc_noown( sizeof( phys_device_config_info ) * 32 );
    krn$_wait( &pb->owner_s );
    memset( &ioctl, 0, sizeof( ioctl ) );
    ioctl.cmd_id = 1;
    ioctl.cmd_opcode = CMD_IOCTL;
    ioctl.control = 0x10;
    ioctl.data_size = sizeof( *controller_info );
    ioctl.ioctl_opcode = IOCTL_GETCONTROLLERINFO;
    ioctl.data_pointer_1 = (unsigned long)controller_info | io_get_window_base( pb );
    ioctl.data_size_1 = sizeof( *controller_info );
    mbx_status = dac960_issue_mbx( pb, &ioctl );
    if( mbx_status )
	printf( "GETCONTROLLERINFO failed with status %04x\n", mbx_status );
    else
	{
	nd = controller_info->num_drives;
	no = controller_info->num_offline;
	nc = controller_info->num_critical;
	printf( "Firmware Version V%d.%d\n",
		controller_info->fw_ver_major, controller_info->fw_ver_minor );
	printf( "Number of Drives = %d (Optimal = %d, Degraded = %d, Failed = %d)\n",
		nd, nd - nc - no, nc, no );
	printf( "\n" );
	for( i = 0xc000; ; i++ )
	    {
	    if( killpending( ) )
		break;
	    memset( &ioctl, 0, sizeof( ioctl ) );
	    ioctl.cmd_id = 1;
	    ioctl.cmd_opcode = CMD_IOCTL;
	    ioctl.control = 0x10;
	    ioctl.data_size = sizeof( phys_device_config_info );
	    ioctl.device = i;
	    ioctl.ioctl_opcode = IOCTL_GETDEVCONFINFOVALID;
	    ioctl.data_pointer_1 = (unsigned long)&phys_device_config_info | io_get_window_base( pb );
	    ioctl.data_size_1 = sizeof( phys_device_config_info );
	    mbx_status = dac960_issue_mbx( pb, &ioctl );
	    if( mbx_status )
		if( mbx_status == 0x0002 )
		    break;
		else
		    printf( "GETDEVCONFINFOVALID failed with status %04x\n", mbx_status );
	    else
		{
		k = phys_device_config_info.device - 0xc000;
		saved_phys_device_config_info[k] = phys_device_config_info;
		sc = phys_device_config_info.channel;
		st = phys_device_config_info.target;
		sl = phys_device_config_info.lun;
		switch( phys_device_config_info.state )
		    {
		    case 0:
			status = "Unconfigured";
			break;

		    case 1:
		    case 9:
			status = "Online";
			break;

		    case 3:
			status = "Rebuild";
			break;

		    case 4:
		    case 16:
			status = "Offline";
			break;

		    case 8:
			status = "Dead";
			break;

		    case 32:
		    case 33:
			status = "Standby";
			break;

		    default:
			status = "Unknown";
			break;
		    }
		strcpy( info, "" );
		strcpy( version, "" );
		dac960_show_inquiry( pb, sc, st, sl, info, version );
		printf( "Channel %d, Target %d, LUN %d\t%24s  %4s  %s\n",
			sc, st, sl, info, version, status );
		i = phys_device_config_info.device;
		}
	    }
	for( i = 0; i < pb->pb.num_sb; i++ )
	    {
	    if( killpending( ) )
		break;
	    sb = pb->pb.sb[i];
	    if( sb )
		{
		ub = sb->first_ub;
		if( ub )
		    {
		    printf( "\n" );
		    printf( "%s\n", ub->string );
		    memset( &ioctl, 0, sizeof( ioctl ) );
		    ioctl.cmd_id = 1;
		    ioctl.cmd_opcode = CMD_IOCTL;
		    ioctl.control = 0x10;
		    ioctl.data_size = sizeof( *device_config_info );
		    ioctl.device = ub->unit;
		    ioctl.ioctl_opcode = IOCTL_GETDEVCONFINFOVALID;
		    ioctl.data_pointer_1 = (unsigned long)device_config_info | io_get_window_base( pb );
		    ioctl.data_size_1 = sizeof( *device_config_info );
		    mbx_status = dac960_issue_mbx( pb, &ioctl );
		    if( mbx_status )
			fprintf( el_fp, "GETDEVCONFINFOVALID failed with status %04x on %s\n", mbx_status, pb->pb.name );
		    else
			{
			mc = device_config_info->member_capacity;
			for( j = 0; j < device_config_info->members; j++ )
			    {
			    ss = device_config_info->raid_device_config_info[j].start_sector;
			    k = device_config_info->raid_device_config_info[j].device - 0xc000;
			    sc = saved_phys_device_config_info[k].channel;
			    st = saved_phys_device_config_info[k].target;
			    sl = saved_phys_device_config_info[k].lun;
			    printf( "    Channel %d, Target %d, LUN %d, %d MB (LBNs %d to %d)\n",
				    sc, st, sl,	mc / 2048, ss, ss + mc - 1 );
			    }
			}
		    }
		}
	    }
	}
    krn$_post( &pb->owner_s );
    free( controller_info );
    free( device_config_info );
    free( saved_phys_device_config_info );
    }

int dac960_show_inquiry( struct dac960_pb *pb, int channel, int target, int lun, char *info, char *version )
    {
    void *mbx_ptr;
    unsigned char dat_in[MAX_INQ_SZ];
    int mbx_status;
    struct scsi scsi;
    struct mbx_3 mbx;
    struct dcdb dcdb;

    if( pb->new )
	{
	memset( &scsi, 0, sizeof( scsi ) );
	scsi.cmd_id = 1;
	scsi.cmd_opcode = CMD_SCSI_PT;
	scsi.control = 0x10;
	scsi.data_size = sizeof( dat_in );
	scsi.lun = lun;
	scsi.target = target;
	scsi.channel = channel;
	scsi.cdb_size = 6;
	scsi.cdb[0] = scsi_k_cmd_inquiry;
	scsi.cdb[4] = sizeof( dat_in );
	scsi.data_pointer_1 = (unsigned long)dat_in | io_get_window_base( pb );
	scsi.data_size_1 = sizeof( dat_in );
	mbx_ptr = &scsi;
	}
    else
	{
	mbx.opcode = OPCODE_DCDB;
	mbx.pointer = (unsigned long)&dcdb | io_get_window_base( pb );
	dcdb.address = ( channel << 4 ) + target;
	dcdb.control = 0x91;
	dcdb.dat_in_out_len = sizeof( dat_in );
	dcdb.dat_in_out = (unsigned long)dat_in | io_get_window_base( pb );
	dcdb.cdb_len = 6;
	dcdb.cdb[0] = scsi_k_cmd_inquiry;
	dcdb.cdb[1] = 0;
	dcdb.cdb[2] = 0;
	dcdb.cdb[3] = 0;
	dcdb.cdb[4] = sizeof( dat_in );
	dcdb.cdb[5] = 0;
	mbx_ptr = &mbx;
	}
    mbx_status = dac960_issue_mbx( pb, mbx_ptr );
    if( mbx_status )
	if( ( mbx_status == 0x000e ) || ( mbx_status == 0x0105 ) )
	    ;
	else
	    printf( "DCDB (INQUIRY) to channel %d, target %d failed with status %04x\n",
		    channel, target, mbx_status );
    else
	{
	dac960_format_inquiry( dat_in, info, version );
	return( 1 );
	}
    return( 0 );
    }

void dac960_format_inquiry( char *inq, char *info, char *version )
    {
    char *c;
    int inq_len;
    int i;

    inq_len = inq[4] + 5;
    if( ( inq[3] & 0x0f ) == 0 )
	{
	/*
	 *  Take the brain-dead TK50 and TZ30 into account.  These devices
	 *  send back only 5 bytes of inquiry data, and the second byte
	 *  distinguishes them apart.
	 */
	if( ( inq_len == 5 ) && ( ( inq[0] & 0x1f ) == 1 ) )
	    {
	    if( inq[1] == 0x80+0x30 )
		strcpy( info, "TZ30" );
	    else if( inq[1] == 0x80+0x50 )
		strcpy( info, "TK50" );
	    else
		strcpy( info, "" );
	    }
	else
	    strcpy( info, "" );
	}
    else
	{
	/*
	 *  Convert the space-padded vendor ID and product ID fields into
	 *  a normal 0-terminated string.
	 */
	c = info;
	if( memcmp( &inq[8], "DEC ", 4 ) == 0 )
	    {
	    memcpy( c, &inq[16], 16 );
	    for( i = 0; i < 16; i++, c++ )
		if( c[0] == ' ' )
		    break;
	    }
	else
	    {
	    memcpy( c, &inq[8], 8 );
	    c += 8;
	    for( i = 0; i < 8; i++, c-- )
		if( c[-1] != ' ' )
		    break;
	    if( inq[16] != ' ' )
		*c++ = ' ';
	    memcpy( c, &inq[16], 16 );
	    c += 16;
	    for( i = 0; i < 16; i++, c-- )
		if( c[-1] != ' ' )
		    break;
	    }
	*c++ = 0;
	sprintf( version, "%4.4s", &inq[32] );
	}
    }

#endif

#if DRIVERSHUT

/*+
 * ============================================================================
 * = dac960_setmode - change driver state				      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * 	This routine changes the state of the port driver.  The following
 * 	states are relevant:
 *
 *	    DDB$K_STOP -
 *			Suspends the  port driver.  dac960_poll stops checking
 *			things in this state, and interrupts from the
 *			device are dismissed.
 *
 *	    DDB$K_RESTARTABLE -
 *			Same as DDB$K_STOP, except the port driver may be
 *			restarted dynamically by the class driver.
 *
 *	    DDB$K_START -
 *			Changes the   port   driver   to   interrupt  mode.
 *			dac960_poll is once again enabled, as well as device
 *			interrupt handling.
 *
 *	    DDB$K_INTERRUPT -
 *			Same as DDB$K_START.
 *  
 *	    DDB$K_ASSIGN  -	
 *			Assigns controller device letters.
 *
 * FORM OF CALL:
 *  
 *	dac960_setmode(mode, pb)
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
 *	struct pb *pb - pointer to port block information
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/
int dac960_setmode(int mode, struct pb *gpb)
{
    int i, j, k, t;
    struct dac960_pb *pb;
    struct sb *sb;
    struct ub *ub;

    switch (mode) {
	case DDB$K_ASSIGN: 
	    break;

	case DDB$K_START: 

	case DDB$K_INTERRUPT:

	    if ( gpb->dpb ) {
		gpb->dpb->ref++;
		return msg_success;
	    } else {
		alloc_dpb( gpb, &pb, sizeof( struct dac960_pb ));
	    }
 
#if 0
	    if (dev->flags)
		return msg_failure;
#endif

	    krn$_set_console_mode(0);

	    break;	    

	case DDB$K_STOP: 

	    pb = gpb->dpb;

	    if (!pb)
		return msg_failure;

	    if (pb->pb.ref == 0)
		return msg_success;

	    pb->pb.ref--;
	    if (pb->pb.ref != 0)
		return msg_success;

	    dac960_clear_controller(pb);

/* Free all vectors */

	    int_vector_clear(pb->pb.vector);

/* Free all semaphores */

	    krn$_semrelease( &pb->owner_s );

/* Free all dynamic memory */

	    for (i = 0; i < pb->pb.num_sb; i++) {
		if (pb->pb.sb[i]) {
		    sb = pb->pb.sb[i];
		    ub = sb->first_ub;
		    if( ub ) {
			remove_ub(ub);		/* Free all unit blocks */
			free(ub);
		    }
		    free(pb->pb.sb[i]);
		}
	    }
	    free(pb->pb.sb);
	    free(pb);

	    gpb->dpb = 0;

	    break;

    }					/* switch */

    return msg_success;

}					/* dac960_setmode */

/*+
 * ============================================================================
 * = dac960_poll_units -							      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *  
 * FORM OF CALL:
 *  
 *	dac960_poll_units(pb, verbose)
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct pb *pb	- Pointer to port block structure of adapter.
 *	int verbose		- if set, print node information.
 *
 * SIDE EFFECTS:
 *
 *	Files will be created.
 *
-*/
int dac960_poll_units(struct pb *gpb, int verbose)
{
    if (dac960_initialize(gpb, TRUE) != msg_success)
	return msg_failure;
}

/*+
 * ============================================================================
 * = dac960_establish_connection - establish a connection to given unit        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine establishes a connection to a given unit.
 *  
 * FORM OF CALL:
 *  
 *	dac960_establish_connection( inode, pb, class )
 *  
 * RETURNS:
 *
 *	success or failure.
 *       
 * ARGUMENTS:
 *
 *	struct INODE *inode	- pointer to inode.
 *	struct pb *pb		- pointer to port block
 *	int class		- class of connection
 *
 * SIDE EFFECTS:
 *
 *	Connection established to given unit.
 *
-*/
int dac960_establish_connection(struct INODE *inode, struct pb *gpb, int class)
{

/* diagnostic mode, node already configured */

    if (diagnostic_mode_flag)
	return msg_success;

/* if device not in file system, fail */

    if (!inode->inuse) {
	return msg_failure;
    }

    if (dac960_initialize(gpb, FALSE) != msg_success)
	return msg_failure;
}

int dac960_initialize(struct pb *gpb, int verbose )
    {

    struct dac960_pb *pb;

    pb = gpb->dpb;

    pb->pb.vector = 0;

    if ( pb->pb.bus == 1 )
	pb->pb.type = TYPE_EISA;
    else
	pb->pb.type = TYPE_PCI;

    /*
     * Using the state filed in the pb to turn on printing of the inode.
     */

    if ( verbose )
	pb->pb.state = TRUE;
    
    pb->pb.ref++;

    dac960_init_port( pb );
    return msg_success;
    }
#endif

