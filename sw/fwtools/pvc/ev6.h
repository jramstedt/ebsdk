/* ev6.h */
/*
 *	
 *	Copyright (c) 1996
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
 *	----------------------------------------------------------------------
 *     V3.24 fls	9/24/96 - Copied from ev6_defs.h which is created
 *			from ev6_defs.dnr using:
 *			unix> defnr -h -i $tpass/ev6_defs.dnr -o ev6_defs.h
 */

#define MAXSTALLCOUNT 50	/* max ev6 stall cycles allowed */
/* decc won't accept 64 bit long #define MAX_INT 100000000000	maximum integer for long (64 bits)*/

/* follow taken from ev6 perf. model */
#define DIFF(A,B,SIZE) 	((A)>=(B)?(A)-(B):(A)-(B)+(SIZE))
#define SUM(A,B,SIZE) 	((A)+(B)<(SIZE)?(A)+(B):(A)+(B)-(SIZE))
#define INC(A,SIZE)  	((A)==(SIZE)-1?0:(A)+1)
#define ADD(A,B,SIZE)	((A+B)>=SIZE?(A+B-SIZE):(A+B))
#define DEC(A,SIZE)	((A)==0?(SIZE)-1:(A)-1)
#define POW2(n)		(1 << (n))
#define MASK(n)		((n) - 1)
#define ADD_SAT(a,b,c)  (((a+b)>c)?c:(a+b))
#define SUB_SAT(a,b,c)  (((a-b)<c)?c:(a-b))

#define EBOX_QUEUE_ENTRIES 20
#ifndef FBOX_QUEUE_ENTRIES
#define FBOX_QUEUE_ENTRIES 14
#endif
#define BAD_QUEUE_COUNT    0

#define EBOX_WIDTH 4	/* no. of ebox pipelines (L0,U0,L1,U1) */
#define FBOX_WIDTH 4	/* no. of fbox pipelines (FA,FM,FST0,FST11) */
#define SPLIT_EBOX 1

#define ICACHE_WIDTH_LOG2	2  /* 2^2 = 4 instructions */
#define ICACHE_WIDTH		POW2(ICACHE_WIDTH_LOG2)	 /* 4 */
#define SLOT_WIDTH ICACHE_WIDTH
#define E_ARB_2 1		/* number of ebox cluster arbiters in EV6*/

#define C0A_ARB 1			/* Define cluster arb priority */
#define C0B_ARB 4
#define C1A_ARB 2
#define C1B_ARB 3

#define FROM_EBOX 0		/*fls issue_inst will issue integer inst. */
#define FROM_FBOX 1		/*fls issue_inst will issue float inst. */


/* EBOX */

#define ALU_LATENCY 1
#define SHF_LATENCY 1
#define JSR_BSR_LATENCY 3		/* JSR,BSR,BR have this latency */
#define IMUL_LATENCY 6
#define IMUL_LATENCY_TO_RETIRE ALU_LATENCY /* make integer multiply errors non-recoverable */



/* FBOX */

#ifndef FADD_LATENCY
#define FADD_LATENCY 4
#endif
#define FMUL_LATENCY 4
#define FDIV_S_LATENCY 12       /* Single precision, doesn't include Add pipe latenct */
#define FDIV_T_LATENCY 15       /* Double precision */
#define FADD_PIPES 1		/* number of FADD pipes, also includes fcmov,fcpys,fcbr... */
#define FMUL_PIPES 1            /* number of FMUL pipes */
#define FST_PIPES 2
#define FDIV_NUM 1
#define FBOX_STORE_DELAY 2
#define FBOX_SLOW_STORE 1

/* retire stage */

#define CBR_MISPREDICT_TRAP_DELAY 3  /* delay from R stage */
#define JSR_MISPREDICT_TRAP_DELAY 4  /* delay from R stage */
#define MEM_TRAP_DELAY            7  /* delay from D stage */
#define DTBMISS_TRAP_DELAY        7  /* delay from D stage */
#define ITBMISS_TRAP_DELAY        5  /* delay from fetch stage */

#define EBOX_RETIRE       2      /* delay beyond operate latency */
#define FBOX_RETIRE       2      /* delay beyond operate latency */
#define BRANCH_RETIRE     3      /* delay from R stage */
#define JSR_RETIRE        6      /* delay from R stage */
#define MEM_RETIRE        6      /* delay from R stage */

#define RSTALL_MISP_TRAP    0x038   /* retire logic stalls for mispredict, starting at R stage */
#define RSTALL_MEM_TRAP     0x3F0   /* retire logic stalls for Mbox trap, starting cycle after D stage */
#define RSTALL_DTBMISS_TRAP 0x3F0   /* retire logic stalls for DTBmiss trap, starting cycle after D stage */
#define RSTALL_ITBMISS_TRAP 0x3F0   /* retire logic stalls for ITBmiss trap, starting cycle after fetch stage */

#define LDQ_FREE_PER_CYCLE  4   /* number of LDQ entries freed/cycle after retire & done */
#define DONE_BIT_DELAY      3   /* delay for a done bit set on a hit */
#define LDQ_RETIRE_LATENCY  3   /* delay from retire ptr -> ldq entry freed   */
#define STQ_RETIRE_LATENCY  3   /* delay from retire ptr -> stq entry retired */

#define RETIRE_GROUP_SIZE   2	/* Number of Mapper blocks across which retire pointer can sweep */
#define REG_FREE_GROUP_SIZE 2
#define RETIRE_CBR_MAX      1

#define MAX_REG_FREE_LATENCY 3
#define EREG_FREE_LATENCY   3   /* retire ptr sweep to map stage which uses register */
#define FREG_FREE_LATENCY   3   /* retire ptr sweep to map stage which uses register */


/* queue stage */

/*
** IPR Scoreboard bit stuff, 8 bits:
**     - lower 4 for ld/st issue interlocks, clear on issue of MT
**     - upper 4 for IPR W/W conflicts, clear on retire of MT
*/

#define IPR_BIT_NUM      8
#define IPR_LS_BIT_NUM   4
#define IPR_MASK         MASK(1<<IPR_BIT_NUM)
#define IPR_LS_MASK      MASK(1<<IPR_LS_BIT_NUM)
#define IPR_RET_MASK     (MASK(1<<(IPR_BIT_NUM-IPR_LS_BIT_NUM))<<IPR_LS_BIT_NUM)
#define IPR_CLR_DELAY    3
#define IS_MTPR(a)       ((a>>26)==0x1D)
#define IPR_BITS(a)      (a&IPR_MASK)
#define IPR_LS_BITS(a)   (a&IPR_LS_MASK)
#define IPR_RET_BITS(a)  (a&IPR_RET_MASK)

/* Map stage */
#define MAX_INFLIGHT 80
#define MAP_WIDTH 4
#define MAP_EBOX 4
#define MAP_FBOX 4

#ifndef MAX_EREG
#define MAX_EREG 80             /* 8 are for shadow-PAL */
#endif

#ifndef MAX_FREG
#define MAX_FREG 72
#endif



#define PAL_REG(a)   ((a&0xc)==8) /* define PAL shadow region in register number space */

#define IREG_FREE(num,delay) {			           \
	if (num>63) ifree3[delay] |= (long)1<<(num-64);    \
	else if (num>31) ifree2[delay] |= (long)1<<(num-32);    \
	     else ifree1[delay] |= (long)1<<num;                \
	ifree_cnt[delay]++;				   \
	}
#define FREG_FREE(num,delay) {			                                 \
	if ((num-MAX_EREG)>63) ffree3[delay] |= (long)1<<((num-MAX_EREG)-64);    \
	else if ((num-MAX_EREG)>31) ffree2[delay] |= (long)1<<((num-MAX_EREG)-32);    \
	     else ffree1[delay] |= (long)1<<(num-MAX_EREG);                           \
	ffree_cnt[delay]++;				                         \
	}
#define IREG_MAP(num) {			                   \
	if (num>63) ifree3[0] &= ~((long)1<<(num-64));     \
	else if (num>31) ifree2[0] &= ~((long)1<<(num-32));     \
	    else ifree1[0] &= ~((long)1<<num);                 \
	}
#define FREG_MAP(num) {			                   \
	if (num>63) ffree3[0] &= ~((long)1<<(num-64));     \
	else if (num>31) ffree2[0] &= ~((long)1<<(num-32));     \
	     else ffree1[0] &= ~((long)1<<num);                 \
	}

/* fls these only works for 64 bit longs 
#define IS_IREG_FREE(num) ((num>31)?(ifree2[0]&((long)1<<(num-32))):(ifree1[0]&((long)1<<num)))
#define IS_FREG_FREE(num) ((num>31)?(ffree2[0]&((long)1<<(num-32))):(ffree1[0]&((long)1<<num)))
*/
#define MAP_SKID_STALLS 2
#define MAP_EAT_UNUSED 2        /* cycles after map in which found but not used registers are out of play */
#define DEBUG_MAPPER 0

typedef struct reg_dirty {
  long d0;
  long d1;
  long spec_start;
  long spec_end;
  int  free_when_clean;
} DIRTY;
DIRTY reg_dirty[MAX_EREG+MAX_FREG];




enum classes 	{qi_none, qi_ild, qi_fld, qi_ist, qi_fst,
			 qi_lda,qi_mem_misc,qi_rpcc,qi_rx,
			 qi_mfpr, qi_ibr, qi_jsr, qi_fbr,
			 qi_iadd, qi_ilog, qi_ishf, qi_cmov, qi_cmov1,qi_cmov2, 
			 qi_imul, 
			 qi_imisc,qi_fadd,qi_fmul, qi_fcmov1,qi_fcmov2,
			 qi_fdiv, qi_fsqrt, qi_nop,
			 qi_ftoi, qi_itof, qi_mx_fpcr,
			 qi_mtpr, qi_max};

enum pipes  {PIPE_NONE= 0,
		    PIPE_L0=1, PIPE_L1=2, 
		    PIPE_U0=4, PIPE_U1=8,
		    PIPE_FA=16, PIPE_FM=32,
		    PIPE_FST0=64, PIPE_FST1=128   }; 

enum id_resourse	{Nores,
		Scbd0=1<<0, Scbd1=1<<1, Scbd2=1<<2, Scbd3=1<<3, Scbd4=1<<4,
		Scbd5=1<<5, Scbd6=1<<6,	Scbd7=1<<7, Memory_op=1<<8, Lock_inst=1<<9,
		Ibox_ipr=1<<10, Stf_itof=1<<11, Mbox_ipr=1<<12,
		Jsr=1<<13,
		Physical_inst=1<<14, Hwret=1<<15, Hwret_stall=1<<16,
		Arith_trap=1<<17,
		Virtual_inst=1<<18, Hw_stpc=1<<19, Mf=1<<20,
		Read=1<<21, Write=1<<22, Mt=1<<23, Br_cond=1<<24, Br=1<<25,
		Maxres = 26};		/* don't change without checking  Maxid_ipr */



enum id_ipr	{Noid_ipr = 	0<<Maxres,
			   Dtb_asn0 = 	1<<Maxres, 
			   Dtb_asn1 = 	2<<Maxres, 
			   Dtb_tag0 = 	3<<Maxres, 
			   Dtb_tag1 = 	4<<Maxres, 
			   Dtb_pte0 = 	5<<Maxres, 
			   Dtb_pte1 = 	6<<Maxres, 
			   Mt_fpcr = 	7<<Maxres, 	/* ev6 not hardware ipr */
			   Dtb_is0 = 	8<<Maxres, 
			   Dtb_is1 = 	9<<Maxres, 
			   Itb_ia  = 	10<<Maxres, 
			   Itb_iap  = 	11<<Maxres, 
			   Ic_flush  = 	12<<Maxres, 
			   Clear_map = 	13<<Maxres, 
			   Sleep    = 	14<<Maxres, 
			   Mm_stat = 	15<<Maxres, 
			   Va = 	16<<Maxres, 
			   Va_form = 	17<<Maxres, 
			   Dc_ctl = 	18<<Maxres, 
			   Data = 	19<<Maxres, 
			   Itb_tag = 	20<<Maxres, 
			   Maxid_ipr = 	31<<Maxres};

enum usage_flag_type {CMP_OR_LOGICAL, NORMAL};

enum stall_reason_type {NO_REASON =	0,
			       REG_DEPEND =	1<<0,
			       IPR_STALL =	1<<1,
			       PIPE_BUSY =	1<<2,
			       PIPE_L0_BUSY =   1<<3,	
			       PIPE_U0_BUSY =   1<<4,   
			       PIPE_L1_BUSY =   1<<5,
			       PIPE_U1_BUSY =   1<<6,
			       PIPE_FA_BUSY =   1<<7,
			       PIPE_FM_BUSY =   1<<8,
			       PIPE_FST0_BUSY = 1<<9,
			       PIPE_FST1_BUSY = 1<<10,
			       PIPE_L0_NOT_ALLOWED =   1<<11,	
			       PIPE_U0_NOT_ALLOWED =   1<<12,   
			       PIPE_L1_NOT_ALLOWED =   1<<13,  
			       PIPE_U1_NOT_ALLOWED =   1<<14, 
			       CLUSTER_BUSY =	1<<15,
			       EBOX_WRITE_PORT_BUSY = 	1<<16,
			       ISSUED_INST=		1<<17,
			       NO_MORE_INSTRUCTIONS=	1<<18,
			       MAP_STALL =		1<<19,
			       INUM_STALL =		1<<20,
			       IQ_STALL =		1<<21,
			       FQ_STALL =		1<<22,
			       BR_STALL =		1<<23,	/* means same as IC_BR_STALL in ic_state */
			       CMOV_STALL =		1<<23, 	/* means same as IC_CMOV_STALL in ic_state */
			       FETCH_STALL =		1<<24,	/* means same as IC_FETCH_STALL in ic_state */
			       HW_RET_STALL =	1<<25,   	/* means same as IC_HW_RET_STALL in ic_state */
			       RETIRE_WAIT =	1<<26	/* print retire wait cycles after all inst issued.*/
			       };

enum stall_flag {NO_STALL, STALL};
enum scoreboard_flag {NO_INIT_MAX_INT, INIT_MAX_INT};


typedef enum {
  IC_BR_STALL,
  IC_CMOV_STALL,
  IC_FETCH,
  IC_SEND_MISS,
  IC_IFQ,
  IC_IFQ_DELAY,
  IC_FILL,
  IC_BUBBLE_WAIT,
  IC_MISPRED_WAIT,
  IC_ITB_MISS_WAIT,
  IC_HW_RET_STALL
} IC_STATE;


/*
 * define the IPR's
 */

#define MAX_ADU_IPR		(128)

#define	EV6__SLEEP 0x17		/* V3.24 was not in ev6_defs.dnr */


/* ev6_defs.h last built 20-Nov-1996 from ev6_dnr dated 15-oct-1996 */

/* C (-h) output created by DEFNR */

#ifndef __EV6_DEFS_H__
#define __EV6_DEFS_H__


#define	EV6__DTBM_DOUBLE_3_ENTRY 0x100	
#define	EV6__DTBM_DOUBLE_4_ENTRY 0x180	
#define	EV6__FEN_ENTRY 0x200	
#define	EV6__UNALIGN_ENTRY 0x280	
#define	EV6__DTBM_SINGLE_ENTRY 0x300	
#define	EV6__DFAULT_ENTRY 0x380	
#define	EV6__OPCDEC_ENTRY 0x400	
#define	EV6__IACV_ENTRY 0x480	
#define	EV6__MCHK_ENTRY 0x500	
#define	EV6__ITB_MISS_ENTRY 0x580	
#define	EV6__ARITH_ENTRY 0x600	
#define	EV6__INTERRUPT_ENTRY 0x680	
#define	EV6__MT_FPCR_ENTRY 0x700	
#define	EV6__RESET_ENTRY 0x780	
#define	EV6__HALT_ENTRY 0x2000	
#define	EV6__CALL_PAL_00_ENTRY 0x2000	
#define	EV6__CALL_PAL_01_ENTRY 0x2040	
#define	EV6__CALL_PAL_02_ENTRY 0x2080	
#define	EV6__CALL_PAL_03_ENTRY 0x20c0	
#define	EV6__CALL_PAL_04_ENTRY 0x2100	
#define	EV6__CALL_PAL_05_ENTRY 0x2140	
#define	EV6__CALL_PAL_06_ENTRY 0x2180	
#define	EV6__CALL_PAL_07_ENTRY 0x21c0	
#define	EV6__CALL_PAL_08_ENTRY 0x2200	
#define	EV6__CALL_PAL_09_ENTRY 0x2240	
#define	EV6__CALL_PAL_0A_ENTRY 0x2280	
#define	EV6__CALL_PAL_0B_ENTRY 0x22c0	
#define	EV6__CALL_PAL_0C_ENTRY 0x2300	
#define	EV6__CALL_PAL_0D_ENTRY 0x2340	
#define	EV6__CALL_PAL_0E_ENTRY 0x2380	
#define	EV6__CALL_PAL_0F_ENTRY 0x23c0	
#define	EV6__CALL_PAL_10_ENTRY 0x2400	
#define	EV6__CALL_PAL_11_ENTRY 0x2440	
#define	EV6__CALL_PAL_12_ENTRY 0x2480	
#define	EV6__CALL_PAL_13_ENTRY 0x24c0	
#define	EV6__CALL_PAL_14_ENTRY 0x2500	
#define	EV6__CALL_PAL_15_ENTRY 0x2540	
#define	EV6__CALL_PAL_16_ENTRY 0x2580	
#define	EV6__CALL_PAL_17_ENTRY 0x25c0	
#define	EV6__CALL_PAL_18_ENTRY 0x2600	
#define	EV6__CALL_PAL_19_ENTRY 0x2640	
#define	EV6__CALL_PAL_1A_ENTRY 0x2680	
#define	EV6__CALL_PAL_1B_ENTRY 0x26c0	
#define	EV6__CALL_PAL_1C_ENTRY 0x2700	
#define	EV6__CALL_PAL_1D_ENTRY 0x2740	
#define	EV6__CALL_PAL_1E_ENTRY 0x2780	
#define	EV6__CALL_PAL_1F_ENTRY 0x27c0	
#define	EV6__CALL_PAL_20_ENTRY 0x2800	
#define	EV6__CALL_PAL_21_ENTRY 0x2840	
#define	EV6__CALL_PAL_22_ENTRY 0x2880	
#define	EV6__CALL_PAL_23_ENTRY 0x28c0	
#define	EV6__CALL_PAL_24_ENTRY 0x2900	
#define	EV6__CALL_PAL_25_ENTRY 0x2940	
#define	EV6__CALL_PAL_26_ENTRY 0x2980	
#define	EV6__CALL_PAL_27_ENTRY 0x29c0	
#define	EV6__CALL_PAL_28_ENTRY 0x2a00	
#define	EV6__CALL_PAL_29_ENTRY 0x2a40	
#define	EV6__CALL_PAL_2A_ENTRY 0x2a80	
#define	EV6__CALL_PAL_2B_ENTRY 0x2ac0	
#define	EV6__CALL_PAL_2C_ENTRY 0x2b00	
#define	EV6__CALL_PAL_2D_ENTRY 0x2b40	
#define	EV6__CALL_PAL_2E_ENTRY 0x2b80	
#define	EV6__CALL_PAL_2F_ENTRY 0x2bc0	
#define	EV6__CALL_PAL_30_ENTRY 0x2c00	
#define	EV6__CALL_PAL_31_ENTRY 0x2c40	
#define	EV6__CALL_PAL_32_ENTRY 0x2c80	
#define	EV6__CALL_PAL_33_ENTRY 0x2cc0	
#define	EV6__CALL_PAL_34_ENTRY 0x2d00	
#define	EV6__CALL_PAL_35_ENTRY 0x2d40	
#define	EV6__CALL_PAL_36_ENTRY 0x2d80	
#define	EV6__CALL_PAL_37_ENTRY 0x2dc0	
#define	EV6__CALL_PAL_38_ENTRY 0x2e00	
#define	EV6__CALL_PAL_39_ENTRY 0x2e40	
#define	EV6__CALL_PAL_3A_ENTRY 0x2e80	
#define	EV6__CALL_PAL_3B_ENTRY 0x2ec0	
#define	EV6__CALL_PAL_3C_ENTRY 0x2f00	
#define	EV6__CALL_PAL_3D_ENTRY 0x2f40	
#define	EV6__CALL_PAL_3E_ENTRY 0x2f80	
#define	EV6__CALL_PAL_3F_ENTRY 0x2fc0	
#define	EV6__CALL_PAL_80_ENTRY 0x3000	
#define	EV6__CALL_PAL_81_ENTRY 0x3040	
#define	EV6__CALL_PAL_82_ENTRY 0x3080	
#define	EV6__CALL_PAL_83_ENTRY 0x30c0	
#define	EV6__CALL_PAL_84_ENTRY 0x3100	
#define	EV6__CALL_PAL_85_ENTRY 0x3140	
#define	EV6__CALL_PAL_86_ENTRY 0x3180	
#define	EV6__CALL_PAL_87_ENTRY 0x31c0	
#define	EV6__CALL_PAL_88_ENTRY 0x3200	
#define	EV6__CALL_PAL_89_ENTRY 0x3240	
#define	EV6__CALL_PAL_8A_ENTRY 0x3280	
#define	EV6__CALL_PAL_8B_ENTRY 0x32c0	
#define	EV6__CALL_PAL_8C_ENTRY 0x3300	
#define	EV6__CALL_PAL_8D_ENTRY 0x3340	
#define	EV6__CALL_PAL_8E_ENTRY 0x3380	
#define	EV6__CALL_PAL_8F_ENTRY 0x33c0	
#define	EV6__CALL_PAL_90_ENTRY 0x3400	
#define	EV6__CALL_PAL_91_ENTRY 0x3440	
#define	EV6__CALL_PAL_92_ENTRY 0x3480	
#define	EV6__CALL_PAL_93_ENTRY 0x34c0	
#define	EV6__CALL_PAL_94_ENTRY 0x3500	
#define	EV6__CALL_PAL_95_ENTRY 0x3540	
#define	EV6__CALL_PAL_96_ENTRY 0x3580	
#define	EV6__CALL_PAL_97_ENTRY 0x35c0	
#define	EV6__CALL_PAL_98_ENTRY 0x3600	
#define	EV6__CALL_PAL_99_ENTRY 0x3640	
#define	EV6__CALL_PAL_9A_ENTRY 0x3680	
#define	EV6__CALL_PAL_9B_ENTRY 0x36c0	
#define	EV6__CALL_PAL_9C_ENTRY 0x3700	
#define	EV6__CALL_PAL_9D_ENTRY 0x3740	
#define	EV6__CALL_PAL_9E_ENTRY 0x3780	
#define	EV6__CALL_PAL_9F_ENTRY 0x37c0	
#define	EV6__CALL_PAL_A0_ENTRY 0x3800	
#define	EV6__CALL_PAL_A1_ENTRY 0x3840	
#define	EV6__CALL_PAL_A2_ENTRY 0x3880	
#define	EV6__CALL_PAL_A3_ENTRY 0x38c0	
#define	EV6__CALL_PAL_A4_ENTRY 0x3900	
#define	EV6__CALL_PAL_A5_ENTRY 0x3940	
#define	EV6__CALL_PAL_A6_ENTRY 0x3980	
#define	EV6__CALL_PAL_A7_ENTRY 0x39c0	
#define	EV6__CALL_PAL_A8_ENTRY 0x3a00	
#define	EV6__CALL_PAL_A9_ENTRY 0x3a40	
#define	EV6__CALL_PAL_AA_ENTRY 0x3a80	
#define	EV6__CALL_PAL_AB_ENTRY 0x3ac0	
#define	EV6__CALL_PAL_AC_ENTRY 0x3b00	
#define	EV6__CALL_PAL_AD_ENTRY 0x3b40	
#define	EV6__CALL_PAL_AE_ENTRY 0x3b80	
#define	EV6__CALL_PAL_AF_ENTRY 0x3bc0	
#define	EV6__CALL_PAL_B0_ENTRY 0x3c00	
#define	EV6__CALL_PAL_B1_ENTRY 0x3c40	
#define	EV6__CALL_PAL_B2_ENTRY 0x3c80	
#define	EV6__CALL_PAL_B3_ENTRY 0x3cc0	
#define	EV6__CALL_PAL_B4_ENTRY 0x3d00	
#define	EV6__CALL_PAL_B5_ENTRY 0x3d40	
#define	EV6__CALL_PAL_B6_ENTRY 0x3d80	
#define	EV6__CALL_PAL_B7_ENTRY 0x3dc0	
#define	EV6__CALL_PAL_B8_ENTRY 0x3e00	
#define	EV6__CALL_PAL_B9_ENTRY 0x3e40	
#define	EV6__CALL_PAL_BA_ENTRY 0x3e80	
#define	EV6__CALL_PAL_BB_ENTRY 0x3ec0	
#define	EV6__CALL_PAL_BC_ENTRY 0x3f00	
#define	EV6__CALL_PAL_BD_ENTRY 0x3f40	
#define	EV6__CALL_PAL_BE_ENTRY 0x3f80	
#define	EV6__CALL_PAL_BF_ENTRY 0x3fc0	

