/* file:	pc264.c
 *                   
 * Copyright (C) 1997, 1998, 1999 by
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
 *      Alpha SRM Console Firmware
 *                                                                   
 *  MODULE DESCRIPTION:                                              
 *                                                                   
 *	Platform specific miscelleneous routines.
 *                                                                   
 *  AUTHORS:
 *
 *     	Eric Goehl
 *
 *  CREATION DATE:
 *  
 *      26-Feb-1997
 *
 *  MODIFICATION HISTORY:
 *      
 *	egg	20-Aug-1999	Added DS20E andXP2000 lurt information for Goldrack.
 *	jwj	13-May-1999	Fix show_cpu to handle new srom revision format (increase rev[]).
 *	jwj	27-Apr-1999	Change SROM revision printout.
 *	er	03-Mar-1999	Added SMM and LURT values for dual processor DS20.
 *	er	03-Sep-1998	Changed console_restart() to use soft reset register.
 *	er	21-Aug-1998	Added code to show_power() to display/clear RCM
 *				EEPROM environmental data.
 *	er	07-Aug-1998	Added ev_wr_shutdown_temp(), ev_rd_shutdown_temp(),
 *				and validate_temp_value() routines.
 *				Added code to build_dsrdb() to differentiate between
 *				DP264 and AlphaServer DS20 based on member id.
 *	er	16-Jun-1998	Added show_cpu(), halt_switch_in() and
 *				get_srom_revision() routines.
 *	er	26-May-1998	Added show_power routine.
 *	er	12-Mar-1998	Combined elements from APC.C and old PC264.C
 *				Reworked ARC/NT firmware support routines for 
 *				ARC SCB parameter passing.
 *	er	24-Sep-1997	Added assign_pci_vector() and clear_cpu_errors()
 *				to work with tsunami_io.c
 *
 *--
 */                                                

#include	"cp$src:platform.h"	
#include        "cp$src:kernel_def.h"
#include	"cp$src:pb_def.h"
#include	"cp$src:pal_def.h"
#include	"cp$src:common.h"
#include	"cp$src:ctype.h"
#include	"cp$src:hwrpb_def.h"
#include	"cp$src:alphascb_def.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:msg_def.h"
#include 	"cp$src:ev_def.h"
#include	"cp$inc:prototypes.h"
#include	"cp$src:arc_scb_def.h"
/* Platform specific definition files */
#include 	"cp$src:platform_cpu.h"
#include	"cp$inc:platform_io.h"
#include	"cp$src:tsunami.h"
#include	"cp$src:isacfg.h"
#include	"cp$src:romhead.h"
#include	"cp$src:impure_def.h"

#define DEBUG	0

void __MB( void );
#define mb( ) __MB( )

/* External data structures and definitions */

extern int HWRPB;
extern int do_bpt();
extern int primary_cpu;
extern int in_console;
extern int graphics_console;
extern unsigned __int64 mem_size;
extern int cpu_mask;
extern int ev_initing;

/* External function prototype declarations */

/* From kernel_alpha.mar */
extern unsigned __int64 mfpr_whami( void );                                                               
extern unsigned __int64 ldq( unsigned __int64 address );
extern void stq( unsigned __int64 address, unsigned __int64 data );

/* From kernel.c */
extern int null_procedure( void );

/* From ev_driver. c */ 
extern int ev_read (char *name, struct EVNODE **evptr, int option);

/* From printf.c */                             
extern int err_printf( char *format, ... );
extern int pprintf( char *format, ... );

/* From filesys.c */
extern struct FILE *fopen( const char *__filename, const char *__mode, ... );
extern size_t fread( void *__ptr, size_t __size, size_t __nmemb, struct FILE *__stream );
extern int fseek( struct FILE *__stream, __int64 __offset, int __whence );
extern int fclose( struct FILE *__stream );

/* From startstop.c */
extern void start_drivers( void );
extern void stop_drivers( void );

/* From toy_driver.c */
extern void rtc_write( int offset, int data );
extern int rtc_read( int offset );

/* From isacfg.c */
extern unsigned long isacfg_get_device( char *handle, int instance, struct TABLE_ENTRY *ptr );

/* From tsunami_io.c */
extern void tsunami_probe_local( );
extern unsigned char xtig( struct pb *pb, unsigned __int64 offset, unsigned char data,
		    unsigned char (*ld_or_st)( ) );

/* From timer.c */
extern int krn$_timer_get_cycle_time( int id );
extern int krn$_psec_cycle( int id );

/* From readline.c */
extern int read_with_prompt( 
    char *prompt, 
    int maxbuf, 
    char *buf,
    struct FILE *fp_in, 
    struct FILE *fp_out, 
    int echo );

#if XDELTA_ON
extern void *memset( void *__s, int __c, size_t __n );
extern int *getpd( int i );
extern void *dyn$_malloc( unsigned int size, unsigned int option, unsigned int modulus, 
			  unsigned int remainder, struct ZONE *zone );
#endif
   
/* Global data structures and definitions */

int n810_speed_ptr;
int isa_count = 1; 	/* required for DE205_driver.c??? */	
int all_cpu_mask;
                                                       

/*=======================================================================
// platform() - Used to indicate whether this code is running 
//		on the ISP model or on real hardware.
//=======================================================================
*/
int platform( ) {

    int platform;

    /*
     * ISP flag at location 0xBFFC.  In your ISP startup
     * command procedure, deposit 0xBFFC with CAFEBEEF.
     */
    if ( (*(int *)0xBFFC == 0xCAFEBEEF) ){
        platform = ISP_MODEL;
    } else {
        platform = REAL_HW;
    }
    return platform;
}


/*+
 * ============================================================================
 * = whoami - Identify on which processor the process is running              =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Return a small (less that 63) integer indicating which CPU the caller
 *	is running on.  Note that by the time the caller gets around to using
 *	the information, they may have been moved to another CPU.
 *
 * FORM OF CALL:
 *
 *	whoami( ); 
 *
 * RETURNS:
 *
 *      CPU number on which current process is running
 *                          
 * ARGUMENTS:
 *
 *      None
 *
 * SIDE EFFECTS:
 *                 
 *      None
 *   
-*/
int whoami( void ) 
{
    return( mfpr_whami( ) );
}


