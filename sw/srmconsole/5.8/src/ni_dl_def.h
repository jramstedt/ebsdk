/*
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
 * and  should  not  be  construed  as a commitment by Digital Equipment
 * Corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by Digital.
 *
 *
 * Abstract:	Datalink Definitions
 *
 * Author:	John A. DeNisco
 *
 * Modifications:
 *
 *	pel	06-Jul-1993	remove PORT_CALLBACK stop routine pointer
 *
 *	pel	23-Jun-1993	Add ptr to stop port routine to PORT_CALLBACK
 *
 *	jad	02-Jun-1992	Fix unaligned queue.
 *
 *	jds	16-Aug-1991	Made driver flags conditional.
 *
 *	jad	10-AUG-1990	Initial entry.
 */
 

/*Program definitions*/
/*Constants*/
#define NDL$K_DEV_NAME_MAX 4		/* MAX device name length	*/
#define NDL$K_NO_VERSION 0              /* Pass the message directly    */
#define NDL$K_VERSION_3 1               /* All MOP versions before 4.   */
#define NDL$K_VERSION_4 4               /* MOP version 4.               */
#define NDL$K_DONT_INC_LENGTH 0		/* Include the length flags.    */
#define NDL$K_INC_LENGTH 1
#define NDL$K_VERSION_4 4               /* MOP version 4.               */
#define NDL$K_PKT_MOD 0x200		/* Packet default modulus	*/

/*Mode constants*/
#define NDL$K_NORMAL_OM 0		/*Normal mode*/
#define NDL$K_INT 1 			/*Internal loopback*/
#define NDL$K_EXT 2 			/*External loopback*/
#define NDL$K_NORMAL_FILTER 3		/*Normal filter*/
#define NDL$K_PROM 4 			/*Promiscious*/
#define NDL$K_MULTICAST 5		/*All multicast frames passed*/
#define NDL$K_INT_PROM 6 		/*Internal loopback and promiscious*/
#define NDL$K_INT_FC 7 			/*Internal loopback and force coll*/
#define NDL$K_NO_FC 8 			/*No force collision*/
#define NDL$K_DEFAULT 9			/*Default*/
#define NDL$K_TP 10			/*Twisted pair*/
#define NDL$K_AUI 11			/*AUI*/
#define NDL$K_BNC 12			/*BNC*/
#define NDL$K_TP_FD 13			/*Full Duplex, Twisted pair*/

/*Driver flags*/
#define	NDL$M_BROADCAST 1		/*Broadcast*/
#define	NDL$M_ENA_BROADCAST 1		/*Enable broadcast*/
#define	NDL$M_DIS_BROADCAST 0		/*Disable broadcast*/
#define	NDL$M_HASH 2			/*Hash filter*/
#define	NDL$M_ENA_HASH 2		/*Use hash filter*/
#define	NDL$M_DIS_HASH 0		/*Don't use hash filter*/
#define	NDL$M_INVF 4			/*Inverse filter*/
#define	NDL$M_ENA_INVF 4		/*Use inverse filter*/
#define	NDL$M_DIS_INVF 0		/*Don't use inverse filter*/
#define	NDL$M_MEMZONE 8			/*Use memzone for buffers*/
#define	NDL$M_NOMEMZONE 0		/*Don't use memzone for buffers*/

/*Driver flags default*/
#define NDL$K_DRIVER_FLAGS_DEF 0

/*Data link data structures*/
struct NI_RCV_E {
    struct NI_RCV_E *flink;             /* Pointer to next NI_RCV_E         */
    struct NI_RCV_E *blink;             /* Pointer to prev NI_RCV_E         */
    U_INT_16 prot;			/* Protocol type		    */
    U_INT_8 mca[6];			/* Multicast address enabled	    */
    void (*rp)(struct FILE *fp,	        /* Pointer to Receive routine       */
		U_INT_8 *msg,
		U_INT_8 *source,
		U_INT_16 size,
		int version);
    };

/*Callback structure*/
struct PORT_CALLBACK {
    void (*st)(struct NI_GBL *np);        /* Pointer to status or Control T*/
					  /* routine*/
    int (*cm)(struct FILE *fp,int mode);  /* Pointer to port change mode*/
					  /* routine*/
    int (*ri)(struct FILE *fp);		  /* Pointer to port reinit*/
					  /* routine*/
    void (*re)(struct NI_ENV *ev,char *name); /* Pointer to read env routine*/
    void (*dc)(struct NI_GBL *np); 	  /* Pointer to dump csrs routine*/
    };

