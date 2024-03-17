/*---------------------------------------------------------------------------

  extract.c

  This file contains the high-level routines ("driver routines") for extrac-
  ting and testing zipfile members.  It calls the low-level routines in files
  explode.c, inflate.c, unreduce.c and unshrink.c.

  Contains:  extract_or_test_files()
             store_info()
             extract_or_test_member()
             TestExtraField()
             test_compr_eb()
             memextract()
             memflush()
             fnfilter()

  ---------------------------------------------------------------------------*/


#define EXTRACT_C
#define UNZIP_INTERNAL
#include "unzip.h"
#include "crypt.h"
#ifdef WINDLL
#  ifdef POCKET_UNZIP
#    include "wince/intrface.h"
#  else
#    include "windll/windll.h"
#  endif
#endif

#define GRRDUMP(buf,len) { \
    int i, j; \
 \
    for (j = 0;  j < (len)/16;  ++j) { \
        printf("        "); \
        for (i = 0;  i < 16;  ++i) \
            printf("%02x ", (uch)(buf)[i+(j<<4)]); \
        printf("\n        "); \
        for (i = 0;  i < 16;  ++i) { \
            char c = (char)(buf)[i+(j<<4)]; \
 \
            if (c == '\n') \
                printf("\\n "); \
            else if (c == '\r') \
                printf("\\r "); \
            else \
                printf(" %c ", c); \
        } \
        printf("\n"); \
    } \
    if ((len) % 16) { \
        printf("        "); \
        for (i = j<<4;  i < (len);  ++i) \
            printf("%02x ", (uch)(buf)[i]); \
        printf("\n        "); \
        for (i = j<<4;  i < (len);  ++i) { \
            char c = (char)(buf)[i]; \
 \
            if (c == '\n') \
                printf("\\n "); \
            else if (c == '\r') \
                printf("\\r "); \
            else \
                printf(" %c ", c); \
        } \
        printf("\n"); \
    } \
}

static int store_info OF((__GPRO));
static int extract_or_test_member OF((__GPRO));
#ifndef SFX
   static int TestExtraField OF((__GPRO__ uch *ef, unsigned ef_len));
   static int test_compr_eb OF((__GPRO__ uch *eb, unsigned eb_size,
        unsigned compr_offset,
        int (*test_uc_ebdata)(__GPRO__ uch *eb, unsigned eb_size,
                              uch *eb_ucptr, ulg eb_ucsize)));
#endif
#ifdef UNIX
   static int dircomp OF((ZCONST zvoid *a, ZCONST zvoid *b));
#endif



/*******************************/
/*  Strings used in extract.c  */
/*******************************/

static char Far VersionMsg[] =
  "   skipping: %-22s  need %s compat. v%u.%u (can do v%u.%u)\n";
static char Far ComprMsgNum[] =
  "   skipping: %-22s  unsupported compression method %d\n";
#ifndef SFX
   static char Far ComprMsgName[] =
     "   skipping: %-22s  `%s' method not supported\n";
   static char Far CmprNone[]       = "store";
   static char Far CmprShrink[]     = "shrink";
   static char Far CmprReduce[]     = "reduce";
   static char Far CmprImplode[]    = "implode";
   static char Far CmprTokenize[]   = "tokenize";
   static char Far CmprDeflate[]    = "deflate";
   static char Far CmprEnDeflate[]  = "enhanced deflate";
   static char Far CmprDCLImplode[] = "DCL implode";
   static char Far *ComprNames[NUM_METHODS] = {
     CmprNone, CmprShrink, CmprReduce, CmprReduce, CmprReduce, CmprReduce,
     CmprImplode, CmprTokenize, CmprDeflate, CmprEnDeflate, CmprDCLImplode
   };
#endif /* !SFX */
static char Far FilNamMsg[] =
  "%s:  bad filename length (%s)\n";
static char Far ExtFieldMsg[] =
  "%s:  bad extra field length (%s)\n";
static char Far OffsetMsg[] =
  "file #%d:  bad zipfile offset (%s):  %ld\n";
static char Far ExtractMsg[] =
  "%8sing: %-22s  %s%s";
#ifndef SFX
   static char Far LengthMsg[] =
     "%s  %s:  %ld bytes required to uncompress to %lu bytes;\n    %s\
      supposed to require %lu bytes%s%s%s\n";
#endif

static char Far BadFileCommLength[] = "%s:  bad file comment length\n";
static char Far LocalHdrSig[] = "local header sig";
static char Far BadLocalHdr[] = "file #%d:  bad local header\n";
static char Far AttemptRecompensate[] = "  (attempting to re-compensate)\n";
#ifndef SFX
   static char Far BackslashPathSep[] =
     "warning:  %s appears to use backslashes as path separators\n";
#endif
static char Far SkipVolumeLabel[] = "   skipping: %-22s  %svolume label\n";

#ifdef UNIX  /* messages of code for setting directory attributes */
   static char Far DirlistNoMem[] =
     "warning:  cannot alloc memory for dir times/permissions/UIDs/GIDs\n";
   static char Far DirlistEntryNoMem[] =
     "cannot alloc memory for dir times/permissions/UID/GID\n";
   static char Far DirlistSortNoMem[] =
     "warning:  cannot alloc memory to sort dir times/perms/etc.\n";
   static char Far DirlistUidGidFailed[] =
     "warning:  cannot set UID %d and/or GID %d for %s\n";
   static char Far DirlistUtimeFailed[] =
     "warning:  cannot set modification, access times for %s\n";
#  ifndef NO_CHMOD
     static char Far DirlistChmodFailed[] =
       "warning:  cannot set permissions for %s\n";
#  endif
#endif

#ifndef WINDLL
   static char Far ReplaceQuery[] =
     "replace %s? [y]es, [n]o, [A]ll, [N]one, [r]ename: ";
   static char Far AssumeNone[] = " NULL\n(assuming [N]one)\n";
   static char Far NewNameQuery[] = "new name: ";
   static char Far InvalidResponse[] = "error:  invalid response [%c]\n";
#endif /* !WINDLL */

static char Far ErrorInArchive[] = "At least one %serror was detected in %s.\n";
static char Far ZeroFilesTested[] = "Caution:  zero files tested in %s.\n";

#ifndef VMS
   static char Far VMSFormatQuery[] =
     "\n%s:  stored in VMS format.  Extract anyway? (y/n) ";
#endif

#if CRYPT
   static char Far SkipCannotGetPasswd[] =
     "   skipping: %-22s  unable to get password\n";
   static char Far SkipIncorrectPasswd[] =
     "   skipping: %-22s  incorrect password\n";
   static char Far FilesSkipBadPasswd[] =
     "%d file%s skipped because of incorrect password.\n";
   static char Far MaybeBadPasswd[] =
     "    (may instead be incorrect password)\n";
#else
   static char Far SkipEncrypted[] =
     "   skipping: %-22s  encrypted (not supported)\n";
#endif

static char Far NoErrInCompData[] =
  "No errors detected in compressed data of %s.\n";
static char Far NoErrInTestedFiles[] =
  "No errors detected in %s for the %d file%s tested.\n";
static char Far FilesSkipped[] =
  "%d file%s skipped because of unsupported compression or encoding.\n";

static char Far ErrUnzipFile[] = "  error:  %s%s %s\n";
static char Far ErrUnzipNoFile[] = "\n  error:  %s%s\n";
static char Far NotEnoughMem[] = "not enough memory to ";
static char Far InvalidComprData[] = "invalid compressed data to ";
static char Far Inflate[] = "inflate";

#ifndef SFX
   static char Far Explode[] = "explode";
#ifndef LZW_CLEAN
   static char Far Unshrink[] = "unshrink";
#endif
#endif

#if (!defined(DELETE_IF_FULL) || !defined(HAVE_UNLINK))
   static char Far FileTruncated[] = "warning:  %s is probably truncated\n";
#endif

static char Far FileUnknownCompMethod[] = "%s:  unknown compression method\n";
static char Far BadCRC[] = " bad CRC %08lx  (should be %08lx)\n";

      /* TruncEAs[] also used in OS/2 mapname(), close_outfile() */
char Far TruncEAs[] = " compressed EA data missing (%d bytes)%s";
char Far TruncNTSD[] = " compressed WinNT security data missing (%d bytes)%s";

#ifndef SFX
   static char Far InconsistEFlength[] = "bad extra-field entry:\n \
     EF block length (%u bytes) exceeds remaining EF data (%u bytes)\n";
   static char Far InvalidComprDataEAs[] = " invalid compressed data for EAs\n";
#  if (defined(WIN32) && defined(NTSD_EAS))
     static char Far InvalidSecurityEAs[] = " EAs fail security check\n";
#  endif
   static char Far UnsuppNTSDVersEAs[] = " unsupported NTSD EAs version %d\n";
   static char Far BadCRC_EAs[] = " bad CRC for extended attributes\n";
   static char Far UnknComprMethodEAs[] =
     " unknown compression method for EAs (%u)\n";
   static char Far NotEnoughMemEAs[] = " out of memory while inflating EAs\n";
   static char Far UnknErrorEAs[] = " unknown error on extended attributes\n";
