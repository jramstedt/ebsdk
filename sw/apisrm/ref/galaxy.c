/* file:	galaxy.c
 *
 * Copyright (C) 1992 by
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
 *      Alpha Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *	galaxy command
 *
 *  AUTHORS:
 *
 *      numerous
 *
 *  CREATION DATE:
 *  
 *      8-May-1997
 *
 *  MODIFICATION HISTORY:
 *
 *	dtm	8-May-1997	Initial Galaxy support on TurboLaser
 *--
 */
#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$src:ctype.h"
#include "cp$src:parse_def.h"
#include "cp$src:msg_def.h"
#include "cp$inc:kernel_entry.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:console_hal_macros.h"
#include "cp$src:ev_def.h"
#include "cp$src:impure_def.h"
#include "cp$src:pal_def.h"
#include "cp$src:hwrpb_def.h"
#include "cp$src:memory_chunk.h"
#if TURBO
#include "cp$src:mem_def.h"
#include "cp$src:tlsb.h"
#endif

#if CLIPPER
#include "cp$src:clipper_dpr_def.h"
#endif

#if RAWHIDE
#include "cp$src:platform_cpu.h"
#include "cp$inc:platform_io.h"

typedef unsigned __int64 UINT;	    /* kludge for rawhide build */
typedef __int64 INT;		    /* kludge for rawhide build */
#define	iop_mask iod_mask	    /* for galaxy on rh, iods are iops */

extern UINT memory_test_desc_table;
#endif

extern struct LOCK spl_kernel;
extern struct window_head config;
extern struct QUEUE pcbq;
extern int shell_startup();
extern UINT mem_size;
extern UINT memory_high_limit;
extern int mem_mask;
extern int cpu_mask;
extern int in_console;
extern int primary_cpu;
extern int timer_cpu;
extern int poll_pid;
extern int console_mode[MAX_PROCESSOR_ID];
extern struct TTPB *hwrpbtt_ttpbs[];
extern int galaxy_node_number;
extern int galaxy_partitions;
extern int HWRPB;

#if (TURBO || RAWHIDE)
#undef galaxy_check_boot
extern int hwrpbtt_inited;
extern int all_cpu_mask;
extern int iop_mask;
extern int lp_hard_partition;
extern u_int64 hf_buf_adr;
extern config_size;
#endif

#if (CLIPPER)
extern int all_cpu_mask;
#define iop_mask (0x3)
#endif

#define MIN_PRIVATE_SIZE 64*1024*1024
#define MIN_SHARED_SIZE 8*1024*1024

extern null_procedure();

start_helper(int *addr, int primary, int target);
extern migrate_cpu();
char *mask_to_id (int mask, char *s);
migrate_helper(int target);

#if TURBO
void galaxy_set_poll_affinity(int id);
#endif

#if RAWHIDE
void move_rawhide_bitmap(UINT galaxy_base_pa, UINT galaxy_mem_size);
#endif

/*+
 * ============================================================================
 * = lpinit - start up secondary partitions.                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Validates that the LP environment variables specify a valid
 *	partition configuration.  If so, each processor that belongs to a
 *	secondary partition will be started.  By definition, the "galaxy"
 *	needs to be start only once, and this command enforces that rule.
 *
 *	Command operation is determined by the setting of the following set
 *	of environment variables:
 *
 *	LP_COUNT - the total number of partitions 
 *	LP_CPU_MASKx - a bitmask of CPU whami IDs that belong to partition x
 *	LP_IO_MASKx - a bitmask of TIOP slot numbers that belong to partition x
 *	LP_MEM_SIZEx - the size of private memory dedicated to partition x
 *	LP_SHARED_MEM_SIZE - the size of shared memory to be reserved
 *	LP_MEM_MODE - set to 'isolate' for hard partitioning (TurboLaser only)
 *
 *	Note that only the LP_COUNT, LP_CPU_MASKx, and LP_IO_MASKx EVs need to
 *	be defined in order to start all partitions.  By default, memory will
 *	equally divided among all partitions, and no shared memory is reserved.
 *	If the LP_SHARED_MEM_SIZE EV is defined, that amount of memory is first
 *	reserved at the high end of physical memory, and the remaining available
 *	memory divided equally among partitions.  If any partition has its
 *	LP_MEM_SIZEx EV defined, that amount of memory is reserved for that
 *	partition.  Any remaining memory is divided equally among parition which
 *	have no explicit memory size defined.  Note that it is possible, by
 *	defining LP_MEM_SIZEx for all partitions, to have remaining, unallocated
 *	memory.  
 *
 *	Partition 0 private memory always begins at physical address 0. The
 *	succeeding partition private memory are allocated next.  Shared memory
 *	is allocated at the physical address that follows the last partition's
 *	private memory.  Unallocated memory, if any, follows at the highest
 *	physical memory addresses.  A data structure beginning at fixed address
 *	0x40 is built to pass the memory allocation information between
 *	partitions.
 *
 *	An undocumented debug feature of the command is the ability to start
 *	a single CPU, rather than the default of starting all CPUs in all
 *	secondary partitions, via the '-cpu' command qualifier.  Once specified,
 *	the remaining CPUs can be started individually via the 'migrate'
 *	command.
 *	
 *   COMMAND FMT: galaxy 2 BZ 8 lpinit
 *
 *   COMMAND FORM:
 *  
 *	lpinit ( )
 *  
 *   COMMAND TAG: galaxy 0 RXBZ lpinit
 *
 *   COMMAND ARGUMENT(S):
 *
 *	none
 *
 *   COMMAND OPTION(S):
 *
 *	none
 *
 *   COMMAND EXAMPLE(S):
 *~
 *      >>> lpinit
 *~
 *   COMMAND REFERENCES:
 *
 *	none
 *
 * FORM OF CALL:
 *  
 *	lpinit( argc, *argv[] )
 *  
 * RETURN CODES:
 *
 *	msg_success - success
 *       
 * ARGUMENTS:
 *
 *	int argc - number of command line arguments passed by the shell
 *	char *argv [] - array of pointers to arguments
 *
 * SIDE EFFECTS:
 *
 *      A V5.0 FRU Configuration Tree is built.
 *
-*/

