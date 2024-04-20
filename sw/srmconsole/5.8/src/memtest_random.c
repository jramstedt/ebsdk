/* file:	memtest_random.c
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
 *      This exerciser module contains a random test.  This test will 
 *	test random addresses within the specified range with random data of 
 *	random length.  (For more details of each test see the 
 *	overview under the MEMTEST.C description.)
 *
 *	The random test has been stubbed for platforms that do not require 
 *	it under the name STUB_MEMTEST_RANDOM.C.
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
#include "cp$inc:kernel_entry.h"

extern struct PCB	*getpcb(void);
extern void 	       	*dyn$_malloc();                  
extern void		krn$_readtime(INT *);
extern int     	     	killpending();                     
extern int     	     	check_kill();
                                                                
extern void  create_iob ();
extern void  remq_iob();             
extern void  load_error_info(int err_num,int data,int read_data,UINT addr);

extern int log2(int);

int memtest_random()
{

	struct PCB	*pcb;
	struct IOB	*iob_ptr;
	externals	*eptr;
	unsigned int	i=0,j=0,status;
	UINT		start_time,stop_time;
                     
	UINT 		no_sign_ext_addr;
	UINT		addr_rand_index;
	UINT 		rand_addr;
	UINT   		start_data, end_data, data_range;
	UINT		addr_seed, data_seed, data_rand_index;
	unsigned int	restrict_modulo=0xffffffff;
	unsigned int	quad_data_seed;
	unsigned int	read_data;

   	pcb = getpcb(); 
   	eptr=(externals *)pcb->pcb$a_globals;

#if DEBUG                                 
    printf("eptr->block_size : %x\n",eptr->block_size);         
    printf("eptr->start_addr : %x\n",eptr->start_addr);
#endif                                               


    	if (eptr->fast)	                
    	{                         
	   return msg_success;
        }                                  

	else
	{
	    krn$_readtime ((INT *)&start_time);      

	    /* Initialize seed and index to 0 */
	    addr_seed = addr_rand_index = 0;
/*    
*	cto - this didn't work in the new random scheme,
*	so replace with what EXER does currently
*
* The length the user has inputted is in bytes. Number of longwords
*	       to test will be length/4.  Find the index into the LCG table 
*	       that will give a range of numbers up to the number of longwords
* 	       to test. 
*	    while ( (
* lcg_coefficients[addr_rand_index].m < (eptr->block_size>>2)) && 
*	            (lcg_coefficients[addr_rand_index].a) )
*	        addr_rand_index++;
*/
    	    addr_rand_index = log2(eptr->block_size>>2);

	    /* Find power of 2 that greater than or equal to len to use as
	       loop counter for random calls */
	    for (j = 1; j <= 0x80000000; j=j<<1)
	      	if (j >= (eptr->block_size>>2)) break;

	    /* Allow any data pattern from 0 to 0xffffffff */
	    data_range = 0xffffffff; 

	    /* Select random data index (1-32) from user entered seed */
	    data_rand_index = eptr->rand_seed % 32;
	    data_seed = eptr->rand_seed;
/*
*	cto - this didn't work in the new random scheme,
*	so replace with what EXER does currently
*
*
* Find the index into the LCG table that will give a range of 
*	       data patterns up to the allowed data_range 
*	    while ( (lcg_coefficients[data_rand_index].m < 
*  data_range) && (lcg_coefficients[data_rand_index].a) )
*	        data_rand_index++;
*/	
    	    data_rand_index = log2(data_range);

	    for (i = 0; i < j; ++i)
	    { 

		    /* Get random pattern for data -- Need to limit data seed
		       so don't get overflow in arithmetic for calculating
 		       random number.  Random number is calculated by 
		       (data_seed)*lcg_coefficients[i].a + lcg_coefficients[i].c) % lcg_coefficients[i].m.  Need
  		       to make sure (data_seed)*lcg_coefficients[i].a + lcg_coefficients[i].c does not
		       exceed (2**32)-1.  The largest lcg_coefficients[i].a is 65557 and the 
		       largest lcg_coefficients[i].c is 907526589.  Therefore:
		       ((data_seed)*65557) + 907526589 < (2**32)-1.
		       By limitting (modulo'ing) the addr_seed, you will not 
		       be guaranteed to hit every address */
/* This modulo function will not be required once a 64 bit compiler is
   available -- note for VAX, a different random algorithm (using h floating
   point arithmetic) will need to be employed to prevent overflow arithemtic
   errors */
	        if ((addr_seed = random(addr_seed % restrict_modulo, 
		     addr_rand_index)) < (eptr->block_size>>2)) 
		{

	            /* Make address at least longword aligned */
	            rand_addr = eptr->start_addr + (addr_seed<<2);
	
		    no_sign_ext_addr = rand_addr;

		    /* Get random pattern for data -- Need to limit data seed
		       so don't get overflow in arithmetic for calculating
 		       random number.  Random number is calculated by 
		       (data_seed)*lcg_coefficients[i].a + lcg_coefficients[i].c) % lcg_coefficients[i].m.  Need
  		       to make sure (data_seed)*lcg_coefficients[i].a + lcg_coefficients[i].c does not
		       exceed 2**31.  The largest lcg_coefficients[i].a is 65557 and the 
		       largest lcg_coefficients[i].c is 907526589.  Therefore:
		       ((data_seed)*65557) + 907526589 < 2**31. */
	            data_seed = random(data_seed, data_rand_index);

		    /* Randomly choose to do quadword or longword stores */
	  	    if (data_seed & 1) {

			/* Perform a write, memory barrier, and read of data */
		        read_data = stl_ldl(no_sign_ext_addr,data_seed);
	
			/* Make sure data written and returned are the same */
		        if (read_data != (unsigned int)data_seed)
	                {    

		            /* error discovered */
		           load_error_info(7,data_seed,read_data,
				no_sign_ext_addr);

       		            if ((status = report_error()) != msg_success)
			        return(status);
		        }

		    }

		    else {

	                /* Make address quadword aligned */
	                rand_addr &= ~7;
		        no_sign_ext_addr = rand_addr;

		        quad_data_seed = data_seed | ((~data_seed) << 32);
                    
		        read_data = stq_ldq(no_sign_ext_addr,quad_data_seed);
                        
		        if (read_data != quad_data_seed)
		        {    

		            /* error discovered */
		            load_error_info(10,quad_data_seed,read_data,
				no_sign_ext_addr);

       		            if ((status = report_error()) != msg_success)
		                return(status);
		        }
                    
	            }
                
	        }

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
