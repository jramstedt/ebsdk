/* file:	tt_driver.c
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
 *      Cobra Firmware
 *
 *  ABSTRACT:	TT class driver
 *
 *  Interface between port drivers and I/O routines
 *
 *
 *  AUTHORS:
 *
 *	AJ Beaverson
 *
 *  CREATION DATE:
 *  
 *	19-Dec-1990	Adapted from adutt_driver
 *
 *
 *  MODIFICATION HISTORY:
 *
 *	dwr	15-Sep-1993	Added code to tt_write so 2ndary's create a 
 *				tt_write() process on the primary. Also added 
 *				sec_tt_write and include file opt_turbo.h. 
 *
 *	rnf	30-Jul-1993	Added include file: opt_medulla.h
 *
 *      pel     23-Jun-1993     Add tt0_allow_login for Morgan so both serial
 *				ports can run loopback tests at once w/o a
 *				shell eating the characters from those ports.
 *
 *      cbf     22-Jan-1992     disable ^t, ^x and ^o when console secure
 *
 *	pel	15-Jan-1993	Conditionalize for morgan
 *
 *	cbf	03-Nov-1992     added set_ctrl_state for SECURE features
 *
 *	jad	09-Sep-1992	Move switch_to_tty to here.
 *
 *	pel	20-Aug-1992	don't start a shell when char rcvd on port w/o
 *				shell unless port blk allow_login flag set
 *				via set tt_allow_login 0
 *
 *	ajb	14-Jul1-1992	Put in JENSEN hooks
 *
 *	ajb	15-Jan-1992	handshake new shell creation in the ^x action
 *				routine with the new shell's registering with
 *				the ^c handler.
 *
 *	sfs	14-Nov-1991	Temporarily add Cobra-specific TT_OUT_POLLED
 *				and TT_IN_POLLED.
 *
 *	jad	08-Aug-1991	Add support for cb_proto
 *
 *	ajb	9-May-1991	Control C tweaks.  Broadcast the ^c to a (small)
 *				list of pids.
 *
 *	djm	19-Apr-1991	Added set_passthru routine.
 *
 *	jad	12-Apr-1991	Fix extra character bug.
 *
 *	jad	04-Apr-1991	Change the calling sequence to tt_setmode.
 *
 *	pel	12-Mar-1991	let fopen/close incr/decr inode ref & collapse
 *				wrt_ref, rd_ref into just ref.
 *
 *	kl	01-Mar-1991	Txon only if interrupt mode
 *
 *	sfs	19-Feb-1991	Use a dispatch table in TTPB to call the
 *				port driver.  Remove TT_DELETE.  Add code to
 *				TT_CONNECT.
 *
 *	dtm	12-Feb-1991	add 8530 port driver init.
 *
 *	pel	07-Feb-1991	make the device writable.
 *
 *	ajb	04-feb-1991	change calling sequence to xx_read and xx_write
 *
 *	sfs	25-Jan-1991	Enable interrupts on ADU
 *
 *	ajb	16-Jan-1991	Don't print ^c
 *
 *	ajb	02-Jan-1991	Added flow control
 *
 *--
 */

#include	"cp$src:platform.h"
#include	"cp$src:common.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:ansi_def.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:tt_def.h"
#include "cp$inc:kernel_entry.h"
#include "cp$inc:prototypes.h"

extern struct LOCK spl_kernel;
extern int null_procedure ();		/* Null procedure	*/
extern int shell_startup ();
extern struct QUEUE pollq;
extern int main_shell_pid;
extern int primary_cpu;
extern int timer_cpu;
extern int in_console;
extern int cbip;
extern int halt_pending;
extern struct QUEUE pcbq;
#if ALCOR
extern gbl_ctrl_x_flag;
#endif
#if !TURBO
extern int controlp_enable;
#else
#define controlp_enable (!secure_switch())
#endif

#if SECURE
extern int secure;
#endif

extern struct TTPB *console_ttpb;
extern struct QUEUE tt_pbs;

struct SEMAPHORE tt_control_sem;

extern int tt_inited;

extern int graphics_console;
extern int both_console;
extern int graphics_enabled;

int tt_read ();
int tt_write ();
int tt_open ();
int tt_setmode ();

extern struct DDB *tt_ddb_ptr;

struct DDB tt_ddb = {
	"tt",			/* how this routine wants to be called	*/
	tt_read,		/* read routine				*/
	tt_write,		/* write routine			*/
	tt_open,		/* open routine				*/
	null_procedure,		/* close routine			*/
	null_procedure,		/* name expansion routine		*/
	null_procedure,		/* delete routine			*/
	null_procedure,		/* create routine			*/
	tt_setmode,		/* setmode				*/
	null_procedure,		/* validation routine			*/
	0,			/* class specific use			*/
	0,			/* allows information			*/
	0,			/* must be stacked			*/
	0,			/* is a flash update driver		*/
	0,			/* is a block device			*/
	1,			/* not seekable				*/
	0,			/* is an ethernet device		*/
	0,			/* is a filesystem driver		*/
};


#if ALCOR
/*									    */
/* The following global provides srom_put_char() support.		    */
/* If the srom_enabled flag is set, output of the tt_polled routine is	    */
/* redirected to the srom port instead of the comm port.		    */
/*									    */
extern int *srom_flag_ptr;	/* defined in alcor.c			    */
#endif



