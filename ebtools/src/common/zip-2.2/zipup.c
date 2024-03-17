/*

 Copyright (C) 1990-1997 Mark Adler, Richard B. Wales, Jean-loup Gailly,
 Kai Uwe Rommel, Onno van der Linden and Igor Mandrichenko.
 Permission is granted to any individual or institution to use, copy, or
 redistribute this software so long as all of the original files are included,
 that it is not sold for profit, and that this copyright notice is retained.

*/

/*
 *  zipup.c by Mark Adler and Jean-loup Gailly.
 */

#define NOCPYRT         /* this is not a main module */
#include <ctype.h>
#include "zip.h"

#ifndef UTIL            /* This module contains no code for Zip Utilities */

#include "revision.h"
#include "crypt.h"

#ifdef OS2
#  include "os2/os2zip.h"
#endif

#if defined(MMAP)
#  include <sys/mman.h>
#  ifndef PAGESIZE   /* used to be SYSV, what about pagesize on SVR3 ? */
#    define PAGESIZE getpagesize()
#  endif
#  if defined(NO_VALLOC) && !defined(valloc)
#    define valloc malloc
#  endif
#endif


/* Use the raw functions for MSDOS and Unix to save on buffer space.
   They're not used for VMS since it doesn't work (raw is weird on VMS).
   (This sort of stuff belongs in fileio.c, but oh well.) */

#ifdef AMIGA
#  include "amiga/zipup.h"
#endif /* AMIGA */

#ifdef AOSVS
#  include "aosvs/zipup.h"
#endif /* AOSVS */

#ifdef ATARI
#  include "atari/zipup.h"
#endif

#ifdef __BEOS__
#  include "beos/zipup.h"
#endif

#ifdef __human68k__
#  include "human68k/zipup.h"
#endif /* __human68k__ */

#ifdef DOS
#  include "msdos/zipup.h"
#endif /* DOS */

#ifdef OS2
#  include "os2/zipup.h"
#endif /* OS2 */

#ifdef RISCOS
#  include "acorn/zipup.h"
#endif

#ifdef TOPS20
#  include "tops20/zipup.h"
#endif

#ifdef UNIX
#  include "unix/zipup.h"
#endif

#ifdef CMS_MVS
#  include "zipup.h"
#endif /* CMS_MVS */

#ifdef TANDEM
#  include "zipup.h"
#endif /* TANDEM */

#ifdef VMS
#  include "vms/zipup.h"
#endif /* VMS */

#ifdef QDOS
#  include "qdos/zipup.h"
#endif /* QDOS */

#ifdef WIN32
#  include "win32/zipup.h"
#endif


/* Deflate "internal" global data (currently not in zip.h) */
#if defined(MMAP) || defined(BIG_MEM)
  extern uch * window;          /* Used to read all input file at once */
#endif
extern ulg window_size;         /* size of said window */

/* Local data */

  local ulg crc;       /* crc on uncompressed file data */
  local ftype ifile;   /* file to compress */
#if defined(MMAP) || defined(BIG_MEM)
  local long remain;
  /* window bytes not yet processed.
   *  >= 0 only for BIG_MEM or MMAP, -1 for normal reads.
   */
#endif /* MMAP || BIG_MEM */
#ifdef DEBUG
  ulg isize;           /* input file size. global only for debugging */
#else /* !DEBUG */
  local ulg isize;     /* input file size. */
#endif /* ?DEBUG */

/* Local functions */
#ifndef RISCOS
   local int suffixes OF((char *, char *));
#else
   local int filetypes OF((char *, char *));
#endif


int percent(n, m)
ulg n;
ulg m;               /* n is the original size, m is the new size */
/* Return the percentage compression from n to m using only integer
   operations */
{
  if (n > 0xffffffL)            /* If n >= 16M */
  {                             /*  then divide n and m by 256 */
    n += 0x80;  n >>= 8;
    m += 0x80;  m >>= 8;
  }
  return n > m ? (int)(1 + (200 * (n - m)/n)) / 2 : 0;
}

