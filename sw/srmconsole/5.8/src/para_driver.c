/* file:	para_driver.c
 *
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
 *      Jensen Exerciser
 *
 *  MODULE DESCRIPTION:
 *
 *	A driver that supports the parallel port on the VLSI Technologies 
 *      82C106 PC/AT integerated combination I/O chip.  
 *
 *  AUTHORS:
 *
 *	Judy Gold
 *
 *  CREATION DATE:
 *  
 *	20-Oct-1992	Initial version
 *			
 *
 *  MODIFICATION HISTORY:
 *
 *	jeg	17-Aug-1994	poll for ACK before BUSY, shorten delay in 
 *				character strobing, conditionalize io for Jensen
 *
 *	dtr	27-Jan-1994	Changed the calls to inport/outport to eisa_x
 *
 *      dpm	21-Jun-1993     Added checks for the parallel loopback
 *				if present,bypass the busy checks and the
 *				error checks.
 *
 *	pel	25-Feb-1993	replace xbus_in/outport w/ in/outport so
 * 				they'll work w/ either xbus or eisa.
 *
 * 	23-Feb-1993	jeg	Initialize printer device in open rather than 
 *				in init.  This reduces console powerup time.
 *
 *--
 */

#include 	"cp$src:platform.h"
#include	"cp$src:common.h"
#include	"cp$src:kernel_def.h"
#include 	"cp$src:platform_cpu.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:stddef.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:combo_def.h"
#include	"cp$src:isacfg.h"
#include 	"cp$inc:platform_io.h"
#include 	"cp$inc:prototypes.h"

#if WILDFIRE || WFSTDIO
extern struct pb stdio_pb;
#define para_pb &stdio_pb
#else
#define para_pb 0
#endif

#define para_outportb(c,v) outportb(para_pb,c,v)
#define para_inportb(c)    inportb(para_pb,c)

#ifdef LPT_CSR_BASE
#define LPTD (LPT_CSR_BASE+0)
#define LPTS (LPT_CSR_BASE+1)
#define LPTC (LPT_CSR_BASE+2)
#endif

extern	null_procedure ();

extern 	int spl_kernel;

#if WFSTDIO
int para_isr () { pprintf ("LPT interrupt\n"); io_issue_eoi (para_pb, LPT_VECTOR); }
#endif

int para_init ();
int para_open (struct FILE *fp, char *info, char *next, char *mode);
int para_close (struct FILE *fp);
int para_write (struct FILE *fp, int size, int number, unsigned char *buf);
int para_read (struct FILE *fp, int size, int number, unsigned char *buf);

struct DDB para_ddb = {
	"para",			/* how this routine wants to be called	*/
	para_read,		/* read routine				*/
	para_write,		/* write routine			*/
	para_open,		/* open routine				*/
	para_close,		/* close routine			*/
	null_procedure,		/* name expansion routine		*/
	null_procedure,		/* delete routine			*/
	null_procedure,		/* create routine			*/
	null_procedure,		/* setmode				*/
	null_procedure,		/* validation routine			*/
	0,			/* class specific use			*/
	1,			/* allows information			*/
	0,			/* must be stacked			*/
	0,			/* is a flash update driver		*/
	0,			/* is a block device			*/
	1,			/* not seekable				*/
	0,			/* is an ethernet device		*/
	0,			/* is a filesystem driver		*/
};
int para_lpbk;    /* flag indicating if a parallel port loopback is conn */

/*
 * Initialize the driver and the inode for the Parallel Port
 */
int para_init () {
	struct INODE 	*ip;
	int status;
        int found;
	
/* allocate an inode so that the parallel port appears as a read/write inode */

     found = config_get_device("LPT1");
     if (!found)
     {
	status = msg_success;
     }
     else 
     {
	allocinode (para_ddb.name, 1, &ip);
	ip->dva = & para_ddb;
	ip->attr = ATTR$M_READ | ATTR$M_WRITE;
	ip->misc = 0;		/* flag printer is not yet initialized */

	INODE_UNLOCK (ip);

#if WFSTDIO
        int_vector_set (LPT_VECTOR, para_isr, para_pb);
 	io_enable_interrupts (para_pb, LPT_VECTOR); 
#endif

	status = msg_success;
      }

return(status);
}

