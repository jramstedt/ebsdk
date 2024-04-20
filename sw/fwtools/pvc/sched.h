/* sched.h */

/*
 *	
 *	Copyright (c) 1992
 *	Digital Equipment Corporation, Maynard, Massachusetts 01754
 *	
 *	This software is furnished under a license for use only on a single computer
 *	system  and  may be  copied  only  with the inclusion of the above copyright
 *	notice.  This software, or  any other copies thereof, may not be provided or
 *	otherwise made  available  to any other person except for use on such system
 *	and to one who agrees to these license terms.  Title to and ownership of the
 *	software shall at all times remain in DEC.
 *	
 *	The information  in  this software  is  subject to change without notice and
 *	should not be construed as a commitment by Digital Equipment Corporation.
 *	
 *	DEC  assumes no responsibility for the use or reliability of its software on
 *	equipment which is not supplied by DEC.
 *	
 *	
 *	REVISION HISTORY:		
 *	
 *	Rev	Who	When		What		
 *	----------------------------------------------------------------------
 *	  0     gpc     AUG-92          First Created
 *	
 */

#ifndef SCHED_H
#define SCHED_H 1
#define MAX_EV6_PERMUTATION 4096;	/* should be MAXFLOW-4 max ev6 permutation length */
/* destination registers */
enum dest_regs {Noreg=1, Ra=2, Rc=3};

#define chop_space(str)	{char *p=str;while(*p!='\0')p++;while(isspace(*--p))*p='\0';}
#define x3or(a, b, c) (((a) && !(b) && !(c)) || (!(a) && (b) && !(c)) || (!(a) && !(b) && (c)))

/*
 * Since the architected classes of instructions does not always correspond exactly
 * with the classes of instructions that behave the same (multiple-issue wise) for a
 * given implementation, each instruction has to have its own instruction class pointer
 */
#define AssignUnqIcp(idx) {\
\
  temp = (struct class_info *) malloc (sizeof (struct class_info));\
  *temp = *icp;\
  instr_array[idx] = temp;\
}  

void get_next_branch (NODE *, INSTR *next_flow[]);
void follow_code (NODE *); 
int follow_branches (int, NODE *); 
int follow_conditionals (NODE *); 
int follow_jump (NODE *); 
int is_cond_or_jsr (int); 
int is_br_or_bsr (int); 
void alpha_init_classes(void); 

#endif
