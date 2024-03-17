/* common.c */

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
 *	Who	When		What		
 *	---------------------------------------------------------------
 *V3.24 fls	SEP-1996	Added new EV6 instructions.
 *				call decode_hw_memory for ev4/ev5 hw_rei to allow
 *				ev6 hw_ret decode. Not strictly hw_mem 
 *				format, but saves creating new routines.
 *				Changed WR_FPCTL to MT_FPCR
 *					RD_FPCTL to MF_FPCR to match latest 
 *					ALPHA SRM.
 *	GPC 	4-aug-1992	Wedged into PVC
 *	HA	03-Apr-1992	EV5
 *	HA	18-Feb-1992	Move opcode_table definition
 *	DRM	21-DEC-1988	Add decode_operate_2, decode_hw_memory,decode_hw_rei, decode_ipr.
 *	SJM	15-Nov-1988	First pass 
 */

#include <stdio.h>
#include "ispdef.h"
#include <ctype.h>
#include "types.h"
#include "alpha_op.h"	/*V3.26 renamed*/
#include "opcode.h"
  
/* declare the routines we use for decode */
void decode_by_subfun();
void decode_by_memfun();
void decode_by_epi_subfun();
void decode_by_fpu_subfun();
void decode_operate_ra_fc();
void decode_operate_fa_rc();
void decode_operate_2();
void decode_operate_3();
void decode_operate_1f();
void decode_operate_2f();
void decode_operate_3f();
void decode_memory();
void decode_memoryf();
void decode_branch();
void decode_branchf();
void decode_epi();
void decode_epi_rn();
void decode_hw_memory(disp_table *);
void decode_ipr(disp_table *);
void decode_jsrm();

/* tables */
char reg_name_table[MAX_SREG][8];
char freg_name_table[MAX_SREG][8];

char ipr_name_table[MAX_IPR][32];				/* v3.25 */
char adu_ipr_name_table[MAX_ADU_IPR][16];
char pax_ipr_name_table[25][16];

disp_table	opcode_table[MAX_OPCODE];
disp_table	epi_func_table[MAX_EPI_SUBFUN];
disp_table	sync_func_table[MAX_SUBFUN];
disp_table	fpieee_func_table[MAX_FPU_SUBFUN];
disp_table	fpdec_func_table[MAX_FPU_SUBFUN];
disp_table	fpaux_func_table[MAX_FPU_SUBFUN];
disp_table	sqrt_func_table[MAX_FPU_SUBFUN];		/* V3.24 */

disp_table	addi_func_table[MAX_SUBFUN];
disp_table	logi_func_table[MAX_SUBFUN];
disp_table	sext_func_table[MAX_SUBFUN];			/* V3.24 */
disp_table	shfi_func_table[MAX_SUBFUN];
disp_table	muli_func_table[MAX_SUBFUN];
disp_table	itof_func_table[MAX_SUBFUN];			/* V3.24 */

/*---------------------------------------------------------------------------*/
/* Local Variables for this module:					     */
/*---------------------------------------------------------------------------*/
union INSTR_FORMAT instruction;
char  *text;
B64   *pc;
extern Chip *chip;

/*****************************************************************************/
/*ROUTINE ISP_DECOMPOSE:						     */
/*****************************************************************************/
int isp_decompose(union squad *pcptr,union INSTR_FORMAT *ins,char *buffer)
{
      void (*rtn)();
      instruction.instr = ins->instr;
      text = buffer;
      pc = pcptr;
      
      if(instruction.op_format.opcode < MAX_OPCODE)
	/* get addr of next rtn */
	rtn = opcode_table[instruction.op_format.opcode].decode; 
      else
	rtn = 0;
      if(rtn != 0)
	/* call it with next param */
	(*rtn)(opcode_table[instruction.op_format.opcode].next);
      else
	sprintf(text,"[Unknown opcode field]????????");			/*V3.24*/
      return(1);
}
/*****************************************************************************/
/*ROUTINE DECODE_BY_SUBFUN:						     */
/*****************************************************************************/
void decode_by_subfun(table)
disp_table *table;
{
      void (*rtn1)();
      if(instruction.op_format.func < MAX_SUBFUN)
	rtn1 = table[instruction.op_format.func].decode;	/* get addr of next rtn */
      else
	rtn1 = 0;
      if(rtn1 != 0)
	(*rtn1)(table[instruction.op_format.func].next);	/* call it with next param */
      else
	sprintf(text,"[Unknown Operate func field ]????????");	/*V3.24*/
      return;
}
/*****************************************************************************/
void decode_by_fpu_subfun(table)
disp_table *table;
{
      void (*rtn1)();
      if(instruction.fpu_format.func < MAX_FPU_SUBFUN)
	rtn1 = table[instruction.fpu_format.func].decode;	/* get addr of next rtn */
      else
	rtn1 = 0;
      if(rtn1 != 0)
	(*rtn1)(table[instruction.fpu_format.func].next);	/* call it with next param */
      else
	sprintf(text,"[Unknown fpu func field]????????");	/*V3.24*/
      return;    
}
/*****************************************************************************/
void decode_by_memfun(table)
disp_table *table;
{
      void (*rtn1)();
      int t;
      t = ((instruction.epi_format.func & 0xFFFF)>>10);
      if(t < MAX_SUBFUN)
	rtn1 = table[t].decode;			/* get addr of next rtn */
      else rtn1 = 0;
      if(rtn1 != 0)
	(*rtn1)(table[t].next);			/* call it with next param */
      else sprintf(text,"[Unknown memory func field]????????"); /*V3.24 */
      return;
}
/*****************************************************************************/
void decode_by_epi_subfun(table)
disp_table *table;
{
      void (*rtn1)();
      if (instruction.epi_format.func < MAX_EPI_SUBFUN)
	rtn1 = table[instruction.epi_format.func].decode;	/* get addr of next rtn */
      else rtn1 = 0;
      if (rtn1 != 0)
	(*rtn1)(table[instruction.epi_format.func].next);	/* call it with next param */
      else sprintf(text,"[Unknown pal func field]????????");	/*V3.24 */
      return;    
}
/*****************************************************************************/
/* For one operand operates like IMPLVER Rc */
void decode_operate_1(entry)
disp_table *entry;
{
      sprintf(text,"%-12s",entry->text);
      /*if (instruction.op_format.litflag == 0) */
      sprintf(&text[strlen(text)],"%s",
	      &reg_name_table[instruction.op_format.rc][0]);
      /*else
	sprintf(&text[strlen(text)],"#%d",instruction.opl_format.lit);
	*/
      return;    
}

/*****************************************************************************/
/* For FTOIT and FTOIS Fa,Rc format  */
void decode_operate_fa_rc(entry)				/*V3.24*/
disp_table *entry;
{
      sprintf(text,"%-12s",entry->text);
      sprintf(&text[strlen(text)],"%s, %s",
	      &freg_name_table[instruction.op_format.ra][0],
	      &reg_name_table[instruction.op_format.rc][0]);
      return;    
}
/*****************************************************************************/
/* For ITOFF/ITOFS/ITOFT  e.g. ITOFF Ra,Fc format  */
void decode_operate_ra_fc(entry)				/*V3.24*/
disp_table *entry;
{
      sprintf(text,"%-12s",entry->text);
      sprintf(&text[strlen(text)],"%s, %s",
	      &reg_name_table[instruction.op_format.ra][0],
	      &freg_name_table[instruction.op_format.rc][0]);
      return;    
}
/*****************************************************************************/
/* For two operand operates with literal mode allowed like AMASK */
void decode_operate_2(entry)				/*V3.24*/
disp_table *entry;
{
      sprintf(text,"%-12s",entry->text);
      if (instruction.op_format.litflag == 0) 
      sprintf(&text[strlen(text)],"%s, %s",
	      &reg_name_table[instruction.op_format.rb][0],
	      &reg_name_table[instruction.op_format.rc][0]);
      else
	sprintf(&text[strlen(text)],"#%d, %s",
	instruction.opl_format.lit,
	&reg_name_table[instruction.op_format.rc][0]);

      return;    
}
/*****************************************************************************/
void decode_operate_3(entry)
disp_table *entry;
{
      sprintf(text,"%-12s",entry->text);
      if(instruction.op_format.litflag == 0)
	sprintf(&text[strlen(text)],"%s, %s, %s",
		&reg_name_table[instruction.op_format.ra][0],
		&reg_name_table[instruction.op_format.rb][0],
		&reg_name_table[instruction.op_format.rc][0]);
      else
	sprintf(&text[strlen(text)],"%s, #%d, %s",
		&reg_name_table[instruction.op_format.ra][0],
		instruction.opl_format.lit,
		&reg_name_table[instruction.op_format.rc][0]);
      return;    
}
/*****************************************************************************/
void decode_operate_3f(entry)
disp_table *entry;
{
      sprintf(text,"%-12s",entry->text);
      /*if(instruction.op_format.litflag == 0) */
      sprintf(&text[strlen(text)],"%s, %s, %s",
	      &freg_name_table[instruction.op_format.ra][0],
	      &freg_name_table[instruction.op_format.rb][0],
	      &freg_name_table[instruction.op_format.rc][0]);
      /*else
	sprintf(&text[strlen(text)],"%s, #%d, %s",
	&freg_name_table[instruction.op_format.ra][0],instruction.opl_format.lit,
	&freg_name_table[instruction.op_format.rc][0]);
	*/	
      return;    
}
/*****************************************************************************/
/* For two operand operates like CVTxx */
void decode_operate_2f(entry)
disp_table *entry;
{
      sprintf(text,"%-12s",entry->text);
      /*if (instruction.op_format.litflag == 0) */
      sprintf(&text[strlen(text)],"%s, %s",
	      &freg_name_table[instruction.op_format.rb][0],
	      &freg_name_table[instruction.op_format.rc][0]);
      /*else
	sprintf(&text[strlen(text)],"%s, #%d",
	&freg_name_table[instruction.op_format.rc][0],
	instruction.opl_format.lit);
	*/
      return;    
}
/*****************************************************************************/
/* For two operand operates like FLBC */
void decode_operate_1f(entry)
disp_table *entry;
{
      sprintf(text,"%-12s",entry->text);
      /*if (instruction.op_format.litflag == 0) */
      sprintf(&text[strlen(text)],"%s",
	      &freg_name_table[instruction.op_format.ra][0]);
      /*else
	sprintf(&text[strlen(text)],"#%d",instruction.opl_format.lit);
	*/
      return;    
}
/*****************************************************************************/
void decode_memory(entry)
disp_table *entry;
{
      sprintf(text,"%-12s",entry->text);
      /*if (
	(instruction.op_format.opcode == EVX$OPC_STL)  ||
	(instruction.op_format.opcode == EVX$OPC_STQ)  ||
	(instruction.op_format.opcode == EVX$OPC_STUQ) ||
	(instruction.op_format.opcode == EVX$OPC_STF)  ||
	(instruction.op_format.opcode == EVX$OPC_STD)  ||
	(instruction.op_format.opcode == EVX$OPC_STS)  ||
	(instruction.op_format.opcode == EVX$OPC_STT)  ||
	(instruction.op_format.opcode == EVX$OPC_THAWL)  ||
	(instruction.op_format.opcode == EVX$OPC_THAWQ)  ||
	(instruction.op_format.opcode == EVX$OPC_JSR_M)
	)
	*/
      sprintf(&text[strlen(text)],"%s, %d(%s)",
	      &reg_name_table[instruction.op_format.ra][0],
	      instruction.mem_format.disp,
	      &reg_name_table[instruction.op_format.rb][0]);
      /*else
	sprintf(&text[strlen(text)],"%d(%s), %s",
	instruction.mem_format.disp,
	&reg_name_table[instruction.op_format.rb][0],
	&reg_name_table[instruction.op_format.ra][0]);
	*/
      return;    
}
/*****************************************************************************/
void decode_memoryf(entry)
disp_table *entry;
{
      sprintf(text,"%-12s",entry->text);
      /*if (
	(instruction.op_format.opcode == EVX$OPC_STL)  ||
	(instruction.op_format.opcode == EVX$OPC_STQ)  ||
	(instruction.op_format.opcode == EVX$OPC_STUQ) ||
	(instruction.op_format.opcode == EVX$OPC_STF)  ||
	(instruction.op_format.opcode == EVX$OPC_STD)  ||
	(instruction.op_format.opcode == EVX$OPC_STS)  ||
	(instruction.op_format.opcode == EVX$OPC_THAWL)  ||
	(instruction.op_format.opcode == EVX$OPC_THAWQ)  ||
	(instruction.op_format.opcode == EVX$OPC_STT)
	)
	*/
      sprintf(&text[strlen(text)],"%s, %d(%s)",
	      &freg_name_table[instruction.op_format.ra][0],
	      instruction.mem_format.disp,
	      &reg_name_table[instruction.op_format.rb][0]);
      /*else
	sprintf(&text[strlen(text)],"%d(%s), %s",
	instruction.mem_format.disp,
	&reg_name_table[instruction.op_format.rb][0],
	&freg_name_table[instruction.op_format.ra][0]);
	*/
      return;    
}
/*****************************************************************************/
/* For JSR with hints instrs */
void decode_jsrm(entry)
disp_table *entry;
{
      switch ((instruction.mem_format.disp>>14) & 3)
	{
	    case 00: sprintf(text,"%-12s","JMP");	break;
	    case 01: sprintf(text,"%-12s","JSR");	break;
	    case 02: sprintf(text,"%-12s","RET");	break;
	    case 03: sprintf(text,"%-12s","JSR_CORET");	break;
	    default: sprintf(text,"%-12s","JSR_?????");	break;
	};
      sprintf(&text[strlen(text)],"%s, (%s)",
	      &reg_name_table[instruction.op_format.ra][0],
	      &reg_name_table[instruction.op_format.rb][0]);
      return;    
}
/*****************************************************************************/
/* For the EPI mode loads/stores and memory format of JSR */
void decode_hw_memory(entry)
disp_table *entry;
{
      sprintf(text,"%-12s",entry->text);
      sprintf(&text[strlen(text)],"%s, %d(%s)",
	      &reg_name_table[instruction.op_format.ra][0],
	      instruction.hwmem_format.disp,
	      &reg_name_table[instruction.op_format.rb][0]);
      
      if(instruction.hwmem_format.quad)
	sprintf(&text[strlen(text)],",QUAD");
      else
	sprintf(&text[strlen(text)],",LONG");
      
      if(instruction.hwmem_format.physical)
	sprintf(&text[strlen(text)],",PHYSICAL");
      else
	sprintf(&text[strlen(text)],",VIRTUAL");
      
      if(instruction.hwmem_format.chk) sprintf(&text[strlen(text)],",MMCheck");
      if(instruction.hwmem_format.alt_mode) sprintf(&text[strlen(text)],",ALT_MODE");
      return;    
}
/*****************************************************************************/
void decode_branch(entry)
disp_table *entry;
{
      sprintf(text,"%-12s",entry->text);
      sprintf(&text[strlen(text)],"%s, %x",
	      &reg_name_table[instruction.op_format.ra][0],
	      (instruction.bra_format.disp<<2) + pc->ulong[0] + 4);
      return;
}
/*****************************************************************************/
void decode_branchf(entry)
disp_table *entry;
{
      sprintf(text,"%-12s",entry->text);
      sprintf(&text[strlen(text)],"%s, %x",
	      &freg_name_table[instruction.op_format.ra][0],
	      (instruction.bra_format.disp<<2) + pc->ulong[0] + 4);
      return;
}
/*****************************************************************************/
/* for HW_MTPR, HW_MFPR instructions */
void decode_ipr(entry)
disp_table *entry;
{
      sprintf(text,"%-12s", entry->text);
      /*if (instruction.op_format.opcode == EVX$OPC_HW_MTPR) */
      sprintf(&text[strlen(text)],"%s, ",
	      &reg_name_table[instruction.op_format.ra][0]);
      
      sprintf(&text[strlen(text)],"%s",
	      &ipr_name_table[instruction.hwm_format.func][0]);
      
      /*else
	sprintf(&text[strlen(text)],"%s, %s",
	&ipr_name_table[instruction.epi_format.func][0],
	&reg_name_table[instruction.op_format.ra][0]);
	*/    
      return;
}
/*****************************************************************************/
/*****************************************************************************/
void decode_epi(entry)
disp_table *entry;
{
      sprintf(text,"%-12s",entry->text);
}

void decode_epi_rn(entry)
disp_table *entry;
{
      sprintf(text,"%-12s	(%s)",
	      entry->text,&reg_name_table[instruction.op_format.rb][0]);
}



int	parse_get_instr_idx(union INSTR_FORMAT *instr)
/*
 ********************************************************************************
 *	PURPOSE:
 *	        Given a 32-bit EVAX instruction, parses it using the instruction
 *	        decode tables (defined in ispdef.h) and returns an integer which 
 *	        uniquely specifies the instruction.  This integer is used by the rest
 *	        of the performance  model as an index into various arrays.
 *	INPUT PARAMETERS:
 *	        instr:	32-bit machine instruction
 *	OUTPUT PARAMETERS:
 *	        idx:	Integer which uniquely identifies the instruction
 *	IMPLICIT INPUTS:
 *	        Use intruction decode tables defined in evaxdef.h.
 *	IMPLICIT OUTPUTS:
 *	        None.
 *	AUTHOR/DATE:
 *	        Dirk Meyer,   2-Mar-89
 ********************************************************************************
 */
{
      unsigned int	opcode, subfunc, idx;
      disp_table	*tp;
      
      opcode = instr->op_format.opcode;
      
      if (opcode_table[opcode].action == DO_DISP_BY_FUNC) {
	    tp = opcode_table[opcode].next;
	    subfunc = instr->op_format.func;
	    idx = (tp+subfunc)->action;	  
      }
      else if (opcode_table[opcode].action == DO_DISP_BY_FPU_FUNC) {
	    tp = opcode_table[opcode].next;
	    subfunc = instr->fpu_format.func;
	    idx = (tp+subfunc)->action;	  
      }
      else if (opcode_table[opcode].action == DO_DISP_BY_MEM_FUNC) {
	    tp = opcode_table[opcode].next;
	    subfunc = (instr->epi_format.func & 0xFFFF)>>10;
	    idx = (tp+subfunc)->action;	  
      }
      else {
	    idx = opcode_table[opcode].action;
      }
      
      return(idx);
}

