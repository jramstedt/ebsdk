/* file:    iic_driver.c
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
 *      Cobra Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *  IIC bus driver for EVAX/Cobra firmware.
 *
 *  AUTHORS:
 *
 *  David Baird
 *
 *  CREATION DATE:
 *  
 *  2-Jan-1991
 *
 *
 *  MODIFICATION HISTORY:
 * 
 *	jp	24-mar-1994	Add support for mikasa. Mikasa ocp is same
 *				as sable, however our rom space is not on
 *				iic bus. Also, no interrupt line available
 *				so we will run polled mode (like pele)
 *
 *	mc	02-Apr-1993	Initialize ilist pointer to first entry in
 *				iic_node_list before creation of iic master
 *				inode.
 *
 *	db	11-Sep-1992	added check for driver enabled in the open
 *					routine
 *
 *	db	27-Jul-1992	Fixed iic bus command timeouts 
 *
 *	db	25-Jun-1992	Add ISR timeout support
 *
 *	db	10-Jun-1992	Add PSC command timeout support
 *
 *	db	9-Jun-1992	correct cpu naming convention cpu1/2 to cpu0/1
 *
 *	db	3-Jun-1992	Try again - see below
 *
 *	db	2-Jun-1992	Fix a driver race condition
 *
 *	db	29-May-1992	changed printf's to msg numbers
 *
 *	db	04-May-1992	Added iic_vterm (0x44) to driver list
 *
 *	db	27-Apr-1992	Updated to support the PSC command
 *
 *	ajb	17-Jan-1992	Change calling sequence to common_convert
 *
 *	db	20-Dec-1991 Updated to add an Interrupt Driver for Cobra
 *
 *	db	06-Dec-1991	Updated to support the Cobra I/O module based IIC Bus 
 *					interface.
 *
 *	db	24-Oct-1991	Updated to enhance the Serial ROM based
 *					Mini-Console
 *
 *  db  05-Aug-1991 	Add support for the SROM-based 
 *          			Mini-console.
 *
 *  db  17_Jun-1991 	Add support for Lost Arbitration and Slave
 *              		operation
 *
 *  ajb 13-jun-1991 	change getpcb call
 *
 *  db 	19-Apr-1991 	Fix a compile time error
 *
 *  db 	17-Apr-1991 	verify that a device exists before creating
 *              		an inode for it. 
 *
 *  db 	16-Apr-1991 	add diagnostic code for the init routine
 *
 *  db 	28-Mar-1991 	add support for fseek, and clean up the error 
 *              		reporting.
 *
 *  pel 12-Mar-1991 	let fopen/close incr/decr inode ref & collapse
 *              		wrt_ref, rd_ref into just ref.
 *
 *  dwb 2-Jan-1991  	Initial Code 
 *
 *--
 */

/*Include files*/

#include "cp$src:platform.h"
#include    "cp$src:common.h"
#include    "cp$src:prdef.h"
#include    "cp$src:kernel_def.h"
#include    "cp$src:dynamic_def.h"
#include    "cp$src:ansi_def.h"
#include    "cp$src:stddef.h"
#include    "cp$src:iic_def.h"
#include    "cp$src:srom_def.h"
#include    "cp$src:msg_def.h"
#include    "cp$inc:platform_io.h"
#include    "cp$inc:kernel_entry.h"
#include "cp$inc:prototypes.h"
#include "cp$src:iic_devices.h"

/*External references*/
extern int spl_kernel;          /* Kernel spinlock  */  
extern struct QUEUE pollq;      /* Driver Polling Queue */
extern int null_procedure ();       /* Null procedure   */
extern int iic_write_csr();     /* Write to IIC bus CSR Routine */
extern char iic_read_csr();     /* Read to IIC bus CSR Routine */
int float_csr (struct FILE *fp, int size, int width, U_INT_32 mask,
	       int read_offset, int write_offset);
#if RAWHIDE || PC264
extern int diagnostic_mode_flag; /* Diagnostic mode flag */
int iic_poll_pid = 0;
#endif                                                         
                                                               
#define POLLED (MIKASA || ALCOR || RAWHIDE || PC264 || K2 || TAKARA)

#if SWORDFISH
#undef IIC_VECTOR
#define IIC_VECTOR (PCI_BASE_VECTOR+17)
#endif

/*
 *  Define the Available IIC bus nodes with addresses. 
 */

#define IIC_MSTR_TYPE   0
#define IIC_LED_TYPE    1
#define IIC_EEROM_TYPE  2
#define IIC_UP_TYPE		3

/*Routine definitions*/
int iic_close (struct FILE *fp);
int iic_delete (struct INODE *ip);
int iic_init ();
int iic_open (struct FILE *fp, char *file_name, char *next, char *mode);
int iic_read (struct FILE *fp,int size,int number,char *buf);
int iic_setmode ();
int iic_write (struct FILE *fp,int size,int number,char *buf);
void iic_poll_rt(struct IICPB *pb);
void iic_service( struct IICPB *pb);
int iic_reset(struct FILE *fp);        /* IIC bus Reset Routine */
int iic_busb(int er_rpt,struct FILE *fp);   /* IIC bus Busy test routine */
int iic_rw_common(char node, int count, char *buf, int direction, 
     struct IICPB *pb, int er_rpt, char * name); /* Common Read/Write Routine */
int iic_reg_test(struct FILE *fp); /* iic bus controller register test */
int ireg_test(int target, char mask,struct FILE *fp); /* register test */

#if SABLE || RAWHIDE
void srom_display (struct IICPB *pb );
#endif

/*Global variables*/
struct IICB *iic_isr_pb;            /*Pointer to the port block*/
                    /*This is to be used by the isr only*/
struct IICPB *iicdb;            /* Pointer to the driver block  */
extern int iic_inited;
int    iic_debug=0;
#define IIC_DEBUG 0

#if SABLE || MIKASA || ALCOR || RAWHIDE  || PC264 || K2 || TAKARA
     
