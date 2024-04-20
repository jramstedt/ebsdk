/* file:	ni_datalink.c
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
 *	NI datalink routines.
 *
 *  AUTHORS:
 *
 *	John DeNisco
 *
 *  CREATION DATE:
 *  
 *      10-Aug-1990
 *
 *  MODIFICATION HISTORY:
 *
 *	ajb	07-Feb-1992	Change space to misc field in headers for
 *				dynamic blocks.
 *
 *	kp	09-Jan-1992	Remove CB_PROTO usage.
 *
 *	phk	19-Nov-1991	Replace pprintf by err_printf
 *
 *	jad	12-Nov-1991	Spinlocks changed to raising ipl must be
 *				back after the reason for nested exception
 *				is found.
 *
 *	jds	16-Aug-1991	Made lad_new_dl call OPTIONAL.
 *
 *	jad	07-Mar-1991	Change init sequence.
 *
 *	jad	10-Aug-1990	Initial entry.
 *--
 */

/*Include files*/

#include "cp$src:platform.h"
#include "cp$src:common.h"		/*Common definitions*/
#include "cp$src:kernel_def.h"		/*Kernel definitions*/
#include "cp$src:inet.h"
#include "cp$src:prototypes.h"
#include "cp$src:msg_def.h"		/*Message definitions*/
#include "cp$src:dynamic_def.h"		/*Memory allocation defs*/
#include "cp$src:ni_env.h"		/*Environment variables*/
#include "cp$src:ni_dl_def.h"		/*NI datalink definitions*/
#include "cp$src:mop_counters.h"	/*MOP counters*/
#include "cp$inc:kernel_entry.h"

/*Some debug constants*/
#define NDL_DEBUG 0

/*Local constants*/
#define malloc(x) dyn$_malloc(x,DYN$K_SYNC|DYN$K_NOOWN)
#define free(x) dyn$_free(x,DYN$K_SYNC|DYN$K_NOOWN)
#define _addq_v_u(x,y) addq_v_u(&(x),y)

/*External references*/
extern struct ZONE *memzone;		/*Memory zone           */
extern struct LOCK spl_kernel;
extern struct FILE *fopen(char *, char *);
extern unsigned int poweron_msec[2];
extern int mop_init1(struct INODE *ip, unsigned char *sa);

/*Routines (General NI)*/
void init_mop_counters(struct MOP$_V4_COUNTERS *mc);
void print_enet_address(char *name, unsigned char *enet);
void print_memory(void *ptr_in, int rows);
void print_mop_counters(struct MOP$_V4_COUNTERS *mc);
void print_pkt(void *pkt, int size);
void pprint_pkt(void *pkt, int size);
int convert_enet_address(unsigned char *enet_addr, char *enet_addr_str);
void set_timer(struct TIMERQ *tq, int timeout_value);
int wait_msg(struct TIMERQ *tq);

/*Routine definitions*/
void *ndl_allocate_msg(struct INODE *ip, int version, int inc_len);
void *ndl_allocate_pkt(struct INODE *ip, int rem_mask);
void ndl_deallocate_msg(struct INODE *ip, void *msg, int version, int inc_len);
void ndl_deallocate_pkt(struct INODE *ip, void *pkt);
void ndl_get_nisa(struct FILE *fp, unsigned char *sa);
void ndl_handle_dl_msg(struct FILE *fp, char *msg, unsigned short size);
void ndl_handle_msg(struct FILE *fp, char *msg, unsigned short size,
  int version);
int ndl_init(struct INODE *ip, struct PORT_CALLBACK *cb, unsigned char *sa,
  int *rp);
int ndl_preallocate(struct INODE *ip, unsigned char **msg, unsigned char **inp,
  int version, int inc_len, unsigned short is, unsigned short type);
short int ndl_rcv_info_size(struct FILE *fp, int version, int inc_len);
void ndl_receive_msg(struct FILE *fp, char *msg, unsigned short size);
unsigned short ndl_reuse_msg(struct FILE *fp, unsigned char **msg,
  unsigned char *ip, int version, int inc_len, unsigned short is);
void ndl_send(struct FILE *fp, int version, void *ip, int is, int inc_len,
  unsigned char *dest, unsigned short type);
void ndl_set_rcv(struct INODE *ip, unsigned short prot, unsigned char *mca,
  void (*rp)(struct FILE *fp, unsigned char *msg, unsigned char *source,
  unsigned short size, int version));
void ndl_send_test(struct FILE *fp, char *dest);
void ndl_send_xid(struct FILE *fp, char *dest);
void ndl_unsupported_msg(struct FILE *fp, char *msg, unsigned short size);
void ndl_handle_valid_dl_msg(struct FILE *fp, struct NDL$IEEE_HEADER *ip,
  unsigned short size);

/*+
 * ============================================================================
 * = ndl_init - Initialization routine for the NI datalink.                   =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will initialize the NI datalink driver. The protocol
 *	list used for the class driver receive routines is also
 *	initialized here.	
 *
 * FORM OF CALL:
 *
 *	ndl_init(ip,cb,sa,rp);
 *
 * RETURNS:
 *	
 *	int Status - msg_success or FALIURE.
 *
 * ARGUMENTS:
 *
 *	struct INODE *ip - Pointer to the ports inode.
 *	struct PORT_CALLBACK *cb - Pointer to the port callback structure.
 *	unsigned char *sa - Pointer to the ports station address.
 *	void *rp - Pointer to the holder for the receive routine.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int ndl_init(struct INODE *ip, struct PORT_CALLBACK *cb, unsigned char *sa,
  int *rp)
{
    struct NI_GBL *np;			/*Pointer to NI global database*/
    struct NI_DL *dl;			/*Pointer to the datalink*/
    char dev[10];
    int i;

    trace("ndl_init\n", 0);

/*Get the pointers to the datalink info*/
    np = (struct NI_GBL *) ip->misc;
    dl = (struct NI_DL *) np->dlp;

/*Save the pointer to the callback structure*/
    dl->cb = cb;

/*Copy our station address*/
    memcpy(dl->sa, sa, 6);

/*Set the class driver routine*/
    *rp = (int) ndl_receive_msg;

/*Initialize the protocol list and semaphore*/
    dl->nrh.flink = &(dl->nrh);
    dl->nrh.blink = &(dl->nrh);

/*Init the mop driver*/
    mop_init1(ip, dl->sa);

/*Init the lad driver*/
    lad_new_dl(ip->name);

#if ( STARTSHUT || DRIVERSHUT )

/* Init the loop driver */

    loop_init1(ip->name);
#endif

/*Return a success*/
    return (msg_success);
}

/*+
 * ============================================================================
 * = ndl_shutdown - Shutdown routine for the NI datalink.                     =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will shutdown the NI datalink driver. The protocol
 *	list used for the class driver receive routines is also
 *	deleted here.
 *
 * FORM OF CALL:
 *
 *	ndl_shutdown(ip);
 *
 * RETURNS:
 *	
 *	int Status - msg_success or FALIURE.
 *
 * ARGUMENTS:
 *
 *	struct INODE *ip - Pointer to the ports inode.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int ndl_shutdown(struct INODE *ip)
{
    struct NI_GBL *np;			/*Pointer to NI global database*/
    struct NI_DL *dl;			/*Pointer to the datalink*/
    void *qentry;

#if OPTIONAL
/* Shutdown the lad driver */
    lad_shutdown_dl(ip->name);
#endif

/* Shutdown the mop driver,the loop driver and the inet driver */

#if ( STARTSHUT || DRIVERSHUT )
    loop_shutdown(ip->name);

    inet_shutdown();
#endif

    mop_shutdown(ip->name);

/* Get the pointers to the datalink info */

    np = (struct NI_GBL *) ip->misc;
    dl = (struct NI_DL *) np->dlp;

/* Delete the protocol list */

    while (dl->nrh.flink != &dl->nrh) {
	qentry = remq_lock(dl->nrh.flink);
	free(qentry);
    }

/* Free the callback and datalink structures: */

    free(dl->cb);
    free(np->dlp);
    np->dlp = NULL;

/* Return a success */

    return (msg_success);
}

