/*
 * Copyright (C) 1992, 1993 by
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
 * Abstract:	IDE floppy disk driver - specifically for the Intel 82077AA
 *		floppy disk controller chip
 *
 * Author:	Judith E. Gold                                            
 *
 * Modifications:
 *
 *	
 *
 *	jeg	December 1992		Initial entry  
 *
 *	dtr	6-May-1993		Added a Morgan conditional to
 *					enable/disable the PIC bit for the
 *					Floppy.  If polled is set then the
 *					interrupt enable for the floppy is
 *					disable and if interrupt then it is
 *					enabled.
 *
 *   D. Marsh	8-Jul-1993	        enabled the interrupts on a MORGAN 
 *					system in an attempt to speed up
 *					the code. Changed the MAX_UNITS to
 *				        1 for Morgan so that non-existant
 *					devices don't show up. 
 *					Made the timeout in IDE_READ_RESULT 1,
 *					from 100 to speed up performance.
 *
 *   D. Marsh   3-Aug-1993		Initialized the name string in
 *					ide_new_unit so it prints right 
 *					with a "show device"
 *
 *   D.Marsh    31-AUG-1993             Don't use the boot sector in 
 *					ide_common_io to determine the sectors
 *					per track or number of heads. This
 *					relies on the MSDOS boot sector to 
 *					work, and that may not be valid. Use
 *					the default values instead.
 *
 *  D. Rice and D. Marsh		Dma buffer are now based at 1GB but will
 *					remap to 0.  Added a config call to try
 *					and slow dma down to avaoid the overrun
 *					errors.
 *
 *	jmp	29-Nov-1993		Add avanti support
 *
 *	dpm	2-Dec-1993		use mustang.h instead of cobra.h for 
 *					mustang put the SIO into burst mode for 
 *					DMA
 *
 *	jeg	10-Dec-1993		allow any size (less than end of file)
 *					transfer.  This speeds things up
 *					considerably.
 *
 *	jeg	20-Jan-1994		add support for 2.88 M floppies.
 *				        rename driver from ide_driver to 
 *				        dv_driver.
 *
 *	dtr	29-Sep-1994		Removed the additional platform_io.h 
 *					that was added in previous added.
 *
 *	er	21-Oct-1994		Conditionalize for SD164
 *
 *	dwb	22-Oct-1994		Fix a Hang on Mikasa
 *
 *	er	27-Dec-1994		Conditionalize for EB66
 *
 *	rbb	13-Mar-1995		Conditionalize for EB164
 *  
 *	DTR	24-may-1995		conditioal for IPL_RUN
 *  
 *	swt	28-Jun-1995		add Noritake platform
 *
 *	er	21-Aug-1995		Changed EB platforms to run in polled mode.
 *
 *	mar	25-Jan-1996		Changed floppy driver default from
 *					RX26 to RX* for Sable/Turbo/Rawhide
 *   	 				since the console can not distinguish
 *   	 				between an RX23 or RX26. Sable and
 *					Lynx now ship with RX23's.
 *
 *	tna     11-Nov-1998     Conditionalized for YUKONA
 */

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$src:prototypes.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:ev_def.h"
#include "cp$src:msg_def.h"
#include "cp$src:stddef.h"
#include "cp$src:ide_def.h"
#include "cp$src:ide_csr_def.h"
#include "cp$src:hwrpb_def.h"
#include "cp$src:probe_io_def.h"
#include "cp$src:platform_cpu.h"

#include "cp$src:eisa.h"
#include "cp$src:isp_def.h"
#include "cp$src:ctype.h"
#include "cp$src:ub_def.h"
#include "cp$src:pb_def.h"
#include "cp$src:ide_pb_def.h"
#include "cp$inc:platform_io.h"
#include "cp$src:isacfg.h"
#include "cp$src:mem_def.h"
#include "cp$inc:kernel_entry.h"

#if MODULAR
extern struct DDB *dv_ddb_ptr;
#endif

#if WILDFIRE
extern struct pb stdio_pb;
#endif

#if TURBO || CORTEX || YUKONA || (IPL_RUN >= 20 ) || WFSTDIO
int ide_polled_flag = 1;
#else
int ide_polled_flag = 0;
#endif

#define IDE_NUM_UNITS 1

#define DEBUG_FW 0

#if DEBUG_FW
#include "cp$src:print_var.h"
int dv_trace = 1;			/* hi level trace */
int dv_trace_reg = 0;			/* reg r/w and interrupts */
#define dprintf(cond, format, args) 	if (cond) printf(format, args)
#else
#define dprintf(cond, format, args)
#endif

extern struct HWRPB *hwrpb;
extern int sniff_eisa();
extern struct QUEUE pollq;		/*POLLQ			*/
extern null_procedure( );

int ide_write_csr(unsigned int csr,unsigned int value, struct ide_pb *pb);
unsigned char ide_read_csr(unsigned int csr, struct ide_pb *pb);

#if !STARTSHUT
ide_open();
ide_close();
ide_read();
ide_write();
#endif

ide_setmode();
void ide_poll();
ide_interrupt();
int ide_read_result();
int ide_send_command();
int ide_recalibrate_cmd();
int ide_specify_cmd();
int ide_perp_mode_cmd();
int ide_configure_cmd();
int ide_sense_int_status_cmd();
int ide_seek_cmd();
int ide_rw_data_cmd();
int ide_new_unit();
int ide_check_sr0();
int ide_check_status();
int floppy_spinup();
int floppy_spindown();
int enable_controller();
int jensen_dma_control();
int ide_do_io();
int ide_common_io();

/*
 * This table enumerates the various standard floppy disks supported by the 
 * ide floppy disk controller.
 */
static struct FLOPPY_DEVTAB {
	char name [8];                 /* name for device */
	unsigned int	sect_per_track;/* sectors per track */
	unsigned char	srt;	       /* step rate interval */
	unsigned char	hut;	       /* head unload time - 256 mS */
	unsigned char	datarate;      /* kb per sec sent, */
				       /* 500 Kbps for MFM, 125 Kbps for FM */
	unsigned char   hlt_dma;       /* head load time and ND bit */
	unsigned char	precomp;       /* value to write to DSR */
	unsigned short  maxblk;	       /* maximum number of block on disk */
} floppy_devtab [] = {
	{"ED",	36, 0x09, 0x0e, 3, 0x3a, 7, 5760   }, /* ED (2.88M) */
	{"HD", 	18, 0x0C, 0x0f,	0, 2,    0xc, 2880 }, /* HD (1.44M) */
	{"DD", 	 9, 0x0E, 0x04,	2, 2,    0xe, 1440 }, /* DD (720K)  */
};


#if !STARTSHUT
struct DDB ide_ddb = {
	"ide",			/* how this routine wants to be called	*/
	ide_read,		/* read routine				*/
	ide_write,		/* write routine			*/
	ide_open,		/* open routine				*/
	ide_close,		/* close routine			*/
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

#if !STARTSHUT
/*+
 * ============================================================================
 * = dv_init - DV initialization.                                             =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      Initialize floppy disk controllers.
 *
 * FORM OF CALL:
 *
 *	dv_init (); 
 *
 * RETURNS:
 *
 *	xxxx
 *   
 * ARGUMENTS:
 *
 *	None
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int dv_init( )  
 {
#if SABLE || MIKASA || REGATTA || WILDFIRE || MARVEL || FALCON
    int dv_init_units( );

    find_pb( "82077", sizeof( struct ide_pb ), dv_init_units );
#else
    int found;
    struct ide_pb *pb;
    
    found = config_get_device("FLOPPY");
    if (!found) 
       return msg_success;

    /* allocate memory for the port block */

    pb = (struct ide_pb *)malloc_noown(sizeof(struct ide_pb));