/*+
 * ============================================================================
 * = galaxy - alias for lpinit.                                               =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	For historical purposes, the 'galaxy' command has been retained as
 *	an alias for the 'lpinit' command.  
 *
 *   COMMAND FMT: galaxy 2 BZ 8 galaxy
 *
 *   COMMAND FORM:
 *  
 *	galaxy ( )
 *  
 *   COMMAND TAG: galaxy 0 RXBZ galaxy
 *
 *   COMMAND ARGUMENT(S):
 *
 *	none
 *
 *   COMMAND OPTION(S):
 *
 *	none
 *
 *   COMMAND EXAMPLE(S):
 *~
 *      >>> galaxy
 *~
 *   COMMAND REFERENCES:
 *
 *	none
 *
 * FORM OF CALL:
 *  
 *	galaxy( argc, *argv[] )
 *  
 * RETURN CODES:
 *
 *	msg_success - success
 *       
 * ARGUMENTS:
 *
 *	int argc - number of command line arguments passed by the shell
 *	char *argv [] - array of pointers to arguments
 *
 * SIDE EFFECTS:
 *
 *      A V5.0 FRU Configuration Tree is built.
 *
-*/
#if (TURBO || RAWHIDE)
#define QQ	0			/* qualifier -q */
#define QCPU	1			/* qualifier -cpu */
#define QMAX	2
int galaxy (int argc, char **argv) {
    int id,pid;
    int *addr;
    struct EVNODE evn, *evp=&evn;
    int nodes;
    int bad_config = 0;
    int galaxy_io_mask[4],galaxy_cpu_mask[4];
    UINT galaxy_mem_size[8],galaxy_base_pa[8],gbase_addr=0;
    UINT shared_size=0;
    UINT specific_size=0;
    UINT remaining_mem;
    int specific_nodes = 0;
    int unspecified_nodes = 0;
    char evname[80];
    char tmp[80];
    int i,j,k;
    int cpus=0,iops=0;
    int noprompt = FALSE;
    struct QSTRUCT qual[QMAX];
    struct HWRPB *hwrpb;
    int cpu;
    int galaxy_init[4] = {0,0,0,0};
    int new_cpu_mask = cpu_mask;
    UINT tree;

#if TURBO
#define PROCESSOR_STEP	2
#else
#define PROCESSOR_STEP	1
#endif

    if (galaxy_node_number || (cpu_mask != all_cpu_mask)) {
	printf("Secondary partitions have already been started\n");
	return msg_failure;
    }

#if RAWHIDE
    if (ev_read("memory_test", &evp, 0) == msg_success)
	if (evp->value.string[0] != 'f') {
	    printf("Galaxy requires a full memory test to be performed.\n");
	    printf("Execute the command \"SET memory_test full\" at the console prompt\n");
	    printf("and reset the system.\n");
	    return msg_failure;
	}
#endif

    qscan(&argc, argv, "-", "q %xcpu", qual);
    if ((qual[QQ].present) || (argc == 0))
	noprompt = TRUE;

    if (ev_read("lp_count", &evp, 0) == msg_success) {
	nodes = atoi(evp->value.string);

	if (nodes != galaxy_partitions) {
	    printf("System must be reset after modifying LP_COUNT\n");
	    return msg_failure;
	}

	if (nodes > 0) {

#if TURBO
	    /* Check for memory partitioned on module basis (unix) */
	    if (ev_read("lp_mem_mode",&evp,0) == msg_success) {
		if (strcmp(evp->value.string,"isolate") == 0)
		    lp_hard_partition = 1;
	    }
	    if (lp_hard_partition) {
		int memcnt;
		int *devlist;
		struct device *dev;
		int sets;
		shared_size = 0;
		memcnt = find_all_dev(&config, TLSB$K_DTYPE_MIC, 0, 0, FAD$M_PASSED_ST);
		devlist = (int *) malloc(memcnt * sizeof(int *));
		find_all_dev(&config, TLSB$K_DTYPE_MIC, memcnt, devlist, FAD$M_PASSED_ST);
		for (i=0,sets=0;i<memcnt;i++) {
		    dev = (struct device *)devlist[i];
		    if (dev->devdep.mem.set > sets) sets = dev->devdep.mem.set;
		}
		if (memcnt < nodes) {
		    printf("Insufficient memory modules to partition system.\n");
		    bad_config = 1;
		}
		else if (sets < nodes) {
		    printf("Insufficient memory interleave sets to partition system.\n");
		    printf("Issue command \"set interleave none\" then reset system.\n");
		    bad_config = 1;
		}
		if (bad_config) return msg_failure;
		for (i=0;i<nodes;i++) galaxy_mem_size[i] = 0;
		if (sets > nodes) {
		    UINT set_size[8] = {0,0,0,0,0,0,0,0};
		    int current_partition;
		    for (i=0;i<memcnt;i++) {
			dev = (struct device *)devlist[i];
			set_size[dev->devdep.mem.set-1] += (UINT)dev->devdep.mem.size*1024*1024;
		    }
		    current_partition = 0;
		    for (i=0;i<sets-1;i++) {
			if (galaxy_mem_size[current_partition] && ((galaxy_mem_size[current_partition] + set_size[i]) >
			    (mem_size/nodes)) && (current_partition<(nodes-1)))
			    if (abs((INT)(galaxy_mem_size[current_partition] + set_size[i] - (mem_size/nodes))) >
				abs((INT)((mem_size/nodes) - galaxy_mem_size[current_partition]))) current_partition++;
			galaxy_mem_size[current_partition] += set_size[i];
		    }
		    /* Add the last module/set to the last partition */
		    galaxy_mem_size[nodes-1] += set_size[i];
		}
		else {
		    for (i=0;i<memcnt;i++) {
			dev = (struct device *)devlist[i];
			galaxy_mem_size[dev->devdep.mem.set-1] += (UINT)dev->devdep.mem.size*1024*1024;
		    }
		}
	    }

	    else 
#endif
	    {

	    /* Check for explicit shared memory size */
	    sprintf(evname,"lp_shared_mem_size");
	    if (ev_read(evname, &evp, 0) == msg_success) {
		shared_size = xtoi(evp->value.string);
		if ((shared_size != 0) && (shared_size < MIN_SHARED_SIZE)) {
		    printf("Shared memory must be at least %dMB in size\n",MIN_SHARED_SIZE/(1024*1024));
		    shared_size = MIN_SHARED_SIZE;
		    bad_config = 1;
		}
		if ((shared_size != 0) && (shared_size & (MIN_SHARED_SIZE-1))) {
		    printf("Shared memory alignment must be %dMB\n",MIN_SHARED_SIZE/(1024*1024));
		    shared_size &= ~(MIN_SHARED_SIZE-1);
		    bad_config = 1;
		}
	    }
	    else shared_size = 0;
	    /* Determine total of instance-specific memory sizes */
	    for (i=0;i<nodes;i++) {
		sprintf(evname,"lp_mem_size%d",i);
		if ((ev_read(evname, &evp, 0) == msg_success) &&
		    ((galaxy_mem_size[i] = xtoi(evp->value.string)) > 0)) {
		    if (galaxy_mem_size[i] < MIN_PRIVATE_SIZE) {
			printf("Partition %d: Private memory must be at least %dMB in size\n",i,MIN_PRIVATE_SIZE/(1024*1024));
			galaxy_mem_size[i] = MIN_PRIVATE_SIZE;
			bad_config = 1;
		    }
		    if ((galaxy_mem_size[i] & (MIN_PRIVATE_SIZE-1)) && (i<nodes-1)) {
			printf("Partition %d: Private memory granularity must be %dMB\n",i,MIN_PRIVATE_SIZE/(1024*1024));
			galaxy_mem_size[i] &= ~(MIN_PRIVATE_SIZE-1);
			bad_config = 1;
		    }
		    specific_nodes++;
		}
		else galaxy_mem_size[i] = 0;
		specific_size += galaxy_mem_size[i];
	    }
	    }

	    /* Divide remaining memory equally among remaining instances */
	    /* Also, read and convert the cpu and io masks */
	    remaining_mem = mem_size - specific_size - shared_size;
	    unspecified_nodes = nodes - specific_nodes;
	    for (i=0;i<nodes;i++) {
		if (!galaxy_mem_size[i]) {
		    if (i == nodes - 1) galaxy_mem_size[i] = remaining_mem;
		    else {
			galaxy_mem_size[i] = ((remaining_mem + ((unspecified_nodes*MIN_PRIVATE_SIZE)-1)) / 
			 unspecified_nodes) & ~(MIN_PRIVATE_SIZE-1);
			unspecified_nodes--;
		    }
		    if (galaxy_mem_size[i] > remaining_mem)
			galaxy_mem_size[i] = remaining_mem;
		    else remaining_mem -= galaxy_mem_size[i];
		}
		galaxy_base_pa[i] = gbase_addr;
		gbase_addr += galaxy_mem_size[i];
		if ((galaxy_base_pa[i] & (MIN_PRIVATE_SIZE-1))) {
		    printf("Partition %d: Private memory alignment must be %dMB\n",i,MIN_PRIVATE_SIZE/(1024*1024));
		    bad_config = 1;
		}
		sprintf(evname,"lp_io_mask%d",i);
		if (ev_read(evname, &evp, 0) == msg_success)
		    galaxy_io_mask[i] = xtoi(evp->value.string);
		else galaxy_io_mask[i] = 0;
		sprintf(evname,"lp_cpu_mask%d",i);
		if (ev_read(evname, &evp, 0) == msg_success)
		    galaxy_cpu_mask[i] = xtoi(evp->value.string);
		else galaxy_cpu_mask[i] = 0;
	    }

	    /* Merge all the CPU masks and IO masks */
	    for (i=0;i<nodes;i++) {
		if (galaxy_mem_size[i] < MIN_PRIVATE_SIZE) {
		    printf("Partition %d: private memory must be at least %dMB in size\n",i,MIN_PRIVATE_SIZE/(1024*1024));
		    galaxy_mem_size[i] = MIN_PRIVATE_SIZE;
		    bad_config = 1;
		}
		cpus |= galaxy_cpu_mask[i];
		iops |= galaxy_io_mask[i];
	    }

	    /* Check that all CPUs and TIOPs are used */
	    /*   but don't fail the config   */
	    j = (cpus ^ cpu_mask) & cpu_mask;
	    k = (iops ^ iop_mask) & iop_mask;
	    if (j) printf("CPU %s not configured in any partition\n",mask_to_id(j,tmp));
	    if (k) printf("TIOP %s not configured in any partition\n",mask_to_id(k,tmp));

	    /* Check that all defined CPUs and TIOPs exist */
	    j = cpus & ~cpu_mask;
	    k = iops & ~iop_mask;
	    if (j) printf("Non-existent CPU %s configured in a partition\n",mask_to_id(j,tmp));
	    if (k) printf("Non-existent TIOP %s configured in a partition\n",mask_to_id(k,tmp));
	    bad_config |= j | k;

	    for (i=0,k=0;i<nodes;i++) {
		for (j=0; j<MAX_PROCESSOR_ID; j += PROCESSOR_STEP)
		    if (galaxy_cpu_mask[i]>>j & 1) break;
		if (j >= MAX_PROCESSOR_ID) {
		    printf("No valid primary processor specified for partition %d\n",i);
		    bad_config = 1;
		}
		else {
		    printf("Partition %d: Primary CPU = %d\n",i,j);
		    if (i != 0) new_cpu_mask &= ~(1<<j);
		}
		for (j=i+1;j<nodes;j++) {
		    if ((i != j) && (galaxy_cpu_mask[i] & galaxy_cpu_mask[j])) {
			printf("CPU %s configured in multiple partitions\n",
				mask_to_id(galaxy_cpu_mask[i] & galaxy_cpu_mask[j],tmp));
			bad_config = 1;
		    }
		    if ((i != j) && (galaxy_io_mask[i] & galaxy_io_mask[j])) {
			printf("TIOP %s configured in multiple partitions\n",
				mask_to_id(galaxy_io_mask[i] & galaxy_io_mask[j],tmp));
			bad_config = 1;
		    }
		}
	    }

	    {
	    INT msize = 0;
	    for (i=0;i<nodes;i++) {
		int *m,*b;
		*(__int64 *)(0x40+(8*i)) = galaxy_base_pa[i];
		msize += galaxy_mem_size[i];
		b = &galaxy_base_pa[i];
		m = &galaxy_mem_size[i];
		printf("Partition %d: Memory Base = %x%08x   Size = %x%08x\n",
		    i,b[1],b[0],m[1],m[0]);
	    }
	    galaxy_base_pa[i] = msize;
	    *(__int64 *)0x40 = 0;
	    *(__int64 *)(0x40+(8*i)) = galaxy_base_pa[i];
	    *(__int64 *)(0x40+(8*(i+1))) = galaxy_base_pa[i] + shared_size;
	    *(__int64 *)(0x40+(8*(i+2))) = mem_size;
	    if ((msize < 0) || ((msize+shared_size) > mem_size)) {
		printf("Total partition memory sizes are greater than available memory\n");
		bad_config = 1;
	    }
	    else {
		int *m,*b;
		b = &msize;
		m = &shared_size;
		if (shared_size) 
		    printf("Shared Memory Base = %x%08x   Size = %x%08x\n",
			b[1],b[0],m[1],m[0]);
		else printf("No Shared Memory\n");
	    }
	    }
	} else {
	    printf("System not configured to run multiple partitions\n");
	    return msg_failure;	    /* not a galaxy */
	}
    }
    else {
	printf("System not configured to run multiple partitions\n");
	return msg_failure;
    }

    if (bad_config) {
	if (noprompt == TRUE)
	    tmp[0] = 'N';
	else
	    read_with_prompt("\nDo you want to attempt to boot secondary partitions anyway? (Y/<N>) ",80,tmp,0,0,1);
	if (toupper(tmp[0]) != 'Y') {
	    pprintf("Secondary partitions not started...\n");
	    return msg_failure;
	}
    }

#if (TURBO || RAWHIDE)
    ovly_load("GALAXY");		/* Make sure overlay stays resident */
    *(INT *)0x80 = tree = gct();

    if (HWRPB) {
	hwrpb = HWRPB;
	hwrpb->RSVD_HW[0] = *(INT *)0x80;
	hwrpb->FRU_OFFSET[0] = *(INT *)0x80 - 0x2000;
	twos_checksum(hwrpb, offsetof(struct HWRPB,CHKSUM)/4,
	    (UINT *)hwrpb->CHKSUM);
	printf( "initializing GCT/FRU at offset %x\n",hwrpb->FRU_OFFSET[0]);
    }
#else
    if (HWRPB) {
	hwrpb = HWRPB;
	tree = hwrpb->RSVD_HW[0];
    }
#endif
    cpu_mask = new_cpu_mask;
    printf("LP Configuration Tree = %x\n",tree);

    for (i=1;i<nodes;i++) {
	int primary;
	addr = dyn$_malloc(sizeof(UINT),DYN$K_SYNC|DYN$K_NOOWN);
	*(UINT *)addr = galaxy_base_pa[i] + PAL$PAL_BASE + 1;
	if (galaxy_cpu_mask[i]) {
	    for (primary=0; 
		    primary < MAX_PROCESSOR_ID; 
		    primary += PROCESSOR_STEP)
		if (galaxy_cpu_mask[i]>>primary & 1) break;
	    if (!galaxy_init[i]) {
		/* Copy PAL/Decompression code and portion of bitmap */
#if TURBO
		memcpy64(galaxy_base_pa[i],(int *)0x0,0x5200);
#if EV6
		memcpy64(galaxy_base_pa[i]+0x10000,(int *)0x10000,0xc000);
		memcpy64(galaxy_base_pa[i]+0x1e000,(int *)0x1e000,0x2000);
#else
		memcpy64(galaxy_base_pa[i]+0xc000,(int *)0xc000,0xa000);
		memcpy64(galaxy_base_pa[i]+0x16000,(int *)0x16000,0x2000);
#endif
		memcpy64(galaxy_base_pa[i]+0x400000,(0x200000+(galaxy_base_pa[i]/0x10000)),0x100000);
#endif

#if RAWHIDE
		memcpy64(galaxy_base_pa[i],(int *)0x0,0x20000);
#endif

#if (RAWHIDE)
		/* Store the CPU ID number of the primary for this "node" */
		
		*(UINT *) (galaxy_base_pa[i] + PAL$PRIMARY) = primary;
#endif
#if RAWHIDE
		/* copy the bitmap and memory descriptor info (instance) */
		move_rawhide_bitmap(galaxy_base_pa[i], 
				    galaxy_mem_size[i]);

#endif
	    }
	}
	for (j=0;j<MAX_PROCESSOR_ID;j++)
	    if (galaxy_cpu_mask[i]>>j & 1) {
		if (qual[QCPU].present) 
		    if (j != qual[QCPU].value.integer) continue;
		if ((galaxy_init[i]>>j) & 1)
		    pprintf("cpu %d already started in partition %d\n",j,i);
		else {
		    int *p=addr;
		    UINT *ptr=addr;
		    pprintf("starting cpu %d in partition %d at address %x%08x\n",j,i,p[1],p[0]);
		    krn$_micro_delay(1000000);
		    pid = krn$_create(start_helper,null_procedure,0,5,1<<j,0,
			"galaxy_start",0,0,0,0,0,0,addr,primary,i);
		    galaxy_init[i] |= 1<<j;
		}
	    }
    }
#if RAWHIDE
    /* copy the bitmap and memory descriptor info (primary) */
    if (memory_test_desc_table > galaxy_base_pa[1])
	move_rawhide_bitmap(0, 
			    galaxy_mem_size[0]);
    krn$_sleep(5000);	/* wait for other instances to start */
#endif
#if MODULAR
    memory_high_limit = galaxy_base_pa[1] - (mem_size - memory_high_limit);
    hf_buf_adr -= (mem_size - galaxy_base_pa[1]);
    mem_size = galaxy_base_pa[1];
#endif
#if TURBO
    /* Since the shared memory bitmap always resides in instance 0, copy 
     * the bitmap to its target boot location now, in case a secondary 
     * instance is booted first.  
     */
    memcpy64(memory_high_limit,0x200000,hf_buf_adr-memory_high_limit-config_size);
#endif
    return msg_success;
}
#endif

