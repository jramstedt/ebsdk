/* file:	ie.c
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
 *	Interrupt/Exception utilities
 *
 *  AUTHORS:
 *
 *      K LeMieux
 *
 *  CREATION DATE:
 *  
 *      14-Nov-1990
 *
 *  MODIFICATION HISTORY:
 *
 *      tna     11-Nov-1998     Conditionalized for YUKONA
 *
 *	rbb	13-Mar-1995	Conditionalize for the EB164
 *
 *	dwb	04-Nov-1994	On Mikasa unexpected interrupt, disable the
 *				interrupt vector and issue and EOI if eisa. 
 *				only report the event to the event_log.
 *
 *	jmp	30-nov-1993	Add Epic Interrupt Handler for AVANTI
 *
 *	sfs	09-Dec-1991	Add alignment fault reporting.
 *
 *	jrk	17-May-1991	add OPTIONAL conditional
 *
 *	ajb	28-Feb-1991	Fix minor alignment nit in call to malloc
 *
 *	kl	20-Feb-1990	Have all system-level exception handlers check
 *				for process-level exception handlers before 
 *				executing.
 *
 *	kl	25-Jan-1991	Change VAX interrupts/exceptions to follow Alpha
 *
 *	sfs	21-Jan-1991	Temporarily disable ADU timer interrupts.
 *
 *	kl	11-Jan-1990	Additional Alpha SCB initialization
 *
 *	kl 	13-Dec-1990	Connect process specific exception handling.
 *
 *	ajb	26-Nov-1990	Add default unexpected interrupt/exception
 *				catcher.
 *
 *	kl	14-Nov-1990	Initial entry.
 *--
 */


#include	"cp$src:platform.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:stddef.h"
#include	"cp$src:common.h"
#include	"cp$inc:prototypes.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:pd_def.h"
#include	"cp$src:alphascb_def.h"
#include	"cp$src:ctype.h"
#include	"cp$src:platform_cpu.h"

#define TRUE 1
#define FALSE 0

#ifdef MAX_SCB_ENTRIES
#undef SCB_ENTRIES
#define SCB_ENTRIES	MAX_SCB_ENTRIES
#endif

#if TURBO
#undef SCB_ENTRIES
#define SCB_ENTRIES	768
extern int	cbip;
extern int eat_interrupts[MAX_PROCESSOR_ID];
#include "cp$src:turbo_eeprom.h"
extern int xdelta_shared_adr;
extern int xdelta_startup;
#endif

#if RAWHIDE
extern int	cbip;
#endif

extern int 	spl_kernel;
void	default_handler();
extern struct file *el_fp;
unsigned int	*scb;		/* the scb itself		*/
struct SCBV	*scbv;		/* parallel structure		*/

/*
 * Keep from infinitely looping if the default_handler generates
 * an unexpected exception
 */
int unexpected_in_progress = 0;

/* pdlist removed, 1-Oct-93, jrk */

struct PDLIST pdlist [] = {
  {0, 0}
};

#if 0
#if EXTRA
extern struct PDLIST pdlist[];
#endif
#endif

#if AVANTI || K2
extern void	avanti_csr_dump ();
#endif

extern int	common_isr ();
int		access_violation ();
int		unaligned_fault ();

