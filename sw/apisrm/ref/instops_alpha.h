/*
 * alpha_inst_ops.h - op code definitions for alpha instructions
 */

/* ALPHA Instruction OPTABLE */

struct alpha_s_opinfo {
	U_INT_16 opcode;
	U_INT_16 format;
	u_int rad;
	u_int rad1;
};

union alpha_instruction {
	u_int	inst;
	struct {
		int disp : 16;
		unsigned Rb : 5;
		unsigned Ra : 5;
		unsigned opcode : 6;
	} memory_format;
	struct {
		int disp : 21;
		unsigned Ra : 5;
		unsigned opcode : 6;
	} branch_format;
	struct {
		unsigned Rc : 5;
		unsigned func : 7;
		unsigned litc : 1;
		unsigned sbz : 3;
		unsigned Rb : 5;
		unsigned Ra : 5;
		unsigned opcode : 6;
	} operate0_format;
	struct {
		unsigned Rc : 5;
		unsigned func : 7;
		unsigned litc : 1;
		unsigned lit : 8;
		unsigned Ra : 5;
		unsigned opcode : 6;
	} operate1_format;
	struct {
		unsigned Fc : 5;
		unsigned func : 11;
		unsigned Fb : 5;
		unsigned Fa : 5;
		unsigned opcode : 6;
	} foperate_format;
	struct {
		unsigned function : 26;
		unsigned opcode : 6;
	} pal_format;
	struct {
		unsigned function : 16;
		unsigned Rb : 5;
		unsigned Ra : 5;
		unsigned opcode : 6;
	} sync_format;
	struct {
		unsigned index : 5;
		unsigned ibx : 1;
		unsigned abx : 1;
		unsigned pal : 1;
		unsigned ign : 8;
		unsigned Rb : 5;
		unsigned Ra : 5;
		unsigned opcode : 6;
	} hw_mtmf_format;
	struct {
		unsigned disp : 12;
		unsigned type : 4;
		unsigned Rb : 5;
		unsigned Ra : 5;
		unsigned opcode : 6;
	} hw_ldst_format;
	struct {
		unsigned ign : 14;
		unsigned hint : 2;
		unsigned Rb : 5;
		unsigned Ra : 5;
		unsigned opcode : 6;
	} hw_rei_format;
};

#define Append(string) AppendString((sizeof (string)-1), string)

#define SS_NORMAL		1
#define LIB$_NOINSTRAN		0
#define LIB$_NOSPACE		2
#define LIB$_INVREG		4
#define LIB$_INVCLASS		6
#define LIB$_DECODE_ERROR	256

#define OP_CH_SIZE 12		/* Size, in ascii chars, of opcode mnemonic. */

/* bounds for the instruction fields */

#define EIF$Opc_lower_bound	0
#define EIF$Opc_upper_bound	64
#define EIF$Opr_lower_bound	0
#define EIF$Opr_upper_bound	128
#define EIF$Flt_lower_bound	0
#define EIF$Flt_upper_bound	2048

/* decode field printing widths */

#define EIF$Mem_imm_width	4
#define EIF$ldst_imm_width	3
#define EIF$Brnch_imm_width	6
#define EIF$Opr_imm_width	2
 
/* Codes to refer to the register operand numbers */

#define EVX$K_R0	0
#define EVX$K_R1	1
#define EVX$K_R2	2
#define EVX$K_R3	3
#define EVX$K_R4	4
#define EVX$K_R5	5
#define EVX$K_R6	6
#define EVX$K_R7	7
#define EVX$K_R8	8
#define EVX$K_R9	9
#define EVX$K_R10	10
#define EVX$K_R11	11
#define EVX$K_R12	12
#define EVX$K_R13	13
#define EVX$K_R14	14
#define EVX$K_R15	15
#define EVX$K_R16	16
#define EVX$K_R17	17
#define EVX$K_R18	18
#define EVX$K_R19	19
#define EVX$K_R20	20
#define EVX$K_R21	21
#define EVX$K_R22	22
#define EVX$K_R23	23
#define EVX$K_R24	24
#define EVX$K_R25	25
#define EVX$K_R26	26
#define EVX$K_R27	27
#define EVX$K_R28	28
#define EVX$K_R29	29
#define EVX$K_R30	30
#define EVX$K_R31	31
#define EVX$K_F0	0
#define EVX$K_F1	1
#define EVX$K_F2	2
#define EVX$K_F3	3
#define EVX$K_F4	4
#define EVX$K_F5	5
#define EVX$K_F6	6
#define EVX$K_F7	7
#define EVX$K_F8	8
#define EVX$K_F9	9
#define EVX$K_F10	10
#define EVX$K_F11	11
#define EVX$K_F12	12
#define EVX$K_F13	13
#define EVX$K_F14	14
#define EVX$K_F15	15
#define EVX$K_F16	16
#define EVX$K_F17	17
#define EVX$K_F18	18
#define EVX$K_F19	19
#define EVX$K_F20	20
#define EVX$K_F21	21
#define EVX$K_F22	22
#define EVX$K_F23	23
#define EVX$K_F24	24
#define EVX$K_F25	25
#define EVX$K_F26	26
#define EVX$K_F27	27
#define EVX$K_F28	28
#define EVX$K_F29	29
#define EVX$K_F30	30
#define EVX$K_F31	31
#define EIF$Zero_register	32