struct IIC_NODES {
    char *name;      /* pointer to INODE name    */
    int  class;      /* Device Type for this Node    */
    int  length;     /* Device data length       */
    int  test;       /* Flag to Force creation of INODE w/o Test */
    char node;       /* Node address for this Node   */
    char check;	     /* Check type */
    char match;      /* Match on a bit in this mask */
    char junk[1];    /* Pad the structure to ensue longword aligment */
} iic_node_list[] = {   
#if SWORDFISH
    "iic_cont",					IIC_MSTR_TYPE,  256,				1,	0xB6,							0,	0,	0,
	"lm75_primary__",			IIC_EEROM_TYPE,	sizeof(LM75),		0,	LM_75_PRIMARY & 0xff,			0,	0,	0,
	"lm75_secondary__",			IIC_EEROM_TYPE,	sizeof(LM75),		0,	LM_75_SECONDARY & 0xff,			0,	0,	0,
	"adm9240_primary__",		IIC_EEROM_TYPE,	sizeof(ADM9240),	0,	ADM_9240_PRIMARY & 0xff,		0,	0,	0,
	"adm9240_secondary__",		IIC_EEROM_TYPE,	sizeof(ADM9240),	0,	ADM_9240_SECONDARY & 0xff,		0,	0,	0,
	"cs20_psu__",				IIC_EEROM_TYPE,	sizeof(ADM9240),	0,	ADM_9240_CS20PSU & 0xff,		0,	0,	0,
	"mb_pcf8574__",				IIC_UP_TYPE,	sizeof(PCF8574),	1,	PCF_8574_MOTHERBOARD & 0xff,	0,	0,	0,
	"mb_pcf8574_secondary__",	IIC_UP_TYPE,	sizeof(PCF8574),	0,	PCF_8574_MOTHERBOARDSEC & 0xff,	0,	0,	0,
#else
#if !RAWHIDE
#if SABLE || MIKASA || ALCOR || K2 || TAKARA || PC264
     	"iic_cont",  IIC_MSTR_TYPE,  256, 1, 0xB6, 0, 0, 0,
#endif
#if !(MIKASA || ALCOR || K2 || TAKARA || PC264)
     	"iic_mem0",  IIC_EEROM_TYPE, 256, 1, 0xA0, 1, 1, 0,
     	"iic_mem1",  IIC_EEROM_TYPE, 256, 1, 0xA2, 1, 1, 0,
     	"iic_mem2",  IIC_EEROM_TYPE, 256, 1, 0xA4, 1, 1, 0,
	"iic_mem3",  IIC_EEROM_TYPE, 256, 1, 0xA6, 1, 1, 0,
	"iic_cpu0",  IIC_EEROM_TYPE, 256, 1, 0xA8, 1, 2, 0,
	"iic_cpu1",  IIC_EEROM_TYPE, 256, 1, 0xAA, 1, 2, 0,
#endif
#if SABLE
	"iic_cpu2",  IIC_EEROM_TYPE, 256, 1, 0xAE, 1,    2, 0,
	"iic_cpu3",  IIC_EEROM_TYPE, 256, 1, 0xA2, 1,    2, 0,
	"iic_extio", IIC_EEROM_TYPE, 256, 1, 0xAE, 1, 0x10, 0,
	"iic_extio", IIC_EEROM_TYPE, 256, 1, 0xA2, 1, 0x10, 0,
	
	/* Catch-all inode names. If we don't match a memory, cpu, or io */
	/* we will still create an inode so we can at least access the EEROM */

	"iic_a0",    IIC_EEROM_TYPE, 256, 1, 0xA0, 1, 0xfe, 0,
	"iic_a2",    IIC_EEROM_TYPE, 256, 1, 0xA2, 1, 0xec, 0,
	"iic_a4",    IIC_EEROM_TYPE, 256, 1, 0xA4, 1, 0xfe, 0,
	"iic_a6",    IIC_EEROM_TYPE, 256, 1, 0xA6, 1, 0xfe, 0,
	"iic_a8",    IIC_EEROM_TYPE, 256, 1, 0xA8, 1, 0xfd, 0,
	"iic_aa",    IIC_EEROM_TYPE, 256, 1, 0xAA, 1, 0xfd, 0,
	"iic_ae",    IIC_EEROM_TYPE, 256, 1, 0xAE, 1, 0xed, 0,
#endif


#if !(MIKASA || ALCOR || K2 || TAKARA || PC264)
	"iic_io",    IIC_EEROM_TYPE, 256, 1, 0xAC, 0, 0, 0,
#endif
#if !WEBBRICK
	"iic_ocp0",  IIC_LED_TYPE,     1, 1, 0x40, 0, 0, 0,
	"iic_ocp1",  IIC_LED_TYPE,     1, 1, 0x42, 0, 0, 0,
    	"iic_8574_ocp",		IIC_LED_TYPE,      1, 1, 0x4E, 0, 0, 0,  
#endif
#if SABLE
	"iic_psc",   IIC_LED_TYPE,     1, 1, 0x46, 0, 0, 0,
#endif

#if PC264
      	"iic_smb0", 	IIC_EEROM_TYPE, 256, 1, 0xA2, 0, 0, 0,
#if !WEBBRICK
      	"iic_cpu0", 	IIC_EEROM_TYPE, 256, 1, 0xA4, 0, 0, 0,
      	"iic_cpu1", 	IIC_EEROM_TYPE, 256, 1, 0xAC, 0, 0, 0,
#endif
	"iic_system0",	IIC_LED_TYPE,   1,   1, 0x70, 0, 0, 0,
	"iic_system1",	IIC_LED_TYPE,   1,   1, 0x72, 0, 0, 0,
#endif                                       
#endif
#if RAWHIDE
	"iic_cont",	    IIC_MSTR_TYPE,  256, 1, 0xB6, 0, 0, 0,
	"iic_mthrbrd",	    IIC_EEROM_TYPE, 256, 1, 0xA2, 0, 0, 0,
	"iic_cpu0",	    IIC_EEROM_TYPE, 256, 1, 0xA4, 0, 0, 0,
	"iic_cpu1",	    IIC_EEROM_TYPE, 256, 1, 0xA6, 0, 0, 0,
	"iic_bridge0",	    IIC_EEROM_TYPE, 256, 1, 0xA8, 0, 0, 0,
	"iic_saddle0",	    IIC_EEROM_TYPE, 256, 1, 0xAA, 0, 0, 0,
	"iic_cpu_io0",      IIC_EEROM_TYPE, 256, 1, 0xAC, 0, 0, 0,    /* see powerup_rawhide.c */
	"iic_cpu_io1",      IIC_EEROM_TYPE, 256, 1, 0xAE, 0, 0, 0,    /* see powerup_rawhide.c */
	"iic_ocp0",	    IIC_LED_TYPE,     1, 1, 0x40, 0, 0, 0,
	"iic_ocp1",	    IIC_LED_TYPE,     1, 1, 0x42, 0, 0, 0,
	"iic_mem0",         IIC_LED_TYPE,     1, 1, 0x48, 0, 0, 0,
	"iic_mem1",         IIC_LED_TYPE,     1, 1, 0x4A, 0, 0, 0,
	"iic_mem2",         IIC_LED_TYPE,     1, 1, 0x4C, 0, 0, 0,
	"iic_mem3",         IIC_LED_TYPE,     1, 1, 0x4E, 0, 0, 0,
	"iic_system0",	    IIC_LED_TYPE,     1, 1, 0x70, 0, 0, 0,
	"iic_system1",	    IIC_LED_TYPE,     1, 1, 0x72, 0, 0, 0,
#endif
/*									*/
/*	We want sniff for KCRCM devices on all platforms		*/
/*									*/
	"iic_rcm_temp",      IIC_LED_TYPE,          4, 1, 0x9e, 0, 0, 0,
	"iic_rcm_nvram0",    IIC_EEROM_TYPE,      256, 1, 0xC0, 0, 0, 0,
	"iic_rcm_nvram1",    IIC_EEROM_TYPE,      256, 1, 0xC2, 0, 0, 0,
	"iic_rcm_nvram2",    IIC_EEROM_TYPE,      256, 1, 0xC4, 0, 0, 0,
	"iic_rcm_nvram3",    IIC_EEROM_TYPE,      256, 1, 0xC6, 0, 0, 0,
	"iic_rcm_nvram4",    IIC_EEROM_TYPE,      256, 1, 0xC8, 0, 0, 0,
	"iic_rcm_nvram5",    IIC_EEROM_TYPE,      256, 1, 0xCa, 0, 0, 0,
	"iic_rcm_nvram6",    IIC_EEROM_TYPE,      256, 1, 0xCc, 0, 0, 0,
	"iic_rcm_nvram7",    IIC_EEROM_TYPE,      256, 1, 0xCe, 0, 0, 0,
#endif
};