#define	EV6__CC 0xc0	
#define	EV6__CC__COUNTER_BOT__S 0x0	
#define	EV6__CC__COUNTER_BOT__V 0x4	
#define	EV6__CC__COUNTER_BOT__M 0xf	
#define	EV6__CC__COUNTER__S 0x4	
#define	EV6__CC__COUNTER__V 0x1c	
#define	EV6__CC__COUNTER__M 0xfffffff	
#define	EV6__CC__OFFSET__S 0x20	
#define	EV6__CC__OFFSET__V 0x20	
#define	EV6__CC__OFFSET__M 0xffffffff	

#define	EV6__CC_CTL 0xc1	
#define	EV6__CC_CTL__RSV1__S 0x0	
#define	EV6__CC_CTL__RSV1__V 0x4	
#define	EV6__CC_CTL__RSV1__M 0xf	
#define	EV6__CC_CTL__COUNTER__S 0x4	
#define	EV6__CC_CTL__COUNTER__V 0x1c	
#define	EV6__CC_CTL__COUNTER__M 0xfffffff	
#define	EV6__CC_CTL__CC_ENA__S 0x20	
#define	EV6__CC_CTL__CC_ENA__V 0x1	
#define	EV6__CC_CTL__CC_ENA__M 0x1	
#define	EV6__CC_CTL__RSV2__S 0x21	
#define	EV6__CC_CTL__RSV2__V 0x1f	
#define	EV6__CC_CTL__RSV2__M 0x7fffffff	

#define	EV6__VA 0xc2	
#define	EV6__VA__ADDR__S 0x0	
#define	EV6__VA__ADDR__V 0x40	
#define	EV6__VA__ADDR__M 0xffffffffffffffff	

#define	EV6__VA_FORM 0xc3	
#define	EV6__VA_FORM__RSV1__S 0x0	
#define	EV6__VA_FORM__RSV1__V 0x3	
#define	EV6__VA_FORM__RSV1__M 0x7	
#define	EV6__VA_FORM__VA__S 0x3	
#define	EV6__VA_FORM__VA__V 0x1e	
#define	EV6__VA_FORM__VA__M 0x3fffffff	
#define	EV6__VA_FORM__VPTB__S 0x21	
#define	EV6__VA_FORM__VPTB__V 0x1f	
#define	EV6__VA_FORM__VPTB__M 0x7fffffff	

#define	EV6__VA_FORM_48 0xc3	
#define	EV6__VA_FORM_48__RSV1__S 0x0	
#define	EV6__VA_FORM_48__RSV1__V 0x3	
#define	EV6__VA_FORM_48__RSV1__M 0x7	
#define	EV6__VA_FORM_48__VA__S 0x3	
#define	EV6__VA_FORM_48__VA__V 0x23	
#define	EV6__VA_FORM_48__VA__M 0x7ffffffff	
#define	EV6__VA_FORM_48__VA_SEXT0__S 0x26	
#define	EV6__VA_FORM_48__VA_SEXT0__V 0x1	
#define	EV6__VA_FORM_48__VA_SEXT0__M 0x1	
#define	EV6__VA_FORM_48__VA_SEXT1__S 0x27	
#define	EV6__VA_FORM_48__VA_SEXT1__V 0x1	
#define	EV6__VA_FORM_48__VA_SEXT1__M 0x1	
#define	EV6__VA_FORM_48__VA_SEXT2__S 0x28	
#define	EV6__VA_FORM_48__VA_SEXT2__V 0x1	
#define	EV6__VA_FORM_48__VA_SEXT2__M 0x1	
#define	EV6__VA_FORM_48__VA_SEXT3__S 0x29	
#define	EV6__VA_FORM_48__VA_SEXT3__V 0x1	
#define	EV6__VA_FORM_48__VA_SEXT3__M 0x1	
#define	EV6__VA_FORM_48__VA_SEXT4__S 0x2a	
#define	EV6__VA_FORM_48__VA_SEXT4__V 0x1	
#define	EV6__VA_FORM_48__VA_SEXT4__M 0x1	
#define	EV6__VA_FORM_48__VPTB__S 0x2b	
#define	EV6__VA_FORM_48__VPTB__V 0x15	
#define	EV6__VA_FORM_48__VPTB__M 0x1fffff	

#define	EV6__VA_FORM_32 0xc3	
#define	EV6__VA_FORM_32__RSV1__S 0x0	
#define	EV6__VA_FORM_32__RSV1__V 0x3	
#define	EV6__VA_FORM_32__RSV1__M 0x7	
#define	EV6__VA_FORM_32__VA__S 0x3	
#define	EV6__VA_FORM_32__VA__V 0x13	
#define	EV6__VA_FORM_32__VA__M 0x7ffff	
#define	EV6__VA_FORM_32__RSV2__S 0x16	
#define	EV6__VA_FORM_32__RSV2__V 0x8	
#define	EV6__VA_FORM_32__RSV2__M 0xff	
#define	EV6__VA_FORM_32__VPTB__S 0x1e	
#define	EV6__VA_FORM_32__VPTB__V 0x22	
#define	EV6__VA_FORM_32__VPTB__M 0x3ffffffff	

#define	EV6__VA_CTL 0xc4	
#define	EV6__VA_CTL__B_ENDIAN__S 0x0	
#define	EV6__VA_CTL__B_ENDIAN__V 0x1	
#define	EV6__VA_CTL__B_ENDIAN__M 0x1	
#define	EV6__VA_CTL__VA_48__S 0x1	
#define	EV6__VA_CTL__VA_48__V 0x1	
#define	EV6__VA_CTL__VA_48__M 0x1	
#define	EV6__VA_CTL__VA_FORM_32__S 0x2	
#define	EV6__VA_CTL__VA_FORM_32__V 0x1	
#define	EV6__VA_CTL__VA_FORM_32__M 0x1	
#define	EV6__VA_CTL__RSV1__S 0x3	
#define	EV6__VA_CTL__RSV1__V 0x1b	
#define	EV6__VA_CTL__RSV1__M 0x7ffffff	
#define	EV6__VA_CTL__VPTB__S 0x1e	
#define	EV6__VA_CTL__VPTB__V 0x22	
#define	EV6__VA_CTL__VPTB__M 0x3ffffffff	

#define	EV6__ITB_TAG 0x0	
#define	EV6__ITB_TAG__RSV1__S 0x0	
#define	EV6__ITB_TAG__RSV1__V 0xd	
#define	EV6__ITB_TAG__RSV1__M 0x1fff	
#define	EV6__ITB_TAG__VA__S 0xd	
#define	EV6__ITB_TAG__VA__V 0x23	
#define	EV6__ITB_TAG__VA__M 0x7ffffffff	
#define	EV6__ITB_TAG__RSV2__S 0x30	
#define	EV6__ITB_TAG__RSV2__V 0x10	
#define	EV6__ITB_TAG__RSV2__M 0xffff	

#define	EV6__ITB_PTE 0x1	
#define	EV6__ITB_PTE__RSV1__S 0x0	
#define	EV6__ITB_PTE__RSV1__V 0x4	
#define	EV6__ITB_PTE__RSV1__M 0xf	
#define	EV6__ITB_PTE__ASM__S 0x4	
#define	EV6__ITB_PTE__ASM__V 0x1	
#define	EV6__ITB_PTE__ASM__M 0x1	
#define	EV6__ITB_PTE__GH__S 0x5	
#define	EV6__ITB_PTE__GH__V 0x2	
#define	EV6__ITB_PTE__GH__M 0x3	
#define	EV6__ITB_PTE__RSV2__S 0x7	
#define	EV6__ITB_PTE__RSV2__V 0x1	
#define	EV6__ITB_PTE__RSV2__M 0x1	
#define	EV6__ITB_PTE__KRE__S 0x8	
#define	EV6__ITB_PTE__KRE__V 0x1	
#define	EV6__ITB_PTE__KRE__M 0x1	
#define	EV6__ITB_PTE__ERE__S 0x9	
#define	EV6__ITB_PTE__ERE__V 0x1	
#define	EV6__ITB_PTE__ERE__M 0x1	
#define	EV6__ITB_PTE__SRE__S 0xa	
#define	EV6__ITB_PTE__SRE__V 0x1	
#define	EV6__ITB_PTE__SRE__M 0x1	
#define	EV6__ITB_PTE__URE__S 0xb	
#define	EV6__ITB_PTE__URE__V 0x1	
#define	EV6__ITB_PTE__URE__M 0x1	
#define	EV6__ITB_PTE__RSV3__S 0xc	
#define	EV6__ITB_PTE__RSV3__V 0x1	
#define	EV6__ITB_PTE__RSV3__M 0x1	
#define	EV6__ITB_PTE__PFN__S 0xd	
#define	EV6__ITB_PTE__PFN__V 0x1f	
#define	EV6__ITB_PTE__PFN__M 0x7fffffff	
#define	EV6__ITB_PTE__RSV4__S 0x2c	
#define	EV6__ITB_PTE__RSV4__V 0x14	
#define	EV6__ITB_PTE__RSV4__M 0xfffff	

#define	EV6__ITB_IAP 0x2	
#define	EV6__ITB_IAP__RSV__S 0x0	
#define	EV6__ITB_IAP__RSV__V 0x40	
#define	EV6__ITB_IAP__RSV__M 0xffffffffffffffff	

#define	EV6__ITB_IA 0x3	
#define	EV6__ITB_IA__RSV__S 0x0	
#define	EV6__ITB_IA__RSV__V 0x40	
#define	EV6__ITB_IA__RSV__M 0xffffffffffffffff	

#define	EV6__ITB_IS 0x4	
#define	EV6__ITB_IS__RSV1__S 0x0	
#define	EV6__ITB_IS__RSV1__V 0xd	
#define	EV6__ITB_IS__RSV1__M 0x1fff	
#define	EV6__ITB_IS__VA__S 0xd	
#define	EV6__ITB_IS__VA__V 0x23	
#define	EV6__ITB_IS__VA__M 0x7ffffffff	
#define	EV6__ITB_IS__RSV2__S 0x30	
#define	EV6__ITB_IS__RSV2__V 0x10	
#define	EV6__ITB_IS__RSV2__M 0xffff	

#define	EV6__EXC_ADDR 0x6	
#define	EV6__EXC_ADDR__PAL__S 0x0	
#define	EV6__EXC_ADDR__PAL__V 0x1	
#define	EV6__EXC_ADDR__PAL__M 0x1	
#define	EV6__EXC_ADDR__RSV__S 0x1	
#define	EV6__EXC_ADDR__RSV__V 0x1	
#define	EV6__EXC_ADDR__RSV__M 0x1	
#define	EV6__EXC_ADDR__PC__S 0x2	
#define	EV6__EXC_ADDR__PC__V 0x2e	
#define	EV6__EXC_ADDR__PC__M 0x3fffffffffff	
#define	EV6__EXC_ADDR__PC_SEXT0__S 0x30	
#define	EV6__EXC_ADDR__PC_SEXT0__V 0x1	
#define	EV6__EXC_ADDR__PC_SEXT0__M 0x1	
#define	EV6__EXC_ADDR__PC_SEXT1__S 0x31	
#define	EV6__EXC_ADDR__PC_SEXT1__V 0x1	
#define	EV6__EXC_ADDR__PC_SEXT1__M 0x1	
#define	EV6__EXC_ADDR__PC_SEXT2__S 0x32	
#define	EV6__EXC_ADDR__PC_SEXT2__V 0x1	
#define	EV6__EXC_ADDR__PC_SEXT2__M 0x1	
#define	EV6__EXC_ADDR__PC_SEXT3__S 0x33	
#define	EV6__EXC_ADDR__PC_SEXT3__V 0x1	
#define	EV6__EXC_ADDR__PC_SEXT3__M 0x1	
#define	EV6__EXC_ADDR__PC_SEXT4__S 0x34	
#define	EV6__EXC_ADDR__PC_SEXT4__V 0x1	
#define	EV6__EXC_ADDR__PC_SEXT4__M 0x1	
#define	EV6__EXC_ADDR__PC_SEXT5__S 0x35	
#define	EV6__EXC_ADDR__PC_SEXT5__V 0x1	
#define	EV6__EXC_ADDR__PC_SEXT5__M 0x1	
#define	EV6__EXC_ADDR__PC_SEXT6__S 0x36	
#define	EV6__EXC_ADDR__PC_SEXT6__V 0x1	
#define	EV6__EXC_ADDR__PC_SEXT6__M 0x1	
#define	EV6__EXC_ADDR__PC_SEXT7__S 0x37	
#define	EV6__EXC_ADDR__PC_SEXT7__V 0x1	
#define	EV6__EXC_ADDR__PC_SEXT7__M 0x1	
#define	EV6__EXC_ADDR__PC_SEXT8__S 0x38	
#define	EV6__EXC_ADDR__PC_SEXT8__V 0x1	
#define	EV6__EXC_ADDR__PC_SEXT8__M 0x1	
#define	EV6__EXC_ADDR__PC_SEXT9__S 0x39	
#define	EV6__EXC_ADDR__PC_SEXT9__V 0x1	
#define	EV6__EXC_ADDR__PC_SEXT9__M 0x1	
#define	EV6__EXC_ADDR__PC_SEXT10__S 0x3a	
#define	EV6__EXC_ADDR__PC_SEXT10__V 0x1	
#define	EV6__EXC_ADDR__PC_SEXT10__M 0x1	
#define	EV6__EXC_ADDR__PC_SEXT11__S 0x3b	
#define	EV6__EXC_ADDR__PC_SEXT11__V 0x1	
#define	EV6__EXC_ADDR__PC_SEXT11__M 0x1	
#define	EV6__EXC_ADDR__PC_SEXT12__S 0x3c	
#define	EV6__EXC_ADDR__PC_SEXT12__V 0x1	
#define	EV6__EXC_ADDR__PC_SEXT12__M 0x1	
#define	EV6__EXC_ADDR__PC_SEXT13__S 0x3d	
#define	EV6__EXC_ADDR__PC_SEXT13__V 0x1	
#define	EV6__EXC_ADDR__PC_SEXT13__M 0x1	
#define	EV6__EXC_ADDR__PC_SEXT14__S 0x3e	
#define	EV6__EXC_ADDR__PC_SEXT14__V 0x1	
#define	EV6__EXC_ADDR__PC_SEXT14__M 0x1	
#define	EV6__EXC_ADDR__PC_SEXT15__S 0x3f	
#define	EV6__EXC_ADDR__PC_SEXT15__V 0x1	
#define	EV6__EXC_ADDR__PC_SEXT15__M 0x1	

#define	EV6__IVA_FORM 0x7	
#define	EV6__IVA_FORM__RSV1__S 0x0	
#define	EV6__IVA_FORM__RSV1__V 0x3	
#define	EV6__IVA_FORM__RSV1__M 0x7	
#define	EV6__IVA_FORM__VA__S 0x3	
#define	EV6__IVA_FORM__VA__V 0x1e	
#define	EV6__IVA_FORM__VA__M 0x3fffffff	
#define	EV6__IVA_FORM__VPTB__S 0x21	
#define	EV6__IVA_FORM__VPTB__V 0x1f	
#define	EV6__IVA_FORM__VPTB__M 0x7fffffff	

