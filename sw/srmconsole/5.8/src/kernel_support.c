/* file:	kernel_support.c
 *
 * Copyright (C) 1993 by
 * Digital Equipment Corporation, Maynard, Massachusetts.
 * All rights reserved.
 *
 * This software is furnished under a license and may be used and copied
 * only  in  accordance  of  the  terms  of  such  license  and with the
 * inclusion of the above copyright notice. This software or  any  other
 * copies thereof may not be provided or otherwise made available to any
 * other person.  No title to and  ownership of the  software is  hereby
 * transferred.
 *
 * The information in this software is  subject to change without notice
 * and  should  not  be  construed  as a commitment by digital equipment
 * corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by digital.
 */

/*
 *++
 *  FACILITY:
 *
 *      Alpha Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *      This module provides support routines for the kernel.
 *
 *  AUTHORS:
 *
 *      J. Kirchoff
 *
 *  CREATION DATE:
 *
 *	jmp	29-Nov-1993	Add avanti support
 *
 *      5-Oct-1993
 *
 *--
 */

#include	"cp$src:platform.h"
#include	"cp$src:alpha_def.h"
#include	"cp$src:ctype.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:common.h"
#include	"cp$src:wwid.h"
#include	"cp$src:pb_def.h"
#include	"cp$src:sb_def.h"
#include	"cp$src:cb_def.h"
#include	"cp$src:ub_def.h"
#include	"cp$src:pal_def.h"
#include	"cp$src:platform_cpu.h"
#include	"cp$inc:kernel_entry.h"

#if TURBO || RAWHIDE
#include	"cp$src:mem_def.h"
#endif
#include "cp$inc:prototypes.h"

extern struct LOCK spl_kernel;
extern struct LOCK el_spinlock;

extern struct SEMAPHORE pbs_lock;
extern struct pb **pbs;
extern unsigned int num_pbs;
extern unsigned int num_pbs_all;
extern struct SEMAPHORE sbs_lock;
extern struct sb **sbs;
extern unsigned int num_sbs;
extern unsigned int num_sbs_all;
extern struct SEMAPHORE ubs_lock;
extern struct ub **ubs;
extern unsigned int num_ubs;
extern unsigned int num_ubs_all;
#if FIBRE_CHANNEL
extern struct SEMAPHORE wwids_lock;
extern struct registered_wwid **wwids;
extern unsigned int num_wwids;
extern unsigned int num_wwids_all;
extern unsigned int wwid_ev_mask;
extern struct registered_portname **portnames;
extern unsigned int num_portnames;
extern unsigned int portname_ev_mask;
#endif /* FIBRE_CHANNEL */
extern unsigned int memory_low_limit;
extern unsigned __int64 memory_high_limit;


extern int	drivers_running;

extern int cb_ref;
extern int cb_fp[16];

/* below pointers required by modular */
#if FIBRE_CHANNEL
int wwidmgr_mode = 0;  
#endif /* FIBRE_CHANNEL */

#if MODULAR
extern struct window_head config;
#endif

#if MODULAR && !RHMIN
int boot( int argc, char *argv[] )
{
    boot_shared_adr = ovly_load("BOOT");
    if (!boot_shared_adr) {
	pprintf("Load of BOOT failed, FATAL\n");
	return msg_failure;
    }
    return boot_support(argc, argv);
}
#endif


int arcboot( int argc, char *argv[] )
{
#if ARC_SUPPORT
#if MODULAR
    arc_shared_adr = ovly_load("ARC");
    if (!arc_shared_adr) {
	pprintf("Load of ARC failed, FATAL\n");
	return msg_failure;
    }
    boot_shared_adr = ovly_load("BOOT");
    if (!boot_shared_adr) {
	pprintf("Load of BOOT failed, FATAL\n");
	return msg_failure;
    }
#endif

    arcboot_support(argc, argv);

#if MODULAR
    ovly_remove("BOOT");
    boot_shared_adr = 0;
#endif

#endif
    return msg_success;
}

void insert_pb( struct pb *pb )
    {
    krn$_wait( &pbs_lock );
    if( num_pbs == num_pbs_all )
	{
	num_pbs_all += 32;
	pbs = dyn$_realloc( pbs,
		sizeof( struct pb * ) * num_pbs_all,
		DYN$K_SYNC | DYN$K_NOOWN );
	}
    pbs[num_pbs++] = pb;
    krn$_post( &pbs_lock );
    }

void insert_sb( struct sb *sb )
    {
    krn$_wait( &sbs_lock );
    if( num_sbs == num_sbs_all )
	{
	num_sbs_all += 32;
	sbs = dyn$_realloc( sbs,
		sizeof( struct sb * ) * num_sbs_all,
		DYN$K_SYNC | DYN$K_NOOWN );
	}
    sbs[num_sbs++] = sb;
    krn$_post( &sbs_lock );
    }

