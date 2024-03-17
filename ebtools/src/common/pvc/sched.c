/* sched.c -- Schedule pal code */

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
 *	  3	sgm	SEP-92		Rewrite BSR stack handing code.
 *	  2	gpc	AUG-92		Modify for use with EV5
 *	  1     gpc     JUN-92          Revised to include computed gotos
 *	  0     gpc     JAN-92          First Created
 *	  
 * $Log: sched.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:23  paradis
 * Initial CVS checkin
 *
 * Revision 1.11  1995/11/14  21:52:29  cruz
 * Removed extra local variable.
 *
 * Revision 1.10  1995/11/14  21:15:35  cruz
 * Added prototypes and removed unused variables.
 *
 * Revision 1.9  1995/09/18  16:06:48  cruz
 * Made a million changes:
 * 1) Nodes are now malloc'ed and free.  This allows for flows
 * (or paths) to grow quite a bit more than before.
 * 2) Allows flows to be checked immediately and the space freed.
 *
 * Revision 1.8  1995/08/02  19:52:17  cruz
 *  V3.21:  Added code for creating flow chart.  To enable,
 *           compile with RRC_COMPILE_FLOW_CODE defined.
 *
 * Revision 1.7  1995/07/27  19:37:47  cruz
 * V3.21: Fixed problem that caused stack to be wrong.  See
 *        follow_conditionals for more detail.
 *
 * Revision 1.6  1995/07/27  17:17:07  cruz
 * V3.21:  Made fix so all possible branch paths are tested when
 *         we have the case of a branch inside a routine that it
 *         is called from the same place (as in a recursive
 *         routine).  This fixed involved moving the bsr_addr_hash
 * 	   field from the instruction record to the current tree node.
 *
 * Revision 1.5  1995/07/25  15:13:00  cruz
 * V3.21:  Added code to detect recursive BSR loops.
 * 	   Modified code to fix bug which prevented all flows
 *         to be checked when a branch was found inside a loop
 *         that was called more than one.  I still need to fix
 *         one more thing before it checks all of the possible
 *         paths.
 *
 * Revision 1.4  1995/05/23  21:25:42  cruz
 * Added RCS log directive.
 *
 */



#include "types.h"
#include "ispdef.h"
#include "list.h"
#include "sched.h"
#include "memory.h"

extern char pheader[];
extern CYCLE *runtime[];
extern List *go_no_further, *bsr_stack;
extern INSTR *exe;
extern int interrupt, delay, flags, perm;
extern Chip *chip;
extern Entry  *pal_entry;		/* pal_entry->next , pal_entry->prev, pal_entry->killed,pal_entry->offset and pal_entry->name */

extern int wr (int, char *, ...);
extern int not_ignored (int, int);

#ifdef RRC_COMPILE_FLOW_CODE
/* RRC:  Added this code for flow display */
int center (char * str);
char *lineof (int n, char *str);
void print_connect_line();
void print_current_instruction(NODE *current, char *box_str);
void print_message_in_box(char *message, char *box_str);
int print_current_instruction_with_arrow(NODE *current, char *box_str, int node);
int RRC_node = 1;
#endif /* RRC_COMPILE_FLOW_CODE */
void destroy_tree(NODE *root) ;
extern void check_current_path (int permutation);  /* RRC: Created this routine so it can be called WHILE a tree is being created */
static void RestoreBsrStack(NODE *current);

static long current_addr_hash;

CLASS_INFO *instr_array[MAX_INDEX];		/* V3.24 */



void get_next_branch (NODE *tree, INSTR *next_flow[])
/*
 ********************************************************************************
 *	PURPOSE:
 *	 	To extract from the tree one the next instruction flow that will
 *	 	be scheduled. After extracting that branch, we must remove it from the tree.
 *	 
 *	 	The algorithm is the following:
 *	 
 *	 	while there is a fall through path, take it
 *	 	if there is a branch path, take it
 *	 	go back to beginning
 *	 
 *	 	if we're at the bottom of the tree, 
 *	 		put a list terminator
 *	 		remove that branch from the tree
 *	 
 *	INPUT PARAMETERS:
 *	 	The tree and an array of pointers to ints.
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:
 *		get_next_branch
 *	AUTHOR/DATE:
 *	 	Greg Coladonato, November 1991
 ********************************************************************************
 */
{
      int count=0;
      
      /* follow the inline cases first. */	 
      while (tree->inline) 
	{
	      next_flow[count++] = tree->instruction;
	      tree = tree->inline;
	}
      next_flow[count++] = tree->instruction;
      
      /* When those run out, take a conditional branch, if there is one. */	 
      if (tree->branch) 
	{
	      get_next_branch (tree->branch, &next_flow[count]);
	}
      
      /*	 
       *  When that option is exhausted, check for a list of computed
       *  goto targets.
       */	 
      if (tree->jump_dests && length_L(tree->jump_dests)) 
	{
	      NODE *goto_dest = head_L(tree->jump_dests);
	      
	      get_next_branch (goto_dest, &next_flow[count]);
	}
      
      /*	 
       *  When we get to here, if we're at the bottom of the tree, 
       *  prune the branch that we're on from the tree.
       *  We might not be at the bottom of the tree -- if we took
       *  a conditional branch then when we eventually finish, the
       *  instantiation that generated the branch will be in the
       *  middle of the final flow.
       */	 
      if (!tree->inline && !tree->branch && !tree->jump_dests) 
	{
	      /*	 
	       *  Put a null in the end to signal that we're done.
	       */	 
	      next_flow[count] = NULL;
              if (flags & FLG$CHECKNOW) return;  /* RRC: Don't prune the tree because it's not 'fully grown' yet */
	      /*	 
	       *  While there is a parent node and it has only one child
	       *  keep walking up the tree
	       */	 
	      while (tree->previous && x3or(tree->previous->inline,  
					    tree->previous->branch,
					    (tree->previous->jump_dests &&
					     length_L(tree->previous->jump_dests) == 1)))
		      tree = tree->previous;

	      /* If we're not at the root, remove one of the branches of the tree */	 
	      if (tree->previous) 
		{
		      tree = tree->previous;
		      if (tree->inline) tree->inline = NULL;
		      else if (tree->branch) tree->branch = NULL;
		      else 
			{
			      /*
			       * We have to pluck one of the twigs out of the numerous
			       * possible branch destinations -- the one that we just 
			       * traced down (the first one)
			       */
			      NODE *goto_dest = (NODE *) head_L(tree->jump_dests);
			      remove_nofree_L(tree->jump_dests, NODE, goto_dest);
			      if (length_L(tree->jump_dests) == 0)
				destroy_L(tree->jump_dests, NODE);
			}
		}
	}
}

long hash_bsr_stack() 
/*
 ********************************************************************************
 *       PURPOSE:
 * 		Compute Current BSR hash.  This should be unique for the current set of BSR calls...  Assumption:  The
 * 		only time we will see a repeat BSR hash is if we have a recursive routine.  In that case we bag out of
 * 		the flow.
 *	 INPUT PARAMETERS:
 *	 OUTPUT PARAMETERS:
 *		Hash value based on addresses on BSR stack
 *	 IMPLICIT INPUTS:
 *	 FUNCTIONS CALLED DIRECTLY:
 *	 IMPLICIT OUTPUTS:
 *	 AUTHOR/DATE:
 *	 	Greg Coladonato, November 1991
 ********************************************************************************
 */
{
      int c=1;
      int hash= -1;

      if (bsr_stack) {
	  for_L(bsr_stack, Bsr_Stack, i) {
	    hash=hash+c*i->pc_plus_four;
	    c++;
	  }
	  end_L(bsr_stack, Bsr_Stack, i);
      }
      return(hash);
}


int InBsrStack (int ReturnPC)
/*                  
 ********************************************************************************
 *	 PURPOSE:
 *	 	To detect if current call context is in the BSR stack.
 *	 INPUT PARAMETERS:
 *	 	ReturnPC: The return PC of the current call.
 *	 OUTPUT PARAMETERS:
 *		The number of times the Return PC was found in the stack.
 *	 AUTHOR/DATE:
 *	 	Rogelio Cruz, 7/24/95
 *              I found the need to add this routine to detect recursive BSR loops.
 ********************************************************************************
 */
{
      int found = 0;

      if (bsr_stack) {
	  for_L(bsr_stack, Bsr_Stack, i) {
	    if (i->pc_plus_four == ReturnPC)
		found++;
	  }
	  end_L(bsr_stack, Bsr_Stack, i);
      }
      return(found);

}

void add_to_bsr_history (NODE *current)
/*                  
 ********************************************************************************
 *	 PURPOSE:
 *	 	To mark the current instruction as seen in the current call context.
 *	 INPUT PARAMETERS:
 *	 	current: a pointer to the node that this invocation of the code works on
 *	 OUTPUT PARAMETERS:
 *	 IMPLICIT INPUTS:
 *	 FUNCTIONS CALLED DIRECTLY:
 *	 IMPLICIT OUTPUTS:
 *	 AUTHOR/DATE:
 *	 	Rogelio Cruz, 7/21/95
 *              This piece of code used to be at the beginning of follow_code
 *   	        I moved it to a separate function that can be called from
 *		follow_conditionals and follow_jump AFTER checkforrevisit has
 *		been called.   The reason I did this is because if you add
 *		the current call context to the instruction's BSR history list
 *		and then call checkforrevisit, it would report that the current
 *		instruction has already been executed in the same call context.
 *		This is only a problem where branches or jumps are inside a
 *		routine that is called more than one from the same flow.  For
 *		example, supposed you had the following code:
 *		
 *		BSR R0, RT1
 *		BSR R0, RT1
 *		HW_REI
 *
 *		RT1:
 *			BEQ R1, SKIP
 *			MT
 *			MF
 *		SKIP:
 *			RET R31, (R0)
 *
 *		Before the change, PVC would only check two of the four possible
 *		flows ( 1- fall-thru, taken, 2- taken, fall-thru).  As a result,
 *		a violation would not be detected if you had the fall-thru, fall-thru
 *		case (a read after a write violation).
 ********************************************************************************
 */

/*
* See if the current BSR hash has already been added to the bsr history list for this instruction.  If yes, we may
* have recursion or we may just be coming here again via a different path.  In any case, don't need to add
* another element to the bsr history list.
*/
{
  int recursion_flag=FALSE;
  Bsr_History *i;
  
  for_L(current->instruction->bsr_history, Bsr_History, i)
    {
      if (i->bsr_addr_hash==current_addr_hash)
        {
          /*	     	wr (FLG$TRACE, "BSR hash found in hash list already.  Address %x\n",current->instruction->address); */
          recursion_flag=TRUE;
        }
    }
  end_L(current->instruction->bsr_history, Bsr_History, i);

  if (!recursion_flag) {
    i = (Bsr_History *) malloc (sizeof (Bsr_History));
    i->bsr_addr_hash = current_addr_hash;
    add_L(current->instruction->bsr_history, Bsr_History, i)
  }
}



