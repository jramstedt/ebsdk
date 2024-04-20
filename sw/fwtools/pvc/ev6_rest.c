/* Implement the ev6 processor specific RESTRICTIONS  */
/* #define guideline_6  !define to include check_ev6_guideline_6, which is incomplete */
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
 *	  V3.24 fls Nov-96 - extracted from ev6.c
 */

#include "types.h"
#include "ispdef.h"
#include "rest.h"
#include "sched.h"
#include "memory.h"
#include "ev6.h"
#include "body.h"
#include "alpha_op.h"
#include "opcode.h"

static int restriction_num;			/*V3.24 restriction number used in pvc ignore labels   */

void check_ev6_restriction_2  (INSTR *[], int, int *, int, int);	/*V3.24*/
void check_ev6_restriction_4  (INSTR *[], int, int *, int, int);	/*V3.24*/
void check_ev6_restriction_12 (INSTR *[], int, int *, int, int);	/*V3.24*/
void check_ev6_restriction_14 (INSTR *[], int, int *, int, int);	/*V3.24*/
void check_ev6_restriction_18 (INSTR *[], int, int *, int, int);	/*V3.24*/
void check_ev6_guideline_26   (INSTR *[], int, int *, int, int);	/*V3.24*/

void check_ev6_guideline_11(INSTR *[], int, List *); 				/*V3.24*/
void check_ev6_guideline_6  (INSTR *[], int, List *); 				/*V3.24*/
void check_ev6_restriction_7(INSTR *[], int, List *); 				/*V3.24*/
void check_ev6_restriction_17(INSTR *[], int, List *); 				/*V3.24*/
void check_ev6_restriction_19(INSTR *[], int, List *); 				/*V3.24*/
void check_ev6_restriction_21(INSTR *[], int, List *); 				/*V3.24*/
void check_ev6_restriction_21a(INSTR *[], int, List *); 				/*V3.24*/
void check_ev6_restriction_21b(INSTR *[], int, List *); 				/*V3.24*/
void check_ev6_restriction_22(INSTR *[], int, List *); 				/*V3.24*/
void check_ev6_restriction_22a(INSTR *[], int, List *); 				/*V3.24*/
void check_ev6_restriction_22b(INSTR *[], int, List *); 				/*V3.24*/
void check_ev6_restriction_23(INSTR *[], int, List *); 				/*V3.24*/
void check_ev6_restriction_24a(INSTR *[], int, List *); 				/*V3.24*/
void check_ev6_restriction_24b(INSTR *[], int, List *); 				/*V3.24*/
void check_ev6_restriction_24c(INSTR *[], int, List *); 				/*V3.24*/
void check_ev6_restriction_27(INSTR *[], int, List *); 				/*V3.24*/
void check_ev6_restriction_28a(INSTR *[], int, List *); 				/*V3.24*/
void check_ev6_restriction_28b(INSTR *[], int, List *); 				/*V3.24*/
void check_ev6_restriction_28c(INSTR *[], int, List *); 				/*V3.24*/
void check_ev6_restriction_28d(INSTR *[], int, List *); 				/*V3.24*/
void check_ev6_guideline_29 (INSTR *[], int, List *); 				/*V3.24*/
void check_ev6_restriction_30 (INSTR *[], int, List *); 				/*V3.24*/
void check_ev6_restriction_33 (INSTR *[], int, int *, int, int);
void check_ev6_restriction_34 (INSTR *[], int, List *); 				/*V3.24*/

extern int  wr (int, char *, ...); 	/* V3.25 */
extern int force_fail_lock_ev6;						/* in EV6.C for restriction_27*/
extern int first_fetch_block_ev6;					/* in EV6.C */
extern int 	delay, interrupt;
extern int pal_warning_count;
extern int pal_error_count;
extern List 	*restrictions[];
extern List 	*caveat_list;		/*V3.24*/
extern ERR_MSG	*message;		/*V3.24*/
extern INSTR	*exe;			/*V3.24*/
extern CYCLE	*runtime[];
extern CLASS_INFO *instr_array[];
extern char 	pheader[];
extern ERR_MSG *search_ev6_caveat(int, List *, int);
extern ERR_MSG *search_addr_caveat(int, List *, int);

extern int get_inst_id (INSTR *);						/* in ev6.c */
extern int get_ipr_id (int);							/* in ev6.c */
extern int check_mfmt_reg_ev6(INSTR *[], int, int, int, int); 			/* in ev6.c */



void check_ev6_restriction_2 (INSTR *raw_code[], int offs, int *id, int poss, int count)
/*
 ********************************************************************************
 *	PURPOSE:
 *		Check EV6 restriction #2
 * (2) Restriction: No Multiple Writers to IPRs in Same Scoreboard group
 * 
 *    Only one explicit writer (HW_MTPR) to IPRs that are in the same group can
 *    appear in the same fetch block (octaword-aligned octaword). Multiple
 *    explicit writers to IPRs that are NOT in the same scoreboard group can
 *    appear. If this restriction is violated the IPR readers might not see the
 *    in-order state. Also, the IPR might ultimately end up with a bad value. 
 *    This is for convenience of implementation. 
 * 
 *	INPUT PARAMETERS:
 *		raw_code: pointer to the array of instructions from follow_code
 *		offs: where we are in the above array
 *		id:   array of ipr info such as scoreboard bits
 *		count: Number of Longwords in fetch block. Max = 4.
 *	OUTPUT PARAMETERS:
 *		None
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:            
 *	AUTHOR/DATE:
 *	V3.24 FLS Aug 96 
 ********************************************************************************
 */
{
      int i, index[4], t[4];
      for (i=0; i<4; i++) t[i]=0;
      for (i=0; i<count; i++) {				
      	    index[poss+i] = raw_code[offs+i]->index;						/* get unique inst. index */
            if (index[poss+i] == DO_HW_MTPR) 	
		    t[i] = id[i] & (Scbd7|Scbd6|Scbd5|Scbd4|Scbd3|Scbd2|Scbd1|Scbd0);	/* get ipr scoreboard bits */
      }

	/* if any two ids contain common scoreboard bit then error */
	if ( (t[0] & t[1]) || (t[0] & t[2]) || (t[0] & t[3])  ||
	     (t[1] & t[2]) || (t[1] & t[3]) || (t[2] & t[3]) != 0) {

	    restriction_num= 2;
	    if (new_error (raw_code[offs]->address, restriction_num)) {
	     	wr (FLG$ERR, "%s\n", pheader);
	     	wr (FLG$ERR, "***\n");

	        for (i=poss; i<count; i++) {
	    	    wr (FLG$ERR, "Error at %X: %s\n", 
			 raw_code[offs+i]->address,raw_code[offs+i]->decoded);
		}
	    	wr (FLG$ERR, "(PVC #%d) RESTRICTION #2: \n",restriction_num);
	        wr (FLG$ERR, "          Only one explicit writer (Hw_mtpr) to IPRs that have any common\n");
 	    	wr (FLG$ERR, "          scoreboard bits can be in the same fetch block. \n");
	    	wr (FLG$ERR, "          Suggestion: Add NOPs to ensure the offending Hw_mtpr instructions \n");
	    	wr (FLG$ERR, "                      are in different fetch blocks (octaword-aligned octaword).\n***\n");
	        pal_error_count++;
	    }
	}
}

void check_ev6_restriction_4 (INSTR *raw_code[], int offs, int *id, int poss, int count)
/*
 ********************************************************************************
 *	PURPOSE:
 *		Check EV6 restriction #4
 *
 * (4) Restriction: No Writers and Readers to IPRs in Same Scoreboard
 * Group
 *
 *   Within one fetch block (octaword-aligned octaword), an implicit or explicit
 *   reader of an IPR in a particular Scoreboard Group an not follow an explicit
 *   writer (HW_MTPR) to an IPR in that scoreboard group. This is for
 *   convenience of implementation. 
 *   Note that implicit readers include all
 *   memory operations, JMP of all varieties, and HW_RET instructions which do
 *   not have the 'stall' bit set. 
 *   
 *   Dstream Memory operations include LDx, Stx, HW_LD, HW_ST.
 *   				HW_LD,HW_ST (virtual mode affects dtb)
 *   Istream Memory operations include JSR, HW_RET.
 *
 *   Clarification from Ed M. (edited by FLS)
 *    Any mem op (ld,st types) will implicitly 
 *    read the dtb, so you cannot allow an explicit dtb writer (mtpr) in the 
 *    same fetch block as a ld/st.  The itb has a similar restriction, but 
 *    would require that no virtual istream reference to the itb can occur 
 *    in the same fetch block.  Now this generally can't happen because an 
 *    mtpr itb and a virtual ref isn't going to be in the same fetch block 
 *    unless the mtpr is enabled with the  hw_ena bit that allows 
 *    palmode instr in native mode.  The normal way to exit palcode is via a 
 *    hw_ret which by definition will not get to native mode til the next fetch.
 *
 *  Concise explanation:
 *  
 *  The problem is the scoreboard bit logic in the map stage which cannot 
 *  detect and stall on a scoreboard bit in the same cycle that it is being 
 *  set.  Since a reader (explicit or implicit) within a scoreboard group 
 *  relies on the map stall to insure synchronization with previous 
 *  reader/writers. you can't do an mtpr and mfpr(or other implicit read) 
 *  within the same scoreboard group and expect the reader to stall properly. 
 *
 * Memo from Ed Mclellan explaining why hw_ret_stall is ok while hw_ret is not.
 *
 * From: CRL::"ejm@digital.com" "Edward J. McLellan" 18-SEP-1996 10:45:41.48
 * 
 * Dan and I discussed this and decided that the best action is to 
 * add a caveat to exclude hw_ret_stall from this restriction.  This 
 * releases the itb case.  The tb_mb version of itb fill does not 
 * affect this issue.  Dan is going to reword the restriction. 
 * 
 * We can't recall a specific failure due to a mt with either jsr or 
 * hw_ret in the same fetch block, but since it doesn't seem to be 
 * a hardship (I assume you agree), we should leave that much of the 
 * restriction intact.  
 * 
 *	INPUT PARAMETERS:
 *		raw_code: pointer to the array of instructions from follow_code
 *		offs: where we are in the above array
 *		id:   array of ipr info such as scoreboard bits
 *		count: Number of Longwords in fetch block. Max = 4.
 *		       count is expected to be > 1.
 *	OUTPUT PARAMETERS:
 *		None
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:            
 *	AUTHOR/DATE:
 *	V3.24 FLS Aug 96 
 ********************************************************************************
 */
{
    int i, j, k,index[4], t[4], scbd_mask;
    int found_mt = FALSE,  error= FALSE;
    int idtmp, ipr_num;
    scbd_mask = (Scbd7|Scbd6|Scbd5|Scbd4|Scbd3|Scbd2|Scbd1|Scbd0);

for (i=0; i<count; i++) index[i]=0;		/* clear indexes */

    /* check hw_mtpr followed by hw_mfpr or jsr or hw_ret or memory ops.  */
    /* if hw_mtpr IPR uses scbd bits <3:0> (dtb or itb_pte) then check
	for hw_mfpr and  
	LDx/STx which can be IMPLICIT readers of DTB.
	JSR/HW_RET can be IMPLICIT readers of itb_pte.
	If scbd0 then must further check that it is dtb or itb_pte. It
	is okay for dtb writes to be follwed by hw_ret without stall.
	Hw_ret_stall is okay in both cases. 
       else just check for hw_mfpr with same scoreboard bits.
       if a /hw_ret_stall /=hw_jsr_stall follows MTpr then 
       it is okay since the stall will retire instructions in order
       and wait for the mtpr then refetch the following instructions.
	NOTE: HW_RET Read/Write bits are set in get_inst_id routine.
    */

      /* store this fetch block's index into index[] array. Note that
       * if poss is not 0 then the fetch block is less than 4 Longwords
       * and therefore index[0] will not contain fetch_block[0].
       */
      for (i=0; i<count; i++) {				
      	    index[i] = raw_code[offs+i]->index;						/* get unique inst. index */
            if (index[i] == DO_HW_MTPR) 						        /* check if MT in any cache line */
		found_mt = TRUE;
      }

      if (found_mt == TRUE) {
       for (j=0; j<count; j++) {	
         for (i=0; i<4; i++) t[i]=0;		/* init temp id bits */
         if (index[j] == DO_HW_MTPR) {	
            t[j] = id[j] & scbd_mask;
	    ipr_num = raw_code[offs+j]->bits.ev6_hwmxpr_format.index;	/* get hw_mtpr ipr */
	    idtmp = get_ipr_id (ipr_num) ;				/* get Ibox_ipr or Mbox_ipr bits */
	    idtmp |= raw_code[offs+j]->bits.ev6_hwmxpr_format.scbd ; 	/* add in instr. scoreboard bits*/
	   if (j+1 < count) {
             for (i=j+1; i<count; i++) {	

		 if ((id[i] & Read) && (id[i] & Virtual_inst)) /* check HW_RET, HW_LDx, and LDx memory op */
		       t[i] = (Scbd3|Scbd2|Scbd1|Scbd0);      	/* simulate scbd<3:0> bits*/
	         if ((id[i] & Write) && (id[i] & Virtual_inst)) /* check HW_RET, HW_STx and STx memory op*/
		       t[i] = (Scbd3|Scbd2|Scbd1|Scbd0);     	/* simulate scbd<3:0> bits*/

                 if (index[i] == DO_HW_MFPR)
                     t[i] = id[i] & scbd_mask;
	         else if (index[i] == DO_JSR) 	{			/* JSR/JMP/RET/JSR_COR  only affect itb */
			     if (idtmp & Ibox_ipr)
		       		   t[i] = idtmp; 			/* set t[i]= mtpr ipr scbd to force error*/
			     else  t[i]=0;				/* else not violation */
		         }
                 else if (id[i] & Hwret_stall) {		/* if hw_jmp_stall or hw_ret_stall/ hw_jsr_stall/hw_coro_stall */
			     if (idtmp & Ibox_ipr) 
		       		 for (k=j+1; k<count; k++) t[k] = 0; /* else no violation prior to stall*/
				  break;
		      }
	         else if (index[i] == DO_HW_RET) 	{			/* hw_ret only affects itb */
			 t[i]=0;				/*assume not violation */
			 if (raw_code[offs+i]->bits.ev6_hwret_format.hint !=0) { /* if not HW_JMP form of HW_RET */
			     if (idtmp & Ibox_ipr)
		       		   t[i] = idtmp; 		/* set t[i]= mtpr ipr scbd to force error*/
			 }
		      }
	     }
	     /* if any two ids contain common scoreboard bit then error */
	     if ( (t[0] & t[1]) || (t[0] & t[2]) || (t[0] & t[3])  ||
	         (t[1] & t[2]) || (t[1] & t[3]) || (t[2] & t[3]) != 0)
	              error = TRUE;
	   } /* if (j+1 < count) */
         } /* if (index[j] == DO_HW_MTPR) */
       } /* for (j=i; j<count; j++) */
      } /*n if (found_mt == TRUE) */

	if (error == TRUE) {
	    restriction_num= 4;
	    if (new_error (raw_code[offs]->address, restriction_num)) {
	     	wr (FLG$ERR, "%s\n", pheader);
	     	wr (FLG$ERR, "***\n");

	        for (i=0; i<count; i++) {
	    	    wr (FLG$ERR, "Error at %X: %s\n", 
			 raw_code[offs+i]->address,raw_code[offs+i]->decoded);
		}
	    	wr (FLG$ERR, "(PVC #%d) RESTRICTION #4: \n",restriction_num);
	        wr (FLG$ERR, "          No writers and readers to IPRs in the same scoreboard group.\n");
	    	wr (FLG$ERR, "          Within one fetch block an implicit or explicit reader of an IPR\n");
	    	wr (FLG$ERR, "          can not follow an explicit writer (Hw_mtpr) to an IPR with any\n");
	    	wr (FLG$ERR, "          common scoreboard bits. \n");
	    	wr (FLG$ERR, "          Note: Implicit readers include all memory  operations.\n");
	    	wr (FLG$ERR, "               D-stream memory  operations include LDx, Stx, and \n");
	    	wr (FLG$ERR, "                                virtual mode Hw_ld or Hw_st. \n");
	    	wr (FLG$ERR, "               I-stream memory  operations include Jsr and Hw_ret \n");
	        wr (FLG$ERR, "           		      instructions without the 'stall' bit set.\n");
	    	wr (FLG$ERR, "          Suggestion:  Add NOPs to ensure the offending Hw_mtpr instructions\n");
	    	wr (FLG$ERR, "                       are in different fetch blocks (octaword-aligned octaword).\n***\n");
	        pal_error_count++;
	    }
	}
}


void check_ev6_guideline_6 (INSTR *raw_code[], int offs, List *caveat_list)
/*
 ********************************************************************************
 *	PURPOSE:
 *	    Check end of permutation guideline 6.
 *
 *	INPUT PARAMETERS:
 *		raw_code: pointer to the array of instructions from follow_code
 *		offs: where we are in the above array
 *		      Instruction at offs=0 is the pal entry instruction.
 *		caveat_list: pointer to caveat list
 *
 *	OUTPUT PARAMETERS:
 *		None
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:            
 *	AUTHOR/DATE:
 *	V3.24 FLS Nov-96 
 ********************************************************************************
 */
{
#ifdef guideline_6

	int  idtmp=0;
	int i,j;
	int id1,ipr_num;
	int addr_mt_first=0;	
	int addr_mf=0;	
	int addr_mt_last=0;	
	int last_addr_mt_first=0;		
	int found_mt_first=FALSE;
	int found_mf=FALSE;
	int found_mt_last=FALSE;
	int error=FALSE;
        int scbd[4];
	char *decode_mt_first,*decode_mf, *decode_mt_last;			/* palcode instruction ascii */


        /* search raw_code[offs] for the first MT  then search for MF - MT to same scbd group */
	/* note: hw_ret/stall can be either hw_ret_stall, hw_jmp_stall, hw_jsr_stall, or hw_jcr_stall 
	/* if a hw_ret/stall (actually a hw_jmp_stall) found after MT or MF no violation */
	for (i=0; i<4; i++) scbd[i]=0;		/* clear scoreboard bits */
	j=0;
	while (raw_code[j] != 0) {		
	    if (found_mf==FALSE) {
	        if (raw_code[j]->index == DO_HW_MTPR)  {
		    scbd[0] = raw_code[j]->bits.ev6_hwmxpr_format.scbd;	/* get ipr scoreboard bits*/
		    decode_mt_first =raw_code[j]->decoded;		/* for error report */
		    addr_mt_first =raw_code[j]->address;		/* for error report */
		    if (scbd[0] != 0) found_mt_first=TRUE;
	        }
	    }

	    if (found_mt_first==TRUE) {
		idtmp = get_inst_id (raw_code[j]);  			
		if (idtmp & Hwret_stall) 				/* if hw_jmp_stall found no error */
			found_mt_first=FALSE;
	        if (raw_code[j]->index == DO_HW_MFPR)  {
	            scbd[1] = raw_code[j]->bits.ev6_hwmxpr_format.scbd;	/* get ipr scoreboard bits*/
	    	    decode_mf =raw_code[j]->decoded;			/* for error report */
		    addr_mf =raw_code[j]->address;				/* for error report */
		    if ((scbd[0] & scbd[1]) != 0) found_mf=TRUE;
	        }			    
	    }			    
	    if (found_mf==TRUE) {
		if (raw_code[j]->index == DO_HW_MTPR)  {
	            scbd[2] = raw_code[j]->bits.ev6_hwmxpr_format.scbd;	/* get ipr scoreboard bits*/
	            decode_mt_last =raw_code[j]->decoded;		/* for error report */
	            addr_mt_last =raw_code[j]->address;			/* for error report */
	            if ((scbd[0] & scbd[1] & scbd[2]) != 0) found_mt_last=TRUE;
	        }
	    }
	   if (found_mt_first==TRUE &&  found_mf==TRUE && found_mt_last==TRUE) {
		  error=TRUE;
		  break;
	   }
	   j++;
        } /* while (raw_code[j] != 0)...*/

          if (error == TRUE ) {	
      	    restriction_num= 6;
	    if (new_error (addr_mt_first, restriction_num)) {
	         	wr (FLG$WARN, "%s\n", pheader);
	     	    wr (FLG$WARN, "***\n");
		 if (found_mt_first==TRUE)
	    	    wr (FLG$WARN, "Warning at %X: %s\n", addr_mt_first,decode_mt_first);
		 if (found_mf==TRUE)
	    	    wr (FLG$WARN, "Warning at %X: %s\n", addr_mf,decode_mf);
		 if (found_mt_last==TRUE)
	    	    wr (FLG$WARN, "Warning at %X: %s\n", addr_mt_last,decode_mt_last);

	    	    wr (FLG$WARN, "(PVC #%d) guideline #6: \n",restriction_num);
	    	    wr (FLG$WARN, "           Suggestion: Add a hw_mtpr to a different scoreboard group\n");
	    	    wr (FLG$WARN, "                       between the hw_mfpr and hw_mtpr instructions.\n***\n");
	            pal_error_count++;
	    } /* if (new_error ... */
          } /* if (error == TRUE) */
#endif
}



