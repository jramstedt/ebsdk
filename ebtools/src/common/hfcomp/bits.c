/*

 Copyright (C) 1990-1992 Mark Adler, Richard B. Wales, Jean-loup Gailly,
 Kai Uwe Rommel and Igor Mandrichenko.
 Permission is granted to any individual or institution to use, copy, or
 redistribute this software so long as all of the original files are included
 unmodified, that it is not sold for profit, and that this copyright notice
 is retained.

*/

/*
 *  bits.c by Jean-loup Gailly and Kai Uwe Rommel.
 *
 *  This is a new version of im_bits.c originally written by Richard B. Wales
 *
 *  PURPOSE
 *
 *      Output variable-length bit strings. Compression can be done
 *      to a file or to memory.
 *
 *  DISCUSSION
 *
 *      The PKZIP "deflate" file format interprets compressed file data
 *      as a sequence of bits.  Multi-bit strings in the file may cross
 *      byte boundaries without restriction.
 *
 *      The first bit of each byte is the low-order bit.
 *
 *      The routines in this file allow a variable-length bit value to
 *      be output right-to-left (useful for literal values). For
 *      left-to-right output (useful for code strings from the tree routines),
 *      the bits must have been reversed first with bi_reverse().
 *
 *      For in-memory compression, the compressed bit stream goes directly
 *      into the requested output buffer. The input data is read in blocks
 *      by the mem_read() function.
 *
 *  INTERFACE
 *
 *      void bi_init (FILE *zipfile)
 *          Initialize the bit string routines.
 *
 *      void send_bits (int value, int length)
 *          Write out a bit string, taking the source bits right to
 *          left.
 *
 *      int bi_reverse (int value, int length)
 *          Reverse the bits of a bit string, taking the source bits left to
 *          right and emitting them right to left.
 *
 *      void bi_windup (void)
 *          Write out any remaining bits in an incomplete byte.
 *
 *      void copy_block(char far *buf, unsigned len, int header)
 *          Copy a stored block to the zip file, storing first the length and
 *          its one's complement if requested.
 *
 *      int seekable(void)
 *          Return true if the zip file can be seeked.
 *
 *      ULONG memcompress (char *tgt, ulg tgtsize, char *src, ulg srcsize);
 *          Compress the source buffer src into the target buffer tgt.
 */

/* modification History
 *
 * 14-Mar-1994, Don Rice.  Started work on using this in the srm console build.
 *  The first thng taht was done was to make the compression/decompression files
 *  ansi complient.
 *
*/

#include    <stdio.h>
#include    "compress.h"
#include    <string.h>
/* ===========================================================================
 * Local data used by the "bit string" routines.
 */

static unsigned short bi_buf;
/* Output buffer. bits are inserted starting at the bottom (least significant
 * bits).
 */

#define Buf_size (8 * 2*sizeof(char))
/* Number of bits used within bi_buf. (bi_buf might be implemented on
 * more than 16 bits on some systems.)
 */

static int bi_valid;
/* Number of valid bits in bi_buf.  All bits above the last valid bit
 * are always zero.
 */

static unsigned char *in_buf, *out_buf;
/* Current input and output buffers. in_buf is used only for in-memory
 * compression.
 */

static ULONG in_offset, out_offset;
/* Current offset in input and output buffers. in_offset is used only for
 * in-memory compression.
 */

static ULONG in_size, out_size;
/* Size of current input and output buffers */

#ifdef DEBUG
ULONG bits_sent;   /* bit length of the compressed data */
#endif

/* Output a 16 bit value to the bit stream, lower (oldest) byte first */
#define PUTSHORT(w) \
{ if (out_offset < out_size-1) { \
    out_buf[out_offset++] = (char) ((w) & 0xff); \
    out_buf[out_offset++] = (char) ((USHORT)(w) >> 8); \
  } \
}

#define PUTBYTE(b) \
{ if (out_offset < out_size) { \
    out_buf[out_offset++] = (char) (b); \
  } \
}

/* ===========================================================================
 * Send a value on a given number of bits.
 * IN assertion: length <= 16 and value fits in length bits.
 */
void send_bits(int value, int length)
    /* value to send */
    /* number of bits */
{
#ifdef DEBUG
    Tracevv((stderr," l %2d v %4x ", length, value));
    Assert(length > 0 && length <= 15, "invalid length");
    bits_sent += (ULONG)length;
#endif
    /* If not enough room in bi_buf, use (valid) bits from bi_buf and
     * (16 - bi_valid) bits from value, leaving (width - (16-bi_valid))
     * unused bits in value.
     */
    if (bi_valid > (int)Buf_size - length) {
        bi_buf |= (value << bi_valid);
        PUTSHORT(bi_buf);
        bi_buf = (USHORT)value >> (Buf_size - bi_valid);
        bi_valid += length - Buf_size;
    } else {
        bi_buf |= value << bi_valid;
        bi_valid += length;
    }
}

