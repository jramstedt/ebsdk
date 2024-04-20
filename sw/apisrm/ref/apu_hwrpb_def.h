/********************************************************************************************************************************/
/* Created  2-FEB-1994 06:59:09 by VAX SDL V3.2-12     Source:  2-FEB-1994 06:58:03 MORGN:[PEACOCK.AVANTI.TOOLS]HWRPB_DEF.SDI;5 */
/********************************************************************************************************************************/
/* file:	hwrpb_def.sdl                                               */
/*                                                                          */
/* Copyright (C) 1990 by                                                    */
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
/*	DTR	15-Jun-1992	Added a contant for the CTB so that the hrwpb */
/*				base could be located as a reference to this. */
/*				                                            */
/*	tlc	28-jan-1992	change memdsc data types to int64           */
/*                                                                          */
/*      jrh	13-Sep-1991	Added in space for a SCB. This must be      */
/*				page aligned (8kb boundary)                 */
/*                                                                          */
/*	dtr	12-Aug-1981	Removed ctb and crb definitions.  They are  */
/*				separate files now.  Space is the only      */
/*				thing allocated here.                       */
/*				                                            */
/*	dtr	3-jun-1991	Updated to refelct the overall structure of the */
/*				hwrpb for flamingo.  This is no longer      */
/*				compatible with cobra/laser design.  The basic */
/*				structure remains the same but there are    */
/*				flamingo (******) specific definitions in it. */
/*				                                            */
/*	jrh	30-May-1991	Update to reflect HWRPB revision 2 and      */
/*				added TAGS to all structures                */
/*                                                                          */
/*	dtm	 5-Dec-1990	CRB and MEMDSC updates                      */
/*                                                                          */
/*	dtm	 9-Nov-1990	Updated for SRM Rev. 2.1                    */
/*                                                                          */
/*	dtm	24-Aug-1990	Initial entry.                              */
/*                                                                          */
/*                                                                          */
 
/*** MODULE $hwrpbdef ***/
#define HWRPB$_REVISION 2               /* revision of this hwrpb definition */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/* Definition of the state bits                                             */
/*                                                                          */
#define SLOT$M_BIP 1
#define SLOT$M_RC 2
#define SLOT$M_PA 4
#define SLOT$M_PP 8
#define SLOT$M_OH 16
#define SLOT$M_CV 32
#define SLOT$M_PV 64
#define SLOT$M_PMV 128
#define SLOT$M_PL 256
struct STATE {
    unsigned SLOT$V_BIP : 1;
    unsigned SLOT$V_RC : 1;
    unsigned SLOT$V_PA : 1;
    unsigned SLOT$V_PP : 1;
    unsigned SLOT$V_OH : 1;
    unsigned SLOT$V_CV : 1;
    unsigned SLOT$V_PV : 1;
    unsigned SLOT$V_PMV : 1;
    unsigned SLOT$V_PL : 1;
    unsigned SLOT$v_filler1 : 7;
    unsigned SLOT$V_HALT_REQ : 8;
    unsigned SLOT$v_filler2 : 8;
    unsigned int SLOT$l_filler3;
    } ;