/*+                         
 * ============================================================================
 * = primary - Determine if the process is running on the primary processor.  =
 * ============================================================================
 *   
 * OVERVIEW:
 *   
 *   	Return a value of 1 if the caller is running on the primary processor.
 *   	Return a value of 0 is the caller is running on a secondary processor.
 *
 * FORM OF CALL:
 *
 *	primary( ); 
 *
 * RETURN CODES:
 *                          
 *      0 - current process is running on a secondary processor.
 *      1 - current process is running on the primary processor.
 *
 * ARGUMENTS:
 *
 *      None
 *
 * SIDE EFFECTS:
 *
 *      None
 *   
-*/  
primary( void ) 
{
    return( whoami( ) == *( uint32 * )PAL$PRIMARY );
}                           
     
    
/*+  
 * ============================================================================
 * = node_halt - node halt the processor                                      =
 * ============================================================================
 *   
 * OVERVIEW:
 *   
 *   	Node halts the given processor.
 *   
 * FORM OF CALL:
 *   
 *   	node_halt( id ); 
 *   
 * RETURNS:                 
 *   
 *      None                                              
 *
 * ARGUMENTS:
 *
 *      int id - processor to halt
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/  
node_halt( int id ) 
{   
    if ( !( in_console & ( 1 << id ) ) ) {
	cserve( CSERVE$MP_WORK_REQUEST, id, MP$HALT );
	krn$_sleep( 5 );
    }
}    


/*+
 * ============================================================================
 * = start_secondary - Start a secondary processor at the specified address.  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine first forces a secondary processor into Console mode.
 *
 * FORM OF CALL:
 *                          
 *	start_secondary( id, address ); 
 *
 * RETURN CODES:
 *
 *      0 - success
 *      1 - secondary processor failed to start.
 *
 * ARGUMENTS:
 *
 *      int id		- id of processor to start
 *	*int address	- physical address of routine to be started.
 *
 * SIDE EFFECTS:
 *
 *      None                
 *
-*/
start_secondaries( )
{
    int i;
    struct EVNODE ev, *evp;

    evp = &ev;
    ev_read( "cpu_enabled", &evp, 0 );
    for( i = 0; i < MAX_PROCESSOR_ID; i++ )
	if( ( i != primary_cpu ) && ( evp->value.integer & ( 1 << i ) ) )
     	    start_secondary( i, PAL$PAL_BASE + 1 );
}

start_secondary( int id, int address )
{
    ( ( uint64 * )PAL$CPU0_START_BASE )[id] = address;
    mb( );
    outtig( NULL, 0xC00028 + id, 0 );
    krn$_sleep( 1000 );
}


/*+
 * ============================================================================
 * = console_restart - Restart the system.                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *    
 *	Platform specific code for restarting the system.
 *
 * FORM OF CALL:
 *
 *	console_restart( ); 
 *
 * RETURNS:
 *
 *      None
 *
 * ARGUMENTS:
 *
 *      None
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
console_restart( )
{
    pprintf( "Initializing...\n" );

    krn$_micro_delay( 10000 );

    outtig( NULL, 0xE00004, 1 );
}


void clear_cpu_errors( int id )
{
    unsigned __int64 temp;

    temp = ReadTsunamiCSR( CSR_MISC );
    temp |= ( uint64 )1 << 28;	    /* Clear NXM error */
    WriteTsunamiCSR( CSR_MISC, temp );

    WriteTsunamiCSR( PCHIP0_PERROR, ReadTsunamiCSR( PCHIP0_PERROR ) );
    WriteTsunamiCSR( PCHIP1_PERROR, ReadTsunamiCSR( PCHIP1_PERROR ) );
}

void reset_cpu( int id )
{
    clear_cpu_errors( id );
}

#define MAX_REV_LEVELS 8                      
#define RCS_LOCK_SIGNATURE 0x55

void get_srom_revision( char *srev, int id )
{
    struct impure *impure_ptr;
    unsigned int signature;
    int i, rev, increase;

    impure_ptr = ( struct impure * )( PAL$IMPURE_BASE + PAL$IMPURE_COMMON_SIZE + ( id * PAL$IMPURE_SPECIFIC_SIZE ) );
    signature = *( unsigned __int64 * )( impure_ptr->cns$srom_signature );
    if ( ( signature >> 16 ) == 0xDECB ) {
	rev = ( int )( ( * ( unsigned __int64 * )( impure_ptr->cns$srom_srom_rev ) ) >> ( 8 * ( MAX_REV_LEVELS - 1 ) ) ) & 0xFF;
	if ( rev == ( int )( RCS_LOCK_SIGNATURE & 0xFF ) ) {
	    sprintf( srev++, "X" );
	}
	else if ( isalpha( rev ) ) {
	    sprintf( srev++, "%c", ( char )rev );
	}
	else if ( rev != ( int )( ~RCS_LOCK_SIGNATURE & 0xFF ) ) {
	    sprintf( srev, "?" );
	    return;
	}         
	for ( i = 0; i < MAX_REV_LEVELS; i++ ) {
	    rev = ( int )( ( * ( unsigned __int64 * )( impure_ptr->cns$srom_srom_rev ) ) >> ( 8 * i ) ) & 0xFF;
	    if ( rev == 0 ) break;
            increase = sprintf( srev, "%s%d", ( i == 0 ) ? "" : ".", rev );
            srev +=  increase;
	}   
    }
}


                            
/*******************************************************************************
 *                                                                             *
 *               Dynamic System Recognition Data Block Routines                *
 *                                                                             *
 ******************************************************************************/
           
#define LURT_COLS 9
#define NAME_SIZE 80
           
/*               
** LURT information for AlphaPC 264DP and AlphaServer DS20
*/               
#define PLATFORM_DP264_DSRDB \
    { 1838, 15, -1, -1, -1, -1, -1, -1, 1050, 1050, "AlphaPC 264DP %3d MHz" }
                                        
#define PLATFORM_DS20_SINGLE_CPU_DSRDB \
    { 1838, 25, -1, -1, -1, -1, -1, -1, 1050, 1050, "AlphaServer DS20 %3d MHz" }
                                      
#define PLATFORM_DS20_DUAL_CPU_DSRDB \
    { 1920, 75, -1, -1, -1, -1, -1, -1, 1050, 1050, "AlphaServer DS20 %3d MHz" }
                                 
const char XP2000[] = "COMPAQ AlphaStation DS20E %3d MHz";                            
const char DS20E[]  = "COMPAQ AlphaServer DS20E %3d MHz";       

/* EV6 single cpu */                                   
#define PLATFORM_DS20E_SINGLE_CPU_DSRDB \
    { 1840, 25, -1, -1, -1, -1, -1, -1, 1050, 1050, DS20E }
                                                         
/* EV6 dual cpu */                                       
#define PLATFORM_DS20E_DUAL_CPU_DSRDB \
    { 1921, 75, -1, -1, -1, -1, -1, -1, 1050, 1050, DS20E }
                                                          
/* EV67 single cpu */                                    
#define PLATFORM_DS20E67_SINGLE_CPU_DSRDB \
    { 1939, 25, -1, -1, -1, -1, -1, -1, 1050, 1050, DS20E }
                                                                          
/* EV67 dual cpu */                                                     
#define PLATFORM_DS20E67_DUAL_CPU_DSRDB \
    { 1940, 75, -1, -1, -1, -1, -1, -1, 1050, 1050, DS20E }   
                                                                         
/* EV6 single cpu */                                     
#define PLATFORM_XP2000_SINGLE_CPU_DSRDB \
    { 1941, 20, -1, -1, -1, -1, -1, -1, 1050, 1050, XP2000 }
                                                       
