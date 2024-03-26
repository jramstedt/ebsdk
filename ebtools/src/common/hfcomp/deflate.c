/*

 Copyright (C) 1990-1992 Mark Adler, Richard B. Wales, Jean-loup Gailly,
 Kai Uwe Rommel and Igor Mandrichenko.
 Permission is granted to any individual or institution to use, copy, or
 redistribute this software so long as all of the original files are included
 unmodified, that it is not sold for profit, and that this copyright notice
 is retained.

*/

/*
 *  deflate.c by Jean-loup Gailly.
 *
 *  PURPOSE
 *
 *      Identify new text as repetitions of old text within a fixed-
 *      length sliding window trailing behind the new text.
 *
 *  DISCUSSION
 *
 *      The "deflation" process depends on being able to identify portions
 *      of the input text which are identical to earlier input (within a
 *      sliding window trailing behind the input currently being processed).
 *
 *      The most straightforward technique turns out to be the fastest for
 *      most input files: try all possible matches and select the longest.
 *      The key feature is of this algorithm is that insertion and deletions
 *      from the string dictionary are very simple and thus fast. Insertions
 *      and deletions are performed at each input character, whereas string
 *      matches are performed only when the previous match ends. So it is
 *      preferable to spend more time in matches to allow very fast string
 *      insertions and deletions. The matching algorithm for small strings
 *      is inspired from that of Rabin & Karp. A brute force approach is
 *      used to find longer strings when a small match has been found.
 *      A similar algorithm is used in comic (by Jan-Mark Wams) and freeze
 *      (by Leonid Broukhis).
 *         A previous version of this file used a more sophisticated algorithm
 *      (by Fiala and Greene) which is guaranteed to run in linear amortized
 *      time, but has a larger average cost and uses more memory. However
 *      the F&G algorithm may be faster for some highly redundant files if
 *      the parameter max_chain_length (described below) is too large.
 *
 *  ACKNOWLEDGEMENTS
 *
 *      The idea of lazy evaluation of matches is due to Jan-Mark Wams, and
 *      I found it in 'freeze' written by Leonid Broukhis.
 *      Thanks to many info-zippers for bug reports and testing.
 *
 *  REFERENCES
 *
 *      APPNOTE.TXT documentation file in PKZIP 2.0 distribution.
 *
 *      A description of the Rabin and Karp algorithm is given in the book
 *         "Algorithms" by R. Sedgewick, Addison-Wesley, p252.
 *
 *      Fiala,E.R., and Greene,D.H.
 *         Data Compression with Finite Windows, Comm.ACM, 32,4 (1989) 490-595
 *
 *  INTERFACE
 *
 *      void lm_init (int pack_level, USHORT *flags)
 *          Initialize the "longest match" routines for a new file
 *
 *      ULONG deflate (void)
 *          Processes a new input file and return its compressed length. Sets
 *          the compressed length, crc, deflate flags and internal file
 *          attributes.
 */

/*
 * This file is now special purpose to compress a single file for 
 * NT firmware compression. This file comes from base level 19p1 of zip.
 * 
 * Wim Colgate, Digital Equipment Corporation.
 *
 * 14-Mar-1994, Don Rice.  Started work on using this in the srm console build.
 *  The first thng taht was done was to make the compression/decompression files
 *  ansi complient.
 *
 * 15-jun-1994, Don Rice.  Change the PACK_LEVEL to 8 from 9.  It loks like 
 *  there may be a problem with it.
 */

#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>

#include    "compress.h"

static void fill_window(void);
extern	int verbose;

#define HASH_BITS  (15)

/* HASH_SIZE and WSIZE must be powers of two */

#define HASH_SIZE (unsigned)(1<<HASH_BITS)
#define HASH_MASK (HASH_SIZE-1)
#define WMASK     (WSIZE-1)

/* Tail of hash chains */

#define NIL 0

#define FAST 4
#define SLOW 2

/* Matches of length 3 are discarded if their distance exceeds TOO_FAR */

#define TOO_FAR 4096

/* ===========================================================================
 * Local data used by the "longest match" routines.
 */

