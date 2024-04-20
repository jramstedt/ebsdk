/*
 * file:	diag_def.h
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
 *
 *
 * Abstract:	Common diagnostic header information.
 *
 * Author:	Mark Cacciapouti (mrc)
 *
 * Modifications:
 *
 *	jpo	23-Oct-1993	Add quadword sized scratch
 *
 *	twp	04-Dec-1991	Incrementing max_tests from 50 to 100 (IOP has
 *				somewhere around 65 tests)
 *
 *	twp	07-May-1991	Removed all qualifiers other than -p, -t and -g
 *
 *	twp	07-Mar-1991 	Removed external group references -- all group
 *				addition will be made to diag_groups.h and no
 *				changes are necessary to this file -- now
 *				only contains qualifier defines
 *
 *	mrc	14-Feb-1991	Add section masks and externs, and get rid of
 *				other stuff.
 *
 *	mrc	14-Jan-1991	Initial entry.
 */

/*------------------------------------------------------------------------------
	Qualifier list and total.  Define the new qualifiers here.
------------------------------------------------------------------------------*/

#define MAX_DEVICES 	50	/* Max devices user can enter          */
#define MAX_TESTS      100	/* Max tests user can enter          */
#define	QP		0       /* Pass Count             */
#define	QG		1	/* Group Name (used to be section)  */
#define	QT		2	/* List of Test To Run    */
#define	QSE		3	/* Soft Error Threshold   */
#define	NUM_QUALS	4	/* Number of Options      */
#define QSCAN_INTRO	"-"	/* Precedes Options       */
#define QSCAN_QUALIF_STRING "%dp %sg %st %dse "
#define QUALIF_STRING_LEN (sizeof(QSCAN_QUALIF_STRING))

/*------------------------------------------------------------------------------
  Diagnostic Common Variables.

  Common diagnostic variables should be placed in this structure and the
  structure should define a typedef.  The initialization code can then
  dynamically allocate space for these variables.   This structure will be
  malloc'ed once by DIAG_INIT() and used by all diagnostics for common
  error reporting in diag_support.c

------------------------------------------------------------------------------*/

typedef struct diag_common_struct {
	int		diag$l_pass_count;	
	int 		(*diag$a_ext_status)();
	unsigned int 	diag$l_test_num;
	unsigned int 	diag$l_error_num;
	unsigned int 	diag$l_error_type;
	char 		diag$b_fru [32];
	int 		diag$l_error_msg;
	int 		(*diag$a_ext_error)();                           
	void  		*diag$l_nodespace;	/* Pointer to nodespace     */

	int		diag$l_expect;
	int		diag$l_expect2;	/* rest of 64 bit-ness */
	int		diag$l_rcvd;
	int		diag$l_rcvd2;		/* rest of 64 bit-ness */
	int		diag$l_failadr;
	int		diag$l_failadr2;	/* rest of 64 bit-ness */

	int		diag$l_tlber;	     /* TLBER CSR for TURBO logging */
	int		diag$l_lber;		/* LBER CSR for logging */
	int		diag$l_lmerr;		/* LMERR CSR for logging */
	int 		diag$l_exception_flag;
	int		diag$l_disable_logging; 
	int		diag$l_scratch;	/* Diagnostic temp lw */

	int		diag$l_fru1;		/* Sub FRU ie SIMM on Module */
	int		diag$l_scratch2;	/* Future expansion */
	int		diag$l_scratch3;	/* Future expansion */
	int		diag$l_scratch4;	/* Future expansion */
	int		diag$l_scratch5;	/* Future expansion */
	int		diag$l_scratch6;	/* Future expansion */
	unsigned __int64	diag$l_scratch_qw;  
	unsigned __int64	diag$l_scratch_qw1;	
} diag_common;