    pb->pb.type = TYPE_EISA;
    pb->pb.hose = EISA_HOSE;

#if EIGER
    /* Required for VMS Ship Kit i.e. dva0.0.0.1000.0 */
    pb->pb.bus = 1;
#endif

    pb->pb.protocol = "dv";

    pb->pb.vector = FLOPPY_VECTOR;

    dv_init_units(pb);

    insert_pb( pb );		/* add this PB to the global PB list */
#endif
    return msg_success;       
 }

#else	/* if !STARTSHUT */

/*+
 * ============================================================================
 * = dv_init - dv initialization.                                             =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      Initialize floppy disk controllers.
 *
 * FORM OF CALL:
 *
 *	dv_init (); 
 *
 * RETURNS:
 *
 *	xxxx
 *   
 * ARGUMENTS:
 *
 *	struct device *dev - pointer to device structure
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int dv_init( struct device *dev )  
 {
    struct ide_pb *pb;

    /* allocate memory for the port block */

    pb = (struct ide_pb *) malloc_noown(sizeof(struct ide_pb) );
    dprintf(dv_trace, "dv_init, pb = %x\n",p_args1(pb));

    /* load pb into device structure */

    dev->devdep.io_device.devspecific = (void *) pb;

    pb->pb.hose = dev->hose;
    pb->pb.slot = dev->slot;
    pb->pb.bus = dev->bus;
    pb->pb.function = dev->function;
    pb->pb.protocol = "dv";

    dv_init_units(pb);

    insert_pb( pb );		/* add this PB to the global PB list */

    return msg_success;

}
#endif

/*+
 * ============================================================================
 * = dv_init_units - DV unit initialization.                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      Initialize units off a floppy disk controller
 *
 * FORM OF CALL:
 *
 *	dv_init_units (struct ide_pb *pb);
 *
 * RETURNS:
 *
 *	xxxx
 *   
 * ARGUMENTS:
 *
 *	struct ide_pb *pb - pointer to IDE port block
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int dv_init_units(struct ide_pb *pb)
{
    struct INODE *ip;
    struct IDE_GBL *idep;
    struct TIMERQ *tq;		
    struct ide_regs ide_reg;
    struct ub *ub;
    int unit_num;
    char name [16];	/* enough to hold DVAx */
 
    /*
     * Allocate an inode for each possible unit
     */
    for ( unit_num = 0; unit_num < IDE_NUM_UNITS; unit_num++) {

        /* Create a new "file".  */
	set_io_name( name, 0, unit_num, 0, pb );
    	allocinode( name, 1, &ip );

#if STARTSHUT
	ip->dva = dv_ddb_ptr;
#else
    	ip->dva = & ide_ddb;
#endif

	/*  assume largest possible value of floppy disk media */
    	ip->len[0] = ip->alloc[0] = 737280;	/* for lowest density */
    	ip->loc = 0; 		/* fill in with memory address later on */
    	ip->attr = ATTR$M_READ;
	ip->bs = BYTES_PER_SECTOR;
    	ub = ip->misc = (int *) malloc_noown(sizeof(struct ub));

	ub->inode = ip;            	/* fill in back pointer */
	ub->pb = (struct pb *) pb;   	/* put it in an unused field! */
	ide_new_unit (ub, unit_num);
    	INODE_UNLOCK (ip);

        pb->ub[unit_num] = ub;		/* stash the ub */

        /* get memory for results buffer */
        pb->results[unit_num] = (unsigned char *)malloc_noown(IDE_MAX_RESULT_BYTES);

	/* and the boot block */
        pb->bs[unit_num] = (struct bootsector *)malloc_noown(BYTES_PER_SECTOR);

    }

    pb->pb.setmode = ide_setmode;
    set_io_name( pb->pb.name, 0, 0, 0, pb );

    if ( ide_polled_flag ) {

        /*Set up the poll queue*/

#if STARTSHUT
        pb->pb.vector = FLOPPY_INT_VECTOR;
        io_issue_eoi (pb, pb->pb.vector);	/* Clear out the 8259's */
#endif
	pb->pqp.flink = & pb->pqp.flink;
        pb->pqp.blink = & pb->pqp.flink;
	pb->pqp.routine = ide_poll;
	pb->pqp.param = pb;
	insq_lock (&pb->pqp.flink, &pollq);
	pb->pb.mode = DDB$K_POLLED;	/* set mode polled */
	pb->pb.desired_mode = DDB$K_POLLED;
    } else {
#if !STARTSHUT
        int_vector_set (pb->pb.vector, ide_interrupt, pb);
 	io_enable_interrupts (pb, pb->pb.vector); 
	pb->pb.mode = DDB$K_INTERRUPT;	/* set mode interrupt */
	pb->pb.desired_mode = DDB$K_INTERRUPT;
#else
#if RAWHIDE
        pb->pb.vector = FLOPPY_INT_VECTOR;
        int_vector_set (pb->pb.vector, ide_interrupt, pb);
        /* enable IIC interrupts -- removed because they're already enabled */
	pb->pb.mode = DDB$K_INTERRUPT;	/* set mode interrupt */
	pb->pb.desired_mode = DDB$K_INTERRUPT;
	outportb(pb, 0x21,0xbb);
#endif
#endif
    }		

    krn$_seminit( &pb->isr_t.sem, 0, "ide_isr"); 
    krn$_init_timer(&pb->isr_t);
    krn$_seminit( &pb->owner_s, 1, "ide_owner" );
    pb->pb.ref++;

    return msg_success;       
}

#if !STARTSHUT
/*+
 * ============================================================================
 * = ide_open - open an IDE device.                                           =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      ide_open() attempts to read from a floppy disk at the specified unit
 *      number (i.e. dva0, dva1, dva2, dva3).  It goes through all devices
 *      specified in the floppy_device table until it can read the disk with no
 *      error.  . 
 *
 * FORM OF CALL:
 *
 *	ide_open (); 
 *
 * RETURNS:
 *
 *	msg_success -	disk file is open.
 *      msg_failure -   disk file was not able to be opened.  Either the format
 *			is not supported, or there is something wrong with the
 *			disk drive.
 *   
 * ARGUMENTS:                                                    
 *
 *	struct FILE *fp - pointer to file.
 *	char *
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int ide_open(struct FILE *fp, char *info, char *next, char *mode)
 {
    struct FLOPPY_DEVTAB *fdtab;
    struct INODE *ip;
    struct ub *ub;
    struct ide_pb *pb;
    int i, t;
    int unit_num; 
    struct ide_regs *ireg;

#if STARTSHUT
    t = start_driver(fp);
    if (t != msg_success) {
/*	stop_driver(fp);  */
	return (t);	
    }
#endif

/* all this rigamarole to access the pb */

    ip = fp->ip;
    INODE_LOCK (ip);
    ub = ip->misc;
    INODE_UNLOCK (ip);
    pb = (struct ide_pb *)ub->pb;
                                
/* obtain unit number from file name.  Assume name is 4th character, 1 digit. */
    unit_num = ip->name[UNIT_POSITION] - '0';