/* A Pos is an index in the character window. We use short instead of int to
 * save space in the various tables. UINT is used only for parameter passing.
 */

typedef USHORT Pos;
typedef unsigned IPos;

  /* Sliding window. Input bytes are read into the second half of the window,
   * and move to the first half later to keep a dictionary of at least WSIZE
   * bytes. With this organization, matches are limited to a distance of
   * WSIZE-MAX_MATCH bytes, but this ensures that IO is always
   * performed with a length multiple of the block size. Also, it limits
   * the window size to 64K, which is quite useful on MSDOS.
   * To do: limit the window size to WSIZE+BSZ if SMALL_MEM (the code would
   * be less efficient since the data would have to be copied WSIZE/BSZ times)
   */
#ifdef	_MSDOS
UCHAR far window[2L*WSIZE];
#else
UCHAR window[2L*WSIZE];
#endif
  /* Link to older string with same hash index. To limit the size of this
   * array to 64K, this link is maintained only for the last 32K strings.
   * An index in this array is thus a window index modulo 32K.
   */
Pos    prev[WSIZE];

  /* Heads of the hash chains or NIL */
Pos    head[HASH_SIZE];

/* window position at the beginning of the current output block. Gets
 * negative when the window is moved backwards.
 */
long block_start;

#ifdef	_MSDOS
static unsigned near ins_h;  /* hash index of string to be inserted */
#else
static unsigned ins_h;  /* hash index of string to be inserted */
#endif

/* Number of bits by which ins_h and del_h must be shifted at each
 * input step. It must be sUCHAR that after MIN_MATCH steps, the oldest
 * byte no longer takes part in the hash key, that is:
 *   H_SHIFT * MIN_MATCH >= HASH_BITS
 */

#define H_SHIFT  ((HASH_BITS+MIN_MATCH-1)/MIN_MATCH)

/* Length of the best match at previous step. Matches not greater than this
 * are discarded. This is used in the lazy match evaluation.
 */

#ifdef	_MSDOS
unsigned int near prev_length;

unsigned near strstart;      /* start of string to insert */
unsigned near match_start;   /* start of matching string */
static int      near eofile;        /* flag set at end of input file */
static unsigned near lookahead;     /* number of valid bytes ahead in window */

/* To speed up deflation, hash chains are never searched beyond this length.
 * A higher limit improves compression ratio but degrades the speed.
 */

unsigned near max_chain_length;

/* Attempt to find a better match only when the current match is strictly
 * smaller than this value.
 */

/* Use a faster search when the previous match is longer than this */
int near good_match;

#else
unsigned int prev_length;

unsigned strstart;      /* start of string to insert */
unsigned match_start;   /* start of matching string */
static int eofile;        /* flag set at end of input file */
static unsigned lookahead;     /* number of valid bytes ahead in window */

/* To speed up deflation, hash chains are never searched beyond this length.
 * A higher limit improves compression ratio but degrades the speed.
 */

unsigned max_chain_length;

/* Use a faster search when the previous match is longer than this */
int good_match;

#endif

/* Attempt to find a better match only when the current match is strictly
 * smaller than this value.
 */
static unsigned int max_lazy_match;

/* Values for max_lazy_match, good_match and max_chain_length, depending on
 * the desired pack level (0..9). The values given below have been tuned to
 * exclude worst case performance for pathological files. Better values may be
 * found for specific files.
 */
typedef struct config {
   int good_length;
   int max_lazy;
   unsigned max_chain;
   UCHAR flag;
} config;

static config configuration_table[10] = {
/*      good lazy chain flag */
/* 0 */ {0,    0,    0,  0},     /* store only */
/* 1 */ {4,    4,   16,  FAST},  /* maximum speed  */
/* 2 */ {6,    8,   16,  0},
/* 3 */ {8,   16,   32,  0},
/* 4 */ {8,   32,   64,  0},
/* 5 */ {8,   64,  128,  0},
/* 6 */ {8,  128,  256,  0},
/* 7 */ {8,  128,  512,  0},
/* 8 */ {32, 258, 1024,  0},
/* 9 */ {32, 258, 4096,  SLOW}}; /* maximum compression */

