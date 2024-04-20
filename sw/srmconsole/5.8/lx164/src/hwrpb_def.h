/********************************************************************************************************************************/
/* Created: 21-JUN-2000 10:29:39 by OpenVMS SDL EV1-33     */
/* Source:  24-APR-2000 10:16:57 AFW_USER$:[CONSOLE.V58.COMMON.SRC]HWRPB_DEF.SDL */
/********************************************************************************************************************************/
/* file:	hwrpb_def.sdl                                               */
/*                                                                          */
/* Copyright (C) 1990, 1991 by                                              */
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
/* Abstract:	Hardware Restart Paramater Block (HWRPB) data structure     */
/*		definitions for Alpha firmware.                             */
/*                                                                          */
/* Author:	David Mayo                                                  */
/*                                                                          */
/* Modifications:                                                           */
/*                                                                          */
/*	ajb	26-nov-1991	Create some constants that are needed in    */
/*				constructing the physical layout of the     */
/*				bootstrap addres space.                     */
/*                                                                          */
/*	jds	24-Oct-1991	Replaced TERM_IO CRB entry with separate entries for each GBUS UART. */
/*                                                                          */
/*	jds	 2-Oct-1991	Updated to conform to Alpha Console Architecture Rev 3.2. */
/*                                                                          */
/*	kl	27-Jun-1991	Update Reason_for_halt constants, add halt_requested */
/*				constants for console SRM ECO 3.1.          */
/*                                                                          */
/*	kl	17-Jun-1991	Replace quadword filler with RXRDY bitfield mask,  */
/*				add/modify SLOT STATE bits, replace ASTEN,ASTSR  */
/*				halt fields with ARGLIST and RETURN. Modify */
/*				Reason_for_Halt constants. This is a subset of the  */
/*				Console SRM ECO 3.1                         */
/*                                                                          */
/*	jds	29-May-1991	Added some CRB/CTB defs; added prefix for   */
/*				CTB defs.                                   */
/*                                                                          */
/*	jds	23-May-1991	Added some CRB defs; added prefix for CRB   */
/*				defs.                                       */
/*                                                                          */
/*	dtm	 5-Dec-1990	CRB and MEMDSC updates                      */
/*                                                                          */
/*	dtm	 9-Nov-1990	Updated for SRM Rev. 2.1                    */
/*                                                                          */
/*	dtm	24-Aug-1990	Initial entry.                              */
/*                                                                          */
/*** MODULE $hwrpbdef ***/
/* CTB bus types                                                            */
#define TURBO_CHANNEL_BUS 0
#define ISA_BUS 1
#define EISA_BUS 2
#define PCI_BUS 3
#define HWRPB$_REVISION 13              /* revision of this hwrpb definition */
/*                                                                          */
/* Structure of HWRPB                                                       */
/*                                                                          */
struct HWRPB {
    unsigned int BASE [2];              /* Physical address of base of HWRPB */
    unsigned int IDENT [2];             /* Identifying string 'HWRPB'       */
    unsigned int REVISION [2];          /* HWRPB version number             */
    unsigned int HWRPB_SIZE [2];        /* HWPRB size                       */
    unsigned int PRIMARY [2];           /* ID of primary processor          */
    unsigned int PAGESIZE [2];          /* System page size in bytes        */
    unsigned int PA_SIZE [2];           /* Physical address size in bits    */
    unsigned int ASN_MAX [2];           /* Maximum ASN value                */
    unsigned int SERIALNUM [2] [2];     /* System serial number             */
    unsigned int SYSTYPE [2];           /* Alpha system type (Laser)        */
    unsigned int SYSVAR [2];            /* Laser system subtype             */
    unsigned int SYSREV [2];            /* System revision                  */
    unsigned int ICLK_FREQ [2];         /* Interval clock interrupt frequency */
    unsigned int CC_FREQ [2];           /* Cycle Counter frequency          */
    unsigned int VPTBR [2];             /* Virtual page table base register */
    unsigned int RSVD_ARCH [2];         /* Reserved for architecture use, SBZ */
    unsigned int THB_OFFSET [2];        /* Offset to Translation Buffer Hint Block */
    unsigned int NPROC [2];             /* Number of processor supported    */
    unsigned int SLOT_SIZE [2];         /* Size of Per-CPU Slots in bytes   */
    unsigned int SLOT_OFFSET [2];       /* Offset to Per-CPU Slots          */
    unsigned int NUM_CTBS [2];          /* Number of CTBs in CTB table      */
    unsigned int CTB_SIZE [2];          /* Size of largest CTB in CTB table */
    unsigned int CTB_OFFSET [2];        /* Offset to Console Terminal Block */
    unsigned int CRB_OFFSET [2];        /* Offset to Console Routine Block  */
    unsigned int MEM_OFFSET [2];        /* Offset to Memory Data Descriptors */
    unsigned int CONFIG_OFFSET [2];     /* Offset to Configuration Data Table */
    unsigned int FRU_OFFSET [2];        /* Offset to FRU Table              */
    unsigned int SAVE_TERM [2];         /* Starting VA of SAVE_TERM routine */
    unsigned int SAVE_TERM_VALUE [2];   /* Procedure Value of SAVE_TERM routine */
    unsigned int RESTORE_TERM [2];      /* Starting VA of RESTORE_TERM routine */
    unsigned int RESTORE_TERM_VALUE [2]; /* Procedure Value of RESTORE_TERM routine */
    unsigned int RESTART [2];           /* VA of restart routine            */
    unsigned int RESTART_VALUE [2];     /* Restart procedure value          */
    unsigned int RSVD_SW [2];           /* Reserved to System Software      */
    unsigned int RSVD_HW [2];           /* Reserved to Hardware             */
    unsigned int CHKSUM [2];            /* Checksum of HWRPB                */
    unsigned int RXRDY [2];             /* RX Ready bitmask                 */
    unsigned int TXRDY [2];             /* TX Ready bitmask                 */
    unsigned int DSRDB_OFFSET [2];      /* Offset to Dynamic System Recognition Data Block */
    } ;
 
