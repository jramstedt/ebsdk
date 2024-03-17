/*---------------------------------------------------------------------------

  qdos.c

  QDOS-specific routines for use with Info-ZIP's UnZip 5.3 and later.

  Contains:  Qstrfix()
             QFilename()
             QMatch()
             chowner()
             Qgetch()
             QReturn()
             LastDir()
             do_wild()           <-- generic enough to put in file_io.c?
             mapattr()
             mapname()
             checkdir()
             qfix()
             close_outfile()
             stamp_file()
             getp()
             version()

  ---------------------------------------------------------------------------*/

#define UNZIP_INTERNAL

#include "unzip.h"
#include "crypt.h"
#include "ttyio.h"
#include <dirent.h>
#include "izqdos.h"
#include "version.h"

char _prog_name[] = "UnZip";
/* sorrid hack at request of GRR follows; hope the compiler stays kind to us */
char _version[] = {UZ_MAJORVER+'0','.',UZ_MINORVER+'0',PATCHLEVEL+'0'};
char _extra[] = " " BETALEVEL;
char _copyright[] = "(c) Info-ZIP Group";
char *  _endmsg = NULL;
long _stack = 8*1024;         /* huge stack (for qdos) */

extern void consetup_title(chanid_t,struct WINDOWDEF *);
void (*_consetup)(chanid_t,struct WINDOWDEF *) = consetup_title;

struct WINDOWDEF _condetails =
{
    2,
    1,
    0,
    7,
    500,
    220,
    2,
    30
};


static jobid_t chowner(chanid_t chan)
{
    extern char *_sys_var;
    char *scht;
    long *cdb;
    long jid;

    scht = *((char **)(_sys_var + 0x78));
    cdb = *(long **)((long *)scht  + (chan & 0xffff));
    jid = *(cdb + 2);
    return jid;
}

int QReturn(int err)
{
    jobid_t me,you;

    me = getpid();
    you = chowner(getchid(0));

    if((me == you) && ((qlflag & 4) == 0))
    {
        if(isatty(0) && isatty(2) && qlwait)
        {
            char c = 0;
            fputs("Press a key to exit", stderr);
            if((io_fbyte(getchid(0), qlwait, &c) == 0) && c == 27)
            {
                io_fbyte(getchid(0), -1, &c);
            }
        }
    }
    exit(err);
}

#ifndef FUNZIP

static int created_dir;        /* used in mapname(), checkdir() */
static int renamed_fullpath;   /* ditto */

char *Qstrfix (char *p)
{
    char *q;
    for (q = p; (q = strstr(q, ".zip"));)
    {
        memcpy(q, "_zip", 4);
        q += 4;
    }
    return p;
}

void QFilename(char *f)
{
    char *o,*p,*q = strdup(f);
    p = q;

    if(*q == '.' && *(q+1) == '/') q += 2;
    o = q;

    for(;*q;q++)
    {
        if(*q == '/') *q = '_';
        if((qlflag & 1) == 0)
        {
            if(*q == '.') *q = '_';
        }
    }
    strcpy(f,o);
    free(p);
}

int QMatch(uch c1, uch c2)
{
    int m =0;

    if(c1 != c2)
    {
        if(c1 == '_' && (c2 == '.' || c2 == '/'))
        {
            m = 1;
        }
    }
    else
    {
        m = 1;
    }
    return m;
}


int Qgetch(void)
{
    char ch;

    if(io_fbyte(getchid(0), -1, &ch) < 0)
    {
        return EOF;
    }
    else
    {
        return (int) ch;
    }
}

#ifndef SFX
char *LastDir(char *ws)
{
    char *p;
    char *q = ws;
    struct stat s;

    for(p = ws; *p; p++)
    {
        if(*p == '_')
        {
            char c;

            p++;
            c = *p;
            *p = 0;
            if(stat(ws, &s) == 0 && S_ISDIR(s.st_mode))
            {
                q = p;
            }
            *p = c;
        }
    }
    return q;
}


/**********************/
/* Function do_wild() */   /* for porting:  dir separator; match(ignore_case) */
/**********************/