void check_ev6_restriction_7 (INSTR *raw_code[], int offs, List *caveat_list)
/*
 ********************************************************************************
 *	PURPOSE:
 *	    Check end of permutation restriction 7.
 *
 *                                                                       
 * (7) Restriction: Replay trap and interrupt code sequence and STF/ITOF

 *   On an MBOX replay trap, the EV6 Ibox guarantees that the refetched load or
 * store that caused the trap will issue before any newer loads or stores. For
 * loads and integer stores this is a consequence of the natural operation 
 * of the issue queue. The refetched instruction enter the age-prioritized queue
 * ahead of newer loads and stores will not have any dependencies on dirty
 * registers. Since there is no time-overhead for checking these register
 * dependencies (i.e. it is known upon enqueueing that there are no dirty
 * registers) The queue will issue it in priority order. For floating stores,
 * there is normally some overhead associated with checking the floating point
 * source register dirty status so the store would normally wait before issuing.
 * This would have the undesired consequence of allowing newer loads and stores
 * to issue out-of-order. A deadlock can occur if this out-of-order issue causes
 * the floating store to continually replay trap. To avoid the deadlock, on a
 * floating store replay trap, the source register dirty status is not checked
 * (the source register is assumed to be clean because the store was issued
 * before). 

 * The hardware mechanism which keeps track of replayed floating stores and
 * cancels the dirty register check requires some software restrictions to
 * guarantee that it is applied appropriately to the replayed instruction and 
 * not to other floating stores. 
 * It operates by marking the position in the fetch
 * block (low two bits of the PC) where the replay trap occurred and then
 * canceling the floating point dirty source register check of the next valid
 * instruction enqueued to the integer queue (integer instructions, all loads
 * and stores, and ITOF) which has the same position in the fetch block 
 * (normally the
 * replayed STF). If the PC is somehow diverted to a PAL flow, this hardware
 * might inadvertently cancel the register check of some other STF (or ITOF)
 * instruction. Fortunately, there are a minimal number of reasons why the PC
 * might be diverted during a replay trap. They are:  Interrupts  ITB Fill 
 * (others?) 
 * 
 * In these PAL flows, a STF or ITOF instruction in a given position in a fetch
 * block must be preceded by a valid instruction that is issued out of the 
 * integer queue in the same position in an earlier fetch block. 
 * Acceptable instruction classes include loads, integer stores, 
 * integer operates that do not have R31 as a destination, branches. 
 * 
 *    Example: 
 *    Bad_Interrupt_Flow_Entry: 
 *    ADDQ R31,R31,R0 
 *    STF Fa,(Rb) ; this STF might NOT undergo a dirty source register check and
 *    might give wrong results 
 *    ADDQ R31,R31,R0 
 *    ADDQ R31,R31,R0 
 *    ... 
 *    Good_Interrupt_Flow_Entry: 
 *    ADDQ R31,R31,R0 ; enables FP dirty source register check for (PC<1:0> ==
 *    00) 
 *    ADDQ R31,R31,R0 ; enables FP dirty source register check for (PC<1:0> ==
 *    01) 
 *    ADDQ R31,R31,R0 ; enables FP dirty source register check for (PC<1:0> ==
 *    10) 
 *    ADDQ R31,R31,R0 ; enables FP dirty source register check for (PC<1:0> ==
 *    11) 
 * 
 *    ADDQ R31,R31,R0 
 *    STF Fa,(Rb) ; this STF will successfully undergo a dirty 
 *  		  ; source register check 
 *    ADDQ R31,R31,R0 
 *    ADDQ R31,R31,R0 
 * 
 *
 *	INPUT PARAMETERS:
 *		raw_code: pointer to the array of instructions from follow_code
 *		offs: where we are in the above array
 *		      Instruction at offs=0 is the pal entry instruction.
 *		caveat_list: pointer to caveat list
 *
 *	OUTPUT PARAMETERS:
 *		None
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:            
 *	AUTHOR/DATE:
 *	V3.24 FLS Oct-96 
 ********************************************************************************
 */
{
	int  idtmp=0;
	int i,j;
	int id1;
	int found_stf_itof=TRUE;
	int found_good_inst;
	int found_candidate;
	int fetch_block_candidate;
	int fetch_block_stf_itof;
	int error= FALSE;
        int addr_stf_itof=0;						/* palcode address of caveat */
        int index,class;
	int reg_dst;
        CLASS_INFO *class_def; 
	ERR_MSG	*entry;			
        char *decode_stf_itof;						/* palcode instruction ascii */
      	
	id1 =  Stf_itof;							/* search for stf/sts/stt/std=stg*/

	while(found_stf_itof == TRUE ) {		
	    if (entry = search_ev6_caveat (id1, caveat_list, addr_stf_itof)) {
	        addr_stf_itof = entry->errnum;		/* get palcode address */	
		decode_stf_itof = entry->explanation; 	/* get palcode instruction ascii */	
		found_stf_itof=TRUE;
	    } else found_stf_itof=FALSE;
	    free (entry);

	error=FALSE;				/* assume the best  */
	/* Good CASE example
	 *		fetch_block[a]
	 *		fetch_block[a+1]	addq r31,r31,r0		; integer operate inst at address[1:0]=01
	 *		fetch_block[a+2]
	 *		fetch_block[a+3]
	 *		...
	 *		...
	 *		fetch_block[n]
	 *		fetch_block[n+1]	stf	f0,(r20)	;floating store at address[1:0]=01
	 *		fetch_block[n+2]
	 *		fetch_block[n+3]
	 *		...
	 *	if the addq was at any other fetch_block offset then it would be error case.
	 */
	found_good_inst = FALSE;
	if (found_stf_itof==TRUE) {
	    i=0;			/* raw_code[0]->address = pal_entry */
	    while (raw_code[i] != 0) {   
	        if (raw_code[i]->address == addr_stf_itof) {
		    fetch_block_stf_itof = raw_code[i]->address & 0xF;
		    /* search for an instruction that issues from the IQ at the same fetch_block offset as the STF */
		    /* class qi_itof issues out of the IQ, but is not included in acceptable inst here */
		    for (j=i; j >= 0 ;j--) {
      	            	index = raw_code[j]->index;	/* get unique inst. index */
	            	class = instr_array[index]->imp->di_class;
	    		if ( (class ==qi_ild) || (class==qi_ist) || (class==qi_ibr) || 
			     (class==qi_jsr) ||  (class==qi_mfpr) || (class==qi_mtpr)) { /* not conditional on r31=dest */
			         found_candidate = TRUE;
			}
	    		else if ( (class ==qi_ild) ) {			/* ldq_u  r31=dest not issued from IQ  */
			      if ( (raw_code[j]->index != DO_LDQU) || 
			       ((raw_code[j]->index == DO_LDQU) && (raw_code[j]->bits.mem_format.ra !=31)) )
			     	 found_candidate = TRUE;
			     }
	    		else if ( (class ==qi_iadd) || (class==qi_ilog) || (class==qi_imul) || 
			          (class==qi_ishf) || (class==qi_imisc) ) {   /* integer operates conditional on r31 =dest */
				      class_def = instr_array[raw_code[j]->index];
				      if (class_def->arch->dest_reg == Ra) 
				               reg_dst = raw_code[j]->bits.op_format.ra;
      				      else if (class_def->arch->dest_reg == Rc) 
					       reg_dst = raw_code[j]->bits.op_format.rc;

			      	      if (reg_dst != 31 ) {
			                 found_candidate = TRUE;
				      }
			      }
			if (found_candidate==TRUE) {
	   		    fetch_block_candidate = raw_code[j]->address & 0xF;
			    if (fetch_block_candidate == fetch_block_stf_itof) {
				found_good_inst = TRUE;
				break;
			    }else found_candidate = FALSE;
			}
		    } /* for (j...) */
		    break;
	   	 } /* if (raw_code...)*/
	         i++;
	     } /* while ... */
             if (found_good_inst == FALSE)
		    error=TRUE;
             else   error=FALSE;
	} /* 	if (found_stf_itof==TRUE... */


        if (error == TRUE ) {	
	    restriction_num= 7;
	    if (new_error (addr_stf_itof, restriction_num)) {
	            wr (FLG$ERR, "%s\n", pheader);
	     	    wr (FLG$ERR, "***\n");
	    	    wr (FLG$ERR, "Error at %X: %s\n", addr_stf_itof, decode_stf_itof);
	    	    wr (FLG$ERR, "(PVC #%d) RESTRICTION #7: \n",restriction_num);
	            wr (FLG$ERR, "          The CPU hardware logic that keeps track of replayed floating\n");
	            wr (FLG$ERR, "          stores and cancels dirty register checks, requires this PALcode \n");
	            wr (FLG$ERR, "          restriction to ensure it is applied to the replayed instruction\n");
	            wr (FLG$ERR, "          and not to other floating stores. \n");
	            wr (FLG$ERR, "          The following suggestion will ensure that if a fault or\n");
	            wr (FLG$ERR, "          exception occurs the CPU will not inadvertently cancel\n");
	            wr (FLG$ERR, "          the source register check of some other STF/T/S/D or ITOFx\n");
	            wr (FLG$ERR, "          instruction.\n");
	    	    wr (FLG$ERR, "          Suggestion: Put the following instruction in a fetch block\n");
	            wr (FLG$ERR, "                      prior to the %s\n",decode_stf_itof);
	            wr (FLG$ERR, "                      at the same fetch block (octaword-aligned octaword)\n");
	            wr (FLG$ERR, "                      offset.\n");
	            wr (FLG$ERR, "\n");
	    	    wr (FLG$ERR, "                      addq r31,r31,r0 \n***\n");
	            pal_error_count++;
	    } /* if (new_error ... */
        } /* if (error == TRUE) */
     } /* while ... */
}

void check_ev6_guideline_11 (INSTR *raw_code[], int offs, List *caveat_list)
/*
 ********************************************************************************
 *	PURPOSE:
 *	    Check end of permutation guideline 11
 *
 *
 *(11) Guideline: Ibox IPR update synchronization 
 *
 *   When updating any Ibox IPR, a return to native(virtual) mode should use the
 *   HW_RET instruction with associated STALL bit set to insure that the
 *   updated IPR value affects all instructions following the return path. The new
 *   IPR value takes effect only after the associated HW_MTPR instruction
 *   retires. 
 *
 *	INPUT PARAMETERS:
 *		raw_code: pointer to the array of instructions from follow_code
 *		offs: where we are in the above array
 *		      Instruction at offs=0 is the pal entry instruction.
 *		caveat_list: pointer to caveat list
 *
 *	OUTPUT PARAMETERS:
 *		None
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:            
 *	AUTHOR/DATE:
 *	V3.24 FLS Sep-96 
 ********************************************************************************
 */
{
	int  idtmp=0;
	int i;
	int id1;
	int addr1=0;			/* palcode address of caveat */
	int found_mt_ibox=TRUE;
	int found_addr1;
	int found_hwret;
	int found_hwret_stall;
	int error= FALSE;
	int index,found_bsr=FALSE;
	ERR_MSG	*entry;			
	char *decode1;			/* palcode instruction ascii */
      	
	id1 = Mt + Ibox_ipr;		
	while(found_mt_ibox == TRUE ) {		/* search for all MT ibox_iprs */
	    if (entry = search_ev6_caveat (id1, caveat_list, addr1)) {
	        addr1 = entry->errnum;		/* get palcode address */	
		decode1 = entry->explanation; 	/* get palcode instruction ascii */	
		found_mt_ibox=TRUE;
	    } else found_mt_ibox=FALSE;
	    free (entry);

	    /* search raw_code[offs] for addr1 then search for hwret/stall or hw_ret */
	    /* note: hw_ret/stall can be either hw_ret_stall, hw_jmp_stall, hw_jsr_stall, or hw_jcr_stall 
	    /* if a hw_ret/stall found first all is well else if hw_ret found then issue error */
	    /* if bsr found then ignore the following hw_ret or hw_jcr, since it is return from bsr and */
	    /* not the end of pal entry flow */
            found_hwret=FALSE;
	    found_hwret_stall=FALSE;
            found_addr1=FALSE;
            found_bsr=FALSE;
	    if (found_mt_ibox==TRUE) {
	        i=0;
      	        while (raw_code[i] != 0) {
	   	    if (raw_code[i]->address == addr1) {
			offs = i+1;
      	    	        while (raw_code[offs] != 0) {		
			    index = raw_code[offs]->index;		     
			    if (index == DO_BSR) 
				found_bsr=TRUE;
			    idtmp = get_inst_id (raw_code[offs]);
	   	            if (idtmp & Hwret_stall) {
		                found_hwret_stall=TRUE; /* found hw_ret, hw_jmp,hw_jsr,hw_jcr with STALL*/
		                break;
		            }
			    if (idtmp & Hwret) {
			       /* if this is a BSR followed by HW_RET (hint=2) or HW_JCR (hint=3) then okay */
			        if ((raw_code[offs]->bits.ev6_hwret_format.hint == 2) ||
				    (raw_code[offs]->bits.ev6_hwret_format.hint == 3)) {
				       if (found_bsr==TRUE)
				           found_bsr=FALSE;   /* ok, hw_ret or hw_jcr follows a BSR*/
			               else {
				             found_hwret=TRUE;  /* error, found hw_ret, hw_jcr with no  BSR*/
		                              break;	
				            }
				}
		            }
	                    offs++;
	   	        } /* while ... */
	   	     } /* if (raw_code...)*/
	             i++;
	         } /* while ... */

	    if (found_mt_ibox==TRUE && found_hwret == TRUE && found_hwret_stall==FALSE)	
	           error=TRUE;
	    else   error=FALSE;
	  } /* 	if (found_mt_ibox==TRUE... */

          if (error == TRUE ) {	
	      restriction_num= 11;
	      if (new_error (addr1, restriction_num)) {
	            wr (FLG$WARN, "%s\n", pheader);
	     	    wr (FLG$WARN, "***\n");
	    	    wr (FLG$WARN, "Warning at %X: %s\n",  addr1,decode1);
	    	    wr (FLG$WARN, "(PVC #%d) GUIDELINE #11: \n",restriction_num);
	    	    wr (FLG$WARN, "           A Hw_mtpr to any IBOX IPR should be followed by a Hw_ret_stall,\n");
	    	    wr (FLG$WARN, "           Hw_jmp_stall, or rarely Hw_jsr_stall/ Hw_jcr_stall.\n");
	    	    wr (FLG$WARN, "           The IBOX IPR will not be updated until the Hw_mtpr retires.\n");
	    	    wr (FLG$WARN, "           NOTE: This is a GUIDELINE not a restriction.\n");
	            wr (FLG$ERR, "                To force PVC to ignore this error, put the ignore label at\n");
	            wr (FLG$ERR, "                address %x: \n",addr1);
	    	    wr (FLG$WARN, "           Suggestion: Use Hw_ret_stall to return to native mode. \n***\n");
	            pal_error_count++;
	      } /* if (new_error ... */
          } /* if (error == TRUE) */
        } /* while(found_mt_ibox == TRUE ... */
}



void check_ev6_restriction_12 (INSTR *raw_code[], int offs, int *id, int poss, int count)
/*
 ********************************************************************************
 *	PURPOSE:
 *		Check EV6 restriction #12.
 *
 *(12) Restriction: HW_MFPR EXC_ADDR/IVA_FORM/EXC_SUM Usage 
 *
 *    These non-renamed hardware registers can only be read reliably by a 
 *    HW_MFPR within the first fetch block of a PAL entry point 
 *    AND prior to any taken branch.
 *    The other instructions in the fetch block should not include 
 *    PAL instructions that might trap, since a trap might change the value 
 *    in the  IVA_FORM, EXC_SUM registers.
 *
 *	    Ed m agreed can ignore a Hw_st/P or Hw_ld/P since the ev6
 *	    goes ahead and even can retire the  hw_ld/p before
 *	    the read data is returned to the register.
 *
 *
 *    Hardware protects the values from overwrite for the first fetch block 
 *    (4 instructions) of a pal flow. During this fetch block, the PALcode 
 *    should save whatever registers it will need.
 *
 * 
 *	INPUT PARAMETERS:
 *		raw_code: pointer to the array of instructions from follow_code
 *		offs: where we are in the above array
 *		id:   array of ipr info such as scoreboard bits
 *		count: Number of Longwords in fetch block. Max = 4.
 *	OUTPUT PARAMETERS:
 *		None
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:            
 *	AUTHOR/DATE:
 *	V3.24 FLS Aug 96 
 ********************************************************************************
 */
{
      int i, j, index[4];
      int found_br=FALSE;
      int found_mfplus=FALSE;
      int found_them=FALSE;
      int found_trap_inst=FALSE;
      int ipr_num, idtmp;
      int error=FALSE;
      int addr0;
      for (i=0; i<4; i++) index[i]=0;

      /* check if a Hw_mfpr exc_addr/iva_form/exc_sum is in first fetch block 
	after a branch or branch conditional which is a violation. 
      */
          for (i=0; i<count; i++) {				
	   	if (get_inst_id (raw_code[offs+i]) & Br)  {
		    found_br = TRUE;
		    break;
		}
          }

        if (first_fetch_block_ev6 == TRUE) {
            if (found_br == TRUE ) {
		i++;					/* skip past br */
		j = i;			
                for (j=i; j<count; j++) {				
      	                index[j] = raw_code[offs+j]->index;	/* get unique inst. index */
                        if (index[j] == DO_HW_MFPR) {
		            ipr_num = raw_code[offs+j]->bits.ev6_hwmxpr_format.index;
		            if ((ipr_num == EV6__EXC_ADDR) || 
			        (ipr_num == EV6__IVA_FORM) ||
		                (ipr_num == EV6__EXC_SUM)) {
		                   found_mfplus = TRUE;
			           break;
		            }
	                } 
	         } /* for (j=i; j<count; j++) */
              }	/* if found_br == TRUE */

              if (found_mfplus == TRUE) {			/* j = offset then error */
	        restriction_num= 12;
	        if (new_error (raw_code[offs+j]->address, restriction_num)) {
	         	wr (FLG$ERR, "%s\n", pheader);
	     	    wr (FLG$ERR, "***\n");

	            for (i=0; i<count; i++) {
	    	        wr (FLG$ERR, "Error at %X: %s\n", 
			 raw_code[offs+i]->address,raw_code[offs+i]->decoded);
		    }
	    	    wr (FLG$ERR, "(PVC #%d) RESTRICTION #12: \n",restriction_num);
	    	    wr (FLG$ERR, "          IPRs  EXC_ADDR, IVA_FORM, and EXC_SUM IPRs can only be read reliably\n");
	    	    wr (FLG$ERR, "          by an Hw_mfpr within the first fetch block of a PAL entry point,\n");
	    	    wr (FLG$ERR, "          and prior to any taken branch.\n");
	    	    wr (FLG$ERR, "          Suggestion: Move the Hw_mfpr before the conditional Branch.\n");
	    	    wr (FLG$ERR, "***\n");
	            pal_error_count++;
	        }
             }

	     /* check that no instructions that could trap in first fetch block too*/
	     /* hw_ld/p and hw_st/p are considered non trapping here */
             if (found_mfplus == FALSE) {		
                    for (j=0; j<count; j++) {				
      	                index[j] = raw_code[offs+j]->index;	/* get unique inst. index */
                        if (index[j] == DO_HW_MFPR) {
		            ipr_num = raw_code[offs+j]->bits.ev6_hwmxpr_format.index;
		            if ((ipr_num == EV6__EXC_ADDR) || 
			        (ipr_num == EV6__IVA_FORM) ||
		                (ipr_num == EV6__EXC_SUM)) 
		                      found_them = TRUE;
	   	         } 
			idtmp = get_inst_id (raw_code[offs+j]);			/* get class of instruction */

			/* if trappable instruction and not hw_ld/p or hw_st/p */
	    	  	if ((idtmp & Arith_trap  || idtmp & Memory_op) &&
			     !(idtmp & Physical_inst ))	
		    		   found_trap_inst = TRUE;
                        if (found_them == TRUE && found_trap_inst == TRUE ) {		/* V3.27 */
			    addr0 = raw_code[offs+j]->address;				/* address of trappable inst */
			    error = TRUE;
			    break;
			}
		    }
			if (error == TRUE ) {			/* V3.27 */
			restriction_num=12;
	        	if (new_error (addr0, restriction_num)) {
	         		wr (FLG$ERR, "%s\n", pheader);
	     	    		wr (FLG$ERR, "***\n");
	                    for (i=0; i<count; i++) {
	    	    		  wr (FLG$ERR, "Error at %X: %s\n", 
			 		raw_code[offs+i]->address,raw_code[offs+i]->decoded);
		            }
	    	    	   wr (FLG$ERR, "(PVC #%d) RESTRICTION #12a: \n",restriction_num);
	    	    	   wr (FLG$ERR, "          There cannot be any instructions that could trap in the same\n");
	    	    	   wr (FLG$ERR, "          fetch block as  Hw_mfpr EXC_ADDR ,  Hw_mfpr IVA_FORM, \n");
	                   wr (FLG$ERR, "          or  Hw_mfpr EXC_SUM.  Trappable instructions include memory\n");
	                   wr (FLG$ERR, "          operations and arithmetic instructions. \n");
	                   wr (FLG$ERR, "          NOTE: To force PVC to ignore this error, put the ignore label at\n");
	                   wr (FLG$ERR, "                address %x: \n",addr0);
	    	    	   wr (FLG$ERR, "          Suggestion: Move the possible trapping instruction out of the \n");
	    	    	   wr (FLG$ERR, "                      first fetch block (octaword-aligned octaword).\n***\n");
	            	   pal_error_count++;
	               }
		   }
	      }
        }else {		/* ELSE  not first_fetch_block_ev6 */

                for (j=0; j<count; j++) {				
      	                index[j] = raw_code[offs+j]->index;	/* get unique inst. index */
                        if (index[j] == DO_HW_MFPR) {
		            ipr_num = raw_code[offs+j]->bits.ev6_hwmxpr_format.index;
		            if ((ipr_num == EV6__EXC_ADDR) || 
			        (ipr_num == EV6__IVA_FORM) ||
		                (ipr_num == EV6__EXC_SUM)) {
		                   found_them = TRUE;
			           break;
		            }
	                } 
	         } /* for (j=0; j<count; j++) */

            if (found_them == TRUE) {			/* then error */
		restriction_num=12;
	        if (new_error (raw_code[offs+j]->address, restriction_num)) {
	         	wr (FLG$ERR, "%s\n", pheader);
	     	    wr (FLG$ERR, "***\n");
	    	    wr (FLG$ERR, "Error at %X: %s\n", 
			 raw_code[offs+j]->address,raw_code[offs+j]->decoded);

	    	    wr (FLG$ERR, "(PVC #%d) RESTRICTION #12b: \n",restriction_num);
	    	    wr (FLG$ERR, "          IPRs EXC_ADDR, IVA_FORM, and EXC_SUM IPRs can only be read reliably\n");
	    	    wr (FLG$ERR, "          by an Hw_mfpr within the first fetch block of a PAL entry point,\n");
	    	    wr (FLG$ERR, "          and prior to any taken branch.\n");
	    	    wr (FLG$ERR, "          Suggestion: Move the Hw_mfpr into the first\n");
	    	    wr (FLG$ERR, "                      fetch block (octaword-aligned octaword).\n");
	    	    wr (FLG$ERR, "***\n");
	            pal_error_count++;
		}
            } /* if found_them == TRUE */
	} /* if first_fetch_block_ev6 == TRUE */
}


void check_ev6_restriction_14 (INSTR *raw_code[], int offs, int *id, int poss, int count)
/*
 ********************************************************************************
 *	PURPOSE:
 *		Check EV6 restriction #14.
 *
 * (14) Restriction: HW_RET 
 *   No hw_ret in the first fetch block of PAL routine. The HW_RET will be
 *   mispredicted and the JSR/RETURN stack might lose its synchronization. 
 * Modified 5-dec-1996:
 *
 *    I just modified pal restriction 14 which looks for hw_ret
 *    placement in a pal routine.  It can't be in the first
 *    fetch block in general, but should be ok in the first
 *    fetch block of a call_pal routine.  Call_pals are
 *    treated like jsrs, and need to work for short routines
 *    with returns in the first fetch block.
 *
 *	INPUT PARAMETERS:
 *		raw_code: pointer to the array of instructions from follow_code
 *		offs: where we are in the above array
 *		id:   array of ipr info such as scoreboard bits
 *		count: Number of Longwords in fetch block. Max = 4.
 *	OUTPUT PARAMETERS:
 *		None
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:            
 *	AUTHOR/DATE:
 *	V3.24 FLS Aug 96 
 ********************************************************************************
 */
{
  int i, index[4];

  /* if HW_RET in first fetch block and not call_pal then error. This must only be called
   * at the first entry of a pal flow.
   */

  if (first_fetch_block_ev6 == TRUE) {
   if ((raw_code[offs]->address < EV6__CALL_PAL_00_ENTRY) ||
       (raw_code[offs]->address > (EV6__CALL_PAL_80_ENTRY + 0xfff))) {

      for (i=0; i<4; i++) index[i]=0;
      for (i=0; i<count; i++) {				
      	   index[i] = raw_code[offs+i]->index;				/* get unique inst. index */

        if ((index[i] == DO_HW_RET) &&
	   (raw_code[offs+i]->bits.ev6_hwret_format.hint != 0)) { /* not hw_jmp */
	    restriction_num= 14;
	    if (new_error (raw_code[offs+i]->address, restriction_num)) {
	     	wr (FLG$ERR, "%s\n", pheader);
	     	wr (FLG$ERR, "***\n");
	    	wr (FLG$ERR, "Error at %X: %s\n", 
		    raw_code[offs+i]->address,raw_code[offs+i]->decoded);
	    	wr (FLG$ERR, "(PVC #%d) RESTRICTION #14: \n",restriction_num);
	    	wr (FLG$ERR, "          No Hw_ret allowed in the first fetch block of a PAL routine.\n");
	    	wr (FLG$ERR, "          Suggestion: Add NOPs to ensure the Hw_ret is moved to the second\n");
	    	wr (FLG$ERR, "                      fetch block (octaword-aligned octaword).\n***\n");
	        pal_error_count++;
	    }
	}
      }

   } /* if ((raw_code[offs]->address < EV6__CALL_PAL_00_ENTRY) ||... */
  } /* if (first_fetch_block_ev6 == TRUE... */

}

void check_ev6_restriction_17 (INSTR *raw_code[], int offs, List *caveat_list)
/*
 ********************************************************************************
 *	PURPOSE:
 *	    Check end of permutation restriction 17.
 *
 * (17) Restriction: MTPR to DTB_TAG0/DTB_PTE0/DTB_TAG1/DTB_PTE1 
 *
 *   These four writes must be executed atomically, i.e. either all four must
 *   retire or none of them may retire. 
 *
 *   In other words there can be no Arithmetic traps or Branch type instructions between these
 *   four Hw_MTPR instructions. 
 *
 *	INPUT PARAMETERS:
 *		raw_code: pointer to the array of instructions from follow_code
 *		offs: where we are in the above array
 *		      Instruction at offs=0 is the pal entry instruction.
 *		caveat_list: pointer to caveat list
 *
 *	OUTPUT PARAMETERS:
 *		None
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:            
 *	AUTHOR/DATE:
 *	V3.24 FLS Sep-96 
 *	V3.25 FLS Mar-97 updated description to remove "must issue"
 *			 did not change code, which goes beyond the restriction by
 *			 requiring all four MT instructions to be in order. Although
 *			 pvc does allow intervening instructions.
 ********************************************************************************
 */
{
      int  idtmp=0;
      int error= FALSE;
      int id1,id2,id3,id4;
      ERR_MSG	*entry;			
      int addr1=0,addr2=0,addr3=0,addr4=0;			/* palcode address of caveat */
      int last_addr1=0;
      int er1,er2,er3,er4;
      int found_tag0=TRUE;
      int found_pte0=TRUE;
      int found_tag1=TRUE;
      int found_pte1=TRUE;
      char *decode1, *decode2, *decode3,*decode4;		/* palcode instruction ascii */
      	

	/* if MT Dtb_tag0  found then ensure followed by
	      MT Dtb_tag1
	      MT Dtb_pte0
	      MT Dtb_pte1
	 With no intervening trap (Arith_trap) or Branch (Br) instructions.
	 This will require they be written in order as above, even though that
	 is not a requirement of the restriction, the coding effort just isn't
	 worth it, since everyone who is smart will be using a code template anyway.
	*/
	id1 = Mt + Dtb_tag0;		/* search for hw_mtpr to Dtb_tag0 */
	id2 = Mt + Dtb_tag1;
	id3 = Mt + Dtb_pte0;	
	id4 = Mt + Dtb_pte1;		


     while (found_tag0==TRUE || found_tag1==TRUE || found_pte0==TRUE || found_pte1==TRUE) {		/* if any written */

	/* first check that all IPRs are written to or none are */
	if (entry = search_ev6_caveat (id1, caveat_list, addr1)) {
		addr1 = entry->errnum;		/* get palcode address */	
		decode1 = entry->explanation; 	/* get palcode instruction ascii */	
		found_tag0=TRUE;
	}else found_tag0=FALSE;
	free (entry);

	if (entry = search_addr_caveat (id2, caveat_list, addr1+4)) {
		addr2 = entry->errnum;		/* get palcode address */	
		decode2 = entry->explanation; 	/* get palcode instruction ascii */	
		found_tag1=TRUE;
	}else found_tag1=FALSE;
	free (entry);

	if (entry = search_addr_caveat (id3, caveat_list, addr1+8)) {
		addr3 = entry->errnum;		/* get palcode address */	
		decode3 = entry->explanation; 	/* get palcode instruction ascii */	
		found_pte0=TRUE;
	}else found_pte0=FALSE;
	free (entry);
	if (entry = search_addr_caveat (id4, caveat_list, addr1+12)) {
		addr4 = entry->errnum;		/* get palcode address */	
		decode4 = entry->explanation; 	/* get palcode instruction ascii */	
		found_pte1=TRUE;
	}else found_pte1=FALSE;
	free (entry);
            
	error = FALSE;
	if (found_tag0==TRUE || found_tag1==TRUE || found_pte0==TRUE || found_pte1==TRUE) {		/* if any written */
	    if (found_tag0==TRUE && found_tag0==TRUE && found_pte0==TRUE && found_pte1==TRUE) {    /* all must be written */
		  error=FALSE;
	    }else error=TRUE;									/* else error */
	}

        if (error == TRUE ) {	
	    restriction_num= 17;
	    if (addr1 > 0 && new_error (addr1, restriction_num )) er1=TRUE; else er1=FALSE;
	    if (addr2 > 0 && new_error (addr2, restriction_num )) er2=TRUE; else er2=FALSE;
	    if (addr3 > 0 && new_error (addr3, restriction_num )) er3=TRUE; else er3=FALSE;
	    if (addr4 > 0 && new_error (addr4, restriction_num )) er4=TRUE; else er4=FALSE;

	if (er1==TRUE || er2==TRUE || er3==TRUE || er4==TRUE) {
	        wr (FLG$ERR, "%s\n", pheader);
	     	wr (FLG$ERR, "***\n");
	if (er1==TRUE) 
	      wr (FLG$ERR, "Error at %X: %s\n",  addr1,decode1);
	if (er2==TRUE) 
	      wr (FLG$ERR, "Error at %X: %s\n",  addr2,decode2);
	if (er3==TRUE) 
	      wr (FLG$ERR, "Error at %X: %s\n",  addr3,decode3);
	if (er4==TRUE) 
	      wr (FLG$ERR, "Error at %X: %s\n",  addr4,decode4);

	    	    wr (FLG$ERR, "(PVC #%d) RESTRICTION #17: \n",restriction_num);
	            wr (FLG$ERR, "           Hw_mtpr DTB_TAG0,  Hw_mtpr DTB_PTE0,  Hw_mtpr DTB_TAG1,\n");
	            wr (FLG$ERR, "           and  Hw_mtpr DTB_PTE1 must complete atomically.\n");
	    	    wr (FLG$ERR, "           In other words all four IPRs must be written to in sequence \n");
	    	    wr (FLG$ERR, "           with no intervening Branches, traps, or exceptions.\n");
	    	    wr (FLG$ERR, "           Suggestion: Here is a code example to follow:\n");
	    	    wr (FLG$ERR, "		hw_mtpr r4,  EV6__DTB_TAG0	; (2&6,0L) write tag0.\n");
	    	    wr (FLG$ERR, "		hw_mtpr r4,  EV6__DTB_TAG1	; (1&5,1L) write tag1.\n");
	    	    wr (FLG$ERR, "		hw_mtpr r25, EV6__DTB_PTE0	; (0&4,0L) write pte0.\n");
	    	    wr (FLG$ERR, "		hw_mtpr r25, EV6__DTB_PTE1	; (3&7,1L) write pte1.\n***\n");
	            pal_error_count++;
	    } /* if (addr1 > 0 || ... */
        } /* if (error == TRUE) */
     addr1 = addr1 + 16;	/* point to next possible tag0 address */
     } /* while ... */
}




