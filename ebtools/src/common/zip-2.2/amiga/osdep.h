/*

 Copyright (C) 1990-1996 Mark Adler, Richard B. Wales, Jean-loup Gailly,
 Kai Uwe Rommel, Onno van der Linden, Igor Mandrichenko, Paul Kienitz and
 John Bush.
 Permission is granted to any individual or institution to use, copy, or
 redistribute this software so long as all of the original files are included,
 that it is not sold for profit, and that this copyright notice is retained.

*/

/* default to MEDIUM_MEM, but allow makefile override */
#if ( (!defined(BIG_MEM)) && (!defined(SMALL_MEM)))
#  define MEDIUM_MEM
#endif

#define USE_CASE_MAP
#define USE_EF_UT_TIME
#define HANDLE_AMIGA_SFX
#define PROCNAME(n) (action == ADD || action == UPDATE ? wild(n) : procname(n))
#define EXIT(e) ClearIOErr_exit(e)
void ClearIOErr_exit(int e);

#ifdef __SASC
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <exec/execbase.h>
#  if (defined(_M68020) && (!defined(__USE_SYSBASE)))
                            /* on 68020 or higher processors it is faster   */
#    define __USE_SYSBASE   /* to use the pragma libcall instead of syscall */
#  endif                    /* to access functions of the exec.library      */
#  include <proto/exec.h>   /* see SAS/C manual:part 2,chapter 2,pages 6-7  */
#  include <proto/dos.h>
#  if (defined(_M68020) && !defined(UNALIGNED_OK))
#     define UNALIGNED_OK
#  endif
#  ifndef REENTRANT
#    define REENTRANT
#  endif
#  if (defined(_NEAR_DATA) && !defined(DYN_ALLOC))
#    define DYN_ALLOC
#  endif
#  ifdef DEBUG
#    include <sprof.h>      /* profiler header file */
#  endif
   /* define USE_TIME_LIB if replacement functions of time_lib are available */
   /* replaced are: tzset(), time(), localtime() and gmtime()                */
#  define USE_TIME_LIB

/*
 A word on short-integers and SAS/C (a bug of [mc]alloc?)
 Using short integers (i.e. compiling with option SHORT-INTEGERS) is
 *not* recommended. To get maximum compression ratio the window size stored
 in WSIZE should be 32k (0x8000). However, since the size of the window[]
 array is 2*WSIZE, 65536 bytes must be allocated. The calloc function can
 only allocate UINT_MAX (defined in limits.h) bytes which is one byte short
 (65535) of the maximum window size if you are compiling with short-ints.
 You'll get an error message "Out of memory (window allocation)" whenever
 you try to deflate. Note that the compiler won't produce any warning.
 The maximum window size with short-integers is therefore 32768 bytes.
 The following is only implemented to allow the use of short-integers but
 it is once again not recommended because of a loss in compression ratio.
*/
#  if (defined(_SHORTINT) && !defined(WSIZE))
#    define WSIZE 0x4000        /* only half of maximum window size */
#  endif                        /* possible with short-integers     */
#endif /* __SASC */

#ifdef AZTEC_C
#  define NO_UNISTD_H
#  define NO_RMDIR
#  define BROKEN_FSEEK
#  define USE_TIME_LIB
#  include "amiga/z-stat.h"
#endif

#ifdef USE_TIME_LIB
extern int real_timezone_is_set;
#  define VALID_TIMEZONE(tempvar) (tzset(), real_timezone_is_set)
#else
#  define VALID_TIMEZONE(tempvar) ((tempvar = getenv("TZ")) && tempvar[0])
#endif

#ifdef ZCRYPT_INTERNAL
#  ifndef CLIB_EXEC_PROTOS_H
     void *FindTask(void *);
#  endif
#  define ZCR_SEED2     (unsigned)(ulg)FindTask(NULL)
#endif

char *GetComment(char *);

#define FOPR "rb"
#define FOPM "rb+"
#define FOPW "wb"
/* prototype for ctrl-C trap function */
void _abort(void);