char *do_wild(__G__ wildspec)
    __GDEF
    char *wildspec;         /* only used first time on a given dir */
{
    static DIR *dir = (DIR *)NULL;
    static char *dirname, *wildname, matchname[FILNAMSIZ];
    static int firstcall=TRUE, have_dirname, dirnamelen;
    struct dirent *file;
    char basedir[40];

    /* Even when we're just returning wildspec, we *always* do so in
     * matchname[]--calling routine is allowed to append four characters
     * to the returned string, and wildspec may be a pointer to argv[].
     */
    if (firstcall) {        /* first call:  must initialize everything */
        char *ws = NULL, *us = NULL;

        firstcall = FALSE;

        /* break the wildspec into a directory part and a wildcard filename */

        ws = (char *) iswild(wildspec);
        us = LastDir(wildspec);

        if(us == wildspec || us > ws)
        {
            dirname = basedir;
            getcwd(basedir, sizeof(basedir)-1);
            dirnamelen = strlen(basedir);
            have_dirname = FALSE;
            wildname = wildspec;
        } else {
            wildname = us;     /* point at character after '/' */
            dirnamelen = wildname - wildspec;
            if ((dirname = (char *)malloc(dirnamelen+1)) == (char *)NULL) {
                Info(slide, 0x201, ((char *)slide,
                  "warning:  cannot allocate wildcard buffers\n"));
                strcpy(matchname, wildspec);
                return matchname;   /* but maybe filespec was not a wildcard */
            }
            strncpy(dirname, wildspec, dirnamelen);
            dirname[dirnamelen] = '\0';   /* terminate for strcpy below */
            have_dirname = TRUE;
        }

        if ((dir = opendir(dirname)) != (DIR *)NULL) {
            while ((file = readdir(dir)) != (struct dirent *)NULL) {
                if (match(file->d_name, wildname, 2)) {  /* 0 == case sens. */
                    if (have_dirname) {
                        strcpy(matchname, dirname);
                        strcpy(matchname+dirnamelen, file->d_name);
                    } else
                        strcpy(matchname, file->d_name);
                    return matchname;
                }
            }
            /* if we get to here directory is exhausted, so close it */
            closedir(dir);
            dir = (DIR *)NULL;
        }

        /* return the raw wildspec in case that works (e.g., directory not
         * searchable, but filespec was not wild and file is readable) */
        strcpy(matchname, wildspec);
        return matchname;
    }

    /* last time through, might have failed opendir but returned raw wildspec */
    if (dir == (DIR *)NULL) {
        firstcall = TRUE;  /* nothing left to try--reset for new wildspec */
        if (have_dirname)
            free(dirname);
        return (char *)NULL;
    }

    /* If we've gotten this far, we've read and matched at least one entry
     * successfully (in a previous call), so dirname has been copied into
     * matchname already.
     */
    while ((file = readdir(dir)) != (struct dirent *)NULL)
        if (match(file->d_name, wildname, 0)) {   /* 0 == don't ignore case */
            if (have_dirname) {
                /* strcpy(matchname, dirname); */
                strcpy(matchname+dirnamelen, file->d_name);
            } else
                strcpy(matchname, file->d_name);
            return matchname;
        }

    closedir(dir);     /* have read at least one dir entry; nothing left */
    dir = (DIR *)NULL;
    firstcall = TRUE;  /* reset for new wildspec */
    if (have_dirname)
        free(dirname);
    return (char *)NULL;

} /* end function do_wild() */

#endif /* !SFX */





/**********************/
/* Function mapattr() */
/**********************/
int mapattr(__G)
    __GDEF
{
    ulg tmp = G.crec.external_file_attributes;

    switch (G.pInfo->hostnum) {
        case QDOS_:
        case UNIX_:
        case VMS_:
        case ACORN_:
        case ATARI_:
        case BEOS_:
            G.pInfo->file_attr = (unsigned)(tmp >> 16);
            return 0;
        case AMIGA_:
            tmp = (unsigned)(tmp>>17 & 7);   /* Amiga RWE bits */
            G.pInfo->file_attr = (unsigned)(tmp<<6 | tmp<<3 | tmp);
            break;
        /* all remaining cases:  expand MSDOS read-only bit into write perms */
        case FS_FAT_:
        case FS_HPFS_:
        case FS_NTFS_:
        case MAC_:
        case TOPS20_:
        default:
            tmp = !(tmp & 1) << 1;   /* read-only bit --> write perms bits */
            G.pInfo->file_attr = (unsigned)(0444 | tmp<<6 | tmp<<3 | tmp);
            break;
    } /* end switch (host-OS-created-by) */

    /* for originating systems with no concept of "group," "other," "system": */
    umask( (int)(tmp=umask(0)) );    /* apply mask to expanded r/w(/x) perms */
    G.pInfo->file_attr &= ~tmp;

    return 0;

} /* end function mapattr() */



