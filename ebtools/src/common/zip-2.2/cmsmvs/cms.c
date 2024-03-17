/*

 Copyright (C) 1990-1996 Mark Adler, Richard B. Wales, Jean-loup Gailly,
 Kai Uwe Rommel, Onno van der Linden, George Petrov and Igor Mandrichenko.
 Permission is granted to any individual or institution to use, copy, or
 redistribute this software so long as all of the original files are included,
 that it is not sold for profit, and that this copyright notice is retained.

*/

/*
 * VM/CMS specific things.
 */

#include "zip.h"

int procname(n)
char *n;                /* name to process */
/* Process a name or sh expression to operate on (or exclude).  Return
   an error code in the ZE_ class. */
{
  FILE *stream;

  if (strcmp(n, "-") == 0)   /* if compressing stdin */
    return newname(n, 0);
  else {
     if ((stream = fopen(n, "r")) != (FILE *)NULL)
        {
        fclose(stream);
        return newname(n, 0);
        }
     else return ZE_MISS;
  }
  return ZE_OK;
}
