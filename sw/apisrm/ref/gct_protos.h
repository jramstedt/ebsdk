#ifndef _GCT_PROTOS_H_
#define _GCT_PROTOS_H_

#include "gct.h"

/*
 * Some handy macros
 */
#define _GCT_MAKE_ADDRESS(_gct_o) ((GCT_NODE *)((char *) GCT_POINTER_TO_ROOT + _gct_o));
#define _GCT_MAKE_HANDLE(_gct_a) ((char *) _gct_a - (char *) GCT_POINTER_TO_ROOT);

#endif
