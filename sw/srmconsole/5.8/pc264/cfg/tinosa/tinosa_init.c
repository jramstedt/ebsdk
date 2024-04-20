/* file:	nautilus_init.c
 *                 
 * Copyright (C) 1996, 1997, 1998 by                           
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
 *      Alpha Firmware for PC264
 *                 
 *  MODULE DESCRIPTION:     
 *
 *	PC264 platform specific initialization routines.
 *
 *  AUTHORS:                                              
 *
 *	Eric Goehl
 *
 *  CREATION DATE:
 *  
 *      31-Jul-1996
 *
 *  MODIFICATION HISTORY:
 *
 *	er	03-Sep-1998	Clear the soft reset register in platform_init1().
 *	er	20-Jul-1998	Map I2C controller into low BIOS region of ROM space.
 *	er	15-Jul-1998	Enable fast discard and disable DCRTO reporting
 *				on Pchip 0.  This is a requirement for the
 *				external arbiter, used on Rev D. motherboards,
 *				to function properly with the Cypress ISA bridge.
 *	er	25-Jun-1998	Establish system event (680) handler.
 *				Changed Cypress external PCI arbiter protocol 
 *				control register for USB function to use the 
 *				SREQ/SGNT signal pair instead of PREQ/PGNT.
 *	er	26-Mar-1998	Added init_serial_port routine.
 *	er	09-Feb-1998	Added fix for IDE I/O read and DMA cycle
 *				deadlock condition.
 *	er	23-Oct-1997	Added more Cypress initialization to fix
 *				floppy, enable USB.
 *	er	25-Sep-1997	Added Cypress and Tsunami initialization.
 *	er	24-Aug-1997	Enabling writes to flash ROM.
 *     	egg	31-Jul-1996	Initial port from eb164_init.c
 *--
 */                                                

#include 	"cp$src:platform.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:pb_def.h"
#include	"cp$src:common.h"
#include	"cp$inc:prototypes.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:hwrpb_def.h"
#include 	"cp$src:ev_def.h"
#include 	"cp$src:alpha_def.h"
#include 	"cp$src:aprdef.h"
#include	"cp$src:alphascb_def.h"
#include	"cp$src:parse_def.h"
#include	"cp$src:pal_def.h"
#include	"cp$src:tt_def.h"
#include	"cp$src:probe_io_def.h"
#include	"cp$src:combo_def.h"
              
/* Pc264 specific definition files */
#include	"cp$src:platform_cpu.h"
#include	"cp$inc:platform_io.h"
#include 	"cp$src:apc.h"
#include 	"cp$src:tsunami.h"	
#include	"cp$src:m1543c_def.h"
#include	"cp$src:impure_def.h"	/* struct impure */

#include	"cp$src:nt_types.h"

int io_bus_reset;
/* external data needed */
extern int do_bpt();

extern int primary_cpu;
extern int keybd_present;
extern int graphics_console;
extern int CTB$MAX_CTBS;
extern int CTB$MAX_SIZE;
extern struct TTPB serial_ttpb;
extern struct TTPB null_ttpb;
extern struct TTPB *console_ttpb;	/* pointer to primary console port */
extern struct HQE mchq_660;			/*MCHECK handler queue*/
extern int HWRPB;
extern struct LOCK spl_kernel;
extern struct SEMAPHORE pbs_lock;
extern unsigned int num_pbs;
extern struct pb **pbs;
extern int poison_hwrpb;

/* external function prototypes */
extern void mcheck_handler(int vector);
extern void mcheck_handler_620(int vector);
extern void mcheck_handler_630(int vector);
extern void mcheck_handler_680(int vector);
extern void pc264_unexpected_int();
extern void pc264_ipir(void);
extern void pc264_perfmon_isr(void);
extern void node_halt_secondaries();
extern void reset_cpu(int id);
extern void clear_cpu_errors(int id);
extern int whoami(void);
extern int spinlock(struct LOCK *spl_kernel);
extern int spinunlock(struct LOCK *spl_kernel);
extern void setup_io(void);
extern void krn$_reset_toy(void);
extern void io_enable_interrupts(struct pb *pb, unsigned int vector);
extern int read_pci_id(int hose, int bus, int slot, int function);
extern void get_pci_csr_vector(struct pb *pb);
extern void sys_irq_init(void);
extern void iic_reinit(void);

