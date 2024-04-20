/*
 * file:	show_hwrpb.c
 *
 * Copyright (C) 1990 by
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
 * Abstract:	This module contains the display routines for the hwrpb.
 *
 * Author:	J.Kirchoff
 *
 * Modifications:
 *
 *	ska	11-Nov-1998	Conditionalize for Yukona.
 *
 */

#include	"cp$src:platform.h"
#include	"cp$src:common.h"
#include	"cp$src:hwrpb_def.h"

#include "cp$inc:kernel_entry.h"
#include "cp$inc:prototypes.h"

extern int cpu_mask;
extern int hwrpb;

#define ofh(x) (offsetof(struct HWRPB, x))
#define ph(x)	hwrpbp->x[1], hwrpbp->x[0]

#define DEBUG 0

/*+
 * ============================================================================
 * = show hwrpb - Display the address of the HWRPB.                           =
 * ============================================================================
 *
 * OVERVIEW:
 *          
 *      Display the address of the Alpha HWRPB.
 *
 *   COMMAND FORM:
 *  
 *   	show hwrpb ()
 *  
 *   COMMAND ARGUMENT(S):
 *
 *	None
 *
 *   COMMAND OPTION(S):
 *
 *	None
 *
 *   COMMAND EXAMPLE(S):
 *~
 *	>>>show hwrpb
 *	HWRPB is at 2000
 *	>>>
 *~
 * FORM OF CALL:                             
 *  
 *	showhwrpb ()
 *  
 * RETURN CODES:
 *
 *	None.
 *       
 * ARGUMENTS:
 *
 *	none
 *
 * SIDE EFFECTS:
 *
 *      None                                 
 *
-*/

void showhwrpb()
{
    struct HWRPB *hwrpbp;

    hwrpbp = hwrpb;

    printf ("HWRPB is at %x\n", hwrpbp);

    if (hwrpb == 0)
	return;

#if MODULAR
    if (boot_shared_adr != 0)
	hwrpb_valid(hwrpb);
#else
    hwrpb_valid(hwrpb);
#endif

#if DEBUG || TURBO || CORTEX || YUKONA || RAWHIDE || EXTRA || CLIPPER
    show_hwrpb_sub(hwrpbp);
#endif
}

#if DEBUG || TURBO || CORTEX || YUKONA || RAWHIDE || EXTRA || CLIPPER