/*+
 * ============================================================================
 * = ndl_receive_msg - Datalink receive message routine.                      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will handle received messages. It is expected that this
 *	routine will be called from a port driver. This routine validates and
 *	classifies the received messages. First, the message format, V3
 *	Ethernet or V4 IEEE, is determined. Next, if V4, any IEEE 802.2 XID
 *	or TEST messages are handled.
 *
 * FORM OF CALL:
 *
 *	ndl_receive_msg(fp,msg,size);
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	struct FILE *fp - Pointer to the file being used.
 *	char *msg - Pointer to the received message.
 *	unsigned short size - Size of the packet received.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void ndl_receive_msg(struct FILE *fp, char *msg, unsigned short size)
{
    int i, j;
    unsigned char *lptr;
    struct NDL$ENET_HEADER *enet;
    struct NDL$IEEE_HEADER *ip;
    unsigned short prot, len;
    unsigned long ui_pid;

    trace("ndl_receive_msg\n", 0);

#if NDL_DEBUG
    pprintf("Message Recieved\n");
    pprint_pkt(msg, size);
#endif

/*We first check and see if we have an ethernet packet*/
    enet = (struct NDL$ENET_HEADER *) msg;
    prot = ((enet->NDL$B_ENET_PROT[0]) << 8) | (enet->NDL$B_ENET_PROT[1]);

/*Check the protocol*/
    if (prot > NDL$K_MAX_DATA_LENGTH)

	/*We have an ethernet packet so now handle the V3 NDL message*/
	ndl_handle_msg(fp, msg, size, NDL$K_VERSION_3);
    else {

	/*We now check and see if we have a NDL sub protocol*/
	ip = (struct NDL$IEEE_HEADER *) msg;
	memcpy((unsigned char *) (&ui_pid),
	  (unsigned char *) (&(ip->NDL$B_IEEE_PROT_UI)), 4);

	/*Check the length*/
	len = ((ip->NDL$B_IEEE_LEN[0]) << 8) | (ip->NDL$B_IEEE_LEN[1]);
	if ((len < NDL$K_MIN_DATA_LENGTH)) {

	    /*Its an invalid message*/
	    ndl_unsupported_msg(fp, msg, size);
	} else if ((ip->NDL$B_IEEE_SSAP == NDL$K_IEEE_SNAP) && (ip->
	  NDL$B_IEEE_DSAP == NDL$K_IEEE_SNAP) && (ui_pid == NDL$K_IEEE_UI_PID)) {

	    /*Check the SSAP, DSAP, the UI ,and PID*/
	    /*and we once again handle the V4 NDL message*/
	    ndl_handle_msg(fp, msg, size, NDL$K_VERSION_4);
	} else {

	    /*We don't so handle the datalink message*/
	    ndl_handle_dl_msg(fp, msg, size);
	}
    }
}

/*+
 * ============================================================================
 * = ndl_handle_dl_msg - Handle a datalink message.                           =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will handle messages that are not destined for a class
 *	driver. If the DSAP and SSAP are valid they are handled if not they
 *	are they are called unsupported.
 *
 * FORM OF CALL:
 *
 *	ndl_handle_dl_msg(fp,msg,size)
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	struct FILE *fp - Pointer to the file being used.
 *	char *msg - Pointer to the datalink message.
 *	unsigned short size - Size of the datalink message.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void ndl_handle_dl_msg(struct FILE *fp, char *msg, unsigned short size)
{
    struct NDL$IEEE_HEADER *ip;

    trace("ndl_handle_dl_msg\n", 0);

/*Get the pointer the packet*/
    ip = (struct NDL$IEEE_HEADER *) msg;

/*First lets see if its a good DSAP*/
    if ((ip->NDL$B_IEEE_DSAP == NDL$K_IEEE_SNAP) | (ip->NDL$B_IEEE_DSAP == 0))

      /*See if its a command or response*/
      {
	if (((ip->NDL$B_IEEE_SSAP) & NDL$M_IEEE_C_R_BIT) == 0)

	    /*Its a command*/
	    ndl_handle_valid_dl_msg(fp, ip, size);
	else

	  /*Its unsupported*/
	    ndl_unsupported_msg(fp, msg, size);
    } else

	/*Its unsupported*/
	ndl_unsupported_msg(fp, msg, size);

}

/*+
 * ============================================================================
 * = ndl_handle_valid_dl_msg - Handle a valid datalink message.               =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will handle messages that are not destined for a class
 *	driver. The messages will either be a test or xid message or invalid.
 *
 * FORM OF CALL:
 *
 *	ndl_handle_valid_dl_msg(fp,ip,size)
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	struct FILE *fp - Pointer to the file being used.
 *	struct NDL$IEEE_HEADER *ip - Pointer to the datalink message.
 *	unsigned short size - Size of the datalink message.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void ndl_handle_valid_dl_msg(struct FILE *fp, struct NDL$IEEE_HEADER *ip,
  unsigned short size)
{
    int i;
    struct NI_GBL *np;			/*Pointer to NI global database*/
    struct NI_DL *dl;			/*Pointer to the datalink*/
    struct NDL$IEEE_XID *xp;		/*Pointer to XID message*/
    unsigned short len;			/*Length*/
    U_INT_8 temp, prot_ui;

    trace("ndl_handle_valid_dl_msg\n", 0);

/*Get the pointer to the datalink*/
    np = (struct NI_GBL *) fp->ip->misc;
    dl = (struct NI_DL *) np->dlp;

/*Mask out the poll/final bit*/
    prot_ui = ip->NDL$B_IEEE_PROT_UI & (~NDL$M_IEEE_P_F_BIT);

/*Get the length*/
    len = ((ip->NDL$B_IEEE_LEN[0]) << 8) | (ip->NDL$B_IEEE_LEN[1]);

/*Do a length check*/
    if (size > NDL$K_MIN_PKT_LENGTH) {

	/*Size must be equal to length + header*/
	if (size != (len + NDL$K_IEEE_HEADER_LEN)) {
	    ndl_unsupported_msg(fp, (char *) ip, size);
	    return;
	}
    } else if ((len + NDL$K_IEEE_HEADER_LEN) > NDL$K_MIN_PKT_LENGTH) {
	ndl_unsupported_msg(fp, (char *) ip, size);
	return;
    }

/*See what kind of message it is*/
    if (prot_ui == NDL$K_IEEE_CTL_XID) {

	/*We've got an XID message send the response*/
	/*Fill in the info using what we already have*/
	for (i = 0; i < 6; i++)
	    ip->NDL$B_IEEE_DEST[i] = ip->NDL$B_IEEE_SOURCE[i];
	for (i = 0; i < 6; i++)
	    ip->NDL$B_IEEE_SOURCE[i] = dl->sa[i];
	temp = ip->NDL$B_IEEE_DSAP;
	ip->NDL$B_IEEE_DSAP = ip->NDL$B_IEEE_SSAP;
	ip->NDL$B_IEEE_SSAP = (temp) | NDL$M_IEEE_C_R_BIT;

	/*Get a pointer to the transact ID part and fill it in*/
	xp = (struct NDL$IEEE_XID *) (ip->NDL$B_IEEE_PROT_PREFIX);
	xp->NDL$B_XID_FORMAT = NDL$K_XID_FORMAT;
	xp->NDL$B_XID_CLASS = NDL$K_XID_CLASS;
	xp->NDL$B_XID_WINDOW = NDL$K_XID_WINDOW;

	/*Send the message*/
	fwrite(ip, 1, len + NDL$K_IEEE_HEADER_LEN, fp);
    } else if (prot_ui == NDL$K_IEEE_CTL_TEST) {

	/*We've got an Test message send the response*/
	/*Fill in the info using what we already have*/
	for (i = 0; i < 6; i++)
	    ip->NDL$B_IEEE_DEST[i] = ip->NDL$B_IEEE_SOURCE[i];
	for (i = 0; i < 6; i++)
	    ip->NDL$B_IEEE_SOURCE[i] = dl->sa[i];
	temp = ip->NDL$B_IEEE_DSAP;
	ip->NDL$B_IEEE_DSAP = ip->NDL$B_IEEE_SSAP;
	ip->NDL$B_IEEE_SSAP = (temp) | NDL$M_IEEE_C_R_BIT;

	/*Send the message*/
	fwrite(ip, 1, len + NDL$K_IEEE_HEADER_LEN, fp);
    } else

	/*Its unsupported*/
	ndl_unsupported_msg(fp, (char *) ip, size);
}

