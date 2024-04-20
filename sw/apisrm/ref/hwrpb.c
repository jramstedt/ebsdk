/*
 * file:	hwrpb.c
 *
 * Copyright (C) 1990, 1995 by
 * Digital Equipment Corporation, Maynard, Massachusetts.
 * All rights reserved.
 *
 * This software is furnished under a license and may be used and copied
 * only  in  accordance  of  the  terms  of  such  license  and with the
 * inclusion of the above copyright notice. This software or  any  other
 * copies thereof may not be provided or otherwise made available to any
 * other person.  No title to and  ownership of the  software is  hereby
 * transferred.
 *
 * The information in this software is  subject to change without notice
 * and  should  not  be  construed  as a commitment by digital equipment
 * corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by digital.
 *
 *
 * Abstract:	This module contains the validation and action routines
 *		for pre-defined environment variables.
 *
 * Author:	David Mayo
 *
 * Modifications:
 *
 *	mar	17-Mar-1999	Added conditionals for mikasa/corelle 
 *				in build_ctb_ws()
 *	tna	9-Feb-1999	Added 2 more Yukona conditionals
 *
 *	tna	4-Jan-1998	Conditionalized for Yukona
 *
 *	sm	 9-Jul-1996	Conditionalise for Takara
 *
 *	jcc	25-Jan-1996	Conditionalize for Cortex
 *
 *	dwn	14-Sep-1995	Added TokenRing cannonical station address format
 *
 *	er	21-Aug-1995	Added conditional for depth probing for
 *				EB platforms.
 *
 *	swt	28-Jun-1995	Add Noritake platform.
 *
 *	rbb	31-May-1995	Correct system family ID for EB164
 *
 *	rbb	10-Mar-1995	Conditionalize for EB164
 *
 *	dtr	7-Mar-1995	Variable added for depth probing for mikasa and
 *				avanti.  (avanti/mikasa added to sable
 *				conditional)
 *				
 *	er	02-Feb-1995	Adding EB66 conditional to build_tbh
 *
 *	er	27-Dec-1994	Conditionalize for EB66
 *
 *	noh	22-Nov-1994	Changed LCA4 software compatibility 
 *				values. (LCA4 != EV4).
 *
 *	bobf	22-Aug-1994	Medulla Only: changed 'osflash' to 
 *				'userflash'
 *
 *	er	15-Aug-1994	Conditionalize for SD164
 *
 *	er	23-Jun-1994	Conditionalize for EB64+
 *
 *	dtr	5-May-1994	correct problem with term_type field always
 *				getting set to graphics in ctb-4.  The code was
 *				never checking to see if keyboard present and
 *				console was serial line.
 *				
 *	jeg	 3-May-1994	fill in kbd_type field in CTB.
 *
 *	dtr(brown) 29-Apr-1994	Merged in all of the lean changes.  Reference
 *				generation is 140.
 *
 *	dpm	6-Jan-1994	remove MUSTANG from the #define that sets the 
 *				HWRPB$_SYSTYPE to 8 as ours is 14 
 *
 *	dpm	2-Dec-1993	using mustang.h instead of cobra.h for
 *				mustang
 *
 *	jmp	11-29-93	Add avanti support
 *
 *      hcb     10-Nov-1993     Add support for calls 32 subclass version 3
 *				serial number decode
 *
 *	pel	28-Sep-1993	Support floppy boot by adding DVA fd_table.
 *
 *	cgh	14-Sep-1993	Add in Medulla support
 *
 *	phs	22-Jun-1993	Allow pmem boot.
 *
 *	dtr	24-May-1993	Add in Morgan support
 *
 *	hcb	31-Mar-1993	Add routine to rebuild memdsc and crb
 *
 *	pel	15-Jan-1993	Conditionalize for morgan
 *
 *	pel	01-Dec-1992	twos_checksum; protect w/ spinlock.
 *				
 *	pel	06-Nov-1992	move build_fru_table into cobra.c
 *
 *	pel	03-Nov-1992	add build_fru_table routine stub.
 *
 *	pel	26-Oct-1992	fill in hwrpb per slot cpu serial# & cpu rev#
 *
 *	pel	12-Oct-1992	show_hwrpb; fix calc of hwrpb per CPU slots
 *
 *	pel	08-Oct-1992	fill in TB hint block, change
 *				hwrpb revision from 2 to 3.
 *
 *	ajb	17-Jul-1992	Move srm'isms from memconfig_cobra
 *
 *	kp	09-Jan-1991	Remove CB_PROTO usage.
 *
 *	ajb	19-Dec-1991	Tally page usage at end of build_crb
 *
 *	ajb	06-Dec-1991	add showhwrpb
 *
 *	dtm	 2-Dec-1991	Fix up CRB entries
 *
 *	ajb	20-Nov-1991	Change amount of space allocated to bootstrap
 *				page tables
 *
 *	sfs	14-Nov-1991	Add Cobra-specific CRB information.
 *
 *	ajb	08-Nov-1991	Add comments regarding per cpu slot
 *				construction.
 *
 *	jds	24-Oct-1991	Added separate CRB entries for each GBUS UART
 *
 *	jds	 3-Oct-1991	Updated HWRPB references to conform to Alpha Console Architecture rev 3.2.
 *				Added initialization of CTB_TABLE_ADDR for VAX callbacks.
 *
 *	jad 	03-Oct-91	Fix for cb_proto build
 *
 *	ajb	16-Sep-91	Fix bugette in modulo arithmetic
 *
 *	kl	 8-Aug-1991	64 bit checksum
 *	
 *	dtm	 6-Aug-1991	Add UART address to CRB entries.
 *
 *	jds	29-May-1991	Updated CTB symbol references.
 *
 *	jds	23-May-1991	Added CRB initialization for VAX callbacks.
 *
 *	kl 	25-Apr-1991	hwrpb_valid, get_hwrpb
 *
 *	kl	23-Apr-1991	Add get_hwrpb_slot
 *
 *	dtm	05-Feb-1991	Modifications for ADU.
 *
 *	dtm	05-Dec-1990	CRB and MEMDSC work.
 *
 *	dtm	06-Nov-1990	CTB work.
 *
 *	dtm	12-Oct-1990	Initial. 
 *
 */

#include	"cp$src:platform.h"
#include	"cp$src:common.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:ev_def.h"
#include	"cp$src:mem_def.h"
#include	"cp$src:hwrpb_def.h"
#include	"cp$src:pal_def.h"
#include	"cp$src:impure_def.h"
#include	"cp$src:platform_cpu.h"
#include	"cp$src:get_console_base.h"
#include	"cp$inc:kernel_entry.h"
#include	"cp$src:memory_chunk.h"

#if RAWHIDE
#include	"cp$inc:platform_io.h"
#endif

#if TURBO 
#include	"cp$src:gbus_def.h"
#include	"cp$src:gbus_misc.h"
#include	"cp$src:xmidef.h"
#include	"cp$src:emulate.h"
#endif

#include	"cp$inc:prototypes.h"

#if EV6
extern int page_table_levels;
#else
#define page_table_levels 3
#define HWRPB$_VA_SIZE 43
#endif

#if APC_PLATFORM
extern int get_sysvar( );
#endif

#if MEDULLA
extern void log_reset_rsn();
extern int Med_system_type();
#endif

#if CORTEX || YUKONA
extern int get_sysvar();
#endif 

#if ALCOR
extern int memory_tested;
#endif                   

#if MODULAR
extern struct window_head config;
#endif

#include	"cp$src:version.h"

#if SABLE || TURBO || RAWHIDE
#include	"cp$src:aprdef.h"
#include	"cp$src:alpha_def.h"
#endif

#if SABLE 
#include	"cp$src:eerom_def.h"
#endif

#define hlt$c_reset 0
#define hlt$c_hw_halt 1
#define hlt$c_sw_halt 5
#define hlt$c_callback 33
#define hlt$c_migrate 100

#define DEBUG 0

#if (TURBO || RAWHIDE)
extern int flash_var_major;
extern int flash_seq_minor;
#endif

extern int CTB_units[];
extern struct CTB_VT_table CTB_VT_tab[];
extern struct CTB_WS_table CTB_WS_tab[];
extern int graphics_console;
extern int keybd_present;
extern struct TTPB serial_ttpb;
extern struct TTPB null_ttpb;
extern struct TTPB *console_ttpb;

#if RAWHIDE
extern struct device *mem_device[MAX_MEM_CNT];
extern UINT memory_test_desc_table;
#endif

#if SABLE || TURBO || RAWHIDE
extern unsigned int ctb_kbd_lang;
#endif

#if MIKASA
extern struct SEMAPHORE pbs_lock;
extern struct pb **pbs;
extern unsigned int num_pbs;
#endif

extern int dispatch[];
extern int fixup[];
extern unsigned __int64 mem_size;
extern unsigned int memory_low_limit;
extern unsigned __int64 memory_high_limit;
extern unsigned int high_heap_size;

#if SABLE || MIKASA || AVANTI || K2 || ALCOR
extern int bus_probe_algorithm;
#endif

int CRB;

#if (MODULAR && SABLE) || (MODULAR && CORELLE)
extern int CTB$MAX_CTBS;
extern int CTB$MAX_SIZE;
#else
int CTB$MAX_CTBS = CTB$MAX_UNITS;
#if 0
int CTB$MAX_SIZE = sizeof(struct CTB_VT);
#else
int CTB$MAX_SIZE = sizeof(struct CTB_WS);
#endif
#endif

#if !MODULAR
int HWRPB_OFFSET;
#else
extern int HWRPB_OFFSET;
#endif
 
#if TURBO
extern void *bitmap_temp_area;
extern int cpu_mask;
#if GALAXY
extern UINT hf_buf_adr;
extern int lp_hard_partition;
#endif
#endif

extern struct SEMAPHORE pbs_lock;
extern struct pb **pbs;
extern unsigned int num_pbs;
extern unsigned int num_pbs_all;