/* EV6 dual cpu */                                     
#define PLATFORM_XP2000_DUAL_CPU_DSRDB \
    { 1942, 70, -1, -1, -1, -1, -1, -1, 1050, 1050, XP2000 }
                                                          
/* EV67 single cpu */                                  
#define PLATFORM_XP200067_SINGLE_CPU_DSRDB \
    { 1943, 20, -1, -1, -1, -1, -1, -1, 1050, 1050, XP2000 }
                                                                          
/* EV67 dual cpu */                                                     
#define PLATFORM_XP200067_DUAL_CPU_DSRDB \
    { 1944, 70, -1, -1, -1, -1, -1, -1, 1050, 1050, XP2000 }   
                                                                         
struct {                                                       
    int smm;                                                   
    int lurt[LURT_COLS];                                       
    char *name;                                                
} dsrdb_n[] = { PLATFORM_DP264_DSRDB,                     /* 0 */
	     	PLATFORM_DS20_SINGLE_CPU_DSRDB,           /* 1 */       
	     	PLATFORM_DS20_DUAL_CPU_DSRDB,             /* 2 */       
	     	PLATFORM_DS20E_SINGLE_CPU_DSRDB,          /* 3 */       
	     	PLATFORM_DS20E_DUAL_CPU_DSRDB,            /* 4 */       
	     	PLATFORM_DS20E67_SINGLE_CPU_DSRDB,        /* 5 */       
	     	PLATFORM_DS20E67_DUAL_CPU_DSRDB,          /* 6 */      
	     	PLATFORM_XP2000_SINGLE_CPU_DSRDB,         /* 7 */       
	     	PLATFORM_XP2000_DUAL_CPU_DSRDB,           /* 8 */       
	     	PLATFORM_XP200067_SINGLE_CPU_DSRDB,       /* 9 */       
	     	PLATFORM_XP200067_DUAL_CPU_DSRDB };       /* 10 */      
                              
             
int dualCPU(void)
{            
int r=0;            
struct impure *impure_ptr;
uint32 signature;                 
uint32 proc_mask;                 
             
    impure_ptr = ( struct impure * )( PAL$IMPURE_BASE + PAL$IMPURE_COMMON_SIZE +
		    whoami( ) * PAL$IMPURE_SPECIFIC_SIZE );
    signature = *( uint64 * )( impure_ptr->cns$srom_signature );
    if ( ( signature >> 20 ) == 0xDEC ) {
	proc_mask = *( uint64 * )( impure_ptr->cns$srom_proc_mask );
	proc_mask &= ~( 1 << whoami( ) );
	if ( proc_mask != 0 ) {
	    r = 1;
	}
    }
  return r;
}
/*+                           
 * ============================================================================
 * = build_dsrdb - Build the dynamic system recognition data block            =
 * ============================================================================
 *      
 * OVERVIEW:
 *      
 *	Platform-specific code for building the dynamic system recognition
 *      data block in the HWRPB.
 *      
 * FORM OF CALL:
 *      
 *	build_dsrdb (hwrpb_ptr, offset); 
 *      
 * RETURNS:
 *      
 *      None
 *      
 * ARGUMENTS:
 *
 *      hwrpb  - Address of the HWRPB
 *      offset - Offset from HWRPB to where DSRDB will reside
 *
 * SIDE EFFECTS:
 *             
 *      None   
 *             
-*/            
build_dsrdb( struct HWRPB *hwrpb, int offset )                     
{                                                              
    int size;                                                      
    int i;                                                     
    __int64 *lurt;                                                     
    __int64 *name;                                                     
    struct DSRDB *dsrdb;
    char t[NAME_SIZE];
    int cpu_speed;                    
    int SysType; 
    struct FILE *fp;                     
                                      
    cpu_speed = krn$_timer_get_cycle_time( primary_cpu ) / 1000000;     
/*                                    
** Use the member id field from the system variation offset in the HWRPB to 
** determine the system type.         
*/                                    
    switch ( hwrpb->SYSVAR[0] >> 10 ) {
    	case 1:		    /* AlphaPC 264DP */
	    SysType = 0;              
	    break;                    
	case 6:		    /* AlphaServer DS20 */
	    SysType = dualCPU()?2:1;  
     	    break;                                    
	case 8:	       	    /* AlphaServer DS20E (aka Goldrack) */
            {                                         
               int type, xp2000, dual=dualCPU();      
                                                      
               fp = fopen("iic_rcm_nvram0", "srb" );  
               fseek( fp, 0x11, SEEK_SET);
               fread( &type , 1, 1, fp);             
               xp2000 = type & 1? 1:0;     /* Determine if Goldrack is an XP2000 or an DS20E */
               fclose(fp);                  
                                            
      	       type = ((int *)(PAL$IMPURE_BASE + 
                               IMPURE$CPU_TYPE_OFFSET + 4))[0];
      	       if ( type == 11 )       	        /* EV67 */            
                  if (xp2000)                                      
      	             SysType = dual?10:9;                      
                  else                                             
      	             SysType = dual?6:5;                      
               else                                                
                  if (xp2000)                                      
      	             SysType = dual?8:7;                      
                  else                          /* Assume EV6 */   
                     SysType = dual?4:3; 
            }                                                 
            break;                                            
      	default:                                              
      	    qprintf( "Error determining system type, SYSVAR = %x\n", hwrpb->SYSVAR[0] );
      	    qprintf( "Defaulting system type to AlphaPC 264DP\n" );
      	    SysType = 0;                                      
    }                                                         
/*                                                                 
** Save offset to DSRDB in HWRPB                              
*/                                                                 
    hwrpb->DSRDB_OFFSET[0] = offset;  
/*                       
** Allocate the DSRDB ...
*/                       
    dsrdb = ( int )hwrpb + offset;                  
    size = sizeof( *dsrdb ) + 8 + LURT_COLS * 8 + 8 + NAME_SIZE;
    memset( dsrdb, 0, size );       
/*                                         
** FIll in the SMM and set up LURT and name offsets ...
*/                                         
    dsrdb->SMM[0] = dsrdb_n[SysType].smm;
    dsrdb->LURT_OFFSET[0] = sizeof( *dsrdb );
    dsrdb->NAME_OFFSET[0] = sizeof( *dsrdb ) + 8 + LURT_COLS * 8;
/*                                         
** Set up the LURT information             
*/                                         
    lurt = ( int )dsrdb + dsrdb->LURT_OFFSET[0];
    *lurt++ = LURT_COLS;                   
    for( i = 0; i < LURT_COLS; i++ )       
	*lurt++ = dsrdb_n[SysType].lurt[i];
/*                                         
** Save the system name string             
*/                                         
    name = ( int )dsrdb +  dsrdb->NAME_OFFSET[0];
    sprintf( t, dsrdb_n[SysType].name, cpu_speed );
    *name++ = strlen( t );                 
    strcpy( name, t );                     

    return( size );                        
}                                      