int fill_common_tables (void)
/*
 ********************************************************************************
 *	PURPOSE:
 *	INPUT PARAMETERS:
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Greg Coladonato, Aug 92
 ********************************************************************************
 */
{
      int i;
      
      /* This part gets executed to fill in the tables at run time */
      /* set the register names to "Rn" */
      
      for (i=0; i < MAX_SREG; i++)
	if (reg_name_table[i][0] == 0)
	  sprintf(reg_name_table[i],"R%d",i);
      
      for (i=0; i < MAX_SREG; i++)
	if (freg_name_table[i][0] == 0)
	  sprintf(freg_name_table[i],"F%d",i);
      
      /* start loading all the tables */
      
      /* point all the next fields at themselves by default */
      for(i = 0; i < MAX_OPCODE; i++)
	opcode_table[i].next = &opcode_table[i];
      
      for(i = 0; i < MAX_EPI_SUBFUN; i++)
	epi_func_table[i].next = &epi_func_table[i];
      
      for(i = 0; i < MAX_FPU_SUBFUN; i++)
	{
	      fpieee_func_table[i].next = &fpieee_func_table[i];
	      fpdec_func_table[i].next  = &fpdec_func_table[i];
	      fpaux_func_table[i].next  = &fpaux_func_table[i];
	      sqrt_func_table[i].next = &sqrt_func_table[i];
	};
      
      for(i = 0; i < MAX_SUBFUN; i++)
	{
	      sync_func_table[i].next = &sync_func_table[i];
	      addi_func_table[i].next = &addi_func_table[i];
	      logi_func_table[i].next = &logi_func_table[i];
	      sext_func_table[i].next = &sext_func_table[i];
	      shfi_func_table[i].next = &shfi_func_table[i];
	      muli_func_table[i].next = &muli_func_table[i];
	};
      
      opcode_table[EVX$OPC_PAL].decode = decode_by_epi_subfun;
      opcode_table[EVX$OPC_PAL].next	  = epi_func_table;
      opcode_table[EVX$OPC_PAL].action = DO_EPICODE;
      
      epi_func_table[EVX$PAL_BPT].decode	= decode_epi;
      epi_func_table[EVX$PAL_BPT].text	= "BPT";
      epi_func_table[EVX$PAL_CHME].decode	= decode_epi;
      epi_func_table[EVX$PAL_CHME].text	= "CHME";
      epi_func_table[EVX$PAL_CHMK].decode	= decode_epi;
      epi_func_table[EVX$PAL_CHMK].text	= "CHMK";
      epi_func_table[EVX$PAL_CHMS].decode	= decode_epi;
      epi_func_table[EVX$PAL_CHMS].text	= "CHMS";
      
      epi_func_table[EVX$PAL_CHMU].decode	= decode_epi;
      epi_func_table[EVX$PAL_CHMU].text	= "CHMU";
      epi_func_table[EVX$PAL_DRAINA].decode	= decode_epi;
      epi_func_table[EVX$PAL_DRAINA].text	= "DRAINA";
      epi_func_table[EVX$PAL_HALT].decode	= decode_epi;
      epi_func_table[EVX$PAL_HALT].text	= "HALT";
      epi_func_table[EVX$PAL_IMB].decode	= decode_epi;
      epi_func_table[EVX$PAL_IMB].text	= "IMB";
      
      epi_func_table[EVX$PAL_INSQHIL].decode	= decode_epi;
      epi_func_table[EVX$PAL_INSQHIL].text	= "INSQHIL";
      epi_func_table[EVX$PAL_INSQTIL].decode	= decode_epi;
      epi_func_table[EVX$PAL_INSQTIL].text	= "INSQTIL";
      epi_func_table[EVX$PAL_INSQHIQ].decode	= decode_epi;
      epi_func_table[EVX$PAL_INSQHIQ].text	= "INSQHIQ";
      epi_func_table[EVX$PAL_INSQTIQ].decode	= decode_epi;
      epi_func_table[EVX$PAL_INSQTIQ].text	= "INSQTIQ";
      
      epi_func_table[EVX$PAL_INSQUEL].decode	= decode_epi;
      epi_func_table[EVX$PAL_INSQUEL].text	= "INSQUEL";
      epi_func_table[EVX$PAL_INSQUEQ].decode	= decode_epi;
      epi_func_table[EVX$PAL_INSQUEQ].text	= "INSQUEQ";
      epi_func_table[EVX$PAL_INSQUELD].decode	= decode_epi;
      epi_func_table[EVX$PAL_INSQUELD].text	= "INSQUELD";
      epi_func_table[EVX$PAL_INSQUEQD].decode	= decode_epi;
      epi_func_table[EVX$PAL_INSQUEQD].text	= "INSQUEQD";
      
      epi_func_table[EVX$PAL_LDQP].decode	= decode_epi;
      epi_func_table[EVX$PAL_LDQP].text	= "LDQP";
      epi_func_table[EVX$PAL_MFPR_ASN].decode	= decode_epi;
      epi_func_table[EVX$PAL_MFPR_ASN].text	= "MFPR_ASN";
      /*	epi_func_table[EVX$PAL_MFPR_ASTEN].decode = decode_epi;	*/
      /*	epi_func_table[EVX$PAL_MFPR_ASTEN].text	= "MFPR_ASTEN";	*/
      /*	epi_func_table[EVX$PAL_MFPR_ASTSR].decode = decode_epi;	*/
      /*	epi_func_table[EVX$PAL_MFPR_ASTSR].text	= "MFPR_ASTSR";	*/
      
      /*	epi_func_table[EVX$PAL_MFPR_AT].decode	= decode_epi;	*/
      /*	epi_func_table[EVX$PAL_MFPR_AT].text	= "MFPR_AT";	*/
      epi_func_table[EVX$PAL_MFPR_FEN].decode	= decode_epi;
      epi_func_table[EVX$PAL_MFPR_FEN].text	= "MFPR_FEN";
      /*	epi_func_table[EVX$PAL_MFPR_IPIR].decode = decode_epi;	*/
      /*	epi_func_table[EVX$PAL_MFPR_IPIR].text	= "MFPR_IPIR";	*/
      epi_func_table[EVX$PAL_MFPR_IPL].decode	= decode_epi;
      epi_func_table[EVX$PAL_MFPR_IPL].text	= "MFPR_IPL";
      
      epi_func_table[EVX$PAL_MFPR_MCES].decode = decode_epi;
      epi_func_table[EVX$PAL_MFPR_MCES].text	= "MFPR_MCES";
      epi_func_table[EVX$PAL_MFPR_PCBB].decode = decode_epi;
      epi_func_table[EVX$PAL_MFPR_PCBB].text	= "MFPR_PCBB";
      epi_func_table[EVX$PAL_MFPR_PTBR].decode = decode_epi;
      epi_func_table[EVX$PAL_MFPR_PTBR].text	= "MFPR_PTBR";
      epi_func_table[EVX$PAL_MFPR_SISR].decode = decode_epi;
      epi_func_table[EVX$PAL_MFPR_SISR].text	= "MFPR_SISR";
      
      epi_func_table[EVX$PAL_MFPR_TBCHK].decode = decode_epi;
      epi_func_table[EVX$PAL_MFPR_TBCHK].text	= "MFPR_TBCHK";
      epi_func_table[EVX$PAL_MFPR_ESP].decode	= decode_epi;
      epi_func_table[EVX$PAL_MFPR_ESP].text	= "MFPR_ESP";
      epi_func_table[EVX$PAL_MFPR_SSP].decode	= decode_epi;
      epi_func_table[EVX$PAL_MFPR_SSP].text	= "MFPR_SSP";
      epi_func_table[EVX$PAL_MFPR_USP].decode	= decode_epi;
      epi_func_table[EVX$PAL_MFPR_USP].text	= "MFPR_USP";
      
      epi_func_table[EVX$PAL_MFPR_WHAMI].decode = decode_epi;
      epi_func_table[EVX$PAL_MFPR_WHAMI].text	= "MFPR_WHAMI";
      /*	epi_func_table[EVX$PAL_MTPR_ASTRR].decode = decode_epi;	*/
      /*	epi_func_table[EVX$PAL_MTPR_ASTRR].text	= "MTPR_ASTRR";	*/
      /*	epi_func_table[EVX$PAL_MTPR_AT].decode	= decode_epi;	*/
      /*	epi_func_table[EVX$PAL_MTPR_AT].text	= "MTPR_AT";	*/
      epi_func_table[EVX$PAL_MTPR_FEN].decode	= decode_epi;
      epi_func_table[EVX$PAL_MTPR_FEN].text	= "MTPR_FEN";
      
      epi_func_table[EVX$PAL_MTPR_IPL].decode	= decode_epi;
      epi_func_table[EVX$PAL_MTPR_IPL].text	= "MTPR_IPL";
      epi_func_table[EVX$PAL_MTPR_MCES].decode = decode_epi;
      epi_func_table[EVX$PAL_MTPR_MCES].text	= "MTPR_MCES";
      epi_func_table[EVX$PAL_MFPR_PRBR].decode = decode_epi;
      epi_func_table[EVX$PAL_MFPR_PRBR].text	= "MFPR_PRBR";
      epi_func_table[EVX$PAL_MTPR_PRBR].decode = decode_epi;
      epi_func_table[EVX$PAL_MTPR_PRBR].text	= "MTPR_PRBR";
      
      epi_func_table[EVX$PAL_MFPR_SCBB].decode = decode_epi;
      epi_func_table[EVX$PAL_MFPR_SCBB].text	= "MFPR_SCBB";
      epi_func_table[EVX$PAL_MTPR_SCBB].decode = decode_epi;
      epi_func_table[EVX$PAL_MTPR_SCBB].text	= "MTPR_SCBB";
      epi_func_table[EVX$PAL_MTPR_SIRR].decode = decode_epi;
      epi_func_table[EVX$PAL_MTPR_SIRR].text	= "MTPR_SIRR";
      epi_func_table[EVX$PAL_MTPR_TBIA].decode = decode_epi;
      epi_func_table[EVX$PAL_MTPR_TBIA].text	= "MTPR_TBIA";
      
      epi_func_table[EVX$PAL_MTPR_TBIS].decode = decode_epi;
      epi_func_table[EVX$PAL_MTPR_TBIS].text	= "MTPR_TBIS";
      epi_func_table[EVX$PAL_MTPR_ESP].decode	= decode_epi;
      epi_func_table[EVX$PAL_MTPR_ESP].text	= "MTPR_ESP";
      epi_func_table[EVX$PAL_MTPR_SSP].decode	= decode_epi;
      epi_func_table[EVX$PAL_MTPR_SSP].text	= "MTPR_SSP";
      epi_func_table[EVX$PAL_MTPR_USP].decode	= decode_epi;
      epi_func_table[EVX$PAL_MTPR_USP].text	= "MTPR_USP";
      
      /*	epi_func_table[EVX$PAL_PAST].decode	= decode_epi;	*/
      /*	epi_func_table[EVX$PAL_PAST].text	= "PAST";	*/
      epi_func_table[EVX$PAL_PROBER].decode	= decode_epi;
      epi_func_table[EVX$PAL_PROBER].text	= "PROBER";
      epi_func_table[EVX$PAL_PROBEW].decode	= decode_epi;
      epi_func_table[EVX$PAL_PROBEW].text	= "PROBEW";
      epi_func_table[EVX$PAL_RD_PS].decode	= decode_epi;
      epi_func_table[EVX$PAL_RD_PS].text	= "RD_PS";
      
      epi_func_table[EVX$PAL_REI].decode	= decode_epi;
      epi_func_table[EVX$PAL_REI].text	= "REI";
      epi_func_table[EVX$PAL_REMQHIL].decode	= decode_epi;
      epi_func_table[EVX$PAL_REMQHIL].text	= "REMQHIL";
      epi_func_table[EVX$PAL_REMQTIL].decode	= decode_epi;
      epi_func_table[EVX$PAL_REMQTIL].text	= "REMQTIL";
      epi_func_table[EVX$PAL_REMQHIQ].decode	= decode_epi;
      epi_func_table[EVX$PAL_REMQHIQ].text	= "REMQHIQ";
      
      epi_func_table[EVX$PAL_REMQTIQ].decode	= decode_epi;
      epi_func_table[EVX$PAL_REMQTIQ].text	= "REMQTIQ";
      epi_func_table[EVX$PAL_REMQUEL].decode	= decode_epi;
      epi_func_table[EVX$PAL_REMQUEL].text	= "REMQUEL";
      epi_func_table[EVX$PAL_REMQUEQ].decode	= decode_epi;
      epi_func_table[EVX$PAL_REMQUEQ].text	= "REMQUEQ";
      epi_func_table[EVX$PAL_REMQUELD].decode	= decode_epi;
      epi_func_table[EVX$PAL_REMQUELD].text	= "REMQUELD";
      
      epi_func_table[EVX$PAL_REMQUEQD].decode	= decode_epi;
      epi_func_table[EVX$PAL_REMQUEQD].text	= "REMQUEQD";
      epi_func_table[EVX$PAL_STQP].decode	= decode_epi;
      epi_func_table[EVX$PAL_STQP].text	= "STQP";
      epi_func_table[EVX$PAL_SWASTEN].decode	= decode_epi;
      epi_func_table[EVX$PAL_SWASTEN].text	= "SWASTEN";
      epi_func_table[EVX$PAL_SWPCTX].decode	= decode_epi;
      epi_func_table[EVX$PAL_SWPCTX].text	= "SWPCTX";
      
      
      epi_func_table[EVX$PAL_BUGCHK].decode	= decode_epi;
      epi_func_table[EVX$PAL_BUGCHK].text	= "BUGCHK";
      epi_func_table[EVX$PAL_WR_PS_SW].decode	= decode_epi;
      epi_func_table[EVX$PAL_WR_PS_SW].text	= "WR_PS_SW";
      epi_func_table[EVX$PAL_CFLUSH].decode	= decode_epi;
      epi_func_table[EVX$PAL_CFLUSH].text	= "CFLUSH";
      epi_func_table[EVX$PAL_MTPR_ASTEN].decode = decode_epi;
      epi_func_table[EVX$PAL_MTPR_ASTEN].text	= "MTPR_ASTEN";
      epi_func_table[EVX$PAL_MTPR_ASTSR].decode = decode_epi;
      epi_func_table[EVX$PAL_MTPR_ASTSR].text	= "MTPR_ASTSR";
      epi_func_table[EVX$PAL_MTPR_IPIR].decode = decode_epi;
      epi_func_table[EVX$PAL_MTPR_IPIR].text	= "MTPR_IPIR";
      epi_func_table[EVX$PAL_MTPR_TBIAP].decode = decode_epi;
      epi_func_table[EVX$PAL_MTPR_TBIAP].text	= "MTPR_TBIAP";
      epi_func_table[EVX$PAL_RSCC].decode	= decode_epi;
      epi_func_table[EVX$PAL_RSCC].text	= "RSCC";
      
      
      opcode_table[EVX$OPC_LDB].decode	= decode_memory;
      opcode_table[EVX$OPC_LDB].text	= "LDBU";
      opcode_table[EVX$OPC_LDB].action 	= DO_LDB;

      opcode_table[EVX$OPC_LDW].decode	= decode_memory;
      opcode_table[EVX$OPC_LDW].text	= "LDWU";
      opcode_table[EVX$OPC_LDW].action 	= DO_LDW;

      opcode_table[EVX$OPC_LDL].decode	= decode_memory;
      opcode_table[EVX$OPC_LDL].text	= "LDL";
      opcode_table[EVX$OPC_LDL].action 	= DO_LDL;
      
      opcode_table[EVX$OPC_LDQ].decode	= decode_memory;
      opcode_table[EVX$OPC_LDQ].text	= "LDQ";
      opcode_table[EVX$OPC_LDQ].action 	= DO_LDQ;
      
      opcode_table[EVX$OPC_LDLL].decode	= decode_memory;
      opcode_table[EVX$OPC_LDLL].text	= "LDL/L";
      opcode_table[EVX$OPC_LDLL].action 	= DO_LDLL;
      
      opcode_table[EVX$OPC_LDQL].decode	= decode_memory;
      opcode_table[EVX$OPC_LDQL].text	= "LDQ/L";
      opcode_table[EVX$OPC_LDQL].action 	= DO_LDQL;
      
      opcode_table[EVX$OPC_LDQU].decode	= decode_memory;
      opcode_table[EVX$OPC_LDQU].text	= "LDQ/U";
      opcode_table[EVX$OPC_LDQU].action 	= DO_LDQU;
      
      opcode_table[EVX$OPC_LDA].decode	= decode_memory;
      opcode_table[EVX$OPC_LDA].text	= "LDA";
      opcode_table[EVX$OPC_LDA].action 	= DO_LDAQ;
      
      opcode_table[EVX$OPC_LDAH].decode	= decode_memory;
      opcode_table[EVX$OPC_LDAH].text	= "LDAH";
      opcode_table[EVX$OPC_LDAH].action 	= DO_LDAH;
      

      opcode_table[EVX$OPC_STB].decode	= decode_memory;
      opcode_table[EVX$OPC_STB].text	= "STB";
      opcode_table[EVX$OPC_STB].action 	= DO_STB;

      opcode_table[EVX$OPC_STW].decode	= decode_memory;
      opcode_table[EVX$OPC_STW].text	= "STW";
      opcode_table[EVX$OPC_STW].action 	= DO_STW;

      opcode_table[EVX$OPC_STL].decode	= decode_memory;
      opcode_table[EVX$OPC_STL].text	= "STL";
      opcode_table[EVX$OPC_STL].action 	= DO_STL;
      
      opcode_table[EVX$OPC_STQ].decode	= decode_memory;
      opcode_table[EVX$OPC_STQ].text	= "STQ";
      opcode_table[EVX$OPC_STQ].action 	= DO_STQ;
      
      opcode_table[EVX$OPC_STLC].decode	= decode_memory;
      opcode_table[EVX$OPC_STLC].text	= "STL/C";
      opcode_table[EVX$OPC_STLC].action 	= DO_STLC;
      
      opcode_table[EVX$OPC_STQC].decode	= decode_memory;
      opcode_table[EVX$OPC_STQC].text	= "STQ/C";
      opcode_table[EVX$OPC_STQC].action 	= DO_STQC;
      
      opcode_table[EVX$OPC_STQU].decode	= decode_memory;
      opcode_table[EVX$OPC_STQU].text	= "STQ/U";
      opcode_table[EVX$OPC_STQU].action 	= DO_STQU;
      
      opcode_table[EVX$OPC_LDF].decode	= decode_memoryf;
      opcode_table[EVX$OPC_LDF].text	= "LDF";
      opcode_table[EVX$OPC_LDF].action 	= DO_LDF;
      
      opcode_table[EVX$OPC_LDD].decode	= decode_memoryf;
      opcode_table[EVX$OPC_LDD].text	= "LDD";
      opcode_table[EVX$OPC_LDD].action 	= DO_LDD;
      
      opcode_table[EVX$OPC_LDS].decode	= decode_memoryf;
      opcode_table[EVX$OPC_LDS].text	= "LDS";
      opcode_table[EVX$OPC_LDS].action 	= DO_LDS;
      
      opcode_table[EVX$OPC_LDT].decode	= decode_memoryf;
      opcode_table[EVX$OPC_LDT].text	= "LDT";
      opcode_table[EVX$OPC_LDT].action	= DO_LDT;
      
      opcode_table[EVX$OPC_STF].decode	= decode_memoryf;
      opcode_table[EVX$OPC_STF].text	= "STF";
      opcode_table[EVX$OPC_STF].action	= DO_STF;
      
      opcode_table[EVX$OPC_STD].decode	= decode_memoryf;
      opcode_table[EVX$OPC_STD].text	= "STD";
      opcode_table[EVX$OPC_STD].action	= DO_STD;
      
      opcode_table[EVX$OPC_STS].decode	= decode_memoryf;
      opcode_table[EVX$OPC_STS].text	= "STS";
      opcode_table[EVX$OPC_STS].action	= DO_STS;
      
      opcode_table[EVX$OPC_STT].decode	= decode_memoryf;
      opcode_table[EVX$OPC_STT].text	= "STT";
      opcode_table[EVX$OPC_STT].action	= DO_STT;
      
      opcode_table[EVX$OPC_BEQ].decode	= decode_branch;
      opcode_table[EVX$OPC_BEQ].text	= "BEQ";
      opcode_table[EVX$OPC_BEQ].action	= DO_BEQ;
      
      opcode_table[EVX$OPC_BNE].decode	= decode_branch;
      opcode_table[EVX$OPC_BNE].text	= "BNE";
      opcode_table[EVX$OPC_BNE].action	= DO_BNE;
      
      opcode_table[EVX$OPC_BLT].decode	= decode_branch;
      opcode_table[EVX$OPC_BLT].text	= "BLT";
      opcode_table[EVX$OPC_BLT].action	= DO_BLT;
      
      opcode_table[EVX$OPC_BLE].decode	= decode_branch;
      opcode_table[EVX$OPC_BLE].text	= "BLE";
      opcode_table[EVX$OPC_BLE].action	= DO_BLE;
      
      opcode_table[EVX$OPC_BGT].decode	= decode_branch;
      opcode_table[EVX$OPC_BGT].text	= "BGT";
      opcode_table[EVX$OPC_BGT].action	= DO_BGT;
      
      opcode_table[EVX$OPC_BGE].decode	= decode_branch;
      opcode_table[EVX$OPC_BGE].text	= "BGE";
      opcode_table[EVX$OPC_BGE].action	= DO_BGE;
      
      opcode_table[EVX$OPC_BLBC].decode	= decode_branch;
      opcode_table[EVX$OPC_BLBC].text	= "BLBC";
      opcode_table[EVX$OPC_BLBC].action	= DO_BLBC;
      
      opcode_table[EVX$OPC_BLBS].decode	= decode_branch;
      opcode_table[EVX$OPC_BLBS].text	= "BLBS";
      opcode_table[EVX$OPC_BLBS].action	= DO_BLBS;
      
      opcode_table[EVX$OPC_BR].decode	= decode_branch;
      opcode_table[EVX$OPC_BR].text	= "BR";
      opcode_table[EVX$OPC_BR].action	= DO_BR;
      
      opcode_table[EVX$OPC_BSR].decode	= decode_branch;
      opcode_table[EVX$OPC_BSR].text	= "BSR";
      opcode_table[EVX$OPC_BSR].action	= DO_BSR;
      
      opcode_table[EVX$OPC_JSR].decode	= decode_jsrm;
      opcode_table[EVX$OPC_JSR].text	= "JSR";
      opcode_table[EVX$OPC_JSR].action	= DO_JSR;
      
      opcode_table[EVX$OPC_FBEQ].decode	= decode_branchf;
      opcode_table[EVX$OPC_FBEQ].text	= "FBEQ";
      opcode_table[EVX$OPC_FBEQ].action	= DO_BFEQ;
      
      opcode_table[EVX$OPC_FBNE].decode	= decode_branchf;
      opcode_table[EVX$OPC_FBNE].text	= "FBNE";
      opcode_table[EVX$OPC_FBNE].action	= DO_BFNE;
      
      opcode_table[EVX$OPC_FBLT].decode	= decode_branchf;
      opcode_table[EVX$OPC_FBLT].text	= "FBLT";
      opcode_table[EVX$OPC_FBLT].action	= DO_BFLT;
      
      opcode_table[EVX$OPC_FBLE].decode	= decode_branchf;
      opcode_table[EVX$OPC_FBLE].text	= "FBLE";
      opcode_table[EVX$OPC_FBLE].action	= DO_BFLE;
      
      opcode_table[EVX$OPC_FBGT].decode	= decode_branchf;
      opcode_table[EVX$OPC_FBGT].text	= "FBGT";
      opcode_table[EVX$OPC_FBGT].action	= DO_BFGT;
      
      opcode_table[EVX$OPC_FBGE].decode	= decode_branchf;
      opcode_table[EVX$OPC_FBGE].text	= "FBGE";
      opcode_table[EVX$OPC_FBGE].action	= DO_BFGE;
      
      opcode_table[EVX$OPC_ADDI].decode	= decode_by_subfun;
      opcode_table[EVX$OPC_ADDI].next	= addi_func_table;
      opcode_table[EVX$OPC_ADDI].action	= DO_DISP_BY_FUNC;
      
      addi_func_table[EVX$ADDI_ADDL].decode	= decode_operate_3;
      addi_func_table[EVX$ADDI_ADDL].text	= "ADDL";
      addi_func_table[EVX$ADDI_ADDL].action	= DO_ADDL;
      
      addi_func_table[EVX$ADDI_ADDLV].decode	= decode_operate_3;
      addi_func_table[EVX$ADDI_ADDLV].text	= "ADDL/V";
      addi_func_table[EVX$ADDI_ADDLV].action	= DO_ADDLV;
      
      
      addi_func_table[EVX$ADDI_ADDQ].decode	= decode_operate_3;
      addi_func_table[EVX$ADDI_ADDQ].text	= "ADDQ";
      addi_func_table[EVX$ADDI_ADDQ].action	= DO_ADDQ;
      
      addi_func_table[EVX$ADDI_ADDQV].decode	= decode_operate_3;
      addi_func_table[EVX$ADDI_ADDQV].text	= "ADDQ/V";
      addi_func_table[EVX$ADDI_ADDQV].action	= DO_ADDQV;
      
      addi_func_table[EVX$ADDI_S4ADDL].decode	= decode_operate_3;
      addi_func_table[EVX$ADDI_S4ADDL].text	= "S4ADDL";
      addi_func_table[EVX$ADDI_S4ADDL].action	= DO_S4ADDL;
      
      addi_func_table[EVX$ADDI_S4ADDQ].decode	= decode_operate_3;
      addi_func_table[EVX$ADDI_S4ADDQ].text	= "S4ADDQ";
      addi_func_table[EVX$ADDI_S4ADDQ].action	= DO_S4ADDQ;
      
      addi_func_table[EVX$ADDI_S8ADDL].decode	= decode_operate_3;
      addi_func_table[EVX$ADDI_S8ADDL].text	= "S8ADDL";
      addi_func_table[EVX$ADDI_S8ADDL].action	= DO_S8ADDL;
      
      addi_func_table[EVX$ADDI_S8ADDQ].decode	= decode_operate_3;
      addi_func_table[EVX$ADDI_S8ADDQ].text	= "S8ADDQ";
      addi_func_table[EVX$ADDI_S8ADDQ].action	= DO_S8ADDQ;
      
      addi_func_table[EVX$ADDI_S4SUBL].decode	= decode_operate_3;
      addi_func_table[EVX$ADDI_S4SUBL].text	= "S4SUBL";
      addi_func_table[EVX$ADDI_S4SUBL].action	= DO_S4SUBL;
      
      addi_func_table[EVX$ADDI_S4SUBQ].decode	= decode_operate_3;
      addi_func_table[EVX$ADDI_S4SUBQ].text	= "S4SUBQ";
      addi_func_table[EVX$ADDI_S4SUBQ].action	= DO_S4SUBQ;
      
      addi_func_table[EVX$ADDI_S8SUBL].decode	= decode_operate_3;
      addi_func_table[EVX$ADDI_S8SUBL].text	= "S8SUBL";
      addi_func_table[EVX$ADDI_S8SUBL].action	= DO_S8SUBL;
      
      addi_func_table[EVX$ADDI_S8SUBQ].decode	= decode_operate_3;
      addi_func_table[EVX$ADDI_S8SUBQ].text	= "S8SUBQ";
      addi_func_table[EVX$ADDI_S8SUBQ].action	= DO_S8SUBQ;
      
      addi_func_table[EVX$ADDI_CMPEQ].decode	= decode_operate_3;
      addi_func_table[EVX$ADDI_CMPEQ].text	= "CMPEQ";
      addi_func_table[EVX$ADDI_CMPEQ].action	= DO_CMPEQ;
      
      addi_func_table[EVX$ADDI_CMPLT].decode	= decode_operate_3;
      addi_func_table[EVX$ADDI_CMPLT].text	= "CMPLT";
      addi_func_table[EVX$ADDI_CMPLT].action	= DO_CMPLT;
      
      addi_func_table[EVX$ADDI_CMPLE].decode	= decode_operate_3;
      addi_func_table[EVX$ADDI_CMPLE].text	= "CMPLE";
      addi_func_table[EVX$ADDI_CMPLE].action	= DO_CMPLE;
      
      addi_func_table[EVX$ADDI_CMPULT].decode	= decode_operate_3;
      addi_func_table[EVX$ADDI_CMPULT].text	= "CMPULT";
      addi_func_table[EVX$ADDI_CMPULT].action	= DO_CMPULT;
      
      addi_func_table[EVX$ADDI_CMPULE].decode	= decode_operate_3;
      addi_func_table[EVX$ADDI_CMPULE].text	= "CMPULE";
      addi_func_table[EVX$ADDI_CMPULE].action	= DO_CMPULE;
      
      addi_func_table[EVX$ADDI_CMPBGE].decode	= decode_operate_3;
      addi_func_table[EVX$ADDI_CMPBGE].text	= "CMPBGE";
      addi_func_table[EVX$ADDI_CMPBGE].action	= DO_CMPBGE;
      
      addi_func_table[EVX$ADDI_SUBL].decode	= decode_operate_3;
      addi_func_table[EVX$ADDI_SUBL].text	= "SUBL";
      addi_func_table[EVX$ADDI_SUBL].action	= DO_SUBL;
      
      addi_func_table[EVX$ADDI_SUBLV].decode	= decode_operate_3;
      addi_func_table[EVX$ADDI_SUBLV].text	= "SUBL/V";
      addi_func_table[EVX$ADDI_SUBLV].action	= DO_SUBLV;
      
      addi_func_table[EVX$ADDI_SUBQ].decode	= decode_operate_3;
      addi_func_table[EVX$ADDI_SUBQ].text	= "SUBQ";
      addi_func_table[EVX$ADDI_SUBQ].action	= DO_SUBQ;
      
      addi_func_table[EVX$ADDI_SUBQV].decode	= decode_operate_3;
      addi_func_table[EVX$ADDI_SUBQV].text	= "SUBQ/V";
      addi_func_table[EVX$ADDI_SUBQV].action	= DO_SUBQV;
      
      opcode_table[EVX$OPC_MULI].decode	= decode_by_subfun;
      opcode_table[EVX$OPC_MULI].next	= muli_func_table;
      opcode_table[EVX$OPC_MULI].action	= DO_DISP_BY_FUNC;
      
      muli_func_table[EVX$MULI_MULL].decode	= decode_operate_3;
      muli_func_table[EVX$MULI_MULL].text	= "MULL";
      muli_func_table[EVX$MULI_MULL].action	= DO_MULL;
      
      muli_func_table[EVX$MULI_MULLV].decode	= decode_operate_3;
      muli_func_table[EVX$MULI_MULLV].text	= "MULL/V";
      muli_func_table[EVX$MULI_MULLV].action	= DO_MULLV;
      
      muli_func_table[EVX$MULI_MULQ].decode	= decode_operate_3;
      muli_func_table[EVX$MULI_MULQ].text	= "MULQ";
      muli_func_table[EVX$MULI_MULQ].action	= DO_MULQ;
      
      muli_func_table[EVX$MULI_MULQV].decode	= decode_operate_3;
      muli_func_table[EVX$MULI_MULQV].text	= "MULQ/V";
      muli_func_table[EVX$MULI_MULQV].action	= DO_MULQV;
      
      muli_func_table[EVX$MULI_UMULH].decode	= decode_operate_3;
      muli_func_table[EVX$MULI_UMULH].text	= "UMULH";
      muli_func_table[EVX$MULI_UMULH].action	= DO_UMULH;
      
      
      opcode_table[EVX$OPC_LOGI].decode	= decode_by_subfun;
      opcode_table[EVX$OPC_LOGI].next	= logi_func_table;
      opcode_table[EVX$OPC_LOGI].action	= DO_DISP_BY_FUNC;
      
      logi_func_table[EVX$LOGI_AND].decode	= decode_operate_3;
      logi_func_table[EVX$LOGI_AND].text	= "AND";
      logi_func_table[EVX$LOGI_AND].action	= DO_AND;
      
      logi_func_table[EVX$LOGI_BIC].decode	= decode_operate_3;
      logi_func_table[EVX$LOGI_BIC].text	= "BIC";
      logi_func_table[EVX$LOGI_BIC].action	= DO_BIC;
      
      logi_func_table[EVX$LOGI_BIS].decode	= decode_operate_3;
      logi_func_table[EVX$LOGI_BIS].text	= "BIS";
      logi_func_table[EVX$LOGI_BIS].action	= DO_OR;
      
      logi_func_table[EVX$LOGI_ORNOT].decode	= decode_operate_3;
      logi_func_table[EVX$LOGI_ORNOT].text	= "ORNOT";
      logi_func_table[EVX$LOGI_ORNOT].action	= DO_ORNOT;
      
      logi_func_table[EVX$LOGI_XOR].decode	= decode_operate_3;
      logi_func_table[EVX$LOGI_XOR].text	= "XOR";
      logi_func_table[EVX$LOGI_XOR].action	= DO_XOR;
      
      logi_func_table[EVX$LOGI_EQV].decode	= decode_operate_3;
      logi_func_table[EVX$LOGI_EQV].text	= "EQV";
      logi_func_table[EVX$LOGI_EQV].action	= DO_EQV;
      
      
      logi_func_table[EVX$LOGI_CMOVEQ].decode	= decode_operate_3;
      logi_func_table[EVX$LOGI_CMOVEQ].text	= "CMOVEQ";
      logi_func_table[EVX$LOGI_CMOVEQ].action	= DO_CMOVEQ;
      
      logi_func_table[EVX$LOGI_CMOVNE].decode	= decode_operate_3;
      logi_func_table[EVX$LOGI_CMOVNE].text	= "CMOVNE";
      logi_func_table[EVX$LOGI_CMOVNE].action	= DO_CMOVNE;
      
      logi_func_table[EVX$LOGI_CMOVLT].decode	= decode_operate_3;
      logi_func_table[EVX$LOGI_CMOVLT].text	= "CMOVLT";
      logi_func_table[EVX$LOGI_CMOVLT].action	= DO_CMOVLT;
      
      logi_func_table[EVX$LOGI_AMASK].decode	= decode_operate_2; /*V3.24*/
      logi_func_table[EVX$LOGI_AMASK].text	= "AMASK";
      logi_func_table[EVX$LOGI_AMASK].action	= DO_AMASK;

      logi_func_table[EVX$LOGI_CMOVLE].decode	= decode_operate_3;
      logi_func_table[EVX$LOGI_CMOVLE].text	= "CMOVLE";
      logi_func_table[EVX$LOGI_CMOVLE].action	= DO_CMOVLE;
      
      logi_func_table[EVX$LOGI_CMOVGT].decode	= decode_operate_3;
      logi_func_table[EVX$LOGI_CMOVGT].text	= "CMOVGT";
      logi_func_table[EVX$LOGI_CMOVGT].action	= DO_CMOVGT;

      logi_func_table[EVX$LOGI_IMPLVER].decode	= decode_operate_1; /*V3.24*/
      logi_func_table[EVX$LOGI_IMPLVER].text	= "IMPLVER";
      logi_func_table[EVX$LOGI_IMPLVER].action	= DO_IMPLVER;
      
      logi_func_table[EVX$LOGI_CMOVGE].decode	= decode_operate_3;
      logi_func_table[EVX$LOGI_CMOVGE].text	= "CMOVGE";
      logi_func_table[EVX$LOGI_CMOVGE].action	= DO_CMOVGE;
      
      logi_func_table[EVX$LOGI_CMOVLBS].decode = decode_operate_3;
      logi_func_table[EVX$LOGI_CMOVLBS].text	= "CMOVLBS";
      logi_func_table[EVX$LOGI_CMOVLBS].action = DO_CMOVLBS;
      
      logi_func_table[EVX$LOGI_CMOVLBC].decode = decode_operate_3;
      logi_func_table[EVX$LOGI_CMOVLBC].text	= "CMOVLBC";
      logi_func_table[EVX$LOGI_CMOVLBC].action = DO_CMOVLBC;
      
      opcode_table[EVX$OPC_SHFI].decode	= decode_by_subfun;
      opcode_table[EVX$OPC_SHFI].next	= shfi_func_table;
      opcode_table[EVX$OPC_SHFI].action	= DO_DISP_BY_FUNC;
      
      shfi_func_table[EVX$SHFI_SLL].decode	= decode_operate_3;
      shfi_func_table[EVX$SHFI_SLL].text	= "SLL";
      shfi_func_table[EVX$SHFI_SLL].action	= DO_SLL;
      
      shfi_func_table[EVX$SHFI_SRL].decode	= decode_operate_3;
      shfi_func_table[EVX$SHFI_SRL].text	= "SRL";
      shfi_func_table[EVX$SHFI_SRL].action	= DO_SRL;
      
      shfi_func_table[EVX$SHFI_SRA].decode	= decode_operate_3;
      shfi_func_table[EVX$SHFI_SRA].text	= "SRA";
      shfi_func_table[EVX$SHFI_SRA].action	= DO_SRA;
      
      shfi_func_table[EVX$SHFI_EXTQL].decode	= decode_operate_3;
      shfi_func_table[EVX$SHFI_EXTQL].text	= "EXTQL";
      shfi_func_table[EVX$SHFI_EXTQL].action	= DO_EXTQL;
      
      shfi_func_table[EVX$SHFI_EXTLL].decode	= decode_operate_3;
      shfi_func_table[EVX$SHFI_EXTLL].text	= "EXTLL";
      shfi_func_table[EVX$SHFI_EXTLL].action	= DO_EXTLL;
      
      shfi_func_table[EVX$SHFI_EXTWL].decode	= decode_operate_3;
      shfi_func_table[EVX$SHFI_EXTWL].text	= "EXTWL";
      shfi_func_table[EVX$SHFI_EXTWL].action	= DO_EXTWL;
      
      shfi_func_table[EVX$SHFI_EXTBL].decode	= decode_operate_3;
      shfi_func_table[EVX$SHFI_EXTBL].text	= "EXTBL";
      shfi_func_table[EVX$SHFI_EXTBL].action	= DO_EXTBL;
      
      shfi_func_table[EVX$SHFI_EXTQH].decode	= decode_operate_3;
      shfi_func_table[EVX$SHFI_EXTQH].text	= "EXTQH";
      shfi_func_table[EVX$SHFI_EXTQH].action	= DO_EXTQH;
      
      shfi_func_table[EVX$SHFI_EXTLH].decode	= decode_operate_3;
      shfi_func_table[EVX$SHFI_EXTLH].text	= "EXTLH";
      shfi_func_table[EVX$SHFI_EXTLH].action	= DO_EXTLH;
      
      shfi_func_table[EVX$SHFI_EXTWH].decode	= decode_operate_3;
      shfi_func_table[EVX$SHFI_EXTWH].text	= "EXTWH";
      shfi_func_table[EVX$SHFI_EXTWH].action	= DO_EXTWH;
      
      shfi_func_table[EVX$SHFI_INSQL].decode	= decode_operate_3;
      shfi_func_table[EVX$SHFI_INSQL].text	= "INSQL";
      shfi_func_table[EVX$SHFI_INSQL].action	= DO_INSQL;
      
      shfi_func_table[EVX$SHFI_INSLL].decode	= decode_operate_3;
      shfi_func_table[EVX$SHFI_INSLL].text	= "INSLL";
      shfi_func_table[EVX$SHFI_INSLL].action	= DO_INSLL;
      
      shfi_func_table[EVX$SHFI_INSWL].decode	= decode_operate_3;
      shfi_func_table[EVX$SHFI_INSWL].text	= "INSWL";
      shfi_func_table[EVX$SHFI_INSWL].action	= DO_INSWL;
      
      shfi_func_table[EVX$SHFI_INSBL].decode	= decode_operate_3;
      shfi_func_table[EVX$SHFI_INSBL].text	= "INSBL";
      shfi_func_table[EVX$SHFI_INSBL].action	= DO_INSBL;
      
      shfi_func_table[EVX$SHFI_INSQH].decode	= decode_operate_3;
      shfi_func_table[EVX$SHFI_INSQH].text	= "INSQH";
      shfi_func_table[EVX$SHFI_INSQH].action	= DO_INSQH;
      
      shfi_func_table[EVX$SHFI_INSLH].decode	= decode_operate_3;
      shfi_func_table[EVX$SHFI_INSLH].text	= "INSLH";
      shfi_func_table[EVX$SHFI_INSLH].action	= DO_INSLH;
      
      shfi_func_table[EVX$SHFI_INSWH].decode	= decode_operate_3;
      shfi_func_table[EVX$SHFI_INSWH].text	= "INSWH";
      shfi_func_table[EVX$SHFI_INSWH].action	= DO_INSWH;
      
      shfi_func_table[EVX$SHFI_MSKQL].decode	= decode_operate_3;
      shfi_func_table[EVX$SHFI_MSKQL].text	= "MSKQL";
      shfi_func_table[EVX$SHFI_MSKQL].action	= DO_MSKQL;
      
      shfi_func_table[EVX$SHFI_MSKLL].decode	= decode_operate_3;
      shfi_func_table[EVX$SHFI_MSKLL].text	= "MSKLL";
      shfi_func_table[EVX$SHFI_MSKLL].action	= DO_MSKLL;
      
      shfi_func_table[EVX$SHFI_MSKWL].decode	= decode_operate_3;
      shfi_func_table[EVX$SHFI_MSKWL].text	= "MSKWL";
      shfi_func_table[EVX$SHFI_MSKWL].action	= DO_MSKWL;
      
      shfi_func_table[EVX$SHFI_MSKBL].decode	= decode_operate_3;
      shfi_func_table[EVX$SHFI_MSKBL].text	= "MSKBL";
      shfi_func_table[EVX$SHFI_MSKBL].action	= DO_MSKBL;
      
      shfi_func_table[EVX$SHFI_MSKQH].decode	= decode_operate_3;
      shfi_func_table[EVX$SHFI_MSKQH].text	= "MSKQH";
      shfi_func_table[EVX$SHFI_MSKQH].action	= DO_MSKQH;
      
      shfi_func_table[EVX$SHFI_MSKLH].decode	= decode_operate_3;
      shfi_func_table[EVX$SHFI_MSKLH].text	= "MSKLH";
      shfi_func_table[EVX$SHFI_MSKLH].action	= DO_MSKLH;
      
      shfi_func_table[EVX$SHFI_MSKWH].decode	= decode_operate_3;
      shfi_func_table[EVX$SHFI_MSKWH].text	= "MSKWH";
      shfi_func_table[EVX$SHFI_MSKWH].action	= DO_MSKWH;
      
      shfi_func_table[EVX$SHFI_ZAP].decode	= decode_operate_3;
      shfi_func_table[EVX$SHFI_ZAP].text	= "ZAP";
      shfi_func_table[EVX$SHFI_ZAP].action	= DO_ZAP;
      
      shfi_func_table[EVX$SHFI_ZAPNOT].decode	= decode_operate_3;
      shfi_func_table[EVX$SHFI_ZAPNOT].text	= "ZAPNOT";
      shfi_func_table[EVX$SHFI_ZAPNOT].action	= DO_ZAPNOT;
      
      opcode_table[EVX$OPC_FPIEEE].decode	= decode_by_fpu_subfun;
      opcode_table[EVX$OPC_FPIEEE].next	= fpieee_func_table;
      opcode_table[EVX$OPC_FPIEEE].action	= DO_DISP_BY_FPU_FUNC;
      
      fpieee_func_table[EVX$FPIEEE_ADDS].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_ADDS].text		= "ADDS";
      fpieee_func_table[EVX$FPIEEE_ADDS].action	= DO_ADDS;
      
      fpieee_func_table[EVX$FPIEEE_ADDSC].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_ADDSC].text	= "ADDS/C";
      fpieee_func_table[EVX$FPIEEE_ADDSC].action	= DO_ADDSC;
      
      fpieee_func_table[EVX$FPIEEE_ADDSM].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_ADDSM].text	= "ADDS/M";
      fpieee_func_table[EVX$FPIEEE_ADDSM].action	= DO_ADDSM;
      
      fpieee_func_table[EVX$FPIEEE_ADDSP].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_ADDSP].text	= "ADDS/P";
      fpieee_func_table[EVX$FPIEEE_ADDSP].action	= DO_ADDSP;
      
      fpieee_func_table[EVX$FPIEEE_ADDSU].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_ADDSU].text	= "ADDS/U";
      fpieee_func_table[EVX$FPIEEE_ADDSU].action	= DO_ADDSU;
      
      fpieee_func_table[EVX$FPIEEE_ADDSUC].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_ADDSUC].text	= "ADDS/UC";
      fpieee_func_table[EVX$FPIEEE_ADDSUC].action	= DO_ADDSUC;
      
      fpieee_func_table[EVX$FPIEEE_ADDSUM].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_ADDSUM].text	= "ADDS/UM";
      fpieee_func_table[EVX$FPIEEE_ADDSUM].action	= DO_ADDSUM;
      
      fpieee_func_table[EVX$FPIEEE_ADDSUP].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_ADDSUP].text	= "ADDS/UP";
      fpieee_func_table[EVX$FPIEEE_ADDSUP].action	= DO_ADDSUP;
      
      fpieee_func_table[EVX$FPIEEE_ADDSS].decode	= decode_operate_3f;   /*V3.24*/
      fpieee_func_table[EVX$FPIEEE_ADDSS].text	=  "ADDS/S";
      fpieee_func_table[EVX$FPIEEE_ADDSS].action	= DO_ADDSS;

      fpieee_func_table[EVX$FPIEEE_ADDSSC].decode	= decode_operate_3f;   /*V3.24*/
      fpieee_func_table[EVX$FPIEEE_ADDSSC].text	=  "ADDS/SC";
      fpieee_func_table[EVX$FPIEEE_ADDSSC].action	= DO_ADDSSC;


      fpieee_func_table[EVX$FPIEEE_ADDSSU].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_ADDSSU].text	= "ADDS/SU";
      fpieee_func_table[EVX$FPIEEE_ADDSSU].action	= DO_ADDSSU;
      
      fpieee_func_table[EVX$FPIEEE_ADDSSUC].decode 	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_ADDSSUC].text	= "ADDS/SUC";
      fpieee_func_table[EVX$FPIEEE_ADDSSUC].action	= DO_ADDSSUC;
      
      fpieee_func_table[EVX$FPIEEE_ADDSSUM].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_ADDSSUM].text	= "ADDS/SUM";
      fpieee_func_table[EVX$FPIEEE_ADDSSUM].action	= DO_ADDSSUM;
      
      fpieee_func_table[EVX$FPIEEE_ADDSSUP].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_ADDSSUP].text	= "ADDS/SUP";
      fpieee_func_table[EVX$FPIEEE_ADDSSUP].action	= DO_ADDSSUP;
      
      fpieee_func_table[EVX$FPIEEE_ADDSSUI].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_ADDSSUI].text	= "ADDS/SUI";
      fpieee_func_table[EVX$FPIEEE_ADDSSUI].action	= DO_ADDSSUI;
      
      fpieee_func_table[EVX$FPIEEE_ADDSSUIC].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_ADDSSUIC].text	= "ADDS/SUIC";
      fpieee_func_table[EVX$FPIEEE_ADDSSUIC].action	= DO_ADDSSUIC;
      
      fpieee_func_table[EVX$FPIEEE_ADDSSUIM].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_ADDSSUIM].text	= "ADDS/SUIM";
      fpieee_func_table[EVX$FPIEEE_ADDSSUIM].action	= DO_ADDSSUIM;
      
      fpieee_func_table[EVX$FPIEEE_ADDSSUIP].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_ADDSSUIP].text	= "ADDS/SUIP";
      fpieee_func_table[EVX$FPIEEE_ADDSSUIP].action	= DO_ADDSSUIP;
      
      fpieee_func_table[EVX$FPIEEE_ADDT].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_ADDT].text		= "ADDT";
      fpieee_func_table[EVX$FPIEEE_ADDT].action	= DO_ADDT;
      
      fpieee_func_table[EVX$FPIEEE_ADDTC].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_ADDTC].text	= "ADDT/C";
      fpieee_func_table[EVX$FPIEEE_ADDTC].action	= DO_ADDTC;
      
      fpieee_func_table[EVX$FPIEEE_ADDTM].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_ADDTM].text	= "ADDT/M";
      fpieee_func_table[EVX$FPIEEE_ADDTM].action	= DO_ADDTM;
      
      fpieee_func_table[EVX$FPIEEE_ADDTP].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_ADDTP].text	= "ADDT/P";
      fpieee_func_table[EVX$FPIEEE_ADDTP].action	= DO_ADDTP;
      
      fpieee_func_table[EVX$FPIEEE_ADDTS].decode	= decode_operate_3f;   /*V3.24*/
      fpieee_func_table[EVX$FPIEEE_ADDTS].text	=  "ADDT/S";
      fpieee_func_table[EVX$FPIEEE_ADDTS].action	= DO_ADDTS;

      fpieee_func_table[EVX$FPIEEE_ADDTSC].decode	= decode_operate_3f;   /*V3.24*/
      fpieee_func_table[EVX$FPIEEE_ADDTSC].text	=  "ADDT/SC";
      fpieee_func_table[EVX$FPIEEE_ADDTSC].action	= DO_ADDTSC;

      fpieee_func_table[EVX$FPIEEE_ADDTSM].decode	= decode_operate_3f;   /*V3.24*/
      fpieee_func_table[EVX$FPIEEE_ADDTSM].text	=  "ADDT/SM";
      fpieee_func_table[EVX$FPIEEE_ADDTSM].action	= DO_ADDTSM;

      fpieee_func_table[EVX$FPIEEE_ADDTSD].decode	= decode_operate_3f;   /*V3.24*/
      fpieee_func_table[EVX$FPIEEE_ADDTSD].text	=  "ADDT/SD";
      fpieee_func_table[EVX$FPIEEE_ADDTSD].action	= DO_ADDTSD;

      fpieee_func_table[EVX$FPIEEE_ADDTU].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_ADDTU].text	= "ADDT/U";
      fpieee_func_table[EVX$FPIEEE_ADDTU].action	= DO_ADDTU;
      
      fpieee_func_table[EVX$FPIEEE_ADDTUC].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_ADDTUC].text	= "ADDT/UC";
      fpieee_func_table[EVX$FPIEEE_ADDTUC].action	= DO_ADDTUC;
      
      fpieee_func_table[EVX$FPIEEE_ADDTUM].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_ADDTUM].text	= "ADDT/UM";
      fpieee_func_table[EVX$FPIEEE_ADDTUM].action	= DO_ADDTUM;
      
      fpieee_func_table[EVX$FPIEEE_ADDTUP].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_ADDTUP].text	= "ADDT/UP";
      fpieee_func_table[EVX$FPIEEE_ADDTUP].action	= DO_ADDTUP;
      
      
      fpieee_func_table[EVX$FPIEEE_ADDTSU].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_ADDTSU].text	= "ADDT/SU";
      fpieee_func_table[EVX$FPIEEE_ADDTSU].action	= DO_ADDTSU;
      
      fpieee_func_table[EVX$FPIEEE_ADDTSUC].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_ADDTSUC].text	= "ADDT/SUC";
      fpieee_func_table[EVX$FPIEEE_ADDTSUC].action	= DO_ADDTSUC;
      
      fpieee_func_table[EVX$FPIEEE_ADDTSUM].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_ADDTSUM].text	= "ADDT/SUM";
      fpieee_func_table[EVX$FPIEEE_ADDTSUM].action	= DO_ADDTSUM;
      
      fpieee_func_table[EVX$FPIEEE_ADDTSUP].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_ADDTSUP].text	= "ADDT/SUP";
      fpieee_func_table[EVX$FPIEEE_ADDTSUP].action	= DO_ADDTSUP;
      
      fpieee_func_table[EVX$FPIEEE_ADDTSUI].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_ADDTSUI].text	= "ADDT/SUI";
      fpieee_func_table[EVX$FPIEEE_ADDTSUI].action	= DO_ADDTSUI;
      
      fpieee_func_table[EVX$FPIEEE_ADDTSUIC].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_ADDTSUIC].text	= "ADDT/SUIC";
      fpieee_func_table[EVX$FPIEEE_ADDTSUIC].action	= DO_ADDTSUIC;
      
      fpieee_func_table[EVX$FPIEEE_ADDTSUIM].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_ADDTSUIM].text	= "ADDT/SUIM";
      fpieee_func_table[EVX$FPIEEE_ADDTSUIM].action	= DO_ADDTSUIM;
      
      fpieee_func_table[EVX$FPIEEE_ADDTSUIP].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_ADDTSUIP].text	= "ADDT/SUIP";
      fpieee_func_table[EVX$FPIEEE_ADDTSUIP].action	= DO_ADDTSUIP;
      
      fpieee_func_table[EVX$FPIEEE_SUBS].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_SUBS].text		= "SUBS";
      fpieee_func_table[EVX$FPIEEE_SUBS].action	= DO_SUBS;
      
      fpieee_func_table[EVX$FPIEEE_SUBSC].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_SUBSC].text	= "SUBS/C";
      fpieee_func_table[EVX$FPIEEE_SUBSC].action	= DO_SUBSC;
      
      fpieee_func_table[EVX$FPIEEE_SUBSM].decode	= decode_operate_3f;
      
      fpieee_func_table[EVX$FPIEEE_SUBSM].text	= "SUBS/M";
      fpieee_func_table[EVX$FPIEEE_SUBSM].action	= DO_SUBSM;
      
      fpieee_func_table[EVX$FPIEEE_SUBSP].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_SUBSP].text	= "SUBS/P";
      fpieee_func_table[EVX$FPIEEE_SUBSP].action	= DO_SUBSP;
      
      fpieee_func_table[EVX$FPIEEE_SUBSU].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_SUBSU].text	= "SUBS/U";
      fpieee_func_table[EVX$FPIEEE_SUBSU].action	= DO_SUBSU;
      
      fpieee_func_table[EVX$FPIEEE_SUBSUC].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_SUBSUC].text	= "SUBS/UC";
      fpieee_func_table[EVX$FPIEEE_SUBSUC].action	= DO_SUBSUC;
      
      fpieee_func_table[EVX$FPIEEE_SUBSUM].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_SUBSUM].text	= "SUBS/UM";
      fpieee_func_table[EVX$FPIEEE_SUBSUM].action	= DO_SUBSUM;
      
      fpieee_func_table[EVX$FPIEEE_SUBSUP].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_SUBSUP].text	= "SUBS/UP";
      fpieee_func_table[EVX$FPIEEE_SUBSUP].action	= DO_SUBSUP;
      
      fpieee_func_table[EVX$FPIEEE_SUBSSU].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_SUBSSU].text	= "SUBS/SU";
      fpieee_func_table[EVX$FPIEEE_SUBSSU].action	= DO_SUBSSU;
      
      fpieee_func_table[EVX$FPIEEE_SUBSSUC].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_SUBSSUC].text	= "SUBS/SUC";
      fpieee_func_table[EVX$FPIEEE_SUBSSUC].action	= DO_SUBSSUC;
      
      fpieee_func_table[EVX$FPIEEE_SUBSSUM].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_SUBSSUM].text	= "SUBS/SUM";
      fpieee_func_table[EVX$FPIEEE_SUBSSUM].action	= DO_SUBSSUM;
      
      fpieee_func_table[EVX$FPIEEE_SUBSSUP].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_SUBSSUP].text	= "SUBS/SUP";
      fpieee_func_table[EVX$FPIEEE_SUBSSUP].action	= DO_SUBSSUP;
      
      fpieee_func_table[EVX$FPIEEE_SUBSSUI].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_SUBSSUI].text	= "SUBS/SUI";
      fpieee_func_table[EVX$FPIEEE_SUBSSUI].action	= DO_SUBSSUI;
      
      fpieee_func_table[EVX$FPIEEE_SUBSSUIC].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_SUBSSUIC].text	= "SUBS/SUIC";
      fpieee_func_table[EVX$FPIEEE_SUBSSUIC].action	= DO_SUBSSUIC;
      
      fpieee_func_table[EVX$FPIEEE_SUBSSUIM].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_SUBSSUIM].text	= "SUBS/SUIM";
      fpieee_func_table[EVX$FPIEEE_SUBSSUIM].action	= DO_SUBSSUIM;
      
      fpieee_func_table[EVX$FPIEEE_SUBSSUIP].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_SUBSSUIP].text	= "SUBS/SUIP";
      fpieee_func_table[EVX$FPIEEE_SUBSSUIP].action	= DO_SUBSSUIP;
      
      fpieee_func_table[EVX$FPIEEE_SUBT].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_SUBT].text		= "SUBT";
      fpieee_func_table[EVX$FPIEEE_SUBT].action	= DO_SUBT;
      
      fpieee_func_table[EVX$FPIEEE_SUBTC].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_SUBTC].text	= "SUBT/C";
      fpieee_func_table[EVX$FPIEEE_SUBTC].action	= DO_SUBTC;
      
      fpieee_func_table[EVX$FPIEEE_SUBTM].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_SUBTM].text	= "SUBT/M";
      fpieee_func_table[EVX$FPIEEE_SUBTM].action	= DO_SUBTM;
      
      fpieee_func_table[EVX$FPIEEE_SUBTP].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_SUBTP].text	= "SUBT/P";
      fpieee_func_table[EVX$FPIEEE_SUBTP].action	= DO_SUBTP;
      
      fpieee_func_table[EVX$FPIEEE_SUBTU].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_SUBTU].text	= "SUBT/U";
      fpieee_func_table[EVX$FPIEEE_SUBTU].action	= DO_SUBTU;
      
      fpieee_func_table[EVX$FPIEEE_SUBTUC].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_SUBTUC].text	= "SUBT/UC";
      fpieee_func_table[EVX$FPIEEE_SUBTUC].action	= DO_SUBTUC;
      
      fpieee_func_table[EVX$FPIEEE_SUBTUM].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_SUBTUM].text	= "SUBT/UM";
      fpieee_func_table[EVX$FPIEEE_SUBTUM].action	= DO_SUBTUM;
      
      fpieee_func_table[EVX$FPIEEE_SUBTUP].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_SUBTUP].text	= "SUBT/UP";
      fpieee_func_table[EVX$FPIEEE_SUBTUP].action	= DO_SUBTUP;
      
      fpieee_func_table[EVX$FPIEEE_SUBTSU].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_SUBTSU].text	= "SUBT/SU";
      fpieee_func_table[EVX$FPIEEE_SUBTSU].action	= DO_SUBTSU;
      
      fpieee_func_table[EVX$FPIEEE_SUBTSUC].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_SUBTSUC].text	= "SUBT/SUC";
      fpieee_func_table[EVX$FPIEEE_SUBTSUC].action	= DO_SUBTSUC;
      
      fpieee_func_table[EVX$FPIEEE_SUBTSUM].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_SUBTSUM].text	= "SUBT/SUM";
      fpieee_func_table[EVX$FPIEEE_SUBTSUM].action	= DO_SUBTSUM;
      
      fpieee_func_table[EVX$FPIEEE_SUBTSUP].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_SUBTSUP].text	= "SUBT/SUP";
      fpieee_func_table[EVX$FPIEEE_SUBTSUP].action	= DO_SUBTSUP;
      
      fpieee_func_table[EVX$FPIEEE_SUBTSUI].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_SUBTSUI].text	= "SUBT/SUI";
      fpieee_func_table[EVX$FPIEEE_SUBTSUI].action	= DO_SUBTSUI;
      
      fpieee_func_table[EVX$FPIEEE_SUBTSUIC].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_SUBTSUIC].text	= "SUBT/SUIC";
      fpieee_func_table[EVX$FPIEEE_SUBTSUIC].action	= DO_SUBTSUIC;
      
      fpieee_func_table[EVX$FPIEEE_SUBTSUIM].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_SUBTSUIM].text	= "SUBT/SUIM";
      fpieee_func_table[EVX$FPIEEE_SUBTSUIM].action	= DO_SUBTSUIM;
      
      fpieee_func_table[EVX$FPIEEE_SUBTSUIP].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_SUBTSUIP].text	= "SUBT/SUIP";
      fpieee_func_table[EVX$FPIEEE_SUBTSUIP].action	= DO_SUBTSUIP;
      
      fpieee_func_table[EVX$FPIEEE_MULS].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_MULS].text		= "MULS";
      fpieee_func_table[EVX$FPIEEE_MULS].action	= DO_MULS;
      
      fpieee_func_table[EVX$FPIEEE_MULSC].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_MULSC].text	= "MULS/C";
      fpieee_func_table[EVX$FPIEEE_MULSC].action	= DO_MULSC;
      
      fpieee_func_table[EVX$FPIEEE_MULSM].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_MULSM].text	= "MULS/M";
      fpieee_func_table[EVX$FPIEEE_MULSM].action	= DO_MULSM;
      
      fpieee_func_table[EVX$FPIEEE_MULSP].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_MULSP].text	= "MULS/P";
      fpieee_func_table[EVX$FPIEEE_MULSP].action	= DO_MULSP;
      
      fpieee_func_table[EVX$FPIEEE_MULSU].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_MULSU].text	= "MULS/U";
      fpieee_func_table[EVX$FPIEEE_MULSU].action	= DO_MULSU;
      
      fpieee_func_table[EVX$FPIEEE_MULSUC].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_MULSUC].text	= "MULS/UC";
      fpieee_func_table[EVX$FPIEEE_MULSUC].action	= DO_MULSUC;
      
      fpieee_func_table[EVX$FPIEEE_MULSUM].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_MULSUM].text	= "MULS/UM";
      fpieee_func_table[EVX$FPIEEE_MULSUM].action	= DO_MULSUM;
      
      fpieee_func_table[EVX$FPIEEE_MULSUP].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_MULSUP].text	= "MULS/UP";
      fpieee_func_table[EVX$FPIEEE_MULSUP].action	= DO_MULSUP;
      
      fpieee_func_table[EVX$FPIEEE_MULSSU].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_MULSSU].text	= "MULS/SU";
      fpieee_func_table[EVX$FPIEEE_MULSSU].action	= DO_MULSSU;
      
      fpieee_func_table[EVX$FPIEEE_MULSSUC].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_MULSSUC].text	= "MULS/SUC";
      fpieee_func_table[EVX$FPIEEE_MULSSUC].action	= DO_MULSSUC;
      
      fpieee_func_table[EVX$FPIEEE_MULSSUM].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_MULSSUM].text	= "MULS/SUM";
      fpieee_func_table[EVX$FPIEEE_MULSSUM].action	= DO_MULSSUM;
      
      fpieee_func_table[EVX$FPIEEE_MULSSUP].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_MULSSUP].text	= "MULS/SUP";
      fpieee_func_table[EVX$FPIEEE_MULSSUP].action	= DO_MULSSUP;
      
      fpieee_func_table[EVX$FPIEEE_MULSSUI].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_MULSSUI].text	= "MULS/SUI";
      fpieee_func_table[EVX$FPIEEE_MULSSUI].action	= DO_MULSSUI;
      
      fpieee_func_table[EVX$FPIEEE_MULSSUIC].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_MULSSUIC].text	= "MULS/SUIC";
      fpieee_func_table[EVX$FPIEEE_MULSSUIC].action	= DO_MULSSUIC;
      
      fpieee_func_table[EVX$FPIEEE_MULSSUIM].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_MULSSUIM].text	= "MULS/SUIM";
      fpieee_func_table[EVX$FPIEEE_MULSSUIM].action	= DO_MULSSUIM;
      
      fpieee_func_table[EVX$FPIEEE_MULSSUIP].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_MULSSUIP].text	= "MULS/SUIP";
      fpieee_func_table[EVX$FPIEEE_MULSSUIP].action	= DO_MULSSUIP;
      
      fpieee_func_table[EVX$FPIEEE_MULT].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_MULT].text		= "MULT";
      fpieee_func_table[EVX$FPIEEE_MULT].action	= DO_MULT;
      
      fpieee_func_table[EVX$FPIEEE_MULTC].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_MULTC].text	= "MULT/C";
      fpieee_func_table[EVX$FPIEEE_MULTC].action	= DO_MULTC;
      
      fpieee_func_table[EVX$FPIEEE_MULTM].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_MULTM].text	= "MULT/M";
      fpieee_func_table[EVX$FPIEEE_MULTM].action	= DO_MULTM;
      
      fpieee_func_table[EVX$FPIEEE_MULTP].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_MULTP].text	= "MULT/P";
      fpieee_func_table[EVX$FPIEEE_MULTP].action	= DO_MULTP;
      
      fpieee_func_table[EVX$FPIEEE_MULTU].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_MULTU].text	= "MULT/U";
      fpieee_func_table[EVX$FPIEEE_MULTU].action	= DO_MULTU;
      
      fpieee_func_table[EVX$FPIEEE_MULTUC].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_MULTUC].text	= "MULT/UC";
      fpieee_func_table[EVX$FPIEEE_MULTUC].action	= DO_MULTUC;
      
      fpieee_func_table[EVX$FPIEEE_MULTUM].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_MULTUM].text	= "MULT/UM";
      fpieee_func_table[EVX$FPIEEE_MULTUM].action	= DO_MULTUM;
      
      fpieee_func_table[EVX$FPIEEE_MULTUP].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_MULTUP].text	= "MULT/UP";
      fpieee_func_table[EVX$FPIEEE_MULTUP].action	= DO_MULTUP;
      
      fpieee_func_table[EVX$FPIEEE_MULTSU].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_MULTSU].text	= "MULT/SU";
      fpieee_func_table[EVX$FPIEEE_MULTSU].action	= DO_MULTSU;
      
      fpieee_func_table[EVX$FPIEEE_MULTSUC].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_MULTSUC].text	= "MULT/SUC";
      fpieee_func_table[EVX$FPIEEE_MULTSUC].action	= DO_MULTSUC;
      
      fpieee_func_table[EVX$FPIEEE_MULTSUM].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_MULTSUM].text	= "MULT/SUM";
      fpieee_func_table[EVX$FPIEEE_MULTSUM].action	= DO_MULTSUM;
      
      fpieee_func_table[EVX$FPIEEE_MULTSUP].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_MULTSUP].text	= "MULT/SUP";
      fpieee_func_table[EVX$FPIEEE_MULTSUP].action	= DO_MULTSUP;
      
      fpieee_func_table[EVX$FPIEEE_MULTSUI].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_MULTSUI].text	= "MULT/SUI";
      fpieee_func_table[EVX$FPIEEE_MULTSUI].action	= DO_MULTSUI;
      
      fpieee_func_table[EVX$FPIEEE_MULTSUIC].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_MULTSUIC].text	= "MULT/SUIC";
      fpieee_func_table[EVX$FPIEEE_MULTSUIC].action	= DO_MULTSUIC;
      
      fpieee_func_table[EVX$FPIEEE_MULTSUIM].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_MULTSUIM].text	= "MULT/SUIM";
      fpieee_func_table[EVX$FPIEEE_MULTSUIM].action	= DO_MULTSUIM;
      
      fpieee_func_table[EVX$FPIEEE_MULTSUIP].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_MULTSUIP].text	= "MULT/SUIP";
      fpieee_func_table[EVX$FPIEEE_MULTSUIP].action	= DO_MULTSUIP;
      
      fpieee_func_table[EVX$FPIEEE_DIVS].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_DIVS].text		= "DIVS";
      fpieee_func_table[EVX$FPIEEE_DIVS].action	= DO_DIVS;
      
      fpieee_func_table[EVX$FPIEEE_DIVSC].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_DIVSC].text	= "DIVS/C";
      fpieee_func_table[EVX$FPIEEE_DIVSC].action	= DO_DIVSC;
      
      fpieee_func_table[EVX$FPIEEE_DIVSM].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_DIVSM].text	= "DIVS/M";
      fpieee_func_table[EVX$FPIEEE_DIVSM].action	= DO_DIVSM;
      
      fpieee_func_table[EVX$FPIEEE_DIVSP].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_DIVSP].text	= "DIVS/P";
      fpieee_func_table[EVX$FPIEEE_DIVSP].action	= DO_DIVSP;
      
      fpieee_func_table[EVX$FPIEEE_DIVSU].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_DIVSU].text	= "DIVS/U";
      fpieee_func_table[EVX$FPIEEE_DIVSU].action	= DO_DIVSU;
      
      fpieee_func_table[EVX$FPIEEE_DIVSUC].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_DIVSUC].text	= "DIVS/UC";
      fpieee_func_table[EVX$FPIEEE_DIVSUC].action	= DO_DIVSUC;
      
      fpieee_func_table[EVX$FPIEEE_DIVSUM].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_DIVSUM].text	= "DIVS/UM";
      fpieee_func_table[EVX$FPIEEE_DIVSUM].action	= DO_DIVSUM;
      
      fpieee_func_table[EVX$FPIEEE_DIVSUP].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_DIVSUP].text	= "DIVS/UP";
      fpieee_func_table[EVX$FPIEEE_DIVSUP].action	= DO_DIVSUP;
      
      fpieee_func_table[EVX$FPIEEE_DIVSSU].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_DIVSSU].text	= "DIVS/SU";
      fpieee_func_table[EVX$FPIEEE_DIVSSU].action	= DO_DIVSSU;
      
      fpieee_func_table[EVX$FPIEEE_DIVSSUC].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_DIVSSUC].text	= "DIVS/SUC";
      fpieee_func_table[EVX$FPIEEE_DIVSSUC].action	= DO_DIVSSUC;
      
      fpieee_func_table[EVX$FPIEEE_DIVSSUM].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_DIVSSUM].text	= "DIVS/SUM";
      fpieee_func_table[EVX$FPIEEE_DIVSSUM].action	= DO_DIVSSUM;
      
      fpieee_func_table[EVX$FPIEEE_DIVSSUP].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_DIVSSUP].text	= "DIVS/SUP";
      fpieee_func_table[EVX$FPIEEE_DIVSSUP].action	= DO_DIVSSUP;
      
      fpieee_func_table[EVX$FPIEEE_DIVSSUI].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_DIVSSUI].text	= "DIVS/SUI";
      fpieee_func_table[EVX$FPIEEE_DIVSSUI].action	= DO_DIVSSUI;
      
      fpieee_func_table[EVX$FPIEEE_DIVSSUIC].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_DIVSSUIC].text	= "DIVS/SUIC";
      fpieee_func_table[EVX$FPIEEE_DIVSSUIC].action	= DO_DIVSSUIC;
      
      fpieee_func_table[EVX$FPIEEE_DIVSSUIM].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_DIVSSUIM].text	= "DIVS/SUIM";
      fpieee_func_table[EVX$FPIEEE_DIVSSUIM].action	= DO_DIVSSUIM;
      
      fpieee_func_table[EVX$FPIEEE_DIVSSUIP].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_DIVSSUIP].text	= "DIVS/SUIP";
      fpieee_func_table[EVX$FPIEEE_DIVSSUIP].action	= DO_DIVSSUIP;
      
      fpieee_func_table[EVX$FPIEEE_DIVT].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_DIVT].text		= "DIVT";
      fpieee_func_table[EVX$FPIEEE_DIVT].action	= DO_DIVT;
      
      fpieee_func_table[EVX$FPIEEE_DIVTC].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_DIVTC].text	= "DIVT/C";
      fpieee_func_table[EVX$FPIEEE_DIVTC].action	= DO_DIVTC;
      
      fpieee_func_table[EVX$FPIEEE_DIVTM].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_DIVTM].text	= "DIVT/M";
      fpieee_func_table[EVX$FPIEEE_DIVTM].action	= DO_DIVTM;
      
      fpieee_func_table[EVX$FPIEEE_DIVTP].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_DIVTP].text	= "DIVT/P";
      fpieee_func_table[EVX$FPIEEE_DIVTP].action	= DO_DIVTP;
      
      fpieee_func_table[EVX$FPIEEE_DIVTU].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_DIVTU].text	= "DIVT/U";
      fpieee_func_table[EVX$FPIEEE_DIVTU].action	= DO_DIVTU;
      
      fpieee_func_table[EVX$FPIEEE_DIVTUC].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_DIVTUC].text	= "DIVT/UC";
      fpieee_func_table[EVX$FPIEEE_DIVTUC].action	= DO_DIVTUC;
      
      fpieee_func_table[EVX$FPIEEE_DIVTUM].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_DIVTUM].text	= "DIVT/UM";
      fpieee_func_table[EVX$FPIEEE_DIVTUM].action	= DO_DIVTUM;
      
      fpieee_func_table[EVX$FPIEEE_DIVTUP].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_DIVTUP].text	= "DIVT/UP";
      fpieee_func_table[EVX$FPIEEE_DIVTUP].action	= DO_DIVTUP;
      
      fpieee_func_table[EVX$FPIEEE_DIVTSU].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_DIVTSU].text	= "DIVT/SU";
      fpieee_func_table[EVX$FPIEEE_DIVTSU].action	= DO_DIVTSU;
      
      fpieee_func_table[EVX$FPIEEE_DIVTSUC].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_DIVTSUC].text	= "DIVT/SUC";
      fpieee_func_table[EVX$FPIEEE_DIVTSUC].action	= DO_DIVTSUC;
      
      fpieee_func_table[EVX$FPIEEE_DIVTSUM].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_DIVTSUM].text	= "DIVT/SUM";
      fpieee_func_table[EVX$FPIEEE_DIVTSUM].action	= DO_DIVTSUM;
      
      fpieee_func_table[EVX$FPIEEE_DIVTSUP].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_DIVTSUP].text	= "DIVT/SUP";
      fpieee_func_table[EVX$FPIEEE_DIVTSUP].action	= DO_DIVTSUP;
      
      fpieee_func_table[EVX$FPIEEE_DIVTSUI].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_DIVTSUI].text	= "DIVT/SUI";
      fpieee_func_table[EVX$FPIEEE_DIVTSUI].action	= DO_DIVTSUI;
      
      fpieee_func_table[EVX$FPIEEE_DIVTSUIC].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_DIVTSUIC].text	= "DIVT/SUIC";
      fpieee_func_table[EVX$FPIEEE_DIVTSUIC].action	= DO_DIVTSUIC;
      
      fpieee_func_table[EVX$FPIEEE_DIVTSUIM].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_DIVTSUIM].text	= "DIVT/SUIM";
      fpieee_func_table[EVX$FPIEEE_DIVTSUIM].action	= DO_DIVTSUIM;
      
      fpieee_func_table[EVX$FPIEEE_DIVTSUIP].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_DIVTSUIP].text	= "DIVT/SUIP";
      fpieee_func_table[EVX$FPIEEE_DIVTSUIP].action	= DO_DIVTSUIP;
      
      fpieee_func_table[EVX$FPIEEE_CMPTEQ].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_CMPTEQ].text	= "CMPTEQ";
      fpieee_func_table[EVX$FPIEEE_CMPTEQ].action	= DO_CMPTEQ;
      
      fpieee_func_table[EVX$FPIEEE_CMPTLT].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_CMPTLT].text	= "CMPTLT";
      fpieee_func_table[EVX$FPIEEE_CMPTLT].action	= DO_CMPTLT;
      
      fpieee_func_table[EVX$FPIEEE_CMPTLE].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_CMPTLE].text	= "CMPTLE";
      fpieee_func_table[EVX$FPIEEE_CMPTLE].action	= DO_CMPTLE;
      
      fpieee_func_table[EVX$FPIEEE_CMPTUN].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_CMPTUN].text	= "CMPTUN";
      fpieee_func_table[EVX$FPIEEE_CMPTUN].action	= DO_CMPTUN;
      
      fpieee_func_table[EVX$FPIEEE_CMPTEQSU].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_CMPTEQSU].text	= "CMPTEQ/SU";
      fpieee_func_table[EVX$FPIEEE_CMPTEQSU].action	= DO_CMPTEQSU;
      
      fpieee_func_table[EVX$FPIEEE_CMPTLTSU].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_CMPTLTSU].text	= "CMPTLT/SU";
      fpieee_func_table[EVX$FPIEEE_CMPTLTSU].action	= DO_CMPTLTSU;
      
      fpieee_func_table[EVX$FPIEEE_CMPTLESU].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_CMPTLESU].text	= "CMPTLE/SU";
      fpieee_func_table[EVX$FPIEEE_CMPTLESU].action	= DO_CMPTLESU;
      
      fpieee_func_table[EVX$FPIEEE_CMPTUNSU].decode	= decode_operate_3f;
      fpieee_func_table[EVX$FPIEEE_CMPTUNSU].text	= "CMPTUN/SU";
      fpieee_func_table[EVX$FPIEEE_CMPTUNSU].action	= DO_CMPTUNSU;
      
      fpdec_func_table[EVX$FPDEC_CVTQF].decode        = decode_operate_2f;
      fpdec_func_table[EVX$FPDEC_CVTQF].text          = "CVTQF";
      fpdec_func_table[EVX$FPDEC_CVTQF].action        = DO_CVTQF;
      
      fpdec_func_table[EVX$FPDEC_CVTQFC].decode       = decode_operate_2f;
      fpdec_func_table[EVX$FPDEC_CVTQFC].text         = "CVTQF/C";
      fpdec_func_table[EVX$FPDEC_CVTQFC].action       = DO_CVTQFC;
      
      fpdec_func_table[EVX$FPDEC_CVTQFS].decode       = decode_operate_2f;
      fpdec_func_table[EVX$FPDEC_CVTQFS].text         = "CVTQF/S";
      fpdec_func_table[EVX$FPDEC_CVTQFS].action       = DO_CVTQFS;
      
      fpdec_func_table[EVX$FPDEC_CVTQFSC].decode      = decode_operate_2f;
      fpdec_func_table[EVX$FPDEC_CVTQFSC].text        = "CVTQF/SC";
      fpdec_func_table[EVX$FPDEC_CVTQFSC].action      = DO_CVTQFSC;
      
      fpieee_func_table[EVX$FPIEEE_CVTQS].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTQS].text	= "CVTQS";
      fpieee_func_table[EVX$FPIEEE_CVTQS].action	= DO_CVTQS;
      
      fpieee_func_table[EVX$FPIEEE_CVTQSC].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTQSC].text	= "CVTQS/C";
      fpieee_func_table[EVX$FPIEEE_CVTQSC].action	= DO_CVTQSC;
      
      fpieee_func_table[EVX$FPIEEE_CVTQSM].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTQSM].text	= "CVTQS/M";
      fpieee_func_table[EVX$FPIEEE_CVTQSM].action	= DO_CVTQSM;
      
      fpieee_func_table[EVX$FPIEEE_CVTQSP].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTQSP].text	= "CVTQS/P";
      fpieee_func_table[EVX$FPIEEE_CVTQSP].action	= DO_CVTQSP;
      
      fpieee_func_table[EVX$FPIEEE_CVTQSSUI].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTQSSUI].text	= "CVTQS/SUI";
      fpieee_func_table[EVX$FPIEEE_CVTQSSUI].action	= DO_CVTQSSUI;
      
      fpieee_func_table[EVX$FPIEEE_CVTQSSUIC].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTQSSUIC].text	= "CVTQS/SUIC";
      fpieee_func_table[EVX$FPIEEE_CVTQSSUIC].action	= DO_CVTQSSUIC;
      
      fpieee_func_table[EVX$FPIEEE_CVTQSSUIM].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTQSSUIM].text	= "CVTQS/SUIM";
      fpieee_func_table[EVX$FPIEEE_CVTQSSUIM].action	= DO_CVTQSSUIM;
      
      fpieee_func_table[EVX$FPIEEE_CVTQSSUIP].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTQSSUIP].text	= "CVTQS/SUIP";
      fpieee_func_table[EVX$FPIEEE_CVTQSSUIP].action	= DO_CVTQSSUIP;
      
      fpieee_func_table[EVX$FPIEEE_CVTQT].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTQT].text	= "CVTQT";
      fpieee_func_table[EVX$FPIEEE_CVTQT].action	= DO_CVTQT;
      
      fpieee_func_table[EVX$FPIEEE_CVTQTC].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTQTC].text	= "CVTQT/C";
      fpieee_func_table[EVX$FPIEEE_CVTQTC].action	= DO_CVTQTC;
      
      fpieee_func_table[EVX$FPIEEE_CVTQTM].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTQTM].text	= "CVTQT/M";
      fpieee_func_table[EVX$FPIEEE_CVTQTM].action	= DO_CVTQTM;
      
      fpieee_func_table[EVX$FPIEEE_CVTQTP].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTQTP].text	= "CVTQT/P";
      fpieee_func_table[EVX$FPIEEE_CVTQTP].action	= DO_CVTQTP;
      
      fpieee_func_table[EVX$FPIEEE_CVTQTSUI].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTQTSUI].text	= "CVTQT/SUI";
      fpieee_func_table[EVX$FPIEEE_CVTQTSUI].action	= DO_CVTQTSUI;
      
      fpieee_func_table[EVX$FPIEEE_CVTQTSUIC].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTQTSUIC].text	= "CVTQT/SUIC";
      fpieee_func_table[EVX$FPIEEE_CVTQTSUIC].action	= DO_CVTQTSUIC;
      
      fpieee_func_table[EVX$FPIEEE_CVTQTSUIM].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTQTSUIM].text	= "CVTQT/SUIM";
      fpieee_func_table[EVX$FPIEEE_CVTQTSUIM].action	= DO_CVTQTSUIM;
      
      fpieee_func_table[EVX$FPIEEE_CVTQTSUIP].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTQTSUIP].text	= "CVTQT/SUIP";
      fpieee_func_table[EVX$FPIEEE_CVTQTSUIP].action	= DO_CVTQTSUIP;
      
      fpieee_func_table[EVX$FPIEEE_CVTST].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTST].text	= "CVTST";
      fpieee_func_table[EVX$FPIEEE_CVTST].action	= DO_CVTST;
      fpieee_func_table[EVX$FPIEEE_CVTSTS].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTSTS].text	= "CVTSTS";
      fpieee_func_table[EVX$FPIEEE_CVTSTS].action	= DO_CVTSTS;
      
      fpieee_func_table[EVX$FPIEEE_CVTTS].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTTS].text	= "CVTTS";
      fpieee_func_table[EVX$FPIEEE_CVTTS].action	= DO_CVTTS;
      
      fpieee_func_table[EVX$FPIEEE_CVTTSC].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTTSC].text	= "CVTTS/C";
      fpieee_func_table[EVX$FPIEEE_CVTTSC].action	= DO_CVTTSC;
      
      fpieee_func_table[EVX$FPIEEE_CVTTSM].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTTSM].text	= "CVTTS/M";
      fpieee_func_table[EVX$FPIEEE_CVTTSM].action	= DO_CVTTSM;
      
      fpieee_func_table[EVX$FPIEEE_CVTTSP].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTTSP].text	= "CVTTS/P";
      fpieee_func_table[EVX$FPIEEE_CVTTSP].action	= DO_CVTTSP;
      
      fpieee_func_table[EVX$FPIEEE_CVTTSU].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTTSU].text	= "CVTTS/U";
      fpieee_func_table[EVX$FPIEEE_CVTTSU].action	= DO_CVTTSU;
      
      fpieee_func_table[EVX$FPIEEE_CVTTSUC].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTTSUC].text	= "CVTTS/UC";
      fpieee_func_table[EVX$FPIEEE_CVTTSUC].action	= DO_CVTTSUC;
      
      fpieee_func_table[EVX$FPIEEE_CVTTSUM].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTTSUM].text	= "CVTTS/UM";
      fpieee_func_table[EVX$FPIEEE_CVTTSUM].action	= DO_CVTTSUM;
      
      fpieee_func_table[EVX$FPIEEE_CVTTSUP].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTTSUP].text	= "CVTTS/UP";
      fpieee_func_table[EVX$FPIEEE_CVTTSUP].action	= DO_CVTTSUP;
      
      fpieee_func_table[EVX$FPIEEE_CVTTSSU].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTTSSU].text	= "CVTTS/SU";
      fpieee_func_table[EVX$FPIEEE_CVTTSSU].action	= DO_CVTTSSU;
      
      fpieee_func_table[EVX$FPIEEE_CVTTSSUC].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTTSSUC].text	= "CVTTS/SUC";
      fpieee_func_table[EVX$FPIEEE_CVTTSSUC].action	= DO_CVTTSSUC;
      
      fpieee_func_table[EVX$FPIEEE_CVTTSSUM].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTTSSUM].text	= "CVTTS/SUM";
      fpieee_func_table[EVX$FPIEEE_CVTTSSUM].action	= DO_CVTTSSUM;
      
      fpieee_func_table[EVX$FPIEEE_CVTTSSUP].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTTSSUP].text	= "CVTTS/SUP";
      fpieee_func_table[EVX$FPIEEE_CVTTSSUP].action	= DO_CVTTSSUP;
      
      fpieee_func_table[EVX$FPIEEE_CVTTSSUI].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTTSSUI].text	= "CVTTS/SUI";
      fpieee_func_table[EVX$FPIEEE_CVTTSSUI].action	= DO_CVTTSSUI;
      
      fpieee_func_table[EVX$FPIEEE_CVTTSSUIC].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTTSSUIC].text	= "CVTTS/SUIC";
      fpieee_func_table[EVX$FPIEEE_CVTTSSUIC].action	= DO_CVTTSSUIC;
      
      fpieee_func_table[EVX$FPIEEE_CVTTSSUIM].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTTSSUIM].text	= "CVTTS/SUIM";
      fpieee_func_table[EVX$FPIEEE_CVTTSSUIM].action	= DO_CVTTSSUIM;
      
      fpieee_func_table[EVX$FPIEEE_CVTTSSUIP].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTTSSUIP].text	= "CVTTS/SUIP";
      fpieee_func_table[EVX$FPIEEE_CVTTSSUIP].action	= DO_CVTTSSUIP;
      
      fpieee_func_table[EVX$FPIEEE_CVTTQ].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTTQ].text	= "CVTTQ";
      fpieee_func_table[EVX$FPIEEE_CVTTQ].action	= DO_CVTTQ;
      
      fpieee_func_table[EVX$FPIEEE_CVTTQC].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTTQC].text	= "CVTTQ/C";
      fpieee_func_table[EVX$FPIEEE_CVTTQC].action	= DO_CVTTQC;
      
      fpieee_func_table[EVX$FPIEEE_CVTTQV].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTTQV].text	= "CVTTQ/V";
      fpieee_func_table[EVX$FPIEEE_CVTTQV].action	= DO_CVTTQV;
      
      fpieee_func_table[EVX$FPIEEE_CVTTQVC].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTTQVC].text	= "CVTTQ/VC";
      fpieee_func_table[EVX$FPIEEE_CVTTQVC].action	= DO_CVTTQVC;
      
      fpieee_func_table[EVX$FPIEEE_CVTTQSV].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTTQSV].text	= "CVTTQ/SV";
      fpieee_func_table[EVX$FPIEEE_CVTTQSV].action	= DO_CVTTQSV;
      
      fpieee_func_table[EVX$FPIEEE_CVTTQSVC].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTTQSVC].text	= "CVTTQ/SVC";
      fpieee_func_table[EVX$FPIEEE_CVTTQSVC].action	= DO_CVTTQSVC;
      
      fpieee_func_table[EVX$FPIEEE_CVTTQSVI].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTTQSVI].text	= "CVTTQ/SVI";
      fpieee_func_table[EVX$FPIEEE_CVTTQSVI].action	= DO_CVTTQSVI;
      
      fpieee_func_table[EVX$FPIEEE_CVTTQSVIC].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTTQSVIC].text	= "CVTTQ/SVIC";
      fpieee_func_table[EVX$FPIEEE_CVTTQSVIC].action	= DO_CVTTQSVIC;
      
      fpieee_func_table[EVX$FPIEEE_CVTTQD].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTTQD].text	= "CVTTQ/D";
      fpieee_func_table[EVX$FPIEEE_CVTTQD].action	= DO_CVTTQD;
      
      fpieee_func_table[EVX$FPIEEE_CVTTQVD].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTTQVD].text	= "CVTTQ/VD";
      fpieee_func_table[EVX$FPIEEE_CVTTQVD].action	= DO_CVTTQVD;
      
      fpieee_func_table[EVX$FPIEEE_CVTTQSVD].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTTQSVD].text	= "CVTTQ/SVD";
      fpieee_func_table[EVX$FPIEEE_CVTTQSVD].action	= DO_CVTTQSVD;
      
      fpieee_func_table[EVX$FPIEEE_CVTTQSVID].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTTQSVID].text	= "CVTTQ/SVIM";
      fpieee_func_table[EVX$FPIEEE_CVTTQSVID].action	= DO_CVTTQSVIM;
      
      fpieee_func_table[EVX$FPIEEE_CVTTQM].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTTQM].text	= "CVTTQ/M";
      fpieee_func_table[EVX$FPIEEE_CVTTQM].action	= DO_CVTTQM;
      
      fpieee_func_table[EVX$FPIEEE_CVTTQVM].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTTQVM].text	= "CVTTQ/VM";
      fpieee_func_table[EVX$FPIEEE_CVTTQVM].action	= DO_CVTTQVM;
      
      fpieee_func_table[EVX$FPIEEE_CVTTQSVM].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTTQSVM].text	= "CVTTQ/SVM";
      fpieee_func_table[EVX$FPIEEE_CVTTQSVM].action	= DO_CVTTQSVM;
      
      fpieee_func_table[EVX$FPIEEE_CVTTQSVIM].decode	= decode_operate_2f;
      fpieee_func_table[EVX$FPIEEE_CVTTQSVIM].text	= "CVTTQ/SVIM";
      fpieee_func_table[EVX$FPIEEE_CVTTQSVIM].action	= DO_CVTTQSVIM;
      
      opcode_table[EVX$OPC_FPDEC].decode	= decode_by_fpu_subfun;
      opcode_table[EVX$OPC_FPDEC].next	= fpdec_func_table;
      opcode_table[EVX$OPC_FPDEC].action	= DO_DISP_BY_FPU_FUNC;
      
      fpdec_func_table[EVX$FPDEC_ADDF].decode		= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_ADDF].text		= "ADDF";
      fpdec_func_table[EVX$FPDEC_ADDF].action		= DO_ADDF;
      
      fpdec_func_table[EVX$FPDEC_ADDFC].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_ADDFC].text		= "ADDF/C";
      fpdec_func_table[EVX$FPDEC_ADDFC].action	= DO_ADDFC;
      
      fpdec_func_table[EVX$FPDEC_ADDFU].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_ADDFU].text		= "ADDF/U";
      fpdec_func_table[EVX$FPDEC_ADDFU].action	= DO_ADDFU;
      
      fpdec_func_table[EVX$FPDEC_ADDFUC].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_ADDFUC].text		= "ADDF/UC";
      fpdec_func_table[EVX$FPDEC_ADDFUC].action	= DO_ADDFUC;
      
      fpdec_func_table[EVX$FPDEC_ADDFS].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_ADDFS].text		= "ADDF/S";
      fpdec_func_table[EVX$FPDEC_ADDFS].action	= DO_ADDFS;
      
      fpdec_func_table[EVX$FPDEC_ADDFSC].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_ADDFSC].text		= "ADDF/SC";
      fpdec_func_table[EVX$FPDEC_ADDFSC].action	= DO_ADDFSC;
      
      fpdec_func_table[EVX$FPDEC_ADDFSU].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_ADDFSU].text		= "ADDF/SU";
      fpdec_func_table[EVX$FPDEC_ADDFSU].action	= DO_ADDFSU;
      
      fpdec_func_table[EVX$FPDEC_ADDFSUC].decode 	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_ADDFSUC].text	= "ADDF/SUC";
      fpdec_func_table[EVX$FPDEC_ADDFSUC].action	= DO_ADDFSUC;
      
      fpdec_func_table[EVX$FPDEC_ADDG].decode		= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_ADDG].text		= "ADDG";
      fpdec_func_table[EVX$FPDEC_ADDG].action		= DO_ADDG;
      
      fpdec_func_table[EVX$FPDEC_ADDGC].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_ADDGC].text		= "ADDG/C";
      fpdec_func_table[EVX$FPDEC_ADDGC].action	= DO_ADDGC;
      
      fpdec_func_table[EVX$FPDEC_ADDGU].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_ADDGU].text		= "ADDG/U";
      fpdec_func_table[EVX$FPDEC_ADDGU].action	= DO_ADDGU;
      
      fpdec_func_table[EVX$FPDEC_ADDGUC].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_ADDGUC].text		= "ADDG/UC";
      fpdec_func_table[EVX$FPDEC_ADDGUC].action	= DO_ADDGUC;
      
      fpdec_func_table[EVX$FPDEC_ADDGS].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_ADDGS].text		= "ADDG/S";
      fpdec_func_table[EVX$FPDEC_ADDGS].action	= DO_ADDGS;
      
      fpdec_func_table[EVX$FPDEC_ADDGSC].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_ADDGSC].text		= "ADDG/SC";
      fpdec_func_table[EVX$FPDEC_ADDGSC].action	= DO_ADDGSC;
      
      fpdec_func_table[EVX$FPDEC_ADDGSU].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_ADDGSU].text		= "ADDG/SU";
      fpdec_func_table[EVX$FPDEC_ADDGSU].action	= DO_ADDGSU;
      
      fpdec_func_table[EVX$FPDEC_ADDGSUC].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_ADDGSUC].text	= "ADDG/SUC";
      fpdec_func_table[EVX$FPDEC_ADDGSUC].action	= DO_ADDGSUC;
      
      fpdec_func_table[EVX$FPDEC_CMPGEQ].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_CMPGEQ].text		= "CMPGEQ";
      fpdec_func_table[EVX$FPDEC_CMPGEQ].action	= DO_CMPGEQ;
      
      fpdec_func_table[EVX$FPDEC_CMPGLT].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_CMPGLT].text		= "CMPGLT";
      fpdec_func_table[EVX$FPDEC_CMPGLT].action	= DO_CMPGLT;
      
      fpdec_func_table[EVX$FPDEC_CMPGLE].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_CMPGLE].text		= "CMPGLE";
      fpdec_func_table[EVX$FPDEC_CMPGLE].action	= DO_CMPGLE;
      
      fpdec_func_table[EVX$FPDEC_CMPGEQS].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_CMPGEQS].text	= "CMPGEQ/S";
      fpdec_func_table[EVX$FPDEC_CMPGEQS].action	= DO_CMPGEQS;
      
      fpdec_func_table[EVX$FPDEC_CMPGLTS].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_CMPGLTS].text	= "CMPGLT/S";
      fpdec_func_table[EVX$FPDEC_CMPGLTS].action	= DO_CMPGLTS;
      
      fpdec_func_table[EVX$FPDEC_CMPGLES].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_CMPGLES].text	= "CMPGLE/S";
      fpdec_func_table[EVX$FPDEC_CMPGLES].action	= DO_CMPGLES;
      
      fpdec_func_table[EVX$FPDEC_CVTDG].decode	= decode_operate_2f;
      fpdec_func_table[EVX$FPDEC_CVTDG].text		= "CVTDG";
      fpdec_func_table[EVX$FPDEC_CVTDG].action	= DO_CVTDG;
      
      fpdec_func_table[EVX$FPDEC_CVTDGC].decode	= decode_operate_2f;
      fpdec_func_table[EVX$FPDEC_CVTDGC].text		= "CVTDG/C";
      fpdec_func_table[EVX$FPDEC_CVTDGC].action	= DO_CVTDGC;
      
      fpdec_func_table[EVX$FPDEC_CVTDGU].decode	= decode_operate_2f;
      fpdec_func_table[EVX$FPDEC_CVTDGU].text		= "CVTDG/U";
      fpdec_func_table[EVX$FPDEC_CVTDGU].action	= DO_CVTDGU;
      
      fpdec_func_table[EVX$FPDEC_CVTDGUC].decode	= decode_operate_2f;
      fpdec_func_table[EVX$FPDEC_CVTDGUC].text	= "CVTDG/UC";
      fpdec_func_table[EVX$FPDEC_CVTDGUC].action	= DO_CVTDGUC;
      
      fpdec_func_table[EVX$FPDEC_CVTDGS].decode	= decode_operate_2f;
      fpdec_func_table[EVX$FPDEC_CVTDGS].text		= "CVTDG/S";
      fpdec_func_table[EVX$FPDEC_CVTDGS].action	= DO_CVTDGS;
      
      fpdec_func_table[EVX$FPDEC_CVTDGSC].decode	= decode_operate_2f;
      fpdec_func_table[EVX$FPDEC_CVTDGSC].text	= "CVTDG/SC";
      fpdec_func_table[EVX$FPDEC_CVTDGSC].action	= DO_CVTDGSC;
      
      fpdec_func_table[EVX$FPDEC_CVTDGSU].decode	= decode_operate_2f;
      fpdec_func_table[EVX$FPDEC_CVTDGSU].text	= "CVTDG/SU";
      fpdec_func_table[EVX$FPDEC_CVTDGSU].action	= DO_CVTDGSU;
      
      fpdec_func_table[EVX$FPDEC_CVTDGSUC].decode	= decode_operate_2f;
      fpdec_func_table[EVX$FPDEC_CVTDGSUC].text	= "CVTDG/SUC";
      fpdec_func_table[EVX$FPDEC_CVTDGSUC].action	= DO_CVTDGSUC;
      
      fpdec_func_table[EVX$FPDEC_CVTGD].decode	= decode_operate_2f;
      fpdec_func_table[EVX$FPDEC_CVTGD].text		= "CVTGD";
      fpdec_func_table[EVX$FPDEC_CVTGD].action	= DO_CVTGD;
      
      fpdec_func_table[EVX$FPDEC_CVTGDS].decode	= decode_operate_2f;
      fpdec_func_table[EVX$FPDEC_CVTGDS].text		= "CVTGD/S";
      fpdec_func_table[EVX$FPDEC_CVTGDS].action	= DO_CVTGDS;
      
      fpdec_func_table[EVX$FPDEC_CVTGDU].decode	= decode_operate_2f;
      fpdec_func_table[EVX$FPDEC_CVTGDU].text		= "CVTGD/U";
      fpdec_func_table[EVX$FPDEC_CVTGDU].action	= DO_CVTGDU;
      
      fpdec_func_table[EVX$FPDEC_CVTGDC].decode	= decode_operate_2f;
      fpdec_func_table[EVX$FPDEC_CVTGDC].text		= "CVTGD/C";
      fpdec_func_table[EVX$FPDEC_CVTGDC].action	= DO_CVTGDC;
      
      fpdec_func_table[EVX$FPDEC_CVTGDUC].decode	= decode_operate_2f;
      fpdec_func_table[EVX$FPDEC_CVTGDUC].text	= "CVTGD/UC";
      fpdec_func_table[EVX$FPDEC_CVTGDUC].action	= DO_CVTGDUC;
      
      fpdec_func_table[EVX$FPDEC_CVTGDSC].decode	= decode_operate_2f;
      fpdec_func_table[EVX$FPDEC_CVTGDSC].text	= "CVTGD/SC";
      fpdec_func_table[EVX$FPDEC_CVTGDSC].action	= DO_CVTGDSC;
      
      fpdec_func_table[EVX$FPDEC_CVTGDSU].decode	= decode_operate_2f;
      fpdec_func_table[EVX$FPDEC_CVTGDSU].text	= "CVTGD/SU";
      fpdec_func_table[EVX$FPDEC_CVTGDSU].action	= DO_CVTGDSU;
      
      fpdec_func_table[EVX$FPDEC_CVTGDSUC].decode	= decode_operate_2f;
      fpdec_func_table[EVX$FPDEC_CVTGDSUC].text	= "CVTGD/SUC";
      fpdec_func_table[EVX$FPDEC_CVTGDSUC].action	= DO_CVTGDSUC;
      
      fpdec_func_table[EVX$FPDEC_CVTGF].decode	= decode_operate_2f;
      fpdec_func_table[EVX$FPDEC_CVTGF].text		= "CVTGF";
      fpdec_func_table[EVX$FPDEC_CVTGF].action	= DO_CVTGF;
      
      fpdec_func_table[EVX$FPDEC_CVTGFC].decode	= decode_operate_2f;
      fpdec_func_table[EVX$FPDEC_CVTGFC].text		= "CVTGF/C";
      fpdec_func_table[EVX$FPDEC_CVTGFC].action	= DO_CVTGFC;
      
      fpdec_func_table[EVX$FPDEC_CVTGFU].decode	= decode_operate_2f;
      fpdec_func_table[EVX$FPDEC_CVTGFU].text		= "CVTGF/U";
      fpdec_func_table[EVX$FPDEC_CVTGFU].action	= DO_CVTGFU;
      
      fpdec_func_table[EVX$FPDEC_CVTGFUC].decode	= decode_operate_2f;
      fpdec_func_table[EVX$FPDEC_CVTGFUC].text	= "CVTGF/UC";
      fpdec_func_table[EVX$FPDEC_CVTGFUC].action	= DO_CVTGFUC;
      
      fpdec_func_table[EVX$FPDEC_CVTGFS].decode	= decode_operate_2f;
      fpdec_func_table[EVX$FPDEC_CVTGFS].text		= "CVTGF/S";
      fpdec_func_table[EVX$FPDEC_CVTGFS].action	= DO_CVTGFS;
      
      fpdec_func_table[EVX$FPDEC_CVTGFSC].decode	= decode_operate_2f;
      fpdec_func_table[EVX$FPDEC_CVTGFSC].text	= "CVTGF/SC";
      fpdec_func_table[EVX$FPDEC_CVTGFSC].action	= DO_CVTGFSC;
      
      fpdec_func_table[EVX$FPDEC_CVTGFSU].decode	= decode_operate_2f;
      fpdec_func_table[EVX$FPDEC_CVTGFSU].text	= "CVTGF/SU";
      fpdec_func_table[EVX$FPDEC_CVTGFSU].action	= DO_CVTGFSU;
      
      fpdec_func_table[EVX$FPDEC_CVTGFSUC].decode	= decode_operate_2f;
      fpdec_func_table[EVX$FPDEC_CVTGFSUC].text	= "CVTGF/SUC";
      fpdec_func_table[EVX$FPDEC_CVTGFSUC].action	= DO_CVTGFSUC;
      
      fpdec_func_table[EVX$FPDEC_CVTQG].decode	= decode_operate_2f;
      fpdec_func_table[EVX$FPDEC_CVTQG].text		= "CVTQG";
      fpdec_func_table[EVX$FPDEC_CVTQG].action	= DO_CVTQG;
      
      fpdec_func_table[EVX$FPDEC_CVTQGC].decode	= decode_operate_2f;
      fpdec_func_table[EVX$FPDEC_CVTQGC].text		= "CVTQG/C";
      fpdec_func_table[EVX$FPDEC_CVTQGC].action	= DO_CVTQGC;
      
      fpdec_func_table[EVX$FPDEC_CVTQGS].decode	= decode_operate_2f;
      fpdec_func_table[EVX$FPDEC_CVTQGS].text		= "CVTQG/S";
      fpdec_func_table[EVX$FPDEC_CVTQGS].action	= DO_CVTQGS;
      
      fpdec_func_table[EVX$FPDEC_CVTQGSC].decode	= decode_operate_2f;
      fpdec_func_table[EVX$FPDEC_CVTQGSC].text	= "CVTQG/SC";
      fpdec_func_table[EVX$FPDEC_CVTQGSC].action	= DO_CVTQGSC;
      
      fpdec_func_table[EVX$FPDEC_DIVF].decode		= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_DIVF].text		= "DIVF";
      fpdec_func_table[EVX$FPDEC_DIVF].action		= DO_DIVF;
      
      fpdec_func_table[EVX$FPDEC_DIVFC].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_DIVFC].text		= "DIVF/C";
      fpdec_func_table[EVX$FPDEC_DIVFC].action	= DO_DIVFC;
      
      fpdec_func_table[EVX$FPDEC_DIVFU].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_DIVFU].text		= "DIVF/U";
      fpdec_func_table[EVX$FPDEC_DIVFU].action	= DO_DIVFU;
      
      fpdec_func_table[EVX$FPDEC_DIVFUC].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_DIVFUC].text		= "DIVF/UC";
      fpdec_func_table[EVX$FPDEC_DIVFUC].action	= DO_DIVFUC;
      
      fpdec_func_table[EVX$FPDEC_DIVFS].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_DIVFS].text		= "DIVF/S";
      fpdec_func_table[EVX$FPDEC_DIVFS].action	= DO_DIVFS;
      
      fpdec_func_table[EVX$FPDEC_DIVFSC].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_DIVFSC].text		= "DIVF/SC";
      fpdec_func_table[EVX$FPDEC_DIVFSC].action	= DO_DIVFSC;
      
      fpdec_func_table[EVX$FPDEC_DIVFSU].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_DIVFSU].text		= "DIVF/SU";
      fpdec_func_table[EVX$FPDEC_DIVFSU].action	= DO_DIVFSU;
      
      fpdec_func_table[EVX$FPDEC_DIVFSUC].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_DIVFSUC].text	= "DIVF/SUC";
      fpdec_func_table[EVX$FPDEC_DIVFSUC].action	= DO_DIVFSUC;
      
      fpdec_func_table[EVX$FPDEC_DIVG].decode		= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_DIVG].text		= "DIVG";
      fpdec_func_table[EVX$FPDEC_DIVG].action		= DO_DIVG;
      
      fpdec_func_table[EVX$FPDEC_DIVGC].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_DIVGC].text		= "DIVG/C";
      fpdec_func_table[EVX$FPDEC_DIVGC].action	= DO_DIVGC;
      
      fpdec_func_table[EVX$FPDEC_DIVGU].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_DIVGU].text		= "DIVG/U";
      fpdec_func_table[EVX$FPDEC_DIVGU].action	= DO_DIVGU;
      
      fpdec_func_table[EVX$FPDEC_DIVGUC].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_DIVGUC].text		= "DIVG/UC";
      fpdec_func_table[EVX$FPDEC_DIVGUC].action	= DO_DIVGUC;
      
      fpdec_func_table[EVX$FPDEC_DIVGS].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_DIVGS].text		= "DIVG/S";
      fpdec_func_table[EVX$FPDEC_DIVGS].action	= DO_DIVGS;
      
      fpdec_func_table[EVX$FPDEC_DIVGSC].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_DIVGSC].text		= "DIVG/SC";
      fpdec_func_table[EVX$FPDEC_DIVGSC].action	= DO_DIVGSC;
      
      fpdec_func_table[EVX$FPDEC_DIVGSU].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_DIVGSU].text		= "DIVG/SU";
      fpdec_func_table[EVX$FPDEC_DIVGSU].action	= DO_DIVGSU;
      
      fpdec_func_table[EVX$FPDEC_DIVGSUC].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_DIVGSUC].text	= "DIVG/SUC";
      fpdec_func_table[EVX$FPDEC_DIVGSUC].action	= DO_DIVGSUC;
      
      fpdec_func_table[EVX$FPDEC_MULF].decode		= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_MULF].text		= "MULF";
      fpdec_func_table[EVX$FPDEC_MULF].action		= DO_MULF;
      
      fpdec_func_table[EVX$FPDEC_MULFC].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_MULFC].text		= "MULF/C";
      fpdec_func_table[EVX$FPDEC_MULFC].action	= DO_MULFC;
      
      fpdec_func_table[EVX$FPDEC_MULFU].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_MULFU].text		= "MULF/U";
      fpdec_func_table[EVX$FPDEC_MULFU].action	= DO_MULFU;
      
      fpdec_func_table[EVX$FPDEC_MULFUC].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_MULFUC].text		= "MULF/UC";
      fpdec_func_table[EVX$FPDEC_MULFUC].action	= DO_MULFUC;
      
      fpdec_func_table[EVX$FPDEC_MULFS].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_MULFS].text		= "MULF/S";
      fpdec_func_table[EVX$FPDEC_MULFS].action	= DO_MULFS;
      
      fpdec_func_table[EVX$FPDEC_MULFSC].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_MULFSC].text		= "MULF/SC";
      fpdec_func_table[EVX$FPDEC_MULFSC].action	= DO_MULFSC;
      
      fpdec_func_table[EVX$FPDEC_MULFSU].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_MULFSU].text		= "MULF/SU";
      fpdec_func_table[EVX$FPDEC_MULFSU].action	= DO_MULFSU;
      
      fpdec_func_table[EVX$FPDEC_MULFSUC].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_MULFSUC].text	= "MULF/SUC";
      fpdec_func_table[EVX$FPDEC_MULFSUC].action	= DO_MULFSUC;
      
      fpdec_func_table[EVX$FPDEC_MULG].decode		= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_MULG].text		= "MULG";
      fpdec_func_table[EVX$FPDEC_MULG].action		= DO_MULG;
      
      fpdec_func_table[EVX$FPDEC_MULGC].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_MULGC].text		= "MULG/C";
      fpdec_func_table[EVX$FPDEC_MULGC].action	= DO_MULGC;
      
      fpdec_func_table[EVX$FPDEC_MULGU].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_MULGU].text		= "MULG/U";
      fpdec_func_table[EVX$FPDEC_MULGU].action	= DO_MULGU;
      
      fpdec_func_table[EVX$FPDEC_MULGUC].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_MULGUC].text		= "MULG/UC";
      fpdec_func_table[EVX$FPDEC_MULGUC].action	= DO_MULGUC;
      
      fpdec_func_table[EVX$FPDEC_MULGS].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_MULGS].text		= "MULG/S";
      fpdec_func_table[EVX$FPDEC_MULGS].action	= DO_MULGS;
      
      fpdec_func_table[EVX$FPDEC_MULGSC].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_MULGSC].text		= "MULG/SC";
      fpdec_func_table[EVX$FPDEC_MULGSC].action	= DO_MULGSC;
      
      fpdec_func_table[EVX$FPDEC_MULGSU].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_MULGSU].text		= "MULG/SU";
      fpdec_func_table[EVX$FPDEC_MULGSU].action	= DO_MULGSU;
      
      fpdec_func_table[EVX$FPDEC_MULGSUC].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_MULGSUC].text	= "MULG/SUC";
      fpdec_func_table[EVX$FPDEC_MULGSUC].action 	= DO_MULGSUC;
      
      fpdec_func_table[EVX$FPDEC_SUBF].decode		= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_SUBF].text		= "SUBF";
      fpdec_func_table[EVX$FPDEC_SUBF].action		= DO_SUBF;
      
      fpdec_func_table[EVX$FPDEC_SUBFC].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_SUBFC].text		= "SUBF/C";
      fpdec_func_table[EVX$FPDEC_SUBFC].action	= DO_SUBFC;
      
      fpdec_func_table[EVX$FPDEC_SUBFU].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_SUBFU].text		= "SUBF/U";
      fpdec_func_table[EVX$FPDEC_SUBFU].action	= DO_SUBFU;
      
      fpdec_func_table[EVX$FPDEC_SUBFUC].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_SUBFUC].text		= "SUBF/UC";
      fpdec_func_table[EVX$FPDEC_SUBFUC].action	= DO_SUBFUC;
      
      fpdec_func_table[EVX$FPDEC_SUBFS].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_SUBFS].text		= "SUBF/S";
      fpdec_func_table[EVX$FPDEC_SUBFS].action	= DO_SUBFS;
      
      fpdec_func_table[EVX$FPDEC_SUBFSC].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_SUBFSC].text		= "SUBF/SC";
      fpdec_func_table[EVX$FPDEC_SUBFSC].action	= DO_SUBFSC;
      
      fpdec_func_table[EVX$FPDEC_SUBFSU].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_SUBFSU].text		= "SUBF/SU";
      fpdec_func_table[EVX$FPDEC_SUBFSU].action	= DO_SUBFSU;
      
      fpdec_func_table[EVX$FPDEC_SUBFSUC].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_SUBFSUC].text	= "SUBF/SUC";
      fpdec_func_table[EVX$FPDEC_SUBFSUC].action	= DO_SUBFSUC;
      
      fpdec_func_table[EVX$FPDEC_SUBG].decode		= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_SUBG].text		= "SUBG";
      fpdec_func_table[EVX$FPDEC_SUBG].action		= DO_SUBG;
      
      fpdec_func_table[EVX$FPDEC_SUBGC].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_SUBGC].text		= "SUBG/C";
      fpdec_func_table[EVX$FPDEC_SUBGC].action	= DO_SUBGC;
      
      fpdec_func_table[EVX$FPDEC_SUBGU].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_SUBGU].text		= "SUBG/U";
      fpdec_func_table[EVX$FPDEC_SUBGU].action	= DO_SUBGU;
      
      fpdec_func_table[EVX$FPDEC_SUBGUC].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_SUBGUC].text		= "SUBG/UC";
      fpdec_func_table[EVX$FPDEC_SUBGUC].action	= DO_SUBGUC;
      
      fpdec_func_table[EVX$FPDEC_SUBGS].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_SUBGS].text		= "SUBG/S";
      fpdec_func_table[EVX$FPDEC_SUBGS].action	= DO_SUBGS;
      
      fpdec_func_table[EVX$FPDEC_SUBGSC].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_SUBGSC].text		= "SUBG/SC";
      fpdec_func_table[EVX$FPDEC_SUBGSC].action	= DO_SUBGSC;
      
      fpdec_func_table[EVX$FPDEC_SUBGSU].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_SUBGSU].text		= "SUBG/SU";
      fpdec_func_table[EVX$FPDEC_SUBGSU].action	= DO_SUBGSU;
      
      fpdec_func_table[EVX$FPDEC_SUBGSUC].decode	= decode_operate_3f;
      fpdec_func_table[EVX$FPDEC_SUBGSUC].text	= "SUBG/SUC";
      fpdec_func_table[EVX$FPDEC_SUBGSUC].action	= DO_SUBGSUC;
      
      fpdec_func_table[EVX$FPDEC_CVTGQ].decode	= decode_operate_2f;
      fpdec_func_table[EVX$FPDEC_CVTGQ].text		= "CVTGQ";
      fpdec_func_table[EVX$FPDEC_CVTGQ].action	= DO_CVTGQ;
      
      fpdec_func_table[EVX$FPDEC_CVTGQC].decode	= decode_operate_2f;
      fpdec_func_table[EVX$FPDEC_CVTGQC].text		= "CVTGQ/C";
      fpdec_func_table[EVX$FPDEC_CVTGQC].action	= DO_CVTGQC;
      
      fpdec_func_table[EVX$FPDEC_CVTGQV].decode	= decode_operate_2f;
      fpdec_func_table[EVX$FPDEC_CVTGQV].text		= "CVTGQ/V";
      fpdec_func_table[EVX$FPDEC_CVTGQV].action	= DO_CVTGQV;
      
      fpdec_func_table[EVX$FPDEC_CVTGQVC].decode	= decode_operate_2f;
      fpdec_func_table[EVX$FPDEC_CVTGQVC].text	= "CVTGQ/VC";
      fpdec_func_table[EVX$FPDEC_CVTGQVC].action	= DO_CVTGQVC;
      
      fpdec_func_table[EVX$FPDEC_CVTGQS].decode	= decode_operate_2f;
      fpdec_func_table[EVX$FPDEC_CVTGQS].text		= "CVTGQ/S";
      fpdec_func_table[EVX$FPDEC_CVTGQS].action	= DO_CVTGQS;
      
      fpdec_func_table[EVX$FPDEC_CVTGQSC].decode	= decode_operate_2f;
      fpdec_func_table[EVX$FPDEC_CVTGQSC].text	= "CVTGQ/SC";
      fpdec_func_table[EVX$FPDEC_CVTGQSC].action	= DO_CVTGQSC;
      
      fpdec_func_table[EVX$FPDEC_CVTGQSV].decode	= decode_operate_2f;
      fpdec_func_table[EVX$FPDEC_CVTGQSV].text	= "CVTGQ/SV";
      fpdec_func_table[EVX$FPDEC_CVTGQSV].action	= DO_CVTGQSV;
      
      fpdec_func_table[EVX$FPDEC_CVTGQSVC].decode	= decode_operate_2f;
      fpdec_func_table[EVX$FPDEC_CVTGQSVC].text	= "CVTGQ/SVC";
      fpdec_func_table[EVX$FPDEC_CVTGQSVC].action	= DO_CVTGQSVC;
      
      opcode_table[EVX$OPC_FPAUX].decode	= decode_by_fpu_subfun;
      opcode_table[EVX$OPC_FPAUX].next	= fpaux_func_table;
      opcode_table[EVX$OPC_FPAUX].action	= DO_DISP_BY_FPU_FUNC;
      
      fpaux_func_table[EVX$FPAUX_CPYS].decode		= decode_operate_3f;
      fpaux_func_table[EVX$FPAUX_CPYS].text		= "CPYS";
      fpaux_func_table[EVX$FPAUX_CPYS].action		= DO_CPYS;
      
      fpaux_func_table[EVX$FPAUX_CPYSN].decode	= decode_operate_3f;
      fpaux_func_table[EVX$FPAUX_CPYSN].text		= "CPYSN";
      fpaux_func_table[EVX$FPAUX_CPYSN].action	= DO_CPYSN;
      
      fpaux_func_table[EVX$FPAUX_CPYSE].decode	= decode_operate_3f;
      fpaux_func_table[EVX$FPAUX_CPYSE].text		= "CPYSE";
      fpaux_func_table[EVX$FPAUX_CPYSE].action	= DO_CPYSE;

      fpaux_func_table[EVX$FPAUX_CPYSEE].decode	= decode_operate_3f;	/* V3.24 in HAL, but not a valid instruction? */
      fpaux_func_table[EVX$FPAUX_CPYSEE].text		= "CPYSEE";
      fpaux_func_table[EVX$FPAUX_CPYSEE].action	= DO_CPYSEE;
      
      /* REMOVE THESE TBD ***** */
      fpaux_func_table[EVX$FPAUX_CPYS &~0X20].decode		= decode_operate_3f;
      fpaux_func_table[EVX$FPAUX_CPYS &~0X20].text		= "CPYS";
      fpaux_func_table[EVX$FPAUX_CPYS &~0X20].action		= DO_CPYS;
      
      fpaux_func_table[EVX$FPAUX_CPYSN &~0X20].decode	= decode_operate_3f;
      fpaux_func_table[EVX$FPAUX_CPYSN &~0X20].text		= "CPYSN";
      fpaux_func_table[EVX$FPAUX_CPYSN &~0X20].action	= DO_CPYSN;
      
      fpaux_func_table[EVX$FPAUX_CPYSE &~0X20].decode	= decode_operate_3f;
      fpaux_func_table[EVX$FPAUX_CPYSE &~0X20].text		= "CPYSE";
      fpaux_func_table[EVX$FPAUX_CPYSE &~0X20].action	= DO_CPYSE;
      
      /* REMOVE THESE TBD ***** */
      
      fpaux_func_table[EVX$FPAUX_MF_FPCR].decode	= decode_operate_1f;
      fpaux_func_table[EVX$FPAUX_MF_FPCR].text	= "MF_FPCR";		/*V3.24*/
      fpaux_func_table[EVX$FPAUX_MF_FPCR].action	= DO_MF_FPCR;
      
      fpaux_func_table[EVX$FPAUX_MT_FPCR].decode	= decode_operate_1f;
      fpaux_func_table[EVX$FPAUX_MT_FPCR].text	= "MT_FPCR";		/*V3.24*/
      fpaux_func_table[EVX$FPAUX_MT_FPCR].action	= DO_MT_FPCR;
      
      fpaux_func_table[EVX$FPAUX_CVTLQ].decode	= decode_operate_2f;
      fpaux_func_table[EVX$FPAUX_CVTLQ].text		= "CVTLQ";
      fpaux_func_table[EVX$FPAUX_CVTLQ].action	= DO_CVTLQ;
      
      fpaux_func_table[EVX$FPAUX_CVTQL].decode	= decode_operate_2f;
      fpaux_func_table[EVX$FPAUX_CVTQL].text		= "CVTQL";
      fpaux_func_table[EVX$FPAUX_CVTQL].action	= DO_CVTQL;
      
      fpaux_func_table[EVX$FPAUX_CVTQLV].decode	= decode_operate_2f;
      fpaux_func_table[EVX$FPAUX_CVTQLV].text		= "CVTQL/V";
      fpaux_func_table[EVX$FPAUX_CVTQLV].action	= DO_CVTQLV;
      
      fpaux_func_table[EVX$FPAUX_CVTQLSV].decode	= decode_operate_2f;
      fpaux_func_table[EVX$FPAUX_CVTQLSV].text	= "CVTQL/SV";
      fpaux_func_table[EVX$FPAUX_CVTQLSV].action	= DO_CVTQLSV;
      
      fpaux_func_table[EVX$FPAUX_FCMOVEQ].decode	= decode_operate_3;
      fpaux_func_table[EVX$FPAUX_FCMOVEQ].text	= "FCMOVEQ";
      fpaux_func_table[EVX$FPAUX_FCMOVEQ].action	= DO_FCMOVEQ;
      
      fpaux_func_table[EVX$FPAUX_FCMOVNE].decode	= decode_operate_3;
      fpaux_func_table[EVX$FPAUX_FCMOVNE].text	= "FCMOVNE";
      fpaux_func_table[EVX$FPAUX_FCMOVNE].action	= DO_FCMOVNE;
      
      fpaux_func_table[EVX$FPAUX_FCMOVLT].decode	= decode_operate_3;
      fpaux_func_table[EVX$FPAUX_FCMOVLT].text	= "FCMOVLT";
      fpaux_func_table[EVX$FPAUX_FCMOVLT].action	= DO_FCMOVLT;
      
      fpaux_func_table[EVX$FPAUX_FCMOVLE].decode	= decode_operate_3;
      fpaux_func_table[EVX$FPAUX_FCMOVLE].text	= "FCMOVLE";
      fpaux_func_table[EVX$FPAUX_FCMOVLE].action	= DO_FCMOVLE;
      
      fpaux_func_table[EVX$FPAUX_FCMOVGT].decode	= decode_operate_3;
      fpaux_func_table[EVX$FPAUX_FCMOVGT].text	= "FCMOVGT";
      fpaux_func_table[EVX$FPAUX_FCMOVGT].action	= DO_FCMOVGT;
      
      fpaux_func_table[EVX$FPAUX_FCMOVGE].decode	= decode_operate_3;
      fpaux_func_table[EVX$FPAUX_FCMOVGE].text	= "FCMOVGE";
      fpaux_func_table[EVX$FPAUX_FCMOVGE].action	= DO_FCMOVGE;
      
      /* REMOVE THESE TBD ***** */
      fpaux_func_table[EVX$FPAUX_FCMOVEQ &~0X20].decode	= decode_operate_3;
      fpaux_func_table[EVX$FPAUX_FCMOVEQ &~0X20].text	= "FCMOVEQ";
      fpaux_func_table[EVX$FPAUX_FCMOVEQ &~0X20].action	= DO_FCMOVEQ;
      
      fpaux_func_table[EVX$FPAUX_FCMOVNE &~0X20].decode	= decode_operate_3;
      fpaux_func_table[EVX$FPAUX_FCMOVNE &~0X20].text	= "FCMOVNE";
      fpaux_func_table[EVX$FPAUX_FCMOVNE &~0X20].action	= DO_FCMOVNE;
      
      fpaux_func_table[EVX$FPAUX_FCMOVLT &~0X20].decode	= decode_operate_3;
      fpaux_func_table[EVX$FPAUX_FCMOVLT &~0X20].text	= "FCMOVLT";
      fpaux_func_table[EVX$FPAUX_FCMOVLT &~0X20].action	= DO_FCMOVLT;
      
      fpaux_func_table[EVX$FPAUX_FCMOVLE &~0X20].decode	= decode_operate_3;
      fpaux_func_table[EVX$FPAUX_FCMOVLE &~0X20].text	= "FCMOVLE";
      fpaux_func_table[EVX$FPAUX_FCMOVLE &~0X20].action	= DO_FCMOVLE;
      
      fpaux_func_table[EVX$FPAUX_FCMOVGT &~0X20].decode	= decode_operate_3;
      fpaux_func_table[EVX$FPAUX_FCMOVGT &~0X20].text	= "FCMOVGT";
      fpaux_func_table[EVX$FPAUX_FCMOVGT &~0X20].action	= DO_FCMOVGT;
      
      fpaux_func_table[EVX$FPAUX_FCMOVGE &~0X20].decode	= decode_operate_3;
      fpaux_func_table[EVX$FPAUX_FCMOVGE &~0X20].text	= "FCMOVGE";
      fpaux_func_table[EVX$FPAUX_FCMOVGE &~0X20].action	= DO_FCMOVGE;
      /* REMOVE THESE TBD ***** */
      
      opcode_table[EVX$OPC_SYNC].decode	= decode_by_memfun;
      opcode_table[EVX$OPC_SYNC].next	= sync_func_table;
      opcode_table[EVX$OPC_SYNC].action	= DO_DISP_BY_MEM_FUNC;
      
      sync_func_table[EVX$SYNC_DRAINT>>10].decode	= decode_epi;
      sync_func_table[EVX$SYNC_DRAINT>>10].text	= "DRAINT";
      sync_func_table[EVX$SYNC_DRAINT>>10].action	= DO_DRAINT;
      
      sync_func_table[EVX$SYNC_EXCB>>10].decode		= decode_epi;
      sync_func_table[EVX$SYNC_EXCB>>10].text		= "EXCB";
      sync_func_table[EVX$SYNC_EXCB>>10].action		= DO_DRAINT;
      
      sync_func_table[EVX$SYNC_MB>>10].decode		= decode_epi;
      sync_func_table[EVX$SYNC_MB>>10].text		= "MB";
      sync_func_table[EVX$SYNC_MB>>10].action		= DO_MB;
      
      sync_func_table[EVX$SYNC_WMB>>10].decode		= decode_epi;
      sync_func_table[EVX$SYNC_WMB>>10].text		= "WMB";
      sync_func_table[EVX$SYNC_WMB>>10].action		= DO_MB;
      
      sync_func_table[EVX$SYNC_RCC>>10].decode	= decode_epi;
      sync_func_table[EVX$SYNC_RCC>>10].text		= "RPCC";
      sync_func_table[EVX$SYNC_RCC>>10].action	= DO_RCC;
      
      sync_func_table[EVX$SYNC_RC>>10].decode		= decode_epi;
      sync_func_table[EVX$SYNC_RC>>10].text		= "RC";
      sync_func_table[EVX$SYNC_RC>>10].action		= DO_RC;
      
      sync_func_table[EVX$SYNC_RS>>10].decode		= decode_epi;
      sync_func_table[EVX$SYNC_RS>>10].text		= "RS";
      sync_func_table[EVX$SYNC_RS>>10].action		= DO_RS;
      
      sync_func_table[EVX$SYNC_FETCH>>10].decode 	= decode_epi_rn;
      sync_func_table[EVX$SYNC_FETCH>>10].text	= "FETCH";
      sync_func_table[EVX$SYNC_FETCH>>10].action	= DO_FETCH;
      
      sync_func_table[EVX$SYNC_FETCH_M>>10].decode	= decode_epi_rn;
      sync_func_table[EVX$SYNC_FETCH_M>>10].text 	= "FETCH_M";
      sync_func_table[EVX$SYNC_FETCH_M>>10].action	= DO_FETCH_M;

      sync_func_table[EVX$SYNC_WH64>>10].decode	= decode_epi_rn;	/*V3.24*/
      sync_func_table[EVX$SYNC_WH64>>10].text 	= "WH64";
      sync_func_table[EVX$SYNC_WH64>>10].action	= DO_WH64;

      sync_func_table[EVX$SYNC_ECB>>10].decode	= decode_epi_rn;	/*V3.24*/
      sync_func_table[EVX$SYNC_ECB>>10].text 	= "ECB";
      sync_func_table[EVX$SYNC_ECB>>10].action	= DO_ECB;
      
      opcode_table[EVX$OPC_HW_LD].decode	= chip->decode_hw_memory;
      opcode_table[EVX$OPC_HW_LD].text	= 	"LD";	
      opcode_table[EVX$OPC_HW_LD].action	= DO_HW_LD;
      
      opcode_table[EVX$OPC_HW_ST].decode	= chip->decode_hw_memory;
      opcode_table[EVX$OPC_HW_ST].text	=	 "ST";
      opcode_table[EVX$OPC_HW_ST].action	= DO_HW_ST;
      
      opcode_table[EVX$OPC_HW_MTPR].decode	= chip->decode_ipr;
      opcode_table[EVX$OPC_HW_MTPR].text		= "MT"; /*V3.24*/
      opcode_table[EVX$OPC_HW_MTPR].action	= DO_HW_MTPR;
      
      opcode_table[EVX$OPC_HW_MFPR].decode	= chip->decode_ipr;
      opcode_table[EVX$OPC_HW_MFPR].text		= "MF"; /*V3.24*/
      opcode_table[EVX$OPC_HW_MFPR].action	= DO_HW_MFPR;
      
      opcode_table[EVX$OPC_HW_REI].decode	= chip->decode_hw_memory; /*V3.24*/
      opcode_table[EVX$OPC_HW_REI].text	= "HW_REI";
      opcode_table[EVX$OPC_HW_REI].action	= DO_HW_REI;


      opcode_table[EVX$OPC_SEXT].decode	= decode_by_subfun;
      opcode_table[EVX$OPC_SEXT].next	= sext_func_table;
      opcode_table[EVX$OPC_SEXT].action	= DO_DISP_BY_FUNC;
      
      sext_func_table[EVX$SEXT_SEXTB].decode	= decode_operate_2;
      sext_func_table[EVX$SEXT_SEXTB].text	= "SEXTB";
      sext_func_table[EVX$SEXT_SEXTB].action	= DO_SEXTB;

      sext_func_table[EVX$SEXT_SEXTW].decode	= decode_operate_2;
      sext_func_table[EVX$SEXT_SEXTW].text	= "SEXTW";
      sext_func_table[EVX$SEXT_SEXTW].action	= DO_SEXTW;

      sext_func_table[EVX$SEXT_CTPOP].decode	= decode_operate_2;
      sext_func_table[EVX$SEXT_CTPOP].text	= "CTPOP";
      sext_func_table[EVX$SEXT_CTPOP].action	= DO_CTPOP;

      sext_func_table[EVX$SEXT_CTLZ].decode	= decode_operate_2;
      sext_func_table[EVX$SEXT_CTLZ].text	= "CTLZ";
      sext_func_table[EVX$SEXT_CTLZ].action	= DO_CTLZ;

      sext_func_table[EVX$SEXT_CTTZ].decode	= decode_operate_2;
      sext_func_table[EVX$SEXT_CTTZ].text	= "CTTZ";
      sext_func_table[EVX$SEXT_CTTZ].action	= DO_CTTZ;

      sext_func_table[EVX$SEXT_PERR].decode	= decode_operate_3;
      sext_func_table[EVX$SEXT_PERR].text	= "PERR";
      sext_func_table[EVX$SEXT_PERR].action	= DO_PERR;

      sext_func_table[EVX$SEXT_MINUB8].decode	= decode_operate_3;
      sext_func_table[EVX$SEXT_MINUB8].text	= "MINUB8";
      sext_func_table[EVX$SEXT_MINUB8].action	= DO_MINUB8;

      sext_func_table[EVX$SEXT_MINUW4].decode	= decode_operate_3;
      sext_func_table[EVX$SEXT_MINUW4].text	= "MINUW4";
      sext_func_table[EVX$SEXT_MINUW4].action	= DO_MINUW4;

      sext_func_table[EVX$SEXT_MINSB8].decode	= decode_operate_3;
      sext_func_table[EVX$SEXT_MINSB8].text	= "MINSB8";
      sext_func_table[EVX$SEXT_MINSB8].action	= DO_MINSB8;

      sext_func_table[EVX$SEXT_MINSW4].decode	= decode_operate_3;
      sext_func_table[EVX$SEXT_MINSW4].text	= "MINSW4";
      sext_func_table[EVX$SEXT_MINSW4].action	= DO_MINSW4;

      sext_func_table[EVX$SEXT_MAXUB8].decode	= decode_operate_3;
      sext_func_table[EVX$SEXT_MAXUB8].text	= "MAXUB8";
      sext_func_table[EVX$SEXT_MAXUB8].action	= DO_MAXUB8;

      sext_func_table[EVX$SEXT_MAXUW4].decode	= decode_operate_3;
      sext_func_table[EVX$SEXT_MAXUW4].text	= "MAXUW4";
      sext_func_table[EVX$SEXT_MAXUW4].action	= DO_MAXUW4;

      sext_func_table[EVX$SEXT_MAXSB8].decode	= decode_operate_3;
      sext_func_table[EVX$SEXT_MAXSB8].text	= "MAXSB8";
      sext_func_table[EVX$SEXT_MAXSB8].action	= DO_MAXSB8;

      sext_func_table[EVX$SEXT_MAXSW4].decode	= decode_operate_3;
      sext_func_table[EVX$SEXT_MAXSW4].text	= "MAXSW4";
      sext_func_table[EVX$SEXT_MAXSW4].action	= DO_MAXSW4;

      sext_func_table[EVX$SEXT_UNPKBW].decode	= decode_operate_2;
      sext_func_table[EVX$SEXT_UNPKBW].text	= "UNPKBW";
      sext_func_table[EVX$SEXT_UNPKBW].action	= DO_UNPKBW;

      sext_func_table[EVX$SEXT_UNPKBL].decode	= decode_operate_2;
      sext_func_table[EVX$SEXT_UNPKBL].text	= "UNPKBL";
      sext_func_table[EVX$SEXT_UNPKBL].action	= DO_UNPKBL;

      sext_func_table[EVX$SEXT_PKWB].decode	= decode_operate_2;
      sext_func_table[EVX$SEXT_PKWB].text	= "PKWB";
      sext_func_table[EVX$SEXT_PKWB].action	= DO_PKWB;

      sext_func_table[EVX$SEXT_PKLB].decode	= decode_operate_2;
      sext_func_table[EVX$SEXT_PKLB].text	= "PKLB";
      sext_func_table[EVX$SEXT_PKLB].action	= DO_PKLB;

      sext_func_table[EVX$SEXT_FTOIT].decode	= decode_operate_fa_rc;
      sext_func_table[EVX$SEXT_FTOIT].text	= "FTOIT";
      sext_func_table[EVX$SEXT_FTOIT].action	= DO_FTOIT;

      sext_func_table[EVX$SEXT_FTOIS].decode	= decode_operate_fa_rc;
      sext_func_table[EVX$SEXT_FTOIS].text	= "FTOIS";
      sext_func_table[EVX$SEXT_FTOIS].action	= DO_FTOIS;

      opcode_table[EVX$OPC_SQRT].decode	= decode_by_fpu_subfun;
      opcode_table[EVX$OPC_SQRT].next	= sqrt_func_table;
      opcode_table[EVX$OPC_SQRT].action	= DO_DISP_BY_FPU_FUNC;