/*
 * All the different classes of instructions (used to decode).
 * Each different class has a different printing format.
 */

#define EVX$K_imemory	1
#define EVX$K_fmemory	2
#define EVX$K_ibranch	3
#define EVX$K_fbranch	4
#define EVX$K_operate	5
#define EVX$K_float2	6
#define EVX$K_float3	7
#define EVX$K_jsr	8
#define EVX$K_rcc	9
#define EVX$K_pal	10
#define EVX$K_sync0	11
#define EVX$K_sync1	12

#define EVX$K_addi	13
#define EVX$K_logi	14
#define EVX$K_shfi	15
#define EVX$K_muli	16
#define EVX$K_sync	17
#define EVX$K_fpaux	18
#define EVX$K_fpdec	19
#define EVX$K_fpieee	20

#define EVX$K_unknown	21

#define EVX$K_mtmf	22
#define EVX$K_ldst	23
#define EVX$K_hwrei	24

/*
 *   Opcodes for the Memory format instructions
 */

#define EVX$OPC_LDL		40
#define EVX$OPC_LDQ		41
#define EVX$OPC_LDLL		42
#define EVX$OPC_LDQL		43
#define EVX$OPC_LDQU		11
#define EVX$OPC_LDA		8
#define EVX$OPC_STL		44
#define EVX$OPC_STQ		45
#define EVX$OPC_STLC		46
#define EVX$OPC_STQC		47
#define EVX$OPC_STQU		15
#define EVX$OPC_LDAH		9
#define EVX$OPC_LDF		32
#define EVX$OPC_LDD		33
#define EVX$OPC_LDS		34
#define EVX$OPC_LDT		35
#define EVX$OPC_STF		36
#define EVX$OPC_STD		37
#define EVX$OPC_STS		38
#define EVX$OPC_STT		39
#define EVX$OPC_SYNC		24
#define EVX$SYNC_MB		16384
#define EVX$SYNC_FETCH		32768
#define EVX$SYNC_RS		61440
#define EVX$SYNC_DRAINT		0
#define EVX$SYNC_FETCH_M	40960
#define EVX$SYNC_RPCC		49152
#define EVX$SYNC_RC		57344
#define EVX$OPC_JSR		26

/*
 *   Opcodes for the Branch format instructions
 */

#define EVX$OPC_BR		48
#define EVX$OPC_BSR		52
#define EVX$OPC_BLBC		56
#define EVX$OPC_BLBS		60
#define EVX$OPC_FBEQ		49
#define EVX$OPC_FBNE		53
#define EVX$OPC_BEQ		57
#define EVX$OPC_BNE		61
#define EVX$OPC_FBLT		50
#define EVX$OPC_FBGE		54
#define EVX$OPC_BLT		58
#define EVX$OPC_BGE		62
#define EVX$OPC_FBLE		51
#define EVX$OPC_FBGT		55
#define EVX$OPC_BLE		59
#define EVX$OPC_BGT		63

/*
 *   Opcodes for the Operate format instructions
 */