int get_sysvar( void )
{
    struct FILE *fp;
    int sysvar = HWRPB$_SYSVAR;
/*
** Figure out a unique member id for the system variation by attempting 
** to open the temperature sensor device on the server management card.  
** This device in only present on the AlphaServer DS20.
**
**  Family ID	 Member ID      System
**   SYSTYPE	SYSVAR<15:10>   Name
**  ---------	-------------   ----------------
**     34	     1	        AlphaPC 264DP
**     34	     6	        AlphaServer DS20
**
*/
    if ( fp = fopen( "iic_ocp0", "sr+" ) ) {		  /* Goldrush */
	sysvar |= 6 << 10;                                                    
	fclose( fp );                                           
    }                                                           
    else  if ( fp = fopen( "iic_8574_ocp", "sr+" ) ) {    /* Goldrack */
 	sysvar |= 8 << 10;                                   
 	fclose( fp );                                      
    }                                                           
    else                                                  /* DP264 */
 	sysvar |= 1 << 10;
                      
    return( sysvar );
}                  
                  
/*******************************************************************************
 *                                                                             *
 *                    ISA Configuration Support Routines                       *
 *                                                                             *
 ******************************************************************************/
                   
/*+                
 * ============================================================================
 * = config_get_device - Return whether ISA device enabled                    =
 * ============================================================================
 *                 
 * OVERVIEW:       
 *                 
 *	The calling routine passes in the name of the ISA device, and this
 *	platform specific code will search for that device and return 1 if
 *      it is found and 0 if it is disabled or unavailable.
 *                 
 * FORM OF CALL:   
 *                 
 *	found = config_get_device( "devname" ); 
 *
 * RETURNS:                 
 *
 *      found - 1 if enabled
 *              0 if disabled or unavailable
 *
 * ARGUMENTS:
 *
 *      char *devname - device for which to search 
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
unsigned long config_get_device( char *devname )
{
    struct TABLE_ENTRY temp;
    int found;

    found = isacfg_get_device( devname, 1, &temp );
    if ( ( found ) && ( temp.device_enable == 1 ) )
	return 1;
    else
	return 0;
}


#if XDELTA_ON
/*******************************************************************************
 *                                                                             *
 *                         XDELTA Support Routines                             *
 *                                                                             *
 ******************************************************************************/

extern int scb;                
extern char BPT_SYNC[];
extern char EXCEPT_SYNC[];
int exe$gl_scb = 0;
int xdt$gl_scb = 0;
int mmg$gl_sptbase = 0;
int mmg$gl_va_to_vpn = -HWRPB$_PAGEBITS;
int xdt$gl_owner_id = -1;
int xdt$gl_benign_cpus = 0;
int xdt$gl_interlock = 0;
int smp$gl_primid;
int smp$gl_cpuconf = (1<<HWRPB$_NPROC)-1;
int exe$gl_state = 0;
int exe$gl_time_control = 0;

smp$intall_bit_acq( )
    {
    }

ini$writable( )
    {
    }

ini$rdonly( )
    {
    }

xdelta( )
    {
    int *pd;
    int do_bpt( );

    smp$gl_primid = primary_cpu;
    memset( BPT_SYNC, 0, 8 );
    memset( EXCEPT_SYNC, 0, 8 );
    exe$gl_scb = scb;
    xdt$gl_scb = dyn$_malloc( SCB_ENTRIES * SCB_ENTRY_SIZE,
	    DYN$K_SYNC|DYN$K_ALIGN, HWRPB$_PAGESIZE, 0, ( struct ZONE * )0 );
    pd = getpd( do_bpt );
    xdt$init( pd[2] );
    }

con$save_cty( )
    {
    }

con$restore_cty( )
    {
    }

con$checkchar( )
    {
    if (!(inportb(0, 0x3fd) & 1)) return 0;
    return (inportb(0, 0x3f8));
    }

con$getchar( )
    {
    return( jxgetc( ) );
    }

con$putchar( char c )
    {
    jputc( c );
    }
#else
fake_bpt( )
    {
    struct PCB *pcb;
    struct ALPHA_CTX *acx;

    pcb = getpcb( );
    acx = pcb->pcb$a_acp;
    pprintf( "breakpoint at PC %x desired, XDELTA not loaded\n", acx->acx$q_pc[0] );
    }

xdelta( )
    {
    int_vector_set( SCB$Q_BREAKPOINT, fake_bpt );
    }
#endif


/*******************************************************************************
 *                                                                             *
 *                     ARC/NT Firmware Support Routines                        *
 *                                                                             *
 ******************************************************************************/

#define SCB_FW_RCU_LOCATION "multi()disk()fdisk()swxcrmgr.exe"

struct ARCFW_SCB *arcfw_scb = ARCFW_SCB_ADDRESS;

arc_load_scb( int load_type, int use_graphics )
{
    int console_type;

    if ( graphics_console || use_graphics )
    	console_type = 0;
    else
    	console_type = SCB_COM1;

    switch ( load_type ) {
    	case FIRMWARE:
	    arcfw_scb->flags = console_type;
	    break;

	case RCU:
	    arcfw_scb->flags = console_type | SCB_APP_PATH;
	    strcpy( arcfw_scb->app_path, SCB_FW_RCU_LOCATION );
	    break;

    	default:
	    err_printf( "Unknown ARC SCB load type = %d\n", load_type );
    }
    arcfw_scb->signature = SCB_SIGNATURE;
    arcfw_scb->version = SCB_VERSION_3;
    arcfw_scb->platform_offset = 0;
    arcfw_scb->common_console_data = 0;
    arcfw_scb->hwrpb = PAL$HWRPB_BASE;
}   

