/*
**++
**  FACILITY:  Common console compression file
**
**  MODULE DESCRIPTION:
**
**      This module is the main control module for the compression code.  This
**	is based on the front end of the LZW module.  The LZW algorithm is no
**	longer used and is replaced by a huffman encoding algorithm that is more
**	efficient and is unencumbered.
**
**  AUTHORS:
**
**      Don Rice and others.  See inflate.c and deflate.c for all of the real
**	information on those files.
**
**  CREATION DATE:  10-Mar-1994
**
**  MODIFICATION HISTORY:
**
**      10-Mar-1994 D. Rice - original implementation
**--
**
** $Log: hfcomp.c,v $
** Revision 1.1.1.1  1998/12/29 21:36:25  paradis
** Initial CVS checkin
**
 * Revision 1.16  1997/11/19  21:48:33  pbell
 * Updated for DC21264
 *
 * Revision 1.15  1997/05/06  15:43:33  fdh
 * Corrected a typo.
 *
 * Revision 1.14  1997/05/06  15:26:23  fdh
 * Modified the 21064, 21066 and 21164 switches to recogize
 * the various variations of those families.  Also update the
 * usage text.
 *
 * Revision 1.13  1997/02/21  20:36:58  fdh
 * Corrected seen indexing.
 * Report version for verbose mode.
 *
 * Revision 1.12  1996/12/22  12:25:31  fdh
 * Modified to use a different header signature.  The old
 * heaer signature is also present for backward compatibility
 * with the old header.  The new signature indicates the
 * presence of additional header fields.
 *
 * Corrected error handling.
 *
 * Revision 1.11  1996/07/23  22:33:33  fdh
 * Conditionalized for _WIN32 because we can't use the
 * linker defined _end symbol.
 *
 * Revision 1.10  1996/06/13  03:15:39  fdh
 * Modified to use the linker defined variable _end instead
 * of the harwired precessor definition.
 *
 * Revision 1.9  1996/04/30  20:46:46  cruz
 * Changed the name of some variables to avoid lint warning.
 *
*/
#ifndef LINT
static char *rcsid = "$Id: hfcomp.c,v 1.1.1.1 1998/12/29 21:36:25 paradis Exp $";
#endif

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <ctype.h>
#include "compress.h"
#include "header.h"

#ifdef VMS
#define SUCCESS 1
#define FAILURE 0
#else 
#define SUCCESS 0
#define FAILURE 1
#endif


unsigned mop_header[128] = {
 0x003000A8 , 0x00580044 , 0x00000000 , 0x35303230 , 0x00000101 , 0xFFFFFFFF , 0xFFFFFFFF , 0x00000000 ,
 0x01000020 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 ,
 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x504F4D03 , 0x00000000 ,
 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 ,
 0x2E315604 , 0x00000030 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x2D353005 , 0x00003530 ,
 0x00000000 , 0x00000000 , 0x00000010 , 0x00000000 , 0x00000080 , 0x00000002 , 0xFFFF0000 , 0xFFFFFFFF ,
 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF ,
 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF ,
 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF ,
 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF ,
 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF ,
 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF ,
 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF ,
 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF ,
 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF ,
 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF 
};
#define NB_BLOCK_IX  0xAA  /* offset of 'number_of_block' in the header */

struct header *hp;

FILE *fin;
FILE *fout;
FILE *prein;

static char *infile;
static char *outfile;

int osize;
unsigned size;
int prepend_size;

UCHAR	*prepend_buf;

double compression;
UCHAR *inptr;
UCHAR *outptr;

struct stat statbuf;

int verbose = FALSE;

/* Initialization FORCES variables into .data section. */
/* This is important, because .bss normally established by the OS, */
/* and for the firmware boot loading, it is not. */

PUCHAR compressed = 0;
LONG compressedSize = 0;

PUCHAR decompressed = 0;
LONG decompressedSize = 0;

INT bits_left = 0;

