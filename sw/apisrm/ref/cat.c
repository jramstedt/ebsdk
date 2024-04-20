/* file:	cat.c
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
 *	Cat (catenate) command
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
 *	phk	11-may-1992	Add -l qualifier
 *	ajb	30-jan-1992	Broken out from shell_commands
 *--
 */
#include "cp$src:platform.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:parse_def.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:ansi_def.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:common.h"
#include "cp$inc:prototypes.h"
#include	"cp$src:limits.h"

#if TURBO
extern int conzone_inited;
#endif

/*+
 * ============================================================================
 * = cat - Catenate files to standard output.                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Concatenates files that you specify to the standard output. If you 
 *	do not specify files on the command line, cat copies standard input 
 *	to standard output. 
 *
 *	You can also copy or append one file to another by specifying 
 *	I/O redirection.
 *
 *	Portions of files may be catenated by using the -start and -length
 *	qualifiers.
 *  
 *   COMMAND FMT: cat 0 Z 0 cat
 *
 *   COMMAND FORM:
 *  
 *	cat ( [-length <num>] [-block <size>] [-start <offset>] 
 *	      [-quiet] [<file>...] )
 *  
 *   COMMAND TAG: cat 0 RXBZ cat
 *
 *   COMMAND ARGUMENT(S):
 *
 * 	<file>... - Specifies the name of the input file or files to be copied.
 *
 *   COMMAND OPTION(S):
 *
 *	-length <num> - Specifies the number of bytes in hex of each input file
 *		   to copy.
 *
 *	-start  <offset> - Specifies the offset to seek to in hex.  If the
 *		file(s) are not seekable (eg pipelines), then this qualifier
 *		has no effect.
 *
 *	-block  <size> - Size of the internal buffer cat uses to copy files, in
 *		hex. By default, this is DEF_ALLOC (2048) bytes.  For
 *		performance in copying disks, a number such as 10000 can be
 *		used.
 *
 *	-quiet	- Use silent mode on fopens.
 *
 *   COMMAND EXAMPLE(S):
 *~
 *	>>>echo acbde >foo
 *	>>>hd foo
 *	00000000  61 63 62 64 65 0A                                 acbde.
 *	>>>cat foo
 *	acbde
 *	>>>cat foo foo
 *	acbde
 *	acbde
 *	>>>cat -l 3 foo
 *	acb>>>cat -s 1 -l 1 foo
 *	c>>>
 *	>>>cat foo |cat|cat
 *	acbde
 *	>>>
 *~
 *   COMMAND REFERENCES:
 *  
 *	echo, hd, more
 *  
 * FORM OF CALL:
 *  
 *	cat ( argc, *argv[] )
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

#define QS	0	/* starting address */
#define	QL	1	/* length	*/
#define	QB	2	/* blocksize	*/
#define	QQ	3	/* quiet */
#define QCRLF	4	/* CRLF mode (CR, CR/LF, LF/CR, all map to LF) */
#define	QMAX	5
#define QSTRING	"%xstart %xlength %xblock quiet crlf"

int cat (int argc, char *argv[]) {
	int		argix;
	struct FILE	*fin;
	struct FILE	*fout;
	struct PCB	*pcb;
	struct QSTRUCT	qual [QMAX];
	unsigned char	*buf;
	int		status;
	unsigned int	blocksize;
	char		*mode;

	/*
	 * Pick off qualifiers
	 */
	status = qscan (&argc, argv, "-", QSTRING, qual);
	if (status != msg_success){
	    err_printf( status ); 	
	    return status;
        }

	mode = "r";
	if (qual[QQ].present) mode = "rs";

	pcb = getpcb();
	fout = pcb->pcb$a_stdout;

#if TURBO
	if (conzone_inited) {
	    if (fout->local_offset[0] == 0x800000) {
		err_printf("Illegal after Diagnostic Mode, must Init first\n");
		return msg_failure;
	    }
	}
#endif
	/*
	 * Allocate space for the buffer
	 */
	blocksize = DEF_ALLOC;
	if (qual [QB].present) blocksize = qual [QB].value.integer;
        buf = (char *) dyn$_malloc (blocksize, DYN$K_SYNC | DYN$K_NOFLOOD | DYN$K_NOWAIT);
	if (!buf) {
	    err_printf ("cat: can't allocate enough memory\n");
	    return msg_failure;
	}
	/*
	 * copy from stdin to stdout
	 */
	if (argc == 1) {
	    status = cat_helper (qual, buf, blocksize, pcb->pcb$a_stdin, pcb->pcb$a_stdout);

	/*
	 * Copy from a list of files to stdout
	 */
	} else {
	    for (argix=1; argix<argc; argix++) {
		if ((fin = fopen (argv [argix], mode)) == NULL) {
		    status = msg_nofile;
		    break;
		}
		status = cat_helper (qual, buf, blocksize, fin, fout);
		fclose (fin);
	    }
	}

	free (buf);
	return (int) status;
}	    

int cat_helper (struct QSTRUCT *qual, char *buf, int blocksize, struct FILE *fin, struct FILE *fout) {
	unsigned int	bytes_requested;
	unsigned int	bytes_read;
	unsigned int	bytes_written;
	unsigned int	offset;
	int		status;
	int		i;

	/*
	 * If the length is not specified, set it to a large number
	 * (and hope that we never see that many bytes)
	 */
	if (qual [QL].present) {
	    bytes_requested = qual [QL].value.integer;
	} else {
	    bytes_requested = LONG_MAX;
	}

	/* seek (or skip) to the proper offset */
	offset = 0;
	if (qual [QS].present) {
	    offset = qual [QS].value.integer;
	}
	if (offset) {
	    if (!fin->ip->dva->sequential) {
		fseek (fin, offset, SEEK_SET);
	    } else {
		/* skip over information by sequential reads */
	    }
	}

	while ((bytes_requested > 0) && !killpending ()) {
	    bytes_read = fread (buf, min (bytes_requested,blocksize), 1, fin);
	    if (bytes_read == 0) break;

	    if (qual [QCRLF].present) {
		for (i=0; i<bytes_read; i++) {
		    if (buf[i] == CR) {
			buf[i] = LF;
		        if (i+1 < bytes_read && buf[i+1] == LF) {
			    bytes_read--;
			    memcpy (buf+i+1, buf+i+2, bytes_read-i-1);
			}
		    } else if (buf[i] == LF) {
		        if (i+1 < bytes_read && buf[i+1] == CR) {
			    bytes_read--;
			    memcpy (buf+i+1, buf+i+2, bytes_read-i-1);
			}
		    }
		}
	    }

	    if (fwrite (buf, bytes_read, 1, fout) != bytes_read) {
		return fout->status;
	    }

	    bytes_requested -= bytes_read;
	}

	return msg_success;
}