/*++
 *
 *  Support has been added for a parallel port loopback connector. This allows
 *  for the exer to be run in a read/write mode on the port. A global flag 
 *  has been added to indicate the presence of a loopback connector. The 
 *  loopback connector has the following configuration:
 *
 *    1  --->  13      Strobe---->Select
 *    2  --->  15      DB0------->Error
 *    10 <---  16      Ack<-------Init
 *    11 <---  17      Busy<------Selectin
 *    12 <---  14      Paperend<--Autofeed
 *
 *    From the registers the connection is:
 *
 *     DB0(Data Reg, Bit 0)----->  ERR (STR bit3)
 *
 *      CTR Bit             STR Bit
 *
 *      STB  0  ---invert-->SLCT 4
 *      AFD  1  ---invert-->PE   5
 *      INIT 2  ----------->ACK  6
 *      SLIN 3  ----------->BUSY 7
 *
 *  The driver checks for a loopback by checking if the ERR bit follows
 *  data bit 0, and sets the flag. This check is done in the open routine
 *  and is reset back in the close routine. If a loopback is detected the 
 *  driver no longer checks for busy, or any error conditions. Note that 
 *  if no loopback is connected the read portion of the driver is invalid. 
 *  
 *--
 */
 

void chk_para_lpbk()
 {
	 int i;
	 volatile unsigned char chr;

	 para_lpbk=0;

	 para_outportb((uint64)LPTD,01);		/* write a 1 to DB0 */
	 krn$_sleep(1);
	 chr=para_inportb((uint64)LPTS) & LPTS$M_ERROR;	/* read status err bit*/
         if (chr == 0)                  	/* 0 = no lpbk */
            return;

         para_outportb((uint64)LPTD,0);              /* write a 0 to DB0 */
	 krn$_sleep(1);
	 chr=para_inportb((uint64)LPTS) & LPTS$M_ERROR; /* read status err bit */
         if (chr != 0)                          /* 1 = no lpbk */
            return;

	/* check the control lines */
	for (i=0; i<16; i++)
	    {
	    para_outportb((uint64)LPTC,LPTC$M_IRQ_EN+i);
	    krn$_sleep(1);
	    chr = para_inportb((uint64)LPTS);

	    if ((chr & 0xf0) != ((i<<4) ^ 0x30))
		{
		err_printf("Parallel port loopback test failure\n");
		err_printf("LPTS Expected %02x Received %02x\n", (i<<4) ^ 0x30, chr & 0xf0);
		}	
	    }

         para_outportb((uint64)LPTD,01);             /* write a 1 to DB0 */
	 krn$_sleep(1);
	 chr=para_inportb((uint64)LPTS) & LPTS$M_ERROR; /* read status err bit */
         if (chr == 0)                          /* 0 = no lpbk */
            return;

         para_lpbk=1;                           /* err bit follows DB0 */
			   			/* if loopback connected */
 }	


int para_raw_init()
 {
	int para_status();
        if (para_lpbk == 1)
	        return msg_success;
	para_outportb((uint64)LPTC,0);		/* initialize printer */
	krn$_sleep(1);
/* Deassert init, put printer online, enable line feed at end of line.     */
/* Also enable interrupts so that IRQ LPTS bit will assert with ACK.       */
/* we are still running polled, interrupt is not enabled in IRQ controller */
	para_outportb((uint64)LPTC,LPTC$M_INIT | 
		LPTC$M_AUTO_FD | LPTC$M_SLCT_IN | LPTC$M_IRQ_EN);  
	krn$_sleep(3000);	       /* wait for printer to go online */
	if(para_status())
		return msg_failure;
	
 }

int para_open (struct FILE *fp, char *info, char *next, char *mode)
 {
	struct INODE *inode;
        int para_raw_init();
	int chk_para_lpbk();

        inode = fp->ip;
/* check for loopback, if it will set flag */
	chk_para_lpbk();
	if(!inode->misc)
		if(para_raw_init())
			return msg_failure;
	inode->misc = 1;
       	return msg_success;
 }


int para_close (struct FILE *fp)
 {
	struct INODE *ip;
	para_lpbk=0;             		/* set loopback to none */
	return msg_success;
 }