/* reset results buffer */
    memset(pb->results[unit_num],0,IDE_MAX_RESULT_BYTES);

    ireg = IDE_REG_BASE;
    /* try everything in the device table until it works */
    for(i=0;i<(sizeof(floppy_devtab)/sizeof(floppy_devtab[0]));i++)
     {
	fdtab = &floppy_devtab[i];
    	enable_controller(pb,unit_num);		/* reset it */
	ide_write_csr((int)&(ireg->msr_dsr),fdtab->precomp, pb);
    	ide_write_csr((int)&(ireg->dir_ccr),fdtab->datarate, pb);
    	ide_specify_cmd(fdtab,pb);
  	ide_perp_mode_cmd(pb);  
      	floppy_spinup(device_sel[unit_num],pb);
	ide_recalibrate_cmd(pb,unit_num); 
	ub->ref = i;   /* row number in table */	
   	if(ide_read(fp,BYTES_PER_SECTOR,1,pb->bs[unit_num])==
 	       (BYTES_PER_SECTOR * 1))  /* read boot sector */
 	 {
		INODE_LOCK (ip);
		/* set size of disk found */
		ip->len[0] = ip->alloc[0] = fdtab->maxblk * 512;
		INODE_UNLOCK (ip);
		break;			
	 }
	else
	 {
		floppy_spindown(pb);    
	 }
    	fp->local_offset[0] = 0;   	/* must reset this to the beginning */
    	fp->local_offset[1] = 0;	/* of the disk after each attempt */
      } /* end for i */

    if(i >= sizeof(floppy_devtab)/sizeof(floppy_devtab[0]))
	return msg_failure;   /* couldn't read the floppy */

    fp->local_offset[0] = 0;   	/* must reset this to the beginning */
    fp->local_offset[1] = 0;	/* of the disk after each attempt */

    /* we've found the device if we get here */
    /* Treat the info field as a hex offset if it is present*/
    /* Convert and copy it to local offset*/

    common_driver_open (fp, info, next, mode);
    if(*fp->offset % BYTES_PER_SECTOR)
     {
	    err_printf("offset must be a multiple of the sector size\n");
	    return msg_failure;
     }

    pb->cur_cyl = 0;		      /* reset current disk cylinder */

    return msg_success;	
 }


/*+
 * ============================================================================
 * = ide_close - close an IDE device.             =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      .
 *
 * FORM OF CALL:
 *
 *	ide_close (); 
 *
 * RETURNS:
 *
 *	msg_success
 *   
 * ARGUMENTS:
 *
 *	struct FILE *fp - pointer to IDE controller.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int ide_close(struct FILE *fp)
 {
	struct INODE *ip;
	struct ide_pb *pb;
	struct ub *ub;
        int i;

/* all this rigamarole to access the pb */
	ip = (struct INODE *) fp->ip;           
	INODE_LOCK (ip);
        ub = ip->misc;
        pb = (struct ide_pb *)ub->pb;

	floppy_spindown(pb);  

	INODE_UNLOCK (ip);

    	krn$_semrelease( &pb->isr_t.sem); 	/* release isr semaphore */

	return msg_success;
 }


/*+
 * ============================================================================
 * = ide_read - read from an IDE disk.             =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      Reads from an IDE floppy disk.
 *
 * FORM OF CALL:
 *
 *	ide_read (); 
 *
 * RETURNS:
 *
 *	xxxx
 *   
 *  FORMAL PARAMETERS:
 *
 *      struct FILE *fp	Pointer to FILE structure.  This should
 *			correspond to the floppy drive unit.
 *	int size	size of item in bytes
 *	int number 	number of items.
 *	char *buf	Address to write the data to.
 *
 *  RETURN VALUE:
 *
 *	Number of bytes read, or 0 on error.
 *
 *  IMPLICIT INPUTS:
 *
 *      NONE
 *
 *  IMPLICIT OUTPUTS:
 *
 *      NONE
 *
 *  SIDE EFFECTS:
 *
 
 *--
 */

int ide_read( struct FILE *fp, int size, int number, unsigned char *s )
 {
   unsigned int bytes_read;

   bytes_read = ide_common_io(fp,size,number,s,FL_TO_MEM);

   return bytes_read;
 } /* end ide_read() */

/*+                                                         
 * ============================================================================
 * = ide_write - write to IDE disk.             =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      .
 *
 * FORM OF CALL:
 *
 *	ide_write (); 
 *
 * RETURNS:
 *
 *	xxxx
 *   
 *  FORMAL PARAMETERS:
 *
 *      struct FILE *fp	Pointer to FILE structure.  This should
 *			correspond to the IDE controller.??
 *	int size	size of item in bytes
 *	int number 	number of items.
 *	char *buf	Address to write the data to.
 *
 *  RETURN VALUE:
 *
 *	Number of bytes written, or 0 on error.
 *
 *  IMPLICIT INPUTS:
 *
 *      NONE
 *
 *  IMPLICIT OUTPUTS:
 *
 *      NONE
 *
 *  SIDE EFFECTS:
 *
 *
 *--
 */


int ide_write( struct FILE *fp, int size, int number, unsigned char *s )
 {
   unsigned int bytes_written;

   bytes_written = ide_common_io(fp,size,number,s,MEM_TO_FL);
   return bytes_written;
 } /* end ide_write() */

#endif

/*+
 * ============================================================================
 * = ide_common_io - send read or write to a IDE disk.             =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      Send read data or write data command to an IDE floppy disk.
 *
 * FORM OF CALL:
 *
 *	ide_common_io (); 
 *
 * RETURNS:
 *
 *	xxxx
 *   
 *  FORMAL PARAMETERS:
 *
 *      struct FILE *fp	Pointer to FILE structure.  This should
 *			correspond to the floppy drive unit.
 *	int size	size of item in bytes
 *	int number 	number of items.
 *	char *buf	Address to write the data to.
 *	int direction   read or write
 *
 *  RETURN VALUE:
 *
 *	Number of bytes read/written, or 0 on error.
 *
 *  IMPLICIT INPUTS:
 *
 *      NONE
 *
 *  IMPLICIT OUTPUTS:
 *
 *      NONE
 *
 *  SIDE EFFECTS:
 *
 *      NONE
 *--
 */
int ide_common_io( struct FILE *fp, int size, int number, unsigned char *s,
                   int direction)
 {
    int n = 0;
    int t = size * number;
    int i;
    int r;
    char *p;

    p = dyn$_malloc( 8192, DYN$K_SYNC|DYN$K_NOOWN|DYN$K_ALIGN, 8192, 0 );
    while( n < t )
	{
	r = min( t - n, 8192 );
	if( direction == MEM_TO_FL )
	    memcpy( p, s + n, r );
	i = ide_common_io_sub( fp, r, 1, p, direction );
	if( direction == FL_TO_MEM )
	    memcpy( s + n, p, r );
	if( i )
	    n += i;
	else
	    break;
	}
    dyn$_free( p, DYN$K_SYNC|DYN$K_NOOWN );
    return( n );
 }

