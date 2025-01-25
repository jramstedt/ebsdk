#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

typedef enum _CONVERT_MODE {
    normal = 0,
    pvc
} CONVERT_MODE, *PCONVERT_MODE;

#define USAGE {fprintf(stderr, "USAGE: %s [-pvc] <infile> <outfile>\n", progname);}

char *progname = NULL;

int 
ConvertMapFile(
    char *infilename,
    char *outfilename,
    CONVERT_MODE mode
    )
{
  FILE *infile = NULL;
  FILE *outfile = NULL;
  char buffer[128];
  char *bufptr;
  char label[80];
  unsigned long upper, lower;
  int retval = 0;

  /*
   * do {} while(false); is used as an exception loop
   * Its body is executed exactly once, but if a problem
   * occurs, we can break out and hit common cleanup code
   */
  do {
    if ((infile = fopen(infilename, "r")) == NULL) {
      fprintf(stderr, "ERROR: Couldn't open input file \"%s\".\n", infilename);
      retval = -2;
      break;
    }
  
    if ((outfile = fopen(outfilename, "w")) == NULL) {
      fprintf(stderr, "ERROR: Couldn't open output file \"%s\".\n", outfilename);
      retval = -2;
      break;
    }

    /*
     * Search for "SYMBOL TABLE" which marks the start of the symbol table
     */
    while ((bufptr = fgets(buffer, sizeof(buffer), infile)) != NULL) {
      if (strstr(buffer, "SYMBOL TABLE") != NULL) {
	/*
	 * We found it - eat the next line (------) and finish the loop
	 */
	fgets(buffer, sizeof(buffer), infile);
	break;
      }
    }

    if (bufptr == NULL) {
      /*
       * We did not find "SYMBOL TABLE" before an error or end of file
       * was encountered
       */
      fprintf(stderr, "ERROR: \"SYMBOL TABLE\" not found.\n");
      retval = -3;
      break;
    }

    /*
     * We are at the beginning of the symbol table - start converting
     * input format:
     *    label = [upper lower]
     *             Other information
     * output format:
     *    label lower
     */
    fprintf(stderr, "Symbol table located -- starting conversion...");
    while(fgets(buffer, sizeof(buffer), infile)) {
      switch(mode) {
      case normal:
        if (!isspace(buffer[0]) && (strchr(buffer, '=') != NULL)) {        
          sscanf(buffer, "%s = [%lx %lx]", label, &upper, &lower);
          fprintf(outfile, "%s %08x\n", label, lower);
        }
        break;
      case pvc:
        if (!isspace(buffer[0]) && 
            (strchr(buffer, '=') != NULL) &&
            !strncmp(buffer, "pvc$", strlen("pvc$"))) {        
          sscanf(buffer, "%s = [%lx %lx]", label, &upper, &lower);
          fprintf(outfile, "%s %08x\n", label, lower);
        }
        break;
      }
    }
    fprintf(stderr, "done\n");
  } while(false);

  /*
   * Clean up
   */
  if (infile != NULL) fclose(infile);
  if (outfile != NULL) fclose(outfile);

  return retval;
}

int
main(
    int argc, 
    char **argv
    )
{
  int carg = 1;
  CONVERT_MODE mode = normal;

  progname = *argv;

  if (argc < 3) {
    USAGE;
    return -1;
  }

  if (!strcmp(argv[1], "-pvc")) {
    mode = pvc;
    carg++;
  }

  if (argc - carg != 2) {
    USAGE;
    return -1;
  }

  return ConvertMapFile(argv[carg], argv[carg+1], mode);
}

