//******************************************************************************
//
// File:        INTRFACE.CPP
//
// Description: This module acts as the interface between the Info-ZIP code and
//              our Windows code in WINMAIN.CPP.  We expose the needed
//              functions to query a file list, test file(s), extract file(s),
//              and display a zip file comment.  The windows code is never
//              bothered with understanding the Globals structure.
//
//              This module also catches all the callbacks from the Info-ZIP
//              code, cleans up the data provided in the callback, and then
//              forwards the information to the appropriate function in the
//              windows code.  These callbacks include status messages, file
//              lists, comments, password prompt, and file overwrite prompts.
//
//              Finally, this module implements the few functions that the
//              Info-ZIP code expects the port to implement. These functions are
//              OS dependent and are mostly related to validating file names and
//              directoies, and setting file attributes and dates of saved files.
//
// Copyright:   All the source files for Pocket UnZip, except for components
//              written by the Info-ZIP group, are copyrighted 1997 by Steve P.
//              Miller.  The product "Pocket UnZip" itself is property of the
//              author and cannot be altered in any way without written consent
//              from Steve P. Miller.
//
// Disclaimer:  All project files are provided "as is" with no guarantee of
//              their correctness.  The authors are not liable for any outcome
//              that is the result of using this source.  The source for Pocket
//              UnZip has been placed in the public domain to help provide an
//              understanding of its implementation.  You are hereby granted
//              full permission to use this source in any way you wish, except
//              to alter Pocket UnZip itself.  For comments, suggestions, and
//              bug reports, please write to stevemil@pobox.com.
//
// Functions:   DoListFiles
//              DoExtractOrTestFiles
//              DoGetComment
//              SetExtractToDirectory
//              InitGlobals
//              FreeGlobals
//              IsFileOrDirectory
//              SmartCreateDirectory
//              ExtractOrTestFilesThread
//              CheckForAbort
//              SetCurrentFile
//              UzpMessagePrnt2
//              UzpInput2
//              UzpMorePause
//              UzpPassword
//              UzpReplace
//              UzpSound
//              SendAppMsg
//              win_fprintf
//              mapattr
//              utimeToFileTime
//              GetFileTimes
//              close_outfile
//              do_wild
//              mapname
//              test_NT
//              checkdir
//              match
//              iswild
//              IsOldFileSystem
//
//
// Date      Name          History
// --------  ------------  -----------------------------------------------------
// 02/01/97  Steve Miller  Created (Version 1.0 using Info-ZIP UnZip 5.30)
//
//******************************************************************************


//******************************************************************************
#if 0 // The following information and structure are here just for reference
//******************************************************************************
//
// The Windows CE version of Unzip builds with the following defines set:
//
//
//    WIN32
//    _WINDOWS
//    UNICODE
//    _UNICODE
//    WIN32_LEAN_AND_MEAN
//    STRICT
//
//    POCKET_UNZIP         (Main define - Always set)
//
//    UNZIP_INTERNAL
//    WINDLL
//    DLL
//    REENTRANT
//    USE_EF_UT_TIME
//    NO_ZIPINFO
//    NO_STDDEF_H
//    NO_NTSD_WITH_RSXNT
//
//    USE_SMITH_CODE       (optional - See COPYING document)
//    USE_UNSHRINK         (optional - See COPYING document)
//
//    DEBUG                (When building for Debug)
//    _DEBUG               (When building for Debug)
//    NDEBUG               (When building for Retail)
//    _NDEBUG              (When building for Retail)
//
//    _WIN32_WCE=100       (When building for Windows CE native)
//
// This causes our Globals structure to look like the following.  The only
// things we care about is this Globals structure, the process_zipfiles()
// function, and a few callback functions.  The Info-ZIP code has not been
// been modified in any way.
//

struct Globals {
   int            zipinfo_mode;         // behave like ZipInfo or like normal UnZip?
   int            aflag;                // -a: do ASCII-EBCDIC and/or end-of-line translation
   int            cflag;                // -c: output to stdout
   int            C_flag;               // -C: match filenames case-insensitively
   int            dflag;                // -d: all args are files/dirs to be extracted
   int            fflag;                // -f: "freshen" (extract only newer files)
   int            hflag;                // -h: header line (zipinfo)
   int            jflag;                // -j: junk pathnames (unzip)
   int            lflag;                // -12slmv: listing format (zipinfo)
   int            L_flag;               // -L: convert filenames from some OSes to lowercase
   int            overwrite_none;       // -n: never overwrite files (no prompting)
   int            overwrite_all;        // -o: OK to overwrite files without prompting
   int            P_flag;               // -P: give password on command line (ARGH!)
   int            qflag;                // -q: produce a lot less output
   int            sflag;                // -s: convert spaces in filenames to underscores
   int            volflag;              // -$: extract volume labels
   int            tflag;                // -t: test (unzip) or totals line (zipinfo)
   int            T_flag;               // -T: timestamps (unzip) or dec. time fmt (zipinfo)
   int            uflag;                // -u: "update" (extract only newer/brand-new files)
   int            vflag;                // -v: (verbosely) list directory
   int            V_flag;               // -V: don't strip VMS version numbers
   int            X_flag;               // -X: restore owner/protection or UID/GID or ACLs
   int            zflag;                // -z: display the zipfile comment (only, for unzip)
   int            filespecs;            // number of real file specifications to be matched
   int            xfilespecs;           // number of excluded filespecs to be matched
   int            process_all_files;
   int            create_dirs;          // used by main(), mapname(), checkdir()
   int            extract_flag;
   int            newzip;               // reset in extract.c; used in crypt.c
   LONGINT        real_ecrec_offset;
   LONGINT        expect_ecrec_offset;
   long           csize;                // used by decompr. (NEXTBYTE): must be signed
   long           ucsize;               // used by unReduce(), explode()
   long           used_csize;           // used by extract_or_test_member(), explode()
   int            filenotfound;
   int            redirect_data;        // redirect data to memory buffer
   int            redirect_text;        // redirect text output to buffer
   unsigned       _wsize;
   int            stem_len;
   int            putchar_idx;
   uch           *redirect_pointer;
   uch           *redirect_buffer;
   unsigned       redirect_size;
   char         **pfnames;
   char         **pxnames;
   char           sig[5];
   char           answerbuf[10];
   min_info       info[DIR_BLKSIZ];
   min_info      *pInfo;
   union work     area;                 // see unzpriv.h for definition of work
   ulg near      *crc_32_tab;
   ulg            crc32val;             // CRC shift reg. (was static in funzip)
   uch           *inbuf;                // input buffer (any size is OK)
   uch           *inptr;                // pointer into input buffer
   int            incnt;
   ulg            bitbuf;
   int            bits_left;            // unreduce and unshrink only
   int            zipeof;
   char          *argv0;                // used for NT and EXE_EXTENSION
   char          *wildzipfn;
   char          *zipfn;                // GRR:  MSWIN:  must nuke any malloc'd zipfn...
   int            zipfd;                // zipfile file handle
   LONGINT        ziplen;
   LONGINT        cur_zipfile_bufstart; // extract_or_test, readbuf, ReadByte
   LONGINT        extra_bytes;          // used in unzip.c, misc.c
   uch           *extra_field;          // Unix, VMS, Mac, OS/2, Acorn, ...
   uch           *hold;
   char           local_hdr_sig[5];     // initialize sigs at runtime so unzip
   char           central_hdr_sig[5];   //  executable won't look like a zipfile
   char           end_central_sig[5];

