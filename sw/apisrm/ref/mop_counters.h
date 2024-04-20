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
 * Abstract: Mop counter Definitions
 *
 * Author:	John A. DeNisco
 *
 * Modifications:
 *
 *	jad	08-Nov-1990	Remove integer definitions.
 *
 *	jad	6-Sep-1990	Initial entry.
 */
 
/*Time constants*/
#define MOP$K_TIME_VERSION 0x10000000
#define MOP$K_1MS_BY_100NS 10000
#define MOP$K_1S_BY_1MS 1000

/*MAX constants*/
#define MOP$K_MAX_32 0xffffffff
#define MOP$K_MAX_16 0xffff

/*                                                                          */
/* MOP V4 counters block definition.                                        */
/*                                                                          */
struct MOP$_V4_COUNTERS {               		/* Where updated.*/
    U_INT_64 MOP_V4CNT_TIME_RELATIVE;			/* MOP DRIVER	*/
    U_INT_32 MOP_V4CNT_TIME_INACCURACY; 		/* PORT         */
    U_INT_32 MOP_V4CNT_TIME_VERSION;			/* PORT         */
    U_INT_64 MOP_V4CNT_RX_BYTES;			/* PORT		*/
    U_INT_64 MOP_V4CNT_TX_BYTES;			/* PORT		*/
    U_INT_64 MOP_V4CNT_RX_FRAMES;			/* PORT		*/
    U_INT_64 MOP_V4CNT_TX_FRAMES;			/* PORT		*/
    U_INT_64 MOP_V4CNT_RX_MCAST_BYTES;			/* PORT       	*/
    U_INT_64 MOP_V4CNT_RX_MCAST_FRAMES;			/* PORT       	*/
    U_INT_64 MOP_V4CNT_TX_INIT_DEFERRED;		/* PORT       	*/
    U_INT_64 MOP_V4CNT_TX_ONE_COLLISION;		/* PORT       	*/
    U_INT_64 MOP_V4CNT_TX_MULTI_COLLISION;		/* PORT       	*/
    U_INT_64 MOP_V4CNT_TX_FAIL_EXCESS_COLLS;		/* PORT       	*/
    U_INT_64 MOP_V4CNT_TX_FAIL_CARRIER_CHECK;		/* PORT       	*/
    U_INT_64 MOP_V4CNT_TX_FAIL_SHRT_CIRCUIT;		/* PORT         */
    U_INT_64 MOP_V4CNT_TX_FAIL_OPEN_CIRCUIT;		/* PORT 	*/
    U_INT_64 MOP_V4CNT_TX_FAIL_LONG_FRAME;		/* Always zero	*/
    U_INT_64 MOP_V4CNT_TX_FAIL_REMOTE_DEFER;		/* PORT		*/
    U_INT_64 MOP_V4CNT_RX_FAIL_BLOCK_CHECK;		/* PORT       	*/
    U_INT_64 MOP_V4CNT_RX_FAIL_FRAMING_ERR;		/* PORT       	*/
    U_INT_64 MOP_V4CNT_RX_FAIL_LONG_FRAME;		/* PORT       	*/
    U_INT_64 MOP_V4CNT_UNKNOWN_DESTINATION;		/* PORT & MOP DRIVER*/
    U_INT_64 MOP_V4CNT_DATA_OVERRUN;			/* PORT       */
    U_INT_64 MOP_V4CNT_NO_SYSTEM_BUFFER;		/* PORT       */
    U_INT_64 MOP_V4CNT_NO_USER_BUFFER;			/* Always zero..*/
    U_INT_64 MOP_V4CNT_FAIL_COLLIS_DETECT;		/* PORT       */
    } ;

/*                                                                          */
/* MOP V3 counters block definition.                                        */
/*                                                                          */
#define MOP$M_V3BIT_TXFAIL_EXCESS_COLLS 1
#define MOP$M_V3BIT_TXFAIL_CARRIER_CHECK 2
#define MOP$M_V3BIT_TXFAIL_SHRT_CIRCUIT 4
#define MOP$M_V3BIT_TXFAIL_OPEN_CIRCUIT 8
#define MOP$M_V3BIT_TXFAIL_LONG_FRAME 16
#define MOP$M_V3BIT_TXFAIL_REMOTE_DEFER 32
#define MOP$M_V3BIT_RXFAIL_BLOCK_CHECK 1
#define MOP$M_V3BIT_RXFAIL_FRAMING_ERR 2
#define MOP$M_V3BIT_RXFAIL_LONG_FRAME 4

struct MOP$_V3_COUNTERS {
    U_INT_16 MOP_V3CNT_SECONDS_SINCE_ZERO;
    U_INT_32 MOP_V3CNT_RX_BYTES;
    U_INT_32 MOP_V3CNT_TX_BYTES;
    U_INT_32 MOP_V3CNT_RX_FRAMES;
    U_INT_32 MOP_V3CNT_TX_FRAMES;
    U_INT_32 MOP_V3CNT_RX_MCAST_BYTES;
    U_INT_32 MOP_V3CNT_RX_MCAST_FRAMES;
    U_INT_32 MOP_V3CNT_TX_INIT_DEFERRED;
    U_INT_32 MOP_V3CNT_TX_ONE_COLLISION;
    U_INT_32 MOP_V3CNT_TX_MULTI_COLLISION;
    U_INT_16 MOP_V3CNT_TXFAILURES;
    U_INT_16 MOP_V3CNT_TXFAIL_BITMAP;
    U_INT_16 MOP_V3CNT_RXFAILURES;
    U_INT_16 MOP_V3CNT_RXFAIL_BITMAP;
    U_INT_16 MOP_V3CNT_UNKNOWN_DESTINATION;
    U_INT_16 MOP_V3CNT_DATA_OVERRUN;
    U_INT_16 MOP_V3CNT_NO_SYSTEM_BUFFER;
    U_INT_16 MOP_V3CNT_NO_USER_BUFFER;
    } ;
