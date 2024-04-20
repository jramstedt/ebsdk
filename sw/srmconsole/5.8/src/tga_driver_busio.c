 /*
 *        Copyright (c) Digital Equipment Corporation, 1990
 *        All Rights Reserved.  Unpublished-rights reserved
 *        under the copyright laws of the United States.
 *
 *        The software contained on this media is proprietary to"
 *        and embodies confidential technology of Digital.  Possession,
 *        use, duplication or dissemination of the software and media
 *        is authorized only pursuant to a valid written license from
 *        Digital.
 *
 *        RESTRICTED RIGHTS LEGEND  Use, duplication, or disclosure"
 *        by the U.S. Government is subject to restrictions as set 
 *        forth in subparagraph (c)(1)(ii) of DFARS 252.227-7013, or 
 *        in FAR 52.227-19, as applicable.
 */



/*
 *++
 *  FACILITY:
 *
 *      sfbp Driver support routines
 *
 *  ABSTRACT:
 *
 *      This module provides support routines for the sfbp console drivers. 
 *	These are routines to do write buffer flush,wait retrace,etc.
 *
 *  AUTHORS:
 *
 *      James Peacock
 *
 *
 *  CREATION DATE:      12-13-90 
 *
 *  MODIFICATION HISTORY:
 *	James Peacock	12-13-90	Port from LCG
 *
 *
 *--
*/
#include	"cp$src:platform.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:stddef.h"
#include	"cp$src:common.h"
#include "cp$inc:prototypes.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:tt_def.h"
#include 	"cp$src:ev_def.h"
#include 	"cp$src:time.h"		
#include 	"cp$src:pb_def.h"
#include        "cp$src:eisa.h"   
#include	"cp$inc:platform_io.h"
#include	"cp$src:display.h"
#include	"cp$src:hwrpb_def.h"

#include "cp$src:tga_sfbp_include.h"
#include "cp$src:tga_sfbp_callbacks.h"
#include "cp$src:tga_sfbp_prom_defines.h"
#include "cp$src:tga_sfbp_def.h"
#include "cp$src:tga_sfbp_protos.h"
#include "cp$src:tga_sfbp_463.h"
#include "cp$src:tga_sfbp_485.h"
#include "cp$src:tga_sfbp_ext.h"
#include "cp$src:tga_sfbp_crb_def.h"
#include "cp$src:tga_sfbp_ctb_def.h"



/* These are constants used by the cfg_read/cfg_write code      */
#define   PCI_byte_mask 0
#define   PCI_word_mask 1
#define   PCI_long_mask 3

void     pci_outmem (unsigned __int64 address, unsigned int value, int length);
int      pci_inmem  (unsigned __int64 address, int length);

extern   struct pb *TGA_pb[];


/*+
 * ===================================================
 * = sfbp$$sfbp_write_pci_config - Write data pci config =
 * ===================================================
 *
 *   
 * OVERVIEW:
 *  
 *     Load data into PCI Config Space 
 *  
 * FORM OF CALL:
 *  
 *     sfbp$$sfbp_write_pci_config (scia_ptr,address,data ,mask);
 *
 * RETURNS:
 *	SS$K_SUCCESS 
 *
 * ARGUMENTS:
 *	scia_ptr - shared driver pointer
 *	address  - Config Register Offset 
 *	data	 - data to load into register
 *      mask     - byte mask
 *
 * IMPLICIT INPUTS:
 *     none
 *  
 * IMPLICIT OUTPUTS:
 *     none
 *
 * FUNCTIONAL DESCRIPTION:
 *      Load PCI Config Space 
 *
 * CALLS:
 *      none
 *
 * ALGORITHM:
 *
--*/
int	sfbp$$sfbp_write_pci_config (SCIA *scia_ptr,int address,int data,int mask )

{

register char   *config;

outcfgl (TGA_pb[data_ptr->sfbp$l_slot_number],address,data); 

return (SS$K_SUCCESS );
}



/*+
 * ===================================================
 * = sfbp$$sfbp_read_pci_config - Read data from ASIC register =
 * ===================================================
 *
 *   
 * OVERVIEW:
 *  
 *     Read data from into ASIC register
 *  
 * FORM OF CALL:
 *  
 *     sfbp$$sfbp_read_pci_config (scia_ptr,address )
 *
 * RETURNS:
 *	SS$K_SUCCESS 
 *
 * ARGUMENTS:
 *	scia_ptr - shared driver pointer
 *	address  - ASIC register
 *
 * IMPLICIT INPUTS:
 *     none
 *  
 * IMPLICIT OUTPUTS:
 *     none
 *
 * FUNCTIONAL DESCRIPTION:
 *      Read PCI Config Register 
 *
 * CALLS:
 *      none
 *
 * ALGORITHM:
 *
--*/
int	sfbp$$sfbp_read_pci_config (SCIA *scia_ptr,int address )
                   
{                  
register int data; 
                                           
data    = incfgl (TGA_pb[data_ptr->sfbp$l_slot_number],address);
        
return (data);
}       
        
       
/*+     
 * ===================================================
 * = sfbp$$bus_write - Bus Write =
 * ===================================================
 *      
 *      
 * OVERVIEW:
 *      
 *     Bus Write Operation to an I/O Address 
 *  
 * FORM OF CALL:
 *  
 *     sfbp$$bus_write (address,data,mask)
 *
 * RETURNS:
 *	SS$K_SUCCESS 
 *
 * ARGUMENTS:
 *	address  - BT459 register
 *	data	 - data to load into register
 *      mask     - mask to to be used
 *
 * IMPLICIT INPUTS:
 *     none
 *  
 * IMPLICIT OUTPUTS:
 *     none
 *
 * FUNCTIONAL DESCRIPTION:
 *      Bus Write Operation using data to the address.
 *
 * CALLS:
 *	none
 *
 * ALGORITHM:
 *	none
 *
--*/
int	sfbp$$bus_write (unsigned int *address,int data,int mask )