void insert_ub( struct ub *ub )
    {
    krn$_wait( &ubs_lock );
    if( num_ubs == num_ubs_all )
	{
	num_ubs_all += 32;
	ubs = dyn$_realloc( ubs,
		sizeof( struct ub * ) * num_ubs_all,
		DYN$K_SYNC | DYN$K_NOOWN );
	}
    ubs[num_ubs++] = ub;
    krn$_post( &ubs_lock );
    }
#if FIBRE_CHANNEL
/*
 * Lock must be taken out by caller.
 */
void insert_wwid( struct registered_wwid *wwid )
    {
    int i, collision=0;

    wwid->unit = wwid_2_unit( &wwid->wwid );
    
    for( i = 0; i < num_wwids; i++ )
       {
       if( wwids[i]->unit == wwid->unit ) 
          {
          collision++;
          }
       }
    wwid->collision = collision;

    if( num_wwids == num_wwids_all )
	{
	num_wwids_all += 32;
	wwids = dyn$_realloc( wwids,
		sizeof( struct registered_wwid * ) * num_wwids_all,
		DYN$K_SYNC | DYN$K_NOOWN );
	}
    wwids[num_wwids++] = wwid;
    }
#endif /* FIBRE_CHANNEL */


void remove_pb( struct pb *pb )
    {
    int i;

    krn$_wait( &pbs_lock );
    for( i = 0; i < num_pbs; i++ )
	if( pb == pbs[i] )
	    {
	    pbs[i] = pbs[--num_pbs];
	    break;
	    }
    krn$_post( &pbs_lock );
    }

void remove_sb( struct sb *sb )
    {
    int i;
     
    krn$_wait( &sbs_lock );
    for( i = 0; i < num_sbs; i++ )
	if( sb == sbs[i] )
	    {
	    sbs[i] = sbs[--num_sbs];
	    break;
	    }
    krn$_post( &sbs_lock );
    }

void remove_ub( struct ub *ub )
    {
    int i;

    krn$_wait( &ubs_lock );
    for( i = 0; i < num_ubs; i++ )
	if( ub == ubs[i] )
	    {
	    ubs[i] = ubs[--num_ubs];
	    break;
	    }
    krn$_post( &ubs_lock );
    }

int strcmp_pb( struct pb *pb1, struct pb *pb2 )
    {
    int t;

    t = strcmp( pb1->string, pb2->string );
    if( t == 0 )
	t = pb1->order - pb2->order;
    return( t );
    }

int strcmp_sb( struct sb *sb1, struct sb *sb2 )
    {
    return( strcmp( sb1->string, sb2->string ) );
    }

int strcmp_ub( struct ub *ub1, struct ub *ub2 )
    {
    return( strcmp( ub1->string, ub2->string ) );
    }

struct pb *pb_name_match( char *name ) {

    struct pb *pb;
    struct pb *matching_pb = 0;
    int i;

    krn$_wait( &pbs_lock );
    for( i = 0; i < num_pbs; i++ ) {
	pb = pbs[i];
	if ( pb->name && ( strncmp( pb->name, name, 4 ) == 0 )) {
	    matching_pb = pb;
	    break;	
	}
    }
    krn$_post( &pbs_lock );
    return ( matching_pb );
}

int get_matching_pb( int hose, int bus, int slot, int function, int (*callback)( ), protoargs int p1, int p2 )
    {
    int i;
    struct pb *pb;

    krn$_wait( &pbs_lock );
    for( i = 0; i < num_pbs; i++ )
	{
	pb = pbs[i];
	if( ( hose >= 0 ) && ( hose != pb->hose ) )
	    continue;
	if( ( bus >= 0 ) && ( bus != pb->bus ) )
	    continue;
	if( ( slot >= 0 ) && ( slot != pb->slot ) )
	    continue;
	if( ( function >= 0 ) && ( function != pb->function ) )
	    continue;
	if( callback )
	    callback( pb, p1, p2 );
	else
	    {
	    krn$_post( &pbs_lock );
	    return( pb );
	    }
	}
    krn$_post( &pbs_lock );
    return( NULL );
    }

#if !(TURBO || RAWHIDE)
int controller_id_to_num(char *id)
{
    int num = 0;

    while (*id != '\0') {
	num *= 26;
	num += tolower(*id) - 'a' + 1;
	id++;
    }

    return num - 1;
}

char *controller_num_to_id(int num, char *id)
{
    int i = 0;
    int len;

    num++;
    do {
	id[i] = (num - 1) % 26 + 'a';
	num = (num - 1) / 26;
	i++;
    } while (num);
    id[i] = '\0';
    len = i - 1;
    for (i = 0; i <= len / 2; i++) {
	char temp;

	temp = id[i];
	id[i] = id[len - i];
	id[len - i] = temp;
    }
    return id;
}
#endif

