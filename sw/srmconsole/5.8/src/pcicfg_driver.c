/* file:	pcicfg_driver.c
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
 *
 *
 * Abstract:	Pci config driver. This module is used to access
 *		pci configuration space for Medulla.
 *
 * Author:	Joe Smyth
 *
 * Modifications
 *
 *	jje	26-Feb-1996	Conditionalize for Cortex
 *
 *	swt	28-Jun-1995	Add Noritake platform.
 *
 *	bobf	11-Jan-1995	Conditionalized out configuration routines
 *				for Medulla (similar to TURBO)
 *
 *	bobf	23-Aug-1994	Removed old, Medulla debug code
 *
 *	jmp	17-mar-94	Use Hamel Sizing now
 *
 *	jmp	14-feb-94	Add support for mikasa and also integrate auto sizing
 *				to map devices using build_pci_addr routine. Avanti and
 *				mikasa do not use the below_mb_search, so I put 
 *				everything at 100000. I also have the SIO pass 2 
 *				references here also, the actual routine is in the
 *				platform specific module.
 *
 *	jmp	05-jan-1993	Added support for avanti, modified device table
 *				to include mem base and io base addresses. Also
 *				added a machine check flag for sizing. Avanti 
 *				application has pcicfg_driver to sniff and set
 *				the membase/iobase values in the device_info
 *				structure. The device driver then pulls these
 *				values and does it's own specific initialization.
 *				
 *      joes    29-sep-1993 changed to new bus interface functions and moved tulip
 *	                    init code to ew_driver.c
 *
 *	joes	17-Sep-1993	Disable Tulip interrupts when first initing.
 *
 *	bobf	13-Sep-1993	1. added spinlocks/spinunlocks & swap_to_config
 *				   and swap_to_io to the lowest level of config
 *				   access routines.
 *				2. deleted pci_cfgread/write routines.
 *				3. changed Tulip latency counter to 0xff00
 *
 *	joes	13-Aug-1993 	updated for Tulip bringup
 *
 *	b&m	11-Aug-1993	changed local offset to offset and bit shifting
 *				to render reg_num, slot, bus, etc in pcicfg_r/w.
 *
 *	joes	5-Aug-1993 	added creation of global fp for pci config accesses
 *
 *      joes	4-Aug-1993	Added spinlocks to control access to pci space.
 *				this is required for ev4-Dcard because of address space sharing
 *				between pciio and pcicfg spaces. A bit in a local csr flips
 *				between the two.
 *
 *      joes	4-Aug-1993	removed pcicfgsync semaphore. Not needed.
 *				MB() will do the job ok!
 *
 *	bobf	30-Jul-1993 	Medulla Debug:
 *				1. corrected semaphore csr_ready initial value
 *				2. commented out Pci_Configure
 *				3. commented out krn$_waits in swap_to_io
 *				4. commented out krn$_waits in swap_to_config
 *				5. corrected argument order on fread/fwrites
 *				6. removed swap_to_config in pcicfg_open
 *				7. added swap_to_config/swap_to_io in
 *				   pcicfg_read and pcicfg_write
 *				
 *	joes	27-May-1993	first pass leveraged off eisa driver
 *  
 *  	joes	8-Jun-1993	added limited support for multiple buses and functions
 *  
 *	joes	9-Jun-1993	added support in build_pci_addr_space() for multi-
 *				function devices. pci_cfgtab array now replaced by a linked list of
 *				structures containing all the devices location info. This is
 *				created at init time.
 */


#include        "cp$src:platform.h"
#include    "cp$src:kernel_def.h"
#include    "cp$src:dynamic_def.h"
#include    "cp$src:common.h"
#include "cp$inc:prototypes.h"
#include    "cp$src:msg_def.h"
#include    "cp$inc:platform_io.h"
#include    "cp$src:pb_def.h"
#include    "cp$src:ev_def.h"		

#define malloc(x) dyn$_malloc(x,DYN$K_SYNC|DYN$K_NOOWN)
#define free(x) dyn$_free (x,DYN$K_SYNC|DYN$K_NOOWN)

int pcicfg_read  (struct FILE *fp,int size,int number,unsigned char *c);
int pcicfg_write (struct FILE *fp, int size, int number, unsigned char *buf);
int pcicfg_open  (struct FILE *fp, char *info, char *next, char *mode);
int pcicfg_close (struct FILE *fp);

