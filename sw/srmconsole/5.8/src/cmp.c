/* file:	cmp.c
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
 *      Alpha Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *	Shell cmp command
 *
 *  AUTHORS:
 *
 *      AJ Beaverson
 *
 *  CREATION DATE:
 *  
 *      11-Feb-1991
 *
 *  MODIFICATION HISTORY:
 *
 *--
 */
#include "cp$src:platform.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:parse_def.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:stddef.h"
#include	"cp$src:common.h"
#include "cp$inc:prototypes.h"
#include	"cp$src:msg_def.h"

/*+
 * ============================================================================
 * = cmp - Compare two files.                                                 = 
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This command is only built into SBLOAD, SLLOAD, GMLOAD, and GLLOAD.
 *  
 *	Compares the contents of two files that you specify and reports 
 *	offsets where the files are different.
 *
 *	The cmp command reads the files into internal buffers in blocks of  
 *	1024 bytes (the block size can be changed with the -b qualifier) and
 *	compares the files on a byte by byte basis.  If the files are equal, 
 *	or if the -s qualifier is present, cmp does not issue a report but
 *	simply sets the return status code.
 *
 *	One of the files may be '-', in which case stdin is used.
 *
 *	You can compare portions of files by using the -n qualifier and the
 *	skipcounts for each file. Also using the -n qualifier, you can 
 *	specify how many characters, in decimal, are to	be compared.
 *
 *	If you specify skip parameters, then the appropriate number of
 *	characters are ignored from the input stream.  If one of the files
 *	is a pipe,  then the pipe is drained for skipcount characters by
 *	reading and discarding the data.  Otherwise, the characters are 
 *	skipped by an fseek call.  You specify skipcounts in decimal.
 *
 *	If you specify skipcounts, offsets are reported relative to 
 *	the skipcount.
 *	
 *   COMMAND FMT: cmp 0 Z 0 cmp
 *
 *   COMMAND FORM:
 *  
 *	cmp ( [-n <bytes>] [-b <size>] [-s] <file1>  <file2> [<skip1>] [<skip2>] )
 *  
 *   COMMAND TAG: cmp 0 RXBZ cmp
 *
 *   COMMAND ARGUMENT(S):
 *
 * 	<file1> - Specifies the first file to be compared.
 * 	<file2> - Specifies the second file to be compared.
 *	<skip1> - Specifies the number of characters to skip in file1.
 *	<skip2> - Specifies the number of characters to skip in file2.
 *
 *   COMMAND OPTION(S):
 *
 *	-n <bytes> - Specifies the number of bytes to compare.
 *	-b <size>  - Specifies the user defined buffer size.
 *	-s	   - Set status code only (don't report differences)
 *
 *   COMMAND EXAMPLE(S):
 *~
 *  >>>
 *  >>>echo abc > foo
 *  >>>echo abcd > bar
 *  >>>echo zzabc > foobar
 *  >>>cmp foo bar
 *  cmp: files not equal length
 *  >>>cmp -n 3 foo bar
 *  >>>cmp foo foo
 *  >>>cmp foo foobar
 *  cmp: files not equal length
 *  >>>cmp foo foobar 0 2
 *  >>>
 *  >>>cmp bar foobar -n 4
 *  cmp: files not equal at offset 00000000 61 7A
 *  cmp: files not equal at offset 00000001 62 7A
 *  cmp: files not equal at offset 00000002 63 61
 *  cmp: files not equal at offset 00000003 64 62
 *  >>>
 *~
 *   COMMAND REFERENCES:
 *
 *	cat, echo
 *
 * FORM OF CALL:
 *  
 *	cmp( argc, *argv[] )
 *  
 * RETURN CODES:
 *
 *	msg_success - Success
 *       
 * ARGUMENTS:
 *
 *	int argc - number of command line arguments passed by the shell
 *	char *argv [] - array of pointers to arguments
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
#define	QB	0	/* user defined buffer size	*/
#define	QN	1	/* length			*/
#define QS	2	/* set status code only		*/
#define	QMAX	3
#define QSTRING "%db %dn s"