/*** MODULE $dsrdbdef ***/
/*                                                                          */
/* Structure of Dynamic System Recognition Data Block                       */
/*                                                                          */
struct DSRDB {
    unsigned int SMM [2];               /* SMM value                        */
    unsigned int LURT_OFFSET [2];       /* Offset to LURT                   */
    unsigned int NAME_OFFSET [2];       /* Offset to NAME                   */
    } ;
 
/*** MODULE $tbhintdef ***/
/*                                                                          */
/* Structure of Translation Buffer Granularity Hint Block                   */
/*                                                                          */
struct TB_HINT {
    short int itb [16];                 /* instruction cache xlation buf hints */
    short int dtb [16];                 /* data cache translation buf hints */
    } ;
 
/*** MODULE $slotdef ***/
/*                                                                          */
/* Structure of Per-CPU Slots                                               */
/*                                                                          */
#define HALT_REQUEST$K_DEFAULT 0
#define HALT_REQUEST$K_SV_RST_TERM 1
#define HALT_REQUEST$K_COLD_BOOT 2
#define HALT_REQUEST$K_WARM_BOOT 3
#define HALT_REQUEST$K_REMAIN_HALTED 4
#define HALT_REQUEST$K_POWER_OFF_SYSTEM 5
#define HALT_REQUEST$K_RESERVED 6
#define HALT_REQUEST$K_MIGRATE 7
#define SLOT$M_BIP 0x1
#define SLOT$M_RC 0x2
#define SLOT$M_PA 0x4
#define SLOT$M_PP 0x8
#define SLOT$M_OH 0x10
#define SLOT$M_CV 0x20
#define SLOT$M_PV 0x40
#define SLOT$M_PMV 0x80
#define SLOT$M_PL 0x100
#define SLOT$M_RSVD_MBZ0 0xFE00
#define SLOT$M_HALT_REQUESTED 0xFF0000
#define SLOT$M_RSVD_MBZ1 0xFF000000
struct SLOT_STATE {
    unsigned SLOT$V_BIP : 1;            /* Boot in progress                 */
    unsigned SLOT$V_RC : 1;             /* Restart capable                  */
    unsigned SLOT$V_PA : 1;             /* Processor available              */
    unsigned SLOT$V_PP : 1;             /* Processor present                */
    unsigned SLOT$V_OH : 1;             /* Operator halted                  */
    unsigned SLOT$V_CV : 1;             /* Context valid                    */
    unsigned SLOT$V_PV : 1;             /* Palcode valid                    */
    unsigned SLOT$V_PMV : 1;            /* Palcode memory valid             */
    unsigned SLOT$V_PL : 1;             /* Palcode loaded                   */
    unsigned SLOT$V_RSVD_MBZ0 : 7;      /* Reserved MBZ                     */
    unsigned SLOT$V_HALT_REQUESTED : 8; /* Halt requested                   */
    unsigned SLOT$V_RSVD_MBZ1 : 8;      /* Reserved MBZ                     */
    unsigned int SLOT$L_RSVD_MBZ2;      /* Reserved MBZ                     */
    } ;