/*                                                                          */
/* Structure of Per-CPU Slots                                               */
/*                                                                          */
struct SLOT {
    struct  {
        int64 hwpcb$Q_KSP;
        int64 hwpcb$Q_ESP;
        int64 hwpcb$Q_SSP;
        int64 hwpcb$Q_USP;
        int64 hwpcb$Q_PTBR;
        int64 hwpcb$Q_ASN;
        int64 hwpcb$Q_ASTEN_SR;
        int64 hwpcb$Q_FEN;
        int64 hwpcb$Q_CC;
        int64 hwpcb$Q_SCRATCH [7];
        } SLOT$R_HWPCB;
    struct STATE SLOT$R_STATE_FLAGS;
    int64 SLOT$Q_PAL_MEM_LEN;
    int64 SLOT$Q_PAL_SCR_LEN;
    int64 SLOT$Q_PAL_MEM_ADR;
    int64 SLOT$Q_PAL_SCR_ADR;
    int SLOT$L_PAL_REV;
    int SLOT$L_PAL_VAR;
    int64 SLOT$Q_CPU_TYPE;
    int64 SLOT$Q_CPU_VAR;
    int64 SLOT$Q_CPU_REV;
    int64 SLOT$Q_SERIAL_NUM [2];
    int64 SLOT$Q_LOGOUT_MEM_ADR;
    int64 SLOT$Q_LOGOUT_LENGTH;
    int64 SLOT$Q_HALT_PCBB;
    int64 SLOT$Q_HALT_PC;
    int64 SLOT$Q_HALT_PS;
    int64 SLOT$Q_HALT_ARGLIST;
    int64 SLOT$Q_HALT_RET_ADDR;
    int64 SLOT$Q_HALT_PROC_VALUE;
    int64 SLOT$Q_HALTCODE;
    int64 SLOT$Q_RSVD_SW;
    int64 SLOT$Q_ICBA [21];             /* interprocessor communications (not */
/* used on flamingo                                                         */
    int64 SLOT$Q_PALCODE_REVS [16];     /* PALcode revisions available.     */
/* offset 0 not used                                                        */
/* Offset 1 EVMS version                                                    */
/* Offset 2 OSF version                                                     */
/* remainder reserved.                                                      */
    char SLOT$b_hwpcb_filler [176];
/*                                                                          */
/* DSRDB data structure ... this will not/ can not  move                    */
/*                                                                          */
    int64 SLOT$q_dsrdb_smm;
    int64 SLOT$q_dsrdb_lurt_off;
    int64 SLOT$q_dsrdb_name_off;
    int64 SLOT$q_dsrdb_lurt_count;
    int64 SLOT$q_dsrdb_lurt_table [20];
    int64 SLOT$q_dsrdb_name_count;
    int64 SLOT$q_dsrdb_name [7];
    } ;
#define HALT$K_BOOTSTRAP 0
#define HALT$K_POWERUP 1
#define HALT$K_OPERATOR_HALT 2
#define HALT$K_OPERATOR_CRASH 3
#define HALT$K_KSTACK_NOTVALID 4
#define HALT$K_SCBB_NOTVALID 5
#define HALT$K_PTBR_NOTVALID 6
#define HALT$K_UNKNOWN_REQUEST 7
#define HALT$K_DOUBLE_ERROR 8
/*                                                                          */
/* Memory Descriptor definitions                                            */
/*                                                                          */
struct CLUSTER {
    int64 MEMDSC$Q_START_PFN;
    int64 MEMDSC$Q_PFN_COUNT;
    int64 MEMDSC$Q_TEST_COUNT;
    int64 MEMDSC$Q_BITMAP_VA;
    int64 MEMDSC$Q_BITMAP_PA;
    int64 MEMDSC$Q_BITMAP_CHKSUM;
    int64 MEMDSC$Q_USAGE;
    } ;
struct MEMDSC {
    int64 memdsc$Q_CHECKSUM;
    int64 memdsc$Q_RSVD;
    int64 memdsc$Q_CLUSTER_COUNT;
    struct CLUSTER memdsc$r_cluster [3];
    } ;
