/*

 Copyright (C) 1990-1997 Mark Adler, Richard B. Wales, Jean-loup Gailly,
 Kai Uwe Rommel, Christian Spieler, Onno van der Linden and Igor Mandrichenko.
 Permission is granted to any individual or institution to use, copy, or
 redistribute this software so long as all of the original files are included,
 that it is not sold for profit, and that this copyright notice is retained.

*/

#define fhow "r","mbc=60"
#define fbad NULL
typedef void *ftype;
#define zopen(n,p)   (vms_native?vms_open(n)    :(ftype)fopen((n), p))
#define zread(f,b,n) (vms_native?vms_read(f,b,n):fread((b),1,(n),(FILE*)(f)))
#define zclose(f)    (vms_native?vms_close(f)   :fclose((FILE*)(f)))
#define zerr(f)      (vms_native?vms_error(f)   :ferror((FILE*)(f)))
#define zstdin stdin

ftype vms_open OF((char *));
int vms_read OF((ftype, char *, int));
int vms_close OF((ftype));
int vms_error OF((ftype));
void vms_get_attributes OF((ftype, struct zlist far *, iztimes *f_utim));