/*+
 * ============================================================================
 * = tt_tx_interrupt - ISR for the SSC transmitter                            =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Output the next characters from the transmit queue for the SSC serial
 *	line.
 *  
 * FORM OF CALL:
 *  
 *	tt_tx_interrupt (ttpb)
 *  
 * RETURNS:             
 *
 *	0 - nothing
 *       
 * ARGUMENTS:           
 *
 *	struct TTPB *ttpb	- port block
 *
 * SIDE EFFECTS:
 *
 *
-*/
void tt_tx_interrupt (struct TTPB *ttpb) {

	struct TTYQ	*txqp;
        char		c;
	int		in;

	txqp = & ttpb->txq;
        in = txqp->in & txqp->qmask;

	/*
	 * Stop transmitting if we have been throttled by an XOFF or the
	 * transmitter is empty.
	 */                                                
	if ((ttpb->flow == XOFF) || (in == txqp->out)) {
	    if (ttpb->mode == DDB$K_INTERRUPT)
		ttpb->txoff (ttpb->port);
	    
	/*
	 * We have a character to send out
	 */
	} else {
	    while (ttpb->txready (ttpb->port) && (in != txqp->out)) {
		c = txqp->buf [txqp->out];
		txqp->out = (txqp->out + 1) & txqp->qmask;
		ttpb->txsend (ttpb->port, c);
	    }
	}

	/*
	 * Post any waiter
	 */
	krn$_bpost (&txqp->ready);
}

/*+
 * ============================================================================
 * = tt_rx_interrupt - ISR for the SSC receiver 			      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Read in the characters from the SSC uart and stuff it into the
 *	typeahead buffer.  No flow control or buffer overflow is detected.
 *  
 * FORM OF CALL:
 *  
 *	tt_rx_interrupt (ttpb)
 *  
 * RETURNS:
 *
 *	0 - nothing
 *       
 * ARGUMENTS:
 *
 *	struct TTPB *ttpb	- port block
 *
 * SIDE EFFECTS:
 *
 *
-*/
void tt_rx_interrupt (struct TTPB *int_ttpb)
{
	struct TTPB	*ttpb;
	struct TTYQ	*rxqp;
	struct TTYQ	*final_rxqp;
	char		c;

	ttpb = int_ttpb;
	rxqp = &ttpb->rxq;
	final_rxqp = &ttpb->rxq;

	/*
	 * Read in the characters until the ready bit doesn't light
	 */
	while (1) {
	    ttpb = int_ttpb;
	    rxqp = &ttpb->rxq;

	    if (!ttpb->rxready (ttpb->port))
		break;

	    c = ttpb->rxread (ttpb->port);

#if DEBUG            
	    if (c == CTRL_P) {
		pprintf ("\n*** ^P ***\n");
	    }

	    if (c == CTRL_V) {
		char state[25];
		struct QUEUE *p;
		struct PCB *pcb;
		struct TTPB *ttpb;
		struct TTYQ *txqp;
		int in;
		char c;

		spinlock (&spl_kernel);
		pprintf ("\n*** ^V ***\n");
		pprintf (" ID       PCB     Pri CPU Time Affinity CPU  Program    State\n");
		pprintf ("-------- -------- --- -------- -------- --- ---------- ------------------------\n");
		p = pcbq.flink;
		while (p != &pcbq) {
		    pcb = (struct PCB *) ((unsigned int) p - offsetof (struct PCB, pcb$r_pq));
		    switch (pcb->pcb$l_pstate) {
			case KRN$K_NULL:
			    sprintf (state, "null");
			    break;
			case KRN$K_WAIT:
			    sprintf (state, "waiting on %-13.13s", pcb->pcb$a_semp->name);
			    break;
			case KRN$K_READY:
			    sprintf (state, "ready");
			    break;
			case KRN$K_RUNNING:
			    sprintf (state, "running");
			    break;
			default:
			    sprintf ("?\n");
			    break;
		    }
		    pprintf ("%08x %08x %d %10d %08x %d %12.12s %s\n",
			pcb->pcb$l_pid,
			pcb,
			pcb->pcb$l_pri,
			pcb->pcb$q_cputime [0],
			pcb->pcb$l_affinity,
			pcb->pcb$l_last_processor,
			pcb->pcb$b_name,
			state
		    );
		    p = p->flink;
		}
		ttpb = tt_pbs.flink;
		while (ttpb != &tt_pbs.flink) {
		    txqp = &ttpb->txq;
		    in = txqp->in & txqp->qmask;
		    while (in != txqp->out) {
			c = txqp->buf [txqp->out];
			pprintf ("%s: %02x `%c'\n", ttpb->ip->name, c, c);
			txqp->out = (txqp->out + 1) & txqp->qmask;
		    }
		    ttpb = ttpb->ttpb.flink;
		}
		spinunlock (&spl_kernel);
	    }
#endif
	    /*
	     * If this is the console terminal, and the primary CPU is not in
	     * console mode, then the only thing we do is to check for ^P.
	     */
	    if ((ttpb == console_ttpb) && !(in_console & (1 << primary_cpu))) {
		if (controlp_enable && (c == CTRL_P)) {
		    halt_pending = 1;
		}
		continue;
	    }

	    /*
	     * If this character was just received on a port without a shell,
	     * start one up.
	     */
#if TURBO
	    if (ttpb->login_ena) {
		if (main_shell_pid
			&& (ttpb->ctrlch.flink == &ttpb->ctrlch)
			&& (ttpb->ctrlch.blink == &ttpb->ctrlch)) {
		    if (both_console && graphics_enabled) {
			ttpb = console_ttpb;
			rxqp = &ttpb->rxq;
			final_rxqp = &ttpb->rxq;
		    } else {
			ttpb->ctrl_x_rec = 1;
			krn$_bpost (&tt_control_sem);
			if (c == CR)
			    continue;
		    }
		}
	    }
#endif

#if !TURBO
	    if (ttpb->login_ena) {
		if (main_shell_pid
			&& (ttpb->ctrlch.flink == &ttpb->ctrlch)
			&& (ttpb->ctrlch.blink == &ttpb->ctrlch)) {
		    ttpb->ctrl_x_rec = 1;
		    krn$_bpost (&tt_control_sem);
		    if (c == CR)
			continue;
		}
	    }
#endif
	    /*
	     * Flow control enabled and an XON received.  Start the transmitter.
	     */
	    if (ttpb->flow && (c == CTRL_C)) {
		ttpb->flow = XON;
		if (ttpb->mode == DDB$K_INTERRUPT)
		    ttpb->txon (ttpb->port);
	    }

	    if (ttpb->flow && (c == XON)) {
		ttpb->flow = XON;
		if (ttpb->mode == DDB$K_INTERRUPT)
		    ttpb->txon (ttpb->port);

	    /*
	     * Flow control enabled and an XOFF received.  Stop the transmitter.
	     */
	    } else if (ttpb->flow && (c == XOFF)) {
		ttpb->flow = XOFF;

	    /*
	     * Control characters only have special meaning if logins are
	     * allowed; if logins are not allowed, pass all characters
	     * received straight into the typeahead buffer.
	     */
	    } else if (ttpb->login_ena) {

#if MODULAR || XDELTA_ON
		/*
		 * Control D enabled and received
		 */
		if (ttpb->ctrl_d_ena && (c == CTRL_D)) {
#if SECURE
		    if (!secure) {
#endif
  			ttpb->ctrl_d_rec = 1;
			krn$_bpost (&tt_control_sem);
#if SECURE
		    }
#endif
		} else
#endif
		/*
		 * Control O enabled and received
		 */
		if (ttpb->ctrl_o_ena && (c == CTRL_O)) {
  			ttpb->ctrl_o_rec = 1;
			krn$_bpost (&tt_control_sem);

		/*
		 * Control X enabled and received
		 */
		} else if ((c == CTRL_X) && (ttpb->ctrl_x_ena)) {
   		      ttpb->ctrl_x_rec = 1;
		      krn$_bpost (&tt_control_sem);

		/*
		 * Control C enabled and received
		 */
		} else if (ttpb->ctrl_c_ena && (c == CTRL_C)) {
		    ttpb->ctrl_c_rec = 1;

		    rxqp->buf [rxqp->in] = CTRL_C;	/* put ^C in buffer */
		    rxqp->in = (rxqp->in + 1) & rxqp->qmask;

		    krn$_bpost (&tt_control_sem);

		/*
		 * Control T enabled and received
		 */
		} else if (ttpb->ctrl_t_ena && (c == CTRL_T)) {
			ttpb->ctrl_t_rec = 1;
			krn$_bpost (&tt_control_sem);


                /*
	         * Echo ^G as the bell.   And don't put ^G into buffer.
	         * This supports secure mode indication.  
	         */
	        } else if (c == CTRL_G) {
         	    ttpb->txsend (ttpb->port, CTRL_G);


		/*
		 * Regular character, insert into typeahead buffer
		 */
		} else {
		    rxqp->buf [rxqp->in] = c;
		    rxqp->in = (rxqp->in + 1) & rxqp->qmask;
		}

	    /*
	     * Logins not allowed, insert into typeahead buffer
	     */
	    } else {
		rxqp->buf [rxqp->in] = c;
		rxqp->in = (rxqp->in + 1) & rxqp->qmask;

	    }

	}

	/*
	 * Post any waiter
	 */
	rxqp = final_rxqp;
	krn$_bpost (&rxqp->ready);
}