#define	EV6__IVA_FORM_48 0x7	
#define	EV6__IVA_FORM_48__RSV1__S 0x0	
#define	EV6__IVA_FORM_48__RSV1__V 0x3	
#define	EV6__IVA_FORM_48__RSV1__M 0x7	
#define	EV6__IVA_FORM_48__VA__S 0x3	
#define	EV6__IVA_FORM_48__VA__V 0x23	
#define	EV6__IVA_FORM_48__VA__M 0x7ffffffff	
#define	EV6__IVA_FORM_48__VA_SEXT0__S 0x26	
#define	EV6__IVA_FORM_48__VA_SEXT0__V 0x1	
#define	EV6__IVA_FORM_48__VA_SEXT0__M 0x1	
#define	EV6__IVA_FORM_48__VA_SEXT1__S 0x27	
#define	EV6__IVA_FORM_48__VA_SEXT1__V 0x1	
#define	EV6__IVA_FORM_48__VA_SEXT1__M 0x1	
#define	EV6__IVA_FORM_48__VA_SEXT2__S 0x28	
#define	EV6__IVA_FORM_48__VA_SEXT2__V 0x1	
#define	EV6__IVA_FORM_48__VA_SEXT2__M 0x1	
#define	EV6__IVA_FORM_48__VA_SEXT3__S 0x29	
#define	EV6__IVA_FORM_48__VA_SEXT3__V 0x1	
#define	EV6__IVA_FORM_48__VA_SEXT3__M 0x1	
#define	EV6__IVA_FORM_48__VA_SEXT4__S 0x2a	
#define	EV6__IVA_FORM_48__VA_SEXT4__V 0x1	
#define	EV6__IVA_FORM_48__VA_SEXT4__M 0x1	
#define	EV6__IVA_FORM_48__VPTB__S 0x2b	
#define	EV6__IVA_FORM_48__VPTB__V 0x15	
#define	EV6__IVA_FORM_48__VPTB__M 0x1fffff	

#define	EV6__IVA_FORM_32 0x7	
#define	EV6__IVA_FORM_32__RSV1__S 0x0	
#define	EV6__IVA_FORM_32__RSV1__V 0x3	
#define	EV6__IVA_FORM_32__RSV1__M 0x7	
#define	EV6__IVA_FORM_32__VA__S 0x3	
#define	EV6__IVA_FORM_32__VA__V 0x13	
#define	EV6__IVA_FORM_32__VA__M 0x7ffff	
#define	EV6__IVA_FORM_32__RSV2__S 0x16	
#define	EV6__IVA_FORM_32__RSV2__V 0x8	
#define	EV6__IVA_FORM_32__RSV2__M 0xff	
#define	EV6__IVA_FORM_32__VPTB__S 0x1e	
#define	EV6__IVA_FORM_32__VPTB__V 0x22	
#define	EV6__IVA_FORM_32__VPTB__M 0x3ffffffff	

#define	EV6__PS 0x9	
#define	EV6__PS__RSV0__S 0x0	
#define	EV6__PS__RSV0__V 0x3	
#define	EV6__PS__RSV0__M 0x7	
#define	EV6__PS__CM__S 0x3	
#define	EV6__PS__CM__V 0x2	
#define	EV6__PS__CM__M 0x3	
#define	EV6__PS__RSV1__S 0x5	
#define	EV6__PS__RSV1__V 0x8	
#define	EV6__PS__RSV1__M 0xff	
#define	EV6__PS__ASTEN__S 0xd	
#define	EV6__PS__ASTEN__V 0x1	
#define	EV6__PS__ASTEN__M 0x1	
#define	EV6__PS__SIEN__S 0xe	
#define	EV6__PS__SIEN__V 0xf	
#define	EV6__PS__SIEN__M 0x7fff	
#define	EV6__PS__PCEN__S 0x1d	
#define	EV6__PS__PCEN__V 0x2	
#define	EV6__PS__PCEN__M 0x3	
#define	EV6__PS__CREN__S 0x1f	
#define	EV6__PS__CREN__V 0x1	
#define	EV6__PS__CREN__M 0x1	
#define	EV6__PS__SLEN__S 0x20	
#define	EV6__PS__SLEN__V 0x1	
#define	EV6__PS__SLEN__M 0x1	
#define	EV6__PS__EIEN__S 0x21	
#define	EV6__PS__EIEN__V 0x6	
#define	EV6__PS__EIEN__M 0x3f	
#define	EV6__PS__RSV2__S 0x27	
#define	EV6__PS__RSV2__V 0x19	
#define	EV6__PS__RSV2__M 0x1ffffff	

#define	EV6__IER 0xa	
#define	EV6__IER__RSV0__S 0x0	
#define	EV6__IER__RSV0__V 0x3	
#define	EV6__IER__RSV0__M 0x7	
#define	EV6__IER__CM__S 0x3	
#define	EV6__IER__CM__V 0x2	
#define	EV6__IER__CM__M 0x3	
#define	EV6__IER__RSV1__S 0x5	
#define	EV6__IER__RSV1__V 0x8	
#define	EV6__IER__RSV1__M 0xff	
#define	EV6__IER__ASTEN__S 0xd	
#define	EV6__IER__ASTEN__V 0x1	
#define	EV6__IER__ASTEN__M 0x1	
#define	EV6__IER__SIEN__S 0xe	
#define	EV6__IER__SIEN__V 0xf	
#define	EV6__IER__SIEN__M 0x7fff	
#define	EV6__IER__PCEN__S 0x1d	
#define	EV6__IER__PCEN__V 0x2	
#define	EV6__IER__PCEN__M 0x3	
#define	EV6__IER__CREN__S 0x1f	
#define	EV6__IER__CREN__V 0x1	
#define	EV6__IER__CREN__M 0x1	
#define	EV6__IER__SLEN__S 0x20	
#define	EV6__IER__SLEN__V 0x1	
#define	EV6__IER__SLEN__M 0x1	
#define	EV6__IER__EIEN__S 0x21	
#define	EV6__IER__EIEN__V 0x6	
#define	EV6__IER__EIEN__M 0x3f	
#define	EV6__IER__RSV2__S 0x27	
#define	EV6__IER__RSV2__V 0x19	
#define	EV6__IER__RSV2__M 0x1ffffff	

#define	EV6__IER_CM 0xb	
#define	EV6__IER_CM__RSV0__S 0x0	
#define	EV6__IER_CM__RSV0__V 0x3	
#define	EV6__IER_CM__RSV0__M 0x7	
#define	EV6__IER_CM__CM__S 0x3	
#define	EV6__IER_CM__CM__V 0x2	
#define	EV6__IER_CM__CM__M 0x3	
#define	EV6__IER_CM__RSV1__S 0x5	
#define	EV6__IER_CM__RSV1__V 0x8	
#define	EV6__IER_CM__RSV1__M 0xff	
#define	EV6__IER_CM__ASTEN__S 0xd	
#define	EV6__IER_CM__ASTEN__V 0x1	
#define	EV6__IER_CM__ASTEN__M 0x1	
#define	EV6__IER_CM__SIEN__S 0xe	
#define	EV6__IER_CM__SIEN__V 0xf	
#define	EV6__IER_CM__SIEN__M 0x7fff	
#define	EV6__IER_CM__PCEN__S 0x1d	
#define	EV6__IER_CM__PCEN__V 0x2	
#define	EV6__IER_CM__PCEN__M 0x3	
#define	EV6__IER_CM__CREN__S 0x1f	
#define	EV6__IER_CM__CREN__V 0x1	
#define	EV6__IER_CM__CREN__M 0x1	
#define	EV6__IER_CM__SLEN__S 0x20	
#define	EV6__IER_CM__SLEN__V 0x1	
#define	EV6__IER_CM__SLEN__M 0x1	
#define	EV6__IER_CM__EIEN__S 0x21	
#define	EV6__IER_CM__EIEN__V 0x6	
#define	EV6__IER_CM__EIEN__M 0x3f	
#define	EV6__IER_CM__RSV2__S 0x27	
#define	EV6__IER_CM__RSV2__V 0x19	
#define	EV6__IER_CM__RSV2__M 0x1ffffff	

#define	EV6__SIRR 0xc	
#define	EV6__SIRR__RSV1__S 0x0	
#define	EV6__SIRR__RSV1__V 0xe	
#define	EV6__SIRR__RSV1__M 0x3fff	
#define	EV6__SIRR__SIR__S 0xe	
#define	EV6__SIRR__SIR__V 0xf	
#define	EV6__SIRR__SIR__M 0x7fff	
#define	EV6__SIRR__RSV2__S 0x1d	
#define	EV6__SIRR__RSV2__V 0x23	
#define	EV6__SIRR__RSV2__M 0x7ffffffff	

#define	EV6__ISUM 0xd	
#define	EV6__ISUM__RSV0__S 0x0	
#define	EV6__ISUM__RSV0__V 0x3	
#define	EV6__ISUM__RSV0__M 0x7	
#define	EV6__ISUM__ASTK__S 0x3	
#define	EV6__ISUM__ASTK__V 0x1	
#define	EV6__ISUM__ASTK__M 0x1	
#define	EV6__ISUM__ASTE__S 0x4	
#define	EV6__ISUM__ASTE__V 0x1	
#define	EV6__ISUM__ASTE__M 0x1	
#define	EV6__ISUM__RSV1__S 0x5	
#define	EV6__ISUM__RSV1__V 0x4	
#define	EV6__ISUM__RSV1__M 0xf	
#define	EV6__ISUM__ASTS__S 0x9	
#define	EV6__ISUM__ASTS__V 0x1	
#define	EV6__ISUM__ASTS__M 0x1	
#define	EV6__ISUM__ASTU__S 0xa	
#define	EV6__ISUM__ASTU__V 0x1	
#define	EV6__ISUM__ASTU__M 0x1	
#define	EV6__ISUM__RSV2__S 0xb	
#define	EV6__ISUM__RSV2__V 0x3	
#define	EV6__ISUM__RSV2__M 0x7	
#define	EV6__ISUM__SI__S 0xe	
#define	EV6__ISUM__SI__V 0xf	
#define	EV6__ISUM__SI__M 0x7fff	
#define	EV6__ISUM__PC__S 0x1d	
#define	EV6__ISUM__PC__V 0x2	
#define	EV6__ISUM__PC__M 0x3	
#define	EV6__ISUM__CR__S 0x1f	
#define	EV6__ISUM__CR__V 0x1	
#define	EV6__ISUM__CR__M 0x1	
#define	EV6__ISUM__SL__S 0x20	
#define	EV6__ISUM__SL__V 0x1	
#define	EV6__ISUM__SL__M 0x1	
#define	EV6__ISUM__EI__S 0x21	
#define	EV6__ISUM__EI__V 0x6	
#define	EV6__ISUM__EI__M 0x3f	
#define	EV6__ISUM__RSV3__S 0x27	
#define	EV6__ISUM__RSV3__V 0x19	
#define	EV6__ISUM__RSV3__M 0x1ffffff	

#define	EV6__HW_INT_CLR 0xe	
#define	EV6__HW_INT_CLR__RSV1__S 0x0	
#define	EV6__HW_INT_CLR__RSV1__V 0x1a	
#define	EV6__HW_INT_CLR__RSV1__M 0x3ffffff	
#define	EV6__HW_INT_CLR__FBTP__S 0x1a	
#define	EV6__HW_INT_CLR__FBTP__V 0x1	
#define	EV6__HW_INT_CLR__FBTP__M 0x1	
#define	EV6__HW_INT_CLR__FBDP__S 0x1b	
#define	EV6__HW_INT_CLR__FBDP__V 0x1	
#define	EV6__HW_INT_CLR__FBDP__M 0x1	
#define	EV6__HW_INT_CLR__MCHK_D__S 0x1c	
#define	EV6__HW_INT_CLR__MCHK_D__V 0x1	
#define	EV6__HW_INT_CLR__MCHK_D__M 0x1	
#define	EV6__HW_INT_CLR__PC__S 0x1d	
#define	EV6__HW_INT_CLR__PC__V 0x2	
#define	EV6__HW_INT_CLR__PC__M 0x3	
#define	EV6__HW_INT_CLR__CR__S 0x1f	
#define	EV6__HW_INT_CLR__CR__V 0x1	
#define	EV6__HW_INT_CLR__CR__M 0x1	
#define	EV6__HW_INT_CLR__SL__S 0x20	
#define	EV6__HW_INT_CLR__SL__V 0x1	
#define	EV6__HW_INT_CLR__SL__M 0x1	
#define	EV6__HW_INT_CLR__RSV2__S 0x21	
#define	EV6__HW_INT_CLR__RSV2__V 0x1f	
#define	EV6__HW_INT_CLR__RSV2__M 0x7fffffff	

#define	EV6__EXC_SUM 0xf	
#define	EV6__EXC_SUM__SWC__S 0x0	
#define	EV6__EXC_SUM__SWC__V 0x1	
#define	EV6__EXC_SUM__SWC__M 0x1	
#define	EV6__EXC_SUM__INV__S 0x1	
#define	EV6__EXC_SUM__INV__V 0x1	
#define	EV6__EXC_SUM__INV__M 0x1	
#define	EV6__EXC_SUM__DZE__S 0x2	
#define	EV6__EXC_SUM__DZE__V 0x1	
#define	EV6__EXC_SUM__DZE__M 0x1	
#define	EV6__EXC_SUM__FOV__S 0x3	
#define	EV6__EXC_SUM__FOV__V 0x1	
#define	EV6__EXC_SUM__FOV__M 0x1	
#define	EV6__EXC_SUM__UNF__S 0x4	
#define	EV6__EXC_SUM__UNF__V 0x1	
#define	EV6__EXC_SUM__UNF__M 0x1	
#define	EV6__EXC_SUM__INE__S 0x5	
#define	EV6__EXC_SUM__INE__V 0x1	
#define	EV6__EXC_SUM__INE__M 0x1	
#define	EV6__EXC_SUM__IOV__S 0x6	
#define	EV6__EXC_SUM__IOV__V 0x1	
#define	EV6__EXC_SUM__IOV__M 0x1	
#define	EV6__EXC_SUM__INT__S 0x7	
#define	EV6__EXC_SUM__INT__V 0x1	
#define	EV6__EXC_SUM__INT__M 0x1	
#define	EV6__EXC_SUM__REG__S 0x8	
#define	EV6__EXC_SUM__REG__V 0x5	
#define	EV6__EXC_SUM__REG__M 0x1f	
#define	EV6__EXC_SUM__BAD_IVA__S 0xd	
#define	EV6__EXC_SUM__BAD_IVA__V 0x1	
#define	EV6__EXC_SUM__BAD_IVA__M 0x1	
#define	EV6__EXC_SUM__RSV1__S 0xe	
#define	EV6__EXC_SUM__RSV1__V 0x1b	
#define	EV6__EXC_SUM__RSV1__M 0x7ffffff	
#define	EV6__EXC_SUM__PC_OVFL__S 0x29	
#define	EV6__EXC_SUM__PC_OVFL__V 0x1	
#define	EV6__EXC_SUM__PC_OVFL__M 0x1	
#define	EV6__EXC_SUM__SET_INV__S 0x2a	
#define	EV6__EXC_SUM__SET_INV__V 0x1	
#define	EV6__EXC_SUM__SET_INV__M 0x1	
#define	EV6__EXC_SUM__SET_DZE__S 0x2b	
#define	EV6__EXC_SUM__SET_DZE__V 0x1	
#define	EV6__EXC_SUM__SET_DZE__M 0x1	
#define	EV6__EXC_SUM__SET_FOV__S 0x2c	
#define	EV6__EXC_SUM__SET_FOV__V 0x1	
#define	EV6__EXC_SUM__SET_FOV__M 0x1	
#define	EV6__EXC_SUM__SET_UNF__S 0x2d	
#define	EV6__EXC_SUM__SET_UNF__V 0x1	
#define	EV6__EXC_SUM__SET_UNF__M 0x1	
#define	EV6__EXC_SUM__SET_INE__S 0x2e	
#define	EV6__EXC_SUM__SET_INE__V 0x1	
#define	EV6__EXC_SUM__SET_INE__M 0x1	
#define	EV6__EXC_SUM__SET_IOV__S 0x2f	
#define	EV6__EXC_SUM__SET_IOV__V 0x1	
#define	EV6__EXC_SUM__SET_IOV__M 0x1	
#define	EV6__EXC_SUM__SEXT0__S 0x30	
#define	EV6__EXC_SUM__SEXT0__V 0x1	
#define	EV6__EXC_SUM__SEXT0__M 0x1	
#define	EV6__EXC_SUM__SEXT1__S 0x31	
#define	EV6__EXC_SUM__SEXT1__V 0x1	
#define	EV6__EXC_SUM__SEXT1__M 0x1	
#define	EV6__EXC_SUM__SEXT2__S 0x32	
#define	EV6__EXC_SUM__SEXT2__V 0x1	
#define	EV6__EXC_SUM__SEXT2__M 0x1	
#define	EV6__EXC_SUM__SEXT3__S 0x33	
#define	EV6__EXC_SUM__SEXT3__V 0x1	
#define	EV6__EXC_SUM__SEXT3__M 0x1	
#define	EV6__EXC_SUM__SEXT4__S 0x34	
#define	EV6__EXC_SUM__SEXT4__V 0x1	
#define	EV6__EXC_SUM__SEXT4__M 0x1	
#define	EV6__EXC_SUM__SEXT5__S 0x35	
#define	EV6__EXC_SUM__SEXT5__V 0x1	
#define	EV6__EXC_SUM__SEXT5__M 0x1	
#define	EV6__EXC_SUM__SEXT6__S 0x36	
#define	EV6__EXC_SUM__SEXT6__V 0x1	
#define	EV6__EXC_SUM__SEXT6__M 0x1	
#define	EV6__EXC_SUM__SEXT7__S 0x37	
#define	EV6__EXC_SUM__SEXT7__V 0x1	
#define	EV6__EXC_SUM__SEXT7__M 0x1	
#define	EV6__EXC_SUM__SEXT8__S 0x38	
#define	EV6__EXC_SUM__SEXT8__V 0x1	
#define	EV6__EXC_SUM__SEXT8__M 0x1	
#define	EV6__EXC_SUM__SEXT9__S 0x39	
#define	EV6__EXC_SUM__SEXT9__V 0x1	
#define	EV6__EXC_SUM__SEXT9__M 0x1	
#define	EV6__EXC_SUM__SEXT10__S 0x3a	
#define	EV6__EXC_SUM__SEXT10__V 0x1	
#define	EV6__EXC_SUM__SEXT10__M 0x1	
#define	EV6__EXC_SUM__SEXT11__S 0x3b	
#define	EV6__EXC_SUM__SEXT11__V 0x1	
#define	EV6__EXC_SUM__SEXT11__M 0x1	
#define	EV6__EXC_SUM__SEXT12__S 0x3c	
#define	EV6__EXC_SUM__SEXT12__V 0x1	
#define	EV6__EXC_SUM__SEXT12__M 0x1	
#define	EV6__EXC_SUM__SEXT13__S 0x3d	
#define	EV6__EXC_SUM__SEXT13__V 0x1	
#define	EV6__EXC_SUM__SEXT13__M 0x1	
#define	EV6__EXC_SUM__SEXT14__S 0x3e	
#define	EV6__EXC_SUM__SEXT14__V 0x1	
#define	EV6__EXC_SUM__SEXT14__M 0x1	
#define	EV6__EXC_SUM__SEXT15__S 0x3f	
#define	EV6__EXC_SUM__SEXT15__V 0x1	
#define	EV6__EXC_SUM__SEXT15__M 0x1	

#define	EV6__PAL_BASE 0x10	
#define	EV6__PAL_BASE__RSV1__S 0x0	
#define	EV6__PAL_BASE__RSV1__V 0xf	
#define	EV6__PAL_BASE__RSV1__M 0x7fff	
#define	EV6__PAL_BASE__PAL_BASE__S 0xf	
#define	EV6__PAL_BASE__PAL_BASE__V 0x1d	
#define	EV6__PAL_BASE__PAL_BASE__M 0x1fffffff	
#define	EV6__PAL_BASE__RSV2__S 0x2c	
#define	EV6__PAL_BASE__RSV2__V 0x14	
#define	EV6__PAL_BASE__RSV2__M 0xfffff	