   local_file_hdr lrec;                 // used in unzip.c, extract.c
   cdir_file_hdr  crec;                 // used in unzip.c, extract.c, misc.c
   ecdir_rec      ecrec;                // used in unzip.c, extract.c
   struct stat    statbuf;              // used by main, mapname, check_for_newer

   int            mem_mode;
   uch           *outbufptr;            // extract.c static
   ulg            outsize;              // extract.c static
   int            reported_backslash;   // extract.c static
   int            disk_full;
   int            newfile;

   int            didCRlast;            // fileio static
   ulg            numlines;             // fileio static: number of lines printed
   int            sol;                  // fileio static: at start of line
   int            no_ecrec;             // process static
   FILE          *outfile;
   uch           *outbuf;
   uch           *realbuf;

   uch           *outbuf2;              //  main() (never changes); else malloc'd
   uch           *outptr;
   ulg            outcnt;               // number of chars stored in outbuf
   char          *filename;

   char          *pwdarg;               // pointer to command-line password (-P option)
   int            nopwd;                // crypt static
   ulg            keys[3];              // crypt static: keys defining pseudo-random sequence
   char          *key;                  // crypt static: decryption password or NULL

   unsigned       hufts;                // track memory usage

   struct huft   *fixed_tl;             // inflate static
   struct huft   *fixed_td;             // inflate static
   int            fixed_bl
   int            fixed_bd;             // inflate static
   unsigned       wp;                   // inflate static: current position in slide
   ulg            bb;                   // inflate static: bit buffer
   unsigned       bk;                   // inflate static: bits in bit buffer
   MsgFn         *message;
   InputFn       *input;
   PauseFn       *mpause;
   PasswdFn      *decr_passwd;
   ReplaceFn     *replace;
   SoundFn       *sound;

   int            incnt_leftover;       // so improved NEXTBYTE does not waste input
   uch           *inptr_leftover;

   // These are defined in PUNZIP.H.
   char           matchname[FILNAMSIZ]; // used by do_wild()
   int            notfirstcall;         // used by do_wild()
   char          *zipfnPtr;
   char          *wildzipfnPtr;
};

#endif // #if 0 - This struct is here just for reference

//******************************************************************************

extern "C" {
#define __INTRFACE_CPP__
#define UNZIP_INTERNAL
#include "unzip.h"
#include "crypt.h"     // Needed to pick up CRYPT define
#include <commctrl.h>
#include "intrface.h"
#include "winmain.h"

#ifndef _WIN32_WCE
#include <process.h>   // _beginthreadex() and _endthreadex()
#endif

}
#include <tchar.h> // Must be outside of extern "C" block


//******************************************************************************
//***** "Local" Global Variables
//******************************************************************************

static USERFUNCTIONS  g_uf;
static DCL            g_dcl;
static EXTRACT_INFO  *g_pExtractInfo = NULL;
static FILE_NODE     *g_pFileLast    = NULL;
static CHAR           g_szExtractToDirectory[_MAX_PATH];
static BOOL           g_fOutOfMemory;

//******************************************************************************
//***** Local Function Prototypes
//******************************************************************************

extern "C" {

// Our exposed interface functions to the Info-ZIP core.
BOOL DoListFiles(LPCSTR szZipFile);
BOOL DoExtractOrTestFiles(LPCSTR szZipFile, EXTRACT_INFO *pei);
BOOL DoGetComment(LPCSTR szFile);
BOOL SetExtractToDirectory(LPTSTR szDirectory);

// Internal functions.
struct Globals* InitGlobals(LPCSTR szZipFile);
void FreeGlobals(Globals *pG);
int IsFileOrDirectory(LPCTSTR szPath);
BOOL SmartCreateDirectory(struct Globals *pG, LPCSTR szDirectory);

#ifdef _WIN32_WCE
DWORD WINAPI ExtractOrTestFilesThread(LPVOID lpv);
#else
unsigned __stdcall ExtractOrTestFilesThread(void *lpv);
#endif

void CheckForAbort(struct Globals *pG);
void SetCurrentFile(struct Globals *pG);

// Callbacks from Info-ZIP code.
int UzpMessagePrnt2(zvoid *pG, uch *buffer, ulg size, int flag);
int UzpInput2(zvoid *pG, uch *buffer, int *size, int flag);
void UzpMorePause(zvoid *pG, const char *szPrompt, int flag);
int UzpPassword(zvoid *pG, int *pcRetry, char *szPassword, int nSize,
                const char *szZipFile, const char *szFile);
int WINAPI UzpReplace(char *szFile);
void WINAPI UzpSound(void);
void WINAPI SendAppMsg(ulg dwSize, ulg dwCompressedSize, int ratio, int month,
                       int day, int year, int hour, int minute, int uppercase,
                       char *szPath, char *szMethod, ulg dwCRC);
int win_fprintf(FILE *file, unsigned int dwCount, char far *buffer);

// Functions that Info-ZIP expects the port to write and export.
void utimeToFileTime(time_t ut, FILETIME *pft, BOOL fOldFileSystem);
int GetFileTimes(struct Globals *pG, FILETIME *pftCreated, FILETIME *pftAccessed,
                 FILETIME *pftModified);
int mapattr(struct Globals *pG);
void close_outfile(struct Globals *pG);
char* do_wild(struct Globals *pG, char *wildspec);
int mapname(struct Globals *pG, int renamed);
int test_NT(struct Globals *pG, uch *eb, unsigned eb_size);
int checkdir(struct Globals *pG, char *pathcomp, int flag);

// Check for FAT, VFAT, HPFS, etc.
BOOL IsOldFileSystem(char *szPath);

} // extern "C"


//******************************************************************************
//***** Our exposed interface functions to the Info-ZIP core
//******************************************************************************

int DoListFiles(LPCSTR szZipFile) {

   int result;

   // Create our Globals struct and fill it in whith some default values.
   struct Globals *pG = InitGlobals(szZipFile);
   if (!pG) {
      return PK_MEM;
   }

   pG->vflag = 1; // verbosely: list directory (for WIN32 it is 0 or 1)
   pG->process_all_files = TRUE; // improves speed

   g_pFileLast = NULL;
   g_fOutOfMemory = FALSE;

   // We wrap some exception handling around the entire Info-ZIP engine to be
   // safe.  Since we are running on a device with tight memory configurations,
   // all sorts of problems can arise when we run out of memory.
   __try {

      // Call the unzip routine.  We will catch the file information in a
      // callback to SendAppMsg().
      result = process_zipfiles(pG);

      // Make sure we didn't run out of memory in the process.
      if (g_fOutOfMemory) {
         result = PK_MEM;
      }

   } __except(EXCEPTION_EXECUTE_HANDLER) {

      // Catch any exception here.
      DebugOut(TEXT("Exception 0x%08X occurred in DoListFiles()"),
               GetExceptionCode());
      result = PK_EXCEPTION;
   }

   g_pFileLast = NULL;

   // It is possible that the ZIP engine change the file name a bit (like adding
   // a ".zip" if needed).  If so, we will pick up the new name.
   if ((result != PK_EXCEPTION) && pG->zipfn && *pG->zipfn) {
      strcpy(g_szZipFile, pG->zipfn);
   }

   // Free our globals.
   FreeGlobals(pG);

   return result;
}