char *set_io_prefix( struct pb *pb, char *name, char *string )
    {
    char controller[4];

    sprintf( name, "%s%s0",
	    pb->protocol,
	    controller_num_to_id( pb->controller, controller ) );
    name[4] = 0;
    strcat( name, string );
    return( name );
    }

char *set_io_name( char *name, char *prefix, int unit, int node_id, struct pb *pb )
    {
    char *protocol;
    char controller[4];

    if( prefix )
	protocol = prefix;
    else
	protocol = pb->protocol;
    sprintf( name, "%s%s%d.%d.%d.%d.%d",
	    protocol,
	    controller_num_to_id( pb->controller, controller ),
	    unit,
	    node_id,
	    pb->channel,
	    pb->bus * 1000 + pb->function * 100 + pb->slot,
	    pb->hose );
    return( name );
    }

char *set_io_alias( char *name, char *prefix, int unit, struct pb *pb )
    {
    char *protocol;
    char controller[4];

    if( prefix )
	protocol = prefix;
    else
	protocol = pb->protocol;
    sprintf( name, "%s%s%d",
	    protocol,
	    controller_num_to_id( pb->controller, controller ),
	    unit );
    strupper( name );
    return( name );
    }

#define COLUMNS 16

/*
 * Dump data in hex in byte, word, longword or quadword format.  If the
 * buffer/file isn't a multiple of the data width, then the last datum is 
 * zero extended. If asked to print zero_based address then do so, else 
 * print offset based address if fin is not supplied (fin = 0).
 */
void hd_helper (struct FILE *fout, struct FILE *fin, unsigned int offset, 
  unsigned int n, int datawidth, int zero_based)
{
	unsigned char	c;
	int		i, j, k;
	unsigned int	bytes_read;
 	unsigned char	datum [COLUMNS];
	char		line [128];
	unsigned int	offset_base = offset;
	int		ascii_column;

	if (fin && offset) {
	    fseek (fin, offset, SEEK_SET);
	}

	ascii_column = 10 + COLUMNS*2 + (COLUMNS/datawidth) + 2;

	while (n) {

	    memset (line, ' ', sizeof (line));

	    /*
	     * Read in a line's worth of data, but don't exceed the
	     * boundaries of the block requested.
	     */
	    memset (datum, 0, sizeof (datum));
	    if (fin) {
		bytes_read = fread (datum, 1, umin (n, sizeof (datum)), fin);
	    } else {
		bytes_read = umin (n, sizeof (datum));
		memcpy (datum, offset, bytes_read);
	    }
	    if (bytes_read == 0) break;

	    /* print address */
	    sprintf (line, "%z%08x", (zero_based) ? (offset-offset_base) : offset);

	    /* print out hex version of datum */
	    for (i=0; i<(bytes_read + datawidth - 1)/datawidth; i++) {
		for (k=0,j=datawidth-1; j>=0; k++,j--) {
		    int line_index;
		    line_index = 10 + datawidth*2*i + k*2 + i;
		    sprintf (&line [line_index], "%z%02X", (unsigned int) datum [i*datawidth + j]);
		}
	    }

	    /* ascii version of data */
	    for (i=0; i<bytes_read; i++) {
		c = datum [i];
		line [ascii_column + i] = isprint (c) ? c : '.';
		line [ascii_column + i + 1] = 0;
	    }

	    fprintf (fout, "%s\n", line);
	    if (killpending ()) break;

	    offset += bytes_read;
	    n -= bytes_read;
	}
}


#if STARTSHUT
int start_driver(struct FILE *fp) {
    int t, hose, slot;
    int loadadr;
    struct device *dev;
    struct INODE *inode;

    t = msg_success;

    inode = fp->ip;

    /* Lookup setmode pointer based on filename: */

    explode_devname(inode->name, 0, 0, 0, 0, 0, &slot, &hose);

    dev = find_dev_in_slot(&config, hose, slot);
    if (dev == NULL)
	return msg_failure;

    if (dev->tbl->overlay)
	loadadr = ovly_load(dev->tbl->overlay);
    else
	loadadr = ovly_load(dev->tbl->name);
    if (!loadadr)
	return msg_failure;

    /* Call setmode to start the port (if not already started): */
#if TURBO
    t = dev->tbl->setmode(DDB$K_START, dev);
#endif
#if RAWHIDE
    t = driver_set( DDB$K_START, dev );
#endif
    if (!((t == msg_success) /* || (t == msg_port_state_running) */ ))
	return t;

/* Find the unit we want: */

#if TURBO
    t = dev->tbl->establish(inode, dev, DEVCLASS$K_DISK);
#endif
#if RAWHIDE
    t = driver_establish(inode, dev, DEVCLASS$K_DISK);
#endif
    return t;
}