void check_ev6_restriction_18 (INSTR *raw_code[], int offs, int *id, int poss, int count)
/*
 ********************************************************************************
 *	PURPOSE:
 *		Check EV6 restriction #18.
 *
 *(18) Restriction: No FP OPERATES,FP CONDITIONAL
 * BRANCHES, FTOI, or STF in same fetch block as MTPR 
 *
 *   For convenience of implementation, no floating point operate instructions,
 *   floating point stores, floating-to-integer register moves, or FP conditional
 *   branches in the same fetch block as any hardware move-to-processor register
 *   instructions. This inludes
 *   ADDx/MULx/DIVx/SQRTx/FPConditionalBranch/STx/FTOIx (where x =
 *   any applicable floating point data type) but does not include LDx/ITOFx. 
 * 
 *	INPUT PARAMETERS:
 *		raw_code: pointer to the array of instructions from follow_code
 *		offs: where we are in the above array
 *		id:   array of ipr info such as scoreboard bits
 *		count: Number of Longwords in fetch block. Max = 4.
 *	OUTPUT PARAMETERS:
 *		None
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:            
 *	AUTHOR/DATE:
 *	V3.24 FLS Aug 96 
 ********************************************************************************
 */
{
      int i, index[4];
      int found_mt=FALSE;
      int class;

      for (i=0; i<4; i++) index[i]=0;

      for (i=0; i<count; i++) {				
      	    index[i] = raw_code[offs+i]->index;						/* get unique inst. index */
            if (index[i] == DO_HW_MTPR) 						        /* check if MT in any cache line */
		found_mt = TRUE;
      }

      if (found_mt == TRUE ) {
	/* check for any fp instruction classes except classes containing
	   floating point LDx or ITOF instructions.
	*/

          for (i=0; i<count; i++) {				
	    if (raw_code[offs+i] != 0) {
      	        index[i] = raw_code[offs+i]->index;	/* get unique inst. index */
	        class = instr_array[index[i]]->imp->di_class;
	    }else {
		class = 0;				/* undefined index */
		break;
	     }
	    if ((class ==qi_fadd) || (class==qi_fmul) || (class==qi_fdiv) || (class==qi_fsqrt) ||
	       (class==qi_fbr) || (class==qi_fst) || (class==qi_ftoi)) {
		restriction_num= 18;
	        if (new_error (raw_code[offs+i]->address, restriction_num)) {
	         	wr (FLG$ERR, "%s\n", pheader);
	     	    wr (FLG$ERR, "***\n");

	            for (i=0; i<count; i++) {
	    	       wr (FLG$ERR, "Error at %X: %s\n", 
		           raw_code[offs+i]->address,raw_code[offs+i]->decoded);
		    }
	    	    wr (FLG$ERR, "(PVC #%d) RESTRICTION #18: \n",restriction_num);
	            wr (FLG$ERR, "          No floating point  operate,  stores, floating move to integer, \n");
	    	    wr (FLG$ERR, "          or floating conditional branches, allowed in the same fetch\n");
	    	    wr (FLG$ERR, "          block as a Hw_mtpr.\n");
	            wr (FLG$ERR, "          Suggestion:  Add NOPs to ensure the Hw_mtpr is moved to a \n");
	    	    wr (FLG$ERR, "                       different fetch block (octaword-aligned octaword).\n***\n");
	            pal_error_count++;
	        }
	    }
          }
      }

}


void check_ev6_restriction_19 (INSTR *raw_code[], int offs, List *caveat_list)
/*
 ********************************************************************************
 *	PURPOSE:
 *	    Check end of permutation restriction 19
 *
 * (19) Restriction: HW_RET/STALL after updating the FPCR via MT_FPCR in PALmode
 *
 *   FPCR updating happens in hardware based on the retire of nontrapping
 *   version of MT_FPCR (in PALcode). Use a HW_RET/STALL after the
 *   nontrapping MT_FPCR to achieve minimum latency (4 cycles) between the
 *   retiring of the MT_FPCR and the first FLOP that uses the updated FPCR. 
 *
 *	INPUT PARAMETERS:
 *		raw_code: pointer to the array of instructions from follow_code
 *		offs: where we are in the above array
 *		      Instruction at offs=0 is the pal entry instruction.
 *		caveat_list: pointer to caveat list
 *
 *	OUTPUT PARAMETERS:
 *		None
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:            
 *	AUTHOR/DATE:
 *	V3.24 FLS Sep-96 
 ********************************************************************************
 */
{
	int  idtmp=0;
	int i;
	int id1;
	int addr1=0;			/* palcode address of caveat */
	int found_mt_fpcr=TRUE;
	int found_addr1;
	int found_hwret;
	int found_hwret_stall;
	int index,found_bsr=FALSE;
	int error= FALSE;
	ERR_MSG	*entry;			
	char *decode1;			/* palcode instruction ascii */
      	
	id1 = Mt_fpcr;			/* search for MT_FPCR */
	while(found_mt_fpcr == TRUE ) {		
	    if (entry = search_ev6_caveat (id1, caveat_list, addr1)) {
	        addr1 = entry->errnum;		/* get palcode address */	
		decode1 = entry->explanation; 	/* get palcode instruction ascii */	
		found_mt_fpcr=TRUE;
	    } else found_mt_fpcr=FALSE;
	    free (entry);

	    /* search raw_code[offs] for addr1 then search for hwret/stall or hw_ret */
	    /* note: hw_ret/stall can be either hw_ret_stall, hw_jmp_stall, hw_jsr_stall, or hw_jcr_stall 
	    /* if a hw_ret/stall found first all is well else if hw_ret found then issue error */
	    /* if bsr found then ignore the following hw_ret or hw_jcr, since it is return from bsr and */
	    /* not the end of pal entry flow */
            found_hwret=FALSE;
	    found_hwret_stall=FALSE;
            found_addr1=FALSE;
	    if (found_mt_fpcr==TRUE) {
	        i=0;
      	        while (raw_code[i] != 0) {
	   	    if (raw_code[i]->address == addr1) {
			offs = i+1;
      	    	        while (raw_code[offs] != 0) {		
			    index = raw_code[offs]->index;		     
			    if (index == DO_BSR) 
				found_bsr=TRUE;
		            idtmp = get_inst_id (raw_code[offs]);
	   	            if (idtmp & Hwret_stall) {
		                found_hwret_stall=TRUE;
		                break;
		            }
			    if (idtmp & Hwret) {
			       /* if this is a BSR followed by HW_RET (hint=2) or HW_JCR (hint=3) then okay */
			        if ((raw_code[offs]->bits.ev6_hwret_format.hint == 2) ||
				    (raw_code[offs]->bits.ev6_hwret_format.hint == 3)) {
				       if (found_bsr==TRUE)
				           found_bsr=FALSE;   /* ok, hw_ret or hw_jcr follows a BSR*/
			               else {
				             found_hwret=TRUE;  /* error, found hw_ret, hw_jcr with no  BSR*/
		                             break;	
				            }
				}
		            }
	                    offs++;
	   	        } /* while ... */
	   	     } /* if (raw_code...)*/
	             i++;
	         } /* while ... */

	    if (found_mt_fpcr==TRUE && found_hwret == TRUE && found_hwret_stall==FALSE)	
	           error=TRUE;
	    else   error=FALSE;
	  } /* 	if (found_mt_fpcr==TRUE... */

        if (error == TRUE ) {	
	    restriction_num= 19;
	    if (new_error (addr1, restriction_num)) {
	            wr (FLG$ERR, "%s\n", pheader);
	     	    wr (FLG$ERR, "***\n");
	    	    wr (FLG$ERR, "Error at %X: %s\n", addr1,decode1);
	    	    wr (FLG$ERR, "(PVC #%d) RESTRICTION #19: \n",restriction_num);
	    	    wr (FLG$ERR, "           A Mt_fpcr should followed by a Hw_ret_stall, Hw_jmp_stall, or \n");
	    	    wr (FLG$ERR, "           rarely by Hw_jsr_stall, or Hw_jcr_stall.\n");
	    	    wr (FLG$ERR, "           Use one of these hw_ret stall instructions to achieve minimum\n");
	            wr (FLG$ERR, "           latency between the retiring of the Mt_fpcr instruction\n");
	            wr (FLG$ERR, "           and the first internal register that uses the updated FPCR.\n");
	    	    wr (FLG$ERR, "           Suggestion: Put a Hw_jmp_stall after the MT_FPCR.\n***\n");
	            pal_error_count++;
	    } /* if (new_error ... */
        } /* if (error == TRUE) */
     } /* while ... */
}


void check_ev6_restriction_21 (INSTR *raw_code[], int offs, List *caveat_list)
/*
 ********************************************************************************
 *	PURPOSE:
 *	    Check end of permutation restriction 21.
 *	    Same as check_ev6_restriction_22 except asn.
 *
 *(21) Restriction: HW_RET/STALL after MT ASN0/ASN1 
 *
 *   There must be a scoreboard bit -> register dependency chain to prevent MT
 *   ASN0 or MT ASN1 from issuing while ANY of scoreboard bits <7:4> are set. A
 *   code sequence which accomplishes this: 
 *
 *   HW_MFPR IPR_VA,SCBD<7,6,5,4>,R0 
 *   XOR R0,R0,R0 
 *   BIS R0,R9,R9 
 *   BIS R31,R31,R31 
 *
 *   HW_MTPR R9,IS0,SCBD<6> 
 *   HW_MTPR R9,ASN1,SCBD<7> 
 *
 *   This sequence guarantees, through the register dependency on R0, that 
 *   neither HW_MTPR are issued before scoreboard bits <7:4> are cleared.
 *
 * The following parts of the restriction are checked in check_ev6_restriction_21a
 * and check_ev6_restriction_21b.
 *
 *      In addition, there must be a HW_RET/STALL after a MT
 *      ASN0/MT ASN1 pair. Also, these two writes must be executed atomically, i.e.
 *      either both must retire or neither may retire. 
 *
 *	INPUT PARAMETERS:
 *		raw_code: pointer to the array of instructions from follow_code
 *		offs: where we are in the above array
 *		      Instruction at offs=0 is the pal entry instruction.
 *		caveat_list: pointer to caveat list
 *
 *	OUTPUT PARAMETERS:
 *		None
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:            
 *	AUTHOR/DATE:
 *	V3.24 FLS Sep-96 
 *	V3.25 FLS Mar-97 updated description to remove "must issue"
 *	V3.26 FLS Apr-97 change check from a fixed sequence of instructions.
 ********************************************************************************
 */
{
	int  idtmp=0;
	int i,j;
        int ra;			
	int rb;		
	int id1;
	int found_dtb_asn0=TRUE;
	int found_rc_rb_match=FALSE;
	int found_ra_rb_match=FALSE;
	int found_mt_scbd74=FALSE;
	int error= FALSE;
	int rc;
	int tmp;
        int addr0=0, addr1=0,addr2=0;					/* palcode address of caveat */
        int scbd74_mask = (Scbd7|Scbd6|Scbd5|Scbd4);
	ERR_MSG	*entry;			
        CLASS_INFO *class_def; 
        char *decode0;			/* palcode instruction ascii */
      	
	id1 = Mt + Dtb_asn0;			/* search for DTB_ASN0 */
	while(found_dtb_asn0 == TRUE ) {		
	    if (entry = search_ev6_caveat (id1, caveat_list, addr0)) {
	        addr0 = entry->errnum;		/* get palcode address */	
		decode0 = entry->explanation; 	/* get palcode instruction ascii */	
		found_dtb_asn0=TRUE;
	    } else found_dtb_asn0=FALSE;
	    free (entry);

	/* CASE 1 - Register written to Dtb_asn0 SAME as read from IPR dependent on scbd<7:4> such as VA.
	 * For example: 
	 *		Hw_mfpr ra, EV6__VA     !or other ipr dependent on scbd<7:4>
	 *		...
	 *		Hw_mtpr rb,EV6_DTB_ASN0
	 *
	 * If ra = rb this is okay since the Hw_mtpr will stall 
 	 * waiting for the Hw_mfpr to finish. There is no need for the 
	 * register code dependency scheme required by restriction #21, since 
	 * the registers are the same.
	 * No error will be displayed here, if ra not same as rb
	 * then case 2 will be called to check for the example dependency code.
	 */
	found_ra_rb_match=FALSE;	
	if (found_dtb_asn0==TRUE) {
	    i=0;								  /* raw_code[0]->address = pal_entry */
	    while (raw_code[i] != 0) {   
	        if (raw_code[i]->address == addr0) {
		    rb = raw_code[i]->bits.ev6_hwmxpr_format.rb;		/* get rb from hw_mtpr rb, EV6__DTB_ASN0 */
		    for (j=i; j >= 0 ;j--) {
		        if (raw_code[j]->index == DO_HW_MFPR) {
		    	    tmp = raw_code[j]->bits.ev6_hwmxpr_format.scbd & scbd74_mask; /* get instr. scoreboard  7:4 bits*/
			    if (tmp == scbd74_mask) {			     /* if this ipr uses scoreboard bits <7,6,5,4> */
			        ra = raw_code[j]->bits.ev6_hwmxpr_format.ra; /* then save the register no. */
			        if (ra == rb) 
				    found_ra_rb_match = TRUE;				/* success */
				addr1 = raw_code[j]->address;			    /* save address of Hw_mfpr */
				found_mt_scbd74=TRUE;
			        break;
			    }
			}
		    } /* for (j...) */
	   	} /* if (raw_code...)*/
	         i++;
	    } /* while (raw_code[i] != 0)... */

	/* CASE 2 - Since ra not same as rb , there must be some dependency code instead.
	 * For example:
	 * 		
	 *		Hw_mfpr ra, EV6__VA
	 *		... Since rb not same as ra then need
	 *		... the dependency code similar to that displayed  in error msg 
	 *		... here to force rb dependent on ra.
	 *		... The reason the register dependency has to be contrived 
	 *		... is because the VA is in a register when the routine
	 *		... is entered and not from a Hw_mfpr va, EV6__VA instruction.
	 *		Hw_mtpr rb,EV6_DTB_ASN0
	 *
	 * Here is one code sequence that works:
	 * RB = reg with virtual address on entry 
	 *     Hw_mfpr ra,EV6__VA 			! or any other ipr with scoreboard bits<7:4>=1111 ( clr_map,sleep,va_form)
	 *     xor ra,ra,ra				! clear ra
	 *     bis ra,rc,rc				! 
	 *     nop		   	                ! to force next fetch block to avoid other palcode restriction (29?)
	 *     Hw_mtpr rb, EV6__DTB_ASN0
	 *     Hw_mtpr rb, EV6__DTB_ASN1		! not checked here, will fail restriction 21a if absent
	 * 
	 *  The code will only check that RB = rb. 
	 * addr1:  Hw_mfpr ra,EV6__VA 			! or any other ipr with scoreboard bits<7:4>=1111 ( clr_map,sleep,va_form)
         *	    .
	 *         bis ra,RB,RB				!  check write to RB
         *	    .
	 * addr0:  Hw_mtpr rb, EV6__DTB_ASN0	! check rc = rb
	 *  
	 *  Restriction 21a will check that the  Hw_mtpr rb, EV6__DTB_ASN1 follows, but will not check that its RB is
         *  the same as the RB in Hw_mtpr rb, EV6__DTB_ASN0; it would be overkill.
	 */
	  found_rc_rb_match = FALSE;
	  if (found_ra_rb_match==FALSE && found_mt_scbd74==TRUE) {
	        i=j;						/* j = addr1 from case 1 above */
	        while (raw_code[i] != 0) {   			/* search between addr1 and addr0 */
	   	    if (raw_code[i]->address != addr0) {
		       class_def = instr_array[raw_code[i]->index];
		        if ( class_def->arch->dest_reg == Rc && 
			     raw_code[i]->bits.op_format.rc != 31 ) {			/* is a  write to "RB" */
	           	      rc = raw_code[i]->bits.op_format.rc; 	/* get register to check */
			      if (rc == rb) {
				  found_rc_rb_match = TRUE;				/* success */
		                  break;
			      }
		        } 
	   	     } else 
			   break;						/* stop at Hw_mtpr rb, dtb_asn0 */
	             i++;
	        } /* while ... */
	  } /* if (found_ra_rb_match==FALSE && found_mt_scbd74==TRUE... */

	if (found_ra_rb_match== FALSE && (found_mt_scbd74==FALSE || found_rc_rb_match==FALSE))	
	           error=TRUE;
	else   error=FALSE;

        if (error == TRUE ) {	
	    restriction_num= 21;
	    if (new_error (addr0, restriction_num)) {
	            wr (FLG$ERR, "%s\n", pheader);
	     	    wr (FLG$ERR, "***\n");
	            wr (FLG$ERR, "Error at %X: %s\n",  addr0,decode0);
	    	    wr (FLG$ERR, "(PVC #%d) RESTRICTION #21: \n",restriction_num);
	            wr (FLG$ERR, "          There must be a scoreboard bit to register dependency chain to\n");
	            wr (FLG$ERR, "          prevent Hw_mtpr ASN0 (and Hw_mtpr ASN1) from issuing while ANY of\n");
	            wr (FLG$ERR, "          the scoreboard bits <7:4>  are set. This code dependency is needed\n");
	            wr (FLG$ERR, "          because Hw_mtpr ASN0 will only force a stall on scoreboard bit <6>,\n");
	            wr (FLG$ERR, "          yet should not issue before scoreboard bits <7,5,4> are clear.\n");
	            wr (FLG$ERR, "          NOTE: To force PVC to ignore this error, put the ignore label at\n");
	            wr (FLG$ERR, "                address %x: \n",addr0);
	    	    wr (FLG$ERR, "          Suggestion: Here is a code example to follow: \n");
	            wr (FLG$ERR, "          	        R16 is assumed to contain the VA on entry, EV6__VA\n");
	            wr (FLG$ERR, "          		is only read because it uses scoreboard bits <7:4>.\n");
	    	    wr (FLG$ERR, "                hw_mfpr	r6, EV6__VA		; (4-7, 1L) \n");
	    	    wr (FLG$ERR, "                xor	r6, r6, r6			; clear r6  \n");
	    	    wr (FLG$ERR, "                bis	r6, r16, r16			; force register dependency \n");
	    	    wr (FLG$ERR, "                bis     r31, r31, r31			; force fetch block \n");
	    	    wr (FLG$ERR, "                hw_mtpr	r16, EV6__DTB_ASN0	; (6,0L)  \n");
	    	    wr (FLG$ERR, "                hw_mtpr	r16, EV6__DTB_ASN1	; (7,1L)  \n***\n");
	            pal_error_count++;
	    } /* if (new_error ... */
        } /* if (error == TRUE) */
       } /* if (found_dtb_asn0==TRUE... */
     } /* while ... */

}


void check_ev6_restriction_21a (INSTR *raw_code[], int offs, List *caveat_list)
/*
 ********************************************************************************
 *	PURPOSE:
 *	    Check end of permutation restriction 21a
 *
 * RESTRICTION #21a - HW_MTPR to ASN0 or ASN1 must issue
 *		to both or not at all.
 *
 *	INPUT PARAMETERS:
 *		raw_code: pointer to the array of instructions from follow_code
 *		offs: where we are in the above array
 *		      Instruction at offs=0 is the pal entry instruction.
 *		caveat_list: pointer to caveat list
 *
 *	OUTPUT PARAMETERS:
 *		None
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:            
 *	AUTHOR/DATE:
 *	V3.24 FLS Sep-96 
 ********************************************************************************
 */
{
      int error= FALSE;
      int id1,id2;
      ERR_MSG	*entry;			
      int addr1=0,addr2=0;			/* palcode address of caveat */
      char *decode1, *decode2;		/* palcode instruction ascii */
	
	/* if MT ASN0 found ensure MT ASN1 found.
	   if MT ASN1 found ensure MT ASN0 found.
	*/
	id1 = Mt + Dtb_asn0;		/* search for hw_mtpr Dtb_asn0 */
	id2 = Mt + Dtb_asn1;		/* search for hw_mtpr Dtb_asn1 */

	if (entry = search_ev6_caveat (id1, caveat_list, addr1)) {
		addr1 = entry->errnum;		/* get palcode address */	
		decode1 = entry->explanation; 	/* get palcode instruction ascii */	
	}else addr1=0;
	free (entry);

	if (entry = search_ev6_caveat (id2, caveat_list, addr1)) {
		addr2 = entry->errnum;		
		decode2 = entry->explanation;	
	}else addr2=0;
	free (entry);
	if ((addr1 != 0) && (addr2 == 0))
	        error = TRUE;
	if ((addr2 != 0) && (addr1 == 0))
	        error = TRUE;

        if ((error == TRUE ) && (addr1 != 0)) {	
	    restriction_num= 211;
	    if (new_error (addr1, restriction_num)) {
	            wr (FLG$ERR, "%s\n", pheader);
	     	    wr (FLG$ERR, "***\n");
	            wr (FLG$ERR, "Error at %X: %s\n",  addr1,decode1);
	    	    wr (FLG$ERR, "(PVC #%d) RESTRICTION #21a: \n",restriction_num);
	    	    wr (FLG$ERR, "          A Hw_mtpr ASN0 must be followed by a Hw_mtpr ASN1.\n");
	    	    wr (FLG$ERR, "          Suggestion: Add a Hw_mtpr ASN1 after the Hw_mtpr ASN0 \n");
	    	    wr (FLG$ERR, "                      with no intervening instructions. \n***\n");
	            pal_error_count++;
	    } /* if (new_error ... */
        } /* if (error == TRUE) */

        if ((error == TRUE ) && (addr2 != 0)) {	
	    restriction_num= 211;
	    if (new_error (addr2, restriction_num)) {
	         	wr (FLG$ERR, "%s\n", pheader);
	     	    wr (FLG$ERR, "***\n");
	            wr (FLG$ERR, "Error at %X: %s\n",  addr2,decode2);
	    	    wr (FLG$ERR, "(PVC #%d) RESTRICTION #21a: \n",restriction_num);
	    	    wr (FLG$ERR, "          A Hw_mtpr ASN0 must be followed by a Hw_mtpr ASN1.\n");
	    	    wr (FLG$ERR, "          Suggestion: Add a Hw_mtpr ASN1 after the Hw_mtpr ASN0 \n");
	    	    wr (FLG$ERR, "                      with no intervening instructions. \n***\n");
	            pal_error_count++;
	    } /* if (new_error ... */
        } /* if (error == TRUE) */
}


