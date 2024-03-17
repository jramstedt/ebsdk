/* tailor.h -- Not copyrighted 1993 Mark Adler */

#ifdef AMIGA
#include "amiga/osdep.h"
#endif

#ifdef AOSVS
#include "aosvs/osdep.h"
#endif

#ifdef ATARI
#include "atari/osdep.h"
#endif

#ifdef __BEOS__
#include "beos/osdep.h"
#endif

#ifdef DOS
#include "msdos/osdep.h"
#endif

#ifdef __human68k__
#include "human68k/osdep.h"
#endif

#ifdef OS2
#include "os2/osdep.h"
#endif

#ifdef __riscos
#include "acorn/osdep.h"
#endif

#ifdef QDOS
#include "qdos/osdep.h"
#endif

#ifdef __TANDEM
#include "tandem.h"
#endif

#ifdef UNIX
#include "unix/osdep.h"
#endif

#if defined(__COMPILER_KCC__) || defined(TOPS20)
#include "tops20/osdep.h"
#endif

#if defined(VMS) || defined(__VMS)
#include "vms/osdep.h"
#endif

#if defined(VM_CMS) || defined(MVS)
#include "cmsmvs.h"
#endif

#ifdef WIN32
#include "win32/osdep.h"
#endif

/* When "void" is an alias for "int", prototypes cannot be used. */
#if (defined(NO_VOID) && !defined(NO_PROTO))
#  define NO_PROTO
#endif

/* Used to remove arguments in function prototypes for non-ANSI C */
#ifndef NO_PROTO
#  define OF(a) a
#else /* NO_PROTO */
#  define OF(a) ()
#endif /* ?NO_PROTO */

/* Avoid using const if compiler does not support it */
#if (!defined(ZCONST) && (!defined(NO_CONST) || defined(USE_CONST)))
#  define ZCONST const
#endif

#ifndef ZCONST
#  define ZCONST
#endif

/*
 * case mapping functions. case_map is used to ignore case in comparisons,
 * to_up is used to force upper case even on Unix (for dosify option).
 */
#ifdef USE_CASE_MAP
#  define case_map(c) upper[(c) & 0xff]
#  define to_up(c)    upper[(c) & 0xff]
#else
#  define case_map(c) (c)
#  define to_up(c)    ((c) >= 'a' && (c) <= 'z' ? (c)-'a'+'A' : (c))
#endif /* USE_CASE_MAP */

/* Define void, zvoid, and extent (size_t) */
#include <stdio.h>

#ifndef NO_STDDEF_H
#  include <stddef.h>
#endif /* !NO_STDDEF_H */

#ifndef NO_STDLIB_H
#  include <stdlib.h>
#endif /* !NO_STDLIB_H */

#ifndef NO_UNISTD_H
#  include <unistd.h> /* usually defines _POSIX_VERSION */
#endif /* !NO_UNISTD_H */

#ifndef NO_FCNTL_H
#  include <fcntl.h>
#endif /* !NO_FNCTL_H */

#ifndef NO_STRING_H
#  include <string.h>
#else
#  include <strings.h>
#endif /* NO_STRING_H */

#ifdef NO_VOID
#  define void int
   typedef char zvoid;
#else /* !NO_VOID */
# ifdef NO_TYPEDEF_VOID
#  define zvoid void
# else
   typedef void zvoid;
# endif
#endif /* ?NO_VOID */

#ifdef NO_STRRCHR
#  define strrchr rindex
#endif

#ifdef NO_STRCHR
#  define strchr index
#endif

/*
 * A couple of forward declarations that are needed on systems that do
 * not supply C runtime library prototypes.
 */
#ifdef NO_PROTO
char *strcpy();
char *strcat();
char *strrchr();
/* XXX use !defined(ZMEM) && !defined(__hpux__) ? */
#if !defined(ZMEM) && defined(NO_STRING_H)
char *memset();
char *memcpy();
#endif /* !ZMEM && NO_STRING_H */

/* XXX use !defined(__hpux__) ? */
#ifdef NO_STDLIB_H
char *calloc();
char *malloc();
char *getenv();
long atol();
#endif /* NO_STDLIB_H */

#endif /* NO_PROTO */

#ifndef NO_MKTEMP
char *mktemp();
#endif /* !NO_MKTEMP */

/*
 * SEEK_* macros, should be defined in stdio.h
 */
/* Define fseek() commands */
#ifndef SEEK_SET
#  define SEEK_SET 0
#endif /* !SEEK_SET */

#ifndef SEEK_CUR
#  define SEEK_CUR 1
#endif /* !SEEK_CUR */

#ifndef FALSE
#  define FALSE 0
#endif

#ifndef TRUE
#  define TRUE 1
#endif

#ifdef NO_SIZE_T
   typedef unsigned int extent;
#else
   typedef size_t extent;
#endif

#ifdef NO_TIME_T
   typedef long time_t;
#endif

/* System independent replacement for "struct utimbuf", which is missing
 * in many older OS environments.
 */
