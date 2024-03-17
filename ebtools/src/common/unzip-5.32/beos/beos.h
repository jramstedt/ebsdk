/* beos.h -- A few handy things for the BeOS port.     */
/* (c) 1997 Chris Herborth (chrish@qnx.com)            */
/* This is covered under the usual Info-ZIP copyright. */

#define EF_BE_ID        0x6542      /* 'Be' in little-endian form    */
#define EF_BE_SIZE      5           /* at least unsigned long + flag */

#define EF_BE_FL_NATURAL    0x01    /* data is 'natural' (not compressed) */
#define EF_BE_FL_BADBITS    0xfe    /* bits currently undefined           */

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
