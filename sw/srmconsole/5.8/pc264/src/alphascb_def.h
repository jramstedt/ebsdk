/********************************************************************************************************************************/
/* Created: 21-AUG-2000 16:05:20 by OpenVMS SDL EV1-31     */
/* Source:  27-OCT-1997 16:46:43 NIGHTRIDER:[CONSOLE.V58.COMMON.SRC]ALPHASCB_DEF */
/********************************************************************************************************************************/
/* file:	alpha_scb_def.sdl                                           */
/*                                                                          */
/* Copyright (C) 1987, 1988 by                                              */
/* Digital Equipment Corporation, Maynard, Massachusetts.                   */
/* All rights reserved.                                                     */
/*                                                                          */
/* This software is furnished under a license and may be used and copied    */
/* only  in  accordance  of  the  terms  of  such  license  and with the    */
/* inclusion of the above copyright notice. This software or  any  other    */
/* copies thereof may not be provided or otherwise made available to any    */
/* other person.  No title to and  ownership of the  software is  hereby    */
/* transferred.                                                             */
/*                                                                          */
/* The information in this software is  subject to change without notice    */
/* and  should  not  be  construed  as a commitment by digital equipment    */
/* corporation.                                                             */
/*                                                                          */
/* Digital assumes no responsibility for the use  or  reliability of its    */
/* software on equipment which is not supplied by digital.                  */
/*                                                                          */
/*                                                                          */
/* Facility:	Alpha Console                                               */
/*                                                                          */
/* Abstract:	SCB definitions for Alpha                                   */
/*                                                                          */
/* Modifications:                                                           */
/*                                                                          */
/*                                                                          */
/*** MODULE $scbdef ***/
#define SCB$Q_000 0                     /* 000	unused 		unused      */
#define SCB$Q_FLOAT_DISABLE 1           /* 010	float_disable   Floating disabled */
#define SCB$Q_020 2                     /* 020	unused 		unused      */
#define SCB$Q_030 3                     /* 030	unused 		unused      */
#define SCB$Q_040 4                     /* 040	unused 		unused      */
#define SCB$Q_050 5                     /* 050	unused 		unused      */
#define SCB$Q_060 6                     /* 060	unused 		unused      */
#define SCB$Q_070 7                     /* 070	unused 		unused      */
#define SCB$Q_ACCVIO 8                  /* 080	accvio		Access violation */
#define SCB$Q_TNV 9                     /* 090	tnv		Translation not valid */
#define SCB$Q_FAULT_ON_READ 10          /* 0A0  fault_on_read   Fault on data stream read */
#define SCB$Q_FAULT_ON_WRITE 11         /* 0B0  fault_on_write  Fault on data stream write */
#define SCB$Q_FAULT_ON_EXE 12           /* 0C0  fault_on_read   Fault on Istream access */
#define SCB$Q_0D0 13                    /* 0D0	unused 		unused      */
#define SCB$Q_0E0 14                    /* 0E0	unused 		unused      */
#define SCB$Q_0F0 15                    /* 0F0	unused 		unused      */
#define SCB$Q_100 16                    /* 100	unused 		unused      */
#define SCB$Q_110 17                    /* 110	unused 		unused      */
#define SCB$Q_120 18                    /* 120	unused 		unused      */
#define SCB$Q_130 19                    /* 130	unused 		unused      */
#define SCB$Q_140 20                    /* 140	unused 		unused      */
#define SCB$Q_150 21                    /* 150	unused 		unused      */
#define SCB$Q_160 22                    /* 160	unused 		unused      */
#define SCB$Q_170 23                    /* 170	unused 		unused      */
#define SCB$Q_180 24                    /* 180	unused 		unused      */
#define SCB$Q_190 25                    /* 190	unused 		unused      */
#define SCB$Q_1A0 26                    /* 1A0	unused 		unused      */
#define SCB$Q_1B0 27                    /* 1B0	unused 		unused      */
#define SCB$Q_1C0 28                    /* 1C0	unused 		unused      */
#define SCB$Q_1D0 29                    /* 1D0	unused 		unused      */
#define SCB$Q_1E0 30                    /* 1E0	unused 		unused      */
#define SCB$Q_1F0 31                    /* 1F0	unused 		unused      */
#define SCB$Q_ARITHMETIC 32             /* 200	arithmetic	Arithmetic            */
#define SCB$Q_210 33                    /* 210	unused 		unused      */
#define SCB$Q_220 34                    /* 220	unused 		unused      */
#define SCB$Q_230 35                    /* 230	unused 		unused      */
#define SCB$Q_KERNEL_AST 36             /* 240	kernel_ast	Kernel mode AST */
#define SCB$Q_EXEC_AST 37               /* 250	exec_ast	Executive mode AST */
#define SCB$Q_SUPER_AST 38              /* 260	super_ast	Supervisor mode AST */
#define SCB$Q_USER_AST 39               /* 270	user_ast	User mode AST */
#define SCB$Q_UNALIGNED 40              /* 280	unaligned	Unaligned fault */
#define SCB$Q_290 41                    /* 290	unused 		unused      */
#define SCB$Q_2A0 42                    /* 2A0	unused 		unused      */
#define SCB$Q_2B0 43                    /* 2B0	unused 		unused      */
#define SCB$Q_2C0 44                    /* 2C0	unused 		unused      */
#define SCB$Q_2D0 45                    /* 2D0	unused 		unused      */
#define SCB$Q_2E0 46                    /* 2E0	unused 		unused      */
#define SCB$Q_2F0 47                    /* 2F0	unused 		unused      */
#define SCB$Q_300 48                    /* 300	unused 		unused      */
#define SCB$Q_310 49                    /* 310	unused 		unused      */
#define SCB$Q_320 50                    /* 320	unused 		unused      */
#define SCB$Q_330 51                    /* 330	unused 		unused      */
#define SCB$Q_340 52                    /* 340	unused 		unused      */
#define SCB$Q_350 53                    /* 350	unused 		unused      */
#define SCB$Q_360 54                    /* 360	unused 		unused      */
#define SCB$Q_370 55                    /* 370	unused 		unused      */
#define SCB$Q_380 56                    /* 380	unused 		unused      */
#define SCB$Q_390 57                    /* 390	unused 		unused      */
#define SCB$Q_3A0 58                    /* 3A0	unused 		unused      */
#define SCB$Q_3B0 59                    /* 3B0	unused 		unused      */
#define SCB$Q_3C0 60                    /* 3C0	unused 		unused      */
#define SCB$Q_3D0 61                    /* 3D0	unused 		unused      */
#define SCB$Q_3E0 62                    /* 3E0	unused 		unused      */
#define SCB$Q_3F0 63                    /* 3F0	unused 		unused      */
#define SCB$Q_BREAKPOINT 64             /* 400	breakpoint	Breakpoint            */
#define SCB$Q_BUGCHECK 65               /* 410	bugcheck	Bugcheck    */
#define SCB$Q_ILLEGAL_INSTR 66          /* 420	Illegal_instr	Illegal instruction */
#define SCB$Q_ILLEGAL_PAL_OP 67         /* 430  Illegal_pal_op  Illegal PAL operand */
#define SCB$Q_440 68                    /* 440	unused 		unused      */
#define SCB$Q_450 69                    /* 450	unused 		unused      */
#define SCB$Q_460 70                    /* 460	unused 		unused      */
#define SCB$Q_470 71                    /* 470	reserved        Reserved for performance monitor */
#define SCB$Q_CHMK 72                   /* 480	CHMK		CHMK        */
#define SCB$Q_CHME 73                   /* 490	CHME		CHME        */
#define SCB$Q_CHMS 74                   /* 4A0	CHMS		CHMS        */
#define SCB$Q_CHMU 75                   /* 4B0	CHMU		CHMU        */
#define SCB$Q_4C0 76                    /* 4C0	reserved	reserved for Digital */
#define SCB$Q_4D0 77                    /* 4D0	reserved	reserved for Digital */
#define SCB$Q_4E0 78                    /* 4E0	reserved	reserved for Digital */
#define SCB$Q_4F0 79                    /* 4F0	reserved	reserved for Digital */
#define SCB$Q_500 80                    /* 500	unused 		unused      */
#define SCB$Q_SL1 81                    /* 510	SL1     	Software level 1      */
#define SCB$Q_SL2 82                    /* 520	SL2		Software level 2 */
#define SCB$Q_SL3 83                    /* 530	SL3		Software level 3 */
#define SCB$Q_SL4 84                    /* 540	SL4		Software level 4 */
#define SCB$Q_SL5 85                    /* 550	SL5		Software level 5 */
#define SCB$Q_SL6 86                    /* 560	SL6		Software level 6 */
#define SCB$Q_SL7 87                    /* 570	SL7		Software level 7 */
#define SCB$Q_SL8 88                    /* 580	SL8		Software level 8 */
#define SCB$Q_SL9 89                    /* 590	SL9		Software level 9 */
#define SCB$Q_SLA 90                    /* 5A0	SLA		Software level A */
#define SCB$Q_SLB 91                    /* 5B0	SLB		Software level B */
#define SCB$Q_SLC 92                    /* 5C0	SLC		Software level C */
#define SCB$Q_SLD 93                    /* 5D0	SLD		Software level D */
#define SCB$Q_SLE 94                    /* 5E0	SLE		Software level E */
#define SCB$Q_SLF 95                    /* 5F0	SLF		Software level F */
#define SCB$Q_INTV 96                   /* 600	intv		Interval clock interrupt */
#define SCB$Q_INTERPROCESSOR 97         /* 610	interprocessor  Interprocessor interrupt */
#define SCB$Q_SYS_CORR_ERR 98           /* 620	sys_corr_err    System corrected error interrupt */
#define SCB$Q_PR_CORR_ERR 99            /* 630	pr_corr_err	Processor corrected error */
#define SCB$Q_POWER_FAIL 100            /* 640	power_fail	Power failure interrupt */
#define SCB$Q_650 101                   /* 650	unused		Unused      */
#define SCB$Q_SYS_MCHK 102              /* 660	sys_mchk	System machine check */
#define SCB$Q_PR_MCHK 103               /* 670	pr_mchk		Processor machine check  */
#define SCB$Q_680 104                   /* 680	680		Reserved - processor specific */
#define SCB$Q_690 105                   /* 690	690		Reserved - processor specific */
#define SCB$Q_6A0 106                   /* 6A0	6A0		Reserved - processor specific */
#define SCB$Q_6B0 107                   /* 6B0	6B0		Reserved - processor specific */
#define SCB$Q_6C0 108                   /* 6C0  6C0		unused      */
#define SCB$Q_6D0 109                   /* 6D0  6D0		unused      */
#define SCB$Q_6E0 110                   /* 6E0  6E0		unused      */
#define SCB$Q_6F0 111                   /* 6F0	6F0		unused      */
/*                                                                          */
#define SCB_ENTRIES 512
#define SCB_ENTRY_SIZE 16
#define MAX_EXC_VECTOR 103
/*+                                                                         */
/* ===============================================                          */
/* = SCBV - auxillary scb vector                 =                          */
/* ===============================================                          */
/*                                                                          */
/* STRUCTURE OVERVIEW:                                                      */
/*                                                                          */
/*	                                                                    */
/*                                                                          */
/*                                                                          */
/* STRUCTURE MEMBERS:                                                       */
/*                                                                          */
/*:                                                                         */
/*       ---------------------------------------------------------------------- */
/*       Structure Members    | Data Type  | Description                    */
/*       ---------------------------------------------------------------------- */
/*                     routine|	address	   | address of routine to be called */
/*                            |	       	   |                                */
/*                     p0     |	address    | parameter to be passed         */
/*:      ---------------------------------------------------------------------- */
/*                                                                          */
/*-                                                                         */
/*                                                                          */
struct SCBV {
    int scbv$l_pd;                      /* PD for called routine            */
    int scbv$l_p0;
    int scbv$l_p1;
    } ;
 
