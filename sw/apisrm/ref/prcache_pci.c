/* file:	prcache_pci.c
 *
 * Copyright (C) 1994 by
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
 */

/*
 *++
 *  FACILITY:
 *
 *      Sable Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *	Commands to support the PCI NVRAM disk cache.
 *
 *  AUTHORS:
 *
 *      Harold Buckingham
 *
 *  CREATION DATE:
 *  
 *      25-May-1994
 *
 *  MODIFICATION HISTORY:
 *
 *	hcb	25-Oct-1994	Access all CSRs through cfg space.  This allows
 *				the memory based CSRs to be placed in either
 *				sparce or dense space without this code caring.
 *
 *      hcb     05-Oct-1994	Only look at DIAG_FAIL bit
 *
 *      hcb     01-Aug-1994	Use quadwords for zeroing the NVRAM
 *
 *      hcb     22-Jul-1994	Debug on real hardware in Sable
 *
 *      hcb     25-May-1994	Initial entry.
 *
 *--
 */


#include "cp$src:platform.h"
#include "cp$src:common.h"			/*Kernel definitions*/
#include "cp$inc:prototypes.h"
#include "cp$src:kernel_def.h"			/*Kernel definitions*/
#include "cp$src:msg_def.h"			/*Message definitions*/
#include "cp$src:pb_def.h"			/*Port block definitions*/
#include "cp$src:pci_nvram_pb_def.h"
#include "cp$inc:kernel_entry.h"

#if TURBO
#include "cp$src:mem_def.h"
#include "cp$inc:platform_io.h"
struct PCI_NVRAM_PB nvr_pb;
#endif

#define DEBUG 0

/*External routine definitions*/

/*+
 * ============================================================================
 * = pci_nvram_find - Determine if a PCI NVRAM card is installed            =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Determine if a PCI NVRAM crad is installed  
 *
 * FORM OF CALL:
 *
 *      pci_nvram_find();
 *
 * RETURN CODES:
 *
 *   	msg_success  - found a PCI NVRAM card
 *
 *   	msg_failure  - did not find a PCI NVRAM card
 *
 * ARGUMENTS:
 *
 *	None
 *
 * SIDE EFFECTS:
 *
 *  	None
 *
-*/
int pci_nvram_find() {

    struct PCI_NVRAM_PB *pb;

#if DEBUG
    pprintf("pci_nvram_find\n");
#endif
    /* See if an NVRAM Disk cache board is installed */
    pb = pci_nvram_get_pb();

    if (pb == NULL)
	return(msg_failure);

    return(msg_success);
}

/*+
 * ============================================================================
 * = pci_nvram_configured - Determine if a PCI NVRAM module is configured   =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Determine if a PCI NVRAM module is configured
 *
 * FORM OF CALL:
 *
 *      pci_nvram_configured();
 *
 * RETURN CODES:
 *
 *   	msg_success  - The PCI NVRAM module is configured
 *
 *   	msg_failure  - The PCI NVRAM module is not configured
 *
 * ARGUMENTS:
 *
 *	None
 *
 * SIDE EFFECTS:
 *
 *  	None
 *
-*/
int pci_nvram_configured() {

#if DEBUG
    pprintf("pci_nvram_configured\n");
#endif

    return(pci_nvram_find());

    return(msg_success);
}

/*+
 * ============================================================================
 * = pci_nvram_test_status - Determine the test status of a PCI NVRAM module=
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Determine the powerup diagnostic status of a PCI NVRAM module
 *
 * FORM OF CALL:
 *
 *      pci_nvram_test_status();
 *
 * RETURN CODES:
 *
 *   	msg_success  - The PCI NVRAM module passed powerup diagnostics
 *
 *   	msg_failure  - The PCI NVRAM module failed powerup diagnostics
 *
 * ARGUMENTS:
 *
 *	None
 *
 * SIDE EFFECTS:
 *
 *  	None
 *                   
-*/
int pci_nvram_test_status() {

    struct PCI_NVRAM_PB *pb;

#if DEBUG
    pprintf("pci_nvram_test_status\n");
#endif

    /* get the port block */
    pb = pci_nvram_get_pb();

    /* Read base address register 1 to get base address of memory */
    pb->nvram_mem_base = incfgl(pb, BASE_ADDR0) & ~15;

    /* write the disgnostic status to NVRAM location 0x3f8 */
    if ((indmeml(pb, pb->nvram_mem_base|DIAG_REG_OFFSET)) & DIAG_FAIL)
	return(msg_failure);

    return(msg_success);

}

/*+
 * ============================================================================
 * = pci_nvram_zero - Zero the contents of a PCI NVRAM module               =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Zero the contents of the a PCI NVRAM module.
 *
 * FORM OF CALL:
 *
 *      pci_nvram_zero();
 *
 * RETURN CODES:
 *
 *   	msg_success  - the PCI NVRAM module contents was zeroed
 *
 * ARGUMENTS:
 *
 *	None
 *
 * SIDE EFFECTS:
 *
 *  	None
 *
-*/
int pci_nvram_zero() {

    unsigned __int64 *addr;
    struct PCI_NVRAM_PB *pb;
    unsigned int i, loop_len;

#if DEBUG
    pprintf("start pci_nvram_zero\n");
#endif

    /* get the port block */
    pb = pci_nvram_get_pb();

    addr = pb->nvram_mem_base;

    loop_len = pb->nvram_size>>3;
#if DEBUG
    pprintf("loop_len = %x\n",loop_len);
#endif

    for (i=0; i<loop_len; i++) {
	outdmemq(pb, addr, 0);
#if DEBUG
	if ((i < 10) || (i > (loop_len-10)))	
	    pprintf("addr = %x\n",addr);
#endif
	addr++;
    }
#if DEBUG
    pprintf("end pci_nvram_zero\n");
#endif

    return(msg_success);
}