void check_ev6_restriction_21b (INSTR *raw_code[], int offs, List *caveat_list)
/*
 ********************************************************************************
 *	PURPOSE:
 *	    Check end of permutation restriction 21b
 *
 * RESTRICTION #21b - An HW_MTPR to ASN0 or ASN1 must be
 *     		followed by a  HW_RET_STALL
 *     		HW_JMP_STALL, HW_JSR_STALL, or HW_JCR_STALL.
 *
 *	INPUT PARAMETERS:
 *		raw_code: pointer to the array of instructions from follow_code
 *		offs: where we are in the above array
 *		      Instruction at offs=0 is the pal entry instruction.
 *		caveat_list: pointer to caveat list
 *
 *	OUTPUT PARAMETERS:
 *		None
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:            
 *	AUTHOR/DATE:
 *	V3.24 FLS Sep-96 
 ********************************************************************************
 */
{
	int  idtmp=0;
	int i;
	int id1;
	int addr1=0;			/* palcode address of caveat */
	int found_dtb_asn0=TRUE;
	int found_addr1;
	int found_hwret;
	int found_hwret_stall;
	int index,found_bsr=FALSE;
	int error= FALSE;
	ERR_MSG	*entry;			
	char *decode1;			/* palcode instruction ascii */
      	
	id1 = Mt + Dtb_asn0;		/* search for dtb_asn0 */
	while(found_dtb_asn0 == TRUE ) {		
	    if (entry = search_ev6_caveat (id1, caveat_list, addr1)) {
	        addr1 = entry->errnum;		/* get palcode address */	
		decode1 = entry->explanation; 	/* get palcode instruction ascii */	
		found_dtb_asn0=TRUE;
	    } else found_dtb_asn0=FALSE;
	    free (entry);

	    /* search raw_code[offs] for addr1 then search for hwret/stall or hw_ret */
	    /* note: hw_ret/stall can be either hw_ret_stall, hw_jmp_stall, hw_jsr_stall, or hw_jcr_stall 
	    /* if a hw_ret/stall found first all is well else if hw_ret found then issue error */
	    /* if bsr found then ignore the following hw_ret or hw_jcr, since it is return from bsr and */
	    /* not the end of pal entry flow */
            found_hwret=FALSE;
	    found_hwret_stall=FALSE;
            found_addr1=FALSE;
	    if (found_dtb_asn0==TRUE) {
	        i=0;
      	        while (raw_code[i] != 0) {
	   	    if (raw_code[i]->address == addr1) {
			offs = i+1;
      	    	        while (raw_code[offs] != 0) {		
			    index = raw_code[offs]->index;		     
			    if (index == DO_BSR) 
				found_bsr=TRUE;
		            idtmp = get_inst_id (raw_code[offs]);
	   	            if (idtmp & Hwret_stall) {
		                found_hwret_stall=TRUE;
		                break;
		            }
			    if (idtmp & Hwret) {
			       /* if this is a BSR followed by HW_RET (hint=2) or HW_JCR (hint=3) then okay */
			        if ((raw_code[offs]->bits.ev6_hwret_format.hint == 2) ||
				    (raw_code[offs]->bits.ev6_hwret_format.hint == 3)) {
				       if (found_bsr==TRUE)
				           found_bsr=FALSE;   /* ok, hw_ret or hw_jcr follows a BSR*/
			               else {
				             found_hwret=TRUE;  /* error, found hw_ret, hw_jcr with no  BSR*/
		                             break;	
				            }
				}
		            }
	                    offs++;
	   	        } /* while ... */
	   	     } /* if (raw_code...)*/
	             i++;
	         } /* while ... */

	    if (found_dtb_asn0==TRUE && found_hwret == TRUE && found_hwret_stall==FALSE)	
	           error=TRUE;
	    else   error=FALSE;
	  } /* 	if (found_dtb_asn0==TRUE... */

        if (error == TRUE ) {	
      	    restriction_num= 212;
	    if (new_error (addr1, restriction_num)) {
	            wr (FLG$ERR, "%s\n", pheader);
	     	    wr (FLG$ERR, "***\n");
	            wr (FLG$ERR, "Error at %X: %s\n",  addr1,decode1);
	    	    wr (FLG$ERR, "(PVC #%d) RESTRICTION #21b: \n",restriction_num);
	    	    wr (FLG$ERR, "          A Hw_mtpr ASN0, and Hw_mtpr ASN1 must followed by a Hw_ret_stall,\n");
	    	    wr (FLG$ERR, "          Hw_jmp_stall, or rarely  Hw_jsr_stall, or Hw_jcr_stall.\n");
	    	    wr (FLG$ERR, "          Suggestion: Change the Hw_ret that returns to native mode to \n");
	    	    wr (FLG$ERR, "                      a Hw_ret_stall.\n***\n");
	            pal_error_count++;
	    } /* if (new_error ... */
        } /* if (error == TRUE) */
     } /* while ... */
}

void check_ev6_restriction_22 (INSTR *raw_code[], int offs, List *caveat_list)
/*
 ********************************************************************************
 *	PURPOSE:
 *	    Check end of permutation restriction 22.
 *	    Same as check_ev6_restriction_21 except is0.
 *
 *(22) Restriction: HW_RET/STALL after MT IS0/IS1 
 *
 *   There must be a scoreboard bit -> register dependency chain to prevent MT
 *   IS0 or MT IS1 from issuing while ANY of scoreboard bits <7:4> are set. A
 *   code sequence which accomplishes this: 
 *
 *   HW_MFPR IPR_VA,SCBD<7,6,5,4>,R0 
 *   XOR R0,R0,R0 
 *   BIS R0,R9,R9 
 *   BIS R31,R31,R31 
 *
 *   HW_MTPR R9,IS0,SCBD<6> 
 *   HW_MTPR R9,IS1,SCBD<7> 
 *
 *   This sequence guarantees, through the register dependency on R0, that 
 *   neither HW_MTPR are issued before scoreboard bits <7:4> are cleared.
 *
 * The following parts of the restriction are checked in check_ev6_restriction_22a
 * and check_ev6_restriction_22b.
 *
 *      In addition, there must be a HW_RET/STALL after a MT
 *      IS0/MT IS1 pair. Also, these two writes must be executed atomically, i.e.
 *      either both must retire or neither may retire. 
 *
 *	INPUT PARAMETERS:
 *		raw_code: pointer to the array of instructions from follow_code
 *		offs: where we are in the above array
 *		      Instruction at offs=0 is the pal entry instruction.
 *		caveat_list: pointer to caveat list
 *
 *	OUTPUT PARAMETERS:
 *		None
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:            
 *	AUTHOR/DATE:
 *	V3.24 FLS Sep-96 
 *	V3.25 FLS Mar-97 updated description to remove "must issue"
 *	V3.26 FLS Apr-97 change check from a fixed sequence of instructions.
 ********************************************************************************
 */
{
	int  idtmp=0;
	int i,j;
        int ra;			
	int rb;		
	int id1;
	int found_dtb_is0=TRUE;
	int found_rc_rb_match=FALSE;
	int found_ra_rb_match=FALSE;
	int found_mt_scbd74=FALSE;
	int error= FALSE;
	int rc;
	int tmp;
        int addr0=0, addr1=0,addr2=0;					/* palcode address of caveat */
        int scbd74_mask = (Scbd7|Scbd6|Scbd5|Scbd4);
	ERR_MSG	*entry;			
        CLASS_INFO *class_def; 
        char *decode0;				/* palcode instruction ascii */
      	
	id1 = Mt + Dtb_is0;			/* search for DTB_is0 */
	while(found_dtb_is0 == TRUE ) {		
	    if (entry = search_ev6_caveat (id1, caveat_list, addr0)) {
	        addr0 = entry->errnum;		/* get palcode address */	
		decode0 = entry->explanation; 	/* get palcode instruction ascii */	
		found_dtb_is0=TRUE;
	    } else found_dtb_is0=FALSE;
	    free (entry);

	/* CASE 1 - Register written to Dtb_is0 SAME as read from IPR dependent on scbd<7:4> such as VA.
	 * For example: 
	 *		Hw_mfpr ra, EV6__VA     !or other ipr dependent on scbd<7:4>
	 *		...
	 *		Hw_mtpr rb,EV6_DTB_is0
	 *
	 * If ra = rb this is okay since the Hw_mtpr will stall 
 	 * waiting for the Hw_mfpr to finish. There is no need for the 
	 * register code dependency scheme required by restriction #22, since 
	 * the registers are the same.
	 * No error will be displayed here, if ra not same as rb
	 * then case 2 will be called to check for the example dependency code.
	 */
	found_ra_rb_match=FALSE;	
	if (found_dtb_is0==TRUE) {
	    i=0;								  /* raw_code[0]->address = pal_entry */
	    while (raw_code[i] != 0) {   
	        if (raw_code[i]->address == addr0) {
		    rb = raw_code[i]->bits.ev6_hwmxpr_format.rb;		/* get rb from hw_mtpr rb, EV6__DTB_is0 */
		    for (j=i; j >= 0 ;j--) {
		        if (raw_code[j]->index == DO_HW_MFPR) {
		    	    tmp = raw_code[j]->bits.ev6_hwmxpr_format.scbd & scbd74_mask; /* get instr. scoreboard  7:4 bits*/
			    if (tmp == scbd74_mask) {			     /* if this ipr uses scoreboard bits <7,6,5,4> */
			        ra = raw_code[j]->bits.ev6_hwmxpr_format.ra; /* then save the register no. */
			        if (ra == rb) 
				    found_ra_rb_match = TRUE;				/* success */
				addr1 = raw_code[j]->address;			    /* save address of Hw_mfpr */
				found_mt_scbd74=TRUE;
			        break;
			    }
			}
		    } /* for (j...) */
	   	} /* if (raw_code...)*/
	         i++;
	    } /* while (raw_code[i] != 0)... */

	/* CASE 2 - Since ra not same as rb , there must be some dependency code instead.
	 * For example:
	 * 		
	 *		Hw_mfpr ra, EV6__VA
	 *		... Since rb not same as ra then need
	 *		... the dependency code similar to that displayed  in error msg 
	 *		... here to force rb dependent on ra.
	 *		... The reason the register dependency has to be contrived 
	 *		... is because the VA is in a register when the routine
	 *		... is entered and not from a Hw_mfpr va, EV6__VA instruction.
	 *		Hw_mtpr rb,EV6_DTB_is0
	 *
	 * Here is one code sequence that works:
	 * RB = reg with virtual address on entry 
	 *     Hw_mfpr ra,EV6__VA 			! or any other ipr with scoreboard bits<7:4>=1111 ( clr_map,sleep,va_form)
	 *     xor ra,ra,ra				! clear ra
	 *     bis ra,rc,rc				! 
	 *     nop		   	                ! to force next fetch block to avoid other palcode restriction (29?)
	 *     Hw_mtpr rb, EV6__DTB_is0
	 *     Hw_mtpr rb, EV6__DTB_is1		! not checked here, will fail restriction 22a if absent
	 * 
	 *  The code will only check that RB = rb. 
	 * addr1:  Hw_mfpr ra,EV6__VA 			! or any other ipr with scoreboard bits<7:4>=1111 ( clr_map,sleep,va_form)
         *	    .
	 *         bis ra,RB,RB				!  check write to RB
         *	    .
	 * addr0:  Hw_mtpr rb, EV6__DTB_is0	! check rc = rb
	 *  
	 *  Restriction 22a will check that the  Hw_mtpr rb, EV6__DTB_is1 follows, but will not check that its RB is
         *  the same as the RB in Hw_mtpr rb, EV6__DTB_is0; it would be overkill.
	 */
	  found_rc_rb_match = FALSE;
	  if (found_ra_rb_match==FALSE && found_mt_scbd74==TRUE) {
	        i=j;						/* j = addr1 from case 1 above */
	        while (raw_code[i] != 0) {   			/* search between addr1 and addr0 */
	   	    if (raw_code[i]->address != addr0) {
		       class_def = instr_array[raw_code[i]->index];
		        if ( class_def->arch->dest_reg == Rc && 
			     raw_code[i]->bits.op_format.rc != 31 ) {			/* is a  write to "RB" */
	           	      rc = raw_code[i]->bits.op_format.rc; 	/* get register to check */
			      if (rc == rb) {
				  found_rc_rb_match = TRUE;				/* success */
		                  break;
			      }
		        } 
	   	     } else 
			   break;						/* stop at Hw_mtpr rb, dtb_is0 */
	             i++;
	        } /* while ... */
	  } /* if (found_ra_rb_match==FALSE && found_mt_scbd74==TRUE... */

	if (found_ra_rb_match== FALSE && (found_mt_scbd74==FALSE || found_rc_rb_match==FALSE))	
	           error=TRUE;
	else   error=FALSE;

        if (error == TRUE ) {	
	    restriction_num= 22;
	    if (new_error (addr0, restriction_num)) {
	            wr (FLG$ERR, "%s\n", pheader);
	     	    wr (FLG$ERR, "***\n");
	            wr (FLG$ERR, "Error at %X: %s\n",  addr0,decode0);
	    	    wr (FLG$ERR, "(PVC #%d) RESTRICTION #22: \n",restriction_num);
	            wr (FLG$ERR, "          There must be a scoreboard bit to register dependency chain to\n");
	            wr (FLG$ERR, "          prevent Hw_mtpr is0 (and Hw_mtpr is1) from issuing while ANY of\n");
	            wr (FLG$ERR, "          the scoreboard bits <7:4>  are set. This code dependency is needed\n");
	            wr (FLG$ERR, "          because Hw_mtpr is0 will only force a stall on scoreboard bit <6>,\n");
	            wr (FLG$ERR, "          yet should not issue before scoreboard bits <7,5,4> are clear.\n");
	            wr (FLG$ERR, "          NOTE: To force PVC to ignore this error, put the ignore label at\n");
	            wr (FLG$ERR, "                address %x: \n",addr0);
	    	    wr (FLG$ERR, "          Suggestion: Here is a code example to follow: \n");
	            wr (FLG$ERR, "          	        R16 is assumed to contain the VA on entry, EV6__VA\n");
	            wr (FLG$ERR, "          		is only read because it uses scoreboard bits <7:4>.\n");
	    	    wr (FLG$ERR, "                hw_mfpr	r6, EV6__VA		; (4-7, 1L) \n");
	    	    wr (FLG$ERR, "                xor	r6, r6, r6			; clear r6  \n");
	    	    wr (FLG$ERR, "                bis	r6, r16, r16			; force register dependency \n");
	    	    wr (FLG$ERR, "                bis     r31, r31, r31			; force fetch block \n");
	    	    wr (FLG$ERR, "                hw_mtpr	r16, EV6__DTB_IS0	; (6,0L)  \n");
	    	    wr (FLG$ERR, "                hw_mtpr	r16, EV6__DTB_IS1	; (7,1L)  \n***\n");
	            pal_error_count++;
	    } /* if (new_error ... */
        } /* if (error == TRUE) */
       } /* if (found_dtb_is0==TRUE... */
     } /* while ... */

}

void check_ev6_restriction_22a (INSTR *raw_code[], int offs, List *caveat_list)
/*
 ********************************************************************************
 *	PURPOSE:
 *	    Check end of permutation restriction 22a
 *
 * RESTRICTION #22a - HW_MTPR to IS0 or IS1 must issue
 *		to both or not at all.
 *
 *	INPUT PARAMETERS:
 *		raw_code: pointer to the array of instructions from follow_code
 *		offs: where we are in the above array
 *		      Instruction at offs=0 is the pal entry instruction.
 *		caveat_list: pointer to caveat list
 *
 *	OUTPUT PARAMETERS:
 *		None
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:            
 *	AUTHOR/DATE:
 *	V3.24 FLS Sep-96 
 ********************************************************************************
 */
{
      int error= FALSE;
      int id1,id2;
      ERR_MSG	*entry;			
      int addr1=0,addr2=0;			/* palcode address of caveat */
      char *decode1, *decode2;		/* palcode instruction ascii */
	
	/* if MT IS0 found ensure MT IS1 found.
	   if MT IS1 found ensure MT IS0 found.
	*/
	id1 = Mt + Dtb_is0;		/* search for hw_mtpr Dtb_is0 */
	id2 = Mt + Dtb_is1;		/* search for hw_mtpr Dtb_is1 */

	if (entry = search_ev6_caveat (id1, caveat_list, addr1)) {
		addr1 = entry->errnum;		/* get palcode address */	
		decode1 = entry->explanation; 	/* get palcode instruction ascii */	
	}else addr1=0;
	free (entry);

	if (entry = search_ev6_caveat (id2, caveat_list, addr1)) {
		addr2 = entry->errnum;		
		decode2 = entry->explanation;	
	}else addr2=0;
	free (entry);
	if ((addr1 != 0) && (addr2 == 0))
	        error = TRUE;
	if ((addr2 != 0) && (addr1 == 0))
	        error = TRUE;

        if ((error == TRUE ) && (addr1 != 0)) {	
	    restriction_num= 221;
	    if (new_error (addr1, restriction_num)) {
	            wr (FLG$ERR, "%s\n", pheader);
	     	    wr (FLG$ERR, "***\n");
	            wr (FLG$ERR, "Error at %X: %s\n",  addr1,decode1);
	    	    wr (FLG$ERR, "(PVC #%d) RESTRICTION #22a: \n",restriction_num );
	    	    wr (FLG$ERR, "          A Hw_mtpr IS0 must be followed by a Hw_mtpr IS1.\n");
	    	    wr (FLG$ERR, "          Suggestion: Add a Hw_mtpr IS1 after the Hw_mtpr IS0 \n");
	    	    wr (FLG$ERR, "                      with no intervening instructions. \n***\n");
	            pal_error_count++;
	    } /* if (new_error ... */
        } /* if (error == TRUE) */

        if ((error == TRUE ) && (addr2 != 0)) {	
	   restriction_num= 221;
	    if (new_error (addr2, restriction_num)) {
	         	wr (FLG$ERR, "%s\n", pheader);
	     	    wr (FLG$ERR, "***\n");
	            wr (FLG$ERR, "Error at %X: %s\n",  addr2,decode2);
	    	    wr (FLG$ERR, "(PVC #%d) RESTRICTION #22a: \n",restriction_num);
	    	    wr (FLG$ERR, "          A Hw_mtpr IS0 must be followed by a Hw_mtpr IS1.\n");
	    	    wr (FLG$ERR, "          Suggestion: Add a Hw_mtpr IS1 after the Hw_mtpr IS0 \n");
	    	    wr (FLG$ERR, "                      with no intervening instructions. \n***\n");
	            pal_error_count++;
	    } /* if (new_error ... */
        } /* if (error == TRUE) */
}


void check_ev6_restriction_22b (INSTR *raw_code[], int offs, List *caveat_list)
/*
 ********************************************************************************
 *	PURPOSE:
 *	    Check end of permutation restriction 22b
 *
 * RESTRICTION #22b - An HW_MTPR to IS0 or IS1 must be
 *     		followed by a  HW_RET_STALL
 *     		HW_JMP_STALL, HW_JSR_STALL, or HW_JCR_STALL.
 *
 *	INPUT PARAMETERS:
 *		raw_code: pointer to the array of instructions from follow_code
 *		offs: where we are in the above array
 *		      Instruction at offs=0 is the pal entry instruction.
 *		caveat_list: pointer to caveat list
 *
 *	OUTPUT PARAMETERS:
 *		None
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:            
 *	AUTHOR/DATE:
 *	V3.24 FLS Sep-96 
 ********************************************************************************
 */
{
	int  idtmp=0;
	int i;
	int id1;
	int addr1=0;			/* palcode address of caveat */
	int found_dtb_is0=TRUE;
	int found_addr1;
	int found_hwret;
	int found_hwret_stall;
	int index,found_bsr=FALSE;
	int error= FALSE;
	ERR_MSG	*entry;			
	char *decode1;			/* palcode instruction ascii */
      	
	id1 = Mt + Dtb_is0;		/* search for dtb_is0 */
	while(found_dtb_is0 == TRUE ) {		
	    if (entry = search_ev6_caveat (id1, caveat_list, addr1)) {
	        addr1 = entry->errnum;		/* get palcode address */	
		decode1 = entry->explanation; 	/* get palcode instruction ascii */	
		found_dtb_is0=TRUE;
	    } else found_dtb_is0=FALSE;
	    free (entry);

	    /* search raw_code[offs] for addr1 then search for hwret/stall or hw_ret */
	    /* note: hw_ret/stall can be either hw_ret_stall, hw_jmp_stall, hw_jsr_stall, or hw_jcr_stall 
	    /* if a hw_ret/stall found first all is well else if hw_ret found then issue error */
	    /* if bsr found then ignore the following hw_ret or hw_jcr, since it is return from bsr and */
	    /* not the end of pal entry flow */
            found_hwret=FALSE;
	    found_hwret_stall=FALSE;
            found_addr1=FALSE;
	    if (found_dtb_is0==TRUE) {
	        i=0;
      	        while (raw_code[i] != 0) {
	   	    if (raw_code[i]->address == addr1) {
			offs = i+1;
      	    	        while (raw_code[offs] != 0) {		
			    index = raw_code[offs]->index;		     
			    if (index == DO_BSR) 
				found_bsr=TRUE;
		            idtmp = get_inst_id (raw_code[offs]);
	   	            if (idtmp & Hwret_stall) {
		                found_hwret_stall=TRUE;
		                break;
		            }
			    if (idtmp & Hwret) {
			       /* if this is a BSR followed by HW_RET (hint=2) or HW_JCR (hint=3) then okay */
			        if ((raw_code[offs]->bits.ev6_hwret_format.hint == 2) ||
				    (raw_code[offs]->bits.ev6_hwret_format.hint == 3)) {
				       if (found_bsr==TRUE)
				           found_bsr=FALSE;   /* ok, hw_ret or hw_jcr follows a BSR*/
			               else {
				             found_hwret=TRUE;  /* error, found hw_ret, hw_jcr with no  BSR*/
		                             break;	
				            }
				}
		            }
	                    offs++;
	   	        } /* while ... */
	   	     } /* if (raw_code...)*/
	             i++;
	         } /* while ... */

	    if (found_dtb_is0==TRUE && found_hwret == TRUE && found_hwret_stall==FALSE)	
	           error=TRUE;
	    else   error=FALSE;
	  } /* 	if (found_dtb_is0==TRUE... */
 
        if (error == TRUE ) {	
      	    restriction_num= 222;
	    if (new_error (addr1, restriction_num)) {
	         	wr (FLG$ERR, "%s\n", pheader);
	     	    wr (FLG$ERR, "***\n");
	            wr (FLG$ERR, "Error at %X: %s\n",  addr1,decode1);
	    	    wr (FLG$ERR, "(PVC #%d) RESTRICTION #22b:\n",restriction_num);
	    	    wr (FLG$ERR, "          A Hw_mtpr IS0, and Hw_mtpr IS1 must followed by a Hw_ret_stall,\n");
	    	    wr (FLG$ERR, "          Hw_jmp_stall, or rarely  Hw_jsr_stall, or Hw_jcr_stall.\n");
	    	    wr (FLG$ERR, "          Suggestion: Change the Hw_ret that returns to native mode to \n");
	    	    wr (FLG$ERR, "                      a Hw_ret_stall.\n***\n");
	            pal_error_count++;
	    } /* if (new_error ... */
        } /* if (error == TRUE) */
     } /* while ... */
}