#if (CLIPPER)
#define QQ	0			/* qualifier -q */
#define QCPU	1			/* qualifier -cpu */
#define QMAX	2
int galaxy (int argc, char **argv) {
    int id,pid;
    int *addr;
    struct EVNODE evn, *evp=&evn;
    int nodes;
    int bad_config = 0;
    int galaxy_io_mask[4],galaxy_cpu_mask[4];
    unsigned __int64 galaxy_base_pa[4];
    char evname[80];
    char tmp[80];
    int i,j,k;
    int cpus=0,iops=0;
    int noprompt = FALSE;
    struct QSTRUCT qual[QMAX];
    struct HWRPB *hwrpb;
    int cpu;
    int galaxy_init[4] = {0,0,0,0};
    int new_cpu_mask = cpu_mask;
    UINT tree;
    extern int ___verylast;

    if (galaxy_node_number || (cpu_mask != all_cpu_mask)) {
	printf("Secondary partitions have already been started\n");
	return msg_failure;
    }

    qscan(&argc, argv, "-", "q %xcpu", qual);
    if ((qual[QQ].present) || (argc == 0))
	noprompt = TRUE;

    if (ev_read("lp_count", &evp, 0) == msg_success) {
	nodes = atoi(evp->value.string);

	if (nodes != galaxy_partitions) {
	    printf("System must be reset after modifying LP_COUNT\n");
	    return msg_failure;
	}

	if (nodes > 0) {

	    /* Merge all the CPU masks and IO masks */
	    for (i=0;i<nodes;i++) {
		sprintf(evname,"lp_io_mask%d",i);
		if (ev_read(evname, &evp, 0) == msg_success)
		    galaxy_io_mask[i] = xtoi(evp->value.string);
		else galaxy_io_mask[i] = 0;
		sprintf(evname,"lp_cpu_mask%d",i);
		if (ev_read(evname, &evp, 0) == msg_success)
		    galaxy_cpu_mask[i] = xtoi(evp->value.string);
		else galaxy_cpu_mask[i] = 0;

		cpus |= galaxy_cpu_mask[i];
		iops |= galaxy_io_mask[i];
		galaxy_base_pa[i] = *(UINT *) (0x40 + 8*i);
	    }

	    /* Check that all CPUs and TIOPs are used */
	    /*   but don't fail the config   */
	    j = (cpus ^ cpu_mask) & cpu_mask;
	    k = (iops ^ iop_mask) & iop_mask;
	    if (j) printf("CPU %s not configured in any partition\n",mask_to_id(j,tmp));
	    if (k) printf("TIOP %s not configured in any partition\n",mask_to_id(k,tmp));

	    /* Check that all defined CPUs and TIOPs exist */
	    j = cpus & ~cpu_mask;
	    k = iops & ~iop_mask;
	    if (j) printf("Non-existent CPU %s configured in a partition\n",mask_to_id(j,tmp));
	    if (k) printf("Non-existent TIOP %s configured in a partition\n",mask_to_id(k,tmp));
	    bad_config |= j | k;

	    for (i=0,k=0;i<nodes;i++) {
		for (j=0; j<MAX_PROCESSOR_ID; j += 1)
		    if (galaxy_cpu_mask[i]>>j & 1) break;
		if (j >= MAX_PROCESSOR_ID) {
		    printf("No valid primary processor specified for partition %d\n",i);
		    bad_config = 1;
		}
		else {
		    printf("Partition %d: Primary CPU = %d\n",i,j);
		    if (i != 0) new_cpu_mask &= ~(1<<j);
		}
		for (j=i+1;j<nodes;j++) {
		    if ((i != j) && (galaxy_cpu_mask[i] & galaxy_cpu_mask[j])) {
			printf("CPU %s configured in multiple partitions\n",
				mask_to_id(galaxy_cpu_mask[i] & galaxy_cpu_mask[j],tmp));
			bad_config = 1;
		    }
		    if ((i != j) && (galaxy_io_mask[i] & galaxy_io_mask[j])) {
			printf("TIOP %s configured in multiple partitions\n",
				mask_to_id(galaxy_io_mask[i] & galaxy_io_mask[j],tmp));
			bad_config = 1;
		    }
		}
	    }

	} else {
	    printf("System not configured to run multiple partitions\n");
	    return msg_failure;	    /* not a galaxy */
	}
    }
    else {
	printf("System not configured to run multiple partitions\n");
	return msg_failure;
    }

    if (bad_config) {
	if (noprompt == TRUE)
	    tmp[0] = 'N';
	else
	    read_with_prompt("\nDo you want to attempt to boot secondary partitions anyway? (Y/<N>) ",80,tmp,0,0,1);
	if (toupper(tmp[0]) != 'Y') {
	    pprintf("Secondary partitions not started...\n");
	    return msg_failure;
	}
    }

    if (HWRPB) {
	hwrpb = HWRPB;
	tree = hwrpb->RSVD_HW[0];
    }
    cpu_mask = new_cpu_mask;
    printf("LP Configuration Tree = %x\n",tree);

    for (i=1;i<nodes;i++) {
	int primary;
	addr = dyn$_malloc(sizeof(UINT),DYN$K_SYNC|DYN$K_NOOWN);
	*(UINT *)addr = galaxy_base_pa[i] + PAL$PAL_BASE + 1;
	if (galaxy_cpu_mask[i]) {
	    for (primary=0; 
		    primary < MAX_PROCESSOR_ID; 
		    primary += 1)
		if (galaxy_cpu_mask[i]>>primary & 1) break;
	    if (!galaxy_init[i]) {
		/* Copy PAL/Decompression code and portion of bitmap */

		memcpy64(galaxy_base_pa[i],(int *)0x0,&___verylast);

		/* Store the CPU ID number of the primary for this "node" */
		
		*(UINT *) (galaxy_base_pa[i] + PAL$PRIMARY) = primary;
	    }
	}
	for (j=0;j<MAX_PROCESSOR_ID;j++)
	    if (galaxy_cpu_mask[i]>>j & 1) {
		if (qual[QCPU].present) 
		    if (j != qual[QCPU].value.integer) continue;
		if ((galaxy_init[i]>>j) & 1)
		    pprintf("cpu %d already started in partition %d\n",j,i);
		else {
		    int *p=addr;
		    UINT *ptr=addr;
		    int lbs=1;
#if CLIPPER
		    /* set the jump flag */
		    write_dpr( offsetof( struct DPR, dpr_r_mp_ctl ) + 
				j * 0x10 +
				offsetof( struct MP_CONTROL, jump_flag ), 
				1, &lbs );
#endif
		    pprintf("starting cpu %d in partition %d at address %x%08x\n",j,i,p[1],p[0]);
		    krn$_micro_delay(1000000);
		    pid = krn$_create(start_helper,null_procedure,0,5,1<<j,0,
			"galaxy_start",0,0,0,0,0,0,addr,primary,i);
		    galaxy_init[i] |= 1<<j;
		}
	    }
    }
    return msg_success;
}
#endif

