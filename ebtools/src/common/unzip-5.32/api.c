/*---------------------------------------------------------------------------

  api.c

  This module supplies an UnZip engine for use directly from C/C++
  programs.  The functions are:

    UzpVer *UzpVersion(void);
    void UzpVersion2(UzpVer2 *version)
    int UzpMain(int argc, char *argv[]);
    int UzpAltMain(int argc, char *argv[], UzpInit *init);
    int UzpUnzipToMemory(char *zip, char *file, UzpBuffer *retstr);
    int UzpValidate(char *archive, int AllCodes);
    int UzpGrep(char *archive, char *file, char *pattern, int cmd, int SkipBin);

  OS/2 only (for now):

    int UzpFileTree(char *name, cbList(callBack), char *cpInclude[],
          char *cpExclude[]);

  You must define `DLL' in order to include the API extensions.

  ---------------------------------------------------------------------------*/


#ifdef OS2
#  define  INCL_DOSMEMMGR
#  include <os2.h>
#endif
#include <setjmp.h>

#define UNZIP_INTERNAL
#include "unzip.h"
#include "version.h"
#ifdef WINDLL
#  include "windll\windll.h"
#endif
#ifdef USE_ZLIB
#  include "zlib.h"
#endif


jmp_buf dll_error_return;

/*---------------------------------------------------------------------------
    Documented API entry points
  ---------------------------------------------------------------------------*/


UzpVer * UZ_EXP UzpVersion()   /* should be pointer to const struct */
{
    static UzpVer version;     /* doesn't change between calls */


    version.structlen = UZPVER_LEN;

#ifdef BETA
    version.flag = 1;
#else
    version.flag = 0;
#endif
    version.betalevel = BETALEVEL;
    version.date = VERSION_DATE;

#ifdef ZLIB_VERSION
    version.zlib_version = ZLIB_VERSION;
    version.flag |= 2;
#else
    version.zlib_version = NULL;
#endif

    /* someday each of these may have a separate patchlevel: */
    version.unzip.major = UZ_MAJORVER;
    version.unzip.minor = UZ_MINORVER;
    version.unzip.patchlevel = PATCHLEVEL;

    version.zipinfo.major = ZI_MAJORVER;
    version.zipinfo.minor = ZI_MINORVER;
    version.zipinfo.patchlevel = PATCHLEVEL;

    /* these are retained for backward compatibility only: */
    version.os2dll.major = UZ_MAJORVER;
    version.os2dll.minor = UZ_MINORVER;
    version.os2dll.patchlevel = PATCHLEVEL;

    version.windll.major = UZ_MAJORVER;
    version.windll.minor = UZ_MINORVER;
    version.windll.patchlevel = PATCHLEVEL;

    return &version;
}

void UZ_EXP UzpVersion2(UzpVer2 *version)
{

    version->structlen = UZPVER_LEN;

#ifdef BETA
    version->flag = 1;
#else
    version->flag = 0;
#endif
    strcpy(version->betalevel, BETALEVEL);
    strcpy(version->date, VERSION_DATE);

#ifdef ZLIB_VERSION
    strcpy(version->zlib_version, ZLIB_VERSION);
    version->flag |= 2;
#else
    version->zlib_version[0] = '\0';
#endif

    /* someday each of these may have a separate patchlevel: */
    version->unzip.major = UZ_MAJORVER;
    version->unzip.minor = UZ_MINORVER;
    version->unzip.patchlevel = PATCHLEVEL;

    version->zipinfo.major = ZI_MAJORVER;
    version->zipinfo.minor = ZI_MINORVER;
    version->zipinfo.patchlevel = PATCHLEVEL;

    /* these are retained for backward compatibility only: */
    version->os2dll.major = UZ_MAJORVER;
    version->os2dll.minor = UZ_MINORVER;
    version->os2dll.patchlevel = PATCHLEVEL;

    version->windll.major = UZ_MAJORVER;
    version->windll.minor = UZ_MINORVER;
    version->windll.patchlevel = PATCHLEVEL;
}





#ifndef WINDLL

int UZ_EXP UzpAltMain(int argc, char *argv[], UzpInit *init)
{
    int r, (*dummyfn)();


    CONSTRUCTGLOBALS();

    if (init->structlen >= (sizeof(ulg) + sizeof(dummyfn)) && init->msgfn)
        G.message = init->msgfn;

    if (init->structlen >= (sizeof(ulg) + 2*sizeof(dummyfn)) && init->inputfn)
        G.input = init->inputfn;

    if (init->structlen >= (sizeof(ulg) + 3*sizeof(dummyfn)) && init->pausefn)
        G.mpause = init->pausefn;

    if (init->structlen >= (sizeof(ulg) + 4*sizeof(dummyfn)) && init->userfn)
        (*init->userfn)();    /* allow void* arg? */

    r = unzip(__G__ argc, argv);
    DESTROYGLOBALS()
    RETURN(r);
}

#endif