/*+
 * ============================================================================
 * = Default_handler - default handler                                        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *   Default handler - dump process state
 *
 * FORM OF CALL:
 *
 *	default_handler(vector)
 *
 * RETURNS:
 *
 *      None
 *
 * ARGUMENTS:
 *
 *      int 	exc_vector		- vector index
 *
 * SIDE EFFECTS:
 *
 *	None
-*/
void default_handler (int index) {
	struct PCB *pcb;
	struct ALPHA_CTX *acp;
	int *fp;
	int *sp;
	int pc;
	int ptr;
	int id = whoami();

	pcb = getpcb ();
	acp = pcb->pcb$a_acp;

#if TURBO || RAWHIDE
	/*
	 * Hopefully, a temporary solution to pending VMS IO device interrupts 
	 * during callbacks.
	 */

	if (( index >= 0x80 ) && ( cbip )) {
	    qprintf("Dismissing OS generated interrupt during IO callback - vector %x0 \n", index);
	    return;
	}
#if TURBO
	if (eat_interrupts[id]) {
	    qprintf("Dismissing interrupt during startup - vector %x0 \n", index);
	    return;			/* HACK! - ignore interrupts if eat_interrupts[id] is set */
	}
#endif
#endif

#if !(SABLE || MEDULLA || CORTEX || YUKONA || RAWHIDE || TURBO)
	if (index >= 0x80) {
	    if (el_fp) {
		fprintf (el_fp, "unexpected exception/interrupt through vector %X0\n", index);
	    }
            io_disable_interrupts (0, index);
            io_issue_eoi (0, index);
	    return;
	}
#endif

	pprintf ("CPU%d: unexpected exception/interrupt through vector %X0\n", id, index);

#if SABLE
	if (index >= 0x60)
	    return;
#endif

#if TURBO

        /* print exception type string as shown in SRM */

	print_exception_type(index*0x10);

#endif
	if (unexpected_in_progress) {
	   pprintf("\nNested exception - console restarting\n");
           console_restart();
	}
        unexpected_in_progress = 1;

#if TURBO
	ptr = malloc(sizeof(struct HALT_LOG));
	read_halt_data(ptr);
	print_halt_data(ptr, 0, id);
	free(ptr);
#endif

	pprintf ("process %s, pcb = %08X\n\n", pcb->pcb$b_name, pcb);
	pprintf (" pc: %08X %08X  ps: %08X %08X\n",
	    acp->acx$q_pc [1], acp->acx$q_pc [0],
	    acp->acx$q_ps [1], acp->acx$q_ps [0]);
	pprintf (" r2: %08X %08X  r5: %08X %08X\n",
	    acp->acx$q_exc_r2 [1], acp->acx$q_exc_r2 [0],
	    acp->acx$q_exc_r5 [1], acp->acx$q_exc_r5 [0]);
	pprintf (" r3: %08X %08X  r6: %08X %08X\n",
	    acp->acx$q_exc_r3 [1], acp->acx$q_exc_r3 [0],
	    acp->acx$q_exc_r6 [1], acp->acx$q_exc_r6 [0]);
	pprintf (" r4: %08X %08X  r7: %08X %08X\n",
	    acp->acx$q_exc_r4 [1], acp->acx$q_exc_r4 [0],
	    acp->acx$q_exc_r7 [1], acp->acx$q_exc_r7 [0]);

/*Walk the stack*/

	fp = acp->acx$q_r29[0];
	sp = acp->acx$q_r30[0];
	pc = acp->acx$q_pc[0] - 4;

	walk_stack (fp, sp, pc, acp, 1);

#if TURBO
	if (xdelta_shared_adr) {
	    pprintf("\nConsole Crash... Type ;P to restart\n");
	    xdelta_startup = 1;
	    do_bpt ();
	    xdelta_startup = 0;
	}
#else
	do_bpt ();
#endif
	unexpected_in_progress = 0;
	console_restart ();
}

#define pd_kind_null 8
#define pd_kind_fp_stack 9
#define pd_kind_fp_register 10

struct pd {
    unsigned kind : 4;
    unsigned handler_valid : 1;
    unsigned handler_reinvokable : 1;
    unsigned handler_data_valid : 1;
    unsigned base_reg_is_fp : 1;
    unsigned rei_return : 1;
    unsigned stack_return_value : 1;
    unsigned base_frame : 1;
    unsigned fill1 : 1;
    unsigned native : 1;
    unsigned no_jacket : 1;
    unsigned tie_frame : 1;
    unsigned fill2 : 1;
    variant_union {
	unsigned short int rsa_offset;
	variant_struct {
	    unsigned char save_fp;
	    unsigned char save_ra;
	    } fill6;
	} fill7;
    long fill3;
    unsigned long int entry;
    long fill4;
    unsigned long int size;
    long fill5;
    unsigned long int ireg_mask;
    unsigned long int freg_mask;
    } ;

