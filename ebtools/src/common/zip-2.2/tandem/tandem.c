/*

 Copyright (C) 1990-1996 Mark Adler, Richard B. Wales, Jean-loup Gailly,
 Kai Uwe Rommel, Onno van der Linden,zGeorge Petrov and Igor Mandrichenko.
 Permission is granted to any individual or institution to use, copy, or
 redistribute this software so long as all of the original files are included,
 that it is not sold for profit, and that this copyright notice is retained.

*/

/*
 * routines common to TANDEM
 */

#include "zip.h"

#include <time.h>

#define MATCH shmatch


#define DISK_DEVICE        3


/* Library functions not in (most) header files */

#ifndef UTIL    /* the companion #endif is a bit of ways down ... */

int isatty (fnum)
int fnum;
{
  return -1;
}


extern char *label;
local ulg label_time = 0;
local ulg label_mode = 0;
local time_t label_utim = 0;

#include <tal.h>
#include <cextdecs(FILE_GETINFOLISTBYNAME_, \
                   FILENAME_SCAN_,          \
                   INTERPRETTIMESTAMP       \
                  )>
#include <cextdecs(FILENAME_FINDSTART_, \
                   FILENAME_FINDNEXT_,  \
                   FILENAME_FINDFINISH_ \
                  )>
#include <cextdecs(SETMODE)>

/* TANDEM version of chmod() function */

int chmod(file, unix_sec)
const char *file;
mode_t unix_sec;
{
FILE *stream;

struct nsk_sec_type
{
  unsigned progid : 1;
  unsigned clear  : 1;
  unsigned null   : 2;
  unsigned read   : 3;
  unsigned write  : 3;
  unsigned execute: 3;
  unsigned purge  : 3;
};

union nsk_sec_ov
{
  struct nsk_sec_type bit_ov;
  short int_ov;
};

union nsk_sec_ov nsk_sec;

short fnum, fdes, err, nsk_sec_int;

  nsk_sec.bit_ov.progid = 0;
  nsk_sec.bit_ov.clear  = 0;
  nsk_sec.bit_ov.null   = 0;

  /*  4="N", 5="C", 6="U", 7="-"   */

  err = unix_sec & S_IROTH;

  if (unix_sec & S_IROTH) nsk_sec.bit_ov.read = 4;
  else if (unix_sec & S_IRGRP) nsk_sec.bit_ov.read = 5;
  else if (unix_sec & S_IRUSR) nsk_sec.bit_ov.read = 6;
  else nsk_sec.bit_ov.read = 7;

  if (unix_sec & S_IWOTH) nsk_sec.bit_ov.write = 4;
  else if (unix_sec & S_IWGRP) nsk_sec.bit_ov.write = 5;
  else if (unix_sec & S_IWUSR) nsk_sec.bit_ov.write = 6;
  else nsk_sec.bit_ov.write = 7;

  if (unix_sec & S_IXOTH) nsk_sec.bit_ov.execute = 4;
  else if (unix_sec & S_IXGRP) nsk_sec.bit_ov.execute = 5;
  else if (unix_sec & S_IXUSR) nsk_sec.bit_ov.execute = 6;
  else nsk_sec.bit_ov.execute = 7;

  nsk_sec.bit_ov.purge = nsk_sec.bit_ov.write;

  nsk_sec_int = nsk_sec.int_ov;

  fdes = open(file, (O_EXCLUSIVE | O_RDONLY));
  fnum = fdtogfn (fdes);
  err = SETMODE (fnum, SET_FILE_SECURITY, nsk_sec_int);
  err = close(fdes);

}



/* TANDEM version of stat() funcion */

time_t gmt_to_time_t (long long *);