int UZ_EXP UzpUnzipToMemory(char *zip, char *file, UzpBuffer *retstr)
{
    int r;
#if (defined(WINDLL) && !defined(CRTL_CP_IS_ISO))
    char *intern_zip, *intern_file;
#endif

    CONSTRUCTGLOBALS();
#if (defined(WINDLL) && !defined(CRTL_CP_IS_ISO))
   intern_zip = (char *)malloc(strlen(zip)+1);
   if (intern_zip == NULL)
      return PK_MEM;
   intern_file = (char *)malloc(strlen(file)+1);
   if (intern_file == NULL) {
      free(intern_zip);
      return PK_MEM;
   }
   ISO_TO_INTERN(zip, intern_zip);
   ISO_TO_INTERN(file, intern_file);
#  define zip intern_zip
#  define file intern_file
#endif
    G.redirect_data = 1;
    r = unzipToMemory(__G__ zip, file, retstr)==0;
    DESTROYGLOBALS()
#if (defined(WINDLL) && !defined(CRTL_CP_IS_ISO))
#  undef file
#  undef zip
    free(intern_file);
    free(intern_zip);
#endif
    return r;
}





#ifdef OS2DLL

int UZ_EXP UzpFileTree(char *name, cbList(callBack), char *cpInclude[],
                char *cpExclude[])
{
    int r;

    CONSTRUCTGLOBALS();
    G.qflag = 2;
    G.vflag = 1;
    G.C_flag = 1;
    G.wildzipfn = name;
    G.process_all_files = TRUE;
    if (cpInclude) {
        char **ptr = cpInclude;

        while (*ptr != NULL) ptr++;
        G.filespecs = ptr - cpInclude;
        G.pfnames = cpInclude, G.process_all_files = FALSE;
    }
    if (cpExclude) {
        char **ptr = cpExclude;

        while (*ptr != NULL) ptr++;
        G.xfilespecs = ptr - cpExclude;
        G.pxnames = cpExclude, G.process_all_files = FALSE;
    } else

    G.processExternally = callBack;
    r = process_zipfiles(__G)==0;
    DESTROYGLOBALS()
    return r;
}

#endif /* OS2DLL */




/*---------------------------------------------------------------------------
    Helper functions
  ---------------------------------------------------------------------------*/


void setFileNotFound(__G)
    __GDEF
{
    G.filenotfound++;
}



int unzipToMemory(__GPRO__ char *zip, char *file, UzpBuffer *retstr)
{
    int r;
    char *incname[2];

    G.process_all_files = FALSE;
    G.extract_flag = TRUE;
    G.qflag = 2;
    G.C_flag = 1;
    G.wildzipfn = zip;

    G.pfnames = incname;
    incname[0] = file;
    incname[1] = NULL;
    G.filespecs = 1;

    redirect_outfile(__G);
    r = process_zipfiles(__G);
    if (retstr) {
        retstr->strptr = (char *)G.redirect_buffer;
        retstr->strlength = G.redirect_size;
    }
    r |= G.filenotfound;
    if (r)
        return r;   /* GRR:  these two lines don't make much sense... */
    return r;
}



int redirect_outfile(__G)
     __GDEF
{
    G.redirect_size = G.lrec.ucsize;
#ifdef OS2
    DosAllocMem((void **)&G.redirect_buffer, G.redirect_size+1,
      PAG_READ|PAG_WRITE|PAG_COMMIT);
    G.redirect_pointer = G.redirect_buffer;
#else
    G.redirect_pointer = G.redirect_buffer = malloc(G.redirect_size+1);
#endif
    if (!G.redirect_buffer)
        return FALSE;
    G.redirect_pointer[G.redirect_size] = 0;
    return TRUE;
}



int writeToMemory(__GPRO__ uch *rawbuf, ulg size)
{
    if (rawbuf != G.redirect_pointer)
        memcpy(G.redirect_pointer,rawbuf,size);
    G.redirect_pointer += size;
    return 0;
}





/* Purpose: Determine if file in archive contains the string szSearch

   Parameters: archive  = archive name
               file     = file contained in the archive. This cannot be
                          a wild card to be meaningful
               pattern  = string to search for
               cmd      = 0 - case-insensitive search
                          1 - case-sensitve search
                          2 - case-insensitive, whole words only
                          3 - case-sensitive, whole words only
               SkipBin  = if true, skip any files that have control
                          characters other than CR, LF, or tab in the first
                          100 characters.

   Returns:    TRUE if a match is found
               FALSE if no match is found
               -1 on error

   Comments: This does not pretend to be as useful as the standard
             Unix grep, which returns the strings associated with a
             particular pattern, nor does it search past the first
             matching occurrence of the pattern.
 */

