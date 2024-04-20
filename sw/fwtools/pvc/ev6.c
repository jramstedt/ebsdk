/* EV6.C */
/* EV6 processor specific code split into EV6.C and EV6_RESTRICTIONS.C */
/* will set is1 etc to 0 for guideline 6, if     runtime[i]==0 then stall cycle. */
/* Guideline 6 not implemented...
/* else runtime[cycle_count]_is1 points to an INSTR in raw_code[offs]. */
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
 *	  
 *	  V3.23 fls Jul-96	Merged VMS only with EBSDK version.
 *			Since most of the changes were in the VMS only 
 *			version, it is used as the base and the following 
 *			EBSDK edits have been included:
 * 			$Log: ev6.c,v $
 * 			Revision 1.1  2000/08/09 17:02:18  rickg
 * 			Added support in the Makefile to build pvc, Initial checkin of pvc itself
 * 			pvc known to build on Linux/Alpha.
 * 			BUG(S) FIXED:	None
 * 			BACKEND RELATED: None
 *
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
 *	  V3.24 fls Jul-96 added check for ipr gtr than MAX_IPR in
 *			decode_ipr routine. This caused a hard to debug
 *			bad stack crash when running ev6 palcode with set
 *			cpu ev5. 
 *			added check for index <= 0 in slot_insts routine.
 *	  V3.24 fls Aug-96 - Converted to EV6.
 *			Unlike ev4/ev5 ev6 will do the restriction
 *			checking before schedule_code. In fact it is called
 *			in the schedule_code routine since the checks are
 *			address (fetch_block) related and not cycle related.
 *			The chip specific check restriction routine that 
 *			is called by body.c has been renamed check_cycle_restrictions, and is null.

/*
 * This module must implement the following function
 *
 *	void ev6_init (Chip *)
 *		
 *		This function must assign valid function pointers to
 *		each of the fields of the structure passed as an argument.
 */

#include "types.h"
#include "ispdef.h"
#include "rest.h"
#include "sched.h"
#include "memory.h"
#include "ev6.h"
#include "body.h"
#include "alpha_op.h"		/*v3.26 */
#include "opcode.h"


int force_fail_lock_ev6;		/*V3.24 for restriction_27*/
int first_fetch_block_ev6;

static int exe_offset_to_entry;		/* points to current pal entry address in exe array */
static int iq_ipr_bits;                     /* IPR scoreboard bits */
static int iq_ipr_clr[IPR_CLR_DELAY];      /* delay chain for clearing 'em */
static int iq_ipr_clr_ptr;
static int iq_ipr_inum[IPR_BIT_NUM];
static int iq_holes[2];
static int iq_in_last;
static int iq_out[2];
static int iq_spec_out[2];
static int fq_holes[2];
static int fq_in_last;
static int fq_out[2];
static int fq_spec_out[2];

static int  iq_mb_stall;			/* fls not modeled yet */
static int  iq_mb_stall_extra;			/* fls not modeled yet */
static int  mb_flag;				/* fls not modeled yet */
static long iq_mb_stall_inum;			/* fls not modeled yet */
static int  iq_stc_stall;			/* fls not modeled and not planned */
static int  iq_stc_stall_inum;			/* fls not modeled and not planned */
static int  iq_issue_disable;			/* pvc doesn't model load miss */
static int  hwret_stall_inum;			/* hw ret inst inum for retire*/
static int  instructions_loaded;		/* not used, but loaded same as perf model */
static int subcluster_table[POW2(EBOX_WIDTH*2)];	/* convert per ebox slotting rules */
static int e_arb_order[EBOX_WIDTH];			/* arb between L0,L1 and U0,U1 */
static int e_write_port[EBOX_WIDTH];			/* for write port scheduling */
static int issue_pipe[qi_max];
static int reg_usage[64];
static int reg_usage_flag[64];
static int operate_latency[qi_max];
static int retire_latency[qi_max];
static int last_address;
static int worst_ebox_busy;		/* worst case ebox busy */
static int worst_stalls;		/* worst case stalls */
static int worst_ebox_busy_perm;	/* worst case pipe busy permutation*/
static int worst_stall_perm;		/* worst case stall permutation*/
static int worst_cycles;		/* worst case cycle count */
static int worst_cycle_perm;		/* worst case cycle permutation*/
static int stall_reason;		/* enum reason for stall */
static int tick_stall_reason;		/* for tick printout */
static int tick_busy_reason;		/* for tick printout */
static int issued_this_cycle;	
static int cycle_count;	
static int schedule_finished;
static int total_stall;
static int total_ebox_issued;
static int total_fbox_issued;
static int fbox_issued_count;
static int ebox_busy_count;
static int pipe_busy[EBOX_WIDTH];
static int stall_count;
static int total_squashed;
static int inst_count;
static int inst_in_ebox;
static int ebox_count;
static int ebox_queue_ptr;
static int ic_fetch_ptr;	
static int instructions_fetched;	/* inum which does not wrap like ev6 hardware */
static int inst_in_fbox;
static int fbox_busy_reason;		/* enum reason for fbox busy */
static int fbox_issued_this_cycle;
static int fbox_count;
static int fbox_queue_ptr;
static long ifree1[MAX_REG_FREE_LATENCY+1];
static long ifree2[MAX_REG_FREE_LATENCY+1];
static long ifree3[MAX_REG_FREE_LATENCY+1];	/* pvc assumes 32 bit longs */
static long ffree1[MAX_REG_FREE_LATENCY+1];
static long ffree2[MAX_REG_FREE_LATENCY+1];
static long ffree3[MAX_REG_FREE_LATENCY+1];	/* pvc assumes 32 bit longs */
static long inum_stalls;
static long retire_stalls;
static int  ifree_cnt[MAX_REG_FREE_LATENCY+1];
static int  ffree_cnt[MAX_REG_FREE_LATENCY+1];
static int  map_skid,inum_skid;
static int inum_count;
static char out_buffer[128];

static int dependent_reg_mask;	/* for ..register dependency printout */
static int reg_map[96];         /* int: 0-31   fpt: 32-63   pal shadow: 64-95 (sparse) */
static int group_retire_ptr;
static int retire_ptr;
static int inflight_tail;
static int group_inum_count;
static int retire_stall;
static int debug_dump_tick_num;		/*V3.25*/
static List 	*caveat_list;		/*V3.24*/
static ERR_MSG	*message;		/*V3.24*/

static INSTR    *tick_stage_reg_ebox[EBOX_WIDTH];
static INSTR    *tick_stage_reg_fbox[FADD_PIPES+FMUL_PIPES+FST_PIPES];
static INSTR	*inflight[MAX_INFLIGHT];
static INSTR    *ebox_queue[EBOX_QUEUE_ENTRIES];
static INSTR    *fbox_queue[EBOX_QUEUE_ENTRIES];
static INSTR    *stage_slot[SLOT_WIDTH];
static INSTR    *stage_map[MAP_WIDTH];
static INSTR    *stage_icache[ICACHE_WIDTH];
static INSTR    *sched_code[MAXFLOW*2];    /* V3.24 must be much larger than raw_code */
static FDIV     fdiv[FDIV_NUM];

static IC_STATE ic_state;
static IC_STATE ic_last_state;
static IC_STATE ic_br_stall_return;
static IC_STATE ic_cmov_stall_return;


extern INSTR	*exe;				/* V3.24 */
extern Entry    *pal_entry;	/* pal_entry->next , pal_entry->prev, pal_entry->killed,pal_entry->offset and pal_entry->name */
extern CYCLE 	*runtime[MAXRUNCYCLES];
extern CLASS_INFO *instr_array[];
extern int 	delay, interrupt;
extern List 	*restrictions[];
extern int pal_warning_count;
extern int pal_error_count;
extern int perm;			/* current permutation */
extern int flags;			/* Or'd flags such as FLG$SCHED2 */
extern char 	pheader[];
extern void add_one_caveat (int , int, int , char *,  List *);
extern int  wr (int, char *, ...); 	/* V3.25 */


extern void check_ev6_restriction_2  (INSTR *[], int, int *, int, int);	/*V3.24*/
extern void check_ev6_restriction_4  (INSTR *[], int, int *, int, int);	/*V3.24*/
extern void check_ev6_restriction_12 (INSTR *[], int, int *, int, int);	/*V3.24*/
extern void check_ev6_restriction_14 (INSTR *[], int, int *, int, int);	/*V3.24*/
extern void check_ev6_restriction_18 (INSTR *[], int, int *, int, int);	/*V3.24*/
extern void check_ev6_guideline_26   (INSTR *[], int, int *, int, int);	/*V3.24*/

extern void check_ev6_guideline_11(INSTR *[], int, List *); 				/*V3.24*/
extern void check_ev6_guideline_6  (INSTR *[], int, List *); 				/*V3.24*/
extern void check_ev6_restriction_7(INSTR *[], int, List *); 				/*V3.24*/
extern void check_ev6_restriction_17(INSTR *[], int, List *); 				/*V3.24*/
extern void check_ev6_restriction_19(INSTR *[], int, List *); 				/*V3.24*/
extern void check_ev6_restriction_21(INSTR *[], int, List *); 				/*V3.24*/
extern void check_ev6_restriction_21a(INSTR *[], int, List *); 				/*V3.24*/
extern void check_ev6_restriction_21b(INSTR *[], int, List *); 				/*V3.24*/
extern void check_ev6_restriction_22(INSTR *[], int, List *); 				/*V3.24*/
extern void check_ev6_restriction_22a(INSTR *[], int, List *); 				/*V3.24*/
extern void check_ev6_restriction_22b(INSTR *[], int, List *); 				/*V3.24*/
extern void check_ev6_restriction_23(INSTR *[], int, List *); 				/*V3.24*/
extern void check_ev6_restriction_24ab(INSTR *[], int, List *); 				/*V3.24*/
extern void check_ev6_restriction_24c(INSTR *[], int, List *); 				/*V3.24*/
extern void check_ev6_restriction_27(INSTR *[], int, List *); 				/*V3.24*/
extern void check_ev6_restriction_28a(INSTR *[], int, List *); 				/*V3.24*/
extern void check_ev6_restriction_28b(INSTR *[], int, List *); 				/*V3.24*/
extern void check_ev6_restriction_28c(INSTR *[], int, List *); 				/*V3.24*/
extern void check_ev6_restriction_28d(INSTR *[], int, List *); 				/*V3.24*/
extern void check_ev6_guideline_29(INSTR *[], int, List *); 				/*V3.24*/
extern void check_ev6_restriction_30(INSTR *[], int, List *); 				/*V3.24*/
extern void check_ev6_restriction_33 (INSTR *[], int, int *, int, int);
extern void check_ev6_restriction_34 (INSTR *[], int, List *); 				/*V3.24*/

/*
 *	Functions declared in this module
 *
 */

void ev6_init (Chip *);
int get_inst_id (INSTR *);		/* can't be static, called by ev6_restriction.c */
int get_ipr_id (int);			/* called by ev6_restriction.c */
int check_mfmt_reg_ev6(INSTR *[], int, int, int, int); /* called by ev6_restriction.c */
static void init_stuff (void);
static void at_pal_entry(void);
static void at_pal_return(void);
static int schedule_code (INSTR *[], int, int *);
static void pack_stage(INSTR *stage[], int width);
static void pack_queue(INSTR *queue[], int depth, int *q_ptr, int *count, int *bad_count, int width );
static void ipr_scbd();
static int dirty(INSTR *inst);
static void get_dependent_reg (INSTR *inst);
static void retire();
static int reg_stage();
static void print_instructions(int );
static void queue();
static void map();
static int slot();
static void ic();
static int ic_fill_stage(int, int );
static int copy_sched_code (INSTR *raw_code[]);
static int fill_sched_code (INSTR *raw_code[], int i, int j,int );
static int issue_inst (INSTR *[], int, int,int,int, int, int );
static int is_fp_inst (INSTR *inst);
static int is_pal_shadow (int);
static int is_ps_write (INSTR *inst);
static int mxpr_pipe (INSTR *);
static int get_operate_latency(INSTR *);
static int get_retire_latency(INSTR *);
static void check_address_restrictions (INSTR *[], int); /*V3.24*/
static void check_permutation_restrictions (INSTR *[], int, List *); /*V3.24*/
static void check_cycle_restrictions (int);		/*V3.24 not used by ev6 */
static void print_cycle_ev6 (int org, int i, int, int, int);
static void init_restrictions (void);
static void decode_ipr (disp_table *);
static void decode_hw_memory (disp_table *);
static void init_pipe_use (void);
static void init_operate_latency (void);
static void init_retire_latency (void);
static void init_classes(void);
static void init_decode_tables (void);

static void add_if_ev6_caveat (INSTR *[], int, int *, int, int); /*V3.24*/
static void tick();
static void tick_stage(INSTR *stage[],int width);
static void tick_premap_stage(INSTR *stage[],int width);


void ev6_init (Chip *c)
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
 *	V3.24 FLS Sep-96  Modified for EV6. added at_pal_entry and at_pal_return
 ********************************************************************************
 */
{
      c->chip_init = init_stuff;
      c->restriction_check = check_cycle_restrictions;	/*V3.24 */
      c->decode_ipr = decode_ipr;
      c->decode_hw_memory = decode_hw_memory;
      c->schedule_code = schedule_code;
      c->at_pal_entry = at_pal_entry;			/*V3.24 */
      c->at_pal_return = at_pal_return;			/*V3.24 */

      delay = 0;					/* latency for cache misses */
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
      init_operate_latency();
      init_retire_latency();
      init_classes();
      init_restrictions();
      init_decode_tables();
      force_fail_lock_ev6=FALSE;       /* reset for restriction #27 */

}      

static void at_pal_entry (void)
/*
 ********************************************************************************
 *	PURPOSE:
 *		init variables that are valid between pal entry and pal exit
 *	INPUT PARAMETERS:
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	V3.24 FLS Sep-96 - Called for each pal entry before any permutations
 *			   done.
 ********************************************************************************
 */
{
       exe_offset_to_entry=0;			/* init exe array pal entry pointer */
      worst_stall_perm=0;		/* worst case stall permutation*/
      worst_ebox_busy_perm=0;   	/* worst case pipe busy permutation*/
      worst_ebox_busy=0;	       /* worst case ebox busy count */
      worst_stalls=0;		       /* worst case stall count */
      worst_cycles=0;		       /* worst case cycle count */
      worst_cycle_perm=0;		/* worst case cycle permutation*/

}      



static void at_pal_return (void)
/*
 ********************************************************************************
 *	PURPOSE:
 *		Do operations after all permutations in a pal entry done.
 *	INPUT PARAMETERS:
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	V3.24 FLS Sep-96 - Called for each pal return after all permutaitions done.
 ********************************************************************************
 */
{
    /* if SCHEDULED_CODE flag set then print total stalls for this entry */
  if ((flags & FLG$SCHED2) || (flags & FLG$CYCLES)) {				/*fx add FLG$GUIDELINE here guideline 6?*/

        printf ("Statistics for %s routine at pal entry address: %X \n",pal_entry->name,pal_entry->offset);
        printf ("    Highest cycle      count is  %3d in Permutation (%d) \n",worst_cycles,worst_cycle_perm);
        printf ("    Highest ebox stall count is  %3d in Permutation (%d) \n",worst_stalls,worst_stall_perm);
        printf ("    Highest ebox busy  count is  %3d in Permutation (%d) \n",worst_ebox_busy,worst_ebox_busy_perm);
        wr (FLG$SCHED2,"Statistics for %s routine at pal entry address: %X \n",pal_entry->name,pal_entry->offset);
        wr (FLG$SCHED2,"    Highest cycle      count is  %3d in Permutation (%d) \n",worst_cycles,worst_cycle_perm);
        wr (FLG$SCHED2,"    Highest ebox stall count is  %3d in Permutation (%d) \n",worst_stalls,worst_stall_perm);
        wr (FLG$SCHED2,"    Highest ebox busy  count is  %3d in Permutation (%d) \n",worst_ebox_busy,worst_ebox_busy_perm);
    }
}      




static int schedule_code (INSTR *raw_code[], int delay_flag, int *length)
/*
 * *******************************************************************************
 *	 PURPOSE:
 *		 EV6 code scheduler with much simplification.
 *		 Calls EV6 restriction check routines.
 *
 * First the raw_code[] array is copied to sched_code[] array, with
 * all fetch blocks filled out from exe[] array. This is needed to do the
 * slotting correctly, since the ev6 assigns subclusters based on fetch
 * block address bits <3:0>. These added instructions are set to ->slot_only,
 * since they are not executed in a given permutation (branch path).
 * One consequence of this that ev6 restriction checking (except for guideline
 * 6) must be done prior to the schedule code, since they expect raw_code[]
 * without the extra instructions. 
 *
 * PVC view of the EV6 Pipeline:
 *
 *  Stage		 Perf. model 		PVC model 
 *  -------------------	----------------	-------------------------
 *
 *  Stage 0 Inst. fetch     ic()      		ic() infinite icache
 *			     |			     assumed.
 *			     |
 *  Stage 1 Inst. slot	    slot ();     	slot ()
 *			     |
 *  Stage 2 Map		    map ();      	map ()
 *   			     |
 *  Stage 3 Issue	    queue ()		queue ()
 *   			     |
 *  Stage 4 Reg		    reg ()		select ebox clusters A/B
 *   			     |			select ebox pipes 
 *  			     |issue insts.	issue instructions
 *  
 *
 * PVC next calls EV6 restriction check routines.
 *
 *	 INPUT PARAMETERS:
 *	 	raw_code: An array of the instructions to schedule
 *		delay_flag: either true or false depending on whether PVC
 *			would like this routine to take cache miss latencies
 *			into account.
 *			(not supported for EV6)
 *	 OUTPUT PARAMETERS:
 *	 	the length of the scheduled code sequence.
 *	 IMPLICIT INPUTS:
 *	 IMPLICIT OUTPUTS:
 * 	 FUNCTIONS CALLED DIRECTLY:
 *			check_address_restrictions
 *			check_permutation_restrictions
 *			slot
 *	 AUTHOR/DATE:
 *	V3.24 FLS Nov-96 EV6 scheduler
 *****************************************************************************
 */
{
   int  i, j,offs;

  /* Call ev6 restriction check routines */

  cycle_count=0;					 /* V3.25 in case FLG$SCHED2 clear */

  if (flags & FLG$ERR || flags & FLG$WARN) {
      offs = 0;							/* Offset into fetch block and raw_code[]*/
      create_L(caveat_list);		
        check_address_restrictions(raw_code, offs);	    	/* check  ev6 address related restrictions */
        check_permutation_restrictions( raw_code, offs, caveat_list);/* check ev6 permutation related restrictions */
      destroy_L(caveat_list, Restriction);	/* caveat list lifetime is from pal entry to end of permutation*/
  }



  /* Schedule the code in this permutation */
  if ((flags & FLG$SCHED2) || (flags & FLG$CYCLES)) {				/*fx add FLG$GUIDELINE here guideline 6?*/


      if (delay_flag)
	{
	      /* There is no support for this in the EV6 module */
	      *length = 0;
	wr (FLG$SCHED2, "PVC -There is no support for the set delay command, 0 is assumed \n");
	      return(0);
	}
    /* print warning that this is an optimistic scheduler */
    if (perm == 0) {
        wr (FLG$SCHED2,"NOTE: \n");
        wr (FLG$SCHED2,"    The PVC scheduler is a much simplified model of the 21264.\n");
        wr (FLG$SCHED2,"    It does partially model the Retire, Reg, Queue, map, slot and ic stages,\n");
        wr (FLG$SCHED2,"    but assumes zero latency memories and caches. \n");
        wr (FLG$SCHED2,"    It models iq,fq,ipr,map, and register dependent stalls, but does not \n");
        wr (FLG$SCHED2,"    model br prediction stalls, mb stalls, or inum stalls.\n");
        wr (FLG$SCHED2,"    PVC can be used to check for excessive IPR or register dependency stalls.\n");
        wr (FLG$SCHED2,"\n");
        wr (FLG$SCHED2,"\n");
    }
   /* print "scheduling permutation N */
    wr (FLG$SCHED2,"======> Scheduling pal entry address: %X  PERMUTATION (%d)\n",pal_entry->offset,perm);

    schedule_finished= FALSE;

/* init variables like perf model init_global_vars */

    i=0;    
    while (raw_code[i] != 0) {
	raw_code[i]->use_ca=0;		/* EV6 use integer A-type subcluster , statically computed */
	raw_code[i]->issued=0;		/* EV6 inst has not been issued */
	raw_code[i]->ipr_stall=0;	/* EV6 ipr scoreboard bits */
        raw_code[i]->mra=-1;	        /* EV6 mapped ra  register*/
        raw_code[i]->mrb=-1;	        /* EV6 mapped rb register*/
        raw_code[i]->mrc=-1;	        /* EV6 mapped  dest_reg  rc or ra*/
        raw_code[i]->oldrc=-1;	        /* EV6 mapped  dest_reg  rc or ra*/
	raw_code[i]->slot_only=0;	/* EV6 hack to slot but not execute*/
	raw_code[i]->entry_busy=0;	/* EV6 mark this entry unused*/
	raw_code[i]->cloned=0;		/* EV6 mark this entry not cloned*/
	raw_code[i]->e_nullify_time=MAXRUNCYCLES;	/* EV6 cycle when nop inst counted as iq_spec_out  */
	raw_code[i]->f_nullify_time=MAXRUNCYCLES;	/* EV6 cycle when nop inst counted as fq_spec_out  */
	raw_code[i]->retire_time=0;	/* EV6 cycle when inst will retire */
	raw_code[i]->fpst_valid=0;	/* EV6 used to make fbox signal ibox to issue stores */
	raw_code[i]->inst_num=0;	/* EV6 inum stored in ic() routine */
	i++; 
    }

  /* copy raw_code[] into sched_code[] filling out fetch blocks from   exe[] */
  /* note the filled in fetch blocks will not have inums (inst_num) assigned and all */
  /* will have ->slot_only=1, so they will not issue (since they are not part of */
  /* the current permutation */
  copy_sched_code (raw_code); 


/* init schedule_code variables */

    ic_fetch_ptr = 0;			/* sched_code offset */
    ic_state = IC_FETCH;		/* setup first ic state - force ic_fetch since we don't model icache fills */
    instructions_loaded = 0;		/* written but not used by pvc  */
    instructions_fetched = 0;		/* init inum aka inst_num */
    iq_ipr_bits=0;			/* must be cleared every permutation */
    inst_count=i;

    cycle_count=0;	
    total_stall=0;
    fbox_issued_count=0;
    ebox_busy_count=0;
    total_squashed=0;
    total_ebox_issued=0;
    total_fbox_issued=0;
    stall_count=0;

    inst_in_ebox= 0;
    inst_in_fbox= 0;
    ebox_queue_ptr = EBOX_QUEUE_ENTRIES-4;
    fbox_queue_ptr = FBOX_QUEUE_ENTRIES-4;
    iq_holes[0] = EBOX_QUEUE_ENTRIES;
    iq_holes[1] = EBOX_QUEUE_ENTRIES;
    fq_holes[0] = FBOX_QUEUE_ENTRIES;
    fq_holes[1] = FBOX_QUEUE_ENTRIES;
    iq_in_last=0;
    fq_in_last=0;
     iq_spec_out[0]=0;
     iq_spec_out[1]=0;
     fq_spec_out[0]=0;
     fq_spec_out[1]=0;
     iq_out[0]=0;
     iq_out[1]=0;
     fq_out[0]=0;
     fq_out[1]=0;

    for ( i = 0 ; i < EBOX_WIDTH ; i++ ) {
	   pipe_busy[i] = 0;
	   e_write_port[i] = e_write_port[i] >> 1;
    }
    for ( i = 0 ; i < EBOX_QUEUE_ENTRIES ; i++ ) {
	   ebox_queue[i]=0;				/* clear between permutations */
    }
    for ( i = 0 ; i < FBOX_QUEUE_ENTRIES ; i++ ) {
	   fbox_queue[i]=0;				/* clear between permutations */
    }
    hwret_stall_inum = -1;
    stall_reason = 0;
    dependent_reg_mask = 0;
    fbox_busy_reason = 0;
    for (i =0; i < MAP_WIDTH; i++) stage_map[i]=0;		/* init stage_map per cycle */
    for (i =0; i < SLOT_WIDTH; i++) stage_slot[i]=0;		/* init stage_slot per cycle */
    for (i =0; i < ICACHE_WIDTH; i++) stage_icache[i]=0;	/* init stage_icache per cycle */

    /* I don't know where ifree1 and ifree2 are inited in perf model */
    /* pvc has to do it for future permutations. */
  for (i =0;i < MAX_REG_FREE_LATENCY+1; i++) {
	 ifree1[i]=0; ifree2[i]=0; ifree3[i]=0;
	 ifree_cnt[i]=0;
	 ffree1[i]=0; ffree2[i]=0; ffree3[i]=0;
	 ffree_cnt[i]=0;
  }  
  /* pvc has a problem with regs that have not been written before they 
  are read. For example in the itb miss routine entry 580: there is a
   BLT r22, 5c0  instruction. R22 (aka p_misc) is a pal shadow reg that
   		  if set indicates 1-1 mapping. Pvc maps r4 to r22
		  in the inst at 580: hw_mfpr r4,ev6__iva_form.
		  This incorrectly causes the BLT r22, 5c0 to wait, since
		  it thinks it is dependent on the hw_mfpr r4,xx retiring.
   Solution:
	    set ifree1[0]=0 so r0-r31 not free to be allocated. (Real hardware
	    requires registers to be written to in pal reset code.
	    (see pal restriction 1 in ev6 spec.) this applies to fpu regs 
	    too.
 */

  ifree1[0] = 0;			/* v3.25 force r0-r31 allocated */
  ifree2[0] = ~(0L);
  ifree3[0] = ~(0L);
  ffree1[0] = 0;			/* v3.25 force f0-f31 allocated */
  ffree2[0] = ~(0L);
  ffree3[0] = ~(0L);
  ifree_cnt[0] = MAX_EREG-31-8;	/* max minus r0-r31 and pal shadows allocate when init register maps*/
  ffree_cnt[0] = MAX_FREG-31;	

  /* init the register maps */
  /* These are freed at retire time, as the registers are reused. (see oldrc) */

  j = 0;
  for (i = 0; i < 31; i++) {
    reg_map[i] = i;
    IREG_MAP(i);
    reg_map[i+32] = i+MAX_EREG;
    FREG_MAP(i);
    if (PAL_REG(i)) {
      reg_map[i+64] = j+31;
      IREG_MAP(j+31);
      j++;
    }
  }


  for (i = 0; i < FDIV_NUM; i++) fdiv[i].busy = 0;

 for ( i = 0 ; i < (MAX_EREG+MAX_FREG) ; i++ )  {
    reg_dirty[i].d0 = 0;
    reg_dirty[i].d1 = 0;
  }
  for ( i = 0; i < MAX_INFLIGHT; i++) {
    inflight[i] = 0;
  }
  group_retire_ptr = retire_ptr = inflight_tail = 0;
  inum_count=0;					/* not in perf model */

 debug_dump_tick_num=0;				/* for tick printout */
/* =============== begin main schedule loop =================*/

/*fx squash nops etc by incr total_squashed and setting ->issued=1 ?*/

  while (!schedule_finished) {



    retire(); 		/* RETIRE need to clear ipr bits*/
    
    reg_stage(); 		/* REG STAGE (4)  This stage (in pvc) issues and prints instructions */

    if (cycle_count != 0 || inst_in_ebox || inst_in_fbox)  {	/* spin until cycle 0 instructions  issued */
	stall_reason = 0;
        if (total_ebox_issued + total_fbox_issued + total_squashed >= inst_count) {	/*all instr. issued */
            if (hwret_stall_inum < 0  && inum_count <= 0) { /* if no hw_ret_stall in progress and all retired */
	     schedule_finished = TRUE; 
	     break; 
	    } else {		/* print out stalls that would occur before entering native mode */
		    if (hwret_stall_inum > 0)
		         stall_reason = HW_RET_STALL;	
		    else stall_reason = RETIRE_WAIT;	
		    tick_busy_reason = stall_reason;	/* for tick printout */
		    print_cycle_ev6 (FLG$SCHED2, cycle_count,STALL,stall_reason,FROM_EBOX); 
	           }
         } 

        dependent_reg_mask = 0;
	fbox_busy_reason = 0;
	issued_this_cycle=0;
	fbox_issued_this_cycle=0;
	cycle_count++;
	if (cycle_count > worst_cycles) {
	    worst_cycles = cycle_count;  /* update worst case */
	    worst_cycle_perm = perm;					  /* with perm no. */
	}
    }
    queue(); 			/* QUEUE STAGE (3) */
    
    map();			/* MAP STAGE (2) */
    
    slot();			/* SLOT STAGE (1) slot next 4 inst if no stall */

    /* ic STAGE (0) */   

    ic(); 			/* IC STAGE (0) to call ic_fill_stage to get 4 inst from sched_code*/

    ipr_scbd(); 		/* update ipr scoreboard bits */

    if (flags & FLG$DEBUG_DUMP) {
	tick(); 		/* print tick info for debug of pvc */
	debug_dump_tick_num++;	/* update entry no. for tick printout */
    }
  } /* while !schedule_finished..*/

/* =============== end main schedule loop =================*/

    /* print totals for this permutation */
    wr (FLG$SCHED2,"    Permutation (%d) \n",perm);
    wr (FLG$SCHED2,"        cycle       count: %d \n",cycle_count);
    wr (FLG$SCHED2,"        ebox stall  count: %d \n",total_stall);
    wr (FLG$SCHED2,"        ebox busy   count: %d \n",ebox_busy_count);
    wr (FLG$SCHED2,"        instruction count: %d \n",inst_count);
    wr (FLG$SCHED2,"        ebox issued count: %d \n",total_ebox_issued);
    wr (FLG$SCHED2,"        fbox issued count: %d \n",total_fbox_issued);
    wr (FLG$SCHED2,"           squashed count: %d \n",total_squashed);
    wr (FLG$SCHED2, "\n");

 /* free up memory malloc in fill_sched_code */
    i=0;
    while (sched_code[i] != 0) {
	if (sched_code[i]->cloned)  free (sched_code[i]);
	i++;
    }
  } /* if (flags & FLG$SCHED2.... */

 *length = cycle_count;			/* v3.25 return for set flag cycle_count */
return(1);
}