#define EVX$OPC_ADDI		16
#define EVX$OPC_LOGI		17
#define EVX$OPC_SHFI		18
#define EVX$OPC_MULI		19
#define EVX$ADDI_ADDL		0
#define EVX$ADDI_ADDLV		64
#define EVX$ADDI_ADDQ		32
#define EVX$ADDI_ADDQV		96
#define EVX$ADDI_SUBL		9
#define EVX$ADDI_SUBLV		73
#define EVX$ADDI_SUBQ		41
#define EVX$ADDI_SUBQV		105
#define EVX$ADDI_CMPEQ		45
#define EVX$ADDI_CMPLT		77
#define EVX$ADDI_CMPLE		109
#define EVX$ADDI_CMPULT		29
#define EVX$ADDI_CMPULE		61
#define EVX$ADDI_CMPBGE		15
#define EVX$LOGI_AND		0
#define EVX$LOGI_BIC		8
#define EVX$LOGI_CMOVEQ		36
#define EVX$LOGI_CMOVNE		38
#define EVX$LOGI_CMOVLBS	20
#define EVX$LOGI_BIS		32
#define EVX$LOGI_ORNOT		40
#define EVX$LOGI_CMOVLT		68
#define EVX$LOGI_CMOVGE		70
#define EVX$LOGI_CMOVLBC	22
#define EVX$LOGI_XOR		64
#define EVX$LOGI_EQV		72
#define EVX$LOGI_CMOVLE		100
#define EVX$LOGI_CMOVGT		102
#define EVX$SHFI_SLL		57
#define EVX$SHFI_SRA		60
#define EVX$SHFI_SRL		52
#define EVX$SHFI_EXTBL		6
#define EVX$SHFI_EXTWL		22
#define EVX$SHFI_EXTLL		38
#define EVX$SHFI_EXTQL		54
#define EVX$SHFI_EXTWH		90
#define EVX$SHFI_EXTLH		106
#define EVX$SHFI_EXTQH		122
#define EVX$SHFI_INSBL		11
#define EVX$SHFI_INSWL		27
#define EVX$SHFI_INSLL		43
#define EVX$SHFI_INSQL		59
#define EVX$SHFI_INSWH		87
#define EVX$SHFI_INSLH		103
#define EVX$SHFI_INSQH		119
#define EVX$SHFI_MSKBL		2
#define EVX$SHFI_MSKWL		18
#define EVX$SHFI_MSKLL		34
#define EVX$SHFI_MSKQL		50
#define EVX$SHFI_MSKWH		82
#define EVX$SHFI_MSKLH		98
#define EVX$SHFI_MSKQH		114
#define EVX$SHFI_ZAP		48
#define EVX$SHFI_ZAPNOT		49
#define EVX$MULI_MULL		0
#define EVX$MULI_MULQV		96
#define EVX$MULI_MULLV		64
#define EVX$MULI_UMULH		48
#define EVX$MULI_MULQ		32

/*
 *   Opcodes for the Floating point Operate format instructions
 */

#define EVX$OPC_FPDEC		21
#define EVX$OPC_FPIEEE		22
#define EVX$OPC_FPAUX		23

/*
 *   FUNCTION FIELDS for the Floating point instructions
 */

