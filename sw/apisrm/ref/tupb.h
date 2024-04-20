/*
 * Copyright (C) 1993 by
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
 * and  should  not  be  construed  as a commitment by Digital Equipment
 * Corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by Digital.
 *
 *
 * Abstract:	Tulip port block Definitions
 *
 * Author:	John A. DeNisco
 *
 * Modifications:
 *
 *	joes	12-Oct-1993	Update for new BUS protocol see eco 11.
 *
 *	joes	11-Oct-1993	Update for ew callbacks see eco 28.
 *
 *	joes 	17-Aug-1993 Changed definition of out/inportx for medulla as
 *      		    it has correct offsets for the registers defined.
 *
 *	joes	13-Jul-1993	Update for Medulla.
 *
 *	jad	08-Apr-1993	Initial entry.
 */
 
/*Constants*/
#define TU_K_UNINITIALIZED 0
#define TU_K_HARDWARE_INITIALIZED 1
#define TU_K_STOPPED 2
#define TU_K_STARTED 3
#define TU_K_OPEN_TIMEOUT_SECS 1		/*1 seconds		    */
#define TU_PERFECT_MAX 16
#define TU_FILTER_MAX TU_PERFECT_MAX
#define TU_XMT_TIMEOUT 2000			/*Transmit timeout 2 Secs   */
#define TU_SWITCH_MAX 10

/*Modes*/
#define TU_K_NOT_USED	0			/*Not used		    */
#define TU_K_AUTO	1			/*Auto sensing		    */
#define TU_K_TWISTED	2			/*Twisted Pair		    */
#define TU_K_AUI	3			/*AUI			    */
#define TU_K_BNC	4			/*BNC			    */
#define TU_K_TWISTED_FD	5			/*Full Duplex, Twisted Pair */
#define TU_K_FAST	6			/*Fast 100BaseT		    */
#define TU_K_FASTFD	7			/*Fast 100BaseT Full Duplex */
#define TU_K_AUTO_NEGOTIATE 8			/*Auto negotiate	    */


/*Mode flag passed to OS in CFDA*/	
/* The 21143 specs calls this the CFDD register */

#define TU_K_OS_NOT_USED   0			/*Not used		    */
#define TU_K_OS_TWISTED	   0x300		/*Twisted Pair		    */
#define TU_K_OS_TWISTED_FD 0x400		/*Full Duplex, Twisted Pair */
#define TU_K_OS_AUI	   0x100		/*AUI			    */
#define TU_K_OS_BNC	   0x200		/*BNC			    */
#define TU_K_OS_F          0x500		/*FAST Half Duplex 100BaseTx*/
#define TU_K_OS_FFD        0x600		/*FAST Full Duplex 100BaseTx*/
#define TU_K_OS_FX         0x700		/*FAST Half Duplex 100BaseFx*/
#define TU_K_OS_FFX   	   0x800		/*FAST Full Duplex 100BaseFx*/
#define TU_K_OS_AN	   0x1000		/*Auto Negotiation	    */


/*Min and max packet size for an ethernet*/
#define TU_MIN_PKT_SIZE 60			/*Minimum packet size*/
#define TU_CRC_SIZE 4				/*CRC size	     */
#define TU_MAX_PKT_SIZE NDL$K_MAX_PACK_LENGTH 	/*Maximum packet size*/

/*Default environment vars*/
#define TU_XMT_MOD 512			/*Environment variable defaults     */
#define TU_XMT_REM 0
#define TU_RCV_MOD 512
#define TU_RCV_REM 0
#define TU_MSG_BUF_SIZE TU_MAX_PKT_SIZE
#define TU_MSG_MOD 0
#define TU_MSG_REM 0
#if MEDULLA
#define TU_XMT_BUF_CNT 16		/*Buf cnt must be a power of 2*/
#define TU_RCV_BUF_CNT 16
#else
#define TU_XMT_BUF_CNT 8		/*Buf cnt must be a power of 2*/
#define TU_RCV_BUF_CNT 8
#endif
#define TU_SER_COM_VAL 0

/*Callback message received*/
#define EW_MAX_RCV_MSGS 10
#define EW_CALLBACK_TIMEOUT 100		/*100 * 1/100 of a second*/
#define EW_CALLBACK_WAIT 10		/*10 milliseconds = 1/100 second*/
struct EW_MSG {
    struct EW_MSG *flink;
    struct EW_MSG *blink;
    unsigned char *msg;
    int size;
};

/*Tulip port block structure*/
volatile struct TUPB
{
    struct pb pb;                       /*System port block             */
    struct LOCK spl_port;		/*Must be aligned on a quadword */
    struct POLLQ pqp;			/*Poll queue                    */
    struct SEMAPHORE rx_isr;            /*Receive process semaphore     */
    struct SEMAPHORE cmplt_rx_s;        /*Receive process semaphore     */
    struct SEMAPHORE tu_port_s;		/* Port semaphore */
    struct SEMAPHORE tu_ready_s;	/* Used to sync up port processes */
    struct TIMERQ *tx_isr;		/*Transmit process timers	*/
    struct INODE *ip;			/* Pointer to associated INODE */
    struct MOP$_V4_COUNTERS *mc;	/*Pointer to the MOP counters	*/	
    unsigned long csr13;		/*Value for csr13		*/

    unsigned int rm;			/*Register multiplier		*/
    unsigned int last_auto_mode;	/*Last mode we were in before 	*/
					/*we auto sensed again 		*/
    unsigned int tx_index_in,tx_index_out; /*Transmit indices           */
    unsigned int rx_index;              /*Receive index                 */
    PTU_RING tdes,rdes;			/*Pointer to the XMT,RCV rings	*/
    void (*lrp)(struct FILE*,char*,U_INT_16); /*Pointer to the datalink rcv*/
    int (*lwp)();			/* Pointer to send routine */
    int mode;				/*Driver mode			*/
    int state;				/*Driver state flag		*/
    int pci_type;			/* chip type 21040,21041,21140  */
    unsigned char sa[8];		/*Our station address		*/
    unsigned char filter[TU_FILTER_MAX][8];  /*Perfect addresses	*/
    uint64 nc;				/* No carrier count		*/
    u_long tu_flags_saved;		/* Saved tu flags		*/
    int fcnt;				/* Number of filter addresses	*/
    int fcount;				/* Count of open files that are not part of the driver */
    int ti,tps,tu,tjt,unf,ri,ru,rps;	/* Counts			*/
    int rwt,at,fd,se,lnf,tbf,tto;
    int lkf,ato,anc,aoc;
    int rcv_msg_cnt;			/*Number of recieved messages*/
    struct EW_MSG rqh;			/*Recieved msg queue header*/
    int nway_supported;			/*Supports Auto Negotiation*/
    int fast_speed,full_duplex; 	/*supports 100Mb and full duplex*/
    int controller_type;		/*type of controller e.g. DE500BA*/
};