int ide_common_io_sub( struct FILE *fp, int size, int number, unsigned char *s,
                   int direction)
 {
   struct INODE *ip;
   struct ub *ub;
   struct ide_pb *pb;
   struct FLOPPY_DEVTAB *fdtab;
   struct ide_regs *ireg;
   int cylinder;   	/* desired cylinder */
   int last_cyl;	/* last cylinder to read/write */
   int head = 0;        /* desired head */
   int sector;          /* desired sector */
   int sector_p_t = 0;	/* number of sectors per track */
   int lbn;		/* logical block number */
   int total_heads = 0;
   int unit_num;
   unsigned int bytes_left; /* number of bytes to remaining to read/write */ 
   unsigned int byte_count; /* number of bytes to access on this pass of the loop */
   unsigned int bytes_r_track;  /* number of bytes remaining in a track */
   int i;
   unsigned int max_byte;   /* highest byte accessible on this density floppy */


   bytes_left = size * number;
   ip = fp->ip;

/* must read/write in sector multiples */
   if ((*fp->offset + bytes_left) % BYTES_PER_SECTOR)
    {
	err_printf( msg_bad_request, ip->name );
	fp->status = msg_bad_request;
	fp->count = 0;
	return 0;  	 
    }

   fp->count = 0;			/* intialize it */
   lbn = *fp->offset/BYTES_PER_SECTOR;	/* position in file to read/write */
   ub = ip->misc;       
   pb = (struct ide_pb *)ub->pb; 
   fdtab = &floppy_devtab[ub->ref];

   ireg = IDE_REG_BASE;

/* obtain unit number from file name.  Assume name is 4th character, 1 digit. */
   unit_num = ip->name[UNIT_POSITION] - '0';
   sector_p_t = fdtab->sect_per_track;       
   total_heads = 2;
   max_byte = sector_p_t * 80 * 2 * BYTES_PER_SECTOR;   

   fp->status = msg_success;
   while(bytes_left > 0)
    {
	if (*fp->offset > max_byte)
	{
	    err_printf("Cannot access data beyond an offset of %d\n",
							max_byte-1);
	    err_printf("Offset requested was %d\n",*fp->offset);
	    break;
	}


	/* figure out the cylinder,sector, and head numbers to be read/written */
   	cylinder = lbn / (sector_p_t * total_heads);   
   	head = (lbn % (sector_p_t * total_heads)) / sector_p_t; 
   	sector = lbn % sector_p_t;
	
	/* calculate number of bytes remaining in the track */
	bytes_r_track = (sector_p_t - sector) * BYTES_PER_SECTOR;
	/* do we need to read across tracks? */
	if(bytes_left <= bytes_r_track)
	 {
		if(bytes_left <= MAX_DMA_BYTES)
		 {
			/* number of bytes to access during this DMA */
			byte_count = bytes_left; 
			bytes_left = 0;   /* remaining bytes to access */
		 }
		else
		 {
			byte_count = MAX_DMA_BYTES;
			bytes_left -= byte_count;
		 }
	 }
	else
	 {
		byte_count = bytes_r_track;
		if(byte_count > MAX_DMA_BYTES)
			byte_count = MAX_DMA_BYTES;
		bytes_left -= byte_count;
	 }
        krn$_wait(&pb->owner_s);    /* lock out other units */
	if(ide_do_io(s,pb,unit_num,cylinder,head,sector,sector_p_t,
			ub->ref,direction,byte_count) == 0)
	 {
		krn$_post(&pb->owner_s);
		fp->status = msg_failure;
		break;
	 }
	else 	
	 {      
		krn$_post(&pb->owner_s);
		/* adjust pointers and status flags */
   		*fp->offset += byte_count;
   		fp->count += byte_count;
    	 }
	lbn = *fp->offset/BYTES_PER_SECTOR; /* next logical block to access */	
        s+= byte_count;    /* buffer must be incremented */
    } /* end while */

   return fp->count;
 }  /* end ide_common_io () */

/*+
 * ============================================================================
 * = ide_do_io - sets up read or write to an IDE floppy disk.             =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      Sets up a read or write from an IDE floppy disk.
 *	Seeks to the proper cylinder and sets up the DMA engine.
 *	A read or write is attempted MAX_RETRY number of times.
 *
 * FORM OF CALL:
 *
 *	ide_do_io (); 
 *
 * RETURNS:
 *
 *	xxxx
 *   
 *  FORMAL PARAMETERS:
 *
 *	char *s				- user's buffer
 *      int total_heads			- number of heads
 *	struct ide_pb *pb		- pointer to port block
 *	int unit_num			- driver number
 *	int cylinder			- cylinder to read/write
 *	int head			- head to read/write
 *	int sector_p_t			- number of sectors/track
 *	unsigned int index		- index into device table
 *	int direction			- denotes read or write
 *
 *  RETURN VALUE:
 *
 *	Number of bytes read/written, or 0 on error.
 *
 *  IMPLICIT INPUTS:
 *
 *      NONE
 *
 *  IMPLICIT OUTPUTS:
 *
 *      NONE
 *
 *  SIDE EFFECTS:
 *
 
 *--
 */

int ide_do_io(char *s,struct ide_pb *pb,int unit_num,int cylinder,int head, 
               int sector,int sector_p_t,unsigned int index,int direction,
	       int len)
 {  
   int i;

   for(pb->retries =0;pb->retries < MAX_RETRIES;pb->retries++)  
    {		
     	/* seek to proper cylinder if necessary */
   	if(cylinder != pb->cur_cyl)
         {
		if(ide_seek_cmd(pb,unit_num,head,cylinder) != msg_success)
                 {
			continue;      /* bad transaction */
		 }	           
		pb->cur_cyl = cylinder; /* adjust current cylinder pointer */
         }	

	/* set up ISP chip. DMA into the user's buffer */
	if(ide_configure_cmd(pb) != msg_success)    /* enable fifo */
		continue;
   	jensen_dma_control(direction,s,len,pb);	

   	if(ide_rw_data_cmd(pb,unit_num,cylinder,head,sector,sector_p_t,index,direction) != msg_success)
         {
		/* reset results buffer */
		memset(pb->results[unit_num],0,IDE_MAX_RESULT_BYTES);
		continue;               /* bad transaction */
          }
	else
		break;	                /* good transaction */         
    } /* end for */  

   if(pb->retries >= MAX_RETRIES)
    {
	if(direction == FL_TO_MEM)
   		qprintf("IDE bad sector encountered or read unsuccessful\n");
	else
   		qprintf("IDE bad sector encountered or write unsuccessful\n");
   	return 0;			      /* no bytes read */
    }
   return (BYTES_PER_SECTOR);      /* success if we get here, return # bytes read */

 } /* end ide_do_io()  */
/*+
 * ============================================================================
 * = ide_recalibrate_cmd - send the IDE recalibrate command  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      Disk head will be reset to the first cylinder, track, and sector of 
 *	the disk.
 *
 * FORM OF CALL:
 *
 *	ide_recalibrate_cmd (); 
 *
 * RETURNS:
 *
 *	msg_success		- successful execution
 *	msg_failure 		- unsuccessful execution
 *   
 * ARGUMENTS:
 *
 *      int drive_num - number of the drive to be read;
 *
 * SIDE EFFECTS:
 *
 *      Disk drive head will be moved to cylinder 0, track 0, sector 0.
 *
-*/

int ide_recalibrate_cmd(struct ide_pb *pb, int drive_num)
  {
	unsigned char cmd[IDE_MAX_COMMAND_BYTES];
	int retries;		  	  /* disk may be > 77 tracks.
					   * if so, will need to send 
                                           * recalibrate command twice.
					   */

	for(retries=0;retries<2;retries++)
   	 {
		/* fill in command bytes */
		cmd[0] = IDE_RECALIBRATE; 
		cmd[1] = drive_num;

		pb->isr_t.sem.count = 0;
		if(ide_send_command(cmd,2,pb) != msg_success)
			return msg_failure;
		krn$_start_timer(&pb->isr_t,5000);
		if( !(krn$_wait(&pb->isr_t.sem) & 1))
		 {
			qprintf("floppy drive unit is not present \n");
		 }
		krn$_stop_timer(&pb->isr_t);
		/* clears the interrupt, also checks status */
		if(ide_sense_int_status_cmd(pb,drive_num) != msg_success) /* bad status */
                 {

			/* have already used up retry or not recalibrated */ 
			if( retries || (pb->results[drive_num][2] != 0))
			 {
				qprintf("did not seek to cylinder 0\n");
				qprintf("instead seeked to cylinder %d\n",pb->results[drive_num][2]);
				qprintf("or retries used up\n");
				return msg_failure;	           
			 }
			else
				continue;
		 }
		else      			/* good status */
		 {
			krn$_sleep(15);   
			return msg_success;
		 }
	 } /* end for */
  }

/*+
 * ============================================================================
 * = ide_specify_cmd - send the IDE specify command  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      ide_specify_cmd enables one to set media-specific parameters such as
 *	head load and unload times, and step rate interval.
 *
 * FORM OF CALL:
 *
 *	ide_specify_cmd (); 
 *
 * RETURNS:
 *
 *	msg_success 		- successful execution
 *   
 * ARGUMENTS:
 *
 *	struct floppy_devtab *ftabp - pointer to media device table
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int ide_specify_cmd(struct FLOPPY_DEVTAB *ftab, struct ide_pb *pb)
  {
	unsigned char cmd[IDE_MAX_COMMAND_BYTES];

/* fill in command bytes */
	cmd[0] = IDE_SPECIFY; 
	cmd[1] = (ftab->srt << 4) | ftab->hut; /* step rate interval & head 
						  unload time*/
	cmd[2] = ftab->hlt_dma;   	/* head load time and non-DMA bits */
	if(ide_send_command(cmd,3,pb) != msg_success)
		return msg_failure;
	return msg_success;
  }