struct set_param_table term_name[] = {
	{"com1",	0},
	{"com2",	1},
	{0}
    };                                             

int CTB_units[] = {
	CTB$K_DZ_WS
	};

struct CTB_VT_table CTB_VT_tab[] = {
	{0, 0, 0, 0, 0}
	};

struct CTB_WS_table CTB_WS_tab[] = {                  
	{0, 0, 21, 0, 0, CTB$K_NONE}
	};

      

void pb_reinit(struct pb *pb)
{
    qprintf("pb_reinit\n");

	switch (pb->type)
	{
    	case TYPE_PCI:
		    read_pci_id(pb->hose, pb->bus, pb->slot, pb->function);
		    get_pci_csr_vector(pb);
			break;
    }
}


/*+
 * ============================================================================
 * = platform_init1 - First platform specific initialization                  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Perform console initialization that is required by the platform.
 *	This routine should do any specific initialization for setting
 *	up the console data structures.
 *
 *	This routine is called as the first thing in KRN$_IDLE.
 *
 * FORM OF CALL:
 *
 *	platform_init1 (); 
 *
 * RETURNS:
 *
 *      Nothing                                                                      
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
platform_init1 (void)
{                                          
    if (primary())
	{
		setup_io();
    } 

    clear_cpu_errors(whoami());
}  
   
  
/*+
 * ============================================================================
 * = platform_init2 - Second platform specific initialization                  =
 * ============================================================================
 *            
 * OVERVIEW:  
 *            
 * 	Perform console initialization that is required by the platform.
 * 	This routine should do any specific initialization for setting
 * 	up the console data structures.
 *            
 * 	This routine is called in KRN$_IDLE after some memory/variable 
 *      initialization has occured. 
 *            
 * NOTE: I am not too sure why this code needs to be placed here rather than 
 *       the routine platform_init1();
 *            
 * FORM OF CALL:
 * 
 *	platform_init2 (); 
 *
 * RETURNS:
 *
 *      Nothing
 *                          
 * ARGUMENTS:
 *
 *      None
-*/
platform_init2(void)
{
	struct pb pb;
	struct impure *impure_ptr;
	unsigned int signature;

	if (primary())
	{
		/*
		** Initialize the serial port(s) ...
		*/
		pb.hose = 0;
		pb.bus = 0;
		pb.slot = 5;
		pb.function = 0;

		init_serial_port(&pb, COM1, 0);
		init_serial_port(&pb, COM2, 0);

		/* 
		** Set graphics_console global variable according to TOY nvram value
		** to reflect graphics or serial console mode. 
		*/
		ev_console_init(0, 0);	

		/*
		** Probe the keyboard controller and set keybd_present global
		** variable according to whether or not an attatched keyboard
		** was detected.
		*/
		keybd_present = kbd_attatched();

		/*
		** If we know we'll be using the serial port as primary console then
		** setup for using those I/O routines now so we can display ASAP.
		** Otherwise use null I/O routines until a graphics driver instantiates
		** itself as the primary console and initializes its hardware.
		*/
		CTB$MAX_CTBS = CTB$MAX_UNITS;
		CTB$MAX_SIZE = sizeof(struct CTB_WS);
		if (graphics_console && keybd_present)
		{
			console_ttpb = &null_ttpb;
			CTB_WS_tab[0].TYPE = CTB$K_DZ_WS;
		}
		else
		{
			console_ttpb = &serial_ttpb;
			CTB_WS_tab[0].TYPE = CTB$K_DZ_VT;
		}

		if (platform() == ISP_MODEL)
		    qprintf("Running on the ISP model.\n");

		impure_ptr = (struct impure *)(PAL$IMPURE_BASE + PAL$IMPURE_COMMON_SIZE + whoami() * PAL$IMPURE_SPECIFIC_SIZE);
		signature = *(unsigned __int64 *)(impure_ptr->cns$srom_signature);

#if NT_ONLY_SUPPORT
		if ((signature >> 15) & 0x01)
		{
			poison_hwrpb = 1;
		}
		else
		{
			poison_hwrpb = 0;
		}
#endif
	}
}    
    
