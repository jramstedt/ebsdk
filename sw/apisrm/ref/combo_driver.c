/* file:	combo_driver.c
 *
 * Copyright (C) 1992 by
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
 *      Jensen Exerciser
 *
 *  MODULE DESCRIPTION:
 *
 *	A collection of drivers that supports the VLSI Technologies 82C106
 *	PC/AT integerated combination I/O chip.  Specifically, inodes are
 *	exported for the following devices:
 *
 *		COM1
 *		COM2
 *		PRN
 *		MOUSE
 *		KBD
 *
 *  AUTHORS:
 *
 *	AJ Beaverson, Judy Gold
 *
 *  CREATION DATE:
 *  
 *	16-Jul-1992	Adapted from ssctt_driver
 *
 *
 *  MODIFICATION HISTORY:
 *
 *      mab     26-Jun-1997     Com error message for systems with OCP
 *	
 *      mab     26-Jun-1997     Com error message for systems with OCP
 *
 *	jje	09-Aug-1996	Don't init serial ports for Cortex.
 *
 *	er	05-Jul-1996	Removing EBxxx conditionals for polled mode.
 *
 *	er	02-Apr-1996	Added conditional for EBxx platforms to initialize
 *				serial ports elsewhere.
 *
 *	pmd 	19-Jan-1995	Conditionalize for MTU.
 *
 *	er	21-Aug-1995	Changed EB platforms to run in polled mode.
 *
 *	dtr	8-aug-1995	backed out previous changes until I understand
 *				why it doesn't work.
 *
 *	dtr	7-Aug-1995	changed the routines to accept port block as the
 *				parameter and not line number.  The calls to the
 *				console routines are now consistant in that they
 *				now all get the port block.
 *				
 *	swt	28-Jun-1995	add Noritake platform.
 *	
 *	dtr	24-may-1995	ipl_run added as aconditional for polled.
 *	
 *	adm	28-Mar-1995	add function "combott_lineset()".
 *				to support call_back 5 (SET_TERM_CTL)
 *
 *	rbb	13-Mar-1995	Conditionalize for EB164
 *
 *      dpm	08-Feb-1995	Set the serial port interrupt enable regsiter
 *				to 0 before writing it to enable recieve
 *				interrupts in combott_rxon
 *
 *	er	27-Dec-1994	Conditionalize for EB66
 *
 *	dwb	04-Nov-1994	Add Alcor and Avanti to the Sync fix below
 *
 *	dwb	22-Oct-1994	Force the Combo Driver to be in Sync with the 
 *				OS driver on Mikasa. This fixes a problem when
 *				the OS Crashes and we Lose the Serial Line 
 *				Input.
 *
 *	er	21-Oct-1994	Force SD164 to run interrupt driven
 *
 *	dwb	22-Sep-1994	Force Mikasa to run interrupt driven
 *
 *	er	19-Sep-1994	Issue EOI for EB64+
 *
 *	er	15-Aug-1994	Conditionalize for SD164
 *
 *	er	19-Jul-1994	Conditionalize for EB64+
 *	
 *	jeg	11-Apr-1994	put support for Jensen back into this driver
 *				via xbus routines.
 *
 *	dtr	25-Feb-1994	Mustang now does eoi.
 *
 *	dtr	27-Jan-1994	inport/outport changed to eisa_x
 *
 *	dtr	16-Dec-1993	Proper lean support 
 *
 *	jmp	29-Nov-1993	Add avanti support
 *
 *	pel	31-Aug-1993	Morgan, run COM1 and COM2 interrupt mode.
 *
 *	pel	11-Mar-1993	Morgan, don't init com1, com2; done in PAL
 *
 *	pel	25-Feb-1993	replace xbus_in/outport w/ in/outport so
 * 				they'll work w/ either xbus or eisa.
 *
 *	pel	08-Feb-1993	use DRIVER_MODE instead of hard coding
 *				DDB$K_POLLED/INTERRUPT. For morgan.
 *
 *	jeg	25-Jan-1993	replace read_ and write_ combo with xbus driver
 *				calls.
 *
 *	jeg	15-Oct-1992	add second serial port, put in modem init.
 *
 *	jeg	16-Sep-1992	put them back.
 * 
 *	jeg	03-Aug-1992	eliminate ldq/stq_piocsr calls.	
 *
 *      jeg	31-Jul-1992	Initial version.
 *--
 */

