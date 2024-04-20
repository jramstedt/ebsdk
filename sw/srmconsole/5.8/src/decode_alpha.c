/*
 * decode_alpha.c -- ALPHA Instruction Printer
 */

#define DEBUG 0

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:instops_alpha.h"
#include "cp$inc:prototypes.h"

int ALPHA_InsDecode();			/* Decode an instruction. */
int ParseOpcode();
int ParseMemoryOperands();
int ParseBranchOperands();
int ParseOperateOperands();
int ParseFloatOperands();
int ParseSyncOperands();
int TranslateIntegerRegister();
int TranslateFloatRegister();
int TranslateImmediate();
int PrintIntegerRegister();
int PrintInstructionRegister();
int PrintBSRadr();
int PrintPDsymbol();

void AppendHex();			/* Append a hex string */
void AppendRad50();			/* Append a rad50 string */
void AppendChar();			/* Append a character */
void AppendString();			/* Append a string */

extern int get_memory();
extern int print_debug_flag;
#if MODULAR
extern int ovly_cpu;
extern int load_symbols_flag;
#endif

globalref struct alpha_s_opinfo ALPHA_OPINFO[];
globalref struct alpha_s_opinfo INST_INFO_OPS[];
globalref int INST_LENGTH_OPS;
globalref struct alpha_s_opinfo INST_INFO_JSR[];
globalref int INST_LENGTH_JSR;
globalref struct alpha_s_opinfo INST_INFO_ADDI[];
globalref int INST_LENGTH_ADDI;
globalref struct alpha_s_opinfo INST_INFO_LOGI[];
globalref int INST_LENGTH_LOGI;
globalref struct alpha_s_opinfo INST_INFO_SHFI[];
globalref int INST_LENGTH_SHFI;
globalref struct alpha_s_opinfo INST_INFO_MULI[];
globalref int INST_LENGTH_MULI;
globalref struct alpha_s_opinfo INST_INFO_SYNC[];
globalref int INST_LENGTH_SYNC;
globalref struct alpha_s_opinfo INST_INFO_FPAUX[];
globalref int INST_LENGTH_FPAUX;
globalref struct alpha_s_opinfo INST_INFO_FPDEC[];
globalref int INST_LENGTH_FPDEC;
globalref struct alpha_s_opinfo INST_INFO_FPIEEE[];
globalref int INST_LENGTH_FPIEEE;
globalref struct alpha_s_opinfo INST_INFO_PAL[];
globalref int INST_LENGTH_PAL;
globalref struct alpha_s_opinfo INST_INFO_PAL_HW_LD[];
globalref int INST_LENGTH_PAL_HW_LD;
globalref struct alpha_s_opinfo INST_INFO_PAL_HW_ST[];
globalref int INST_LENGTH_PAL_HW_ST;

struct alpha_s_opinfo *opinfo_ptr;

u_char *inst_text_address;
int inst_text_left;
int inst_text_size;
int save_inst_text_address;
int save_inst_text_left;
int ins_print_length;

#define ins_ra 0
#define ins_rb 1
#define ins_rc 2
#define ins_rx_not_used 31

int lib$ins_pc;
int lib$ins_ra_reg;
int lib$ins_rb_reg;
int lib$ins_rc_reg;
int lib$ins_r27_reg;
int lib$ins_bsr_adr;

/*
 * lib$ins_decode
 *
 * FUNCTIONAL DESCRIPTION:
 * 
 *   translates one EVAX machine instruction into the assembler string
 *   equivalent.
 * 
 * FORMAL PARAMETERS:
 * 
 *   StreamPntr	 - Address of the instruction stream pointer. The i-stream
 *		   pointer is used to fetch the machine instruction and is
 *		   then updated to point to the next instruction.
 *   OutBufDesc	 - Address of a string descriptor in which to store the
 *		   output assembler string.
 *   RetLen	 - Address of an integer to place the out assembler string
 *		   length.
 *   SymbolizeRtn - Always 0.
 *
 * RETURN VALUE:
 *   If successful, the formal parameters are all modified and SS$_NORMAL is
 *   returned. Otherwise, one of LIB$_NOINSTRAN (no translation), LIB$_NOSPACE
 *   (not enough space in output buffer), LIB$_INVREG (invalid register),
 *   LIB$_INVCLASS (invalid opcode class/format) or, LIB$DECODE_ERROR 
 *   (internal bug) is returned.
 * 
 * ASSUMPTIONS:
 *   this routine assumes access to the instruction stream. If this routine is
 *   called to translate an instruction in a running system, the caller must
 *   probe the i-stream in advance to verify access rights.
 * 
 */

int lib$ins_decode(u_int **StreamPntr, int *OutBufDesc[], int *RetLen, int SymbolizeRtn)
{
    int status;

    lib$ins_ra_reg = ins_rx_not_used;	/* mark not used */
    lib$ins_rb_reg = ins_rx_not_used;
    lib$ins_rc_reg = ins_rx_not_used;
    lib$ins_r27_reg = ins_rx_not_used;
    lib$ins_bsr_adr = 0;

    inst_text_address = OutBufDesc[1];
    inst_text_left = OutBufDesc[0];	/* Setup output buffer */
    inst_text_size = inst_text_left;

    save_inst_text_address = OutBufDesc[1];
    save_inst_text_left = OutBufDesc[0];

    lib$ins_pc = *StreamPntr;

    status = ALPHA_InsDecode(*StreamPntr);

    ins_print_length = inst_text_size - inst_text_left;
    *RetLen = ins_print_length;

    (*StreamPntr)++;

#if DEBUG
    printf("Status = %x, RetLen = %d\n", status, *RetLen);
    printf("Inst = \t%s\n", OutBufDesc[1]);
#endif

    return status;
}

/*
 * ALPHA_InsDecode (StreamPntr)
 *
 * FUNCTIONAL DESCRIPTION:
 * 
 *   translates one EVAX machine instruction into the assembler string
 *   equivalent.
 * 
 * FORMAL PARAMETERS:
 * 
 *   StreamPntr	 - Address of the instruction stream pointer. The i-stream
 *		   pointer is used to fetch the machine instruction and is
 *		   then updated to point to the next instruction.
 *
 * RETURN VALUE:
 *   If successful, the formal parameters are all modified and SS_NORMAL is
 *   returned. Otherwise, one of LIB$_NOINSTRAN (no translation), LIB$_NOSPACE
 *   (not enough space in output buffer), LIB$_INVREG (invalid register),
 *   LIB$_INVCLASS (invalid opcode class/format) or, LIB$DECODE_ERROR 
 *   (internal bug) is returned.
 * 
 * ASSUMPTIONS:
 *   this routine assumes access to the instruction stream. If this routine is
 *   called to translate an instruction in a running system, the caller must
 *   probe the i-stream in advance to verify access rights.
 * 
 */