time_t gmt_to_time_t (gmt)
  long long *gmt;
{
  struct tm temp_tm;
  short  date_time[8];
  long   julian_dayno;

  julian_dayno = INTERPRETTIMESTAMP (*gmt, date_time);

  temp_tm.tm_sec   = date_time[5];
  temp_tm.tm_min   = date_time[4];
  temp_tm.tm_hour  = date_time[3];
  temp_tm.tm_mday  = date_time[2];
  temp_tm.tm_mon   = date_time[1] - 1;     /* C's so sad */
  temp_tm.tm_year  = date_time[0] - 1900;  /* it's almost funny */
  temp_tm.tm_isdst = -1;  /* don't know */

  return (mktime(&temp_tm));
}

void zexit(status)
int status;
{
  terminate_program (0,0,status,,,);   /* Exit(>0) creates saveabend files */
}


#ifdef fopen
  #undef fopen
#endif

FILE *zipopen(fname, opt)
const char *fname;
const char *opt;
{
  int fdesc;

  if (strcmp(opt,FOPW) == 0)
    if ((fdesc = creat(fname,,100,500)) != -1)
      close(fdesc);

  return fopen(fname,opt);
}
#define fopen zipopen

#ifdef putc
  #undef putc
#endif

int zputc(ch, fptr)
int ch;
FILE *fptr;
{
  int err;
  err = putc(ch,fptr);
  fflush(fptr);
  return err;
}
#define putc zputc

int utime OF((char *, ztimbuf *));

int utime(file, time)
char *file;
ztimbuf *time;
{
  return 0;
}


short parsename(
const char *,
char *,
char *
);

short parsename(srce, fname, ext)
const char *srce;
char *fname;
char *ext;
{
  /* As a way of supporting DOS extensions from Tandem we look for a space
     separated extension string after the Guardian filename
     e.g. ZIP ZIPFILE "$DATA4.TESTING.INVOICE TXT"
  */

  char *fstart;
  char *fptr;
  short extension = 0;

  *fname = *ext = '\0';  /* set to null string */

  fstart = (char *) srce;

  if ((fptr = strrchr(fstart, TANDEM_EXTENSION)) != NULL) {
    extension = 1;

    fptr++;
    strncat(ext, fptr, _min(EXTENSION_MAX, strlen(fptr)));

    fptr = strchr(fstart, TANDEM_EXTENSION);  /* End of filename */
    strncat(fname, fstart, _min(FILENAME_MAX, (fptr - fstart)));
  }
  else {
    /* just copy string */
    strncat(fname, srce, _min(FILENAME_MAX, strlen(srce)));
  }

  return extension;
}

int stat(n, s)
const char *n;
struct stat *s;
{
  #define list_items 14
  #define rlist_size 200

  short err, i, extension;
  char fname[FILENAME_MAX + 1];
  short fnamelen;
  char ext[EXTENSION_MAX + 1];
                        /* #0  #1  #2  #3 #4 #5 #6 #7 #8 #9 #10 #11 #12 #13 */
  short ilist[list_items]={62,117,145,142,58,41,42,30,31,75, 78, 79, 60,119};
  short ilen[list_items] ={ 2,  4,  4,  2, 1, 1, 1, 1, 1, 1,  1,  1,  1,  4};
  short ioff[list_items];
  short rlist[rlist_size];
  short extra[2];
  short *rlen=&extra[0];
  short *err_item=&extra[1];
  unsigned short *fowner;
  unsigned short *fprogid;
  char *fsec;

  short end, count, kind, level, options, searchid;
  short info[5];

  /* Initialise stat structure */
  s->st_dev = _S_GUARDIANOBJECT;
  s->st_ino = 0;
  s->st_nlink = 0;
  s->st_rdev = 0;
  s->st_uid = s->st_gid = 0;
  s->st_size = 0;
  s->st_atime = s->st_ctime = s->st_mtime = 0;
  s->st_reserved[0] = 0;

  /* Check to see if name contains a (pseudo) file extension */
  extension = parsename (n,fname,ext);

  fnamelen = strlen(fname);