#define EVX$FPAUX_CPYS			0
#define EVX$FPAUX_CVTLQ			16
#define EVX$FPAUX_FCMOVEQ		10
#define EVX$FPAUX_FCMOVNE		11
#define EVX$FPAUX_CPYSN			1
#define EVX$FPAUX_CVTQL			48
#define EVX$FPAUX_FCMOVLT		12
#define EVX$FPAUX_FCMOVGE		13
#define EVX$FPAUX_CPYSE			2
#define EVX$FPAUX_CVTQLV		304
#define EVX$FPAUX_FCMOVLE		14
#define EVX$FPAUX_FCMOVGT		15
#define EVX$FPAUX_CPYSEE		3
#define EVX$FPIEEE_CMPSEQ		133
#define EVX$FPIEEE_CMPSLT		134
#define EVX$FPIEEE_CMPSLE		135
#define EVX$FPIEEE_CMPSUN		132
#define EVX$FPIEEE_CMPTEQ		165
#define EVX$FPIEEE_CMPTLT		166
#define EVX$FPIEEE_CMPTLE		167
#define EVX$FPIEEE_CMPTUN		164
#define EVX$FPIEEE_CMPSEQSU		1413
#define EVX$FPIEEE_CMPSLTSU		1414
#define EVX$FPIEEE_CMPSLESU		1415
#define EVX$FPIEEE_CMPSUNSU		1412
#define EVX$FPIEEE_CMPTEQSU		1445
#define EVX$FPIEEE_CMPTLTSU		1446
#define EVX$FPIEEE_CMPTLESU		1447
#define EVX$FPIEEE_CMPTUNSU		1444
#define EVX$FPIEEE_CVTQS		188
#define EVX$FPIEEE_CVTQSC		60
#define EVX$FPIEEE_CVTQSM		124
#define EVX$FPIEEE_CVTQSP		252
#define EVX$FPIEEE_CVTQSSUI		1980
#define EVX$FPIEEE_CVTQSSUIC		1852
#define EVX$FPIEEE_CVTQSSUIM		1916
#define EVX$FPIEEE_CVTQSSUIP		2044
#define EVX$FPIEEE_CVTQT		190
#define EVX$FPIEEE_CVTQTC		62
#define EVX$FPIEEE_CVTQTM		126
#define EVX$FPIEEE_CVTQTP		254
#define EVX$FPIEEE_CVTQTSUI		1982
#define EVX$FPIEEE_CVTQTSUIC		1854
#define EVX$FPIEEE_CVTQTSUIM		1918
#define EVX$FPIEEE_CVTQTSUIP		2046
#define EVX$FPIEEE_CVTST		142
#define EVX$FPIEEE_CVTSTC		14
#define EVX$FPIEEE_CVTSTM		78
#define EVX$FPIEEE_CVTSTP		206
#define EVX$FPIEEE_CVTSTSUI		1934
#define EVX$FPIEEE_CVTSTSUIC		1806
#define EVX$FPIEEE_CVTSTSUIM		1870
#define EVX$FPIEEE_CVTSTSUIP		1998
#define EVX$FPIEEE_CVTTS		172
#define EVX$FPIEEE_CVTTSC		44
#define EVX$FPIEEE_CVTTSM		108
#define EVX$FPIEEE_CVTTSP		236
#define EVX$FPIEEE_CVTTSU		428
#define EVX$FPIEEE_CVTTSUC		300
#define EVX$FPIEEE_CVTTSUM		364
#define EVX$FPIEEE_CVTTSUP		492
#define EVX$FPIEEE_CVTTSSU		1452
#define EVX$FPIEEE_CVTTSSUC		1324
#define EVX$FPIEEE_CVTTSSUM		1388
#define EVX$FPIEEE_CVTTSSUP		1516
#define EVX$FPIEEE_CVTTSSUI		1964
#define EVX$FPIEEE_CVTTSSUIC		1836
#define EVX$FPIEEE_CVTTSSUIM		1900
#define EVX$FPIEEE_CVTTSSUIP		2028
#define EVX$FPIEEE_DIVS			131
#define EVX$FPIEEE_DIVSC		3
#define EVX$FPIEEE_DIVSM		67
#define EVX$FPIEEE_DIVSP		195
#define EVX$FPIEEE_DIVSU		387
#define EVX$FPIEEE_DIVSUC		259
#define EVX$FPIEEE_DIVSUM		323
#define EVX$FPIEEE_DIVSUP		451
#define EVX$FPIEEE_DIVSSU		1411
#define EVX$FPIEEE_DIVSSUC		1283
#define EVX$FPIEEE_DIVSSUM		1347
#define EVX$FPIEEE_DIVSSUP		1475
#define EVX$FPIEEE_DIVSSUI		1923
#define EVX$FPIEEE_DIVSSUIC		1795
#define EVX$FPIEEE_DIVSSUIM		1859
#define EVX$FPIEEE_DIVSSUIP		1987
#define EVX$FPIEEE_DIVT			163
#define EVX$FPIEEE_DIVTC		35
#define EVX$FPIEEE_DIVTM		99
#define EVX$FPIEEE_DIVTP		227
#define EVX$FPIEEE_DIVTU		419
#define EVX$FPIEEE_DIVTUC		291
#define EVX$FPIEEE_DIVTUM		355
#define EVX$FPIEEE_DIVTUP		483
#define EVX$FPIEEE_DIVTSU		1443
#define EVX$FPIEEE_DIVTSUC		1315
#define EVX$FPIEEE_DIVTSUM		1379
#define EVX$FPIEEE_DIVTSUP		1507
#define EVX$FPIEEE_DIVTSUI		1955
#define EVX$FPIEEE_DIVTSUIC		1827
#define EVX$FPIEEE_DIVTSUIM		1891
#define EVX$FPIEEE_DIVTSUIP		2019
#define EVX$FPIEEE_MULS			130
#define EVX$FPIEEE_MULSC		2
#define EVX$FPIEEE_MULSM		66
#define EVX$FPIEEE_MULSP		194
#define EVX$FPIEEE_MULSU		386
#define EVX$FPIEEE_MULSUC		258
#define EVX$FPIEEE_MULSUM		322
#define EVX$FPIEEE_MULSUP		450
#define EVX$FPIEEE_MULSSU		1410
#define EVX$FPIEEE_MULSSUC		1282
#define EVX$FPIEEE_MULSSUM		1346
#define EVX$FPIEEE_MULSSUP		1474
#define EVX$FPIEEE_MULSSUI		1922
#define EVX$FPIEEE_MULSSUIC		1794
#define EVX$FPIEEE_MULSSUIM		1858
#define EVX$FPIEEE_MULSSUIP		1986
#define EVX$FPIEEE_MULT			162
#define EVX$FPIEEE_MULTC		34
#define EVX$FPIEEE_MULTM		98
#define EVX$FPIEEE_MULTP		226
#define EVX$FPIEEE_MULTU		418
#define EVX$FPIEEE_MULTUC		290
#define EVX$FPIEEE_MULTUM		354
#define EVX$FPIEEE_MULTUP		482
#define EVX$FPIEEE_MULTSU		1442
#define EVX$FPIEEE_MULTSUC		1314
#define EVX$FPIEEE_MULTSUM		1378
#define EVX$FPIEEE_MULTSUP		1506
#define EVX$FPIEEE_MULTSUI		1954
#define EVX$FPIEEE_MULTSUIC		1826
#define EVX$FPIEEE_MULTSUIM		1890
#define EVX$FPIEEE_MULTSUIP		2018
#define EVX$FPIEEE_SUBS			129
#define EVX$FPIEEE_SUBSC		1
#define EVX$FPIEEE_SUBSM		65
#define EVX$FPIEEE_SUBSP		193
#define EVX$FPIEEE_SUBSU		385
#define EVX$FPIEEE_SUBSUC		257
#define EVX$FPIEEE_SUBSUM		321
#define EVX$FPIEEE_SUBSUP		449
#define EVX$FPIEEE_SUBSSU		1409
#define EVX$FPIEEE_SUBSSUC		1281
#define EVX$FPIEEE_SUBSSUM		1345
#define EVX$FPIEEE_SUBSSUP		1473
#define EVX$FPIEEE_SUBSSUI		1921
#define EVX$FPIEEE_SUBSSUIC		1793
#define EVX$FPIEEE_SUBSSUIM		1857
#define EVX$FPIEEE_SUBSSUIP		1985
#define EVX$FPIEEE_SUBT			161
#define EVX$FPIEEE_SUBTC		33
#define EVX$FPIEEE_SUBTM		97
#define EVX$FPIEEE_SUBTP		225
#define EVX$FPIEEE_SUBTU		417
#define EVX$FPIEEE_SUBTUC		289
#define EVX$FPIEEE_SUBTUM		353
#define EVX$FPIEEE_SUBTUP		481
#define EVX$FPIEEE_SUBTSU		1441
#define EVX$FPIEEE_SUBTSUC		1313
#define EVX$FPIEEE_SUBTSUM		1377
#define EVX$FPIEEE_SUBTSUP		1505
#define EVX$FPIEEE_SUBTSUI		1953
#define EVX$FPIEEE_SUBTSUIC		1825
#define EVX$FPIEEE_SUBTSUIM		1889
#define EVX$FPIEEE_SUBTSUIP		2017
#define EVX$FPIEEE_ADDS			128
#define EVX$FPIEEE_ADDSC		0
#define EVX$FPIEEE_ADDSM		64
#define EVX$FPIEEE_ADDSP		192
#define EVX$FPIEEE_ADDSU		384
#define EVX$FPIEEE_ADDSUC		256
#define EVX$FPIEEE_ADDSUM		320
#define EVX$FPIEEE_ADDSUP		448
#define EVX$FPIEEE_ADDSSU		1408
#define EVX$FPIEEE_ADDSSUC		1280
#define EVX$FPIEEE_ADDSSUM		1344
#define EVX$FPIEEE_ADDSSUP		1472
#define EVX$FPIEEE_ADDSSUI		1920
#define EVX$FPIEEE_ADDSSUIC		1792
#define EVX$FPIEEE_ADDSSUIM		1856
#define EVX$FPIEEE_ADDSSUIP		1984
#define EVX$FPIEEE_ADDT			160
#define EVX$FPIEEE_ADDTC		32
#define EVX$FPIEEE_ADDTM		96
#define EVX$FPIEEE_ADDTP		224
#define EVX$FPIEEE_ADDTU		416
#define EVX$FPIEEE_ADDTUC		288
#define EVX$FPIEEE_ADDTUM		352
#define EVX$FPIEEE_ADDTUP		480
#define EVX$FPIEEE_ADDTSU		1440
#define EVX$FPIEEE_ADDTSUC		1312
#define EVX$FPIEEE_ADDTSUM		1376
#define EVX$FPIEEE_ADDTSUP		1504
#define EVX$FPIEEE_ADDTSUI		1952
#define EVX$FPIEEE_ADDTSUIC		1824
#define EVX$FPIEEE_ADDTSUIM		1888
#define EVX$FPIEEE_ADDTSUIP		2016
#define EVX$FPIEEE_CVTSQ		143
#define EVX$FPIEEE_CVTSQC		15
#define EVX$FPIEEE_CVTSQV		399
#define EVX$FPIEEE_CVTSQVC		271
#define EVX$FPIEEE_CVTSQSV		1423
#define EVX$FPIEEE_CVTSQSVC		1295
#define EVX$FPIEEE_CVTSQSVI		1935
#define EVX$FPIEEE_CVTSQSVIC		1807
#define EVX$FPIEEE_CVTTQ		175
#define EVX$FPIEEE_CVTTQC		47
#define EVX$FPIEEE_CVTTQV		431
#define EVX$FPIEEE_CVTTQVC		303
#define EVX$FPIEEE_CVTTQSV		1455
#define EVX$FPIEEE_CVTTQSVC		1327
#define EVX$FPIEEE_CVTTQSVI		1967
#define EVX$FPIEEE_CVTTQSVIC		1839