typedef struct ztimbuf {
    time_t actime;              /* new access time */
    time_t modtime;             /* new modification time */
} ztimbuf;

/* This macro round a time_t value to the OS specific resolution */
#ifndef ROUNDED_TIME
#  define ROUNDED_TIME(time)   (time)
#endif

/* Some systems define S_IFLNK but do not support symbolic links */
#if defined (S_IFLNK) && defined(NO_SYMLINK)
#  undef S_IFLNK
#endif

#ifndef FOPR    /* fallback default definitions for FOPR, FOPM, FOPW: */
#  define FOPR "r"
#  define FOPM "r+"
#  define FOPW "w"
#endif /* fallback definition */

/* Open the old zip file in exclusive mode if possible (to avoid adding
 * zip file to itself).
 */
#ifdef OS2
#  define FOPR_EX FOPM
#else
#  define FOPR_EX FOPR
#endif


/* MDOS file attribute for directories */
#define MSDOS_DIR_ATTR 0x10


/* Define this symbol if your target allows access to unaligned data.
 * This is not mandatory, just a speed optimization. The compressed
 * output is strictly identical.
 */
#if (defined(MSDOS) && !defined(WIN32)) || defined(i386)
#    define UNALIGNED_OK
#endif
#if defined(mc68020) || defined(vax)
#    define UNALIGNED_OK
#endif

#ifdef SMALL_MEM
#   define CBSZ 2048 /* buffer size for copying files */
#   define ZBSZ 2048 /* buffer size for temporary zip file */
#endif

#ifdef MEDIUM_MEM
#  define CBSZ 8192
#  define ZBSZ 8192
#endif

#ifndef CBSZ
#  define CBSZ 16384
#  define ZBSZ 16384
#endif

#ifndef MEMORY16
#  ifdef __WATCOMC__
#    undef huge
#    undef far
#    undef near
#  endif
#  ifndef __IBMC__
#    define huge
#    define far
#    define near
#  endif
#  define nearmalloc malloc
#  define nearfree free
#  define farmalloc malloc
#  define farfree free
#endif /* !MEMORY16 */

#if (defined(BIG_MEM) || defined(MMAP)) && !defined(DYN_ALLOC)
#   define DYN_ALLOC
#endif

#ifndef SSTAT
#  define SSTAT      stat
#endif
#ifdef S_IFLNK
#  define LSTAT      lstat
#  define LSSTAT(n, s)  (linkput ? lstat((n), (s)) : SSTAT((n), (s)))
#else
#  define LSTAT      SSTAT
#  define LSSTAT     SSTAT
#endif

/* The following default definition of the second input for the crypthead()
 * random seed computation can be used on most systems (all those that
 * supply a UNIX compatible getpid() function).
 */
#ifdef ZCRYPT_INTERNAL
#  ifndef ZCR_SEED2
#    define ZCR_SEED2     (unsigned) getpid()   /* use PID as seed pattern */
#  endif
#endif /* ZCRYPT_INTERNAL */

/* The following OS codes are defined in pkzip appnote.txt */
#ifdef AMIGA
#  define OS_CODE  0x100
#endif
#ifdef VMS
#  define OS_CODE  0x200
#endif
/* unix    3 */
#ifdef VM_CMS
#  define OS_CODE  0x400
#endif
#ifdef ATARI
#  define OS_CODE  0x500
#endif
#ifdef OS2
#  define OS_CODE  0x600
#endif
#ifdef MACOS
#  define OS_CODE  0x700
#endif
/* z system 8 */
/* cp/m     9 */
#ifdef TOPS20
#  define OS_CODE  0xa00
#endif
#ifdef WIN32
#  define OS_CODE  0xb00
#endif
#ifdef QDOS
#  define OS_CODE  0xc00
#endif
#ifdef RISCOS
#  define OS_CODE  0xd00
#endif
#ifdef VFAT
#  define OS_CODE  0xe00
#endif
#ifdef MVS
#  define OS_CODE  0xf00
#endif
#ifdef __BEOS__
#  define OS_CODE  0x1000
#endif
#ifdef TANDEM
#  define OS_CODE  0x1100
#endif

#define NUM_HOSTS 18
/* Number of operating systems. Should be updated when new ports are made */

#if defined(DOS) && !defined(OS_CODE)
#  define OS_CODE  0x000
#endif

#ifndef OS_CODE
#  define OS_CODE  0x300  /* assume Unix */
#endif

/* can't use "return 0" from main() on VMS */
#ifndef EXIT
#  define EXIT  exit
#endif
#ifndef RETURN
#  define RETURN return
#endif

#ifndef ZIPERR
#  define ZIPERR ziperr
#endif

#ifndef MY_ZCALLOC /* Any system without a special calloc function */
#  define zcalloc(items,size) \
          (zvoid far *)calloc((unsigned)(items), (unsigned)(size))
#  define zcfree    free
#endif /* !MY_ZCALLOC */

/* end of tailor.h */
