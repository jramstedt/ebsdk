/* file:	entry.c
 *
 * Copyright (C) 1990 by
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
 *      Console entry service
 *
 *  AUTHORS:
 *
 *      Kevin LeMieux
 *
 *  CREATION DATE:
 *  
 *      16-Apr-1991
 *
 *  MODIFICATION HISTORY:
 *
 *	tna     11-Nov-1998     Conditionalized for YUKONA
 *
 *	mar	09-Oct-1998	system_reset_or_error() - remove conditional
 *                              implemented 2-October. The better solutions was
 *				to increase the stack size to 6K. See
 *				powerup_mikasa.c.
 *
 *	mar	02-Oct-1998	system_reset_or_error()
 *				Conditionalized for Corelle, Pinkasa to use same
 *	                        algorithm in gen=162. The problem was with new
 *				algorithm (gen=163) was that with BOOT_RESET set
 *				to ON, the console would hang trying to boot
 *				a device (CD or Disk).
 *
 *	twp	30-Mar-1996	Conditionalized for RHMIN
 *
 *	jje	21-Feb-1996	Conditionalized for Cortex
 *
 *	er	19-Dec-1995	Conditionalize for EB164
 *
 *	sew	24-Oct-1995	Added clear VGA interrupt mask prior to
 *				booting. Conditionalize for AVMIN.
 *
 *	er	27-Dec-1994	Conditionalize for EB66
 *
 *	er	15-Aug-1994	Conditionalize for SD164
 *
 *	jmp	11-29-93	Add avanti support
 *
 *	kl	23-Aug-1991	External and node reset entries post the
 *				entry process.
 *
 *	kl	27-Jun-1991	Update to Console SRM ECO 3.1
 *
 *	jds	04-Jun-1991	Tweaked VAX callback entry dispatch code.
 *
 *--
 */

#define SECONDARY_TIMEOUT 10000
#define NODE_HALT_TIME 1000

#include "cp$src:platform.h"
#include "cp$src:prdef.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:common.h"
#include "cp$src:stddef.h"
#include "cp$src:msg_def.h"
#include "cp$src:ev_def.h"
#include "cp$src:hwrpb_def.h"
#include "cp$src:combo_def.h"
#include "cp$src:pal_def.h"
#include "cp$src:impure_def.h"
#include "cp$src:aprdef.h"
#include "cp$src:get_console_base.h"
#include "cp$inc:kernel_entry.h"
#include "cp$inc:prototypes.h"

#if EV6
extern int page_table_levels;
#endif

#if SABLE
#if GAMMA
#include "cp$src:gamma_cpu.h"
#else
#include "cp$src:cbr_sbl_cpu.h"
#endif
#endif

#if TURBO
#include "cp$src:emulate.h"
#endif

#if MODULAR
#if !TURBO 
#undef real_slot
#define real_slot(x) x
#endif
#endif

#if MONET
int read_boot_args();
extern int expected_entry;
#endif

#if AVANTI
extern void outportb (struct pb *pb,unsigned __int64 addr,unsigned char data);
#endif

#if !(EV5 || EV6)
#define EV4 1
#endif

#define ref_quad(x) (&(__int64)(x))

extern int primary_cpu;
extern int in_console;
extern int cpu_mask;
#if MIKASA || RAWHIDE
extern int com1_baud;
extern int com2_baud;
#endif
#if RAWHIDE
extern int cpu_available;
#endif
extern struct HWRPB *hwrpb;
extern char *entry_boottype;

#if MODULAR
extern struct TTPB *hwrpbtt_ttpbs[];
extern int hwrpbtt_inited;
#endif

#if GALAXY
extern int galaxy_partitions;
extern int galaxy_node_number;
extern int all_cpu_mask;
#endif

extern null_procedure( );

void sec_init( struct TIMERQ *tqe );
void init_cpu_state_all_cpus( );
void node_halt_secondaries( );
void node_halt_primary( );
void restart_cpu( int haltcode );
void secondary_start( );
void boot_system( char *boottype );
void system_reset_or_error( uint32 state_l, uint32 state_h, int is_primary );
void entry( );

#if TURBO || RAWHIDE
void secondary_swappal( );
#endif