  options = 3; /* Allow Subvols and Templates */
  err = FILENAME_SCAN_( fname,
                        fnamelen,
                        &count,
                        &kind,
                        &level,
                        options
                      );

  if (err != 0 || kind == 2) return -1;

  if (kind == 1 || level < 2) {
    /* Pattern, Subvol Name or One part Filename - lets see if it exists */
    err = FILENAME_FINDSTART_ ( &searchid,
                                fname,
                                fnamelen,
                                ,
                                DISK_DEVICE
                              );

    if (err != 0) {
      end = FILENAME_FINDFINISH_ ( searchid );
      return -1;
    }

    err = FILENAME_FINDNEXT_ ( searchid,
                               fname,
                               FILENAME_MAX,
                               &fnamelen,
                               info
                              );
    end = FILENAME_FINDFINISH_ ( searchid );

    if (err != 0)
      return -1;  /* Non existing template, subvol or file */

    if (kind == 1 || info[2] == -1) {
      s->st_mode = S_IFDIR;    /* Its an existing template or directory */
      return 0;
    }

    /* Must be a real file so drop to code below to get info on it */
  }

  err = FILE_GETINFOLISTBYNAME_( fname,
                                 fnamelen,
                                 ilist,
                                 list_items,
                                 rlist,
                                 rlist_size,
                                 rlen,
                                 err_item
                               );

  if (err != 0) return -1;

  ioff[0] = 0;

  /*  Build up table of offets into result list */
  for (i=1; i < list_items; i++)
    ioff[i] = ioff[i-1] + ilen[i-1];


  /* Setup timestamps */
  s->st_atime = gmt_to_time_t ((long long *)&rlist[ioff[1]]);
  s->st_mtime = s->st_ctime = gmt_to_time_t ((long long *)&rlist[ioff[2]]);
  s->st_reserved[0] = (int64_t) gmt_to_time_t ((long long *)&rlist[ioff[13]]);

  s->st_size = *(off_t *)&rlist[ioff[3]];

  fowner = (unsigned short *)&rlist[ioff[4]];
  s->st_uid = *fowner & 0x00ff;
  s->st_gid = *fowner >> 8;

  /* Note that Purge security (fsec[3]) in NSK has no relevance to stat() */
  fsec = (char *)&rlist[ioff[0]];
  fprogid = (unsigned short *)&rlist[ioff[12]];

  s->st_mode = S_IFREG |  /* Regular File */
  /*  Parse Read Flag */
               ((fsec[0] & 0x03) == 0x00 ? S_IROTH : 0) |
               ((fsec[0] & 0x02) == 0x00 ? S_IRGRP : 0) |
               ((fsec[0] & 0x03) != 0x03 ? S_IRUSR : 0) |
  /*  Parse Write Flag */
               ((fsec[1] & 0x03) == 0x00 ? S_IWOTH : 0) |
               ((fsec[1] & 0x02) == 0x00 ? S_IWGRP : 0) |
               ((fsec[1] & 0x03) != 0x03 ? S_IWUSR : 0) |
  /*  Parse Execute Flag */
               ((fsec[2] & 0x03) == 0x00 ? S_IXOTH : 0) |
               ((fsec[2] & 0x02) == 0x00 ? S_IXGRP : 0) |
               ((fsec[2] & 0x03) != 0x03 ? S_IXUSR : 0) |
  /*  Parse Progid */
               (*fprogid == 1 ? (S_ISUID | S_ISGID) : 0) ;

  return 0;
}