#if GALAXY
extern int galaxy_partitions;
extern int galaxy_node_number;
#endif

#if GALAXY && RAWHIDE
static UINT bitmap_start;
#endif

/*+
 * ============================================================================
 * = build_hwrpb - build the hardware restart parameter block                 =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Build the hwrpb at the address specified.
 *
 * FORM OF CALL:
 *
 *	build_hwrpb (hwrpb)
 *
 * RETURN CODES:
 *
 *	msg_success	- environment variable found
 *
 * ARGUMENTS:
 *
 *	struct HWRPB *hwrpb	- address to build hwrpb
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int build_hwrpb(struct HWRPB *hwrpb) {
    struct MEMDSC *memdsc;
    int i,unit,size,offset;
    struct EVNODE *evp, evn;
    char name[10];

/* Calculate total size of HWRPB and fill in size field: */

	size = sizeof(struct HWRPB) + 			/* Base structure */
		sizeof(struct TB_HINT) +		/* xlatn Buf Hint blk */
		sizeof(struct SLOT) * HWRPB$_NPROC +	/* Per-CPU slots */
		CTB$MAX_SIZE * CTB$MAX_UNITS +		/* CTBs */
		sizeof(struct CRB) + MEMDSC_SIZE;	/* CRB and Memory Descriptors */

	/*
	 * Build the HWRPB.  First clear all locations within the base
	 * structure.
	 */
	memset(hwrpb,0,size);

	/*
	 * Now fill in the fields.
	 */
	*(UINT *)hwrpb->BASE = virt_to_phys(hwrpb);
	hwrpb->IDENT[0] = 0x50525748;
	hwrpb->IDENT[1] = 0x00000042;
	*(UINT *)hwrpb->REVISION = HWRPB$_REVISION;
	*(UINT *)hwrpb->PRIMARY = whoami();
	*(UINT *)hwrpb->PAGESIZE = HWRPB$_PAGESIZE;
	hwrpb->PA_SIZE[0] = HWRPB$_PA_SIZE;
	if (page_table_levels == 4) {
	    hwrpb->PA_SIZE[1] = HWRPB$_VA_SIZE - (HWRPB$_PAGEBITS * 4 - 3 * 3);
	} else {
	    hwrpb->PA_SIZE[1] = 0;
	}

#if TURBO
#undef HWRPB$_ASN_MAX
#define HWRPB$_ASN_MAX asn_max()
#endif

	*(UINT *)hwrpb->ASN_MAX = HWRPB$_ASN_MAX;

#if TURBO | RAWHIDE
	evp = &evn;
	ev_read("sys_serial_num", &evp, 0);
        strncpy(hwrpb->SERIALNUM, evp->value.string, 16);
#endif

/* 
 * Turbo wants to determine the system type at run-time.  This is used
 * right now for the emulation, where we still have to provide a LASER
 * systype for the operating system, but in the ISP model we want to 
 * provide the TURBO system type.  Wouldn't it be slicker for the console
 * to figure this out at run-time anyway (although more ROM code consuming).
 * djm December 7, 1993.
 */
#if TURBO
#undef HWRPB$_SYSTYPE
#define HWRPB$_SYSTYPE systype()
#endif

	*(UINT *)hwrpb->SYSTYPE = HWRPB$_SYSTYPE;
	*(UINT *)hwrpb->SYSVAR = HWRPB$_SYSVAR;

#if APC_PLATFORM
	*(UINT *)hwrpb->SYSVAR = get_sysvar( );
#endif
#if MTU
	*(UINT *)hwrpb->SYSVAR = sys_variation();
#endif
#if CORTEX || YUKONA 
	*(UINT *)hwrpb->SYSVAR = get_sysvar();
#endif
#if MEDULLA
	*(UINT *)hwrpb->SYSVAR = Med_system_type();
#endif
#if SABLE || MIKASA || AVANTI || K2 || ALCOR
	if (bus_probe_algorithm)
	    *(UINT *)hwrpb->SYSVAR += 1<<16;
#endif

	*(UINT *)hwrpb->NPROC = HWRPB$_NPROC;
	*(UINT *)hwrpb->ICLK_FREQ = HWRPB$_ICLK_FREQ;
	*(UINT *)hwrpb->CC_FREQ = krn$_timer_get_cycle_time (whoami());
	hwrpb->VPTBR[0] = 0;
	if (page_table_levels == 4) {
	    hwrpb->VPTBR[1] = 1 << (13 + 10 + 10 + 10 - 32);
	} else {
	    hwrpb->VPTBR[1] = 1 << (13 + 10 + 10 - 32);
	}

/* Calculate total size of HWRPB and fill in size field: */

	*(UINT *)hwrpb->HWRPB_SIZE = size;
	*(UINT *)hwrpb->THB_OFFSET = sizeof(struct HWRPB);

	/*
	 * Build the per-cpu slots.  First find where they start.
	 */
	*(UINT *)hwrpb->SLOT_SIZE = sizeof(struct SLOT);
	*(UINT *)hwrpb->SLOT_OFFSET = *(UINT *)hwrpb->THB_OFFSET + sizeof(struct TB_HINT);

	/*
	 * Find where everything starts.
	 */
	*(UINT *)hwrpb->CTB_OFFSET = *(UINT *)hwrpb->SLOT_OFFSET + *(UINT *)hwrpb->NPROC * sizeof(struct SLOT);
	*(UINT *)hwrpb->CRB_OFFSET = *(UINT *)hwrpb->CTB_OFFSET + CTB$MAX_UNITS * CTB$MAX_SIZE;
	*(UINT *)hwrpb->MEM_OFFSET = *(UINT *)hwrpb->CRB_OFFSET + sizeof(struct CRB);
	offset = *(UINT *)hwrpb->MEM_OFFSET + MEMDSC_SIZE;

	/*
	 * Build the Translation Buffer Granularity Hint Block.
	 */
	build_tbh(hwrpb);

	/*
	 * Build the Console Terminal Block.
	 */
	*(UINT *)hwrpb->NUM_CTBS = CTB$MAX_CTBS;
	*(UINT *)hwrpb->CTB_SIZE = CTB$MAX_SIZE;
	for (unit=0;unit<CTB$MAX_CTBS;unit++)
	    build_ctb(hwrpb,unit);

	/*
	 * Build the Memory Descriptor if there is memory.
	 */
	if (mem_size)
	    build_memdsc(hwrpb);  

	/*
	 * Build the Console Routine Block.
	 */
	CRB = (int)hwrpb + *(UINT *)hwrpb->CRB_OFFSET;
	build_crb(hwrpb);

	/*
	 * Build the config block.
	 */
	size = build_config(hwrpb,offset);
	*(UINT *)hwrpb->HWRPB_SIZE += size;
	offset += size;

	/*
	 * Build the DSRDB block.
	 */
	size = build_dsrdb(hwrpb,offset);
	*(UINT *)hwrpb->HWRPB_SIZE += size;
	offset += size;

	/*
	 * Checksum the appropriate area in the HWRPB
	 */

	HWRPB_OFFSET = offset;
	twos_checksum(hwrpb,offsetof(struct HWRPB,CHKSUM)/4,(UINT *)hwrpb->CHKSUM);


	return msg_success;
}

/*----------*/
void build_tbh (struct HWRPB *hwrpb) {
    struct TB_HINT	*tbh;  /*ptr to translation buf granularity hint block*/

    tbh = (int)hwrpb + *(UINT *)hwrpb->THB_OFFSET;
    tbh->itb[1] = 8;
    tbh->itb[8] = 4;
    tbh->dtb[1] = 32;
    tbh->dtb[8] = 4;
}

/*+
 * ============================================================================
 * = build_slot - build a per-cpu slot in the hwrpb                           =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Build our per-cpu slot in the hwrpb.
 *
 * FORM OF CALL:
 *
 *	build_slot (hwrpb,cpu_no)
 *
 * RETURN CODES:
 *
 *	msg_success	- environment variable found
 *                                     
 * ARGUMENTS:
 *
 *	struct HWRPB *hwrpb	- base address of the HWRPB
 *	int cpu_no - Cpu slot to be initialized.
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int build_slot(struct HWRPB *hwrpb,int cpu_no) {
	int chip;
	int pass;
	int dc_stat;
        int id;
	int i;
	int log_ptr;
        struct EVNODE *evp, evn;
	struct SLOT *slot;
	struct PALVERSION *pal_version;
	struct PALVERSION *osfpal_version;
	int ev5p2, ev5p4, ev5pother, ev56;
	struct
	    {
	    char minor;
	    char major;
	    char variant;
	    char sbz;
	    variant_union
		{
		variant_struct
		    {
		    short compatibility;
		    short max_proc;
		    } vs;
		long sequence;
		} vu;
	    } *slot_version;

	slot = (void *)((int)hwrpb + *(UINT *)hwrpb->SLOT_OFFSET + 
	  (cpu_no * sizeof(struct SLOT)));

	chip = ((int *)(PAL$IMPURE_BASE + IMPURE$CPU_TYPE_OFFSET))[cpu_no*2+1];
	pass = ((int *)(PAL$IMPURE_BASE + IMPURE$CPU_TYPE_OFFSET))[cpu_no*2+0];
	slot->CPU_TYPE[0] = chip;
	if (chip == 2)
	    slot->CPU_TYPE[1] = pass > 2;
	else
	    slot->CPU_TYPE[1] = pass;

	switch (chip) {
	    case 4:	/* LCA4 is not compatible with EV4P3 */
		slot->CPU_COMP[0] = 0;
		slot->CPU_COMP[1] = 0;
		break;
	    case 6:	/* EV45 is compatible with EV4P3 */
		slot->CPU_COMP[0] = 2;
		slot->CPU_COMP[1] = 1;
		break;
	    case 7:	/* EV56 is compatible with EV5P4 */
	    case 9:	/* PCA56 is compatible with EV5P4 */
		slot->CPU_COMP[0] = 5;
		slot->CPU_COMP[1] = 5;
		break;
	    case 11:	/* EV67 is compatible with EV6P2.3 */
	    case 12:	/* EV68CB is compatible with EV6P2.3 */
	    case 13:	/* EV68AL is compatible with EV6P2.3 */
		slot->CPU_COMP[0] = 8;
		slot->CPU_COMP[1] = 4;
		break;
	}

