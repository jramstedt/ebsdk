/*

 Copyright (C) 1990-1996 Mark Adler, Richard B. Wales, Jean-loup Gailly,
 Kai Uwe Rommel, Onno van der Linden and Igor Mandrichenko.
 Permission is granted to any individual or institution to use, copy, or
 redistribute this software so long as all of the original files are included,
 that it is not sold for profit, and that this copyright notice is retained.

*/

#include <sys/xglob.h>
#include <io.h>

#ifdef ZCRYPT_INTERNAL
#  include <process.h>          /* getpid() declaration for srand seed */
#endif

#define USE_CASE_MAP

#define ROUNDED_TIME(time)  (((time) + 1) & (~1))

#define PROCNAME(n) (action == ADD || action == UPDATE ? wild(n) : procname(n))
#define SSTAT h68_stat
   int h68_stat (char *, struct stat *);
#define OS_CODE  0x300  /* pretend it's Unix */