#define max_iic_node (sizeof(iic_node_list)/sizeof(struct IIC_NODES))
#define IIC_TEST_NODE 0x40
#define IIC_TEST_NODE_DATA 0x00
#endif

int iic_validate ();

struct DDB iic_ddb = {
	"iic_mst",		/* how this routine wants to be called	*/
	iic_read,		/* read routine				*/
	iic_write,		/* write routine			*/
	iic_open,		/* open routine				*/
	iic_close,		/* close routine			*/
	null_procedure,		/* name expansion routine		*/
	iic_delete,		/* delete routine			*/
	null_procedure,		/* create routine			*/
	iic_setmode,		/* setmode				*/
	iic_validate,		/* validation routine			*/
	0,			/* class specific use			*/
	1,			/* allows information			*/
	0,			/* must be stacked			*/
	0,			/* is a flash update driver		*/
	0,			/* is a block device			*/
	0,			/* not seekable				*/
	0,			/* is an ethernet device		*/
	0,			/* is a filesystem driver		*/
};


#if IIC_DEBUG
enum itypes {iinit,iopen,iread,iwrite,iclose,isetmode,ipoll};
static char	*itext[7]= 
  {"init: ","open: ","read: ","write: ","close: ","setmode: ","poll"};
int    iop;
#endif


/*+
 * ============================================================================
 * = iic_write - Write a block of data                                         =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *  Write a block of data to the IIC bus.
 *  
 * FORM OF CALL:
 *  
 *  iic_write(fp, size, number, buf)
 *  
 * RETURNS:
 *
 *      len - bytes transferred
 *
 *       
 * ARGUMENTS:
 *
 *  struct FILE *fp - Pointer to a file.
 *  int size - Number of bytes/block to be transferred.
 *  int number - Number of blocks to be transferred.
 *  char *buf - Pointer to the buffer the data is taken from.
 *
 * SIDE EFFECTS:
 *
 *  None
 *
-*/

int iic_write (struct FILE *fp, int size, int number, char *buf) {
struct INODE *ip;           /*Pointer to Inode     */
struct IICCB *iicbp;            /*Pointer to the port block*/ 
struct IICPB *pb;           /*Pointer to the Driver Block */
int status, count,j,k;
char node,buf1[2];

#if IIC_DEBUG
 iop = iwrite;
 pprintf ("%s \n",itext[iop]);
#endif

/*Get a pointer to the port block*/
    ip = fp->ip;
    iicbp = (struct IICCB*)ip->misc;
    pb = iicbp->pb;

    count = max (min ((size*number), ip->len[0] - *fp->offset), 0);
    if (count == 0) {
        return(count);
    }

    if (iicbp->type != IIC_EEROM_TYPE) {
        status = iic_rw_common(iicbp->address,count,buf,IIC_WRITE_DIR,pb,1,
							fp->ip->name);
    }
    else {
        for (j=0;j<count;j++) {
            buf1[0] = *fp->offset + j;
            buf1[1] = buf[j];
            k = 2;
	    
#if IIC_DEBUG
	    pprintf ("%s address 0x%x buf1 0x%x\n",itext[iop],buf1[k],count);
#endif
            status = iic_rw_common(iicbp->address,k,buf1,IIC_WRITE_DIR,pb,1,
							fp->ip->name);
            if (status != k)
                break;
            krn$_sleep(60);
        }
        if(j >= count)
            status = count;
        else
            status = status - 1 + j;
    }   
            
    *fp->offset += status;
    return (status);
}



/*+
 * ============================================================================
 * = iic_read - Read a block of data.                                         =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *  Read in a block of characters from the IIC bus.
 *       Return the number of characters read in.
 *
 *  
 * FORM OF CALL:
 *  
 *  iic_read (fp, size, number, buf)
 *  
 * RETURNS:
 *
 *      s - Bytes transferred. Anything other than the amount requested
 *      indicates EOF or an error.
 *       
 * ARGUMENTS:
 *
 *  struct FILE *fp - Pointer to a file.
 *  int size - Number of bytes/block to be transferred.
 *  int number - Number of blocks to be transferred.
 *  char *buf - Pointer to the buffer the data is written to.
 *
 * SIDE EFFECTS:
 *
 *  None
 *
-*/

int iic_read (struct FILE *fp, int size, int number, char *buf) {
struct INODE *ip;           /*Pointer to Inode     */
struct IICCB *iicbp;            /*Pointer to the port block*/ 
struct IICPB *pb;           /*Pointer to the Driver Block */
int status,count;
char node,buf1[1];

#if IIC_DEBUG
  iop = iread;
  pprintf ("%s \n",itext[iop]);
#endif

/*Get a pointer to the port block*/
    ip = fp->ip;
    iicbp = (struct IICCB*)ip->misc;
    pb = iicbp->pb;

    count = max (min ((size*number), ip->len[0] - *fp->offset), 0);
    if (count == 0) {
        return(count);
    }
    if (iicbp->type == IIC_EEROM_TYPE) {
		buf1[0] = (char) *fp->offset;
        status = iic_rw_common(iicbp->address,1,buf1,IIC_WRITE_DIR,pb,1,
							fp->ip->name);

        if(status != 1)
            return(status);
    }

#if IIC_DEBUG
    pprintf ("%s call read address count %d \n",itext[iop],count);
#endif
    status = iic_rw_common(iicbp->address,count,buf,IIC_READ_DIR,pb,1,
							fp->ip->name);
#if IIC_DEBUG
    pprintf ("%s buf 0x%x\n",itext[iop],buf[0]&0xff);
#endif

    *fp->offset += status;
    return(status);
}