/*+
 * ============================================================================
 * = tt_control - invoke a routine when a semaphore is posted                 =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine invokes an action routine whenever a terminal character
 *	semaphore is posted.  Conceptually, this can be thought of as binding
 *	a code fragment to a semaphore's being posted.
 *  
 * FORM OF CALL:
 *  
 *	tt_control ()
 *  
 * RETURNS:
 *
 *	never returns
 *       
 * ARGUMENTS:
 *	none
 *
 * SIDE EFFECTS:
 *
 *
-*/
void tt_control () {
	struct PCB *pcb;
	struct TTPB *ttpb;

	pcb = getpcb();
	while (1) {
	    krn$_wait (&tt_control_sem);
	    spinlock (&spl_kernel);
	    ttpb = tt_pbs.flink;
	    spinunlock (&spl_kernel);
	    while (ttpb != &tt_pbs.flink) {
		if (!ttpb->fp) {
		    ttpb->fp = fopen (ttpb->ip->name, "w");
		}
		pcb->pcb$a_stdout = ttpb->fp;
		if (ttpb->ctrl_o_rec) {
		    ttpb->ctrl_o_rec = 0;
		    control_o_action (ttpb);
		}
		if (ttpb->ctrl_c_rec) {
		    ttpb->ctrl_c_rec = 0;
		    control_c_action (ttpb);
		}
		if (ttpb->ctrl_x_rec) {
		    ttpb->ctrl_x_rec = 0;
		    control_x_action (ttpb);
		}
		if (ttpb->ctrl_t_rec) {
		    ttpb->ctrl_t_rec = 0;
		    control_t_action (ttpb);
		}
#if MODULAR || XDELTA_ON
		if (ttpb->ctrl_d_rec) {
		    ttpb->ctrl_d_rec = 0;
		    control_d_action (ttpb);
		}
#endif
		spinlock (&spl_kernel);
		ttpb = ttpb->ttpb.flink;
		spinunlock (&spl_kernel);
	    }
	}
}

void control_o_action (struct TTPB *ttpb) {
	struct PCB *pcb;

	pcb = getpcb();
	if (ttpb->stop) {
	    ttpb->stop = 0;
	    printf ("^O\n");
	} else {
	    ttpb->stop = 0;
	    printf ("^O\n");
	    ttpb->stop = 1;
	}
}

/*
 * Set the control C pending bit in a queue of processes.
 */
void control_c_action (struct TTPB *ttpb) {
	struct PCB *pcb;
	struct QUEUE	*qp;

	ttpb->stop = 0;

	/*
	 * Walk down the notification list and light the control C bit in
	 * each process.
	 */
	spinlock (&spl_kernel);
	qp = ttpb->ctrlch.flink;
	while (qp != &ttpb->ctrlch.flink) {
	    pcb = (int) qp - offsetof (struct PCB, pcb$r_ctrlcq);
	    pcb->pcb$l_killpending |= SIGNAL_CONTROLC;
	    qp = qp->flink;
	}
	spinunlock (&spl_kernel);

}

