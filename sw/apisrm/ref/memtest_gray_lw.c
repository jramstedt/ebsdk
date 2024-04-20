/* file:	memtest_gray_lw.c
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
 *	it under the name STUB_MEMTEST_GRAY.C.
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

extern unsigned int poweron_msec [2];

extern struct PCB	*getpcb(void);
extern void 	       	*dyn$_malloc();                  
extern void		krn$_readtime(INT *);
extern int     	     	killpending();                     
extern int     	     	check_kill();
                                                                
extern void  create_iob ();
extern void  remq_iob();             
extern void  load_error_info(int err_num,int data,int read_data,UINT addr);


/* Test 1 - LW version */
#if TURBO
int memtest_gray_lw()
#else
int memtest_gray()
#endif
{                                         
    struct PCB	  	*pcb;  
    struct IOB	      	*iob_ptr;   	
    externals  	      	*eptr;                     
    u_int		i,j,status,temp;
    UINT 		start, stop;    	/* Quadwords */
    UINT		test_addr;
    u_int		data,read_data;                              
    int			loop_len;                                         
                                                                     
    pcb = getpcb();
    eptr=(externals *)pcb->pcb$a_globals;                               

#if DEBUG                                         
    printf("eptr->block_size : %x\n",eptr->block_size);
    printf("eptr->len : %x\n",eptr->len);         
    printf("eptr->incr : %x\n",eptr->incr);         
    printf("eptr->start_addr : %x\n",eptr->start_addr);
#endif                                                

    	if (eptr->fast)	                
    	{                                       
           *(UINT*) &start = *(UINT*) &poweron_msec;
	   graycode_memtest(eptr->start_addr,eptr->block_size,
			    eptr->do_mb,0,0,&temp);
           *(UINT*) &stop = *(UINT*) &poweron_msec;
	   *(UINT*) &stop -= *(UINT*) &start;
       	   if (eptr->timer)	                
           {
       	       if (*(UINT*)&stop == 0)	                
       	           printf("Run time less than 1 millisecond\n");    
	       else
	       {                             
       	           printf("Number of milliseconds: %d\n",*(UINT*) &stop);    
		   printf("MB/second: %d\n", ((*(UINT*)&eptr->block_size*4000)/
					(1024*1024))/(*(UINT*)&stop));
	       }
           }
                                                    
        }                                  
       	else                                         
	{                                       
                               
/* First loop - Flood all memory under test with alternating graycode of the */
/* address followed by the inverse graycode of the address. 	     	     */

/* For Alpha, need to get rid of sign extension for addresses >2 Gig (bit
   31 may be set 
*
*  cto 31-jan-94 - use 64 bit types to avoid this
*/
	    /* Initialize some values */                           
      	    test_addr = eptr->start_addr;
	    /* Note the >>2 is from the # of LWs */
	    loop_len = (UINT) ((UINT) (eptr->block_size>>2)/
			       (UINT) (eptr->incr));
	    loop_len = (loop_len & ~7) - 1;
            if (loop_len == -1)
		return(msg_failure);

#if DEBUG                    
    printf("loop_len : %x\n",loop_len);
#endif                            
            /* read the start time */
            *(UINT*) &start = *(UINT*) &poweron_msec;   

	   j=0;            
    	   do          
 	   {                                 
#if DEBUG
    printf("test_addr : %x\n",test_addr);
#endif
                for (i=0; i<4; i++)
	        {	
 	            *(u_int *)test_addr = (j>>1)^j;            
              	    test_addr += eptr->incr*(sizeof(u_int *)); 
    	            j++; 	                              
    	        }	
                for (i=0; i<4; i++)
	        {	
    	      	    *(u_int *)test_addr = (j>>1)^j^-1;
              	    test_addr += eptr->incr*(sizeof(u_int *)); 
	      	    j++;	                                         
	        }	
 	    } while (j < loop_len);   

#if AVANTI || K2
	krn$_sleep(1000);
#endif

/* add a call to check_kill to se if a ^c was entered */
 	   check_kill();

/* Second loop - Read and verify all memory flooded in first loop. Then      */
/* write alternating inverse graycode of the address followed by the         */
/* graycode of the address. The inverse of each location from first loop.    */

/* Reinitialize no_sign_ext_addr back to beginning of testing */
/* need to get rid of sign extension for addresses >2 Gig (bit
*   31 will be set 
*
*  cto 31-jan-94 - use 64 bit types to avoid this
*/
	    test_addr = eptr->start_addr;             
	    j=0;                                      
	    do	                                      
	    {                             
                for (i=0; i<4; i++)
	        {	
		    data = (j>>1)^j;          
    	            read_data = *(u_int *)test_addr;	
                    if (data != read_data)         
                    {                                                  
       	            /* error discovered  */                    
	     	  	load_error_info(41,data,read_data,test_addr);
      	      	  	if ((status = report_error()) != msg_success)
      	   	            return(status);
              	    }                                 
       	      	    *(u_int *)test_addr = data^-1;                   
              	    test_addr += eptr->incr*(sizeof(u_int *)); 
	            j++;       	           
                }                 
    
                for (i=0; i<4; i++)
	        {	
    
	            data = (j>>1)^j^-1;        
      	            read_data = *(u_int *)test_addr;	                       
              	    if (data != read_data)
              	    {                                                  
	      	    /* error discovered  */           
	  	  	load_error_info(42,data,read_data,test_addr);
      	      	  	if ((status = report_error()) != msg_success)
    		       	    return(status);     
              	    }                         
	      	    *(u_int *)test_addr = data^-1;                       
              	    test_addr += eptr->incr*(sizeof(u_int *)); 
    	      	    j++; 	                              
                }                                     
            } while (j < loop_len);                                    
                              
#if AVANTI || K2
	krn$_sleep(1000);
#endif

/* add a call to check_kill to se if a ^c was entered */
 	   check_kill();

/* Third loop - Read and verify all memory under test with graycode patterns */
/* written in second loop. 						     */
	    test_addr = eptr->start_addr;             
	    j=0;                                
	    do                      
    	    {                                    
                for (i=0; i<4; i++)
	        {	
      	      	    data = (j>>1)^j^-1;       
	      	    read_data = *(u_int *)test_addr;	     
              	    if (data != read_data)                    
       	      	    {       	              
	      	    /* error discovered  */
		  	load_error_info(43,data,read_data,test_addr);
    	 	  	if ((status = report_error()) != msg_success)
    	      	       	    return(status);       
       	      	    }	                             
              	    test_addr += eptr->incr*(sizeof(u_int *)); 
              	    j++;                   
                }         
                for (i=0; i<4; i++)
	        {	
                             
       	      	    data = (j>>1)^j;          
	      	    read_data = *(u_int *)test_addr;	        
              	    if (data != read_data)                    
    	      	    { 	                              
	      	    /* error discovered  */        
    	   	    load_error_info(44,data,read_data,test_addr);
	     	    if ((status = report_error()) != msg_success)
	  	    	return(status);     
	      	    }	                                            
              	    test_addr += eptr->incr*(sizeof(u_int *)); 
	      	    j++;     	                        
	      	}	
            } while (j < loop_len);	           
                 
#if AVANTI || K2
	krn$_sleep(1000);
#endif

           *(UINT*) &stop = *(UINT*) &poweron_msec;
	   *(UINT*) &stop -= *(UINT*) &start;
       	   if (eptr->timer)	                
           {
       	       if (*(UINT*)&stop == 0)	                
       	           printf("Run time less than 1 millisecond\n");    
    	       else
	       {	                    
       	           printf("Number of milliseconds: %d\n",*(UINT*) &stop);    
		   printf("MB/second: %d\n", ((*(UINT*)&eptr->block_size*4000)/
					(1024*1024))/(*(UINT*)&stop));
	       }                
           }
    	}                   
	if ((iob_ptr = (struct IOB *)find_pcb_iob()) != NULL)
	{
	   /* reset the device to "memory" */	
	   strncpy(pcb->pcb$b_dev_name, "memory", MAX_NAME);   
	   strncpy(iob_ptr->name, pcb->pcb$b_dev_name, MAX_NAME);

	   /* add the bytes read and written */	
	   *(UINT*) &iob_ptr->bytes_read += *(UINT *) &eptr->block_size;
	   *(UINT*) &iob_ptr->bytes_written += *(UINT *) &eptr->block_size;
	}
#if DEBUG
    	printf("bytes_read : %x %08x\n", iob_ptr->bytes_read[1],
					 iob_ptr->bytes_read[0]);
       	printf("bytes_written : %x %08x\n",iob_ptr->bytes_written[1],
					   iob_ptr->bytes_written[0]);
#endif
    return msg_success;                                  
                                           
}