#define	EV6__I_CTL 0x11	
#define	EV6__I_CTL__SPCE__S 0x0	
#define	EV6__I_CTL__SPCE__V 0x1	
#define	EV6__I_CTL__SPCE__M 0x1	
#define	EV6__I_CTL__IC_EN__S 0x1	
#define	EV6__I_CTL__IC_EN__V 0x2	
#define	EV6__I_CTL__IC_EN__M 0x3	
#define	EV6__I_CTL__SPE__S 0x3	
#define	EV6__I_CTL__SPE__V 0x3	
#define	EV6__I_CTL__SPE__M 0x7	
#define	EV6__I_CTL__SDE__S 0x6	
#define	EV6__I_CTL__SDE__V 0x2	
#define	EV6__I_CTL__SDE__M 0x3	
#define	EV6__I_CTL__SBE__S 0x8	
#define	EV6__I_CTL__SBE__V 0x2	
#define	EV6__I_CTL__SBE__M 0x3	
#define	EV6__I_CTL__BP_MODE__S 0xa	
#define	EV6__I_CTL__BP_MODE__V 0x2	
#define	EV6__I_CTL__BP_MODE__M 0x3	
#define	EV6__I_CTL__HWE__S 0xc	
#define	EV6__I_CTL__HWE__V 0x1	
#define	EV6__I_CTL__HWE__M 0x1	
#define	EV6__I_CTL__SL_XMIT__S 0xd	
#define	EV6__I_CTL__SL_XMIT__V 0x1	
#define	EV6__I_CTL__SL_XMIT__M 0x1	
#define	EV6__I_CTL__SL_RCV__S 0xe	
#define	EV6__I_CTL__SL_RCV__V 0x1	
#define	EV6__I_CTL__SL_RCV__M 0x1	
#define	EV6__I_CTL__VA_48__S 0xf	
#define	EV6__I_CTL__VA_48__V 0x1	
#define	EV6__I_CTL__VA_48__M 0x1	
#define	EV6__I_CTL__VA_FORM_32__S 0x10	
#define	EV6__I_CTL__VA_FORM_32__V 0x1	
#define	EV6__I_CTL__VA_FORM_32__M 0x1	
#define	EV6__I_CTL__SINGLE_ISSUE_L__S 0x11	
#define	EV6__I_CTL__SINGLE_ISSUE_L__V 0x1	
#define	EV6__I_CTL__SINGLE_ISSUE_L__M 0x1	
#define	EV6__I_CTL__PCT0_EN__S 0x12	
#define	EV6__I_CTL__PCT0_EN__V 0x1	
#define	EV6__I_CTL__PCT0_EN__M 0x1	
#define	EV6__I_CTL__PCT1_EN__S 0x13	
#define	EV6__I_CTL__PCT1_EN__V 0x1	
#define	EV6__I_CTL__PCT1_EN__M 0x1	
#define	EV6__I_CTL__CALL_PAL_R23__S 0x14	
#define	EV6__I_CTL__CALL_PAL_R23__V 0x1	
#define	EV6__I_CTL__CALL_PAL_R23__M 0x1	
#define	EV6__I_CTL__MCHK_EN__S 0x15	
#define	EV6__I_CTL__MCHK_EN__V 0x1	
#define	EV6__I_CTL__MCHK_EN__M 0x1	
#define	EV6__I_CTL__TB_MB_EN__S 0x16	
#define	EV6__I_CTL__TB_MB_EN__V 0x1	
#define	EV6__I_CTL__TB_MB_EN__M 0x1	
#define	EV6__I_CTL__BIST_FAIL__S 0x17	
#define	EV6__I_CTL__BIST_FAIL__V 0x1	
#define	EV6__I_CTL__BIST_FAIL__M 0x1	
#define	EV6__I_CTL__RSV1__S 0x18	
#define	EV6__I_CTL__RSV1__V 0x6	
#define	EV6__I_CTL__RSV1__M 0x3f	
#define	EV6__I_CTL__VPTB__S 0x1e	
#define	EV6__I_CTL__VPTB__V 0x12	
#define	EV6__I_CTL__VPTB__M 0x3ffff	
#define	EV6__I_CTL__VPTB_SEXT0__S 0x30	
#define	EV6__I_CTL__VPTB_SEXT0__V 0x1	
#define	EV6__I_CTL__VPTB_SEXT0__M 0x1	
#define	EV6__I_CTL__VPTB_SEXT1__S 0x31	
#define	EV6__I_CTL__VPTB_SEXT1__V 0x1	
#define	EV6__I_CTL__VPTB_SEXT1__M 0x1	
#define	EV6__I_CTL__VPTB_SEXT2__S 0x32	
#define	EV6__I_CTL__VPTB_SEXT2__V 0x1	
#define	EV6__I_CTL__VPTB_SEXT2__M 0x1	
#define	EV6__I_CTL__VPTB_SEXT3__S 0x33	
#define	EV6__I_CTL__VPTB_SEXT3__V 0x1	
#define	EV6__I_CTL__VPTB_SEXT3__M 0x1	
#define	EV6__I_CTL__VPTB_SEXT4__S 0x34	
#define	EV6__I_CTL__VPTB_SEXT4__V 0x1	
#define	EV6__I_CTL__VPTB_SEXT4__M 0x1	
#define	EV6__I_CTL__VPTB_SEXT5__S 0x35	
#define	EV6__I_CTL__VPTB_SEXT5__V 0x1	
#define	EV6__I_CTL__VPTB_SEXT5__M 0x1	
#define	EV6__I_CTL__VPTB_SEXT6__S 0x36	
#define	EV6__I_CTL__VPTB_SEXT6__V 0x1	
#define	EV6__I_CTL__VPTB_SEXT6__M 0x1	
#define	EV6__I_CTL__VPTB_SEXT7__S 0x37	
#define	EV6__I_CTL__VPTB_SEXT7__V 0x1	
#define	EV6__I_CTL__VPTB_SEXT7__M 0x1	
#define	EV6__I_CTL__VPTB_SEXT8__S 0x38	
#define	EV6__I_CTL__VPTB_SEXT8__V 0x1	
#define	EV6__I_CTL__VPTB_SEXT8__M 0x1	
#define	EV6__I_CTL__VPTB_SEXT9__S 0x39	
#define	EV6__I_CTL__VPTB_SEXT9__V 0x1	
#define	EV6__I_CTL__VPTB_SEXT9__M 0x1	
#define	EV6__I_CTL__VPTB_SEXT10__S 0x3a	
#define	EV6__I_CTL__VPTB_SEXT10__V 0x1	
#define	EV6__I_CTL__VPTB_SEXT10__M 0x1	
#define	EV6__I_CTL__VPTB_SEXT11__S 0x3b	
#define	EV6__I_CTL__VPTB_SEXT11__V 0x1	
#define	EV6__I_CTL__VPTB_SEXT11__M 0x1	
#define	EV6__I_CTL__VPTB_SEXT12__S 0x3c	
#define	EV6__I_CTL__VPTB_SEXT12__V 0x1	
#define	EV6__I_CTL__VPTB_SEXT12__M 0x1	
#define	EV6__I_CTL__VPTB_SEXT13__S 0x3d	
#define	EV6__I_CTL__VPTB_SEXT13__V 0x1	
#define	EV6__I_CTL__VPTB_SEXT13__M 0x1	
#define	EV6__I_CTL__VPTB_SEXT14__S 0x3e	
#define	EV6__I_CTL__VPTB_SEXT14__V 0x1	
#define	EV6__I_CTL__VPTB_SEXT14__M 0x1	
#define	EV6__I_CTL__VPTB_SEXT15__S 0x3f	
#define	EV6__I_CTL__VPTB_SEXT15__V 0x1	
#define	EV6__I_CTL__VPTB_SEXT15__M 0x1	

#define	EV6__IC_FLUSH_ASM 0x12	
#define	EV6__IC_FLUSH_ASM__RSV__S 0x0	
#define	EV6__IC_FLUSH_ASM__RSV__V 0x40	
#define	EV6__IC_FLUSH_ASM__RSV__M 0xffffffffffffffff	

#define	EV6__IC_FLUSH 0x13	
#define	EV6__IC_FLUSH__RSV__S 0x0	
#define	EV6__IC_FLUSH__RSV__V 0x40	
#define	EV6__IC_FLUSH__RSV__M 0xffffffffffffffff	

#define	EV6__PCTR_CTL 0x14	
#define	EV6__PCTR_CTL__SL1__S 0x0	
#define	EV6__PCTR_CTL__SL1__V 0x4	
#define	EV6__PCTR_CTL__SL1__M 0xf	
#define	EV6__PCTR_CTL__SL0__S 0x4	
#define	EV6__PCTR_CTL__SL0__V 0x1	
#define	EV6__PCTR_CTL__SL0__M 0x1	
#define	EV6__PCTR_CTL__RSV1__S 0x5	
#define	EV6__PCTR_CTL__RSV1__V 0x1	
#define	EV6__PCTR_CTL__RSV1__M 0x1	
#define	EV6__PCTR_CTL__PCTR1__S 0x6	
#define	EV6__PCTR_CTL__PCTR1__V 0x14	
#define	EV6__PCTR_CTL__PCTR1__M 0xfffff	
#define	EV6__PCTR_CTL__RSV2__S 0x1a	
#define	EV6__PCTR_CTL__RSV2__V 0x2	
#define	EV6__PCTR_CTL__RSV2__M 0x3	
#define	EV6__PCTR_CTL__PCTR0__S 0x1c	
#define	EV6__PCTR_CTL__PCTR0__V 0x14	
#define	EV6__PCTR_CTL__PCTR0__M 0xfffff	
#define	EV6__PCTR_CTL__RSV3__S 0x30	
#define	EV6__PCTR_CTL__RSV3__V 0x10	
#define	EV6__PCTR_CTL__RSV3__M 0xffff	

#define	EV6__CLR_MAP 0x15	
#define	EV6__CLR_MAP__RSV__S 0x0	
#define	EV6__CLR_MAP__RSV__V 0x40	
#define	EV6__CLR_MAP__RSV__M 0xffffffffffffffff	

#define	EV6__I_STAT 0x16	
#define	EV6__I_STAT__RSV1__S 0x0	
#define	EV6__I_STAT__RSV1__V 0x1d	
#define	EV6__I_STAT__RSV1__M 0x1fffffff	
#define	EV6__I_STAT__TPE__S 0x1d	
#define	EV6__I_STAT__TPE__V 0x1	
#define	EV6__I_STAT__TPE__M 0x1	
#define	EV6__I_STAT__DPE__S 0x1e	
#define	EV6__I_STAT__DPE__V 0x1	
#define	EV6__I_STAT__DPE__M 0x1	
#define	EV6__I_STAT__RSV2__S 0x1f	
#define	EV6__I_STAT__RSV2__V 0x21	
#define	EV6__I_STAT__RSV2__M 0x1ffffffff	

#define	EV6__ASN 0x41	
#define	EV6__ASN__RSV0__S 0x0	
#define	EV6__ASN__RSV0__V 0x1	
#define	EV6__ASN__RSV0__M 0x1	
#define	EV6__ASN__PPCE__S 0x1	
#define	EV6__ASN__PPCE__V 0x1	
#define	EV6__ASN__PPCE__M 0x1	
#define	EV6__ASN__FPE__S 0x2	
#define	EV6__ASN__FPE__V 0x1	
#define	EV6__ASN__FPE__M 0x1	
#define	EV6__ASN__RSV1__S 0x3	
#define	EV6__ASN__RSV1__V 0x2	
#define	EV6__ASN__RSV1__M 0x3	
#define	EV6__ASN__ASTER__S 0x5	
#define	EV6__ASN__ASTER__V 0x4	
#define	EV6__ASN__ASTER__M 0xf	
#define	EV6__ASN__ASTRR__S 0x9	
#define	EV6__ASN__ASTRR__V 0x4	
#define	EV6__ASN__ASTRR__M 0xf	
#define	EV6__ASN__RSV2__S 0xd	
#define	EV6__ASN__RSV2__V 0x1a	
#define	EV6__ASN__RSV2__M 0x3ffffff	
#define	EV6__ASN__ASN__S 0x27	
#define	EV6__ASN__ASN__V 0x8	
#define	EV6__ASN__ASN__M 0xff	
#define	EV6__ASN__RSV3__S 0x2f	
#define	EV6__ASN__RSV3__V 0x11	
#define	EV6__ASN__RSV3__M 0x1ffff	

#define	EV6__ASTER 0x42	
#define	EV6__ASTER__RSV0__S 0x0	
#define	EV6__ASTER__RSV0__V 0x1	
#define	EV6__ASTER__RSV0__M 0x1	
#define	EV6__ASTER__PPCE__S 0x1	
#define	EV6__ASTER__PPCE__V 0x1	
#define	EV6__ASTER__PPCE__M 0x1	
#define	EV6__ASTER__FPE__S 0x2	
#define	EV6__ASTER__FPE__V 0x1	
#define	EV6__ASTER__FPE__M 0x1	
#define	EV6__ASTER__RSV1__S 0x3	
#define	EV6__ASTER__RSV1__V 0x2	
#define	EV6__ASTER__RSV1__M 0x3	
#define	EV6__ASTER__ASTER__S 0x5	
#define	EV6__ASTER__ASTER__V 0x4	
#define	EV6__ASTER__ASTER__M 0xf	
#define	EV6__ASTER__ASTRR__S 0x9	
#define	EV6__ASTER__ASTRR__V 0x4	
#define	EV6__ASTER__ASTRR__M 0xf	
#define	EV6__ASTER__RSV2__S 0xd	
#define	EV6__ASTER__RSV2__V 0x1a	
#define	EV6__ASTER__RSV2__M 0x3ffffff	
#define	EV6__ASTER__ASN__S 0x27	
#define	EV6__ASTER__ASN__V 0x8	
#define	EV6__ASTER__ASN__M 0xff	
#define	EV6__ASTER__RSV3__S 0x2f	
#define	EV6__ASTER__RSV3__V 0x11	
#define	EV6__ASTER__RSV3__M 0x1ffff	

#define	EV6__ASTRR 0x44	
#define	EV6__ASTRR__RSV0__S 0x0	
#define	EV6__ASTRR__RSV0__V 0x1	
#define	EV6__ASTRR__RSV0__M 0x1	
#define	EV6__ASTRR__PPCE__S 0x1	
#define	EV6__ASTRR__PPCE__V 0x1	
#define	EV6__ASTRR__PPCE__M 0x1	
#define	EV6__ASTRR__FPE__S 0x2	
#define	EV6__ASTRR__FPE__V 0x1	
#define	EV6__ASTRR__FPE__M 0x1	
#define	EV6__ASTRR__RSV1__S 0x3	
#define	EV6__ASTRR__RSV1__V 0x2	
#define	EV6__ASTRR__RSV1__M 0x3	
#define	EV6__ASTRR__ASTER__S 0x5	
#define	EV6__ASTRR__ASTER__V 0x4	
#define	EV6__ASTRR__ASTER__M 0xf	
#define	EV6__ASTRR__ASTRR__S 0x9	
#define	EV6__ASTRR__ASTRR__V 0x4	
#define	EV6__ASTRR__ASTRR__M 0xf	
#define	EV6__ASTRR__RSV2__S 0xd	
#define	EV6__ASTRR__RSV2__V 0x1a	
#define	EV6__ASTRR__RSV2__M 0x3ffffff	
#define	EV6__ASTRR__ASN__S 0x27	
#define	EV6__ASTRR__ASN__V 0x8	
#define	EV6__ASTRR__ASN__M 0xff	
#define	EV6__ASTRR__RSV3__S 0x2f	
#define	EV6__ASTRR__RSV3__V 0x11	
#define	EV6__ASTRR__RSV3__M 0x1ffff	

#define	EV6__PPCE 0x48	
#define	EV6__PPCE__RSV0__S 0x0	
#define	EV6__PPCE__RSV0__V 0x1	
#define	EV6__PPCE__RSV0__M 0x1	
#define	EV6__PPCE__PPCE__S 0x1	
#define	EV6__PPCE__PPCE__V 0x1	
#define	EV6__PPCE__PPCE__M 0x1	
#define	EV6__PPCE__FPE__S 0x2	
#define	EV6__PPCE__FPE__V 0x1	
#define	EV6__PPCE__FPE__M 0x1	
#define	EV6__PPCE__RSV1__S 0x3	
#define	EV6__PPCE__RSV1__V 0x2	
#define	EV6__PPCE__RSV1__M 0x3	
#define	EV6__PPCE__ASTER__S 0x5	
#define	EV6__PPCE__ASTER__V 0x4	
#define	EV6__PPCE__ASTER__M 0xf	
#define	EV6__PPCE__ASTRR__S 0x9	
#define	EV6__PPCE__ASTRR__V 0x4	
#define	EV6__PPCE__ASTRR__M 0xf	
#define	EV6__PPCE__RSV2__S 0xd	
#define	EV6__PPCE__RSV2__V 0x1a	
#define	EV6__PPCE__RSV2__M 0x3ffffff	
#define	EV6__PPCE__ASN__S 0x27	
#define	EV6__PPCE__ASN__V 0x8	
#define	EV6__PPCE__ASN__M 0xff	
#define	EV6__PPCE__RSV3__S 0x2f	
#define	EV6__PPCE__RSV3__V 0x11	
#define	EV6__PPCE__RSV3__M 0x1ffff	

#define	EV6__FPE 0x50	
#define	EV6__FPE__RSV0__S 0x0	
#define	EV6__FPE__RSV0__V 0x1	
#define	EV6__FPE__RSV0__M 0x1	
#define	EV6__FPE__PPCE__S 0x1	
#define	EV6__FPE__PPCE__V 0x1	
#define	EV6__FPE__PPCE__M 0x1	
#define	EV6__FPE__FPE__S 0x2	
#define	EV6__FPE__FPE__V 0x1	
#define	EV6__FPE__FPE__M 0x1	
#define	EV6__FPE__RSV1__S 0x3	
#define	EV6__FPE__RSV1__V 0x2	
#define	EV6__FPE__RSV1__M 0x3	
#define	EV6__FPE__ASTER__S 0x5	
#define	EV6__FPE__ASTER__V 0x4	
#define	EV6__FPE__ASTER__M 0xf	
#define	EV6__FPE__ASTRR__S 0x9	
#define	EV6__FPE__ASTRR__V 0x4	
#define	EV6__FPE__ASTRR__M 0xf	
#define	EV6__FPE__RSV2__S 0xd	
#define	EV6__FPE__RSV2__V 0x1a	
#define	EV6__FPE__RSV2__M 0x3ffffff	
#define	EV6__FPE__ASN__S 0x27	
#define	EV6__FPE__ASN__V 0x8	
#define	EV6__FPE__ASN__M 0xff	
#define	EV6__FPE__RSV3__S 0x2f	
#define	EV6__FPE__RSV3__V 0x11	
#define	EV6__FPE__RSV3__M 0x1ffff	

#define	EV6__ASN_ASTER 0x43	
#define	EV6__ASN_ASTER__RSV0__S 0x0	
#define	EV6__ASN_ASTER__RSV0__V 0x1	
#define	EV6__ASN_ASTER__RSV0__M 0x1	
#define	EV6__ASN_ASTER__PPCE__S 0x1	
#define	EV6__ASN_ASTER__PPCE__V 0x1	
#define	EV6__ASN_ASTER__PPCE__M 0x1	
#define	EV6__ASN_ASTER__FPE__S 0x2	
#define	EV6__ASN_ASTER__FPE__V 0x1	
#define	EV6__ASN_ASTER__FPE__M 0x1	
#define	EV6__ASN_ASTER__RSV1__S 0x3	
#define	EV6__ASN_ASTER__RSV1__V 0x2	
#define	EV6__ASN_ASTER__RSV1__M 0x3	
#define	EV6__ASN_ASTER__ASTER__S 0x5	
#define	EV6__ASN_ASTER__ASTER__V 0x4	
#define	EV6__ASN_ASTER__ASTER__M 0xf	
#define	EV6__ASN_ASTER__ASTRR__S 0x9	
#define	EV6__ASN_ASTER__ASTRR__V 0x4	
#define	EV6__ASN_ASTER__ASTRR__M 0xf	
#define	EV6__ASN_ASTER__RSV2__S 0xd	
#define	EV6__ASN_ASTER__RSV2__V 0x1a	
#define	EV6__ASN_ASTER__RSV2__M 0x3ffffff	
#define	EV6__ASN_ASTER__ASN__S 0x27	
#define	EV6__ASN_ASTER__ASN__V 0x8	
#define	EV6__ASN_ASTER__ASN__M 0xff	
#define	EV6__ASN_ASTER__RSV3__S 0x2f	
#define	EV6__ASN_ASTER__RSV3__V 0x11	
#define	EV6__ASN_ASTER__RSV3__M 0x1ffff	