int arc_to_srm( void )
{
    struct impure *impure_ptr;
    uint32 signature;
    struct ARCFW_SCB *fwscb;
    struct ARCFW_SCB_CCD *fwccd;
    unsigned int *argv;
    int i;
    struct HWRPB *hwrpb;
    struct SLOT *slot;

#if DEBUG
    char *arg;
    unsigned int *envp;
#endif

    impure_ptr = ( struct impure * )( PAL$IMPURE_BASE + PAL$IMPURE_COMMON_SIZE +
		    whoami( ) * PAL$IMPURE_SPECIFIC_SIZE );
    signature = *( uint64 * )( impure_ptr->cns$srom_signature );
#if DEBUG
    pprintf( "arc_to_srm: SROM signature...0x%x\n", signature );
    pprintf( "arc_to_srm: SROM proc_id.....0x%x.\n", *( uint64 * )( impure_ptr->cns$srom_proc_id ) );  
    pprintf( "arc_to_srm: SROM mem_size....0x%x.\n", *( uint64 * )( impure_ptr->cns$srom_mem_size ) );
    pprintf( "arc_to_srm: SROM cycle_cnt...0x%x.\n", *( uint64 * )( impure_ptr->cns$srom_cycle_cnt ) );
    pprintf( "arc_to_srm: SROM proc_mask...0x%x.\n", *( uint64 * )( impure_ptr->cns$srom_proc_mask ) );
    pprintf( "arc_to_srm: SROM sysctx......0x%x.\n", *( uint64 * )( impure_ptr->cns$srom_sysctx ) );      
#endif
    if ( ( signature >> 16 ) == 0xDECC ) {
    	fwscb = ( struct ARCFW_SCB * )( impure_ptr->cns$srom_sysctx[0] & 0x7FFFFFFF );
	if ( fwscb ) {
#if DEBUG
	    pprintf( "arc_to_srm: FW SCB @ 0x%x\n", fwscb );
	    pprintf( "arc_to_srm: FW SCB Signature....0x%x\n", fwscb->signature );
	    pprintf( "arc_to_srm: FW SCB Version......0x%x\n", fwscb->version );
	    pprintf( "arc_to_srm: FW SCB Flags........0x%x\n", fwscb->flags );
	    pprintf( "arc_to_srm: FW SCB CCD..........0x%x\n", fwscb->common_console_data );
#endif
	    if ( fwscb->flags & SCB_COMMON_CONSOLE_DATA ) {
		fwccd = ( struct ARCFW_CCD * )( fwscb->common_console_data & 0x7FFFFFFF );
		if ( fwccd ) {
#if DEBUG
		    pprintf( "arc_to_srm: FW CCD @ 0x%x\n", fwccd );
		    pprintf( "arc_to_srm: FW CCD Version......0x%x\n", fwccd->version );
		    pprintf( "arc_to_srm: FW CCD Argc.........0x%x\n", fwccd->argc );
		    argv = ( unsigned int * )( fwccd->argv & 0x7FFFFFFF );
		    for ( i = 0;  i < fwccd->argc;  i++ ) {
			arg = ( char * )( argv[i] & 0x7FFFFFFF );
			pprintf( "arc_to_srm: FW CCD Argv[%d].....%s\n", i, arg );
		    }
		    envp = ( unsigned int * )( fwccd->envp & 0x7FFFFFFF );
		    for ( i = 0;  envp[i] != 0;  i++ ) {
			arg = ( char * )( envp[i] & 0x7FFFFFFF );
			pprintf( "arc_to_srm: FW CCD Envp[%d].....%s\n", i, arg );
		    }
		    pprintf( "arc_to_srm: FW CCD Flags........0x%x\n", fwccd->flags );
#endif
		    if ( fwccd->flags & SCB_CCD_BOOT ) {
			hwrpb = HWRPB;
			slot = ( struct SLOT * )( ( unsigned int )hwrpb + *hwrpb->SLOT_OFFSET );
			for ( i = 0;  i < *hwrpb->NPROC;  i++, slot++ ) {
			    if ( ( i == primary_cpu ) && ( cpu_mask & ( 1 << i ) ) ) {
				slot->STATE.SLOT$V_HALT_REQUESTED = HALT_REQUEST$K_COLD_BOOT;
			    }
			}
			return( 1 );
		    }
		    else if ( fwccd->flags & SCB_CCD_INSTALL ) {
			return( 1 );
		    }
		}
	    }
	}
    }
    return( 0 );
}

/*
 * ============================================================================
 *   jump_to_arc - Jump to the ARC/NT Firmware				      =
 * ============================================================================
 *
 *  OVERVIEW:
 *
 *	Jump to ARC/NT firmware in PAL mode.
 *
 *  FORM OF CALL:
 *
 *	jump_to_arc( p )
 *
 *  RETURN CODES:
 *
 *	None
 *
 *  ARGUMENTS:
 *
 *	int p - starting address of ARC/NT firmware
 *
 *  SIDE EFFECTS:
 *
 *	None
 *
 */
jump_to_arc( int p )
{
    unsigned __int64 args[6];
    struct impure *impure_ptr;
    uint32 signature, sys_rev;

    args[0] = ( unsigned __int64 )p;
    args[1] = mem_size;
    args[2] = ( unsigned __int64 )krn$_psec_cycle( primary_cpu );

    impure_ptr = ( struct impure * )( PAL$IMPURE_BASE + PAL$IMPURE_COMMON_SIZE +
		    whoami( ) * PAL$IMPURE_SPECIFIC_SIZE );
    signature = *( uint64 * )( impure_ptr->cns$srom_signature );
    sys_rev = signature & 0x0000FFFF;
    signature = 0xDECC0000 | sys_rev;
    args[3] = ( unsigned __int64 )signature;

    args[4] = ( unsigned __int64 )cpu_mask;
    args[5] = ( unsigned __int64 )ARCFW_SCB_ADDRESS;
    cserve( CSERVE$JUMP_TO_ARC, p | 1, args );
}
   
/* 
 * ============================================================================
 *   load_arc_fw - Load the ARC/NT Firmware                                   =
 * ============================================================================
 *
 *  OVERVIEW:
 *
 *	Load the ARC/NT firmware from flash ROM or from a floppy diskette.
 *
 *  FORM OF CALL:
 *
 *	load_arc_fw(  )
 *
 *  RETURN CODES:
 *
 *	msg_success or msg_failure
 *
 *  ARGUMENTS:
 *
 *	None
 *
 *  SIDE EFFECTS:
 *
 *	None
 *
 */
load_arc_fw( ) 
{
    int i, j;
    int dest;
    struct FILE *fp;
    romheader_t hdr;
    unsigned int checksum;
    char *srcname[] = { "ntrom", "fat:dp264nt.rom/dva0", NULL };
    struct PCB *pcb;
    int year;
                                              
    start_drivers( );                         
    for ( i = 0; srcname[i] != NULL; i++ ) {  
   	if ( ( fp = fopen( srcname[i], "r" ) ) == NULL ) continue;
   	do {              
   	    /* Read the ROM header */
   	    if ( fread( &hdr, sizeof( hdr ), 1, fp ) != sizeof( hdr ) ) {
   		err_printf ( "Error reading ROM header\n" );
   		break;    
   	    }
   	    /* Validate the ROM header */
   	    if ( (hdr.romh.V0.signature != ROM_H_SIGNATURE) || (hdr.romh.V0.csignature != ~ROM_H_SIGNATURE) ) {
   		err_printf ( "ROM header signature does not match expected pattern %08X %08X\n",
			    ROM_H_SIGNATURE, ~ROM_H_SIGNATURE );
		break;
	    }
	    /* Read the ROM image */
	    fseek( fp, hdr.romh.V0.hsize, SEEK_SET );
	    dest = hdr.romh.V0.destination.low;
	    if ( fread( dest, hdr.romh.V0.size, 1, fp ) != hdr.romh.V0.size ) {
		err_printf ( "Error reading ROM image\n" );
		break;
	    }
	    /* Validate the ROM image checksum */
	    checksum = 0;
	    for ( j = 0; j < hdr.romh.V0.size; j++ )
		COMPUTE_CHECKSUM( *(unsigned char *)(dest + j), checksum );
	    if ( checksum != hdr.romh.V0.checksum ) {
		err_printf ( "Computed ROM image checksum 0x%04X does not match header checksum 0x%04X\n",
			    checksum, hdr.romh.V0.checksum );
		break;
	    }
	    /* Jump and execute the image ... */
	    fclose( fp );
	    pcb = getpcb( );
	    pcb->pcb$l_affinity = 1 << primary_cpu;
	    reschedule( 1 );
	    stop_drivers( );

	    /* Reset clock for NT */
	    year = rtc_read( 0x09 );
	    if ( ( year >= 93 ) && ( year <= 99 ) )
	    	year -= 80;
	    else if ( ( year >= 0 ) && ( year <= 12 ) )
	    	year += 20;
	    ( void )rtc_write( 0x09, year );

	    for ( j = 0;  j < MAX_PROCESSOR_ID;  j++ ) {
	    	if ( j != primary_cpu ) {
                    krn$_create( jump_to_arc, null_procedure, 0, 7, 1 << j, 0,
                    "jump_to_arc", 0, 0, 0, 0, 0, 0, dest );
		}
	    }
	    jump_to_arc( dest );
	} while ( FALSE );
	fclose ( fp );
    }
    return ( msg_failure );
}