/*
 * Detach current foreground process from the shell and put it in
 * the background.
 */
void control_x_action (struct TTPB *ttpb)
{
	int sh ();
	int null_procedure ();
	int	ipl;
	struct TTYQ *rxqp;
	struct PCB *pcb;
	struct QUEUE	*qp;
	char	**argv;
	int	pid;
	extern int shell_stack;

	ttpb->stop = 0;

	/*
	 * Tell all foreground shells to go away.  We look down the control
	 * c list of pids to find the shells. We set a bit to exit
	 * when the current command is complete.
	 */
	spinlock (&spl_kernel);
	qp = ttpb->ctrlch.flink;
	while (qp != &ttpb->ctrlch.flink) {
	    pcb = (int) qp - offsetof (struct PCB, pcb$r_ctrlcq);
	    if (pcb->pcb$l_shellp) {
		sh_request_exit (pcb->pcb$l_shellp);
	    }
	    qp = qp->flink;
	}
	spinunlock (&spl_kernel);

	ipl = mtpr_ipl (IPL_SYNC);

	rxqp = &ttpb->rxq;
	rxqp->buf [rxqp->in] = CTRL_X;		/* put ^X in buffer */
	rxqp->in = (rxqp->in + 1) & rxqp->qmask;

	mtpr_ipl (ipl);
	krn$_bpost (&rxqp->ready);

	krn$_sleep (10);
	/*
	 * Create a new shell
	 */
        argv = malloc (2 * sizeof (char *));
	argv [0] = mk_dynamic ("shell");
	argv [1] = 0;
	pid = krn$_create (
		sh,		/* address of process		*/
		shell_startup,	/* startup routine		*/
		0,		/* completion semaphore		*/
		3,		/* process priority		*/
		1<<primary_cpu,	/* cpu affinity mask		*/
		shell_stack,	/* stack size			*/
		"shell",	/* process name			*/
		ttpb->ip->name, "r",
		ttpb->ip->name, "w",
		ttpb->ip->name, "w",
		1, argv);

	/*
	 * Wait until the newly created shell has registered itself.  If
	 * we don't wait, rapid ^x's don't kill previous shells.
	 */
#if ALCOR
	gbl_ctrl_x_flag = 1;		/* flag for NVR scrip bypass */
#endif
	while (1) {
	    if (pcb = krn$_findpcb (pid)) {
		if (pcb->pcb$l_shellp) {
		    spinunlock (&spl_kernel);
		    break;
		}
		spinunlock (&spl_kernel);
	    } else {
		break;
	    }
	    krn$_sleep (10);
	}
}

/*+
 * ============================================================================
 * = control_t - Process that handles control T 	                      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This process waits for control T semaphores to be posted, and then 
 *	acts on them.
 *  
 * FORM OF CALL:
 *  
 *	control_t (struct TTPB *ttpb)
 *  
 * RETURNS:
 *
 *       
 * ARGUMENTS:
 *
 *	struct TTPB *ttpb	- port block
 *
 * SIDE EFFECTS:
 *
-*/
void control_t_action (struct TTPB *ttpb)
{
    int	ipl;
    struct TTYQ	*rxqp;

    ttpb->stop = 0;				/* allow output */
    printf ("\n");

    krn$_walkpcb ();			/* print processes */

    ipl = mtpr_ipl (IPL_SYNC);

    rxqp = &ttpb->rxq;
    rxqp->buf [rxqp->in] = CTRL_T;		/* put ^T in buffer */
    rxqp->in = (rxqp->in + 1) & rxqp->qmask;

    mtpr_ipl (ipl);
    krn$_bpost (&rxqp->ready);
}

#if MODULAR || XDELTA_ON
/*
 * Take a breakpoint to xdelta
 */
void control_d_action (struct TTPB *ttpb)
{
    int	ipl;
    int change_graphics = 0;
    int save_graphics_console;
    int save_both_console;
    struct TTYQ	*rxqp;

    ttpb->stop = 0;
    ttpb->linecnt = 0;

#if MODULAR && !XDELATA_ON
    if (!xdelta_shared_adr) {
	xdelta_shared_adr = ovly_load("XDELTA");
	if (!xdelta_shared_adr) {
	    pprintf("Load of XDELTA failed\n");
	    return;
	}
	xdelta();
    }
#endif
    printf ("\nTransferring to xdelta\n");

    if ((graphics_enabled) && (!both_console)) {
	save_graphics_console = graphics_console;
	save_both_console = both_console;
	graphics_console = 0;
	both_console = 1;
	change_graphics = 1;
    }

    do_bpt();

    if (change_graphics) {
	graphics_console = save_graphics_console;
	both_console = save_both_console;
    }

    ipl = mtpr_ipl (IPL_SYNC);

    rxqp = &ttpb->rxq;
    rxqp->buf [rxqp->in] = CTRL_D;		/* put ^D in buffer */
    rxqp->in = (rxqp->in + 1) & rxqp->qmask;

    mtpr_ipl (ipl);
    krn$_bpost (&rxqp->ready);
}
#endif

/*+
 * ============================================================================
 * = tt_qinit - Initialize tty queues                                      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routines will initialize all the tty queues used by the ADU port
 *	driver.
 *
 * FORM OF CALL:
 *
 *	tt_qinit (q, size, prefix); 
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS: 
 *
 *      struct TTYQ *q - Queue to be initialized.
 *	int size - Queue size, must be a power of 2.
 *	char *prefix - Used for semaphore name.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
void tt_qinit (struct TTYQ *q, int size, char *prefix) {
	char	sname [MAX_NAME];	/* Semaphore name	*/

	/*
	 * Init the queue variables
	 */
	q->qmask = size - 1;
	q->buf = (char*) dyn$_malloc (size, DYN$K_SYNC|DYN$K_NOOWN);
	q->in = 0;
	q->out = 0;

	/*
	 * Make semaphore names and init the semaphores
	 */
	sprintf (sname, "%s_ready", prefix);
	krn$_seminit (&q->ready, 1, sname);
}


