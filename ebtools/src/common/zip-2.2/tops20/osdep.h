/*

 Copyright (C) 1990-1996 Mark Adler, Richard B. Wales, Jean-loup Gailly,
 Kai Uwe Rommel, Onno van der Linden and Igor Mandrichenko.
 Permission is granted to any individual or institution to use, copy, or
 redistribute this software so long as all of the original files are included,
 that it is not sold for profit, and that this copyright notice is retained.

*/

#ifndef TOPS20
#define TOPS20
#endif

#define NO_PROTO
#define NO_SYMLINK
#define NO_TERMIO
#define DIRENT
#define BIG_MEM
#define REALLY_SHORT_SYMS
#define window_size winsiz

extern int isatty();

#define FOPR "rb"
#define FOPM "r+b"
#define FOPW "w8"

#define CBSZ 524288
#define ZBSZ 524288

#include <sys/types.h>
#include <sys/stat.h>