/*+
 * ============================================================================
 * = ndl_handle_msg - Handle message.                                         =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will handle messages. The type of message version 3
 *	or version 4 is specified with the call. The routine figures out
 *	which version message it is, then looks at the protocol and then
 *	takes the appropriate action.
 *
 * FORM OF CALL:
 *
 *	ndl_handle_msg(fp,msg,size,version)
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	struct FILE *fp - Pointer to the file being used.
 *	char *msg - Pointer to the MOP message.
 *	unsigned short size - Size of the MOP message.
 *	int version - Version of the MOP message.
 *		      NDL$K_VERSION_3 = versions 1-3.
 *		      NDL$K_VERSION_4 = version 4.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void ndl_handle_msg(struct FILE *fp, char *msg, unsigned short size,
  int version)
{
    struct NDL$ENET_HEADER *enet;	/*Message pointers*/
    struct NDL$IEEE_HEADER *ieee;
    unsigned char *source;		/*Source address*/
    unsigned short prot, is;		/*Protocol,size of the info*/
    struct NI_GBL *np;			/*Pointer to NI global database*/
    struct MOP$_V4_COUNTERS *mc;	/*Pointer to the MOP counters*/
    struct NI_DL *dl;			/*Pointer to the datalink*/
    struct NI_RCV_E *cr;		/*Pointer to the Receive elements*/
    unsigned char *ip;			/*Pointer to the message information*/
    int i;

    trace("ndl_handle_msg\n", 0);

/*Get the version specific informantion*/
    if (version == NDL$K_VERSION_3) {
	enet = (struct NDL$ENET_HEADER *) msg;
	source = &(enet->NDL$B_ENET_SOURCE[0]);
	prot = ((enet->NDL$B_ENET_PROT[0]) << 8) | (enet->NDL$B_ENET_PROT[1]);

	/*The info size is size of the message - the header*/
	is = size - (sizeof(struct NDL$ENET_HEADER) - 2);

	/*The pointer is the msg + the header - the length*/
	ip = (unsigned char *) ((long) msg +
	  (sizeof(struct NDL$ENET_HEADER) - 2));
    } else {
	ieee = (struct NDL$IEEE_HEADER *) msg;
	source = &(ieee->NDL$B_IEEE_SOURCE[0]);
	prot = ((ieee->NDL$B_IEEE_PROT[0]) << 8) | (ieee->NDL$B_IEEE_PROT[1]);
	is = ((ieee->NDL$B_IEEE_LEN[0]) << 8) | (ieee->NDL$B_IEEE_LEN[1]);
	is -= NDL$K_IEEE_H_SIZE;
	ip = (unsigned char *) ((long) msg + sizeof(struct NDL$IEEE_HEADER));
    }

/*Get the pointer to the datalink*/
    np = (struct NI_GBL *) fp->ip->misc;
    dl = (struct NI_DL *) np->dlp;
    mc = (struct MOP$_V4_COUNTERS *) np->mcp;

/*Find the appropriate class driver receive routine*/
/*Search the list*/
    spinlock(&spl_kernel);
    cr = dl->nrh.flink;
    while (cr != &(dl->nrh)) {

	/*Does the protocol match*/
	/*We've got it so we're done*/
	if (cr->prot == prot)
	    break;
	else

	  /*Get the next element*/
	    cr = cr->flink;
    }
    spinunlock(&spl_kernel);

/*If we found the element execute the routine if not error*/
    if (cr != &(dl->nrh)) {

	/*If its multicast make sure its correct or enabled*/
	if (msg[0] & 1) {

	    /*Check the address*/
	    if (memcmp(cr->mca, msg, 6)) {

		/*The multicast address is not correct or enabled*/
		/*Take care of the mop counter*/
		_addq_v_u((mc->MOP_V4CNT_UNKNOWN_DESTINATION.l), 1);

		/*Free the buffer*/
		ndl_deallocate_pkt(fp->ip, msg);
	    } else

		/*Call the routine*/
		(*(cr->rp))(fp, ip, source, is, version);
	} else

	    /*Simply call the routine*/
	    (*(cr->rp))(fp, ip, source, is, version);

    } else {

	/*Update the mop counter*/
	_addq_v_u((mc->MOP_V4CNT_UNKNOWN_DESTINATION.l), 1);

	/*Free the buffer*/
	ndl_deallocate_pkt(fp->ip, msg);
    }
}

/*+
 * ============================================================================
 * = ndl_get_nisa - Gets the NI station address.                              =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will get the NI station address and move to the specified
 *	address. 
 *
 * FORM OF CALL:
 *
 *	ndl_get_nisa (fp,sa); 
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *      struct FILE *fp - Pointer to the file being used.
 *	char *sa - Pointer to be used for the station address.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void ndl_get_nisa(struct FILE *fp, unsigned char *sa)
{
    struct NI_GBL *np;			/*Pointer to NI global database*/
    struct NI_DL *dl;			/*Pointer to the datalink*/
    int i;				/*index*/

    trace("ndl_get_nisa\n", 0);

/*Get the pointers to the datalink info*/
    np = (struct NI_GBL *) fp->ip->misc;
    dl = (struct NI_DL *) np->dlp;

/*Get the address*/
    for (i = 0; i < 6; i++)
	sa[i] = dl->sa[i];
}

/*+
 * ============================================================================
 * = ndl_allocate_msg - Allocates a message.                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine allocates a message to be filled in by the caller.
 *	Memory for the message is allocated. A pointer to the information
 *	part of the message is returned. This is different depending on which
 *	version is set in the argument. 
 *
 * FORM OF CALL:
 *
 *	ndl_allocate_msg (ip,version,inc_len); 
 *
 * RETURNS:
 *
 *	void *msg = Pointer to the information part of the message.
 *
 * ARGUMENTS:
 *
 *	struct INODE *ip - Inode of the current port.
 *      int version - Version packet to be allocated.
 *		      NDL$K_VERSION_3 = versions 1-3.
 *		      NDL$K_VERSION_4 = version 4.
 *	int inc_len - Specifies whether or not we include the length field
 *			for V3 packets.
 *			NDL$K_DONT_INC_LENGTH = Don't include the length.
 *			NDL$K_INC_LENGTH 1 = Include the length field.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void *ndl_allocate_msg(struct INODE *ip, int version, int inc_len)
{
    char *msg, *inp;

    trace("ndl_allocate_msg\n", 0);

/*Allocate the message*/
    msg = (char *) ndl_allocate_pkt(ip, 0);
    if (version == NDL$K_VERSION_3) {
	if (inc_len)
	    inp = (char *) ((long) msg + sizeof(struct NDL$ENET_HEADER));
	else
	    inp = (char *) ((long) msg + sizeof(struct NDL$ENET_HEADER) - 2);
    } else
	inp = (char *) ((long) msg + sizeof(struct NDL$IEEE_HEADER));

/*Return it*/
#if NDL_DEBUG
#if 0
    printf("allocate msg: %08x inp: %08x\n", msg, inp);
#endif
#endif
    return (inp);
}

/*+
 * ============================================================================
 * = ndl_deallocate_msg - Deallocates a message.                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine deallocates a message.
 *
 * FORM OF CALL:
 *
 *	ndl_deallocate_msg (ip,inp,version,inc_len); 
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	struct INODE *ip - Inode of the current port.
 *	void *inp = Pointer to the information part of the message.
 *      int version - Version packet to be allocated.
 *		      NDL$K_VERSION_3 = versions 1-3.
 *		      NDL$K_VERSION_4 = version 4.
 *	int inc_len - Specifies whether or not we include the length field
 *			for V3 packets.
 *			NDL$K_DONT_INC_LENGTH = Don't include the length.
 *			NDL$K_INC_LENGTH 1 = Include the length field.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void ndl_deallocate_msg(struct INODE *ip, void *inp, int version, int inc_len)
{
    void *msg;

    trace("ndl_deallocate_msg\n", 0);

/*Allocate the message*/
    if (version == NDL$K_VERSION_3)
	if (inc_len)
	    msg = (char *) ((long) inp - sizeof(struct NDL$ENET_HEADER));
	else
	    msg = (char *) ((long) inp - (sizeof(struct NDL$ENET_HEADER) - 2));
    else
	msg = (char *) ((long) inp - sizeof(struct NDL$IEEE_HEADER));