/* Note: the current code requires max_lazy >= MIN_MATCH and max_chain >= 4
 * but these restrictions can easily be removed at a small cost.
 */

/* result of memcmp for equal strings */

#define EQUAL 0

int  longest_match (UINT cur_match);

#ifdef DEBUG
static  void check_match (UINT start, IPos match, int length);
#endif

/* The arguments must not have side effects. */

#define MIN(a,b) ((a) <= (b) ? (a) : (b))

/* ===========================================================================
 * Update a hash value with the given input byte
 * IN  assertion: all calls to to UPDATE_HASH are made with consecutive
 *    input characters, so that a running hash key can be computed from the
 *    previous key instead of complete recalculation each time.
 */

#define UPDATE_HASH(h,c) (h = (((h)<<H_SHIFT) ^ (c)) & HASH_MASK)

/* ===========================================================================
 * Insert string s in the dictionary and set match_head to the previous head
 * of the hash chain (the most recent string with same hash key). Return
 * the previous length of the hash chain.
 * IN  assertion: all calls to to INSERT_STRING are made with consecutive
 *    input characters and the first MIN_MATCH bytes of s are valid
 *    (except for the last MIN_MATCH-1 bytes of the input file).
 */

#define INSERT_STRING(s, match_head) \
   (UPDATE_HASH(ins_h, window[(s) + MIN_MATCH-1]), \
    prev[(s) & WMASK] = (Pos)(match_head = head[ins_h]), \
    head[ins_h] = (s))

/* ===========================================================================
 * Initialize the "longest match" routines for a new file
 */

void lm_init (void)
{

    register unsigned j;
/*    int pack_level = 9; */
/* from 9 to 8 because there may be a problem */
    int pack_level = 8;    

    /* Initialize the hash table. */
    for (j = 0;  j < HASH_SIZE; j++) head[j] = NIL;
    /* prev will be initialized on the fly */

    /* Set the default configuration parameters:
     */

    max_lazy_match   = configuration_table[pack_level].max_lazy;
    good_match       = configuration_table[pack_level].good_length;
    max_chain_length = configuration_table[pack_level].max_chain;
/*    *flags          |= configuration_table[pack_level].flag; */

    /* ??? reduce max_chain_length for binary files */

    strstart = 0;
    block_start = 0L;

    eofile = 0;

    ins_h = 0;

    while (lookahead < MIN_LOOKAHEAD && !eofile) fill_window();

    /* If lookahead < MIN_MATCH, ins_h is garbage, but this is
     * not important since only literal bytes will be emitted.
     */

    for (j=0; j<MIN_MATCH-1; j++) UPDATE_HASH(ins_h, window[j]);

}

/* ===========================================================================
 * Set match_start to the longest match starting at the given string and
 * return its length. Matches shorter or equal to prev_length are discarded,
 * in which case the result is equal to prev_length and match_start is
 * garbage.
 * IN assertions: cur_match is the head of the hash chain for the current
 *   string (strstart) and its distance is <= MAX_DIST, and prev_length >= 1
 */