static void retire()
/*
 * *******************************************************************************
 *	 PURPOSE:
 *
 *	 INPUT PARAMETERS:
 *	 OUTPUT PARAMETERS:
 *	 IMPLICIT INPUTS:
 *	 	ebox_queue: An array of integer instructions to schedule
 *	 	fbox_queue: An array of float  instructions to schedule
 *	 IMPLICIT OUTPUTS:
 * 	 FUNCTIONS CALLED DIRECTLY:
 *
 *	 AUTHOR/DATE:
 *	V3.24 FLS Dec-96 EV6 scheduler
 *****************************************************************************
 */
{
    int i;
    int index,class;
    int next;
    int delta;
    int crossings = 0;
    int cbr_retired = 0;
    int inst_retired = 0;

  /*
  ** The retire pointer can sweep across RETIRE_GROUP_SIZE (2 for EV6) MAP_WIDTH boundaries and pick up all the strays on either side.
  ** Only one conditional branch per cycle.
  */
   while (inum_count > 0) {			/* something to retire */
    next = INC(retire_ptr,MAX_INFLIGHT);
    if ((next/MAP_WIDTH) != (retire_ptr/MAP_WIDTH)) crossings++;
    if (crossings > RETIRE_GROUP_SIZE) break;
    if (inflight[retire_ptr] != 0) {

	/* fls?? not in perf model, how does he do it */
	/* fls?? in pvc  we were retiring inst before they were issued */
      if (inflight[retire_ptr]->issued == 0) break;
      if (inflight[retire_ptr]->retire_time > cycle_count) break;
      index = inflight[retire_ptr]->index;
      class = instr_array[index]->imp->di_class;
      if ((class == qi_ibr) || (class== qi_fbr)) {
	if (cbr_retired >= RETIRE_CBR_MAX) break;
	cbr_retired++;
	/* SIM_RETIRE(inflight[retire_ptr],SKIP_GHIST_COUNT);   not needed by PVC since permutations simulate cond brs */
      }

     /* MTPR retire-time stuff, fls much modified from perf model*/
     /* ld/st scoreboard bits are cleared in issue_inst */
   	     index = inflight[retire_ptr]->index;
	     class = instr_array[index]->imp->di_class;
             if ( class == qi_mtpr ) {   /* if mtpr Clear IPR non ld/st scoreboard bits */	
               if (IPR_RET_BITS(inflight[retire_ptr]->bits.instr)) {
                 for (i = IPR_LS_BIT_NUM; i < IPR_BIT_NUM; i++) {
 	           if (iq_ipr_inum[i] == inflight[retire_ptr]->inst_num) {
	             iq_ipr_clr[ADD(iq_ipr_clr_ptr,IPR_CLR_DELAY-1,IPR_CLR_DELAY)] |= 1<<i;
	           }
                 }
               }
             }
      /* remove hw_ret stall so ic() can stop stalling */
      if (hwret_stall_inum == inflight[retire_ptr]->inst_num) hwret_stall_inum = (-1);
          inflight[retire_ptr]->retire_time = -1;
	  inst_retired++;
	  if (flags & FLG$DEBUG_DUMP) 
              wr (FLG$DEBUG_DUMP,"\ncycle:%4d retired inum:%-4d %4x: %s",cycle_count,
          inflight[retire_ptr]->inst_num,inflight[retire_ptr]->address,inflight[retire_ptr]->decoded);
        } /* if (inflight[retire_ptr]!= 0... */
       inum_count--;
       retire_ptr=next;
   } /* while (inum_count > 0 )... */


  /*
  **  The group retire pointer sweeps past groups of Inums (8 Inums in Ev6) and frees registers and cloned memeory.
  */
  delta = DIFF(retire_ptr,group_retire_ptr,MAX_INFLIGHT);
  if (delta >= (MAP_WIDTH*RETIRE_GROUP_SIZE)) delta = (MAP_WIDTH*RETIRE_GROUP_SIZE);
  else if (delta >= (MAP_WIDTH*REG_FREE_GROUP_SIZE)) delta = (MAP_WIDTH*REG_FREE_GROUP_SIZE);
  else delta = 0;
  for (i = 0; i < delta; i++) {
    if ((inflight[group_retire_ptr] != 0) && (inflight[group_retire_ptr]->mrc != (-1))) {
      /*if inst not issued that sets regdirty cycle can't free it*/
      if (reg_dirty[inflight[group_retire_ptr]->oldrc].d0 != MAXRUNCYCLES) {
	if (inflight[group_retire_ptr]->mrc < MAX_EREG) {				
	  IREG_FREE(inflight[group_retire_ptr]->oldrc,EREG_FREE_LATENCY);
	  inflight[group_retire_ptr]->oldrc = -1;			    /* mark oldrc unused (pvc only method) */
	}
	else {
	  FREG_FREE(inflight[group_retire_ptr]->oldrc,FREG_FREE_LATENCY);
	  inflight[group_retire_ptr]->oldrc = -1;			    /* mark oldrc unused (pvc only method) */
	}
      }
      else {
	reg_dirty[inflight[group_retire_ptr]->oldrc].free_when_clean = 1;
      }
    }
    if (inflight[group_retire_ptr] != 0)
    	if (inflight[group_retire_ptr]->cloned)  free (inflight[group_retire_ptr]);		/* free memory allocated for clone */
    inflight[group_retire_ptr] = 0;							/* retire this inst */
    group_retire_ptr = INC(group_retire_ptr,MAX_INFLIGHT);
  }
  group_inum_count -= delta;

  if (flags & FLG$DEBUG_DUMP) 
    if (inst_retired != 0) 
    wr (FLG$DEBUG_DUMP,"\ncycle: %d inst_retired:%d\n",cycle_count,inst_retired);      
}

	

static void print_instructions(from_box)
/*
 * *******************************************************************************
 *	 PURPOSE:
 *
 *	 INPUT PARAMETERS:
 * 		from_box: FROM_EBOX of FROM_FBOX
 *	 OUTPUT PARAMETERS:
 *	 IMPLICIT INPUTS:
 *	 	ebox_queue: An array of integer instructions to schedule
 *	 	fbox_queue: An array of float  instructions to schedule
 *	 IMPLICIT OUTPUTS:
 * 	 FUNCTIONS CALLED DIRECTLY:
 *
 *	 AUTHOR/DATE:
 *	V3.24 FLS Dec-96 EV6 scheduler
 *****************************************************************************
 */
{
    int i;

 if (cycle_count != 0 || inst_in_ebox || inst_in_fbox)  {	/* spin until inst issued */
  if (from_box == FROM_EBOX) {
	       if ((total_ebox_issued +  total_fbox_issued + total_squashed >= inst_count) )
			stall_reason = NO_MORE_INSTRUCTIONS;
		if (issued_this_cycle < EBOX_WIDTH) {
		   if (stall_reason != 0 && stall_reason != ISSUED_INST && stall_reason != NO_MORE_INSTRUCTIONS){
	 	       if ((stall_reason & REG_DEPEND) ||
	 	           (stall_reason & CLUSTER_BUSY) ||
	 	           (stall_reason & IPR_STALL)	||
	 	           (stall_reason & EBOX_WRITE_PORT_BUSY) ||
			    (stall_reason & PIPE_BUSY))		  ebox_busy_count++;
		       if (ebox_busy_count > worst_ebox_busy) {
		        worst_ebox_busy = ebox_busy_count;                 /* update worst case */
		        worst_ebox_busy_perm = perm;			  /* with perm no. */
		       }
		   }
		}
		if (issued_this_cycle == 0 ) {	/* stall cycle */
		    if (stall_reason != 0) stall_count++;				/* consecutive stall count */
		    if (stall_reason != 0)  total_stall++;				/* total stall count */
		    tick_stall_reason = stall_reason;		/* for tick printout */
        	    print_cycle_ev6 (FLG$SCHED2, cycle_count,STALL,stall_reason,FROM_EBOX); /* print stall cycle */
		    if (stall_reason && total_stall > worst_stalls) {
		        worst_stalls = total_stall;  /* update worst case */
		        worst_stall_perm = perm;					  /* with perm no. */
		    }
		    /* runtime must be allocated and set to -1 for guideline 6 */
	   	    /* will set is1 etc to 0 for guideline 6, if runtime[i]==0 then stall cycle. */
	            /* else runtime[cycle_count]_is1 points to an INSTR in ebox_queue[offs]. */
		    if (cycle_count+1 < MAXRUNCYCLES) {
	                runtime[cycle_count+1] = Mem_Alloc_CYCLE (ebox_queue[ebox_queue_ptr], 0); /* for guideline 6 */
			runtime[cycle_count+1]->is1=0;				 /* for guideline 6 */
	                runtime[cycle_count+1]->is2=0;				 /* for guideline 6 */
	                runtime[cycle_count+1]->is3=0;				 /* for guideline 6 */
	                runtime[cycle_count+1]->is4=0;				 /* for guideline 6 */
		    }
		}else {					/* not stall cycle */
			stall_count=0;				/* reset stall count */
			tick_busy_reason = stall_reason;	/* for tick printout */
			print_cycle_ev6 (FLG$SCHED2, cycle_count,NO_STALL,stall_reason,FROM_EBOX); /* print 1 to 4 insts */
		      }
		if (stall_count > MAXSTALLCOUNT) {
      		    printf("  %%PVC_ERROR in EV6.C->print_instructions routine \n");
	            printf("    Permutation (%d) \n",perm);
      		    printf("    Stall cycle count (%d) exceeds MAXSTALLCOUNT limit.\n",stall_count);
		    if (total_ebox_issued + total_fbox_issued + total_squashed != inst_count) {
      		        printf("    Some instructions did not get issued or squashed.\n");
	    	        printf("    total instruction count: %d \n",inst_count);
	    	        printf("    total ebox issued count: %d \n",total_ebox_issued);
	    	        printf("    total fbox issued count: %d \n",total_fbox_issued);
	    	        printf("    total squashed    count: %d \n",total_squashed);
	            }		
      		    wr (FLG$SCHED2,"    Stall cycle count (%d) exceeds MAXSTALLCOUNT limit.\n",stall_count);
		    schedule_finished=TRUE;
		}
                for ( i = 0 ; i < EBOX_WIDTH ; i++ ) {
		     tick_stage_reg_ebox[i]=0;
	             pipe_busy[i] = 0;
	             e_write_port[i] = e_write_port[i] >> 1;
                }
  }/* if ((from_box == FROM_EBOX) ..*/

  if (from_box == FROM_FBOX) {
		/* check if fbox stall cycle */
		/* no max stall check in fbox.. */
	        if ((total_ebox_issued +  total_fbox_issued + total_squashed >= inst_count) )
				fbox_busy_reason = NO_MORE_INSTRUCTIONS;
		if (fbox_issued_this_cycle == 0 ) {	/* stall cycle */
		      print_cycle_ev6 (FLG$SCHED2, cycle_count,STALL,fbox_busy_reason,FROM_FBOX); /* print stall cycle */
		}
		else {
		        print_cycle_ev6 (FLG$SCHED2, cycle_count,NO_STALL,fbox_busy_reason,FROM_FBOX); /* print 1 to 4 insts */
		      }
  } /*  if ((from_box == FROM_FBOX).. */
 } /*  if (cycle_count != 0 || ... */
}


static int reg_stage()
/*
 * *******************************************************************************
 *	 PURPOSE:
 *
 *	 INPUT PARAMETERS:
 *	 OUTPUT PARAMETERS:
 *	 IMPLICIT INPUTS:
 *	 	ebox_queue: An array of integer instructions to schedule
 *	 	fbox_queue: An array of float  instructions to schedule
 *	 IMPLICIT OUTPUTS:
 * 	 FUNCTIONS CALLED DIRECTLY:
 *
 *	 AUTHOR/DATE:
 *	V3.24 FLS Dec-96 EV6 scheduler
 *****************************************************************************
 */
{
      int 	i,j,k;
      int dirt, class,index;
      int ebox_issued;			/* perf model uses this as a spec issued flag */
      int cluster_dirt;
      int load_issued=0;
      int store_issued=0;
      int found_store=0;
      int wr_regfile_inst;		
      int valid_pipes=0;
      int c0,c1;			/* latency for ebox cross cluster delay */
      int finished=FALSE;
      int pipe_idx;
      int fdiv_count;
      int fadd_count=0;
      int fmul_count=0;
      int fst_count=0;
      

    /* REG STAGE (4) */

      iq_issue_disable = 0;		/* pvc doesn't model load miss */



  iq_holes[0] = iq_holes[1];
if ( inst_in_ebox ) pack_queue(ebox_queue,EBOX_QUEUE_ENTRIES,&ebox_queue_ptr,&inst_in_ebox,&iq_holes[1],MAP_EBOX); 
  if (!BAD_QUEUE_COUNT) iq_holes[1] = EBOX_QUEUE_ENTRIES - inst_in_ebox;
  iq_out[0] = iq_out[1]; iq_out[1] = 0;
  iq_spec_out[0] = iq_spec_out[1]; iq_spec_out[1] = 0;

  for (i = 0; i < EBOX_QUEUE_ENTRIES; i++)
    if (ebox_queue[i] != NULL)
      if (ebox_queue[i]->e_nullify_time == cycle_count) {
	ebox_queue[i] = NULL;
	iq_spec_out[1]++;
      }

  fq_holes[0] = fq_holes[1];
  if ( inst_in_fbox ) pack_queue(fbox_queue,FBOX_QUEUE_ENTRIES,&fbox_queue_ptr,&inst_in_fbox,&fq_holes[1],MAP_FBOX);
  if (!BAD_QUEUE_COUNT) fq_holes[1] = FBOX_QUEUE_ENTRIES - inst_in_fbox;
  fq_out[0] = fq_out[1]; fq_out[1] = 0;
  fq_spec_out[0] = fq_spec_out[1]; fq_spec_out[1] = 0;

  for (i = 0; i < FBOX_QUEUE_ENTRIES; i++)
    if (fbox_queue[i] != NULL)
      if (fbox_queue[i]->f_nullify_time == cycle_count) {
	fbox_queue[i] = NULL;
	fq_spec_out[1]++;
      }

/* Issue EBOX instructions */


 if ( inst_in_ebox && !iq_issue_disable ) {
   issued_this_cycle=0;
    
  for (j = 0; issued_this_cycle < EBOX_WIDTH && j < ebox_queue_ptr; j++) {
          ebox_issued = 0;
     /* search ebox_queue and fbox_queue for executable instructions */
        if ( ebox_queue[j] == NULL) continue;
 
	if (ebox_queue[j]->issued) { stall_reason |= ISSUED_INST; continue; } 
        if (ebox_queue[j]->ipr_stall) { stall_reason |= IPR_STALL; continue; }

	dirt = dirty (ebox_queue[j]);	 /* get reg dependent dirty bits */

      /* Look for an ebox integer  pipe to which we can issue */
      for (k = 0; k < EBOX_WIDTH; k++) {
	    /* the perf model calls any inst (and others) that uses ra,rb,rc a irec_docode.op = TRUE */
	    /* the perf model must know which are affected by the register file cross cluster delay */
            /* pvc does not store this architectural info (normally done in sched.c), so we will  */
            /* have to decode it here. Not sure I have them all. */
            index = ebox_queue[j]->index;
	    class = instr_array[index]->imp->di_class;
	    wr_regfile_inst = (instr_array[index]->arch->dest_reg != Noreg) || (class == qi_lda) ||
			      (class == qi_mfpr) || (class == qi_mtpr) || (index == DO_MB);

	    pipe_idx = e_arb_order[k];				/* pipe idx:  0:c0a, 1:c0b, 2:c1a, 3:c1b */

            /*fls?	mem_pipe_idx = mem_pipe_map[pipe_idx]; */

	    if (pipe_busy[pipe_idx]) { 
	        if (pipe_idx == 0) { stall_reason |= PIPE_L0_BUSY | PIPE_BUSY; continue;}
	        if (pipe_idx == 1) { stall_reason |= PIPE_U0_BUSY | PIPE_BUSY; continue;}
	        if (pipe_idx == 2) { stall_reason |= PIPE_L1_BUSY | PIPE_BUSY; continue;}
	        if (pipe_idx == 3) { stall_reason |= PIPE_U1_BUSY | PIPE_BUSY; continue;}
	    }
            /*fls?	if (ebox_queue[j]->irec->decode.ld && !dcache_load_ok[mem_pipe_idx] ) continue; */
            /*fls?	if (ebox_queue[j]->irec->decode.st && !dcache_store_ok[mem_pipe_idx] ) continue; */

	    /* check if statically computed subclusters pipe assignments allow issue */
	    if (ebox_queue[j]->use_ca &&   (pipe_idx & 1)) { 
			stall_reason |= PIPE_L0_NOT_ALLOWED | PIPE_L1_NOT_ALLOWED; continue;
	    }
	    if (!ebox_queue[j]->use_ca && !(pipe_idx & 1)) {
			 stall_reason |= PIPE_U0_NOT_ALLOWED | PIPE_U1_NOT_ALLOWED; continue;
	    }

	    /* ebox cluster 0 <-> cluster 1  write port conflict */
	    if ((wr_regfile_inst || ebox_queue[j]->index == DO_JSR || ebox_queue[j]->index == DO_BR)
	       && (e_write_port[pipe_idx] & (1 << get_operate_latency(ebox_queue[j])))) 
			{ stall_reason |= EBOX_WRITE_PORT_BUSY; continue;}

	    if ((class == qi_mfpr) || (class == qi_mtpr))
		 valid_pipes = mxpr_pipe (ebox_queue[j]);   /* MxPRs use diff. pipes depending on the IPR */
	    else valid_pipes = issue_pipe[class];	     /* other instructions only depend on class */

	    /* if instruction doesn't run in current selected pipe continue to try another */
	    if ((pipe_idx == 0) && !(valid_pipes & PIPE_L0)) 
		{ stall_reason |= PIPE_L0_NOT_ALLOWED;  continue; }
	    if ((pipe_idx == 1) && !(valid_pipes & PIPE_U0)) 
		{ stall_reason |= PIPE_U0_NOT_ALLOWED;  continue; }
	    if ((pipe_idx == 2) && !(valid_pipes & PIPE_L1)) 
		{ stall_reason |= PIPE_L1_NOT_ALLOWED;  continue;}
	    if ((pipe_idx == 3) && !(valid_pipes & PIPE_U1))
		{ stall_reason |= PIPE_U1_NOT_ALLOWED; continue;}

            cluster_dirt = dirt & ((pipe_idx & 0x2) ? 0x2 : 0x1); /* Pick up correct dirty bit:  c0 vs. c1 */

	      if (!cluster_dirt) {
	               ebox_issued=1;
	      } else  if (dirt) {
			 stall_reason |= REG_DEPEND; 
			 get_dependent_reg(ebox_queue[j]);	/* for ..register dependency printout */
		      }
		      else
			 stall_reason |= CLUSTER_BUSY;		/* ebox regfile cross write delay */

	if (ebox_issued) {
	    if (wr_regfile_inst || ebox_queue[j]->index == DO_JSR || ebox_queue[j]->index == DO_BR)
	          e_write_port[pipe_idx] += (1 << get_operate_latency(ebox_queue[j]));
	    tick_stage_reg_ebox[pipe_idx] = ebox_queue[j];
	    c0 = SPLIT_EBOX && !(pipe_idx & 0x2);	/*calculate possible cross cluster latency */
	    c1 = SPLIT_EBOX &&  (pipe_idx & 0x2);

	        /* issue integer instruction */	    
		ebox_queue[j]->issued=1;		/* must be done here issue_inst does ebox_queue[j]=NULL */
	        issue_inst (ebox_queue,FROM_EBOX,j,c0,c1, issued_this_cycle, cycle_count); 
		stall_count=0;
                issued_this_cycle++;
	        total_ebox_issued++;
	        pipe_busy[pipe_idx] = 1;
		break;
	} /* if (ebox_issued).. */

      } /* for (k = 0; k < EBOX_WIDTH; k++... */

      if (issued_this_cycle == EBOX_WIDTH) break;	/* issued 4 this cycle */
      
  }/* for (j = 0; total_ebox_issued < EBOX_WIDTH && j < ebox_queue_ptr; j++.. */
 } /* if inst_in_ebox... */

 print_instructions(FROM_EBOX);	/* update ebox totals and print instructions or stall cycles */

  for (i=0;i<FBOX_WIDTH;i++) 	/* clear out reg stage from previous cycle */
    tick_stage_reg_fbox[i]=0;    

/* Issue FBOX instructions */

  fadd_count=0;
  for (i = 0; i < FDIV_NUM; i++) {
    if (fdiv[i].busy) {
      fdiv[i].busy--;
      if (!fdiv[i].busy) {
	fadd_count++;
	reg_dirty[fdiv[i].inst->mrc].d0 = cycle_count+FADD_LATENCY;
	reg_dirty[fdiv[i].inst->mrc].d1 = cycle_count+FADD_LATENCY;
/*	p_reg_value[fdiv[i].inst->mrc] = fdiv[i].inst->newrc_value; */
	if (reg_dirty[fdiv[i].inst->mrc].free_when_clean) {
	  FREG_FREE(fdiv[i].inst->mrc,0);
	} 
      } 
    } 
  }
  fdiv_count = 0;
  for (i = 0; i < FDIV_NUM; i++) if (fdiv[i].busy) fdiv_count++;

  fmul_count=0;
  fst_count=0;
  if (inst_in_fbox ) {
    fbox_issued_this_cycle=0;
    for (j = 0 ; fbox_issued_this_cycle < FBOX_WIDTH && j < fbox_queue_ptr ; j++) {
      if (fbox_queue[j] == 0) continue;
      if (fbox_queue[j]->issued) { fbox_busy_reason |= ISSUED_INST; continue; } 

/* in perf model 
 *      if ( fbox_queue[j]->inst_num > oldest ) oldest = fbox_queue[j]->inst_num;
 *      if ( fbox_queue[j]->inst_num < youngest ) youngest = fbox_queue[j]->inst_num;
 */
      /* if there's a remaining functional unit and the source registers are available */
      index = fbox_queue[j]->index;
      class = instr_array[index]->imp->di_class;

      if ((class == qi_fmul) && (fmul_count >= FMUL_PIPES)) { 
	    fbox_busy_reason |= PIPE_BUSY | PIPE_FM_BUSY; continue;
      }
      else if ((class == qi_fst) && (fst_count >= FST_PIPES)) {
		fbox_busy_reason |= PIPE_BUSY | PIPE_FST0_BUSY | PIPE_FST1_BUSY; continue;
	   }
      else if ( !( (class == qi_fst) || (class == qi_fmul)) && (fadd_count >= FADD_PIPES)) {
		fbox_busy_reason |= PIPE_BUSY | PIPE_FM_BUSY; continue;
	   }  

      if (class == qi_fdiv && (fdiv_count >= FDIV_NUM)) {
	   fbox_busy_reason |= PIPE_BUSY | PIPE_FA_BUSY; continue;
      }
      dirt = dirty(fbox_queue[j]);

	if (class == qi_fmul) {
	  i = fmul_count;
	  fmul_count++;
	}
	else {
	  if (class == qi_fst) {
	    i = FMUL_PIPES+FADD_PIPES+fst_count;
	    fst_count++;
	  }
	  else {
	    i = FMUL_PIPES+fadd_count;
	    fadd_count++;
	    if (class == qi_fdiv) fdiv_count++;
	  }
	}
	tick_stage_reg_fbox[i] = fbox_queue[j];
	if (dirt) {						/* pvc diffs from perf model here */
		 fbox_busy_reason |= REG_DEPEND; 
	} 
	else {
                 /* 
	         * The instruction issues & its operands are available
	         */
	         if (class == qi_fdiv) {
	             for (i = 0; i < FDIV_NUM; i++) {
	               if (fdiv[i].busy) continue;
	               fdiv[i].busy = get_operate_latency(fbox_queue[j]) - FADD_LATENCY;
	               fdiv[i].inst = fbox_queue[j];
	            }
	           }
	      fbox_queue[j]->issued=1;		/* must be done here issue_inst does ebox_queue[j]=NULL */
	      issue_inst (fbox_queue,FROM_FBOX,j,0,0, fbox_issued_this_cycle, cycle_count);
              fbox_issued_this_cycle++;
	      total_fbox_issued++;
	    }
    if (fbox_issued_this_cycle == FBOX_WIDTH) break;	/* issued 2 this cycle */
    } /* for (j = 0 ; fbox_issued_this_cycle ... */

  print_instructions(FROM_FBOX);	/* update fbox totals and print instructions or stall cycles */
  } /*   if (inst_in_fbox... */

return(1);
}



static void queue()
/*
 * *******************************************************************************
 *	 PURPOSE: 
 *		Ev6 iq and fq ..
 *
 *	 INPUT PARAMETERS:
 *	 OUTPUT PARAMETERS:
 *	 IMPLICIT INPUTS:
 *	 	ebox_queue: An array of integer instructions to map
 *	 	fbox_queue: An array of float  instructions to map
 *	 IMPLICIT OUTPUTS:
 * 	 FUNCTIONS CALLED DIRECTLY:
 *
 *	 AUTHOR/DATE:
 *	V3.24 FLS Dec-96 Map registers for EV6 scheduler
 *****************************************************************************
 */

{

  int i,j,bits;
  int index,class,reg_dst;
  int reg_is_31=FALSE;
  int DO_TRAPB = 0;	/* not in ispdef.h yet */
  int DO_EXCB = 0;	/* not in ispdef.h yet */
  int do_squash = FALSE;
  int iq_free,fq_free;
  int eq_need=0;
  int fq_need=0;

  iq_free = iq_holes[0] + iq_out[0] + iq_spec_out[0] - iq_in_last;
  fq_free = fq_holes[0] + fq_out[0] + fq_spec_out[0] - fq_in_last;
  if (!BAD_QUEUE_COUNT) {	
    if (iq_free != (EBOX_QUEUE_ENTRIES - inst_in_ebox)) { 
      printf("  %%PVC_ERROR internal error in ev6.c queue routine\n");
      printf(" Ebox queue count error:   iq_free:%d  should be:%d\n",iq_free,EBOX_QUEUE_ENTRIES - inst_in_ebox);
      printf("\tcycle_count:%ld\n",cycle_count);
 	exit(0);
    }
    if (fq_free != (FBOX_QUEUE_ENTRIES - inst_in_fbox)) { 
      printf("  %%PVC_ERROR internal error in ev6.c queue routine\n");
      printf("  Fbox queue count error:   fq_free:%d  should be:%d\n",fq_free,FBOX_QUEUE_ENTRIES - inst_in_fbox);
      printf("\tcycle_count:%ld\n",cycle_count);
      exit(0);
    }
  }

  iq_in_last = 0;
  fq_in_last = 0;



  for (i = 0; i < MAP_WIDTH; i++) {
    if ( stage_map[i] == 0 ) break;
    eq_need += !(is_fp_inst(stage_map[i]));
/* perf model    fq_need += (is_fp_inst(stage_map[i]) || (FBOX_SLOW_STORE &&  stage_map[i]->irec->decode.st && (stage_map[i]->irec->rb > 31) && (stage_map[i]->irec->rb < 64)));
*/
    fq_need += is_fp_inst(stage_map[i]);
    index = stage_map[i]->index;
    class = instr_array[index]->imp->di_class;
    if ((class == qi_mtpr) && (IPR_BITS(stage_map[i]->bits.instr) & iq_ipr_bits)) {
      stall_reason |= IPR_STALL;
      return;
    }
  }
  if (iq_free < eq_need) {
    stall_reason= IQ_STALL;
    return;
  }
  if (fq_free < fq_need) {
    stall_reason= FQ_STALL;
    fbox_busy_reason= stall_reason;
    return;
  }

  for ( i = 0 ; i < MAP_WIDTH ; i++ ) {
    if ( stage_map[i] == 0 ) break;
    stage_map[i]->fpst_valid = cycle_count;
    index = stage_map[i]->index;
    class = instr_array[index]->imp->di_class;
    if (instr_array[index]->arch->dest_reg == Ra) 
        reg_dst = stage_map[i]->bits.op_format.ra;
    else if (instr_array[index]->arch->dest_reg == Rc) 
             reg_dst = stage_map[i]->bits.op_format.rc;
    if (reg_dst == 31)  
	 reg_is_31 = TRUE; 
    else reg_is_31 = FALSE;

    if ( FBOX_SLOW_STORE &&  (class == qi_ist) && (is_fp_inst(stage_map[i])) && 
	dirty(stage_map[i]) ) {   /* fpst goes in both queues */

        stage_map[i]->fpst_valid = MAXRUNCYCLES;			/*fls was MAX_INT */
        fbox_queue[fbox_queue_ptr++] = stage_map[i];
        inst_in_fbox++;
        fq_in_last++;
        ebox_queue[ebox_queue_ptr++] = stage_map[i];
        iq_in_last++;
        inst_in_ebox++;
        stage_map[i] = 0;
    }
    else if (is_fp_inst(stage_map[i])) {
          fbox_queue[fbox_queue_ptr++] = stage_map[i];
          inst_in_fbox++;
          fq_in_last++;

	/* squash float instructions that do nothing */
	if ((index != DO_MT_FPCR) && (reg_is_31)){
	     if ((class == qi_fadd) || (class == qi_fsqrt) || 
		 (class == qi_itof) || (class == qi_fmul) || 
		 (class == qi_fdiv)) {
		        stage_map[i]->issued=1;	/* mark it already issued to squash it */
		        stage_map[i]->retire_time= -1;	 /*  mark it retired */
			stage_map[i]->f_nullify_time = cycle_count+1;
	                total_squashed++;			/* count it */
	    }
	}
          stage_map[i] = 0;
     }/* if ( FBOX_SLOW_STORE &&  (class == qi_ist).... */
     else  {
          ebox_queue[ebox_queue_ptr] = stage_map[i];
          if ((class == qi_ild) || (class == qi_ist)) {
	      ebox_queue[ebox_queue_ptr]->ipr_stall = iq_ipr_bits & IPR_LS_MASK;
          }
          ebox_queue_ptr++;

          if ((class == qi_mtpr) && IPR_BITS(stage_map[i]->bits.instr)) {
	    bits = IPR_BITS(stage_map[i]->bits.instr);
	    iq_ipr_bits |= bits;
	    j = 0;
	    while (bits) {
	      if (bits&1) {
	        iq_ipr_inum[j] = stage_map[i]->inst_num;
	      }
	      bits = bits>>1;
	      j++;
	    }
          }
	    do_squash = FALSE;
	    if ((index == DO_TRAPB) || (index == DO_EXCB)) 
		  do_squash=TRUE;
	    if ((reg_is_31)){
	     if ((class == qi_ild) || (class == qi_ist) || 
		 (class == qi_ilog) || (class == qi_imul))
		  do_squash = TRUE;
	    }
	    if (do_squash) {
	        stage_map[i]->issued=1;	/* mark it already issued to squash it */
                stage_map[i]->retire_time= -1;	   /*  mark it retired */
		stage_map[i]->e_nullify_time = cycle_count+1;
	        total_squashed++;			/* count it */
	    }
          inst_in_ebox++;
          iq_in_last++;
          stage_map[i] = 0;
         }
  }/*   for ( i = 0 ; i < MAP_WIDTH ; i++... */

      pack_stage(stage_map,MAP_WIDTH);

}


