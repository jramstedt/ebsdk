/*+
 * file:	mc_driver.c
 *
 * Copyright (C) 1995 by
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
 *++
 *  FACILITY:
 * 
 *      Console Firmware
 *
 *  ABSTRACT:	Memory Channel driver.
 *
 *      This module defines Pci Memory Channel Flash Rom resident
 *	powerup diagnostics.  A non Rom resident Manufacturing/Field Service
 *      MC-Diagnostic will also be available to complement these powerup 
 *	diagnostic tests.  There is also a console based Pci Memory
 *	Channel exerciser.
 *
 *      Tests within this module will be called during console startup.
 *
 * Author:
 *
 *	William Clemence
 *
 * Creation Date:
 *
 *	24-May-1995
 *
-*/

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$src:kernel_def.h"
#include "cp$src:parse_def.h"		/* struct QSTRUCT */
#include "cp$inc:platform_io.h"
#include "cp$src:dynamic_def.h"		/* Memory allocation definitions */
#include "cp$src:msg_def.h"
#include "cp$src:ctype.h"
#include "cp$src:pb_def.h"		/* Port Block (PB) Definitions */
#include "cp$src:pcimc_def.h"
#include "cp$inc:kernel_entry.h"	/* needed for Turbo */
#include "cp$inc:prototypes.h"

/* debug */
#define DEBUG_PRINT 0
#if DEBUG_PRINT
#define dprintf printf
#else
#define dprintf
#endif

#if !(TURBO || RAWHIDE)
#define fix_mc_names
#define mc_mark_dev_bad
#define GET_MC_NAME(a,b) strcpy(a,b)
#else
#define GET_MC_NAME(a,b) dev_to_mc_name(a,b)
#endif

extern struct ZONE *memzone;
extern struct LOCK spl_kernel;
extern null_procedure();
extern int cpip;			/* console powerup in progress flag */
extern unsigned int poweron_msec[2];	/* see timer.c */

#if !TURBO
extern unsigned long indmeml( struct pb *pb, unsigned __int64 offset );
extern void outdmeml( struct pb *pb, unsigned __int64 offset, unsigned long data );
#endif

/*
/*  Forward Test Reference Declarations
*/
static int pcimcpup_t12s(struct PCIMC_PB *mcpb);
static int pcimcpup_t3s(struct PCIMC_PB *mcpb);
static int mc_initpb(struct PCIMC_PB *mcpb);

/*
 * These globals to this program will keep track of the MCPBs found
 * by find_pb.
*/
static int mcpb_count;
static struct PCIMC_PB *mcpb_pointers[PCIMC_MAX_SLOT];

static int mc_tests_running = 0;
static int data_long[] = {0xFFFFFFFF, 0x00000000, 0x55555555, 0xAAAAAAAA};
static int data_short[] = {0x5555, 0xAAAA};
static int dump_count = 0; /* for register dump header printing */

static char *mc_node_respond = "%sResponse from node %d on %s\n";
static char *mc_testing = "Testing MC-Adapter(s)\n";
static char *mc_testing_failed = "Adapter %s, failed t-%d\n";
static char *mc_detect_error = "Detected errors on\n";
static char *mc_dump_header = 
  "         LCSR    MCERR    MCPORT   PRBAR    CFG04    CFG10    MODCFG     POS \n";
                                                                 
#if TURBO || SABLE
static char *mc_config_err1 = 
  "********************************************************\7\n";
static char *mc_config_err2 = 
  "** Memory Channel hardware requirement ERROR  # %-2d    **\n";
static char *mc_config_err3 = 
  "** See release notes, or Memory Channel User's Guide. **\n";
#endif