static int wsip = 0;

void sysfault_walk_stack( int *fp, int *sp, int pc, int acp, int p_flag )
{
    int id = whoami();

    pprintf("\nSYSFAULT CPU%d - pc = %08x\n", id, pc);
    walk_stack(fp, sp, pc, acp, p_flag);
}

void walk_stack( int *fp, int *sp, int pc, int acp, int p_flag )
    {
    char c1;
    char c2;
    int i;
    int pflag;
    int irsm;
    int frsm;
    int *rsa;
    int regs[31];
    struct PCB *pcb;
    struct ALPHA_CTX *acx;
    struct pd *pd;
    struct PDLIST *pde;
    char *sym;
    int id = whoami();

    if( wsip ) {
	pprintf("\nDouble Exception from CPU %d\n", id);
	krn$_sleep(5000);	/* sleep 5 secs to allow printing */
    	console_restart( );
    }
    wsip = 1;

#if MODULAR
    acx = acp;
    pflag = p_flag;
#else
    pcb = getpcb( );
    acx = pcb->pcb$a_acp;
    pflag = 1;
#endif

    if (acx) {
    regs[0] = acx->acx$q_r0[0];
    regs[1] = acx->acx$q_r1[0];
    regs[2] = acx->acx$q_r2[0];
    regs[3] = acx->acx$q_r3[0];
    regs[4] = acx->acx$q_r4[0];
    regs[5] = acx->acx$q_r5[0];
    regs[6] = acx->acx$q_r6[0];
    regs[7] = acx->acx$q_r7[0];
    regs[8] = acx->acx$q_r8[0];
    regs[9] = acx->acx$q_r9[0];
    regs[10] = acx->acx$q_r10[0];
    regs[11] = acx->acx$q_r11[0];
    regs[12] = acx->acx$q_r12[0];
    regs[13] = acx->acx$q_r13[0];
    regs[14] = acx->acx$q_r14[0];
    regs[15] = acx->acx$q_r15[0];
    regs[16] = acx->acx$q_r16[0];
    regs[17] = acx->acx$q_r17[0];
    regs[18] = acx->acx$q_r18[0];
    regs[19] = acx->acx$q_r19[0];
    regs[20] = acx->acx$q_r20[0];
    regs[21] = acx->acx$q_r21[0];
    regs[22] = acx->acx$q_r22[0];
    regs[23] = acx->acx$q_r23[0];
    regs[24] = acx->acx$q_r24[0];
    regs[25] = acx->acx$q_r25[0];
    regs[26] = acx->acx$q_r26[0];
    regs[27] = acx->acx$q_r27[0];
    regs[28] = acx->acx$q_r28[0];
    regs[29] = acx->acx$q_r29[0];
    regs[30] = acx->acx$q_r30[0];

    if (pflag) {
    pprintf( "\nexception context saved starting at %08X\n", acx );

    pprintf ("\nGPRs:\n");
    pprintf ("  0: %08X %08X  16: %08X %08X\n",
	    acx->acx$q_r0 [1], acx->acx$q_r0 [0],
	    acx->acx$q_r16 [1], acx->acx$q_r16 [0]);
    pprintf ("  1: %08X %08X  17: %08X %08X\n",
	    acx->acx$q_r1 [1], acx->acx$q_r1 [0],
	    acx->acx$q_r17 [1], acx->acx$q_r17 [0]);
    pprintf ("  2: %08X %08X  18: %08X %08X\n",
	    acx->acx$q_r2 [1], acx->acx$q_r2 [0],
	    acx->acx$q_r18 [1], acx->acx$q_r18 [0]);
    pprintf ("  3: %08X %08X  19: %08X %08X\n",
	    acx->acx$q_r3 [1], acx->acx$q_r3 [0],
	    acx->acx$q_r19 [1], acx->acx$q_r19 [0]);
    pprintf ("  4: %08X %08X  20: %08X %08X\n",
	    acx->acx$q_r4 [1], acx->acx$q_r4 [0],
	    acx->acx$q_r20 [1], acx->acx$q_r20 [0]);
    pprintf ("  5: %08X %08X  21: %08X %08X\n",
	    acx->acx$q_r5 [1], acx->acx$q_r5 [0],
	    acx->acx$q_r21 [1], acx->acx$q_r21 [0]);
    pprintf ("  6: %08X %08X  22: %08X %08X\n",
	    acx->acx$q_r6 [1], acx->acx$q_r6 [0],
	    acx->acx$q_r22 [1], acx->acx$q_r22 [0]);
    pprintf ("  7: %08X %08X  23: %08X %08X\n",
	    acx->acx$q_r7 [1], acx->acx$q_r7 [0],
	    acx->acx$q_r23 [1], acx->acx$q_r23 [0]);
    pprintf ("  8: %08X %08X  24: %08X %08X\n",
	    acx->acx$q_r8 [1], acx->acx$q_r8 [0],
	    acx->acx$q_r24 [1], acx->acx$q_r24 [0]);
    pprintf ("  9: %08X %08X  25: %08X %08X\n",
	    acx->acx$q_r9 [1], acx->acx$q_r9 [0],
	    acx->acx$q_r25 [1], acx->acx$q_r25 [0]);
    pprintf (" 10: %08X %08X  26: %08X %08X\n",
	    acx->acx$q_r10 [1], acx->acx$q_r10 [0],
	    acx->acx$q_r26 [1], acx->acx$q_r26 [0]);
    pprintf (" 11: %08X %08X  27: %08X %08X\n",
	    acx->acx$q_r11 [1], acx->acx$q_r11 [0],
	    acx->acx$q_r27 [1], acx->acx$q_r27 [0]);
    pprintf (" 12: %08X %08X  28: %08X %08X\n",
	    acx->acx$q_r12 [1], acx->acx$q_r12 [0],
	    acx->acx$q_r28 [1], acx->acx$q_r28 [0]);
    pprintf (" 13: %08X %08X  29: %08X %08X\n",
	    acx->acx$q_r13 [1], acx->acx$q_r13 [0],
	    acx->acx$q_r29 [1], acx->acx$q_r29 [0]);
    pprintf (" 14: %08X %08X  30: %08X %08X\n",
	    acx->acx$q_r14 [1], acx->acx$q_r14 [0],
	    acx->acx$q_r30 [1], acx->acx$q_r30 [0]);
    pprintf (" 15: %08X %08X\n",
	    acx->acx$q_r15 [1], acx->acx$q_r15 [0]);
	}
    }

    pprintf( "\n" );

#if MODULAR
    if (pflag)
	ovly_identify();
#endif

    if (pflag)
	pprintf( "dump of active call frames:\n" );

    while( fp && ( pd = ( *fp & 7 ) ? (int)fp : *fp ) ) {
	pprintf( "\n" );
#if SYMBOLS
	pprintf( "PC  =  %08X\n", pc );
	pprintf( "PD  =  %08X ", pd );
	sym = sym_table_lookup(pd);
	if (sym)
	    pprintf("(%s)\n", sym);
	else
	    pprintf("\n");
#else
	pde = find_pd( pd );
	if( pde )
	    pprintf( "PC  =  %08X (%s + %08X)\n", pc, pde->name, pc - pd->entry );
	else
	    pprintf( "PC  =  %08X\n", pc );
	pprintf( "PD  =  %08X\n", pd );
#endif
    if (pflag) {
	pprintf( "FP  =  %08X\n", fp );
	pprintf( "SP  =  %08X\n", sp );
    }
	if( pd->kind == pd_kind_fp_stack )
	    {
	    if( pd->base_reg_is_fp )
		if( pd->rsa_offset )
		    rsa = (int)fp + pd->rsa_offset;
		else
		    {
		    pprintf( "bad PD; base register is FP, but RSA is 0\n" );
		    return;
		    }
	    else
		if( pd->rsa_offset )
		    rsa = (int)sp + pd->rsa_offset;
		else
		    {
		    pprintf( "bad PD; procedure is register based\n" );
		    return;
		    }
	    irsm = ( pd->ireg_mask & 0x2fffffff );
	    frsm = ( pd->freg_mask & 0x7fffffff );
	    if( irsm || frsm )
		{
		int *temp_rsa;

		temp_rsa = rsa + 2;
		if (pflag)
		    pprintf( "\n" );

		for( i = 0; i < 32; i++ )
		    if( pd->ireg_mask & ( 1 << i ) )
			{
			if (pflag)
			    pprintf( "R%d ", i );
			regs[i] = *temp_rsa;
			temp_rsa += 2;
			}
		for( i = 0; i < 32; i++ )
		    if( pd->freg_mask & ( 1 << i ) ) {
			if (pflag)
			    pprintf( "F%d ", i );
		    }
		if (pflag)
		    pprintf( "saved starting at %08X\n\n", rsa );
		    for( i = 0; i < 32; i++ )
			if( pd->ireg_mask & ( 1 << i ) ) {
			    if (pflag)
				pprintf( "R%-2d =  %08X\n", i, regs[i] );
			}
		}
	    i = count_bits( irsm );
	    fp = rsa[i*2];
	    sp = (int)sp + pd->size;
	    pc = rsa[0];
	    regs[29] = fp;
	    regs[30] = sp;
	    }
	else if( pd->kind == pd_kind_fp_register )
	    {
	    if (pflag)
		pprintf( "\nFP saved in R%d, PC saved in R%d\n", pd->save_fp, pd->save_ra );
	    fp = regs[pd->save_fp];
	    sp = (int)sp + pd->size;
	    pc = regs[pd->save_ra];
	    regs[29] = fp;
	    regs[30] = sp;
	    }
	else
	    {
	    pprintf( "bad PD; KIND =  %d\n", pd->kind );
	    return;
	    }
	}
    pprintf( "\n" );
    wsip = 0;
    }