int ALPHA_InsDecode(u_int *StreamPntr)
{
    u_int MachineInstr;
    int status, InstrFormat;

/* get the instruction from the instruction stream */

    MachineInstr = *StreamPntr;

#if DEBUG
    printf("Instruction = %08x\n", MachineInstr);
#endif

    if (MachineInstr == 0x5fff041f) {
	Append("FNOP        ");		/* do FNOP special */
	return SS_NORMAL;
    }

/*
 * Add the opcode string to the assembly string and find out
 * which instruction format it uses (could be one of EVX$K_imemory,
 * EVX$K_fmemory, EVX$K_ibranch, EVX$K_fbranch, EVX$K_operate, EVX$K_float2,
 * EVX$K_float3, EVX$K_jsr, EVX$K_sync0, EVX$K_sync1, EVX$K_pal or 
 * EVX$K_unknown)
 */

    InstrFormat = ParseOpcode(MachineInstr);


#if DEBUG
    printf("Format = %x\n", InstrFormat);
#endif
/*
 * Case on the instruction format, and for each type, pick up the
 * appropriate operands.
 */

    switch (InstrFormat) {

	case EVX$K_imemory: 
	case EVX$K_fmemory: 
	case EVX$K_jsr: 
	case EVX$K_ldst: 

	    status = ParseMemoryOperands(InstrFormat, MachineInstr);
	    break;

	case EVX$K_ibranch: 
	case EVX$K_fbranch: 

	    status = ParseBranchOperands(InstrFormat, MachineInstr);
	    break;

	case EVX$K_operate: 

	    status = ParseOperateOperands(MachineInstr);
	    break;

	case EVX$K_float2: 
	case EVX$K_float3: 

	    status = ParseFloatOperands(InstrFormat, MachineInstr);
	    break;

	case EVX$K_pal: 
	case EVX$K_hwrei: 
	case EVX$K_sync0: 		/* Instructions with no operands */

	    status = SS_NORMAL;
	    break;

	case EVX$K_sync1: 		/* One operand sync instructions */

	    status = ParseSyncOperands(MachineInstr);
	    break;

	case EVX$K_mtmf: 
	    status = ParseMtmfOperands(MachineInstr);
	    break;

	default: 
	    status = LIB$_NOINSTRAN;
    }

    return status;
}					/*  Of routine alpha_ins_decode */

/*
 * ParseOpcode - extracts the opcode from an EVAX instruction
 *
 * FUNCTIONAL DESCRIPTION:
 *
 *   Extracts the EVAX opcode and possible function fields (for OPERATE,
 *   FLOAT and PAL instructions), puts the symbol string name in the output
 *   buffer, updates the output buffer return length and returns the opcode
 *   class (one of EVX$K_imemory, EVX$K_fmemory, EVX$K_ibranch, EVX$K_fbranch,
 *   EVX$K_operate, EVX$K_float2, EVX$K_float3, EVX$K_jsr, EVX$K_sync,
 *   EVX$K_pal or EVX$K_unknown)
 *
 * FORMAL PARAMETERS:
 *
 *   MachineInstr	- an unsigned integer where the lower 32 bits
 *			  contain the EVAX machine instruction.
 *
 * RETURN VALUE:
 *
 *   Returns the opcode class as described above.
 *
 */

int ParseOpcode(u_int MInstr)
{
    int Ops, Opcode;
    union alpha_instruction MachineInstr;

    MachineInstr.inst = MInstr;
/*
 * determine the opcode, opcode class, and set the opcode string
 */

    Opcode = MachineInstr.pal_format.opcode;

#if DEBUG
    printf("Opcode = %x\n", Opcode);
#endif

    Ops = ParseOps(Opcode, &INST_INFO_OPS[0], INST_LENGTH_OPS);

/*
 * Case on the op type returned, and for each type, do any additional
 * decoding if required.
 */

#if DEBUG
    printf("Ops = %x\n", Ops);
#endif

    switch (Ops) {

	case EVX$K_pal: 
	    Opcode = MachineInstr.pal_format.function;
	    Ops = ParseOps(Opcode, &INST_INFO_PAL[0], INST_LENGTH_PAL);
	    break;

	case EVX$K_jsr: 
	    Opcode = (MachineInstr.memory_format.disp >> 14) & 3;
	    Ops = ParseOps(Opcode, &INST_INFO_JSR[0], INST_LENGTH_JSR);
	    break;

	case EVX$K_addi: 
	    Opcode = MachineInstr.operate0_format.func;
	    Ops = ParseOps(Opcode, &INST_INFO_ADDI[0], INST_LENGTH_ADDI);
	    break;

	case EVX$K_logi: 
	    Opcode = MachineInstr.operate0_format.func;
	    Ops = ParseOps(Opcode, &INST_INFO_LOGI[0], INST_LENGTH_LOGI);
	    break;

	case EVX$K_shfi: 
	    Opcode = MachineInstr.operate0_format.func;
	    Ops = ParseOps(Opcode, &INST_INFO_SHFI[0], INST_LENGTH_SHFI);
	    break;

	case EVX$K_muli: 
	    Opcode = MachineInstr.operate0_format.func;
	    Ops = ParseOps(Opcode, &INST_INFO_MULI[0], INST_LENGTH_MULI);
	    break;

	case EVX$K_sync: 
	    Opcode = MachineInstr.sync_format.function;
	    Ops = ParseOps(Opcode, &INST_INFO_SYNC[0], INST_LENGTH_SYNC);
	    break;

	case EVX$K_fpaux: 
	    Opcode = MachineInstr.foperate_format.func;
	    Ops = ParseOps(Opcode, &INST_INFO_FPAUX[0], INST_LENGTH_FPAUX);
	    break;

	case EVX$K_fpdec: 
	    Opcode = MachineInstr.foperate_format.func;
	    Ops = ParseOps(Opcode, &INST_INFO_FPDEC[0], INST_LENGTH_FPDEC);
	    break;

	case EVX$K_fpieee: 
	    Opcode = MachineInstr.foperate_format.func;
	    Ops = ParseOps(Opcode, &INST_INFO_FPIEEE[0], INST_LENGTH_FPIEEE);
	    break;

	case EVX$K_mtmf: 
	case EVX$K_hwrei: 
	    break;

	case EVX$K_ldst: 
	    Opcode = MachineInstr.hw_ldst_format.type;
	    if (MachineInstr.pal_format.opcode == EVX$PAL_HW_LD)
		Ops = ParseOps(Opcode, &INST_INFO_PAL_HW_LD[0],
					INST_LENGTH_PAL_HW_LD);
	    else 		
		Ops = ParseOps(Opcode, &INST_INFO_PAL_HW_ST[0],
					INST_LENGTH_PAL_HW_ST);
	    break;
    }
    if (Ops == EVX$K_unknown)
	return EVX$K_unknown;

    AppendRad50(OP_CH_SIZE / 3, &(*opinfo_ptr).rad);

    return Ops;
}					/* Of routine ParseOpcode */

int ParseOps(int code, int table, int length)
{
    int i;

#if DEBUG
    printf("ParseOps; code = %x, table = %x, length = %x\n", code, table,
      length);
#endif

    opinfo_ptr = table;

    for (i = 0; i < length; i++) {
	if (code == opinfo_ptr->opcode) {
	    return opinfo_ptr->format;
	    break;
	}
	opinfo_ptr++;
    }

    return EVX$K_unknown;
}					/* Of routine ParseOpcode */

