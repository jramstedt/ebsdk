/*

 Copyright (C) 1990-1996 Mark Adler, Richard B. Wales, Jean-loup Gailly,
 Kai Uwe Rommel, Onno van der Linden and Igor Mandrichenko.
 Permission is granted to any individual or institution to use, copy, or
 redistribute this software so long as all of the original files are included,
 that it is not sold for profit, and that this copyright notice is retained.

*/

#define O_RDONLY (0)
#define O_UNCONVERTED     (0400) /* Forced NO conversion requested */
#define fhow (O_RDONLY | O_UNCONVERTED)
#define fbad (-1)
typedef int ftype;
#define zopen(n,p) open(n,p)
#define zread(f,b,n) read(f,b,n)
#define zclose(f) close(f)
#define zerr(f) (k == (extent)(-1L))
#define zstdin 0
