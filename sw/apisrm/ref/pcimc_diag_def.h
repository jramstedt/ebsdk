/* File:	pcimc_diag_def.h
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
 * and  should  not  be  construed  as a commitment by Digital Equipment
 * Corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by Digital.
 *
 *
 * Abstract:	Pci-Reflective Memory Diagnostic header file
 *
 * Author:	D.W. Neale
 *
 * Modifications:
 *
 *	dwn	09-May-1994	Initial Entry
 */

/* LOCAL SYMBOLS and Macro definitions
*/
#define u_quad  unsigned __int64
#define u_long  unsigned int
#define u_char  unsigned char
#define u_short unsigned short

#define QSCAN_DONE 0

/*  diag_def.h defines the following default command line qualifiers
/*
/*	MAX_DEVICES 	50	/* Max devices user can enter
/*	MAX_TESTS      100	/* Max tests user can enter  
/*	QP		0       /* Pass Count
/*	QG		1	/* Group Name (used to be section)
/*	QT		2	/* List of Test To Run  
/*	QSE		3	/* Soft Error Threshold 
/*	NUM_QUALS	4	/* Number of Options    
/*	QSCAN_INTRO	"-"	/* Precedes Options     
/*	QSCAN_QUALIF_STRING "%dp %sg %st %dse "
/*	QUALIF_STRING_LEN (sizeof(QSCAN_QUALIF_STRING))
/*	
/* Pci-MC will define these additional Command Line Qualifier, 
/* QSTRUCT array, offsets.
/*	NUM_QUALS is defined by diag_def.h and denotes the number of
/*	default command line qualifiers, ie -t -p -q.  A diagnostic
/*	can optionally define additional command qualifiers via the
/*	below offsets and diag_init() call.
*/
#define QNP	NUM_QUALS + 0	/* Disable Printing for test data */
#define QDD	NUM_QUALS + 1	/* Display test data to console terminal */
#define QNE	NUM_QUALS + 2	/* NoErrors, Disable local error Printing */
#define QLP	NUM_QUALS + 3 	/* LooP tight scoop loop */
#define QQV	NUM_QUALS + 4	/* Quick Verify */
#define QNQV	NUM_QUALS + 5	/* NOT quick verify, nvram_diag testing */
#define QNC	NUM_QUALS + 6	/* Do NOT ask for confirmation */
#define QNID	NUM_QUALS + 7	/* User selected MC-Node id to test */
#define QMCPA   NUM_QUALS + 8	/* User selected MC-Page address to test */
#define QBCST	NUM_QUALS + 9	/* User selected MC-Bcst to test */
#define QNRAND	NUM_QUALS + 10	/* Do not use random data */

#define         MORE_QUALS 11

/* Interesting Note:
/*	The PCIMC_QUALIF_STRING's MUST be defined in the EXACT order as
/*	their array index counterparts, ie QNP QDD QNE....
/*
/*	If not, then access to the struct QSTRUCT quals[NUM_QUALS + MORE_QUALS];
/*	which qscan loads up from the command line qualifiers,  will be out 
/*	of sequence.
/*
/* 	Supported radix types, see qscan.c:
/*		%x %d %s 
*/
#define PCIMC_QUALIF_STRING  "np dd ne lp qv nqv nc %dnid %xmcpa bcst nrand"

/* EXTERNAL DEFINED 
/*
*/

#define PCIMCVERIFY 0
#define PCIMCINPROG 0
#define PCIMCIDLE   1
#define PCIMCPASS   2
#define PCIMCPOLLTIMEOUT   3
#define PCIMCDATATIMEOUT   4
#define PCIMCDATAERROR     5
#define PCIMCDATARESEND    6

/* Used by Pci-MC Diag's and Exercisers, when multiple Spur module 
/* reside in the same System and same Pci Bus.
/*
/* A Pci-MC Node, can either be a Ping or a Pong, depending on how the
/* Ping/Pong Exerciser is executed at the console command line.  Each
/* node will support ONE each, ping and pong processes.
/*
*/
struct MCDEVLIST {
    unsigned __int64
    	PingBytesWrit,
    	PingBytesRead,
    	PongBytesWrit,
    	PongBytesRead;

    unsigned int
    	PingPassFail,		/* 0=Idle, 1=TstInProgress, 2=Pass, 3=Fail */
    	PongPassFail;		/* 0=Idle, 1=TstInProgress, 2=Pass, 3=Fail */

    unsigned int
    	PingPid,		/* Process ID, ie >>> kill pid */
    	PongPid,		

    	PingWaits,		
    	PongRetry,		

    	PingXferSize,		
    	PongXferSize,		

    	PingSizeErr[9],		/* 1, 2, 4, 8, 16, 32, 64, 128, 256 */
    	PongSizeErr[9],		/* 1, 2, 4, 8, 16, 32, 64, 128, 256 */

    	PingErrCnt,		
    	PongErrCnt,		

    	PingIntrCnt,		
    	PongIntrCnt,		
    	TotIntrCnt,		
	stachgintrcnt,

	pingintrmissed,
	pongintrmissed,

    	PingNid,		
    	PongNid,
    	PingPageBase,
    	PongPageBase,
    	PingPgBaseAdr,
    	PongPgBaseAdr;

    char *DevName;
    };


/*
/*  GLOBAL VARIABLE STRUCTURE
/*	To conserve Flash Rom space, all program global 
/*	variable storage is defined within this structure
/*	and malloc'ed at run time.
*/
struct GLOBALS_STRUCT {

    /* Required Global data for ALL Sable Std IO Diag's
    */
    struct QSTRUCT qual[NUM_QUALS + MORE_QUALS];

    unsigned int
    	expect,			/* 32 bit Extended error good data */
    	rcvd,			/* 32 bit Extended error bad  data */
    	FailPage,		/* Extended error failing MC-Page*/
    	FailPageOff,		/* Extended error failing MC-Page Offset */
    	FailAdrs,		/* Extended error failing Non-MC errors */
    	error_msg,		/* Load/ClearErrorField() return status */
    	return_msg;		/* report_error() return status */


    /* Optional Global data
    */
    unsigned int
    	PtdrEnMask,
    	MCMalloc,
    	MCNumPages,
    	MCRcvSize,	
    	MCTstPage,	
    	mcpingid,
    	mcpongid,
    	mcpingpg,
    	mcpongpg,
    	mcpingpgadr,
    	mcpongpgadr,
    	random;


    struct FILE	    *SpurAfp;	/* MC-Exer file pointer */
    struct FILE	    *SpurBfp;	/* MC-Exer file pointer */
    struct INODE    *SpurAip;	/* MC-Exer inode pointer */
    struct INODE    *SpurBip;	/* MC-Exer inode pointer */

    struct FILE	    *fp;	/* file  pointer to mca0 */
    struct INODE    *ip;	/* inode pointer to mca0 */
    struct PCIMC_PB *mcpb;	/* port block pointer to mca0 */
    struct PCIMCQ   *pctintr;	/* pointer to the queue entry semaphore */

    int   data_sel;		/* init data switch select */
    int   complement;		/* alternates between 1's and 0's */
    int   shift_cnt;		/* total of 64/16 shifting patterns 0 */
    unsigned int   shift_long;	/* shifting data pattern */
    unsigned int   wlong;	/* write data pattern */
    unsigned short shift_short;	/* shifting data pattern */
    unsigned short wshort;	/* write data pattern */
    unsigned char  shift_byte;	/* shifting data pattern */
    unsigned char  wbyte;	/* write data pattern */

    };