#endif /* !SFX */

static char Far UnsupportedExtraField[] =
  "\nerror:  unsupported extra-field compression type (%u)--skipping\n";
static char Far BadExtraFieldCRC[] =
  "error [%s]:  bad extra-field CRC %08lx (should be %08lx)\n";





/**************************************/
/*  Function extract_or_test_files()  */
/**************************************/

int extract_or_test_files(__G)    /* return PK-type error code */
     __GDEF
{
    uch *cd_inptr;
    int i, j, cd_incnt, renamed, query, filnum=(-1), blknum=0;
    int error, error_in_archive=PK_COOL, *fn_matched=NULL, *xn_matched=NULL;
#ifdef WINDLL
    int done_once = 0;
#else
    int len;
#endif
    ush members_remaining, num_skipped=0, num_bad_pwd=0;
    long cd_bufstart, bufstart, inbuf_offset, request;
    LONGINT old_extra_bytes = 0L;
#ifdef UNIX
    int do_dirtimes=TRUE, num_dirs=0;
    dirtime *dirlist=(dirtime *)NULL, **sorted_dirlist=(dirtime **)NULL;
#endif


/*---------------------------------------------------------------------------
    The basic idea of this function is as follows.  Since the central di-
    rectory lies at the end of the zipfile and the member files lie at the
    beginning or middle or wherever, it is not very desirable to simply
    read a central directory entry, jump to the member and extract it, and
    then jump back to the central directory.  In the case of a large zipfile
    this would lead to a whole lot of disk-grinding, especially if each mem-
    ber file is small.  Instead, we read from the central directory the per-
    tinent information for a block of files, then go extract/test the whole
    block.  Thus this routine contains two small(er) loops within a very
    large outer loop:  the first of the small ones reads a block of files
    from the central directory; the second extracts or tests each file; and
    the outer one loops over blocks.  There's some file-pointer positioning
    stuff in between, but that's about it.  Btw, it's because of this jump-
    ing around that we can afford to be lenient if an error occurs in one of
    the member files:  we should still be able to go find the other members,
    since we know the offset of each from the beginning of the zipfile.
  ---------------------------------------------------------------------------*/

    G.pInfo = G.info;
    members_remaining = G.ecrec.total_entries_central_dir;

#ifdef UNIX
    dirlist = (dirtime *)malloc(members_remaining*sizeof(dirtime));
    if (dirlist == (dirtime *)NULL) {
        Info(slide, 0x401, ((char *)slide, LoadFarString(DirlistNoMem)));
        do_dirtimes = FALSE;
    }
#endif /* UNIX */

#if CRYPT
    G.newzip = TRUE;
#endif
#ifndef SFX
    G.reported_backslash = FALSE;
#endif

    /* malloc space for check on unmatched filespecs (OK if one or both NULL) */
    if (G.filespecs > 0  &&
        (fn_matched=(int *)malloc(G.filespecs*sizeof(int))) != (int *)NULL)
        for (i = 0;  i < G.filespecs;  ++i)
            fn_matched[i] = FALSE;
    if (G.xfilespecs > 0  &&
        (xn_matched=(int *)malloc(G.xfilespecs*sizeof(int))) != (int *)NULL)
        for (i = 0;  i < G.xfilespecs;  ++i)
            xn_matched[i] = FALSE;

/*---------------------------------------------------------------------------
    Begin main loop over blocks of member files.  We know the entire central
    directory is on this disk:  we would not have any of this information un-
    less the end-of-central-directory record was on this disk, and we would
    not have gotten to this routine unless this is also the disk on which
    the central directory starts.  In practice, this had better be the ONLY
    disk in the archive, but we'll add multi-disk support soon.
  ---------------------------------------------------------------------------*/

    while (members_remaining) {
        j = 0;
#ifdef AMIGA
        memzero(G.filenotes, DIR_BLKSIZ * sizeof(char *));
#endif

        /*
         * Loop through files in central directory, storing offsets, file
         * attributes, case-conversion and text-conversion flags until block
         * size is reached.
         */

        while (members_remaining && (j < DIR_BLKSIZ)) {
            --members_remaining;
            G.pInfo = &G.info[j];

            if (readbuf(__G__ G.sig, 4) == 0) {
                error_in_archive = PK_EOF;
                members_remaining = 0;  /* ...so no more left to do */
                break;
            }
            if (strncmp(G.sig, G.central_hdr_sig, 4)) {  /* just to make sure */
                Info(slide, 0x401, ((char *)slide, LoadFarString(CentSigMsg),
                  j));
                Info(slide, 0x401, ((char *)slide, LoadFarString(ReportMsg)));
                error_in_archive = PK_BADERR;
                members_remaining = 0;  /* ...so no more left to do */
                break;
            }
            /* process_cdir_file_hdr() sets pInfo->hostnum, pInfo->lcflag */
            if ((error = process_cdir_file_hdr(__G)) != PK_COOL) {
                error_in_archive = error;   /* only PK_EOF defined */
                members_remaining = 0;  /* ...so no more left to do */
                break;
            }
            if ((error = do_string(__G__ G.crec.filename_length, DS_FN)) !=
                 PK_COOL)
            {
                if (error > error_in_archive)
                    error_in_archive = error;
                if (error > PK_WARN) {  /* fatal:  no more left to do */
                    Info(slide, 0x401, ((char *)slide, LoadFarString(FilNamMsg),
                      FnFilter1(G.filename), "central"));
                    members_remaining = 0;
                    break;
                }
            }
            if ((error = do_string(__G__ G.crec.extra_field_length,
                EXTRA_FIELD)) != 0)
            {
                if (error > error_in_archive)
                    error_in_archive = error;
                if (error > PK_WARN) {  /* fatal */
                    Info(slide, 0x401, ((char *)slide,
                      LoadFarString(ExtFieldMsg),
                      FnFilter1(G.filename), "central"));
                    members_remaining = 0;
                    break;
                }
            }
#ifdef AMIGA
            G.filenote_slot = j;
            if ((error = do_string(__G__ G.crec.file_comment_length,
                                   G.N_flag ? FILENOTE : SKIP)) != PK_COOL)
#else
            if ((error = do_string(__G__ G.crec.file_comment_length, SKIP))
                != PK_COOL)
#endif
            {
                if (error > error_in_archive)
                    error_in_archive = error;
                if (error > PK_WARN) {  /* fatal */
                    Info(slide, 0x421, ((char *)slide,
                      LoadFarString(BadFileCommLength),
                      FnFilter1(G.filename)));
                    members_remaining = 0;
                    break;
                }
            }
            if (G.process_all_files) {
                if (store_info(__G))
                    ++j;  /* file is OK; info[] stored; continue with next */
                else
                    ++num_skipped;
            } else {
                int   do_this_file = FALSE;
                char  **pfn = G.pfnames-1;

                while (*++pfn)
                    if (match(G.filename, *pfn, G.C_flag)) {
                        do_this_file = TRUE;   /* ^-- ignore case or not? */
                        if (fn_matched)
                            fn_matched[(int)(pfn-G.pfnames)] = TRUE;
                        break;       /* found match, so stop looping */
                    }
                if (do_this_file) {  /* check if this is an excluded file */
                    char  **pxn = G.pxnames-1;

                    while (*++pxn)
                        if (match(G.filename, *pxn, G.C_flag)) {
                            do_this_file = FALSE;  /* ^-- ignore case or not? */
                            if (xn_matched)
                                xn_matched[(int)(pxn-G.pxnames)] = TRUE;
                            break;
                        }
                }
                if (do_this_file)
                    if (store_info(__G))
                        ++j;            /* file is OK */
                    else
                        ++num_skipped;  /* unsupp. compression or encryption */
            } /* end if (process_all_files) */


        } /* end while-loop (adding files to current block) */

        /* save position in central directory so can come back later */
        cd_bufstart = G.cur_zipfile_bufstart;
        cd_inptr = G.inptr;
        cd_incnt = G.incnt;

    /*-----------------------------------------------------------------------
        Second loop:  process files in current block, extracting or testing
        each one.
      -----------------------------------------------------------------------*/

        for (i = 0; i < j; ++i) {
            filnum = i + blknum*DIR_BLKSIZ;
            G.pInfo = &G.info[i];
#ifdef NOVELL_BUG_FAILSAFE
            G.dne = FALSE;  /* assume file exists until stat() says otherwise */
#endif

            /* if the target position is not within the current input buffer
             * (either haven't yet read far enough, or (maybe) skipping back-
             * ward), skip to the target position and reset readbuf(). */

            /* ZLSEEK(pInfo->offset):  */
            request = G.pInfo->offset + G.extra_bytes;
            inbuf_offset = request % INBUFSIZ;
            bufstart = request - inbuf_offset;

            Trace((stderr, "\ndebug: request = %ld, inbuf_offset = %ld\n",
              request, inbuf_offset));
            Trace((stderr,
              "debug: bufstart = %ld, cur_zipfile_bufstart = %ld\n",
              bufstart, G.cur_zipfile_bufstart));
            if (request < 0) {
                Info(slide, 0x401, ((char *)slide, LoadFarStringSmall(SeekMsg),
                  G.zipfn, LoadFarString(ReportMsg)));
                error_in_archive = PK_ERR;
                if (filnum == 0 && G.extra_bytes != 0L) {
                    Info(slide, 0x401, ((char *)slide,
                      LoadFarString(AttemptRecompensate)));
                    old_extra_bytes = G.extra_bytes;
                    G.extra_bytes = 0L;
                    request = G.pInfo->offset;  /* could also check if != 0 */
                    inbuf_offset = request % INBUFSIZ;
                    bufstart = request - inbuf_offset;
                    Trace((stderr, "debug: request = %ld, inbuf_offset = %ld\n",
                      request, inbuf_offset));
                    Trace((stderr,
                      "debug: bufstart = %ld, cur_zipfile_bufstart = %ld\n",
                      bufstart, G.cur_zipfile_bufstart));
                } else {
                    error_in_archive = PK_BADERR;
                    continue;  /* this one hosed; try next */
                }
            }
            /* try again */
            if (request < 0) {
                Trace((stderr, "debug: recompensated request still < 0\n"));
                Info(slide, 0x401, ((char *)slide, LoadFarStringSmall(SeekMsg),
                  G.zipfn, LoadFarString(ReportMsg)));
                error_in_archive = PK_BADERR;
                continue;
            } else if (bufstart != G.cur_zipfile_bufstart) {
                Trace((stderr, "debug: bufstart != cur_zipfile_bufstart\n"));
#ifdef USE_STRM_INPUT
                fseek((FILE *)G.zipfd,(LONGINT)bufstart,SEEK_SET);
                G.cur_zipfile_bufstart = ftell((FILE *)G.zipfd);
#else /* !USE_STRM_INPUT */
                G.cur_zipfile_bufstart =
                  lseek(G.zipfd,(LONGINT)bufstart,SEEK_SET);
#endif /* ?USE_STRM_INPUT */
                if ((G.incnt = read(G.zipfd,(char *)G.inbuf,INBUFSIZ)) <= 0)
                {
                    Info(slide, 0x401, ((char *)slide, LoadFarString(OffsetMsg),
                      filnum, "lseek", bufstart));
                    error_in_archive = PK_BADERR;
                    continue;   /* can still do next file */
                }
                G.inptr = G.inbuf + (int)inbuf_offset;
                G.incnt -= (int)inbuf_offset;
            } else {
                G.incnt += (int)(G.inptr-G.inbuf) - (int)inbuf_offset;
                G.inptr = G.inbuf + (int)inbuf_offset;
            }

            /* should be in proper position now, so check for sig */
            if (readbuf(__G__ G.sig, 4) == 0) {  /* bad offset */
                Info(slide, 0x401, ((char *)slide, LoadFarString(OffsetMsg),
                  filnum, "EOF", request));
                error_in_archive = PK_BADERR;
                continue;   /* but can still try next one */
            }
            if (strncmp(G.sig, G.local_hdr_sig, 4)) {
                Info(slide, 0x401, ((char *)slide, LoadFarString(OffsetMsg),
                  filnum, LoadFarStringSmall(LocalHdrSig), request));
                /*
                    GRRDUMP(G.sig, 4)
                    GRRDUMP(G.local_hdr_sig, 4)
                 */
                error_in_archive = PK_ERR;
                if ((filnum == 0 && G.extra_bytes != 0L) ||
                    (G.extra_bytes == 0L && old_extra_bytes != 0L)) {
                    Info(slide, 0x401, ((char *)slide,
                      LoadFarString(AttemptRecompensate)));
                    if (G.extra_bytes) {
                        old_extra_bytes = G.extra_bytes;
                        G.extra_bytes = 0L;
                    } else
                        G.extra_bytes = old_extra_bytes;  /* third attempt */
                    ZLSEEK(G.pInfo->offset)
                    if (readbuf(__G__ G.sig, 4) == 0) {  /* bad offset */
                        Info(slide, 0x401, ((char *)slide,
                          LoadFarString(OffsetMsg), filnum, "EOF", request));
                        error_in_archive = PK_BADERR;
                        continue;   /* but can still try next one */
                    }
                    if (strncmp(G.sig, G.local_hdr_sig, 4)) {
                        Info(slide, 0x401, ((char *)slide,
                          LoadFarString(OffsetMsg), filnum,
                          LoadFarStringSmall(LocalHdrSig), request));
                        error_in_archive = PK_BADERR;
                        continue;
                    }
                } else
                    continue;  /* this one hosed; try next */
            }
            if ((error = process_local_file_hdr(__G)) != PK_COOL) {
                Info(slide, 0x421, ((char *)slide, LoadFarString(BadLocalHdr),
                  filnum));
                error_in_archive = error;   /* only PK_EOF defined */
                continue;   /* can still try next one */
            }
            if ((error = do_string(__G__ G.lrec.filename_length, DS_FN)) !=
                 PK_COOL)
            {
                if (error > error_in_archive)
                    error_in_archive = error;
                if (error > PK_WARN) {
                    Info(slide, 0x401, ((char *)slide, LoadFarString(FilNamMsg),
                      FnFilter1(G.filename), "local"));
                    continue;   /* go on to next one */
                }
            }
            if (G.extra_field != (uch *)NULL) {
                free(G.extra_field);
                G.extra_field = (uch *)NULL;
            }
            if ((error =
                 do_string(__G__ G.lrec.extra_field_length, EXTRA_FIELD)) != 0)
            {
                if (error > error_in_archive)
                    error_in_archive = error;
                if (error > PK_WARN) {
                    Info(slide, 0x401, ((char *)slide,
                      LoadFarString(ExtFieldMsg),
                      FnFilter1(G.filename), "local"));
                    continue;   /* go on */
                }
            }

            /*
             * just about to extract file:  if extracting to disk, check if
             * already exists, and if so, take appropriate action according to
             * fflag/uflag/overwrite_all/etc. (we couldn't do this in upper
             * loop because we don't store the possibly renamed filename[] in
             * info[])
             */
#ifdef DLL
            if (!G.tflag && !G.cflag && !G.redirect_data)
#else
            if (!G.tflag && !G.cflag)
#endif
            {
                renamed = FALSE;   /* user hasn't renamed output file yet */

startover:
                query = FALSE;
#ifdef MACOS
                G.macflag = (G.pInfo->hostnum == MAC_);
#endif
                /* for files from DOS FAT, check for use of backslash instead
                 *  of slash as directory separator (bug in some zipper(s); so
                 *  far, not a problem in HPFS, NTFS or VFAT systems)
                 */
#ifndef SFX
                if (G.pInfo->hostnum == FS_FAT_ && !strchr(G.filename, '/')) {
                    char *p=G.filename-1;

                    while (*++p) {
                        if (*p == '\\') {
                            if (!G.reported_backslash) {
                                Info(slide, 0x21, ((char *)slide,
                                  LoadFarString(BackslashPathSep), G.zipfn));
                                G.reported_backslash = TRUE;
                                if (!error_in_archive)
                                    error_in_archive = PK_WARN;
                            }
                            *p = '/';
                        }
                    }
                }
#endif /* !SFX */

                /* mapname can create dirs if not freshening or if renamed */
                if ((error = mapname(__G__ renamed)) > PK_WARN) {
                    if (error == IZ_CREATED_DIR) {
#ifdef UNIX
                        if (do_dirtimes) {
                            unsigned eb_izux_flg;

                            dirlist[num_dirs].fn =
                              (char *)malloc(strlen(G.filename) + 1);
                            if (dirlist[num_dirs].fn == (char *)NULL) {
                                Info(slide, 0x401, ((char *)slide,
                                  LoadFarString(DirlistEntryNoMem)));
                                if (!error_in_archive)
                                    error_in_archive = PK_WARN;
                                continue;
                            }
                            strcpy(dirlist[num_dirs].fn, G.filename);
                            dirlist[num_dirs].perms = G.pInfo->file_attr;
                            eb_izux_flg = G.extra_field? ef_scan_for_izux(
                              G.extra_field, G.lrec.extra_field_length, 0,
                              G.lrec.last_mod_file_date,
                              &dirlist[num_dirs].u.t3, dirlist[num_dirs].uidgid)
                              : 0;
                            if (eb_izux_flg & EB_UT_FL_MTIME) {
                                TTrace((stderr,
                                  "\nextract:  Unix dir e.f. modtime = %ld\n",
                                  dirlist[num_dirs].u.t3.mtime));
                            } else {
                                dirlist[num_dirs].u.t3.mtime =
                                  dos_to_unix_time(G.lrec.last_mod_file_date,
                                                   G.lrec.last_mod_file_time);
                            }
                            if (eb_izux_flg & EB_UT_FL_ATIME) {
                                TTrace((stderr,
                                  "\nextract:  Unix dir e.f. actime = %ld\n",
                                  dirlist[num_dirs].u.t3.atime));
                            } else {
                                dirlist[num_dirs].u.t3.atime =
                                  dirlist[num_dirs].u.t3.mtime;
                            }
                            dirlist[num_dirs].have_uidgid =
                                (G.X_flag && (eb_izux_flg & EB_UX2_VALID));
                            ++num_dirs;
                        }
#endif /* UNIX */
                    } else if (error == IZ_VOL_LABEL) {
#ifdef DOS_OS2_W32
                        Info(slide, 0x401, ((char *)slide,
                          LoadFarString(SkipVolumeLabel),
                          FnFilter1(G.filename),
                          G.volflag? "hard disk " : ""));
#else
                        Info(slide, 1, ((char *)slide,
                          LoadFarString(SkipVolumeLabel),
                          FnFilter1(G.filename), ""));
#endif
                    /*  if (!error_in_archive)
                            error_in_archive = PK_WARN;  */
                    } else if (error > PK_ERR  &&  error_in_archive < PK_ERR)
                        error_in_archive = PK_ERR;
                    Trace((stderr, "mapname(%s) returns error = %d\n",
                      FnFilter1(G.filename), error));
                    continue;   /* go on to next file */
                }

#ifdef QDOS
                QFilename(__G__ G.filename);
#endif
                switch (check_for_newer(__G__ G.filename)) {
                    case DOES_NOT_EXIST:
#ifdef NOVELL_BUG_FAILSAFE
                        G.dne = TRUE;   /* stat() says file DOES NOT EXIST */
#endif
                        /* if freshening, don't skip if just renamed */
                        if (G.fflag && !renamed)
                            continue;   /* freshen (no new files):  skip */
                        break;
                    case EXISTS_AND_OLDER:
                        if (G.overwrite_none) {
#ifdef WINDLL
                            char szStr[PATH_MAX];

                            if ((!lpDCL->PromptToOverwrite) || (done_once)) {
                                sprintf(szStr,
                                  "Target file exists.\nSkipping %s\n",
                                  FnFilter1(G.filename));
                                win_fprintf(stdout, strlen(szStr), szStr);
                            } else {
                                query = TRUE;
                                break;
                            }
#endif /* WINDLL */
                            continue;   /* never overwrite:  skip file */
                        }
#ifdef UNIXBACKUP
                        if (!G.overwrite_all && !G.B_flag)
#else
                        if (!G.overwrite_all)
#endif
                            query = TRUE;
                        break;
                    case EXISTS_AND_NEWER:             /* (or equal) */
                        if (G.overwrite_none || (G.uflag && !renamed)) {
#ifdef WINDLL
                            char szStr[PATH_MAX];

                            if ((!lpDCL->PromptToOverwrite) || (done_once)) {
                                sprintf(szStr,
                                  "Target file newer.\nSkipping %s\n",
                                  FnFilter1(G.filename));
                                win_fprintf(stdout, strlen(szStr), szStr);
                            } else {
                                query = TRUE;
                                break;
                            }
#endif /* WINDLL */
                            continue;  /* skip if update/freshen & orig name */
                        }
#ifdef UNIXBACKUP
                        if (!G.overwrite_all && !G.B_flag)
#else
                        if (!G.overwrite_all)
#endif
                            query = TRUE;
                        break;
                }
                if (query) {
#ifdef WINDLL
                    switch (G.replace != NULL ?
                            (*G.replace)(G.filename) : IDM_REPLACE_NONE) {
                        case IDM_REPLACE_RENAME:
                            _ISO_INTERN(G.filename);
                            renamed = TRUE;
                            goto startover;
                        case IDM_REPLACE_YES:
                            break;
                        case IDM_REPLACE_ALL:
                            G.overwrite_all = TRUE;
                            G.overwrite_none = FALSE;  /* just to make sure */
                            break;
                        case IDM_REPLACE_NONE:
                            G.overwrite_none = TRUE;
                            G.overwrite_all = FALSE;  /* make sure */
                            done_once = TRUE;
                            /* FALL THROUGH, skip */
                        case IDM_REPLACE_NO:
                            {
                                char szStr[PATH_MAX];

                                sprintf(szStr,
                                  "Target file newer.\nSkipping %s\n",
                                  FnFilter1(G.filename));
                                win_fprintf(stdout, strlen(szStr), szStr);
                            }
                            continue;
                    }
#else /* !WINDLL */
reprompt:
                    Info(slide, 0x81, ((char *)slide,
                      LoadFarString(ReplaceQuery),
                      FnFilter1(G.filename)));
                    if (fgets(G.answerbuf, 9, stdin) == (char *)NULL) {
                        Info(slide, 1, ((char *)slide,
                          LoadFarString(AssumeNone)));
                        *G.answerbuf = 'N';
                        if (!error_in_archive)
                            error_in_archive = 1;  /* not extracted:  warning */
                    }
                    switch (*G.answerbuf) {
                        case 'A':   /* dangerous option:  force caps */
                            G.overwrite_all = TRUE;
                            G.overwrite_none = FALSE;  /* just to make sure */
                            break;
                        case 'r':
                        case 'R':
                            do {
                                Info(slide, 0x81, ((char *)slide,
                                  LoadFarString(NewNameQuery)));
                                fgets(G.filename, FILNAMSIZ, stdin);
                                /* usually get \n here:  better check for it */
                                len = strlen(G.filename);
                                if (G.filename[len-1] == '\n')
                                    G.filename[--len] = 0;
                            } while (len == 0);
#ifdef WIN32  /* WIN32 fgets( ... , stdin) returns OEM coded strings */
                            _OEM_INTERN(G.filename);
#endif
                            renamed = TRUE;
                            goto startover;   /* sorry for a goto */
                        case 'y':
                        case 'Y':
                            break;
                        case 'N':
                            G.overwrite_none = TRUE;
                            G.overwrite_all = FALSE;  /* make sure */
                            /* FALL THROUGH, skip */
                        case 'n':
                            continue;   /* skip file */
                        default:
                            Info(slide, 1, ((char *)slide,
                              LoadFarString(InvalidResponse), *G.answerbuf));
                            goto reprompt;   /* yet another goto? */
                    } /* end switch (*answerbuf) */
#endif /* ?WINDLL */
                } /* end if (query) */
            } /* end if (extracting to disk) */

#if CRYPT
            if (G.pInfo->encrypted && (error = decrypt(__G)) != PK_COOL) {
                if (error == PK_WARN) {
                    if (!((G.tflag && G.qflag) || (!G.tflag && !QCOND2)))
                        Info(slide, 0x401, ((char *)slide,
                          LoadFarString(SkipIncorrectPasswd),
                          FnFilter1(G.filename)));
                    ++num_bad_pwd;
                } else {  /* (error > PK_WARN) */
                    if (error > error_in_archive)
                        error_in_archive = error;
                    Info(slide, 0x401, ((char *)slide,
                      LoadFarString(SkipCannotGetPasswd),
                      FnFilter1(G.filename)));
                }
                continue;   /* go on to next file */
            }
#endif /* CRYPT */
#ifdef WINDLL         /* GRR:  this should be made generic:  ifdef SOUND? */
            /* play sound during extraction or test, if requested */
            if (G.sound != NULL)
                (*G.sound)();
#endif
#ifdef AMIGA
            G.filenote_slot = i;
#endif
            G.disk_full = 0;
            if ((error = extract_or_test_member(__G)) != PK_COOL) {
                if (error > error_in_archive)
                    error_in_archive = error;       /* ...and keep going */
                if (G.disk_full > 1) {
                    if (fn_matched)
                        free((zvoid *)fn_matched);
                    if (xn_matched)
                        free((zvoid *)xn_matched);
                    return error_in_archive;        /* (unless disk full) */
                }
            }
        } /* end for-loop (i:  files in current block) */


        /*
         * Jump back to where we were in the central directory, then go and do
         * the next batch of files.
         */

#ifdef USE_STRM_INPUT
        fseek((FILE *)G.zipfd, (LONGINT)cd_bufstart, SEEK_SET);
        G.cur_zipfile_bufstart = ftell((FILE *)G.zipfd);
#else /* !USE_STRM_INPUT */
        G.cur_zipfile_bufstart = lseek(G.zipfd,(LONGINT)cd_bufstart,SEEK_SET);
#endif /* ?USE_STRM_INPUT */
        read(G.zipfd, (char *)G.inbuf, INBUFSIZ);  /* been here before... */
        G.inptr = cd_inptr;
        G.incnt = cd_incnt;
        ++blknum;

#ifdef TEST
        printf("\ncd_bufstart = %ld (%.8lXh)\n", cd_bufstart, cd_bufstart);
        printf("cur_zipfile_bufstart = %ld (%.8lXh)\n", cur_zipfile_bufstart,
          cur_zipfile_bufstart);
        printf("inptr-inbuf = %d\n", G.inptr-G.inbuf);
        printf("incnt = %d\n\n", G.incnt);
#endif

    } /* end while-loop (blocks of files in central directory) */

/*---------------------------------------------------------------------------
    Go back through saved list of directories, sort and set times/perms/UIDs
    and GIDs from the deepest level on up.
  ---------------------------------------------------------------------------*/

#ifdef UNIX
    if (num_dirs > 0) {
        sorted_dirlist = (dirtime **)malloc(num_dirs*sizeof(dirtime *));
        if (sorted_dirlist == (dirtime **)NULL) {
            Info(slide, 0x401, ((char *)slide,
              LoadFarString(DirlistSortNoMem)));
        } else {
            if (num_dirs == 1)
                sorted_dirlist[0] = dirlist;
            else {
                for (i = 0;  i < num_dirs;  ++i)
                    sorted_dirlist[i] = &dirlist[i];
                qsort((char *)sorted_dirlist, num_dirs, sizeof(dirtime *),
                  dircomp);
            }

            Trace((stderr, "setting Unix dir times/perms/UIDs/GIDs\n"));
            for (i = 0;  i < num_dirs;  ++i) {
                dirtime *d = sorted_dirlist[i];

                Trace((stderr, "dir = %s\n", d->fn));
                if (d->have_uidgid &&
                    chown(d->fn, (uid_t)d->uidgid[0], (gid_t)d->uidgid[1]))
                {
                    Info(slide, 0x201, ((char *)slide,
                      LoadFarString(DirlistUidGidFailed),
                      d->uidgid[0], d->uidgid[1], d->fn));
                    if (!error_in_archive)
                        error_in_archive = PK_WARN;
                }
                if (utime(d->fn, &d->u.t2)) {
                    Info(slide, 0x201, ((char *)slide,
                      LoadFarString(DirlistUtimeFailed), d->fn));
                    if (!error_in_archive)
                        error_in_archive = PK_WARN;
                }
#ifndef NO_CHMOD
                if (chmod(d->fn, 0xffff & d->perms)) {
                    Info(slide, 0x201, ((char *)slide,
                      LoadFarString(DirlistChmodFailed), d->fn));
                    /* perror("chmod (file attributes) error"); */
                    if (!error_in_archive)
                        error_in_archive = PK_WARN;
                }
#endif /* !NO_CHMOD */
                free(d->fn);
            }
            free(sorted_dirlist);
        }
        free(dirlist);
    }
#endif /* UNIX */

#if (defined(WIN32) && defined(NTSD_EAS))
    process_defer_NT(__G);  /* process any deferred items for this .zip file */
#endif

/*---------------------------------------------------------------------------
    Check for unmatched filespecs on command line and print warning if any
    found.  Free allocated memory.
  ---------------------------------------------------------------------------*/

    if (fn_matched) {
        for (i = 0;  i < G.filespecs;  ++i)
            if (!fn_matched[i]) {
#ifdef DLL
                if (!G.redirect_data && !G.redirect_text)
                    Info(slide, 0x401, ((char *)slide,
                      LoadFarString(FilenameNotMatched), G.pfnames[i]));
                else
                    setFileNotFound(__G);
#else
                Info(slide, 1, ((char *)slide,
                  LoadFarString(FilenameNotMatched), G.pfnames[i]));
#endif
                if (error_in_archive <= PK_WARN)
                    error_in_archive = PK_FIND;   /* some files not found */
            }
        free((zvoid *)fn_matched);
    }
    if (xn_matched) {
        for (i = 0;  i < G.xfilespecs;  ++i)
            if (!xn_matched[i])
                Info(slide, 0x401, ((char *)slide,
                  LoadFarString(ExclFilenameNotMatched), G.pxnames[i]));
        free((zvoid *)xn_matched);
    }

/*---------------------------------------------------------------------------
    Double-check that we're back at the end-of-central-directory record, and
    print quick summary of results, if we were just testing the archive.  We
    send the summary to stdout so that people doing the testing in the back-
    ground and redirecting to a file can just do a "tail" on the output file.
  ---------------------------------------------------------------------------*/

#ifndef SFX
    if (readbuf(__G__ G.sig, 4) == 0)
        error_in_archive = PK_EOF;
    if (strncmp(G.sig, G.end_central_sig, 4)) {         /* just to make sure */
        Info(slide, 0x401, ((char *)slide, LoadFarString(EndSigMsg)));
        Info(slide, 0x401, ((char *)slide, LoadFarString(ReportMsg)));
        if (!error_in_archive)       /* don't overwrite stronger error */
            error_in_archive = PK_WARN;
    }
#endif /* !SFX */
    ++filnum;  /* initialized to -1, so now zero if no files found */
    if (G.tflag) {
        int num=filnum - num_bad_pwd;

        if (G.qflag < 2) {         /* GRR 930710:  was (G.qflag == 1) */
            if (error_in_archive)
                Info(slide, 0, ((char *)slide, LoadFarString(ErrorInArchive),
                  (error_in_archive == 1)? "warning-" : "", G.zipfn));
            else if (num == 0)
                Info(slide, 0, ((char *)slide, LoadFarString(ZeroFilesTested),
                  G.zipfn));
            else if (G.process_all_files && (num_skipped+num_bad_pwd == 0))
                Info(slide, 0, ((char *)slide, LoadFarString(NoErrInCompData),
                  G.zipfn));
            else
                Info(slide, 0, ((char *)slide, LoadFarString(NoErrInTestedFiles)
                  , G.zipfn, num, (num==1)? "":"s"));
            if (num_skipped > 0)
                Info(slide, 0, ((char *)slide, LoadFarString(FilesSkipped),
                  num_skipped, (num_skipped==1)? "":"s"));
#if CRYPT
            if (num_bad_pwd > 0)
                Info(slide, 0, ((char *)slide, LoadFarString(FilesSkipBadPasswd)
                  , num_bad_pwd, (num_bad_pwd==1)? "":"s"));
#endif /* CRYPT */
        } else if ((G.qflag == 0) && !error_in_archive && (num == 0))
            Info(slide, 0, ((char *)slide, LoadFarString(ZeroFilesTested),
              G.zipfn));
    }

    /* give warning if files not tested or extracted (first condition can still
     * happen if zipfile is empty and no files specified on command line) */

    if ((filnum == 0) && error_in_archive <= PK_WARN) {
        if (num_skipped > 0)
            error_in_archive = IZ_UNSUP; /* unsupport. compression/encryption */
        else
            error_in_archive = PK_FIND;  /* no files found at all */
    }
#if CRYPT
    else if ((filnum == num_bad_pwd) && error_in_archive <= PK_WARN)
        error_in_archive = IZ_BADPWD;    /* bad passwd => all files skipped */
#endif
    else if ((num_skipped > 0) && error_in_archive <= PK_WARN)
        error_in_archive = IZ_UNSUP;     /* was PK_WARN; Jean-loup complained */
#if CRYPT
    else if ((num_bad_pwd > 0) && !error_in_archive)
        error_in_archive = PK_WARN;
#endif

    return error_in_archive;

} /* end function extract_or_test_files() */