#define	EV6__ASN_ASTRR 0x45	
#define	EV6__ASN_ASTRR__RSV0__S 0x0	
#define	EV6__ASN_ASTRR__RSV0__V 0x1	
#define	EV6__ASN_ASTRR__RSV0__M 0x1	
#define	EV6__ASN_ASTRR__PPCE__S 0x1	
#define	EV6__ASN_ASTRR__PPCE__V 0x1	
#define	EV6__ASN_ASTRR__PPCE__M 0x1	
#define	EV6__ASN_ASTRR__FPE__S 0x2	
#define	EV6__ASN_ASTRR__FPE__V 0x1	
#define	EV6__ASN_ASTRR__FPE__M 0x1	
#define	EV6__ASN_ASTRR__RSV1__S 0x3	
#define	EV6__ASN_ASTRR__RSV1__V 0x2	
#define	EV6__ASN_ASTRR__RSV1__M 0x3	
#define	EV6__ASN_ASTRR__ASTER__S 0x5	
#define	EV6__ASN_ASTRR__ASTER__V 0x4	
#define	EV6__ASN_ASTRR__ASTER__M 0xf	
#define	EV6__ASN_ASTRR__ASTRR__S 0x9	
#define	EV6__ASN_ASTRR__ASTRR__V 0x4	
#define	EV6__ASN_ASTRR__ASTRR__M 0xf	
#define	EV6__ASN_ASTRR__RSV2__S 0xd	
#define	EV6__ASN_ASTRR__RSV2__V 0x1a	
#define	EV6__ASN_ASTRR__RSV2__M 0x3ffffff	
#define	EV6__ASN_ASTRR__ASN__S 0x27	
#define	EV6__ASN_ASTRR__ASN__V 0x8	
#define	EV6__ASN_ASTRR__ASN__M 0xff	
#define	EV6__ASN_ASTRR__RSV3__S 0x2f	
#define	EV6__ASN_ASTRR__RSV3__V 0x11	
#define	EV6__ASN_ASTRR__RSV3__M 0x1ffff	

#define	EV6__ASN_PPCE 0x49	
#define	EV6__ASN_PPCE__RSV0__S 0x0	
#define	EV6__ASN_PPCE__RSV0__V 0x1	
#define	EV6__ASN_PPCE__RSV0__M 0x1	
#define	EV6__ASN_PPCE__PPCE__S 0x1	
#define	EV6__ASN_PPCE__PPCE__V 0x1	
#define	EV6__ASN_PPCE__PPCE__M 0x1	
#define	EV6__ASN_PPCE__FPE__S 0x2	
#define	EV6__ASN_PPCE__FPE__V 0x1	
#define	EV6__ASN_PPCE__FPE__M 0x1	
#define	EV6__ASN_PPCE__RSV1__S 0x3	
#define	EV6__ASN_PPCE__RSV1__V 0x2	
#define	EV6__ASN_PPCE__RSV1__M 0x3	
#define	EV6__ASN_PPCE__ASTER__S 0x5	
#define	EV6__ASN_PPCE__ASTER__V 0x4	
#define	EV6__ASN_PPCE__ASTER__M 0xf	
#define	EV6__ASN_PPCE__ASTRR__S 0x9	
#define	EV6__ASN_PPCE__ASTRR__V 0x4	
#define	EV6__ASN_PPCE__ASTRR__M 0xf	
#define	EV6__ASN_PPCE__RSV2__S 0xd	
#define	EV6__ASN_PPCE__RSV2__V 0x1a	
#define	EV6__ASN_PPCE__RSV2__M 0x3ffffff	
#define	EV6__ASN_PPCE__ASN__S 0x27	
#define	EV6__ASN_PPCE__ASN__V 0x8	
#define	EV6__ASN_PPCE__ASN__M 0xff	
#define	EV6__ASN_PPCE__RSV3__S 0x2f	
#define	EV6__ASN_PPCE__RSV3__V 0x11	
#define	EV6__ASN_PPCE__RSV3__M 0x1ffff	

#define	EV6__ASN_FPE 0x51	
#define	EV6__ASN_FPE__RSV0__S 0x0	
#define	EV6__ASN_FPE__RSV0__V 0x1	
#define	EV6__ASN_FPE__RSV0__M 0x1	
#define	EV6__ASN_FPE__PPCE__S 0x1	
#define	EV6__ASN_FPE__PPCE__V 0x1	
#define	EV6__ASN_FPE__PPCE__M 0x1	
#define	EV6__ASN_FPE__FPE__S 0x2	
#define	EV6__ASN_FPE__FPE__V 0x1	
#define	EV6__ASN_FPE__FPE__M 0x1	
#define	EV6__ASN_FPE__RSV1__S 0x3	
#define	EV6__ASN_FPE__RSV1__V 0x2	
#define	EV6__ASN_FPE__RSV1__M 0x3	
#define	EV6__ASN_FPE__ASTER__S 0x5	
#define	EV6__ASN_FPE__ASTER__V 0x4	
#define	EV6__ASN_FPE__ASTER__M 0xf	
#define	EV6__ASN_FPE__ASTRR__S 0x9	
#define	EV6__ASN_FPE__ASTRR__V 0x4	
#define	EV6__ASN_FPE__ASTRR__M 0xf	
#define	EV6__ASN_FPE__RSV2__S 0xd	
#define	EV6__ASN_FPE__RSV2__V 0x1a	
#define	EV6__ASN_FPE__RSV2__M 0x3ffffff	
#define	EV6__ASN_FPE__ASN__S 0x27	
#define	EV6__ASN_FPE__ASN__V 0x8	
#define	EV6__ASN_FPE__ASN__M 0xff	
#define	EV6__ASN_FPE__RSV3__S 0x2f	
#define	EV6__ASN_FPE__RSV3__V 0x11	
#define	EV6__ASN_FPE__RSV3__M 0x1ffff	

#define	EV6__ASTER_ASTRR 0x46	
#define	EV6__ASTER_ASTRR__RSV0__S 0x0	
#define	EV6__ASTER_ASTRR__RSV0__V 0x1	
#define	EV6__ASTER_ASTRR__RSV0__M 0x1	
#define	EV6__ASTER_ASTRR__PPCE__S 0x1	
#define	EV6__ASTER_ASTRR__PPCE__V 0x1	
#define	EV6__ASTER_ASTRR__PPCE__M 0x1	
#define	EV6__ASTER_ASTRR__FPE__S 0x2	
#define	EV6__ASTER_ASTRR__FPE__V 0x1	
#define	EV6__ASTER_ASTRR__FPE__M 0x1	
#define	EV6__ASTER_ASTRR__RSV1__S 0x3	
#define	EV6__ASTER_ASTRR__RSV1__V 0x2	
#define	EV6__ASTER_ASTRR__RSV1__M 0x3	
#define	EV6__ASTER_ASTRR__ASTER__S 0x5	
#define	EV6__ASTER_ASTRR__ASTER__V 0x4	
#define	EV6__ASTER_ASTRR__ASTER__M 0xf	
#define	EV6__ASTER_ASTRR__ASTRR__S 0x9	
#define	EV6__ASTER_ASTRR__ASTRR__V 0x4	
#define	EV6__ASTER_ASTRR__ASTRR__M 0xf	
#define	EV6__ASTER_ASTRR__RSV2__S 0xd	
#define	EV6__ASTER_ASTRR__RSV2__V 0x1a	
#define	EV6__ASTER_ASTRR__RSV2__M 0x3ffffff	
#define	EV6__ASTER_ASTRR__ASN__S 0x27	
#define	EV6__ASTER_ASTRR__ASN__V 0x8	
#define	EV6__ASTER_ASTRR__ASN__M 0xff	
#define	EV6__ASTER_ASTRR__RSV3__S 0x2f	
#define	EV6__ASTER_ASTRR__RSV3__V 0x11	
#define	EV6__ASTER_ASTRR__RSV3__M 0x1ffff	

#define	EV6__ASTER_PPCE 0x4a	
#define	EV6__ASTER_PPCE__RSV0__S 0x0	
#define	EV6__ASTER_PPCE__RSV0__V 0x1	
#define	EV6__ASTER_PPCE__RSV0__M 0x1	
#define	EV6__ASTER_PPCE__PPCE__S 0x1	
#define	EV6__ASTER_PPCE__PPCE__V 0x1	
#define	EV6__ASTER_PPCE__PPCE__M 0x1	
#define	EV6__ASTER_PPCE__FPE__S 0x2	
#define	EV6__ASTER_PPCE__FPE__V 0x1	
#define	EV6__ASTER_PPCE__FPE__M 0x1	
#define	EV6__ASTER_PPCE__RSV1__S 0x3	
#define	EV6__ASTER_PPCE__RSV1__V 0x2	
#define	EV6__ASTER_PPCE__RSV1__M 0x3	
#define	EV6__ASTER_PPCE__ASTER__S 0x5	
#define	EV6__ASTER_PPCE__ASTER__V 0x4	
#define	EV6__ASTER_PPCE__ASTER__M 0xf	
#define	EV6__ASTER_PPCE__ASTRR__S 0x9	
#define	EV6__ASTER_PPCE__ASTRR__V 0x4	
#define	EV6__ASTER_PPCE__ASTRR__M 0xf	
#define	EV6__ASTER_PPCE__RSV2__S 0xd	
#define	EV6__ASTER_PPCE__RSV2__V 0x1a	
#define	EV6__ASTER_PPCE__RSV2__M 0x3ffffff	
#define	EV6__ASTER_PPCE__ASN__S 0x27	
#define	EV6__ASTER_PPCE__ASN__V 0x8	
#define	EV6__ASTER_PPCE__ASN__M 0xff	
#define	EV6__ASTER_PPCE__RSV3__S 0x2f	
#define	EV6__ASTER_PPCE__RSV3__V 0x11	
#define	EV6__ASTER_PPCE__RSV3__M 0x1ffff	

#define	EV6__ASTER_FPE 0x52	
#define	EV6__ASTER_FPE__RSV0__S 0x0	
#define	EV6__ASTER_FPE__RSV0__V 0x1	
#define	EV6__ASTER_FPE__RSV0__M 0x1	
#define	EV6__ASTER_FPE__PPCE__S 0x1	
#define	EV6__ASTER_FPE__PPCE__V 0x1	
#define	EV6__ASTER_FPE__PPCE__M 0x1	
#define	EV6__ASTER_FPE__FPE__S 0x2	
#define	EV6__ASTER_FPE__FPE__V 0x1	
#define	EV6__ASTER_FPE__FPE__M 0x1	
#define	EV6__ASTER_FPE__RSV1__S 0x3	
#define	EV6__ASTER_FPE__RSV1__V 0x2	
#define	EV6__ASTER_FPE__RSV1__M 0x3	
#define	EV6__ASTER_FPE__ASTER__S 0x5	
#define	EV6__ASTER_FPE__ASTER__V 0x4	
#define	EV6__ASTER_FPE__ASTER__M 0xf	
#define	EV6__ASTER_FPE__ASTRR__S 0x9	
#define	EV6__ASTER_FPE__ASTRR__V 0x4	
#define	EV6__ASTER_FPE__ASTRR__M 0xf	
#define	EV6__ASTER_FPE__RSV2__S 0xd	
#define	EV6__ASTER_FPE__RSV2__V 0x1a	
#define	EV6__ASTER_FPE__RSV2__M 0x3ffffff	
#define	EV6__ASTER_FPE__ASN__S 0x27	
#define	EV6__ASTER_FPE__ASN__V 0x8	
#define	EV6__ASTER_FPE__ASN__M 0xff	
#define	EV6__ASTER_FPE__RSV3__S 0x2f	
#define	EV6__ASTER_FPE__RSV3__V 0x11	
#define	EV6__ASTER_FPE__RSV3__M 0x1ffff	

#define	EV6__ASTRR_PPCE 0x4c	
#define	EV6__ASTRR_PPCE__RSV0__S 0x0	
#define	EV6__ASTRR_PPCE__RSV0__V 0x1	
#define	EV6__ASTRR_PPCE__RSV0__M 0x1	
#define	EV6__ASTRR_PPCE__PPCE__S 0x1	
#define	EV6__ASTRR_PPCE__PPCE__V 0x1	
#define	EV6__ASTRR_PPCE__PPCE__M 0x1	
#define	EV6__ASTRR_PPCE__FPE__S 0x2	
#define	EV6__ASTRR_PPCE__FPE__V 0x1	
#define	EV6__ASTRR_PPCE__FPE__M 0x1	
#define	EV6__ASTRR_PPCE__RSV1__S 0x3	
#define	EV6__ASTRR_PPCE__RSV1__V 0x2	
#define	EV6__ASTRR_PPCE__RSV1__M 0x3	
#define	EV6__ASTRR_PPCE__ASTER__S 0x5	
#define	EV6__ASTRR_PPCE__ASTER__V 0x4	
#define	EV6__ASTRR_PPCE__ASTER__M 0xf	
#define	EV6__ASTRR_PPCE__ASTRR__S 0x9	
#define	EV6__ASTRR_PPCE__ASTRR__V 0x4	
#define	EV6__ASTRR_PPCE__ASTRR__M 0xf	
#define	EV6__ASTRR_PPCE__RSV2__S 0xd	
#define	EV6__ASTRR_PPCE__RSV2__V 0x1a	
#define	EV6__ASTRR_PPCE__RSV2__M 0x3ffffff	
#define	EV6__ASTRR_PPCE__ASN__S 0x27	
#define	EV6__ASTRR_PPCE__ASN__V 0x8	
#define	EV6__ASTRR_PPCE__ASN__M 0xff	
#define	EV6__ASTRR_PPCE__RSV3__S 0x2f	
#define	EV6__ASTRR_PPCE__RSV3__V 0x11	
#define	EV6__ASTRR_PPCE__RSV3__M 0x1ffff	

#define	EV6__ASTRR_FPE 0x54	
#define	EV6__ASTRR_FPE__RSV0__S 0x0	
#define	EV6__ASTRR_FPE__RSV0__V 0x1	
#define	EV6__ASTRR_FPE__RSV0__M 0x1	
#define	EV6__ASTRR_FPE__PPCE__S 0x1	
#define	EV6__ASTRR_FPE__PPCE__V 0x1	
#define	EV6__ASTRR_FPE__PPCE__M 0x1	
#define	EV6__ASTRR_FPE__FPE__S 0x2	
#define	EV6__ASTRR_FPE__FPE__V 0x1	
#define	EV6__ASTRR_FPE__FPE__M 0x1	
#define	EV6__ASTRR_FPE__RSV1__S 0x3	
#define	EV6__ASTRR_FPE__RSV1__V 0x2	
#define	EV6__ASTRR_FPE__RSV1__M 0x3	
#define	EV6__ASTRR_FPE__ASTER__S 0x5	
#define	EV6__ASTRR_FPE__ASTER__V 0x4	
#define	EV6__ASTRR_FPE__ASTER__M 0xf	
#define	EV6__ASTRR_FPE__ASTRR__S 0x9	
#define	EV6__ASTRR_FPE__ASTRR__V 0x4	
#define	EV6__ASTRR_FPE__ASTRR__M 0xf	
#define	EV6__ASTRR_FPE__RSV2__S 0xd	
#define	EV6__ASTRR_FPE__RSV2__V 0x1a	
#define	EV6__ASTRR_FPE__RSV2__M 0x3ffffff	
#define	EV6__ASTRR_FPE__ASN__S 0x27	
#define	EV6__ASTRR_FPE__ASN__V 0x8	
#define	EV6__ASTRR_FPE__ASN__M 0xff	
#define	EV6__ASTRR_FPE__RSV3__S 0x2f	
#define	EV6__ASTRR_FPE__RSV3__V 0x11	
#define	EV6__ASTRR_FPE__RSV3__M 0x1ffff	

#define	EV6__PPCE_FPE 0x58	
#define	EV6__PPCE_FPE__RSV0__S 0x0	
#define	EV6__PPCE_FPE__RSV0__V 0x1	
#define	EV6__PPCE_FPE__RSV0__M 0x1	
#define	EV6__PPCE_FPE__PPCE__S 0x1	
#define	EV6__PPCE_FPE__PPCE__V 0x1	
#define	EV6__PPCE_FPE__PPCE__M 0x1	
#define	EV6__PPCE_FPE__FPE__S 0x2	
#define	EV6__PPCE_FPE__FPE__V 0x1	
#define	EV6__PPCE_FPE__FPE__M 0x1	
#define	EV6__PPCE_FPE__RSV1__S 0x3	
#define	EV6__PPCE_FPE__RSV1__V 0x2	
#define	EV6__PPCE_FPE__RSV1__M 0x3	
#define	EV6__PPCE_FPE__ASTER__S 0x5	
#define	EV6__PPCE_FPE__ASTER__V 0x4	
#define	EV6__PPCE_FPE__ASTER__M 0xf	
#define	EV6__PPCE_FPE__ASTRR__S 0x9	
#define	EV6__PPCE_FPE__ASTRR__V 0x4	
#define	EV6__PPCE_FPE__ASTRR__M 0xf	
#define	EV6__PPCE_FPE__RSV2__S 0xd	
#define	EV6__PPCE_FPE__RSV2__V 0x1a	
#define	EV6__PPCE_FPE__RSV2__M 0x3ffffff	
#define	EV6__PPCE_FPE__ASN__S 0x27	
#define	EV6__PPCE_FPE__ASN__V 0x8	
#define	EV6__PPCE_FPE__ASN__M 0xff	
#define	EV6__PPCE_FPE__RSV3__S 0x2f	
#define	EV6__PPCE_FPE__RSV3__V 0x11	
#define	EV6__PPCE_FPE__RSV3__M 0x1ffff	

#define	EV6__ASN_ASTER_ASTRR 0x47	
#define	EV6__ASN_ASTER_ASTRR__RSV0__S 0x0	
#define	EV6__ASN_ASTER_ASTRR__RSV0__V 0x1	
#define	EV6__ASN_ASTER_ASTRR__RSV0__M 0x1	
#define	EV6__ASN_ASTER_ASTRR__PPCE__S 0x1	
#define	EV6__ASN_ASTER_ASTRR__PPCE__V 0x1	
#define	EV6__ASN_ASTER_ASTRR__PPCE__M 0x1	
#define	EV6__ASN_ASTER_ASTRR__FPE__S 0x2	
#define	EV6__ASN_ASTER_ASTRR__FPE__V 0x1	
#define	EV6__ASN_ASTER_ASTRR__FPE__M 0x1	
#define	EV6__ASN_ASTER_ASTRR__RSV1__S 0x3	
#define	EV6__ASN_ASTER_ASTRR__RSV1__V 0x2	
#define	EV6__ASN_ASTER_ASTRR__RSV1__M 0x3	
#define	EV6__ASN_ASTER_ASTRR__ASTER__S 0x5	
#define	EV6__ASN_ASTER_ASTRR__ASTER__V 0x4	
#define	EV6__ASN_ASTER_ASTRR__ASTER__M 0xf	
#define	EV6__ASN_ASTER_ASTRR__ASTRR__S 0x9	
#define	EV6__ASN_ASTER_ASTRR__ASTRR__V 0x4	
#define	EV6__ASN_ASTER_ASTRR__ASTRR__M 0xf	
#define	EV6__ASN_ASTER_ASTRR__RSV2__S 0xd	
#define	EV6__ASN_ASTER_ASTRR__RSV2__V 0x1a	
#define	EV6__ASN_ASTER_ASTRR__RSV2__M 0x3ffffff	
#define	EV6__ASN_ASTER_ASTRR__ASN__S 0x27	
#define	EV6__ASN_ASTER_ASTRR__ASN__V 0x8	
#define	EV6__ASN_ASTER_ASTRR__ASN__M 0xff	
#define	EV6__ASN_ASTER_ASTRR__RSV3__S 0x2f	
#define	EV6__ASN_ASTER_ASTRR__RSV3__V 0x11	
#define	EV6__ASN_ASTER_ASTRR__RSV3__M 0x1ffff	