#if (TURBO || RAWHIDE || CLIPPER)
void start_helper(int *addr, int primary, int target) {
    struct SLOT *slot;
    struct HWRPB *hwrpb;

    if (HWRPB) {
	hwrpb = HWRPB;
	slot = (int)hwrpb + *(UINT *)hwrpb->SLOT_OFFSET + 
		*(UINT *)hwrpb->SLOT_SIZE * whoami();
	slot->STATE.SLOT$V_PA = 0;	/* not available */
    }

#if CLIPPER
    /* don't move to the new instance until the jump bit is clear */
    /* this emulates the normal SROM -> console start sequence */
    if (primary != whoami()) {	/* we're not the new primary */
	int jump = 1;

	while (jump == 1) {
	    /* read the jump flag */
	    read_dpr( offsetof( struct DPR, dpr_r_mp_ctl ) + 
				whoami() * 0x10 +
				offsetof( struct MP_CONTROL, jump_flag ), 
				1, &jump );
	}
    }
#endif

    spinlock(&spl_kernel);
    in_console &= ~(1<<whoami());
    cpu_mask &= ~(1<<whoami());
    spinunlock(&spl_kernel);

#if TURBO
    if (primary == whoami()) {
	write_csr_32($SYS_BUS_ADR(primary/2) + 0x1440, 0);
    }
    else {
	while (!(read_csr_32($SYS_BUS_ADR(primary/2) + 0x1440) & 1)) {}
    }
#endif

    cserve(62,target);
    spinlock(&spl_kernel);
    in_console |= 1<<whoami();
    cpu_mask |= (1 << whoami());
    spinunlock(&spl_kernel);

    /* handle the case where we're coming back to a booted instance */
    galaxy_check_boot();    /* this will set processor avail in hwrpb */
    galaxy_set_run_bit(0);
}