static void map()
/*
 * *******************************************************************************
 *	 PURPOSE:
 *
 *	 INPUT PARAMETERS:
 *	 OUTPUT PARAMETERS:
 *	 IMPLICIT INPUTS:
 *	 	ebox_queue: An array of integer instructions to map
 *	 	fbox_queue: An array of float  instructions to map
 *	 IMPLICIT OUTPUTS:
 * 	 FUNCTIONS CALLED DIRECTLY:
 *
 *	 AUTHOR/DATE:
 *	V3.24 FLS Dec-96 Map registers for EV6 scheduler
 *****************************************************************************
 */
{

  unsigned long temp;
  int i,j;
  int map_count = 0;
  int fbox_count = 0;
  int ebox_count = 0;
  int reg_mapped = 0;
  int free_int_vector[MAP_WIDTH];
  int free_fp_vector[MAP_WIDTH];
  int ebox_mapped=0;
  int fbox_mapped=0;
  int ereg_count=0,freg_count=0;
  static int inum;
  CLASS_INFO *class_def; 

  /* Update free list and move pending free pipe */
  ifree1[0] |= ifree1[1];
  ifree2[0] |= ifree2[1];
  ifree3[0] |= ifree3[1];
  ffree1[0] |= ffree1[1];
  ffree2[0] |= ffree2[1];
  ffree3[0] |= ffree3[1];
  ifree_cnt[0] += ifree_cnt[1];
  ffree_cnt[0] += ffree_cnt[1];
  for (i = 1; i < MAX_REG_FREE_LATENCY; i++) {
    ifree1[i] = ifree1[i+1];
    ifree2[i] = ifree2[i+1];
    ifree3[i] = ifree3[i+1];
    ffree1[i] = ffree1[i+1];
    ffree2[i] = ffree2[i+1];
    ffree3[i] = ffree3[i+1];
    ifree_cnt[i] = ifree_cnt[i+1];
    ffree_cnt[i] = ffree_cnt[i+1];
  }
  ifree1[MAX_REG_FREE_LATENCY] = 0;
  ifree2[MAX_REG_FREE_LATENCY] = 0;
  ifree3[MAX_REG_FREE_LATENCY] = 0;
  ffree1[MAX_REG_FREE_LATENCY] = 0;
  ffree2[MAX_REG_FREE_LATENCY] = 0;
  ffree3[MAX_REG_FREE_LATENCY] = 0;
  ifree_cnt[MAX_REG_FREE_LATENCY] = 0;  
  ffree_cnt[MAX_REG_FREE_LATENCY] = 0;  
		
 if (ifree_cnt[0] < MAP_EBOX) {				/*fls if not 4? free regs return */
    stall_reason = MAP_STALL;
  }
  else if (ffree_cnt[0] < MAP_FBOX) {
    stall_reason = MAP_STALL;
  }
  else if ( (stage_map[0] == 0) && (stage_slot[0] != 0) ) {

     int set_ld_st_wait_used = 0;

    for ( i= 0 ; i < MAP_WIDTH ; i++ )  {
	free_int_vector[i]=0;			/* shouldn't need to do this. */
        free_fp_vector[i]=0;			/* but will to be safe */
    }

    /* For PVC have renamed mapped to ebox_mapped and fbox_mapped. */
    /* this to fix a bug herein that should free reg 0000 when it     */
    /* wasn't due to MAP_EAT_UNUSED code further on. The problem occurred    */  
    /* when ifree1 had only two registers mapped but the MAP_EAT_UNUSED      */
    /* code freed up 000000 as free register 0. This could be avoided by     */
    /* not using 000000 as free reg 0, but I am not going to make that change*/

    /* To make this go as fast as possible don't use IS_IREG_FREE */
    ebox_mapped=0;
    j=0;
    temp = ifree1[0];
    while ((temp != 0) && (ebox_mapped<MAP_EBOX)) {
      if (temp & 0x1)
	free_int_vector[ebox_mapped++]=j;
      j++;      
      temp = temp >> 1;
    }
    j = 32;
    temp = ifree2[0];
    while ((temp != 0) && (ebox_mapped<MAP_EBOX) && (j<MAX_EREG)) {
      if (temp & 0x1)
	free_int_vector[ebox_mapped++]=j;
      j++;      
      temp = temp >> 1;
    }
    /* fls pvc needs third reg in case 32 bit vax */
    j = 64;
    temp = ifree3[0];
    while ((temp != 0) && (ebox_mapped<MAP_EBOX) && (j<MAX_EREG)) {
      if (temp & 0x1)
	free_int_vector[ebox_mapped++]=j;
      j++;      
      temp = temp >> 1;
    }

    /* To make this go as fast as possible don't use IS_FREG_FREE */
    fbox_mapped=0;
    j=0;
    temp = ffree1[0];
    while ((temp != 0) && (fbox_mapped<MAP_FBOX)) {
      if (temp & 0x1)
	free_fp_vector[fbox_mapped++]=j;
      j++;      
      temp = temp >> 1;
    }
    j = 32;
    temp = ffree2[0];
    while ((temp != 0) && (fbox_mapped<MAP_FBOX) && (j<MAX_FREG)) {
      if (temp & 0x1)
	free_fp_vector[fbox_mapped++]=j;
      j++;      
      temp = temp >> 1;
    }
    j = 64;
    temp = ffree3[0];
    while ((temp != 0) && (fbox_mapped<MAP_FBOX) && (j<MAX_FREG)) {
      if (temp & 0x1)
	free_fp_vector[fbox_mapped++]=j;
      j++;      
      temp = temp >> 1;
    }

    for ( i= 0 ; i < MAP_WIDTH ; i++ )  {
      if ( stage_slot[i] == 0 ) break;
      
      if ((fbox_count >= MAP_FBOX) &&  is_fp_inst(stage_slot[i]))  break;		/*fls fp inst? */
      if ((ebox_count >= MAP_EBOX) && !is_fp_inst(stage_slot[i]))  break;		/*fls integer inst? */

      inflight[inflight_tail] = stage_slot[i];
      inflight_tail = INC(inflight_tail,MAX_INFLIGHT);

      if (is_fp_inst(stage_slot[i])) fbox_count++;
      else ebox_count++;

      stage_map[i] = stage_slot[i];
      map_count++;

/*fls cmov1 cmov2 clone code  code deleted here */
/*   will need an fbox version */
        class_def = instr_array[stage_map[i]->index];
	/* ra and rb are not allocated from free regs, since they are not loaded as a result of ebox ops, but from mem or iprs */
        /* so for pvc must add some way to mark ra dirty when it is a dest_reg? */
	/* if inst uses ra reg not as dest_reg get one from free list and write it to mra else set mra=-1 */
	if (( (class_def->arch->dest_reg != Ra) && class_def->arch->op_ra &&  (stage_map[i]->bits.op_format.ra != 31)) ||
	     ((class_def->arch->dest_reg != Ra) && class_def->arch->ra_flt && (stage_map[i]->bits.op_format.ra != 31))) {
	        if ( is_fp_inst(stage_map[i]) ) 
	             stage_map[i]->mra = reg_map[stage_map[i]->bits.op_format.ra+32];	/* make float ra reg dependent */
                else stage_map[i]->mra = reg_map[stage_map[i]->bits.op_format.ra];		/* make ra reg dependent */
	}else stage_map[i]->mra = -1;

	if ( ((class_def->arch->op_rb  && (stage_map[i]->bits.op_format.rb != 31)) ||
	     (class_def->arch->rb_flt && (stage_map[i]->bits.op_format.rb != 31))) &&
	     (!(class_def->arch->chk_lit_mode && stage_map[i]->bits.opl_format.litflag))) {	/* don't check literals */
	        if ( is_fp_inst(stage_map[i]) ) 
	             stage_map[i]->mrb = reg_map[stage_map[i]->bits.op_format.rb+32];	/* make float rb reg dependent */
                else stage_map[i]->mrb = reg_map[stage_map[i]->bits.op_format.rb];		/* make rb reg dependent */
	}else stage_map[i]->mrb = -1;

	/* pvc differs from perf model here. the perf model uses parse_xxx code to store ra into rc
         * when it is the dest reg (this isn't obvious). PVC will do the same.
	 */
	/* if this instruction has a dest_reg then get free reg */
	if ((class_def->arch->dest_reg == Rc && (stage_map[i]->bits.op_format.rc != 31)) ||
	    (class_def->arch->dest_reg == Ra && (stage_map[i]->bits.op_format.ra != 31))) {
	reg_mapped++;
	if (is_fp_inst(stage_map[i]))  {
	    if (class_def->arch->dest_reg == Rc) {
	         stage_map[i]->oldrc = reg_map[stage_map[i]->bits.op_format.rc];  /* save old register number */
	         stage_map[i]->mrc = reg_map[stage_map[i]->bits.op_format.rc] = MAX_EREG+free_fp_vector[freg_count];
	    }
	    else {
	         stage_map[i]->oldrc = reg_map[stage_map[i]->bits.op_format.ra];  /* save old register number */
	         stage_map[i]->mrc = reg_map[stage_map[i]->bits.op_format.ra] = MAX_EREG+free_fp_vector[freg_count];
	         stage_map[i]->mra = -1;					/* force mra unused */
		 }
	    FREG_MAP(free_fp_vector[freg_count]);
	    freg_count++;
	}
	else {
	    if (free_int_vector[ereg_count] > MAX_EREG) {		/* for debug only */
	        printf("  %%PVC_ERROR internal error in EV6.C->map routine \n");
	        printf("    invalid free register > r31 in free_int_vector[ereg_count]. \n");
	            printf("   free_int_vector[ereg_count]= %X \n",free_int_vector[ereg_count]);
	    }
	    if (class_def->arch->dest_reg == Rc) {
	         stage_map[i]->oldrc = reg_map[stage_map[i]->bits.op_format.rc];  /* save old register number */
	         stage_map[i]->mrc = reg_map[stage_map[i]->bits.op_format.rc] =  free_int_vector[ereg_count];
	    }
	    else {
	         stage_map[i]->oldrc = reg_map[stage_map[i]->bits.op_format.ra];  /* save old register number */
	         stage_map[i]->mrc = reg_map[stage_map[i]->bits.op_format.ra] =  free_int_vector[ereg_count];
	         stage_map[i]->mra = -1;					/* force mra unused */
		 }
	    IREG_MAP(free_int_vector[ereg_count]);
	    ereg_count++;
	}
        reg_dirty[stage_map[i]->mrc].d0 = MAXRUNCYCLES; /* set it dirty */
        reg_dirty[stage_map[i]->mrc].d1 = MAXRUNCYCLES; 
	reg_dirty[stage_map[i]->mrc].free_when_clean = 0;
      }
      else {
	stage_map[i]->mrc = -1;
	stage_map[i]->oldrc = -1;  
      }


     stage_slot[i] = 0;				/* mark it mapped */
    } /* for ( i= 0 ; i < MAP_WIDTH ; i++... */
   } /* if ( (stage_map[0] == 0) && (stage_slot[0] != 0) .. */


  if (map_count != 0) {
    for (i = map_count; i < MAP_WIDTH; i++) {
      inflight_tail = INC(inflight_tail,MAX_INFLIGHT);   /* Eat MAP_WIDTH inums per cycle */
    }
    inum_count += MAP_WIDTH;
    group_inum_count += MAP_WIDTH;

    if (MAP_EAT_UNUSED) {
      if (ereg_count < ebox_mapped) {			/* fls this not in perf. model */
          for (i = ereg_count; i < ebox_mapped; i++) {	/* fls this is i < MAP_EBOX in perf model */
	    IREG_MAP(free_int_vector[i]);
	    IREG_FREE(free_int_vector[i],MAP_EAT_UNUSED+1);
	    ereg_count++;
          }

      }
      if (freg_count < fbox_mapped) {			/* fls this not in perf. model */
          for (i = freg_count; i < fbox_mapped; i++) {	/* fls this is i < MAP_FBOX in perf model */
	    FREG_MAP(free_fp_vector[i]);
	    FREG_FREE(free_fp_vector[i]+MAX_EREG,MAP_EAT_UNUSED+1);
	    freg_count++;
          }
     }
   }
  }
      
  ifree_cnt[0] -= ereg_count;
  ffree_cnt[0] -= freg_count;
  pack_stage(stage_slot,SLOT_WIDTH);

}


static int dirty(INSTR *inst)
{
  int d = 0;
  if ( inst->mra != -1 ) {
    if ( reg_dirty[inst->mra].d0 > cycle_count )  d = 0x1;
    if ( reg_dirty[inst->mra].d1 > cycle_count )  d |= 0x2;
  }
  if ( inst->mrb != -1 ) {
    if ( reg_dirty[inst->mrb].d0 > cycle_count )  d |= 0x1;
    if ( reg_dirty[inst->mrb].d1 > cycle_count )  d |= 0x2;
  }
  return(d);
}

static void get_dependent_reg(INSTR *inst)
{
  int d;
  int rega,regb;

  d=0;
  if ( inst->mra != -1 ) {
    if ( reg_dirty[inst->mra].d0 > cycle_count )  d = 0x1;
    if ( reg_dirty[inst->mra].d1 > cycle_count )  d |= 0x2;
  }
  if ((d & 1) || (d & 2)) {
        rega = inst->bits.op_format.ra;
	dependent_reg_mask |=  (1 << rega);
  }
  d=0;
  if ( inst->mrb != -1 ) {
    if ( reg_dirty[inst->mrb].d0 > cycle_count )  d |= 0x1;
    if ( reg_dirty[inst->mrb].d1 > cycle_count )  d |= 0x2;
  }

  if ((d & 1) || (d & 2)) {
        regb = inst->bits.op_format.rb;
	dependent_reg_mask |=  (1 << regb);
  }
  return;
}


static void ipr_scbd()
{
  int i;

  /* Update IQ IPR scoreboard bits */

  iq_ipr_bits &= ~iq_ipr_clr[iq_ipr_clr_ptr];
  if (iq_ipr_clr[iq_ipr_clr_ptr]) {
    for (i = 0; i < EBOX_QUEUE_ENTRIES; i++) {	
      if (ebox_queue[i] == 0) continue;		
      ebox_queue[i]->ipr_stall &= iq_ipr_bits;
    }
  }
  iq_ipr_clr[iq_ipr_clr_ptr] = 0;
  iq_ipr_clr_ptr = INC(iq_ipr_clr_ptr,IPR_CLR_DELAY);
}


static void ic()
/*
 * *******************************************************************************
 *	 PURPOSE:
 *
 *	 INPUT PARAMETERS:
 *	 OUTPUT PARAMETERS:
 *	 IMPLICIT INPUTS:
 *	 	ebox_queue: An array of integer instructions to schedule
 *	 	fbox_queue: An array of float  instructions to schedule
 *	 IMPLICIT OUTPUTS:
 * 	 FUNCTIONS CALLED DIRECTLY:
 *
 *	 AUTHOR/DATE:
 *	V3.24 FLS Dec-96 EV6 scheduler
 *****************************************************************************
 */
{

    /* process_traps();  not modeled in pvc  */
    /* process_ifills(); not modeled in pvc  */

    if  (sched_code[ic_fetch_ptr] == 0) return;		/* no more instructions in this permutation */

    /* not modeled in pvc  */
/*  if (ic_trap) {
        ic_state = IC_FETCH;
        ic_fetch_ptr = ic_trap_ptr;
        return;
      }
*/
 /* find out which stall is in progress and set ic_state for next time thru here */

  switch (ic_state) {

  case IC_BR_STALL:              /* NOT MODELED IN PVC We're stalled because we need to predict through more branches */
    if (stage_icache[0] != NULL)  {
      stall_reason |= BR_STALL;
      if (flags & FLG$DEBUG_DUMP)     wr (FLG$DEBUG_DUMP,"ic(): cycle: %6d  IC_BR_STALL \n",cycle_count);
      break;
    }
    ic_state = ic_br_stall_return;
    instructions_loaded = ic_fill_stage(0,0);
    break;


  case IC_CMOV_STALL:              /* We stalled because we hit a cmov  NOT IN PVC YET */
    if (stage_icache[0] != NULL)  {
      stall_reason |= CMOV_STALL;
      if (flags & FLG$DEBUG_DUMP)     wr (FLG$DEBUG_DUMP,"ic(): cycle: %6d  IC_MOV_STALL \n",cycle_count);
      break;
    }
    ic_state = ic_cmov_stall_return;
    instructions_loaded  = ic_fill_stage(0,0);					/* fill stage_icache[] with instructions */
    break;

  case IC_FETCH:
    if (stage_icache[0] != NULL)  {	
/*       stall_reason |= FETCH_STALL; not needed since the real stall reason is something liek map_stall etc. */
      if (flags & FLG$DEBUG_DUMP)     wr (FLG$DEBUG_DUMP,"ic(): cycle: %6d  IC_FETCH STALL \n",cycle_count);
      break;
    }
    /* there is code in perf model here to create a bubble to write icache */
    /* there is also code in perf model to do ic_probe = ic_lookup */
    /* pvc bypasses all that and just does the ic_fill_stage */
     instructions_loaded = ic_fill_stage(0,0);
     break;

      case IC_HW_RET_STALL:
	stall_reason |= HW_RET_STALL;
        if (hwret_stall_inum < 0) ic_state = IC_FETCH;
        if (flags & FLG$DEBUG_DUMP)     wr (FLG$DEBUG_DUMP,"ic(): cycle: %6d  HW_RET STALL \n",cycle_count);
        break;
  } /* switch (ic_state).. */

}

static int ic_fill_stage(int start, int start_br_count)
/*
 * *******************************************************************************
 *	 PURPOSE:
 *
 *	 INPUT PARAMETERS:
 *	 OUTPUT PARAMETERS:
 *	 IMPLICIT INPUTS:
 *	 	ebox_queue: An array of integer instructions to schedule
 *	 	fbox_queue: An array of float  instructions to schedule
 *	 IMPLICIT OUTPUTS:
 * 	 FUNCTIONS CALLED DIRECTLY:
 *
 *	 AUTHOR/DATE:
 *	V3.24 FLS Dec-96 EV6 scheduler
 *****************************************************************************
 */
{
   int i;
   INSTR    *busy_entry[ICACHE_WIDTH];
   INSTR    *old_ptr;

	for (i=0; i < ICACHE_WIDTH;i++) {
            if  (sched_code[ic_fetch_ptr] == 0) break;	/* TEMP fix in copy_sched_code  - last entry in raw_code not 
						          filled with slot_only entry 
							*/
	    old_ptr = sched_code[ic_fetch_ptr];   			/*grab just in case hw_ret with stall*/
	    stage_icache[i]= sched_code[ic_fetch_ptr++];		/* get entry from sched_code array */

	    /* if stage_icache[i]->entry_busy i.e been in icache then clone an entry; it will be freed at end of schedule_code*/
	    if (stage_icache[i]->entry_busy) {
		busy_entry[i]= stage_icache[i];				/* save pointer to entry to clone */
    		stage_icache[i] = malloc (( 1) * sizeof (INSTR));	/* allocate INSTR  new entry */
		stage_icache[i]->address= busy_entry[i]->address;
		stage_icache[i]->index= busy_entry[i]->index;
		stage_icache[i]->bits.instr= busy_entry[i]->bits.instr;
		stage_icache[i]->decoded= busy_entry[i]->decoded;
		stage_icache[i]->reached= busy_entry[i]->reached;
		stage_icache[i]->jsr_dest_list= busy_entry[i]->jsr_dest_list;
		stage_icache[i]->bsr_history= busy_entry[i]->bsr_history;
		stage_icache[i]->bsr_stack_clone_list= busy_entry[i]->bsr_stack_clone_list;
		stage_icache[i]->use_bsr_stack= busy_entry[i]->use_bsr_stack;
		stage_icache[i]->PVC_Control_Flag= busy_entry[i]->PVC_Control_Flag;
		stage_icache[i]->use_ca=0;			/* EV6 static slot flag */
		stage_icache[i]->issued=0;			/* EV6 inst has not been issued */
		stage_icache[i]->ipr_stall=0;			/* EV6 ipr scoreboard bits */
        	stage_icache[i]->mra=-1;		        /* EV6 mapped ra  register*/
        	stage_icache[i]->mrb=-1;		        /* EV6 mapped rb register*/
        	stage_icache[i]->mrc=-1;		        /* EV6 mapped  dest_reg  rc or ra*/
        	stage_icache[i]->oldrc=-1;		        /* EV6 mapped  dest_reg  rc or ra*/
		stage_icache[i]->slot_only=0;		 	/* EV6 hack to slot but not execute*/
		stage_icache[i]->entry_busy=1;		 	/* EV6  mark clone busy */
		stage_icache[i]->cloned=1;			/* EV6 mark this entry cloned so will be freed*/
		stage_icache[i]->e_nullify_time=MAXRUNCYCLES;	/* EV6 cycle when nop inst counted as iq_spec_out  */
		stage_icache[i]->f_nullify_time=MAXRUNCYCLES;	/* EV6 cycle when nop inst counted as fq_spec_out  */
		stage_icache[i]->retire_time=0;			/* EV6 cycle when inst will retire */
		stage_icache[i]->fpst_valid=0;			/* EV6 make fbox signal ibox to issue stores */
		stage_icache[i]->inst_num = ++instructions_fetched; /* EV6  pseudo inum */
	    }
	    stage_icache[i]->entry_busy=1;		    	/* set this entry busy for next time here */

	    /* if this is a filler address for pvc slotting purposes only then don't assign an inum */
	    /* and don't check for Hwret_stall, since slot only instructions don't retire.*/
	    if (stage_icache[i]->slot_only == 0) {
		  stage_icache[i]->inst_num = ++instructions_fetched; /* assign inum */
	    	/* if hw_ret with stall bit then tell ic() to stall until it is retired */
	    	  if (get_inst_id(old_ptr) &  Hwret_stall) {		/* if hw_jmp_stall, hw_ret_stall, hw_jsr_stall, or hw_jcr_stall */
	      	    ic_state = IC_HW_RET_STALL;				/* ic_fetch_ptr = itbm_return; in perf model, not in  pvc */
	            hwret_stall_inum = old_ptr->inst_num;		/* store hw_ret stall inum for check in retire() */
	      	    break;
	        }
	     }
	} /* for (i=0; i < ICACHE_WIDTH;i++.. */
return(0);					/* return 0, not used yet */
}