#if EXTRA
int find_pd( int pd )
    {
    struct PDLIST *pde;

    pde = pdlist;
    while( pde->pd )
	{
	if( pde->pd == pd )
	    return( pde );
	pde++;
	}
    return( 0 );
    }
#else
int find_pd( int pd )
    {
    return( 0 );
    }
#endif

int count_bits( unsigned long int n )
    {
    int i;

    i = 0;
    while( n )
	{
	if( n & 1 )
	    i++;
	n >>= 1;
	}
    return( i );
    }

/*+
 * ============================================================================
 * = Default_exception_handler - default system level exception handler	      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * The default system level exception handler. 
 *
 * FORM OF CALL:
 *
 *	default_exception_handler (exc_vector) 
 *
 * RETURNS:
 *
 *
 * ARGUMENTS:
 *
 *      int 	exc_vector		- exception vector 
 *
 * SIDE EFFECTS:
 *
 *	None
-*/
void default_exception_handler (int exc_vector) {

	if (p_exception_dispatch ( exc_vector) == msg_failure)
	 default_handler(exc_vector);
}


/*+
 * ============================================================================
 * = exception - determine if vector index corresponds to an exception        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *   Determine if vector index corresponds to an exception        
 *
 * FORM OF CALL:
 *
 *	exception( exc_vector ) 
 *
 * RETURNS:
 *
 *      TRUE
 *	FALSE
 *
 * ARGUMENTS:
 *
 *      int 	exc_vector		- vector index
 *
 * SIDE EFFECTS:
 *
 *	None
-*/
int exception(int exc_vector) {
	/*
	 * Alpha SCB exception mask 
	 */

	unsigned int exc_mask[4] = { 0x00001f02,0xffff00f1,0x0000000f,0x00000000
	 		   	    };

	 if (exc_vector <= MAX_EXC_VECTOR) {
	 if ((1 << (exc_vector % 32)) & exc_mask[exc_vector / 32]) 
	   return TRUE;
	 }
	 return FALSE;
}



