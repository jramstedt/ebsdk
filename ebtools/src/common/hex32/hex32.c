/*****************************************************************************

       Copyright � 1993, 1994 Digital Equipment Corporation,
                       Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, provided  
that the copyright notice and this permission notice appear in all copies  
of software and supporting documentation, and that the name of Digital not  
be used in advertising or publicity pertaining to distribution of the software 
without specific, written prior permission. Digital grants this permission 
provided that you prominently mark, as not part of the original, any 
modifications made to this software or documentation.

Digital Equipment Corporation disclaims all warranties and/or guarantees  
with regard to this software, including all implied warranties of fitness for 
a particular purpose and merchantability, and makes no representations 
regarding the use of, or the results of the use of, the software and 
documentation in terms of correctness, accuracy, reliability, currentness or
otherwise; and you rely on the software, documentation and results solely at 
your own risk. 

******************************************************************************/
#ifndef LINT
__attribute__((unused)) static const char *rcsid = "$Id: hex32.c,v 1.1.1.1 1998/12/29 21:36:20 paradis Exp $";
#endif

/*
 *  ==========================================
 *  generate an Intel hex MCS86 file on stdout
 *  ==========================================
 *
 * $Log: hex32.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:20  paradis
 * Initial CVS checkin
 *
 * Revision 1.5  1997/02/21  20:34:19  fdh
 * Corrected index into seen array.
 * Report version.
 *
 * Revision 1.4  1996/08/20  17:46:49  fdh
 * Added casts...
 *
 * Revision 1.3  1996/06/30  20:54:48  fdh
 * Corrected version banner.
 *
 * Revision 1.2  1996/06/07  02:24:50  fdh
 * Added extended and offset data records.
 *
 * Revision 1.1  1996/06/05  18:54:21  fdh
 * Initial revision
 *
 */

#ifdef VMS
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#else
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define BYTES_PER_REC	16
#define BYTES_PER_BLOCK	(BYTES_PER_REC*16)

static unsigned char buf[BYTES_PER_BLOCK];

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define _SEEN(o) (seen[o-'a'])
#define SEEN_SIZE ('z' - 'a' + 10)
char seen[SEEN_SIZE];			/* upper and lower case */
/*
 *  Declare the routines in this module.
 */
int main(int argc, char **argv);
static unsigned long generate_intel_hex_mcs86(unsigned int offset, FILE *in, FILE *out);

static char *Revision = "$Revision: 1.1.1.1 $";
static char *DateStamp = __DATE__;
static char *TimeStamp = __TIME__;

int main(int argc, char **argv)
{
    char *infile = NULL, *outfile = NULL;
    FILE *ifile = NULL, *ofile = NULL;

    unsigned int offset = 0;
    int i;
    char option, *arg;

/*
 * Parse arguments, but we are only interested in flags.
 * Skip argv[0].
 */

    for (i = 0; i < SEEN_SIZE; i++)
	seen[i] = FALSE;

    for (i = 1; i < argc; i++) {
	arg = argv[i];
	if (*arg == '-') {
/*
 * This is a -xyz style options list.  Work out the options specified.
 */
	    arg++;			/* skip the '-' */
	    while (option = *arg++) {	/* until we reach the '0' string
					 * terminator */
		option = tolower(option);
		switch (option) {
		    case 'v':	/* verbose */
			_SEEN(tolower(option)) = TRUE;
			printf("hex32 - version (%s)\n", rcsid);
			break;

		    case 'o':	/* Offset */
			_SEEN(tolower(option)) = TRUE;
			sscanf(arg, "%X", &offset);
			arg = arg + strlen(arg);
			break;

		    case 'h': 
		    default: 
			fprintf(stderr, "%s - generate an Intel hex32 (MCS86) file.\n", argv[0]);
			fprintf(stderr, "usage: %s [-v -oOffset] <input-file> <output-file>\n", argv[0]);
			fprintf(stderr, "\t-v = verbose, print more information than usual\n");
			fprintf(stderr, "\t-o = Offset, specifies image offset.\n");
			fprintf(stderr, "If input and output files are not\n");
			fprintf(stderr, "specified, then stdin and stdout are used\n");
			exit(0);
			break;
		}
	    }
	} else {
/*
 *  This is a filename, ignore multiple filenames
 */
	    if (infile == NULL)
		infile = arg;
	    else {
		if (outfile != NULL) {
		    fprintf(stderr, "ERROR, too many files supplied\n");
		    exit(1);
		}
		outfile = arg;
	    }
	}
    }

/*
 *  If verbose has been selected reveal the arguments.
 */
    if (_SEEN('v')) {
	fprintf(stderr, "%s - generate an Intel hex32 file [%s %s %s]\n",
		argv[0], Revision, DateStamp, TimeStamp);
	fprintf(stderr, "\n");
    }

/*
 *  If neccessary, open the files (they may be stdin and stdout)
 */

    if (infile != NULL) {
	if (_SEEN('v'))
	    fprintf(stderr, "...input file is %s\n", infile);
#ifdef _WIN32
	ifile = fopen(infile, "rb");
#else
	ifile = fopen(infile, "r");
#endif
	if (ifile == NULL) {
	    fprintf(stderr, "ERROR, failed to open input file %s\n", infile);
	    exit(0);
	}
    }
    if (outfile != NULL) {
	if (_SEEN('v'))
	    fprintf(stderr, "...output file is %s\n", outfile);
	ofile = fopen(outfile, "w");
	if (ofile == NULL) {
	    fprintf(stderr, "ERROR, failed to open output file %s\n", outfile);
	    exit(0);
	}
    }

/*
 *  Now generate an Intel hex MCS86.
 */
    generate_intel_hex_mcs86(offset, (ifile == NULL) ? stdin : ifile, 
      (ofile == NULL) ? stdout : ofile);
/*
 *  Close any opened files.
 */
    if (ifile != NULL)
	fclose(ifile);
    if (ofile != NULL)
	fclose(ofile);
/*
 *  Exit with a good status.
 */
    return EXIT_SUCCESS;
}