struct SLOT {
    union  {
        struct  {
            unsigned int KSP [2];
            unsigned int ESP [2];
            unsigned int SSP [2];
            unsigned int USP [2];
            unsigned int PTBR [2];
            unsigned int ASN [2];
            unsigned int ASTEN_SR [2];
            unsigned int FEN [2];
            unsigned int CC [2];
            unsigned int UNQ [2];
            unsigned int SCRATCH [2] [6];
            } VMS_HWPCB;
        struct  {
            unsigned int KSP [2];
            unsigned int USP [2];
            unsigned int PTBR [2];
            unsigned int CC;
            unsigned int ASN;
            unsigned int UNQ [2];
            unsigned int FEN [2];
            unsigned int SCRATCH [2] [10];
            } OSF_HWPCB;
        } HWPCB;
    struct SLOT_STATE STATE;
    unsigned int PAL_MEM_LEN [2];
    unsigned int PAL_SCR_LEN [2];
    unsigned int PAL_MEM_ADR [2];
    unsigned int PAL_SCR_ADR [2];
    unsigned int PAL_REV [2];
    unsigned int CPU_TYPE [2];
    unsigned int CPU_VAR [2];
    unsigned int CPU_REV [2];
    unsigned int SERIAL_NUM [2] [2];
    unsigned int PAL_LOGOUT [2];
    unsigned int PAL_LOGOUT_LEN [2];
    unsigned int HALT_PCBB [2];
    unsigned int HALT_PC [2];
    unsigned int HALT_PS [2];
    unsigned int HALT_ARGLIST [2];
    unsigned int HALT_RETURN [2];
    unsigned int HALT_VALUE [2];
    unsigned int HALTCODE [2];
    unsigned int RSVD_SW [2];
    unsigned int RXLEN;
    unsigned int TXLEN;
    char RX [80];
    char TX [80];
    unsigned int PAL_REV_AVAIL [2] [16];
    unsigned int CPU_COMP [2];
    unsigned int CONSOLE_DATA_LOG [2];
    unsigned int CONSOLE_DATA_LOG_LEN [2];
    unsigned int BCACHE_INFO [2];
    unsigned int CC_FREQ [2];
    char RSVD_ARCH [8];
    } ;
#define HALT$K_POWERUP 0
#define HALT$K_OPERATOR_REQUEST_CRASH 1
#define HALT$K_KSTACK_NOTVALID 2
#define HALT$K_SCBB_NOTVALID 3
#define HALT$K_PTBR_NOTVALID 4
#define HALT$K_CALL_PAL_HALT 5
#define HALT$K_DOUBLE_ERROR 6
 
/*** MODULE $memdscdef ***/
/*                                                                          */
/* Memory Descriptor definitions                                            */
/*                                                                          */
struct CLUSTER {
    unsigned int START_PFN [2];
    unsigned int PFN_COUNT [2];
    unsigned int TEST_COUNT [2];
    unsigned int BITMAP_VA [2];
    unsigned int BITMAP_PA [2];
    unsigned int BITMAP_CHKSUM [2];
    unsigned int USAGE [2];
    } ;