/*+
 * ============================================================================
 * = int_vector_set - Set an interrupt vector                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	If the current interrupt handler in the SCB vector slot is the default 
 *	handler then the default SCB handler is replaced with the new handler and the 
 *	default SCB parameter is replaced with the new parameter. If the current
 *	interrupt handler is not the default handler then an error message is
 *	returned.
 *
 *	It is the responsibility of the process attempting to establish a new 
 *	interrupt handler to determine how to handle an error return from this 
 *	routine.
 *
 * FORM OF CALL:
 *
 *	int_vector_set( int_vector,handler,param ) 
 *
 * RETURNS:
 *
 *      msg_success
 *	msg_failure
 *
 * ARGUMENTS:
 *
 *      int 	int_vector		- the SCB vector slot
 *	int	handler			- new SCB handler
 *	int 	p0			- new SCB parameter
 *	int 	p1			- new SCB parameter
 *
 * SIDE EFFECTS:
 *
 *	None
-*/
int int_vector_set(int int_vector, int handler, protoargs int p0, int p1)
{
        unsigned int	*lp;

	spinlock(&spl_kernel);
	if  ( ((exception(int_vector)) && ( scbv [int_vector].scbv$l_pd == default_exception_handler)) ||
             ((!(exception(int_vector))) && ( scbv [int_vector].scbv$l_pd == default_handler)) ) {
	       scbv [int_vector].scbv$l_pd = handler;
	       scbv [int_vector].scbv$l_p0 = p0;
	       scbv [int_vector].scbv$l_p1 = p1;
	       spinunlock(&spl_kernel);
	       return msg_success;

	} else {
	   spinunlock(&spl_kernel);
	   return msg_failure;
        }

}
/*+
 * ============================================================================
 * = int_vector_clear - Clear an interrupt vector                             =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	The current interrupt handler, and current SCB parameter for the SCB 
 *	vector slot is replaced with the default handler and default SCB
 *	parameter respectively.  	
 *
 *	It is the responsibilty of the process to release any interrupt handlers
 *	it has established. This should be done in a process specific rundown
 * 	routine to ensure that these handlers are released.
 *
 * FORM OF CALL:
 *
 *	int_vector_clear( int_vector ) 
 *
 * RETURNS:
 *
 *      msg_success
  *
 * ARGUMENTS:
 *
 *      int 	int_vector		- the SCB vector slot
  *
 * SIDE EFFECTS:
 *
 *	None
-*/
int int_vector_clear(int int_vector)
{
	unsigned int	*lp;

	spinlock(&spl_kernel);
	if (exception(int_vector)) {
         scbv [int_vector].scbv$l_pd = default_exception_handler;
        } else {
	 scbv [int_vector].scbv$l_pd = default_handler;
        }
	scbv [int_vector].scbv$l_p0 = int_vector;
	scbv [int_vector].scbv$l_p1 = int_vector;
	spinunlock(&spl_kernel);
	return (int) msg_success;	   

}