/*                                                                          */
/* Console Terminal Block definitions                                       */
/*                                                                          */
#define CTB$K_NONE 0
#define CTB$K_PROCESSOR 1
#define CTB$K_DZ_VT 2
#define CTB$K_DZ_GRAPHICS 3
#define CTB$K_NETWORK 4
/*                                                                          */
/* This note describes the layout of the HWRPB and all of the               */
/* associated memory needed by the pal code and console program. It         */
/* does not define the virtual address of any of it, only the offsets       */
/* for the HWRPB.                                                           */
/*                                                                          */
/* Structure of HWRPB - This is the overall layout of the HWRPB for the     */
/* Flamingo system.  It is divided into several fixed sections rather       */
/* than dynamic sections.                                                   */
/*                                                                          */
/* The first fixed section is the portion of the HWRPB that contains        */
/* all of the fixed information in the HWRPB.  This information             */
/* includes HWRPB version, processor type and variations, page size,        */
/* cycle count value, clock interrupt frequency and the offsets to the      */
/* rest of the structures needed by the console and OS.  The size of        */
/* this is 512 bytes.                                                       */
/*                                                                          */
/* The second section is the CTB which defines the console data. This       */
/* structure contains the physical and virtual addresses of all of the      */
/* data structures needed by the console.  The structure of this is         */
/* still being refined but will contain at least the fixed data             */
/* associated with the console as well as any of the device driver          */
/* descriptors the console will need in order to function.  This            */
/* structure is fixed at 512 bytes.                                         */
/*                                                                          */
/* The third structure is the console routine block.  This is defined       */
/* by the SRM and contains the VA and PA of the dispatch routine, fixup     */
/* routine and a map of all of the I/O segments the console needs to        */
/* function.  The I/O segments are for the entire console system and        */
/* includes the serial line addresses, the Ethernet controller              */
/* addresses, the SCSI addresses and any other addresses that the           */
/* devices will need.  In some cases there may be duplicates of             */
/* addresses within the table but to make the code port easier they         */
/* will be replicated.  The base address of the NVR is one that comes       */
/* to mind.  The size of this is 6616 bytes of which 512 are defined.       */
/* This size allows for 29 I/O segments to be mapped and used.              */
/*                                                                          */
/* The fourth segment is the ROM code.  It contains all of the pal          */
/* code, the console code, selftest code, and any other support             */
/* routines.  This region is 1/2 megabyte in length and is the entire       */
/* contents of the ROMs on the system.                                      */
/*                                                                          */
/* The fifth segment of this data structure is the pal impure data          */
/* area.  A full page has been set aside for this, 8192 bytes.  This        */
/* allows for expansion in the future should the amount of space            */
/* required currently change.                                               */
/*                                                                          */
/* The sixth segment is the console storage area.  The current              */
/* implementation assumes 8192 bytes in length but this may change to       */
/* include all of the special linkage sections needed by the console        */
/* when it is running.                                                      */
/*                                                                          */
/* The seventh segment contains the page tables to initially map all of     */
/* the HWRPB, the pal code, console code and every thing else on a          */
/* VA=PA scheme.  This section requires a total of 5 pages.  1 page for     */
/* L1 page table, 3 pages for L3 pages tables and 1 page for L2 page        */
/* table.  The L3 page tables are broken down into 1 page for the boot      */
/* address space, 1 page for the HWRPB address space, 1 page for the        */
/* pal code address space.  (the third is not used by any operating         */
/* system software and is used merely to map memory 1 for 1.  At this       */
/* point in time it is sincerely hoped that this physical to virtual        */
/* mapping can never exceed the 256meg region set aside as the              */
/* beginning of the HWRPB address space.                                    */
/*                                                                          */
/* The eighth segment is the amount of free space.  This area will          */
/* contain the memory descriptors and any other dynamic functions.  The     */
/* size of this is 424kb.  This seems like a lot of memory to set aside     */
/* for this purpose but a lot will be needed for the maps, and linkage      */
/* sections that will be needed.                                            */
/*                                                                          */
/* The ninth and final segment is the stack frame.  Three pages, 24kb,      */
/* has been set aside for this.  If anymore is needed then we have done     */
/* something wrong.                                                         */
/*                                                                          */
/*                                                                          */
/* 	 This is the physical layout of the words I used above.  The say    */
/* 	 a picture is worth a thousand words.  I sure hope so.              */
/*                                                                          */
/* 	 +----------------------+                                           */
/* 	 |    Fixed portion	|----- Offset 0 - 512 bytes (.5k)           */
/* 	 |      of HWRPB	|                                           */
/* 	 +----------------------+                                           */
/* 	 |    Per CPU SLOT	|-----Offset 200(16) - 512 bytes (.5k)      */
/* 	 |			|                                           */
/* 	 +----------------------+                                           */
/* 	 |   Console Terminal	|-----Offset 400(16) - 512 bytes (.5k)      */
/* 	 |	 Block		|                                           */
/* 	 +----------------------+                                           */
/* 	 |   Console Routine	|-----Offset 600(16) - 6616 bytes (6.5k)    */
/* 	 |	 Block		|                                           */
/* 	 +----------------------+                                           */
/* 	 |    ROM SET DATA     	|-----Offset 4000(16) - 524288 bytes (512k) */
/* 	 |                   	|                                           */
/* 	 +----------------------+                                           */
/* 	 |    PAL IMPURE DATA  	|-----Offset 84000(16) - 8192 bytes (8k)    */
/* 	 |                   	|                                           */
/* 	 +----------------------+                                           */
/* 	 |    Console Storage  	|-----Offset 86000(16) - 8192 bytes (8k)    */
/* 	 |                   	|                                           */
/* 	 +----------------------+                                           */
/* 	 |    Reserved        	|-----Offset 88000(16) - 40960 bytes (40k)  */
/* 	 | (was PTEs)        	|                                           */
/* 	 +----------------------+                                           */
/* 	 |   Reserved memory  	|-----Offset 92000(16) - 434176 bytes (424k) */
/* 	 |                   	|                                           */
/* 	 +----------------------+                                           */
/* 	 |       STACK     	|-----Offset FA000(16) - 24576 bytes (24k)  */
/* 	 |                   	|                                           */
/* 	 +----------------------+                                           */
/* 	 |       L1 PTE    	|-----Offset 100000(16) - 8192 bytes (8k)   */
/* 	 |                   	|                                           */
/* 	 +----------------------+                                           */
/* 	 |       L2 PTE    	|-----Offset 102000(16) - 8192 bytes (8k)   */
/* 	 |                   	|                                           */
/* 	 +----------------------+                                           */
/* 	 |       L3 PTE    	|-----Offset 104000(16) - 8192 bytes (8k)   */
/* 	 |      Region 1     	|                                           */
/* 	 +----------------------+                                           */
/* 	 |       L3 PTE    	|-----Offset 106000(16) - 229376 bytes (224k) */
/* 	 |      Region 0       	|                                           */
/* 	 +----------------------+                                           */
/*                                                                          */
/*                                                                          */
/* Define the system variation fields and constants in the HWRPB            */
/*                                                                          */
#define mcap 1
#define reserved 0
#define detatched_console 1
#define embedded_console 2
#define reserved 0
#define united 1
#define separate 2
#define battery_backup 3
#define hwrpb$m_mpcap 1
#define hwrpb$m_console 30
#define hwrpb$m_powerfail 224
#define hwrpb$m_powerfail_restart 256
struct sysvar {
    unsigned hwrpb$v_mpcap : 1;
    unsigned hwrpb$v_console : 4;
    unsigned hwrpb$v_powerfail : 3;
    unsigned hwrpb$v_powerfail_restart : 1;
    unsigned hwrpb$v_fill_0 : 7;
    } ;