#if TURBO && EV5
	  ev5p2 = 0;
	  ev5p4 = 0;
	  ev5pother = 0;
	  ev56 = 0;

	/*
	 * With the introduction of EV56, have to look at both the 
	 * chip and pass.  If the chip is an EV56(chip=7) then there 
	 * should not be any EV5s so skip mixed passes and just 
	 * do fixups.
	 */

          for ( i = 0; i < MAX_PROCESSOR_ID; i++ ) {
            if ( (1 << i) & cpu_mask ) {
	      if (((int *)(PAL$IMPURE_BASE+IMPURE$CPU_TYPE_OFFSET))[i*2+1] == 7)
	        ev56 = 1;
              else            
                switch(((int *)(PAL$IMPURE_BASE+IMPURE$CPU_TYPE_OFFSET))[i*2+0])
                   {
                        case 1:
                            ev5p2 = 1;
                            break;

                        case 5:
                            ev5p4 = 1;
                            break;

                        default:
                            ev5pother = 1;
                   }
            }
          }
          if (!ev56){
            /* if mixed pass 2 and pass 4 tell VMS pass 3 only */ 
            if ( ev5p2 && ev5p4 && !ev5pother) {
                evp = &evn;
                ev_read( "os_type", &evp, 0 );
                if( (evp->value.string[1] == 'M') || 
                    (evp->value.string[1] == 'p') ) {
                    /* fix hwrpb slots */
                    for( i = 0; i < MAX_PROCESSOR_ID; i++ ) {
                        slot = (void *)((int)hwrpb + *(UINT *)hwrpb->SLOT_OFFSET + 
                                        (i * sizeof(struct SLOT)));
                        if( (slot->CPU_TYPE[1] == 1) ||
                            (slot->CPU_TYPE[1] == 5) )
                            slot->CPU_TYPE[1] = 3;
                    }
                }
            }
          } /* !ev56 */

            /* if all pass 4 or ev56 do fixup */
          if ((ev5p4 && !(ev5p2 || ev5pother)) || ev56) {
                cserve( CSERVE$DO_PASS4_FIXUPS );
          }

	slot = (void *)((int)hwrpb + *(UINT *)hwrpb->SLOT_OFFSET + 
	  (cpu_no * sizeof(struct SLOT)));
#endif

	if (primary ())
	    slot->CPU_VAR[0] = SLOT$_CPU_VAR | 4;
	else
#if RAWHIDE
	    slot->CPU_VAR[0] = SLOT$_CPU_VAR | 4;   /* all are primary eligible */
#else
	    slot->CPU_VAR[0] = SLOT$_CPU_VAR;
#endif                                    
#if TURBO
        /* Get the module's serial # */
        get_tlep_serial_num( cpu_no/2, slot->SERIAL_NUM );

	id = cpu_no;   

	/*
	 * Load PA bit based on the value in CPU_Enabled
	 */
	evp = &evn;
        if (ev_read("cpu_enabled", &evp, 0) == msg_success)
	    if ( evp->value.integer & (1 << id) )
		slot->STATE.SLOT$V_PA = 1;
	    else
		slot->STATE.SLOT$V_PA = 0;
#else
	slot->STATE.SLOT$V_PA = 1;
#endif
	slot->STATE.SLOT$V_PP = 1;

	/*
	 * The KSP and PTBR are filled in by the boot command.
	 */
	slot->STATE.SLOT$V_PV = 1;
	slot->STATE.SLOT$V_PMV = 1;
	slot->STATE.SLOT$V_PL = 1;
	slot->PAL_MEM_LEN[0] = 0;
	slot->PAL_SCR_LEN[0] = 0;
	slot->PAL_MEM_ADR[0] = 0;
	slot->PAL_SCR_ADR[0] = 0;
	*(UINT *)slot->PAL_LOGOUT = pal_virt_to_phys(cpu_no, 
		SLOT$_PAL_LOGOUT + cpu_no * SLOT$_PAL_LOGOUT_LEN);
	slot->PAL_LOGOUT_LEN[0] = SLOT$_PAL_LOGOUT_LEN;
	/*
	 * Set up the console version number.  PAL_REV_AVAIL is a two
	 * dimensional array of longs, so be careful...
	 *
	 * The console number is 0.
	 */
	slot_version = &slot->PAL_REV_AVAIL[0][0];
#if (TURBO || RAWHIDE)
	slot_version->minor = flash_seq_minor & 0xffff;
	slot_version->major = flash_var_major & 0xffff;
	slot_version->variant = 0;
	slot_version->sequence = flash_seq_minor >> 16;
#else
	slot_version->minor = v_minor;
	slot_version->major = v_major;
	slot_version->variant = 0;
	slot_version->sequence = v_sequence;
#endif
 	/*
	 * Set up the VMS PALcode version number.  PAL_REV_AVAIL is a two
	 * dimensional array of longs, so be careful...
	 *
	 * The VMS PALcode number is 1.
	 */

	pal_version = PAL$PAL_BASE + 8;

	slot_version = &slot->PAL_REV_AVAIL[0][2];
	slot_version->minor = pal_version->minor;
	slot_version->major = pal_version->major;
	slot_version->variant = 1;
	slot_version->compatibility = pal_version->xxx;
	slot_version->max_proc = MAX_PROCESSOR_ID;
	/*
	 * Set up the OSF PALcode version number.  PAL_REV_AVAIL is a two
	 * dimensional array of longs, so be careful...
	 *
	 * The OSF PALcode number is 2.
	 */
	osfpal_version = PAL$OSFPAL_BASE + 8;
	slot_version = &slot->PAL_REV_AVAIL[0][4];
	slot_version->minor = osfpal_version->minor;
	slot_version->major = osfpal_version->major;
	slot_version->variant = 2;
	slot_version->compatibility = osfpal_version->xxx;
	slot_version->max_proc = MAX_PROCESSOR_ID;
	/*
	 * Set up the default PALcode version (it's the VMS PALcode version).
	 * Note the funky indexing again.
	 */
	slot->PAL_REV[0] = slot->PAL_REV_AVAIL[0][2];
	slot->PAL_REV[1] = slot->PAL_REV_AVAIL[0][3];

#if TURBO || CLIPPER
	build_halt_log(hwrpb, cpu_no, slot->CONSOLE_DATA_LOG);
#endif

#ifdef SLOT$_BCACHE_INFO
	slot->BCACHE_INFO[1] = SLOT$_BCACHE_INFO;
#ifdef SLOT$_BCACHE_SIZE
	slot->BCACHE_INFO[0] = SLOT$_BCACHE_SIZE;
#else
	get_bcache_info(hwrpb, cpu_no, slot->BCACHE_INFO);
#endif
#endif

	return msg_success;
}


/*+
 * ============================================================================
 * = build_ctb - build the Console Terminal Block                             =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Build the console terminal block at the offset specified in the hwrpb.
 *
 * FORM OF CALL:
 *
 *	build_ctb (hwrpb,unit)
 *
 * RETURN CODES:
 *
 *	msg_success	- environment variable found
 *
 * ARGUMENTS:
 *
 *	struct HWRPB *hwrpb	- base address of the HWRPB
 *	int unit		- unit number of CTB 
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int build_ctb(struct HWRPB *hwrpb, int unit) {
    void *ctb;
    int i;

	ctb = (void *)((int)hwrpb + *(UINT *)hwrpb->CTB_OFFSET + *(UINT *)hwrpb->CTB_SIZE * unit);
	switch (CTB_units[unit]) {
	    case CTB$K_DZ_VT:
		*((struct CTB_VT *)ctb)->CTB_VT$Q_TYPE = CTB_units[unit];
		*((struct CTB_VT *)ctb)->CTB_VT$Q_ID = unit;
		build_ctb_vt(ctb,&CTB_VT_tab[unit]);
		break;

	    case CTB$K_DZ_WS:
		*((struct CTB_WS *)ctb)->CTB_WS$Q_TYPE = CTB$K_DZ_WS;
		*((struct CTB_WS *)ctb)->CTB_WS$Q_ID = unit;
#if SABLE || TURBO || RAWHIDE
		*((struct CTB_WS *)ctb)->CTB_WS$Q_KB_TYPE = ctb_kbd_lang;
#endif
		build_ctb_ws(ctb,&CTB_WS_tab[unit]);
		break;
	}
	return msg_success;
}


/*+
 * ============================================================================
 * = build_ctb_vt - build the DL/DZ VT-specific portion of a CTB              =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Build the device dependent section of a specified console terminal 
 *	block, for a DL/DZ VT device.
 *
 * FORM OF CALL:
 *
 *	build_ctb_vt (ctb)
 *
 * RETURN CODES:
 *
 *	msg_success	- environment variable found
 *
 * ARGUMENTS:
 *
 *	struct CTB_VT *ctb	- base address of the CTB
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int build_ctb_vt(struct CTB_VT *ctb, struct CTB_VT_table *table) {
        struct EVNODE *evp, evn;

	*ctb->CTB_VT$Q_DEV_DEP_LEN = CTB$VT_LEN;
	*ctb->CTB_VT$Q_CSR = table->CSR;
#if RAWHIDE
        ctb->CTB_VT$Q_CSR[1] = CTB$CSR0_H;
#endif
/*
 *  These are already zero...
 *
 *	ctb->CTB_VT$R_TX_INT.TX$V_IE = 0;
 *	ctb->CTB_VT$R_RX_INT.RX$V_IE = 0;
 */
 	ctb->CTB_VT$L_TX_SCB_OFFSET_L = table->TX_SCB;
 	ctb->CTB_VT$L_RX_SCB_OFFSET_L = table->RX_SCB;