/*+
 * ============================================================================
 * = exc_vector_set - Set an exception vector                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	The current exception handler for the given exception vector is replaced
 *	with the new handler and the handler parameter is replaced with the new 
 *	parameter. 
 *	No checking of the existing entry is performed.
 *
 * FORM OF CALL:
 *
 *	exc_vector_set( exc_vector,handler,param ) 
 *
 * RETURNS:
 *
 *      msg_success
 *
 * ARGUMENTS:
 *
 *      int 	exc_vector		- vector
 *	int	handler			- new exception handler
 *	int 	param			- new exception handler parameter
 *
 * SIDE EFFECTS:
 *
 *	None
-*/
int exc_vector_set(int exc_vector, int handler, protoarg int param)
{
	struct HQE	*hqp;
	struct PCB	*pcb;	
	 /*
	  * If this entry as specified by the exception vector is already in the queue then
	  * update the fields of the entry.
	  */

	 if ((hqp = find_handler(exc_vector)) != msg_failure) {
	   hqp->handler = handler;
	   hqp->param = param;
	   return (int) msg_success;
	} else {

	/*
	 * If this entry is not in the queue then allocate an entry and load the fields.
	 */

	   hqp = (void *) dyn$_malloc (sizeof (struct HQE), DYN$K_SYNC | DYN$K_OWN);
	   hqp->vector = exc_vector;
	   hqp->handler = handler;
	   hqp->param = param;

	/*
	 * Put the entry on the process' exception handler queue
	 */
           pcb = getpcb();	   
	   insq (hqp, &pcb->pcb$r_hq);
	   return (int) msg_success;	   
	}
}