//******************************************************************************
BOOL DoExtractOrTestFiles(LPCSTR szZipFile, EXTRACT_INFO *pei) {

   // WARNING!!!  This functions hands the EXTRACT_INFO structure of to a thread
   // to perform the actual extraction/test.  When the thread is done, it will
   // send a message to the progress dialog.  The calling function must not
   // delete the EXTRAT_INFO structure until it receives the message.  Currently,
   // this is not a problem for us since the structure lives on the stack of the
   // calling thread.  The calling thread then displays a dialog that blocks the
   // calling thread from clearing the stack until the dialog is dismissed, which
   // occurs when the dialog receives the message.

   // Create our globals so we can store the file name.
   struct Globals *pG = InitGlobals(szZipFile);
   if (!pG) {
      pei->result = PK_MEM;
      SendMessage(g_hDlgProgress, WM_PRIVATE, MSG_OPERATION_COMPLETE, (LPARAM)pei);
      return FALSE;
   }

   // Store a global pointer to the Extract structure so it can be reached from
   // our thread and callback functions.
   g_pExtractInfo = pei;

   // Spawn our thread
   DWORD dwThreadId;
   HANDLE hThread;

#ifdef _WIN32_WCE

   // On CE, we use good old CreateThread() since the WinCE CRT does not
   // allocate per-thread storage.
   hThread = CreateThread(NULL, 0, ExtractOrTestFilesThread, pG, 0, &dwThreadId);

#else

   // On NT, we need use the CRT's thread function so that we don't leak any
   // CRT allocated memory when the thread exits.
   hThread = (HANDLE)_beginthreadex(NULL, 0, ExtractOrTestFilesThread, pG, 0,
                                    (unsigned*)&dwThreadId);

#endif

   // Bail out if our thread failed to create.
   if (!hThread) {

      DebugOut(TEXT("CreateThread() failed [%u]"), GetLastError());

      // Set our error as a memory error.
      g_pExtractInfo->result = PK_MEM;

      // Free our globals.
      FreeGlobals(pG);

      // Tell the progress dialog that we are done.
      SendMessage(g_hDlgProgress, WM_PRIVATE, MSG_OPERATION_COMPLETE, (LPARAM)pei);

      g_pExtractInfo = NULL;
      return FALSE;
   }

   // Close our thread handle since we have no use for it.
   CloseHandle(hThread);
   return TRUE;
}

//******************************************************************************
int DoGetComment(LPCSTR szFile) {

   int result;

   // Create our Globals struct and fill it in whith some default values.
   struct Globals *pG = InitGlobals(szFile);
   if (!pG) {
      return PK_MEM;
   }

   pG->zflag = TRUE; // display the zipfile comment

   // We wrap some exception handling around the entire Info-ZIP engine to be
   // safe.  Since we are running on a device with tight memory configurations,
   // all sorts of problems can arise when we run out of memory.
   __try {

      // Call the unzip routine.  We will catch the comment string in a callback
      // to win_fprintf().
      result = process_zipfiles(pG);

   } __except(EXCEPTION_EXECUTE_HANDLER) {

      // Catch any exception here.
      DebugOut(TEXT("Exception 0x%08X occurred in DoGetComment()"),
               GetExceptionCode());
      result = PK_EXCEPTION;
   }

   // Free our globals.
   FreeGlobals(pG);

   return result;
}

//******************************************************************************
BOOL SetExtractToDirectory(LPTSTR szDirectory) {

   BOOL fNeedToAddWack = FALSE;

   // Remove any trailing wack from the path.
   int length = _tcslen(szDirectory);
   if ((length > 0) && (szDirectory[length - 1] == TEXT('\\'))) {
      szDirectory[--length] = TEXT('\0');
      fNeedToAddWack = TRUE;
   }

#ifndef _WIN32_WCE

   // Check to see if a root directory was specified.
   if ((length == 2) && isalpha(szDirectory[0]) && (szDirectory[1] == ':')) {

      // If just a root is specified, we need to only verify the drive letter.
      if (!(GetLogicalDrives() & (1 << (tolower(szDirectory[0]) - (int)'a')))) {

         // This drive does not exist.  Bail out with a failure.
         return FALSE;
      }

   } else

#endif

   // We only verify path if length is >0 since we know "\" is valid.
   if (length > 0) {

      // Verify the the path exists and that it is a directory.
      if (IsFileOrDirectory(szDirectory) != 2) {
         return FALSE;
      }
   }

   // Store the directory for when we do an extract.
   wcstombs(g_szExtractToDirectory, szDirectory, countof(g_szExtractToDirectory));

   // We always want a wack at the end of our path.
   strcat(g_szExtractToDirectory, "\\");

   // Add the wack back to the end of the path.
   if (fNeedToAddWack) {
      _tcscat(szDirectory, TEXT("\\"));
   }

   return TRUE;
}

//******************************************************************************
//***** Internal functions
//******************************************************************************

struct Globals* InitGlobals(LPCSTR szZipFile) {

   // Store a global pointer to our USERFUNCTIONS structure so that LIST.C,
   // PROCESS.C, and WINMAIN can access it.
   lpUserFunctions = &g_uf;

   // Clear our USERFUNCTIONS structure and assign our SendAppMsg() function.
   ZeroMemory(&g_uf, sizeof(g_uf));
   g_uf.SendApplicationMessage = SendAppMsg;

   // Store a global pointer to our DCL structure so that EXTRACT.C can access it.
   lpDCL = &g_dcl;

   // Clear our DCL structure.
   ZeroMemory(&g_dcl, sizeof(g_dcl));

   // Create our global structure - pG
   CONSTRUCTGLOBALS();

   // Bail out if we failed to allocate our Globals structure.
   if (!pG) {
      return NULL;
   }

   // Fill in all our callback functions.
   pG->message     = UzpMessagePrnt2;
   pG->input       = UzpInput2;
   pG->mpause      = UzpMorePause;
   pG->replace     = UzpReplace;
   pG->sound       = UzpSound;

#if CRYPT
   pG->decr_passwd = UzpPassword;
#endif

   // Match filenames case-sensitively.  We can do this since we can guarentee
   // exact case because the user can only select files via our UI.
   pG->C_flag = FALSE;

   // Allocate and store the ZIP file name in pG->zipfn
   if (!(pG->zipfnPtr = new char[FILNAMSIZ])) {
      FreeGlobals(pG);
      return NULL;
   }
   pG->zipfn = pG->zipfnPtr;
   strcpy(pG->zipfn, szZipFile);