/*+
 * ============================================================================
 * = iic_rw_common - Common routine for iic bus transmisions                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *  Read or write a block of characters from/to the IIC bus.
 *       Return the number of characters transfered.
 *
 *  
 * FORM OF CALL:
 *  
 *  iic_rw_common (node, count, buf, direction, pb, er_rpt, name)
 *  
 * RETURNS:
 *
 *      s - Bytes transferred. Anything other than the amount requested
 *      indicates EOF or an error.
 *       
 * ARGUMENTS:
 *
 *  char node  - IIC bus address to transfer the data to/from
 *  int count - Number of bytes to be transferred.
 *  char *buf - Pointer to the buffer the data is written to.
 *  int direction - direction of the transfer either IIC_READ_DIR for 
 *      reads or IIC_WRITE_DIR for writes
 *  struct IICPB *pb - pointer to the iic bus driver port data block
 *  int er_rpt - Do not report errors is == 0
 *  char *name - Device name 
 *
 * SIDE EFFECTS:
 *
 *  None
 *
-*/

int iic_rw_common(char node, int count, char *buf, int direction, 
        struct IICPB *pb, int er_rpt, char *name) { 

int status;

    krn$_wait( &pb->avail_sem);

    while(pb->direction != IIC_IDLE)
        krn$_sleep(100);      

#if IIC_DEBUG
    pprintf ("%s now idle \n",itext[iop]);
#endif
    pb->count = count;
    pb->buffer_addr = buf;
    if(direction == IIC_WRITE_DIR) {
        pb->rec_count = 0;
    }
    else {
        pb->rec_count = -1;
    }

#if IIC_DEBUG
    pprintf ("%s check for busy  \n",itext[iop]);
#endif
    if(iic_busb(er_rpt,pb->iic_fp1) != msg_success) {
        krn$_post( &pb->avail_sem);
        return(0);
    }

#if IIC_DEBUG
    pprintf ("%s not busy \n",itext[iop]);
#endif
    node = (node & 0xfe) + direction;
    status = msg_lost_arb;
    while (status == msg_lost_arb)
        {
        pb->direction = direction;
		pb->isr_complete = 0;		
	    pb->misr_t.sem.count = 0;

#if IIC_DEBUG
        pprintf ("%s node 0x%x \n",itext[iop],node );
#endif
 

#if WEBBRICK || PC264
/* If we have lost arbitration we need to check to see if the bus is 
 * busy again before re-issueing the command
 */
	krn$_micro_delay( 1000 );	
	if(iic_busb(er_rpt,pb->iic_fp1) != msg_success) {
	    status = iic_read_csr(IIC_STATUS,pb->iic_fp2) & 0xff;
	    krn$_stop_timer(&pb->misr_t);
	    break;
	}
	
#endif


       iic_write_csr(node,IIC_DATA,pb->iic_fp1);

#if WEBBRICK || PC264

/* check one more time to make sure bus is not busy when reading 
 * before issueing the start command.  This is suggested in the Philips
 * specification.					
 */
	if(iic_busb(er_rpt,pb->iic_fp1) != msg_success) {
	    status = iic_read_csr(IIC_STATUS,pb->iic_fp2) & 0xff;
	    krn$_stop_timer(&pb->misr_t);
	    break;
	}
#endif
#if IIC_DEBUG
        pprintf ("%s start transfer timer \n",itext[iop]);
#endif
        iic_write_csr((IIC_START | pb->int_flag),IIC_STATUS,pb->iic_fp1);
	krn$_start_timer(&pb->misr_t, IIC_MAS_TIMEOUT);


#if IIC_DEBUG
        pprintf ("%s wait for transmission complete \n",itext[iop]);
#endif
        status = krn$_wait (&pb->misr_t.sem);

	krn$_stop_timer(&pb->misr_t);

	if(status!=IIC_SR_DONE)krn$_sleep(5);
        }
	switch(status) {
		case IIC_SR_DONE:
#if IIC_DEBUG
        	        pprintf ("%s transmission done \n",itext[iop]);
#endif
			break;
		case TIMEOUT$K_SEM_VAL:
			if (pb->isr_complete == 1)
				break;		/* not a valid timeout */

#if IIC_DEBUG
        	pprintf ("%s transmission timeout--issue stop-make idle \n",itext[iop]);
#endif
	        iic_write_csr((IIC_STOP | pb->int_flag),IIC_STATUS,pb->iic_fp1);
			pb->rec_count = 0;  /* clear the transfer counter to show error */

		err_printf(msg_ex_cmd_timeout,name);
    			pb->direction = IIC_IDLE;
			break;
		default:
		    if (er_rpt)	
        		err_printf(msg_iic_bad_status,status);
	}

    status = pb->rec_count;
#if IIC_DEBUG
    pprintf ("%s routine done status 0x%x\n",itext[iop],status);
#endif
    krn$_post( &pb->avail_sem);
    return(status);
}


/*+
 * ============================================================================
 * = iic_open - Open the IIC Bus device.                                      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      Open the IIC Bus device.  Allow only one writer/reader.
 *  
 * FORM OF CALL:
 *  
 *  iic_open (fp)
 *  
 * RETURNS:
 *
 *  0 - Success
 *       
 * ARGUMENTS:
 *
 *  struct FILE *fp - File to be opened.
 *
 * SIDE EFFECTS:
 *
 *  None
 *
-*/
int iic_open (struct FILE *fp, char *file_name, char *next, char *mode) {

int status;
struct IICCB *iicbp;
struct IICPB *pb;           /*Pointer to the Driver Block */

#if IIC_DEBUG
    iop = iopen;
    pprintf ("%s entry \n",itext[iop]);
#endif

    iicbp = (struct IICCB*)fp->ip->misc;
    pb = iicbp->pb;
	if(pb->mode != DDB$K_INTERRUPT ) {
	    if(!(fp->mode & MODE_M_SILENT))
		err_printf(msg_driver_stopped,"iic");
	    return(msg_failure);
	}
    krn$_wait( &iicbp->avail_sem);
	if(strlen(file_name) > 0) {		/* If additional information provided */
		if ((status = common_convert(file_name,16,fp->offset,sizeof (*fp->offset))) != msg_success) {
		    return status;
		}
	}	

#if IIC_DEBUG
    pprintf ("%s done \n",itext[iop]);
#endif
    return msg_success;
}


