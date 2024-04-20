/* file:	pdq_driver.c
 *
 * Copyright (C) 1994, 1995 by
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
 *+
 *  FACILITY:
 *
 *      Cobra Firmware
 *
 *  ABSTRACT:	PCI/EISA FDDI port driver.
 *
 *	PDQ (DEFEA/DEFPA) FDDI Port driver.
 *
 *  MODIFICATION HISTORY:
 *
 *
 *      05-Mar-1998     mar     pdq_read () - change PDQ_CALLBACK_TIMEOUT value
 *				for a callback read.
 *	10-Oct-1995	dwn 	Removed reference to on board EISA Memory
 *	06-Sep-1995	dwn 	Adapted to APS, fixed callbacks and stop/start
 *      14-Mar-1995	sfs	Combined FR and FW
 *      14-Mar-1995	dpm 	Merged FR and FW
-*/

/* $INCLUDE_OPTIONS$ */
/* $INCLUDE_OPTIONS_END$ */
#include "cp$src:platform.h"		/* System bus definitions */
#include "cp$src:kernel_def.h"		/* Kernel definitions*/
#include "cp$src:common.h"		/* Common definitions*/
#include "cp$inc:prototypes.h"
#include "cp$src:mop_def.h"     	/* MOP definitions*/
#include "cp$src:mb_def.h"      	/* MOP definitions*/
#include "cp$src:parse_def.h"		/* Parser definitions*/
#include "cp$src:pci_def.h"
#include "cp$src:hwrpb_def.h"
#include "cp$src:ni_dl_def.h"           /* Datalink definitions*/
#include "cp$src:pb_def.h"              /* Port block definitions*/
#include "cp$src:mop_counters64.h"      /* MOP counters*/
#include "cp$src:dynamic_def.h"		/* Memory allocation defs*/
#include "cp$src:ni_env.h"		/* Environment variables*/
#include "cp$src:msg_def.h"		/* Message definitions*/
#include "cp$src:ev_def.h"              /* Environment Variable Data Struct */

#include "cp$src:pdq_def.h"		/* PDQ FDDI Adapter definitions*/
#include "cp$src:pdq_pb_def.h"		/* PDQ PB Port definitions*/

#include "cp$inc:platform_io.h"		
#include "cp$src:probe_io_def.h"
#include "cp$src:ctype.h"
#include "cp$inc:kernel_entry.h"

/*
int pdq_inited = 0;
*/


/* DEBUG CONSTANTS
*/
#define ENABLE_FDDI_RCV      0		/* enable print xmt rcv frame info */
#define ENABLE_FDDI_XMT      0		/* enable print xmt rcv frame info */
#define ENABLE_ENET_IEEE_RCV 0		/* enable print xmt rcv frame info */
#define ENABLE_ENET_IEEE_XMT 0		/* enable print xmt rcv frame info */
#define ENABLE_PRINT_SA      0		/* enable printing Station Adrs */
#define ENABLE_PRINTF        0		/* enable debug pprintfs */

#define CALLBACK_TRACE       0		
#define CALLBACK_PRINTF      0		
#define CONTROL_T	     0		

#define FORCE_LOOPBACK       0		/* inits PDQ in loopback mode */
#define	PDQ_PRINT_CHARSGET   0		/* enables printing StatusCharsGet */

#define PDQ_SERVICE_POST     0		/* enable descriptor index printing */
#define PDQ_BPT              0		/* enable initial XDELTA */
#define PDQ_DEBUG	     0		/* enable Diagnostic Routines */
#define	PDQ_TRACE            0		/* enables common.h trace macro */
#define	PDQ_MAP		     0		/* enables printing eisa memory map */
#define	PDQ_CSR		     0		/* enables printing defea registers */
#define	PDQ_FYI		     0		/* enables printing defea registers */

/* EXTERNAL DECLARATIONS
*/
extern int null_procedure ();
extern do_bpt();		/* see CP:[COBRA_FW.REF]KERNEL_ALPHA.MAR */
extern struct HWRPB *hwrpb;
extern struct file *el_fp;

/* Callback definitions
*/
extern int cb_ref;                      /* Call backs flag */
extern int cb_fp[];

extern struct LOCK spl_kernel;

/* MARCO DEFINITIONS
/*   A macro routine to add a value to a quadword. The address of 
/*   the aligned quadword should be passed, along with the value.
/*   See kernel_alpha.mar for details.
/*
/*   Used to increment the quadword MOP counters
*/
#define _addq_v_u(x,y) addq_v_u(&(x),y)

#define malloc(x) dyn$_malloc(x,DYN$K_FLOOD|DYN$K_SYNC|DYN$K_NOOWN)
#define malloc_a(x,y) dyn$_malloc(x, DYN$K_FLOOD|DYN$K_SYNC|DYN$K_NOOWN|DYN$K_ALIGN,y,0)
#define free(x) dyn$_free(x,DYN$K_SYNC|DYN$K_NOOWN)

#define read_defpa_csr(x,y)  (x->pb.type==TYPE_EISA?inportl(x,(int)x->rp+y):inmeml(x,(int)x->rp+y))
#define write_defpa_csr(x,y,z)  (x->pb.type==TYPE_EISA?outportl(x,(int)x->rp+y,z):outmeml(x,(int)x->rp+y,z))


/* PROTOTYPE DECLARATIONS
*/ 
int pdq_write_csr (struct PDQ_PB *pdq, int csr, unsigned int data);
int pdq_read_csr (struct PDQ_PB *pdq, int csr);
int pdq_write_csr_byte (struct PDQ_PB *pdq, int csr, unsigned int data);
int pdq_read_csr_byte (struct PDQ_PB *pdq, int csr);
int pdq_write_mem (struct PDQ_PB *pdq, unsigned int *ep, int length, unsigned int *buf);
int pdq_read_mem (struct PDQ_PB *pdq, unsigned char *ep, int length, unsigned char *buf);

void pdq_dump_mem (struct PDQ_PB *pdq);
int pdq_close (struct FILE *fp);
int pdq_open (struct FILE *fp, char *info, char *next, char *mode);
int pdq_read (struct FILE *fp, int size, int number, unsigned char *rcv_buf);
int pdq_GetAdapterState (struct PDQ_PB *pdq);
int pdq_CheckXmtDataQueue (struct INODE *ip);
void pdq_FlushUnsolicited (struct INODE *ip);
int  pdq_PostXmtDataDescriptor (struct INODE *ip);
int  pdq_out (struct INODE *ip, char *xmt_buf, int frame_size, int SOBEOB);
int  pdq_write (struct FILE *fp, int item_size, int number, char *user_buf);
void pdq_ServiceRcvDataQueue (struct INODE *ip);
int  pdq_CheckRcvDataQueue (struct INODE *ip);
int  pdq_rx (struct INODE *ip);
void pdq_ServiceCmdRspQueue (struct INODE *ip);
void pdq_ServiceCmdReqQueue (struct INODE *ip);
void pdq_PostCmdReqDescriptor (struct INODE  *ip);
int  pdq_CheckCmdRspQueue (struct INODE *ip);
int  pdq_PollCmdRspQueue (struct INODE *ip);
int  pdq_PortControlCommand (struct PDQ_PB *pdq, unsigned int command);
int  pdq_StatusCharsGet (struct INODE  *ip);
int  pdq_CharsSet (struct INODE  *ip, int ItemCode, int value, int port);
int  pdq_CountersGet (struct INODE *ip);
int  pdq_Start (struct INODE *ip, int ms_wait);
int  pdq_DMA_Init (struct PDQ_PB *pdq);
int  pdq_softReset (struct PDQ_PB *pdq);
int  pdq_init_DBlock (struct PDQ_PB *pdq);
void pdq_init_XmtQueues (struct PDQ_PB *pdq);
void pdq_init_RcvQueues (struct PDQ_PB *pdq);

int  pdq_setmode (struct PDQ_PB *pdq, int mode);

int  pdq_init_dl (struct INODE *ip);
int  pdq_start_PDQ (struct INODE *ip);
int  pdq_malloc_descr (struct INODE *ip);
int  pdq_stop_driver (struct INODE *ip);
int  pdq_restart_driver (struct INODE *ip);
void pdq_control_t (struct NI_GBL *np);
void pdq_dump_csrs (struct NI_GBL *np);
int  pdq_reinit (struct FILE *pf);
int  pdq_init_pb (struct INODE *ip, struct PDQ_PB *pdq);
int  pdq_init ();
int  pdq_init1_pb ();

struct DDB pdq_ddb = {
	"pdq",			/* how this routine wants to be called	*/
	pdq_read,		/* read routine				*/
	pdq_write,		/* write routine			*/
	pdq_open,		/* open routine				*/
	pdq_close,		/* close routine			*/
	null_procedure,		/* name expansion routine		*/
	null_procedure,		/* delete routine			*/
	null_procedure,		/* create routine			*/
	null_procedure,		/* setmode				*/
	null_procedure,		/* validation routine			*/
	0,			/* class specific use			*/
	0,			/* allows information			*/
	0,			/* must be stacked			*/
	0,			/* is this a flash driver		*/
	0,			/* is a block device			*/
	0,			/* not seekable				*/
	2,			/* is an fddi device			*/
	0,			/* is a filesystem driver		*/
    };



/*+
 * ============================================================================
 * = pdq_close - Close this DEFEA 
 * ============================================================================
 *
 * OVERVIEW:       
 *      This routine closes the Eisa Port Driver.
 *
 * RETURNS:
 *	msg_success
 *       
 * ARGUMENTS:
 *	struct FILE *fp -   frx0" Port Driver File to close
 *	
-*/
int pdq_close (struct FILE *fp) {
#if PDQ_TRACE
    pprintf("\nEntering pdq_close %s\n", fp->ip->name);
#endif

    return msg_success;		/* Return a success */
    }


/*+
 * ============================================================================
 * = pdq_open - Open a DEFEA File Pointer
 * ============================================================================
 *
 * OVERVIEW:         
 *      Not used, program stub.
 *  
-*/
int pdq_open (struct FILE *fp, char *info, char *next, char *mode) {

#if PDQ_TRACE
    struct NI_GBL  *np;
    struct PDQ_PB  *pdq;
    int i;

    np  = (struct NI_GBL*)fp->ip->misc;	
    pdq = (struct PDQ_PB*)np->pbp;	

    pprintf("\nEntering pdq_open %s\n", fp->ip->name);
#endif

    return msg_success;
    }


/*+
 * ============================================================================
 * = pdq_read - DEFEA Port Read routine.                                          =
 * ============================================================================
 *
 * OVERVIEW:         
 *      Used Only for Callbacks.  pdq_msg_rcvd() normally returns FDDI packets
 *	to the ni_datalink where MOP processes them.  However
 *	when callbacks are enabled, ie cb_ref!=0, pdq_msg_rcvd() will instead 
 *	deposit all received FDDI packets into the ports pdq->rqh queue.  This 
 *	routine when called, removes the first packet queued and returns the 
 *	raw FDDI packet to its caller.
 *
 * RETURNS:
 *	Copies the first packets data bytes into the callers buffer pointer 
 *	and returns the number of bytes copied.
 *
 *      Returns zero if no packets are queued 
 *                          
 * ARGUMENTS:
 *	*fp      - Device File Pointer 
 *	size     - Used to test for buffer size.
 *	number   - Used to test for buffer size.
 *	*rcv_buf - For MOP load the base of where this is to be loaded.
 *
-*/
int pdq_read (struct FILE *fp, int size, int number, unsigned char *rcv_buf) {
    struct INODE   *ip;	
    struct NI_GBL  *np;
    struct PDQ_PB  *pdq;
    struct PDQ_MSG *mr;	

    int len, retries;
    int callback_timeout;

#if CALLBACK_TRACE
    pprintf ("pdq_read: size: %d number: %d %s\n", size, number, fp->ip->name);
#endif

    np  = (struct NI_GBL*)fp->ip->misc;		
    pdq = (struct PDQ_PB*)np->pbp;

    /* Wait for a packet to be received.  When received, remove it from the
    /* queue, copy it into the callers buffer pointer and return the number
    /* of bytes copied.
    */
    

    /* if this is a callback reference then lower timeout value to improve
     *  read performance at the OS level. This change was requested by OVMS.
     * PDQ_CALLBACK_TIMEOUT replace by callback_timeout.
     */

    if ( cb_ref )
       callback_timeout = 10;
    else
       callback_timeout = 100;

    for (retries=0; retries < callback_timeout; retries++)
    	{
        spinlock(&spl_kernel);		

    	if (pdq->rqh.flink != (struct PDQ_MSG*) &(pdq->rqh.flink))
    	    {
    	    mr = (struct PDQ_MSG *) remq(pdq->rqh.flink);
	    pdq->rcv_msg_cnt--;
            spinunlock(&spl_kernel);	
	    break;
	    }
        else
            spinunlock(&spl_kernel);	


    	/* Delay if no packets queued
    	*/
    	krn$_sleep (PDQ_CALLBACK_WAIT);
	}

    /* Return a byte count of zero if there were no packets queued
    */
    if (retries == callback_timeout)
    	return 0;

#if CALLBACK_TRACE
    pprintf("Callback message received\n");
#endif


    /* Copy the received packet from "mr->msg" to the callers "rcv_buf",
    /* and determine the number of bytes received/copied.
    */
    len = min(mr->size, size*number);
    memcpy(rcv_buf, mr->msg, len);

#if CALLBACK_TRACE
    pprint_pkt (rcv_buf, len);
#endif

    /* Deallocte the received message buffer, 
    /* and the message holder
    */
    ndl_deallocate_pkt(fp->ip, mr->msg);
    free(mr);

#if CALLBACK_TRACE
    pprintf("Exiting pdq_read\n");
#endif

    return len;
    }

 
/*+
 * ============================================================================
 * = pdq_write_mem - Eisa memory copy routine.                          =
 * ============================================================================
 *
 * OVERVIEW:
 *	The DEFEA has up to 64kbyte of on board memory.  This memory can be 
 *	used to map PDQ buffer descriptors and things.  This routine will copy 
 *	from main memory to this DEFEA 64kbyte eisa memory.
 *                    
 *	However the ECU can configure the DEFEA to have either 32k or 64k
 *	and at various beginning addresses, see pdq->emp, pdq->emp_size.
 *
 * RETURNS:
 *	Number of bytes transfered
 *
 * ARGUMENTS:
 *      *ep    - Eisa Memory address
 *	length - number of bytes to write
 *	*buf   - Buffer to be copied. 
 *
-*/
int pdq_write_mem (struct PDQ_PB *pdq, unsigned int *ep, int length, unsigned int *buf) {

    memcpy((unsigned int *)((int)ep & ~io_get_window_base(pdq)), buf, length);

    return length;
    }


/*+
 * ============================================================================
 * = pdq_read_mem - Eisa memory copy routine.                           =
 * ============================================================================
 *
 * OVERVIEW:
 *	The DEFEA and others, has 64kbyte of on board memory.  This memory
 *	can be used to map PDQ buffer descriptors and things.  This routine 
 *	will copy from this DEFEA 64kbyte eisa memory to main memory.
 *	This routine does a copy from eisa memory to main memory.
 *
 *	However the ECU can configure the DEFEA to have either 32k or 64k
 *	and at various beginning addresses, see pdq->emp, pdq->emp_size.
 *
 * RETURNS:
 *	Number of bytes read
 *
 * ARGUMENTS:
 *      *ep    - eisa memory adrs
 *	length - number of bytes to read
 *	*buf   - Buffer to be copied. 
 *
-*/
int pdq_read_mem (struct PDQ_PB *pdq, unsigned char *ep, int length, unsigned char *buf) {

    memcpy(buf, (unsigned char *)((int)ep & ~io_get_window_base(pdq)), length);

    return length;
    }


/*+
 * ============================================================================
 * =  pdq_PortControlCommand - pdq_PortControlCommand			      =
 * ============================================================================
 *
 * OVERVIEW:
 * 	This function issues the command specified in the command argument to
 * 	the PDQ Port Control register. This is accomplished by writing the
 * 	command into the port control register, and by polling for completion
 * 	for up to two seconds.
 *
 * ARGUMENTS:
 *	command -	Command to send the PDQ Controller
 *
-*/
int pdq_PortControlCommand (struct PDQ_PB *pdq, unsigned int command) {
    unsigned int
    	i,
    	reg;

#if PDQ_TRACE
    pprintf("\nEntering pdq_PortControlCommand %s\n", pdq->name);
#endif

    /* The Adaptor will clear the ERROR flag when the command has
    /* been successfully completed.
    */
    reg = command|PDQ_PCTRL_M_CMD_ERROR;	/* OR in the ERROR bit */

    pdq_write_csr(pdq, PDQ_PortControl, reg); 	/*Write the address register*/

    /* Wait a maximum of 2 Seconds for the command to be acknowledged.
    */
    for (i=0; i<2000; i++)
    	{
        krn$_sleep(1);

        reg = pdq_read_csr(pdq, PDQ_PortControl);

    	if (!(reg & PDQ_PCTRL_M_CMD_ERROR))
    	    {
    	    krn$_sleep(1);	
    	    return msg_success; 
    	    }
    	}
    
    fprintf(el_fp, msg_pdq_msg1, pdq->name);

    return msg_failure;
    };



#if PDQ_FYI

/*+
 * ============================================================================
 * =  pdq_GetSAhi - pdq_GetSAhi						      =
 * ============================================================================
 *
 * OVERVIEW:
 *	Each DEFEA, has a unique Network Station Address contained within.
 *	To get this Net_Adrs, the Host issues a Port Control Register 
 *	Command MLA_Read to the Port.  If successful the Port places the 
 *	High Longword of the Net_Adrs in the HOST_DATA_REGISTER.
 *
 *	A Net_Adrs is a structure that consists of two longwords.  
 *	The format of the structure is as follows.  
 *
 *			         sa0  sa1  sa2  sa3  sa4  sa5
 *	Example Network Address: 08 - 00 - 2B - AA - BB - CC
 *
 *		      LW1                           LW0
 *                          sa5  sa4      sa3    sa2    sa1  sa0
 *	    <31:24><23:16><15:8><7:0>   <31:24><23:16><15:8><7:0> 
 *	      xx    xx     CC    BB        AA    2B     00    08
 *
 *	Refer to the "PDQ FDDI Port Specification" for details.
 *
 * RETURNS:
 *	msg_success
 *  	msg_failure 
 *
 * ARGUMENTS:
 *	pdq
-*/
int pdq_GetSAhi (struct PDQ_PB *pdq) {

#if PDQ_TRACE
    pprintf("\nEntering pdq_GetSAhi %s\n", pdq->name);
#endif

    pdq_write_csr(pdq, PDQ_PortDataA, PDQ_PDATA_A_MLA_K_HI); 
    pdq_write_csr(pdq, PDQ_PortDataB, 0); 	
    return (pdq_PortControlCommand(pdq, PDQ_PCTRL_M_MLA));
    }
#endif


#if PDQ_FYI

/*+
 * ============================================================================
 * =  pdq_GetSAlo - pdq_GetSAlo						      =
 * ============================================================================
 *
 * OVERVIEW:
 *	This function issues a Port Control Register Command MLA_Read. 
 *	If successful, the DEFEA Low Longword Net_Station_Address is 
 *	placed in the HOST_DATA_REGISTER.
 *
 * RETURNS:
 *	msg_success
 *  	msg_failure 	
 *
 * ARGUMENTS:
 *	pdq	
-*/
int pdq_GetSAlo (struct PDQ_PB *pdq) {

#if PDQ_TRACE
    pprintf("\nEntering pdq_GetSAlo %s\n", pdq->name);
#endif

    pdq_write_csr(pdq, PDQ_PortDataA, PDQ_PDATA_A_MLA_K_LO); 	
    pdq_write_csr(pdq, PDQ_PortDataB, 0); 	
    return (pdq_PortControlCommand(pdq, PDQ_PCTRL_M_MLA));
    }
#endif


/*+
 * ============================================================================
 * =  pdq_GetAdapterState - pdq_GetAdapterState			 	      =
 * ===========================================================================
 *
 * OVERVIEW:
 *  This function reads the current adapter state from the PORT_STATUS register
 *  and returns it in the low byte.
 *
 * RETURNS:
 *
 * ARGUMENTS:
 *	pdq
 *
-*/
int pdq_GetAdapterState (struct PDQ_PB *pdq) {
   unsigned int
    	ps;

    ps = pdq_read_csr(pdq, PDQ_PortStatus);

#if ENABLE_PRINTF*0
    pprintf("\nPort Status: %08x %s\n", ps, pdq->name);
#endif

    return ((ps & PDQ_PSTATUS_M_STATE) >> PDQ_PSTATUS_V_STATE);
    };


#if PDQ_FYI

/*+
 * ============================================================================
 * =   pdq_GetXmtDescriptor - pdq_GetXmtDescriptor			      =
 * ============================================================================
 *
 * OVERVIEW:
 *	This function returns the address of the next available transmit data
 * 	descriptor. The next available descriptor is defined as the buffer 
 *	pointed to by the producer index. The producer index is used to index 
 *	into the descriptor table, and the address of the descriptor is 
 * 	returned in longword format suitable for casting into whatever pointer 
 *	is desired.
 *
 * RETURNS:
 *	msg_success or msg_failure 
 *
 * ARGUMENTS:
 *	none
-*/
int pdq_GetXmtDescriptor (struct INODE *ip) {
    struct NI_GBL      *np;	/* Pointer to the NI global database*/
    struct PDQ_PB      *pdq;	/* Pointer to the port block*/	
    PDQ_PRODUCER_INDEX *pi;	/* Producer Index block*/	
    unsigned int lwbuf;

#if PDQ_TRACE
    pprintf("\nEntering pdq_GetXmtDescriptor %s\n", ip->name);
#endif

    np  = (struct NI_GBL*)      ip->misc;
    pdq = (struct PDQ_PB*)      np->pbp;
    pi  = (PDQ_PRODUCER_INDEX*) pdq->pip;

    return ((int)(&pdq->emp->db.xmt_data[pi->XmtDataProducer]));	
    }
#endif


#if PDQ_FYI

/*+
 * ============================================================================
 * =   pdq_CheckUnsolQueue - pdq_CheckUnsolQueue				      =
 * ============================================================================
 *
 * OVERVIEW:
 *	This function checks the Unsolicited Queue.  It returns msg_success
 *	if there is work to do on the queue the completion index != consumer 
 *	index).  This function returns msg_failure if there is no work to do 
 *	on the queue.
 *
 * RETURNS:
 *	msg_success or msg_failure 
 *
 * ARGUMENTS:
 *	none
-*/
int pdq_CheckUnsolQueue (struct INODE  *ip) {
    struct NI_GBL 	*np;	/* Pointer to the NI global database*/
    struct PDQ_PB       *pdq;	/* Pointer to the port block*/
    PDQ_PRODUCER_INDEX  *pi;	/* Producer Index block*/	

    unsigned int lwbuf;
    unsigned int  
    	PortConsumer;

    np  = (struct NI_GBL*) 	ip->misc;
    pdq = (struct PDQ_PB*)  	np->pbp;
    pi  = (PDQ_PRODUCER_INDEX*) pdq->pip;

    /* Extract the Ports Unsolicited Consumer index
    */
    pdq_read_mem(pdq, &pdq->emp->cb.unsol, sizeof(PDQ_ULONG), &lwbuf);

    PortConsumer = lwbuf & PDQ_CONS_M_RCV_INDEX;

    /* The Port has been sending the Host Messages and...
    /*    There is work to be done on the Queue when...
    /*        Ports_Consumer_Index != Hosts_Completion_Index 
    */
    return (PortConsumer != pi->UnsolCompletionIndex);
    }
#endif


/*+
 * ============================================================================
 * =   pdq_CheckXmtDataQueue - pdq_CheckXmtDataQueue			      =
 * ============================================================================
 *
 * OVERVIEW:
 *	We are going to transmit ONE Descriptor at a time, so we must first
 * 	ensure the last descriptor Produced by the HOST was Consummed by the 
 *	PORT.
 *
 *	This function checks the Transmit Data Queue.  Returns msg_success if 
 *	the posted frames have been transmitted. i.e. the producer = the 
 *	consumer. Returns msg_faulure otherwise. 
 *
 * RETURNS:
 *	msg_success or msg_failure 
 *
 * ARGUMENTS:
 *	none
-*/
int pdq_CheckXmtDataQueue (struct INODE *ip)
    {
    struct NI_GBL 	*np;	/* Pointer to the NI global database*/
    struct PDQ_PB       *pdq;	/* Pointer to the port block*/
    PDQ_PRODUCER_INDEX  *pi;	/* Producer Index block*/	

    unsigned int lwbuf;
    unsigned int  
    	PortConsumer;

#if PDQ_TRACE
    pprintf("\nEntering pdq_CheckXmtDataQueue %s\n", ip->name);
#endif

    np  = (struct NI_GBL*) 	ip->misc;
    pdq = (struct PDQ_PB*)  	np->pbp;
    pi  = (PDQ_PRODUCER_INDEX*) pdq->pip;

    /* extract the Ports Xmt Consumer index...
    */
    pdq_read_mem(pdq, &pdq->emp->cb.xmt_rcv_data, sizeof(PDQ_ULONG), &lwbuf);

    PortConsumer = (lwbuf & PDQ_CONS_M_XMT_INDEX)>>16;

    /* The PRODUCERS Xmt Descriptor...
    /*     has been CONSUMED by the Port when the...
    /*		Ports_Consumer_Index == Host_Producer_Index 
    */
    return (PortConsumer == pi->XmtDataProducer);
    }



#if PDQ_FYI

/*+
 * ============================================================================
 * =   pdq_CheckCmdReqQueue - pdq_CheckCmdReqQueue			      =
 * ============================================================================
 *
 * OVERVIEW:
 *	This function checks the Command Request Queue.  The  function returns 
 *	msg_success if the posted frames have been transmitted, ie the 
 *	producer = the consumer.  Otherwise the function returns msg_failure.
 *
 * RETURNS:
 *	msg_success or msg_failure 
 *
 * ARGUMENTS:
 *	struct INODE *ip - The Inode of Port Driver Calling this routine
-*/
int pdq_CheckCmdReqQueue (struct INODE *ip) {
    struct NI_GBL 	*np;	/* Pointer to the NI global database*/
    struct PDQ_PB       *pdq;	/* Pointer to the port block*/
    PDQ_PRODUCER_INDEX  *pi;	/* Producer Index block*/	
    unsigned int lwbuf;
    unsigned int  
    	PortConsumer;

#if PDQ_TRACE
    pprintf("\nEntering pdq_CheckCmdReqQueue %s\n", ip->name);
#endif

    np  = (struct NI_GBL*) 	ip->misc;
    pdq = (struct PDQ_PB*)  	np->pbp;
    pi  = (PDQ_PRODUCER_INDEX*) pdq->pip;

    pdq_read_mem(pdq, &pdq->emp->cb.cmd_req, sizeof(PDQ_ULONG), &lwbuf);

    PortConsumer = lwbuf & PDQ_CONS_M_RCV_INDEX;

    /* The PRODUCERS xmt Descriptor...
    /*     has been CONSUMED by the Port when the...
    /*		Ports_Consumer_Index == Host_Producer_Index 
    */
    return (PortConsumer == pi->CmdReqProducer);
    }
