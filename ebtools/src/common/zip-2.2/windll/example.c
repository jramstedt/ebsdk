/*
 A very simplistic example of how to load the zip dll and make a call into it.
 Note that none of the command line options are implemented in this example.

 */

#define WIN32
#define API

#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#ifdef __BORLANDC__
#include <dir.h>
#else
#include <direct.h>
#endif
#include "example.h"
#include "zipver.h"

#ifdef WIN32
#include <commctrl.h>
#include <winver.h>
#else
#include <ver.h>
#endif

#ifdef WIN32
#define ZIP_DLL_NAME "ZIP32.DLL\0"
#else
#define ZIP_DLL_NAME "ZIP16.DLL\0"
#endif

#define DLL_WARNING "Cannot find %s."\
            " The Dll must be in the application directory, the path, "\
            "the Windows directory or the Windows System directory."
#define DLL_VERSION_WARNING "%s has the wrong version number."\
            " Insure that you have the correct dll's installed, and that "\
            "an older dll is not in your path or Windows System directory."

int hFile;              /* file handle */

ZCL ZpZCL;
LPZIPUSERFUNCTIONS lpZipUserFunctions;
HANDLE hZUF = (HANDLE)NULL;
HINSTANCE hUnzipDll;
HANDLE hFileList;
ZPOPT ZpOpt;
#ifdef WIN32
DWORD dwPlatformId = 0xFFFFFFFF;
#endif
HINSTANCE hZipDll;


/* Forward References */
_DLL_ZIP ZpArchive;
_ZIP_USER_FUNCTIONS ZpInit;
ZIPSETOPTIONS ZpSetOptions;

void FreeUpMemory(void);
int WINAPI DummyPassword(char *, int, const char *, const char *);
int WINAPI DummyPrint(char far *, unsigned long);
char far * WINAPI DummyComment(char far *);

#ifdef WIN32
BOOL IsNT(VOID);
#endif