static int copy_sched_code (INSTR *raw_code[])
/*
 ********************************************************************************
 *	 PURPOSE: 
 *		Call fill_sched_code to fill out a fetch block.
 *		The instruction is only used in slot() routine and they
 *		are set sched_code[i]->slot_only=1, so they will not issue.
 *
 *	 INPUT PARAMETERS:
 *	 	i: raw_code offset   - this permutation's instructions
 *	 	j: sched_code offset - schedule_code array 
 *	   offset: fetch block offset 0,4,8 or c.
 *	 OUTPUT PARAMETERS:
 *	 IMPLICIT INPUTS:
 *	 IMPLICIT OUTPUTS:
 *	 AUTHOR/DATE:
 *	V3.24 FLS Jan-97  EV6
 ********************************************************************************
 */
{

  int iold,inew,ibase,i,j,k,m;
  int t0,t1,t2,t3;
  int a0,a1,a2,a3;
  int sched_code_error=FALSE;
  /* copy raw_code[] into sched_code[] filling out fetch blocks from   exe[] */
  /* note the filled in fetch blocks will not have inums assigned and all */
  /* will have ->slot_only=1, so they will not issue (since they are not part of */
  /* the current permutation */

  i=0;    /* raw_code[] ptr */
  j=0;    /* sched_code[] ptr */
  k=0;    /* exe ptr */

  /* match exe[] array to raw_code[] entry point */
  if (!exe_offset_to_entry) {				/* inited in at_pal_entry routine to 0 */
    while (exe[k].decoded) {
        if (exe[k].address == raw_code[i]->address) {
	    exe_offset_to_entry=k;
           break; /* found entry point */
	}
        k++;
    } 
  }
  while (raw_code[i] != 0) {			/* expand raw_code fetch blocks */
    iold=i;
    t0 = raw_code[i]->address & 0xf;
    a0 = raw_code[i]->address & 0xfffffff0;
    	if (raw_code[i+1])  t1 = raw_code[i+1]->address & 0xf; else break;
    		    a1 = raw_code[i+1]->address & 0xfffffff0;
    if (raw_code[i+2])  t2 = raw_code[i+2]->address & 0xf; else break;
    		    a2 = raw_code[i+2]->address & 0xfffffff0;
    if (raw_code[i+3])  t3 = raw_code[i+3]->address & 0xf; else break;
    		    a3 = raw_code[i+3]->address & 0xfffffff0;

    if (a0==a1 && a0==a2 && a0==a3 && t0<t1 && t1<t2 && t2<t3) {		/* case 1 51c0,51c4,51c8,51cc type */
        for (m=0; m < ICACHE_WIDTH; m++) {
            sched_code[j++] = raw_code[i++];	
    	}
    }

   else if ( a0==a1 && a0==a2 && a0!=a3 && t0<t1 && t1<t2 ) {		        /* case 2 51c0,51c4,51c8,xxxx types  */
		if ((t0==0) && (t1==4) && (t2==8)){				/* case 2 */
		    ibase=i;
		    sched_code[j++] = raw_code[i++];	/* copy xxx0 */	
		    sched_code[j++] = raw_code[i++];	/* copy xxx4 */
		    sched_code[j++] = raw_code[i++];	/* copy xxx8 */	
	    	    fill_sched_code (raw_code, ibase,j,0xc); /* get xxxc from exe array */
	            j++;
		}
		else if ((t0==4) && (t1==8) && (t2==0xc)){			/* case 2 */
		        ibase=i;
		        fill_sched_code (raw_code, ibase,j,0); /* get xxx0 from exe array */
	                j++;
		        sched_code[j++] = raw_code[i++];	/* copy xxx4 */
		        sched_code[j++] = raw_code[i++];	/* copy xxx8 */	
		        sched_code[j++] = raw_code[i++];	/* copy xxxc */	
		    }
		else if ((t0==0) && (t1==4) && (t2==0xc)){			/* case 2 */
		        ibase=i;
		        sched_code[j++] = raw_code[i++];	/* copy xxx0 */
		        sched_code[j++] = raw_code[i++];	/* copy xxx4 */
		        fill_sched_code (raw_code, ibase,j,8); /* get xxx8 from exe array */
	                j++;
		        sched_code[j++] = raw_code[i++];	/* copy xxxc */	
		    }
		else if ((t0==0) && (t1==8) && (t2==0xc)){		/* case 2 */
		        ibase=i;
		        sched_code[j++] = raw_code[i++];	/* copy xxx0 */
		        fill_sched_code (raw_code, ibase,j,4); /* get xxx4 from exe array */
	                j++;
		        sched_code[j++] = raw_code[i++];	/* copy xxx8 */	
		        sched_code[j++] = raw_code[i++];	/* copy xxxc */	
		    }
	     }
	else if ( a0==a1 && a0!=a2 && a0!=a3 && t0<t1) {		/* case 3 51c0,51c4,xxxx,xxxx types  */
		if ((t0==0) && (t1==4)){				/* case 3 */
		        ibase=i;
		        sched_code[j++] = raw_code[i++];	/* copy xxx0 */
		        sched_code[j++] = raw_code[i++];	/* copy xxx4 */
		        fill_sched_code (raw_code, ibase,j,8); /* get xxx8 from exe array */
	                j++;
	    	        fill_sched_code (raw_code, ibase,j,0xc); /* get xxxc from exe array */
	                j++;
		    }
		if ((t0==0) && (t1==8)){				/* case 3 */
		        ibase=i;
		        sched_code[j++] = raw_code[i++];	/* copy xxx0 */
		        fill_sched_code (raw_code, ibase,j,4); /* get xxx4 from exe array */
	                j++;
		        sched_code[j++] = raw_code[i++];	/* copy xxx8 */
	    	        fill_sched_code (raw_code, ibase,j,0xc); /* get xxxc from exe array */
	                j++;
		    }
		else if ((t0==4) && (t1==8)){				/* case 3 */
		        ibase=i;
		        fill_sched_code (raw_code, ibase,j,0); /* get xxx0 from exe array */
	                j++;
		        sched_code[j++] = raw_code[i++];	/* copy xxx4 */
		        sched_code[j++] = raw_code[i++];	/* copy xxx8 */	
	    	        fill_sched_code (raw_code, ibase,j,0xc); /* get xxxc from exe array */
	                j++;
		    }
		else if ((t0==0) && (t1==0xc)){				/* case 3 */
		        ibase=i;
		        sched_code[j++] = raw_code[i++];	/* copy xxx0 */
	    	        fill_sched_code (raw_code, ibase,j,4); /* get xxx4 from exe array */
	                j++;
	    	        fill_sched_code (raw_code, ibase,j,8); /* get xxx8 from exe array */
	                j++;
		        sched_code[j++] = raw_code[i++];	/* copy xxxc */	
		    }
		else if ((t0==4) && (t1==0xc)){			/* case 3 */
		        ibase=i;
	    	        fill_sched_code (raw_code, ibase,j,0); /* get xxx0 from exe array */
	                j++;
		        sched_code[j++] = raw_code[i++];	/* copy xxx4 */
	    	        fill_sched_code (raw_code, ibase,j,8); /* get xxx8 from exe array */
	                j++;
		        sched_code[j++] = raw_code[i++];	/* copy xxxc */
		    }
		else if ((t0==8) && (t1==0xc)){			/* case 3  */
		        ibase=i;
	    	        fill_sched_code (raw_code, ibase,j,0); /* get xxx0 from exe array */
	                j++;
	    	        fill_sched_code (raw_code, ibase,j,4); /* get xxx4 from exe array */
	                j++;
		        sched_code[j++] = raw_code[i++];	/* copy xxx8 */
		        sched_code[j++] = raw_code[i++];	/* copy xxxc */	
		    }
	     }
	else 	if ((t0==0)){					/* all other types done 1 at a time  */
		        ibase=i;
		        sched_code[j++] = raw_code[i++];	/* copy xxx0 */
		        fill_sched_code (raw_code, ibase,j,4); /* get xxx4 from exe array */
	                j++;
		        fill_sched_code (raw_code, ibase,j,8); /* get xxx8 from exe array */
	                j++;
	    	        fill_sched_code (raw_code, ibase,j,0xc); /* get xxxc from exe array */
	                j++;
		}
		else if ((t0==4)){		
		            ibase=i;
		            fill_sched_code (raw_code, ibase,j,0); /* get xxx0 from exe array */
	                    j++;
		            sched_code[j++] = raw_code[i++];	/* copy xxx4 */
		            fill_sched_code (raw_code, ibase,j,8); /* get xxx8 from exe array */
	                    j++;
	    	            fill_sched_code (raw_code, ibase,j,0xc); /* get xxxc from exe array */
	                    j++;
		}
		else if ((t0==8)){		
  		            ibase=i;
		            fill_sched_code (raw_code, ibase,j,0); /* get xxx0 from exe array */
	                    j++;
		            fill_sched_code (raw_code, ibase,j,4); /* get xxx4 from exe array */
	                    j++;
		            sched_code[j++] = raw_code[i++];	/* copy xxx8 */
	    	            fill_sched_code (raw_code, ibase,j,0xc); /* get xxxc from exe array */
	                    j++;
		}
		else if ((t0==0xc)){		
		            ibase=i;
		            fill_sched_code (raw_code, ibase,j,0); /* get xxx0 from exe array */
	                    j++;
		            fill_sched_code (raw_code, ibase,j,4); /* get xxx4 from exe array */
	                    j++;
		            fill_sched_code (raw_code, ibase,j,8); /* get xxx8 from exe array */
	                    j++;
		            sched_code[j++] = raw_code[i++];	/* copy xxxc */
		}
	     inew=i;
	     if (inew == iold) {

		    printf("  %%PVC_ERROR internal error in EV6.C->copy_sched_code routine \n");
	            printf("    Unknown fetch block address sequence. \n");
	            printf("    a0= %X  a1= %X a2= %X a3= %X \n",a0,a1,a2,a3);
	            printf("    t0= %X  t1= %X t2= %X t3= %X \n",t0,t1,t2,t3);
    		    for (m=0; m < ICACHE_WIDTH; m++) {
		        sched_code[j++] = raw_code[i++];	/* copy error fetch block to allow continue */
		    }
	     }

} /* while (raw_code[i].. */
    /* for now, will not check for branches in the last raw_code entry.. */
    for (m=0; m < ICACHE_WIDTH; m++) {
        if (raw_code[i+m] == 0) 
	      sched_code[j+m]=0;
        else  {
	      sched_code[j+m]=raw_code[i+m];sched_code[j+1+m]=0;
	      }

    }
 return(1);
}


static int fill_sched_code (INSTR *raw_code[], int i, int j, int offset )
/*
 ********************************************************************************
 *	 PURPOSE: 
 *		Fill sched_code with an instruction to fill out a fetch block.
 *		The instruction is only used in slot() routine and they
 *		are set sched_code[i]->slot_only=1, so they will not issue.
 *
 *	 INPUT PARAMETERS:
 *	 	i: raw_code offset   - this permutation's instructions
 *	 	j: sched_code offset - schedule_code array 
 *	   offset: fetch block offset 0,4,8 or c.
 *	 OUTPUT PARAMETERS:
 *	 IMPLICIT INPUTS:
 *	 IMPLICIT OUTPUTS:
 *	 AUTHOR/DATE:
 *	V3.24 FLS Jan-97  EV6
 ********************************************************************************
 */
{

   int k,l,m;
   
   int found_entry = FALSE;
    k=exe_offset_to_entry;
    while (exe[k].decoded) {
	l = (exe[k].address & 0xFFFFFFFF);						/* stop NT warning */
        m = ((raw_code[i]->address & 0xFFFFFFF0) | offset);
	if (l == m) {
	   found_entry=TRUE;
	   break; /* found address match */
	}
	k++;
    }
    /* if not found, we must put a dummy address in to point to halt */
    /* this can happen due to branches  */
    /* This address will only be seen in slot routine, since ->slot_only=1 */

    /* clone an INSTR entry and copy entry from exe array */	   
    sched_code[j] = malloc (( 1) * sizeof (INSTR));
    if (found_entry==FALSE) {
    	    sched_code[j]->address = 0+offset;			/* fake address 0,4,8 or c */
    	    sched_code[j]->index = DO_EPICODE;			/* halt is a call_pal aka epicode */
	    sched_code[j]->bits.instr = 0;
	    sched_code[j]->decoded = 0;
	    sched_code[j]->reached= 0;
	    sched_code[j]->jsr_dest_list= 0;
	    sched_code[j]->bsr_history= 0;
	    sched_code[j]->bsr_stack_clone_list= 0;
	    sched_code[j]->use_bsr_stack= 0;
	    sched_code[j]->PVC_Control_Flag= 0;

    } else {
    	    sched_code[j]->address = exe[k].address;
    	    sched_code[j]->index = exe[k].index;
	    sched_code[j]->bits.instr = exe[k].bits.instr;
	    sched_code[j]->decoded = exe[k].decoded;
	    sched_code[j]->reached= exe[k].reached;
	    sched_code[j]->jsr_dest_list= exe[k].jsr_dest_list;
	    sched_code[j]->bsr_history= exe[k].bsr_history;
	    sched_code[j]->bsr_stack_clone_list= exe[k].bsr_stack_clone_list;
	    sched_code[j]->use_bsr_stack= exe[k].use_bsr_stack;
	    sched_code[j]->PVC_Control_Flag= exe[k].PVC_Control_Flag;
	   }
    sched_code[j]->use_ca=0;			/* EV6 static slot flag */
    sched_code[j]->issued=1;			/* EV6 just to be safe */
    sched_code[j]->ipr_stall=0;			/* EV6 ipr scoreboard bits */
    sched_code[j]->mra=-1;    	        	/* EV6 mapped ra  register*/
    sched_code[j]->mrb=-1;		        /* EV6 mapped rb register*/
    sched_code[j]->mrc=-1;		        /* EV6 mapped  dest_reg  rc or ra*/
    sched_code[j]->oldrc=-1;		        /* EV6 mapped  dest_reg  rc or ra*/
    sched_code[j]->slot_only=1;			/* and mark it for slotting only; it will not execute */
    sched_code[j]->entry_busy = 0;		/* v3.25 mark it not busy since not in icache yet*/
    sched_code[j]->cloned=1;			/* EV6 mark this entry cloned*/
    sched_code[j]->retire_time=-1;		/* EV6 mark retired just in case */
    sched_code[j]->fpst_valid=0;			/* EV6 make fbox signal ibox to issue stores */
    sched_code[j]->inst_num=0;			/* EV6 inum */
 return(1);
}

/* pack and remove  0 entries from stage */
static void squeeze_stage(INSTR *stage[], int width)
{
  int i,j;
  INSTR *t[4];
  for ( i=0 ; i < width ; i++ ) t[i]=0;

  for ( i=0,j=0 ; i < width ; i++ )  {
    if (stage[i] != 0) t[j++] = stage[i];
  }
  for ( i=0 ; i < width ; i++ ) stage[i]=t[i];
}


/* remove entries==0 from bottom */
static void pack_stage(INSTR *stage[], int width)
{
  int i,j;

  for ( i = 0 ; i < width ; i++ )  {
    if ( stage[i] != 0 ) break;
  }
  if ( i == 0 ) return;
  for ( j = 0 ; i < width ; j++,i++ )  {
    if (stage[i] == 0) break;
    stage[j] = stage[i];
    stage[i] = 0;
  }
}


static int slot()
/*
 ********************************************************************************
 *	PURPOSE:
 *	    EV6 slot stage. 
 *	      1. Selects which ebox integer pipes (L0,L1,U0,U1)
 *		are allowed  for the four instructions in this fetch 
 *		block (aka fetch line). Essentially maps the instruction
 *		mix in the fetch line with the allowable pipes and puts
 *		in 1's where cluster A=(L0,L1) pipes are to be selected.
 *
  *	INPUT PARAMETERS:
 *		sched_code: pointer to the array of instructions from follow_code
 *		offs: where we are in the above array
 *		length: number of entries in sched_code[] array
 *
 *	OUTPUT PARAMETERS:
 *		sched_code[offs]->use_ca: Returns 1's where pipes allowed to issue from lower cluster.
 *				bit 0 = 0 if fetch line 0 issue in Upper cluster B (U0,U1) only
 *				bit 0 = 1 if fetch line 0 issue in lower cluster A (L0,L1) only 
 *				bit 1 = 0 if fetch line 1 issue in Upper cluster B (U0,U1) only
 *				bit 1 = 1 if fetch line 1 issue in lower cluster A (L0,L1) only 
 *				bit 2 = 0 if fetch line 2 issue in Upper cluster B (U0,U1) only
 *				bit 2 = 1 if fetch line 2 issue in lower cluster A (L0,L1) only 
 *				bit 3 = 0 if fetch line 3 issue in Upper cluster B (U0,U1) only
 *				bit 3 = 1 if fetch line 3 issue in lower cluster A (L0,L1) only 
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:
 *		        mxpr_pipe
 *	AUTHOR/DATE:
 *	V3.24 FLS Dec-96 - Modified for EV6 slotting rules
 ********************************************************************************
 */
{
  int index, class;
  int i,k;
  int table_idx=0;
  int valid_pipes;
  int a_only;
  int b_only;
  int t0,t1;

    if (stage_icache[0] != 0) {
     if (stage_slot[0] == 0 ) {		/* will not be 0 if map stage stall */

      for ( i= 0 ; i < SLOT_WIDTH ; i++ )  {
	if ( stage_icache[i] == 0 )  break;		/* if fewer than 4 inst in fetch_block then break */
	 stage_slot[i]= stage_icache[i];
	 stage_icache[i]=0;
	 index = stage_slot[i]->index;
	 if (index > MAX_INDEX || index <= 0) {		/* v3.24 */
		printf("	%%PVC_ERROR in EV6.C->slot routine  - instr_array index out of range. \n");
		printf("        index: %d  stage_slot[i] i= %d\n",index,i);
		printf("        Probably due to illegal 21264 instruction.\n");
		printf("        Illegal instruction is at palcode address : %X\n",stage_slot[i]->address);
		exit(0);
	 } 

	class = instr_array[index]->imp->di_class;

	/* Collect integer subcluster requirements */
	if (E_ARB_2) {	
	    if ((class == qi_mfpr) || (class == qi_mtpr))
		valid_pipes = mxpr_pipe (stage_slot[i]);	/* MxPRs use diff. pipes depending on the IPR */
	    else 
		valid_pipes = issue_pipe[class];		/* other instructions only depend on class */

	    a_only =  ((valid_pipes & PIPE_L0) || (valid_pipes & PIPE_L1)) &&
		     !((valid_pipes & PIPE_U0) || (valid_pipes & PIPE_U1));
	    b_only = !((valid_pipes & PIPE_L0) || (valid_pipes & PIPE_L1)) &&
		      ((valid_pipes & PIPE_U0) || (valid_pipes & PIPE_U1));
	    table_idx = table_idx | ((((b_only & 1) << 1) | (a_only & 1)) << (i<<1));
	}
	 /* fixup in case not fetch block - pvc does not spec execute - branches fixed
	  * for each permutation
	  */
	 k = SLOT_WIDTH;						/* check for all inst in one fetch line */
	 if ((i < SLOT_WIDTH-1) && (stage_slot[i+1] != 0)) {
		  t0 = stage_slot[i]->address   & 0xFFFFFFF0;
	          t1 = stage_slot[i+1]->address & 0xFFFFFFF0;
		  if (t0 != t1)  { 
			k = i+1; 
			break;
		  }
	}
      } /* for ( i= 0 ; i < SLOT_WIDTH... */

      /* Compute subcluster (A = L0,L1 and B = U0,U1) assignments */
	if (E_ARB_2) { 						
	    for (i = 0; i < k; i++) {			
		  if (stage_slot[i] == 0) break;
		  stage_slot[i]->use_ca = subcluster_table[table_idx] & (1<<i);
		  if (stage_slot[i]->slot_only) stage_slot[i]=0;	/* hack after slot delete slot only */
	    }
	}
     }/* if (stage_slot[0] == 0.. */
    } /* if (stage_icache[0] != 0.... */

   squeeze_stage(stage_slot,SLOT_WIDTH);		/* hack, must be done due to slot_only cleanup above */
    pack_stage(stage_icache,ICACHE_WIDTH); /* pack */

return(TRUE);
}


static int issue_inst (INSTR *fbox_or_ebox_queue[], int from_fbox, int j, int c0, int c1, int issued_this_cycle,int cycle_count)
/*
 ********************************************************************************
 *	PURPOSE:
 *		to implement the same logic that is in the EV6 issue stage
 *	INPUT PARAMETERS:
 *		fbox_or_ebox_queue[]: pointer to the array of instructions from follow_code
 *			    if  from_fbox then fbox_or_ebox_queue = fbox_queue
 *			    if !from_fbox then fbox_or_ebox_queue = ebox_queue
 *		j: the current offset into the fbox_or_ebox_queue[] array that were working on
 *		cycle_count: the current cycle count
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCITONS CALLED DIRECTLY:
 *		print_cycle_ev6
 *		
 *	AUTHOR/DATE:
 *	        Greg Coladonato, Aug 92
 *		Paul Rubinfeld, January 1993 - modified module
 *	V3.24 FLS Nov-96 Modified for EV6
 ********************************************************************************
 */
{
    int i, index;
    int class;
    int inst_with_dest_reg;
    int rega,regb;

if (issued_this_cycle==0) {
 if (cycle_count < MAXRUNCYCLES) {
    /* allocate mem for runtime[cycle_count]->is1 etc */
    runtime[cycle_count] = Mem_Alloc_CYCLE (fbox_or_ebox_queue[j], 0);
    runtime[cycle_count]->is1 = 0;
    runtime[cycle_count]->is2 = 0;
    runtime[cycle_count]->is3 = 0;
    runtime[cycle_count]->is4 = 0;
 } else {
        printf("	%%PVC_ERROR in EV6.C->issue_inst routine \n");
        printf("        cycle count (%d) exceeds MAXRUNCYCLES limit.  Increase limit.\n",cycle_count);
        exit(1); 
        }
}

	    switch (issued_this_cycle) {
	         case 0: runtime[cycle_count]->is1 = fbox_or_ebox_queue[j];	break;
	         case 1: runtime[cycle_count]->is2 = fbox_or_ebox_queue[j];	break;
	         case 2: runtime[cycle_count]->is3 = fbox_or_ebox_queue[j];	break;
	         case 3: runtime[cycle_count]->is4 = fbox_or_ebox_queue[j];	break;
	    }  /* end switch */




    index = fbox_or_ebox_queue[j]->index;
    class = instr_array[index]->imp->di_class;
    inst_with_dest_reg = (instr_array[index]->arch->dest_reg != Noreg) || (class == qi_lda) ||
		      (class == qi_mfpr) || (class == qi_mtpr) || (index == DO_MB);

    /* reduce perf model memory.c mem_operation and process_lds_and_sts() routine to just
     * set the latency in ->mrc to the cycle count when ld dest_reg clean.
     */
    /* update register dependency and cross ebox regfile write dependency scoreboard */	
  if ((inst_with_dest_reg || index == DO_BR || index == DO_JSR) && 
      (fbox_or_ebox_queue[j]->mrc != -1) &&
      !(class == qi_fdiv)) {
    reg_dirty[fbox_or_ebox_queue[j]->mrc].d0 = cycle_count +  get_operate_latency(fbox_or_ebox_queue[j])  + c1;
    reg_dirty[fbox_or_ebox_queue[j]->mrc].d1 = cycle_count +  get_operate_latency(fbox_or_ebox_queue[j])  + c0;
  }	  
  else if ((class == qi_ild) || (class == qi_fld)) {
        if (fbox_or_ebox_queue[j]->mrc >= 0) {
          if (fbox_or_ebox_queue[j]->mrc < MAX_EREG) {
	    reg_dirty[fbox_or_ebox_queue[j]->mrc].d0 = cycle_count + get_operate_latency(fbox_or_ebox_queue[j]);
	    reg_dirty[fbox_or_ebox_queue[j]->mrc].d1 = cycle_count + get_operate_latency(fbox_or_ebox_queue[j]);
	    if (reg_dirty[fbox_or_ebox_queue[j]->mrc].free_when_clean) IREG_FREE(fbox_or_ebox_queue[j]->mrc,0);
          }
          else {
	       reg_dirty[fbox_or_ebox_queue[j]->mrc].d0 = cycle_count + get_operate_latency(fbox_or_ebox_queue[j]);
	       reg_dirty[fbox_or_ebox_queue[j]->mrc].d1 = cycle_count + get_operate_latency(fbox_or_ebox_queue[j]);
	       if (reg_dirty[fbox_or_ebox_queue[j]->mrc].free_when_clean) FREG_FREE(fbox_or_ebox_queue[j]->mrc,0);
               }
        }
      }

  /* fls - not close to perf model which only stores retire_lateny if a stx? */
  fbox_or_ebox_queue[j]->retire_time = cycle_count + get_retire_latency(fbox_or_ebox_queue[j]);

  fbox_or_ebox_queue[j]->fpst_valid = cycle_count + FBOX_STORE_DELAY;

  if ( class == qi_mtpr ) {   /* if mtpr Clear IPR scoreboard bits */	
    if (IPR_LS_BITS(fbox_or_ebox_queue[j]->bits.instr)) {
      for (i = 0; i < IPR_LS_BIT_NUM; i++) {
	if (iq_ipr_inum[i] == fbox_or_ebox_queue[j]->inst_num) {
	  iq_ipr_clr[ADD(iq_ipr_clr_ptr,IPR_CLR_DELAY-1,IPR_CLR_DELAY)] |= 1<<i;
	}
      }
    }
  }

  /* clean out this inst register dependent bits in case it was issued after bit set in this cycle */
  /* this can happen - see cluster_dirt = ... in reg_stage */
  /* if this was the only dependent register must clear bit in stall_reason */
  if ( fbox_or_ebox_queue[j]->mra != -1 ) {
        regb = fbox_or_ebox_queue[j]->bits.op_format.ra;
	dependent_reg_mask &= !(1 << rega);			/* clear bit of inst issued this cycle */
  }
  if ( fbox_or_ebox_queue[j]->mrb != -1 ) {
        regb = fbox_or_ebox_queue[j]->bits.op_format.rb;
	dependent_reg_mask &= !(1 << regb);			/* clear bit of inst issued this cycle */
  }

  if (dependent_reg_mask == 0 ) {
	stall_reason = (~REG_DEPEND & stall_reason); /* clear reg_depend bit */
  }

  /* Remove inst from ebox_queue or fbox_queue */
  /* Does not model load hit spec issue discussed in ev6 spec section 2.6 Special cases. */
  if (from_fbox==FROM_FBOX) {					/* fbox_queue = null */
      fbox_or_ebox_queue[j] = NULL;
      fq_out[1]++;
  }
  else {
      fbox_or_ebox_queue[j] = NULL;				/* ebox_queue = null */
      iq_out[1]++;
  }


return(1);
}


static void pack_queue(INSTR *queue[], int depth, int *q_ptr, int *count, int *bad_count, int width )
{
  int src,dest;

  src = dest = 0;
  *count = 0;


  while ( src < depth ) {
    if ( queue[src] != NULL ) (*count)++;
    if ( queue[src] == NULL ) src++;
    else if ( src != dest ) {  
      queue[dest++] = queue[src];
      queue[src++] = NULL;
    }
    else src++;
    if ( queue[dest] != NULL ) dest++;
    if ( (src - dest) > width) dest++; 
  }
  *q_ptr = dest;
}


static int get_operate_latency (INSTR *i)
/*
 ********************************************************************************
 *	PURPOSE:
 *		Get the instruction latency of EV6 instructions including
 *		MF instructions. These are the minimum latencies to produce a
 *		register result. They do not include any retire latencies. For pvc
 *		purposes only MT (MF?) retire latencies will be modeled.
 *		For EV6  IBOX and Mbox IPRs have a latency of 3
 *			 EBOX latency=1, CBOX latency not known?
 *		Since there are no more enums avail to create a Cbox_ipr,
 *		and the Cbox is only written on power up and it is 
 *		not defined in the spec will leave the latency same as
 *		ebox iprs (1).
 *	INPUT PARAMETERS:
 *		i: The instruction in question
 *	OUTPUT PARAMETERS:
 *		the latency in cycles.			
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	V3.24 FLS Nov-96 Modified for EV6
 ********************************************************************************
 */
{
      CLASS_INFO *class_def = instr_array[i->index];
      int	class;
      int	id,ipr_num;
      int       latency_in_cycles;	/* latency to produce a register result */
      /*
       * The latency for non-mfpr instructions is fixed in table
       */
      class = class_def->imp->di_class;
      /* 
       * The latency for Hw_mfpr instructions
       * depends on if Ebox/Ibox/Mbox/Cbox .
       */
      if (class != qi_mfpr)
          latency_in_cycles = operate_latency[class];
      else {
	    ipr_num = i->bits.ev6_hwmxpr_format.index;	/* V3.24 */
	    id = get_ipr_id (ipr_num);
	    if (ipr_num == EV6__DATA)
		latency_in_cycles = 3;		/* Cbox CSR ipr (there is no Cbox_ipr enum) */
	    else {
		if ((id & Mbox_ipr) || (id & Ibox_ipr))
			latency_in_cycles = 3;		/* Ibox and Mbox */
		else    latency_in_cycles = 1;		/* Ebox */
	        }       
	  }
    return (latency_in_cycles);
}      


static int get_retire_latency (INSTR *i)
/*
 ********************************************************************************
 *	PURPOSE:
 *		Get the retire latency of EV6 instructions. Note that
 *		this is the latency from the reg_stage (stage 4).
 *		The spec lists the retire STAGE of instruction in section 2.28,
 *		not latencies.
 *	INPUT PARAMETERS:
 *		i: The instruction in question
 *	OUTPUT PARAMETERS:
 *		the latency in cycles.			
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	V3.24 FLS Feb-97 for EV6
 ********************************************************************************
 */
{
      CLASS_INFO *class_def = instr_array[i->index];
      int	class;
      int       ret_latency_in_cycles;	/* latency from reg() routine */
      /*
       * The latency for non fpdiv/sqrt instructions is fixed in table
       */
      class = class_def->imp->di_class;

      /* Change latency for fp div/sqrt FLS - FOR NOW JUST PUT IN a number fx - fix */
      if ((class != qi_fdiv) && (class != qi_fsqrt))
          ret_latency_in_cycles = retire_latency[class];
      else {
		ret_latency_in_cycles = FDIV_T_LATENCY;		/* FLS - fix this per sqrt fdiv latencies */
	  }
    return (ret_latency_in_cycles);
}      


int get_inst_id (INSTR *i)
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
 *		V3.24 - example of resources is scbd0|scbd1|MT which
 *		is used in check_address_restrictions routine.
 *	If index=0 then return -1.
 *
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Greg Coladonato, Aug 92
 *	V3.27 FLS May-97 Don't include add,sub, and mull as arith_trap 
 *			 inst unless /v bit set.
 *	V3.24 FLS Sep-96 Modified for EV6
 *			 Not all id's are stored here, some are
 *			 are stored in the check_ev6_restriction_xx routines.
 *			 The ones that affect more than one routine are
 *			 usually put here.
 ********************************************************************************
 */
{
      int id = 0, temp, class;

      if (!i) return (-1);
      class = instr_array[i->index]->imp->di_class;

      switch (i->index) 
	{
	    case DO_HW_RET:				/* V3.24 */
	      {
		    temp = i->bits.instr & 0X2000;	/* V3.24 ev6stall=bit13 */
		    switch (temp)
		      {
			case 0X2000:			/* V3.24 ev6stall=bit13 */
			    id =  Hwret_stall;		/* code relies on this being == Hwret_stall alone */
			    break;
			default:
			    id =  Hwret;
		      }
		    break;
	      }

	    case DO_HW_MTPR:
	      {
		    int ipr_num;
		    ipr_num = i->bits.ev6_hwmxpr_format.index;	/* V3.24 */
		    id = Mt | get_ipr_id (ipr_num);
	    	    id |= i->bits.ev6_hwmxpr_format.scbd ; 	/* add in instr. scoreboard bits*/
		    break;
	      }
	    case DO_HW_MFPR:
	      {
		    int ipr_num;
		    ipr_num = i->bits.ev6_hwmxpr_format.index;	/* V3.24 */
		    id = Mf | get_ipr_id (ipr_num);
	    	    id |= i->bits.ev6_hwmxpr_format.scbd ; 	/* add in instr. scoreboard bits*/
		    break;
	      }
	    case DO_LDL:	case DO_LDQ:
	    case DO_LDQU:	case DO_LDD:
	    case DO_LDS:	case DO_LDT:	case DO_LDF: 
	      {
		    id =  Memory_op | Virtual_inst | Read;  /* V3.24 */
		    break;
	      }
	    case DO_LDLL:
  	    case DO_LDQL:
	      {
		    id =  Memory_op | Virtual_inst | Read | Lock_inst;  /* V3.24 */
		    break;
	      }

	    case DO_STB:	case DO_STW:		/*V3.28 */
	    case DO_STL:	case DO_STQ:
	    case DO_STQU:	case DO_STD:
	    case DO_STS:	case DO_STF:	case DO_STT: 
	      {
		    id =  Memory_op | Virtual_inst | Write; /* V3.24 */
		    break;
	      }
	    case DO_STLC:
	    case DO_STQC:	
	      {
		    id =  Memory_op | Virtual_inst | Write | Lock_inst; /* V3.24 */
		    break;
	      }
	    case DO_HW_LD: 	/* V3.24 not considering physical as memory op, for restriction #12*/
	      {
		    if ((i->bits.ev6_hwld_format.type == 0) || (i->bits.ev6_hwld_format.type == 1))
		          id =  Physical_inst | Read;
		    else  id =  Memory_op | Virtual_inst  | Read ; 

		    if (i->bits.ev6_hwld_format.type == 1) 
		              id =  id | Lock_inst;
		    break;
	      }
	    case DO_HW_ST: 	/* V3.24 not considering physical as memory op, for restriction #12*/
	      {
		    if ((i->bits.ev6_hwst_format.type == 0) || 	(i->bits.ev6_hwst_format.type == 1))
		          id =  Physical_inst | Write;
		    else  id =  Memory_op | Virtual_inst | Write ;

		    if (i->bits.ev6_hwst_format.type == 1) 
		              id =  id | Lock_inst;

		    break;
	      }

	    case DO_ADDLV:			/* V3.27 */
	    case DO_ADDQV:			/* V3.27 */
	    case DO_SUBLV:			/* V3.27 */
	    case DO_SUBQV:			/* V3.27 */
	    case DO_MULLV:			/* V3.27 */
	    case DO_MULQV:			/* V3.27 */
	      {
		    id =  id | Arith_trap;	/* for restriction #12, #17 */
		    break;
	      }
	   default:
		    break;

	}

	if ((class==qi_fbr) || (class==qi_ibr) || (class==qi_jsr))
		id =  id | Br;			

	if ((class==qi_fbr) || (class==qi_ibr))
		id =  id  | Br_cond;			

	if ((class==qi_ild)  || (class==qi_fld) ||
	    (class==qi_ist)  || (class==qi_fst))
		   id =  id | Memory_op;			

	if ((class==qi_fadd) || (class==qi_fmul) || 
	    (class==qi_fsqrt)|| (class==qi_fdiv))
		   id =  id | Arith_trap;	/* for restriction #12, #17 */

	if ((class==qi_fst) || (class==qi_itof))
		id =  id  | Stf_itof; /* for check_ev6_restriction_7 */

      return (id);
}