/* ===========================================================================
 * Reverse the first len bits of a code, using straightforward code (a faster
 * method would use a table)
 * IN assertion: 1 <= len <= 15
 */
unsigned bi_reverse(unsigned code, int len)
    /* the value to invert */
    /* its bit length */
{
    register unsigned res = 0;
    do {
        res |= code & 1;
        code >>= 1, res <<= 1;
    } while (--len > 0);
    return res >> 1;
}

/* ===========================================================================
 * Write out any remaining bits in an incomplete byte.
 */
void bi_windup(void)
{
    if (bi_valid > 8) {
        PUTSHORT(bi_buf);
    } else if (bi_valid > 0) {
        PUTBYTE(bi_buf);
    }
    bi_buf = 0;
    bi_valid = 0;
#ifdef DEBUG
    bits_sent = (bits_sent+7) & ~7;
#endif
}

/* ===========================================================================
 * Copy a stored block to the zip file, storing first the length and its
 * one's complement if requested.
 */

#ifdef _MSDOS
void copy_block(char far *buf, unsigned len, int header)
#else
void copy_block(char *buf, unsigned len, int header)
#endif

/* the input data */
/* its length */
/* true if block header must be written */
{
    bi_windup();              /* align on byte boundary */

    if (header) {
        PUTSHORT((USHORT)len);   
        PUTSHORT((USHORT)~len);
#ifdef DEBUG
        bits_sent += 2*16;
#endif
    }
    if (out_offset + (ULONG)len > out_size) {
        fprintf (stderr,"output buffer too small for in-memory compression");
    } else {
        memcpy(out_buf + out_offset, buf, len);
        out_offset += (ULONG)len;
    }
#ifdef DEBUG
    bits_sent += (ULONG)len<<3;
#endif
}

/* ===========================================================================
 * In-memory compression. This version can be used only if the entire input
 * fits in one memory buffer. The compression is then done in a single
 * call of memcompress(). (An extension to allow repeated calls would be
 * possible but is not needed here.)
 * The first two bytes of the compressed output are set to a short with the
 * method used (DEFLATE or STORE). The following four bytes contain the CRC.
 * The values are stored in little-endian order on all machines.
 * This function returns the byte size of the compressed output, including
 * the first six bytes (method and crc).
 *
 *   inputs :
 *	    char *tgt, *src;		target and source buffers
 *	    ULONG tgtsize, srcsize;	target and source sizes
 */

ULONG memcompress(unsigned char *tgt, 
    ULONG tgtsize, 
    unsigned char *src, 
    ULONG srcsize)
{
    int method      = DEFLATE;

    if (tgtsize <= 6L) {
      fprintf (stderr,"target buffer too small");
      return 0;
    }

    in_buf    = src;
    in_size   = srcsize;
    in_offset = 0;

    out_buf    = tgt;
    out_size   = tgtsize;
/*    out_offset = 2 + 4; */
    out_offset = 0;

    ct_init();
    lm_init();
    deflate();

#if 0
    /* For portability, force little-endian order on all machines: */
    tgt[0] = (char)(method & 0xff);
    tgt[1] = (char)((method >> 8) & 0xff);
    tgt[2] = (char)((crc) & 0xff);
    tgt[3] = (char)((crc >> 8) & 0xff);
    tgt[4] = (char)((crc >> 16) & 0xff);
    tgt[5] = (char)((crc >> 24) & 0xff);
#endif

    return out_offset;
}

/* ===========================================================================
 * In-memory read function. As opposed to file_read(), this function
 * does not perform end-of-line translation, and does not update the
 * crc and input size.
 *    Note that the size of the entire input buffer is an unsigned long,
 * but the size used in mem_read() is only an unsigned int. This makes a
 * difference on 16 bit machines. mem_read() may be called several
 * times for an in-memory compression.
 */
int mem_read(char *buf, unsigned size)
{
    if (in_offset < in_size) {
        ULONG block_size = in_size - in_offset;
        if (block_size > (ULONG)size) block_size = (ULONG)size;
        memcpy(buf, in_buf + in_offset, (unsigned)block_size);
        in_offset += block_size;
        return (int)block_size;
    } else {
        return 0; /* end of input */
    }
}