/*+
 * ============================================================================
 * = tt_write - Write a block of data                                         =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * 	Write a block of data to the transmit buffer.  Translate
 * 	newlines into CRLFs.
 *  
 * FORM OF CALL:
 *  
 *	tt_write(fp, int size, int number, buf)
 *  
 * RETURNS:
 *
 *      len - bytes transferred
 *
 *       
 * ARGUMENTS:
 *
 *	struct FILE *fp - Pointer to a file.
 *	int size	- size of item
 *	int number	- number of items
 *	char *buf - Pointer to the buffer the data is taken from.
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int tt_write (struct FILE *fp, int size, int number, char *buf) {
	struct TTPB	*ttpb;	/* port block pointer */
	struct TTYQ	*txqp;
	char		*s;
	int		l;
	int		saved_affinity;
	struct PCB	*pcb;

 	pcb = getpcb ();
	saved_affinity = pcb->pcb$l_affinity;

	/*
	 * If the primary is running the console, then force affinity to
	 * the primary for the duration of this call.
	 */
	if (in_console & (1<<primary_cpu)) {
	    pcb->pcb$l_affinity = 1 << primary_cpu;
	    reschedule (1);
	}

	ttpb = (struct TTPB *) fp->ip->misc;
	txqp = & ttpb->txq;

	l = size * number;
	s = buf;

	/*
	 * see if we're in immediate completion mode; if so, write as many
	 * characters as we can without waiting
	 */
	if (fp->mode & MODE_M_IMMEDIATE) {
	    if (cbip) {
		while ((--l >= 0) && ttpb->txready (ttpb->port)) {
		    ttpb->txsend (ttpb->port, *s++);
		}
		return s - buf;
	    }
	}

	fp->ip->len[0] += l;

	while (--l >= 0) {

	    if (ttpb->stop)
		break;

	    /*
	     * stall if the transmit buffer is full (allow extra space for
	     * newline translation).
	     */
	    while (((txqp->in - txqp->out) & txqp->qmask) >=
		(txqp->qmask - TX_HEADROOM)) {
		krn$_wait (&txqp->ready);
	    }

	    /*
	     * Translate newlines
	     */
	    if (ttpb->translate_nl && (*s == '\n')) {
		txqp->buf [txqp->in] = 0x0d;
		txqp->in = (txqp->in + 1) & txqp->qmask;
	    }
	    
	    txqp->buf [txqp->in] = *s++;
	    txqp->in = (txqp->in + 1) & txqp->qmask;

	    /*
	     * Start the transmitter
	     */
	    if (ttpb->mode == DDB$K_INTERRUPT)
		ttpb->txon (ttpb->port);
	}

	/*
	 * Wait till done
	 */
	while (txqp->in != txqp->out)
	    krn$_sleep (1);

	/*
	 * Restore original affinity (which may not have been changed).
	 */
	pcb->pcb$l_affinity = saved_affinity;
	reschedule (1);

	return size * number;
}

/*+
 * ============================================================================
 * = tt_read - Read a block of characters.                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * 	Read in a block of characters from the receive queue.  Read until the
 *	read length is satisfied or the queue is drained.
 *
 *  
 * FORM OF CALL:
 *  
 *	tt_read (fp, size, number, buf)
 *  
 * RETURNS:
 *
 *      s - Bytes transferred. Anything other than the amount requested
 *	    indicates EOF or an error.
 *       
 * ARGUMENTS:
 *
 *	struct FILE *fp - Pointer to a file.
 *	int size	- size if item
 *	int number	- number of items
 *	char *buf - Pointer to the buffer the data is written to.
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int tt_read (struct FILE *fp, int size, int number, char *buf) {
	struct TTPB	*ttpb;		/* Pointer to the port block */ 
	struct TTYQ	*rxqp;
	int		l;
	char		*s;

	ttpb = (struct TTPB *) fp->ip->misc;
	rxqp = & ttpb->rxq;

	l = size * number;
	s = buf;

	/*
	 * see if we're in immediate completion mode; if so, check for (and
	 * possibly return) a single character
	 */
	if (fp->mode & MODE_M_IMMEDIATE) {
	    if (cbip) {
		if (ttpb->rxready (ttpb->port)) {
		    *s = ttpb->rxread (ttpb->port);
		    if (ttpb == console_ttpb) {
			if (controlp_enable && (*s == CTRL_P)) {
			    halt_pending = 1;
			    return 0;
			}
		    }
		    return 1;
		}
	    } else {
		if (rxqp->in != rxqp->out) {
        	    *s = rxqp->buf [rxqp->out];
		    rxqp->out = (rxqp->out + 1) & rxqp->qmask;
		    return 1;
		}
	    }
	    return 0;
	}

	/*
	 * stall if the receive queue is empty
	 */
	while (rxqp->in == rxqp->out) {
	    krn$_wait (&rxqp->ready);
	    if (killpending ()) {
		break;
	    }
	}

	/*
	 * Read until we drain the buffer or the read is satisfied.
         */	   
	while ((--l >= 0) && (rxqp->out != rxqp->in)) {	
            *s = rxqp->buf [rxqp->out];
	    if (*s == 0x1a) {
		if (s == buf) {
		    rxqp->out = (rxqp->out + 1) & rxqp->qmask;
		}
		break;
	    }
	    s++;
	    rxqp->out = (rxqp->out + 1) & rxqp->qmask;
	}

	return s - buf;
}