void stop_driver(struct FILE *fp) {
    int hose, slot;
    struct device *dev;

    /* Lookup setmode pointer based on filename: */

    explode_devname(fp->ip->name, 0, 0, 0, 0, 0, &slot, &hose);
    dev = find_dev_in_slot(&config, hose, slot);

    /* Call setmode to stop the port: */

#if RAWHIDE
    driver_set(DDB$K_STOP, dev);
#endif
#if TURBO
    dev->tbl->setmode(DDB$K_STOP, dev);
#endif

    if (dev->tbl->overlay)
	ovly_remove(dev->tbl->overlay);
    else
	ovly_remove(dev->tbl->name);
}

void start_drivers () {
}

void stop_drivers () {
}
#endif


/*+
 * ============================================================================
 * = write_pc - simple write to the psr                                       =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine writes a datum to the psr.  This interface is layered
 *	on top of the driver, yet is a simpler interface for psr access under 
 * 	program control.
 *
 *  FORMAL PARAMETERS:
 *
 *	int	*datum	- location of longword data
 *
 *  RETURN VALUE:
 *
 *	Always returns msg_success.
 *
 *  IMPLICIT INPUTS:
 *
 *      NONE
 *
 *  IMPLICIT OUTPUTS:
 *
 *      NONE
 *
 *  SIDE EFFECTS:
 *
 *--
 */
void write_pc( int *value )
{
    struct FILE *fp;

    fp = fopen( "psr", "w" );
    fwrite( value, 8, 1, fp );
    fclose( fp );
}

/*+
 * ============================================================================
 * = write_gpr - simple write to a gpr                                        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine writes a datum to a single gpr.  This interface is layered
 *	on top of the driver, yet is a simpler interface for gpr access under 
 * 	program control.  The offset passed is a gpr number.
 *
 *  FORMAL PARAMETERS:
 *
 *      int	gprnum	- gpr to write to
 *	int	*datum	- location of quadword/longword data
 *
 *  RETURN VALUE:
 *
 *	Always returns msg_success.
 *
 *  IMPLICIT INPUTS:
 *
 *      NONE
 *
 *  IMPLICIT OUTPUTS:
 *
 *      NONE
 *
 *  SIDE EFFECTS:
 *
 *--
 */
void write_gpr (int gprnum, int *datum) {
	struct FILE *fp;

	fp = fopen ("gpr", "w");
	fseek (fp, gprnum, SEEK_SET);
	fwrite (datum, 1, sizeof (REGDATA), fp);
	fclose (fp);
}

/*+
 * ============================================================================
 * = read_gpr - simple read from a gpr                                        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine read a single gpr.  This interface is layered
 *	on top of the driver, yet is a simpler interface for gpr access under 
 * 	program control.  The offset passed is a gpr number.
 *
 *  FORMAL PARAMETERS:
 *
 *      int	gprnum	- gpr to write to
 *	int	*datum	- location of quadword/longword data
 *
 *  RETURN VALUE:
 *
 *	Always returns msg_success.
 *
 *  IMPLICIT INPUTS:
 *
 *      NONE
 *
 *  IMPLICIT OUTPUTS:
 *
 *      NONE
 *
 *  SIDE EFFECTS:
 *
 *
 *--
 */
void read_gpr (int gprnum, int *datum) {
	struct FILE *fp;

	fp = fopen ("gpr", "r");
	fseek (fp, gprnum, SEEK_SET);
	fread (datum, 1, sizeof (REGDATA), fp);
	fclose (fp);
}

/*+
 * ============================================================================
 * = write_ipr - simple write to a ipr                                        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine writes a datum to a single ipr.  This interface is layered
 *	on top of the driver, yet is a simpler interface for ipr access under 
 * 	program control.  The offset passed is a ipr number.
 *
 *	This driver is pathological in that the read and write routines
 *	don't look at at the number and size fields compeletly.  What they
 *	do is always read one and only one ipr into a local copy, and then
 *	completely or partially copy the data to the user's buffer depending
 *	on the total buffer size.  This allows the driver to at least not
 *	trash programs that read drivers one character at a time.
 *
 *
 *  FORMAL PARAMETERS:
 *
 *      int	ipr_num	- ipr to write to
 *	int	*datum	- location of quadword/longword data
 *
 *  RETURN VALUE:
 *
 *	Always returns msg_success.
 *
 *  IMPLICIT INPUTS:
 *
 *      NONE
 *
 *  IMPLICIT OUTPUTS:
 *
 *      NONE
 *
 *  SIDE EFFECTS:
 *
 *--
 */