/*+
 * ============================================================================
 * = pci_nvram_bat_enable - Enable the battery of the PCI NVRAM module      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	The battery disconnect circuit on the PCI NVRAM module is disabled.
 *	This allows the battery to back up the RAM in the event of a
 *	powerup failure.
 *
 * FORM OF CALL:
 *
 *      pci_nvram_bat_enable();
 *
 * RETURN CODES:
 *
 *   	msg_success  - The battery on the PCI NVRAM module is enabled
 *
 *
 * ARGUMENTS:
 *
 *	None
 *
 * SIDE EFFECTS:
 *
 *  	None
 *
-*/
int pci_nvram_bat_enable() {

    struct PCI_NVRAM_PB *pb;

#if DEBUG
    pprintf("pci_nvram_bat_enable\n");
#endif

    /* get the port block */
    pb = pci_nvram_get_pb();

    /* Disable the battery disconnect circuitry.  This allows the battery */
    /* to be used to backup the NVRAM during a powerfail. */
    outcfgl(pb, BATT_CSR, 0);

    return(msg_success);
}

/*+
 * ============================================================================
 * = pci_nvram_disp_status - Display status of the PCI NVRAM module         =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	The complete status of the PCI NVRAM module is displayed
 *
 * FORM OF CALL:
 *
 *      pci_nvram_disp_status();
 *
 * RETURN CODES:
 *
 *   	msg_success  - status of the PCI NVRAM module was displayed
 *
 * ARGUMENTS:
 *
 *	None
 *
 * SIDE EFFECTS:
 *
 *  	None
 *
-*/
int pci_nvram_disp_status() {

    struct PCI_NVRAM_PB *pb;
    unsigned int ssn;
    char dev_bus[4];

#if DEBUG
    pprintf("pci_nvram_diap_status\n");
#endif

    strcpy(dev_bus,"PCI");

    /* get the port block */
    pb = pci_nvram_get_pb();

    printf("%s NVRAM Disk Cache: ",dev_bus);
    if ((indmeml(pb, pb->nvram_mem_base|DIAG_REG_OFFSET)) & DIAG_FAIL)
	printf("failed\n");
    else         
	printf("passed\n");

    printf("Size: %3dMB\n", pb->nvram_size/0x100000);
    printf("%s Memory Address: %08x\n", dev_bus,pb->nvram_mem_base);

    printf("System ID: ");
    if (ssn = trans_presto_ssn())
	printf("%08x\n", ssn);
    else
	printf("??????\n");

    printf("State: ");
    if (indmeml(pb, pb->nvram_mem_base|COOKIE_OFFSET) == COOKIE_VALUE)
	printf(msg_presto_valid);
    else
	printf(msg_presto_not_valid);

    printf("Battery Status: ");
    if (!incfgl(pb, BATT_CSR) & CSRD_BATT_FAIL)
	printf("failed\n");
    else {
	printf("good ");
	if (incfgl(pb, BATT_CSR) & CSRD_BATT_CHRG)
	    printf("(Charged)\n");
	else
	    printf("(Charging)\n");
    }
    printf("Battery Disconnect Circuit Status: ");
    if (incfgl(pb, BATT_CSR) & CSRD_BATT_DISC)
	printf("enabled\n");
    else
	printf("disabled\n");

    return(msg_success);
}


#if !(TURBO || RAWHIDE)
int pci_nvram_get_pb() {
    return((struct PCI_NVRAM_PB *)
		find_pb("pci_nvram", sizeof(struct PCI_NVRAM_PB), 0));
}
#endif

#if TURBO
int pci_nvram_get_pb() {  
    extern struct window_head config;
    struct device *nvr_dev;
    int nvcnt;

    nvcnt = find_all_dev(&config, PCI$K_DTYPE_DEC_ZEPHYR, 1, &nvr_dev, 
                                                              FAD$K_ALLDEV);
#if 0
    pprintf("Found %d nvrams\n",nvcnt);
#endif

    if (nvcnt == 0) return 0;

    /* get the hose and slot */
    nvr_pb.pb.hose = nvr_dev->hose;
    nvr_pb.pb.slot = nvr_dev->slot;

    /* Read base address register 1 to get base address of memory */
    nvr_pb.nvram_mem_base = (incfgl(&nvr_pb.pb, 0x10) & 0xFFFFFFF0);

    /* Write to base address register 1 with all 1's.  
     * Writes to 0 bits will have no effect. Bits will be set to indicate 
     * the capacity of NVRAM memory. 
     */
    outcfgl(&nvr_pb.pb, 0x10, 0xFFFFFFFF);

    /* Read base address register 1 to get size of memory */
    nvr_pb.nvram_size = (~(incfgl(&nvr_pb.pb, 0x10) & 0xFFFFFFF0)+1); 

    /* Restore Base Address 1 */
    outcfgl(&nvr_pb.pb, 0x10, nvr_pb.nvram_mem_base);

#if 0
    pprintf("nvr_pb.pb.hose : %d\n", nvr_pb.pb.hose);
    pprintf("nvr_pb.pb.slot : %d\n", nvr_pb.pb.slot);
    pprintf("nvr_pb.nvram_size : %x\n", nvr_pb.nvram_size);
    pprintf("nvr_pb.nvram_mem_base : %x\n", nvr_pb.nvram_mem_base);
#endif

    return((struct PCI_NVRAM_PB *) &nvr_pb);
}       
#endif


