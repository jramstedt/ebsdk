/********************************************************************************************************************************/
/* Created: 21-JUN-2000 10:36:58 by OpenVMS SDL EV1-33     */
/* Source:   6-MAR-1997 19:27:02 AFW_USER$:[CONSOLE.V58.COMMON.SRC]EV5_DEFS.SDL; */
/********************************************************************************************************************************/
/*** MODULE $EV5DEF ***/
#define EV5$K_REVISION 47               /* Revision number of this file     */
/* In the definitions below, registers are annotated with one of the following */
/* symbols:                                                                 */
/*                                                                          */
/*      RW - The register may be read and written                           */
/*  	RO - The register may only be read                                  */
/*  	WO - The register may only be written                               */
/*                                                                          */
/* For RO and WO registers, all bits and fields within the register are also */
/* read-only or write-only.  For RW registers, each bit or field within     */
/* the register is annotated with one of the following:                     */
/*                                                                          */
/*  	RW - The bit/field may be read and written                          */
/*  	RO - The bit/field may be read; writes are ignored                  */
/*  	WO - The bit/field may be written; reads return an UNPREDICTABLE result. */
/*  	WZ - The bit/field may be written; reads return a 0                 */
/*  	WC - The bit/field may be read; writes cause state to clear         */
/*  	RC - The bit/field may be read, which also causes state to clear; writes are ignored */
/* Architecturally-defined (SRM) registers for EVMS                         */
#define PT0 320                         /* PALtemp0                         */
#define PT1 321                         /* PALtemp1                         */
#define PT2 322                         /* PALtemp2                         */
#define PT3 323                         /* PALtemp3                         */
#define PT4 324                         /* PALtemp4                         */
#define PT5 325                         /* PALtemp5                         */
#define PT6 326                         /* PALtemp6                         */
#define PT7 327                         /* PALtemp7                         */
#define PT8 328                         /* PALtemp8                         */
#define PT9 329                         /* PALtemp9                         */
#define PT10 330                        /* PALtemp10                        */
#define PT11 331                        /* PALtemp11                        */
#define PT12 332                        /* PALtemp12                        */
#define PT13 333                        /* PALtemp13                        */
#define PT14 334                        /* PALtemp14                        */
#define PT15 335                        /* PALtemp15                        */
#define PT16 336                        /* PALtemp16                        */
#define PT17 337                        /* PALtemp17                        */
#define PT18 338                        /* PALtemp18                        */
#define PT19 339                        /* PALtemp19                        */
#define PT20 340                        /* PALtemp20                        */
#define PT21 341                        /* PALtemp21                        */
#define PT22 342                        /* PALtemp22                        */
#define PT23 343                        /* PALtemp23                        */
#define CBOX_IPR_OFFSET 16777200        /* IPR offset for Cbox IPRs - must still be shifted left 16 */
#define SC_CTL 168                      /* Scache Control register (RW)     */
#define SC_STAT 232                     /* Scache Status register (RO)      */
#define SC_ADDR 392                     /* Scache address registor (RO)     */
#define SC_ADDR_NM 392                  /* Scache address registor (RO)     */
#define SC_ADDR_FHM 392                 /* Scache address registor (RO)     */
#define BC_CTL 296                      /* Bcache Control register (WO)     */
#define BC_CONFIG 456                   /* Bcache Configuration register (WO) */
#define EI_STAT 360                     /* External Interface Status Register (RO) */
#define EI_ADDR 328                     /* External Interface Address Register (RO) */
#define FILL_SYN 104                    /* Fill_Syndrome register (RO)      */
#define BC_TAG_ADDR 264                 /* Bcache Tag Address Register (RO) */
#define LD_LOCK 488                     /* Load Lock address register (RO)  */
#define ASTER 266                       /* Aync System Trap Enable (RW)     */
#define ASTRR 265                       /* Aync System Trap Request (RW)    */
#define EXC_ADDR 267                    /* Exception Address register (RW)  */
#define EXC_SUM 268                     /* Exception Summary register (RW)  */
#define EXC_MASK 269                    /* Exception Mask register (RW)     */
#define HWINT_CLR 277                   /* HW Interrupt Clear register (W)  */
#define IC_FLUSH_CTL 281                /* Flushes entire Icache (WO)       */
#define ICPERR_STAT 282                 /* Icache Parity Error status register (RW) */
#define IC_PERR_STAT 282                /* Icache Parity Error status register (RW) */
#define IC_ROW_MAP 283                  /* Bad Icache Mapping Data (RO)     */
#define ICSR 280                        /* Ibox control and status register (RW) */
#define IFAULT_VA_FORM 274              /* Formatted Faulting Virtual Address (R) */
#define INTID 273                       /* Interrupt ID register (R)        */
#define IPL 272                         /* Interrupt Priority Level (RW)    */
#define ISR 256                         /* Interrupt Summary Register (R)   */
#define ITB_IS 263                      /* Istream TB Invalidate Single (W) */
#define ITB_ASN 259                     /* Address Space Number (RW)        */
#define ITB_IA 261                      /* Istream TB Invalidate All (W)    */
#define ITB_IAP 262                     /* Istream TB Invalidate All Process (W) */
#define ITB_PTE 258                     /* Istream TB PTE (RW)              */
#define ITB_PTE_TEMP 260                /* Istream TB PTE Temp (R)          */
#define ITB_TAG 257                     /* Istream TB Tag (W)               */
#define IVPTBR 275                      /* Virtual Page Table Base Register (RW) */
#define PAL_BASE 270                    /* PAL base register (RW)           */
#define PMCTR 284                       /* Performance counter register (RW) */
#define PS 271                          /* Processor Status Register (RW)   */
#define SIRR 264                        /* SW Interrupt Request Register (W) */
#define SL_TXMIT 278                    /* Serial Line Transmit (W)         */
#define SL_RCV 279                      /* Serial Line Transmit (W)         */
#define ALT_MODE 524                    /* Alternate Mode (W)               */
#define CC 525                          /* Cycle Counter (R)                */
#define CC_CTL 526                      /* Cycle Counter Control (W)        */
#define DC_FLUSH 528                    /* Dcache Flush Register (W)        */
#define DCPERR_STAT 530                 /* Dcache Parity Error status register (RW) */
#define DC_TEST_CTL 531                 /* Dcache Test Tag Control Register (RW) */
#define DC_TEST_TAG 532                 /* Dcache Test TAG Register (RW)    */
#define DC_TEST_TAG_TEMP 533            /* Dcache Test TAG Register (RW)    */
#define DTB_ASN 512                     /* Address Space Number (W)         */
#define DTB_CM 513                      /* Dstream TB Current Mode (W)      */
#define DTB_IA 522                      /* Dstream TB Invalidate All (W)    */
#define DTB_IAP 521                     /* Dstream TB Invalidate All Process (W) */
#define DTB_IS 523                      /* Dstream TB Invalidate Single (W) */
#define DTB_PTE 515                     /* Dstream TB PTE (RW)              */
#define DTB_PTE_TEMP 516                /* Istream TB PTE Temp (R)          */
#define DTB_TAG 514                     /* Dstream TB Tag (W)               */
#define MCSR 527                        /* Mbox control and status register (RW) */
#define DC_MODE 534                     /* Dcache Mode register (RW)        */
#define MAF_MODE 535                    /* MAF Mode register (RW)           */
#define MM_STAT 517                     /* Dstream MM Fault Register        */
#define MVPTBR 520                      /* Mbox Virtual Page Table Base Register (W) */
#define VA 518                          /* Faulting Virtual Address (R)     */
#define VA_FORM 519                     /* Formatted Virtual Address (R)    */
#define EV5_SRM$_PS 0                   /* Processor Status (RW)            */
#define EV5_SRM$_PC 0                   /* Program Counter                  */
#define EV5_SRM$_ASTEN 0                /* AST Enable (RW)                  */
#define EV5_SRM$_ASTSR 0                /* AST Summary (RW)                 */
#define EV5_SRM$_IPIR 0                 /* Interproc. Interrupt (W)         */
#define EV5_SRM$_IPL 0                  /* Interrupt Priority Level (RW)    */
#define EV5_SRM$_MCES 0                 /* Machine Check Error Summary (RW) */
#define EV5_SRM$_PCBB 0                 /* Privileged Context Block Bsse (R) */
#define EV5_SRM$_PRBR 0                 /* Processor Base Register (RW)     */
#define EV5_SRM$_PTBR 0                 /* Page Table Bsse Register (R)     */
#define EV5_SRM$_SCBB 0                 /* Sys Control Block Base Register (RW) */
#define EV5_SRM$_SIRR 0                 /* SW Interrupt Request Register (W) */
#define EV5_SRM$_SISR 0                 /* SW Interrupt Summary Register (R) */
#define EV5_SRM$_TBCHK 0                /* TB Check (R)                     */
#define EV5_SRM$_TB1A 0                 /* TB Invalidate All (W)            */
#define EV5_SRM$_TB1AP 0                /* TB Invalidate All Process (W)    */
#define EV5_SRM$_TB1AD 0                /* TB Invalidate All Dstream (W)    */
#define EV5_SRM$_TB1AI 0                /* TB Invalidate All Istream (W)    */
#define EV5_SRM$_TBIS 0                 /* TB Invalidate Single (W)         */
#define EV5_SRM$_KSP 0                  /* Kernel Stack Pointer             */
#define EV5_SRM$_ESP 0                  /* Executive Stack Pointer (RW)     */
#define EV5_SRM$_SSP 0                  /* Supervisor Stack Pointer (RW)    */
#define EV5_SRM$_USP 0                  /* User Stack Pointer (RW)          */
#define EV5_SRM$_VPTB 0                 /* Virtual Page Table Base (RW)     */
#define EV5_SRM$_WHAMI 0                /* Who Am I (R)                     */
#define EV5_SRM$_CC 0                   /* Cycle Counter (RW)               */
#define EV5_SRM$_UNQ 0                  /* Unique (RW)                      */
/* Processor-specific IPRS.                                                 */
#define EV5$_SC_CTL 168                 /* Scache Control register (RW)     */
#define EV5$_SC_STAT 232                /* Scache Status register (RO)      */
#define EV5$_SC_ADDR 392                /* Scache address registor (RO)     */
#define EV5$_BC_CTL 296                 /* Bcache Control register (WO)     */
#define EV5$_BC_CONFIG 456              /* Bcache Configuration register (WO) */
#define BC_CONFIG$K_SIZE_1MB 1          /* Select 1MB Bcache                */
#define BC_CONFIG$K_SIZE_2MB 2          /* Select 2MB Bcache                */
#define BC_CONFIG$K_SIZE_4MB 3          /* Select 4MB Bcache                */
#define BC_CONFIG$K_SIZE_8MB 4          /* Select 8MB Bcache                */
#define BC_CONFIG$K_SIZE_16MB 5         /* Select 16MB Bcache               */
#define BC_CONFIG$K_SIZE_32MB 6         /* Select 32MB Bcache               */
#define BC_CONFIG$K_SIZE_64MB 7         /* Select 64MB Bcache               */
#define EV5$_EI_STAT 360                /* External Interface Status Register (RO) */
#define EV5$_EI_ADDR 328                /* External Interface Address Register (RO) */
#define EV5$_FILL_SYN 104               /* Fill_Syndrome register (RO)      */
#define EV5$_BC_TAG_ADDR 264            /* Bcache Tag Address Register (RO) */
#define EV5$_ASTER 266                  /* Aync System Trap Enable (RW)     */
#define EV5$_ASTRR 265                  /* Aync System Trap Request (RW)    */
#define EV5$_EXC_ADDR 267               /* Exception Address register (RW)  */
#define EXC_ADDR$V_PA 2
#define EXC_ADDR$S_PA 62
#define EV5$_EXC_SUM 268                /* Exception Summary register (RW)  */
#define EV5$_EXC_MASK 269               /* Exception Mask register (RW)     */
#define EV5$_HWINT_CLR 277              /* HW Interrupt Clear register (W)  */
#define EV5$_IC_FLUSH_CTL 281           /* Flushes entire Icache (WO)       */
#define EV5$_ICPERR_STAT 282            /* Icache Parity Error status register (RW) */
#define EV5$_IC_PERR_STAT 282           /* Icache Parity Error status register (RW) */
#define EV5$_IC_ROW_MAP 283             /* Bad Icache Mapping Data (RO)     */
#define EV5$_ICSR 280                   /* Ibox control and status register (RW) */
#define EV5$_IFAULT_VA_FORM 274         /* Formatted Faulting Virtual Address (R) */
#define EV5$_IFAULT_VA_FORM_NT 274      /* Formatted Faulting Virtual Address - NT mode(R) */
#define IFAULT_VA_FORM_NT$V_VPTB 30
#define IFAULT_VA_FORM_NT$S_VPTB 34
#define EV5$_INTID 273                  /* Interrupt ID register (R)        */
#define EV5$_IPL 272                    /* Interrupt Priority Level (RW)    */
#define EV5$_ITB_IS 263                 /* Istream TB Invalidate Single (W) */
#define EV5$_ITB_ASN 259                /* Address Space Number (RW)        */
#define EV5$_ITB_IA 261                 /* Istream TB Invalidate All (W)    */
#define EV5$_ITB_IAP 262                /* Istream TB Invalidate All Process (W) */
#define EV5$_ITB_PTE 258                /* Istream TB PTE (RW)              */
#define EV5$_ITB_PTE_TEMP 260           /* Istream TB PTE Temp (R)          */
#define EV5$_ITB_TAG 257                /* Istream TB Tag (W)               */
#define EV5$_IVPTBR 275                 /* Virtual Page Table Base Register (RW) */
#define IVPTBR$V_VPTB 30
#define IVPTBR$S_VPTB 34
#define EV5$_PAL_BASE 270               /* PAL base register (RW)           */
#define EV5$_PMCTR 284                  /* performance counter register     */
#define EV5$_PS 271                     /* Processor Status Register (RW)   */
#define EV5$_ISR 256                    /* Interrupt Summary Register (R)   */
#define EV5$_SIRR 264                   /* SW Interrupt Request Register (RW) */
#define EV5$_SL_TXMIT 278               /* Serial Line Transmit (W)         */
#define EV5$_SL_RCV 279                 /* Serial Line Transmit (W)         */
#define EV5$_ALT_MODE 524               /* Alternate Mode (W)               */
#define EV5$_CC 525                     /* Cycle Counter (R)                */
#define EV5$_CC_CTL 526                 /* Cycle Counter Control (W)        */
#define EV5$_DC_FLUSH 528               /* Dcache Flush Register (W)        */
#define EV5$_DCPERR_STAT 530            /* Dcache Parity Error status register (RW) */
#define EV5$_DC_TEST_CTL 531            /* Dcache Test TAG Control Register (RW) */
#define EV5$_DC_TEST_TAG 532            /* Dcache Test TAG Register (RW)    */
#define EV5$_DC_TEST_TAG_TEMP 533       /* Dcache Test TAG Temp Register (RW) */
#define EV5$_DTB_ASN 512                /* Address Space Number (W)         */
#define EV5$_DTB_CM 513                 /* Dstream TB Current Mode (W)      */
#define EV5$_DTB_IA 522                 /* Dstream TB Invalidate All (W)    */
#define EV5$_DTB_IAP 521                /* Dstream TB Invalidate All Process (W) */
#define EV5$_DTB_IS 523                 /* Dstream TB Invalidate Single (W) */
#define EV5$_DTB_PTE 515                /* Dstream TB PTE (RW)              */
#define EV5$_DTB_PTE_TEMP 516           /* Dstream TB PTE Temp (R)          */
#define EV5$_DTB_TAG 514                /* Dstream TB Tag (W)               */
#define EV5$_MCSR 527                   /* Mbox control and status register (RW) */
#define EV5$_DC_MODE 534                /* Dcache mode register (RW)        */
#define EV5$_MAF_MODE 535               /* Mbox MAF Mode register (RW)      */
#define EV5$_MM_STAT 517                /* Dstream MM Fault Register        */
#define EV5$_MVPTBR 520                 /* Mbox Virtual Page Table Base Register (W) */
#define EV5$_VA 518                     /* Faulting Virtual Address (R)     */
#define EV5$_VA_FORM 519                /* Formatted Virtual Address (R)    */
#define EV5$_VA_FORM_NT 519             /* Formatted Virtual Address (R)    */
#define VA_FORM_NT$S_VA 19
#define VA_FORM_NT$V_VPTB 30
#define VA_FORM_NT$S_VPTB 34
union EV5_DEF {
/* Cbox registers.                                                          */
    struct  {
        unsigned SC_CTL$V_SC_FHIT : 1;  /* Force Scache hits (RW,0)         */
        unsigned SC_CTL$V_SC_FLUSH : 1; /* Invalidates Scache tag store (RW,0) */
        unsigned SC_CTL$V_SC_TAG_STAT : 6; /* Value of tag status to write in forced mode (RW,0) */
        unsigned SC_CTL$V_SC_FB_DP : 4; /* Bad Scache Data Parity (RW,0)    */
        unsigned SC_CTL$V_SC_BLK_SIZE : 1; /* Scache/Bcache Block size 32 or 64 byte (RW) */
        unsigned SC_CTL$V_SC_SET_EN : 3; /* Scache Set Enable (RW)          */
        unsigned SC_CTL$V_SC_SOFT_REPAIR : 3; /* Soft SCache Tag repair bits */
        unsigned SC_CTL$$$_FILL_0 : 13;
        unsigned SC_CTL$$$_FILL_1 : 32;
        } EV5R_EV5SC_CTL_BITS;
    struct  {
        unsigned SC_STAT$V_SC_TPERR : 3; /* Tag Lookup Error                */
        unsigned SC_STAT$V_SC_DPERR : 8; /* Data Read Error                 */
        unsigned SC_STAT$V_CBOX_CMD : 5; /* Cbox Command that resulted in Error */
        unsigned SC_STAT$V_SC_SCND_ERR : 1; /* Scache Second Error Occurred */
        unsigned SC_STAT$$$_FILL_0 : 15;
        unsigned SC_STAT$$$_FILL_1 : 32;
        } EV5R_EV5SC_STAT_BITS;
    struct  {
        unsigned SC_ADDR_FHM$$$_FILL_0 : 4;
        unsigned SC_ADDR_FHM$V_SC_TAG_PARITY : 1; /* Tag Parity             */
        unsigned SC_ADDR_FHM$V_TAG_STAT_SB0 : 3; /* Tag Status for Sub-block 0 */
        unsigned SC_ADDR_FHM$V_TAG_STAT_SB1 : 3; /* Tag Status for Sub-block 1 */
        unsigned SC_ADDR_FHM$V_OW_MOD0 : 2; /* Octaword modified bits for sublock 0 */
        unsigned SC_ADDR_FHM$V_OW_MOD1 : 2; /* Octaword modified bits for sublock 1 */
        unsigned SC_ADDR_FHM$V_TAG_LO : 17; /* Lo half of Tag               */
        unsigned SC_ADDR_FHM$V_TAG_HI : 7; /* Hi half of Tag                */
        unsigned SC_ADDR_FHM$$$_FILL_1 : 25;
        } EV5R_EV5SC_ADDR_FHM_BITS;
    struct  {
        unsigned BC_CTL$V_BC_ENABLED : 1; /* External Bcache enabled (WO,0) */
        unsigned BC_CTL$V_ALLOC_CYC : 1; /* Allocate cycle for IPR  (WO,1)  */
        unsigned BC_CTL$V_EI_OPT_CMD : 1; /* optional command group (WO,0)  */
        unsigned BC_CTL$V_EI_OPT_CMD_MB : 1; /* optional command group for MB  (WO,0) */
        unsigned BC_CTL$V_CORR_FILL_DAT : 1; /* Correct fill data from bcache/memory (WO,1) */
        unsigned BC_CTL$V_VTM_FIRST : 1; /* Victim Buffer Present (WO,1)    */
        unsigned BC_CTL$V_EI_ECC_OR_PARITY : 1; /* External Interface ECC/parity enable (WO) */
        unsigned BC_CTL$V_BC_FHIT : 1;  /* Bcache Force Hit (WO,0)          */
        unsigned BC_CTL$V_BC_TAG_STAT : 5; /* Tag Status to be written in BC Force Hit mode (WO) */
        unsigned BC_CTL$V_BC_BAD_DAT : 2; /* Force Bad data to be written (WO,0) */
        unsigned BC_CTL$V_EI_DIS_ERR : 1; /* Error enable (WO,0)            */
        unsigned BC_CTL$V_TL_PIPE_LATCH : 1; /* EV5 pipe control pins  (WO,0) */
        unsigned BC_CTL$V_BC_WAVE_PIPE : 2; /* wave pipelining value (WO,0) */
        unsigned BC_CTL$V_PM_MUX_SEL : 6; /* select BIU parameters for perf.mon.  (WO,0) */
        unsigned BC_CTL$V_DBG_MUX_SEL : 1; /* Select cbox signals driven to mbox (WO,0) */
        unsigned BC_CTL$V_DIS_BAF_BYP : 1; /* disables specualtive bcache reads  (WO,0) */
        unsigned BC_CTL$V_DIS_SC_VIC_BUF : 1; /* disables scache victim buffer (WO,0) */
        unsigned BC_CTL$V_DIS_SYS_ADDR_PAR : 1; /* Disable system cmd/addr parity checking */
        unsigned BC_CTL$V_READ_DIRTY_CLN_SHR : 1; /* Enable EV56 read dirty -> shared clean */
        unsigned BC_CTL$V_WRITE_READ_BUBBLE : 1; /* Enable EV56 write_read_bubble */
        unsigned BC_CTL$V_BC_WAVE_PIPE_2 : 1; /* Bit <2> of BC_WAVE_PIPE    */
        unsigned BC_CTL$V_AUTO_DACK : 1; /* Enable EV56 auto dack           */
        unsigned BC_CTL$V_DIS_BYTE_WORD : 1; /* CBox Byte/Word disable      */
        unsigned BC_CTL$V_STCLK_DELAY : 1; /* Enable STCLK delay            */
        unsigned BC_CTL$V_WRITE_UNDER_MISS : 1; /* Enable EV56 victim writes under misses */
        unsigned BC_CTL$$$_FILL_0 : 28;
        } EV5R_EV5BC_CTL_BITS;
    struct  {
        unsigned BC_CONFIG$V_BC_SIZE : 3; /* Bcache size (WO)               */
        unsigned BC_CONFIG$$$_FILL_0 : 1;
        unsigned BC_CONFIG$V_BC_RD_SPD : 4; /* Bcache Read access speed (WO) */
        unsigned BC_CONFIG$V_BC_WR_SPD : 4; /* Bcache Write access speed (WO) */
        unsigned BC_CONFIG$V_BC_RD_WR_SPC : 3; /* Read/Write switch speed (WO) */
        unsigned BC_CONFIG$$$_FILL_1 : 1;
        unsigned BC_CONFIG$V_FILL_WE_OFFSET : 3; /* Bcache Write Enable Pulse Offset (WO) */
        unsigned BC_CONFIG$$$_FILL_2 : 1;
        unsigned BC_CONFIG$V_BC_WE_CTL : 9; /* Bcache Write enable control (WO) */
        unsigned BC_CONFIG$$$_FILL_3 : 3; /* MBZ Reserved bits              */
        unsigned BC_CONFIG$$$_FILL_4 : 31;
        unsigned BC_CONFIG$V_FILL_0 : 1;
        } EV5R_EV5BC_CONFIG_BITS;
/* Cbox registers, continued                                                */
    struct  {
        unsigned EI_STAT$$$_FILL_0 : 24;
        unsigned EI_STAT$V_SYS_ID : 4;  /* Hardwired chip revision number (pass2 = 2) */
        unsigned EI_STAT$V_BC_TPERR : 1; /* Tag Address Parity Error (RO)   */
        unsigned EI_STAT$V_BC_TC_PERR : 1; /* Tag Control Parity Error Mode (WO) */
        unsigned EI_STAT$V_EI_ES : 1;   /* External interface error source (RO) */
        unsigned EI_STAT$V_COR_ECC_ERR : 1; /* Correctable ECC Error (RO)   */
        unsigned EI_STAT$V_UNC_ECC_ERR : 1; /* Uncorrectable ECC Error (RO) */
        unsigned EI_STAT$V_EI_PAR_ERR : 1; /* Fill Parity Error (RO)        */
        unsigned EI_STAT$V_FIL_IRD : 1; /* Fill Error in Icache or Dcache (RO) */
        unsigned EI_STAT$V_SEO_HRD_ERR : 1; /* Second External Interface Hard Error (RO) */
        unsigned EI_STAT$$$_FILL_1 : 28;
        } EV5R_EV5EI_STAT_BITS;
/*                                                                          */
    struct  {
        unsigned BC_TAG_ADDR$$$_FILL_0 : 12;
        unsigned BC_TAG_ADDR$V_HIT : 1; /* Tag Address Parity Error (RO)    */
        unsigned BC_TAG_ADDR$V_TAGCTL_P : 1; /* Tag Control 		(RO) */
        unsigned BC_TAG_ADDR$V_TAGCTL_D : 1; /* Tag Control 		(RO) */
        unsigned BC_TAG_ADDR$V_TAGCTL_S : 1; /* Tag Control  	(RO)        */
        unsigned BC_TAG_ADDR$V_TAGCTL_V : 1; /* Tag Control  	(RO)        */
        unsigned BC_TAG_ADDR$V_TAG_P : 1; /*                                */
        unsigned BC_TAG_ADDR$$$_FILL_1 : 2;
        unsigned BC_TAG_ADDR$V_BC_TAG : 19; /*                              */
        unsigned BC_TAG_ADDR$$$_FILL_2 : 25;
        } EV5R_EV5BC_TAG_ADDR_BITS;
/* Ibox and Icache registers.                                               */
    struct  {
        unsigned ASTER$V_KAR : 1;       /*                                  */
        unsigned ASTER$V_EAR : 1;       /*                                  */
        unsigned ASTER$V_SAR : 1;       /*                                  */
        unsigned ASTER$V_UAR : 1;       /*                                  */
        unsigned ASTER$$$_FILL_0 : 32;
        unsigned ASTER$$$_FILL_1 : 28;
        } EV5R_EV5ASTER_BITS;
    struct  {
        unsigned ASTRR$V_KAR : 1;       /*                                  */
        unsigned ASTRR$V_EAR : 1;       /*                                  */
        unsigned ASTRR$V_SAR : 1;       /*                                  */
        unsigned ASTRR$V_UAR : 1;       /*                                  */
        unsigned ASTRR$$$_FILL_0 : 32;
        unsigned ASTRR$$$_FILL_1 : 28;
        } EV5R_EV5ASTRR_BITS;
    struct  {
        unsigned EXC_ADDR$V_PAL : 1;    /* PAL mode indicator (RW)          */
        unsigned EXC_ADDR$$$_FILL_0 : 1;
        unsigned EXC_ADDR$V_FILL_1 : 6;
        } EV5R_EV5EXC_ADDR_BITS;
    struct  {
        unsigned EXC_SUM$$$_FILL_0 : 10;
        unsigned EXC_SUM$V_SWC : 1;     /* Software Completion Possible (WA) */
        unsigned EXC_SUM$V_INV : 1;     /* Invalid Operation (WA)           */
        unsigned EXC_SUM$V_DZE : 1;     /* Divide by Zero (WA)              */
        unsigned EXC_SUM$V_FOV : 1;     /* FP Overflow (WA)                 */
        unsigned EXC_SUM$V_UNF : 1;     /* FP Underflow (WA)                */
        unsigned EXC_SUM$V_INE : 1;     /* FP Inexact Error (WA)            */
        unsigned EXC_SUM$V_IOV : 1;     /* Integer Overflow (WA)            */
        unsigned EXC_SUM$$$_FILL_1 : 32;
        unsigned EXC_SUM$$$_FILL_2 : 15;
        } EV5R_EV5EXC_SUM_BITS;
    struct  {
        unsigned HWINT_CLR$$$_FILL_0 : 27;
        unsigned HWINT_CLR$V_PC0C : 1;  /* Clears Perf counter 0 interrupt (WO,0) */
        unsigned HWINT_CLR$V_PC1C : 1;  /* Clears Perf counter 1 interrupt (WO,0) */
        unsigned HWINT_CLR$V_PC2C : 1;  /* Clears Perf counter 2 interrupt (WO,0) */
        unsigned HWINT_CLR$$$_FILL_1 : 2;
        unsigned HWINT_CLR$V_CRDC : 1;  /* Clears correctable read interrupt */
        unsigned HWINT_CLR$V_SLC : 1;   /* Clears Serial Line Interrupts (WO,0) */
        unsigned HWINT_CLR$$$_FILL_2 : 29;
        unsigned HWINT_CLR$V_FILL_2 : 1;
        } EV5R_EV5HWINT_CLR_BITS;
/* Ibox and Icache registers, continued                                     */
    struct  {
        unsigned ICPERR_STAT$$$_FILL_0 : 11;
        unsigned ICPERR_STAT$V_DPE : 1; /* Data Parity Error (RW)           */
        unsigned ICPERR_STAT$V_TPE : 1; /* Tag Parity Error (RW)            */
        unsigned ICPERR_STAT$V_TMR : 1; /* timeout reset error              */
        unsigned ICPERR_STAT$$$_FILL_1 : 32;
        unsigned ICPERR_STAT$$$_FILL_2 : 18;
        } EV5R_EV5ICPERR_STAT_BITS;
    struct  {
        unsigned IC_PERR_STAT$$$_FILL_0 : 11;
        unsigned IC_PERR_STAT$V_DPE : 1; /* Data Parity Error (RW)          */
        unsigned IC_PERR_STAT$V_TPE : 1; /* Tag Parity Error (RW)           */
        unsigned IC_PERR_STAT$V_TMR : 1; /* timeout reset error             */
        unsigned IC_PERR_STAT$$$_FILL_1 : 32;
        unsigned IC_PERR_STAT$$$_FILL_2 : 18;
        } EV5R_EV5IC_PERR_STAT_BITS;
    struct  {
        unsigned ICSR$$$_FILL_0 : 8;
        unsigned ICSR$V_PMA : 1;        /* Peformance Monitor All process enable */
        unsigned ICSR$V_PMP : 1;        /* Performance Monitor Process enable */
        unsigned ICSR$$$_FILL_1 : 7;
        unsigned ICSR$V_BYT : 1;        /* Byte Word ECO enable             */
        unsigned ICSR$V_FMP : 1;        /* Forced PC_Mispredict enable      */
        unsigned ICSR$V_MVE : 1;        /* Motion Video instruction set enable */
        unsigned ICSR$V_IM0 : 1;        /* IRQ 20 Mask                      */
        unsigned ICSR$V_IM1 : 1;        /* IRQ 21 Mask                      */
        unsigned ICSR$V_IM2 : 1;        /* IRQ 22 Mask                      */
        unsigned ICSR$V_IM3 : 1;        /* IRQ 23 Mask                      */
        unsigned ICSR$V_TMM : 1;        /* Select timeout counter mode (short/long) (RW) */
        unsigned ICSR$V_TMD : 1;        /* Disable timeout counter (RW)     */
        unsigned ICSR$V_FPE : 1;        /* Enable floating point instructions (RW) */
        unsigned ICSR$V_HWE : 1;        /* Enable PALRES in kernel mode (RW,0) */
        unsigned ICSR$V_SPE : 2;        /* Enable Istream super page mapping (RW,0) */
        unsigned ICSR$V_SDE : 1;        /* Enable PAL shadow registers (RW,0) */
        unsigned ICSR$$$_FILL_2 : 1;
        unsigned ICSR$V_CRDE : 1;       /* Correctable Read Error Enable    */
        unsigned ICSR$V_SLE : 1;        /* Enable serial line interrupts (RW,0) */
        unsigned ICSR$V_FMS : 1;        /* Force Icache miss (RW,0)         */
        unsigned ICSR$V_FBT : 1;        /* Force Bad Icache tag parity (RW,0) */
        unsigned ICSR$V_FBD : 1;        /* Force Bad Icache data parity  (RW,0) */
        unsigned ICSR$V_DBS : 1;        /* Debug Port Select                */
        unsigned ICSR$V_ISTA : 1;
        unsigned ICSR$V_TST : 1;
        unsigned ICSR$$$_FILL_3 : 24;
        } EV5R_EV5ICSR_BITS;
/* removed 17-jun-1992  register removed from design cjs                    */
/* constant IFAULT_VA equals %x108 tag $;			/* Faulting Virtual Address (R) */
/*     EV5IFAULT_VA_BITS structure fill prefix IFAULT_VA$;                  */
/* 	FILL_0 bitfield length 13 fill tag $$;                              */
/* 	VA bitfield length 30 ;				/*                  */
/* 	FILL_1 bitfield length 31 fill tag $$;                              */
/*     end EV5IFAULT_VA_BITS;                                               */
    struct  {
        unsigned IFAULT_VA_FORM$$$_FILL_0 : 3;
        unsigned IFAULT_VA_FORM$V_VA : 30; /*                               */
        unsigned IFAULT_VA_FORM$V_VPTB : 31; /*                             */
        } EV5R_EV5IFAULT_VA_FORM_BITS;
    struct  {
        unsigned IFAULT_VA_FORM_NT$$$_FILL_0 : 3;
        unsigned IFAULT_VA_FORM_NT$V_VA : 19; /*                            */
        unsigned IFAULT_VA_FORM_NT$$$_FILL_1 : 8;
        unsigned IFAULT_VA_FORM_NT$V_FILL_3 : 2;
        } EV5R_EV5IFAULT_VA_FORM_NT_BITS;
    struct  {
        unsigned INTID$V_INTID : 5;     /* Interrupt ID (R)                 */
        unsigned INTID$$$_FILL_0 : 32;
        unsigned INTID$$$_FILL_1 : 27;
        } EV5R_EV5INTID_BITS;
/* Ibox and Icache registers, continued                                     */
    struct  {
        unsigned IPL$V_IPL : 5;         /* Interrupt Priority Level         */
        unsigned IPL$V_FILL_4 : 3;
        } EV5R_EV5IPL_BITS;
    struct  {
        unsigned ITB_IS$$$_FILL_0 : 13;
        unsigned ITB_IS$V_VA : 30;      /*                                  */
        unsigned ITB_IS$$$_FILL_1 : 31;
        unsigned ITB_IS$V_FILL_5 : 6;
        } EV5R_EV5ITB_IS_BITS;
    struct  {
        unsigned ITB_ASN$$$_FILL_0 : 4;
        unsigned ITB_ASN$V_ASN : 7;     /* Address Space Number             */
        unsigned ITB_ASN$$$_FILL_1 : 31;
        unsigned ITB_ASN$V_FILL_6 : 6;
        } EV5R_EV5ITB_ASN_BITS;
    struct  {
        unsigned ITB_PTE$$$_FILL_0 : 4;
        unsigned ITB_PTE$V_ASM : 1;     /*                                  */
        unsigned ITB_PTE$V_GH : 2;      /*                                  */
        unsigned ITB_PTE$$$_FILL_1 : 1; /*                                  */
        unsigned ITB_PTE$V_KRE : 1;     /*                                  */
        unsigned ITB_PTE$V_ERE : 1;     /*                                  */
        unsigned ITB_PTE$V_SRE : 1;     /*                                  */
        unsigned ITB_PTE$V_URE : 1;     /*                                  */
        unsigned ITB_PTE$$$_FILL_2 : 20;
        unsigned ITB_PTE$V_PFN : 27;    /*                                  */
        unsigned ITB_PTE$$$_FILL_3 : 5;
        } EV5R_EV5ITB_PTE_BITS;
    struct  {
        unsigned ITB_PTE_TEMP$$$_FILL_0 : 13;
        unsigned ITB_PTE_TEMP$V_ASM : 1; /*                                 */
        unsigned ITB_PTE_TEMP$$$_FILL_1 : 4;
        unsigned ITB_PTE_TEMP$V_KRE : 1; /*                                 */
        unsigned ITB_PTE_TEMP$V_ERE : 1; /*                                 */
        unsigned ITB_PTE_TEMP$V_SRE : 1; /*                                 */
        unsigned ITB_PTE_TEMP$V_URE : 1; /*                                 */
        unsigned ITB_PTE_TEMP$$$_FILL_2 : 7; /*                             */
        unsigned ITB_PTE_TEMP$V_GH : 3; /*                                  */
        unsigned ITB_PTE_TEMP$V_PFN : 27; /*                                */
        unsigned ITB_PTE_TEMP$$$_FILL_3 : 5;
        } EV5R_EV5ITB_PTE_TEMP_BITS;
/* Ibox and Icache registers, continued                                     */
    struct  {
        unsigned ITB_TAG$$$_FILL_0 : 13;
        unsigned ITB_TAG$V_VA : 30;     /* Virtual Address                  */
        unsigned ITB_TAG$$$_FILL_1 : 21;
        } EV5R_EV5ITB_TAG_BITS;
    struct  {
        unsigned IVPTBR$$$_FILL_0 : 30;
        unsigned IVPTBR$V_FILL_7 : 2;
        } EV5R_EV5IVPTBR_BITS;
    struct  {
        unsigned PAL_BASE$$$_FILL_0 : 14;
        unsigned PAL_BASE$V_PAL_BASE : 26; /* Base Address of PALcode       */
        unsigned PAL_BASE$$$_FILL_1 : 24;
        } EV5R_EV5PAL_BASE_BITS;
    struct  {
        unsigned PMCTR$V_SEL2 : 4;
        unsigned PMCTR$V_SEL1 : 4;
        unsigned PMCTR$V_KILLK : 1;
        unsigned PMCTR$V_KILLP : 1;
        unsigned PMCTR$V_CTL2 : 2;
        unsigned PMCTR$V_CTL1 : 2;
        unsigned PMCTR$V_CTL0 : 2;
        unsigned PMCTR$V_CTR2 : 14;
        unsigned PMCTR$V_KILLU : 1;
        unsigned PMCTR$V_SEL0 : 1;
        unsigned PMCTR$V_CTR1 : 16;
        unsigned PMCTR$V_CTR0 : 16;
        } EV5R_EV5PMCTR_BITS;
    struct  {
        unsigned PS$$$_FILL_0 : 3;
        unsigned PS$V_CM0 : 1;          /*                                  */
        unsigned PS$V_CM1 : 1;          /*                                  */
        unsigned PS$$$_FILL_1 : 32;
        unsigned PS$$$_FILL_2 : 27;
        } EV5R_EV5PS_BITS;
    struct  {
        unsigned ISR$V_ASTRR : 4;       /* AST request                      */
        unsigned ISR$V_SISR : 15;       /* Software Interrupt Summary requests */
        unsigned ISR$V_ATR : 1;         /* AST request                      */
        unsigned ISR$V_I20 : 1;         /* External HW Interrupt at IPL20   */
        unsigned ISR$V_I21 : 1;         /* External HW Interrupt at IPL21   */
        unsigned ISR$V_I22 : 1;         /* External HW Interrupt at IPL22   */
        unsigned ISR$V_I23 : 1;         /* External HW Interrupt at IPL23   */
        unsigned ISR$$$_FILL_0 : 3;
        unsigned ISR$V_PC0 : 1;         /* External HW Interrupt - Performance Counter 0 */
        unsigned ISR$V_PC1 : 1;         /* External HW Interrupt - Performance Counter 1 */
        unsigned ISR$V_PC2 : 1;         /* External HW Interrupt - Performance Counter 2 */
        unsigned ISR$V_PFL : 1;         /* External HW Interrupt - Power Fail */
        unsigned ISR$V_MCK : 1;         /* External HW Interrupt - System Machine Check */
        unsigned ISR$V_CRD : 1;         /*                                  */
        unsigned ISR$V_SLI : 1;         /* Serial Line interrupt            */
        unsigned ISR$V_HLT : 1;         /* External HW Interrupt - Halt     */
        unsigned ISR$$$_FILL_1 : 29;
        } EV5R_EV5ISR_BITS;
/* removed from spec - 5/1/93                                               */
/*constant SICR equals %x108 tag $;			/* SW Interrupt clear Register (W) */
/*    EV5SICR_BITS structure fill prefix SICR$;                             */
/*	FILL_0 bitfield length 4 fill tag $$;                               */
/*	SICR bitfield length 15 ;				/* Software Interrupt request clears */
/*	FILL_1 bitfield length 13 fill tag $$;                              */
/*	FILL_2 bitfield length 32 fill tag $$;                              */
/*    end EV5SICR_BITS;                                                     */
    struct  {
        unsigned SIRR$$$_FILL_0 : 4;
        unsigned SIRR$V_SIRR : 15;      /* Software Interrupt requests      */
        unsigned SIRR$$$_FILL_1 : 13;
        unsigned SIRR$$$_FILL_2 : 32;
        } EV5R_EV5SIRR_BITS;
/* Ibox and Icache registers, continued                                     */
    struct  {
        unsigned SL_TXMIT$$$_FILL_0 : 7;
        unsigned SL_TXMIT$V_TMT : 1;    /*                                  */
        unsigned SL_TXMIT$$$_FILL_1 : 32;
        unsigned SL_TXMIT$$$_FILL_2 : 24;
        } EV5R_EV5SL_TXMIT_BITS;
    struct  {
        unsigned SL_RCV$$$_FILL_0 : 6;
        unsigned SL_RCV$V_RCV : 1;      /*                                  */
        unsigned SL_RCV$$$_FILL_1 : 32;
        unsigned SL_RCV$$$_FILL_2 : 25;
        } EV5R_EV5SL_RCV_BITS;
/* Mbox and Dcache registers.                                               */
    struct  {
        unsigned ALT_MODE$$$_FILL_0 : 3;
        unsigned ALT_MODE$V_AM0 : 1;    /*                                  */
        unsigned ALT_MODE$V_AM1 : 1;    /*                                  */
        unsigned ALT_MODE$$$_FILL_1 : 32;
        unsigned ALT_MODE$$$_FILL_2 : 27;
        } EV5R_EV5ALT_MODE;
    struct  {
        unsigned CC_CTL$$$_FILL_0 : 32;
        unsigned CC_CTL$V_CC_ENA : 1;
        unsigned CC_CTL$$$_FILL_1 : 31;
        } EV5R_EV5CCTL_BITS;
    struct  {
        unsigned DCPERR_STAT$V_SEO : 1; /* Second Error after Register Locked  (W1C) */
        unsigned DCPERR_STAT$V_LOCK : 1; /* Set when register is locked against updates (W1C) */
        unsigned DCPERR_STAT$V_DP0 : 1; /* Data Parity Error in Dcache Bank 0 (W1C) */
        unsigned DCPERR_STAT$V_DP1 : 1; /* Data Parity Error in Dcache Bank 1 (W1C) */
        unsigned DCPERR_STAT$V_TP0 : 1; /* Tag Parity Error in Dcache Bank 0 (W1C) */
        unsigned DCPERR_STAT$V_TP1 : 1; /* Tag Parity Error in Dcache Bank 1 (W1C) */
        unsigned DCPERR_STAT$$$_FILL_0 : 32;
        unsigned DCPERR_STAT$$$_FILL_1 : 26;
        } EV5R_EV5DCPERR_STAT_BITS;
/* The following two registers are used exclusively for test and diagnostics. */
/* They should not be referenced in normal operation.                       */
    struct  {
        unsigned DC_TEST_CTL$V_BANK0 : 1; /* Enable Dcache Bank0 (RW)       */
        unsigned DC_TEST_CTL$V_BANK1 : 1; /* Enable Dcache Bank1 (RW)       */
        unsigned DC_TEST_CTL$V_FILL_0 : 1; /* Ignored bits                  */
        unsigned DC_TEST_CTL$V_INDEX : 10; /* Dcache Tag Index (RW)         */
        unsigned DC_TEST_CTL$V_DATA : 1; /* Data for Dcache soft repair (RW - PCA56 only) */
        unsigned DC_TEST_CTL$V_SHIFT : 1; /* Shift signal for Dcache soft repair (RW/RAZ - PCA56 only) */
        unsigned DC_TEST_CTL$V_LOAD : 1; /* Load signal for Dcache soft repair (RW,0 - PCA56 only) */
        unsigned DC_TEST_CTL$V_FILL_1 : 16;
        unsigned DC_TEST_CTL$V_FILL_2 : 32;
        } EV5R_EV5DC_TEST_CTL_BITS;
/* Mbox and Dcache registers, continued.                                    */
    struct  {
        unsigned DC_TEST_TAG$$$_FILL_0 : 2; /* Ignored bits                 */
        unsigned DC_TEST_TAG$V_TAG_PAR : 1; /* Tag Parity                   */
        unsigned DC_TEST_TAG$$$_FILL_1 : 8; /* Ignored bits                 */
        unsigned DC_TEST_TAG$V_OW0 : 1; /* Octaword valid bit 0             */
        unsigned DC_TEST_TAG$V_OW1 : 1; /* Octaword valid bit 1             */
        unsigned DC_TEST_TAG$V_TAG : 26; /* Tag<38:13>                      */
        unsigned DC_TEST_TAG$$$_FILL_2 : 25; /* Ignored bits                */
        } EV5R_EV5DC_TEST_TAG_BITS;
    struct  {
        unsigned DC_TEST_TAG_TEMP$$$_FILL_0 : 2; /* Ignored bits            */
        unsigned DC_TEST_TAG_TEMP$V_TAG_PAR : 1; /* Tag Parity              */
        unsigned DC_TEST_TAG_TEMP$V_D0P0 : 1; /* Data 0 parity bit 0        */
        unsigned DC_TEST_TAG_TEMP$V_D0P1 : 1; /* Data 0 parity bit 1        */
        unsigned DC_TEST_TAG_TEMP$V_D1P0 : 1; /* Data 1 parity bit 0        */
        unsigned DC_TEST_TAG_TEMP$V_D1P1 : 1; /* Data 1 parity bit 1        */
        unsigned DC_TEST_TAG_TEMP$$$_FILL_1 : 4; /* Ignored bits            */
        unsigned DC_TEST_TAG_TEMP$V_OW0 : 1; /* Octaword valid bit 0        */
        unsigned DC_TEST_TAG_TEMP$V_OW1 : 1; /* Octaword valid bit 1        */
        unsigned DC_TEST_TAG_TEMP$V_TAG : 26; /* Tag<38:13>                 */
        unsigned DC_TEST_TAG_TEMP$$$_FILL_2 : 25; /* Ignored bits           */
        } EV5R_EV5DC_TEST_TAG_TEMP_BITS;
    struct  {
        unsigned DTB_ASN$$$_FILL_0 : 32;
        unsigned DTB_ASN$$$_FILL_1 : 25;
        unsigned DTB_ASN$V_ASN : 7;     /* Address Space Number             */
        } EV5R_EV5DTB_ASN_BITS;
    struct  {
        unsigned DTB_CM$$$_FILL_0 : 3;
        unsigned DTB_CM$V_CM0 : 1;      /*                                  */
        unsigned DTB_CM$V_CM1 : 1;      /*                                  */
        unsigned DTB_CM$$$_FILL_1 : 32;
        unsigned DTB_CM$$$_FILL_2 : 27;
        } EV5R_EV5DTB_CM_BITS;
    struct  {
        unsigned DTBIS$$$_FILL_0 : 13;
        unsigned DTBIS$V_VA0 : 30;      /*                                  */
        unsigned DTBIS$$$_FILL_1 : 21;
        } EV5R_EV5DTBIS_BITS;
    struct  {
        unsigned DTB_PTE$$$_FILL_0 : 1;
        unsigned DTB_PTE$V_FOR : 1;     /*                                  */
        unsigned DTB_PTE$V_FOW : 1;     /*                                  */
        unsigned DTB_PTE$$$_FILL_1 : 1;
        unsigned DTB_PTE$V_ASM : 1;     /*                                  */
        unsigned DTB_PTE$V_GH : 2;      /*                                  */
        unsigned DTB_PTE$$$_FILL_2 : 1; /*                                  */
        unsigned DTB_PTE$V_KRE : 1;     /*                                  */
        unsigned DTB_PTE$V_ERE : 1;     /*                                  */
        unsigned DTB_PTE$V_SRE : 1;     /*                                  */
        unsigned DTB_PTE$V_URE : 1;     /*                                  */
        unsigned DTB_PTE$V_KWE : 1;     /*                                  */
        unsigned DTB_PTE$V_EWE : 1;     /*                                  */
        unsigned DTB_PTE$V_SWE : 1;     /*                                  */
        unsigned DTB_PTE$V_UWE : 1;     /*                                  */
        unsigned DTB_PTE$$$_FILL_3 : 16; /*                                 */
        unsigned DTB_PTE$V_PFN : 27;    /*                                  */
        unsigned DTB_PTE$$$_FILL_4 : 5;
        } EV5R_EV5DTB_PTE_BITS;
/* Mbox and Dcache registers, continued.                                    */
    struct  {
        unsigned DTB_PTE_TEMP$V_FOR : 1; /*                                 */
        unsigned DTB_PTE_TEMP$V_FOW : 1; /*                                 */
        unsigned DTB_PTE_TEMP$V_KRE : 1; /*                                 */
        unsigned DTB_PTE_TEMP$V_ERE : 1; /*                                 */
        unsigned DTB_PTE_TEMP$V_SRE : 1; /*                                 */
        unsigned DTB_PTE_TEMP$V_URE : 1; /*                                 */
        unsigned DTB_PTE_TEMP$V_KWE : 1; /*                                 */
        unsigned DTB_PTE_TEMP$V_EWE : 1; /*                                 */
        unsigned DTB_PTE_TEMP$V_SWE : 1; /*                                 */
        unsigned DTB_PTE_TEMP$V_UWE : 1; /*                                 */
        unsigned DTB_PTE_TEMP$V_ASM : 1; /*                                 */
        unsigned DTB_PTE_TEMP$V_FILL_0 : 2; /*                              */
        unsigned DTB_PTE_TEMP$V_PFN : 27; /*                                */
        unsigned DTB_PTE_TEMP$$$_FILL_1 : 26;
        unsigned DTB_PTE_TEMP$V_FILL_8 : 6;
        } EV5R_EV5DTB_PTE_TEMP_BITS;
    struct  {
        unsigned DTB_TAG$$$_FILL_0 : 13;
        unsigned DTB_TAG$V_VA : 30;     /*                                  */
        unsigned DTB_TAG$$$_FILL_1 : 21;
        } EV5R_EV5DTB_TAG_BITS;
/* Most MCSR bits are used for testability and diagnostics only.            */
/* For normal operation, they will be supported in the following configuration: */
/* SPLIT_DCACHE = 1, MAF_NOMERGE = 0, WB_FLUSH_ALWAYS = 0, WB_NOMERGE = 0,  */
/* DC_ENA<1:0> = 1, DC_FHIT = 0, DC_BAD_PARITY = 0                          */
    struct  {
        unsigned MCSR$V_BIG_ENDIAN : 1; /* Enable Big Endian Mode (RW,0)    */
        unsigned MCSR$V_SP0 : 1;        /* Super Page Mode 1 Enable         */
        unsigned MCSR$V_SP1 : 1;        /* Super Page Mode 2 Enable         */
        unsigned MCSR$V_MBOX_SEL : 1;   /* MBOX Test select (RW,0)          */
        unsigned MCSR$V_E_BIG_ENDIAN : 1; /* Ebox Enable Big Endian Mode (RW,0) */
        unsigned MCSR$V_DBG_PACKET_SEL : 1; /* Parallel port select (RW,0)  */
        unsigned MCSR$$$_FILL_0 : 26;
        unsigned MCSR$$$_FILL_1 : 32;
        } EV5R_EV5MCSR_BITS;
    struct  {
        unsigned DC_MODE$V_DC_ENA : 1;  /* Dcache Enable (RW,0)             */
        unsigned DC_MODE$V_DC_FHIT : 1; /* Dcache Force Hit (RW,0)          */
        unsigned DC_MODE$V_DC_BAD_PARITY : 1; /* Invert Dcache Parity (RW,0) */
        unsigned DC_MODE$V_DC_PERR_DIS : 1; /* Disable Dcache parity error report (RW,0) */
        unsigned DC_MODE$V_DC_DOA : 1;  /* Hardware Dcache Disable (RO,0)   */
        unsigned DC_MODE$$$_FILL_0 : 27;
        unsigned DC_MODE$$$_FILL_1 : 32;
        } EV5R_EV5DC_MODE_BITS;
    struct  {
        unsigned MAF_MODE$V_MAF_NOMERGE : 1; /* MAF merge Disable (RW,0)    */
        unsigned MAF_MODE$V_WB_FLUSH_ALWAYS : 1; /* Flush Write Buffer (RW,0) */
        unsigned MAF_MODE$V_WB_NOMERGE : 1; /* Disables Write Buffer Merging (RW,0) */
        unsigned MAF_MODE$V_IO_NOMERGE : 1; /* Disable MAF merging of non-cached space loads (replaces maf_no_bypass) */
        unsigned MAF_MODE$V_WB_CNT_DISABLE : 1; /* Disables 64-cycle WB counter in the MAF Arb (RW,0) */
        unsigned MAF_MODE$V_MAF_ARB_DISABLE : 1; /* Disables all Dread and WB Req. in the MAF Arb (RW,0) */
        unsigned MAF_MODE$V_DREAD_PENDING : 1; /* MAF Dread status (R,0)    */
        unsigned MAF_MODE$V_WB_PENDING : 1; /* MAF WB status (R,0)          */
        unsigned MAF_MODE$V_WB_SET_LO_THRESHOLD : 2; /* WB begin low priority arb threshold (RW,0 - PCA56 only) */
        unsigned MAF_MODE$V_WB_CLR_LO_THRESHOLD : 2; /* WB stop low priority arb threshold (RW,0 - PCA56 only) */
        unsigned MAF_MODE$$$_FILL_0 : 20;
        unsigned MAF_MODE$$$_FILL_1 : 32;
        } EV5R_EV5MAF_MODE_BITS;
/* Mbox and Dcache registers, continued.                                    */
    struct  {
        unsigned MM_STAT$V_WR : 1;      /* Error caused by WRITE            */
        unsigned MM_STAT$V_ACV : 1;     /* Access Violation                 */
        unsigned MM_STAT$V_FOR : 1;     /* PTE's FOR bit set                */
        unsigned MM_STAT$V_FOW : 1;     /* PTE's FOW bit set                */
        unsigned MM_STAT$V_DTB_MISS : 1; /* DTB miss                        */
        unsigned MM_STAT$V_BAD_VA : 1;  /* Bad VA trap                      */
        unsigned MM_STAT$V_RA : 5;      /* RA field of faulting address     */
        unsigned MM_STAT$V_OPCODE : 6;  /* Opcode field of faulting instruction */
        unsigned MM_STAT$$$_FILL_0 : 15;
        unsigned MM_STAT$$$_FILL_1 : 32;
        } EV5R_EV5MM_STAT_BITS;
    struct  {
        unsigned MVPTBR$$$_FILL_0 : 32;
        unsigned MVPTBR$$$_FILL_1 : 1;
        unsigned MVPTBR$V_VPTB : 31;    /* Virtual Page Table Base          */
        } EV5R_EV5MVPTBR_BITS;
    struct  {
        unsigned VA_FORM$$$_FILL_0 : 3;
        unsigned VA_FORM$V_VA : 30;     /* Virtual Address                  */
        unsigned VA_FORM$V_VPTB : 31;   /* Virtual Page Table Base          */
        } EV5R_EV5VA_FORM_BITS;
    struct  {
        unsigned VA_FORM_NT$$$_FILL_0 : 3;
        unsigned VA_FORM_NT$V_VA : 19;  /* Virtual Address                  */
        unsigned VA_FORM_NT$$$_FILL_1 : 8;
        unsigned VA_FORM_NT$V_FILL_9 : 2;
        } EV5R_EV5VA_FORM_NT_BITS;
    } ;