static int is_fp_inst (INSTR *i)
/*
 ********************************************************************************
 *	PURPOSE:
 *		Check if EV6 floating point instruction.
 *
 *	INPUT PARAMETERS:
 *		i: the instruction
 *	OUTPUT PARAMETERS:
 *
 *		If index=0 then return -1.
 *
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	V3.24 FLS Dec-96 EV6
 ********************************************************************************
 */
{
      int id = 0, class;

      if (!i) return (-1);
      class = instr_array[i->index]->imp->di_class;

      if   ((class==qi_fbr) || (class==qi_fadd) ||
	    (class==qi_fst) || (class==qi_ftoi) ||
	    (class==qi_fmul) || (class==qi_fld) ||
	    (class==qi_mx_fpcr) || 
	    (class==qi_fsqrt)|| (class==qi_fdiv))

	     return(TRUE);				/* float inst */
	else return(FALSE);				/* integer inst */
}



static int is_ps_write (INSTR *inst)
/*
 ********************************************************************************
 *	PURPOSE:
 *		NOT USED BY Ev6, kept in case needed.
 *              This function takes an index and returns true if it is
 *		a pal shadow register write.
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
 	    case DO_LDQ:	case DO_LDQL:	case DO_LDQU:
	    case DO_LDB:	case DO_LDW:
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
	    case DO_AMASK: 	/* V3.24 */
	    case DO_IMPLVER: 	/* V3.24 */
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
 *		NOT USED BY Ev6, kept in case needed by EV7.
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


int get_ipr_id (int num)
/*
 ********************************************************************************
 *	PURPOSE:
 *		If ibox ipr return Ibox_ipr else if mbox ipr return Mbox_ipr.
 *		
 *	INPUT PARAMETERS:
 *		num: the value of the 12 bit ipr field.		     
 *	OUTPUT PARAMETERS:
 *		a staticly defined constat used to refer to that ipr.
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Greg Coladonato, Aug 92
 *	V3.24 FLS Aug 96 - Modified for EV6 
 *			   It would have been nice to add in cbox_ipr and ebox_ipr,
 *			   but we are currently limited to total of 32 bit enums.
 *			   it would have taken too much work to expand it.
 *      V3.26 FLS Apr 97 - removed scbd bits since bits can be or'd in with
 *			   the hw_mfpr and hw_mtpr instructions so you have to check
 *			   the instructions. get_inst_id returns scbd bits now.
 ********************************************************************************
 */
{
      int ipr;
      
      switch (num)
	{
     /* EBOX IPRs */
      case EV6__CC:		ipr = 0; 	break; /* V3.24 */
      case EV6__CC_CTL:		ipr = 0; 	break; /* V3.24 */
      case EV6__VA:		ipr = 0;	break;	/* V3.24*/
      case EV6__VA_FORM:	ipr = 0;	break;	/* V3.24*/
      case EV6__VA_CTL:		ipr = 0;	break;	/* V3.24*/
      /* IBOX IPRs */
      case EV6__ITB_TAG:	ipr = Ibox_ipr;	break;	/* V3.24*/
      case EV6__ITB_PTE:	ipr = Ibox_ipr;	break;	/* V3.24*/
      case EV6__ITB_IAP:	ipr = Ibox_ipr;	break;	/* V3.24*/
      case EV6__ITB_IA:		ipr = Ibox_ipr;	break;	/* V3.24*/
      case EV6__ITB_IS:		ipr = Ibox_ipr;	break;	/* V3.24*/
      case EV6__EXC_ADDR:	ipr = Ibox_ipr;	break;	/* V3.24*/
      case EV6__IVA_FORM:	ipr = Ibox_ipr;	break;	/* V3.24*/
      case EV6__PS:		ipr = Ibox_ipr;	break;	/* V3.24*/
      case EV6__IER_CM:		ipr = Ibox_ipr;	break;	/* V3.24*/
      case EV6__IER:		ipr = Ibox_ipr;	break;	/* V3.24*/
      case EV6__SIRR:		ipr = Ibox_ipr;	break;	/* V3.24*/
      case EV6__ISUM:		ipr = Ibox_ipr;	break;	/* V3.24*/
      case EV6__HW_INT_CLR:	ipr = Ibox_ipr;	break;	/* V3.24*/
      case EV6__EXC_SUM:	ipr = Ibox_ipr;	break;	/* V3.24*/
      case EV6__PAL_BASE:	ipr = Ibox_ipr;	break;	/* V3.24*/
      case EV6__I_CTL:		ipr = Ibox_ipr;	break;	/* V3.24*/
      case EV6__IC_FLUSH:	ipr = Ibox_ipr;	break;	/* V3.24*/
      case EV6__IC_FLUSH_ASM:	ipr = Ibox_ipr;	break;	/* V3.24*/
      case EV6__PCTR_CTL:	ipr = Ibox_ipr;	break;	/* V3.24*/
      case EV6__CLR_MAP:	ipr = Ibox_ipr;	break;	/* V3.24*/
      case EV6__SLEEP:		ipr = Ibox_ipr;	break;	/* V3.24*/
      case EV6__I_STAT:		ipr = Ibox_ipr;	break;	/* V3.24*/
      case EV6__PROCESS_CONTEXT: ipr = Ibox_ipr; break;	/* V3.24*/
      case EV6__ASN:		ipr = Ibox_ipr;  break;	/* V3.24*/
      case EV6__ASTER:		ipr = Ibox_ipr; break;	/* V3.24*/
      case EV6__ASTRR:		ipr = Ibox_ipr;	break;	/* V3.24*/
      case EV6__PPCE:		ipr = Ibox_ipr; break;	/* V3.24*/
      case EV6__FPE:		ipr = Ibox_ipr;	break;	/* V3.24*/
     /* MBOX IPRs */
      case EV6__DTB_TAG0:	ipr = Mbox_ipr; break;	/* V3.24*/
      case EV6__DTB_TAG1:	ipr = Mbox_ipr; break;	/* V3.24*/
      case EV6__DTB_PTE0:	ipr = Mbox_ipr;	break;	/* V3.24*/
      case EV6__DTB_PTE1:	ipr = Mbox_ipr;	break;	/* V3.24*/
      case EV6__DTB_IAP:	ipr = Mbox_ipr;	break;	/* V3.24*/
      case EV6__DTB_IA:		ipr = Mbox_ipr;	break;	/* V3.24*/
      case EV6__DTB_IS0:	ipr = Mbox_ipr;	break;	/* V3.24*/
      case EV6__DTB_IS1:	ipr = Mbox_ipr;	break;	/* V3.24*/
      case EV6__DTB_ASN0:	ipr = Mbox_ipr;	break;	/* V3.24*/
      case EV6__DTB_ASN1:	ipr = Mbox_ipr;	break;	/* V3.24*/
      case EV6__DTB_ALT_MODE:	ipr = Mbox_ipr;	break;	/* V3.24*/
      case EV6__MM_STAT:	ipr = Mbox_ipr;	break;	/* V3.24*/
      case EV6__M_CTL:		ipr = Mbox_ipr;	break;	/* V3.24*/
      case EV6__DC_CTL:		ipr = Mbox_ipr;	break;	/* V3.24*/
      case EV6__DC_STAT:	ipr = Mbox_ipr;	break;	/* V3.24*/
     /* CBOX IPRs */
      case EV6__DATA:		ipr = 0;	break;	/* V3.24*/
      case EV6__SHIFT_CONTROL:	ipr = 0;	break;	/* V3.24*/
      default:			ipr = 0;         break;	  
	}
      return (ipr);
}






static void print_cycle_ev6 (int org, int i, int stall, int stall_code, int from_box)
/*
 *******************************************************************************
 *	PURPOSE:
 *	        This small function prints out the instructions in one cycle.
 *	INPUT PARAMETERS:
 *	        i: an index into the runtime[] array so we know which cycle to print
 *	        org: a value which designates what information is being printed.
 *		from_fbox: 1 if called for fbox
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:	wr
 *	AUTHOR/DATE:
 *	        Greg Coladonato, December 1991
 *	V3.24 FLS Nov-96 Modified for EV6
 ********************************************************************************
 */
{

    int mask,temp;
    int m,reg_no;		/* offset into reg_name_table */
    char *is1_decoded;		/* for Hw_mxpf without scbd<7:0>=... */
    char *is2_decoded;
    char *is3_decoded;
    char *is4_decoded;

     is1_decoded = calloc (64,sizeof(char));
     is2_decoded = calloc (64,sizeof(char));
     is3_decoded = calloc (64,sizeof(char));
     is4_decoded = calloc (64,sizeof(char));

    if (i >= MAXRUNCYCLES) {
	printf("  %%PVC_ERROR in EV6.C->print_cycle_ev6 routine \n");
	printf("  cycle count (%d) exceeds MAXRUNCYCLES limit.  Increase limit.\n",i);
	exit(1); 
    }
    if (stall == STALL) {
	if (from_box == FROM_EBOX) {
	   if (stall_code == 0) 	
             wr (org, "Cycle: %3d               ..Possible ebox Stall (no instructions in ebox queue )\n", i);
	   if (stall_code == ISSUED_INST) 	
             wr (org, "Cycle: %3d               ..Possible ebox Stall (All slotted instructions are issued)\n", i);
	   if (stall_code & MAP_STALL) 	
             wr (org, "Cycle: %3d               ..Possible ebox Stall (map stage stall)\n", i);
	   if (stall_code & IQ_STALL) 	
             wr (org, "Cycle: %3d               ..Possible ebox Stall (Integer Queue (IQ) stall)\n", i);
	   if (stall_code & IPR_STALL) 	
             wr (org, "Cycle: %3d               ..Possible ebox Stall (hw_mtpr or hw_mfpr ipr dependency)\n", i);
	   if (stall_code & REG_DEPEND) 	
		  if (dependent_reg_mask) {
                     wr (org, "Cycle: %3d               ..Possible ebox Stall (register dependency ", i);
			temp = dependent_reg_mask;
			for (m=0; m < 32; m++) {
			   reg_no =m;
			    if (temp & 0x1) {
              		        wr (org, "%s,", &reg_name_table[reg_no][0]);	/* convert reg number to ascii */
			    }
      			temp = temp >> 1;
			}
                     wr (org, ")\n", i);
		  }
	   if (stall_code & CLUSTER_BUSY) 	
             wr (org, "Cycle: %3d               ..Possible ebox Stall (cluster 0 or 1 busy cross delay busy)\n", i);
	   if (stall_code & EBOX_WRITE_PORT_BUSY) 	
             wr (org, "Cycle: %3d               ..Possible ebox Stall (register file write port busy)\n", i);
	   if (stall_code & PIPE_BUSY) { 	
             wr (org, "Cycle: %3d               ..Possible ebox Stall (Pipeline ", i);
	            if (stall_code & PIPE_L0_BUSY) 	wr (org, "l0 ");
	 	    if (stall_code & PIPE_U0_BUSY) 	wr (org, "u0 ");
	 	    if (stall_code & PIPE_L1_BUSY) 	wr (org, "l1 ");
		    if (stall_code & PIPE_U1_BUSY) 	wr (org, "u1 ");
	            wr (org, "busy)\n");
	   }
	   mask = PIPE_BUSY | IPR_STALL | EBOX_WRITE_PORT_BUSY | CLUSTER_BUSY | REG_DEPEND |
		  MAP_STALL | IQ_STALL | ISSUED_INST  ;
	   if (stall_code == HW_RET_STALL) 	
             wr (org, "Cycle: %3d               ..Stall (hw_jmp_stall, hw_ret_stall, hw_jsr_stall, or hw_jcr_stall)\n", i);
	   else if (stall_code == RETIRE_WAIT) 	
             wr (org, "Cycle: %3d               ..waiting for instructions to retire\n", i);
	   else if (stall_code & mask == 0  &&  stall_code !=0 )
                 wr (org, "Cycle: %3d               ..unknown ebox stall (reason code = %8X)\n",i,stall_code);
       }
       else if ((from_box==FROM_FBOX) && stall_code != ISSUED_INST) {			
	        if (stall_code == 0) 	
                    wr (org, "Cycle: %3d               ..Possible fbox Stall (no instructions in fbox queue )\n", i);
	       else  if (stall_code & FQ_STALL) 	
             		 wr (org, "Cycle: %3d               ..Possible fbox Stall (Floating Point Queue (FQ) stall)\n", i);
	       else if (stall_code & REG_DEPEND) 	
		      if (dependent_reg_mask) {
                         wr (org, "Cycle: %3d               ..Possible ebox Stall (register dependency ", i);
			    temp = dependent_reg_mask;
			    for (m=0; m < 32; m++) {
			       reg_no =m;
			        if (temp & 0x1) {
              		        wr (org, "%s,", &reg_name_table[reg_no][0]);	/* convert reg number to ascii */
			    }
      			temp = temp >> 1;
			}
                      wr (org, ")\n", i);
		     }
	        else if (stall_code & PIPE_BUSY ){
	                  wr (org, "Cycle: %3d               ..Possible fbox Stall (", i);
	                  wr (org, "Pipeline ");
		          if (stall_code & PIPE_FM_BUSY) 	wr (org, "fm ");
		          if (stall_code & PIPE_FA_BUSY) 	wr (org, "fa ");
		          if (stall_code & PIPE_FST0_BUSY) 	wr (org, "fst0 ");
		          if (stall_code & PIPE_FST1_BUSY) 	wr (org, "fst1 ");
	                  wr (org, "busy");
	       		  wr (org, ")\n");
	            }
	       else wr (org, "unknown fbox stall reason  (reason code = %8X",stall_code);
            }
    }else {
          /* if hw_mxpr then fixup printout to strip scbd<7:0>=nnnn to allow printing 2 inst per line */
          if (runtime[i] != 0) {
        	if (runtime[i]->is1 != 0) {
          	    if (runtime[i]->is1->bits.op_format.opcode == EVX$OPC_HW_MTPR) {
        		    sprintf(is1_decoded,"%-12s", "HW_MTPR");
              		    sprintf(&is1_decoded[strlen(is1_decoded)],"%s, ",
        	            &reg_name_table[runtime[i]->is1->bits.op_format.rb][0]);
              		    if (runtime[i]->is1->bits.ev6_hwmxpr_format.index <= MAX_IPR)
                  		    sprintf(&is1_decoded[strlen(is1_decoded)],"%s",
            	          	   &ipr_name_table[runtime[i]->is1->bits.ev6_hwmxpr_format.index][0]);
        
                }else  if (runtime[i]->is1->bits.op_format.opcode == EVX$OPC_HW_MFPR) {
	        	        sprintf(is1_decoded,"%-12s", "HW_MFPR");
          		        sprintf(&is1_decoded[strlen(is1_decoded)],"%s, ",
        	      		&reg_name_table[runtime[i]->is1->bits.op_format.ra][0]);
          			if (runtime[i]->is1->bits.ev6_hwmxpr_format.index <= MAX_IPR)
                  			sprintf(&is1_decoded[strlen(is1_decoded)],"%s",
        	          		&ipr_name_table[runtime[i]->is1->bits.ev6_hwmxpr_format.index][0]);
                        } else strcpy (is1_decoded, runtime[i]->is1->decoded);
          } /* if (runtime[i]->is1 != 0... */

	  if (runtime[i]->is2 != 0) {
      	    if (runtime[i]->is2->bits.op_format.opcode == EVX$OPC_HW_MTPR) {
		    sprintf(is2_decoded,"%-12s", "HW_MTPR");
      		    sprintf(&is2_decoded[strlen(is2_decoded)],"%s, ",
	            &reg_name_table[runtime[i]->is2->bits.op_format.rb][0]);
      		    if (runtime[i]->is2->bits.ev6_hwmxpr_format.index <= MAX_IPR)
          		    sprintf(&is2_decoded[strlen(is2_decoded)],"%s",
	          	   &ipr_name_table[runtime[i]->is2->bits.ev6_hwmxpr_format.index][0]);

            }else  if (runtime[i]->is2->bits.op_format.opcode == EVX$OPC_HW_MFPR) {
		        sprintf(is2_decoded,"%-12s", "HW_MFPR");
      		        sprintf(&is2_decoded[strlen(is2_decoded)],"%s, ",
	      		&reg_name_table[runtime[i]->is2->bits.op_format.ra][0]);
      			if (runtime[i]->is2->bits.ev6_hwmxpr_format.index <= MAX_IPR)
          			sprintf(&is2_decoded[strlen(is2_decoded)],"%s",
	          		&ipr_name_table[runtime[i]->is2->bits.ev6_hwmxpr_format.index][0]);
                    } else strcpy (is2_decoded, runtime[i]->is2->decoded);
	  } /* if (runtime[i]->is2 != 0... */

	  if (runtime[i]->is3 != 0) {
      	    if (runtime[i]->is3->bits.op_format.opcode == EVX$OPC_HW_MTPR) {
		    sprintf(is3_decoded,"%-12s", "HW_MTPR");
      		    sprintf(&is3_decoded[strlen(is3_decoded)],"%s, ",
	            &reg_name_table[runtime[i]->is3->bits.op_format.rb][0]);
      		    if (runtime[i]->is3->bits.ev6_hwmxpr_format.index <= MAX_IPR)
          		    sprintf(&is3_decoded[strlen(is3_decoded)],"%s",
	          	   &ipr_name_table[runtime[i]->is3->bits.ev6_hwmxpr_format.index][0]);

            }else  if (runtime[i]->is3->bits.op_format.opcode == EVX$OPC_HW_MFPR) {
		        sprintf(is3_decoded,"%-12s", "HW_MFPR");
      		        sprintf(&is3_decoded[strlen(is3_decoded)],"%s, ",
	      		&reg_name_table[runtime[i]->is3->bits.op_format.ra][0]);
      			if (runtime[i]->is3->bits.ev6_hwmxpr_format.index <= MAX_IPR)
          			sprintf(&is3_decoded[strlen(is3_decoded)],"%s",
	          		&ipr_name_table[runtime[i]->is3->bits.ev6_hwmxpr_format.index][0]);
                    } else strcpy (is3_decoded, runtime[i]->is3->decoded);
	  } /* if (runtime[i]->is3 != 0... */

	  if (runtime[i]->is4 != 0) {
      	    if (runtime[i]->is4->bits.op_format.opcode == EVX$OPC_HW_MTPR) {
		    sprintf(is4_decoded,"%-12s", "HW_MTPR");
      		    sprintf(&is4_decoded[strlen(is4_decoded)],"%s, ",
	            &reg_name_table[runtime[i]->is4->bits.op_format.rb][0]);
      		    if (runtime[i]->is4->bits.ev6_hwmxpr_format.index <= MAX_IPR)
          		    sprintf(&is4_decoded[strlen(is4_decoded)],"%s",
	          	   &ipr_name_table[runtime[i]->is4->bits.ev6_hwmxpr_format.index][0]);

            }else  if (runtime[i]->is4->bits.op_format.opcode == EVX$OPC_HW_MFPR) {
		        sprintf(is4_decoded,"%-12s", "HW_MFPR");
      		        sprintf(&is4_decoded[strlen(is4_decoded)],"%s, ",
	      		&reg_name_table[runtime[i]->is4->bits.op_format.ra][0]);
      			if (runtime[i]->is4->bits.ev6_hwmxpr_format.index <= MAX_IPR)
          			sprintf(&is4_decoded[strlen(is4_decoded)],"%s",
	          		&ipr_name_table[runtime[i]->is4->bits.ev6_hwmxpr_format.index][0]);
                    } else strcpy (is4_decoded, runtime[i]->is4->decoded);
	  } /* if (runtime[i]->is4 != 0... */
        } /* if (runtime[i] != 0... */

    /* replace instruction in is2_decoded etc for possible resources like pipelines busy */
    /* start at is2_decoded , since if is1_decoded 0 then stall cycle , which is printed above */
    if (runtime[i]->is2 == 0) {
	if (from_box==FROM_EBOX) {
	 if (stall_code == 0)
		 sprintf(is2_decoded, "..no instructions in ebox queue");
	 else if (stall_code == NO_MORE_INSTRUCTIONS)
		 sprintf(is2_decoded, "..no more instructions to issue");
	 else if (stall_code == ISSUED_INST)
		 sprintf(is2_decoded, "..all slotted instructions issued");
	 else if (stall_code & MAP_STALL) 
		  sprintf(is2_decoded, "..map stage busy");
	 else if (stall_code & IQ_STALL) 
		  sprintf(is2_decoded, "..IQ full busy");
	 else if (stall_code & IPR_STALL )	
		  sprintf(is2_decoded, "..hw_mfpr or hw_mtpr ipr dependency");
	 else if (stall_code & REG_DEPEND) {
		  sprintf(is2_decoded, "..register dependency");
		  if (dependent_reg_mask) {
		      sprintf(&is2_decoded[strlen(is2_decoded)]," (");
			temp = dependent_reg_mask;
			for (m=0; m < 32; m++) {
			   reg_no =m;
			    if (temp & 0x1) {
              		        sprintf(&is2_decoded[strlen(is2_decoded)],"%s,",
        	                    &reg_name_table[reg_no][0]);				/* convert reg number to ascii */
			    }
      			temp = temp >> 1;
			}
		      sprintf(&is2_decoded[strlen(is2_decoded)],")");
		  }
	      }
	 else if (stall_code & CLUSTER_BUSY) 
		  sprintf(is2_decoded, "..regfile cross delay busy");
	 else if (stall_code & EBOX_WRITE_PORT_BUSY) 
		 sprintf(is2_decoded, "..regfile write port busy");
	 else if ((stall_code & PIPE_BUSY) && (stall_code & PIPE_L0_BUSY) &&
		  (stall_code & PIPE_U0_BUSY) && (stall_code & PIPE_L1_BUSY) && 
		  (stall_code & PIPE_U1_BUSY))
		 sprintf(is2_decoded, "..pipe l0 u0 l1 u1 busy");
	 else if (stall_code & PIPE_BUSY ){
		  sprintf(is2_decoded,"..pipe ");
	          if (stall_code & PIPE_L0_BUSY) 
			sprintf(&is2_decoded[strlen(is2_decoded)],"l0 ");
	 	  if (stall_code & PIPE_U0_BUSY)   
			sprintf(&is2_decoded[strlen(is2_decoded)],"u0 ");
	 	  if (stall_code & PIPE_L1_BUSY)
			sprintf(&is2_decoded[strlen(is2_decoded)],"l1 ");
		  if (stall_code & PIPE_U1_BUSY) 
			sprintf(&is2_decoded[strlen(is2_decoded)],"u1 ");
		  sprintf(&is2_decoded[strlen(is2_decoded)],"busy");
	      }
	 else if (stall_code & PIPE_L0_NOT_ALLOWED ){		/* assumes PIPE_L1_NOT_ALLOWED also */
		   sprintf(is2_decoded,"..pipes ");
		   sprintf(&is2_decoded[strlen(is2_decoded)],"l0,l1 ");
		   sprintf(&is2_decoded[strlen(is2_decoded)],"not allowed");
	      }
	 else if (stall_code & PIPE_U0_NOT_ALLOWED ){		/* assumes PIPE_U1_NOT_ALLOWED also */
		   sprintf(is2_decoded,"..pipes ");
		   sprintf(&is2_decoded[strlen(is2_decoded)],"u0,u1 ");
		   sprintf(&is2_decoded[strlen(is2_decoded)],"not allowed");
	      }
	 else if (stall_code & HW_RET_STALL) 
		 sprintf(is2_decoded, " ");			/* stall message will follow so don't print busy msg */
	 else sprintf(is2_decoded, "..unknown busy reason code = %8X",stall_code);

	} 
	else if (from_box==FROM_FBOX) {
	 	if (stall_code == 0)
		 	sprintf(is2_decoded, "..no instructions in fbox queue");
	 	else if (stall_code == NO_MORE_INSTRUCTIONS)
		 	sprintf(is2_decoded, "..no more instructions to issue");
                else if (stall_code & FQ_STALL) {
		           sprintf(&is2_decoded[strlen(is2_decoded)],"..FQ full busy ");
	             }
	        else if (stall_code & REG_DEPEND) {
		      sprintf(is2_decoded, "..register dependency");
		      if (dependent_reg_mask) {
		          sprintf(&is2_decoded[strlen(is2_decoded)]," (");
			    temp = dependent_reg_mask;
			    for (m=0; m < 32; m++) {
			       reg_no =m;
			        if (temp & 0x1) {
              		            sprintf(&is2_decoded[strlen(is2_decoded)],"%s,",
        	                        &reg_name_table[reg_no][0]);				/* convert reg number to ascii */
			        }
      			    temp = temp >> 1;
			    }
		          sprintf(&is2_decoded[strlen(is2_decoded)],")");
		      }
	             }
	        else if (stall_code & PIPE_BUSY ){
		        sprintf(is2_decoded,"..pipe ");
	              if (stall_code & PIPE_FM_BUSY) 
			  sprintf(&is2_decoded[strlen(is2_decoded)],"fm ");
	 	      if (stall_code & PIPE_FA_BUSY)
			  sprintf(&is2_decoded[strlen(is2_decoded)],"fa ");
		      if (stall_code & PIPE_FST0_BUSY) 
			  sprintf(&is2_decoded[strlen(is2_decoded)],"fst0 ");
		      if (stall_code & PIPE_FST1_BUSY) 
		          sprintf(&is2_decoded[strlen(is2_decoded)],"fst1 ");
		      sprintf(&is2_decoded[strlen(is2_decoded)],"busy");
	         }

		 else sprintf(is2_decoded, "..unknown fbox busy reason code = %8X",stall_code);
	}
    } /*     if (runtime[i]->is2 == 0.. */

    if (runtime[i]->is3 == 0) {
	if (from_box==FROM_EBOX) {
	 if (stall_code == 0)
		 sprintf(is3_decoded, "..no instructions in ebox queue");
	 else if (stall_code == NO_MORE_INSTRUCTIONS)
		 sprintf(is3_decoded, "..no more instructions to issue");
	 else if (stall_code == ISSUED_INST)
		 sprintf(is3_decoded, "..all slotted instructions issued");
	 else if (stall_code & MAP_STALL) 
		  sprintf(is3_decoded, "..map stage busy");
	 else if (stall_code & IQ_STALL) 
		  sprintf(is3_decoded, "..IQ full busy");
	 else if (stall_code & IPR_STALL )	
		  sprintf(is3_decoded, "..hw_mfpr or hw_mtpr ipr dependency");
	 else if (stall_code & REG_DEPEND) {
		  sprintf(is3_decoded, "..register dependency");
		  if (dependent_reg_mask) {
		      sprintf(&is3_decoded[strlen(is3_decoded)]," (");
			temp = dependent_reg_mask;
			for (m=0; m < 32; m++) {
			   reg_no =m;
			    if (temp & 0x1) {
              		        sprintf(&is3_decoded[strlen(is3_decoded)],"%s,",
        	                    &reg_name_table[reg_no][0]);				/* convert reg number to ascii */
			    }
      			temp = temp >> 1;
			}
		      sprintf(&is3_decoded[strlen(is3_decoded)],")");
		  }
	      }
	 else if (stall_code & CLUSTER_BUSY) 
		  sprintf(is3_decoded, "..regfile cross delay busy");
	 else if (stall_code & EBOX_WRITE_PORT_BUSY) 
		 sprintf(is3_decoded, "..regfile write port busy");
	 else if ((stall_code & PIPE_BUSY) && (stall_code & PIPE_L0_BUSY) &&
		  (stall_code & PIPE_U0_BUSY) && (stall_code & PIPE_L1_BUSY) && 
		  (stall_code & PIPE_U1_BUSY))
		 sprintf(is3_decoded, "..pipe l0 u0 l1 u1 busy");
	 else if (stall_code & PIPE_BUSY ){
		  sprintf(is3_decoded,"..pipe ");
	          if (stall_code & PIPE_L0_BUSY) 
			sprintf(&is3_decoded[strlen(is3_decoded)],"l0 ");
	 	  if (stall_code & PIPE_U0_BUSY)   
			sprintf(&is3_decoded[strlen(is3_decoded)],"u0 ");
	 	  if (stall_code & PIPE_L1_BUSY)
			sprintf(&is3_decoded[strlen(is3_decoded)],"l1 ");
		  if (stall_code & PIPE_U1_BUSY) 
			sprintf(&is3_decoded[strlen(is3_decoded)],"u1 ");
		  sprintf(&is3_decoded[strlen(is3_decoded)],"busy");
	      }
	 else if (stall_code & PIPE_L0_NOT_ALLOWED ){		/* assumes PIPE_L1_NOT_ALLOWED also */
		   sprintf(is3_decoded,"..pipes ");
		   sprintf(&is3_decoded[strlen(is3_decoded)],"l0,l1 ");
		   sprintf(&is3_decoded[strlen(is3_decoded)],"not allowed");
	      }
	 else if (stall_code & PIPE_U0_NOT_ALLOWED ){		/* assumes PIPE_U1_NOT_ALLOWED also */
		   sprintf(is3_decoded,"..pipes ");
		   sprintf(&is3_decoded[strlen(is3_decoded)],"u0,u1 ");
		   sprintf(&is3_decoded[strlen(is3_decoded)],"not allowed");
	      }
	 else if (stall_code & HW_RET_STALL) 
		 sprintf(is3_decoded, " ");			/* stall message will follow so don't print busy msg */
	 else sprintf(is3_decoded, "..unknown busy reason code = %8X",stall_code);
	} 
    } /*     if (runtime[i]->is3 == 0.. */

    if (runtime[i]->is4 == 0) {
	if (from_box==FROM_EBOX) {
	 if (stall_code == 0)
		 sprintf(is4_decoded, "..no instructions in ebox queue");
	 else if (stall_code == NO_MORE_INSTRUCTIONS)
		 sprintf(is4_decoded, "..no more instructions to issue");
	 else if (stall_code == ISSUED_INST)
		 sprintf(is4_decoded, "..all slotted instructions issued");
	 else if (stall_code & MAP_STALL) 
		  sprintf(is4_decoded, "..map stage busy");
	 else if (stall_code & IQ_STALL) 
		  sprintf(is4_decoded, "..IQ full busy");
	 else if (stall_code & IPR_STALL )	
		  sprintf(is4_decoded, "..hw_mfpr or hw_mtpr ipr dependency");
	 else if (stall_code & REG_DEPEND) {
		  sprintf(is4_decoded, "..register dependency");
		  if (dependent_reg_mask) {
		      sprintf(&is4_decoded[strlen(is4_decoded)]," (");
			temp = dependent_reg_mask;
			for (m=0; m < 32; m++) {
			   reg_no =m;
			    if (temp & 0x1) {
              		        sprintf(&is4_decoded[strlen(is4_decoded)],"%s,",
        	                    &reg_name_table[reg_no][0]);				/* convert reg number to ascii */
			    }
      			temp = temp >> 1;
			}
		      sprintf(&is4_decoded[strlen(is4_decoded)],")");
		  }
	      }
	 else if (stall_code & CLUSTER_BUSY) 
		  sprintf(is4_decoded, "..regfile cross delay busy");
	 else if (stall_code & EBOX_WRITE_PORT_BUSY) 
		 sprintf(is4_decoded, "..regfile write port busy");
	 else if ((stall_code & PIPE_BUSY) && (stall_code & PIPE_L0_BUSY) &&
		  (stall_code & PIPE_U0_BUSY) && (stall_code & PIPE_L1_BUSY) && 
		  (stall_code & PIPE_U1_BUSY))
		 sprintf(is4_decoded, "..pipe l0 u0 l1 u1 busy");
	 else if (stall_code & PIPE_BUSY ){
		  sprintf(is4_decoded,"..pipe ");
	          if (stall_code & PIPE_L0_BUSY) 
			sprintf(&is4_decoded[strlen(is4_decoded)],"l0 ");
	 	  if (stall_code & PIPE_U0_BUSY)   
			sprintf(&is4_decoded[strlen(is4_decoded)],"u0 ");
	 	  if (stall_code & PIPE_L1_BUSY)
			sprintf(&is4_decoded[strlen(is4_decoded)],"l1 ");
		  if (stall_code & PIPE_U1_BUSY) 
			sprintf(&is4_decoded[strlen(is4_decoded)],"u1 ");
		  sprintf(&is4_decoded[strlen(is4_decoded)],"busy");
	      }
	 else if (stall_code & PIPE_L0_NOT_ALLOWED ){		/* assumes PIPE_L1_NOT_ALLOWED also */
		   sprintf(is4_decoded,"..pipes ");
		   sprintf(&is4_decoded[strlen(is4_decoded)],"l0,l1 ");
		   sprintf(&is4_decoded[strlen(is4_decoded)],"not allowed");
	      }
	 else if (stall_code & PIPE_U0_NOT_ALLOWED ){		/* assumes PIPE_U1_NOT_ALLOWED also */
		   sprintf(is4_decoded,"..pipes ");
		   sprintf(&is4_decoded[strlen(is4_decoded)],"u0,u1 ");
		   sprintf(&is4_decoded[strlen(is4_decoded)],"not allowed");
	      }
	 else if (stall_code & HW_RET_STALL) 
		 sprintf(is4_decoded, " ");			/* stall message will follow so don't print busy msg */
	 else sprintf(is4_decoded, "..unknown busy reason code = %8X",stall_code);
	} /* if (from_box==FROM_EBOX.. */
    } /*     if (runtime[i]->is4 == 0.. */


    /* print out up to four instructions 2 per line */
    /* Ebox issues up to 4 inst per cycle */
    /* Fbox issues up to 2 inst per cycle */
    if (runtime[i]->is1 != 0) {
	 wr (org, "Cycle: %3d Addr %4X: %-30.40s", i, runtime[i]->is1->address, is1_decoded);
   	 if (runtime[i]->is2 != 0) {
	     wr (org, "\t\t Addr %4X: %-.40s\n", runtime[i]->is2->address, is2_decoded);
	     if (from_box==FROM_EBOX) {
	       if ( runtime[i]->is3 != 0) {
	           wr (org, "Cycle: %3d Addr %4X: %-30.40s", i, runtime[i]->is3->address, is3_decoded);

                   if (runtime[i]->is4 != 0) {
	               wr (org, "\t\t Addr %4X: %-.40s\n", runtime[i]->is4->address, is4_decoded);
	           } else  				/* can't issue 4th inst. this cycle */
	               wr (org, "\t\t %-.80s\n", is4_decoded);

	      } else 					/* can't issue 3rd nor 4th  inst */
		   if (is3_decoded != 0) { 
	               wr (org, "Cycle: %3d %-42.82s\n", i,is3_decoded);
		   }
	     } /* if (from_box==FROM_EBOX.. */

	 } else  					/* can't issue 2nd,3rd nor 4th  inst */
		  if (is2_decoded != 0) {
	                wr (org, "\t\t %-.80s\n", is2_decoded);
	            	if (from_box==FROM_EBOX)  
			     wr (org, "Cycle: %3d\n", i);
		  } 
    } 

/* needed for guideline 6 	runtime[i]=0;	deallocate with a hammer */
	free (is1_decoded);
	free (is2_decoded);
	free (is3_decoded);
	free (is4_decoded);
        }/* if (stall == STALL)... */

}