#endif

/*+
 * ============================================================================
 * = migrate - send all or one cpu to run in designated partition.            =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Removes selected CPUs from the current partition, and sends its
 *	execution to the targeted partition.  The command assumes that
 *	secondary partitions have previously been started via the 'lpinit'
 *	command.  
 *
 *	This command first validates that the target processor can be 
 *	migrated.  The user is not permitted to migrate the primary CPU,
 *	a CPU that is not in console mode, or a non-existant CPU number.
 *	Ultimately, a separate process (migrate_helper) is created to run
 *	on the targeted	CPUs, to perform the actual migration.  The
 *	migrated CPU will return into the migrate_helper context, should
 *	it ever be migrated back into this partition.
 *  
 *   COMMAND FMT: migrate 2 BZ 0 migrate
 *
 *   COMMAND FORM:
 *  
 *	migrate ( -{cpu <cpu_id>,all} -partition <partition_number> )
 *  
 *   COMMAND TAG: migrate 0 RXBZ migrate
 *
 *   COMMAND ARGUMENT(S):
 *
 *	<cpu_id> - whami id of cpu that is to be migrated
 *	<partition_number> - target partition number
 *
 *   COMMAND OPTION(S):
 *
 *	none
 *
 *   COMMAND EXAMPLE(S):
 *~
 *      >>> migrate -cpu 2 -partition 1
 *~
 *   COMMAND REFERENCES:
 *
 *	none
 *
 * FORM OF CALL:
 *  
 *	migrate( argc, *argv[] )
 *  
 * RETURN CODES:
 *
 *	msg_success - success
 *       
 * ARGUMENTS:
 *
 *	int argc - number of command line arguments passed by the shell
 *	char *argv [] - array of pointers to arguments
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
#if (TURBO || RAWHIDE || CLIPPER || WILDFIRE)
#define QCPU	0			/* qualifier -cpu */
#define QPARTITION	1		/* qualifier -partition */
#define QALL	2		/* qualifier -all */
#define QMAX	3
int migrate (int argc, char **argv) {
    struct QSTRUCT qual[QMAX];
    int i,cpu;
    struct EVNODE evn, *evp=&evn;

    qscan(&argc, argv, "-", "%dcpu %xpartition all", qual);
    if ( (!qual[QCPU].present && !qual[QALL].present) || (!qual[QPARTITION].present) || (argc !=1)) {
	printf ("invalid syntax\n");
	return msg_success;
    }

    if (qual[QCPU].present && (qual[QCPU].value.integer == primary_cpu)) {
	printf("can not migrate primary CPU %d\n",qual[QCPU].value.integer);
	return msg_success;
    }

    if (qual[QCPU].present && !((1<<qual[QCPU].value.integer) & in_console)) {
	printf("unable to migrate CPU %d\n",qual[QCPU].value.integer);
	return msg_success;
    }

    if (qual[QPARTITION].value.integer > galaxy_partitions-1) {
	printf("unable to migrate to partition %d\n",qual[QPARTITION].value.integer);
	return msg_success;
    }

    if (qual[QALL].present) i = in_console & ~(1<<primary_cpu);
    else i = 1<<qual[QCPU].value.integer;
    for (cpu=0;cpu<MAX_PROCESSOR_ID;cpu++)
	if ((i>>cpu)&1) {
	    printf("migrating CPU %d to partition %d\n",
		cpu,qual[QPARTITION].value.integer);
	    krn$_create(migrate_helper,null_procedure,0,5,1<<cpu,
		0,"galaxy_migrate",0,0,0,0,0,0,qual[QPARTITION].value.integer,0);
	}
}
#endif