extern int null_procedure ();

#if ( (!TURBO) && (!MEDULLA) && (!CORTEX) && (!APC_PLATFORM) )

void          pci_mikasa_init () ;
void          pci_noritake_init () ;
struct        pci_device_info *get_dev_info(unsigned long dev_vend_id);
unsigned int  pci_configure ();
int           build_pci_addr_space ();
int           check_device_present();

struct  pci_device_info 
    	    {
    	    unsigned long dev_vend_id;
    	    int   (*init)();
    	    char  *pci_name;
	    };

#define LocateAnyWhere32 0
#define LocateBelowMb    2
#define LocateAnyWhere64 4
#define LocateReserved   6
#define LocatePrefetch 	 8
#define LocateMemory	 1
#define IoSpaceEnable    1
#define MemSpaceEnable   2

#define Onek          (1024)
#define ThirtyTwok    (32*1024)
#define FortyEightk   (48*1024)
#define SixtyFourk    (64*1024)
#define OneMeg	      (1024*1024)
#define SixteenMeg    (16*OneMeg)
#define ThirtyTwoMeg  (32*OneMeg)
#define OneHundredTwentyEightMeg (128*OneMeg)
#define TwoHundredFiftySixMeg    (256*OneMeg)
#define TwoGig        (2048*OneMeg)

#define  INTEL_SIO  	0x04848086
#define  ATI_GFX	0x41581002

struct  pci_device_info pci_devtab [] = 
  {
  INTEL_SIO, null_procedure,  "Intel SIO" ,
  NCR_810,   null_procedure,  "ncr_810" ,  
  DEC_TGA,   null_procedure,  "dec_tga" , 
  DEC_TULIP, null_procedure,  "dec_tulip",
  ATI_GFX,   null_procedure,  "Ati Graphics",
  };


/* Global flag set by master abort ISR */
int master_abort_flag;
int ExpectMachineCheck=FALSE;

/*
 * configuration table used by drivers to find base addresses for PCI I/O
 * devices.
 */
unsigned long pci_cfgtab[PCI_K_MAX_SLOTS];    

/*
 * The structure pci_dev_loc specifies the bus#, slot# and function#
 *  of a pci function and it's device/vendor id.
 */

volatile struct pci_dev_loc {
		char bus;
		char slot;
		char function;
		char addr_allocated;
		unsigned long dev_vend_id;
		struct pci_dev_loc *flink;
	};

struct pci_dev_loc root_device = {
		0,
		0,
		0,
		0,
		0,
		NULL
};

/* 	This spinlock is used by the functions that access the I/0 and 
	Config spaces on an EV4-DCard to do synchronisation on the CSR that
	controls the switching between I/O and config spaces */

struct LOCK _align (QUADWORD) spl_pci = {0,IPL_SYNC, 0, 0, 0, 0, 0};

enum pci_status_type
	{
	PciSuccess,
        PciNoDevices,
	PciAddressConflict,
	};

#define  BusMaster 4

#endif

struct DDB pcicfg_ddb = {
	"pcicfg",			/* how this routine wants to be called	*/
	pcicfg_read,		/* read routine				*/
	pcicfg_write,		/* write routine			*/
	pcicfg_open,		/* open routine				*/
	pcicfg_close,		/* close routine			*/
	null_procedure,		/* name expansion routine		*/
	null_procedure,		/* delete routine			*/
	null_procedure,		/* create routine			*/
	null_procedure,		/* setmode 				*/
	null_procedure,		/* validation routine			*/
        0,                      /* class specific use                   */
	1,			/* allows information 			*/
	0,	  		/* must be stacked 			*/
	0,			/* is a flash update driver		*/
	0,			/* is a block device 			*/
        0,                      /* not seekable                         */
	0,                      /* is an ethernet device                */
	0,                      /* is a filesystem driver               */
};



