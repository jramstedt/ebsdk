/*

 Copyright (C) 1990-1996 Mark Adler, Richard B. Wales, Jean-loup Gailly,
 Kai Uwe Rommel, Onno van der Linden, George Petrov and Igor Mandrichenko.
 Permission is granted to any individual or institution to use, copy, or
 redistribute this software so long as all of the original files are included,
 that it is not sold for profit, and that this copyright notice is retained.

*/

/*
 * routines common to VM/CMS and MVS
 */

#include "zip.h"

#include <time.h>
#include <errno.h>

int stat(const char *path, struct stat *buf)
{
   if ((buf->fp = fopen(path, "r")) != NULL) {
      fldata_t fdata;
      if (fldata( buf->fp, buf->fname, &fdata ) == 0) {
         buf->st_dev  = fdata.__device;
         buf->st_mode = *(short *)(&fdata);
      }
      strcpy( buf->fname, path );
      fclose(buf->fp);
   }
   return (buf->fp != NULL ? 0 : 1);
}


#ifndef UTIL    /* the companion #endif is a bit of ways down ... */

#define PAD 0
#define PATH_END '/'

/* Library functions not in (most) header files */

#ifdef USE_ZIPMAIN
int main OF((void));
#endif

int utime OF((char *, ztimbuf *));

extern char *label;
local ulg label_time = 0;
local ulg label_mode = 0;
local time_t label_utim = 0;