/*+
 * ============================================================================
 * = pcimcpup_t3s - PCT Sequential Write Read Test			      =
 * ============================================================================
 *
 * OVERVIEW:
 *	PCT is a 64kb Write Read memory.  This test will sequentially
 *	write/read all 64kb PCT locations with various data patterns.
 *	PCT Parity is NOT generated nor verified during this test.
 *
 * Condensed version of above test; only do 2 data patterns 5's and A's.
-*/
static int pcimcpup_t3s(struct PCIMC_PB *mcpb)
{
    unsigned __int64 xdata = 0x0000FFFF, qdata, el;

    unsigned int PctAdrs, x, i, j, wdata;

    xdata = (xdata << 32) | xdata;

    for (PctAdrs = PCT$K_PctBase; PctAdrs < PCT$K_PctSize; PctAdrs += 2) {

	for (j = 0; j < 2; j++) {
	    wdata = data_short[j];

	    el = (unsigned __int64) wdata | (unsigned __int64) wdata << 32;

	    outdmemq(mcpb, &mcpb->DmemBase->pct[PctAdrs], el);

	    qdata = xdata & indmemq(mcpb, &mcpb->DmemBase->pct[PctAdrs]);

	    if (qdata != el) {
		dprintf("PctOff  : %d\nFailAdrs: %08x\n", PctAdrs,
		  &mcpb->DmemBase->pct[PctAdrs]);
		dprintf("Expected: %016x\nActual  : %016x\n", el, qdata);

		return (msg_failure);
	    }

	}
    }

    /* load it with the address */
    for (PctAdrs = PCT$K_PctBase; PctAdrs < PCT$K_PctSize; PctAdrs++)
	PCT_WRITE(mcpb, &mcpb->DmemBase->pct[PctAdrs], PctAdrs);

    /* check the address */
    for (PctAdrs = PCT$K_PctBase; PctAdrs < PCT$K_PctSize; PctAdrs++) {
	x = PCT_READ(mcpb, &mcpb->DmemBase->pct[PctAdrs]) & 0xFFFF;

	if (PctAdrs != x) {
	    dprintf("PctOff  : %d\nFailAdrs: %08x\n", PctAdrs,
	      &mcpb->DmemBase->pct[PctAdrs]);
	    dprintf("Expected: %08x\nActual  : %08x\n", PctAdrs, x);

	    return (msg_failure);
	}
	check_kill();
    }

    return (msg_success);
}

/*+
 * ============================================================================
 * = pcimcpup_t12s  - 1mb MC-Loopback test                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *	This test enables 1 MB of MC Pages.  These pages are 
 *	Loopbacked tested using varying data patterns.
 *
-*/
static int pcimcpup_t12s(struct PCIMC_PB *mcpb)
{
    unsigned __int64 offset;
    unsigned __int64 p;
    unsigned int buf[2];

    unsigned int i, j, MCMalloc, MCTstPage, MCNumPages, MCPage, rdata, XmtLw,
      RcvLw;

#if TURBO || RAWHIDE
    MCMalloc = dyn$_malloc(SIZE$K_64K + SIZE$K_8K,
      DYN$K_NOWAIT | DYN$K_SYNC | DYN$K_ALIGN, SIZE$K_8K, 0, 0);
    MCNumPages = SIZE$K_64K / mcpb->page_size;
#else
    if (memzone) {
	MCMalloc = dyn$_malloc(SIZE$K_1MB + SIZE$K_8K, DYN$K_ZONE | 
	  DYN$K_NOWAIT | DYN$K_SYNC | DYN$K_ALIGN, SIZE$K_8K, 0, memzone);
	MCNumPages = SIZE$K_1MB / mcpb->page_size;
    }
#endif

    if (MCMalloc) {
	MCTstPage = MCMalloc / mcpb->page_size;

	dprintf("pcimcpup_t12 MC-Receive Malloc'ed Memory\n");
	dprintf("\tMCTstPage  : %d\n", MCTstPage);
	dprintf("\tMCNumPages : %d\n", MCNumPages);
	dprintf("\tMallocAdrs : %x\n\n", MCMalloc);

	for (MCPage = MCTstPage; MCPage <= MCTstPage + MCNumPages; MCPage++) {

	    /* set pct to PCT$V_MCXmtEn | PCT$V_LoopBack | PCT$V_MCRcvEn */
	    PCT_WRITE(mcpb, &mcpb->DmemBase->pct[MCPage], 0xC8);

	    for (j = 0; j < 4; j++) {

		buf[0] = data_long[j];
		buf[1] = data_long[j];

		/* create the proper offset to write too. */
		offset = (unsigned int)mcpb->DmemBase | 
		  (MCPage * mcpb->page_size);

		p = xdmem(mcpb, offset);

		/* Do one long word first and see if it works. */
		*(unsigned __int64 *) (p) = *(unsigned __int64 *) buf;
		mb();
		mb();
		krn$_sleep(1);

		if ((rdata = *(int *)(MCPage * mcpb->page_size)) != 
		  data_long[j]) {
		    dprintf("MC-Loopback Failure Exp %x, Rcv %x, ",
		      data_long[j], rdata);
		    dprintf("MCPage %d, Rcvlw 0\n", MCPage);
		    free(MCMalloc);
		    return (msg_failure);
		}

		/* Now do the remaining ones. */
		for (i = 0; i < mcpb->page_size; i+=ObjSize$K_QW) {
		    *(unsigned __int64 *) (p) = *(unsigned __int64 *) buf;
		    p += ObjSize$K_QW;
		}
		mb();
		mb();

		krn$_sleep(10);
		for (RcvLw = 0; RcvLw < mcpb->page_size; RcvLw+=4) {
		    if ((rdata = *(int *)((MCPage*mcpb->page_size) + RcvLw))
		      != data_long[j]) {
			dprintf("MC-Loopback Failure Exp %x, Rcv %x, ",
			  data_long[j], rdata);
			dprintf("MCPage %d, Rcvlw %x\n", MCPage, RcvLw/4);
			free(MCMalloc);
			return (msg_failure);
		    }
		}

	    }
	}

	free(MCMalloc);
    }
    return (msg_success);
}

