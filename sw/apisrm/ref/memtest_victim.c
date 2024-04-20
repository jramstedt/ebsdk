/* file:	memtest_victim.c
 *     
 * Copyright (C) 1994 by
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
 *      Alpha Firmware
 *              
 *  MODULE DESCRIPTION:
 *
 *      This exerciser contains a victim block test.  The test will perform 
 *	block writes of data, victimize the data, and then read back the 
 *	block and verify it.  This will be performed for the specified 
 *	address.  (For more details of each test see the overview under the 
 *	MEMTEST.C description.)
 *
 *	The victim test has been stubbed for platforms that do not require 
 *	it under the name STUB_MEMTEST_VICTIM.C.
 *
 *  AUTHORS:
 *
 *      Harold Buckingham,Traci Post
 *                     
 *  CREATION DATE:
 *                                                          
 *      24-jan-1994
 *
 *  MODIFICATION HISTORY:          
 *
 *	cto	24-Jan-1994	Break out the Laser memtests into separate 
 *				files.
 *
 *--                   
 */                                       
#include 	"cp$src:prdef.h"
#include	"cp$src:platform.h"                          
#include	"cp$src:kernel_def.h"          
#include	"cp$src:parse_def.h"    
#include	"cp$src:version.h"
#include	"cp$src:dynamic_def.h"
#include 	"cp$src:ctype.h"                                           
#include	"cp$src:stddef.h"
#include	"cp$src:common.h"                 
#include "cp$inc:prototypes.h"
#include 	"cp$src:diag_def.h"                
#include      	"cp$src:diag_groups.h"
#include 	"cp$src:msg_def.h"                               
#include 	"cp$src:ev_def.h"
#include	"cp$src:mem_def.h"
#include	"cp$src:memtest_def.h"
#include	"cp$inc:kernel_entry.h"

#if MODULAR
#include "cp$src:modular_redefinition.h"
#endif

extern struct PCB	*getpcb(void);
extern void 	       	*dyn$_malloc();                  
extern void		krn$_readtime(INT *);
extern int     	     	killpending();                     
extern int     	     	check_kill();
                                                                
extern void  create_iob ();
extern void  remq_iob();             
extern void  load_error_info(int err_num,int data,int read_data,UINT addr);


int memtest_victim()
{
	struct PCB	*pcb;
	struct IOB	*iob_ptr;
	externals	*eptr;
	unsigned int	i=0,status;
    	UINT		test_addr,saved_addr;
	UINT		data,read_data;
	UINT		start_time,stop_time;
	UINT		loop_len,j;
	UINT		pattp;
                     
   	pcb = getpcb(); 
   	eptr=(externals *)pcb->pcb$a_globals;

	/* Fast option not supported for this test */
    	if (eptr->fast)	                
	     return msg_success;

	else
	{
	    krn$_readtime ((INT *)&start_time);      

/* Outer loop to do for entire length of entered memory */
	    test_addr = eptr->start_addr;
	    loop_len = eptr->block_size;  

	    j = 0;
	    while (j < loop_len)
	    {

/* add a call to check_kill to se if a ^c was entered everytime a new chunk
   is tested */
 	        check_kill();

		saved_addr = test_addr;

/* Inner loop - Write a BCACHE_BLOCK_LW's worth of data at starting address, 
   victimize, and read back to verify */

		/* If the test address + a block is going to exceed max address
		   break out of loop and don't test */
		if ((test_addr + BCACHE_BLOCK_LW*4) > 
		      (eptr->start_addr + eptr->block_size)) 
		    break;

		/* Write a block's worth of quadword data from address */
		pattp = (UINT)eptr->patt_block;
		for (i=0; i<BCACHE_BLOCK_LW; ++i, ++i)
		{

	            *(UINT *)test_addr = *(UINT *)pattp;
	            test_addr += 8;
                    pattp += 8;
		}

/* need to get rid of sign extension for addresses >2 Gig (bit 31 will be set)
*
*	cto - use 64 bit types to avoid this
*/
		/* Reset test address back to original */
		test_addr = saved_addr;

		/* Vicitimize the test address by writing the 4MEG offset */
		test_addr += get_bcache_size();
		*(UINT *)test_addr = 0xabcdef;

/* need to get rid of sign extension for addresses >2 Gig (bit 31 will be set)
*
*	cto - use 64 bit types to avoid this
*/
		/* Reset test address back to original */
		test_addr = saved_addr;

		/* Now read the original data back (will victim again) */
		pattp = (UINT)eptr->patt_block;
		for (i=0; i<BCACHE_BLOCK_LW; ++i, ++i)
		{
	            read_data = *(UINT *)test_addr;

		    /* Make sure expected = received */
		    if (read_data != *(UINT *)pattp)
		    {
		        /* error discovered */
		        load_error_info(13,*(UINT *)pattp,
				read_data,test_addr);
			
		        /* error discovered - report in 64 bits */
		        if ((status = report_failure_quad(DIAG$K_HARD,13,
				msg_d_data_comp_err,*(UINT *)pattp,
				(UINT)read_data,(UINT)test_addr)) != 
				msg_success)
			return(status);
		    }
	            test_addr += 8;
	            pattp += 8;
		}

	   	j+=BCACHE_BLOCK_LW/2; /* QWs!! */
	    }
       		
            krn$_readtime((INT *)&stop_time);
	    stop_time -= start_time;
	    if (eptr->timer)
		printf("Number of clock ticks: %d\n",stop_time);
	}
	
	if ((iob_ptr = (struct IOB *)find_pcb_iob()) != NULL)
	{
	   /* add the bytes read and written */	
	   *(UINT*) &iob_ptr->bytes_read += *(UINT *) &eptr->block_size;
	   *(UINT*) &iob_ptr->bytes_written += *(UINT *) &eptr->block_size;
	}

	return msg_success;
}