/*+
 * ============================================================================
 * = exc_vector_clear - Clear an exception vector                             =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * The process' handler queue is searched for the given entry as specified by the
 * vector. If an entry is found then that entry is removed form the queue.
 *
 *
 * FORM OF CALL:
 *
 *	exc_vector_clear( exc_vector ) 
 *
 * RETURNS:
 *
 *      msg_success
 *	msg_failure
 *
 * ARGUMENTS:
 *
 *      int 	exc_vector		- the SCB vector slot
 *
 * SIDE EFFECTS:
 *
 *	None
-*/
int exc_vector_clear(int exc_vector)
{
	struct HQE	*hqp;
	
	 if ((hqp = find_handler(exc_vector)) != msg_failure) {
	  remq (hqp);
	  dyn$_free(hqp,DYN$K_SYNC);
	 } else {
	  return (int) msg_failure;
	 }
}

/*+
 * ============================================================================
 * = p_exception_dispatch - Dispatch to process exception handler             =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * This routine searches the process exception handler queue for a handler for the
 * given exception. If a handler has been defined by the process then that handler 
 * is called otherwise return.
 *
 * FORM OF CALL:
 *
 *	p_exception_dispatch( exc_vector,param ) 
 *
 * RETURNS:
 *
 *      msg_success
 *	msg_failure
 *
 * ARGUMENTS:
 *
 *      int 	exc_vector		- exception vector 
 *
 * SIDE EFFECTS:
 *
 *	None
-*/
int p_exception_dispatch(int exc_vector)
{
	struct HQE	*hqp;

	 if ((hqp = find_handler(exc_vector)) != msg_failure) {
	  (*hqp->handler) (hqp->param);
	  return (int) msg_success;
	 } else {
	  return (int) msg_failure;
	 }
}
	

/*+
 * ============================================================================
 * = find_handler - find process specific handler                             =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * Search through the process' handler queue for a handler for the given 
 * vector offset. 
 *
 * FORM OF CALL:
 *
 *	find_handler( exc_vector ) 
 *
 * RETURNS:
 *
 *	pointer to found handler entry
 *	msg_failure
 *
 * ARGUMENTS:
 *
 *      int 	exc_vector		- exception vector 
 *
 * SIDE EFFECTS:
 *
 *	None
-*/
int find_handler(int exc_vector)
{
	struct HQE	*hqp;
	struct PCB	*pcb;
	struct QUEUE	*p;

	pcb = getpcb();
	p = pcb->pcb$r_hq.flink;
	while (p != &pcb->pcb$r_hq) {
	 hqp = (struct HQE *) (unsigned int) p;
	 if (hqp->vector == exc_vector) {
		return hqp;
		break;
	 }
	p = p->flink;
	}
	return (int) msg_failure;
}