/*
 * ParseMemoryOperands - fetches and symbolizes EVAX memory format operands
 *
 *     FUNCTIONAL DESCRIPTION:
 *
 *	fetches the EVAX memory format instruction operands and appends
 *	their symbolic format to the output buffer. The return length is
 *	updated to contain the length of the final string.
 *
 * FORMAL PARAMETERS:
 *
 *	OpcodeClass	- should be one of EVX$K_imemory or EVX$K_fmemory.
 *	MachineInstr	- an integer where the lower 32 bits are the EVAX
 *			  machine instruction.
 *	
 * RETURN VALUE:
 *
 *	returns SS_NORMAL if successful, LIB$_INVCLASS if the wrong memory
 *	class is passed. Also, calls to other routines may return bad status
 *	(other than SS_NORMAL) which is returned.
 * 
 */

int ParseMemoryOperands(int OpcodeClass, u_int MInstr)
{
    int status, Displacement;
    union alpha_instruction MachineInstr;

    MachineInstr.inst = MInstr;

#if DEBUG
    printf("ParseMemoryOperand, OC = %x, MI = %x\n", OpcodeClass,
      MachineInstr);
#endif

/* Add the Ra/Fa operand and a comma */

    if (OpcodeClass == EVX$K_fmemory) {

	status = TranslateFloatRegister(MachineInstr.memory_format.Ra);

	if (status != SS_NORMAL)
	    return status;

    } else if ((OpcodeClass == EVX$K_imemory) || (OpcodeClass == EVX$K_jsr) ||
	       (OpcodeClass == EVX$K_ldst)) {

	if (strncmp(save_inst_text_address, "JSR", 3) == 0)
	    lib$ins_r27_reg = 27;

	status = TranslateIntegerRegister(MachineInstr.memory_format.Ra, ins_ra);
	if (status != SS_NORMAL)
		return status;

    } else
	return LIB$_INVCLASS;

    AppendChar(',');

    status = 0;

    if (OpcodeClass == EVX$K_jsr)
	Displacement = 0;
    else
	Displacement = MachineInstr.memory_format.disp;

/* translate Disp(Rx) */

    if (Displacement != 0) {

    	if (OpcodeClass == EVX$K_ldst)
	    status = TranslateImmediate(MachineInstr.hw_ldst_format.disp,
	  	EIF$ldst_imm_width);
	else
  	    status = TranslateImmediate(MachineInstr.memory_format.disp,
	  	EIF$Mem_imm_width);

	if (status != SS_NORMAL)
	    return status;
    }

    AppendChar('(');

    status = TranslateIntegerRegister(MachineInstr.memory_format.Rb, ins_rb);

    if (status != SS_NORMAL)
	return status;

    AppendChar(')');

    return SS_NORMAL;
}

/*
 * ParseBranchOperands - fetches and symbolizes EVAX branch format operands
 *
 * FUNCTIONAL DESCRIPTION:
 *
 *	Fetches the EVAX branch format instruction operands and appends
 *	their symbolic format to the output buffer. The return length is
 *	updated to contain the length of the final string.
 *
 * FORMAL PARAMETERS:
 *
 *	OpcodeClass	- should be one of EVX$K_ibranch or EVX$K_fbranch.
 *	MachineInstr	- an integer where the lower 32 bits are the EVAX
 *			  machine instruction.
 *	
 * RETURN VALUE:
 *
 *	returns SS_NORMAL if successful, LIB$_INVCLASS if an invalid branch
 *	class is passed. Also, calls to other routines may return bad status
 *	(other than SS_NORMAL) which is returned.
 * 
 */

int ParseBranchOperands(int OpcodeClass, u_int MInstr)
{
    int status;
    union alpha_instruction MachineInstr;

    MachineInstr.inst = MInstr;

#if DEBUG
    printf("ParseBranchOperand, OC = %x, MI = %x\n", OpcodeClass,
      MachineInstr);
#endif

/* Add the Ra/Fa operand and a comma */

    if (OpcodeClass == EVX$K_fbranch) {

	status = TranslateFloatRegister(MachineInstr.branch_format.Ra);

	if (status != SS_NORMAL)
	    return status;

    } else if (OpcodeClass == EVX$K_ibranch) {

	if (strncmp(save_inst_text_address, "BSR", 3) == 0) {
	    lib$ins_r27_reg = 27;
	    lib$ins_bsr_adr = MachineInstr.branch_format.disp;
	    if (lib$ins_bsr_adr & 0x100000)
		lib$ins_bsr_adr |= 0xffe00000;
	}
	
	status = TranslateIntegerRegister(MachineInstr.branch_format.Ra, ins_ra);
	if (status != SS_NORMAL)
		return status;

    } else
	return LIB$_INVCLASS;

    AppendChar(',');

    status = 0;

    status =
      TranslateImmediate(MachineInstr.branch_format.disp, EIF$Brnch_imm_width);

    if (status != SS_NORMAL)
	return status;

    return SS_NORMAL;
}					/* Of routine ParsebranchOperands */

/*
 * ParseOperateOperands - fetches and symbolizes EVAX operate format operands
 *
 * FUNCTIONAL DESCRIPTION:
 *
 *	fetches the EVAX operate format instruction operands and appends
 *	their symbolic format to the output buffer. The return length is
 *	updated to contain the length of the final string.
 *
 * FORMAL PARAMETERS:
 *
 *	MachineInstr	- an integer where the lower 32 bits are the EVAX
 *			  machine instruction.
 *	
 * RETURN VALUE:
 *
 *	returns SS_NORMAL if successful. Also, calls to other routines may
 * 	return bad status (other than SS_NORMAL) which is returned.
 * 
 */

int ParseOperateOperands(u_int MInstr)
{
    int status;
    union alpha_instruction MachineInstr;

    MachineInstr.inst = MInstr;

#if DEBUG
    printf("ParseOperateOperand, MI = %x\n", MachineInstr);
#endif

    if (strncmp(save_inst_text_address, "BIS", 3) != 0)
	goto ParseOperateOperands_ra;

    if (MachineInstr.operate0_format.Ra != EVX$K_R31)
	goto ParseOperateOperands_ra;

    if (MachineInstr.operate0_format.litc == 1) {
	inst_text_address = save_inst_text_address;
	inst_text_left = save_inst_text_left;
	inst_text_size = save_inst_text_left;
	Append("MOV         ");
	goto ParseOperateOperands_rb;
    }

    if ((MachineInstr.operate0_format.Rb == EVX$K_R31) && 
	(MachineInstr.operate0_format.Rc == EVX$K_R31))  {
	inst_text_address = save_inst_text_address;
	inst_text_left = save_inst_text_left;
	inst_text_size = save_inst_text_left;
	Append("NOP         ");
	goto ParseOperateOperands_rd;
    }

    if (MachineInstr.operate0_format.Rb == EVX$K_R31) {
	inst_text_address = save_inst_text_address;
	inst_text_left = save_inst_text_left;
	inst_text_size = save_inst_text_left;
	Append("CLR         ");
	goto ParseOperateOperands_rc;
    }

    if (MachineInstr.operate0_format.Rc == EVX$K_R31) {
	inst_text_address = save_inst_text_address;
	inst_text_left = save_inst_text_left;
	inst_text_size = save_inst_text_left;
	Append("NOP         ");
	goto ParseOperateOperands_rd;
    }

    inst_text_address = save_inst_text_address;
    inst_text_left = save_inst_text_left;
    inst_text_size = save_inst_text_left;
    Append("MOV         ");
    goto ParseOperateOperands_rb;

ParseOperateOperands_ra:

/* Add the Ra, operand and then a comma */

    status = TranslateIntegerRegister(MachineInstr.operate0_format.Ra, ins_ra);

    if (status != SS_NORMAL)
	return status;

    AppendChar(',');

ParseOperateOperands_rb:

/* Is this the second operand a literal or register? */

    if (MachineInstr.operate0_format.litc == 1) {

/* Literal mode, add '#' and the value */

	AppendChar('#');

	status = TranslateImmediate(MachineInstr.operate1_format.lit,
	  EIF$Opr_imm_width);

	if (status != SS_NORMAL)
	    return status;

    } else {

	/* another register */

	status = TranslateIntegerRegister(MachineInstr.operate0_format.Rb,ins_rb);

	if (status != SS_NORMAL)
	    return status;
    }

    AppendChar(',');

ParseOperateOperands_rc:

/* The last operand is Rc */

    status = TranslateIntegerRegister(MachineInstr.operate0_format.Rc, ins_rc);
    if (status != SS_NORMAL)
	return status;

ParseOperateOperands_rd:

    return SS_NORMAL;
}					/* Of routine ParseOperateOperands */

