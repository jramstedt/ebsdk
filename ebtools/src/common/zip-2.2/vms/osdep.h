/*

 Copyright (C) 1990-1997 Mark Adler, Richard B. Wales, Jean-loup Gailly,
 Kai Uwe Rommel, Christian Spieler, Onno van der Linden and Igor Mandrichenko.
 Permission is granted to any individual or institution to use, copy, or
 redistribute this software so long as all of the original files are included,
 that it is not sold for profit, and that this copyright notice is retained.

*/

#ifndef VMS
#  define VMS 1
#endif

#if !(defined(__DECC) || defined(__DECCXX) || defined(__GNUC__))
     /* VAX C does not properly support the void keyword. (Only functions
        are allowed to have the type "void".)  */
#  ifndef NO_TYPEDEF_VOID
#    define NO_TYPEDEF_VOID
#  endif
#  define NO_FCNTL_H        /* VAXC does not supply fcntl.h. */
#endif /* VAX C */

#define NO_UNISTD_H

#define USE_CASE_MAP
#define PROCNAME(n) (action == ADD || action == UPDATE ? wild(n) : procname(n))

#include <types.h>
#include <stat.h>
#include <unixio.h>

#ifdef ZCRYPT_INTERNAL
#  include <unixlib.h>          /* getpid() declaration for srand seed */
#endif

#if !defined(NO_EF_UT_TIME) && !defined(USE_EF_UT_TIME)
#  if (defined(__VMS_VER) && (__VMS_VER >= 70000000))
#    define USE_EF_UT_TIME
#  endif
#endif

#if defined(VMS_PK_EXTRA) && defined(VMS_IM_EXTRA)
#  undef VMS_IM_EXTRA                 /* PK style takes precedence */
#endif
#if !defined(VMS_PK_EXTRA) && !defined(VMS_IM_EXTRA)
#  define VMS_PK_EXTRA 1              /* PK style VMS support is default */
#endif

#define unlink delete
#define NO_SYMLINK
#define SSTAT vms_stat
#define EXIT(exit_code) vms_exit(exit_code)
#define RETURN(exit_code) {vms_exit(exit_code); return 1;}

/* File operations--use "b" for binary if allowed or fixed length 512 on VMS */
#define FOPR  "rb","ctx=stm","mbc=64"
#define FOPM  "r+b","ctx=stm","rfm=fix","mrs=512","mbc=64"
#define FOPW  "wb","ctx=stm","rfm=fix","mrs=512","mbc=64"