void write_ipr (int ipr_num, int *datum) {
	struct FILE *fp;

	fp = fopen ("ipr", "w");
	fseek (fp, ipr_num, SEEK_SET);
	fwrite (datum, 1, sizeof (REGDATA), fp);
	fclose (fp);
}

/*+
 * ============================================================================
 * = read_ipr - simple read from a ipr                                        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine read a single ipr.  This interface is layered
 *	on top of the driver, yet is a simpler interface for ipr access under 
 * 	program control.  The offset passed is a ipr number.
 *
 *  FORMAL PARAMETERS:
 *
 *      int	ipr_num	- ipr to write to
 *	int	*datum	- location of quadword/longword data
 *
 *  RETURN VALUE:
 *
 *	Always returns msg_success.
 *
 *  IMPLICIT INPUTS:
 *
 *      NONE
 *
 *  IMPLICIT OUTPUTS:
 *
 *      NONE
 *
 *  SIDE EFFECTS:
 *
 *
 *--
 */
void read_ipr (int ipr_num, int *datum) {
	struct FILE *fp;

	fp = fopen ("ipr", "r");
	fseek (fp, ipr_num, SEEK_SET);
	fread (datum, 1, sizeof (REGDATA), fp);
	fclose (fp);
}

/*+
 * ============================================================================
 * = write_pt - simple write to a pt                                        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine writes a datum to a single pt.  This interface is layered
 *	on top of the driver, yet is a simpler interface for pt access under 
 * 	program control.  The offset passed is a pt number.
 *
 *  FORMAL PARAMETERS:
 *
 *      int	ptnum	- pt to write to
 *	int	*datum	- location of quadword/longword data
 *
 *  RETURN VALUE:
 *
 *	Always returns msg_success.
 *
 *  IMPLICIT INPUTS:
 *
 *      NONE
 *
 *  IMPLICIT OUTPUTS:
 *
 *      NONE
 *
 *  SIDE EFFECTS:
 *
 *--
 */
void write_pt (int ptnum, int *datum) {
	struct FILE *fp;

	fp = fopen ("pt", "w");
	fseek (fp, ptnum, SEEK_SET);
	fwrite (datum, 1, sizeof (REGDATA), fp);
	fclose (fp);
}

/*+
 * ============================================================================
 * = read_pt - simple read from a pt                                        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine read a single pt.  This interface is layered
 *	on top of the driver, yet is a simpler interface for pt access under 
 * 	program control.  The offset passed is a pt number.
 *
 *  FORMAL PARAMETERS:
 *
 *      int	ptnum	- pt to write to
 *	int	*datum	- location of quadword/longword data
 *
 *  RETURN VALUE:
 *
 *	Always returns msg_success.
 *
 *  IMPLICIT INPUTS:
 *
 *      NONE
 *
 *  IMPLICIT OUTPUTS:
 *
 *      NONE
 *
 *  SIDE EFFECTS:
 *
 *
 *--
 */
void read_pt (int ptnum, int *datum) {
	struct FILE *fp;

	fp = fopen ("pt", "r");
	fseek (fp, ptnum, SEEK_SET);
	fread (datum, 1, sizeof (REGDATA), fp);
	fclose (fp);
}

/*+
 * ============================================================================
 * = twos_checksum - perform a 2's complement checksum                        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Perform a 64-bit 2's complement checksum, ignoring overflow.
 *
 * FORM OF CALL:
 *
 *	twos_checksum (addr, count, checksum)
 *
 * RETURN CODES:
 *
 *	nothing meaningful
 *
 * ARGUMENTS:
 *
 *	int *addr	- address of beginning of checksummed block
 *	int count	- number of longwords to checksum
 *	unsigned int *checksum	- address that checksum qwd will be written to
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
void twos_checksum(UINT addr, int count, unsigned int *checksum) {
    int i;
    unsigned int chksum[2] = {0,0};

    spinlock(&spl_kernel);
    for (i = 0; i < count; i +=2) {
	*(INT *) &chksum += ldq(addr);
	addr = addr + 8;
    }
    checksum[0] = chksum[0];
    checksum[1] = chksum[1];
    spinunlock(&spl_kernel);
}

#if MODULAR

/*
 * This routine is called early in the idle process before we have any
 * file system or dynamic memory.  This allows us to use qprintf in a 
 * minimal environment.
 */

char el_ibuf [MAX_ELBUF+16];	/* initial buffer until we can expand */
char *el_buffer;		/* plus room for timestamp if full */
volatile int el_in;
volatile int el_out;
volatile int el_len;			/* number of byte in log	*/
volatile int el_pos;			/* line position		*/
volatile int el_size;			/* space allocated for log	*/
volatile int el_lostlines;		/* number of lost lines		*/
struct FILE *el_fp;