/*+
 * ============================================================================
 * = pcicfg_init - Initialization routine for the PCI config space driver.    =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routines initialises the PCI config space. 	
 *	It's primary function is to initialize the driver data base.
 *      It also calls a function that builds a map of the contents of each 
 *	PCI slot and allocates address ranges to the devices in config space.
 *
 * FORM OF CALL:
 *
 *	pcicfg_init (); 
 *      
 * RETURNS:
 *   
 *	int Status - msg_success or FAILURE.
 *
 * ARGUMENTS:
 *
 *	int hose - hose
 *      
 *      None
 *
 * SIDE EFFECTS:               
 *      
 *      pci global slot map is filled out with vendor and device id's.
 *		0xffffffff slot entry == no device present.
 *   
-*/                            

int pcicfg_init (int hose)
{
	struct INODE *ip;		/* Pointer to INODE for this driver */
	int status, count;
	unsigned long scratch = 0x0;
	char inode_name[MAX_NAME];    

/* Fill in i-node for pcicfg */

#if TURBO
    sprintf(inode_name,"%s%x", pcicfg_ddb.name, hose);
    allocinode (inode_name, 1, &ip);
#else
    allocinode (pcicfg_ddb.name, 1, &ip);
#endif

    ip->dva = & pcicfg_ddb;
    ip->attr = ATTR$M_READ | ATTR$M_WRITE;
	ip->len[0] = 256;
	ip->ref = 0;

	ip->misc = hose;
	INODE_UNLOCK (ip);

    return msg_success;
}



/*+
 * ============================================================================
 * = pcicfg_open - Open PCI config space.                                            =
 * ============================================================================
 *
 * OVERVIEW:         
 *  
 *  Open the pci config driver.
 *  The info field may contain information on the device to be accessed in the
 *  following format. The string should be in hex digits:
 *   _______________________________________________________________________________
 *  |3 3 3 3 3 3 3 3 3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1                    |
 *  |9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0|
 *  |-------------------------------------------------------------------------------|
 *  |<-hose number->|<-bus number-> |<-- slot no -->|<-function no->|<-register no->|
 *   -------------------------------------------------------------------------------
 *  
 * FORM OF CALL:
 *  
 *	pcicfg_open (fp, info, next, mode)
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
 *	The info field is converted into a hex number and copied into the offset
 *  field.
 *
-*/

int pcicfg_open (struct FILE *fp, char *info, char *next, char *mode)
{
 struct INODE *ip;

 /* wait for config space to be released by others */
 ip = fp->ip ;                                    


 fp->offset = &fp->local_offset;
 memset (&fp->local_offset, 0, sizeof (fp->local_offset));
        
 /*
  * Treat the info field as a hex offset if it is present
  */

 if (info  &&  *info) {
    common_convert (info, 16, &fp->local_offset, sizeof (fp->local_offset));
 }

 return(msg_success);
}


/*+
 * ============================================================================
 * = pcicfg_close - Close PCI config space.                                          =
 * ============================================================================
 *
 * OVERVIEW:       
 *  
 *      This routine closes the PCI config accesses.
 *
 * FORM OF CALL:
 *  
 *	pcicfg_close (fp)
 *  
 * RETURNS:
 *
 *	None
 *       
 * ARGUMENTS:
 *
 *	struct FILE *fp - File to be closed.
 *	
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/

int pcicfg_close (struct FILE *fp)
{
 return msg_success;
}


/*+
 * ============================================================================
 * = pcicfg_read - Read of pci config space.                                         =
 * ============================================================================
 *
 * OVERVIEW:         
 *  
 *  This routine is used to read pci config space. It's primary function
 *	is to hide all the translations needed to access pci config space.
 *  The format for the offset field is:
 *   _______________________________________________________________________________
 *  |3 3 3 3 3 3 3 3 3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1                    |
 *  |9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0|
 *  |-------------------------------------------------------------------------------|
 *  |<-hose number->|<-bus number-> |<-- slot no -->|<-function no->|<-register no->|
 *   -------------------------------------------------------------------------------
 *
 *  Register number specifies an offset in the range 0-256.
 *  Function number specifies a function in a multi-function slot in the
 *  range 0-7.
 *  Slot number selects one of 32 devices on a given bus (21 allowable on
 *  current implementation).
 *  Bus number selects one of 256 possible buses in a system.
 *  Hose number selects one of 256 possible hoses in a system.
 *  
 *
 * FORM OF CALL:
 *  
 *	pcicfg_read (fp,size,number,c);
 *  
 * RETURNS:
 *
 *	number of bytes read
 *       
 * ARGUMENTS:
 *
 *	struct FILE *fp - Pointer to a file.
 *	int size - size of individual access.
 *	int number - number of accesses.
 *	unsigned char *c - buffer to put the data in.
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/                        

int pcicfg_read (struct FILE *fp,int size,int number,unsigned char *c)
{
	unsigned __int64 offset, reg_num ;
	int i ;
	struct pb pb;

	/*Get the offset*/	

	offset = *(unsigned __int64 *) &fp->local_offset;

	/* interpret the offset */

        /* get the register offset */
	reg_num =  offset & 0xff;

        /* Get the function number */
	pb.function = ((offset>>8) & 0x7);

        /* get the slot number */
	pb.slot = ((offset>>16) & 0x3f);

        /* get the bus number */
	pb.bus = ((offset>>24) & 0xff);

        /* get the hose number */
