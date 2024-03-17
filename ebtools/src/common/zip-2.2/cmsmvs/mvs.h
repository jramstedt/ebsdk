/*

 Copyright (C) 1990-1996 Mark Adler, Richard B. Wales, Jean-loup Gailly,
 Kai Uwe Rommel, Onno van der Linden, George Petrov and Igor Mandrichenko.
 Permission is granted to any individual or institution to use, copy, or
 redistribute this software so long as all of the original files are included,
 that it is not sold for profit, and that this copyright notice is retained.

*/

/* <dirent.h> definitions */

#define NAMELEN     8

struct dirent {
   struct dirent *d_next;
   char   d_name[NAMELEN+1];
};

typedef struct _DIR {
   struct  dirent *D_list;
   struct  dirent *D_curpos;
   char            D_path[FILENAME_MAX];
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

typedef _Packed struct {
   unsigned short int count;
   char rest[RECLEN];
} RECORD;

char    *endmark = "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF";