/************************/
/*  Function mapname()  */
/************************/
                             /* return 0 if no error, 1 if caution (filename */
int mapname(__G__ renamed)   /*  truncated), 2 if warning (skip file because */
    __GDEF                   /*  dir doesn't exist), 3 if error (skip file), */
    int renamed;             /*  or 10 if out of memory (skip file) */
{                            /*  [also IZ_VOL_LABEL, IZ_CREATED_DIR] */
    char pathcomp[FILNAMSIZ];    /* path-component buffer */
    char *pp, *cp=(char *)NULL;  /* character pointers */
    char *lastsemi=(char *)NULL; /* pointer to last semi-colon in pathcomp */
    int quote = FALSE;           /* flags */
    int error = 0;
    register unsigned workch;    /* hold the character being tested */


/*---------------------------------------------------------------------------
    Initialize various pointers and counters and stuff.
  ---------------------------------------------------------------------------*/

    if (G.pInfo->vollabel)
        return IZ_VOL_LABEL;    /* can't set disk volume labels in Unix */

    /* can create path as long as not just freshening, or if user told us */
    G.create_dirs = (!G.fflag || renamed);

    created_dir = FALSE;        /* not yet */

    /* user gave full pathname:  don't prepend rootpath */
    renamed_fullpath = (renamed && (*G.filename == '/'));

    if (checkdir(__G__ (char *)NULL, INIT) == 10)
        return 10;              /* initialize path buffer, unless no memory */

    *pathcomp = '\0';           /* initialize translation buffer */
    pp = pathcomp;              /* point to translation buffer */
    if (G.jflag)                /* junking directories */
        cp = (char *)strrchr(G.filename, '/');
    if (cp == (char *)NULL)     /* no '/' or not junking dirs */
        cp = G.filename;        /* point to internal zipfile-member pathname */
    else
        ++cp;                   /* point to start of last component of path */

#ifdef QDOS
    if(*cp == '.' && *(cp+1) == '/')
    {
        cp += 2;
    }
#endif
/*---------------------------------------------------------------------------
    Begin main loop through characters in filename.
  ---------------------------------------------------------------------------*/

    while ((workch = (uch)*cp++) != 0) {

        if (quote) {                 /* if character quoted, */
            *pp++ = (char)workch;    /*  include it literally */
            quote = FALSE;
        } else
            switch (workch) {
            case '/':             /* can assume -j flag not given */
                *pp = '\0';
                if ((error = checkdir(__G__ pathcomp, APPEND_DIR)) > 1)
                    return error;
                pp = pathcomp;    /* reset conversion buffer for next piece */
                lastsemi = (char *)NULL; /* leave directory semi-colons alone */
                break;

            case ';':             /* VMS version (or DEC-20 attrib?) */
                lastsemi = pp;
                *pp++ = ';';      /* keep for now; remove VMS ";##" */
                break;            /*  later, if requested */

            case '\026':          /* control-V quote for special chars */
                quote = TRUE;     /* set flag for next character */
                break;

            case '.':
                if((qlflag & 1) == 0)
                {
                    *pp++ = '_';
                }
                else
                {
                    *pp++ = '.';
                }
                break;

            default:
                /* allow European characters in filenames: */
                if (isprint(workch) || (128 <= workch && workch <= 254))
                    *pp++ = (char)workch;
            } /* end switch */

    } /* end while loop */

    *pp = '\0';                   /* done with pathcomp:  terminate it */

    /* if not saving them, remove VMS version numbers (appended ";###") */
    if (!G.V_flag && lastsemi) {
        pp = lastsemi + 1;
        while (isdigit((uch)(*pp)))
            ++pp;
        if (*pp == '\0')          /* only digits between ';' and end:  nuke */
            *lastsemi = '\0';
    }

/*---------------------------------------------------------------------------
    Report if directory was created (and no file to create:  filename ended
    in '/'), check name to be sure it exists, and combine path and name be-
    fore exiting.
  ---------------------------------------------------------------------------*/

    if (G.filename[strlen(G.filename) - 1] == '/') {
        G.filename[strlen(G.filename) - 1] = '_';
        checkdir(__G__ G.filename, GETPATH);
        if (created_dir) {
            if (QCOND2) {
                Info(slide, 0, ((char *)slide, "   creating: %s\n",
                  G.filename));
            }
            return IZ_CREATED_DIR;   /* set dir time (note trailing '/') */
        }
        return 2;   /* dir existed already; don't look for data to extract */
    }

    if (*pathcomp == '\0') {
        Info(slide, 1, ((char *)slide, "mapname:  conversion of %s failed\n",
          G.filename));
        return 3;
    }

    checkdir(__G__ pathcomp, APPEND_NAME);  /* returns 1 if truncated: care? */
    checkdir(__G__ G.filename, GETPATH);

    return error;

} /* end function mapname() */