#endif


#if PDQ_FYI

/*+
 * ============================================================================
 * =   pdq_CheckSMTRcvQueue - pdq_CheckSMTRcvQueue			      =
 * ============================================================================
 *
 * OVERVIEW:
 * 	This function checks the SMT Receive Queue.  This function returns 
 *	msg_success if there is work to do on the queue (ie the completion 
 *	index != consumer index).  This function returns msg_failure if there 
 *	is no work to do on the queue.
 *
 * RETURNS:
 *	msg_success or msg_failure 
 *
 * ARGUMENTS:
 *	struct INODE *ip - The Inode of Port Driver Calling this routine
-*/
int pdq_CheckSMTRcvQueue (struct INODE *ip) {
    struct NI_GBL 	*np;	/* Pointer to the NI global database*/
    struct PDQ_PB       *pdq;	/*Pointer to the port block*/
    PDQ_PRODUCER_INDEX  *pi;	/* Producer Index block*/	
    unsigned int  
    	PortConsumer;
    unsigned int lwbuf;

#if PDQ_TRACE
    pprintf("\nEntering pdq_CheckSMTRcvQueue %s\n", ip->name);
#endif

    np  = (struct NI_GBL*) 	ip->misc;
    pdq = (struct PDQ_PB*)  	np->pbp;
    pi  = (PDQ_PRODUCER_INDEX*) pdq->pip;

    /* Extract the Ports SMT Consumer index
    */
    pdq_read_mem(pdq, &pdq->emp->cb.smt_host, sizeof(PDQ_ULONG), &lwbuf);

    PortConsumer = lwbuf & PDQ_CONS_M_RCV_INDEX;

    /* The Port has been sending the Host Messages and...
    /*    There is work to be done on the Queue when...
    /*        Ports_Consumer_Index != Hosts_Completion_Index 
    */
    return (PortConsumer != pi->SMTCompletionIndex);
    }
#endif


/*+
 * ============================================================================
 * =   pdq_FlushUnsolicited - pdq_FlushUnsolicited			      =
 * ============================================================================
 *
 * OVERVIEW:
 * 	This function checks for any frames on the Unsolicited event queue, 
 *	and discards them.
 *
 * RETURNS:
 *	msg_success or msg_failure 
 *
 * ARGUMENTS:
 *	struct INODE *ip - The Inode of Port Driver Calling this routine
-*/
void pdq_FlushUnsolicited (struct INODE *ip) {

    struct NI_GBL 	*np;	/* Pointer to the NI global database*/
    struct PDQ_PB       *pdq;	/* Pointer to the port block*/
    PDQ_PRODUCER_INDEX  *pi;	/* Producer Index block*/	

    unsigned int 
    	comp, 			/* Completion index */
    	prod;			/* Producer index */

    unsigned int 
    	lwbuf,
    	PortConsumer;

    np  = (struct NI_GBL*) 	ip->misc;
    pdq = (struct PDQ_PB*)  	np->pbp;
    pi  = (PDQ_PRODUCER_INDEX*) pdq->pip;

    /* Extract the Ports Unsolicited Consumer index
    */
    pdq_read_mem(pdq, &pdq->emp->cb.unsol, sizeof(PDQ_ULONG), &lwbuf);

    PortConsumer = lwbuf & PDQ_CONS_M_RCV_INDEX;

    /* Throw Unsolicited frames on the floor too 
    */

    /* The Port has been sending the Host Messages and...
    /*    There is work to be done on the Queue when...
    /*        Ports_Consumer_Index != Hosts_Completion_Index 
    */
    if (PortConsumer != pi->UnsolCompletionIndex)
    	{
    	/*
    	*  This block services the Unsolicited Queue. This simply means 
    	*  advancing the completion index to equal the consumer, and advancing
    	*  the producer index to one short of the completion index. This creates
    	*  the ongoing illusion of an always available reveice buffer on the
    	*  unsolicited event queue. This is okay for this application, since we
    	*  are simply trowing Unsolicited events on the floor. This queue does,
    	*  however, have to be serviced, since ignoring it could cause the adapter
    	*  to transition to the halted state. 
    	*/
        /* The Unsolicited Queue has 16 Descriptor.  This Service routine
        /* will always keep 16 of this descriptors produced.  ie the producer
        /* index will always be one less than the Consumer index.
        */
        pdq_read_mem(pdq, &pdq->emp->cb.unsol, sizeof(PDQ_ULONG), &lwbuf);

        pi->UnsolCompletionIndex = lwbuf & PDQ_CONS_M_RCV_INDEX;
        prod = (pi->UnsolCompletionIndex - 1) & 0x000F;	/* keep 16 produced */

        pi->UnsolProducer = prod;

        comp = pdq_read_csr(pdq, PDQ_UnsolProd);

        comp &= ~PDQ_TYPE_1_PROD_M_COMP;	/* clear Completion fields */
        comp &= ~PDQ_TYPE_1_PROD_M_PROD;	/* clear Producer   fields */

        /* Create a new COMPLETION and PRODUCER indexes
        */
        comp |= pi->UnsolCompletionIndex << PDQ_TYPE_1_PROD_V_COMP;
        comp |= prod                     << PDQ_TYPE_1_PROD_V_PROD;

        /* Stuff it into the Adapters register
        */
        pdq_write_csr(pdq, PDQ_UnsolProd, comp); 	
    	}

    }



/*+
 * ============================================================================
 * =  pdq_PostXmtDataDescriptor - pdq_PostXmtDataDescriptor		      = 
 * ============================================================================
 *
 * OVERVIEW:
 *	This function advances the Xmt Data Producer index by one.
 *	It uses the host copy of the Producers and Completion 
 * 	indicees.
 *
 * ARGUMENTS:
 *	struct INODE *ip - The Inode of Port Driver Calling this routine
 *
-*/
void pdq_PostXmtDataDescriptor (struct INODE *ip) {
    struct NI_GBL      *np;	/* Pointer to the NI global database*/
    struct PDQ_PB      *pdq;	/* Pointer to the port block*/	
    PDQ_PRODUCER_INDEX *pi;	/* Producer Index block*/	

    unsigned int
    	reg=0;

#if PDQ_TRACE
    pprintf("\nEntering pdq_PostXmtDataDescriptor %s\n", ip->name);
#endif

    np  = (struct NI_GBL*)      ip->misc;
    pdq = (struct PDQ_PB*)      np->pbp;
    pi  = (PDQ_PRODUCER_INDEX*) pdq->pip;

    /* Using our local Host memory Producer index copy, PRODUCE one
    /* descriptor.  After doing so TRUNCATE the Producer index count by
    /* ALWAYS forcing the index relative to zero when it gets greater 
    /* then 255 decimal. This has an effect of wrapping the indexes to
    /* the beginning of the queue, ie [0].  
    */
    pi->XmtDataProducer++;		/* advance the producer index */
    pi->XmtDataProducer &= 0x00FF;	/* index range of <255:0> */

    /*	 31	 2423	1615	  87	0
    /*	+---------------------------------+
    /*  |xmtData  |RvdData|XmtData|RcvData|
    /*  |Complet  |Complet|Produce|Produce|
    /*  +---------+-------+-------+-------+
    */
    reg |= pi->XmtDataCompletionIndex << PDQ_TYPE_2_PROD_V_XMT_DATA_COMP; /*<<24*/ 
    reg |= pi->XmtDataProducer        << PDQ_TYPE_2_PROD_V_XMT_DATA_PROD; /*<<8*/
    reg |= pi->RcvDataCompletionIndex << PDQ_TYPE_2_PROD_V_RCV_DATA_COMP; /*<<16*/
    reg |= pi->RcvDataProducer        << PDQ_TYPE_2_PROD_V_RCV_DATA_PROD; /*<<0*/

#if PDQ_SERVICE_POST 
    pprintf("\n%s Post:    XMT DATA PRODUCER: %08x\n", ip->name, reg);
#endif

    pdq_write_csr(pdq, PDQ_Type2Prod, reg);
    }



/*+
 * ============================================================================
 * = pdq_out - Message out routine 				      =
 * ============================================================================
 *
 * OVERVIEW:
 * This routine transmits a frame to the adapter. A pointer to the transmit 
 * buffer, and it's size are passed as arguments. This routine 
 * encapsulates the buffer into an FDDI frame, and posts a buffer descriptor to 
 * the transmit data queue. If successful, the number of bytes transmitted is 
 * returned. If unsuccessful, a zero value is returned.
 *
 * In this function, the caller is supplying a buffer in IEEE format. This 
 * routine performs the IEEE to FDDI translation described in the comments 
 * from pdq_read, only in reverse.
 *
 * RETURNS:
 *	int Status - msg_success or FAILURE.
 *
 * ARGUMENTS:
 *	struct INODE *ip - The Inode of Port Driver Calling this routine
 *
-*/
int pdq_out (struct INODE *ip, char *xmt_buf, int frame_size, int SOBEOB) {
    PDQ_XMT_DESCR        xmt_descriptor; /* Xmit Desc for frame sent on fiber */
    PDQ_XMT_DATA_BUFFER  *fddi_xmt_frame;	  /* Frame that gets sent on the fiber */
    struct MOP$_V4_COUNTERS *mc;	
    struct NI_GBL 	 *np;		  /* Pointer to the NI global database*/
    struct PDQ_PB        *pdq;		  /* Pointer to the port block*/
    PDQ_PRODUCER_INDEX   *pi;	/* Producer Index block*/	

    IEEE_FRAME	 	 *ieee_frame;	  /* Frame passed in from caller via *buffer */
    ETHERNET_FRAME	 *enet_frame;	  


    unsigned int
    	lwbuf,
    	reg;

    unsigned char FrameFormat;

    unsigned int *lw_data;

    unsigned int
    	addrs,
    	timeout=2000,
    	status,
    	i;

#if PDQ_TRACE
    pprintf("\nEntering pdq_out %s\n", ip->name);
#endif

    /* Get the pointer to the port block
    */
    np  = (struct NI_GBL*)ip->misc;
    pdq = (struct PDQ_PB*)np->pbp;
    pi  = (PDQ_PRODUCER_INDEX*) pdq->pip;
    mc  = (struct MOP$_V4_COUNTERS*)np->mcp;	

    /* Return error if adapter is not in the LinkAvailable state 
    */
    if ((status = pdq_GetAdapterState(pdq)) != PDQ_STATE_K_LINK_AVAIL) 
    	{
    	fprintf(el_fp, msg_pdq_msg2, ip->name, status);
    	return 0;	/* Return zero bytes written */
    	}

    /* Since we are in LinkAvailable, this is as good a time 
    /* as any to service the Unsolicited queue.
    */
    pdq_FlushUnsolicited(ip);	

    /* Timeout the Port CONSUMING the last descriptor transmitted.
    /* Return with error if the Adapter has not yet CONSUMED the last
    /* Descriptor PRODUCED.
    */
    if (!pdq_CheckXmtDataQueue(ip)) 
        while (timeout--)
    	    {
    	    krn$_sleep(100);		

            if (pdq_CheckXmtDataQueue(ip)) 
    		break;
    	    else
    		if (timeout==0)
    		   {
    		   fprintf(el_fp, msg_pdq_msg3, ip->name);
    		   return 0;		/* Return zero bytes written */
    		   }
    	    }

    /* Each DEFEA Process, has malloc'ed a 
    /*		(4096, DYN$K_SYNC|DYN$K_NOOWN|DYN$K_FLOOD);
    /*
    /* The Transmit buffer is saved in the Port Block.
    /* Get the address of this transmmit buffer.  This implementation
    /* only supports a SINGLE transmit and receive buffer, and its
    /* address is saved in the port block, see pdq_init_DBlock().    
    */
    fddi_xmt_frame = (PDQ_XMT_DATA_BUFFER *)pdq->XmtDatBlock; /* FOREVER ALLOCATED */
    enet_frame = (ETHERNET_FRAME*)xmt_buf;
    ieee_frame = (IEEE_FRAME*)xmt_buf;

    if (((enet_frame->protocol_type_0 << 8) | (enet_frame->protocol_type_1))
    		> 1500)
    	FrameFormat = TRUE;	/* Ethernet Frame format */
    else
    	FrameFormat = FALSE;	/* IEEE Frame format */


    /* Munge the ENET/IEEE frame into an FDDI frame...
    /* - Get the DA, protocol type (PID3 and PID4), and DATA from the 
    /*   Caller's ENET/IEEE buffer. 
    /*   We stuff the PRH, FC, SA, SSAP, DSAP, CNTL, and PID0-2
    */

    /* FDDI Packet Request Header, ENET/IEEE generic fields
    */
    fddi_xmt_frame->prh_0 = 0x20;	/* Unrestricted token use */
    fddi_xmt_frame->prh_1 = 0x28;	/* Unrestricted token release and */
        				/*   Append CRC to end of fram */
    fddi_xmt_frame->prh_2 = 0x00;	/* mbz */

    if (FrameFormat)
        fddi_xmt_frame->fc    = 0x50;	/* Mapped Ethernet */
    else
        fddi_xmt_frame->fc    = 0x54;	/* */


    if (FrameFormat)
    	{
    	/* Ethernet frame format
    	*/
    	memcpy(fddi_xmt_frame->source_addr, enet_frame->source_addr, 6); /* Supply the source address */

        if (pdq->loopback)
            memcpy(fddi_xmt_frame->dest_addr,   enet_frame->source_addr, 6); /* Get dest address from caller frame */
 	else
            memcpy(fddi_xmt_frame->dest_addr,   enet_frame->dest_addr,   6); /* Get dest address from caller frame */
    	}
    else
    	{
    	/* IEEE frame format
    	*/
    	memcpy(fddi_xmt_frame->source_addr, ieee_frame->source_addr, 6); /* Supply the source address */

        if (pdq->loopback)
            memcpy(fddi_xmt_frame->dest_addr,   ieee_frame->source_addr, 6); /* Get dest address from caller frame */
        else
            memcpy(fddi_xmt_frame->dest_addr,   ieee_frame->dest_addr,   6); /* Get dest address from caller frame */
    	}


    if (FrameFormat)
    	{
    	/* Ethernet frame format
    	*/
        fddi_xmt_frame->dsap = 0xaa;
        fddi_xmt_frame->ssap = 0xaa;
        fddi_xmt_frame->cntr = 0x03;

        fddi_xmt_frame->pid_0 = 0;
        fddi_xmt_frame->pid_1 = 0;
        fddi_xmt_frame->pid_2 = 0;
        fddi_xmt_frame->pid_3 = enet_frame->protocol_type_0;
        fddi_xmt_frame->pid_4 = enet_frame->protocol_type_1;
    	}
    else
    	{
    	/* IEEE frame format
    	*/
        fddi_xmt_frame->dsap = ieee_frame->dsap;
        fddi_xmt_frame->ssap = ieee_frame->ssap;
        fddi_xmt_frame->cntr = ieee_frame->cntr;

        fddi_xmt_frame->pid_0 = ieee_frame->pid_0;
        fddi_xmt_frame->pid_1 = ieee_frame->pid_1;
        fddi_xmt_frame->pid_2 = ieee_frame->pid_2;
        fddi_xmt_frame->pid_3 = ieee_frame->pid_3;
        fddi_xmt_frame->pid_4 = ieee_frame->pid_4;
    	}


    /* The total length in bytes of this frame is, MAC+LLC+DATA ie "frame_size".
    /* The data_size of this frame is therefore, 
    /* 	data_size = frame_size - (MAC+LLC)
    */
    if (FrameFormat)
        memcpy(fddi_xmt_frame->data, enet_frame->data, (frame_size-14));
    else
        memcpy(fddi_xmt_frame->data, ieee_frame->data, (frame_size-22));

    if (FrameFormat)
        pdq_write_mem(pdq, &pdq->emp->XmtBuf, (frame_size+10), fddi_xmt_frame);
    else
        pdq_write_mem(pdq, &pdq->emp->XmtBuf, (frame_size+2), fddi_xmt_frame);


#if ENABLE_FDDI_XMT

    pprintf("\nTRANSMIT: FDDI FRAME FORMAT %s\n", ip->name);

    pprintf("XMT fddi_xmt_frame_size: %d\n", frame_size+2);

    pprintf("fddi_xmt_frame->prh_0: %x\n",  fddi_xmt_frame->prh_0);	/* Unrestricted token use */
    pprintf("fddi_xmt_frame->prh_1: %x\n",  fddi_xmt_frame->prh_1);	/* Unrestricted token release and */
    pprintf("\n");
    pprintf("fddi_xmt_frame->prh_2: %x\n",  fddi_xmt_frame->prh_2);	/* mbz */

    pprintf("fddi_xmt_frame->fc: %x\n",     fddi_xmt_frame->fc);		/* FC = 0x54  LLC */

    for (i=0; i<6; i++)
    	pprintf("fddi_xmt_dest_addr[%d]: %x\n", i, fddi_xmt_frame->dest_addr[i]);

    pprintf("\n");

    for (i=0; i<6; i++)
    	pprintf("fddi_xmt_source_addr[%d]: %x\n", i, fddi_xmt_frame->source_addr[i]);

    pprintf("\n");

    pprintf("fddi_xmt_frame->dsap: %x\n",   fddi_xmt_frame->dsap);
    pprintf("fddi_xmt_frame->ssap: %x\n",   fddi_xmt_frame->ssap);
    pprintf("fddi_xmt_frame->cntr: %x\n\n", fddi_xmt_frame->cntr);
    pprintf("\n");
    pprintf("fddi_xmt_frame->pid_0: %x\n",   fddi_xmt_frame->pid_0);
    pprintf("fddi_xmt_frame->pid_1: %x\n",   fddi_xmt_frame->pid_1);
    pprintf("fddi_xmt_frame->pid_2: %x\n",   fddi_xmt_frame->pid_2);
    pprintf("fddi_xmt_frame->pid_3: %x\n",   fddi_xmt_frame->pid_3);
    pprintf("fddi_xmt_frame->pid_4: %x\n\n", fddi_xmt_frame->pid_4);

    lw_data = fddi_xmt_frame->data;

    if (FrameFormat)
        for (i=0; i<4; i++)
    	    pprintf("fddi_xmt_frame->data[%d] = %x\n", i, lw_data[i]);
    else
        for (i=0; i<4; i++)
    	    pprintf("fddi_xmt_frame->data[%d] = %x\n", i, lw_data[i]);

#endif


#if ENABLE_ENET_IEEE_XMT
    if (FrameFormat)
    	{
	spinlock(&spl_kernel);
	pprintf("\nTRANSMIT ETHERNET FRAME FORMAT %s\n", ip->name);

	for (i=0; i<6; i++)
	    pprintf("enet_xmt_dest_addr[%d]: %x\n", i, enet_frame->dest_addr[i]);

	pprintf("\n");

	for (i=0; i<6; i++)
	    pprintf("enet_xmt_source_addr[%d]: %x\n", i, enet_frame->source_addr[i]);

	pprintf("\n");

	pprintf("enet_xmt_frame->protocol_type_0: %x\n", enet_frame->protocol_type_0);
	pprintf("enet_xmt_frame->protocol_type_1: %x\n", enet_frame->protocol_type_1);

	pprintf("\n");

    	lw_data = enet_frame->data;

	for (i=0; i<4; i++)
    	    pprintf("enet_xmt_frame->data[%d] = %x\n", i, lw_data[i]);

	spinunlock(&spl_kernel);
    	}
    else
    	{
	spinlock(&spl_kernel);
	pprintf("\nTRANSMIT IEEE FRAME FORMAT %s\n", ip->name);

	for (i=0; i<6; i++)
	    pprintf("ieee_xmt_dest_addr[%d]: %x\n", i, ieee_frame->dest_addr[i]);

	pprintf("\n");

	for (i=0; i<6; i++)
	    pprintf("ieee_xmt_source_addr[%d]: %x\n", i, ieee_frame->source_addr[i]);

	pprintf("\n");

    	pprintf("ieee_xmt_frame->len[0]: %x\n", ieee_frame->len[0]);
    	pprintf("ieee_xmt_frame->len[1]: %x\n", ieee_frame->len[1]);

    	pprintf("ieee_xmt_frame->dsap: %x\n", ieee_frame->dsap);
    	pprintf("ieee_xmt_frame->ssap: %x\n", ieee_frame->ssap);
    	pprintf("ieee_xmt_frame->cntr: %x\n", ieee_frame->cntr);

    	pprintf("\n");

    	pprintf("ieee_xmt_frame->pid_0: %x\n", ieee_frame->pid_0);
    	pprintf("ieee_xmt_frame->pid_1: %x\n", ieee_frame->pid_1);
    	pprintf("ieee_xmt_frame->pid_2: %x\n", ieee_frame->pid_2);
    	pprintf("ieee_xmt_frame->pid_3: %x\n", ieee_frame->pid_3);
    	pprintf("ieee_xmt_frame->pid_4: %x\n", ieee_frame->pid_4);

    	pprintf("\n");

    	lw_data = ieee_frame->data;

    	for (i=0; i<4; i++)
	    pprintf("ieee_xmt_frame->data[%d] = %x\n", i, lw_data[i]);

    	spinunlock(&spl_kernel);
    	}
#endif




    /* Fill in this "XMT DESCRIPTOR BLOCK" with the newly created FDDI frame...
    /* - An FDDI Header is 2 bytes greater than an IEEE header, ie (frame_size+2)
    /* - SOB and EOB are determined in pdq_write 
    /* - xmt_buf is the IEEE frame, munged into an FDDI frame 
    /*
    /* The PRODUCE the descriptor thus giving it to the Adapter.
    */
    if (FrameFormat)
        xmt_descriptor.long_1   = (frame_size + 10) << PDQ_XMT_DESCR_V_SEG_LEN;
    else
    	xmt_descriptor.long_1   = (frame_size + 2) << PDQ_XMT_DESCR_V_SEG_LEN;

    xmt_descriptor.long_1  |= SOBEOB;
    xmt_descriptor.buff_lo  = &pdq->emp->XmtBuf;

    pdq_write_mem(pdq, (int)(&pdq->emp->db.xmt_data[pi->XmtDataProducer]),
    		sizeof(PDQ_XMT_DESCR), &xmt_descriptor);

    pdq_PostXmtDataDescriptor(ip);       /* Transmit the frame */

    /* POLLED IO:
    /* ----------
    /*	Poll the XmtDataQueue for this message transmission.  If required
    /*  we'll speed up this drive by making it interrupt driven.  But for
    /*  a simple polled IO will do.
    */
    for (i=0; i<1000; i++)
    	{
    	pdq_FlushUnsolicited(ip);		/* Keep Unsolicited frames flushed */

	/* When the frame is sent...
    	/*	Service the Xmt Data Queue and return bytes transmitted
    	*/
    	if (pdq_CheckXmtDataQueue(ip))
    	    {
    	    /*
    	    /* This block services the Xmt Data Queue by simply advancing the 
    	    /* completion index to equal the consumer.
    	    */

            mc->MOP_V4CNT_TX_BYTES += frame_size;
            mc->MOP_V4CNT_TX_FRAMES++;

    	    pdq_read_mem(pdq, &pdq->emp->cb.xmt_rcv_data, sizeof(PDQ_ULONG), &lwbuf);

    	    pi->XmtDataCompletionIndex = (lwbuf & PDQ_CONS_M_XMT_INDEX) >> 16;

    	    reg = pdq_read_csr(pdq, PDQ_Type2Prod);

    	    reg &= ~PDQ_TYPE_2_PROD_M_XMT_DATA_COMP;
    	    reg |=  pi->XmtDataCompletionIndex << PDQ_TYPE_2_PROD_V_XMT_DATA_COMP;

#if PDQ_SERVICE_POST 
    	    pprintf("\n%s Service: XMT DATA PRODUCER: %08x\n", ip->name, reg);
#endif

    	    pdq_write_csr(pdq, PDQ_Type2Prod, reg); 	

    	    return frame_size;		/* and return the size as a success indication */
    	    }

    	krn$_sleep(1);		/* return to console if not sent */
    	}

    /* Need to return with error if fell out of for-loop.
    /* return a byte count of zero to indicate the error.
    */
    pdq_FlushUnsolicited(ip);		/* Keep Unsolicited frames flushed */
    return 0;				/* return zero bytes written */
    }




/*+
 * ============================================================================
 * = pdq_callback_write - DEFEA callback write routine.                        =
 * ============================================================================
 *
 * OVERVIEW:
 *	This routine does an ASYNCHRONOUS callback write.
 *
 * RETURNS:
 *	int Status - msg_success or FAILURE.
 *
 * ARGUMENTS:
 *      *fp       - Pointer to the file to be written.
 *	item_size - size of item
 *	number    - number of items
 *	*XmtBuf   - Buffer to be Transmitted
 *
-*/
int pdq_callback_write (struct FILE *fp,	int item_size, int number, char *XmtBuf) {
    struct NI_GBL   *np;		
    struct PDQ_PB   *pdq;		
    struct TIMERQ   *ctt;		

    unsigned char   *ndl_XmtBuf;	/* ndl_allocate Buffer pointer */

    int 
    	i,
    	size,
    	status,
    	tx_index_in,
    	len, 
    	return_size;

#if PDQ_TRACE
    pprintf("\nEntering pdq_callback_write %s\n", fp->ip->name);
#endif

    /* Get Ports Pointer
    */
    np  = (struct NI_GBL*)  fp->ip->misc;
    pdq = (struct DE422_PB*)np->pbp;

    /* Return if the driver is stopped
    */
    if ((pdq->pb.state == PDQ$K_STOPPED) || (pdq->pb.state == PDQ$K_UNINITIALIZED))
    	return(0);

    /* Copy our Station Address into first 5 bytes of the
    /* buffer to be transmitted.
    */
    for (i=0; i<6; i++)
    	XmtBuf[i+6] = pdq->sa[i];

#if CALLBACK_PRINTF
    pprintf("Callback message written\n");
    pprint_pkt(XmtBuf, item_size*number);
#endif

    len = item_size*number;	/* Total bytes to transmit */

    /* We need to always transmit a MINIMAL size Packet.
    /*   If the Packet is less then the MINIMAL size Packet,
    /*   then pad the packet with zeroes making it of MINIMAL size.
    */
    if (len < PDQ_K_MIN_PKT_SIZE)
    	{
    	ndl_XmtBuf = ndl_allocate_pkt(fp->ip, 0);

    	memcpy(ndl_XmtBuf, XmtBuf, len);

    	ndl_deallocate_pkt(fp->ip, XmtBuf);

    	while (len < PDQ_K_MIN_PKT_SIZE)
    	    ndl_XmtBuf[len++] = 0;
	}
    else
    	ndl_XmtBuf = XmtBuf;


    spinlock(&(pdq->spl_port));		

    size = 
	pdq_out(fp->ip, ndl_XmtBuf, (item_size*number), PDQ_K_SOB|PDQ_K_EOB);

    spinunlock(&(pdq->spl_port));	

    return (size);
    }