void el_setup () {
    el_in = 0;
    el_out = 0;
    el_len = 0;
    el_pos = 0;
    el_size = MAX_ELBUF;
    el_buffer = el_ibuf;
    el_lostlines = 0;
    el_fp = 0;
}

void el_write_time();
void el_store_time(int time, char c);
void el_store_byte(char bufc);

/*
 * Write a line to the event logger.  Well formed input will always have a
 * terminating newline.  Translates nulls into newlines.
 *
 * This routine will work in all contexts.  If we're being called directly from
 * qprintf, then we also print out to the terminal used polled mode and if
 * we're in the process of powering up.
 */

void el_writeline (const char *buf, int buflen)
{
    int i;
    char bufc;

/* lock up access to the event log */

    spinlock (&el_spinlock);
/*
 * Copy the buffer into the event log, checking
 * for overflow into the next line.
 */
    for (i=0; i<buflen; i++) {
	bufc = buf [i];
	if (!el_pos)
	    el_write_time();
	el_store_byte(bufc);
    }
    spinunlock (&el_spinlock);
}

void el_store_byte(char bufc)
{
/* check for overflow into the last line in the log */

    if (el_len == el_size) {
	char c;
	el_lostlines++;
	do {			/* wipe out the last line */
	    c = el_buffer [el_out];
	    if (++el_out >= el_size) el_out = 0;
		el_len--;
	} while (c != '\n');
    }

    el_buffer [el_in] = bufc;
    if (++el_in >= el_size)
	el_in = 0;
    el_len++;
    el_pos++;
    if (bufc == '\n')
	el_pos = 0;
}

void el_write_time()
{
    int sec;
    int minute;
    int hour;
    int id = whoami();

    sec = read_watch_sec();
    minute = read_watch_min();
    hour = read_watch_hour();
    el_store_time(hour, ':');
    el_store_time(minute, '.');
    el_store_time(sec, ' ');
    el_store_time(id, ' ');
}

void el_store_time(int time, char c)
{
    unsigned int digit;

    digit = time % 10;
    time /= 10;
    el_store_byte(time + '0');
    el_store_byte(digit + '0');
    el_store_byte(c);
}

#endif

/*
 * this routine builds an integer from a byte character string
 */ 
unsigned int numfrombytes (unsigned char *s, int n) {
	int   val, i;
	val = 0;
	for (i = 0; i < n; i++)
	    val += ((((unsigned int) s[i]) & 0xff) << (i * 8));
	return (val);
}

#if FIBRE_CHANNEL
int wwidcmp( union wwid *wwid1, union wwid *wwid2)
{
  int status = msg_failure;
  int *iwwid1, *iwwid2;
  

  iwwid1 = wwid1;
  iwwid2 = wwid2;
#if 0
  printf("wwidcmp( %x , %x ) \n", wwid1, wwid2 );
  printf("%x / %x %x %x   %x/ %x %x %x\n", wwid1, iwwid1[0], 
       iwwid1[1], iwwid1[2], wwid2, iwwid2[0], iwwid2[1], iwwid2[2] );
#endif
  
  if ( wwid1->hdr.type == 0 ) {
    return msg_failure;
  }
  if (wwid1->hdr.len != wwid2->hdr.len) {
     return msg_failure;
  }
  switch (wwid1->hdr.type) {
     case SCSI_WWID_PG83_IEEE128:
     case SCSI_WWID_PG83_IEEE64:
     case SCSI_WWID_PG80_SERIALNO:
       status = memcmp( &wwid1->hdr, 
                        &wwid2->hdr,
		        wwid1->hdr.len+sizeof(wwid_hdr_t) );
       break;
     case SCSI_WWID_PG83_VENDOR:
     case SCSI_WWID_SERIALNO:
     case SCSI_WWID_TGT_IDENT:
     case SCSI_WWID_BTL:
     default:
       printf("wwid type %x not implemented\n", wwid1->hdr.type );
       break;
  }
 return status;
}


