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
__attribute__((unused)) static const char *rcsid = "$Id: hexpad.c,v 1.1.1.1 1998/12/29 21:36:22 paradis Exp $";

/*
 * $Log: hexpad.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:22  paradis
 * Initial CVS checkin
 *
 * Revision 1.2  1995/12/23  20:27:33  fdh
 * Changed the default padded data size.
 *
 * Revision 1.1  1995/12/23  18:48:24  fdh
 * Initial revision
 *
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define MAX_READ 120

#define MIN(a,b) (((a)<(b))?(a):(b))
int *icCode;
char ibuf[MAX_READ];
long pad_to;
int pad_byte;
int bytes_per_line;

unsigned long binToHex(FILE *hf)
{
    unsigned long sum = 0;
    long addr, j, checkSum;
    int byte_count;

    addr = 0;

    while (addr < pad_to) {
      byte_count = MIN(bytes_per_line, (int) (pad_to - addr));
      checkSum = byte_count + 0x00 + (addr & 0xff) + ((addr >> 8) & 0xff);
      fprintf(hf, ":%02X", byte_count);
      fprintf(hf, "%02X", (addr >> 8) & 0xff);
      fprintf(hf, "%02X", addr & 0xff);
      fprintf(hf, "00");
      for (j = 0; j < byte_count; j++) {
	fprintf(hf, "%02X", icCode[addr] & 0xff);
	checkSum += icCode[addr];
	sum = (sum + (unsigned long) icCode[addr])
	  & (unsigned long) 0xffffffff;
	++addr;
      }
      fprintf(hf, "%02X", (0 - checkSum) & 0xff);
      fprintf(hf, "\n");
    }
    fprintf(hf, ":00000001FF\n");
    fflush(hf);
    return (sum);
}

unsigned int readHex(FILE *ef)
{
  int cnt = 0;
  int i;
  int byte_count;
  unsigned long address;
  int record_type;
  int cksum, cksum_data;
  int lines = 0;

  icCode = malloc(sizeof(int) * (size_t)(pad_to+1)); /* a place to hold the data */
  /* Initialize the whole array */
  for (i=0; i<pad_to; ++i)
    icCode[i] = pad_byte;

  while (fgets(ibuf, MAX_READ, ef) != 0) {
    if (cnt > pad_to) {
      fprintf(stderr, "ERROR: data size (%x) already exceeds padding requirement (%x).\n", cnt, pad_to);
      exit(1);
    }

    ++lines;
    sscanf(ibuf, ":%02X%04X%02X", &byte_count, &address, &record_type);
    cksum_data = byte_count + (int) ((address & 0xff) + ((address >> 8) & 0xff)) + record_type;
    for (i=0; i<byte_count; ++i) {
      sscanf(ibuf+(2*i)+9, "%02X", &icCode[cnt]);
      cksum_data += icCode[cnt];
      ++cnt;
    }
    sscanf(ibuf+(2*i)+9, "%02X", &cksum);
    cksum_data = (0 - cksum_data) & 0xff;

    if (cksum != cksum_data) {
      fprintf(stderr, "ERROR: Checksum error on line %d\n", lines);
      exit(1);
    }

  }
  return (cnt);
}

void usage()
{
  fprintf(stderr, "usage: hexpad [-hvxb]  <input-file> <output-file>\n");
  fprintf(stderr, "\tv(V) = verbose\n");
  fprintf(stderr, "\th(H) = help, print this text\n");
  fprintf(stderr, "\tx(X) = specifies padded data size in hex.\n");
  fprintf(stderr, "\tb(B) = padding byte\n");
}

int main(int argc, char *argv[])
{
    FILE *HexIn, *HexOut;
    unsigned long sumcheck;
    char *out_file_name = NULL;	/* default output file */
    char *in_file_name = NULL;
    char *arg, option;
    unsigned int verbose = FALSE;
    int i, codeSize;

    pad_to = 0x2004;		/* Default padded data size */
    pad_byte = 0xFF;		/* Default padding byte */
    bytes_per_line = 16;

    if (argc < 2) {
      usage();
      exit(1);
    }

/*
 * Parse arguments, but we are only interested in flags.
 * Skip argv[0].
 */

    i = 1;
    while (i < argc) {
	arg = argv[i];
	if (*arg == '-') {
/*
 * This is a -xyz style options list.  Work out the options specified.
 */
	    arg++;			/* skip the '-' */
	    while (option = *arg++) {	/* until we reach the '0' string
					 * terminator */
		switch (option) {
		    case 'v': 		/* verbose, allow upper and lower case
					 */
		    case 'V': 
			verbose = TRUE;
			break;

		    case 'h': 
		    case 'H': 
		    default: 
			usage();
			exit(1);
			break;

		    case 'X': 
		    case 'x': 
/*
 *  This is a special one, the -x option is followed (immediately)
 *  by a Hex value, truncated to 8 digits.  
 */
			{
			  char strbuf[8+1];

			  sscanf(arg, "%8s", strbuf);
			  pad_to = strtoul(strbuf, NULL, 16);
			  printf("pad_to = %x\n", pad_to);
			}
			arg = arg + strlen(arg);
			break;

		    case 'B': 
		    case 'b': 
/*
 *  Specify the padding byte
 */
			{
			  char strbuf[8+1];

			  sscanf(arg, "%8s", strbuf);
			  pad_byte = (int) strtoul(strbuf, NULL, 16);
			  printf("pad_byte = %x\n", pad_byte);
			}
			arg = arg + strlen(arg);
			break;
		}
	    }
	} else {
/*
 *  This is the input filename, ignore multiple filenames, just take the 
 *  last one specified.  This is not a rolls-royce interface.
 */
	    if (in_file_name == NULL) {
		in_file_name = arg;

		if (out_file_name == NULL)
		  out_file_name = "hexpad.hex";
	    } else
	      out_file_name = arg;
	} 
	i++;
    }

    if (verbose == TRUE)
      printf("hexpad - version (%s)\n", rcsid);

    if (out_file_name == NULL) {
      fprintf (stderr, "No input file specified\n");
      usage();
      exit(1);
    }

    if ((HexIn = fopen(in_file_name, "rb")) != NULL) {
      if (verbose == TRUE) {
	printf("Opened input file: %s\n", in_file_name);
      }
    }
    else {
	printf("Cannot open input file %s\n", in_file_name);
	return (1);
    }

    if (out_file_name == NULL) {
      fprintf (stderr, "No output file specified\n");
      usage();
      exit(1);
    }

    if ((HexOut = fopen(out_file_name, "wb")) != NULL) {
      if (verbose == TRUE) {
	printf("Opened output file: %s\n", out_file_name);
      }
    }
    else {
      printf("Cannot open output file %s\n", out_file_name);
      fclose(HexOut);
      return (1);
    }

    codeSize = readHex(HexIn);

    printf("Input image size: %d (0x%X) bytes\n", codeSize, codeSize);
    if (codeSize > 0) {
      /* write out the hex file */
      sumcheck = binToHex(HexOut);
      printf("Output image %d (0x%X) bytes written to %s\n",
	   pad_to, pad_to, out_file_name);
      printf("\tData sum = %08X\n", sumcheck);
    }

    fclose(HexOut);
    return (0);
}