/*+  
 * ============================================================================
 * = initialize_hardware - initialize hardware                                =
 * ============================================================================
 *
 * OVERVIEW:
 *              
 *	Perform hardware initialization on the PC264. This routine is called
 * 	from krn$_idle only for the primary CPU. 
 *              
 *	This routine is called in KRN$_IDLE after the dynamic memory data  
 *      structure initialization has occured. The routine pci_size_config.c
 *      uses malloc and free so this initialization needs to occur 
 *              
 * FORM OF CALL:
 *
 *	initialize_hardware (); 
 *
 * RETURNS:
 *
 *      Nothing
 *                          
 * ARGUMENTS:
 *
 *      None                 
 *
-*/
initialize_hardware()
{
/*
**  Setup interrupt handlers for the system ....
*/
    int_vector_set(SCB$Q_SLE, (int)do_bpt, 0, 0);             
    int_vector_set(SCB$Q_SYS_CORR_ERR, (int)mcheck_handler_620, SCB$Q_SYS_CORR_ERR, 0);
    int_vector_set(SCB$Q_PR_CORR_ERR, (int)mcheck_handler_630, SCB$Q_PR_CORR_ERR, 0);  
    int_vector_set(SCB$Q_SYS_MCHK, (int)mcheck_handler, SCB$Q_SYS_MCHK, 0);          
    int_vector_set(SCB$Q_PR_MCHK, (int)mcheck_handler, SCB$Q_PR_MCHK, 0);  
    int_vector_set(SCB$Q_6F0, (int)pc264_unexpected_int, 0, 0);          
    int_vector_set(SCB$Q_INTERPROCESSOR, (int)pc264_ipir, 0, 0);          
    int_vector_set(SCB$Q_650, (int)pc264_perfmon_isr, 0, 0);          
    int_vector_set(SCB$Q_470, (int)pc264_perfmon_isr, 0, 0);          
    int_vector_set(SCB$Q_680, (int)mcheck_handler_680, 0, 0);
    mchq_660.flink = &mchq_660;                   
    mchq_660.blink = &mchq_660;               
}                                             
                           

/*+
 * ============================================================================
 * = reset_hardware - reset hardware					      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Perform hardware reset on the Nautilus
 *
 * FORM OF CALL:
 *
 *	reset_hardware (log); 
 *
 * RETURNS:
 *
 *      Nothing
 *                          
 * ARGUMENTS:
 *
 *      log - ???
-*/
reset_hardware(int log) 
{
    struct SLOT *slot;
    struct HWRPB *hwrpb;
    struct pb pb;
    int i;

    node_halt_secondaries();
    reset_cpu(whoami());

#ifdef FIX_LATER
    hwrpb = HWRPB;
    slot = (int)hwrpb + *hwrpb->SLOT_OFFSET + *hwrpb->SLOT_SIZE * whoami();
    if (!slot->STATE.SLOT$V_BIP 
	    || (ReadTsunamiCSR(PCHIP0_WSBA0) & 1) 
	    || (ReadTsunamiCSR(PCHIP1_WSBA0) & 1)
	    || ReadTsunamiCSR(PCHIP0_WSBA2)
	    || ReadTsunamiCSR(PCHIP1_WSBA2))
    {
		spinlock(&spl_kernel);

		/*
		** Reset the I/O buses...
		*/
		io_bus_reset = 1;
		if (log)
			pprintf("Reseting I/O buses...\n");

		krn$_micro_delay(1000000);

		setup_io();

		pb.hose = 0;
		pb.bus = 0;
		pb.slot = 5;
		pb.function = 0;

		init_serial_port(&pb, COM1, 0);
		init_serial_port(&pb, COM2, 0);

		sys_irq_init();

		iic_reinit();

		spinunlock(&spl_kernel);

#if ((IPL_RUN) && (IPL_RUN > 19))
		io_enable_interrupts(&pb, FLOPPY_VECTOR);
		io_enable_interrupts(&pb, COM2_VECTOR);
		io_enable_interrupts(&pb, COM1_VECTOR);
#endif

		krn$_reset_toy();

		krn$_wait(&pbs_lock);
		for (i = 0; i < num_pbs; i++)
			pb_reinit(pbs[i]);
		krn$_post(&pbs_lock);

#if TGA
		tga_reinit(0);    /* Reinit the TGA but do not clear the screen */
#endif
		vga_reinit();	    /* Reinit VGA */

		kbd_reinit();	    /* Reinit keyboard */

		mc_reinit();	    /* Reinit memory channel adapters */
    }
#endif
}                                     