/*+
 * ============================================================================
 * = iic_close - Close the IIC Bus device.                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      Close the IIC Bus device.
 *
 * FORM OF CALL:
 *  
 *  iic_close (fp)
 *  
 * RETURNS:
 *
 *  None
 *       
 * ARGUMENTS:
 *
 *  struct FILE *fp - File to be closed.
 *  
 *
 * SIDE EFFECTS:
 *
 *  None
 *
-*/

int iic_close (struct FILE *fp) {

struct IICCB *iicbp;

#if IIC_DEBUG
    iop = iclose;
    pprintf ("%s entry \n",itext[iop]);
#endif

    iicbp = (struct IICCB*)fp->ip->misc;
    krn$_post( &iicbp->avail_sem);

#if IIC_DEBUG
    pprintf ("%s done \n",itext[iop]);
#endif

    return msg_success;
}
    

/*+
 * ============================================================================
 * = iic_delete - Delete the IIC Bus device.                                  =

 * ============================================================================
 *
 * OVERVIEW:
 *  
 *  Delete the IIC Bus device.
 *  
 * FORM OF CALL:
 *  
 *  iic_delete (ip)
 *
 * RETURNS:
 *
 *  0 - Success
 *       
 * ARGUMENTS:
 *
 *  struct INODE *ip - Inode to be deleted.
 *
 * SIDE EFFECTS:
 *
 *  None
 *
-*/

int iic_delete (struct INODE *ip) {
    INODE_UNLOCK (ip);
    return msg_success;
}


/*+
 * ============================================================================
 * = iic_setmode - Set mode                                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *  Changes the mode of the iic bus driver from polled to interrupt driven or
 *  vice versa.
 *  
 * FORM OF CALL:
 *  
 *  iic_setmode ()
 *  
 * RETURNS:
 *
 *  msg_success - Normal completion.
 *       
 * ARGUMENTS:
 *
 *  None
 *
 * SIDE EFFECTS:
 *
 *  None
 *
-*/

void iic_setmode (int mode) {
#if RAWHIDE
        int global_id = GLOBAL_ID( whoami() );
	UINT busadr;
	unsigned int read_val;
	int status;
#endif
	
#if IIC_DEBUG
    pprintf ("%s mode 0x%x \n",itext[iop],mode);
#endif

    if (mode == DDB$K_READY) return;

    switch (mode) {

    case DDB$K_STOP:
#if IIC_DEBUG
	pprintf ("%s stop - \n",itext[iop]);
#endif
#if SABLE 
	io_disable_interrupts (0, IIC_VECTOR);
#endif
	break;

    case DDB$K_START:
#if IIC_DEBUG
	pprintf ("%s turn on interrupts \n",itext[iop]);
#endif
#if SABLE 
	io_enable_interrupts (0, IIC_VECTOR);
#endif
#if RAWHIDE
/* First stop the polling process */
    if (iic_poll_pid)
    {
	krn$_delete(iic_poll_pid);
	iic_poll_pid = 0;
    }
    int_vector_set(IIC_VECTOR1, iic_service, iicdb);
    int_vector_set(IIC_VECTOR2, iic_service, iicdb);

    /* enable IIC interrupts */
    busadr = SYSTEM_CSR(global_id, IOD0, INT_MASK0);
    read_val = ldl(busadr);
    write_csr_32(busadr,read_val|0x60000);
#endif
	iicdb->int_flag = IIC_ENI;		/* Turn on Interrupts */
	mode = DDB$K_INTERRUPT;
	break;
    }

    iicdb->mode = mode;
}


/*+
 * ============================================================================
 * = iic_validate - validate data structures internal to the IIC Bus driver   =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *  This routine is called by higher level routines to validate internal
 *  data structures and the general health of the IIC Bus driver.
 *  
 * FORM OF CALL:
 *  
 *  IIC_validate (ip)
 *  
 * RETURNS:
 *
 *  msg_success - no errors detected
 *  msg_nl_inode    - inconsistancy in inode entry
 *  msg_nl_ddb  - inconsistancy in ddb entry
 *       
 * ARGUMENTS:
 *
 *  struct INODE *ip - address of inode for TT
 *
 * SIDE EFFECTS:
 *
 *
-*/
int iic_validate () {
#if VALIDATE
    struct DDB  *dp;
    struct FILE *fp;
    char        buf[1],buf1[1];
    int         i,status,rtn_status;

    fp = fopen("iic_ocp0","r+");
    if(fp == 0) {
        printf(msg_bad_open,"iic_ocp0");
        return msg_failure;
    }

    rtn_status = msg_success;

    for (i=0; i<256; i++) {
        buf[0] = (char) i;
                status = fseek(fp,0, SEEK_SET);
        status = fwrite(buf,1,1,fp);
        if(status == 0) {
            printf(msg_dep_devwriteerr,1,1,"iic_ocp0");
            rtn_status = msg_failure;
            break;
        }

        status = fseek(fp,0, SEEK_SET);
        status = fread(buf1,1,1,fp);
        if(status == 0) {
            printf(msg_ex_devreaderr,1,1,"iic_ocp0");
            rtn_status = msg_failure;
            break;
        }

        if (buf[0] != buf1[0]) {
            printf(msg_data_comp_err,1,&buf[0],"iic_ocp0");
            rtn_status = msg_failure;
            break;
        }
    }

    fclose(fp);

    return (rtn_status);
#else 
    return msg_success;
#endif
}


/*+
 * ============================================================================
 * = iic_init - Initialize the IIC Bus driver.                                =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *  Initialize the IIC Bus driver and sniffs the iic bus creating an INODE for each
 *  device it finds on the bus.  A list of all of the address which are analyzed can be
 *  found in IIC_NODE at the top of this file.
 *  
 * FORM OF CALL:
 *  
 *  iic_init ()
 *  
 * RETURNS:
 *
 *  None
 *       
 * ARGUMENTS:
 *
 *  None
 *
 * SIDE EFFECTS:
 *
 *
-*/