#ifndef RISCOS

local int suffixes(a, s)
char *a;                /* name to check suffix of */
char *s;                /* list of suffixes separated by : or ; */
/* Return true if a ends in any of the suffixes in the list s. */
{
  int m;                /* true if suffix matches so far */
  char *p;              /* pointer into special */
  char *q;              /* pointer into name a */

#ifdef QDOS
  short dlen = devlen(a);
  a = a + dlen;
#endif

  m = 1;
#ifdef VMS
  if( (q = strrchr(a,';')) != NULL )    /* Cut out VMS file version */
    --q;
  else
    q = a + strlen(a) - 1;
#else /* !VMS */
  q = a + strlen(a) - 1;
#endif /* ?VMS */
  for (p = s + strlen(s) - 1; p >= s; p--)
    if (*p == ':' || *p == ';')
      if (m)
        return 1;
      else
      {
        m = 1;
#ifdef VMS
        if( (q = strrchr(a,';')) != NULL )      /* Cut out VMS file version */
          --q;
        else
          q = a + strlen(a) - 1;
#else /* !VMS */
        q = a + strlen(a) - 1;
#endif /* ?VMS */
      }
    else
    {
      m = m && q >= a && case_map(*p) == case_map(*q);
      q--;
    }
  return m;
}

#else /* RISCOS */

local int filetypes(a, s)
char *a;                /* extra field of file to check filetype of */
char *s;                /* list of filetypes separated by : or ; */
/* Return true if a is any of the filetypes in the list s. */
{
 char *p;              /* pointer into special */
 char typestr[4];     /* filetype hex string taken from a */

 if ((((unsigned*)a)[2] & 0xFFF00000) != 0xFFF00000) {
 /* The file is not filestamped, always try to compress it */
   return 0;
 }

 sprintf(typestr,"%.3X",(((unsigned*)a)[2] & 0x000FFF00) >> 8);

 for (p=s;p<=s+strlen(s)-3;p+=3) { /* p+=3 to skip 3 hex type */
   while (*p==':' || *p==';')
     p++;

   if (typestr[0]==toupper(p[0]) && typestr[1]==toupper(p[1]) && typestr[2]==toupper(p[2]))
     return 1;
 }
 return 0;
}
#endif /* ?RISCOS */

/* Note: a zip "entry" includes a local header (which includes the file
   name), an encryption header if encrypting, the compressed data
   and possibly an extended local header. */

