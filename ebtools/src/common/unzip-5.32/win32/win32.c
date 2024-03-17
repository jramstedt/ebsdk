/*---------------------------------------------------------------------------

  win32.c

  32-bit Windows-specific (NT/95) routines for use with Info-ZIP's UnZip 5.3
  and later.

  Contains:  GetLoadPath()
             Opendir()
             Readdir()
             Closedir()
             process_defer_NT()   process any deferred items
             SetSD()              set security descriptor on file
             EvalExtraFields()    evaluate and process and extra field NOW
             IsWinNT()            indicate type of WIN32 platform
             test_NTSD()          test integrity of NT security data
             utime2FileTime()
             NTQueryTargetFS()
             UTCtime2Localtime()
             NTtzbugWorkaround()
             getNTfiletime()
             close_outfile()
             stamp_file()
             isfloppy()
             IsVolumeOldFAT()
             IsFileNameValid()
             do_wild()
             mapattr()
             mapname()
             map2fat()
             checkdir()
             version()
             stat_bandaid()       [Watcom only]
             getch()              [Watcom only]

  ---------------------------------------------------------------------------*/


#define UNZIP_INTERNAL
#include "unzip.h"
#include <windows.h>   /* must be AFTER unzip.h to avoid struct G problems */
#ifdef __RSXNT__
#  include "win32/rsxntwin.h"
#endif
#include "win32/nt.h"


#if (defined(__GO32__) || defined(__EMX__))
#  include <dirent.h>        /* use readdir() */
#  define MKDIR(path,mode)   mkdir(path,mode)
#  define Opendir  opendir
#  define Readdir  readdir
#  define Closedir closedir
#  define zdirent  dirent
#  define zDIR     DIR
#else /* !(__GO32__ || __EMX__) */
#  define MKDIR(path,mode)   mkdir(path)

   typedef struct zdirent {
       char    reserved [21];
       char    ff_attrib;
       short   ff_ftime;
       short   ff_fdate;
       long    size;
       char    d_name[MAX_PATH];
       int     d_first;
       HANDLE  d_hFindFile;
   } zDIR;

   static zDIR           *Opendir  (const char *n);
   static struct zdirent *Readdir  (zDIR *d);
   static void            Closedir (zDIR *d);
#endif /* ?(__GO32__ || __EMX__) */


/* Function prototypes */
#ifdef NTSD_EAS
   static int  SetSD(__GPRO__ char *path, PVOLUMECAPS VolumeCaps,
                     uch *eb_ptr, unsigned eb_len);
   static int  EvalExtraFields(__GPRO__ char *path, uch *ef_ptr,
                               unsigned ef_len);
#endif

#if (defined(USE_EF_UT_TIME) || defined(NT_TZBUG_WORKAROUND) || \
     defined(TIMESTAMP))
   static void utime2FileTime(time_t ut, FILETIME *pft);
   static int NTQueryTargetFS(const char *path);
#endif /* USE_EF_UT_TIME || NT_TZBUG_WORKAROUND || TIMESTAMP */
#ifdef NT_TZBUG_WORKAROUND
   static time_t UTCtime2Localtime(time_t utctime);
   static void NTtzbugWorkaround(time_t ut, FILETIME *pft);
#endif /* NT_TZBUG_WORKAROUND */

static int  getNTfiletime   (__GPRO__ FILETIME *pModFT, FILETIME *pAccFT,
                             FILETIME *pCreFT);
static int  isfloppy        (int nDrive);
static int  IsVolumeOldFAT  (const char *name);
static int  IsFileNameValid (const char *name);
static void map2fat         (char *pathcomp, char **pEndFAT);


/* static int created_dir;      */     /* used by mapname(), checkdir() */
/* static int renamed_fullpath; */     /* ditto */
/* static int fnlen;            */     /* ditto */
/* static unsigned nLabelDrive; */     /* ditto */

extern char Far TruncNTSD[];    /* in extract.c */



#ifdef SFX

/**************************/
/* Function GetLoadPath() */
/**************************/

char *GetLoadPath(__GPRO)
{
#ifdef MSC
    extern char *_pgmptr;
    return _pgmptr;

#else    /* use generic API call */

    GetModuleFileName(NULL, G.filename, FILNAMSIZ-1);
    _ISO_INTERN(G.filename);    /* translate to codepage of C rtl's stdio */
    return G.filename;
#endif

} /* end function GetLoadPath() */





#else /* !SFX */

#if (!defined(__GO32__) && !defined(__EMX__))

/**********************/        /* Borrowed from ZIP 2.0 sources            */
/* Function Opendir() */        /* Difference: no special handling for      */
/**********************/        /*             hidden or system files.      */

static zDIR *Opendir(n)
    const char *n;          /* directory to open */
{
    zDIR *d;                /* malloc'd return value */
    char *p;                /* malloc'd temporary string */
    WIN32_FIND_DATA fd;
    int len = strlen(n);

    /* Start searching for files in the MSDOS directory n */

    if ((d = (zDIR *)malloc(sizeof(zDIR))) == NULL ||
        (p = malloc(strlen(n) + 5)) == NULL)
    {
        if (d != (zDIR *)NULL)
            free((void *)d);
        return (zDIR *)NULL;
    }
    INTERN_TO_ISO(n, p);
    if (p[len-1] == ':')
        p[len++] = '.';   /* x: => x:. */
    else if (p[len-1] == '/' || p[len-1] == '\\')
        --len;            /* foo/ => foo */
    strcpy(p+len, "/*");

    if (INVALID_HANDLE_VALUE == (d->d_hFindFile = FindFirstFile(p, &fd))) {
        free((zvoid *)d);
        free((zvoid *)p);
        return NULL;
    }
    strcpy(d->d_name, fd.cFileName);

    free((zvoid *)p);
    d->d_first = 1;
    return d;

} /* end of function Opendir() */




/**********************/        /* Borrowed from ZIP 2.0 sources            */
/* Function Readdir() */        /* Difference: no special handling for      */
/**********************/        /*             hidden or system files.      */

static struct zdirent *Readdir(d)
    zDIR *d;                    /* directory stream from which to read */
{
    /* Return pointer to first or next directory entry, or NULL if end. */

    if ( d->d_first )
        d->d_first = 0;
    else
    {
        WIN32_FIND_DATA fd;

        if ( !FindNextFile(d->d_hFindFile, &fd) )
            return NULL;

        ISO_TO_INTERN(fd.cFileName, d->d_name);
    }
    return (struct zdirent *)d;

} /* end of function Readdir() */




/***********************/
/* Function Closedir() */       /* Borrowed from ZIP 2.0 sources */
/***********************/

static void Closedir(d)
    zDIR *d;                    /* directory stream to close */
{
    FindClose(d->d_hFindFile);
    free(d);
}

#endif /* !__GO32__ && !__EMX__ */
#endif /* ?SFX */




#ifdef NTSD_EAS

/*********************************/
/*  Function process_defer_NT()  */
/*********************************/

void process_defer_NT(__G)
    __GDEF
{
    /* process deferred items */

    unsigned long dir, bytes;
    unsigned long dirfail, bytesfail;

#ifndef NO_NTSD_WITH_RSXNT
    ProcessDefer(&dir, &bytes, &dirfail, &bytesfail);
#else
    dir = bytes = dirfail = bytesfail = 0L;
#endif

    if (!G.tflag && (G.qflag < 2)) {
        if (dir)
            Info(slide, 0, ((char *)slide,
              "    updated: %lu directory entries with %lu bytes security",
              dir, bytes));
        if (dirfail)
            Info(slide, 0, ((char *)slide,
              "     failed: %lu directory entries with %lu bytes security",
              dirfail, bytesfail));
    }
}



/**********************/
/*  Function SetSD()  */   /* return almost-PK errors */
/**********************/

static int SetSD(__G__ path, VolumeCaps, eb_ptr, eb_len)
    __GDEF
    char *path;
    PVOLUMECAPS VolumeCaps;
    uch *eb_ptr;
    unsigned eb_len;
{
    ulg ntsd_ucSize;
    uch *security_data;
    int error;

    if (eb_ptr == NULL || eb_len < EB_NTSD_L_LEN)
        return PK_OK;  /* not a valid NTSD extra field:  assume OK */

    /* check if we know how to handle this version */
    if (*(eb_ptr + (EB_HEADSIZE+EB_NTSD_VERSION)) > (uch)EB_NTSD_MAX_VER)
        return PK_OK;

    ntsd_ucSize = makelong(eb_ptr + (EB_HEADSIZE+EB_UCSIZE_P));
    if (ntsd_ucSize > 0L && eb_len <= (EB_NTSD_L_LEN + EB_CMPRHEADLEN))
        return IZ_EF_TRUNC;               /* no compressed data! */

    /* allocate storage for uncompressed data */
    security_data = (uch *)malloc((extent)ntsd_ucSize);
    if (security_data == (uch *)NULL)
        return PK_MEM4;

    error = memextract(__G__ security_data, ntsd_ucSize,
      (eb_ptr + (EB_HEADSIZE+EB_NTSD_L_LEN)), (ulg)(eb_len - EB_NTSD_L_LEN));

    if (error == PK_OK) {
#ifdef NO_NTSD_WITH_RSXNT
        Info(slide, 0, ((char *)slide,
          "%s port does not yet support setting security", "RSXNT"));
        /* error = PK_OK; */  /* GRR:  change to PK_WARN? */
#else /* !NO_NTSD_WITH_RSXNT */
        if (SecuritySet(path, VolumeCaps, security_data)) {
            error = PK_COOL;
            if (!G.tflag && (G.qflag < 2) &&
                (!(VolumeCaps->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)))
                Info(slide, 0, ((char *)slide, " (%ld bytes security)",
                  ntsd_ucSize));
        }
#endif /* ?NO_NTSD_WITH_RSXNT */
    }

    free(security_data);
    return error;
}