/*+
 * ============================================================================
 * = pcimc_reset - reset the Memory Channel card                              =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *
-*/
static void pcimc_reset(struct PCIMC_PB *mcpb)
{

    unsigned int x, i;


    /* reset the module */
    LCSR_WRITE(mcpb, LCSR$V_PUT | LCSR$V_RESET);
    krn$_micro_delay(500);
    LCSR_WRITE(mcpb, LCSR$V_PUT);

/*
 *  Initialize the 64k pcts
*/
#define BMC_PCT 0x1

    /* Write bad pct everywhere. */
    for (i = 0; i < PCT$K_PctSize; i++) {
	PCT_WRITE(mcpb, &mcpb->DmemBase->pct[i], BMC_PCT);
#if DEBUG_PRINT
	x = 0xFFFF & PCT_READ(mcpb, &mcpb->DmemBase->pct[i]);

	if (x != BMC_PCT)
	    printf("%s FailPct : %d\n FailAdrs: %08x\n Expected: %08x\n Actual  : %08x\n",
	      mcpb->name, i, &mcpb->DmemBase->pct[i], BMC_PCT, x);
#endif
    }

    /* Reset our receive base address register */
    PRBAR_WRITE(mcpb, io_get_window_base(mcpb));
}
/*+
 * ============================================================================
 * = pcimc_dump - Dump errors from the Memory Channel card                    =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *
-*/
static void pcimc_dump(struct PCIMC_PB *mcpb)
{
    if (!dump_count) {
	printf(mc_dump_header);
        dump_count++;
    }                                              
    printf("%s - %08x %08x %08x %08x %08x %08x", mcpb->short_name,
      LCSR_READ(mcpb), MCERR_READ(mcpb), MCPORT_READ(mcpb), PRBAR_READ(mcpb), 
      incfgl(mcpb, PCIMC_CONFIG04), incfgl(mcpb, PCIMC_CONFIG10));
    if (!mcpb->pcimc_mc2_mode) /* if mc1, print n/a for MC2-only regs */
      printf("    n/a      n/a\n");
    else                       /* else mc2 - print modcfg register */
      printf(" %08x %08x\n", MODCFG_READ(mcpb) & 0x000000FF, 
         POS_READ(mcpb) & 0x0000FFFF);
}

