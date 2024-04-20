#ifndef _GCT_WRITE_H_
#define _GCT_WRITE_H_

/*
 *  Need to define GCT_FILE
 *
 */
#include "gct_io.h"

extern int32 gct_write$hw_config_source(GCT_FILE *stream, char *name);
extern int32 gct_write$hw_config_data(GCT_FILE *stream, char *name);

#endif /* _GCT_WRITE_H_ */