/*+
 * ============================================================================
 * = tt_flush - Flush typeahead.                                              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * 	Flush typeahead by emptying the receive queue of each terminal
 *	port block.
 *
 *  
 * FORM OF CALL:
 *  
 *	tt_flush ()
 *  
 * SIDE EFFECTS:
 *
 *	All typeahead buffers are flushed.
 *
-*/
void tt_flush () {
	struct TTPB *ttpb;
	struct TTYQ *rxqp;

	spinlock (&spl_kernel);
	ttpb = tt_pbs.flink;
	while (ttpb != &tt_pbs.flink) {
	    rxqp = & ttpb->rxq;
	    rxqp->in = rxqp->out;
	    ttpb = ttpb->ttpb.flink;
	}
	spinunlock (&spl_kernel);
}

int tt_open (struct FILE *fp) {
	struct INODE *ip;
	int status = msg_success;

	if (strcmp (fp->ip->name, "tt") == 0) {
	    fp->ip->ref--;
	    ip = console_ttpb->ip;
	    if (ip) {
		strcpy (fp->iob.name, ip->name);
		fp->ip = ip;
		fp->ip->ref++;
	    } else {
		status = msg_failure;
	    }
	}
	return status;
}

/*+
 * ============================================================================
 * = tt_init - Initialize the console serial line driver.                     =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Initialize the console serial line driver.  Since this is a class
 *	driver, it must be started before any port drivers.  The class driver
 *	is "discovered" by the port drivers as they come on line.
 *
 *	TT_INIT is called as a phase 0 driver, which means that there is no
 *	timer process, or any other process (other than this one) running.
 *	This means that it can't block or use krn$_sleep.
 *  
 * FORM OF CALL:
 *  
 *	tt_init ()
 *  
 * RETURNS:
 *
 *	None
 *       
 * ARGUMENTS:
 *
 *	None
 *
 * SIDE EFFECTS:
 *
 *	Creates processes tt_rx,tt_tx.
 *	Allocates inode ip.
 *
-*/
int tt_init () {
	struct INODE	*ip;
	extern		tt_setmode ();

	tt_ddb_ptr = &tt_ddb;

	/*
	 * initialize the port block queue
	 */
	tt_pbs.flink = &tt_pbs.flink;
	tt_pbs.blink = &tt_pbs.flink;

	/*
	 * Create a process that handles control character requests
	 */
	krn$_seminit (&tt_control_sem, 0, "tt_control");

	krn$_create (
	    tt_control,		/* process address	*/
	    null_procedure,	/* startup routine	*/
	    0,			/* completion semaphore */
	    6,			/* process priority	*/
	    -1,			/* affinity mask	*/
	    0,			/* default stack size	*/
	    "tt_control",	/* process name		*/
	    "tt", "r",		/* stdin		*/
	    "tt", "w",		/* stdout		*/
	    "tt", "w"		/* stderr	    	*/
	);

	return msg_success;
}

/*
 * Allocate and fill in a tty port block.  Return the address of the port
 * block to the caller so the caller can fill in the rxready, txready etc 
 * routine addresses.  This routine also installs the port block onto the tty
 * class driver's port block queue.
 *
 * This routine returns with the kernel spinlock taken out so that the caller
 * may continue filling in the port block (i.e. the routine addresses) without
 * creating a window where the port block is not fully defined.  Caller's must
 * release the kernel spinlock.
 */
int tt_init_port (char *name) {
	struct TTPB	*ttpb;
	struct INODE	*ip;
	struct POLLQ	*x;
	extern		tt_poll ();

	/*
	 * Create an inode entry, thus making the device visible as a file.
	 */
        allocinode (name, 1, &ip);
	ip->dva = & tt_ddb;
	ip->attr = ATTR$M_READ | ATTR$M_WRITE;

	/*
	 * Allocate the port block
	 */
	ttpb = dyn$_malloc (sizeof (struct TTPB), DYN$K_SYNC|DYN$K_NOOWN);

	ip->misc = ttpb;
	ttpb->ip = ip;
	INODE_UNLOCK (ip);

	/*
	 * Initialize the ^c notification queue
	 */
	ttpb->ctrlch.flink = & ttpb->ctrlch.flink;
	ttpb->ctrlch.blink = & ttpb->ctrlch.flink;

	/* recall buffer initialization */
	rcb_init (ttpb);

	/*
	 * Set up control character recognition and processes
	 */
	ttpb->translate_nl = 1;
	ttpb->stop = 0;
	ttpb->flow = XON;
	ttpb->login_ena = 1;
	ttpb->ctrl_o_ena = 1;
	ttpb->ctrl_c_ena = 1;
	ttpb->ctrl_x_ena = 1;
	ttpb->ctrl_t_ena = 1;
	ttpb->ctrl_d_ena = 1;
	ttpb->ctrl_o_rec = 0;
	ttpb->ctrl_c_rec = 0;
	ttpb->ctrl_x_rec = 0;
	ttpb->ctrl_t_rec = 0;
	ttpb->ctrl_d_rec = 0;

	tt_qinit (&ttpb->rxq, RXQ_SIZE, "rxq");
	tt_qinit (&ttpb->txq, TXQ_SIZE, "txq");

	ttpb->pqp.flink = & ttpb->pqp.flink;
	ttpb->pqp.blink = & ttpb->pqp.flink;
	ttpb->pqp.routine = tt_poll;
	ttpb->pqp.param = ttpb;
	ttpb->mode = -1;
	ttpb->poll = 1;
	spinlock (&spl_kernel);
	insq (&ttpb->pqp.flink, &pollq);

	/*
	 * Put the port block on the pb queue (thus registering it with the
	 * class driver).
	 */
	insq (ttpb, tt_pbs.blink);

/*	spinunlock (&spl_kernel);	caller frees the spinlock */

	return ttpb;
}

