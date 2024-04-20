/* File:	sio_def.h
 *
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
 * Abstract:	Sable Std IO Common header definitions file
 *
 * Author:	D.W. Neale
 *
 * Modifications:
 *
 *	dwn	23-Apr-1993	Initial Entry
 */

/* SIO Diagnostic common String and Format array indexes
*/
#define SIO$Str_0	0
#define SIO$Str_1	1
#define SIO$Str_2	2
#define SIO$Str_3	3
#define SIO$Str_4	4
#define SIO$Str_5	5
#define SIO$Str_6	6
#define SIO$Str_7	7
#define SIO$Str_8	8
#define SIO$Str_9	9
#define SIO$Str_10	10
#define SIO$Str_11	11
#define SIO$Str_12	12
#define SIO$Str_13	13
#define SIO$Str_14	14
#define SIO$Str_15	15
#define SIO$Str_16	16
#define SIO$Str_17	17
#define SIO$Str_18	18
#define SIO$Str_19	19
#define SIO$Str_20	20
#define SIO$Str_21	21
#define SIO$Str_22	22
#define SIO$Str_23	23
#define SIO$Str_24	24

#define SIO$Fmt_0	0
#define SIO$Fmt_1	1
#define SIO$Fmt_2	2
#define SIO$Fmt_3	3
#define SIO$Fmt_4	4
#define SIO$Fmt_5	5
#define SIO$Fmt_6	6
#define SIO$Fmt_7	7
#define SIO$Fmt_8	8
#define SIO$Fmt_9	9
#define SIO$Fmt_10	10
#define SIO$Fmt_11	11
#define SIO$Fmt_12	12
#define SIO$Fmt_13	13
#define SIO$Fmt_14	14

 
/* LOCAL SYMBOLS and Macro definitions
*/
#define u_quad unsigned __int64
#define u_long unsigned long
#define u_char unsigned char

#define SIZE$K_B 	1	/* fread(size)	bytes */
#define SIZE$K_LW 	4	/* longwords */
#define SIZE$K_QW	8	/* quadwords */

#define NUM$K_B		1	/* bytes      */
#define NUM$K_LW	4	/* longwords */
#define NUM$K_QW	8	/* quadwords */

#define QSCAN_DONE 0
#define CAN_CREATE 1   /*        create an inode if name not found */
#define NOT_CREATE 0   /* DO NOT create an inode if name not found */
#define FILE_FOUND 1
#define DEBUG2 	   0

#define	malloc(s)	dyn$_malloc(s,DYN$K_SYNC)
#define	realloc(p,s)	dyn$_realloc(p,s,DYN$K_SYNC)
#define free(x)   	dyn$_free(x, DYN$K_SYNC|DYN$K_NOOWN)


/* Locally defined Command Line Qualifier, QSTRUCT array, offsets.
/*	NUM_QUALS is defined by diag_def.h and denotes the number of
/*	default command line qualifiers, ie -t -p -q.  A diagnostic
/*	can optionally define additional command qualifiers via the
/*	below offsets and diag_init() call.
*/
#define QNP	NUM_QUALS 	/* Disable Printing for test data */
#define QDD	NUM_QUALS + 1	/* Display test data to console terminal */
#define QNE	NUM_QUALS + 2	/* NoErrors, Disable local error Printing */
#define QLP	NUM_QUALS + 3 	/* LooP tight scoop loop */
#define QQV	NUM_QUALS + 4	/* Quick Verify */
#define QNQV	NUM_QUALS + 5	/* NOT quick verify, nvram_diag testing */
#define QNC	NUM_QUALS + 6	/* Do NOT ask for confirmation */

#define         MORE_QUALS 7

/* Interesting Note:
/*	The SIO_QUALIF_STRING's MUST be defined in the EXACT order as
/*	their array index counterparts, ie QNP QDD QNE....
/*
/*	If not, then access to the struct QSTRUCT quals[NUM_QUALS + MORE_QUALS];
/*	which qscan loads up from the command line qualifiers,  will be out 
/*	of sequence.
*/
#define SIO_QUALIF_STRING "np dd ne lp qv nqv nc"


/* EXTERNAL DEFINED 
/*
*/
extern struct LOCK spl_kernel;

extern struct ZONE *memzone;


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
    struct pb *pb;
    u_long saved_config[10];	/* saved configuration data */

    u_long failadr;		/* Extended error failing address */
    u_long return_msg;		/* return report_error() status */

    int data_sel;		/* init data switch select */
    int complement;		/* alternates between 1's and 0's */
    int  shift_cnt;		/* total of 64/16 shifting patterns 0 */

    u_long shift_long;		/* shifting data pattern */
    u_long wlong;		/* write data pattern */
    volatile u_long rlong;	/* read  data pattern */
 
    char shift_byte;		/* shifting data pattern */
    unsigned char wbyte;	/* write data pattern */
    volatile unsigned char rbyte;	/* read  data pattern */
    volatile unsigned char tbyte;	/* read  temp data */

    u_quad q_expect;
    u_quad q_rcvd;
    u_quad q_failadr;
 
    /* Optional Global data
    */
    struct FILE	*uut;		/* file      ptr of UnitUnderTest */

    u_long expect;			/* 32 bit Extended error good data */
    u_long rcvd;			/* 32 bit Extended error bad  data */
    unsigned __int64 __int64Expect;	/* 64 bit Extended error good data */
    unsigned __int64 __int64Rcvd;	/* 64 but Extended error bad  data */
    volatile unsigned char   b_rcvd;	/* received byte data pattern */
    volatile unsigned short  w_rcvd;	/* received word data pattern */
    };

extern char * sio_strings[];
extern char * sio_formats[];
