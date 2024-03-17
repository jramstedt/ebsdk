/*
 Example header file

 Do not use this header file in the WiZ application, use WIZ.H
 instead.

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

typedef int (WINAPI * _DLL_UNZIP)(int, char **, int, char **,
                                  LPDCL, LPUSERFUNCTIONS);
typedef int (WINAPI * _USER_FUNCTIONS)(LPUSERFUNCTIONS);

/* Global variables */

extern LPUSERFUNCTIONS lpUserFunctions;
extern LPDCL lpDCL;

extern HINSTANCE hUnzipDll;

extern int hFile;                 /* file handle             */

/* Global functions */

extern _DLL_UNZIP windll_unzip;
extern _USER_FUNCTIONS UzInit;
int WINAPI DisplayBuf(char far *, unsigned long int);

/* Procedure Calls */
void WINAPI ReceiveDllMessage(unsigned long,unsigned long,
   ush, ush, ush, ush, ush, ush, char, char *, char *, unsigned long, char);
#endif /* _EXAMPLE_H */

