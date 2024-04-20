/* file:	index.c
 *
 * Copyright (C) 1992 by
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
 *      Alpha Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *      Generate numbers within a given range and write to stdout.
 *
 *  AUTHORS:
 *
 *      AJ Beaverson
 *
 *  CREATION DATE:
 *  
 *      29-Apr-1992
 *
 *  MODIFICATION HISTORY:
 *
 *	ajb	29-Apr-1992	Initial creation.
 *--
 */
#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$inc:prototypes.h"
#include	"cp$src:parse_def.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:kernel_def.h"

#define	QIB	0
#define	QIO	1
#define	QID	2
#define	QIX	3
#define	QB	4   
#define	QO	5
#define	QD	6
#define	QX	7
#define	QMAX	8

/*+
 * ============================================================================
 * = index - Generate a sequence of monotonically increasing numbers.	      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Generate numbers in a range in ascending order.  This command is useful
 *	in shell for loops (see the examples).  The lower and upper bound of
 *	the numbers can be controlled, as well as the increment size.  All
 *	numbers are treated as unsigned integers.  Output radices of binary,
 *	octal, decimal and hex are supported, with decimal the default.
 *
 *	Input numbers are by default decimal.
 *
 *   COMMAND FMT: index_cmd 0 0 0 index
 *      
 *   COMMAND FORM:
 *  
 *	index ( [-{b|d|o|x}] [-{ib,id,io,id}]
 *              [[<increment>] <lower_bound>] <upper_bound> )
 *
 *   COMMAND TAG: index_cmd 0 RXBZ index
 *      
 *   COMMAND ARGUMENT(S):
 *
 * 	<increment> - Specifies the value to increment numbers in the sequence.
 *	<lower_bound> - Specifies the starting number.
 *	<upper_bound> - Specifies the ending number.
 *
 *   COMMAND OPTION(S):
 *
 *	-b	- Specifies to print output in binary.
 *	-d	- Specifies to print output in decimal (default).
 *	-o	- Specifies to print output in octal.
 *	-x	- Specifies to print output in hexadecimal.
 *	-ib	- Specifies to default input radix to binary.
 *	-id	- Specifies to default input radix to decimal (default).
 *	-io	- Specifies to default input radix to octal.
 *	-ix	- Specifies to default input radix to hexadecimal.
 *
 *   COMMAND EXAMPLE(S):
 *~
 *      >>>index 3
 *	0
 *	1
 *	2
 *	3
 *      >>>index 1 2
 *	1
 *	2
 *      >>>index 2 10 12 -x
 *	a
 *	c
 *      >>>
 *~
 *   COMMAND REFERENCES:
 *
 *	None
 *
 * FORM OF CALL:
 *  
 *	index (int argc, char *argv [])     
 *  
 * RETURNS:
 *
 *	0 - success
 *       
 * ARGUMENTS:
 *
 * 	argc	- number of words present on command line
 *	argv	- array of pointers to words on command line
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
index_cmd (int argc, char *argv []) {
	UINT	lower;
	UINT	upper;
	UINT	increment;
	UINT	x;
	int	status;
	struct QSTRUCT qual [QMAX];
	char	*radix_out;
	int	defradix;

	/*
	 * Check for radix qualifiers and apply decimal as the default if none
	 * are specified.
	 */                                    
	status = qscan (&argc, argv, "-", "ib io id ix b o d x", qual);
	if (status != msg_success) {
	    err_printf (status);
	    return status;
	}

	/* default the input radix to decimal */
	defradix = 10;
	if (qual [QIB].present) defradix = 2;
	if (qual [QIO].present) defradix = 8;
	if (qual [QID].present) defradix = 10;
	if (qual [QIX].present) defradix = 16;

	increment = 1;
	lower = 0;
	status = 0;
	switch (argc) {
	case 4:
	    status |= common_convert (argv [3], defradix, &upper, sizeof (upper));
	    status |= common_convert (argv [2], defradix, &lower, sizeof (lower));
	    status |= common_convert (argv [1], defradix, &increment, sizeof (increment));
	    break;
	case 3:
	    status |= common_convert (argv [2], defradix, &upper, sizeof (upper));
	    status |= common_convert (argv [1], defradix, &lower, sizeof (lower));
	    if (lower > upper) {
		increment =  -increment;
	    }	    
	    break;
	case 2:
	    status |= common_convert (argv [1], defradix, &upper, sizeof (upper));
	    break;
	default:
	    return msg_success;
	}
	if (status) {
	    return msg_failure;
	}

	radix_out = "%d\n";
	if (qual [QB].present) radix_out = "%b\n";
	if (qual [QD].present) radix_out = "%d\n";
	if (qual [QO].present) radix_out = "%o\n";
	if (qual [QX].present) radix_out = "%x\n";

	while (lower != upper) {
	    printf (radix_out, (UINT) lower);
	    lower += increment;
	    if (killpending())break;
	}
	return msg_success;
}