static unsigned long generate_intel_hex_mcs86(unsigned int offset, FILE *in, FILE *out)
{
  unsigned long SumCheck;
  unsigned long byte_total;
  unsigned long OffsetAddr;
  unsigned long addr;
  unsigned long CheckSum;
  size_t buff_count;
  size_t byte_count;
  int i, j;

  OffsetAddr = offset;
  addr = offset;			/* where offset is zero or set by the
					 * caller */

  SumCheck = (unsigned long) 0;
  byte_total = (unsigned long) 0;

  if _SEEN('v')
    fprintf(stderr, "...placing object at offset %08X\n", addr);

  while (TRUE) {
    if ((buff_count = fread(buf, 1, BYTES_PER_BLOCK, in)) == 0)
      break;			/* eof */

    CheckSum = 0x02 + 0x00 + 0x00 + 0x02
      + ((addr >> (8+4)) & 0xff) + (addr >> 4) & 0xff;
    fprintf(out, ":02000002");
    fprintf(out, "%02X", (addr >> (8+4)) & 0xff);
    fprintf(out, "%02X", (addr >> 4) & 0xff);
    fprintf(out, "%02X", (0 - CheckSum) & 0xff);
    fprintf(out, "\n");

    if ((addr % 0x100000) == 0) {
      CheckSum = 0x02 + 0x00 + 0x00 + 0x04
	+ ((addr >> (16+8)) & 0xff) + (addr >> 16) & 0xff;
      fprintf(out, ":02000004");
      fprintf(out, "%02X", (addr >> (16+8)) & 0xff);
      fprintf(out, "%02X", (addr >> 16) & 0xff);
      fprintf(out, "%02X", (0 - CheckSum) & 0xff);
      fprintf(out, "\n");

      addr = (unsigned long) 0;
    }
    
    OffsetAddr = addr;
    for (i=0; i < (int) buff_count; i+=BYTES_PER_REC) {
      byte_count = MIN(buff_count-i,BYTES_PER_REC);
      CheckSum = byte_count + 0x00
	+ ((addr-OffsetAddr) & 0xff) + (((addr-OffsetAddr) >> 8) & 0xff);
      fprintf(out, ":%02X", byte_count);
      fprintf(out, "%02X", ((addr-OffsetAddr) >> 8) & 0xff);
      fprintf(out, "%02X", (addr-OffsetAddr) & 0xff);
      fprintf(out, "00");
      for (j = 0; j < (int) byte_count; j++) {
	fprintf(out, "%02X", buf[i+j] & 0xff);
	CheckSum += (int) buf[i+j];
	SumCheck = (SumCheck + (unsigned long) buf[i+j])
	  & (unsigned long) 0xffffffff;
      }
      fprintf(out, "%02X", (0 - CheckSum) & 0xff);
      fprintf(out, "\n");
      addr += byte_count;
      byte_total += (long) byte_count;
    }
  }

  fprintf(out, ":00000001FF\n");
  fflush(out);

  fprintf(stderr, "\n----- Data sum = %08lX   Data size = %ld (0x%lX, %3.2f KB) -----\n\n", 
	  SumCheck, byte_total, byte_total, (float) byte_total/1024);

  return SumCheck;
}
