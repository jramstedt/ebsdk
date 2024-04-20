/* ev4.c -- Implement the ev4 processor, in particular */

/*
 *	  
 *	  Copyright (c) 1992
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
 * 	  V3.24 FLS Aug-96	Added more restrictions to the EV4 module so 
 *		it could recognize potential problems with doing an 
 *		MTPR PAL_TEMP, MFPR PAL_TEMP sequence without the intervening 
 *		3 cycles.
 *		This was originally added by
 *		R Cruz in June 96, but had to rewrite due to error numbers
 *		changing. This solution is ugly - had to add a special
 *		use_paltemp_types table to avoid changing pvc error numbers.
 *		- added hw_rei decode to decode_hw_memory 
 *	  0     gpc     AUG-92          First Created
 *	  
 */

/*
 * This module must implement the following function
 *
 *	void ev4_init (Chip *)
 *		
 *		This function must assign valid function pointers to
 *		each of the fields of the structure passed as an argument.
 */

#include "types.h"
#include "ispdef.h"
#include "rest.h"
#include "sched.h"
#include "memory.h"
#include "ev4.h"
#include "opcode.h"	/*V3.4*/
 
/* dual issue classes */
enum di_classes {di_NONE, di_EOP, di_FOP, di_LD, di_IST, di_FST,
		   di_IBR, di_FBR, di_JSR, di_MAX};
static int di_table[di_MAX][di_MAX];

/* producer-consumer classes */
enum pc_classes {pc_NONE, pc_LD, pc_ST, pc_IBR, pc_JSR, pc_IADDLOG,
		   pc_SHIFTCM, pc_ICMP, pc_IMULL, pc_IMULQ, pc_FBR,
		   pc_FPOP, pc_FDIV, pc_ST_A, pc_ST_D, pc_MAX};
static int reg_usage[pc_MAX][32];
static int latency_matrix[pc_MAX][pc_MAX];

/* pal resources */
enum res_types {res_TB_TAG=0, res_ITB_PTE=1, res_ICCSR=2, res_ITB_PTE_TEMP=3,
			 res_EXC_ADDR=4, res_ITBZAP=6, res_ITBASM=7, res_ITBIS=8, 
			 res_PS=9, res_EXC_SUM=10, res_SIRR=13, res_ASTRR=14, 
			 res_HIER=16, res_SIER=17, res_ASTER=18, res_DTB_CTL=32, 
			 res_DTB_PTE=34, res_DTB_PTE_TEMP=35, res_DTBZAP=38, 
			 res_DTBASM=39, res_DTBIS=40, res_ALT_MODE=47, res_FLUSH_IC=53, 
			 res_FLUSH_IC_ASM=54, res_PAL_TEMP=64, res_NONE=65, res_DTB=66, 
			 res_VIRT=67, res_ALTVIRT=68, res_DCACHE=69, res_HWLC=70,
			 res_MAX=75};

/* V3.24 ugly, but use_types table can't be changed since PVC error numvers depend on RES_MAX=75 */
enum use_paltemp_types {Paltemp0=64, Paltemp1, Paltemp2,
                         Paltemp3, Paltemp4, Paltemp5, Paltemp6, Paltemp7, Paltemp8, Paltemp9, 
                         Paltemp10, Paltemp11, Paltemp12, Paltemp13, Paltemp14, Paltemp15, Paltemp16,
                         Paltemp17, Paltemp18, Paltemp19, Paltemp20, Paltemp21, Paltemp22,
                         Paltemp23, Paltemp24, Paltemp25, Paltemp26, Paltemp27, Paltemp28,
                         Paltemp29, Paltemp30,  Paltemp31, 
			 };


/* uses of these resources */ 
enum use_types {use_MT=		(1 * res_MAX),
			 use_MF=	(2 * res_MAX),
			 use_HW_REI=	(3 * res_MAX),
			 use_MEM=	(4 * res_MAX),
			 use_MAX=	(6 * res_MAX),
			 use_MFpaltemp=	(7 * res_MAX),		/* V3.24 */
			 use_MTpaltemp=	(8 * res_MAX)};		/* V3.24 */

struct reg {
      int dirty;
      int address;
};



extern CYCLE 	*runtime[];
extern CLASS_INFO *instr_array[];
extern int delay, interrupt;
extern List *restrictions[];
extern int pal_warning_count;
extern int pal_error_count;

extern char pheader[];

extern int wr (int, char *, ...);
extern void print_cycle (int, int);
extern int is_jsr_stack (int index);
extern void kill_old_caveats(List *list);

int check_position_one (CYCLE *c, struct reg *regs);
int check_position_two (CYCLE *c, struct reg *regs);
int check_dangerous_loads (int length);
int check_dangerous_cbranchs(int length);
int check_cbr(CYCLE *c);
 


/*
 *	Functions declared in this module
 *
 *		This had to be done so that their invocations would compile 
 *		without compiler messages.
 *
 */

static void check_restrictions (int length);
static void special_case (INSTR *inst, int cycle);
static int not_preceded_by_mbs (int cycle);
static void mt_dtbis_case (INSTR *inst, int cycle);
static void mt_itb_case (INSTR *inst, int cycle);
static int inspect_instr (INSTR *defendant, List *list);
static ERR_MSG *check_for_problems (INSTR *plaintiff, List *list);
static void init_di_table(void);
static void init_lat_matrix (void);
static void init_restrictions (void);
static void init_classes(void);
static void init_decode_tables (void);
static void decode_ipr (disp_table *);
static void decode_hw_memory (disp_table *);
static int update_scoreboard (INSTR *this_inst, int cycle, CLASS_INFO *class_def);
static int schedule_code (INSTR *raw_code[], int delay_flag, int *length);
static int check_scoreboard (INSTR *this_inst, int cycle, CLASS_INFO *class_def);
static void init_ev4 (void);
static void do_nothing(void);




void ev4_init (Chip *c)
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
 *	FUNCTIONS CALLED DIRECTLY:
 *	AUTHOR/DATE:
 *	        Greg Coladonato, July 92
 ********************************************************************************
 */
{
      c->restriction_check = check_restrictions;
      c->chip_init = init_ev4;
      c->schedule_code = schedule_code;
      c->decode_ipr = decode_ipr;
      c->decode_hw_memory = decode_hw_memory;
      c->at_pal_entry = do_nothing;
      c->at_pal_return = do_nothing;

}



static void init_ev4 (void)
/*
 ********************************************************************************
 *	PURPOSE:
 *		To initialize all the ev4 specific tables.
 *	INPUT PARAMETERS:
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Greg Coladonato, Aug 92
 ********************************************************************************
 */
{
      init_di_table();
      init_lat_matrix();
      init_classes();
      init_restrictions();
      init_decode_tables();
}      

static void do_nothing (void)
/*
 ********************************************************************************
 *	PURPOSE:
 *	Placeholder for chip specific at_pal_entry and at_pal_exit that
 *	is not used by ev4.
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
 *	 	To schedule code as the EV4 would actually execute it.
 *	 	Note: this code does not schedule Write after Write conflicts.
 *	 
 *	 	The logical flow of the code is as follows: (extracted from the comments)
 *	 
 *	 	While instructions remain to be scheduled
 *		  Wait until the instruction we're on is ready to execute
 *	 	  Execute the first of the pair
 *	 	  If the following instruction could dual-issue with it, do so.
 *	 	  If not, clear it.
 *	 
 *	 INPUT PARAMETERS:
 *	 	raw_code: An array of the instructions to schedule
 *		delay_flag: whether this function should simulate icache misses
 *		length: how many instructions are in this code
 *	 OUTPUT PARAMETERS:
 *	 	the length of the scheduled code sequence.
 *	 IMPLICIT INPUTS:
 *	 IMPLICIT OUTPUTS:
 * 	FUNCTIONS CALLED DIRECTLY:
 *		check_scoreboard
 *		is_br_or_bsr
 *		is_cond_or_jsr
 *		mem_alloc_cycle
 *		print_cycle
 *		update_scoreboard
 *		wr
 *	 AUTHOR/DATE:
 *	 	Greg Coladonato, November 1991
 * *******************************************************************************
 */
{
      int offs=0,count=0,idx;
      int row,col;
      
      /* clean off the scoreboard */
      for (row=0;row<pc_MAX;row++)
	for (col=0;col<32;col++)
	  reg_usage[row][col]=0;
      
      
      for (offs=0,count=0;raw_code[offs];offs++,count++) 
	{
	      idx = raw_code[offs]->index;
	      
	      if (delay_flag && raw_code[offs]->address % 32 == 0) 
		{
		      
		      /* 
		       * when we're on a cache boundary, we have to decrement a certain number 
		       * of cycles off the active caveats to simulate a cache miss. 
		       */
		      int cache_count;
		      wr (FLG$SCHED2, "Cycle:     These stalls simulate a cache miss\n");
		      for (cache_count = 0; cache_count < delay; cache_count++) 
			{
			      runtime[count] = Mem_Alloc_CYCLE(NULL, NULL);
			      print_cycle(FLG$SCHED2, count);
			      count++;
			}
		}
	      
	      if (delay_flag && 
		  offs && 
		  (is_cond_or_jsr(raw_code[offs-1]->index) || is_br_or_bsr(raw_code[offs-1]->index)) &&
		  (raw_code[offs]->address != raw_code[offs-1]->address + 4)) 
		{
		      /*
		       * The instruction before was a branch that was taken; advance the 
		       * cycle counter to account for a branch bubble. 
		       */
		      wr (FLG$SCHED2, "Cycle:     These stalls simulate a branch bubble\n");
		      runtime[count] = Mem_Alloc_CYCLE(NULL, NULL);
		      print_cycle(FLG$SCHED2, count);
		      count++;
		}
	      
	      /* wait until the instruction we're on is ready to execute */	 
	      while (check_scoreboard(raw_code[offs], count, instr_array[idx]) == FALSE)
		{
		      runtime[count] = Mem_Alloc_CYCLE(NULL, NULL);
		      print_cycle(FLG$SCHED2, count);
		      count++;
		}
	      /* Execute the first of the pair */	 
	      runtime[count] = Mem_Alloc_CYCLE(raw_code[offs], NULL);
	      update_scoreboard (raw_code[offs], count, instr_array[idx]);
	      
	      /* If the following instruction could dual-issue with it, do so. */	 
	      if ((raw_code[offs+1] != NULL) && 
		  (raw_code[offs]->address % 8 == 0) &&
		  (raw_code[offs+1]->address - raw_code[offs]->address == 4) &&
		  (di_table[instr_array[idx]->imp->di_class]
		   [instr_array[raw_code[offs+1]->index]->imp->di_class]) &&
		  (check_scoreboard (raw_code[offs+1], count, instr_array[raw_code[offs+1]->index]))) 
		{
		      offs++;
		      runtime[count]->is2 = raw_code[offs]; 
		      update_scoreboard (raw_code[offs], count, instr_array[raw_code[offs]->index]);
		}
	      
	      print_cycle(FLG$SCHED2, count);
	}
      *length = count;
return(1);
}



static int update_scoreboard (INSTR *this_inst, int cycle, CLASS_INFO *class_def) 
/*
 ********************************************************************************
 *	 PURPOSE: Update the register file scoreboard whenever a command issues that
 *	 	writes to that register. This is necessary for scheduling purposes.  
 *	 INPUT PARAMETERS:
 *	 	this_inst: The instruction we'd like to execute
 *	 	cycle: the cycle that we're at
 *	 	class_def: A desciption of the behavior of this instruction
 *	 OUTPUT PARAMETERS:
 *	 	TRUE or FALSE
 *	 IMPLICIT INPUTS:
 *	 	A constant pc_MAX
 *	 IMPLICIT OUTPUTS:
 *	 AUTHOR/DATE:
 *	 Greg Coladonato, November 1991
 ********************************************************************************
 */
{
      int prod = class_def->imp->lat_class, reg;
      
      if (class_def->arch->dest_reg == Ra) reg = this_inst->bits.op_format.ra;	
      else if (class_def->arch->dest_reg == Rc) reg = this_inst->bits.op_format.rc;
      else reg = -1;
      
      if (reg == -1 || reg == 31) return(1);
      else 
	{
	      int i;
	      for (i = 1; i < pc_MAX; i++)
		reg_usage[i][reg] = cycle + latency_matrix[prod][i];
	}
return(1);
}

