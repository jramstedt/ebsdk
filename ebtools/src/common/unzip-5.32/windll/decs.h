#ifndef __unzlib_h   /* prevent multiple inclusions */
#define __unzlib_h

#include <windows.h>
#include "structs.h"

#define UzpMatch match

BOOL    WINAPI Unz_Init(zvoid *, USERFUNCTIONS far *);
BOOL    WINAPI Unz_SetOpts(zvoid *, LPDCL);
int     WINAPI Unz_Unzip(zvoid *, int, char **, int, char **);
int     WINAPI windll_unzip(int, char **, int, char **,
                            DCL far *, USERFUNCTIONS far *);

#endif /* __unzlib_h */