void sec_init( struct TIMERQ *tqe )
    {
    int id;
    int v[2];
    struct SLOT *slot;
    struct HWRPB *hwrpb_ptr;

    id = whoami( );

#if MODULAR && !RHMIN
/* Build this cpu's hwrpb slot */

    build_slot(hwrpb, real_slot(id));

    if ( !hwrpbtt_inited || !hwrpbtt_ttpbs[id]) {
        hwrpbtt_init(id);
    }
#endif

    hwrpb_ptr = hwrpb;
#if TURBO
    /*
     * EMULATION HACK! Turbo MP uses real HWRPB Slots, not emulated ones
    */
    slot = (int)hwrpb_ptr + *hwrpb_ptr->SLOT_OFFSET + *hwrpb_ptr->SLOT_SIZE * (real_slot(id));
#else
    slot = (int)hwrpb_ptr + *hwrpb_ptr->SLOT_OFFSET + *hwrpb_ptr->SLOT_SIZE * id;
#endif
    memset( &slot->HWPCB, 0, sizeof( slot->HWPCB ) );
    *(INT *)slot->HALT_PCBB = virt_to_phys( slot );
    *(INT *)slot->HALT_PC = 0;
    *(INT *)slot->HALT_PS = 0x1f00;
    *(INT *)slot->HALT_ARGLIST = 0;
    *(INT *)slot->HALT_RETURN = 0;
    *(INT *)slot->HALT_VALUE = 0;
    *(INT *)slot->HALTCODE = 0;
    slot->STATE.SLOT$V_RC = 0;
    slot->STATE.SLOT$V_OH = 0;
    slot->STATE.SLOT$V_CV = 0;
    slot->STATE.SLOT$V_BIP = 0;	
    slot->STATE.SLOT$V_HALT_REQUESTED = 0;
    slot->PAL_REV[0] = slot->PAL_REV_AVAIL[0][2];
    slot->PAL_REV[1] = slot->PAL_REV_AVAIL[0][3];
    *(INT *)v = pal_virt_to_phys( id, PAL$PAL_BASE );
    write_ipr( IBX$K_PAL_BASE, v );
#if EV4
    read_ipr( ABX$K_ABOX_CTL, v );
    v[0] &= ~0x20;
#if SABLE
    v[0] |= 0x400;
#endif
    write_ipr( ABX$K_ABOX_CTL, v );
    read_pt( PT$_ICCSR, v );
    v[1] &= ~0x200;
    write_pt( PT$_ICCSR, v );
#if SABLE
    if( primary( ) )
	write_pt( PT$_INTTABLE, ref_quad( PAL$IPL_TABLE_BASE ) );
    else
	write_pt( PT$_INTTABLE, ref_quad( PAL$IPL_TABLE_BASE + 0x200 ) );
#else
    write_pt( PT$_INTTABLE, ref_quad( PAL$IPL_TABLE_BASE ) );
#endif
    write_pt( PT$_UNQ, ref_quad( 0 ) );
#endif
#if EV5
    read_ipr( APR$K_ICSR, v );
    v[0] &= ~0x20000000;
    write_ipr( APR$K_ICSR, v );
    read_ipr( APR$K_MCSR, v );
    v[0] &= ~0x4;
    write_ipr( APR$K_MCSR, v );
#endif
#if EV6
    set_spe( 0 );
    set_va_48( page_table_levels == 4 );
#endif
#if !EV6
    write_pt( PT$_SCC, ref_quad( 0 ) );
    *(INT *)v = virt_to_phys( PAL$IMPURE_BASE
	     + PAL$IMPURE_COMMON_SIZE + id * PAL$IMPURE_SPECIFIC_SIZE );
    write_pt( PT$_IMPURE, v );
#endif
    write_ipr( APR$K_PCBB, slot->HALT_PCBB );
    write_ipr( APR$K_PS, slot->HALT_PS );
    write_ipr( APR$K_ASN, ref_quad( 0 ) );
    write_ipr( APR$K_ASTEN, ref_quad( 0 ) );
    write_ipr( APR$K_ASTSR, ref_quad( 0 ) );
    write_ipr( APR$K_FEN, ref_quad( 0 ) );
    write_ipr( APR$K_MCES, ref_quad( 15 ) );
    write_ipr( APR$K_SISR, ref_quad( 0 ) );
#if SEPARATE_PAGE_TABLES
    write_ipr( APR$K_SYSPTBR, ref_quad( 0 ) );
    write_ipr( APR$K_VIRBND, ref_quad( -1 ) );
#endif

#if WILDFIRE || CLIPPER
    build_halt_log( hwrpb, id, slot->CONSOLE_DATA_LOG );
#endif

#if SABLE
    reset_cpu( id );
#endif
	  
    /*
     *  Post the semaphore to signal initialization complete.
     */
    if (tqe) krn$_post( &tqe->sem );
    }