/****************************************************************************

    FUNCTION: Main(int argc, char **argv)

****************************************************************************/
#ifdef __BORLANDC__
#  ifdef WIN32
#pragma argsused
#  endif
#endif
int main(int argc, char **argv)
{
LPSTR szFileList;
char **index, *sz;
int retcode, i, cc;
DWORD dwVerInfoSize;
DWORD dwVerHnd;
char szFullPath[PATH_MAX];
#ifdef WIN32
char *ptr;
#else
HFILE hfile;
OFSTRUCT ofs;
#endif
HANDLE  hMem;         /* handle to mem alloc'ed */

if (argc < 3)
   return 0;           /* Exits if not proper number of arguments */

hZUF = GlobalAlloc( GPTR, (DWORD)sizeof(ZIPUSERFUNCTIONS));
if (!hZUF)
   {
   return 0;
   }
lpZipUserFunctions = (LPZIPUSERFUNCTIONS)GlobalLock(hZUF);

if (!lpZipUserFunctions)
   {
   GlobalFree(hZUF);
   return 0;
   }

lpZipUserFunctions->print = DummyPrint;
lpZipUserFunctions->password = DummyPassword;
lpZipUserFunctions->comment = DummyComment;

/* Let's go find the dll */
#ifdef WIN32
if (SearchPath(
    NULL,               /* address of search path               */
    ZIP_DLL_NAME,       /* address of filename                  */
    NULL,               /* address of extension                 */
    PATH_MAX,           /* size, in characters, of buffer       */
    szFullPath,         /* address of buffer for found filename */
    &ptr                /* address of pointer to file component */
   ) == 0)
#else
hfile = OpenFile(ZIP_DLL_NAME,  &ofs, OF_SEARCH);
if (hfile == HFILE_ERROR)
#endif
   {
   char str[256];
   wsprintf (str, DLL_WARNING, ZIP_DLL_NAME);
   printf("%s\n", str);
   FreeUpMemory();
   return 0;
   }
#ifndef WIN32
else
   lstrcpy(szFullPath, ofs.szPathName);
_lclose(hfile);
#endif

/* Now we'll check the zip dll version information */
dwVerInfoSize =
    GetFileVersionInfoSize(szFullPath, &dwVerHnd);

if (dwVerInfoSize)
   {
   BOOL  fRet, fRetName;
   char str[256];
   LPSTR   lpstrVffInfo; /* Pointer to block to hold info */
   LPSTR lszVer = NULL;
   LPSTR lszVerName = NULL;
   UINT  cchVer = 0;

   /* Get a block big enough to hold the version information */
   hMem          = GlobalAlloc(GMEM_MOVEABLE, dwVerInfoSize);
   lpstrVffInfo  = GlobalLock(hMem);

   /* Get the version information */
   GetFileVersionInfo(szFullPath, 0L, dwVerInfoSize, lpstrVffInfo);
   fRet = VerQueryValue(lpstrVffInfo,
              TEXT("\\StringFileInfo\\040904E4\\FileVersion"),
               (LPVOID)&lszVer,
               &cchVer);
   fRetName = VerQueryValue(lpstrVffInfo,
               TEXT("\\StringFileInfo\\040904E4\\CompanyName"),
              (LPVOID)&lszVerName,
              &cchVer);
   if (!fRet || !fRetName ||
      (lstrcmpi(lszVer, ZIP_DLL_VERSION) != 0) ||
      (lstrcmpi(lszVerName, COMPANY_NAME) != 0))
      {
      wsprintf (str, DLL_VERSION_WARNING, ZIP_DLL_NAME);
      printf("%s\n", str);
      FreeUpMemory();
      GlobalUnlock(hMem);
      GlobalFree(hMem);
      return 0;
      }
   /* free memory */
   GlobalUnlock(hMem);
   GlobalFree(hMem);
   }
else
   {
   char str[256];
   wsprintf (str, DLL_VERSION_WARNING, ZIP_DLL_NAME);
   printf("%s\n", str);
   FreeUpMemory();
   GlobalUnlock(hMem);
   GlobalFree(hMem);
   return 0;
   }
/* Okay, now we know that the dll exists, and has the proper version
 * information in it. We can go ahead and load it.
 */
hZipDll = LoadLibrary(ZIP_DLL_NAME);
#ifndef WIN32
if (hZipDll > HINSTANCE_ERROR)
#else
if (hZipDll != NULL)
#endif
   {
   (_DLL_ZIP)ZpArchive = (_DLL_ZIP)GetProcAddress(hZipDll, "ZpArchive");
   (ZIPSETOPTIONS)ZpSetOptions = (ZIPSETOPTIONS)GetProcAddress(hZipDll, "ZpSetOptions");
   if (!ZpArchive || !ZpSetOptions)
      {
      char str[256];
      wsprintf (str, "Could not get entry point to %s", ZIP_DLL_NAME);
      MessageBox((HWND)NULL, str, "Info-ZIP Example", MB_ICONSTOP | MB_OK);
      FreeUpMemory();
      return 0;
      }
   }
else
   {
   char str[256];
   wsprintf (str, "Could not load %s", ZIP_DLL_NAME);
   printf("%s\n", str);
   FreeUpMemory();
   return 0;
   }

(_ZIP_USER_FUNCTIONS)ZpInit = (_ZIP_USER_FUNCTIONS)GetProcAddress(hZipDll, "ZpInit");
if (!ZpInit)
   {
   printf("Cannot get address of ZpInit in Zip dll. Terminating...");
   FreeLibrary(hZipDll);
   FreeUpMemory();
   return 0;
   }
if (!(*ZpInit)(lpZipUserFunctions))
   {
   printf("Application functions not set up properly. Terminating...");
   FreeLibrary(hZipDll);
   FreeUpMemory();
   return 0;
   }

/* Here is where the action starts */
ZpOpt.fSuffix = FALSE;        /* include suffixes (not yet implemented) */
ZpOpt.fEncrypt = FALSE;       /* true if encryption wanted */
ZpOpt.fSystem = FALSE;        /* true to include system/hidden files */
ZpOpt.fVolume = FALSE;        /* true if storing volume label */
ZpOpt.fExtra = FALSE;         /* true if including extra attributes */
ZpOpt.fNoDirEntries = FALSE;  /* true if ignoring directory entries */
ZpOpt.fDate = FALSE;          /* true if excluding files earlier than a
                                  specified date */
ZpOpt.fVerbose = FALSE;       /* true if full messages wanted */
ZpOpt.fQuiet = FALSE;         /* true if minimum messages wanted */
ZpOpt.fCRLF_LF = FALSE;       /* true if translate CR/LF to LF */
ZpOpt.fLF_CRLF = FALSE;       /* true if translate LF to CR/LF */
ZpOpt.fJunkDir = FALSE;       /* true if junking directory names */
ZpOpt.fRecurse = FALSE;       /* true if recursing into subdirectories */
ZpOpt.fGrow = FALSE;          /* true if allow appending to zip file */
ZpOpt.fForce = FALSE;         /* true if making entries using DOS names */
ZpOpt.fMove = FALSE;          /* true if deleting files added or updated */
ZpOpt.fUpdate = FALSE;        /* true if updating zip file--overwrite only
                                  if newer */
ZpOpt.fFreshen = FALSE;       /* true if freshening zip file--overwrite only */
ZpOpt.fJunkSFX = FALSE;       /* true if junking sfx prefix*/
ZpOpt.fLatestTime = FALSE;    /* true if setting zip file time to time of
                                  latest file in archive */
ZpOpt.fComment = FALSE;       /* true if putting comment in zip file */
ZpOpt.fOffsets = FALSE;       /* true if updating archive offsets for sfx
                                  files */
ZpOpt.fDeleteEntries = FALSE; /* true if deleting files from archive */
ZpOpt.Date[0] = '\0';         /* Not using, set to 0 length */
getcwd(ZpOpt.szRootDir, MAX_PATH);    /* Set directory to current directory */

ZpZCL.argc = argc - 2;        /* number of files to archive - adjust for the
                                  actual number of file names to be added */
ZpZCL.lpszZipFN = argv[1];    /* archive to be created/updated */

/* Copy over the appropriate portions of argv, basically stripping out argv[0]
   (name of the executable) and argv[1] (name of the archive file)
 */
hFileList = GlobalAlloc( GPTR, 0x10000L);
if ( hFileList )
   {
   szFileList = (char far *)GlobalLock(hFileList);
   }
index = (char **)szFileList;
cc = (sizeof(char *) * ZpZCL.argc);
sz = szFileList + cc;

for (i = 0; i < ZpZCL.argc; i++)
    {
    cc = lstrlen(argv[i+2]);
    lstrcpy(sz, argv[i+2]);
    index[i] = sz;
    sz += (cc + 1);
    }
ZpZCL.FNV = (char **)szFileList;  /* list of files to archive */

/* Set the options */
ZpSetOptions(ZpOpt);

/* Go zip 'em up */
retcode = ZpArchive(ZpZCL);
if (retcode != 0)
   printf("Error in archiving\n");

GlobalUnlock(hFileList);
GlobalFree(hFileList);
FreeUpMemory();
FreeLibrary(hZipDll);
return 1;
}