/*Free the message*/
#if NDL_DEBUG
#if 1
    printf("Freeing address: %08x\n", msg);
#endif
#endif
    ndl_deallocate_pkt(ip, msg);
}

/*+
 * ============================================================================
 * = ndl_reuse_msg - Reuse a message.                                         =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will fix up a message so it can be reused.
 *
 * FORM OF CALL:
 *
 *	ndl_reuse_msg (fp,msg,ip,version,inc_len,is); 
 *
 * RETURNS:
 *
 *      unsigned short ms - Size of the entire message.
 *
 * ARGUMENTS:
 *
 *      struct FILE *fp - Pointer to the file being used.
 *	void **msg = Place where the routine can put a pointer to the
 *			message.
 *	unsigned char *ip = Pointer to the information part of the message.
 *      int version - Version packet to be allocated.
 *		      NDL$K_VERSION_3 = versions 1-3.
 *		      NDL$K_VERSION_4 = version 4.
 *	int inc_len - Specifies whether or not we include the length field
 *			for V3 packets.
 *			NDL$K_DONT_INC_LENGTH = Don't include the length.
 *			NDL$K_INC_LENGTH 1 = Include the length field.
 *      int is - Size of the information part of the message.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

unsigned short ndl_reuse_msg(struct FILE *fp, unsigned char **msg,
  unsigned char *ip, int version, int inc_len, unsigned short is)
{
    struct NI_GBL *np;			/*Pointer to NI global database*/
    struct NI_DL *dl;			/*Pointer to the datalink*/
    unsigned short ms;			/*Complete message size*/
    struct NDL$IEEE_HEADER *ieee;
    struct NDL$GEN_HEADER *gen;

/*Do packet specific stuff*/

    if (version == NDL$K_VERSION_3) {
	if (inc_len)

	    /*Get the message size*/
	    ms = (sizeof(struct NDL$ENET_HEADER) + is);
	else

	  /*Get the message size without the length*/
	    ms = (is + (sizeof(struct NDL$ENET_HEADER) - 2));

	/*Pointer to the generic part of the packet*/
	gen = (struct NDL$GEN_HEADER *) ((long) ip -
	  (sizeof(struct NDL$ENET_HEADER) - 2));
    } else {

	/*Get the message size*/
	ms = (sizeof(struct NDL$IEEE_HEADER) + is);

	/*The ieee packet*/
	ieee = (struct NDL$IEEE_HEADER *) ((long) ip -
	  sizeof(struct NDL$IEEE_HEADER));
	gen = (struct NDL$GEN_HEADER *) ieee;

	/*Fill in the info*/
	ieee->NDL$B_IEEE_LEN[0] = (unsigned char) ((is >> 8) & 0xff);
	ieee->NDL$B_IEEE_LEN[1] = (unsigned char) (is & 0xff);
	ieee->NDL$B_IEEE_DSAP = NDL$K_IEEE_SNAP;
	ieee->NDL$B_IEEE_SSAP = NDL$K_IEEE_SNAP;
	*(unsigned long *) &(ieee->NDL$B_IEEE_PROT_UI) = NDL$K_IEEE_UI_PID;
    }

/*Get the pointers to the datalink info*/
    np = (struct NI_GBL *) fp->ip->misc;
    dl = (struct NI_DL *) np->dlp;

/*Fill in general info*/
    memcpy(gen->NDL$B_GEN_DEST, gen->NDL$B_GEN_SOURCE, 6);
    memcpy(gen->NDL$B_GEN_SOURCE, dl->sa, 6);

/*Return the message pointer and size*/
    *msg = (unsigned char *) gen;
    return (ms);
}

/*+
 * ============================================================================
 * = ndl_allocate_pkt - Allocates a general message.                          =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine allocates a packet. For each packet the maximum size
 *	ethernet packet is allocated. The zone the packet is allocated from
 *	is specified from the environment variable. The alignment is
 *	specified from the arguments.
 *
 * FORM OF CALL:
 *
 *	ndl_allocate_pkt (ip,rem_mask); 
 *
 * RETURNS:
 *
 *	void *pkt = Pointer to the packet.
 *
 * ARGUMENTS:
 *
 *	struct INODE *ip - Inode of the current port.
 *	int rem_mask - Remainder mask of the packet being allocated
 *			this is done to force an alignment.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void *ndl_allocate_pkt(struct INODE *ip, int rem_mask)
{
    struct NI_GBL *np;			/* Pointer to the NI global database*/
    struct NI_ENV *ev;			/*Pointer to the environment
					 * variables*/
    struct ZONE *zone;			/*Memory zone used      */
    int options;			/*Options for malloc*/
    int mod, rem;			/*Modulus and remainder*/
    void *pkt;				/*Pointer to the packet allocated*/

/*Get the pointer to the NI global block*/

    np = (struct NI_GBL *) ip->misc;
/*Get the pointer to the environment variables*/
    ev = (struct NI_ENV *) np->enp;

/*Get the options we need*/
    options = DYN$K_SYNC | DYN$K_FLOOD | DYN$K_NOOWN | DYN$K_ALIGN;

/*Check for zones*/
    if (ev->driver_flags & NDL$M_MEMZONE) {
	options |= DYN$K_ZONE;
	zone = memzone;
    }

/*Check for alignment*/
    if (ev->msg_mod || ev->msg_rem) {
	mod = ev->msg_mod;
	rem = ev->msg_rem & ~rem_mask;
    } else {
	mod = NDL$K_PKT_MOD;
	rem = 0 & ~rem_mask;
    }

/*Allocate the packet*/
    pkt = (void *) dyn$_malloc(NDL$K_PACK_ALLOC, options, mod, rem, zone);

/*Return the packet*/
    return (pkt);
}

/*+
 * ============================================================================
 * = ndl_deallocate_pkt - Deallocates a packet.                               =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine deallocates a packet.
 *
 * FORM OF CALL:
 *
 *	ndl_deallocate_pkt (ip,pkt); 
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	struct INODE *ip - Inode of the current port.
 *	void *pkt = Pointer to the packet.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void ndl_deallocate_pkt(struct INODE *ip, void *pkt)
{

/*Free the packet*/
    free(pkt);
}

/*+
 * ============================================================================
 * = ndl_send - Sends a message.                                              =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine sends  a message. The routine will check the version
 *	then fill in the appropriate fields.
 *
 * FORM OF CALL:
 *
 *	ndl_send(fp,version,ip,is,inc_len,dest,type);
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *      struct FILE *fp - Pointer to the file being used.
 *      int version - Version packet to be allocated.
 *		      NDL$K_NO_VERSION = Pass the message directly.
 *		      NDL$K_VERSION_3 = versions 1-3.
 *		      NDL$K_VERSION_4 = version 4.
 *	void *ip - Pointer to the information oart of the message.
 *      int is - Size of the information part of the message.
 *	int inc_len - Specifies whether or not we include the length field
 *			for V3 packets.
 *			NDL$K_DONT_INC_LENGTH = Don't include the length.
 *			NDL$K_INC_LENGTH 1 = Include the length field.
 *	char *dest - destination of the message.
 *	unsigned short type - The protocol type.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void ndl_send(struct FILE *fp, int version, void *ip, int is, int inc_len,
  unsigned char *dest, unsigned short type)
{
    int ms, i, v4s;
    struct NDL$ENET_HEADER *enet;
    struct NDL$IEEE_HEADER *ieee;
    struct NDL$GEN_HEADER *gen;
    struct NI_GBL *np;			/*Pointer to NI global database*/
    struct NI_DL *dl;			/*Pointer to the datalink*/
    unsigned long temp;			/*Needed for correct alignment*/

    trace("ndl_send\n", 0);