void init_cpu_state_all_cpus( )
    {
    int i;
    struct TIMERQ *tqe;
#if GALAXY
    struct SLOT *slot;
    struct HWRPB *hwrpb_ptr;
#endif

    /*
     *  Get the secondaries into console mode.
     */
    node_halt_secondaries( );

    /*
     *  Allocate a timer queue element.
     */
    tqe = malloc( sizeof( *tqe ) );
    krn$_seminit( &tqe->sem, 0, "init_cpu" );
    krn$_init_timer( tqe );

    /*
     *  Visit each CPU in console mode.
     */ 
    for( i = 0; i < MAX_PROCESSOR_ID; i++ )
	if( in_console & ( 1 << i ) )
	    {
	    tqe->sem.count = 0;
	    krn$_create( sec_init, null_procedure, 0, 4, 1 << i, 0,
		    "sec_init", "tt", "r", "tt", "w", "tt", "w", tqe );

	    /*
	     *  Start the timer.
	     */
	    krn$_start_timer( tqe, SECONDARY_TIMEOUT );
	    krn$_wait( &tqe->sem );

	    /*
	     *  Stop the timer.
	     */
	    krn$_stop_timer( tqe );
	    }
#if GALAXY
        else if (( all_cpu_mask & ( 1 << i ) ) && hwrpb){
            build_slot(hwrpb,i);
	    hwrpb_ptr = hwrpb;
	    slot = (int)hwrpb_ptr + *(UINT *)hwrpb_ptr->SLOT_OFFSET + 
		    *(UINT *)hwrpb_ptr->SLOT_SIZE * i;
	    slot->STATE.SLOT$V_PA = 0;	/* not available */
        }
#endif

    /*
     *  Release the semaphore.
     */
    krn$_semrelease( &tqe->sem );

    /*
     *  Release the timer.
     */
    krn$_release_timer( tqe );

#if MODULAR
    hwrpbtt_inited = 1;		/* Flag HWRPB TT initialization complete */
#endif

    }

void node_halt_secondaries( )
    {
    int not_primary;
    int secondary_not_in_console;
    int i;
    int cpu;

#if RAWHIDE
    cpu = cpu_available;
#else
    cpu = cpu_mask;
#endif

    not_primary = ~( 1 << whoami() );
    secondary_not_in_console = ( in_console ^ cpu ) & not_primary;
    for( i = 0; i < MAX_PROCESSOR_ID; i++ )
	if( secondary_not_in_console & ( 1 << i ) )
	    node_halt( i );
    for( i = 0; i < 10; i++ )
	if( ( in_console ^ cpu ) & not_primary )
	    krn$_sleep( NODE_HALT_TIME );
	else
	    break;
    }

void node_halt_primary( )
    {
    node_halt( primary_cpu );
    }