int iic_init () {
    struct INODE    *ip;
    struct IICCB    *iicbp;     /*Pointer to the port block*/ 
    struct IICPB    *iic_p_b;   /*Pointer to the driver block*/ 
    struct IIC_NODES *ilist;

    char data, sname [MAX_NAME];
    unsigned char buf[1];
    int i,status;

    if (iic_debug)return (msg_success);
    if(iic_inited == 1)
		return(msg_success);

#if IIC_DEBUG
   iop = iinit;
   pprintf ("%s entry \n",itext[iop]);
#endif

/* Create the Driver block */
    iic_p_b = (struct IICPB*)dyn$_malloc(sizeof(struct IICPB),
            DYN$K_SYNC+DYN$K_NOOWN);
    iicdb = iic_p_b;  /* save a pointer for use by the set mode routine */

/* Initialize the Hardware */
#if IIC_DEBUG
    pprintf ("%s iic_reset \n",itext[iop]);
#endif
    if(iic_reset(iic_p_b->iic_fp1) != msg_success)  /* Initilize the Hardware */
        return(msg_failure);       /* Init failure do not enable */

/* Initialize the Driver Semaphores and direction */
#if IIC_DEBUG
    pprintf ("%s initialize structures \n",itext[iop]);
#endif
    krn$_seminit(&iic_p_b->misr_t.sem,0,"iic_done"); /* Driver Completion*/
    krn$_seminit(&iic_p_b->isr_t.sem,0,"iic_s_done"); /* Slave Completion*/
    krn$_init_timer( &iic_p_b->misr_t);
    krn$_init_timer( &iic_p_b->isr_t);
    krn$_seminit(&iic_p_b->avail_sem,1,"iic_avail"); /* Driver Available*/
    iic_p_b->direction = IIC_IDLE;  /* force the poll routine to be idle */


/* Create an inode entry, thus making the device visible as a file. */
    ilist = & iic_node_list[0];
    allocinode (iic_ddb.name, 1, &ip);
    ip->dva = & iic_ddb;
    ip->attr = ATTR$M_READ+ATTR$M_WRITE;
    ip->len[0] = ilist->length;
    ip->alloc[0] = ilist->length;

/*Get the pointers to the port block*/
    ip->misc = (int*)dyn$_malloc(sizeof(struct IICCB),DYN$K_SYNC+DYN$K_NOOWN);
    iicbp = (struct IICCB*)ip->misc;
    iicbp->pb = iic_p_b;
    iicbp->address = ilist->node;
    iicbp->type = ilist->class;
    iic_isr_pb = iicbp;
    INODE_UNLOCK (ip);

/* Make Semphore names and init semaphores */
    sprintf(sname,"%s_avail",ilist->name);
    krn$_seminit(&iicbp->avail_sem,1,sname); /* Node Available*/


#if POLLED
#if RAWHIDE || PC264
/* Start a polling process */
    iic_poll_pid = krn$_create (iic_poll_rt,null_procedure,0, 3, -1, 1024 * 12,
                            "srom_poll", 
                            0, 0,
                            0, 0,
                            0, 0,
                            iic_p_b);
#else
/* Start a polling process */
    krn$_create (iic_poll_rt,null_procedure,0, 1, -1, 1024 * 12,
                            "srom_poll", 
                            0, 0,
                            0, 0,
                            0, 0,
                            iic_p_b);
#endif
#else
	/* Set the int vector */
    status = int_vector_set(IIC_VECTOR, iic_service, iic_p_b);
    if(status != msg_success){
        pprintf(msg_iic_bad_int_set);
    } 

	iicdb->int_flag = IIC_ENI;		/* Turn on Interrupts */
    iic_p_b->mode = DDB$K_INTERRUPT; /* Set interrupt mode */
#if SABLE 
    io_enable_interrupts (0, IIC_VECTOR);
#endif
#endif

/* Create an inode entry for each device on the IIC bus */
    for(i=1;i < max_iic_node;i++) {

    	ilist = & iic_node_list[i];

/* Verify the existence of this device, */
        if(ilist->test == 1) {  /* If test != Force the INODE */
	    
#if IIC_DEBUG
	    pprintf ("%s read verify node 0x%x \n",itext[iop],ilist->node);
#endif
            status = iic_rw_common(ilist->node,1,buf,IIC_READ_DIR,iic_p_b,0,ilist->name);
#if IIC_DEBUG
	    pprintf ("%s node 0x%x status 0x%x data= 0x%x\n",itext[iop],ilist->node,status,buf[0]);
#endif

	    if(status != 1) continue;
        }
#if SABLE || RAWHIDE || PC264
	if (ilist->class == IIC_EEROM_TYPE && ilist->check) {
	    buf[0] = 0xfe;
	    status = iic_rw_common (ilist->node, 1, buf, IIC_WRITE_DIR,
		    iic_p_b, 0, ilist->name);
	    if (status != 1)
		continue;
	    status = iic_rw_common (ilist->node, 1, buf, IIC_READ_DIR,
		    iic_p_b, 0, ilist->name);
	    if (status != 1)
		continue;
	    if (!(ilist->match & (1 << ((buf[0] >> 5) & 7))))
	        continue;
	}
#endif

/* Create an inode entry, thus making the device visible as a file. */
#if IIC_DEBUG
	pprintf("Creating inode %s\n", ilist->name);
#endif
        allocinode (ilist->name, 1, &ip);
        ip->dva = & iic_ddb;
        ip->len[0] = ilist->length;
        ip->alloc[0] = ilist->length;
        ip->attr = ATTR$M_READ+ATTR$M_WRITE;

/*Get the pointers to the port block*/
        ip->misc = (int*)dyn$_malloc(sizeof(struct IICCB),
			DYN$K_SYNC+DYN$K_NOOWN);
        iicbp = (struct IICCB*)ip->misc;
        iicbp->pb = iic_p_b;
        iicbp->address = ilist->node;
        iicbp->type = ilist->class;
        INODE_UNLOCK (ip);

/* Make Semphore names and init semaphores */
        sprintf(sname,"%s_avail",ilist->name);
        krn$_seminit(&iicbp->avail_sem,1,sname); /* Node Available*/

    }
	iic_inited = 1;
    return(msg_success);
}

void iic_reinit () {
    if(iic_inited == 1) {
	iic_reset (iicdb->iic_fp1);
#if SABLE
	io_enable_interrupts (0, IIC_VECTOR);
#endif
    }
}


/*+
 * ============================================================================
 * = iic_reset - Reset the IIC Bus driver hardware.                           =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *  This routine will issue an IIC bus reset.
 *  
 * FORM OF CALL:
 *  
 *  iic_reset (fp)
 *  
 * RETURNS:
 *
 *  None
 *       
 * ARGUMENTS:
 *
 *  fp - File pointer to the master iic bus controller in the system
 *               
 * SIDE EFFECTS:
 *
 *  Reset the IIC bus controller hardware and establishes the 
 *  bus and input clock frequencies for this device.
 *
-*/