#if (TURBO || RAWHIDE || CLIPPER)
void migrate_helper(int target) {
    char address[80];
    char *argv[3];
    UINT base;
    UINT impure_base,impure,console_base,pal_base,tmp;
    struct SLOT *slot;
    struct HWRPB *hwrpb;
    int id = whoami();

/*	  
**  This hunk of code locates the PAL impure area for the instance which we are
**  migrating to. It then uses information in the impure area to determine if
**  the other instance is in console mode or in the system software.
**
**  This impure area is the GALAXY saved context, not the normal PAL
**  context area.
*/	  

    console_base = 0x40 + (8*target);
    impure_base = *(UINT *)console_base;
    console_base = impure_base;

/*	  
**  The address of this CPUs GALAXY impure area in the other instance is
**  computed differently on Rawhide than on Turbo.
*/	  

#if TURBO
#if EV5
    if (whoami()/6) impure_base += PAL$CONSOLE_BASE;
    else impure_base += PAL$PAL_BASE;
    impure_base -= (6 - (whoami()%6)) * pal$impure_size;
#endif
#if EV6
    impure_base += PAL__TEMPS_BASE;
    impure_base -= (12 - whoami()) * offsetof(struct impure,cns$fpe_state);
#endif
#endif

#if RAWHIDE
    impure_base += PAL$IMPURE_BASE;	    /* offset to the PAL impure */
    impure_base += PAL$IMPURE_COMMON_SIZE;  /* ... past the common area */
    impure_base += 4 * PAL$IMPURE_SPECIFIC_SIZE;    /* past the 4 cpu slots */
    impure_base += whoami() * pal$impure_size;	    /* into the galaxy area */
#endif

#if CLIPPER
    impure_base += PAL$MIGRATE_BASE;
    impure_base += (whoami() - 1) * PAL$IMPURE_SPECIFIC_SIZE;
#endif

    impure = impure_base + offsetof(struct impure, cns$pal_base);
    ldqp(&impure,&pal_base);
#if EV5
    impure = impure_base + cns$pt20;
    ldqp(&impure,&tmp);
#endif
#if EV6
    impure = impure_base + offsetof(struct impure, cns$p_misc);
    ldqp(&impure,&tmp);
    tmp >>= 63;
#endif
    if (((pal_base == (console_base + PAL$PAL_BASE)) || 
         (pal_base == (console_base + PAL$OSFPAL_BASE))) &&
         (!(tmp&1))) galaxy_set_run_bit(1);

    if (HWRPB) {
	hwrpb = HWRPB;
	slot = (int)hwrpb + *(UINT *)hwrpb->SLOT_OFFSET + 
		*(UINT *)hwrpb->SLOT_SIZE * whoami();
	slot->STATE.SLOT$V_PA = 0;	/* not available */
    }

    spinlock(&spl_kernel);
    in_console &= ~(1<<whoami());
    cpu_mask &= ~(1<<whoami());
    if ((id == timer_cpu) && in_console) {
	int i,j;
	i = in_console;
	j = 0;
	while (!((i >> j) & 1))
	    j++;
	timer_cpu = j;
#if TURBO
	galaxy_set_poll_affinity(j);
#endif
    }
    spinunlock(&spl_kernel);
    cserve(62,target);
    spinlock(&spl_kernel);
    if (!(in_console & (1<<timer_cpu))) {
	timer_cpu = whoami();
#if TURBO
	galaxy_set_poll_affinity(whoami());
#endif
    }
    cpu_mask |= (1 << whoami());
    in_console |= 1<<whoami();
    spinunlock(&spl_kernel);

    /* handle the case where we're coming back to a booted instance */
    galaxy_check_boot();	/* this will set processor avail in hwrpb */
    galaxy_set_run_bit(0);

}
#endif