#if TURBO
	pb.hose = fp->ip->misc;
#else
	pb.hose = ((offset>>32) & 0xff);
#endif

        /*If not natural alignment fix it up*/
	if((size != 1) && (size != 2) && (size != 4))
	{
	 number *= size;
	 if(number%4)
	     size = 1;
	 else
	 {
	     number = number/4;
	     size = 4;
	 }
	}

	for(i=0; i<number; i++, c+=size,reg_num += size)
	{
		if (reg_num > 0xff)
		{
			reg_num -=size; /* put us back to last location */
			break;
		}
		

	    /*Access for the correct size*/
	    switch(size)
	    {
	        /*Access by words*/
		case 2:
		    *(unsigned short*)c = incfgw(&pb, reg_num);
		    break;
	        /*Access by longwords*/
		case 4:
		    *(unsigned long*)c = incfgl(&pb, reg_num);
		    break;
	        /*Access by bytes*/
	    	default:
		    *(unsigned char*)c = incfgb(&pb, reg_num);
		    break;
	    }
	}

    /*Set the new offset retaining the old bus, slot, function info */	
    *(__int64 *)(&fp->local_offset) &= ~0xff;
    *(__int64 *)(&fp->local_offset) |= reg_num;

    /*Return the bytes read*/
    return size * number;
}

/*+
 * ============================================================================
 * = pcicfg_write - Pci config space write routine.                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine writes pci config space. Its primary function is to hide
 *	the translations needed to write PCI config space.
 *  The format for the offset field is:
 *   _______________________________________________________________________________
 *  |3 3 3 3 3 3 3 3 3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1                    |
 *  |9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0|
 *  |-------------------------------------------------------------------------------|
 *  |<-hose number->|<-bus number-> |<-- slot no -->|<-function no->|<-register no->|
 *   -------------------------------------------------------------------------------
 *
 *  Register number specifies an offset in the range 0-256.
 *  Function number specifies a function in a multi-function slot in the
 *  range 0-7.
 *  Slot number selects one of 32 devices on a given bus (21 allowable on
 *  current implementation).
 *  Bus number selects one of 256 possible buses in a system.
 *  Hose number selects one of 256 possible hoses in a system.
 *  
 *
 * FORM OF CALL:
 *
 *	pcicfg_write (fp, item_size, number, buf); 
 *                    
 * RETURNS:
 *
 *	int Status - msg_success or FAILURE.
 *
 * ARGUMENTS:
 *
 *  struct FILE *fp - Pointer to the file to be written.
 *	int item_size	- size of item
 *	int number	- number of items
 *	char *buf - Buffer to be written. 
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int pcicfg_write (struct FILE *fp, int size, int number, unsigned char *buf)
{
	unsigned __int64 offset, pci_ptr, data, reg_num;
	int i ;
	struct pb pb;

	offset = *(unsigned __int64 *) &fp->local_offset;

	/* interpret the offset */

        /* get the register offset */
	reg_num =  offset & 0xff;

        /* Get the function number */
	pb.function = ((offset>>8) & 0x7);

        /* get the slot number */
	pb.slot = ((offset>>16) & 0x3f);

        /* get the bus number */
	pb.bus = ((offset>>24) & 0xff);

        /* get the hose number */
#if TURBO
	pb.hose = fp->ip->misc;
#else
	pb.hose = ((offset>>32) & 0xff);