/***********************/
/* Function checkdir() */
/***********************/

int checkdir(__G__ pathcomp, flag)
    __GDEF
    char *pathcomp;
    int flag;
/*
 * returns:  1 - (on APPEND_NAME) truncated filename
 *           2 - path doesn't exist, not allowed to create
 *           3 - path doesn't exist, tried to create and failed; or
 *               path exists and is not a directory, but is supposed to be
 *           4 - path is too long
 *          10 - can't allocate memory for filename buffers
 */
{
    static int rootlen = 0;   /* length of rootpath */
    static char *rootpath;    /* user's "extract-to" directory */
    static char *buildpath;   /* full path (so far) to extracted file */
    static char *end;         /* pointer to end of buildpath ('\0') */

#   define FN_MASK   7
#   define FUNCTION  (flag & FN_MASK)


/*---------------------------------------------------------------------------
    APPEND_DIR:  append the path component to the path being built and check
    for its existence.  If doesn't exist and we are creating directories, do
    so for this one; else signal success or error as appropriate.
  ---------------------------------------------------------------------------*/

    if (FUNCTION == APPEND_DIR) {
        int too_long = FALSE;
#ifdef SHORT_NAMES
        char *old_end = end;
#endif

        Trace((stderr, "appending dir segment [%s]\n", pathcomp));
        while ((*end = *pathcomp++) != '\0')
            ++end;
#ifdef SHORT_NAMES   /* path components restricted to 14 chars, typically */
        if ((end-old_end) > FILENAME_MAX)  /* GRR:  proper constant? */
            *(end = old_end + FILENAME_MAX) = '\0';
#endif

        /* GRR:  could do better check, see if overrunning buffer as we go:
         * check end-buildpath after each append, set warning variable if
         * within 20 of FILNAMSIZ; then if var set, do careful check when
         * appending.  Clear variable when begin new path. */

        if ((end-buildpath) > FILNAMSIZ-2)  /* need '/', one-char name, '\0' */
            too_long = TRUE;                /* check if extracting directory? */
        if (stat(buildpath, &G.statbuf)) {    /* path doesn't exist */
            if (!G.create_dirs) {   /* told not to create (freshening) */
                free(buildpath);
                return 2;         /* path doesn't exist:  nothing to do */
            }
            if (too_long) {
                Info(slide, 1, ((char *)slide,
                  "checkdir error:  path too long: %s\n", buildpath));
                free(buildpath);
                return 4;         /* no room for filenames:  fatal */
            }
            if (mkdir(buildpath, 0777) == -1) {   /* create the directory */
                Info(slide, 1, ((char *)slide,
                  "checkdir error:  cannot create %s\n\
                 unable to process %s.\n", buildpath, G.filename));
                free(buildpath);
                return 3;      /* path didn't exist, tried to create, failed */
            }
            created_dir = TRUE;
        } else if (!S_ISDIR(G.statbuf.st_mode)) {
            Info(slide, 1, ((char *)slide,
              "checkdir error:  %s exists but is not directory\n\
                 unable to process %s.\n", buildpath, G.filename));
            free(buildpath);
            return 3;          /* path existed but wasn't dir */
        }
        if (too_long) {
            Info(slide, 1, ((char *)slide,
              "checkdir error:  path too long: %s\n", buildpath));
            free(buildpath);
            return 4;         /* no room for filenames:  fatal */
        }
        *end++ = '_';
        *end = '\0';
        Trace((stderr, "buildpath now = [%s]\n", buildpath));
        return 0;

    } /* end if (FUNCTION == APPEND_DIR) */

/*---------------------------------------------------------------------------
    GETPATH:  copy full path to the string pointed at by pathcomp, and free
    buildpath.
  ---------------------------------------------------------------------------*/

    if (FUNCTION == GETPATH) {
        strcpy(pathcomp, buildpath);
        Trace((stderr, "getting and freeing path [%s]\n", pathcomp));
        free(buildpath);
        buildpath = end = (char *)NULL;
        return 0;
    }

/*---------------------------------------------------------------------------
    APPEND_NAME:  assume the path component is the filename; append it and
    return without checking for existence.
  ---------------------------------------------------------------------------*/

    if (FUNCTION == APPEND_NAME) {
#ifdef SHORT_NAMES
        char *old_end = end;
#endif
        short dlen;

        Trace((stderr, "appending filename [%s]\n", pathcomp));
        while ((*end = *pathcomp++) != '\0') {
            ++end;
#ifdef SHORT_NAMES  /* truncate name at 14 characters, typically */
            if ((end-old_end) > FILENAME_MAX)      /* GRR:  proper constant? */
                *(end = old_end + FILENAME_MAX) = '\0';
#endif
            if(isdirdev(buildpath))
            {
                dlen = 5;
            }
            else
            {
                dlen = 0;
            }

            if ((end-buildpath-dlen) >= FILNAMSIZ) {
                *--end = '\0';
                Info(slide, 0x201, ((char *)slide,
                  "checkdir warning:  path too long; truncating\n\
                   %s\n                -> %s\n", G.filename, buildpath));
                return 1;   /* filename truncated */
            }
        }
        Trace((stderr, "buildpath now = [%s]\n", buildpath));
        return 0;  /* could check for existence here, prompt for new name... */
    }

/*---------------------------------------------------------------------------
    INIT:  allocate and initialize buffer space for the file currently being
    extracted.  If file was renamed with an absolute path, don't prepend the
    extract-to path.
  ---------------------------------------------------------------------------*/

/* GRR:  for VMS and TOPS-20, add up to 13 to strlen */

    if (FUNCTION == INIT) {
        Trace((stderr, "initializing buildpath to "));
        if ((buildpath = (char *)malloc(strlen(G.filename)+rootlen+1)) ==
            (char *)NULL)
            return 10;
        if ((rootlen > 0) && !renamed_fullpath) {
            strcpy(buildpath, rootpath);
            end = buildpath + rootlen;
        } else {
            *buildpath = '\0';
            end = buildpath;
        }
        Trace((stderr, "[%s]\n", buildpath));
        return 0;
    }

/*---------------------------------------------------------------------------
    ROOT:  if appropriate, store the path in rootpath and create it if neces-
    sary; else assume it's a zipfile member and return.  This path segment
    gets used in extracting all members from every zipfile specified on the
    command line.
  ---------------------------------------------------------------------------*/

#if (!defined(SFX) || defined(SFX_EXDIR))
    if (FUNCTION == ROOT) {
        Trace((stderr, "initializing root path to [%s]\n", pathcomp));
        if (pathcomp == (char *)NULL) {
            rootlen = 0;
            return 0;
        }
        if ((rootlen = strlen(pathcomp)) > 0) {

            if (rootlen > 0 && (stat(pathcomp, &G.statbuf) ||
                !S_ISDIR(G.statbuf.st_mode)))          /* path does not exist */
            {
                if (!G.create_dirs                     /* || iswild(pathcomp) */
#ifdef OLD_EXDIR
                                 || !had_trailing_pathsep
#endif
                                                         ) {
                    rootlen = 0;
                    return 2;   /* skip (or treat as stored file) */
                }
                /* create the directory (could add loop here to scan pathcomp
                 * and create more than one level, but why really necessary?) */

                if (mkdir(pathcomp, 0777) == -1) {
                    Info(slide, 1, ((char *)slide,
                      "checkdir:  cannot create extraction directory: %s\n",
                      pathcomp));
                    rootlen = 0;   /* path didn't exist, tried to create, and */
                    return 3;  /* failed:  file exists, or 2+ levels required */
                }
            }
            if (pathcomp[rootlen-1] == '/' || pathcomp[rootlen-1] == '_') {
                pathcomp[--rootlen] = '\0';
            }
            if ((rootpath = (char *)malloc(rootlen+2)) == (char *)NULL) {
                rootlen = 0;
                return 10;
            }
            strcpy(rootpath, pathcomp);
            rootpath[rootlen++] = '_';
            rootpath[rootlen] = '\0';
        }
        Trace((stderr, "rootpath now = [%s]\n", rootpath));
        return 0;
    }
#endif /* !SFX || SFX_EXDIR */

/*---------------------------------------------------------------------------
    END:  free rootpath, immediately prior to program exit.
  ---------------------------------------------------------------------------*/

    if (FUNCTION == END) {
        Trace((stderr, "freeing rootpath\n"));
        if (rootlen > 0) {
            free(rootpath);
            rootlen = 0;
        }
        return 0;
    }

    return 99;  /* should never reach */

} /* end function checkdir() */