void restart_cpu( int haltcode )
    {
    int id;
    int v[2];
    struct SLOT *slot;
    struct HWRPB *hwrpb_ptr;

    id = whoami( );
    pprintf( "\nCPU %d restarting\n\n", id );
    hwrpb_ptr = hwrpb;
#if TURBO
    /*
     * EMULATION HACK! Turbo MP uses real HWRPB Slots, not emulated ones
     */
    slot = (int)hwrpb_ptr + *hwrpb_ptr->SLOT_OFFSET + *hwrpb_ptr->SLOT_SIZE * (real_slot(id));
#else
    slot = (int)hwrpb_ptr + *hwrpb_ptr->SLOT_OFFSET + *hwrpb_ptr->SLOT_SIZE * id;
#endif
    if (haltcode >= 0)
	*slot->HALTCODE = haltcode;
    *(INT *)v = virt_to_phys( slot );
    write_ipr( APR$K_PCBB, v );
    write_ipr( APR$K_ASN, ref_quad( 0 ) );
    write_ipr( APR$K_MCES, ref_quad( 15 ) );
    write_ipr( APR$K_SISR, ref_quad( 0 ) );
    write_ps_high_ipl( slot );
    if( ( ( *(INT *)slot->PAL_REV >> 16 ) & 0xff ) == 2 )
	{
	write_ipr( APR$K_KSP, slot->HWPCB.OSF_HWPCB.KSP );
	write_ipr( APR$K_PTBR, slot->HWPCB.OSF_HWPCB.PTBR );
	write_ipr( APR$K_FEN, slot->HWPCB.OSF_HWPCB.FEN );
	}
    else
	{
	v[1] = 0;
	write_ipr( APR$K_KSP, slot->HWPCB.VMS_HWPCB.KSP );
	write_ipr( APR$K_PTBR, slot->HWPCB.VMS_HWPCB.PTBR );
	write_ipr( APR$K_FEN, slot->HWPCB.VMS_HWPCB.FEN );
	v[0] = ( slot->HWPCB.VMS_HWPCB.ASTEN_SR[0] >> 0 ) & 15;
	write_ipr( APR$K_ASTEN, v );
	v[0] = ( slot->HWPCB.VMS_HWPCB.ASTEN_SR[0] >> 4 ) & 15;
	write_ipr( APR$K_ASTSR, v );
	}
    write_ipr( APR$K_VPTB, hwrpb_ptr->VPTBR );
    write_pc( hwrpb_ptr->RESTART );
    write_gpr( 25, ref_quad( 0 ) );
    write_gpr( 26, ref_quad( 0 ) );
    write_gpr( 27, hwrpb_ptr->RESTART_VALUE );
    console_exit( );
    }