/*Do packet specific stuff*/
    if (version == NDL$K_NO_VERSION) {

	/*Get the message size*/
	ms = is;

	/*Pointer to the generic part of the packet*/
	gen = (struct NDL$GEN_HEADER *) ip;
    } else if (version == NDL$K_VERSION_3) {
	if (inc_len) {

	    /*Get the message size*/
	    ms = (sizeof(struct NDL$ENET_HEADER) + is);

	    /*The enet packet*/
	    enet = (struct NDL$ENET_HEADER *) ((long) ip -
	      sizeof(struct NDL$ENET_HEADER));

	    /*Write the length*/
	    enet->NDL$B_ENET_LEN = is;
	} else {

	    /*Get the message size without the length*/
	    ms = (is + (sizeof(struct NDL$ENET_HEADER) - 2));
	    enet = (struct NDL$ENET_HEADER *) ((long) ip -
	      (sizeof(struct NDL$ENET_HEADER) - 2));
	}

	/*Pointer to the generic part of the packet*/
	gen = (struct NDL$GEN_HEADER *) enet;

	/*Set the protocol*/
	enet->NDL$B_ENET_PROT[0] = (unsigned char) ((type >> 8) & 0xff);
	enet->NDL$B_ENET_PROT[1] = (unsigned char) (type & 0xff);
    } else {

	/*Get the message size*/
	ms = (sizeof(struct NDL$IEEE_HEADER) + is);

	/*Get the v4 size*/
	v4s = is + NDL$K_IEEE_H_SIZE;

	/*The ieee packet*/
	ieee = (struct NDL$IEEE_HEADER *) ((long) ip -
	  sizeof(struct NDL$IEEE_HEADER));
	gen = (struct NDL$GEN_HEADER *) ieee;

	/*Fill in the info*/
	ieee->NDL$B_IEEE_LEN[0] = (unsigned char) ((v4s >> 8) & 0xff);
	ieee->NDL$B_IEEE_LEN[1] = (unsigned char) (v4s & 0xff);
	ieee->NDL$B_IEEE_DSAP = NDL$K_IEEE_SNAP;
	ieee->NDL$B_IEEE_SSAP = NDL$K_IEEE_SNAP;
	temp = NDL$K_IEEE_UI_PID;
	memcpy((unsigned char *) (&(ieee->NDL$B_IEEE_PROT_UI)),
	  (unsigned char *) (&temp), 4);
	ieee->NDL$B_IEEE_PROT[0] = (unsigned char) ((type >> 8) & 0xff);
	ieee->NDL$B_IEEE_PROT[1] = (unsigned char) (type & 0xff);
    }

/*Get the pointers to the datalink info*/
    np = (struct NI_GBL *) fp->ip->misc;
    dl = (struct NI_DL *) np->dlp;

/*Fill in general info*/
    memcpy(gen->NDL$B_GEN_DEST, dest, 6);
    memcpy(gen->NDL$B_GEN_SOURCE, dl->sa, 6);

#if NDL_DEBUG
#if 0
    print_pkt(gen, ms);
#endif
#endif

/*Send the packet*/
    fwrite(gen, 1, ms, fp);
}

/*+
 * ============================================================================
 * = ndl_preallocate - Preallocates a message.                                =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine preallocates and builds a message. This is to be 
 *	used for example to make a list of messages with length etc.
 *	already filled in. The size of the memory allocated will always be
 *	the max enet packet size. This is so it will come from and got
 *	to the enet private lookaside list.
 *
 * FORM OF CALL:
 *
 *	ndl_preallocate(ip,version,msg,inp,is,inc_len,type);
 *
 * RETURNS:
 *
 *	int size - size of the message allocated;
 *
 * ARGUMENTS:
 *
 *      struct INODE *ip - Pointer to the inde being used.
 *	unsigned char **msg - Pointer to the entire message preallocated.
 *	unsigned char **inp - Pointer to the information part of the message
 *			     preallocated.
 *      int version - Version packet to be allocated.
 *		      NDL$K_VERSION_3 = versions 1-3.
 *		      NDL$K_VERSION_4 = version 4.
 *	int inc_len - Specifies whether or not we include the length field
 *			for V3 packets.
 *			NDL$K_DONT_INC_LENGTH = Don't include the length.
 *			NDL$K_INC_LENGTH 1 = Include the length field.
 *      unsigned short is - Size of the information part of the message.
 *	unsigned short type - The protocol type.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int ndl_preallocate(struct INODE *ip, unsigned char **msg, unsigned char **inp,
  int version, int inc_len, unsigned short is, unsigned short type)
{
    int ms, i, v4s;
    unsigned long temp;			/*Needed for correct alignment*/
    struct NDL$ENET_HEADER *enet;
    struct NDL$IEEE_HEADER *ieee;
    struct NDL$GEN_HEADER *gen;
    struct NI_GBL *np;			/*Pointer to NI global database*/
    struct NI_DL *dl;			/*Pointer to the datalink*/

    trace("ndl_preallocate\n", 0);

/*Do message specific stuff*/
    if (version == NDL$K_VERSION_3) {
	if (inc_len) {

	    /*Get the message size*/
	    ms = (sizeof(struct NDL$ENET_HEADER) + is);

	    /*Allocate the message*/
	    enet = ndl_allocate_pkt(ip, 0);

	    /*Write the length*/
	    enet->NDL$B_ENET_LEN = is;

	    /*Pass the message pointers*/
	    *inp =
	      (unsigned char *) ((long) enet + sizeof(struct NDL$ENET_HEADER));
	} else {

	    /*Get the message size without the length*/
	    ms = (is + (sizeof(struct NDL$ENET_HEADER) - 2));

	    /*Allocate the message*/
	    enet = ndl_allocate_pkt(ip, 0);

	    /*Pass the message pointers*/
	    *inp = (unsigned char *) ((long) enet +
	      (sizeof(struct NDL$ENET_HEADER) - 2));
	}

	/*Pointer to the generic part of the packet*/
	gen = (struct NDL$GEN_HEADER *) enet;

	/*Set the protocol*/
	enet->NDL$B_ENET_PROT[0] = (unsigned char) ((type >> 8) & 0xff);
	enet->NDL$B_ENET_PROT[1] = (unsigned char) (type & 0xff);
    } else {

	/*Get the message size*/
	ms = (sizeof(struct NDL$IEEE_HEADER) + is);

	/*Get the v4 size*/
	v4s = is + NDL$K_IEEE_H_SIZE;

	/*Allocate the message*/
	ieee = ndl_allocate_pkt(ip, 0);

	/*Get the general portion of the message*/
	gen = (struct NDL$GEN_HEADER *) ieee;

	/*Fill in the info*/
	ieee->NDL$B_IEEE_LEN[0] = (unsigned char) ((v4s >> 8) & 0xff);
	ieee->NDL$B_IEEE_LEN[1] = (unsigned char) (v4s & 0xff);
	ieee->NDL$B_IEEE_DSAP = NDL$K_IEEE_SNAP;
	ieee->NDL$B_IEEE_SSAP = NDL$K_IEEE_SNAP;
	temp = NDL$K_IEEE_UI_PID;
	memcpy((unsigned char *) (&(ieee->NDL$B_IEEE_PROT_UI)),
	  (unsigned char *) (&temp), 4);
	ieee->NDL$B_IEEE_PROT[0] = (unsigned char) ((type >> 8) & 0xff);
	ieee->NDL$B_IEEE_PROT[1] = (unsigned char) (type & 0xff);

	/*Pass the pointer*/
	*inp =
	  (unsigned char *) ((long) ieee + sizeof(struct NDL$IEEE_HEADER));
    }

/*Get the pointers to the datalink info*/
    np = (struct NI_GBL *) ip->misc;
    dl = (struct NI_DL *) np->dlp;

/*Get the msg pointer*/
    *msg = (unsigned char *) gen;

#if NDL_DEBUG
#if 0
    printf("msg: %08x inp: %08x\n", *msg, *inp);
    print_pkt(gen, ms);
#endif
#endif

/*Return the message size*/
    return (ms);
}

/*+
 * ============================================================================
 * = ndl_set_rcv - Sets the class driver receive routine.                     =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will set the class driver receive routine. It will
 *	insert an element into the receive queue.
 *
 * FORM OF CALL:
 *
 *	ndl_set_rcv (ip,prot,mca,rp); 
 *
 * RETURNS:
 *
 *	int Status - msg_success or FALIURE.
 *
 * ARGUMENTS:
 *
 *      struct INODE *ip - Pointer to the port inode.
 *      prot - Protocol to be used.
 *	unsigned char *mca - Pointer to a multicast address that is to be
 *			     checked before calling the receive routine.
 *      rp - Pointer to a datalink routine that receives messages.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void ndl_set_rcv(struct INODE *ip, unsigned short prot, unsigned char *mca,
  void (*rp)(struct FILE *fp, unsigned char *msg, unsigned char *source,
  unsigned short size, int version))
{
    struct NI_GBL *np;			/*Pointer to NI global database*/
    struct NI_DL *dl;			/*Pointer to the datalink*/
    struct NI_RCV_E *rcv;		/*Pointer to the Receive element*/
    int i;
    int ipl;

    trace("ndl_set_rcv\n", 0);

