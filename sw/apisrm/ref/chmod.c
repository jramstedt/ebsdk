/* file:	chmod.c
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
 *	chmod (Change Mode) command
 *
 *  AUTHORS:
 *
 *      AJ Beaverson
 *
 *  CREATION DATE:
 *  
 *      30-Jan-1992
 *
 *  MODIFICATION HISTORY:
 *
 *	ajb	30-Jan-1992	Broken out from shell_commands
 *--
 */
#include "cp$src:platform.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:common.h"
#include "cp$inc:prototypes.h"
#include	"cp$src:parse_def.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:dynamic_def.h"

/*+
 * ============================================================================
 * = chmod - Change the mode of a file.                                       =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      Changes the specified attributes of a file. The chmod command is a 
 *	subset of the equivalent U*x command. 
 *
 *	You alter a file's attributes by entering an operation on the 
 *	command  line: A minus sign (-) indicates to remove the attribute,
 *	a plus sign (+) indicates to add the attribute, and an 
 *	equals sign (=) indicates to set the attribute absolutely
 *	(clear all other attributes not in the list).
 *	You can only enter one operation per command line. 
 *
 *	To specify an option, you must place the option as the first  
 *	argument in a command line.  
 *  
 *   COMMAND FMT: chmod 3 Z 0 chmod
 *
 *   COMMAND FORM:
 *  
 *	chmod ( [{-|+|=}{r,w,x,b,z}] [<file>...] )
 *  
 *   COMMAND TAG: chmod 0 RXBZ chmod
 *
 *   COMMAND ARGUMENT(S):
 *
 *	<file>... - Specifies the file(s) or inode(s) to be modified.
 *
 *   COMMAND OPTION(S):
 *
 *	r - Set or clear the read    attribute.
 *	w - Set or clear the write   attribute.
 *	x - Set or clear the execute attribute.
 *	b - Set or clear the binary  attribute.
 *	z - Set or clear the expand  attribute.
 *
 *   COMMAND EXAMPLE(S):
 *~
 *	>>> chmod +x script	! makes file script executable
 *	>>> chmod =r errlog	! sets error log to read only
 *	>>> chmod -w dk*	! makes all scsi disks non writeable
 *~
 *   COMMAND REFERENCES:
 *
 *	chown, ls -l
 *
 * FORM OF CALL:
 *  
 *	chmod (argc, *argv[] )
 *  
 * RETURN CODES:
 *
 *	msg_success - Success
 *	msg_nofile - file(s) not found
 *       
 * ARGUMENTS:
 *
 *	int argc - number of command line arguments passed by the shell
 *	char *argv [] - array of pointers to arguments
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int chmod (int argc, char *argv[]) {
	int	argix;
	char	*file;
	char	*info;
	char	*next;
	int	status;
	struct QSTRUCT	qual [5];
	struct INODE *ip;
	int	attr_and;
	int	attr_or;
	enum {QR, QW, QX, QB, QZ} qval;
	int	action;
	int	mask;
#define	ATTR_ALL \
	(ATTR$M_READ \
	| ATTR$M_WRITE \
	| ATTR$M_EXECUTE \
	| ATTR$M_BINARY \
	| ATTR$M_EXPAND)

	/*
	 * Decode the qualifiers.  Only one of -, + or = may be
	 * present.  We have to determine which one is used before
	 * we call qscan because qscan will remove the qualifier(s)
	 * from the argument list.
	 */
	action = 3;
	if (argc >= 1) {
	    switch (argv [1] [0]) {
	    case '-':
		action = 0;
		break;
	    case '+':
		action = 1;
		break;
	    case '=':
		action = 2;
		break;
	    }
	}
	status = qscan (&argc, argv, "-+=", "r w x b z", qual);
	if (status != msg_success) {
	    err_printf (status);
	    return status;
	}

	mask = 0;
	if (qual [QR].present) mask |= ATTR$M_READ;
	if (qual [QW].present) mask |= ATTR$M_WRITE;
	if (qual [QX].present) mask |= ATTR$M_EXECUTE;
	if (qual [QB].present) mask |= ATTR$M_BINARY;
	if (qual [QZ].present) mask |= ATTR$M_EXPAND;

	/*
	 * Construct an AND and an OR bitmask that are applied to the
	 * inodes.  This allows us to traverse the inode list as fast
	 * as possible.
	 */
	switch (action) {
	case 0: /* subtract */
	    attr_and = ~mask;
	    attr_or = 0;
	    break;
	case 1: /* add */
	    attr_and = ATTR_ALL;
	    attr_or = mask;
	    break;
	case 2: /* set */
	    attr_and = 0;
	    attr_or = mask;
	    break;
	case 3: /* no action */
	    return (int ) msg_success;
	}

	/*
	 * Walk down the list of file names
	 */
	for (argix=1; argix<argc; argix++) {
	    file = malloc (strlen (argv [argix]) + 2);
	    status = validate_filename (argv [argix], file, &info, &next);
	    if (status == msg_success) {
		if (info || next) {
		    status = msg_bad_filename;
		} else {
		    switch (allocinode (argv [argix], 1, &ip)) {
			case 0:
			    status = msg_nofile;
			    break;
			case 1:
			    ip->attr = (ip->attr & attr_and) | attr_or;
			    INODE_UNLOCK (ip);
			    break;
			case 2:
			    ip->inuse = 0;
			    INODE_UNLOCK (ip);
			    status = msg_nofile;
			    break;
			case 3:
			    status = msg_ambig_file;
			    break;
		    }
		}
	    }
	    free (file);
	    if (status != msg_success)
		err_printf (status);
	}

	return msg_success;
}