#include	"cp$src:platform.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:stddef.h"
#include	"cp$src:common.h"
#include "cp$inc:prototypes.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:tt_def.h"
#include	"cp$src:combo_def.h"
#include	"cp$src:hwrpb_def.h"
#include	"cp$src:isacfg.h"
#include	"cp$src:pb_def.h"
#include	"cp$src:platform_cpu.h"
#include	"cp$src:ansi_def.h"
#include	"cp$inc:kernel_entry.h"
#if TURBO
#include	"cp$src:mem_def.h"
#endif
#if GALAXY
#include	"cp$src:get_console_base.h"
#include	"cp$src:pal_def.h"
#endif

#if WILDFIRE
#define COM1_VECTOR 0
#define COM2_VECTOR 0
#endif

#if GALAXY
extern int console_mode[];
extern int cpu_mask;
extern int all_cpu_mask;
#endif

#if !MODULAR
extern unsigned char inportb (struct pb *pb,unsigned __int64 addr);
extern void outportb (struct pb *pb,unsigned __int64 addr,unsigned char data);
#endif

#if SABLE || RAWHIDE || MIKASA
extern int dsr_check[3];
#endif
extern int tt_inited;
extern int cbip;
extern struct LOCK spl_kernel;
extern void tt_rx_interrupt();		/* Recieve ISR		*/	
extern void tt_tx_interrupt();		/* Transmit ISR		*/
extern struct DDB *tt_ddb_ptr;
#if TURBO
extern struct pb eisa_pb;
extern struct window_head config;
#endif

/*
 * Port routines for the primary console
 */
int combott_rxoff (int port);
int combott_rxon (int port);
int combott_rxready (int port);
int combott_rxread (int port);
int combott_txoff (int port);
int combott_txon (int port);
int combott_txready (int port);
int combott_txsend (int port, char c);
int combott_lineset (int port, int line_param);   /*adm*/
int comx_int (struct TTPB *ttpb);
#if !RAWHIDE
void init_serial_ports ( );
void init_one_serial_port ( int cpp);
#endif

#if RAWHIDE || (IPL_RUN > 19) || TURBO || WFSTDIO
#define DRIVER_MODE DDB$K_POLLED
#else
#define DRIVER_MODE DDB$K_INTERRUPT
#endif

#define combo_outportb(c,v) outportb(cp->pb,c,v)
#define combo_inportb(c)    inportb(cp->pb,c)

#if !TURBO
#define combo_serial_ttpb serial_ttpb
#define combo_serial_reinit serial_reinit
#endif

/*
 * COM1 serves as the primary console for the firmware. (adm)
 */
struct TTPB combo_serial_ttpb = {
	0, 0,			/* flink/blink */
	combott_rxoff, combott_rxon, combott_rxready, combott_rxread,
	combott_txoff, combott_txon, combott_txready, combott_txsend, combott_lineset,
	0,			/* port identifier */
	1,			/* translate newlines */
	-1,			/* operating mode is interrupt or polled */
	1			/* poll receives when stopped */
};

extern int graphics_console;
extern struct TTPB null_ttpb;
extern struct TTPB *console_ttpb;	/* pointer to primary console port */

#if RAWHIDE
extern struct TTPB *rawhide_combo_ttpb;
#endif

/*
 * This table enumerates the various standard com devices support by the combo
 * chip.
 */
