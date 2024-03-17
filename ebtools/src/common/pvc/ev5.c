/* Implement the ev5 processor, in particular */
/* #define pir_test */

/*
 *	  
 *	  Copyright (c) 1992, 1993
 *	  Digital Equipment Corporation, Maynard, Massachusetts 01754
 *	  
 *	  This software is furnished under a license for use only on a single computer
 *	  system  and  may be  copied  only  with the inclusion of the above copyright
 *	  notice.  This software, or  any other copies thereof, may not be provided or
 *	  otherwise made  available  to any other person except for use on such system
 *	  and to one who agrees to these license terms.  Title to and ownership of the
 *	  software shall at all times remain in DEC.
 *	  
 *	  The information  in  this software  is  subject to change without notice and
 *	  should not be construed as a commitment by Digital Equipment Corporation.
 *	  
 *	  DEC  assumes no responsibility for the use or reliability of its software on
 *	  equipment which is not supplied by DEC.
 *	  
 *	  
 *	  REVISION HISTORY:		
 *	  
 *	  Rev	Who	When		What		
 *	  ----------------------------------------------------------------------
 *	  0     gpc     AUG-92          First Created
 *        1     pir     Jan-93		Updated model to reflect real EV5 issues rules,
 *					IPRs, etc.
 *        2     jem     Sep-93		Updated with new and changed restrictions
 *			Oct-93		Added checks for first cycles of flow & other "special" checks
 *	  3	jem	Dec-93		Added check for writing paltemps in shadow of any mbox instr.
 *					Added checks for mtpr aster,astrr to hw_rei
 *	  4	jem	Jun-94		Fixed up format of error output for "special checks"
 *					Added mt itb_asn -> mt itbis check
 *					Added mt dc_mode/mcsr -> hwrei_stall error check
 *	  				Changed bunch of mt mbox checks from just cycle 2 to cycle 1&2.
 * 	  5     jem     Mar-95          Added some restrictions around mf dc_test_tag and mt dcperr_stat
 *	  
 *	  V3.23 fls	Jul-96		Merged VMS only with EBSDK version.
 *			Since most of the changes were in the VMS only 
 *			version, it is used as the base and the following 
 *			EBSDK edits have been included:
 * 			$Log: ev5.c,v $
 * 			Revision 1.1.1.1  1998/12/29 21:36:23  paradis
 * 			Initial CVS checkin
 * 			
 * 			Revision 1.3  1995/11/14  21:29:07  cruz
 * 			Added function prototypes and removed unused variables.
 *
 * 			Revision 1.2  1995/05/23  22:09:16  cruz
 * 			V3.20: There were a few places where the error count 
 *			was not being updated, even though there was an error.
 *        		I fixed that :)
 *			These EBSDK version changes are marked with V3.23.
 *
 *	  V3.24 fls	Sep-96 added check for ipr gtr than MAX_IPR in
 *			decode_ipr routine. This caused a hard to debug
 *			bad stack crash when running ev6 palcode with set
 *			cpu ev5. 
 *			added check for index <= 0 in slot_insts routine.
 *			replaced 676 with MAX_INDEX.
 *		        - added hw_rei decode to decode_hw_memory 
 */

/*
 * This module must implement the following function
 *
 *	void ev5_init (Chip *)
 *		
 *		This function must assign valid function pointers to
 *		each of the fields of the structure passed as an argument.
 */

#include "types.h"
#include "ispdef.h"
#include "rest.h"
#include "sched.h"
#include "memory.h"
#include "opcode.h"	/*V3.24*/
#include "ev5.h"
#include "body.h"


static enum classes 	{di_none, di_ld, di_ldx_l, di_st, di_mbx,
			   di_mxpr, di_ibr, di_fbr, di_jsr, di_iaddlog,
			   di_shift, di_cmov, di_icmp, di_imull, di_imulq, 
			   di_imulh, di_fadd, di_fdiv, di_fmul, di_fcpys, 
			   di_epicode, di_hwrei, di_max};

static enum pipes 	{ps_e0=1, ps_e1=2, ps_fa=4, ps_fm=8}; 

static enum resources 	{Nores, Isr, Itb_tag, Itb_pte, Itb_asn, Itb_pte_temp,
			   Itbia, Itbiap, Itbis, Sicr, Ifault_va_form, Ivptbr,
			   Exc_addr, Exc_sum, Exc_mask, Pal_base, Ps, Ipl,
			   Intid, Astrr, Aster, Sirr, Hwint_clr, Sl_xmit,
			   Sl_rcv, Icsr, Ic_flush, Ic_perr_stat, Pmctr, Paltemp0, Paltemp1, Paltemp2,
			   Paltemp3, Paltemp4, Paltemp5, Paltemp6, Paltemp7, Paltemp8, Paltemp9, 
			   PaltempA, PaltempB, PaltempC, PaltempD, PaltempE, PaltempF, Paltemp10,
			   Paltemp11, Paltemp12, Paltemp13, Paltemp14, Paltemp15, Paltemp16,
			   Paltemp17, Dtb_asn, Virtual_inst, Dc_mode, Rpcc, 
			   Dtb_cm, Dtb_tag, Dtb_pte, Dtb_pte_temp, Mm_stat, Va,
			   Va_form, Mvptbr, Dtbiap, Dtbia, Dtbis, Alt_mode, Cc,
			   Cc_ctl, Mcsr, Dc_flush, Dc_perr_stat, Dc_test_ctl, 
			   Dc_test_tag, Dc_test_tag_temp, Dc_alt_mode, Maf_mode,
			   Physical_inst, Hwrei, Hwrei_stall, Epicode, Mb, Wmb,
			   Maxres = 100};


static enum uses 	{Nouse = 	0 * Maxres,
			   Mf = 	1 * Maxres, 
			   Mt = 	2 * Maxres, 
			   User =	3 * Maxres,
			   Pal = 	4 * Maxres,
			   Start_of_flow = 5 * Maxres,
			   Maxuse = 	10 * Maxres};

static enum usage_flag_type {CMP_OR_LOGICAL, NORMAL};
static int issue_pipe[di_max];
static int reg_usage[64];
static int reg_usage_flag[64];
static int latency[di_max];
static int last_address;
static int first_instr = TRUE;


extern void kill_old_caveats(List *list); /* V3.25 */
extern int  wr (int, char *, ...); 	/* V3.25 */
extern CYCLE	*runtime[];
extern CLASS_INFO *instr_array[];
extern int 	delay, interrupt;
extern List 	*restrictions[];
extern int pal_warning_count;
extern int pal_error_count;

extern char 	pheader[];



/*
 *	Functions declared in this module
 *
 *		This had to be done so that their invocations would compile 
 *		without compiler messages.
 *
 */

void ev5_init (Chip *);
static void init_stuff (void);
static int schedule_code (INSTR *[], int, int *);
static int issue_insts (INSTR *[], int *, int *);
static int slot_insts (INSTR *[], int, INSTR *[]);
static int load_2after_store (INSTR *, int);
static int scoreboard_clean (INSTR *, int) ;
static int update_scoreboard (INSTR *, int) ;
static int get_id (INSTR *);
static int get_ipr (int);
static int is_hwrei (int);
static int is_mf_match (int, int, int);
static int is_pal_shadow (int);
static int is_ps_write (INSTR *inst);
static int mxpr_pipe (INSTR *);
static int get_latency(INSTR *);
static void check_restrictions (int);
static void special_case (INSTR *inst, int cycle);
static void init_restrictions (void);
static int not_preceded_by_mbs (int);
static void decode_ipr (disp_table *);
static void decode_hw_memory (disp_table *);
static void init_pipe_use (void);
static void init_latencies (void);
static void init_classes(void);
static void init_decode_tables (void);
static void do_nothing(void);



void ev5_init (Chip *c)
/*
 ********************************************************************************
 *	PURPOSE:
 *		To let the rest of the chip know where the important
 *		chip specific functions are.
 *	INPUT PARAMETERS:
 *		c:	This is a pointer to a Chip structure. It has fields
 *			for all the required function pointers
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *		a filled-in chip structure.
 *	FUNCTIONS CALLED DIRECTLY:
 *		none, but the functions enumerated below have to be present in
 *		this module
 *	AUTHOR/DATE:
 *	        Greg Coladonato, Aug 92
 ********************************************************************************
 */
{
      c->chip_init = init_stuff;
      c->restriction_check = check_restrictions;
      c->decode_ipr = decode_ipr;
      c->decode_hw_memory = decode_hw_memory;
      c->schedule_code = schedule_code;
      c->at_pal_entry = do_nothing;
      c->at_pal_return = do_nothing;

      delay = 0;
}





static void init_stuff (void)
/*
 ********************************************************************************
 *	PURPOSE:
 *		Init all the chip specific tables.
 *	INPUT PARAMETERS:
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Greg Coladonato, Aug 92
 ********************************************************************************
 */
{
      init_pipe_use();
      init_latencies();
      init_classes();
      init_restrictions();
      init_decode_tables();
}      

static void do_nothing (void)
/*
 ********************************************************************************
 *	PURPOSE:
 *	Placeholder for chip specific at_pal_entry and at_pal_exit that
 *	is not used by ev5.
 *	INPUT PARAMETERS:
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 ********************************************************************************
 */
{

}      



static int schedule_code (INSTR *raw_code[], int delay_flag, int *length)
/*
 * *******************************************************************************
 *	 PURPOSE:
 *		To schedule code as EV5 would actually run it
 *	 INPUT PARAMETERS:
 *	 	raw_ode: An array of the instructions to schedule
 *		delay_flag: either true or false depending on whether PVC
 *			would like this routine to take cache miss latencies
 *			into account.
 *	 OUTPUT PARAMETERS:
 *	 	the length of the scheduled code sequence.
 *	 IMPLICIT INPUTS:
 *	 IMPLICIT OUTPUTS:
 * 	 FUNCTIONS CALLED DIRECTLY:
 *			slot_insts
 *			issue_insts
 *	 AUTHOR/DATE:
 *	 	Greg Coladonato, August 1992
 * *******************************************************************************
 */
{
      int 	offs, count, idx;
      INSTR 	*slots[4];
      
      if (delay_flag)
	{
	      /* There is no support for this in the EV5 module yet */
	      *length = 0;
	      wr (FLG$SCHED2, "Aborting cache check..\n");
	      return(0);
	}
      
      /* clean off the scoreboard */
      for (idx=0;idx<63;idx++) {
	reg_usage[idx]=0;
	reg_usage_flag[idx] = NORMAL;
      }
      
      for (offs=0,count=0; raw_code[offs];) 
	{
	      /*
	       * Pass the slotting function the code, our position in it, and the array
	       * to write the instructions that get slotted.
	       */
	      
	      slot_insts (raw_code, offs, slots);

	      /*
	       * When we know which instructions get slotted, pass that to the issue 
	       * port, which will loop until they're all done.
	       */

	      issue_insts (slots, &offs, &count);
	}	      
      *length = count;
      wr (FLG$SCHED2, "\n");
return(1);
}



static int issue_insts (INSTR *slots[], int *offs, int *count)
/*
 ********************************************************************************
 *	PURPOSE:
 *		to implement the same logic that is in the EV5 issue stage
 *	INPUT PARAMETERS:
 *		slots: an array of instructions that were passed to the issue stage
 *		offs: the current offset into the exe[] array that were working on
 *		count: the current cycle count
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCITONS CALLED DIRECTLY:
 *		update_scoreboard
 *		scoreboard_clean
 *		print_cycle
 *		load_2after_store
 *	AUTHOR/DATE:
 *	        Greg Coladonato, Aug 92
 *		Paul Rubinfeld, January 1993 - modified module
 *
 ********************************************************************************
 */
{
    int issue_cnt = 0;			/* intruction issue count at each tick */
    int next_slot = 0;  		/* slot array index */
      
    while ( (next_slot < 4) && slots[next_slot]) {
	if (scoreboard_clean(slots[next_slot], *count) && 
	    !load_2after_store (slots[next_slot], *count -2)) {

	      /* If the scoreboard is clean, issue this instruction */

	    switch (issue_cnt) {
	     case 0:
		if (*count < MAXRUNCYCLES)
	            runtime[*count] = Mem_Alloc_CYCLE (slots[next_slot], NULL);
		else {
		    printf("DRY ROT in slot_insts - cycle count (%d) exceeds MAXRUNCYCLES limit.  Increase limit.\n",*count);
		    exit(1); 
		}
		break;
	     case 1:
		runtime[*count]->is2 = slots[next_slot];
		break;
	     case 2:
		runtime[*count]->is3 = slots[next_slot];
		break;
	     case 3:
		runtime[*count]->is4 = slots[next_slot];
		break;
	    }  /* end switch */
	    update_scoreboard (slots[next_slot], *count);
#ifdef pir_test
	    wr (FLG$SCHED2, "Issued %x(hex)\n",slots[next_slot]->address); 
#endif

	    issue_cnt++;
	    next_slot++;
	    (*offs)++;
	} else if (issue_cnt) {

/*
	Can not issue any MORE instructions on this tick

*/
#ifdef pir_test
	    wr (FLG$SCHED2, "Scoreboard stall after %d instruction(s)\n",issue_cnt); 
#endif
	    print_cycle (FLG$SCHED2, *count);
	    (*count)++;
	    issue_cnt = 0;
	} else {

/*
	Can not issue ANY instructions on this tick
*/

	    if (*count < MAXRUNCYCLES)
	        runtime[*count] = Mem_Alloc_CYCLE (NULL, NULL);
	    else 
		printf("DRY ROT in slot_insts - cycle count (%d) exceeds MAXRUNCYCLES limit.  Increase limit.\n",*count);

	    print_cycle (FLG$SCHED2, *count);
	    (*count)++;
	}


    } /* end while */
    print_cycle (FLG$SCHED2, *count);
    (*count)++;
return(1);
}



static int load_2after_store (INSTR *i, int cycle)
/*
 ********************************************************************************
 *	PURPOSE:
 *		Make sure that no loads issue exactly two cycles after a store
 *		issued. EV5 hardware checks for this at the issue stage.
 *		I hope no one else ever has to decode the logic below, because
 *		it's pretty twisted.
 *	INPUT PARAMETERS:
 *		i: the instruction that might be a load
 *		cycle: the cycle in runtime[] that better not have a store in it
 *	OUTPUT PARAMETERS:
 *		true or false, depending on whether we match the template
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Greg Coladonato, Aug 92
 ********************************************************************************
 */
{
      if (i == NULL) {
	printf("DRY ROT in load_2after_store() - i is NULL\n");
	exit(0);
      }

      if (i->index >= MAX_INDEX || i->index < 0) {
	printf("DRY ROT in load_2after_store() - i->index is out of range - index:%d\n",i->index);
	exit(0);
      }

      if (instr_array[i->index] == NULL) {
	printf("DRY ROT in load_2after_store() - instr_array[i->index] is NULL\n");
	exit(0);
      }

      if (instr_array[i->index]->imp == NULL) {
	printf("DRY ROT in load_2after_store() - instr_array[i->index]->imp is NULL\n");
	exit(0);
      }

      if (instr_array[i->index]->imp->di_class != di_ld) return FALSE;

      if (cycle < 0) return FALSE;

      if (!runtime[cycle]) return FALSE;
      if (!runtime[cycle]->is1) return FALSE;
      if ((instr_array[runtime[cycle]->is1->index]->imp->di_class != di_st) &&
	  (runtime[cycle]->is2 ?  
	   (instr_array[runtime[cycle]->is2->index]->imp->di_class != di_st) :
	   (TRUE))) return FALSE;

      return (TRUE);
}




static int slot_insts (INSTR *raw_code[], int position, INSTR *slots[])
/*
 ********************************************************************************
 *	PURPOSE:
 *		To simulate the slotting stage of EV5. 
*
 *	INPUT PARAMETERS:
 *		raw_code: pointer to the array of instructions from follow_code
 *		position: where we are in the above array
 *		slots: the array into which we'll write the instructions which
 *			can be slotted together
 *	OUTPUT PARAMETERS:
 *		None
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *		The values in slots[]
 *	FUNCTIONS CALLED DIRECTLY:
 *		mxpr_pipe
 *	AUTHOR/DATE:
 *	        Greg Coladonato, Aug 92
 *		Paul Rubinfeld, January 1993 - modified module
 *
 ********************************************************************************
 */
{
      int index, class, valid_pipes, i;			/* v3.23 */
      int slot_cnt = 0, load_or_store = 0, allocated_pipes = 0, load_or_mmxpr = 0;
      int e0_or_e1_in_e0 = FALSE;
      int finished = FALSE, slotted = FALSE;
      int l,m;      
      while (!finished && raw_code[position]) 
	{
	      if (position < 0 || position >= 5000) {
		printf("DRY ROT in slot_insts() - code position out of range, position: %d\n", position);
		exit(0);


	      }

	      index = raw_code[position]->index;

	      if (index >= MAX_INDEX || index <= 0) {		/* v3.24 */
		  
		printf("DRY ROT in slot_insts() -instr_array index out of range: index: %d\n",index);
		printf("        raw_code position: %d\n",position);
		printf("        Probably due to illegal 21164 instruction.\n");
		printf("        Illegal instruction is at palcode address : %X\n",raw_code[position]->address);
		exit(0);
	      } 

	      if (instr_array[index]->imp == NULL) {
		printf("DRY ROT in slot_insts() - instr_array[%d]->imp is NULL\n",index);
		printf("        raw_code position: %d\n",position);
		exit(0);
 	      }
	      class = instr_array[index]->imp->di_class;

	      /* MXPRs go down different pipes depending on the IPR */
	      if (class == di_mxpr)
		valid_pipes = mxpr_pipe (raw_code[position]);
	      else 
		valid_pipes = issue_pipe[class];
	      
	      /* the slotting logic knows that a ST and a LD can't issue together */

	      if (class == di_ld || class == di_st) {
		      if ((load_or_store == di_ld && class == di_st) ||
			  (load_or_store == di_st && class == di_ld)) {
		          finished = TRUE;
			  continue;
		      } else
			  load_or_store = class;
		}

	      /* the slotting logic knows that a mbox mxpr and a LD can't issue together */
		if (class == di_ld || ((class == di_mxpr) && (valid_pipes == ps_e0))) {
			if ((load_or_mmxpr == di_ld && class == di_mxpr) ||
			    (load_or_mmxpr == di_mxpr && class == di_ld)) {
		          finished = TRUE;
			  continue;
		      } else
			  load_or_mmxpr = class;
		}

	      /* make sure we're in the same INT16 as we started out*/
	      if (!first_instr) {
		 l = last_address;					/* stop signed/unsigned mismatched NT warning */
	         m = raw_code[position]->address & 0xFFFFFFF0;		/* by splitting these out */
		 if (slot_cnt && (l != m )) {
		     finished = TRUE;
#ifdef pir_test
		     wr (FLG$SCHED2, "%X in next int16\n",raw_code[position]->address);
#endif
		     last_address = raw_code[position]->address & 0xFFFFFFF0;
		     continue;
		  }
	      } else
	          first_instr = FALSE;
	      
	      last_address = raw_code[position]->address & 0xFFFFFFF0;


	      switch (valid_pipes) {
		case ps_e0:
		    if ( !(allocated_pipes & ps_e0))	{
			/* e0 is free */
#ifdef pir_test
		        wr (FLG$SCHED2, "Slotting %X in e0\n",raw_code[position]->address);
#endif
			allocated_pipes |= ps_e0;
			slots[slot_cnt++] = raw_code[position++];
		    } else if (e0_or_e1_in_e0 && !(allocated_pipes & ps_e1)) {
			/* swap current e0 into e1 and issue instruction */
#ifdef pir_test
		        wr (FLG$SCHED2, "Swapping e0 to e1 and Slotting %X in e0\n",raw_code[position]->address);
#endif

			allocated_pipes |= ps_e1;
			slots[slot_cnt++] = raw_code[position++];
		    } else {
			/* e0 has already been allocated and can not be swapped with e1 */
#ifdef pir_test
		        wr (FLG$SCHED2, "e0 not free and e0 can not move to e1\n");
#endif
			finished = TRUE;
		    }
		    break;

		case ps_e1:
		    if ( !(allocated_pipes & ps_e1))	{
			/* e1 is free */
#ifdef pir_test
		        wr (FLG$SCHED2, "Slotting %X in e1\n",raw_code[position]->address);
#endif
			allocated_pipes |= ps_e1;
			slots[slot_cnt++] = raw_code[position++];
		    } else {
			/* e1 has already been allocated */
#ifdef pir_test
		        wr (FLG$SCHED2, "e1 not free\n");
#endif
			finished = TRUE;
		    }
		    break;

		case ps_e0 | ps_e1:
		    if ( !(allocated_pipes & ps_e0))	{
			/* e0 is free */
#ifdef pir_test
		        wr (FLG$SCHED2, "Slotting %X in e0\n",raw_code[position]->address);
#endif
			e0_or_e1_in_e0 = TRUE;
			allocated_pipes |= ps_e0;
			slots[slot_cnt++] = raw_code[position++];
		    } else if ( !(allocated_pipes & ps_e1))	{

			/* e1 is free */
#ifdef pir_test
		        wr (FLG$SCHED2, "Slotting %X in e1\n",raw_code[position]->address);
#endif
			allocated_pipes |= ps_e1;
			slots[slot_cnt++] = raw_code[position++];
		    } else {
			/* both e0 and e1 have already been allocated */
#ifdef pir_test
		        wr (FLG$SCHED2, "e0 and e1 not free\n");
#endif
			finished = TRUE;
		    }
		    break;

		case ps_fa:
		    if ( !(allocated_pipes & ps_fa))	{
			/* fa is free */
#ifdef pir_test
		        wr (FLG$SCHED2, "Slotting %X in fa\n",raw_code[position]->address);
#endif
			allocated_pipes |= ps_fa;
			slots[slot_cnt++] = raw_code[position++];
		    } else {
			/* fa has already been allocated */
#ifdef pir_test
		        wr (FLG$SCHED2, "fa not free\n");
#endif
			finished = TRUE;
		    }
		    break;

		case ps_fm:
		    if ( !(allocated_pipes & ps_fm))	{		
			/* fm is free */
#ifdef pir_test
		        wr (FLG$SCHED2, "Slotting %X in fm\n",raw_code[position]->address);
#endif
		        allocated_pipes |= ps_fm;
		        slots[slot_cnt++] = raw_code[position++];
		    } else {
			/* fm has already been allocated */
#ifdef pir_test
		        wr (FLG$SCHED2, "fm not free\n");
#endif
		        finished = TRUE;
		    }
		    break;

		case ps_fm | ps_fa:
		    if ( !(allocated_pipes & ps_fa))	{
			/* fa is free */
#ifdef pir_test
		        wr (FLG$SCHED2, "Slotting %X in fa\n",raw_code[position]->address);
#endif
			allocated_pipes |= ps_fa;
			slots[slot_cnt++] = raw_code[position++];
		    } else if ( !(allocated_pipes & ps_fm))	{		
			/* fm is free */
#ifdef pir_test
		        wr (FLG$SCHED2, "Slotting %X in fm\n",raw_code[position]->address);
#endif
		        allocated_pipes |= ps_fm;
		        slots[slot_cnt++] = raw_code[position++];
		    } else {
			/* fm has already been allocated */
#ifdef pir_test
		        wr (FLG$SCHED2, "fm and fa not free\n");
#endif
		        finished = TRUE;
		    }

		default:
		    printf("DRY ROT in slot_insts() - Illegal valid_pipes value: %X(hex)\n",valid_pipes);
		    finished = TRUE;
	      }

	}
      
      for (i=slot_cnt; i<4; i++)
	slots[i] = (INSTR *) NULL;
return(1);
}




static int mxpr_pipe (INSTR *i)
/*
 ********************************************************************************
 *	PURPOSE:
 *		This function determines which of the two integer pipes a given
 *		MF or MT will go down. 
 *	INPUT PARAMETERS:
 *		i: The instruction in question
 *	OUTPUT PARAMETERS:
 *		the pipe stage this guy goes down			
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Greg Coladonato, Aug 92
 ********************************************************************************
 */
{
      int stages;

      /* 
       * The HW_MXPR instructions go down different pipes
       * depending on the IPR. IBOX - E1, MBOX - E0
       * Ibox instructions have reg<9:8> = 01
       * Mbox instructions have reg<9:8> = 10
       */
      int ipr_num = i->bits.hwipr_format.reg;
      
      if (((ipr_num >> 8) & 0x3) == 1)
	{
	      /* this is an IBOX ipr. e1 */
	      stages = ps_e1;
	}
      else if (((ipr_num >> 8) & 0x3) == 2)
	{
	      /* this is an MBOX ipr. e0 */
	      stages = ps_e0;
	}
      else 
	{
	   if (new_error (i->address, 0)) 
	   {
	      wr (FLG$ERR, "%s\n", pheader);
	      wr (FLG$ERR, "***\nError executing instruction %s at address %X on cycle ?!!\n", 
		i->decoded, i->address);
	      wr (FLG$ERR, "(PVC #???) Illegal IPR number \n***\n");
	      pal_error_count++;
	   }
	      stages = ps_e0 + ps_e1;
	}

      return (stages);
}      


