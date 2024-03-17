/* types.h */

/*
 *	
 *	Copyright (c) 1992, 1993
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
 *        2     pir     JAN-93		EV5 mods (extended CYCLE definition)
 *	  1	sgm	SEP-92		Added a few new types
 *	  0     gpc     AUG-92          First Created
 *	
 * $log$
 */

#ifndef TYPES_H
#define TYPES_H

/* global include's */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "evstruct.h"
#include "list.h"

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif
#define PAGE_WIDTH 80
#define PAGE_LENGTH 24

/* flag settings */
enum flags {FLG$TRACE  = (1<<0), FLG$BRANCH  = (1<<1), FLG$SCHED1  = (1<<2),
	      FLG$SCHED2 = (1<<3), FLG$REST = (1<<4), FLG$WARN = (1<<5),
	      FLG$ERR = (1<<6), FLG$PERM = (1<<7), FLG$CYCLES = (1<<8),
	      FLG$DEAD = (1<<9), FLG$DELAY = (1<<10), FLG$MEM = (1<<11), 
              FLG$FLOW = (1<<12), FLG$CHECKNOW = (1<<13), 
	      FLG$FREQ = (1<<14), FLG$DEBUG_DUMP = (1<<15)};

/* PVC Control flag settings */
enum PVC_CONTROL_FLAGS  {PVC_CTRL_CLEAR_FLAGS = 0, PVC_CTRL_DONT_FOLLOW_BSR = (1<<0),
                       PVC_CTRL_GO_NO_FURTHER = (1<<1)};

typedef struct error_msg {
      int 	errnum;
      char 	*explanation;
} ERR_MSG;

typedef struct architected {
      int 	op_ra;
      int 	op_rb;
      int 	op_rc;
      int 	ra_flt;
      int 	rb_flt;
      int 	rc_flt;
      int 	dest_reg;
      int 	dest_flt;
      int 	chk_lit_mode;
} ARCHITECTED;

typedef struct implemented {
      int 	di_class;
      int 	lat_class;
} IMPLEMENTED;
      
typedef struct class_info {
      struct architected      	*arch;
      struct implemented	*imp;
} CLASS_INFO;

typedef struct instr {
      int 	address;
      int 	index;
      union INSTR_FORMAT bits;
      char 	*decoded;
      unsigned int reached;
      List 	*jsr_dest_list;
      List	*bsr_history;
      List	*bsr_stack_clone_list;
      int 	use_bsr_stack;
      enum PVC_CONTROL_FLAGS  PVC_Control_Flag;        /* RRC: Communicates to PVC how to handle this instruction in a different fashion than normal */
      int   use_ca;         /* EV6 if 1 use L0,L1 else U0,U1 pipes */
      int	issued;	    /* EV6 if 1 inst has been issued */
      int   ipr_stall;	    /* EV6 ipr scoreboard bits */
      int mra;		    /* EV6 mapped ra dest register*/
      int mrb;		    /* EV6 mapped rb dest register*/
      int mrc;		    /* EV6 mapped rc dest register*/
      int oldrc;	    /* EV6 previous mapped rc dest register*/
      int  slot_only;	    /* EV6 hack to fill out fetch block for slot() */
      int  entry_busy;	    /* EV6 entry used - if same address needed clone using malloc */
      int  cloned;	    /* EV6 entry cloned using malloc , must be freed*/
      long  e_nullify_time; /* EV6 cycle when iq spec inst will retire pvc only uses for nops*/
      long  f_nullify_time; /* EV6 cycle when fq spec inst will retire pvc only uses for nops*/
      long  retire_time;    /* EV6 cycle when inst will retire */
      long  fpst_valid;     /* EV6 used to make fbox signal ibox to issue stores */
      long inst_num;	    /* EV6 inum */
} INSTR;

typedef struct Chip {
      char 	name[8];
      int	initialized;
      void 	(*chip_init) (void);
      void 	(*restriction_check) (int);
      int 	(*schedule_code) (INSTR *[], int, int *);
      void	(*decode_ipr) (disp_table *);
      void	(*decode_hw_memory) (disp_table *);
      void 	(*at_pal_entry) (void);			/*V3.24*/
      void 	(*at_pal_return) (void);		/*V3.24*/
} Chip;

typedef struct cycle {
      INSTR 	*is1;
      INSTR 	*is2;
      INSTR 	*is3;
      INSTR 	*is4;
} CYCLE;

typedef struct node {
      ListCommonFields;
      struct node 	*previous;
      struct node 	*inline;
      struct node 	*branch;
      int       bsr_addr_hash;
      List 	*jump_dests;
      INSTR 	*instruction;
} NODE;

typedef struct Address {
      ListCommonFields;
      int 	addr;
      int 	data;
} Address;

typedef struct Exception {
      ListCommonFields;
      int 	addr;
      int 	errnum;
} Exception;

typedef struct Bsr_Stack_List {
      ListCommonFields;
      List	*bsr_stack;
      int	bsr_addr_hash;
} Bsr_Stack_List;

typedef struct Go_No_Further {
      ListCommonFields;
      int	addr;
      int	bsr_addr_hash;
} Go_No_Further;

typedef struct Bsr_Stack {
      ListCommonFields;
      int 	pc_plus_four;
      int 	bsr_family;
} Bsr_Stack;

typedef struct Bsr_History {
      ListCommonFields;
      int	bsr_addr_hash;
} Bsr_History;

typedef struct GotoInfo {
      ListCommonFields;
      int 	address;
      int 	set;
      int 	source;
      List 	*destinations;
} GotoInfo;

typedef struct Restriction {
      ListCommonFields;
      int 	event;
      int 	prohibited_event;
      int 	cycles;
      char 	*msg;
} Restriction;

typedef struct Entry {
      ListCommonFields;	
      int 	offset;
      char 	name[101];
} Entry;


typedef struct {
  int   busy;
  INSTR *inst;
} FDIV;
 
#endif