/*+
 * ============================================================================
 * = tt_poll - poll the serial line interface for work                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine polls the serial line interface for work and posts the
 *	corresponding semaphores if there is work to be done on the queues.
 *  
 * FORM OF CALL:
 *  
 *	tt_poll (ttpb);
 *  
 * RETURNS:
 *
 *       
 * ARGUMENTS:
 *
 * 	struct TTPB *ttpb - pointer to port block
 *
 * SIDE EFFECTS:
 *
 *
-*/
void tt_poll (struct TTPB *ttpb) {
	int poll;
	int ipl;
	struct TTYQ	*qp;

	qp = &ttpb->rxq;
	krn$_bpost (&qp->ready);
	qp = &ttpb->txq;
	krn$_bpost (&qp->ready);
	if (ttpb->mode == DDB$K_INTERRUPT) return;

	if (in_console & (1<<primary_cpu))
	    poll = ttpb->poll & 1;
	else
	    poll = ttpb->poll & 2;

	ipl = mtpr_ipl (IPL_SYNC);

	if (poll) {
	    /*
	     * Poll for receiver work
	     */
	    if (ttpb->rxready (ttpb->port)) {
		tt_rx_interrupt (ttpb);
	    }
	}

	if (poll || (cbip && primary () && (ttpb == console_ttpb))) {
	    /*
	     * Poll for transmitter work
	     */
	    if (ttpb->txready (ttpb->port)) {
		tt_tx_interrupt (ttpb);
	    }
	}

	mtpr_ipl (ipl);
}

/*+
 * ============================================================================
 * = tt_setmode - Set mode                                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Set mode.
 *  
 * FORM OF CALL:
 *  
 *	tt_setmode ()
 *  
 * RETURNS:
 *
 *	msg_success - Normal completion.
 *       
 * ARGUMENTS:
 *
 *	int mode		- new mode
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
void tt_setmode (int mode) {
	struct TTPB *ttpb;

	if (mode == DDB$K_READY) return;
	spinlock (&spl_kernel);
	ttpb = tt_pbs.flink;
	spinunlock (&spl_kernel);
	while (ttpb != &tt_pbs.flink) {
	    tt_setmode_pb (ttpb, mode);
	    spinlock (&spl_kernel);
	    ttpb = ttpb->ttpb.flink;
	    spinunlock (&spl_kernel);
	}
}

void tt_setmode_pb (struct TTPB *ttpb, int mode) {

	switch (mode) {
	/*
	 * Start or stop driver
	 */
	case DDB$K_START:
	    mode = ttpb->perm_mode;
	    ttpb->poll = 1;
	    break;

	case DDB$K_STOP:
	    mode = DDB$K_POLLED;
	    if (ttpb == console_ttpb)
		ttpb->poll = 1;
	    else
		ttpb->poll = ttpb->perm_poll;
	    break;
	}

#if TURBO
	if (ttpb->perm_mode == DDB$K_POLLED)
	    mode = ttpb->perm_mode;
#endif

	if (ttpb->mode == mode) return;
	ttpb->mode = mode;

	switch (mode) {
	/*
	 * Transition to interrupt mode
	 */
	case DDB$K_INTERRUPT:
#if 0
	    /*
	     * disengage driver from poll queue
	     */	
	    remq_lock (&ttpb->pqp.flink);
#endif

	    /*
	     * Restart interrupts
	     */
	    ttpb->txon (ttpb->port);
	    ttpb->rxon (ttpb->port);
	    break;

	/*
	 * Transition to polled mode
	 */
	case DDB$K_POLLED:
	    /*
	     * Turn off the interrupts
	     */
	    ttpb->txoff (ttpb->port);
	    ttpb->rxoff (ttpb->port);

#if 0
	    /*
	     * insert polling queue element onto the poll queue
	     */
	    ttpb->pqp.routine = tt_poll;
	    ttpb->pqp.param = ttpb;
	    insq_lock (&ttpb->pqp.flink, &pollq);
#endif
            break;
	}
}

/*+
 * ============================================================================
 * = tt_out_polled - Puts a character in the buffer (polled).                 =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine will put a character in the transmit buffer. It
 *	it runs in a polled environment.
 *  
 * FORM OF CALL:  
 *
 *	tt_out_polled (c)  
 *  
 * RETURNS:
 *
 *	None
 *       
 * ARGUMENTS:
 *
 *	char c - Character to be put in the buffer.
 *
 * GLOBALS
 *
 *	srom_flag_ptr - defined in powerup_alcor.c
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
void tt_out_polled (unsigned char c) {
	int	ipl;
	int	i;

#if ALCOR
	/*								    */
	/* The following code redirects output to the serial rom port	    */
	/* instead of sending it to the COMM port if srom_flag_ptr has been */
	/* set.								    */
	/*								    */
	if (*srom_flag_ptr)
	    {
	    ipl = mtpr_ipl (IPL_SYNC);
	    if (c == '\n')
		{
		srom_put_char (0xa);
		srom_put_char (0xd);
		} 
	    else 
		{
		srom_put_char (c);
		}
	    mtpr_ipl (ipl);
	    return;
	    }	    