static void init_restrictions (void)
/*
 ********************************************************************************
 *	PURPOSE: 
 *		NOT USED BY EV6
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
 *V3.24 FLS Sep-1996 Modified for EV6. change MF to HW_MFPTR and MT to HW_MTPR
 ********************************************************************************
 */
{
      extern union INSTR_FORMAT instruction;
      extern char  *text;
      extern B64   *pc;
      
      if (instruction.op_format.opcode == EVX$OPC_HW_MTPR) {	/*V3.24*/
		sprintf(text,"%-12s", "HW_MTPR");
      		sprintf(&text[strlen(text)],"%s, ",
	      		&reg_name_table[instruction.op_format.rb][0]);

      }else if (instruction.op_format.opcode == EVX$OPC_HW_MFPR) {
		sprintf(text,"%-12s", "HW_MFPR");
      		sprintf(&text[strlen(text)],"%s, ",
	      		&reg_name_table[instruction.op_format.ra][0]);
            }    
      
/* v3.24 - add check for ipr index gtr than max_ipr to avoid out of bounds*/
      if (instruction.ev6_hwmxpr_format.index <= MAX_IPR)	/* V3.24 */
          sprintf(&text[strlen(text)],"%s",
	          &ipr_name_table[instruction.ev6_hwmxpr_format.index][0]); /* V3.24 */

      sprintf(&text[strlen(text)],";	scbd<7:0>= ");
      if (instruction.ev6_hwmxpr_format.scbd & 1<<7)
	   sprintf(&text[strlen(text)],"1");
      else sprintf(&text[strlen(text)],"0");
      if (instruction.ev6_hwmxpr_format.scbd & 1<<6)
	   sprintf(&text[strlen(text)],"1");
      else sprintf(&text[strlen(text)],"0");
      if (instruction.ev6_hwmxpr_format.scbd & 1<<5)
	   sprintf(&text[strlen(text)],"1");
      else sprintf(&text[strlen(text)],"0");
      if (instruction.ev6_hwmxpr_format.scbd & 1<<4)
	   sprintf(&text[strlen(text)],"1");
      else sprintf(&text[strlen(text)],"0");

      sprintf(&text[strlen(text)]," ");

      if (instruction.ev6_hwmxpr_format.scbd & 1<<3)
	   sprintf(&text[strlen(text)],"1");
      else sprintf(&text[strlen(text)],"0");
      if (instruction.ev6_hwmxpr_format.scbd & 1<<2)
	   sprintf(&text[strlen(text)],"1");
      else sprintf(&text[strlen(text)],"0");
      if (instruction.ev6_hwmxpr_format.scbd & 1<<1)
	   sprintf(&text[strlen(text)],"1");
      else sprintf(&text[strlen(text)],"0");
      if (instruction.ev6_hwmxpr_format.scbd & 1<<0)
	   sprintf(&text[strlen(text)],"1");
      else sprintf(&text[strlen(text)],"0");
      return;
}