char *mask_to_id (int mask, char *s) {
    char tmp[80];
    int found=0;
    int i;
    *s = 0;
    for (i=0;i<32;i++) {
	if (mask>>i & 1) {
	    sprintf(tmp,"%d",i);
	    if (found) strcat(s,", ");
	    strcat(s,tmp);
	    found++;
	}
    }
    return s;
}

void memcpy64(UINT dest, UINT src, UINT len) {
    UINT d = dest;
    UINT s = src;
    UINT data,i;
    for (i=0;i<len/8;i++) {
	ldqp(&s,&data);
	stqp(&d,&data);
	s += 8;
	d += 8;
    }
}

#if RAWHIDE
/*+
 * ============================================================================
 * = move_rawhide_bitmap - copy the rawhide SROM memory descriptors
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Duplicate the Rawhide SROM memory descriptors and bitmap into
 *	the memory occupied by a Galaxy instance and update the descriptor
 *	info.
 *
 * FORM OF CALL:
 *
 *	move_rawhide_bitmap(start_pa, size)
 *
 * RETURNS:
 *
 *      None
 *                          
 * ARGUMENTS:
 *
 *      start_pa - The first physical address of this instance
 *	size - the size, in bytes, of this instance's memory
 *
 * SIDE EFFECTS:
 *
 *      memory_test_desc_table is updated to point to the copy
 *
-*/
void move_rawhide_bitmap(
UINT galaxy_base_pa, 
UINT galaxy_mem_size)
{
    UINT last_desc, desc_size;
    UINT new_bitmap_loc;
    UINT bitmap_area_start;
    UINT bitmap_offset;
    UINT new_memdsc;
    UINT page_align;
    UINT bitmap_area_pfn;
    UINT start_pfn;
    UINT old_memtest_dsc;
    unsigned int *ptr = (unsigned int *) &page_align;
    unsigned int i;

    ptr[0] = ~(HWRPB$_PAGESIZE - 1);
    ptr[1] = 0xffffffff;

    /* figure out how big the bitmap & descriptors are */

    last_desc = 
	ldq(memory_test_desc_table + offsetof(struct MEMDSC, CLUSTER_COUNT)) - 1;
    desc_size = 
	ldq(memory_test_desc_table + offsetof(struct MEMDSC, CLUSTER[last_desc].PFN_COUNT));
    bitmap_area_pfn = 
	ldq(memory_test_desc_table + offsetof(struct MEMDSC, CLUSTER[last_desc].START_PFN));
    bitmap_area_start = bitmap_area_pfn * HWRPB$_PAGESIZE;

    /* move the bitmap to the end of this memory range */
    new_bitmap_loc = (galaxy_base_pa + 
			galaxy_mem_size - 
			(desc_size * HWRPB$_PAGESIZE)) &
			page_align;

    /* do the move */
    memcpy64(new_bitmap_loc, 
		bitmap_area_start, 
		desc_size * HWRPB$_PAGESIZE);

    /* calculate the offset from the new bitmap to the old one */
    bitmap_offset = bitmap_area_start - new_bitmap_loc;

    /* update the pointers */
    if (galaxy_base_pa == 0) {	    /* instance 0 */
	old_memtest_dsc = memory_test_desc_table;
	memory_test_desc_table -= bitmap_offset;
	new_memdsc = memory_test_desc_table;

    } else {			    /* other instance(s) */
	stq((galaxy_base_pa + MEMORY_TEST_DESC_TABLE),
	     (new_bitmap_loc + (memory_test_desc_table - bitmap_area_start)));
	new_memdsc = (new_bitmap_loc + 
			    (memory_test_desc_table - bitmap_area_start));
    }

    /* fix the BITMAP_PA's */
    for (i = 0; i < last_desc; i++) {
	UINT bitmap_pa;

	bitmap_pa = 
	    ldq(new_memdsc + offsetof(struct MEMDSC, CLUSTER[i].BITMAP_PA));

	if (bitmap_pa) {
	    stq(new_memdsc + offsetof(struct MEMDSC, CLUSTER[i].BITMAP_PA),
		bitmap_pa - bitmap_offset);	/* it's PA has moved */

	    /* reset all the bits in the old bitmap that correspond to */
	    /* the old bitmap area to 1, so the shared memory doesn't */
	    /* have any holes */
	    if ((galaxy_base_pa == 0) && (i == (last_desc - 1))) {
		UINT old_bitmap_pa;
		UINT j, start_pfn;
		UINT bitmap_qw;
		int offset;

		old_bitmap_pa = ldq(old_memtest_dsc + 
				    offsetof(struct MEMDSC, CLUSTER[i].BITMAP_PA));
		start_pfn = ldq(old_memtest_dsc + 
				    offsetof(struct MEMDSC, CLUSTER[i].START_PFN));

		for (j = 0; j < desc_size; j++) {   /* all pages in space */
		    offset = (bitmap_area_pfn - start_pfn + j) / 64;	/* find qw */
		    bitmap_qw = ldq(old_bitmap_pa + (offset * 8));	/* get qw */
		    bitmap_qw = bitmap_qw | ((UINT)1 << ((bitmap_area_pfn - start_pfn + j) % 64));
		    stq(old_bitmap_pa + (offset * 8),			/* update */
			bitmap_qw);
		}
	    }
	}
    }

    /* fix the START_PFN of the last cluster */
    start_pfn = ldq(new_memdsc + offsetof(struct MEMDSC, CLUSTER[last_desc].START_PFN));
    stq((new_memdsc + offsetof(struct MEMDSC, CLUSTER[last_desc].START_PFN)),
	start_pfn - (bitmap_offset / HWRPB$_PAGESIZE));

}
#endif