/********************************/   /* scan extra fields for something */
/*  Function EvalExtraFields()  */   /*  we happen to know */
/********************************/

static int EvalExtraFields(__G__ path, ef_ptr, ef_len)
    __GDEF
    char *path;
    uch *ef_ptr;
    unsigned ef_len;
{
    int rc = PK_OK;

    if (!G.X_flag)
        return PK_OK;  /* user said don't process ACLs; for now, no other
                          extra block types are handled here */

    while (ef_len >= EB_HEADSIZE)
    {
        unsigned eb_id = makeword(EB_ID + ef_ptr);
        unsigned eb_len = makeword(EB_LEN + ef_ptr);

        if (eb_len > (ef_len - EB_HEADSIZE)) {
            /* discovered some extra field inconsistency! */
            Trace((stderr,
              "EvalExtraFields: block length %u > rest ef_size %u\n", eb_len,
              ef_len - EB_HEADSIZE));
            break;
        }

        switch (eb_id)
        {
            /* process security descriptor extra data if:
                 Caller is WinNT AND
                 Target local/remote drive supports acls AND
                 Target file is not a directory (else we defer processing
                   until later)
             */
            case EF_NTSD:
                if (IsWinNT()) {
                    VOLUMECAPS VolumeCaps;

                    /* provide useful input */
                    VolumeCaps.dwFileAttributes = G.pInfo->file_attr;
                    VolumeCaps.bUsePrivileges = (G.X_flag > 1);

#ifndef NO_NTSD_WITH_RSXNT
                    /* check target volume capabilities - just fall through
                     * and try if fail */
                    if (GetVolumeCaps(G.rootpath, path, &VolumeCaps) &&
                        !(VolumeCaps.dwFileSystemFlags & FS_PERSISTENT_ACLS))
                    {
                        rc = PK_OK;
                        break;
                    }
#endif
                    rc = SetSD(__G__ path, &VolumeCaps, ef_ptr, eb_len);
                } else
                    rc = PK_OK;
                break;

#if 0
            /* perhaps later we can add support for unzipping OS/2 EAs to NT */
            case EF_OS2:
                rc = SetEAs(__G__ path, ef_ptr);
                break;

            case EF_IZUNIX:
            case EF_IZUNIX2:
            case EF_TIME:
                break;          /* handled elsewhere */
#else /* ! 0 */
#ifdef DEBUG
            case EF_AV:
            case EF_OS2:
            case EF_PKVMS:
            case EF_PKUNIX:
            case EF_IZVMS:
            case EF_IZUNIX:
            case EF_IZUNIX2:
            case EF_TIME:
            case EF_JLMAC:
            case EF_ZIPIT:
            case EF_VMCMS:
            case EF_MVS:
            case EF_ACL:
            case EF_BEOS:
            case EF_QDOS:
            case EF_AOSVS:
            case EF_SPARK:
            case EF_MD5:
            case EF_ASIUNIX:
                break;          /* shut up for other known e.f. blocks  */
#endif /* DEBUG */
#endif /* ? 0 */

            default:
                Trace((stderr,
                  "EvalExtraFields: unknown extra field block, ID=%u\n",
                  eb_id));
                break;
        }

        ef_ptr += (eb_len + EB_HEADSIZE);
        ef_len -= (eb_len + EB_HEADSIZE);

        if (rc != PK_OK)
            break;
    }

    return rc;
}




#ifndef SFX

/**************************/
/*  Function test_NTSD()  */   /*  returns PK_WARN when NTSD data is invalid */
/**************************/

int test_NTSD(__G__ eb, eb_size, eb_ucptr, eb_ucsize)
    __GDEF
    uch *eb;
    unsigned eb_size;
    uch *eb_ucptr;
    ulg eb_ucsize;
{
    int r = PK_OK;

#ifndef NO_NTSD_WITH_RSXNT
    if (!ValidateSecurity(eb_ucptr))
        r = PK_WARN;
#endif
    return r;

} /* end function test_NTSD() */

#endif /* !SFX */
#endif /* NTSD_EAS */




/**********************/
/* Function IsWinNT() */
/**********************/

int IsWinNT(void)       /* returns TRUE if real NT, FALSE if Win95 or Win32s */
{
    static DWORD g_PlatformId = 0xFFFFFFFF; /* saved platform indicator */

    if (g_PlatformId == 0xFFFFFFFF) {
        /* note: GetVersionEx() doesn't exist on WinNT 3.1 */
        if (GetVersion() < 0x80000000)
            g_PlatformId = TRUE;
        else
            g_PlatformId = FALSE;
    }
    return (int)g_PlatformId;
}




#if (defined(USE_EF_UT_TIME) || defined(NT_TZBUG_WORKAROUND) || \
     defined(TIMESTAMP))

/*****************************/
/* Function utime2FileTime() */     /* convert Unix time_t format into the */
/*****************************/     /* form used by SetFileTime() in NT/95 */

#define UNIX_TIME_ZERO_HI  0x019DB1DE
#define UNIX_TIME_ZERO_LO  0xD53E8000
#define NT_QUANTA_PER_UNIX 10000000

static void utime2FileTime(time_t ut, FILETIME *pft)
{
#if defined(__GNUC__) || defined(ULONG_LONG_MAX)
    unsigned long long NTtime;

    NTtime = ((unsigned long long)ut * NT_QUANTA_PER_UNIX) +
             ((unsigned long long)UNIX_TIME_ZERO_LO +
              ((unsigned long long)UNIX_TIME_ZERO_HI << 32));
    pft->dwLowDateTime = (DWORD)NTtime;
    pft->dwHighDateTime = (DWORD)(NTtime >> 32);

#else /* "unsigned long long" may not be supported */
    unsigned int b1, b2, carry = 0;
    unsigned long r0, r1, r2, r3, r4;

    b1 = ut & 0xFFFF;
    b2 = (ut >> 16) & 0xFFFF;       /* if ut is over 32 bits, too bad */
    r1 = b1 * (NT_QUANTA_PER_UNIX & 0xFFFF);
    r2 = b1 * (NT_QUANTA_PER_UNIX >> 16);
    r3 = b2 * (NT_QUANTA_PER_UNIX & 0xFFFF);
    r4 = b2 * (NT_QUANTA_PER_UNIX >> 16);
    r0 = (r1 + (r2 << 16)) & 0xFFFFFFFF;
    if (r0 < r1)
        carry++;
    r1 = r0;
    r0 = (r0 + (r3 << 16)) & 0xFFFFFFFF;
    if (r0 < r1)
        carry++;
    pft->dwLowDateTime = r0 + UNIX_TIME_ZERO_LO;
    if (pft->dwLowDateTime < r0)
        carry++;
    pft->dwHighDateTime = r4 + (r2 >> 16) + (r3 >> 16)
                            + UNIX_TIME_ZERO_HI + carry;
#endif /* ?(64-bit "unsigned long long" support) */

} /* end function utime2FileTime() */



/******************************/
/* Function NTQueryTargetFS() */
/******************************/

static int NTQueryTargetFS(const char *path)
{
    char     *tmp0;
    char      rootPathName[4];
    char      tmp1[MAX_PATH], tmp2[MAX_PATH];
    unsigned  volSerNo, maxCompLen, fileSysFlags;
#ifdef __RSXNT__        /* RSXNT/EMX C rtl uses OEM charset */
    char *ansi_path = (char *)alloca(strlen(path) + 1);

    INTERN_TO_ISO(path, ansi_path);
    path = ansi_path;
#endif

    if (isalpha(path[0]) && (path[1] == ':'))
        tmp0 = (char *)path;
    else
    {
        GetFullPathName(path, MAX_PATH, tmp1, &tmp0);
        tmp0 = &tmp1[0];
    }
    strncpy(rootPathName, tmp0, 3);   /* Build the root path name, */
    rootPathName[3] = '\0';           /* e.g. "A:/"                */

    GetVolumeInformation((LPCTSTR)rootPathName, (LPTSTR)tmp1, (DWORD)MAX_PATH,
                         (LPDWORD)&volSerNo, (LPDWORD)&maxCompLen,
                         (LPDWORD)&fileSysFlags, (LPTSTR)tmp2, (DWORD)MAX_PATH);

    /* Volumes in (V)FAT and (OS/2) HPFS format store file timestamps in
     * local time!
     */
    return !strncmp(strupr(tmp2), "FAT", 3) ||
           !strncmp(tmp2, "VFAT", 4) ||
           !strncmp(tmp2, "HPFS", 4);

} /* end function NTQueryTargetFS() */

