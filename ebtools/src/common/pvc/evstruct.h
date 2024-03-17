/* evstruct.h */

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
 *	V3.24   fls     Aug-96		changed func to 8 bit field for ev6
 *					added sext_func_table and itof_func_table
 *					and sqrt_func_table.
 *					removed inst_name_table_by_func 
 * 					since it is not used.
 *	  0     gpc     AUG-92          First Created
 */

#ifndef EVSTRUCT_H
#define EVSTRUCT_H 1




/* Machine specific definitions */
/* most of the customization takes place here */

/* number of scaler/fpu/epi/ipr reg's */
#define	MAX_SREG	(32)

/* number of opcode/functions (in dispatch tables) */
#define MAX_OPCODE              (64)
#define MAX_SUBFUN              (128)
#define MAX_FPU_SUBFUN          (2048)
#define MAX_EPI_SUBFUN          (256)

/* register number that should be displayed as "SP" */
#define SP_REG                  (30)

/* define the instruction decoding formats */

struct OPERATE_INSTR
{
      unsigned rc	:5;
      unsigned func	:7;
      unsigned litflag :1;
      unsigned sbz	:3;
      unsigned rb	:5;
      unsigned ra	:5;
      unsigned opcode	:6;
};

struct FPOPERATE_INSTR
{
      unsigned rc	:5;
      unsigned func	:11;
      unsigned rb	:5;
      unsigned ra	:5;
      unsigned opcode	:6;
};

struct OPERATE_LIT_INSTR
{
      unsigned rc	:5;
      unsigned func	:7;
      unsigned litflag :1;
      unsigned lit	:8;
      unsigned ra	:5;
      unsigned opcode	:6;
};


struct BRANCH_INSTR
{
      int disp		:21;
      unsigned ra	:5;		/*V3.24*/
      unsigned opcode	:6;		/*V3.24*/
};

struct MEMORY_INSTR
{
      int disp	:16;
      unsigned rb 	:5;		/*V3.24*/
      unsigned ra	:5;		/*V3.24*/
      unsigned opcode	:6;		/*V3.24*/
};

struct HW_MEMORY_INSTR
{
      int	 disp		:12;
      unsigned quad		:1;
      unsigned modify		:1;
      unsigned alt_mode		:1;
      unsigned physical		:1;
      unsigned chk		:1;
};

struct EV6_HW_LD				/* V3.24 */
{
      int	 disp		:12;		/* signed displacement */
      unsigned len		:1;		/* 0 = lw, 1 = qw */
      unsigned type		:3;		/* 000 = physical */
						/* 001 = physical/lock */
						/* 010 = virtual/vpte */
						/* 011   (Unused)   */
						/* 100 = virtual*/
						/* 101 =  virtual/wrchk */
						/* 110 = virtual/alt */
						/* 111 = virtual/wrchk/alt */
};

struct EV6_HW_ST				/* V3.24 */
{
      int	 disp		:12;		/* signed displacement */
      unsigned len		:1;		/* 0 = lw, 1 = qw */
      unsigned type		:3;		/* 000 = physical */
						/* 001 = physical/conditional */
						/* 010 = virtual */
						/* 011   (Unused)   */
						/* 100   (Unused)   */
						/* 101   (Unused)   */
						/* 110 = virtual/alt*/
						/* 111 = (Unused)   */
};


struct EPICODE_INSTR
{
      unsigned func	:26;
};

struct HW_INSTR
{
      unsigned reg	:12;
};

struct HW_MXPR { /* not yet used */
      unsigned func	:16;
      unsigned rb	:5;
      unsigned ra	:5;
      unsigned opcode	:6;
};

struct EV6_HW_MXPR { /* v3.24 */
      unsigned scbd	:8;		/* v3.24 */
      unsigned index	:8;		/* v3.24 */
      unsigned rb	:5;
      unsigned ra	:5;
      unsigned opcode	:6;
};

struct EV6_HW_RET { /* v3.24 */
      unsigned mbz	:13;		/* v3.24 */
      unsigned stall	:1;		/* v3.24 */
      unsigned hint	:2;
      unsigned rb	:5;
      unsigned ra	:5;
      unsigned opcode	:6;
};

union INSTR_FORMAT {
      struct OPERATE_INSTR		op_format;
      struct FPOPERATE_INSTR		fpu_format;
      struct OPERATE_LIT_INSTR		opl_format;
      struct BRANCH_INSTR		bra_format;
      struct MEMORY_INSTR		mem_format;
      struct HW_MEMORY_INSTR		hwmem_format;
      struct EPICODE_INSTR		epi_format;
      struct HW_INSTR			hwipr_format;
      struct HW_MXPR			hwm_format;
      struct EV6_HW_MXPR		ev6_hwmxpr_format;	 /*v3.24*/
      struct EV6_HW_LD			ev6_hwld_format;	 /*v3.24*/
      struct EV6_HW_ST			ev6_hwst_format;	 /*v3.24*/
      struct EV6_HW_RET			ev6_hwret_format;	 /*v3.24*/
      unsigned int			instr;
};


/* define the ipr's */

#define MAX_IPR			(256*3)
#define MAX_ADU_IPR		(128)




/* union used to over map stuff with multiple "views" */

typedef struct {int foo[2];} Quad ;

union squad
{
      Quad		quad[1];
      float		ffloat[2];
      double		dfloat[1];
      double		gfloat[1];
      float		sfloat[2];
      double		tfloat[1];
      int		slong[2];
      unsigned int	ulong[2];
      short int		sshort[4];
      unsigned short int	ushort[4];
      unsigned char	ubyte[8];
};

typedef union squad B64;

/* V3.24 char  *inst_name_table_by_func[DO_ZZZMAXZZZ];	*/ 

/* define table to hold textual names for all the GPR's */
extern char reg_name_table[MAX_SREG][8];
extern char freg_name_table[MAX_SREG][8];

/* define table to hold textual names for all the GPS's */
extern char ipr_name_table[MAX_IPR][32];				/* v3.25 */
extern char adu_ipr_name_table[MAX_ADU_IPR][16];
extern char pax_ipr_name_table[25][16];


/* note, the following tables, all have 4 longs's per entry even */
/* though only three are used... this allows the /OPTIMIZer in the compiler */
/* to use shift's instead of MULL's to calculate the addresses	*/

/* define table for first level address dispatch for decode */

struct disp_struct
{
      void			(*decode)();
      struct disp_struct	*next;
      char			*text;
      int			 action;
};

typedef struct disp_struct disp_table;

extern disp_table	opcode_table[MAX_OPCODE];

/* define tables for second level address dispatch for decode */
extern disp_table	epi_func_table[MAX_EPI_SUBFUN];
extern disp_table	sync_func_table[MAX_SUBFUN];
extern disp_table	fpieee_func_table[MAX_FPU_SUBFUN];
extern disp_table	fpdec_func_table[MAX_FPU_SUBFUN];
extern disp_table	fpaux_func_table[MAX_FPU_SUBFUN];
extern disp_table	sqrt_func_table[MAX_FPU_SUBFUN];		/* V3.24 */

extern disp_table	addi_func_table[MAX_SUBFUN];
extern disp_table	logi_func_table[MAX_SUBFUN];
extern disp_table	sext_func_table[MAX_SUBFUN];			/* V3.24 */
extern disp_table	shfi_func_table[MAX_SUBFUN];
extern disp_table	muli_func_table[MAX_SUBFUN];
extern disp_table	itof_func_table[MAX_SUBFUN];			/* V3.24 */


#endif