void check_ev6_restriction_23 (INSTR *raw_code[], int offs, List *caveat_list)
/*
 ********************************************************************************
 *	PURPOSE:
 *	    Check end of permutation restriction 23.
 *
 * PVC checking of this restriction boils down to checking that
 * between the last HW_ST/P/CONDITIONAL (aka hw_sxtpc) and the HW_RET
 * is a load or store instruction.
 * Here is an example acceptable case:
 *
 *	hw_stxpc rx,(ry)		; hw_st physical conditional
 *	  ...
 *	  stq ra,(rb) 		; store instruction 
 *	  ...
 *	hw_ret_stall		; return to native mode
 *
 *
 * Here is the actual restriction:
 *
 *(23) Restriction: HW_ST/P/CONDITIONAL does not "clear" the
 *lock flag 
 *
 *   A HW_ST/P/CONDITIONAL will not "clear" the lock flag such that a
 *   successive store-conditional (either STx_C or HW_ST/C) might succeed even
 *   in the absence of a load-locked instruction. In EV6 a store-conditional is
 *   forced to fail if there is an intervening memory operation between the
 *   store-conditional and its address-matching LDxL. The memory operations
 *   are: 
 *
 *
 *   LDL/Q/F/G/S/T 
 *   STL/Q/F/G/S/T 
 *   LDQ_U (not to R31) 
 *   STQ_U 
 *
 *   Absent from this list are HW_LD (any type), HW_ST (any type), ECB, and
 *   WH64. Their absence implies that they will NOT force a subsequent
 *   store-conditional instruction to fail. PALcode MUST insert a memory
 *   operation from the above list after a HW_ST/CONDITIONAL in order to
 *   force a future store-conditional to fail if it was not preceded by a
 *   load-locked: 
 *
 *
 *   HW_LD/L 
 *   xxx 
 *   HW_ST/C -> R0 
 *   Bxx R0, try_again 
 *   STQ 			; force next ST/C to fail if no preceding LDxL 
 *   HW_RET 
 *
 *	INPUT PARAMETERS:
 *		raw_code: pointer to the array of instructions from follow_code
 *		offs: where we are in the above array
 *		      Instruction at offs=0 is the pal entry instruction.
 *		caveat_list: pointer to caveat list
 *
 *	OUTPUT PARAMETERS:
 *		None
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:            
 *	AUTHOR/DATE:
 *	V3.24 FLS Sep-96 
 ********************************************************************************
 */
{
	int  idtmp=0;
	int i;
	int id1;
	int addr1=0;			/* palcode address of caveat */
	int last_addr1=0;		
	int found_hw_stpc=TRUE;
	int found_hwret=FALSE;
	int found_ldst=FALSE;
	int found_hwret_stall=FALSE;
	int index,found_bsr=FALSE;
	int error=FALSE;
	ERR_MSG	*entry;			
	char *decode1;			/* palcode instruction ascii */
      	
	id1 = Hw_stpc;			/* search for  Hw_stpc*/
	/* Find the last hw_stpc in this pal flow */

	while(found_hw_stpc == TRUE ) {		
	    last_addr1 = addr1;
	    if (entry = search_ev6_caveat (id1, caveat_list, addr1)) {
	        addr1 = entry->errnum;		/* get palcode address */	
		decode1 = entry->explanation; 	/* get palcode instruction ascii */	
		found_hw_stpc=TRUE;
	    } else found_hw_stpc=FALSE;
	    free (entry);
        } /* while ... */
	
          addr1 = last_addr1;		/* set addr1 to address of last hw_stpc */

	    /* search raw_code[offs] for addr1 then search for hwret/stall or hw_ret */
	    /* note: hw_ret/stall can be either hw_ret_stall, hw_jmp_stall, hw_jsr_stall, or hw_jcr_stall 
	    /* if a hw_ret or hw_ret/stall found before ld/st then issue error, because after */
	    /* a return to native mode, a stx_c instruction could be executed*/
	    /* if a hw_ret was preceded by a BSR it is not an error, sicne not returning to native mode */
	    if (last_addr1 != 0) {
	        i=0;
      	        while (raw_code[i] != 0) {
	   	    if (raw_code[i]->address == addr1) {
			offs = i+1;
      	    	        while (raw_code[offs] != 0) {		
			    index = raw_code[offs]->index;		     
			    if (index == DO_BSR) 
				found_bsr=TRUE;
		            idtmp = get_inst_id (raw_code[offs]);
	   	            if (idtmp & Hwret_stall) {
		                found_hwret_stall=TRUE;		/* error */
		                break;
		            }
			    if (idtmp & Hwret) {
			       /* if this is a BSR followed by HW_RET (hint=2) or HW_JCR (hint=3) then okay */
			        if ((raw_code[offs]->bits.ev6_hwret_format.hint == 2) ||
				    (raw_code[offs]->bits.ev6_hwret_format.hint == 3)) {
				       if (found_bsr==TRUE)
				           found_bsr=FALSE;   /* ok, hw_ret or hw_jcr follows a BSR*/
			               else {
				             found_hwret=TRUE;  /* error, found hw_ret, hw_jcr with no  BSR*/
		                             break;	
				            }
				}
		            }
	        	    if   (((idtmp == (Memory_op | Virtual_inst | Read))  && (raw_code[offs]->index != DO_HW_LD)) ||
	             		  ((idtmp == (Memory_op | Virtual_inst | Write)) && (raw_code[offs]->index != DO_HW_ST)))  {
				if ( (raw_code[offs]->index != DO_LDQU) || 
				     ((raw_code[offs]->index == DO_LDQU) && (raw_code[offs]->bits.mem_format.ra !=31)) ) {
		                        found_ldst=TRUE;	/* no error, ldst found first */
		                        break;
				}
		            }
	                    offs++;
	   	        } /* while ... */
	   	     } /* if (raw_code...)*/
	             i++;
	         } /* while ... */

	    if (found_ldst==FALSE || found_hwret == TRUE || found_hwret_stall==TRUE)
	           error=TRUE;
	    else   error=FALSE;
	  } /* 	if (found_hw_stpc==TRUE... */

        if (error == TRUE ) {	
      	    restriction_num= 23;
	    if (new_error (addr1, restriction_num)) {
	         	wr (FLG$ERR, "%s\n", pheader);
	     	    wr (FLG$ERR, "***\n");
	            wr (FLG$ERR, "Error at %X: %s\n",  addr1,decode1);
	    	    wr (FLG$ERR, "(PVC #%d) RESTRICTION #23: \n",restriction_num);
	    	    wr (FLG$ERR, "           A Hw_stxpc does not clear the per cpu lock flag. There must be\n");
	    	    wr (FLG$ERR, "           a memory operation after the Hw_stxpc to clear the flag.\n");
	    	    wr (FLG$ERR, "           Acceptable memory operations include LDx, Stx, Ldq_u, and \n");
	    	    wr (FLG$ERR, "           Stq_u instructions.  Hw_ld, Hw_st, ecb, and Wh64 are \n");
	    	    wr (FLG$ERR, "           unacceptable as memory operations for this restriction.\n");
	    	    wr (FLG$ERR, "           Suggestion: Add a Load or store instruction after\n");
	    	    wr (FLG$ERR, "                       the Hw_stxpc to clear the flag.\n***\n");
	            pal_error_count++;
	    } /* if (new_error ... */
        } /* if (error == TRUE) */
}


void check_ev6_restriction_24ab (INSTR *raw_code[], int offs, List *caveat_list)
/*
 ********************************************************************************
 *	PURPOSE:
 *	    Check the first and second part of restriction 24 for IC_FLUSH and
 *	    IC_FLUSH_ASM.  CLEAR_MAP is checked in check_ev6_restriction_24c.
 *
 * RESTRICTION #24a:
 *		An HW_MTPR to IC_FLUSH or IC_FLUSH_ASM must be
 *     		followed by a  HW_RET_STALL
 *     		HW_JMP_STALL, HW_JSR_STALL, or HW_JCR_STALL.
 * RESTRICTION #24b:
 *		In addition, it must be
 *	   	guaranteed that the MT IC_FLUSH or IC_FLUSH_ASM will not
 *	   	retire simultaneously with the HW_RET/STALL. 
 *
 * note: pvc cannot check for valid destinations (static code checker only) 
 *	 in the 10 instructions between the IC_FLUSH and hw_ret/stall.
 *
 *	(24) Restriction: HW_RET/STALL after MT IC_FLUSH,
 *		IC_FLUSH_ASM, CLEAR_MAP
 *	
 *	   There must be a HW_RET/STALL after a HW_MTPR to any of
 *	   these IPRs. The Icache flush associated with these instructions will
 *	   not occur until the HW_RET stall occurs and all outstanding
 *	   I-stream fetches have been completed. In addition, it must be
 *	   guaranteed that the MT IC_FLUSH or IC_FLUSH_ASM will not
 *	   retire simultaneously with the HW_RET/STALL. This can be
 *	   insured by inserting a conditional branch between the two (BNE
 *	   R31, 0; cannot mispredict in palmode), or by insuring at least 10
 *	   instructions between the MT and HW_RET/STALL containing at
 *	   least one instruction in each quad aligned group with a valid
 *	   destination. Finally, the HW_RET/STALL that is used for
 *	   CLEAR_MAP may not also trigger a cache flush. That is, if both a
 *	   CLEAR MAP and IC FLUSH are desired, there must be two
 *	   HW_RET/STALLS (one following each MTPR). 
 *
 *
 *	INPUT PARAMETERS:
 *		raw_code: pointer to the array of instructions from follow_code
 *		offs: where we are in the above array
 *		      Instruction at offs=0 is the pal entry instruction.
 *		caveat_list: pointer to caveat list
 *
 *	OUTPUT PARAMETERS:
 *		None
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:            
 *	AUTHOR/DATE:
 *	V3.24 FLS Sep-96 
 *      V3.25 FLS Mar-97 Updated per pal restriction change date 3/20/97
 ********************************************************************************
 */
{
	int  idtmp=0;
	int i;
	int id1;
	int addr1;			/* palcode address of caveat */
	int found_ic_flush=TRUE;
	int found_addr1;
	int found_hwret;
	int found_hwret_stall;
	int index;
	int found_bsr=FALSE;
	int found_br_cond=FALSE;		/* found conditional branch */
	int found_ten_inst=FALSE;	/* found ten instructions */
	int error_a= FALSE;
	int error_b= FALSE;
	int count_inst=0;
	ERR_MSG	*entry;			
	char *decode1;			/* palcode instruction ascii */
      	
/* 24a check */
	found_ic_flush=TRUE;		/* prime the pump */
	id1 = Mt + Ic_flush;		/* search for Ic_flush or Ic_flush_asm see add_if_ev6_caveat in ev6.c */
	addr1=0;			/* start search at pal entry address */
	while(found_ic_flush == TRUE ) {		
	    if (entry = search_ev6_caveat (id1, caveat_list, addr1)) {
	        addr1 = entry->errnum;		/* get palcode address */	
		decode1 = entry->explanation; 	/* get palcode instruction ascii */	
		found_ic_flush=TRUE;
	    } else found_ic_flush=FALSE;
	    free (entry);

	    /* search raw_code[offs] for addr1 then search for hwret/stall or hw_ret */
	    /* note: hw_ret/stall can be either hw_ret_stall, hw_jmp_stall, hw_jsr_stall, or hw_jcr_stall 
	    /* if a hw_ret/stall found first all is well else if hw_ret found then issue error */
	    /* if bsr found then ignore the following hw_ret or hw_jcr, since it is return from bsr and */
	    /* not the end of pal entry flow */
            found_hwret=FALSE;
	    found_hwret_stall=FALSE;
            found_addr1=FALSE;
	    if (found_ic_flush==TRUE) {
	        i=0;
      	        while (raw_code[i] != 0) {
	   	    if (raw_code[i]->address == addr1) {
			offs = i+1;
      	    	        while (raw_code[offs] != 0) {		
			    index = raw_code[offs]->index;		     
			    if (index == DO_BSR) 
				found_bsr=TRUE;
		            idtmp = get_inst_id (raw_code[offs]);
	   	            if (idtmp & Hwret_stall) {
		                found_hwret_stall=TRUE;
		                break;
		            }
			    if (idtmp & Hwret) {
			       /* if this is a BSR followed by HW_RET (hint=2) or HW_JCR (hint=3) then okay */
			        if ((raw_code[offs]->bits.ev6_hwret_format.hint == 2) ||
				    (raw_code[offs]->bits.ev6_hwret_format.hint == 3)) {
				       if (found_bsr==TRUE)
				           found_bsr=FALSE;   /* ok, hw_ret or hw_jcr follows a BSR*/
			               else {
				             found_hwret=TRUE;  /* error, found hw_ret, hw_jcr with no  BSR*/
		                             break;	
				            }
				}
		            }
	                    offs++;
	   	        } /* while ... */
	   	     } /* if (raw_code...)*/
	             i++;
	         } /* while ... */

	    if (found_ic_flush==TRUE && found_hwret == TRUE && found_hwret_stall==FALSE)	
	           error_a=TRUE;
	    else   error_a=FALSE;
	    found_ic_flush = FALSE;
	  } /* 	if (found_ic_flush==TRUE... */


          if (error_a == TRUE ) {	
	    restriction_num= 241;
	    if (new_error (addr1, restriction_num)) {
	            wr (FLG$ERR, "%s\n", pheader);
	     	    wr (FLG$ERR, "***\n");
	            wr (FLG$ERR, "Error at %X: %s\n",  addr1,decode1);
	    	    wr (FLG$ERR, "(PVC #%d) RESTRICTION #24a: \n",restriction_num);
	    	    wr (FLG$ERR, "          A Hw_mtpr IC_FLUSH or IC_FLUSH_ASM must be followed by \n");
	    	    wr (FLG$ERR, "          a Hw_ret_stall, Hw_jmp_stall, Hw_jsr_stall, or Hw_jcr_stall\n");
	    	    wr (FLG$ERR, "          The Icache flush associated with these instructions will not\n");
	    	    wr (FLG$ERR, "          occur until the stall instruction that follows retires.\n");
	    	    wr (FLG$ERR, "          Suggestion: Here is a sample code example: \n");
	    	    wr (FLG$ERR, "                    hw_mtpr r31, EV6__IC_FLUSH  ;\n");
	    	    wr (FLG$ERR, "                    bne     r31, .               ;conditional branch\n");
	    	    wr (FLG$ERR, "                    hw_ret_stall    (r20)       ;\n");
	            pal_error_count++;
	    } /* if (new_error ... */
          } /* if (error_a == TRUE) */
	} /* while(found_ic_flush == TRUE... */


/* 24b check */
     if (error_a == FALSE) {
	found_ic_flush=TRUE;		/* prime the pump */
	id1 = Mt + Ic_flush;		/* search for Ic_flush or Ic_flush_asm see add_if_ev6_caveat in ev6.c */
	addr1=0;			/* start search at pal entry address */
	while(found_ic_flush == TRUE ) {		
	    if (entry = search_ev6_caveat (id1, caveat_list, addr1)) {
	        addr1 = entry->errnum;		/* get palcode address */	
		decode1 = entry->explanation; 	/* get palcode instruction ascii */	
		found_ic_flush=TRUE;
	    } else found_ic_flush=FALSE;
	    free (entry);

	    /* 
	     * Search raw_code[offs] for addr1 then search for hwret/stall or hw_ret 
	     * this search will succeed, since it was already done in error_a above.
	     * Success scenario #1:
	     *    hw_mtpr r31, IC_FLUSH   ; or IC_FLUSH_ASM
	     *    bne	r31,0		  ; or any conditional br. (class qi_ibr)
	     *		    		  ; bne r31,0 is a good choice since it will always predict correctly
	     *		    		  ; since in palcode cond branches predict fall thru.
	     *    Hw_ret_stall		  ; or hw_jmp_stall, hw_jsr_stall, or hw_jcr_stall
	     *
	     * Success scenario #2:
	     *    hw_mtpr r31, IC_FLUSH   ; or IC_FLUSH_ASM
	     *
	     *       at least
	     *       10 instructions. 	  ; in each fetch block (quad group) there must be at least
	     *				  ; one instruction with a valid destination (no dstream miss)
	     *				  ; pvc cannot check for valid destinations (static code checker only).
	     *    Hw_ret_stall		  ; or hw_jmp_stall, hw_jsr_stall, or hw_jcr_stall
	     */

            found_hwret=FALSE;
	    found_hwret_stall=FALSE;
            found_addr1=FALSE;
            found_ten_inst=FALSE;
            found_br_cond=FALSE;
	    if (found_ic_flush==TRUE) {
	        i=0;
		count_inst=0;
      	        while (raw_code[i] != 0) {
	   	    if (raw_code[i]->address == addr1) {
			offs = i+1;
      	    	        while (raw_code[offs] != 0) {		
		     	    count_inst++;
			    index = raw_code[offs]->index;		     
			    if (index == DO_BSR) 
				found_bsr=TRUE;
		            idtmp = get_inst_id (raw_code[offs]);
	   	            if (idtmp & Br_cond) {			/* found scenario #1 */
		                found_br_cond=TRUE;
		                break;
		            }
	   	            if (idtmp & Hwret_stall) {
		                found_hwret_stall=TRUE;
		                break;
		            }
			    if (idtmp & Hwret) {
			       /* if this is a BSR followed by HW_RET (hint=2) or HW_JCR (hint=3) then okay */
			        if ((raw_code[offs]->bits.ev6_hwret_format.hint == 2) ||
				    (raw_code[offs]->bits.ev6_hwret_format.hint == 3)) {
				       if (found_bsr==TRUE)
				           found_bsr=FALSE;   /* ok, hw_ret or hw_jcr follows a BSR*/
			               else {
				             found_hwret=TRUE;  /* error, found hw_ret, hw_jcr with no  BSR*/
		                             break;	
				            }
				}
		            }
	                    offs++;
      	    	        } /* while (raw_code[offs]... */
	   	     } /* if (raw_code...)*/
	             i++;
	         } /* while ... */
		 if (count_inst >= 10) 			/* if 10 inst between ic_flush and hw_ret/stall */
			found_ten_inst = TRUE;

	    /* Since error_a has already checked for hw_ret/stall following the ic_flush, we don't */
	    /* have to repeat the check here. */
	    if (found_br_cond == FALSE && found_ten_inst == FALSE)	
	           error_b=TRUE;
	    else   error_b=FALSE;
	    found_ic_flush=FALSE;
	  } /* 	if (found_ic_flush==TRUE... */

          if (error_b == TRUE ) {	
	    restriction_num= 242;
	    if (new_error (addr1, restriction_num)) {
	            wr (FLG$ERR, "%s\n", pheader);
	     	    wr (FLG$ERR, "***\n");
	            wr (FLG$ERR, "Error at %X: %s\n",  addr1,decode1);
	    	    wr (FLG$ERR, "(PVC #%d) RESTRICTION #24b: \n",restriction_num);
	    	    wr (FLG$ERR, "          A Hw_mtpr IC_FLUSH or IC_FLUSH_ASM must be followed by \n");
	    	    wr (FLG$ERR, "          conditional branch or at least 10 instructions between\n");
	    	    wr (FLG$ERR, "          the Hw_mtpr and the hw_ret_stall, Hw_jmp_stall, Hw_jsr_stall,\n");
	    	    wr (FLG$ERR, "          or Hw_jcr_stall.\n");
	    	    wr (FLG$ERR, "          Suggestion: Here is a sample code example: \n");
	    	    wr (FLG$ERR, "                    hw_mtpr r31, EV6__IC_FLUSH  ;\n");
	    	    wr (FLG$ERR, "                    bne     r31, .               ;conditional branch\n");
	    	    wr (FLG$ERR, "                    hw_ret_stall    (r20)       ;\n");
	    	    wr (FLG$ERR, "\n***\n");
	            pal_error_count++;
	    } /* if (new_error ... */
          } /* if (error_b == TRUE) */
	} /* while(found_ic_flush == TRUE... */
     } /* if (error_a == FALSE.... */

}

void check_ev6_restriction_24c (INSTR *raw_code[], int offs, List *caveat_list)
/*
 ********************************************************************************
 *	PURPOSE:
 *	    Check end of permutation restriction 24c
 *
 * See check_ev6_restriction_24a comments for the ev6 spec description of
 * restriction 24.
 *
 * RESTRICTION #24c - An HW_MTPR to CLEAR_MAP must be
 *     		followed by a  HW_RET_STALL
 *     		HW_JMP_STALL, HW_JSR_STALL, or HW_JCR_STALL.
 *
 *	INPUT PARAMETERS:
 *		raw_code: pointer to the array of instructions from follow_code
 *		offs: where we are in the above array
 *		      Instruction at offs=0 is the pal entry instruction.
 *		caveat_list: pointer to caveat list
 *
 *	OUTPUT PARAMETERS:
 *		None
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:            
 *	AUTHOR/DATE:
 *	V3.24 FLS Sep-96 
 *      V3.25 FLS Mar-97 Updated per pal restriction change date 3/20/97
 ********************************************************************************
 */
{
	int  idtmp=0;
	int i;
	int id1;
	int addr1=0;			/* palcode address of caveat */
	int found_clear_map=TRUE;
	int found_addr1;
	int found_hwret;
	int found_hwret_stall;
	int index,found_bsr=FALSE;
	int error= FALSE;
	ERR_MSG	*entry;			
	char *decode1;			/* palcode instruction ascii */
      	
	id1 = Mt + Clear_map;		/* search for Clear_map */
	while(found_clear_map == TRUE ) {		
	    if (entry = search_ev6_caveat (id1, caveat_list, addr1)) {
	        addr1 = entry->errnum;		/* get palcode address */	
		decode1 = entry->explanation; 	/* get palcode instruction ascii */	
		found_clear_map=TRUE;
	    } else found_clear_map=FALSE;
	    free (entry);

	    /* search raw_code[offs] for addr1 then search for hwret/stall or hw_ret */
	    /* note: hw_ret/stall can be either hw_ret_stall, hw_jmp_stall, hw_jsr_stall, or hw_jcr_stall 
	    /* if a hw_ret/stall found first all is well else if hw_ret found then issue error */
	    /* if bsr found then ignore the following hw_ret or hw_jcr, since it is return from bsr and */
	    /* not the end of pal entry flow */
            found_hwret=FALSE;
	    found_hwret_stall=FALSE;
            found_addr1=FALSE;
	    if (found_clear_map==TRUE) {
	        i=0;
      	        while (raw_code[i] != 0) {
	   	    if (raw_code[i]->address == addr1) {
			offs = i+1;
      	    	        while (raw_code[offs] != 0) {		
			    index = raw_code[offs]->index;		     
			    if (index == DO_BSR) 
				found_bsr=TRUE;
		            idtmp = get_inst_id (raw_code[offs]);
	   	            if (idtmp & Hwret_stall) {
		                found_hwret_stall=TRUE;
		                break;
		            }
			    if (idtmp & Hwret) {
			       /* if this is a BSR followed by HW_RET (hint=2) or HW_JCR (hint=3) then okay */
			        if ((raw_code[offs]->bits.ev6_hwret_format.hint == 2) ||
				    (raw_code[offs]->bits.ev6_hwret_format.hint == 3)) {
				       if (found_bsr==TRUE)
				           found_bsr=FALSE;   /* ok, hw_ret or hw_jcr follows a BSR*/
			               else {
				             found_hwret=TRUE;  /* error, found hw_ret, hw_jcr with no  BSR*/
		                             break;	
				            }
				}
		            }
	                    offs++;
	   	        } /* while ... */
	   	     } /* if (raw_code...)*/
	             i++;
	         } /* while ... */

	    if (found_clear_map==TRUE && found_hwret == TRUE && found_hwret_stall==FALSE)	
	           error=TRUE;
	    else   error=FALSE;
	  } /* 	if (found_clear_map==TRUE... */

          if (error == TRUE ) {	
	    restriction_num= 243;
	    if (new_error (addr1, restriction_num )) {
	         	wr (FLG$ERR, "%s\n", pheader);
	     	    wr (FLG$ERR, "***\n");
	            wr (FLG$ERR, "Error at %X: %s\n",  addr1,decode1);
	    	    wr (FLG$ERR, "(PVC #%d ) RESTRICTION #24c: \n",restriction_num );
	    	    wr (FLG$ERR, "           A Hw_mtpr CLEAR_MAP must be followed by a Hw_ret_stall, Hw_jmp_stall,\n");
	    	    wr (FLG$ERR, "           or rarely  Hw_jsr_stall, or Hw_jcr_stall.\n");
	    	    wr (FLG$ERR, "           This instruction may not trigger an Icache flush.\n");
	    	    wr (FLG$ERR, "           If an Icache flush is desired you must put a Hw_ret with stall\n");
	    	    wr (FLG$ERR, "           after this instruction and another after the Hw_mtpr IC_FLUSH.\n");
	    	    wr (FLG$ERR, "           Suggestion: Put another Hw_jmp_stall after the Hw_mtpr CLEAR_MAP. \n***\n");
	            pal_error_count++;
	    } /* if (new_error ... */
          } /* if (error == TRUE) */
     } /* while ... */
}


void check_ev6_guideline_26 (INSTR *raw_code[], int offs, int *id, int poss, int count)
/*
 ********************************************************************************
 *	PURPOSE:
 *		Check EV6 Palcode guideline #26.
 *
 *(26) Guideline: Conditional branches in PALcode 
 *
 *	To improve branch prediction it is advisable to avoid doing
 *	conditional branches in the first fetch block following 
 *	PAL entry. This includes all pal entry points except Call_pal
 *	entry points at address 2xxx and 3xxx.
 *	The branch will be predicted not taken 	in the Call_pal 
 *	flows and after the first fetch block at all other
 *	entry points.
 *
 * As written in ev6 spec:
 *   To avoid pollution of the branch predictors and improve overall branch
 *    prediction accuracy, conditional branch instructions in PALcode will be
 *    predicted not taken. The only exception to this rule are conditional branches
 *    within the first cache fetch (up to four instructions) of all pal flows except
 *    call_pal flows. It is advisable that conditional branches be avoided in this
 *    window. 
 *
 * 
 *	INPUT PARAMETERS:
 *		raw_code: pointer to the array of instructions from follow_code
 *		offs: where we are in the above array
 *		id:   array of ipr info such as scoreboard bits
 *		count: Number of Longwords in fetch block. Max = 4.
 *	OUTPUT PARAMETERS:
 *		None
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:            
 *	AUTHOR/DATE:
 *	V3.24 FLS Aug 96 
 ********************************************************************************
 */
{
      int i, index[4];
      int found_brcondit=FALSE;
      int found_mfplus=FALSE;

      for (i=0; i<4; i++) index[i]=0;

      /* check if any conditional br instructions are in first fetch block.
	 If not in call_pal flows (entry =2xxx/3xxx) then it is guideline
	 violation.
      */
      if (first_fetch_block_ev6 == TRUE) {
        if ((raw_code[offs]->address < EV6__CALL_PAL_00_ENTRY) ||
	    (raw_code[offs]->address > (EV6__CALL_PAL_80_ENTRY + 0xfff))) {

          for (i=0; i<count; i++) {				
	   	if (get_inst_id (raw_code[offs+i]) & Br_cond)  {
		    found_brcondit = TRUE;
		    break;
		}
          }

            if (found_brcondit == TRUE ) {		/* then error */
		restriction_num= 26;
	        if (new_error (raw_code[offs+i]->address, restriction_num)) {
	            wr (FLG$WARN, "%s\n", pheader);
	     	    wr (FLG$WARN, "***\n");
	    	    wr (FLG$ERR, "Warning at %X: %s\n", 
			 raw_code[offs+i]->address,raw_code[offs+i]->decoded);
	    	    wr (FLG$WARN, "(PVC #%d) GUIDELINE #26: \n",restriction_num);
	    	    wr (FLG$WARN, "          To improve branch prediction, it is advisable to avoid doing\n");
	    	    wr (FLG$WARN, "          conditional branches in the first fetch block following PAL\n");
	    	    wr (FLG$WARN, "          entry. This includes all pal entry points except Call_pal\n");
	    	    wr (FLG$WARN, "          entry points at address 2xxx and 3xxx.\n");
	    	    wr (FLG$WARN, "          NOTE: This is a GUIDELINE for performance reasons only.\n");
	            wr (FLG$ERR, "                To force PVC to ignore this error, put the ignore label at\n");
	            wr (FLG$ERR, "                address %x: \n",raw_code[offs+i]->address);
	    	    wr (FLG$WARN, "          Suggestion: Move the conditional branch instruction out of the \n");
	    	    wr (FLG$WARN, "                      first fetch block (octaword-aligned octaword).\n***\n");
	            pal_error_count++;
	        } /* if (new_error ... */
            } /* if found_brcondit == TRUE */
        } /* if ((raw_code[offs]->address != EV6__CALL_PAL_00_ENTRY) */
      } /* if first_fetch_block_ev6 == TRUE */
}