/*+
 * ============================================================================
 * = pdq_write - Driver write routine.                                         =
 * ============================================================================
 *
 * OVERVIEW:
 * 	This routine deposits frames into the XmtDataQueue.  A pointer to the 
 *	transmit buffer, and it's size are passed as arguments. This 
 *	routine encapsulates the buffer in an FDDI frame, and posts a buffer 
 * 	descriptor to the transmit data queue. 
 *
 * 	In this function, the caller is supplying a buffer in IEEE format. 
 *	This routine performs the IEEE to FDDI translation described in 
 *	the comments above for fi_read, only in reverse.
 *
 * CALLED FROM:
 *
 *  fopen("mopdl:foo.sys/fra0", "r+")	! MOPDL File Descriptor
 * 	mopdl_open (fp="fra0")		! FDDI  File Descriptor
 *  fseek(fp)
 *  fread(fp)
 * 	mopdl_read (fp="mopdl")
 * 	    mop_send_req_program (nfp="fra0", MOP$K_SYSTEM_IMAGE)
 *	    mop_send_req_program (nfp="fra0", MOP$K_TERTIARY_LOADER)
 *	    mop_send_req_mem_load(nfp="fra0")	
 *	        ndl_send(nfp="fra")
 *	            fwrite_nolock(buffer, size, number, nfp="fra0"); 
 *		        !
 *		        ! Deposits MOP messages packets into the transmit queue
 *		        !
 *			pdq_write(nfp="fra", size, number, buffer)
 *
 *		    pdq_rx()	   !krn$_waits() for messges received,
 *				   !process the received message for errors
 *				   !and gives the message to upper protocol
 *				   !or to pdq_read() if callback enabled.
 *
 *			+---------------+
 *      user_buf->gen->	|  IEEE header	|	Filled by NI_DATALINK.C
 *			|		|
 *			+---------------+
 *			|		|	
 *			|		|	MOP contents filled by:
 *			|   MOP Info	|	    MOPDL_DRIVER.C
 *			|		|	    MOPLB_DRIVER.C
 *			|		|	    MOPRC_DRIVER.C
 *			|		|
 *			|		|
 *			|		|
 *			+---------------+
 *
 * MAXIMUM PACKET SIZE:
 * 	! Header, data, and CRC, no preamble. 
 *	!
 *	ES$K_MAX_PKT_SIZE == NDL$K_MAX_PACK_LENGTH == 1518      
 *
 * Ethernet and IEEE 808.3 lengths.                    
 *   NDL$K_MAX_DATA_LENGTH 1500      !Data without CRC.
 *   NDL$K_MAX_PACK_LENGTH 1518      !Header, data, and CRC, no preamble. 
 *   NDL$K_MIN_PKT_LENGTH  60
 *   NDL$K_MIN_DATA_LENGTH 3
 *   NDL$K_IEEE_HEADER_LEN 14
 *
 * RETURNS:
 *	If successful,   the number of bytes transmitted is returned. 
 *	If unsuccessful, a negative value is returned.
 *
 * ARGUMENTS:
 *      struct FILE         *fp - Pointer to the FDDI file descriptor
 *	int           item_size	- size of item
 *	int              number	- number of items
 *	char          *user_buf - Message Pointer to send including the
-*/
int pdq_write (struct FILE *fp, int item_size, int number, char *user_buf) {
    struct INODE     *ip;	/* */
    struct NI_GBL    *np;	/*Pointer to NI global database*/	
    struct PDQ_PB    *pdq;	/*Pointer to the port block*/

    unsigned char *ndl_XmtBuf;          

    int 
    	i,
        len,			/*Length for padded buffers*/
    	index, 
    	bytes_written;		/*Keeps track of bytes_written*/ 

#if PDQ_TRACE
    pprintf("\nEntering pdq_write %s\n", fp->ip->name);
#endif

    /* DEFINITION OF CALLBACK WRITES 
    /*
    /*	o FDDI Callback Writes are Always ASYNCHRONOUS transfers:
    */

    /* CALLBACK WRITES ARE ENABLED.... 
    /*
    /*	This code:
    /*	  - Searches the callback_file_pointer[] for the first fp found
    /*	  - Note the cp_fp[] file pointer is not the same routine argument
    /*	  - Sends the packet, to the fp
    /*	  - Then EXITS routine.
    */
    for (i=0; i<cb_ref; i++)
    	if (fp == cb_fp[i])
	    {
#if CALLBACK_TRACE
            pprintf ("pdq_write: Callbacks Enabled %s\n", fp->ip->name);
#endif

    	    bytes_written = pdq_callback_write(fp, item_size, number, user_buf);
            return bytes_written;
	    }

    /* 
    /* CALLBACK WRITES ARE DISABLED.... ASYNCHRONOUS Transfers
    /*
    /*	 Get the pointer to the port block
    */

    /* Get the Ports Pointers
    */
    ip    = fp->ip;
    np    = (struct NI_GBL*)ip->misc;
    pdq   = (struct PDQ_PB*)np->pbp;

#if ENABLE_FDDI_XMT
    pprintf("\nPDQ_WRITE: %s size: %d number: %d buffer: %x\n", 
    		ip->name, item_size, number, user_buf);
#endif

    /* If the driver is STOPPED or UNINITIALIZED:
    /*   deallocate the packet and return zero bytes
    */
    if ((pdq->pb.state == PDQ$K_STOPPED) || (pdq->pb.state == PDQ$K_UNINITIALIZED))
    	{
    	ndl_deallocate_pkt (ip, user_buf);
    	return 0;
	}

    /* SINGLE SEGMENT:
    /* ---------------
    /* If the message to be sent will fit into a single packet,
    /* then send this message directly.  Otherwise the message 
    /* must be sent via segments.
    /*
    /* item_size == size of the object, in bytes.
    /* number    == the number of item_size to send 
    */
    if (item_size*number <= PDQ_K_MAX_PKT_SIZE)	
    	{
    	spinlock(&(pdq->spl_port));

        /* Give pdq_out the beginning ADDRESS of the User_buf to transmit,
        /* ie the first and only segement to transmit.
        /*
        /* pdq_out will then stick this ADDRESS into the xmt_descriptor and
        /* and produce the descriptor, thus giving it to the  Port.  
        /*
        /*      SOB=1   ! FIRST segments have START OF BUFFER =1
        /*      EOB=1   ! LAST  segments have END   OF BUFFER =1
        */
#if PDQ_TRACE
        pprintf("\npdq_writing SINGLE SEGMENT %s\n", fp->ip->name);
#endif
    	bytes_written = 
    		pdq_out(ip, user_buf, (item_size*number), PDQ_K_SOB|PDQ_K_EOB);

        spinunlock(&(pdq->spl_port));

    	/* Return the number of bytes written.  
    	*/
        ndl_deallocate_pkt(fp->ip, user_buf);

    	return bytes_written;
	}


    /* MULTIPLE SEGMENTS
    */
    /* The message will NOT fit into a single ndl_allocate packet,
    /* so send it in segments. 
    */
    else
	{	
        spinlock(&(pdq->spl_port));

	/* Init the index and assign bytes_written 
    	/* to the total number of bytes to write
    	*/
    	index          = 0;			/* First Segment */
    	bytes_written  = (item_size * number);	/* total bytes to transfer */

	/* SEND THE FIRST SEGMENT. 
    	/* -----------------------
    	/* This message is sent in segments so only the First_segment 
    	/* is set ie.
    	/*
    	/*	SOB=1	! middle segments have START OF BUFFER =1
    	/*	EOB=0	! middle segments have END   OF BUFFER =0
    	/*
    	/* Give pdq_out the beginning ADDRESS of the User_buf to transmit,
    	/*
    	/* pdq_out will then stick this ADDRESS into the xmt_descriptor and
    	/* and produce the descriptor, thus giving it to the  Port.  
    	*/
    	/* pdq_out returns the number of bytes sent if successfull.  
    	/* If not successfull a byte length of zero is returned.  
    	/* Return with error if pdq_out returns a zero byte length.  
    	/* Otherwise continue to send.
    	*/
#if PDQ_TRACE
        pprintf("\npdq_writing MULTIPE SEGMENT FIRST%s\n", fp->ip->name);
#endif
    	if (!(pdq_out(ip, user_buf, PDQ_K_MAX_PKT_SIZE, PDQ_K_SOB)))
    	    {
            spinunlock(&(pdq->spl_port));        
            ndl_deallocate_pkt(fp->ip, user_buf);
    	    return 0;
    	    }

    	/* For each segment written, the bytes_written will be the
    	/* maximum number of possible to write.  The LAST_SEGMENT,
    	/* unless exactly even number of PDQ_K_MAX_PKT_SIZE, wiil be
    	/* less than a full segment.  This will signal the last 
    	/* segment to transmit.
    	*/
    	index += PDQ_K_MAX_PKT_SIZE;		/* Next segment to send */
	bytes_written -= PDQ_K_MAX_PKT_SIZE;	/* number bytes remaining */

	/* SEND MIDDLE SEGMENTS
    	/* --------------------
	/* Keep sending segments until a segment is less than a complete
    	/* segment, ie the last segment.
    	*/
    	while ((bytes_written - PDQ_K_MAX_PKT_SIZE) > 0)
    	    {
    	    /* Give pdq_out the NEXT ADDRESS of the User_buf to transmit,
    	    /* ie the NEXT SEGEMENT to transmit.
    	    /*
    	    /* pdq_out will then stick this ADDRESS into the xmt_descriptor and
    	    /* and produce the descriptor, thus giving it to the  Port.  
    	    */
    	    /* pdq_out returns the number of bytes sent if successfull.  
    	    /* If not successfull a byte length of zero is returned.  
    	    /* Return with error if pdq_out returns a zero byte length.  
    	    /* Otherwise continue to send.
    	    /*
    	    /*	SOB=0	! middle segments have START OF BUFFER =0
    	    /*	EOB=0	! middle segments have END   OF BUFFER =0
    	    */
#if PDQ_TRACE
    	    pprintf("\npdq_writing MULTIPE SEGMENT MIDDLE%s\n", fp->ip->name);
#endif
    	    if (!(pdq_out(ip, &(user_buf[index]), PDQ_K_MAX_PKT_SIZE, 0)))
	    	{
                spinunlock(&(pdq->spl_port));    
                ndl_deallocate_pkt(fp->ip, user_buf);
    		return 0;
		}

    	    /* dito
    	    */
    	    index += PDQ_K_MAX_PKT_SIZE;		/* Next segment to send */
	    bytes_written -= PDQ_K_MAX_PKT_SIZE;	/* bytes remaining */
	    }    /* end-while */

	/* SEND LAST SEGMENT
    	/* -----------------
    	/*   For each MIDDLE segment written, the bytes_written will be the
    	/*   maximum number bytes possible to write.  The LAST_SEGMENT,
    	/*   unless exactly even number of PDQ_K_MAX_PKT_SIZE, wiLl be
    	/*   less than a full segment.  This will signal the last 
    	/*   segment to transmit.
    	*/
    	/* Give pdq_out the ADDRESS of the User_buf to transmit,
    	/* ie the LAST SEGEMENT to transmit.
    	/*
    	/* pdq_out will then stick this ADDRESS into the xmt_descriptor and
    	/* and produce the descriptor, thus giving it to the  Port.  
    	*/
    	/* pdq_out returns the number of bytes sent if successfull.  
    	/* If not successfull a byte length of zero is returned.  
    	/* Return with error if pdq_out returns a zero byte length.  
    	/* Otherwise continue to send.
    	/*
    	/*	SOB=0	! middle segments have START OF BUFFER =0
    	/*	EOB=1	! middle segments have END   OF BUFFER =1
    	*/
#if PDQ_TRACE
        pprintf("\npdq_writing MULTIPE SEGMENT LAST%s\n", fp->ip->name);
#endif
    	if (!(pdq_out(ip, &(user_buf[index]), bytes_written, PDQ_K_EOB)))
    	    {
            spinunlock(&(pdq->spl_port));        
            ndl_deallocate_pkt(fp->ip, user_buf);
	    return 0;
	    }

        spinunlock(&(pdq->spl_port));    

    	ndl_deallocate_pkt(fp->ip, user_buf);

	return (item_size*number);	/* Return the length */
    	}    /* end  while */
    }


/*+
 * ============================================================================
 * =  pdq_ServiceRcvDataQueue - pdq_ServiceRcvDataQueue			      =
 * ============================================================================
 *
 * OVERVIEW:
 *  This function services the Receive Data Queue by simply advancing the 
 *  completion index by one, and advancing the producer index by one to free 
 *  the receive buffer. It is important to note that this routine frees the 
 *  buffer pointed to by the completion index, i.e. the buffer that was 
 *  returned by GetNextFrame. Therefore, applications using these routines 
 *  should insure that all processing of the received frame is completed 
 *  before calling pdq_ServiceRcvDataQueue.
 *
 * ARGUMENTS:
 *	*ip	 - The Inode of Port Driver Calling this routine
-*/
void pdq_ServiceRcvDataQueue (struct INODE *ip) {
    struct NI_GBL 	*np;	/* Pointer to the NI global database*/
    struct PDQ_PB  	*pdq;	/* Pointer to the port block*/	
    PDQ_PRODUCER_INDEX  *pi;	/* Producer Index block*/	

    unsigned int lwbuf;
    unsigned int 
    	reg;

#if PDQ_TRACE
    pprintf("\nEntering pdq_ServiceRcvDataQueue %s\n", ip->name);
#endif

    np  = (struct NI_GBL*) 	ip->misc;
    pdq = (struct PDQ_PB*)  	np->pbp;
    pi  = (PDQ_PRODUCER_INDEX*) pdq->pip;

    /* Service the RcvData Queue by making the 
    /*	RcvData_PRODUCER_Completion == RcvData_CONSUMER_Completion
    /*	Producing one (1) RcvData_PRODUCER
    */
    pdq_read_mem(pdq, &pdq->emp->cb.xmt_rcv_data, sizeof(PDQ_ULONG), &lwbuf);

    pi->RcvDataCompletionIndex = lwbuf & PDQ_CONS_M_RCV_INDEX;
    pi->RcvDataProducer        = (pi->RcvDataCompletionIndex + 1) & 0x00FF;

    /* Get a copy of the Adapter Register:
    /*	RcvData_PRODUCER_Completion
    /*	RcvData_PRODUCER_Producer
    */
    reg = pdq_read_csr(pdq, PDQ_Type2Prod);

    /* Clear fields for:
    /*	RcvData_PRODUCER_Completion
    /*	RcvData_PRODUCER_Producer
    */
    reg &= ~PDQ_TYPE_2_PROD_M_RCV_DATA_COMP;  
    reg &= ~PDQ_TYPE_2_PROD_M_RCV_DATA_PROD;  

    /* OR in the new fields values for:
    /*	RcvData_PRODUCER_Completion
    /*	RcvData_PRODUCER_Producer
    */
    reg |= pi->RcvDataCompletionIndex << PDQ_TYPE_2_PROD_V_RCV_DATA_COMP;
    reg |= pi->RcvDataProducer        << PDQ_TYPE_2_PROD_V_RCV_DATA_PROD;

#if PDQ_SERVICE_POST 
    pprintf("\n%s Service: RCV DATA PRODUCER: %08x\n", ip->name, reg);
#endif

    /* Write the new value for the 
    /*	RcvData_PRODUCER_Completion
    /*	RcvData_PRODUCER_Producer
    */
    pdq_write_csr(pdq, PDQ_Type2Prod, reg); 	
    }


#if PDQ_FYI

/*+
 * ============================================================================
 * =  pdq_GetNextRcvFrame - 				      =
 * ============================================================================
 *
 * OVERVIEW:
 * This function returns the address of the next RCV Data frame to be processed
 * by the driver. This frame is defined as the buffer pointed to
 * by the RcvDataCompletionIndex. The completion index is used to index into the
 * descriptor table, and the address of the buffer is returned in
 * longword format suitable for casting into whatever pointer is desired.
 *
 * This routine does NOT advance the completion index. To complete the process
 * of servicing the receive data queue, the ServiceRcvDataQueue() function is
 * called. That routine advances the completion index, acknowleging receipt of
 * the frame, and frees the buffer for receiving data. 
 *
 * ARGUMENTS:
 *	*pdq - The Inode of Port Driver Calling this routine
 *
-*/
unsigned int pdq_GetNextRcvFrame (struct INODE *ip) {
    struct NI_GBL  *np;         /*Pointer to NI global database*/
    struct PDQ_PB  *pdq;        /*Pointer to the port block*/

    PDQ_PRODUCER_INDEX *pi;	/* Producer Index block*/	
    unsigned int lwbuf;

    np  = (struct NI_GBL*)ip->misc; 
    pdq = (struct PDQ_PB*)np->pbp;      
    pi  = (PDQ_PRODUCER_INDEX*)pdq->pip;

#if PDQ_TRACE
    pprintf("\nEntering pdq_GetNextRcvFrame %s\n", ip->name);
#endif

    pdq_read_mem(pdq, &pdq->emp->db.rcv_data[pi->RcvDataProducer].buff_lo,
	         sizeof(PDQ_ULONG), &lwbuf);

    return (lwbuf);
    }
#endif


/*+
 * ============================================================================
 * =  pdq_CheckRcvDataQueue - pdq_CheckRcvDataQueue			      =
 * ============================================================================
 *
 * OVERVIEW:
 * 	This function checks the Receive Data Queue.
 *
 * RECEIVE QUEUE MANAGEMENT:
 * ------------------------
 *	The Port has sent the Host a Message(s) when its..
 *	 	Ports_CONSUMER_Index != Hosts_COMPLETION_Index.
 *
 * RETURNS:
 *   False	Port_Consumer_Index == Host_Completion_index  !No frames rcv'ed
 *   True	Port_Consumer_Index != Host_Completion_index  !   frames rcv'ed
 *
 * ARGUMENTS:
 *	*pdq - The Inode of Port Driver Calling this routine
 *
-*/
int pdq_CheckRcvDataQueue (struct INODE *ip) {
    struct NI_GBL  *np;         /*Pointer to NI global database*/
    struct PDQ_PB  *pdq;        /*Pointer to the port block*/

    unsigned int lwbuf;
    unsigned int  
    	PortConsumer;

    PDQ_PRODUCER_INDEX  *pi;	/* Producer Index block*/	

    np  = (struct NI_GBL*)ip->misc; 
    pdq = (struct PDQ_PB*)np->pbp;  
    pi  = (PDQ_PRODUCER_INDEX*)pdq->pip;

    /* Get the Ports Receive Data Consumer index
    */
    pdq_read_mem(pdq, &pdq->emp->cb.xmt_rcv_data, sizeof(PDQ_ULONG), &lwbuf);

    PortConsumer = (lwbuf & PDQ_CONS_M_RCV_INDEX);

    /*	Port_Consumer_Index != Host_Completion_index  !   frames rcv'ed
    /*	Port_Consumer_Index == Host_Completion_index  !No frames rcv'ed
    */
    if (PortConsumer != pi->RcvDataCompletionIndex)
    	{
#if ENABLE_PRINTF
        pprintf("\n%s Check:   RCV DATA QUEUE: Port:%02x, Host:%02x\n", 
			pdq->name, PortConsumer, pi->RcvDataCompletionIndex);
#endif
        return TRUE;
    	}
    else
    	return FALSE;
    }



/*+
 * ============================================================================
 * = pdq_free_rcv_pkts - Frees Received Packets from the CallBack queue                 =
 * ============================================================================
 *
 * OVERVIEW:
 *	This routine will Purge received packets and memory used for the 
 *	CallBack received packet queue.
 *
 * RETURNS:
 *	None
 *
 * ARGUMENTS:
 *	struct INODE *ip - Inode of the current port.
 *
-*/
void pdq_free_rcv_pkts (struct INODE *ip) {
    struct NI_GBL  *np;	
    struct PDQ_PB  *pdq;
    struct PDQ_MSG *mr;	

#if PDQ_TRACE
    pprintf("\nEntering pdq_free_rcv_pkts  %s\n", ip->name);
#endif

    /* Get the pointers to the datalink info
    */
    np  = (struct NI_GBL*) ip->misc;
    pdq = (struct PDQ_PB*) np->pbp;

    spinlock(&spl_kernel);		/* Syncronize QUEUE access */

    while (pdq->rqh.flink != (struct PDQ_MSG *)&(pdq->rqh.flink))
    	{
	/* Purge the Receiver Packet from the queue
    	/* And deallocate/free its memory
    	*/
    	mr = (struct PDQ_MSG *)remq(pdq->rqh.flink);
    	pdq->rcv_msg_cnt--;
    	spinunlock(&spl_kernel);

    	ndl_deallocate_pkt(ip, mr->msg);
    	free(mr);

    	spinlock(&spl_kernel);		/* Syncronize QUEUE access */
	}

    spinunlock(&spl_kernel);
    }



/*+
 * ============================================================================
 * = pdq_msg_rcvd - Handle a received message.                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *	Depending if CALLBACKS are Enabled, this routine will EITHER:
 *	    - Give the Received Packet to the MOP/NI_DATALINK Protocols or
 *	    - Give it to the Callback via the pdq->rqh queue.  pdq_read()
 *	      will remove the Packets from pdq->rqh as called by the OS.
 *
 * ARGUMENTS:
 *	*fp       - Pointer to the receive process port file.
 *	*pdq      - Pointer to the port block.
 *	*mc 	  - Pointer to the counters.
 *	*ndl_msg  - Pointer to the ndl_message received.
 *	size	  -
-*/
void pdq_msg_rcvd (struct FILE *fp, 
    			struct PDQ_PB 		 *pdq,
                        struct MOP$_V4_COUNTERS  *mc,
			char                     *ndl_msg,
                        unsigned int	size) {

    struct PDQ_MSG *mh;

#if PDQ_TRACE
    pprintf("\nEntering pdq_msg_rcvd %s\n", fp->ip->name);
#endif

#if ENABLE_PRINTF
    pprintf(msg_net_008,
	    (unsigned char)ndl_msg[0], (unsigned char)ndl_msg[1], (unsigned char)ndl_msg[2],
	    (unsigned char)ndl_msg[3], (unsigned char)ndl_msg[4], (unsigned char)ndl_msg[5]);
    pprintf(msg_net_010,
	    (unsigned char)ndl_msg[6], (unsigned char)ndl_msg[7], (unsigned char)ndl_msg[8],
	    (unsigned char)ndl_msg[9], (unsigned char)ndl_msg[10], (unsigned char)ndl_msg[11]);
#endif

    mc->MOP_V4CNT_RX_BYTES += size;
    mc->MOP_V4CNT_RX_FRAMES++;

    /* If Callbacks are ENABLED:
    /*  pdq_read() is called via the OS and will return these raw 
    /*  network Packets to the caller
    /*
    /* If Callbacks are DISABLED:
    /*	We'll return these network Packets to the MOP/NI_DATALINK Class
    /*  drivers, ie via the ni_datalink.c routines.
    */
    if (cb_ref)
	{
    	/* While Callbacks are Enabled:
	/*   Purge the Ports Receive Msg queue, if the Host fails to read them
    	/*   before the queue fills.
    	*/
        if (pdq->rcv_msg_cnt > PDQ_MAX_RCV_MSGS)
            pdq_free_rcv_pkts(fp->ip);

	/* Allocate a Message holder
	/* Fill in the message holder with the Receive Packet pointer
    	*/
    	mh = (struct PDQ_MSG*)malloc(sizeof(struct PDQ_MSG));
    	mh->msg  = ndl_msg;
    	mh->size = size;

	/* Insert All Received Packets into the CallBack Receive_Msg_queue.
    	/* This queue is primarily used for CallBacks Reads.  See pdq_read().
 	/* These Received Packets are removed from this queue by either
 	/* pdq_read() or pdq_free_rcv_pkts().    
    	*/
    	spinlock(&spl_kernel);
    	insq(mh, pdq->rqh.blink);
    	pdq->rcv_msg_cnt++;
    	spinunlock(&spl_kernel);

    	/* If Callbacks are ENABLED:
    	/*	THERE IS NO NEED TO GIVE THIS PACKET TO THE UPPER
    	/*	LEVEL MOP/NI_DATALINK PROTOCOLS. SO RETURN.
    	*/
    	return;
	}

    /* CALLBACKS ARE DISABLED.
    /*	SO WE MUST GIVE THIS RECEIVED PACKET TO THE UPPER LEVEL
    /*	MOP/NI_DATALINK PROTOCOLS AND NOT INTO pdq->rqh.
    */
/*    size -= 4;		Already Strip off the crc */

    /* Give the upper level protocols this packet by
    /* calling the Class Drivers "Receive Message Routine".
    /*  	pdq->lrp == *rp = (int)ndl_receive_msg
    */
    if (pdq->lrp)
    	(*(pdq->lrp))(fp, ndl_msg, size);	/* Call the class driver rcv routine */
    else
	{
	/* If there is no "Receive Msg Routine":
    	/*	bump the MOP counter and continue
    	/*	deallocate the packet and return.
    	*/
        mc->MOP_V4CNT_UNKNOWN_DESTINATION++;	

	/* Give the buffer back
    	*/
    	ndl_deallocate_pkt(fp->ip, ndl_msg);
	}

    return;
    }