/***************************/
/*  Function store_info()  */
/***************************/

static int store_info(__G)   /* return 0 if skipping, 1 if OK */
    __GDEF
{
#ifdef SFX
#  define UNKN_COMPR \
   (G.crec.compression_method!=STORED && G.crec.compression_method!=DEFLATED)
#else
#  ifdef COPYRIGHT_CLEAN  /* no reduced files */
#    define UNKN_RED (G.crec.compression_method >= REDUCED1 && \
                      G.crec.compression_method <= REDUCED4)
#  else
#    define UNKN_RED  FALSE  /* reducing not unknown */
#  endif
#  ifdef LZW_CLEAN  /* no shrunk files */
#    define UNKN_SHR (G.crec.compression_method == SHRUNK)
#  else
#    define UNKN_SHR  FALSE  /* unshrinking not unknown */
#  endif
#  define UNKN_COMPR (UNKN_RED || UNKN_SHR || \
   G.crec.compression_method==TOKENIZED || G.crec.compression_method>DEFLATED)
#endif

/*---------------------------------------------------------------------------
    Check central directory info for version/compatibility requirements.
  ---------------------------------------------------------------------------*/

    G.pInfo->encrypted = G.crec.general_purpose_bit_flag & 1;   /* bit field */
    G.pInfo->ExtLocHdr = (G.crec.general_purpose_bit_flag & 8) == 8;  /* bit */
    G.pInfo->textfile = G.crec.internal_file_attributes & 1;    /* bit field */
    G.pInfo->crc = G.crec.crc32;
    G.pInfo->compr_size = G.crec.csize;

    switch (G.aflag) {
        case 0:
            G.pInfo->textmode = FALSE;   /* bit field */
            break;
        case 1:
            G.pInfo->textmode = G.pInfo->textfile;   /* auto-convert mode */
            break;
        default:  /* case 2: */
            G.pInfo->textmode = TRUE;
            break;
    }

    if (G.crec.version_needed_to_extract[1] == VMS_) {
        if (G.crec.version_needed_to_extract[0] > VMS_UNZIP_VERSION) {
            if (!((G.tflag && G.qflag) || (!G.tflag && !QCOND2)))
                Info(slide, 0x401, ((char *)slide, LoadFarString(VersionMsg),
                  FnFilter1(G.filename), "VMS",
                  G.crec.version_needed_to_extract[0] / 10,
                  G.crec.version_needed_to_extract[0] % 10,
                  VMS_UNZIP_VERSION / 10, VMS_UNZIP_VERSION % 10));
            return 0;
        }
#ifndef VMS   /* won't be able to use extra field, but still have data */
        else if (!G.tflag && !G.overwrite_all) {   /* if -o, extract anyway */
            Info(slide, 0x481, ((char *)slide, LoadFarString(VMSFormatQuery),
              FnFilter1(G.filename)));
            fgets(G.answerbuf, 9, stdin);
            if ((*G.answerbuf != 'y') && (*G.answerbuf != 'Y'))
                return 0;
        }
#endif /* !VMS */
    /* usual file type:  don't need VMS to extract */
    } else if (G.crec.version_needed_to_extract[0] > UNZIP_VERSION) {
        if (!((G.tflag && G.qflag) || (!G.tflag && !QCOND2)))
            Info(slide, 0x401, ((char *)slide, LoadFarString(VersionMsg),
              FnFilter1(G.filename), "PK",
              G.crec.version_needed_to_extract[0] / 10,
              G.crec.version_needed_to_extract[0] % 10,
              UNZIP_VERSION / 10, UNZIP_VERSION % 10));
        return 0;
    }

    if UNKN_COMPR {
        if (!((G.tflag && G.qflag) || (!G.tflag && !QCOND2)))
#ifndef SFX
            if (G.crec.compression_method < NUM_METHODS)
                Info(slide, 0x401, ((char *)slide, LoadFarString(ComprMsgName),
                  FnFilter1(G.filename),
                  LoadFarStringSmall(ComprNames[G.crec.compression_method])));
            else
#endif
                Info(slide, 0x401, ((char *)slide, LoadFarString(ComprMsgNum),
                  FnFilter1(G.filename),
                  G.crec.compression_method));
        return 0;
    }