#define HWRPB$K_IT_CLOCK 4194304
#define HWRPB$K_CYCLE_FREQUENCY 100000000
#define hwrpb$K_CHKSUM 288
#define hwrpb$K_TEMP_FILL 512
#define hwrpb$K_TEMP_FILL2 384
/*                                                                          */
#define hwrpb$k_ctb 1536
/* made the CTB consistant with the definition file.                        */
#define hwrpb$k_crb 14848
#define hwrpb$k_top_of_stack 1179648
/* we now set aside 256k for page tables.  This is the max that is          */
#define hwrpb$k_page_count 32764
struct HWRPB {
    int64 hwrpb$Q_BASE;                 /* Physical address of base of HWRPB */
    int64 hwrpb$Q_IDENT;                /* Identifying string 'HWRPB'       */
    int64 hwrpb$Q_REVISION;             /* HWRPB version number             */
    int64 hwrpb$Q_SIZE;                 /* Size of the HWRPB in bytes       */
    int64 hwrpb$Q_PRIMARY;              /* ID of primary processor          */
    int64 hwrpb$Q_PAGESIZE;             /* System page size in bytes        */
    int64 hwrpb$Q_PA_SIZE;              /* Physical address size in bits    */
    int64 hwrpb$Q_ASN_MAX;              /* Maximum ASN value                */
    int64 hwrpb$Q_SERIALNUM [2];        /* System serial number             */
    int64 hwrpb$Q_SYSTYPE;              /* Alpha system type (Laser)        */
    int64 hwrpb$Q_SYSVAR;               /* System subtype                   */
    int64 hwrpb$Q_SYSREV;               /* System revision                  */
    int64 hwrpb$Q_IT_FREQ;              /* Interval Timer frequency         */
    int64 hwrpb$Q_CC_FREQ;              /* Cycle Counter frequency          */
    int64 hwrpb$Q_VPTB_VALUE;           /* Virtual page table base          */
    int64 hwrpb$Q_RSVD_ARCH;            /* reserve for architecture         */
    int64 hwrpb$Q_TBHINT_OFFSET;        /* offset to tb hint block          */
    int64 hwrpb$Q_NPROC;                /* Number of processor slots        */
    int64 hwrpb$Q_SLOT_SIZE;            /* Size of Per-CPU Slots in bytes   */
    int64 hwrpb$Q_SLOT_OFFSET;          /* Offset to Per-CPU Slots          */
    int64 hwrpb$Q_CTB_COUNT;            /* number of CTBs                   */
    int64 hwrpb$Q_CTB_SIZE;             /* Size of the CTB                  */
    int64 hwrpb$Q_CTB_OFFSET;           /* Offset to Console Terminal Block */
    int64 hwrpb$Q_CRB_OFFSET;           /* Offset to Console Routine Block  */
    int64 hwrpb$Q_MEM_OFFSET;           /* Offset to Memory Data Descriptors */
    int64 hwrpb$Q_CONFIG_OFFSET;        /* Offset to Configuration Data block */
    int64 hwrpb$Q_FRU_OFFSET;           /* Offset to FRU Table              */
    int64 hwrpb$Q_SAVE_STATE;           /* Physical address of save state   */
    int64 hwrpb$Q_SAVE_STATE_VALUE;
    int64 hwrpb$Q_RESTORE_STATE;        /* physical address of restore state */
    int64 hwrpb$Q_RESTORE_STATE_VALUE;
    int64 hwrpb$Q_RESTART;              /* VA of restart routine            */
    int64 hwrpb$Q_RESTART_VALUE;        /* Restart procedure value          */
    int64 hwrpb$Q_RSVD_SW;              /* Reserved to System Software      */
    int64 hwrpb$Q_RSVD_HW;              /* Reserved to Hardware             */
    int64 hwrpb$Q_CHKSUM;               /* Checksum of HWRPB                */
    int64 hwrpb$Q_RXRDY;                /* RX Ready bitmask                 */
    int64 hwrpb$Q_TXRDY;                /* TX Ready bitmask                 */
    int64 hwrpb$Q_DSRDB_OFFSET;         /* Offset to DSRDB                  */
    int64 hwrpb$Q_TB_HINT_BLOCK [8];
/* This next statement forces the fixed portion of the HWRPB to be          */
/* aligned on a 512 byte boundary                                           */
/*                                                                          */
    int64 hwrpb$q_hwrpb_filler [16];
/* ********* per cpu slot - one per cpu - only one on flamingo              */
    struct SLOT hwrpb$r_per_cpu;
/* per cpu has been expanded to 1024 because of the new ECOs                */
/* ********* flamingo - fixed at 512 for now.                               */
    char hwrpb$b_ctb [12288];
    unsigned char hwrpb$b_crb [1024];
    unsigned char hwrpb$b_pcb [512];
    unsigned char hwrpb$b_memdsc [1024]; /* must be aligned 16k             */
    unsigned char hwrpb$b_rom_data [720896];
    unsigned char hwrpb$b_pal_impure [8192];
    unsigned char hwrpb$b_console_scratch [8192];
    unsigned char hwrpb$b_mcheck_logout [8192];
    unsigned char hwrpb$b_rom_scb [8192];
    char hwrpb$b_env_var_buffer [8192];
/* dyamic memory stuff.                                                     */
    unsigned char hwrpb$b_reserved_memory [270336];
/* set aside 13 pages for NI buffer - 64kb aligned                          */
    unsigned char hwrpb$b_ni_buffer [106496];
/* reserve 3 pages (24k) for stack                                          */
    unsigned char hwrpb$b_stack [24576];
/* the following was set aside for pte space.  It has now moved out to      */
/* the next megabyte to allow for mapping the hwrpb and turbo channel       */
/* properly                                                                 */
/* allowed before we get into the next address range in the boot address    */
/* space.  We cannot allow the hwrpb address space to roll into the next    */
/* region.                                                                  */
    int64 hwrpb$q_page_tables [32768];
/* set aside 320kb to emulate region 1,2,3 of MIPS KSEG1 address space      */
/* remove for bl7.x                                                         */
/* rex_mem_regions byte unsigned DIMENSION 320*1024;                        */
    } ;