static int get_latency (INSTR *i)
/*
 ********************************************************************************
 *	PURPOSE:
 *		This function determines the latency of a 
 *		MF or MT instruction.  Mbox IPRs have a latency of 2; Ibox
 *		IPRs, 1.
 *	INPUT PARAMETERS:
 *		i: The instruction in question
 *	OUTPUT PARAMETERS:
 *		the pipe stage this guy goes down			
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Greg Coladonato, Aug 92
 ********************************************************************************
 */
{
      CLASS_INFO *class_def = instr_array[i->index];
      int	class;
      int	ipr_num;

      /*
       * The latency for non-mxpr instructions is fixed
       */
      class = class_def->imp->di_class;
      if (class != di_mxpr)
          return( latency[class]);
      /* 
       * The latency for mxpr instructions
       * depends on the where the ipr is.
       * mxpr to Ibox registers have a latency of 1.  mxpr
       * to Mbox registers have a latency of 2.
       * Ibox instructions have reg<9:8> = 01
       * Mbox instructions have reg<9:8> = 10
       */
      ipr_num = i->bits.hwipr_format.reg;
      
      if (((ipr_num >> 8) & 0x3) == 1)
	      return(1);  	/* Ibox IPR */
      else if (((ipr_num >> 8) & 0x3) == 2)
	      return(2);	/* this is an MBOX ipr */
      else {
	      return(1);
      }
}      




static int scoreboard_clean (INSTR *this_inst, int cycle) 
/*
 *******************************************************************************
 *	 PURPOSE:
 *	 	This function returns a value of TRUE or FALSE depending on whether
 *	 	or not the instruction passed to it can issue on this cycle, given the
 *	 	state of the registers.
 *	 INPUT PARAMETERS:
 *	 	this_inst:  The instruction we'd like to issue
 *	 	cycle:      The current cycle
 *	 OUTPUT PARAMETERS:
 *	 	TRUE or FALSE
 *	 IMPLICIT INPUTS:
 *	 IMPLICIT OUTPUTS:
 *	 AUTHOR/DATE:
 *	 Greg Coladonato, November 1991
 ********************************************************************************
 */
{

      CLASS_INFO *class_def = instr_array[this_inst->index];
      int clean = 0;
      
/********************************new code **************************************/
      int offset = 0;
      int fpls = FALSE;


      switch (class_def->imp->di_class) {
        case di_fadd:	case di_fdiv:	case di_fmul:
	case di_fcpys:
	    offset = 32;
      }

      switch (this_inst->index) {
	    case DO_STD:	case DO_STS:	case DO_STF:	case DO_STT: 
	    case DO_LDD:	case DO_LDS:	case DO_LDF:	case DO_LDT: 
	    offset = 32;
	    fpls = TRUE;
      }

      if (class_def->arch->op_ra) {
          if (reg_usage[this_inst->bits.op_format.ra + offset] <= cycle) 
	      clean++;
	 else if ( (reg_usage_flag[this_inst->bits.op_format.ra + offset] == CMP_OR_LOGICAL) &&
		   ( (class_def->imp->di_class == di_cmov) || (class_def->imp->di_class == di_ibr)) )
	      clean++;
      }
      else clean++;
      
      /*	 
       *  Don't scoreboard literals.
       */	 
      if (class_def->arch->op_rb) {
         if (class_def->arch->chk_lit_mode && this_inst->bits.opl_format.litflag)
	     clean++;
	 else if (!fpls && reg_usage[this_inst->bits.op_format.rb + offset] <= cycle) 
	     clean++;
	 else if (fpls && reg_usage[this_inst->bits.op_format.rb] <= cycle) 
	     clean++;
	 else if ( (reg_usage_flag[this_inst->bits.op_format.rb + offset] == CMP_OR_LOGICAL) &&
		   ( (class_def->imp->di_class == di_cmov) || (class_def->imp->di_class == di_ibr)) )
	      clean++;
      }	
      else clean++;
      
      if (class_def->arch->op_rc) {
          if (reg_usage[this_inst->bits.op_format.rc + offset] <= cycle) 
	      clean++;
      }
      else clean++;

/*******************************************************************************/
      if (clean == 3) return (TRUE);
      else return (FALSE);
}



static int update_scoreboard (INSTR *this_inst, int cycle) 
/*
 ********************************************************************************
 *	 PURPOSE: Update the register file scoreboard whenever a command issues that
 *	 	writes to that register. This is necessary for scheduling purposes.  
 *	 INPUT PARAMETERS:
 *	 	this_inst: The instruction we'd like to execute
 *	 	cycle: the cycle that we're at
 *	 OUTPUT PARAMETERS:
 *	 IMPLICIT INPUTS:
 *	 IMPLICIT OUTPUTS:
 *	 AUTHOR/DATE:
 *	 	Greg Coladonato, November 1991
 ********************************************************************************
 */
{
      CLASS_INFO *class_def = instr_array[this_inst->index];
      int prod = class_def->imp->di_class, reg;

      int offset = 0;


      switch (class_def->imp->di_class) {
        case di_fadd:	case di_fdiv:	case di_fmul:
	case di_fcpys:
	    offset = 32;
      }

      switch (this_inst->index) {
	    case DO_STD:	case DO_STS:	case DO_STF:	case DO_STT: 
	    case DO_LDD:	case DO_LDS:	case DO_LDF:	case DO_LDT: 
	    offset = 32;
      }

      
      if (class_def->arch->dest_reg == Ra) 
	reg = this_inst->bits.op_format.ra + offset;	
      else if (class_def->arch->dest_reg == Rc) 
	reg = this_inst->bits.op_format.rc + offset;
      else 
	return(1);
      
      if (reg != 31 && reg != 63)	{

	   reg_usage[reg] = cycle + get_latency(this_inst);
	   switch (this_inst->index) {
		case DO_CMPEQ: 	case DO_CMPLE:	case DO_CMPLT: 
		case DO_CMPULE: case DO_CMPULT:	
		case DO_AND: 	case DO_BIC:	case DO_EQV:	
		case DO_OR:	case DO_ORNOT:	case DO_XOR:
			reg_usage_flag[reg] = CMP_OR_LOGICAL;
			break;
		default:
			reg_usage_flag[reg] = NORMAL;
	   }
      }
return(1);
}



static int get_id (INSTR *i)
/*
 ********************************************************************************
 *	PURPOSE:
 *		To automate recognizing which resources are used by a given 
 *		instruction. This is a different approach than I used in the 
 *		EV4 module, but better because figuring out what an instruction
 *		is doing with the hardware resources all in one place is better
 *		than having it split up.
 *	INPUT PARAMETERS:
 *		i: the instruction that we want the id for
 *	OUTPUT PARAMETERS:
 *		the "id" of this instruction. This is generally a pairing	
 *		of a resource with a particular use of it.
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Greg Coladonato, Aug 92
 ********************************************************************************
 */
{
      int id = 0, temp;

      if (!i) return (-1);

      switch (i->index) 
	{
	    case DO_HW_REI:
	      {
/*		    id = User + Hwrei;	*/
/*		    break;		*/
		    temp = i->bits.instr & 0X4000;
		    switch (temp)
		      {
			case 0X4000:
			    id = User + Hwrei_stall;
			    break;
			default:
			    id = User + Hwrei;
		      }
		    break;
	      }




	    case DO_EPICODE:
	      {
		    id = User + Epicode;
		    break;
	      }
	    case DO_HW_MTPR:
	      {
		    int ipr_num;
		    ipr_num = i->bits.hwipr_format.reg;
		    
		    id = Mt + get_ipr (ipr_num);
		    break;
	      }
	    case DO_HW_MFPR:
	      {
		    int ipr_num;
		    ipr_num = i->bits.hwipr_format.reg;
		    
		    id = Mf + get_ipr (ipr_num);
		    break;
	      }
	    case DO_LDL:	case DO_LDLL:	case DO_LDQ:
	    case DO_LDQL:	case DO_LDQU:	case DO_LDD:
	    case DO_LDS:	case DO_LDT:	case DO_LDF: 
	    case DO_STL:	case DO_STLC:	case DO_STQ:
	    case DO_STQC:	case DO_STQU:	case DO_STD:
	    case DO_STS:	case DO_STF:	case DO_STT: 
	      {
		    id = User + Virtual_inst;
		    break;
	      }
	    case DO_HW_ST:	 
	    case DO_HW_LD: 
	      {
		    if (i->bits.hwmem_format.physical)
		        id = User + Physical_inst;
		    else
		        id = User + Virtual_inst;
		    break;
	      }
	    case DO_RCC:
	      {
		    id = User + Rpcc;
		    break;
	      }
	    case DO_MB:
	      {
		    temp = i->bits.instr & 0X400;
		    switch (temp)
		      {
			case 0X400:
			    id = User + Wmb;
			    break;
			default:
			    id = User + Mb;
		      }
		    break;
	      }
	}
      return (id);
}



static int is_hwrei (int index)
/*
 ********************************************************************************
 *	PURPOSE:
 *              This function takes an index and returns true if it 
 *		a hw_rei or hw_rei_stall
 *	INPUT PARAMETERS:
 *		index:     this is one of the DO_XXX values. There is one for each operation.
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Jeanne Meyer 17-oct-1993
 ********************************************************************************
 */
{
      if (( index == DO_HW_REI))
        {
              return (TRUE);
        }
      else
        {
              return (FALSE);
        }      


}


static int is_mf_match (int num, int index, int ipr_to_match)
/*
 ********************************************************************************
 *	PURPOSE:
 *              This function takes an index and returns true if it is
 *		a hw_mfpr from the ipr specified by ipr_to_match
 *	INPUT PARAMETERS:
 *		index:     this is one of the DO_XXX values. There is one for each operation.
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Jeanne Meyer 17-oct-1993
 ********************************************************************************
 */
{
      if (( index == DO_HW_MFPR) && (num == ipr_to_match))
        {
            return (TRUE);
	}
      else
        {
              return (FALSE);
        }      


}


static int is_ps_write (INSTR *inst)
/*
 ********************************************************************************
 *	PURPOSE:
 *              This function takes an index and returns true if it is
 *		a hw_mfpr from the exc_addr
 *	INPUT PARAMETERS:
 *		index:     this is one of the DO_XXX values. There is one for each operation.
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Jeanne Meyer 1-nov-1993
 ********************************************************************************
 */
{
      int rnum;
      switch (inst->index)
	{
            /* integer LD type instructions (write to Ra) */
	    case DO_LDL:	case DO_LDLL:
 	    case DO_LDQ:	case DO_LDQL:	case DO_LDQU:	case DO_LDB:
	    case DO_LDAL:	case DO_LDAH:	case DO_LDALH:	case DO_LDAQ:
	    case DO_HW_MFPR:	case DO_HW_LD:
	    case DO_RC:		case DO_RS:	case DO_RCC:
		{
		  rnum = inst->bits.op_format.ra;
		  if (is_pal_shadow(rnum)) 
			{
			return(TRUE);
			}
		  else 	return(FALSE);
		  break;
		}
/* integer operates (write to Rc) */
	    case DO_ADDL:	case DO_ADDQ:	case DO_ADDLV:	case DO_ADDQV:
	    case DO_AND:	case DO_BIC:	case DO_CMOVEQ:	case DO_CMOVGE:
	    case DO_CMOVGT:	case DO_CMOVLBC:		case DO_CMOVLBS:
	    case DO_CMOVLT:	case DO_CMOVNE:	case DO_CMPB:	case DO_CMPBGE:
	    case DO_CMPEQ:	case DO_CMPGE:	case DO_CMPGEQ:	case DO_CMPLE:
	    case DO_CMPGT:	case DO_CMPLT:	case DO_CMPNE:	case DO_EQV:
	    case DO_EXTBH:	case DO_EXTBL:	case DO_EXTLH:	case DO_EXTLL:
	    case DO_EXTQH:	case DO_EXTQL:	case DO_EXTWH:	case DO_EXTWL:
	    case DO_INSBH:	case DO_INSBL:	case DO_INSLH:	case DO_INSLL:
	    case DO_INSQH:	case DO_INSQL:	case DO_INSWH:	case DO_INSWL:
	    case DO_ZAP:	case DO_MSKBH:	case DO_MSKLH:	case DO_CMOVLE:
	    case DO_MSKBL:	case DO_MSKLL:	case DO_ZAPNOT:	case DO_MSKQH:
	    case DO_MSKQL:	case DO_MSKWH:	case DO_MSKWL:	case DO_NUDGE:
	    case DO_OR:		case DO_ORNOT:	case DO_ROT:	case DO_SLL:
	    case DO_SRA:	case DO_SRL:	case DO_SUBL:	case DO_SUBQ:
	    case DO_SUBLV:	case DO_SUBQV:	case DO_XOR:	case DO_S4ADDL:
	    case DO_S4ADDQ:	case DO_S8ADDL:	case DO_S8ADDQ:	case DO_S4SUBL:
	    case DO_S4SUBQ:	case DO_S8SUBL:	case DO_S8SUBQ:	
		{
		  rnum = inst->bits.op_format.rc;
		  if (is_pal_shadow(rnum))
			{
			return(TRUE);
			}
		  else 	return(FALSE);
		  break;
		}


/* integer branch types (write to Ra): */
	case DO_JSR:
	case DO_BSR:
	case DO_BR:
		{
		  rnum = inst->bits.op_format.ra;
		  if (is_pal_shadow(rnum)) 
			{
			return(TRUE);
			}
		  else 	return(FALSE);
		  break;
		}

	default:  return(FALSE); break;
    }
}


static int is_pal_shadow (int num)
/*
 ********************************************************************************
 *	PURPOSE:
 *		In this function we return a 1 if the register number is a palshadow register, 0 otherwise
 *
 *	INPUT PARAMETERS:
 *		num: the value of the 5 bit RA or RC field (depending on instruction)
 *	OUTPUT PARAMETERS:
 *		1=palshadow, 0=not palshadow
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Jeanne Meyer 17-oct-1993
 ********************************************************************************
 */
{
      
      switch (num)
	{
	     case 8:			return(TRUE);		break;
	     case 9:			return(TRUE);		break;
	     case 10:			return(TRUE);		break;
	     case 11:			return(TRUE);		break;
	     case 12:			return(TRUE);		break;
	     case 13:			return(TRUE);		break;
	     case 14:			return(TRUE);		break;
	     case 25:			return(TRUE);		break;
	     default:			return(FALSE);		break; /* not a palshadow register */
	}	  
}


static int get_ipr (int num)
/*
 ********************************************************************************
 *	PURPOSE:
 *		In this function we map all the chip iprs that we care about onto
 *		numbers that PVC will use to refer to them. Only the IPRs that
 *		play a role in a restriction have to be non-zero.
 *	INPUT PARAMETERS:
 *		num: the value of the 12 bit ipr field.		     
 *	OUTPUT PARAMETERS:
 *		a staticly defined constat used to refer to that ipr.
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Greg Coladonato, Aug 92
 ********************************************************************************
 */
{
      int ipr;
      
      switch (num)
	{
	    case IPR_ISR:		ipr =  Isr; 		break; /* (0x100)*/
	    case IPR_ITB_TAG:		ipr =  Itb_tag; 	break; /* (0x101)*/
	    case IPR_ITB_PTE:		ipr =  Itb_pte; 	break; /* (0x102)*/
	    case IPR_ITB_ASN:		ipr =  Itb_asn;		break; /* (0x103)*/
	    case IPR_ITB_PTE_TEMP:	ipr =  Itb_pte_temp;	break; /* (0x104)*/
	    case IPR_ITBIA:		ipr =  Itbia;		break; /* (0x105)*/
	    case IPR_ITBIAP:		ipr =  Itbiap;		break; /* (0x106)*/
	    case IPR_ITBIS:		ipr =  Itbis;		break; /* (0x107)*/
	    case IPR_SIRR:		ipr =  Sirr; 		break; /* (0x108)*/
	    case IPR_ASTRR:		ipr =  Astrr; 		break; /* (0x109)*/
	    case IPR_ASTER:		ipr =  Aster; 		break; /* (0x10A)*/
	    case IPR_EXC_ADDR:		ipr =  Exc_addr; 	break; /* (0x10B)*/
	    case IPR_EXC_SUM:		ipr =  Exc_sum; 	break; /* (0x10C)*/
	    case IPR_EXC_MASK:		ipr =  Exc_mask; 	break; /* (0x10D)*/
	    case IPR_PAL_BASE:		ipr =  Pal_base; 	break; /* (0x10E)*/
	    case IPR_PS:		ipr =  Ps;		break; /* (0x10F)*/
	    case IPR_IPL:		ipr =  Ipl; 		break; /* (0x110)*/
	    case IPR_INTID:		ipr =  Intid; 		break; /* (0x111)*/
	    case IPR_IFAULT_VA_FORM:	ipr =  Ifault_va_form;	break; /* (0x112)*/
	    case IPR_IVPTBR:		ipr =  Ivptbr; 		break; /* (0x113)*/
	    case IPR_HWINT_CLR:		ipr =  Hwint_clr; 	break; /* (0x115)*/
	    case IPR_SL_XMIT:		ipr =  Sl_xmit; 	break; /* (0x116)*/
	    case IPR_SL_RCV:		ipr =  Sl_rcv; 		break; /* (0x117)*/
	    case IPR_ICSR:		ipr =  Icsr; 		break; /* (0x118)*/
	    case IPR_IC_FLUSH:		ipr =  Ic_flush; 	break; /* (0x119)*/
	    case IPR_IC_PERR_STAT:	ipr =  Ic_perr_stat; 	break; /* (0x11A)*/
	    case IPR_PMCTR:		ipr =  Pmctr;		break; /* (0x11C)*/
	    case IPR_PALTEMP0:		ipr =  Paltemp0; 	break; /* (0x140)*/
	    case IPR_PALTEMP1:		ipr =  Paltemp1; 	break; /* (0x141)*/
	    case IPR_PALTEMP2:		ipr =  Paltemp2; 	break; /* (0x142)*/
	    case IPR_PALTEMP3:		ipr =  Paltemp3; 	break; /* (0x143)*/
	    case IPR_PALTEMP4:		ipr =  Paltemp4; 	break; /* (0x144)*/
	    case IPR_PALTEMP5:		ipr =  Paltemp5; 	break; /* (0x145)*/
	    case IPR_PALTEMP6:		ipr =  Paltemp6; 	break; /* (0x146)*/
	    case IPR_PALTEMP7:		ipr =  Paltemp7; 	break; /* (0x147)*/
	    case IPR_PALTEMP8:		ipr =  Paltemp8; 	break; /* (0x148)*/
	    case IPR_PALTEMP9:		ipr =  Paltemp9; 	break; /* (0x149)*/
	    case IPR_PALTEMPA:		ipr =  PaltempA; 	break; /* (0x14A)*/
	    case IPR_PALTEMPB:		ipr =  PaltempB; 	break; /* (0x14B)*/
	    case IPR_PALTEMPC:		ipr =  PaltempC; 	break; /* (0x14C)*/
	    case IPR_PALTEMPD:		ipr =  PaltempD; 	break; /* (0x14D)*/
	    case IPR_PALTEMPE:		ipr =  PaltempE; 	break; /* (0x14E)*/
	    case IPR_PALTEMPF:		ipr =  PaltempF; 	break; /* (0x14F)*/
	    case IPR_PALTEMP10:		ipr =  Paltemp10; 	break; /* (0x150)*/
	    case IPR_PALTEMP11:		ipr =  Paltemp11; 	break; /* (0x151)*/
	    case IPR_PALTEMP12:		ipr =  Paltemp12; 	break; /* (0x152)*/
	    case IPR_PALTEMP13:		ipr =  Paltemp13; 	break; /* (0x153)*/
	    case IPR_PALTEMP14:		ipr =  Paltemp14; 	break; /* (0x154)*/
	    case IPR_PALTEMP15:		ipr =  Paltemp15; 	break; /* (0x155)*/
	    case IPR_PALTEMP16:		ipr =  Paltemp16; 	break; /* (0x156)*/
	    case IPR_PALTEMP17:		ipr =  Paltemp17; 	break; /* (0x157)*/

	    case IPR_DTB_ASN:		ipr =  Dtb_asn; 	break; /* (0x200)*/
	    case IPR_DTB_CM:		ipr =  Dtb_cm; 		break; /* (0x201)*/
	    case IPR_DTB_TAG:		ipr =  Dtb_tag; 	break; /* (0x202)*/
	    case IPR_DTB_PTE:		ipr =  Dtb_pte; 	break; /* (0x203)*/
	    case IPR_DTB_PTE_TEMP:	ipr =  Dtb_pte_temp; 	break; /* (0x204)*/
	    case IPR_MM_STAT:		ipr =  Mm_stat; 	break; /* (0x205)*/
	    case IPR_VA:		ipr =  Va; 		break; /* (0x206)*/
	    case IPR_VA_FORM:		ipr =  Va_form; 	break; /* (0x207)*/
	    case IPR_MVPTBR:		ipr =  Mvptbr; 		break; /* (0x208)*/
	    case IPR_DTBIAP:		ipr =  Dtbiap; 		break; /* (0x209)*/
	    case IPR_DTBIA:		ipr =  Dtbia; 		break; /* (0x20A)*/
	    case IPR_DTBIS:		ipr =  Dtbis; 		break; /* (0x20B)*/
	    case IPR_ALT_MODE:		ipr =  Alt_mode; 	break; /* (0x20C)*/
	    case IPR_CC:		ipr =  Cc; 		break; /* (0x20D)*/
	    case IPR_CC_CTL:		ipr =  Cc_ctl; 		break; /* (0x20E)*/
	    case IPR_MCSR:		ipr =  Mcsr; 		break; /* (0x20F)*/
	    case IPR_DC_FLUSH:		ipr =  Dc_flush; 	break; /* (0x210)*/
	    case IPR_DC_PERR_STAT:	ipr =  Dc_perr_stat; 	break; /* (0x212)*/
	    case IPR_DC_TEST_CTL:	ipr =  Dc_test_ctl; 	break; /* (0x213)*/
	    case IPR_DC_TEST_TAG:	ipr =  Dc_test_tag; 	break; /* (0x214)*/
	    case IPR_DC_TEST_TAG_TEMP:	ipr =  Dc_test_tag_temp;break; /* (0x215)*/
	    case IPR_DC_MODE:		ipr =  Dc_mode; 	break; /* (0x216)*/
	    case IPR_MAF_MODE:		ipr =  Maf_mode; 	break; /* (0x217)*/
	    default:			ipr = 0; break;	     
	}
      return (ipr);
}