void FreeUpMemory(void)
{
if (hZUF)
   {
   GlobalUnlock(hZUF);
   GlobalFree(hZUF);
   }
}

#ifdef WIN32
/* This simply determines if we are running on NT */
BOOL IsNT(VOID)
{
if(dwPlatformId != 0xFFFFFFFF)
   return dwPlatformId;
else
/* note: GetVersionEx() doesn't exist on WinNT 3.1 */
   {
   if(GetVersion() < 0x80000000)
      {
      (BOOL)dwPlatformId = TRUE;
      }
   else
      {
      (BOOL)dwPlatformId = FALSE;
      }
    }
return dwPlatformId;
}
#endif

/* Password entry routine - see password.c in the wiz directory for how
   this is actually implemented in Wiz. If you have an encrypted file,
   this will probably give you great pain. Note that none of the
   parameters are being used here, and this will give you warnings.
 */
int WINAPI DummyPassword(char *p, int n, const char *m, const char * name)
{
return 1;
}

/* Dummy "print" routine that simply outputs what is sent from the dll */
int WINAPI DummyPrint(char far *buf, unsigned long size)
{
printf("%s", buf);
return (unsigned int) size;
}


/* Dummy "comment" routine. See comment.c in the wiz directory for how
   this is actually implemented in Wiz. This will probably cause you
   great pain if you ever actually make a call into it.
 */
char far * WINAPI DummyComment(char far *szBuf)
{
szBuf[0] = '\0';
return szBuf;
}

