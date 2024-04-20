/* file:	pci_nvram_driver.c
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
 *  ABSTRACT:	PCI NVRAM driver.
 *
 *  AUTHORS:
 *
 *	Harold Buckingham
 *
 *  CREATION DATE:
 *  
 *      29-Jun-1994
 *
 *  MODIFICATION HISTORY:
 *
 *	dtr	17-mar-1995	base_addr1 to base_addr0, base_addr2 to
 *				base_addr1
 *
 *	hcb	15-Dec-1994	Add check for battery insulator installed.
 *
 *	hcb	25-Oct-1994	Access all CSRs through cfg space.  This allows
 *				the memory based CSRs to be placed in either
 *				sparce or dense space without this code caring.
 *
 *	jrh	31-Aug-1994	Support added for all APS platforms ... Window base
 *                              of 0x40000000 rather than assuming 0
 *
 *	hcb	16-Aug-1994	turn off EDC reporting when reading cookie
 *				and in first pass of sram test loop.
 *
 *	hcb	01-Aug-1994	Changed access to quadwords. Added sleep to
 *				DMA loop. 
 *
 *	hcb	11-Jul-1994	Fill in DMA test
 *
 *	hcb	29-Jun-1994	Initial entry.
 *--
 */

/*Include files*/

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$src:kernel_def.h"			/*Kernel definitions*/
#include "cp$src:msg_def.h"			/*Message definitions*/
#include "cp$src:pb_def.h"			/*Port block definitions*/
#include "cp$src:pci_nvram_pb_def.h"
#include "cp$src:mem_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$inc:platform_io.h"
#include "cp$inc:prototypes.h"

/*External routine definitions*/

#if RAWHIDE
struct PCI_NVRAM_PB nvr_pb;
extern struct window_head config;
#endif

/*Routine definitions*/
int pci_nvram_config();

/*Globals*/
#define DEBUG 0

/*+
 * ============================================================================
 * = pci_nvram_init - Initialization routine for the port driver                     =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routines will initialize the pci nvram module.
 *
 * FORM OF CALL:
 *
 *	pci_nvram_init (); 
 *
 * RETURNS:
 *
 *	int Status - msg_success or FALIURE.
 *
 * ARGUMENTS:
 *
 *      None
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

int pci_nvram_init () {

#if RAWHIDE
    pci_nvram_config( pci_nvram_get_pb() );
#else
    find_pb("pci_nvram", sizeof(struct PCI_NVRAM_PB), pci_nvram_config);
#endif

    return (msg_success);
}

int pci_nvram_config( struct PCI_NVRAM_PB *pb ) { 
#if RAWHIDE
    struct device *dev;
#endif
    unsigned int wdata;
    unsigned int temp, size, i;

#if DEBUG
    printf("start pci_nvram_config\n");
#endif

#if DEBUG || SABLE || RAWHIDE
    qprintf("Testing PCI NVRAM module\n");
#endif

#if DEBUG
    printf("set up addr and size\n");
#endif

    /* Read base address register 1 to get base address of memory */
    pb->nvram_mem_base = (incfgl(pb, BASE_ADDR0) & 0xFFFFFFF0);

    /* Get the size of the NVRAM memory */

    temp = incfgl(pb, MEM_CONFIG);

    switch((temp & CSRA_SRAM_SIZE)>>2) {
	case 0:
		size = 1;
		break;
	case 1:
		size = 4;
		break;
	case 2:
		size = 16;
		break;
	default:
		size = 1;
	}

    pb->nvram_size = (1<<(temp & CSRA_MEM_BANKS))*size*0x80000;

#if DEBUG
    printf("pb->nvram_mem_base: %x\n",pb->nvram_mem_base);
    printf("pb->nvram_size:     %x\n",pb->nvram_size);
    printf("pb->pb.csr:         %x\n",pb->pb.csr);

    for (i=0; i<6; i++)
	printf("%x = %08x\n", (i*4), incfgl(pb, (i*4)));