#if TURBO
        /* 
         * Load the baud rate for TTA0 based on the value in the BAUD EV 
         */
        if ( *ctb->CTB_VT$Q_ID == 0 ) {
	    evp = &evn;
            if (ev_read("tta0_baud", &evp, 0) == msg_success) {
                ctb->CTB_VT$Q_BAUD[0] = atoi(evp->value.string);
                ctb->CTB_VT$Q_BAUD[1] = 0;
            }
        }
       /* load the baud rate for the PS uart*/
       if ( *ctb->CTB_VT$Q_ID == 1 ) {
          ctb->CTB_VT$Q_BAUD[0] = atoi("9600");
          ctb->CTB_VT$Q_BAUD[1] = 0;
        }

#endif
#if MEDULLA || CORTEX || YUKONA
                ctb->CTB_VT$Q_BAUD[0] = 9600;
                ctb->CTB_VT$Q_BAUD[1] = 0;
#endif
	return msg_success;
}

/*+
 * ============================================================================
 * = build_ctb_ws - build the DL/DZ WS-specific portion of a CTB              =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Build the device dependent section of a specified console terminal 
 *	block, for a DL/DZ WS device.
 *
 * FORM OF CALL:
 *
 *	build_ctb_ws (ctb)
 *
 * RETURN CODES:
 *
 *	msg_success	- environment variable found
 *
 * ARGUMENTS:
 *
 *	struct CTB_WS *ctb	- base address of the CTB
 *
 * SIDE EFFECTS:
 *
 *  	None
 *
-*/
int build_ctb_ws(struct CTB_WS *ctb, struct CTB_WS_table *table) 
{
#if (TURBO || RAWHIDE || MIKASA || CORELLE || K2 || TAKARA)
    if (!graphics_console || !keybd_present)
#else
    if ( ( console_ttpb == &null_ttpb ) || ( console_ttpb == &serial_ttpb ) )
#endif
	table->TYPE = 2;
    else
    	table->TYPE = 3;

    *ctb->CTB_WS$Q_DEV_DEP_LEN = CTB$WS_LEN;
    *ctb->CTB_WS$Q_DEV_IPL = table->DEV_IPL;

    ctb->CTB_WS$L_TX_SCB_OFFSET_L = table->TX_SCB;
    ctb->CTB_WS$L_RX_SCB_OFFSET_L = table->RX_SCB;


    *ctb->CTB_WS$Q_TERM_TYPE = table->TYPE;

    ctb->CTB_WS$Q_TERM_OUT_LOC[0] = table->TERM_OUT_LOC;
    ctb->CTB_WS$Q_TERM_IN_LOC[0]  = table->TERM_IN_LOC;
    return msg_success;
}

/*+
 * ============================================================================
 * = build_crb - build the Console Routine Block                              =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Build the Console Routine Block, the data structure used to define
 *	and map the console callback routines.
 *
 * FORM OF CALL:
 *
 *	build_crb (hwrpb)
 *
 * RETURN CODES:
 *
 *	msg_success	- environment variable found
 *
 * ARGUMENTS:
 *
 *	struct HWRPB *hwrpb	- base address of the HWRPB
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int build_crb(struct HWRPB *hwrpb) {
    struct CRB *crb;
    struct MEMDSC *memdsc;
    int i;

	crb = (struct CRB *)((int)hwrpb + *(UINT *)hwrpb->CRB_OFFSET);
	memdsc = (struct MEMDSC *)((int)hwrpb + *(UINT *)hwrpb->MEM_OFFSET);
	memset(crb,0,sizeof(struct CRB));
#if DEBUG
	pprintf("hwrpb @ %08x, crb @ %08x\n", hwrpb, crb);
#endif

	*(UINT *)crb->CRB$Q_PDISPATCH = virt_to_phys (dispatch);
	*(UINT *)crb->CRB$Q_PFIXUP = virt_to_phys (fixup);
                                                              
	*(UINT *)crb->CRB$Q_ENTRIES = CRB$K_ENTRIES;
        
	*(UINT *)crb->CRB$R_ENTRY[CRB$K_CODE].CRB_ENTRY$Q_V_ADDRESS = 0x10000000;
	*(UINT *)crb->CRB$R_ENTRY[CRB$K_CODE].CRB_ENTRY$Q_P_ADDRESS = ( *(UINT *)memdsc->CLUSTER[0].START_PFN + 1 ) * HWRPB$_PAGESIZE;
	*(UINT *)crb->CRB$R_ENTRY[CRB$K_CODE].CRB_ENTRY$Q_PAGE_COUNT = *(UINT *)memdsc->CLUSTER[0].PFN_COUNT - 1;

#if RAWHIDE
	*(UINT *)crb->CRB$Q_ENTRIES = 1;
#else
	*(UINT *)crb->CRB$R_ENTRY[CRB$K_HEAP].CRB_ENTRY$Q_V_ADDRESS = 0x10000000 + 
		*(UINT *)crb->CRB$R_ENTRY[CRB$K_CODE].CRB_ENTRY$Q_PAGE_COUNT * HWRPB$_PAGESIZE;
	*(UINT *)crb->CRB$R_ENTRY[CRB$K_HEAP].CRB_ENTRY$Q_P_ADDRESS = *(UINT *)memdsc->CLUSTER[2].START_PFN * HWRPB$_PAGESIZE;
	*(UINT *)crb->CRB$R_ENTRY[CRB$K_HEAP].CRB_ENTRY$Q_PAGE_COUNT = *(UINT *)memdsc->CLUSTER[2].PFN_COUNT;
#endif

	/* 
	 * Count the number of pages that were mapped
	 */
	*(UINT *)crb->CRB$Q_PAGES = 0;
	for (i=0;i < *(UINT *)crb->CRB$Q_ENTRIES;i++)
	    *(UINT *)crb->CRB$Q_PAGES += *(UINT *)crb->CRB$R_ENTRY[i].CRB_ENTRY$Q_PAGE_COUNT;

	return msg_success;
}

/*+
 * ============================================================================
 * = rebuild_crb - Rebuild the Console Routine Block			      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Reuild the Console Routine Block, the data structure used to define
 *	and map the console callback routines.  This routine will
 *	change the VAs to console understood VAs.
 *
 * FORM OF CALL:
 *
 *	rebuild_crb (hwrpb)
 *
 * RETURN CODES:
 *
 *	msg_success	- descriptors rebuilt
 *
 * ARGUMENTS:
 *
 *	struct HWRPB *hwrpb	- base address of the HWRPB
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int rebuild_crb(struct HWRPB *hwrpb, int *l3pt) {
    struct CRB *crb;
    struct MEMDSC *memdsc;
    int i;

	crb = (struct CRB *)((int)hwrpb + *(UINT *)hwrpb->CRB_OFFSET);
	memdsc = (struct MEMDSC *)((int)hwrpb + *(UINT *)hwrpb->MEM_OFFSET);
	map_via_page_table (l3pt, dispatch, (UINT *)crb->CRB$Q_VDISPATCH);
	map_via_page_table (l3pt, fixup, (UINT *)crb->CRB$Q_VFIXUP);

	*(UINT *)crb->CRB$R_ENTRY[CRB$K_CODE].CRB_ENTRY$Q_V_ADDRESS = 0x10000000;
	*(UINT *)crb->CRB$R_ENTRY[CRB$K_HEAP].CRB_ENTRY$Q_V_ADDRESS = 0x10000000
		+ *(UINT *)crb->CRB$R_ENTRY[CRB$K_CODE].CRB_ENTRY$Q_PAGE_COUNT * HWRPB$_PAGESIZE;

    return msg_success;
}

/*+
 * ============================================================================
 * = build_memdsc - build the Memory Descriptor table                         =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Build the physical memory descriptor table.  This structure describes
 *	clusters of physcially contiguous memory.  This routine is also
 *	responsible for building a bitmap to represent the good pages of the
 *	memory cluster.
 *
 *	If all of physical memory is contiguous, then at most 3 clusters are 
 *	defined.  The first describes the cluster of memory used by the 
 *	firmware and PAL code, and is not available for use by the system.
 *	The second cluster is all of memory that follows the firmware/PAL
 *	cluster, that which is available for use by system software.  A
 *	third cluster is defined only when the firmware/PAL cluster cannot
 *	begin at physical address 0 due to a memory errors.  In this case,
 *	the bad area of memory is defined in the third cluster.
 *
 * FORM OF CALL:
 *
 *	build_memdsc (hwrpb)
 *
 * RETURN CODES:
 *
 *	msg_success	- environment variable found
 *
 * ARGUMENTS:
 *
 *	struct HWRPB *hwrpb	- base address of the HWRPB
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
#if !RAWHIDE
#if MAX_MEMORY_CHUNK == 1
int build_memdsc(struct HWRPB *hwrpb) {
    struct MEMDSC *memdsc;
    unsigned int c0_size,c1_size,c2_size;
    int i;
    int *ptr;
    int c1_tested_size;
    int base_pfn;

	memdsc = (struct MEMDSC *)((int)hwrpb + *(UINT *)hwrpb->MEM_OFFSET);
#if !TURBO
	if (*(UINT *)memdsc->CLUSTER[1].BITMAP_PA)
	    free(phys_to_virt(*(UINT *)memdsc->CLUSTER[1].BITMAP_PA));
#endif
	memset(memdsc,0,MEMDSC_SIZE);
	*(UINT *)memdsc->CLUSTER_COUNT = 3;
	*(UINT *)memdsc->IMP_DATA_PA = 0;

	base_pfn = get_console_base_pfn();

	/*
	 * Cluster 0 represents the firmware, low heap, HWRPB, etc.
	 */
#if HIGH_BOOT_PTEPAGES
	c0_size = memory_low_limit / HWRPB$_PAGESIZE;
