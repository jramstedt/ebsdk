/*****************************************************************************

Copyright � 1994, Digital Equipment Corporation, Maynard, Massachusetts. 

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, provided  
that the copyright notice and this permission notice appear in all copies  
of software and supporting documentation, and that the name of Digital not  
be used in advertising or publicity pertaining to distribution of the software 
without specific, written prior permission. Digital grants this permission 
provided that you prominently mark, as not part of the original, any 
modifications made to this software or documentation.

Digital Equipment Corporation disclaims all warranties and/or guarantees  
with regard to this software, including all implied warranties of fitness for 
a particular purpose and merchantability, and makes no representations 
regarding the use of, or the results of the use of, the software and 
documentation in terms of correctness, accuracy, reliability, currentness or
otherwise; and you rely on the software, documentation and results solely at 
your own risk. 

******************************************************************************/
/*
**
** FACILITY:	
**
**	EBxx Software Tools - makerom
** 
** FUNCTIONAL DESCRIPTION:
** 
**     the front end of the decompressor for both stand-alone and
**     running from the OS prompt, this calls inflate which does
**     the actual decompression.
**     
** 
** CREATION-DATE: 01-Feb-1995
** 
** MODIFIED BY: 
** 
*/
#ifndef lint
__attribute__((used)) static const char *RCSid = "$Id: decomp.c,v 1.1.1.1 1998/12/29 21:36:25 paradis Exp $";
#endif

/*
 * $Log: decomp.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:25  paradis
 * Initial CVS checkin
 *
 * Revision 1.8  1996/12/22  12:27:37  fdh
 * Modified to use a different header signature.  The old
 * heaer signature is also present for backward compatibility
 * with the old header.  The new signature indicates the
 * presence of additional header fields.
 *
 * Revision 1.7  1996/07/18  18:14:32  fdh
 * Modified to not use _end label for _WIN32 builds.
 *
 * Revision 1.6  1996/06/13  03:13:40  fdh
 * Modified to use the linker defined variable _end instead
 * of the harwired precessor definition.
 *
 * Revision 1.5  1995/12/11  17:52:46  fdh
 * Added a missing return value.
 *
 * Revision 1.4  1995/12/07  07:47:53  fdh
 * Moved the header definition to header.h.
 * Modified to use the struct header to specify the decompressed
 * image base address.
 * Modified decompress() to return the pointer to the base of the
 * decompressed image.
 *
 * Revision 1.3  1995/05/19  20:26:39  fdh
 * Made uncompressed image entry point a compile-
 * time parameter throught the DECOMP_DESTINATION macro.
 *
 * Revision 1.2  1995/02/01  20:16:20  curewitz
 * initial version
 *
 */

#ifdef ROM

/* nothing */

#else
#include <stdio.h>
#include <stdlib.h>
#include <stat.h>
#include <string.h>

void exit(int code);
#endif

#include "decomp.h"
#include "header.h"

/* globals */

#ifdef ROM

#ifndef _WIN32
extern unsigned long _end;
extern unsigned long _edata;
extern unsigned long _fbss;
#define _END _end
#define _EDATA _fbss
#endif

#else
CHAR outfile[256];
CHAR infile[256];
CHAR buffer[8192];
FILE *objIn ;
FILE *objOut ;
#endif

INT verbose = FALSE ;

/* Initialization FORCES variables into .data section. */
/* This is important, because .bss normally established by the OS, */
/* and for the firmware boot loading, it is not. */

PUCHAR compressed = 0;
LONG compressedSize = 0;

PUCHAR decompressed = 0;
LONG decompressedSize = 0;

PUCHAR inptr = 0;
PUCHAR outptr = 0;

INT bits_left = 0;

#ifdef ROM

/* nothing */

#else

void help()
{
    printf("decompress [-v] [-o <filename>] infile\n");
    printf("-v              verbose mode\n");
    printf("-o <filename>   create this as output file\n");
    printf("\nIf no output filename is given, input file is rewritten.\n");
}

#endif


#ifdef ROM

/* DECOMP_COMPRESSED = the physical address of the base of the compressed image */
 
unsigned int
decompress( void )
{

    PULONG Signature;
    UINT Found = FALSE;
    struct header *hp;

#ifdef _WIN32
    compressed = (PUCHAR)(DECOMP_COMPRESSED);
#else
    compressed = (PUCHAR)&_END;
#endif /* _WIN32 */

    /* Start at the base of our image and walk through looking for */
    /* the Signature "WimC". The assumption is that the compressed image */
    /* gets stored on a longword boundary. */
    

    for (Signature = (PULONG)compressed; 
         Signature < (PULONG) (compressed + _1MEG); 
         Signature++) {
      if ((*Signature & 0xFFFFFFFF) == SIGNATURE_STAMP_REV2) {
	Found = TRUE;
	break;
      }
    }

    if (Found == FALSE) {
        return 0;
    }

    /*
    ** Work out where the header stucture is relative to the signature.
    ** A new signature was defined to identify the additional fields.
    ** This is because the additional fields beyond the signature and
    ** size needed to be placed before the previously existing fields
    ** for backward compatibility.
    */

    hp = (struct header *) Signature;
    compressedSize = hp->csize;
    compressed = (PUCHAR) ((size_t)Signature+sizeof(struct header));
    decompressed = (PUCHAR) hp->target;

    inptr = compressed;
    outptr = decompressed;

    inflate();

    return hp->target;
}

#else