/* itof are really non fpu operate format, but it will decode okay if we
pretend it is floating point operate since no literal mode. */

      sqrt_func_table[EVX$ITOF_ITOFF].decode	= decode_operate_ra_fc;
      sqrt_func_table[EVX$ITOF_ITOFF].text	= "ITOFF";
      sqrt_func_table[EVX$ITOF_ITOFF].action	= DO_ITOFF;

      sqrt_func_table[EVX$ITOF_ITOFS].decode	= decode_operate_ra_fc;
      sqrt_func_table[EVX$ITOF_ITOFS].text	= "ITOFS";
      sqrt_func_table[EVX$ITOF_ITOFS].action	= DO_ITOFS;

      sqrt_func_table[EVX$ITOF_ITOFT].decode	= decode_operate_ra_fc;
      sqrt_func_table[EVX$ITOF_ITOFT].text	= "ITOFT";
      sqrt_func_table[EVX$ITOF_ITOFT].action	= DO_ITOFT;
      
      sqrt_func_table[EVX$SQRT_SQRTS].decode		= decode_operate_2f;
      sqrt_func_table[EVX$SQRT_SQRTS].text		= "SQRTS";
      sqrt_func_table[EVX$SQRT_SQRTS].action		= DO_SQRTS;
      
      sqrt_func_table[EVX$SQRT_SQRTSC].decode		= decode_operate_2f;
      sqrt_func_table[EVX$SQRT_SQRTSC].text		= "SQRTSC";
      sqrt_func_table[EVX$SQRT_SQRTSC].action		= DO_SQRTSC;
      
      sqrt_func_table[EVX$SQRT_SQRTSM].decode		= decode_operate_2f;
      sqrt_func_table[EVX$SQRT_SQRTSM].text		= "SQRTSM";
      sqrt_func_table[EVX$SQRT_SQRTSM].action		= DO_SQRTSM;
      
      sqrt_func_table[EVX$SQRT_SQRTSD].decode		= decode_operate_2f;
      sqrt_func_table[EVX$SQRT_SQRTSD].text		= "SQRTSD";
      sqrt_func_table[EVX$SQRT_SQRTSD].action		= DO_SQRTSD;
      
      sqrt_func_table[EVX$SQRT_SQRTSU].decode		= decode_operate_2f;
      sqrt_func_table[EVX$SQRT_SQRTSU].text		= "SQRTSU";
      sqrt_func_table[EVX$SQRT_SQRTSU].action		= DO_SQRTSU;
      
      sqrt_func_table[EVX$SQRT_SQRTSUC].decode		= decode_operate_2f;
      sqrt_func_table[EVX$SQRT_SQRTSUC].text		= "SQRTSUC";
      sqrt_func_table[EVX$SQRT_SQRTSUC].action		= DO_SQRTSUC;
      
      sqrt_func_table[EVX$SQRT_SQRTSUM].decode		= decode_operate_2f;
      sqrt_func_table[EVX$SQRT_SQRTSUM].text		= "SQRTSUM";
      sqrt_func_table[EVX$SQRT_SQRTSUM].action		= DO_SQRTSUM;
      
      sqrt_func_table[EVX$SQRT_SQRTSUD].decode		= decode_operate_2f;
      sqrt_func_table[EVX$SQRT_SQRTSUD].text		= "SQRTSUD";
      sqrt_func_table[EVX$SQRT_SQRTSUD].action		= DO_SQRTSUD;
      
      sqrt_func_table[EVX$SQRT_SQRTSSU].decode		= decode_operate_2f;
      sqrt_func_table[EVX$SQRT_SQRTSSU].text		= "SQRTSSU";
      sqrt_func_table[EVX$SQRT_SQRTSSU].action		= DO_SQRTSSU;
      
      sqrt_func_table[EVX$SQRT_SQRTSSUC].decode		= decode_operate_2f;
      sqrt_func_table[EVX$SQRT_SQRTSSUC].text		= "SQRTSSUC";
      sqrt_func_table[EVX$SQRT_SQRTSSUC].action		= DO_SQRTSSUC;
      
      sqrt_func_table[EVX$SQRT_SQRTSSUM].decode		= decode_operate_2f;
      sqrt_func_table[EVX$SQRT_SQRTSSUM].text		= "SQRTSSUM";
      sqrt_func_table[EVX$SQRT_SQRTSSUM].action		= DO_SQRTSSUM;
      
      sqrt_func_table[EVX$SQRT_SQRTSSUD].decode		= decode_operate_2f;
      sqrt_func_table[EVX$SQRT_SQRTSSUD].text		= "SQRTSSUD";
      sqrt_func_table[EVX$SQRT_SQRTSSUD].action		= DO_SQRTSSUD;
      
      sqrt_func_table[EVX$SQRT_SQRTSSUI].decode		= decode_operate_2f;
      sqrt_func_table[EVX$SQRT_SQRTSSUI].text		= "SQRTSSUI";
      sqrt_func_table[EVX$SQRT_SQRTSSUI].action		= DO_SQRTSSUI;
      
      sqrt_func_table[EVX$SQRT_SQRTSSUIC].decode		= decode_operate_2f;
      sqrt_func_table[EVX$SQRT_SQRTSSUIC].text		= "SQRTSSUIC";
      sqrt_func_table[EVX$SQRT_SQRTSSUIC].action		= DO_SQRTSSUIC;
      
      sqrt_func_table[EVX$SQRT_SQRTSSUIM].decode		= decode_operate_2f;
      sqrt_func_table[EVX$SQRT_SQRTSSUIM].text		= "SQRTSSUIM";
      sqrt_func_table[EVX$SQRT_SQRTSSUIM].action		= DO_SQRTSSUIM;
      
      sqrt_func_table[EVX$SQRT_SQRTSSUID].decode		= decode_operate_2f;
      sqrt_func_table[EVX$SQRT_SQRTSSUID].text		= "SQRTSSUID";
      sqrt_func_table[EVX$SQRT_SQRTSSUID].action		= DO_SQRTSSUID;
      
      sqrt_func_table[EVX$SQRT_SQRTT].decode		= decode_operate_2f;
      sqrt_func_table[EVX$SQRT_SQRTT].text		= "SQRTT";
      sqrt_func_table[EVX$SQRT_SQRTT].action		= DO_SQRTT;
      
      sqrt_func_table[EVX$SQRT_SQRTTC].decode		= decode_operate_2f;
      sqrt_func_table[EVX$SQRT_SQRTTC].text		= "SQRTTC";
      sqrt_func_table[EVX$SQRT_SQRTTC].action		= DO_SQRTTC;
      
      sqrt_func_table[EVX$SQRT_SQRTTM].decode		= decode_operate_2f;
      sqrt_func_table[EVX$SQRT_SQRTTM].text		= "SQRTTM";
      sqrt_func_table[EVX$SQRT_SQRTTM].action		= DO_SQRTTM;
      
      sqrt_func_table[EVX$SQRT_SQRTTD].decode		= decode_operate_2f;
      sqrt_func_table[EVX$SQRT_SQRTTD].text		= "SQRTTD";
      sqrt_func_table[EVX$SQRT_SQRTTD].action		= DO_SQRTTD;
      
      sqrt_func_table[EVX$SQRT_SQRTTU].decode		= decode_operate_2f;
      sqrt_func_table[EVX$SQRT_SQRTTU].text		= "SQRTTU";
      sqrt_func_table[EVX$SQRT_SQRTTU].action		= DO_SQRTTU;
      
      sqrt_func_table[EVX$SQRT_SQRTTUC].decode		= decode_operate_2f;
      sqrt_func_table[EVX$SQRT_SQRTTUC].text		= "SQRTTUC";
      sqrt_func_table[EVX$SQRT_SQRTTUC].action		= DO_SQRTTUC;
      
      sqrt_func_table[EVX$SQRT_SQRTTUM].decode		= decode_operate_2f;
      sqrt_func_table[EVX$SQRT_SQRTTUM].text		= "SQRTTUM";
      sqrt_func_table[EVX$SQRT_SQRTTUM].action		= DO_SQRTTUM;
      
      sqrt_func_table[EVX$SQRT_SQRTTUD].decode		= decode_operate_2f;
      sqrt_func_table[EVX$SQRT_SQRTTUD].text		= "SQRTTUD";
      sqrt_func_table[EVX$SQRT_SQRTTUD].action		= DO_SQRTTUD;
      
      sqrt_func_table[EVX$SQRT_SQRTTSU].decode		= decode_operate_2f;
      sqrt_func_table[EVX$SQRT_SQRTTSU].text		= "SQRTTSU";
      sqrt_func_table[EVX$SQRT_SQRTTSU].action		= DO_SQRTTSU;
      
      sqrt_func_table[EVX$SQRT_SQRTTSUC].decode		= decode_operate_2f;
      sqrt_func_table[EVX$SQRT_SQRTTSUC].text		= "SQRTTSUC";
      sqrt_func_table[EVX$SQRT_SQRTTSUC].action		= DO_SQRTTSUC;
      
      sqrt_func_table[EVX$SQRT_SQRTTSUM].decode		= decode_operate_2f;
      sqrt_func_table[EVX$SQRT_SQRTTSUM].text		= "SQRTTSUM";
      sqrt_func_table[EVX$SQRT_SQRTTSUM].action		= DO_SQRTTSUM;
      
      sqrt_func_table[EVX$SQRT_SQRTTSUD].decode		= decode_operate_2f;
      sqrt_func_table[EVX$SQRT_SQRTTSUD].text		= "SQRTTSUD";
      sqrt_func_table[EVX$SQRT_SQRTTSUD].action		= DO_SQRTTSUD;
      
      sqrt_func_table[EVX$SQRT_SQRTTSUI].decode		= decode_operate_2f;
      sqrt_func_table[EVX$SQRT_SQRTTSUI].text		= "SQRTTSUI";
      sqrt_func_table[EVX$SQRT_SQRTTSUI].action		= DO_SQRTTSUI;
      
      sqrt_func_table[EVX$SQRT_SQRTTSUIC].decode		= decode_operate_2f;
      sqrt_func_table[EVX$SQRT_SQRTTSUIC].text		= "SQRTTSUIC";
      sqrt_func_table[EVX$SQRT_SQRTTSUIC].action		= DO_SQRTTSUIC;

      sqrt_func_table[EVX$SQRT_SQRTTSUIM].decode		= decode_operate_2f;
      sqrt_func_table[EVX$SQRT_SQRTTSUIM].text		= "SQRTTSUIM";
      sqrt_func_table[EVX$SQRT_SQRTTSUIM].action		= DO_SQRTTSUIM;

      sqrt_func_table[EVX$SQRT_SQRTTSUID].decode		= decode_operate_2f;
      sqrt_func_table[EVX$SQRT_SQRTTSUID].text		= "SQRTTSUID";
      sqrt_func_table[EVX$SQRT_SQRTTSUID].action		= DO_SQRTTSUID;

      sqrt_func_table[EVX$SQRT_SQRTTSUID].decode		= decode_operate_2f;
      sqrt_func_table[EVX$SQRT_SQRTTSUID].text		= "SQRTTSUID";
      sqrt_func_table[EVX$SQRT_SQRTTSUID].action		= DO_SQRTTSUID;


      /****/
      return(1);
}