/*+
 * ============================================================================
 * = pdq_rx - Recieve process.                                                 =
 * ============================================================================
 *
 * OVERVIEW:
 * This PROCESS reads a received frame from the Adapter. It then
 * ndl_allocate_pkt() a packet, copies the received data into the 
 * buffer IEEE style and returns the number of characters copied. 
 *
 * This routine takes the FDDI frame, and munges it into IEEE frame format. 
 * The upper level Protocol code which receives the frame expects IEEE frame 
 * format.  The conversion is straightforward and as follows:
 *
 * RETURNS:
 * 	msg_failure	- In the event that the adapter is no longer in the 
 * 			  LINK AVAILABLE state. 
 * ARGUMENTS:
 *	*ip - The Inode of Port Driver Calling this routine
-*/
int pdq_rx (struct INODE *ip) {
    struct FILE 	 *fp;		/* DEFEA File descriptor */
    struct NI_GBL        *np;		/* Pointer to NI global database*/	
    struct PDQ_PB        *pdq;		/* Pointer to the port block*/
    struct MOP$_V4_COUNTERS *mc;        /*Pointer to the MOP counters*/
    PDQ_PRODUCER_INDEX *pi;	/* Producer Index block*/	


    PDQ_RCV_DATA_BUFFER *fddi_rcv_frame;	/* Frame as it appears in PDQ Rcv Data Queue */

    ETHERNET_FRAME * enet_frame;  	/* Ethernet Frame Format */
    IEEE_FRAME     * ieee_frame;        /* IEEE Frame Format */

    unsigned char FrameFormat;

    unsigned int *lw_data;

    unsigned int
    	fptr,			/* Receive Frame Pointer */
    	v4s,			/* IEEE v4 size field */
    	status,			/* place holder of status to return */
    	rcv_frame_size, 	/* size of the received FDDI frame */
    	data_size;		/* number of frame data bytes, not incl'ing crc */

#if PDQ_TRACE
    pprintf("\nEntering pdq_rx %s\n", ip->name);
#endif

    /* fopen forever a DEFEA Port Driver Adapter.
    /* Used mainly for CLASS DRIVER Call, "(*(pdq->lrp))(fp, msg, size)",
    /* to gain access to the DEFEA Adapter Port Block structure.
    */
    fp = fopen (ip->name, "r+");

    if (!fp)
    	return msg_failure;

    fddi_rcv_frame = (PDQ_RCV_DATA_BUFFER *)malloc(sizeof(PDQ_RCV_DATA_BUFFER));

    /* Get the pointers
    */
    np  = (struct NI_GBL*)ip->misc;
    pdq = (struct PDQ_PB*)np->pbp;
    mc  = (struct MOP$_V4_COUNTERS*)np->mcp;
    pi  = (PDQ_PRODUCER_INDEX*)pdq->pip;

    /* CONTINUE THIS PROCESS UNTIL KILLED OR ERRORS DETECTED
    /* -----------------------------------------------------
    */    	
    status = msg_success;	/* anticipate good return status */

    while (TRUE)
        {
    	/* KEEP PROCESSING INCOMING MESSAGES AS THEY COME IN
    	/* -------------------------------------------------
    	/*  We will not go back to sleep until they are all processed.
        */    	
	while (pdq_CheckRcvDataQueue(ip)) 
    	    {
            /* To reduce the number of eisa memory reads, first get the size 
    	    /* of the received FDDI frame.  If not zero and if not to large,
    	    /* then read in the entire frame.  
            */


    	    /* pdq_GetNextRcvFrame(ip);
    	    */
    	    pdq_read_mem(pdq, 
    			&pdq->emp->db.rcv_data[pi->RcvDataProducer].buff_lo, 
        		sizeof(PDQ_ULONG), &fptr);

    	    pdq_read_mem(pdq, fptr, sizeof(PDQ_ULONG), fddi_rcv_frame);

            rcv_frame_size = fddi_rcv_frame->rcv_descriptor & PDQ_FMC_DESCR_M_LEN;

            if ((rcv_frame_size == 0) || (rcv_frame_size > (8192-21)))
    		{
    		pdq_FlushUnsolicited(ip);
    		pdq_ServiceRcvDataQueue(ip);	/* reclaim receive buffer */
    		status = msg_failure;
    		break;
    		}

            /* Is the frame reporting errors?
            */
            if (fddi_rcv_frame->rcv_descriptor & PDQ_FMC_DESCR_M_FSB_ERROR)
    		{
    		pdq_FlushUnsolicited(ip);	
    		pdq_ServiceRcvDataQueue(ip);	
    		err_printf(msg_pdq_msg4, ip->name, fddi_rcv_frame->rcv_descriptor);
    		status = msg_failure;
		break;
    		}

    	    /* Good byte count and no errors so read the entire frame.
    	    */
    	    pdq_read_mem(pdq, fptr, (rcv_frame_size+3+4), fddi_rcv_frame);

            /* Get the size of the received FDDI frame. 
    	    /*
    	    /*   RCV_FRAME_SIZE
    	    /*	    Is the size of the rcv data frame, in bytes, from the 
    	    /*	    FC to the CRC inclusive and not to include the:
    	    /*	    	RcvStatus[0:3] nor Prh[0:2]
    	    /*
    	    /*   DATA_SIZE 
    	    /*	    Is the number of data bytes in the frame (i.e. 
    	    /*      rcv_frame_size - (FC+DA+SA+DSAP+SSAP+CNTR+PID[5]+CRC[4])
    	    /*
    	    /*	    The 4 byte CRC code will be copied along with data but 
    	    /*      is not included within the data_size.
    	    /*	    25 == FC+DST[6]+SRC[6]+DSAP+SSAP+CNTR+PID[5]+CRC[4]
            */
            data_size = rcv_frame_size - 25;	/* not including CRC */

#if ENABLE_PRINTF||ENABLE_FDDI_RCV
            pprintf("\nPDQ_RX: %s fddi_rcv_frame_size: %d data_size: %d\n", 
    			fp->ip->name, rcv_frame_size, data_size);
#endif

#if ENABLE_FDDI_RCV
    	    spinlock(&spl_kernel);
    	    pprintf("\nRECEIVE FDDI FRAME FORMAT %s\n", ip->name);

    	    pprintf("fddi_rcv_frame->prh_0: %x\n",  fddi_rcv_frame->prh_0);	/* Unrestricted token use */
    	    pprintf("fddi_rcv_frame->prh_1: %x\n",  fddi_rcv_frame->prh_1);	/* Unrestricted token release and */
    	    pprintf("fddi_rcv_frame->prh_2: %x\n",  fddi_rcv_frame->prh_2);	/* mbz */
    	    pprintf("\n");
    	    pprintf("fddi_rcv_frame->fc: %x\n",     fddi_rcv_frame->fc);		/* FC = 0x54  LLC */

    	    for (v4s=0; v4s<6; v4s++)
	    	pprintf("fddi_rcv_dest_addr[%d]: %x\n", v4s, fddi_rcv_frame->dest_addr[v4s]);

    	    pprintf("\n");

    	    for (v4s=0; v4s<6; v4s++)
        	pprintf("fddi_rcv_source_addr[%d]: %x\n", v4s, fddi_rcv_frame->source_addr[v4s]);

    	    pprintf("\n");

    	    pprintf("fddi_rcv_frame->dsap: %x\n", fddi_rcv_frame->dsap);
    	    pprintf("fddi_rcv_frame->ssap: %x\n", fddi_rcv_frame->ssap);
    	    pprintf("fddi_rcv_frame->cntr: %x\n", fddi_rcv_frame->cntr);
    	    pprintf("\n");
    	    pprintf("fddi_rcv_frame->pid_0: %x\n", fddi_rcv_frame->pid_0);
    	    pprintf("fddi_rcv_frame->pid_1: %x\n", fddi_rcv_frame->pid_1);
    	    pprintf("fddi_rcv_frame->pid_2: %x\n", fddi_rcv_frame->pid_2);
     	    pprintf("fddi_rcv_frame->pid_3: %x\n", fddi_rcv_frame->pid_3);
    	    pprintf("fddi_rcv_frame->pid_4: %x\n", fddi_rcv_frame->pid_4);

    	    pprintf("\n");

    	    lw_data = fddi_rcv_frame->data;

    	    for (v4s=0; v4s<4; v4s++)
                pprintf("fddi_rcv_frame->data[%d] = %x\n", v4s, lw_data[v4s]);

    	   spinunlock(&spl_kernel);
#endif

            /* Fixes received frames data sizes greater then
            /* NDL$K_MAX_DATA_LENGTH
            */
            if (data_size > NDL$K_MAX_DATA_LENGTH)
                {
                pdq_ServiceRcvDataQueue(ip);
                pdq_FlushUnsolicited(ip);
                mc->MOP_V4CNT_UNKNOWN_DESTINATION++;
                continue;
                }

  
    	    if ((!fddi_rcv_frame->pid_0) && 
    		(!fddi_rcv_frame->pid_1) &&
    		(!fddi_rcv_frame->pid_2))
        	FrameFormat = TRUE;	/* Ethernet Frame format */
    	    else
        	FrameFormat = FALSE;	/* IEEE Frame format */


    	    /* ndl_allocate packet where we'll convert the FDDI frame into
    	    /* an IEEE frame, and then pass this IEEE frame up the protocol.
    	    /* Allocate an extra 4 bytes to hold the CRC codes.
    	    /* 
    	    /* Descriptor Data Buffer sizes are fixed. see pdq_init_DBlock()
    	    /* Data buffers passed up the protocol are variable via enviromnent
    	    /* variables.  DEFAULT ndl_allocate_pkt Sizes are as follows:
    	    /*    msg_buf_size = 1518 bytes
    	    /*	  modulus      = 128
    	    /*    remainder    = 0
    	    /*    zone         = 0
    	    */
    	    if (FrameFormat)
    		{
    		enet_frame = ndl_allocate_pkt(ip, 0); 

                /* Convert the FDDI frame into an ETHERNET/IEEE frame 
                */
                memcpy (enet_frame->dest_addr,   fddi_rcv_frame->dest_addr,   6); 	/* Stuff Dest Address */
                memcpy (enet_frame->source_addr, fddi_rcv_frame->source_addr, 6);	/* Stuff Source Address */

                enet_frame->protocol_type_0 = fddi_rcv_frame->pid_3;
                enet_frame->protocol_type_1 = fddi_rcv_frame->pid_4;

    		memcpy (enet_frame->data, fddi_rcv_frame->data, data_size+4);	/* Stuff frame data incl crc */

#if ENABLE_ENET_IEEE_RCV
        	spinlock(&spl_kernel);
    		pprintf("\nRECEIVE ETHERNET FRAME FORMAT %s\n", ip->name);

    		for (v4s=0; v4s<6; v4s++)
	    	    pprintf("enet_rcv_dest_addr[%d]: %x\n", v4s, enet_frame->dest_addr[v4s]);

    		pprintf("\n");

    		for (v4s=0; v4s<6; v4s++)
    		    pprintf("enet_rcv_source_addr[%d]: %x\n", v4s, enet_frame->source_addr[v4s]);

    		pprintf("\n");

    		pprintf("enet_rcv_frame->protocol_type_0: %x\n", enet_frame->protocol_type_0);
    		pprintf("enet_rcv_frame->protocol_type_1: %x\n", enet_frame->protocol_type_1);

    		pprintf("\n");

    		lw_data = enet_frame->data;

    		for (v4s=0; v4s<4; v4s++)
    		    pprintf("enet_rcv_frame->data[%d] = %x\n", v4s, lw_data[v4s]);

    		spinunlock(&spl_kernel);
#endif
    		}
    	    else
    		{
    		ieee_frame = ndl_allocate_pkt(ip, 0); 

                /* Convert the FDDI frame into an ETHERNET/IEEE frame 
                */
                memcpy (ieee_frame->dest_addr,   fddi_rcv_frame->dest_addr,   6); 	/* Stuff Dest Address */
                memcpy (ieee_frame->source_addr, fddi_rcv_frame->source_addr, 6);	/* Stuff Source Address */

    		/* Size of a version 4, IEEE frame, is the size of the data field
    		/* plus the size of the LLC field
    		*/
    		v4s = data_size + NDL$K_IEEE_H_SIZE;
    		ieee_frame->len[0] = (unsigned char)((v4s>>8) & 0xff);
    		ieee_frame->len[1] = (unsigned char)( v4s     & 0xff);

    		ieee_frame->dsap = fddi_rcv_frame->dsap;
    		ieee_frame->ssap = fddi_rcv_frame->ssap;
    		ieee_frame->cntr = fddi_rcv_frame->cntr;

    		ieee_frame->pid_0 = fddi_rcv_frame->pid_0;
    		ieee_frame->pid_1 = fddi_rcv_frame->pid_1;
    		ieee_frame->pid_2 = fddi_rcv_frame->pid_2;
    		ieee_frame->pid_3 = fddi_rcv_frame->pid_3;
    		ieee_frame->pid_4 = fddi_rcv_frame->pid_4;

    		memcpy (ieee_frame->data, fddi_rcv_frame->data, data_size+4);	/* Stuff frame data incl crc */

#if ENABLE_ENET_IEEE_RCV
    		spinlock(&spl_kernel);
    		pprintf("\nRECEIVE IEEE FRAME FORMAT %s\n", ip->name);

    		for (v4s=0; v4s<6; v4s++)
	    	    pprintf("ieee_rcv_dest_addr[%d]: %x\n", v4s, ieee_frame->dest_addr[v4s]);

    		pprintf("\n");

    		for (v4s=0; v4s<6; v4s++)
        	    pprintf("ieee_rcv_source_addr[%d]: %x\n", v4s, ieee_frame->source_addr[v4s]);

    		pprintf("\n");

        	pprintf("ieee_rcv_frame->len[0]: %x\n", ieee_frame->len[0]);
        	pprintf("ieee_rcv_frame->len[1]: %x\n", ieee_frame->len[1]);

    	        pprintf("ieee_rcv_frame->dsap: %x\n", ieee_frame->dsap);
    	        pprintf("ieee_rcv_frame->ssap: %x\n", ieee_frame->ssap);
    	        pprintf("ieee_rcv_frame->cntr: %x\n", ieee_frame->cntr);
    	        pprintf("\n");
    	        pprintf("ieee_rcv_frame->pid_0: %x\n", ieee_frame->pid_0);
    	        pprintf("ieee_rcv_frame->pid_1: %x\n", ieee_frame->pid_1);
    	        pprintf("ieee_rcv_frame->pid_2: %x\n", ieee_frame->pid_2);
     	        pprintf("ieee_rcv_frame->pid_3: %x\n", ieee_frame->pid_3);
    	        pprintf("ieee_rcv_frame->pid_4: %x\n", ieee_frame->pid_4);

    	        pprintf("\n");

    		lw_data = ieee_frame->data;

    		for (v4s=0; v4s<4; v4s++)
        	    pprintf("ieee_rcv_frame->data[%d] = %x\n", v4s, lw_data[v4s]);

    		spinunlock(&spl_kernel);
#endif
    		}


    	    /* It is safe to PRODUCE another receive descriptor, once 
    	    /* this received data has been copied into the maloc'ed buffer.
    	    */
            pdq_ServiceRcvDataQueue(ip);		/* reclaim receive buffer */

    	    /* Call the CLASS DRIVER receive routine.  The size of the 
    	    /* IEEE frame should be the size of the DataFrame plus the
    	    /* size of a standard IEEE header.
    	    /*
    	    /* Also, the upper level protocols will ndl_deallocate_pkt this
    	    /* frame when it has completed processing it.  
    	    /* This implemtation of pdq_driver, only supports xmt/rcv'ing 
    	    /* SINGLE frames at a time.  Therefore, the Descriptor buffers 
    	    /* allocated by this pdq_driver are never deallocated.
    	    */
    	    if (pdq->loopback)
    		if (FrameFormat)
	    	    ndl_deallocate_pkt (ip, enet_frame);
    		else
	    	    ndl_deallocate_pkt (ip, ieee_frame);
    	    else
    		/* This little gem will either give this packet to either
    		/* the CALLBACK queue or to the upper level protocols, ie
    		/* MOP/NI_DATALINK.
    		*/
    		if (FrameFormat)
    		    pdq_msg_rcvd(fp, pdq, (struct MOP$_V4_COUNTERS*)mc, 
    			(int)enet_frame, 
    			(data_size+(sizeof(struct NDL$ENET_HEADER)-2)));   
    		else
    		    pdq_msg_rcvd(fp, pdq, (struct MOP$_V4_COUNTERS*)mc, 
    			(int)ieee_frame, 
    			(data_size+(sizeof(struct NDL$IEEE_HEADER))));   


            pdq_FlushUnsolicited(ip);
    	    }   /* end while (!pdq_CheckRcvDataQueue(pdq))  */

        /* Exit this process if the user sent us a PDQ$K_STOPPED
        */
        if (pdq->pb.state == PDQ$K_STOPPED) 
    	    {
            krn$_post (&(pdq->pdq_stop_rx));
    	    status = msg_success;
    	    break;		/* need to break while(1) to do fclose's */
    	    }

    	/* Continue this DEFEA receive process for as long as adapter is running.
    	/* Kill the process only when the adapter has transitioned to halt state.
    	*/
    	if (pdq_GetAdapterState(pdq) == PDQ_STATE_K_HALTED)
    	    {
    	    err_printf(msg_pdq_msg5, ip->name);
    	    status = msg_failure;
    	    break;	/* need to break while(1) to do fclose's */
    	    }

    	/* ckeck for killpending, flush the Unsolicited queue 
	/* and go to sleep again.
    	*/
    	if (killpending())
	    {
    	    status = msg_success;
    	    break;	/* need to break while(1) to do fclose's */
	    }

    	pdq_FlushUnsolicited(ip);	
    	krn$_sleep(0);                /* return to system and return asap */
        }    /* end while (TRUE) */

    /* On exiting this process, fclose DEFEA which inturn closes
    /* the attached Bus Port Driver.
    */
    free(fddi_rcv_frame);
    fclose(fp);			/* DEFEA Port Driver */
    
    return status;
    }



#if PDQ_FYI

/*+
 * ============================================================================
 * = pdq_interrupt - Interrupt routine.                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *	tbd
 *
 * RETURNS:
 *	None
 *                 
 * ARGUMENTS:
 *	struct PDQ_PB *pdq - pointer to DEFEA port block
 *
-*/
void pdq_interrupt (struct PDQ_PB *pdq) {
    /* Keep looping until we know there were no interrupts set
    */
    while (1)
    	{
    	/* Handle Transmit interrupt
    	*/
    	if (TRUE)
	    {
    	    krn$_bpost (&(pdq->pdq_isr_tx));
    	    (pdq->XmtIntCnt)++;
	    }

    	/* Handle Receive interrupt
    	*/
    	if (TRUE)
	    {
    	    krn$_bpost(&(pdq->pdq_isr_rx));
    	    (pdq->RcvIntCnt)++;
	    }

	/* Acknowledge the interrupts
    	*/

	}
    }
#endif

/*+
 * ============================================================================
 * = pdq_interrupt - Interrupt routine.                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *	tbd
 *
 * RETURNS:
 *	None
 *                 
 * ARGUMENTS:
 *	struct PDQ_PB *pdq - pointer to DEFEA port block
 *
-*/
void pdq_interrupt (struct PDQ_PB *pdq) {

#if 0
		io_issue_eoi (pdq, pdq->pb.vector);
#else
		io_disable_interrupts (pdq, pdq->pb.vector);
#endif
    }



/*+
 * ============================================================================
 * =  pdq_ServiceCmdRspQueue - pdq_ServiceCmdRspQueue			      =
 * ============================================================================
 *
 * OVERVIEW:
 *  This function services the Command Response Queue by simply 
 *  advancing the completion index by one, and advancing
 *  the producer index by one to free up the the Command Response buffer.
 *  We can get away with freeing the command response buffer immediately,
 *  since command responses are generated only after command requests,
 *  thus insuring that the single command response buffer won't get stepped
 *  on.  
 *
 * RETURNS:
 *
 * ARGUMENTS:
 *	*ip - The Inode of Port Driver Calling this routine
 *
-*/
void pdq_ServiceCmdRspQueue (struct INODE *ip) {
    struct NI_GBL      *np;	/* Pointer to the NI global database*/
    struct PDQ_PB      *pdq;	/* Pointer to the port block*/	
    PDQ_PRODUCER_INDEX *pi;	/* Producer Index block*/	

    unsigned int
    	reg;

#if PDQ_TRACE
    pprintf("\nEntering pdq_ServiceCmdRspQueue %s\n", ip->name);
#endif

    np  = (struct NI_GBL*)      ip->misc;
    pdq = (struct PDQ_PB*)      np->pbp;
    pi  = (PDQ_PRODUCER_INDEX*) pdq->pip;

    pi->CmdRspCompletionIndex = (pi->CmdRspCompletionIndex + 1) & 0x000F;
    pi->CmdRspProducer        = (pi->CmdRspProducer + 1)        & 0x000F;

    reg = pdq_read_csr(pdq, PDQ_CmdRspProd);

    /* PDQ_TYPE_1_PROD_M_COMP            0X00003F00  ~0xFFFFC0FF 
    /* PDQ_TYPE_1_PROD_M_PROD       	0X0000003F  ~0xFFFFFFC0 
    */
    reg &= ~PDQ_TYPE_1_PROD_M_COMP;	/* clear bits <13:8> */
    reg &= ~PDQ_TYPE_1_PROD_M_PROD;	/* clear bits <05:0> */

    /* PDQ_TYPE_1_PROD_V_COMP       8
    /* PDQ_TYPE_1_PROD_V_PROD       0
    */
    reg |= pi->CmdRspCompletionIndex << PDQ_TYPE_1_PROD_V_COMP;
    reg |= pi->CmdRspProducer        << PDQ_TYPE_1_PROD_V_PROD;

#if ENABLE_PRINTF
    pprintf("\n%s Service: CMD RESPONSE PRODUCER: %08x\n", ip->name, reg);
#endif

    pdq_write_csr(pdq, PDQ_CmdRspProd, reg); 	
    }



/*+
 * ============================================================================
 * =  pdq_ServiceCmdReqQueue - pdq_ServiceCmdReqQueue			      =
 * ============================================================================
 *
 * OVERVIEW:
 *  This function services the Command Request queue by simply 
 *  advancing the completion index to equal the consumer.
 *
 * ARGUMENTS:
 *	*ip - The Inode of Port Driver Calling this routine
 *
-*/
void pdq_ServiceCmdReqQueue (struct INODE *ip) {
    struct NI_GBL      *np;	/* Pointer to the NI global database*/
    struct PDQ_PB      *pdq;	/* Pointer to the port block*/	
    PDQ_PRODUCER_INDEX *pi;	/* Producer Index block*/	

    unsigned int lwbuf;
    unsigned int
    	reg;

#if PDQ_TRACE
    pprintf("\nEntering pdq_ServiceCmdReqQueue %s\n", ip->name);
#endif

    np  = (struct NI_GBL*)      ip->misc;
    pdq = (struct PDQ_PB*)      np->pbp;
    pi  = (PDQ_PRODUCER_INDEX*) pdq->pip;

    /* PDQ_CONS_M_RCV_INDEX     0X000000FF
    /*	cp.cmd_req tells us how many descriptors the adapter has 
    /*  consumed.  To acknowledge consumed descriptors, we must
    /*  write HOST_TX_CMD_PROD with the same value.
    */
    pdq_read_mem(pdq, &pdq->emp->cb.cmd_req, sizeof(PDQ_ULONG), &lwbuf);

    pi->CmdReqCompletionIndex = (lwbuf & PDQ_CONS_M_RCV_INDEX);

    reg = pdq_read_csr(pdq, PDQ_CmdReqProd);

    /* PDQ_TYPE_1_PROD_M_COMP            0X00003F00  ~0xFFFFC0FF 
    */
    reg &= ~PDQ_TYPE_1_PROD_M_COMP;		/* clear bits <13:8> */
    reg |= pi->CmdReqCompletionIndex << PDQ_TYPE_1_PROD_V_COMP;

#if ENABLE_PRINTF
    pprintf("\n%s Service: CMD REQUEST PRODUCER:  %08x\n", ip->name, reg);
#endif

    pdq_write_csr(pdq, PDQ_CmdReqProd, reg); 	
    };



/*+
 * ============================================================================
 * =  pdq_PostCmdReqDescriptor - pdq_PostCmdReqDescriptor			      =
 * ============================================================================
 *
 * OVERVIEW:
 * 	This function simply advances the Cmd Req Producer index by one.
 * 	The routine uses the host copies of the Producers and Completion 
 *	Indicees, cutting a read/modify write cycle in half.
 *
 * ARGUMENTS:
 *	*ip - The Inode of Port Driver Calling this routine
-*/
void pdq_PostCmdReqDescriptor (struct INODE  *ip) {
    struct NI_GBL      *np;	/* Pointer to the NI global database*/
    struct PDQ_PB      *pdq;	/* Pointer to the port block*/	
    PDQ_PRODUCER_INDEX *pi;	/* Producer Index block*/	

    unsigned int
    	reg=0;

#if PDQ_TRACE
    pprintf("\nEntering pdq_PostCmdReqDescriptor %s\n", ip->name);
#endif

    np  = (struct NI_GBL*)      ip->misc;
    pdq = (struct PDQ_PB*)      np->pbp;
    pi  = (PDQ_PRODUCER_INDEX*) pdq->pip;

    /* Using our local Host memory Producer index copy, PRODUCE one
    /* descriptor.  After doing so, TRUNCATE the Producer index count by
    /* ALWAYS forcing the index relative to zero when it gets greater 
    /* then 15 decimal.  This has an effect of wrapping the indexes to
    /* the beginning of the queue, ie [0].  
    /*				......MAGIC.....
    */
    pi->CmdReqProducer++;		/* advance the producer index */
    pi->CmdReqProducer &= 0x000F;	/* index range of <15:0> */

    /*	         31       1413        8 7   6 5        0
    /*	        +---------------------------------------+
    /* reg =	|  mbz      |Completion| mbz | Producer |
    /*	        +---------------------------------------+
    */
    reg  = pi->CmdReqCompletionIndex << PDQ_TYPE_1_PROD_V_COMP;	/* << 8 */
    reg |= pi->CmdReqProducer        << PDQ_TYPE_1_PROD_V_PROD;	/* << 0 */

#if PDQ_SERVICE_POST 
    pprintf("%s Post:    CMD REQUEST PRODUCER:  %08x\n", ip->name, reg);
#endif

    pdq_write_csr(pdq, PDQ_CmdReqProd, reg); 	
    };


/*+
 * ============================================================================
 * =  pdq_CheckCmdRspQueue - pdq_CheckCmdRspQueue				      =
 * ============================================================================
 *
 * OVERVIEW:
 * This function checks the Command Response Queue.  It returns TRUE if there 
 * is work to do on the queue (ie the completion index != consumer index). 
 * Else it returns FALSE if there is no work to do on the queue.
 *
 * RECEIVE QUEUE MANAGEMENT:
 * ------------------------
 *	The Port has sent the Host a Message(s) when its..
 *	 	Ports_CONSUMER_Index != Hosts_COMPLETION_Index.
 *
 * RETURNS:
 *
 * ARGUMENTS:
 *	*ip - The Inode of Port Driver Calling this routine
 *
-*/
int pdq_CheckCmdRspQueue (struct INODE *ip) {
    struct NI_GBL      *np;	/* Pointer to the NI global database*/
    struct PDQ_PB      *pdq;	/* Pointer to the port block*/	
    PDQ_PRODUCER_INDEX *pi;	/* Producer Index block*/	

    unsigned int
    	PortConsumer;
    unsigned int lwbuf;

#if PDQ_TRACE
    pprintf("\nEntering pdq_CheckCmdRspQueue %s\n", ip->name);
#endif

    np  = (struct NI_GBL*)      ip->misc;
    pdq = (struct PDQ_PB*)      np->pbp;
    pi  = (PDQ_PRODUCER_INDEX*) pdq->pip;

    /* Extract the Ports Consumer Command Request INdex 
    */
    pdq_read_mem(pdq, &pdq->emp->cb.cmd_rsp, sizeof(PDQ_ULONG), &lwbuf);

    PortConsumer = lwbuf & PDQ_CONS_M_RCV_INDEX;	/* 0X000000FF */

    /* The Port has been sending the Host Messages and...
    /*    There is work to be done on the Queue when...
    /*        Ports_Consumer_Index != Hosts_Completion_Index 
    */
    return (PortConsumer != pi->CmdRspCompletionIndex);
    }