static void qfix(__G__ ef_ptr, ef_len)
    __GDEF
    uch *ef_ptr;
    unsigned ef_len;
{

    while (ef_len >= EB_HEADSIZE)
    {
        qdosextra   *extra = (qdosextra *)ef_ptr;
        jbextra     *jbp   = (jbextra   *)ef_ptr;
        unsigned    eb_len = makeword(EB_LEN + ef_ptr);

        if (eb_len > (ef_len - EB_HEADSIZE)) {
            /* discovered some extra field inconsistency! */
            Trace((stderr,
              "qfix: block length %u > rest ef_size %u\n", eb_len,
              ef_len - EB_HEADSIZE));
            break;
        }

        switch (extra->shortid) {
          case SHORTID:
            if (!strncmp(extra->longid, LONGID, strlen(LONGID)))
            {
                if (eb_len != EXTRALEN)
                    fputs("warning: invalid length in Qdos field", stderr);
                if (extra->header.d_type)
                {
                    fs_heads(fgetchid(G.outfile), (timeout_t)-1,
                             &extra->header, 14);
                    G.pInfo->file_attr |= S_IXUSR;
                }
            }

            if (!strncmp(jbp->longid, JBLONGID, strlen(JBLONGID)))
            {
                if (eb_len != JBEXTRALEN)
                    fputs("warning: invalid length in QZ field", stderr);
                if (jbp->header.d_type)
                {
                    fs_heads(fgetchid(G.outfile), (timeout_t)-1,
                             &jbp->header, 14);
                    G.pInfo->file_attr |= S_IXUSR;
                }
            }
            break;

          default:
            Trace((stderr,"qfix: unknown extra field block, ID=%d\n",
               extra->shortid));
            break;
        }

        /* Skip this extra field block */
        ef_ptr += (eb_len + EB_HEADSIZE);
        ef_len -= (eb_len + EB_HEADSIZE);
    }
}