{
pci_outmem(((int)address&(~3)),data,PCI_long_mask);
return(SS$K_SUCCESS);
}


/*+                  
 * ===================================================
 * = sfbp$$bus_read - Bus Read =
 * ===================================================
 *
 *   
 * OVERVIEW:
 *  
 *     Bus Read Operation to an I/O Address 
 *  
 * FORM OF CALL:
 *  
 *     sfbp$$bus_read (address,mask)
 *
 * RETURNS:
 *	SS$K_SUCCESS 
 *
 * ARGUMENTS:
 *	address  - Address  to read
 *      mask     - bytemask
 *
 * IMPLICIT INPUTS:
 *     none
 *  
 * IMPLICIT OUTPUTS:
 *     none
 *
 * FUNCTIONAL DESCRIPTION:
 *      Bus Read Operation to address.
 *
 * CALLS:
 *	none
 *
 * ALGORITHM:
 *	none                             
 *                                       
--*/                                     
int	sfbp$$bus_read (int *address)    
                     
{                    
                     
 return (pci_inmem(address,PCI_long_mask));
                     
}                    
                     

/*+
 * ===================================================
 * = sfbp$$bus_config_write - Bus Write =
 * ===================================================
 *
 *   
 * OVERVIEW:
 *  
 *     Bus Write Operation to an I/O Address 
 *  
 * FORM OF CALL:
 *  
 *     sfbp$$bus_config_write (address,data,mask)
 *
 * RETURNS:
 *	SS$K_SUCCESS 
 *
 * ARGUMENTS:
 *	address  - BT459 register
 *	data	 - data to load into register
 *      mask     - mask to to be used
 *
 * IMPLICIT INPUTS:
 *     none
 *  
 * IMPLICIT OUTPUTS:
 *     none
 *
 * FUNCTIONAL DESCRIPTION:
 *      Bus Write Operation using data to the address.
 *
 * CALLS:
 *	none
 *
 * ALGORITHM:
 *	none
 *
--*/
int	sfbp$$bus_config_write (unsigned int *address,int data,int mask )

{
return(SS$K_SUCCESS);
}


/*+
 * ===================================================
 * = sfbp$$bus_config_read - Bus Read =
 * ===================================================
 *
 *   
 * OVERVIEW:
 *  
 *     Bus Read Operation to an I/O Address 
 *  
 * FORM OF CALL:
 *  
 *     sfbp$$bus_config_read (address,mask)
 *
 * RETURNS:
 *	SS$K_SUCCESS 
 *
 * ARGUMENTS:
 *	address  - Address  to read
 *      mask     - bytemask
 *
 * IMPLICIT INPUTS:
 *     none
 *  
 * IMPLICIT OUTPUTS:
 *     none
 *
 * FUNCTIONAL DESCRIPTION:
 *      Bus Read Operation to address.
 *
 * CALLS:
 *	none
 *
 * ALGORITHM:
 *	none
 *
--*/
int	sfbp$$bus_config_read (int *address)

{
return(SS$K_SUCCESS);
}


/*+
 * ===================================================
 * = sfbp$$wait_csr_ready - Wait for Status Ready =
 * ===================================================
 *
 * OVERVIEW:
 *  
 *     This routine is used to wait for sfb ready
 *  
 * FORM OF CALL:
 *  
 *     sfbp$$wait_csr_ready (scia_ptr,data_ptr)
 *  
 * ARGUMENTS:
 *	scia_ptr - pointer to the shared console interface area
 *      data_ptr - driver data area pointer
 *
 * FUNCTIONAL DESCRIPTION:
 *      Wait for csr ready. The sfb+ has a write queue. If we issue
 *      lots of write commands and then attempt to read from the
 *      frame buffer, we may run into some problems, so it's a 
 *      good idea to wait for command status bit to clear, especially
 *      for diagnostics, which do lots of reads. I am not sure how
 *      long to wait here, but it really should not take longer
 *      than 1 turbo channel cycle to complete this sync up.
 *
	The command status register is a special address that is decoded
	by the interface logic and returns the current status of the command
	buffer.  If the status bit is set, the command buffer is busy
	completing commands, if the bit is not set, the command buffer is
	empty and all previous commands have completed.

	Writes to the command status register are also special. Although
	functionally a noop, the write will be hardware interlocked and
	will not complete until the chip is idle.  This feature allows
	software to insert barriers into the command stream without
	busy-waiting on a command status read.

	31                                                          1  0
	+------------------------------------------------------------+-+
	|                         ignore                             |S|
	+------------------------------------------------------------+-+

(Software notes)

	The sfb+ maintains the idea that the chip is mostly a write-only
	device.  In particular, you can issue painting commands without
	worrying about overflowing the command buffer, as no command
	effectively takes longer that a bus timeout to execute.

	However, if you want to read data back from the screen, using the copy
	buffers or by reading data directly using simple mode, you have to wait
	for the chip to finish executing all of the command queued up in its
	command buffer.  Just busy-wait on the Command Status Register until it
	becomes 0.
 *
 *
--*/

int	sfbp$$sfbp_wait_csr_ready (SCIA *scia_ptr,SFBP *data_ptr)

{
#define  IDLE_WAIT 100000
volatile register int i,j,s=1;

wbflush ();
for (i=0,s=1;s && i<IDLE_WAIT;i++)
    {
    s=(*gfx_func_ptr->pv_sfbp_read_reg)(scia_ptr,sfbp$k_command_status);
    krn$_micro_delay( 10 );
    }

return(SS$K_SUCCESS);
}