/*+
 * ============================================================================
 * =  pdq_pollCmdRspQueue - pdq_pollCmdRspQueue				      = 
 * ============================================================================
 *
 * OVERVIEW:
 *
 * RETURNS:
 *
 * ARGUMENTS:
 *	*ip - The Inode of Port Driver Calling this routine
 *
-*/
int pdq_PollCmdRspQueue (struct INODE *ip) {
   unsigned int
    	i;

#if PDQ_TRACE
    pprintf("\nEntering pdq_PollCmdRspQueue %s\n", ip->name);
#endif

    /* Wait a maximum of 1 second for the Adapter to respond
    */
    for (i=0; i<1000; i++)
    	{
        krn$_sleep(1);			/* wait in 1ms increments */

    	/* Return with Success when Adapter responds with message
    	*/
    	if (pdq_CheckCmdRspQueue(ip))	
    	    {
     	    pdq_ServiceCmdReqQueue (ip);
    	    pdq_ServiceCmdRspQueue (ip);
    	    return msg_success;
    	    };
    	};    /* end for */

    /* Exit with Error if Adapter failed to respond
    */
    fprintf(el_fp, msg_pdq_msg6, ip->name);

    return msg_failure;
    };


/*+
 * ============================================================================
 * = pdq_StatusCharsGet - pdq_StatusCharsGet				      =
 * ============================================================================
 *
 * OVERVIEW:
 *	See PDQ FDDI Adapter Port Specification for command details.
 *
 * ARGUMENTS:
 *	*ip - The Inode of Port Driver Calling this routine
 *
 * RETURN VALUE:
 *	Returns a pointer to the response packet in Main Memory
 *
-*/
int pdq_StatusCharsGet (struct INODE  *ip) {
    struct NI_GBL     *np;	/* Pointer to the NI global database*/
    struct PDQ_PB     *pdq;	/* Pointer to the port block*/	

    PDQ_CMD_STATUS_CHARS_GET_REQ *cmd;
    PDQ_CMD_STATUS_CHARS_GET_RSP *rsp;

#if PDQ_PRINT_CHARSGET  
    char i;
#endif

#if PDQ_TRACE
    pprintf("\nEntering pdq_StatusCharsGet %s\n", ip->name);
#endif

    np  = (struct NI_GBL*)ip->misc;
    pdq = (struct PDQ_PB*)np->pbp;

    cmd = (PDQ_CMD_STATUS_CHARS_GET_REQ *) pdq->CmdReqBlock;
    rsp = (PDQ_CMD_STATUS_CHARS_GET_RSP *) pdq->CmdRspBlock;

    cmd->cmd_type = PDQ_CMD_K_STATUS_CHARS_GET; 

    pdq_write_mem(pdq, &pdq->emp->CmdReqBuf, sizeof(PDQ_CMD_STATUS_CHARS_GET_REQ), cmd);

    pdq_PostCmdReqDescriptor(ip);   	/* Give Command to Adapter */

    if (pdq_PollCmdRspQueue (ip))	/* Wait for Adapter to respond */
    	return msg_failure;

    /* The Adapter has Consummed and Responded to this Command.
    /* Check the returned message for status...
    */
    pdq_read_mem(pdq, &pdq->emp->CmdRspBuf, sizeof(PDQ_CMD_STATUS_CHARS_GET_RSP), rsp);

    if ((rsp->header.status   != PDQ_RSP_K_SUCCESS) ||
    	(rsp->header.cmd_type != PDQ_CMD_K_STATUS_CHARS_GET))
    	{
        fprintf(el_fp, msg_pdq_msg7, 
    			ip->name, rsp->header.status, rsp->header.cmd_type);
    	return msg_failure;
    	}


#if PDQ_PRINT_CHARSGET  
    pprintf("\nSTATUS CHARS GET\n\n");

    pprintf ("header.status: \t\t%x\n", 	rsp->header.status);
    pprintf ("header.cmd_type: \t\t%x\n", 	rsp->header.cmd_type);

    pprintf("station_type: \t\t%x\n", 	rsp->station_type);
    pprintf("smt_ver_id: \t\t%x\n", 	rsp->smt_ver_id);
    pprintf("smt_ver_id_max: \t\t%x\n", rsp->smt_ver_id_max);
    pprintf("smt_ver_id_min: \t\t%x\n", rsp->smt_ver_id_min);
    pprintf("station_state: \t\t%x\n", 	rsp->station_state);
    pprintf("link_addr.lwrd_0: \t\t%x\n", rsp->link_addr.lwrd_0);
    pprintf("link_addr.lwrd_1: \t\t%x\n", rsp->link_addr.lwrd_1);
    pprintf("t_req: \t\t%x\n", 		rsp->t_req);
    pprintf("tvx: \t\t%x\n", 		rsp->tvx);
    pprintf("token_timeout: \t\t%x\n", 	rsp->token_timeout);
    pprintf("purger_enb: \t\t%x\n", 	rsp->purger_enb);
    pprintf("link_state: \t\t%x\n", 	rsp->link_state);
    pprintf("tneg: \t\t%x\n", 		rsp->tneg);
    pprintf("dup_addr_flag: \t\t%x\n", 	rsp->dup_addr_flag);
    pprintf("una.lwrd_0: \t\t%x\n",	rsp->una.lwrd_0);
    pprintf("una.lwrd_1: \t\t%x\n",	rsp->una.lwrd_1);
    pprintf("una_old.lwrd_0: \t\t%x\n", rsp->una_old.lwrd_0);
    pprintf("una_old.lwrd_1: \t\t%x\n", rsp->una_old.lwrd_1);

    pprintf("un_dup_addr_flag: \t\t%x\n", rsp->un_dup_addr_flag);
    pprintf("dna.lwrd_0: \t\t%x\n",	rsp->dna.lwrd_0);
    pprintf("dna.lwrd_1: \t\t%x\n",	rsp->dna.lwrd_1);
    pprintf("dna_old.lwrd_0: \t\t%x\n", rsp->dna_old.lwrd_0);
    pprintf("dna_old.lwrd_1: \t\t%x\n", rsp->dna_old.lwrd_1);

    pprintf("purger_state: \t\t%x\n", 	rsp->purger_state);
    pprintf("fci_mode: \t\t%x\n", 	rsp->fci_mode);
    pprintf("error_reason: \t\t%x\n", 	rsp->error_reason);
    pprintf("loopback: \t\t%x\n", 	rsp->loopback);
    pprintf("ring_latency: \t\t%x\n", 	rsp->ring_latency);

    pprintf("last_dir_beacon_sa.lwrd_0: \t\t%x\n", rsp->last_dir_beacon_sa.lwrd_0);
    pprintf("last_dir_beacon_sa.lwrd_1: \t\t%x\n", rsp->last_dir_beacon_sa.lwrd_1);
    pprintf("last_dir_beacon_una.lwrd_0: \t\t%x\n", rsp->last_dir_beacon_una.lwrd_0);
    pprintf("last_dir_beacon_una.lwrd_1: \t\t%x\n", rsp->last_dir_beacon_una.lwrd_1);

    for (i=0; i<PDQ_PHY_K_MAX; i++)
        pprintf("phy_type[%d]: \t\t%x\n", i, 	rsp->phy_type[i]);

    for (i=0; i<PDQ_PHY_K_MAX; i++)
        pprintf("pmd_type[%d]: \t\t%x\n", i, 	rsp->pmd_type[i]);

    for (i=0; i<PDQ_PHY_K_MAX; i++)
        pprintf("lem_threshold[%d]: \t\t%x\n", i, rsp->lem_threshold[i]);

    for (i=0; i<PDQ_PHY_K_MAX; i++)
        pprintf("phy_state[%d]: \t\t%x\n", i, 	rsp->phy_state[i]);

    for (i=0; i<PDQ_PHY_K_MAX; i++)
        pprintf("nbor_phy_type[%d]: \t\t%x\n", i, rsp->nbor_phy_type[i]);

    for (i=0; i<PDQ_PHY_K_MAX; i++)
        pprintf("link_error_est[%d]: \t\t%x\n", i,rsp->link_error_est[i]);

    for (i=0; i<PDQ_PHY_K_MAX; i++)
        pprintf("broken_reason[%d]: \t\t%x\n", i, rsp->broken_reason[i]);

    for (i=0; i<PDQ_PHY_K_MAX; i++)
        pprintf("reject_reason[%d]: \t\t%x\n", i, rsp->reject_reason[i]);

    pprintf("cntr_interval: \t\t%x\n", 		rsp->cntr_interval);
    pprintf("module_rev: \t\t%x\n", 		rsp->module_rev);
    pprintf("firmware_rev: \t\t%x\n",		rsp->firmware_rev);
    pprintf("mop_device_type: \t\t%x\n", 	rsp->mop_device_type);

    for (i=0; i<PDQ_PHY_K_MAX; i++)
        pprintf("phy_led[%d]: \t\t%x\n", i, 	rsp->phy_led[i]);

    pprintf("flush_time: \t\t%x\n", 		rsp->flush_time);

#endif

    /* Return pointer to response packet
    */
    return rsp;
    };


/*+
 * ============================================================================
 * = pdq_CharsSet - pdq_CharsSet						      =
 * ============================================================================
 *
 * OVERVIEW:
 *	The chars_set command sets DECnet and misc characterics of the adapter
 *	and the FDDI link.  Appropriate fields are range chedked according to 
 *	table 33.  If any range check fails, no values are changed and the 
 *	command fails.
 *	
 * RETURNS:
 *
 * ARGUMENTS:
 *	*ip 	 - DEFEA Inode Pointer
 *	ItemCode - 
 *	port     -
 *
-*/
int pdq_CharsSet (struct INODE  *ip, int ItemCode, int value, int port) {
    struct NI_GBL     *np;	/* Pointer to the NI global database*/
    struct PDQ_PB     *pdq;	/* Pointer to the port block*/	

    PDQ_CMD_CHARS_SET_REQ *cmd;
    PDQ_CMD_CHARS_SET_RSP *rsp;

#if PDQ_TRACE
    pprintf("\nEntering pdq_CharsSet %s\n", ip->name);
#endif

    np  = (struct NI_GBL*)ip->misc;
    pdq = (struct PDQ_PB*)np->pbp;

    cmd = (PDQ_CMD_CHARS_SET_REQ *)pdq->CmdReqBlock;
    rsp = (PDQ_CMD_CHARS_SET_RSP *)pdq->CmdRspBlock;

    cmd->cmd_type           = PDQ_CMD_K_CHARS_SET;	/* %x3*/

    cmd->item[0].item_code  = ItemCode;		/* PDQ_ITEM_K_LOOPBACK_MODE %xE*/
    cmd->item[0].value      = value;		/* PDQ_ITEM_K_LOOPBACK_INT %x1*/
    cmd->item[0].item_index = port;		/* SAS=0, DAS_PortA=0 DAS_PortB=1*/

    cmd->item[1].item_code  = PDQ_ITEM_K_EOL;	/* End of ItemList */
    cmd->item[1].value      = 0;		/* ignored */
    cmd->item[1].item_index = 0;		/* ignored */

    pdq_write_mem(pdq, &pdq->emp->CmdReqBuf, sizeof(PDQ_CMD_CHARS_SET_REQ), cmd);

    pdq_PostCmdReqDescriptor(ip);   	/* Give Command to Adapter */

    if (pdq_PollCmdRspQueue (ip))	/* Wait for Adapter to respond */
    	return msg_failure;

    /* The Adapter has Consummed and Responded to this Command.
    /* Check the returned message for status...
    */
    pdq_read_mem(pdq, &pdq->emp->CmdRspBuf, sizeof(PDQ_CMD_CHARS_SET_RSP), rsp);

    if ((rsp->header.status   != PDQ_RSP_K_SUCCESS) ||
    	(rsp->header.cmd_type != PDQ_CMD_K_CHARS_SET))
    	{
        fprintf(el_fp, msg_pdq_msg8, pdq->name, rsp->header.status);
    	return msg_failure;
    	}

    return msg_success;
    };

#if CONTROL_T	

/*+
 * ============================================================================
 * = pdq_CountersGet - pdq_CountersGet					      =
 * ============================================================================
 *
 * OVERVIEW:
 *	The Counters_Get command reads the adapter's counters.  The counters
 * 	are the union of the counters that are required to suport DECNET and 
 *	SNMP.
 *
 * RETURNS:
 *	Pointer to the PDQ_CMD_CNTRS_GET_RSP buffer
 *
 * ARGUMENTS:
 *	*pdq	-	
 *
-*/
int pdq_CountersGet (struct INODE *ip) {
    struct NI_GBL     *np;	/* Pointer to the NI global database*/
    struct PDQ_PB     *pdq;	/* Pointer to the port block*/	

    PDQ_CMD_CNTRS_GET_REQ *cmd;
    PDQ_CMD_CNTRS_GET_RSP *rsp;

#if PDQ_TRACE
    pprintf("\nEntering pdq_CountersGet %s\n", ip->name);
#endif

    np  = (struct NI_GBL*)ip->misc;
    pdq = (struct PDQ_PB*)np->pbp;

    cmd = (PDQ_CMD_CNTRS_GET_REQ *) pdq->CmdReqBlock;
    rsp = (PDQ_CMD_CNTRS_GET_RSP *) pdq->CmdRspBlock;

    cmd->cmd_type = PDQ_CMD_K_CNTRS_GET;		/* %x5*/

    pdq_write_mem(pdq, &pdq->emp->CmdReqBuf, sizeof(PDQ_CMD_CNTRS_GET_REQ), cmd);

    pdq_PostCmdReqDescriptor(ip);   /* Give Command to Adapter */

    if (pdq_PollCmdRspQueue (ip))	/* Wait for Adapter to respond */
    	return msg_failure;

    /* The Adapter has Consummed and Responded to this Command.
    /* Check the returned message for status...
    */
    pdq_read_mem(pdq, &pdq->emp->CmdRspBuf, sizeof(PDQ_CMD_CNTRS_GET_RSP), rsp);

    if ((rsp->header.status   != PDQ_RSP_K_SUCCESS) || 
    	(rsp->header.cmd_type != PDQ_CMD_K_CNTRS_GET))
    	{
        fprintf(el_fp, msg_pdq_msg9, ip->name);
    	return msg_failure;
    	}

    return rsp;
    };
#endif


/*+
 * ============================================================================
 * = pdq_Start - pdq_Start						      =
 * ============================================================================
 *
 * OVERVIEW:
 * 	The START command directs the adapter to join the FDDI Ring.  If this
 * 	command completes successfully, the adapter transitions to the 
 * 	LINK_UNAVAILABLE state.  However, the driver must wait for the adapter
 * 	to change its state to LINK_AVAILABLE before posting frames on the Xmt
 * 	Data queue.  The adapter makes this transition automatically.
 *
 * RETURNS:
 *
 * ARGUMENTS:
 *	*ip     - The Inode of Port Driver Calling this routine
 *	ms_wait - ms times to wait for link_available, zero is forever
 *
-*/
int pdq_Start (struct INODE *ip, int ms_wait) {
    struct NI_GBL     *np;	/* Pointer to the NI global database*/
    struct PDQ_PB     *pdq;	/* Pointer to the port block*/	

    PDQ_CMD_START_REQ *cmd;	/* Start Command Structure */
    PDQ_CMD_START_RSP *rsp;	/* Start Command Response Structure */

    unsigned int
    	i=0;

#if PDQ_TRACE
    pprintf("\nEntering pdq_Start %s\n", ip->name);
#endif

    np  = (struct NI_GBL*)ip->misc;
    pdq = (struct PDQ_PB*)np->pbp;

    /* The sixteen Command Request  Descriptors point to the same 128 byte buffers.
    /* The sixteen Command Response Descriptors point to the same 128 byte buffers.
    /* We will reuse them over and over, so there is no need to malloc memory 
    /* for them.
    */
    cmd = (PDQ_CMD_START_REQ *) pdq->CmdReqBlock;
    rsp = (PDQ_CMD_START_RSP *) pdq->CmdRspBlock;

    cmd->cmd_type = PDQ_CMD_K_START;	/* Stuff it with the Command Start */

    pdq_write_mem(pdq, &pdq->emp->CmdReqBuf, sizeof(PDQ_CMD_START_REQ), cmd);

    pdq_PostCmdReqDescriptor(ip);   	/* Give Command to Adapter */

    if (pdq_PollCmdRspQueue(ip))		/* Wait for Adapter to respond */
    	return msg_failure;

    /* The Adapter has Consummed and Responded to this Command.
    /* Check the returned message for status...
    */
    pdq_read_mem(pdq, &pdq->emp->CmdRspBuf, sizeof(PDQ_CMD_START_RSP), rsp);

    if ((rsp->header.status   != PDQ_RSP_K_SUCCESS) ||
    	(rsp->header.cmd_type != PDQ_CMD_K_START))
    	{
        fprintf(el_fp, msg_pdq_msg10, ip->name);
    	return msg_failure;
    	}

    /* Wait forever or ms_waits, for the Adapter time to come on line.  
    */
    while (++i != ms_wait)
    	{
        krn$_sleep(10);			/* wait in 10ms increments */

    	if (pdq_GetAdapterState(pdq) == PDQ_STATE_K_LINK_AVAIL)
    	    return msg_success;		/* Command ok? return success */

    	if (killpending())
    	    return msg_failure;		/* return failure if oper kills us */

    	};    /* end for */

    
    fprintf(el_fp, msg_pdq_msg11, ip->name);
    return msg_failure;
    };


/*+
 * ============================================================================
 * = pdq_DMA_Init - pdq_DMA_Init						      =
 * ============================================================================
 *
 * OVERVIEW:
 *  This function issues the Port Control command DMA_init, which tells
 *  the adapter the base address of the 8k Aligned Descriptor Block. The 
 *  PHYSICAL address of the Descriptor Block is passed to the adapter in 
 *  PORT DATA A and PORT DATA B. The adapter is transitioned to the DMA 
 *  Available state upon successful completion of this command.
 *
 *  The +2 which appears in the write to PORT DATA A, is to set BYTE SWAP
 *  on DATA elements but not LITERAL elements. See the port and HW spec
 *  for further details on this messy issue.
 *
 * RETURNS:
 *
 * ARGUMENTS:
 *	*pdq  - 
-*/
int pdq_DMA_Init (struct PDQ_PB *pdq) {
    unsigned int
    	data;

#if PDQ_TRACE
    pprintf("\nEntering pdq_DMA_Init %s\n", pdq->name);
#endif

    /* The "+ 2", sets bit <1:1> and causes the Adapter to DMA data to/from the 
    /* Rcv and Xmt queues in the following manner:
    /*	[31]			 [0]
    /*	byte3	byte2	byte1	byte0
    /*
    /* With byte Swap disabled the Adapter DMA same data as follows:
    /*	[0]			 [31]
    /*	byte3	byte2	byte1	byte0
    */
    data = (unsigned int)(pdq->emp) + 2;  /* Descriptor Block base adrs, LW_bs_data enabled */
    pdq_write_csr(pdq, PDQ_PortDataA, data); 	
    pdq_write_csr(pdq, PDQ_PortDataB, 0); 	

    if (pdq_PortControlCommand(pdq, PDQ_PCTRL_M_INIT) != msg_success)
    	return msg_failure;

    if ((data = pdq_GetAdapterState(pdq)) != PDQ_STATE_K_DMA_AVAIL)
    	{
        fprintf(el_fp, msg_pdq_msg12, pdq->name, data);
    	return msg_failure;
    	}
    else
        return msg_success;             
    };


#if PDQ_FYI

/*+
 * ============================================================================
 * = pdq_SetDMABurstSize - pdq_SetDMABurstSize				      =
 * ============================================================================
 *
 * OVERVIEW:
 *  This function issues the Port Control command SUB_CMD, with the
 *  PORT_DATA_A sub command DMA Burst Size Set. This tells the adapter the
 *  number of longwords it may DMA in a single burst.
 *
 * RETURNS:
 *
 * ARGUMENTS:
 *	pdq	   -	
 *	burst_size _	PDQ programmed DMA burst size
 *
-*/
int pdq_SetDMABurstSize (struct PDQ_PB *pdq, unsigned int burst_size) {

#if PDQ_TRACE
    pprintf("\nEntering pdq_SetDMABurstSize %s\n", pdq->name);
#endif

    pdq_write_csr(pdq, PDQ_PortDataA, PDQ_SUB_CMD_K_BURST_SIZE_SET); 	
    pdq_write_csr(pdq, PDQ_PortDataB, burst_size); 	

    return (pdq_PortControlCommand (pdq, PDQ_PCTRL_M_SUB_CMD));
    };
#endif


#if PDQ_FYI

/*+
 * ============================================================================
 * = pdq_SetConsumerBlockBase - pdq_SetConsumerBlockBase			      =
 * ============================================================================
 *
 * OVERVIEW:
 * 	This function issues the Port Control command comsumer_block, which
 * 	tells the adapter the base of the consumer block. The PHYSICAL address
 * 	of the consumer block is passed to the adapter in PORT DATA A and
 * 	PORT DATA B.
 *
 * RETURNS:
 *
 * ARGUMENTS:
 *	*pdq - 
 *
-*/
int pdq_SetConsumerBlockBase (struct PDQ_PB *pdq) {

#if PDQ_TRACE
    pprintf("\nEntering pdq_SetConsumerBlockBase %s\n", pdq->name);
#endif


    pdq_write_csr(pdq, PDQ_PortDataA, (unsigned int)pdq->emp+pdq->cbp_offset);	
    pdq_write_csr(pdq, PDQ_PortDataB, 0);

    return (pdq_PortControlCommand (pdq, PDQ_PCTRL_M_CONS_BLOCK));
    };
#endif


/*+
 * ============================================================================
 * = pdq_softReset - pdq_softReset					      =
 * ============================================================================
 *
 * OVERVIEW:
 *  This function issues a soft reset to the Adapter. This is done by
 *  asserting the soft reset bit in PORT DATA A, and then asserting the
 *  RESET_ADAPTER in the PORT RESET register. The reset bit is held for
 *  approx one millisecond, and then released. Since SOFT reset
 *  bypasses power up self test, this function polls for about five seconds.
 *  If the adapter has not entered the DMA Unavilable state in that time,
 *  an error is returned. 
 *
 * ARGUMENTS:
 *	*pdq	-
 *
-*/
int pdq_softReset (struct PDQ_PB *pdq) {
   unsigned int
    	data;

#if PDQ_TRACE
    pprintf("\nEntering pdq_softReset %s\n", pdq->name);
#endif

    /* Port Data A register bit definitions (In Reset State ONly)
    /*	31:03	rsvd
    /*	02:02	Soft_reset however Skiping_Selftest
    /*	01:01	Soft_Reset
    /*	00:00	Upgrade
    */
    pdq_write_csr(pdq, PDQ_PortDataA, 4);	
    pdq_write_csr(pdq, PDQ_PortDataB, 0);	
    pdq_write_csr(pdq, PDQ_PortReset, 1);	

    /* Hold Reset for one Millisecond 
    */
    krn$_sleep(10);

    pdq_write_csr(pdq, PDQ_PortReset, 0);	

    /* Allow the Adaptor 5 seconds to reset before erroring
    */

    for (data=0; data<6500; data++) 
    	{
        krn$_sleep(1);			

    	if (pdq_GetAdapterState(pdq) == PDQ_STATE_K_DMA_UNAVAIL)
    	    return msg_success;
    	};    

    fprintf(el_fp, msg_pdq_msg13, pdq->name);

    return msg_failure;
    }


#if PDQ_FYI

/*+
 * ============================================================================
 * = pdq_init_CBlock - pdq_init_CBlock					      =
 * ============================================================================
 *
 * OVERVIEW:
 *	This routine initializes the Eisa Memory Mapped PDQ Consumer Block.
 *
 * RETURNS:
 *	none
 *
 * ARGUMENTS:
 *	*pdq	-	
-*/
void pdq_init_CBlock (struct PDQ_PB *pdq) {
    PDQ_CONSUMER_BLOCK cb;	/* Local copy of Consumer block*/	

#if PDQ_TRACE
    pprintf("\nEntering pdq_init_CBlock %s\n", pdq->name);
#endif

    cb.xmt_rcv_data = 0;
    cb.reserved_1   = 0;
    cb.smt_host     = 0;
    cb.reserved_2   = 0;
    cb.unsol        = 0;
    cb.reserved_3   = 0;
    cb.cmd_rsp      = 0;
    cb.reserved_4   = 0;
    cb.cmd_req      = 0;
    cb.reserved_5   = 0;

    pdq_write_mem(pdq, &pdq->emp->cb, sizeof(PDQ_CONSUMER_BLOCK), &cb);
    };
#endif