/*
 * ParseFloatOperands - fetches and symbolizes EVAX float format operands
 *
 * FUNCTIONAL DESCRIPTION:
 *
 *	fetches the EVAX float format instruction operands and appends
 *	their symbolic format to the output buffer. The return length is
 *	updated to contain the length of the final string.
 *
 * FORMAL PARAMETERS:
 *
 *	OpcodeClass	- one of EVX$K_float2 or EVX$K_float3 (2 or 3 operand
 *			  float operations)
 *	MachineInstr	- an integer where the lower 32 bits are the EVAX
 *			  machine instruction.
 *	
 * RETURN VALUE:
 *
 *	returns SS_NORMAL if successful, LIB$_INVCLASS if an invalid float
 *	class is passed. Also, calls to other routines may return bad status
 *	(other than SS_NORMAL) which is returned.
 * 
 */

int ParseFloatOperands(int OpcodeClass, u_int MInstr)
{
    int status;
    union alpha_instruction MachineInstr;

    MachineInstr.inst = MInstr;

#if DEBUG
    printf("ParseFloatOperand, OC = %x, MI = %x\n", OpcodeClass, MachineInstr);
#endif

    if (OpcodeClass == EVX$K_float3) {

/* Add the Fa operand and a comma */

	status = TranslateFloatRegister(MachineInstr.foperate_format.Fa);

	if (status != SS_NORMAL)
	    return status;

	AppendChar(',');

    } else if (OpcodeClass != EVX$K_float2)
	return LIB$_INVCLASS;

/* Add the Fb operand and a comma */

    status = TranslateFloatRegister(MachineInstr.foperate_format.Fb);

    if (status != SS_NORMAL)
	return status;

    AppendChar(',');

/* Add the Fc operand */

    status = TranslateFloatRegister(MachineInstr.foperate_format.Fc);

    if (status != SS_NORMAL)
	return status;

    return SS_NORMAL;
}

/*
 * ParseSyncOperands - fetches and symbolizes EVAX sync operand
 *
 * FUNCTIONAL DESCRIPTION:
 *
 *	fetches the EVAX sync operand and appends their symbolic format to
 *	the output buffer The return length is updated to contain the
 *	length of the final string
 *
 * FORMAL PARAMETERS:
 *
 *	MachineInstr	- an integer where the lower 32 bits are the EVAX
 *			  machine instruction
 *
 * RETURN VALUE:
 *
 *	returns SS_NORMAL if successful, or a failure code returned by
 *	called routine
 *
 */

int ParseSyncOperands(u_int MInstr)
{
    int status;
    union alpha_instruction MachineInstr;

    MachineInstr.inst = MInstr;

#if DEBUG
    printf("ParseSyncOperand, MI = %x\n", MachineInstr);
#endif

/* FETCH and FETCH_M operand looks like 0(Rb) */

    if ((MachineInstr.sync_format.function == EVX$SYNC_FETCH) ||
      (MachineInstr.sync_format.function == EVX$SYNC_FETCH_M)) {

	AppendChar('0');

	AppendChar('(');

/* Add the register b operand */

	status = TranslateIntegerRegister(MachineInstr.sync_format.Rb, ins_rb);

	AppendChar(')');

    } else

/* Add the register a operand */

	status = TranslateIntegerRegister(MachineInstr.sync_format.Ra, ins_ra);

/* return the status */

    return status;			/* Of routine ParseSyncOperands */
}

/*
 * ParseMtmfOperands - fetches and symbolizes EVAX mtmf operands
 *
 * FUNCTIONAL DESCRIPTION:
 *
 *	fetches the EVAX mtmf operands and appends their symbolic format to
 *	the output buffer The return length is updated to contain the
 *	length of the final string
 *
 * FORMAL PARAMETERS:
 *
 *	MachineInstr	- an integer where the lower 32 bits are the EVAX
 *			  machine instruction
 *
 * RETURN VALUE:
 *
 *	returns SS_NORMAL if successful, or a failure code returned by
 *	called routine
 *
 */

int ParseMtmfOperands(u_int MInstr)
{
    int status;
    union alpha_instruction MachineInstr;

    MachineInstr.inst = MInstr;

#if DEBUG
    printf("ParseMtmfOperand, MI = %x\n", MachineInstr);
#endif


/* Add the register b operand */

    status = TranslateIntegerRegister(MachineInstr.hw_mtmf_format.Rb, ins_rb);

    AppendChar(',');



    if (MachineInstr.hw_mtmf_format.pal)
	Append("PT");

    if ((MachineInstr.hw_mtmf_format.pal && MachineInstr.hw_mtmf_format.abx) ||
	(MachineInstr.hw_mtmf_format.pal && MachineInstr.hw_mtmf_format.ibx))
    {
    	if (MachineInstr.hw_mtmf_format.index < 10)
	    AppendChar('0' + MachineInstr.hw_mtmf_format.index);
	else
	{
	    AppendChar('0' + (MachineInstr.hw_mtmf_format.index / 10));
	    AppendChar('0' + (MachineInstr.hw_mtmf_format.index - 
			     (MachineInstr.hw_mtmf_format.index / 10) * 10));
	}
	AppendChar('+');
    }

    if (MachineInstr.hw_mtmf_format.abx)
	Append("ABOX");

    if (MachineInstr.hw_mtmf_format.abx && MachineInstr.hw_mtmf_format.ibx)
    {
    	if (MachineInstr.hw_mtmf_format.index < 10)
	    AppendChar('0' + MachineInstr.hw_mtmf_format.index);
	else
	{
	    AppendChar('0' + (MachineInstr.hw_mtmf_format.index / 10));
	    AppendChar('0' + (MachineInstr.hw_mtmf_format.index - 
			     (MachineInstr.hw_mtmf_format.index / 10) * 10));
	}
	AppendChar('+');
    }

    if (MachineInstr.hw_mtmf_format.ibx)
	Append("IBOX");

    if (MachineInstr.hw_mtmf_format.index < 10)
	AppendChar('0' + MachineInstr.hw_mtmf_format.index);
    else
    {
	AppendChar('0' + (MachineInstr.hw_mtmf_format.index / 10));
	AppendChar('0' + (MachineInstr.hw_mtmf_format.index - 
			 (MachineInstr.hw_mtmf_format.index / 10) * 10));
    }

/* return the status */

    return status;			/* Of routine ParseMtmfOperands */
}