/*
 *   FUNCTION FIELDS for the DEC Floating Point instructions
 */

#define EVX$FPDEC_ADDD			144
#define EVX$FPDEC_ADDDC			16
#define EVX$FPDEC_ADDDU			400
#define EVX$FPDEC_ADDDUC		272
#define EVX$FPDEC_ADDDS			1168
#define EVX$FPDEC_ADDDSC		1040
#define EVX$FPDEC_ADDDSU		1424
#define EVX$FPDEC_ADDDSUC		1296
#define EVX$FPDEC_ADDF			128
#define EVX$FPDEC_ADDFC			0
#define EVX$FPDEC_ADDFU			384
#define EVX$FPDEC_ADDFUC		256
#define EVX$FPDEC_ADDFS			1152
#define EVX$FPDEC_ADDFSC		1024
#define EVX$FPDEC_ADDFSU		1408
#define EVX$FPDEC_ADDFSUC		1280
#define EVX$FPDEC_ADDG			160
#define EVX$FPDEC_ADDGC			32
#define EVX$FPDEC_ADDGU			416
#define EVX$FPDEC_ADDGUC		288
#define EVX$FPDEC_ADDGS			1184
#define EVX$FPDEC_ADDGSC		1056
#define EVX$FPDEC_ADDGSU		1440
#define EVX$FPDEC_ADDGSUC		1312
#define EVX$FPDEC_SUBD			145
#define EVX$FPDEC_SUBDC			17
#define EVX$FPDEC_SUBDU			401
#define EVX$FPDEC_SUBDUC		273
#define EVX$FPDEC_SUBDS			1169
#define EVX$FPDEC_SUBDSC		1041
#define EVX$FPDEC_SUBDSU		1425
#define EVX$FPDEC_SUBDSUC		1297
#define EVX$FPDEC_SUBF			129
#define EVX$FPDEC_SUBFC			1
#define EVX$FPDEC_SUBFU			385
#define EVX$FPDEC_SUBFUC		257
#define EVX$FPDEC_SUBFS			1153
#define EVX$FPDEC_SUBFSC		1025
#define EVX$FPDEC_SUBFSU		1409
#define EVX$FPDEC_SUBFSUC		1281
#define EVX$FPDEC_SUBG			161
#define EVX$FPDEC_SUBGC			33
#define EVX$FPDEC_SUBGU			417
#define EVX$FPDEC_SUBGUC		289
#define EVX$FPDEC_SUBGS			1185
#define EVX$FPDEC_SUBGSC		1057
#define EVX$FPDEC_SUBGSU		1441
#define EVX$FPDEC_SUBGSUC		1313
#define EVX$FPDEC_MULD			146
#define EVX$FPDEC_MULDC			18
#define EVX$FPDEC_MULDU			402
#define EVX$FPDEC_MULDUC		274
#define EVX$FPDEC_MULDS			1170
#define EVX$FPDEC_MULDSC		1042
#define EVX$FPDEC_MULDSU		1426
#define EVX$FPDEC_MULDSUC		1298
#define EVX$FPDEC_MULF			130
#define EVX$FPDEC_MULFC			2
#define EVX$FPDEC_MULFU			386
#define EVX$FPDEC_MULFUC		258
#define EVX$FPDEC_MULFS			1154
#define EVX$FPDEC_MULFSC		1026
#define EVX$FPDEC_MULFSU		1410
#define EVX$FPDEC_MULFSUC		1282
#define EVX$FPDEC_MULG			162
#define EVX$FPDEC_MULGC			34
#define EVX$FPDEC_MULGU			418
#define EVX$FPDEC_MULGUC		290
#define EVX$FPDEC_MULGS			1186
#define EVX$FPDEC_MULGSC		1058
#define EVX$FPDEC_MULGSU		1442
#define EVX$FPDEC_MULGSUC		1314
#define EVX$FPDEC_DIVD			147
#define EVX$FPDEC_DIVDC			19
#define EVX$FPDEC_DIVDU			403
#define EVX$FPDEC_DIVDUC		275
#define EVX$FPDEC_DIVDS			1171
#define EVX$FPDEC_DIVDSC		1043
#define EVX$FPDEC_DIVDSU		1427
#define EVX$FPDEC_DIVDSUC		1299
#define EVX$FPDEC_DIVF			131
#define EVX$FPDEC_DIVFC			3
#define EVX$FPDEC_DIVFU			387
#define EVX$FPDEC_DIVFUC		259
#define EVX$FPDEC_DIVFS			1155
#define EVX$FPDEC_DIVFSC		1027
#define EVX$FPDEC_DIVFSU		1411
#define EVX$FPDEC_DIVFSUC		1283
#define EVX$FPDEC_DIVG			163
#define EVX$FPDEC_DIVGC			35
#define EVX$FPDEC_DIVGU			419
#define EVX$FPDEC_DIVGUC		291
#define EVX$FPDEC_DIVGS			1187
#define EVX$FPDEC_DIVGSC		1059
#define EVX$FPDEC_DIVGSU		1443
#define EVX$FPDEC_DIVGSUC		1315
#define EVX$FPDEC_CMPDEQ		149
#define EVX$FPDEC_CMPDLT		150
#define EVX$FPDEC_CMPDLE		151
#define EVX$FPDEC_CMPGEQ		165
#define EVX$FPDEC_CMPGLT		166
#define EVX$FPDEC_CMPGLE		167
#define EVX$FPDEC_CMPDEQS		1173
#define EVX$FPDEC_CMPDLTS		1174
#define EVX$FPDEC_CMPDLES		1175
#define EVX$FPDEC_CMPGEQS		1189
#define EVX$FPDEC_CMPGLTS		1190
#define EVX$FPDEC_CMPGLES		1191
#define EVX$FPDEC_CVTDF			156
#define EVX$FPDEC_CVTDFC		28
#define EVX$FPDEC_CVTDFS		1180
#define EVX$FPDEC_CVTDFSC		1052
#define EVX$FPDEC_CVTFG			142
#define EVX$FPDEC_CVTFGC		14
#define EVX$FPDEC_CVTFGS		1166
#define EVX$FPDEC_CVTFGSC		1038
#define EVX$FPDEC_CVTGF			172
#define EVX$FPDEC_CVTGFC		44
#define EVX$FPDEC_CVTGFU		428
#define EVX$FPDEC_CVTGFUC		300
#define EVX$FPDEC_CVTGFS		1196
#define EVX$FPDEC_CVTGFSC		1068
#define EVX$FPDEC_CVTGFSU		1452
#define EVX$FPDEC_CVTGFSUC		1324
#define EVX$FPDEC_CVTQD			189
#define EVX$FPDEC_CVTQDC		61
#define EVX$FPDEC_CVTQDS		1213
#define EVX$FPDEC_CVTQDSC		1085
#define EVX$FPDEC_CVTQF			188
#define EVX$FPDEC_CVTQFC		60
#define EVX$FPDEC_CVTQFS		1212
#define EVX$FPDEC_CVTQFSC		1084
#define EVX$FPDEC_CVTQG			190
#define EVX$FPDEC_CVTQGC		62
#define EVX$FPDEC_CVTQGS		1214
#define EVX$FPDEC_CVTQGSC		1086
#define EVX$FPDEC_CVTDQ			159
#define EVX$FPDEC_CVTDQC		31
#define EVX$FPDEC_CVTDQV		415
#define EVX$FPDEC_CVTDQVC		287
#define EVX$FPDEC_CVTDQS		1183
#define EVX$FPDEC_CVTDQSC		1055
#define EVX$FPDEC_CVTDQSV		1439
#define EVX$FPDEC_CVTDQSVC		1311
#define EVX$FPDEC_CVTGQ			175
#define EVX$FPDEC_CVTGQC		47
#define EVX$FPDEC_CVTGQV		431
#define EVX$FPDEC_CVTGQVC		303
#define EVX$FPDEC_CVTGQS		1199
#define EVX$FPDEC_CVTGQSC		1071
#define EVX$FPDEC_CVTGQSV		1455
#define EVX$FPDEC_CVTGQSVC		1327