int cmp (int argc, char *argv []) {
	struct PCB	*pcb;
	int		buf_size;
	int		read_size;
	struct QSTRUCT	qual [QMAX];
	int		status;
	unsigned int	offset, length;
	int		i;

	struct FD {
	    struct file *fp;
	    char *bp;
	    int skip;
	    int len;
	} fd [2];

	memset (&fd, 0, sizeof (fd));

	/*
	 * Pick off qualifiers
	 */
	status = qscan (&argc, argv, "-", QSTRING, qual);
	if (status != msg_success) {
	    err_printf ("cmp: %m", status);
	    return status;
	}

	/* optional length */
	length = -1;
	if (qual [QN].present) {
	    length = qual [QN].value.integer;
	}

	/*
	 * Insure enough arguments
	 */
	if (argc < 3) {
	    err_printf (msg_insuff_params);
	    return msg_insuff_params;
	}

	/*
	 * Read in skipcounts
	 */
	if (argc >= 4) {
	    fd [0].skip = atoi (argv [3]);
	}
	if (argc >= 5) {
	    fd [1].skip = atoi (argv [4]);
	}
 
	/*
	 * Set up buffers
	 */
	buf_size = 1024;
	if (qual [QB].present) {
	    buf_size = qual [QB].value.integer;
	}
	for (i=0; i<2; i++) {
	    fd [i].bp = malloc (buf_size);
	}

	/*
	 * Open up files
	 */
	pcb = getpcb();
	for (i=1; i<=2; i++) {
	    if (strcmp (argv [i], "-") == 0) {
		fd [i-1].fp = pcb->pcb$a_stdin;
	    } else {
		fd [i-1].fp = fopen (argv [i], "r");
		if (fd [i-1].fp == NULL) {
		    return msg_nofile;
		}
	    }
	}

	status = msg_success;

	/*
	 * Apply the skipcounts to each file. Only seek/read if a non zero
	 * skip count was specified.
	 */
	for (i=0; i<2; i++) {
	    if (fd [i].skip) {
		if (isapipe (fd [i].fp)) {
		    int x, len;
		    x = fd [i].skip;
		    while (x) {
			read_size = min (buf_size, x);
			len = fread (fd [i].bp, 1, read_size, fd [i].fp);
			if (len == 0) return msg_cmp_seek;
			x -= len;
		    }
		} else {
		    if (status = fseek (fd [i].fp, fd [i].skip, SEEK_SET)) {
			return msg_cmp_seek;
		    }
		}
	    }
	}

	/*
	 * Iterate over the files
	 */
	offset = 0;
	while ((status == msg_success) && (length != 0)) {
	    if (killpending ()) {
		break;		
	    }

	    read_size = umin (buf_size, length);
	    for (i=0; i<2; i++) {
		fd [i].len = fread (fd [i].bp, 1, read_size, fd [i].fp);
	    }

	    /* simultaneous eof */
	    if ((fd [0].len == NULL) && (fd [1].len == NULL)) break;

	    /* lengths not equal */
	    if (fd [0].len != fd [1].len) {
		printf (msg_cmp_length);
		status = msg_cmp_length;
		break;
	    }

	    if (memcmp (fd [0].bp, fd [1].bp, fd [0].len)) {
		status = msg_cmp_neq;
		if (!qual [QS].present) {
		    for (i=0; i<fd [0].len; i++) {
			if (killpending ()) break;
			if (fd [0].bp [i] != fd [1].bp [i]) {
			    printf (msg_cmp_neq,
				offset + i,
				fd [0].bp [i] & 0xff,
				fd [1].bp [i] & 0xff
			    );
			}
		    }
		}
	    }

	    length -= fd [0].len;
	    offset += fd [0].len;
	}

	return status;
}