/*+
 * ============================================================================
 * = mc_diag - Memory Channel Read/Write & Loopback diagnostic.               =
 * ============================================================================
 *
 * OVERVIEW:
 *	This is a diagnostic for testing the PCI Memory Channel hardware. The
 *	command performs two tests.  The first test does a sequential PCT 
 *	write/read test.  The second test enables up to 1MB of MC pages which 
 *	are loopback-tested using varying data patterns.  The command simply 
 *	displays a "passed" or "failed device" with a MC module present. With 
 *	no MC module, nothing is displayed and console returns to the prompt.
 *
 *   COMMAND FORM:
 *
 *	mc_diag ( [-d] )
 *
 *   COMMAND TAG: mc_diag 0 RXB mc_diag
 *   COMMAND FMT: mc_diag 5 H 0 mc_diag
 *
 *   COMMAND ARGUMENTS:
 *
 *   COMMAND OPTION(S):
 *
 *	-d - dump data flag. If defined, test is not run and the register
 *	     contents are displayed for LCSR, MCERR, MCPORT, PRBAR, CFG04.
 *                                  (MODCFG and POS for mc2 only).
 *
 *   COMMAND EXAMPLE(S):
 *
 *~
 *	P00>>>mc_diag
 *	Testing MC-Adapter(s)
 *	Adapter mca0, Passed
 *	Adapter mcb0, Passed
 *	P00>>>
 *~
 *~
 *	P00>>>mc_diag -d
 *	         LCSR    MCERR    MCPORT   PRBAR    CFG04    CFG10  
 *	mca0 - 00000800 00000000 02010000 00000000 04000006 c0000008
 *	mcb0 - 00000800 00000000 02010000 00000000 04000006 c8000008
 *	P00>>>
 *~
 *
 *   COMMAND REFERENCES:
 *        mc_diag
 *
 * FORM OF CALL:
 *	mc_diag (argc, *argv[])
 *  
 * ARGUMENTS:
 *	argc 	- number of command line arguments passed by the shell
 *	*argv[]	- array of pointers to arguments
 *
 * RETURN CODES:
 *	msg_success - Normal completion.
-*/

int mc_diag(unsigned int argc, char *argv[])
{
    struct PCIMC_PB *mcpb;
    int first = 1;
    int i;
    int status;
    int found = 0;
    struct QSTRUCT quals[2];
    char name[32];			/* General holder for a name */

    qscan(&argc, argv, "-", "d q", &quals);

    /* save the name if we were asked to test a specific one. */
    if (argc > 1) {
	first = 0;
	GET_MC_NAME(name, argv[1]);
    }

    dump_count = 0;

    for (i = 0; i < mcpb_count; i++) {
	mcpb =  mcpb_pointers[i];

	/* match the name up if we were asked to test for a specific one. */
	if (argc > 1) {
	    if (strwcmp_nocase(name, "mc?0")  ||
	      tolower(name[2]) != mcpb->pb.controller + 'a')
		continue;
	}

	found = 1;

	/* In case we came back from the dead reset our base
	 * address pointers. */
	mc_initpb(mcpb);

	/* if the -d switch was used then just dump the register else
	 * test this thing..... */
	if (quals[0].present) {
	    pcimc_dump(mcpb);
	} else {

	    /* only display this once. */
	    if (first & !cpip) {
		printf(mc_testing);
		first = 0;
	    }
            dump_count = 0; 
	    status = msg_success;
	    pcimc_reset(mcpb);

	    dprintf("starting test 12\n");

#if TURBO
	    /* Only new rev hpc will let the MC work in slots > 7. */
	    if (mcpb->pb.slot > 7) {
		if ((pcia_incsr(mcpb, 0x20018) & 0x78) == 0) {
		    err_printf(mc_config_err1);
		    err_printf(mc_config_err2,2);
		    err_printf(mc_config_err3);
		    err_printf(mc_config_err1);
		}
	    }
#endif
	    if (pcimcpup_t12s(mcpb) != msg_success) {
        	err_printf(mc_testing_failed, mcpb->short_name, 12);
		pcimc_dump(mcpb);
		mc_mark_dev_bad(mcpb);
		status = msg_failure;
	    } else {
		dprintf("starting test 3\n");
		if (pcimcpup_t3s(mcpb) != msg_success) {
		    err_printf(mc_testing_failed, mcpb->short_name, 3);
		    pcimc_dump(mcpb);
		    mc_mark_dev_bad(mcpb);
		    status = msg_failure;
		} else {
		    dprintf("Check mcerr fatal bit\n");
		    if (MCERR_READ(mcpb) & 0x40000) { 
			err_printf(mc_detect_error);
			pcimc_dump(mcpb);
			mc_mark_dev_bad(mcpb);
			status = msg_failure;
		    }
		}
	    }
	    pcimc_reset(mcpb);

	    if (status == msg_success) {

		/* report passed */
		printf("Adapter %s, Passed \n", mcpb->short_name);
	    }

	}
    }					/* end for */


    /* report wrong name only if we were asked to specifically test one. */
    if (argc > 1 && !found)
	err_printf(msg_no_device, argv[1]);

    dprintf("finished tests\n");

    return msg_success;

}