int UZ_EXP UzpGrep(char *archive, char *file, char *pattern, int cmd,
                   int SkipBin)
{
    int retcode = FALSE, compare;
    ulg i, j, patternLen, buflen;
    char * sz, *p;
    UzpBuffer retstr;
#ifdef WINDLL
    LPUSERFUNCTIONS lpTemp;
#endif


#ifdef WINDLL
    /* Turn off any windows printing functions, as they may not have been
     * identified yet. There is no requirement that we initialize the
     * dll with printing stuff for this. */
    UzpNoPrinting(TRUE);

    /* If we haven't initialized the Windows user functions, the pointer
     * for the comment flag may simply have garbage in it, and as a result
     * will take off into never-never land. Save it off, make it null, and
     * restore it below. */
    lpTemp = lpUserFunctions;
    lpUserFunctions = NULL;
#endif

    if (!UzpUnzipToMemory(archive, file, &retstr)) {
#ifdef WINDLL
       lpUserFunctions = lpTemp;
       UzpNoPrinting(FALSE);
#endif
       return -1;   /* not enough memory, file not found, or other error */
    }

    if (SkipBin) {
        if (retstr.strlength < 100)
            buflen = retstr.strlength;
        else
            buflen = 100;
        for (i = 0; i < buflen; i++) {
            if (iscntrl(retstr.strptr[i])) {
                if ((retstr.strptr[i] != 0x0A) &&
                    (retstr.strptr[i] != 0x0D) &&
                    (retstr.strptr[i] != 0x09))
                {
                    /* OK, we now think we have a binary file of some sort */
                    free(retstr.strptr);
#ifdef WINDLL
                    lpUserFunctions = lpTemp;
                    UzpNoPrinting(FALSE);
#endif
                    return FALSE;
                }
            }
        }
    }

    patternLen = strlen(pattern);

    if (retstr.strlength < patternLen) {
#ifdef WINDLL
        lpUserFunctions = lpTemp;
        UzpNoPrinting(FALSE);
#endif
        return FALSE;
    }

    sz = malloc(patternLen + 3); /* add two in case doing whole words only */
    if (cmd > 1) {
        strcpy(sz, " ");
        strcat(sz, pattern);
        strcat(sz, " ");
    } else
        strcpy(sz, pattern);

    if ((cmd == 0) || (cmd == 2)) {
        for (i = 0; i < strlen(sz); i++)
            sz[i] = toupper(sz[i]);
        for (i = 0; i < retstr.strlength; i++)
            retstr.strptr[i] = toupper(retstr.strptr[i]);
    }

    for (i = 0; i < (retstr.strlength - patternLen); i++) {
        p = &retstr.strptr[i];
        compare = TRUE;
        for (j = 0; j < patternLen; j++) {
            /* We cannot do strncmp here, as we may be dealing with a
             * "binary" file, such as a word processing file, or perhaps
             * even a true executable of some sort. */
            if (p[j] != sz[j]) {
                compare = FALSE;
                break;
            }
        }
        if (compare == TRUE) {
            retcode = TRUE;
            break;
        }
    }

    free(sz);
    free(retstr.strptr);

#ifdef WINDLL
    lpUserFunctions = lpTemp;
    UzpNoPrinting(FALSE);
#endif

    return retcode;
}





int UZ_EXP UzpValidate(char *archive, int AllCodes)
{
    int retcode;
    CONSTRUCTGLOBALS();

    G.jflag = 1;
    G.tflag = 1;
    G.overwrite_none = 0;
    G.extract_flag = (!G.zipinfo_mode &&
                      !G.cflag && !G.tflag && !G.vflag && !G.zflag
#ifdef TIMESTAMP
                      && !G.T_flag
#endif
                     );

    G.qflag = 2;               /* turn off all messages */
    G.fValidate = TRUE;
    G.pfnames = &fnames[0];    /* assign default filename vector */
#ifdef WINDLL
    UzpNoPrinting(TRUE);
#endif

    if (archive == NULL) {     /* something is screwed up:  no filename */
        DESTROYGLOBALS();
        return PK_NOZIP;
    }

    G.wildzipfn = (char *)malloc(FILNAMSIZ + 1);
    strcpy(G.wildzipfn, archive);
#if (defined(WINDLL) && !defined(CRTL_CP_IS_ISO))
    _ISO_INTERN(G.wildzipfn);
#endif

    G.process_all_files = TRUE;       /* for speed */

    retcode = setjmp(dll_error_return);

    if (retcode) {
#ifdef WINDLL
        UzpNoPrinting(FALSE);
#endif
        free(G.wildzipfn);
        DESTROYGLOBALS();
        return PK_BADERR;
    }

    retcode = process_zipfiles(__G);

    free(G.wildzipfn);
#ifdef WINDLL
    UzpNoPrinting(FALSE);
#endif
    DESTROYGLOBALS();

    /* PK_WARN == 1 and PK_FIND == 11. When we are just looking at an
       archive, we should still be able to see the files inside it,
       even if we can't decode them for some reason.

       We also still want to be able to get at files even if there is
       something odd about the zip archive, hence allow PK_WARN,
       PK_FIND, IZ_UNSUP as well as PK_ERR
     */

    if (AllCodes)
        return retcode;

    if ((retcode == PK_OK) || (retcode == PK_WARN) || (retcode == PK_ERR) ||
        (retcode == IZ_UNSUP) || (retcode == PK_FIND))
        return TRUE;
    else
        return FALSE;
}