static struct COM_DEVTAB {
	char name [8];
	unsigned int	port;
	unsigned int	rx_vector;
	unsigned int	tx_vector;
	struct TTPB	*ttpb;
	struct pb	*pb;
	unsigned int	ier;
	unsigned int	baud;
	unsigned int	flow;
	unsigned int	modem;
} 
com_devtab [] = {
	{"tta0", COM1,	COM1_VECTOR},
	{"tta1", COM2,	COM2_VECTOR},
#if WFSTDIO
	{"ttb0", COM1,	COM1_VECTOR},
	{"ttb1", COM2,	COM2_VECTOR},
#endif
};

#if !TURBO
int com_devtab_ptr = &com_devtab[0];
#endif

 /* adm
  * When VMS makes a call to change line parameters it passes baud rate parameter in 
  * bits [0:7] of argument:
  * bits [0-7] - baud rate, currently VMS supports following rates:
  *		6 - 300, 8 - 1200, 11 - 2400, 13 - 4800, 15 -9600, 16 - 19200
  *             0xff means 'use current baud rate'   
  */

	/* indecies in vms_baud are used to translate the VMS baud values to combo chip values */
int vms_baud_table [19] =   {
			0,
			BAUD_50,		/* index1, ( 2304) */
			BAUD_75 ,		/* index2 , (  1536)*/
			BAUD_110 ,		/* index3 , (  1047)*/
			BAUD_1345 ,		/* index4 , (  857) */
			BAUD_150 ,		/* index5 , (  768) */
			BAUD_300 ,		/* index6 , (  384) */
			BAUD_600 ,		/* index7 , (  192) */
			BAUD_1200 ,		/* index8 , (  96 ) */
			BAUD_1800 ,		/* index9 , (  64 ) */
			BAUD_2000 ,		/* index10 , (  58 ) */
			BAUD_2400 ,		/* index11 , (  48 ) */
			BAUD_3600 ,		/* index12 , (  32 ) */
			BAUD_4800 ,		/* index13 , (  24 ) */
			BAUD_7200 ,		/* index14 , (  16 ) */
			BAUD_9600 ,		/* index15 , (  12 ) */
			BAUD_19200 ,		/* index16 , (  6  ) */
			BAUD_38400 ,		/* index17 , (  3  ) */
			BAUD_56000 		/* index18 , (  2  ) */
                      };


/*
 * Turn off the transmitter
 */	
void combott_txoff (int port) {
	struct COM_DEVTAB *cp;

#if GALAXY && CLIPPER
	if ((get_console_base_pa() == 0) &&
	    (port == 1) &&
	    (cpu_mask != all_cpu_mask)) {
	    return;
	}
#endif
	cp = com_devtab + port;
	cp->ier &= ~(IER$M_THREIE | IER$M_MSIE);
	combo_outportb (cp->port + IER, cp->ier);
}

/*
 * Turn on the transmitter interrupt
 */
void combott_txon (int port) {
	struct COM_DEVTAB *cp;

#if GALAXY && CLIPPER
	if ((get_console_base_pa() == 0) &&
	    (port == 1) &&
	    (cpu_mask != all_cpu_mask)) {
	    return;
	}
#endif
	cp = com_devtab + port;
	cp->ier |= IER$M_THREIE | IER$M_MSIE;
	io_enable_interrupts (cp->pb, cp->rx_vector);
	combo_outportb (cp->port + IER, 0);
	combo_outportb (cp->port + IER, cp->ier);
}

/*
 * See if any transmit characters are pending
 */