#else
	c0_size = memory_low_limit / HWRPB$_PAGESIZE + BOOT_PTEPAGES;
#endif
	*(UINT *)memdsc->CLUSTER[0].START_PFN = base_pfn;
	*(UINT *)memdsc->CLUSTER[0].PFN_COUNT = c0_size;
	*(UINT *)memdsc->CLUSTER[0].USAGE = 1;

	/*
	 * Cluster 1 represents memory available to the operating
	 * system.
	 */
#if HIGH_BOOT_PTEPAGES
	c1_size = memory_high_limit / HWRPB$_PAGESIZE - c0_size - BOOT_PTEPAGES;
#else
	c1_size = memory_high_limit / HWRPB$_PAGESIZE - c0_size;
#endif
#if !TURBO
    /* 
     * For most platforms, that have < 4GB memory, the heap lies
     * within 4GB making mallocing bitmap space feasible.
     * See AFW_ECO 193.*  djm May 12, 1994
     */
	ptr = malloc_noown((c1_size+63)/8);
#else
    /*
     * For TURBO, cant run the heap over 4GB but still want to put
     * the bitmap at the top of memory.  Make cluster 2 point right
     * at the bitmap describing the system cluster.
     * See AFW_ECO 193.*  djm May 12, 1994
     */
        if (bitmap_temp_area) {
           ptr = bitmap_temp_area;    
        } else {
	   ptr = 0x400000;
	}
#endif

	c1_tested_size = c1_size;
	*(UINT *)memdsc->CLUSTER[1].START_PFN = c0_size + base_pfn;
	*(UINT *)memdsc->CLUSTER[1].PFN_COUNT = c1_size;
	*(UINT *)memdsc->CLUSTER[1].TEST_COUNT = c1_size;
	*(UINT *)memdsc->CLUSTER[1].BITMAP_PA = virt_to_phys(ptr);
	*(UINT *)memdsc->CLUSTER[1].USAGE = 0;
	/*
	 * Cluster 2 represents the high heap.
	 */
	c2_size = mem_size / HWRPB$_PAGESIZE - c0_size - c1_size;
	*(UINT *)memdsc->CLUSTER[2].START_PFN = c0_size + c1_size + base_pfn;
	*(UINT *)memdsc->CLUSTER[2].PFN_COUNT = c2_size;
	*(UINT *)memdsc->CLUSTER[2].USAGE = 1;
	
#if ALCOR
	/* 
	set c1_tested_size to real amount of memory tested 

	memory tested is number of pages tested by platform's powerup path 

	ALCOR will test only 32Mbytes ... the rest is up to the O/S per SRM
	*/
	if (memory_tested == (int)mem_size/0x2000)
	    c1_tested_size = memory_tested - c0_size - c2_size;    
	else
	    c1_tested_size = memory_tested - c0_size;    
	*(UINT *)memdsc->CLUSTER[1].TEST_COUNT = c1_tested_size;
#endif

#if !TURBO
	/*
	 * For now, just mark all available pages as good
	 */                    
	for (i=0;i<c1_tested_size/32;i++) 
		*ptr++ = -1;
	if (c1_tested_size % 32) 
		*ptr++ = (1 << (c1_tested_size % 32)) - 1;
	if (!(c1_tested_size & 32)) 
	    *ptr++ = 0;
#endif

	/*
	 * Checksum all defined bitmaps
	 */
	for (i=0; i < *(UINT *)memdsc->CLUSTER_COUNT; i++) {
	    if (*(UINT *)memdsc->CLUSTER[i].BITMAP_PA == 0) continue;
	    twos_checksum(phys_to_virt(*(UINT *)memdsc->CLUSTER[i].BITMAP_PA),
				      (*(UINT *)memdsc->CLUSTER[i].PFN_COUNT+31)/32,
					(UINT *)memdsc->CLUSTER[i].BITMAP_CHKSUM);
	}


#if TURBO
        /* 
         * Move the bitmap to high memory 
         */                             
        {int qlen;
#if GALAXY
	/* For Galaxy, copy the entire bitmap range in order to include
	   the shared memory region
	 */
	qlen = c2_size * 1024;
#if TURBO
	/* Handle the NT case where the config tree is at the tail
	 * end of the bitmap region.  Don't overwrite it here.
	 */
	if (galaxy_shared_adr) {
	    UINT config_tree_addr = *(UINT *)0x80;
	    if (lp_hard_partition)
		config_tree_addr = phys_to_virt(config_tree_addr);
	    if (config_tree_addr > memory_high_limit) 
		qlen = (config_tree_addr - memory_high_limit)/8;
	}
#endif
#else
        qlen = c1_size/64;
        if (c1_size % 64) {
           qlen=qlen+1;
        }
#endif
        quad_copy(ptr, memory_high_limit, qlen);
	*(UINT *)memdsc->CLUSTER[1].BITMAP_PA = virt_to_phys(memory_high_limit);
        }
#endif


	/*
	 * Checksum the Memory Descriptor area
	 */
	twos_checksum(memdsc,MEMDSC_SIZE/4,(UINT *)memdsc->CHECKSUM);

	return msg_success;
}
#else
int build_memdsc(struct HWRPB *hwrpb) {
    struct MEMDSC *memdsc;
    struct CHUNK_MAP *chunk;
    uint64 c0_size, c_size;
    uint64 c_tested_size;
    uint64 bitmap_pa;
#pragma pointer_size save
#pragma pointer_size 64
    uint64 *p;
#pragma pointer_size restore
    uint64 base_pfn, bitmap_pfn, bitmap_size;
    uint64 high_boot_page_size;
    uint64 high_heap_pages;
    uint64 base_pa;
    int cc, i, j;
    struct CHUNK_MAP chunk_map[MAX_MEMORY_CHUNK];
#if GALAXY && CLIPPER
    uint64 partition_mem=0;
#endif

    collect_memory_chunks( &chunk_map );

    memdsc = (struct MEMDSC *)((int)hwrpb + *(uint64 *)hwrpb->MEM_OFFSET);
    memset( memdsc, 0, MEMDSC_SIZE );

    base_pfn = get_console_base_pfn( );

    /*
     * Cluster 0 represents the console code, low heap, HWRPB, etc.
     */
#if HIGH_BOOT_PTEPAGES
    high_boot_page_size = BOOT_PTEPAGES;
#else
    high_boot_page_size = 0;
#endif
    c0_size = ( memory_low_limit / HWRPB$_PAGESIZE ) + BOOT_PTEPAGES - high_boot_page_size;

    *(uint64 *)memdsc->CLUSTER[0].START_PFN = base_pfn;
    *(uint64 *)memdsc->CLUSTER[0].PFN_COUNT = c0_size;
    *(uint64 *)memdsc->CLUSTER[0].USAGE = 1;
    cc = 1;

    high_heap_pages = (uint64)high_heap_size / HWRPB$_PAGESIZE;

    /*
     * Other clusters represent memory available to the operating
     * system.
     */
    for( i = 0; i < MAX_MEMORY_CHUNK; i++ )
	{
	chunk = &chunk_map[i];
	if( chunk->size == 0 )
	    break;

#if GALAXY && WILDFIRE
	if( galaxy_partitions )
	    {
	    if( ( !( chunk->flags & CHUNK_FLAG_OWNED ) )
		    || ( chunk->owner != galaxy_node_number ) )
		continue;
	    if( chunk->flags & CHUNK_FLAG_SHARED )
		continue;
	    if( chunk->flags & CHUNK_FLAG_BASE )
		if( cc > 1 )
		    pprintf( "\n\ninvalid memory configuration, boot not possible\n\n\n" );
	    }
	if( chunk->flags & CHUNK_FLAG_BITMAP )
	    continue;
	if( chunk->bitmap && ( cc > 1 ) )
	    {
	    base_pfn = chunk->base / HWRPB$_PAGESIZE;
	    c_size = chunk->size / HWRPB$_PAGESIZE;
	    bitmap_pa = chunk->bitmap;
	    *(uint64 *)memdsc->CLUSTER[cc].START_PFN = base_pfn;
	    *(uint64 *)memdsc->CLUSTER[cc].PFN_COUNT = c_size;
	    *(uint64 *)memdsc->CLUSTER[cc].TEST_COUNT = c_size;
	    *(uint64 *)memdsc->CLUSTER[cc].BITMAP_PA = bitmap_pa;
	    *(uint64 *)memdsc->CLUSTER[cc].BITMAP_VA = 0;
	    *(uint64 *)memdsc->CLUSTER[cc].USAGE = 0;
	    cc++;
	    continue;
	    }
#endif

#if GALAXY && CLIPPER
	base_pa = get_console_base_pa();

	if ((chunk->base + chunk->size) <= base_pa)
	    continue;	/* this chunk isn't in our space */

	if (chunk->base < base_pa)
	    {
	    /* this partition starts within this chunk */
	    chunk->size -= base_pa - chunk->base;
	    chunk->base = base_pa;
	    }

	if ((partition_mem + chunk->size) > mem_size)
	    {
	    /* this chunk is too big, trim it down */
	    chunk->size = mem_size - partition_mem;
	    }
	partition_mem += chunk->size;
#endif

	bitmap_size = ( chunk->size / HWRPB$_PAGESIZE + 8 * HWRPB$_PAGESIZE - 1 ) / ( 8 * HWRPB$_PAGESIZE ); 
	c_size = chunk->size / HWRPB$_PAGESIZE - high_boot_page_size - bitmap_size - high_heap_pages;
	base_pfn = chunk->base / HWRPB$_PAGESIZE;
	if( cc == 1 )
	    {
	    c_size -= c0_size;
	    base_pfn += c0_size;
	    }
	bitmap_pfn = base_pfn + c_size;
	bitmap_pa = ( bitmap_pfn + high_boot_page_size ) * HWRPB$_PAGESIZE;
	c_tested_size = c_size;

	*(uint64 *)memdsc->CLUSTER[cc].START_PFN = base_pfn;
	*(uint64 *)memdsc->CLUSTER[cc].PFN_COUNT = c_size;
	*(uint64 *)memdsc->CLUSTER[cc].TEST_COUNT = c_size;
	*(uint64 *)memdsc->CLUSTER[cc].BITMAP_PA = bitmap_pa;
	*(uint64 *)memdsc->CLUSTER[cc].BITMAP_VA = 0;
	*(uint64 *)memdsc->CLUSTER[cc].USAGE = 0;

	cc++;

	/* 
	 * Build the bitmap 
	 */
	*(uint64 *)memdsc->CLUSTER[cc].START_PFN = bitmap_pfn;
	*(uint64 *)memdsc->CLUSTER[cc].PFN_COUNT = bitmap_size + high_boot_page_size + high_heap_pages;
	*(uint64 *)memdsc->CLUSTER[cc].USAGE = 1;
	 
	/*
	 * For now, just mark all available pages as good
	 */
	p = phys_to_virt( bitmap_pa );
	for( j = 0; j < c_tested_size / 64; j++ )
	    *p++ = (uint64)-1;
	if( j = c_tested_size % 64 )
	    *p = ( (uint64)1 << j ) - 1;

	cc++;

	high_boot_page_size = 0;
	high_heap_pages = 0;
#if GALAXY && CLIPPER
	if (partition_mem >= mem_size)
	    break;
#endif
	}

    *(uint64 *)memdsc->CLUSTER_COUNT = cc;

    /*
     * Checksum all defined bitmaps
     */
    for( i = 0; i  <  *(uint64 *)memdsc->CLUSTER_COUNT; i++ )
	{
	if( *(uint64 *)memdsc->CLUSTER[i].BITMAP_PA == 0 )
	    continue;
	twos_checksum( phys_to_virt( *(uint64 *)memdsc->CLUSTER[i].BITMAP_PA ),
				  ( *(uint64 *)memdsc->CLUSTER[i].PFN_COUNT + 31 ) / 32,
				    (uint64 *)memdsc->CLUSTER[i].BITMAP_CHKSUM );
	}

    /*
     * Checksum the Memory Descriptor area
     */
    twos_checksum( memdsc, MEMDSC_SIZE / 4, (uint64 *)memdsc->CHECKSUM );

    return( msg_success );
    }