int longest_match(UINT cur_match)
                             /* current match */
{
    unsigned chain_length = max_chain_length;   /* max hash chain length */
    register UCHAR *scan = window + strstart; /* current string */
    register UCHAR *match = scan;             /* matched string */
    register int len;                           /* length of current match */
    int best_len = prev_length;                 /* best match length so far */

    /* Stop when cur_match becomes <= limit. To simplify the code,
     * we prevent matches with the string of window index 0.
     */

    UINT limit = strstart > (IPos)MAX_DIST ? strstart - (IPos)MAX_DIST : NIL;

    register UCHAR scan_start = *scan;
    register UCHAR scan_end1  = scan[best_len-1];
    register UCHAR scan_end   = scan[best_len];

    /* Do not waste too mUCHAR time if we already have a good match: */
    if (prev_length >= (unsigned) good_match) {
        chain_length >>= 2;
    }

    do {
        Assert(cur_match < strstart, "no future");
        match = window + cur_match;

        /* Skip to next match if the match length cannot increase
         * or if the match length is less than 2:
         */

        if (match[best_len] != scan_end ||
            match[best_len-1] != scan_end1 || *match != scan_start)
           continue;

        /* It is not necessary to compare scan[1] and match[1] since they
         * are always equal when the other bytes match, given that
         * the hash keys are equal and that h_shift+8 <= HASH_BITS,
         * that is, when the last byte is entirely included in the hash key.
         * The condition is equivalent to
         *       (HASH_BITS+2)/3 + 8 <= HASH_BITS
         * or: HASH_BITS >= 13
         * Also, we check for a match at best_len-1 to get rid quickly of
         * the match with the suffix of the match made at the previous step,
         * which is known to fail.
         */

        len = 0;

        do {} while (++len < MAX_MATCH && scan[len] == match[len]);


        if (len > best_len) {
            match_start = cur_match;
            best_len = len;
            if (len == MAX_MATCH) break;

            scan_end1  = scan[best_len-1];
            scan_end   = scan[best_len];
        }
    } while (--chain_length != 0 &&
             (cur_match = prev[cur_match & WMASK]) > limit);

    return best_len;
}

#ifdef DEBUG
/* ===========================================================================
 * Check that the match at match_start is indeed a match.
 */
static void check_match(start, match, length)
    UINT start, match;
    int length;
{
    /* check that the match is indeed a match */
    if (memcmp((char*)window + match,
                (char*)window + start, length) != EQUAL) {
        fprintf(stderr,
            " start %d, match %d, length %d\n",
            start, match, length);
        fprintf(stderr, "invalid match");
    }
    if (verbose > 1) {
        fprintf(stderr,"\\[%d,%d]", start-match, length);
        do { putc(window[start++], stderr); } while (--length != 0);
    }
}
#else
#  define check_match(start, match, length)
#endif

/* ===========================================================================
 * Fill the window when the lookahead becomes insufficient.
 * Updates strstart and lookahead, and sets eofile if end of input file.
 * IN assertion: lookahead < MIN_LOOKAHEAD && strstart + lookahead > 0
 * OUT assertions: at least one byte has been read, or eofile is set;
 *    file reads are performed for at least two bytes (required for the
 *    translate_eol option).
 */
static void fill_window(void)
{
    register unsigned n, m;
    unsigned more = (unsigned)((ULONG)2*WSIZE - (ULONG)lookahead - (ULONG)strstart);
    /* Amount of free space at the end of the window. */

    /* If the window is full, move the upper half to the lower one to make
     * room in the upper half.
     */
    if (more == (unsigned)EOF) {
        /* Very unlikely, but possible on 16 bit machine if strstart == 0
         * and lookahead == 1 (input done one byte at time)
         */
        more--;
    } else if (more <= 1) {
        /* By the IN assertion, the window is not empty so we can't confuse
         * more == 0 with more == 64K on a 16 bit machine.
         */
        memcpy((char*)window, (char*)window+WSIZE, (unsigned)WSIZE);
        match_start -= WSIZE;
        strstart    -= WSIZE;
        /* strstart - WSIZE >= WSIZE - 1 - lookahead >= WSIZE - MIN_LOOKAHEAD
         * so we now have strstart >= MAX_DIST:
         */
        Assert (strstart >= MAX_DIST, "window slide too early");
        block_start -= (long) WSIZE;

        for (n = 0; n < HASH_SIZE; n++) {
            m = head[n];
            head[n] = (Pos)(m >= WSIZE ? m-WSIZE : NIL);
        }
        for (n = 0; n < WSIZE; n++) {
            m = prev[n];
            prev[n] = (Pos)(m >= WSIZE ? m-WSIZE : NIL);
            /* If n is not on any hash chain, prev[n] is garbage but
             * its value will never be used.
             */
        }
        more += WSIZE;
        if (verbose) putc('.', stderr);
    }
    /* At this point, more >= 2 */
    n = mem_read((char*)window+strstart+lookahead, more);
    if (n == 0 || n == (unsigned)EOF) {
        eofile = 1;
    } else {
        lookahead += n;
    }
}

