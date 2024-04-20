#ifdef ROM
#define malloc rom_malloc
#define free rom_free
#define memset rom_memset
#define memcpy rom_memcpy
#endif

/*++

Copyright (c) 1992

Module Name:

    decompress.c

Abstract:

    compress image file headers from file and leaves
    raw image to raw boot on an alpha box.

Author:

    Wim Colgate 02-June-1992

--*/


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



/* globals */

#ifdef ROM

/* nothing */

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

/* ImageBase = the physical address of the base of the compressed image */
 
INT
decompress( ULONG ImageBase )
{
    ULONG Method;
    PULONG Signature;
    UINT Found = FALSE;

    compressed = (PUCHAR)(ImageBase);
/* old code here */
/*    decompressed = (PUCHAR)(BASE_OF_DECOMPRESSED_IMAGE); */

    /* Start at the base of our image and walk through looking for */
    /* the Signature "WimC". The assumption is that the compressed image */
    /* gets stored on a longword boundary. */
    

    for (Signature = (PULONG)compressed; 
         Signature < (PULONG) (compressed + _1MEG); 
         Signature++) {
        if ( *Signature == 0x436D6957 ) {
            Found = TRUE;
            break;
        }
    }

    if (Found == FALSE) {
        return;
    }


    /* following the signature is the size of the compressed */
    /* image, then the beginning of the compressed image. */
    

    Signature++;
    compressedSize = *Signature;
/* we have more information than the original */
/*    Signature++; */

    Signature += 3;
/* new code here */
    decompressed = (PUCHAR)*Signature++;

    compressed = (PUCHAR)Signature;

    inptr = compressed;
    outptr = decompressed;

    inflate();
    return((int)decompressed);
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

    if (strncmp(stamp, "WimC", 4) != 0) {
        fprintf(stderr,"Missing Magic stamp at file offset 0\n");
        return FALSE;
    }
    fread((UCHAR *)&compressedSize, sizeof(ULONG), 1, in);

    
    /* allocate more than we need. */
    

    decompressedSize = compressedSize * 4;

    inptr = compressed = (PUCHAR) malloc(compressedSize);
/* we have more information than the original */
    inptr += 12;
/* */
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
