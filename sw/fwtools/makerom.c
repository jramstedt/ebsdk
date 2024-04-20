/*---------------------------------------------------------------------
 *        [ Copyright (c) 2001 Alpha Processor Inc.] - Unpublished Work
 *          All rights reserved
 * 
 *    This file contains source code written by Alpha Processor, Inc.
 *    It may not be used without express written permission. The
 *    expression of the information contained herein is protected under
 *    federal copyright laws as an unpublished work and all copying
 *    without permission is prohibited and may be subject to criminal
 *    and civil penalties. Alpha Processor, Inc.  assumes no
 *    responsibility for errors, omissions, or damages caused by the use
 *    of these programs or from use of the information contained herein.
 *  
 *-------------------------------------------------------------------*/
/* New version of the "makerom" utility which is hopefully more succinct and
 * simply-coded.  Begun 29 January 2001 by Stig Telfer, API NetWorks Inc. */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "romhead.h"


#define PROGNAME "makerom"
#define BANNER "API NetWorks " PROGNAME " utility, build date " __DATE__ "\n"


/*------------------------------------------------------------------------*/
/* Private data and functions */

static void output( FILE *dest, const char *fmt, va_list ap )
{
    fprintf( dest, PROGNAME ": " );		/* prefix program name */
    vfprintf( dest, fmt, ap );
}

static void error( const char *fmt, ... )
{
    va_list ap;
    va_start( ap, fmt );
    output( stderr, fmt, ap );
    exit( -1 );
    va_end( ap );
}

static unsigned char verbosity = 0;

static void info( const char *fmt, ... )
{
    va_list ap;

    if ( !verbosity )
	return;

    va_start( ap, fmt );
    output( stdout, fmt, ap );
    va_end( ap );
}


static void header_info_dump( romheader_t *H )
{
    info( "ROM header info:\n"
	    "\tHeader type: %d\n"
	    "\tFirmware ID: %d\n"
	    "\tImage size: %dKB\n"
	    "\tImage load address: 0x%lX\n"
	    "\tSuggested ROM offset: 0x%lX\n"
	    "\tHeader checksum: 0x%04X\n"
	    "\tBody checksum: 0x%04X\n",
	    ROMH_VERSION( H ),
	    H->romh.V1.fw_id,
	    H->romh.V0.size >> 10,
	    (unsigned long)H->romh.V0.destination.high << 32 | 
	    (unsigned long)H->romh.V0.destination.low,
	    H->romh.V2.rom_offset,
	    H->romh.hchecksum,
	    H->romh.V0.checksum );
}


/* Compute ROM checksums for header and image body */
unsigned short compute_checksum( unsigned char *img, size_t len )
{
    int i;
    unsigned char r;
    unsigned short csum;

    for (i=0, csum=0; i < len; i++)
    {
	r = img[i];

	/* rotate */
	if (csum & 1)
	    csum = (csum >> 1) + 0x8000;
	else    
	    csum = csum >> 1;
		                    
	csum += r;   
	csum &= 0xFFFF;
    }

    return csum;
}



/* Lookup a string to return the firmware ID type, or FW_UNKNOWN if no match */
#define MAX_KEYS 4

typedef struct { FWID I; char *key[ MAX_KEYS + 1 ]; } fwdata_t;


static const fwdata_t fw_array[] = {
    { FW_DBM, { "0", "DBM", NULL } },
    { FW_WNT, { "1", "ARC", "AlphaBIOS", NULL } },
    { FW_SRM, { "2", "SRM", NULL } },
    { FW_FSB, { "6", "FSB", "Failsafe", NULL } },
    { FW_MILO, { "7", "MILO", "MiniLoader", NULL } },
    { FW_DIAG, { "9", "Diags", "ADE", "Diagnostics", NULL } },
    { FW_SR, { "10", "SROM", "SLRP", "ResetPAL", NULL } },
    { FW_LINUX, { "11", "KROM", "Linux", NULL } },
    { FW_CBOX, { "12", "CBOX", NULL } },
    { FW_OSFPAL, { "13", "OSFPAL", NULL } },
    { FW_INITRD, { "20", "InitRD", NULL } },
    { FW_UNKNOWN, NULL }
};

