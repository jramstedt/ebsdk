/*

 Copyright (C) 1990-1997 Mark Adler, Richard B. Wales, Jean-loup Gailly,
 Kai Uwe Rommel, Onno van der Linden and Igor Mandrichenko.
 Permission is granted to any individual or institution to use, copy, or
 redistribute this software so long as all of the original files are included,
 that it is not sold for profit, and that this copyright notice is retained.

*/

/* Get the right unistd.h; sure wish our headers weren't so screwed. */
#ifdef __GNUC__
#define __USE_FIXED_PROTOTYPES__
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <support/Errors.h>     /* for B_NO_ERROR */

#define USE_EF_UT_TIME          /* Enable use of "UT" extra field time info */

#define EB_L_BE_LEN 5           /* min size is an unsigned long and flag */
#define EB_C_BE_LEN 5           /* Length of data in local EF and flag.  */

#define EB_BE_FL_NATURAL    0x01    /* data is 'natural' (not compressed) */
#define EB_BE_FL_BADBITS    0xfe    /* bits currently undefined           */

/* Set a file's MIME type. */
#define BE_FILE_TYPE_NAME   "BEOS:TYPE"
void setfiletype( const char *file, const char *type );

#ifdef __GNUC__
#  ifndef readlink
/* Somehow, GNU C is missing this for zipup.c. */
extern ssize_t      readlink(const char *path, char *buf, size_t bufsize);
#  endif
#endif

/* Leave this defined until BeOS has a way of accessing the attributes on a */
/* symbolic link from C.  This might appear in DR10, but it doesn't exist   */
/* in Preview Release or Preview Release 2 (aka DR9 and DR9.1). [cjh]       */
#define BE_NO_SYMLINK_ATTRS 1

/*
DR9 'Be' extra-field layout:

'Be'      - signature
ef_size   - size of data in this EF (little-endian unsigned short)
full_size - uncompressed data size (little-endian unsigned long)
flag      - flags (byte)
            flags & EB_BE_FL_NATURAL    = the data is not compressed
            flags & EB_BE_FL_BADBITS    = the data is corrupted or we
                                          can't handle it properly
data      - compressed or uncompressed file attribute data

If flag & EB_BE_FL_NATURAL, the data is not compressed; this optimisation is
necessary to prevent wasted space for files with small attributes (which
appears to be quite common on the Advanced Access DR9 release).  In this
case, there should be ( ef_size - EB_L_BE_LEN ) bytes of data, and full_size
should equal ( ef_size - EB_L_BE_LEN ).

If the data is compressed, there will be ( ef_size - EB_L_BE_LEN ) bytes of
compressed data, and full_size bytes of uncompressed data.

If a file has absolutely no attributes, there will not be a 'Be' extra field.

The uncompressed data is arranged like this:

attr_name\0 - C string
struct attr_info
attr_data (length in attr_info.size)
*/