/*+
 * ============================================================================
 * = scb_init - set up the machine dependent SCB                              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Allocate and fill in the SCB for the system.  Default the entries in 
 *	the SCB to some default handler.  This routine normally called by the
 *	primary processor at powerup.
 *  
 * FORM OF CALL:
 *  
 *	scb_init ()
 *  
 * RETURNS:
 *
 *	msg_success
 *       
 * ARGUMENTS:
 *
 *
 * SIDE EFFECTS:
 *
 *
-*/
void scb_init () {
	unsigned int	*lp;
	unsigned int	*pd;
	int		i;
	

	/*
	 * Set up the SCB.  Over allocate and then page align the SCB.
	 * initialize SCB vectors with exception/interrupt handlers.
	 */

	scb = (void *) dyn$_malloc (
	    SCB_ENTRIES * SCB_ENTRY_SIZE,
	    DYN$K_NOSYNC | DYN$K_ALIGN,
	    1024 * 8,
	    0
	);
	scbv = dyn$_malloc (
	    SCB_ENTRIES * sizeof (struct SCBV),
	    DYN$K_NOSYNC
	);


	for (i=0; i<SCB_ENTRIES; i++) {
	    pd = getpd( common_isr );
	    lp = (int) scb + i * SCB_ENTRY_SIZE;
	    *lp++ = pd [2];
	    *lp++ = 0;
	    *lp++ = & scbv [i];
	    *lp++ = 0;
	    if (exception(i)) {
              scbv [i].scbv$l_pd = default_exception_handler;
            } else {
	      scbv [i].scbv$l_pd = default_handler;
            }
	    scbv [i].scbv$l_p0 = i;
	    scbv [i].scbv$l_p1 = i;
	 }


	/*
	 *  Set up alignment fault handlers.
	 */
	int_vector_set( SCB$Q_UNALIGNED, unaligned_fault, 0, 0 );
	int_vector_set( SCB$Q_ACCVIO, access_violation, 0, 0 );

#if AVANTI
	/*
	 * Set up handler for EPIC error interrupt
	 */
	int_vector_set( 0x94, avanti_csr_dump );
#endif

}

void unaligned_fault( )
    {
    int rw;
    struct PCB *pcb;
    struct ALPHA_CTX *acx;

    pcb = getpcb( );
    acx = pcb->pcb$a_acp;
    rw = acx->acx$q_exc_r5[0];
    if( rw )
	report_fault( "unaligned store" );
    else
	report_fault( "unaligned load" );

#if TURBO
    if (xdelta_shared_adr) {
	pprintf("\nConsole Crash... Type ;P to restart\n");
	xdelta_startup = 1;
	do_bpt ();
	xdelta_startup = 0;
    }
    console_restart ();
#else
    do_bpt( );
#endif
}

void access_violation( )
    {
    struct PCB *pcb;
    struct ALPHA_CTX *acx;

    pcb = getpcb( );
    acx = pcb->pcb$a_acp;
    acx->acx$q_pc[0] += 4;
    report_fault( "access violation" );

#if TURBO
    if (xdelta_shared_adr) {
	pprintf("\nConsole Crash... Type ;P to restart\n");
	xdelta_startup = 1;
	do_bpt ();
	xdelta_startup = 0;
    }
    console_restart ();
#else
    do_bpt( );
#endif
    }

void report_fault( char *msg )
    {
    struct PCB *pcb;
    struct ALPHA_CTX *acx;
    int *fp;
    int *sp;
    int pc[2];
    int va[2];
    struct pd *pd;
    struct PDLIST *pde;

    pcb = getpcb( );
    acx = pcb->pcb$a_acp;
    fp = acx->acx$q_r29[0];
    sp = acx->acx$q_r30[0];
    pc[0] = acx->acx$q_pc[0] - 4;
    pc[1] = acx->acx$q_pc[1];
    va[0] = acx->acx$q_exc_r4[0];
    va[1] = acx->acx$q_exc_r4[1];
    pd = ( *fp & 7 ) ? (int)fp : *fp;
    pde = find_pd( pd );
    if( pde )
	pprintf( "\n%s fault\n    PCB =  %08X (%s)\n    PC  =  %08X %08X (%s + %08X)\n    VA  =  %08X %08X\n",
		msg,
		pcb,
		pcb->pcb$b_name,
		pc[1], pc[0],
		pde->name,
		pc - pd->entry,
		va[1], va[0] );
    else
	pprintf( "\n%s fault\n    PCB =  %08X (%s)\n    PC  =  %08X %08X\n    VA  =  %08X %08X\n",
		msg,
		pcb,
		pcb->pcb$b_name,
		pc[1], pc[0],
		va[1], va[0] );
    walk_stack( fp, sp, pc[0], acx, 1 );
    }

int getpd( int i )
    {
    return( i );
    }