static FWID id_lookup( const char *txt )
{
    fwdata_t const *F = fw_array;		/* tricky usage of const here */
    char *const *key;

    while( F->I != FW_UNKNOWN )
    {
	key = F->key;

	while( *key != NULL )
	{
	    if ( strcmp( *key, txt ) == 0 )		/* match? */
		return F->I;

	    key++;
	}
	F++;
    }

    /* no match */
    return FW_UNKNOWN;
}


static void usage( void )
{
    verbosity = 1;
    info( "Accepted arguments:\n"
        "\t-l <hex-address>\tLoad address in memory\n"
        "\t-r <hex-address>\tOffset into ROM\n"
        "\t-x <hex-value>\t\tHex value for 8-byte firmware rev field\n"
        "\t-s <string>\t\tString value for 8-byte firmware rev field\n"
        "\t-p <part-data>\t\tString in API part format\n"
        "\t\t\t\t[Major.Minor-Miniscule Category-Index-Revision]\n"
        "\t-i <FW ID>\t\tFirmware ID number or string\n"
        "\t-d <hdr rev>\t\tHeader revision (2 or 3 accepted)\n" );

    info( "Example usage:\n"
       "\t" PROGNAME " -l 8000 -p \"1.45-00 61-0039-3A\" -o diags.rom diags.img\n" );

    exit( 0 );
}


static romheader_t *hdr;

static unsigned long LoadAddress, RomOffset;
static char *outputfile = NULL, *inputfile = NULL;

/*------------------------------------------------------------------------*/
/* main function */