int zipup(z, y)
struct zlist far *z;    /* zip entry to compress */
FILE *y;                /* output file */
/* Compress the file z->name into the zip entry described by *z and write
   it to the file *y. Encrypt if requested.  Return an error code in the
   ZE_ class.  Also, update tempzn by the number of bytes written. */
{
  iztimes f_utim;       /* UNIX GMT timestamps, filled by filetime() */
  ulg tim;              /* time returned by filetime() */
  ulg a = 0L;           /* attributes returned by filetime() */
  char *b;              /* malloc'ed file buffer */
  extent k = 0;         /* result of zread */
  int l = 0;            /* true if this file is a symbolic link */
  int m;                /* method for this entry */
  ulg o, p;             /* offsets in zip file */
  long q = -3L;         /* size returned by filetime */
  int r;                /* temporary variable */
  ulg s = 0L;           /* size of compressed data */
  int isdir;            /* set for a directory name */
  int set_type = 0;     /* set if file type (ascii/binary) unknown */

  z->nam = strlen(z->iname);
  isdir = z->iname[z->nam-1] == '/';

  if ((tim = filetime(z->name, &a, &q, &f_utim)) == 0 || q < -2L)
    return ZE_OPEN;

  /* q is set to -1 if the input file is a device, -2 for a volume label */
  if (q == -2L) {
     isdir = 1;
     q = 0;
  } else if (isdir != ((a & MSDOS_DIR_ATTR) != 0)) {
     /* don't overwrite a directory with a file and vice-versa */
     return ZE_MISS;
  }
  z->att = (ush)UNKNOWN; /* will be changed later */
  z->atx = 0; /* may be changed by set_extra_field() */

  /* Free the old extra fields which are probably obsolete */
  if (z->ext) {
    free((zvoid *)(z->extra));
  }
  if (z->cext && z->extra != z->cextra) {
    free((zvoid *)(z->cextra));
  }
  z->extra = z->cextra = NULL;
  z->ext = z->cext = 0;

#if defined(MMAP) || defined(BIG_MEM)
  remain = -1L; /* changed only for MMAP or BIG_MEM */
#endif /* MMAP || BIG_MEM */
  window_size = 0L;

  /* Select method based on the suffix and the global method */
#ifndef RISCOS
  m = special != NULL && suffixes(z->name, special) ? STORE : method;
#else /* RISCOS  must set m after setting extra field */
  m = method;
#endif /* ?RISCOS */

  /* Open file to zip up unless it is stdin */
  if (strcmp(z->name, "-") == 0)
  {
    ifile = (ftype)zstdin;
#if defined(MSDOS) || defined(__human68k__)
    setmode(zstdin, O_BINARY);
#endif
    z->tim = tim;
  }
  else
  {
#if !(defined(VMS) && defined(VMS_PK_EXTRA))
    if (extra_fields) {
      /* create extra field and change z->att and z->atx if desired */
      set_extra_field(z, &f_utim);
#ifdef QLZIP
      if(qlflag)
          a |= (S_IXUSR) << 16;   /* Cross compilers don't set this */
#endif
#ifdef RISCOS
      m = special != NULL && filetypes(z->extra, special) ? STORE : method;
#endif /* RISCOS */
    }
#endif /* !(VMS && VMS_PK_EXTRA) */
    l = issymlnk(a);
    if (l)
      ifile = fbad;
    else if (isdir) { /* directory */
      ifile = fbad;
      m = STORE;
      q = 0;
    }
    else {
#ifdef CMS_MVS
      if (bflag) {
        if ((ifile = zopen(z->name, fhowb)) == fbad)
           return ZE_OPEN;
      }
      else
#endif /* CMS_MVS */
      if ((ifile = zopen(z->name, fhow)) == fbad)
         return ZE_OPEN;
    }

    z->tim = tim;

#if defined(VMS) && defined(VMS_PK_EXTRA)
    /* vms_get_attributes must be called after vms_open() */
    if (extra_fields) {
      /* create extra field and change z->att and z->atx if desired */
      vms_get_attributes(ifile, z, &f_utim);
    }
#endif /* VMS && VMS_PK_EXTRA */

#ifdef MMAP
    /* Map ordinary files but not devices. This code should go in fileio.c */
    if (q > 0 && !translate_eol) {
      if (window != NULL)
        free(window);  /* window can't be a mapped file here */
      window_size = q + MIN_LOOKAHEAD;
      remain = window_size & (PAGESIZE-1);
      /* If we can't touch the page beyond the end of file, we must
       * allocate an extra page.
       */
      if (remain > MIN_LOOKAHEAD) {
        window = (uch*)mmap(0, window_size, PROT_READ, MAP_PRIVATE, ifile, 0);
      } else {
        window = (uch*)valloc(window_size - remain + PAGESIZE);
        if (window != NULL) {
          window = (uch*)mmap((char*)window, window_size - remain, PROT_READ,
                        MAP_PRIVATE | MAP_FIXED, ifile, 0);
        } else {
          window = (uch*)(-1);
        }
      }
      if (window == (uch*)(-1)) {
        Trace((mesg, " mmap failure on %s\n", z->name));
        window = NULL;
        window_size = 0L;
        remain = -1L;
      } else {
        remain = q;
      }
    }
#else /* !MMAP */
# ifdef BIG_MEM
    /* Read the whole input file at once */
    if (q > 0 && !translate_eol) {
      window_size = q + MIN_LOOKAHEAD;
      window = window ? (uch*) realloc(window, (unsigned)window_size)
                      : (uch*) malloc((unsigned)window_size);
      /* Just use normal code if big malloc or realloc fails: */
      if (window != NULL) {
        remain = zread(ifile, (char*)window, q+1);
        if (remain != q) {
          fprintf(mesg, " q=%ld, remain=%ld ", q, remain);
          error("can't read whole file at once");
        }
      } else {
        window_size = 0L;
      }
    }
# endif /* BIG_MEM */
#endif /* ?MMAP */

  } /* strcmp(z->name, "-") == 0 */

  if (l || q == 0)
    m = STORE;
  if (m == BEST)
    m = DEFLATE;

  /* Do not create STORED files with extended local headers if the
   * input size is not known, because such files could not be extracted.
   * So if the zip file is not seekable and the input file is not
   * on disk, obey the -0 option by forcing deflation with stored block.
   * Note however that using "zip -0" as filter is not very useful...
   * ??? to be done.
   */

  /* Fill in header information and write local header to zip file.
   * This header will later be re-written since compressed length and
   * crc are not yet known.
   */

  /* (Assume ext, cext, com, and zname already filled in.) */
#if defined(OS2) || defined(WIN32)
  z->vem = z->dosflag ? (dosify ? 20 :  /* Made under MSDOS by PKZIP 2.0 */
                        (0 + Z_MAJORVER * 10 + Z_MINORVER))
         : OS_CODE + Z_MAJORVER * 10 + Z_MINORVER;
  /* For a FAT file system, we cheat and pretend that the file
   * was not made on OS2 but under DOS. unzip is confused otherwise.
   */
#else /* !(OS2 || WIN32) */
  z->vem = dosify ? 20 : OS_CODE + Z_MAJORVER * 10 + Z_MINORVER;
#endif /* ?(OS2 || WIN32) */

  z->ver = m == STORE ? 10 : 20;     /* Need PKUNZIP 2.0 except for store */
  z->crc = 0;  /* to be updated later */
  /* Assume first that we will need an extended local header: */
  z->flg = 8;  /* to be updated later */
#if CRYPT
  if (key != NULL) {
    z->flg |= 1;
    /* Since we do not yet know the crc here, we pretend that the crc
     * is the modification time:
     */
    z->crc = z->tim << 16;
  }
#endif /* CRYPT */
  z->lflg = z->flg;
  z->how = m;                             /* may be changed later  */
  z->siz = m == STORE && q >= 0 ? q : 0;  /* will be changed later  */
  z->len = q >= 0 ? q : 0;                /* may be changed later  */
  z->dsk = 0;
  if (z->att == (ush)UNKNOWN) {
      z->att = BINARY;                    /* set sensible value in header */
      set_type = 1;
  }
  /* Attributes from filetime(), flag bits from set_extra_field(): */
#if defined(DOS) || defined(OS2) || defined(WIN32)
  z->atx = z->dosflag ? a & 0xff : a | (z->atx & 0x0000ff00);
#else
  z->atx = dosify ? a & 0xff : a | (z->atx & 0x0000ff00);
#endif /* DOS || OS2 || WIN32 */
  z->off = tempzn;
  if ((r = putlocal(z, y)) != ZE_OK)
    return r;
  tempzn += 4 + LOCHEAD + z->nam + z->ext;

#if CRYPT
  if (key != NULL) {
    crypthead(key, z->crc, y);
    z->siz += RAND_HEAD_LEN;  /* to be updated later */
    tempzn += RAND_HEAD_LEN;
  }
#endif /* CRYPT */
  if (ferror(y))
    ZIPERR(ZE_WRITE, "unexpected error on zip file");

  o = ftell(y); /* for debugging only, ftell can fail on pipes */
  if (ferror(y))
    clearerr(y);

  /* Write stored or deflated file to zip file */
  isize = 0L;
  crc = CRCVAL_INITIAL;

  if (m == DEFLATE) {
     bi_init(y);
     if (set_type) z->att = (ush)UNKNOWN; /* will be changed in deflate() */
     ct_init(&z->att, &m);
     lm_init(level, &z->flg);
     s = deflate();
  }
  else if (!isdir)
  {
#ifdef SBSZ
    if ((b = malloc(SBSZ)) == NULL)
#else
    if ((b = malloc(CBSZ)) == NULL)
#endif
       return ZE_MEM;

    if (l) {
#ifdef SBSZ
      k = rdsymlnk(z->name, b, SBSZ);
#else
      k = rdsymlnk(z->name, b, CBSZ);
#endif
/*
 * compute crc first because zfwrite will alter the buffer b points to !!
 */
      crc = crc32(crc, (uch *) b, k);
      if (zfwrite(b, 1, k, y) != k)
      {
        free((zvoid *)b);
        return ZE_TEMP;
      }
      isize = k;

#ifdef MINIX
      q = k;
#endif /* MINIX */
    }
    else
    {
#ifdef SBSZ
      while ((k = file_read(b, SBSZ)) > 0 && k != (extent) EOF)
#else
      while ((k = file_read(b, CBSZ)) > 0 && k != (extent) EOF)
#endif
      {
        if (zfwrite(b, 1, k, y) != k)
        {
          free((zvoid *)b);
          return ZE_TEMP;
        }
#ifndef WINDLL
        if (verbose) putc('.', stderr);
#else
        if (verbose) fprintf(stdout,"%c",'.');
#endif
      }
    }
    free((zvoid *)b);
    s = isize;
  }
  if (ifile != fbad && zerr(ifile)) {
    perror("\nzip warning");
    zipwarn("could not read input file: ", z->zname);
  }
  if (ifile != fbad)
    zclose(ifile);
#ifdef MMAP
  if (remain >= 0L) {
    munmap((caddr_t) window, window_size);
    window = NULL;
  }
#endif /*MMAP */

  tempzn += s;
  p = tempzn; /* save for future fseek() */

#if (!defined(MSDOS) || defined(OS2))
#if !defined(VMS) && !defined(CMS_MVS)
  /* Check input size (but not in VMS -- variable record lengths mess it up)
   * and not on MSDOS -- diet in TSR mode reports an incorrect file size)
   */
#ifndef TANDEM /* Tandem EOF does not match byte count unless Unstructured */
  if (q >= 0 && isize != (ulg)q && !translate_eol)
  {
    Trace((mesg, " i=%ld, q=%ld ", isize, q));
    zipwarn(" file size changed while zipping ", z->name);
  }
#endif /* !TANDEM */
#endif /* !VMS && !CMS_MVS */
#endif /* (!MSDOS || OS2) */

  /* Try to rewrite the local header with correct information */
  z->crc = crc;
  z->siz = s;
#if CRYPT
  if (key != NULL)
    z->siz += RAND_HEAD_LEN;
#endif /* CRYPT */
  z->len = isize;
#ifdef BROKEN_FSEEK
  if (!fseekable(y) || fseek(y, z->off, SEEK_SET))
#else
  if (fseek(y, z->off, SEEK_SET))
#endif
  {
    if (z->how != (ush) m)
       error("can't rewrite method");
    if (m == STORE && q < 0)
       ZIPERR(ZE_PARMS, "zip -0 not supported for I/O on pipes or devices");
    if ((r = putextended(z, y)) != ZE_OK)
      return r;
    tempzn += 16L;
    z->flg = z->lflg; /* if flg modified by inflate */
  } else {
     /* seek ok, ftell() should work, check compressed size */
#if !defined(VMS) && !defined(CMS_MVS)
    if (p - o != s) {
      fprintf(mesg, " s=%ld, actual=%ld ", s, p-o);
      error("incorrect compressed size");
    }
#endif /* !VMS && !CMS_MVS */
    z->how = m;
    z->ver = m == STORE ? 10 : 20;     /* Need PKUNZIP 2.0 except for store */
    if ((z->flg & 1) == 0)
      z->flg &= ~8; /* clear the extended local header flag */
    z->lflg = z->flg;
    /* rewrite the local header: */
    if ((r = putlocal(z, y)) != ZE_OK)
      return r;
    if (fseek(y, p, SEEK_SET))
      return ZE_READ;
    if ((z->flg & 1) != 0) {
      /* encrypted file, extended header still required */
      if ((r = putextended(z, y)) != ZE_OK)
        return r;
      tempzn += 16L;
    }
  }
  /* Free the local extra field which is no longer needed */
  if (z->ext) {
    if (z->extra != z->cextra) {
      free((zvoid *)(z->extra));
      z->extra = NULL;
    }
    z->ext = 0;
  }

  /* Display statistics */
  if (noisy)
  {
    if (verbose)
      fprintf(mesg, "\t(in=%lu) (out=%lu)", isize, s);
    if (m == DEFLATE)
      fprintf(mesg, " (deflated %d%%)\n", percent(isize, s));
    else
      fprintf(mesg, " (stored 0%%)\n");
    fflush(mesg);
  }
  return ZE_OK;
}