int combott_txready (int port) {
	struct COM_DEVTAB *cp;
	char c;

	cp = com_devtab + port;
	c = combo_inportb (cp->port + MSR);

/* Added for OCP COM error display. Set global variable if DSR is not asserted and COMx_FLOW is set to hardware or both */

#if SABLE || RAWHIDE || MIKASA
	if( !(c & MSR$M_DSR) && ((cp->flow == 2) || (cp->flow == 3))){ 
	  dsr_check[port] = 1;
	}
#endif

#if GALAXY && CLIPPER

        /* if this is instance 0 and this is com2 and galaxy is started */
        /* then handle specially */
        if ((get_console_base_pa() == 0) &&
            (port == 1) &&
            (cpu_mask != all_cpu_mask)) {

            /* if we're at high ipl & forwarding is enabled then */
            /* perform the forwarding here */
            if ((console_mode[whoami()] != INTERRUPT_MODE) &&
                (*(UINT *) (PAL$IRQ_FWD_FLAG) == 1)) {
                if (combo_inportb (cp->port + IER) &&
                    (combo_inportb (cp->port + LSR) & LSR$M_THRE)) {
                    *(UINT *) (PAL$IRQ_VECT) = cp->rx_vector << 4;
                }
            }

            return(0);  /* don't service in this instance */
        }

        if ((cp->ier) && (cp->ttpb->mode == DDB$K_POLLED)) {
            io_issue_eoi (0, cp->rx_vector);    /* clear out any ints */
        }
#endif

	if (!cbip && cp->flow & 2) {
	    if ((c & MSR$M_DSR) && !(c & MSR$M_CTS)) {
		return (0);
	    }
	}
	if (combo_inportb (cp->port + LSR) & LSR$M_THRE) {
	    return (1);
	} else {
	    if (cp->ier) {
		combo_outportb (cp->port + IER, 0);
		combo_outportb (cp->port + IER, cp->ier);
	    }
	    return (0);
	}
}

/*
 * Send a character
 */
void combott_txsend (int port, char c) {
	struct COM_DEVTAB *cp;

#if GALAXY && CLIPPER
	if ((get_console_base_pa() == 0) &&
	    (port == 1) &&
	    (cpu_mask != all_cpu_mask)) {
	    return;
	}
#endif
	cp = com_devtab + port;
	combo_outportb (cp->port + THR, c);
}

/*
 * Turn off receiver interrupts
 */
void combott_rxoff (int port) {
	struct COM_DEVTAB *cp;

#if GALAXY && CLIPPER
	if ((get_console_base_pa() == 0) &&
	    (port == 1) &&
	    (cpu_mask != all_cpu_mask)) {
	    return;
	}
#endif
	cp = com_devtab + port;
	cp->ier &= ~IER$M_RDAIE;
#if SABLE
	if (cp->ier == 0)
	    io_disable_interrupts (0, cp->rx_vector);
#else
        io_disable_interrupts (cp->pb, cp->rx_vector);
#endif
	combo_outportb (cp->port + IER, cp->ier);
}

/*
 * Turn on receiver interrupts
 */
void combott_rxon (int port) {
	struct COM_DEVTAB *cp;

#if GALAXY && CLIPPER
	if ((get_console_base_pa() == 0) &&
	    (port == 1) &&
	    (cpu_mask != all_cpu_mask)) {
	    return;
	}
#endif
	cp = com_devtab + port;
	cp->ier |= IER$M_RDAIE;
	io_enable_interrupts (cp->pb, cp->rx_vector);
	combo_outportb (cp->port + IER, 0);
	combo_outportb (cp->port + IER, cp->ier);
}

/*
 * See if any receive characters are pending
 */