/*
 *   Opcode/Functions for the PAL code instructions
 */

#define EVX$OPC_PAL		0
#define EVX$PAL_BPT		128
#define EVX$PAL_BUGCHK		129
#define EVX$PAL_CHME		130
#define EVX$PAL_CHMK		131
#define EVX$PAL_CHMS		132
#define EVX$PAL_CHMU		133
#define EVX$PAL_IMB		134
#define EVX$PAL_INSQHIL		135
#define EVX$PAL_INSQTIL		136
#define EVX$PAL_INSQHIQ		137
#define EVX$PAL_INSQTIQ		138
#define EVX$PAL_INSQUEL		139
#define EVX$PAL_INSQUEQ		140
#define EVX$PAL_INSQUELD	141
#define EVX$PAL_INSQUEQD	142
#define EVX$PAL_PROBER		143
#define EVX$PAL_PROBEW		144
#define EVX$PAL_RD_PS		145
#define EVX$PAL_REI		146
#define EVX$PAL_REMQHIL		147
#define EVX$PAL_REMQTIL		148
#define EVX$PAL_REMQHIQ		149
#define EVX$PAL_REMQTIQ		150
#define EVX$PAL_REMQUEL		151
#define EVX$PAL_REMQUEQ		152
#define EVX$PAL_REMQUELD	153
#define EVX$PAL_REMQUEQD	154
#define EVX$PAL_SWASTEN		155
#define EVX$PAL_WR_PS_SW	156
#define EVX$PAL_CFLUSH		1
#define EVX$PAL_TRAPB		2
#define EVX$PAL_HALT		0
#define EVX$PAL_LDQP		3
#define EVX$PAL_STQP		4
#define EVX$PAL_SWPCTX		5
#define EVX$PAL_MFPR_ASN	6
#define EVX$PAL_MTPR_ASTEN	7
#define EVX$PAL_MTPR_ASTSR	8
#define EVX$PAL_MFPR_AT		9
#define EVX$PAL_MTPR_AT		10
#define EVX$PAL_MFPR_FEN	11
#define EVX$PAL_MTPR_FEN	12
#define EVX$PAL_MTPR_IPIR	13
#define EVX$PAL_MFPR_IPL	14
#define EVX$PAL_MTPR_IPL	15
#define EVX$PAL_MFPR_MCES	16
#define EVX$PAL_MTPR_MCES	17
#define EVX$PAL_MFPR_PCBB	18
#define EVX$PAL_MFPR_PRBR	19
#define EVX$PAL_MTPR_PRBR	20
#define EVX$PAL_MFPR_PTBR	21
#define EVX$PAL_MFPR_SCBB	22
#define EVX$PAL_MTPR_SCBB	23
#define EVX$PAL_MTPR_SIRR	24
#define EVX$PAL_MFPR_SISR	25
#define EVX$PAL_MFPR_TBCHK	26
#define EVX$PAL_MTPR_TBIA	27
#define EVX$PAL_MTPR_TBIAP	28
#define EVX$PAL_MTPR_TBIS	29
#define EVX$PAL_MFPR_ESP	30
#define EVX$PAL_MTPR_ESP	31
#define EVX$PAL_MFPR_SSP	32
#define EVX$PAL_MTPR_SSP	33
#define EVX$PAL_MFPR_USP	34
#define EVX$PAL_MTPR_USP	35
#define EVX$PAL_MFPR_WHAMI	63
#define EVX$PAL_RESERVED_DVT	254
#define EVX$PAL_RESERVED_SIM	255