int para_write (struct FILE *fp, int size, int number, unsigned char *buf)
 {
	int len;
	int char_sent;
	char chr;
	int status;
	int para_send(unsigned char ch);
	int chk_para_lpbk();

/* check for loopback if the flag is set. This checks that the lpbk has */
/* not been removed, or if something breaks.				*/
	if (para_lpbk==1) 
	 {
	   chk_para_lpbk();
	    if (para_lpbk!=1)      
              return 0;
	 }

	char_sent=0;
	len = size * number;
	while(len--)
	 {
		chr = *buf++;		  /* write this character */
		status = para_send(chr);  /* onto the printer */	
		char_sent++;
         }
/* if no error, return len. Otherwise, return 0 and put error in fp->status */
	if(!status)
		return char_sent;	

        else {
		fp->status = msg_failure;
		return 0;
	}
 }


int para_send(unsigned char ch)
 {
	int retries = 10;

	/* wait for non-busy state */

	if (!para_lpbk)
	 {
	  	/* first, look for ACK */
          	while(!(para_inportb((uint64)LPTS) & LPTS$M_ACK) && --retries)
           	 {
			krn$_sleep(1);
	   	 }
          	if(!retries)
			return msg_failure;

		retries = 10;			/* reset retries */
	  	/* now, look for busy */
          	while(!(para_inportb((uint64)LPTS) & LPTS$M_BUSY) && --retries) 
           	 {
			krn$_sleep(1);
	   	 }
          	if(!retries)
			return msg_failure;
    	 }
	/* transmit the character */

	krn$_micro_delay(1);
	para_outportb((uint64)LPTD,ch);	/* character into data buffer */
	krn$_micro_delay(1);

	/* latch the character into the printer */
	para_outportb((uint64)LPTC,LPTC$M_INIT | 
		LPTC$M_STROBE | LPTC$M_AUTO_FD | LPTC$M_IRQ_EN);
	krn$_micro_delay(1);
	para_outportb((uint64)LPTC,LPTC$M_INIT | LPTC$M_AUTO_FD 
		| LPTC$M_IRQ_EN);
	krn$_micro_delay(1);
	if(para_status())       /* check for errors */
		return 0;   
	else
		return msg_success;

 }
 


int para_read (struct FILE *fp, int size, int number, unsigned char *buf)
 {
	int len;
	int char_read;
	unsigned char chr;
	int status;
	int para_rcvd(unsigned char *ch);
	
	len = size * number;
	char_read=0;
	while(len--)
	 {
		status = para_rcvd(&chr);  /* onto the printer */	
 		*buf++= chr;		  /* write this character */
		char_read++;
         }
/* if no error, return len. Otherwise, return 0 and put error in fp->status */
	if(!status)
		return char_read;	

        else {
		fp->status = msg_failure;
		return 0;
	}
 }


int para_rcvd(unsigned char *ch)
 {
	int retries = 10;

	/* wait for non-busy state */

	if (!para_lpbk)
	while(!(para_inportb((uint64)LPTS) & LPTS$M_BUSY) && retries--)
         {
		krn$_sleep(5);
	 }

        if(!retries)
		return msg_failure;

	/* read  the character set the direction to 1 to read it from the */
	/* data pins							  */

		krn$_sleep(1);		
		*ch = para_inportb((uint64)LPTD);
		krn$_sleep(1);
 		if(para_status())       /* check for errors */
			return msg_failure;
		else
			return msg_success;

 }
 


int para_status()
 {
		unsigned char temp;

		if (para_lpbk==1)
		  return msg_success;

		temp = para_inportb((uint64)LPTS) & 0xff;
		if(temp == 0x7f)                   /* kludge, no printer */
			return msg_failure;        /* connected          */
		if(!(temp & LPTS$M_ERROR))
		 {
			err_printf("Printer error \n");
		 	return msg_failure;
		 }
		if(!(temp & LPTS$M_SLCT))
		 {
			err_printf("Printer offline \n");
		 	return msg_failure;
		 }
		if(temp & LPTS$M_PE)
		 {
			err_printf("Printer paper empty \n");
		 	return msg_failure;
		 }

		return msg_success;
 }