/*+
 * ============================================================================
 * = mc_cable - Memory Channel cable check diagnostic. 			      =
 * ============================================================================
 *
 * OVERVIEW:
 *	A diagnostic to check PCI Memory Channel cable connections.
 *	The program will report all established connections to all
 *   	Memory Channel adapters. It will then report any changes in
 *   	status on any of the connections. If the MC module is present AND the
 *	cable is either not present or not good, the command will not output 
 *	anything. Either way, the user is required to exit this routine by 
 *	typing ctrl/c.
 *
 *   COMMAND FORM:
 *
 *         mc_cable ( ) 
 *
 *   COMMAND TAG: pcimc_cable 0 RXB mc_cable
 *   COMMAND FMT: pcimc_cable 5 H 0 mc_cable
 *
 *   COMMAND ARGUMENTS:
 *
 *   COMMAND OPTION(S):
 *
 *   COMMAND EXAMPLE(S):
 *
 *~
 *	P00>>>mc_cable
 *	To exit MC_CABLE, type [CTRL/C]
 *	mca0 node id 0 is online
 *	No Response from node 1 on mca0
 *	mcb0 node id 1 is online
 *	Response from node 0 on mcb0
 *	Response from node 1 on mca0
 * 	^C
 *	P00>>>^C
 *	P00>>>
 *~
 *
 *   COMMAND REFERENCES:
 *        pcimc_cable
 *
 * FORM OF CALL:
 *	mc_cable (argc, *argv[])
 *  
 * ARGUMENTS:
 *	argc 	- number of command line arguments passed by the shell
 *	*argv[]	- array of pointers to arguments
 *
 * RETURN CODES:
 *	msg_success - Normal completion.
-*/
/*
 * Our test page is a reserved Memory Channel page based upon an agreement with
 * the OS group's so that we, diagnostics, don't step on them. This page was
 * page 36 with the assumption of always having 8k page sizes. Well that
 * was not always true but the address range must still be true. So this
 * preposes that we own address window 48000 hex (page 36 times 8k) for at 
 * an 8k range after that 48000-4A000.
*/
#define MCLOOP_PAGE 0x48000
#define MCLOOP_ACK 1
#define MCLOOP_NOACK 2
#define MCLOOP_OFFLINE 0
#define MCLOOP_ONLINE 2

