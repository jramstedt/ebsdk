#ifndef _ZIP_STRUCTS_H
#define _ZIP_STRUCTS_H

#ifndef Far
#  define Far far
#endif

/* Porting definations between Win 3.1x and Win32 */
#ifdef WIN32
#  define far
#  define _far
#  define __far
#  define near
#  define _near
#  define __near
#endif

#ifndef PATH_MAX
# define PATH_MAX 128
#endif

#include "api.h"

#endif /* _ZIP_STRUCTS_H */
