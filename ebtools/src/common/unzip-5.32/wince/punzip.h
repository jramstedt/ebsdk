//******************************************************************************
//
// File:        PUNZIP.H
//
// Description: This is our global header for the entire Pocket UnZip project.
//              This header contains all global project build flags, defines,
//              constants, and macros.  It also includes all other headers that
//              are needed by the project.
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
//
// Date      Name          History
// --------  ------------  -----------------------------------------------------
// 02/01/97  Steve Miller  Created (Version 1.0 using Info-ZIP UnZip 5.30)
//
//******************************************************************************

#ifndef __PUNZIP_H__
#define __PUNZIP_H__

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
//***** Standard Win32 project flags
//******************************************************************************

#ifndef WIN32
#define WIN32
#endif

#ifndef _WINDOWS
#define _WINDOWS
#endif

#ifndef UNICODE
#define UNICODE
#endif

#ifndef _UNICODE
#define _UNICODE
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef STRICT
#define STRICT
#endif

#if defined(_DEBUG) && !defined(DEBUG)
#define DEBUG
#endif

#if defined(DEBUG) && !defined(_DEBUG)
#define _DEBUG
#endif

#if defined(_NDEBUG) && !defined(NDEBUG)
#define NDEBUG
#endif

#if defined(NDEBUG) && !defined(_NDEBUG)
#define _NDEBUG
#endif


//******************************************************************************
//***** Pocket Unzip and Info-ZIP flags
//******************************************************************************

#ifndef POCKET_UNZIP
#define POCKET_UNZIP
#endif

#ifndef UNZIP_INTERNAL
#define UNZIP_INTERNAL
#endif

#ifndef WINDLL
#define WINDLL
#endif

#ifndef DLL
#define DLL
#endif

#ifndef REENTRANT
#define REENTRANT
#endif

#ifndef USE_EF_UT_TIME
#define USE_EF_UT_TIME
#endif

#ifndef NO_ZIPINFO
#define NO_ZIPINFO
#endif

#ifndef NO_STDDEF_H
#define NO_STDDEF_H
#endif

#ifndef NO_NTSD_EAS
#define NO_NTSD_EAS
#endif

#ifdef NTSD_EAS
#undef NTSD_EAS
#endif

// Read COPYING document before enabling this define.
#if 0
#ifndef USE_SMITH_CODE
#define USE_SMITH_CODE
#endif
#endif

// Read COPYING document before enabling this define.
#if 0
#ifndef USE_UNSHRINK
#define USE_UNSHRINK
#endif
#endif


//******************************************************************************
//***** Global defines, constants, and macros
//******************************************************************************

#ifndef PATH_MAX
#define PATH_MAX _MAX_PATH
#endif

// WinCE uses ISO 8859-1 as codepage for 8-bit chars
#define CRTL_CP_IS_ISO
// The functionality of ISO <--> OEM conversion IS needed on WinCE, too!!
// (Otherwise, extended ASCII filenames and passwords in archives coming
// from other platforms may not be handled properly.)
// Since WinCE does not supply ISO <--> OEM conversion, we try to fall
// back to the hardcoded tables in the generic UnZip code.

#define ASCII2OEM(c) (((c) & 0x80) ? iso2oem[(c) & 0x7f] : (c))
#if !defined(CRYP_USES_ISO2OEM)
#  define CRYP_USES_ISO2OEM
#endif

#define INTERN_TO_ISO(src, dst)  {if ((src) != (dst)) strcpy((dst), (src));}
#define INTERN_TO_OEM(src, dst)  {register uch c;\
    register uch *dstp = (uch *)(dst), *srcp = (uch *)(src);\
    do {\
        c = (uch)foreign(*srcp++);\
        *dstp++ = (uch)ASCII2OEM(c);\
    } while (c != '\0');}


#if (defined(_MSC_VER) && !defined(MSC))
#define MSC
#endif

#ifdef MSC
#define DIR_END   '\\' // ZipInfo with VC++ 4.0 requires this
#endif

// We are defining a few new error types for our code.
#define PK_EXCEPTION  500
#define PK_ABORTED    501

#ifndef DATE_FORMAT
#define DATE_FORMAT   DF_MDY
#endif
#define lenEOL        2
#define PutNativeEOL  {*q++ = native(CR); *q++ = native(LF);}

#define countof(a) (sizeof(a)/sizeof(*(a)))

// The max number of retires (not including the first attempt) for entering
// a password for and encrypted file before giving up on that file.
#define MAX_PASSWORD_RETRIES 2

// Variables that we want to add to our Globals struct.
#define SYSTEM_SPECIFIC_GLOBALS  \
   char  matchname[FILNAMSIZ];   \
   int   notfirstcall;           \
   char *zipfnPtr;               \
   char *wildzipfnPtr;


//******************************************************************************
//***** Global headers
//******************************************************************************

#include <windows.h>
#include <setjmp.h>
#include <stdlib.h>
#include <excpt.h>
#include "wince\wince.h"     // Our WinCE specific code and our debug function.
#include "wince\resource.h"  // Our resource constants
#include "wince\punzip.rcv"  // Our version information.

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __PUNZIP_H__
