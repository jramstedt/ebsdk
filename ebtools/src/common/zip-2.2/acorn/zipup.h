/*

 Copyright (C) 1990-1996 Mark Adler, Richard B. Wales, Jean-loup Gailly,
 Kai Uwe Rommel, Onno van der Linden, Sergio Monesi, Karl Davis and
 Igor Mandrichenko.
 Permission is granted to any individual or institution to use, copy, or
 redistribute this software so long as all of the original files are included,
 that it is not sold for profit, and that this copyright notice is retained.

*/

#define fhow "r"
#define fbad (NULL)
typedef FILE *ftype;
#define zopen(n,p) fopen(n,p)
#define zread(f,b,n) fread((b),1,(n),(FILE*)(f))
#define zclose(f) fclose(f)
#define zerr(f) (k==(extent)(-1L))
#define zstdin 0