/*+
 * ============================================================================
 * = wwid2wwev - convert a wwid struct into an string for ev storage          =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Convert a wwid into a string that is stored into the wwid<n> ev.
 *      Also useful, a pointer to the WWID portion of this string is
 *      the return value.  A wwid<n> string has the form:
 *
 *            unit collision wwid
 *
 *      For example:
 *             
 *            18456 1 WWID:04100020:"SEAGATE ST19171FC       LA202003"
 *
 * FORM OF CALL:
 *
 *	wwid2wwev( wwid, text )               
 *
 * RETURN CODES:
 *
 *      success - pointer to the WWID string
 *      failure - NULL
 *
 * ARGUMENTS:
 *
 *	wwid *wwid	- pointer to a wwid                          
 *	char *text  	- pointer to a string to receive the results
 *      int unit        - unit number
 *      int collision   - collision number
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
char *wwid2wwev( char *text, int unit, int collision, union wwid *wwid )
{
  int i, j = 0;
  unsigned char *s, *pWWID;

    sprintf( text, "%d %d WWID:%08x:", unit, collision, wwid->hdr );
    pWWID = strstr( text, "WWID:");
    s = text + strlen(text);
    switch (wwid->hdr.type) {
     case SCSI_WWID_PG83_IEEE128:
       for( i = 0; i < 16; i = i + 2 ) {
           sprintf( s, "%02x%02x", wwid->ieee128.ieee[i], 
                                                    wwid->ieee128.ieee[i+1] );
	   s = s + strlen(s);
           if( i < 14) {
	     *s = '-';
	      s++;
	     *s = 0;
	   }
       }
       break;
     case SCSI_WWID_PG83_IEEE64:
       pprintf("dump pg83 IEEE64 not yet implemented\n");
       break;
     case SCSI_WWID_PG80_SERIALNO:
       /*
        * Todo: check out length before committing to ev.  
	*/
       sprintf( s, "\"%s\"", &wwid->serialno.vid ); /* pick up vid,pid,sn */
       break;
     case SCSI_WWID_PG83_VENDOR:
     case SCSI_WWID_SERIALNO:
     case SCSI_WWID_TGT_IDENT:
     case SCSI_WWID_BTL:
     case SCSI_WWID_DEC_UNIQUE:
       pprintf("wwid type %x not implemented\n", wwid->hdr.type );
       break;
     default:
#if 1
       pprintf("wwid2wwev - invalid wwid :%x\n", wwid );
#endif
       text[0] = 0;  /* return a null string */
       pWWID = 0;
       break;
  }
  return pWWID;
}


/*+
 * ============================================================================
 * = wwev2wwid - convert a string wwid ev format into the wwid structure      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Load up a wwid structure from a string.  This is done when
 *      an wwid<n> environment variable is read.
 *
 * FORM OF CALL:
 *
 *	wwev2wwid( txt, *unit, *collision, *wwid )                
 *
 * RETURN CODES:
 *
 *      success - pointer to the WWID string
 *      failure - NULL
 *
 * ARGUMENTS:
 *
 *	char *text  	- pointer to a string 
 *	wwid *wwid	- pointer to a wwid to receive the results         
 *      int *unit       - pointer to int to receive unit
 *      int *collision  - pointer to int to receive collision
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
char *wwev2wwid( char *txt, int *unit, int *collision, union wwid *w )
{
  int i,j;
  char *s, *pWWID;
  char token[10];

  strelement(txt, 0, " ", &token ); /* pick off the unit number */
  *unit = atoi( &token );
  strelement(txt, 1, " ", &token ); /* pick off the collision number */
  *collision = atoi( &token );

  pWWID = strstr(txt, "WWID:");   /* position to the WWID        */
  if (!pWWID) 
    return 0;
  s = &pWWID[5];   /* get past WWID: */
  if(s[8] != ':') {
    return 0;
  }
  common_convert( s, 16, &w->hdr, 4);
  switch (w->hdr.type) {
     case SCSI_WWID_PG83_IEEE128:
       s = s + 9;                                  /* get past header */
       token[2] = 0;                               /* asciz setup     */
       for( i=0, j=0; i<39; i=i+5, j=j+2 ) {
	   token[0] = s[i];
	   token[1] = s[i+1];
	   w->ieee128.ieee[j] = xtoi( token );
	   token[0] = s[i+2];
	   token[1] = s[i+3];
	   w->ieee128.ieee[j+1] = xtoi( token );
	   }
       break;
     case SCSI_WWID_PG83_IEEE64:
       pprintf("dump pg83 not yet implemented\n");
       break;
     case SCSI_WWID_PG80_SERIALNO:
       /* Start at character 10 - beyond the 04100020:"  */
       memcpy( &w->serialno.vid, &s[10], w->hdr.len );
       break;
     case SCSI_WWID_PG83_VENDOR:
     case SCSI_WWID_SERIALNO:
     case SCSI_WWID_TGT_IDENT:
     case SCSI_WWID_BTL:
     case SCSI_WWID_DEC_UNIQUE:
       pprintf("wwid type %x not implemented\n", w->hdr.type );
       break;
  }
  return pWWID;
}

/*
 *  Need to find w/ collision also!
 */
struct registered_wwid *find_wwidfilter_from_unit_col( int unit, int col, int *idx )
{
  int i;
  struct registered_wwid *filter = NULL;
  for( i = 0; i < num_wwids; i++ ){
     if( (wwids[i]->unit == unit) && (wwids[i]->collision == col) ) {
       filter = wwids[i];
       break;
     }
  }
  *idx = i;
  return filter;

}