#if (!CRYPT)
    if (G.pInfo->encrypted) {
        if (!((G.tflag && G.qflag) || (!G.tflag && !QCOND2)))
            Info(slide, 0x401, ((char *)slide, LoadFarString(SkipEncrypted),
              FnFilter1(G.filename)));
        return 0;
    }
#endif /* !CRYPT */

    /* map whatever file attributes we have into the local format */
    mapattr(__G);   /* GRR:  worry about return value later */

    G.pInfo->offset = (long)G.crec.relative_offset_local_header;
    return 1;

} /* end function store_info() */





/***************************************/
/*  Function extract_or_test_member()  */
/***************************************/

static int extract_or_test_member(__G)    /* return PK-type error code */
     __GDEF
{
    char *nul="[empty] ", *txt="[text]  ", *bin="[binary]";
#ifdef CMS_MVS
    char *ebc="[ebcdic]";
#endif
    register int b;
    int r, error=PK_COOL;
    ulg wsize;


/*---------------------------------------------------------------------------
    Initialize variables, buffers, etc.
  ---------------------------------------------------------------------------*/

    G.bits_left = 0;
    G.bitbuf = 0L;       /* unreduce and unshrink only */
    G.zipeof = 0;
    G.newfile = TRUE;
    G.crc32val = CRCVAL_INITIAL;

#ifdef SYMLINKS
    /* if file came from Unix and is a symbolic link and we are extracting
     * to disk, prepare to restore the link */
    if (S_ISLNK(G.pInfo->file_attr) &&
        (G.pInfo->hostnum == UNIX_ || G.pInfo->hostnum == ATARI_ ||
         G.pInfo->hostnum == BEOS_) &&
        !G.tflag && !G.cflag && (G.lrec.ucsize > 0))
        G.symlnk = TRUE;
    else
        G.symlnk = FALSE;
#endif /* SYMLINKS */

    if (G.tflag) {
        if (!G.qflag)
            Info(slide, 0, ((char *)slide, LoadFarString(ExtractMsg), "test",
              FnFilter1(G.filename), "", ""));
    } else {
#ifdef DLL
        if (G.cflag && !G.redirect_data)
#else
        if (G.cflag)
#endif
        {
#if (defined(OS2) && defined(__IBMC__) && (__IBMC__ >= 200))
            G.outfile = freopen("", "wb", stdout);   /* VAC++ ignores setmode */
#else
            G.outfile = stdout;
#endif
#ifdef DOS_FLX_H68_OS2_W32
#if (defined(__HIGHC__) && !defined(FLEXOS))
            setmode(G.outfile, _BINARY);
#else /* !(defined(__HIGHC__) && !defined(FLEXOS)) */
            setmode(fileno(G.outfile), O_BINARY);
#endif /* ?(defined(__HIGHC__) && !defined(FLEXOS)) */
#           define NEWLINE "\r\n"
#else /* !DOS_FLX_H68_OS2_W32 */
#           define NEWLINE "\n"
#endif /* ?DOS_FLX_H68_OS2_W32 */
#ifdef VMS
            if (open_outfile(__G))   /* VMS:  required even for stdout! */
                return PK_DISK;
#endif
        } else if (open_outfile(__G))
            return PK_DISK;
    }

/*---------------------------------------------------------------------------
    Unpack the file.
  ---------------------------------------------------------------------------*/

    defer_leftover_input(__G);    /* so NEXTBYTE bounds check will work */
    switch (G.lrec.compression_method) {
        case STORED:
            if (!G.tflag && QCOND2) {
#ifdef SYMLINKS
                if (G.symlnk)   /* can also be deflated, but rarer... */
                    Info(slide, 0, ((char *)slide, LoadFarString(ExtractMsg),
                      "link", FnFilter1(G.filename), "", ""));
                else
#endif /* SYMLINKS */
                Info(slide, 0, ((char *)slide, LoadFarString(ExtractMsg),
                  "extract", FnFilter1(G.filename),
                  (G.aflag != 1 /* && G.pInfo->textfile==G.pInfo->textmode */)?
                  "" : (G.lrec.ucsize == 0L? nul : (G.pInfo->textfile? txt :
                  bin)), G.cflag? NEWLINE : ""));
            }
#ifdef DLL
            if (G.redirect_data)
                wsize = G.redirect_size+1, G.outptr = G.redirect_buffer;
            else
#endif
                wsize = WSIZE, G.outptr = slide;
            G.outcnt = 0L;
            while ((b = NEXTBYTE) != EOF && !G.disk_full) {
                *G.outptr++ = (uch)b;
                if (++G.outcnt == wsize) {
                    flush(__G__ slide, G.outcnt, 0);
                    G.outptr = slide;
                    G.outcnt = 0L;
                }
            }
#ifdef DLL
            if (G.outcnt && !G.redirect_data)
#else
            if (G.outcnt)          /* flush final (partial) buffer */
#endif
                flush(__G__ slide, G.outcnt, 0);
            break;

#ifndef SFX
#ifndef LZW_CLEAN
        case SHRUNK:
            if (!G.tflag && QCOND2) {
                Info(slide, 0, ((char *)slide, LoadFarString(ExtractMsg),
                  LoadFarStringSmall(Unshrink), FnFilter1(G.filename),
                  (G.aflag != 1 /* && G.pInfo->textfile==G.pInfo->textmode */)?
                  "" : (G.pInfo->textfile? txt : bin), G.cflag? NEWLINE : ""));
            }
            if ((r = unshrink(__G)) != PK_COOL) {
                if ((G.tflag && G.qflag) || (!G.tflag && !QCOND2))
                    Info(slide, 0x401, ((char *)slide,
                      LoadFarStringSmall(ErrUnzipFile),
                      LoadFarString(NotEnoughMem),
                      LoadFarStringSmall2(Unshrink),
                      FnFilter1(G.filename)));
                else
                    Info(slide, 0x401, ((char *)slide,
                      LoadFarStringSmall(ErrUnzipNoFile),
                      LoadFarString(NotEnoughMem),
                      LoadFarStringSmall2(Unshrink)));
                error = r;
            }
            break;
#endif /* !LZW_CLEAN */

#ifndef COPYRIGHT_CLEAN
        case REDUCED1:
        case REDUCED2:
        case REDUCED3:
        case REDUCED4:
            if (!G.tflag && QCOND2) {
                Info(slide, 0, ((char *)slide, LoadFarString(ExtractMsg),
                  "unreduc", FnFilter1(G.filename),
                  (G.aflag != 1 /* && G.pInfo->textfile==G.pInfo->textmode */)?
                  "" : (G.pInfo->textfile? txt : bin), G.cflag? NEWLINE : ""));
            }
            unreduce(__G);
            break;
#endif /* !COPYRIGHT_CLEAN */

        case IMPLODED:
            if (!G.tflag && QCOND2) {
                Info(slide, 0, ((char *)slide, LoadFarString(ExtractMsg),
                  "explod", FnFilter1(G.filename),
                  (G.aflag != 1 /* && G.pInfo->textfile==G.pInfo->textmode */)?
                  "" : (G.pInfo->textfile? txt : bin), G.cflag? NEWLINE : ""));
            }
            if (((r = explode(__G)) != 0) && (r != 5)) { /* treat 5 specially */
                if ((G.tflag && G.qflag) || (!G.tflag && !QCOND2))
                    Info(slide, 0x401, ((char *)slide,
                      LoadFarStringSmall(ErrUnzipFile), r == 3?
                      LoadFarString(NotEnoughMem) :
                      LoadFarString(InvalidComprData),
                      LoadFarStringSmall2(Explode),
                      FnFilter1(G.filename)));
                else
                    Info(slide, 0x401, ((char *)slide,
                      LoadFarStringSmall(ErrUnzipNoFile), r == 3?
                      LoadFarString(NotEnoughMem) :
                      LoadFarString(InvalidComprData),
                      LoadFarStringSmall2(Explode)));
                error = (r == 3)? PK_MEM3 : PK_ERR;
            }
            if (r == 5) {
                int warning = ((ulg)G.used_csize <= G.lrec.csize);

                if ((G.tflag && G.qflag) || (!G.tflag && !QCOND2))
                    Info(slide, 0x401, ((char *)slide, LoadFarString(LengthMsg),
                      "", warning?  "warning" : "error", G.used_csize,
                      G.lrec.ucsize, warning?  "  " : "", G.lrec.csize,
                      " [", FnFilter1(G.filename), "]"));
                else
                    Info(slide, 0x401, ((char *)slide, LoadFarString(LengthMsg),
                      "\n", warning? "warning" : "error", G.used_csize,
                      G.lrec.ucsize, warning? "  ":"", G.lrec.csize,
                      "", "", "."));
                error = warning? PK_WARN : PK_ERR;
            }
            break;
#endif /* !SFX */

        case DEFLATED:
            if (!G.tflag && QCOND2) {
                Info(slide, 0, ((char *)slide, LoadFarString(ExtractMsg),
                  "inflat", FnFilter1(G.filename),
                  (G.aflag != 1 /* && G.pInfo->textfile==G.pInfo->textmode */)?
                  "" : (G.pInfo->textfile? txt : bin), G.cflag? NEWLINE : ""));
            }
#ifndef USE_ZLIB  /* zlib's function is called inflate(), too */
#  define UZinflate inflate
#endif
            if ((r = UZinflate(__G)) != 0) {
                if ((G.tflag && G.qflag) || (!G.tflag && !QCOND2))
                    Info(slide, 0x401, ((char *)slide,
                      LoadFarStringSmall(ErrUnzipFile), r == 3?
                      LoadFarString(NotEnoughMem) :
                      LoadFarString(InvalidComprData),
                      LoadFarStringSmall2(Inflate),
                      FnFilter1(G.filename)));
                else
                    Info(slide, 0x401, ((char *)slide,
                      LoadFarStringSmall(ErrUnzipNoFile), r == 3?
                      LoadFarString(NotEnoughMem) :
                      LoadFarString(InvalidComprData),
                      LoadFarStringSmall2(Inflate)));
                error = (r == 3)? PK_MEM3 : PK_ERR;
            }
            break;

        default:   /* should never get to this point */
            Info(slide, 0x401, ((char *)slide,
              LoadFarString(FileUnknownCompMethod), FnFilter1(G.filename)));
            /* close and delete file before return? */
            undefer_input(__G);
            return PK_WARN;

    } /* end switch (compression method) */

/*---------------------------------------------------------------------------
    Close the file and set its date and time (not necessarily in that order),
    and make sure the CRC checked out OK.  Logical-AND the CRC for 64-bit
    machines (redundant on 32-bit machines).
  ---------------------------------------------------------------------------*/

#ifdef VMS                  /* VMS:  required even for stdout! (final flush) */
    if (!G.tflag)           /* don't close NULL file */
        close_outfile(__G);
#else
#ifdef DLL
    if (!G.tflag && (!G.cflag || G.redirect_data))
        if (G.redirect_data)
            FINISH_REDIRECT();
        else
            close_outfile(__G);
#else
    if (!G.tflag && !G.cflag)   /* don't close NULL file or stdout */
        close_outfile(__G);
#endif
#endif /* VMS */

            /* GRR: CONVERT close_outfile() TO NON-VOID:  CHECK FOR ERRORS! */


    if (G.disk_full) {            /* set by flush() */
        if (G.disk_full > 1) {
#if (defined(DELETE_IF_FULL) && defined(HAVE_UNLINK))
            /* delete the incomplete file if we can */
            if (unlink(G.filename) != 0)
                Trace((stderr, "extract.c:  could not delete %s\n",
                  FnFilter1(G.filename)));
#else
            /* warn user about the incomplete file */
            Info(slide, 0x421, ((char *)slide, LoadFarString(FileTruncated),
              FnFilter1(G.filename)));
#endif
            error = PK_DISK;
        } else {
            error = PK_WARN;
        }
    }

    if (error > PK_WARN) {/* don't print redundant CRC error if error already */
        undefer_input(__G);
        return error;
    }
    if (G.crc32val != G.lrec.crc32) {
        /* if quiet enough, we haven't output the filename yet:  do it */
        if ((G.tflag && G.qflag) || (!G.tflag && !QCOND2))
            Info(slide, 0x401, ((char *)slide, "%-22s ",
              FnFilter1(G.filename)));
        Info(slide, 0x401, ((char *)slide, LoadFarString(BadCRC), G.crc32val,
          G.lrec.crc32));
#if CRYPT
        if (G.pInfo->encrypted)
            Info(slide, 0x401, ((char *)slide, LoadFarString(MaybeBadPasswd)));
#endif
        error = PK_ERR;
    } else if (G.tflag) {
#ifndef SFX
        if (G.extra_field) {
            if ((r = TestExtraField(__G__ G.extra_field,
                                    G.lrec.extra_field_length)) > error)
                error = r;
        } else
#endif /* !SFX */
        if (!G.qflag)
            Info(slide, 0, ((char *)slide, " OK\n"));
    } else {
        if (QCOND2 && !error)   /* GRR:  is stdout reset to text mode yet? */
            Info(slide, 0, ((char *)slide, "\n"));
    }

    undefer_input(__G);
    return error;

} /* end function extract_or_test_member() */