/*
 * TranslateIntegerRegister - translates a register number to its symbol
 *
 * FUNCTIONAL DESCRIPTION:
 *
 *	Translate an EVAX integer register number into it's symbolic name
 *	and appends the string to the end of the output buffer It sets the
 *	return length to output buffer's final length
 *
 * FORMAL PARAMETERS:
 *
 *	RegisterNumber	- The machines register number
 *	Operand		- Instruction operand
 *
 * RETURN VALUE:
 *
 *	SS_NORMAL if succesful, LIB$_NOSPACE if the buffer is not big
 *	enough and LIB$_INVREG if an invalid register was specified
 */

int TranslateIntegerRegister(int RegisterNumber, int Operand)
{
/* save operand register number */

    switch (Operand) {
	case ins_ra:
	    lib$ins_ra_reg = RegisterNumber;
	    break;
	case ins_rb:
	    lib$ins_rb_reg = RegisterNumber;
	    break;
	case ins_rc:
	    lib$ins_rc_reg = RegisterNumber;
	    break;
    }

     return PrintIntegerRegister(RegisterNumber);

}					/* TranslateIntegerRegister */

int PrintIntegerRegister(int RegisterNumber)
{

/* case on register number */

    switch (RegisterNumber) {

	case EVX$K_R0: 
	case EVX$K_R1: 
	case EVX$K_R2: 
	case EVX$K_R3: 
	case EVX$K_R4: 
	case EVX$K_R5: 
	case EVX$K_R6: 
	case EVX$K_R7: 
	case EVX$K_R8: 
	case EVX$K_R9: 

/* append the register name */

	    AppendChar('R');
	    AppendChar('0' + RegisterNumber);
	    break;

	case EVX$K_R10: 
	case EVX$K_R11: 
	case EVX$K_R12: 
	case EVX$K_R13: 
	case EVX$K_R14: 
	case EVX$K_R15: 
	case EVX$K_R16: 
	case EVX$K_R17: 
	case EVX$K_R18: 
	case EVX$K_R19: 
	case EVX$K_R20: 
	case EVX$K_R21: 
	case EVX$K_R22: 
	case EVX$K_R23: 
	case EVX$K_R24: 
	case EVX$K_R25: 
	case EVX$K_R26: 
	case EVX$K_R27: 
	case EVX$K_R28: 
	case EVX$K_R29: 

/* append the register name */

	    AppendChar('R');
	    AppendChar('0' + (RegisterNumber / 10));
	    AppendChar('0' + (RegisterNumber - (RegisterNumber / 10) * 10));
	    break;

	case EVX$K_R30: 

/* stack pointer */

	    Append("SP");
	    break;

	case EVX$K_R31: 

/* zero register */

/*	    Append("RZ");	*/
	    Append("R31");
	    break;

	default: 
	    return LIB$_INVREG;
    }
    return SS_NORMAL;
}					/* PrintIntegerRegister */

/*
 * TranslateFloatRegister - translates a float register number to its symbol
 *
 * FUNCTIONAL DESCRIPTION:
 *
 *	Translates an EVAX floating point register number into it's
 *	symbolic name and appends the string to the end of the output
 *	buffer It sets the return length to the output buffers final
 *	length
 *
 * FORMAL PARAMETERS:
 *
 *	RegisterNumber	- The machines register number
 *
 * RETURN VALUE:
 *
 *	SS_NORMAL if succesful, LIB$_INVREG if an invalid register was 
 *	specified.
 */

int TranslateFloatRegister(int RegisterNumber)
{
/* case on register number */

    switch (RegisterNumber) {

	case EVX$K_F0: 
	case EVX$K_F1: 
	case EVX$K_F2: 
	case EVX$K_F3: 
	case EVX$K_F4: 
	case EVX$K_F5: 
	case EVX$K_F6: 
	case EVX$K_F7: 
	case EVX$K_F8: 
	case EVX$K_F9: 

/* append the register name */

	    AppendChar('F');
	    AppendChar('0' + RegisterNumber);
	    break;

	case EVX$K_F10: 
	case EVX$K_F11: 
	case EVX$K_F12: 
	case EVX$K_F13: 
	case EVX$K_F14: 
	case EVX$K_F15: 
	case EVX$K_F16: 
	case EVX$K_F17: 
	case EVX$K_F18: 
	case EVX$K_F19: 
	case EVX$K_F20: 
	case EVX$K_F21: 
	case EVX$K_F22: 
	case EVX$K_F23: 
	case EVX$K_F24: 
	case EVX$K_F25: 
	case EVX$K_F26: 
	case EVX$K_F27: 
	case EVX$K_F28: 
	case EVX$K_F29: 
	case EVX$K_F30: 
	case EVX$K_F31: 

/* append the register name */

	    AppendChar('F');
	    AppendChar('0' + (RegisterNumber / 10));
	    AppendChar('0' + (RegisterNumber - (RegisterNumber / 10) * 10));
	    break;

	default: 
	    return LIB$_INVREG;
    }

    return SS_NORMAL;
}					/* Of routine TranslateFloatRegister */

/*
 * TranslateImmediate - translates a sign extended immediate to ascii
 *
 * FUNCTIONAL DESCRIPTION:
 *
 *	Translates a sign extended immediate into it's ASCII hexadecimal
 *	equivalent Append the string to the output buffer and update the
 *	return length to the output buffers final length
 *
 * FORMAL PARAMETERS:
 *
 *	Immediate	- The sign extended value to translate
 *	NumChars	- The number of digit characters to produce
 *
 * RETURN VALUE:
 *	SS_NORMAL when succesful, LIB$_DECODE_ERROR for an internal bug
 */

int TranslateImmediate(int Immediate, int NumChars)
{
    int CharCount;
    char ch;

/* Put the '#X' chars in */

/*  append_char('#'); */
/*  append_char('X'); */

    AppendHex(Immediate, NumChars, 1);

    return SS_NORMAL;
}					/* Of routine TranslateImmediate */

/*
 * AppendHex(value, width, negate)
 *
 *	This routine appends a given hex value to the current output
 *	Buffer.
 *
 * Inputs:
 *
 *	Value = absolute value
 *	width = number of bytes to display
 *	negate = if 1, print negatives
 *
 * Outputs:
 *
 *	The hex value is appended.
 */

