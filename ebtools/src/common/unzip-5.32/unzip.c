/*---------------------------------------------------------------------------

  unzip.c

  UnZip - a zipfile extraction utility.  See below for make instructions, or
  read the comments in Makefile and the various Contents files for more de-
  tailed explanations.  To report a bug, send a *complete* description to
  Zip-Bugs@lists.wku.edu; include machine type, operating system and ver-
  sion, compiler and version, and reasonably detailed error messages or prob-
  lem report.  To join Info-ZIP, see the instructions in README.

  UnZip 5.x is a greatly expanded and partially rewritten successor to 4.x,
  which in turn was almost a complete rewrite of version 3.x.  For a detailed
  revision history, see UnzpHist.zip at quest.jpl.nasa.gov.  For a list of
  the many (near infinite) contributors, see "CONTRIBS" in the UnZip source
  distribution.

  ---------------------------------------------------------------------------

  [from original zipinfo.c]

  This program reads great gobs of totally nifty information, including the
  central directory stuff, from ZIP archives ("zipfiles" for short).  It
  started as just a testbed for fooling with zipfiles, but at this point it
  is actually a useful utility.  It also became the basis for the rewrite of
  UnZip (3.16 -> 4.0), using the central directory for processing rather than
  the individual (local) file headers.

  As of ZipInfo v2.0 and UnZip v5.1, the two programs are combined into one.
  If the executable is named "unzip" (or "unzip.exe", depending), it behaves
  like UnZip by default; if it is named "zipinfo" or "ii", it behaves like
  ZipInfo.  The ZipInfo behavior may also be triggered by use of unzip's -Z
  option; for example, "unzip -Z [zipinfo_options] archive.zip".

  Another dandy product from your buddies at Newtware!

  Author:  Greg Roelofs, newt@pobox.com, http://pobox.com/~newt/
           23 August 1990 -> April 1997

  ---------------------------------------------------------------------------

  Version:  unzip53.{tar.Z | tar.gz | zip} for Unix, VMS, OS/2, MS-DOS, Amiga,
              Atari, Windows 3.x/95/NT/CE, Macintosh, Human68K, Acorn RISC OS,
              BeOS, SMS/QDOS, VM/CMS, MVS, AOS/VS and TOPS-20.  Decryption
              requires sources in zcrypt27.zip.  See the accompanying "WHERE"
              file in the main source distribution for ftp, uucp, BBS and mail-
              server sites, or see http://www.cdrom.com/pub/infozip/UnZip.html .

  Copyrights:  see accompanying file "COPYING" in UnZip source distribution.
               (This software is free but NOT IN THE PUBLIC DOMAIN.  There
               are some restrictions on commercial use.)

  ---------------------------------------------------------------------------*/



#define UNZIP_C
#define UNZIP_INTERNAL
#include "unzip.h"        /* includes, typedefs, macros, prototypes, etc. */
#include "crypt.h"
#include "version.h"
#ifdef USE_ZLIB
#  include "zlib.h"
#endif

#ifndef WINDLL            /* The WINDLL port uses windll/windll.c instead... */


/*************/
/* Constants */
/*************/

#include "consts.h"  /* all constant global variables are in here */
                     /* (non-constant globals were moved to globals.c) */

/* constant local variables: */

#ifndef SFX
   static char Far EnvUnZip[] = ENV_UNZIP;
   static char Far EnvUnZip2[] = ENV_UNZIP2;
   static char Far EnvZipInfo[] = ENV_ZIPINFO;
   static char Far EnvZipInfo2[] = ENV_ZIPINFO2;
#ifdef RISCOS
   static char Far EnvUnZipExts[] = ENV_UNZIPEXTS;
#endif /* RISCOS */
#endif

#if (!defined(SFX) || defined(SFX_EXDIR))
   static char Far NotExtracting[] = "caution:  not extracting; -d ignored\n";
   static char Far MustGiveExdir[] =
     "error:  must specify directory to which to extract with -d option\n";
   static char Far OnlyOneExdir[] =
     "error:  -d option used more than once (only one exdir allowed)\n";
#endif

#if CRYPT
   static char Far MustGivePasswd[] =
     "error:  must give decryption password with -P option\n";
#endif

#ifndef SFX
   static char Far Zfirst[] = 
   "error:  -Z must be first option for ZipInfo mode (check UNZIP variable?)\n";
#endif
static char Far InvalidOptionsMsg[] = "error:\
  -fn or any combination of -c, -l, -p, -t, -u and -v options invalid\n";
static char Far IgnoreOOptionMsg[] =
  "caution:  both -n and -o specified; ignoring -o\n";

/* usage() strings */
#ifndef SFX
#ifdef VMS
   static char Far Example3[] = "vms.c";
   static char Far Example2[] = "  unzip\
 \"-V\" foo \"Bar\" => must quote uppercase options and filenames in VMS\n";
#else /* !VMS */
   static char Far Example3[] = "ReadMe";
#ifdef RISCOS
   static char Far Example2[] =
"  unzip foo -d RAM:$   => extract all files from foo into RAMDisc\n";
#else /* !RISCOS */
#if defined(OS2) || (defined(DOS_FLX_OS2_W32) && defined(MORE))
   static char Far Example2[] = "";   /* no room:  too many local3[] items */
#else /* !OS2 */
   static char Far Example2[] = " \
 unzip -p foo | more  => send contents of foo.zip via pipe into program more\n";
#endif /* ?OS2 */
#endif /* ?RISCOS */
#endif /* ?VMS */

/* local1[]:  command options */
#if (defined(DLL) && defined(API_DOC))
   static char Far local1[] = "  -A  print extended help for API functions";
#else /* !(DLL && API_DOC) */
   static char Far local1[] = "";
#endif /* ?(DLL && API_DOC) */

/* local2[] and local3[]:  modifier options */
#ifdef DOS_FLX_OS2_W32
#ifdef FLEXOS
   static char Far local2[] = "";
#else
   static char Far local2[] = " -$  label removables (-$$ => fixed disks)";
#endif
#ifdef OS2
#ifdef MORE
   static char Far local3[] = "\
  -X  restore ACLs if supported              -s  spaces in filenames => '_'\n\
                                             -M  pipe through \"more\" pager\n";
#else
   static char Far local3[] = " \
 -X  restore ACLs if supported              -s  spaces in filenames => '_'\n\n";
#endif /* ?MORE */
#else /* !OS2 */
#ifdef WIN32
#ifdef MORE
   static char Far local3[] = "\
  -X  restore ACLs (-XX => use privileges)   -s  spaces in filenames => '_'\n\
                                             -M  pipe through \"more\" pager\n";
#else
   static char Far local3[] = " \
 -X  restore ACLs (-XX => use privileges)   -s  spaces in filenames => '_'\n\n";
#endif /* ?MORE */
#else /* !WIN32 */
#ifdef MORE
   static char Far local3[] = "  -\
M  pipe through \"more\" pager              -s  spaces in filenames => '_'\n\n";
#else
   static char Far local3[] = "\
                                             -s  spaces in filenames => '_'\n";
#endif
#endif /* ?WIN32 */
#endif /* ?OS2 || ?WIN32 */
#else /* !DOS_FLX_OS2_W32 */
#ifdef VMS
   static char Far local2[] = "\"-X\" restore owner/protection info";
#ifdef MORE
   static char Far local3[] = "  \
                                          \"-M\" pipe through \"more\" pager\n";
#else
   static char Far local3[] = "\n";
#endif
#else /* !VMS */
#ifdef UNIX
   static char Far local2[] = " -X  restore UID/GID info";