#ifndef SFX

/*******************************/
/*  Function TestExtraField()  */
/*******************************/

static int TestExtraField(__G__ ef, ef_len)
    __GDEF
    uch *ef;
    unsigned ef_len;
{
    ush ebID;
    unsigned ebLen;
    int r;

    /* we know the regular compressed file data tested out OK, or else we
     * wouldn't be here ==> print filename if any extra-field errors found
     */
    while (ef_len >= EB_HEADSIZE) {
        ebID = makeword(ef);
        ebLen = (unsigned)makeword(ef+EB_LEN);

        if (ebLen > (ef_len - EB_HEADSIZE)) {
           /* Discovered some extra field inconsistency! */
            if (G.qflag)
                Info(slide, 1, ((char *)slide, "%-22s ",
                  FnFilter1(G.filename)));
            Info(slide, 1, ((char *)slide, LoadFarString(InconsistEFlength),
              ebLen, (ef_len - EB_HEADSIZE)));
            return PK_ERR;
        }

        switch (ebID) {
            case EF_OS2:
            case EF_ACL:
                if ((r = test_compr_eb(__G__ ef, ebLen, EB_OS2_CHEAD, NULL))
                    != PK_OK) {
                    if (G.qflag)
                        Info(slide, 1, ((char *)slide, "%-22s ",
                          FnFilter1(G.filename)));
                    switch (r) {
                        case IZ_EF_TRUNC:
                            Info(slide, 1, ((char *)slide,
                              LoadFarString(TruncEAs),
                              ebLen-(EB_OS2_CHEAD+EB_CMPRHEADLEN), "\n"));
                            break;
                        case PK_ERR:
                            Info(slide, 1, ((char *)slide,
                              LoadFarString(InvalidComprDataEAs)));
                            break;
                        case PK_MEM3:
                        case PK_MEM4:
                            Info(slide, 1, ((char *)slide,
                              LoadFarString(NotEnoughMemEAs)));
                            break;
                        default:
                            if ((r & 0xff) != PK_ERR)
                                Info(slide, 1, ((char *)slide,
                                  LoadFarString(UnknErrorEAs)));
                            else {
                                ush m = (ush)(r >> 8);
                                if (m == DEFLATED)            /* GRR KLUDGE! */
                                    Info(slide, 1, ((char *)slide,
                                      LoadFarString(BadCRC_EAs)));
                                else
                                    Info(slide, 1, ((char *)slide,
                                      LoadFarString(UnknComprMethodEAs), m));
                            }
                            break;
                    }
                    return r;
                }
                break;

            case EF_NTSD:
                Trace((stderr, "ebID: %i / ebLen: %u\n", ebID, ebLen));
                r = ebLen < EB_NTSD_L_LEN ? IZ_EF_TRUNC :
                    ((ef[EB_HEADSIZE+EB_NTSD_VERSION] > EB_NTSD_MAX_VER) ?
                     (PK_WARN | 0x4000) :
                     test_compr_eb(__G__ ef, ebLen, EB_NTSD_L_LEN, TEST_NTSD));
                if (r != PK_OK) {
                    if (G.qflag)
                        Info(slide, 1, ((char *)slide, "%-22s ",
                          FnFilter1(G.filename)));
                    switch (r) {
                        case IZ_EF_TRUNC:
                            Info(slide, 1, ((char *)slide,
                              LoadFarString(TruncNTSD),
                              ebLen-(EB_NTSD_L_LEN+EB_CMPRHEADLEN), "\n"));
                            break;
#if (defined(WIN32) && defined(NTSD_EAS))
                        case PK_WARN:
                            Info(slide, 1, ((char *)slide,
                              LoadFarString(InvalidSecurityEAs)));
                            break;
#endif
                        case PK_ERR:
                            Info(slide, 1, ((char *)slide,
                              LoadFarString(InvalidComprDataEAs)));
                            break;
                        case PK_MEM3:
                        case PK_MEM4:
                            Info(slide, 1, ((char *)slide,
                              LoadFarString(NotEnoughMemEAs)));
                            break;
                        case (PK_WARN | 0x4000):
                            Info(slide, 1, ((char *)slide,
                              LoadFarString(UnsuppNTSDVersEAs),
                              (int)ef[EB_HEADSIZE+EB_NTSD_VERSION]));
                            r = PK_WARN;
                            break;
                        default:
                            if ((r & 0xff) != PK_ERR)
                                Info(slide, 1, ((char *)slide,
                                  LoadFarString(UnknErrorEAs)));
                            else {
                                ush m = (ush)(r >> 8);
                                if (m == DEFLATED)            /* GRR KLUDGE! */
                                    Info(slide, 1, ((char *)slide,
                                      LoadFarString(BadCRC_EAs)));
                                else
                                    Info(slide, 1, ((char *)slide,
                                      LoadFarString(UnknComprMethodEAs), m));
                            }
                            break;
                    }
                    return r;
                }
                break;
            case EF_PKVMS:
            case EF_ASIUNIX:
            case EF_IZVMS:
            case EF_IZUNIX:
            case EF_VMCMS:
            case EF_MVS:
            case EF_SPARK:
            case EF_AV:
            default:
                break;
        }
        ef_len -= (ebLen + EB_HEADSIZE);
        ef += (ebLen + EB_HEADSIZE);
    }

    if (!G.qflag)
        Info(slide, 0, ((char *)slide, " OK\n"));

    return PK_COOL;

} /* end function TestExtraField() */