void check_ev6_restriction_27 (INSTR *raw_code[], int offs, List *caveat_list)
/*
 ********************************************************************************
 *	PURPOSE:
 *	    Check end of permutation restriction 27.  The palcode is checked
 *	    in the schedule_code routine and if it is the powerup/chip reset
 *	    pal entry (address=0) and a load/store instruction, a global
 *	    flag force_fail_lock_ev6 is set TRUE. This routine checks that flag
 *	    then searches the current pal entry for load/store to give the
 *	    palcoder the benefit of the doubt, not to mention we may
 *	    be in powerup pal entry (though not likely).
 *
 * RESTRICTION #27 - 
 * (27) Restriction: Reset of 'Force-Fail Lock Flag' State in PALcode 
 *
 *  A virtual mode load or store is required in PAL code before the execution 
 *  of any load-locked or store-conditional instructions. 
 *  The virtual-mode load or store may not be a HW_LD, HW_ST, LDx_L, ECB, or 
 *  WH64. 
 *  Clarification: The virtual mode load/store  only has to be done once 
 *		   after power-up or chip reset, so is normally done in
 *		   the power up palcode (pal entry address 0000000 
 *
 *	INPUT PARAMETERS:
 *		raw_code: pointer to the array of instructions from follow_code
 *		offs: where we are in the above array
 *		      Instruction at offs=0 is the pal entry instruction.
 *		caveat_list: pointer to caveat list
 *
 *	OUTPUT PARAMETERS:
 *		None
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:            
 *	AUTHOR/DATE:
 *	V3.24 FLS Sep-96 
 ********************************************************************************
 */
{
	int  idtmp=0;
	int id1;
	int addr1=0;			/* palcode address of caveat */
	int found_lock_inst=TRUE;
	int found_addr1;
	int error= FALSE;
	ERR_MSG	*entry;			
	char *decode1;			/* palcode instruction ascii */
      	
	id1 = Lock_inst;		/* search for lock_inst */
	/* Finding one of these errors is enough, so will not use while loop to search caveat_list again */
	if (entry = search_ev6_caveat (id1, caveat_list, addr1)) {
	    addr1 = entry->errnum;		/* get palcode address */	
            decode1 = entry->explanation; 	/* get palcode instruction ascii */	
	    found_lock_inst=TRUE;
	} else found_lock_inst=FALSE;
	free (entry);

	/* If global force_fail_lock_ev6 = FALSE then*/
	/* search raw_code[offs] for a load/store inst. then search for addr1 */
	/* if addr1 found before a load or store then issue error */
        /* A load/store is normally done in the POWERUP routine, but we can't fail if it is done in */
        /* the current pal entry flow */	
        found_addr1=FALSE;
	if (force_fail_lock_ev6==FALSE) {
	    if (found_lock_inst==TRUE) {
	        offs=0;
      	        while (raw_code[offs] != 0) {
		       idtmp = get_inst_id (raw_code[offs]);
		       if (raw_code[offs]->address == addr1) {
		           found_addr1=TRUE;					/* error addr1 found before load/store */
		           break;
		       } 
		       else  if ((idtmp == (Memory_op | Virtual_inst | Read)) && (raw_code[offs]->index != DO_HW_LD))
		                 break;									/* non locked load*/
		       else  if ((idtmp == (Memory_op | Virtual_inst | Write)) && (raw_code[offs]->index != DO_HW_ST))
		                break;									/* non locked store*/
	             offs++;
	        } /* while(raw_code[offs]... */

	    if (found_addr1==TRUE )	
	           error=TRUE;
	    else   error=FALSE;
	  } /* 	if (found_lock_inst==TRUE... */

          if (error == TRUE ) {	
	    restriction_num= 27;
	    if (new_error (addr1, restriction_num)) {
	         	wr (FLG$ERR, "%s\n", pheader);
	     	    wr (FLG$ERR, "***\n");
	            wr (FLG$ERR, "Error at %X: %s\n",  addr1,decode1);
	    	    wr (FLG$ERR, "(PVC #%d) RESTRICTION #27:\n",restriction_num);
	            wr (FLG$ERR, "          A virtual mode load or store instruction is required in the \n");
	    	    wr (FLG$ERR, "          PAL power-up routine to reset a hardware flag before any\n");
	    	    wr (FLG$ERR, "          LDx_L, STx_C, HW_LDxL, or HW_STxPC lock related instruction\n");
	    	    wr (FLG$ERR, "          is executed.\n");
	    	    wr (FLG$ERR, "          NOTE: Please ignore this error if a PVC DO POWERUP command has\n");
	    	    wr (FLG$ERR, "                not been done or there is no address 0000000 in your \n");
	    	    wr (FLG$ERR, "                .entry file.\n");
	    	    wr (FLG$ERR, "          Suggestion: Ensure there is a ldq or stq instruction in \n");
	    	    wr (FLG$ERR, "          		 your PAL power-up routine.\n");
	    	    wr (FLG$ERR, "***\n");
	            pal_error_count++;
	    } /* if (new_error ... */
          } /* if (error == TRUE) */
	} /* if (force_fail_lock_ev6==FALSE... */
}


void check_ev6_restriction_28a (INSTR *raw_code[], int offs, List *caveat_list)
/*
 ********************************************************************************
 *	PURPOSE:
 *	    Check end of permutation restriction 28 (checks MM_STAT).
 *
 *	    Except for IPRs checked, code is the same for routines 
 *	    check_ev6_restriction_28a, check_ev6_restriction_28b, check_ev6_restriction_28c,
 *	    and check_ev6_restriction_28d.
 *	    They check IPRs MM_STAT, VA, VA_FORM and MT DC_CTL respectively.
 *	    (I know what you are thinking, but I don't get paid by number of
 *	     lines of code, just thought it would make it 
 *	     easier to read code to replicate the routines.)
 *
 *(28) Restriction: Enforce Ordering between IPRs Implicitly Written
 *by Loads and Subsequent Loads
 *
 *   Certain IPR's which are updated as a result of faulting memory operations
 *   require software assistance to maintain ordering against newer instructions.
 *   Consider the following code sequence: 
 *
 *
 *   HW_MFPR IPR_MM_STAT 
 *   LDQ rx,(ry) 
 *
 *   It is typically the case that these instructions would issue in-order: The MFPR
 *   is data-ready and both instructions use a lower subcluster. However, the
 *   HW_MFPR's (and HW_MTPR's) respond to certain resource-busy
 *   indications and do not issue when the MBOX informs the IBOX that a certain
 *   set of resources (store-bubbles) are busy. The LD's respond to a different set
 *   of resource-busy indications (load-bubbles) and could issue around the
 *   HW_MFPR in the presence of the former. Software assistance is required to
 *   enforce the issue order. One bullet-proof way is to insert a 'memory barrier'
 *   instruction before the first load that occurs after the HW_MFPR MM_STAT.
 *   The VA, VA_FORM, and DC_CTL registers require a similar constraint. The
 *   exception address and exception summary register are protected in hardware
 *   against these overwrites as long as they are read within the first four
 *   instructions of a PALflow AND prior to any taken branch in that PALflow,
 *   whichever is earlier (see restriction #12). Note that all LOAD instructions
 *   except HW_LD might modify any or all of these registers. HW_LD does not
 *   modify MM_STAT. 
 *
 *	INPUT PARAMETERS:
 *		raw_code: pointer to the array of instructions from follow_code
 *		offs: where we are in the above array
 *		      Instruction at offs=0 is the pal entry instruction.
 *		caveat_list: pointer to caveat list
 *
 *	OUTPUT PARAMETERS:
 *		None
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:            
 *	AUTHOR/DATE:
 *	V3.24 FLS Sep-96 
 ********************************************************************************
 */
{
    int  idtmp=0;
    int i;		
    int offs_mm_stat, offs_br,offs_ld;
    int id1;
    int mm_stat_reg, br_cond_reg, dependency;
    int found_mm_stat=TRUE;
    int found_ld=FALSE;
    int found_mb=FALSE;
    int found_va=FALSE;
    int found_br_taken=FALSE;
    int case_1_error;
    int error= FALSE;
    int next_inline_addr;
    int  addr_mm_stat=0,addr_ld=0,addr3=0,addr4=0;			/* palcode address of caveat */

    ERR_MSG	*entry;			
    char *decode_mf, *decode_ld;					/* palcode instruction ascii */

    while (found_mm_stat == TRUE) {		
	/* Good case 1: MB between MF mm_stat and LD
	*	Hw_mfpr ra,mm_stat/VA/VA_FORM or Hw_mtpr DC_CTL
	*	MB
	*	LD ry,(rz)
	*  If we find an MB between the mf/mt and LD all is well; else must check case 2
	*/
	         id1 = Mf + Mm_stat;					/* search for hw_mfpr mm_stat */
        if (entry = search_ev6_caveat (id1, caveat_list, addr_mm_stat)) {
            addr_mm_stat = entry->errnum;				/* get palcode address */	
	    decode_mf = entry->explanation; 				/* get palcode instruction ascii */	
	    found_mm_stat=TRUE;
        } else found_mm_stat=FALSE;
        free (entry);

        /* first search for ldx, if not found then exit... */
        if (found_mm_stat==TRUE) {
            i=0;
	    offs_ld=0;
	    while (raw_code[i] != 0) {
	   	if (raw_code[i]->address == addr_mm_stat) {
		    offs_mm_stat = i;
	            mm_stat_reg = raw_code[i]->bits.ev6_hwmxpr_format.ra;  /* save MF mm_stat reg. no. */
		    /* Search for LDx after the mf/mt */
                    found_ld=FALSE;
                    found_mb=FALSE;
                    offs = i+1;
                    while (raw_code[offs] != 0) {			
                        idtmp = get_inst_id (raw_code[offs]);
                        if   (((idtmp == (Memory_op|Virtual_inst|Read))  && 
                              (raw_code[offs]->index != DO_HW_LD)) ||
            		      ((idtmp == (Memory_op|Virtual_inst|Read|Lock_inst)) && 
			      (raw_code[offs]->index != DO_HW_LD)))  {
	                      offs_ld = offs;
			      decode_ld =raw_code[offs_ld]->decoded;		/* for error report */
			      addr_ld =raw_code[offs_ld]->address;		/* for error report */
	                      found_ld=TRUE;
	                      break;
	               }
	                    offs++;
	   	   } /* while ... */
		   break;
	   	 } /* if (raw_code...)*/
	           i++;
	     } /* while ... */
	 } /* if (found_mm_stat==TRUE... */

	 case_1_error=FALSE;

        if (found_ld==TRUE) {
	     /* search raw_code[offs] for mm_stat then search for LDx */
	     /* if a MB found first all is well else check for case 2 */
	     i=0;
      	     while (raw_code[i] != 0) {
	   	if (raw_code[i]->address == addr_mm_stat) {
		    /* Search for MB and LDx; if MB found first all  is well */
            	    found_mb=FALSE;
		    offs = i+1;
      	    	    while (raw_code[offs] != 0) {			
		        idtmp = get_inst_id (raw_code[offs]);
			if   (((idtmp == (Memory_op|Virtual_inst|Read))  && 
			        (raw_code[offs]->index != DO_HW_LD)) ||
				((idtmp == (Memory_op|Virtual_inst|Read|Lock_inst)) && 
				(raw_code[offs]->index != DO_HW_LD)))  {
	                            break;				 /* error, LD found before MB */
	                }
			if ((raw_code[offs]->index == DO_MB) && 
			    !(raw_code[offs]->bits.instr & 0X400)){    /* not WMB */
		                found_mb=TRUE;				/* success, MB found first */
		                break;
		        }
	                offs++;
	   	    } /* while ... */
		    break;
	        } /* if (raw_code...)*/
	        i++;
	     } /* while ... */

	     if (found_mb==FALSE)	
	         case_1_error=TRUE;
        } /* 	if (found_ld==TRUE... */

	if (case_1_error==TRUE) {
	    /* Good case 2: Conditional Branch between MF mm_stat and LD  	*/
	    /*     Hw_mfpr ra,MM_STAT/VA/VA_FORM  or Hw_mtpr DC_CTL	      	*/
	    /*     Br_cond rx, label --->label: LD ry,(rz)	! branch taken	*/
	    i=0;
      	    while (raw_code[i] != 0) {
	   	if (raw_code[i]->address == addr_mm_stat) {
		  offs = i+1;
		  /* search for br_taken to LD, offs_ld is offs to LD instruction */
		  found_br_taken = FALSE;
      	          while ((offs != offs_ld) && (raw_code[offs] != 0)) {			
		    idtmp = get_inst_id (raw_code[offs]);
		    if (idtmp & Br_cond) {
		    next_inline_addr = raw_code[offs]->address + 4;
		    if(next_inline_addr != raw_code[offs+1]->address) {	/* br taken */
		        offs_br=offs;
			found_br_taken = TRUE;
			br_cond_reg = raw_code[offs_br]->bits.bra_format.ra;  /* save br_cond reg. no. */
			break;			
		    }
	          }
		  offs++;
	   	  }
		  break;
	       } /* if (raw_code...)*/
	       i++;
	    } /* while ... */

	    if (found_br_taken==TRUE ) {
		   dependency = check_mfmt_reg_ev6(raw_code, offs_mm_stat, mm_stat_reg, offs_br, br_cond_reg);
			
	    }
	    if ( found_br_taken== FALSE || dependency==FALSE)
		error=TRUE;
            else error=FALSE;

        } /* if (case_1_error==TRUE... */


        if (error == TRUE ) {	
	    restriction_num= 28;
	    if (new_error (addr_mm_stat, restriction_num)) {
	            wr (FLG$ERR, "%s\n", pheader);
	     	    wr (FLG$ERR, "***\n");
	    	    wr (FLG$ERR, "Error at %X: %s\n",  addr_mm_stat,decode_mf);
	    	    wr (FLG$ERR, "Error at %X: %s\n",  addr_ld,decode_ld);
	    	    wr (FLG$ERR, "(PVC #%d) RESTRICTION #28a: \n",restriction_num);
	            wr (FLG$ERR, "          There are certain conditions that could allow a LDx instruction\n");
	            wr (FLG$ERR, "          to issue before a Hw_mfpr mm_stat instruction even though it \n");
		    wr (FLG$ERR, "          follows it in memory. The LDx could get a TBmiss or fault and\n");
		    wr (FLG$ERR, "          overwrite the mm_stat ipr. \n");
		    wr (FLG$ERR, "          This restriction also applies to Hw_mfpr Va, Hw_mfpr Va_form\n");
	            wr (FLG$ERR, "          and Hw_mtpr dc_ctl. \n");
		    wr (FLG$ERR, "          Suggestion 1: Put a MB instruction between the Hw_mfpr mm_stat\n");
	    	    wr (FLG$ERR, "               	  and the LDx instruction. \n");
	    	    wr (FLG$ERR, "          Suggestion 2: Put a conditional branch instruction dependent\n");
	    	    wr (FLG$ERR, "               	  on register RA in between the Hw_mfpr RA,mm_stat\n");
	    	    wr (FLG$ERR, "               	  and LDx instructions. \n***\n");
	            pal_error_count++;
	    } /* if (new_error ... */
        } /* if (error == TRUE) */
     } /* while ... */
}


void check_ev6_restriction_28b (INSTR *raw_code[], int offs, List *caveat_list)
/*
 ********************************************************************************
 *	PURPOSE:
 *	    Check end of permutation restriction 28 (checks VA).
 *
 *	    Except for IPRs checked, code is the same for routines 
 *	    check_ev6_restriction_28b, check_ev6_restriction_28b, check_ev6_restriction_28c,
 *	    and check_ev6_restriction_28d.
 *	    They check IPRs MM_STAT, VA, VA_FORM and MT DC_CTL respectively.
 *
 *(28) Restriction: Enforce Ordering between IPRs Implicitly Written
 *by Loads and Subsequent Loads
 *
 *   Certain IPR's which are updated as a result of faulting memory operations
 *   require software assistance to maintain ordering against newer instructions.
 *   Consider the following code sequence: 
 *
 *
 *   HW_MFPR IPR_MM_STAT 
 *   LDQ rx,(ry) 
 *
 *   It is typically the case that these instructions would issue in-order: The MFPR
 *   is data-ready and both instructions use a lower subcluster. However, the
 *   HW_MFPR's (and HW_MTPR's) respond to certain resource-busy
 *   indications and do not issue when the MBOX informs the IBOX that a certain
 *   set of resources (store-bubbles) are busy. The LD's respond to a different set
 *   of resource-busy indications (load-bubbles) and could issue around the
 *   HW_MFPR in the presence of the former. Software assistance is required to
 *   enforce the issue order. One bullet-proof way is to insert a 'memory barrier'
 *   instruction before the first load that occurs after the HW_MFPR MM_STAT.
 *   The VA, VA_FORM, and DC_CTL registers require a similar constraint. The
 *   exception address and exception summary register are protected in hardware
 *   against these overwrites as long as they are read within the first four
 *   instructions of a PALflow AND prior to any taken branch in that PALflow,
 *   whichever is earlier (see restriction #12). Note that all LOAD instructions
 *   except HW_LD might modify any or all of these registers. HW_LD does not
 *   modify MM_STAT. 
 *
 *	INPUT PARAMETERS:
 *		raw_code: pointer to the array of instructions from follow_code
 *		offs: where we are in the above array
 *		      Instruction at offs=0 is the pal entry instruction.
 *		caveat_list: pointer to caveat list
 *
 *	OUTPUT PARAMETERS:
 *		None
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:            
 *	AUTHOR/DATE:
 *	V3.24 FLS Sep-96 
 ********************************************************************************
 */
{
    int  idtmp=0;
    int i;		
    int offs_va, offs_br,offs_ld;
    int id1;
    int va_reg, br_cond_reg, dependency;
    int found_va=TRUE;
    int found_ld=FALSE;
    int found_mb=FALSE;
    int found_br_taken=FALSE;
    int case_1_error;
    int error= FALSE;
    int next_inline_addr;
    int  addr_va=0,addr_ld=0,addr3=0,addr4=0;			/* palcode address of caveat */

    ERR_MSG	*entry;			
    char *decode_mf, *decode_ld;					/* palcode instruction ascii */

    while (found_va == TRUE) {		
	/* Good case 1: MB between MF va and LD
	*	Hw_mfpr ra,/VA/VA_FORM or Hw_mtpr DC_CTL
	*	MB
	*	LD ry,(rz)
	*  If we find an MB between the mf/mt and LD all is well; else must check case 2
	*/
	         id1 = Mf + Va;					/* search for hw_mfpr va */
        if (entry = search_ev6_caveat (id1, caveat_list, addr_va)) {
            addr_va = entry->errnum;				/* get palcode address */	
	    decode_mf = entry->explanation; 				/* get palcode instruction ascii */	
	    found_va=TRUE;
        } else found_va=FALSE;
        free (entry);

        /* first search for ldx, if not found then exit... */
        if (found_va==TRUE) {
            i=0;
	    offs_ld=0;
	    while (raw_code[i] != 0) {
	   	if (raw_code[i]->address == addr_va) {
		    offs_va = i;
	            va_reg = raw_code[i]->bits.ev6_hwmxpr_format.ra;  /* save MF va reg. no. */
		    /* Search for LDx after the mf/mt */
                    found_ld=FALSE;
                    found_mb=FALSE;
                    offs = i+1;
                    while (raw_code[offs] != 0) {			
                        idtmp = get_inst_id (raw_code[offs]);
                        if   (((idtmp == (Memory_op|Virtual_inst|Read))  && 
                              (raw_code[offs]->index != DO_HW_LD)) ||
            		      ((idtmp == (Memory_op|Virtual_inst|Read|Lock_inst)) && 
			      (raw_code[offs]->index != DO_HW_LD)))  {
	                      offs_ld = offs;
			      decode_ld =raw_code[offs_ld]->decoded;		/* for error report */
			      addr_ld =raw_code[offs_ld]->address;		/* for error report */
	                      found_ld=TRUE;
	                      break;
	               }
	                    offs++;
	   	   } /* while ... */
		   break;
	   	 } /* if (raw_code...)*/
	           i++;
	     } /* while ... */
	 } /* if (found_va==TRUE... */

	 case_1_error=FALSE;

        if (found_ld==TRUE) {
	     /* search raw_code[offs] for va then search for LDx */
	     /* if a MB found first all is well else check for case 2 */
	     i=0;
      	     while (raw_code[i] != 0) {
	   	if (raw_code[i]->address == addr_va) {
		    /* Search for MB and LDx; if MB found first all  is well */
            	    found_mb=FALSE;
		    offs = i+1;
      	    	    while (raw_code[offs] != 0) {			
		        idtmp = get_inst_id (raw_code[offs]);
			if   (((idtmp == (Memory_op|Virtual_inst|Read))  && 
			        (raw_code[offs]->index != DO_HW_LD)) ||
				((idtmp == (Memory_op|Virtual_inst|Read|Lock_inst)) && 
				(raw_code[offs]->index != DO_HW_LD)))  {
	                            break;				 /* error, LD found before MB */
	                }
			if ((raw_code[offs]->index == DO_MB) && 
			    !(raw_code[offs]->bits.instr & 0X400)){    /* not WMB */
		                found_mb=TRUE;				/* success, MB found first */
		                break;
		        }
	                offs++;
	   	    } /* while ... */
		    break;
	        } /* if (raw_code...)*/
	        i++;
	     } /* while ... */

	     if (found_mb==FALSE)	
	         case_1_error=TRUE;
        } /* 	if (found_ld==TRUE... */

	if (case_1_error==TRUE) {
	    /* Good case 2: Conditional Branch between MF va and LD  	*/
	    /*     Hw_mfpr ra,va/VA/VA_FORM  or Hw_mtpr DC_CTL	      	*/
	    /*     Br_cond rx, label --->label: LD ry,(rz)	! branch taken	*/
	    i=0;
      	    while (raw_code[i] != 0) {
	   	if (raw_code[i]->address == addr_va) {
		  offs = i+1;
		  /* search for br_taken to LD, offs_ld is offs to LD instruction */
		  found_br_taken = FALSE;
      	          while ((offs != offs_ld) && (raw_code[offs] != 0)) {			
		    idtmp = get_inst_id (raw_code[offs]);
		    if (idtmp & Br_cond) {
		    next_inline_addr = raw_code[offs]->address + 4;
		    if(next_inline_addr != raw_code[offs+1]->address) {	/* br taken */
		        offs_br=offs;
			found_br_taken = TRUE;
			br_cond_reg = raw_code[offs_br]->bits.bra_format.ra;  /* save br_cond reg. no. */
			break;			
		    }
	          }
		  offs++;
	   	  }
		  break;
	       } /* if (raw_code...)*/
	       i++;
	    } /* while ... */

	    if (found_br_taken==TRUE ) {
		   dependency = check_mfmt_reg_ev6(raw_code, offs_va, va_reg, offs_br, br_cond_reg);
			
	    }
	    if ( found_br_taken== FALSE || dependency==FALSE)
		error=TRUE;
            else error=FALSE;

        } /* if (case_1_error==TRUE... */


        if (error == TRUE ) {	
	    restriction_num= 28;
	    if (new_error (addr_va, restriction_num)) {
	            wr (FLG$ERR, "%s\n", pheader);
	     	    wr (FLG$ERR, "***\n");
	    	    wr (FLG$ERR, "Error at %X: %s\n",  addr_va,decode_mf);
	    	    wr (FLG$ERR, "Error at %X: %s\n",  addr_ld,decode_ld);
	    	    wr (FLG$ERR, "(PVC #%d) RESTRICTION #28b: \n",restriction_num);
	            wr (FLG$ERR, "          There are certain conditions that could allow a LDx instruction\n");
	            wr (FLG$ERR, "          to issue before a Hw_mfpr VA instruction even though it \n");
		    wr (FLG$ERR, "          follows it in memory. The LDx could get a TBmiss or fault and\n");
		    wr (FLG$ERR, "          overwrite the VA ipr. \n");
		    wr (FLG$ERR, "          This restriction also applies to Hw_mfpr mm_stat, Hw_mfpr Va_form\n");
	            wr (FLG$ERR, "          and Hw_mtpr dc_ctl. \n");
		    wr (FLG$ERR, "          Suggestion 1: Put a MB instruction between the Hw_mfpr VA\n");
	    	    wr (FLG$ERR, "               	  and the LDx instruction. \n");
	    	    wr (FLG$ERR, "          Suggestion 2: Put a conditional branch instruction dependent\n");
	    	    wr (FLG$ERR, "               	  on register RA in between the Hw_mfpr RA,VA\n");
	    	    wr (FLG$ERR, "               	  and LDx instructions. \n***\n");
	            pal_error_count++;
	    } /* if (new_error ... */
        } /* if (error == TRUE) */
     } /* while ... */
}