int file_read(buf, size)
  char *buf;
  unsigned size;
/* Read a new buffer from the current input file, perform end-of-line
 * translation, and update the crc and input file size.
 * IN assertion: size >= 2 (for end-of-line translation)
 */
{
  unsigned len;
  char *b;

#if defined(MMAP) || defined(BIG_MEM)
  if (remain == 0L) {
    return 0;
  } else if (remain > 0L) {
    /* The window data is already in place. We still compute the crc
     * by 32K blocks instead of once on whole file to keep a certain
     * locality of reference.
     */
    Assert (buf == (char*)window + isize, "are you lost?");
    if (size > remain) size = remain;
    if (size > WSIZE) size = WSIZE; /* don't touch all pages at once */
    remain -= (long) size;
    len = size;
  } else
#endif /* MMAP || BIG_MEM */
  if (translate_eol == 0) {
    len = zread(ifile, buf, size);
    if (len == (unsigned)EOF || len == 0) return (int)len;
  } else if (translate_eol == 1) {
    /* Transform LF to CR LF */
    size >>= 1;
    b = buf+size;
    size = len = zread(ifile, b, size);
    if (len == (unsigned)EOF || len == 0) return (int)len;
#ifdef EBCDIC
    if (aflag == ASCII)
    {
       do {
          char c;

          if ((c = *b++) == '\n') {
             *buf++ = CR; *buf++ = LF; len++;
          } else {
            *buf++ = (char)ascii[(uch)c];
          }
       } while (--size != 0);
    }
    else
#endif /* EBCDIC */
    {
       do {
          if ((*buf++ = *b++) == '\n') *(buf-1) = '\r', *buf++ = '\n', len++;
       } while (--size != 0);
    }
    buf -= len;

  } else {
    /* Transform CR LF to LF and suppress final ^Z */
    b = buf;
    size = len = zread(ifile, buf, size-1);
    if (len == (unsigned)EOF || len == 0) return (int)len;
    buf[len] = '\n'; /* I should check if next char is really a \n */
#ifdef EBCDIC
    if (aflag == ASCII)
    {
       do {
          char c;

          if ((c = *b++) == '\r' && *b == '\n') {
             len--;
          } else {
             *buf++ = (char)(c == '\n' ? LF : ascii[(uch)c]);
          }
       } while (--size != 0);
    }
    else
#endif /* EBCDIC */
    {
       do {
          if (( *buf++ = *b++) == '\r' && *b == '\n') buf--, len--;
       } while (--size != 0);
    }
    if (len == 0) {
       zread(ifile, buf, 1); len = 1; /* keep single \r if EOF */
#ifdef EBCDIC
       if (aflag == ASCII) {
          *buf = (char)(*buf == '\n' ? LF : ascii[(uch)(*buf)]);
       }
#endif
    } else {
       buf -= len;
       if (buf[len-1] == CTRLZ) len--; /* suppress final ^Z */
    }
  }
  crc = crc32(crc, (uch *) buf, len);
  isize += (ulg)len;
  return (int)len;
}
#endif /* !UTIL */