int fstat(int fd, struct stat *buf)
{
   fldata_t fdata;

   if ((fd != -1) && (fldata( (FILE *)fd, buf->fname, &fdata ) == 0)) {
      buf->st_dev  = fdata.__device;
      buf->st_mode = *(short *)(&fdata);
      buf->fp      = (FILE *)fd;
      return 0;
   }
   return -1;
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
  char mem[10] = "";    /* member name */
  char ext[10] = "";     /* extension name */

  dosflag = dosify;  /* default for non-DOS non-OS/2 */

  /* Find starting point in name before doing malloc */
  for (t = x; *t == '/'; t++)
    ;

  /* Make changes, if any, to the copied name (leave original intact) */
  if (!pathput)
    t = last(t, PATH_END);

  /* Malloc space for internal name and copy it */
  if ((n = malloc(strlen(t) + 1)) == NULL)
    return NULL;
  strcpy(n, t);

#ifdef MVS
  /* Change member names to fn.ext */
  if (t = strrchr(n, '(')) {
     *t = '\0';
     strcpy(mem,t+1);        /* Save member name */
     if (t = strchr(mem, ')')) *t = '\0';         /* Set end of mbr */
     /* Save extension */
     if (t = strrchr(n, '.')) t++;
     else t = n;
     strcpy(ext,t);
     /* Build name as "member.ext" */
     strcpy(t,mem);
     strcat(t,".");
     strcat(t,ext);
  }

  /* Change all but the last '.' to '/' */
  if (t = strrchr(n, '.')) {
     while (--t > n)
        if (*t == '.')
          *t = '/';
  }
#else
  /* On CMS, remove the filemode (all past 2nd '.') */
  if (t = strchr(n, '.'))
     if (t = strchr(t+1, '.'))
        *t = '\0';
  t = n;
#endif

  n = strtoasc(n,t);

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

  if ((x = malloc(strlen(n) + 1 + PAD)) == NULL)
    return NULL;
  strtoebc(x, n);
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
iztimes *t;             /* return value: access, modific. and creation times */
{
  FILE *stream;
  time_t ltime;

  if (strcmp(f, "-") != 0) {    /* if not compressing stdin */
     if ((stream = fopen(f, "rb")) == (FILE *)NULL) {
        return 0;
     } else {
        if (n != NULL) {
/**        *n = -1L;  **/
           /* Input file is now RECFM F LRECL 1, so this will work */
           fseek(stream,0L,SEEK_END);
           *n = ftell(stream);
        }
        fclose(stream);
     }
  }
  else {
     if (n != NULL) {
        *n = -1L;
     }
  }

  /* Return current time for all the times -- for now */
  time(&ltime);
  if (t != NULL)
     t->atime = t->mtime = t->ctime = ltime;

  return unix2dostime(&ltime);
}



int set_extra_field(z, z_utim)
struct zlist far *z;
iztimes *z_utim;
/* create extra field and change z->att if desired */
{
   fldata_t fdata;
   FILE *stream;
   char *eb_ptr;
#ifdef USE_EF_UT_TIME
   extent ef_l_len = (EB_HEADSIZE+EB_UT_LEN(1));
#else /* !USE_EF_UT_TIME */
   extent ef_l_len = 0;
#endif /* ?USE_EF_UT_TIME */
   int set_cmsmvs_eb = 0;

/*translate_eol = 0;*/
  if (aflag == ASCII) {
     z->att = ASCII;
  } else {
    if (bflag)
      z->att = BINARY;
    else
      z->att = __EBCDIC;
    ef_l_len += sizeof(fdata)+EB_HEADSIZE;
    set_cmsmvs_eb = 1;
  }

  if (ef_l_len > 0) {
    z->extra = (char *)malloc(ef_l_len);
    if (z->extra == NULL) {
       printf("\nFLDATA : Unable to allocate memory !\n");
       return ZE_MEM;
    }
    z->cext = z->ext = ef_l_len;
    eb_ptr = z->cextra = z->extra;

    if (set_cmsmvs_eb) {
      if (bflag)
/***    stream = fopen(z->zname,"rb,type=record");   $RGH$ ***/
        stream = fopen(z->zname,"rb");
      else
        stream = fopen(z->zname,"r");
      if (stream == NULL) {
        printf("\nFLDATA : Could not open file : %s !\n",z->zname);
        printf("Error %d: '%s'\n", errno, strerror(errno));
        return ZE_NONE;
      }

      fldata(stream,z->zname,&fdata);
      /*put the system ID */
#ifdef VM_CMS
      *(eb_ptr) = EF_VMCMS & 0xFF;
      *(eb_ptr+1) = EF_VMCMS >> 8;
#else
      *(eb_ptr) = EF_MVS & 0xFF;
      *(eb_ptr+1) = EF_MVS >> 8;
#endif
      *(eb_ptr+2) = sizeof(fdata) & 0xFF;
      *(eb_ptr+3) = sizeof(fdata) >> 8;

      memcpy(eb_ptr+EB_HEADSIZE,&fdata,sizeof(fdata));
      fclose(stream);
#ifdef USE_EF_UT_TIME
      eb_ptr += (sizeof(fdata)+EB_HEADSIZE);
#endif /* USE_EF_UT_TIME */
    }
#ifdef USE_EF_UT_TIME
    eb_ptr[0]  = 'U';
    eb_ptr[1]  = 'T';
    eb_ptr[2]  = EB_UT_LEN(1);          /* length of data part of e.f. */
    eb_ptr[3]  = 0;
    eb_ptr[4]  = EB_UT_FL_MTIME;
    eb_ptr[5]  = (char)(z_utim->mtime);
    eb_ptr[6]  = (char)(z_utim->mtime >> 8);
    eb_ptr[7]  = (char)(z_utim->mtime >> 16);
    eb_ptr[8]  = (char)(z_utim->mtime >> 24);
#endif /* USE_EF_UT_TIME */
  }

  return ZE_OK;
}

int deletedir(d)
char *d;                /* directory to delete */
/* Delete the directory *d if it is empty, do nothing otherwise.
   Return the result of rmdir(), delete(), or system().
   For VMS, d must be in format [x.y]z.dir;1  (not [x.y.z]).
 */
{
    return 0;
}

#ifdef USE_ZIPMAIN
/* This function is called as main() to parse arguments                */
/* into argc and argv.  This is required for stand-alone               */
/* execution.  This calls the "real" main() when done.                 */

int main(void)
   {
    int  argc=0;
    char *argv[50];

    int  iArgLen;
    char argstr[256];
    char **pEPLIST, *pCmdStart, *pArgStart, *pArgEnd;

   /* Get address of extended parameter list from S/370 Register 0 */
   pEPLIST = (char **)__xregs(0);

   /* Null-terminate the argument string */
   pCmdStart = *(pEPLIST+0);
   pArgStart = *(pEPLIST+1);
   pArgEnd   = *(pEPLIST+2);
   iArgLen   = pArgEnd - pCmdStart + 1;

   /* Make a copy of the command string */
   memcpy(argstr, pCmdStart, iArgLen);
   argstr[iArgLen] = '\0';  /* Null-terminate */

   /* Store first token (cmd) */
   argv[argc++] = strtok(argstr, " ");

   /* Store the rest (args) */
   while (argv[argc-1])
      argv[argc++] = strtok(NULL, " ");
   argc--;  /* Back off last NULL entry */

   /* Call "real" main() function */
   return zipmain(argc, argv);

}
#endif /* USE_ZIPMAIN */

#endif /* !UTIL */


/******************************/
/*  Function version_local()  */
/******************************/

void version_local()
{
    int len;
    char liblvlmsg [50+1];

    union {
       unsigned int iVRM;
       struct {
          unsigned int v:8;
          unsigned int r:8;
          unsigned int m:8;
       } xVRM;
    } VRM;

    /* Break down the runtime library level */
    VRM.iVRM = __librel();
    sprintf(liblvlmsg, "Using runtime library level V%dR%dM%d.\n",
            VRM.xVRM.v, VRM.xVRM.r, VRM.xVRM.m);


    printf("Compiled with %s%s for %s%s%s\n",
    /* Add compiler name and level */
      "C/370", "",         /* Assumed.  Can't get compiler name/lvl(?) */

#ifdef VM_CMS
      "VM/CMS",
#else
      "MVS",
#endif

    /* Add timestamp */
#ifdef __DATE__
      " on " __DATE__
#ifdef __TIME__
     " at " __TIME__
#endif
     ".\n",
#else
      "",
#endif

    liblvlmsg
    );
} /* end function version_local() */