void secondary_start( )
    {
    int id;
    int v[2];
#if SABLE
    UINT two = 2;
    UINT three = 3;
    UINT csr[2];
    UINT temp;
#endif
    struct SLOT *slot;
    struct HWRPB *hwrpb_ptr;

    id = whoami( );
    hwrpb_ptr = hwrpb;

    /*
     *  If the HWRPB is valid, and context is valid, then start, otherwise
     *  stay in console mode.
     */

#if !RHMIN
    if( hwrpb_ptr && hwrpb_valid( hwrpb_ptr ) )
#else
    if (1)
#endif
	{
#if TURBO
        /*
         * EMULATION HACK! Turbo MP uses real HWRPB Slots, not emulated ones
         */
	slot = (int)hwrpb_ptr + *hwrpb_ptr->SLOT_OFFSET + *hwrpb_ptr->SLOT_SIZE * (real_slot(id));
#else
	slot = (int)hwrpb_ptr + *hwrpb_ptr->SLOT_OFFSET + *hwrpb_ptr->SLOT_SIZE * id;
#endif
	if( slot->STATE.SLOT$V_CV )
	    {
	    slot->STATE.SLOT$V_BIP = 1;
#if 0
	    if( krn$_timer_get_cycle_time( id )
		    != krn$_timer_get_cycle_time( primary_cpu ) )
		printf ("WARNING...\n  CPU speed different than primary\n");
#endif
#if SABLE
#if GAMMA
	    temp = 0x70000000;
	    temp <<= 32;
	    temp |= 0x70000000;
	    csr[0] = READ_CPU_CSR( primary_cpu, CREG_ADDR );
	    csr[1] = READ_CPU_CSR( id, CREG_ADDR );
#else
	    temp = 0xc0000000;
	    temp <<= 32;
	    temp |= 0xc0000000;
	    csr[0] = READ_CPU_CSR( primary_cpu, BCC_ADDR );
	    csr[1] = READ_CPU_CSR( id, BCC_ADDR );
#endif
	    if( ( csr[0] & temp ) ^ ( csr[1] & temp ) )
		printf ("WARNING...\n  CPU cache size different than primary\n");
#endif
	    if( ( ( *(INT *)slot->PAL_REV >> 16 ) & 0xff ) == 2 )
		{
		write_ipr( APR$K_KSP, slot->HWPCB.OSF_HWPCB.KSP );
		write_ipr( APR$K_PTBR, slot->HWPCB.OSF_HWPCB.PTBR );
		*(INT *)v = pal_virt_to_phys( id, PAL$OSFPAL_BASE );
		write_ipr( IBX$K_PAL_BASE, v );
		*(INT *)slot->HALT_PS = 0x7;
#if !TURBO
#if EV4
		write_ipr( APR$K_PS, slot->HALT_PS );
		read_ipr( ABX$K_ABOX_CTL, v );
		v[0] |= 0x20;
		write_ipr( ABX$K_ABOX_CTL, v );
		read_pt( PT$_ICCSR, v );
		v[1] |= 0x200;
		write_pt( PT$_ICCSR, v );
#if SABLE
		v[0] = 0x72727272;
		v[1] = 0x40424272;
		write_pt( PT$_INTMASK, v );
#endif
#if AVANTI || MIKASA || K2 || MTU || CORTEX || YUKONA
		v[0] = 0x00000000;
		v[1] = 0x00000000;
		write_pt( PT$_INTMASK, v );
#endif
#endif
#if EV5
		read_ipr( APR$K_ICSR, v );
		v[0] |= 0x20000000;
		write_ipr( APR$K_ICSR, v );
		read_ipr( APR$K_MCSR, v );
		v[0] |= 0x4;
		write_ipr( APR$K_MCSR, v );
		v[0] = 0x14020100;
		v[1] = 0x1f1e1615;
		write_pt( PT$_INTMASK, v );
		write_pt( 27, ref_quad( 7 ) );
#endif
#if EV6
		write_ipr( APR$K_PS, slot->HALT_PS );
		set_spe( 2 << ( page_table_levels == 4 ) );
#endif
#endif
#if TURBO || RAWHIDE
                /* 
                 * Helper routine to setup KSEG mapping, IPL/PS and intmask. 
                 * Takes care of real HW. 
                 */
                secondary_swappal();
#endif
		}
	    else
		{
		write_ipr( APR$K_KSP, slot->HWPCB.VMS_HWPCB.KSP );
		write_ipr( APR$K_PTBR, slot->HWPCB.VMS_HWPCB.PTBR );
		}
	    write_ipr( APR$K_VPTB, hwrpb_ptr->VPTBR );
	    write_pc( hwrpb_ptr->RESTART );
	    write_gpr( 25, ref_quad( 0 ) );
	    write_gpr( 26, ref_quad( 0 ) );
	    write_gpr( 27, hwrpb_ptr->RESTART_VALUE );
	    console_exit( );
	    }
	}
    }

void boot_system( char *boottype )
    {
#if !RHMIN
    int argc;
    char *argv[16];
    int id;
    struct PCB *pcb;
  
    id = whoami( );
    if( !( in_console & ( 1 << primary_cpu ) ) )
	{
	pprintf( "\nCPU %d requesting primary to boot\n\n", id );
	entry_boottype = boottype;
	node_halt_primary( );
	}
    else
	{
	pprintf( "\nCPU %d booting\n\n", id );
	pcb = getpcb( );
	controlc_register( pcb->pcb$l_pid );

#if MONET
	if ( read_boot_args( &argc, argv ) == msg_success) {
	    boot( argc, argv );
	    return;
	}
#endif

#if TURBO || RAWHIDE || MIKASA || BOOT_RESET
	if ( eeprom_read_boot_args( &argc, argv ) == msg_success)
	    {
	    boot( argc, argv );
	    return;
	    }
#endif

#if AVANTI
	/* Reset the VGA graphics controller interrupt register prior */
	/* to reboooting.					      */

	outportb(0,0xA1,0xFF);
	outportb(0,0x21,0xFB);
#endif

	/* Booting ...*/

	argv[0] = boottype;
	argv[1] = 0;
	boot( 1, argv );
	}
#else
	pprintf ("booting not supported in loadable console\n");
#endif
    }