/*+
 * ============================================================================
 * = pdq_init_DBlock - Initialize PDQ Descriptor Block 			      =
 * ============================================================================
 *
 * OVERVIEW:
 *	This routine initializes the Eisa Memory Mapped PDQ Descriptor Block.
 *
 * ARGUMENTS:
 *	*pdq	-
 *
-*/
void pdq_init_DBlock (struct PDQ_PB *pdq) {

    unsigned int 
    	emp,
    	lwbuf,
    	i;

    PDQ_RCV_DESCR   
    	*rcv_ptr, 
    	*rcv_ptrr;

    PDQ_XMT_DESCR   
    	*xmt_ptr; 

#if PDQ_TRACE
    pprintf("\nEntering pdq_init_DBlock %s\n", pdq->name);
#endif

    /* There are 256 Transmit_Data and Receive_data Descriptors.  ALL PDQ
    /* Descriptors consists of two (2) Longwords.
    /*
    /* For now, point all Receiver_Data descriptors to the same 8k rcv buffer.
    /* Being a single segment of size 8192.  pdq_rx() will ndl_allocate()
    /* an IEEE frame and munged the received FDDI frame into a IEEE frame.
    /*
    /* Point all Transmit_data descriptors to the same 4k buffer. 
    /* Transmit IEEE frame will be munged into an FDDI frame by pdq_out().  
    /* The seg_len field will be defined by pdq_out() after munging.
    */
    rcv_ptr = pdq->emp->db.rcv_data;
    xmt_ptr = pdq->emp->db.xmt_data;
    emp     = (unsigned int)pdq->emp;

    for (i=0; i<256; i++)
    	{
    	/* Rcv Desc, seg_len = 8192 bytes, seg_cnt=0, SOB=1
    	*/
        lwbuf = 0x9F800000;		/* SOB=1, 8kbytes */
    	pdq_write_mem(pdq, &rcv_ptr[i].long_1,  sizeof(PDQ_ULONG), &lwbuf);
        lwbuf = emp + pdq->rcv_offset;
    	pdq_write_mem(pdq, &rcv_ptr[i].buff_lo, sizeof(PDQ_ULONG), &lwbuf);

    	/* Xmt Desc, We'll fill in the size in pdq_out(), SOB=1,EOB=1
    	/*  However we'll reserve a 4k buffer into which we'll munge the
    	/*  IEEE frame into an FDDI frame.
    	*/
        lwbuf = PDQ_K_SOB|PDQ_K_EOB;   	/* SOB=1, EOB=1, 0bytes */
        pdq_write_mem(pdq, &xmt_ptr[i].long_1,  sizeof(PDQ_ULONG), &lwbuf);
        lwbuf = emp + pdq->xmt_offset;
        pdq_write_mem(pdq, &xmt_ptr[i].buff_lo, sizeof(PDQ_ULONG), &lwbuf);
    	}

    rcv_ptr = pdq->emp->db.smt_host;

    /* SMT Descriptor queue
    */
    for (i=0; i<64; i++)
    	{
    	/* Rcv Desc, 0 bytes, seg_cnt=0, SOB=0
    	*/
        lwbuf = PDQ_K_SOB;   		/* SOB=1, 0bytes */
        pdq_write_mem(pdq, &rcv_ptr[i].long_1, sizeof(PDQ_ULONG), &lwbuf);
        lwbuf = 0;
        pdq_write_mem(pdq, &rcv_ptr[i].buff_lo, sizeof(PDQ_ULONG), &lwbuf);
    	}


    rcv_ptr  = pdq->emp->db.unsol;
    rcv_ptrr = pdq->emp->db.cmd_rsp;
    xmt_ptr  = pdq->emp->db.cmd_req;

    for (i=0; i<16; i++)
    	{
	/* UNSOL RESPONSE QUEUE a Rcv Descripor
    	*/
        lwbuf = 0x82000000;      		/* SOB=1, 512bytes */
        pdq_write_mem(pdq, &rcv_ptr[i].long_1, sizeof(PDQ_ULONG), &lwbuf);
        lwbuf = emp + pdq->unsol_offset;
        pdq_write_mem(pdq, &rcv_ptr[i].buff_lo, sizeof(PDQ_ULONG), &lwbuf);

	/* COMMAND RESPONSE QUEUE a Rcv Descripor
    	*/
    	lwbuf = 0x82000000;    			/* SOB=1, 512bytes */
    	pdq_write_mem(pdq, &rcv_ptrr[i].long_1, sizeof(PDQ_ULONG), &lwbuf);
        lwbuf = emp + pdq->CmdRsp_offset;
        pdq_write_mem(pdq, &rcv_ptrr[i].buff_lo, sizeof(PDQ_ULONG), &lwbuf);

    	/* COMMAND REQUEST QUEUE a Xmt Descriptor
    	*/
    	lwbuf = 0xC2000000;    			/* SOB=1, EOB=1, 512bytes */
        pdq_write_mem(pdq, &xmt_ptr[i].long_1, sizeof(PDQ_ULONG), &lwbuf);
        lwbuf = emp + pdq->CmdReq_offset;
        pdq_write_mem(pdq, &xmt_ptr[i].buff_lo, sizeof(PDQ_ULONG), &lwbuf);
    	}

    };


/*+
 * ============================================================================
 * = pdq_init_XmtQueues - pdq_init_XmtQueues				      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * This function initializes the Transmit queues. This is done by setting
 * the producer, consumer, and completion index for the XmtData and CmdReq
 * queues all to zero.
 *
 * RETURNS:
 *
 * ARGUMENTS:
 *	*pdq	-	
 *
-*/
void pdq_init_XmtQueues (struct PDQ_PB *pdq) {
    PDQ_PRODUCER_INDEX  *pi;

    unsigned int 
    	data;

#if PDQ_TRACE
    pprintf("\nEntering pdq_init_XmtQueues %s\n", pdq->name);
#endif

    pi   = (PDQ_PRODUCER_INDEX*)pdq->pip;

    /* Transmit Data Queue
    */
    data = pdq_read_csr(pdq, PDQ_Type2Prod);
    data &= ~PDQ_TYPE_2_PROD_M_XMT_DATA_COMP;      /* clear completed index */
    data &= ~PDQ_TYPE_2_PROD_M_XMT_DATA_PROD;      /* clear producer  index */

    pdq_write_csr(pdq, PDQ_Type2Prod, data);	
    pi->XmtDataCompletionIndex = 0;		/* Completion=0 Produce=0 */
    pi->XmtDataProducer        = 0;

    /* Command Request Queue
    */
    data = 0;					/* Completion=0 Produce=0 */
    pdq_write_csr(pdq, PDQ_CmdReqProd, 0); 	
    pi->CmdReqCompletionIndex = 0;
    pi->CmdReqProducer        = 0;

    };



/*+
 * ============================================================================
 * = pdq_init_RcvQueues - pdq_init_RcvQueues				      =
 * ============================================================================
 *
 * OVERVIEW:
 * This function initializes the Receive queues. This is done by setting
 * the consumer, and completion index for the XmtData and CmdReq
 * queues all to zero. The producer for each used queue is set to one, 
 * indicating one free buffer in each queue. The SMT queue is unused in
 * remote boot, so the producer is zeroed for that queue.
 *
 * RETURNS:
 *
 * ARGUMENTS:
 *	*pdq	-	FDDI Adapter INODE pointer 
 *
-*/
void pdq_init_RcvQueues (struct PDQ_PB *pdq) {
    PDQ_PRODUCER_INDEX  *pi;	/* Producer Index block*/	

    unsigned int 
    	data;

#if PDQ_TRACE
    pprintf("\nEntering pdq_init_RcvQueues %s\n", pdq->name);
#endif

    pi = (PDQ_PRODUCER_INDEX*)pdq->pip;

    /* Receive Data Queue
    */
    data = pdq_read_csr(pdq, PDQ_Type2Prod);
    data &= ~PDQ_TYPE_2_PROD_M_RCV_DATA_COMP;	/* Clear completion index */
    data &= ~PDQ_TYPE_2_PROD_M_RCV_DATA_PROD;	/* Clear Producer   index */

    /* Completion=0 Produce=1 Receive  Descriptor 
    */
    data |= 0x00000001;				/* produce one rcv desc */
    pdq_write_csr(pdq, PDQ_Type2Prod, data); 	
    pi->RcvDataCompletionIndex = 0;
    pi->RcvDataProducer        = 1;

    /* SMT Queue
    */
    pdq_write_csr(pdq, PDQ_HostSmtProd, 0); 	
    pi->SMTCompletionIndex = 0;
    pi->SMTProducer        = 0;

    /* Unsolicited Queue
    */
    pdq_write_csr(pdq, PDQ_UnsolProd, 15); 	
    pi->UnsolCompletionIndex = 0;
    pi->UnsolProducer        = 15;

    /*  Command Response Queue
    */
    pdq_write_csr(pdq, PDQ_CmdRspProd, 1); 	
    pi->CmdRspCompletionIndex = 0;
    pi->CmdRspProducer        = 1;
    };



/*+
 * ============================================================================
 * = pdq_setmode - Set mode                                                    =
 * ============================================================================
 *
 * OVERVIEW:
 * 	This routine is called by driver_setmode() to walk down the list 
 *	of FDDI Inodes in order to change the running state of ALL FDDI 
 *	drivers to either DDB$K_STOP or DDB$K_START.
 *  
 * FORM OF CALL:
 *	driver_setmode(mode)
 *	    (*ip->dva->setmode) (mode);
 *	   	pdq_setmode (mode)
 *  
 * RETURNS:
 *	msg_success or msg_failure 
 *       
 * ARGUMENTS:
 *	int mode - DDB$K_STOP	   DDB$K_START
 *
 * 	The Drivers Port Block saves the state of the Driver and are 
 *	as follows:
 *   		PDQ$K_UNINITIALIZED 0
 *		    The driver is set to PDQ$K_UNINITIALIZED during pdq_init_pb()
 *   		PDQ$K_STOPPED       1
 *		    The driver is set to PDQ$K_STOPPED by this routine.
 *		    This pdq->pb.state causes pdq_write() to return 0 bytes.
 *   		PDQ$K_STARTED       2
 *		    The driver is set to PDQ$K_STARTED during pdq_start_PDQ().
 *		    This pdq->pb.state causes pdq_rx() to poll for receive frames.
-*/
void pdq_setmode (struct PDQ_PB *pdq, int mode) {

#if PDQ_TRACE
    pprintf("\nEntering pdq_setmode\n");
#endif

    if (mode == DDB$K_STOP) 
	pdq_stop_driver(pdq->ip);

    if (mode == DDB$K_START) 
	pdq_restart_driver(pdq->ip);	    	    	

    }


/*+
 * ============================================================================
 * = pdq_init_dl - Initialization the NI Data Link			      =
 * ============================================================================
 *
 * OVERVIEW:
 *	This routine initializes those NI Data Link Port Block entries.
 *
 * RETURNS:
 *
 * ARGUMENTS:
 *	*ip	-	FDDI Adapter INODE pointer 
 *
-*/
int pdq_init_dl (struct INODE *ip) {
    struct PDQ_PB           *pdq;	/* Pointer to the port block */	
    struct NI_GBL           *np;	/* Pointer to the NI global database */
    struct PORT_CALLBACK    *cb;	/* Callback structure */
    struct MOP$_V4_COUNTERS *mc;	/* Pointer to the MOP counters */	

#if PDQ_TRACE
    pprintf("\nEntering pdq_init_dl %s\n", ip->name);
#endif

    np      = (struct NI_GBL*)          ip->misc;
    np->dlp = (int*) malloc(sizeof(struct NI_DL));
    np->mcp = (int*) malloc(sizeof(struct MOP$_V4_COUNTERS));
    pdq     = (struct PDQ_PB*)          np->pbp;
    mc      = (struct MOP$_V4_COUNTERS*)np->mcp;

    /* Setup the Ports Callback structure
    */
    cb     = (struct PORT_CALLBACK*) malloc(sizeof(struct PORT_CALLBACK)); 	
    cb->st = null_procedure;		/* control_t */
    cb->cm = null_procedure;		/* Change the mode */
    cb->ri = null_procedure;		/* Reinitialize */
    cb->re = null_procedure;		
    cb->dc = pdq_dump_csrs;		/* dump csrs */

    /* Initialize the NI_DataLink, see NI_DATALINK.C
    /*	ndl_init() will init 
    /*  	*rp = (int)ndl_receive_msg
    */
    ndl_init(ip, cb, pdq->sa, (int *)&(pdq->lrp));

    /* Initialize the mop counters
    */
    init_mop_counters(mc);	/* see NI_DATALINK.C */

    return msg_success;
    }



/*+
 * ============================================================================
 * = pdq_GetSid - Get Station ID						      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * RETURNS:
 *
 * ARGUMENTS:
 *	*ip	-	FDDI Adapter INODE pointer 
 *
-*/
void pdq_GetSid (struct INODE *ip) {

    PDQ_CMD_STATUS_CHARS_GET_RSP *rsp;
    struct NI_GBL *np;
    struct MB *mbp;
    char      
    	i,
    	*sid_ptr;

#if PDQ_TRACE
    pprintf("\nEntering pdq_GetSid %s\n", ip->name);
#endif

    np      = (struct NI_GBL*)ip->misc;
    mbp     = (struct MB*)np->mbp;

    rsp = (PDQ_CMD_STATUS_CHARS_GET_RSP *)pdq_StatusCharsGet(ip);

    sid_ptr = &rsp->station_id.octet_7_4;

    for (i=0; i<8; i++)
    	mbp->sid[i] = sid_ptr[i];

#if ENABLE_PRINTF
    pprintf("Station_id.octet_7_4: %x Station_id.octet_3_0 %x\n",
    		rsp->station_id.octet_7_4, rsp->station_id.octet_3_0);
#endif

    }
    	


/*+
 * ============================================================================
 * = pdq_start_PDQ - Start PDQ DMA controler			      =
 * ============================================================================
 *
 * OVERVIEW:
 *	A PROCESS to complete the initialization of a DEFEA eisa/fddi adapter
 *	module pointed to by *ip.  A PROCESS is needed to allow for 
 *	initialization and connection of point-to-point configured eisa/fddi 
 *	adapters on the same system.
 *
 * RETURNS:
 *
 * ARGUMENTS:
 *	*ip	 -	FDDI Adapter INODE pointer 
-*/
int pdq_start_PDQ (struct INODE *ip) {
    PDQ_CONSUMER_BLOCK cb;	/* Local copy of Consumer block*/	
    struct NI_GBL *np;		/* Pointer to the NI global database*/
    struct PDQ_PB *pdq;		/* Pointer to the port block*/	

    char name[20];       	/*General holder for a name*/

    int data;

#if PDQ_TRACE
    pprintf("\nEntering pdq_start_PDQ %s\n", ip->name);
#endif

    np  = (struct NI_GBL*)ip->misc;
    pdq = (struct PDQ_PB*)np->pbp;

    if (pdq_softReset(pdq))
    	return msg_failure;

    /* FDDI ADAPTER NET STATION ADDRESS:
    /*	Get the station address and save it in the Port Block.  A later
    /*	call to "ndl_init(,,,pdq->sa)" will also save the station address
    /*  in "memcpy(dl->sa, pdq->sa, 6)".  From this, the ni_datalink can fill
    /*  the IEEE frames with "xmt_frame->source_addr" information.
    /*
    /* Tell the FDDI Adapter to put its LOW Station Address 
    /* into its HOST_DATA register.  Then read HOST_DATA reg.
    */
    pdq_write_csr(pdq, PDQ_PortDataA, PDQ_PDATA_A_MLA_K_LO); 	
    pdq_write_csr(pdq, PDQ_PortDataB, 0); 	
    pdq_PortControlCommand(pdq, PDQ_PCTRL_M_MLA);
    data = pdq_read_csr(pdq, PDQ_HostData);
    *(unsigned long*)(&(pdq->sa[0])) = pdq_read_csr(pdq, PDQ_HostData);

    /* Tell the FDDI Adapter to put its Station Address HIGH
    /* into its HOST_DATA register. Then read HOST_DATA reg.
    */
    pdq_write_csr(pdq, PDQ_PortDataA, PDQ_PDATA_A_MLA_K_HI); 	
    pdq_write_csr(pdq, PDQ_PortDataB, 0); 	
    pdq_PortControlCommand(pdq, PDQ_PCTRL_M_MLA);
    *(unsigned long*)(&(pdq->sa[4])) = pdq_read_csr(pdq, PDQ_HostData);

    strcpy(pdq->pb.name, ip->name);

    set_io_alias(pdq->pb.alias, 0, 0, pdq);

    sprintf(pdq->pb.info, "%02X-%02X-%02X-%02X-%02X-%02X",
    			pdq->sa[0], pdq->sa[1], pdq->sa[2],
    			pdq->sa[3], pdq->sa[4], pdq->sa[5]);

    /* Enable DMA transactions 
    */
    if (pdq->pb.type == TYPE_PCI)
	pdq_write_csr(pdq, PFI_ModeControl, 1);

    /* Init the Descriptor and Consumer Blocks 
    */
    cb.xmt_rcv_data = 0;
    cb.reserved_1   = 0;
    cb.smt_host     = 0;
    cb.reserved_2   = 0;
    cb.unsol        = 0;
    cb.reserved_3   = 0;
    cb.cmd_rsp      = 0;
    cb.reserved_4   = 0;
    cb.cmd_req      = 0;
    cb.reserved_5   = 0;
    pdq_write_mem(pdq, &pdq->emp->cb, sizeof(PDQ_CONSUMER_BLOCK), &cb);

    pdq_init_DBlock (pdq);		/* Descriptor Block */

    /* Init the Xmt and Rcv Queues
    */
    pdq_init_XmtQueues(pdq);		/* XmtData, CmdReq*/
    pdq_init_RcvQueues(pdq);		/* RcvData, SMT, Unsol, CmdRsp*/

#if PDQ_DEBUG
    pdq_dump_mem(pdq);
#endif

    /* Disable Interrupts 
    */
    pdq_write_csr(pdq, PDQ_HostIntEnbX, 0); 	

#if ENABLE_PRINT_SA
    print_enet_address(ip->name, pdq->sa);      /* see NI_DATALINK.C */
#endif

    /* Tell the DEFEA the Consumer Block base address
    */
    pdq_write_csr(pdq, PDQ_PortDataA, (unsigned int)pdq->emp+pdq->cbp_offset); 	
    pdq_write_csr(pdq, PDQ_PortDataB, 0); 	

    if (pdq_PortControlCommand (pdq, PDQ_PCTRL_M_CONS_BLOCK))
    	return msg_failure;

    /* Set the DMA Burst size to 8 longwords 
    */
    pdq_write_csr(pdq, PDQ_PortDataA, PDQ_SUB_CMD_K_BURST_SIZE_SET); 	
    pdq_write_csr(pdq, PDQ_PortDataB, PDQ_PDATA_B_DMA_BURST_SIZE_8); 	

    if (pdq_PortControlCommand (pdq, PDQ_PCTRL_M_SUB_CMD))
    	return msg_failure;

    /* Tell the DEFEA the Descriptor Block base address, also
    /* transition the adapter to DMAavailable state.
    */
    if (pdq_DMA_Init(pdq))
    	return msg_failure;

    /* Clears all pending interrupts
    */
    pdq_write_csr(pdq, PDQ_HostIntType0, 0xFFFFFFFF);	

    /* Issue start command.  Forever wait for the adapter to transitions 
    /* to Link_Available.
    */
    pdq_Start(ip, 600);

    /* Put the Adapter's Port 0, into "Internal Loopback Mode" if enabled
    */
    if (pdq->loopback)
    	{
        if (pdq_CharsSet(ip, PDQ_ITEM_K_LOOPBACK_MODE, PDQ_ITEM_K_LOOPBACK_EXT, 	0))
    	    return msg_failure;

    	pdq_StatusCharsGet(ip);
    	}

    /* Fill in the Port Block Structure, see cp$src:PB_DEF.H
    /* Done for setshow.c.  See below comment.
    */
    pdq->pb.channel    = 0;
    pdq->pb.mode       = DDB$K_POLLED;

    /* System Block pointer and number
    */
    pdq->pb.sb     = 0;
    pdq->pb.num_sb = 0;
    sprintf(pdq->pb.string, "%-24s   %-8s   %24s", 
    			pdq->pb.name, pdq->pb.alias, pdq->pb.info);


    /* Initialize the DATA LINK struture if the Port was not
    /* already started ie PDQ$K_UNINITIALIZED.  If the Port was
    /* initialized then stopped, ie PDQ$K_STOPPED, then there is no
    /* need to init the data link again.
    */
    if (pdq->pb.state == PDQ$K_UNINITIALIZED)
        pdq_init_dl(ip);
    else
        /* Done to allow -stop/-start to work.
        /* Allows the pdq_rx process to start successfully.
        */
        krn$_sleep(1000);
 

    /* moved here to allow mop rx counters to work...
    /* Start the transmit receive processes.  Bind them to a processor 
    /* so we can syncronize by only raising ipl.
    */
    sprintf(name, "rx_%4.4s", ip->name);
    krn$_create (pdq_rx, null_procedure,
    	0, 5, -1, 1024*4, name, "tt", "r", "tt", "w", "tt", "w", ip);

    /* Get the Station ID for MOP
    */
    pdq_GetSid(ip);

    pdq->pb.state = PDQ$K_STARTED;	/* Tell world PDQ is started */

#if PDQ_DEBUG
    qprintf(msg_port_inited, ip->name);
#endif

    return msg_success;
    };




/*+
 * ============================================================================
 * = pdq_malloc_descr - Malloc PDQ Host Shared Memory 			      =
 * ============================================================================
 *
 * OVERVIEW:
 *	Mallocs dynamic memory for the PDQ DMA controller 
 *
 * RETURNS:
 *	msg_success or msg_failure 
 *
 * ARGUMENTS:
 *	*ip	-	FDDI Adapter INODE pointer 
 *
-*/
int pdq_malloc_descr (struct INODE *ip) {
    struct NI_GBL *np;		/* Pointer to the NI global database*/
    struct PDQ_PB *pdq;		/* Pointer to the port block*/	
    struct NI_ENV *NI_ev;	/* Pointer to the environment variables*/	

#if PDQ_TRACE
    pprintf("\nEntering pdq_malloc_descr %s\n", ip->name);
#endif

    np    = (struct NI_GBL*)ip->misc;
    pdq   = (struct PDQ_PB*)np->pbp;
    NI_ev = (struct NI_ENV*)np->enp;

    /* Where IEEE Packets are translated into FDDI Packets.
    /* Where PDQ Commands/Response come/go to
    */
    pdq->XmtDatBlock = dyn$_malloc (4096,
                                DYN$K_SYNC|DYN$K_NOOWN|DYN$K_FLOOD);
    pdq->CmdRspBlock = dyn$_malloc (512,
                                DYN$K_SYNC|DYN$K_NOOWN|DYN$K_FLOOD|DYN$K_ALIGN, 128, 0);
    pdq->CmdReqBlock = dyn$_malloc (512,
                                DYN$K_SYNC|DYN$K_NOOWN|DYN$K_FLOOD);


    /* Malloc the Hosts Copy, of the PDQ Producer Index Register.
    /* No byte alignment requirments.
    */
    pdq->pip = (void*) dyn$_malloc (sizeof (PDQ_PRODUCER_INDEX), 
    					DYN$K_SYNC|DYN$K_NOOWN|
    					DYN$K_FLOOD|DYN$K_ALIGN,
    					64,
    					0);

    /* Calculate the DEFEA On-Board Eisa Mapped Memory offsets.
    */
    pdq->rcv_offset    = sizeof(PDQ_DESCR_BLOCK);

    pdq->CmdRsp_offset = pdq->rcv_offset + 8192;

    pdq->CmdReq_offset = pdq->CmdRsp_offset + 512;

    pdq->unsol_offset  = pdq->CmdReq_offset +  512;

    pdq->cbp_offset    = pdq->unsol_offset + 512; 

    pdq->xmt_offset    = pdq->cbp_offset + sizeof(PDQ_CONSUMER_BLOCK) + 1624;

    return msg_success;
    }



/*+
 * ============================================================================
 * = pdq_stop_driver - Stop the FDDI Port Driver.                              =
 * ============================================================================
 *
 * OVERVIEW:
 *	This routine will change the state of the FDDI Port Driver, pointed to
 *	by its argument "struct INODE *ip", to the DDB$STOP state.  By changing
 *	its state to DDB$STOP, will cause the associated pdq_rx process to exit 
 *	and for the pdq_write routine to return a byte xmitted count of zero.
 *
 * RETURNS:
 *	msg_success or msg_failure 
 *
 * ARGUMENTS:
 *
 *	*ip - An Inode on the pdqip.flink queue
 *
-*/
int pdq_stop_driver (struct INODE *ip) {
    struct NI_GBL *np;
    struct PDQ_PB *pdq;

#if PDQ_TRACE
    pprintf("\nEntering pdq_stop_driver %s\n", ip->name);
#endif

    /* Get some pointers
    */
    np   = (struct NI_GBL*)ip->misc;
    pdq  = (struct PDQ_PB*) np->pbp;

#if ENABLE_PRINTF
    pprintf("Stopping ip->name %s\n",  ip->name);
    pprintf("Stopping pdq->name %s\n", pdq->name);
#endif

    /* Just return if already stopped 
    */
    if (pdq->pb.state == PDQ$K_STOPPED)
    	return(msg_success);

    /* Just return if unitialized 
    */
    if (pdq->pb.state == PDQ$K_UNINITIALIZED)
    	return(msg_failure);

    /* Change the state of this Port Driver to PDQ$K_STOPPED then wait 
    /* for its associated pdq_rx receive process to exit before reseting 
    /* the FDDI Adapter.
    */
    pdq->pb.state = PDQ$K_STOPPED;

    if (pdq_softReset(pdq))
    	return msg_failure;

    pdq_free_rcv_pkts(ip);	/* Purge the CallBack Receive Packet queue */

    if (pdq->pb.type == TYPE_EISA)
	pdq_write_csr_byte(pdq, ESIC_FuncCntrl, 0); 

    /* Return a success
    */
    return(msg_success);
    }


/*+
 * ============================================================================
 * = pdq_restart_driver - Restarts the FDDI Port Driver.                       =
 * ============================================================================
 *
 * OVERVIEW:
 *	The Inode passed to this routine is on the queue of all FDDI 
 *	allocinodes, ie pdqip.flink.  These Inodes at one time were 
 *	started via pdq_init() and then stopped via pdq_stopped_driver().  
 *	This routine will then restart the FDDI Port Driver denoted by its 
 *	argument "struct INODE *ip".
 *
 * RETURNS:
 *	msg_success or msg_failure 
 *
 * ARGUMENTS:
 *
 *	struct INODE *ip - An Inode on the pdqip.flink queue
 *
-*/
int pdq_restart_driver (struct INODE *ip) {
    struct NI_GBL *np;		/*Pointer to the NI global database*/
    struct PDQ_PB *pdq;		/*Pointer to the port block*/	

#if PDQ_TRACE
    pprintf("\nEntering pdq_restart_driver %s\n", ip->name);
#endif

    /* Get some pointers
    */
    np  = (struct NI_GBL*)ip->misc;
    pdq = (struct PDQ_PB*) np->pbp;

    /* Just return if already started
    */
    if (pdq->pb.state == PDQ$K_STARTED)
    	return(msg_success);

    /* Only allowed to restart the Driver from the Stopped state.
    /* Return with error if the Driver is uninitialized.
    */
    if (pdq->pb.state == PDQ$K_UNINITIALIZED)
    	return(msg_failure);

    if (pdq->pb.type == TYPE_EISA)
	pdq_bus_init_eisa(pdq);

    /* Stop_driver sets pdq->pb.state to PDQ$K_STOPPED
    /*  The receive process will exit when pdq->pb.state==PDQ$K_STOPPED
    /*
    /* pdq->pb.state must be set to PDQ$K_RESTARTED allowing the receive 
    /* process to NOT exit when restarted.
    */
    pdq->pb.state = PDQ$K_RESTARTED;
 
    pdq_start_PDQ(ip);

    /* Return a success
    */
    return(msg_success);
    }