void AppendHex(u_int value, int width, int negate)
{
    int cnt;
    u_int n;
    char prbuf[11];
    char *cp;

    n = value;
    cp = prbuf;

    if (negate) {
	if ((int) n < 0) {
	    Append("-");
	    n = (unsigned) (-(int) n);
	}
    }

    cnt = 0;

    do {
	cnt++;
	*cp++ = "0123456789ABCDEF"[n % 16];
	n /= 16;
    } while (n);

    if (width != 0) {
	cnt = width - cnt;
	while (cnt-- > 0)
	    Append("0");
    }

    do {
	cp--;
	AppendString(1, cp);
    } while (cp > prbuf);
}

/*
 * AppendRad50(nwords, words)
 *
 *	This routine converts a series of rad50 words to ascii and
 *	Appends it to the current output buffer.
 *
 *	RAD50 character set
 *	   26 letters, A-Z
 *	   10 digits,  0-9
 *	   3 special,  $ % .
 *	If char is ., replace with underscore
 *
 * Inputs:
 *
 *	Nwords = number of words to convert
 *	Words = address of words to convert
 *
 * Outputs:
 *
 *	The string is appended, without any padding or fill.
 */

void AppendRad50(int nwords, u_short *words)
{
    int i, j;
    u_short number;
    u_char c[3];
    u_char c1;

    for (i = 0; i < nwords; i++) {
	number = words[i];

	for (j = 2; j >= 0; j--) {
	    c[j] = number % 40;
	    number /= 40;
	}
	for (j = 0; j <= 2; j++) {
	    c1 = c[j];

	    if (c1 == 0)
		c[j] = ' ';
	    else if (c1 <= 26)
		c[j] = c[j] + 'A' - 1;
	    else if (c1 == 27)
		c[j] = '$';
	    else if (c1 > 27)
		c[j] = c[j] + '.' - 28;

	    if (c[j] == '.')
		c[j] = '_';

	    AppendString(1, &c[j]);
	}
    }
}

/*
 * AppendString(length, string)
 *
 *	Append a string to the current output buffer.
 *
 * Inputs:
 *
 *	Length = length of string
 *	String = address of string
 *
 *	Inst_text_address = address of next available byte in user buffer
 *	Inst_text_left = number of bytes left in user buffer
 *
 * Outputs:
 *
 *	Inst_text_address, inst_text_left are updated.
 */

void AppendString(int length, char *string)
{
    if (length == 0)
	length = strlen(string);

    if (inst_text_left >= length) {
	strncpy(inst_text_address, string, length);
	inst_text_address += length;
	inst_text_left -= length;
	inst_text_address[1] = 0;
    }
}

/*
 * AppendChar(ch)
 *
 *	Append a character to the current output buffer.
 *
 * Inputs:
 *
 *	ch = character
 *
 *	Inst_text_address = address of next available byte in user buffer
 *	Inst_text_left = number of bytes left in user buffer
 *
 * Outputs:
 *
 *	Inst_text_address, inst_text_left are updated.
 */

void AppendChar(char ch)
{
    char c1;

    c1 = ch;

    AppendString(1, &c1);
}

/*
 * lib$ins_registers
 *
 * FUNCTIONAL DESCRIPTION:
 * 
 *   prints the registers for one EVAX machine instruction
 * 
 * FORMAL PARAMETERS:
 * 
 *   Context	 - Address of the register context storage
 *   OutBufDesc	 - Address of a string descriptor in which to store the
 *		   output assembler string.
 *   RetLen	 - Address of an integer to place the out assembler string
 *		   length.
 *
 * RETURN VALUE:
 *   If successful, SS$_NORMAL is returned.
 *   Otherwise, one of LIB$_NOINSTRAN (translation off),
 *   LIB$_NOSPACE (not enough space in output buffer),
 *   or LIB$DECODE_ERROR (internal bug) is returned.
 * 
 * ASSUMPTIONS:
 *   this routine assumes the registers to be printed has been setup by the
 *   call to lib$ins_decode.
 */

int lib$ins_registers(int Context, int *OutBufDesc[], int *RetLen)
{
    inst_text_address = OutBufDesc[1];
    inst_text_left = OutBufDesc[0];	/* Setup output buffer */
    inst_text_size = inst_text_left;

    while (ins_print_length++ < 25) 
	Append(" ");

    Append(" ");
    if (lib$ins_r27_reg != 27)
	PrintInstructionRegister(Context, lib$ins_ra_reg);

    PrintInstructionRegister(Context, lib$ins_rb_reg);

    PrintInstructionRegister(Context, lib$ins_rc_reg);

    if (lib$ins_r27_reg == 27)
	PrintInstructionRegister(Context, lib$ins_r27_reg);

    if (lib$ins_bsr_adr != 0)
	PrintBSRadr();

    if (lib$ins_r27_reg == 27)
	PrintPDsymbol(Context);

    *RetLen = inst_text_size - inst_text_left;
    return SS_NORMAL;
}

void PrintInstructionRegister(int *Context[], int RegisterNumber)
{
    int data;

    if (RegisterNumber == ins_rx_not_used)
	return;

    Append(" ");
    if ((RegisterNumber < 10) || (RegisterNumber == 30))
	Append(" ");
    PrintIntegerRegister(RegisterNumber);
    Append("/ ");

    data = *(*Context + 1 + (RegisterNumber * 2));
    AppendHex(data, 8, 0);
    AppendChar(' ');

    data = *(*Context + (RegisterNumber * 2));
    AppendHex(data, 8, 0);
}

void PrintBSRadr()
{
    int data;

    data = lib$ins_pc + 4 + (lib$ins_bsr_adr * 4);

    Append(" ");
    AppendHex(data, 8, 0);
}

void PrintPDsymbol(int *Context[])
{
#if SYMBOLS
    int data;

    data = *(*Context + (27 * 2));
    if (!data)
	return;
    data = sym_table_lookup(data);

    if (data != 0) {
	Append(" ");
	AppendString(0, data);
    }
#endif
}

/*
 * lib$sys_lookup
 *
 * FUNCTIONAL DESCRIPTION:
 * 
 *   Looks up a given string to find its address equivalant.
 * 
 * FORMAL PARAMETERS:
 * 
 *   SymPntr	 - Address of the symbol string.
 *   Term 	 - Address of a location to store the termination char.
 *
 * RETURN VALUE:
 *   If successful, the the value of the symbol is returned.
 *   Otherwise, a 0 is returned.
 * 
 */

int lib$sym_lookup(char *SymPntr, int *Address)
{
    int status;
    int length;
    int symbol;
    char *sptr;
    char *ptr;
    char c;

	*Address = 0;

#if !SYMBOLS
	return 0;
#endif
#if SYMBOLS
	length = strlen(SymPntr);
	sptr = strcspn(SymPntr, "/!\"=;");

	if (sptr == length)
	    return SymPntr;

	ptr = (int)SymPntr + (int)sptr;
	c = *ptr;
	*ptr = 0;
	status = sym_lookup (SymPntr, &symbol);
	*ptr = c;

	if (status == 0)
	    *Address = symbol;

	return ptr;
#endif
}

/*
 * lib$ovly_display
 *
 * FUNCTIONAL DESCRIPTION:
 * 
 *   Displays the current resident overlays.
 * 
 * FORMAL PARAMETERS:
 * 
 *   None.
 *
 * RETURN VALUE:
 *
 *   None.
 * 
 */