#endif

	/*If not natural alignment fix it up*/
	if((size != 1) && (size != 2) && (size != 4))
	{
	 number *= size;
	 if(number%4)
	     size = 1;
	 else
	 {
	     number = number/4;
	     size = 4;
	 }
	}

	for(i=0; i<number; i++, buf+=size,reg_num += size)
	{
		if (reg_num > 0xff)
		{
			reg_num -=size; /* put us back to last location */
			break;
		}

	    /*Access for the correct size*/
	    switch(size)
	    {
	        /*Access by words*/
		case 2:
		    outcfgw(&pb, reg_num, *(unsigned short*)buf);
		    break;
	        /*Access by longwords*/
		case 4:
		    outcfgl(&pb, reg_num, *(unsigned long*)buf);
		    break;
	        /*Access by bytes*/
	    	default:
		    outcfgb(&pb, reg_num, *(unsigned char*)buf);
	    }
	}

    /*Set the new offset retaining the old bus, slot, function info */	
    *(__int64 *)(&fp->local_offset) &= ~0xff;
    *(__int64 *)(&fp->local_offset) |= reg_num;

    /*Return the bytes read*/
    return size * number;
}

#if ( (!TURBO) && (!MEDULLA) && (!CORTEX) && (!APC_PLATFORM) )


/*
 * get_dev_info() searches the global pci_devtab for a match between
 * the device/vendor id passed in and the pci_devtab entries. 
 */

struct pci_device_info *get_dev_info(unsigned long dev_vend_id)
{
 struct pci_device_info *dev_info ;
 int i,done;
	
 for (i=done=0;!done && i*sizeof(struct pci_device_info)<sizeof (pci_devtab);i++)
	{
    	dev_info = &pci_devtab[i];
	if (dev_info->dev_vend_id == dev_vend_id)done=1;
	}

 return (done ? dev_info : 0);
}


/*+
 * ============================================================================
 * = pci_configure - Configure the pci address space.                                       =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will sniff the pci bus for the any installed devices. 
 *	It fills out the Global PCI Config Table with the relevant data.
 *
 * FORM OF CALL:
 *
 *	pci_configure ();
 *                    
 * RETURNS:
 *
 *	char status 
 *			0 success
 *			1 No devices present
 *			2 Address space conflict
 *
 * SIDE EFFECTS:
 *
 *	A linked list of device locations is created which has an entry for
 *	every device found giving bus#, slot#, function# and id.
 *
-*/

unsigned int pci_configure ()
{
	unsigned int slot, id, cache_l_size;
	int i,dev_cnt = 0, func_no;
	struct pci_dev_loc *curr_device;
	struct pci_dev_loc *next_device;
	/* currently we have only a single bus. Support for multiple buses would be 
	 * added here and in build_pci_addr_space() depending on system topology.
	 */
	struct pb pb;
	int bus = 0;
	int BusMax,FunctionMax,SlotMax;

	BusMax 	    = PCI_K_MAX_BUS;
        FunctionMax = 8;

	SlotMax	    = PCI_K_MAX_SLOTS;

	curr_device = &root_device;
	/* init the port block */
	pb.hose = 0;
	pb.bus = 0;
	pb.slot = 0;
	pb.function = 0;
	ExpectMachineCheck = FALSE;

	for(pb.bus = 0; pb.slot < BusMax; pb.bus++)
	{
	/* Build up the config table for each device */
	for(pb.slot = 0; pb.slot < SlotMax ; pb.slot++)
	{
		for(pb.function = 0; pb.function < FunctionMax; pb.function++)
		{
			/* Read the vendor and device id			*/
			/* Set the Expected Machine Check While Doing this	*/
			/* This is so I don't get dumped on While sniffing	*/
			ExpectMachineCheck = TRUE;
			id = incfgl(&pb, VEND_DEV_ID);
			/* if no device present move to next slot */
			if (check_device_present(id))
			{
				/* Found a PCI device	*/
				curr_device->bus      = pb.bus;
				curr_device->function = pb.function;
				curr_device->slot     = pb.slot;
				curr_device->addr_allocated = 0;
				curr_device->dev_vend_id    = id;
				curr_device->flink          = (struct pci_dev_loc *)malloc(sizeof(struct pci_dev_loc));
				curr_device->flink->flink   = NULL;
				curr_device 		    = curr_device->flink;
				/* increment the count of valid devices */
				dev_cnt++; 

				/* check if we're single or multi-function */
				cache_l_size = incfgl(&pb, CACHE_L_SIZ);

				/* single function device so go to next slot */
				if(cache_l_size & 0x00800000 == 0)break; 
				
				/* We should also put a check in here for prefetch	*/
				/* Since pre-fetch implies dense space access		*/				
			}
			else break; /* next slot please */
		} /* end of function loop */
	} /* end of slot loop */
	} /* end of bus loop */

    /* Clear the Expected Machine Check Flag 	*/
	ExpectMachineCheck = 0;

    /* Any devices present? */
	if (dev_cnt == 0)
		return PciNoDevices; /* no devices present */

	/* Build our PCI address space map */
	if (build_pci_addr_space())
		return PciAddressConflict; /* Address space allocation error */

	/*Otherwise return OK*/
	return PciSuccess;
}