static void decode_hw_memory (disp_table *entry)
/*
 ********************************************************************************
 *	PURPOSE:
 *		To allow each chip to have its own function for decoding 
 *		palcode memory reference instructions HW_LD and HW_ST.
 *		Includes HW_RET which is not a mmeory format instruction,
 *		but saves adding a new chip specific routine.
 *	INPUT PARAMETERS:
 *		entry: instruction dispatch table 
 *			.decode = pointer to this chip specific routine
 *			.text   = "ST" or "LD" or "HW_REI"
 *				   (modified here to HW_ST and HW_LD)
 *			.action   = idx = DO_HW_LD or DO_HW_ST
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *    V3.24 FLS Feb 97 Modified for EV6. matched hw_ld to HAL assembler 
 *		       except upper pvc is in upper case
 ********************************************************************************
 */
{
      extern union INSTR_FORMAT instruction;
      extern char  *text;
      extern B64   *pc;
      int type;      


      /* decode HW_LD */    
      if (instruction.op_format.opcode == EVX$OPC_HW_LD) {
    
	  sprintf(text,"%s", "HW_LD");
          if(instruction.ev6_hwld_format.len == 1) 
	       sprintf(&text[strlen(text)],"Q");
          else sprintf(&text[strlen(text)],"L");

          type = instruction.ev6_hwld_format.type;
          switch (type) {
              case 0: sprintf(&text[strlen(text)],"P    ");
        	      break;
              case 1: sprintf(&text[strlen(text)],"PL   ");
        	      break;
              case 2: sprintf(&text[strlen(text)],"V    ");
        	      break;
              case 3: sprintf(&text[strlen(text)],"     ");
        	      break;
              case 4: sprintf(&text[strlen(text)],"     ");
	              break;
              case 5: sprintf(&text[strlen(text)],"w    ");
        	      break;
              case 6: sprintf(&text[strlen(text)],"A    ");
        	      break;
              case 7: sprintf(&text[strlen(text)],"WA   ");
        	      break;
          }
 	  sprintf(&text[strlen(text)],"%s, %d(%s)",
        	&reg_name_table[instruction.op_format.ra][0],
        	instruction.hwmem_format.disp,
        	&reg_name_table[instruction.op_format.rb][0]);

    	     /* decode HW_ST */
      } else if (instruction.op_format.opcode == EVX$OPC_HW_ST) {

	  sprintf(text,"%s", "HW_ST");
          if(instruction.ev6_hwld_format.len == 1) 
	       sprintf(&text[strlen(text)],"Q");
          else sprintf(&text[strlen(text)],"L");

          type = instruction.ev6_hwld_format.type;
          switch (type) {
              case 0: sprintf(&text[strlen(text)],"P    ");
        	      break;
              case 1: sprintf(&text[strlen(text)],"PC   ");
        	      break;
              case 2: sprintf(&text[strlen(text)],"     ");
        	      break;
              case 3: sprintf(&text[strlen(text)],"     ");
        	      break;
              case 4: sprintf(&text[strlen(text)],"     ");
	              break;
              case 5: sprintf(&text[strlen(text)],"     ");
        	      break;
              case 6: sprintf(&text[strlen(text)],"A    ");
        	      break;
              case 7: sprintf(&text[strlen(text)],"     ");
        	      break;
          }
 	  sprintf(&text[strlen(text)],"%s, %d(%s)",
        	&reg_name_table[instruction.op_format.ra][0],
        	instruction.hwmem_format.disp,
        	&reg_name_table[instruction.op_format.rb][0]);

      } else if (instruction.op_format.opcode == EVX$OPC_HW_RET) {
	  
	  sprintf(text,"%s", "HW_");
          type = instruction.ev6_hwret_format.hint;
          switch (type) {
              case 0: sprintf(&text[strlen(text)],"JMP");
        	      break;
              case 1: sprintf(&text[strlen(text)],"JSR");
        	      break;
              case 2: sprintf(&text[strlen(text)],"RET");
        	      break;
              case 3: sprintf(&text[strlen(text)],"JCR");
        	      break;
              case 4: sprintf(&text[strlen(text)],"???");
	              break;
              case 5: sprintf(&text[strlen(text)],"???");
        	      break;
              case 6: sprintf(&text[strlen(text)],"???");
        	      break;
              case 7: sprintf(&text[strlen(text)],"???");
        	      break;
          }
          if(instruction.ev6_hwret_format.stall) 
              sprintf(&text[strlen(text)],"_STALL ");
	  else sprintf(&text[strlen(text)],"       ");

          sprintf(&text[strlen(text)],"(");
 	  sprintf(&text[strlen(text)],"%s",
        	&reg_name_table[instruction.op_format.rb][0]);
          sprintf(&text[strlen(text)],")");

      }
      return;    
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
 *	V3.24 FLS Nov-96 Modified for EV6
 ********************************************************************************
 */
{
      int ebox_pipe=0;
      int id,ipr_num;
      /* 
       * The HW_MXPR instructions go down different pipes
       * depending on the IPR. 
       */
      ipr_num = i->bits.ev6_hwmxpr_format.index;	/* V3.24 */
      id = get_ipr_id (ipr_num);

      if (id & Ibox_ipr) 
	     ebox_pipe = PIPE_L0;				/* all ibox iprs use the L0 ebox pipe */
      else   
	     switch (ipr_num) {					/* else check Ebox,Mbox,Cbox, individually */

		case EV6__DTB_TAG0:
		case EV6__DTB_PTE0:
		case EV6__DTB_IS0:
		case EV6__DTB_ASN0:
		case EV6__DTB_ALT_MODE:
		case EV6__MM_STAT:
		case EV6__M_CTL:
		case EV6__DC_CTL:
		case EV6__DC_STAT:
		case EV6__DATA:
		case EV6__SHIFT_CONTROL:
				  ebox_pipe = PIPE_L0; 
				  break; 
		case EV6__CC_CTL:
		case EV6__VA:
		case EV6__VA_FORM:
		case EV6__VA_CTL:
      		case EV6__CC:
		case EV6__DTB_TAG1:
		case EV6__DTB_PTE1:
		case EV6__DTB_IAP:
		case EV6__DTB_IA:
		case EV6__DTB_IS1:
		case EV6__DTB_ASN1:
				  ebox_pipe = PIPE_L1; 
				  break; 
		default:
	   		{
	      		wr (FLG$ERR, "%s\n", pheader);
	      		wr (FLG$ERR, "***\nError executing instruction %s at address %X \n", 
			    i->decoded, i->address);
	      		wr (FLG$ERR, "%%PVC_ERROR in EV6.C->mxpr_pipe Illegal IPR number \n***\n");
	      		pal_error_count++;
	   		}
	     } /* switch (ipr_num)...*/

      return (ebox_pipe);
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
  int i;

  for (i = 0; i < EBOX_WIDTH; i++) e_write_port[i] = 0;

  /* Fill in EBOX arb priority */
  e_arb_order[C0A_ARB-1] = 0;
  e_arb_order[C0B_ARB-1] = 1;
  if (EBOX_WIDTH > 2) e_arb_order[C1A_ARB-1] = 2;
  if (EBOX_WIDTH > 3) e_arb_order[C1B_ARB-1] = 3;

  /* fill issue_pipe array with pipes the instructions can be slotted to */
      issue_pipe[qi_ild] = 	(PIPE_L0 | PIPE_L1);
      issue_pipe[qi_ist] = 	(PIPE_L0 | PIPE_L1);
      issue_pipe[qi_fld] = 	(PIPE_L0 | PIPE_L1);
      issue_pipe[qi_fst] = 	(PIPE_L0 | PIPE_L1 | PIPE_FST0 | PIPE_FST1);
      issue_pipe[qi_lda] = 	(PIPE_L0 | PIPE_L1 | PIPE_U0 | PIPE_U1);
      issue_pipe[qi_mem_misc] =	(PIPE_L1);
      issue_pipe[qi_rpcc] =	(PIPE_L1);
      issue_pipe[qi_ibr] =	(PIPE_U0 | PIPE_U1);
      issue_pipe[qi_jsr] =	(PIPE_L0);	
      issue_pipe[qi_iadd] =	(PIPE_L0 | PIPE_L1 | PIPE_U0 | PIPE_U1);
      issue_pipe[qi_ilog] =	(PIPE_L0 | PIPE_L1 | PIPE_U0 | PIPE_U1);
      issue_pipe[qi_ishf] =	(PIPE_U0 | PIPE_U1);
      issue_pipe[qi_cmov] =	(PIPE_L0 | PIPE_L1 | PIPE_U0 | PIPE_U1);
      issue_pipe[qi_cmov1] =	(PIPE_L0 | PIPE_L1 | PIPE_U0 | PIPE_U1);
      issue_pipe[qi_cmov2] =	(PIPE_L0 | PIPE_L1 | PIPE_U0 | PIPE_U1);
      issue_pipe[qi_imul] =	(PIPE_U1);
      issue_pipe[qi_imisc] =	(PIPE_U0);
      issue_pipe[qi_fbr] =	(PIPE_FA);
      issue_pipe[qi_fadd] =	(PIPE_FA);
      issue_pipe[qi_fmul] =	(PIPE_FM);
      issue_pipe[qi_fcmov1] =	(PIPE_FA);
      issue_pipe[qi_fcmov2] =	(PIPE_FA);
      issue_pipe[qi_fdiv] =	(PIPE_FA);
      issue_pipe[qi_fsqrt] =	(PIPE_FA);
      issue_pipe[qi_nop] =	(PIPE_NONE);
      issue_pipe[qi_ftoi] =	(PIPE_L0 | PIPE_L1);
      issue_pipe[qi_itof] =	(PIPE_L0 | PIPE_L1);
      issue_pipe[qi_mx_fpcr] =	(PIPE_FM);

    
/* fill subcluster array that will map which pipes can be used based
 * on the instruction mix in the fetch line (aka fetch block)
 * This implements the instruction type to slotting table in the ev6 spec
 * in the ebox slotting section.
   For example the first two entries in the spec table are:
	Instruction Type	Slotting
	3 2 1 0			3 2 1 0
	E E E E                 U L U L
	E E E L			U L U L
   Where:
	E - instruction that can execute in either subcluster
	L - instruction that can execute in lower only (L0,L1)
	U - instruction that can execute in upper only (U0,U1)
    In the table below the offsets into subcluster_table
		  E= "00" 
	          L= "01"
  	          U= "10"
    For The values stored in the array:
		  U= 0
		  L= 1
 */

    subcluster_table[strtol("00000000",0,2)] = strtol("0101",0,2);  /*   00 00 00 00 0101 */
    subcluster_table[strtol("00000001",0,2)] = strtol("0101",0,2);  /*   00 00 00 01 0101 */
    subcluster_table[strtol("00000010",0,2)] = strtol("0110",0,2);  /*   00 00 00 10 0110 */
    subcluster_table[strtol("00000100",0,2)] = strtol("0110",0,2);  /*   00 00 01 00 0110 */
    subcluster_table[strtol("00000101",0,2)] = strtol("0011",0,2);  /*   00 00 01 01 0011 */
    subcluster_table[strtol("00000110",0,2)] = strtol("0110",0,2);  /*   00 00 01 10 0110 */
    subcluster_table[strtol("00001000",0,2)] = strtol("0101",0,2);  /*   00 00 10 00 0101 */
    subcluster_table[strtol("00001001",0,2)] = strtol("0101",0,2);  /*   00 00 10 01 0101 */
    subcluster_table[strtol("00001010",0,2)] = strtol("1100",0,2);  /*   00 00 10 10 1100 */
    subcluster_table[strtol("00010000",0,2)] = strtol("0101",0,2);  /*   00 01 00 00 0101 */
    subcluster_table[strtol("00010001",0,2)] = strtol("0101",0,2);  /*   00 01 00 01 0101 */
    subcluster_table[strtol("00010010",0,2)] = strtol("0110",0,2);  /*   00 01 00 10 0110 */
    subcluster_table[strtol("00010100",0,2)] = strtol("0110",0,2);  /*   00 01 01 00 0110 */
    subcluster_table[strtol("00010101",0,2)] = strtol("0111",0,2);  /*   00 01 01 01 0111 */
    subcluster_table[strtol("00010110",0,2)] = strtol("0110",0,2);  /*   00 01 01 10 0110 */
    subcluster_table[strtol("00011000",0,2)] = strtol("0101",0,2);  /*   00 01 10 00 0101 */
    subcluster_table[strtol("00011001",0,2)] = strtol("0101",0,2);  /*   00 01 10 01 0101 */
    subcluster_table[strtol("00011010",0,2)] = strtol("1100",0,2);  /*   00 01 10 10 1100 */
    subcluster_table[strtol("00100000",0,2)] = strtol("1010",0,2);  /*   00 10 00 00 1010 */
    subcluster_table[strtol("00100001",0,2)] = strtol("1001",0,2);  /*   00 10 00 01 1001 */
    subcluster_table[strtol("00100010",0,2)] = strtol("1010",0,2);  /*   00 10 00 10 1010 */
    subcluster_table[strtol("00100100",0,2)] = strtol("1010",0,2);  /*   00 10 01 00 1010 */
    subcluster_table[strtol("00100101",0,2)] = strtol("0011",0,2);  /*   00 10 01 01 0011 */
    subcluster_table[strtol("00100110",0,2)] = strtol("1010",0,2);  /*   00 10 01 10 1010 */
    subcluster_table[strtol("00101000",0,2)] = strtol("1001",0,2);  /*   00 10 10 00 1001 */
    subcluster_table[strtol("00101001",0,2)] = strtol("1001",0,2);  /*   00 10 10 01 1001 */
    subcluster_table[strtol("00101010",0,2)] = strtol("1000",0,2);  /*   00 10 10 10 1000 */
    subcluster_table[strtol("01000000",0,2)] = strtol("1010",0,2);  /*   01 00 00 00 1010 */
    subcluster_table[strtol("01000001",0,2)] = strtol("1001",0,2);  /*   01 00 00 01 1001 */
    subcluster_table[strtol("01000010",0,2)] = strtol("1010",0,2);  /*   01 00 00 10 1010 */
    subcluster_table[strtol("01000100",0,2)] = strtol("1010",0,2);  /*   01 00 01 00 1010 */
    subcluster_table[strtol("01000101",0,2)] = strtol("1011",0,2);  /*   01 00 01 01 1011 */
    subcluster_table[strtol("01000110",0,2)] = strtol("1010",0,2);  /*   01 00 01 10 1010 */
    subcluster_table[strtol("01001000",0,2)] = strtol("1001",0,2);  /*   01 00 10 00 1001 */
    subcluster_table[strtol("01001001",0,2)] = strtol("1001",0,2);  /*   01 00 10 01 1001 */
    subcluster_table[strtol("01001010",0,2)] = strtol("1100",0,2);  /*   01 00 10 10 1100 */
    subcluster_table[strtol("01010000",0,2)] = strtol("1100",0,2);  /*   01 01 00 00 1100 */
    subcluster_table[strtol("01010001",0,2)] = strtol("1101",0,2);  /*   01 01 00 01 1101 */
    subcluster_table[strtol("01010010",0,2)] = strtol("1100",0,2);  /*   01 01 00 10 1100 */
    subcluster_table[strtol("01010100",0,2)] = strtol("1110",0,2);  /*   01 01 01 00 1110 */
    subcluster_table[strtol("01010101",0,2)] = strtol("1111",0,2);  /*   01 01 01 01 1111 */
    subcluster_table[strtol("01010110",0,2)] = strtol("1110",0,2);  /*   01 01 01 10 1110 */
    subcluster_table[strtol("01011000",0,2)] = strtol("1100",0,2);  /*   01 01 10 00 1100 */
    subcluster_table[strtol("01011001",0,2)] = strtol("1101",0,2);  /*   01 01 10 01 1101 */
    subcluster_table[strtol("01011010",0,2)] = strtol("1100",0,2);  /*   01 01 10 10 1100 */
    subcluster_table[strtol("01100000",0,2)] = strtol("1010",0,2);  /*   01 10 00 00 1010 */
    subcluster_table[strtol("01100001",0,2)] = strtol("1001",0,2);  /*   01 10 00 01 1001 */
    subcluster_table[strtol("01100010",0,2)] = strtol("1010",0,2);  /*   01 10 00 10 1010 */
    subcluster_table[strtol("01100100",0,2)] = strtol("1010",0,2);  /*   01 10 01 00 1010 */
    subcluster_table[strtol("01100101",0,2)] = strtol("1011",0,2);  /*   01 10 01 01 1011 */
    subcluster_table[strtol("01100110",0,2)] = strtol("1010",0,2);  /*   01 10 01 10 1010 */
    subcluster_table[strtol("01101000",0,2)] = strtol("1001",0,2);  /*   01 10 10 00 1001 */
    subcluster_table[strtol("01101001",0,2)] = strtol("1001",0,2);  /*   01 10 10 01 1001 */
    subcluster_table[strtol("01101010",0,2)] = strtol("1000",0,2);  /*   01 10 10 10 1000 */
    subcluster_table[strtol("10000000",0,2)] = strtol("0101",0,2);  /*   10 00 00 00 0101 */
    subcluster_table[strtol("10000001",0,2)] = strtol("0101",0,2);  /*   10 00 00 01 0101 */
    subcluster_table[strtol("10000010",0,2)] = strtol("0110",0,2);  /*   10 00 00 10 0110 */
    subcluster_table[strtol("10000100",0,2)] = strtol("0110",0,2);  /*   10 00 01 00 0110 */
    subcluster_table[strtol("10000101",0,2)] = strtol("0011",0,2);  /*   10 00 01 01 0011 */
    subcluster_table[strtol("10000110",0,2)] = strtol("0110",0,2);  /*   10 00 01 10 0110 */
    subcluster_table[strtol("10001000",0,2)] = strtol("0101",0,2);  /*   10 00 10 00 0101 */
    subcluster_table[strtol("10001001",0,2)] = strtol("0101",0,2);  /*   10 00 10 01 0101 */
    subcluster_table[strtol("10001010",0,2)] = strtol("0100",0,2);  /*   10 00 10 10 0100 */
    subcluster_table[strtol("10010000",0,2)] = strtol("0101",0,2);  /*   10 01 00 00 0101 */
    subcluster_table[strtol("10010001",0,2)] = strtol("0101",0,2);  /*   10 01 00 01 0101 */
    subcluster_table[strtol("10010010",0,2)] = strtol("0110",0,2);  /*   10 01 00 10 0110 */
    subcluster_table[strtol("10010100",0,2)] = strtol("0110",0,2);  /*   10 01 01 00 0110 */
    subcluster_table[strtol("10010101",0,2)] = strtol("0111",0,2);  /*   10 01 01 01 0111 */
    subcluster_table[strtol("10010110",0,2)] = strtol("0110",0,2);  /*   10 01 01 10 0110 */
    subcluster_table[strtol("10011000",0,2)] = strtol("0101",0,2);  /*   10 01 10 00 0101 */
    subcluster_table[strtol("10011001",0,2)] = strtol("0101",0,2);  /*   10 01 10 01 0101 */
    subcluster_table[strtol("10011010",0,2)] = strtol("0100",0,2);  /*   10 01 10 10 0100 */
    subcluster_table[strtol("10100000",0,2)] = strtol("0011",0,2);  /*   10 10 00 00 0011 */
    subcluster_table[strtol("10100001",0,2)] = strtol("0011",0,2);  /*   10 10 00 01 0011 */
    subcluster_table[strtol("10100010",0,2)] = strtol("0010",0,2);  /*   10 10 00 10 0010 */
    subcluster_table[strtol("10100100",0,2)] = strtol("0011",0,2);  /*   10 10 01 00 0011 */
    subcluster_table[strtol("10100101",0,2)] = strtol("0011",0,2);  /*   10 10 01 01 0011 */
    subcluster_table[strtol("10100110",0,2)] = strtol("0010",0,2);  /*   10 10 01 10 0010 */
    subcluster_table[strtol("10101000",0,2)] = strtol("0001",0,2);  /*   10 10 10 00 0001 */
    subcluster_table[strtol("10101001",0,2)] = strtol("0001",0,2);  /*   10 10 10 01 0001 */
    subcluster_table[strtol("10101010",0,2)] = strtol("0000",0,2);  /*   10 10 10 10 0000 */

}




static void init_operate_latency (void)
/*
 ********************************************************************************
 *	 PURPOSE:
 *		Implment the register result latencies of instructions on EV6.
 *		This is a somewhat simplified treatment, for example, the muls
 *		are dealt with as only one value.
 *	 INPUT PARAMETERS:
 *	 OUTPUT PARAMETERS:
 *	 IMPLICIT INPUTS:
 *	 IMPLICIT OUTPUTS:
 *	 AUTHOR/DATE:
 *	V3.24 FLS Jan-97 Modified for EV6 as per EV6 Rev 2.0 spec dated Apr 10,1996
 ********************************************************************************
 */
{
      operate_latency[qi_none] =	0;
      operate_latency[qi_ild]  =	3;	/* V3.24 13+ if dcache miss and bcache? */
      operate_latency[qi_fld]  =	4;	/* V3.24 8+ if dcache miss and bcache? */
      operate_latency[qi_ist]  =	0;	/* V3.24 */
      operate_latency[qi_fst]  =	0;	/* V3.24 */
      operate_latency[qi_rpcc] =	1;	/* V3.24 */
      operate_latency[qi_lda] 	=	1;	/* V3.24 /* not a class in ev6 spec */
      operate_latency[qi_mtpr] =	0;	/* V3.24 hw_mtpr produced no register result so 0*/
      operate_latency[qi_jsr] =	        3; 	/* V3.24 */
      operate_latency[qi_ibr] =		0;	/* V3.24 */
      operate_latency[qi_iadd] 	=	1;	/* V3.24 */
      operate_latency[qi_ilog] 	=	1;	/* V3.24 */
      operate_latency[qi_ishf] =	1;	/* V3.24 */
      operate_latency[qi_cmov] =	1;	/* V3.24 */
      operate_latency[qi_cmov1] =	1;	/* V3.24 */
      operate_latency[qi_cmov2] =	1;	/* V3.24 */
      operate_latency[qi_imul] =	7;	/* V3.24 */
      operate_latency[qi_imisc] =	3;	/* V3.24 */
      operate_latency[qi_fadd] =	4;	/* V32.4  6 if consumer fst or ftoi */
      operate_latency[qi_fdiv] =	12;	/* V3.24 single prec. can be up to 15 - see spec section 2.7.3 */
      operate_latency[qi_fmul] =	4;	/* V3.24  6 if consumer fst or ftoi */
      operate_latency[qi_fcmov1] =	4;	/* V3.24  only consumer is fcmov2*/
      operate_latency[qi_fcmov2] =	4;	/* V3.24  6 if consumer fst or ftoi */
      operate_latency[qi_fsqrt] =	32;	/* V3.24 can be less  - see spec section 2.7.3 */
      operate_latency[qi_ftoi] =	3;	/* V3.24 */
      operate_latency[qi_itof] =	4;	/* V3.24 */
      operate_latency[qi_nop] 	=	0;
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
      
/* V3.24 EV6 orphans - don't know where they belong yet */

      imp =  (struct implemented *)calloc(1, sizeof(struct implemented));
      imp->di_class = qi_rpcc;		/* fls temp???? */
      instr_array[DO_DRAINT]->imp = imp;		/* fls ??? */
      instr_array[DO_RCC]->imp = imp;			/* rpcc */
      instr_array[DO_NUDGE]->imp = imp;			/* fls ??? */

/* V3.24 not in EV6 */      instr_array[DO_FETCH]->imp = imp;
/* V3.24 not in EV6 */      instr_array[DO_FETCH_M]->imp = imp;


      /* iadd class all opcode= 10x except CMPBGE  */
      /* integer arithmetic */
      imp = (struct implemented *) calloc(1, sizeof(struct implemented));
      imp->di_class = qi_iadd;			/* V3.24 */
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
      instr_array[DO_CMPB]->imp = imp;		/* V3.24 */
      instr_array[DO_CMPEQ]->imp = imp;
      instr_array[DO_CMPLE]->imp = imp;
      instr_array[DO_CMPLT]->imp = imp;
      instr_array[DO_CMPNE]->imp = imp;
      instr_array[DO_CMPUGE]->imp = imp;
      instr_array[DO_CMPUGT]->imp = imp;
      instr_array[DO_CMPULE]->imp = imp;
      instr_array[DO_CMPULT]->imp = imp;

      /* ilog class */
      /* integer logicals */
      imp = (struct implemented *) calloc(1, sizeof(struct implemented));
      imp->di_class = qi_ilog;			/* V3.24 */
      instr_array[DO_AMASK]->imp = imp;		/* V3.24 */
      instr_array[DO_IMPLVER]->imp = imp;	/* V3.24 */
      instr_array[DO_AND]->imp = imp;
      instr_array[DO_BIC]->imp = imp;
      instr_array[DO_EQV]->imp = imp;
      instr_array[DO_OR]->imp = imp;
      instr_array[DO_ORNOT]->imp = imp;
      instr_array[DO_XOR]->imp = imp;
      instr_array[DO_CMPBGE]->imp = imp;	/* V3.24 */

      /* lda class */
      imp = (struct implemented *) calloc(1, sizeof(struct implemented));
      imp->di_class = qi_lda;			/* V3.24 */
      instr_array[DO_LDAH]->imp = imp;
      instr_array[DO_LDAL]->imp = imp;
      instr_array[DO_LDALH]->imp = imp;
      instr_array[DO_LDAQ]->imp = imp;
      
      /* mem_misc class */
      imp = (struct implemented *) calloc(1, sizeof(struct implemented));
      imp->di_class = qi_mem_misc;			/* V3.24 */
      instr_array[DO_WH64]->imp = imp;
      instr_array[DO_ECB]->imp = imp;
      instr_array[DO_MB]->imp = imp;		/* V3.24 */

      
      /* CMOV class */
      imp = (struct implemented *) calloc(1, sizeof(struct implemented));
      imp->di_class = qi_cmov;
      instr_array[DO_CMOVEQ]->imp = imp;
      instr_array[DO_CMOVGE]->imp = imp;
      instr_array[DO_CMOVGT]->imp = imp;
      instr_array[DO_CMOVLBC]->imp = imp;
      instr_array[DO_CMOVLBS]->imp = imp;
      instr_array[DO_CMOVLE]->imp = imp;
      instr_array[DO_CMOVLT]->imp = imp;
      instr_array[DO_CMOVNE]->imp = imp;
      
      /* SHIFT class */
      /* bit field */
      imp = (struct implemented *) calloc(1, sizeof(struct implemented));
      imp->di_class = qi_ishf;
      instr_array[DO_SEXTB]->imp = imp;		/* V3.24 */
      instr_array[DO_SEXTW]->imp = imp;		/* V3.24 */
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
      
      /* imul class */
      imp = (struct implemented *) calloc(1, sizeof(struct implemented));
      imp->di_class = qi_imul;
      instr_array[DO_MULL]->imp = imp;
      instr_array[DO_MULLV]->imp = imp;
      instr_array[DO_MULQ]->imp = imp;		/* V3.24 */
      instr_array[DO_MULQV]->imp = imp;		/* V3.24 */
      instr_array[DO_UMULH]->imp = imp;		/* V3.24 */
      
      /* imisc class */
      imp = (struct implemented *) calloc(1, sizeof(struct implemented));
      imp->di_class = qi_imisc;
      instr_array[DO_CTPOP]->imp = imp;		/* V3.24 */
      instr_array[DO_CTLZ]->imp = imp;		/* V3.24 */
      instr_array[DO_CTTZ]->imp = imp;		/* V3.24 */
      instr_array[DO_PERR]->imp = imp;		/* V3.24 */
      instr_array[DO_MINUB8]->imp = imp;		/* V3.24 */
      instr_array[DO_MINUW4]->imp = imp;		/* V3.24 */
      instr_array[DO_MINSB8]->imp = imp;		/* V3.24 */
      instr_array[DO_MINSW4]->imp = imp;		/* V3.24 */
      instr_array[DO_MAXUB8]->imp = imp;		/* V3.24 */
      instr_array[DO_MAXUW4]->imp = imp;		/* V3.24 */
      instr_array[DO_MAXSB8]->imp = imp;		/* V3.24 */
      instr_array[DO_MAXSW4]->imp = imp;		/* V3.24 */
      instr_array[DO_UNPKBW]->imp = imp;		/* V3.24 */
      instr_array[DO_UNPKBL]->imp = imp;		/* V3.24 */
      instr_array[DO_PKWB]->imp = imp;		/* V3.24 */
      instr_array[DO_PKLB]->imp = imp;		/* V3.24 */

      /* Normal floating point operates */
      imp = (struct implemented *)calloc(1, sizeof(struct implemented));
      imp->di_class = qi_fadd;
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
      instr_array[DO_ADDSS]->imp = imp;
      instr_array[DO_ADDSSC]->imp = imp;
      instr_array[DO_ADDSSU]->imp = imp;
      instr_array[DO_ADDSSM]->imp = imp;
      instr_array[DO_ADDSSD]->imp = imp;
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
      instr_array[DO_ADDTS]->imp = imp;
      instr_array[DO_ADDTSC]->imp = imp;
      instr_array[DO_ADDTSM]->imp = imp;
      instr_array[DO_ADDTSD]->imp = imp;
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

      instr_array[DO_CPYS]->imp = imp;		/* V3.24 */
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
      instr_array[DO_WR_FPCTL]->imp = imp;
      
      instr_array[DO_CPYSE]->imp = imp;
      instr_array[DO_CPYSEE]->imp = imp;
      instr_array[DO_CPYSN]->imp = imp;
      
      /* FDIV (2 operands & one dest) */
      imp = (struct implemented *)calloc(1, sizeof(struct implemented));
      imp->di_class = qi_fdiv;
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
      imp->di_class = qi_fmul;
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
      

      /* SQRT class*/
      
      imp = (struct implemented *)calloc(1, sizeof(struct implemented));
      imp->di_class = qi_fsqrt;

      instr_array[DO_SQRTS]->imp = imp;
      instr_array[DO_SQRTSC]->imp = imp;
      instr_array[DO_SQRTSM]->imp = imp;
      instr_array[DO_SQRTSD]->imp = imp;
      instr_array[DO_SQRTSU]->imp = imp;
      instr_array[DO_SQRTSUC]->imp = imp;
      instr_array[DO_SQRTSUM]->imp = imp;
      instr_array[DO_SQRTSUD]->imp = imp;
      instr_array[DO_SQRTSSU]->imp = imp;
      instr_array[DO_SQRTSSUC]->imp = imp;
      instr_array[DO_SQRTSSUM]->imp = imp;
      instr_array[DO_SQRTSSUD]->imp = imp;
      instr_array[DO_SQRTSSUI]->imp = imp;
      instr_array[DO_SQRTSSUIC]->imp = imp;
      instr_array[DO_SQRTSSUIM]->imp = imp;
      instr_array[DO_SQRTSSUID]->imp = imp;
      instr_array[DO_SQRTT]->imp = imp;
      instr_array[DO_SQRTTC]->imp = imp;
      instr_array[DO_SQRTTM]->imp = imp;
      instr_array[DO_SQRTTD]->imp = imp;
      instr_array[DO_SQRTTU]->imp = imp;
      instr_array[DO_SQRTTUC]->imp = imp;
      instr_array[DO_SQRTTUM]->imp = imp;
      instr_array[DO_SQRTTUD]->imp = imp;
      instr_array[DO_SQRTTSU]->imp = imp;
      instr_array[DO_SQRTTSUC]->imp = imp;
      instr_array[DO_SQRTTSUM]->imp = imp;
      instr_array[DO_SQRTTSUD]->imp = imp;
      instr_array[DO_SQRTTSUI]->imp = imp;
      instr_array[DO_SQRTTSUIC]->imp = imp;
      instr_array[DO_SQRTTSUIM]->imp = imp;
      instr_array[DO_SQRTTSUID]->imp = imp;

      
      /* MXPR class split into MF and MT class, since only MF has a register result latency*/
      imp =  (struct implemented *)calloc(1, sizeof(struct implemented));
      imp->di_class = qi_mfpr;
      instr_array[DO_HW_MFPR]->imp = imp;

      imp =  (struct implemented *)calloc(1, sizeof(struct implemented));
      imp->di_class = qi_mtpr;
      instr_array[DO_HW_MTPR]->imp = imp;

      /* ild class all integer loads */
      imp = (struct implemented *)calloc(1, sizeof(struct implemented));
      imp->di_class = qi_ild;
      instr_array[DO_HW_LD]->imp = imp;
      instr_array[DO_LDB]->imp = imp;		/* V3.28 */
      instr_array[DO_LDW]->imp = imp;		/* V3.28 */
      instr_array[DO_LDL]->imp = imp;
      instr_array[DO_LDQ]->imp = imp;
      instr_array[DO_LDQU]->imp = imp;
      instr_array[DO_LDLL]->imp = imp;  	/* V3.24 Not sure belongs here */
      instr_array[DO_LDQL]->imp = imp;		/* V3.24 Not sure belongs here */
      
      /* ist class all integer stores */
      
      imp = (struct implemented *)calloc(1, sizeof(struct implemented));
      imp->di_class = qi_ist;
      instr_array[DO_HW_ST]->imp = imp;
      instr_array[DO_STB]->imp = imp;		/* V3.28 */
      instr_array[DO_STW]->imp = imp;		/* V3.28 */
      instr_array[DO_STL]->imp = imp;
      instr_array[DO_STQ]->imp = imp;
      instr_array[DO_STQU]->imp = imp;
      instr_array[DO_STLC]->imp = imp;
      instr_array[DO_STQC]->imp = imp;
        
      /* fld class all floating loads */
      imp = (struct implemented *)calloc(1, sizeof(struct implemented));
      imp->di_class = qi_fld;
      instr_array[DO_LDS]->imp = imp;
      instr_array[DO_LDT]->imp = imp;
      instr_array[DO_LDD]->imp = imp;
      instr_array[DO_LDF]->imp = imp;


      /* fst class all floating stores */
      imp = (struct implemented *)calloc(1, sizeof(struct implemented));
      imp->di_class = qi_fst;
      instr_array[DO_STF]->imp = imp;
      instr_array[DO_STS]->imp = imp;
      instr_array[DO_STT]->imp = imp;
      instr_array[DO_STD]->imp = imp;	/* V3.24 same as STG */


      /* ftoi class*/
      imp = (struct implemented *)calloc(1, sizeof(struct implemented));
      imp->di_class = qi_ftoi;
      instr_array[DO_FTOIT]->imp = imp;		/* V3.24 */
      instr_array[DO_FTOIS]->imp = imp;		/* V3.24 */

      /* itof class */
      imp = (struct implemented *)calloc(1, sizeof(struct implemented));
      imp->di_class = qi_itof;
      instr_array[DO_ITOFF]->imp = imp;		/* V3.24 */
      instr_array[DO_ITOFS]->imp = imp;		/* V3.24 */
      instr_array[DO_ITOFT]->imp = imp;		/* V3.24 */


      /* ibr class One Integer operand conditional branches */
      
      imp =  (struct implemented *)calloc(1, sizeof(struct implemented));
      imp->di_class = qi_ibr;
      
      instr_array[DO_BEQ]->imp = imp;
      instr_array[DO_BGE]->imp = imp;
      instr_array[DO_BGT]->imp = imp;
      instr_array[DO_BLBC]->imp = imp;
      instr_array[DO_BLBS]->imp = imp;
      instr_array[DO_BLE]->imp = imp;
      instr_array[DO_BLT]->imp = imp;
      instr_array[DO_BNE]->imp = imp;
      
      /* fbr class - one float operand conditional branches */
      imp =  (struct implemented *) calloc(1, sizeof(struct implemented));
      imp->di_class = qi_fbr;
      instr_array[DO_BFEQ]->imp = imp;
      instr_array[DO_BFGE]->imp = imp;
      instr_array[DO_BFGT]->imp = imp;
      instr_array[DO_BFLE]->imp = imp;
      instr_array[DO_BFLT]->imp = imp;
      instr_array[DO_BFNE]->imp = imp;
      instr_array[DO_FLBC]->imp = imp;
      
      /* JSR class */
      imp =  (struct implemented *)calloc(1, sizeof(struct implemented));
      imp->di_class = qi_jsr;
      instr_array[DO_HW_REI]->imp = imp;	/* fls change to do_hw_ret */
      instr_array[DO_BR]->imp = imp;
      instr_array[DO_BSR]->imp = imp;
      instr_array[DO_JSR]->imp = imp;
      /* CALL_PAL (aka EPICODE) is in jsr class for EV6*/
      instr_array[DO_EPICODE]->imp = imp;  

      /* MX_FPCR class */
      imp =  (struct implemented *)calloc(1, sizeof(struct implemented));
      imp->di_class = qi_mx_fpcr;
      instr_array[DO_MF_FPCR]->imp = imp;
      instr_array[DO_MT_FPCR]->imp = imp;


}



static void init_retire_latency (void)
/*
 ********************************************************************************
 *	 PURPOSE:
 *		Implement the register retire latencies of instructions on EV6.
 *		This is a somewhat simplified treatment, for example, the muls
 *		are dealt with as only one value.
 *		The latencies are from the reg stage (pipeline stage 4).
 *	 INPUT PARAMETERS:
 *	 OUTPUT PARAMETERS:
 *	 IMPLICIT INPUTS:
 *	 IMPLICIT OUTPUTS:
 *	 AUTHOR/DATE:
 *	V3.24 FLS Feb-97 EV6 as per spec v2.0 dated 12-apr-1996 and perf model.
 ********************************************************************************
 */
{
      retire_latency[qi_none] =	0;
      retire_latency[qi_ild]  =		MEM_RETIRE;			/* integer loads */
      retire_latency[qi_fld]  =		MEM_RETIRE;			/* floating loads */
      retire_latency[qi_ist]  =		MEM_RETIRE;			/* integer stores */
      retire_latency[qi_fst]  =		MEM_RETIRE;			/* floating stores*/
      retire_latency[qi_rpcc] =		ALU_LATENCY + EBOX_RETIRE;	/* read cycle counter*/
      retire_latency[qi_lda] 	=	ALU_LATENCY + EBOX_RETIRE;	/* load address*/
      retire_latency[qi_mfpr] =		ALU_LATENCY + EBOX_RETIRE;	/* MT */
      retire_latency[qi_mtpr] =		ALU_LATENCY + EBOX_RETIRE;	/* MT */
      retire_latency[qi_jsr] =	        JSR_RETIRE; 			/* jsr, jmp,ret,cor*/
      retire_latency[qi_ibr] =		BRANCH_RETIRE;			/* integer branch*/
      retire_latency[qi_iadd] 	=	ALU_LATENCY + EBOX_RETIRE;	/* integer add */
      retire_latency[qi_ilog] 	=	ALU_LATENCY + EBOX_RETIRE;	/* logical inst.  */
      retire_latency[qi_ishf] =		SHF_LATENCY + EBOX_RETIRE;	/* shift inst. */
      retire_latency[qi_cmov] =		ALU_LATENCY + EBOX_RETIRE;	/* cmov - same as logical inst.*/
      retire_latency[qi_cmov1] =	ALU_LATENCY + EBOX_RETIRE;	/* fls - later when cmov split added */
      retire_latency[qi_cmov2] =	ALU_LATENCY + EBOX_RETIRE;	/* fls - later when cmov split added*/
      retire_latency[qi_imul] =		IMUL_LATENCY_TO_RETIRE + EBOX_RETIRE;	/* integer multiply*/
      retire_latency[qi_imisc] =	ALU_LATENCY + EBOX_RETIRE;	/* fls - GUESS!! new ev6 min max instructions*/
      retire_latency[qi_fadd] =		FADD_LATENCY + FBOX_RETIRE;	/* floating multiply*/
      retire_latency[qi_fdiv] =		FDIV_T_LATENCY + FADD_LATENCY;	/* NOT USED - see get_retire_latency routine*/
      retire_latency[qi_fmul] =		FMUL_LATENCY + FBOX_RETIRE;	/* floating multiply*/
      retire_latency[qi_fcmov1] =	4;	/* fls later will be used for split fcmov */
      retire_latency[qi_fcmov2] =	4;	/* fls later will be used for split fcmov */
      retire_latency[qi_fsqrt] =	32;	/* NOT USED - see get_retire_latency routine */
      retire_latency[qi_ftoi] =		EBOX_RETIRE + FBOX_RETIRE;	/* FLS GUESS!! */
      retire_latency[qi_itof] =		EBOX_RETIRE + FBOX_RETIRE;	/* FLS GUESS!!*/
      retire_latency[qi_nop] 	=	0;				/* not used - these are squashed */
}



static void init_decode_tables (void)
/*
 ********************************************************************************
 *	PURPOSE:
 *		Since each chip has its own iprs, each chip must be able to
 *		initialize the decode table with the approprite names
 *		EV6
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
      /* load all unusual names for IPRs here */
      strcpy(ipr_name_table[EV6__PS],"EV6__PS");		/*index  (0x9) */
      /* EBOX IPRs */
      strcpy(ipr_name_table[EV6__CC],"EV6__CC");		/*index  (1100 0000 base 2) */
      strcpy(ipr_name_table[EV6__CC_CTL],"EV6__CC_CTL");	/*index  (1100 0001 base 2) */
      strcpy(ipr_name_table[EV6__VA],"EV6__VA");		/*index  (1100 0010 base 2) */
      strcpy(ipr_name_table[EV6__VA_FORM],"EV6__VA_FORM");	/*index  (1100 0011 base 2) */
      strcpy(ipr_name_table[EV6__VA_CTL],"EV6__VA_CTL");	/*index  (1100 0100 base 2) */
      /* IBOX IPRs */
      strcpy(ipr_name_table[EV6__ITB_TAG],"EV6__ITB_TAG");	/*index  (0000 0000 base 2) */
      strcpy(ipr_name_table[EV6__ITB_PTE],"EV6__ITB_PTE");	/*index  (0000 0001 base 2) */
      strcpy(ipr_name_table[EV6__ITB_IAP],"EV6__ITB_IAP");	/*index  (0000 0010 base 2) */
      strcpy(ipr_name_table[EV6__ITB_IA],"EV6__ITB_IA");		/*index  (0000 0011 base 2) */
      strcpy(ipr_name_table[EV6__ITB_IS],"EV6__ITB_IS");		/*index  (0000 0100 base 2) */
      strcpy(ipr_name_table[EV6__EXC_ADDR],"EV6__EXC_ADDR");	/*index  (0000 0110 base 2) */
      strcpy(ipr_name_table[EV6__IVA_FORM],"EV6__IVA_FORM");	/*index  (0000 0111 base 2) */
      strcpy(ipr_name_table[EV6__IER_CM],"EV6__IER_CM");	/*index  (0000 10x1 base 2) */
      strcpy(ipr_name_table[EV6__IER],"EV6__IER");		/*index  (0000 101x base 2) */
      strcpy(ipr_name_table[EV6__SIRR],"EV6__SIRR");		/*index  (0000 1100 base 2) */
      strcpy(ipr_name_table[EV6__ISUM],"EV6__ISUM");		/*index  (0000 1101 base 2) */
      strcpy(ipr_name_table[EV6__HW_INT_CLR],"EV6__HW_INT_CLR");/*index  (0000 1110 base 2) */
      strcpy(ipr_name_table[EV6__EXC_SUM],"EV6__EXC_SUM");	/*index  (0000 1111 base 2) */
      strcpy(ipr_name_table[EV6__PAL_BASE],"EV6__PAL_BASE");	/*index  (0001 0000 base 2) */
      strcpy(ipr_name_table[EV6__I_CTL],"EV6__I_CTL");	/*index  (0001 0001 base 2) */
      strcpy(ipr_name_table[EV6__IC_FLUSH_ASM],"EV6__IC_FLUSH_ASM");	/*index  (0001 0010 base 2) */
      strcpy(ipr_name_table[EV6__IC_FLUSH],"EV6__IC_FLUSH");	/*index  (0001 0011 base 2) */
      strcpy(ipr_name_table[EV6__PCTR_CTL],"EV6__PCTR_CTL");	/*index  (0001 0100 base 2) */
      strcpy(ipr_name_table[EV6__CLR_MAP],"EV6__CLR_MAP");	/*index  (0001 0101 base 2) */
      strcpy(ipr_name_table[EV6__SLEEP],"EV6__SLEEP");	/*index  (0001 0111 base 2) */
      strcpy(ipr_name_table[EV6__I_STAT],"EV6__I_STAT");	/*index  (0001 0110 base 2) */
      strcpy(ipr_name_table[EV6__ASN],"EV6__ASN");		/*index  (01xx xxx1 base 2) */
      strcpy(ipr_name_table[EV6__ASTER],"EV6__ASTER");	/*index  (01xx xx1x base 2) */
      strcpy(ipr_name_table[EV6__ASTRR],"EV6__ASTRR");	/*index  (01xx x1xx base 2) */
      strcpy(ipr_name_table[EV6__PPCE],"EV6__PPCE");		/*index  (01xx 1xxx base 2) */
      strcpy(ipr_name_table[EV6__FPE],"EV6__FPE");		/*index  (01x1 xxxx base 2) */
     /* IBOX IPRs */
      strcpy(ipr_name_table[EV6__DTB_TAG0],"EV6__DTB_TAG0");	/*index  (0010 0000 base 2) */
      strcpy(ipr_name_table[EV6__DTB_TAG1],"EV6__DTB_TAG1");	/*index  (1010 0000 base 2) */
      strcpy(ipr_name_table[EV6__DTB_PTE0],"EV6__DTB_PTE0");	/*index  (0010 0001 base 2) */
      strcpy(ipr_name_table[EV6__DTB_PTE1],"EV6__DTB_PTE1");	/*index  (1010 0001 base 2) */
      strcpy(ipr_name_table[EV6__DTB_IAP],"EV6__DTB_IAP");	/*index  (1010 0010 base 2) */
      strcpy(ipr_name_table[EV6__DTB_IA],"EV6__DTB_IA");	/*index  (1010 0011 base 2) */
      strcpy(ipr_name_table[EV6__DTB_IS0],"EV6__DTB_IS0");	/*index  (0010 0100 base 2) */
      strcpy(ipr_name_table[EV6__DTB_IS1],"EV6__DTB_IS1");	/*index  (1010 0100 base 2) */
      strcpy(ipr_name_table[EV6__DTB_ASN0],"EV6__DTB_ASN0");	/*index  (0010 0101 base 2) */
      strcpy(ipr_name_table[EV6__DTB_ASN1],"EV6__DTB_ASN1");	/*index  (1010 0101 base 2) */
      strcpy(ipr_name_table[EV6__DTB_ALT_MODE],"EV6__DTB_ALT_MODE");	/*index  (1010 0110 base 2) */
      strcpy(ipr_name_table[EV6__MM_STAT],"EV6__MM_STAT");	/*index  (1010 0111 base 2) */
      strcpy(ipr_name_table[EV6__M_CTL],"EV6__M_CTL");	/*index  (0010 1000 base 2) */
      strcpy(ipr_name_table[EV6__DC_CTL],"EV6__DC_CTL");	/*index  (0010 1001 base 2) */
      strcpy(ipr_name_table[EV6__DC_STAT],"EV6__DC_STAT");	/*index  (0010 1010 base 2) */
     /* CBOX IPRs */
      strcpy(ipr_name_table[EV6__DATA],"EV6__DATA");		/*index  (0010 1011 base 2) */
      strcpy(ipr_name_table[EV6__SHIFT_CONTROL],"EV6__SHIFT_CONTROL");/*index  (0010 1100 base 2) */
      strcpy(ipr_name_table[EV6__PROCESS_CONTEXT],"EV6__PROCESS_CONTEXT");/*index  (5F) */

}