   // Allocate and store the ZIP file name in pG->zipfn.  This needs to done
   // so that do_wild() does not wind up clearing out the zip file name when
   // it returns in process.c
   if (!(pG->wildzipfnPtr = new char[FILNAMSIZ])) {
      FreeGlobals(pG);
      return NULL;
   }
   pG->wildzipfn = pG->wildzipfnPtr;
   strcpy(pG->wildzipfn, szZipFile);

   return pG;
}

//******************************************************************************
void FreeGlobals(Globals *pG) {

   // Free our ZIP file name
   if (pG->zipfnPtr) {
      delete[] pG->zipfnPtr;
      pG->zipfnPtr = pG->zipfn = NULL;
   }

   // Free our wild name buffer
   if (pG->wildzipfnPtr) {
      delete[] pG->wildzipfnPtr;
      pG->wildzipfnPtr = pG->wildzipfn = NULL;
   }

   // Free everything else.
   DESTROYGLOBALS()
}

//******************************************************************************
int IsFileOrDirectory(LPCTSTR szPath) {

   // Geth the attributes of the item.
   DWORD dwAttribs = GetFileAttributes(szPath);

   // Bail out now if we could not find the path at all.
   if (dwAttribs == 0xFFFFFFFF) {
      return 0;
   }

   // Return 1 for file and 2 for directory.
   return ((dwAttribs & FILE_ATTRIBUTE_DIRECTORY) ? 2 : 1);
}

//******************************************************************************
BOOL SmartCreateDirectory(struct Globals *pG, LPCSTR szDirectory) {

   // Copy path to a UNICODE buffer.
   TCHAR szBuffer[_MAX_PATH];
   mbstowcs(szBuffer, szDirectory, countof(szBuffer));

   int x = IsFileOrDirectory(szBuffer);

   // Create the directory if it does not exist.
   if (x == 0) {
      if (!CreateDirectory(szBuffer, NULL)) {
         Info(slide, 1, ((char *)slide, "error creating directory: %s\n", szDirectory));
         return FALSE;
      }

   // If there is a file with the same name, then display an error.
   } else if (x == 1) {
      Info(slide, 1, ((char *)slide,
           "cannot create %s as a file with same name already exists.\n",
           szDirectory));
      return FALSE;
   }

   // If the directory already exists or was created, then return success.
   return TRUE;
}

//******************************************************************************
#ifdef _WIN32_WCE