/*Get the pointer to the datalink*/
    np = (struct NI_GBL *) ip->misc;
    dl = (struct NI_DL *) np->dlp;

/*Set up the new element*/
    rcv = (struct NI_RCV_E *) malloc(sizeof(struct NI_RCV_E));
    rcv->prot = prot;
    for (i = 0; i < 6; i++)
	rcv->mca[i] = mca[i];
    rcv->rp = rp;

/*Insert the element in the queue*/
    insq_lock(&(rcv->flink), dl->nrh.blink);
}

/*+
 * ============================================================================
 * = ndl_send_xid - Sends the transact ID message.                            =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will send the transact ID message.
 *
 * FORM OF CALL:
 *
 *	ndl_send_xid(fp,dest);
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	struct FILE *fp - Pointer to the file beign used.
 *	char *dest - Pointer to 6 bytes containing the destination address.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void ndl_send_xid(struct FILE *fp, char *dest)
{
    struct NI_GBL *np;			/*Pointer to NI global database*/
    struct NI_DL *dl;			/*Pointer to the datalink*/
    struct NDL$IEEE_HEADER *mp;		/*Message pointer*/
    struct NDL$IEEE_XID *xp;		/*Pointer to XID message*/
    int i, size;			/*Size of the message*/

    trace("ndl_send_xid\n", 0);

/*Get the pointer to the datalink*/
    np = (struct NI_GBL *) fp->ip->misc;
    dl = (struct NI_DL *) np->dlp;

/*Allocate the message*/
    size = sizeof(struct NDL$IEEE_HEADER);
    mp = (struct NDL$IEEE_HEADER *) ndl_allocate_pkt(fp->ip, 0);

/*Fill in the info*/
    for (i = 0; i < 6; i++)
	mp->NDL$B_IEEE_DEST[i] = dest[i];
    for (i = 0; i < 6; i++)
	mp->NDL$B_IEEE_SOURCE[i] = dl->sa[i];
    mp->NDL$B_IEEE_LEN[0] = (unsigned char) ((size >> 8) & 0xff);
    mp->NDL$B_IEEE_LEN[1] = (unsigned char) (size & 0xff);
    mp->NDL$B_IEEE_DSAP = NDL$K_IEEE_SNAP;
    mp->NDL$B_IEEE_SSAP = NDL$K_IEEE_SNAP;
    mp->NDL$B_IEEE_PROT_UI = NDL$K_IEEE_CTL_XID;
/*Get a pointer to the transact ID part and fill it in*/
    xp = (struct NDL$IEEE_XID *) (mp->NDL$B_IEEE_PROT_PREFIX);
    xp->NDL$B_XID_FORMAT = NDL$K_XID_FORMAT;
    xp->NDL$B_XID_CLASS = NDL$K_XID_CLASS;
    xp->NDL$B_XID_WINDOW = NDL$K_XID_WINDOW;

/*Send the message*/
    fwrite(mp, 1, size, fp);
}

/*+
 * ============================================================================
 * = ndl_send_test - Sends the test message.                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will send the test message.
 *
 * FORM OF CALL:
 *
 *	ndl_send_test(fp,dest);
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	struct FILE *fp - Pointer to the file beign used.
 *	char *dest - Pointer to 6 bytes containing the destination address.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void ndl_send_test(struct FILE *fp, char *dest)
{
    struct NI_GBL *np;			/*Pointer to NI global database*/
    struct NI_DL *dl;			/*Pointer to the datalink*/
    struct NDL$IEEE_HEADER *mp;		/*Message pointer*/
    int i, size;			/*Size of the message*/

    trace("ndl_send_test\n", 0);

/*Get the pointer to the datalink*/
    np = (struct NI_GBL *) fp->ip->misc;
    dl = (struct NI_DL *) np->dlp;

/*Allocate the message*/
    size = sizeof(struct NDL$IEEE_HEADER);
    mp = (struct NDL$IEEE_HEADER *) ndl_allocate_pkt(fp->ip, 0);

/*Fill in the info*/
    for (i = 0; i < 6; i++)
	mp->NDL$B_IEEE_DEST[i] = dest[i];
    for (i = 0; i < 6; i++)
	mp->NDL$B_IEEE_SOURCE[i] = dl->sa[i];
    mp->NDL$B_IEEE_LEN[0] = (unsigned char) ((size >> 8) & 0xff);
    mp->NDL$B_IEEE_LEN[1] = (unsigned char) (size & 0xff);
    mp->NDL$B_IEEE_DSAP = NDL$K_IEEE_SNAP;
    mp->NDL$B_IEEE_SSAP = NDL$K_IEEE_SNAP;
    mp->NDL$B_IEEE_PROT_UI = NDL$K_IEEE_CTL_TEST;
    for (i = 0; i < 3; i++)
	mp->NDL$B_IEEE_PROT_PREFIX[i] = 0;
    for (i = 0; i < 2; i++)
	mp->NDL$B_IEEE_PROT[i] = 0;

/*Send the message*/
    fwrite(mp, 1, size, fp);
}

/*+
 * ============================================================================
 * = ndl_unsupported_msg - Handles an unsupported datalink message.           =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will handle unsupported messages.
 *
 * FORM OF CALL:
 *
 *	 ndl_unsupported_msg(fp,msg,size);
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	struct FILE *fp - Pointer to the file being used.
 *	char *msg - Pointer to the MOP message.
 *	unsigned short size - Size of the MOP message.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void ndl_unsupported_msg(struct FILE *fp, char *msg, unsigned short size)
{
    struct NI_GBL *np;			/*Pointer to NI global database*/
    struct MOP$_V4_COUNTERS *mc;	/*Pointer to the MOP counters*/

    trace("ndl_unsupported_message\n", 0);

/*Get the pointer to the MOP counters*/
    np = (struct NI_GBL *) fp->ip->misc;
    mc = (struct MOP$_V4_COUNTERS *) np->mcp;

/*Display a message*/
#if NDL_DEBUG
    printf("An unsupported message was received.\n");
    print_pkt(msg, size);
#endif

/*Update the mop counters*/
    _addq_v_u((mc->MOP_V4CNT_UNKNOWN_DESTINATION.l), 1);

/*Free the buffer*/
    ndl_deallocate_pkt(fp->ip, msg);
}

/*+
 * ============================================================================
 * = ndl_rcv_info_size - Gets the size of the Receive info.                   =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will return the size of the maximum receive information
 *	field. This is simply the size of the receive buffer minus the header.
 *
 * FORM OF CALL:
 *
 *	 ndl_rcv_info_size(fp,version,inc_len);
 *
 * RETURNS:
 *
 *	short int size - Size of the info part of the message.
 *
 * ARGUMENTS:
 *
 *	struct FILE *fp - Pointer to the file beign used.
 *      int version - Version packet to be allocated.
 *		      NDL$K_VERSION_3 = versions 1-3.
 *		      NDL$K_VERSION_4 = version 4.
 *	int inc_len - Specifies whether or not we include the length field
 *			for V3 packets.
 *			NDL$K_DONT_INC_LENGTH = Don't include the length.
 *			NDL$K_INC_LENGTH 1 = Include the length field.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

short int ndl_rcv_info_size(struct FILE *fp, int version, int inc_len)
{
    struct NI_GBL *np;			/*Pointer to NI global database*/
    struct NI_ENV *ev;			/*Pointer to the environment
					 * variables*/

    trace("ndl_rcv_info_size\n", 0);

/*Get the pointer to the environment variables*/
    np = (struct NI_GBL *) fp->ip->misc;
    ev = (struct NI_ENV *) np->enp;

/*Check the version then return the right size*/
    if (version == NDL$K_VERSION_3)
	if (inc_len)
	    return ((short int) (ev->msg_buf_size) -
	      sizeof(struct NDL$ENET_HEADER));
	else
	    return ((short int) (ev->msg_buf_size) -
	      sizeof(struct NDL$ENET_HEADER) + 2);
    else
	return ((short int) (ev->msg_buf_size) -
	  sizeof(struct NDL$IEEE_HEADER));
}