/*
 * build_pci_addr_space() implements the allocating of address space in the PCI
 * bus in accordance with PCI spec 2.0. 
 * To figure out the address space that each base register decodes you write 
 * all 1's to the register and read back the contents. Bits not decoded are
 * cleared. Devices must be aligned to this implicit page size!
 *
 * IO allocation: 
 * Assume a minimum page size of 256 bytes, as per PCI Spec 2.0. That is, every 
 * device's address space will begin on a 256b boundary even if it only takes 
 * up a single longword of space. So go through all the configuration spaces 
 * of the PCI searching for I/O base addresses which decode <256b and start 
 * setting up their base addresses at increments of 256b. Then go back and 
 * search for an I/O device that decodes 512b, align to a 512b boundary 
 * above the current high water mark and set up the register. Then move to 
 * 1kb and so on until all I/O base addresses are allocated.
 *
 * Mem allocation: Similar algorithm to I/O space except you need to find any
 * devices that want to reside in the first 1Mb of mem and allocate them there
 * first. If you find any devices that reside below 1Mb and allocate them there
 * then the minimum page size for all other devices is defaulted to 1Mb and 
 * even if a device only requires a few long words it will be allocated on a 1
 * Mb page boundary. You could implement an algorithm that would fill in the
 * "holes" left by the below 1mb allocation, but it would be horrendously
 * complex to do (by all means be my guest if you'd like to try).
 * The increased page size is to circumvent address contention. The algorithm
 * below has been tested with arrays of addresses used to simulate base 
 * registers and has been found to work correctly. I'm waiting with bated
 * breath for real h/w and devices.
 *                      
 * There is a problem for Medulla: Flash mem is hardwired to the 
 * first 1Mb of mem space. This is'nt a problem for the "standard" Medulla 
 * devices, but you never can tell what somebody will plug into the mezzanine.
 * Any devices plugged into the mezzanine that ask for mem space below 1Mb
 * will cause a PCI init error.
 *
 * Update 17-Jun-1993 Expansion ROM base addresses are handled. They are
 * treated as the 7th base address. The loop which looks at individual base
 * addresses treats this seventh address as a base address.
 *
 * Compromises and Exceptions:
 * This function does not as yet implement the 64 bit addressing aspects of 
 * 64bit PCI and such registers are ignored.
 *
 * Executable mem spaces are not yet catered for.
 *
 */
