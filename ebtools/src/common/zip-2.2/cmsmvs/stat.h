/*

 Copyright (C) 1990-1996 Mark Adler, Richard B. Wales, Jean-loup Gailly,
 Kai Uwe Rommel, Onno van der Linden, George Petrov and Igor Mandrichenko.
 Permission is granted to any individual or institution to use, copy, or
 redistribute this software so long as all of the original files are included,
 that it is not sold for profit, and that this copyright notice is retained.

*/

/*  stat.h

    Definitions used for file status functions

*/

#ifndef __STAT_H
#define __STAT_H

#include <stdio.h>

#define S_IFMT  0xF000  /* file type mask */
#define S_IFDIR 0x4000  /* directory */
#define S_IFIFO 0x1000  /* FIFO special */
#define S_IFCHR 0x2000  /* character special */
#define S_IFBLK 0x3000  /* block special */
#define S_IFREG 0x8000  /* or just 0x0000, regular */
#define S_IREAD 0x0100  /* owner may read */
#define S_IWRITE 0x0080 /* owner may write */
#define S_IEXEC 0x0040  /* owner may execute <directory search> */

struct  stat
{
    short st_dev;      /* Drive number of disk containing the  */
                       /* file or file handle if the file is   */
                       /* on device                            */
    short st_ino;      /* Not meaningfull for VM/CMS           */
    short st_mode;     /* Bit mask giving information about    */
                       /* the file's mode                      */
    short st_nlink;    /* Set to the integer constant 1        */
    int   st_uid;      /* Not meaningfull for VM/CMS           */
    int   st_gid;      /* Not meaningfull for VM/CMS           */
    short st_rdev;     /* Same as st_dev                       */
    long  st_size;     /* Size of the file in bytes            */
    long  st_atime;    /* Most recent access                   */
    long  st_mtime;    /* Same as st_atime                     */
    long  st_ctime;    /* Same as st_atime                     */
    FILE  *fp;
    char  fname[FILENAME_MAX];
};

int stat(const char *path, struct stat *sb);
int fstat(int fd, struct stat *sb);

#endif  /* __STAT_H */