void show_hwrpb_sub(struct HWRPB *hwrpbp)
{
    int	i;
    struct SLOT *slot;

    printf ("%08X hwrpb\n", hwrpbp);
    printf ("	%3d  %08X %08X Physical address of base of HWRPB\n",	ofh(BASE), ph(BASE));
    printf ("	%3d  %08X %08X Identifying string 'HWRPB'\n",		ofh(IDENT), ph(IDENT));
    printf ("	%3d  %08X %08X HWRPB version number\n",			ofh(REVISION), ph(REVISION));
    printf ("	%3d  %08X %08X HWPRB size\n",				ofh(HWRPB_SIZE), ph(HWRPB_SIZE));
    printf ("	%3d  %08X %08X ID of primary processor\n",		ofh(PRIMARY), ph(PRIMARY));
    printf ("	%3d  %08X %08X System page size in bytes\n",		ofh(PAGESIZE), ph(PAGESIZE));
    printf ("	%3d  %08X %08X Physical address size in bits\n",	ofh(PA_SIZE), ph(PA_SIZE));
    printf ("	%3d  %08X %08X Maximum ASN value\n",			ofh(ASN_MAX), ph(ASN_MAX));
    printf ("	%3d  %08X %08X System serial number\n",	ofh(SERIALNUM), hwrpbp->SERIALNUM [0] [1], hwrpbp->SERIALNUM [0] [0]);
    printf ("	%3d  %08X %08X\n", (offsetof(struct HWRPB, SERIALNUM)+8),hwrpbp->SERIALNUM [1] [1], hwrpbp->SERIALNUM [1] [0]);
    printf ("	%3d  %08X %08X Alpha system type\n",			ofh(SYSTYPE), ph(SYSTYPE));
    printf ("	%3d  %08X %08X system subtype\n",			ofh(SYSVAR), ph(SYSVAR));
    printf ("	%3d  %08X %08X System revision\n",			ofh(SYSREV), ph(SYSREV));
    printf ("	%3d  %08X %08X Interval clock interrupt frequency\n",	ofh(ICLK_FREQ), ph(ICLK_FREQ));
    printf ("	%3d  %08X %08X Cycle Counter frequency\n",		ofh(CC_FREQ), ph(CC_FREQ));
    printf ("	%3d  %08X %08X Virtual page table base\n",		ofh(VPTBR), ph(VPTBR));
    printf ("	%3d  %08X %08X Reserved for architecture use, SBZ\n",	ofh(RSVD_ARCH), ph(RSVD_ARCH));
    printf ("	%3d  %08X %08X Offset to Translation Buffer Hint Block\n",ofh(THB_OFFSET), ph(THB_OFFSET));
    printf ("	%3d  %08X %08X Number of processor supported\n",	ofh(NPROC), ph(NPROC));
    printf ("	%3d  %08X %08X Size of Per-CPU Slots in bytes\n",	ofh(SLOT_SIZE), ph(SLOT_SIZE));
    printf ("	%3d  %08X %08X Offset to Per-CPU Slots\n",		ofh(SLOT_OFFSET), ph(SLOT_OFFSET));
    printf ("	%3d  %08X %08X Number of CTBs in CTB table\n",		ofh(NUM_CTBS), ph(NUM_CTBS));
    printf ("	%3d  %08X %08X Size of largest CTB in CTB table\n",	ofh(CTB_SIZE), ph(CTB_SIZE));
    printf ("	%3d  %08X %08X Offset to Console Terminal Block\n",	ofh(CTB_OFFSET), ph(CTB_OFFSET));
    printf ("	%3d  %08X %08X Offset to Console Routine Block\n",	ofh(CRB_OFFSET), ph(CRB_OFFSET));
    printf ("	%3d  %08X %08X Offset to Memory Data Descriptors\n",	ofh(MEM_OFFSET), ph(MEM_OFFSET));
    printf ("	%3d  %08X %08X Offset to Configuration Data Table\n",	ofh(CONFIG_OFFSET), ph(CONFIG_OFFSET));
    printf ("	%3d  %08X %08X Offset to FRU Table\n",			ofh(FRU_OFFSET), ph(FRU_OFFSET));
    printf ("	%3d  %08X %08X Starting VA of SAVE_TERM routine\n",	ofh(SAVE_TERM), ph(SAVE_TERM));
    printf ("	%3d  %08X %08X Procedure Value of SAVE_TERM routine\n",	ofh(SAVE_TERM_VALUE), ph(SAVE_TERM_VALUE));
    printf ("	%3d  %08X %08X Starting VA of RESTORE_TERM routine\n",	ofh(RESTORE_TERM), ph(RESTORE_TERM));
    printf ("	%3d  %08X %08X Procedure Value of RESTORE_TERM routine\n",ofh(RESTORE_TERM_VALUE), ph(RESTORE_TERM_VALUE));
    printf ("	%3d  %08X %08X VA of restart routine\n",		ofh(RESTART), ph(RESTART));
    printf ("	%3d  %08X %08X Restart procedure value\n",		ofh(RESTART_VALUE), ph(RESTART_VALUE));
    printf ("	%3d  %08X %08X Reserved to System Software\n",		ofh(RSVD_SW), ph(RSVD_SW));
    printf ("	%3d  %08X %08X Reserved to Hardware\n",			ofh(RSVD_HW), ph(RSVD_HW));
    printf ("	%3d  %08X %08X Checksum of HWRPB\n",			ofh(CHKSUM), ph(CHKSUM));
    printf ("	%3d  %08X %08X RX Ready bitmask\n",			ofh(RXRDY), ph(RXRDY));
    printf ("	%3d  %08X %08X TX Ready bitmask\n",			ofh(TXRDY), ph(TXRDY));
    printf ("	%3d  %08X %08X Offset to DSRDB\n",			ofh(DSRDB_OFFSET), ph(DSRDB_OFFSET));

/* show all the cpu slots */

    slot = (void *) ((unsigned) hwrpbp + *hwrpbp->SLOT_OFFSET);
    for (i=0; i<*hwrpbp->NPROC; i++,slot++) {
	if ((cpu_mask & (1 << i)))
	    show_slot (hwrpbp, slot, i);
    }

/* show console terminal blocks */

    show_ctb (hwrpbp);

/* show console routine blocks */

    show_crb (hwrpbp);

/* show memory descriptors */

    show_desc (hwrpbp, (int)hwrpbp + *hwrpbp->MEM_OFFSET);

/* show dynamic system recognition data block */

    show_dsrdb(hwrpb, (int)hwrpbp + *hwrpbp->DSRDB_OFFSET);
}