int combott_rxready (int port) {
	struct COM_DEVTAB *cp;
	char c;

	cp = com_devtab + port;
	c = combo_inportb (cp->port + MSR);

#if GALAXY && CLIPPER
        /* if this is instance 0 and this is com2 and galaxy is started */
        /* then handle specially */
        if ((get_console_base_pa() == 0) &&
            (port == 1) &&
            (cpu_mask != all_cpu_mask)) {

            /* if we're at high ipl & forwarding is enabled then */
            /* perform the forwarding here */
            if ((console_mode[whoami()] != INTERRUPT_MODE) &&
                (*(UINT *) (PAL$IRQ_FWD_FLAG) == 1)) {
                if (combo_inportb (cp->port + IER) &&
                    (combo_inportb (cp->port + LSR) & LSR$M_DR)) {
                    *(UINT *) (PAL$IRQ_VECT) = cp->rx_vector << 4;
                }
            }

            return(0);  /* don't service in this instace */
        }

        if ((cp->ier) && (cp->ttpb->mode == DDB$K_POLLED)) {
            io_issue_eoi (0, cp->rx_vector);    /* clear out any ints */
        }
#endif

	if (combo_inportb (cp->port + LSR) & LSR$M_DR) {
	    return (1);
	} else {
	    if (cp->ier) {
		combo_outportb (cp->port + IER, 0);
		combo_outportb (cp->port + IER, cp->ier);
	    }
	    return (0);
	}
}

/*
 * Read in the next receive character
 */
int combott_rxread (int port) {
	struct COM_DEVTAB *cp;

#if GALAXY && CLIPPER
	if ((get_console_base_pa() == 0) &&
	    (port == 1) &&
	    (cpu_mask != all_cpu_mask)) {
	    return(0);
	}
#endif
	cp = com_devtab + port;
	return (combo_inportb (cp->port + RBR));
}

/*
 * set up the hardware for COM1 and COM2
 */
int combott_init () {
	struct COM_DEVTAB *cp;
	struct TTPB *ttpb;
	struct INODE *ip;
	int i;
	int size_com_devtab;
	int found;
#if TURBO
	struct device *dev;
#endif

	found = config_get_device("COM2");
	if (!found)
	   size_com_devtab = 1;
	else
	   size_com_devtab = sizeof (com_devtab) / sizeof (com_devtab [0]);

	/*
	 * Initialize all the com devices that this chip supports
	 */
	for (i=0; i<size_com_devtab; i++) {

#if GALAXY && (SABLE || RAWHIDE)
	    if (i != whoami ()) continue;
#endif
	    cp = &com_devtab [i];

#if GALAXY && CLIPPER
            if (get_console_base_pa() && (i != 1))
                continue;   /* instance 1 only gets com2 */
#endif

#if TURBO
	    if (i == 0)
		dev = find_dev("com1", &config);
	    if (i == 1)
		dev = find_dev("com2", &config);
	    if (dev == 0)
		continue;		/* no comm ports */

	    /* Load the base address from the device to pb */

	    cp->pb = malloc_noown(sizeof(struct pb));
	    cp->pb->hose = dev->hose;
	    cp->pb->slot = dev->slot;
	    cp->pb->bus = dev->bus;
	    cp->pb->function = dev->function;
	    cp->pb->channel = dev->channel;
	    cp->pb->controller = dev->devdep.io_device.controller;
	    cp->pb->config_device = dev;

#if GALAXY
	    if (get_console_base_pa() == 0) 
#endif
		cp->name[2] = 'b';
#endif

#if ALCOR || MEDULLA || TURBO
	    init_one_serial_port( cp);
#endif
		    /* allocate a port block for the new device */
	    cp->ttpb = ttpb = tt_init_port (cp->name); /* returns with spinlock taken */

		    /* connect the read/write routines to the port block */
	    ttpb->rxoff = combott_rxoff;
	    ttpb->rxon = combott_rxon;
	    ttpb->rxready = combott_rxready;
	    ttpb->rxread = combott_rxread;
	    ttpb->txoff = combott_txoff;
	    ttpb->txon = combott_txon;
	    ttpb->txready = combott_txready;
	    ttpb->txsend = combott_txsend;
	    ttpb->port = i;
	    ttpb->perm_mode = DRIVER_MODE;
#if GALAXY && CLIPPER
	    ttpb->perm_poll = 1;
#else
	    ttpb->perm_poll = 0;
#endif
	    spinunlock(&spl_kernel);

		    /* connect to the interrupts */
	    int_vector_set (cp->rx_vector, comx_int, ttpb);

	    if (DRIVER_MODE == DDB$K_INTERRUPT) {
		io_enable_interrupts (cp->pb, cp->rx_vector);
#if MTU
		/* Set IRQ level sensitive */
		set_level_sensitive(cp->rx_vector & 0x0f); 
#endif		
	    }

	    /* set the polling/interrupt mode */
	    tt_setmode_pb (ttpb, DRIVER_MODE);
	}


	/*
	 * Create a generic inode for the device TT and wire it to COM1.
	 */
	if ((console_ttpb == &null_ttpb) || (console_ttpb == &combo_serial_ttpb)) {
#if RAWHIDE
#if GALAXY
	    if (whoami () == 0)
		rawhide_combo_ttpb = com_devtab [0].ttpb;
	    if (whoami () == 1)
		rawhide_combo_ttpb = com_devtab [1].ttpb;
#else
	    rawhide_combo_ttpb = com_devtab [0].ttpb;
#endif
#else
#if GALAXY && (CLIPPER || SABLE)
	    if (get_console_base_pa())
		console_ttpb = com_devtab [1].ttpb;
	    else
		console_ttpb = com_devtab [0].ttpb;
#else
#if GALAXY && TURBO
	    if (get_console_base_pa())
#endif
	    console_ttpb = com_devtab [0].ttpb;
#endif
	    graphics_console = 0;
#endif
	}

#if TURBO
#if GALAXY
	if (get_console_base_pa() == 0)
#endif
	    return msg_success;
#endif

	allocinode ("tt", 1, &ip);
	ip->dva = tt_ddb_ptr;
	ip->attr = ATTR$M_READ | ATTR$M_WRITE;
#if RAWHIDE && GALAXY
	ip->misc = rawhide_combo_ttpb;
#else
	ip->misc = console_ttpb;
#endif
	INODE_UNLOCK (ip);

	/* primary console is now on line */
	tt_inited = 1;

	return msg_success;
}  