#ifdef MORE
   static char Far local3[] = "\
                                             -M  pipe through \"more\" pager\n";
#else
   static char Far local3[] = "\n";
#endif
#else /* !UNIX */
#ifdef AMIGA
    static char Far local2[] = " -N  restore comments as filenotes";
#ifdef MORE
    static char Far local3[] = "\
                                             -M  pipe through \"more\" pager\n";
#else
    static char Far local3[] = "\n";
#endif
#else /* !AMIGA */
#ifdef MORE
    static char Far local2[] = " -M  pipe through \"more\" pager";
    static char Far local3[] = "\n";
#else
    static char Far local2[] = "";   /* Atari, Mac, etc. */
    static char Far local3[] = "";
#endif
#endif /* ?AMIGA */
#endif /* ?UNIX */
#endif /* ?VMS */
#endif /* ?DOS_FLX_OS2_W32 */
#endif /* !SFX */

#ifndef NO_ZIPINFO
#ifdef VMS
   static char Far ZipInfoExample[] = "* or % (e.g., \"*font-%.zip\")";
#else
   static char Far ZipInfoExample[] = "*, ?, [] (e.g., \"[a-j]*.zip\")";
#endif

static char Far ZipInfoUsageLine1[] = "\
ZipInfo %d.%d%d%s of %s, by Greg Roelofs and the fine folks at Info-ZIP.\n\
\n\
List name, date/time, attribute, size, compression method, etc., about files\n\
in list (excluding those in xlist) contained in the specified .zip archive(s).\
\n\"file[.zip]\" may be a wildcard name containing %s.\n\n\
   usage:  zipinfo [-12smlvChMtTz] file[.zip] [list...] [-x xlist...]\n\
      or:  unzip %s-Z%s [-12smlvChMtTz] file[.zip] [list...] [-x xlist...]\n";

static char Far ZipInfoUsageLine2[] = "\nmain\
 listing-format options:             -s  short Unix \"ls -l\" format (def.)\n\
  -1  filenames ONLY, one per line       -m  medium Unix \"ls -l\" format\n\
  -2  just filenames but allow -h/-t/-z  -l  long Unix \"ls -l\" format\n\
                                         -v  verbose, multi-page format\n";

static char Far ZipInfoUsageLine3[] = "miscellaneous options:\n\
  -h  print header line       -t  print totals for listed files or for all\n\
  -z  print zipfile comment  %c-T%c print file times in sortable decimal format\
\n %c-C%c be case-insensitive   %s\
  -x  exclude filenames that follow from listing\n";
#ifdef MORE
#ifdef VMS
   static char Far ZipInfoUsageLine4[] =
     " \"-M\" page output through built-in \"more\"\n";
#else
   static char Far ZipInfoUsageLine4[] =
     "  -M  page output through built-in \"more\"\n";
#endif
#else /* !MORE */
   static char Far ZipInfoUsageLine4[] = "";
#endif /* ?MORE */
#endif /* !NO_ZIPINFO */

#ifdef BETA
#  ifdef VMSCLI
   /* BetaVersion[] is also used in vms/cmdline.c:  do not make it static */
     char Far BetaVersion[] = "%s\
        THIS IS STILL A BETA VERSION OF UNZIP%s -- DO NOT DISTRIBUTE.\n\n";
#  else
     static char Far BetaVersion[] = "%s\
        THIS IS STILL A BETA VERSION OF UNZIP%s -- DO NOT DISTRIBUTE.\n\n";
#  endif
#endif

#ifdef SFX
#  ifdef VMSCLI
   /* UnzipSFXBanner[] is also used in vms/cmdline.c:  do not make it static */
     char Far UnzipSFXBanner[] =
#  else
     static char Far UnzipSFXBanner[] =
#  endif
     "UnZipSFX %d.%d%d%s of %s, by Info-ZIP (Zip-Bugs@lists.wku.edu).\n";
#  ifdef SFX_EXDIR
     static char Far UnzipSFXOpts[] =
    "Valid options are -tfupcz and -d <exdir>; modifiers are -abjnoqCL%sV%s.\n";
#  else
     static char Far UnzipSFXOpts[] =
       "Valid options are -tfupcz; modifiers are -abjnoqCL%sV%s.\n";
#  endif
#else /* !SFX */
   static char Far CompileOptions[] = "UnZip special compilation options:\n";
   static char Far CompileOptFormat[] = "\t%s\n";
   static char Far EnvOptions[] = "\nUnZip and ZipInfo environment options:\n";
   static char Far EnvOptFormat[] = "%16s:  %s\n";
   static char Far None[] = "[none]";
#  ifdef ASM_CRC
     static char Far AsmCRC[] = "ASM_CRC";
#  endif
#  ifdef ASM_INFLATECODES
     static char Far AsmInflateCodes[] = "ASM_INFLATECODES";
#  endif
#  ifdef CHECK_VERSIONS
     static char Far Check_Versions[] = "CHECK_VERSIONS";
#  endif
#  ifdef COPYRIGHT_CLEAN
     static char Far Copyright_Clean[] =
     "COPYRIGHT_CLEAN (PKZIP 0.9x unreducing method not supported)";
#  endif
#  ifdef DEBUG
     static char Far UDebug[] = "DEBUG";
#  endif
#  ifdef DEBUG_TIME
     static char Far DebugTime[] = "DEBUG_TIME";
#  endif
#  ifdef DLL
     static char Far Dll[] = "DLL";
#  endif
#  ifdef DOSWILD
     static char Far DosWild[] = "DOSWILD";
#  endif
#  ifdef LZW_CLEAN
     static char Far LZW_Clean[] =
     "LZW_CLEAN (PKZIP/Zip 1.x unshrinking method not supported)";
#  endif
#  ifndef MORE
     static char Far No_More[] = "NO_MORE";
#  endif
#  ifdef NO_ZIPINFO
     static char Far No_ZipInfo[] = "NO_ZIPINFO";
#  endif
#  ifdef NTSD_EAS
     static char Far NTSDExtAttrib[] = "NTSD_EAS";
#  endif
#  ifdef OS2_EAS
     static char Far OS2ExtAttrib[] = "OS2_EAS";
#  endif
#  ifdef REENTRANT
     static char Far Reentrant[] = "REENTRANT";
#  endif
#  ifdef REGARGS
     static char Far RegArgs[] = "REGARGS";
#  endif
#  ifdef RETURN_CODES
     static char Far Return_Codes[] = "RETURN_CODES";
#  endif
#  ifdef TIMESTAMP
     static char Far TimeStamp[] = "TIMESTAMP";
#  endif
#  ifdef UNIXBACKUP
     static char Far UnixBackup[] = "UNIXBACKUP";
#  endif
#  ifdef USE_EF_UT_TIME
     static char Far Use_EF_UT_time[] = "USE_EF_UT_TIME";
#  endif
#  ifndef LZW_CLEAN
     static char Far Use_Unshrink[] =
     "USE_UNSHRINK (PKZIP/Zip 1.x unshrinking method supported)";
#  endif
#  ifndef COPYRIGHT_CLEAN
     static char Far Use_Smith_Code[] =
     "USE_SMITH_CODE (PKZIP 0.9x unreducing method supported)";
#  endif
#  ifdef USE_VFAT
     static char Far Use_VFAT_support[] = "USE_VFAT";
#  endif
#  ifdef USE_ZLIB
     static char Far UseZlib[] =
     "USE_ZLIB (compiled with version %s; using version %s)";
#  endif
#  ifdef VMS_TEXT_CONV
     static char Far VmsTextConv[] = "VMS_TEXT_CONV";