void check_ev6_restriction_28c (INSTR *raw_code[], int offs, List *caveat_list)
/*
 ********************************************************************************
 *	PURPOSE:
 *	    Check end of permutation restriction 28 (checks VA_FORM).
 *
 *	    Except for IPRs checked, code is the same for routines 
 *	    check_ev6_restriction_28c, check_ev6_restriction_28c, check_ev6_restriction_28c,
 *	    and check_ev6_restriction_28d.
 *	    They check IPRs MM_STAT, VA, VA_FORM and MT DC_CTL respectively.
 *
 *(28) Restriction: Enforce Ordering between IPRs Implicitly Written
 *by Loads and Subsequent Loads
 *
 *   Certain IPR's which are updated as a result of faulting memory operations
 *   require software assistance to maintain ordering against newer instructions.
 *   Consider the following code sequence: 
 *
 *
 *   HW_MFPR IPR_MM_STAT 
 *   LDQ rx,(ry) 
 *
 *   It is typically the case that these instructions would issue in-order: The MFPR
 *   is data-ready and both instructions use a lower subcluster. However, the
 *   HW_MFPR's (and HW_MTPR's) respond to certain resource-busy
 *   indications and do not issue when the MBOX informs the IBOX that a certain
 *   set of resources (store-bubbles) are busy. The LD's respond to a different set
 *   of resource-busy indications (load-bubbles) and could issue around the
 *   HW_MFPR in the presence of the former. Software assistance is required to
 *   enforce the issue order. One bullet-proof way is to insert a 'memory barrier'
 *   instruction before the first load that occurs after the HW_MFPR MM_STAT.
 *   The VA, VA_FORM, and DC_CTL registers require a similar constraint. The
 *   exception address and exception summary register are protected in hardware
 *   against these overwrites as long as they are read within the first four
 *   instructions of a PALflow AND prior to any taken branch in that PALflow,
 *   whichever is earlier (see restriction #12). Note that all LOAD instructions
 *   except HW_LD might modify any or all of these registers. HW_LD does not
 *   modify MM_STAT. 
 *
 *	INPUT PARAMETERS:
 *		raw_code: pointer to the array of instructions from follow_code
 *		offs: where we are in the above array
 *		      Instruction at offs=0 is the pal entry instruction.
 *		caveat_list: pointer to caveat list
 *
 *	OUTPUT PARAMETERS:
 *		None
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:            
 *	AUTHOR/DATE:
 *	V3.24 FLS Sep-96 
 ********************************************************************************
 */
{
    int  idtmp=0;
    int i;		
    int offs_va_form, offs_br,offs_ld;
    int id1;
    int va_form_reg, br_cond_reg, dependency;
    int found_va_form=TRUE;
    int found_ld=FALSE;
    int found_mb=FALSE;
    int found_br_taken=FALSE;
    int case_1_error;
    int error= FALSE;
    int next_inline_addr;
    int  addr_va_form=0,addr_ld=0,addr3=0,addr4=0;			/* palcode address of caveat */

    ERR_MSG	*entry;			
    char *decode_mf, *decode_ld;					/* palcode instruction ascii */

    while (found_va_form == TRUE) {		
	/* Good case 1: MB between MF va_form and LD
	*	Hw_mfpr ra,/VA/VA_FORM or Hw_mtpr DC_CTL
	*	MB
	*	LD ry,(rz)
	*  If we find an MB between the mf/mt and LD all is well; else must check case 2
	*/
	         id1 = Mf + Va_form;					/* search for hw_mfpr va_form */
        if (entry = search_ev6_caveat (id1, caveat_list, addr_va_form)) {
            addr_va_form = entry->errnum;				/* get palcode address */	
	    decode_mf = entry->explanation; 				/* get palcode instruction ascii */	
	    found_va_form=TRUE;
        } else found_va_form=FALSE;
        free (entry);

        /* first search for ldx, if not found then exit... */
        if (found_va_form==TRUE) {
            i=0;
	    offs_ld=0;
	    while (raw_code[i] != 0) {
	   	if (raw_code[i]->address == addr_va_form) {
		    offs_va_form = i;
	            va_form_reg = raw_code[i]->bits.ev6_hwmxpr_format.ra;  /* save MF va_form reg. no. */
		    /* Search for LDx after the mf/mt */
                    found_ld=FALSE;
                    found_mb=FALSE;
                    offs = i+1;
                    while (raw_code[offs] != 0) {			
                        idtmp = get_inst_id (raw_code[offs]);
                        if   (((idtmp == (Memory_op|Virtual_inst|Read))  && 
                              (raw_code[offs]->index != DO_HW_LD)) ||
            		      ((idtmp == (Memory_op|Virtual_inst|Read|Lock_inst)) && 
			      (raw_code[offs]->index != DO_HW_LD)))  {
	                      offs_ld = offs;
			      decode_ld =raw_code[offs_ld]->decoded;		/* for error report */
			      addr_ld =raw_code[offs_ld]->address;		/* for error report */
	                      found_ld=TRUE;
	                      break;
	               }
	                    offs++;
	   	   } /* while ... */
		   break;
	   	 } /* if (raw_code...)*/
	           i++;
	     } /* while ... */
	 } /* if (found_va_form==TRUE... */

	 case_1_error=FALSE;

        if (found_ld==TRUE) {
	     /* search raw_code[offs] for va_form then search for LDx */
	     /* if a MB found first all is well else check for case 2 */
	     i=0;
      	     while (raw_code[i] != 0) {
	   	if (raw_code[i]->address == addr_va_form) {
		    /* Search for MB and LDx; if MB found first all  is well */
            	    found_mb=FALSE;
		    offs = i+1;
      	    	    while (raw_code[offs] != 0) {			
		        idtmp = get_inst_id (raw_code[offs]);
			if   (((idtmp == (Memory_op|Virtual_inst|Read))  && 
			        (raw_code[offs]->index != DO_HW_LD)) ||
				((idtmp == (Memory_op|Virtual_inst|Read|Lock_inst)) && 
				(raw_code[offs]->index != DO_HW_LD)))  {
	                            break;				 /* error, LD found before MB */
	                }
			if ((raw_code[offs]->index == DO_MB) && 
			    !(raw_code[offs]->bits.instr & 0X400)){    /* not WMB */
		                found_mb=TRUE;				/* success, MB found first */
		                break;
		        }
	                offs++;
	   	    } /* while ... */
		    break;
	        } /* if (raw_code...)*/
	        i++;
	     } /* while ... */

	     if (found_mb==FALSE)	
	         case_1_error=TRUE;
        } /* 	if (found_ld==TRUE... */

	if (case_1_error==TRUE) {
	    /* Good case 2: Conditional Branch between MF va_form and LD  	*/
	    /*     Hw_mfpr ra,va/VA_FORM  or Hw_mtpr DC_CTL	      	*/
	    /*     Br_cond rx, label --->label: LD ry,(rz)	! branch taken	*/
	    i=0;
      	    while (raw_code[i] != 0) {
	   	if (raw_code[i]->address == addr_va_form) {
		  offs = i+1;
		  /* search for br_taken to LD, offs_ld is offs to LD instruction */
		  found_br_taken = FALSE;
      	          while ((offs != offs_ld) && (raw_code[offs] != 0)) {			
		    idtmp = get_inst_id (raw_code[offs]);
		    if (idtmp & Br_cond) {
		    next_inline_addr = raw_code[offs]->address + 4;
		    if(next_inline_addr != raw_code[offs+1]->address) {	/* br taken */
		        offs_br=offs;
			found_br_taken = TRUE;
			br_cond_reg = raw_code[offs_br]->bits.bra_format.ra;  /* save br_cond reg. no. */
			break;			
		    }
	          }
		  offs++;
	   	  }
		  break;
	       } /* if (raw_code...)*/
	       i++;
	    } /* while ... */

	    if (found_br_taken==TRUE ) {
		   dependency = check_mfmt_reg_ev6(raw_code, offs_va_form, va_form_reg, offs_br, br_cond_reg);
			
	    }
	    if ( found_br_taken== FALSE || dependency==FALSE)
		error=TRUE;
            else error=FALSE;

        } /* if (case_1_error==TRUE... */

        if (error == TRUE ) {	
	    restriction_num= 28;
	    if (new_error (addr_va_form, restriction_num)) {
	            wr (FLG$ERR, "%s\n", pheader);
	     	    wr (FLG$ERR, "***\n");
	    	    wr (FLG$ERR, "Error at %X: %s\n",  addr_va_form,decode_mf);
	    	    wr (FLG$ERR, "Error at %X: %s\n",  addr_ld,decode_ld);
	    	    wr (FLG$ERR, "(PVC #%d) RESTRICTION #28c: \n",restriction_num);
	            wr (FLG$ERR, "          There are certain conditions that could allow a LDx instruction\n");
	            wr (FLG$ERR, "          to issue before a Hw_mfpr va_form instruction even though it \n");
		    wr (FLG$ERR, "          follows it in memory. The LDx could get a TBmiss or fault and\n");
		    wr (FLG$ERR, "          overwrite the va_form ipr. \n");
		    wr (FLG$ERR, "          This restriction also applies to Hw_mfpr mm_stat, Hw_mfpr va\n");
	            wr (FLG$ERR, "          and Hw_mtpr dc_ctl. \n");
		    wr (FLG$ERR, "          Suggestion 1: Put a MB instruction between the Hw_mfpr va_form\n");
	    	    wr (FLG$ERR, "               	  and the LDx instruction. \n");
	    	    wr (FLG$ERR, "          Suggestion 2: Put a conditional branch instruction dependent\n");
	    	    wr (FLG$ERR, "               	  on register RA in between the Hw_mfpr RA,VA_FORM\n");
	    	    wr (FLG$ERR, "               	  and LDx instructions. \n***\n");
	            pal_error_count++;
	    } /* if (new_error ... */
        } /* if (error == TRUE) */
     } /* while ... */
}


void check_ev6_restriction_28d (INSTR *raw_code[], int offs, List *caveat_list)
/*
 ********************************************************************************
 *	PURPOSE:
 *	    Check end of permutation restriction 28 (checks DC_CTL).
 *
 *	    Except for IPRs checked, code is the same for routines 
 *	    check_ev6_restriction_28a check_ev6_restriction_28b, check_ev6_restriction_28c,
 *	    and check_ev6_restriction_28d.
 *	    They check IPRs MM_STAT, VA, VA_FORM and MT DC_CTL respectively.
 *	    Actually 28d differs in that it doesn't allow the
 *	    register dependency since the DC_CTL is a write only ipr.
 *
 *(28) Restriction: Enforce Ordering between IPRs Implicitly Written
 *by Loads and Subsequent Loads
 *
 *   Certain IPR's which are updated as a result of faulting memory operations
 *   require software assistance to maintain ordering against newer instructions.
 *   Consider the following code sequence: 
 *
 *
 *   HW_MFPR IPR_MM_STAT 
 *   LDQ rx,(ry) 
 *
 *   It is typically the case that these instructions would issue in-order: The MFPR
 *   is data-ready and both instructions use a lower subcluster. However, the
 *   HW_MFPR's (and HW_MTPR's) respond to certain resource-busy
 *   indications and do not issue when the MBOX informs the IBOX that a certain
 *   set of resources (store-bubbles) are busy. The LD's respond to a different set
 *   of resource-busy indications (load-bubbles) and could issue around the
 *   HW_MFPR in the presence of the former. Software assistance is required to
 *   enforce the issue order. One bullet-proof way is to insert a 'memory barrier'
 *   instruction before the first load that occurs after the HW_MFPR MM_STAT.
 *   The VA, VA_FORM, and DC_CTL registers require a similar constraint. The
 *   exception address and exception summary register are protected in hardware
 *   against these overwrites as long as they are read within the first four
 *   instructions of a PALflow AND prior to any taken branch in that PALflow,
 *   whichever is earlier (see restriction #12). Note that all LOAD instructions
 *   except HW_LD might modify any or all of these registers. HW_LD does not
 *   modify MM_STAT. 
 *
 *	INPUT PARAMETERS:
 *		raw_code: pointer to the array of instructions from follow_code
 *		offs: where we are in the above array
 *		      Instruction at offs=0 is the pal entry instruction.
 *		caveat_list: pointer to caveat list
 *
 *	OUTPUT PARAMETERS:
 *		None
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:            
 *	AUTHOR/DATE:
 *	V3.24 FLS Sep-96 
 ********************************************************************************
 */
{
    int  idtmp=0;
    int i;		
    int offs_dc_ctl, offs_br,offs_ld;
    int id1;
    int dc_ctl_reg, br_cond_reg, dependency;
    int found_dc_ctl=TRUE;
    int found_ld=FALSE;
    int found_mb=FALSE;
    int found_br_taken=FALSE;
    int case_1_error;
    int error= FALSE;
    int next_inline_addr;
    int  addr_dc_ctl=0,addr_ld=0,addr3=0,addr4=0;			/* palcode address of caveat */

    ERR_MSG	*entry;			
    char *decode_mf, *decode_ld;					/* palcode instruction ascii */

    while (found_dc_ctl == TRUE) {		
	/* Good case 1: MB between MF dc_ctl and LD
	*	Hw_mfpr ra,/VA/dc_ctl or Hw_mtpr DC_CTL
	*	MB
	*	LD ry,(rz)
	*  If we find an MB between the mf/mt and LD all is well; else must check case 2
	*/
	   id1 = Mt + Dc_ctl;					/* search for hw_mfpr dc_ctl */
        if (entry = search_ev6_caveat (id1, caveat_list, addr_dc_ctl)) {
            addr_dc_ctl = entry->errnum;				/* get palcode address */	
	    decode_mf = entry->explanation; 				/* get palcode instruction ascii */	
	    found_dc_ctl=TRUE;
        } else found_dc_ctl=FALSE;
        free (entry);

        /* first search for ldx, if not found then exit... */
        if (found_dc_ctl==TRUE) {
            i=0;
	    offs_ld=0;
	    while (raw_code[i] != 0) {
	   	if (raw_code[i]->address == addr_dc_ctl) {
		    offs_dc_ctl = i;
	            dc_ctl_reg = raw_code[i]->bits.ev6_hwmxpr_format.ra;  /* save MF dc_ctl reg. no. */
		    /* Search for LDx after the mf/mt */
                    found_ld=FALSE;
                    found_mb=FALSE;
                    offs = i+1;
                    while (raw_code[offs] != 0) {			
                        idtmp = get_inst_id (raw_code[offs]);
                        if   (((idtmp == (Memory_op|Virtual_inst|Read))  && 
                              (raw_code[offs]->index != DO_HW_LD)) ||
            		      ((idtmp == (Memory_op|Virtual_inst|Read|Lock_inst)) && 
			      (raw_code[offs]->index != DO_HW_LD)))  {
	                      offs_ld = offs;
			      decode_ld =raw_code[offs_ld]->decoded;		/* for error report */
			      addr_ld =raw_code[offs_ld]->address;		/* for error report */
	                      found_ld=TRUE;
	                      break;
	               }
	                    offs++;
	   	   } /* while ... */
		   break;
	   	 } /* if (raw_code...)*/
	           i++;
	     } /* while ... */
	 } /* if (found_dc_ctl==TRUE... */

	 case_1_error=FALSE;

        if (found_ld==TRUE) {
	     /* search raw_code[offs] for dc_ctl then search for LDx */
	     /* if a MB found first all is well else check for case 2 */
	     i=0;
      	     while (raw_code[i] != 0) {
	   	if (raw_code[i]->address == addr_dc_ctl) {
		    /* Search for MB and LDx; if MB found first all  is well */
            	    found_mb=FALSE;
		    offs = i+1;
      	    	    while (raw_code[offs] != 0) {			
		        idtmp = get_inst_id (raw_code[offs]);
			if   (((idtmp == (Memory_op|Virtual_inst|Read))  && 
			        (raw_code[offs]->index != DO_HW_LD)) ||
				((idtmp == (Memory_op|Virtual_inst|Read|Lock_inst)) && 
				(raw_code[offs]->index != DO_HW_LD)))  {
	                            break;				 /* error, LD found before MB */
	                }
			if ((raw_code[offs]->index == DO_MB) && 
			    !(raw_code[offs]->bits.instr & 0X400)){    /* not WMB */
		                found_mb=TRUE;				/* success, MB found first */
		                break;
		        }
	                offs++;
	   	    } /* while ... */
		    break;
	        } /* if (raw_code...)*/
	        i++;
	     } /* while ... */

	     if (found_mb==FALSE)	
	         case_1_error=TRUE;
        } /* 	if (found_ld==TRUE... */

	    /* NOTE: This code doesn't apply to DC_CTL , but is left here to   */
	    /* make it easier for maintainers to compare with check_ev6_restrictions_28a/b/c */
	if (case_1_error==TRUE) {
	    /* Good case 2: Conditional Branch between MF dc_ctl and LD  	*/
	    /*     Hw_mfpr ra,mm_stat/va/va_form  or Hw_mtpr DC_CTL	      	*/
	    /*     Br_cond rx, label --->label: LD ry,(rz)	! branch taken	*/
	    i=0;
      	    while (raw_code[i] != 0) {
	   	if (raw_code[i]->address == addr_dc_ctl) {
		  offs = i+1;
		  /* search for br_taken to LD, offs_ld is offs to LD instruction */
		  found_br_taken = FALSE;
      	          while ((offs != offs_ld) && (raw_code[offs] != 0)) {			
		    idtmp = get_inst_id (raw_code[offs]);
		    if (idtmp & Br_cond) {
		    next_inline_addr = raw_code[offs]->address + 4;
		    if(next_inline_addr != raw_code[offs+1]->address) {	/* br taken */
		        offs_br=offs;
			found_br_taken = TRUE;
			br_cond_reg = raw_code[offs_br]->bits.bra_format.ra;  /* save br_cond reg. no. */
			break;			
		    }
	          }
		  offs++;
	   	  }
		  break;
	       } /* if (raw_code...)*/
	       i++;
	    } /* while ... */

	    if (found_br_taken==TRUE ) {
		   dependency = check_mfmt_reg_ev6(raw_code, offs_dc_ctl, dc_ctl_reg, offs_br, br_cond_reg);
			
	    }
	    if ( found_br_taken== FALSE || dependency==FALSE)
		error=TRUE;
            else error=FALSE;

        } /* if (case_1_error==TRUE... */

	/* NOTE: for check_ev6_restriction_28d (DC_CTL), we only check for MB found*/
	if (found_dc_ctl==TRUE && found_ld==TRUE && found_mb==FALSE)	
	     error=TRUE;
        else error=FALSE;

        if (error == TRUE ) {	
	    restriction_num= 28;
	    if (new_error (addr_dc_ctl, restriction_num)) {
	            wr (FLG$ERR, "%s\n", pheader);
	     	    wr (FLG$ERR, "***\n");
	    	    wr (FLG$ERR, "Error at %X: %s\n",  addr_dc_ctl,decode_mf);
	    	    wr (FLG$ERR, "Error at %X: %s\n",  addr_ld,decode_ld);
	    	    wr (FLG$ERR, "(PVC #%d) RESTRICTION #28d: \n",restriction_num);
	            wr (FLG$ERR, "          There are certain conditions that could allow a LDx instruction\n");
	            wr (FLG$ERR, "          to issue before a Hw_mtpr dc_ctl instruction even though it \n");
		    wr (FLG$ERR, "          follows it in the program. The LDx could get a TBmiss or fault,\n");
		    wr (FLG$ERR, "          and a diabolical palcoder could overwrite the dc_ctl ipr. \n");
		    wr (FLG$ERR, "          This restriction also applies to Hw_mfpr mm_stat, Hw_mfpr va\n");
	            wr (FLG$ERR, "          and Hw_mfpr va_form. \n");
		    wr (FLG$ERR, "          Suggestion:   Put a MB instruction between the Hw_mtpr dc_ctl\n");
	    	    wr (FLG$ERR, "               	  and the LDx instruction. \n***\n");
	            pal_error_count++;
	    } /* if (new_error ... */
        } /* if (error == TRUE) */
     } /* while ... */
}

void check_ev6_guideline_29 (INSTR *raw_code[], int offs, List *caveat_list)
/*
 ********************************************************************************
 *	PURPOSE:
 *	    Check end of permutation guideline 29
 *
 *(29) Guideline: JSR,JMP,RET,JSR_COR usage in PALcode
 *
 * (29) Guideline: JSR,JMP,RET,JSR_COR usage in PALcode
 * 
 *    Unprivileged JSR,JMP,RET,JSR_COR instructions will always
 *    mispredict when used in PALcode. In addition, HW_RET to a
 *    palmode target will always mispredict since the jsr stack only predicts
 *    native mode return addresses. HW_RET to a native mode target uses
 *    the jsr stack for prediction and should generally be used when exiting
 *    palmode in order to maintain jsr stack alignment since all pal traps
 *    also push the value of the EXC_ADDR on the jsr stack. Other
 *    privileged versions of the JSR type instructions
 *    (HW_JSR,HW_JMP,HW_JSR_COR) can be used both within
 *    palmode or to exit palmode and generate a predicted target based on
 *    their hint bits and the current processor palmode state. 
 *
 *	INPUT PARAMETERS:
 *		raw_code: pointer to the array of instructions from follow_code
 *		offs: where we are in the above array
 *		      Instruction at offs=0 is the pal entry instruction.
 *		caveat_list: pointer to caveat list
 *
 * Here is a mail message from Ed Mclellan (edited) that helps clarify this guideline:
 *
 * 	There are a couple of causes for the mispredicts.  First, all stack 
 * pops concatenate an lsb of 0 (native mode), so any stack users (return, hw_ret)
 * will mispredict if you attempt to use it to jump to a pal target.  We cannot 
 * predict a return to a palmode target by popping the stack. 
 * 
 * 	Second, we force a mispredict on any non-priv JSR (jsr,jmp,ret,cor) 
 * if the target is palmode.  This is the one that guideline 29 addresses. 
 * 
 * Privileged jsrs, (hw_jsr,hw_jmp,hw_cor,hw_ret) are allowed to compare their 
 * register target lsbs with the predicted path pc lsb, so they will check for 
 * palmode/native mode targets and mispredict appropriately.  
 * 
 * 	The combination of the two imply 
 * -non-priv jsrs can predict correctly only in native mode
 * -some priv jsrs (hw_jmp,hw_jsr,hw_cor) can predict correctly to any target/mode, 
 * -hw_ret can predict correctly only to native mode
 * 
 * 	Since the jsr stack will push a return address on any 
 * pal trap, you should make sure that stack push/pops are done in pairs, which 
 * seems to imply that on double misses, you will incur at least one mispredict 
 * when returning from the nested routine to the first palmode routine.  It 
 * would maybe be smart for us not to push an address on double misses. 
 * 
 *	OUTPUT PARAMETERS:
 *		None
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:            
 *	AUTHOR/DATE:
 *	V3.24 FLS Sep-96 
 *	V3.25 FLS Mar-97 - change error message wording to reflect that
 *			   hw_ret to palmode will mispredict.
 ********************************************************************************
 */
{
	int id1;
	int addr1=0;			/* palcode address of caveat */
	int found_jsr=TRUE;
	int error= FALSE;
	ERR_MSG	*entry;			
	char *decode1;			/* palcode instruction ascii */
      	
	id1 = Jsr;			/* search for Jsr/Jmp/Ret/Jsr_coroutine */
	while(found_jsr == TRUE ) {		
	    if (entry = search_ev6_caveat (id1, caveat_list, addr1)) {
	        addr1 = entry->errnum;		/* get palcode address */	
		decode1 = entry->explanation; 	/* get palcode instruction ascii */	
		found_jsr=TRUE;
	    } else found_jsr=FALSE;
	    free (entry);

	    if (found_jsr==TRUE )	
	           error=TRUE;
	    else   error=FALSE;

          if (error == TRUE ) {	
	    restriction_num= 29;
	    if (new_error (addr1, restriction_num)) {
	            wr (FLG$WARN, "%s\n", pheader);
	     	    wr (FLG$WARN, "***\n");
	    	    wr (FLG$ERR, "Warning at %X: %s\n",  addr1,decode1);
	    	    wr (FLG$WARN, "(PVC #%d) GUIDELINE #29:\n",restriction_num);
	            wr (FLG$WARN, "          Jsr, Jmp, Ret, and Jsr_coroutine will always mispredict\n");
	            wr (FLG$ERR, "           when used in PALcode. The privileged variants of these\n");
	            wr (FLG$ERR, "           instructions (Hw_jsr,Hw_jmp,Hw_jcr,Hw_ret) will predict correctly.\n");
	            wr (FLG$ERR, "           Hw_ret, however will only predict correctly when returning\n");
	            wr (FLG$ERR, "           to native (non PALmode) addresses. \n");
	    	    wr (FLG$WARN, "          NOTE: This is a GUIDELINE for performance reasons only.\n");
	            wr (FLG$ERR, "                To force PVC to ignore this error, put the ignore label at\n");
	            wr (FLG$ERR, "                address %x: \n",addr1);
	    	    wr (FLG$WARN, "          Suggestion: Use the privileged variants Hw_jmp, Hw_ret, \n");
	    	    wr (FLG$WARN, "                      hw_jsr or hw_jcr instead.\n***\n");
	            pal_warning_count++;
	    } /* if (new_error ... */
          } /* if (error == TRUE) */
     } /* while ... */
}