static void add_if_ev6_caveat (INSTR *raw_code[], int offs, int *id, int poss, int count)
/*
 ********************************************************************************
 *	PURPOSE:
 *		Add pal entry flow caveats to be checked by end of permutation routines
 *		such as check_ev6_guideline_11.
 *
 *	INPUT PARAMETERS:
 *		raw_code: pointer to the array of instructions from follow_code
 *		offs: where we are in the above array
 *		      Instruction at offs=0 is the pal entry instruction.
 *		id:   array of ipr info such as scoreboard bits
 *		poss:  Position in fetch block. 
 *		count: Number of Longwords in fetch block. Max = 4.
 *
 *	OUTPUT PARAMETERS:
 *		None
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:            
 *	   add_one_caveat
 * EV6 uses the caveat_list array for different purposes as follows:
 *
 *			           EV4/EV5		   EV6
 *	event1:	aka event:      trigger event id	palcode address of event instruction
 *	event2: aka prohibited: prohib. event id	event id
 *	cyc:	aka cycles:	cycle count		0
 *	desc:	aka msg:	error msg		instruction ascii decode at palcode address

 *	AUTHOR/DATE:
 *	V3.24 FLS Sep-96 
 ********************************************************************************
 */
{
      int i, index[4], caveat_id;
      int address;			/* palcode address of caveat */
      int cycle=0;			/* artifact from ev4/ev5 */
      int idtmp, ipr_num;

      for (i=0; i<count; i++) {				

            /* If powerup (aka PAL_RESET) entry routine */
            /* then check if ldx/stx to satisfy restriction #27 */
            if (raw_code[0]->address == 0) {
	        idtmp = get_inst_id (raw_code[offs+i]);
	        if   (((idtmp == (Memory_op | Virtual_inst | Read))  && (raw_code[offs+i]->index != DO_HW_LD)) ||
	             ((idtmp == (Memory_op | Virtual_inst | Write)) && (raw_code[offs+i]->index != DO_HW_ST)))
		         force_fail_lock_ev6 = TRUE;
	        }
      	    index[i] = raw_code[offs+i]->index;						/* get unique inst. index */
	    address = raw_code[offs+i]->address;
	    ipr_num = raw_code[offs+i]->bits.ev6_hwmxpr_format.index;
	    idtmp = get_ipr_id (ipr_num);					       /* get ids such as Ibox_ipr */

	    caveat_id=0;
            if (index[i] == DO_HW_MTPR) {
	       if (ipr_num == EV6__DTB_ASN0) 
		    caveat_id = Mt + Dtb_asn0;		/* for check_ev6_restriction_21a/b */
	       else if (ipr_num == EV6__DTB_ASN1) 
		    caveat_id = Mt + Dtb_asn1;		/* for check_ev6_restriction_21a/b */
	       else if (ipr_num == EV6__DTB_IS0)
		    caveat_id = Mt + Dtb_is0;		/* for check_ev6_restriction_22a/b */
	       else if (ipr_num == EV6__DTB_IS1)
		    caveat_id = Mt + Dtb_is1;		/* for check_ev6_restriction_22a/b */
	       else if (ipr_num == EV6__IC_FLUSH)
		    caveat_id = Mt + Ic_flush;		/* for check_ev6_restriction_24ab */
	       else if (ipr_num == EV6__IC_FLUSH_ASM)
		    caveat_id = Mt + Ic_flush;		/* for check_ev6_restriction_24ab */
	       else if (ipr_num == EV6__CLR_MAP)
		    caveat_id = Mt + Clear_map;		/* for check_ev6_restriction_24c */
	       else if (ipr_num == EV6__DTB_TAG0)
		    caveat_id = Mt + Dtb_tag0;		/* for check_ev6_restriction_17 */
	       else if (ipr_num == EV6__DTB_PTE0)
		    caveat_id = Mt + Dtb_pte0;		/* for check_ev6_restriction_17 */
	       else if (ipr_num == EV6__DTB_TAG1)
		    caveat_id = Mt + Dtb_tag1;		/* for check_ev6_restriction_17 */
	       else if (ipr_num == EV6__DTB_PTE1)
		    caveat_id = Mt + Dtb_pte1;		/* for check_ev6_restriction_17 */
	       else if (ipr_num == EV6__DC_CTL)
		    caveat_id = Mt + Dc_ctl;		/* for check_ev6_restriction_28d */
	       else if (ipr_num == EV6__DATA)
		    caveat_id = Mt + Data;		/* for check_ev6_restriction_30 */
	       else if (ipr_num == EV6__ITB_TAG)
		    caveat_id = Mt + Itb_tag;		/* for check_ev6_restriction_34 */
	       else if (idtmp & Ibox_ipr)
		    caveat_id = Mt + Ibox_ipr;		/* for check_ev6_guideline_11 */
	    } 
            else if (index[i] == DO_HW_MFPR) {
	             if (ipr_num == EV6__MM_STAT)
		         caveat_id = Mf + Mm_stat;	/* for check_ev6_restriction_28a */
	       	     else if (ipr_num == EV6__VA)
		         caveat_id = Mf  + Va;		/* for check_ev6_restriction_28b */
	       	     else if (ipr_num == EV6__VA_FORM)
		         caveat_id = Mf + Va_form;	/* for check_ev6_restriction_28c */
		 }
	    else if (id[i] & Hwret_stall) 		/* check if hw_jmp_stall, hw_ret_stall or rare hw_jsr_stall, hw_jcr_stall */
		    caveat_id = id[i];	
	    else if (id[i] & Hwret) 			/* check if hw_jmp, hw_ret or rare hw_jsr, hw_jcr no stall */
		    caveat_id = id[i];	
	    else if (index[i] == DO_MT_FPCR) 		/* check if MT_FPCR */
		    caveat_id =  Mt_fpcr;		/* for check_ev6_restriction_19 */
	    else if ((index[i] == DO_HW_ST) &&		/* check if HW_STPC */
		     (id[i] & Lock_inst))
		         caveat_id = Hw_stpc;		/* for check_ev6_restriction_23 */
	    else if (id[i] & Lock_inst) 		/* check if lock instruction */
		    caveat_id =  Lock_inst;		/* for check_ev6_restriction_27 */
	    else if (index[i] == DO_JSR) 		/* check if JSR/JMP/RET/JSR_COROUTINE */
		    caveat_id =  Jsr;			/* for check_ev6_guideline_29 */
	    else if (id[i] & Stf_itof) 			/* check if STF/G/S instruction */
		    caveat_id =  Stf_itof;			/* for check_ev6_restriction_7 */

	    if (caveat_id != 0) {
		add_one_caveat (address,caveat_id, cycle ,raw_code[offs+i]->decoded,caveat_list);			/* add thie entry to the caveat_list */
	    }

      }
}



static void check_address_restrictions (INSTR *raw_code[], int offs)
/*
 ********************************************************************************
 *	PURPOSE:
 *		Check EV6 fetch block address related restrictions
 *		Calls add_if_ev6_caveat to store caveats (e.g. Mt asn0)
 *		that will be checked in end of permutation check routines,
 *		such as check_ev6_guideline_11 etc.
 *	INPUT PARAMETERS:
 *		raw_code: pointer to the array of instructions from follow_code
 *		offs: where we are in the above array
 *		      offs=0 is the pal entry instruction.

 *	OUTPUT PARAMETERS:
 *		None
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:
 *	AUTHOR/DATE:
 *	V3.24 FLS Nov-96 Modified for EV6
 ********************************************************************************
 */
{
      int i, index, fetch_block;
      int t0,t1;
      int finished = FALSE, br_taken=FALSE;
      int id[4];			/* V3.24 save ipr scbd bits etc*/
      int count; 			/* fetch block size*/
      int poss; 			/* fetch block offset*/

      first_fetch_block_ev6 = TRUE;			/* first time this permutation */
      while (!finished && raw_code[offs]) 
	{

	   fetch_block = raw_code[offs]->address & 0xF;
	   if (fetch_block == 0)   {  poss=0;  count = 4; }
	   if (fetch_block == 4)   {  poss=1;  count = 3; }
	   if (fetch_block == 8)   {  poss=2;  count = 2; }
	   if (fetch_block == 0xC) {  poss=3;  count = 1; }

	   /* fixup counts due to end of flow (raw_code[offs]=0) */
	   /*		   and br taken. */
	   if (poss==0)	{	
	      if (raw_code[offs]   != 0) {
		   if (raw_code[offs+1] != 0) {
		         t0 = raw_code[offs]->address   & 0xFFFFFFF0;
		         t1 = raw_code[offs+1]->address & 0xFFFFFFF0;
		         if (t0 != t1)  count = 1;
	           } else count = 1;
		   if (count==4) {
		       if (raw_code[offs+2] != 0) {
		         t0 = raw_code[offs+1]->address   & 0xFFFFFFF0;
		         t1 = raw_code[offs+2]->address & 0xFFFFFFF0;
		         if (t0 != t1)  count = 2;
	               } else count = 2;
		   }
		   if (count==4) {
		       if (raw_code[offs+3] != 0) {
		         t0 = raw_code[offs+2]->address   & 0xFFFFFFF0;
		         t1 = raw_code[offs+3]->address & 0xFFFFFFF0;
		         if (t0 != t1)  count = 3;
	                } else count = 3;
		   }
	      } else count = 0;
	   }
	   else if (poss==1)	{	
	          if (raw_code[offs]   != 0) {
		       if (raw_code[offs+1] != 0) {
		           t0 = raw_code[offs]->address   & 0xFFFFFFF0;
		           t1 = raw_code[offs+1]->address & 0xFFFFFFF0;
		           if (t0 != t1)  count = 1;
	               } else count = 1;
		       if (count==3) {
		           if (raw_code[offs+2] != 0) {
		             t0 = raw_code[offs+1]->address   & 0xFFFFFFF0;
		             t1 = raw_code[offs+2]->address & 0xFFFFFFF0;
		             if (t0 != t1)  count = 2;
	                    } else count = 2;
		       }
	          } else count = 0;
	       }

	   else if (poss==2)	{	
	          if (raw_code[offs]   != 0) {
		       if (raw_code[offs+1] != 0) {
		           t0 = raw_code[offs]->address   & 0xFFFFFFF0;
		           t1 = raw_code[offs+1]->address & 0xFFFFFFF0;
		           if (t0 != t1)  count = 1;
	               } else count = 1;
	          } else count = 0;
	       }

	   else if (poss==3)
		    if (raw_code[offs] == 0) count = 0;


	  /* check instruction index (i.e. valid instruction) is okay */
	  for (i=poss; i<count; i++) {				/* V3.24 */
	    if (raw_code[offs+i] != 0) {
	      if (offs+i < 0 || offs+i >= 5000) {
		printf("	%%PVC_ERROR in EV6.C->check_address_restrictions routine\n");
		printf("	raw_code offs out of range, offs: %d\n", offs+i);
		exit(0);
	      }
	      index = raw_code[offs+i]->index;

	      if (index > MAX_INDEX || index <= 0) {	
		  
		printf("	%%PVC_ERROR in EV6.C->check_address_restrictions routine \n");
		printf("	instr_array index out of range. \n");
		printf("        index: %d  raw_code offset: %d\n",index,offs+i);
		printf("        Probably due to illegal 21264 instruction.\n");
		printf("        Illegal instruction is at palcode address : %X\n",raw_code[offs+i]->address);
		exit(0);
	      } 

	      if (instr_array[index]->imp == 0) {
		printf("	%%PVC_ERROR in EV6.C->check_address_restrictions routine \n");
		printf("	instr_array[%d]->imp is 0\n",index);
		printf("        raw_code offset: %d\n",offs+i);
		exit(0);
 	      }

	    } else {
			finished = TRUE;
			break;
		   }
	  }

	/* get id info such as inst index and scoreboard bits for MF and MT iprs  etc. */
	for (i=poss; i<count; i++) {				/* V3.24 */
	   id[i] = get_inst_id (raw_code[offs+i]);  
	}


/*================================================================================================*/
	/* Check all EV6 restrictions that can be checked by PVC */
	/*  restriction 1 not checked in PVC */
	/* restriction 3 (removed from EV6 spec)*/
/*================================================================================================*/

	/* for end of permutation checking by check_permutation_restrictions */
	add_if_ev6_caveat(raw_code, offs, id, poss, count);  /* add caveat id such as hw_mtpr ASn0 */

	if (count > 1) {
	    check_ev6_restriction_2(raw_code, offs, id, poss, count); 
	    check_ev6_restriction_4(raw_code, offs, id, poss, count); 
	    check_ev6_restriction_12(raw_code, offs, id, poss, count); 
	    check_ev6_restriction_33(raw_code, offs, id, poss, count); 
	}

	if (first_fetch_block_ev6 == TRUE) {			/* check first fetch block only */
		check_ev6_restriction_14(raw_code, offs, id, poss, count);  
		check_ev6_guideline_26(raw_code, offs, id, poss, count); 
	}
	check_ev6_restriction_18(raw_code, offs, id, poss, count); 


        first_fetch_block_ev6 = FALSE;
	if (count==0) 	finished = TRUE;
	offs = offs + count ;		/* point to next fetch block */
	}
}

static void check_permutation_restrictions (INSTR *raw_code[], int offs,  List *caveat_list)
/*
 ********************************************************************************
 *	PURPOSE:
 *		Check EV6 permutation related restrictions. These are restrictions
 *	that live from pal_entry to pal_exit.
 *	A permutation is a single flow of code with branches forced to be taken for
 *	one permutation then forced not taken for the next permutation.
 *
 *	INPUT PARAMETERS:
 *		raw_code: pointer to the array of instructions from follow_code
 *		offs: where we are in the above array
 *	OUTPUT PARAMETERS:
 *		None
 *	IMPLICIT INPUTS:
 *		Assumes check_address_restrictions has been called to check that raw_code[offs]
 *		indexes are all valid.
 *		add_if_ev6_caveat is assumed called in check_address_restrictions routine.
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:
 *	AUTHOR/DATE:
 *	V3.24 FLS Sep-96 Modified for EV6
 ********************************************************************************
 */
{
        first_fetch_block_ev6 = TRUE;			/* first time this permutation */

	check_ev6_restriction_7 (raw_code,offs, caveat_list);
	check_ev6_restriction_17 (raw_code,offs, caveat_list);
	check_ev6_restriction_19 (raw_code,offs, caveat_list);
	check_ev6_restriction_21 (raw_code,offs, caveat_list);
	check_ev6_restriction_21a(raw_code,offs, caveat_list);
	check_ev6_restriction_21b(raw_code,offs, caveat_list);
	check_ev6_restriction_22 (raw_code,offs, caveat_list);
	check_ev6_restriction_22a(raw_code,offs, caveat_list);
	check_ev6_restriction_22b(raw_code,offs, caveat_list);
	check_ev6_restriction_23(raw_code,offs, caveat_list);
	check_ev6_restriction_24ab(raw_code,offs, caveat_list);
	check_ev6_restriction_24c(raw_code,offs, caveat_list);
	check_ev6_guideline_11 (raw_code,offs, caveat_list);/* guideline 11 has lowest priority of hw_ret/stall type restrictions*/
	check_ev6_restriction_27(raw_code,offs, caveat_list);
	check_ev6_restriction_28a(raw_code,offs, caveat_list);
	check_ev6_restriction_28b(raw_code,offs, caveat_list);
	check_ev6_restriction_28c(raw_code,offs, caveat_list);
	check_ev6_restriction_28d(raw_code,offs, caveat_list);
	check_ev6_guideline_29   (raw_code,offs, caveat_list);
	check_ev6_restriction_30(raw_code,offs, caveat_list);
	check_ev6_guideline_6 (raw_code,offs, caveat_list);
	check_ev6_restriction_34 (raw_code,offs, caveat_list);

}



int check_mfmt_reg_ev6 (INSTR *raw_code[], int offs_mm_stat, int mm_stat_reg, int offs_br, int br_cond_reg )
/*
 ********************************************************************************
 *	 PURPOSE: 
 *	 	Check for register dependency between mm_stat_reg and br_cond_reg 
 *		between  raw_code[offs_mm_stat] and raw_code[br_cond].
 *
 *	 INPUT PARAMETERS:
 *	 	
 *	 	
 *	 OUTPUT PARAMETERS:
 *	 IMPLICIT INPUTS:
 *	 IMPLICIT OUTPUTS:
 *	 AUTHOR/DATE:
 *	V3.24 FLS Oct-96 Modified for EV6
 ********************************************************************************
 */
{

      int i, mm_dependent=0, br_dependent=0;
      int reg_dst, reg_src;
      CLASS_INFO *class_def; 

      if (mm_stat_reg == br_cond_reg) 
	    return (TRUE);			/* if equal very dependent */


    mm_dependent = 1<<mm_stat_reg;		/* set mm_stat reg in a bit mask */
    br_dependent = 1<<br_cond_reg;		/* set br_cond reg in a bit mask */

    for (i=offs_mm_stat; i < offs_br; i++) {
        class_def = instr_array[raw_code[i]->index];

	if (class_def->arch->dest_reg == Rc) {	/* if mm_stat_reg read then store reg_dst bit mask */
	    reg_dst = raw_code[i]->bits.op_format.rc;
	    reg_src = raw_code[i]->bits.op_format.ra;
	    if (reg_src == mm_stat_reg)
                mm_dependent = mm_dependent | 1<<reg_dst;  /* write register bit  dependent on mm_stat_reg */	    
	    if (reg_dst == br_cond_reg)
                    br_dependent = br_dependent | 1<<reg_src;  /* write register bit  dependent on br_cond_reg*/	    
	}
        
    } /* for.. */
    if (mm_dependent & br_dependent) 		/* if any dependent register bits return true */
         return(TRUE);          
    else return(FALSE);
}


static void check_cycle_restrictions (int length)
/*
 ********************************************************************************
 *	PURPOSE:
 *		NOT USED BY EV6.
 *	INPUT PARAMETERS:
 *	        The length of the code. This tells us how far to look in runtime[].
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:
 *	AUTHOR/DATE:
 *	        Greg Coladonato, August 1991
 *	V3.24 FLS renamed from check_ev6_restrictions. This is called by body.c
 *		  but for EV6 does nothing, all checking is done when body.c
 *		  calls the chip specific schedule_code routine.
 ********************************************************************************
 */
{
    wr (FLG$SCHED2, "\n\n");
}



static void tick_premap_stage(INSTR *stage[],int width)
/*
 ********************************************************************************
 *	PURPOSE:
 *		Print tick  suppport routine,
 *	INPUT PARAMETERS:
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:
 ********************************************************************************
 */
{
  
  int i;
  for ( i = 0; i < width ; i++ ) {
    if (i != 0) wr (FLG$DEBUG_DUMP,"\n           ");
    if ( stage[i] == 0 ) continue;
    wr (FLG$DEBUG_DUMP,"cycle:%4d inum:%-4d %4x: %s",cycle_count,stage[i]->inst_num,stage[i]->address,stage[i]->decoded); /* addr: instruction */
  }
}


static void tick_stage(INSTR *stage[],int width)
/*
 ********************************************************************************
 *	PURPOSE:
 *		Print tick suppport routine,
 *	INPUT PARAMETERS:
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:
 ********************************************************************************
 */
{
  int i,j;
  char iflag;
  char rflag;

  for ( i,j = 0; i < width ; i++ ) {
    if (j==4) {
	 wr (FLG$DEBUG_DUMP,"\n          ");
	 j=0;
    } else j++;
    if ( stage[i] == 0 ) wr (FLG$DEBUG_DUMP,"                                  ");
    else {
      if (stage[i]->issued) iflag = 'i'; else iflag = ' ';
      if (stage[i]->retire_time == -1) rflag = 'r'; else rflag = ' ';
      wr (FLG$DEBUG_DUMP,"inum:%-4d%c%c %4x    ",stage[i]->inst_num, iflag, rflag,stage[i]->address); /* print address */
    }
  }
}




static void tick()
/*
 ********************************************************************************
 *	PURPOSE:
 *		Print tick file info.
 *	INPUT PARAMETERS:
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:
 ********************************************************************************
 */
{
  int i,j;
  int temp,cnt;
  char ifree[MAX_EREG];
  char ffree[MAX_FREG];

 /* This is previous cycle stall reason */
 if (tick_stall_reason && cycle_count != 0 ) 
      wr (FLG$DEBUG_DUMP,"\nStall Reason: %08x",tick_stall_reason);
 else wr (FLG$DEBUG_DUMP,"\nBusy Reason: %08x",tick_busy_reason);
 tick_stall_reason=0;
 tick_busy_reason=0;

 /* current cycle */
 wr (FLG$DEBUG_DUMP,"\n\n\n====> Tick number:%-4d Cycle:%-4d permutation:%-4d\n",debug_dump_tick_num,cycle_count,perm);

    /* To make this go as fast as possible don't use IS_IREG_FREE */
    /* ebox free regs */
    i=0;
    temp = ifree1[0];
    for (j=0; j<32; j++) {
      if (temp & 0x1)
           ifree[i+j]='1';		/* perf model used 'f', but too confusing to me */
      else ifree[i+j]='0';		/* mark it free */
      temp = temp >> 1;
    }
    i=32;
    temp = ifree2[0];
    for (j=0; j<32; j++) {
      if (temp & 0x1)
           ifree[i+j]='1';		/* perf model used 'f', but too confusing to me */
      else ifree[i+j]='0';		/* mark it free */
      temp = temp >> 1;
    }
    /* fls pvc needs third reg in case 32 bit vax */
    i=64;
    temp = ifree3[0];
    for (j=0; j<32; j++) {
      if (i+j > MAX_EREG) break;
      if (temp & 0x1)
           ifree[i+j]='1';		/* perf model used 'f', but too confusing to me */
      else ifree[i+j]='0';		/* mark it free */
      temp = temp >> 1;
    }


    /* fbox free regs */
    i=0;
    temp = ffree1[0];
    for (j=0; j<32; j++) {
      if (temp & 0x1)
           ffree[i+j]='1';		/* perf model used 'f', but too confusing to me */
      else ffree[i+j]='0';		/* mark it free */
      temp = temp >> 1;
    }
    i=32;
    temp = ffree2[0];
    for (j=0; j<32; j++) {
      if (temp & 0x1)
           ffree[i+j]='1';		/* perf model used 'f', but too confusing to me */
      else ffree[i+j]='0';		/* mark it free */
      temp = temp >> 1;
    }
    /* fls pvc needs third reg in case 32 bit vax */
    i=64;
    temp = ffree3[0];
    for (j=0; j<32; j++) {
      if (i+j > MAX_FREG) break;
      if (temp & 0x1)
           ffree[i+j]='1';		/* perf model used 'f', but too confusing to me */
      else ffree[i+j]='0';		/* mark it free */
      temp = temp >> 1;
    }


  if (group_retire_ptr != inflight_tail)
    cnt = DIFF(inflight_tail,group_retire_ptr,MAX_INFLIGHT);
  else if (inflight[inflight_tail] != 0)
    cnt = MAX_INFLIGHT;
  else 
    cnt = 0;

  wr (FLG$DEBUG_DUMP,"\ncycle: %4d  ifree_cnt[0]: %d  ffree_cnt[0]: %d inums_not_retired: %d  retPtrs(%d,%d)%c  nxtRet: %d  eQ: %d  fQ: %d\n",
	  cycle_count,
	  ifree_cnt[0],
	  ffree_cnt[0],
	  group_inum_count,
	  retire_ptr,group_retire_ptr,
	  (retire_stall & 0x1)?'S':'.',
	  (inflight[retire_ptr]!=0)?inflight[retire_ptr]->inst_num:0,
	  inst_in_ebox,
	  inst_in_fbox);


 wr (FLG$DEBUG_DUMP,
 "\n        freereg0                                                                               freereg80");

  wr (FLG$DEBUG_DUMP,"\nfree_int_list: ");
  for (i=0;i<MAX_EREG;i+=8) {
    wr (FLG$DEBUG_DUMP,"%c%c%c%c%c%c%c%c ",
	    ifree[i+0],
	    ifree[i+1],
	    ifree[i+2],
	    ifree[i+3],
	    ifree[i+4],
	    ifree[i+5],
	    ifree[i+6],
	    ifree[i+7]);
  }
  wr (FLG$DEBUG_DUMP,"\nfree_fp_list:  ");
  for (i=0;i<MAX_FREG;i+=8) {
    wr (FLG$DEBUG_DUMP,"%c%c%c%c%c%c%c%c ",
	    ffree[i+0],
	    ffree[i+1],
	    ffree[i+2],
	    ffree[i+3],
	    ffree[i+4],
	    ffree[i+5],
	    ffree[i+6],
	    ffree[i+7]);
  }
  wr (FLG$DEBUG_DUMP,"\n\n");

  wr (FLG$DEBUG_DUMP," icache:   ");
  tick_premap_stage(stage_icache,ICACHE_WIDTH);
  wr (FLG$DEBUG_DUMP,"\n\n    slot:  ");
  tick_premap_stage(stage_slot,SLOT_WIDTH);
  wr (FLG$DEBUG_DUMP,"\n\n     map: ");
  tick_stage(stage_map,MAP_WIDTH);
  wr (FLG$DEBUG_DUMP,"\n\n   eboxq: ");
  tick_stage(ebox_queue,EBOX_QUEUE_ENTRIES);

  wr (FLG$DEBUG_DUMP,"\n    ereg: ");
  tick_stage(tick_stage_reg_ebox,EBOX_WIDTH);

  wr (FLG$DEBUG_DUMP,"\n   fboxq: ");
/*fls was   tick_stage(fbox_queue,FBOX_QUEUE_ENTRIES); */
  tick_stage(fbox_queue,fbox_queue_ptr);

  wr (FLG$DEBUG_DUMP,"\n    freg: ");
  tick_stage(tick_stage_reg_fbox,FBOX_WIDTH);
  wr (FLG$DEBUG_DUMP,"\n");

  /* Register dirty bits */

  wr (FLG$DEBUG_DUMP,"            ");
  for (i = 0; i < MAX_EREG; i++)
    if ((i % 10) == 0)
      wr (FLG$DEBUG_DUMP,"%1d",i/10);
    else
      wr (FLG$DEBUG_DUMP," ");
  wr (FLG$DEBUG_DUMP,"\n");
  wr (FLG$DEBUG_DUMP,"            ");
  for (i = 0; i < MAX_EREG; i++)
    wr (FLG$DEBUG_DUMP,"%1d",i%10);
  wr (FLG$DEBUG_DUMP,"\n");
  wr (FLG$DEBUG_DUMP,"int_dirty0: ");
  for ( i = 0; i < MAX_EREG ; i++ ) 
    if ( reg_dirty[i].d0 > cycle_count) wr (FLG$DEBUG_DUMP,"d"); 
    else wr (FLG$DEBUG_DUMP,".");
  wr (FLG$DEBUG_DUMP,"\n");
  wr (FLG$DEBUG_DUMP,"int_dirty1: ");
  for ( i = 0; i < MAX_EREG ; i++ ) 
    if ( reg_dirty[i].d1 > cycle_count) wr (FLG$DEBUG_DUMP,"d"); 
    else wr (FLG$DEBUG_DUMP,".");
  wr (FLG$DEBUG_DUMP,"\n\n");


  wr (FLG$DEBUG_DUMP,"            ");
  for (i = MAX_EREG; i < MAX_EREG+MAX_FREG; i++)
    if ((i % 10) == 0)
      wr (FLG$DEBUG_DUMP,"%1d",i/100);
    else
      wr (FLG$DEBUG_DUMP," ");
  wr (FLG$DEBUG_DUMP,"\n");
  wr (FLG$DEBUG_DUMP,"            ");
  for (i = MAX_EREG; i < MAX_EREG+MAX_FREG; i++)
    if ((i % 10) == 0)
      wr (FLG$DEBUG_DUMP,"%1d",(i>=100)?((i-100)/10):(i/10));
    else
      wr (FLG$DEBUG_DUMP," ");
  wr (FLG$DEBUG_DUMP,"\n");
  wr (FLG$DEBUG_DUMP,"            ");
  for (i = MAX_EREG; i < MAX_EREG+MAX_FREG; i++)
    wr (FLG$DEBUG_DUMP,"%1d",i%10);
  wr (FLG$DEBUG_DUMP,"\n");
  wr (FLG$DEBUG_DUMP," fp_dirty0: ");
  for (i = MAX_EREG; i < MAX_EREG+MAX_FREG; i++ )
    if ( reg_dirty[i].d0 > cycle_count) wr (FLG$DEBUG_DUMP,"d"); 
    else wr (FLG$DEBUG_DUMP,".");
  wr (FLG$DEBUG_DUMP,"\n");
  wr (FLG$DEBUG_DUMP," fp_dirty1: ");
  for (i = MAX_EREG; i < MAX_EREG+MAX_FREG; i++ ) {
    if ( reg_dirty[i].d0 > cycle_count) wr (FLG$DEBUG_DUMP,"d"); 
    else wr (FLG$DEBUG_DUMP,".");
  }
  wr (FLG$DEBUG_DUMP,"\n\n");

/*  wr (FLG$DEBUG_DUMP,"IPR SCBD: %08x   MB STALL:%d   STC STALL:%d\n",iq_ipr_bits,iq_mb_stall,iq_stc_stall); */
  
 wr (FLG$DEBUG_DUMP,"IPR SCBD: %08x \n",iq_ipr_bits);
 

/*  wr (FLG$DEBUG_DUMP,"\n  dcache: "); */
/*  tick_mem_stage(mem_pipe[DCACHE_STAGE],MAX_MEM_WIDTH); */
  wr (FLG$DEBUG_DUMP,"\n");

}