INT
decompress (FILE *in, FILE *out)
{
    struct stat statbuf;
    ULONG bytesRead;
    ULONG bytesWritten;
    ULONG bytesToWrite;
    ULONG Method;
    ULONG filesize;
    PUCHAR tmp;
    CHAR stamp[4];

    if (fstat(fileno(in), &statbuf)) {
        fprintf(stderr,"Couldn't stat input file\n");
        return FALSE;
    }

    /* remove method and length longword */

    filesize = statbuf.st_size - sizeof(ULONG)*2; 

    
    /* read in options method and size of compressed file */
    

    fseek(in, 0, SEEK_SET);
    fread(stamp, sizeof(ULONG), 1, in);

    if ((*Signature & 0xFFFFFFFF) != SIGNATURE_STAMP_REV2) {
        fprintf(stderr,"Missing Magic stamp at file offset 0\n");
        return FALSE;
    }
    fread((UCHAR *)&compressedSize, sizeof(ULONG), 1, in);

    
    /* allocate more than we need. */
    

    decompressedSize = compressedSize * 4;

    inptr = compressed = (PUCHAR) malloc(compressedSize);
    tmp = (PUCHAR) malloc(decompressedSize + 512);
    memset(tmp, 0, decompressedSize);
    outptr = decompressed  = (tmp+compressedSize);

    if (compressed == NULL || decompressed == NULL) {
        fprintf(stderr,"Couldn't allocate buffers for decompression\n");
        return FALSE;
    }

    bytesRead = fread(compressed, 1, compressedSize, in);

    if (verbose) {
        fprintf(stderr,"Size of compressed file %x\n", compressedSize);
        fprintf(stderr,"Bytes read %x\n", bytesRead);
    }

    bytesRead = fread(stamp, 1, compressedSize, in);
    bytesRead = fread(stamp, 1, compressedSize, in);
    bytesRead = fread(stamp, 1, compressedSize, in);

    inflate();

    
    /* decompressedSize gets changed by unImplod */
    

    bytesToWrite = (ULONG)(outptr - decompressed);

    bytesWritten = fwrite(decompressed, bytesToWrite, 1,out);

    if (verbose) {
        fprintf(stderr,"Bytes written %x\n", bytesWritten);
    }


    return TRUE;
}


INT main ( int argc, 
            char *argv[] )
{

    INT i;
    INT j;

    
    /* No args? then print out the version number only */
    

    if (argc == 1) {
        printf("decompress Version 2.0\n");
        help();
        exit(0);
    }

    outfile[0] = '\0' ;
    infile[0] = '\0' ;

    
    /* Loop through the arguments; */
    

    for( i=1; i < argc ; i++ ) {
        if (strcmp(argv[i], "-?") == 0 ||
            strcmp(argv[i], "-h") == 0) {
            help() ;
            exit(1);
        }
        else
        if (strcmp(argv[i], "-v") == 0)
            verbose = TRUE ;
        else
        if (strcmp(argv[i], "-o") == 0) {
            if (i < argc+1) {
                strcpy( outfile, argv[i+1] );
#ifdef _MSDOS
                if (!(objOut = fopen(outfile, "wb")))  {
#else
                if (!(objOut = fopen(outfile, "wb", "rfm=fix", "mrs=512")))  {
#endif
                    printf("Can't open %s, not converted\n", outfile);
                    return;
                }
                i++ ;
            }
            else {
                printf("Need a file name after -o.\n");
                return;
            }
        }
        else {            /* must be input file */
            strcpy( infile, argv[i] );
            if (!(objIn = fopen(infile, "rb")))  {
                printf("Can't open %s, not converted\n", infile);
                return;
            }

            if (outfile[0] == '\0') {
                strcpy( outfile, "compress" );
#ifdef	_MSDOS
                if (!(objOut = fopen(outfile, "wb")))  {
#else
                if (!(objOut = fopen(outfile, "wb", "rfm=fix", "mrs=512")))  {
#endif
                    printf("Can't open %s, not converted\n", outfile);
                    return;
                }
            }

        }
    }

    
    /* decompress file */
    

    if (decompress( objIn, objOut ) != TRUE) {
        fclose(objIn);
        fclose(objOut);
        remove(outfile);
        printf("Couldn't compress file (%s)\n", infile ) ;
        return ;
    }

    fclose(objIn);
    fclose(objOut);

    
    /* If we were successful, copy the file back onto itself if */
    /* temporary file */
    

    if (strcmp(outfile,"compress") == 0) {
        if (remove(infile)) {            /* first remove the original */
            printf("Can't remove %s, not converted\n", infile);
            remove(outfile);
            return ;
        }
        if (rename(outfile, infile)) {   /* Can't rename? then copy */
            objIn = fopen(outfile, "rb");
#ifdef	_MSDOS
            objOut = fopen(infile, "wb");
#else
            objOut = fopen(infile, "wb", "rfm=fix", "mrs=512");
#endif
            if (!objIn || !objOut)  {
                printf("Can't rename %s to %s\n", outfile, infile);
                fclose(objIn);
                fclose(objOut);
                return ;
            }
            do {                         /* buffer at a time copy. */
                 j = fread(buffer, 1, sizeof(buffer), objIn);
                 if (j) {
                     fwrite(buffer, j, 1, objOut);
                 }
            } while (j);
            fclose(objIn);
            fclose(objOut);
            remove(outfile);
        }
    }

    return 0 ;
}
#endif
