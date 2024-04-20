/* file:	crc.c
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
 *	Calculate a 32 bit signature on a data stream.
 *  AUTHORS:
 *
 *      AJ Beaverson
 *
 *  CREATION DATE:
 *  
 *      26-Feb-1991
 *
 *  MODIFICATION HISTORY:
 *
 *	jad	18-Apr-1991	Pass crc to crc32.
 *
 *	jad	28-Mar-1991	Added crc32.
 *
*--
 */
#include "cp$src:platform.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:stddef.h"
#include	"cp$src:common.h"
#include "cp$inc:prototypes.h"
#include	"cp$src:msg_def.h"

/*+
 * ============================================================================
 * = crc - Calculate a CRC on a file.                                         =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Calculates a CRC on a file(s). If no filenames are present on the
 *	command line, then use standard input.
 *
 *	This command is only built into SBLOAD, SLLOAD, GMLOAD, and GLLOAD.
 *
 *   COMMAND FMT: crc_cmd 0 NZ 0 crc
 *
 *   COMMAND FORM:
 *  
 *	crc ( <file>... )
 *  
 *   COMMAND TAG: crc_cmd 0 RXBZ crc
 *
 *   COMMAND ARGUMENT(S):
 *
 * 	file	- file or files on which to calculate the CRC
 *
 *
 *   COMMAND EXAMPLE(S):
 *~
 *	>>>echo 'the quick brown fox jumped over the lazy dog' >foo
 *	>>>crc foo
 *	D5CE9DD0 foo
 *	>>>echo 'the quick brown fox jumped over the lazy dog' | crc
 *	D5CE9DD0
 *	>>>cat foo >bar
 *	>>>crc foo bar
 *	D5CE9DD0 foo
 *	D5CE9DD0 bar
 *	>>>
 *~
 * FORM OF CALL:
 *  
 *	crc ( argc, *argv[] )
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
int crc_cmd (int argc, char *argv []) {
 	struct FILE	*fin;
	int		argix;
	struct PCB	*pcb;
	int		status;
	unsigned int	check;


	/*
	 * Take from standard input
	 */
	if (argc == 1) {
	    pcb = getpcb ();
	    check = crc_helper (pcb->pcb$a_stdin);
	    printf ("%08X\n", check);
	} 

	/*
	 * Iterate over the arguments
	 */
	for (argix=1; argix<argc; argix++) {
 	    if ((fin = fopen (argv [argix], "r")) == NULL) {
		continue;
	    }
  	    check = crc_helper (fin);
	    fclose (fin);
	    printf ("%08X %s\n", check, argv [argix]);
	}	   	

	return msg_success;
}

/*----------*/
int crc_helper (struct FILE *fin) {
	unsigned char	buf [512];
	int		bytes_read;
	int		i;
	unsigned int crc32 ();
	unsigned int check;
 
	check = 0;
	while (bytes_read = fread (buf, 1, sizeof (buf), fin)) {
	    for (i=0; i<bytes_read; i++) {
		check = crc32 (buf + i, 1, 1, check);
	    }
	    if (killpending ()) break;
	}
	return check;
}


/*+
 * ============================================================================
 * = crc32 - Perform crc 32 on a bitstream.				      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Performs a crc 32 operation on a bitstream.
 *
 * FORM OF CALL:
 *
 *	crc32 (m,n,s,crc); 
 *
 * RETURNS:
 *
 *	unsigned int crc - The CRC for the bitstream.
 *
 * ARGUMENTS:
 *
 *      unsigned char *m - Pointer to the start of the bitstream.
 *	unsigned int n - Number of items in the bitstream.
 *	unsigned int s - Size of each item.
 *	unsigned int crc - CRC to be used as seed.
 *
 * SIDE EFFECTS:
 *
 *      None
-*/

#define CRC32_MASK 0x04c11db6
#define NCRC32_MASK 0x0fb3ee249

unsigned int crc32(unsigned char *m,unsigned int n,unsigned int s,
							      unsigned int crc)
{
int i,j;

	/*Go through each item*/
	for(j=0; j<n; j++)

	    /*Go through each bit in the item*/
	    for(i=0; i<(s*8); i++)

		/*Calculate the CRC*/
		calc_crc32(&crc,(m[j]>>i)&1);

	/*Return the crc*/
	return(crc);
}


/*+
 * ============================================================================
 * = calc_crc32 - Calculate a new crc for a bit.   		              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Calculates a new CRC for a bit.
 *
 * FORM OF CALL:
 *
 *	crc32 (crc,bit); 
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	unsigned int *crc - Pointer to the CRC. 
 *	int bit - 0 or 1 the value of the bit being worked on.
 *
 * SIDE EFFECTS:
 *
 *      None
-*/

void calc_crc32(unsigned int *crc,int bit)
{

/*Modify the CRC according to bit passed*/
	if(((*crc>>31)&1) ^ bit)
	{
	    *crc <<= 1;
	    *crc = ((((*crc ^ 0xffffffff) & CRC32_MASK) |
					(*crc & NCRC32_MASK)) & 0xfffffffe) | 1;
	}
	else
	{
	    *crc <<= 1;
	    *crc = ((((*crc^0) & CRC32_MASK) | 
					(*crc & NCRC32_MASK)) & 0xfffffffe);
	}
}