/******************************/
/*  Function test_compr_eb()  */
/******************************/

static int test_compr_eb(__G__ eb, eb_size, compr_offset, test_uc_ebdata)
    __GDEF
    uch *eb;
    unsigned eb_size;
    unsigned compr_offset;
    int (*test_uc_ebdata) OF((__GPRO__ uch *eb, unsigned eb_size,
                              uch *eb_ucptr, ulg eb_ucsize));
{
    ulg eb_ucsize;
    uch *eb_ucptr;
    int r;

    if ((eb_size < (EB_UCSIZE_P + 4)) ||
        ((eb_ucsize = makelong(eb+(EB_HEADSIZE+EB_UCSIZE_P))) > 0L &&
         eb_size <= (compr_offset + EB_CMPRHEADLEN)))
        return IZ_EF_TRUNC;               /* no compressed data! */

    if ((eb_ucptr = (uch *)malloc((extent)eb_ucsize)) == (uch *)NULL)
        return PK_MEM4;

    r = memextract(__G__ eb_ucptr, eb_ucsize,
                   eb + (EB_HEADSIZE + compr_offset),
                   (ulg)(eb_size - compr_offset));

    if (r == PK_OK && test_uc_ebdata != NULL)
        r = (*test_uc_ebdata)(__G__ eb, eb_size, eb_ucptr, eb_ucsize);

    free(eb_ucptr);
    return r;

} /* end function test_compr_eb() */