#endif /* USE_EF_UT_TIME || NT_TZBUG_WORKAROUND || TIMESTAMP */



#ifndef NT_TZBUG_WORKAROUND
#  define UTIME_BOUNDCHECK_1(utimval) \
     if (fs_uses_loctime) { \
         utime_dosmin = dos_to_unix_time((unsigned)DOSDATE_MINIMUM, 0); \
         if ((ulg)utimval < (ulg)utime_dosmin) \
             utimval = utime_dosmin; \
     }
#  define UTIME_BOUNDCHECK_N(utimval) \
     if (fs_uses_loctime && ((ulg)utimval < (ulg)utime_dosmin)) \
         utimval = utime_dosmin;
#  define NT_TZBUG_PRECOMPENSATE(ut, pft)

#else /* !NT_TZBUG_WORKAROUND */
#  define UTIME_1980_JAN_01_00_00   315532800L
#  define UTIME_BOUNDCHECK_1(utimval)
#  define UTIME_BOUNDCHECK_N(utimval)
#  define NT_TZBUG_PRECOMPENSATE(ut, pft) \
     if (fs_uses_loctime) NTtzbugWorkaround(ut, pft);

   /* nonzero if `y' is a leap year, else zero */
#  define leap(y) (((y)%4 == 0 && (y)%100 != 0) || (y)%400 == 0)
   /* number of leap years from 1970 to `y' (not including `y' itself) */
#  define nleap(y) (((y)-1969)/4 - ((y)-1901)/100 + ((y)-1601)/400)

extern ZCONST ush ydays[];

/********************************/
/* Function UTCtime2Localtime() */   /* borrowed from Zip's mkgmtime() */
/********************************/

static time_t UTCtime2Localtime(time_t utctime)
{
    time_t utc = utctime;
    struct tm *tm;
    unsigned years, months, days, hours, minutes, seconds;


#ifdef __BORLANDC__   /* Borland C++ 5.x crashes when trying to reference tm */
    if (utc < UTIME_1980_JAN_01_00_00)
        utc = UTIME_1980_JAN_01_00_00;
#endif
    tm = localtime(&utc);

    years = tm->tm_year + 1900; /* year - 1900 -> year */
    months = tm->tm_mon;        /* 0..11 */
    days = tm->tm_mday - 1;     /* 1..31 -> 0..30 */
    hours = tm->tm_hour;        /* 0..23 */
    minutes = tm->tm_min;       /* 0..59 */
    seconds = tm->tm_sec;       /* 0..61 in ANSI C */

    /* set `days' to the number of days into the year */
    days += ydays[months] + (months > 1 && leap(years));

    /* now set `days' to the number of days since 1 Jan 1970 */
    days += 365 * (years - 1970) + nleap(years);

    return (time_t)(86400L * (ulg)days + 3600L * (ulg)hours +
                    (ulg)(60 * minutes + seconds));

} /* end function UTCtime2Localtime() */



/********************************/
/* Function NTtzbugWorkaround() */
/********************************/

static void NTtzbugWorkaround(time_t ut, FILETIME *pft)
{
    FILETIME C_RTL_locft, NTAPI_locft;
    time_t ux_loctime = UTCtime2Localtime(ut);

    /* This routine is only used when the target file system stores time-
     * stamps as local time in MSDOS format.  Thus we make sure that the
     * resulting timestamp is within the range of MSDOS date-time values. */
    if (ux_loctime < UTIME_1980_JAN_01_00_00)
        ux_loctime = UTIME_1980_JAN_01_00_00;

    utime2FileTime(ux_loctime, &C_RTL_locft);
    if (!FileTimeToLocalFileTime(pft, &NTAPI_locft))
        return;
    else {
        long time_shift_l, time_shift_h;
        int carry = 0;

        time_shift_l = C_RTL_locft.dwLowDateTime - NTAPI_locft.dwLowDateTime;
        if (C_RTL_locft.dwLowDateTime < NTAPI_locft.dwLowDateTime)
            carry--;
        time_shift_h = C_RTL_locft.dwHighDateTime - NTAPI_locft.dwHighDateTime;
        pft->dwLowDateTime += time_shift_l;
        if (pft->dwLowDateTime < (ulg)time_shift_l)
            carry++;
        pft->dwHighDateTime += time_shift_h + carry;
    }
} /* end function NTtzbugWorkaround() */

#endif /* ?NT_TZBUG_WORKAROUND */



/****************************/      /* Get the file time in a format that */
/* Function getNTfiletime() */      /*  can be used by SetFileTime() in NT */
/****************************/

static int getNTfiletime(__G__ pModFT, pAccFT, pCreFT)
    __GDEF
    FILETIME *pModFT;
    FILETIME *pAccFT;
    FILETIME *pCreFT;
{
#ifdef NT_TZBUG_WORKAROUND
    time_t ux_modtime;
#else /* !NT_TZBUG_WORKAROUND */
    FILETIME locft;    /* 64-bit value made up of two 32-bit [low & high] */
    WORD wDOSDate;     /* for converting from DOS date to Windows NT */
    WORD wDOSTime;
#endif /* ?NT_TZBUG_WORKAROUND */
#ifdef USE_EF_UT_TIME
    unsigned eb_izux_flg;
    iztimes z_utime;   /* struct for Unix-style actime & modtime, + creatime */
#endif
#if (defined(USE_EF_UT_TIME) && !defined(NT_TZBUG_WORKAROUND))
    time_t utime_dosmin;
# endif
#if (defined(USE_EF_UT_TIME) || defined(NT_TZBUG_WORKAROUND))
    int fs_uses_loctime = NTQueryTargetFS(G.filename);
#endif

    /* Copy and/or convert time and date variables, if necessary;
     * return a flag indicating which time stamps are available. */
#ifdef USE_EF_UT_TIME
    if (G.extra_field &&
        ((eb_izux_flg = ef_scan_for_izux(G.extra_field,
          G.lrec.extra_field_length, 0, G.lrec.last_mod_file_date,
          &z_utime, NULL)) & EB_UT_FL_MTIME))
    {
        TTrace((stderr, "getNTfiletime:  Unix e.f. modif. time = %lu\n",
          z_utime.mtime));
        UTIME_BOUNDCHECK_1(z_utime.mtime)
        utime2FileTime(z_utime.mtime, pModFT);
        NT_TZBUG_PRECOMPENSATE(z_utime.mtime, pModFT)
        if (eb_izux_flg & EB_UT_FL_ATIME) {
            UTIME_BOUNDCHECK_N(z_utime.atime)
            utime2FileTime(z_utime.atime, pAccFT);
            NT_TZBUG_PRECOMPENSATE(z_utime.atime, pAccFT)
        }
        if (eb_izux_flg & EB_UT_FL_CTIME) {
            UTIME_BOUNDCHECK_N(z_utime.ctime)
            utime2FileTime(z_utime.ctime, pCreFT);
            NT_TZBUG_PRECOMPENSATE(z_utime.ctime, pCreFT)
        }
        return (int)eb_izux_flg;
    }
#endif /* USE_EF_UT_TIME */
#ifdef NT_TZBUG_WORKAROUND
    ux_modtime = dos_to_unix_time(G.lrec.last_mod_file_date,
                                  G.lrec.last_mod_file_time);
    utime2FileTime(ux_modtime, pModFT);
    NT_TZBUG_PRECOMPENSATE(ux_modtime, pModFT)
#else /* !NT_TZBUG_WORKAROUND */

    wDOSTime = (WORD)G.lrec.last_mod_file_time;
    wDOSDate = (WORD)G.lrec.last_mod_file_date;

    /* The DosDateTimeToFileTime() function converts a DOS date/time
     * into a 64-bit Windows NT file time */
    if (!DosDateTimeToFileTime(wDOSDate, wDOSTime, &locft))
    {
        Info(slide, 0, ((char *)slide, "DosDateTime failed: %d\n",
          (int)GetLastError()));
        return 0;
    }
    if (!LocalFileTimeToFileTime(&locft, pModFT))
    {
        Info(slide, 0, ((char *)slide, "LocalFileTime failed: %d\n",
          (int)GetLastError()));
        *pModFT = locft;
    }
#endif /* ?NT_TZBUG_WORKAROUND */
    *pAccFT = *pModFT;
    return (EB_UT_FL_MTIME | EB_UT_FL_ATIME);

} /* end function getNTfiletime() */




/****************************/
/* Function close_outfile() */
/****************************/