void lib$ovly_display()
{
#if MODULAR
    ovly_identify();
#endif
}

/*
 * lib$ovly_pc
 *
 * FUNCTIONAL DESCRIPTION:
 * 
 *   Displays the overlay name and offset.
 * 
 * FORMAL PARAMETERS:
 * 
 *   address 	 - address to be looked up.
 *   OutBufDesc	 - Address of a string descriptor in which to store the
 *		   output string.
 *
 * RETURN VALUE:
 *
 *   If successful, SS$_NORMAL is returned.
 *   Otherwise, 0 is returned.
 * 
 */

int lib$ovly_pc(int Address, int *OutBufDesc[])
{
    int adr, data;
    char *name;

    OutBufDesc[0] = 0;

#if MODULAR

    adr = ovly_find_address(Address, &name);
    if (!adr)
	return 0;

    data = Address - adr;

    sprintf(OutBufDesc[1], "%s+%x ", name, data);
    OutBufDesc[0] = strlen(OutBufDesc[1]);

    return SS_NORMAL;
#endif
    return 0;
}

/*
 * lib$sym_pc
 *
 * FUNCTIONAL DESCRIPTION:
 * 
 *   Displays the symbol name and offset.
 * 
 * FORMAL PARAMETERS:
 * 
 *   address 	 - address to be looked up.
 *   OutBufDesc	 - Address of a string descriptor in which to store the
 *		   output string.
 *
 * RETURN VALUE:
 *
 *   If successful, SS$_NORMAL is returned.
 *   Otherwise, 0 is returned.
 * 
 */

int lib$sym_pc(int Address, int *OutBufDesc[])
{
    int adr, diff;
    char *name;

    OutBufDesc[0] = 0;

#if SYMBOLS

    adr = find_pd_symbol(Address, &name);
    if (!adr)
	return 0;

    diff = Address - adr;

    sprintf(OutBufDesc[1], "%s+%x ", name, diff);
    OutBufDesc[0] = strlen(OutBufDesc[1]);

    return SS_NORMAL;
#endif
    return 0;
}

/*
 * lib$walk_stack
 *
 * FUNCTIONAL DESCRIPTION:
 * 
 *   Displays the stack
 * 
 * FORMAL PARAMETERS:
 * 
 *   fp		 - frame pointer
 *   sp		 - stack pointer
 *   pc		 - program counter
 *   acp	 - alpha context pointer
 *   pflag	 - 0 = pc & pd, 1 = all
 *
 * RETURN VALUE:
 *
 *   None.
 * 
 */

void lib$walk_stack(int *fp, int *sp, int pc, int *acp[], int pflag)
{
    struct ALPHA_CTX ctx, *acx;
    int pdebug_save;

#if SYMBOLS

    pdebug_save = print_debug_flag;
    print_debug_flag = 1;
    show_process_stack_usage( sp );
    print_debug_flag = pdebug_save;

    acx = &ctx;

    acx->acx$q_r0[0] = acp[0];
    acx->acx$q_r0[1] = acp[1];
    acx->acx$q_r1[0] = acp[2];
    acx->acx$q_r1[1] = acp[3];
    acx->acx$q_r2[0] = acp[4];
    acx->acx$q_r2[1] = acp[5];
    acx->acx$q_r3[0] = acp[6];
    acx->acx$q_r3[1] = acp[7];
    acx->acx$q_r4[0] = acp[8];
    acx->acx$q_r4[1] = acp[9];
    acx->acx$q_r5[0] = acp[10];
    acx->acx$q_r5[1] = acp[11];
    acx->acx$q_r6[0] = acp[12];
    acx->acx$q_r6[1] = acp[13];
    acx->acx$q_r7[0] = acp[14];
    acx->acx$q_r7[1] = acp[15];
    acx->acx$q_r8[0] = acp[16];
    acx->acx$q_r8[1] = acp[17];
    acx->acx$q_r9[0] = acp[18];
    acx->acx$q_r9[1] = acp[19];
    acx->acx$q_r10[0] = acp[20];
    acx->acx$q_r10[1] = acp[21];
    acx->acx$q_r11[0] = acp[22];
    acx->acx$q_r11[1] = acp[23];
    acx->acx$q_r12[0] = acp[24];
    acx->acx$q_r12[1] = acp[25];
    acx->acx$q_r13[0] = acp[26];
    acx->acx$q_r13[1] = acp[27];
    acx->acx$q_r14[0] = acp[28];
    acx->acx$q_r14[1] = acp[29];
    acx->acx$q_r15[0] = acp[30];
    acx->acx$q_r15[1] = acp[31];
    acx->acx$q_r16[0] = acp[32];
    acx->acx$q_r16[1] = acp[33];
    acx->acx$q_r17[0] = acp[34];
    acx->acx$q_r17[1] = acp[35];
    acx->acx$q_r18[0] = acp[36];
    acx->acx$q_r18[1] = acp[37];
    acx->acx$q_r19[0] = acp[38];
    acx->acx$q_r19[1] = acp[39];
    acx->acx$q_r20[0] = acp[40];
    acx->acx$q_r20[1] = acp[41];
    acx->acx$q_r21[0] = acp[42];
    acx->acx$q_r21[1] = acp[43];
    acx->acx$q_r22[0] = acp[44];
    acx->acx$q_r22[1] = acp[45];
    acx->acx$q_r23[0] = acp[46];
    acx->acx$q_r23[1] = acp[47];
    acx->acx$q_r24[0] = acp[48];
    acx->acx$q_r24[1] = acp[49];
    acx->acx$q_r25[0] = acp[50];
    acx->acx$q_r25[1] = acp[51];
    acx->acx$q_r26[0] = acp[52];
    acx->acx$q_r26[1] = acp[53];
    acx->acx$q_r27[0] = acp[54];
    acx->acx$q_r27[1] = acp[55];
    acx->acx$q_r28[0] = acp[56];
    acx->acx$q_r28[1] = acp[57];
    acx->acx$q_r29[0] = acp[58];
    acx->acx$q_r29[1] = acp[59];
    acx->acx$q_r30[0] = acp[60];
    acx->acx$q_r30[1] = acp[61];

    walk_stack(fp, sp, pc, acx, pflag);
#endif
}

/*
 * lib$display_processes
 *
 * FUNCTIONAL DESCRIPTION:
 * 
 *   Displays the current processes
 * 
 * FORMAL PARAMETERS:
 * 
 *   None.
 *
 * RETURN VALUE:
 *
 *   None.
 * 
 */

void lib$display_processes()
{
#if SYMBOLS
    int pdebug_save;

    pdebug_save = print_debug_flag;
    print_debug_flag = 1;

    pprintf ("\n");

    krn$_walkpcb();

    print_debug_flag = pdebug_save;
#endif
}

/*
 * lib$break_at_ovly
 *
 * FUNCTIONAL DESCRIPTION:
 * 
 *   Sets a global with the name of the overlay to break at.
 * 
 * FORMAL PARAMETERS:
 * 
 *   None.
 *
 * RETURN VALUE:
 *
 *   None.
 * 
 */