#undef max
#undef min
#define max(a,b) (a) > (b) ? (a) : (b);
#define min(a,b) (a) < (b) ? (a) : (b);

#define TOOLBOX_VARIABLE "EB_TOOLBOX"
#define _SEEN(o) (seen[o-'a'])
#define SEEN_SIZE ('z' - 'a' + 10)
char seen[SEEN_SIZE];			/* upper and lower case */

/* prototypes */
void inflate(void);
static void usage(const char *command);

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      decompress stop finishes the decompress operation by writing out the
**	decompressed file.
**
**  FORMAL PARAMETERS:
**
**      pointer to output file descriptor, source buffer, and number of bytes to
**	write.
**
**  RETURN VALUE:
**
**      none
**
**  SIDE EFFECTS:
**
**      new file that contains the uncompresssed file
**
**
**--
*/
void decompress_stop (FILE *fdout, UCHAR *out_ptr, int dsize)
{
#ifdef VMS
  int i;
  
  /* flush the output buffer */



    for (i=0; i < dsize / 512; i++) {
	fwrite (&out_ptr[i*512], 512, 1, fdout);
    }
#else
    fwrite (out_ptr, 1, dsize, fdout);
#endif

}


/*	 
**  decompression routines.  This is used as a test vehicle to make sure what is
**  compressed can be expanded properly.
**  
*/	 

decompress_start(FILE *fdin)
{
    int	status = SUCCESS;
    int	count = 0;

    UCHAR *buffer;
    
    count = 0;

    hp= (struct header *)malloc(512);	/* enough for first block */

    /* Read in the decompression header (and then ignore it!) */
    (void)fread (hp, 1, 512, fdin);

    while (hp->ID_rev2 != SIGNATURE_STAMP_REV2 && count < 40)
    {
	(void)fread (hp, 1, 512, fdin);	/* read the second block */
	
    }
    if (count >= 40)
    {
	status = FAILURE;
    }	

    if (status == SUCCESS)
    {
	/* get enough space for the entire decompressed file + first block  */

    	buffer = (UCHAR *)malloc(512 + ((hp->csize +511)&~511));

	memcpy (buffer,hp,512);
	free(hp);
	hp = (struct header *)buffer;
	inptr = buffer;
	compressedSize = hp->csize;

	decompressedSize = 0;

    }
    return(status);
}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      The routine will set the code up for the rest of the decompression code.
**	
**
**  FORMAL PARAMETERS:
**
**      in:
**          input file decriptor
**       
**      out:
**          output file descriptor
**
**  RETURN VALUE:
**
**      always 1
**
**  SIDE EFFECTS:
**
**      new file is created that has the expanded code
**
**
**--
*/
int decompress (FILE *in, FILE *out)
{

    /* adjust the input buffer to the start of the second block */

    inptr = &inptr[512];
    
    if (fstat(fileno(in), &statbuf)) {
        fprintf(stderr,"Couldn't stat input file\n");
        return FALSE;
    }

    if (statbuf.st_size == 0) {
      fprintf(stderr, "Input file of zero bytes can not be compressed\n");
      return FALSE;
    }

    outptr = decompressed = (UCHAR *)malloc(statbuf.st_size * 4);

    /* don't forget we alreay read the first block */
    fread(inptr, statbuf.st_size - 512, sizeof(UCHAR), fin);

    inptr = (UCHAR *)hp;
    inptr = &inptr[sizeof(struct header)];
    
    inflate();

    decompressedSize = ((long)(outptr - decompressed) + 511) & ~511;
    return 1;
}