void check_ev6_restriction_30 (INSTR *raw_code[], int offs, List *caveat_list)
/*
 ********************************************************************************
 *	PURPOSE:
 *	    Check end of permutation restriction 30.
 *
 * Ensure MB and example occurs before WRITE ev6__data 
 *                                                                       
 * There are a number of simplifications assumed for restriction 30:
 *	1. It is assumed that WRITE EV6__DATA code will only occur once in a pal
 *	entry. This is fairly safe since the CBOX CSR ipr (aka ev6__data)  will 
 *      only be written in the pal reset  aka powerup or srom code, since it 
 *      sets up parameters that affect the pin bus such as sysclk divisors. 
 *
 * (30) Restriction: MTPR CBOX CSR 
 * 
 *    External bus activity must be isolated from writes to the CBOX CSR. This
 *    requires that all dstream and istream fills must be avoided until after the
 *    MTPR update completes. An MB instruction can block dstream activity, but
 *    blocking all istream fills including prefetches requires more extensive code.
 *    One example that will block all istream fill requests and stall instruction fetch
 *    until after the desired MTPR action completes is provided below. 
 * 
 *    ;
 *    ;Macro to stop IBOX from sending requests to CBOX
 *    ;Replace i1-i6 with instruction(s) you want done with ibox quiet
 *    ;This macro is assumed to be used in palmode
 * 
 *    .macro stop_ibox i1=nop, i2=nop, i3=nop, i4=nop, i5=nop, i6=nop, -
 *                     ?touch1, ?touch2, ?touch3, ?over1, ?over2, ?start, ?end
 *            .align 6, 0x47ff041f    ;Next 16 instructions must be cache block aligned, fill with BIS NOP
 *            br r31, touch1          ;skip around and pull in next 4 sub-blks into ICACHE
 *    start:
 *            i1
 *            i2
 *            br r31, over1           ;skip over touch stuff
 * 
 *    touch1:
 *            br r31, touch2          ;skip around and pull in next 4 blk into ICACHE
 *    over1:
 *            i3
 *            i4
 *            br r31, over2
 *            
 *    touch2:
 *            br r31, touch3          ;skip around and pull in next 4 blk into ICACHE
 *    over2:
 *            i5
 *            i6
 *            hw_mtpr r31, <0x0610>   ;dummy IPR write - sets SCBD bit 4
 *            
 *            hw_mtpr r31, <0x0610>   ;also a dummy IPR write but gets stalled until above write retires
 *            beq r31, end            ;Predicts fall through since in PALmode
 *            br r31, .-4             ;fools ibox predictor into infinite loop
 *    touch3:
 *            br r31, start
 *    end:
 *    .endm
 * 
 *
 *	INPUT PARAMETERS:
 *		raw_code: pointer to the array of instructions from follow_code
 *		offs: where we are in the above array
 *		      Instruction at offs=0 is the pal entry instruction.
 *		caveat_list: pointer to caveat list
 *
 *	OUTPUT PARAMETERS:
 *		None
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:            
 *	AUTHOR/DATE:
 *	V3.24 FLS Oct-96 
 ********************************************************************************
 */
{
	int i,j,k;
	int id1;
	int found_ev6_data=TRUE;
	int found_mb = FALSE;
	int found_br = FALSE;
	int found_br_l2 = FALSE;
	int found_br_l3 = FALSE;
	int found_br_l4 = FALSE;
	int found_br_l5 = FALSE;
	int found_br_l6 = FALSE;
	int found_br_l7 = FALSE;
	int found_br_start = FALSE;
	int found_mt_r20 = FALSE;
	int found_mt_dummy1 = FALSE;
	int found_mt_dummy2 = FALSE;
	int found_aligned_br=FALSE;
	int error= FALSE;
        int addr_ev6_data=0;						/* palcode address of caveat */
	ERR_MSG	*entry;			
        char *decode_ev6_data;						/* palcode instruction ascii */
      	
	/* CASE  WRITE EV6__DATA  example code 
	 * test_30:
	 *	srl r19, #12, r20		
	 * 	srl r19, #6, r21		
	 * 	bis r31, #3 , r22 		;fls_xx #3= enable dcache set 1:0
	 * 	hw_mtpr r22, ev6__dc_ctl          ; set dcache enables before dstream
	 * 				          ;  access or icache preloading
	 * ;pvc starts check here
	 * 	mb                                ; make sure all dstream is done
	 * 					  ; Next 16 instructions must be cache block aligned
	 * 	.align 6, <1207895071>	 	  ; fill with nops  (47FF041F hex)
	 * 	br r31, L2                  	  ; skip, to get 4 sub-blks into ICACHE
	 * start: hw_mtpr r20, ev6__data   	  ;i1 start here when icache loaded<========== first occurrence
	 * 	nop				  ;i2
	 * 	br r31, L3                   	  ; skip over icache touch code
	 * L2:	br r31, L4                   	  ; skip, to get 4 sub-blks into ICACHE
	 * L3:	hw_mtpr r21, ev6__data		  ;i3 write next 6 bits of data 
	 * 	nop                               ;i4
	 * 	br r31, L5
	 * L4:	br r31, L6                    	  ; skip, to get 4 sub-blks into ICACHE
	 * L5:	hw_mtpr r19, ev6__data		  ;i5 write next 6 bits of data
	 * 	nop                               ;i6
	 * 	hw_mtpr r31, <ev6__exc_addr ! 16> ; dummy IPR write - sets SCBD bit 4
	 * 	hw_mtpr r31, <ev6__exc_addr ! 16> ; stalls until above write retires
	 * 	beq r31, L7                   	  ; Predicts fall through since in PALmode
         * pvc$ignore$1006:                       ; ignore PVC infinite loop message
	 * iloop:	br r31, iloop                     ; fools ibox predictor into infinite loop
	 * L6:	br r31, start			  ; br to execute code in now in icache
	 * L7:					  ;end
	 * ;pvc ends check here
	 * 
	 */

	/* We only search for the first occurrence of MT EV6__DATA,
	 * since the restriction allows any instructions in i1,i2,i3,i4,
	 * i5,i6 locations.  
	 * NOTE: for pvc will require i1 = hw_mtpr rx, EV6__DATA 
	 */
	id1 =  Mt + Data;				/* search for MT ev6__data */
	    if (entry = search_ev6_caveat (id1, caveat_list, addr_ev6_data)) {
	        addr_ev6_data = entry->errnum;		/* get palcode address */	
		decode_ev6_data = entry->explanation; 	/* get palcode instruction ascii */	
		found_ev6_data=TRUE;
	    } else found_ev6_data=FALSE;
	    free (entry);

	error=FALSE;	
	if (found_ev6_data==TRUE) {
	    i=0;						/* raw_code[0]->address = pal_entry */
	    while (raw_code[i] != 0) {   
	        if (raw_code[i]->address == addr_ev6_data) {
		    /* if hw_mtpr r21, ev6__data is at i then the aligned br must be  at i-4 */
		        j=i-4;
			if ((raw_code[j]->index == DO_BR) && (raw_code[j] != 0)) {
			         found_br = TRUE;
			    /* we are here in the example code:
			     *    	br r31, L2                ;j  skip, to get 4 sub-blks into ICACHE
	 		     * start: hw_mtpr r20, ev6__data   	  ;j+4   i1 start here when icache loaded
	 		     *     	nop			  ;j+5   i2  NOTE: This could be other than a nop
	 		     * 	        br r31, L3                ;j+6    skip over icache touch code
		             * L2:	br r31, L4                ;j+1  skip, to get 4 sub-blks into ICACHE
			     * L3:	hw_mtpr r21, ev6__data	  ;j+7  NOT CHECKED i3 write next 6 bits of data 
		 	     * 		nop                       ;j+8  NOT CHECKED i4 NOTE: This could be other than a nop
	 		     * 		br r31, L5                ;j+9
			     * L4:	br r31, L6                ;j+2  skip, to get 4 sub-blks into ICACHE
		             * L5:	hw_mtpr r19, ev6__data	  ;j+10  NOT CHECKED  i5 write next 6 bits of data
		 	     * 		nop                       ;j+11 NOT CHECKED   i6 NOTE: This could be other than a nop
			     * 	hw_mtpr r31, <ev6__exc_addr ! 16> ;j+12 dummy IPR write - sets SCBD bit 4
	 		     * 	hw_mtpr r31, <ev6__exc_addr ! 16> ;j+13 stalls until above write retires
	 		     * 	        beq r31, L7               ;j+14 Predicts fall through since in PALmode
         		     * pvc$ignore$1006:                   ;     NOT CHECKED optional label ignore PVC infinite loop message
	 		     * iloop:	br r31, iloop             ;NOT CHECKED fools ibox predictor into infinite loop
			     *					  ;PVC gets here on permutation 1 but not 0
	 		     * L6:	br r31, start		  ;j+3  br to execute code in now in icache
			     * L7:				  ;      label NOT CHECKED 
			     */
			     if ((raw_code[j]->address & 0x0000003f) == 0) found_aligned_br=TRUE;
			     /* either j+4 or J+5 could be the hw_mtpr rx, ev6_data */
	    		     if (raw_code[j+4] != 0)
			     	 if (raw_code[j+4]->index == DO_HW_MTPR) found_mt_r20=TRUE;	/* check hw_mtpr r20,ev6__data  */
	    		     if (raw_code[j+5] != 0)
			     	 if (raw_code[j+5]->index == DO_HW_MTPR) found_mt_r20=TRUE;	/* check hw_mtpr r20,ev6__data  */
	    		     if (raw_code[j+6] != 0)
			     	 if (raw_code[j+6]->index == DO_BR) found_br_l3=TRUE;		/* check br r31,l3  */
	    		     if (raw_code[j+1] != 0)
			         if (raw_code[j+1]->index == DO_BR) found_br_l4=TRUE;		/* check l2: br r31,l4  */
			     /* j+7 not checked */
			     /* j+8 not checked */
	    		     if (raw_code[j+9] != 0)
			         if (raw_code[j+9]->index == DO_BR) found_br_l5=TRUE;		/* check br r31,l5  */
	    		     if (raw_code[j+2] != 0)
			         if (raw_code[j+2]->index == DO_BR) found_br_l6=TRUE;		/* check l4: br r31,l6  */
			     /* j+10 not checked */
			     /* j+11 not checked */
	    		     if (raw_code[j+12] != 0)
			     	 if (raw_code[j+12]->index == DO_HW_MTPR) found_mt_dummy1=TRUE; /* check hw_mtpr r31, <ev6__exc.. */
	    		     if (raw_code[j+13] != 0)
			     	 if (raw_code[j+13]->index == DO_HW_MTPR) found_mt_dummy2=TRUE; /* check hw_mtpr r31, <ev6__exc.. */
	    		     if (raw_code[j+14] != 0)
			         if (raw_code[j+14]->index == DO_BEQ) found_br_l7=TRUE;		/* check l4: br r31,l7  */
	    		     if (raw_code[j+3] != 0)
			         if (raw_code[j+3]->index == DO_BR) found_br_start=TRUE;	/* check L6:     br r31, start*/
			}
		    if (found_br==TRUE) {
		        for (k=j-1; k>0 ;k--) {
		            if ((raw_code[k]->index == DO_MB) &&                                /* check for MB */
			        (raw_code[k]->bits.instr & 0X400) == 0){    /* not WMB */
		                    found_mb=TRUE;
		            }
		        }
		    }
		    break;
	   	 } /* if (raw_code...)*/
	         i++;
	     } /* while ... */

             if ((found_mb==FALSE     || found_aligned_br==FALSE || found_mt_r20==FALSE || found_br_l3==FALSE ) || 
		 (found_br_l4==FALSE  || found_br_l5==FALSE || found_br_l6==FALSE  || found_mt_dummy1==FALSE )  || 
		 (found_mt_dummy2==FALSE || found_br_l7==FALSE || found_br_start==FALSE))
		    error=TRUE;
	} /* if (found_ev6_data==TRUE... */

        if (error == TRUE ) {	
	    restriction_num= 30;
	    if (new_error (addr_ev6_data, restriction_num)) {
	            wr (FLG$ERR, "%s\n", pheader);
	     	    wr (FLG$ERR, "***\n");
	    	    wr (FLG$ERR, "Error at %X: %s\n",  addr_ev6_data,decode_ev6_data);
	    	    wr (FLG$ERR, "(PVC #%d)  RESTRICTION #30: \n",restriction_num);
		  if (found_aligned_br==FALSE)
	                  wr (FLG$ERR, "           Cache Block aligned branch similar to br r31, L2\n");
		  else if (found_mb==FALSE)
	                  wr (FLG$ERR, "           Memory Barrier MB instruction\n");
		  else if (found_mt_r20==FALSE)
	                  wr (FLG$ERR, "           Hw_mtpr rx, EV6_DATA \n");
		  else if (found_br_l3==FALSE)
	                wr (FLG$ERR, "             Similar code to  br r31, L3 \n");
		  else if (found_br_l4==FALSE)
	                wr (FLG$ERR, "             br r31, L4 or similar code\n");
		  else if (found_br_l5==FALSE)
	                wr (FLG$ERR, "             br r31, L5 or similar code\n");
		  else if (found_br_l6==FALSE)
	                wr (FLG$ERR, "             br r31, L6 or similar code\n");
		  else if (found_mt_dummy1==FALSE)
	                wr (FLG$ERR, "             hw_mtpr r31, <ev6__exc_addr!16> or similar code\n");
		  else if (found_mt_dummy2==FALSE)
	                wr (FLG$ERR, "             hw_mtpr r31, <ev6__exc_addr!16> or similar code\n");
		  else if (found_br_start==FALSE)
	                wr (FLG$ERR, "             L6::br r31, start or similar code\n");

	                wr (FLG$ERR, "           not found or not found where expected.\n");
	    	        wr (FLG$ERR, "           All external bus activity must be stopped before writing\n");
	                wr (FLG$ERR, "           to the EV6__DATA IPR. \n");
	                wr (FLG$ERR, "Suggestion: The Memory barrier instruction MB will stop Dstream activity.\n");
	                wr (FLG$ERR, "           Stopping Istream activity is more difficult, but can be done\n");
	                wr (FLG$ERR, "           using this code example:.\n");
	    	        wr (FLG$ERR, "\n");
	                wr (FLG$ERR, "; r19,r20,r21 contain ev6__data on entry\n");
	                wr (FLG$ERR, "        mb                        ; ensure all dstream is done\n");
	                wr (FLG$ERR, "; Next 16 instructions must be cache block aligned\n");
	                wr (FLG$ERR, ".align 6, <1207895071>            ; fill with nops (47FF041F hex)\n");
	                wr (FLG$ERR, "        br r31, L2                ; skip, to get 4 sub-blks into ICACHE\n");
	                wr (FLG$ERR, " start: hw_mtpr r20, ev6__data    ; i1 start here after icache loaded\n");
	                wr (FLG$ERR, "        nop                       ; i2\n");
	                wr (FLG$ERR, "        br r31, L3                ; skip over ICACHE touch code\n");
	                wr (FLG$ERR, "L2:     br r31, L4                ; skip, to get 4 sub-blks into ICACHE\n");
	                wr (FLG$ERR, "L3:     hw_mtpr r21, ev6__data    ; i3 write next 6 bits of data\n");
	                wr (FLG$ERR, "        nop                       ; i4\n");
	                wr (FLG$ERR, "        br r31, L5\n");
	                wr (FLG$ERR, "L4:     br r31, L6                ; skip, to get 4 sub-blks into ICACHE\n");
	                wr (FLG$ERR, "L5:     hw_mtpr r19, ev6__data    ; i5 skip, to get 4 sub-blks into ICACHE\n");
	                wr (FLG$ERR, "        nop                       ; i6\n");
	                wr (FLG$ERR, "    hw_mtpr r31, <ev6__exc_addr ! 16> ; dummy IPR  write - sets SCBD bit 4\n");
	                wr (FLG$ERR, "    hw_mtpr r31, <ev6__exc_addr ! 16> ; stalls until above write retires\n");
	                wr (FLG$ERR, "        beq r31, L7               ; Predicts fall through since in PALmode\n");
	                wr (FLG$ERR, "pvc$ignore$1006:                  ; ignore PVC infinite loop message\n");
	                wr (FLG$ERR, "iloop:   br r31, iloop            ; fools predictor into infinite loop\n");
	                wr (FLG$ERR, "L6:      br r31, start            ; br to execute code now in ICACHE\n");
	    	        wr (FLG$ERR, "l7:\n");
	                wr (FLG$ERR, "NOTE: To force PVC to ignore this error, put the ignore label at\n");
	                wr (FLG$ERR, "address %x: \n",addr_ev6_data);
	    	        wr (FLG$ERR, "\n***\n");
	                pal_error_count++;
	    } /* if (new_error ... */
        } /* if (error == TRUE) */
}

void check_ev6_restriction_33 (INSTR *raw_code[], int offs, int *id, int poss, int count)
/*
 ********************************************************************************
 *	PURPOSE:
 *		Check EV6 restriction #33.
 * 
 * (33) Restriction: HW_LD physical/lock usage
 * 
 *    The HW_LD physical/lock instruction must be located as one of the
 *    first 3 instructions in a quad-instruction aligned fetch block. A
 *    pipeline error can occur if the HW_LD physical/lock is fetched as the
 *    4th instruction of the fetch block. 
 *
 *	INPUT PARAMETERS:
 *		raw_code: pointer to the array of instructions from follow_code
 *		offs: where we are in the above array
 *		id:   array of ipr info such as scoreboard bits
 *		count: Number of Longwords in fetch block. Max = 4.
 *	OUTPUT PARAMETERS:
 *		None
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:            
 *	AUTHOR/DATE:
 *	V3.24 FLS Feb 97 
 ********************************************************************************
 */
{
  int i, index[4];

  /* if hw_ld locked, it must not be in fourth fetch block offset =3 */
  /* fetch block [0] - hw_ld lock ok */
  /* fetch block [1] - hw_ld lock ok */
  /* fetch block [2] - hw_ld lock ok */
  /* fetch block [3] - if hw_ld lock error */

  i=3;							
  if (count == 4) {					/* requires 4 inst in fetch block */
    index[i] = raw_code[offs+i]->index;			/* get unique inst. index */

    if ((index[i] == DO_HW_LD) &&
	(raw_code[offs+i]->bits.ev6_hwld_format.type == 1 )) { 		/* hw_ld/physical/lock */

	restriction_num= 33;
        if (new_error (raw_code[offs+i]->address, restriction_num)) {
            wr (FLG$ERR, "%s\n", pheader);
	    wr (FLG$ERR, "***\n");
	    wr (FLG$ERR, "Error at %X: %s\n", 
		raw_code[offs+i]->address,raw_code[offs+i]->decoded);
	    wr (FLG$ERR, "(PVC #%d) RESTRICTION #33: \n",restriction_num);
	    wr (FLG$ERR, "          A HW_LD Physical/lock cannot be in the fourth position of\n");
	    wr (FLG$ERR, "          a fetch block in a PAL routine.\n");
	    wr (FLG$ERR, "          Suggestion: Add a NOP to ensure the Hw_ldlpl or hw_ldqpl\n");
	    wr (FLG$ERR, "                      is moved to another position in the\n");
	    wr (FLG$ERR, "                      fetch block (octaword-aligned octaword).\n***\n");
            pal_error_count++;
        }
    } /* if ((index[i] == DO_HW_LD... */
  } /* if (count == 4)... */
}

void check_ev6_restriction_34 (INSTR *raw_code[], int offs, List *caveat_list)
/*
 ********************************************************************************
 *	PURPOSE:
 *	    Check end of permutation restriction 34
 *
 *
 * (34) Restriction: Writing multiple ITB entries in the same PAL flow
 *
 *   If a PALcode flow intends to write multiple ITB entries, additional
 *   scoreboard bits should be set to avoid possible corruption of the
 *   TAG ipr prior to final update in the ITB. The addition of scoreboard
 *   bits 0 & 4 to the standard scoreboard bit 6 for ITB_TAG will prevent
 *   subsequent MTPR ITB_TAG writes from changing the staging
 *   register TAG value prior to retirement of the MTPR ITB_PTE,
 *   which triggers the final ITB update. 
 *
 *
 *	INPUT PARAMETERS:
 *		raw_code: pointer to the array of instructions from follow_code
 *		offs: where we are in the above array
 *		      Instruction at offs=0 is the pal entry instruction.
 *		caveat_list: pointer to caveat list
 *
 *	OUTPUT PARAMETERS:
 *		None
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:            
 *	AUTHOR/DATE:
 *      V3.26 FLS Apr-97 
 ********************************************************************************
 */
{
	int  idtmp=0;
	int i;
	int id1;
	int addr1=0;			/* palcode address of caveat */
	int addr2=0;			
	int found_Itb_tag=TRUE;
	int found_addr1;
	int found_itb_tag_2;
        int found_scbd0_4=0;
	int index;
	int ipr_num;
	int error= FALSE;
	ERR_MSG	*entry;			
	char *decode1;			/* palcode instruction ascii */
	char *decode2;			/* palcode instruction ascii */
	
	id1 = Mt + Itb_tag;		/* search for Itb_tag */

	    if (entry = search_ev6_caveat (id1, caveat_list, addr1)) {
	        addr1 = entry->errnum;		/* get palcode address */	
		decode1 = entry->explanation; 	/* get palcode instruction ascii */	
		found_Itb_tag=TRUE;
	    } else found_Itb_tag=FALSE;
	    free (entry);

	    /* search raw_code[offs] for addr1 then search for another MT EV6__ITB_TAG instruction.
	     * Restriction 34 only applies if there are more than one MT  EV6__ITB_TAG per entry point,
	     * so if no more found all is well.
	     * If another is found keep searching until the last one in this entry is found.
	     * When the last MT EV6__ITB_TAG is found then check that it has the additional scoreboard bits
	     * 0 & 4 are set. These will have to be set by or'ing ^x11 with the instruction, since the default
	     * is to only set scoreboard bit 6.
	     */
            found_itb_tag_2=FALSE;
            found_scbd0_4=FALSE;
            found_addr1=FALSE;
	    if (found_Itb_tag==TRUE) {
	        i=0;
      	        while (raw_code[i] != 0) {
	   	    if (raw_code[i]->address == addr1) {
			offs = i+1;
      	    	        while (raw_code[offs] != 0) {		
			    index = raw_code[offs]->index;		     
			    if (index == DO_HW_MTPR) {
				ipr_num = raw_code[offs]->bits.ev6_hwmxpr_format.index;
				if (ipr_num == EV6__ITB_TAG) {
				    if ((raw_code[offs]->bits.ev6_hwmxpr_format.scbd & Scbd0) && 
				        (raw_code[offs]->bits.ev6_hwmxpr_format.scbd & Scbd4)) 	found_scbd0_4=TRUE; /* all is well */
				    else found_scbd0_4=FALSE;
				    found_itb_tag_2 = TRUE;
				    addr2 = raw_code[offs]->address;			
				    decode2=raw_code[offs]->decoded;
				}
			    }
	                    offs++;
	   	        } /* while ... */
	   	     } /* if (raw_code...)*/
	             i++;
	         } /* while ... */

	    if (found_Itb_tag==TRUE && found_itb_tag_2 == TRUE && found_scbd0_4==FALSE)	
	           error=TRUE;
	    else   error=FALSE;
	  } /* 	if (found_Itb_tag==TRUE... */

          if (error == TRUE ) {	
	    restriction_num= 34;
	    if (new_error (addr1, restriction_num )) {
	         	wr (FLG$ERR, "%s\n", pheader);
	     	    wr (FLG$ERR, "***\n");
	            wr (FLG$ERR, "Error at %X: %s\n",  addr2,decode2);
	    	    wr (FLG$ERR, "(PVC #%d ) RESTRICTION #34: \n",restriction_num );
	    	    wr (FLG$ERR, "           The last Hw_mtpr ITB_TAG in the same PALcode entry point flow\n");
	    	    wr (FLG$ERR, "           must set additional scoreboard bits 0 & 4 \n");
	    	    wr (FLG$ERR, "           \n");
	    	    wr (FLG$ERR, "           Suggestion: Add the scoreboard bits 0 & 4 to the\n");
	    	    wr (FLG$ERR, "                       last Hw_mtpr ITB_TAG instruction.\n");
	    	    wr (FLG$ERR, "           \n");
	    	    wr (FLG$ERR, "                       Here is a code example to follow:\n");
	    	    wr (FLG$ERR, "           \n");
	    	    wr (FLG$ERR, "                       hw_mtpr r6, <EV6__ITB_TAG ! ^x11> ;\n");
	    	    wr (FLG$ERR, "\n***");
	            pal_error_count++;
	    } /* if (new_error ... */
          } /* if (error == TRUE) */
}