/*
 * ============================================================================
 *   arc - Transfer control to ARC/NT firmware				      =
 * ============================================================================
 *
 *  OVERVIEW:
 *
 *	Load the NT firmware from the flash ROM or floppy diskette into
 *	memory and jump to it.
 *
 *  COMMAND FMT: arc 2 Z 16 arc
 *
 *  COMMAND FORM:
 *
 *	    arc ( )
 *
 *  COMMAND TAG: arc 0 RXBZ arc nt alphabios
 *
 *  FORM OF CALL:
 *
 *	arc( argc, *argv[] );
 *
 *  RETURN CODES:
 *
 *	msg_success or msg_failure
 *
 *  ARGUMENTS:
 *
 *	    int argc - number of command line arguments passed by shell
 *	char *argv[] - array of pointers to arguments
 *
 *  SIDE EFFECTS:
 *
 *	None
 *
 */
arc( int argc, char *argv[] )
{
    int graphics;

    if ( ( argc == 2 ) && ( argv[1][1] == 'g' ) )
	graphics = 1;
    else
	graphics = 0;

    arc_load_scb( FIRMWARE, graphics );
    return( load_arc_fw( ) );
}

/*
 * ============================================================================
 *   rcu - Transfer control to RAID Configuration Utility		      =
 * ============================================================================
 *
 *  OVERVIEW:
 *
 *	Loads the NT firmware from the flash ROM and invokes the RAID
 *	Configuration Utility (RCU).
 *
 *  COMMAND FMT: rcu 2 Z 16 rcu
 *
 *  COMMAND FORM:
 *
 *	    rcu ( )
 *
 *  COMMAND TAG: rcu 0 RXBZ rcu
 *
 *  FORM OF CALL:
 *
 *	rcu( argc, *argv[] );
 *
 *  RETURN CODES:
 *
 *	msg_success or msg_failure
 *
 *  ARGUMENTS:
 *
 *	    int argc - number of command line arguments passed by shell
 *	char *argv[] - array of pointers to arguments
 *
 *  SIDE EFFECTS:
 *
 *	None
 *
 */
rcu( int argc, char *argv[] )
{
    int graphics;

    if ( ( argc == 2 ) && ( argv[1][1] == 'g' ) )
    	graphics = 1;
    else
    	graphics = 0;

    arc_load_scb( RCU, graphics );
    return( load_arc_fw( ) );
}

/*
 * COMMAND TAG: jtopal 0 RXBZ jtopal
 */
jtopal( int argc, char *argv[] )
{
    if ( argc == 2 ) {
	arc_load_scb( FIRMWARE, 0 );
	jump_to_arc( xtoi( argv[1] ) );
    }
}


/*******************************************************************************
 *                                                                             *
 *                    Server Management Support Routines                       *
 *                                                                             *
 ******************************************************************************/

#define THRESHOLD_CSR 0xA1
#define HYSTERESIS_CSR 0xA2
#define READ_AMBIENT 0xAA
#define CONTINUOUS_MODE 0xEE

#define ENVIRON_ENTRY_SIZE 6

struct environ_data_header {
    int	checksum;
    short int type;
    short int rev;
    int	length;
    int	count;
};

int ev_wr_shutdown_temp( char *name, struct EVNODE *ev ) 
{
    unsigned char data[4];
    struct FILE *fp;

    if ( fp = fopen( "iic_rcm_temp", "sr+" ) ) {
	data[0] = THRESHOLD_CSR;
	data[1] = atoi( ev->value.string );  /* Get user-specified temperature */
	data[2] = 0;                         /* We won't support half degrees */
	data[3] = 0;
	fwrite( data, 4, 1, fp );            /* Write the threshold register */
        fclose( fp );
	fp = fopen( "iic_rcm_temp", "sr+" );
	data[0] = HYSTERESIS_CSR;
	data[1] -= 1;                        /* Set hysteresis 1 degree less */
	fwrite( data, 4, 1, fp );            /* than shutdown threshold */
        fclose( fp );
    } 
    else {
	return( msg_failure );
    }
    if ( ev_initing )
	return( msg_success );
    return( ev_sev( name, ev ) );
}

int ev_rd_shutdown_temp( char *name, struct EVNODE *ev, struct EVNODE *ev_copy ) 
{
    if ( !ev_initing )
	strcpy( ev_copy->value.string, ev->value.string );
    return( msg_success );
}

int validate_temp_value(int *value) 
{
    int temp;
    unsigned char data[1];
    struct FILE *fp;

    if ( fp = fopen( "iic_rcm_temp", "sr+" ) ) {
	data[0] = CONTINUOUS_MODE;
	fwrite( data, 1, 1, fp );
	data[0] = READ_AMBIENT;
	fwrite( data, 1, 1, fp );
	fread( data, 1, 1, fp );             /* Get ambient temperature.. */
        fclose( fp );
    } 
    else {
	err_printf( "Error reading ambient temperature\n" );
	return( msg_failure );
    }
    temp = atoi( *value );
/*
 * Don't let user select threshold temperature unless 10 degrees C more
 * than current ambient.. Maximum threshold value supported by is 127 
 * degrees C.
 */
    if ( temp < data[0] + 10 ) {
	err_printf( "Shutdown temp must be 10 or more degrees above current ambient temp (%d degrees C)\n", data[0] );
	return( msg_failure );
    }
    if ( temp > 127 ) {
	err_printf( "Shutdown temp must be less than 128 degrees C\n" );
	return( msg_failure );
    }
    return( msg_success );
}