/*
 * This initialization procedure creates inodes for several of the devices in
 * the 82c106 chip
 */
#if WILDFIRE || WFSTDIO
void combo_init_pb (struct pb *pb) {
	com_devtab [pb->channel].pb = pb;
	com_devtab [pb->channel].rx_vector = pb->vector;
}
#endif

int combo_init (int phase) {
#if WILDFIRE || WFSTDIO
	find_pb ("16450", 0, combo_init_pb);
#endif
#if TURBO
	if (eisa_shared_adr)
#endif
	combott_init ();

	return msg_success;
}

/*+
 * ============================================================================
 * = comx_int - ISR for COMx receive or transmit interrupt                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Determine if it's a receive or transmit interrupt and call the
 *	appropriate routine(s).  Keep reading the IIR register until all
 *	receive and transmit interrupts have been processed.
 *  
 * FORM OF CALL:
 *  
 *	comx_int (ttpb)
 *  
 * RETURNS:
 *
 *	0 - nothing
 *       
 * ARGUMENTS:
 *
 *	struct TTPB *ttpb	- port block
-*/
void comx_int (struct TTPB *ttpb) {
    char	c;
    int		i;
    int		port;
    struct COM_DEVTAB *cp;

    port = ttpb->port;

    cp = com_devtab + port;

    for (i = 0; i < 4; i++) {
#if WILDFIRE /* WildBRAT 1731 */
	if ((combo_inportb (cp->port + LSR) & LSR$M_DR) && (pca_rev_pb (cp->pb) == 1))
	    c = 4;
	else
#endif
	c = combo_inportb (cp->port + IIR);
	if (c & 1) {
	    break;
	}
	if (c & 4) {
	    tt_rx_interrupt (ttpb);
	} else {
	    tt_tx_interrupt (ttpb);
	}
    }
    io_issue_eoi (cp->pb, cp->rx_vector);

}