// On WinCE, we declare our thread function the way CreateThread() likes it.
DWORD WINAPI ExtractOrTestFilesThread(LPVOID lpv) {

#else

// On WinNT, we declare our thread function the way _beginthreadex likes it.
unsigned __stdcall ExtractOrTestFilesThread(void *lpv) {

#endif

   struct Globals *pG = (struct Globals*)lpv;

   if (g_pExtractInfo->fExtract) {

      pG->extract_flag = TRUE;

      switch (g_pExtractInfo->overwriteMode) {

         case OM_NEWER:
            pG->uflag = TRUE; // Update (extract only newer/brand-new files)
            break;

         case OM_ALWAYS:
            pG->overwrite_all = TRUE; // OK to overwrite files without prompting
            break;

         case OM_NEVER:
            pG->overwrite_none = TRUE; // Never overwrite files (no prompting)
            break;

         default:
            g_dcl.PromptToOverwrite = TRUE; // Force a prompt
            break;
      }

      // Throw away paths if requested.
      pG->jflag = !g_pExtractInfo->fRestorePaths;

   } else {
      pG->tflag = TRUE;
   }

   if (g_pExtractInfo->szFileList) {
      pG->filespecs = g_pExtractInfo->dwFileCount;
      pG->pfnames = g_pExtractInfo->szFileList;
   } else {
      // Improves performance if all files are being extracted.
      pG->process_all_files = TRUE;
   }

   // All args are files/dirs to be extracted.
   pG->dflag = TRUE;

   // Invalidate our file offset to show that we are starting a new operation.
   g_pExtractInfo->dwFileOffset = 0xFFFFFFFF;

   // We wrap some exception handling around the entire Info-ZIP engine to be
   // safe.  Since we are running on a device with tight memory configurations,
   // all sorts of problems can arise when we run out of memory.
   __try {

      // Put a jump marker on our stack so the user can abort.
      int error = setjmp(dll_error_return);

      // If setjmp() returns 0, then we just set our jump marker and we can
      // continue with the operation.  If setjmp() returned something else,
      // then we reached this point because the operation was aborted and
      // set our instruction pointer back here.

      if (error > 0) {
         // We already called process_zipfiles() and were thrown back here.
         g_pExtractInfo->result = (error == 1) ? PK_BADERR : error;

      } else {
         // Entering Info-ZIP... close your eyes.
         g_pExtractInfo->result = process_zipfiles(pG);
      }

   } __except(EXCEPTION_EXECUTE_HANDLER) {

      // Catch any exception here.
      DebugOut(TEXT("Exception 0x%08X occurred in ExtractOrTestFilesThread()"),
               GetExceptionCode());
      g_pExtractInfo->result = PK_EXCEPTION;
   }

   // Free our globals.
   FreeGlobals(pG);

   // Tell the progress dialog that we are done.
   SendMessage(g_hDlgProgress, WM_PRIVATE, MSG_OPERATION_COMPLETE,
               (LPARAM)g_pExtractInfo);

   // Clear our global pointer as we are done with it.
   g_pExtractInfo = NULL;

#ifndef _WIN32_WCE
   // On NT, we need to free any CRT allocated memory.
   _endthreadex(0);
#endif

   return 0;
}

//******************************************************************************
void CheckForAbort(struct Globals *pG) {
   if (g_pExtractInfo->fAbort) {

      // Add a newline to our log if we are in the middle of a line of text.
      if (!g_pExtractInfo->fNewLineOfText) {
         SendMessage(g_hWndMain, WM_PRIVATE, MSG_ADD_TEXT_TO_EDIT, (LPARAM)"\n");
      }

      // Make sure whatever file we are currently processing gets closed.
      if (((int)pG->outfile != 0) && ((int)pG->outfile != -1)) {
         if (g_pExtractInfo->fExtract && *pG->filename) {

            // Make sure the user is aware that this file is screwed.
            SendMessage(g_hWndMain, WM_PRIVATE, MSG_ADD_TEXT_TO_EDIT,
                        (LPARAM)"warning: ");
            SendMessage(g_hWndMain, WM_PRIVATE, MSG_ADD_TEXT_TO_EDIT,
                        (LPARAM)pG->filename);
            SendMessage(g_hWndMain, WM_PRIVATE, MSG_ADD_TEXT_TO_EDIT,
                        (LPARAM)" is probably truncated.\n");
         }

         // Close the file.
         close_outfile(pG);
      }

      // Display an aborted message in the log
      SendMessage(g_hWndMain, WM_PRIVATE, MSG_ADD_TEXT_TO_EDIT,
                  (LPARAM)"Operation aborted by user.\n");

      // I hate to do this... Take a giant step out of here.
      longjmp(dll_error_return, PK_ABORTED);
   }
}

//******************************************************************************
void SetCurrentFile(struct Globals *pG) {

   // Reset all our counters as we about to process a new file.
   g_pExtractInfo->dwFileOffset = (DWORD)pG->pInfo->offset;
   g_pExtractInfo->dwFile++;
   g_pExtractInfo->dwBytesWrittenThisFile = 0;
   g_pExtractInfo->dwBytesWrittenPreviousFiles += g_pExtractInfo->dwBytesTotalThisFile;
   g_pExtractInfo->dwBytesTotalThisFile = pG->ucsize;
   g_pExtractInfo->szFile = pG->filename;
   g_pExtractInfo->fNewLineOfText = TRUE;

   // Pass control to our GUI thread to do a full update our progress dialog.
   SendMessage(g_hWndMain, WM_PRIVATE, MSG_UPDATE_PROGRESS_COMPLETE,
               (LPARAM)g_pExtractInfo);

   // Check our abort flag.
   CheckForAbort(pG);
}


//******************************************************************************
//***** Callbacks from Info-ZIP code.
//******************************************************************************

int UzpMessagePrnt2(zvoid *pG, uch *buffer, ulg size, int flag) {

   // Some ZIP files cause us to get called during DoListFiles(). We only handle
   // messages while processing DoExtractFiles().
   if (!g_pExtractInfo) {
      if (g_hWndEdit) {
         SendMessage(g_hWndMain, WM_PRIVATE, MSG_ADD_TEXT_TO_EDIT,
                     (LPARAM)buffer);
      } else {
         DebugOut(TEXT("Unhandled call to UzpMessagePrnt2(\"%S\")"), buffer);
      }
      return 0;
   }

   // When extracting, mapname() will get called for every file which in turn
   // will call SetCurrentFile().  For testing though, mapname() never gets
   // called so we need to be on the lookout for a new file.
   if (g_pExtractInfo->dwFileOffset != (DWORD)((struct Globals*)pG)->pInfo->offset) {
      SetCurrentFile((struct Globals*)pG);
   }

   // Make sure this message was inteded for us to display.
   if (!MSG_NO_WGUI(flag) && !MSG_NO_WDLL(flag)) {

      // Insert a leading newline if requested to do so.
      if (MSG_LNEWLN(flag) && !g_pExtractInfo->fNewLineOfText) {
         SendMessage(g_hWndMain, WM_PRIVATE, MSG_ADD_TEXT_TO_EDIT, (LPARAM)"\n");
         g_pExtractInfo->fNewLineOfText = TRUE;
      }

      // Since we use a proportional font, we need to do a little cleanup of the
      // text we are passed since it assumes a fixed font and adds padding to try
      // to line things up.  We remove leading whitespace on any new line of text.
      if (g_pExtractInfo->fNewLineOfText) {
         while (*buffer == ' ') {
            buffer++;
         }
      }

      // We always remove trailing whitespace.
      LPSTR psz = (LPSTR)buffer + strlen((LPSTR)buffer) - 1;
      while ((psz >= (LPSTR)buffer) && (*psz == ' ')) {
         *(psz--) = '\0';
      }

      // Determine if the next line of text will be a new line of text.
      g_pExtractInfo->fNewLineOfText = ((*psz == '\r') || (*psz == '\n'));

      // Change all forward slashes to back slashes in the buffer
      ForwardSlashesToBackSlashesA((LPSTR)buffer);

      // Add the cleaned-up text to our extraction log edit control.
      SendMessage(g_hWndMain, WM_PRIVATE, MSG_ADD_TEXT_TO_EDIT, (LPARAM)buffer);

      // Append a trailing newline if requested to do so.
      if (MSG_TNEWLN(flag) || MSG_MNEWLN(flag) && !g_pExtractInfo->fNewLineOfText) {
         SendMessage(g_hWndMain, WM_PRIVATE, MSG_ADD_TEXT_TO_EDIT, (LPARAM)"\n");
         g_pExtractInfo->fNewLineOfText = TRUE;
      }
   }

   return 0;
}

//******************************************************************************
int UzpInput2(zvoid *pG, uch *buffer, int *size, int flag) {
   DebugOut(TEXT("WARNING: UzpInput2(...) called"));
   return 0;
}

//******************************************************************************
void UzpMorePause(zvoid *pG, const char *szPrompt, int flag) {
   DebugOut(TEXT("WARNING: UzpMorePause(...) called"));
}

//******************************************************************************
int UzpPassword(zvoid *pG, int *pcRetry, char *szPassword, int nSize,
                const char *szZipFile, const char *szFile)
{
   // Return Values:
   //    IZ_PW_ENTERED    got some PWD string, use/try it
   //    IZ_PW_CANCEL     no password available (for this entry)
   //    IZ_PW_CANCELALL  no password, skip any further PWD request
   //    IZ_PW_ERROR      failure (no mem, no tty, ...)

#if CRYPT

   // Build the data structure for our dialog.
   DECRYPT_INFO di;
   di.retry      = *pcRetry;
   di.szPassword = szPassword;
   di.nSize      = nSize;
   di.szFile     = szFile;

   // Clear the password to be safe.
   *di.szPassword = '\0';

   // On our first call for a file, *pcRetry == 0.  If we would like to allow
   // for retries, then we set the value of *pcRetry to the number of retries we
   // are willing to allow.  We will be recalled as neccessary, each time with
   // *pcRetry being decremented once.  1 is the last retry we will get.
   *pcRetry = (*pcRetry == 0) ? MAX_PASSWORD_RETRIES : (*pcRetry - 1);

   // Pass control to our GUI thread which will prompt the user for a password.
   return SendMessage(g_hWndMain, WM_PRIVATE, MSG_PROMPT_FOR_PASSWORD, (LPARAM)&di);

#else
   return -2;
#endif
}

//******************************************************************************
int WINAPI UzpReplace(char *szFile) {
   // Pass control to our GUI thread which will prompt the user to overwrite.
   return SendMessage(g_hWndMain, WM_PRIVATE, MSG_PROMPT_TO_REPLACE, (LPARAM)szFile);
}

//******************************************************************************
void WINAPI UzpSound(void) {
   // Do nothing.
}

//******************************************************************************
// Called from LIST.C
void WINAPI SendAppMsg(ulg dwSize, ulg dwCompressedSize, int ratio, int month,
                       int day, int year, int hour, int minute, int uppercase,
                       char *szPath, char *szMethod, ulg dwCRC)
{
   // If we are out of memory, then just bail since we will only make things worse.
   if (g_fOutOfMemory) {
      return;
   }

   // We get our Globals structure and then retrieve the real file name.
   GETGLOBALS()
   szPath = pG->filename;

   // Allocate a FILE_NODE large enough to hold this file.
   int length = strlen(szPath) + strlen(szMethod);
   g_pFileLast = (FILE_NODE*)new BYTE[sizeof(FILE_NODE) + (sizeof(TCHAR) * length)];

   // Bail out if we failed to allocate the node.
   if (!g_pFileLast) {
      DebugOut(TEXT("Failed to create a FILE_NODE for \"%S\"."), szPath);
      g_fOutOfMemory = TRUE;
      return;
   }

   // Fill in our node.
   g_pFileLast->dwSize           = dwSize;
   g_pFileLast->dwCompressedSize = dwCompressedSize;
   g_pFileLast->dwCRC            = dwCRC;
   g_pFileLast->szComment        = NULL;
   g_pFileLast->szType           = NULL;

   // Fix the year value to contain the real year.
   year += 1900;

   // Year:   0 - 4095 (12) 1111 1111 1111 0000 0000 0000 0000 0000 (0xFFF00000)
   // Month:  1 -   12 ( 4) 0000 0000 0000 1111 0000 0000 0000 0000 (0x000F0000)
   // Day:    1 -   31 ( 5) 0000 0000 0000 0000 1111 1000 0000 0000 (0x0000F800)
   // Hour:   0 -   23 ( 5) 0000 0000 0000 0000 0000 0111 1100 0000 (0x000007C0)
   // Minute: 0 -   59 ( 6) 0000 0000 0000 0000 0000 0000 0011 1111 (0x0000003F)

   // Do some bit shifting to make the date and time fit in a DWORD.
   g_pFileLast->dwModified = (((DWORD)(year   & 0x0FFF) << 20) |
                              ((DWORD)(month  & 0x000F) << 16) |
                              ((DWORD)(day    & 0x001F) << 11) |
                              ((DWORD)(hour   & 0x001F) <<  6) |
                              ((DWORD)(minute & 0x003F)));

   // We need to get our globals structure to determine our attributes and
   // encryption information.
   g_pFileLast->dwAttributes = (pG->crec.external_file_attributes & 0xFF);
   if (pG->crec.general_purpose_bit_flag & 1) {
      g_pFileLast->dwAttributes |= FILE_ATTRIBUTE_ENCRYPTED;
   }

   // Store the path and method in our string buffer.
   strcpy(g_pFileLast->szPathAndMethod, szPath);
   strcpy(g_pFileLast->szPathAndMethod + strlen(szPath) + 1, szMethod);

   // Pass the file object to our windows code to have it added to our list.
   AddFileToListView(g_pFileLast);
}

//******************************************************************************
int win_fprintf(FILE *file, unsigned int dwCount, char far *buffer) {

   // win_fprintf() is used within Info-ZIP to write to a file as well as log
   // information.  If the "file" is a real file handle (not stdout or stderr),
   // then we write the data to the file and return.

   if ((file != stdout) && (file != stderr)) {

      DWORD dwBytesWriten = 0;
#ifdef _WIN32_WCE
      // On WinCE all FILEs are really HANDLEs.  See WINCE.CPP for more info.
      WriteFile((HANDLE)file, buffer, dwCount, &dwBytesWriten, NULL);
#else
      dwBytesWriten = fwrite(buffer, 1, dwCount, file);
#endif

      // Update our bytes written count.
      g_pExtractInfo->dwBytesWrittenThisFile += dwBytesWriten;

      // Pass control to our GUI thread to do a partial update our progress dialog.
      SendMessage(g_hWndMain, WM_PRIVATE, MSG_UPDATE_PROGRESS_PARTIAL,
                  (LPARAM)g_pExtractInfo);

      // Check our abort flag.
      GETGLOBALS();
      CheckForAbort(pG);

      return dwBytesWriten;
   }

   // Check to see if we are expecting a extraction progress string
   if (g_pExtractInfo) {

      // Most of our progress strings come to our UzpMessagePrnt2() callback,
      // but we occasionally get one here.  We will just forward it to
      // UzpMessagePrnt2() as if it never came here.  To do this, we need to
      // get a pointer to our Globals struct.  Calling GETGLOBALS() sort of
      // breaks us from be REENTRANT, but we don't support that anyway.
      GETGLOBALS();
      UzpMessagePrnt2(pG, (uch*)buffer, dwCount, 0);
      return dwCount;
   }

   // Check to see if we are expecting a zip file comment string.
   if (g_hWndEdit) {

      // Change all forward slashes to back slashes in the buffer
      ForwardSlashesToBackSlashesA((LPSTR)buffer);

      SendMessage(g_hWndMain, WM_PRIVATE, MSG_ADD_TEXT_TO_EDIT, (LPARAM)buffer);
      return dwCount;
   }

   // Check to see if we are expecting a compressed file comment string.
   if (g_pFileLast) {

      // Calcalute the size of the buffer we will need to store this comment.
      // We are going to convert all ASC values 0 - 31 (excpet tab, new line,
      // and CR) to ^char.
      int size = 1;
      for (char *p2, *p1 = buffer; *p1; p1++) {
         size += ((*p1 >= 32) || (*p1 == '\t') || (*p1 == '\r') || (*p1 == '\n')) ? 1 : 2;
      }

      // Allocate a comment buffer and assign it to the last file node we saw.
      if (g_pFileLast->szComment = new CHAR[size]) {

         // Copy while formatting.
         for (p1 = buffer, p2 = (char*)g_pFileLast->szComment; *p1; p1++) {
            if ((*p1 >= 32) || (*p1 == '\t') || (*p1 == '\r') || (*p1 == '\n')) {
               *(p2++) = *p1;
            } else {
               *(p2++) = '^';
               *(p2++) = 64 + *p1;
            }
         }
         *p2 = '\0';
      }

      // Update the attributes of the file node to incldue the comment attribute.
      g_pFileLast->dwAttributes |= FILE_ATTRIBUTE_COMMENT;

      // Clear the file node so we don't try to add another bogus comment to it.
      g_pFileLast = NULL;

      return dwCount;
   }

   if (dwCount >= _MAX_PATH) {
      buffer[_MAX_PATH] = '\0';
   }
   DebugOut(TEXT("Unhandled call to win_fprintf(\"%S\")"), buffer);
   return dwCount;
}


//******************************************************************************
//***** Functions that Info-ZIP expects the port to write and export.
//***** Some of this code was stolen from the WIN32 port and highly modified.
//******************************************************************************

int mapattr(struct Globals *pG) {

   // Check to see if we are extracting this file for viewing.  Currently, we do
   // this by checking the szMappedPath member of our extract info stucture
   // since we know OnActionView() is the only one who sets this member.

   if (g_pExtractInfo && g_pExtractInfo->szMappedPath) {

      // If we are extracting for view only, then we ignore the file's real
      // attributes and force the file to create as read-only.  We make the file
      // read-only to help prevent the user from making changes to the temporary
      // file and then trying to save the changes back to a file that we will
      // eventually delete.
      pG->pInfo->file_attr = FILE_ATTRIBUTE_READONLY;

   } else {

      // Store the attribute exactly as it appears for normal extraction/test.
      pG->pInfo->file_attr = (unsigned)pG->crec.external_file_attributes & 0xff;
   }
   return PK_OK;
}

//******************************************************************************
void utimeToFileTime(time_t ut, FILETIME *pft, BOOL fOldFileSystem) {

   // time_t    is a 32-bit value for the seconds since January 1, 1970
   // FILETIME  is a 64-bit value for the number of 100-nanosecond intervals since
   //           January 1, 1601
   // DWORDLONG is a 64-bit int that we can use to perform large math operations.


   // time_t has minimum of 1/1/1970.  Many file systems, such as FAT, have a
   // minimum date of 1/1/1980.  If extracting to one of those file systems and
   // out time_t is less than 1980, then we make it 1/1/1980.
   // (365 days/yr * 10 yrs + 3 leap yr days) * (60 secs * 60 mins * 24 hrs).
   if (fOldFileSystem && (ut < 0x12CFF780)) {
      ut = 0x12CFF780;
   }

   // Compute the FILETIME for the given time_t.
   DWORDLONG dwl = ((DWORDLONG)116444736000000000 +
                   ((DWORDLONG)ut * (DWORDLONG)10000000));

   // Store the return value.
   *pft = *(FILETIME*)&dwl;

   // Now for the next fix for old file systems.  If we are in Daylight Savings
   // Time (DST) and the file is not in DST, then we need subtract off the DST
   // bias from the filetime.  This is due to a bug in Windows (NT, CE, and 95)
   // that causes the DST bias to be added to all file times when the system
   // is in DST, even if the file is not in DST.  This only effects old file
   // systems since they store local times instead of UTC times.  Newer file
   // systems like NTFS and CEFS store UTC times.

   if (fOldFileSystem) {

      // We use the CRT's localtime() and Win32's FileTimeToLocalTime()
      // functions to compute the DST bias.  This works because localtime()
      // correctly adds the DST bias only if the file time is in DST.
      // FileTimeToLocalTime() always adds the DST bias to the time.
      // Therefore, if the functions return different results, we know we
      // are dealing with a non-DST file during a system DST.

      FILETIME ftCRT, ftWin32;

      // Get the CRT result - result is a "tm" struct.
      struct tm *ptmCRT = localtime(&ut);

      // Convert the "tm" struct to a FILETIME.
      SYSTEMTIME stCRT;
      ZeroMemory(&stCRT, sizeof(stCRT));
      stCRT.wYear   = ptmCRT->tm_year + 1900;
      stCRT.wMonth  = ptmCRT->tm_mon + 1;
      stCRT.wDay    = ptmCRT->tm_mday;
      stCRT.wHour   = ptmCRT->tm_hour;
      stCRT.wMinute = ptmCRT->tm_min;
      stCRT.wSecond = ptmCRT->tm_sec;
      SystemTimeToFileTime(&stCRT, &ftCRT);

      // Get the Win32 result - result is a FILETIME.
      if (FileTimeToLocalFileTime(pft, &ftWin32)) {

         // Subtract the difference from our current filetime.
         *(DWORDLONG*)pft -= *(DWORDLONG*)&ftWin32 - *(DWORDLONG*)&ftCRT;
      }
   }
}

//******************************************************************************
int GetFileTimes(struct Globals *pG, FILETIME *pftCreated, FILETIME *pftAccessed,
                 FILETIME *pftModified)
{
   // We need to check to see if this file system is limited.  This includes
   // FAT, VFAT, and HPFS.  It does not include NTFS and CEFS.  The limited
   // file systems can not support dates < 1980 and they store file local times
   // for files as opposed to UTC times.
   BOOL fOldFileSystem = IsOldFileSystem(pG->filename);

#ifdef USE_EF_UT_TIME  // Always true for WinCE build

   if (pG->extra_field) {

      // Structure for Unix style actime, modtime, creatime
      iztimes z_utime;

      // Get any date/time we can.  This can return 0 to 3 unix time fields.
      unsigned eb_izux_flg = ef_scan_for_izux(pG->extra_field,
                                              pG->lrec.extra_field_length, 0,
                                              pG->lrec.last_mod_file_date,
                                              &z_utime, NULL);

      // We require at least a modified time.
      if (eb_izux_flg & EB_UT_FL_MTIME) {

         // We know we have a modified time, so get it first.
         utimeToFileTime(z_utime.mtime, pftModified, fOldFileSystem);

         // Get the accessed time if we have one.
         if (eb_izux_flg & EB_UT_FL_ATIME) {
            utimeToFileTime(z_utime.atime, pftAccessed, fOldFileSystem);
         }

         // Get the created time if we have one.
         if (eb_izux_flg & EB_UT_FL_CTIME) {
            utimeToFileTime(z_utime.ctime, pftCreated, fOldFileSystem);
         }

         // Return our flags.
         return (int)eb_izux_flg;
      }
   }

#endif // USE_EF_UT_TIME

   // If all else fails, we can resort to using the DOS date and time data.
   time_t ux_modtime = dos_to_unix_time(G.lrec.last_mod_file_date,
                                        G.lrec.last_mod_file_time);
   utimeToFileTime(ux_modtime, pftModified, fOldFileSystem);

   *pftAccessed = *pftModified;

   return (EB_UT_FL_MTIME | EB_UT_FL_ATIME);
}

//******************************************************************************
void close_outfile(struct Globals *pG) {

   // Get the 3 time stamps for the file.
   FILETIME ftCreated, ftAccessed, ftModified;
   int timeFlags = GetFileTimes(pG, &ftCreated, &ftAccessed, &ftModified);

   TCHAR szFile[_MAX_PATH];
   mbstowcs(szFile, pG->filename, countof(szFile));

#ifdef _WIN32_WCE

   // Cast the outfile to a HANDLE (since that is really what it is), and
   // flush the file.  We need to flush, because any unsaved data that is
   // written to the file during CloseHandle() will step on the work done
   // by SetFileTime().
   HANDLE hFile = (HANDLE)pG->outfile;
   FlushFileBuffers(hFile);

#else

   // Close the file and then re-open it using the Win32 CreateFile() call.
   // SetFileTime() requires a Win32 file HANDLE created with GENERIC_WRITE
   // access.
   fclose(pG->outfile);
   HANDLE hFile = CreateFile(szFile, GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
                             OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

#endif

   // Set the file's date and time.
   if (hFile != INVALID_HANDLE_VALUE) {

      // Make sure we retrieved some valid time stamp(s)
      if (timeFlags) {

         // Set the various date and time fields.
         if (!SetFileTime(hFile,
                 (timeFlags & EB_UT_FL_CTIME) ? &ftCreated  : NULL,
                 (timeFlags & EB_UT_FL_ATIME) ? &ftAccessed : NULL,
                 (timeFlags & EB_UT_FL_MTIME) ? &ftModified : NULL))
         {
            DebugOut(TEXT("SetFileTime() failed [%u]"), GetLastError());
         }

      } else {
         DebugOut(TEXT("GetFileTimes() failed"));
      }

      // Close out file.
      CloseHandle(hFile);

   } else {
      DebugOut(TEXT("CreateFile() failed [%u]"), GetLastError());
   }

   // If the file was successfully written, then set the attributes.
   if (!pG->disk_full && !g_pExtractInfo->fAbort) {
      if (!SetFileAttributes(szFile, G.pInfo->file_attr & 0x7F)) {
         DebugOut(TEXT("SetFileAttributes() failed [%u]"), GetLastError());
      }
   }

   // Clear outfile so we know it is closed.
   pG->outfile = 0;

   return;
}

//******************************************************************************
// Called by PROCESS.C
char* do_wild(struct Globals *pG, char *wildspec) {

   // This is a very slimmed down version of do_wild() taken from WIN32.C.
   // Since we don't support wildcards, we basically just return the wildspec
   // passed in as the filename.

   // First call - must initialize everything.
   if (!pG->notfirstcall) {
      pG->notfirstcall = TRUE;
      return strcpy(pG->matchname, wildspec);
   }

   // Last time through - reset for new wildspec.
   pG->notfirstcall = FALSE;

   return (char*)NULL;
}

//******************************************************************************
// Called from EXTRACT.C
//
// returns:  1 - (on APPEND_NAME) truncated filename
//           2 - path doesn't exist, not allowed to create
//           3 - path doesn't exist, tried to create and failed; or
//               path exists and is not a directory, but is supposed to be
//           4 - path is too long
//          10 - can't allocate memory for filename buffers
//
// IZ_VOL_LABEL   - Path was a volume label, skip it.
// IZ_CREATED_DIR - Created a directory.
//
int mapname(struct Globals *pG, int renamed) {

   // mapname() is a great place to reset all our status counters for the next
   // file to be processed since it is called for every zip file member before
   // any work is done with that member.
   SetCurrentFile(pG);

   // If Volume Label, skip the "extraction" quietly
   if (pG->pInfo->vollabel) {
      return IZ_VOL_LABEL;
   }

   CHAR szBuffer[countof(pG->filename)] = "", *pIn, *pOut, *pLastSemi = NULL;

   // Initialize file path buffer with our "extract to" path.
   strcpy(szBuffer, g_szExtractToDirectory);
   pOut = szBuffer + strlen(szBuffer);

   // Point pIn to beginning of our internal pathname.
   // If we are junking paths, then locate the file portion of the path.
   pIn = (pG->jflag) ? (CHAR*)GetFileFromPath(pG->filename) : pG->filename;

   // Begin main loop through characters in filename.
   for ( ; *pIn; pIn++) {

      // Make sure we don't overflow our output buffer.
      if (pOut >= (szBuffer + countof(szBuffer) - 2)) {
         Info(slide, 1, ((char*)slide, "path too long: %s\n", pG->filename));
         return 4;
      }

      // Examine the next character in our input buffer.
      switch (*pIn) {

         // Check for a directory wack.
         case '/':
         case '\\':
            *pOut = '\0';
            if (!SmartCreateDirectory(pG, szBuffer)) {
               Info(slide, 1, ((char*)slide, "failure extracting: %s\n",
                    pG->filename));
               return 3;
            }
            *(pOut++) = '\\';
            pLastSemi = NULL;  // Leave any directory semi-colons alone
            break;

         // Check for illegal characters and replace with underscore.
         case ':':
         case '*':
         case '?':
         case '"':
         case '<':
         case '>':
         case '|':
            *(pOut++) = '_';
            break;

         // Check for start of VMS version.
         case ';':
            pLastSemi = pOut;  // Make note as to where we are.
            *(pOut++) = *pIn;  // Leave the semi-colon alone for now.
            break;

         default:
            // Allow European characters and spaces in filenames.
            *(pOut++) = ((*pIn >= 0x20) ? *pIn : '_');
      }
   }

   // Done with output buffer, terminate it.
   *pOut = '\0';

   // Remove any VMS version numbers if found (appended ";###").
   if (pLastSemi) {

      // Walk over all digits following the semi-colon.
      for (pOut = pLastSemi + 1; (*pOut >= '0') && (*pOut <= '9'); pOut++) {
      }

      // If we reached the end, then nuke the semi-colon and digits.
      if (!*pOut) {
         *pLastSemi = '\0';
      }
   }

   // Copy the mapped name back to the internal path buffer
   strcpy(pG->filename, szBuffer);

   // Fill in the mapped name buffer if the original caller requested us to.
   if (g_pExtractInfo->szMappedPath) {
      strcpy(g_pExtractInfo->szMappedPath, szBuffer);
   }

   // If it is a directory, then display the "creating" status text.
   if ((pOut > szBuffer) && (pOut[-1] == TEXT('\\'))) {
      Info(slide, 0, ((char *)slide, "creating: %s\n", pG->filename));
      return IZ_CREATED_DIR;
   }

   return PK_OK;
}

//******************************************************************************
// Called from EXTRACT.C
int test_NT(struct Globals *pG, uch *eb, unsigned eb_size) {
   // This function is called when an NT security descriptor is found in the
   // extra field.  We have nothing to do, so we just return success.
   return PK_OK;
}

//******************************************************************************
// Called from PROCESS.C
int checkdir(struct Globals *pG, char *pathcomp, int flag) {
   // This function is only called by free_G_buffers() from PROCESS.C with the
   // flag set to END.  We have nothing to do, so we just return success.
   return PK_OK;
}

//******************************************************************************
// Called from EXTRACT.C and LIST.C
int match(char *string, char *pattern, int ignore_case) {
   // match() for the other ports compares a file in the Zip file with some
   // command line file pattern.  In our case, we always pass in exact matches,
   // so we can simply do a string compare to see if we have a match.
   return (strcmp(string, pattern) == 0);
}

//******************************************************************************
// Called from PROCESS.C
int iswild(char *pattern) {
   // Our file patterns never contain wild characters.  They are always exact
   // matches of file names in our Zip file.
   return FALSE;
}

//******************************************************************************
//***** Functions to correct time stamp bugs on old file systems.
//******************************************************************************

//******************************************************************************
// Borrowed/Modified from win32.c
BOOL IsOldFileSystem(char *szPath) {

#ifdef _WIN32_WCE

   char szRoot[10];

   // Get the first nine characters of the path.
   strncpy(szRoot, szPath, 9);
   szRoot[9] = '\0';

   // Convert to uppercase to help with compare.
   _strupr(szRoot);

   // PC Cards are mounted off the root in a directory called "\PC Cards".
   // PC Cards are FAT, no CEOS.  We need to check if the file is being
   // extracted to the PC card.
   return !strcmp(szRoot, "\\PC CARD\\");

#else

   char szRoot[_MAX_PATH] = "\0\0\0", szFS[64];

   // Check to see if our path contains a drive letter.
   if (isalpha(szPath[0]) && (szPath[1] == ':') && (szPath[2] == '\\')) {

      // If so, then just copy the drive letter, colon, and wack to our root path.
      strncpy(szRoot, szPath, 3);

   } else {

      // Expand the path so we can get a drive letter.
      GetFullPathNameA(szPath, sizeof(szRoot), szRoot, NULL);

      // Make sure we actually got a drive letter back in our root path buffer..
      if (!isalpha(szRoot[0]) || (szRoot[1] != ':') || (szRoot[2] != '\\')) {

         // When in doubt, return TRUE.
         return TRUE;
      }
   }

   // NULL terminate after the wack to ensure we have just the root path.
   szRoot[3] = '\0';

   // Get the file system type string.
   GetVolumeInformationA(szRoot, NULL, 0, NULL, NULL, NULL, szFS, sizeof(szFS));

   // Ensure that the file system type string is uppercase.
   strupr(szFS);

   // Return true for (V)FAT and (OS/2) HPFS format.
   return !strncmp(szFS, "FAT",  3) ||
          !strncmp(szFS, "VFAT", 4) ||
          !strncmp(szFS, "HPFS", 4);

#endif // _WIN32_WCE
}