int iic_reset (struct FILE *fp) {
    char data;
    int i;

/* Initialize the Hardware */
#if IIC_DEBUG
    pprintf ("%s iic_reset \n",itext[iop]);
#endif
    iic_chip_reset();

#if IIC_DEBUG
    pprintf ("%s write pin \n",itext[iop]);
#endif
    if( iic_write_csr(IIC_PIN,IIC_STATUS,fp) != msg_success)
        return(msg_failure);

/* Test the iic bus controler registers */

#if 0
    if( iic_reg_test(fp) != msg_success)
        return(msg_failure); 
#endif

/* Reset the Slave Address */

    if( iic_write_csr(IIC_S0P,IIC_STATUS,fp) != msg_success)
        return(msg_failure);
    data = (iic_node_list[0].node >> 1) & 0x7f;
#if IIC_DEBUG
    pprintf ("%s reset slave address 0x%x \n",itext[iop],
	iic_node_list[0].node&0xff);
#endif
    if( iic_write_csr(data,IIC_DATA,fp) != msg_success)
        return(msg_failure);        

/* Define the iic bus clock frequencies */

    if( iic_write_csr(IIC_S2,IIC_STATUS,fp) != msg_success)
        return(msg_failure);
    if( iic_write_csr(IIC_CLOCK,IIC_DATA,fp) != msg_success)
        return(msg_failure);

    iic_write_csr(IIC_INIT,IIC_STATUS,fp);
#if IIC_DEBUG
    pprintf ("%s iic_reset done-enable serial communication \n",itext[iop]);
#endif
    return(msg_success);
}


/*+
 * ============================================================================
 * = iic_poll_rt - poll the IIC bus for pending interrupt                     =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *  This routine polls the IIC bus master controler for a pending
 *  interrupt condition, and calls the iic_service when an interrupt is
 *  pending
 *  
 * FORM OF CALL:
 *  
 *  iic_poll_rt (iicpb)
 *  
 * RETURNS:
 *
 *  None
 *       
 * ARGUMENTS:
 *
 *  iicpb - pointer to the IIC driver port block
 *              
 * SIDE EFFECTS:
 *
 *  None
 *
-*/

void iic_poll_rt( struct IICPB *pb) {

union   IIC_STATUS_UNION status;

#if SABLE || MIKASA || ALCOR || RAWHIDE  || PC264 || K2 || TAKARA
#if IIC_DEBUG
  pprintf ("poll_rt: \n");
#endif
  while(1) {
 
    status.sword = iic_read_csr(IIC_STATUS,pb->iic_fp1) & 0xff;

    if(status.bits.pin == 0) {
        iic_service(pb);	/* If IIC ready service it */
    }

#if WEBBRICK || PC264
/* For the Philips 8584 PIN=1 for lost arbitration condition need to check for LAB bit
 */
    if(status.bits.lab !=0){
	iic_service(pb);    /* we have an error service it */
     }
#endif

    if(pb->direction == IIC_IDLE)
        krn$_sleep(1);

     
#if RAWHIDE
    /* Only allow this process to be killed if we've switched to interrupt 
       mode */
    if (iicdb->mode == DDB$K_INTERRUPT)
        check_kill();
#endif
  }
#endif
}
 

/*+
 * ============================================================================
 * = iic_service - services an IIC bus interrupt                              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *  This routine services the IIC bus master controler interrupts
 *  transmitting or recieving the next byte of data as necessary
 *  This routine will also terminate the write/read tranaction on either
 *  completion or status error condition.
 *  
 * FORM OF CALL:
 *  
 *  iic_service (iicpb)
 *  
 * RETURNS:
 *
 *  None
 *       
 * ARGUMENTS:
 *
 *  iicpb - pointer to the IIC driver port block
 *              
 * SIDE EFFECTS:
 *
 *  This routine updates the rec_count, status, buffer fields of the 
 *  port block as necessary.
 *
-*/