void lib$break_at_ovly(char *ovly_name)
{
#if MODULAR
extern char *break_ovly_name[MAX_NAME];
extern int break_at_overlay;

    break_at_overlay = 1;
    ovly_name[strlen(ovly_name) - 1] = 0;
    strcpy(&break_ovly_name[0], ovly_name);
#endif
}

/*
 * lib$sym_expand
 *
 * FUNCTIONAL DESCRIPTION:
 * 
 *   Displays symbols
 * 
 * FORMAL PARAMETERS:
 * 
 *   None.
 *
 * RETURN VALUE:
 *
 *   None.
 * 
 */

#if SYMBOLS

int lib$sym_expand_cnt;

int lib$sym_expand_cb(char *buf)
{
    pprintf ("%s\n", buf);
    lib$sym_expand_cnt++;
    return 1;
}
#endif

int lib$sym_expand(char *SymPntr)
{
    int length;
    char *sptr;
    char *ptr;
    char c;

#if SYMBOLS

    lib$sym_expand_cnt = 0;

    length = strlen(SymPntr);
    sptr = strcspn(SymPntr, "\n\r/!\"=;");

    if (sptr == length)
	return 0;

    pprintf ("\n");

    ptr = (int)SymPntr + (int)sptr;
    c = *ptr;
    *ptr = 0;

    sym_expand(0, SymPntr, 0, 1, lib$sym_expand_cb, 0, 0);

    return lib$sym_expand_cnt;
#endif
    return 0;
}

/*
 * lib$dynamic
 *
 * FUNCTIONAL DESCRIPTION:
 * 
 *   Checks and printf dynamic memory usage.
 * 
 * FORMAL PARAMETERS:
 * 
 *   None.
 *
 * RETURN VALUE:
 *
 *   None.
 * 
 */

void lib$dynamic()
{
#if SYMBOLS

#if TURBO
#define dynamic (*(advcmd_shared_adr[2]))
extern int (**advcmd_shared_adr)();	/* pointer to advcmd overlay */
extern int firewall_flag;
#endif
    int pdebug_save;

    pdebug_save = print_debug_flag;
    print_debug_flag = 1;

    pprintf ("\n");

    dyn$_check();		/* show dynamic memory usage */

    print_debug_flag = pdebug_save;
#endif
}

/*
 * lib$call_rtn
 *
 * FUNCTIONAL DESCRIPTION:
 * 
 *   Calls a user specified routine that is found in the console
 * 
 * FORMAL PARAMETERS:
 * 
 *   rtn   - pd address of routine to be performed.
 *   p0:p5 - data to passed to routine.
 *
 * RETURN VALUE:
 *
 *   None.
 * 
 */

void lib$call_rtn(int *rtn(), protoargs int p0, int p1, int p2, int p3, int p4, int p5)
{
#if SYMBOLS
    int pdebug_save;
    int retval;
    char *sym = 0;

    sym = sym_table_lookup(rtn);
    if (!sym) {
	pprintf("No symbol found for %x\n", rtn);
	return;
    }

    pprintf("\n%s(%x,%x,%x,%x,%x,%x)\n", sym, p0, p1, p2, p3, p4, p5);

    pdebug_save = print_debug_flag;
    print_debug_flag = 1;

    retval = rtn(p0,p1,p2,p3,p4,p5);

    print_debug_flag = pdebug_save;

    pprintf("%s return value = %x\n", sym, retval);
#endif                                                              
}

/*
 * lib$call_cmd
 *
 * FUNCTIONAL DESCRIPTION:
 * 
 *   Calls a user specified command routine that is found in the console
 *   Allow up to 8 arguments or a total of 80 characters.
 * 
 * FORMAL PARAMETERS:
 * 
 *   rtn    - pd address of routine to be performed.
 *   strptr - pointer to asciz string for command.
 *
 * RETURN VALUE:
 *
 *   None.
 * 
 */

void lib$call_cmd(int *rtn(), char *strptr)
{
#if SYMBOLS

#define MAX_WORDS	9
#define MAX_CHARS	80
#define MAX_TOKEN	80
#define ARRAY_SIZE	(4 * (MAX_WORDS)) + MAX_CHARS + MAX_TOKEN

    int i;
    int pdebug_save;
    int retval;
    int qflag;
    int argc; 
    int **argv;
    char *sym = 0;
    char *iptr;
    char *wptr;
    char *aptr;
    char *array;
    char *token;
    char c;

    sym = sym_table_lookup(rtn);
    if (!sym) {
	pprintf("No symbol found for %x\n", rtn);
	return;
    }

    pprintf("\n%s(%s)\n", sym,strptr);

    array = dyn$_malloc(ARRAY_SIZE, DYN$K_SYNC|DYN$K_NOWAIT);
    if (!array) {
	pprintf ("call_cmd: can't allocate enough memory\n");
	return;
    }

    argc = 1;
    argv = array;
    aptr = array + (4 * MAX_WORDS);
    token = aptr + MAX_CHARS;
    iptr = strptr;

    for (i = 0; i < MAX_WORDS - 1; i++) {
	wptr = token;
	while ((*iptr == ' ') || (*iptr == '\t'))
	    iptr++;

	c = *iptr;
	if ((c == 0) || (c == '#') || (c == '!') || (c == '\n'))
	    break;

	qflag = 0;
	if (c == '"') {qflag = 1; iptr++;}

	while (1) {
	    c = *iptr;
	    if (c == 0) break;
	    if (c == '\n') break;
	    if (c == ' ') {if (!qflag) break;}
	    if (c == '\t') {if (!qflag) break;}
	    if (c == '"') {iptr++; break;}
	    *wptr++ = c;
	    *wptr = 0;
	    iptr++;
	}

	argv[argc++] = aptr;
	strcpy(aptr, token);
	aptr += strlen(token) + 1;
    }

    for (i = 1; i < argc; i++)
	pprintf("%d - %s, ", i, argv[i]);
    pprintf("\n");

    pdebug_save = print_debug_flag;
    print_debug_flag = 1;

    retval = rtn(argc, argv);

    print_debug_flag = pdebug_save;

    pprintf("%s return value = %x\n", sym, retval);

    free(array);
#endif                                                              
}

/*
 * lib$toggle
 *
 * FUNCTIONAL DESCRIPTION:
 * 
 *   Toggles register display for xdelta and
 *   symbols loading and unloading.
 * 
 * FORMAL PARAMETERS:
 * 
 *   register_flag - 0 = turn off symbols
 *		     1 = turn on symbols
 *
 * RETURN VALUE:
 *
 *   None.
 * 
 */

void lib$toggle(int register_flag)
{
#if MODULAR
    int id = whoami();

    if (register_flag) {
	if (!load_symbols_flag) {
	    if (mfpr_ipl() == IPL_RUN) {
		ovly_symbol_load();
	    } else {
		if ((ovly_cpu / 2) == (id / 2))
		    ovly_symbol_load();
	    }
	}
    } else {
	if (load_symbols_flag) {
	    if (mfpr_ipl() == IPL_RUN) {
		ovly_symbol_flush();
	    } else {
		if ((ovly_cpu / 2) == (id / 2))
		    ovly_symbol_flush();
	    }
	}
    }
#endif
}