int build_pci_addr_space ()
{   
	int  i, j;
	struct pb pb;
        unsigned long align_size = MIN_ALIGN_IO;
	unsigned long align_size_mask = MIN_ALIGN_IO_MASK;
	unsigned long cur_mem_min_align = MIN_ALIGN_MEM;
	unsigned long decoded_space, AddressSpace, locate_addr;
	unsigned long temp_long;
	unsigned __int64 offset;
	int below_mb_search = 1; 

	int num_below_mb = 0;
	int reserved_mask;
	int io = 0; /* flag indicating address to be allocated in io (true) or 
                 		* mem (false) address spaces */

	/* declare pointer to linked list of valid devices */
	struct pci_dev_loc *curr_device; 	
	struct pci_dev_loc *next_device; 	

	/*Initial mem hi water mark = 1Mb hardwired to the SIO */
  	unsigned long hi_water_mem = (unsigned long)0x100000 ;
	/* Initial io hi water mark = 64kb hardwired to the SIO */ 
  	unsigned long hi_water_io = (unsigned long)0x10000 ;

	/*init the port block */
	pb.hose = 0;
	pb.bus = 0;
	pb.slot = 0;
	pb.function = 0;
	
	for (;align_size;)
	{
		curr_device = &root_device; 
		for (;curr_device->flink; curr_device=curr_device->flink)
		{
		  /* If all base register addresses not allocated */
		  if (curr_device->addr_allocated != 0x7f) 
		   {
			pb.bus      = curr_device->bus;
			pb.slot     = curr_device->slot;
			pb.function = curr_device->function;
			/* Go check each base address register	*/
			/* For Memory and I/O Space it needs	*/
			for (i = 0; i < 7; i++)
			{
			   /* If base register already allocated..continue */
			   if (curr_device->addr_allocated & (1<<i))continue; 
		           if (i == 6)
				{
		                offset = (unsigned __int64)EXP_ROM_BASE ;
				reserved_mask = 0xfffff800;
				io = 0; 
				}
				else
		                {
				offset = (unsigned __int64) (BASE_ADDR0 + i*4);
				reserved_mask = 0xffffff00;
				}
			  /* Do a config read of base address register	*/
			  /* Find out how much device needs		*/

			  decoded_space = incfgl(&pb, offset); 
			  decoded_space |= reserved_mask; 

			  outcfgl(&pb, offset, decoded_space);
			  decoded_space = incfgl(&pb, offset); 
			  temp_long = decoded_space;

			  /* If zero..then device does not allocate space */
			  /* Such as the SIO bridge chip		  */

			  if (temp_long == 0)
				{curr_device->addr_allocated |= (1<<i);continue;}

			  /* If not rom space, its mem or I/O 	*/
			  if (i != 6) io = temp_long & 1;

			  /* If Device Needs to be below 1Mbyte */
			  /* As indicated by config cycle read  */
			  if (below_mb_search && !io && i != 6)
				{
				 if ((temp_long & 0x7) != LocateBelowMb ) continue;
				 else num_below_mb++;
				}
			  decoded_space &= reserved_mask ; 

			  /* Get the Address Space Needed		*/
			  AddressSpace = (~decoded_space) + 1; 
				
			  /* Special Case TGA since it really wants 	*/
			  /* 128 Mbyte max				*/
			  /* This should go away with pass 3		*/
			  /* TGA does everything in dense space		*/
			  if (curr_device->dev_vend_id==DEC_TGA )
					AddressSpace=OneHundredTwentyEightMeg;
			
		 	  /* If device needs more then current alignment*/
			  /* The continue looping until I get aligned   */
			  if (AddressSpace > align_size)continue;
			  if (io)
				  {
				/* It's IO so use the hi_water_io as allocation base  */
				/* calculate where we want to position this device    */
				/* AddressSpace must equal align_size or else we should have  */
				/* already allocated this address, unless the AddressSpace is */
				/* less than the MIN_ALIGN_IO size			      */
				if (AddressSpace < MIN_ALIGN_IO)
					{
					locate_addr = hi_water_io  ;
					hi_water_io = locate_addr + MIN_ALIGN_IO ;
					/* check for address rollover */
					if ( locate_addr > hi_water_io)
					return msg_failure ;
					}
					else
				        {
				        /* in this case AddressSpace == align_size */
					hi_water_io &= ~align_size_mask;
					hi_water_io += AddressSpace ;
					locate_addr  = hi_water_io  ;
					/* check for address rollover */
					if ( locate_addr < AddressSpace)
						return msg_failure ;
					}

				}	/* End if IO space Calculation	*/
				else
				{
				/* It's MEM so use the hi_water_mem as allocation base  */
				/* calculate where we want to position this device.	*/
				/* AddressSpace must equal align_size or else we should */
				/* have already allocated this address, unless the      */
				/* AddressSpace is less than the cur_mem_min_align size */
				if (AddressSpace < cur_mem_min_align)
					{
					locate_addr  = hi_water_mem  ;
					hi_water_mem = locate_addr + cur_mem_min_align ;
					/* check for address rollover */
					if ( locate_addr > hi_water_mem)
						return msg_failure ;
					}
					else
					{
					/* in this case AddressSpace == align_size */
					hi_water_mem &= ~align_size_mask;
					hi_water_mem += AddressSpace ;
					locate_addr = hi_water_mem  ;
					/* check for address rollover */
					if ( locate_addr < AddressSpace)
						return msg_failure ;
					}
				}	/* End if Memory Space	*/

				/* Check for Device Below 1Mbyte	*/
				/* Might Not be able to fit		*/
				if ((below_mb_search) && (locate_addr > 0xfffff))
						return msg_failure; 

				/* Now that is done..map the device	*/
				/* mark this base addr as allocated */	
				if (io)outcfgl(&pb, offset, locate_addr);
				else outcfgl(&pb, offset, locate_addr);
				curr_device->addr_allocated |= (1<<i);
				if (io)
				{
					/* enable the devices IO space in Command register */
					temp_long = incfgl(&pb, COM_STAT);
					temp_long |= IoSpaceEnable;
					temp_long &= 0xffff; 
					outcfgl(&pb, COM_STAT, temp_long);
				} 
		                else
				{
					/* enable the devices MEM space in Command register */
					/* Set the Bus Master Bits Also			    */
					temp_long = incfgl(&pb, COM_STAT);
					temp_long |= MemSpaceEnable | BusMaster | 0x40;
					temp_long &= 0xffff; 
					outcfgl(&pb, COM_STAT, temp_long);
					outcfgl(&pb, CACHE_L_SIZ,0xff00 );
				} 

			} 	/* End Each Base Register   (i<7) */
		   }		/* End if Address Allocated	  */
 		} 	  	/* End current Device List	  */

		
		/* We will stay in this loop moving the alignment */
		/* Size Up to the next order of 2		  */
		align_size_mask |= align_size ;
		align_size       = align_size << 1;

		/* If the align size is 1 Mbyte and we are looking*/
		/* Then restart the allocation process now	  */
		if ((align_size == 0x100000) && (below_mb_search))
		  {
		  below_mb_search = 0x0;
		  align_size = MIN_ALIGN_IO;
		  align_size_mask = MIN_ALIGN_IO_MASK;
		  if (num_below_mb)
		    {
		     cur_mem_min_align = ONE_MEG;
		     hi_water_mem = ONE_MEG;
		     } else  cur_mem_min_align = MIN_ALIGN_MEM;
	          }		

	} 		  /* End of Major Loop 		*/

	return msg_success ;
}