#define	EV6__ASN_ASTER_PPCE 0x4b	
#define	EV6__ASN_ASTER_PPCE__RSV0__S 0x0	
#define	EV6__ASN_ASTER_PPCE__RSV0__V 0x1	
#define	EV6__ASN_ASTER_PPCE__RSV0__M 0x1	
#define	EV6__ASN_ASTER_PPCE__PPCE__S 0x1	
#define	EV6__ASN_ASTER_PPCE__PPCE__V 0x1	
#define	EV6__ASN_ASTER_PPCE__PPCE__M 0x1	
#define	EV6__ASN_ASTER_PPCE__FPE__S 0x2	
#define	EV6__ASN_ASTER_PPCE__FPE__V 0x1	
#define	EV6__ASN_ASTER_PPCE__FPE__M 0x1	
#define	EV6__ASN_ASTER_PPCE__RSV1__S 0x3	
#define	EV6__ASN_ASTER_PPCE__RSV1__V 0x2	
#define	EV6__ASN_ASTER_PPCE__RSV1__M 0x3	
#define	EV6__ASN_ASTER_PPCE__ASTER__S 0x5	
#define	EV6__ASN_ASTER_PPCE__ASTER__V 0x4	
#define	EV6__ASN_ASTER_PPCE__ASTER__M 0xf	
#define	EV6__ASN_ASTER_PPCE__ASTRR__S 0x9	
#define	EV6__ASN_ASTER_PPCE__ASTRR__V 0x4	
#define	EV6__ASN_ASTER_PPCE__ASTRR__M 0xf	
#define	EV6__ASN_ASTER_PPCE__RSV2__S 0xd	
#define	EV6__ASN_ASTER_PPCE__RSV2__V 0x1a	
#define	EV6__ASN_ASTER_PPCE__RSV2__M 0x3ffffff	
#define	EV6__ASN_ASTER_PPCE__ASN__S 0x27	
#define	EV6__ASN_ASTER_PPCE__ASN__V 0x8	
#define	EV6__ASN_ASTER_PPCE__ASN__M 0xff	
#define	EV6__ASN_ASTER_PPCE__RSV3__S 0x2f	
#define	EV6__ASN_ASTER_PPCE__RSV3__V 0x11	
#define	EV6__ASN_ASTER_PPCE__RSV3__M 0x1ffff	

#define	EV6__ASN_ASTER_FPE 0x53	
#define	EV6__ASN_ASTER_FPE__RSV0__S 0x0	
#define	EV6__ASN_ASTER_FPE__RSV0__V 0x1	
#define	EV6__ASN_ASTER_FPE__RSV0__M 0x1	
#define	EV6__ASN_ASTER_FPE__PPCE__S 0x1	
#define	EV6__ASN_ASTER_FPE__PPCE__V 0x1	
#define	EV6__ASN_ASTER_FPE__PPCE__M 0x1	
#define	EV6__ASN_ASTER_FPE__FPE__S 0x2	
#define	EV6__ASN_ASTER_FPE__FPE__V 0x1	
#define	EV6__ASN_ASTER_FPE__FPE__M 0x1	
#define	EV6__ASN_ASTER_FPE__RSV1__S 0x3	
#define	EV6__ASN_ASTER_FPE__RSV1__V 0x2	
#define	EV6__ASN_ASTER_FPE__RSV1__M 0x3	
#define	EV6__ASN_ASTER_FPE__ASTER__S 0x5	
#define	EV6__ASN_ASTER_FPE__ASTER__V 0x4	
#define	EV6__ASN_ASTER_FPE__ASTER__M 0xf	
#define	EV6__ASN_ASTER_FPE__ASTRR__S 0x9	
#define	EV6__ASN_ASTER_FPE__ASTRR__V 0x4	
#define	EV6__ASN_ASTER_FPE__ASTRR__M 0xf	
#define	EV6__ASN_ASTER_FPE__RSV2__S 0xd	
#define	EV6__ASN_ASTER_FPE__RSV2__V 0x1a	
#define	EV6__ASN_ASTER_FPE__RSV2__M 0x3ffffff	
#define	EV6__ASN_ASTER_FPE__ASN__S 0x27	
#define	EV6__ASN_ASTER_FPE__ASN__V 0x8	
#define	EV6__ASN_ASTER_FPE__ASN__M 0xff	
#define	EV6__ASN_ASTER_FPE__RSV3__S 0x2f	
#define	EV6__ASN_ASTER_FPE__RSV3__V 0x11	
#define	EV6__ASN_ASTER_FPE__RSV3__M 0x1ffff	

#define	EV6__ASN_ASTRR_PPCE 0x4d	
#define	EV6__ASN_ASTRR_PPCE__RSV0__S 0x0	
#define	EV6__ASN_ASTRR_PPCE__RSV0__V 0x1	
#define	EV6__ASN_ASTRR_PPCE__RSV0__M 0x1	
#define	EV6__ASN_ASTRR_PPCE__PPCE__S 0x1	
#define	EV6__ASN_ASTRR_PPCE__PPCE__V 0x1	
#define	EV6__ASN_ASTRR_PPCE__PPCE__M 0x1	
#define	EV6__ASN_ASTRR_PPCE__FPE__S 0x2	
#define	EV6__ASN_ASTRR_PPCE__FPE__V 0x1	
#define	EV6__ASN_ASTRR_PPCE__FPE__M 0x1	
#define	EV6__ASN_ASTRR_PPCE__RSV1__S 0x3	
#define	EV6__ASN_ASTRR_PPCE__RSV1__V 0x2	
#define	EV6__ASN_ASTRR_PPCE__RSV1__M 0x3	
#define	EV6__ASN_ASTRR_PPCE__ASTER__S 0x5	
#define	EV6__ASN_ASTRR_PPCE__ASTER__V 0x4	
#define	EV6__ASN_ASTRR_PPCE__ASTER__M 0xf	
#define	EV6__ASN_ASTRR_PPCE__ASTRR__S 0x9	
#define	EV6__ASN_ASTRR_PPCE__ASTRR__V 0x4	
#define	EV6__ASN_ASTRR_PPCE__ASTRR__M 0xf	
#define	EV6__ASN_ASTRR_PPCE__RSV2__S 0xd	
#define	EV6__ASN_ASTRR_PPCE__RSV2__V 0x1a	
#define	EV6__ASN_ASTRR_PPCE__RSV2__M 0x3ffffff	
#define	EV6__ASN_ASTRR_PPCE__ASN__S 0x27	
#define	EV6__ASN_ASTRR_PPCE__ASN__V 0x8	
#define	EV6__ASN_ASTRR_PPCE__ASN__M 0xff	
#define	EV6__ASN_ASTRR_PPCE__RSV3__S 0x2f	
#define	EV6__ASN_ASTRR_PPCE__RSV3__V 0x11	
#define	EV6__ASN_ASTRR_PPCE__RSV3__M 0x1ffff	

#define	EV6__ASN_ASTRR_FPE 0x55	
#define	EV6__ASN_ASTRR_FPE__RSV0__S 0x0	
#define	EV6__ASN_ASTRR_FPE__RSV0__V 0x1	
#define	EV6__ASN_ASTRR_FPE__RSV0__M 0x1	
#define	EV6__ASN_ASTRR_FPE__PPCE__S 0x1	
#define	EV6__ASN_ASTRR_FPE__PPCE__V 0x1	
#define	EV6__ASN_ASTRR_FPE__PPCE__M 0x1	
#define	EV6__ASN_ASTRR_FPE__FPE__S 0x2	
#define	EV6__ASN_ASTRR_FPE__FPE__V 0x1	
#define	EV6__ASN_ASTRR_FPE__FPE__M 0x1	
#define	EV6__ASN_ASTRR_FPE__RSV1__S 0x3	
#define	EV6__ASN_ASTRR_FPE__RSV1__V 0x2	
#define	EV6__ASN_ASTRR_FPE__RSV1__M 0x3	
#define	EV6__ASN_ASTRR_FPE__ASTER__S 0x5	
#define	EV6__ASN_ASTRR_FPE__ASTER__V 0x4	
#define	EV6__ASN_ASTRR_FPE__ASTER__M 0xf	
#define	EV6__ASN_ASTRR_FPE__ASTRR__S 0x9	
#define	EV6__ASN_ASTRR_FPE__ASTRR__V 0x4	
#define	EV6__ASN_ASTRR_FPE__ASTRR__M 0xf	
#define	EV6__ASN_ASTRR_FPE__RSV2__S 0xd	
#define	EV6__ASN_ASTRR_FPE__RSV2__V 0x1a	
#define	EV6__ASN_ASTRR_FPE__RSV2__M 0x3ffffff	
#define	EV6__ASN_ASTRR_FPE__ASN__S 0x27	
#define	EV6__ASN_ASTRR_FPE__ASN__V 0x8	
#define	EV6__ASN_ASTRR_FPE__ASN__M 0xff	
#define	EV6__ASN_ASTRR_FPE__RSV3__S 0x2f	
#define	EV6__ASN_ASTRR_FPE__RSV3__V 0x11	
#define	EV6__ASN_ASTRR_FPE__RSV3__M 0x1ffff	

#define	EV6__ASN_PPCE_FPE 0x59	
#define	EV6__ASN_PPCE_FPE__RSV0__S 0x0	
#define	EV6__ASN_PPCE_FPE__RSV0__V 0x1	
#define	EV6__ASN_PPCE_FPE__RSV0__M 0x1	
#define	EV6__ASN_PPCE_FPE__PPCE__S 0x1	
#define	EV6__ASN_PPCE_FPE__PPCE__V 0x1	
#define	EV6__ASN_PPCE_FPE__PPCE__M 0x1	
#define	EV6__ASN_PPCE_FPE__FPE__S 0x2	
#define	EV6__ASN_PPCE_FPE__FPE__V 0x1	
#define	EV6__ASN_PPCE_FPE__FPE__M 0x1	
#define	EV6__ASN_PPCE_FPE__RSV1__S 0x3	
#define	EV6__ASN_PPCE_FPE__RSV1__V 0x2	
#define	EV6__ASN_PPCE_FPE__RSV1__M 0x3	
#define	EV6__ASN_PPCE_FPE__ASTER__S 0x5	
#define	EV6__ASN_PPCE_FPE__ASTER__V 0x4	
#define	EV6__ASN_PPCE_FPE__ASTER__M 0xf	
#define	EV6__ASN_PPCE_FPE__ASTRR__S 0x9	
#define	EV6__ASN_PPCE_FPE__ASTRR__V 0x4	
#define	EV6__ASN_PPCE_FPE__ASTRR__M 0xf	
#define	EV6__ASN_PPCE_FPE__RSV2__S 0xd	
#define	EV6__ASN_PPCE_FPE__RSV2__V 0x1a	
#define	EV6__ASN_PPCE_FPE__RSV2__M 0x3ffffff	
#define	EV6__ASN_PPCE_FPE__ASN__S 0x27	
#define	EV6__ASN_PPCE_FPE__ASN__V 0x8	
#define	EV6__ASN_PPCE_FPE__ASN__M 0xff	
#define	EV6__ASN_PPCE_FPE__RSV3__S 0x2f	
#define	EV6__ASN_PPCE_FPE__RSV3__V 0x11	
#define	EV6__ASN_PPCE_FPE__RSV3__M 0x1ffff	

#define	EV6__ASTER_ASTRR_PPCE 0x4e	
#define	EV6__ASTER_ASTRR_PPCE__RSV0__S 0x0	
#define	EV6__ASTER_ASTRR_PPCE__RSV0__V 0x1	
#define	EV6__ASTER_ASTRR_PPCE__RSV0__M 0x1	
#define	EV6__ASTER_ASTRR_PPCE__PPCE__S 0x1	
#define	EV6__ASTER_ASTRR_PPCE__PPCE__V 0x1	
#define	EV6__ASTER_ASTRR_PPCE__PPCE__M 0x1	
#define	EV6__ASTER_ASTRR_PPCE__FPE__S 0x2	
#define	EV6__ASTER_ASTRR_PPCE__FPE__V 0x1	
#define	EV6__ASTER_ASTRR_PPCE__FPE__M 0x1	
#define	EV6__ASTER_ASTRR_PPCE__RSV1__S 0x3	
#define	EV6__ASTER_ASTRR_PPCE__RSV1__V 0x2	
#define	EV6__ASTER_ASTRR_PPCE__RSV1__M 0x3	
#define	EV6__ASTER_ASTRR_PPCE__ASTER__S 0x5	
#define	EV6__ASTER_ASTRR_PPCE__ASTER__V 0x4	
#define	EV6__ASTER_ASTRR_PPCE__ASTER__M 0xf	
#define	EV6__ASTER_ASTRR_PPCE__ASTRR__S 0x9	
#define	EV6__ASTER_ASTRR_PPCE__ASTRR__V 0x4	
#define	EV6__ASTER_ASTRR_PPCE__ASTRR__M 0xf	
#define	EV6__ASTER_ASTRR_PPCE__RSV2__S 0xd	
#define	EV6__ASTER_ASTRR_PPCE__RSV2__V 0x1a	
#define	EV6__ASTER_ASTRR_PPCE__RSV2__M 0x3ffffff	
#define	EV6__ASTER_ASTRR_PPCE__ASN__S 0x27	
#define	EV6__ASTER_ASTRR_PPCE__ASN__V 0x8	
#define	EV6__ASTER_ASTRR_PPCE__ASN__M 0xff	
#define	EV6__ASTER_ASTRR_PPCE__RSV3__S 0x2f	
#define	EV6__ASTER_ASTRR_PPCE__RSV3__V 0x11	
#define	EV6__ASTER_ASTRR_PPCE__RSV3__M 0x1ffff	

#define	EV6__ASTER_ASTRR_FPE 0x56	
#define	EV6__ASTER_ASTRR_FPE__RSV0__S 0x0	
#define	EV6__ASTER_ASTRR_FPE__RSV0__V 0x1	
#define	EV6__ASTER_ASTRR_FPE__RSV0__M 0x1	
#define	EV6__ASTER_ASTRR_FPE__PPCE__S 0x1	
#define	EV6__ASTER_ASTRR_FPE__PPCE__V 0x1	
#define	EV6__ASTER_ASTRR_FPE__PPCE__M 0x1	
#define	EV6__ASTER_ASTRR_FPE__FPE__S 0x2	
#define	EV6__ASTER_ASTRR_FPE__FPE__V 0x1	
#define	EV6__ASTER_ASTRR_FPE__FPE__M 0x1	
#define	EV6__ASTER_ASTRR_FPE__RSV1__S 0x3	
#define	EV6__ASTER_ASTRR_FPE__RSV1__V 0x2	
#define	EV6__ASTER_ASTRR_FPE__RSV1__M 0x3	
#define	EV6__ASTER_ASTRR_FPE__ASTER__S 0x5	
#define	EV6__ASTER_ASTRR_FPE__ASTER__V 0x4	
#define	EV6__ASTER_ASTRR_FPE__ASTER__M 0xf	
#define	EV6__ASTER_ASTRR_FPE__ASTRR__S 0x9	
#define	EV6__ASTER_ASTRR_FPE__ASTRR__V 0x4	
#define	EV6__ASTER_ASTRR_FPE__ASTRR__M 0xf	
#define	EV6__ASTER_ASTRR_FPE__RSV2__S 0xd	
#define	EV6__ASTER_ASTRR_FPE__RSV2__V 0x1a	
#define	EV6__ASTER_ASTRR_FPE__RSV2__M 0x3ffffff	
#define	EV6__ASTER_ASTRR_FPE__ASN__S 0x27	
#define	EV6__ASTER_ASTRR_FPE__ASN__V 0x8	
#define	EV6__ASTER_ASTRR_FPE__ASN__M 0xff	
#define	EV6__ASTER_ASTRR_FPE__RSV3__S 0x2f	
#define	EV6__ASTER_ASTRR_FPE__RSV3__V 0x11	
#define	EV6__ASTER_ASTRR_FPE__RSV3__M 0x1ffff	

#define	EV6__ASTER_PPCE_FPE 0x5a	
#define	EV6__ASTER_PPCE_FPE__RSV0__S 0x0	
#define	EV6__ASTER_PPCE_FPE__RSV0__V 0x1	
#define	EV6__ASTER_PPCE_FPE__RSV0__M 0x1	
#define	EV6__ASTER_PPCE_FPE__PPCE__S 0x1	
#define	EV6__ASTER_PPCE_FPE__PPCE__V 0x1	
#define	EV6__ASTER_PPCE_FPE__PPCE__M 0x1	
#define	EV6__ASTER_PPCE_FPE__FPE__S 0x2	
#define	EV6__ASTER_PPCE_FPE__FPE__V 0x1	
#define	EV6__ASTER_PPCE_FPE__FPE__M 0x1	
#define	EV6__ASTER_PPCE_FPE__RSV1__S 0x3	
#define	EV6__ASTER_PPCE_FPE__RSV1__V 0x2	
#define	EV6__ASTER_PPCE_FPE__RSV1__M 0x3	
#define	EV6__ASTER_PPCE_FPE__ASTER__S 0x5	
#define	EV6__ASTER_PPCE_FPE__ASTER__V 0x4	
#define	EV6__ASTER_PPCE_FPE__ASTER__M 0xf	
#define	EV6__ASTER_PPCE_FPE__ASTRR__S 0x9	
#define	EV6__ASTER_PPCE_FPE__ASTRR__V 0x4	
#define	EV6__ASTER_PPCE_FPE__ASTRR__M 0xf	
#define	EV6__ASTER_PPCE_FPE__RSV2__S 0xd	
#define	EV6__ASTER_PPCE_FPE__RSV2__V 0x1a	
#define	EV6__ASTER_PPCE_FPE__RSV2__M 0x3ffffff	
#define	EV6__ASTER_PPCE_FPE__ASN__S 0x27	
#define	EV6__ASTER_PPCE_FPE__ASN__V 0x8	
#define	EV6__ASTER_PPCE_FPE__ASN__M 0xff	
#define	EV6__ASTER_PPCE_FPE__RSV3__S 0x2f	
#define	EV6__ASTER_PPCE_FPE__RSV3__V 0x11	
#define	EV6__ASTER_PPCE_FPE__RSV3__M 0x1ffff	

#define	EV6__ASTRR_PPCE_FPE 0x5c	
#define	EV6__ASTRR_PPCE_FPE__RSV0__S 0x0	
#define	EV6__ASTRR_PPCE_FPE__RSV0__V 0x1	
#define	EV6__ASTRR_PPCE_FPE__RSV0__M 0x1	
#define	EV6__ASTRR_PPCE_FPE__PPCE__S 0x1	
#define	EV6__ASTRR_PPCE_FPE__PPCE__V 0x1	
#define	EV6__ASTRR_PPCE_FPE__PPCE__M 0x1	
#define	EV6__ASTRR_PPCE_FPE__FPE__S 0x2	
#define	EV6__ASTRR_PPCE_FPE__FPE__V 0x1	
#define	EV6__ASTRR_PPCE_FPE__FPE__M 0x1	
#define	EV6__ASTRR_PPCE_FPE__RSV1__S 0x3	
#define	EV6__ASTRR_PPCE_FPE__RSV1__V 0x2	
#define	EV6__ASTRR_PPCE_FPE__RSV1__M 0x3	
#define	EV6__ASTRR_PPCE_FPE__ASTER__S 0x5	
#define	EV6__ASTRR_PPCE_FPE__ASTER__V 0x4	
#define	EV6__ASTRR_PPCE_FPE__ASTER__M 0xf	
#define	EV6__ASTRR_PPCE_FPE__ASTRR__S 0x9	
#define	EV6__ASTRR_PPCE_FPE__ASTRR__V 0x4	
#define	EV6__ASTRR_PPCE_FPE__ASTRR__M 0xf	
#define	EV6__ASTRR_PPCE_FPE__RSV2__S 0xd	
#define	EV6__ASTRR_PPCE_FPE__RSV2__V 0x1a	
#define	EV6__ASTRR_PPCE_FPE__RSV2__M 0x3ffffff	
#define	EV6__ASTRR_PPCE_FPE__ASN__S 0x27	
#define	EV6__ASTRR_PPCE_FPE__ASN__V 0x8	
#define	EV6__ASTRR_PPCE_FPE__ASN__M 0xff	
#define	EV6__ASTRR_PPCE_FPE__RSV3__S 0x2f	
#define	EV6__ASTRR_PPCE_FPE__RSV3__V 0x11	
#define	EV6__ASTRR_PPCE_FPE__RSV3__M 0x1ffff	

