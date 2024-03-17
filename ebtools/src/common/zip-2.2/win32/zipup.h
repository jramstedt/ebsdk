/*

 Copyright (C) 1990-1996 Mark Adler, Richard B. Wales, Jean-loup Gailly,
 Kai Uwe Rommel, Onno van der Linden and Igor Mandrichenko.
 Permission is granted to any individual or institution to use, copy, or
 redistribute this software so long as all of the original files are included,
 that it is not sold for profit, and that this copyright notice is retained.

*/

#include <share.h>
#ifndef O_RDONLY
#  define O_RDONLY   0
#endif
#ifndef O_BINARY
#  define O_BINARY   0
#endif
#define fhow         (O_RDONLY|O_BINARY)
#define fbad         (-1)
typedef int          ftype;
#if defined(__WATCOMC__) || defined(__BORLANDC__) || defined(__EMX__)
#  define zopen(n,p) sopen(n,p,SH_DENYNO)
#else
#  define zopen(n,p) _sopen(n,p,_SH_DENYNO)
#endif
#define zread(f,b,n) read(f,b,n)
#define zclose(f)    close(f)
#define zerr(f)      (k == (extent)(-1L))
#define zstdin       0