struct MEMDSC {
    unsigned int CHECKSUM [2];
    unsigned int IMP_DATA_PA [2];
    unsigned int CLUSTER_COUNT [2];
    struct  {
        unsigned int START_PFN [2];
        unsigned int PFN_COUNT [2];
        unsigned int TEST_COUNT [2];
        unsigned int BITMAP_VA [2];
        unsigned int BITMAP_PA [2];
        unsigned int BITMAP_CHKSUM [2];
        unsigned int USAGE [2];
        } CLUSTER [1];
    } ;
#define MEMDSC_SIZE (sizeof(struct MEMDSC) + (sizeof(struct CLUSTER) * (MEMORY_CLUSTER_COUNT - 1)))
 
/*** MODULE $ctbdef ***/
/*                                                                          */
/* Console Terminal Block definitions                                       */
/*                                                                          */
#define CTB$K_NONE 0
#define CTB$K_PROCESSOR 1
#define CTB$K_DZ_VT 2
#define CTB$K_DZ_GRAPHICS 3
#define CTB$K_DZ_WS 4
struct CTB_VT_table {
    int CSR;                            /* Base CSR address                 */
    int TX_IPL;                         /* TX interrupt IPL                 */
    int TX_SCB;                         /* TX interrupt SCB vector          */
    int RX_IPL;                         /* RX interrupt IPL                 */
    int RX_SCB;                         /* RX interrupt SCB vector          */
    } ;
struct CTB_WS_table {
    unsigned int TERM_OUT_LOC;          /* location/type of output terminal */
    unsigned int TERM_IN_LOC;           /* location/type of input  terminal */
    int DEV_IPL;                        /* Device IPL                       */
    int TX_SCB;                         /* TX interrupt SCB vector          */
    int RX_SCB;                         /* RX interrupt SCB vector          */
    int TYPE;                           /* Terminal type                    */
    } ;
#define CTB$VT_LEN 96                   /* DL/DZ VT device dependent field  */
#define CTB$WS_LEN 352                  /* Workstation device dependent field */
#define TX$M_SCB_OFFSET_H 0x7FFFFFFF
#define TX$M_IE 0x80000000
#define RX$M_SCB_OFFSET_H 0x7FFFFFFF
#define RX$M_IE 0x80000000
struct CTB_VT {
    unsigned int CTB_VT$Q_TYPE [2];
    unsigned int CTB_VT$Q_ID [2];
    unsigned int CTB_VT$Q_RSVD [2];
    unsigned int CTB_VT$Q_DEV_DEP_LEN [2];
    unsigned int CTB_VT$Q_CSR [2];
    unsigned int CTB_VT$L_TX_SCB_OFFSET_L;
    struct  {
        unsigned TX$V_SCB_OFFSET_H : 31;
        unsigned TX$V_IE : 1;
        } CTB_VT$R_TX_INT;
    unsigned int CTB_VT$L_RX_SCB_OFFSET_L;
    struct  {
        unsigned RX$V_SCB_OFFSET_H : 31;
        unsigned RX$V_IE : 1;
        } CTB_VT$R_RX_INT;
    unsigned int CTB_VT$Q_BAUD [2];
    unsigned int CTB_VT$Q_PUTS_STATUS [2];
    unsigned int CTB_VT$Q_GETC_STATUS [2];
    unsigned char CTB_VT$B_CON_RSVD [64];
    } ;