void show_slot (struct HWRPB *hwrpb, struct SLOT *slot, int index)
{
    int	i;

    printf ("\n");
    printf ("%08X slot at index %d\n", slot, index);

    printf ("	%08X %08X KSP\n", slot->HWPCB.VMS_HWPCB.KSP [1], slot->HWPCB.VMS_HWPCB.KSP [0]);
    printf ("	%08X %08X ESP\n", slot->HWPCB.VMS_HWPCB.ESP [1], slot->HWPCB.VMS_HWPCB.ESP [0]);
    printf ("	%08X %08X SSP\n", slot->HWPCB.VMS_HWPCB.SSP [1], slot->HWPCB.VMS_HWPCB.SSP [0]);
    printf ("	%08X %08X USP\n", slot->HWPCB.VMS_HWPCB.USP [1], slot->HWPCB.VMS_HWPCB.USP [0]);
    printf ("	%08X %08X PTBR\n", slot->HWPCB.VMS_HWPCB.PTBR [1], slot->HWPCB.VMS_HWPCB.PTBR [0]);
    printf ("	%08X %08X ASN\n", slot->HWPCB.VMS_HWPCB.ASN [1], slot->HWPCB.VMS_HWPCB.ASN [0]);
    printf ("	%08X %08X ASTEN_SR\n", slot->HWPCB.VMS_HWPCB.ASTEN_SR [1], slot->HWPCB.VMS_HWPCB.ASTEN_SR [0]);
    printf ("	%08X %08X FEN\n", slot->HWPCB.VMS_HWPCB.FEN [1], slot->HWPCB.VMS_HWPCB.FEN [0]);
    printf ("	%08X %08X CC\n", slot->HWPCB.VMS_HWPCB.CC [1], slot->HWPCB.VMS_HWPCB.CC [0]);

    for (i=0; i<7; i++) {
	printf ("	%08X %08X SCRATCH [%d]\n",slot->HWPCB.VMS_HWPCB.SCRATCH [1] [i], slot->HWPCB.VMS_HWPCB.SCRATCH [0] [i], i);
    }

    printf ("	%d Boot in progress\n", slot->STATE.SLOT$V_BIP );
    printf ("	%d Restart capable\n", slot->STATE.SLOT$V_RC );
    printf ("	%d Processor available\n", slot->STATE.SLOT$V_PA );
    printf ("	%d Processor present\n", slot->STATE.SLOT$V_PP );
    printf ("	%d Operator halted\n", slot->STATE.SLOT$V_OH );
    printf ("	%d Context valid\n", slot->STATE.SLOT$V_CV );
    printf ("	%d Palcode valid\n", slot->STATE.SLOT$V_PV );
    printf ("	%d Palcode memory valid\n", slot->STATE.SLOT$V_PMV );
    printf ("	%d Palcode loaded\n", slot->STATE.SLOT$V_PL );
    printf ("	%d Reserved MBZ\n", slot->STATE.SLOT$V_RSVD_MBZ0 );
    printf ("	%d Halt requested	\n", slot->STATE.SLOT$V_HALT_REQUESTED );
    printf ("	%d Reserved MBZ\n", slot->STATE.SLOT$V_RSVD_MBZ1 );
    printf ("	%d Reserved MBZ\n", slot->STATE.SLOT$L_RSVD_MBZ2 );

    printf ("	%08X %08X PAL_MEM_LEN 		\n", slot->PAL_MEM_LEN [1], slot->PAL_MEM_LEN [0]);
    printf ("	%08X %08X PAL_SCR_LEN 		\n", slot->PAL_SCR_LEN [1], slot->PAL_SCR_LEN [0]);
    printf ("	%08X %08X PAL_MEM_ADR 		\n", slot->PAL_MEM_ADR [1], slot->PAL_MEM_ADR [0]);
    printf ("	%08X %08X PAL_SCR_ADR 		\n", slot->PAL_SCR_ADR [1], slot->PAL_SCR_ADR [0]);
    printf ("	%08X %08X PAL_REV 		\n", slot->PAL_REV [1], slot->PAL_REV [0]);
    printf ("	%08X %08X CPU_TYPE 		\n", slot->CPU_TYPE [1], slot->CPU_TYPE [0]);
    printf ("	%08X %08X CPU_VAR 		\n", slot->CPU_VAR [1], slot->CPU_VAR [0]);
    printf ("	%08X %08X CPU_REV 		\n", slot->CPU_REV [1], slot->CPU_REV [0]);

    for (i=0; i<2; i++) {
	printf ("	%08X %08X SERIAL_NUM		\n", slot->SERIAL_NUM [i] [1], slot->SERIAL_NUM [i] [0]);
    }

    printf ("	%08X %08X PAL_LOGOUT 		\n", slot->PAL_LOGOUT [1], slot->PAL_LOGOUT [0]);
    printf ("	%08X %08X PAL_LOGOUT_LEN 	\n", slot->PAL_LOGOUT_LEN [1], slot->PAL_LOGOUT_LEN [0]);
    printf ("	%08X %08X HALT_PCBB 		\n", slot->HALT_PCBB [1], slot->HALT_PCBB [0]);
    printf ("	%08X %08X HALT_PC 		\n", slot->HALT_PC [1], slot->HALT_PC [0]);
    printf ("	%08X %08X HALT_PS 		\n", slot->HALT_PS [1], slot->HALT_PS [0]);
    printf ("	%08X %08X HALT_ARGLIST 		\n", slot->HALT_ARGLIST [1], slot->HALT_ARGLIST [0]);
    printf ("	%08X %08X HALT_RETURN 		\n", slot->HALT_RETURN [1], slot->HALT_RETURN [0]);
    printf ("	%08X %08X HALT_VALUE 		\n", slot->HALT_VALUE [1], slot->HALT_VALUE [0]);
    printf ("	%08X %08X HALTCODE 		\n", slot->HALTCODE [1], slot->HALTCODE [0]);
    printf ("	%08X %08X RSVD_SW 		\n", slot->RSVD_SW [1], slot->RSVD_SW [0]);
    printf ("	%08X RXLEN			\n", slot->RXLEN);
    printf ("	%08X TXLEN			\n", slot->TXLEN);
    printf ("	%08X %08X CPU_COMP 		\n", slot->CPU_COMP [1], slot->CPU_COMP [0]);
}

