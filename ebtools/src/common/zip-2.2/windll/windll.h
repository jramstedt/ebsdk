/*
 WiZ 1.0 header file for zip dll
*/
#ifndef _WINDLL_H
#define _WINDLL_H

#include "structs.h"

#ifndef MSWIN
#define MSWIN
#endif

#ifndef USE_ZIPMAIN
#   define USE_ZIPMAIN
#endif

#ifndef NDEBUG
#  define WinAssert(exp) \
        {\
        if (!(exp))\
            {\
            char szBuffer[40];\
            sprintf(szBuffer, "File %s, Line %d",\
                    __FILE__, __LINE__) ;\
            if (IDABORT == MessageBox((HWND)NULL, szBuffer,\
                "Assertion Error",\
                MB_ABORTRETRYIGNORE|MB_ICONSTOP))\
                    FatalExit(-1);\
            }\
        }

#else
#  define WinAssert(exp)
#endif

#define cchFilesMax 4096

extern int WINAPI ZpArchive(ZCL C);
extern DLLPRNT *lpZipPrint;
extern HWND hGetFilesDlg;
extern char szFilesToAdd[80];
extern char rgszFiles[cchFilesMax];
BOOL WINAPI CommentBoxProc(HWND hwndDlg, WORD wMessage, WPARAM wParam, LPARAM lParam);
BOOL PasswordProc(HWND, WORD, WPARAM, LPARAM);
void CenterDialog(HWND hwndParent, HWND hwndDlg);
void comment(unsigned int);

extern LPSTR szCommentBuf;
extern HANDLE hStr;
extern HWND hWndMain;
extern HWND hInst;
void __far __cdecl perror(const char *);

#endif /* _WINDLL_H */