#endif
#endif
#if RAWHIDE
#define THIRTY_TWO_MB 0x2000000
#define SIXTY_FOUR_MB 0x4000000
#define TWO_MB 0x200000
int build_memdsc(struct HWRPB *hwrpb) {
    struct MEMDSC *memdsc;
    UINT memtest_dsc;
    UINT c0_size = 0, c_size = 0;
    UINT addr;
    int i,j;
    struct device *dev;
    struct daughter_memory *dp;
    int first_memory = 0;
    UINT mem_base = 0;
    UINT cur_base = 0;
    int mem_index = 0, cc = 0;
    UINT align_size = 0;
    UINT first_size = 0;
    int bitmap_cluster_index = 0;
    UINT page_align;
    unsigned int *ptr;

#if GALAXY
    if (galaxy_partitions > 1) {
	build_memdsc_galaxy(hwrpb);	/* handle in a special routine */
	return;
    }
#endif

    memtest_dsc = memory_test_desc_table;

    dev = mem_device[0];
    dp = dev->devdep.mem.dp;
    while ( mem_index < MEM_MAX_ID && !first_size ) {
	if ( *(UINT *) dp->dm[mem_index].size ) {
	    first_memory = mem_index;
	    first_size = *(UINT *) dp->dm[mem_index].size;
	    c_size = align_size = first_size;
	    if ( first_size == THIRTY_TWO_MB )
		align_size = SIXTY_FOUR_MB;
	    if ( tincup() && dimm_present() ) 
		align_size = DIMM_MEM_ALIGN * MEGABYTE;
	    mem_base = *(UINT *) dp->dm[mem_index].base;
	    cur_base = mem_base + first_size;
	}
	mem_index++;
    }

    memdsc = (struct MEMDSC *)((int)hwrpb + *(UINT *)hwrpb->MEM_OFFSET);
    memset(memdsc,0,MEMDSC_SIZE);
    *(UINT *)memdsc->IMP_DATA_PA = 0;

    /*
     * Cluster 0 represents the firmware, low heap, HWRPB, etc.
     */

    c0_size = TWO_MB / HWRPB$_PAGESIZE;
    c_size = *(UINT *) dp->dm[first_memory].size - ( c0_size * HWRPB$_PAGESIZE );

    *(UINT *)memdsc->CLUSTER[0].START_PFN = 0;
    *(UINT *)memdsc->CLUSTER[0].PFN_COUNT = c0_size;
    *(UINT *)memdsc->CLUSTER[0].BITMAP_PA = 0;
    *(UINT *)memdsc->CLUSTER[0].USAGE = 1;
    *(UINT *)memdsc->CLUSTER_COUNT = 1;
    cc = 1;

    /*
     * Other clusters represent memory available to the operating
     * system.
     */
    
    if ( first_size ) {
	while ( mem_index <= MEM_MAX_ID ) {
	    while ((!( cur_base & ( align_size - 1 ))) && ( mem_index < MEM_MAX_ID )) {
		cur_base = cur_base + *(UINT *) dp->dm[mem_index].size;
		c_size = c_size + *(UINT *) dp->dm[mem_index].size;
		mem_index++;
	    }
	    if ( c_size ) {
		if ( cc == 1 ) {
		    mem_base = c0_size * HWRPB$_PAGESIZE;
		    
		    /*
		     * memory_high_limit = base of bitmap = bitmap PA of 
		     * first system descriptor
		     */

		    ptr = ( unsigned int * ) &page_align; 
		    ptr[0] = ( ~( HWRPB$_PAGESIZE - 1 ));
		    ptr[1] = 0xFFFFFFFF;
		    addr = memtest_dsc + (offsetof(struct MEMDSC,CLUSTER[1].BITMAP_PA));
		    memory_high_limit = ldq(addr) & page_align;

		}

		*(UINT *)memdsc->CLUSTER[cc].START_PFN = mem_base / HWRPB$_PAGESIZE;
		*(UINT *)memdsc->CLUSTER[cc].PFN_COUNT = c_size / HWRPB$_PAGESIZE;
		*(UINT *)memdsc->CLUSTER[cc].TEST_COUNT = c_size / HWRPB$_PAGESIZE;
		*(UINT *)memdsc->CLUSTER[cc].USAGE = 0;

		/*
		 * Assign the bitmap addresses for this cluster. Bitmaps
		 * are built on powerup by the memory test.
		 */

		if ( memtest_dsc )
		    assign_cluster_bitmap( memdsc, cc, memtest_dsc );
		else
		    build_fake_bitmap( memdsc, cc );

		*(UINT *)memdsc->CLUSTER_COUNT = *(UINT *)memdsc->CLUSTER_COUNT + 1;
	    }
	    cc++;
	    mem_base = *(UINT *) dp->dm[mem_index].base;
	    c_size = *(UINT *) dp->dm[mem_index].size;
	    cur_base = mem_base + c_size;
	    mem_index++;
	}
    }

    /*
     * Make one more descriptor for the bitmaps that are generated by the memory test.
     */

    build_bitmap_cluster( memdsc, memtest_dsc );

    /*
     * Checksum all defined bitmaps
     */

    for (i=0; i < *(UINT *)memdsc->CLUSTER_COUNT; i++) {
	if (*(UINT *)memdsc->CLUSTER [i].BITMAP_PA == 0) continue;
	twos_checksum(*(UINT *)memdsc->CLUSTER[i].BITMAP_PA,
		     (*(UINT *)memdsc->CLUSTER[i].PFN_COUNT+31)/32,
		       (UINT *)memdsc->CLUSTER[i].BITMAP_CHKSUM);
    }

    /*
     * Checksum the Memory Descriptor area
     */

    twos_checksum(memdsc,MEMDSC_SIZE/4,(UINT *)memdsc->CHECKSUM);
    return msg_success;
}