/* ===========================================================================
 * FlUSHORT the current block, with given end-of-file flag.
 * IN assertion: strstart is set to the end of the current match.
 */
#define FLUSH_BLOCK(eof) \
   flush_block(block_start >= 0L ? (char*)&window[block_start] : (char*)NULL,\
               (ULONG)(strstart - block_start), (eof))

/* ===========================================================================
 * Same as above, but achieves better compression. We use a lazy
 * evaluation for matches: a match is finally adopted only if there is
 * no better match at the next window position.
 */
ULONG deflate(void)
{
    UINT hash_head = 0;      /* head of hash chain */
    UINT prev_match;         /* previous match */
    int flush;               /* set if current block must be flushed */
    int match_available = 0; /* set if previous match exists */
    register unsigned match_length = MIN_MATCH-1; /* length of best match */
#ifdef DEBUG
    extern ULONG isize;        /* byte length of input file, for debug only */
#endif

    /* Process the input block. */
    while (lookahead != 0) {
        /* Insert the string window[strstart .. strstart+2] in the
         * dictionary, and set hash_head to the head of the hash chain:
         */
        INSERT_STRING(strstart, hash_head);

        /* Find the longest match, discarding those <= prev_length.
         */
        prev_length = match_length, prev_match = match_start;
        match_length = MIN_MATCH-1;

        if (hash_head != NIL && prev_length < max_lazy_match &&
            strstart - hash_head <= MAX_DIST) {
            /* To simplify the code, we prevent matches with the string
             * of window index 0 (in particular we have to avoid a match
             * of the string with itself at the start of the input file).
             */
            match_length = longest_match (hash_head);
            /* longest_match() sets match_start */
            if (match_length > lookahead) match_length = lookahead;
            /* Ignore a length 3 match if it is too distant: */
            if (match_length == MIN_MATCH && strstart-match_start > TOO_FAR){
                /* If prev_match is also MIN_MATCH, match_start is garbage
                 * but we will ignore the current match anyway.
                 */
                match_length--;
            }
        }
        /* If there was a match at the previous step and the current
         * match is not better, output the previous match:
         */
        if (prev_length >= MIN_MATCH && match_length <= prev_length) {

            check_match(strstart-1, prev_match, prev_length);

            flush = ct_tally(strstart-1-prev_match, prev_length - MIN_MATCH);

            /* Insert in hash table all strings up to the end of the match.
             * strstart-1 and strstart are already inserted.
             */
            lookahead -= prev_length-1;
            prev_length -= 2;
            do {
                strstart++;
                INSERT_STRING(strstart, hash_head);
                /* strstart never exceeds WSIZE-MAX_MATCH, so there are
                 * always MIN_MATCH bytes ahead. If lookahead < MIN_MATCH
                 * these bytes are garbage, but it does not matter since the
                 * next lookahead bytes will always be emitted as literals.
                 */
            } while (--prev_length != 0);
            match_available = 0;
            match_length = MIN_MATCH-1;

        } else if (match_available) {
            /* If there was no match at the previous position, output a
             * single literal. If there was a match but the current match
             * is longer, truncate the previous match to a single literal.
             */
            flush = ct_tally (0, window[strstart-1]);
            Tracevv((stderr,"%c",window[strstart-1]));
            lookahead--;
        } else {
            /* There is no previous match to compare with, wait for
             * the next step to decide.
             */
            match_available = 1;
            flush = 0;
            lookahead--;
        }
        if (flush) FLUSH_BLOCK(0), block_start = strstart;
        strstart++;
#if 0
        Assert (strstart <= isize && lookahead <= isize, "a bit too far");
#endif

        /* Make sure that we always have enough lookahead, except
         * at the end of the input file. We need MAX_MATCH bytes
         * for the next match, plus MIN_MATCH bytes to insert the
         * string following the next match.
         */
        while (lookahead < MIN_LOOKAHEAD && !eofile) fill_window();
    }
    if (match_available) ct_tally (0, window[strstart-1]);

    return FLUSH_BLOCK(1); /* eof */
}