void iic_service( struct IICPB *pb) {

union   IIC_STATUS_UNION status;
char    address,data;
#if RAWHIDE
int global_id = GLOBAL_ID( whoami() );
UINT busadr;
#endif
   status.sword = iic_read_csr(IIC_STATUS,pb->iic_fp2) & 0x7f;

#if SABLE 
    io_issue_eoi (0, IIC_VECTOR);
#endif

#if WEBBRICK
    if((status.bits.lab !=0) || (status.bits.ber != 0)){
#else
   if(status.bits.lab != 0) {
#endif
        pb->direction = IIC_IDLE;
		pb->isr_complete = 1;  /* indicate isr completed */
#if IIC_DEBUG
        pprintf ("%s lost arbitration-make idle \n",itext[iop]);
#endif
#if WEBBRICK || PC264
    /* if the Philips 8584 losses arbitration it may hang and needs to be reset.
     * Philips also suggests waiting before re-issueing the command as after a reset
     * the 8584 might not recognize a busy bus.
     */
	iic_write_csr((IIC_PIN | pb->int_flag),IIC_STATUS, pb->iic_fp1);
	iic_write_csr((IIC_INIT | pb->int_flag), IIC_STATUS, pb->iic_fp1);
	krn$_sleep(100);
#endif
        krn$_post(&pb->misr_t.sem,msg_lost_arb);      
        return;
   }



    switch (pb->direction) {

       case IIC_IDLE:
        if((status.bits.aas == 0) || (pb->slave_addr == 0))
            return;
        address = iic_read_csr(IIC_DATA,pb->iic_fp2);
        if ((address && 0x01) == 1) {
            pb->direction = IIC_S_READ;
#if IIC_DEBUG
    	    pprintf ("%s service-dir=read \n",itext[iop]);
#endif
        }
        else {
            pb->direction = IIC_S_WRITE;
            pprintf("slave write\n");
        }
        break;
    
       case IIC_WRITE_DIR:
#if IIC_DEBUG
 	pprintf ("%s iic_service write\n",itext[iop]);
#endif
        if(status.sword != 0x00)
	    {
#if IIC_DEBUG
	    /* We have an error Roger	*/
 	    pprintf ("%s iic_service status 0x%x\n",itext[iop],status);
            if(status.bits.bb) pprintf ("%s iic_service bus busy \n",itext[iop]);
            if(status.bits.lab)pprintf ("%s iic_service lost arb \n",itext[iop]);
            if(status.bits.aas)pprintf ("%s iic_service aas \n",itext[iop]);
            if(status.bits.ado)pprintf ("%s iic_service last rec \n",itext[iop]);
            if(status.bits.ber)pprintf ("%s iic_service bus error \n",itext[iop]);
            if(status.bits.sts)pprintf ("%s external master stop \n",itext[iop]);
#endif
            pb->status = status.sword;

            iic_write_csr((IIC_STOP | pb->int_flag),IIC_STATUS,pb->iic_fp2);
            pb->direction = IIC_IDLE;
	    pb->isr_complete = 1;  
#if IIC_DEBUG
   	    pprintf ("%s iic_service error-stop \n",itext[iop]);
#endif
            krn$_post(&pb->misr_t.sem,msg_failure);
            return;
            }   

#if IIC_DEBUG
 	pprintf ("%s iic_service status ok \n",itext[iop]);
#endif
	/* On a write, rec_count is zero	*/
	/* initially				*/
        if(pb->count == pb->rec_count) 
	    {
#if IIC_DEBUG
 	    pprintf ("%s iic_service transmit done-stop and post\n",itext[iop]);
#endif

	    iic_write_csr((IIC_STOP | pb->int_flag),IIC_STATUS,pb->iic_fp2);

            pb->direction = IIC_IDLE;
	    pb->isr_complete = 1;  
            krn$_post(&pb->misr_t.sem,IIC_SR_DONE);
            } 
        else
            {
#if IIC_DEBUG
   	    pprintf ("%s iic_service write next byte 0x%x\n",
		itext[iop],pb->buffer_addr[pb->rec_count]&0xff);
#endif
	    iic_write_csr(pb->buffer_addr[pb->rec_count++],
                    IIC_DATA,pb->iic_fp2);
	    }

        break;

       case IIC_READ_DIR:
#if IIC_DEBUG
 	pprintf ("%s iic_service read \n",itext[iop]);
#endif
        if (pb->rec_count == -1) 
	  {
	  /* rec_count = -1 if have not receive anything yet */
	  /* So this is the first byte received		     */
          if(status.sword != 0x00)
	        {
#if IIC_DEBUG
 		pprintf ("%s iic_service read status 0x%x\n",itext[iop],status);
                if(status.bits.bb) pprintf ("%s iic_service bus busy \n",itext[iop]);
                if(status.bits.lab)pprintf ("%s iic_service lost arb \n",itext[iop]);
                if(status.bits.aas)pprintf ("%s iic_service aas \n",itext[iop]);
                if(status.bits.ado)pprintf ("%s iic_service last rec \n",itext[iop]);
                if(status.bits.ber)pprintf ("%s iic_service bus error \n",itext[iop]);
                if(status.bits.sts)pprintf ("%s external master stop \n",itext[iop]);
#endif
                pb->status = status.sword;
                iic_write_csr((IIC_STOP | pb->int_flag),IIC_STATUS,pb->iic_fp2);
                pb->direction = IIC_IDLE;
				pb->isr_complete = 1;  /* indicate isr completed */
                krn$_post(&pb->misr_t.sem,msg_failure);
                return;
                }   

            pb->buffer_addr[++pb->rec_count] = 
		iic_read_csr(IIC_DATA,pb->iic_fp2);

#if IIC_DEBUG
 	    pprintf ("%s iic_service data 0x%x\n",itext[iop],pb->buffer_addr[pb->rec_count]&0xff);
#endif   
 	    }
            else 
	        {
#if IIC_DEBUG
 	          pprintf ("%s rec count 0x%x\n",itext[iop],pb->rec_count);
#endif   

		  /* Multiple Byte Transfer	*/    
		  /* If not last bit and count=1*/
		  /* STOP transmission		*/
	            if((((status.sword != 0x08) &&
	                 (status.sword != 0x11)) &&
			    (pb->count == pb->rec_count)) ||
		        ((status.sword != 0x00) &&
			    (pb->count != pb->rec_count))) 
		   {
		   pb->status = status.sword;
                   iic_write_csr((IIC_STOP | pb->int_flag),IIC_STATUS,pb->iic_fp2);
                   pb->direction = IIC_IDLE;
		   pb->isr_complete = 1;  
#if IIC_DEBUG
  	           pprintf ("%s done-post error and make idle \n",itext[iop]);
#endif
                   krn$_post(&pb->misr_t.sem,msg_failure);
                   return;
                   }

	       /* Else must be done so post it	*/
               if(pb->count == pb->rec_count)
		 {
                 iic_write_csr((IIC_STOP | pb->int_flag),IIC_STATUS,pb->iic_fp2);
                 pb->direction     = IIC_IDLE;
		 pb->isr_complete  = 1;  
#if IIC_DEBUG
  	         pprintf ("%s done post and make idle count 0x%x\n",itext[iop],pb->count);
#endif
                 krn$_post(&pb->misr_t.sem,IIC_SR_DONE);
                 }
		else
		    {
		    pb->buffer_addr[pb->rec_count++] = 
			    iic_read_csr(IIC_DATA,pb->iic_fp2);
		    if (pb->count == pb->rec_count)
			iic_write_csr((IIC_NACK | pb->int_flag),IIC_STATUS,pb->iic_fp2);
		    }
	     }  
        break;

       case IIC_S_READ:
#if IIC_DEBUG
 	pprintf ("%s iic_s_read iic_service status_read \n",itext[iop]);
#endif
        if(status.sword == 0x20) {
#if IIC_DEBUG
 	    pprintf ("%s iic_service stop \n",itext[iop]);
#endif
            pb->slave_addr = 0;
            pb->direction = IIC_IDLE;
			pb->isr_complete = 1;  /* indicate isr completed */
            krn$_post(&pb->isr_t.sem,IIC_SR_DONE);
        }
        data = iic_read_csr(IIC_DATA,pb->iic_fp2);
        if(pb->slave_addr == 0)
		break;
        pb->slave_addr[pb->slave_count++] = data;
        if((pb->slave_addr[0] == pb->slave_count) || 
       (pb->slave_count > sizeof(struct SD_PACKET))){
            pb->slave_addr = 0;
            pb->direction = IIC_IDLE;
			pb->isr_complete = 1;  /* indicate isr completed */
            krn$_post(&pb->isr_t.sem,IIC_SR_DONE);
        }
        break;

       case IIC_S_WRITE:
            break;

    }

 return;
}


/*+
 * ============================================================================
 * = iic_busb - Wait for the IIC bus to be not busy                           =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      This routine will poll the IIC bus until the bus is detected to be in
 *  idle state or 100 poll attempts have been made.
 *
 *  The routine will return 0 on sucess and 1 on busy timeout.
 *
 * FORM OF CALL:
 *  
 *  iic_busb (er_rpt,fp)
 *  
 * RETURNS:
 *
 *  0 - sucess
 *  1 - bus busy timeout
 *       
 * ARGUMENTS:
 *
 *  int er_rpt - Do not display error reports if == 0
 *              
 *  fp - File pointer to the Cobra iic bus mailbox file. 
 *               
 * SIDE EFFECTS:
 *
 *  None
 *
-*/

int iic_busb(int er_rpt,struct FILE *fp){

int i;
union IIC_STATUS_UNION status;

#if IIC_DEBUG
    pprintf ("%s wait for bus not busy \n",itext[iop]);
#endif

    for(i=0;i<100000;i++){
        status.sword = iic_read_csr(IIC_STATUS,fp) & 0xff;
        if(status.bits.bb == 1){
#if IIC_DEBUG
  	    pprintf ("%s iic_busb bus not busy\n",itext[iop]);
#endif

            return(msg_success);
        }
    }
    if(er_rpt)
        err_printf(msg_iic_bus_timeout);

    return(msg_failure);
}
