/* file:	memtest_march.c
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
 *      This exerciser module contains a march test.  A marching pattern and 
 *	inverse marching pattern are written, read, and verified for the 
 *	specified address range.  (For more details of each test see the 
 *	overview under the MEMTEST.C description.)
 *
 *	The march test has been stubbed for platforms that do not require 
 *	it under the name STUB_MEMTEST_MARCH.C.
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
#include	"cp$src:prdef.h"
#include	"cp$src:platform.h"                          
#include	"cp$src:kernel_def.h"          
#include	"cp$src:parse_def.h"    
#include	"cp$src:version.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:ctype.h"                                           
#include	"cp$src:stddef.h"
#include	"cp$src:common.h"                 
#include	"cp$inc:prototypes.h"                 
#include	"cp$src:diag_def.h"                
#include	"cp$src:diag_groups.h"
#include	"cp$src:msg_def.h"                               
#include	"cp$src:ev_def.h"
#include	"cp$src:mem_def.h"
#include	"cp$src:memtest_def.h"
#include	"cp$inc:kernel_entry.h"

#if MODULAR
#include "cp$src:modular_redefinition.h"
#endif

extern struct PCB	*getpcb(void);
extern void		*dyn$_malloc();                  
extern void		krn$_readtime(INT *);
extern int		killpending();                     
extern int		check_kill();
                                                                
extern void  create_iob ();
extern void  remq_iob();             
extern void  load_error_info(int err_num,int data,int read_data,UINT addr);

int memtest_march() {
	struct PCB	*pcb;
	struct IOB	*iob_ptr;
	externals	*eptr;
	int		status;
	UINT		j;
	UINT		test_addr;
	UINT		data,read_data;
	UINT		loop_len;
	UINT 		start_time,stop_time;

	volatile	u_int x;

	pcb = getpcb(); 
	eptr=(externals *)pcb->pcb$a_globals;

	krn$_readtime ((INT *)&start_time);      

/* First loop - Flood all memory under test by marching test_pattern down
   consecutive address lines of memory */
	test_addr = eptr->start_addr;
	data = eptr->test_patt;

	loop_len = (eptr->block_size >> 3) / (2 * (eptr->incr + 1));
	j=0;
	while (j < loop_len) {
	    *(UINT *)test_addr = data;
	    test_addr += 8;
	    *(UINT *)test_addr = data;
	    test_addr += (8 * 2 * eptr->incr) + 8;
	    ++j;
	}

/* Add a call to check_kill to see if a ^c was entered */
	check_kill();

/* Second loop - Read and verify all flooded in first loop. Then write
   inverted pattern down consecutive address lines of memory */

/* Reinitialize test_addr back to beginning of testing */
	test_addr = eptr->start_addr;

	j=0;
	while (j < loop_len) {
	    read_data = *(UINT *)test_addr;
	    if ( !(eptr->fast) ) {
		if (read_data != data) {
		    load_error_info(5,(int)eptr->test_patt,(int)read_data,test_addr);

		    /* error discovered - report in 64 bits */
		    if ((status = report_failure_quad(DIAG$K_HARD,5,msg_d_data_comp_err,(UINT)data,
				  (UINT)read_data,(UINT)test_addr)) != msg_success)
		    return(status);
		}
	    }
	    *(UINT *)test_addr = ~(data);
	    test_addr += 8;
	    read_data = *(UINT *)test_addr;
	    if ( !(eptr->fast) ) {
		if (read_data != data) {
		    load_error_info(5,(int)eptr->test_patt,(int)read_data,test_addr);

		    /* error discovered - report in 64 bits */
		    if ((status = report_failure_quad(DIAG$K_HARD,5,msg_d_data_comp_err,(UINT)data,
				  (UINT)read_data,(UINT)test_addr)) != msg_success)
		    return(status);
		}
	    }
	    *(UINT *)test_addr = ~(data);
	    test_addr += (8 * 2 * eptr->incr) + 8;
	    ++j;
	}

/* Add a call to check_kill to see if a ^C was entered */
	check_kill();

/* Third loop - Read and verify all memory under test with inverted pattern 
   written in second loop */

	/* Keep test_addr at end of memory under test to read the pattern
	   from the bottom of memory up and write 0's */
	test_addr -= (8 * 2 * eptr->incr) + 8;

	j=0;
	while (j < loop_len) {
	    read_data = *(UINT *)test_addr;
	    if ( !(eptr->fast) ) {
		if (read_data != (UINT)~(data)) {    
		    load_error_info(6,(int)~(data),(int)read_data,test_addr);

		    /* error discovered */
		    if ((status = report_failure_quad(DIAG$K_HARD,6,msg_d_data_comp_err,(UINT)~(data),
			          (UINT)read_data,(UINT)test_addr)) != msg_success)
			return(status);
		}
	    }
	    *(UINT *)test_addr = 0;
	    test_addr -= 8;
	    read_data = *(UINT *)test_addr;
	    if ( !(eptr->fast) ) {
		if (read_data != (UINT)~(data)) {    
		    load_error_info(6,(int)~(data),(int)read_data,test_addr);

		    /* error discovered */
		    if ((status = report_failure_quad(DIAG$K_HARD,6,msg_d_data_comp_err,(UINT)~(data),
			          (UINT)read_data,(UINT)test_addr)) != msg_success)
			return(status);
		}
	    }
	    *(UINT *)test_addr = 0;
	    test_addr -= (8 * 2 * eptr->incr) + 8;
	    ++j;
	}

	krn$_readtime((INT *)&stop_time);
	stop_time -= start_time;
	if (eptr->timer)
	    printf("Number of clock ticks: %d\n",stop_time);
	
	if ((iob_ptr = (struct IOB *)find_pcb_iob()) != NULL) {
	   /* add the bytes read and written */	
	    *(UINT*) &iob_ptr->bytes_read += *(UINT *) &eptr->block_size;
	    *(UINT*) &iob_ptr->bytes_written += *(UINT *) &eptr->block_size;
	}

	return msg_success;
}