void show_ctb_vt (struct CTB_VT *ctb) {

	printf ("	%08X %08X TYPE\n", ctb->CTB_VT$Q_TYPE [1], ctb->CTB_VT$Q_TYPE [0]);
	printf ("	%08X %08X ID\n", ctb->CTB_VT$Q_ID [1], ctb->CTB_VT$Q_ID [0]);
	printf ("	%08X %08X RSVD\n", ctb->CTB_VT$Q_RSVD [1], ctb->CTB_VT$Q_RSVD [0]);
	printf ("	%08X %08X DEV_DEP_LEN\n", ctb->CTB_VT$Q_DEV_DEP_LEN [1], ctb->CTB_VT$Q_DEV_DEP_LEN [0]);
	printf ("	%08X %08X CSR\n", ctb->CTB_VT$Q_CSR [1], ctb->CTB_VT$Q_CSR [0]);
	printf ("	%08X %08X TX_SCB_OFFSET\n", ctb->CTB_VT$R_TX_INT, ctb->CTB_VT$L_TX_SCB_OFFSET_L);
	printf ("	%08X %08X RX_SCB_OFFSET\n", ctb->CTB_VT$R_RX_INT, ctb->CTB_VT$L_RX_SCB_OFFSET_L);
	printf ("	%08X %08X BAUD\n", ctb->CTB_VT$Q_BAUD [1], ctb->CTB_VT$Q_BAUD [0]);
	printf ("	%08X %08X PUTS_STATUS\n", ctb->CTB_VT$Q_PUTS_STATUS [1], ctb->CTB_VT$Q_PUTS_STATUS [0]);
	printf ("	%08X %08X GETC_STATUS\n", ctb->CTB_VT$Q_GETC_STATUS [1], ctb->CTB_VT$Q_GETC_STATUS [1]);
}

