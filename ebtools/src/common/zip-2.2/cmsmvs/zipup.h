/*

 Copyright (C) 1990-1996 Mark Adler, Richard B. Wales, Jean-loup Gailly,
 Kai Uwe Rommel, Onno van der Linden, George Petrov and Igor Mandrichenko.
 Permission is granted to any individual or institution to use, copy, or
 redistribute this software so long as all of the original files are included,
 that it is not sold for profit, and that this copyright notice is retained.

*/

#define fhow  "r"    /* maybe recfm=f ? */
#define fhowb "rb"   /* maybe recfm=f ? */

#define fbad NULL
typedef FILE *ftype;
#define zopen(n,p)   (ftype)fopen((n),(p))
#define zread(f,b,n) fread((b),1,(n),(FILE*)(f))
#define zclose(f)    fclose((FILE*)(f))
#define zerr(f)      ferror((FILE*)(f))
#define zstdin       stdin