/*----------*/
void compress_stop (void) {
#ifdef VMS
  int i;
#endif
  int stacksize;


  /* Update the header */
  hp->ID_rev2 = SIGNATURE_STAMP_REV2;
  hp->ID_stamp = SIGNATURE_STAMP;
  hp->csize = size;
  hp->dsize = statbuf.st_size;
  hp->stack = 0;
  stacksize = 0;
  hp->stack += stacksize;

  /* flush the output buffer */
#ifdef VMS

    if (prepend_size)
    {
	for (i=0; i < prepend_size/512; i++)
	{
	    fwrite (&prepend_buf[i*512], 512, 1, fout);
	}
    }
    /* now write out the header */
    fwrite((char *)hp, sizeof(struct header), 1, fout);

    /* compensate for header */
    for (i=0; i<(size + 512 + 20) / 512; i++) {
	fwrite (&outptr[i*512], 512, 1, fout);
    }
#else
    if (prepend_size) {
      if (_SEEN('v')) {
	fprintf(stderr, "...prepended image size %d (0x%X) bytes\n",
		prepend_size, prepend_size);
      }
      fwrite (prepend_buf, prepend_size, 1, fout);
    }

    /* now write out the header */
    if (_SEEN('v')) {
      fprintf(stderr, "...compressed image header size %d (0x%X) bytes\n",
	      sizeof(struct header), sizeof(struct header));
    }
    fwrite(hp, sizeof(struct header), 1, fout);

    /* now write out the compressed image */
    if (_SEEN('v')) {
      fprintf(stderr, "...compressed image size %d (0x%X) bytes\n",
	      hp->csize, hp->csize);
    }
    fwrite (outptr, 1, hp->csize, fout);
#endif

  /* print out compression summary */
    compression = (double)hp->csize / (double) hp->dsize;
    compression = (1.0 - compression) * 100.0;

  fprintf (stderr, "%6.2f %%  (%10d bytes) %s\n",
	   compression,
	   hp->csize,
	   infile
	   );


}


int compress (FILE *in, FILE *out)
{

    inptr = (UCHAR *)malloc(statbuf.st_size);
    outptr = (UCHAR *)malloc(statbuf.st_size);

    fread(inptr, statbuf.st_size, sizeof(UCHAR), fin);

    size = memcompress(outptr, (ULONG)statbuf.st_size, 
                       inptr, (ULONG)statbuf.st_size);

    return size;
}


/*----------*/
int main (int argc, char *argv []) {
    char cpu[15] = "21064";	/* Assume EV4 cpu.	*/

    int i;
    char option, *arg;
    unsigned int target;
    char *toolbox;
    char prefile[256];

    if ((toolbox = getenv(TOOLBOX_VARIABLE)) == NULL) {
      fprintf(stderr, "The %s environment variable must be defined\n", TOOLBOX_VARIABLE);
      exit(FAILURE);
    }

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
		    case 'v': 		/* verbose */
			_SEEN(option) = TRUE;
			printf("hfcomp - version (%s)\n", rcsid);
			break;
/*
 *  This is a special one, the -o option is followed (immediately)
 *  by a number, the offset of the code into the ROM.
 */
		    case 't': 
			_SEEN(option) = TRUE;
			sscanf(arg, "%X", &target);
			arg = arg + strlen(arg);
			break;

		    case '2':
			strncpy(cpu, arg-1, 14);
			arg = arg + strlen(arg);
			break;

		    default: 
			fprintf(stderr, "Unknown option \"%s\"\n", arg);
		    case 'h': 
		    case '?': 
			usage(argv[0]);
			exit(SUCCESS);
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
		    exit(FAILURE);
		}
		outfile = arg;
	    }
	}
    }

    if     (strncmp(cpu, "21064", 5) == 0) sprintf(prefile, "%s/decmp64.img", toolbox);
    else if(strncmp(cpu, "21066", 5) == 0) sprintf(prefile, "%s/decmp64.img", toolbox);
    else if(strncmp(cpu, "21164", 5) == 0) sprintf(prefile, "%s/decmp164.img", toolbox);
    else if(strncmp(cpu, "21264", 5) == 0) sprintf(prefile, "%s/decmp264.img", toolbox);
    else
    {
	fprintf(stderr, "Illegall cpu type specified\n");
	usage(argv[0]);
	exit(FAILURE);
    }

    hp = (struct header *)malloc(sizeof(struct header));
    memset(hp, 0, sizeof(struct header));
    hp->target = target;

      prein = fopen (prefile, "rb");
      if (prein == 0) {
	fprintf (stderr, "%s can't open %s for input\n", argv [0], prefile);
	fprintf(stderr, "Check the %s environment variable\n", TOOLBOX_VARIABLE);
	usage(argv[0]);
	exit (FAILURE);
      }

    if (fstat(fileno(prein), &statbuf) != 0) {
        perror("Error:");
        fprintf(stderr,"Couldn't stat input file %s\n", prefile);
        return FALSE;
    }
    if (_SEEN('v')) {
      fprintf(stderr, "Opened decompression library image %s, %d (0x%X) bytes)\n",
	      prefile, statbuf.st_size, statbuf.st_size);
    }