struct registered_wwid *find_wwidfilter_from_evnum( int evnum, int *idx )
{
  int i;
  struct registered_wwid *filter = NULL;
  for( i = 0; i < num_wwids; i++ ){
     if( wwids[i]->evnum == evnum ) {
       filter = wwids[i];
       break;
     }
  }
  *idx = i;
  return filter;

}


struct registered_wwid *find_wwidfilter_from_wwids( union wwid *w, int *idx )
{
/*
 * TODO: Make sure caller has the lock
 */

  int i;
  int status;
  struct registered_wwid *filter = NULL;
  char srm_wr[80], *srm_wwid;
  for( i = 0; i < num_wwids; i++ ){
     status = wwidcmp( &wwids[i]->wwid, w );
     if( status == msg_success ) {
       filter = wwids[i];
       break;
     }
  }
  *idx = i;
  return filter;
}



struct registered_portname *fc_find_portname( unsigned __int64 *p1, int *idx,
                                                                int swap_bytes )
  {
   int i,status;
   unsigned __int64 swap_p1, p2;
   struct registered_portname *filter = NULL;
   unsigned char *src = p1, *dst = &swap_p1;
   int len = 8;

   /*
    * Spin the portname around that was passed in.  It is compared against
    * the data structure values which are stored in a displayable format.
    */
   if( swap_bytes )
     {
      src += len;
      while( --len >= 0 )
         *dst++ = *--src;
     }

   /*
    * Walk the list of registered portnames and compare.
    */
   for(i=0; i<num_portnames; i++)
     {
       p2 = xtoi(portnames[i]->portname);
       if(swap_bytes)
         {
          /*pprintf("comparing %x against %x\n", swap_p1, p2 ); */
          status = memcmp (&swap_p1, &p2, 8);     /* sizeof(NAME_TYPE) */
         }
       else
         {
          /*pprintf("comparing %x against %x\n", *p1, p2 ); */
          status = memcmp (p1, &p2, 8);     /* sizeof(NAME_TYPE) */
         }
       if(status==0) {
         *idx = i;
         filter = portnames[i];
         break;
       }
     }
   return filter;
  }


/*
 * WWID_2_UNIT
 *
 * Functional description:
 *
 *   Compute a VMS device unit number from an SRM-prefixed SCSI WWID.
 *
 *   This routine computes the CRC-16 of the entire WWID and returns the
 *   low order 15-bits of the result as the unit number.
 *
 *
 * Calling convention:
 *
 *   unit = wwid_2_unit( wwid_p )
 *
 * Formal parameters:
 *
 *   wwid_p	Input	Pointer to an SRM-prefixed WWID.  The WWID data
 *			(after the prefix) should be in SCSI most-significant-
 *			byte-first order.
 *
 * Return value:
 *
 *   unit		Unit number.  The unit number fits within the
 *			low-order 15-bits.  The remaining upper-order bits
 *			are all zero.
 */

int  wwid_2_unit( union wwid *wwid_p ) {

  static uint     /* CRC table for CRC-16 */
    crc_table[16] = { 0x00000000, 0x0000CC01, 0x0000D801, 0x00001400, 
                      0x0000F001, 0x00003C00, 0x00002800, 0x0000E401, 
                      0x0000A001, 0x00006C00, 0x00007800, 0x0000B401, 
                      0x00005000, 0x00009C01, 0x00008801, 0x00004400 };

  uchar *next_byte_p;		/* Pointer to next byte in WWID string */
  int n_bytes;			/* Number bytes left in WWID string */
  int crc_idx;			/* Computed index into crc_table */

  uint crc_value;		/* Accumulator for CRC-16 result */

  /* Point to the first byte of the WWID string.  Start out with result=0 */

  next_byte_p = (void *) wwid_p;
  crc_value = 0;

  /* Consider each byte in the WWID string and compute its contribution
   * to the CRC-16 result.  The size of the WWID string is the length from
   * the prefix plus the size of the prefix itself.
   */

  for(n_bytes = wwid_p->hdr.len + sizeof( wwid_p->hdr );
      n_bytes > 0; n_bytes-- ) {

    crc_value ^= *next_byte_p++;	/* XOR next byte into result */

    crc_idx = crc_value & 0xF;          /* low-order 4-bits is CRC table index */
    crc_value >>= 4;			/* right shift result 4-bits */
    crc_value ^= crc_table[crc_idx];	/* XOR table entry using index */

    crc_idx = crc_value & 0xF;          /* low-order 4-bits is CRC table index */
    crc_value >>= 4;			/* right shift result 4-bits */
    crc_value ^= crc_table[crc_idx];	/* XOR table entry using index */
  }

  return crc_value & 0x7FFF;		/* 15-bit unit numbers */
}

#endif /* FIBRE_CHANNEL */