void close_outfile(__G)
    __GDEF
{
    FILETIME Modft;    /* File time type defined in NT, `last modified' time */
    FILETIME Accft;    /* NT file time type, `last access' time */
    FILETIME Creft;    /* NT file time type, `file creation' time */
    HANDLE hFile;      /* File handle defined in NT    */
    int gotTime;
#ifdef __RSXNT__        /* RSXNT/EMX C rtl uses OEM charset */
    char *ansi_name = (char *)alloca(strlen(G.filename) + 1);

    INTERN_TO_ISO(G.filename, ansi_name);
#   define Ansi_Fname  ansi_name
#else
#   define Ansi_Fname  G.filename
#endif

    /* don't set the time stamp on standard output */
    if (G.cflag) {
        fclose(G.outfile);
        return;
    }

    gotTime = getNTfiletime(__G__ &Modft, &Accft, &Creft);

    /* Close the file and then re-open it using the Win32
     * CreateFile call, so that the file can be created
     * with GENERIC_WRITE access, otherwise the SetFileTime
     * call will fail. */
    fclose(G.outfile);

    /* open a handle to the file before processing extra fields;
       we do this in case new security on file prevents us from updating
       time stamps */
    hFile = CreateFile(Ansi_Fname, GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
         OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    /* sfield@microsoft.com: set attributes before time in case we decide to
       support other filetime members later.  This also allows us to apply
       attributes before the security is changed, which may prevent this
       from succeeding otherwise.  Also, since most files don't have
       any interesting attributes, only change them if something other than
       FILE_ATTRIBUTE_ARCHIVE appears in the attributes.  This works well
       as an optimization because FILE_ATTRIBUTE_ARCHIVE gets applied to the
       file anyway, when it's created new. */
    if((G.pInfo->file_attr & 0x7F) & ~FILE_ATTRIBUTE_ARCHIVE) {
        if (!SetFileAttributes(Ansi_Fname, G.pInfo->file_attr & 0x7F))
            Info(slide, 1, ((char *)slide,
              "\nwarning (%d): could not set file attributes\n",
              (int)GetLastError()));
    }

#ifdef NTSD_EAS
    /* set extra fields, both stored-in-zipfile and .LONGNAME flavors */
    if (G.extra_field) {    /* zipfile extra field may have extended attribs */
        int err = EvalExtraFields(__G__ G.filename, G.extra_field,
                                  G.lrec.extra_field_length);

        if (err == IZ_EF_TRUNC) {
            if (G.qflag)
                Info(slide, 1, ((char *)slide, "%-22s ",
                  FnFilter1(G.filename)));
            Info(slide, 1, ((char *)slide, LoadFarString(TruncNTSD),
              makeword(G.extra_field+2)-10, G.qflag? "\n":""));
        }
    }
#endif /* NTSD_EAS */

    if ( hFile == INVALID_HANDLE_VALUE )
        Info(slide, 1, ((char *)slide,
          "\nCreateFile error %d when trying set file time\n",
          (int)GetLastError()));
    else {
        if (gotTime) {
            FILETIME *pModft = (gotTime & EB_UT_FL_MTIME) ? &Modft : NULL;
            FILETIME *pAccft = (gotTime & EB_UT_FL_ATIME) ? &Accft : NULL;
            FILETIME *pCreft = (gotTime & EB_UT_FL_CTIME) ? &Creft : NULL;

            if (!SetFileTime(hFile, pCreft, pAccft, pModft))
                Info(slide, 0, ((char *)slide, "\nSetFileTime failed: %d\n",
                  (int)GetLastError()));
        }
        CloseHandle(hFile);
    }

    return;

#undef Ansi_Fname

} /* end function close_outfile() */




#ifdef TIMESTAMP

/*************************/
/* Function stamp_file() */
/*************************/

int stamp_file(ZCONST char *fname, time_t modtime)
{
    FILETIME Modft;    /* File time type defined in NT, `last modified' time */
    HANDLE hFile;      /* File handle defined in NT    */
    int errstat = 0;   /* return status: 0 == "OK", -1 == "Failure" */
#ifndef NT_TZBUG_WORKAROUND
    time_t utime_dosmin;        /* internal variable for UTIME_BOUNDCHECK_1 */
#endif
    int fs_uses_loctime = NTQueryTargetFS(fname);
#ifdef __RSXNT__        /* RSXNT/EMX C rtl uses OEM charset */
    char *ansi_name = (char *)alloca(strlen(fname) + 1);

    INTERN_TO_ISO(fname, ansi_name);
#   define Ansi_Fname  ansi_name
#else
#   define Ansi_Fname  fname
#endif

    /* open a handle to the file to prepare setting the mod-time stamp */
    hFile = CreateFile(Ansi_Fname, GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
         OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if ( hFile == INVALID_HANDLE_VALUE ) {
        errstat = -1;
    } else {
        /* convert time_t modtime into WIN32 native 64bit format */
        UTIME_BOUNDCHECK_1(modtime)
        utime2FileTime(modtime, &Modft);
        NT_TZBUG_PRECOMPENSATE(modtime, &Modft)
        /* set Access and Modification times of the file to modtime */
        if (!SetFileTime(hFile, NULL, &Modft, &Modft)) {
            errstat = -1;
        }
        CloseHandle(hFile);
    }

    return errstat;

#undef Ansi_Fname
} /* end function stamp_file() */

#endif /* TIMESTAMP */




/***********************/
/* Function isfloppy() */   /* more precisely, is it removable? */
/***********************/

static int isfloppy(int nDrive)   /* 1 == A:, 2 == B:, etc. */
{
    char rootPathName[4];

    rootPathName[0] = (char)('A' + nDrive - 1);   /* build the root path */
    rootPathName[1] = ':';                        /*  name, e.g. "A:/" */
    rootPathName[2] = '/';
    rootPathName[3] = '\0';

    return (GetDriveType(rootPathName) == DRIVE_REMOVABLE);

} /* end function isfloppy() */




/*****************************/
/* Function IsVolumeOldFAT() */
/*****************************/

/*
 * Note:  8.3 limits on filenames apply only to old-style FAT filesystems.
 *        More recent versions of Windows (Windows NT 3.5 / Windows 4.0)
 *        can support long filenames (LFN) on FAT filesystems.  Check the
 *        filesystem maximum component length field to detect LFN support.
 *        [GRR:  this routine is only used to determine whether spaces in
 *        filenames are supported...]
 */

static int IsVolumeOldFAT(const char *name)
{
    char     *tmp0;
    char      rootPathName[4];
    char      tmp1[MAX_PATH], tmp2[MAX_PATH];
    unsigned  volSerNo, maxCompLen, fileSysFlags;
#ifdef __RSXNT__        /* RSXNT/EMX C rtl uses OEM charset */
    char *ansi_name = (char *)alloca(strlen(name) + 1);

    INTERN_TO_ISO(name, ansi_name);
    name = ansi_name;
#endif

    if (isalpha(name[0]) && (name[1] == ':'))
        tmp0 = (char *)name;
    else
    {
        GetFullPathName(name, MAX_PATH, tmp1, &tmp0);
        tmp0 = &tmp1[0];
    }
    strncpy(rootPathName, tmp0, 3);   /* Build the root path name, */
    rootPathName[3] = '\0';           /* e.g. "A:/"                */

    GetVolumeInformation((LPCTSTR)rootPathName, (LPTSTR)tmp1, (DWORD)MAX_PATH,
                         (LPDWORD)&volSerNo, (LPDWORD)&maxCompLen,
                         (LPDWORD)&fileSysFlags, (LPTSTR)tmp2, (DWORD)MAX_PATH);

    /* Long Filenames (LFNs) are available if the component length is > 12 */
    return maxCompLen <= 12;
/*  return !strncmp(strupr(tmp2), "FAT", 3);   old version */

}




/******************************/
/* Function IsFileNameValid() */
/******************************/

static int IsFileNameValid(const char *name)
{
    HFILE    hf;
    OFSTRUCT of;
#ifdef __RSXNT__        /* RSXNT/EMX C rtl uses OEM charset */
    char *ansi_name = (char *)alloca(strlen(name) + 1);

    INTERN_TO_ISO(name, ansi_name);
    name = ansi_name;
#endif

    hf = OpenFile(name, &of, OF_READ | OF_SHARE_DENY_NONE);
    if (hf == HFILE_ERROR)
        switch (GetLastError())
        {
            case ERROR_INVALID_NAME:
            case ERROR_FILENAME_EXCED_RANGE:
                return FALSE;
            default:
                return TRUE;
        }
    else
        _lclose(hf);
    return TRUE;
}




#ifndef SFX

/************************/
/*  Function do_wild()  */   /* identical to OS/2 version */
/************************/

char *do_wild(__G__ wildspec)
    __GDEF
    char *wildspec;         /* only used first time on a given dir */
{
 /* static zDIR *dir = NULL;                               */
 /* static char *dirname, *wildname, matchname[FILNAMSIZ]; */
 /* static int firstcall=TRUE, have_dirname, dirnamelen;   */
    struct zdirent *file;

    /* Even when we're just returning wildspec, we *always* do so in
     * matchname[]--calling routine is allowed to append four characters
     * to the returned string, and wildspec may be a pointer to argv[].
     */
    if (!G.notfirstcall) {  /* first call:  must initialize everything */
        G.notfirstcall = TRUE;

        if (!iswild(wildspec)) {
            strcpy(G.matchname, wildspec);
            G.have_dirname = FALSE;
            G.wild_dir = NULL;
            return G.matchname;
        }

        /* break the wildspec into a directory part and a wildcard filename */
        if ((G.wildname = strrchr(wildspec, '/')) == NULL &&
            (G.wildname = strrchr(wildspec, ':')) == NULL) {
            G.dirname = ".";
            G.dirnamelen = 1;
            G.have_dirname = FALSE;
            G.wildname = wildspec;
        } else {
            ++G.wildname;     /* point at character after '/' or ':' */
            G.dirnamelen = G.wildname - wildspec;
            if ((G.dirname = (char *)malloc(G.dirnamelen+1)) == NULL) {
                Info(slide, 1, ((char *)slide,
                  "warning:  cannot allocate wildcard buffers\n"));
                strcpy(G.matchname, wildspec);
                return G.matchname; /* but maybe filespec was not a wildcard */
            }
            strncpy(G.dirname, wildspec, G.dirnamelen);
            G.dirname[G.dirnamelen] = '\0';    /* terminate for strcpy below */
            G.have_dirname = TRUE;
        }
        Trace((stderr, "do_wild:  dirname = [%s]\n", G.dirname));

        if ((G.wild_dir = (zvoid *)Opendir(G.dirname)) != NULL) {
            while ((file = Readdir((zDIR *)G.wild_dir)) != NULL) {
                Trace((stderr, "do_wild:  Readdir returns %s\n", file->d_name));
                if (match(file->d_name, G.wildname, 1)) { /* 1 == ignore case */
                    Trace((stderr, "do_wild:  match() succeeds\n"));
                    if (G.have_dirname) {
                        strcpy(G.matchname, G.dirname);
                        strcpy(G.matchname+G.dirnamelen, file->d_name);
                    } else
                        strcpy(G.matchname, file->d_name);
                    return G.matchname;
                }
            }
            /* if we get to here directory is exhausted, so close it */
            Closedir((zDIR *)G.wild_dir);
            G.wild_dir = NULL;
        }
        Trace((stderr, "do_wild:  Opendir(%s) returns NULL\n", G.dirname));

        /* return the raw wildspec in case that works (e.g., directory not
         * searchable, but filespec was not wild and file is readable) */
        strcpy(G.matchname, wildspec);
        return G.matchname;
    }

    /* last time through, might have failed opendir but returned raw wildspec */
    if (G.wild_dir == NULL) {
        G.notfirstcall = FALSE;    /* reset for new wildspec */
        if (G.have_dirname)
            free(G.dirname);
        return (char *)NULL;
    }

    /* If we've gotten this far, we've read and matched at least one entry
     * successfully (in a previous call), so dirname has been copied into
     * matchname already.
     */
    while ((file = Readdir((zDIR *)G.wild_dir)) != NULL)
        if (match(file->d_name, G.wildname, 1)) {   /* 1 == ignore case */
            if (G.have_dirname) {
                /* strcpy(G.matchname, G.dirname); */
                strcpy(G.matchname+G.dirnamelen, file->d_name);
            } else
                strcpy(G.matchname, file->d_name);
            return G.matchname;
        }

    Closedir((zDIR *)G.wild_dir);  /* at least one entry read; nothing left */
    G.wild_dir = NULL;
    G.notfirstcall = FALSE;        /* reset for new wildspec */
    if (G.have_dirname)
        free(G.dirname);
    return (char *)NULL;

} /* end function do_wild() */

#endif /* !SFX */



/**********************/
/* Function mapattr() */
/**********************/

/* Identical to MS-DOS, OS/2 versions.  However, NT has a lot of extra
 * permission stuff, so this function should probably be extended in the
 * future. */

int mapattr(__G)
    __GDEF
{
    /* set archive bit (file is not backed up): */
    G.pInfo->file_attr = (unsigned)(G.crec.external_file_attributes | FILE_ATTRIBUTE_ARCHIVE) &
      0xff;
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
    char pathcomp[FILNAMSIZ];   /* path-component buffer */
    char *pp, *cp=NULL;         /* character pointers */
    char *lastsemi = NULL;      /* pointer to last semi-colon in pathcomp */
    int error;
    register unsigned workch;   /* hold the character being tested */


/*---------------------------------------------------------------------------
    Initialize various pointers and counters and stuff.
  ---------------------------------------------------------------------------*/

    /* can create path as long as not just freshening, or if user told us */
    G.create_dirs = (!G.fflag || renamed);

    G.created_dir = FALSE;      /* not yet */
    G.renamed_fullpath = FALSE;
    G.fnlen = strlen(G.filename);

    if (renamed) {
        cp = G.filename - 1;    /* point to beginning of renamed name... */
        while (*++cp)
            if (*cp == '\\')    /* convert backslashes to forward */
                *cp = '/';
        cp = G.filename;
        /* use temporary rootpath if user gave full pathname */
        if (G.filename[0] == '/') {
            G.renamed_fullpath = TRUE;
            pathcomp[0] = '/';  /* copy the '/' and terminate */
            pathcomp[1] = '\0';
            ++cp;
        } else if (isalpha(G.filename[0]) && G.filename[1] == ':') {
            G.renamed_fullpath = TRUE;
            pp = pathcomp;
            *pp++ = *cp++;      /* copy the "d:" (+ '/', possibly) */
            *pp++ = *cp++;
            if (*cp == '/')
                *pp++ = *cp++;  /* otherwise add "./"? */
            *pp = '\0';
        }
    }

    /* pathcomp is ignored unless renamed_fullpath is TRUE: */
    if ((error = checkdir(__G__ pathcomp, INIT)) != 0)    /* init path buffer */
        return error;           /* ...unless no mem or vol label on hard disk */

    *pathcomp = '\0';           /* initialize translation buffer */
    pp = pathcomp;              /* point to translation buffer */
    if (!renamed) {             /* cp already set if renamed */
        if (G.jflag)            /* junking directories */
            cp = (char *)strrchr(G.filename, '/');
        if (cp == NULL)         /* no '/' or not junking dirs */
            cp = G.filename;    /* point to internal zipfile-member pathname */
        else
            ++cp;               /* point to start of last component of path */
    }

/*---------------------------------------------------------------------------
    Begin main loop through characters in filename.
  ---------------------------------------------------------------------------*/

    while ((workch = (uch)*cp++) != 0) {

        switch (workch) {
        case '/':             /* can assume -j flag not given */
            *pp = '\0';
            if ((error = checkdir(__G__ pathcomp, APPEND_DIR)) > 1)
                return error;
            pp = pathcomp;    /* reset conversion buffer for next piece */
            lastsemi = NULL;  /* leave directory semi-colons alone */
            break;

        case ':':             /* drive names not stored in zipfile, */
        case '<':             /*  so no colons allowed */
        case '>':             /* no redirection symbols allowed either */
        case '|':             /* no pipe signs allowed */
        case '"':             /* no double quotes allowed */
        case '?':             /* no wildcards allowed */
        case '*':
            *pp++ = '_';      /* these rules apply equally to FAT and NTFS */
            break;
        case ';':             /* start of VMS version? */
            lastsemi = pp;    /* remove VMS version later... */
            *pp++ = ';';      /*  but keep semicolon for now */
            break;

        case ' ':             /* keep spaces unless specifically */
            /* NT cannot create filenames with spaces on FAT volumes */
            if (G.sflag || IsVolumeOldFAT(G.filename))
                *pp++ = '_';
            else
                *pp++ = ' ';
            break;

        default:
            /* allow European characters in filenames: */
            if (isprint(workch) || workch >= 127)
                *pp++ = (char)workch;
        } /* end switch */
    } /* end while loop */

    *pp = '\0';                   /* done with pathcomp:  terminate it */

    /* if not saving them, remove VMS version numbers (appended "###") */
    if (!G.V_flag && lastsemi) {
        pp = lastsemi + 1;        /* semi-colon was kept:  expect #'s after */
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

    if (G.filename[G.fnlen-1] == '/') {
        checkdir(__G__ G.filename, GETPATH);
        if (G.created_dir) {
            if (QCOND2) {
                Info(slide, 0, ((char *)slide, "   creating: %-22s\n",
                  FnFilter1(G.filename)));
            }
            /* HG: are we setting the date & time on a newly created   */
            /*     dir?  Not quite sure how to do this.  It does not   */
            /*     seem to be done in the MS-DOS version of mapname(). */

#ifdef NTSD_EAS
            /* set extra fields, both stored-in-zipfile and .LONGNAME flavors */
            if (G.extra_field) { /* zipfile e.f. may have extended attribs */
                int err = EvalExtraFields(__G__ G.filename, G.extra_field,
                                          G.lrec.extra_field_length);

                if (err == IZ_EF_TRUNC) {
                    if (G.qflag)
                        Info(slide, 1, ((char *)slide, "%-22s ",
                          FnFilter1(G.filename)));
                    Info(slide, 1, ((char *)slide, LoadFarString(TruncNTSD),
                      makeword(G.extra_field+2)-10, G.qflag? "\n":""));
                }
            }
#endif /* NTSD_EAS */
            return IZ_CREATED_DIR;      /* set dir time (note trailing '/') */
        }
        return 2;   /* dir existed already; don't look for data to extract */
    }

    if (*pathcomp == '\0') {
        Info(slide, 1, ((char *)slide, "mapname:  conversion of %s failed\n",
          FnFilter1(G.filename)));
        return 3;
    }

    checkdir(__G__ pathcomp, APPEND_NAME);  /* returns 1 if truncated: care? */
    checkdir(__G__ G.filename, GETPATH);
    Trace((stderr, "mapname returns with filename = [%s] (error = %d)\n\n",
      FnFilter1(G.filename), error));

    if (G.pInfo->vollabel) {    /* set the volume label now */
        char drive[4];
#ifdef __RSXNT__        /* RSXNT/EMX C rtl uses OEM charset */
        char *ansi_name = (char *)alloca(strlen(G.filename) + 1);
        INTERN_TO_ISO(G.filename, ansi_name);
#       define Ansi_Fname  ansi_name
#else
#       define Ansi_Fname  G.filename
#endif

        /* Build a drive string, e.g. "b:" */
        drive[0] = (char)('a' + G.nLabelDrive - 1);
        strcpy(drive + 1, ":\\");
        if (QCOND2)
            Info(slide, 0, ((char *)slide, "labelling %s %-22s\n", drive,
              FnFilter1(G.filename)));
        if (!SetVolumeLabel(drive, Ansi_Fname)) {
            Info(slide, 1, ((char *)slide,
              "mapname:  error setting volume label\n"));
            return 3;
        }
        return 2;   /* success:  skip the "extraction" quietly */
#undef Ansi_Fname
    }

    return error;

} /* end function mapname() */




/**********************/
/* Function map2fat() */        /* Not quite identical to OS/2 version */
/**********************/

static void map2fat(pathcomp, pEndFAT)
    char *pathcomp, **pEndFAT;
{
    char *ppc = pathcomp;       /* variable pointer to pathcomp */
    char *pEnd = *pEndFAT;      /* variable pointer to buildpathFAT */
    char *pBegin = *pEndFAT;    /* constant pointer to start of this comp. */
    char *last_dot = NULL;      /* last dot not converted to underscore */
    int dotname = FALSE;        /* flag:  path component begins with dot */
                                /*  ("." and ".." don't count) */
    register unsigned workch;   /* hold the character being tested */


    /* Only need check those characters which are legal in NTFS but not
     * in FAT:  to get here, must already have passed through mapname.
     * Also must truncate path component to ensure 8.3 compliance.
     */
    while ((workch = (uch)*ppc++) != 0) {
        switch (workch) {
            case '[':
            case ']':
            case '+':
            case ',':
            case ';':
            case '=':
                *pEnd++ = '_';      /* convert brackets to underscores */
                break;

            case '.':
                if (pEnd == *pEndFAT) {   /* nothing appended yet... */
                    if (*ppc == '\0')     /* don't bother appending a */
                        break;            /*  "./" component to the path */
                    else if (*ppc == '.' && ppc[1] == '\0') {   /* "../" */
                        *pEnd++ = '.';    /* add first dot, unchanged... */
                        ++ppc;            /* skip second dot, since it will */
                    } else {              /*  be "added" at end of if-block */
                        *pEnd++ = '_';    /* FAT doesn't allow null filename */
                        dotname = TRUE;   /*  bodies, so map .exrc -> _.exrc */
                    }                     /*  (extra '_' now, "dot" below) */
                } else if (dotname) {     /* found a second dot, but still */
                    dotname = FALSE;      /*  have extra leading underscore: */
                    *pEnd = '\0';         /*  remove it by shifting chars */
                    pEnd = *pEndFAT + 1;  /*  left one space (e.g., .p1.p2: */
                    while (pEnd[1]) {     /*  __p1 -> _p1_p2 -> _p1.p2 when */
                        *pEnd = pEnd[1];  /*  finished) [opt.:  since first */
                        ++pEnd;           /*  two chars are same, can start */
                    }                     /*  shifting at second position] */
                }
                last_dot = pEnd;    /* point at last dot so far... */
                *pEnd++ = '_';      /* convert dot to underscore for now */
                break;

            default:
                *pEnd++ = (char)workch;

        } /* end switch */
    } /* end while loop */

    *pEnd = '\0';                 /* terminate buildpathFAT */

    /* NOTE:  keep in mind that pEnd points to the end of the path
     * component, and *pEndFAT still points to the *beginning* of it...
     * Also note that the algorithm does not try to get too fancy:
     * if there are no dots already, the name either gets truncated
     * at 8 characters or the last underscore is converted to a dot
     * (only if more characters are saved that way).  In no case is
     * a dot inserted between existing characters.
     */
    if (last_dot == NULL) {       /* no dots:  check for underscores... */
        char *plu = strrchr(pBegin, '_');   /* pointer to last underscore */

        if (plu == NULL) {   /* no dots, no underscores:  truncate at 8 chars */
            *pEndFAT += 8;        /* (or could insert '.' and keep 11...?) */
            if (*pEndFAT > pEnd)
                *pEndFAT = pEnd;  /* oops...didn't have 8 chars to truncate */
            else
                **pEndFAT = '\0';
        } else if (MIN(plu - pBegin, 8) + MIN(pEnd - plu - 1, 3) > 8) {
            last_dot = plu;       /* be lazy:  drop through to next if-blk */
        } else if ((pEnd - *pEndFAT) > 8) {
            *pEndFAT += 8;        /* more fits into just basename than if */
            **pEndFAT = '\0';     /*  convert last underscore to dot */
        } else
            *pEndFAT = pEnd;      /* whole thing fits into 8 chars or less */
    }

    if (last_dot != NULL) {       /* one dot (or two, in the case of */
        *last_dot = '.';          /*  "..") is OK:  put it back in */

        if ((last_dot - pBegin) > 8) {
            char *p, *q;
            int i;

            p = last_dot;
            q = last_dot = pBegin + 8;
            for (i = 0;  (i < 4) && *p;  ++i)  /* too many chars in basename: */
                *q++ = *p++;                   /*  shift .ext left and trun- */
            *q = '\0';                         /*  cate/terminate it */
            *pEndFAT = q;
        } else if ((pEnd - last_dot) > 4) {    /* too many chars in extension */
            *pEndFAT = last_dot + 4;
            **pEndFAT = '\0';
        } else
            *pEndFAT = pEnd;   /* filename is fine; point at terminating zero */

        if ((last_dot - pBegin) > 0 && last_dot[-1] == ' ')
            last_dot[-1] = '_';                /* NO blank in front of '.'! */
    }
} /* end function map2fat() */




/***********************/       /* Borrowed from os2.c for UnZip 5.1.        */
/* Function checkdir() */       /* Difference: no EA stuff                   */
/***********************/       /*             HPFS stuff works on NTFS too  */

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
 /* static int rootlen = 0;     */   /* length of rootpath */
 /* static char *rootpath;      */   /* user's "extract-to" directory */
 /* static char *buildpathHPFS; */   /* full path (so far) to extracted file, */
 /* static char *buildpathFAT;  */   /*  both HPFS/EA (main) and FAT versions */
 /* static char *endHPFS;       */   /* corresponding pointers to end of */
 /* static char *endFAT;        */   /*  buildpath ('\0') */

#   define FN_MASK   7
#   define FUNCTION  (flag & FN_MASK)



/*---------------------------------------------------------------------------
    APPEND_DIR:  append the path component to the path being built and check
    for its existence.  If doesn't exist and we are creating directories, do
    so for this one; else signal success or error as appropriate.
  ---------------------------------------------------------------------------*/

    if (FUNCTION == APPEND_DIR) {
        char *p = pathcomp;
        int too_long=FALSE;

        Trace((stderr, "appending dir segment [%s]\n", pathcomp));
        while ((*G.endHPFS = *p++) != '\0')     /* copy to HPFS filename */
            ++G.endHPFS;
        if (IsFileNameValid(G.buildpathHPFS)) {
            p = pathcomp;
            while ((*G.endFAT = *p++) != '\0')  /* copy to FAT filename, too */
                ++G.endFAT;
        } else
            map2fat(pathcomp, &G.endFAT);   /* map into FAT fn, update endFAT */

        /* GRR:  could do better check, see if overrunning buffer as we go:
         * check endHPFS-buildpathHPFS after each append, set warning variable
         * if within 20 of FILNAMSIZ; then if var set, do careful check when
         * appending.  Clear variable when begin new path. */

        /* next check:  need to append '/', at least one-char name, '\0' */
        if ((G.endHPFS-G.buildpathHPFS) > FILNAMSIZ-3)
            too_long = TRUE;                    /* check if extracting dir? */
#ifdef FIX_STAT_BUG
        /* Borland C++ 5.0 does not handle a call to stat() well if the
         * directory does not exist (it tends to crash in strange places.)
         * This is apparently a problem only when compiling for GUI rather
         * than console. The code below attempts to work around this problem.
         */
        if (access(G.buildpathFAT, 0) != 0) {
            if (!G.create_dirs) { /* told not to create (freshening) */
                free(G.buildpathHPFS);
                free(G.buildpathFAT);
                return 2;         /* path doesn't exist:  nothing to do */
            }
            if (too_long) {   /* GRR:  should allow FAT extraction w/o EAs */
                Info(slide, 1, ((char *)slide,
                  "checkdir error:  path too long: %s\n",
                  FnFilter1(G.buildpathHPFS)));
                free(G.buildpathHPFS);
                free(G.buildpathFAT);
                return 4;         /* no room for filenames:  fatal */
            }
            if (MKDIR(G.buildpathFAT, 0777) == -1) { /* create the directory */
                Info(slide, 1, ((char *)slide,
                  "checkdir error:  cannot create %s\n\
                 unable to process %s.\n",
                  FnFilter2(G.buildpathFAT), FnFilter1(G.filename)));
                free(G.buildpathHPFS);
                free(G.buildpathFAT);
                return 3;      /* path didn't exist, tried to create, failed */
            }
            G.created_dir = TRUE;
        }
#endif /* FIX_STAT_BUG */
        if (SSTAT(G.buildpathFAT, &G.statbuf))   /* path doesn't exist */
        {
            if (!G.create_dirs) { /* told not to create (freshening) */
                free(G.buildpathHPFS);
                free(G.buildpathFAT);
                return 2;         /* path doesn't exist:  nothing to do */
            }
            if (too_long) {   /* GRR:  should allow FAT extraction w/o EAs */
                Info(slide, 1, ((char *)slide,
                  "checkdir error:  path too long: %s\n",
                  FnFilter1(G.buildpathHPFS)));
                free(G.buildpathHPFS);
                free(G.buildpathFAT);
                return 4;         /* no room for filenames:  fatal */
            }
            if (MKDIR(G.buildpathFAT, 0777) == -1) { /* create the directory */
                Info(slide, 1, ((char *)slide,
                  "checkdir error:  cannot create %s\n\
                 unable to process %s.\n",
                  FnFilter2(G.buildpathFAT), FnFilter1(G.filename)));
                free(G.buildpathHPFS);
                free(G.buildpathFAT);
                return 3;      /* path didn't exist, tried to create, failed */
            }
            G.created_dir = TRUE;
        } else if (!S_ISDIR(G.statbuf.st_mode)) {
            Info(slide, 1, ((char *)slide,
              "checkdir error:  %s exists but is not directory\n   \
              unable to process %s.\n",
              FnFilter2(G.buildpathFAT), FnFilter1(G.filename)));
            free(G.buildpathHPFS);
            free(G.buildpathFAT);
            return 3;          /* path existed but wasn't dir */
        }
        if (too_long) {
            Info(slide, 1, ((char *)slide,
              "checkdir error:  path too long: %s\n",
               FnFilter1(G.buildpathHPFS)));
            free(G.buildpathHPFS);
            free(G.buildpathFAT);
            return 4;         /* no room for filenames:  fatal */
        }
        *G.endHPFS++ = '/';
        *G.endFAT++ = '/';
        *G.endHPFS = *G.endFAT = '\0';
        Trace((stderr, "buildpathHPFS now = [%s]\nbuildpathFAT now =  [%s]\n",
          FnFilter1(G.buildpathHPFS), FnFilter2(G.buildpathFAT)));
        return 0;

    } /* end if (FUNCTION == APPEND_DIR) */

/*---------------------------------------------------------------------------
    GETPATH:  copy full FAT path to the string pointed at by pathcomp (want
    filename to reflect name used on disk, not EAs; if full path is HPFS,
    buildpathFAT and buildpathHPFS will be identical).  Also free both paths.
  ---------------------------------------------------------------------------*/

    if (FUNCTION == GETPATH) {
        Trace((stderr, "getting and freeing FAT path [%s]\n",
          FnFilter1(G.buildpathFAT)));
        Trace((stderr, "freeing HPFS path [%s]\n",
          FnFilter1(G.buildpathHPFS)));
        strcpy(pathcomp, G.buildpathFAT);
        free(G.buildpathFAT);
        free(G.buildpathHPFS);
        G.buildpathHPFS = G.buildpathFAT = G.endHPFS = G.endFAT = NULL;
        return 0;
    }

/*---------------------------------------------------------------------------
    APPEND_NAME:  assume the path component is the filename; append it and
    return without checking for existence.
  ---------------------------------------------------------------------------*/

    if (FUNCTION == APPEND_NAME) {
        char *p = pathcomp;
        int error = 0;

        Trace((stderr, "appending filename [%s]\n", FnFilter1(pathcomp)));
        while ((*G.endHPFS = *p++) != '\0') {   /* copy to HPFS filename */
            ++G.endHPFS;
            if ((G.endHPFS-G.buildpathHPFS) >= FILNAMSIZ) {
                *--G.endHPFS = '\0';
                Info(slide, 1, ((char *)slide,
                  "checkdir warning:  path too long; truncating\n \
                  %s\n                -> %s\n",
                  FnFilter1(G.filename), FnFilter2(G.buildpathHPFS)));
                error = 1;   /* filename truncated */
            }
        }

        if ( G.pInfo->vollabel || IsFileNameValid(G.buildpathHPFS)) {
            p = pathcomp;
            while ((*G.endFAT = *p++) != '\0')  /* copy to FAT filename, too */
                ++G.endFAT;
        } else
            map2fat(pathcomp, &G.endFAT);   /* map into FAT fn, update endFAT */
        Trace((stderr, "buildpathHPFS: %s\nbuildpathFAT:  %s\n",
          FnFilter1(G.buildpathHPFS), FnFilter2(G.buildpathFAT)));

        return error;  /* could check for existence, prompt for new name... */

    } /* end if (FUNCTION == APPEND_NAME) */

/*---------------------------------------------------------------------------
    INIT:  allocate and initialize buffer space for the file currently being
    extracted.  If file was renamed with an absolute path, don't prepend the
    extract-to path.
  ---------------------------------------------------------------------------*/

    if (FUNCTION == INIT) {
        Trace((stderr, "initializing buildpathHPFS and buildpathFAT to "));
        if ((G.buildpathHPFS = (char *)malloc(G.fnlen+G.rootlen+1)) == NULL)
            return 10;
        if ((G.buildpathFAT = (char *)malloc(G.fnlen+G.rootlen+1)) == NULL) {
            free(G.buildpathHPFS);
            return 10;
        }
        if (G.pInfo->vollabel) { /* use root or renamed path, but don't store */
/* GRR:  for network drives, do strchr() and return IZ_VOL_LABEL if not [1] */
            if (G.renamed_fullpath && pathcomp[1] == ':')
                *G.buildpathHPFS = (char)ToLower(*pathcomp);
            else if (!G.renamed_fullpath && G.rootlen > 1 &&
                     G.rootpath[1] == ':')
                *G.buildpathHPFS = (char)ToLower(*G.rootpath);
            else {
                char tmpN[MAX_PATH], *tmpP;
                if (GetFullPathName(".", MAX_PATH, tmpN, &tmpP) > MAX_PATH)
                { /* by definition of MAX_PATH we should never get here */
                    Info(slide, 1, ((char *)slide,
                      "checkdir warning: current dir path too long\n"));
                    return 1;   /* can't get drive letter */
                }
                G.nLabelDrive = *tmpN - 'a' + 1;
                *G.buildpathHPFS = (char)(G.nLabelDrive - 1 + 'a');
            }
            G.nLabelDrive = *G.buildpathHPFS - 'a' + 1; /* save for mapname() */
            if (G.volflag == 0 || *G.buildpathHPFS < 'a'  /* no labels/bogus? */
                 || (G.volflag == 1 && !isfloppy(G.nLabelDrive))) { /* !fixed */
                free(G.buildpathHPFS);
                free(G.buildpathFAT);
                return IZ_VOL_LABEL;   /* skipping with message */
            }
            *G.buildpathHPFS = '\0';
        } else if (G.renamed_fullpath) /* pathcomp = valid data */
            strcpy(G.buildpathHPFS, pathcomp);
        else if (G.rootlen > 0)
            strcpy(G.buildpathHPFS, G.rootpath);
        else
            *G.buildpathHPFS = '\0';
        G.endHPFS = G.buildpathHPFS;
        G.endFAT = G.buildpathFAT;
        while ((*G.endFAT = *G.endHPFS) != '\0') {
            ++G.endFAT;
            ++G.endHPFS;
        }
        Trace((stderr, "[%s]\n", FnFilter1(G.buildpathHPFS)));
        return 0;
    }

/*---------------------------------------------------------------------------
    ROOT:  if appropriate, store the path in rootpath and create it if neces-
    sary; else assume it's a zipfile member and return.  This path segment
    gets used in extracting all members from every zipfile specified on the
    command line.  Note that under OS/2 and MS-DOS, if a candidate extract-to
    directory specification includes a drive letter (leading "x:"), it is
    treated just as if it had a trailing '/'--that is, one directory level
    will be created if the path doesn't exist, unless this is otherwise pro-
    hibited (e.g., freshening).
  ---------------------------------------------------------------------------*/

#if (!defined(SFX) || defined(SFX_EXDIR))
    if (FUNCTION == ROOT) {
        Trace((stderr, "initializing root path to [%s]\n",
          FnFilter1(pathcomp)));
        if (pathcomp == NULL) {
            G.rootlen = 0;
            return 0;
        }
        if ((G.rootlen = strlen(pathcomp)) > 0) {
            int had_trailing_pathsep=FALSE, has_drive=FALSE, xtra=2;

            if (isalpha(pathcomp[0]) && pathcomp[1] == ':')
                has_drive = TRUE;   /* drive designator */
            if (pathcomp[G.rootlen-1] == '/' || pathcomp[G.rootlen-1] == '\\') {
                pathcomp[--G.rootlen] = '\0';
                had_trailing_pathsep = TRUE;
            }
            if (has_drive && (G.rootlen == 2)) {
                if (!had_trailing_pathsep)   /* i.e., original wasn't "x:/" */
                    xtra = 3;      /* room for '.' + '/' + 0 at end of "x:" */
            } else if (G.rootlen > 0) {   /* need not check "x:." and "x:/" */
                if (SSTAT(pathcomp, &G.statbuf) || !S_ISDIR(G.statbuf.st_mode))
                {
                    /* path does not exist */
                    if (!G.create_dirs /* || iswild(pathcomp) */ ) {
                        G.rootlen = 0;
                        return 2;   /* treat as stored file */
                    }
                    /* create directory (could add loop here to scan pathcomp
                     * and create more than one level, but really necessary?) */
                    if (MKDIR(pathcomp, 0777) == -1) {
                        Info(slide, 1, ((char *)slide,
                          "checkdir:  cannot create extraction directory: %s\n",
                          FnFilter1(pathcomp)));
                        G.rootlen = 0; /* path didn't exist, tried to create, */
                        return 3;  /* failed:  file exists, or need 2+ levels */
                    }
                }
            }
            if ((G.rootpath = (char *)malloc(G.rootlen+xtra)) == NULL) {
                G.rootlen = 0;
                return 10;
            }
            strcpy(G.rootpath, pathcomp);
            if (xtra == 3)                  /* had just "x:", make "x:." */
                G.rootpath[G.rootlen++] = '.';
            G.rootpath[G.rootlen++] = '/';
            G.rootpath[G.rootlen] = '\0';
            Trace((stderr, "rootpath now = [%s]\n", FnFilter1(G.rootpath)));
        }
        return 0;
    }
#endif /* !SFX || SFX_EXDIR */

/*---------------------------------------------------------------------------
    END:  free rootpath, immediately prior to program exit.
  ---------------------------------------------------------------------------*/

    if (FUNCTION == END) {
        Trace((stderr, "freeing rootpath\n"));
        if (G.rootlen > 0) {
            free(G.rootpath);
            G.rootlen = 0;
        }
        return 0;
    }

    return 99;  /* should never reach */

} /* end function checkdir() */





#ifndef SFX
#ifndef WINDLL

/************************/
/*  Function version()  */
/************************/

void version(__G)
    __GDEF
{
    int len;
#if (defined(_MSC_VER) || defined(__WATCOMC__) || defined(__DJGPP__))
    char buf[80];
#if (defined(_MSC_VER) && (_MSC_VER > 900))
    char buf2[80];
#endif
#endif

    len = sprintf((char *)slide, CompiledWith,

#if defined(_MSC_VER)  /* MSC == VC++, but what about SDK compiler? */
      (sprintf(buf, "Microsoft C %d.%02d ", _MSC_VER/100, _MSC_VER%100), buf),
#  if (_MSC_VER == 800)
      "(Visual C++ v1.1)",
#  elif (_MSC_VER == 850)
      "(Windows NT v3.5 SDK)",
#  elif (_MSC_VER == 900)
      "(Visual C++ v2.x)",
#  elif (_MSC_VER > 900)
      (sprintf(buf2, "(Visual C++ %d.%d)", _MSC_VER/100 - 6, _MSC_VER%100/10),
        buf2),
#  else
      "(bad version)",
#  endif
#elif defined(__WATCOMC__)
#  if (__WATCOMC__ % 10 > 0)
      (sprintf(buf, "Watcom C/C++ %d.%02d", __WATCOMC__ / 100,
       __WATCOMC__ % 100), buf), "",
#  else
      (sprintf(buf, "Watcom C/C++ %d.%d", __WATCOMC__ / 100,
       (__WATCOMC__ % 100) / 10), buf), "",
#  endif
#elif defined(__BORLANDC__)
      "Borland C++",
#  if (__BORLANDC__ < 0x0200)
      " 1.0",
#  elif (__BORLANDC__ == 0x0200)
      " 2.0",
#  elif (__BORLANDC__ == 0x0400)
      " 3.0",
#  elif (__BORLANDC__ == 0x0410)   /* __BCPLUSPLUS__ = 0x0310 */
      " 3.1",
#  elif (__BORLANDC__ == 0x0452)   /* __BCPLUSPLUS__ = 0x0320 */
      " 4.0 or 4.02",
#  elif (__BORLANDC__ == 0x0460)   /* __BCPLUSPLUS__ = 0x0340 */
      " 4.5",
#  elif (__BORLANDC__ == 0x0500)   /* GRR:  assume this will stay sync'd? */
      " 5.0",
#  else
      " later than 5.0",
#  endif
#elif defined(__GNUC__)
#  ifdef __RSXNT__
#    if defined(__EMX__)
      "rsxnt(emx)+gcc ",
#    elif defined(__DJGPP__)
      (sprintf(buf, "rsxnt(djgpp) v%d.%02d / gcc ", __DJGPP__, __DJGPP_MINOR__),
       buf),
#    elif defined(__GO32__)
      "rsxnt(djgpp) v1.x / gcc ",
#    else
      "rsxnt(unknown) / gcc ",
#    endif
#  else
      "gcc ",
#  endif
      __VERSION__,
#else /* !_MSC_VER, !__WATCOMC__, !__BORLANDC__, !__GNUC__ */
      "unknown compiler (SDK?)", "",
#endif /* ?compilers */

      "Windows 95 / Windows NT", "\n(32-bit)",

#ifdef __DATE__
      " on ", __DATE__
#else
      "", ""
#endif
    );

    (*G.message)((zvoid *)&G, slide, (ulg)len, 0);

    return;

} /* end function version() */

#endif /* !WINDLL */
#endif /* !SFX */




#ifdef __WATCOMC__

/* This papers over a bug in Watcom 10.6's standard library... sigh */
/* Apparently it applies to both the DOS and Win32 stat()s.         */
/* I believe they said this was fixed for 11.0?                     */

int stat_bandaid(const char *path, struct stat *buf)
{
  char newname[4];
  if (!stat(path, buf))
    return 0;
  else if (!strcmp(path, ".") || (path[0] && !strcmp(path + 1, ":."))) {
    strcpy(newname, path);
    newname[strlen(path) - 1] = '\\';   /* stat(".") fails for root! */
    return stat(newname, buf);
  } else
    return -1;
}

/* Watcom 10.6's getch() does not handle Alt+<digit><digit><digit>. */
/* Note that if PASSWD_FROM_STDIN is defined, the file containing   */
/* the password must have a carriage return after the word, not a   */
/* Unix-style newline (linefeed only).  This discards linefeeds.    */

int getch(void)
{
  HANDLE stin;
  DWORD rc;
  unsigned char buf[2];
  int ret = -1;

#  ifdef PASSWD_FROM_STDIN
  DWORD odemode = ~0;
  stin = GetStdHandle(STD_INPUT_HANDLE);
  if (GetConsoleMode(stin, &odemode))
    SetConsoleMode(stin, ENABLE_PROCESSED_INPUT);  /* raw except ^C noticed */
#  else
  if (!(stin = CreateFile("CONIN$", GENERIC_READ | GENERIC_WRITE,
                          FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL)))
    return -1;
  SetConsoleMode(stin, ENABLE_PROCESSED_INPUT);    /* raw except ^C noticed */
#  endif
  if (ReadFile(stin, &buf, 1, &rc, NULL) && rc == 1)
    ret = buf[0];
  /* when the user hits return we get CR LF.  We discard the LF, not the CR,
   * because when we call this for the first time after a previous input
   * such as the one for "replace foo? [y]es, ..." the LF may still be in
   * the input stream before whatever the user types at our prompt. */
  if (ret == '\n')
    if (ReadFile(stin, &buf, 1, &rc, NULL) && rc == 1)
      ret = buf[0];
#  ifdef PASSWD_FROM_STDIN
  if (odemode != ~0)
    SetConsoleMode(stin, odemode);
#  else
  CloseHandle(stin);
#  endif
  return ret;
}

#endif /* __WATCOMC__ */