void print_environ_data( __int64 data )
{
    unsigned int i;
    unsigned int found = 0;
    char *c = &data;
    static char *months[ ] = {
	"000", 
	"JAN", "FEB", "MAR", "APR", 
	"MAY", "JUN", "JUL", "AUG",
	"SEP", "OCT", "NOV", "DEC" };

    for ( i = 0;  i < 4;  i++ ) {
    	if ( c[2 + i] < 0 )
	    c[2 + i] = 0;
    }
    if ( c[5] > 12 )
    	c[5] = 0;

    printf( "%s %2d %2d:%02d  ", months[c[5]], c[4], c[3], c[2] );

    if ( !( c[0] & 1 ) ) {
    	printf( "Temperature Failure" );
	found++;
    }
    if ( !( c[0] & 2 ) ) {
    	int fan;
	static char *fan_fail[ ] = { "0,1", "1", "0" };
	fan = ( ( c[1] & 0x20 ) >> 5 ) | ( c[1] & 0x2 );
	if ( fan != 0x3 ) {
	    printf( "%sSystem Fan %s Failure", ( found ? ", " : "" ), fan_fail[fan] );
	    found++;
	}
    }
    if ( !( c[0] & 8 ) ) {
    	int fan;
	static char *fan_fail[ ] = { "0,1", "1", "0" };
	fan = ( ( c[1] & 0x20 ) >> 5 ) | ( c[1] & 0x2 );
	if ( fan != 0x3 ) {
	    printf( "%sCPU Fan %s Failure", ( found ? ", " : "" ), fan_fail[fan] );
	    found++;
	}
    }
    if ( c[0] & 0x10 ) {
	if ( !( c[1] & 0x10 ) ) {
	    printf( "%sFan Tray Failure", ( found ? ", " : "" ) );
	    found++;
	}
    }
    if ( c[0] & 0xe0 ) {
    	int ps = c[0]>>5;                                                               
	static char *ps_fail[ ] = { "", "0", "1", "0,1", "2", "0,2", "1,2", "0,1,2" };
                                             
        unsigned int presence_mask = !(c[1] & 0x80);	 /* Power Supply 0 present? */ 
	presence_mask |= !(c[1] & 0x10) << 1;          /* Power Supply 1 present? */ 
	presence_mask |= !(c[1] & 0x40) << 2;          /* Power Supply 2 present? */ 

        found = ps & presence_mask;                            
        printf( "Power Supply %s Failure", ps_fail[found] );
    }                                                    
    if ( !found )
    	printf( "Temperature, Fans, Power Supplies Normal" );
    printf( "\n" );
}


/*+
 * ============================================================================
 * = show power - Display power supply and fan status.                        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will display the power supply and fan status.
 *
 * COMMAND FORM:
 *
 *	show power ( )
 *                                                                                           
 * COMMAND ARGUMENT(S):                                                                      
 *          
 *	None
 *
 * COMMAND OPTION(S):
 *
 *	None                                 
 *
 * COMMAND EXAMPLE(S):
 *
 *~
 *	P00>>>show power
 *
 *                          Status
 *	Power Supply 0       good
 *	Power Supply 1	     good
 *	System Fans          good
 *	CPU Fans             good
 *	Temperature          good
 *~
 *
 * FORM OF CALL:
 *
 *	show_power( argc, *argv[] )
 *
 * RETURNS:
 *         
 *	None.
 *
 * ARGUMENTS:
 *
 *	int argc - number of command line arguments passed by the shell
 *	char *argv [] - array of pointers to arguments
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/
void show_power( int argc, char *argv[] )
{                                    
    struct FILE *fp;                 
    unsigned char fail_register;     
    unsigned char func_register;     
    int status;                      
    unsigned int i;                                           
    unsigned char ps_present[ ] = { 7, 4, 6 };                
    unsigned char ps_ok[ ] = { 5, 6, 7 };                     
    unsigned char fan_ok[ ] = { 5, 1 };                       
    unsigned char temp[1];                                    
    struct environ_data_header header;                        
    char buf[80];                                             
    int events;                                               
    int nbytes;                                               
    __int64 data;
    char *ptr;
    int goldrack;
                
    if ( fp = fopen( "iic_8574_ocp", "sr+" ) ) 
       goldrack = 1;
    else
       goldrack = 0;
    fclose(fp);     

/*
**
** Failing Fan and Power Supply Register
** -------------------------------------
**
** Located at address 0111000 on the I2C bus, this register
** decodes as follows (LOW = low asserted):
** 
**	+---+---+---+---+---+---+---+---+
**	| 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 | : 70h
**	+---+---+---+---+---+---+---+---+
**	  |   |   |   |   |   |   |   |
**	  |   |   |   |   |   |   |   +----> Pull up
**        |   |   |   |   |   |   +--------> CPU 1 Fan/System Fan 1 (LOW)
**	  |   |   |   |   |   +------------> Pull up
**	  |   |   |   |   +----------------> Pull up
**	  |   |   |   +--------------------> Power Supply 1 or Fan Tray Present (LOW)
**	  |   |   +------------------------> CPU 0 Fan/System Fan 0 (LOW)
**	  |   +----------------------------> SFM2 Power Supply 2 Present (LOW)
**	  +--------------------------------> Power Supply 0 Present (LOW)
*/                            
    if ( ( fp = fopen( "iic_system0", "sr" ) ) == NULL ) {
	return;                                         
    }                                                   
    status = fread( &fail_register, 1, 1, fp );         
    fclose( fp );                                       
    if ( !status ) {                                    
	printf( "Error reading failing fan/power supply status\n" );
	return;                                         
    }   
/*      
**      
** Fan, Temperature, and Power Supply Function Register
** ----------------------------------------------------
**      
** Located at address 0111001 on the I2C bus, this register
** decodes as follows (LOW = low asserted):
**      
**	+---+---+---+---+---+---+---+---+
**	| 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 | : 72h
**	+---+---+---+---+---+---+---+---+
**	  |   |   |   |   |   |   |   |
**	  |   |   |   |   |   |   |   +----> Temp OK
**        |   |   |   |   |   |   +--------> System Fan OK
**	  |   |   |   |   |   +------------> Tied to GND
**	  |   |   |   |   +----------------> CPU Fans OK
**	  |   |   |   +--------------------> Fan Tray Failed (SFM2-- pulled Down)
**	  |   |   +------------------------> Power Supply 0 OK (LOW)
**	  |   +----------------------------> Power Supply 1 OK (LOW)
**	  +--------------------------------> SFM2 Power Supply 2 OK (LOW)
*/      
    if ( ( fp = fopen( "iic_system1", "sr" ) ) == NULL ) {
   	return;
    }   
    status = fread( &func_register, 1, 1, fp );
    fclose( fp );
    if ( !status ) {
	printf( "Error reading fan, temp, and power supply function status\n" );
	return;
    }   
/*      
** Display the status of the power supplies ...
*/      
    printf( "\n                        Status\n" );
    for ( i = 0;  i < 3;  i++ ) {     
        printf( "Power Supply %d%s", i, i && !goldrack ? "/Fan Tray" : "         " );
    	if ( ( fail_register & ( 1 << ps_present[i] ) ) ) {
	    printf( "  not present\n" );          
	}                                         
	else {                                    
	    if ( !( func_register & ( 1 << ps_ok[i] ) ) )
		printf( "  good\n" );                      
	    else                     
		printf( "* BAD *\n" );
	}
    }   