/* This routine handles the case of migrating a new CPU into a partition
 * which has already booted the OS.  First, build the new HWRPB Per-CPU
 * Slot information.  Then, request a START command from the OS.
 */
#if (TURBO || RAWHIDE || CLIPPER || WILDFIRE)
void galaxy_check_boot() {
    int id = whoami();
    struct FILE *hfp;
    char hwrpbttn[16];
    int wait = 0;

    spinlock(&spl_kernel);
    cpu_mask |= (1 << id);
    spinunlock(&spl_kernel);

#if MODULAR
    if (hwrpbtt_inited) {
#else
    if (1) {
#endif
	if (console_mode[primary_cpu] == POLLED_MODE) {
	    console_mode[id] = POLLED_MODE;
	    mtpr_ipl(IPL_SYNC);
	}
	spinlock(&spl_kernel);
	if (!(in_console & (1<<timer_cpu))) {
	    timer_cpu = id;
#if TURBO
	    galaxy_set_poll_affinity(id);
#endif
	}
	spinunlock(&spl_kernel);
#if ( TURBO ||  RAWHIDE )
	sec_init(0);
#else
	if (HWRPB) {
	    struct SLOT *slot;
	    struct HWRPB *hwrpb;
	    hwrpb = HWRPB;
	    slot = (int)hwrpb + *(UINT *)hwrpb->SLOT_OFFSET + 
		    *(UINT *)hwrpb->SLOT_SIZE * whoami();
	    slot->STATE.SLOT$V_PA = 1;	/* available */
	}
#endif
	krn$_sleep(500);	/* wait for shell to start */
	hwrpb_tt_flush(id);	/* flush out any leftover tx data */
	sprintf(hwrpbttn, "hwrpbtta%d", id);
	hfp = fopen(hwrpbttn, "sa");
	if (hfp) {
	    fputs( "?STARTREQ?\n", hfp);
	    fclose(hfp);
	}
    }
}
#endif

void galaxy_isolate_partition(int phase) {
#if TURBO
UINT tmp;
int i,j;
struct TLSB *tlsb;
struct TLSB *tiop;
extern int galaxy_io_mask;
int not_mem_mask;
int lmmr_mask;

if (phase == 0) {
    /* 
     * First, make sure any other partition's address are flushed out
     * of our B-Cache
     */
    for (i=0;i<0x400000;i+=0x20)
	*(volatile UINT *) &tmp = *(UINT *)i;
    return;
}

if (phase == 1) {
    /* 
     * Clear the CPU and TIOP TLMMRs that don't correspond to this partition
     */
    not_mem_mask = mem_mask ^ get_galaxy_mem_mask(galaxy_node_number);
    lmmr_mask = 0;
    for (j=0;j<=8;j++) {
	int tlvid;
	if ((not_mem_mask>>j)&1) {
	    tlvid = read_csr_32($SYS_BUS_ADR(j) + 0xc0);
	    if (tlvid & 0x80) lmmr_mask |= (1<<(tlvid&7));
	    if (tlvid & 0x8000) lmmr_mask |= (1<<((tlvid>>8)&7));
	}
    }
    for (j=0;j<=8;j++) {
	if ((galaxy_io_mask>>j)&1) {
	    /* Repeat for all IOPs in our partition */
	    tiop = (struct TLSB *) $SYS_BUS_ADR(j);
	    tlsb = (struct TLSB *) $SYS_BUS_ADR(whoami()/2);
	    for (i=0;i<8;i++) {
		if (lmmr_mask & (1<<i)) {
		    write_csr_32((UINT)(&tiop->tlsb_r_tlmmr0)+(0x40*i), 0);
		    write_csr_32((UINT)(&tlsb->tlsb_r_tlmmr0)+(0x40*i), 0);
		}
	    }
	}
    }
    return;
}
#endif
}

#if TURBO
int get_galaxy_mem_mask (int partition) {
    int memcnt;
    int *devlist;
    struct device *dev;
    UINT ba, ea;
    int i,tmp=0;

    memcnt = find_all_dev(&config, TLSB$K_DTYPE_MIC, 0, 0, FAD$M_PASSED_ST);
    devlist = (int *) malloc(memcnt * sizeof(int *));
    find_all_dev(&config, TLSB$K_DTYPE_MIC, memcnt, devlist, FAD$M_PASSED_ST);

    ba = *(UINT *)(0x40 + (8 * partition));	/* partition begin addr */
    ea = *(UINT *)(0x40 + (8 * (partition+1)));	/* partition end addr */

    for (i=0;i<memcnt;i++) {
	dev = (struct device *)devlist[i];
	if (((UINT)dev->devdep.mem.base*1024*1024 >= ba) &&
	    ((UINT)dev->devdep.mem.base*1024*1024 < ea))
	    tmp |= 1<<dev->slot;
    }
    return tmp;
}
#endif

#if TURBO
void galaxy_set_poll_affinity(int id) {
	struct QUEUE	*q;
	struct PCB	*pcb;

	/*
	 * Walk down the PCB queue
	 */
	q = pcbq.flink;
	while ((void *) q != (void *) & pcbq.flink) {
	    pcb = (struct PCB *) ((int) q - offsetof (struct PCB, pcb$r_pq));
	    if (pcb->pcb$l_pid == poll_pid) break;
	    q = q->flink;
	}
	pcb->pcb$l_affinity = 1<<id;
}
#endif
