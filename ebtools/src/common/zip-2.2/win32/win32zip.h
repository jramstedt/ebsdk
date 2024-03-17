/*

 Copyright (C) 1990-1997 Mark Adler, Richard B. Wales, Jean-loup Gailly,
 Kai Uwe Rommel, Onno van der Linden and Igor Mandrichenko.
 Permission is granted to any individual or institution to use, copy, or
 redistribute this software so long as all of the original files are included,
 that it is not sold for profit, and that this copyright notice is retained.

*/

/*
 * NT specific functions for ZIP.
 */

int GetFileMode(char *name);
long GetTheFileTime(char *name, iztimes *z_times);

int IsFileNameValid(char *name);
int IsFileSystemOldFAT(char *dir);
void ChangeNameForFAT(char *name);

char *getVolumeLabel(int drive, ulg *vtime, ulg *vmode, time_t *vutim);

#if 0 /* never used ?? */
char *StringLower(char *);
#endif

char *GetLongPathEA(char *name);