void system_reset_or_error( protonoargs uint32 state_l, uint32 state_h, int is_primary )
    {
    int id;
    int bip;
    int rc;
    int cv;
    int hr;
    struct EVNODE *ev;
    struct HWRPB *hwrpb_ptr;
    struct SLOT *slot;
    struct EVNODE *evp_os_type, ev_os_type;
    struct EVNODE *evp_auto_action, ev_auto_action;
    char os_type = 0;
    char auto_action = 'H';
    int status = msg_success;
    struct SLOT_STATE state;
    union { struct { uint32 l; uint32 h; } q; struct SLOT_STATE s; } state_u;

    state_u.q.l = state_l;
    state_u.q.h = state_h;
    state = state_u.s;

    krn$_sleep( 10 );
    id = whoami( );

#if TURBO || ( MIKASA && !PRIMCOR ) || BOOT_RESET
    if ( nvr_init_expected() )
	return;
#endif

#if TURBO || RAWHIDE || MIKASA || BOOT_RESET
    if( nvr_boot_expected() ){
	boot_system( "c" );   
        return;
    }
#endif
#if MONET
    if( expected_entry ){
	expected_entry = 0;
	boot_system( "c" );   
        return;
    }
#endif
/* new algorithm */

    evp_os_type = &ev_os_type;
    evp_auto_action = &ev_auto_action;

    if (ev_read("os_type", &evp_os_type, EV$K_SYSTEM) == msg_success)
	os_type = *evp_os_type->value.string;

    if (ev_read("auto_action", &evp_auto_action, 0) == msg_success)
	auto_action = *evp_auto_action->value.string;

    hwrpb_ptr = hwrpb;

#if TURBO
    if (hwrpb_ptr == 0) {
	if (os_type == 'N') {
	    if (secure_switch())
		return;
	}
    }
#endif

#if GALAXY && !MODULAR
    if ((!galaxy_node_number) && (galaxy_partitions > 0) &&
	((auto_action == 'B') || (auto_action == 'R')) &&
	(all_cpu_mask == cpu_mask)) 
	{
	pprintf("\nAuto-Starting secondary partitions...\n");
	status = galaxy(0,0);
	if (status != msg_success)
	    return;
	}
#endif

    if (hwrpb_ptr == 0) {
       if( auto_action == 'R' )
	    {
		boot_system( "d" );
	    }
	else if( auto_action == 'B' )
	    {
		boot_system( "d" );
	    }
	return;
    } 

#if !RHMIN
    if( hwrpb_ptr && hwrpb_valid( hwrpb_ptr ) )
#else
    if (0)
#endif
	{
#if TURBO
        slot = (int)hwrpb_ptr + *hwrpb_ptr->SLOT_OFFSET + *hwrpb_ptr->SLOT_SIZE * (real_slot(id));
#else
        slot = (int)hwrpb_ptr + *hwrpb_ptr->SLOT_OFFSET + *hwrpb_ptr->SLOT_SIZE * id;
#endif
	bip = state.SLOT$V_BIP;
	rc = state.SLOT$V_RC;
	cv = state.SLOT$V_CV;
	hr = state.SLOT$V_HALT_REQUESTED;

	if( hr == HALT_REQUEST$K_POWER_OFF_SYSTEM )
	    {
	    /*
	     * Any platform which supports powering the system
	     * off should be added to the #if below.
	     */
#if MONET || K2 || TAKARA || EIGER || WILDFIRE
	    power_off_system( );
#endif
	    /*
	     * If we return, the system was not actually powered off, so
	     * just halt and remain halted.
	     */
	    hr = HALT_REQUEST$K_REMAIN_HALTED;
	    }

/*
 * On a shutdown halt request, the operating system may turn off DTR and RTS
 * when it shuts down it's serial driver (UNIX does this for instance). The
 * console therefore needs to reset these bits so a remote user can re-connect
 * via modem. Do so only for the shutdown case. Sable re-inits the serial ports
 * later in stop_hardware() so it is not done here..
 */
	if( hr == HALT_REQUEST$K_REMAIN_HALTED )
	    {
	    if( is_primary )
		{
#if GALAXY && RAWHIDE
		/* go back to interrupt mode to allow interrupt forwarding */
		if (galaxy_partitions > 1)
		    krn$_set_console_mode(INTERRUPT_MODE);
#endif
#if GALAXY && RAWHIDE
		if ((galaxy_partitions < 2) || (galaxy_node_number == 0))
		    init_serial_port( 0, COM1, com1_baud );
		if ((galaxy_partitions < 2) || (galaxy_node_number == 1))
		    init_serial_port( 0, COM2, com2_baud );
#else
#if MIKASA || RAWHIDE
		init_serial_port( 0, COM1, com1_baud );
		init_serial_port( 0, COM2, com2_baud );
#endif
#endif
		start_drivers( );
		}
	    }
	else if( hr == HALT_REQUEST$K_COLD_BOOT )
	    {
	    boot_system( "c" );
	    }
	else if( hr == HALT_REQUEST$K_WARM_BOOT )
	    {
	    boot_system( "w" );
	    }
#if GALAXY
	else if ( hr == HALT_REQUEST$K_MIGRATE)
	    {
		int destination = state.SLOT$L_RSVD_MBZ2 & 0xff;
#if MODULAR
		if (galaxy_shared_adr) 
		    {
		    migrate_helper(destination);
		    }
#else
		migrate_helper(destination);
#endif
	    }
#endif
	else if( auto_action == 'R' )
	    {
	    if( rc && cv )
		restart_cpu( -1 );
	    else if( !bip )
		boot_system( "d" );
	    else
		pprintf( "boot failure\n" );
	    }
	else if( auto_action == 'B' )
	    {
	    if( !bip )
		boot_system( "d" );
	    else
		pprintf( "boot failure\n" );
	    }
	}
    }	      