#ifdef QDOS
#  include <utime.h>
   long timezone = 0;
#endif


/****************************/
/* Function close_outfile() */
/****************************/

void close_outfile(__G)
    __GDEF
{
    iztimes zt;
#ifdef USE_EF_UT_TIME
    unsigned eb_izux_flg;
#endif

    if (G.extra_field) {
        qfix(__G__ G.extra_field, G.lrec.extra_field_length);
    }

    fclose(G.outfile);

/*---------------------------------------------------------------------------
    Change the file permissions from default ones to those stored in the
    zipfile.
  ---------------------------------------------------------------------------*/

#ifndef NO_CHMOD
    if (chmod(G.filename, 0xffff & G.pInfo->file_attr))
        perror("chmod (file attributes) error");
#endif

/*---------------------------------------------------------------------------
    Convert from MSDOS-format local time and date to Unix-format 32-bit GMT
    time:  adjust base year from 1980 to 1970, do usual conversions from
    yy/mm/dd hh:mm:ss to elapsed seconds, and account for timezone and day-
    light savings time differences.  If we have a Unix extra field, however,
    we're laughing:  both mtime and atime are ours.
  ---------------------------------------------------------------------------*/

#ifdef USE_EF_UT_TIME
    eb_izux_flg = (G.extra_field ? ef_scan_for_izux(G.extra_field,
                   G.lrec.extra_field_length, 0, G.lrec.last_mod_file_date,
                   &zt, z_uidgid) : 0);
    if (eb_izux_flg & EB_UT_FL_MTIME) {
        TTrace((stderr, "\nclose_outfile:  Unix e.f. modif. time = %ld\n",
          zt.mtime));
    } else {
        zt.mtime = dos_to_unix_time(G.lrec.last_mod_file_date,
                                    G.lrec.last_mod_file_time);
    }
    if (eb_izux_flg & EB_UT_FL_ATIME) {
        TTrace((stderr, "close_outfile:  Unix e.f. access time = %ld\n",
          zt.atime));
    } else {
        zt.atime = zt.mtime;

        TTrace((stderr, "\nclose_outfile:  modification/access times = %ld\n",
          zt.mtime));
    }
#else
    zt.atime = zt.mtime = dos_to_unix_time(G.lrec.last_mod_file_date,
                                           G.lrec.last_mod_file_time);
#endif

    /* set the file's access and modification times */
    if (utime(G.filename, (struct utimbuf *)&zt)) {
        Info(slide, 0x201, ((char *)slide,
          "warning:  cannot set the time for %s\n", G.filename));
    }

} /* end function close_outfile() */