/*+
 * ============================================================================
 * = ide_configure_cmd - send the IDE configure command  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      ide_configure_cmd allows one to select special features of the 
 *      controller chip.
 *
 * FORM OF CALL:
 *
 *	ide_configure_cmd (); 
 *
 * RETURNS:
 *
 *	msg_success 		- successful execution
 *   
 * ARGUMENTS:
 *
 *	struct ide_pb *pb 		- pointer to port block
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int ide_configure_cmd(struct ide_pb *pb)
  {
	unsigned char cmd[IDE_MAX_COMMAND_BYTES];
	
/* fill in command bytes */
	cmd[0] = IDE_CONFIGURE; 
	cmd[1] = 0;
	cmd[2] = 0x0b;   			/* EIS,EFIFO,POLL,FIFOTHR=12 */
	cmd[3] = 0;                      /* PRETRK */

#if 0
	pb->isr_t.sem.count = 0;
#endif
	if(ide_send_command(cmd,4,pb) != msg_success)
		return msg_failure;
#if 0
	krn$_start_timer(&pb->isr_t,5000);
	if( !(krn$_wait(&pb->isr_t.sem) & 1))
	 {
	   	qprintf("timeout occurred in sending configure cmd\n"); 
		return msg_failure;
	 }
	krn$_stop_timer(&pb->isr_t);
	if(sem_val)
		return msg_failure;   /* OR ERROR HANDLE HERE */
#endif
	return msg_success;
}
/*+
 * ============================================================================
 * = ide_perp_mode_cmd - send the IDE perpendicular mode command  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      ide_perp_mode_cmd allows compatibility with perpendicular (vertical)
 *	recording disk drives.
 *
 * FORM OF CALL:
 *
 *	ide_perp_mode_cmd (); 
 *
 * RETURNS:
 *
 *	msg_success 		- successful execution
 *   
 * ARGUMENTS:
 *
 *	None
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int ide_perp_mode_cmd(struct ide_pb *pb)
  {
	unsigned char cmd[IDE_MAX_COMMAND_BYTES];

/* fill in command bytes */
	cmd[0] = IDE_PERPENDICULAR_MODE; 
	cmd[1] = 0x84;			/* overwrite, first unit, GAP=WGATE=0 */
	if(ide_send_command(cmd,2,pb) != msg_success)
		return msg_failure;
	return msg_success;
  }

/*+
 * ============================================================================
 * = ide_sense_int_status_cmd - send the IDE sense interrupt status command  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      This command must be issued following a seek, relative seek or 
 *	recalibrate command.  If not issued, the drive will continue to be
 *	busy indefinitely.
 *
 * FORM OF CALL:
 *
 *	ide_sense_int_status_cmd (); 
 *
 * RETURNS:
 *
 *	msg_success 		- successful execution
 *	msg_failure 		- unsuccessful execution
 *   
 * ARGUMENTS:
 *
 *	struct ide_pb *pb	- pointer to port block
 *	int unit_num		- number of drive
 *
 * SIDE EFFECTS:
 *
 *      Disk drive interrupt will be cleared, thus another IDE command may be
 *	executed.
 *
-*/

int ide_sense_int_status_cmd(struct ide_pb *pb, int unit_num)
 {
	unsigned char cmd[IDE_MAX_COMMAND_BYTES];
	struct ide_regs *iregs = IDE_REG_BASE;


	cmd[0] = IDE_SENSE_INT_STATUS;  	/* fill in command block */
	if(ide_send_command(cmd,1,pb) != msg_success) /* send it off           */ 
		return msg_failure;
	if(ide_read_result(pb,2,unit_num) != msg_success) /* read 2 result bytes   */	
		return msg_failure;             

	if(ide_check_sr0(pb,unit_num) != msg_success)
		return msg_failure;
	else
		return msg_success;
 }  /* end ide_sense_int_status_cmd() */


/*+
 * ============================================================================
 * = ide_seek_cmd - send the IDE seek command  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      Places disk read/write head over specified cylinder.
 *
 * FORM OF CALL:
 *
 *	ide_seek_cmd (); 
 *
 * RETURNS:
 *
 *	msg_success 		- successful execution
 *	msg_failure 		- unsuccessful execution
 *   
 * ARGUMENTS:
 *
 *	struct ide_pb *pb -    pointer to ide pork block
 *      int drive_num -        disk driver number (0-3)
 *	int head -	       head to seek to
 *      int desired_cyl -      cylinder number to seek to
 *
 * SIDE EFFECTS:
 *
 *      Disk drive head will be moved to desired cylinder.
 *
-*/
int ide_seek_cmd(struct ide_pb *pb, int drive_num, int head, int desired_cyl)
  {
	unsigned char cmd[IDE_MAX_COMMAND_BYTES];
	
/* fill in command bytes */
	cmd[0] = IDE_SEEK; 
	cmd[1] = drive_num | (head << 2);
	cmd[2] = desired_cyl;

	pb->isr_t.sem.count = 0;
	if(ide_send_command(cmd,3,pb) != msg_success)
		return msg_failure;
	krn$_start_timer(&pb->isr_t,5000);
	if( !(krn$_wait(&pb->isr_t.sem) & 1))
	 {
	   	qprintf("timeout occurred in sending seek cmd\n"); 
	 }
	krn$_stop_timer(&pb->isr_t);
	if(ide_sense_int_status_cmd(pb,drive_num) != msg_success)  /* clears the interrupt */
		return msg_failure;	  /* also checks status byte */
	else
	 {
		krn$_sleep(15);
		return msg_success;
	 }
  }




/*+
 * ============================================================================
 * = ide_rw_data_cmd - send the IDE read_data or write_data command  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      Data is read/written from the disk at the specified drive, head,  
 *	cylinder, and sector.
 *
 * FORM OF CALL:
 *
 *	ide_rw_data_cmd (); 
 *
 * RETURNS:
 *
 *	msg_success		- successful execution
 *	msg_failure		- unsuccessful execution
 *   
 * ARGUMENTS:
 *
 *	struct ide_pb *pb - pointer to ide pork block
 *      int drive_num - disk unit number (0-3)
 *      int cylinder - desired cylinder to be read/written
 *	int sector - desired sector to be read/written
 *	int sector_p_t - sectors/track
 *	unsigned int index - index into floppy device table
 *	int direction - write or read
 *
 * SIDE EFFECTS:
 *
 *      floppy disk may be altered due to a write.
 *
-*/
int ide_rw_data_cmd(struct ide_pb *pb, int drive_num, int cylinder, int head,
                       int sector, int sector_p_t, unsigned int index, 
                       int direction)
  {
	struct ide_regs *iregs = IDE_REG_BASE;
	unsigned char cmd[IDE_MAX_COMMAND_BYTES];
        struct FLOPPY_DEVTAB *fdtab;
        unsigned int density = MFM;	


	fdtab = &floppy_devtab[index];

/* fill in command bytes */
	if(direction == FL_TO_MEM)         /* read */
		cmd[0] = IDE_READ_DATA | (SK << 5) | (density << 6);
	else                               /* write */
		cmd[0] = IDE_WRITE_DATA | (SK << 5) | (density << 6);
	cmd[1] = (head << 2) | drive_num;
	cmd[2] = cylinder;         /* desired cylinder */
	cmd[3] = head;	           /* desired head  */
	cmd[4] = sector + 1;       /* desired sector */
	cmd[5] = N;		   /* 512 bytes/sector assumed */
	cmd[6] = sector_p_t;       /* last sector number in track */ 
	cmd[7] = GPL;              /* gap 3 length */ 
	cmd[8] = 0xff;    	   /* DTL field not used */        

	pb->isr_t.sem.count = 0;
	if(ide_send_command(cmd,9,pb) != msg_success)		/* send it off */
		return msg_failure;
	krn$_start_timer(&pb->isr_t,3000);
	if( !(krn$_wait(&pb->isr_t.sem) & 1))
	 {
	   	qprintf("timeout occurred in sending r/w data cmd\n"); 
		/* get here if no media, unhang controller by writing to FIFO */
		ide_write_csr((int)&(iregs->fifo),0,pb); 
		krn$_sleep(1);
	 }
	krn$_stop_timer(&pb->isr_t);
	if(ide_read_result(pb,7,drive_num) != msg_success) 	/* see if command executed ok */
		return msg_failure;
	if(ide_check_status(pb,drive_num,direction) != msg_success) /* examine status registers */
		return msg_failure;
	return msg_success;
  }  /* end ide_rw_data_cmd() */