#if TURBO || RAWHIDE
/*
 * Helper routine to prepare the secondaries for swapping to OSF PAL. 
 */
void secondary_swappal (void)
{
    int v[2];
    struct SLOT *slot;
    struct HWRPB *hwrpb_ptr;
    int id;
    struct impure *impure_ptr;

    id = whoami();
    hwrpb_ptr = hwrpb;
    impure_ptr = PAL$IMPURE_BASE + PAL$IMPURE_COMMON_SIZE +
	         id * PAL$IMPURE_SPECIFIC_SIZE;

#if EV5
    /* Set ICSR:<SPE1> to enable I-Stream Kseg mapping */

    read_ipr( APR$K_ICSR, v );
    v[0] |= 0x20000000;
    write_ipr( APR$K_ICSR, v );
  
    /* Set MCSR:SP<1> to enable D-Stream Kseg mapping */		

    read_ipr( APR$K_MCSR, v );
    v[0] |= 0x4;
    write_ipr( APR$K_MCSR, v );

    /* 
     * Raise IPL. OSF has only 8 priority levels but EV5 has 32. Shadow
     * register 11 is load with a 7 (the highest OSF IPL) and the EV5's
     * IPL IPR is loaded with 1F.
     */
    v[0] = 0x1F00;
    v[1] = 0;
    write_ipr( APR$K_PS, v );

    impure_ptr->cns$shadow11[0] = 7;
    impure_ptr->cns$shadow11[1] = 0;

#if TURBO
    v[0] = 0x14020100;	/* IntMask... Need a symbol */
    v[1] = 0x1f1e1615;
#endif
#if RAWHIDE
    v[0] = 0x15020100;
    v[1] = 0x1f1e1715;
#endif
    write_pt( 8, v ); 
#endif

#if EV6
    set_spe( 2 << ( page_table_levels == 4 ) );

    /* 
     * Raise IPL. OSF has only 8 priority levels but EV6 has 32.  Shadow
     * register p_misc is loaded with a 7 (the highest OSF IPL) and the EV6's
     * IER IPR is loaded with 0x4000000000.
     */
    v[0] = 0x7;
    v[1] = 0;
    write_ipr( APR$K_PS, v );

    impure_ptr->cns$ier_cm[0] = 0;
    impure_ptr->cns$ier_cm[1] = 0x40;

#endif
}
#endif

void write_ps_high_ipl( struct SLOT *slot )
    {
    if( ( ( *(INT *)slot->PAL_REV >> 16 ) & 0xff ) == 2 )
	{
#if EV4 || EV6
	write_ipr( APR$K_PS, ref_quad( 0x7 ) );
#endif
#if EV5
	write_ipr( APR$K_PS, ref_quad( 0x1f00 ) );
	write_pt( 27, ref_quad( 7 ) );
#endif
	}
    else
	{
	write_ipr( APR$K_PS, ref_quad ( 0x1f00 ) );
	}
    }
