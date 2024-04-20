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
 * Abstract: Mop counter Definitions taking advantage of __int64
 *
 * Author:	John A. DeNisco
 *
 * Modifications:
 *
 *	jad	15-Sep-1990	Initial entry.
 */
 
/*                                                                          */
/* MOP V4 counters block definition.                                        */
/*                                                                          */
struct MOP$_V4_COUNTERS {               		/* Where updated.*/
    uint64 MOP_V4CNT_TIME_RELATIVE;			/* MOP DRIVER	*/
    uint32 MOP_V4CNT_TIME_INACCURACY; 			/* PORT         */
    uint32 MOP_V4CNT_TIME_VERSION;			/* PORT         */
    uint64 MOP_V4CNT_RX_BYTES;				/* PORT		*/
    uint64 MOP_V4CNT_TX_BYTES;				/* PORT		*/
    uint64 MOP_V4CNT_RX_FRAMES;				/* PORT		*/
    uint64 MOP_V4CNT_TX_FRAMES;				/* PORT		*/
    uint64 MOP_V4CNT_RX_MCAST_BYTES;			/* PORT       	*/
    uint64 MOP_V4CNT_RX_MCAST_FRAMES;			/* PORT       	*/
    uint64 MOP_V4CNT_TX_INIT_DEFERRED;			/* PORT       	*/
    uint64 MOP_V4CNT_TX_ONE_COLLISION;			/* PORT       	*/
    uint64 MOP_V4CNT_TX_MULTI_COLLISION;		/* PORT       	*/
    uint64 MOP_V4CNT_TX_FAIL_EXCESS_COLLS;		/* PORT       	*/
    uint64 MOP_V4CNT_TX_FAIL_CARRIER_CHECK;		/* PORT       	*/
    uint64 MOP_V4CNT_TX_FAIL_SHRT_CIRCUIT;		/* PORT         */
    uint64 MOP_V4CNT_TX_FAIL_OPEN_CIRCUIT;		/* PORT 	*/
    uint64 MOP_V4CNT_TX_FAIL_LONG_FRAME;		/* Always zero	*/
    uint64 MOP_V4CNT_TX_FAIL_REMOTE_DEFER;		/* PORT		*/
    uint64 MOP_V4CNT_RX_FAIL_BLOCK_CHECK;		/* PORT       	*/
    uint64 MOP_V4CNT_RX_FAIL_FRAMING_ERR;		/* PORT       	*/
    uint64 MOP_V4CNT_RX_FAIL_LONG_FRAME;		/* PORT       	*/
    uint64 MOP_V4CNT_UNKNOWN_DESTINATION;		/* PORT & MOP DRIVER*/
    uint64 MOP_V4CNT_DATA_OVERRUN;			/* PORT       */
    uint64 MOP_V4CNT_NO_SYSTEM_BUFFER;			/* PORT       */
    uint64 MOP_V4CNT_NO_USER_BUFFER;			/* Always zero..*/
    uint64 MOP_V4CNT_FAIL_COLLIS_DETECT;		/* PORT       */
    } ;