void show_ctb_ws (struct CTB_WS *ctb) {

	printf ("	%08X %08X TYPE\n", ctb->CTB_WS$Q_TYPE [1], ctb->CTB_WS$Q_TYPE [0]);
	printf ("	%08X %08X ID\n", ctb->CTB_WS$Q_ID [1], ctb->CTB_WS$Q_ID [0]);
	printf ("	%08X %08X RSVD\n", ctb->CTB_WS$Q_RSVD [1], ctb->CTB_WS$Q_RSVD [0]);
	printf ("	%08X %08X DEV_DEP_LEN\n", ctb->CTB_WS$Q_DEV_DEP_LEN [1], ctb->CTB_WS$Q_DEV_DEP_LEN [0]);
	printf ("	%08X %08X DEV_IPL\n", ctb->CTB_WS$Q_DEV_IPL [1], ctb->CTB_WS$Q_DEV_IPL [0]);
	printf ("	%08X %08X TX_SCB_OFFSET\n", ctb->CTB_WS$R_TX_INT, ctb->CTB_WS$L_TX_SCB_OFFSET_L);
	printf ("	%08X %08X RX_SCB_OFFSET\n", ctb->CTB_WS$R_RX_INT, ctb->CTB_WS$L_RX_SCB_OFFSET_L);
	printf ("	%08X %08X TERM_TYPE\n", ctb->CTB_WS$Q_TERM_TYPE [1], ctb->CTB_WS$Q_TERM_TYPE [0]);
	printf ("	%08X %08X KB_TYPE\n", ctb->CTB_WS$Q_KB_TYPE [1], ctb->CTB_WS$Q_KB_TYPE [0]);
	printf ("	%08X %08X KB_TRN_TBL\n", ctb->CTB_WS$Q_KB_TRN_TBL [1], ctb->CTB_WS$Q_KB_TRN_TBL [0]);
	printf ("	%08X %08X KB_MAP_TBL\n", ctb->CTB_WS$Q_KB_MAP_TBL [1], ctb->CTB_WS$Q_KB_MAP_TBL [0]);
	printf ("	%08X %08X KB_STATE\n", ctb->CTB_WS$Q_KB_STATE [1], ctb->CTB_WS$Q_KB_STATE [0]);
	printf ("	%08X %08X LAST_KEY\n", ctb->CTB_WS$Q_LAST_KEY [1], ctb->CTB_WS$Q_LAST_KEY [0]);
	printf ("	%08X %08X US_FONT\n", ctb->CTB_WS$Q_US_FONT [1], ctb->CTB_WS$Q_US_FONT [0]);
	printf ("	%08X %08X MCS_FONT\n", ctb->CTB_WS$Q_MCS_FONT [1], ctb->CTB_WS$Q_MCS_FONT [0]);
	printf ("	%08X %08X FONT_WIDTH\n", ctb->CTB_WS$Q_FONT_WIDTH [1], ctb->CTB_WS$Q_FONT_WIDTH [0]);
	printf ("	%08X %08X FONT_HEIGHT\n", ctb->CTB_WS$Q_FONT_HEIGHT [1], ctb->CTB_WS$Q_FONT_HEIGHT [0]);
	printf ("	%08X %08X MONITOR_WIDTH\n", ctb->CTB_WS$Q_MONITOR_WIDTH [1], ctb->CTB_WS$Q_MONITOR_WIDTH [0]);
	printf ("	%08X %08X MONITOR_HEIGHT\n", ctb->CTB_WS$Q_MONITOR_HEIGHT [1], ctb->CTB_WS$Q_MONITOR_HEIGHT [0]);
	printf ("	%08X %08X MONITOR_DENSITY\n", ctb->CTB_WS$Q_MONITOR_DENSITY [1], ctb->CTB_WS$Q_MONITOR_DENSITY [0]);
	printf ("	%08X %08X NUM_PLANES\n", ctb->CTB_WS$Q_NUM_PLANES [1], ctb->CTB_WS$Q_NUM_PLANES [0]);
	printf ("	%08X %08X CURSOR_WIDTH\n", ctb->CTB_WS$Q_CURSOR_WIDTH [1], ctb->CTB_WS$Q_CURSOR_WIDTH [0]);
	printf ("	%08x %08X CURSOR_HEIGHT\n", ctb->CTB_WS$Q_CURSOR_HEIGHT [1], ctb->CTB_WS$Q_CURSOR_HEIGHT [0]);
	printf ("	%08X %08X NUM_HEADS\n", ctb->CTB_WS$Q_NUM_HEADS [1], ctb->CTB_WS$Q_NUM_HEADS [0]);
	printf ("	%08X %08X OPWIN\n", ctb->CTB_WS$Q_OPWIN [1], ctb->CTB_WS$Q_OPWIN [0]);
	printf ("	%08X %08X HEAD_OFFSET\n", ctb->CTB_WS$Q_HEAD_OFFSET [1], ctb->CTB_WS$Q_HEAD_OFFSET [0]);
	printf ("	%08X %08X PUTCHAR_ROUTINE\n", ctb->CTB_WS$Q_PUTCHAR_ROUTINE [1], ctb->CTB_WS$Q_PUTCHAR_ROUTINE [0]);
	printf ("	%08X %08X IO_STATE\n", ctb->CTB_WS$Q_IO_STATE [1], ctb->CTB_WS$Q_IO_STATE [0]);
	printf ("	%08X %08X LISTENER_STATE\n", ctb->CTB_WS$Q_LISTENER_STATE [1], ctb->CTB_WS$Q_LISTENER_STATE [0]);
	printf ("	%08X %08X EXT_INFO\n", ctb->CTB_WS$Q_EXT_INFO [1], ctb->CTB_WS$Q_EXT_INFO [0]);
	printf ("	%08X %08X TERM_OUT_LOC\n", ctb->CTB_WS$Q_TERM_OUT_LOC [1], ctb->CTB_WS$Q_TERM_OUT_LOC [0]);
	printf ("	%08X %08X SERVER_OFFSET\n", ctb->CTB_WS$Q_SERVER_OFFSET [1], ctb->CTB_WS$Q_SERVER_OFFSET [0]);
	printf ("	%08X %08X LINE_PARAM\n", ctb->CTB_WS$Q_LINE_PARAM [1], ctb->CTB_WS$Q_LINE_PARAM [0]);
	printf ("	%08X %08X TERM_IN_LOC\n", ctb->CTB_WS$Q_TERM_IN_LOC [1], ctb->CTB_WS$Q_TERM_IN_LOC [0]);
}