#if GALAXY
/*+
 * ============================================================================
 * = build_memdsc_galaxy - build the Memory Descriptor (FOR RAWHIDE GALAXY)   =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Build the physical memory descriptor table.  This structure describes
 *	clusters of physcially contiguous memory.  This routine determines
 *	the subset of memory used by this galaxy instance and creates
 *	descriptors which point to those subsets of the bitmap.
 *
 *	If all of physical memory is contiguous, then 3 clusters are 
 *	defined.  The first describes the cluster of memory used by the 
 *	firmware and PAL code, and is not available for use by the system.
 *	The second cluster is all of memory that follows the firmware/PAL
 *	cluster, that which is available for use by system software.  A
 *	third cluster is defined which contains the pages used by the bitmaps.
 *
 * FORM OF CALL:
 *
 *	build_memdsc_galaxy (hwrpb)
 *
 * RETURN CODES:
 *
 *	none
 *
 * ARGUMENTS:
 *
 *	struct HWRPB *hwrpb	- base address of the HWRPB
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int build_memdsc_galaxy(struct HWRPB *hwrpb) {
    struct EVNODE evn, *evp=&evn;
    char evname[80];
    int i;
    struct MEMDSC *memdsc;
    UINT memtest_dsc;
    int base_pfn, pfn_count, end_pfn;
    int ccount;
    int srom_clusters;
    unsigned int *ptr;

    /* I. Build a descriptor for the Impure, PAL, & Console pages	    */

    memtest_dsc = (UINT) phys_to_virt(memory_test_desc_table);
    memdsc = (struct MEMDSC *) ((int) hwrpb + *(UINT *)hwrpb->MEM_OFFSET);
    memset(memdsc, 0, MEMDSC_SIZE);
    *(UINT *)memdsc->IMP_DATA_PA = 0;

    /* the high limit is capped by either the start of the bitmap */
    /* or the limit on instance 0 determined in galaxy.c */

    srom_clusters = ldq(memtest_dsc + offsetof(struct MEMDSC, CLUSTER_COUNT));
					    /* find last cluster */
    memory_high_limit = 
		phys_to_virt(
		    ldq(memtest_dsc +
		offsetof(struct MEMDSC, CLUSTER[srom_clusters - 1].START_PFN ))
			 * HWRPB$_PAGESIZE);
    bitmap_start = memory_high_limit;
    if (mem_size < memory_high_limit)
	memory_high_limit = mem_size;

    base_pfn = get_console_base_pfn();
    pfn_count = (virt_to_phys(memory_high_limit) / HWRPB$_PAGESIZE) - base_pfn;
    end_pfn = base_pfn + pfn_count - 1;

    *(UINT *)memdsc->CLUSTER[0].START_PFN = base_pfn;
    *(UINT *)memdsc->CLUSTER[0].PFN_COUNT = TWO_MB / HWRPB$_PAGESIZE;
    *(UINT *)memdsc->CLUSTER[0].TEST_COUNT = TWO_MB / HWRPB$_PAGESIZE;
    *(UINT *)memdsc->CLUSTER[0].BITMAP_PA = 0;
    *(UINT *)memdsc->CLUSTER[0].USAGE = 1;
    *(UINT *)memdsc->CLUSTER_COUNT = ccount = 1;

    base_pfn += TWO_MB / HWRPB$_PAGESIZE;
    pfn_count -= TWO_MB / HWRPB$_PAGESIZE;  /* account for cluster 0 */

    /* II. Loop through the memory descriptors built by the SROM */
    /*     and create descriptors for those which are within     */
    /*     this partition.					  */

    for (i = 1; i < srom_clusters - 1; i++) {
	int memtest_start_pfn = ldq(memtest_dsc +
				offsetof(struct MEMDSC, CLUSTER[i].START_PFN));
	int memtest_pfn_count = ldq( memtest_dsc +
				offsetof(struct MEMDSC, CLUSTER[i].PFN_COUNT));
	int memtest_end_pfn = memtest_start_pfn + memtest_pfn_count - 1;

	if (base_pfn > memtest_start_pfn) {

	    /*	    =====	OR        =====	    partition's memory	*/
	    /*	  ++++			+++++++++   SROM descriptor	*/

	    if (base_pfn <= memtest_end_pfn) {
		/* overlap between the partition and this descriptor */
		*(UINT *)memdsc->CLUSTER[ccount].START_PFN = base_pfn;
		if (end_pfn > memtest_end_pfn) {
		    *(UINT *)memdsc->CLUSTER[ccount].PFN_COUNT = 
			*(UINT *)memdsc->CLUSTER[ccount].TEST_COUNT = 
			memtest_end_pfn - base_pfn + 1;
		} else {
		    *(UINT *)memdsc->CLUSTER[ccount].PFN_COUNT = 
			*(UINT *)memdsc->CLUSTER[ccount].TEST_COUNT = 
			pfn_count;  /* total overlap */
		}
		*(UINT *)memdsc->CLUSTER[ccount].BITMAP_PA = 
		    ldq(memtest_dsc +
			offsetof(struct MEMDSC, CLUSTER[ccount].BITMAP_PA)) +
		    (base_pfn - memtest_start_pfn) / 8;	/* 8 pages per byte */
		
	    }
	} else {

	    /*	    =====	OR        =====	    partition's memory	*/
	    /*	      ++++		   +++	    SROM descriptor	*/

	    if (memtest_start_pfn <= end_pfn) {
		/* overlap between the partition and this descriptor */
		*(UINT *)memdsc->CLUSTER[ccount].START_PFN = memtest_start_pfn;
		if (end_pfn > memtest_end_pfn) {
		    *(UINT *)memdsc->CLUSTER[ccount].PFN_COUNT = 
			*(UINT *)memdsc->CLUSTER[ccount].TEST_COUNT = 
			memtest_pfn_count;  /* total overlap */
		} else {
		    *(UINT *)memdsc->CLUSTER[ccount].PFN_COUNT = 
			*(UINT *)memdsc->CLUSTER[ccount].TEST_COUNT = 
			end_pfn - memtest_start_pfn + 1;
		}
		*(UINT *)memdsc->CLUSTER[ccount].BITMAP_PA = 
		    ldq(memtest_dsc +
			offsetof(struct MEMDSC, CLUSTER[ccount].BITMAP_PA));
		
	    }
	}
	*(UINT *)memdsc->CLUSTER[ccount].USAGE = 0;

	/* init the virtual address of this bitmap */

	*(UINT *)memdsc->CLUSTER_COUNT = ++ccount;
    }	/* end of for */

    /* III. Build a descriptor which matches the SROM bitmap descriptor */

    *(UINT *)memdsc->CLUSTER[ccount].START_PFN = 
	ldq(memtest_dsc + 
	    offsetof(struct MEMDSC, CLUSTER[srom_clusters - 1].START_PFN));
    *(UINT *)memdsc->CLUSTER[ccount].PFN_COUNT = 
	ldq(memtest_dsc +
	    offsetof(struct MEMDSC, CLUSTER[srom_clusters - 1].PFN_COUNT));
    *(UINT *)memdsc->CLUSTER[ccount].TEST_COUNT = 
	ldq(memtest_dsc +
	    offsetof(struct MEMDSC, CLUSTER[srom_clusters - 1].TEST_COUNT));
    *(UINT *)memdsc->CLUSTER[ccount].BITMAP_PA = 0;
    *(UINT *)memdsc->CLUSTER[ccount].USAGE = 1;
    *(UINT *)memdsc->CLUSTER_COUNT = ++ccount;

    /* IV. Checksum the descriptors */

    for (i = 0; i < ccount; i++) {
	if (*(UINT *)memdsc->CLUSTER[i].BITMAP_PA == 0)
	    continue;
	twos_checksum(phys_to_virt(*(UINT *)memdsc->CLUSTER[i].BITMAP_PA),
			(*(UINT *)memdsc->CLUSTER[i].PFN_COUNT+31)/32,
			(UINT *)memdsc->CLUSTER[i].BITMAP_CHKSUM);
    }
}
#endif

/*+
 * ============================================================================
 * = assign_cluster_bitmap - assign cluster bitmap 			      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Assign a cluster a bitmap. Bitmaps are built earlier by memory test.
 *
 * FORM OF CALL:
 *
 *	assign_cluster_bitmap ( memdsc, cc, memtest_dsc )
 *
 * RETURN CODES:
 *
 *
 * ARGUMENTS:
 *
 *	struct MEMDSC memdsc		- base address of the MEMDSC
 *	int cc				- cluster count
 *	UINT memtest_dsc		- base address of the memtest descriptor table
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
void assign_cluster_bitmap( struct MEMDSC *memdsc, int cc, UINT memtest_dsc ) {

    int i;
    int found = FALSE;
    UINT mem_page_base = *(UINT *)memdsc->CLUSTER[cc].START_PFN;
    UINT mem_page_count = *(UINT *)memdsc->CLUSTER[cc].PFN_COUNT;
    UINT count, start_pfn,pfn_count,usage;

    /*
     * Find the memory test descriptor that is associated with this chunk of memory.
     */

    count = ldq(memtest_dsc + offsetof(struct MEMDSC, CLUSTER_COUNT));
    for ( i = 0; i <  count; i++ ) {
	start_pfn = ldq(memtest_dsc + offsetof(struct MEMDSC, CLUSTER[i].START_PFN));
	pfn_count = ldq(memtest_dsc + offsetof(struct MEMDSC, CLUSTER[i].PFN_COUNT));
	usage = ldq(memtest_dsc + offsetof(struct MEMDSC, CLUSTER[i].USAGE));
	if (( mem_page_base <= start_pfn ) && (( mem_page_base + mem_page_count ) >= (start_pfn + pfn_count)) && ( usage == 0 )) {
		found = TRUE;
		break;
	}
    }

    if ( found ) {

	/*
	 * Load the bitmap PA and tested page count directly from the memtest bitmap descriptor.
	 */

	*(UINT *)memdsc->CLUSTER[cc].TEST_COUNT = ldq(memtest_dsc + offsetof(struct MEMDSC, CLUSTER[i].TEST_COUNT));
	*(UINT *)memdsc->CLUSTER[cc].BITMAP_PA = ldq(memtest_dsc + offsetof(struct MEMDSC, CLUSTER[i].BITMAP_PA));
    }
}