static void check_restrictions (int length)
/*
 ********************************************************************************
 *	PURPOSE:
 *	        To take an array of cycles, with 0, 1, 2, 3 or 4 Alpha instructions in each,
 *	        and determine whether it violates any of the PAL restrictions    
 *	INPUT PARAMETERS:
 *	        The length of the code. This tells us how far to look in runtime[].
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:
 *		check_for_problems
 *		inspect_instr
 *		new_error
 *		remove_old_caveats
 *		special_case
 *	AUTHOR/DATE:
 *	        Greg Coladonato, August 1991
 ********************************************************************************
 */
{
    int 	i, idx1=0, idx2=0, idx3=0, idx4=0;
    int 	ipr1=0,ipr2=0,ipr3=0,ipr4=0;
    List 	*caveat_list;
    ERR_MSG	*message;
    int 	id1, id2, id3, id4;
    int		ipr_to_match;

    create_L(caveat_list);

    /* check for restrictions at the beginning of a flow */
    if (runtime[0]->is1) 
	{
	idx1 = runtime[0]->is1->index;
	ipr1 = runtime[0]->is1->bits.hwipr_format.reg;
	
	if (runtime[0]->is2) 
	    {
	    idx2 = runtime[0]->is2->index;
	    ipr2 = runtime[0]->is2->bits.hwipr_format.reg;
	    if (runtime[0]->is3)
		{
	    	idx3 = runtime[0]->is3->index;
		ipr3 = runtime[0]->is3->bits.hwipr_format.reg;
	    	if (runtime[0]->is4)
		    {
	    	    idx4 = runtime[0]->is4->index;
		    ipr4 = runtime[0]->is4->bits.hwipr_format.reg;
                    }
		}
	    }
	}


    /*	Make sure no hw_rei or mf exc_addr in first cycle of a flow */
    if (runtime[0]->is1->address >= 0x2000)		/* just check call_pals */
	{
     	if ( is_hwrei (idx1) || is_hwrei (idx2) || is_hwrei (idx3) || is_hwrei (idx4) )
	    {
	    if (new_error (runtime[0]->is1->address, 0)) 
	    {
	    wr (FLG$ERR, "%s\n", pheader);
	    wr (FLG$ERR, "***\nError executing instruction HW_REI at address %X on cycle %d!!\n", 
	      runtime[0]->is1->address, 0);
	    wr (FLG$ERR, "(PVC #???) A HW_REI or HW_REI_STALL can not execute in the first cycle of a CALLPAL flow.\n***\n");
	    pal_error_count++;
	    }
	    }
	ipr_to_match = IPR_EXC_ADDR;
    	if ( (   is_mf_match (ipr1,idx1,ipr_to_match)) || (is_mf_match (ipr2,idx2,ipr_to_match)) 
	      || (is_mf_match (ipr3,idx3,ipr_to_match)) || (is_mf_match (ipr4,idx4,ipr_to_match)) )
	    {
	    if (new_error (runtime[0]->is1->address, 0)) 
	    {
	    wr (FLG$ERR, "%s\n", pheader);
	    wr (FLG$ERR, "***\nError executing instruction HW_MFPR EXC_ADDR at address %X on cycle %d!!\n", 
	      runtime[0]->is1->address, 0);
	    wr (FLG$ERR, "(PVC #???) A HW_MFPR EXC_ADDR can not execute in the first 2 cycles of a CALLPAL flow.\n***\n");
	    pal_error_count++;
	    }
	    }
	}

    /* check for no mf exc_sum or exc_mask in first cycle of arith or mchk flow */
    if ( (runtime[0]->is1->address == 0x0500) || (runtime[0]->is1->address == 0x0400))
	{

	ipr_to_match = IPR_EXC_MASK;
    	if ( (   is_mf_match (ipr1,idx1,ipr_to_match)) || (is_mf_match (ipr2,idx2,ipr_to_match)) 
	      || (is_mf_match (ipr3,idx3,ipr_to_match)) || (is_mf_match (ipr4,idx4,ipr_to_match)) )
	    {
	    if (new_error (runtime[0]->is1->address, 0)) 
	    {
	    wr (FLG$ERR, "%s\n", pheader);
	    wr (FLG$ERR, "***\nError executing instruction HW_MFPR EXC_MASK at address %X on cycle %d!!\n", 
	      runtime[0]->is1->address, 0);
	    wr (FLG$ERR, "(PVC #???) A HW_MFPR EXC_MASK can not execute in the first 2 cycles of a ARITH or MCHK flow.\n***\n");
	    pal_error_count++;
	    }
	    }
	ipr_to_match = IPR_EXC_SUM;
    	if ( (   is_mf_match (ipr1,idx1,ipr_to_match)) || (is_mf_match (ipr2,idx2,ipr_to_match)) 
	      || (is_mf_match (ipr3,idx3,ipr_to_match)) || (is_mf_match (ipr4,idx4,ipr_to_match)) )
	    {
	    if (new_error (runtime[0]->is1->address, 0)) 
	    {
	    wr (FLG$ERR, "%s\n", pheader);
	    wr (FLG$ERR, "***\nError executing instruction HW_MFPR EXC_SUM at address %X on cycle %d!!\n", 
	      runtime[0]->is1->address, 0);
	    wr (FLG$ERR, "(PVC #???) A HW_MFPR EXC_SUM can not execute in the first 2 cycles of a ARITH or MCHK flow.\n***\n");
	    pal_error_count++;
	    }
	    }
	}

    /*	Now check 2nd cycle */
    /* reset everything to 0 */
    idx1=idx2=idx3=idx4=ipr1=ipr2=ipr3=ipr4=0;
    if (runtime[1]->is1) 
	{
	idx1 = runtime[1]->is1->index;
	ipr1 = runtime[1]->is1->bits.hwipr_format.reg;
	
	if (runtime[1]->is2) 
	    {
	    idx2 = runtime[1]->is2->index;
	    ipr2 = runtime[1]->is2->bits.hwipr_format.reg;
	    if (runtime[1]->is3)
		{
	    	idx3 = runtime[1]->is3->index;
		ipr3 = runtime[1]->is3->bits.hwipr_format.reg;
	    	if (runtime[1]->is4)
		    {
	    	    idx4 = runtime[1]->is4->index;
		    ipr4 = runtime[1]->is4->bits.hwipr_format.reg;
                    }
		}
	    }
	}


    if (runtime[0]->is1->address >= 0x2000)		/* just check call_pals */
	{
    /*	Make sure no mf exc_addr in second cycle of a flow */
	ipr_to_match = IPR_EXC_ADDR;
    	if ( (    is_mf_match (ipr1,idx1,ipr_to_match)) || (is_mf_match (ipr2,idx2,ipr_to_match)) 
  	      || (is_mf_match (ipr3,idx3,ipr_to_match)) || (is_mf_match (ipr4,idx4,ipr_to_match)) )
	    {
	    if (new_error (runtime[1]->is1->address, 0)) 
	    {
	    wr (FLG$ERR, "%s\n", pheader);
	    wr (FLG$ERR, "***\nError executing instruction HW_MFPR EXC_ADDR at address %X on cycle %d!!\n", 
	      runtime[0]->is1->address, 0);
	    wr (FLG$ERR, "(PVC #???) A HW_MFPR EXC_ADDR can not execute in the first 2 cycles of a CALLPAL flow.\n***\n");
	    pal_error_count++;
	    }
	    }

	}
    if ( (runtime[0]->is1->address == 0x0500) || (runtime[0]->is1->address == 0x0400))
	{
    /*	Make sure no mf exc_mask, exc_sum in second cycle of mchk,arith flow */
	ipr_to_match = IPR_EXC_MASK;
    	if ( (    is_mf_match (ipr1,idx1,ipr_to_match)) || (is_mf_match (ipr2,idx2,ipr_to_match)) 
  	      || (is_mf_match (ipr3,idx3,ipr_to_match)) || (is_mf_match (ipr4,idx4,ipr_to_match)) )
	    {
	    if (new_error (runtime[1]->is1->address, 0)) 
	    {
	    wr (FLG$ERR, "%s\n", pheader);
	    wr (FLG$ERR, "***\nError executing instruction HW_MFPR EXC_MASK at address %X on cycle %d!!\n", 
	      runtime[0]->is1->address, 0);
	    wr (FLG$ERR, "(PVC #???) A HW_MFPR EXC_MASK can not execute in the first 2 cycles of a ARITH or MCHK flow.\n***\n");
	    pal_error_count++;
	    }
	    }
	ipr_to_match = IPR_EXC_SUM;
    	if ( (    is_mf_match (ipr1,idx1,ipr_to_match)) || (is_mf_match (ipr2,idx2,ipr_to_match)) 
  	      || (is_mf_match (ipr3,idx3,ipr_to_match)) || (is_mf_match (ipr4,idx4,ipr_to_match)) )
	    {
	    if (new_warning (runtime[1]->is1->address, 0)) 
	    {
	    wr (FLG$ERR, "%s\n", pheader);
	    wr (FLG$ERR, "***\nError executing instruction HW_MFPR EXC_SUM at address %X on cycle %d!!\n", 
	      runtime[0]->is1->address, 0);
	    wr (FLG$ERR, "(PVC #???) A HW_MFPR EXC_SUM can not execute in the first 2 cycles of a ARITH or MCHK flow.\n***\n");
	    pal_error_count++;
	    }
	    }

	}



    /* check for restrictions that fit the "mold" */      
    for (i=0; i<length && !interrupt; i++)  {
	id1 = get_id (runtime[i]->is1);
	id2 = get_id (runtime[i]->is2);
	id3 = get_id (runtime[i]->is3);
	id4 = get_id (runtime[i]->is4);

	if (id1 != -1) {

/*
 * At least one instruction issued this cycle.
 * See if it can execute, then post its concerns
 * into the caveat list
 */

	    if  ((message = search_caveat (id1, caveat_list, runtime[i]->is1)) && 
	          new_error (runtime[i]->is1->address, message->errnum)) {
		wr (FLG$ERR, "%s\n", pheader);
		wr (FLG$ERR, "***\nError executing instruction %s at address %X on cycle %d!!\n", 
		    runtime[i]->is1->decoded, runtime[i]->is1->address, i);
		wr (FLG$ERR, "(PVC #%d) %s\n***\n", message->errnum, message->explanation);
	    pal_error_count++;				/* V3.23 */
/*		free (message->explanation); */
		free (message);
	    }
	    add_caveat (id1, caveat_list);

/* 
 * There can be a second instruction in this cycle 
 * only if the first instruction issued
 */

	    if (id2 != -1) {
		if ((message = search_caveat (id2, caveat_list,runtime[i]->is2)) &&
		    new_error (runtime[i]->is2->address, message->errnum)) {
		    wr (FLG$ERR, "%s\n", pheader);
		    wr (FLG$ERR, "***\nError executing instruction %s at address %X on cycle %d!!\n", 
			runtime[i]->is2->decoded, runtime[i]->is2->address, i);
		    wr (FLG$ERR, "(PVC #%d) %s\n***\n", message->errnum, message->explanation);
		    pal_error_count++;				/* V3.23 */
/*		    free (message->explanation); */
		    free (message);
		}
	        add_caveat (id2, caveat_list);

/* 
 * There can be a third instruction in this cycle 
 * only if the second instruction issued
 */

		if (id3 != -1) {
		    if ((message = search_caveat (id3, caveat_list, runtime[i]->is3)) &&
			      new_error (runtime[i]->is3->address, message->errnum)) {
			wr (FLG$ERR, "%s\n", pheader);
			wr (FLG$ERR, "***\nError executing instruction %s at address %X on cycle %d!!\n", 
			      runtime[i]->is3->decoded, runtime[i]->is3->address, i);
			wr (FLG$ERR, "(PVC #%d) %s\n***\n", message->errnum, message->explanation);
	    		pal_error_count++;				/* V3.23 */
/*			free (message->explanation); */
			free (message);
		    }
		    add_caveat (id3, caveat_list);

/* 
 * There can be a fourth instruction in this cycle 
 * only if the third instruction issued
*/

		    if (id4 != -1) {
			if ((message = search_caveat (id4, caveat_list, runtime[i]->is4)) &&
			     new_error (runtime[i]->is4->address, message->errnum)) {
			    wr (FLG$ERR, "%s\n", pheader);
			    wr (FLG$ERR, "***\nError executing instruction %s at address %X on cycle %d!!\n", 
				    runtime[i]->is4->decoded, runtime[i]->is4->address, i);
			    wr (FLG$ERR, "(PVC #%d) %s\n***\n", message->errnum, message->explanation);
	    		    pal_error_count++;				/* V3.23 */
/*			    free (message->explanation); */
			    free (message);
			}
			add_caveat (id4, caveat_list);
		    }
		}		      
	    }		      
	}	

/*******************************************************************************************/
/*   JEM - add special case checks for restrictions that don't conform to the standard     */
/*******************************************************************************************/
	    /*	 
	    *  Next, add the caveats that the special checks engender to the list
	    */	 
	    if (runtime[i]->is1 != NULL) 
		{
		special_case (runtime[i]->is1, i);
		if (runtime[i]->is2 != NULL) 
		    {
		    special_case (runtime[i]->is2, i);
		    if (runtime[i]->is3 != NULL) 
		    	{
			special_case (runtime[i]->is3, i);
		  	if (runtime[i]->is4 != NULL) 
		    	    {
			    special_case (runtime[i]->is4, i);
		     	    }
		    	}
		    }
		}

/*******************************************************************************************/
	remove_old_caveats(caveat_list);
    }	
    kill_old_caveats(caveat_list);
    destroy_L(caveat_list, Restriction);
    wr (FLG$SCHED2, "\n\n");
}