/*
 * check_device_present() checks for a flag from PAL code or an ISR, MCHK
 * handler, that indicates that a PCI master-abort occurred indicating that no
 * device responded to a config cycle.
 *
 */
int check_device_present(int dev_vend_id)
{
	/* second check in case all 1's not implemented by host to pci bridge
	 * or not required
	 */
	if (dev_vend_id == 0xffffffff)
	{
		master_abort_flag = 0;
		return 0;
	}

	if (master_abort_flag)
	{
		master_abort_flag = 0; /* reset the flag */
		return 0;
	}
	else
		return 1;

}


/*+
 * ============================================================================
 * = pci_sniff - locate a device on the PCI bus.                                       =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routines will sniff the pci bus for the specified module. 
 *	It returns a pointer to an array of longs containing the devices location
 *	information. 
 *
 * FORM OF CALL:
 *
 *	pci_sniff (id); 
 *                    
 * RETURNS:
 *
 *	unsigned long * - pointer to array of longs containing location information.
 *
 * ARGUMENTS:
 *
 *	unsigned long id - id we're looking for.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

unsigned long *pci_sniff (unsigned long id)
{
    struct pci_dev_loc *curr_device ;
    int i = 0;
    unsigned long *thelist, *nextlistloc;
	
	curr_device = &root_device;
	thelist = (unsigned long *)malloc(40); /* we can return up to 19 device locations */
	/* Search the linked list for the ID */
	nextlistloc = thelist;
	
	memset (thelist, 0, 40);

	for (i=0;i<20 && curr_device->flink;curr_device=curr_device->flink)
	 {
          if (curr_device->dev_vend_id == id)
		{	
		*nextlistloc =  (curr_device->bus << 16) | (curr_device->slot << 8) | (curr_device->function);
		nextlistloc++;
		i++;
		} 
	 }

	return thelist;
}

#endif