char *ex2in(x, isdir, pdosflag)
char *x;                /* external file name */
int isdir;              /* input: x is a directory */
int *pdosflag;          /* output: force MSDOS file attributes? */
/* Convert the external file name to a zip file name, returning the malloc'ed
   string or NULL if not enough memory. */
{
  char *n;              /* internal file name (malloc'ed) */
  char *t;              /* shortened name */
  int dosflag;
  char *p;               /* pointer to temp area */
  char fname[FILENAME_MAX + 1]= ""; /* file name */
  char ext[EXTENSION_MAX + 1] = ""; /* extension name */
  short extension;    /* does the filename contain an extension */

  dosflag = dosify;  /* default for non-DOS non-OS/2 */

  /* Find starting point in name before doing malloc */
  t = x;

  /* Make changes, if any, to the copied name (leave original intact) */

  if (!pathput)
    t = last(t, TANDEM_DELIMITER);

  /* Malloc space for internal name and copy it */
  if ((n = malloc(strlen(t) + 4)) == NULL) /* + 4 for safety */
    return NULL;

  extension = parsename(t,fname,ext);
  t = fname;

  *n= '\0';

  while (*t != '\0') {  /* File part could be sys,vol,subvol or file */
    if (*t == TANDEM_NODE) {    /* System Name */
      strcat(n, INTERNAL_NODE_STR);
      t++;
    }
    else if (*t == TANDEM_DELIMITER) {  /* Volume or Subvol */
           strcat(n, INTERNAL_DELIMITER_STR);
           t++;
         };
    p = strchr(t,TANDEM_DELIMITER);
    if (p == NULL) break;
    strncat(n,t,(p - t));
    t = p;
  }

  strcat(n,t);  /* mop up any left over characters */

  if (extension) {
    strcat(n,DOS_EXTENSION_STR);
    strcat(n,ext);
  };

  if (isdir == 42) return n;      /* avoid warning on unused variable */

  if (dosify)
    msname(n);

  /* Returned malloc'ed name */
  if (pdosflag)
    *pdosflag = dosflag;

  return n;
}


char *in2ex(n)
char *n;                /* internal file name */
/* Convert the zip file name to an external file name, returning the malloc'ed
   string or NULL if not enough memory. */
{
  char *x;              /* external file name */
  char *t;              /* pointer to internal */
  char *p;              /* pointer to internal */
  char *e;              /* pointer to internal */

  int len;

  if ((x = malloc(strlen(n) + 4)) == NULL)  /* + 4 for safety */
    return NULL;

  t = n;

  *x= '\0';

  if (!pathput)
    t = last(t, INTERNAL_DELIMITER);

  while (*t != '\0') {  /* File part could be sys,vol,subvol or file */
    if (*t == INTERNAL_DELIMITER) {    /* System, Volume or Subvol Name */
      t++;
      if (*t == INTERNAL_DELIMITER) {  /* System */
        strcat(x, TANDEM_NODE_STR);
        t++;
      }
      else
        strcat(x, TANDEM_DELIMITER_STR);
    }
    p = strchr(t,INTERNAL_DELIMITER);
    if (p == NULL) break;
    if ((e = strchr(t,DOS_EXTENSION)) == NULL)
      e = p;
    else
      e = (e < p ? e : p);
    len = _min (MAXFILEPARTLEN, (e - t));
    strncat(x,t,(e - t));
    t = p;
  }

  if ((e = strchr(t,DOS_EXTENSION)) == NULL)
    strcat(x,t);
  else
    strncat(x,t,(e - t));

  return x;
}

void stamp(f, d)
char *f;                /* name of file to change */
ulg d;                  /* dos-style time to change it to */
/* Set last updated and accessed time of file f to the DOS time d. */
{
  ztimbuf u;            /* argument for utime() */

  /* Convert DOS time to time_t format in u.actime and u.modtime */
  u.actime = u.modtime = dos2unixtime(d);

  utime(f, &u);
}