static void init_restrictions (void)
/*
 ********************************************************************************
 *	PURPOSE: 
 *	        To initialize the array of restrictions. The format for this
 *	        code should be simple enough to modify as the restrictions change.
 *
 *	        The add_restriction function takes four arguments -- 
 *	
 *	        the sum of the use and resource for the first half
 *	        the sum of the use and resource for the second half
 *	        a string of the numbers of cycles which cannot separate the two
 *	        the error message
 *		
 *	INPUT PARAMETERS:
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	        The restrictions array.
 *	IMPLICIT OUTPUTS:
 *	        A completely filled restrictions array.
 *	FUNCTIONS CALLED DIRECTLY:
 *		add_restriction
 *	AUTHOR/DATE:
 *	        Greg Coladonato, December 1991
 ********************************************************************************
 */
{
      clean_restrictions();

      add_restriction (User + Virtual_inst,	Mt + Dtb_asn,		"0",	"MBOX INST --> MT or MF DTB_ASN IN 0");
      add_restriction (User + Virtual_inst,	Mt + Dtb_cm,		"0",	"MBOX INST --> MT or MF DTB_CM IN 0");
      add_restriction (User + Virtual_inst,	Mt + Dtb_tag,		"0",	"MBOX INST --> MT or MF DTB_TAG IN 0");
      add_restriction (User + Virtual_inst,	Mt + Dtb_pte,		"0",	"MBOX INST --> MT or MF DTB_PTE IN 0");
      add_restriction (User + Virtual_inst,	Mt + Dtb_pte_temp,	"0",	"MBOX INST --> MT or MF DTB_PTE_TEMP IN 0");
      add_restriction (User + Virtual_inst,	Mt + Mm_stat,		"0",	"MBOX INST --> MT or MF MM_STAT IN 0");
      add_restriction (User + Virtual_inst,	Mt + Va,		"0",	"MBOX INST --> MT or MF DTB_VA IN 0");
      add_restriction (User + Virtual_inst,	Mt + Va_form,		"0",	"MBOX INST --> MT or MF VA_FROM IN 0");
      add_restriction (User + Virtual_inst,	Mt + Mvptbr,		"0",	"MBOX INST --> MT or MF MVPTBR IN 0");
      add_restriction (User + Virtual_inst,	Mt + Dtbiap,		"0",	"MBOX INST --> MT or MF DTBIAP IN 0");
      add_restriction (User + Virtual_inst,	Mt + Dtbia,		"0",	"MBOX INST --> MT or MF DTBIA IN 0");
      add_restriction (User + Virtual_inst,	Mt + Dtbis,		"01",	"VIRTUAL MBOX INST --> MT DTBIS IN 01");
      add_restriction (User + Virtual_inst,	Mt + Alt_mode,		"0",	"MBOX INST --> MT ALT_MODE IN 0");
      add_restriction (User + Virtual_inst,	Mt + Cc,		"0",	"MBOX INST --> MT CC IN 0");
      add_restriction (User + Virtual_inst,	Mt + Cc_ctl,		"0",	"MBOX INST --> MT CC_CTL IN 0");
      add_restriction (User + Virtual_inst,	Mt + Mcsr,		"0",	"MBOX INST --> MT MCSR IN 0");
      add_restriction (User + Virtual_inst,	Mt + Dc_flush,		"0",	"MBOX INST --> MT DC_FLUSH IN 0");
      add_restriction (User + Virtual_inst,	Mt + Dc_perr_stat,	"0",	"MBOX INST --> MT DC_PERR_STAT IN 0");
      add_restriction (User + Virtual_inst,	Mt + Dc_test_ctl,	"0",	"MBOX INST --> MT DC_TEST_CTL IN 0");
      add_restriction (User + Virtual_inst,	Mt + Dc_test_tag,	"0",	"MBOX INST --> MT DC_TEST_TAG IN 0");
      add_restriction (User + Virtual_inst,	Mt + Dc_test_tag_temp,	"0",	"MBOX INST --> MT DC_TEST_TAG_TEMP IN 0");
      add_restriction (User + Virtual_inst,	Mt + Dc_mode,		"0",	"MBOX INST --> MT DC_MODE IN 0");
      add_restriction (User + Virtual_inst,	Mt + Maf_mode,		"0",	"MBOX INST --> MT MAF_MODE IN 0");
      add_restriction (User + Physical_inst,	Mt + Dtb_asn,		"0",	"MBOX INST --> MT DTB_ASN IN 0");
      add_restriction (User + Physical_inst,	Mt + Dtb_cm,		"0",	"MBOX INST --> MT DTB_CM IN 0");
      add_restriction (User + Physical_inst,	Mt + Dtb_tag,		"0",	"MBOX INST --> MT DTB_TAG IN 0");
      add_restriction (User + Physical_inst,	Mt + Dtb_pte,		"0",	"MBOX INST --> MT DTB_PTE IN 0");
      add_restriction (User + Physical_inst,	Mt + Dtb_pte_temp,	"0",	"MBOX INST --> MT DTB_PTE_TEMP IN 0");
      add_restriction (User + Physical_inst,	Mt + Mm_stat,		"0",	"MBOX INST --> MT MM_STAT IN 0");
      add_restriction (User + Physical_inst,	Mt + Va,		"0",	"MBOX INST --> MT DTB_VA IN 0");
      add_restriction (User + Physical_inst,	Mt + Va_form,		"0",	"MBOX INST --> MT VA_FORM IN 0");
      add_restriction (User + Physical_inst,	Mt + Mvptbr,		"0",	"MBOX INST --> MT MVPTBR IN 0");
      add_restriction (User + Physical_inst,	Mt + Dtbiap,		"0",	"MBOX INST --> MT DTBIAP IN 0");
      add_restriction (User + Physical_inst,	Mt + Dtbia,		"0",	"MBOX INST --> MT DTBIA IN 0");
      add_restriction (User + Physical_inst,	Mt + Dtbis,		"01",	"MBOX INST --> MT DTBIS IN 01");
      add_restriction (User + Wmb,		Mt + Dtbis,		"01",	"WMB --> MT DTBIS IN 01");
      add_restriction (User + Physical_inst,	Mt + Alt_mode,		"0",	"MBOX INST --> MT ALT_MODE IN 0");
      add_restriction (User + Physical_inst,	Mt + Cc,		"0",	"MBOX INST --> MT CC IN 0");
      add_restriction (User + Physical_inst,	Mt + Cc_ctl,		"0",	"MBOX INST --> MT CC_CTL IN 0");
      add_restriction (User + Physical_inst,	Mt + Mcsr,		"0",	"MBOX INST --> MT MCSR IN 0");
      add_restriction (User + Physical_inst,	Mt + Dc_flush,		"0",	"MBOX INST --> MT DC_FLUSH IN 0");
      add_restriction (User + Physical_inst,	Mt + Dc_perr_stat,	"0",	"MBOX INST --> MT DC_PERR_STAT IN 0");
      add_restriction (User + Physical_inst,	Mt + Dc_test_ctl,	"0",	"MBOX INST --> MT DC_TEST_CTL IN 0");
      add_restriction (User + Physical_inst,	Mt + Dc_test_tag,	"0",	"MBOX INST --> MT DC_TEST_TAG IN 0");
      add_restriction (User + Physical_inst,	Mt + Dc_test_tag_temp,	"0",	"MBOX INST --> MT DC_TEST_TAG_TEMP IN 0");
      add_restriction (User + Physical_inst,	Mt + Dc_mode,		"0",	"MBOX INST --> MT DC_MODE IN 0");
      add_restriction (User + Physical_inst,	Mt + Maf_mode,		"0",	"MBOX INST --> MT MAF_MODE IN 0");
      add_restriction (User + Virtual_inst,	Mf + Dtb_asn,		"0",	"MBOX INST --> MF DTB_ASN IN 0");
      add_restriction (User + Virtual_inst,	Mf + Dtb_cm,		"0",	"MBOX INST --> MF DTB_CM IN 0");
      add_restriction (User + Virtual_inst,	Mf + Dtb_tag,		"0",	"MBOX INST --> MF DTB_TAG IN 0");
      add_restriction (User + Virtual_inst,	Mf + Dtb_pte,		"0",	"MBOX INST --> MF DTB_PTE IN 0");
      add_restriction (User + Virtual_inst,	Mf + Dtb_pte_temp,	"0",	"MBOX INST --> MF DTB_PTE_TEMP IN 0");
      add_restriction (User + Virtual_inst,	Mf + Mm_stat,		"0",	"MBOX INST --> MF MM_STAT IN 0");
      add_restriction (User + Virtual_inst,	Mf + Va,		"0",	"MBOX INST --> MF DTB_VA IN 0");
      add_restriction (User + Virtual_inst,	Mf + Va_form,		"0",	"MBOX INST --> MF VA_FORM IN 0");
      add_restriction (User + Virtual_inst,	Mf + Mvptbr,		"0",	"MBOX INST --> MF MVPTBR IN 0");
      add_restriction (User + Virtual_inst,	Mf + Dtbiap,		"0",	"MBOX INST --> MF DTBIAP IN 0");
      add_restriction (User + Virtual_inst,	Mf + Dtbia,		"0",	"MBOX INST --> MF DTBIA IN 0");
      add_restriction (User + Virtual_inst,	Mf + Dtbis,		"0",	"MBOX INST --> MF DTBIS IN 0");
      add_restriction (User + Virtual_inst,	Mf + Alt_mode,		"0",	"MBOX INST --> MF ALT_MODE IN 0");
      add_restriction (User + Virtual_inst,	Mf + Cc,		"0",	"MBOX INST --> MF CC IN 0");
      add_restriction (User + Virtual_inst,	Mf + Cc_ctl,		"0",	"MBOX INST --> MF CC_CTL IN 0");
      add_restriction (User + Virtual_inst,	Mf + Mcsr,		"0",	"MBOX INST --> MF MCSR IN 0");
      add_restriction (User + Virtual_inst,	Mf + Dc_flush,		"0",	"MBOX INST --> MF DC_FLUSH IN 0");
      add_restriction (User + Virtual_inst,	Mf + Dc_perr_stat,	"012",	"MBOX INST --> MF DC_PERR_STAT IN 012");
      add_restriction (User + Virtual_inst,	Mf + Dc_test_ctl,	"0",	"MBOX INST --> MF DC_TEST_CTL IN 0");
      add_restriction (User + Virtual_inst,	Mf + Dc_test_tag,	"0",	"MBOX INST --> MF DC_TEST_TAG IN 0");
      add_restriction (User + Virtual_inst,	Mf + Dc_test_tag_temp,	"0",	"MBOX INST --> MF DC_TEST_TAG_TEMP IN 0");
      add_restriction (User + Virtual_inst,	Mf + Dc_mode,		"0",	"MBOX INST --> MF DC_MODE IN 0");
      add_restriction (User + Virtual_inst,	Mf + Maf_mode,		"012",
	"WARNING - MBOX INST --> MF MAF_MODE IN 012. WB_PENDING/DREAD_PENDING may not be updated");
      add_restriction (User + Physical_inst,	Mf + Dtb_asn,		"0",	"MBOX INST --> MF DTB_ASN IN 0");
      add_restriction (User + Physical_inst,	Mf + Dtb_cm,		"0",	"MBOX INST --> MF DTB_CM IN 0");
      add_restriction (User + Physical_inst,	Mf + Dtb_tag,		"0",	"MBOX INST --> MF DTB_TAG IN 0");
      add_restriction (User + Physical_inst,	Mf + Dtb_pte,		"0",	"MBOX INST --> MF DTB_PTE IN 0");
      add_restriction (User + Physical_inst,	Mf + Dtb_pte_temp,	"0",	"MBOX INST --> MF DTB_PTE_TEMP IN 0");
      add_restriction (User + Physical_inst,	Mf + Mm_stat,		"0",	"MBOX INST --> MF MM_STAT IN 0");
      add_restriction (User + Physical_inst,	Mf + Va,		"0",	"MBOX INST --> MF DTB_VA IN 0");
      add_restriction (User + Physical_inst,	Mf + Va_form,		"0",	"MBOX INST --> MF VA_FORM IN 0");
      add_restriction (User + Physical_inst,	Mf + Mvptbr,		"0",	"MBOX INST --> MF MVPTBR IN 0");
      add_restriction (User + Physical_inst,	Mf + Dtbiap,		"0",	"MBOX INST --> MF DTBIAP IN 0");
      add_restriction (User + Physical_inst,	Mf + Dtbia,		"0",	"MBOX INST --> MF DTBIA IN 0");
      add_restriction (User + Physical_inst,	Mf + Dtbis,		"0",	"MBOX INST --> MF DTBIS IN 0");
      add_restriction (User + Physical_inst,	Mf + Alt_mode,		"0",	"MBOX INST --> MF ALT_MODE IN 0");
      add_restriction (User + Physical_inst,	Mf + Cc,		"0",	"MBOX INST --> MF CC IN 0");
      add_restriction (User + Physical_inst,	Mf + Cc_ctl,		"0",	"MBOX INST --> MF CC_CTL IN 0");
      add_restriction (User + Physical_inst,	Mf + Mcsr,		"0",	"MBOX INST --> MF MCSR IN 0");
      add_restriction (User + Physical_inst,	Mf + Dc_flush,		"0",	"MBOX INST --> MF DC_FLUSH IN 0");
      add_restriction (User + Physical_inst,	Mf + Dc_perr_stat,	"012",	"MBOX INST --> MF DC_PERR_STAT IN 012");
      add_restriction (User + Physical_inst,	Mf + Dc_test_ctl,	"0",	"MBOX INST --> MF DC_TEST_CTL IN 0");
      add_restriction (User + Physical_inst,	Mf + Dc_test_tag,	"0",	"MBOX INST --> MF DC_TEST_TAG IN 0");
      add_restriction (User + Physical_inst,	Mf + Dc_test_tag_temp,	"0",	"MBOX INST --> MF DC_TEST_TAG_TEMP IN 0");
      add_restriction (User + Physical_inst,	Mf + Dc_mode,		"0",	"MBOX INST --> MF DC_MODE IN 0");
      add_restriction (User + Physical_inst,	Mf + Maf_mode,		"012",
	"WARNING - MBOX INST --> MF MAF_MODE IN 012. WB_PENDING/DREAD_PENDING may not be updated");
      add_restriction (User + Virtual_inst,	Mt + Paltemp0,		"01",	"VIRTUAL MBOX INST --> MT PT0 IN 01");
      add_restriction (User + Virtual_inst,	Mt + Paltemp1,		"01",	"VIRTUAL MBOX INST --> MT PT1 IN 01");
      add_restriction (User + Virtual_inst,	Mt + Paltemp2,		"01",	"VIRTUAL MBOX INST --> MT PT2 IN 01");
      add_restriction (User + Virtual_inst,	Mt + Paltemp3,		"01",	"VIRTUAL MBOX INST --> MT PT3 IN 01");
      add_restriction (User + Virtual_inst,	Mt + Paltemp4,		"01",	"VIRTUAL MBOX INST --> MT PT4 IN 01");
      add_restriction (User + Virtual_inst,	Mt + Paltemp5,		"01",	"VIRTUAL MBOX INST --> MT PT5 IN 01");
      add_restriction (User + Virtual_inst,	Mt + Paltemp6,		"01",	"VIRTUAL MBOX INST --> MT PT6 IN 01");
      add_restriction (User + Virtual_inst,	Mt + Paltemp7,		"01",	"VIRTUAL MBOX INST --> MT PT7 IN 01");
      add_restriction (User + Virtual_inst,	Mt + Paltemp8,		"01",	"VIRTUAL MBOX INST --> MT PT8 IN 01");
      add_restriction (User + Virtual_inst,	Mt + Paltemp9,		"01",	"VIRTUAL MBOX INST --> MT PT9 IN 01");
      add_restriction (User + Virtual_inst,	Mt + PaltempA,		"01",	"VIRTUAL MBOX INST --> MT PT10 IN 01");
      add_restriction (User + Virtual_inst,	Mt + PaltempB,		"01",	"VIRTUAL MBOX INST --> MT PT11 IN 01");
      add_restriction (User + Virtual_inst,	Mt + PaltempC,		"01",	"VIRTUAL MBOX INST --> MT PT12 IN 01");
      add_restriction (User + Virtual_inst,	Mt + PaltempD,		"01",	"VIRTUAL MBOX INST --> MT PT13 IN 01");
      add_restriction (User + Virtual_inst,	Mt + PaltempE,		"01",	"VIRTUAL MBOX INST --> MT PT14 IN 01");
      add_restriction (User + Virtual_inst,	Mt + PaltempF,		"01",	"VIRTUAL MBOX INST --> MT PT15 IN 01");
      add_restriction (User + Virtual_inst,	Mt + Paltemp10,		"01",	"VIRTUAL MBOX INST --> MT PT16 IN 01");
      add_restriction (User + Virtual_inst,	Mt + Paltemp11,		"01",	"VIRTUAL MBOX INST --> MT PT17 IN 01");
      add_restriction (User + Virtual_inst,	Mt + Paltemp12,		"01",	"VIRTUAL MBOX INST --> MT PT18 IN 01");
      add_restriction (User + Virtual_inst,	Mt + Paltemp13,		"01",	"VIRTUAL MBOX INST --> MT PT19 IN 01");
      add_restriction (User + Virtual_inst,	Mt + Paltemp14,		"01",	"VIRTUAL MBOX INST --> MT PT20 IN 01");
      add_restriction (User + Virtual_inst,	Mt + Paltemp15,		"01",	"VIRTUAL MBOX INST --> MT PT21 IN 01");
      add_restriction (User + Virtual_inst,	Mt + Paltemp16,		"01",	"VIRTUAL MBOX INST --> MT PT22 IN 01");
      add_restriction (User + Virtual_inst,	Mt + Paltemp17,		"01",	"VIRTUAL MBOX INST --> MT PT23 IN 01");
      add_restriction (User + Virtual_inst,     Mt + Isr,         	"01",   "VIRTUAL MBOX INST --> MT ISR IN 01");
      add_restriction (User + Virtual_inst,     Mt + Itb_tag,		"01",   "VIRTUAL MBOX INST --> MT ITB_TAG IN 01");
      add_restriction (User + Virtual_inst,     Mt + Itb_pte,         	"01",   "VIRTUAL MBOX INST --> MT ITB_PTE IN 01");
      add_restriction (User + Virtual_inst,     Mt + Itb_asn,         	"01",   "VIRTUAL MBOX INST --> MT ITB_ASN IN 01");
      add_restriction (User + Virtual_inst,     Mt + Itb_pte_temp,      "01",   "VIRTUAL MBOX INST --> MT ITB_PTE_TEMP IN 01");
      add_restriction (User + Virtual_inst,     Mt + Itbia,         	"01",   "VIRTUAL MBOX INST --> MT ITBIA IN 01");
      add_restriction (User + Virtual_inst,     Mt + Itbiap,         	"01",   "VIRTUAL MBOX INST --> MT ITBIAP IN 01");
      add_restriction (User + Virtual_inst,     Mt + Itbis,         	"01",   "VIRTUAL MBOX INST --> MT ITBIS IN 01");
      add_restriction (User + Virtual_inst,     Mt + Sirr,         	"01",   "VIRTUAL MBOX INST --> MT SIRR IN 01");
      add_restriction (User + Virtual_inst,     Mt + Ifault_va_form,    "01",   "VIRTUAL MBOX INST --> MT IFAULT_VA_FORM IN 01");
      add_restriction (User + Virtual_inst,     Mt + Ivptbr,         	"01",   "VIRTUAL MBOX INST --> MT IVPTBR IN 01");
      add_restriction (User + Virtual_inst,     Mt + Exc_sum,         	"01",   "VIRTUAL MBOX INST --> MT EXC_SUM IN 01");
      add_restriction (User + Virtual_inst,     Mt + Exc_mask,		"01",   "VIRTUAL MBOX INST --> MT EXC_MASK IN 01");
      add_restriction (User + Virtual_inst,     Mt + Pal_base,		"01",   "VIRTUAL MBOX INST --> MT PAL_BASE IN 01");
      add_restriction (User + Virtual_inst,     Mt + Ps,		"01",   "VIRTUAL MBOX INST --> MT PS IN 01");
      add_restriction (User + Virtual_inst,     Mt + Ipl,		"01",   "VIRTUAL MBOX INST --> MT IPL IN 01");
      add_restriction (User + Virtual_inst,     Mt + Intid,         	"01",   "VIRTUAL MBOX INST --> MT INTID IN 01");
      add_restriction (User + Virtual_inst,     Mt + Astrr,		"01",   "VIRTUAL MBOX INST --> MT ASTRR IN 01");
      add_restriction (User + Virtual_inst,     Mt + Aster,         	"01",   "VIRTUAL MBOX INST --> MT ASTER IN 01");
      add_restriction (User + Virtual_inst,     Mt + Hwint_clr,         "01",   "VIRTUAL MBOX INST --> MT HWINT_CLR IN 01");
      add_restriction (User + Virtual_inst,     Mt + Sl_xmit,		"01",   "VIRTUAL MBOX INST --> MT SL_XMIT IN 01");
      add_restriction (User + Virtual_inst,     Mt + Sl_rcv,         	"01",   "VIRTUAL MBOX INST --> MT SL_RCV IN 01");
      add_restriction (User + Virtual_inst,     Mt + Icsr,       	"01",   "VIRTUAL MBOX INST --> MT ICSR IN 01");
      add_restriction (User + Virtual_inst,     Mt + Ic_flush,		"01",   "VIRTUAL MBOX INST --> MT IC_FLUSH IN 01");
      add_restriction (User + Virtual_inst,     Mt + Ic_perr_stat,	"01",   "VIRTUAL MBOX INST --> MT IC_PERR_STAT IN 01");
      add_restriction (User + Virtual_inst,     Mt + Pmctr,         	"01",   "VIRTUAL MBOX INST --> MT PMCTR IN 01");

      add_restriction (User + Physical_inst,	Mt + Paltemp0,		"01",	"PHYSICAL MBOX INST --> MT PT0 IN 01");
      add_restriction (User + Physical_inst,	Mt + Paltemp1,		"01",	"PHYSICAL MBOX INST --> MT PT1 IN 01");
      add_restriction (User + Physical_inst,	Mt + Paltemp2,		"01",	"PHYSICAL MBOX INST --> MT PT2 IN 01");
      add_restriction (User + Physical_inst,	Mt + Paltemp3,		"01",	"PHYSICAL MBOX INST --> MT PT3 IN 01");
      add_restriction (User + Physical_inst,	Mt + Paltemp4,		"01",	"PHYSICAL MBOX INST --> MT PT4 IN 01");
      add_restriction (User + Physical_inst,	Mt + Paltemp5,		"01",	"PHYSICAL MBOX INST --> MT PT5 IN 01");
      add_restriction (User + Physical_inst,	Mt + Paltemp6,		"01",	"PHYSICAL MBOX INST --> MT PT6 IN 01");
      add_restriction (User + Physical_inst,	Mt + Paltemp7,		"01",	"PHYSICAL MBOX INST --> MT PT7 IN 01");
      add_restriction (User + Physical_inst,	Mt + Paltemp8,		"01",	"PHYSICAL MBOX INST --> MT PT8 IN 01");
      add_restriction (User + Physical_inst,	Mt + Paltemp9,		"01",	"PHYSICAL MBOX INST --> MT PT9 IN 01");
      add_restriction (User + Physical_inst,	Mt + PaltempA,		"01",	"PHYSICAL MBOX INST --> MT PT10 IN 01");
      add_restriction (User + Physical_inst,	Mt + PaltempB,		"01",	"PHYSICAL MBOX INST --> MT PT11 IN 01");
      add_restriction (User + Physical_inst,	Mt + PaltempC,		"01",	"PHYSICAL MBOX INST --> MT PT12 IN 01");
      add_restriction (User + Physical_inst,	Mt + PaltempD,		"01",	"PHYSICAL MBOX INST --> MT PT13 IN 01");
      add_restriction (User + Physical_inst,	Mt + PaltempE,		"01",	"PHYSICAL MBOX INST --> MT PT14 IN 01");
      add_restriction (User + Physical_inst,	Mt + PaltempF,		"01",	"PHYSICAL MBOX INST --> MT PT15 IN 01");
      add_restriction (User + Physical_inst,	Mt + Paltemp10,		"01",	"PHYSICAL MBOX INST --> MT PT16 IN 01");
      add_restriction (User + Physical_inst,	Mt + Paltemp11,		"01",	"PHYSICAL MBOX INST --> MT PT17 IN 01");
      add_restriction (User + Physical_inst,	Mt + Paltemp12,		"01",	"PHYSICAL MBOX INST --> MT PT18 IN 01");
      add_restriction (User + Physical_inst,	Mt + Paltemp13,		"01",	"PHYSICAL MBOX INST --> MT PT19 IN 01");
      add_restriction (User + Physical_inst,	Mt + Paltemp14,		"01",	"PHYSICAL MBOX INST --> MT PT20 IN 01");
      add_restriction (User + Physical_inst,	Mt + Paltemp15,		"01",	"PHYSICAL MBOX INST --> MT PT21 IN 01");
      add_restriction (User + Physical_inst,	Mt + Paltemp16,		"01",	"PHYSICAL MBOX INST --> MT PT22 IN 01");
      add_restriction (User + Physical_inst,	Mt + Paltemp17,		"01",	"PHYSICAL MBOX INST --> MT PT23 IN 01");
      add_restriction (User + Physical_inst,     Mt + Isr,         	"01",   "PHYSICAL MBOX INST --> MT ISR IN 01");
      add_restriction (User + Physical_inst,     Mt + Itb_tag,		"01",   "PHYSICAL MBOX INST --> MT ITB_TAG IN 01");
      add_restriction (User + Physical_inst,     Mt + Itb_pte,         	"01",   "PHYSICAL MBOX INST --> MT ITB_PTE IN 01");
      add_restriction (User + Physical_inst,     Mt + Itb_asn,         	"01",   "PHYSICAL MBOX INST --> MT ITB_ASN IN 01");
      add_restriction (User + Physical_inst,     Mt + Itb_pte_temp,      "01",   "PHYSICAL MBOX INST --> MT ITB_PTE_TEMP IN 01");
      add_restriction (User + Physical_inst,     Mt + Itbia,         	"01",   "PHYSICAL MBOX INST --> MT ITBIA IN 01");
      add_restriction (User + Physical_inst,     Mt + Itbiap,         	"01",   "PHYSICAL MBOX INST --> MT ITBIAP IN 01");
      add_restriction (User + Physical_inst,     Mt + Itbis,         	"01",   "PHYSICAL MBOX INST --> MT ITBIS IN 01");
      add_restriction (User + Physical_inst,     Mt + Sirr,         	"01",   "PHYSICAL MBOX INST --> MT SIRR IN 01");
      add_restriction (User + Physical_inst,     Mt + Ifault_va_form,    "01",   "PHYSICAL MBOX INST --> MT IFAULT_VA_FORM IN 01");
      add_restriction (User + Physical_inst,     Mt + Ivptbr,         	"01",   "PHYSICAL MBOX INST --> MT IVPTBR IN 01");
      add_restriction (User + Physical_inst,     Mt + Exc_sum,         	"01",   "PHYSICAL MBOX INST --> MT EXC_SUM IN 01");
      add_restriction (User + Physical_inst,     Mt + Exc_mask,		"01",   "PHYSICAL MBOX INST --> MT EXC_MASK IN 01");
      add_restriction (User + Physical_inst,     Mt + Pal_base,		"01",   "PHYSICAL MBOX INST --> MT PAL_BASE IN 01");
      add_restriction (User + Physical_inst,     Mt + Ps,		"01",   "PHYSICAL MBOX INST --> MT PS IN 01");
      add_restriction (User + Physical_inst,     Mt + Ipl,		"01",   "PHYSICAL MBOX INST --> MT IPL IN 01");
      add_restriction (User + Physical_inst,     Mt + Intid,         	"01",   "PHYSICAL MBOX INST --> MT INTID IN 01");
      add_restriction (User + Physical_inst,     Mt + Astrr,		"01",   "PHYSICAL MBOX INST --> MT ASTRR IN 01");
      add_restriction (User + Physical_inst,     Mt + Aster,         	"01",   "PHYSICAL MBOX INST --> MT ASTER IN 01");
      add_restriction (User + Physical_inst,     Mt + Hwint_clr,         "01",   "PHYSICAL MBOX INST --> MT HWINT_CLR IN 01");
      add_restriction (User + Physical_inst,     Mt + Sl_xmit,		"01",   "PHYSICAL MBOX INST --> MT SL_XMIT IN 01");
      add_restriction (User + Physical_inst,     Mt + Sl_rcv,         	"01",   "PHYSICAL MBOX INST --> MT SL_RCV IN 01");
      add_restriction (User + Physical_inst,     Mt + Icsr,       	"01",   "PHYSICAL MBOX INST --> MT ICSR IN 01");
      add_restriction (User + Physical_inst,     Mt + Ic_flush,		"01",   "PHYSICAL MBOX INST --> MT IC_FLUSH IN 01");
      add_restriction (User + Physical_inst,     Mt + Ic_perr_stat,	"01",   "PHYSICAL MBOX INST --> MT IC_PERR_STAT IN 01");
      add_restriction (User + Physical_inst,     Mt + Pmctr,         	"01",   "PHYSICAL MBOX INST --> MT PMCTR IN 01");

      add_restriction (User + Wmb,	Mt + Paltemp0,		"01",	"WMB --> MT PT0 IN 01");
      add_restriction (User + Wmb,	Mt + Paltemp1,		"01",	"WMB --> MT PT1 IN 01");
      add_restriction (User + Wmb,	Mt + Paltemp2,		"01",	"WMB --> MT PT2 IN 01");
      add_restriction (User + Wmb,	Mt + Paltemp3,		"01",	"WMB --> MT PT3 IN 01");
      add_restriction (User + Wmb,	Mt + Paltemp4,		"01",	"WMB --> MT PT4 IN 01");
      add_restriction (User + Wmb,	Mt + Paltemp5,		"01",	"WMB --> MT PT5 IN 01");
      add_restriction (User + Wmb,	Mt + Paltemp6,		"01",	"WMB --> MT PT6 IN 01");
      add_restriction (User + Wmb,	Mt + Paltemp7,		"01",	"WMB --> MT PT7 IN 01");
      add_restriction (User + Wmb,	Mt + Paltemp8,		"01",	"WMB --> MT PT8 IN 01");
      add_restriction (User + Wmb,	Mt + Paltemp9,		"01",	"WMB --> MT PT9 IN 01");
      add_restriction (User + Wmb,	Mt + PaltempA,		"01",	"WMB --> MT PT10 IN 01");
      add_restriction (User + Wmb,	Mt + PaltempB,		"01",	"WMB --> MT PT11 IN 01");
      add_restriction (User + Wmb,	Mt + PaltempC,		"01",	"WMB --> MT PT12 IN 01");
      add_restriction (User + Wmb,	Mt + PaltempD,		"01",	"WMB --> MT PT13 IN 01");
      add_restriction (User + Wmb,	Mt + PaltempE,		"01",	"WMB --> MT PT14 IN 01");
      add_restriction (User + Wmb,	Mt + PaltempF,		"01",	"WMB --> MT PT15 IN 01");
      add_restriction (User + Wmb,	Mt + Paltemp10,		"01",	"WMB --> MT PT16 IN 01");
      add_restriction (User + Wmb,	Mt + Paltemp11,		"01",	"WMB --> MT PT17 IN 01");
      add_restriction (User + Wmb,	Mt + Paltemp12,		"01",	"WMB --> MT PT18 IN 01");
      add_restriction (User + Wmb,	Mt + Paltemp13,		"01",	"WMB --> MT PT19 IN 01");
      add_restriction (User + Wmb,	Mt + Paltemp14,		"01",	"WMB --> MT PT20 IN 01");
      add_restriction (User + Wmb,	Mt + Paltemp15,		"01",	"WMB --> MT PT21 IN 01");
      add_restriction (User + Wmb,	Mt + Paltemp16,		"01",	"WMB --> MT PT22 IN 01");
      add_restriction (User + Wmb,	Mt + Paltemp17,		"01",	"WMB --> MT PT23 IN 01");
      add_restriction (User + Wmb,     Mt + Isr,         	"01",   "WMB --> MT ISR IN 01");
      add_restriction (User + Wmb,     Mt + Itb_tag,		"01",   "WMB --> MT ITB_TAG IN 01");
      add_restriction (User + Wmb,     Mt + Itb_pte,         	"01",   "WMB --> MT ITB_PTE IN 01");
      add_restriction (User + Wmb,     Mt + Itb_asn,         	"01",   "WMB --> MT ITB_ASN IN 01");
      add_restriction (User + Wmb,     Mt + Itb_pte_temp,      "01",   "WMB --> MT ITB_PTE_TEMP IN 01");
      add_restriction (User + Wmb,     Mt + Itbia,         	"01",   "WMB --> MT ITBIA IN 01");
      add_restriction (User + Wmb,     Mt + Itbiap,         	"01",   "WMB --> MT ITBIAP IN 01");
      add_restriction (User + Wmb,     Mt + Itbis,         	"01",   "WMB --> MT ITBIS IN 01");
      add_restriction (User + Wmb,     Mt + Sirr,         	"01",   "WMB --> MT SIRR IN 01");
      add_restriction (User + Wmb,     Mt + Ifault_va_form,    "01",   "WMB --> MT IFAULT_VA_FORM IN 01");
      add_restriction (User + Wmb,     Mt + Ivptbr,         	"01",   "WMB --> MT IVPTBR IN 01");
      add_restriction (User + Wmb,     Mt + Exc_sum,         	"01",   "WMB --> MT EXC_SUM IN 01");
      add_restriction (User + Wmb,     Mt + Exc_mask,		"01",   "WMB --> MT EXC_MASK IN 01");
      add_restriction (User + Wmb,     Mt + Pal_base,		"01",   "WMB --> MT PAL_BASE IN 01");
      add_restriction (User + Wmb,     Mt + Ps,		"01",   "WMB --> MT PS IN 01");
      add_restriction (User + Wmb,     Mt + Ipl,		"01",   "WMB --> MT IPL IN 01");
      add_restriction (User + Wmb,     Mt + Intid,         	"01",   "WMB --> MT INTID IN 01");
      add_restriction (User + Wmb,     Mt + Astrr,		"01",   "WMB --> MT ASTRR IN 01");
      add_restriction (User + Wmb,     Mt + Aster,         	"01",   "WMB --> MT ASTER IN 01");
      add_restriction (User + Wmb,     Mt + Hwint_clr,         "01",   "WMB --> MT HWINT_CLR IN 01");
      add_restriction (User + Wmb,     Mt + Sl_xmit,		"01",   "WMB --> MT SL_XMIT IN 01");
      add_restriction (User + Wmb,     Mt + Sl_rcv,         	"01",   "WMB --> MT SL_RCV IN 01");
      add_restriction (User + Wmb,     Mt + Icsr,       	"01",   "WMB --> MT ICSR IN 01");
      add_restriction (User + Wmb,     Mt + Ic_flush,		"01",   "WMB --> MT IC_FLUSH IN 01");
      add_restriction (User + Wmb,     Mt + Ic_perr_stat,	"01",   "WMB --> MT IC_PERR_STAT IN 01");
      add_restriction (User + Wmb,     Mt + Pmctr,         	"01",   "WMB --> MT PMCTR IN 01");

/* new restrictions 13-jan-1994 */
      add_restriction (Mf + Itb_pte,		Mf + Itb_pte_temp,	"123",	"MF ITB_PTE --> MF ITB_PTE_TEMP IN 123");
      add_restriction (Mf + Itb_pte,		Mf + Itb_pte,		"1",	"MF ITB_PTE --> MF ITB_PTE IN 1");

      add_restriction (Mt + Isr,		Mf + Isr,		"12",	"MT ISR --> MF ISR IN 12");
      add_restriction (Mt + Itb_tag,		Mf + Itb_tag,		"12",	"MT ITB_TAG --> MF ITB_TAG IN 12");
      add_restriction (Mt + Itb_pte,		Mf + Itb_pte,		"12",	"MT ITB_PTE --> MF ITB_PTE IN 12");
      add_restriction (Mt + Itb_asn,		Mf + Itb_asn,		"12",	"MT ITB_ASN --> MF ITB_ASN IN 12");
      add_restriction (Mt + Itb_pte_temp,	Mf + Itb_pte_temp,	"12",	"MT ITB_PTE_TEMP --> MF ITB_PTE_TEMP IN 12");
      add_restriction (Mt + Itbia,		Mf + Itbia,		"12",	"MT ITBIA --> MF ITBIA IN 12");
      add_restriction (Mt + Itbiap,		Mf + Itbiap,		"12",	"MT ITBIAP --> MF ITBIAP IN 12");
      add_restriction (Mt + Itbis,		Mf + Itbis,		"12",	"MT ITBIS --> MF ITBIS IN 12");
      add_restriction (Mt + Sirr,		Mf + Sirr,		"12",	"MT SIRR --> MF SIRR IN 12");
      add_restriction (Mt + Ifault_va_form, 	Mf + Ifault_va_form,	"12",	"MT IFAULT_VA_FORM --> MF IFAULT_VA_FORM IN 12");
      add_restriction (Mt + Ivptbr,		Mf + Ivptbr,		"12",	"MT IVPTBR --> MF IVPTBR IN 12");
      add_restriction (Mt + Exc_addr,		Mf + Exc_addr,		"12",	"MT EXC_ADDR --> MF EXC_ADDR IN 12");
      add_restriction (Mt + Exc_sum,		Mf + Exc_sum,		"12",	"MT EXC_SUM --> MF EXC_SUM IN 12");
      add_restriction (Mt + Exc_mask,		Mf + Exc_mask,		"12",	"MT EXC_MASK --> MF EXC_MASK IN 12");
      add_restriction (Mt + Pal_base,		Mf + Pal_base,		"12",	"MT PAL_BASE --> MF PAL_BASE IN 12");
      add_restriction (Mt + Ps,			Mf + Ps,		"12",	"MT PS --> MF PS IN 12");
      add_restriction (Mt + Ipl,		Mf + Ipl,		"12",	"MT IPL --> MF IPL IN 12");
      add_restriction (Mt + Intid,		Mf + Intid,		"12",	"MT INTID --> MF INTID IN 12");
      add_restriction (Mt + Astrr,		Mf + Astrr,		"12",	"MT ASTRR --> MF ASTRR IN 12");
      add_restriction (Mt + Aster,		Mf + Aster,		"12",	"MT ASTER --> MF ASTER IN 12");
      add_restriction (Mt + Hwint_clr,		Mf + Hwint_clr,		"12",	"MT HWINT_CLR --> MF HWINT_CLR IN 12");
      add_restriction (Mt + Sl_xmit,		Mf + Sl_xmit,		"12",	"MT SL_XMIT --> MF SL_XMIT IN 12");
      add_restriction (Mt + Sl_rcv,		Mf + Sl_rcv,		"12",	"MT SL_RCV --> MF SL_RCV IN 12");
      add_restriction (Mt + Icsr,		Mf + Icsr,		"12",	"MT ICSR --> MF ICSR IN 12");
      add_restriction (Mt + Ic_flush,		Mf + Ic_flush,		"12",	"MT IC_FLUSH --> MF IC_FLUSH IN 12");
      add_restriction (Mt + Ic_perr_stat,	Mf + Ic_perr_stat,	"12",	"MT IC_PERR_STAT --> MF IC_PERR_STAT IN 12");
      add_restriction (Mt + Pmctr,		Mf + Pmctr,		"12",	"MT PMCTR --> MF PMCTR IN 12");
      add_restriction (Mt + Paltemp0,		Mf + Paltemp0,		"12",	"MT PALTEMP0 --> MF PALTEMP0 IN 12");
      add_restriction (Mt + Paltemp1,		Mf + Paltemp1,		"12",	"MT PALTEMP1 --> MF PALTEMP1 IN 12");
      add_restriction (Mt + Paltemp2,		Mf + Paltemp2,		"12",	"MT PALTEMP2 --> MF PALTEMP2 IN 12");
      add_restriction (Mt + Paltemp3,		Mf + Paltemp3,		"12",	"MT PALTEMP3 --> MF PALTEMP3 IN 12");
      add_restriction (Mt + Paltemp4,		Mf + Paltemp4,		"12",	"MT PALTEMP4 --> MF PALTEMP4 IN 12");
      add_restriction (Mt + Paltemp5,		Mf + Paltemp5,		"12",	"MT PALTEMP5 --> MF PALTEMP5 IN 12");
      add_restriction (Mt + Paltemp6,		Mf + Paltemp6,		"12",	"MT PALTEMP6 --> MF PALTEMP6 IN 12");
      add_restriction (Mt + Paltemp7,		Mf + Paltemp7,		"12",	"MT PALTEMP7 --> MF PALTEMP7 IN 12");
      add_restriction (Mt + Paltemp8,		Mf + Paltemp8,		"12",	"MT PALTEMP8 --> MF PALTEMP8 IN 12");
      add_restriction (Mt + Paltemp9,		Mf + Paltemp9,		"12",	"MT PALTEMP9 --> MF PALTEMP9 IN 12");
      add_restriction (Mt + PaltempA,		Mf + PaltempA,		"12",	"MT PALTEMPA --> MF PALTEMPA IN 12");
      add_restriction (Mt + PaltempB,		Mf + PaltempB,		"12",	"MT PALTEMPB --> MF PALTEMPB IN 12");
      add_restriction (Mt + PaltempC,		Mf + PaltempC,		"12",	"MT PALTEMPC --> MF PALTEMPC IN 12");
      add_restriction (Mt + PaltempD,		Mf + PaltempD,		"12",	"MT PALTEMPD --> MF PALTEMPD IN 12");
      add_restriction (Mt + PaltempE,		Mf + PaltempE,		"12",	"MT PALTEMPE --> MF PALTEMPE IN 12");
      add_restriction (Mt + PaltempF,		Mf + PaltempF,		"12",	"MT PALTEMPF --> MF PALTEMPF IN 12");
      add_restriction (Mt + Paltemp10,		Mf + Paltemp10,		"12",	"MT PALTEMP10 --> MF PALTEMP10 IN 12");
      add_restriction (Mt + Paltemp11,		Mf + Paltemp11,		"12",	"MT PALTEMP11 --> MF PALTEMP11 IN 12");
      add_restriction (Mt + Paltemp12,		Mf + Paltemp12,		"12",	"MT PALTEMP12 --> MF PALTEMP12 IN 12");
      add_restriction (Mt + Paltemp13,		Mf + Paltemp13,		"12",	"MT PALTEMP13 --> MF PALTEMP13 IN 12");
      add_restriction (Mt + Paltemp14,		Mf + Paltemp14,		"12",	"MT PALTEMP14 --> MF PALTEMP14 IN 12");
      add_restriction (Mt + Paltemp15,		Mf + Paltemp15,		"12",	"MT PALTEMP15 --> MF PALTEMP15 IN 12");
      add_restriction (Mt + Paltemp16,		Mf + Paltemp16,		"12",	"MT PALTEMP16 --> MF PALTEMP16 IN 12");
      add_restriction (Mt + Paltemp17,		Mf + Paltemp17,		"12",	"MT PALTEMP17 --> MF PALTEMP17 IN 12");

      add_restriction (Mt + Astrr,		Mf + Intid,		"012345","MT ASTRR --> MF INTID IN 012345");
      add_restriction (Mt + Aster,		Mf + Intid,		"012345","MT ASTER --> MF INTID IN 012345");
      add_restriction (Mt + Sirr,		Mf + Intid,		"01234","MT SIRR --> MF INTID IN 01234");

/* Astrr & Aster need cycles 0 & 1 to complete interrupt before exiting palcode */
/* Astrr, Aster need cycles 0,1,2 and Sirr needs 0 & 1 in all flows for hidden mf intid's */
      add_restriction (Mt + Astrr,		User + Hwrei,		"01",	"MT ASTRR --> HW_REI IN 01");
      add_restriction (Mt + Aster,		User + Hwrei,		"01",	"MT ASTER --> HW_REI IN 01");

      add_restriction (Mt + Exc_addr,		User + Hwrei,		"01",	"MT EXC_ADDR --> HW_REI IN 01");
      add_restriction (Mt + Exc_addr,		User + Hwrei_stall,	"01",	"MT EXC_ADDR --> HW_REI_STALL IN 01");

      add_restriction (Mt + Icsr,		User + Hwrei,		"01234","MT ICSR --> HW_REI IN 01234");
      add_restriction (Mt + Icsr,		User + Hwrei_stall,	"01",	"MT ICSR --> HW_REI_STALL IN 01");

      add_restriction (Mt + Itb_asn,		User + Hwrei_stall,	"01234", "MT ITB_ASN --> HW_REI_STALL IN 01234");
      add_restriction (Mt + Itb_asn,		User + Hwrei,		"01234567", "MT ITB_ASN MUST BE FOLLOWED BY HW_REI_STALL ");

      add_restriction (Mt + Itb_asn,		Mt + Itbis,	"0123", "MT ITB_ASN --> MT ITBIS IN 0123");

      add_restriction (Mt + Ivptbr, 		Mf + Ifault_va_form,	"012",	"MF IFAULT_VA_FORM WON'T HAVE NEW IVPTBR IN 012");
	
      add_restriction (Mt + Pal_base,		User + Epicode, 	"01234567","MT PAL_BASE --> CALL_PAL IN 01234567");
      add_restriction (Mt + Pal_base,		User + Hwrei, 		"0123456","MT PAL_BASE --> HW_REI IN 0123456");
      add_restriction (Mt + Pal_base,		User + Hwrei_stall, 	"0123456","MT PAL_BASE --> HW_REI_STALL IN 0123456");

      add_restriction (Mt + Ps,			User + Hwrei,		"012",	"MT PS --> HW_REI IN 012");
      add_restriction (Mt + Ps,			User + Hwrei_stall,	"012",	"MT PS --> HW_REI_STALL IN 012");

      add_restriction (Mt + Cc, 		User + Rpcc, 		"012", 	"MT CC --> RPCC IN 012");
      add_restriction (Mt + Cc_ctl, 		User + Rpcc, 		"012", 	"MT CC_CTL --> RPCC IN 012");

      add_restriction (Mt + Dc_flush, 		User + Virtual_inst, 	"12", 	"MT DC_FLUSH --> MBOX INST IN 12");
      add_restriction (Mt + Dc_flush, 		User + Physical_inst, 	"12", 	"MT DC_FLUSH --> MBOX INST IN 12");

      add_restriction (Mt + Dc_mode, 		User + Virtual_inst, 	"1234",	"MT DC_MODE --> MBOX INST IN 1234");
      add_restriction (Mt + Dc_mode, 		User + Physical_inst, 	"1234",	"MT DC_MODE --> MBOX INST IN 1234");
      add_restriction (Mt + Dc_mode, 		Mf + Dc_mode, 		"12",	"MT DC_MODE --> MF DC_MODE IN 12");

      add_restriction (Mt + Dc_perr_stat, 	User + Virtual_inst, 	"1", 	"MT DC_PERR_STAT --> MBOX INST IN 1");
      add_restriction (Mt + Dc_perr_stat, 	User + Physical_inst, 	"1", 	"MT DC_PERR_STAT --> MBOX INST IN 1");
      add_restriction (Mt + Dc_perr_stat, 	Mf + Dc_perr_stat, 	"123", 	"MT DCPERR_STAT --> MF DCPERR_STAT IN 123");

      add_restriction (Mt + Dc_test_ctl,	Mf + Dc_test_tag, 	"123",	"MT DC_TEST_CTL --> MF DC_TEST_TAG IN 123");
      add_restriction (Mt + Dc_test_ctl,	Mf + Dc_test_ctl, 	"12",	"MT DC_TEST_CTL --> MF DC_TEST_CTL IN 12");

      add_restriction (Mt + Dc_test_tag,	Mf + Dc_test_tag, 	"123",	"MT DC_TEST_TAG --> MF DC_TEST_TAG IN 123");
/* 16-mar-95 next 3 new restrictions added: */
      add_restriction (Mt + Dc_test_tag,	User + Virtual_inst,	"123",	"MT DC_TEST_TAG --> VIRTUAL ACCESS IN 123");
      add_restriction (Mt + Dc_test_tag,	User + Physical_inst,	"123",	"MT DC_TEST_TAG --> PHYSICAL ACCESS IN 123");
      add_restriction (Mt + Dc_test_tag,	User + Hwrei, 		"012",	"MT DC_TEST_TAG --> HWREI IN 012");

      add_restriction (Mt + Dtb_asn, 		User + Virtual_inst, 	"123",	"MT DTB_ASN --> VIRTUAL ACCESS IN 123");

      add_restriction (Mt + Dtb_cm, 		User + Virtual_inst, 	"12",	"MT DTB_CM --> VIRTUAL ACCESS IN 12");
      add_restriction (Mt + Alt_mode, 		User + Virtual_inst, 	"12",	"MT ALT_MODE --> VIRTUAL ACCESS IN 12");

/* 30-jun-94 changed the following from "2" to "12" to account for unforseen delay (alloc_cycle)? */
      add_restriction (Mt + Dtb_pte, 		User + Virtual_inst, 	"012",	"MT DTB_PTE --> VIRTUAL ACCESS IN 012");
      add_restriction (Mt + Dtb_pte, 		Mt + Dtb_asn, 		"12",	"MT DTB_PTE --> MT DTB_ASN IN 12");
      add_restriction (Mt + Dtb_pte, 		Mt + Dtb_cm, 		"12",	"MT DTB_PTE --> MT DTB_CM IN 12");
      add_restriction (Mt + Dtb_pte, 		Mt + Alt_mode, 		"12",	"MT DTB_PTE --> MT ALT_MODE IN 12");
      add_restriction (Mt + Dtb_pte, 		Mt + Mcsr, 		"12",	"MT DTB_PTE --> MT MCSR IN 12");
      add_restriction (Mt + Dtb_pte, 		Mt + Maf_mode, 		"12",	"MT DTB_PTE --> MT MAF_MODE IN 12");
      add_restriction (Mt + Dtb_pte, 		Mt + Dc_mode, 		"12",	"MT DTB_PTE --> MT DC_MODE IN 12");
      add_restriction (Mt + Dtb_pte, 		Mt + Dc_perr_stat, 	"12",	"MT DTB_PTE --> MT DC_PERR_STAT IN 12");
      add_restriction (Mt + Dtb_pte, 		Mt + Dc_test_ctl, 	"12",	"MT DTB_PTE --> MT DC_TEST_CTL IN 12");
      add_restriction (Mt + Dtb_pte, 		Mt + Dc_test_tag, 	"12",	"MT DTB_PTE --> MT DC_TEST_TAG IN 12");

      add_restriction (Mt + Dtb_tag,		User + Virtual_inst,	"123",	"MT DTB_TAG --> VIRTUAL ACCESS IN 123");
      add_restriction (Mt + Dtb_tag,		Mt + Dtb_tag,		"1",	"MT DTB_TAG --> MT DTB_TAG IN 1");
      add_restriction (Mt + Dtb_tag,		Mf + Dtb_pte,		"12",	"MT DTB_TAG --> MF DTB_PTE IN 12");
      add_restriction (Mt + Dtb_tag,		Mt + Dtbis,		"12",	"MT DTB_TAG --> MT DTBIS IN 12");

      add_restriction (Mt + Dtbiap,		User + Virtual_inst,	"0123",	"MT DTBIAP --> VIRTUAL ACCESS IN 0123");
      add_restriction (Mt + Dtbia,		User + Virtual_inst,	"0123",	"MT DTBIA --> VIRTUAL ACCESS IN 0123");
      add_restriction (Mt + Dtbiap,		Mt + Dtbis,		"012",	"MT DTBIAP --> MT DTBIS IN 12");
      add_restriction (Mt + Dtbia,		Mt + Dtbis,		"012",	"MT DTBIA --> MT DTBIS IN 12");
      add_restriction (Mt + Dtbia,		Mf + Dtb_pte,		"1",	"MT DTBIA --> MF DTB_PTE IN 11");

      add_restriction (Mt + Maf_mode,		User + Virtual_inst,	"123",	"MT MAF_MODE --> MBOX INST IN 123");
      add_restriction (Mt + Maf_mode,		User + Physical_inst,	"123",	"MT MAF_MODE --> MBOX INST IN 123");
      add_restriction (Mt + Maf_mode,		User + Wmb,		"123",	"MT MAF_MODE --> MB INST IN 123");
      add_restriction (Mt + Maf_mode,		Mf + Maf_mode,		"12",	"MT MAF_MODE --> MF MAF_MODE IN 12");

      add_restriction (Mt + Mcsr, 		User + Virtual_inst, 	"01234","MT MCSR --> VIRTUAL ACCESS IN 01234");
      add_restriction (Mt + Mcsr, 		Mf + Mcsr, 		"12", 	"MCSR DOESN'T HAVE UPDATED VALUE IN 12");
      add_restriction (Mt + Mcsr, 		Mf + Va_form, 		"123", 	"MT MCSR --> MF VA_FORM IN 123");

      add_restriction (Mt + Mvptbr, 		Mf + Va_form, 		"12", 	"MT MVPTBR --> MF VA_FORM IN 12");

      add_restriction (Mf + Dc_test_tag, 	Mf + Dc_test_tag_temp, 	"1", 	"MF DC_TEST_TAG --> MF DC_TEST_TAG_TEMP IN 1");

      add_restriction (Mf + Dtb_pte, 		User + Virtual_inst, 	"12", 	"MF DTB_PTE --> VIRTUAL ACCESS IN 12");
      add_restriction (Mf + Dtb_pte, 		User + Physical_inst, 	"1", 	"MF DTB_PTE --> MBOX INST IN 1");
      add_restriction (Mf + Dtb_pte, 		Mt + Dc_test_ctl,	"1", 	"MF DTB_PTE --> MT DC_TEST_CTL IN 1");
      add_restriction (Mf + Dtb_pte, 		Mt + Dc_test_tag, 	"1", 	"MF DTB_PTE --> MT DC_TEST_TAG IN 1");
      add_restriction (Mf + Dtb_pte, 		Mf + Dtb_pte_temp, 	"123", 	"MF DTB_PTE --> MF DTB_PTE_TEMP IN 123");
      add_restriction (Mf + Dtb_pte, 		Mf + Dtb_pte, 		"1", 	"MF DTB_PTE --> MF DTB_PTE IN 1");

/* added 24-JAN-93 */

      add_restriction (Mt + Dtbiap,		User + Hwrei,	"012",		"MT DTBIAP --> HWREI IN 012");
      add_restriction (Mt + Dtbia,		User + Hwrei,	"012",		"MT DTBIA --> HWREI IN 012");
      add_restriction (Mt + Cc, 		User + Hwrei,	"01", 		"MT CC --> HWREI IN 01");
      add_restriction (Mt + Cc_ctl, 		User + Hwrei,	"01", 		"MT CC_CTL --> HWREI IN 01");
      add_restriction (Mt + Dc_flush, 		User + Hwrei,	"01", 		"MT DC_FLUSH --> HWREI IN 01");
      add_restriction (Mt + Dc_mode, 		User + Hwrei,	"0123", 	"MT DC_MODE --> HWREI IN 0123");
      add_restriction (Mt + Dc_mode, 		User + Hwrei_stall,	"01", 	"MT DC_MODE --> HWREI_STALL IN 01");
      add_restriction (Mt + Dc_test_ctl,	User + Hwrei,	"0", 		"MT DC_TEST_CTL --> HWREI IN 0");
      add_restriction (Mt + Dtb_asn, 		User + Hwrei,	"012", 		"MT DTB_ASN --> HWREI IN 012");
      add_restriction (Mt + Dtb_cm, 		User + Hwrei,	"01",           "MT DTB_CM --> HWREI IN 01");
      add_restriction (Mt + Alt_mode, 		User + Hwrei,	"01",           "MT ALT_MODE --> HWREI IN 01");
      add_restriction (Mt + Dtb_tag,		User + Hwrei,	"012",          "MT DTB_TAG --> HWREI IN 012");
      add_restriction (Mt + Maf_mode,		User + Hwrei,	"012",          "MT MAF_MODE--> HWREI IN 012");
      add_restriction (Mt + Mcsr, 		User + Hwrei,	"0123",         "MT MCSR --> HWREI IN 0123");
      add_restriction (Mt + Mcsr, 		User + Hwrei_stall,	"01",         "MT MCSR --> HWREI_STALL IN 01");
      add_restriction (Mt + Dtb_pte, 		User + Hwrei,	"01",            "MT DTB_PTE --> HWREI IN 01");

/*  ???     add_restriction (User + Virtual_inst,	User + Hwrei,	"01",						*/
/*  ???	"WARNING - HWREI WITHIN 2 CYCLES OF MBOX INST\n  POSSIBLE HIDDEN MBOX INST --> MT MBOX IPR RESTRICTION");	*/

      add_restriction (Mt + Astrr,		User + Hwrei,	"012",
	"WARNING - HWREI WITHIN 2 CYCLES OF MT ASTRR\n  POSSIBLE HIDDEN MT ASTRR --> MF INTID RESTRICTION\n");
      add_restriction (Mt + Aster,		User + Hwrei,	"012",
	"WARNING - HWREI WITHIN 2 CYCLES OF MT ASTER\n  POSSIBLE HIDDEN MT ASTER --> MF INTID RESTRICTION\n");
      add_restriction (Mt + Sirr,		User + Hwrei,		"01",
	"WARNING - HWREI WITHIN 1 CYCLE OF MT SIRR\n  POSSIBLE HIDDEN MT SIRR --> MF INTID RESTRICTION\n");
      add_restriction (Mt + Sirr,		User + Hwrei,	"0123",
	"WARNING - HWREI WITHIN 3 CYCLES OF MT SIRR\n  POSSIBLE HIDDEN MT SIRR --> MF INTID RESTRICTION\n--ONLY 1 BUBBLE CYCLE NEEDED IN CALL_PALs");

#ifndef NOHIDDEN
/* the following are really only a problem in flows where the hw_rei can return directly to palcode - (which call_pals cannot) */
      add_restriction (Mt + Isr,		User + Hwrei,	"01",	
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT ISR --> MF ISR RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + Itb_pte,		User + Hwrei,	"01",
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT ITB_PTE --> MF ITB_PTE RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + Itb_asn,		User + Hwrei,	"01",
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT ITB_ASN --> MF ITB_ASN RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + Itb_pte_temp,	User + Hwrei,	"01",	
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT ITB_PTE_TEMP --> MF ITB_PTE_TEMP RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + Sirr,		User + Hwrei,	"01",		
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT SIRR --> MF SIRR RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + Astrr,		User + Hwrei,	"01234",
	"WARNING - HWREI WITHIN 4 CYCLES OF MT ASTRR\n  POSSIBLE HIDDEN MT ASTRR --> MF INTID RESTRICTION\n--ONLY 2 BUBBLE CYCLES NEEDED IN CALL_PALs");
      add_restriction (Mt + Aster,		User + Hwrei,	"01234",
	"WARNING - HWREI WITHIN 4 CYCLES OF MT ASTER\n  POSSIBLE HIDDEN MT ASTER --> MF INTID RESTRICTION\n--ONLY 2 BUBBLE CYCLES NEEDED IN CALL_PALs");
      add_restriction (Mt + Ifault_va_form,	User + Hwrei,	"01", 	
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT IFAULT_VA_FORM --> MF IFAULT_VA_FORM RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + Ivptbr,		User + Hwrei,	"01",		
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT IVPTBR --> MF IVPTBR RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + Exc_addr,		User + Hwrei,	"01",		
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT EXC_ADDR --> MF EXC+ADDR RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + Exc_sum,		User + Hwrei,	"01",		
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT EXC_SUM --> MF EXC_SUM RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + Exc_mask,		User + Hwrei,	"01",		
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT EXC_MASK --> MF EXC_MASK RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + Pal_base,		User + Hwrei,	"01",		
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT PAL_BASE --> MF PAL_BASE RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + Ps,			User + Hwrei,	"01",			
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT PS --> MF PS RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + Ipl,		User + Hwrei,	"01",		
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT IPL --> MF IPL RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + Intid,		User + Hwrei,	"01",		
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT INTID --> MF INTID RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + Astrr,		User + Hwrei,	"01",		
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT ASTRR --> MF ASTRR RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + Aster,		User + Hwrei,	"01",		
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT ASTER --> MF ASTER RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + Sl_rcv,		User + Hwrei,	"01",		
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT SL_RCV --> MF SL_RCV RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + Icsr,		User + Hwrei,	"01",		
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT ICSR --> MF ICSR RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + Ic_perr_stat,	User + Hwrei,	"01",	
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT IC_PERR_STAT --> MF IC_PERR_STAT RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + Pmctr,	User + Hwrei,	"01",	
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT PMCTR --> MF PMCTR RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + Paltemp0,		User + Hwrei,	"01",	
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT PALTEMP0 --> MF PALTEMP0 RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + Paltemp1,		User + Hwrei,	"01",		
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT PALTEMP1 --> MF PALTEMP1 RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + Paltemp2,		User + Hwrei,	"01",		
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT PALTEMP2 --> MF PALTEMP2 RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + Paltemp3,		User + Hwrei,	"01",		
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT PALTEMP3 --> MF PALTEMP3 RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + Paltemp4,		User + Hwrei,	"01",		
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT PALTEMP4 --> MF PALTEMP4 RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + Paltemp5,		User + Hwrei,	"01",		
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT PALTEMP5 --> MF PALTEMP5 RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + Paltemp6,		User + Hwrei,	"01",		
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT PALTEMP6 --> MF PALTEMP6 RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + Paltemp7,		User + Hwrei,	"01",		
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT PALTEMP7 --> MF PALTEMP7 RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + Paltemp8,		User + Hwrei,	"01",		
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT PALTEMP8 --> MF PALTEMP8 RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + Paltemp9,		User + Hwrei,	"01",		
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT PALTEMP9 --> MF PALTEMP9 RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + PaltempA,		User + Hwrei,	"01",		
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT PALTEMPA --> MF PALTEMPA RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + PaltempB,		User + Hwrei,	"01",		
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT PALTEMPB --> MF PALTEMPB RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + PaltempC,		User + Hwrei,	"01",		
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT PALTEMPC --> MF PALTEMPC RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + PaltempD,		User + Hwrei,	"01",		
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT PALTEMPD --> MF PALTEMPD RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + PaltempE,		User + Hwrei,	"01",		
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT PALTEMPE --> MF PALTEMPE RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + PaltempF,		User + Hwrei,	"01",		
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT PALTEMPF --> MF PALTEMPF RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + Paltemp10,		User + Hwrei,	"01",		
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT PALTEMP10 --> MF PALTEMP10 RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + Paltemp11,		User + Hwrei,	"01",		
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT PALTEMP11 --> MF PALTEMP11 RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + Paltemp12,		User + Hwrei,	"01",		
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT PALTEMP12 --> MF PALTEMP12 RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + Paltemp13,		User + Hwrei,	"01",		
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT PALTEMP13 --> MF PALTEMP13 RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + Paltemp14,		User + Hwrei,	"01",		
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT PALTEMP14 --> MF PALTEMP14 RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + Paltemp15,		User + Hwrei,	"01",		
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT PALTEMP15 --> MF PALTEMP15 RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + Paltemp16,		User + Hwrei,	"01",
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT PALTEMP16 --> MF PALTEMP16 RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + Paltemp17,		User + Hwrei,	"01",
	"WARNING - HWREI WITHIN 2 CYCLES OF MT IBOX IPR\n  POSSIBLE HIDDEN MT PALTEMP17 --> MF PALTEMP17 RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");


      add_restriction (Mt + Dc_test_ctl,	User + Hwrei,	"012",
	"WARNING - HWREI WITHIN 3 CYCLES OF MT DC_TEST_CTL\n  POSSIBLE HIDDEN MT DC_TEST_CTL--> MF DC_TEST_TAG RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + Mvptbr, 		User + Hwrei,	"01",
	"WARNING - HWREI WITHIN 2 CYCLES OF MT MVPTBR\n  POSSIBLE HIDDEN MT MVPTBR --> MF VA_FORM RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");


/* ADDED 16-FEB-93 */

      add_restriction (Mt + Astrr,		User + Hwrei_stall, "012",
	"WARNING - HWREI_STALL 3 CYCLES AFTER MT ASTRR\n  POSSIBLE HIDDEN MT ASTRR --> MF INTID RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + Aster,		User + Hwrei_stall, "012",
	"WARNING - HWREI_STALL 3 CYCLES AFTER MT ASTER\n  POSSIBLE HIDDEN MT ASTER --> MF INTID RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
      add_restriction (Mt + Sirr,		User + Hwrei_stall, "01",
	"WARNING - HWREI_STALL 2 CYCLES AFTER MT SIRR\n  POSSIBLE HIDDEN MT SIRR  --> MF INTID RESTRICTION\n--NOT A PROBLEM IN CALL_PALs");
#endif

}



static void special_case (INSTR *inst, int cycle)
/*
 ********************************************************************************
 *	PURPOSE:
 *	        To look at an instruction and perform a special check on it depending
 *	        on what instruction it is. Some restrictions didn't seem to
 *		fit in the restrictions[] paradigm.
 *	INPUT PARAMETERS:
 *	        inst: a single instruction
 *	        cycle: the cycle we're on, so we can index into the runtime[].
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	        the runtime array
 *	IMPLICIT OUTPUTS:
 *	        a message to output that there is a warning.
 *	FUNCTIONS CALLED DIRECTLY:
 *		is_ps_write
 *		new_warning
 *	AUTHOR/DATE:
 *	        Jeanne Meyer  1-Nov-1993
 ********************************************************************************
 */
{
      if (inst->index == DO_HW_REI) 
	    {
	    /* check for a write to a pal shadow register in the 0 or 1 cycle prior to HW_REI */
	    /* same cycle: */
	    if (runtime[cycle]->is1 != NULL) 
	    	{
	    	if (is_ps_write (runtime[cycle]->is1))
		    {
		    if (new_error (inst->address, 0)) 
		    	{
		     	wr (FLG$ERR, "%s\n", pheader);
		    	wr (FLG$ERR, "***\nError executing instruction %s at address %X on cycle %d!!\n", 
			  runtime[cycle]->is1->decoded, runtime[cycle]->is1->address, cycle);
		    	wr (FLG$ERR, "(PVC #???) Write to palshadow register -> HW_REI IN 0,1\n***\n");
		        pal_error_count++;
	   	    	}
		    } 
	    	if (runtime[cycle]->is2 != NULL) 
	    	    {
	    	    if (is_ps_write (runtime[cycle]->is2))
		    	{
		    	if (new_error (inst->address, 0)) 
			    	{
			     	wr (FLG$ERR, "%s\n", pheader);
			    	wr (FLG$ERR, "***\nError executing instruction %s at address %X on cycle %d!!\n", 
				  runtime[cycle]->is2->decoded, runtime[cycle]->is2->address, cycle);
			    	wr (FLG$ERR, "(PVC #???) Write to palshadow register -> HW_REI IN 0,1\n***\n");
			        pal_error_count++;
		   	    	}
		    	} 
	    	    if (runtime[cycle]->is3 != NULL) 
	    	    	{
	    	    	if (is_ps_write (runtime[cycle]->is3))
		    	    {
			    if (new_error (inst->address, 0)) 
			    	{
			     	wr (FLG$ERR, "%s\n", pheader);
			    	wr (FLG$ERR, "***\nError executing instruction %s at address %X on cycle %d!!\n", 
				  runtime[cycle]->is3->decoded, runtime[cycle]->is3->address, cycle);
			    	wr (FLG$ERR, "(PVC #???) Write to palshadow register -> HW_REI IN 0,1\n***\n");
			        pal_error_count++;
		   	    	}
		    	    } 
			    if (runtime[cycle]->is4 != NULL) 
	    	    	    {
	    	    		if (is_ps_write (runtime[cycle]->is4))
		    	    	{
			        if (new_error (inst->address, 0)) 
			    	    {
			     	    wr (FLG$ERR, "%s\n", pheader);
			    	    wr (FLG$ERR, "***\nError executing instruction %s at address %X on cycle %d!!\n", 
				      runtime[cycle]->is4->decoded, runtime[cycle]->is4->address, cycle);
			    	    wr (FLG$ERR, "(PVC #???) Write to palshadow register -> HW_REI IN 0,1\n***\n");
			            pal_error_count++;
	   	    	    	    }
		    	    	} 
			    }
			}
	    	    }
	    	}
	    /* previous cycle: */
	   if (cycle>0) 
	    {
	    if (runtime[cycle-1]->is1 != NULL) 
	    	{
	    	if (is_ps_write (runtime[cycle-1]->is1))
		    {
		    if (new_error (inst->address, 0)) 
		    	{
		     	wr (FLG$ERR, "%s\n", pheader);
		    	wr (FLG$ERR, "***\nError executing instruction %s at address %X on cycle %d!!\n", 
			  runtime[cycle-1]->is1->decoded, runtime[cycle-1]->is1->address, cycle-1);
		    	wr (FLG$ERR, "(PVC #???) Write to palshadow register -> HW_REI IN 0,1\n***\n");
		        pal_error_count++;
	   	    	}
		    } 
	    	if (runtime[cycle-1]->is2 != NULL) 
	    	    {
	    	    if (is_ps_write (runtime[cycle-1]->is2))
		    	{
		    	if (new_error (inst->address, 0)) 
			    	{
			     	wr (FLG$ERR, "%s\n", pheader);
			    	wr (FLG$ERR, "***\nError executing instruction %s at address %X on cycle %d!!\n", 
				  runtime[cycle-1]->is2->decoded, runtime[cycle-1]->is2->address, cycle-1);
			    	wr (FLG$ERR, "(PVC #???) Write to palshadow register -> HW_REI IN 0,1\n***\n");
			        pal_error_count++;
		   	    	}
		    	} 
	    	    if (runtime[cycle-1]->is3 != NULL) 
	    	    	{
	    	    	if (is_ps_write (runtime[cycle-1]->is3))
		    	    {
			    if (new_error (inst->address, 0)) 
			    	{
			     	wr (FLG$ERR, "%s\n", pheader);
			    	wr (FLG$ERR, "***\nError executing instruction %s at address %X on cycle %d!!\n", 
				  runtime[cycle-1]->is3->decoded, runtime[cycle-1]->is3->address, cycle-1);
			    	wr (FLG$ERR, "(PVC #???) Write to palshadow register -> HW_REI IN 0,1\n***\n");
			        pal_error_count++;
		   	    	}
		    	    } 
			    if (runtime[cycle-1]->is4 != NULL) 
	    	    	    {
	    	    		if (is_ps_write (runtime[cycle-1]->is4))
		    	    	{
			        if (new_error (inst->address, 0)) 
			    	    {
			     	    wr (FLG$ERR, "%s\n", pheader);
			    	    wr (FLG$ERR, "***\nError executing instruction %s at address %X on cycle %d!!\n", 
				      runtime[cycle-1]->is4->decoded, runtime[cycle-1]->is4->address, cycle-1);
			    	    wr (FLG$ERR, "(PVC #???) Write to palshadow register -> HW_REI IN 0,1\n***\n");
			            pal_error_count++;
	   	    	    	    }
		    	    	} 
			    }
			}
	    	    }
	    	}
	     }
	  }  
}


static int not_preceded_by_mbs (int cycle)
/*
 ********************************************************************************
 *	PURPOSE:
 *		This isn't used by any of the EV5 code yet.
 *	INPUT PARAMETERS:
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Greg Coladonato, June 92
 ********************************************************************************
 */
{
      if ((runtime[cycle-1] == NULL || runtime[cycle-2] == NULL) ||
	  (runtime[cycle-1]->is1 == NULL || runtime[cycle-2]->is1 == NULL) ||
	  (runtime[cycle-1]->is1->index != DO_MB || runtime[cycle-2]->is1->index != DO_MB)) 
	{
	      /* the case is not strictly mb, mb, mt pal_base. we have to look more in depth */
	      int temp, num_found = 0;
	      
	      temp = cycle - 1;
	      while (temp >= 0 && num_found < 2) 
		{
		      if (runtime[temp] && !runtime[temp]->is1) 
			{
			      /* this cycle is a stall. go to the one before it. */
			      temp--;
			      continue;
			}
		      else if (runtime[temp] && runtime[temp]->is1 && runtime[temp]->is1->index == DO_MB) 
			{
			      /* this cycle has an MB on it. */
			      num_found++;
			      temp--;
			      continue;
			}
		      else 
			{
			      /* this cycle has a random instruction. the test fails */
			      break;
			}
		}
	      if (num_found != 2) 
		{
		      /* we didn't find enough MB's in a row. */
		      return (TRUE);
		}
	      else 
		{
		      /* we found that at least two MBs immediately precede this instruction */
		      return (FALSE);
		}
	}
      else 
	{
	      /* this mt pal_base has two mb's right before it. */
	      return (FALSE);
	}
}



static void decode_ipr (disp_table *entry)
/*
 ********************************************************************************
 *	PURPOSE:
 *		Allow each chip to have its own function to decode IPRs.
 *		This is necessary because each implementation will have its
 *		own IPRs.
 *	INPUT PARAMETERS:
 *		entry: some wierd data structure
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Greg Coladonato, Aug 92
 ********************************************************************************
 */
{
      extern union INSTR_FORMAT instruction;
      extern char  *text;
      extern B64   *pc;
      
      sprintf(text,"%-12s", entry->text);
      
      sprintf(&text[strlen(text)],"%s, ",
	      &reg_name_table[instruction.op_format.ra][0]);
      
/* v3.24 - add check for ipr index gtr than max_ipr to avoid out of bounds*/
      if (instruction.hwm_format.func <= MAX_IPR)
          sprintf(&text[strlen(text)],"%s",
	          &ipr_name_table[instruction.hwm_format.func][0]);
      return;
}




static void decode_hw_memory (disp_table *entry)
/*
 ********************************************************************************
 *	PURPOSE:
 *		To allow each chip to have its own function for decoding 
 *		palcode memory reference instructions. This was necessary      
 *		because EV5 uses a "MMCheck" instead of "MODIFY"
 *	INPUT PARAMETERS:
 *		entry: some wierd data structure
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Greg Coladonato, Aug 92
 *	FLS Sep-96 added HW_REI decode 
 ********************************************************************************
 */
{
      extern union INSTR_FORMAT instruction;
      extern char  *text;
      extern B64   *pc;
      
      sprintf(text,"%-12s",entry->text);	/* store LD or ST or HW_REI */

      if (instruction.op_format.opcode != EVX$OPC_HW_REI) {

          sprintf(&text[strlen(text)],"%s, %d(%s)",
	      &reg_name_table[instruction.op_format.ra][0],
	      instruction.hwmem_format.disp,
	      &reg_name_table[instruction.op_format.rb][0]);
      
          if(instruction.hwmem_format.quad)	sprintf(&text[strlen(text)],",QUAD");
          else sprintf(&text[strlen(text)],",LONG");
      
          if(instruction.hwmem_format.physical) sprintf(&text[strlen(text)],",PHYSICAL");
          else sprintf(&text[strlen(text)],",VIRTUAL");
      
          if(instruction.hwmem_format.chk) sprintf(&text[strlen(text)],",MMCheck");
          if(instruction.hwmem_format.alt_mode) sprintf(&text[strlen(text)],",ALT_MODE");
      }
      return;    
}



static void init_pipe_use (void)
/*
 ********************************************************************************
 *	 PURPOSE:
 *	 	Initialize the table that is necessary to determine whether two   
 *	 	instruction types can be issued at the same time.
 *	 INPUT PARAMETERS:
 *	 OUTPUT PARAMETERS:
 *	 IMPLICIT INPUTS:
 *	 IMPLICIT OUTPUTS:
 *	 AUTHOR/DATE:
 *	 	Greg Coladonato, November 1991
 *********************************************************************************
 */
{
      issue_pipe[di_ld] = 	(ps_e0 | ps_e1);
      issue_pipe[di_ldx_l] =	(ps_e0);
      issue_pipe[di_st] =	(ps_e0);
      issue_pipe[di_mbx] =	(ps_e0);
      issue_pipe[di_mxpr] =	(ps_e0 | ps_e1);
      issue_pipe[di_ibr] =	(ps_e1);
      issue_pipe[di_fbr] =	(ps_fa);
      issue_pipe[di_jsr] =	(ps_e1);	
      issue_pipe[di_hwrei] =	(ps_e1);	
      issue_pipe[di_iaddlog] =	(ps_e0 | ps_e1);	
      issue_pipe[di_shift] =	(ps_e0);
      issue_pipe[di_cmov] =	(ps_e0 | ps_e1);
      issue_pipe[di_icmp] =	(ps_e0 | ps_e1);
      issue_pipe[di_imull] =	(ps_e0);
      issue_pipe[di_imulq] =	(ps_e0);
      issue_pipe[di_imulh] =	(ps_e0);
      issue_pipe[di_fadd] =	(ps_fa);
      issue_pipe[di_fdiv] =	(ps_fa);
      issue_pipe[di_fmul] =	(ps_fm);
      issue_pipe[di_fcpys] =	(ps_fa | ps_fm);
      issue_pipe[di_epicode]=	(ps_e1);
}




static void init_latencies (void)
/*
 ********************************************************************************
 *	 PURPOSE:
 *		Implment the result latencies of instructions on EV5.
 *		This is a somewhat simplified treatment, for example, the muls
 *		are dealt with as only one value.
 *	 INPUT PARAMETERS:
 *	 OUTPUT PARAMETERS:
 *	 IMPLICIT INPUTS:
 *	 IMPLICIT OUTPUTS:
 *	 AUTHOR/DATE:
 *	 	Greg Coladonato, August 1992
 ********************************************************************************
 */
{
      latency[di_none] =	0;
      latency[di_ld] = 		2;
      latency[di_ldx_l] =	2;
      latency[di_st] = 		0;
      latency[di_mbx] = 	2;
      latency[di_mxpr] =	1;
      latency[di_ibr] =		0;
      latency[di_fbr] =		0;
      latency[di_jsr] =		1;	
      latency[di_hwrei] =		1;	
      latency[di_iaddlog] =	1;	
      latency[di_shift] =	1;
      latency[di_cmov] =	2;
      latency[di_icmp] =	1;
      latency[di_imull] =	8;
      latency[di_imulq] =	12;
      latency[di_imulh] =	13;
      latency[di_fadd] =	4;
      latency[di_fdiv] =	10;
      latency[di_fmul] =	4;
      latency[di_fcpys] =	4;
      latency[di_epicode]=	1;
}



static void init_classes(void)
/*
 ********************************************************************************
 *	 PURPOSE:
 *	 	To initialize the array that allows us to characterize the behavior
 *	 	of different classes of instructions. There are two levels of 
 *		behavior that we're interested in --   
 *
 *			architectural - which arguments an instruction takes,
 *					which operand is the result, etc
 *			implementation - which instructions dual issue with one
 *				     	another, what the producer-consumer
 *					latencies are
 *
 *		This function initializes the latter.
 *
 *		Please note: the callocs done in this fuction are never freed
 *		It would have required an additional entry in the Chip 
 *		structure, and I didn't feel that it was necesary, since there 
 *		are only about 20 calloc's. The only drawback is that if you
 *		change from one chip to the other, the old implementation 
 *		specific class information will lie around in memory taking
 *		up space. As this won't be a common scenario, I let it slide.
 *	 INPUT PARAMETERS:
 *	 OUTPUT PARAMETERS:
 *	 IMPLICIT INPUTS:
 *		instr_array[], with a CLASS_INFO structure already allocated
 *			at each array entry
 *	 IMPLICIT OUTPUTS:
 *	 AUTHOR/DATE:
 *		Greg Coladonato, August 1992
 ********************************************************************************
 */
{
      struct implemented *imp;
      
      /* IADDLOG class  */
      
      imp = (struct implemented *) calloc(1, sizeof(struct implemented));

      imp->di_class = di_iaddlog;
      
      /* integer arithmetic */
      
      instr_array[DO_ADDL]->imp = imp;
      instr_array[DO_ADDLV]->imp = imp;
      instr_array[DO_ADDQ]->imp = imp;
      instr_array[DO_ADDQV]->imp = imp;
      instr_array[DO_SUBL]->imp = imp;
      instr_array[DO_SUBLV]->imp = imp;
      instr_array[DO_SUBQ]->imp = imp;
      instr_array[DO_SUBQV]->imp = imp;
      instr_array[DO_S4ADDL]->imp = imp;
      instr_array[DO_S4ADDQ]->imp = imp;
      instr_array[DO_S8ADDL]->imp = imp;
      instr_array[DO_S8ADDQ]->imp = imp;
      instr_array[DO_S4SUBL]->imp = imp;
      instr_array[DO_S4SUBQ]->imp = imp;
      instr_array[DO_S8SUBL]->imp = imp;
      instr_array[DO_S8SUBQ]->imp = imp;
      
      /* logicals */
      
      instr_array[DO_AND]->imp = imp;
      instr_array[DO_BIC]->imp = imp;
      instr_array[DO_EQV]->imp = imp;
      instr_array[DO_OR]->imp = imp;
      instr_array[DO_ORNOT]->imp = imp;
      instr_array[DO_XOR]->imp = imp;
      
      instr_array[DO_LDAH]->imp = imp;
      instr_array[DO_LDAL]->imp = imp;
      instr_array[DO_LDALH]->imp = imp;
      instr_array[DO_LDAQ]->imp = imp;
      
      /* ICMP class */
      
      imp = (struct implemented *) calloc(1, sizeof(struct implemented));

      imp->di_class = di_icmp;
      
      instr_array[DO_CMPB]->imp = imp;
      instr_array[DO_CMPBGE]->imp = imp;
      instr_array[DO_CMPEQ]->imp = imp;
      instr_array[DO_CMPLE]->imp = imp;
      instr_array[DO_CMPLT]->imp = imp;
      instr_array[DO_CMPNE]->imp = imp;
      instr_array[DO_CMPUGE]->imp = imp;
      instr_array[DO_CMPUGT]->imp = imp;
      instr_array[DO_CMPULE]->imp = imp;
      instr_array[DO_CMPULT]->imp = imp;
      
      /* CMOV class */
      
      imp = (struct implemented *) calloc(1, sizeof(struct implemented));

      imp->di_class = di_cmov;
      
      /* CMOVEx */
      
      instr_array[DO_CMOVEQ]->imp = imp;
      instr_array[DO_CMOVGE]->imp = imp;
      instr_array[DO_CMOVGT]->imp = imp;
      instr_array[DO_CMOVLBC]->imp = imp;
      instr_array[DO_CMOVLBS]->imp = imp;
      instr_array[DO_CMOVLE]->imp = imp;
      instr_array[DO_CMOVLT]->imp = imp;
      instr_array[DO_CMOVNE]->imp = imp;
      
      /* SHIFT class */
      
      imp = (struct implemented *) calloc(1, sizeof(struct implemented));
      
      imp->di_class = di_shift;
      
      /* bit field */
      
      instr_array[DO_EXTBH]->imp = imp;
      instr_array[DO_EXTBL]->imp = imp;
      instr_array[DO_EXTLH]->imp = imp;
      instr_array[DO_EXTLL]->imp = imp;
      instr_array[DO_EXTQH]->imp = imp;
      instr_array[DO_EXTQL]->imp = imp;
      instr_array[DO_EXTWH]->imp = imp;
      instr_array[DO_EXTWL]->imp = imp;
      instr_array[DO_INSBH]->imp = imp;
      instr_array[DO_INSBL]->imp = imp;
      instr_array[DO_INSLH]->imp = imp;
      instr_array[DO_INSLL]->imp = imp;
      instr_array[DO_INSQH]->imp = imp;
      instr_array[DO_INSQL]->imp = imp;
      instr_array[DO_INSWH]->imp = imp;
      instr_array[DO_INSWL]->imp = imp;
      instr_array[DO_MRGB]->imp = imp;
      instr_array[DO_ZAP]->imp = imp;
      instr_array[DO_MSKBH]->imp = imp;
      instr_array[DO_MSKBL]->imp = imp;
      instr_array[DO_MSKLH]->imp = imp;
      instr_array[DO_MSKLL]->imp = imp;
      instr_array[DO_ZAPNOT]->imp = imp;
      instr_array[DO_MSKQH]->imp = imp;
      instr_array[DO_MSKQL]->imp = imp;
      instr_array[DO_MSKWH]->imp = imp;
      instr_array[DO_MSKWL]->imp = imp;
      instr_array[DO_ROT]->imp = imp;
      instr_array[DO_SLL]->imp = imp;
      instr_array[DO_SRA]->imp = imp;
      instr_array[DO_SRL]->imp = imp;
      instr_array[DO_RC]->imp = imp;		/* RC,RS similar to shifts in latency & pipe */
      instr_array[DO_RS]->imp = imp;
      
      /* IMULL class */
      
      imp = (struct implemented *) calloc(1, sizeof(struct implemented));

      imp->di_class = di_imull;
      
      instr_array[DO_MULL]->imp = imp;
      instr_array[DO_MULLV]->imp = imp;
      
      /* IMULQ class */
      
      imp = (struct implemented *) calloc(1, sizeof(struct implemented));

      imp->di_class = di_imulq;
      
      instr_array[DO_MULQ]->imp = imp;
      instr_array[DO_MULQV]->imp = imp;

      /* IMULH class */
      
      imp = (struct implemented *) calloc(1, sizeof(struct implemented));

      imp->di_class = di_imulh;

      instr_array[DO_UMULH]->imp = imp;
      
      /* Normal floating point operates */
      
      imp = (struct implemented *)calloc(1, sizeof(struct implemented));

      imp->di_class = di_fadd;
      
      instr_array[DO_ADDD]->imp = imp;
      instr_array[DO_ADDDC]->imp = imp;
      instr_array[DO_ADDDS]->imp = imp;
      instr_array[DO_ADDDSC]->imp = imp;
      instr_array[DO_ADDDSU]->imp = imp;
      instr_array[DO_ADDDSUC]->imp = imp;
      instr_array[DO_ADDDU]->imp = imp;
      instr_array[DO_ADDDUC]->imp = imp;
      instr_array[DO_ADDF]->imp = imp;
      instr_array[DO_ADDFC]->imp = imp;
      instr_array[DO_ADDFS]->imp = imp;
      instr_array[DO_ADDFSC]->imp = imp;
      instr_array[DO_ADDFSU]->imp = imp;
      instr_array[DO_ADDFSUC]->imp = imp;
      instr_array[DO_ADDFU]->imp = imp;
      instr_array[DO_ADDFUC]->imp = imp;
      instr_array[DO_ADDG]->imp = imp;
      instr_array[DO_ADDGC]->imp = imp;
      instr_array[DO_ADDGS]->imp = imp;
      instr_array[DO_ADDGSC]->imp = imp;
      instr_array[DO_ADDGSU]->imp = imp;
      instr_array[DO_ADDGSUC]->imp = imp;
      instr_array[DO_ADDGU]->imp = imp;
      instr_array[DO_ADDGUC]->imp = imp;
      instr_array[DO_ADDS]->imp = imp;
      instr_array[DO_ADDSC]->imp = imp;
      instr_array[DO_ADDSM]->imp = imp;
      instr_array[DO_ADDSP]->imp = imp;
      instr_array[DO_ADDSSU]->imp = imp;
      instr_array[DO_ADDSSUC]->imp = imp;
      instr_array[DO_ADDSSUI]->imp = imp;
      instr_array[DO_ADDSSUIC]->imp = imp;
      instr_array[DO_ADDSSUIM]->imp = imp;
      instr_array[DO_ADDSSUIP]->imp = imp;
      instr_array[DO_ADDSSUM]->imp = imp;
      instr_array[DO_ADDSSUP]->imp = imp;
      instr_array[DO_ADDSU]->imp = imp;
      instr_array[DO_ADDSUC]->imp = imp;
      instr_array[DO_ADDSUM]->imp = imp;
      instr_array[DO_ADDSUP]->imp = imp;
      instr_array[DO_ADDT]->imp = imp;
      instr_array[DO_ADDTC]->imp = imp;
      instr_array[DO_ADDTM]->imp = imp;
      instr_array[DO_ADDTP]->imp = imp;
      instr_array[DO_ADDTSU]->imp = imp;
      instr_array[DO_ADDTSUC]->imp = imp;
      instr_array[DO_ADDTSUI]->imp = imp;
      instr_array[DO_ADDTSUIC]->imp = imp;
      instr_array[DO_ADDTSUIM]->imp = imp;
      instr_array[DO_ADDTSUIP]->imp = imp;
      instr_array[DO_ADDTSUM]->imp = imp;
      instr_array[DO_ADDTSUP]->imp = imp;
      instr_array[DO_ADDTU]->imp = imp;
      instr_array[DO_ADDTUC]->imp = imp;
      instr_array[DO_ADDTUM]->imp = imp;
      instr_array[DO_ADDTUP]->imp = imp;
      
      instr_array[DO_CMPDEQ]->imp = imp;
      instr_array[DO_CMPDEQS]->imp = imp;
      instr_array[DO_CMPDGE]->imp = imp;
      instr_array[DO_CMPDGES]->imp = imp;
      instr_array[DO_CMPDGT]->imp = imp;
      instr_array[DO_CMPDGTS]->imp = imp;
      instr_array[DO_CMPDLE]->imp = imp;
      instr_array[DO_CMPDLES]->imp = imp;
      instr_array[DO_CMPDLT]->imp = imp;
      instr_array[DO_CMPDLTS]->imp = imp;
      instr_array[DO_CMPDNE]->imp = imp;
      instr_array[DO_CMPDNES]->imp = imp;
      instr_array[DO_CMPFEQ]->imp = imp;
      instr_array[DO_CMPFEQS]->imp = imp;
      instr_array[DO_CMPFGE]->imp = imp;
      instr_array[DO_CMPFGES]->imp = imp;
      instr_array[DO_CMPFGT]->imp = imp;
      instr_array[DO_CMPFGTS]->imp = imp;
      instr_array[DO_CMPFLE]->imp = imp;
      instr_array[DO_CMPFLES]->imp = imp;
      instr_array[DO_CMPFLT]->imp = imp;
      instr_array[DO_CMPFLTS]->imp = imp;
      instr_array[DO_CMPFNE]->imp = imp;
      instr_array[DO_CMPFNES]->imp = imp;
      instr_array[DO_CMPGE]->imp = imp;
      instr_array[DO_CMPGEQ]->imp = imp;
      instr_array[DO_CMPGEQS]->imp = imp;
      instr_array[DO_CMPGGE]->imp = imp;
      instr_array[DO_CMPGGES]->imp = imp;
      instr_array[DO_CMPGGT]->imp = imp;
      instr_array[DO_CMPGGTS]->imp = imp;
      instr_array[DO_CMPGLE]->imp = imp;
      instr_array[DO_CMPGLES]->imp = imp;
      instr_array[DO_CMPGLT]->imp = imp;
      instr_array[DO_CMPGLTS]->imp = imp;
      instr_array[DO_CMPGNE]->imp = imp;
      instr_array[DO_CMPGNES]->imp = imp;
      instr_array[DO_CMPGT]->imp = imp;
      instr_array[DO_CMPSEQ]->imp = imp;
      instr_array[DO_CMPSEQSU]->imp = imp;
      instr_array[DO_CMPSLE]->imp = imp;
      instr_array[DO_CMPSLESU]->imp = imp;
      instr_array[DO_CMPSLT]->imp = imp;
      instr_array[DO_CMPSLTSU]->imp = imp;
      instr_array[DO_CMPSUN]->imp = imp;
      instr_array[DO_CMPSUNSU]->imp = imp;
      instr_array[DO_CMPTEQ]->imp = imp;
      instr_array[DO_CMPTEQSU]->imp = imp;
      instr_array[DO_CMPTLE]->imp = imp;
      instr_array[DO_CMPTLESU]->imp = imp;
      instr_array[DO_CMPTLT]->imp = imp;
      instr_array[DO_CMPTLTSU]->imp = imp;
      instr_array[DO_CMPTUN]->imp = imp;
      instr_array[DO_CMPTUNSU]->imp = imp;
      
      instr_array[DO_FCMOVEQ]->imp = imp;
      instr_array[DO_FCMOVGE]->imp = imp;
      instr_array[DO_FCMOVGT]->imp = imp;
      instr_array[DO_FCMOVLE]->imp = imp;
      instr_array[DO_FCMOVLT]->imp = imp;
      instr_array[DO_FCMOVNE]->imp = imp;
      
      instr_array[DO_SUBD]->imp = imp;
      instr_array[DO_SUBDC]->imp = imp;
      instr_array[DO_SUBDS]->imp = imp;
      instr_array[DO_SUBDSC]->imp = imp;
      instr_array[DO_SUBDSU]->imp = imp;
      instr_array[DO_SUBDSUC]->imp = imp;
      instr_array[DO_SUBDU]->imp = imp;
      instr_array[DO_SUBDUC]->imp = imp;
      instr_array[DO_SUBF]->imp = imp;
      instr_array[DO_SUBFC]->imp = imp;
      instr_array[DO_SUBFS]->imp = imp;
      instr_array[DO_SUBFSC]->imp = imp;
      instr_array[DO_SUBFSU]->imp = imp;
      instr_array[DO_SUBFSUC]->imp = imp;
      instr_array[DO_SUBFU]->imp = imp;
      instr_array[DO_SUBFUC]->imp = imp;
      instr_array[DO_SUBG]->imp = imp;
      instr_array[DO_SUBGC]->imp = imp;
      instr_array[DO_SUBGS]->imp = imp;
      instr_array[DO_SUBGSC]->imp = imp;
      instr_array[DO_SUBGSU]->imp = imp;
      instr_array[DO_SUBGSUC]->imp = imp;
      instr_array[DO_SUBGU]->imp = imp;
      instr_array[DO_SUBGUC]->imp = imp;
      instr_array[DO_SUBS]->imp = imp;
      instr_array[DO_SUBSC]->imp = imp;
      instr_array[DO_SUBSM]->imp = imp;
      instr_array[DO_SUBSP]->imp = imp;
      instr_array[DO_SUBSSU]->imp = imp;
      instr_array[DO_SUBSSUC]->imp = imp;
      instr_array[DO_SUBSSUI]->imp = imp;
      instr_array[DO_SUBSSUIC]->imp = imp;
      instr_array[DO_SUBSSUIM]->imp = imp;
      instr_array[DO_SUBSSUIP]->imp = imp;
      instr_array[DO_SUBSSUM]->imp = imp;
      instr_array[DO_SUBSSUP]->imp = imp;
      instr_array[DO_SUBSU]->imp = imp;
      instr_array[DO_SUBSUC]->imp = imp;
      instr_array[DO_SUBSUM]->imp = imp;
      instr_array[DO_SUBSUP]->imp = imp;
      instr_array[DO_SUBT]->imp = imp;
      instr_array[DO_SUBTC]->imp = imp;
      instr_array[DO_SUBTM]->imp = imp;
      instr_array[DO_SUBTP]->imp = imp;
      instr_array[DO_SUBTSU]->imp = imp;
      instr_array[DO_SUBTSUC]->imp = imp;
      instr_array[DO_SUBTSUI]->imp = imp;
      instr_array[DO_SUBTSUIC]->imp = imp;
      instr_array[DO_SUBTSUIM]->imp = imp;
      instr_array[DO_SUBTSUIP]->imp = imp;
      instr_array[DO_SUBTSUM]->imp = imp;
      instr_array[DO_SUBTSUP]->imp = imp;
      instr_array[DO_SUBTU]->imp = imp;
      instr_array[DO_SUBTUC]->imp = imp;
      instr_array[DO_SUBTUM]->imp = imp;
      instr_array[DO_SUBTUP]->imp = imp;
      
      instr_array[DO_CVTDF]->imp = imp;
      instr_array[DO_CVTDFC]->imp = imp;
      instr_array[DO_CVTDFS]->imp = imp;
      instr_array[DO_CVTDFSC]->imp = imp;
      instr_array[DO_CVTDFSU]->imp = imp;
      instr_array[DO_CVTDFSUC]->imp = imp;
      instr_array[DO_CVTDFU]->imp = imp;
      instr_array[DO_CVTDFUC]->imp = imp;
      instr_array[DO_CVTDL]->imp = imp;
      instr_array[DO_CVTDLC]->imp = imp;
      instr_array[DO_CVTDLS]->imp = imp;
      instr_array[DO_CVTDLSC]->imp = imp;
      instr_array[DO_CVTDLSV]->imp = imp;
      instr_array[DO_CVTDLSVC]->imp = imp;
      instr_array[DO_CVTDLV]->imp = imp;
      instr_array[DO_CVTDLVC]->imp = imp;
      instr_array[DO_CVTDQ]->imp = imp;
      instr_array[DO_CVTDQC]->imp = imp;
      instr_array[DO_CVTDQS]->imp = imp;
      instr_array[DO_CVTDQSC]->imp = imp;
      instr_array[DO_CVTDQSV]->imp = imp;
      instr_array[DO_CVTDQSVC]->imp = imp;
      instr_array[DO_CVTDQV]->imp = imp;
      instr_array[DO_CVTDQVC]->imp = imp;
      instr_array[DO_CVTFD]->imp = imp;
      instr_array[DO_CVTFDC]->imp = imp;
      instr_array[DO_CVTFDS]->imp = imp;
      instr_array[DO_CVTFDSC]->imp = imp;
      instr_array[DO_CVTFDSU]->imp = imp;
      instr_array[DO_CVTFDSUC]->imp = imp;
      instr_array[DO_CVTFDU]->imp = imp;
      instr_array[DO_CVTFDUC]->imp = imp;
      instr_array[DO_CVTFG]->imp = imp;
      instr_array[DO_CVTFGC]->imp = imp;
      instr_array[DO_CVTFGS]->imp = imp;
      instr_array[DO_CVTFGSC]->imp = imp;
      instr_array[DO_CVTFGSU]->imp = imp;
      instr_array[DO_CVTFGSUC]->imp = imp;
      instr_array[DO_CVTFGU]->imp = imp;
      instr_array[DO_CVTFGUC]->imp = imp;
      instr_array[DO_CVTFL]->imp = imp;
      instr_array[DO_CVTFLC]->imp = imp;
      instr_array[DO_CVTFLS]->imp = imp;
      instr_array[DO_CVTFLSC]->imp = imp;
      instr_array[DO_CVTFLSV]->imp = imp;
      instr_array[DO_CVTFLSVC]->imp = imp;
      instr_array[DO_CVTFLV]->imp = imp;
      instr_array[DO_CVTFLVC]->imp = imp;
      instr_array[DO_CVTFQ]->imp = imp;
      instr_array[DO_CVTFQC]->imp = imp;
      instr_array[DO_CVTFQS]->imp = imp;
      instr_array[DO_CVTFQSC]->imp = imp;
      instr_array[DO_CVTFQSV]->imp = imp;
      instr_array[DO_CVTFQSVC]->imp = imp;
      instr_array[DO_CVTFQV]->imp = imp;
      instr_array[DO_CVTFQVC]->imp = imp;
      instr_array[DO_CVTGF]->imp = imp;
      instr_array[DO_CVTGFC]->imp = imp;
      instr_array[DO_CVTGFS]->imp = imp;
      instr_array[DO_CVTGFSC]->imp = imp;
      instr_array[DO_CVTGFSU]->imp = imp;
      instr_array[DO_CVTGFSUC]->imp = imp;
      instr_array[DO_CVTGFU]->imp = imp;
      instr_array[DO_CVTGFUC]->imp = imp;
      instr_array[DO_CVTGL]->imp = imp;
      instr_array[DO_CVTGLC]->imp = imp;
      instr_array[DO_CVTGLS]->imp = imp;
      instr_array[DO_CVTGLSC]->imp = imp;
      instr_array[DO_CVTGLSV]->imp = imp;
      instr_array[DO_CVTGLSVC]->imp = imp;
      instr_array[DO_CVTGLV]->imp = imp;
      instr_array[DO_CVTGLVC]->imp = imp;
      instr_array[DO_CVTGQ]->imp = imp;
      instr_array[DO_CVTGQC]->imp = imp;
      instr_array[DO_CVTGQS]->imp = imp;
      instr_array[DO_CVTGQSC]->imp = imp;
      instr_array[DO_CVTGQSV]->imp = imp;
      instr_array[DO_CVTGQSVC]->imp = imp;
      instr_array[DO_CVTGQV]->imp = imp;
      instr_array[DO_CVTGQVC]->imp = imp;
      instr_array[DO_CVTLD]->imp = imp;
      instr_array[DO_CVTLDC]->imp = imp;
      instr_array[DO_CVTLDS]->imp = imp;
      instr_array[DO_CVTLDSC]->imp = imp;
      instr_array[DO_CVTLF]->imp = imp;
      instr_array[DO_CVTLFC]->imp = imp;
      instr_array[DO_CVTLFS]->imp = imp;
      instr_array[DO_CVTLFSC]->imp = imp;
      instr_array[DO_CVTLG]->imp = imp;
      instr_array[DO_CVTLGC]->imp = imp;
      instr_array[DO_CVTLGS]->imp = imp;
      instr_array[DO_CVTLGSC]->imp = imp;
      instr_array[DO_CVTLQ]->imp = imp;
      instr_array[DO_CVTLS]->imp = imp;
      instr_array[DO_CVTLSC]->imp = imp;
      instr_array[DO_CVTLSM]->imp = imp;
      instr_array[DO_CVTLSP]->imp = imp;
      instr_array[DO_CVTLSSU]->imp = imp;
      instr_array[DO_CVTLSSUC]->imp = imp;
      instr_array[DO_CVTLSSUI]->imp = imp;
      instr_array[DO_CVTLSSUIC]->imp = imp;
      instr_array[DO_CVTLSSUIM]->imp = imp;
      instr_array[DO_CVTLSSUIP]->imp = imp;
      instr_array[DO_CVTLSSUM]->imp = imp;
      instr_array[DO_CVTLSSUP]->imp = imp;
      instr_array[DO_CVTLT]->imp = imp;
      instr_array[DO_CVTLTC]->imp = imp;
      instr_array[DO_CVTLTM]->imp = imp;
      instr_array[DO_CVTLTP]->imp = imp;
      instr_array[DO_CVTLTSU]->imp = imp;
      instr_array[DO_CVTLTSUC]->imp = imp;
      instr_array[DO_CVTLTSUI]->imp = imp;
      instr_array[DO_CVTLTSUIC]->imp = imp;
      instr_array[DO_CVTLTSUIM]->imp = imp;
      instr_array[DO_CVTLTSUIP]->imp = imp;
      instr_array[DO_CVTLTSUM]->imp = imp;
      instr_array[DO_CVTLTSUP]->imp = imp;
      instr_array[DO_CVTQD]->imp = imp;
      instr_array[DO_CVTQDC]->imp = imp;
      instr_array[DO_CVTQDS]->imp = imp;
      instr_array[DO_CVTQDSC]->imp = imp;
      instr_array[DO_CVTQF]->imp = imp;
      instr_array[DO_CVTQFC]->imp = imp;
      instr_array[DO_CVTQFS]->imp = imp;
      instr_array[DO_CVTQFSC]->imp = imp;
      instr_array[DO_CVTQG]->imp = imp;
      instr_array[DO_CVTQGC]->imp = imp;
      instr_array[DO_CVTQGS]->imp = imp;
      instr_array[DO_CVTQGSC]->imp = imp;
      instr_array[DO_CVTQL]->imp = imp;
      instr_array[DO_CVTQLV]->imp = imp;
      instr_array[DO_CVTQLSV]->imp = imp;
      instr_array[DO_CVTQS]->imp = imp;
      instr_array[DO_CVTQSC]->imp = imp;
      instr_array[DO_CVTQSM]->imp = imp;
      instr_array[DO_CVTQSP]->imp = imp;
      instr_array[DO_CVTQSSU]->imp = imp;
      instr_array[DO_CVTQSSUC]->imp = imp;
      instr_array[DO_CVTQSSUI]->imp = imp;
      instr_array[DO_CVTQSSUIC]->imp = imp;
      instr_array[DO_CVTQSSUIM]->imp = imp;
      instr_array[DO_CVTQSSUIP]->imp = imp;
      instr_array[DO_CVTQSSUM]->imp = imp;
      instr_array[DO_CVTQSSUP]->imp = imp;
      instr_array[DO_CVTQT]->imp = imp;
      instr_array[DO_CVTQTC]->imp = imp;
      instr_array[DO_CVTQTM]->imp = imp;
      instr_array[DO_CVTQTP]->imp = imp;
      instr_array[DO_CVTQTSU]->imp = imp;
      instr_array[DO_CVTQTSUC]->imp = imp;
      instr_array[DO_CVTQTSUI]->imp = imp;
      instr_array[DO_CVTQTSUIC]->imp = imp;
      instr_array[DO_CVTQTSUIM]->imp = imp;
      instr_array[DO_CVTQTSUIP]->imp = imp;
      instr_array[DO_CVTQTSUM]->imp = imp;
      instr_array[DO_CVTQTSUP]->imp = imp;
      instr_array[DO_CVTSL]->imp = imp;
      instr_array[DO_CVTSLC]->imp = imp;
      instr_array[DO_CVTSLSV]->imp = imp;
      instr_array[DO_CVTSLSVC]->imp = imp;
      instr_array[DO_CVTSLSVI]->imp = imp;
      instr_array[DO_CVTSLSVIC]->imp = imp;
      instr_array[DO_CVTSLV]->imp = imp;
      instr_array[DO_CVTSLVC]->imp = imp;
      instr_array[DO_CVTSQ]->imp = imp;
      instr_array[DO_CVTSQC]->imp = imp;
      instr_array[DO_CVTSQSV]->imp = imp;
      instr_array[DO_CVTSQSVC]->imp = imp;
      instr_array[DO_CVTSQSVI]->imp = imp;
      instr_array[DO_CVTSQSVIC]->imp = imp;
      instr_array[DO_CVTSQV]->imp = imp;
      instr_array[DO_CVTSQVC]->imp = imp;
      instr_array[DO_CVTST]->imp = imp;
      instr_array[DO_CVTSTC]->imp = imp;
      instr_array[DO_CVTSTM]->imp = imp;
      instr_array[DO_CVTSTP]->imp = imp;
      instr_array[DO_CVTSTSU]->imp = imp;
      instr_array[DO_CVTSTSUC]->imp = imp;
      instr_array[DO_CVTSTSUI]->imp = imp;
      instr_array[DO_CVTSTSUIC]->imp = imp;
      instr_array[DO_CVTSTSUIM]->imp = imp;
      instr_array[DO_CVTSTSUIP]->imp = imp;
      instr_array[DO_CVTSTSUM]->imp = imp;
      instr_array[DO_CVTSTSUP]->imp = imp;
      instr_array[DO_CVTSTU]->imp = imp;
      instr_array[DO_CVTSTUC]->imp = imp;
      instr_array[DO_CVTSTUM]->imp = imp;
      instr_array[DO_CVTSTUP]->imp = imp;
      instr_array[DO_CVTTL]->imp = imp;
      instr_array[DO_CVTTLC]->imp = imp;
      instr_array[DO_CVTTLSV]->imp = imp;
      instr_array[DO_CVTTLSVC]->imp = imp;
      instr_array[DO_CVTTLSVI]->imp = imp;
      instr_array[DO_CVTTLSVIC]->imp = imp;
      instr_array[DO_CVTTLV]->imp = imp;
      instr_array[DO_CVTTLVC]->imp = imp;
      instr_array[DO_CVTTQ]->imp = imp;
      instr_array[DO_CVTTQC]->imp = imp;
      instr_array[DO_CVTTQSV]->imp = imp;
      instr_array[DO_CVTTQSVC]->imp = imp;
      instr_array[DO_CVTTQSVI]->imp = imp;
      instr_array[DO_CVTTQSVIC]->imp = imp;
      instr_array[DO_CVTTQV]->imp = imp;
      instr_array[DO_CVTTQVC]->imp = imp;
      instr_array[DO_CVTTS]->imp = imp;
      instr_array[DO_CVTTSC]->imp = imp;
      instr_array[DO_CVTTSM]->imp = imp;
      instr_array[DO_CVTTSP]->imp = imp;
      instr_array[DO_CVTTSSU]->imp = imp;
      instr_array[DO_CVTTSSUC]->imp = imp;
      instr_array[DO_CVTTSSUI]->imp = imp;
      instr_array[DO_CVTTSSUIC]->imp = imp;
      instr_array[DO_CVTTSSUIM]->imp = imp;
      instr_array[DO_CVTTSSUIP]->imp = imp;
      instr_array[DO_CVTTSSUM]->imp = imp;
      instr_array[DO_CVTTSSUP]->imp = imp;
      instr_array[DO_CVTTSU]->imp = imp;
      instr_array[DO_CVTTSUC]->imp = imp;
      instr_array[DO_CVTTSUM]->imp = imp;
      instr_array[DO_CVTTSUP]->imp = imp;
      instr_array[DO_CVTDG]->imp = imp;
      instr_array[DO_CVTDGC]->imp = imp;
      instr_array[DO_CVTDGU]->imp = imp;
      instr_array[DO_CVTDGUC]->imp = imp;
      instr_array[DO_CVTGD]->imp = imp;
      instr_array[DO_RD_FPCTL]->imp = imp;
      instr_array[DO_WR_FPCTL]->imp = imp;
      
      instr_array[DO_CPYSE]->imp = imp;
      instr_array[DO_CPYSEE]->imp = imp;
      instr_array[DO_CPYSN]->imp = imp;
      
      /* FDIV (2 operands & one dest) */
      
      imp = (struct implemented *)calloc(1, sizeof(struct implemented));

      imp->di_class = di_fdiv;
      
      instr_array[DO_DIV]->imp = imp;
      instr_array[DO_DIVD]->imp = imp;
      instr_array[DO_DIVDC]->imp = imp;
      instr_array[DO_DIVDS]->imp = imp;
      instr_array[DO_DIVDSC]->imp = imp;
      instr_array[DO_DIVDSU]->imp = imp;
      instr_array[DO_DIVDSUC]->imp = imp;
      instr_array[DO_DIVDU]->imp = imp;
      instr_array[DO_DIVDUC]->imp = imp;
      instr_array[DO_DIVF]->imp = imp;
      instr_array[DO_DIVFC]->imp = imp;
      instr_array[DO_DIVFS]->imp = imp;
      instr_array[DO_DIVFSC]->imp = imp;
      instr_array[DO_DIVFSU]->imp = imp;
      instr_array[DO_DIVFSUC]->imp = imp;
      instr_array[DO_DIVFU]->imp = imp;
      instr_array[DO_DIVFUC]->imp = imp;
      instr_array[DO_DIVG]->imp = imp;
      instr_array[DO_DIVGC]->imp = imp;
      instr_array[DO_DIVGS]->imp = imp;
      instr_array[DO_DIVGSC]->imp = imp;
      instr_array[DO_DIVGSU]->imp = imp;
      instr_array[DO_DIVGSUC]->imp = imp;
      instr_array[DO_DIVGU]->imp = imp;
      instr_array[DO_DIVGUC]->imp = imp;
      instr_array[DO_DIVS]->imp = imp;
      instr_array[DO_DIVSC]->imp = imp;
      instr_array[DO_DIVSM]->imp = imp;
      instr_array[DO_DIVSP]->imp = imp;
      instr_array[DO_DIVSSU]->imp = imp;
      instr_array[DO_DIVSSUC]->imp = imp;
      instr_array[DO_DIVSSUI]->imp = imp;
      instr_array[DO_DIVSSUIC]->imp = imp;
      instr_array[DO_DIVSSUIM]->imp = imp;
      instr_array[DO_DIVSSUIP]->imp = imp;
      instr_array[DO_DIVSSUM]->imp = imp;
      instr_array[DO_DIVSSUP]->imp = imp;
      instr_array[DO_DIVSU]->imp = imp;
      instr_array[DO_DIVSUC]->imp = imp;
      instr_array[DO_DIVSUM]->imp = imp;
      instr_array[DO_DIVSUP]->imp = imp;
      instr_array[DO_DIVT]->imp = imp;
      instr_array[DO_DIVTC]->imp = imp;
      instr_array[DO_DIVTM]->imp = imp;
      instr_array[DO_DIVTP]->imp = imp;
      instr_array[DO_DIVTSU]->imp = imp;
      instr_array[DO_DIVTSUC]->imp = imp;
      instr_array[DO_DIVTSUI]->imp = imp;
      instr_array[DO_DIVTSUIC]->imp = imp;
      instr_array[DO_DIVTSUIM]->imp = imp;
      instr_array[DO_DIVTSUIP]->imp = imp;
      instr_array[DO_DIVTSUM]->imp = imp;
      instr_array[DO_DIVTSUP]->imp = imp;
      instr_array[DO_DIVTU]->imp = imp;
      instr_array[DO_DIVTUC]->imp = imp;
      instr_array[DO_DIVTUM]->imp = imp;
      instr_array[DO_DIVTUP]->imp = imp;
      instr_array[DO_DIVV]->imp = imp;
      
      /* FMUL class*/
      
      imp = (struct implemented *)calloc(1, sizeof(struct implemented));

      imp->di_class = di_fmul;
      
      instr_array[DO_MULD]->imp = imp;
      instr_array[DO_MULDC]->imp = imp;
      instr_array[DO_MULDS]->imp = imp;
      instr_array[DO_MULDSC]->imp = imp;
      instr_array[DO_MULDSU]->imp = imp;
      instr_array[DO_MULDSUC]->imp = imp;
      instr_array[DO_MULDU]->imp = imp;
      instr_array[DO_MULDUC]->imp = imp;
      instr_array[DO_MULF]->imp = imp;
      instr_array[DO_MULFC]->imp = imp;
      instr_array[DO_MULFS]->imp = imp;
      instr_array[DO_MULFSC]->imp = imp;
      instr_array[DO_MULFSU]->imp = imp;
      instr_array[DO_MULFSUC]->imp = imp;
      instr_array[DO_MULFU]->imp = imp;
      instr_array[DO_MULFUC]->imp = imp;
      instr_array[DO_MULG]->imp = imp;
      instr_array[DO_MULGC]->imp = imp;
      instr_array[DO_MULGS]->imp = imp;
      instr_array[DO_MULGSC]->imp = imp;
      instr_array[DO_MULGSU]->imp = imp;
      instr_array[DO_MULGSUC]->imp = imp;
      instr_array[DO_MULGU]->imp = imp;
      instr_array[DO_MULGUC]->imp = imp;
      instr_array[DO_MULS]->imp = imp;
      instr_array[DO_MULSC]->imp = imp;
      instr_array[DO_MULSM]->imp = imp;
      instr_array[DO_MULSP]->imp = imp;
      instr_array[DO_MULSSU]->imp = imp;
      instr_array[DO_MULSSUC]->imp = imp;
      instr_array[DO_MULSSUI]->imp = imp;
      instr_array[DO_MULSSUIC]->imp = imp;
      instr_array[DO_MULSSUIM]->imp = imp;
      instr_array[DO_MULSSUIP]->imp = imp;
      instr_array[DO_MULSSUM]->imp = imp;
      instr_array[DO_MULSSUP]->imp = imp;
      instr_array[DO_MULSU]->imp = imp;
      instr_array[DO_MULSUC]->imp = imp;
      instr_array[DO_MULSUM]->imp = imp;
      instr_array[DO_MULSUP]->imp = imp;
      instr_array[DO_MULT]->imp = imp;
      instr_array[DO_MULTC]->imp = imp;
      instr_array[DO_MULTM]->imp = imp;
      instr_array[DO_MULTP]->imp = imp;
      instr_array[DO_MULTSU]->imp = imp;
      instr_array[DO_MULTSUC]->imp = imp;
      instr_array[DO_MULTSUI]->imp = imp;
      instr_array[DO_MULTSUIC]->imp = imp;
      instr_array[DO_MULTSUIM]->imp = imp;
      instr_array[DO_MULTSUIP]->imp = imp;
      instr_array[DO_MULTSUM]->imp = imp;
      instr_array[DO_MULTSUP]->imp = imp;
      instr_array[DO_MULTU]->imp = imp;
      instr_array[DO_MULTUC]->imp = imp;
      instr_array[DO_MULTUM]->imp = imp;
      instr_array[DO_MULTUP]->imp = imp;
      
      /* FCPYS class*/
      
      imp = (struct implemented *)calloc(1, sizeof(struct implemented));

      imp->di_class = di_fcpys;
      
      instr_array[DO_CPYS]->imp = imp;
      
      /* MXPR class */
      
      imp =  (struct implemented *)calloc(1, sizeof(struct implemented));

      imp->di_class = di_mxpr;
      
      instr_array[DO_HW_MFPR]->imp = imp;
      instr_array[DO_HW_MTPR]->imp = imp;

      /* MBX class */
      
      imp =  (struct implemented *)calloc(1, sizeof(struct implemented));

      imp->di_class = di_mbx;
/* normally, ldx_l, stx_c and mb's would fit in this class, but current slot routine handles incorrectly */      
      
      /* LD class */
      
      imp = (struct implemented *)calloc(1, sizeof(struct implemented));

      imp->di_class = di_ld;
      
      instr_array[DO_HW_LD]->imp = imp;
      instr_array[DO_LDL]->imp = imp;
      instr_array[DO_LDQ]->imp = imp;
      instr_array[DO_LDQU]->imp = imp;
      instr_array[DO_LDS]->imp = imp;
      instr_array[DO_LDT]->imp = imp;
      instr_array[DO_LDD]->imp = imp;
      instr_array[DO_LDF]->imp = imp;
      
      /* ST class */
      
      imp = (struct implemented *)calloc(1, sizeof(struct implemented));

      imp->di_class = di_st;
      
      instr_array[DO_HW_ST]->imp = imp;
      instr_array[DO_STL]->imp = imp;
      instr_array[DO_STQ]->imp = imp;
      instr_array[DO_STQU]->imp = imp;
      instr_array[DO_STD]->imp = imp;
      instr_array[DO_STF]->imp = imp;
      instr_array[DO_STS]->imp = imp;
      instr_array[DO_STT]->imp = imp;
      instr_array[DO_STLC]->imp = imp;
      instr_array[DO_STQC]->imp = imp;
      instr_array[DO_MB]->imp = imp;
      instr_array[DO_LDLL]->imp = imp;  /* ldx_l is treated like a mb */
      instr_array[DO_LDQL]->imp = imp;
      
      /* One Integer operand conditional branches */
      
      imp =  (struct implemented *)calloc(1, sizeof(struct implemented));

      imp->di_class = di_ibr;
      
      instr_array[DO_BEQ]->imp = imp;
      instr_array[DO_BGE]->imp = imp;
      instr_array[DO_BGT]->imp = imp;
      instr_array[DO_BLBC]->imp = imp;
      instr_array[DO_BLBS]->imp = imp;
      instr_array[DO_BLE]->imp = imp;
      instr_array[DO_BLT]->imp = imp;
      instr_array[DO_BNE]->imp = imp;
      
      /* one float operand conditional branches */
      
      imp =  (struct implemented *) calloc(1, sizeof(struct implemented));

      imp->di_class = di_fbr;
      
      instr_array[DO_BFEQ]->imp = imp;
      instr_array[DO_BFGE]->imp = imp;
      instr_array[DO_BFGT]->imp = imp;
      instr_array[DO_BFLE]->imp = imp;
      instr_array[DO_BFLT]->imp = imp;
      instr_array[DO_BFNE]->imp = imp;
      instr_array[DO_FLBC]->imp = imp;
      
      /* JSR class */
      
      imp =  (struct implemented *)calloc(1, sizeof(struct implemented));

      imp->di_class = di_jsr;
      
      instr_array[DO_BR]->imp = imp;
      instr_array[DO_BSR]->imp = imp;
      instr_array[DO_JSR]->imp = imp;

      /* CALL_PAL class (aka epicode) */
      
      imp =  (struct implemented *)calloc(1, sizeof(struct implemented));

      imp->di_class = di_epicode;
      
      instr_array[DO_EPICODE]->imp = imp;

      /* HWREI class */
      
      imp =  (struct implemented *)calloc(1, sizeof(struct implemented));

      imp->di_class = di_hwrei;
      
      instr_array[DO_HW_REI]->imp = imp;
      
      /* LDX_L class */
      
      imp =  (struct implemented *)calloc(1, sizeof(struct implemented));

      imp->di_class = di_ldx_l;
      
      instr_array[DO_DRAINT]->imp = imp;
      instr_array[DO_RCC]->imp = imp;
      instr_array[DO_NUDGE]->imp = imp;
      instr_array[DO_FETCH]->imp = imp;
      instr_array[DO_FETCH_M]->imp = imp;

}



static void init_decode_tables (void)
/*
 ********************************************************************************
 *	PURPOSE:
 *		Since each chip has its own iprs, each chip must be able to
 *		initialize the decode table with the approprite names
 *	INPUT PARAMETERS:
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *		char ipr_name_table[][16]
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Greg Coladonato, Aug 92
 ********************************************************************************
 */

{
      /* load all unusual names for regs here */

      strcpy(ipr_name_table[IPR_ISR],"ISR");			/* (0x100) */
      strcpy(ipr_name_table[IPR_ITB_TAG],"ITB_TAG");		/* (0x101) */
      strcpy(ipr_name_table[IPR_ITB_PTE],"ITB_PTE");		/* (0x102) */
      strcpy(ipr_name_table[IPR_ITB_ASN],"ITB_ASN");		/* (0x103) */
      strcpy(ipr_name_table[IPR_ITB_PTE_TEMP],"ITB_PTE_TEMP");	/* (0x104) */
      strcpy(ipr_name_table[IPR_ITBIA],"ITBIA");		/* (0x105) */
      strcpy(ipr_name_table[IPR_ITBIAP],"ITBIAP");		/* (0x106) */
      strcpy(ipr_name_table[IPR_ITBIS],"ITBIS");		/* (0x107) */
      strcpy(ipr_name_table[IPR_SIRR],"SIRR");			/* (0x108) */
      strcpy(ipr_name_table[IPR_ASTRR],"ASTRR");		/* (0x109) */
      strcpy(ipr_name_table[IPR_ASTER],"ASTER");		/* (0x10A) */
      strcpy(ipr_name_table[IPR_EXC_ADDR],"EXC_ADDR");		/* (0x10B) */
      strcpy(ipr_name_table[IPR_EXC_SUM],"EXC_SUM");		/* (0x10C) */
      strcpy(ipr_name_table[IPR_EXC_MASK],"EXC_MASK");		/* (0x10D) */
      strcpy(ipr_name_table[IPR_PAL_BASE],"PAL_BASE");		/* (0x10E) */
      strcpy(ipr_name_table[IPR_PS],"PS");			/* (0x10F) */
      strcpy(ipr_name_table[IPR_IPL],"IPL");			/* (0x110) */
      strcpy(ipr_name_table[IPR_INTID],"INTID");		/* (0x111) */
      strcpy(ipr_name_table[IPR_IFAULT_VA_FORM],"IFAULT_VA_FORM");/* (0x112) */
      strcpy(ipr_name_table[IPR_IVPTBR],"IVPTBR");		/* (0x113) */
      strcpy(ipr_name_table[IPR_HWINT_CLR],"HWINT_CLR");	/* (0x115) */
      strcpy(ipr_name_table[IPR_SL_XMIT],"SL_XMIT");		/* (0x116) */
      strcpy(ipr_name_table[IPR_SL_RCV],"SL_RCV");		/* (0x117) */
      strcpy(ipr_name_table[IPR_ICSR],"ICSR");			/* (0x118) */
      strcpy(ipr_name_table[IPR_IC_FLUSH],"IC_FLUSH");		/* (0x119) */
      strcpy(ipr_name_table[IPR_IC_PERR_STAT],"IPERR_STAT");	/* (0x11A) */
      strcpy(ipr_name_table[IPR_PMCTR],"PMCTR");		/* (0x11C) */
      strcpy(ipr_name_table[IPR_PALTEMP0],"PALTEMP[0]");	/* (0x140) */
      strcpy(ipr_name_table[IPR_PALTEMP1],"PALTEMP[1]");	/* (0x141) */
      strcpy(ipr_name_table[IPR_PALTEMP2],"PALTEMP[2]");	/* (0x142) */
      strcpy(ipr_name_table[IPR_PALTEMP3],"PALTEMP[3]");	/* (0x143) */
      strcpy(ipr_name_table[IPR_PALTEMP4],"PALTEMP[4]");	/* (0x144) */
      strcpy(ipr_name_table[IPR_PALTEMP5],"PALTEMP[5]");	/* (0x145) */
      strcpy(ipr_name_table[IPR_PALTEMP6],"PALTEMP[6]");	/* (0x146) */
      strcpy(ipr_name_table[IPR_PALTEMP7],"PALTEMP[7]");	/* (0x147) */
      strcpy(ipr_name_table[IPR_PALTEMP8],"PALTEMP[8]");	/* (0x148) */
      strcpy(ipr_name_table[IPR_PALTEMP9],"PALTEMP[9]");	/* (0x149) */
      strcpy(ipr_name_table[IPR_PALTEMPA],"PALTEMP[A]");	/* (0x14A) */
      strcpy(ipr_name_table[IPR_PALTEMPB],"PALTEMP[B]");	/* (0x14B) */
      strcpy(ipr_name_table[IPR_PALTEMPC],"PALTEMP[C]");	/* (0x14C) */
      strcpy(ipr_name_table[IPR_PALTEMPD],"PALTEMP[D]");	/* (0x14D) */
      strcpy(ipr_name_table[IPR_PALTEMPE],"PALTEMP[E]");	/* (0x14E) */
      strcpy(ipr_name_table[IPR_PALTEMPF],"PALTEMP[F]");	/* (0x14F) */
      strcpy(ipr_name_table[IPR_PALTEMP10],"PALTEMP[10]");	/* (0x150) */
      strcpy(ipr_name_table[IPR_PALTEMP11],"PALTEMP[11]");	/* (0x151) */
      strcpy(ipr_name_table[IPR_PALTEMP12],"PALTEMP[12]");	/* (0x152) */
      strcpy(ipr_name_table[IPR_PALTEMP13],"PALTEMP[13]");	/* (0x153) */
      strcpy(ipr_name_table[IPR_PALTEMP14],"PALTEMP[14]");	/* (0x154) */
      strcpy(ipr_name_table[IPR_PALTEMP15],"PALTEMP[15]");	/* (0x155) */
      strcpy(ipr_name_table[IPR_PALTEMP16],"PALTEMP[16]");	/* (0x156) */
      strcpy(ipr_name_table[IPR_PALTEMP17],"PALTEMP[17]");	/* (0x157) */
      strcpy(ipr_name_table[IPR_DTB_ASN],"DTB_ASN");		/* (0x200) */
      strcpy(ipr_name_table[IPR_DTB_CM],"DTB_CM");		/* (0x201) */
      strcpy(ipr_name_table[IPR_DTB_TAG],"DTB_TAG");		/* (0x202) */
      strcpy(ipr_name_table[IPR_DTB_PTE],"DTB_PTE");		/* (0x203) */
      strcpy(ipr_name_table[IPR_DTB_PTE_TEMP],"DTB_PTE_TEMP");	/* (0x204) */
      strcpy(ipr_name_table[IPR_MM_STAT],"MM_STAT");		/* (0x205) */
      strcpy(ipr_name_table[IPR_VA],"VA");			/* (0x206) */
      strcpy(ipr_name_table[IPR_VA_FORM],"VA_FORM");		/* (0x207) */
      strcpy(ipr_name_table[IPR_MVPTBR],"MVPTBR");		/* (0x208) */
      strcpy(ipr_name_table[IPR_DTBIAP],"DTBIAP");		/* (0x209) */
      strcpy(ipr_name_table[IPR_DTBIA],"DTBIA");		/* (0x20A) */
      strcpy(ipr_name_table[IPR_DTBIS],"DTBIS");		/* (0x20B) */
      strcpy(ipr_name_table[IPR_ALT_MODE],"ALT_MODE");		/* (0x20C) */
      strcpy(ipr_name_table[IPR_CC],"CC");			/* (0x20D) */
      strcpy(ipr_name_table[IPR_CC_CTL],"CC_CTL");		/* (0x20E) */
      strcpy(ipr_name_table[IPR_MCSR],"MCSR");			/* (0x20F) */
      strcpy(ipr_name_table[IPR_DC_FLUSH],"DC_FLUSH");		/* (0x210) */
      strcpy(ipr_name_table[IPR_DC_PERR_STAT],"PERR_STAT");	/* (0x212) */
      strcpy(ipr_name_table[IPR_DC_TEST_CTL],"DC_TEST_CTL");	/* (0x213) */
      strcpy(ipr_name_table[IPR_DC_TEST_TAG],"DC_TEST_TAG");	/* (0x214) */
      strcpy(ipr_name_table[IPR_DC_TEST_TAG_TEMP],"DC_TEST_TAG_TEMP");	/* (0x215) */
      strcpy(ipr_name_table[IPR_DC_MODE],"DC_MODE");		/* (0x216) */
      strcpy(ipr_name_table[IPR_MAF_MODE],"MAF_MODE");		/* (0x217) */
      
      strcpy(ipr_name_table[EVX$IPR_FP_CTL],"FP_CTL");
      
      strcpy(ipr_name_table[EVX$IPR_DTB_CTL],"DTB_CTL");
      strcpy(ipr_name_table[EVX$IPR_MM_CSR],"MM_CSR");
      
      strcpy(ipr_name_table[EVX$IPR_BIU_ADDR],"BIU_ADDR");
      strcpy(ipr_name_table[EVX$IPR_BIU_STAT],"BIU_STAT");
      strcpy(ipr_name_table[EVX$IPR_DC_ADDR],"DC_ADDR");
      strcpy(ipr_name_table[EVX$IPR_DC_STAT],"DC_STAT");
      strcpy(ipr_name_table[EVX$IPR_FILL_ADDR],"FILL_ADDR");
      strcpy(ipr_name_table[EVX$IPR_ABOX_CTL],"ABOX_CTL");
      strcpy(ipr_name_table[EVX$IPR_BIU_CTL],"BIU_CTL");
      strcpy(ipr_name_table[EVX$IPR_FILL_SYNDROME],"FILL_SYNDROME");
      strcpy(ipr_name_table[EVX$IPR_BC_TAG],"BC_TAG");
      
      strcpy(ipr_name_table[EVX$IPR_HIRR],"HIRR");
      strcpy(ipr_name_table[EVX$IPR_ASTRR],"ASTRR");
      strcpy(ipr_name_table[EVX$IPR_HIER],"HIER");
      strcpy(ipr_name_table[EVX$IPR_SIER],"SIER");
      
      strcpy(ipr_name_table[EVX$IPR_ISSUE_CHK],"ISSUE_CHK");
      strcpy(ipr_name_table[EVX$IPR_SINGLE_ISSUE],"SINGLE_ISSUE");
      strcpy(ipr_name_table[EVX$IPR_DUAL_ISSUE],"DUAL_ISSUE");
      
      strcpy(ipr_name_table[IPR_PAL_R0],"PT0");
      strcpy(ipr_name_table[IPR_PAL_R1],"PT1");
      strcpy(ipr_name_table[IPR_PAL_R2],"PT2");
      strcpy(ipr_name_table[IPR_PAL_R3],"PT3");
      strcpy(ipr_name_table[IPR_PAL_R4],"PT4");
      strcpy(ipr_name_table[IPR_PAL_R5],"PT5");
      strcpy(ipr_name_table[IPR_PAL_R6],"PT6");
      strcpy(ipr_name_table[IPR_PAL_R7],"PT7");
      strcpy(ipr_name_table[IPR_PAL_R8],"PT8");
      strcpy(ipr_name_table[IPR_PAL_R9],"PT9");
      strcpy(ipr_name_table[IPR_PAL_R10],"PT10");
      strcpy(ipr_name_table[IPR_PAL_R11],"PT11");
      strcpy(ipr_name_table[IPR_PAL_R12],"PT12");
      strcpy(ipr_name_table[IPR_PAL_R13],"PT13");
      strcpy(ipr_name_table[IPR_PAL_R14],"PT14");
      strcpy(ipr_name_table[IPR_PAL_R15],"PT15");
      strcpy(ipr_name_table[IPR_PAL_R16],"PT16");
      strcpy(ipr_name_table[IPR_PAL_R17],"PT17");
      strcpy(ipr_name_table[IPR_PAL_R18],"PT18");
      strcpy(ipr_name_table[IPR_PAL_R19],"PT19");
      strcpy(ipr_name_table[IPR_PAL_R20],"PT20");
      strcpy(ipr_name_table[IPR_PAL_R21],"PT21");
      strcpy(ipr_name_table[IPR_PAL_R22],"PT22");
      strcpy(ipr_name_table[IPR_PAL_R23],"PT23");
      strcpy(ipr_name_table[IPR_PAL_R24],"PT24");
      strcpy(ipr_name_table[IPR_PAL_R25],"PT25");
      strcpy(ipr_name_table[IPR_PAL_R26],"PT26");
      strcpy(ipr_name_table[IPR_PAL_R27],"PT27");
      strcpy(ipr_name_table[IPR_PAL_R28],"PT28");
      strcpy(ipr_name_table[IPR_PAL_R29],"PT29");
      strcpy(ipr_name_table[IPR_PAL_R30],"PT30");
      strcpy(ipr_name_table[IPR_PAL_R31],"PT31");
      
      strcpy(ipr_name_table[EVX$IPR_INTR_FLAG],"INTR_FLAG");
      strcpy(ipr_name_table[EVX$IPR_LOCK_FLAG],"LOCK_FLAG");
}