/*+
 * ============================================================================
 * = print_memory - Display some memory.                                      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will print some rows of memory.
 *
 * FORM OF CALL:
 *
 *	print_memory (ptr_in,rows); 
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	void *ptr_in - Pointer to the memory to be printed.
 *	int rows - Number of rows to be printed.
 *
 *  SIDE EFFECTS:
 *
 *      none
 *
-*/

void print_memory(void *ptr_in, int rows)
{
    long *ptr_out;			/*Starting memory address*/
    int i, j;				/*Counters*/

    ptr_out = (long *) ptr_in;
    for (i = 0; i < rows; i++) {
	ptr_out += 3;
	for (j = 0; j < 4; j++)
	    pprintf(" %08x", *ptr_out--);
	pprintf(" :%08x\n", ++ptr_out);
	ptr_out += 4;
    }
}

/*+
 * ============================================================================
 * = init_mop_counters - Initializes the MOP counters.                        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine initializes the MOP counters.
 *
 * FORM OF CALL:
 *
 *	init_mop_counters (mc); 
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	struct MOP$_V4_COUNTERS *mc - Pointer to the MOP counter block.
 *
 *  SIDE EFFECTS:
 *
 *      None
 *
-*/

void init_mop_counters(struct MOP$_V4_COUNTERS *mc)
{

    trace("init_mop_counters\n", 0);

/*Init the counters*/
    *(UINT *) (&(mc->MOP_V4CNT_TIME_RELATIVE.l)) = *(UINT *) poweron_msec;
    mc->MOP_V4CNT_TIME_INACCURACY = MOP$K_1MS_BY_100NS;
    mc->MOP_V4CNT_TIME_VERSION = MOP$K_TIME_VERSION;
    mc->MOP_V4CNT_RX_BYTES.l = 0;
    mc->MOP_V4CNT_RX_BYTES.h = 0;
    mc->MOP_V4CNT_TX_BYTES.l = 0;
    mc->MOP_V4CNT_TX_BYTES.h = 0;
    mc->MOP_V4CNT_RX_FRAMES.l = 0;
    mc->MOP_V4CNT_RX_FRAMES.h = 0;
    mc->MOP_V4CNT_TX_FRAMES.l = 0;
    mc->MOP_V4CNT_TX_FRAMES.h = 0;
    mc->MOP_V4CNT_RX_MCAST_BYTES.l = 0;
    mc->MOP_V4CNT_RX_MCAST_BYTES.h = 0;
    mc->MOP_V4CNT_RX_MCAST_FRAMES.l = 0;
    mc->MOP_V4CNT_RX_MCAST_FRAMES.h = 0;
    mc->MOP_V4CNT_TX_INIT_DEFERRED.l = 0;
    mc->MOP_V4CNT_TX_INIT_DEFERRED.h = 0;
    mc->MOP_V4CNT_TX_ONE_COLLISION.l = 0;
    mc->MOP_V4CNT_TX_ONE_COLLISION.h = 0;
    mc->MOP_V4CNT_TX_MULTI_COLLISION.l = 0;
    mc->MOP_V4CNT_TX_MULTI_COLLISION.h = 0;
    mc->MOP_V4CNT_TX_FAIL_EXCESS_COLLS.l = 0;
    mc->MOP_V4CNT_TX_FAIL_EXCESS_COLLS.h = 0;
    mc->MOP_V4CNT_TX_FAIL_CARRIER_CHECK.l = 0;
    mc->MOP_V4CNT_TX_FAIL_CARRIER_CHECK.h = 0;
    mc->MOP_V4CNT_TX_FAIL_SHRT_CIRCUIT.l = 0;
    mc->MOP_V4CNT_TX_FAIL_SHRT_CIRCUIT.h = 0;
    mc->MOP_V4CNT_TX_FAIL_OPEN_CIRCUIT.l = 0;
    mc->MOP_V4CNT_TX_FAIL_OPEN_CIRCUIT.h = 0;
    mc->MOP_V4CNT_TX_FAIL_LONG_FRAME.l = 0;
    mc->MOP_V4CNT_TX_FAIL_LONG_FRAME.h = 0;
    mc->MOP_V4CNT_TX_FAIL_REMOTE_DEFER.l = 0;
    mc->MOP_V4CNT_TX_FAIL_REMOTE_DEFER.h = 0;
    mc->MOP_V4CNT_RX_FAIL_BLOCK_CHECK.l = 0;
    mc->MOP_V4CNT_RX_FAIL_BLOCK_CHECK.h = 0;
    mc->MOP_V4CNT_RX_FAIL_FRAMING_ERR.l = 0;
    mc->MOP_V4CNT_RX_FAIL_FRAMING_ERR.h = 0;
    mc->MOP_V4CNT_RX_FAIL_LONG_FRAME.l = 0;
    mc->MOP_V4CNT_RX_FAIL_LONG_FRAME.h = 0;
    mc->MOP_V4CNT_UNKNOWN_DESTINATION.l = 0;
    mc->MOP_V4CNT_UNKNOWN_DESTINATION.h = 0;
    mc->MOP_V4CNT_DATA_OVERRUN.l = 0;
    mc->MOP_V4CNT_DATA_OVERRUN.h = 0;
    mc->MOP_V4CNT_NO_SYSTEM_BUFFER.l = 0;
    mc->MOP_V4CNT_NO_SYSTEM_BUFFER.h = 0;
    mc->MOP_V4CNT_NO_USER_BUFFER.l = 0;
    mc->MOP_V4CNT_NO_USER_BUFFER.h = 0;
    mc->MOP_V4CNT_FAIL_COLLIS_DETECT.l = 0;
    mc->MOP_V4CNT_FAIL_COLLIS_DETECT.h = 0;
}

/*+
 * ============================================================================
 * = print_enet_address - Display an ethernet address.                        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine displays an ethernet address.
 *
 * FORM OF CALL:
 *
 *	print_enet_address (name,enet); 
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	char *name - String to be displayed before the address.
 *	unsigned char *enet - Pointer to the enet address.
 *
 *  SIDE EFFECTS:
 *
 *      None
 *
-*/

void print_enet_address(char *name, unsigned char *enet)
{

/*Display the address*/
    printf("%s %02x-%02x-%02x-%02x-%02x-%02x\n", name, enet[0], enet[1],
      enet[2], enet[3], enet[4], enet[5]);
}

/*+
 * ============================================================================
 * = print_mop_counters - Displays the MOP counters.                          =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine displays the MOP counters.
 *
 * FORM OF CALL:
 *
 *	print_mop_counters (mc); 
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	struct MOP$_V4_COUNTERS *mc - Pointer to the MOP counter block.
 *
 *  SIDE EFFECTS:
 *
 *      None
 *
-*/