/*+
 * ============================================================================
 * = ide_check_sr0 - check status register 0     =
 * ============================================================================
 *
 * OVERVIEW:
 *	Checks the status register 0, necessary after most commands are
 *      issued.
 *
 *
 * FORM OF CALL:
 *
 *	ide_check_sr0 (); 
 *
 * RETURNS:
 *
 *	msg_success			- successful execution
 *	msg_failure			- command not terminated successfully,
 *					  or invalid IDE command
 *   
 * ARGUMENTS:
 *
 *	struct ide_pb *pb   		- pointer to port block
 *	int unit_num			- drive number
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int ide_check_sr0(struct ide_pb *pb, int unit_num)
 {
	char stat_reg;


	switch(pb->results[unit_num][0] & 0xC0)  /* check ST0 register    */
	 {
		case 0: break;		/* normal termination of command */

		case 0x40: 
			   qprintf("IDE command not successfully terminated\n");
			   qprintf("ST0 = %02x\n",pb->results[unit_num][0]);
			   return msg_failure;
	
		case 0x80:		
			   qprintf("Invalid IDE command\n"); 
			   qprintf("ST0 = %02x\n",pb->results[unit_num][0]);
			   return msg_failure;

		case 0xC0:
			   qprintf("SR0 = C0\n");
			   break;	/* ignore this error - it's obtained 
					 * when reseting the controller.     */
	
		default:
			   qprintf("Error, ST0 =%02x\n",pb->results[unit_num][0]);
			   return msg_failure;
	 }
	return msg_success;
 } /* end ide_check_sr0() */

/*+
 * ============================================================================
 * = ide_check_status - check command status  =
 * ============================================================================
 *
 * OVERVIEW:
 *	Checks the status registers after a read, write or verify command is 
 *      issued.
 *
 *
 * FORM OF CALL:
 *
 *	ide_check_status (); 
 *
 * RETURNS:
 *
 *	msg_success			- successful completion
 *	msg_failure			- any error occurred
 *   
 * ARGUMENTS:
 *
 *	struct ide_pb *pb   		- pointer to port block
 *	int unit_num			- drive number
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int ide_check_status(struct ide_pb *pb, int unit_num, int direction)
 {
	char stat_reg;
	int error = 0;

	if(ide_check_sr0(pb,unit_num) != msg_success)	/* look at SR0 first */
		error = 1;		/* flag an error if we find one */

/* Status Register 1 */
	stat_reg = pb->results[unit_num][1];
	if(stat_reg)      		
		error = 1;		/* flag an error if we find one */
	if(stat_reg & SR1$M_MA)
		qprintf("Missing address mark\n");
	if(stat_reg & SR1$M_NW)
	 {
		/* ignore this error except on the last try for a write */
		if((direction == MEM_TO_FL) && (pb->retries == 2))
			err_printf("Disk is write protected\n");
		else
			error = 0;
	 }
	if(stat_reg & SR1$M_ND)
		qprintf("Could not find sector\n");

	if(stat_reg & SR1$M_OR)
		qprintf("Soft error: data overrun or underrun\n");

	if(stat_reg & SR1$M_EN)
	   	qprintf("Tried to read beyond final sector of track\n"); 
 
/* Status Register 2 */	
	stat_reg = pb->results[unit_num][2];
	if(stat_reg)      		
		error = 1;		/* flag an error if we find one */
	if(stat_reg & SR2$M_DD)
		qprintf("CRC error\n");
	if(stat_reg & SR2$M_BC)
		qprintf("Bad track\n");
	


	if(error)
		return msg_failure;
	else
		return msg_success;	
 }

/*+
 * ============================================================================
 * = ide_send_command - send an IDE command  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *     Read each byte in the command buffer into the controller FIFO.  The 
 *     send times out if the controller is not ready to accept a byte within
 *     250 usec.
 *
 * FORM OF CALL:
 *
 *	ide_send_command (); 
 *
 * RETURNS:
 *
 *	msg_success			- successful completion
 *	msg_failure			- command send timed out
 *   
 * ARGUMENTS:
 *
 *	unsigned char *buffer - 	Pointer to the command bytes to send.
 *	int num_cmd_bytes     -	        how many command bytes in this command.
 *
 * SIDE EFFECTS:
 *
 *      command is written to disk driver controller.
 *
-*/

int ide_send_command(unsigned char *buffer, int num_cmd_bytes, struct ide_pb *pb)
  {
	struct ide_regs *iregs = IDE_REG_BASE;
	int i,j;
	int num_tries = 50;

	for(i=0;i<num_cmd_bytes;i++)
	 {
		for(j=num_tries; j>0; j--)
		 {
			/* check for ready in */
			if((ide_read_csr((int)&(iregs->msr_dsr),pb)& MSR_DSR$M_RQM) == 
                           MSR_DSR$M_RQM) 
	         	 {
				/* get next byte */
				ide_write_csr((int)&(iregs->fifo),buffer[i],pb); 
				break;
		 	 }
			else
				krn$_sleep(1);
	         }
		if(j == 0)
		 {
			qprintf("timeout in sending IDE command %02x\n", buffer[0]);
			return msg_failure;
		 }
         } /* end for */
	return msg_success;
  }
/*+
 * ============================================================================
 * = ide_read_result - read result bytes after sending IDE command  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *     Read each byte from the controller FIFO into the result buffer.  The 
 *     read times out if the controller is not ready to send a byte within
 *     250 usec.
 *
 * FORM OF CALL:
 *
 *	ide_read_result ()
 *
 * RETURNS:
 *
 *	msg_success			- successful completion
 *	msg_failure			- read timed out
 *   
 * ARGUMENTS:
 *
 *	struct ide_pb *pb 		- pointer to port block
 *	unsigned int result_len		- number of bytes in result phase
 *	int unit_num			- drive number
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int ide_read_result(struct ide_pb *pb, unsigned int result_len, int unit_num)
  {
	struct ide_regs *iregs = IDE_REG_BASE;
	int i,j;
	int num_tries = 50;
        unsigned char temp;
	

	for(i=0;i < result_len;i++)
	 {
		for(j=num_tries;j>0; j--)
		 {
		  /* check for ready out */
			if((temp = ide_read_csr((int)&(iregs->msr_dsr),pb)& 
                   	  (MSR_DSR$M_RQM | MSR_DSR$M_DIO)) == 
                   	  (MSR_DSR$M_RQM | MSR_DSR$M_DIO))      
		 	 {
		         	pb->results[unit_num][i] = 
			 	      ide_read_csr((int)&(iregs->fifo),pb); 
				break;
		 	 }
			else
		 	 {
				krn$_sleep(1); /* was 100 */
		 	 }	
	         } /* end for j */
		if(j == 0)
		 {
			qprintf("timeout in reading IDE status\n"); 
			return msg_failure;
		 }
