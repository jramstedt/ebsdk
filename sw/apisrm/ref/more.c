/* file:	more.c
 *
 * Copyright (C) 1990, 1993 by
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
 *      Cobra/Laser Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *      MORE, a program that delivers an input stream to the output stream
 *	in measured (as in one screenfull) doses.
 *
 *  AUTHORS:
 *
 *      AJ Beaverson
 *
 *  CREATION DATE:
 *  
 *	ajb	31-Jan-1991
 *
 *  MODIFICATION HISTORY:
 *
 *	sfs	19-Feb-1991	Remove concept of "console name".
 *
 *--
 */

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$src:kernel_def.h"
#include "cp$src:parse_def.h"
#include "cp$src:ctype.h"
#include "cp$src:msg_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:tt_def.h"
#include "cp$inc:prototypes.h"

#if TURBO
extern int show_device_flag;
#endif

#define	QN	0	/* indecies into qualifier structures */
#define	QMAX	1
#define QSTRING "%d"

/*+
 * ============================================================================
 * = more - Display output one screen at a time.                              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	More prints the file(s) on the command line (or from stdin)
 *	on the display (or to stdout), a screen at a time (23 lines).
 *	The -n option maybe used to alter the number of lines displayed.
 *	More lines may be displayed, by hitting a key.
 *  
 *   COMMAND FMT: more 0 Z 0 more
 *
 *   COMMAND FORM:
 *  
 *	more ( [-<pagesize> ] [<file>...] )
 *  
 *   COMMAND TAG: more 0 RXBZ more
 *
 *   COMMAND ARGUMENT(S):
 *
 * 	<file>... - Specifies the file(s) to be displayed.  This argument is
 *		    optional.  If no file is specified, input is from stdin.
 *
 *   COMMAND OPTION(S):
 *
 *	-<pagesize> - Specifies the number of lines to print before waiting
 *			for a prompt.  The default is 23.
 *
 *   COMMAND EXAMPLE(S):
 *~
 *      >>>help * | more
 *      >>>more pwrup
 *      >>>more -3 pwrup 
 *      echo "Memory Testing and Configuration Status"
 *      show memory
 *      if (show full_powerup_diags | grep ON >nl) then
 *      --More-- (SPACE - next page, ENTER - next line, Q - quit)
 *~
 *   COMMAND REFERENCES:
 *  
 *      cat, hd
 *
 * FORM OF CALL:
 *  
 *	more (int argc, char *argv [])
 *  
 * RETURN CODES:
 *
 *	msg_success - success
 *	msg_nofile - if fopen failed
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

int more (int argc, char *argv [])
{
	int		saved_page = -1;
	struct FILE	*fin;
	struct FILE	*control;
	struct TTPB	*ttpb = 0;
	char		control_name [128];
	int		status,argix;
	struct PCB 	*pcb, *parent;
	struct QSTRUCT	qual [QMAX];
	int		pagesize;	/* lines on page	*/
	int		i;

	krn$_sleep(1000);		/* allow piped process to start */

	/*
	 * Decode the flags (argc,argv are adjusted and flags culled out).
	 */
	status = qscan (&argc, argv,"-", QSTRING, qual);
	if (status != msg_success) {
	    err_printf (status);
	    return status;
	}

	/*
	 * Default the page size if not defined
	 */
	if (qual [QN].present) {
	    pagesize = qual [QN].value.integer;
	} else {
	    pagesize = 23;
	}

	/* Find a controlling tty */
	if (!krn$_find_controlling_tt (control_name)) {
	    return msg_nofile;
	}
	control = fopen (control_name, "r");
	if (control == 0) return msg_nofile;
	
#if MODULAR
	ttpb = control->ip->misc;
	saved_page = ttpb->page;
	ttpb->page = 0;
#endif

	/*
	 * Read from standard input if no files were specified on the
	 * command line.  To find the controlling tty, we look at our
	 * ancestors.
	 */
	pcb = getpcb();
	if (argc == 1) {
	    more_helper (control, pcb->pcb$a_stdin, pagesize, ttpb);

	/*
	 * Visit each file specification on the command line
	 */
	} else {
	    for (argix=1; argix<argc; argix++) {
		if (killpending ()) break;
		fin = (struct FILE *) fopen (argv [argix], "r");
		if (fin == NULL) {
		    err_printf ("more: can't open %s\n", argv [argix]);
		    continue;
		}
		more_helper (control, fin, pagesize, ttpb);
		fclose (fin); /* release as soon as possible */
	    }
	}
#if MODULAR
	if (saved_page != -1)
	    ttpb->page = saved_page;
#endif

	return msg_success;
}

/*----------*/
void more_helper (struct FILE *control, struct FILE *fp, int pagesize, struct TTPB *ttpb) {
	char	line [256];
	int	n;
	int	c;
	int	tty;

	tty = isatty (control);

#if TURBO
    if (show_device_flag) {		/* only do this for show device */
	if (tty) {
	    ttpb->page = pagesize;	/* have the printf driver do paging */
	    while (1) {
		if (fgets (line, sizeof (line), fp) == NULL)
		    return;		/* piped process completed */
		printf ("%s", line);
		if (killpending ()) {
		    control_c_action (ttpb);	/* kill the piped process */
		}
	    }
	}
    }
#endif

	c = ' ';
	while (1) {
	    switch (c) {
	    case ' ':
		n = pagesize;
		break;
	    case 0x0d:
		n = 1;
		break;
	    case '0':		/* finish up */
		n = 1000;
		break;
	    case 0x10:		/* control-p */
	    case 0x19:		/* control-y */
	    case 0x03:		/* control-c */
	    case 'Q':
	    case 'q':
		if (ttpb)
		    control_c_action (ttpb);
		return;
	    default:
		n = 0;
		break;
	    }
	    while (n-- > 0) {
		if (fgets (line, sizeof (line), fp) == NULL) {
		    return;
		}
		printf ("%s", line);
		if (killpending ()) return;
	    }

	    if (tty) {
		printf ("--More-- (SPACE - next page, ENTER - next line, Q - quit)");
		c = fgetc (control);
		printf ("\033[2K\033[256D");
	    }
	}
}