static void decode_ipr (disp_table *entry)
/*
 ********************************************************************************
 *	PURPOSE:
 *		since each chip has its own IPRs, a chip-specific function is 
 *		necessary to decode them.
 *	INPUT PARAMETERS:
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Greg Coladonato, July 92
 ********************************************************************************
 */
{
      extern union INSTR_FORMAT instruction;
      extern char  *text;
      extern B64   *pc;
      
      sprintf(text,"%-12s", entry->text);
      /*    if (instruction.op_format.opcode == EVX$OPC_HW_MTPR) */
      sprintf(&text[strlen(text)],"%s, ",
	      &reg_name_table[instruction.op_format.ra][0]);
      
      if ((instruction.epi_format.func & 
	   (EVX$IPR_PAL | EVX$IPR_IBOX | EVX$IPR_ABOX)) == 0)
	sprintf(&text[strlen(text)],"NOP");
      
      if (instruction.epi_format.func & EVX$IPR_PAL)
	{
	      sprintf(&text[strlen(text)],"%s",
		      &ipr_name_table[instruction.epi_format.func & 0x9F][0]);
	      if (instruction.epi_format.func & (EVX$IPR_IBOX | EVX$IPR_ABOX))
		sprintf(&text[strlen(text)],"_");
	};
      
      if (instruction.epi_format.func & EVX$IPR_IBOX)
	{
	      sprintf(&text[strlen(text)],"%s",
		      &ipr_name_table[instruction.epi_format.func & 0x3F][0]);
	      if (instruction.epi_format.func & (EVX$IPR_ABOX))
		sprintf(&text[strlen(text)],"_");
	};
      if (instruction.epi_format.func & EVX$IPR_ABOX)
	{
	      sprintf(&text[strlen(text)],"%s",
		      &ipr_name_table[instruction.epi_format.func & 0x5F][0]);
	};
}


static void decode_hw_memory (disp_table *entry)
/*
 ********************************************************************************
 *	PURPOSE:
 *		This decode is processor specific.
 *	INPUT PARAMETERS:
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:
 *	AUTHOR/DATE:
 *	        Greg Coladonato, July 92
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
      
          if (instruction.hwmem_format.quad)
        	sprintf(&text[strlen(text)],",QUAD");
          else
        	sprintf(&text[strlen(text)],",LONG");
      
          if (instruction.hwmem_format.physical)
        	sprintf(&text[strlen(text)],",PHYSICAL");
          else
        	sprintf(&text[strlen(text)],",VIRTUAL");
      
          if (instruction.hwmem_format.modify)
        	sprintf(&text[strlen(text)],",MODIFY");
          
         if (instruction.hwmem_format.alt_mode)
        	sprintf(&text[strlen(text)],",ALT_MODE");
      }
}




static int check_scoreboard (INSTR *this_inst, int cycle, CLASS_INFO *class_def) 
/*
 *******************************************************************************
 *	 PURPOSE:
 *	 	This function returns a value of TRUE or FALSE depending on whether
 *	 	or not the instruction passed to it can issue on this cycle, given the
 *	 	state of the registers.
 *
 *		don't remove any of the braces in this function. They're all necessary
 *	 INPUT PARAMETERS:
 *	 	this_inst:  The instruction we'd like to issue
 *	 	cycle:      The current cycle
 *	 	class_def:  The structure defining the behavior of this instruction
 *	 OUTPUT PARAMETERS:
 *	 	TRUE or FALSE
 *	 IMPLICIT INPUTS:
 *	 IMPLICIT OUTPUTS:
 *	 AUTHOR/DATE:
 *	 Greg Coladonato, November 1991
 ********************************************************************************
 */
{
      int clean = 0, cons = class_def->imp->lat_class;
      
      /*	 
       *  Have different cases for store, to distinguish between store data 
       *  conflicts and store address conflicts.
       */	 
      if (cons == pc_ST) 
	{
	      if (class_def->arch->op_ra) 
		{
		      if (reg_usage[pc_ST_D][this_inst->bits.op_format.ra] <= cycle) clean++;
		}
	      else clean++;
	      
	      if (class_def->arch->op_rb) 
		{
		      if (reg_usage[pc_ST_A][this_inst->bits.op_format.rb] <= cycle) clean++;
		}	
	      else clean++;
	      
	      clean++;  /* stores never use r3 */
	}
      
      /*	 
       *  For all other instructions. 
       */	 
      else 
	{
	      if (class_def->arch->op_ra) 
		{
		      if (reg_usage[cons][this_inst->bits.op_format.ra] <= cycle) clean++;
		}
	      else clean++;
	      
	      /*	 
	       *  Don't scoreboard literals.
	       */	 
	      if (class_def->arch->op_rb) 
		{
		      if (class_def->arch->chk_lit_mode && this_inst->bits.opl_format.litflag) clean++;
		      else if (reg_usage[cons][this_inst->bits.op_format.rb] <= cycle) clean++;
		}	
	      else clean++;
	      
	      if (class_def->arch->op_rc) 
		{
		      if (reg_usage[cons][this_inst->bits.op_format.rc] <= cycle) clean++;
		}
	      else clean++;
	}
      
      if (clean == 3) return (TRUE);
      else return (FALSE);
}