#define TX$M_SCB_OFFSET_H 0x7FFFFFFF
#define TX$M_IE 0x80000000
#define RX$M_SCB_OFFSET_H 0x7FFFFFFF
#define RX$M_IE 0x80000000
struct CTB_WS {
    unsigned int CTB_WS$Q_TYPE [2];
    unsigned int CTB_WS$Q_ID [2];
    unsigned int CTB_WS$Q_RSVD [2];
    unsigned int CTB_WS$Q_DEV_DEP_LEN [2];
    unsigned int CTB_WS$Q_DEV_IPL [2];
    unsigned int CTB_WS$L_TX_SCB_OFFSET_L;
    struct  {
        unsigned TX$V_SCB_OFFSET_H : 31;
        unsigned TX$V_IE : 1;
        } CTB_WS$R_TX_INT;
    unsigned int CTB_WS$L_RX_SCB_OFFSET_L;
    struct  {
        unsigned RX$V_SCB_OFFSET_H : 31;
        unsigned RX$V_IE : 1;
        } CTB_WS$R_RX_INT;
    unsigned int CTB_WS$Q_TERM_TYPE [2];
    unsigned int CTB_WS$Q_KB_TYPE [2];
    unsigned int CTB_WS$Q_KB_TRN_TBL [2];
    unsigned int CTB_WS$Q_KB_MAP_TBL [2];
    unsigned int CTB_WS$Q_KB_STATE [2];
    unsigned int CTB_WS$Q_LAST_KEY [2];
    unsigned int CTB_WS$Q_US_FONT [2];
    unsigned int CTB_WS$Q_MCS_FONT [2];
    unsigned int CTB_WS$Q_FONT_WIDTH [2];
    unsigned int CTB_WS$Q_FONT_HEIGHT [2];
    unsigned int CTB_WS$Q_MONITOR_WIDTH [2];
    unsigned int CTB_WS$Q_MONITOR_HEIGHT [2];
    unsigned int CTB_WS$Q_MONITOR_DENSITY [2];
    unsigned int CTB_WS$Q_NUM_PLANES [2];
    unsigned int CTB_WS$Q_CURSOR_WIDTH [2];
    unsigned int CTB_WS$Q_CURSOR_HEIGHT [2];
    unsigned int CTB_WS$Q_NUM_HEADS [2];
    unsigned int CTB_WS$Q_OPWIN [2];
    unsigned int CTB_WS$Q_HEAD_OFFSET [2];
    unsigned int CTB_WS$Q_PUTCHAR_ROUTINE [2];
    unsigned int CTB_WS$Q_IO_STATE [2];
    unsigned int CTB_WS$Q_LISTENER_STATE [2];
    unsigned int CTB_WS$Q_EXT_INFO [2];
    unsigned int CTB_WS$Q_TERM_OUT_LOC [2];
    unsigned int CTB_WS$Q_SERVER_OFFSET [2];
    unsigned int CTB_WS$Q_LINE_PARAM [2];
    unsigned int CTB_WS$Q_TERM_IN_LOC [2];
    unsigned char CTB_WS$B_CON_RSVD [80];
    } ;
 
/*** MODULE $crbdef ***/
/*                                                                          */
/* Structure of Console Routine Block (CRB)                                 */
/*                                                                          */
#define CRB$K_CODE 0                    /* CRB entry for code.              */
#define CRB$K_HEAP 1                    /* CRB entry for heap.              */
#define CRB$K_ENTRIES 2
struct CRB_ENTRY {
    unsigned int CRB_ENTRY$Q_V_ADDRESS [2];
    unsigned int CRB_ENTRY$Q_P_ADDRESS [2];
    unsigned int CRB_ENTRY$Q_PAGE_COUNT [2];
    } ;
#define CRB_ENTRY$K_SIZE 24
struct CRB {
    unsigned int CRB$Q_VDISPATCH [2];
    unsigned int CRB$Q_PDISPATCH [2];
    unsigned int CRB$Q_VFIXUP [2];
    unsigned int CRB$Q_PFIXUP [2];
    unsigned int CRB$Q_ENTRIES [2];
    unsigned int CRB$Q_PAGES [2];
    struct CRB_ENTRY CRB$R_ENTRY [2];
    } ;
#define CRB$K_SIZE 96
/*                                                                          */
/* Definitions for the physical layout of the bootstrap space               */
/*                                                                          */
 
/*** MODULE $bootdef ***/
#define BOOT_BSSPAGES 32
#define BOOT_STACKPAGES 2
#define BOOT_GUARDPAGES 1
#define BOOT_L0PAGES 1                  /* for alpha                        */
#define BOOT_L1PAGES 1                  /* for alpha                        */
#define BOOT_L2PAGES 1                  /* for alpha                        */
#define BOOT_L30PAGES 2                 /* for alpha                        */
#define BOOT_L31PAGES 4                 /* for alpha                        */
#define BOOT_PTEPAGES 9
 