/*      
** Display the status of the system fans ... 
*/
    if ( func_register & ( 1 << 1 ) ) {
	printf( "System Fans            " );
	printf( "  good\n" );
    }
    else {
    	for ( i = 0;  i < 2;  i++ ) {
	    printf( "System Fan %d           ", i );
	    if ( fail_register & ( 1 << fan_ok[i] ) )
		printf( "  good\n" );
	    else
		printf( "* BAD *\n" );
	}
    }
/*
** Display the status of the CPU fans ...
*/
    if ( func_register & ( 1 << 3 ) ) {
	printf( "CPU Fans               " );
	printf( "  good\n" );
    }
    else {
    	for ( i = 0;  i < 2;  i++ ) {
	    printf( "CPU Fan %d              ", i );
	    if ( fail_register & ( 1 << fan_ok[i] ) )
		printf( "  good\n" );
	    else
		printf( "* BAD *\n" );
	}
    }
/*
** Display the temperature status ...
*/
    printf( "Temperature            " );
    if ( fail_register & ( 1 << 0 ) )
	printf( "  good\n" );
    else
	printf( "* BAD *\n" );	
/*
** Display the current ambient temperature ...
*/
    if ( ( fp = fopen( "iic_rcm_temp", "sr+" ) ) != NULL ) {
    	temp[0] = CONTINUOUS_MODE;
	fwrite( temp, 1, 1, fp );
	temp[0] = READ_AMBIENT;
	fwrite( temp, 1, 1, fp );
	status = fread( temp, 1, 1, fp );
	if ( !status )
	    printf( "Error reading current ambient temperature\n" );
	else
	    printf( "\nCurrent ambient temperature is %d degrees C\n", temp[0] );
	fclose( fp );
    }
/*
** Display the system shutdown temperature ...
*/
    if ( ( fp = fopen( "iic_rcm_temp", "sr+" ) ) != NULL ) {
    	temp[0] = THRESHOLD_CSR;
	fwrite( temp, 1, 1, fp );
	status = fread( temp, 1, 1, fp );
	if ( !status )
	    printf( "Error reading system shutdown temperature\n" );
	else
	    printf( "System shutdown temperature is set to %d degrees C\n\n", temp[0] );
	fclose( fp );
    }
/*
** Display any environmental events that are logged in NVRAM
*/
    if ( ( fp = fopen( "iic_rcm_nvram6", "sr+" ) ) != NULL ) {
    	status = fread( &header, 1, sizeof( struct environ_data_header ), fp );
	if ( !status ) {
	    printf( "Error reading environmental event data\n" );
	}
	else {
	    events = min( header.count, ( header.length - sizeof( struct environ_data_header ) ) / ENVIRON_ENTRY_SIZE ); 
	    printf( "%d Environmental events are logged in nvram\n", events );
	    if ( header.count ) {
		read_with_prompt( "Do you want to view the events? (Y/<N>) ", sizeof( buf ), buf, 0, 0, 1 );
		if ( toupper( buf[0] ) == 'Y' ) {
		    printf( "\nTotal Environmental Events: %d (%d logged)\n\n", header.count, events );
		    for ( i = 0;  i < events;  i++ ) {
			nbytes = fread( &data, 1, ENVIRON_ENTRY_SIZE, fp );
			if ( ( nbytes != ENVIRON_ENTRY_SIZE ) && 
			     ( ( i * ENVIRON_ENTRY_SIZE + sizeof( struct environ_data_header ) + nbytes ) == 256 ) )
			{
			    ptr = &data;
			    fclose( fp );
			    if ( ( fp = fopen( "iic_rcm_nvram7", "rw" ) ) != NULL ) 
				nbytes += fread( &ptr[nbytes], 1, ENVIRON_ENTRY_SIZE - nbytes, fp );
			}
			if ( nbytes == ENVIRON_ENTRY_SIZE ) {
			    printf( "%d  ", i + 1 );
			    print_environ_data( data );
			}
		    }
		}
		read_with_prompt( "\nDo you want to clear all events from nvram? (Y/<N>) ", sizeof( buf ), buf, 0, 0, 1 );
		if ( toupper( buf[0] ) == 'Y' ) {
		    header.checksum = 0;
		    header.type = 0x57;
		    header.rev = 1;
		    header.length = 0x200;
		    header.count = 0;
		    fclose( fp );
		    if ( ( fp = fopen( "iic_rcm_nvram6", "rw" ) ) != NULL ) {
			fwrite( &header, 1, sizeof( struct environ_data_header ), fp );
		    }
		}
	    }
	}
	fclose( fp );
    }
}


/*+
 * ============================================================================
 * = show cpu - Display the status of each CPU.                               =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will display the status of each CPU in the system.
 *
 * COMMAND FORM:
 *
 *	show cpu ( )
 *                    
 * COMMAND ARGUMENT(S):
 *          
 *	None
 *
 * COMMAND OPTION(S):
 *
 *	None                                 
 *
 * COMMAND EXAMPLE(S):
 *
 *~
 *	P00>>>show cpu
 *
 *	Primary CPU:	    00
 *	Active CPUs:	    00	    01
 *	Configured CPUs:    00	    01
 *	SROM Revision	    V3.0    V3.0
 *~
 *
 * FORM OF CALL:
 *
 *	show_cpu( argc, *argv[] )
 *
 * RETURNS:
 *         
 *	None.
 *
 * ARGUMENTS:
 *
 *	int argc - number of command line arguments passed by the shell
 *	char *argv [] - array of pointers to arguments
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/

void show_cpu( int argc, char *argv[] )
{
    int i;
    char rev[10];

/* Print out the id of the primary CPU */

    printf( "\nPrimary CPU:      %02d", primary_cpu );

/* Print out active CPUs from in_console mask */

    printf( "\nActive CPUs:      " );
    for ( i = 0;  i < MAX_PROCESSOR_ID;  i++ ) {
    	if ( ( in_console >> i ) & 1 ) {
	    printf( "%02d     ", i );
	}
    }

/* Print out configured CPUs from cpu_mask */

    printf( "\nConfigured CPUs:  " );
    for ( i = 0;  i < MAX_PROCESSOR_ID;  i++ ) {
    	if ( ( cpu_mask >> i ) & 1 ) {
	    printf( "%02d     ", i );
	}
    }

/* Print out each CPU's SROM revision */
    
    printf( "\nSROM Revision:    " );
    for ( i = 0;  i < MAX_PROCESSOR_ID;  i ++ ) {
    	if ( ( cpu_mask >> i ) & 1 ) {
	    get_srom_revision( &rev, i );
	    printf( "%5s  ", rev );
	}
    }
    printf( "\n\n" );
}


halt_switch_in( )
{
    int halt_status;

    halt_status = ( *( int * )PAL$HALT_SWITCH_IN );
    *( int * )PAL$HALT_SWITCH_IN = 0;
    return( halt_status );
}