#if CONTROL_T	
/*+
 * ============================================================================
 * = pdq_control_t - Control T CALLBACK ROUTINE 			      =
 * ============================================================================
 *
 * OVERVIEW:
 *	This routine is used to display Port and Device counters in real time.
 *
 * RETURNS:
 *	None
 *
 * ARGUMENTS:
 *	*np 	- 	Pointer to the NI global database.
 *
 *
-*/
void pdq_control_t (struct NI_GBL *np) {
    struct FILE 	 *fp;		/* FDDI Port Driver */
    struct PDQ_PB	 *pdq;		/* Pointer to the port block*/	
    PDQ_CMD_CNTRS_GET_RSP *rsp;		/* CountersGet Command Response */


    /* Get the pointer to the port block
    */
    pdq = (struct PDQ_PB*)   np->pbp;

#if PDQ_TRACE
    pprintf("\nEntering pdq_control_t\n", pdq->name);
#endif

    /* Can't do Control_t until the Port has started
    */
    if (pdq->pb.state != PDQ$K_STARTED)
    	{
        err_printf(msg_pdq_msg14, pdq->name);	
    	return;
    	}

    /* fopen a FDDI Port Driver
    */
    fp  = (struct FILE *)fopen(pdq->name, "r+");

    rsp = pdq_CountersGet (fp->ip);

    /* DEVICE SPECIFIC:
    */
    printf(msg_ez_inf_dev_spec);
    printf("time_since_reset: %08x%08x\n", rsp->time_since_reset.ms, rsp->time_since_reset.ls);
    /* Station/SMT Counters
    */
    printf("\n Station/SMT Counters\n");	
    printf("time_since_reset: %08x%08x\n", rsp->cntrs.traces_rcvd.ms, rsp->cntrs.traces_rcvd.ls);
    /* Link/MAC Counters
    */
    printf("\n Link/MAC Counters\n");	

    printf("frame_cnt: %08x%08x\n", rsp->cntrs.frame_cnt.ms, rsp->cntrs.frame_cnt.ls);
    printf("error_cnt: %08x%08x\n", rsp->cntrs.error_cnt.ms, rsp->cntrs.error_cnt.ls);
    printf("lost_cnt: %08x%08x\n", rsp->cntrs.lost_cnt.ms, rsp->cntrs.lost_cnt.ls);
    printf("octets_rcvd: %08x%08x\n", rsp->cntrs.octets_rcvd.ms, rsp->cntrs.octets_rcvd.ls);
    printf("octets_sent: %08x%08x\n", rsp->cntrs.octets_sent.ms, rsp->cntrs.octets_sent.ls);
    printf("pdus_rcvd: %08x%08x\n", rsp->cntrs.pdus_rcvd.ms, rsp->cntrs.pdus_rcvd.ls);
    printf("pdus_sent: %08x%08x\n", rsp->cntrs.pdus_sent.ms, rsp->cntrs.pdus_sent.ls);
    printf("mcast_octets_rcvd: %08x%08x\n", rsp->cntrs.mcast_octets_rcvd.ms, rsp->cntrs.mcast_octets_rcvd.ls);
    printf("mcast_octets_sent: %08x%08x\n", rsp->cntrs.mcast_octets_sent.ms, rsp->cntrs.mcast_octets_sent.ls);
    printf("mcast_pdus_rcvd: %08x%08x\n", rsp->cntrs.mcast_pdus_rcvd.ms, rsp->cntrs.mcast_pdus_rcvd.ls);
    printf("mcast_pdus_sent: %08x%08x\n", rsp->cntrs.mcast_pdus_sent.ms, rsp->cntrs.mcast_pdus_sent.ls);
    printf("xmt_underruns: %08x%08x\n", rsp->cntrs.xmt_underruns.ms, rsp->cntrs.xmt_underruns.ls);
    printf("xmt_failures: %08x%08x\n", rsp->cntrs.xmt_failures.ms, rsp->cntrs.xmt_failures.ls);
    printf("block_check_errors: %08x%08x\n", rsp->cntrs.block_check_errors.ms, rsp->cntrs.block_check_errors.ls);
    printf("frame_status_errors: %08x%08x\n", rsp->cntrs.frame_status_errors.ms, rsp->cntrs.frame_status_errors.ls);
    printf("pdu_length_errors: %08x%08x\n", rsp->cntrs.pdu_length_errors.ms, rsp->cntrs.pdu_length_errors.ls);
    printf("rcv_overruns: %08x%08x\n", rsp->cntrs.rcv_overruns.ms, rsp->cntrs.rcv_overruns.ls);
    printf("user_buff_unavailable: %08x%08x\n", rsp->cntrs.user_buff_unavailable.ms, rsp->cntrs.user_buff_unavailable.ls);
    printf("inits_initiated: %08x%08x\n", rsp->cntrs.inits_initiated.ms, rsp->cntrs.inits_initiated.ls);
    printf("inits_rcvd: %08x%08x\n", rsp->cntrs.inits_rcvd.ms, rsp->cntrs.inits_rcvd.ls);
    printf("beacons_initiated: %08x%08x\n", rsp->cntrs.beacons_initiated.ms, rsp->cntrs.beacons_initiated.ls);
    printf("dup_addrs: %08x%08x\n", rsp->cntrs.dup_addrs.ms, rsp->cntrs.dup_addrs.ls);
    printf("dup_tokens: %08x%08x\n", rsp->cntrs.dup_tokens.ms, rsp->cntrs.dup_tokens.ls);
    printf("purge_errors: %08x%08x\n", rsp->cntrs.purge_errors.ms, rsp->cntrs.purge_errors.ls);
    printf("fci_strip_errors: %08x%08x\n", rsp->cntrs.fci_strip_errors.ms, rsp->cntrs.fci_strip_errors.ls);
    printf("traces_initiated: %08x%08x\n", rsp->cntrs.traces_initiated.ms, rsp->cntrs.traces_initiated.ls);
    printf("directed_beacons_rcvd: %08x%08x\n", rsp->cntrs.directed_beacons_rcvd.ms, rsp->cntrs.directed_beacons_rcvd.ls);
    printf("emac_frame_alignment_errors: %08x%08x\n", rsp->cntrs.emac_frame_alignment_errors.ms, rsp->cntrs.emac_frame_alignment_errors.ls);

    /* Phy/PORT Counters
    */
    printf("\n Link/MAC Counters\n");	
    printf("ebuff_errors[PHY_A]: %08x%08x\n", rsp->cntrs.ebuff_errors[0].ms, rsp->cntrs.ebuff_errors[1].ls);
    printf("ebuff_errors[PHY_B]: %08x%08x\n", rsp->cntrs.ebuff_errors[0].ms, rsp->cntrs.ebuff_errors[1].ls);

    printf("lct_rejects[PHY_A]: %08x%08x\n", rsp->cntrs.lct_rejects[0].ms, rsp->cntrs.lct_rejects[1].ls);
    printf("lct_rejects[PHY_B]: %08x%08x\n", rsp->cntrs.lct_rejects[0].ms, rsp->cntrs.lct_rejects[1].ls);

    printf("lem_rejects[PHY_A]: %08x%08x\n", rsp->cntrs.lem_rejects[0].ms, rsp->cntrs.lem_rejects[1].ls);
    printf("link_errors[PHY_B]: %08x%08x\n", rsp->cntrs.lem_rejects[0].ms, rsp->cntrs.lem_rejects[1].ls);

    printf("link_errors[PHY_A]: %08x%08x\n", rsp->cntrs.link_errors[0].ms, rsp->cntrs.link_errors[1].ls);
    printf("link_errors[PHY_B]: %08x%08x\n", rsp->cntrs.link_errors[0].ms, rsp->cntrs.link_errors[1].ls);

    printf("connections[PHY_A]: %08x%08x\n", rsp->cntrs.connections[0].ms, rsp->cntrs.connections[1].ls);
    printf("connections[PHY_B]: %08x%08x\n", rsp->cntrs.connections[0].ms, rsp->cntrs.connections[1].ls);

    /* PORT INFO:
    printf(msg_ez_inf_003);		
    printf("XmtFrames: %d RcvFrames: %d\n", pdq->xmt_fms, pdq->rcv_fms);
    printf("XmtBytes: %d RcvBytes: %d\n", pdq->xmt_byt, pdq->rcv_byt);
    printf("XmtAvg: %d RcvAvg: %d\n", pdq->xmt_avg, pdq->rcv_avg);
    */ 

    fclose(fp);
    }
#endif



#if PDQ_MAP

/*+
 * ============================================================================
 * = pdq_print_map - Prints the DEFEA Eisa Memory Map
 * ============================================================================
 *
 * OVERVIEW:
 *	Dumps the Physical Address for Eisa Memory Mapped PDQ Descriptors 
 *	and things.
 *
 * ARGUMENTS:
 *	*pdq -	DEFEA Port Block Pointer
 *
-*/
void pdq_print_map (struct PDQ_PB *pdq) {

#if PDQ_TRACE
    pprintf("\nEntering pdq_print_map %s\n", pdq->name);
#endif

    pprintf("\nDescriptor Block Address MAP\n");
    pprintf(" %s &pdq->emp->db.rcv_data: %x\n", pdq->name,  &pdq->emp->db.rcv_data);
    pprintf(" %s &pdq->emp->db.xmt_data: %x\n", pdq->name, &pdq->emp->db.xmt_data);
    pprintf(" %s &pdq->emp->db.smt_host: %x\n", pdq->name,  &pdq->emp->db.smt_host);
    pprintf(" %s &pdq->emp->db.unsol   : %x\n", pdq->name,  &pdq->emp->db.unsol);
    pprintf(" %s &pdq->emp->db.cmd_rsp : %x\n", pdq->name,  &pdq->emp->db.cmd_rsp);
    pprintf(" %s &pdq->emp->db.cmd_req : %x\n", pdq->name,  &pdq->emp->db.cmd_req);

    pprintf("\nConsumer Block Address MAP\n");
    pprintf(" %s &pdq->emp->cb.xmt_rcv_data: %x\n", pdq->name,  &pdq->emp->cb.xmt_rcv_data);
    pprintf(" %s &pdq->emp->cb.smt_host    : %x\n", pdq->name,  &pdq->emp->cb.smt_host);
    pprintf(" %s &pdq->emp->cb.unsol       : %x\n", pdq->name,  &pdq->emp->cb.unsol);
    pprintf(" %s &pdq->emp->cb.cmd_rsp     : %x\n", pdq->name,  &pdq->emp->cb.cmd_rsp);
    pprintf(" %s &pdq->emp->cb.cmd_req     : %x\n", pdq->name,  &pdq->emp->cb.cmd_req);

    pprintf("\nTransmit Data Buffer Address MAP\n");
    pprintf(" %s &pdq->emp->XmtBuf: %x\n", pdq->name,  &pdq->emp->XmtBuf);

    pprintf("\nReceive Data Buffer Address MAP\n");
    pprintf(" %s &pdq->emp->RcvBuf: %x\n", pdq->name,  &pdq->emp->RcvBuf);

    pprintf("\nCommand Response Buffer Address MAP\n");
    pprintf(" %s &pdq->emp->CmdRspBuf: %x\n", pdq->name,  &pdq->emp->CmdRspBuf);

    pprintf("\nCommand Request  Buffer Address MAP\n");
    pprintf(" %s &pdq->emp->CmdReqBuf: %x\n", pdq->name,  &pdq->emp->CmdReqBuf);

    pprintf("\nUnsolicited Buffer Address MAP\n");
    pprintf(" %s &pdq->emp->UnsolBuf: %x\n", pdq->name,  &pdq->emp->UnsolBuf);

    pprintf("\nDEFEA On-Board Base Address MAP\n");
    pprintf(" %s pdq->emp+pdq->cbp_offset   : %x\n", pdq->name,  (unsigned int)pdq->emp+pdq->cbp_offset);
    pprintf(" %s pdq->emp+pdq->xmt_offset   : %x\n", pdq->name,  (unsigned int)pdq->emp+pdq->xmt_offset);
    pprintf(" %s pdq->emp+pdq->rcv_offset   : %x\n", pdq->name,  (unsigned int)pdq->emp+pdq->rcv_offset);
    pprintf(" %s pdq->emp+pdq->CmdRsp_offset: %x\n", pdq->name,  (unsigned int)pdq->emp+pdq->CmdRsp_offset);
    pprintf(" %s pdq->emp+pdq->CmdReq_offset: %x\n", pdq->name,  (unsigned int)pdq->emp+pdq->CmdReq_offset);
    pprintf(" %s pdq->emp+pdq->unsol_offset : %x\n", pdq->name,  (unsigned int)pdq->emp+pdq->unsol_offset);


    pprintf("\nDEFEA Register Address MAP\n");
    pprintf(" %s PDQ_PortReset   : %x\n", pdq->name,  PDQ_PortReset);
    pprintf(" %s PDQ_HostData    : %x\n", pdq->name,  PDQ_HostData);
    pprintf(" %s PDQ_PortControl : %x\n", pdq->name,  PDQ_PortControl);
    pprintf(" %s PDQ_PortDataA   : %x\n", pdq->name,  PDQ_PortDataA);
    pprintf(" %s PDQ_PortDataB   : %x\n", pdq->name,  PDQ_PortDataB);
    pprintf(" %s PDQ_PortStatus  : %x\n", pdq->name,  PDQ_PortStatus);
    pprintf(" %s PDQ_HostIntType0: %x\n", pdq->name,  PDQ_HostIntType0);
    pprintf(" %s PDQ_HostIntEnbX : %x\n", pdq->name,  PDQ_HostIntEnbX);
    pprintf(" %s PDQ_Type2Prod   : %x\n", pdq->name,  PDQ_Type2Prod);
    pprintf(" %s PDQ_CmdRspProd  : %x\n", pdq->name,  PDQ_CmdRspProd);
    pprintf(" %s PDQ_CmdReqProd  : %x\n", pdq->name,  PDQ_CmdReqProd);
    pprintf(" %s PDQ_HostSmtProd : %x\n", pdq->name,  PDQ_HostSmtProd);
    pprintf(" %s PDQ_UnsolProd   : %x\n", pdq->name,  PDQ_UnsolProd);
    }
#endif


/*+
 * ============================================================================
 * = pdq_dump_csrs - Dump CSRs CALLBACK ROUTINE 		 	              =
 * ============================================================================
 *
 * OVERVIEW:
 *	This routine will display the contents of the DEFEA PDQ, ESIC and
 *	Miscellaneous registers.
 *
 * ARGUMENTS:
 *	*pdq -	DEFEA Port Block Pointer
 *
-*/
void pdq_dump_csrs (struct NI_GBL *np) {
#if PDQ_DEBUG

    struct PDQ_PB *pdq;

    pdq = (struct PDQ_PB*)np->pbp;	/* PORT Block Pointer */

    if (pdq->pb.type==TYPE_PCI)
    	{
    	printf("PCI Config Registers\n");
    	printf("\t%x: \t%x\n", VEND_DEV_ID, incfgl(pdq, VEND_DEV_ID));
    	printf("\t%x: \t%x\n", COM_STAT,    incfgl(pdq, COM_STAT));
        printf("\t%x: \t%x\n", REV_ID,      incfgl(pdq, REV_ID));
        printf("\t%x: \t%x\n", BASE_ADDR0,  incfgl(pdq, BASE_ADDR0));
        printf("\t%x: \t%x\n", BASE_ADDR1,  incfgl(pdq, BASE_ADDR1));
        printf("\t%x: \t%x\n", BASE_ADDR2,  incfgl(pdq, BASE_ADDR2));
        printf("\t%x: \t%x\n", INT_LINE,    incfgl(pdq, INT_LINE));
    	}

    printf("PDQ Registers\n");
    printf("\t%x: \t%x\n", PDQ_PortReset,   read_defpa_csr(pdq, PDQ_PortReset));
    printf("\t%x: \t%x\n", PDQ_HostData,    read_defpa_csr(pdq, PDQ_HostData));
    printf("\t%x: \t%x\n", PDQ_PortControl, read_defpa_csr(pdq, PDQ_PortControl));
    printf("\t%x: \t%x\n", PDQ_PortDataA,   read_defpa_csr(pdq, PDQ_PortDataA));
    printf("\t%x: \t%x\n", PDQ_PortDataB,   read_defpa_csr(pdq, PDQ_PortDataB));
    printf("\t%x: \t%x\n", PDQ_PortStatus,  read_defpa_csr(pdq, PDQ_PortStatus));
    printf("\t%x: \t%x\n", PDQ_HostIntType0,read_defpa_csr(pdq, PDQ_HostIntType0));
    printf("\t%x: \t%x\n", PDQ_HostIntEnbX, read_defpa_csr(pdq, PDQ_HostIntEnbX));
    printf("\t%x: \t%x\n", PDQ_Type2Prod,   read_defpa_csr(pdq, PDQ_Type2Prod));
    printf("\t%x: \t%x\n", PDQ_CmdRspProd,  read_defpa_csr(pdq, PDQ_CmdRspProd));
    printf("\t%x: \t%x\n", PDQ_CmdReqProd,  read_defpa_csr(pdq, PDQ_CmdReqProd));
    printf("\t%x: \t%x\n", PDQ_HostSmtProd, read_defpa_csr(pdq, PDQ_HostSmtProd));
    printf("\t%x: \t%x\n", PDQ_UnsolProd,   read_defpa_csr(pdq, PDQ_UnsolProd));

    if (pdq->pb.type==TYPE_EISA)
    	{
    	printf("Eisa ESIC Registers\n");
    	printf("\t%x: \t%x\n", pdq->pb.csr+ESIC_SlotCntrl,
        	inportb(pdq, pdq->pb.csr+ESIC_SlotCntrl));

    	printf("\t%x: \t%x\n", pdq->pb.csr+ESIC_IoAdrsCmp0,
        	inportb(pdq, pdq->pb.csr+ESIC_IoAdrsCmp0));

    	printf("\t%x: \t%x\n", pdq->pb.csr+ESIC_IoAdrsCmp1,
        	inportb(pdq, pdq->pb.csr+ESIC_IoAdrsCmp1));

    	printf("\t%x: \t%x\n", pdq->pb.csr+ESIC_IntChanCfgSta,
        	inportb(pdq, pdq->pb.csr+ESIC_IntChanCfgSta));

    	printf("\t%x: \t%x\n", pdq->pb.csr+ESIC_FuncCntrl,
        	inportb(pdq, pdq->pb.csr+ESIC_FuncCntrl));

    	printf("\t%x: \t%x\n", pdq->pb.csr+PDQ_BurstHoldoff,
        	inportb(pdq, pdq->pb.csr+PDQ_BurstHoldoff));

    	printf("\t%x: \t%x\n", pdq->pb.csr+PDQ_SlotIO,
        	inportb(pdq, pdq->pb.csr+PDQ_SlotIO));
    	}
    else
    	{
    	printf("PCI PFI Registers\n");
    	printf("\t%x: \t%x\n", PFI_ModeControl, read_defpa_csr(pdq, PFI_ModeControl));
    	printf("\t%x: \t%x\n", PFI_Status,      read_defpa_csr(pdq, PFI_Status));
    	}
#endif
    }


#if PDQ_DEBUG

/*+
 * ============================================================================
 * = pdq_dump_mem - Dump Eisa Memory 		 	              =
 * ============================================================================
 *
 * OVERVIEW:
 *	Dumps the contents of the Eisa Memory Mapped PDQ Descriptors and things.
 * ARGUMENTS:
 *	*pdq -	DEFEA Port Block Pointer
 *
-*/
void pdq_dump_mem (struct PDQ_PB *pdq) {
    PDQ_PRODUCER_INDEX  *pi;	/* Producer Index block*/	
    unsigned int lwbuf;

    unsigned char i;

#if PDQ_TRACE
    pprintf("\nEntering pdq_dump_mem %s\n", pdq->name);
#endif

    pi = (PDQ_PRODUCER_INDEX*)pdq->pip;

    pprintf("\nHost Copy of Producer Index  CONTENTS\n");
    pprintf(" %s pi->XmtDataCompletionIndex: %x\n", pdq->name,   pi->XmtDataCompletionIndex);
    pprintf(" %s pi->RcvDataCompletionIndex: %x\n", pdq->name,  pi->RcvDataCompletionIndex);
    pprintf(" %s pi->SMTCompletionIndex: %x\n", pdq->name,      pi->SMTCompletionIndex);
    pprintf(" %s pi->UnsolCompletionIndex: %x\n", pdq->name,    pi->UnsolCompletionIndex);
    pprintf(" %s pi->CmdRspCompletionIndex: %x\n", pdq->name,   pi->CmdRspCompletionIndex);
    pprintf(" %s pi->CmdReqCompletionIndex: %x\n\n", pdq->name, pi->CmdReqCompletionIndex);

    pprintf(" %s pi->XmtDataProducer: %x\n", pdq->name, pi->XmtDataProducer);
    pprintf(" %s pi->RcvDataProducer: %x\n", pdq->name, pi->RcvDataProducer);
    pprintf(" %s pi->SMTProducer: %x\n", pdq->name,     pi->SMTProducer);
    pprintf(" %s pi->UnsolProducer: %x\n", pdq->name,   pi->UnsolProducer);
    pprintf(" %s pi->CmdRspProducer: %x\n", pdq->name,  pi->CmdRspProducer);
    pprintf(" %s pi->CmdReqProducer: %x\n", pdq->name,  pi->CmdReqProducer);
    
    pprintf("\nReceive, Descriptor Block Memory CONTENTS\n");

    for (i=0; i<10; i++)
    	{
    	/* Rcv Desc, seg_len = 8192 bytes, seg_cnt=0, SOB=1
    	*/
        pdq_read_mem(pdq, &pdq->emp->db.rcv_data[i].long_1, sizeof(PDQ_ULONG), &lwbuf);
        pprintf(" %s &pdq->emp->db.rcv_data[%d].long_1: %x\n", pdq->name, i, lwbuf);
        pdq_read_mem(pdq, &pdq->emp->db.rcv_data[i].buff_lo, sizeof(PDQ_ULONG), &lwbuf);
        pprintf(" %s &pdq->emp->db.rcv_data[%d].buff_lo: %x\n", pdq->name, i, lwbuf);
    	}

    pprintf("\nTransmit Descriptor Block Memory CONTENTS\n");

    for (i=0; i<10; i++)
    	{
    	/* Xmt Desc, We'll fill in the size in pdq_out(), SOB=1,EOB=1
    	/*  However we'll reserve a 4k buffer into which we'll munge the
    	/*  IEEE frame into an FDDI frame.
    	*/
        pdq_read_mem(pdq, &pdq->emp->db.xmt_data[i].long_1, sizeof(PDQ_ULONG), &lwbuf);
        pprintf(" %s &pdq->emp->db.xmt_data[%d].long_1: %x\n", pdq->name, i, lwbuf);
        pdq_read_mem(pdq, &pdq->emp->db.xmt_data[i].buff_lo, sizeof(PDQ_ULONG), &lwbuf);
        pprintf(" %s &pdq->emp->db.xmt_data[%d].buff_lo: %x\n", pdq->name, i, lwbuf);
    	}

    }
#endif



#if PDQ_FYI

/*+
 * ============================================================================
 * = pdq_type0_interrupt - Type 0 interrupt routine.                           =
 * ============================================================================
 *
 * OVERVIEW:
 *	This routine processes the type 0 interrupts. 
 *
 * RETURNS:
 *	None
 *
 * ARGUMENTS:
 *	*pdq 	- 	Pointer to the fr port block.
 *
 *
-*/
void pdq_type0_interrupt (struct PDQ_PB *pdq) {
    }

/*+
 * ============================================================================
 * = pdq_type1_interrupt - Type 1 interrupt routine.                           =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine processes the type 1 interrupts. 
 *
 * FORM OF CALL:
 *
 *	pdq_type1_interrupt (pdq); 
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *	*pdq	 - 	Pointer to the fr port block.
 *
 * SIDE EFFECTS:
 *      None
 *
-*/
void pdq_type1_interrupt (struct PDQ_PB *pdq) {
    }

/*+
 * ============================================================================
 * = pdq_type2_interrupt - Type 2 interrupt routine.                           =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine processes the type 2 interrupts. 
 *
 * FORM OF CALL:
 *
 *	pdq_type2_interrupt (pdq); 
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *	*pdq 	- 	Pointer to the fr port block.
 *
 * SIDE EFFECTS:
 *      None
 *
-*/
void pdq_type2_interrupt (struct PDQ_PB *pdq) {
    }
#endif


#if PDQ_DEBUG

#define QDEVICE 0
#define QPASS   1
#define QMAX    2
#define QSTRING "%sdevice %dpass"

/*+
 * ============================================================================
 * = pdq_cmd - pdq_cmd						              =
 * ============================================================================
 *
 *   OVERVIEW:
 *	A way to call FDDI routines from the console.
 *
 *   COMMANDDDDD FMTTTTT: pdq_cmd 0 Z 0 pdq_cmd
 *  
 *   COMMANDDDDD FORMMMMM:
 *  
 *	pdq_cmd ( [<test_arg>] )
 *  
 *   COMMANDDDDD TAGGGGGG: pdq_cmd 0 RXBZ pdq_cmd
 *  
 *   COMMANDDDDD ARGUMENTTTTT(S):
 *
 *	<test_arg> - no arg stop driver, one arg start driver
 *
 * FORM OF CALL:
 *
 * RETURNS:
 *
 * ARGUMENTS:
 *
 * SIDE EFFECTS:
 *      
 *
-*/
int pdq_cmd (int argc, char *argv) {

    if (argc==1)
    	{
    	driver_setmode(DDB$K_STOP,0);
    	return;
    	}

    if (argc==2)
    	{
    	driver_setmode(DDB$K_START,0);
    	return;
    	}
    }
#endif