volatile struct NI_DL {
	struct NI_RCV_E nrh;		/*Head of the receive Q*/
 	struct PORT_CALLBACK *cb;	/*Callback pointers*/
    	U_INT_8 sa[6];	       		/*Our station address*/
    };


/*Datalink constants*/

/* IEEE 802 SNAP SAP with Protocol ID (AA-AA-03-08-00-2b-xx-xx)             */

#define NDL$K_IEEE_SNAP_SAP 43690       /* AA-AA SNAP SAP.                  */
#define NDL$K_IEEE_SNAP 170             /* AA SNAP DSAP (or SSAP).          */
#define NDL$K_IEEE_UI_PID 721422339     /* 03-08-00-2B UI and 3 bytes of prefix. */
#define NDL$K_IEEE_CTL_XID 175          /* 101x1111 P/F bit is don't care.  */
#define NDL$K_IEEE_CTL_TEST 227         /* 111x0011 P/F bit is don't care.  */
#define NDL$M_IEEE_P_F_BIT 16           /* 00010000 P/F bit.                */
#define NDL$M_IEEE_I_G_BIT 1		/* 00000001 I/G bit.                */
#define NDL$M_IEEE_C_R_BIT 1		/* 00000001 C/R bit.                */

/* Ethernet and IEEE 808.3 lengths.                                         */

#define NDL$K_MAX_DATA_LENGTH 1500      /* Data without CRC.                */
#define NDL$K_MAX_PACK_LENGTH 1518      /* Header, data, and CRC, no preamble. */
#define NDL$K_PACK_ALLOC 1536		/* Header, data, and CRC, no preamble. */
#define NDL$K_MIN_PKT_LENGTH 60
#define NDL$K_MIN_DATA_LENGTH 3
#define NDL$K_IEEE_HEADER_LEN 14


/*Datalink structures*/

/* GNERIC Logical Link Control header.                                      */

struct NDL$GEN_HEADER {
    U_INT_8 NDL$B_GEN_DEST [6];   	/*Destination                       */
    U_INT_8 NDL$B_GEN_SOURCE [6]; 	/*Source                            */
    } ;

/* ENET Logical Link Control header.                                        */

struct NDL$ENET_HEADER {
    U_INT_8 NDL$B_ENET_DEST [6];  	/*Destination                       */
    U_INT_8 NDL$B_ENET_SOURCE [6]; 	/*Source                            */
    U_INT_8 NDL$B_ENET_PROT [2];  	/*Protocol                          */
    U_INT_16 NDL$B_ENET_LEN;	        /*length                            */
    } ;

/* IEEE Logical Link Control header.                                        */
#define NDL$K_IEEE_H_SIZE 8

/* IEEE Logical Link Control header.                                        */

struct NDL$IEEE_HEADER {
    U_INT_8 NDL$B_IEEE_DEST [6];  	/*Destination                       */
    U_INT_8 NDL$B_IEEE_SOURCE [6]; 	/*Source                            */
    U_INT_8 NDL$B_IEEE_LEN [2];   	/*length                            */
    U_INT_8 NDL$B_IEEE_DSAP;      	/*Destination service access point  */
    U_INT_8 NDL$B_IEEE_SSAP;      	/*Source service access point       */
    U_INT_8 NDL$B_IEEE_PROT_UI;   	/*Unumbered information             */
    U_INT_8 NDL$B_IEEE_PROT_PREFIX [3]; /*Protocol identifier	            */
    U_INT_8 NDL$B_IEEE_PROT [2];  	/* 	""                          */
    } ;

/* XID (IEEE 802.2 format) message.                                         */

#define NDL$K_XID_FORMAT 129            /* IEEE Basic format.               */
#define NDL$K_XID_CLASS 1               /* Class/Type 1 service.            */
#define NDL$K_XID_WINDOW 0              /* Window size.                     */

struct NDL$IEEE_XID {
    U_INT_8 NDL$B_XID_FORMAT;
    U_INT_8 NDL$B_XID_CLASS;
    U_INT_8 NDL$B_XID_WINDOW;
    U_INT_8 NDL$B_XID_SIZE;
    } ;

/* TEST (IEEE 802.2 format) message.                                        */

struct NDL$IEEE_TEST {
    U_INT_8 NDL$B_TST_DATA;
    } ;