void show_ctb (struct HWRPB *hwrpb) {
    struct CTB_VT *ctb;

    ctb = (void *)((int)hwrpb + *hwrpb->CTB_OFFSET);

    printf ("\n");
    printf ("%08X	console terminal block\n", ctb);
    switch (*(ctb->CTB_VT$Q_TYPE)) {
	case CTB$K_DZ_VT:
	    show_ctb_vt (ctb);
	    break;   
	case CTB$K_DZ_GRAPHICS:
	case CTB$K_DZ_WS:
	    show_ctb_ws (ctb);
	    break;
	default:                  
	    printf ("Unrecognized CTB type = %08X %08X\n", ctb->CTB_VT$Q_TYPE [1], ctb->CTB_VT$Q_TYPE [0]);
	}
}

void show_crb (struct HWRPB *hwrpb)
{
    struct CRB		*crb;
    struct CRB_ENTRY	*crbe;
    int	i;

    crb = (int)hwrpb + *hwrpb->CRB_OFFSET;

    printf ("\n");
    printf ("%08X	console routine block\n", crb);

    printf ("	%08X %08X VDISPATCH\n", crb->CRB$Q_VDISPATCH [1], crb->CRB$Q_VDISPATCH [0]);
    printf ("	%08X %08X PDISPATCH\n", crb->CRB$Q_PDISPATCH [1], crb->CRB$Q_PDISPATCH [0]);
    printf ("	%08X %08X VFIXUP\n", crb->CRB$Q_VFIXUP [1], crb->CRB$Q_VFIXUP [0]);
    printf ("	%08X %08X PFIXUP\n", crb->CRB$Q_PFIXUP [1], crb->CRB$Q_PFIXUP [0]);
    printf ("	%08X %08X ENTRIES\n", crb->CRB$Q_ENTRIES [1], crb->CRB$Q_ENTRIES [0]);
    printf ("	%08X %08X PAGES\n", crb->CRB$Q_PAGES [1], crb->CRB$Q_PAGES [0]);

    crbe = & crb->CRB$R_ENTRY [0];
    for (i=0; i<crb->CRB$Q_ENTRIES [0]; i++, crbe++) {
	printf ("\n");
	printf ("	%08X %08X V_ADDRESS\n", crbe->CRB_ENTRY$Q_V_ADDRESS [1], crbe->CRB_ENTRY$Q_V_ADDRESS [0]);
	printf ("	%08X %08X P_ADDRESS\n", crbe->CRB_ENTRY$Q_P_ADDRESS [1], crbe->CRB_ENTRY$Q_P_ADDRESS [0]);
	printf ("	%08X %08X PAGE_COUNT\n", crbe->CRB_ENTRY$Q_PAGE_COUNT [1], crbe->CRB_ENTRY$Q_PAGE_COUNT [0]);
    }
}