/*+
 * ============================================================================
 * = pdq_init_pb - Initialization the DEFEA Port Block			      =
 * ============================================================================
 *
 * OVERVIEW:
 *	This routine malloc's memory for the DEFEA's port block and defines
 *	the data within.
 *
 * ARGUMENTS:
 *	*ip	- DEFEA INODE pointer 
 *	*pdq	- DEFEA Port Block pointer
 *
-*/
int pdq_init_pb (struct INODE *ip, struct PDQ_PB *pdq) {
    struct NI_GBL *np;		/* Pointer to the NI global database */
    struct PBQ    *ipe;		/* See KERNEL_DEF.H Pointer to a Inode element*/
    struct NI_ENV *NI_ev;	

    char name[20];		/* General holder for a name */

    unsigned int 
    	i,
    	data;

#if PDQ_TRACE
    pprintf("\nEntering pdq_init_pb %s\n", ip->name);
#endif

    /* Malloc memory DEFEA Data Structures.
    */
    ip->misc = (int*) malloc(sizeof(struct NI_GBL));
    np       = (struct NI_GBL*)ip->misc;
    np->pbp  = pdq;
    np->enp  = (int *)malloc(sizeof (struct NI_ENV));

    NI_ev    = (struct NI_ENV *)np->enp;

    NI_ev->mode          = 0;
    NI_ev->driver_flags  = 0;
    NI_ev->msg_buf_size  = 1518;
    NI_ev->msg_mod       = 0;
    NI_ev->msg_rem       = 0;
    NI_ev->msg_prefix_size = 0;

    strcpy (pdq->name, ip->name);	    	/* fra0.0.0.0.1 */
    strncpy(pdq->short_name, ip->name, 4);	/* fra0         */

    /* Initialize the port spinlock
    */
    pdq->spl_port.value    = 0;		/* contents of the actual lock */
    pdq->spl_port.req_ipl  = IPL_SYNC;	/* Raise this CPUs IPL to 31 */
    pdq->spl_port.sav_ipl  = 0;		/* IPL to restore this CPU typically 0 */
    pdq->spl_port.attempts = 0;		/* times spinlock taken out */
    pdq->spl_port.retries  = 0;		/* times spinlock failed */
    pdq->spl_port.owner    = 0;		/* PID of current owner */
    pdq->spl_port.count    = 0;		/* number of concurrent locks */

    pdq->emp = malloc_a(sizeof(DEFEA_EISA_MEMORY),0x2000) | io_get_window_base(pdq);
    pdq->emp_size = sizeof(DEFEA_EISA_MEMORY);
 
    if (pdq->pb.type==TYPE_EISA)
    	pdq_bus_init_eisa(pdq);		

    pdq->ip = ip;
    pdq->pb.setmode = pdq_setmode;

    /* Initialize the CALLBACK receive msg queue
    /* -----------------------------------------
    /* Initialize the Call Back Messages Received List.  
    /* If Call Backs are enabled, the DEFEA port driver will stuff this
    /* receive queue with incoming messages as soon as they arrive.  
    /* pdq_read() will remove them from this list as called by the OS.
    */
    spinlock(&spl_kernel);
    pdq->rcv_msg_cnt = 0;
    pdq->rqh.flink   = &(pdq->rqh);
    pdq->rqh.blink   = &(pdq->rqh);
    spinunlock(&spl_kernel);

    /* Initialize some flags
    */
    pdq->pb.state      = PDQ$K_UNINITIALIZED;
    pdq->filter_type   = 0;                    
    pdq->setup_init    = 0;                    
    pdq->password_init = 0;

#if FORCE_LOOPBACK
    pdq->loopback      = TRUE;
#else
    pdq->loopback      = FALSE;
#endif

    /* Set up the interrupt vector 
    */
    int_vector_set (pdq->pb.vector, pdq_interrupt, pdq);
    io_enable_interrupts (pdq, pdq->pb.vector);

    /* ndl_init() will assign this location with the following address:
    /*  *rp = (int)ndl_receive_msg	...magic code....
    */
    pdq->lrp = 0;		/* ndl_init() call defines this routine adrs */

    /* Initialize semaphores
    */
    sprintf(name, "%4.4s_isr_tx", ip->name);
    krn$_seminit(&(pdq->pdq_isr_tx), 0, name);

    sprintf(name, "%4.4s_isr_rx", ip->name);
    krn$_seminit(&(pdq->pdq_isr_rx), 0, name);

    sprintf(name, "%4.4spdq_out", ip->name);
    krn$_seminit(&(pdq->pdq_out), 1, name);

    sprintf(name, "%4.4spdq_stop_rx", ip->name);
    krn$_seminit(&(pdq->pdq_stop_rx), 0, name);

    return msg_success;
    }


/*+
 * ============================================================================
 * = pdq_bus_init_eisa - Initialize the DEFEA Eisa bus control                      =
 * ============================================================================
 *
 * OVERVIEW:
 *	The DEFEA adapter is initialized in three steps:
 *	1. First step is done by the Eisa Configuration Utility, ECU.  
 *	   It initializes the necessary ESIC registers and sets the following.
 *		- IRQ
 *		- Base Memory Address
 *		- Size of mapped memory (32K or 64k)
 *	2. Second step is done by the port driver.  It performs the necessary
 *	   initialization to complete the address decode for the adapter.
 *	3. Third step is also done by the port driver, is to enable the EISA
 *	   interrupt logic.
 *
 *	See "Eisa PDQ FDDI Adapter Port Specification"
 *
 * ARGUMENTS:
 *	*pdq - DEFEA Port Block pointer
-*/
void pdq_bus_init_eisa (struct PDQ_PB *pdq) {
    unsigned char data;

#if PDQ_TRACE
    pprintf("\nEntering pdq_bus_init_eisa %s\n", pdq->name);
#endif

#if ENABLE_PRINTF
    pprintf("ECU Memory Configuration\n");
    pprintf(" Adrs: %x Data: %x\n", pdq->pb.csr+0xC85, pdq_read_csr_byte(pdq, 0xC85));
    pprintf(" Adrs: %x Data: %x\n", pdq->pb.csr+0xC85, pdq_read_csr_byte(pdq, 0xC86));
    pprintf(" Adrs: %x Data: %x\n", pdq->pb.csr+0xC85, pdq_read_csr_byte(pdq, 0xC87));
    pprintf(" Adrs: %x Data: %x\n", pdq->pb.csr+0xC85, pdq_read_csr_byte(pdq, 0xC88));
    pprintf(" Adrs: %x Data: %x\n", pdq->pb.csr+0xC85, pdq_read_csr_byte(pdq, 0xC89));
    pprintf(" Adrs: %x Data: %x\n", pdq->pb.csr+0xC85, pdq_read_csr_byte(pdq, 0xC8A));
    pprintf(" Adrs: %x Data: %x\n", pdq->pb.csr+0xC85, pdq_read_csr_byte(pdq, 0xC8B));
    pprintf(" Adrs: %x Data: %x\n", pdq->pb.csr+0xC85, pdq_read_csr_byte(pdq, 0xC8E));
    pprintf(" Adrs: %x Data: %x\n", pdq->pb.csr+0xC85, pdq_read_csr_byte(pdq, 0xC8F));
    pprintf(" Adrs: %x Data: %x\n", pdq->pb.csr+0xC85, pdq_read_csr_byte(pdq, 0xCA8));
#endif

    /* Enable access to memory and IO on the board
    */    
    pdq_write_csr_byte(pdq, ESIC_FuncCntrl, 0x23); 


    /* Set the IO decode range of the board
    */
    data = pdq->pb.slot * 16;
    pdq_write_csr_byte(pdq, ESIC_IoAdrsCmp0, data); 
    pdq_write_csr_byte(pdq, ESIC_IoAdrsCmp1, data); 

    /* Enable access to rest of module (including PDQ and Packet memory
    */
    pdq_write_csr_byte(pdq, ESIC_SlotCntrl, 0x01); 

    /* Memory and IO access modes:
    /* 	IO Mode     - Access Adapter Memory at specified base address.
    /*		      Access PDQ and ESIC registers using eisa slot ID
    /*
    /*  Memory Mode - Access PDQ registers using offset from specified 
    /*		      base address
    /*		      Access ESIC registers using eisa slot ID
    */
    data = pdq_read_csr_byte(pdq, PDQ_BurstHoldoff);

#if 1
    	data &= 0xFE;	/* IO Mode */
#else
    	data |= 0x01;	/* Memory Mode */
#endif

    pdq_write_csr_byte(pdq, PDQ_BurstHoldoff, data); 
    
    /* Enable interrupts
    */
    data  = pdq_read_csr_byte(pdq, ESIC_IntChanCfgSta);
    data |= 0x08;
    pdq_write_csr_byte(pdq, ESIC_IntChanCfgSta, data);

#if ENABLE_PRINTF
    pprintf("Adrs: %x ESIC_FuncCntrl \t%x\n", pdq->pb.csr+ESIC_FuncCntrl, 	pdq_read_csr_byte(pdq, ESIC_FuncCntrl)); 
    pprintf("Adrs: %x ESIC_IoAdrsCmp0 \t%x\n", pdq->pb.csr+ESIC_IoAdrsCmp0,	pdq_read_csr_byte(pdq, ESIC_IoAdrsCmp0)); 
    pprintf("Adrs: %x ESIC_IoAdrsCmp1 \t%x\n", pdq->pb.csr+ESIC_IoAdrsCmp1,	pdq_read_csr_byte(pdq, ESIC_IoAdrsCmp1)); 
    pprintf("Adrs: %x ESIC_SlotCntrl \t%x\n", pdq->pb.csr+ESIC_SlotCntrl,	pdq_read_csr_byte(pdq, ESIC_SlotCntrl)); 
    pprintf("Adrs %x PDQ_BurstHoldoff after: \t%x\n", pdq->pb.csr+PDQ_BurstHoldoff,	pdq_read_csr_byte(pdq, PDQ_BurstHoldoff)); 
    pprintf("Adrs %x ESIC_IntChanCfgSta after \t%x\n", pdq->pb.csr+ESIC_IntChanCfgSta,	pdq_read_csr_byte(pdq, ESIC_IntChanCfgSta)); 
#endif
    }


/*+
 * ============================================================================
 * = pdq_init - Initialization routine for Eisa/FDDI the port driver          =
 * ============================================================================
 *
 * OVERVIEW:
 *	This routine is called during powerup and allocates an INODE for all 
 *	configured Eisa/FDDI adapters sized during eisa_bus_sniff().  It
 *	initializes the INODEs data structures, then turns on the PDQ for each 
 *	device.
 *
 * CALLED FROM:
 *	filesys.c  ddb_startup(phase);  dst[i]
 *
 * RETURNS:
 *	msg_success or msg_failure 
 *
-*/
int pdq_init () {

/*
    if (pdq_inited)
	return msg_success;
    pdq_inited = 1;
*/

    find_pb("fw", sizeof(struct PDQ_PB), pdq_init1_pb);
    find_pb("fr", sizeof(struct PDQ_PB), pdq_init1_pb);

    return(msg_success);
    }


/*+
 * ============================================================================
 * = pdq_init1_pb - Initialization routine for Eisa/FDDI the port driver        =
 * ============================================================================
 *
 * OVERVIEW:
 *	This routine is called during powerup and allocates an INODE for all 
 *	configured Eisa/FDDI adapters sized during eisa_bus_sniff().  It
 *	initializes the INODEs data structures, then turns on the PDQ for each 
 *	device.
 *
 * CALLED FROM:
 *	filesys.c  ddb_startup(phase);  dst[i]
 *
 * RETURNS:
 *	msg_success or msg_failure 
 *
-*/
int pdq_init1_pb(struct PDQ_PB *pdq) {
    struct INODE  *ip;

    char   
    	pdq_name[20],		/* DEFEA INODE Name */
    	name[20];		/* General holder for a name */
    
#if PDQ_TRACE
    pprintf("\nEntering pdq_init\n");
#endif

#if PDQ_BPT
    do_bpt();
#endif

    log_driver_init(pdq);

    /* Do the first stage of the driver init
    /* Create a File Name for this DEFEA Port Driver 
    /*
    /*       fxa0.0.0.0.1
    /*       | || | | | |
    /*       | || | | | |
    /*       | || | | | +-- Bus #: 0,2==PCI_TYPE  1==EISA_TYPE
    /*       | || | | | 
    /*       | || | | +---- Slot #       : Eisa Slot 1:8 
    /*       | || | +------ Channel #    : used for multi-channel devices
    /*       | || +-------- Bus Node #   : Device's bus ID
    /*       | |+---------- Device Unit #: Device's unique system unit numb
    /*       | +----------- Controller ID: One letter controller designator
    /*       +------------- Driver ID: Two letter port controller designator
    /*                               ER - Ethernet   - Eisa port
    /*                               EW - Ethernet   - PCI  port
    /*                               FA - DEFFA      - Fbus port
    /*                               FR - DEFEA      - Eisa port
    /*
    */
    set_io_prefix(pdq, name, "");
    set_io_name(pdq_name, 0, 0, 0, pdq);

    /* Get the CSR address.
    */
    switch (pdq->pb.type)
	{
	case TYPE_EISA:
	    pdq->rp = (struct PDQ_CSRS *)pdq->pb.csr;
	    break;
	case TYPE_PCI:
    	    pdq->rp = (struct PDQ_CSRS *)(incfgl(pdq, 0x10) & ~0xf);
	    break;
	}

    /* Create an inode for this DEFEA, thus making the 
    /* device visible as a file. 
    */
    allocinode (pdq_name, 1, &ip);

    ip->dva   = &pdq_ddb;
    ip->attr  = ATTR$M_READ|ATTR$M_WRITE;
    ip->loc   = 0;

    INODE_UNLOCK (ip);		/* Make the DEFEA INODE available for use */

    /* Initialize this DEFEA's Port Block struture and
    /* attach it to its INODE.
    */
    pdq_init_pb(ip, pdq);

    /* see MOP_DRIVER.C
    /*     Creates the following NETTEST Environment Var's
    /*        fwa_loop_size
    /*        fwa_loop_inc
    /*        fwa_loop_patt
    /*        fwa_loop_count
    /*        fwa_lp_msg_node
    */
    mop_set_env(name);		/* see MOP_DRIVER.C */

    pdq_malloc_descr(ip);	/* malloc memory and define offsets */

#if PDQ_MAP
    pdq_print_map(pdq);
#endif
#if PDQ_CSR
    pdq_dump_csrs(pdq);
#endif

    pdq_start_PDQ(ip);

#if PDQ_DEBUG
    krn$_sleep(4000);		/* makes for better log file */
#endif

    return msg_success;
    }

void pdq_write_csr (struct PDQ_PB *pdq, int csr, unsigned int data)
    {
    unsigned int offset;

    offset = (unsigned int)pdq->rp + csr;
    switch (pdq->pb.type)
	{
	case TYPE_EISA:
	    outportl (pdq, offset, data);
	    break;
	case TYPE_PCI:
	    outmeml (pdq, offset, data);
	    break;
	}
    }

int pdq_read_csr (struct PDQ_PB *pdq, int csr)
    {
    unsigned int offset;
    unsigned int data;

    offset = (unsigned int)pdq->rp + csr;
    switch (pdq->pb.type)
	{
	case TYPE_EISA:
	    data = inportl (pdq, offset);
	    break;
	case TYPE_PCI:
	    data = inmeml (pdq, offset);
	    break;
	}
    return data;
    }

void pdq_write_csr_byte (struct PDQ_PB *pdq, int csr, unsigned int data)
    {
    unsigned int offset;

    offset = (unsigned int)pdq->rp + csr;
    outportb (pdq, offset, data);
    }

int pdq_read_csr_byte (struct PDQ_PB *pdq, int csr)
    {
    unsigned int offset;
    unsigned int data;

    offset = (unsigned int)pdq->rp + csr;
    data = inportb (pdq, offset);
    return data;
    }

/*+
 * ============================================================================
 * = net.hlp - Help text to Access a network port 			      =
 * ============================================================================
 *
 * GENERAL OVERVIEW:
 *	Using the consoles >>> net command to perform network device specific 
 *	maintenance operations.
 *  
 *   FORM:
 *      net ( [-s]   [-sa]    [-ri]    [-ic]   [-id] 
 *	      [-l0]  [-l1]    [-rb]    [-csr]  [-els] 
 *	      [-kls] [-start] [-stop]  
 *	      [-cm <mode_string>] 
 *	      [-da <node_address>] 
 *	      [-l  <file_name>] 
 *	      [-lw <wait_in_secs>]
 *	      [-sv <mop_version>] 
 *	      <port_name> ) 
 *  
 *   ARGUMENT(S):
 *	<port_name> - Specifies the Networdk port on which to operate.
 *
 *   OPTION(S):
 *	-s     - Display port status information including MOP counters.
 *	-sa    - Display the port's Ethernet station address.
 *	-ri    - Reinitialize the port drivers.
 *	-start - Start the port drivers.
 *	-stop  - Stop the port drivers.
 *	-ic    - Initialize the MOP counters.
 *	-id    - Send a MOP Request ID to the specified node.
 *		 Uses -da to specify the destination address.
 *	-l0    - Send an Ethernet loopback to the specified node.
 *		 Uses -da to specify the destination address.
 *	-l1    - Do a MOP loopback requester.
 *	-rb    - Send a MOP V4 boot message to remote boot a node.
 *	 	 Uses -da to specify the destination address.
 *	-csr   - Displays the values of the Ethernet port CSRs.
 *	-els   - Enable the extended DVT loop service.
 *	-kls   - Kill the extended DVT loop service.
 *
 *	-cm <mode_string> - Change the mode of the port device.
 *		The mode string may be any one of the following abbreviations.
 *		      o	nm =  Normal mode
 *		      o	in =  Internal Loopback
 *		      o	ex =  External Loopback
 *		      o	nf =  Normal Filter
 *		      o	pr =  Promiscious
 *		      o	mc =  Multicast
 *		      o	ip =  Internal Loopback and Promiscious
 *		      o	fc =  Force collisions
 *		      o	nofc = Don't Force collisions
 *		      o	df   = Default 
 *		      o	tp   =  Twisted pair
 *		      o	aui  = AUI
 *	-da <node_address> - Specifies the destination address of a node
 *			     to be used with -l0, -id, or -rb, options.
 *	-l  <file_name>    - Attempt a MOP load of the file name.
 *	-lw <wait_in_secs> - Wait the number of seconds specified for the 
 *			     loop messages from the -l1 option to return.  
 *			     If the messages don't return in the time period, 
 *			     an error message is generated.
 *	-sv <mop_version>  - Set the prefered MOP version number for operations.
 *		             Legitimate values are 3 or 4.
 *
 *   EXAMPLE(S):
 *	Display the local Ethernet ports' station address.
 *      >>>net -sa
 *      -fra0:  08-00-2b-1d-02-91
 *      >>>net -sa frb0
 *      -frb0:  08-00-2b-1d-02-92
 *
 *	Display the fra0 port status, including the MOP counters.
 *      >>>net -s
-*/


/*+                                                                  
 * ============================================================================
 * = nettest.hlp - Help text to test the network ports using MOP loopback     =
 * ============================================================================
 *                                                
 * GENERAL OVERVIEW:                                            
 *                                                                           
 *      Nettest contains the basic options to allow the user to run MOP
 *	loopback tests.  It is assumed that nettest will be included in a
 *	script for most applications.  Many environment variables can be
 *	set to customize nettest.  These may be set/show, from the console 
 *	before nettest is started, ie.
 *	>>>
 *	>>>show fra*
 *	dwa0_loop_count         3e8
 *	dwa0_loop_inc           a
 *	dwa0_loop_patt          ffffffff
 *	dwa0_loop_size          2e
 *	dwa0_lp_msg_node        1
 *	>>>
 *	>>>set dwa0_loop_count 200
 *	>>>set dwa0_loop_inc   10
 *
 *   ENVIRONMENT VARIABLE(S):
 *	To show/set port environment variable perform the following console
 *	command line:
 *	    >>> show dwa*
 *	    >>> set  dwa0_loop_count %d100000
 *
 *	dwa0_lp_msg_node _
 *		This qualifier specifies the number of unique frames queued to 
 *		each port.  The destination of the frame is determined in the 
 *		file lp_nodes_dwa0. To set this qualifier perform the following 
 *		console command line, the default is 1 frame.
 *		    >>> set  dwa0_lp_msg_node %d50
 *	dwa0_loop_count  - 
 *		This qualifier specifies the number of times to ReTransmit 
 *		each frame to the node specified in lp_nodes_dwa0.To set this 
 *		qualifier perform the following console command line, the 
 *		default is %d1000:
 *		    >>> set  dwa0_loop_count %d100000
 *	dwa0_loop_size   - 
 *		The initial byte size of each LoopBack frame is specified by
 *		this qualifier.  The byte size of the frames will grow by 
 *		dwa0_loop_inc bytes for each ReTransmission of the frame. 
 *		To set this qualifier perform the following console command 
 *		line, the default is %d46
 *		    >>> set  dwa0_loop_size %d1500
 *	dwa0_loop_inc    - 
 *		Specifies the growth of each ReTransmitted frame to a maximum
 *		of %d1500 bytes for both ENET/IEEE/FDDI. To set this qualifier 
 *		perform the following console command line, the default is %d10
 *		    >>> set  dwa0_loop_inc  %d20
 *	- dwa0_loop_patt   
 *		Specifies the frame data pattern.  A value of F's selects
 *		all possible patterns.  If not F's, a fixed pattern is selected.
 *		If the value is F's,  each time a frame is ReTransmitted, the 
 *		next dwa0_loop_patt is selected, and the frame size is
 *		incremented by dwa0_loop_inc.  To set this qualifier perform 
 *		the following console command line, the default is %d10
 *		    >>> set  dwa0_loop_patt 5.
 *		Patterns to select from are as following:
 *
 *		o 0 : all zeros
 *		o 1 : all ones
 *		o 2 : all fives
 *		o 3 : all 0xAs
 *		o 4 : incrementing data
 *		o 5 : decrementing data
 *		o ffffffff : all patterns
 *
 * CREATING lp_nodes_dwa0
 *	To create <lp_nodes_dwa0> from the console perform the following 
 *	command lines:
 *	>>>
 *	>>>net -sa dwa0
 *	-dwa0:  08-00-2b-a1-71-7a
 *	>> 
 *	>>> net -sa dwa0 > ndbr/lp_nodes_dwa0
 *	>>>
 *	>>> cat ndbr/lp_nodes_dwa0
 *	Node: 08-00-2b-a1-71-7a
 *	>>>
 *	>>> echo "-file:  08-00-2b-a1-71-7a" >  ndbr/lp_nodes_all
 *	>>> echo "-file:  08-00-2b-a1-96-4b" >> ndbr/lp_nodes_all
 *	>>>
 *	>>>cat ndbr/lp_nodes_all
 *	Node: 08-00-2b-a1-71-7a
 *	Node: 08-00-2b-a1-96-4b
 *
 * DETAILED OVERVIEW:                                            
 *  	The routine MOP_LOOP_REQUESTER() is called to queue any number of frames
 *	to the NetWork Port specified by the file lp_nodes_dwa0. The qualifier 
 *	dwa0_lp_msg_node specifies the number of unique frames queued to the
 *	SOURCE port.  The destination of the LoopBack frame is determined in 
 *	the file lp_nodes_dwa0. For each dwa0_lp_msg_node frame ReTransmitted 
 *	the:
 *	    - next dwa0_loop_patt is selected
 *	    - next dwa0_loop_size is incremented by dwa0_loop_inc
 *
 *	Once lp_nodes_dwa0 receives the LoopBack frame, it FORWARDs it back 
 *	to Source port as a LoopBack REPLY.  When the Source port receives the 
 *	REPLY, it counts the number of times received and will either:
 *	   - ReTransmit it, if dwa0_loop_count is not reached
 *	   - Stop ReTransmission once dwa0_loop_count is reached
 *
 *	Once MOP_LOOP_REQUESTER() has queued dwa0_lp_msg_node frames to
 *	each lp_nodes_dwa0 ports, MOPLP_WAIT() is called.  This routine
 *	waits for each dwa0_lp_msg_node to be ReTransmitted for
 * 	dwa0_loop_count times.  MOPLP_WAIT() will continue waiting for
 *	each counter to be reached for -to xx seconds.  An error will be 
 *	reported, along with the number of frames currently received and 
 *	frames expected, if the timeout expired.
 *
 *   FORM:                                             
 *                                                  
 *	>>> nettest ( [-f <file>] [-mode <port_mode>] [-p <pass_count>]
 *                [-sv <mop_version>] [-to <loop_time>] [-w <wait_time>] 
 *		  [<port>] )
 *                                                                           
 *   TAG: nettest 0 RXBZ nettest
 *                                               
 *   ARGUMENT(S):     
 *                                                                 
 *	<port> - 
 *		Specifies the SOURCE Network Port on which to run this test.
 *                                             
 *   OPTION(S):                                                
 *                                                             
 *     -f <lp_nodes_dwa0> - 
 *		A file containing the list of DESTINATION network station
 *		addresses to loop messages to.  The default file name is 
 *		lp_nodes_dwa0 for port dwa0.  The files by default have their 
 *		own station address.		
 *	
 *     -p <pass_count> - 
 *		Specifies the number of times to run the test.  Use 0 to run
 *		test forever.  The default is 1.  Note: This is the number of 
 *		passes for the diagnostic.  Each pass will send the number of 
 *		loop messages as set by the environment variable, 
 *		dwa0_loop_count.  
 *                                                     
 *     -sv <mop_version> - 
 *		Specifies which MOP version protocol to use.
 *		If 3, then MOP V3 (DECNET Phase IV) packet format is used.
 *		If 4, then MOP V4 (DECNET Phase V IEEE 802.3) format is used.
 *                     
 *     -to <seconds>   - 
 *		Specifies the time in seconds allowed for the loop
 *		messages to be returned.  The default is 2 seconds.	
 *		                                 
 *     -w <seconds> - 
 *		Specifies the time in seconds to wait between passes
 *		of the test.  The default is 0 (no delay).  The network 
 *		device can be very CPU intensive.  This option will allow 
 *		other processes to run.		
 *		                                 
 *     -mode <TGEC_mode> - 
 *		Specifies the mode to set the port adapter (TGEC). The 
 *		default is 'ex' (external loopback).  This qualifier is 
 *		unsupported for other network device.
 *
 *		Allowed values are:
 *
 *		o df : default, use environment variable values
 *		o ex : external loopback
 *		o in : internal loopback
 *	       	o nm : normal mode         
 *		o nf : normal filter
 *		o pr : promiscious
 *		o mc : multicast                    
 *		o ip : internal loopback and promiscious
 *    		o fc : force collisions
 *		o nofc : do not force collisions
 *		o nc : do not change mode
 *
 *   COMMAND EXAMPLE(S):                                 
 *                                                               
 *	>>>nettest		   - internal loopback test on port dwa0
 *	>>>nettest dw*		   - internal loopback test on ports dwa0/dwb0
 *	>>>nettest -mode ex 	   - external loopback test on port dwa0
 *	>>>nettest -mode ex -w 10  - external loopback test on port dwa0 wait 10
 *				     seconds between tests	
 *	>>>nettest -f lp_nodes_dwa0 dwb0 -to 20 & 
 *	>>>nettest -f lp_nodes_dwb0 dwa0 -to 20 & 
 *
 * ADDITIONAL HELP:
 *	As nettest is running as a background process, the user can view
 *	it runtime status by executing the following console command:
 *	>>> show_status
 *	 ID       Program      Device       Pass  Hard/Soft Bytes Written  Bytes Read
 *	-------- ------------ ------------ ------ --------- ------------- -------------
 *	0000006b      nettest dwb0.0.0.5.1     23    0    0       1058068       1058000
 *	0000006e      nettest dwa0.0.0.4.1     19    0    0        874068        874000
 *
*/