#  endif
#  ifdef VMSCLI
     static char Far VmsCLI[] = "VMSCLI";
#  endif
#  ifdef VMSWILD
     static char Far VmsWild[] = "VMSWILD";
#  endif
#  if CRYPT
#    ifdef PASSWD_FROM_STDIN
       static char Far PasswdStdin[] = "PASSWD_FROM_STDIN";
#    endif
     static char Far Decryption[] = "\t[decryption, version %d.%d%s of %s]\n";
     static char Far CryptDate[] = CR_VERSION_DATE;
#  endif
#  ifndef __RSXNT__
#    ifdef __EMX__
       static char Far EnvEMX[] = "EMX";
       static char Far EnvEMXOPT[] = "EMXOPT";
#    endif
#    ifdef __GO32__
       static char Far EnvGO32[] = "GO32";
       static char Far EnvGO32TMP[] = "GO32TMP";
#    endif
#  endif /* !__RSXNT__ */

#ifdef VMS
/* UnzipUsageLine1[] is also used in vms/cmdline.c:  do not make it static */
   char Far UnzipUsageLine1[] = "\
UnZip %d.%d%d%s of %s, by Info-ZIP.  For more details see: unzip -v.\n\n";
#ifdef COPYRIGHT_CLEAN
   static char Far UnzipUsageLine1v[] = "\
UnZip %d.%d%d%s of %s, by Info-ZIP.  Maintained by Greg Roelofs.  Send\n\
bug reports to the authors at Zip-Bugs@lists.wku.edu; see README for details.\
\n\n";
#else
   static char Far UnzipUsageLine1v[] = "\
UnZip %d.%d%d%s of %s, by Info-ZIP.  UnReduce (c) 1989 by S. H. Smith.\n\
Send bug reports to authors at Zip-Bugs@lists.wku.edu; see README for details.\
\n\n";
#endif /* ?COPYRIGHT_CLEAN */
#else /* !VMS */
#ifdef COPYRIGHT_CLEAN
   static char Far UnzipUsageLine1[] = "\
UnZip %d.%d%d%s of %s, by Info-ZIP.  Maintained by Greg Roelofs.  Send\n\
bug reports to the authors at Zip-Bugs@lists.wku.edu; see README for details.\
\n\n";
#else
   static char Far UnzipUsageLine1[] = "\
UnZip %d.%d%d%s of %s, by Info-ZIP.  UnReduce (c) 1989 by S. H. Smith.\n\
Send bug reports to authors at Zip-Bugs@lists.wku.edu; see README for details.\
\n\n";
#endif /* ?COPYRIGHT_CLEAN */
#define UnzipUsageLine1v        UnzipUsageLine1
#endif /* ?VMS */

/*
static char Far UnzipUsageLine2v[] = "\
Latest sources and executables are always in ftp.uu.net:/pub/archiving/zip, at\
\nleast as of date of this release; see \"WHERE\" for other ftp and non-ftp \
sites.\n\n";
 */
static char Far UnzipUsageLine2v[] = "\
Latest sources and executables are at ftp://ftp.cdrom.com/pub/infozip/ , as of\
\nabove date; see http://www.cdrom.com/pub/infozip/UnZip.html for other sites.\
\n\n";

static char Far UnzipUsageLine2[] = "\
Usage: unzip %s[-opts[modifiers]] file[.zip] [list] [-x xlist] [-d exdir]\n \
 Default action is to extract files in list, except those in xlist, to exdir;\n\
  file[.zip] may be a wildcard.  %s\n";

#ifdef NO_ZIPINFO
#  define ZIPINFO_MODE_OPTION  ""
   static char Far ZipInfoMode[] =
     "(ZipInfo mode is disabled in this version.)";
#else
#  define ZIPINFO_MODE_OPTION  "[-Z] "
#  ifdef VMS
     static char Far ZipInfoMode[] =
       "\"-Z\" => ZipInfo mode (`unzip \"-Z\"' for usage).";
#  else
     static char Far ZipInfoMode[] =
       "-Z => ZipInfo mode (\"unzip -Z\" for usage).";
#  endif
#endif /* ?NO_ZIPINFO */

#ifdef VMS
   static char Far VMSusageLine2b[] = "\
=> define foreign command symbol in LOGIN.COM:  $ unzip :== $dev:[dir]unzip.exe\
\n";
#endif

static char Far UnzipUsageLine3[] = "\n\
  -p  extract files to pipe, no messages     -l  list files (short format)\n\
  -f  freshen existing files, create none    -t  test compressed archive data\n\
  -u  update files, create if necessary      -z  display archive comment\n\
  -x  exclude files that follow (in xlist)   -d  extract files into exdir\n\
%s\n";

static char Far UnzipUsageLine4[] = "\
modifiers:                                   -q  quiet mode (-qq => quieter)\n\
  -n  never overwrite existing files         -a  auto-convert any text files\n\
  -o  overwrite files WITHOUT prompting      -aa treat ALL files as text\n \
 -j  junk paths (do not make directories)   -v  be verbose/print version info\n\
 %c-C%c match filenames case-insensitively    %c-L%c make (some) names \
lowercase\n %-42s %c-V%c retain VMS version numbers\n%s";

static char Far UnzipUsageLine5[] = "\
Examples (see unzip.doc for more info):\n\
  unzip data1 -x joe   => extract all files except joe from zipfile data1.zip\n\
%s\
  unzip -fo foo %-6s => quietly replace existing %s if archive file newer\n";
#endif /* ?SFX */





/*****************************/
/*  main() / UzpMain() stub  */
/*****************************/

int MAIN(argc, argv)   /* return PK-type error code (except under VMS) */
    int argc;
    char *argv[];
{
    int r;

    CONSTRUCTGLOBALS();
    r = unzip(__G__ argc, argv);
    DESTROYGLOBALS()
    RETURN(r);
}




/*******************************/
/*  Primary UnZip entry point  */
/*******************************/