void show_desc (struct HWRPB *hwrpb, struct MEMDSC *memdsc)
{
    int	i, j;
    int	bad;
    union quadadr {
	UINT qadr;
	int ladr[2];
    } memadr;

    printf ("\n");
    printf ("%08X	memory descriptor\n", memdsc);
    printf ("	%08X %08X CHECKSUM\n", memdsc->CHECKSUM [1], memdsc->CHECKSUM [0]);
    printf ("	%08X %08X IMP_DATA_PA\n", memdsc->IMP_DATA_PA [1], memdsc->IMP_DATA_PA [0]);
    printf ("	%08X %08X CLUSTER_COUNT\n", memdsc->CLUSTER_COUNT [1], memdsc->CLUSTER_COUNT [0]);

    for (i=0; i<memdsc->CLUSTER_COUNT [0]; i++) {
	printf ("\n");
	printf ("	%08X %08X START_PFN\n", memdsc->CLUSTER [i].START_PFN [1], memdsc->CLUSTER [i].START_PFN [0]);
	printf ("	%08X %08X PFN_COUNT\n", memdsc->CLUSTER [i].PFN_COUNT [1], memdsc->CLUSTER [i].PFN_COUNT [0]);
	printf ("	%08X %08X TEST_COUNT\n", memdsc->CLUSTER [i].TEST_COUNT [1], memdsc->CLUSTER [i].TEST_COUNT [0]);
	printf ("	%08X %08X BITMAP_VA\n", memdsc->CLUSTER [i].BITMAP_VA [1], memdsc->CLUSTER [i].BITMAP_VA [0]);
	printf ("	%08X %08X BITMAP_PA\n", memdsc->CLUSTER [i].BITMAP_PA [1], memdsc->CLUSTER [i].BITMAP_PA [0]);
	printf ("	%08X %08X BITMAP_CHKSUM\n", memdsc->CLUSTER [i].BITMAP_CHKSUM [1], memdsc->CLUSTER [i].BITMAP_CHKSUM [0]);
	printf ("	%08X %08X USAGE\n", memdsc->CLUSTER [i].USAGE [1], memdsc->CLUSTER [i].USAGE [0]);

/* Summarize the bad pages in a cluster.  Ignore undefined bitmaps. */

	bad = 0;
	memadr.ladr[0] = memdsc->CLUSTER [i].BITMAP_PA [0];
	memadr.ladr[1] = memdsc->CLUSTER [i].BITMAP_PA [1];

#if NONZERO_CONSOLE_BASE
	if (memadr.qadr)
	    memadr.qadr = phys_to_virt(memadr.qadr);
#endif

	for (j=0;memadr.qadr && j<memdsc->CLUSTER [i].PFN_COUNT [0]; j++) {
	    if (getbit64 (j, memadr.qadr) == 0) {
		bad++;
	    }
	}
	printf ("	         %8d bad page(s)\n", bad);
    }
}