ulg filetime(f, a, n, t)
char *f;                /* name of file to get info on */
ulg *a;                 /* return value: file attributes */
long *n;                /* return value: file size */
iztimes *t;             /* return value: access and modification time */
{
  struct stat s;
  char fname[FILENAME_MAX + 1];

  if (strcmp(f, "-") == 0) {    /* if compressing stdin */
    if (n != NULL) {
      *n = -1L;
    }
  }

  strcpy(fname, f);

  if (stat(fname, &s) != 0) return 0;

  if (a!= NULL) {
    *a = ((ulg)s.st_mode << 16) | !(s.st_mode & S_IWUSR);
    if ((s.st_mode & S_IFMT) == S_IFDIR) {
      *a |= MSDOS_DIR_ATTR;
    }
  }

  if (n!= NULL)
    *n = (s.st_mode & S_IFMT) == S_IFREG ? s.st_size : -1L;

  if (t != NULL) {
    t->atime = s.st_atime;
    t->mtime = s.st_mtime;
    t->ctime = (time_t) s.st_reserved[0];
  }

  return unix2dostime(&s.st_mtime);
}

int set_extra_field(z, z_utim)
struct zlist far *z;
iztimes *z_utim;
/* create extra field and change z->att if desired */
{
  return ZE_OK;
}

int deletedir(d)
char *d;                /* directory to delete */
/* Delete the directory *d if it is empty, do nothing otherwise.
   Return the result of rmdir(), delete(), or system().
   For VMS, d must be in format [x.y]z.dir;1  (not [x.y.z]).
 */
{
    return rmdir(d);
}

/*  COMMENTED OUT - SEE LATER DECLARATION
  void version_local()
  {
      printf("Compiled with %s under %s.\n", "T9255C", "TANDEM NSK"
      );
  }
*/

#endif /* !UTIL */


/*
 * TANDEM
 */

DIR *opendir(const char *dirname)
{
   short i, resolve;
   char sname[FILENAME_MAX + 1];
   short snamelen;
   char fname[FILENAME_MAX + 1];
   short fnamelen;
   char *p;
   short searchid,err,end;
   struct dirent *entry;
   DIR *dirp;
   char ext[EXTENSION_MAX + 1];
   short extension;

   extension = parsename(dirname, sname, ext);
   snamelen = strlen(sname);

   /*  First we work out how detailed the template is...
    *  e.g. If the template is DAVES*.* we want the search result
    *       in the same format
    */

   p = sname;
   i = 0;
   while ((p = strchr(p, TANDEM_DELIMITER)) != NULL){
     i++;
     p++;
   };
   resolve = 2 - i;

   /*  Attempt to start a filename template */
   err = FILENAME_FINDSTART_ ( &searchid,
                               sname,
                               snamelen,
                               resolve,
                               DISK_DEVICE
                             );
   if (err != 0) {
     end = FILENAME_FINDFINISH_(searchid);
     return NULL;
   }

   /* Create DIR structure */
   if ((dirp = malloc(sizeof(DIR))) == NULL ) {
     end = FILENAME_FINDFINISH_(searchid);
     return NULL;
   }
   dirp->D_list = dirp->D_curpos = NULL;
   strcpy(dirp->D_path, dirname);

   while ((err = FILENAME_FINDNEXT_(searchid,
                                    fname,
                                    FILENAME_MAX,
                                    &fnamelen
                                   )
           ) == 0 ){
     /*  Create space for entry */
     if ((entry = malloc (sizeof(struct dirent))) == NULL) {
       end = FILENAME_FINDFINISH_(searchid);
       return NULL;
     }

     /*  Link to last entry */
     if (dirp->D_curpos == NULL)
       dirp->D_list = dirp->D_curpos = entry;  /* First name */
     else {
       dirp->D_curpos->d_next = entry;         /* Link */
       dirp->D_curpos = entry;
     };
     /* Add directory entry */
     *dirp->D_curpos->d_name = '\0';
     strncat(dirp->D_curpos->d_name,fname,fnamelen);
     if (extension) {
       strcat(dirp->D_curpos->d_name,TANDEM_EXTENSION_STR);
       strcat(dirp->D_curpos->d_name,ext);
     };
     dirp->D_curpos->d_next = NULL;
   };

   end = FILENAME_FINDFINISH_(searchid);

   if (err = 1) {  /*  Should return EOF at end of search */
     dirp->D_curpos = dirp->D_list;        /* Set current pos to start */
     return dirp;
   }
   else
     return NULL;
}