int unzip(__G__ argc, argv)
    __GDEF
    int argc;
    char *argv[];
{
#ifndef NO_ZIPINFO
    char *p;
#endif
#ifdef DOS_FLX_H68_OS2_W32
    int i;
#endif
    int retcode, error=FALSE;

/*---------------------------------------------------------------------------
    Macintosh initialization code.
  ---------------------------------------------------------------------------*/

#ifdef MACOS
    int a;

    for (a = 0;  a < 4;  ++a)
        G.rghCursor[a] = GetCursor(a+128);
    G.giCursor = 0;
    error = FALSE;
#endif

#if defined(__IBMC__) && defined(__DEBUG_ALLOC__)
    extern void DebugMalloc(void);

    atexit(DebugMalloc);
#endif

#ifdef MALLOC_WORK
    G.area.Slide =(uch *)calloc(8193, sizeof(shrint)+sizeof(uch)+sizeof(uch));
    G.area.shrink.Parent = (shrint *)G.area.Slide;
    G.area.shrink.value = G.area.Slide + (sizeof(shrint)*(HSIZE+1));
    G.area.shrink.Stack = G.area.Slide +
                           (sizeof(shrint) + sizeof(uch))*(HSIZE+1);
#endif

/*---------------------------------------------------------------------------
    Human68K initialization code.
  ---------------------------------------------------------------------------*/

#ifdef __human68k__
    InitTwentyOne();
#endif

/*---------------------------------------------------------------------------
    Acorn RISC OS initialization code.
  ---------------------------------------------------------------------------*/

#ifdef RISCOS
    set_prefix();
#endif

/*---------------------------------------------------------------------------
    Set signal handler for restoring echo, warn of zipfile corruption, etc.
  ---------------------------------------------------------------------------*/

#if (!defined(CMS_MVS))
    signal(SIGINT, handler);
#ifdef SIGTERM                 /* some systems really have no SIGTERM */
    signal(SIGTERM, handler);
#endif
#ifdef SIGBUS
    signal(SIGBUS, handler);
#endif
#ifdef SIGSEGV
    signal(SIGSEGV, handler);
#endif
#endif /* !CMS_MVS */

#if (defined(WIN32) && defined(__RSXNT__))
    for (i = 0 ; i < argc; i++) {
       _ISO_INTERN(argv[i]);
    }
#endif

/*---------------------------------------------------------------------------
    First figure out if we're running in UnZip mode or ZipInfo mode, and put
    the appropriate environment-variable options into the queue.  Then rip
    through any command-line options lurking about...
  ---------------------------------------------------------------------------*/

#ifdef SFX
    G.argv0 = argv[0];
#if defined(OS2) || defined(WIN32)
    G.zipfn = GetLoadPath(__G);/* non-MSC NT puts path into G.filename[] */
#else
    G.zipfn = G.argv0;
#endif

#ifdef VMSCLI
    {
        ulg status = vms_unzip_cmdline(&argc, &argv);
        if (!(status & 1))
            return status;
    }
#endif /* VMSCLI */

    G.zipinfo_mode = FALSE;
    error = uz_opts(__G__ &argc, &argv);   /* UnZipSFX call only */

#else /* !SFX */

    G.noargs = (argc == 1);   /* no options, no zipfile, no anything */

#ifdef RISCOS
    /* get the extensions to swap from environment */
    getRISCOSexts(ENV_UNZIPEXTS);
#endif

#ifdef MSDOS
    /* extract MKS extended argument list from environment (before envargs!) */
    mksargs(&argc, &argv);
#endif

#ifdef VMSCLI
    {
        ulg status = vms_unzip_cmdline(&argc, &argv);
        if (!(status & 1))
            return status;
    }
#endif /* VMSCLI */

#ifndef NO_ZIPINFO
    if ((p = strrchr(argv[0], DIR_END)) == (char *)NULL)
        p = argv[0];
    else
        ++p;

    if (STRNICMP(p, LoadFarStringSmall(Zipnfo), 7) == 0 ||
        STRNICMP(p, "ii", 2) == 0 ||
        (argc > 1 && strncmp(argv[1], "-Z", 2) == 0))
    {
        G.zipinfo_mode = TRUE;
        envargs(__G__ &argc, &argv, LoadFarStringSmall(EnvZipInfo),
          LoadFarStringSmall2(EnvZipInfo2));
        error = zi_opts(__G__ &argc, &argv);
    } else
#endif /* NO_ZIPINFO */
    {
        G.zipinfo_mode = FALSE;
        envargs(__G__ &argc, &argv, LoadFarStringSmall(EnvUnZip),
          LoadFarStringSmall2(EnvUnZip2));
        error = uz_opts(__G__ &argc, &argv);
    }

#endif /* ?SFX */

    if ((argc < 0) || error)
        return error;

/*---------------------------------------------------------------------------
    Now get the zipfile name from the command line and then process any re-
    maining options and file specifications.
  ---------------------------------------------------------------------------*/

#ifdef DOS_FLX_H68_OS2_W32
    /* convert MSDOS-style directory separators to Unix-style ones for
     * user's convenience (include zipfile name itself)
     */
    for (G.pfnames = argv, i = argc+1;  i > 0;  --i) {
        char *q;

        for (q = *G.pfnames;  *q;  ++q)
            if (*q == '\\')
                *q = '/';
        ++G.pfnames;
    }
#endif /* DOS_FLX_H68_OS2_W32 */

#ifndef SFX
    G.wildzipfn = *argv++;
#endif

#if (defined(SFX) && !defined(SFX_EXDIR)) /* only check for -x */

    G.filespecs = argc;
    G.xfilespecs = 0;

    if (argc > 0) {
        char **pp = argv-1;

        G.pfnames = argv;
        while (*++pp)
            if (strcmp(*pp, "-x") == 0) {
                if (pp > argv) {
                    *pp = 0;              /* terminate G.pfnames */
                    G.filespecs = pp - G.pfnames;
                } else {
                    G.pfnames = fnames;  /* defaults */
                    G.filespecs = 0;
                }
                G.pxnames = pp + 1;      /* excluded-names ptr: _after_ -x */
                G.xfilespecs = argc - G.filespecs - 1;
                break;                    /* skip rest of args */
            }
        G.process_all_files = FALSE;
    } else
        G.process_all_files = TRUE;      /* for speed */

#else /* !SFX || SFX_EXDIR */             /* check for -x or -d */

    G.filespecs = argc;
    G.xfilespecs = 0;

    if (argc > 0) {
        int in_files=FALSE, in_xfiles=FALSE;
        char **pp = argv-1;

        G.process_all_files = FALSE;
        G.pfnames = argv;
        while (*++pp) {
            Trace((stderr, "pp - argv = %d\n", pp-argv));
#ifdef CMS_MVS
            if (!G.dflag && STRNICMP(*pp, "-d", 2) == 0) {
#else
            if (!G.dflag && strncmp(*pp, "-d", 2) == 0) {
#endif
                char *q = *pp;
                int firstarg = (pp == argv);

                G.dflag = TRUE;
                if (in_files) {      /* ... zipfile ... -d exdir ... */
                    *pp = (char *)NULL;         /* terminate G.pfnames */
                    G.filespecs = pp - G.pfnames;
                    in_files = FALSE;
                } else if (in_xfiles) {
                    *pp = (char *)NULL;         /* terminate G.pxnames */
                    G.xfilespecs = pp - G.pxnames;
                    /* "... -x xlist -d exdir":  nothing left */
                }
                /* first check for "-dexdir", then for "-d exdir" */
                if (q[2])
                    q += 2;
                else if (*++pp)
                    q = *pp;
                else {
                    Info(slide, 0x401, ((char *)slide,
                      LoadFarString(MustGiveExdir)));
                    return(PK_PARAM);  /* don't extract here by accident */
                }
                if (G.extract_flag) {
                    G.create_dirs = !G.fflag;
                    if ((error = checkdir(__G__ q, ROOT)) > 2) {
                        return(error);  /* out of memory, or file in way */
                    }
                } else
                    Info(slide, 0x401, ((char *)slide,
                      LoadFarString(NotExtracting)));
                if (firstarg)   /* ... zipfile -d exdir ... */
                    if (pp[1]) {
                        G.pfnames = pp + 1;  /* argv+2 */
                        G.filespecs = argc - (G.pfnames-argv);  /* for now... */
                    } else {
                        G.process_all_files = TRUE;
                        G.pfnames = fnames;  /* GRR: necessary? */
                        G.filespecs = 0;     /* GRR: necessary? */
                        break;
                    }
            } else if (!in_xfiles) {
                if (strcmp(*pp, "-x") == 0) {
                    in_xfiles = TRUE;
                    if (pp == G.pfnames) {
                        G.pfnames = fnames;  /* defaults */
                        G.filespecs = 0;
                    } else if (in_files) {
                        *pp = 0;                   /* terminate G.pfnames */
                        G.filespecs = pp - G.pfnames;  /* adjust count */
                        in_files = FALSE;
                    }
                    G.pxnames = pp + 1; /* excluded-names ptr starts after -x */
                    G.xfilespecs = argc - (G.pxnames-argv);  /* anything left */
                } else
                    in_files = TRUE;
            }
        }
    } else
        G.process_all_files = TRUE;      /* for speed */

#endif /* ?(SFX && !SFX_EXDIR) */

/*---------------------------------------------------------------------------
    Okey dokey, we have everything we need to get started.  Let's roll.
  ---------------------------------------------------------------------------*/

    retcode = process_zipfiles(__G);
    return(retcode);

} /* end main()/unzip() */





/**********************/
/* Function uz_opts() */
/**********************/

int uz_opts(__G__ pargc, pargv)
    int *pargc;
    char ***pargv;
    __GDEF
{
    char **argv, *s;
#if (!defined(SFX) || defined(SFX_EXDIR))
    char *exdir = NULL;   /* initialized to shut up false GCC warning */
#endif
    int argc, c, error=FALSE, negative=0;


    argc = *pargc;
    argv = *pargv;

    while (--argc > 0 && (*++argv)[0] == '-') {
        s = argv[0] + 1;
        while ((c = *s++) != 0) {    /* "!= 0":  prevent Turbo C warning */
#ifdef CMS_MVS
            switch (tolower(c))
#else
            switch (c)
#endif
            {
                case ('-'):
                    ++negative;
                    break;
                case ('a'):
                    if (negative) {
                        G.aflag = MAX(G.aflag-negative,0);
                        negative = 0;
                    } else
                        ++G.aflag;
                    break;
#if (defined(DLL) && defined(API_DOC))
                case ('A'):    /* extended help for API */
                    APIhelp(__G__ argc,argv);
                    *pargc = -1;  /* signal to exit successfully */
                    return 0;
#endif
                case ('b'):
                    if (negative) {
#ifdef VMS
                        G.bflag = MAX(G.bflag-negative,0);
#endif
                        negative = 0;   /* do nothing:  "-b" is default */
                    } else {
#ifdef VMS
                        if (G.aflag == 0)
                           ++G.bflag;
#endif
                        G.aflag = 0;
                    }
                    break;
#ifdef UNIXBACKUP
                case ('B'): /* -B: back up existing files */
                    if (negative)
                        G.B_flag = FALSE, negative = 0;
                    else
                        G.B_flag = TRUE;
                    break;
#endif
                case ('c'):
                    if (negative) {
                        G.cflag = FALSE, negative = 0;
#ifdef NATIVE
                        G.aflag = 0;
#endif
                    } else {
                        G.cflag = TRUE;
#ifdef NATIVE
                        G.aflag = 2;  /* so you can read it on the screen */
#endif
#ifdef DLL
                        if (G.redirect_text)
                            G.redirect_data = 2;
#endif
                    }
                    break;
#ifndef CMS_MVS
                case ('C'):    /* -C:  match filenames case-insensitively */
                    if (negative)
                        G.C_flag = FALSE, negative = 0;
                    else
                        G.C_flag = TRUE;
                    break;
#endif /* !CMS_MVS */
#if (!defined(SFX) || defined(SFX_EXDIR))
                case ('d'):
                    if (negative) {   /* negative not allowed with -d exdir */
                        Info(slide, 0x401, ((char *)slide,
                          LoadFarString(MustGiveExdir)));
                        return(PK_PARAM);  /* don't extract here by accident */
                    }
                    if (G.dflag) {
                        Info(slide, 0x401, ((char *)slide,
                          LoadFarString(OnlyOneExdir)));
                        return(PK_PARAM);    /* GRR:  stupid restriction? */
                    } else {
                        G.dflag = TRUE;
                        /* first check for "-dexdir", then for "-d exdir" */
                        exdir = s;
                        if (*exdir == 0) {
                            if (argc > 1) {
                                --argc;
                                exdir = *++argv;
                                if (*exdir == '-') {
                                    Info(slide, 0x401, ((char *)slide,
                                      LoadFarString(MustGiveExdir)));
                                    return(PK_PARAM);
                                }
                                /* else exdir points at extraction dir */
                            } else {
                                Info(slide, 0x401, ((char *)slide,
                                  LoadFarString(MustGiveExdir)));
                                return(PK_PARAM);
                            }
                        }
                        /* exdir now points at extraction dir (-dexdir or
                         *  -d exdir); point s at end of exdir to avoid mis-
                         *  interpretation of exdir characters as more options
                         */
                        if (*s != 0)
                            while (*++s != 0)
                                ;
                    }
                    break;
#endif /* !SFX || SFX_EXDIR */
                case ('e'):    /* just ignore -e, -x options (extract) */
                    break;
                case ('f'):    /* "freshen" (extract only newer files) */
                    if (negative)
                        G.fflag = G.uflag = FALSE, negative = 0;
                    else
                        G.fflag = G.uflag = TRUE;
                    break;
                case ('h'):    /* just print help message and quit */
                    *pargc = -1;
                    return USAGE(PK_OK);
                case ('j'):    /* junk pathnames/directory structure */
                    if (negative)
                        G.jflag = FALSE, negative = 0;
                    else
                        G.jflag = TRUE;
                    break;
#ifndef SFX
                case ('l'):
                    if (negative) {
                        G.vflag = MAX(G.vflag-negative,0);
                        negative = 0;
                    } else
                        ++G.vflag;
                    break;
#endif /* !SFX */
#ifndef CMS_MVS
                case ('L'):    /* convert (some) filenames to lowercase */
                    if (negative)
                        G.L_flag = FALSE, negative = 0;
                    else
                        G.L_flag = TRUE;
                    break;
#endif /* !CMS_MVS */
#ifdef MORE
#ifdef CMS_MVS
                case ('m'):
#endif
                case ('M'):    /* send all screen output through "more" fn. */
/* GRR:  eventually check for numerical argument => height */
                    if (negative)
                        G.M_flag = FALSE, negative = 0;
                    else
                        G.M_flag = TRUE;
                    break;
#endif /* MORE */
                case ('n'):    /* don't overwrite any files */
                    if (negative)
                        G.overwrite_none = FALSE, negative = 0;
                    else
                        G.overwrite_none = TRUE;
                    break;
#ifdef AMIGA
                case ('N'):    /* restore comments as filenotes */
                    if (negative)
                        G.N_flag = FALSE, negative = 0;
                    else
                        G.N_flag = TRUE;
                    break;
#endif /* AMIGA */
                case ('o'):    /* OK to overwrite files without prompting */
                    if (negative) {
                        G.overwrite_all = MAX(G.overwrite_all-negative,0);
                        negative = 0;
                    } else
                        ++G.overwrite_all;
                    break;
                case ('p'):    /* pipes:  extract to stdout, no messages */
                    if (negative) {
                        G.cflag = FALSE;
                        G.qflag = MAX(G.qflag-999,0);
                        negative = 0;
                    } else {
                        G.cflag = TRUE;
                        G.qflag += 999;
                    }
                    break;
#if CRYPT
                /* GRR:  yes, this is highly insecure, but dozens of people
                 * have pestered us for this, so here we go... */
                case ('P'):
                    if (negative) {   /* negative not allowed with -P passwd */
                        Info(slide, 0x401, ((char *)slide,
                          LoadFarString(MustGivePasswd)));
                        return(PK_PARAM);  /* don't extract here by accident */
                    }
                    if (G.P_flag) {
/*
                        GRR:  eventually support multiple passwords?
                        Info(slide, 0x401, ((char *)slide,
                          LoadFarString(OnlyOnePasswd)));
                        return(PK_PARAM);
 */
                    } else {
                        G.P_flag = TRUE;
                        /* first check for "-dpasswd", then for "-d passwd" */
                        G.pwdarg = s;
                        if (*G.pwdarg == 0) {
                            if (argc > 1) {
                                --argc;
                                G.pwdarg = *++argv;
                                if (*G.pwdarg == '-') {
                                    Info(slide, 0x401, ((char *)slide,
                                      LoadFarString(MustGivePasswd)));
                                    return(PK_PARAM);
                                }
                                /* else pwdarg points at decryption password */
                            } else {
                                Info(slide, 0x401, ((char *)slide,
                                  LoadFarString(MustGivePasswd)));
                                return(PK_PARAM);
                            }
                        }
                        /* pwdarg now points at decryption password (-Ppasswd or
                         *  -P passwd); point s at end of passwd to avoid mis-
                         *  interpretation of passwd characters as more options
                         */
                        if (*s != 0)
                            while (*++s != 0)
                                ;
                    }
                    break;
#endif /* CRYPT */
                case ('q'):    /* quiet:  fewer comments/messages */
                    if (negative) {
                        G.qflag = MAX(G.qflag-negative,0);
                        negative = 0;
                    } else
                        ++G.qflag;
                    break;
#ifdef QDOS
                case ('Q'):   /* QDOS flags */
                    qlflag ^= strtol(s, &s, 10);
                    break;    /* we XOR this as we can config qlflags */
#endif
#ifdef DOS_FLX_OS2_W32
                case ('s'):    /* spaces in filenames:  allow by default */
                    if (negative)
                        G.sflag = FALSE, negative = 0;
                    else
                        G.sflag = TRUE;
                    break;
#endif /* DOS_FLX_OS2_W32 */
                case ('t'):
                    if (negative)
                        G.tflag = FALSE, negative = 0;
                    else
                        G.tflag = TRUE;
                    break;
#ifdef TIMESTAMP
                case ('T'):
                    if (negative)
                        G.T_flag = FALSE, negative = 0;
                    else
                        G.T_flag = TRUE;
                    break;
#endif
                case ('u'):    /* update (extract only new and newer files) */
                    if (negative)
                        G.uflag = FALSE, negative = 0;
                    else
                        G.uflag = TRUE;
                    break;
#ifndef CMS_MVS
                case ('U'):    /* obsolete; to be removed in version 6.0 */
                    if (negative)
                        G.L_flag = TRUE, negative = 0;
                    else
                        G.L_flag = FALSE;
                    break;
#endif /* !CMS_MVS */
#ifndef SFX
                case ('v'):    /* verbose */
                    if (negative) {
                        G.vflag = MAX(G.vflag-negative,0);
                        negative = 0;
                    } else if (G.vflag)
                        ++G.vflag;
                    else
                        G.vflag = 2;
                    break;
#endif /* !SFX */
#ifndef CMS_MVS
                case ('V'):    /* Version (retain VMS/DEC-20 file versions) */
                    if (negative)
                        G.V_flag = FALSE, negative = 0;
                    else
                        G.V_flag = TRUE;
                    break;
#endif /* !CMS_MVS */
                case ('x'):    /* extract:  default */
#ifdef SFX
                    /* when 'x' is the only option in this argument, and the
                     * next arg is not an option, assume this initiates an
                     * exclusion list (-x xlist):  terminate option-scanning
                     * and leave uz_opts with argv still pointing to "-x";
                     * the xlist is processed later
                     */
                    if (s - argv[0] == 2 && *s == 0 &&
                        argc > 1 && argv[1][0] != '-') {
                        /* break out of nested loops without "++argv;--argc" */
                        goto opts_done;
                    }
#endif /* SFX */
                    break;
#if defined(VMS) || defined(UNIX) || defined(OS2) || defined(WIN32)
                case ('X'):   /* restore owner/protection info (need privs?) */
                    if (negative) {
                        G.X_flag = MAX(G.X_flag-negative,0);
                        negative = 0;
                    } else
                        ++G.X_flag;
                    break;
#endif /* VMS || UNIX || OS2 || WIN32 */
                case ('z'):    /* display only the archive comment */
                    if (negative) {
                        G.zflag = MAX(G.zflag-negative,0);
                        negative = 0;
                    } else
                        ++G.zflag;
                    break;
#ifndef SFX
                case ('Z'):    /* should have been first option (ZipInfo) */
                    Info(slide, 0x401, ((char *)slide, LoadFarString(Zfirst)));
                    error = TRUE;
                    break;
#endif /* !SFX */
#ifdef DOS_OS2_W32
                case ('$'):
                    if (negative) {
                        G.volflag = MAX(G.volflag-negative,0);
                        negative = 0;
                    } else
                        ++G.volflag;
                    break;
#endif /* DOS_OS2_W32 */
                default:
                    error = TRUE;
                    break;

            } /* end switch */
        } /* end while (not end of argument string) */
    } /* end while (not done with switches) */

/*---------------------------------------------------------------------------
    Check for nonsensical combinations of options.
  ---------------------------------------------------------------------------*/

#ifdef SFX
opts_done:  /* yes, very ugly...but only used by UnZipSFX with -x xlist */
#endif

    if ((G.cflag && G.tflag) || (G.cflag && G.uflag) ||
        (G.tflag && G.uflag) || (G.fflag && G.overwrite_none))
    {
        Info(slide, 0x401, ((char *)slide, LoadFarString(InvalidOptionsMsg)));
        error = TRUE;
    }
    if (G.aflag > 2)
        G.aflag = 2;
#ifdef VMS
    if (G.bflag > 2)
        G.bflag = 2;
#endif
    if (G.overwrite_all && G.overwrite_none) {
        Info(slide, 0x401, ((char *)slide, LoadFarString(IgnoreOOptionMsg)));
        G.overwrite_all = FALSE;
    }
#ifdef MORE
    if (G.M_flag && !isatty(1))  /* stdout redirected: "more" func. useless */
        G.M_flag = 0;
#endif

#ifdef SFX
    if (error)
#else
    if ((argc-- == 0) || error)
#endif
    {
        *pargc = argc;
        *pargv = argv;
#ifndef SFX
        if (G.vflag >= 2 && argc == -1) {              /* "unzip -v" */
            if (G.qflag > 3)                           /* "unzip -vqqqq" */
                Info(slide, 0, ((char *)slide, "%d\n",
                  (UZ_MAJORVER*100 + UZ_MINORVER*10 + PATCHLEVEL)));
            else {
                char *envptr, *getenv();
                int numopts = 0;

                Info(slide, 0, ((char *)slide, LoadFarString(UnzipUsageLine1v),
                  UZ_MAJORVER, UZ_MINORVER, PATCHLEVEL, BETALEVEL,
                  LoadFarStringSmall(VersionDate)));
                Info(slide, 0, ((char *)slide,
                  LoadFarString(UnzipUsageLine2v)));
                version(__G);
                Info(slide, 0, ((char *)slide, LoadFarString(CompileOptions)));
#ifdef ASM_CRC
                Info(slide, 0, ((char *)slide, LoadFarString(CompileOptFormat),
                  LoadFarStringSmall(AsmCRC)));
                ++numopts;
#endif
#ifdef ASM_INFLATECODES
                Info(slide, 0, ((char *)slide, LoadFarString(CompileOptFormat),
                  LoadFarStringSmall(AsmInflateCodes)));
                ++numopts;
#endif
#ifdef CHECK_VERSIONS
                Info(slide, 0, ((char *)slide, LoadFarString(CompileOptFormat),
                  LoadFarStringSmall(Check_Versions)));
                ++numopts;
#endif
#ifdef COPYRIGHT_CLEAN
                Info(slide, 0, ((char *)slide, LoadFarString(CompileOptFormat),
                  LoadFarStringSmall(Copyright_Clean)));
                ++numopts;
#endif
#ifdef DEBUG
                Info(slide, 0, ((char *)slide, LoadFarString(CompileOptFormat),
                  LoadFarStringSmall(UDebug)));
                ++numopts;
#endif
#ifdef DEBUG_TIME
                Info(slide, 0, ((char *)slide, LoadFarString(CompileOptFormat),
                  LoadFarStringSmall(DebugTime)));
                ++numopts;
#endif
#ifdef DLL
                Info(slide, 0, ((char *)slide, LoadFarString(CompileOptFormat),
                  LoadFarStringSmall(Dll)));
                ++numopts;
#endif
#ifdef DOSWILD
                Info(slide, 0, ((char *)slide, LoadFarString(CompileOptFormat),
                  LoadFarStringSmall(DosWild)));
                ++numopts;
#endif
#ifdef LZW_CLEAN
                Info(slide, 0, ((char *)slide, LoadFarString(CompileOptFormat),
                  LoadFarStringSmall(LZW_Clean)));
                ++numopts;
#endif
#ifndef MORE
                Info(slide, 0, ((char *)slide, LoadFarString(CompileOptFormat),
                  LoadFarStringSmall(No_More)));
                ++numopts;
#endif
#ifdef NO_ZIPINFO
                Info(slide, 0, ((char *)slide, LoadFarString(CompileOptFormat),
                  LoadFarStringSmall(No_ZipInfo)));
                ++numopts;
#endif
#ifdef NTSD_EAS
                Info(slide, 0, ((char *)slide, LoadFarString(CompileOptFormat),
                  LoadFarStringSmall(NTSDExtAttrib)));
                ++numopts;
#endif
#ifdef OS2_EAS
                Info(slide, 0, ((char *)slide, LoadFarString(CompileOptFormat),
                  LoadFarStringSmall(OS2ExtAttrib)));
                ++numopts;
#endif
#ifdef REENTRANT
                Info(slide, 0, ((char *)slide, LoadFarString(CompileOptFormat),
                  LoadFarStringSmall(Reentrant)));
                ++numopts;
#endif
#ifdef REGARGS
                Info(slide, 0, ((char *)slide, LoadFarString(CompileOptFormat),
                  LoadFarStringSmall(RegArgs)));
                ++numopts;
#endif
#ifdef RETURN_CODES
                Info(slide, 0, ((char *)slide, LoadFarString(CompileOptFormat),
                  LoadFarStringSmall(Return_Codes)));
                ++numopts;
#endif
#ifdef TIMESTAMP
                Info(slide, 0, ((char *)slide, LoadFarString(CompileOptFormat),
                  LoadFarStringSmall(TimeStamp)));
                ++numopts;
#endif
#ifdef UNIXBACKUP
                Info(slide, 0, ((char *)slide, LoadFarString(CompileOptFormat),
                  LoadFarStringSmall(UnixBackup)));
                ++numopts;
#endif
#ifdef USE_EF_UT_TIME
                Info(slide, 0, ((char *)slide, LoadFarString(CompileOptFormat),
                  LoadFarStringSmall(Use_EF_UT_time)));
                ++numopts;
#endif
#ifndef COPYRIGHT_CLEAN
                Info(slide, 0, ((char *)slide, LoadFarString(CompileOptFormat),
                  LoadFarStringSmall(Use_Smith_Code)));
                ++numopts;
#endif
#ifndef LZW_CLEAN
                Info(slide, 0, ((char *)slide, LoadFarString(CompileOptFormat),
                  LoadFarStringSmall(Use_Unshrink)));
                ++numopts;
#endif
#ifdef USE_VFAT
                Info(slide, 0, ((char *)slide, LoadFarString(CompileOptFormat),
                  LoadFarStringSmall(Use_VFAT_support)));
                ++numopts;
#endif
#ifdef USE_ZLIB
                sprintf((char *)(slide+256), LoadFarStringSmall(UseZlib),
                  ZLIB_VERSION, zlib_version);
                Info(slide, 0, ((char *)slide, LoadFarString(CompileOptFormat),
                  (char *)(slide+256)));
                ++numopts;
#endif
#ifdef VMS_TEXT_CONV
                Info(slide, 0, ((char *)slide, LoadFarString(CompileOptFormat),
                  LoadFarStringSmall(VmsTextConv)));
                ++numopts;
#endif
#ifdef VMSCLI
                Info(slide, 0, ((char *)slide, LoadFarString(CompileOptFormat),
                  LoadFarStringSmall(VmsCLI)));
                ++numopts;
#endif
#ifdef VMSWILD
                Info(slide, 0, ((char *)slide, LoadFarString(CompileOptFormat),
                  LoadFarStringSmall(VmsWild)));
                ++numopts;
#endif
#if CRYPT
# ifdef PASSWD_FROM_STDIN
                Info(slide, 0, ((char *)slide, LoadFarString(CompileOptFormat),
                  LoadFarStringSmall(PasswdStdin)));
# endif
                Info(slide, 0, ((char *)slide, LoadFarString(Decryption),
                  CR_MAJORVER, CR_MINORVER, CR_BETA_VER,
                  LoadFarStringSmall(CryptDate)));
                ++numopts;
#endif /* CRYPT */
                if (numopts == 0)
                    Info(slide, 0, ((char *)slide,
                      LoadFarString(CompileOptFormat),
                      LoadFarStringSmall(None)));

                Info(slide, 0, ((char *)slide, LoadFarString(EnvOptions)));
                envptr = getenv(LoadFarStringSmall(EnvUnZip));
                Info(slide, 0, ((char *)slide, LoadFarString(EnvOptFormat),
                  LoadFarStringSmall(EnvUnZip),
                  (envptr == (char *)NULL || *envptr == 0)?
                  LoadFarStringSmall2(None) : envptr));
                envptr = getenv(LoadFarStringSmall(EnvUnZip2));
                Info(slide, 0, ((char *)slide, LoadFarString(EnvOptFormat),
                  LoadFarStringSmall(EnvUnZip2),
                  (envptr == (char *)NULL || *envptr == 0)?
                  LoadFarStringSmall2(None) : envptr));
                envptr = getenv(LoadFarStringSmall(EnvZipInfo));
                Info(slide, 0, ((char *)slide, LoadFarString(EnvOptFormat),
                  LoadFarStringSmall(EnvZipInfo),
                  (envptr == (char *)NULL || *envptr == 0)?
                  LoadFarStringSmall2(None) : envptr));
                envptr = getenv(LoadFarStringSmall(EnvZipInfo2));
                Info(slide, 0, ((char *)slide, LoadFarString(EnvOptFormat),
                  LoadFarStringSmall(EnvZipInfo2),
                  (envptr == (char *)NULL || *envptr == 0)?
                  LoadFarStringSmall2(None) : envptr));
#ifndef __RSXNT__
#ifdef __EMX__
                envptr = getenv(LoadFarStringSmall(EnvEMX));
                Info(slide, 0, ((char *)slide, LoadFarString(EnvOptFormat),
                  LoadFarStringSmall(EnvEMX),
                  (envptr == (char *)NULL || *envptr == 0)?
                  LoadFarStringSmall2(None) : envptr));
                envptr = getenv(LoadFarStringSmall(EnvEMXOPT));
                Info(slide, 0, ((char *)slide, LoadFarString(EnvOptFormat),
                  LoadFarStringSmall(EnvEMXOPT),
                  (envptr == (char *)NULL || *envptr == 0)?
                  LoadFarStringSmall2(None) : envptr));
#endif /* __EMX__ */
#ifdef __GO32__
                envptr = getenv(LoadFarStringSmall(EnvGO32));
                Info(slide, 0, ((char *)slide, LoadFarString(EnvOptFormat),
                  LoadFarStringSmall(EnvGO32),
                  (envptr == (char *)NULL || *envptr == 0)?
                  LoadFarStringSmall2(None) : envptr));
                envptr = getenv(LoadFarStringSmall(EnvGO32TMP));
                Info(slide, 0, ((char *)slide, LoadFarString(EnvOptFormat),
                  LoadFarStringSmall(EnvGO32TMP),
                  (envptr == (char *)NULL || *envptr == 0)?
                  LoadFarStringSmall2(None) : envptr));
#endif /* __GO32__ */
#endif /* !__RSXNT__ */
#ifdef RISCOS
                envptr = getenv(LoadFarStringSmall(EnvUnZipExts));
                Info(slide, 0, ((char *)slide, LoadFarString(EnvOptFormat),
                  LoadFarStringSmall(EnvUnZipExts),
                  (envptr == (char *)NULL || *envptr == 0)?
                  LoadFarStringSmall2(None) : envptr));
#endif /* RISCOS */
            }
            return 0;
        }
        if (!G.noargs && !error)
            error = PK_PARAM;   /* had options (not -h or -v) but no zipfile */
#endif /* !SFX */
        return USAGE(error);
    }

#ifdef SFX
    /* print our banner unless we're being fairly quiet */
    if (G.qflag < 2)
        Info(slide, error? 1 : 0, ((char *)slide, LoadFarString(UnzipSFXBanner),
          UZ_MAJORVER, UZ_MINORVER, PATCHLEVEL, BETALEVEL,
          LoadFarStringSmall(VersionDate)));
#ifdef BETA
    /* always print the beta warning:  no unauthorized distribution!! */
    Info(slide, error? 1 : 0, ((char *)slide, LoadFarString(BetaVersion), "\n",
      "SFX"));
#endif
#endif /* SFX */

    if (G.cflag || G.tflag || G.vflag || G.zflag
#ifdef TIMESTAMP
                                                 || G.T_flag
#endif
                                                            )
        G.extract_flag = FALSE;
    else
        G.extract_flag = TRUE;

#if (!defined(SFX) || defined(SFX_EXDIR))
    if (G.dflag) {
        if (G.extract_flag) {
            G.create_dirs = !G.fflag;
            if ((error = checkdir(__G__ exdir, ROOT)) > 2)
                return error;   /* out of memory, or file in way */
        } else /* -d ignored */
            Info(slide, 0x401, ((char *)slide, LoadFarString(NotExtracting)));
    }
#endif /* !SFX || SFX_EXDIR */

    *pargc = argc;
    *pargv = argv;
    return 0;

} /* end function uz_opts() */





/********************/
/* Function usage() */
/********************/

#ifdef SFX
#  ifdef VMS
#    define LOCAL "X.  Quote uppercase options"
#  endif
#  ifdef UNIX
#    define LOCAL "X"
#  endif
#  ifdef DOS_OS2_W32
#    define LOCAL "s$"
#  endif
#  ifdef FLEXOS
#    define LOCAL "s"
#  endif
#  ifdef AMIGA
#    define LOCAL "N"
#  endif
   /* Default for all other systems: */
#  ifndef LOCAL
#    define LOCAL ""
#  endif

#  ifdef MORE
#    define SFXOPT1 "M"
#  else
#    define SFXOPT1 ""
#  endif

int usage(__G__ error)   /* return PK-type error code */
    int error;
     __GDEF
{
    Info(slide, error? 1 : 0, ((char *)slide, LoadFarString(UnzipSFXBanner),
      UZ_MAJORVER, UZ_MINORVER, PATCHLEVEL, BETALEVEL,
      LoadFarStringSmall(VersionDate)));
    Info(slide, error? 1 : 0, ((char *)slide, LoadFarString(UnzipSFXOpts),
      SFXOPT1, LOCAL));
#ifdef BETA
    Info(slide, error? 1 : 0, ((char *)slide, LoadFarString(BetaVersion), "\n",
      "SFX"));
#endif

    if (error)
        return PK_PARAM;
    else
        return PK_COOL;     /* just wanted usage screen: no error */

} /* end function usage() */





#else /* !SFX */
#  ifdef VMS
#    define QUOT '\"'
#    define QUOTS "\""
#  else
#    define QUOT ' '
#    define QUOTS ""
#  endif

int usage(__G__ error)   /* return PK-type error code */
    int error;
    __GDEF
{
    int flag = (error? 1 : 0);


/*---------------------------------------------------------------------------
    Print either ZipInfo usage or UnZip usage, depending on incantation.
    (Strings must be no longer than 512 bytes for Turbo C, apparently.)
  ---------------------------------------------------------------------------*/

    if (G.zipinfo_mode) {

#ifndef NO_ZIPINFO

        Info(slide, flag, ((char *)slide, LoadFarString(ZipInfoUsageLine1),
          ZI_MAJORVER, ZI_MINORVER, PATCHLEVEL, BETALEVEL,
          LoadFarStringSmall(VersionDate),
          LoadFarStringSmall2(ZipInfoExample), QUOTS,QUOTS));
        Info(slide, flag, ((char *)slide, LoadFarString(ZipInfoUsageLine2)));
        Info(slide, flag, ((char *)slide, LoadFarString(ZipInfoUsageLine3),
          QUOT,QUOT, QUOT,QUOT, LoadFarStringSmall(ZipInfoUsageLine4)));
#ifdef VMS
        Info(slide, flag, ((char *)slide, "\nRemember that non-lowercase\
 filespecs must be quoted in VMS (e.g., \"Makefile\").\n"));
#endif

#endif /* !NO_ZIPINFO */

    } else {   /* UnZip mode */

        Info(slide, flag, ((char *)slide, LoadFarString(UnzipUsageLine1),
          UZ_MAJORVER, UZ_MINORVER, PATCHLEVEL, BETALEVEL,
          LoadFarStringSmall(VersionDate)));
#ifdef BETA
        Info(slide, flag, ((char *)slide, LoadFarString(BetaVersion), "", ""));
#endif

        Info(slide, flag, ((char *)slide, LoadFarString(UnzipUsageLine2),
          ZIPINFO_MODE_OPTION, LoadFarStringSmall(ZipInfoMode)));
#ifdef VMS
        if (!error)  /* maybe no command-line tail found; show extra help */
            Info(slide, flag, ((char *)slide, LoadFarString(VMSusageLine2b)));
#endif

        Info(slide, flag, ((char *)slide, LoadFarString(UnzipUsageLine3),
          LoadFarStringSmall(local1)));

        Info(slide, flag, ((char *)slide, LoadFarString(UnzipUsageLine4),
          QUOT,QUOT, QUOT,QUOT, LoadFarStringSmall(local2), QUOT,QUOT,
          LoadFarStringSmall2(local3)));

        /* This is extra work for SMALL_MEM, but it will work since
         * LoadFarStringSmall2 uses the same buffer.  Remember, this
         * is a hack. */
        Info(slide, flag, ((char *)slide, LoadFarString(UnzipUsageLine5),
          LoadFarStringSmall(Example2), LoadFarStringSmall2(Example3),
          LoadFarStringSmall2(Example3)));

    } /* end if (G.zipinfo_mode) */

    if (error)
        return PK_PARAM;
    else
        return PK_COOL;     /* just wanted usage screen: no error */

} /* end function usage() */

#endif /* ?SFX */
#endif /* !WINDLL */