#ifdef _WIN32
    /* Can't use the linker defined _end symbol */
    if (statbuf.st_size > (DECOMP_COMPRESSED - DECOMP_PALBASE)) {
      fprintf(stderr, "The decompression library file %s", prefile);
      fprintf(stderr, " is greater than %d bytes", prepend_size);
      fprintf(stderr, " %s must be rebuilt.\n", argv[0]);
      exit(FAILURE);
    }

    prepend_size = DECOMP_COMPRESSED - DECOMP_PALBASE;
#else
    prepend_size = statbuf.st_size;
#endif

    prepend_buf = malloc(prepend_size);
    memset(prepend_buf, 0, prepend_size);
    fread(prepend_buf, statbuf.st_size, 1, prein);
    fclose(prein);

  /* open up input file */
    if (infile == NULL) {
      fprintf (stderr, "No input file specified\n");
      usage(argv[0]);
      exit(FAILURE);
#if 0
      fin = stdin;
      infile = "stdin";
#endif
    }
    else {
      fin = fopen (infile, "rb");
      if (fin == 0) {
	fprintf (stderr, "%s can't open %s for input\n", argv [0], infile);
	usage(argv[0]);
	exit (FAILURE);
      }
    }

    if (fstat(fileno(fin), &statbuf) != 0) {
        perror("Error:");
        fprintf(stderr,"Couldn't stat input file %s\n", infile);
        return FALSE;
    }
    if (_SEEN('v')) {
      fprintf(stderr, "Opened input image file %s, %d (0x%X) bytes)\n",
	      infile, statbuf.st_size, statbuf.st_size);
    }

  /* open up output file */
    if (outfile == NULL) {
      fprintf (stderr, "No output file specified\n");
      usage(argv[0]);
      exit(FAILURE);
#if 0
      fout = stdout;
      outfile = "stdout";
#endif
    }
    else {
#ifdef VMS
      fout = fopen (outfile, "w", "mrs=512", "rfm=fix");
#else
      fout = fopen (outfile, "wb");
#endif
      if (fout == 0) {
	fprintf (stderr, "%s can't open %s for output\n", argv [0], outfile);
	exit (FAILURE);
      }
    }


    /*
     **
     ** compress file
     **
     */
    
    if (!(compress( fin, fout ) > 0)) {
      fclose(fin);
      fclose(fout);
      printf("\nCouldn't compress file (%s)\n", infile ) ;
      exit (FAILURE);
    }
    
    compress_stop();
    
    fclose(fin);
    fclose(fout);
    
    return SUCCESS ;
  }

static void usage(const char *command)
{
  printf("\nUsage: %s [options] <input-file> <output-file>\n\n", command);
  printf("   -v            Verbose mode on\n");
  printf("   -h            Help, print this text\n");
  printf("   -t            Target location where decompressed image should go.\n");
  printf("   -21064        Generate code for DECChip 21064 (default).\n");
  printf("   -21064A       Generate code for DECChip 21064A (default).\n");
  printf("   -21066        Generate code for DECChip 21066/21068\n");
  printf("   -21164        Generate code for DECChip 21164\n");
  printf("   -21164A       Generate code for DECChip 21164A\n");
  printf("   -21164PC      Generate code for DECChip 21164PC\n");
  printf("\n");
}