int pcimc_cable(unsigned int argc, char *argv[])
{


    struct PCIMC_PB *mcpb;
    unsigned char *AckPtr;
    unsigned int *wbuf;
    unsigned int mcport, mcprbar;

    unsigned int j, i, lcsr;
    unsigned int flip = 0;
    
    /*  0-15 nodes that would respond
     *  16 on-line/off-line indicator
     *  17 virtual hub config going on-line indicator
     */

    int node_state[PCIMC_MAX_SLOT][18];  

    dprintf("Loop address is %x or pct %d/%d \n", MCLOOP_PAGE,
      MCLOOP_PAGE/8192, MCLOOP_PAGE/4096);

    if (mc_tests_running) {
	printf("MC_CABLE already running\n");
	return msg_success;
    }
    mc_tests_running = 1;

    /* Tell user how to get out. */
    if (mcpb_count) 
        printf("To exit MC_CABLE, type <CTRL/C>\n");
    else
	printf("No MC modules are present - command not executed.\n");

    dump_count = 0; /* for dump header printing (error cases only) */

    /* Turn on our new mapping */
    for (i = 0; i < mcpb_count; i++) {
	mcpb =  mcpb_pointers[i];

	/* In case we came back from the dead reset our base
	 * address pointers and ourselfs. */
	mc_initpb(mcpb);
	pcimc_reset(mcpb);

	mcprbar = mc_sg_map_on(mcpb);
    }

    AckPtr = 0x500000 + MCLOOP_PAGE;


    /* force all nodes to respond. */
    for (i = 0; i < PCIMC_MAX_SLOT; i++) {
	for (j = 0; j < 18; j++)
	    node_state[i][j] = 0;
    }
    do {
	for (i = 0;(i < mcpb_count) && (!killpending()); i++) {
	    mcpb =  mcpb_pointers[i];

	    flip = ~flip;

	    mcport = MCPORT_READ(mcpb);

	    /* If the MC adapter is offline; then reset it and try to  go
	     * online. */
	    if (!(mcport & MCPORT$V_LnkHubOnLine)) {

		/* If we entered here before and this is virtual hub
		 * then just wait to go on-line. */
		if ((node_state[i][17]) &&
		  (LCSR_READ(mcpb) & LCSR$V_VIRTUAL_HUB_CONFIG))
		    continue;

	 	if (node_state[i][16] != MCLOOP_OFFLINE) {
		    printf("%s is offline\n", mcpb->short_name);
		    node_state[i][16] = MCLOOP_OFFLINE;
		}                      

		pcimc_reset(mcpb);
		krn$_sleep(10);
		    
		/* if we don't see the hub ok signal then don't try to
		 * go on-line on hub config only. Fixes a hub hardware
		 * bug. */
		if (!(LCSR_READ(mcpb) & LCSR$V_VIRTUAL_HUB_CONFIG)) {
		    if (!(MCPORT_READ(mcpb) & MCPORT$V_LnkHubOk))
			continue;
		}

		/* Set the OnLine Enable bit and clear Put */
		MCPORT_WRITE(mcpb, MCPORT_READ(mcpb) | MCPORT$V_OnLineEnable);
		LCSR_WRITE(mcpb, LCSR_READ(mcpb) & ~LCSR$V_PUT);

		/* reset all nodes displayed if brought on-line */
		for (j = 0; j < 16; j++)
		    if (node_state[i][j])
			node_state[i][j] = MCLOOP_NOACK;

		/* If just going on-line delay a bit more */
		krn$_sleep(500);

		node_state[i][17] = 1; /* flag indicating about to go online */

	    } else {

		/* If the MC adapter in online. */

		node_state[i][17] = 0; /* clear the 'about to go online' flag */
		if (node_state[i][16] != MCLOOP_ONLINE) {
		    printf("%s node id %d is online\n", mcpb->short_name,
		      (mcport >> 16) & 0x3F);
		    node_state[i][(mcport >> 16) & 0x3F] = MCLOOP_NOACK;
		    node_state[i][16] = MCLOOP_ONLINE;
		}

		/*  Set our new prbar value. */
		PRBAR_WRITE(mcpb, mcprbar);

		/* Enable this pct to have (PCT$V_Ack|PCT$V_BrdCst|
		 * PCT$V_MCXmtEn|PCT$V_MCRcvEn) on */
		PCT_WRITE(mcpb, 
		  &mcpb->DmemBase->pct[MCLOOP_PAGE/mcpb->page_size], 0x1e8);

		/* The MC-Ack page is 64 bytes, ie DHW, ie 16LW, in length.
		 *  The MC-Write can occur anywhere within this DHW.  The
		 * returning Ack bytes will be received starting at byte
		 * offset 0, ie LW0, within the DHW. */
		for (j = 0; j < 16; j++)
		    AckPtr[j] = 0xff;

		/* Send the data. We use flip for the send data; crude 
		 * but very small code. */
		outdmeml(mcpb,
		  (unsigned int) mcpb->DmemBase | (MCLOOP_PAGE + 60), flip);

		lcsr = LCSR_READ(mcpb);
		if (lcsr & (LCSR$V_XMTERR | LCSR$V_RCVERR)) {
		    err_printf(mc_detect_error);
		    pcimc_dump(mcpb);
		    outcfgl(mcpb, PCIMC_CONFIG04, incfgl(mcpb, PCIMC_CONFIG04));
		    pcimc_reset(mcpb);
		} else {

		    krn$_sleep(700);
		    for (j = 0; j <
		      ((lcsr & LCSR$V_VIRTUAL_HUB_CONFIG) ? 2 : 8);
		      j++) {
			if (AckPtr[j] == 0xff) {
			    if (node_state[i][j] != MCLOOP_NOACK) {
				printf(mc_node_respond, "No ", j,
				  mcpb->short_name);
				node_state[i][j] = MCLOOP_NOACK;
			    }
			    dprintf("No ACK MC-node : %d\n", j);
			} else {
			    if (node_state[i][j] != MCLOOP_ACK) {
				printf(mc_node_respond, "", j,
				  mcpb->short_name);
				node_state[i][j] = MCLOOP_ACK;
			    }
			    dprintf("Ack MC-node: %d\tAck Data: %x\n", j,
			      AckPtr[j]);
			}

		    }
		}

#if DEBUG_PRINT
		krn$_sleep(4000);
#endif
	    }				/* end node on-line */

	}				/* end for */

	/* Repeat every 1/2 second */
	krn$_sleep(500);

    } while (!(killpending()) && (mcpb_count));


    /* Go and reset the adapter to place offline. */
    for (i = 0; i < mcpb_count; i++) {
	mcpb =  mcpb_pointers[i];

	pcimc_reset(mcpb);
    }

    /* Turn off our new mapping. Note do it here so other nodes acking us will 
     * not get through. */
    for (i = 0; i < mcpb_count; i++) {
	mcpb =  mcpb_pointers[i];
	mc_sg_map_off(mcpb);
    }

    mc_tests_running = 0;

    return msg_success;
}