#ifdef TIMESTAMP

/***************************/
/*  Function stamp_file()  */
/***************************/

int stamp_file(fname, modtime)
    ZCONST char *fname;
    time_t modtime;
{
    struct utimbuf tp;

    tp.modtime = tp.actime = modtime;
    return (utime(fname, &tp));

} /* end function stamp_file() */

#endif /* TIMESTAMP */




#ifndef SFX

/************************/
/*  Function version()  */
/************************/

void version(__G)
    __GDEF
{

    sprintf((char *)slide, LoadFarString(CompiledWith),
           "c68", " v4.2x", "SMS/QDOS",
            " on ", __DATE__, "","");
    (*G.message)((zvoid *)&G, slide, (ulg)strlen((char *)slide), 0);

} /* end function version() */

#endif /* !SFX */
#endif /* !FUNZIP */

#if CRYPT

char *getp(__G__ m, p, n)
    __GDEF
    const char *m;              /* prompt for password */
    char *p;                    /* return value: line input */
    int n;                      /* bytes available in p[] */
{
    int c;                      /* one-byte buffer for read() to use */
    int i;                      /* number of characters input */
    char *w;                    /* warning on retry */

    /* get password */
    w = "";
    sd_cure(getchid(0), -1);    /* enable cursor */
    do {
        fputs(w, stderr);       /* warning if back again */
        fputs(m, stderr);       /* display prompt and flush */
        fflush(stderr);
        i = 0;
        do {
            c = getch();
            if (c == 0xc2) {
                if (i > 0) {
                    i--; /* the `del' keys works */
                    fputs("\b \b", stderr);
                }
            }
            else if (i < n) {
                p[i++] = c;     /* truncate past n */
                if(c != '\n') putc('*', stderr);
            }
        } while (c != '\n');

        putc('\n', stderr);  fflush(stderr);
        w = "(line too long--try again)\n";
    } while (p[i-1] != '\n');

    p[i-1] = 0;                 /* terminate at newline */
    sd_curs(getchid(0), -1);    /* suppress cursor */
    return p;                   /* return pointer to password */

} /* end function getp() */

#endif /* CRYPT */

