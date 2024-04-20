/* file:	bufcat.c
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
 *	bufCat (buffered catenate) command
 *
 *  AUTHORS:
 *
 *      AJ Beaverson
 *
 *  CREATION DATE:
 *  
 *      20-Aug-1993
 *
 *  MODIFICATION HISTORY:
 *
 *--
 */
#include "cp$src:platform.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:parse_def.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:common.h"
#include "cp$inc:prototypes.h"
#include	"cp$src:limits.h"

/*+
 * ============================================================================
 * = bufcat - Buffered Copy files to standard output.                                     =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Concatenates files that you specify to the standard output. If you 
 *	do not specify files on the command line, bufcat copies standard input 
 *	to standard output. 
 *
 *	This command is a buffered version of the cat command. The command
 *	creates separate reader and writer processes so that overlapped i/o
 *	may be performed.  This is especially useful for disk copies.
 *
 *  
 *   COMMAND FMT: bufcat 4 Z 0 bufcat
 *
 *   COMMAND FORM:
 *  
 *	bufcat ( [-block <hex_buffersize> [-buf <hex_bufcount>] <file>... )
 *  
 *   COMMAND TAG: bufcat 0 RXBZ bufcat
 *
 *   COMMAND ARGUMENT(S):
 *
 * 	<file>... - Specifies the name of the input file or files to be copied.
 *
 *   COMMAND OPTION(S):
 *
 *	-l <num> - Specifies the number of bytes (decimal) of each input file
 *		   to copy (not implemented for bufcat)
 *	-buffer <num> - Specifies the number of buffers to use.  Default is 2.
 *	-block <num> - Specifies the buffer size.  Default is 2048 bytes.
 *
 *   COMMAND EXAMPLE(S):
 *~
 *~
 *  
 * FORM OF CALL:
 *  
 *	bufcat ( argc, *argv[] )
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

#define	QL	0	/* length			*/
#define	QBLOCK	1	/* blocksize			*/
#define	QBUFFER	2	/* number of buffers to use	*/
#define	QMAX	3
#define QSTRING	"%xl %xblock %dbuffers"

typedef struct _CATCTX {
	struct FILE	*fin;
	struct FILE	*fout;
	unsigned char	**buf;	/* array of pointers to buffers */
	unsigned int 	*len;	/* array of lengths		*/
	unsigned int	in;
	unsigned int	out;
	unsigned int	num_blocks;	/* number of blocks		*/
	unsigned int	blocksize;      /* in bytes 			*/

	struct SEMAPHORE	full;
	struct SEMAPHORE	empty;

	struct SEMAPHORE	reader_done;
	struct SEMAPHORE	writer_done;	
} CATCTX;

extern int null_procedure ();

bufcat (int argc, char *argv[]) {
	CATCTX		*catp;
	int		argix;
	struct PCB	*pcb;
	struct QSTRUCT	qual [QMAX];
	int		status;
	int		i;

	catp = (struct CATP *) malloc (sizeof (*catp));

	/*
	 * Pick off qualifiers
	 */
	status = qscan (&argc, argv, "-", QSTRING, qual);
	if (status != msg_success) return status;

	/* Number of buffers to use.  Default to 2 (otherwise the user should
	 * be using the regular cat)
	 */
	catp->num_blocks = 2;
	if (qual [QBUFFER].present) {
	    catp->num_blocks = qual [QBUFFER].value.integer;
	}

	/*
	 * Allocate space for the buffers
	 */
	catp->blocksize = DEF_ALLOC;
	if (qual [QBLOCK].present) catp->blocksize = qual [QBLOCK].value.integer;
	catp->buf = malloc (catp->num_blocks * sizeof (char *));
	catp->len = malloc (catp->num_blocks * sizeof (int));
	for (i=0; i<catp->num_blocks; i++) {
	    catp->buf [i] = dyn$_malloc (catp->blocksize, DYN$K_SYNC|DYN$K_NOFLOOD| DYN$K_NOWAIT);
	    if (catp->buf [i] == 0) {
		err_printf ("can't allocate enough memory\n");
		return msg_failure;
	    }
	}

	/* default the input and output files */
	pcb = getpcb();
	catp->fout = pcb->pcb$a_stdout;
	catp->fin = pcb->pcb$a_stdin;

	/*
	 * copy from stdin to stdout
	 */
	if (argc == 1) {
	   status = bufcat_helper (catp);

	/*
	 * Copy from a list of files to stdout
	 */
	} else {
	    for (argix=1; argix<argc; argix++) {
		if ((catp->fin = fopen (argv [argix], "r")) == NULL) {
		    status = msg_nofile;
		    break;
		}
		status = bufcat_helper (catp);
		fclose (catp->fin);
	    }
	}

	return (int) status;
}	    


cat_reader (CATCTX *catp) {
	int	length;
	int	t;

	t = 0;
	do {
	    krn$_wait (&catp->empty);

	    length = fread (catp->buf [catp->in], 1, catp->blocksize, catp->fin);
	    catp->len [catp->in] = length;

	    catp->in++;
	    catp->in %= catp->num_blocks;

	    krn$_post (&catp->full);
	    t += length;

	} while (length);
	return t;
}

cat_writer (CATCTX *catp) {
	int	t;

	t = 0;
	while (1) {
	    krn$_wait (&catp->full);

	    if (catp->len [catp->out] == 0) break;
	    t += catp->len [catp->out];
	    fwrite (catp->buf [catp->out], 1, catp->len [catp->out], catp->fout);

	    catp->out++;
	    catp->out %= catp->num_blocks;

	    krn$_post (&catp->empty);
	}
	return t;
}

bufcat_helper (CATCTX *catp) {
	int		reader_pid;
	int		writer_pid;
	struct PCB 	*pcb;

	pcb = getpcb ();

	krn$_seminit (&catp->empty, catp->num_blocks, "cat_empty");
	krn$_seminit (&catp->full, 0, "cat_full");
	krn$_seminit (&catp->reader_done, 0, "reader_done");
	krn$_seminit (&catp->writer_done, 0, "writer_done");

	reader_pid = krn$_create (
	    cat_reader,			/* procedure		*/
	    null_procedure,		/* startup routine	*/
	    &catp->reader_done,		/* completion semaphore	*/
	    pcb->pcb$l_pri,		/* priority		*/
	    -1, 			/* affinity		*/
	    0,				/* size			*/
	    "cat_reader",		/* process name		*/
	    0, 0,			/* stdin		*/
	    0, 0,			/* stdout		*/
	    0, 0,			/* parent's stderr	*/
	    catp
	);

	writer_pid = krn$_create (
	    cat_writer,			/* procedure		*/
	    null_procedure,		/* startup routine	*/
	    &catp->writer_done,		/* completion semaphore	*/
	    pcb->pcb$l_pri,		/* priority		*/
	    -1, 			/* affinity		*/
	    0,				/* size			*/
	    "cat_writer",		/* process name		*/
	    0, 0,			/* stdin		*/
	    0, 0,			/* stdout		*/
	    0, 0,			/* parent's stderr	*/
	    catp
	);

	krn$_wait (&catp->reader_done);
	krn$_wait (&catp->writer_done);

	return msg_success;
}

