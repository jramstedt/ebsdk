/* file:	hd.c
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
 *	hd (hexadecimal dump) command
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
 *	ajb	30-jan-1992	broken out from shell_commands
 *--
 */
#include "cp$src:platform.h"
#include "cp$src:kernel_def.h"
#include "cp$src:parse_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:common.h"
#include "cp$inc:prototypes.h"
#include "cp$src:msg_def.h"
#include "cp$src:ctype.h"

/*+
 * ============================================================================
 * = hd - Dump the contents of a file in hexadecimal and ASCII.               =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Dump the contents of a file in hexadecimal and ASCII.
 *
 *   COMMAND FMT: hd 0 Z 0 hd
 *
 *   COMMAND FORM:
 *  
 *	hd ( [-{byte|word|long|quad}] [-{sb|eb} <n>] <file>[:<offset>]... )
 *  
 *   COMMAND TAG: hd 0 RXBZ hd
 *
 *   COMMAND ARGUMENT(S):
 *
 * 	<file>... - Specifies the file or files to be displayed.
 *
 *   COMMAND OPTION(S):
 *
 *	-byte - print out data in byte sizes
 *	-word - print out data by word
 *	-long - print out data by longword
 *	-quad - print out data  by quadword
 *
 *	-sb   - start block
 *	-eb   - end block
 *
 *   COMMAND EXAMPLE(S):
 *~
 *	>>>hd toy
 *	00000000  01 00 04 00 0E FF 05 1B 05 2F 25 0E 50 80 00 45   ........./%.P..E
 *	00000010  7F 45 03 7F 7F 7F 7F 7F 00 00 7F 7F 7F 7F 7F 7F   .E..............
 *	00000020  7F 7F 7F 7F 7F 7F 7F 7F 7F 7F 7D 7F 7F 7F 7F 01   ..........}.....
 *	00000030  7F 7F 7F 7F 7F 7F 7F 7F 7F 7F 7F 7F 7F 7F 7E 02   ..............~.
 *	>>>hd toy:10 -word
 *	00000000  457F 7F03 7F7F 7F7F 0000 7F7F 7F7F 7F7F   .E..............
 *	00000010  7F7F 7F7F 7F7F 7F7F 7F7F 7F7D 7F7F 017F   ..........}.....
 *	00000020  7F7F 7F7F 7F7F 7F7F 7F7F 7F7F 7F7F 027E   ..............~.
 *	>>>hd toy:20 -long
 *	00000000  7F7F7F7F 7F7F7F7F 7F7D7F7F 017F7F7F   ..........}.....
 *	00000010  7F7F7F7F 7F7F7F7F 7F7F7F7F 027E7F7F   ..............~.
 *	>>>hd toy:30 -quad
 *	00000000  7F7F7F7F7F7F7F7F 7F7F7F7F027E7F7F   ..............~.
 *	>>>hd toy:30 -byte
 *	00000000  7F 7F 7F 7F 7F 7F 7F 7F 7F 7F 7F 7F 7F 7F 7E 02   ..............~.
 *	>>>hd toy -eb 1
 *	block 0
 *	00000000  01 00 04 00 0E FF 05 1B 05 2F 25 0E 50 80 00 45   ........./%.P..E
 *	00000010  7F 45 03 7F 7F 7F 7F 7F 00 00 7F 7F 7F 7F 7F 7F   .E..............
 *	00000020  7F 7F 7F 7F 7F 7F 7F 7F 7F 7F 7D 7F 7F 7F 7F 01   ..........}.....
 *	00000030  7F 7F 7F 7F 7F 7F 7F 7F 7F 7F 7F 7F 7F 7F 7E 02   ..............~.
 *	>>>
 *~
 *
 * FORM OF CALL:
 *  
 *	hd ( argc, *argv[] )
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
 *	None
 *
-*/

#define QBYTE	0
#define QWORD	1
#define QLONG	2
#define QQUAD	3
#define QSB	4
#define QEB	5
#define QQ	6
#define QMAX	7
#define QSTRING "byte word long quad %dsb %deb quiet"

#define MAX_DATUM 8			/* quadword is max data size */

int hd(int argc, char *argv[])
{
    struct FILE *fin;
    int argix;
    struct PCB *pcb;
    struct QSTRUCT qual[QMAX];
    int status;
    unsigned int offset;
    int datawidth;
    int sblock;
    int eblock;
    int length;
    int quiet;

/* Pick off qualifiers */

    status = qscan(&argc, argv, "-", QSTRING, qual);
    if (status != msg_success) {
	err_printf(status);
	return status;
    }

    pcb = getpcb();

    quiet = 0;
    offset = 0;
    sblock = 0;
    eblock = 0;
    length = -1;
    datawidth = 1;

    if (qual[QQ].present)
	quiet = 1;

    if (qual[QBYTE].present)
	datawidth = 1;
    if (qual[QWORD].present)
	datawidth = 2;
    if (qual[QLONG].present)
	datawidth = 4;
    if (qual[QQUAD].present)
	datawidth = 8;

    if (qual[QSB].present) {
	sblock = qual[QSB].value.integer;
	eblock = sblock + 1;
	length = 512;
	offset = sblock * 512;
    }
    if (qual[QEB].present) {
	eblock = (qual[QEB].value.integer) + 1;
	length = (eblock - sblock) * 512;
    }

/* Take from standard input */

    if (argc == 1) {
	hd_helper(pcb->pcb$a_stdout, pcb->pcb$a_stdin, offset, length,
	  datawidth);
	return msg_success;
    }

/* Iterate over the arguments */

    for (argix = 1; argix < argc; argix++) {

	if ((fin = fopen(argv[argix], "r")) == NULL)
	    continue;

	if (eblock && fin) {
	    hd_block(fin, offset, length, datawidth, quiet);
	} else {
	    hd_helper(pcb->pcb$a_stdout, fin, offset, length, datawidth);
	}
	fclose(fin);
    }
    return (int) msg_success;
}

void hd_block(struct FILE *fin, unsigned int offset,
  unsigned int n, int datawidth, int quiet)
{
    unsigned int bytes_read;
    struct FILE *dout;
    struct PCB *pcb;
    char *bufp;

    bufp = malloc(512);

    pcb = getpcb();

    while (n) {

	if (offset)
	    fseek(fin, offset, SEEK_SET);

	bytes_read = fread(bufp, 1, 512, fin);
	if (bytes_read == 0)
	    break;

	printf("block %d\n", offset / 512);

	if (!quiet) {
	    dout = fopen("PMEM", "r");
	    fseek(dout, bufp, SEEK_SET);
	    hd_helper(pcb->pcb$a_stdout, dout, 0, bytes_read, datawidth);
	    fclose(dout);
	}	   

	if (killpending ()) break;

	offset += bytes_read;
	n -= bytes_read;
    }
    free(bufp);
}