#if !RAWHIDE
void init_serial_ports ( ) {
    struct COM_DEVTAB *cp;
    int i;
    int size_com_devtab;
    int found;

    found = config_get_device("COM2");
    if (!found) 
       size_com_devtab = 1;
    else
       size_com_devtab = sizeof (com_devtab) / sizeof (com_devtab [0]);

	/*
	 * Initialize all the com devices that this chip supports
	 */
    for (i=0; i<size_com_devtab; i++) {
	cp = &com_devtab [i];
	init_one_serial_port( cp);
    }

    return;
}
#endif

void init_one_serial_port ( int cpp) {

    struct COM_DEVTAB *cp = cpp;
 
    /* turn off interrupts */
    combo_outportb (cp->port + IER, 0);

    combo_outportb (cp->port + LCR, LCR$M_DLA); 	/* access divisor latches */

	/* line speed = 9600 baud */
    combo_outportb (cp->port + DLL, BAUD_9600>>0);      
    combo_outportb (cp->port + DLM, BAUD_9600>>8);      

	/* 8 bit char, 1 stop bit, no parity */
    combo_outportb (cp->port + LCR, CHAR_8BIT | LCR$M_SBS);

	/* modem init - DTR, RTS active, OUT1 low, OUT2 enabled */
    combo_outportb (cp->port + MCR, MCR$M_DTR | MCR$M_RTS | MCR$M_OUT1 | MCR$M_OUT2); 

	/* turn FIFOs on */
    combo_outportb (cp->port + FCR, FCR$M_FE | FCR$M_RFR | FCR$M_TFR);

	/* flush receive buffer */
    while (combo_inportb (cp->port + LSR) & LSR$M_DR) 
	combo_inportb (cp->port + RBR);
    return;
}



/*+
 * ============================================================================
 * = combott_lineset - set line parameters (baud, char_size, stop, parity)    =
 * ============================================================================
 * adm
 * OVERVIEW:
 *  
 *	Sets baud rate, character size in bits, number of stop bits and party
 *  
 * FORM OF CALL:
 *  
 *	int combott_lineset ( int uport, int line_param)
 *  
 * RETURNS:
 *
 *	msg_success
 *      msg_failure
 *       
 * ARGUMENTS:
 *
 *	int	uport	- index in 'com_devtab'		
 *
 *      int	line_param - has 4 fields (as passed by VMS):
 * bits [0-7] - baud rate, currently VMS supports following rates:
 *		6 - 300, 8 - 1200, 11 - 2400, 13 - 4800, 15 -9600, 16 - 19200
 *             0xff means 'use current rate'   
 * 
 * bits [8-15] - character size, where
 *             0 - 5bits, 1 - 6bits, 2 - 7bits, 3 - 8bits,
 *             0xff means 'use current size'
 *
 * bits [16-23] - number of stop bits where
 *             0 - 0 stop bits, 1 - 1 stop bit, 2 - 1.5 stop bits, 3 - 2 stop bits,
 *             0xff means 'use current number of stop bits'
 *
 * bits [24-31] - parity where
 *             0 - disabled, 1 - odd parity, 2 - parity disabled/even parity set (same as 0)
 *             3 - even parity.
 *             0xff means 'use current setting for parity'
 *
-*/