struct dirent *readdir(DIR *dirp)
{
   struct dirent *cur;

   cur = dirp->D_curpos;
   dirp->D_curpos = dirp->D_curpos->d_next;
   return cur;
}

void rewinddir(DIR *dirp)
{
   dirp->D_curpos = dirp->D_list;
}

int closedir(DIR *dirp)
{
   struct dirent *node;

   while (dirp->D_list != NULL) {
      node = dirp->D_list;
      dirp->D_list = dirp->D_list->d_next;
      free( node );
   }
   free( dirp );
   return 0;
}

local char *readd(d)
DIR *d;                 /* directory stream to read from */
/* Return a pointer to the next name in the directory stream d, or NULL if
   no more entries or an error occurs. */
{
  struct dirent *e;

  e = readdir(d);
  return e == NULL ? (char *) NULL : e->d_name;
}

int procname(n)
char *n;                /* name to process */
/* Process a name or sh expression to operate on (or exclude).  Return
   an error code in the ZE_ class. */
{
  char *a;              /* path and name for recursion */
  DIR *d;               /* directory stream from opendir() */
  char *e;              /* pointer to name from readd() */
  int m;                /* matched flag */
  char *p;              /* path for recursion */
  struct stat s;        /* result of stat() */
  struct zlist far *z;  /* steps through zfiles list */

  if (strcmp(n, "-") == 0)   /* if compressing stdin */
    return newname(n, 0);
  else if (stat(n, &s))
  {
    /* Not a file or directory--search for shell expression in zip file */
    p = ex2in(n, 0, (int *)NULL);       /* shouldn't affect matching chars */
    m = 1;
    for (z = zfiles; z != NULL; z = z->nxt) {
      if (MATCH(p, z->zname))
      {
        z->mark = pcount ? filter(z->zname) : 1;
        if (verbose)
            fprintf(mesg, "zip diagnostic: %scluding %s\n",
               z->mark ? "in" : "ex", z->name);
        m = 0;
      }
    }
    free((zvoid *)p);
    return m ? ZE_MISS : ZE_OK;
  }

  /* Live name--use if file, recurse if directory */
  if ((s.st_mode & S_IFDIR) == 0)
  {
    /* add or remove name of file */
    if ((m = newname(n, 0)) != ZE_OK)
      return m;
  } else {
    if ((p = malloc(strlen(n)+4)) == NULL)
      return ZE_MEM;

    strcpy(p, n);

    /* No concept of directories on Tandem - so do not store them ...*/
    /* code removed from which attempted to save dir name if dirnames set */

    /*  Test for recurse being set removed, since Tandem has no dir concept */
    /*  recurse into template */
    if ((d = opendir(n)) != NULL)
    {
      while ((e = readd(d)) != NULL) {
        if ((m = procname(e)) != ZE_OK)   /* recurse on name */
        {
          if (m == ZE_MISS)
            zipwarn("name not matched: ", e);
          else
            ziperr(m, e);
        }
      }
      closedir(d);
    }
    free((zvoid *)p);
  } /* (s.st_mode & S_IFDIR) == 0) */
  return ZE_OK;
}

/******************************/
/*  Function version_local()  */
/******************************/

void version_local()
{
    static ZCONST char CompiledWith[] = "Compiled with %s%s for %s%s%s%s.\n\n";
#if 0
    char buf[40];
#endif

    printf(CompiledWith,

#ifdef __GNUC__
      "gcc ", __VERSION__,
#else
#  if 0
      "cc ", (sprintf(buf, " version %d", _RELEASE), buf),
#  else
      "unknown compiler", "",
#  endif
#endif

      "Tandem/NSK", "",

#ifdef __DATE__
      " on ", __DATE__
#else
      "", ""
#endif
      );

} /* end function version_local() */