/* check for invalid opcode */
		if(i==0 && pb->results[unit_num][0]== 0x80)
		  {
			qprintf("Invalid opcode \n"); 
			return msg_failure;
                  }

         } /* end for */
	return msg_success;
 }

/*+
 * ============================================================================
 * = ide_new_unit - make a unit known to the system                          =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine builds the data structures necessary to make a IDE
 *	unit known to the system.
 *  
 * FORM OF CALL:
 *  
 *	ide_new_unit( )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct ub *ub			- pointer to unit block
 *	int i				- index into floppy device table
 *	int unit_num			- number of unit found
 *
 * SIDE EFFECTS:
 *
 *	A unit block is filled in and put into the ub queue.
 *
-*/
void ide_new_unit( struct ub *ub, int unit_num )
{
    struct ide_pb *pb;

#if WILDFIRE
    pb = ub->pb;
    if( pb->pb.hose == stdio_pb.hose )
#endif
    sprintf( ub->alias, "DVA%d", unit_num );

#if SABLE || TURBO
    /* default controller device name */
    strcpy(ub->info,"RX26/RX23");
#endif

#if RAWHIDE
    /* default controller device name */
    strcpy(ub->info,"RX23");
#endif

#if TURBO || RAWHIDE
    sprintf( ub->string, "%-18s %-24s %5s",
	    ub->inode->name, ub->alias, ub->info);
#else
    sprintf( ub->string, "%-24s   %-8s   %24s",
	    ub->inode->name, ub->alias, ub->info );
#endif

    insert_ub( ub );
}

/*+                      
 * ============================================================================
 * = ide_poll - IDE polling routine, checks for interrupts            =
 * ============================================================================
 *               
 * OVERVIEW:                                                                  
 *  
 *      This routine is the polling routine for the ide driver.
 *	It is called from the timer task. It will simply check for
 *	an interrupt and call the isr if there is one.
 *           
 * FORM OF CALL:                                                 
 *  
 *  	ide_poll ();
 *              
 *                    
 * ARGUMENTS:                           
 *             
 *  	struct ide_pb - pointer to ide port block.
 *                          
-*/             
void ide_poll (struct ide_pb *pb)
{
    struct isp_regs *isp;

#if TURBO
    outportb (pb, 0x536, 0xA);
    if ( inportb(pb,0x536) & 0x80 )
        ide_interrupt(pb);
#else
#if SABLE
    if( io_interrupt_pending( pb, pb->pb.vector ) )
	ide_interrupt( pb );
#else
    isp = 0;

    /* poll and read interrupt service (IS) reg */  
    outportb (pb,(unsigned __int64)&(isp->int1_control), 0x0a);

    if (inportb(pb,(unsigned __int64)&(isp->int1_control))&IDE_INT_MASK) {
        ide_interrupt(pb);	/* if we have an interrupt call the isr */
    }
#endif
#endif
}

#if STARTSHUT

/*+
 * ============================================================================
 * = ide_setmode - stop or start the IDE driver                              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine deals with requests to stop or start the IDE driver.
 *  
 * FORM OF CALL:
 *  
 *	ide_setmode()
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	int mode			- the new mode
 *
 * SIDE EFFECTS:
 *
 *	The driver may be stopped or started.
 *
-*/
int ide_setmode( int mode, struct device *dev )
{
    int i, unit_num;
    struct ide_pb *pb;

    switch ( mode ) {

	case DDB$K_ASSIGN:
	    break;

 	case DDB$K_START:
        case DDB$K_INTERRUPT:
	    dprintf(dv_trace, "dv setmode - start\n", p_args0);

	    pb = (void *) dev->devdep.io_device.devspecific;
	    if ( pb) {
		pb->pb.ref++;
		return msg_success;
	    }

	    krn$_set_console_mode(INTERRUPT_MODE);

            if ( dv_init(dev) != msg_success )
                return msg_failure;
	    break;	    

        case DDB$K_STOP:

	    dprintf(dv_trace, "dv setmode - stop\n", p_args0);
	    pb = (struct ide_pb *) dev->devdep.io_device.devspecific;

	    if (!pb)
		return msg_failure;

            remove_pb (pb);

	    pb->pb.ref--;
	    if (pb->pb.ref != 0)
		return msg_success;
            /* 
             * If running polled, remove poll routine from poll queue. If 
             * interrupt, clear interrupt vectors (Only cleared vectors if
	     * RAWHIDE.  Don't think it would hurt Sable, but didn't want
	     * to chance it.  If it isn't done on MODULAR consoles, things
	     * will blow up)
             */
            if ( ide_polled_flag ) 
	        remq_lock (&pb->pqp.flink);
#if RAWHIDE
	    else {
		if( pb->pb.mode == DDB$K_INTERRUPT )
		    io_disable_interrupts( 0, pb->pb.vector );
		int_vector_clear(pb->pb.vector);
	    }
#endif
            /* release semaphores */
            krn$_semrelease ( &pb->isr_t.sem );
            krn$_semrelease ( &pb->owner_s);

            /* delete timers */
            krn$_release_timer ( &pb->isr_t );

            /* free dynamic memory */
            for ( unit_num = 0; unit_num < IDE_NUM_UNITS; unit_num++ ) {
		remove_ub ( pb->ub[unit_num] );	/* remove UB */
                free ( pb->results[unit_num] );	/* results buffer */
                free ( pb->bs[unit_num] );	/* boot sector params */
                free ( pb->ub[unit_num] );	/* unit block */
            }

            free (pb);		/* port block itself */

	    dev->devdep.io_device.devspecific = 0;

	    break;

 	case DDB$K_POLLED:
        default:
	    err_printf("setmode error - illegal mode.\n");
	    return msg_failure;
 	    break;
     }

}

void ide_config( struct device *dev )
{
    struct ide_pb *pb;
    int unit_num;   

    pb = dev->devdep.io_device.devspecific;
    for ( unit_num = 0; unit_num < IDE_NUM_UNITS; unit_num++) {
    	printf( "%s\n", pb->ub[unit_num]->string);
    }

    return;
}

#else	/* if STARTSHUT */

/*+
 * ============================================================================
 * = ide_setmode - stop or start the IDE driver                              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine changes the state of the port driver. The following
 *	states are relevant:
 *
 *	DDB$K_STOP -	Suspends the port driver.  Interrupts from the
 *			device are dismissed.
 *
 *	DDB$K_RESTARTABLE -
 *			Same as DDB$K_STOP, except the port driver may be
 *			restarted dynamically by the class driver.
 *
 *	DDB$K_START -	Changes the port driver to interrupt mode.
 *			device interrupt handling is once again enabled.
 *
 *	DDB$K_INTERRUPT -
 *			Same as DDB$K_START.
 *
 *	DDB$K_ASSIGN  -	Assigns controller device letters.
 *
 *  
 * FORM OF CALL:
 *  
 *	ide_setmode()
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	int mode			- the new mode
 *
 * SIDE EFFECTS:
 *
 *	The driver may be stopped or started.
 *
-*/
void ide_setmode( struct ide_pb *pb, int mode )
    {
    krn$_wait( &pb->owner_s );
    switch( mode )
	{
 	case DDB$K_STOP:
	    if( pb->pb.mode != DDB$K_STOP )
		{
		if( pb->pb.mode == DDB$K_INTERRUPT )
		    io_disable_interrupts( pb, pb->pb.vector );
		else
		    remq_lock( &pb->pqp.flink );
 		pb->pb.mode = DDB$K_STOP;
		}
 	    break;	    
 
 	case DDB$K_START:
 	    if( pb->pb.mode == DDB$K_STOP )
 		{
		pb->pb.mode = pb->pb.desired_mode;
		if( pb->pb.desired_mode == DDB$K_INTERRUPT )
		    io_enable_interrupts( pb, pb->pb.vector );
		else
		    insq_lock( &pb->pqp.flink, &pollq );
		}
 	    break;
	}
    krn$_post( &pb->owner_s );
    }