/*+
 * ============================================================================
 * = mc_init - Initialization routine for the mc adapters.		      =
 * ============================================================================
 *
 * OVERVIEW:
 *      This routine is called during powerup and to find and init all the 
 *	memory channel(s).
 *
 * CALLED FROM:
 *
 *      filesys.c  ddb_startup(phase);  dst[i]
 *
 * RETURNS:
 *      msg_success or msg_failure
 *
-*/
int mc_init()
{
    int mc_init1_pb();

    mcpb_count = 0;
#if MODULAR
    mc_find_pb("mc", sizeof(struct PCIMC_PB), mc_init1_pb);
#else
#if SBLOAD || MKLOAD
    find_pb("mc", 0, mc_init1_pb);
#else 
    find_pb("mc", sizeof(struct PCIMC_PB), mc_init1_pb);
#endif
#endif
    
    return msg_success;
}

/*+
 * ============================================================================
 * = mc_reinit - Reinitialization routine for the mc devices.		      =
 * ============================================================================
 *
 * OVERVIEW:
 *      This routine is called to reset/reinit the mc adapters.
 *
 * RETURNS:
 *      msg_success
 *
-*/
int mc_reinit()
{
    struct PCIMC_PB *mcpb;
    unsigned int i;

    dprintf("mc_reinit called\n");

    /* Walk through and reset the adapters. */
    for (i = 0; i < mcpb_count; i++) {
	mcpb =  mcpb_pointers[i];

	/* In case we came back from the dead reset our base
	 * address pointers. */
	mc_initpb(mcpb);

	/* now do the meat-and-potatoes; reset */
	pcimc_reset(mcpb);
    }

    return msg_success;
}

/*+
 * ============================================================================
 * = mc_init1_pb - Pci Memory Channel port diagnostic			      =	
 * ============================================================================
 *
 * OVERVIEW:
 *	This routine is called by find_pb above for every pb found.
 *	It will fill in the mcpb information and save a table of mcpb
 *	pointers.
 *
 *
 * RETURNS:
 *	msg_success or FAILURE.
-*/
int mc_init1_pb(struct PCIMC_PB *mcpb)
{
    
#if SABLE
    struct pb pb;

    pb.function = 0;
    pb.slot = 2;
    pb.bus = 0;
    /* Check PCEB rev and non-primary bus */
    if (incfgb(&pb,8) < 4 || mcpb->pb.bus) {
	err_printf(mc_config_err1);
	err_printf(mc_config_err2,(mcpb->pb.bus) ? 3 : 1);
	err_printf(mc_config_err3);
	err_printf(mc_config_err1);
	krn$_sleep(1000);
    }
#endif
    dprintf("Entering mc_init1_pb\n");

    /* only save so many. */
    if (mcpb_count < PCIMC_MAX_SLOT) {

	/* Save this mcpb in the mcpb_pointer array */
	mcpb_pointers[mcpb_count] = mcpb;
	mcpb_count++;

	mc_initpb(mcpb);		/* Defines MC Adapter port data */

	pcimc_reset(mcpb);

	dprintf("Done mc_init1_pb\n");
    }

    return msg_success;
}