void follow_code (NODE *current)
/*                  
 ********************************************************************************
 *	 PURPOSE:
 *	 	To create a tree of instructions that represents all the possible paths
 *	 	of execution of the code. Basically, we need to make sure that every 
 *	 	possible sequence of instructions gets tested in order to be thorough.
 *	 INPUT PARAMETERS:
 *	 	current: a pointer to the node that this invocation of the code works on
 *	 OUTPUT PARAMETERS:
 *	 IMPLICIT INPUTS:
 *	 FUNCTIONS CALLED DIRECTLY:
 *		follow_*
 *		mem_alloc_node
 *		not_ignored
 *		wr
 *	 IMPLICIT OUTPUTS:
 *	 AUTHOR/DATE:
 *	 	Greg Coladonato, November 1991
 ********************************************************************************
 */
{
      int idx, next_inst_inline = TRUE, block_length=0;
      int max_block_length;				/* V3.24 */
      int found_dest;					/* V3.25 */
      int expd;				/* t3.28 */
      int recv;				/* t3.28 */
      while (next_inst_inline && !interrupt) 
	{

	      wr (FLG$TRACE, "%X\t%s\n", current->instruction->address, current->instruction->decoded);
	      current->instruction->reached++;

	      current_addr_hash = hash_bsr_stack();
              current->bsr_addr_hash = current_addr_hash;
	      idx = current->instruction->index;
	      

	      /*	 
	  	*  If we get to a HW_REI or a HALT, we don't need to go any further
		*  for ev6 hw_rei = hw_ret. If hw_ret with hint=00= hw_jmp 
		*			      or hw_ret with hint=01 hw_jsr
		*  then not end of flow so call follow_jump.
	       */	 
	      if (idx==DO_HW_REI || (current->instruction->PVC_Control_Flag & PVC_CTRL_GO_NO_FURTHER) )
	         {
#ifdef RRC_COMPILE_FLOW_CODE
                      { /* RRC:  Added this code for flow display */
                        print_connect_line();
                        print_current_instruction(current, "%");
                      }
#endif /* RRC_COMPILE_FLOW_CODE */
	          found_dest=FALSE;			/* V3.25 */

		  if (strcmp (chip->name, "21264") == 0)		/* if EV6 */
		    {
		       if ((current->instruction->bits.ev6_hwret_format.hint == 0) ||			/* if hw_jmp */
		          (current->instruction->bits.ev6_hwret_format.hint == 1))			/* if hw_jsr */
			 {
                              if (current->instruction->PVC_Control_Flag & PVC_CTRL_GO_NO_FURTHER)
				{			
		      		next_inst_inline = FALSE;
                                if (current->instruction->PVC_Control_Flag & PVC_CTRL_GO_NO_FURTHER)
                                    wr (FLG$TRACE, "Current permutation stopped on user request.\n", perm);
			        wr (FLG$TRACE, "Permutation %d completed normally.\n\n", perm);
                                if (flags & FLG$CHECKNOW) check_current_path(perm);
			        perm++;
				}
			      else
				{			
		      		    next_inst_inline = FALSE;
		      		    if (current->instruction->jsr_dest_list) {
			              wr (FLG$TRACE, "HW_JMP or HW_JSR  followed as if it were a Computed goto.\n\n");
			 	      found_dest = follow_jump (current);
				    }
				    if (found_dest==FALSE) {
				         if (not_ignored(current->instruction->address, 1007)) {
				           wr (FLG$TRACE | FLG$WARN, "(PVC #%d) Permutation %d completed	*ABNORMALLY**.\n", 1007, perm);
				           wr (FLG$TRACE | FLG$WARN, "	PVC can't do computed gotos without your assistance.\n");
			                   wr (FLG$TRACE | FLG$WARN, "      You must put PVC computed goto labels in your palcode.\n");
			                   wr (FLG$TRACE | FLG$WARN, "      Suggestion: Here is a code example to follow:\n");
			                   wr (FLG$TRACE | FLG$WARN, "		pvc$label1$2100:                        ! source label.\n");
			                   wr (FLG$TRACE | FLG$WARN, "				hw_jmp_stall (r3)       ! computed goto.\n");
			                   wr (FLG$TRACE | FLG$WARN, "		pvc$label1$2100.1:                      ! destination label.\n");
				           wr (FLG$WARN, "Address of computed goto: %X\n\n", current->instruction->address);
				         }
			                 wr (FLG$TRACE, "Permutation %d completed because I couldn't follow that goto.\n\n", perm);
					 if (flags & FLG$CHECKNOW) check_current_path(perm);
			      		 perm++;
				     } /* if (found_dest==FALSE... */
				}
			 }
		       else		/* EV6 but hw_ret or hw_coroutine*/
			 {
		      		next_inst_inline = FALSE;
		       /* fls- hw_coroutine treated as hw_jsr only -hopefully it won't be used in palcode */
		          if (current->instruction->jsr_dest_list) {		/* hw_coroutine */
			      found_dest = follow_jump (current);

				if (found_dest==FALSE) {
				         if (not_ignored(current->instruction->address, 1007)) {
				           wr (FLG$TRACE | FLG$WARN, "(PVC #%d) Permutation %d completed	*ABNORMALLY**.\n", 1007, perm);
				           wr (FLG$TRACE | FLG$WARN, "	PVC can't do computed gotos without your assistance.\n");
			                   wr (FLG$TRACE | FLG$WARN, "      You must put PVC computed goto labels in your palcode.\n");
			                   wr (FLG$TRACE | FLG$WARN, "      Suggestion: Here is a code example to follow:\n");
			                   wr (FLG$TRACE | FLG$WARN, "		pvc$label1$2100:                        ! source label.\n");
			                   wr (FLG$TRACE | FLG$WARN, "				hw_jmp_stall (r3)       ! computed goto.\n");
			                   wr (FLG$TRACE | FLG$WARN, "		pvc$label1$2100.1:                      ! destination label.\n");
				           wr (FLG$WARN, "Address of computed goto: %X\n\n", current->instruction->address);
				         }
			                 wr (FLG$TRACE, "Permutation %d completed because I couldn't follow that goto.\n\n", perm);
					 if (flags & FLG$CHECKNOW) check_current_path(perm);
			      		 perm++;
				} /* if (found_dest==FALSE... */
		          } /* if (current->instruction->jsr_dest_list... */
#ifndef alt_bsr
		          else if (current->instruction->use_bsr_stack) {
#else
			  else if (  ((current->instruction->bits.mem_format.disp >> 14) & 0x3) == 02) {
#endif
			      Bsr_Stack *top;
			      
#ifdef RRC_COMPILE_FLOW_CODE
                              { /* RRC:  Added this code for flow display */
                                print_connect_line();
                                print_current_instruction(current, "+");
                              }
#endif /* RRC_COMPILE_FLOW_CODE */

			      top = tail_L(bsr_stack);
			      if (length_L(bsr_stack) && top->bsr_family == current->instruction->use_bsr_stack) {
				      /* 
				       * the address on the top of the stack is the right one. take it.
				       */
				      wr (FLG$TRACE, "Return followed via bsr return stack.\n");
				      current->inline = Mem_Alloc_NODE(current, NULL, NULL, NULL, 
								       &exe[top->pc_plus_four / 4]);
				      remove_L(bsr_stack, Bsr_Stack, top);
				      wr (FLG$TRACE, "Removing this entry from bsr stack.\n");
	    			      

				      follow_code (current->inline);
			      }	else {
				      /*
				       * Somehow, our stack got messed up; Punt.
				       */
		      		      /* T3.28 print Error Checking the xxxxx routine, entry point nnnn: */
		      		      wr (FLG$TRACE | FLG$WARN, 
					  "\nError Checking the %s routine, entry point %X:\n", pal_entry->name, pal_entry->offset);

				      if (length_L(bsr_stack) == 0) {
					wr (FLG$TRACE | FLG$WARN , 
						"PVC Warning - Permutation %d terminated abnormally.\n", perm);
					wr (FLG$TRACE | FLG$WARN , 
						"	       RET with no PVC Address found on BSR stack.\n", perm);
					wr (FLG$TRACE | FLG$WARN , 
						"	       The .exe file may be corrupted.\n", perm);
				      } else {
					      wr (FLG$TRACE | FLG$WARN , 
						"PVC Warning - Permutation %d terminated abnormally.\n", perm);
					      wr (FLG$TRACE | FLG$WARN , 
						"	       Wrong PVC Address found on BSR stack.\n", perm);
					      expd = top->bsr_family + 4000 - 1 ;			/* t3.28 see body.c use_bsr_stack */
					      recv = current->instruction->use_bsr_stack + 4000 - 1 ; /* t3.28 see body.c use_bsr_stack */
					      wr (FLG$TRACE | FLG$WARN, "   expected pvc label at ret = %d \n", expd);
					      wr (FLG$TRACE | FLG$WARN, "   received pvc label at ret = %d \n", recv);
					      wr (FLG$TRACE | FLG$WARN , "	       The .exe file may be corrupted.\n", perm);
					    }
		      		      wr (FLG$TRACE | FLG$WARN, "Suggestions:\n");
		      		      wr (FLG$TRACE | FLG$WARN, "         1. Ensure your .map file is up to date.\n");
		      		      wr (FLG$TRACE | FLG$WARN, "         2. Generate a PVC trace file:\n");
		      		      wr (FLG$TRACE | FLG$WARN, "                pvc> Set flag trace\n");
		      		      wr (FLG$TRACE | FLG$WARN, "                pvc> Set log trace.log\n");
		      		      wr (FLG$TRACE | FLG$WARN, "                pvc> do %X\n" ,pal_entry->offset);
		      		      wr (FLG$TRACE | FLG$WARN, "         3. Check your palcode to ensure all BSR and RET \n");
		      		      wr (FLG$TRACE | FLG$WARN, "            have matching pvc labels such as:\n");
		      		      wr (FLG$TRACE | FLG$WARN, "\n");
    		      		      wr (FLG$TRACE | FLG$WARN, "             pvc$xxx$4000:     bsr r7,subr1  \n");
		      		      wr (FLG$TRACE | FLG$WARN, "                                nop  \n");
		      		      wr (FLG$TRACE | FLG$WARN, "             subr1:\n");
		      		      wr (FLG$TRACE | FLG$WARN, "             pvc$xxy$4000.1:  ret (r7) \n");


#ifdef RRC_COMPILE_FLOW_CODE  
                                      { /* RRC:  Added this code for flow display */
                                        char temp[PAGE_WIDTH+20];
                                        print_connect_line();
                                        sprintf (temp, "BSR stack is messed up.  Permutation %d", perm);
                                        print_message_in_box(temp, ":");
                                        wr(FLG$FLOW, "\n\n\n");
                                      }
#endif /* RRC_COMPILE_FLOW_CODE */
                                      if (flags & FLG$CHECKNOW) check_current_path(perm);
				      perm++;
			      }
		          } else {
                                if (current->instruction->PVC_Control_Flag & PVC_CTRL_GO_NO_FURTHER)
                                    wr (FLG$TRACE, "Current permutation stopped on user request.\n", perm);
			        wr (FLG$TRACE, "Permutation %d completed normally.\n\n", perm);
                                if (flags & FLG$CHECKNOW) check_current_path(perm);
			        perm++;
		      	        }

			 }
		    }
		  else 		/* NOT EV6 */
		    {
		      next_inst_inline = FALSE;
		      if (current->instruction->jsr_dest_list && !(current->instruction->PVC_Control_Flag & PVC_CTRL_GO_NO_FURTHER))
			{
			      wr (FLG$TRACE, "HW_REI followed as if it were a Computed goto..\n\n");
			      follow_jump (current);
			}			  
		      else 
			{
                              if (current->instruction->PVC_Control_Flag & PVC_CTRL_GO_NO_FURTHER)
                                wr (FLG$TRACE, "Current permutation stopped on user request.\n", perm);
			      wr (FLG$TRACE, "Permutation %d completed normally.\n\n", perm);
#ifdef RRC_COMPILE_FLOW_CODE
                              { /* RRC:  Added this code for flow display */
                                char temp[PAGE_WIDTH+20];
                                center("|");
                                wr(FLG$FLOW, "\n");
                                sprintf (temp, "Permutation %d", perm);
                                print_message_in_box(temp, ":");
                                wr(FLG$FLOW, "\n\n\n");
                              }
#endif /* RRC_COMPILE_FLOW_CODE */
/* RRC:  Now that we are done with this path, let's check it for violations*/
                               if (flags & FLG$CHECKNOW) check_current_path(perm);
			      perm++;
			}
		    }/* end if ev6 */

		}
	      else if (idx == DO_UNDEFINED) 
		{
		      if (not_ignored(current->instruction->address, 1001)) 
			{
			      wr (FLG$TRACE | FLG$WARN, "(PVC #%d) Permutation %d completed **ABNORMALLY**.\n", 1001, perm);
		              wr (FLG$TRACE | FLG$WARN, "            due to an Ilegal Opcode \n");
			      wr (FLG$TRACE | FLG$WARN, "Address of ILLOPC: %X\n\n", current->instruction->address);
			}
		      wr (FLG$TRACE, "Permutation %d completed abnormally via ILLOPC.\n\n", perm);
#ifdef RRC_COMPILE_FLOW_CODE  
                      { /* RRC:  Added this code for flow display */
                        char temp[PAGE_WIDTH+20];
                        print_connect_line();
                        sprintf (temp, "Illegal instruction @ 0x%X. Permutation %d", current->instruction->address, perm);
                        print_message_in_box(temp, ":");
                        wr(FLG$FLOW, "\n\n\n");
                      }
#endif /* RRC_COMPILE_FLOW_CODE */
                      if (flags & FLG$CHECKNOW) check_current_path(perm);
		      perm++;
		      next_inst_inline = FALSE;
		}
	      else if (idx == DO_EPICODE && current->instruction->bits.instr != 0) 
		{
		      if (not_ignored(current->instruction->address, 1002)) 
			{
			      wr (FLG$WARN, "%s\n", pheader);
			      wr (FLG$WARN, "(PVC #%d) %s %s", 1002,
				  current->instruction->decoded,"CALL_PAL encountered. It will be ignored\n");
			      wr (FLG$WARN, "so make sure\nit is not violating any PAL restrictions.\n");
			      wr (FLG$WARN, "Address: %4X\n\n", current->instruction->address);
			}
		}
	      else if (idx == DO_EPICODE && current->instruction->bits.instr == 0) 
		{
		      if (not_ignored(current->instruction->address, 1003)) 
			{
			      wr (FLG$TRACE | FLG$WARN, "%s\n", pheader);
			      wr (FLG$TRACE | FLG$WARN, "(PVC #%d) Permutation %d completed **ABNORMALLY**\n",
				  1003, perm);
		      	      wr (FLG$TRACE | FLG$WARN, "            due to a HALT instruction.\n");
			      wr (FLG$TRACE | FLG$WARN, "Address of HALT: %X\n\n", current->instruction->address);
			}
		      wr (FLG$TRACE, "Permutation %d completed abnormally via HALT.\n\n", perm);
#ifdef RRC_COMPILE_FLOW_CODE  
                      { /* RRC:  Added this code for flow display */
                        char temp[PAGE_WIDTH+20];
                        print_connect_line();
                        sprintf (temp, "HALT instruction @ 0x%X. Permutation %d", current->instruction->address, perm);
                        print_message_in_box(temp, ":");
                        wr(FLG$FLOW, "\n\n\n");
                      }
#endif /* RRC_COMPILE_FLOW_CODE */
                      if (flags & FLG$CHECKNOW) check_current_path(perm);
		      perm++;
		      next_inst_inline = FALSE;
		}
	      else if ((idx == DO_HW_LD ) && (current->instruction->bits.op_format.ra == 4) &&
		                             (current->instruction->bits.hwmem_format.physical == 0)) 
		   {
		    if (strcmp (chip->name, "21264") != 0) {
		        if (not_ignored(current->instruction->address, 1005)) 
			  {
			      wr (FLG$WARN, "%s\n", pheader);
			      wr (FLG$WARN, "(PVC #%d) %s %s", 1005,
				  current->instruction->decoded,"Virtual HW_LD R4, (x) may be illegal\n");
			      wr (FLG$WARN, "Address of LD %4X\n\n", current->instruction->address);
			  }
		     }
		}
	      else if (idx == DO_JSR) {			/* can be  jmp, ret, jsr, or ret */
		      next_inst_inline = FALSE;
		      if (current->instruction->jsr_dest_list) 
			 follow_jump (current);
#ifndef alt_bsr
		      else if (current->instruction->use_bsr_stack) {
#else
			else if (  ((current->instruction->bits.mem_format.disp >> 14) & 0x3) == 02) {
#endif
			      Bsr_Stack *top;
			      
#ifdef RRC_COMPILE_FLOW_CODE
                              { /* RRC:  Added this code for flow display */
                                print_connect_line();
                                print_current_instruction(current, "+");
                              }
#endif /* RRC_COMPILE_FLOW_CODE */

			      top = tail_L(bsr_stack);
			      if (length_L(bsr_stack) && top->bsr_family == current->instruction->use_bsr_stack) {
				      /* 
				       * the address on the top of the stack is the right one. take it.
				       */
				      wr (FLG$TRACE, "Return followed via bsr return stack.\n");
				      current->inline = Mem_Alloc_NODE(current, NULL, NULL, NULL, 
								       &exe[top->pc_plus_four / 4]);
				      remove_L(bsr_stack, Bsr_Stack, top);
				      wr (FLG$TRACE, "Removing this entry from bsr stack.\n");
	    			      

				      follow_code (current->inline);
			      }	else {
				      /*
				       * Somehow, our stack got screwed. Punt.
				       */
		      		      /* T3.28 print Error Checking the xxxxx routine, entry point nnnn: */
		      		      wr (FLG$TRACE | FLG$WARN, 
					  "\nError Checking the %s routine, entry point %X:\n", pal_entry->name, pal_entry->offset);

				      if (length_L(bsr_stack) == 0)
					wr (FLG$TRACE | FLG$WARN, "Permutation %d terminated, RET with empty BSR stack\n", perm);
				      else {
					wr (FLG$TRACE | FLG$WARN, 
						"Permutation %d terminated, Wrong Address on BSR stack\n", perm);
					expd = top->bsr_family + 4000 - 1 ;			/* t3.28 see body.c use_bsr_stack */
					recv = current->instruction->use_bsr_stack + 4000 - 1 ; /* t3.28 see body.c use_bsr_stack */
					wr (FLG$TRACE | FLG$WARN, "   expected pvc label at ret = %d \n", expd);
					wr (FLG$TRACE | FLG$WARN, "   received pvc label at ret = %d \n", recv);
					   }
		      		      wr (FLG$TRACE | FLG$WARN, "Suggestions:\n");
		      		      wr (FLG$TRACE | FLG$WARN, "         1. Ensure your .map file is up to date.\n");
		      		      wr (FLG$TRACE | FLG$WARN, "         2. Generate a PVC trace file:\n");
		      		      wr (FLG$TRACE | FLG$WARN, "                pvc> Set flag trace\n");
		      		      wr (FLG$TRACE | FLG$WARN, "                pvc> Set log trace.log\n");
		      		      wr (FLG$TRACE | FLG$WARN, "                pvc> do %X\n" ,pal_entry->offset);
		      		      wr (FLG$TRACE | FLG$WARN, "         3. Check your palcode to ensure all BSR and RET \n");
		      		      wr (FLG$TRACE | FLG$WARN, "            have matching pvc labels such as:\n");
		      		      wr (FLG$TRACE | FLG$WARN, "\n");
    		      		      wr (FLG$TRACE | FLG$WARN, "             pvc$xxx$4000:     bsr r7,subr1  \n");
		      		      wr (FLG$TRACE | FLG$WARN, "                                nop  \n");
		      		      wr (FLG$TRACE | FLG$WARN, "             subr1:\n");
		      		      wr (FLG$TRACE | FLG$WARN, "             pvc$xxy$4000.1:  ret (r7) \n");


#ifdef RRC_COMPILE_FLOW_CODE  
                                      { /* RRC:  Added this code for flow display */
                                        char temp[PAGE_WIDTH+20];
                                        print_connect_line();
                                        sprintf (temp, "BSR stack is messed up.  Permutation %d", perm);
                                        print_message_in_box(temp, ":");
                                        wr(FLG$FLOW, "\n\n\n");
                                      }
#endif /* RRC_COMPILE_FLOW_CODE */
                                      if (flags & FLG$CHECKNOW) check_current_path(perm);
				      perm++;
			      }
		      } else {
			      if (not_ignored(current->instruction->address, 1007)) {
				       wr (FLG$TRACE | FLG$WARN, "(PVC #%d) Permutation %d completed	*ABNORMALLY**.\n", 1007, perm);
				       wr (FLG$TRACE | FLG$WARN, "	PVC can't do computed gotos without your assistance.\n");
			               wr (FLG$TRACE | FLG$WARN, "      You must put PVC computed goto labels in your palcode.\n");
			               wr (FLG$TRACE | FLG$WARN, "      Suggestion: Here is a code example to follow:\n");
			               wr (FLG$TRACE | FLG$WARN, "      	pvc$label1$2100:                        ! source label.\n");
			               wr (FLG$TRACE | FLG$WARN, "				hw_jmp_stall (r3)       ! computed goto.\n");
			               wr (FLG$TRACE | FLG$WARN, "		pvc$label1$2100.1:                      ! destination label.\n");
				      wr (FLG$WARN, "Address of computed goto: %X\n\n", current->instruction->address);
			      }
			      wr (FLG$TRACE, "Permutation %d completed because I couldn't follow that goto.\n\n", perm);
#ifdef RRC_COMPILE_FLOW_CODE  
                              { /* RRC:  Added this code for flow display */
                                char temp[PAGE_WIDTH+20];
                                print_connect_line();
                                sprintf (temp, "Need address of computed goto @ 0x%X. Permutation %d", current->instruction->address, perm);
                                print_message_in_box(temp, ":");
                                wr(FLG$FLOW, "\n\n\n");
                              }
#endif /* RRC_COMPILE_FLOW_CODE */
                              if (flags & FLG$CHECKNOW) check_current_path(perm);
			      perm++;
		      }
	      }
	      
	      
	      /* If we're at a conditional branch, consider both paths */	 
	      else if (( idx  == DO_BEQ) 	|| ( idx == DO_BGE) ||
		       ( idx == DO_BGT) 	|| ( idx == DO_BLBC) ||
		       ( idx == DO_BLBS) 	|| ( idx == DO_BLE) ||
		       ( idx == DO_BLT) 	|| ( idx == DO_BNE)) 
		      next_inst_inline = follow_conditionals (current);
	      /* Handle all other intructions. */	    
	      else 
		{
		      /* Is it a branch? */	 
		      if ((idx == DO_BSR) || (idx == DO_BR)) 
			{
			      next_inst_inline = follow_branches (idx, current);
			}
		      /* If we get to here, it must be a 'normal' instruction */	 
		      else 
			{
			      block_length++;
			      /*	 
			       *  Do we need to go any further into this code?
			       *  This is an issue because it is a significant minimization to be able
			       *  to stop tracing the code after being 15 instructions deep into it.
			       *  The longest shadow of any of the pal violations is 7 cycles, which
			       *	is over by 16 instructions. 
			       *  FLS Feb 7,1997 - replaced 16 with 512 (arbitrary) since ev6 restrictions
	 		       *  are not cycle dependent.
			       */	 
		  
		  	     if (strcmp (chip->name, "21264") == 0) {
			          max_block_length = MAX_EV6_PERMUTATION;	/* EV6 */
		  	     } else 
				 max_block_length = 16;

			      if (block_length == max_block_length) 
				{
				      int been_here = FALSE;
				      
				      for_L(go_no_further, Go_No_Further, a) 
					{
					      if (a->addr==current->instruction->address && a->bsr_addr_hash==current_addr_hash)
						been_here = TRUE;
					} 
				      end_L(go_no_further, Go_No_Further, a);
				      
				      if (been_here) 
					{
					      wr (FLG$TRACE, "I've already been here. Permutation %d completed.\n\n", perm);
					      next_inst_inline = FALSE;
#ifdef RRC_COMPILE_FLOW_CODE  
                                              { /* RRC:  Added this code for flow display */
                                                char temp[PAGE_WIDTH+20];
                                                center("|");
                                                wr(FLG$FLOW, "\n");
                                                sprintf (temp, "Been here before.  Permutation %d", perm);
                                                print_message_in_box(temp, ":");
                                                wr(FLG$FLOW, "\n\n\n");
                                              }
#endif /* RRC_COMPILE_FLOW_CODE */
                                              if (flags & FLG$CHECKNOW) check_current_path(perm);
					      perm++;
					}			    
				      else 
					{
					      Go_No_Further *a;
					      a = (Go_No_Further *) malloc (sizeof (Go_No_Further));
					      a->addr = current->instruction->address;
					      a->bsr_addr_hash = current_addr_hash;
					      add_L(go_no_further, Go_No_Further, a);
					}
				}
			      
			}
		}  
         /* Get the next instruction */	 	   
         if (next_inst_inline) 
           {	
             current->inline = Mem_Alloc_NODE(current, NULL, NULL, NULL, 
                                              &exe[current->instruction->address/4 + 1]);
             current = current->inline;
           }

	}
}



int checkforinfloop(NODE *temp, long dest)
/*
 ********************************************************************************
 *	 PURPOSE:
 *	 	Prevents following infinite loops by checking whether this
 *		unconditional branch branches backwords into a flow that will
 *	  	unconditionally hit the unconditional branch again.
 *	 INPUT PARAMETERS:
 *	 	current: the pointer to, naturally, our current spot in the tree
 *		dest:    target address of this unconditional branch
 *	 OUTPUT PARAMETERS:
 *
 *	 IMPLICIT INPUTS:
 *	 IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:
 *	 AUTHOR/DATE:
 *	 	SGM	17-SEP-1992
 *********************************************************************************
 */
{
      int found_target=FALSE;
      int found_escape=FALSE;
      int idx;

      while (temp->previous!=NULL && !found_target && !found_escape) {
	    if (temp->previous->instruction->address==dest)
		found_target=TRUE;	/* this branch does branch back into the same flow */

	    idx = temp->previous->instruction->index;
	    switch(idx) {
		case DO_BSR:
		case DO_BR:
	        case DO_BEQ: 	
		case DO_BGE:
		case DO_BGT:
		case DO_BLBC:
		case DO_BLBS:
		case DO_BLE:
		case DO_BLT:
		case DO_BNE:
		case DO_JSR:
		  found_escape=TRUE;
		default:;
	    }

	    temp = temp->previous;
      }

      /* if we branched back and we didn't find a way out, then we have an infinite loop */
      if (found_target && !found_escape)
	  return(TRUE);
      else
          return(FALSE);
}



int follow_branches (int idx, NODE *current)
/*
 *******************************************************************************
 *	PURPOSE:
 *		Take care of branches
 *	INPUT PARAMETERS:
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:
 *		follow_code
 *		mem_alloc_node
 *		not_ignored
 *		wr
 *	AUTHOR/DATE:
 *		Greg Coladonato, July 92
 ********************************************************************************
 */
{
  int addr, ReturnPC;
#ifdef RRC_COMPILE_FLOW_CODE  
  { /* RRC:  Added this code for flow display */
    print_connect_line();
    print_current_instruction(current, "+");
  }
#endif /* RRC_COMPILE_FLOW_CODE */

#ifndef alt_bsr
      if (idx == DO_BSR && current->instruction->use_bsr_stack) 
#else
      if (idx == DO_BSR)
#endif
	{
	      /*
	       * If this flag is set, then we push the return address onto a stack.
	       * When we hit a JSR with this flag also set, we look to see if 
	       * they are a matching pair, then act accordingly. 
	       */
	      Bsr_Stack *b;

          if (exe[current->instruction->address/4].PVC_Control_Flag & PVC_CTRL_DONT_FOLLOW_BSR)
		{
                  wr (FLG$TRACE, "We are not supposed to follow this BSR.  Continuing with the next instruction.\n");
                  return (TRUE);
		}

	      ReturnPC = current->instruction->address + 4;
	      
	      b = malloc (sizeof(Bsr_Stack));
	      b->pc_plus_four = ReturnPC;
	      b->bsr_family = current->instruction->use_bsr_stack;
	      add_L(bsr_stack, Bsr_Stack, b);
	      wr (FLG$TRACE, "Pushing address onto BSR stack. Elements in stack: %d.\n",
		  length_L(bsr_stack));
	}
      

      addr = exe[(strtol (current->instruction->decoded + 16, NULL, 16) / 4)].address;

      if ((addr >= 1024*1024) || (addr <= 0) || (addr == current->instruction->address)) 
	{
	      if (not_ignored(current->instruction->address, 1006)) 
		{
		      wr (FLG$TRACE | FLG$WARN, "(PVC #%d) Permutation %d completed **ABNORMALLY**.\n",
			  1006, perm);
		      wr (FLG$TRACE | FLG$WARN, "Br to %x out of range or creates an infinite loop.\n",addr);
		      wr (FLG$TRACE | FLG$WARN, "Address of br: %X\n\n", current->instruction->address);
/* RRC:  Now that we are done with this path, let's check it for violations*/
                      if (flags & FLG$CHECKNOW) check_current_path(perm);
		      perm++;
		}
	}
      else 
	{
	      /*
	       * Follow the branch, unless the jsr_dest_list is set
	       */
	      if (exe[current->instruction->address/4].jsr_dest_list == (List *) TRUE) 
		{
		      wr (FLG$TRACE, "We are not supposed to go down this branch. Perm #%d.\n", perm);
/* RRC:  Now that we are done with this path, let's check it for violations*/
                      if (flags & FLG$CHECKNOW) check_current_path(perm);
		      perm++;
		}
	      else 
		{
		      current->inline = Mem_Alloc_NODE(current, NULL, NULL, NULL, 
						       &exe[(strtol (current->instruction->decoded + 16, NULL, 16) / 4)]);
		      /*
		       * Check for infinite loop.  (yes, it sounds stupid, but
		       * people do code em).  We have an infinite loop if following our
		       * path backwards we come to the branch target BEFORE coming to any
		       * other branches (conditional or otherwise) or JSRs.  Note that this code isn't perfect,
		       * but it will catch "simple" infinite loops.  If you code multiple loops that form an infinite
		       * loop, you deserve to have PVC blow up.
		       */
		      if (idx == DO_BR) {
			 if (checkforinfloop(current,addr)) {
			      wr (FLG$TRACE | FLG$WARN, "(PVC #%d) Permutation %d completed **ABNORMALLY**.\n",
				  1006, perm);
			      wr (FLG$TRACE | FLG$WARN, "Br to %x creates an infinite loop.\n",addr);
			      wr (FLG$TRACE | FLG$WARN, "Address of br: %X\n\n", current->instruction->address);
/* RRC:  Now that we are done with this path, let's check it for violations*/
                               if (flags & FLG$CHECKNOW) check_current_path(perm);
			      perm++;			     
			 } else {
		              follow_code (current->inline);
			 }
		      } else {  /* RRC: Added this check for recursive BSR loops.  If the ReturnPC is found in the */
                                /* stack once (in addition to the one just added), then we have recursion. */ 
			  if (InBsrStack (ReturnPC) == 1+1) {
			      wr (FLG$TRACE | FLG$WARN, "(PVC #%d) Permutation %d completed **ABNORMALLY**.\n",
				  1006, perm);
			      wr (FLG$TRACE | FLG$WARN, "BSR to %x creates an infinite loop.\n",addr);
			      wr (FLG$TRACE | FLG$WARN, "Address of BSR: %X\n\n", current->instruction->address);
/* RRC:  Now that we are done with this path, let's check it for violations*/
                               if (flags & FLG$CHECKNOW) check_current_path(perm);
			      perm++;
                          }
                          else
                            follow_code (current->inline);
		      }
		}
	}

      return(FALSE);
}


int checkforrevisit(NODE *temp, long next)
/*
 ********************************************************************************
 *	 PURPOSE:
 *	 	Prevents following loops infinitely by checking whether this flow
 *		already contains these nodes.
 *	 INPUT PARAMETERS:
 *	 	current: the pointer to, naturally, our current spot in the tree
 *		next: address of what would be the next instruction after this one
 *		in the flow
 *	 OUTPUT PARAMETERS:
 *
 *	 IMPLICIT INPUTS:
 *	 IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:
 *	 AUTHOR/DATE:
 *	 	SGM	17-SEP-1992
 *       MODIFIED BY/DATE:
 *              Rogelio Cruz, 7/24/95
 *              This routine had a logic flowed which prevented some branch flows
 *              from being executed.  I rewrote it to fix that problem.
 *********************************************************************************
 */
{
#ifndef OLD_REVISIT_CODE
      NODE *current;

      current = temp;

      /* Search the current tree path backwards looking for a node that has the same
         instruction as our current node.  If the addresses match, then check the bsr
         address hash to see if the call context at that point is the same as ours.  If
         it does, then check the address where we're going from here to see if it's the
         same.  If so, then we can say that we've been here before and there's no need
         to do it again.
      */

      while (temp->previous != NULL) {
	    if ((temp->previous->instruction->address==current->instruction->address) &&
                (temp->previous->bsr_addr_hash == current_addr_hash) &&
	        (temp->instruction->address==next))
	            return(TRUE);
	    temp=temp->previous;
      }
      return(FALSE);
#else

      long found_context=FALSE;
      NODE *current;

      current = temp;

      while (temp->previous != NULL) {
	    if (temp->previous->instruction->address==current->instruction->address) { 
		/*
		 * Check that call context is same
		 */
		  for_L(temp->previous->instruction->bsr_history, Bsr_History, i) {
		    if (i->bsr_addr_hash==current_addr_hash)
		      found_context=TRUE;
		  }
		  end_L(current->instruction->bsr_history, Bsr_History, i);

	          if (temp->instruction->address==next && found_context)
	            return(TRUE);
	    }
	    temp=temp->previous;
      }
      return(FALSE);
#endif

}



int follow_conditionals (NODE *current)
/*
 ********************************************************************************
 *	 PURPOSE:
 *	 	make the follow_code function a more manageable size
 *	 INPUT PARAMETERS:
 *	 	current: the pointer to, naturally, our current spot in the tree
 *	 OUTPUT PARAMETERS:
 *	 IMPLICIT INPUTS:
 *	 IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:
 *		follow_code
 *		mem_alloc_node
 *		wr
 *	 AUTHOR/DATE:
 *	 	Greg Coladonato, June 1992
 *  		Rogelio Cruz, 7/20/95
 *		Added code for adding current call context to the BSR history
 *		after checking for a revisit.  It used to do this before the
 *		check which caused some flows not to be checked.  See add_to_bsr_history
 *		for more details.
 *
 *********************************************************************************
 */
{
      int dont_inline = FALSE, dont_take_branch = FALSE;
      int next_inst_inline = TRUE;

#ifdef RRC_COMPILE_FLOW_CODE
      int RRC_node_local;

      { /* RRC:  Added this code for flow display */
        print_connect_line();
        RRC_node_local = print_current_instruction_with_arrow(current, "-", RRC_node);
        RRC_node++;
      }
#endif /* RRC_COMPILE_FLOW_CODE */
      
      /* check if we have already been down the no-branch path before */
      dont_inline = checkforrevisit(current,current->instruction->address+4);

#ifdef OLD_REVISIT_CODE
      add_to_bsr_history(current);	/* RRC:  Add context AFTER the first check. */ 
#endif

      if (dont_inline) {
	      wr (FLG$TRACE, 
		  "Infinite loop from not taking branch at %X was declined\n", 
		  current->instruction->address); 

#ifdef RRC_COMPILE_FLOW_CODE  
              { /* RRC:  Added this code for flow display */
                print_connect_line();
                print_message_in_box("Infinite loop if continued.", "#");
                wr(FLG$FLOW, "\n\n\n");
              }
#endif /* RRC_COMPILE_FLOW_CODE */
      
      } else {
	      current->inline = Mem_Alloc_NODE(current, NULL, NULL, NULL, &exe[current->instruction->address/4 + 1]);
	      wr (FLG$TRACE, "Branch not taken from %x:\n", current->instruction->address); 
	      if (length_L(bsr_stack)) {
		  Bsr_Stack  *b;
		  Bsr_Stack_List *l;

		  for_L(current->instruction->bsr_stack_clone_list, Bsr_Stack_List,l) {
                      if (l->bsr_addr_hash==current_addr_hash) {
                          wr (FLG$WARN, "Sanity check fail. Identical BSR stack already saved at Address %X\n",current->instruction->address);
		      }
		  }
		  end_L(current->instruction->bsr_stack_clone_list, Bsr_Stack_List,l);

		  wr (FLG$TRACE, "Cloning the current bsr stack  Current address hash = %X \n",current_addr_hash); /*T3.28 */
		  
		  l=malloc (sizeof(Bsr_Stack_List));
		  l->bsr_addr_hash = current_addr_hash;
			
		  create_L(l->bsr_stack);

		  for_L(bsr_stack, Bsr_Stack, i) {
		    b=malloc (sizeof(Bsr_Stack));
	            b->pc_plus_four = i->pc_plus_four;
	            b->bsr_family = i->bsr_family;
	            add_L(l->bsr_stack, Bsr_Stack, b);
		  }
		  end_L(bsr_stack, Bsr_Stack, i);
		  
		  if (l->bsr_stack==0) wr (FLG$WARN, "** Pushing a NULL BSR stack **\n");
		  add_L(current->instruction->bsr_stack_clone_list,Bsr_Stack_List, l);

              }
	      follow_code (current->inline);

/* RRC:  Now that we are done with this path, let's destroy it since it has already been checked. */
              if (flags & FLG$CHECKNOW) {
                destroy_tree(current->inline);  /* RRC: Free tree branch. */
                current->inline = NULL;  
              }

              RestoreBsrStack(current);  /* RRC: Substituted code for a routine */
      }

      current_addr_hash = hash_bsr_stack();

      dont_take_branch = checkforrevisit(current,strtol (current->instruction->decoded + 16, NULL, 16));
#ifdef RRC_COMPILE_FLOW_CODE  
      {
        char temp[PAGE_WIDTH+20];
        sprintf(temp, "[%d - Continued @ 0x%X]", RRC_node_local, strtol (current->instruction->decoded + 16, NULL, 16));
        center(temp);
        wr(FLG$FLOW, "\n");
      }
#endif /* RRC_COMPILE_FLOW_CODE */
      if (dont_take_branch) {
	      wr (FLG$TRACE, 
		  "Infinite loop from taking branch at %X was declined\n", 
		  current->instruction->address);

#ifdef RRC_COMPILE_FLOW_CODE  
              { /* RRC:  Added this code for flow display */
                print_connect_line();
                print_message_in_box("Infinite loop if branch taken.", "#");
                wr(FLG$FLOW, "\n\n\n");
              }
#endif /* RRC_COMPILE_FLOW_CODE */
      } else {   
	      /*
	       * Unless the extra-special flag is set...
	       */
	      if (!exe[current->instruction->address/4].jsr_dest_list) {
		  current->branch = Mem_Alloc_NODE(current, NULL, NULL, NULL, &exe[(strtol (current->instruction->decoded + 16, NULL, 16) / 4)]);
		  wr (FLG$TRACE, 
		      "Branch taken from %X to %X:\n", 
		      current->instruction->address, 
		      current->branch->instruction->address);
		  follow_code (current->branch);

/* RRC:  Now that we are done with this path, let's destroy it since it has already been checked. */
                  if (flags & FLG$CHECKNOW) {
                    destroy_tree(current->branch);  /* RRC: Free tree branch. */
                    current->branch = NULL;  
                  }

	      }
      }
      next_inst_inline = FALSE;
      if (dont_take_branch && dont_inline) {
	      wr (FLG$TRACE, "Progress on this branch terminated. Permutation number %d\n", perm);

#ifdef RRC_COMPILE_FLOW_CODE  
              { /* RRC:  Added this code for flow display */
                char temp[PAGE_WIDTH+20];
                center("|");
                wr(FLG$FLOW, "\n");
                sprintf (temp, "Branch done.  Permutation %d", perm);
                print_message_in_box(temp, ":");
                wr(FLG$FLOW, "\n\n\n");
              }
#endif /* RRC_COMPILE_FLOW_CODE */

/* RRC:  Now that we are done with this path, let's check it for violations*/
              if (flags & FLG$CHECKNOW) check_current_path(perm);
	      perm++;	    
      }

      return (next_inst_inline);
}  



int follow_jump (NODE *current)
/*
 ********************************************************************************
 *	 PURPOSE:
 *	 	This code makes it possible for PVC to follow computed goto calls. 
 *	 INPUT PARAMETERS:
 *	 	current: the pointer to, naturally, our current spot in the tree
 *	 OUTPUT PARAMETERS:
 *	 IMPLICIT INPUTS:
 *	 IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:
 *		follow_code
 *		mem_alloc_node
 *		wr
 *	 AUTHOR/DATE:
 *	 	Greg Coladonato, June 1992
 *       MODIFIED BY:
 *              Rogelio R. Cruz
 *              There was some faulty logic in this routine which caused the
 *              following two promblems:  
 *              1)  Whenever there was a computed goto in the path (JSR, JMP)
 *                  with more than 1 destination, the BSR stack was not cloned
 *                  which caused all sorts of problems if it was changed in one
 *                  of the paths (RET, BSR) whenever it went to execute one of
 *                  the other paths.  Solution: Clone the BSR stack before
 *                  following one of the destination paths.
 *              2)  If one of the branches of the computed goto is a loop and
 *                  the body of the loop is less than the 16 instructions 
 *                  cut-off limit, then the code would go into an endless loop.
 *                  Solution:  Treat the computed goto as a branch and find out
 *                  if we have already been thru the same sequence of instructions.
 *
 *  		Rogelio Cruz, 7/20/95
 *		Added code for adding current call context to the BSR history
 *		after checking for a revisit.  It used to do this before the
 *		check which caused some flows not to be check.  See add_to_bsr_history
 *		for more details.

 *********************************************************************************
 */
{
      NODE *temp;
      int found_dest_address=FALSE;	/* V3.25 true if an address found in jsr_dest_list */
	
#ifdef RRC_COMPILE_FLOW_CODE
      int RRC_node_local = RRC_node;

      { /* RRC:  Added this code for flow display */
        char  temp[PAGE_WIDTH*2] = "";
        char  temp2[PAGE_WIDTH+20] = "";

        print_connect_line();
        print_current_instruction(current, "?");
        center("|");
        wr(FLG$FLOW, "\n");

        for_L(current->instruction->jsr_dest_list, Address, a) 
          {
            sprintf(temp2, "[%d]", RRC_node);
            RRC_node++;
            strcat(temp, temp2);
          }
        end_L(current->instruction->jsr_dest_list, Address, a);
        center(temp);
        wr(FLG$FLOW, "\n\n\n");
      }
#endif /* RRC_COMPILE_FLOW_CODE */


     for_L(current->instruction->jsr_dest_list, Address, a) 
	{
          int have_visit = FALSE;
	  found_dest_address=TRUE;			/* V3.25 */
                  /* check if we have already been down one of the destination paths */
          have_visit = checkforrevisit(current, a->addr);

#ifdef OLD_REVISIT_CODE
         add_to_bsr_history(current);	/* RRC:  Add context AFTER the check. */ 
#endif

#ifdef RRC_COMPILE_FLOW_CODE  
          {
            char temp[PAGE_WIDTH+20];
            sprintf(temp, "[%d - Continued @ 0x%X]", RRC_node_local, a->addr);
            RRC_node_local++;
            center(temp);
            wr(FLG$FLOW, "\n");
          }
#endif /* RRC_COMPILE_FLOW_CODE */

          if (have_visit) {  /* If visited once, then don't do any of the other paths */
	      wr (FLG$TRACE, 
		  "Infinite loop from computed goto path from %X to %X was declined.\n", 
		  current->instruction->address, a->addr); 
              wr (FLG$TRACE, "Permutation %d completed.\n\n", perm);

#ifdef RRC_COMPILE_FLOW_CODE  
              { /* RRC:  Added this code for flow display */
                print_connect_line();
                print_message_in_box("Infinite loop if path is taken.", "#");
                wr(FLG$FLOW, "\n\n\n");
              }
#endif /* RRC_COMPILE_FLOW_CODE */
/* RRC:  Now that we are done with this path, let's check it for violations*/
              if (flags & FLG$CHECKNOW) check_current_path(perm);
              perm++;
	      return (found_dest_address);		/* V3.25 */
          }
              temp = Mem_Alloc_NODE(current, NULL, NULL, NULL, &exe[a->addr / 4]);
	      if (!current->jump_dests) create_L(current->jump_dests);
	      add_L(current->jump_dests, NODE, temp);
	      
	      if (length_L(bsr_stack)) {
		  Bsr_Stack  *b;
		  Bsr_Stack_List *l;
/*
		  for_L(current->instruction->bsr_stack_clone_list, Bsr_Stack_List,l) {
                      if (l->bsr_addr_hash==current_addr_hash) {
                          wr (FLG$WARN, "Sanity check fail. Identical BSR stack already saved at Address %X\n",current->instruction->address);
		      }
		  }
		  end_L(current->instruction->bsr_stack_clone_list, Bsr_Stack_List,l);
*/
		  wr (FLG$TRACE, "Cloning the current bsr stack  Current address hash = %X\n",current_addr_hash); /* T3.28 */
		  
		  l=malloc (sizeof(Bsr_Stack_List));
		  l->bsr_addr_hash = current_addr_hash;
			
		  create_L(l->bsr_stack);

		  for_L(bsr_stack, Bsr_Stack, i) {
		    b=malloc (sizeof(Bsr_Stack));
	            b->pc_plus_four = i->pc_plus_four;
	            b->bsr_family = i->bsr_family;
	            add_L(l->bsr_stack, Bsr_Stack, b);
		  }
		  end_L(bsr_stack, Bsr_Stack, i);
		  
		  if (l->bsr_stack==0) wr (FLG$WARN, "** Pushing a NULL BSR stack **\n");
		  add_L(current->instruction->bsr_stack_clone_list,Bsr_Stack_List, l);

                }

	      wr (FLG$TRACE, "Computed Goto from %X to %X:\n", 
		  current->instruction->address, 
		  temp->instruction->address);

	      follow_code (temp);
/* RRC:  Now that we are done with this path, let's destroy it since it has already been checked. */
              if (flags & FLG$CHECKNOW) {
                remove_nofree_L(current->jump_dests, NODE, temp);
                destroy_tree(temp);  /* RRC: Free tree branch. */
                destroy_L(current->jump_dests, NODE);
              }


              RestoreBsrStack(current);  /* RRC: Substituted code for a routine */

        }
      end_L(current->instruction->jsr_dest_list, Address, a);

 return (found_dest_address);		/* V3.25 */
}  



static void RestoreBsrStack(NODE *current)
/*
 ********************************************************************************
 *	PURPOSE:
 *	      Restores the BSR stack from clone list.
 *	INPUT PARAMETERS:
 *	      current:  Current node.
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Rogelio R. Cruz, 8/10/95.  Moved this code from follow_conditionals
 *              to this new routine that will also be called from follow_jumps.
 *              Note that I have made several changes from the original code, 
 *              including freeing items removed from the clone list and rearranging
 *              the order to prevent possible accesses of NULL pointers.  Moved
 *              the following comment from follow_conditionals.  
 *  		Rogelio Cruz, 7/27/95 
 *		Modified the code to ALWAYS delete the current stack after it is done
 *              with the fall-thru case and then restore it from the clone list or
 *              set to NULL if the clone list is empty.  If we don't do this, then
 *              we have a problem where the stack is not right.  Consider the following
 *              example.
 *
 *              BNE RO, sys_mchk  
 *              BSR R0, t1
 *              HW_REI
 *           t1:BEQ R1, something
 *              RET
 *
 *           something: HW_REI
 *
 *              Here's what was happening before: 
 *              BNE RO, sys_mchk  (the stack is NOT cloned because it's currently empty)  
 *              BSR R0, t1        (add return PC to stack, length = 1)
 *           t1:BEQ R1, something (clone stack and do fall thru path)
 *              RET
 *              HW_REI            (return to BEQ instruction and restore stack from
 *                                 clone list.  Stack length = 1)
 *      something: HW_REI        (The Taken path of BEQ instruction is performed.  No
 *                                RETs are performed before the path is done, therefore,
 *                                the stack is not changed.  The HW_REI causes a return
 *                                to the BNE for performing the TAKEN path.  The stack
 *                                is not restored from the clone list because the stack
 *                                was empty when that instruction and thus the stack from
 *                                the fall-thru path is not destroyed, leaving 1 element
 *                                on the stack when there really isn't any.
 *
 *
 ********************************************************************************
 */
{
  Bsr_Stack_List *i;

  /* deallocate current bsr stack and pop cloned stack from clone list */
  if (bsr_stack) /* RRC: Destroy the current BSR stack now. */
    destroy_L(bsr_stack, Bsr_Stack);

  if (length_L(current->instruction->bsr_stack_clone_list)) {
    wr (FLG$TRACE, "Popping saved BSR stack from clone list\n");
    i=tail_L(current->instruction->bsr_stack_clone_list);

    if (i==0) {  /* RRC:  Changed the order of things to avoid a possible NULL access */
      wr (FLG$WARN, "** Looking to take conditional branch but stack of saved BSR stacks is empty.\n");
      create_L(bsr_stack);
    }
    else {
      if (i->bsr_stack==NULL)
        wr (FLG$WARN, "** BSR stack pointer set to NULL **\n");

      bsr_stack=i->bsr_stack;	/* make the saved bsr stack be the current bsr stack */
      remove_L(current->instruction->bsr_stack_clone_list, Bsr_Stack_List,i); /* RRC: Remove using free() */
    }
  }
  else
    create_L(bsr_stack);  /* RRC:  Create a new one if no clone exists */
  
}


int is_jsr_stack (int index)
/*
 ********************************************************************************
 *	PURPOSE:
 *	        This function takes an index and returns true if it is one of the
 *		instructions that uses the jsr stack.
 *	INPUT PARAMETERS:
 *	      index:    this is one of the DO_XXX values. There is one for each operation.
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Greg Coladonato, June 1991
 ********************************************************************************
 */
{
      if (( index == DO_JSR) ||	( index == DO_BEQ) ||	( index == DO_BGE) ||
	  ( index == DO_BGT) ||	( index == DO_BLBC) ||	( index == DO_BLBS) || 
	  ( index == DO_BLE) ||	( index == DO_BLT) ||	( index == DO_BNE) ||
	  ( index == DO_BSR) || ( index == DO_HW_REI))
	{
	      return (TRUE);
	}
      else 
	{
	      return (FALSE);
	}
}


int is_cond_or_jsr (int index)
/*
 ********************************************************************************
 *	PURPOSE:
 *	        This function takes an index and returns true if it is one of the pc-
 *		changing branch operations and false otherwise.
 *	INPUT PARAMETERS:
 *	      index:    this is one of the DO_XXX values. There is one for each operation.
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Greg Coladonato, June 1991
 ********************************************************************************
 */
{
      if (( index == DO_JSR) ||	( index == DO_BEQ) ||	( index == DO_BGE) ||
	  ( index == DO_BGT) ||	( index == DO_BLBC) ||	( index == DO_BLBS) || 
	  ( index == DO_BLE) ||	( index == DO_BLT) ||	( index == DO_BNE)) 
	{
	      return (TRUE);
	}
      else 
	{
	      return (FALSE);
	}
}




int is_br_or_bsr (int index)
/*
 ********************************************************************************
 *	PURPOSE:
 *	        This function takes an index and returns true if it is one of the 
 *		unconditional branches.
 *	INPUT PARAMETERS:
 *	      index:    this is one of the DO_XXX values. There is one for each operation.
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Greg Coladonato, June 1991
 ********************************************************************************
 */
{
      if ((index == DO_BSR) ||	(index == DO_BR)) 
	{
	      return (TRUE);
	}
      else 
	{
	      return (FALSE);
	}
}



void alpha_init_classes(void)
/*
 ********************************************************************************
 *	 PURPOSE:
 *	 	To initialize the array that allows us to characterize the behavior
 *	 	of different classes of instructions.
 *	 INPUT PARAMETERS:
 *	 	An array of pointers to CLASS_INFO structures.                                           
 *	 OUTPUT PARAMETERS:
 *	 IMPLICIT INPUTS:
 *	 IMPLICIT OUTPUTS:
 *	 AUTHOR/DATE:
 *	 	Dirk Meyer, for the performance model
 ********************************************************************************
 */
{
      struct class_info *icp, *temp;
      
      /* SEXT class  */				/* V3.24 */
      /* this class definition is for instruction decode only */
      icp = (struct class_info *) calloc(1, sizeof(struct class_info));
      icp->arch = (struct architected *) calloc (1, sizeof (struct architected));

      icp->arch->op_ra = FALSE;			/* V3.24 */
      icp->arch->op_rb = TRUE;
      icp->arch->op_rc = TRUE;
      icp->arch->ra_flt = FALSE;
      icp->arch->rb_flt = FALSE;
      icp->arch->rc_flt = FALSE;
      icp->arch->dest_reg = Rc;
      icp->arch->dest_flt = FALSE;
      icp->arch->chk_lit_mode = TRUE;

      AssignUnqIcp (DO_SEXTB);			/* V3.24 */
      AssignUnqIcp (DO_SEXTW);			/* V3.24 */
      AssignUnqIcp (DO_CTPOP);			/* V3.24 */
      AssignUnqIcp (DO_CTLZ);			/* V3.24 */
      AssignUnqIcp (DO_CTTZ);			/* V3.24 */
      AssignUnqIcp (DO_UNPKBW);			/* V3.24 */
      AssignUnqIcp (DO_UNPKBL);			/* V3.24 */
      AssignUnqIcp (DO_PKWB);			/* V3.24 */
      AssignUnqIcp (DO_PKLB);			/* V3.24 */

      /* FTOIT class  e.g. ftoit fa,rc */			/* V3.24 */
      /* this class definition is for instruction decode only */
      icp = (struct class_info *) calloc(1, sizeof(struct class_info));
      icp->arch = (struct architected *) calloc (1, sizeof (struct architected));

      icp->arch->op_ra = FALSE;			/* V3.24 */
      icp->arch->op_rb = FALSE;
      icp->arch->op_rc = TRUE;
      icp->arch->ra_flt = TRUE;
      icp->arch->rb_flt = FALSE;
      icp->arch->rc_flt = FALSE;
      icp->arch->dest_reg = Rc;
      icp->arch->dest_flt = FALSE;
      icp->arch->chk_lit_mode = FALSE;

      AssignUnqIcp (DO_FTOIT);			/* V3.24 */
      AssignUnqIcp (DO_FTOIS);			/* V3.24 */

      /* ITOF class  e.g. ITOFT Ra,Fc */			/* V3.24 */
      /* this class definition is for instruction decode only */
      icp = (struct class_info *) calloc(1, sizeof(struct class_info));
      icp->arch = (struct architected *) calloc (1, sizeof (struct architected));

      icp->arch->op_ra = TRUE;			/* V3.24 */
      icp->arch->op_rb = FALSE;
      icp->arch->op_rc = FALSE;
      icp->arch->ra_flt = FALSE;
      icp->arch->rb_flt = FALSE;
      icp->arch->rc_flt = TRUE;			/* V3.24 */
      icp->arch->dest_reg = Rc;
      icp->arch->dest_flt = TRUE;
      icp->arch->chk_lit_mode = FALSE;

      AssignUnqIcp (DO_ITOFF);			/* V3.24 */
      AssignUnqIcp (DO_ITOFS);			/* V3.24 */
      AssignUnqIcp (DO_ITOFT);			/* V3.24 */


      /* IADDLOG class  */
      /* this class definition is for instruction decode only */
      
      icp = (
      struct class_info *) calloc(1, sizeof(struct class_info));
      icp->arch = (struct architected *) calloc (1, sizeof (struct architected));
      
      icp->arch->op_ra = TRUE;
      icp->arch->op_rb = TRUE;
      icp->arch->op_rc = TRUE;	
      icp->arch->ra_flt = FALSE;
      icp->arch->rb_flt = FALSE;
      icp->arch->rc_flt = FALSE;
      icp->arch->dest_reg = Rc;
      icp->arch->dest_flt = FALSE;
      icp->arch->chk_lit_mode = TRUE;
      
      /* integer arithmetic */
      			
      AssignUnqIcp (DO_PERR);			/* V3.24 */
      AssignUnqIcp (DO_MINUB8);			/* V3.24 */
      AssignUnqIcp (DO_MINUW4);			/* V3.24 */
      AssignUnqIcp (DO_MINSB8);			/* V3.24 */
      AssignUnqIcp (DO_MINSW4);			/* V3.24 */
      AssignUnqIcp (DO_MAXUB8);			/* V3.24 */
      AssignUnqIcp (DO_MAXUW4);			/* V3.24 */
      AssignUnqIcp (DO_MAXSB8);			/* V3.24 */
      AssignUnqIcp (DO_MAXSW4);			/* V3.24 */
      AssignUnqIcp (DO_ADDL);
      AssignUnqIcp (DO_ADDLV);
      AssignUnqIcp (DO_ADDQ);
      AssignUnqIcp (DO_ADDQV);
      AssignUnqIcp (DO_SUBL);
      AssignUnqIcp (DO_SUBLV);
      AssignUnqIcp (DO_SUBQ);
      AssignUnqIcp (DO_SUBQV);
      AssignUnqIcp (DO_S4ADDL);
      AssignUnqIcp (DO_S4ADDQ);
      AssignUnqIcp (DO_S8ADDL);
      AssignUnqIcp (DO_S8ADDQ);
      AssignUnqIcp (DO_S4SUBL);
      AssignUnqIcp (DO_S4SUBQ);
      AssignUnqIcp (DO_S8SUBL);
      AssignUnqIcp (DO_S8SUBQ);
      
      /* logicals */
      
      AssignUnqIcp (DO_AMASK);		/* V3.24 */
      AssignUnqIcp (DO_IMPLVER);	/* V3.24 */
      AssignUnqIcp (DO_AND);
      AssignUnqIcp (DO_BIC);
      AssignUnqIcp (DO_EQV);
      AssignUnqIcp (DO_OR);
      AssignUnqIcp (DO_ORNOT);
      AssignUnqIcp (DO_XOR);
      
      /* integer compares */
      free (icp);   /* RRC:  Free previous calloc of icp */   
      icp = (struct class_info *) calloc(1, sizeof(struct class_info));
      icp->arch = (struct architected *) calloc (1, sizeof (struct architected));
      
      icp->arch->op_ra = TRUE;
      icp->arch->op_rb = TRUE;
      icp->arch->op_rc = TRUE;			/* V3.24 */
      icp->arch->ra_flt = FALSE;
      icp->arch->rb_flt = FALSE;
      icp->arch->rc_flt = FALSE;
      icp->arch->dest_reg = Rc;
      icp->arch->dest_flt = FALSE;
      icp->arch->chk_lit_mode = TRUE;
      
      AssignUnqIcp (DO_CMPB);
      AssignUnqIcp (DO_CMPBGE);
      AssignUnqIcp (DO_CMPEQ);
      AssignUnqIcp (DO_CMPLE);
      AssignUnqIcp (DO_CMPLT);
      AssignUnqIcp (DO_CMPNE);
      AssignUnqIcp (DO_CMPUGE);
      AssignUnqIcp (DO_CMPUGT);
      AssignUnqIcp (DO_CMPULE);
      AssignUnqIcp (DO_CMPULT);
      
      /* SHIFTCM class */
      free (icp);   /* RRC:  Free previous calloc of icp */   
      icp = (struct class_info *) calloc(1, sizeof(struct class_info));
      icp->arch = (struct architected *) calloc (1, sizeof (struct architected));
      
      icp->arch->op_ra = TRUE;
      icp->arch->op_rb = TRUE;		/* V3.24 */
      icp->arch->op_rc = TRUE;
      icp->arch->ra_flt = FALSE;
      icp->arch->rb_flt = FALSE;
      icp->arch->rc_flt = FALSE;
      icp->arch->dest_reg = Rc;
      icp->arch->dest_flt = FALSE;
      icp->arch->chk_lit_mode = TRUE;
      
      /* CMOVEx */
      
      AssignUnqIcp (DO_CMOVEQ);
      AssignUnqIcp (DO_CMOVGE);
      AssignUnqIcp (DO_CMOVGT);
      AssignUnqIcp (DO_CMOVLBC);
      AssignUnqIcp (DO_CMOVLBS);
      AssignUnqIcp (DO_CMOVLE);
      AssignUnqIcp (DO_CMOVLT);
      AssignUnqIcp (DO_CMOVNE);
      
      /* bit field */
      
      AssignUnqIcp (DO_EXTBH);
      AssignUnqIcp (DO_EXTBL);
      AssignUnqIcp (DO_EXTLH);
      AssignUnqIcp (DO_EXTLL);
      AssignUnqIcp (DO_EXTQH);
      AssignUnqIcp (DO_EXTQL);
      AssignUnqIcp (DO_EXTWH);
      AssignUnqIcp (DO_EXTWL);
      AssignUnqIcp (DO_INSBH);
      AssignUnqIcp (DO_INSBL);
      AssignUnqIcp (DO_INSLH);
      AssignUnqIcp (DO_INSLL);
      AssignUnqIcp (DO_INSQH);
      AssignUnqIcp (DO_INSQL);
      AssignUnqIcp (DO_INSWH);
      AssignUnqIcp (DO_INSWL);
      AssignUnqIcp (DO_MRGB);
      AssignUnqIcp (DO_ZAP);
      AssignUnqIcp (DO_MSKBH);
      AssignUnqIcp (DO_MSKBL);
      AssignUnqIcp (DO_MSKLH);
      AssignUnqIcp (DO_MSKLL);
      AssignUnqIcp (DO_ZAPNOT);
      AssignUnqIcp (DO_MSKQH);
      AssignUnqIcp (DO_MSKQL);
      AssignUnqIcp (DO_MSKWH);
      AssignUnqIcp (DO_MSKWL);
      
      /* shift */
      
      AssignUnqIcp (DO_ROT);
      AssignUnqIcp (DO_SLL);
      AssignUnqIcp (DO_SRA);
      AssignUnqIcp (DO_SRL);
      
      /* integer longword muls */
      free (icp);   /* RRC:  Free previous calloc of icp */         
      icp = (struct class_info *) calloc(1, sizeof(struct class_info));
      icp->arch = (struct architected *) calloc (1, sizeof (struct architected));
      
      icp->arch->op_ra = TRUE;
      icp->arch->op_rb = TRUE;
      icp->arch->op_rc = TRUE;	/* V3.24 */
      icp->arch->ra_flt = FALSE;
      icp->arch->rb_flt = FALSE;
      icp->arch->rc_flt = FALSE;
      icp->arch->dest_reg = Rc;
      icp->arch->dest_flt = FALSE;
      icp->arch->chk_lit_mode = TRUE;
      
      AssignUnqIcp (DO_MULL);
      AssignUnqIcp (DO_MULLV);
      
      /* integer muls */
      free (icp);   /* RRC:  Free previous calloc of icp */   
      icp = (struct class_info *) calloc(1, sizeof(struct class_info));
      icp->arch = (struct architected *) calloc (1, sizeof (struct architected));
      
      icp->arch->op_ra = TRUE;
      icp->arch->op_rb = TRUE;
      icp->arch->op_rc = TRUE;		/* V3.24 */
      icp->arch->ra_flt = FALSE;
      icp->arch->rb_flt = FALSE;
      icp->arch->rc_flt = FALSE;
      icp->arch->dest_reg = Rc;
      icp->arch->dest_flt = FALSE;
      icp->arch->chk_lit_mode = TRUE;
      
      AssignUnqIcp (DO_MULQ);
      AssignUnqIcp (DO_MULQV);
      AssignUnqIcp (DO_UMULH);
      
      /* Normal floating point (excluding FDIV) (2 operands & one dest) */
      free (icp);   /* RRC:  Free previous calloc of icp */         
      icp = (struct class_info *)calloc(1, sizeof(struct class_info));
      icp->arch = (struct architected *) calloc (1, sizeof (struct architected));
      
      icp->arch->op_ra = FALSE;		/* V3.24 */
      icp->arch->op_rb = FALSE;		/* V3.24 */
      icp->arch->op_rc = FALSE;
      icp->arch->ra_flt = TRUE;
      icp->arch->rb_flt = TRUE;
      icp->arch->rc_flt = TRUE;
      icp->arch->dest_reg = Rc;
      icp->arch->dest_flt = TRUE;
      icp->arch->chk_lit_mode = FALSE;
      
      AssignUnqIcp (DO_ADDD);
      AssignUnqIcp (DO_ADDDC);
      AssignUnqIcp (DO_ADDDS);
      AssignUnqIcp (DO_ADDDSC);
      AssignUnqIcp (DO_ADDDSU);
      AssignUnqIcp (DO_ADDDSUC);
      AssignUnqIcp (DO_ADDDU);
      AssignUnqIcp (DO_ADDDUC);
      AssignUnqIcp (DO_ADDF);
      AssignUnqIcp (DO_ADDFC);
      AssignUnqIcp (DO_ADDFS);
      AssignUnqIcp (DO_ADDFSC);
      AssignUnqIcp (DO_ADDFSU);
      AssignUnqIcp (DO_ADDFSUC);
      AssignUnqIcp (DO_ADDFU);
      AssignUnqIcp (DO_ADDFUC);
      AssignUnqIcp (DO_ADDG);
      AssignUnqIcp (DO_ADDGC);
      AssignUnqIcp (DO_ADDGS);
      AssignUnqIcp (DO_ADDGSC);
      AssignUnqIcp (DO_ADDGSU);
      AssignUnqIcp (DO_ADDGSUC);
      AssignUnqIcp (DO_ADDGU);
      AssignUnqIcp (DO_ADDGUC);
      AssignUnqIcp (DO_ADDS);
      AssignUnqIcp (DO_ADDSC);
      AssignUnqIcp (DO_ADDSM);
      AssignUnqIcp (DO_ADDSP);
      AssignUnqIcp (DO_ADDSS);
      AssignUnqIcp (DO_ADDSSC);
      AssignUnqIcp (DO_ADDSSM);
      AssignUnqIcp (DO_ADDSSD);
      AssignUnqIcp (DO_ADDSSU);
      AssignUnqIcp (DO_ADDSSUC);
      AssignUnqIcp (DO_ADDSSUI);
      AssignUnqIcp (DO_ADDSSUIC);
      AssignUnqIcp (DO_ADDSSUIM);
      AssignUnqIcp (DO_ADDSSUIP);
      AssignUnqIcp (DO_ADDSSUM);
      AssignUnqIcp (DO_ADDSSUP);
      AssignUnqIcp (DO_ADDSU);
      AssignUnqIcp (DO_ADDSUC);
      AssignUnqIcp (DO_ADDSUM);
      AssignUnqIcp (DO_ADDSUP);
      AssignUnqIcp (DO_ADDT);
      AssignUnqIcp (DO_ADDTC);
      AssignUnqIcp (DO_ADDTM);
      AssignUnqIcp (DO_ADDTP);
      AssignUnqIcp (DO_ADDTS);
      AssignUnqIcp (DO_ADDTSC);
      AssignUnqIcp (DO_ADDTSU);
      AssignUnqIcp (DO_ADDTSM);
      AssignUnqIcp (DO_ADDTSD);
      AssignUnqIcp (DO_ADDTSUC);
      AssignUnqIcp (DO_ADDTSUI);
      AssignUnqIcp (DO_ADDTSUIC);
      AssignUnqIcp (DO_ADDTSUIM);
      AssignUnqIcp (DO_ADDTSUIP);
      AssignUnqIcp (DO_ADDTSUM);
      AssignUnqIcp (DO_ADDTSUP);
      AssignUnqIcp (DO_ADDTU);
      AssignUnqIcp (DO_ADDTUC);
      AssignUnqIcp (DO_ADDTUM);
      AssignUnqIcp (DO_ADDTUP);
      
      AssignUnqIcp (DO_CMPDEQ);
      AssignUnqIcp (DO_CMPDEQS);
      AssignUnqIcp (DO_CMPDGE);
      AssignUnqIcp (DO_CMPDGES);
      AssignUnqIcp (DO_CMPDGT);
      AssignUnqIcp (DO_CMPDGTS);
      AssignUnqIcp (DO_CMPDLE);
      AssignUnqIcp (DO_CMPDLES);
      AssignUnqIcp (DO_CMPDLT);
      AssignUnqIcp (DO_CMPDLTS);
      AssignUnqIcp (DO_CMPDNE);
      AssignUnqIcp (DO_CMPDNES);
      AssignUnqIcp (DO_CMPFEQ);
      AssignUnqIcp (DO_CMPFEQS);
      AssignUnqIcp (DO_CMPFGE);
      AssignUnqIcp (DO_CMPFGES);
      AssignUnqIcp (DO_CMPFGT);
      AssignUnqIcp (DO_CMPFGTS);
      AssignUnqIcp (DO_CMPFLE);
      AssignUnqIcp (DO_CMPFLES);
      AssignUnqIcp (DO_CMPFLT);
      AssignUnqIcp (DO_CMPFLTS);
      AssignUnqIcp (DO_CMPFNE);
      AssignUnqIcp (DO_CMPFNES);
      AssignUnqIcp (DO_CMPGE);
      AssignUnqIcp (DO_CMPGEQ);
      AssignUnqIcp (DO_CMPGEQS);
      AssignUnqIcp (DO_CMPGGE);
      AssignUnqIcp (DO_CMPGGES);
      AssignUnqIcp (DO_CMPGGT);
      AssignUnqIcp (DO_CMPGGTS);
      AssignUnqIcp (DO_CMPGLE);
      AssignUnqIcp (DO_CMPGLES);
      AssignUnqIcp (DO_CMPGLT);
      AssignUnqIcp (DO_CMPGLTS);
      AssignUnqIcp (DO_CMPGNE);
      AssignUnqIcp (DO_CMPGNES);
      AssignUnqIcp (DO_CMPGT);
      AssignUnqIcp (DO_CMPSEQ);
      AssignUnqIcp (DO_CMPSEQSU);
      AssignUnqIcp (DO_CMPSLE);
      AssignUnqIcp (DO_CMPSLESU);
      AssignUnqIcp (DO_CMPSLT);
      AssignUnqIcp (DO_CMPSLTSU);
      AssignUnqIcp (DO_CMPSUN);
      AssignUnqIcp (DO_CMPSUNSU);
      AssignUnqIcp (DO_CMPTEQ);
      AssignUnqIcp (DO_CMPTEQSU);
      AssignUnqIcp (DO_CMPTLE);
      AssignUnqIcp (DO_CMPTLESU);
      AssignUnqIcp (DO_CMPTLT);
      AssignUnqIcp (DO_CMPTLTSU);
      AssignUnqIcp (DO_CMPTUN);
      AssignUnqIcp (DO_CMPTUNSU);
      
      AssignUnqIcp (DO_FCMOVEQ);
      AssignUnqIcp (DO_FCMOVGE);
      AssignUnqIcp (DO_FCMOVGT);
      AssignUnqIcp (DO_FCMOVLE);
      AssignUnqIcp (DO_FCMOVLT);
      AssignUnqIcp (DO_FCMOVNE);
      
      AssignUnqIcp (DO_MULD);
      AssignUnqIcp (DO_MULDC);
      AssignUnqIcp (DO_MULDS);
      AssignUnqIcp (DO_MULDSC);
      AssignUnqIcp (DO_MULDSU);
      AssignUnqIcp (DO_MULDSUC);
      AssignUnqIcp (DO_MULDU);
      AssignUnqIcp (DO_MULDUC);
      AssignUnqIcp (DO_MULF);
      AssignUnqIcp (DO_MULFC);
      AssignUnqIcp (DO_MULFS);
      AssignUnqIcp (DO_MULFSC);
      AssignUnqIcp (DO_MULFSU);
      AssignUnqIcp (DO_MULFSUC);
      AssignUnqIcp (DO_MULFU);
      AssignUnqIcp (DO_MULFUC);
      AssignUnqIcp (DO_MULG);
      AssignUnqIcp (DO_MULGC);
      AssignUnqIcp (DO_MULGS);
      AssignUnqIcp (DO_MULGSC);
      AssignUnqIcp (DO_MULGSU);
      AssignUnqIcp (DO_MULGSUC);
      AssignUnqIcp (DO_MULGU);
      AssignUnqIcp (DO_MULGUC);
      AssignUnqIcp (DO_MULS);
      AssignUnqIcp (DO_MULSC);
      AssignUnqIcp (DO_MULSM);
      AssignUnqIcp (DO_MULSP);
      AssignUnqIcp (DO_MULSSU);
      AssignUnqIcp (DO_MULSSUC);
      AssignUnqIcp (DO_MULSSUI);
      AssignUnqIcp (DO_MULSSUIC);
      AssignUnqIcp (DO_MULSSUIM);
      AssignUnqIcp (DO_MULSSUIP);
      AssignUnqIcp (DO_MULSSUM);
      AssignUnqIcp (DO_MULSSUP);
      AssignUnqIcp (DO_MULSU);
      AssignUnqIcp (DO_MULSUC);
      AssignUnqIcp (DO_MULSUM);
      AssignUnqIcp (DO_MULSUP);
      AssignUnqIcp (DO_MULT);
      AssignUnqIcp (DO_MULTC);
      AssignUnqIcp (DO_MULTM);
      AssignUnqIcp (DO_MULTP);
      AssignUnqIcp (DO_MULTSU);
      AssignUnqIcp (DO_MULTSUC);
      AssignUnqIcp (DO_MULTSUI);
      AssignUnqIcp (DO_MULTSUIC);
      AssignUnqIcp (DO_MULTSUIM);
      AssignUnqIcp (DO_MULTSUIP);
      AssignUnqIcp (DO_MULTSUM);
      AssignUnqIcp (DO_MULTSUP);
      AssignUnqIcp (DO_MULTU);
      AssignUnqIcp (DO_MULTUC);
      AssignUnqIcp (DO_MULTUM);
      AssignUnqIcp (DO_MULTUP);
      
      AssignUnqIcp (DO_SUBD);
      AssignUnqIcp (DO_SUBDC);
      AssignUnqIcp (DO_SUBDS);
      AssignUnqIcp (DO_SUBDSC);
      AssignUnqIcp (DO_SUBDSU);
      AssignUnqIcp (DO_SUBDSUC);
      AssignUnqIcp (DO_SUBDU);
      AssignUnqIcp (DO_SUBDUC);
      AssignUnqIcp (DO_SUBF);
      AssignUnqIcp (DO_SUBFC);
      AssignUnqIcp (DO_SUBFS);
      AssignUnqIcp (DO_SUBFSC);
      AssignUnqIcp (DO_SUBFSU);
      AssignUnqIcp (DO_SUBFSUC);
      AssignUnqIcp (DO_SUBFU);
      AssignUnqIcp (DO_SUBFUC);
      AssignUnqIcp (DO_SUBG);
      AssignUnqIcp (DO_SUBGC);
      AssignUnqIcp (DO_SUBGS);
      AssignUnqIcp (DO_SUBGSC);
      AssignUnqIcp (DO_SUBGSU);
      AssignUnqIcp (DO_SUBGSUC);
      AssignUnqIcp (DO_SUBGU);
      AssignUnqIcp (DO_SUBGUC);
      AssignUnqIcp (DO_SUBS);
      AssignUnqIcp (DO_SUBSC);
      AssignUnqIcp (DO_SUBSM);
      AssignUnqIcp (DO_SUBSP);
      AssignUnqIcp (DO_SUBSSU);
      AssignUnqIcp (DO_SUBSSUC);
      AssignUnqIcp (DO_SUBSSUI);
      AssignUnqIcp (DO_SUBSSUIC);
      AssignUnqIcp (DO_SUBSSUIM);
      AssignUnqIcp (DO_SUBSSUIP);
      AssignUnqIcp (DO_SUBSSUM);
      AssignUnqIcp (DO_SUBSSUP);
      AssignUnqIcp (DO_SUBSU);
      AssignUnqIcp (DO_SUBSUC);
      AssignUnqIcp (DO_SUBSUM);
      AssignUnqIcp (DO_SUBSUP);
      AssignUnqIcp (DO_SUBT);
      AssignUnqIcp (DO_SUBTC);
      AssignUnqIcp (DO_SUBTM);
      AssignUnqIcp (DO_SUBTP);
      AssignUnqIcp (DO_SUBTSU);
      AssignUnqIcp (DO_SUBTSUC);
      AssignUnqIcp (DO_SUBTSUI);
      AssignUnqIcp (DO_SUBTSUIC);
      AssignUnqIcp (DO_SUBTSUIM);
      AssignUnqIcp (DO_SUBTSUIP);
      AssignUnqIcp (DO_SUBTSUM);
      AssignUnqIcp (DO_SUBTSUP);
      AssignUnqIcp (DO_SUBTU);
      AssignUnqIcp (DO_SUBTUC);
      AssignUnqIcp (DO_SUBTUM);
      AssignUnqIcp (DO_SUBTUP);
      
      /* FDIV (2 operands & one dest) */
      free (icp);   /* RRC:  Free previous calloc of icp */         
      icp = (struct class_info *)calloc(1, sizeof(struct class_info));
      icp->arch = (struct architected *) calloc (1, sizeof (struct architected));
      
      icp->arch->op_ra = FALSE;		/* V3.24 */
      icp->arch->op_rb = FALSE;		/* V3.24 */
      icp->arch->op_rc = FALSE;
      icp->arch->ra_flt = TRUE;
      icp->arch->rb_flt = TRUE;
      icp->arch->rc_flt = TRUE;
      icp->arch->dest_reg = Rc;
      icp->arch->dest_flt = TRUE;
      icp->arch->chk_lit_mode = FALSE;
      
      AssignUnqIcp (DO_DIV);
      AssignUnqIcp (DO_DIVD);
      AssignUnqIcp (DO_DIVDC);
      AssignUnqIcp (DO_DIVDS);
      AssignUnqIcp (DO_DIVDSC);
      AssignUnqIcp (DO_DIVDSU);
      AssignUnqIcp (DO_DIVDSUC);
      AssignUnqIcp (DO_DIVDU);
      AssignUnqIcp (DO_DIVDUC);
      AssignUnqIcp (DO_DIVF);
      AssignUnqIcp (DO_DIVFC);
      AssignUnqIcp (DO_DIVFS);
      AssignUnqIcp (DO_DIVFSC);
      AssignUnqIcp (DO_DIVFSU);
      AssignUnqIcp (DO_DIVFSUC);
      AssignUnqIcp (DO_DIVFU);
      AssignUnqIcp (DO_DIVFUC);
      AssignUnqIcp (DO_DIVG);
      AssignUnqIcp (DO_DIVGC);
      AssignUnqIcp (DO_DIVGS);
      AssignUnqIcp (DO_DIVGSC);
      AssignUnqIcp (DO_DIVGSU);
      AssignUnqIcp (DO_DIVGSUC);
      AssignUnqIcp (DO_DIVGU);
      AssignUnqIcp (DO_DIVGUC);
      AssignUnqIcp (DO_DIVS);
      AssignUnqIcp (DO_DIVSC);
      AssignUnqIcp (DO_DIVSM);
      AssignUnqIcp (DO_DIVSP);
      AssignUnqIcp (DO_DIVSSU);
      AssignUnqIcp (DO_DIVSSUC);
      AssignUnqIcp (DO_DIVSSUI);
      AssignUnqIcp (DO_DIVSSUIC);
      AssignUnqIcp (DO_DIVSSUIM);
      AssignUnqIcp (DO_DIVSSUIP);
      AssignUnqIcp (DO_DIVSSUM);
      AssignUnqIcp (DO_DIVSSUP);
      AssignUnqIcp (DO_DIVSU);
      AssignUnqIcp (DO_DIVSUC);
      AssignUnqIcp (DO_DIVSUM);
      AssignUnqIcp (DO_DIVSUP);
      AssignUnqIcp (DO_DIVT);
      AssignUnqIcp (DO_DIVTC);
      AssignUnqIcp (DO_DIVTM);
      AssignUnqIcp (DO_DIVTP);
      AssignUnqIcp (DO_DIVTSU);
      AssignUnqIcp (DO_DIVTSUC);
      AssignUnqIcp (DO_DIVTSUI);
      AssignUnqIcp (DO_DIVTSUIC);
      AssignUnqIcp (DO_DIVTSUIM);
      AssignUnqIcp (DO_DIVTSUIP);
      AssignUnqIcp (DO_DIVTSUM);
      AssignUnqIcp (DO_DIVTSUP);
      AssignUnqIcp (DO_DIVTU);
      AssignUnqIcp (DO_DIVTUC);
      AssignUnqIcp (DO_DIVTUM);
      AssignUnqIcp (DO_DIVTUP);
      AssignUnqIcp (DO_DIVV);
      
      /* One operand floating point */
      free (icp);   /* RRC:  Free previous calloc of icp */         
      icp = (struct class_info *)calloc(1, sizeof(struct class_info));
      icp->arch = (struct architected *) calloc (1, sizeof (struct architected));
      
      icp->arch->op_ra = FALSE;
      icp->arch->op_rb = FALSE;
      icp->arch->op_rc = FALSE;
      icp->arch->ra_flt = TRUE;
      icp->arch->rb_flt = TRUE;
      icp->arch->rc_flt = TRUE;
      icp->arch->dest_reg = Rc;
      icp->arch->dest_flt = TRUE;
      icp->arch->chk_lit_mode = FALSE;
      
      /* float converts */
      
      AssignUnqIcp (DO_CPYS);
      AssignUnqIcp (DO_CPYSE);
      AssignUnqIcp (DO_CPYSEE);
      AssignUnqIcp (DO_CPYSN);
      
      AssignUnqIcp (DO_CVTDF);
      AssignUnqIcp (DO_CVTDFC);
      AssignUnqIcp (DO_CVTDFS);
      AssignUnqIcp (DO_CVTDFSC);
      AssignUnqIcp (DO_CVTDFSU);
      AssignUnqIcp (DO_CVTDFSUC);
      AssignUnqIcp (DO_CVTDFU);
      AssignUnqIcp (DO_CVTDFUC);
      AssignUnqIcp (DO_CVTDL);
      AssignUnqIcp (DO_CVTDLC);
      AssignUnqIcp (DO_CVTDLS);
      AssignUnqIcp (DO_CVTDLSC);
      AssignUnqIcp (DO_CVTDLSV);
      AssignUnqIcp (DO_CVTDLSVC);
      AssignUnqIcp (DO_CVTDLV);
      AssignUnqIcp (DO_CVTDLVC);
      AssignUnqIcp (DO_CVTDQ);
      AssignUnqIcp (DO_CVTDQC);
      AssignUnqIcp (DO_CVTDQS);
      AssignUnqIcp (DO_CVTDQSC);
      AssignUnqIcp (DO_CVTDQSV);
      AssignUnqIcp (DO_CVTDQSVC);
      AssignUnqIcp (DO_CVTDQV);
      AssignUnqIcp (DO_CVTDQVC);
      AssignUnqIcp (DO_CVTFD);
      AssignUnqIcp (DO_CVTFDC);
      AssignUnqIcp (DO_CVTFDS);
      AssignUnqIcp (DO_CVTFDSC);
      AssignUnqIcp (DO_CVTFDSU);
      AssignUnqIcp (DO_CVTFDSUC);
      AssignUnqIcp (DO_CVTFDU);
      AssignUnqIcp (DO_CVTFDUC);
      AssignUnqIcp (DO_CVTFG);
      AssignUnqIcp (DO_CVTFGC);
      AssignUnqIcp (DO_CVTFGS);
      AssignUnqIcp (DO_CVTFGSC);
      AssignUnqIcp (DO_CVTFGSU);
      AssignUnqIcp (DO_CVTFGSUC);
      AssignUnqIcp (DO_CVTFGU);
      AssignUnqIcp (DO_CVTFGUC);
      AssignUnqIcp (DO_CVTFL);
      AssignUnqIcp (DO_CVTFLC);
      AssignUnqIcp (DO_CVTFLS);
      AssignUnqIcp (DO_CVTFLSC);
      AssignUnqIcp (DO_CVTFLSV);
      AssignUnqIcp (DO_CVTFLSVC);
      AssignUnqIcp (DO_CVTFLV);
      AssignUnqIcp (DO_CVTFLVC);
      AssignUnqIcp (DO_CVTFQ);
      AssignUnqIcp (DO_CVTFQC);
      AssignUnqIcp (DO_CVTFQS);
      AssignUnqIcp (DO_CVTFQSC);
      AssignUnqIcp (DO_CVTFQSV);
      AssignUnqIcp (DO_CVTFQSVC);
      AssignUnqIcp (DO_CVTFQV);
      AssignUnqIcp (DO_CVTFQVC);
      AssignUnqIcp (DO_CVTGF);
      AssignUnqIcp (DO_CVTGFC);
      AssignUnqIcp (DO_CVTGFS);
      AssignUnqIcp (DO_CVTGFSC);
      AssignUnqIcp (DO_CVTGFSU);
      AssignUnqIcp (DO_CVTGFSUC);
      AssignUnqIcp (DO_CVTGFU);
      AssignUnqIcp (DO_CVTGFUC);
      AssignUnqIcp (DO_CVTGL);
      AssignUnqIcp (DO_CVTGLC);
      AssignUnqIcp (DO_CVTGLS);
      AssignUnqIcp (DO_CVTGLSC);
      AssignUnqIcp (DO_CVTGLSV);
      AssignUnqIcp (DO_CVTGLSVC);
      AssignUnqIcp (DO_CVTGLV);
      AssignUnqIcp (DO_CVTGLVC);
      AssignUnqIcp (DO_CVTGQ);
      AssignUnqIcp (DO_CVTGQC);
      AssignUnqIcp (DO_CVTGQS);
      AssignUnqIcp (DO_CVTGQSC);
      AssignUnqIcp (DO_CVTGQSV);
      AssignUnqIcp (DO_CVTGQSVC);
      AssignUnqIcp (DO_CVTGQV);
      AssignUnqIcp (DO_CVTGQVC);
      AssignUnqIcp (DO_CVTLD);
      AssignUnqIcp (DO_CVTLDC);
      AssignUnqIcp (DO_CVTLDS);
      AssignUnqIcp (DO_CVTLDSC);
      AssignUnqIcp (DO_CVTLF);
      AssignUnqIcp (DO_CVTLFC);
      AssignUnqIcp (DO_CVTLFS);
      AssignUnqIcp (DO_CVTLFSC);
      AssignUnqIcp (DO_CVTLG);
      AssignUnqIcp (DO_CVTLGC);
      AssignUnqIcp (DO_CVTLGS);
      AssignUnqIcp (DO_CVTLGSC);
      AssignUnqIcp (DO_CVTLQ);
      AssignUnqIcp (DO_CVTLS);
      AssignUnqIcp (DO_CVTLSC);
      AssignUnqIcp (DO_CVTLSM);
      AssignUnqIcp (DO_CVTLSP);
      AssignUnqIcp (DO_CVTLSSU);
      AssignUnqIcp (DO_CVTLSSUC);
      AssignUnqIcp (DO_CVTLSSUI);
      AssignUnqIcp (DO_CVTLSSUIC);
      AssignUnqIcp (DO_CVTLSSUIM);
      AssignUnqIcp (DO_CVTLSSUIP);
      AssignUnqIcp (DO_CVTLSSUM);
      AssignUnqIcp (DO_CVTLSSUP);
      AssignUnqIcp (DO_CVTLT);
      AssignUnqIcp (DO_CVTLTC);
      AssignUnqIcp (DO_CVTLTM);
      AssignUnqIcp (DO_CVTLTP);
      AssignUnqIcp (DO_CVTLTSU);
      AssignUnqIcp (DO_CVTLTSUC);
      AssignUnqIcp (DO_CVTLTSUI);
      AssignUnqIcp (DO_CVTLTSUIC);
      AssignUnqIcp (DO_CVTLTSUIM);
      AssignUnqIcp (DO_CVTLTSUIP);
      AssignUnqIcp (DO_CVTLTSUM);
      AssignUnqIcp (DO_CVTLTSUP);
      AssignUnqIcp (DO_CVTQD);
      AssignUnqIcp (DO_CVTQDC);
      AssignUnqIcp (DO_CVTQDS);
      AssignUnqIcp (DO_CVTQDSC);
      AssignUnqIcp (DO_CVTQF);
      AssignUnqIcp (DO_CVTQFC);
      AssignUnqIcp (DO_CVTQFS);
      AssignUnqIcp (DO_CVTQFSC);
      AssignUnqIcp (DO_CVTQG);
      AssignUnqIcp (DO_CVTQGC);
      AssignUnqIcp (DO_CVTQGS);
      AssignUnqIcp (DO_CVTQGSC);
      AssignUnqIcp (DO_CVTQL);
      AssignUnqIcp (DO_CVTQLV);
      AssignUnqIcp (DO_CVTQLSV);
      AssignUnqIcp (DO_CVTQS);
      AssignUnqIcp (DO_CVTQSC);
      AssignUnqIcp (DO_CVTQSM);
      AssignUnqIcp (DO_CVTQSP);
      AssignUnqIcp (DO_CVTQSSU);
      AssignUnqIcp (DO_CVTQSSUC);
      AssignUnqIcp (DO_CVTQSSUI);
      AssignUnqIcp (DO_CVTQSSUIC);
      AssignUnqIcp (DO_CVTQSSUIM);
      AssignUnqIcp (DO_CVTQSSUIP);
      AssignUnqIcp (DO_CVTQSSUM);
      AssignUnqIcp (DO_CVTQSSUP);
      AssignUnqIcp (DO_CVTQT);
      AssignUnqIcp (DO_CVTQTC);
      AssignUnqIcp (DO_CVTQTM);
      AssignUnqIcp (DO_CVTQTP);
      AssignUnqIcp (DO_CVTQTSU);
      AssignUnqIcp (DO_CVTQTSUC);
      AssignUnqIcp (DO_CVTQTSUI);
      AssignUnqIcp (DO_CVTQTSUIC);
      AssignUnqIcp (DO_CVTQTSUIM);
      AssignUnqIcp (DO_CVTQTSUIP);
      AssignUnqIcp (DO_CVTQTSUM);
      AssignUnqIcp (DO_CVTQTSUP);
      AssignUnqIcp (DO_CVTSL);
      AssignUnqIcp (DO_CVTSLC);
      AssignUnqIcp (DO_CVTSLSV);
      AssignUnqIcp (DO_CVTSLSVC);
      AssignUnqIcp (DO_CVTSLSVI);
      AssignUnqIcp (DO_CVTSLSVIC);
      AssignUnqIcp (DO_CVTSLV);
      AssignUnqIcp (DO_CVTSLVC);
      AssignUnqIcp (DO_CVTSQ);
      AssignUnqIcp (DO_CVTSQC);
      AssignUnqIcp (DO_CVTSQSV);
      AssignUnqIcp (DO_CVTSQSVC);
      AssignUnqIcp (DO_CVTSQSVI);
      AssignUnqIcp (DO_CVTSQSVIC);
      AssignUnqIcp (DO_CVTSQV);
      AssignUnqIcp (DO_CVTSQVC);
      AssignUnqIcp (DO_CVTST);
      AssignUnqIcp (DO_CVTSTC);
      AssignUnqIcp (DO_CVTSTM);
      AssignUnqIcp (DO_CVTSTP);
      AssignUnqIcp (DO_CVTSTSU);
      AssignUnqIcp (DO_CVTSTSUC);
      AssignUnqIcp (DO_CVTSTSUI);
      AssignUnqIcp (DO_CVTSTSUIC);
      AssignUnqIcp (DO_CVTSTSUIM);
      AssignUnqIcp (DO_CVTSTSUIP);
      AssignUnqIcp (DO_CVTSTSUM);
      AssignUnqIcp (DO_CVTSTSUP);
      AssignUnqIcp (DO_CVTSTU);
      AssignUnqIcp (DO_CVTSTUC);
      AssignUnqIcp (DO_CVTSTUM);
      AssignUnqIcp (DO_CVTSTUP);
      AssignUnqIcp (DO_CVTTL);
      AssignUnqIcp (DO_CVTTLC);
      AssignUnqIcp (DO_CVTTLSV);
      AssignUnqIcp (DO_CVTTLSVC);
      AssignUnqIcp (DO_CVTTLSVI);
      AssignUnqIcp (DO_CVTTLSVIC);
      AssignUnqIcp (DO_CVTTLV);
      AssignUnqIcp (DO_CVTTLVC);
      AssignUnqIcp (DO_CVTTQ);
      AssignUnqIcp (DO_CVTTQC);
      AssignUnqIcp (DO_CVTTQSV);
      AssignUnqIcp (DO_CVTTQSVC);
      AssignUnqIcp (DO_CVTTQSVI);
      AssignUnqIcp (DO_CVTTQSVIC);
      AssignUnqIcp (DO_CVTTQV);
      AssignUnqIcp (DO_CVTTQVC);
      AssignUnqIcp (DO_CVTTS);
      AssignUnqIcp (DO_CVTTSC);
      AssignUnqIcp (DO_CVTTSM);
      AssignUnqIcp (DO_CVTTSP);
      AssignUnqIcp (DO_CVTTSSU);
      AssignUnqIcp (DO_CVTTSSUC);
      AssignUnqIcp (DO_CVTTSSUI);
      AssignUnqIcp (DO_CVTTSSUIC);
      AssignUnqIcp (DO_CVTTSSUIM);
      AssignUnqIcp (DO_CVTTSSUIP);
      AssignUnqIcp (DO_CVTTSSUM);
      AssignUnqIcp (DO_CVTTSSUP);
      AssignUnqIcp (DO_CVTTSU);
      AssignUnqIcp (DO_CVTTSUC);
      AssignUnqIcp (DO_CVTTSUM);
      AssignUnqIcp (DO_CVTTSUP);
      AssignUnqIcp (DO_CVTDG);
      AssignUnqIcp (DO_CVTDGC);
      AssignUnqIcp (DO_CVTDGU);
      AssignUnqIcp (DO_CVTDGUC);
      AssignUnqIcp (DO_CVTGD);
      AssignUnqIcp (DO_RD_FPCTL);
      AssignUnqIcp (DO_WR_FPCTL);
      
      /* SQRT e.g sqrtx fb,fc */
      free (icp);   /* RRC:  Free previous calloc of icp */         
      icp = (struct class_info *)calloc(1, sizeof(struct class_info));
      icp->arch = (struct architected *) calloc (1, sizeof (struct architected));
      
      icp->arch->op_ra = FALSE;
      icp->arch->op_rb = FALSE;			/* V3.24 */
      icp->arch->op_rc = FALSE;			/* V3.24 */
      icp->arch->ra_flt = TRUE;
      icp->arch->rb_flt = TRUE;
      icp->arch->rc_flt = TRUE;
      icp->arch->dest_reg = Rc;
      icp->arch->dest_flt = TRUE;
      icp->arch->chk_lit_mode = FALSE;
      
      AssignUnqIcp (DO_SQRTS);			/* V3.24 */
      AssignUnqIcp (DO_SQRTSC);			/* V3.24 */
      AssignUnqIcp (DO_SQRTSM);			/* V3.24 */
      AssignUnqIcp (DO_SQRTSD);			/* V3.24 */
      AssignUnqIcp (DO_SQRTSU);			/* V3.24 */
      AssignUnqIcp (DO_SQRTSUC);			/* V3.24 */
      AssignUnqIcp (DO_SQRTSUM);			/* V3.24 */
      AssignUnqIcp (DO_SQRTSUD);			/* V3.24 */
      AssignUnqIcp (DO_SQRTSSU);			/* V3.24 */
      AssignUnqIcp (DO_SQRTSSUC);			/* V3.24 */
      AssignUnqIcp (DO_SQRTSSUM);			/* V3.24 */
      AssignUnqIcp (DO_SQRTSSUD);			/* V3.24 */
      AssignUnqIcp (DO_SQRTSSUI);			/* V3.24 */
      AssignUnqIcp (DO_SQRTSSUIC);			/* V3.24 */
      AssignUnqIcp (DO_SQRTSSUIM);			/* V3.24 */
      AssignUnqIcp (DO_SQRTSSUID);			/* V3.24 */
      AssignUnqIcp (DO_SQRTT);			/* V3.24 */
      AssignUnqIcp (DO_SQRTTC);			/* V3.24 */
      AssignUnqIcp (DO_SQRTTM);			/* V3.24 */
      AssignUnqIcp (DO_SQRTTD);			/* V3.24 */
      AssignUnqIcp (DO_SQRTTU);			/* V3.24 */
      AssignUnqIcp (DO_SQRTTUC);			/* V3.24 */
      AssignUnqIcp (DO_SQRTTUM);			/* V3.24 */
      AssignUnqIcp (DO_SQRTTUD);			/* V3.24 */
      AssignUnqIcp (DO_SQRTTSU);			/* V3.24 */
      AssignUnqIcp (DO_SQRTTSUC);			/* V3.24 */
      AssignUnqIcp (DO_SQRTTSUM);			/* V3.24 */
      AssignUnqIcp (DO_SQRTTSUD);			/* V3.24 */
      AssignUnqIcp (DO_SQRTTSUI);			/* V3.24 */
      AssignUnqIcp (DO_SQRTTSUIC);			/* V3.24 */
      AssignUnqIcp (DO_SQRTTSUIM);			/* V3.24 */
      AssignUnqIcp (DO_SQRTTSUID);			/* V3.24 */
      
      /* LOAD ADDRESS - uses IALU */
      free (icp);   /* RRC:  Free previous calloc of icp */   
      icp = (struct class_info *)calloc(1, sizeof(struct class_info));
      icp->arch = (struct architected *) calloc (1, sizeof (struct architected));
      
      icp->arch->op_ra = TRUE;		/* V3.24 */
      icp->arch->op_rb = TRUE;		
      icp->arch->op_rc = FALSE;
      icp->arch->ra_flt = FALSE;
      icp->arch->rb_flt = FALSE;
      icp->arch->rc_flt = FALSE;                            
      icp->arch->dest_reg = Ra;
      icp->arch->dest_flt = FALSE;
      icp->arch->chk_lit_mode = FALSE;
      
      AssignUnqIcp (DO_LDAH);
      AssignUnqIcp (DO_LDAL);
      AssignUnqIcp (DO_LDALH);
      AssignUnqIcp (DO_LDAQ);
      
      /* HW_MFPR - to match EV3 assume same IBUFF as integer stores  */
      free (icp);   /* RRC:  Free previous calloc of icp */         
      icp =  (struct class_info *)calloc(1, sizeof(struct class_info));
      icp->arch = (struct architected *) calloc (1, sizeof (struct architected));
      	
      icp->arch->op_ra = TRUE;		/* changed for V3.24 */
      icp->arch->op_rb = TRUE;		/* changed for V3.24 */
      icp->arch->op_rc = FALSE;
      icp->arch->ra_flt = FALSE;
      icp->arch->rb_flt = FALSE;
      icp->arch->rc_flt = FALSE;
      icp->arch->dest_reg = Ra;
      icp->arch->dest_flt = FALSE;
      icp->arch->chk_lit_mode = FALSE;
      
      AssignUnqIcp (DO_HW_MFPR);
      AssignUnqIcp (DO_RCC);
      AssignUnqIcp (DO_RC);
      AssignUnqIcp (DO_RS);
      
      /* HW_MTPR - to match Ev3 assume same IBUFF as integer stores */
      free (icp);   /* RRC:  Free previous calloc of icp */         
      icp =  (struct class_info *)calloc(1, sizeof(struct class_info));
      icp->arch = (struct architected *) calloc (1, sizeof (struct architected));
      
      icp->arch->op_ra = TRUE;
      icp->arch->op_rb = TRUE;		/* changed for V3.24 EV3/4: Ra & Rb must be identical */
      icp->arch->op_rc = FALSE;
      icp->arch->ra_flt = FALSE;
      icp->arch->rb_flt = FALSE;
      icp->arch->rc_flt = FALSE;
      icp->arch->dest_reg = Noreg;
      icp->arch->dest_flt = FALSE;
      icp->arch->chk_lit_mode = FALSE;
      
      AssignUnqIcp (DO_HW_MTPR);
      
      /* Create an instruction class record for integer memory loads, and
	 fill in the blanks */
      free (icp);   /* RRC:  Free previous calloc of icp */         
      icp = (struct class_info *)calloc(1, sizeof(struct class_info));
      icp->arch = (struct architected *) calloc (1, sizeof (struct architected));
      
      icp->arch->op_ra = TRUE;			/* V3.24 */
      icp->arch->op_rb = TRUE;			
      icp->arch->op_rc = FALSE;
      icp->arch->ra_flt = FALSE;
      icp->arch->rb_flt = FALSE;
      icp->arch->rc_flt = FALSE;
      icp->arch->dest_reg = Ra;
      icp->arch->dest_flt = FALSE;
      icp->arch->chk_lit_mode = FALSE;
      
      AssignUnqIcp (DO_HW_LD);
      AssignUnqIcp (DO_LDL);
      AssignUnqIcp (DO_LDB);			/* V3.24 */
      AssignUnqIcp (DO_LDW);			/* V3.24 */
      AssignUnqIcp (DO_LDLL);
      AssignUnqIcp (DO_LDQ);
      AssignUnqIcp (DO_LDQL);
      AssignUnqIcp (DO_LDQU);
      
      /* Now do integer stores */
      free (icp);   /* RRC:  Free previous calloc of icp */         
      icp = (struct class_info *)calloc(1, sizeof(struct class_info));
      icp->arch = (struct architected *) calloc (1, sizeof (struct architected));
      
      icp->arch->op_ra = TRUE;
      icp->arch->op_rb = TRUE;
      icp->arch->op_rc = FALSE;
      icp->arch->ra_flt = FALSE;
      icp->arch->rb_flt = FALSE;
      icp->arch->rc_flt = FALSE;
      icp->arch->dest_reg = Noreg;
      icp->arch->dest_flt = FALSE;
      icp->arch->chk_lit_mode = FALSE;
      
      AssignUnqIcp (DO_HW_ST);
      AssignUnqIcp (DO_STL);
      AssignUnqIcp (DO_STB);			/* V3.24 */
      AssignUnqIcp (DO_STW);			/* V3.24 */
      AssignUnqIcp (DO_STLC);
      AssignUnqIcp (DO_STQ);
      AssignUnqIcp (DO_STQC);
      AssignUnqIcp (DO_STQU);
      
      /* WH64 AND ECB memory format  e.g. ECB (rb.ab) */
      free (icp);   /* RRC:  Free previous calloc of icp */         
      icp = (struct class_info *)calloc(1, sizeof(struct class_info));
      icp->arch = (struct architected *) calloc (1, sizeof (struct architected));
      
      icp->arch->op_ra = FALSE;			/* V3.24 */
      icp->arch->op_rb = TRUE;
      icp->arch->op_rc = FALSE;
      icp->arch->ra_flt = FALSE;
      icp->arch->rb_flt = FALSE;
      icp->arch->rc_flt = FALSE;
      icp->arch->dest_reg = Noreg;
      icp->arch->dest_flt = FALSE;
      icp->arch->chk_lit_mode = FALSE;
      
      AssignUnqIcp (DO_WH64);			 /* V3.24 */
      AssignUnqIcp (DO_ECB);			 /* V3.24 */

      /* Next do floating point loads */
      free (icp);   /* RRC:  Free previous calloc of icp */         
      icp = (struct class_info *)calloc(1, sizeof(struct class_info));
      icp->arch = (struct architected *) calloc (1, sizeof (struct architected));
      
      icp->arch->op_ra = FALSE;		
      icp->arch->op_rb = FALSE;		/* V3.24 */
      icp->arch->op_rc = FALSE;
      icp->arch->ra_flt = TRUE;
      icp->arch->rb_flt = TRUE;
      icp->arch->rc_flt = FALSE;
      icp->arch->dest_reg = Ra;
      icp->arch->dest_flt = TRUE;
      icp->arch->chk_lit_mode = FALSE;
      
      AssignUnqIcp (DO_LDS);
      AssignUnqIcp (DO_LDT);
      AssignUnqIcp (DO_LDD);
      AssignUnqIcp (DO_LDF);
      
      /* Next, floating point stores */
      free (icp);   /* RRC:  Free previous calloc of icp */         
      icp = (struct class_info *)calloc(1, sizeof(struct class_info));
      icp->arch = (struct architected *) calloc (1, sizeof (struct architected));
      
      icp->arch->op_ra = FALSE;
      icp->arch->op_rb = FALSE;
      icp->arch->op_rc = FALSE;
      icp->arch->ra_flt = TRUE;
      icp->arch->rb_flt = TRUE;
      icp->arch->rc_flt = FALSE;
      icp->arch->dest_reg = Noreg;
      icp->arch->dest_flt = TRUE;
      icp->arch->chk_lit_mode = FALSE;
      
      AssignUnqIcp (DO_STD);
      AssignUnqIcp (DO_STF);
      AssignUnqIcp (DO_STS);
      AssignUnqIcp (DO_STT);
      
      /* Next, SWAP */
      free (icp);   /* RRC:  Free previous calloc of icp */         
      icp =  (struct class_info *)calloc(1, sizeof(struct class_info));
      icp->arch = (struct architected *) calloc (1, sizeof (struct architected));
      
      icp->arch->op_ra = TRUE;
      icp->arch->op_rb = TRUE;
      icp->arch->op_rc = FALSE;
      icp->arch->ra_flt = FALSE;
      icp->arch->rb_flt = FALSE;
      icp->arch->rc_flt = FALSE;
      icp->arch->dest_reg = Ra;
      icp->arch->dest_flt = FALSE;
      icp->arch->chk_lit_mode = FALSE;

      free(icp->arch); /* RRC:  Looks like this instruction is not used.  Free the space. */      
      /* One Integer operand conditional branches */
      free (icp);   /* RRC:  Free previous calloc of icp */         
      icp =  (struct class_info *)calloc(1, sizeof(struct class_info));
      icp->arch = (struct architected *) calloc (1, sizeof (struct architected));
      
      icp->arch->op_ra = TRUE;
      icp->arch->op_rb = FALSE;
      icp->arch->op_rc = FALSE;
      icp->arch->ra_flt = FALSE;
      icp->arch->rb_flt = FALSE;
      icp->arch->rc_flt = FALSE;
      icp->arch->dest_reg = Noreg;
      icp->arch->dest_flt = FALSE;
      icp->arch->chk_lit_mode = FALSE;
      
      AssignUnqIcp (DO_BEQ);
      AssignUnqIcp (DO_BGE);
      AssignUnqIcp (DO_BGT);
      AssignUnqIcp (DO_BLBC);
      AssignUnqIcp (DO_BLBS);
      AssignUnqIcp (DO_BLE);
      AssignUnqIcp (DO_BLT);
      AssignUnqIcp (DO_BNE);
      
      /* one float operand conditional branches */
      free (icp);   /* RRC:  Free previous calloc of icp */         
      icp =  (struct class_info *) calloc(1, sizeof(struct class_info));
      icp->arch = (struct architected *) calloc (1, sizeof (struct architected));
      
      icp->arch->op_ra = FALSE;		/* V3.24 */
      icp->arch->op_rb = FALSE;
      icp->arch->op_rc = FALSE;
      icp->arch->ra_flt = TRUE;
      icp->arch->rb_flt = FALSE;
      icp->arch->rc_flt = FALSE;
      icp->arch->dest_reg = Noreg;
      icp->arch->dest_flt = FALSE;
      icp->arch->chk_lit_mode = FALSE;
      
      AssignUnqIcp (DO_BFEQ);
      AssignUnqIcp (DO_BFGE);
      AssignUnqIcp (DO_BFGT);
      AssignUnqIcp (DO_BFLE);
      AssignUnqIcp (DO_BFLT);
      AssignUnqIcp (DO_BFNE);
      AssignUnqIcp (DO_FLBC);
      
      /* unconditional branch with dest reg - BSR, BR */
      free (icp);   /* RRC:  Free previous calloc of icp */         
      icp =  (struct class_info *)calloc(1, sizeof(struct class_info));
      icp->arch = (struct architected *) calloc (1, sizeof (struct architected));
      
      icp->arch->op_ra = TRUE;		/* V3.24 */
      icp->arch->op_rb = FALSE;
      icp->arch->op_rc = FALSE;
      icp->arch->ra_flt = FALSE;
      icp->arch->rb_flt = FALSE;
      icp->arch->rc_flt = FALSE;
      icp->arch->dest_reg = Ra;
      icp->arch->dest_flt = FALSE;
      icp->arch->chk_lit_mode = FALSE;
      
      AssignUnqIcp (DO_BR);
      AssignUnqIcp (DO_BSR);
      
      /* Memory format JSR */
      free (icp);   /* RRC:  Free previous calloc of icp */         
      icp =  (struct class_info *)calloc(1, sizeof(struct class_info));
      icp->arch = (struct architected *) calloc (1, sizeof (struct architected));
      
      icp->arch->op_ra = TRUE;		/* V3.24 */
      icp->arch->op_rb = TRUE;
      icp->arch->op_rc = FALSE;
      icp->arch->ra_flt = FALSE;
      icp->arch->rb_flt = FALSE;
      icp->arch->rc_flt = FALSE;
      icp->arch->dest_reg = Ra;
      icp->arch->dest_flt = FALSE;
      icp->arch->chk_lit_mode = FALSE;
      
      AssignUnqIcp (DO_JSR);
      
      
      /* CALL_PAL - to match EV3, assume we use the same IBUFF as integer 
	 stores  */
      free (icp);   /* RRC:  Free previous calloc of icp */         
      icp =  (struct class_info *)calloc(1, sizeof(struct class_info));
      icp->arch = (struct architected *) calloc (1, sizeof (struct architected));
      
      icp->arch->op_ra = FALSE;		
      icp->arch->op_rb = FALSE;		
      icp->arch->op_rc = FALSE;
      icp->arch->ra_flt = FALSE;
      icp->arch->rb_flt = FALSE;
      icp->arch->rc_flt = FALSE;
      icp->arch->dest_reg = Noreg;
      icp->arch->dest_flt = FALSE;
      icp->arch->chk_lit_mode = FALSE;
      
      AssignUnqIcp (DO_EPICODE);		/* V3.24 aka CALL_PAL */
      
      /* HW_REI:  This looks just like a zero operand integer branch */
      free (icp);   /* RRC:  Free previous calloc of icp */         
      icp =  (struct class_info *)calloc(1, sizeof(struct class_info));
      icp->arch = (struct architected *) calloc (1, sizeof (struct architected));
      
      icp->arch->op_ra = TRUE;			/* TRUE but always =r31 */
      icp->arch->op_rb = TRUE;			/* V3.24 */
      icp->arch->op_rc = FALSE;
      icp->arch->ra_flt = FALSE;
      icp->arch->rb_flt = FALSE;
      icp->arch->rc_flt = FALSE;
      icp->arch->dest_reg = Noreg;
      icp->arch->dest_flt = FALSE;
      icp->arch->chk_lit_mode = FALSE;
      
      AssignUnqIcp (DO_HW_REI);
      
      /*  DRAIN - to match EV3 assume same IBUFF as integer stores */
      free (icp);   /* RRC:  Free previous calloc of icp */         
      icp =  (struct class_info *)calloc(1, sizeof(struct class_info));
      icp->arch = (struct architected *) calloc (1, sizeof (struct architected));
      
      icp->arch->op_ra = FALSE;
      icp->arch->op_rb = FALSE;
      icp->arch->op_rc = FALSE;
      icp->arch->ra_flt = FALSE;
      icp->arch->rb_flt = FALSE;
      icp->arch->rc_flt = FALSE;
      icp->arch->dest_reg = Noreg;
      icp->arch->dest_flt = FALSE;
      icp->arch->chk_lit_mode = FALSE;
      
      AssignUnqIcp (DO_DRAINT);
      
      
      /* Next, NUDGE  */
      free (icp);   /* RRC:  Free previous calloc of icp */         
      icp =  (struct class_info *)calloc(1, sizeof(struct class_info));
      icp->arch = (struct architected *) calloc (1, sizeof (struct architected));
      
      icp->arch->op_ra = FALSE;
      icp->arch->op_rb = FALSE;
      icp->arch->op_rc = FALSE;
      icp->arch->ra_flt = FALSE;
      icp->arch->rb_flt = FALSE;
      icp->arch->rc_flt = FALSE;
      icp->arch->dest_reg = Noreg;
      icp->arch->dest_flt = FALSE;
      icp->arch->chk_lit_mode = FALSE;
      
      AssignUnqIcp (DO_NUDGE);
      
      /* Next, MB*/
      free (icp);   /* RRC:  Free previous calloc of icp */         
      icp =  (struct class_info *)calloc(1, sizeof(struct class_info));
      icp->arch = (struct architected *) calloc (1, sizeof (struct architected));
      
      icp->arch->op_ra = FALSE;
      icp->arch->op_rb = FALSE;
      icp->arch->op_rc = FALSE;
      icp->arch->ra_flt = FALSE;
      icp->arch->rb_flt = FALSE;
      icp->arch->rc_flt = FALSE;
      icp->arch->dest_reg = Noreg;
      icp->arch->dest_flt = FALSE;
      icp->arch->chk_lit_mode = FALSE;
      
      AssignUnqIcp (DO_MB);
      
      /* Next, FETCH, FETCH_M */
      free (icp);   /* RRC:  Free previous calloc of icp */         
      icp =  (struct class_info *)calloc(1, sizeof(struct class_info));
      icp->arch = (struct architected *) calloc (1, sizeof (struct architected));
      
      icp->arch->op_ra = FALSE;
      icp->arch->op_rb = TRUE;
      icp->arch->op_rc = FALSE;
      icp->arch->ra_flt = FALSE;
      icp->arch->rb_flt = FALSE;
      icp->arch->rc_flt = FALSE;
      icp->arch->dest_reg = Noreg;
      icp->arch->dest_flt = FALSE;
      icp->arch->chk_lit_mode = FALSE;
      
      AssignUnqIcp (DO_FETCH);
      AssignUnqIcp (DO_FETCH_M);
      free (icp);   /* RRC:  Free previous calloc of icp */
}

#ifdef RRC_COMPILE_FLOW_CODE  
void print_connect_line()
/*
 *******************************************************************************
 *	PURPOSE:
 *		Prints connecting lines with stack length and bsr hash value.
 *	AUTHOR/DATE:
 *		Rogelio R. Cruz 7/28/95
 ********************************************************************************
 */
{ /* RRC:  Added this code for flow display */
  char temp[PAGE_WIDTH+20];
  center("|");
  wr(FLG$FLOW, "\n");
  sprintf (temp, "L:%d,H:%d", length_L(bsr_stack), current_addr_hash);
  center(temp);
  wr(FLG$FLOW, "\n");
  center("|");
  wr(FLG$FLOW, "\n");
}

void print_current_instruction(NODE *current, char *box_str)
/*
 *******************************************************************************
 *	PURPOSE:
 *		Prints the current instruction inside a centered box.
 *	AUTHOR/DATE:
 *		Rogelio R. Cruz 7/28/95
 ********************************************************************************
 */
{ /* RRC:  Added this code for flow display */
  char temp[PAGE_WIDTH+20];
  sprintf (temp, "%s 0x%X: %s %s", box_str, current->instruction->address, current->instruction->decoded, box_str);
  center(lineof (strlen (temp), box_str));
  wr(FLG$FLOW, "\n");
  center(temp);
  wr(FLG$FLOW, "\n");
  center(lineof (strlen (temp), box_str));
  wr(FLG$FLOW, "\n");
}

void print_message_in_box(char *message, char *box_str)
/*
 *******************************************************************************
 *	PURPOSE:
 *		Prints the current instruction inside a centered box.
 *	AUTHOR/DATE:
 *		Rogelio R. Cruz 7/28/95
 ********************************************************************************
 */
{ /* RRC:  Added this code for flow display */
  char temp[PAGE_WIDTH+20];
  sprintf (temp, "%s %s %s", box_str, message, box_str);
  center(lineof (strlen (temp), box_str));
  wr(FLG$FLOW, "\n");
  center(temp);
  wr(FLG$FLOW, "\n");
  center(lineof (strlen (temp), box_str));
  wr(FLG$FLOW, "\n");
}


int print_current_instruction_with_arrow(NODE *current, char *box_str, int node)
/*
 *******************************************************************************
 *	PURPOSE:
 *		Prints the current instruction inside a centered box with arrow
 *              coming out of the right side.
 *	 OUTPUT PARAMETERS:
 *              Returns the current RRC_node.
 *	AUTHOR/DATE:
 *		Rogelio R. Cruz 7/28/95
 ********************************************************************************
 */
{ /* RRC:  Added this code for flow display */
  char temp[PAGE_WIDTH+20];
  char temp2[PAGE_WIDTH+20];

  sprintf (temp, "%s 0x%X: %s %s", box_str, current->instruction->address, current->instruction->decoded, box_str);
  center(lineof (strlen (temp), box_str));
  wr(FLG$FLOW, "\n");
  center(temp);

  sprintf (temp2, "========> [%d]", node);
  wr(FLG$FLOW, "%s\n", temp2);

  center(lineof (strlen (temp), box_str));
  wr(FLG$FLOW, "\n");
  return (node);
}
char temp[200];
char *lineof (int n, char *str)
/*
 ********************************************************************************
 *	PURPOSE: 
 *	      Prints the character specified N times.
 *	INPUT PARAMETERS:
 *	      n: Number of times to concatenate string
 *            val: string to be repeated.
 *	OUTPUT PARAMETERS:
 *            pointer to created string.
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Rogelio R. Cruz, 7/27/95
 ********************************************************************************
 */
{
  int i;
  temp[0]='\0';
  for (i=0; i < n; i++) strcat(temp, str);
}

int center (char * str)
{
  int width = PAGE_WIDTH;
  int center;

  center = width/2;
  wr (FLG$FLOW, "%*.*s", center + strlen(str)/2, width, str);
  return(center);
}


#endif /* RRC_COMPILE_FLOW_CODE */


void destroy_tree(NODE *root) 
/*
 ********************************************************************************
 *	PURPOSE: 
 *	      Frees up space taken up by a tree of NODES
 *	INPUT PARAMETERS:
 *	      root: Place in the tree where we should begin.
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Rogelio R. Cruz, 8/2/95
 ********************************************************************************
 */
{
NODE *temp;

   while (root){
     temp = root->inline;
     free(root);
     root = temp;
   }
}