int main( int argc, char *argv[] )
{
    char c;
    int rval;
    unsigned major, minor, rev, api_category, api_index, api_rev;
    char api_letter;
    api_fwid_t *A;
    FWID id;
    int inputfd, outputfd;
    struct stat statdata;
    off_t inputsize;
    char *inputbuf;
    size_t hdr_bytes, body_bytes;
    int hdr_rev = 2;

    info( BANNER );

    hdr = calloc( 1, sizeof( romheader_t ) );		/* alloc and clear */
    if( hdr == NULL )
    {
	error( "alloc failed!\n" );
    }

    /* Fill in the basic details */
    hdr->romh.V0.signature = ROM_H_SIGNATURE;
    hdr->romh.V0.csignature = ~ROM_H_SIGNATURE;
    hdr->romh.V0.hsize = sizeof( romheader_t );


    /* Process extra arguments */
    while( (c=getopt( argc, argv, "l:r:cx:s:p:f:i:vho:d:" )) != -1 )
    {
	switch( c )
	{
	    case 'l':		/* Load address in memory of the image */
		LoadAddress = strtoul( optarg, NULL, 16 );
		hdr->romh.V0.destination.high = LoadAddress >> 32;
		hdr->romh.V0.destination.low = LoadAddress & 0xFFFFFFFFU;
		info( "Got load address of 0x%lX\n", LoadAddress );
		break;

	    case 'r':		/* Optional offset into ROM of the image */
		RomOffset = strtoul( optarg, NULL, 16 );
		hdr->romh.V2.rom_offset = RomOffset & 0xFFFFFFFFU;
		info( "Got ROM offset of 0x%lX\n", RomOffset );
		break;

	    case 'x':		/* Hex value for firmware rev field */
		error( "Option -x not done yet\n" );
		break;

	    case 's':		/* String value for firmware rev field */
		strncpy( hdr->romh.V1.fwoptid.id, optarg, 8 );
		info( "Got FW rev string of %s\n", optarg );
		break;

	    case 'p':		/* Revision and API P/N for FW rev field */
		A = &hdr->romh.V1.fwoptid.api_id_S;
		rval = sscanf( optarg, "%d.%d-%d %d-%d-%d%c",
			&major, &minor, &rev, &api_category, &api_index, 
			&api_rev, &api_letter );

		/* Collect all the fields together into the right data struct */
		A->major = major, A->minor = minor, A->rev = rev;
		A->api_category = api_category;
		A->api_major = api_index / 100;
		A->api_minor = api_index % 100;
		A->api_rev = api_rev;
		A->api_letter = api_letter - 'A';

		info( "Got API part %d.%d-%d %02d-%02d%02d-%d%c\n",
			A->major, A->minor, A->rev, A->api_category,
			A->api_major, A->api_minor, A->api_rev, 
			A->api_letter + 'A' );

		/* Copy the data struct wholesale into our ROM header */
		memcpy( &hdr->romh.V1.fwoptid, A, sizeof( api_fwid_t ) );
		break;

	    case 'i':		/* FW ID number or string */
		id = id_lookup( optarg );
		if ( id == FW_UNKNOWN )
		{
		    error( "Firmware ID %s is unrecognised\n", optarg );
		}

		info( "Got FW ID of %d from string %s\n", id, optarg );
		hdr->romh.V1.fw_id = id; 
		break;

	    case 'v':		/* Verbosity */
		verbosity = 1;
		break;

	    case 'o':		/* Output filename */
		outputfile = malloc( strlen( optarg ) + 1 );
		if ( outputfile == NULL )
			error( "Couldn't alloc for handling output file\n");
		strcpy( outputfile, optarg );
		info( "Output filename is %s\n", outputfile );
		break;

	    case 'd':
		hdr_rev = atoi( optarg );
		info( "Got request for version %d header\n", hdr_rev );
		if ( hdr_rev != 2 && hdr_rev != 3 )
		    usage();
		break;

	    case 'h':		/* Help message & quit */
	    default:		/* Something went wrong */
		usage();
	}
    }


    hdr->romh.V1.hversion = 1, hdr->romh.V1.hversion_ext = hdr_rev - 1;

    /* Open input file, if not null, and read in */
    inputfile = argv[ optind ];

    if ( inputfile == NULL )
    {
	error( "Input file name not specified\n" );
    } else {

	inputfd = open( inputfile, O_RDONLY );
	if ( inputfd == -1 )
	{
	    perror( PROGNAME );
	    error( "Could not open input file %s\n", inputfile );
	}


	rval = fstat( inputfd, &statdata );
	inputsize = statdata.st_size;

	inputbuf = malloc( inputsize );
	if ( inputbuf == NULL )
	{
	    error( "Allocation failure on reading %dKB of input!\n",
		    inputsize >> 10 );
	}

	body_bytes = read( inputfd, inputbuf, inputsize );
	if ( body_bytes != inputsize )
	{
	    perror( PROGNAME );
	    error( "Failure on reading of %dKB of input!\n",
		    inputsize >> 10 );
	}

	hdr->romh.V0.size = hdr->romh.V1.rimage_size = body_bytes;
	close( inputfd );
    }


    /* Open output file, if not null */
    if ( outputfile == NULL )
    {
	error( "Output file name not specified\n" );

    } else {

	outputfd = creat( outputfile, 0644 );
	if ( outputfd == -1 )
	{
	    perror( PROGNAME );
	    error( "Could not open output file %s\n", outputfile );
	}
    }


    /* Perform checksums */
    hdr->romh.V0.checksum = compute_checksum( inputbuf, inputsize );
    hdr->romh.hchecksum = compute_checksum( (char *)hdr, 
	    sizeof( romheader_t ) - sizeof( unsigned ) );


    /* Write everything out */
    if ( hdr_rev == 2 )
    {
	hdr_bytes = write( outputfd, hdr, sizeof( romheader_t ) );
	if ( hdr_bytes != sizeof( romheader_t ) )
	{
	    perror( PROGNAME );
	    error( "Failure on writing ROM header to output file\n" );
	}

	body_bytes = write( outputfd, inputbuf, inputsize );
	if( body_bytes != inputsize )
	{
	    perror( PROGNAME );
	    error( "Failure on writing ROM image data to output file\n" );
	}

    }
    else	/* Version 3 ROM format has header separate (after) body */
    {
	body_bytes = write( outputfd, inputbuf, inputsize );
	if( body_bytes != inputsize )
	{
	    perror( PROGNAME );
	    error( "Failure on writing ROM image data to output file\n" );
	}

	hdr_bytes = write( outputfd, hdr, sizeof( romheader_t ) );
	if ( hdr_bytes != sizeof( romheader_t ) )
	{
	    perror( PROGNAME );
	    error( "Failure on writing ROM header to output file\n" );
	}
    }
    close( outputfd );
    info( "Wrote %dKB data to %s\n", body_bytes >> 10, outputfile );

    header_info_dump( hdr );
    return 0;
}

