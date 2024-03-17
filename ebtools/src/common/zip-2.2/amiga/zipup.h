/*

 Copyright (C) 1990-1996 Mark Adler, Richard B. Wales, Jean-loup Gailly,
 Kai Uwe Rommel, Onno van der Linden, Igor Mandrichenko, Paul Kienitz and
 John Bush.
 Permission is granted to any individual or institution to use, copy, or
 redistribute this software so long as all of the original files are included,
 that it is not sold for profit, and that this copyright notice is retained.

*/

#ifndef __amiga_zipup_h
#define __amiga_zipup_h

#ifndef O_RAW
#  define O_RAW      0
#endif
#define fhow         (O_RDONLY | O_RAW)
#define fbad         (-1)
typedef int          ftype;
#define zopen(n,p)   open(n,p)
#define zread(f,b,n) read(f,b,n)
#define zclose(f)    close(f)
#define zerr(f)      (k == (extent)(-1L))
#define zstdin       0

#endif /* __amiga_zipup_h */

