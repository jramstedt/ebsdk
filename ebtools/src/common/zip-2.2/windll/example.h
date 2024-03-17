/*
 Example header file
*/
#ifndef _EXAMPLE_H
#define _EXAMPLE_H

#include <windows.h>
#include <assert.h>    /* required for all Windows applications */
#include <stdlib.h>
#include <stdio.h>
#include <commdlg.h>
#include <dlgs.h>
#include <windowsx.h>

#include "structs.h"

/* Defines */
#ifndef MSWIN
#define MSWIN
#endif

typedef int (WINAPI * _DLL_ZIP)(LPZCL);
typedef int (WINAPI * _ZIP_USER_FUNCTIONS)(LPZIPUSERFUNCTIONS);
typedef BOOL (WINAPI * ZIPSETOPTIONS)(LPZPOPT);

/* Global variables */

extern LPZIPUSERFUNCTIONS lpZipUserFunctions;

extern HINSTANCE hZipDll;

extern int hFile;                 /* file handle             */

/* Global functions */

extern _DLL_ZIP ZpArchive;
extern _ZIP_USER_FUNCTIONS ZpInit;
int WINAPI DisplayBuf(char far *, unsigned long int);
extern ZIPSETOPTIONS ZpSetOptions;

#endif /* _EXAMPLE_H */

