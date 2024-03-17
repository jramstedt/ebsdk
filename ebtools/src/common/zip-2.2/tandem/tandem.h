/*

 Copyright (C) 1990-1996 Mark Adler, Richard B. Wales, Jean-loup Gailly,
 Kai Uwe Rommel, Onno van der Linden, George Petrov and Igor Mandrichenko.
 Permission is granted to any individual or institution to use, copy, or
 redistribute this software so long as all of the original files are included,
 that it is not sold for profit, and that this copyright notice is retained.

*/

#ifndef __tandem_h   /* prevent multiple inclusions */
#define __tandem_h

#define TANDEM       /* better than __TANDEM */

#define NO_UNISTD_H
#define USE_CASE_MAP


/* Include file for TANDEM */

#ifndef NULL
#  define NULL 0
#endif

#include <time.h>               /* the usual non-BSD time functions */
#include <stdio.h>               /* the usual non-BSD time functions */
#include <sysstat.h>
#include <unistd.h>
#include <errno.h>

#define PASSWD_FROM_STDIN
                  /* Kludge until we know how to open a non-echo tty channel */

#define MAXFILEPARTLEN 8
#define MAXPATHLEN 128
#define EXTENSION_MAX 3
#define NO_RMDIR
#define NO_MKTEMP

#ifdef UNZIP                    /* definitions for UNZIP */
/* #define INBUFSIZ 8192       */
/* #define USE_STRM_INPUT      */
/* #define USE_FWRITE          */
/* #define REALLY_SHORT_SYMS   */
/* #define PATH_MAX 128        */
#endif /* UNZIP */

#define EXIT zexit     /*  To stop creation of Abend files */
#define RETURN zexit   /*  To stop creation of Abend files */
#define fopen zipopen  /*  To allow us to set extent sizes */
#define putc zputc     /*  To allow us to auto flush  */

void zexit (int);

FILE *zipopen(
const char *,
const char *
);

int zputc(
int,
FILE *
);

#define FOPR "rb"
#define FOPM "r+"
#define FOPW "wb"
#define FOPWT "w"

#define CBSZ 0x10000  /* Was used for both fcopy and file_read.        */
                      /* Created separate define (SBSZ) for file_read  */
                      /* fcopy param is type size_t (unsigned long)    */
                      /* For Guardian we choose a multiple of 4K       */

#define ZBSZ 0x10000  /* This is used in call to setvbuf, 64K seems to work  */
                      /* in all memory models. Again it is an unsigned long  */
                      /* For Guardian we choose a multiple of 4K             */

#ifndef __INT32
#define SBSZ 0x04000  /* For STORE method we can use a maximum of int        */
                      /* size.  In Large memory model this equates to 32767  */
                      /* We use a multiple of 4k to match Guardian I/O       */
#else
#define SBSZ 0x10000  /* WIDE model so we can use 64K                        */
#endif /* __INT32 */

/* For deflate.c,  need to look into BI_MEM and DYN_ALLOC defines */

/* <dirent.h> definitions */

#define NAMELEN FILENAME_MAX+1+EXTENSION_MAX   /* allow for space extension */

struct dirent {
   struct dirent *d_next;
   char   d_name[NAMELEN+1];
};

typedef struct _DIR {
   struct  dirent *D_list;
   struct  dirent *D_curpos;
   char            D_path[NAMELEN+1];
} DIR;

DIR *          opendir(const char *dirname);
struct dirent *readdir(DIR *dirp);
void           rewinddir(DIR *dirp);
int            closedir(DIR *dirp);
char *         readd(DIR *dirp);

#define ALIAS_MASK  (unsigned int) 0x80
#define SKIP_MASK   (unsigned int) 0x1F
#define TTRLEN      3
#define RECLEN      254

#define DISK_DEVICE        3
#define SET_FILE_SECURITY  1

#define DOS_EXTENSION      '.'
#define TANDEM_EXTENSION   ' '
#define TANDEM_DELIMITER   '.'
#define TANDEM_NODE        '\\'
#define INTERNAL_DELIMITER '/'
#define INTERNAL_NODE      '//'
#define TANDEM_WILD_1      '*'
#define TANDEM_WILD_2      '?'

#define DOS_EXTENSION_STR      "."
#define TANDEM_EXTENSION_STR   " "
#define TANDEM_DELIMITER_STR   "."
#define TANDEM_NODE_STR        "\\"
#define INTERNAL_DELIMITER_STR "/"
#define INTERNAL_NODE_STR      "//"

typedef struct {
   unsigned short int count;
   char rest[RECLEN];
} RECORD;

char    *endmark = "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF";


#endif /* !__tandem_h */
