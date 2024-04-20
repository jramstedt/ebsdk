#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$inc:prototypes.h"
#include "cp$src:kernel_def.h"
#include "cp$src:pb_def.h"
#include "cp$src:probe_io_def.h"
scsi_lun_map( struct scsi_sb *sb, int function, int lun )
    {
    }
use_eisa_ecu_data( )
    {
    }
kbd_get( )
    {
    }
mouse_get( )
    {
    }
kbd_put( )
    {
    }
mouse_put( )
    {
    }
kbd_reinit( )
    {
    }
tga_reinit( )
    {
    }
int reusememory;
vga_reinit( )
    {
    }
rtc_read( int offset )
    {
    if( offset == 0x0f )
	return( 0 );
    return( -1 );
    }
rtc_write( )
    {
    return( 0 );
    }
set_serial_params( )
    {
    }
unsigned int pci_size[SIZE_GRAN_BASE_NUM];
int pci_bus;
int manual_config;
int bus_probe_algorithm;
int probe_number;
unsigned int num_pbs;
unsigned int num_pbs_all;
unsigned int num_sbs;
unsigned int num_sbs_all;
unsigned int num_ubs;
unsigned int num_ubs_all;
struct ub **ubs;
struct pb **pbs;
struct sb **sbs;
struct SEMAPHORE scs_lock;
struct SEMAPHORE ubs_lock;
struct SEMAPHORE pbs_lock;
struct SEMAPHORE sbs_lock;
probe_io( )
    {
    krn$_seminit( &scs_lock, 1, "scs_lock" );
    krn$_seminit( &ubs_lock, 1, "ubs_lock" );

    num_pbs = 0;
    num_pbs_all = 32;
    pbs = malloc( sizeof( struct pb * ) * 32 );

    krn$_seminit( &sbs_lock, 1, "sbs_lock" );
    num_sbs = 0;
    num_sbs_all = 32;
    sbs = malloc( sizeof( struct sb * ) * 32 );

    krn$_seminit( &pbs_lock, 1, "pbs_lock" );
    num_ubs = 0;
    num_ubs_all = 32;
    ubs = malloc( sizeof( struct ub * ) * 32 );
    }