#endif

    /* This is setup by sable_io */
    /* Set the Enable Memory Space bit in CSR 1 */
    wdata = (incfgl(pb, DEV_CSR) | CSR1_ENA_MEM);
    outcfgl(pb, DEV_CSR, wdata);

    /* turn off EDC reporting in case there is bad EDC in the Cookie */
    /* location. */
    outcfgl(pb, EDC_CTRL, (incfgl(pb, EDC_CTRL) | CSRB_DISA_EDC_RPT));

    /* Check the magic cookie */
    /* if the magic cookie is set then just return */
    /* Later we might add a little testing */
    if (pci_nvram_check_cookie(pb) == msg_failure)
    {
	/* turn the EDC reporting back on */
	outcfgl(pb, EDC_CTRL, (incfgl(pb, EDC_CTRL) & ~CSRB_DISA_EDC_RPT));
	err_printf(msg_cmm_wrn_presto_valid);
	return(msg_success);
    }

    /* Setup CSR1 */
    /* Set the the parity error response and the serr# driver */
    /* Check with OSF Do they want me to do this???? */
    wdata = (incfgl(pb, DEV_CSR) | (CSR1_ENA_PAR_ERR | CSR1_ENA_SER_DRIVR));
    outcfgl(pb, DEV_CSR, wdata);

    /* Setup CSR3 */
    /* set the cache line size and the latency timer value */
    outcfgl(pb, MISC_FUNC1, 0x808);

    /* Run the diagnostics */
    if (pci_nvram_diag_tests(pb) == msg_success)
	pb->diag_status = DIAG_PASS;
    else
	pb->diag_status = DIAG_FAIL;

#if RAWHIDE
    dev = find_dev("pci_nvram0", &config);
    qprintf("PCI NVRAM tests ");
    if ( pb->diag_status == DIAG_PASS ) {
	qprintf("passed\n");
	dev->flags = 0;
    } else {
	qprintf("failed\n");
	dev->flags = 1;
    }
#endif

#if DEBUG
    printf("diag_status: %x \n",pb->diag_status);
#endif

    /* write the diagnostic status to NVRAM location 0x3f8 */
    outdmeml(pb, pb->nvram_mem_base|DIAG_REG_OFFSET, pb->diag_status);

#if DEBUG
    for (i=0; i<6; i++)
	printf("%x = %08x\n", (i*4), incfgl(pb, (i*4)));
#endif

#if DEBUG
    printf("end pci_nvram_config\n");
#endif

}

int pci_nvram_check_cookie(struct PCI_NVRAM_PB *pb) {

    unsigned int cookie_value;

    cookie_value = indmeml(pb, pb->nvram_mem_base | COOKIE_OFFSET);

#if DEBUG
    printf("cookie_value = %x\n",cookie_value);
#endif

    if (cookie_value == COOKIE_VALUE)
	return (msg_failure);
    else
	return(msg_success);
} 
int pci_nvram_zero_sram(struct PCI_NVRAM_PB *pb) {

    unsigned __int64 *addr;
    unsigned int i, loop_len;

#if DEBUG
    printf("start pci_nvram_zero_sram\n");
#endif

    addr = pb->nvram_mem_base;

    loop_len = pb->nvram_size>>3;
#if DEBUG
    printf("loop_len = %x\n",loop_len);
#endif

    for (i=0; i<loop_len; i++) {
	outdmemq(pb, addr, 0);
#if DEBUG
	if ((i < 10) || (i > (loop_len-10)))	
	    printf("addr = %08x\n",addr);
#endif
	addr++;
    }
#if DEBUG
    printf("end pci_nvram_zero_sram\n");
#endif
    return (msg_success);
}


/* These are diags below here.  They may get moved to another file later */
int pci_nvram_diag_tests(struct PCI_NVRAM_PB *pb) {

    /* test the battery */
    if (pci_nvram_battery_diag(pb) == msg_failure)
	return(msg_failure);

    /* test the SRAMs */
    if (pci_nvram_sram_diag(pb) == msg_failure)
	return(msg_failure);

    /* test the DMA */

    if (pci_nvram_dma_diag(pb) == msg_failure)
	return(msg_failure);

    /* zero the SRAMs */
    if (pci_nvram_zero_sram(pb) == msg_failure)
	return(msg_failure);

    return (msg_success);
}