static void check_restrictions (int length)
/*
 ********************************************************************************
 *	PURPOSE:
 *	        To take an array of cycles, with 0, 1, or 2 EV4 instructions in each,
 *	        and determine whether it violates any of the PAL restrictions    
 *	INPUT PARAMETERS:
 *	        The length of the code. This tells us how far to look in runtime[].
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:
 *		check_dangerous_loads
 *		check_dangerous_cbranchs
 *		check_for_problems
 *		inspect_instr
 *		is_jsr_stack
 *		new_error
 *		remove_old_caveats
 *		special_case
 *		wr
 *	AUTHOR/DATE:
 *	        Greg Coladonato, December 1991
 ********************************************************************************
 */
    {
    int i, idx1, idx2=0;
    List *caveat_list;
    ERR_MSG *message = NULL;
      
    /* before we do anything else, lets make sure the loads are all kosher */
    check_dangerous_loads(length);
    check_dangerous_cbranchs(length);

    create_L(caveat_list);
      
    if (runtime[0]->is1) 
	{
	idx1 = runtime[0]->is1->index;
	if (runtime[0]->is2) 
	    {
	    idx2 = runtime[0]->is2->index;
	    }
	}
    else 
	{
	idx1 = runtime[delay]->is1->index;
	if (runtime[delay]->is2) 
	    {
	    idx2 = runtime[delay]->is2->index;
	    }
	}
      
    /*	 
    *  Make sure the JSR stack is not used in the first cycle of a flow.
    */	 
    if ( is_jsr_stack (idx1) || is_jsr_stack (idx2) ) 
	{
	wr (FLG$WARN, "%s\n", pheader);
	wr (FLG$WARN, "An instruction that uses the JSR stack can not execute in\n");
	wr (FLG$WARN, "in the first cycle of a PAL flow.\n");
	    pal_warning_count++;
	}
      
    for (i=0;i<length && !interrupt;i++) 
	{
	/*	 
	*  First we have to check that any instructions scheduled to execute
	*  this cycle can do so safely.
	*/	 
	if (runtime[i]->is1 != NULL) 
	    {
	    if  ((message = check_for_problems (runtime[i]->is1, caveat_list)) &&
		new_error (runtime[i]->is1->address, message->errnum)) 
		{
		wr (FLG$ERR, "%s\n", pheader);
		wr (FLG$ERR, "***\nError executing instruction %s at address %X on cycle %d!!\n", 
		    runtime[i]->is1->decoded,
		    runtime[i]->is1->address,
		    i);
		wr (FLG$ERR, "(PVC #%d) %s\n***\n", message->errnum, message->explanation);
		    pal_error_count++;
		}
            if (message) free(message); /* RRC:  Let's free up this space, ok? */

	    if (runtime[i]->is2 != NULL) 
		{
		if ((message = check_for_problems (runtime[i]->is2, caveat_list)) &&
		    new_error (runtime[i]->is2->address, message->errnum))
		    {
		    wr (FLG$ERR, "%s\n", pheader);
		    wr (FLG$ERR, "***\nError executing instruction %s at address %X on cycle %d!!\n", 
			runtime[i]->is2->decoded, 
			runtime[i]->is2->address,
			i);
		    wr (FLG$ERR, "(PVC #%d) %s\n***\n", message->errnum, message->explanation);
			pal_error_count++;
		    }
                if (message) free(message); /* RRC:  Let's free up this space, ok? */
		}
	    }
	      
	    /*	 
	    *  Next, add the caveats that these instructions engender to the list
	    */	 
	    if (runtime[i]->is1 != NULL) 
		{
		if (inspect_instr (runtime[i]->is1, caveat_list)) 
		    {
		    special_case (runtime[i]->is1, i);
		    }	
		if (runtime[i]->is2 != NULL) 
		    {
		    if (inspect_instr (runtime[i]->is2, caveat_list))
			special_case (runtime[i]->is2, i);
		    }
		}
	      
	    remove_old_caveats(caveat_list);
	    }
	kill_old_caveats(caveat_list);
	destroy_L(caveat_list, Restriction); 
	wr (FLG$SCHED2, "\n\n");
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
 *		mt_dtbis_case
 *		mt_itb_case
 *		new_warning
 *		not_preceded_by_mbs
 *		wr
 *	AUTHOR/DATE:
 *	        Greg Coladonato, December 1991  
 ********************************************************************************
 */
{
      int ibox, abox, pal, idx, operand;
      
      operand = inst->bits.hwipr_format.reg;
      pal = (operand >> 7) & 1; 
      abox = (operand >> 6) & 1;
      ibox = (operand >> 5) & 1;
      idx = (operand % 32);
      
      switch (inst->index) 
	{
	    case DO_HW_MTPR:
	      /*	 
	       *  If there is a MT ITB_PTE, ITB_PTE_TEMP, TB_TAG, make sure that
	       *	there are no loads on the cycle before.
	       */	 
	      if (ibox && (idx == 0 || idx == 1 || idx == 3)) 
		mt_itb_case (inst, cycle);

	      /*	 
	       *  This is for making sure that we don't bypass into a MT DTBIS.
	       */	 
	      if (abox && idx == 8) 
		mt_dtbis_case (inst, cycle);

	      /*
	       *  Two MB's should follow any move to the pal base.
	       */
	      if ( ibox && idx == 11 ) 
		{
		      if (not_preceded_by_mbs(cycle)) 
			{
			      if (new_warning (inst->address, 0)) 
				{
				      wr (FLG$ERR, "%s\n", pheader);
				      wr (FLG$ERR, "You'd best put two MB's before that MT PAL_BASE");
				      wr (FLG$ERR, " in order isolate potential MCHK's\n");
				      wr (FLG$ERR, "Address: %X\n", inst->address);
			              pal_error_count++;
				}
			} 
		}
	      break;
	      
	    case DO_HW_MFPR:
	      if ( (abox + ibox + pal) > 1) 
		{
		      if (new_warning (inst->address, 0)) 
			{
			      wr (FLG$ERR, "%s\n", pheader);
			      wr (FLG$ERR, "An MF can have one and only one source.\n\n");	/*V3.24*/
			      wr (FLG$ERR, "Address: %X\n", inst->address);
			      pal_error_count++;
			}
		}
	      break;
	}
}



static int not_preceded_by_mbs (int cycle)
/*
 ********************************************************************************
 *	PURPOSE:
 *		When moving a value to PAL_BASE, we have to make sure there are
 *		two MB's immediately before it. Since a cache block could happen
 *		between the MBs and the first MB could dual issue with another
 *		instruction, checking for two MBs without dereferencing a null
 *		pointer is not a trivial task.
 *	INPUT PARAMETERS:
 *		cycle: where we are in the runtime code.
 *	OUTPUT PARAMETERS:
 *		TRUE or FALSE depending on the truth or falsehood of the predicate
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Greg Coladonato, June 92
 ********************************************************************************
 */
{
      if ((runtime[cycle-1] == NULL || runtime[cycle-2] == NULL) ||
	  (runtime[cycle-1]->is1 == NULL || runtime[cycle-2]->is1 == NULL) ||
	  (runtime[cycle-1]->is1->index != DO_MB || 
		    ((runtime[cycle-2]->is2) ? (runtime[cycle-2]->is2->index != DO_MB) :
					      (runtime[cycle-2]->is1->index != DO_MB)))) 
	{
	      /* the case is not strictly mb, mb, mt pal_base. we have to look more in depth */
	      /* this was changed to account for the fact that the first MB could dual issue */
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
		      /* we didn't find enough MB's in a row. */
		      return (TRUE);
	      else 
		      /* we found that at least two MBs immediately precede this instruction */
		      return (FALSE);
	}
      else 
	      /* this mt pal_base has two mb's right before it. */
	      return (FALSE);
}



static void mt_dtbis_case (INSTR *inst, int cycle)
/*
 ********************************************************************************
 *	PURPOSE:
 *		To isolate the functionality of checking for the MT DTBIS case.
 *		For some odd hardware reason, the source register for a DTBIS
 *		cannot be bypassed in from the result of an operate.
 *	INPUT PARAMETERS:
 *		inst: The instruction that's the DTBIS
 *		cycle: the cycle that it's on in the code.
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *		warning messages if this rule is violated.
 *	FUNCTIONS CALLED DIRECTLY:
 *		new_warning
 *		wr
 *	AUTHOR/DATE:
 *	        Greg Coladonato, June 92
 ********************************************************************************
 */
{
      int i, ok=TRUE;
      
      for (i=0;i<4 && (cycle-i) >= 0;i++) 
	{
	      /*	 
	       *  If an instruction issues on this cycle, check to make
	       *	sure it's destination register is not our source register.
	       */	 
	      if (runtime[cycle-i]->is1) 
		{
		      switch (instr_array[runtime[cycle-i]->is1->index]->arch->dest_reg) 
			{
			    case Ra:
			      if (inst->bits.op_format.ra == runtime[cycle-i]->is1->bits.op_format.ra)
				ok = FALSE;
			      break;
			    case Rc:
			      if (inst->bits.op_format.ra == runtime[cycle-i]->is1->bits.op_format.rc)
				ok = FALSE;
			      break;
			}
		      if (runtime[cycle-i]->is2) 
			{
			      switch (instr_array[inst->index]->arch->dest_reg) 
				{
				    case Ra:
				      if (inst->bits.op_format.ra == runtime[cycle-i]->is2->bits.op_format.ra)
					ok = FALSE;
				      break;
				    case Rc:
				      if (inst->bits.op_format.ra == runtime[cycle-i]->is2->bits.op_format.rc)
					ok = FALSE;
				      break;
				}
			}
		}
	}
      if (!ok) 
	{
	      if (new_warning (inst->address, 0))
		{
		      wr (FLG$ERR, "%s\n", pheader);
		      wr (FLG$ERR, "You bypassed into this DTBIS! Illegal!!\n");
		      wr (FLG$ERR, "Address: %X\n", inst->address);
		      pal_error_count++;
		}
	}
}



static void mt_itb_case (INSTR *inst, int cycle)
/*
 ********************************************************************************
 *	PURPOSE:
 *		To check for violations surrounding the MT ITBx case.
 *		EV4 does not allow there to be a load on the previous cycle
 *	INPUT PARAMETERS:
 *		inst: The instruction that's the DTBIS
 *		cycle: the cycle that it's on in the code.
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *		warning messages if this rule is violated.
 *	FUNCTIONS CALLED DIRECTLY:
 *		new_warning
 *		wr
 *	AUTHOR/DATE:
 *	        Greg Coladonato, June 92
 ********************************************************************************
 */
{
      if (runtime[cycle-1]->is1 != NULL) 
	{
	      switch (runtime[cycle-1]->is1->index) 
		{ 
		    case DO_LDL:	case DO_LDLL:	case DO_LDQ:
		    case DO_LDQL:	case DO_LDQU:	case DO_LDD:
		    case DO_LDS:	case DO_LDT:	case DO_LDF: 
		      if (new_warning (inst->address, 0)) 
			{
			      wr (FLG$ERR, "%s\n", pheader);
			      wr (FLG$ERR, "There can not be a load on the previous cycle. Fix it.\n");
			      wr (FLG$ERR, "Address: %X\n", inst->address);
			      pal_error_count++;

			}
		}
	      if (runtime[cycle-1]->is2 != NULL) 
		{
		      switch (runtime[cycle-1]->is2->index) 
			{ 
			    case DO_LDL:	case DO_LDLL:	case DO_LDQ:
			    case DO_LDQL:	case DO_LDQU:	case DO_LDD:
			    case DO_LDS:	case DO_LDT:	case DO_LDF: 
			      if (new_warning (inst->address, 0)) 
				{
				      wr (FLG$ERR, "%s\n", pheader);
				      wr (FLG$ERR, "There can not be a load on the previous cycle. Fix it.\n");
				      wr (FLG$ERR, "Address: %X\n", inst->address);
			              pal_error_count++;
				}
			}
		}
	}
}



static int inspect_instr (INSTR *defendant, List *list)
/*
 ********************************************************************************
 *	PURPOSE:
 *	        To look at an instruction and determine whether or not it might 
 *	        engender any pal violations down the road. This function is responsible
 *	        for recognizing all possible PALcode restriction violating instructions.
 *	INPUT PARAMETERS:
 *	        defendant: a single instruction
 *		list: the list of things to watch out for, caveats
 *	OUTPUT PARAMETERS:
 *	        status: this is true if this instruction has a warning associated with it.
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:
 *		add_caveat
 *	AUTHOR/DATE:
 *	        Greg Coladonato, December 1991  
 ********************************************************************************
 */
{
      int status = FALSE;
      
      switch (defendant->index) 
	{
	    case DO_HW_MTPR: 
	      {
		    /*	 
		     *  These are the major culprits.
		     */	 
		    int ibox,abox,pal,idx,operand;
		    
		    operand = defendant->bits.hwipr_format.reg;
		    pal = (operand >> 7) & 1; 
		    abox = (operand >> 6) & 1;
		    ibox = (operand >> 5) & 1;
		    idx = (operand % 32);

/* V3.24 in the ev4 beh palcode pal_beh the pal bit is also set for iccsr etc.,  */
		    if (ibox) add_caveat (use_MT + idx, list);
		    else if (abox) add_caveat (use_MT + 32 + idx, list);
		    else if (pal)  add_caveat (use_MTpaltemp + Paltemp0 + idx, list);	/* V3.24 */

		    if (ibox && (idx == 0 || idx == 1 || idx == 3)) status = TRUE;
		    if (abox && idx == 8)	status = TRUE;
		    if (ibox && idx == 11)	status = TRUE;
	      };
	      break;
	    case DO_HW_MFPR: 
	      {
		    /*	 
		     *  Sometimes, we must wait a certain amount of cycles in between
		     *	two MF's.
		     */	 
		    int ibox,abox,pal,idx,operand;
		    
		    operand = defendant->bits.hwipr_format.reg;
		    pal = (operand >> 7) & 1; 
		    abox = (operand >> 6) & 1;
		    ibox = (operand >> 5) & 1;
		    idx = (operand % 32);
		    
/* V3.24 in the ev4 beh palcode pal_beh the pal bit is also set for iccsr etc.,  */
		    if (ibox) add_caveat (use_MF + idx, list);
		    else if (abox) add_caveat (use_MF + 32 + idx, list);
		    else if (pal)  add_caveat (use_MFpaltemp + Paltemp0 + idx, list);	/* V3.24 */

		    if ( (abox + ibox + pal) > 1) status = TRUE;
		    if ( ibox && idx == 11 ) status = TRUE;
	      };
	      break;
	    case DO_HW_ST:
	    case DO_HW_LD: 
	      {
		    /*	 
		     *  Depending on the type of action this instruction is taking, it could
		     *	cause problems.
		     */	 	
		    int alt,phys;
		    
		    alt = defendant->bits.hwmem_format.alt_mode;
		    phys = defendant->bits.hwmem_format.physical;
		    
		    if (alt == TRUE && phys == TRUE)
		      add_caveat (use_MEM + res_HWLC, list);
	      };
	      break;	    	
	    default:
	      /*	 
	       *  This instruction could not be a villain.
	       */
	      break;
	}    
      return (status);
}



static      char  err1[] = "You can't MT DTBIS and ITBIS simultaneously.";
static ERR_MSG *check_for_problems (INSTR *plaintiff, List *list)
/*
 ********************************************************************************
 *	PURPOSE:
 *	        This function is the complement of inspect_instr. This function is 
 *	        responsible for identifying all instructions that might not be able to
 *	        issue yet, and looking in the list for them.
 *	INPUT PARAMETERS:
 *	        plaintiff: An instruction that wants to issue this cycle.
 *	        list: A pointer to a list of events that can NOT happen this cycle	
 *	OUTPUT PARAMETERS:
 *	        A pointer to an explanation structure if there is an error.
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:
 *		search_caveat
 *	AUTHOR/DATE:
 *	        Greg Coladonato, December 1991  
 ********************************************************************************
 */
{
      ERR_MSG *errmsg = NULL;
      switch (plaintiff->index) 
	{
	    case DO_HW_MTPR: 
	      {
		    /*	 
		     *  These are the major culprits.
		     */	 
		    int ibox,abox,pal,idx,operand;
		    
		    operand = plaintiff->bits.hwipr_format.reg;
		    pal = (operand >> 7) & 1; 
		    abox = (operand >> 6) & 1;
		    ibox = (operand >> 5) & 1;
		    idx = (operand % 32);
		    
/* V3.24 in the ev4 beh palcode pal_beh the pal bit is also set for iccsr etc.,  */
		    if (ibox) errmsg = search_caveat (use_MT + idx, list, plaintiff);
		    else if (abox && !errmsg) errmsg = search_caveat (use_MT + 32 + idx, list, plaintiff);
		    else if (pal && !errmsg) errmsg = search_caveat (use_MTpaltemp + Paltemp0 + idx, list, plaintiff);	/* V3.24 */

		    if ((ibox && abox) && (idx == 8) && !errmsg) 
		      {
			    errmsg = (ERR_MSG *) calloc (1, sizeof (ERR_MSG));
			    errmsg->explanation = err1;  /* RRC:  Modified to use static data instead of malloc */
		      }
	      };
	      break;
	    case DO_HW_MFPR: 
	      {
		    /*	 
		     *  Sometimes, we must wait a certain amount of cycles in between
		     *	two MF's.
		     */	 
		    int ibox,abox,pal,idx,operand;
		    
		    operand = plaintiff->bits.hwipr_format.reg;
		    pal = (operand >> 7) & 1; 
		    abox = (operand >> 6) & 1;
		    ibox = (operand >> 5) & 1;
		    idx = (operand % 32);
		    
/* V3.24 in the ev4 beh palcode pal_beh the pal bit is also set for iccsr etc.,  */
		    if (ibox) errmsg = search_caveat (use_MF + idx, list, plaintiff);
		    else if (abox && !errmsg) errmsg = search_caveat (use_MF + 32 + idx, list, plaintiff);
		    else if (pal && !errmsg) errmsg = search_caveat (use_MFpaltemp + Paltemp0 + idx, list, plaintiff);	/* V3.24 */

	      };
	      break;
	    case DO_HW_ST:
	    case DO_HW_LD: 
	      {
		    /*	 
		     *  Depending on the type of action this instruction is taking, it could
		     *	cause problems.
		     */	 	
		    int alt,phys,load;
		    
		    alt = plaintiff->bits.hwmem_format.alt_mode;
		    phys = plaintiff->bits.hwmem_format.physical;
		    load = (plaintiff->index == DO_HW_LD);
		    
		    if (alt && !phys) errmsg = search_caveat (use_MEM + res_ALTVIRT, list, plaintiff);
		    if (!alt && !phys && !errmsg) errmsg = search_caveat (use_MEM + res_VIRT, list, plaintiff);
		    if (!errmsg) errmsg = search_caveat (use_MEM + res_DTB, list, plaintiff);
		    if (load && !errmsg) errmsg = search_caveat (use_MEM + res_DCACHE, list, plaintiff);	
		    
		    
	      };
	      break;	    	
	    case DO_HW_REI: 
	      {
		    /*	 
		     *  It is often the case that we can't HW_REI until all the
		     *	I-Stream resources (ITB) are unallocated. 
		     */	 
		    errmsg = search_caveat (use_HW_REI + res_NONE, list, plaintiff);

		    /* also check that the hw_rei, is correctly specified */
		    if (plaintiff->bits.instr != 0x7BFF8000)
			{
			wr (FLG$ERR, "%s\n", pheader);
			wr (FLG$ERR, "Address: %X\n", plaintiff->address);
			wr (FLG$ERR, "HW_REI is incorrectly specified, should be 0x7BFF8000\n");
			pal_error_count++;
			}

	      };
	      break;
	    case DO_LDL:	case DO_LDLL:	case DO_LDQ:
	    case DO_LDQL:	case DO_LDQU:	case DO_LDD:
	    case DO_LDS:	case DO_LDT:	case DO_LDF: 
	      
	      errmsg = search_caveat (use_MEM + res_DCACHE, list, plaintiff);	
	      
	    case DO_STL:	case DO_STLC:	case DO_STQ:
	    case DO_STQC:	case DO_STQU:	case DO_STD:
	    case DO_STS:	case DO_STF:	case DO_STT: 
	      {
		    /*	 
		     *  After MT'ing to some IPR's, Virtual Memory accesses are not
		     *	allowed for a number of cycles.
		     */
		    if (!errmsg) errmsg = search_caveat (use_MEM + res_DTB, list, plaintiff);	 
		    if (!errmsg) errmsg = search_caveat (use_MEM + res_VIRT, list, plaintiff);	 
	      };
	    default:
	      /*	 
	       *  This instruction could not be a villain.
	       */
	      break;
	}    
      return (errmsg);
}



static void init_di_table(void)
/*
 ********************************************************************************
 *	 PURPOSE:
 *	 	Initialize the table that is necessary to determine whether two   
 *	 	instruction types can be issued at the same time on EV4.
 *	 INPUT PARAMETERS:
 *	 OUTPUT PARAMETERS:
 *	 IMPLICIT INPUTS:
 *	 	This function assumes a di_table matrix has been defined, as
 *	 	well as a constant di_MAX
 *	 IMPLICIT OUTPUTS:
 *	 AUTHOR/DATE:
 *	 	Greg Coladonato, November 1991
 *********************************************************************************
 */
{
      /* 
       * 
       * Initialize the dual issue table
       * 
       *
       * Dual issue table
       * ----------------
       *
       *        eop   fop    ld   ist   fst   ibr   fbr   jsr
       *      +-----+-----+-----+-----+-----+-----+-----+-----+
       *  eop |  0  |  1  |  1  |  1  |  0  |  1  |  0  |  1  |
       *      +-----+-----+-----+-----+-----+-----+-----+-----+
       *  fop |  1  |  0  |  1  |  0  |  1  |  0  |  1  |  0  |   
       *      +-----+-----+-----+-----+-----+-----+-----+-----+
       *  ld  |  1  |  1  |  0  |  0  |  0  |  0  |  0  |  0  | 
       *      +-----+-----+-----+-----+-----+-----+-----+-----+
       *  ist |  1  |  0  |  0  |  0  |  0  |  0  |  1  |  0  |  
       *      +-----+-----+-----+-----+-----+-----+-----+-----+
       *  fst |  0  |  1  |  0  |  0  |  0  |  1  |  0  |  0  |  
       *      +-----+-----+-----+-----+-----+-----+-----+-----+
       *  ibr |  1  |  0  |  0  |  0  |  1  |  0  |  0  |  0  |  
       *      +-----+-----+-----+-----+-----+-----+-----+-----+
       *  fbr |  0  |  1  |  0  |  1  |  0  |  0  |  0  |  0  |  
       *      +-----+-----+-----+-----+-----+-----+-----+-----+
       *  jsr |  1  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |
       *      +-----+-----+-----+-----+-----+-----+-----+-----+
       *	 
       */
      
      int i,j;
      
      for(i=1;i<di_MAX;i++)
	for(j=1;j<di_MAX;j++)
	  di_table[i][j]=0;
      
      ++di_table[di_EOP][di_FOP];
      ++di_table[di_EOP][di_LD];
      ++di_table[di_EOP][di_IST];
      ++di_table[di_EOP][di_IBR];
      ++di_table[di_EOP][di_JSR];
      
      ++di_table[di_FOP][di_EOP];
      ++di_table[di_FOP][di_LD];
      ++di_table[di_FOP][di_FST];
      ++di_table[di_FOP][di_FBR];
      
      ++di_table[di_LD][di_EOP];
      ++di_table[di_LD][di_FOP];
      
      ++di_table[di_IST][di_EOP];
      ++di_table[di_IST][di_FBR];
      
      ++di_table[di_FST][di_FOP];
      ++di_table[di_FST][di_IBR];
      
      ++di_table[di_IBR][di_EOP];
      ++di_table[di_IBR][di_FST];
      
      ++di_table[di_FBR][di_FOP];
      ++di_table[di_FBR][di_IST];
      
      ++di_table[di_JSR][di_EOP];
      
}



static void init_lat_matrix (void)
/*
 ********************************************************************************
 *	 PURPOSE:
 *	 	To initialize the matrix that determines scheduling when there
 *	 	is a register conflict between two instructions.                         
 *	 INPUT PARAMETERS:
 *	 OUTPUT PARAMETERS:
 *	 IMPLICIT INPUTS:
 *	 	It assumes a global variable named lat_matrix has been defined, 
 *	 	as well as a constant pc_MAX
 *	 IMPLICIT OUTPUTS:
 *	 AUTHOR/DATE:
 *	 	Greg Coladonato, November 1991
 ********************************************************************************
 */
{
      /* latency matrix init
       *
       *             LD    JSR   IAL SHIFTCM ICMP IMULL IMULQ  FPOP  FDIV   
       *           +-----+-----+-----+-----+-----+-----+-----+-----+-----+
       * LD        +  3  |  3  |  2  |  2  |  2  |  21 |  23 |  X  |  X  |
       *           +-----+-----+-----+-----+-----+-----+-----+-----+-----+
       * ST_A      +  3  |  3  |  2  |  2  |  2  |  21 |  23 |  X  |  X  |
       *           +-----+-----+-----+-----+-----+-----+-----+-----+-----+
       * ST_D      +  3  |  3  |  0  |  0  |  0  |  20 |  22 |  X  |  X  |
       *           +-----+-----+-----+-----+-----+-----+-----+-----+-----+
       * IBR       +  3  |  3  |  1  |  2  |  1  |  21 |  23 |  X  |  X  |
       *           +-----+-----+-----+-----+-----+-----+-----+-----+-----+
       * JSR       +  3  |  3  |  2  |  2  |  2  |  21 |  23 |  X  |  X  |
       *           +-----+-----+-----+-----+-----+-----+-----+-----+-----+
       * IADDLOG   +  3  |  3  |  1  |  2  |  2  |  21 |  23 |  X  |  X  |
       *           +-----+-----+-----+-----+-----+-----+-----+-----+-----+
       * SHIFTCM   +  3  |  3  |  1  |  2  |  2  |  21 |  23 |  X  |  X  |
       *           +-----+-----+-----+-----+-----+-----+-----+-----+-----+
       * ICMP      +  3  |  3  |  1  |  2  |  2  |  21 |  23 |  X  |  X  |
       *           +-----+-----+-----+-----+-----+-----+-----+-----+-----+
       * IMULL     +  3  |  3  |  1  |  2  |  2  |  21 |  23 |  X  |  X  |
       *           +-----+-----+-----+-----+-----+-----+-----+-----+-----+
       * IMULQ     +  3  |  3  |  1  |  2  |  2  |  21 |  23 |  X  |  X  |
       *           +-----+-----+-----+-----+-----+-----+-----+-----+-----+
       * FBR       +  3  |  X  |  X  |  X  |  X  |  X  |  X  |  6  | 34  |
       *           +-----+-----+-----+-----+-----+-----+-----+-----+-----+
       * FPOP      +  3  |  X  |  X  |  X  |  X  |  X  |  X  |  6  | 34  |
       *           +-----+-----+-----+-----+-----+-----+-----+-----+-----+
       * FDIV      +  3  |  X  |  X  |  X  |  X  |  X  |  X  |  6  | 34  |
       *           +-----+-----+-----+-----+-----+-----+-----+-----+-----+
       */
      
      int i,j;
      
      for(i=1;i<pc_MAX;i++)
	for(j=1;j<pc_MAX;j++)
	  latency_matrix[i][j]=0;
      
      latency_matrix[pc_LD][pc_LD] = 3;
      latency_matrix[pc_LD][pc_ST_A] = 3;
      latency_matrix[pc_LD][pc_ST_D] = 3;
      latency_matrix[pc_LD][pc_IBR] = 3;
      latency_matrix[pc_LD][pc_JSR] = 3;
      latency_matrix[pc_LD][pc_IADDLOG] = 3;
      latency_matrix[pc_LD][pc_SHIFTCM] = 3;
      latency_matrix[pc_LD][pc_ICMP] = 3;
      latency_matrix[pc_LD][pc_IMULL] = 3;
      latency_matrix[pc_LD][pc_IMULQ] = 3;
      latency_matrix[pc_LD][pc_FBR] = 3;
      latency_matrix[pc_LD][pc_FPOP] = 3;
      latency_matrix[pc_LD][pc_FDIV] = 3;
      
      latency_matrix[pc_JSR][pc_LD] = 3;
      latency_matrix[pc_JSR][pc_ST_A] = 3;
      latency_matrix[pc_JSR][pc_ST_D] = 3;
      latency_matrix[pc_JSR][pc_IBR] = 3;
      latency_matrix[pc_JSR][pc_JSR] = 3;
      latency_matrix[pc_JSR][pc_IADDLOG] = 3;
      latency_matrix[pc_JSR][pc_SHIFTCM] = 3;
      latency_matrix[pc_JSR][pc_ICMP] = 3;
      latency_matrix[pc_JSR][pc_IMULL] = 3;
      latency_matrix[pc_JSR][pc_IMULQ] = 3;
      
      latency_matrix[pc_IADDLOG][pc_LD] = 2;
      latency_matrix[pc_IADDLOG][pc_ST_A] = 2;
      latency_matrix[pc_IADDLOG][pc_ST_D] = 0;
      latency_matrix[pc_IADDLOG][pc_IBR] = 1;
      latency_matrix[pc_IADDLOG][pc_JSR] = 2;
      latency_matrix[pc_IADDLOG][pc_IADDLOG] = 1;
      latency_matrix[pc_IADDLOG][pc_SHIFTCM] = 1;
      latency_matrix[pc_IADDLOG][pc_ICMP] = 1;
      latency_matrix[pc_IADDLOG][pc_IMULL] = 1;
      latency_matrix[pc_IADDLOG][pc_IMULQ] = 1;
      
      latency_matrix[pc_SHIFTCM][pc_LD] = 2;
      latency_matrix[pc_SHIFTCM][pc_ST_A] = 2;
      latency_matrix[pc_SHIFTCM][pc_ST_D] = 0;
      latency_matrix[pc_SHIFTCM][pc_IBR] = 2;
      latency_matrix[pc_SHIFTCM][pc_JSR] = 2;
      latency_matrix[pc_SHIFTCM][pc_IADDLOG] = 2;
      latency_matrix[pc_SHIFTCM][pc_SHIFTCM] = 2;
      latency_matrix[pc_SHIFTCM][pc_ICMP] = 2;
      latency_matrix[pc_SHIFTCM][pc_IMULL] = 2;
      latency_matrix[pc_SHIFTCM][pc_IMULQ] = 2;
      
      latency_matrix[pc_ICMP][pc_LD] = 2;
      latency_matrix[pc_ICMP][pc_ST_A] = 2;
      latency_matrix[pc_ICMP][pc_ST_D] = 0;
      latency_matrix[pc_ICMP][pc_IBR] = 1;
      latency_matrix[pc_ICMP][pc_JSR] = 2;
      latency_matrix[pc_ICMP][pc_IADDLOG] = 2;
      latency_matrix[pc_ICMP][pc_SHIFTCM] = 2;
      latency_matrix[pc_ICMP][pc_ICMP] = 2;
      latency_matrix[pc_ICMP][pc_IMULL] = 2;
      latency_matrix[pc_ICMP][pc_IMULQ] = 2;
      
      latency_matrix[pc_IMULL][pc_LD] = 21;
      latency_matrix[pc_IMULL][pc_ST_A] = 21;
      latency_matrix[pc_IMULL][pc_ST_D] = 20;
      latency_matrix[pc_IMULL][pc_IBR] = 21;
      latency_matrix[pc_IMULL][pc_JSR] = 21;
      latency_matrix[pc_IMULL][pc_IADDLOG] = 21;
      latency_matrix[pc_IMULL][pc_SHIFTCM] = 21;
      latency_matrix[pc_IMULL][pc_ICMP] = 21;
      latency_matrix[pc_IMULL][pc_IMULL] = 21;
      latency_matrix[pc_IMULL][pc_IMULQ] = 21;
      
      latency_matrix[pc_IMULQ][pc_LD] = 23;
      latency_matrix[pc_IMULQ][pc_ST_A] = 23;
      latency_matrix[pc_IMULQ][pc_ST_D] = 22;
      latency_matrix[pc_IMULQ][pc_IBR] = 23;
      latency_matrix[pc_IMULQ][pc_JSR] = 23;
      latency_matrix[pc_IMULQ][pc_IADDLOG] = 23;
      latency_matrix[pc_IMULQ][pc_SHIFTCM] = 23;
      latency_matrix[pc_IMULQ][pc_ICMP] = 23;
      latency_matrix[pc_IMULQ][pc_IMULL] = 23;
      latency_matrix[pc_IMULQ][pc_IMULQ] = 23;
      
      latency_matrix[pc_FPOP][pc_FBR] = 6;
      latency_matrix[pc_FPOP][pc_FPOP] = 6;
      latency_matrix[pc_FPOP][pc_FDIV] = 6;
      
      latency_matrix[pc_FDIV][pc_FBR] = 34;
      latency_matrix[pc_FDIV][pc_FPOP] = 34;
      latency_matrix[pc_FDIV][pc_FDIV] = 34;
}



static void init_restrictions (void)
/*
 ********************************************************************************
 *	PURPOSE: 
 *	        To initialize the array of restrictions. The format for this
 *	        code should be simple enough to modify as the restrictions change.
 *
 *	        Note: I assume that two instructions that would cause a PAL
 *	        violation could not possibly issue together. This is true
 *	        for EV-4, as far as I can tell.
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
      clean_restrictions ();
      
      add_restriction (use_MT + res_DTB_CTL, use_MT + res_ITB_PTE, "1",
		       "You can't MT ITB_PTE one cycle after a MT DTB_CTL");

      add_restriction (use_MT + res_DTB_CTL, use_MF + res_ITB_PTE, "1",
		       "You can't MF ITB_PTE one cycle after a MT DTB_CTL");
      
      add_restriction (use_MF + res_DTB_PTE, use_MF + res_DTB_PTE, "1",
		       "You cannot move from the DTB_PTE two consecutive cycles.");

      add_restriction (use_MF + res_DTB_PTE, use_MF + res_DTB_PTE_TEMP, "12",
		       "You must wait two cycles before moving from DTB_PTE_TEMP.");

      add_restriction (use_MF + res_DTB_PTE, use_MEM + res_DTB, "123",
		       "You cannot use the DTB in the three cycles following a MT DTB_PTE.");
      
      add_restriction (use_MT + res_DTB_PTE, use_MT + res_TB_TAG, "12",
		       "You cannot MT TB_TAG within two cycles of writing the DTB_PTE.");

      add_restriction (use_MT + res_DTB_PTE, use_MEM + res_DTB, "123",
		       "You cannot use the DTB during the three cycles following a MT DTB_PTE.");

      add_restriction (use_MT + res_DTB_PTE, use_MF + res_DTB_PTE, "123",
		       "You cannot move from DTB_PTE during for three cycles after a MT DTB_PTE.");
      
      add_restriction (use_MF + res_ITB_PTE, use_MF + res_ITB_PTE, "1",
		       "You cannot move from the DTB_PTE two consecutive cycles.");

      add_restriction (use_MF + res_ITB_PTE, use_MF + res_ITB_PTE_TEMP, "12",
		       "You must wait two cycles before moving from ITB_PTE_TEMP.");

      add_restriction (use_MT + res_ITB_PTE, use_MF + res_ITB_PTE, "123",
		       "You cannot read the ITB_PTE without three intervening cycles of delay.");

      add_restriction (use_MT + res_ITB_PTE, use_MT + res_TB_TAG, "12",
		       "You cannot MT TB_TAG within two cycles of writing the ITB_PTE.");

      add_restriction (use_MT + res_ITB_PTE, use_HW_REI + res_NONE, "12",
		       "You cannot HW_REI in the two cycles following a MT ITB_PTE.");

      add_restriction (use_MT + res_EXC_ADDR, use_HW_REI + res_NONE, "1",
		       "One cycle of delay must lie between a MT EXC_ADDR and the HW_REI.");
/* V3.24 replaced by mt paltempn mf paltempn check 
	add_restriction (use_MT + res_PAL_TEMP, use_MF + res_PAL_TEMP, "2",
		       "You may not MF a pal temp exactly two cycles after you wrote to a pal temp.");
*/
      add_restriction (use_MT + res_PS, use_MEM + res_VIRT, "123",
		       "You cannot use virtual memory until 3 bubbles after a MT PS.");

      add_restriction (use_MT + res_PS, use_MF + res_PS, "123",
		       "You cannot use read the PS until 3 bubbles after a MT PS.");
      
      add_restriction (use_MT + res_PS, use_HW_REI + res_NONE, "12345",
		       "You cannot HW_REI until 5 bubbles after a MT PS.");
      
      add_restriction (use_MT + res_EXC_SUM, use_MF + res_EXC_SUM, "123",
		       "You can't read back from the EXC_SUM until 3 bubbles after writing it.");
      
      add_restriction (use_MT + res_ICCSR, use_MF + res_ICCSR, "123",
		       "You can't read back from the ICCSR until 3 bubbles after writing it.");
      
      add_restriction (use_MT + res_ICCSR, use_HW_REI + res_NONE, "1234567",
		       "You can't HW_REI during the 7 cycles following a MT ICCSR <FEN> or <ASN>");
      
      add_restriction (use_MT + res_ITBZAP, use_HW_REI + res_NONE, "12",
		       "You can't HW_REI during the 2 cycles following a MT ITBZAP, ITBASM, or ITBIS.");
      add_restriction (use_MT + res_ITBASM, use_HW_REI + res_NONE, "12",
		       "You can't HW_REI during the 2 cycles following a MT ITBZAP, ITBASM, or ITBIS.");
      add_restriction (use_MT + res_ITBIS, use_HW_REI + res_NONE, "12",
		       "You can't HW_REI during the 2 cycles following a MT ITBZAP, ITBASM, or ITBIS.");
      
      add_restriction (use_MT + res_DTBZAP, use_MEM + res_DTB, "123",
		       "You can't use the DTB during the 3 cycles following a MT DTBZAP, DTBASM, or DTBIS.");
      add_restriction (use_MT + res_DTBASM, use_MEM + res_DTB, "123",
		       "You can't use the DTB during the 3 cycles following a MT DTBZAP, DTBASM, or DTBIS.");
      add_restriction (use_MT + res_DTBIS, use_MEM + res_DTB, "123",
		       "You can't use the DTB during the 3 cycles following a MT DTBZAP, DTBASM, or DTBIS.");
      
      add_restriction (use_MT + res_ALT_MODE, use_MF + res_ALT_MODE, "1",
		       "You can't read back from the ALT_MODE ipr w/o allowing one cycle inbetween.");

      add_restriction (use_MT + res_ALT_MODE, use_MEM + res_ALTVIRT, "12",
		       "You can't use the ALT_MODE ipr via an ALT_MODE virtual access for 2 bubbles.");
      
      add_restriction (use_MT + res_SIER, use_HW_REI + res_NONE, "123",
		       "You can not HW_REI or read what you wrote in an interrupt register\n\
	until three intervening cycles of delay have passed");
      add_restriction (use_MT + res_SIER, use_MF + res_SIER, "123",
		       "You can not HW_REI or read what you wrote in an interrupt register\n\
	until three intervening cycles of delay have passed");
      
      add_restriction (use_MT + res_SIRR, use_HW_REI + res_NONE, "123",
		       "You can not HW_REI or read what you wrote in an interrupt register\n\
	until three intervening cycles of delay have passed");
      add_restriction (use_MT + res_SIRR, use_MF + res_SIRR, "123",
		       "You can not HW_REI or read what you wrote in an interrupt register\n\
	until three intervening cycles of delay have passed");
      
      add_restriction (use_MT + res_HIER, use_HW_REI + res_NONE, "123",
		       "You can not HW_REI or read what you wrote in an interrupt register\n\
	until three intervening cycles of delay have passed");
      add_restriction (use_MT + res_HIER, use_MF + res_HIER, "123",
		       "You can not HW_REI or read what you wrote in an interrupt register\n\
	until three intervening cycles of delay have passed");
      
      add_restriction (use_MT + res_ASTER, use_HW_REI + res_NONE, "123",
		       "You can not HW_REI or read what you wrote in an interrupt register\n\
	until three intervening cycles of delay have passed");
      add_restriction (use_MT + res_ASTER, use_MF + res_ASTER, "123",
		       "You can not HW_REI or read what you wrote in an interrupt register\n\
	until three intervening cycles of delay have passed");
      
      add_restriction (use_MT + res_ASTRR, use_HW_REI + res_NONE, "123",
		       "You can not HW_REI or read what you wrote in an interrupt register\n\
	until three intervening cycles of delay have passed");
      add_restriction (use_MT + res_ASTRR, use_MF + res_ASTRR, "123",
		       "You can not HW_REI or read what you wrote in an interrupt register\n\
	until three intervening cycles of delay have passed");
      
      add_restriction (use_MT + res_FLUSH_IC, use_HW_REI + res_NONE, "123456789",
		       "You cannot HW_REI for 9 cycles following a MT FLUSH_IC | FLUSH_IC_ASM");
      add_restriction (use_MT + res_FLUSH_IC_ASM, use_HW_REI + res_NONE, "123456789",
		       "You cannot HW_REI for 9 cycles following a MT FLUSH_IC | FLUSH_IC_ASM");
      
      add_restriction (use_MEM + res_HWLC, use_MEM + res_DCACHE, "12",
		       "You can not use the Dcache for the two cycles following a HW_STC."); 
      
      add_restriction (use_MT + res_DTB_PTE, use_HW_REI + res_NONE, "12",
		       "You cannot HW_REI in the two cycles following a MT DTB_PTE.");

      add_restriction (use_MTpaltemp+ Paltemp0, use_MFpaltemp + Paltemp0, "2",	"No MT PALTEMP0 --> MF PALTEMP0 alllowed in cycle 2");
      add_restriction (use_MTpaltemp + Paltemp1, use_MFpaltemp + Paltemp1, "2","No MT  PALTEMP1 --> MF PALTEMP1 alllowed in cycle 2");
      add_restriction (use_MTpaltemp + Paltemp2, use_MFpaltemp + Paltemp2, "2",	"No MT  PALTEMP2 --> MF PALTEMP2 alllowed in cycle 2");
      add_restriction (use_MTpaltemp + Paltemp3, use_MFpaltemp + Paltemp3, "2",	"No MT  PALTEMP3 --> MF PALTEMP3 alllowed in cycle 2");
      add_restriction (use_MTpaltemp + Paltemp4, use_MFpaltemp + Paltemp4, "2",	"No MT  PALTEMP4 --> MF PALTEMP4 alllowed in cycle 2");
      add_restriction (use_MTpaltemp + Paltemp5, use_MFpaltemp + Paltemp5, "2",	"No MT  PALTEMP5 --> MF PALTEMP5 alllowed in cycle 2");
      add_restriction (use_MTpaltemp + Paltemp6, use_MFpaltemp + Paltemp6, "2",	"No MT  PALTEMP6 --> MF PALTEMP6 alllowed in cycle 2");
      add_restriction (use_MTpaltemp + Paltemp7, use_MFpaltemp + Paltemp7, "2",	"No MT  PALTEMP7 --> MF PALTEMP7 alllowed in cycle 2");
      add_restriction (use_MTpaltemp + Paltemp8, use_MFpaltemp + Paltemp8, "2",	"No MT  PALTEMP8 --> MF PALTEMP8 alllowed in cycle 2");
      add_restriction (use_MTpaltemp + Paltemp9, use_MFpaltemp + Paltemp9, "2",	"No MT  PALTEMP9 --> MF PALTEMP9 allowed in cycle 2");
      add_restriction (use_MTpaltemp + Paltemp10, use_MFpaltemp + Paltemp10, "2",	"No MT  PALTEMP10 --> MF PALTEMP10 allowed in cycle 2");
      add_restriction (use_MTpaltemp + Paltemp11, use_MFpaltemp + Paltemp11, "2",	"No MT  PALTEMP11--> MF PALTEMP11 allowed in cycle 2");
      add_restriction (use_MTpaltemp + Paltemp12, use_MFpaltemp + Paltemp12, "2",	"No MT  PALTEMP12 --> MF PALTEMP12 allowed in cycle 2");
      add_restriction (use_MTpaltemp + Paltemp13, use_MFpaltemp + Paltemp13, "2",	"No MT  PALTEMP13 --> MF PALTEMP13 allowed in cycle 2");
      add_restriction (use_MTpaltemp + Paltemp14, use_MFpaltemp + Paltemp14, "2",	"No MT  PALTEMP14 --> MF PALTEMP14 allowed in cycle 2");
      add_restriction (use_MTpaltemp + Paltemp15, use_MFpaltemp + Paltemp15, "2",	"No MT  PALTEMP15 --> MF PALTEMP15 allowed in cycle 2");
      add_restriction (use_MTpaltemp + Paltemp16, use_MFpaltemp + Paltemp16, "2",	"No MT  PALTEMP16 --> MF PALTEMP16 allowed in cycle 2");
      add_restriction (use_MTpaltemp + Paltemp17, use_MFpaltemp + Paltemp17, "2",	"No MT  PALTEMP17 --> MF PALTEMP17 allowed in cycle 2");
      add_restriction (use_MTpaltemp + Paltemp18, use_MFpaltemp + Paltemp18, "2",	"No MT PALTEMP18 --> MF PALTEMP18 allowed in cycle 2");
      add_restriction (use_MTpaltemp + Paltemp19, use_MFpaltemp + Paltemp19, "2",	"No MT  PALTEMP19 --> MF PALTEMP19 allowed in cycle 2");
      add_restriction (use_MTpaltemp + Paltemp20, use_MFpaltemp + Paltemp20, "2",	"No MT  PALTEMP20 --> MF PALTEMP20 allowed in cycle 2");
      add_restriction (use_MTpaltemp + Paltemp21, use_MFpaltemp + Paltemp21, "2",	"No MT  PALTEMP21 --> MF PALTEMP21 allowed in cycle 2");
      add_restriction (use_MTpaltemp + Paltemp22, use_MFpaltemp + Paltemp22, "2",	"No MT  PALTEMP22 --> MF PALTEMP22 allowed in cycle 2");
      add_restriction (use_MTpaltemp + Paltemp23, use_MFpaltemp + Paltemp23, "2",	"No MT  PALTEMP23 --> MF PALTEMP23 allowed in cycle 2");
      add_restriction (use_MTpaltemp + Paltemp24, use_MFpaltemp + Paltemp24, "2",	"No MT  PALTEMP24 --> MF PALTEMP24 allowed in cycle 2");
      add_restriction (use_MTpaltemp + Paltemp25, use_MFpaltemp + Paltemp25, "2",	"No MT  PALTEMP25 --> MF PALTEMP25 allowed in cycle 2");
      add_restriction (use_MTpaltemp + Paltemp26, use_MFpaltemp + Paltemp26, "2",	"No MT  PALTEMP26 --> MF PALTEMP26 allowed in cycle 2");
      add_restriction (use_MTpaltemp + Paltemp27, use_MFpaltemp + Paltemp27, "2",	"No MT  PALTEMP27 --> MF PALTEMP27 allowed in cycle 2");
      add_restriction (use_MTpaltemp + Paltemp28, use_MFpaltemp + Paltemp28, "2",	"No MT  PALTEMP28 --> MF PALTEMP28 allowed in cycle 2");
      add_restriction (use_MTpaltemp + Paltemp29, use_MFpaltemp + Paltemp29, "2",	"No MT  PALTEMP29 --> MF PALTEMP29 allowed in cycle 2");
      add_restriction (use_MTpaltemp + Paltemp30, use_MFpaltemp + Paltemp30, "2",	"No MT  PALTEMP30 --> MF PALTEMP30 allowed in cycle 2");
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
 *		instr_array
 *	 IMPLICIT OUTPUTS:
 *	 AUTHOR/DATE:
 *		Greg Coladonato, 1992
 ********************************************************************************
 */
{
      struct implemented *imp;
      
      /* IADDLOG class  */
      
      imp = (struct implemented *) calloc(1, sizeof(struct implemented));

      imp->di_class = di_EOP;
      imp->lat_class = pc_IADDLOG; 
      
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
      
      /* integer compares */
      
      imp = (struct implemented *) calloc(1, sizeof(struct implemented));

      imp->di_class = di_EOP;
      imp->lat_class = pc_ICMP; 
      
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
      
      /* SHIFTCM class */
      
      imp = (struct implemented *) calloc(1, sizeof(struct implemented));

      imp->di_class = di_EOP;
      imp->lat_class = pc_SHIFTCM; 
      
      /* CMOVEx */
      
      instr_array[DO_CMOVEQ]->imp = imp;
      instr_array[DO_CMOVGE]->imp = imp;
      instr_array[DO_CMOVGT]->imp = imp;
      instr_array[DO_CMOVLBC]->imp = imp;
      instr_array[DO_CMOVLBS]->imp = imp;
      instr_array[DO_CMOVLE]->imp = imp;
      instr_array[DO_CMOVLT]->imp = imp;
      instr_array[DO_CMOVNE]->imp = imp;
      
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
      
      /* shift */
      
      instr_array[DO_ROT]->imp = imp;
      instr_array[DO_SLL]->imp = imp;
      instr_array[DO_SRA]->imp = imp;
      instr_array[DO_SRL]->imp = imp;
      
      /* integer longword muls */
      
      imp = (struct implemented *) calloc(1, sizeof(struct implemented));

      imp->di_class = di_EOP;
      imp->lat_class = pc_IMULL; 
      
      instr_array[DO_MULL]->imp = imp;
      instr_array[DO_MULLV]->imp = imp;
      
      /* integer muls */
      
      imp = (struct implemented *) calloc(1, sizeof(struct implemented));

      imp->di_class = di_EOP;
      imp->lat_class = pc_IMULQ; 
      
      instr_array[DO_MULQ]->imp = imp;
      instr_array[DO_MULQV]->imp = imp;
      instr_array[DO_UMULH]->imp = imp;
      
      /* Normal floating point (excluding FDIV) (2 operands & one dest) */
      
      imp = (struct implemented *)calloc(1, sizeof(struct implemented));

      imp->di_class = di_FOP;
      imp->lat_class = pc_FPOP; 
      
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
      
      /* FDIV (2 operands & one dest) */
      
      imp = (struct implemented *)calloc(1, sizeof(struct implemented));

      imp->di_class = di_FOP;
      imp->lat_class = pc_FDIV; 
      
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
      
      /* One operand floating point */
      
      imp = (struct implemented *)calloc(1, sizeof(struct implemented));

      imp->di_class = di_FOP;
      imp->lat_class = pc_FPOP; 
      
      /* float converts */
      
      instr_array[DO_CPYS]->imp = imp;
      instr_array[DO_CPYSE]->imp = imp;
      instr_array[DO_CPYSEE]->imp = imp;
      instr_array[DO_CPYSN]->imp = imp;
      
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
      
      
      /* LOAD ADDRESS - uses IALU */
      imp = (struct implemented *)calloc(1, sizeof(struct implemented));

      imp->di_class = di_EOP;
      imp->lat_class = pc_IADDLOG; 
      
      instr_array[DO_LDAH]->imp = imp;
      instr_array[DO_LDAL]->imp = imp;
      instr_array[DO_LDALH]->imp = imp;
      instr_array[DO_LDAQ]->imp = imp;
      
      /* HW_MFPR - to match EV3 assume same IBUFF as integer stores  */
      
      imp =  (struct implemented *)calloc(1, sizeof(struct implemented));

      imp->di_class = di_IST;
      imp->lat_class = pc_LD; 
      
      instr_array[DO_HW_MFPR]->imp = imp;
      instr_array[DO_RCC]->imp = imp;
      instr_array[DO_RC]->imp = imp;
      instr_array[DO_RS]->imp = imp;
      
      /* HW_MTPR - to match Ev3 assume same IBUFF as integer stores */
      
      imp =  (struct implemented *)calloc(1, sizeof(struct implemented));

      imp->di_class = di_IST;
      imp->lat_class = pc_ST; 
      
      instr_array[DO_HW_MTPR]->imp = imp;
      
      /* Create an instruction class record for integer memory loads, and
	 fill in the blanks */
      
      imp = (struct implemented *)calloc(1, sizeof(struct implemented));

      imp->di_class = di_LD;
      imp->lat_class = pc_LD; 
      
      instr_array[DO_HW_LD]->imp = imp;
      instr_array[DO_LDL]->imp = imp;
      instr_array[DO_LDLL]->imp = imp;
      instr_array[DO_LDQ]->imp = imp;
      instr_array[DO_LDQL]->imp = imp;
      instr_array[DO_LDQU]->imp = imp;
      
      /* Now do integer stores */
      
      imp = (struct implemented *)calloc(1, sizeof(struct implemented));

      imp->di_class = di_IST;
      imp->lat_class = pc_ST; 
      
      instr_array[DO_HW_ST]->imp = imp;
      instr_array[DO_STL]->imp = imp;
      instr_array[DO_STLC]->imp = imp;
      instr_array[DO_STQ]->imp = imp;
      instr_array[DO_STQC]->imp = imp;
      instr_array[DO_STQU]->imp = imp;
      
      /* Next do floating point loads */
      
      imp = (struct implemented *)calloc(1, sizeof(struct implemented));

      imp->di_class = di_LD;
      imp->lat_class = pc_LD; 
      
      instr_array[DO_LDS]->imp = imp;
      instr_array[DO_LDT]->imp = imp;
      instr_array[DO_LDD]->imp = imp;
      instr_array[DO_LDF]->imp = imp;
      
      /* Next, floating point stores */
      
      imp = (struct implemented *)calloc(1, sizeof(struct implemented));

      imp->di_class = di_FST;
      imp->lat_class = pc_ST; 
      
      instr_array[DO_STD]->imp = imp;
      instr_array[DO_STF]->imp = imp;
      instr_array[DO_STS]->imp = imp;
      instr_array[DO_STT]->imp = imp;
      
      /* Next, SWAP */
      
      imp =  (struct implemented *)calloc(1, sizeof(struct implemented));

      imp->di_class = di_IST;
      imp->lat_class = pc_ST; 
      free(imp); /* RRC:  Looks like this instruction is not used.  Free the space. */
      /* One Integer operand conditional branches */
      
      imp =  (struct implemented *)calloc(1, sizeof(struct implemented));

      imp->di_class = di_IBR;
      imp->lat_class = pc_IBR; 
      
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

      imp->di_class = di_FBR;
      imp->lat_class = pc_FBR; 
      
      instr_array[DO_BFEQ]->imp = imp;
      instr_array[DO_BFGE]->imp = imp;
      instr_array[DO_BFGT]->imp = imp;
      instr_array[DO_BFLE]->imp = imp;
      instr_array[DO_BFLT]->imp = imp;
      instr_array[DO_BFNE]->imp = imp;
      instr_array[DO_FLBC]->imp = imp;
      
      /* unconditional branch with dest reg - BSR, BR */
      
      imp =  (struct implemented *)calloc(1, sizeof(struct implemented));

      imp->di_class = di_JSR;
      imp->lat_class = pc_JSR; 
      
      instr_array[DO_BR]->imp = imp;
      instr_array[DO_BSR]->imp = imp;
      
      /* Memory format JSR */
      
      imp =  (struct implemented *)calloc(1, sizeof(struct implemented));

      imp->di_class = di_JSR;
      imp->lat_class = pc_JSR; 
      
      instr_array[DO_JSR]->imp = imp;
      
      
      /* CALL_PAL - to match EV3, assume we use the same IBUFF as integer 
	 stores  */
      
      imp =  (struct implemented *)calloc(1, sizeof(struct implemented));

      imp->di_class = di_IST;
      imp->lat_class = pc_JSR; 
      
      instr_array[DO_EPICODE]->imp = imp;
      
      /* HW_REI:  This looks just like a zero operand integer branch */
      
      imp =  (struct implemented *)calloc(1, sizeof(struct implemented));

      imp->di_class = di_JSR;
      imp->lat_class = pc_JSR; 
      
      instr_array[DO_HW_REI]->imp = imp;
      
      /*  DRAIN - to match EV3 assume same IBUFF as integer stores */
      
      imp =  (struct implemented *)calloc(1, sizeof(struct implemented));

      imp->di_class = di_IST;
      imp->lat_class = pc_LD; 
      
      instr_array[DO_DRAINT]->imp = imp;
      
      
      /* Next, NUDGE  */
      
      imp =  (struct implemented *)calloc(1, sizeof(struct implemented));

      imp->di_class = di_IST;
      imp->lat_class = pc_LD; 
      
      instr_array[DO_NUDGE]->imp = imp;
      
      /* Next, MB*/
      
      imp =  (struct implemented *)calloc(1, sizeof(struct implemented));

      imp->di_class = di_IST;
      imp->lat_class = pc_LD; 
      
      instr_array[DO_MB]->imp = imp;
      
      /* Next, FETCH, FETCH_M */
      
      imp =  (struct implemented *)calloc(1, sizeof(struct implemented));

      imp->di_class = di_IST;
      imp->lat_class = pc_LD; 
      
      instr_array[DO_FETCH]->imp = imp;
      instr_array[DO_FETCH_M]->imp = imp;
}



static void init_decode_tables (void)
/*
 ********************************************************************************
 *	PURPOSE:
 *	INPUT PARAMETERS:
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Greg Coladonato, July 92
 ********************************************************************************
 */
{
      int i;
      
      /* load all unusual names for regs here */
      
      strcpy(reg_name_table[SP_REG],"SP");
      
      strcpy(ipr_name_table[EVX$IPR_DTB_CTL],"DTB_CTL");
      strcpy(ipr_name_table[EVX$IPR_DTB_PTE],"DTB_PTE");
      strcpy(ipr_name_table[EVX$IPR_DPTE_TEMP],"DPTE_TEMP");
      strcpy(ipr_name_table[EVX$IPR_MM_CSR],"MM_CSR");
      strcpy(ipr_name_table[EVX$IPR_VA],"VA");
      
      /* tbzap, tbasm, tbis now have separate ibox/abox versions	*/
      
      strcpy(ipr_name_table[EVX$IPR_ITBZAP],"ITBZAP");
      strcpy(ipr_name_table[EVX$IPR_DTBZAP],"DTBZAP");
      strcpy(ipr_name_table[EVX$IPR_ITBASM],"ITBASM");
      strcpy(ipr_name_table[EVX$IPR_DTBASM],"DTBASM");
      strcpy(ipr_name_table[EVX$IPR_ITBIS],"ITBIS");
      strcpy(ipr_name_table[EVX$IPR_DTBIS],"DTBIS");
      
      
      strcpy(ipr_name_table[EVX$IPR_BIU_ADDR],"BIU_ADDR");
      strcpy(ipr_name_table[EVX$IPR_BIU_STAT],"BIU_STAT");
      strcpy(ipr_name_table[EVX$IPR_DC_ADDR],"DC_ADDR");
      strcpy(ipr_name_table[EVX$IPR_DC_STAT],"DC_STAT");
      strcpy(ipr_name_table[EVX$IPR_FILL_ADDR],"FILL_ADDR");
      strcpy(ipr_name_table[EVX$IPR_ABOX_CTL],"ABOX_CTL");
      strcpy(ipr_name_table[EVX$IPR_ALT_MODE],"ALT_MODE");
      strcpy(ipr_name_table[EVX$IPR_CC],"CC");
      strcpy(ipr_name_table[EVX$IPR_CC_CTL],"CC_CTL");
      strcpy(ipr_name_table[EVX$IPR_BIU_CTL],"BIU_CTL");
      strcpy(ipr_name_table[EVX$IPR_FILL_SYNDROME],"FILL_SYNDROME");
      strcpy(ipr_name_table[EVX$IPR_BC_TAG],"BC_TAG");
      strcpy(ipr_name_table[EVX$IPR_FLUSH_IC],"FLUSH_IC[ASM]");
      strcpy(ipr_name_table[EVX$IPR_FLUSH_IC_ASM],"FLUSH_IC_ASM");
      
      
      strcpy(ipr_name_table[EVX$IPR_TB_TAG],"TB_TAG");
      strcpy(ipr_name_table[EVX$IPR_ITB_PTE],"ITB_PTE");
      strcpy(ipr_name_table[EVX$IPR_ICCSR],"ICCSR");
      strcpy(ipr_name_table[EVX$IPR_IPTE_TEMP],"IPTE_TEMP");
      strcpy(ipr_name_table[EVX$IPR_EXC_ADDR],"EXC_ADDR");
      strcpy(ipr_name_table[EVX$IPR_SL_RCV],"SL_RCV");
      
      strcpy(ipr_name_table[EVX$IPR_PS],"PS");
      
      
      strcpy(ipr_name_table[EVX$IPR_EXC_SUM],"EXC_SUM");
      strcpy(ipr_name_table[EVX$IPR_PAL_BASE],"PAL_BASE");
      strcpy(ipr_name_table[EVX$IPR_HIRR],"HIRR");
      strcpy(ipr_name_table[EVX$IPR_SIRR],"SIRR");
      strcpy(ipr_name_table[EVX$IPR_ASTRR],"ASTRR");
      strcpy(ipr_name_table[EVX$IPR_HIER],"HIER");
      strcpy(ipr_name_table[EVX$IPR_SIER],"SIER");
      strcpy(ipr_name_table[EVX$IPR_ASTER],"ASTER");
      strcpy(ipr_name_table[EVX$IPR_SL_CLR],"SL_CLR");
      strcpy(ipr_name_table[EVX$IPR_SL_XMIT],"SL_XMIT");
      
      
      strcpy(ipr_name_table[EVX$IPR_ISSUE_CHK],"ISSUE_CHK");
      strcpy(ipr_name_table[EVX$IPR_SINGLE_ISSUE],"SINGLE_ISSUE");
      strcpy(ipr_name_table[EVX$IPR_DUAL_ISSUE],"DUAL_ISSUE");
      
      strcpy(ipr_name_table[EVX$IPR_PAL_R0],"PT0");
      strcpy(ipr_name_table[EVX$IPR_PAL_R1],"PT1");
      strcpy(ipr_name_table[EVX$IPR_PAL_R2],"PT2");
      strcpy(ipr_name_table[EVX$IPR_PAL_R3],"PT3");
      strcpy(ipr_name_table[EVX$IPR_PAL_R4],"PT4");
      strcpy(ipr_name_table[EVX$IPR_PAL_R5],"PT5");
      strcpy(ipr_name_table[EVX$IPR_PAL_R6],"PT6");
      strcpy(ipr_name_table[EVX$IPR_PAL_R7],"PT7");
      strcpy(ipr_name_table[EVX$IPR_PAL_R8],"PT8");
      strcpy(ipr_name_table[EVX$IPR_PAL_R9],"PT9");
      strcpy(ipr_name_table[EVX$IPR_PAL_R10],"PT10");
      strcpy(ipr_name_table[EVX$IPR_PAL_R11],"PT11");
      strcpy(ipr_name_table[EVX$IPR_PAL_R12],"PT12");
      strcpy(ipr_name_table[EVX$IPR_PAL_R13],"PT13");
      strcpy(ipr_name_table[EVX$IPR_PAL_R14],"PT14");
      strcpy(ipr_name_table[EVX$IPR_PAL_R15],"PT15");
      strcpy(ipr_name_table[EVX$IPR_PAL_R16],"PT16");
      strcpy(ipr_name_table[EVX$IPR_PAL_R17],"PT17");
      strcpy(ipr_name_table[EVX$IPR_PAL_R18],"PT18");
      strcpy(ipr_name_table[EVX$IPR_PAL_R19],"PT19");
      strcpy(ipr_name_table[EVX$IPR_PAL_R20],"PT20");
      strcpy(ipr_name_table[EVX$IPR_PAL_R21],"PT21");
      strcpy(ipr_name_table[EVX$IPR_PAL_R22],"PT22");
      strcpy(ipr_name_table[EVX$IPR_PAL_R23],"PT23");
      strcpy(ipr_name_table[EVX$IPR_PAL_R24],"PT24");
      strcpy(ipr_name_table[EVX$IPR_PAL_R25],"PT25");
      strcpy(ipr_name_table[EVX$IPR_PAL_R26],"PT26");
      strcpy(ipr_name_table[EVX$IPR_PAL_R27],"PT27");
      strcpy(ipr_name_table[EVX$IPR_PAL_R28],"PT28");
      strcpy(ipr_name_table[EVX$IPR_PAL_R29],"PT29");
      strcpy(ipr_name_table[EVX$IPR_PAL_R30],"PT30");
      strcpy(ipr_name_table[EVX$IPR_PAL_R31],"PT31");
      
      strcpy(ipr_name_table[EVX$IPR_FP_CTL],"FP_CTL");
      
      
      strcpy(pax_ipr_name_table[0],"ASN");
      strcpy(pax_ipr_name_table[1],"ASTEN");
      strcpy(pax_ipr_name_table[2],"ASTSR");
      strcpy(pax_ipr_name_table[3],"AT");
      strcpy(pax_ipr_name_table[4],"FEN");
      strcpy(pax_ipr_name_table[5],"IPIR");
      strcpy(pax_ipr_name_table[6],"IPL");
      strcpy(pax_ipr_name_table[7],"MCES");
      strcpy(pax_ipr_name_table[8],"PCBB");
      strcpy(pax_ipr_name_table[9],"PRBR");
      strcpy(pax_ipr_name_table[10],"PTBR");
      strcpy(pax_ipr_name_table[11],"SCBB");
      strcpy(pax_ipr_name_table[12],"SIRR");
      strcpy(pax_ipr_name_table[13],"SISR");
      strcpy(pax_ipr_name_table[14],"TBCHK");
      strcpy(pax_ipr_name_table[15],"TBIA");
      strcpy(pax_ipr_name_table[16],"TBIAP");
      strcpy(pax_ipr_name_table[17],"TBIS");
      strcpy(pax_ipr_name_table[18],"ESP");
      strcpy(pax_ipr_name_table[19],"SSP");
      strcpy(pax_ipr_name_table[20],"USP");
      strcpy(pax_ipr_name_table[21],"WHAMI");
      
      strcpy(ipr_name_table[EVX$IPR_INTR_FLAG],"INTR_FLAG");
      strcpy(ipr_name_table[EVX$IPR_LOCK_FLAG],"LOCK_FLAG");
      
      
      i = 0;
      strcpy(adu_ipr_name_table[i++],"FLAG");
      strcpy(adu_ipr_name_table[i++],"HALT");
      
      
      for (; i < 32+2; i++)
	sprintf(adu_ipr_name_table[i],"R%d",i-(2));
      
      for (; i < 32+2+32; i++)
	sprintf(adu_ipr_name_table[i],"FR%d",i-(2+32));
      
      strcpy(adu_ipr_name_table[i++],"MCHKFLG");
      
      for (; i < 32+2+32+1+32; i++)
	sprintf(adu_ipr_name_table[i],"PT%d",i-(2+32+1+32));
      
      
      strcpy(adu_ipr_name_table[i++],"EXC_ADDR");
      strcpy(adu_ipr_name_table[i++],"PAL_BASE");
      strcpy(adu_ipr_name_table[i++],"HIRR");
      strcpy(adu_ipr_name_table[i++],"HIER");
      strcpy(adu_ipr_name_table[i++],"MM_CSR");
      strcpy(adu_ipr_name_table[i++],"VA");
      strcpy(adu_ipr_name_table[i++],"BIU_ADDR");
      strcpy(adu_ipr_name_table[i++],"BIU_STAT");
      strcpy(adu_ipr_name_table[i++],"DC_ADDR");
      strcpy(adu_ipr_name_table[i++],"FILL_ADDR");
      strcpy(adu_ipr_name_table[i++],"DC_STAT");
      strcpy(adu_ipr_name_table[i++],"FILL_SYNDROME");
      strcpy(adu_ipr_name_table[i++],"BC_TAG");      
}      



/*
 * 	Code for checking 'dangerous' loads.
 *  
 *		In ev4, one of the PAL code restrictions is that a MF cannot 
 * 		execute exactly 2 cycles after a MT when the argument is a PAL TEMP
 *		both times. This restriction is unique in that a violation could occur
 *		as a result of two instructions moving farther away from each other
 *		in the pipe. As a result, this special test was written to see if there
 *		are any MT/MF combinations that are susceptible to being wedged apart 
 *		by a load that misses the cache.
 *
 *		As far as I know, this is peculiar to the EV4 implementation.
 */




int check_dangerous_loads (int length)
/*
 ********************************************************************************
 *	PURPOSE:
 *      	To look through the code for any loads which could possibly cause
 *      	problems down the line
 *	INPUT PARAMETERS:
 *	      The length of the code sequence
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED:
 *		check_position_one
 *		check_position_two
 *	AUTHOR/DATE:
 *	        Greg Coladonato, June 1992
 *	REVISION:
 *		SGM, Aug 1992	Fix bug.  The length of the code sequence to
 *		check for dangerous loads is 1 less than the actual length, since
 *		the code looks ahead by one issue slot. 
 ********************************************************************************
 */
{
      int 	cycle_count, i;
      struct reg regs[32];
      
      for (i=0; i<32; i++)  /* RRC: Changed to 32 so it initializes ALL 32 registers! */ 
	{
	      regs[i].dirty = FALSE;
	      regs[i].address = FALSE;
	}
      
      for (cycle_count = 0; cycle_count < length-1; cycle_count++) 
	{
	      if (runtime[cycle_count]->is1) 
		{
		      check_position_one (runtime[cycle_count], regs);
		      if (runtime[cycle_count]->is2) 
			{
			      check_position_two (runtime[cycle_count], regs);
			}
		}
	}
return(1);
}



int check_load_dirty (struct reg regs[], INSTR *i)
/*
 ********************************************************************************
 *	PURPOSE:
 * 		This code looks in our array to see if the operands were using were last sourced
 * 		from a load. If so, it spits out a message saying that the load operation should
 * 		be moved. 
 *	INPUT PARAMETERS:
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:	wr
 *	AUTHOR/DATE:
 *	        Greg Coladonato, July 92
 ********************************************************************************
 */
{
      struct architected *table;
      
      table = instr_array[i->index]->arch;

      if (table->op_ra && regs[i->bits.op_format.ra].dirty && (i->bits.op_format.ra != 31)) 
	{
	      wr (FLG$ERR, "Hey! Move that operation out from between the MT and the MF.\n");
	      wr (FLG$ERR, "It's in a very dangerous position where it is.\n");
	      wr (FLG$ERR, "Address: %X\n", i->address);
	      wr (FLG$ERR, "Load address: %X\n", regs[i->bits.op_format.ra].address);
	      pal_error_count++;
	}
      if (table->op_rb && regs[i->bits.op_format.rb].dirty && (i->bits.op_format.rb != 31) &&
	   !(table->chk_lit_mode && i->bits.opl_format.litflag)) 
	{
	      wr (FLG$ERR, "Hey! Move that operation out from between the MT and the MF.\n");
	      wr (FLG$ERR, "It's in a very dangerous position where it is.\n");
	      wr (FLG$ERR, "Address: %X\n", i->address);
	      wr (FLG$ERR, "Load address: %X\n", regs[i->bits.op_format.rb].address);
	      pal_error_count++;
	}
      if (table->op_rc && regs[i->bits.op_format.rc].dirty && (i->bits.op_format.rc != 31)) 
	{
	      wr (FLG$ERR, "Hey! Move that operation out from between the MT and the MF.\n");
	      wr (FLG$ERR, "It's in a very dangerous position where it is.\n");
	      wr (FLG$ERR, "Address: %X\n", i->address);
	      wr (FLG$ERR, "Load address: %X\n", regs[i->bits.op_format.rc].address);
	      pal_error_count++;
	}
return(1);
}



int check_position_two (CYCLE *c, struct reg *regs)
/*
 ********************************************************************************
 *	PURPOSE:
 *	INPUT PARAMETERS:
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY: check_load_dirty
 *	AUTHOR/DATE:
 *	        Greg Coladonato, July 92
 ********************************************************************************
 */
{
      struct architected *table;
	      
      switch (c->is2->index)                                                       
        {                                                                             
            case DO_LDL:        case DO_LDLL:   case DO_LDQ:                          
            case DO_LDQL:       case DO_LDQU:   case DO_LDD:                          
            case DO_LDS:        case DO_LDT:    case DO_LDF:                          
              {                                                                       
                    int         targ_reg;                                             
                                                                                      
                    targ_reg = c->is2->bits.op_format.ra;                          
                    if (targ_reg != 31)                                               
                      {                                                               
                            regs[targ_reg].dirty = TRUE;                              
                            regs[targ_reg].address = c->is2->address;              
                      }                                                               
              }                                                                       
              break;                                                                  
            case DO_HW_MTPR:                                                          
              {                                                                       
                    if (((c+1)->is2 &&                                             
                         (c+1)->is2->index == DO_HW_MFPR))                         
                      {                                                               
                            /*                                                        
                             * A MT issued in the second half of cycle 0, and a MF    
                             * issued in the second half of cycle 1. In order for     
                             * this to cause a problem, there has to be an instruction
                             * in the first half of cycle 1 that uses a register      
                             * as a source that was last prepared by a load.          
                             */                                                       
                                                                                      
                            if ((c+1)->is1)                                        
                              check_load_dirty (regs, (c+1)->is1);                 
                      }                                                               
              }                                                                       
              break;                                                                  
            default:                                                                  
              {                                                                       
                    /*                                                                
                     * Any instructions that use a register can clear that register's 
                     * flag in our array.                                             
                     */                                                               
                    table = instr_array[c->is2->index]->arch;                            
                                                                                      
                    if (table->op_ra)                                                 
                      regs[c->is2->bits.op_format.ra].dirty = FALSE;               
                    if (table->op_rb)                                                 
                      regs[c->is2->bits.op_format.rb].dirty = FALSE;               
                    if (table->op_rc)                                                 
                      regs[c->is2->bits.op_format.rc].dirty = FALSE;               
              }                                                                       
        }                                                                             
return(1);
}




int check_position_one (CYCLE *c, struct reg *regs)
/*
 ********************************************************************************
 *	PURPOSE:
 *	INPUT PARAMETERS:
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY: check_load_dirty
 *	AUTHOR/DATE:
 *	        Greg Coladonato, July 92
 ********************************************************************************
 */
{
      struct architected *table;
	      
      switch (c->is1->index)                                                                
	{                                                                             
	    case DO_LDL:        case DO_LDLL:   case DO_LDQ:                          
	    case DO_LDQL:       case DO_LDQU:   case DO_LDD:                          
	    case DO_LDS:        case DO_LDT:    case DO_LDF:                          
	      {                                                                       
		    int         targ_reg;                                             
		    
		    targ_reg = c->is1->bits.op_format.ra;                             
		    if (targ_reg != 31)                                               
		      {                                                               
			    regs[targ_reg].dirty = TRUE;                              
			    regs[targ_reg].address = c->is1->address;
		      }                                                               
	      }                                                                       
	      break;                                                                  
	    case DO_HW_MTPR:                                                          
	      {                                                                       
		    if ((c+1)->is1 && (c+1)->is1->index == DO_HW_MFPR)
		      {                                                               
			    /*                                                        
			     * A MT issued in the first half of cycle 0, and a MF     
			     * issued in the first half of cycle 1. In order for      
			     * this to cause a problem, there has to be an instruction
			     * in the second half of cycle 0 that uses a register as  
			     * a source that was last prepared by a load.             
			     */                                                       
			    
			    if (c->is2)                                               
			      check_load_dirty (regs, c->is2);                        
		      }                                                               
		    
		    if ((c+1)->is2 && (c+1)->is2->index == DO_HW_MFPR)                
		      {                                                               
			    /*                                                        
			     * A MT issued in the first half of cycle 0, and a MF     
			     * issued in the second half of cycle 1. In order for     
			     * this to cause a problem, there has to be an instruction
			     * in the second half of cycle 0 or in the first half of  
			     * cycle 1 that uses a register as a source that was last 
			     * prepared by a load.                                    
			     */                                                       
			    
			    if (c->is2)                                               
			      check_load_dirty (regs, c->is2);                        
			    
			    if ((c+1)->is1)                                           
			      check_load_dirty (regs, (c+1)->is1);                    
		      }                                                               
	      }                                                                       
	      break;                                                                  
	    default:                                                                  
	      {                                                                       
		    /*                                                                
		     * Any instructions that use a register can clear that register's 
		     * flag in our array.                                             
		     */                                                               
		    table = instr_array[c->is1->index]->arch;                               
		    
		    if (table->op_ra)                                                 
		      regs[c->is1->bits.op_format.ra].dirty = FALSE;                  
		    if (table->op_rb)                                                 
		      regs[c->is1->bits.op_format.rb].dirty = FALSE;                  
		    if (table->op_rc)                                                 
		      regs[c->is1->bits.op_format.rc].dirty = FALSE;                  
	      }                                                                       
	}                                                                             
return(1);
}


struct  {
	long	address;
	long	cycle;
	} last_cbr;


/*
 * 	Code for checking 'dangerous' branchs.
 *  
 *		In ev4, one of the PAL code restrictions is that a MF cannot 
 * 		execute exactly 2 cycles after a MT when the argument is a PAL TEMP
 *		both times. This restriction is unique in that a violation could occur
 *		as a result of two instructions moving farther away from each other
 *		in the pipe. As a result, this special test was written to see if there
 *		are any MT/MF combinations that are susceptible to being wedged apart 
 *		by a conditional branch mispredict that sucked a integer 
 *		multiply into the pipe. This is an EV4 anomaly that causes no 
 *		other side affects except for a bonus 1 cycle delay, in a very
 *		infrequent case.
 *
 */



int check_dangerous_cbranchs(int length)
/*
 ********************************************************************************
 *	PURPOSE:
 *      	To look through the code for any conditional branches which could possibly cause
 *      	problems down the line
 *	INPUT PARAMETERS:
 *	      The length of the code sequence
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED:
 *		check_cbr
 *	AUTHOR/DATE:
 *	        Steve Morris, June 1993
 *	REVISION:
 *		Cloned from the dangerous load code.
 ********************************************************************************
 */
{
      int 	cycle_count;

      
      last_cbr.cycle = -2;
      for (cycle_count = 0; cycle_count < length-1; cycle_count++) 
	{
	if (runtime[cycle_count]->is1) 
	    check_cbr(runtime[cycle_count]);
	}
return(1);
}


int check_cbr(CYCLE *c)
/*
 ********************************************************************************
 *	PURPOSE:
 *	INPUT PARAMETERS:
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Steve Morris, June 93
 ********************************************************************************
 */
    {
    struct architected *table;
    int errf=0;
	      
    if (c->is1)
	{
	if  (    
	    (c->is1->index == DO_BEQ) ||
	    (c->is1->index == DO_BNE) ||
	    (c->is1->index == DO_BGE) ||
	    (c->is1->index == DO_BGT) ||
	    (c->is1->index == DO_BLBS) ||
	    (c->is1->index == DO_BLBC) ||
	    (c->is1->index == DO_BLE) ||
	    (c->is1->index == DO_BLT) ||
	    (c->is1->index == DO_BFEQ) ||
	    (c->is1->index == DO_BFGE) ||
	    (c->is1->index == DO_BFGT) ||
	    (c->is1->index == DO_BFLE) ||
	    (c->is1->index == DO_BFLT) ||
	    (c->is1->index == DO_BFNE) ||
	    (c->is1->index == DO_FLBC)
	    )
	    {
	    last_cbr.address = c->is1->address;
	    last_cbr.cycle = (long) c;
	    }


	if (
	    (c->is1->index == DO_HW_MTPR) &&
	    ((c->is1->bits.hwipr_format.reg>>7) & 1) &&
	    (last_cbr.cycle == (long) c-1)
	    )
	    {
	    if ((c+1)->is1)
		if (((c+1)->is1->index == DO_HW_MFPR) &&
		    (((c+1)->is1->bits.hwipr_format.reg>>7) & 1))
		    errf |= 1;
	    if ((c+1)->is2)
		if (((c+1)->is2->index == DO_HW_MFPR) &&
		    (((c+1)->is2->bits.hwipr_format.reg>>7) & 1))
		    errf |= 1;
	    if ((errf) && ((c)->is2))
		{
		/*                                                        
		* A MT (pt) issued in the first half of cycle 0, and a MF (pt)
		* issued in either half of cycle 1. In order for     
		* this to cause a problem, there has to be an instruction
		* in the second half of cycle 1 that uses a register      
		* that a mispredicted branch caused to be dirty by an Imul.
		* or a floating conditional branch that a mispredicted branch
		* caused to be dirty by an Fdiv.
		*/                                                       

		table = instr_array[(c)->is2->index]->arch;

		if (
		    (c->is2->index == DO_BFEQ) ||
		    (c->is2->index == DO_BFGE) ||
		    (c->is2->index == DO_BFGT) ||
		    (c->is2->index == DO_BFLE) ||
		    (c->is2->index == DO_BFLT) ||
		    (c->is2->index == DO_BFNE) ||
		    (c->is2->index == DO_FLBC) ||

		    (
		    (table->op_ra && ((c)->is2->bits.op_format.ra != 31)) ||
		    (table->op_rb && ((c)->is2->bits.op_format.rb != 31) &&
		      !(table->chk_lit_mode && (c)->is2->bits.opl_format.litflag)) ||
		    (table->op_rc && ((c)->is2->bits.op_format.rc != 31)) 
		    ))
		    {
		    if (new_warning ((c)->is2->address, 0)) 
			{
			wr (FLG$ERR, "%s\n", pheader);
			wr (FLG$ERR, "Address: %X\n", (c)->is2->address);
			wr (FLG$ERR, "Hey! Move that operation out from between the MT and the MF.\n");
			wr (FLG$ERR, "It's in a very dangerous position where it is.\n");
			wr (FLG$ERR, "Address: %lx\n", (c)->is2->address);
			wr (FLG$ERR, "CBR address: %lx\n", last_cbr.address);
			pal_error_count++;
			}
		    }
		}
	    }
	}

    if (c->is2)
	{
	if  (    
	    (c->is2->index == DO_BEQ) ||
	    (c->is2->index == DO_BNE) ||
	    (c->is2->index == DO_BGE) ||
	    (c->is2->index == DO_BGT) ||
	    (c->is2->index == DO_BLBS) ||
	    (c->is2->index == DO_BLBC) ||
	    (c->is2->index == DO_BLE) ||
	    (c->is2->index == DO_BLT) ||
	    (c->is2->index == DO_BFEQ) ||
	    (c->is2->index == DO_BFGE) ||
	    (c->is2->index == DO_BFGT) ||
	    (c->is2->index == DO_BFLE) ||
	    (c->is2->index == DO_BFLT) ||
	    (c->is2->index == DO_BFNE) ||
	    (c->is2->index == DO_FLBC)
	    )
	    {
	    last_cbr.address = c->is2->address;
	    last_cbr.cycle = (long) c;
	    }
	}

    return(1);
    }                                                                             