void show_dsrdb(struct HWRPB *hwrpb, struct DSRDB *dsrdb)
{
    int i, j;
    int lurt_count;
    int name_count;
    int *lurt;
    char *name;

    printf ("\n");
    printf ("%08X	Dynamic System Recognition Data block\n", dsrdb);
    printf ("	%08X %08X SMM\n", dsrdb->SMM [1], dsrdb->SMM [0]);
    printf ("	%08X %08X Offset to LURT\n", dsrdb->LURT_OFFSET [1], dsrdb->LURT_OFFSET [0]);
    printf ("	%08X %08X Offset to Name Count\n", dsrdb->NAME_OFFSET [1], dsrdb->NAME_OFFSET [0]);

    lurt = (int)dsrdb + dsrdb->LURT_OFFSET[0];
    lurt_count = *lurt;
    printf ("	%08X %08X LURT Count\n", 0, lurt_count);

    lurt = lurt + 1;

    for (i=1, j=1; i <= (lurt_count*2); i+=2, j++)
	printf ("	%08X %08X LURT Column %d\n",
	  *(lurt +i+1), *(lurt +i) ,j);

    name_count = *(int *)((int)dsrdb + dsrdb->NAME_OFFSET[0]);

    printf ("	%08X %08X Name Count\n", 0, name_count);

    name = ((int)dsrdb + dsrdb->NAME_OFFSET[0]) + 8;
    printf ("	Platform Name = %s\n", name);
}

#endif