/*                                                                          */
/*** MODULE $dsrdbdef ***/
/*                                                                          */
/* Structure of Dynamic System Recognition Data Block                       */
/*                                                                          */
struct DSRDB {
    unsigned int SMM [2];               /* SMM value                        */
    unsigned int LURT_OFFSET [2];       /* Offset to LURT                   */
    unsigned int NAME_OFFSET [2];       /* Offset to NAME                   */
    };
/*  The following constants are first used in cp$dispatcher.c.  These will make */
/*  the runtime decision modifications easier.                              */
/*                                                                          */
/*  THIS IS REALLY A MESS - Given that we have *so many platforms, we should get */
/*  rid of "sand_sandplus", "flam_pink_ultra", "sand_plus_ultra", and (possibly) */
/*  "pelican_system" - which probably makes the most sense out of all of them. */
/*  Unfortunately I don't have time to do this now, so I'm going to work within */
/*  the constraints set forth...                                            */
/*                                                                          */
/*  KEEP IT SIMPLE - JUST USE THE SYSTEM NAME AND DON'T TRY TO GROUP THEM... */
/*                                                                          */
/*	sand_sandplus	= Sandpiper, Sandpiper Plus, or Sandpiper 45        */
/*	flam_pink_ultra	= Flamingo, Hot Pink, Flamingo Ultra, or Flamingo 45 */
/*	sand_plus_ultra	= Sandpiper, Sandpiper Plus, Flamingo Ultra, or Flamingo 45 */
/*	pelican_system  = Pelican, Pelica, Pelican Plus or Pelica Plus      */
/*                                                                          */
#define sts$k_sand 1
#define sts$k_sand_plus 2
#define sts$k_sand_45 4
#define sts$k_flam 8
#define sts$k_flam_pink 16
#define sts$k_flam_ultra 32
#define sts$k_flam_ultra_oldsrom 64
#define sts$k_flam_45 128
#define sts$k_sand_sandplus 256
#define sts$k_flam_pink_ultra 512
#define sts$k_sand_plus_ultra 1024
#define sts$k_pelican 4096
#define sts$k_pelican_l 8192
#define sts$k_pelican_x 16384
#define sts$k_pelican_lx 32768
#define sts$k_pelican_45 65536
#define sts$k_pelica_45 131072
#define sts$k_pelican_system 262144