/*+
 * ============================================================================
 * = mc_initpb - Initialization of the MC-PortBlock                           =
 * ============================================================================
 *
 * OVERVIEW:
 *	Port Blocks are the means by which the console/probe_io saves and
 *	announces device characteristics for other software to enjoy.
 *	In addition to those things save by probe_io, the devices init code
 *	extends the definition of its devices port block to enclude other
 *	meaningful runtime information.  This routine completes the building
 *	of the MC-Pci Adapter port block.
 *	
 * ARGUMENTS:
 *	mcpb 	- Re-allocated device port block pointer
 *
 * RETURNS:
 *	msg_success
-*/
static int mc_initpb(struct PCIMC_PB *mcpb)
{
    unsigned int i;
    int saved_cfg04;
    int saved_cfg10;
    int saved_modcfg, saved_upper16;

    /* mca0 */
    sprintf(mcpb->short_name,  "mc%c0", mcpb->pb.controller + 'a');

    fix_mc_names(mcpb);

    /* debug */
    dprintf("Entering mc_initpb %s\n", mcpb->short_name);
   
    /* Save where console carved up the Pci Address space on our behalf. */
    mcpb->DmemBase = 0xF8000000 & incfgl(mcpb, PCIMC_CONFIG10);

/* Host Bus to Pci Bus address translation for a 128mb MC Region:
/*
/* <33:32><31:30><29:27><26:00>
/*    |      |      |      |
/*    |      |      |      +- Relative to zero 128mb offset; mcpb->MCXmtRegion
/*    |      |      +-------- Config10 Base Register; Probe_io()
/*    |      +--------------- T2; HAEO_1<1:0>; 1 of 4 1gb pci adrs regions
/*    +---------------------- EvAdrs; T2 decoded; Pci Dense Memory region
/*
*/

    /* Create the Pci CSR Memory Mapped CSRs */
    mcpb->lcsr = (unsigned int) mcpb->DmemBase + PCIMC_LCSR;
    mcpb->prbar = (unsigned int) mcpb->DmemBase + PCIMC_PRBAR;

    /* Create the Back CSR Memory Mapped CSRs */
    mcpb->mcerr = (unsigned int) mcpb->DmemBase + PCIMC_MCERR;
    mcpb->mcport = (unsigned int) mcpb->DmemBase + PCIMC_MCPORT;
    mcpb->modcfg = (unsigned int) mcpb->DmemBase + PCIMC_MODCFG;
    mcpb->pos = (unsigned int) mcpb->DmemBase + PCIMC_POS;

    /* Specify csr read and write routines */ 
    mcpb->csr_read = indmeml;   /* to be updated for MC2 sparse mem reads */
    mcpb->csr_write = outdmeml; /* to be updated for MC2 sparse mem writes */

    /* Fill in the info field to include the rev of the adapter */
    sprintf(mcpb->pb.info,"Rev: %x, %s", 
      (incfgl(mcpb, PCIMC_CONFIG08) & Cfg08h$M_RevID), mcpb->short_name);

    /* MC1 / MC2 determination */
    if ((incfgl(mcpb, PCIMC_CONFIG08) & (Cfg08h$M_RevID & 0x0000007F)) >= 0x20)
    	mcpb->pcimc_mc2_mode = 1; 
    else
    	mcpb->pcimc_mc2_mode = 0;

    /* Specify page size */
    if (mcpb->pcimc_mc2_mode) {  /* if mc2, set page_size according to hw */
	if (MODCFG_READ(mcpb) & MODCFG$V_PageSize) 
	   mcpb->page_size = SIZE$K_8K;	   
        else 
	   mcpb->page_size = SIZE$K_4K;
    }
    else /* else mc1 (and page_size can only be 8K) */
       mcpb->page_size = SIZE$K_8K;    

    /* Fill in Page size offset used to determine address offset on fly. 
       This is used only in certain code that requires special pages that 
       are hardcoded to 8KB.  In these cases, the address of the 8kb page is
       the base and this PgOffset is the offset of the next 8KB page in 
       terms of whatever page size is used here */
    mcpb->PgOffset = SIZE$K_8K/mcpb->page_size;

    /* If MC2 - set TurboLaser Mode Bit in modcfg reg accordingly */
    if (mcpb->pcimc_mc2_mode) {

	/* save current value of the MODCFG register. First save the upper 16
	   bits, as of yet undefinded.  Then, copy current state settings for 
	   window and page size into their respective override positions to
	   prepare for config override condition (ORed with upper 16 bits).
	   Turbo override bit is cleared. 
	*/
        saved_upper16 = MODCFG_READ(mcpb) & 0xFFFF0000;
        saved_modcfg = (((MODCFG_READ(mcpb)) & 0x6) << 2) | saved_upper16;

        /* if Turbo, set Turbo Mode Override bit*/
#if TURBO
	saved_modcfg |= MODCFG$V_TurboModOvride;
#endif
	MODCFG_WRITE(mcpb, MODCFG$V_CfgJmprOveride | saved_modcfg);	
    }

    return msg_success;
}