#endif /* !SFX */





/***************************/
/*  Function memextract()  */
/***************************/

int memextract(__G__ tgt, tgtsize, src, srcsize)  /* extract compressed */
    __GDEF                                        /*  extra field block; */
    uch *tgt, *src;                               /*  return PK-type error */
    ulg tgtsize, srcsize;                         /*  level */
{
    uch *old_inptr=G.inptr;
    int  old_incnt=G.incnt, r, error=PK_OK;
    ush  method;
    ulg  extra_field_crc;


    method = makeword(src);
    extra_field_crc = makelong(src+2);

    /* compressed extra field exists completely in memory at this location: */
    G.inptr = src + 2 + 4;      /* method and extra_field_crc */
    G.incnt = (int)(G.csize = (long)(srcsize - (2 + 4)));
    G.mem_mode = TRUE;
    G.outbufptr = tgt;
    G.outsize = tgtsize;

    switch (method) {
        case STORED:
            memcpy((char *)tgt, (char *)G.inptr, (extent)G.incnt);
            G.outcnt = G.csize;   /* for CRC calculation */
            break;
        case DEFLATED:
            G.outcnt = 0L;
            if ((r = UZinflate(__G)) != 0) {
                if (!G.tflag)
                    Info(slide, 0x401, ((char *)slide,
                      LoadFarStringSmall(ErrUnzipNoFile), r == 3?
                      LoadFarString(NotEnoughMem) :
                      LoadFarString(InvalidComprData),
                      LoadFarStringSmall2(Inflate)));
                error = (r == 3)? PK_MEM3 : PK_ERR;
            }
            if (G.outcnt == 0L)   /* inflate's final FLUSH sets outcnt */
                break;
            break;
        default:
            if (G.tflag)
                error = PK_ERR | ((int)method << 8);
            else {
                Info(slide, 0x401, ((char *)slide,
                  LoadFarString(UnsupportedExtraField), method));
                error = PK_ERR;  /* GRR:  should be passed on up via SetEAs() */
            }
            break;
    }

    G.inptr = old_inptr;
    G.incnt = old_incnt;
    G.mem_mode = FALSE;

    if (!error) {
        register ulg crcval = crc32(CRCVAL_INITIAL, tgt, (extent)G.outcnt);

        if (crcval != extra_field_crc) {
            if (G.tflag)
                error = PK_ERR | (DEFLATED << 8);  /* kludge for now */
            else {
                Info(slide, 0x401, ((char *)slide,
                  LoadFarString(BadExtraFieldCRC), G.zipfn, crcval,
                  extra_field_crc));
                error = PK_ERR;
            }
        }
    }
    return error;

} /* end function memextract() */