/*+
 * ============================================================================
 * = build_bitmap_cluster - build bitmap cluster			      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Build a descriptor that describes all memory associated with the bitmap.
 *
 * FORM OF CALL:
 *
 *	build_bitmap_cluster ( memdsc, memtest_dsc, bitmap_cluster_index )
 *
 * RETURN CODES:
 *
 *
 * ARGUMENTS:
 *
 *	struct MEMDSC memdsc			- base address of the MEMDSC
 *	UINT memtest_dsc			- base address of the memtest_dsc
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
void build_bitmap_cluster ( struct MEMDSC *memdsc, UINT memtest_dsc ) {

    int cc = *(UINT *)memdsc->CLUSTER_COUNT;
    int i;
    int found = FALSE;
    UINT count, usage;

    /*
     * Find the memtest cluster that has a usage of 1 - this is the bitmap cluster.
     */

    count = ldq(memtest_dsc + offsetof(struct MEMDSC, CLUSTER_COUNT));
    for ( i = 1; i < count; i++ ) {
	usage = ldq(memtest_dsc + offsetof(struct MEMDSC, CLUSTER[i].USAGE));
	if ( usage == 1 ) {
	    found = TRUE;
	    break;
	}
    }

    if ( found ) {
	*(UINT *)memdsc->CLUSTER[cc].START_PFN = ldq(memtest_dsc + offsetof(struct MEMDSC, CLUSTER[i].START_PFN));
	*(UINT *)memdsc->CLUSTER[cc].PFN_COUNT = ldq(memtest_dsc + offsetof(struct MEMDSC, CLUSTER[i].PFN_COUNT));
	*(UINT *)memdsc->CLUSTER[cc].BITMAP_PA = 0;
	*(UINT *)memdsc->CLUSTER[cc].USAGE = 1;
	*(UINT *)memdsc->CLUSTER_COUNT = *(UINT *)memdsc->CLUSTER_COUNT + 1;

	/*
	 * Search the clusters for the system cluster from which the 
	 * bitmap pages came. Subtract the pages allocated for the bitmap 
	 * from that cluster.
	 */

	for ( i = 1; i < memdsc->CLUSTER_COUNT; i++ ) {
	    if ( *(UINT *)memdsc->CLUSTER[i].USAGE == 0 ) {
		if (( *(UINT *)memdsc->CLUSTER[cc].START_PFN >= *(UINT *)memdsc->CLUSTER[i].START_PFN ) &&
		    (( *(UINT *)memdsc->CLUSTER[cc].START_PFN + *(UINT *)memdsc->CLUSTER[cc].PFN_COUNT ) <= 
		     ( *(UINT *)memdsc->CLUSTER[i].START_PFN + *(UINT *)memdsc->CLUSTER[i].PFN_COUNT ))) {
		    break;
		    
		}
	    }
	}
	if ( *(UINT *)memdsc->CLUSTER[i].PFN_COUNT == *(UINT *)memdsc->CLUSTER[i].TEST_COUNT )
	    *(UINT *)memdsc->CLUSTER[i].TEST_COUNT = *(UINT *)memdsc->CLUSTER[i].TEST_COUNT - *(UINT *)memdsc->CLUSTER[cc].PFN_COUNT;	
	*(UINT *)memdsc->CLUSTER[i].PFN_COUNT = *(UINT *)memdsc->CLUSTER[i].PFN_COUNT - *(UINT *)memdsc->CLUSTER[cc].PFN_COUNT;	
    }

}


/*+
 * ============================================================================
 * = build_fake_bitmap - build fake bitmap				      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Build fake bitmap.
 *
 * FORM OF CALL:
 *
 *	build_fake_bitmap ( memdsc, cc )
 *
 * RETURN CODES:
 *
 *
 * ARGUMENTS:
 *
 *	struct MEMDSC memdsc	- base address of the MEMDSC
 *	int cc			- cluster count
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
void build_fake_bitmap( struct MEMDSC *memdsc, int cc ) {
    int *ptr;
    int tested_size;
    int i;

    /*
     * This code builds an all good bitmap for the cluster.
     * It is built in memory that is malloc'ed by the console.
     */
	
    ptr = malloc_noown((*(UINT *)memdsc->CLUSTER[cc].PFN_COUNT+63)/8);

    *(UINT *)memdsc->CLUSTER[cc].BITMAP_PA = (int) ptr;

    tested_size = *(UINT *)memdsc->CLUSTER[cc].PFN_COUNT;
    for ( i = 0; i < tested_size / 32; i++) 
	*ptr++ = -1;
    if ( tested_size % 32 ) 
	*ptr++ = ( 1 << ( tested_size % 32 )) - 1;
    if (!( tested_size & 32 )) 
	*ptr++ = 0;
}
#endif

/*+
 * ============================================================================
 * = rebuild_memdsc - rebuild the Memory Descriptor table		      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Rebuild the physical memory descriptor table.  This routine will
 *	change the VAs to console understood VAs and will recalculate the
 *	checksums.
 *
 * FORM OF CALL:
 *
 *	rebuild_memdsc (hwrpb)
 *
 * RETURN CODES:
 *
 *	msg_success	- descriptors rebuilt
 *
 * ARGUMENTS:
 *
 *	struct HWRPB *hwrpb	- base address of the HWRPB
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int rebuild_memdsc(struct HWRPB *hwrpb, int *l3pt) {
    struct MEMDSC *memdsc;
    int i;

    memdsc = (struct MEMDSC *)((int)hwrpb + *(UINT *)hwrpb->MEM_OFFSET);

    /*
     * Setup the VA and Checksum all defined bitmaps
     */ 
    for (i=0; i < *(UINT *)memdsc->CLUSTER_COUNT; i++) {
	if (*(UINT *)memdsc->CLUSTER[i].BITMAP_PA == 0) continue;
	map_via_page_table (l3pt, phys_to_virt(*(UINT *)memdsc->CLUSTER[i].BITMAP_PA),
				   (UINT *)memdsc->CLUSTER[i].BITMAP_VA);

	twos_checksum(phys_to_virt(*(UINT *)memdsc->CLUSTER[i].BITMAP_PA),
				  (*(UINT *)memdsc->CLUSTER[i].PFN_COUNT+31)/32,
				    (UINT *)memdsc->CLUSTER[i].BITMAP_CHKSUM);
    }

    memdsc->CHECKSUM[0] = 0;
    memdsc->CHECKSUM[1] = 0;
    /*
     * Checksum the Memory Descriptor area
     */
    twos_checksum(memdsc,MEMDSC_SIZE/4,(UINT *)memdsc->CHECKSUM);

    return msg_success;
}

/*+
 * ============================================================================
 * = hwrpb_valid - validate HWRPB					      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * 	Validate HWRPB
 *
 * FORM OF CALL:
 *  
 *	hwrpb_valid(hwrpb)
 *  
 * RETURNS:
 *
 *      TRUE or FALSE
 *       
 * ARGUMENTS:
 *
 *	struct *HWRPB
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int hwrpb_valid(struct HWRPB *hwrpb) {
	unsigned int chksum[2] = {0,0};

	if (hwrpb == 0) return 0;
	twos_checksum(hwrpb,offsetof(struct HWRPB,CHKSUM)/4,(UINT *)chksum);

	if ((*(UINT *)hwrpb->BASE == virt_to_phys(hwrpb)) &&
	   (hwrpb->IDENT[0] == 0x50525748) &&
	   (hwrpb->IDENT[1] == 0x00000042) &&
	   (*(UINT *)chksum == *(UINT *)hwrpb->CHKSUM))
	 return TRUE;
	else
	{
	   pprintf("warning -- HWRPB is invalid\n");
	   return FALSE;
	}	   
}

#if 0
/*
 *++
 * =========================================================================
 * = find_topology - walk a window structure looking for a topology        =
 * =========================================================================
 *
 *  FUNCTIONAL DESCRIPTION:
 *   
 *      find_topology
 *
 *  FORMAL PARAMETERS:
 *   
 *      int hose
 *	int slot
 *	int channel 
 *	int node
 *
 *
 *  RETURN VALUE:
 *      
 *  struct device* - a pointer to the device data structure if the
 *		     device was found.  If not found NULL is returned.
 *   
 *  IMPLICIT INPUTS:
 *
 *      NONE
 *               
 *  IMPLICIT OUTPUTS:
 *
 *      NONE
 *
 *  SIDE EFFECTS:
 *
 *
 *--
 */
struct device* find_topology(struct window_head *w,
                             int hose, int slot, int channel, int node){
   int *i,j,k;
   struct device *dev; 
   struct device* hit = 0;
                 
  if (w==0) {
    return 0;
  }

    i = w->descr;
    j = 0;
    while( (hit==0) && (j<w->count) ) {
      if(dev = i[j]) {                          
          if ( (dev->hose == hose)        && 
	       (dev->slot == slot)        && 
     	       (dev->channel == channel)  && 
	       (dev->node == node)        ){
	   hit = dev;
         } else if (dev->tbl->class  == DEVTYPE$K_IO_WINDOW) {
           hit = find_topology(dev->devdep.io_window.window,
                                hose,slot,channel,node); 
         }
      } 
      j = j + 1;
    } 
    
    return hit;

} /* end find_topology */
#endif

void hwrpb_load_halt(struct HWRPB *hwrpb, struct impure *impure, int id)
{
    struct SLOT *slot;

    slot = (int)hwrpb + *(UINT *)hwrpb->SLOT_OFFSET + *(UINT *)hwrpb->SLOT_SIZE * id;
    *(UINT *)slot->HALT_PCBB = *(UINT *)(impure->cns$pt[0] + 2*PT$_PCBB);
    *(UINT *)slot->HALT_PC = *(UINT *)impure->cns$exc_addr;
    *(UINT *)slot->HALT_PS = *(UINT *)(impure->cns$pt[0] + 2*PT$_PS) & 0xffff;
    *(UINT *)slot->HALT_ARGLIST = *(UINT *)(impure->cns$gpr[0] + 2*25);
    *(UINT *)slot->HALT_RETURN = *(UINT *)(impure->cns$gpr[0] + 2*26);
    *(UINT *)slot->HALT_VALUE = *(UINT *)(impure->cns$gpr[0] + 2*27);
    *(UINT *)slot->HALTCODE = *(UINT *)impure->cns$hlt;
#if MEDULLA || CORTEX || YUKONA
    log_reset_rsn();  /* Fills in RSVD_SW field of Per CPU Slot */
#endif
    if( *(UINT *)impure->cns$hlt == hlt$c_hw_halt )
	slot->STATE.SLOT$V_OH = 1;	    

#if GALAXY
    /* Galaxy interim workaround					*/
    /* PALcode converts a go-to cserve into a hlt$c_migrate entry.	*/
    /* We convert this entry type into a halt instruction executed with */
    /* a migrate request and partition number.				*/
    /* The code in entry thinks we did a halt -migrate			*/
    if (impure->cns$hlt[0] == hlt$c_migrate) {
	*(UINT *)slot->HALTCODE = hlt$c_sw_halt;
	slot->STATE.SLOT$V_HALT_REQUESTED = 0x7;
	slot->STATE.SLOT$L_RSVD_MBZ2 = 
				*(UINT *)(impure->cns$gpr[0] + 2*17) & 0xffff;
    }
#endif

    /*
     *  Flush output which is caught in the HWRPB.
     */
    hwrpb_comm_flush( );
}

#undef DEBUG