int pci_nvram_battery_diag(struct PCI_NVRAM_PB *pb) {

    int i, j;

#if DEBUG
    pprintf("1 incfgl(pb, BATT_CSR) = %x\n",incfgl(pb, BATT_CSR));
#endif

    /* Check the BF flag the bit should be set if the battery is good */
    if (!incfgl(pb, BATT_CSR) & CSRD_BATT_FAIL) {

#if DEBUG
	printf("failure1 incfgl(pb, BATT_CSR) = %x\n",
			incfgl(pb, BATT_CSR));
#endif
	return(msg_failure);
    }

    /* Check to see if the battery insulator is installed */
    /* or if the battery is missing */ 
#if DEBUG
    pprintf("2 incfgl(pb, BATT_CSR) = %x\n",incfgl(pb, BATT_CSR));
#endif
    if (incfgl(pb, BATT_CSR) & CSRD_BATT_CHRG) {
	outcfgl(pb, BATT_CSR, (incfgl(pb, BATT_CSR) | CSRD_DIAG_MODE1));
#if DEBUG
	pprintf("3 incfgl(pb, BATT_CSR) = %x\n",incfgl(pb, BATT_CSR));
#endif
	if (!(incfgl(pb, BATT_CSR) & CSRD_BATT_OK)) {
#if DEBUG
    pprintf("4 incfgl(pb, BATT_CSR) = %x\n",incfgl(pb, BATT_CSR));
#endif
	    err_printf("\n*** Error PCI NVRAM - Battery Test ***\n");
	    err_printf("Battery missing or Insulator installed\n\n");
	    /* clear bit5 */
	    outcfgl(pb, BATT_CSR, (incfgl(pb, BATT_CSR) & ~CSRD_DIAG_MODE1));
#if DEBUG
    pprintf("5 incfgl(pb, BATT_CSR) = %x\n",incfgl(pb, BATT_CSR));
#endif
	    return(msg_failure);
	}
	/* clear bit5 */
	outcfgl(pb, BATT_CSR, (incfgl(pb, BATT_CSR) & ~CSRD_DIAG_MODE1));
#if DEBUG
    pprintf("6 incfgl(pb, BATT_CSR) = %x\n",incfgl(pb, BATT_CSR));
#endif
    }
#if DEBUG
    pprintf("7 incfgl(pb, BATT_CSR) = %x\n",incfgl(pb, BATT_CSR));
#endif

    /* Test the battery disconnect circuit. Turn it off, Turn it on and */
    /* repeat */
    for (j=0; j<2; j++) {

	/* Disable the battery disconnect circuitry.  This allows the battery */
	/* to be used to backup the NVRAM during a powerfail. */
	outcfgl(pb, BATT_CSR, 0);
#if DEBUG
	printf("j = %x\n",j);
#endif

	/* Check the BDCONN flag the bit should be clear */
	if (incfgl(pb, BATT_CSR) & CSRD_BATT_DISC) {
#if DEBUG
	    printf("failure2 incfgl(pb, BATT_CSR) = %x\n",
			incfgl(pb, BATT_CSR));
#endif
	    return(msg_failure);
	}

	/* Enable the battery disconnect circuitry.  This will prohibit the */
	/* battery from backing up the NVRAm on a powerfail. */
	for (i=0; i<5; i++) {
	    outcfgl(pb, BATT_CSR,(((PRESTO_BATT_ENAB>>i) & 1) << 1));

#if DEBUG
    	    printf("(PRESTO_BATT_ENAB>>i)&1 = %x\n",(PRESTO_BATT_ENAB>>i) & 1);
    	    printf("(PRESTO_BATT_ENAB>>i)&1<<1 = %x\n",
			((PRESTO_BATT_ENAB>>i) & 1)<<1);
	    printf("incfgl(pb, BATT_CSR) = %x\n",
			incfgl(pb, BATT_CSR));
#endif
	}

	/* Check the BDCONN flag the bit should be set */
	if (!(incfgl(pb, BATT_CSR) & CSRD_BATT_DISC)) {
#if DEBUG
	    printf("failure3 incfgl(pb, BATT_CSR) = %x\n",
			incfgl(pb, BATT_CSR));
#endif
	    return(msg_failure);
	}
    }
    return(msg_success);
}