/*************************/
/*  Function memflush()  */
/*************************/

int memflush(__G__ rawbuf, size)
    __GDEF
    uch *rawbuf;
    ulg size;
{
    if (size > G.outsize)
        return 50;   /* more data than output buffer can hold */

    memcpy((char *)G.outbufptr, (char *)rawbuf, (extent)size);
    G.outbufptr += (unsigned int)size;
    G.outsize -= size;
    G.outcnt += size;

    return 0;

} /* end function memflush() */





/*************************/
/*  Function fnfilter()  */        /* here instead of in list.c for SFX */
/*************************/

char *fnfilter(raw, space)         /* convert name to safely printable form */
    char *raw;
    uch *space;
{
#ifndef NATIVE   /* ASCII:  filter ANSI escape codes, etc. */
    uch *r=(uch *)raw, *s=space;

    while (*r) {
#ifdef QDOS
        if (qlflag & 2) {
            if (*r == '/' || *r == '.') {
                ++r;
                *s++ = '_';
                continue;
            }
        } else
#endif
        if (*r < 32)
            *s++ = '^', *s++ = (uch)(64 + *r++);
        else
            *s++ = *r++;
    }
    *s = 0;

#ifdef WINDLL
    INTERN_TO_ISO((char *)space, (char *)space);  /* translate to ANSI */
#else
#ifdef WIN32
    if ( !IsWinNT() ) {
        /* Win95 console uses OEM character coding */
        INTERN_TO_OEM((char *)space, (char *)space);
    } else {
        /* >>>>> this guess has to be checked !!!! <<<<<< */
        /* WinNT console uses ISO character coding */
        INTERN_TO_ISO((char *)space, (char *)space);
    }
#endif /* WIN32 */
#endif /* ?WINDLL */

    return (char *)space;

#else /* NATIVE:  EBCDIC or whatever */
    return raw;
#endif

} /* end function fnfilter() */





#ifdef UNIX   /* must sort saved directories so can set perms from bottom up */

/************************/
/*  Function dircomp()  */
/************************/

static int dircomp(a, b)   /* used by qsort(); swiped from Zip */
    ZCONST zvoid *a, *b;
{
    /* order is significant:  this sorts in reverse order (deepest first) */
    return strcmp((*(dirtime **)b)->fn, (*(dirtime **)a)->fn);
 /* return namecmp((*(dirtime **)b)->fn, (*(dirtime **)a)->fn); */
}



#if 0   /* not used in Unix, but maybe for future OSes? */

/************************/
/*  Function namecmp()  */
/************************/

static int namecmp(s1, s2)   /* [not] used by dircomp(); swiped from Zip */
    ZCONST char *s1, *s2;
{
    int d;

    for (;;) {
        d = (int)(uch)case_map(*s1)
          - (int)(uch)case_map(*s2);

        if (d || *s1 == 0 || *s2 == 0)
            return d;

        s1++;
        s2++;
    }
}

#endif /* 0 */
#endif /* UNIX */