/*
 *   Opcodes reserved for PALcode
 */

#define EVX$OPC_PAL19		25
#define EVX$OPC_PAL1B		27
#define EVX$OPC_PAL1D		29
#define EVX$OPC_PAL1E		30
#define EVX$OPC_PAL1F		31

/*
 *   Opcodes for Special PALcode Instructions
 */
#define EVX$PAL_HW_MFPR		25
#define EVX$PAL_HW_MTPR		29
#define EVX$PAL_HW_REI		30

#define EVX$PAL_HW_LD		27
#define EVX$PAL_HW_ST		31

#define EVX$PAL_HW_LDQ		1
#define EVX$PAL_HW_LDW		2
#define EVX$PAL_HW_LDQW		3
#define EVX$PAL_HW_LDA		4
#define EVX$PAL_HW_LDQA		5
#define EVX$PAL_HW_LDAW		6
#define EVX$PAL_HW_LDQWA	7
#define EVX$PAL_HW_LDP		8
#define EVX$PAL_HW_LDPQ		9		
#define EVX$PAL_HW_LDPW		10
#define EVX$PAL_HW_LDPWQ	11
#define EVX$PAL_HW_LDPA		12
#define EVX$PAL_HW_LDPQA	13
#define EVX$PAL_HW_LDPWA	14
#define EVX$PAL_HW_LDPQWA	15

