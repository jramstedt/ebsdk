#include <stdio.h>
#include <stdlib.h>


/* The purpose of this program is to make a gzip-compressed ROM image component
 * that can be unzipped by a header that is capable of a bit more than the
 * standard micropal. */

/* As an exercise, I'm writing this in the good old-fashioned way and making
 * no platform-dependent calls */

static char *progname;

static void myexit(const char *msg)
{
        if (msg)        fprintf(stderr, "%s: %s", progname, msg);
        fprintf(stderr, "usage: %s <input file> <output file>\n", progname);
        exit(-1);
}


#define VECSZ 8

int main( int argc, char *argv[] )
{
    char *ifname, *ofname;
    FILE *ifd, *ofd;
    unsigned char datain[VECSZ];
    unsigned nbytes;
    unsigned i;

    progname = argv[0];
    argv++, argc--;			/* skip on past program name */

    /* currently, we require input and output to be files - no stdio */
    if ( argc != 2 ) 	myexit("Please specify the correct number of args\n");
    ifname = argv[0], ofname = argv[1];

    ifd = fopen( ifname, "r" );
    if ( ifd == NULL )	myexit("Error opening input file\n" );

    ofd = fopen( ofname, "w" );
    if ( ofd == NULL )	myexit("Error opening output file\n" );


    /* setup */
    fprintf( ofd, "/* This file is auto-generated, don't bother editing */\n"
		  "static unsigned char blob[] = { ");


    /* main loop: read in a row at a time, create a row of array data */
    do {
	nbytes = fread( datain, sizeof( unsigned char), VECSZ, ifd );

	if ( nbytes == 0 )	break;		/* no more to print */

	fprintf(ofd, "\n\t");
	for ( i=0; i < nbytes; i++ )
	    fprintf( ofd, "0x%02X, ", datain[i] );
    } while ( nbytes == VECSZ );		/* ie a full line read */

    fprintf( ofd, "\n};\n");

    fclose( ifd ), fclose( ofd );

    return 0;
}