#endif

 	ipl = mtpr_ipl (IPL_SYNC);

	if (console_ttpb->flow == XOFF) {
	    for (i = 0; i < 100000; i++) {
		if (console_ttpb->rxready (console_ttpb->port)) {
		    c = console_ttpb->rxread (console_ttpb->port);
		    if ((c == XON) || (c == CTRL_C)) {
			break;
#if !TURBO
		    } else if (cbip && controlp_enable && (c == CTRL_P)) {
			halt_pending = 1;
#endif
		    }
		}
	    }
	    if (console_ttpb->mode == DDB$K_INTERRUPT) {
		console_ttpb->flow = 1;
	    } else {
		console_ttpb->flow = XON;
	    }
	}

	if (console_ttpb->translate_nl && (c == '\n')) {
	    while (!console_ttpb->txready (console_ttpb->port));
	    console_ttpb->txsend (console_ttpb->port, 0x0d);
	}

	while (!console_ttpb->txready (console_ttpb->port));
	console_ttpb->txsend (console_ttpb->port, c);

	if (console_ttpb->rxready (console_ttpb->port)) {
	    c = console_ttpb->rxread (console_ttpb->port);
	    if (c == XOFF) {
		while (1) {
		    while (!console_ttpb->rxready (console_ttpb->port));
		    c = console_ttpb->rxread (console_ttpb->port);
		    if ((c == XON) || (c == CTRL_C)) {
			break;
#if !TURBO
		    } else if (cbip && controlp_enable && (c == CTRL_P)) {
			halt_pending = 1;
#endif
		    }
		}
#if !TURBO
	    } else if (cbip && controlp_enable && (c == CTRL_P)) {
		halt_pending = 1;
#endif
	    }
	}
	mtpr_ipl (ipl);
}


/*+
 * ============================================================================
 * = tt_in_polled - Takes a character from the buffer (polled).               =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine will take a character from the receive buffer. It
 *	it runs in a polled environment.
 *  
 * FORM OF CALL:
 *
 *	tt_in_polled ()  
 *  
 * RETURNS:
 *
 *	c - Character to be taken from the buffer.
 *       
 * ARGUMENTS:
 *
 *	None
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int tt_in_polled () {
	char	c;

	int	ipl;

 	ipl = mtpr_ipl (IPL_SYNC);

	while (!console_ttpb->rxready (console_ttpb->port));
	c = console_ttpb->rxread (console_ttpb->port);

 	mtpr_ipl (ipl);

	return c;
}

/*+
 * ============================================================================
 * = tt_in_check - Checks for and takes a character from the buffer (polled). =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine checks for a character available and takes a character
 *	from the receive buffer if available, other return 0. It runs in a
 *	polled environment.
 *  
 * FORM OF CALL:
 *
 *	tt_in_check ()  
 *  
 * RETURNS:
 *
 *	c - Character taken from the buffer.
 *	0 - No character available.
 *       
 * ARGUMENTS:
 *
 *	None
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int tt_in_check () {
	char	c;

	int	ipl;

 	ipl = mtpr_ipl (IPL_SYNC);

	c = 0;

	if (console_ttpb->rxready (console_ttpb->port))
	    c = console_ttpb->rxread (console_ttpb->port);

 	mtpr_ipl (ipl);

	return c;
}


/*+
 * ============================================================================
 * = switch_to_tty - Switches the tty.                                        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *
 * We start out with our print routine tied to pprintf since the tty
 * driver doesn't exist yet.  The disadvantage of this is that pprint
 * is unsynchronized.  We switch the to tty driver as soon as possible.
 *
 *  
 * FORM OF CALL:
 *
 *	switch_to_tty ()  
 *  
 * RETURNS:
 *
 *	None
 *       
 * ARGUMENTS:
 *
 *	None
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/

int switch_to_tty () {
	struct PCB *pcb;

	/*
	 * If the tty driver is already our stdin, then return
	 */
	pcb = getpcb ();
	if (pcb->pcb$a_stdin->ip->dva == & tt_ddb) {
	    return 0;
        }

	/*
	 * See if the tty driver is online yet
	 */
	if (tt_inited) {
	    fclose (pcb->pcb$a_stdin);
	    fclose (pcb->pcb$a_stdout);
	    fclose (pcb->pcb$a_stderr);
	    pcb->pcb$a_stdin_name =  "tt";
	    pcb->pcb$a_stdout_name = "tt";
	    pcb->pcb$a_stderr_name = "tt";

	    pcb->pcb$a_stderr = fopen (pcb->pcb$a_stderr_name, 
					pcb->pcb$b_stderr_mode);
	    pcb->pcb$a_stdout = fopen (pcb->pcb$a_stdout_name, 
					pcb->pcb$b_stdout_mode);
	    pcb->pcb$a_stdin = fopen (pcb->pcb$a_stdin_name, 
					pcb->pcb$b_stdin_mode);
	    return 1;
	} else {
	    return 0;
	}
}

void tt_check_controlp () {
#if !TURBO
	unsigned char c;

	while (console_ttpb->rxready (console_ttpb->port)) {
	    c = console_ttpb->rxread (console_ttpb->port);
	    if (controlp_enable && (c == CTRL_P)) {
		halt_pending = 1;
	    }
	}
#endif
}

#if MONET
void halt_reprompt()
{
    int	ipl;
    struct TTPB *ttpb;
    struct TTYQ	*rxqp;

    spinlock (&spl_kernel);
    ttpb = (struct TTPB *)tt_pbs.flink;
    spinunlock (&spl_kernel);

    while ((struct TTPB *)ttpb != (struct TTPB *)&tt_pbs.flink) {

	ipl = mtpr_ipl (IPL_SYNC);

	ttpb->linecnt = 0;

	rxqp = (struct TTYQ *)&ttpb->rxq;
	rxqp->buf [rxqp->in] = 0x0d;		/* put CR in buffer */
	rxqp->in = (rxqp->in + 1) & rxqp->qmask;

	mtpr_ipl (ipl);
	krn$_bpost (&rxqp->ready, 1);

	spinlock (&spl_kernel);
	ttpb = (struct TTPB *)ttpb->ttpb.flink;
	spinunlock (&spl_kernel);
    }
}

void clear_linecnt()
{
    int	ipl;
    struct TTPB *ttpb;

    spinlock (&spl_kernel);
    ttpb = (struct TTPB *)tt_pbs.flink;

    while ((struct TTPB *)ttpb != (struct TTPB *)&tt_pbs.flink) {
	ttpb->linecnt = 0;
	ttpb = (struct TTPB *)ttpb->ttpb.flink;
    }
    spinunlock (&spl_kernel);
}
#endif