#define EVX$PAL_HW_STQ		1
#define EVX$PAL_HW_STW		2
#define EVX$PAL_HW_STQW		3
#define EVX$PAL_HW_STA		4
#define EVX$PAL_HW_STQA		5
#define EVX$PAL_HW_STAW		6
#define EVX$PAL_HW_STQWA	7
#define EVX$PAL_HW_STP		8
#define EVX$PAL_HW_STPQ		9		
#define EVX$PAL_HW_STPW		10
#define EVX$PAL_HW_STPWQ	11
#define EVX$PAL_HW_STPA		12
#define EVX$PAL_HW_STPQA	13
#define EVX$PAL_HW_STPWA	14
#define EVX$PAL_HW_STPQWA	15


/*
 *   Opcodes reserved for DIGITAL
 */

#define EVX$OPC_OPC01		1
#define EVX$OPC_OPC02		2
#define EVX$OPC_OPC03		3
#define EVX$OPC_OPC04		4
#define EVX$OPC_OPC05		5
#define EVX$OPC_OPC06		6
#define EVX$OPC_OPC07		7
#define EVX$OPC_OPC0A		10
#define EVX$OPC_OPC0C		12
#define EVX$OPC_OPC0D		13
#define EVX$OPC_OPC0E		14
#define EVX$OPC_OPC14		20
#define EVX$OPC_OPC1C		28

/* [end of file] alpha_inst_ops */
