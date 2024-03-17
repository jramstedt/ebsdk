#ifndef _STRUCTS_H
#define _STRUCTS_H

#ifndef Far
#  define Far far
#endif

/* Porting definitions between Win 3.1x and Win32 */
#ifdef WIN32
#  define far
#  define _far
#  define __far
#  define near
#  define _near
#  define __near
#  ifndef FAR
#    define FAR
#  endif
#endif

#ifndef PATH_MAX
#  define PATH_MAX 128            /* max total file or directory name path */
#endif

#ifndef DEFINED_ONCE
#define DEFINED_ONCE
#ifndef ush
typedef unsigned short  ush;
#endif
typedef int (WINAPI DLLPRNT) (LPSTR, unsigned long);
typedef int (WINAPI DLLPASSWORD) (LPSTR, int, const LPSTR, const LPSTR);
#endif
typedef void (WINAPI DLLSND) (void);
typedef int (WINAPI DLLREPLACE)(LPSTR);
typedef void (WINAPI DLLMESSAGE)(unsigned long,unsigned long,
   ush, ush, ush, ush, ush, ush, char, LPSTR, LPSTR, unsigned long, char);

typedef struct {
DLLPRNT *print;
DLLSND *sound;
DLLREPLACE *replace;
DLLPASSWORD *password;
DLLMESSAGE *SendApplicationMessage;
WORD cchComment;
unsigned long TotalSizeComp;
unsigned long TotalSize;
int CompFactor;
unsigned int NumMembers;
} USERFUNCTIONS, far * LPUSERFUNCTIONS;

typedef struct {
int ExtractOnlyNewer;
int SpaceToUnderscore;
int PromptToOverwrite;
int fQuiet;
int ncflag;
int ntflag;
int nvflag;
int nUflag;
int nzflag;
int ndflag;
int noflag;
int naflag;
int nZIflag;
int C_flag;
int fPrivilege;
LPSTR lpszZipFN;
LPSTR lpszExtractDir;
} DCL, _far *LPDCL;

#endif /* _STRUCTS_H */