void print_mop_counters(struct MOP$_V4_COUNTERS *mc)
{
    UINT tz;				/*Time since zeroed*/

/*Get time since since zeroed*/

    tz = *(UINT *) poweron_msec - *(UINT *) (&(mc->MOP_V4CNT_TIME_RELATIVE.l));

/*Display time since zeroed*/
    printf(msg_ez_inf_mop_cnt);
    printf(msg_ez_inf_t_s_0, tz / MOP$K_1S_BY_1MS);

/*Display the MOP counters*/
    printf(msg_ez_inf_tx);
    printf(msg_ez_inf_b_f, mc->MOP_V4CNT_TX_BYTES.l,
      mc->MOP_V4CNT_TX_FRAMES.l);
    printf(msg_ez_inf_df_oc_mc, mc->MOP_V4CNT_TX_INIT_DEFERRED.l,
      mc->MOP_V4CNT_TX_ONE_COLLISION.l, mc->MOP_V4CNT_TX_MULTI_COLLISION.l);
    printf(msg_ez_inf_tx_f);
    printf(msg_ez_inf_ec_cc_sc, mc->MOP_V4CNT_TX_FAIL_EXCESS_COLLS.l, mc->
      MOP_V4CNT_TX_FAIL_CARRIER_CHECK.l, mc->MOP_V4CNT_TX_FAIL_SHRT_CIRCUIT.l);
    printf(msg_ez_inf_oc_lf_rd, mc->MOP_V4CNT_TX_FAIL_OPEN_CIRCUIT.l, mc->
      MOP_V4CNT_TX_FAIL_LONG_FRAME.l, mc->MOP_V4CNT_TX_FAIL_REMOTE_DEFER.l);
    printf(msg_ez_inf_cd, mc->MOP_V4CNT_FAIL_COLLIS_DETECT.l);
    printf(msg_ez_inf_rx);
    printf(msg_ez_inf_b_f, mc->MOP_V4CNT_RX_BYTES.l,
      mc->MOP_V4CNT_RX_FRAMES.l);
    printf(msg_ez_inf_mb_mf, mc->MOP_V4CNT_RX_MCAST_BYTES.l,
      mc->MOP_V4CNT_RX_MCAST_FRAMES.l);
    printf(msg_ez_inf_rx_f);
    printf(msg_ez_inf_bc_fe, mc->MOP_V4CNT_RX_FAIL_BLOCK_CHECK.l,
      mc->MOP_V4CNT_RX_FAIL_FRAMING_ERR.l, mc->MOP_V4CNT_RX_FAIL_LONG_FRAME.l);
    printf(msg_ez_inf_ud_do_ns, mc->MOP_V4CNT_UNKNOWN_DESTINATION.l,
      mc->MOP_V4CNT_DATA_OVERRUN.l, mc->MOP_V4CNT_NO_SYSTEM_BUFFER.l);
    printf(msg_ez_inf_nub, mc->MOP_V4CNT_NO_USER_BUFFER.l);
}

/*+
 * ============================================================================
 * = print_pkt - Display a packet.                                            =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine displays a packet.
 *
 * FORM OF CALL:
 *
 *	print_pkt (pkt,size); 
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	void *pkt - Pointer to the packet to be printed.
 *	int size - Size of the packet in bytes.
 *
 *  SIDE EFFECTS:
 *
 *      None
 *
-*/

void print_pkt(void *pkt, int size)
{
    int i;
    unsigned char *lptr;

    err_printf(msg_ez_inf_add_sz, pkt, size);
    lptr = pkt;
    for (i = 0; i < size; i++)
	err_printf("%02x %s", lptr[i], (i % 20 == 19 ? "\n" : ""));
    err_printf("\n");
}

/*+
 * ============================================================================
 * = pprint_pkt - Display a packet using pprintf.                             =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine displays a packet using pprintf.
 *
 * FORM OF CALL:
 *
 *	pprint_pkt (pkt,size); 
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	void *pkt - Pointer to the packet to be printed.
 *	int size - Size of the packet in bytes.
 *
 *  SIDE EFFECTS:
 *
 *      None
 *
-*/

void pprint_pkt(void *pkt, int size)
{
    int i;
    unsigned char *lptr;

    pprintf(msg_ez_inf_add_sz, pkt, size);
    lptr = pkt;
    for (i = 0; i < size; i++)
	pprintf("%02x %s", lptr[i], (i % 20 == 19 ? "\n" : ""));
    pprintf("\n");
}

/*+
 * ============================================================================
 * = set_timer - Set timer.                                                   =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will set and start the timer specified.
 *
 * FORM OF CALL:
 *
 *	set_timer (tq,timeout_value); 
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	struct TIMERQ *tq - Pointer to the timer to be set and started.
 *	int timeout_value - Value at which the timer will timeout.
 *
 *  SIDE EFFECTS:
 *
 *      None
 *
-*/

void set_timer(struct TIMERQ *tq, int timeout_value)
{
    char name[20];			/*General holder for a name*/

    trace("set_timer\n", 0);

/*Initialize the timer*/
    sprintf(name, "tqs_%08x", tq);
    krn$_seminit(&(tq->sem), 0, name);
    krn$_init_timer(tq);

/*Set Timer semaphore count, alt flag, and start the timer*/
    tq->sem.count = 0;
    tq->alt = FALSE;
    krn$_start_timer(tq, timeout_value);
}

/*+
 * ============================================================================
 * = wait_msg - Wait for a message or timeout.                                =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will wait for a message or timeout.
 *	msg_success is returned if we got the interrupt, msg_failure
 *	returned if we timed out. The set_timer routine
 *	should be used to set up the timer before this is
 *	routine is called.
 *
 * FORM OF CALL:
 *
 *	 wait_msg (tq); 
 *
 * RETURNS:
 *
 *	int Status - msg_success or FALIURE.
 *
 * ARGUMENTS:
 *
 *	struct TIMERQ *tq - Pointer to the timer to be set and started.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int wait_msg(struct TIMERQ *tq)
{

    trace("wait_msg\n", 0);

/*Wait for the semaphore or timer*/
    krn$_wait(&(tq->sem));
    if (tq->alt) {
	krn$_stop_timer(tq);
	return (msg_success);
    } else {
	krn$_stop_timer(tq);
	return (msg_failure);
    }
}

/*+
 * ============================================================================
 * = convert_enet_address - Converts an enet address string to 6 bytes.       =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will convert an enet from a string to 8 bytes.
 *	The string will be expected to be a valid "C" type string
 *	(Terminated with a null character).
 *
 * FORM OF CALL:
 *
 *	convert_enet_address (enet_addr,enet_addr_string); 
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	unsigned char *enet_addr - Place for the routine to put the
 *					converted address.
 *	char *enet_addr_string - The enet address string.
 *
 *  SIDE EFFECTS:
 *
 *      None
 *
-*/

int convert_enet_address(unsigned char *enet_addr, char *enet_addr_str)
{
    char c, *cp, cs[100];		/*Character and character pointers*/
    unsigned long value;		/*Value of a byte in the address*/
    int csi, ai;			/*Character string index,address
					 * index*/

/*Init some stuff*/

    cp = enet_addr_str;
    c = *cp++;
    csi = 0;
    ai = 0;

/*Look at the string until we reach the null character*/
    while ((c) && (c != '\n')) {

	/*Ignore the white space*/
	if ((c != ' ') && (c != '\t')) {
	    if ((c >= '0') && (c <= '9') || (c >= 'a') && (c <= 'f') ||
	      (c >= 'A') && (c <= 'F')) {

		/*If the char is number put it in the temp string*/
		cs[csi++] = c;
	    } else if (c == '-') {

		/*If we get a "-" store the byte*/
		/*Check for invalid address*/
		if ((csi != 2) || (ai > 4)) {
		    ni_error("", msg_ndl_err_001, enet_addr_str);
		    return (msg_failure);
		}

		/*Load the byte*/
		cs[csi] = 0;
		enet_addr[ai++] = (unsigned char) xtoi(cs);

		/*Reset the index*/
		csi = 0;
	    } else {

		/*If its not any of the previous its an error*/
		ni_error("", msg_ndl_err_001, enet_addr_str);
		return (msg_failure);
	    }
	}

	/*Get the next character*/
	c = *cp++;
    }

/*Get the last byte*/
/*Check for invalid address*/
    if ((csi != 2) || (ai != 5)) {
	ni_error("", msg_ndl_err_001, enet_addr_str);
	return (msg_failure);
    }

/*Load the byte*/
    cs[csi] = 0;
    enet_addr[ai] = (unsigned char) xtoi(cs);

/*Return a success*/
    return (msg_success);
}

/*+
 * ============================================================================
 * = ni_error - Report a network error.                                       =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine is used to report a network error. a maximum of 6
 *	arguments for the err_printf can be specified. If s1 is null
 *      nothing is printed for that string.
 *
 * FORM OF CALL:
 *
 *	ni_error(s1,s2,p0,p1,p2,p3,p4,p5);
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	char *s1 - String that will be displayed in parenthesis to used for
 *		   information like failed port.
 *	char *s2 - Actual error message text.
 *	void p0-p5 - Parameters to be used by err_printf.
 *
 *  SIDE EFFECTS:
 *
 *      None
 *
-*/

void ni_error(char *s1, char *s2, protoargs int p0, int p1, int p2, int p3, int p4, int p5)
{
    if (*s1)
	err_printf(msg_ndl_err_002, s1);
    else
	err_printf(msg_ndl_err_003);
    err_printf(s2, p0, p1, p2, p3, p4, p5);
    err_printf("\n");
}