#define	EV6__ASN_ASTER_ASTRR_PPCE 0x4f	
#define	EV6__ASN_ASTER_ASTRR_PPCE__RSV0__S 0x0	
#define	EV6__ASN_ASTER_ASTRR_PPCE__RSV0__V 0x1	
#define	EV6__ASN_ASTER_ASTRR_PPCE__RSV0__M 0x1	
#define	EV6__ASN_ASTER_ASTRR_PPCE__PPCE__S 0x1	
#define	EV6__ASN_ASTER_ASTRR_PPCE__PPCE__V 0x1	
#define	EV6__ASN_ASTER_ASTRR_PPCE__PPCE__M 0x1	
#define	EV6__ASN_ASTER_ASTRR_PPCE__FPE__S 0x2	
#define	EV6__ASN_ASTER_ASTRR_PPCE__FPE__V 0x1	
#define	EV6__ASN_ASTER_ASTRR_PPCE__FPE__M 0x1	
#define	EV6__ASN_ASTER_ASTRR_PPCE__RSV1__S 0x3	
#define	EV6__ASN_ASTER_ASTRR_PPCE__RSV1__V 0x2	
#define	EV6__ASN_ASTER_ASTRR_PPCE__RSV1__M 0x3	
#define	EV6__ASN_ASTER_ASTRR_PPCE__ASTER__S 0x5	
#define	EV6__ASN_ASTER_ASTRR_PPCE__ASTER__V 0x4	
#define	EV6__ASN_ASTER_ASTRR_PPCE__ASTER__M 0xf	
#define	EV6__ASN_ASTER_ASTRR_PPCE__ASTRR__S 0x9	
#define	EV6__ASN_ASTER_ASTRR_PPCE__ASTRR__V 0x4	
#define	EV6__ASN_ASTER_ASTRR_PPCE__ASTRR__M 0xf	
#define	EV6__ASN_ASTER_ASTRR_PPCE__RSV2__S 0xd	
#define	EV6__ASN_ASTER_ASTRR_PPCE__RSV2__V 0x1a	
#define	EV6__ASN_ASTER_ASTRR_PPCE__RSV2__M 0x3ffffff	
#define	EV6__ASN_ASTER_ASTRR_PPCE__ASN__S 0x27	
#define	EV6__ASN_ASTER_ASTRR_PPCE__ASN__V 0x8	
#define	EV6__ASN_ASTER_ASTRR_PPCE__ASN__M 0xff	
#define	EV6__ASN_ASTER_ASTRR_PPCE__RSV3__S 0x2f	
#define	EV6__ASN_ASTER_ASTRR_PPCE__RSV3__V 0x11	
#define	EV6__ASN_ASTER_ASTRR_PPCE__RSV3__M 0x1ffff	

#define	EV6__ASN_ASTER_ASTRR_FPE 0x57	
#define	EV6__ASN_ASTER_ASTRR_FPE__RSV0__S 0x0	
#define	EV6__ASN_ASTER_ASTRR_FPE__RSV0__V 0x1	
#define	EV6__ASN_ASTER_ASTRR_FPE__RSV0__M 0x1	
#define	EV6__ASN_ASTER_ASTRR_FPE__PPCE__S 0x1	
#define	EV6__ASN_ASTER_ASTRR_FPE__PPCE__V 0x1	
#define	EV6__ASN_ASTER_ASTRR_FPE__PPCE__M 0x1	
#define	EV6__ASN_ASTER_ASTRR_FPE__FPE__S 0x2	
#define	EV6__ASN_ASTER_ASTRR_FPE__FPE__V 0x1	
#define	EV6__ASN_ASTER_ASTRR_FPE__FPE__M 0x1	
#define	EV6__ASN_ASTER_ASTRR_FPE__RSV1__S 0x3	
#define	EV6__ASN_ASTER_ASTRR_FPE__RSV1__V 0x2	
#define	EV6__ASN_ASTER_ASTRR_FPE__RSV1__M 0x3	
#define	EV6__ASN_ASTER_ASTRR_FPE__ASTER__S 0x5	
#define	EV6__ASN_ASTER_ASTRR_FPE__ASTER__V 0x4	
#define	EV6__ASN_ASTER_ASTRR_FPE__ASTER__M 0xf	
#define	EV6__ASN_ASTER_ASTRR_FPE__ASTRR__S 0x9	
#define	EV6__ASN_ASTER_ASTRR_FPE__ASTRR__V 0x4	
#define	EV6__ASN_ASTER_ASTRR_FPE__ASTRR__M 0xf	
#define	EV6__ASN_ASTER_ASTRR_FPE__RSV2__S 0xd	
#define	EV6__ASN_ASTER_ASTRR_FPE__RSV2__V 0x1a	
#define	EV6__ASN_ASTER_ASTRR_FPE__RSV2__M 0x3ffffff	
#define	EV6__ASN_ASTER_ASTRR_FPE__ASN__S 0x27	
#define	EV6__ASN_ASTER_ASTRR_FPE__ASN__V 0x8	
#define	EV6__ASN_ASTER_ASTRR_FPE__ASN__M 0xff	
#define	EV6__ASN_ASTER_ASTRR_FPE__RSV3__S 0x2f	
#define	EV6__ASN_ASTER_ASTRR_FPE__RSV3__V 0x11	
#define	EV6__ASN_ASTER_ASTRR_FPE__RSV3__M 0x1ffff	

#define	EV6__ASN_ASTER_PPCE_FPE 0x5b	
#define	EV6__ASN_ASTER_PPCE_FPE__RSV0__S 0x0	
#define	EV6__ASN_ASTER_PPCE_FPE__RSV0__V 0x1	
#define	EV6__ASN_ASTER_PPCE_FPE__RSV0__M 0x1	
#define	EV6__ASN_ASTER_PPCE_FPE__PPCE__S 0x1	
#define	EV6__ASN_ASTER_PPCE_FPE__PPCE__V 0x1	
#define	EV6__ASN_ASTER_PPCE_FPE__PPCE__M 0x1	
#define	EV6__ASN_ASTER_PPCE_FPE__FPE__S 0x2	
#define	EV6__ASN_ASTER_PPCE_FPE__FPE__V 0x1	
#define	EV6__ASN_ASTER_PPCE_FPE__FPE__M 0x1	
#define	EV6__ASN_ASTER_PPCE_FPE__RSV1__S 0x3	
#define	EV6__ASN_ASTER_PPCE_FPE__RSV1__V 0x2	
#define	EV6__ASN_ASTER_PPCE_FPE__RSV1__M 0x3	
#define	EV6__ASN_ASTER_PPCE_FPE__ASTER__S 0x5	
#define	EV6__ASN_ASTER_PPCE_FPE__ASTER__V 0x4	
#define	EV6__ASN_ASTER_PPCE_FPE__ASTER__M 0xf	
#define	EV6__ASN_ASTER_PPCE_FPE__ASTRR__S 0x9	
#define	EV6__ASN_ASTER_PPCE_FPE__ASTRR__V 0x4	
#define	EV6__ASN_ASTER_PPCE_FPE__ASTRR__M 0xf	
#define	EV6__ASN_ASTER_PPCE_FPE__RSV2__S 0xd	
#define	EV6__ASN_ASTER_PPCE_FPE__RSV2__V 0x1a	
#define	EV6__ASN_ASTER_PPCE_FPE__RSV2__M 0x3ffffff	
#define	EV6__ASN_ASTER_PPCE_FPE__ASN__S 0x27	
#define	EV6__ASN_ASTER_PPCE_FPE__ASN__V 0x8	
#define	EV6__ASN_ASTER_PPCE_FPE__ASN__M 0xff	
#define	EV6__ASN_ASTER_PPCE_FPE__RSV3__S 0x2f	
#define	EV6__ASN_ASTER_PPCE_FPE__RSV3__V 0x11	
#define	EV6__ASN_ASTER_PPCE_FPE__RSV3__M 0x1ffff	

#define	EV6__ASN_ASTRR_PPCE_FPE 0x5d	
#define	EV6__ASN_ASTRR_PPCE_FPE__RSV0__S 0x0	
#define	EV6__ASN_ASTRR_PPCE_FPE__RSV0__V 0x1	
#define	EV6__ASN_ASTRR_PPCE_FPE__RSV0__M 0x1	
#define	EV6__ASN_ASTRR_PPCE_FPE__PPCE__S 0x1	
#define	EV6__ASN_ASTRR_PPCE_FPE__PPCE__V 0x1	
#define	EV6__ASN_ASTRR_PPCE_FPE__PPCE__M 0x1	
#define	EV6__ASN_ASTRR_PPCE_FPE__FPE__S 0x2	
#define	EV6__ASN_ASTRR_PPCE_FPE__FPE__V 0x1	
#define	EV6__ASN_ASTRR_PPCE_FPE__FPE__M 0x1	
#define	EV6__ASN_ASTRR_PPCE_FPE__RSV1__S 0x3	
#define	EV6__ASN_ASTRR_PPCE_FPE__RSV1__V 0x2	
#define	EV6__ASN_ASTRR_PPCE_FPE__RSV1__M 0x3	
#define	EV6__ASN_ASTRR_PPCE_FPE__ASTER__S 0x5	
#define	EV6__ASN_ASTRR_PPCE_FPE__ASTER__V 0x4	
#define	EV6__ASN_ASTRR_PPCE_FPE__ASTER__M 0xf	
#define	EV6__ASN_ASTRR_PPCE_FPE__ASTRR__S 0x9	
#define	EV6__ASN_ASTRR_PPCE_FPE__ASTRR__V 0x4	
#define	EV6__ASN_ASTRR_PPCE_FPE__ASTRR__M 0xf	
#define	EV6__ASN_ASTRR_PPCE_FPE__RSV2__S 0xd	
#define	EV6__ASN_ASTRR_PPCE_FPE__RSV2__V 0x1a	
#define	EV6__ASN_ASTRR_PPCE_FPE__RSV2__M 0x3ffffff	
#define	EV6__ASN_ASTRR_PPCE_FPE__ASN__S 0x27	
#define	EV6__ASN_ASTRR_PPCE_FPE__ASN__V 0x8	
#define	EV6__ASN_ASTRR_PPCE_FPE__ASN__M 0xff	
#define	EV6__ASN_ASTRR_PPCE_FPE__RSV3__S 0x2f	
#define	EV6__ASN_ASTRR_PPCE_FPE__RSV3__V 0x11	
#define	EV6__ASN_ASTRR_PPCE_FPE__RSV3__M 0x1ffff	

#define	EV6__ASTER_ASTRR_PPCE_FPE 0x5e	
#define	EV6__ASTER_ASTRR_PPCE_FPE__RSV0__S 0x0	
#define	EV6__ASTER_ASTRR_PPCE_FPE__RSV0__V 0x1	
#define	EV6__ASTER_ASTRR_PPCE_FPE__RSV0__M 0x1	
#define	EV6__ASTER_ASTRR_PPCE_FPE__PPCE__S 0x1	
#define	EV6__ASTER_ASTRR_PPCE_FPE__PPCE__V 0x1	
#define	EV6__ASTER_ASTRR_PPCE_FPE__PPCE__M 0x1	
#define	EV6__ASTER_ASTRR_PPCE_FPE__FPE__S 0x2	
#define	EV6__ASTER_ASTRR_PPCE_FPE__FPE__V 0x1	
#define	EV6__ASTER_ASTRR_PPCE_FPE__FPE__M 0x1	
#define	EV6__ASTER_ASTRR_PPCE_FPE__RSV1__S 0x3	
#define	EV6__ASTER_ASTRR_PPCE_FPE__RSV1__V 0x2	
#define	EV6__ASTER_ASTRR_PPCE_FPE__RSV1__M 0x3	
#define	EV6__ASTER_ASTRR_PPCE_FPE__ASTER__S 0x5	
#define	EV6__ASTER_ASTRR_PPCE_FPE__ASTER__V 0x4	
#define	EV6__ASTER_ASTRR_PPCE_FPE__ASTER__M 0xf	
#define	EV6__ASTER_ASTRR_PPCE_FPE__ASTRR__S 0x9	
#define	EV6__ASTER_ASTRR_PPCE_FPE__ASTRR__V 0x4	
#define	EV6__ASTER_ASTRR_PPCE_FPE__ASTRR__M 0xf	
#define	EV6__ASTER_ASTRR_PPCE_FPE__RSV2__S 0xd	
#define	EV6__ASTER_ASTRR_PPCE_FPE__RSV2__V 0x1a	
#define	EV6__ASTER_ASTRR_PPCE_FPE__RSV2__M 0x3ffffff	
#define	EV6__ASTER_ASTRR_PPCE_FPE__ASN__S 0x27	
#define	EV6__ASTER_ASTRR_PPCE_FPE__ASN__V 0x8	
#define	EV6__ASTER_ASTRR_PPCE_FPE__ASN__M 0xff	
#define	EV6__ASTER_ASTRR_PPCE_FPE__RSV3__S 0x2f	
#define	EV6__ASTER_ASTRR_PPCE_FPE__RSV3__V 0x11	
#define	EV6__ASTER_ASTRR_PPCE_FPE__RSV3__M 0x1ffff	

#define	EV6__PROCESS_CONTEXT 0x5f	
#define	EV6__PROCESS_CONTEXT__RSV0__S 0x0	
#define	EV6__PROCESS_CONTEXT__RSV0__V 0x1	
#define	EV6__PROCESS_CONTEXT__RSV0__M 0x1	
#define	EV6__PROCESS_CONTEXT__PPCE__S 0x1	
#define	EV6__PROCESS_CONTEXT__PPCE__V 0x1	
#define	EV6__PROCESS_CONTEXT__PPCE__M 0x1	
#define	EV6__PROCESS_CONTEXT__FPE__S 0x2	
#define	EV6__PROCESS_CONTEXT__FPE__V 0x1	
#define	EV6__PROCESS_CONTEXT__FPE__M 0x1	
#define	EV6__PROCESS_CONTEXT__RSV1__S 0x3	
#define	EV6__PROCESS_CONTEXT__RSV1__V 0x2	
#define	EV6__PROCESS_CONTEXT__RSV1__M 0x3	
#define	EV6__PROCESS_CONTEXT__ASTER__S 0x5	
#define	EV6__PROCESS_CONTEXT__ASTER__V 0x4	
#define	EV6__PROCESS_CONTEXT__ASTER__M 0xf	
#define	EV6__PROCESS_CONTEXT__ASTRR__S 0x9	
#define	EV6__PROCESS_CONTEXT__ASTRR__V 0x4	
#define	EV6__PROCESS_CONTEXT__ASTRR__M 0xf	
#define	EV6__PROCESS_CONTEXT__RSV2__S 0xd	
#define	EV6__PROCESS_CONTEXT__RSV2__V 0x1a	
#define	EV6__PROCESS_CONTEXT__RSV2__M 0x3ffffff	
#define	EV6__PROCESS_CONTEXT__ASN__S 0x27	
#define	EV6__PROCESS_CONTEXT__ASN__V 0x8	
#define	EV6__PROCESS_CONTEXT__ASN__M 0xff	
#define	EV6__PROCESS_CONTEXT__RSV3__S 0x2f	
#define	EV6__PROCESS_CONTEXT__RSV3__V 0x11	
#define	EV6__PROCESS_CONTEXT__RSV3__M 0x1ffff	

#define	EV6__DTB_TAG0 0x20	
#define	EV6__DTB_TAG0__RSV1__S 0x0	
#define	EV6__DTB_TAG0__RSV1__V 0xd	
#define	EV6__DTB_TAG0__RSV1__M 0x1fff	
#define	EV6__DTB_TAG0__VA__S 0xd	
#define	EV6__DTB_TAG0__VA__V 0x23	
#define	EV6__DTB_TAG0__VA__M 0x7ffffffff	
#define	EV6__DTB_TAG0__RSV2__S 0x30	
#define	EV6__DTB_TAG0__RSV2__V 0x10	
#define	EV6__DTB_TAG0__RSV2__M 0xffff	

#define	EV6__DTB_TAG1 0xa0	
#define	EV6__DTB_TAG1__RSV1__S 0x0	
#define	EV6__DTB_TAG1__RSV1__V 0xd	
#define	EV6__DTB_TAG1__RSV1__M 0x1fff	
#define	EV6__DTB_TAG1__VA__S 0xd	
#define	EV6__DTB_TAG1__VA__V 0x23	
#define	EV6__DTB_TAG1__VA__M 0x7ffffffff	
#define	EV6__DTB_TAG1__RSV2__S 0x30	
#define	EV6__DTB_TAG1__RSV2__V 0x10	
#define	EV6__DTB_TAG1__RSV2__M 0xffff	

#define	EV6__DTB_PTE0 0x21	
#define	EV6__DTB_PTE0__RSV1__S 0x0	
#define	EV6__DTB_PTE0__RSV1__V 0x1	
#define	EV6__DTB_PTE0__RSV1__M 0x1	
#define	EV6__DTB_PTE0__FOR__S 0x1	
#define	EV6__DTB_PTE0__FOR__V 0x1	
#define	EV6__DTB_PTE0__FOR__M 0x1	
#define	EV6__DTB_PTE0__FOW__S 0x2	
#define	EV6__DTB_PTE0__FOW__V 0x1	
#define	EV6__DTB_PTE0__FOW__M 0x1	
#define	EV6__DTB_PTE0__RSV2__S 0x3	
#define	EV6__DTB_PTE0__RSV2__V 0x1	
#define	EV6__DTB_PTE0__RSV2__M 0x1	
#define	EV6__DTB_PTE0__ASM__S 0x4	
#define	EV6__DTB_PTE0__ASM__V 0x1	
#define	EV6__DTB_PTE0__ASM__M 0x1	
#define	EV6__DTB_PTE0__GH__S 0x5	
#define	EV6__DTB_PTE0__GH__V 0x2	
#define	EV6__DTB_PTE0__GH__M 0x3	
#define	EV6__DTB_PTE0__RSV3__S 0x7	
#define	EV6__DTB_PTE0__RSV3__V 0x1	
#define	EV6__DTB_PTE0__RSV3__M 0x1	
#define	EV6__DTB_PTE0__KRE__S 0x8	
#define	EV6__DTB_PTE0__KRE__V 0x1	
#define	EV6__DTB_PTE0__KRE__M 0x1	
#define	EV6__DTB_PTE0__ERE__S 0x9	
#define	EV6__DTB_PTE0__ERE__V 0x1	
#define	EV6__DTB_PTE0__ERE__M 0x1	
#define	EV6__DTB_PTE0__SRE__S 0xa	
#define	EV6__DTB_PTE0__SRE__V 0x1	
#define	EV6__DTB_PTE0__SRE__M 0x1	
#define	EV6__DTB_PTE0__URE__S 0xb	
#define	EV6__DTB_PTE0__URE__V 0x1	
#define	EV6__DTB_PTE0__URE__M 0x1	
#define	EV6__DTB_PTE0__KWE__S 0xc	
#define	EV6__DTB_PTE0__KWE__V 0x1	
#define	EV6__DTB_PTE0__KWE__M 0x1	
#define	EV6__DTB_PTE0__EWE__S 0xd	
#define	EV6__DTB_PTE0__EWE__V 0x1	
#define	EV6__DTB_PTE0__EWE__M 0x1	
#define	EV6__DTB_PTE0__SWE__S 0xe	
#define	EV6__DTB_PTE0__SWE__V 0x1	
#define	EV6__DTB_PTE0__SWE__M 0x1	
#define	EV6__DTB_PTE0__UWE__S 0xf	
#define	EV6__DTB_PTE0__UWE__V 0x1	
#define	EV6__DTB_PTE0__UWE__M 0x1	
#define	EV6__DTB_PTE0__RSV4__S 0x10	
#define	EV6__DTB_PTE0__RSV4__V 0x10	
#define	EV6__DTB_PTE0__RSV4__M 0xffff	
#define	EV6__DTB_PTE0__PFN__S 0x20	
#define	EV6__DTB_PTE0__PFN__V 0x1f	
#define	EV6__DTB_PTE0__PFN__M 0x7fffffff	
#define	EV6__DTB_PTE0__RSV5__S 0x3f	
#define	EV6__DTB_PTE0__RSV5__V 0x1	
#define	EV6__DTB_PTE0__RSV5__M 0x1	