#endif

/*+
 * ============================================================================
 * = ide_interrupt - service a port interrupt                                 =
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
 *	ide_interrupt( )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct ide_pb *pb		- port block
 *
 * SIDE EFFECTS:
 *
 *	Waiting tasks may be awoken.
 *
-*/

void ide_interrupt(struct ide_pb *pb)
{
    krn$_post (&pb->isr_t.sem, 1);
    if (pb->pb.vector)
	io_issue_eoi (pb, pb->pb.vector);
}

/*+
 * ============================================================================
 * = enable_controller - reset IDE controller                              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine resets the IDE controller by clearing status bits set from
 *      polling after the controller is reset.
 *  
 * FORM OF CALL:
 *  
 *	enable_controller()
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	struct ide_pb *pb 		- pointer to port block.
 *
 * SIDE EFFECTS:
 *
 *	floppy disk drive will be reset.
 *
-*/
void enable_controller(struct ide_pb *pb,int unit_num)
 {
    struct ide_regs *iregs = IDE_REG_BASE;

#if 0
    /* reset and initialize floppy controller to allow DMAs */
    ide_write_csr((int)&(iregs->dor),0,pb);
    ide_write_csr((int)&(iregs->dor),DOR$M_N_RESET | DOR$M_N_DMA_GATE,pb);

    ide_sense_int_status_cmd(pb,unit_num);   /* clears any interrupt pending */
                                             /* status is checked in sense   */
					     /* int status cmd.  	     */
#endif

    /* reset controller, enable drive motor */
    ide_write_csr((int)&(iregs->dor),DOR$M_N_DMA_GATE,pb);
    ide_write_csr((int)&(iregs->dor),DOR$M_N_RESET | DOR$M_N_DMA_GATE,pb);
    krn$_start_timer(&pb->isr_t,5000);
    if( !(krn$_wait(&pb->isr_t.sem) & 1))
      {
	qprintf("interrupt due to reset never occurred \n");
     }
    krn$_stop_timer(&pb->isr_t);
 }

/*+
 * ============================================================================
 * = floppy_spinup - start up a drive                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine enables the drive specified as an argument and starts its 
 *      motor. 
 *  
 * FORM OF CALL:
 *  
 *	floppy_spinup()
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	int drive		- unit 0, 1, 2 or 3
 *
 * SIDE EFFECTS:
 *
 *	floppy disk and motor may be enabled.
 *
-*/
void floppy_spinup(int drive, struct ide_pb *pb)
 {	
	struct ide_regs *iregs = IDE_REG_BASE;

	ide_write_csr((int)&(iregs->dor),DOR$M_N_RESET | DOR$M_N_DMA_GATE |
		       drive,pb);	/* enable drive and motor */
	krn$_sleep(1000);
	
	
 }

/*+
 * ============================================================================
 * = floppy_spindown - run down a drive                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine disables any enabled drive.
 *  
 * FORM OF CALL:
 *  
 *	floppy_spindown()
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *	int drive		- unit 0, 1, 2 or 3
 *
 * SIDE EFFECTS:
 *
 *	floppy disk and motor may be disabled.
 *
-*/
void floppy_spindown(struct ide_pb *pb)
 {
	struct ide_regs *iregs = IDE_REG_BASE;

    /* disable previously enabled drive and motor */
    ide_write_csr((int)&(iregs->dor),DOR$M_N_RESET | DOR$M_N_DMA_GATE,pb); 
 }

/*+
 * ============================================================================
 * = jensen_dma_control - set up ISP for dma read or write                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine sets up the EISA Integrated System Peripheral chip DMA
 *	controller.  
 *  
 * FORM OF CALL:
 *  
 *	jensen_dma_control( )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *      int direction	-	indicates disk read or write.
 *	char *s		-	pointer to buffer to be read into/written from
 *	unsigned int num_xfer_bytes - number of bytes to be dma'ed
 *
 * SIDE EFFECTS:
 *
 *	Jensen module will be enabled for DMA.
 *
-*/
void jensen_dma_control(int direction, char *s, unsigned int num_xfer_bytes, struct ide_pb *pb)
 {
	struct isp_regs *isp;

	isp = 0; 
	outportb(pb,(unsigned __int64)&(isp->dma2_stat_cmd),0x00);
	outportb(pb,(unsigned __int64)&(isp->dma1_stat_cmd),0x00);
	outportb(pb,(unsigned __int64)&(isp->dma2_write_mode),0xC0);
	if(direction == FL_TO_MEM)	/* if read from disk */
	 {
		/* put the SIO DMA into burst mode */
		outportb(pb,(unsigned __int64)&(isp->dma1_write_mode),0x06);
		outportb(pb,(unsigned __int64)&(isp->dma1_ext_write_mode),0x02);
	 }
	else    /* if write to disk */
	 {
		/* put the SIO DMA into burst mode */
		outportb(pb,(unsigned __int64)&(isp->dma1_write_mode),0x0A); 
		outportb(pb,(unsigned __int64)&(isp->dma1_ext_write_mode),0x02);
         }
	outportb(pb,(unsigned __int64)&(isp->dma1_clear_ptr_ff),0x00);	/* target address */
	outportb(pb,(unsigned __int64)&(isp->dma1_ch2_curadd),(int)s >> 0);
	outportb(pb,(unsigned __int64)&(isp->dma1_ch2_curadd),(int)s >> 8);
	outportb(pb,(unsigned __int64)&(isp->dmapage_ch2),(int)s >> 16);
	outportb(pb,(unsigned __int64)&(isp->dma_highpage_ch2),
	    ((int)s >> 24) | ((int)io_get_window_base(pb) >> 24));
	outportb(pb,(unsigned __int64)&(isp->dma1_clear_ptr_ff),0x00);     /* count */
	outportb(pb,(unsigned __int64)&(isp->dma1_ch2_curcount),(num_xfer_bytes-1) >> 0);
	outportb(pb,(unsigned __int64)&(isp->dma1_ch2_curcount),(num_xfer_bytes-1) >> 8);
	outportb(pb,(unsigned __int64)&(isp->dma1_ch2_curcount_h), (num_xfer_bytes-1) >> 16);

	outportb(pb,(unsigned __int64)&(isp->dma2_write_smask),0x00);   /* enable */
	outportb(pb,(unsigned __int64)&(isp->dma1_write_smask),0x02);
 			
 }	


/*+
 * ============================================================================
 * = ide_write_csr - write IDE csr				          =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *     Write an IDE csr.	
 *  
 * FORM OF CALL:
 *  
 *	ide_write_csr( )
 *  
 * RETURNS:
 *
 *	Nothing.
 *       
 * ARGUMENTS:
 *
 *      unsigned int csr	- csr address
 *	unsigned int value	- data to write to the csr
 *
 * SIDE EFFECTS:
 *
 *      None.
-*/
void ide_write_csr(unsigned int csr,unsigned int value, struct ide_pb *pb)
 {
	outportb(pb,(unsigned __int64)csr,(unsigned char)value);
 }

/*+
 * ============================================================================
 * = ide_read_csr - read IDE csr
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *   	read an IDE csr.	
 *  
 * FORM OF CALL:
 *  
 *	ide_read_csr( )
 *  
 * RETURNS:
 *
 *	value read from csr.
 *       
 * ARGUMENTS:
 *
 *      unsigned int csr	- address of csr to be read
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/
unsigned char ide_read_csr(unsigned int csr, struct ide_pb *pb)
 {
	return(inportb(pb,(unsigned __int64)csr));
 }