#define NUM_TEST_DATA 2
int pci_nvram_sram_diag(struct PCI_NVRAM_PB *pb) {

    unsigned __int64 *start_addr, *addr;
    unsigned __int64 read_data, write_data;
    unsigned __int64 test_data[NUM_TEST_DATA+2] = { 0xaaaaaaaa,
						    0x55555555,
						    0xffffffff,
						    0x00000000 };
    unsigned int i, j, loop_len;

#if DEBUG
    printf("start pci_nvram_sram_diag\n");
#endif

    start_addr = (unsigned int*)((int)pb->nvram_mem_base);

    loop_len = pb->nvram_size>>3;
#if DEBUG
    printf("start_addr = %x\n",start_addr);
    printf("loop_len = %x\n",loop_len);
#endif

    for (j=0; j<NUM_TEST_DATA; j++) {
	addr = start_addr;
	write_data = (test_data[j] | (test_data[j]<<32));
 
	/* first loop - write the data */
	for (i=0; i<loop_len; i++) {
	    /* write the data */
	    outdmemq(pb, addr, write_data);
#if DEBUG
	    if ((i < 10) || (i > (loop_len-10)))	
		printf("%08x = %016x\n",addr, write_data);
#endif
	    addr++;
	}

	/* turn the EDC reporting back on the first pass only. */
	if (!j)
	    outcfgl(pb, EDC_CTRL, (incfgl(pb, EDC_CTRL) & ~CSRB_DISA_EDC_RPT));

	addr = start_addr;
	/* second loop read and compare the data */
	for (i=0; i<loop_len; i++) {
	    /* read the data */
	    read_data = indmemq(pb, addr);
#if DEBUG
	    if ((i < 10) || (i > (loop_len-10)))	
		printf("%08x = %016x\n",addr, read_data);
#endif
	    if (read_data != write_data) {
		err_printf("\n*** Error PCI NVRAM - Memory Test ***\n");
		err_printf("PCI memory address %08x\n", addr);
		err_printf("expected data       %016x\n", write_data);
		err_printf("received data       %016x\n\n", read_data);
		return (msg_failure);
	    }
	    addr++;
	}
    }
#if DEBUG
    printf("end pci_nvram_sram_diag\n");
#endif
    return (msg_success);
}

#define DMA_BUFF_SIZE (64 * 1024) - 8
#define DMA_QW_SIZE (DMA_BUFF_SIZE>>3)