#define	EV6__DTB_PTE1 0xa1	
#define	EV6__DTB_PTE1__RSV1__S 0x0	
#define	EV6__DTB_PTE1__RSV1__V 0x1	
#define	EV6__DTB_PTE1__RSV1__M 0x1	
#define	EV6__DTB_PTE1__FOR__S 0x1	
#define	EV6__DTB_PTE1__FOR__V 0x1	
#define	EV6__DTB_PTE1__FOR__M 0x1	
#define	EV6__DTB_PTE1__FOW__S 0x2	
#define	EV6__DTB_PTE1__FOW__V 0x1	
#define	EV6__DTB_PTE1__FOW__M 0x1	
#define	EV6__DTB_PTE1__RSV2__S 0x3	
#define	EV6__DTB_PTE1__RSV2__V 0x1	
#define	EV6__DTB_PTE1__RSV2__M 0x1	
#define	EV6__DTB_PTE1__ASM__S 0x4	
#define	EV6__DTB_PTE1__ASM__V 0x1	
#define	EV6__DTB_PTE1__ASM__M 0x1	
#define	EV6__DTB_PTE1__GH__S 0x5	
#define	EV6__DTB_PTE1__GH__V 0x2	
#define	EV6__DTB_PTE1__GH__M 0x3	
#define	EV6__DTB_PTE1__RSV3__S 0x7	
#define	EV6__DTB_PTE1__RSV3__V 0x1	
#define	EV6__DTB_PTE1__RSV3__M 0x1	
#define	EV6__DTB_PTE1__KRE__S 0x8	
#define	EV6__DTB_PTE1__KRE__V 0x1	
#define	EV6__DTB_PTE1__KRE__M 0x1	
#define	EV6__DTB_PTE1__ERE__S 0x9	
#define	EV6__DTB_PTE1__ERE__V 0x1	
#define	EV6__DTB_PTE1__ERE__M 0x1	
#define	EV6__DTB_PTE1__SRE__S 0xa	
#define	EV6__DTB_PTE1__SRE__V 0x1	
#define	EV6__DTB_PTE1__SRE__M 0x1	
#define	EV6__DTB_PTE1__URE__S 0xb	
#define	EV6__DTB_PTE1__URE__V 0x1	
#define	EV6__DTB_PTE1__URE__M 0x1	
#define	EV6__DTB_PTE1__KWE__S 0xc	
#define	EV6__DTB_PTE1__KWE__V 0x1	
#define	EV6__DTB_PTE1__KWE__M 0x1	
#define	EV6__DTB_PTE1__EWE__S 0xd	
#define	EV6__DTB_PTE1__EWE__V 0x1	
#define	EV6__DTB_PTE1__EWE__M 0x1	
#define	EV6__DTB_PTE1__SWE__S 0xe	
#define	EV6__DTB_PTE1__SWE__V 0x1	
#define	EV6__DTB_PTE1__SWE__M 0x1	
#define	EV6__DTB_PTE1__UWE__S 0xf	
#define	EV6__DTB_PTE1__UWE__V 0x1	
#define	EV6__DTB_PTE1__UWE__M 0x1	
#define	EV6__DTB_PTE1__RSV4__S 0x10	
#define	EV6__DTB_PTE1__RSV4__V 0x10	
#define	EV6__DTB_PTE1__RSV4__M 0xffff	
#define	EV6__DTB_PTE1__PFN__S 0x20	
#define	EV6__DTB_PTE1__PFN__V 0x1f	
#define	EV6__DTB_PTE1__PFN__M 0x7fffffff	
#define	EV6__DTB_PTE1__RSV5__S 0x3f	
#define	EV6__DTB_PTE1__RSV5__V 0x1	
#define	EV6__DTB_PTE1__RSV5__M 0x1	

#define	EV6__DTB_IAP 0xa2	
#define	EV6__DTB_IAP__RSV__S 0x0	
#define	EV6__DTB_IAP__RSV__V 0x40	
#define	EV6__DTB_IAP__RSV__M 0xffffffffffffffff	

#define	EV6__DTB_IA 0xa3	
#define	EV6__DTB_IA__RSV__S 0x0	
#define	EV6__DTB_IA__RSV__V 0x40	
#define	EV6__DTB_IA__RSV__M 0xffffffffffffffff	

#define	EV6__DTB_IS0 0x24	
#define	EV6__DTB_IS0__RSV1__S 0x0	
#define	EV6__DTB_IS0__RSV1__V 0xd	
#define	EV6__DTB_IS0__RSV1__M 0x1fff	
#define	EV6__DTB_IS0__VA__S 0xd	
#define	EV6__DTB_IS0__VA__V 0x23	
#define	EV6__DTB_IS0__VA__M 0x7ffffffff	
#define	EV6__DTB_IS0__RSV2__S 0x30	
#define	EV6__DTB_IS0__RSV2__V 0x10	
#define	EV6__DTB_IS0__RSV2__M 0xffff	

#define	EV6__DTB_IS1 0xa4	
#define	EV6__DTB_IS1__RSV1__S 0x0	
#define	EV6__DTB_IS1__RSV1__V 0xd	
#define	EV6__DTB_IS1__RSV1__M 0x1fff	
#define	EV6__DTB_IS1__VA__S 0xd	
#define	EV6__DTB_IS1__VA__V 0x23	
#define	EV6__DTB_IS1__VA__M 0x7ffffffff	
#define	EV6__DTB_IS1__RSV2__S 0x30	
#define	EV6__DTB_IS1__RSV2__V 0x10	
#define	EV6__DTB_IS1__RSV2__M 0xffff	

#define	EV6__DTB_ASN0 0x25	
#define	EV6__DTB_ASN0__RSV__S 0x0	
#define	EV6__DTB_ASN0__RSV__V 0x38	
#define	EV6__DTB_ASN0__RSV__M 0xffffffffffffff	
#define	EV6__DTB_ASN0__ASN__S 0x38	
#define	EV6__DTB_ASN0__ASN__V 0x8	
#define	EV6__DTB_ASN0__ASN__M 0xff	

#define	EV6__DTB_ASN1 0xa5	
#define	EV6__DTB_ASN1__RSV__S 0x0	
#define	EV6__DTB_ASN1__RSV__V 0x38	
#define	EV6__DTB_ASN1__RSV__M 0xffffffffffffff	
#define	EV6__DTB_ASN1__ASN__S 0x38	
#define	EV6__DTB_ASN1__ASN__V 0x8	
#define	EV6__DTB_ASN1__ASN__M 0xff	

#define	EV6__DTB_ALT_MODE 0x26	
#define	EV6__DTB_ALT_MODE__MODE__S 0x0	
#define	EV6__DTB_ALT_MODE__MODE__V 0x2	
#define	EV6__DTB_ALT_MODE__MODE__M 0x3	
#define	EV6__DTB_ALT_MODE__RSV1__S 0x2	
#define	EV6__DTB_ALT_MODE__RSV1__V 0x3e	
#define	EV6__DTB_ALT_MODE__RSV1__M 0x3fffffffffffffff	

#define	EV6__MM_STAT 0x27	
#define	EV6__MM_STAT__WR__S 0x0	
#define	EV6__MM_STAT__WR__V 0x1	
#define	EV6__MM_STAT__WR__M 0x1	
#define	EV6__MM_STAT__ACV__S 0x1	
#define	EV6__MM_STAT__ACV__V 0x1	
#define	EV6__MM_STAT__ACV__M 0x1	
#define	EV6__MM_STAT__FOR__S 0x2	
#define	EV6__MM_STAT__FOR__V 0x1	
#define	EV6__MM_STAT__FOR__M 0x1	
#define	EV6__MM_STAT__FOW__S 0x3	
#define	EV6__MM_STAT__FOW__V 0x1	
#define	EV6__MM_STAT__FOW__M 0x1	
#define	EV6__MM_STAT__OPCODE__S 0x4	
#define	EV6__MM_STAT__OPCODE__V 0x6	
#define	EV6__MM_STAT__OPCODE__M 0x3f	
#define	EV6__MM_STAT__DC_TAG_PERR__S 0xa	
#define	EV6__MM_STAT__DC_TAG_PERR__V 0x1	
#define	EV6__MM_STAT__DC_TAG_PERR__M 0x1	
#define	EV6__MM_STAT__RSV1__S 0xb	
#define	EV6__MM_STAT__RSV1__V 0x35	
#define	EV6__MM_STAT__RSV1__M 0x1fffffffffffff	

#define	EV6__M_CTL 0x28	
#define	EV6__M_CTL__RSV1__S 0x0	
#define	EV6__M_CTL__RSV1__V 0x1	
#define	EV6__M_CTL__RSV1__M 0x1	
#define	EV6__M_CTL__SPE__S 0x1	
#define	EV6__M_CTL__SPE__V 0x3	
#define	EV6__M_CTL__SPE__M 0x7	
#define	EV6__M_CTL__RSV2__S 0x4	
#define	EV6__M_CTL__RSV2__V 0x3c	
#define	EV6__M_CTL__RSV2__M 0xfffffffffffffff	

#define	EV6__DC_CTL 0x29	
#define	EV6__DC_CTL__SET_EN__S 0x0	
#define	EV6__DC_CTL__SET_EN__V 0x2	
#define	EV6__DC_CTL__SET_EN__M 0x3	
#define	EV6__DC_CTL__F_HIT__S 0x2	
#define	EV6__DC_CTL__F_HIT__V 0x1	
#define	EV6__DC_CTL__F_HIT__M 0x1	
#define	EV6__DC_CTL__FLUSH__S 0x3	
#define	EV6__DC_CTL__FLUSH__V 0x1	
#define	EV6__DC_CTL__FLUSH__M 0x1	
#define	EV6__DC_CTL__F_BAD_TPAR__S 0x4	
#define	EV6__DC_CTL__F_BAD_TPAR__V 0x1	
#define	EV6__DC_CTL__F_BAD_TPAR__M 0x1	
#define	EV6__DC_CTL__F_BAD_DECC__S 0x5	
#define	EV6__DC_CTL__F_BAD_DECC__V 0x1	
#define	EV6__DC_CTL__F_BAD_DECC__M 0x1	
#define	EV6__DC_CTL__DCTAG_PAR_EN__S 0x6	
#define	EV6__DC_CTL__DCTAG_PAR_EN__V 0x1	
#define	EV6__DC_CTL__DCTAG_PAR_EN__M 0x1	
#define	EV6__DC_CTL__DCDAT_ERR_EN__S 0x7	
#define	EV6__DC_CTL__DCDAT_ERR_EN__V 0x1	
#define	EV6__DC_CTL__DCDAT_ERR_EN__M 0x1	
#define	EV6__DC_CTL__RSV1__S 0x8	
#define	EV6__DC_CTL__RSV1__V 0x38	
#define	EV6__DC_CTL__RSV1__M 0xffffffffffffff	

#define	EV6__DC_STAT 0x2a	
#define	EV6__DC_STAT__TPERR_P0__S 0x0	
#define	EV6__DC_STAT__TPERR_P0__V 0x1	
#define	EV6__DC_STAT__TPERR_P0__M 0x1	
#define	EV6__DC_STAT__TPERR_P1__S 0x1	
#define	EV6__DC_STAT__TPERR_P1__V 0x1	
#define	EV6__DC_STAT__TPERR_P1__M 0x1	
#define	EV6__DC_STAT__DECC_ERR__S 0x2	
#define	EV6__DC_STAT__DECC_ERR__V 0x1	
#define	EV6__DC_STAT__DECC_ERR__M 0x1	
#define	EV6__DC_STAT__DECC_COR__S 0x3	
#define	EV6__DC_STAT__DECC_COR__V 0x1	
#define	EV6__DC_STAT__DECC_COR__M 0x1	
#define	EV6__DC_STAT__SEO__S 0x4	
#define	EV6__DC_STAT__SEO__V 0x1	
#define	EV6__DC_STAT__SEO__M 0x1	
#define	EV6__DC_STAT__RSV1__S 0x5	
#define	EV6__DC_STAT__RSV1__V 0x3b	
#define	EV6__DC_STAT__RSV1__M 0x7ffffffffffffff	

#define	EV6__DATA 0x2b	
#define	EV6__DATA__C_DATA__S 0x0	
#define	EV6__DATA__C_DATA__V 0x6	
#define	EV6__DATA__C_DATA__M 0x3f	
#define	EV6__DATA__RSV1__S 0x6	
#define	EV6__DATA__RSV1__V 0x3a	
#define	EV6__DATA__RSV1__M 0x3ffffffffffffff	

#define	EV6__SHIFT_CONTROL 0x2c	
#define	EV6__SHIFT_CONTROL__C_SHIFT__S 0x0	
#define	EV6__SHIFT_CONTROL__C_SHIFT__V 0x1	
#define	EV6__SHIFT_CONTROL__C_SHIFT__M 0x1	
#define	EV6__SHIFT_CONTROL__RSV1__S 0x1	
#define	EV6__SHIFT_CONTROL__RSV1__V 0x3f	
#define	EV6__SHIFT_CONTROL__RSV1__M 0x7fffffffffffffff	

#define	EV6__FEN 0x10000	
#define	EV6__FEN__FEN__S 0x0	
#define	EV6__FEN__FEN__V 0x1	
#define	EV6__FEN__FEN__M 0x1	
#define	EV6__FEN__RSV2__S 0x1	
#define	EV6__FEN__RSV2__V 0x3f	
#define	EV6__FEN__RSV2__M 0x7fffffffffffffff	

#define	EV6__FPCR 0x10001	
#define	EV6__FPCR__RSV1__S 0x0	
#define	EV6__FPCR__RSV1__V 0x31	
#define	EV6__FPCR__RSV1__M 0x1ffffffffffff	
#define	EV6__FPCR__INVD__S 0x31	
#define	EV6__FPCR__INVD__V 0x1	
#define	EV6__FPCR__INVD__M 0x1	
#define	EV6__FPCR__DZED__S 0x32	
#define	EV6__FPCR__DZED__V 0x1	
#define	EV6__FPCR__DZED__M 0x1	
#define	EV6__FPCR__OVFD__S 0x33	
#define	EV6__FPCR__OVFD__V 0x1	
#define	EV6__FPCR__OVFD__M 0x1	
#define	EV6__FPCR__INV__S 0x34	
#define	EV6__FPCR__INV__V 0x1	
#define	EV6__FPCR__INV__M 0x1	
#define	EV6__FPCR__DZE__S 0x35	
#define	EV6__FPCR__DZE__V 0x1	
#define	EV6__FPCR__DZE__M 0x1	
#define	EV6__FPCR__OVF__S 0x36	
#define	EV6__FPCR__OVF__V 0x1	
#define	EV6__FPCR__OVF__M 0x1	
#define	EV6__FPCR__UNF__S 0x37	
#define	EV6__FPCR__UNF__V 0x1	
#define	EV6__FPCR__UNF__M 0x1	
#define	EV6__FPCR__INE__S 0x38	
#define	EV6__FPCR__INE__V 0x1	
#define	EV6__FPCR__INE__M 0x1	
#define	EV6__FPCR__IOV__S 0x39	
#define	EV6__FPCR__IOV__V 0x1	
#define	EV6__FPCR__IOV__M 0x1	
#define	EV6__FPCR__DYN__S 0x3a	
#define	EV6__FPCR__DYN__V 0x2	
#define	EV6__FPCR__DYN__M 0x3	
#define	EV6__FPCR__UNDZ__S 0x3c	
#define	EV6__FPCR__UNDZ__V 0x1	
#define	EV6__FPCR__UNDZ__M 0x1	
#define	EV6__FPCR__UNFD__S 0x3d	
#define	EV6__FPCR__UNFD__V 0x1	
#define	EV6__FPCR__UNFD__M 0x1	
#define	EV6__FPCR__INED__S 0x3e	
#define	EV6__FPCR__INED__V 0x1	
#define	EV6__FPCR__INED__M 0x1	
#define	EV6__FPCR__SUM__S 0x3f	
#define	EV6__FPCR__SUM__V 0x1	
#define	EV6__FPCR__SUM__M 0x1	

#define	EV6__HW_INT_REG 0x1000e	
#define	EV6__HW_INT_REG__RSV1__S 0x0	
#define	EV6__HW_INT_REG__RSV1__V 0x1a	
#define	EV6__HW_INT_REG__RSV1__M 0x3ffffff	
#define	EV6__HW_INT_REG__FBTP__S 0x1a	
#define	EV6__HW_INT_REG__FBTP__V 0x1	
#define	EV6__HW_INT_REG__FBTP__M 0x1	
#define	EV6__HW_INT_REG__FBDP__S 0x1b	
#define	EV6__HW_INT_REG__FBDP__V 0x1	
#define	EV6__HW_INT_REG__FBDP__M 0x1	
#define	EV6__HW_INT_REG__MCHK_D__S 0x1c	
#define	EV6__HW_INT_REG__MCHK_D__V 0x1	
#define	EV6__HW_INT_REG__MCHK_D__M 0x1	
#define	EV6__HW_INT_REG__PC__S 0x1d	
#define	EV6__HW_INT_REG__PC__V 0x2	
#define	EV6__HW_INT_REG__PC__M 0x3	
#define	EV6__HW_INT_REG__CR__S 0x1f	
#define	EV6__HW_INT_REG__CR__V 0x1	
#define	EV6__HW_INT_REG__CR__M 0x1	
#define	EV6__HW_INT_REG__SL__S 0x20	
#define	EV6__HW_INT_REG__SL__V 0x1	
#define	EV6__HW_INT_REG__SL__M 0x1	
#define	EV6__HW_INT_REG__RSV2__S 0x21	
#define	EV6__HW_INT_REG__RSV2__V 0x1f	
#define	EV6__HW_INT_REG__RSV2__M 0x7fffffff	

#define	EV6_SCB__CC 0x20	
#define	EV6_SCB__CC_CTL 0x20	
#define	EV6_SCB__VA 0xf0	
#define	EV6_SCB__VA_FORM 0xf0	
#define	EV6_SCB__VA_CTL 0x20	
#define	EV6_SCB__ITB_TAG 0x40	
#define	EV6_SCB__ITB_PTE 0x11	
#define	EV6_SCB__ITB_IAP 0x10	
#define	EV6_SCB__ITB_IA 0x10	
#define	EV6_SCB__ITB_IS 0x50	
#define	EV6_SCB__EXC_ADDR 0x0	
#define	EV6_SCB__IVA_FORM 0x0	
#define	EV6_SCB__PS 0x10	
#define	EV6_SCB__IER 0x10	
#define	EV6_SCB__IER_CM 0x10	
#define	EV6_SCB__SIRR 0x10	
#define	EV6_SCB__ISUM 0x0	
#define	EV6_SCB__HW_INT_CLR 0x10	
#define	EV6_SCB__EXC_SUM 0x0	
#define	EV6_SCB__PAL_BASE 0x10	
#define	EV6_SCB__I_CTL 0x10	
#define	EV6_SCB__IC_FLUSH 0x10	
#define	EV6_SCB__IC_FLUSH_ASM 0x10	
#define	EV6_SCB__PCTR_CTL 0x10	
#define	EV6_SCB__CLR_MAP 0xf0	
#define	EV6_SCB__I_STAT 0x0	
#define	EV6_SCB__ASN 0x10	
#define	EV6_SCB__ASTER 0x10	
#define	EV6_SCB__ASTRR 0x10	
#define	EV6_SCB__PPCE 0x10	
#define	EV6_SCB__FPE 0x10	
#define	EV6_SCB__PROCESS_CONTEXT 0x10	
#define	EV6_SCB__DTB_TAG0 0x44	
#define	EV6_SCB__DTB_TAG1 0x22	
#define	EV6_SCB__DTB_PTE0 0x11	
#define	EV6_SCB__DTB_PTE1 0x88	
#define	EV6_SCB__DTB_IAP 0x80	
#define	EV6_SCB__DTB_IA 0x80	
#define	EV6_SCB__DTB_IS0 0x40	
#define	EV6_SCB__DTB_IS1 0x80	
#define	EV6_SCB__DTB_ASN0 0x10	
#define	EV6_SCB__DTB_ASN1 0x80	
#define	EV6_SCB__DTB_ALT_MODE 0x40	
#define	EV6_SCB__MM_STAT 0x0	
#define	EV6_SCB__M_CTL 0x40	
#define	EV6_SCB__DC_CTL 0x40	
#define	EV6_SCB__DC_STAT 0x40	
#define	EV6_SCB__DATA 0x40	
#define	EV6_SCB__SHIFT_CONTROL 0x40	
#endif