#define CBOX_CFG 8                      /* Bcache Configuration Register (RW) */
#define CBOX_ADDR 136                   /* Bcache/External Interface Address Register (RO) */
#define CBOX_STAT 264                   /* Bcache/External Interface Status Register (RO) */
#define CBOX_CFG2 392                   /* Bcache Configuration Register 2 (RW) */
#define PCA56$_CBOX_CFG 8               /* Bcache Configuration Register (RW) */
#define CBOX_CFG$K_SIZE_512KB 0         /* Select 512 KB Bcache             */
#define CBOX_CFG$K_SIZE_1MB 1           /* Select 1 MB Bcache               */
#define CBOX_CFG$K_SIZE_2MB 2           /* Select 2 MB Bcache               */
#define CBOX_CFG$K_SIZE_4MB 3           /* Select 4 MB Bcache               */
#define PCA56$_CBOX_ADDR 136            /* Bcache/External Interface Address Register (RO) */
#define PCA56$_CBOX_STAT 264            /* Bcache/External Interface Status Register (RO) */
#define PCA56$_CBOX_CFG2 392            /* Bcache Configuration Register 2 (RW) */
union PCA56_DEF {
/* PCA56 External Interface (CBU) Registers                                 */
    struct  {
        unsigned CBOX_CFG$$$_FILL_0 : 4;
        unsigned CBOX_CFG$V_BC_CLK_RATIO : 4; /* Bcache clock period in CPU cycles (RW,3) */
        unsigned CBOX_CFG$V_BC_LATENCY_OFF : 4; /* Bcache latency offset in CPU cycles (RW,0) */
        unsigned CBOX_CFG$V_BC_SIZE : 2; /* Bcache size (RW,0)              */
        unsigned CBOX_CFG$V_BC_CLK_DELAY : 2; /* Bcache clock delay in CPU cycles (RW,1) */
        unsigned CBOX_CFG$V_BC_RW_OFF : 3; /* Bcache read-to-write offset in CPU cycles (RW,0) */
        unsigned CBOX_CFG$V_BC_PROBE_DURING_FILL : 1; /* Enables Bcache tag probes under fills (RW,0) */
        unsigned CBOX_CFG$V_BC_FILL_DLY_OFF : 3; /* Bcache fill delay offset in CPU cycles (RW,1) */
        unsigned CBOX_CFG$V_IO_PARITY_ENABLE : 1; /* Enable parity checking on I/O reads (RW,0) */
        unsigned CBOX_CFG$V_MEM_PARITY_ENABLE : 1; /* Enable parity checking on memory reads (RW,0) */
        unsigned CBOX_CFG$V_BC_FORCE_HIT : 1; /* Force read/write hits in Bcache (RW,0) */
        unsigned CBOX_CFG$V_BC_FORCE_ERR : 1; /* Force Bcache errors (RW,0) */
        unsigned CBOX_CFG$V_BC_BIG_DRV : 1; /* Enable 50% more drive on selected pins (RW,0) */
        unsigned CBOX_CFG$V_BC_TAG_DATA : 3; /* Tag data to be written when BC_FORCE_HIT = 1 (RW,0) */
        unsigned CBOX_CFG$V_BC_ENABLE : 1; /* External Bcache enabled (RW,0) */
        unsigned CBOX_CFG$$$_FILL_1 : 32;
        } PCA56R_PCA56CBOX_CFG_BITS;
    struct  {
        unsigned CBOX_STAT$$$_FILL_0 : 4;
        unsigned CBOX_STAT$V_SYS_CLK_RATIO : 4; /* System clock period in CPU Cycles (RO,0) */
        unsigned CBOX_STAT$V_CHIP_REV : 4; /* Current revision of chip (RO,0) */
        unsigned CBOX_STAT$V_DATA_PAR_ERR : 4; /* Data read parity error (RO,0) */
        unsigned CBOX_STAT$V_TAG_PAR_ERR : 1; /* Bcache tag store parity error (RO,0) */
        unsigned CBOX_STAT$V_TAG_DIRTY : 1; /* Bcache data modified and not written to memory (RO,0) */
        unsigned CBOX_STAT$V_MEMORY : 1; /* Error on fill data from memory (RO,0) */
        unsigned CBOX_STAT$V_MULTI_ERR : 1; /* Multiple errors detected (RO,0) */
        unsigned CBOX_STAT$$$_FILL_1 : 12;
        unsigned CBOX_STAT$$$_FILL_2 : 32;
        } PCA56R_PCA56CBOX_STAT_BITS;
    struct  {
        unsigned CBOX_CFG2$$$_FILL_0 : 4;
        unsigned CBOX_CFG2$V_BC_REG_REG : 1; /* Bcache is built from REG/REG SSRAM (RW,1) */
        unsigned CBOX_CFG2$V_DBG_SEL : 1; /* Selects CBOX debug information (RW,0) */
        unsigned CBOX_CFG2$V_BC_THREE_MISS : 1; /* Allow 3 read misses to be launched (RW,0) */
        unsigned CBOX_CFG2$V_PUF_DELAY : 1; /* Used to determine tag_ram_output deassert (RW,0) */
        unsigned CBOX_CFG2$V_PM0_MUX : 3; /* Select parameter for performance counter 0 (RW,0) */
        unsigned CBOX_CFG2$V_PM1_MUX : 3; /* Select parameters for performance counter 1 (RW,0) */
        unsigned CBOX_CFG2$$$_FILL_1 : 18;
        unsigned CBOX_CFG2$$$_FILL_2 : 32;
        } PCA56R_PCA56CBOX_CFG2_BITS;
    } ;
 