int combott_lineset ( int uport, int line_param) 
 {
    struct	COM_DEVTAB	*cp;

    int	vms_rate;		/* value in bits [0:7] of 'line_param'   */
    char char_size_value;	/* value in bits [8:15] of 'line_param'  */
    char stop_bits_value;	/* value in bits [16:23] of 'line_param' */
    char parity_value;		/* value in bits [24:31] of 'line_param' */

    char lcr_save;		

/*    struct	LCRR	 lcr_new; */

    cp = &com_devtab[uport];
		
    vms_rate = line_param & 0xff;
    char_size_value	=  (line_param >> 8) & 0xff;
    stop_bits_value	=  (line_param >> 16) & 0xff;
    parity_value	=  (line_param >> 24) & 0xff;
/*
   qprintf ("combott_lineset:line_param-0x%x, vms_rate - %d, char_size - %d, stop_bits_value - %d, parity_value - %d\n",
    line_param, vms_rate, char_size_value, stop_bits_value, parity_value);
*/

	/* return failure if number of stop bits is 0 or  (is 1.5 and char.size is more then 5bits)*/
    if(((char_size_value == 5) && (stop_bits_value == 2)) || (stop_bits_value == 0))
     {
	return (msg_failure);
     }


	/* now fix the 'stop_bits_value' to 0 or 1 as required by combo hardware */
    if(stop_bits_value != 0xff)
     {
       (stop_bits_value == 1) ? (stop_bits_value = 0) : (stop_bits_value = 1);
     }

		  /* save Line Control Reg */
    lcr_save = combo_inportb (cp->port + LCR);	  

	/* modify baud rate if required */
    if (vms_rate != 0xff)
     {
	combo_outportb (cp->port + LCR, lcr_save | LCR$M_DLA); 	/* access divisor latches */
	/* set divisors to  line speed in vms_baud_table[vms_rate] */
        combo_outportb (cp->port + DLL, (char) (vms_baud_table[vms_rate] & 0xff));      
        combo_outportb (cp->port + DLM, (char) ((vms_baud_table[vms_rate] >> 8) & 0xff));      
        combo_outportb (cp->port + LCR, lcr_save);
     }
/*
    qprintf ("combott_lineset:after writing divisor regs\n");
    qprintf ("combott_lineset:CP->port+DLL 0x%x, CP->port+DLM - 0x%x\n", (cp->port + DLL), (cp->port + DLM));
*/


	/* change char. size */

    if (char_size_value != 0xff)
      {
	lcr_save = (lcr_save & ~LCR$M_WLS) | char_size_value;
      }

	/* change  stop bits */
    if (stop_bits_value != 0xff)
      {
	lcr_save = (lcr_save & ~LCR$M_SBS) | (stop_bits_value << 2);
      }

	/* change parity */
    if (parity_value != 0xff)
      {
	lcr_save = (lcr_save & ~LCR$M_PE) | ((parity_value & 1) << 3);
	lcr_save = (lcr_save & ~LCR$M_EPS) | (((parity_value>>1) & 1) << 4);
      }
/*
    qprintf ("combott_lineset: LCR value - %x \n", lcr_save);
*/
	/* set new value for LCR */
    combo_outportb (cp->port + LCR, lcr_save); 


    return (msg_success);	    /* 0 on success, 1 on failure */
}

void set_serial_params( int com, int baud, int flow, int modem )
    {
    struct COM_DEVTAB *cp;

    switch( com )
	{
	case COM1:
#if GALAXY && (SABLE || RAWHIDE)
	    if (whoami () != 0) return;
#endif
	    cp = com_devtab + 0;
	    break;
	case COM2:
#if GALAXY && (SABLE || RAWHIDE)
	    if (whoami () != 1) return;
#endif
	    cp = com_devtab + 1;
	    break;
	default:
	    return;
	}
    cp->baud = baud;
    cp->flow = flow;
    cp->modem = modem;
    if (flow & 1)
	cp->ttpb->flow = XON;
    else
	cp->ttpb->flow = 0;
    }

void combo_serial_reinit( )
    {
    com_devtab [0].ttpb = 0;
    com_devtab [0].pb = 0;
    com_devtab [0].ier = 0;
    com_devtab [0].baud = 0;
    com_devtab [0].flow = 0;
    com_devtab [0].modem = 0;
    com_devtab [1].ttpb = 0;
    com_devtab [1].pb = 0;
    com_devtab [1].ier = 0;
    com_devtab [1].baud = 0;
    com_devtab [1].flow = 0;
    com_devtab [1].modem = 0;
    }
