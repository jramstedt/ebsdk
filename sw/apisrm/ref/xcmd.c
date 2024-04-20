/* file:	xcmd.c
 *
 * Copyright (C) 1990 by
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
 *	X command for downloading
 *
 *  AUTHORS:
 *
 *      D. Marsh
 *
 *  CREATION DATE:
 *  
 *      
 *
 *  MODIFICATION HISTORY:
 *
 *  dpm		26-apr-1994	Changed from a spinlock to disabling the serial
 *				port interrupts directly. Changed from using 
 *				printf command to send the prompt to writing 
 *				it directly.
 *
 *
 *--
 */

#include        "cp$src:platform.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:impure_def.h"
#include	"cp$src:stddef.h"
#include	"cp$src:common.h"
#include "cp$inc:prototypes.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:tt_def.h"
#include	"cp$src:parse_def.h"
#include 	"cp$inc:platform_io.h"
#include 	"cp$src:dynamic_def.h"
#include "cp$src:eisa.h"
#include "cp$src:parser.h"
#include "cp$src:pal_def.h"                  


extern  struct ZONE *memzone;
extern  struct LOCK spl_kernel;
extern  int combott_rxready ();
extern  int combott_rxread ();
extern  int combott_txread ();
extern  int combott_rxoff (); 
extern  int combott_rxon ();
extern  int mtpr_ipl();

unsigned int x_getbyte(char *);

/*+
 * ============================================================================
 * = xcmd - Load a file via serial port 1.                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Downloads a file via serial port 1. It is invoked by the xload 
 *      program on the host.  Use the boot pmem:xxx command to execute the 
 *	code. X command will not work if xdelta is loaded. 
 *  
 *   COMMAND FMT: xcmd 0 0 0 xcmd
 *
 *   COMMAND FORM:
 *
 *	xcmd  (load_address  load_size)
 *  
 *   COMMAND TAG: xcmd 0 RXB xcmd
 *
 *   COMMAND ARGUMENT(S):
 *
 *
 *   COMMAND OPTION(S):
 *
 *
 *   ENVIRONMENT VARIABLE(S):
 *
 *
 *   COMMAND EXAMPLE(S): x 800000 3d5
 *
 *
 *   COMMAND REFERENCES:
 *
 *
 * FORM OF CALL:
 *  
 *  
 * RETURN CODES:
 *
 *       
 * ARGUMENTS:
 *
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void  xcmd (int argc, char *argv[]) 
{

unsigned int  cpy_size,i;
unsigned int timeout, rcvd_chars;
unsigned char chk_sum=0,sent_chksum,next_char;
unsigned char *tmp_cpy, *cpy_addr;                                       
int	ipl;
char ok_string[] = ">>>\n";
char bad_string[] = "?2A\n";

/* argc, argv contain the command and arguments, get the checksum from the */
/* serial port 1. Since its not ascii text, no control code are valid      */

      combott_rxoff (0); 
         timeout=x_getbyte(&sent_chksum);

        cpy_addr= xtoi(argv[1]);
        cpy_size= xtoi(argv[2]);

/* calculate the checksum from argv and argc */
	while (--argc >= 0)
         {
          i=0;
          tmp_cpy=argv[argc];
          while ((next_char=*tmp_cpy++) != '\0')
           chk_sum+=next_char;
         }

/* add 80 hex to the command checksum as the xload program adds 4 spaces */
/* that the console code strips out, we have no way to get them back.    */
 chk_sum=chk_sum+0x80; 
 chk_sum=~chk_sum+1;


 if ( chk_sum != sent_chksum)
  {
   for (i=0;i<4;i++)
    {
     while (combott_txready(0)==0);
       combott_txsend (0, bad_string[i]);
    }

    combott_rxon (0); 
/* command checksum error note it and return */
    err_printf("command checksum=%x sent checksum=%x \n",chk_sum,sent_chksum);
  } 
 else
  {
/* since our ipl is high, send out the ok string char by char */
   for (i=0;i<4;i++)
    {
     while (combott_txready(0)==0);
       combott_txsend (0, ok_string[i]);
    }
/* put the transfered file into a buffer just in case the transfer fails    */
/* then we won't have trashed any memory, allocate 1 extra byte for checksum*/
    chk_sum=0;
    tmp_cpy=dyn$_malloc(cpy_size+1,DYN$K_SYNC|DYN$K_ZONE,-1, 0, memzone);
    timeout=1;
    rcvd_chars=0;
/* get the next cpy_size bytes from serial port 1, treat them as data */
/* not text. Put them in out temp buffer and calculate the checksum   */

    while((rcvd_chars<=cpy_size) && (timeout != 0))
      {
         timeout=x_getbyte(&tmp_cpy[rcvd_chars]);
         if(rcvd_chars!=cpy_size)
          chk_sum+=tmp_cpy[rcvd_chars];
         rcvd_chars++;         
      }

      combott_rxon (0); 


/* the last byte recieved is the checksum get ity and check it */
    rcvd_chars--;
    sent_chksum=tmp_cpy[rcvd_chars];
    chk_sum=~chk_sum+1;
    if ((sent_chksum != chk_sum)||(timeout==0))
     {
       pprintf("?2A >>>\n");
       err_printf("dchecksum=%x sent dchecksum=%x \n",chk_sum,sent_chksum);
       err_printf("Timo chars xfered= %x expected= %x\n",rcvd_chars,cpy_size);
     } 
    else   /* all went well copy it to the user defined dest. */
     {
	for (i=0;i<=cpy_size;i++)
          cpy_addr[i]=tmp_cpy[i];
     }

    dyn$_free( tmp_cpy,DYN$K_SYNC|DYN$K_ZONE,memzone);
  }


}

unsigned int x_getbyte(char *rcvd)
{
unsigned int tt_timeout=0x00ffffff; 

       while ((combott_rxready(0)==0)&&(--tt_timeout !=0));

       if (tt_timeout !=0)
         {
         *rcvd=combott_rxread(0);
         }

       return(tt_timeout);
}