int pci_nvram_dma_diag(struct PCI_NVRAM_PB *pb) {

    unsigned __int64 *main_mem_buff;
    volatile unsigned __int64 read_data;
    unsigned __int64 write_data = 0xffffffff;
    unsigned int nvram_buff, master_addr, slave_addr;
    int	i;

#if DEBUG
    printf("start pci_nvram_dma_diag\n");
#endif

    main_mem_buff = (unsigned __int64 *)dyn$_malloc(DMA_BUFF_SIZE, 
					DYN$K_SYNC|DYN$K_FLOOD);
    slave_addr = (unsigned int)main_mem_buff | io_get_window_base(pb);

    nvram_buff = pb->nvram_mem_base;
    master_addr = 0;

    write_data = (write_data | (write_data<<32));

#if DEBUG
    printf("main_mem_buff = %08x\n",main_mem_buff);
    printf("DMA_BUFF_SIZE = %08x\n",DMA_BUFF_SIZE);
    printf("DMA_QW_SIZE   = %08x\n",DMA_QW_SIZE);
    printf("slave_addr    = %08x\n",slave_addr);
    printf("nvram_buff    = %08x\n",nvram_buff);
    printf("master_addr   = %08x\n",master_addr);
#endif
	
    /* setup the DMA Slave Address register */
    outcfgl(pb, DMA_SLAVE_ADDR, slave_addr);

    /* setup the DMA Master Address register */
    outcfgl(pb, DMA_MASTR_ADDR, master_addr);

    /* setup the byte count register */
    outcfgl(pb, DMA_BYTE_CNT, DMA_BUFF_SIZE);

    /* Initialize NVRAM memory with A's pattern */
    for (i=0; i<DMA_QW_SIZE; i++)
	outdmemq(pb, nvram_buff+(i*8), write_data);

    /* Write to the DMA Command Register with DMA command which specifies */
    /* direction of data. Once this register is loaded, the DMA will be */
    /* started. First, write to Host memory. */
    outcfgl(pb, DMA_COMMAND, CSR13_WRT_TO_SLAVE);

    /* wait for the DMA to finish */
    /* wait for an interrupt????? */
    /* for now we'll just spin on the byte counter */
    i = 100;
    while (--i) {
	if (!(incfgl(pb, DMA_BYTE_CNT)))
	    break;
	krn$_sleep(100);
    }

#if DEBUG
    printf("dma countdown was %d\n",i);
#endif

    if (!i) {
	err_printf("\n*** Error PCI NVRAM - DMA Test ***\n");
	err_printf("DMA transfer timed out\n\n");
	return(msg_failure);
    }

    /* Check the data */
    for (i=0; i<DMA_QW_SIZE; i++) {
	read_data = *(main_mem_buff + i);

#if DEBUG
/*    if ((i < 10) || (i > (DMA_QW_SIZE-10))) */
	if ((i < 10) || (i > 0x3ff0))
	    printf("%08x: %016x\n",(main_mem_buff+i),read_data);
#endif

	if (read_data != write_data) {
	    err_printf("\n*** Error PCI NVRAM - DMA Test ***\n");
	    err_printf("Main memory address %08x\n", main_mem_buff+i);
	    err_printf("expected data      %016x\n",   write_data);
	    err_printf("received data      %016x\n\n", read_data);
	    return (msg_failure);
	}
    }

#if DEBUG
    printf("zero the nvram SRAM\n");
#endif

    /* Zero out NVRAM memory used in DMA transfer */
    for (i=0; i<DMA_QW_SIZE; i++)
	outdmemq(pb, nvram_buff+(i*8), 0);

    /* setup the DMA Slave Address register */
    outcfgl(pb, DMA_SLAVE_ADDR, slave_addr);

    /* setup the DMA Master Address register */
    outcfgl(pb, DMA_MASTR_ADDR, master_addr);

    /* setup the byte count register */
    outcfgl(pb, DMA_BYTE_CNT, DMA_BUFF_SIZE);

    /* Load the DMA Command Register with the Read From Slave command. */
    /* Once this register is loaded, the DMA will be started. */
    outcfgl(pb, DMA_COMMAND, CSR13_RD_FROM_SLAVE);

    /* wait for the DMA to finish */
    /* wait for an interrupt????? */
    /* for now we'll just spin on the byte counter */
    i = 100;
    while (--i) {
	if (!(incfgl(pb, DMA_BYTE_CNT)))
	    break;
	krn$_sleep(100);
    }

#if DEBUG
    printf("dma countdown was %d\n",i);
#endif

    if (!i) {
	err_printf("\n*** Error PCI NVRAM - DMA Test ***\n");
	err_printf("DMA transfer timed out\n\n");
	return(msg_failure);
    }

    /* Check the data */
    /* Read locations in NVRAM memory to verify DMA transfer occured */
    for (i=0; i<DMA_QW_SIZE; i++) {
	read_data = indmemq(pb, nvram_buff+(i*8));

#if DEBUG
/*	if ((i < 10) || (i > (DMA_QW_SIZE-10))) */
	if ((i < 10) || (i > 0x3ff0))
	    printf("%08x: %016x\n",(nvram_buff+(i*4)),read_data);
#endif

	if (read_data != write_data) {
	    err_printf("\n*** Error PCI NVRAM - DMA Test ***\n");
	    err_printf("PCI memory address %08x\n", nvram_buff+(i*4));
	    err_printf("expected data      %016x\n",   write_data);
	    err_printf("received data      %016x\n\n", read_data);
	    return (msg_failure);
	}
    }

    /* Clear the DMA CSRs */
    outcfgl(pb, DMA_MASTR_ADDR, 0);
    outcfgl(pb, DMA_SLAVE_ADDR, 0);

#if DEBUG
    printf("end pci_nvram_dma_diag\n");
#endif

    return (msg_success);
}

#if RAWHIDE
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

