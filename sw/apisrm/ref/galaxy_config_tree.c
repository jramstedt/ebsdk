/*
 *++
 *  FACILITY:
 *
 *      Alpha Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *	Galaxy Configuration Tree
 *
 *  AUTHORS:
 *
 *      Nigel J. Croxon    (To control a world, to command a Galaxy!)
 *
 *  CREATION DATE:
 *  
 *      28-Feb-1998
 *
 *  MODIFICATION HISTORY:
 *
 *	njc	11-Jan-1999    Merge in GCT_TREE_INIT.C routines
 *	njc	18-Aug-1998    Add support for CLIPPER, WILDFIRE, GOLDRUSH
 *	njc	28-Feb-1998    Initial Config Tree support for TurboLaser
 *
 *--
 */
#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$src:ctype.h"
#include "cp$src:parse_def.h"
#include "cp$src:msg_def.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:console_hal_macros.h"
#include "cp$src:ev_def.h"
#include "cp$src:impure_def.h"
#include "cp$src:hwrpb_def.h"
#include "cp$src:pal_def.h"
#include "cp$src:fru50_def.h"
#include "cp$src:platform_fru.h"
#include "cp$src:gct.h"
#include "cp$src:gct_tree_init.h"
#include "cp$src:gct_routines.h"
#include "cp$src:gct_util.h"
#include "cp$src:gct_nodeid.h"
#include "cp$src:gct_tree.h"
#include "cp$src:galaxy_def_configs.h"
#if RAWHIDE || CLIPPER || WILDFIRE || MARVEL
#include "cp$src:platform_cpu.h"
#endif
#include "cp$src:get_console_base.h"

int __CMP_STORE_QUAD(volatile void *__source, __int64 __old_value, 
                     __int64 __new_value, volatile void  *__dest);

#if MODULAR
#include "cp$inc:kernel_entry.h"
#undef gct
#undef galaxy_set_run_bit
#endif

GCT_NODE *gct_root;

#define malloc(x) dyn$_malloc(x,DYN$K_FLOOD|DYN$K_NOOWN)

extern int HWRPB;
extern int max_hose;
extern int cpu_mask;
#if TURBO
extern int iop_mask;
extern int lp_hard_partition;
#else
#define lp_hard_partition 0
#endif
extern struct LOCK spl_kernel;
extern int shell_startup();
extern unsigned __int64 mem_size;
extern unsigned __int64 memory_high_limit;
extern int cpu_mask;
extern int in_console;
extern int primary_cpu;
extern int timer_cpu;
extern int poll_pid;
extern migrate_cpu();
extern int all_cpu_mask;

#if (TURBO || RAWHIDE || CLIPPER || WILDFIRE || MARVEL)
extern int mem_mask;
extern galaxy_node_number;
extern int galaxy_partitions;
#define UINT   unsigned __int64
#define uint   unsigned int
#define	int32	int
#define uint32  unsigned int
extern UINT hf_buf_adr;
#endif
#if RAWHIDE
extern int iod_mask;
extern UINT memory_test_desc_table;
#endif

null_procedure(void);
int tree_debug = 0;

int machine;
int galaxy_init;
int status;
char *gct_buffer;
char *config_tree;
int gct_size;
unsigned int config_size = 0x8000;
int tree_built;

#if (WINDJAMMER)
unsigned __int64 virt_to_phys(unsigned __int64 va){
	return msg_success;
}
#endif

#if (PC264)
int galaxy_node_number;
#endif

/*+
 * ============================================================================
 * = gct - build configuration tree                                           =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Build a Galaxy Configuration Tree
 *  
 *   COMMAND FMT: gct 2 Z 0 gct
 *
 *   COMMAND FORM:
 *  
 *	gct
 *  
 *   COMMAND TAG: gct 0 RXBZ gct
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
 *      >>> gct
 *~
 *   COMMAND REFERENCES:
 *
 *	none
 *
 * FORM OF CALL:
 *  
 *	gct()
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

int gct() {
    struct EVNODE      evn, *evp=&evn;
    int                notree_tmp = 0;
    GCT_PLATFORM_DATA  *pPlatform_data;
    GCT_BINDINGS       *pPlatform_bindings;
    GCT_PLATFORM_TREE  *pPlatform_tree, *platform_tree;
    GCT_GALAXY_NVRAM   *galaxy_nvram;
    GCT_BUFFER_HEADER  *pHeader;
    GCT_ROOT_NODE      *pRoot;
    char               galaxy_id[GALAXY_ID_LENGTH+8] = {""};
    struct HWRPB       *hwrpb;
    int		       j;

#if RAWHIDE
    pPlatform_data     = &rawhide_data;
#endif
#if TURBO
    pPlatform_data     = &TL_data;
    config_size        = 0x4000;
#endif
#if (TURBO||RAWHIDE)
    pPlatform_bindings = &TL_bindings;
    galaxy_nvram       = TL_galaxy_config;
    platform_tree      = TL_tree; 
#if RAWHIDE
    config_size        = 0x2000;
#endif
#endif
#if (CLIPPER || PC264)
    pPlatform_data     = &generic_data;
    pPlatform_bindings = &generic_bindings;
    platform_tree      = regatta_tree;
#endif
#if (PC264)
    galaxy_nvram       = generic_galaxy_config;
#endif
#if (WILDFIRE)
    pPlatform_data     = &wildfire_data;
    pPlatform_bindings = &wildfire_bindings;
    galaxy_nvram       = wildfire_galaxy_config;
    platform_tree      = wildfire_tree;
#endif
#if (MARVEL)
    pPlatform_data     = &marvel_data;
    galaxy_nvram       = marvel_galaxy_config;
    platform_tree      = marvel_tree;
#endif

    if (ev_read("lp_notree", &evp, 0) == msg_success)
	notree_tmp = atoi(evp->value.string);

    if (!notree_tmp) {

#if WILDFIRE
	if (tree_built) {
	    return ( config_tree );
	}
#else
	hwrpb = HWRPB;
	if ( !hwrpb || !(hwrpb->FRU_OFFSET[0]) ) {
#if (TURBO || RAWHIDE)
	    unsigned __int64 *base_addr = 0x40;
	    struct EVNODE evn,*evp=&evn;
	    ev_read("os_type", &evp, EV$K_SYSTEM);
	    if ((*evp->value.string == 'N') && galaxy_partitions)
#if TURBO
		config_tree = base_addr[1] - (mem_size - hf_buf_adr) - config_size;
#endif
#if RAWHIDE
		config_tree = base_addr[1] - config_size;
#endif
	    else
#endif
	    config_tree = dyn$_malloc(config_size,DYN$K_FLOOD|DYN$K_NOOWN|DYN$K_ALIGN,0x2000,0,0);
	    if (tree_debug) pprintf("Malloc Config_tree %x\n",config_tree);
	}	
#endif
	else {
	    memset (config_tree, 0, config_size);
	    if (tree_debug) pprintf("MemSet Config_tree %x\n",config_tree);
	}

#if !GCT6
	pHeader = (GCT_BUFFER_HEADER *) config_tree;
#endif
	gct_root = config_tree + GCT__BUFFER_HEADER_SIZE;
	pRoot = (GCT_ROOT_NODE *)  _GCT_MAKE_ADDRESS (0);
	gct_init$init_tree(pPlatform_data, pPlatform_bindings, config_tree, config_size);

#if ( TURBO || RAWHIDE )
	gct_build_platform_tree();
#endif

/*  Clipper, PC264 & Wildfire build the tree by moving xxxxx_tree into platform_tree  */
	gct_init$populate_tree(platform_tree);

#if (CLIPPER)
	if (tree_debug) pprintf("GCT Configuring HW branch\n");
	gct_init$platform_hw();
	/*
	 * Set the maximum number of partitions that can be created
	 */
        j = gct$get_max_partition();

	if (j < pRoot->max_platform_partition) pRoot->max_partition = j;
	    else pRoot->max_partition = pRoot->max_platform_partition;

	if (tree_debug) pprintf("GCT Configuring FRU branch\n");
	gct_init$platform_fru();
	if (tree_debug) pprintf("GCT Configuring SW branch\n");
	gct_init$platform_sw();
	if (tree_debug) pprintf("GCT Configuring Galaxy\n");
 	gct_init$configure_built_galaxy( galaxy_id );
#endif

#if WILDFIRE
	/*
	 * Set the maximum number of partitions that can be created
	 */
        j = gct$get_max_partition();

	if (j < pRoot->max_platform_partition) pRoot->max_partition = j;
	    else pRoot->max_partition = pRoot->max_platform_partition;

	if (tree_debug) pprintf("GCT Configuring SW branch\n");
	gct_init$platform_sw();

	if (tree_debug) pprintf("GCT Configuring HW branch\n");
	gct_init$platform_hw();

	if (tree_debug) pprintf("GCT Configuring FRU branch\n");
	gct_init$platform_fru();
#endif

#if (PC264)
	if (tree_debug) pprintf("GCT Configuring HW branch\n");
	gct_init$pc264_hw();
#endif

#if MARVEL
	if (tree_debug) pprintf("GCT Configuring SW branch\n");
	gct_init$platform_sw();

	if (tree_debug) pprintf("GCT Configuring HW branch\n");
	gct_init$platform_hw();
#endif

#if (TURBO || RAWHIDE || PC264)
	if (tree_debug) pprintf("GCT Make SW branch\n");
	gct_init$configure_galaxy( galaxy_nvram, galaxy_id);
#endif

#if (PC264)
	if (tree_debug) pprintf("GCT Configuring SW branch\n");
	gct_init$pc264_sw();
#endif

#if (CLIPPER || PC264)
	/* Adjust the config size down to only the used size */
	config_size -= pRoot->available;

	/* round size up to closest page boundry */
	config_size = ((unsigned int) config_size + 0x2000 - 1) & ~(0x2000 - 1);
	pHeader->buffer_size = config_size;
	pRoot->available = pRoot->first_free = 0;
#endif

#if !GCT6
	/* Checksum the config tree before returning */
	twos_checksum( ((int *)&pHeader->buffer_size ), config_size, &pHeader->buffer_cksum );
#endif

	if (tree_debug) pprintf("GCT %x\n",config_tree);
	tree_built = 1;
	return ( config_tree );
    } else {
	return ( 0 );
    }  /* end No-Tree */

}

/*+
 * ============================================================================
 * = GCT_PLATFORM_TREE -                                                      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Galaxy Platform Tree
 *
 *   COMMAND ARGUMENT(S):
 *
 *	none
 *
 *   COMMAND OPTION(S):
 *
 *	none
 *
 *   COMMAND REFERENCES:
 *
 *	none
 *
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
void gct_build_platform_tree() {
#if (TURBO || RAWHIDE)
	GCT_PLATFORM_TREE *tmp, *entry;
	GCT_GALAXY_NVRAM *sw_tmp, *sw_entry;
	struct EVNODE evn, *evp=&evn;
	int i;
	int j;
	int gal_partition = 0;
	int gal_cpum = 0;
	int gal_iom = 0;
	char evname[80];
	int primary_found ;
	int one_mem_sub = 0;
        unsigned __int64 *base_addr = 0x40;
#if TURBO
	int hard_cpu_mask = cpu_mask;
	int hard_iop_mask = iop_mask;
	int hard_mem_mask = mem_mask;
#endif

	tmp = TL_tree;			/* get tree base */ 
	tmp++;				/* bump pass HW_ROOT */

#if TURBO
#define cpu_mask hard_cpu_mask
#define iop_mask hard_iop_mask
#define mem_mask hard_mem_mask
	if (lp_hard_partition) {
	    sprintf(evname,"lp_cpu_mask%d",galaxy_node_number);
	    if (ev_read(evname, &evp, 0) == msg_success)
		hard_cpu_mask = xtoi(evp->value.string);
	    sprintf(evname,"lp_io_mask%d",galaxy_node_number);
	    if (ev_read(evname, &evp, 0) == msg_success)
		hard_iop_mask = xtoi(evp->value.string);
	    hard_mem_mask = get_galaxy_mem_mask(galaxy_node_number);
	}
#endif

	if (tree_debug) pprintf("Adding HW_ROOT\n");
	for (i=0;i<16;i++) {
#if TURBO
	    if (iop_mask & (1<<i)) {
#endif
#if RAWHIDE
	    if (iod_mask & (1<<i)) {
#endif
		uint *p;
		if (tree_debug) pprintf("HW IO port\n");
		entry = &TL_tree_template[0];
		memcpy(tmp, entry, sizeof(GCT_PLATFORM_TREE));
		tmp->id = i;

		/* define starting/ending PCI I/O Space addresses */
		p = &tmp->private[0];
#if TURBO
		p[0] = 0;
		p[1] = 0x80 + (i-4)*0x10;
		p[2] = 0xffffffff;
		p[3] = p[1] | 0xf;
#endif
#if RAWHIDE
		p[0] = 0;
		p[1] = 0xf8 + (i*0x20);
		p[2] = 0xffffffff;
		p[3] = p[1] | 0x1;
#endif
		tmp++;
	    }
	}
	for (i=0;i<16;i++) {
	    if (cpu_mask & (1<<i)) {
#if TURBO
		if (i%2 == 0) {
#endif
		    if (tree_debug) pprintf("HW CPU module  slot %x\n",i);
		    entry = &TL_tree_template[1];
		    memcpy(tmp, entry, sizeof(GCT_PLATFORM_TREE));
#if TURBO
		    tmp->id = i/2;
#endif
#if RAWHIDE
		    tmp->id = i;
#endif
		    tmp++;
#if TURBO
		}
#endif
		if (tree_debug) pprintf("HW CPU\n");
		entry = &TL_tree_template[2];
		memcpy(tmp, entry, sizeof(GCT_PLATFORM_TREE));
		tmp->id = i;
#if TURBO
		if (i%2) {
#endif
#if RAWHIDE
		if ((i == 2) || ( i == 3)) {
#endif
		    if (tree_debug) pprintf("setting NOPrimary on this CPU\n");
		    tmp->subtype = SNODE_CPU_NOPRIMARY;
		}
		tmp++;
	    }
	}
	if (one_mem_sub == 0) {
	    if (tree_debug) pprintf("HW Memory Sub\n");
	    entry = &TL_tree_template[3];
	    memcpy(tmp, entry, sizeof(GCT_PLATFORM_TREE));
	    tmp->id = 0;
	    tmp->private[1] = mem_size - 1;
#if TURBO
	    if (lp_hard_partition) {
		tmp->id = base_addr[galaxy_node_number];
		tmp->private[0] = base_addr[galaxy_node_number];
		tmp->private[1] = base_addr[galaxy_node_number+1] - 1;
	    }
#endif
	    tmp++;
	    one_mem_sub = 1;
	}

	for (i=0;i<16;i++) {
	    if (mem_mask & (1<<i)) {
		if (tree_debug) pprintf("HW Memory Ctrl  slot %x\n",i);
		entry = &TL_tree_template[4];
		memcpy(tmp, entry, sizeof(GCT_PLATFORM_TREE));
		tmp->id = i;
		tmp++;
	    }
	} 

	if (ev_read("lp_count", &evp, 0) == msg_success)
		gal_partition = atoi(evp->value.string);

	for (i=0;i<gal_partition;i++) {
#if TURBO
	    if (lp_hard_partition && (i != galaxy_node_number)) continue;
#endif
	    if (tree_debug) pprintf("Private Memory Desc\n");
	    entry = &TL_tree_template[5];
	    memcpy(tmp, entry, sizeof(GCT_PLATFORM_TREE));
	    tmp->id = base_addr[i];
	    tmp->private[0] = base_addr[i];
	    tmp->private[1] = base_addr[i+1] - base_addr[i];
	    tmp->private[2] = 0;
	    tmp++;
	} 

	if (( base_addr[gal_partition+1] - base_addr[gal_partition]) > 0 ) {
	    if (tree_debug) pprintf("Shared Memory Desc\n");
	    entry = &TL_tree_template[5];
	    memcpy(tmp, entry, sizeof(GCT_PLATFORM_TREE));
	    tmp->id = base_addr[gal_partition];
	    tmp->private[0] = base_addr[gal_partition];
	    tmp->private[1] = base_addr[gal_partition+1] - base_addr[gal_partition];
	    /* locate the bitmap */
#if TURBO
	    tmp->private[2] = base_addr[1] - (mem_size - memory_high_limit)
		+ base_addr[gal_partition]/0x10000 - 0x20;
#endif
#if RAWHIDE
	    {
		int i;
		int bitmap_offset;
		UINT memtest_dsc;
		int memtest_start_pfn;
		int memtest_end_pfn;
		int shared_pfn = base_addr[gal_partition] / HWRPB$_PAGESIZE;

		memtest_dsc = phys_to_virt(memory_test_desc_table);
		for (i = 0; i < *(UINT *) ((struct MEMDSC *)memtest_dsc)->CLUSTER_COUNT; i++) {
		    memtest_start_pfn = 
				*(UINT *)((struct MEMDSC *)memtest_dsc)->CLUSTER[i].START_PFN;
		    memtest_end_pfn = memtest_start_pfn +
				*(UINT *)((struct MEMDSC *)memtest_dsc)->CLUSTER[i].PFN_COUNT - 1;
		    if ((shared_pfn > memtest_start_pfn) &&
			(shared_pfn < memtest_end_pfn)) {
			/* compute the bitmap offset and */
			/* use the bitmap in instance 0 */
			bitmap_offset = mem_size - 
				*(UINT *)((struct MEMDSC *)memtest_dsc)->CLUSTER[i].BITMAP_PA;
			tmp->private[2] = 
				base_addr[1] - bitmap_offset +
				(shared_pfn - memtest_start_pfn) / 8;
			break;
		    }
		}
	    }
#endif
	    if (tree_debug) pprintf("Base %x  Size %x  Bitmap %x\n",
				  base_addr[gal_partition],
                                  base_addr[gal_partition+1] - base_addr[gal_partition],
				  tmp->private[2]);
	    tmp++;
	}

	if (( base_addr[gal_partition+2] - base_addr[gal_partition+1]) > 0 ) {
	    if (tree_debug) pprintf("Leftover Memory in Unowned Desc\n");
	    entry = &TL_tree_template[5];
	    memcpy(tmp, entry, sizeof(GCT_PLATFORM_TREE));
	    tmp->id = base_addr[gal_partition+1];
	    tmp->private[0] = base_addr[gal_partition+1];
	    tmp->private[1] = base_addr[gal_partition+2] - base_addr[gal_partition+1];
	    /* locate the bitmap */
#if TURBO
	    tmp->private[2] = base_addr[1] - (mem_size - memory_high_limit)
		+ base_addr[gal_partition+1]/0x10000 - 0x20;
#endif
#if RAWHIDE
	    {
		int i;
		int bitmap_offset;
		UINT memtest_dsc;
		int memtest_start_pfn;
		int memtest_end_pfn;
		int unowned_pfn = base_addr[gal_partition+1] / HWRPB$_PAGESIZE;

		memtest_dsc = phys_to_virt(memory_test_desc_table);
		for (i = 0; i < *(UINT *) ((struct MEMDSC *)memtest_dsc)->CLUSTER_COUNT; i++) {
		    memtest_start_pfn = 
				*(UINT *)((struct MEMDSC *)memtest_dsc)->CLUSTER[i].START_PFN;
		    memtest_end_pfn = memtest_start_pfn +
				*(UINT *)((struct MEMDSC *)memtest_dsc)->CLUSTER[i].PFN_COUNT - 1;
		    if ((unowned_pfn > memtest_start_pfn) &&
			(unowned_pfn < memtest_end_pfn)) {
			/* compute the bitmap offset and */
			/* use the bitmap in instance 0 */
			bitmap_offset = mem_size - 
				*(UINT *)((struct MEMDSC *)memtest_dsc)->CLUSTER[i].BITMAP_PA;
			tmp->private[2] = 
				base_addr[1] - bitmap_offset +
				(unowned_pfn - memtest_start_pfn) / 8;
			break;
		    }
		}
	    }
#endif
	    if (tree_debug) pprintf("Base %x  Size %x\n",
				    base_addr[gal_partition+1],
                                  mem_size - base_addr[gal_partition+1],
				  tmp->private[2]);
	    tmp++;
	}

	if (tree_debug) pprintf("Adding SW_ROOT \n");
	entry = &TL_tree_template[6];
	memcpy(tmp, entry, sizeof(GCT_PLATFORM_TREE));
	tmp->id =  0;
	tmp++;

	if (tree_debug) pprintf("Adding Template_Root\n");
	entry = &TL_tree_template[7];
	memcpy(tmp, entry, sizeof(GCT_PLATFORM_TREE));
	tmp->id =  0;
	tmp++;

	if (tree_debug) pprintf("Adding Template IOP \n");
	entry = &TL_tree_template[8];
	memcpy(tmp, entry, sizeof(GCT_PLATFORM_TREE));
	tmp->id =  0;
	tmp++;

	if (tree_debug) pprintf("Adding Template CPU \n");
	entry = &TL_tree_template[9];
	memcpy(tmp, entry, sizeof(GCT_PLATFORM_TREE));
	tmp->id =  0;
	tmp++;

	if (tree_debug) pprintf("Adding Template Memory Desc\n");
	entry = &TL_tree_template[10];
	memcpy(tmp, entry, sizeof(GCT_PLATFORM_TREE));
	tmp->id =  0;
	tmp++;

	sw_tmp = TL_galaxy_config;	/* get nvram tree base */
	sw_tmp++;			/* bump pass community */
	if (tree_debug) pprintf("SW Partition Adding Community\n");

	for (i=0;i<16;i++) {
	    if (cpu_mask & (1<<i)) {
#if TURBO
		if (i%2 == 0) {
#endif
		    if (tree_debug) pprintf("SW CPU module  slot %x\n",i);
		    sw_entry = &TL_galaxy_config_template[1];
		    memcpy(sw_tmp, sw_entry,sizeof(GCT_GALAXY_NVRAM));
#if TURBO
		    sw_tmp->id = i/2;
#else
		    sw_tmp->id = i;
#endif
		    sw_tmp++;
#if TURBO
		}
#endif
	    }

	} 

	if (tree_debug) pprintf("SW Memory Sub\n");
	sw_entry = &TL_galaxy_config_template[2];
	memcpy(sw_tmp, sw_entry, sizeof(GCT_GALAXY_NVRAM));
	sw_tmp->id = 0;
#if TURBO
	if (lp_hard_partition) sw_tmp->id = base_addr[galaxy_node_number];
#endif
	sw_tmp++;

	if (( base_addr[gal_partition+1] - base_addr[gal_partition]) > 0 ) {
	    if (tree_debug) pprintf("SW Shared Memory Desc\n");
	    sw_entry = &TL_galaxy_config_template[3];
	    memcpy(sw_tmp, sw_entry, sizeof(GCT_GALAXY_NVRAM));
	    sw_tmp->id = base_addr[gal_partition];
	    sw_tmp++;

	    sw_entry = &TL_galaxy_config_template[7];
	    memcpy(sw_tmp, sw_entry, sizeof(GCT_GALAXY_NVRAM));
	    sw_tmp->id = base_addr[gal_partition];
	    sw_tmp->private[3] = base_addr[gal_partition];
	    sw_tmp->private[4] = base_addr[gal_partition+1] - base_addr[gal_partition];
	    sw_tmp++;
	}

	for (i=0;i<gal_partition;i++) {

#if TURBO
	    if (lp_hard_partition && (i != galaxy_node_number)) continue;
#endif
	    if (tree_debug) pprintf("SW Partitions %x\n",i);
	    sw_entry = &TL_galaxy_config_template[8];
	    memcpy(sw_tmp, sw_entry, sizeof(GCT_GALAXY_NVRAM));
	    sw_tmp->private[0] += base_addr[i];

	    /* set the os_type */
	    ev_read("os_type", &evp, EV$K_SYSTEM);
	    sw_tmp->private[1] = gct_os_type(evp->value.string);
	    sw_tmp->id = i;
	    sw_tmp++;

	    if (tree_debug) pprintf("SW Private Memory Desc\n");
	    sw_entry = &TL_galaxy_config_template[3];
	    memcpy(sw_tmp, sw_entry, sizeof(GCT_GALAXY_NVRAM));
	    sw_tmp->id = base_addr[i];
	    sw_tmp++;

	    sw_entry = &TL_galaxy_config_template[4];
	    memcpy(sw_tmp, sw_entry, sizeof(GCT_GALAXY_NVRAM));
	    sw_tmp->id = base_addr[i];
	    sw_tmp->private[1] = i;
	    sw_tmp->private[3] = base_addr[i];
	    sw_tmp++;

	    sw_entry = &TL_galaxy_config_template[5];
	    memcpy(sw_tmp, sw_entry, sizeof(GCT_GALAXY_NVRAM));
	    sw_tmp->id = base_addr[i];
	    sw_tmp->private[1] = i;
	    sw_tmp->private[3] = base_addr[i]+TWO_MEG;
	    sw_tmp->private[4] = base_addr[i+1] - base_addr[i] - TWO_MEG - (0x41*0x2000);
	    sw_tmp++;

	    sw_entry = &TL_galaxy_config_template[6];
	    memcpy(sw_tmp, sw_entry, sizeof(GCT_GALAXY_NVRAM));
	    sw_tmp->id = base_addr[i];
	    sw_tmp->private[1] = i;
	    sw_tmp->private[3] = base_addr[i+1] - (0x41*0x2000);
	    sw_tmp++;

	    sprintf(evname,"lp_io_mask%d",i);
	    if (ev_read(evname, &evp, 0) == msg_success)
	   	gal_iom = xtoi(evp->value.string);

	    for (j=0;j<16;j++) {
		if (gal_iom & (1<<j)) {
		    if (tree_debug) pprintf("SW IOP \n");
		    sw_entry = &TL_galaxy_config_template[9];
		    memcpy(sw_tmp, sw_entry, sizeof(GCT_GALAXY_NVRAM));
		    sw_tmp->id = j;
		    sw_tmp++;
	        }
	    }

	    sprintf(evname,"lp_cpu_mask%d",i);
	    if (ev_read(evname, &evp, 0) == msg_success)
	   	gal_cpum = xtoi(evp->value.string);
	    primary_found = 0 ;

	    for (j=0;j<16;j++) {
		if (gal_cpum & (1<<j)) {
#if TURBO
		    if((j%2 ==0) && (primary_found == 0)) {
#endif
#if RAWHIDE
		    if (primary_found == 0) {
#endif
			if (tree_debug) pprintf("SW Primary CPU\n");
			sw_entry = &TL_galaxy_config_template[10];
			memcpy(sw_tmp, sw_entry, sizeof(GCT_GALAXY_NVRAM));
			primary_found = 1;
		    }
		    else {
			if (tree_debug) pprintf("SW Secondary cpu \n");
		        sw_entry = &TL_galaxy_config_template[11];
			memcpy(sw_tmp, sw_entry, sizeof(GCT_GALAXY_NVRAM));
#if TURBO
			if (j%2) {
			    if (tree_debug) pprintf("setting NOPrimary on this CPU\n");
			    sw_tmp->subtype = SNODE_CPU_NOPRIMARY;
			}
#endif
#if RAWHIDE
			if (tree_debug) pprintf("setting NOPrimary on this CPU\n");
			sw_tmp->subtype = SNODE_CPU_NOPRIMARY;
#endif
		    }
		    sw_tmp->id = j;
		    sw_tmp++;
	        }
	    }

	    if (i == galaxy_node_number) {
		for (j=0;j<16;j++) {
		    if (mem_mask & (1<<j)) {
			if (tree_debug) pprintf("SW Memory Ctrl\n");
			sw_entry = &TL_galaxy_config_template[12];
			memcpy(sw_tmp, sw_entry, sizeof(GCT_GALAXY_NVRAM));
			sw_tmp->id = j;
			sw_tmp++;
		    }
		} 
	    }
	}

#endif
}

/* end platform build tree */


/*
 * BUILD_TLV
 *
 *
 *  Build a Tag Length Value field in a nodes structure.
 *
 */

int build_tlv(GCT_TLV *pTLV, int style, char *strtmp, int length, GCT_NODE *pNode)
{
  GCT_TLV  *pTMP;
  unsigned char *pValue;
  int *pIValue;

  switch(style) {
	case 0:
	    printf("Not a TLV\n");
	    break;
	case TLV_TAG_ISOLATIN1:
	    pTLV->tlv_tag = TLV_TAG_ISOLATIN1;
	    pTLV->tlv_length = length;
	    pValue = (unsigned char *) &pTLV->tlv_value;
	    strncpy((char *) pValue, strtmp, pTLV->tlv_length);
	    pNode->size = (pNode->size + _ROUND_TLV_LENGTH(pTLV->tlv_length) + 4);
	    break;
	case TLV_TAG_QUOTED:
	    printf("quoted value\n");
	    break;
	case TLV_TAG_BINARY:
	    printf("binary\n");
	    break;
	case TLV_TAG_UNICODE:
	    printf("unicode\n");
	    break;
	default:
	    break;
  }

  return (SUCCESS);
}



/*
 *  Routine:
 *
 *	gct_util$appendchar
 *
 *  Abstract:
 *
 *	This is a utility routine used by printf.
 *
 *  Inputs:
 *
 *	c			- character
 *      **p			- Pointer to string pointer
 *
 *  Outputs:
 *
 *	string pointer is updated.
 *
 *  Side Effects:
 *
 *	Output character is appended to the string (zero terminated)
 *
 *  Completion Codes:
 *
 *	none
 */

void gct_util$appendchar( char c, char **p)
{
  char *tp = *p;
  
  *tp++ = c;
  *tp   = 0;
  *p += 1;

}

/*
 *  Stolen and hacked from STRTOL.C in [ACRTL] routines.
 *
 *  Convert a string of ASCII characters to an appropriate numeric value.
 *  The only difference between this routine and strtol is the type of the
 *  result and the conversion routine it uses.
 *
 *  Inputs: nptr    - Address of the source string.
 *	    endptr  - Address of a location into which is stored a pointer to
 *		      the first invalid character.
 *	    base    - Base for the conversion.  If this is zero, then the
 *		      contents fo the source string determine the base.
 *
 *  Returns: value  - Converted value if conversion is successful.
 *
 */

unsigned __int64 gct_util$strtouq (char *nptr, char **endptr, int base) {

    /*
     *  Local data declarations
     */
    char *ptr = nptr;
    char *subj_ptr;
    unsigned __int64 res = 0;
    int cur_digit, invalid = 0, sign = 1;

    /*
     * Make sure base is valid and nptr is not NULL.
     */
    if ( ((base < 0) || (base == 1) || (base > 36)) || (nptr == 0)) {
	if (endptr != 0)
	    *endptr = (char *) nptr;
	return(0);
    }

    /*
     * Skip white space
     */
    while ((*ptr == ' ') || (*ptr == '\n') || (*ptr == 9) )
	ptr++;

    /*
     * Check for sign, skip it if there is one.
     */
    if (*ptr == '-') {
	sign = -1;
	ptr++;
    } else if (*ptr == '+') {
	ptr++;
    }

    /*
     * If base = 0, figure out what base really is.  If base is hex, skip any
     * leading "0x".  We are now at the beginning of the subject sequence.
     */
    if (base == 0) {
	if (*ptr == '0') {
	    if ((*(ptr+1) == 'x') || (*(ptr+1) == 'X')) {    /* Base is hex */
		base = 16;
		ptr += 2;		    /* increment pointer past '0x' */
	    } else {			    /* Base is octal */
		base = 8;
	    }
	} else {			    /* default base is 10 */
	    base = 10;
	}
    } else {
	if ( (base == 16) &&
	    ( (*ptr == '0') && ((*(ptr+1) == 'x') || (*(ptr+1) == 'X')) ) )
		ptr += 2;		    /* increment pointer past '0x' */

    }
    subj_ptr = ptr;

    while (!invalid) {
	cur_digit = *ptr++;
	/* upcase - be more efficient than _toupper() */
	if (cur_digit >= 'a' && cur_digit <= 'z')
	    cur_digit &= 0xDF;
	if (((cur_digit < '0') || (cur_digit > 'Z')) || ((cur_digit > '9') && (cur_digit < 'A')))
	    break; /* exitloop */
	if (cur_digit >= 'A')		    /* Put 'A' - 'Z' right after '9' */
	    cur_digit -= 7;
	cur_digit -= '0';		    /* convert letter to digit */
	if ((cur_digit >= 0) && (cur_digit < base)) {
	    res = (res * base) + cur_digit;
	} else {
	    invalid = 1;
	}
    }
    --ptr;		/* back up pointer to first invalid character */

    res *= sign;    /* figure in sign */

    /*
     * If endptr non-NULL, store ptr in endptr
     */
    if (endptr) {       
    	if (subj_ptr == ptr)	/* if no valid characters, set *endptr to 1st */
	    *endptr = (char *) nptr;
	  else  
	    *endptr = (char *) ptr;
    }

    return(res);
}



/*
 *	Routine:	gct__alloc_node
 *
 *	Abstract:	Allocates a node from free pool or the lookaside
 *			list and initializes the fields to defaults or
 *			zero.
 *
 *	Inputs:		nodeType	- The TYPE of the node
 *
 *	Outputs:	GCT_HANDLE of the allocated node, or zero.
 *
 *	Side Effects:
 *
 */

GCT_HANDLE gct__alloc_node(char nodeType, char nodeSubType, unsigned __int64 flags, int extra_size, void *type_specific) {

  GCT_ROOT_NODE *pRoot;
  GCT_HANDLE newNode = 0, node;
  GCT_NODE *pNode, *pPrev;

  unsigned __int64 *pClear;
  int size = 0, j;

  /*
   *   Get the root node
   */
  pRoot = (GCT_ROOT_NODE *)  _GCT_MAKE_ADDRESS (0);

 /*   Check the lookaside list for any deleted nodes that can be
  *   reclaimed.
  */
  node = pRoot->lookaside;

 /*
  *  round to a 64 byte block
  */
  size = (((gct_node_info[nodeType].size + extra_size) + 63) & ~63);

  while (node) {
    pNode = _GCT_MAKE_ADDRESS(node);
    if (pNode->size == size) {
      newNode = _GCT_MAKE_HANDLE(pNode);
      if (newNode == pRoot->lookaside) {
        if (pNode->owner) pRoot->lookaside = pNode->owner;
      }
      else {
        pPrev->owner = pNode->owner;
      }
      break;
    }
    pPrev = pNode;
    node  = pNode->owner;
  }


  if (!newNode) {
    newNode = pRoot->first_free;
    pNode   = _GCT_MAKE_ADDRESS( pRoot->first_free);

   /*
    *   Allocate the memory
    */
    pRoot->available -= size;
    if (pRoot->available <= 0) {
	printf("GCT-Alloc Node Failed\n");
	return (0);
    }
    pRoot->first_free += size;

  }

 /*
  *   Initialize the node.  Clear it by quadwords
  */
  pClear = (unsigned __int64 *) ((char *) pNode);
  
  for (j = size >> 3; j > 0; j -= 1) {
    *pClear++ = 0;
  }

 /*
  *   Init the type, size, and make it valid
  */
  pNode->type    = nodeType;
  pNode->subtype = nodeSubType;
  pNode->node_flags = flags;
  pNode->size    = size;
  pNode->magic   = NODE_VALID;

  return (newNode);
}



/*
 *	Routine:	gct__delete_node
 *
 *	Abstract:	Unlinks a node from the tree and
 *			places it onto the lookaside list.
 *			Nodes in the lookaside list are
 *			linked by the OWNER field.
 *
 *	Inputs:		node		- Handle of the node to delete
 *
 *	Outputs:	none
 *
 *	Side Effects:
 *
 */

void gct__delete_node(GCT_HANDLE node) {

#if 0
  GCT_NODE *pNode, *pParent, *pNext, *pPrev;
  GCT_ROOT_NODE *pRoot;

  pRoot   = (GCT_ROOT_NODE *) _GCT_MAKE_ADDRESS (0);
  pNode   = _GCT_MAKE_ADDRESS( node);
  pParent = _GCT_MAKE_ADDRESS( pNode->parent);
  pNext   = _GCT_MAKE_ADDRESS( pNode->next_sib);
  pPrev   = _GCT_MAKE_ADDRESS( pNode->prev_sib);

 /*
  *    Unlink the node from the tree.
  *    If this is the first child of the Parent, make the next
  *    sibling the child.
  */
  if (pParent->child == node) pParent->child = pNode->next_sib;

 /*
  *    If there was a next sibling, make the previous
  *    sibling it's new previous sibling.
  */
  if (pNode->next_sib) pNext->prev_sib = pNode->prev_sib; 

 /*
  *    Unlink the previous node as well
  */
  if (pNode->prev_sib) pPrev->next_sib = pNode->next_sib; 

 /*
  *    Clear the pointers
  */
  pNode->parent  = 0;
  pNode->prev_sib = 0;
  pNode->next_sib = 0;
  pNode->child   = 0;
  pNode->owner   = 0;

 /*
  *   Now put each node in the branch onto the lookaside list
  */
  pNext = pNode;

  while (1) {

    if (pNext->child) {
      _GCT_MAKE_ADDRESS( pNext->child);
    }
    else {
      if (pNext->next_sib) {
        _GCT_MAKE_ADDRESS( pNext->next_sib);
      }
      else {
        pNext->type = 0;
        pNext->owner = 0;
        pNode->magic = 0;
        pNext->owner = _GCT_MAKE_HANDLE( pRoot->lookaside);
        pRoot->lookaside = _GCT_MAKE_HANDLE(pNext);
        if (pNext->prev_sib) {
          pNext = _GCT_MAKE_ADDRESS( pNext->prev_sib);
          pNext->next_sib = 0;
        }
        else {
          if (pNext->parent) {
            pNext = _GCT_MAKE_ADDRESS( pNext->parent);
            pNext->child = 0;
          }
          else break;
        }
      }
    }
  }
#endif
}



/*
 *	Routine:	gct__insert_node
 *
 *	Abstract:	This routine inserts a node into
 *			the tree.  It takes a reference node,
 *			and can insert as a sibling, child,
 *			last sibling, or tha last sibling of
 *			its child.
 *
 *	Inputs:		current		- Handle of reference node
 *			new		- Node to insert
 *			ins_type	- type of insertion
 *
 *	Outputs:	none
 *
 *	Side Effects:
 *
 */

void gct__insert_node(GCT_HANDLE current, GCT_HANDLE newNode, int ins_type)
{
  GCT_NODE *pCurrent, *pNew, *pTemp;
  char buffer[80];

  pCurrent = _GCT_MAKE_ADDRESS(current);
  pNew     = _GCT_MAKE_ADDRESS(newNode);

  if (ins_type == GCT__INSERT_NEXT_SIBLING) {

  if (tree_debug)
    printf("  - Inserting %x node %x, as the next sibling of %x\n",pNew->type, newNode, current);

    pNew->prev_sib = current;

    if (pCurrent->next_sib != 0) {
      pTemp = _GCT_MAKE_ADDRESS(pCurrent->next_sib);
      pTemp->prev_sib = newNode;
      pNew->next_sib = pCurrent->next_sib;
    }

    pCurrent->next_sib = newNode;
    pNew->parent       = pCurrent->parent;

  }

  if (ins_type == GCT__INSERT_CHILD) {

  if (tree_debug)
    printf("  - Inserting %x node %x, as the child of %x\n",pNew->type, newNode, current);

    pNew->parent = current;

    if (pCurrent->child != 0) {
      pTemp = _GCT_MAKE_ADDRESS(pCurrent->child);
      pTemp->parent = newNode;
      pNew->child = pCurrent->child;
    }

    pCurrent->child = newNode;

  }

  if (ins_type == GCT__INSERT_LAST_SIBLING) {

  if (tree_debug)
    printf("  - Inserting %x node %x, as the last sibling of %x\n",pNew->type, newNode, current);


    if (pCurrent->next_sib == 0) {
      pCurrent->next_sib = newNode;
      pNew->prev_sib = current;
    }
    else {
      pTemp = pCurrent;
      while (pTemp->next_sib) {
        pTemp = _GCT_MAKE_ADDRESS(pTemp->next_sib);
      }
      pTemp->next_sib = newNode;
      pNew->prev_sib = _GCT_MAKE_HANDLE(pTemp);
    }
  }

  if (ins_type == GCT__INSERT_CHILD_LAST_SIB) {

  if (tree_debug)
    printf("  - Inserting %x node %x, as the last sibling of the children of %x\n",pNew->type, newNode, current);

    pNew->parent = current;

    if (pCurrent->child == 0) {
      pCurrent->child = newNode;
    }
    else {
      pTemp = _GCT_MAKE_ADDRESS(pCurrent->child);
      while (pTemp->next_sib) {
        pTemp = _GCT_MAKE_ADDRESS(pTemp->next_sib);
      }
      pTemp->next_sib = newNode;
      pNew->prev_sib = _GCT_MAKE_HANDLE(pTemp);
    }
  }

}

/*
 *	Routine:	gct_partition_init
 *
 *	Abstract:	Initialize the hwrpb address in a partition node.
 *
 *	Inputs:		node		- Handle of the node to init
 *			type_specific	- A pointer to a uint64 with the hwrpb address
 *
 *	Outputs:	int		  SUCCESS - Success
 *
 *	Side Effects:	
 *
 */

int gct__partition_init( GCT_HANDLE node, GCT_HANDLE community,
 GCT_HANDLE partition, void *type_specific) {

  GCT_PARTITION_NODE *pPartition;
  unsigned __int64 *hwrpb;

  hwrpb = (unsigned __int64 *) type_specific;
  pPartition = (GCT_PARTITION_NODE *) _GCT_MAKE_ADDRESS(node);
  pPartition->hwrpb   = hwrpb[0];
  pPartition->os_type = hwrpb[1];

  return (SUCCESS);
}

/*
 *	Routine:	gct_cpu_init
 *
 *	Abstract:	Initialize a CPU node and make it the primary for
 *			the partition.
 *
 *	Inputs:		node		- Handle of the node to init
 *			type_specific	- Pointer to a partition ID
 *
 *	Outputs:	int		  SUCCESS - Success
 *
 *	Side Effects:	
 *
 */

int gct__cpu_init( GCT_HANDLE node, GCT_HANDLE community, GCT_HANDLE partition, void *type_specific) {

  GCT_NODE *pCPU;
  unsigned __int64 *what;
  GCT_HANDLE partitionHandle;

  what = (unsigned __int64 *) type_specific;

  pCPU = _GCT_MAKE_ADDRESS(node);

 /*
  *   If the owner of the CPU is not the community, then assign the
  *   current_owner field to the owner
  */
  if (community != partition) pCPU->current_owner = partition;

  pCPU->node_flags |= *what;

  return (SUCCESS);
}

/*
 *	Routine:	gct$assign_hw
 *
 *	Abstract:	This routine assigns a hardware device to a
 *			partition or community.
 *
 *	Inputs:		sw_node 	- The partition or community HANDLE
 *			node		- handle of the node to assign
 *
 *	Outputs:	int32
 *
 *			SUCCESS		- Success
 *			NOTPARTITION	- The partition node was NOT a partition!
 *			ILLEGALTREE	- The partition nodes parent was NOT a
 *					  community!
 *			NOTHARDWARE	- A hardware node was NOT hardware!
 *			ALREADYOWNED	- The hardware is already owned by another
 *					  partition or community
 *			NOTALLOWED	- An ancestor between the node and it's
 *					  CONFIG binding had an incompatable owner.
 *			
 *	Side Effects:	
 *
 */

int gct$assign_hw( GCT_HANDLE sw_node, GCT_HANDLE hw_node, unsigned int active, unsigned __int64 incarnation) {

  int status;
  GCT_HANDLE community;
  GCT_NODE *pNode;

  _GCT__LOCK_TREE(LOCK_FOR_UPDATE, GCT__INTERNAL_ID, ASSIGN_HW, GCT__RETRY_MAX);

 /*
  *   Validate both handles
  */
  status = gct__validate_handle( sw_node);

  if (status != SUCCESS) {
    if (tree_debug)     printf("Invalid SW Handle\n");
    gct__unlock_tree(UNLOCK_NO_UPDATE, GCT__INTERNAL_ID);
    return (status);
  }

  status = gct__validate_handle( hw_node);

  if (status != SUCCESS) {
    if (tree_debug)     printf("Invalid HW Handle\n");
    gct__unlock_tree(UNLOCK_NO_UPDATE, GCT__INTERNAL_ID);
    return (status);
  }

 /*
  *  Make sure its hardware...
  */
  pNode = _GCT_MAKE_ADDRESS(hw_node);
  if (!pNode->node_hardware) {
    if (tree_debug) printf("Not a HW node\n");
    gct__unlock_tree(UNLOCK_NO_UPDATE, GCT__INTERNAL_ID);
    return (NOTHARDWARE);
  }

 /*
  *  Quick check... is it already owned?
  */
  if (pNode->owner) {
    if (pNode->owner == sw_node) status = SUCCESS;
    else status = ALREADYOWNED;
    if (tree_debug) printf("Check and is Already Owned\n");
    gct__unlock_tree(UNLOCK_NO_UPDATE, GCT__INTERNAL_ID);
    return (status);
  }

 /*
  *   See what the SW node is...
  */
  pNode = _GCT_MAKE_ADDRESS(sw_node);

  if (pNode->type == NODE_SW_ROOT) {
    if (tree_debug) printf("Assign hw to sw root\n");
   /*
    *   Assign the hardware
    */
    status = gct__assign_hw_to_sw_root( sw_node, hw_node, active);
  }

  if (pNode->type == NODE_HARD_PARTITION) {
    if (tree_debug) printf("Assign hw to hard_partition\n");
   /*
    *   Assign the hardware
    */
    status = gct__assign_hw_to_hard_part( sw_node, hw_node, active);
  }

  if (pNode->type == NODE_COMMUNITY) {
    if (tree_debug) printf("Assign hw to community\n");
   /*
    *   Cake.  Its a community.  Assign the hardware.
    */
    status = gct__assign_hw_to_community( sw_node, hw_node, active);
  }

  if (pNode->type == NODE_PARTITION) {
    if (tree_debug) printf("Assign  hw to partition\n");
   /*
    *   Assign the hardware
    */
    status = gct__assign_hw_to_partition( sw_node, hw_node, active);
  }

  if (status == SUCCESS) {
    gct__unlock_tree(UNLOCK_UPDATE, GCT__INTERNAL_ID);
    return (status);
  }
  else {
    gct__unlock_tree(UNLOCK_NO_UPDATE, GCT__INTERNAL_ID);
    return (status);
  }

}


/*
 *	Routine:	gct$find_node
 *
 *	Abstract:
 *
 *        This routine is a shell that calls the utility routine to lookup
 *	  a node based on the input criteria.
 *
 *	  The configuration tree is searched, starting at a specified
 *	  node in the tree, for the first occurance of a node matching the
 *	  search criteria.  Searches may be for any node, for nodes of a specific
 *	  type, nodes owned by a specific instance (or unassigned), or for both
 *	  instance and type.
 *	
 *	  The routine returns the GCT_HANDLE of the matching node, or zero (the
 *	  handle of the root) if no match occurs.  If the routine is repeatedly
 *	  called with the handle returned from the previous call, the entire
 *	  tree will be traversed.
 *	
 *	Inputs:		root		- This node serves as the root for
 *					  the search.  The search will not
 *					  go "above" this node.
 *			current		- the address of the node handle to start the
 *                                        search at.
 *			search_criteria	- A bitmask indicating which fields
 *					  are significant in the match.
 *			search_type	- The TYPE of node to search for
 *			search_subType	- The SUBTYPE of the node to search for
 *			search_id	- The ID of the node to search for
 *			search_owner	- The OWNER field of the node to search for
 *
 *	Outputs:	*current	- The ID of the node that matches, or
 *					  zero.
 *
 *			int32
 *
 *			SUCCESS		- Success
 *			BADHANDLE	- Input handle was invalid
 *
 *	Side Effects:
 *
 */

int gct$find_node( GCT_HANDLE top, GCT_HANDLE *node,
               int search_criteria, char search_type,
               char search_subType, GCT_ID search_id,
               GCT_HANDLE search_owner, int *depth ) {

  int status;

  if (!node) return (BADPARAM);

  _GCT__LOCK_TREE(LOCK_FOR_READ, GCT__INTERNAL_ID, FIND_NODE, GCT__RETRY_MAX);

  status = gct__validate_handle( *node);

  if (status != SUCCESS) {
    gct__unlock_tree(UNLOCK_READ, GCT__INTERNAL_ID);
    return (status);
  }

  status = gct__validate_handle( top);

  if (status != SUCCESS) {
    gct__unlock_tree(UNLOCK_READ, GCT__INTERNAL_ID);
    return (status);
  }

  *node = gct__lookup_node( top, *node, search_criteria, search_type, search_subType, search_id, search_owner, depth );

  gct__unlock_tree(UNLOCK_READ, GCT__INTERNAL_ID);

  if (*node == 0) return (NOTFOUND);
  else return (SUCCESS);
}





/*
 *	Routine:	gct__lock_tree
 *
 *	Abstract:
 *
 *	  This routine takes out the lock on the tree
 *
 *	Inputs:	lock_type	- What type of lock (read/update/etc)
 *		caller_id	- ID to be stored for the lock
 *		lock_caller	- ID of the calling routine for error tracing
 *		retry		- Retry count
 *
 *	Outputs:		   1 = Success
 *
 *	Side Effects:
 *
 */


int gct__lock_tree( int lock_type, GCT_ID caller_id, int lock_caller, unsigned __int64 retry) {

  GCT_ROOT_NODE *pRoot;
  pRoot = (GCT_ROOT_NODE *) _GCT_MAKE_ADDRESS(0);

  return(SUCCESS);
}


/*
 *	Routine:	gct__unlock_tree
 *
 *	Abstract:
 *
 *	  This routine unlocks the tree
 *
 *	Inputs:	lock_type	- What type of unlock (read/update/noupdateetc)
 *		caller_id	- ID that we think has the lock
 *
 *	Outputs:		   1 = Success
 *
 *	Side Effects:
 *
 */


int gct__unlock_tree( int lock_type, GCT_ID caller_id) {

  GCT_ROOT_NODE *pRoot;
  pRoot = (GCT_ROOT_NODE *) _GCT_MAKE_ADDRESS(0);

  return(SUCCESS);
}



/*
 *	Routine:	gct$get_max_partitions
 *
 *	Abstract:	Figure out how many partitions can dance on the
 *			head of a pin... uh.  Oh.  How may total partitions
 *			will the configuration support.
 *
 *			This is accomplished by walking the template tree,
 *			figuring out how many of what device are needed, and
 *			then finding the total number and dividing.  For
 *			memory, we get the total memory size, and divide
 *			by the amount of memory needed.
 *
 *	Inputs:		none
 *
 *	Outputs:	int32		  Max Partitions
 *
 *	Side Effects:	
 *
 */

int gct$get_max_partition() {

  GCT_HANDLE        hw_root, tp_root, node, xnode;
  GCT_NODE          *pNode, *pXNode;
  GCT_MEM_DESC_NODE *pDesc;
  GCT_ROOT_NODE     *pRoot;
  int j, k, found, required, byteoff;

#ifndef SNODE_BYTES
#define SNODE_BYTES ((SNODE_LAST>>3)+1)
#endif

  char done[NODE_LAST][SNODE_BYTES];

  unsigned __int64 total_memory = 0, req_memory = 0, maxp, temp;
  char bitval;

  pRoot = (GCT_ROOT_NODE *)  _GCT_MAKE_ADDRESS (0);

  maxp = pRoot->max_partition;

 /*
  *  Clear array.  Used to tell if we have been to the node yet.
  */
  for (j = 0; j < NODE_LAST; j += 1) {
    for (k = 0; k < SNODE_BYTES; k += 1) {
      done[j][k] = 0;
    }
  }

  _GCT__LOCK_TREE( LOCK_FOR_READ, GCT__INTERNAL_ID, GET_MAX_PARTITION, GCT__RETRY_MAX);

  hw_root = gct__lookup_node( 0, 0, FIND_BY_TYPE, NODE_HW_ROOT, 0, 0, 0, 0);
  tp_root = gct__lookup_node( 0, 0, FIND_BY_TYPE, NODE_TEMPLATE_ROOT, 0, 0, 0, 0);

 /*
  *  Loop thru the template root.  For each node type, find out how many there
  *  are in the HW branch.
  */
  xnode = tp_root;
  while (xnode) {

    xnode = gct__lookup_node( tp_root, xnode, FIND_ANY, 0, 0, 0, 0, 0);

    if (xnode) {

      pXNode  = _GCT_MAKE_ADDRESS( xnode);

      bitval  = 1 << (pXNode->subtype & 0x07);
      byteoff = pXNode->subtype >> 3;

      if (tree_debug) pprintf("Processing template node type: %x\n",
	    pXNode->type);

      switch (pXNode->type) {

        case NODE_MEMORY_DESC:

          if (done[pXNode->type][byteoff] & bitval) break;

         /*
          *   Find the total memory.  Use the HW root for the search so
          *   we don't get the template size included.
          */
          node = hw_root;
          while (node) {
            node = gct__lookup_node( hw_root, node, FIND_BY_TYPE, NODE_MEMORY_DESC, 0, 0, 0, 0);
            if (node) {
              pNode  = _GCT_MAKE_ADDRESS( node);
              if ((!pNode->node_unavailable) &&
		  !(pNode->node_flags & NODE_HW_TEMPLATE)) {
                pDesc  = (GCT_MEM_DESC_NODE *) pNode;
#if !GCT6
                total_memory += pDesc->mem_info.base_size;
#else
                total_memory += pDesc->mem_cluster_desc.mcd_pfn_count * HWRPB$_PAGESIZE;
#endif
              }
            }
          }
        
         /*
          *   Get total memory needed per partition
          */
          node = tp_root;
          while (node) {
            node = gct__lookup_node( tp_root, node, FIND_BY_TYPE, NODE_MEMORY_DESC, 0, 0, 0, 0);
            if (node) {
              pNode  = _GCT_MAKE_ADDRESS( node);
              pDesc  = (GCT_MEM_DESC_NODE *) pNode;
#if !GCT6
              req_memory += pDesc->mem_info.base_size;
#else
              req_memory += pDesc->mem_cluster_desc.mcd_pfn_count * HWRPB$_PAGESIZE;
#endif
            }
          }
        
	  if (tree_debug) pprintf("Template requires %x, tree has %x memory\n",
	    req_memory, total_memory);

          temp = total_memory / req_memory;
          if (temp < maxp) maxp = temp;
        
          done[pXNode->type][byteoff] |= bitval;
          break;

        default:

          bitval  = 1 << (pXNode->subtype & 0x07);
          byteoff = pXNode->subtype >> 3;

          if (done[pXNode->type][byteoff] & bitval) break;

          found = required = 0;

         /*
          *   How many do we have?
          */
          node = hw_root;
          while (node) {
            node = gct__lookup_node( hw_root, node, FIND_BY_TYPE | FIND_BY_SUBTYPE, pXNode->type, pXNode->subtype, 0, 0, 0);
            if (node) {
              pNode  = _GCT_MAKE_ADDRESS( node);
              if ((!pNode->node_unavailable) &&
		  !(pNode->node_flags & NODE_HW_TEMPLATE)) found += 1;
            }
          }
        
         /*
          *   How many do we need?
          */
          node = tp_root;
          while (node) {
            node = gct__lookup_node( tp_root, node,FIND_BY_TYPE | FIND_BY_SUBTYPE, pXNode->type, pXNode->subtype, 0, 0, 0);
            if (node) {
              required += 1;
            }
          }
        
	  if (tree_debug) 
	    pprintf("Template requires %d, tree has %d nodes of type %x\n",
	    required, found, pXNode->type);

          temp = found / required;
          if (temp < maxp) maxp = temp;

          done[pXNode->type][byteoff] |= bitval;
          break;

      }
    }
  }

  gct__unlock_tree(UNLOCK_READ, GCT__INTERNAL_ID);
  return (maxp);
}


/*
 *	Routine:	gct__assign_hw_to_sw_root
 *
 *	Abstract:
 *
 *	  This routine attempts to assign hard partition ownership to a node and
 *	  it's children.
 *
 *	  Hardware components may be assigned a hardware owner.  The valid
 *	  ownership states are:
 *
 *	  ROOT			Not owned
 *	  COMMUNITY		Owned by Community (shared resource)
 *	  PARTITION		Owned by a Partition exclusive use)
 *	
 *	  A hardware node may be comprised of multiple descendents.
 *	  When the hardware node is assigned an partition owner,
 *	  all of it's descendents must be owned by the partition.
 *	  In addition, if the CONFIG binding is NOT the HW_ROOT,
 *	  then all ancestors between the node, and the CONFIG
 *	  binding must *also* have the same partition or community
 *	  owner.  This also means that all descendents of the CONFIG
 *	  binding must have the same partition owner.
 *
 *	  If the CONFIG binding owner is to a Community, then it
 *	  must be the same community as the partition.
 *
 *	  Simple?
 *  
 *	Inputs:	partition	- The partition to be assigned the HW
 *		community	- The community the partition belongs to
 *		hw_node		- The HW node to be assigned
 *
 *	Outputs:		   1 = Success
 *				  -1 = Partition is already owned by someone else
 *				  -2 = An anscestor node between the config node
 *				       is owned by another partition or community.
 *
 *	Side Effects:
 *
 */

int gct__assign_hw_to_sw_root( GCT_HANDLE sw_root, GCT_HANDLE hw_node,  int active) {

  GCT_NODE *pNode, *pTemp;
  GCT_HANDLE hw_root, tnode;

 /*
  *  Get the hw root & sw root
  */
  hw_root = gct__lookup_node( 0, 0, FIND_BY_TYPE, NODE_HW_ROOT, 0, 0, 0, 0);

 /*
  *  Get the address of the hw node to be assigned
  */
  pNode      = _GCT_MAKE_ADDRESS(hw_node);

  tnode = pNode->child;
  while (tnode) {
	pTemp = _GCT_MAKE_ADDRESS( tnode);
	if (pTemp->owner_both) {
	    if (pTemp->owner) {
		if (tree_debug) printf("Child Already Owned\n");
		return (CHILDALREADYOWNED);
	     }
	     if (tree_debug) printf("Child still active\n");
	     return (CHILDSTILLACTIVE);
	}
	tnode = gct__lookup_node( pNode->child, tnode, FIND_ANY, 0, 0, 0, 0, 0);
  }

 /*
  *   The node has no owner, and is not active
  *
  *   Now, check the simplest case - a CONFIG
  *   binding to the HW_ROOT
  */
  if (pNode->config == hw_root) {
	if (tree_debug) printf("Binding to HW_ROOT.\n");
	/*
	*  Success.  The simple case that the
	*  config pointer is the HW root
	*/
	pNode->owner = sw_root;
	if (active) pNode->current_owner = sw_root;

	/*
	*  Assign all the children (owned and active)
	*/
 	tnode = pNode->child;
 	while (tnode) {
	    pTemp = _GCT_MAKE_ADDRESS( tnode);
	    pTemp->owner = sw_root;
	    if (active) pTemp->current_owner = sw_root;
	    if (tree_debug) printf("assign %x owner %x current_owner %x\n",pTemp->type,pTemp->owner,pTemp->current_owner);
 	    tnode = gct__lookup_node( pNode->child, tnode, FIND_ANY, 0, 0, 0, 0, 0);
	}
	if (tree_debug) printf("Return from swroot\n");
	return (SUCCESS);
  }

  /*
  *   Assign the node to the sw_root
  */
  pNode->owner = sw_root;
  if (active) pNode->current_owner = sw_root;
  if (tree_debug) printf("swroot return good\n");
  return (SUCCESS);

}



/*
 *	Routine:	gct__assign_hw_to_hard_partition
 *
 *	Abstract:
 *
 *	  This routine attempts to assign hard partition ownership to a node and
 *	  it's children.
 *
 *	  Hardware components may be assigned a hardware owner.  The valid
 *	  ownership states are:
 *
 *	  ROOT			Not owned
 *	  COMMUNITY		Owned by Community (shared resource)
 *	  PARTITION		Owned by a Partition exclusive use)
 *	
 *	  A hardware node may be comprised of multiple descendents.
 *	  When the hardware node is assigned an partition owner,
 *	  all of it's descendents must be owned by the partition.
 *	  In addition, if the CONFIG binding is NOT the HW_ROOT,
 *	  then all ancestors between the node, and the CONFIG
 *	  binding must *also* have the same partition or community
 *	  owner.  This also means that all descendents of the CONFIG
 *	  binding must have the same partition owner.
 *
 *	  If the CONFIG binding owner is to a Community, then it
 *	  must be the same community as the partition.
 *
 *	  Simple?
 *  
 *	Inputs:	partition	- The partition to be assigned the HW
 *		community	- The community the partition belongs to
 *		hw_node		- The HW node to be assigned
 *
 *	Outputs:		   1 = Success
 *				  -1 = Partition is already owned by someone else
 *				  -2 = An anscestor node between the config node
 *				       is owned by another partition or community.
 *
 *	Side Effects:
 *
 */

int gct__assign_hw_to_hard_part( GCT_HANDLE hardpartition, GCT_HANDLE hw_node,  int active) {

  GCT_NODE *pHard_Partition, *pPartition, *pCommunity, *pNode, *pTemp, *pConfig;
  GCT_HANDLE hw_root, tnode, community, sw_root; 

 /*
  *  Get the hw root & sw root
  */
  hw_root = gct__lookup_node( 0, 0, FIND_BY_TYPE, NODE_HW_ROOT, 0, 0, 0, 0);
  sw_root = gct__lookup_node( 0, 0, FIND_BY_TYPE, NODE_SW_ROOT, 0, 0, 0, 0);

 /*
  *  Get the address of the hard partition, and the hw node to be assigned
  */
  pHard_Partition = _GCT_MAKE_ADDRESS(hardpartition);
  pNode      = _GCT_MAKE_ADDRESS(hw_node);

  tnode = pNode->child;
  while (tnode) {
	pTemp = _GCT_MAKE_ADDRESS( tnode);
	if (pTemp->owner_both) {
	    if (pTemp->owner) {
		if (tree_debug) printf("Child Already Owned\n");
		return (CHILDALREADYOWNED);
	     }
	     if (tree_debug) printf("Child still active\n");
	     return (CHILDSTILLACTIVE);
	}
	tnode = gct__lookup_node( pNode->child, tnode, FIND_ANY, 0, 0, 0, 0, 0);
  }

 /*
  *   The node has no owner, and is not active
  *
  *   Now, check the simplest case - a CONFIG
  *   binding to the HW_ROOT
  */
  if (pNode->config == hw_root) {
	if (tree_debug) printf("Binding to HW_ROOT.\n");
	/*
	*  Success.  The simple case that the
	*  config pointer is the HW root
	*/
	pNode->owner = hardpartition;
	if (active) pNode->current_owner = hardpartition;

	/*
	*  Assign all the children (owned and active)
	*/
 	tnode = pNode->child;
 	while (tnode) {
	    pTemp = _GCT_MAKE_ADDRESS( tnode);
	    pTemp->owner = hardpartition;
	    if (active) pTemp->current_owner = hardpartition;
	    if (tree_debug) printf("assign %x owner %x current_owner %x\n",pTemp->type,pTemp->owner,pTemp->current_owner);
 	    tnode = gct__lookup_node( pNode->child, tnode, FIND_ANY, 0, 0, 0, 0, 0);
	}
	if (tree_debug) printf("Return from hard_partition\n");
	return (SUCCESS);
  }

  /*
  *   Assign the node to the hard_partition
  */
  pNode->owner = hardpartition;
  if (active) pNode->current_owner = hardpartition;
  if (tree_debug) printf("hardpartition return good\n");
  return (SUCCESS);

}



/*
 *	Routine:	gct__assign_hw_to_partition
 *
 *	Abstract:
 *
 *	  This routine attempts to assign partition ownership to a node and
 *	  it's children.
 *
 *	  Hardware components may be assigned a hardware owner.  The valid
 *	  ownership states are:
 *
 *	  ROOT			Not owned
 *	  COMMUNITY		Owned by Community (shared resource)
 *	  PARTITION		Owned by a Partition exclusive use)
 *	
 *	  A hardware node may be comprised of multiple descendents.
 *	  When the hardware node is assigned an partition owner,
 *	  all of it's descendents must be owned by the partition.
 *	  In addition, if the CONFIG binding is NOT the HW_ROOT,
 *	  then all ancestors between the node, and the CONFIG
 *	  binding must *also* have the same partition or community
 *	  owner.  This also means that all descendents of the CONFIG
 *	  binding must have the same partition owner.
 *
 *	  If the CONFIG binding owner is to a Community, then it
 *	  must be the same community as the partition.
 *
 *	  Simple?
 *  
 *	Inputs:	partition	- The partition to be assigned the HW
 *		community	- The community the partition belongs to
 *		hw_node		- The HW node to be assigned
 *
 *	Outputs:		   1 = Success
 *				  -1 = Partition is already owned by someone else
 *				  -2 = An anscestor node between the config node
 *				       is owned by another partition or community.
 *
 *	Side Effects:
 *
 */

int gct__assign_hw_to_partition( GCT_HANDLE partition, GCT_HANDLE hw_node,  int active) {

  GCT_NODE *pPartition, *pCommunity, *pNode, *pTemp, *pConfig;
  GCT_HANDLE hw_root, tnode, community; 

 /*
  *  Get the hw root
  */
  hw_root = gct__lookup_node( 0, 0, FIND_BY_TYPE, NODE_HW_ROOT, 0, 0, 0, 0);

 /*
  *  Get the address of the partition, and the hw node to be assigned
  */
  pPartition = _GCT_MAKE_ADDRESS(partition);
  pNode      = _GCT_MAKE_ADDRESS(hw_node);

 /*
  *  Get the community node
  */
  community = pPartition->owner;
  pCommunity = _GCT_MAKE_ADDRESS(community);

 /*
  *  Sanity check.  Make sure it *is* a community
  */
  if (pCommunity->type != NODE_COMMUNITY) {
    if (tree_debug) printf("Not a Community.\n");
    return (ILLEGALTREE);
  }

 /*
  *   Is the node already owned by the specified partition?
  */
  if (pNode->owner == partition) {
    if (tree_debug) printf("Not a Partition.\n");
    return (SUCCESS);
  }

 /*
  *  Next cheap test.  Don't allow assignment if it's owned by someone else.
  */
  if (pNode->owner) {
    if (tree_debug) printf("Already Owned. In Partition\n");
    return (ALREADYOWNED);
  }

 /*
  *  Make sure it's not *active* on some other partition (that is,
  *  it's been deassigned, but it still has a current_owner).
  */
  if ((pNode->current_owner) && (pNode->current_owner != partition)) {
    if (tree_debug) printf("Still Active. In partition\n");
    return (STILLACTIVE);
  }

 /*
  *  One last things to check... we don't want to
  *  succeed if any of the component parts of the
  *  node is still active... or worse - a child is
  *  owned (this can only happen if there is a bug
  *  in some callback code).
  *
  */

  tnode = pNode->child;
  while (tnode) {
    pTemp = _GCT_MAKE_ADDRESS( tnode);
    if (pTemp->owner_both) {
      if (pTemp->owner) {
	 if (tree_debug) printf("Child Already Owned\n");
	 return (CHILDALREADYOWNED);
      }
      if (tree_debug) printf("Child still active\n");
      return (CHILDSTILLACTIVE);
    }
    tnode = gct__lookup_node( pNode->child, tnode, FIND_ANY, 0, 0, 0, 0, 0);
  }

 /*
  *   The node has no owner, and is not active
  *
  *   Now, check the simplest case - a CONFIG
  *   binding to the HW_ROOT
  */
  if (pNode->config == hw_root) {
    if (tree_debug) printf("Binding to HW_ROOT.\n");
   /*
    *  Success.  The simple case that the
    *  config pointer is the HW root
    */
    pNode->owner = partition;
    if (active) pNode->current_owner = partition;

   /*
    *  Assign all the children (owned and active)
    */
    tnode = pNode->child;
    while (tnode) {
      pTemp = _GCT_MAKE_ADDRESS( tnode);
      pTemp->owner = partition;
      if (active) pTemp->current_owner = partition;
      if (tree_debug) printf("assign %x owner %x current_owner %x\n",pTemp->type,pTemp->owner,pTemp->current_owner);
      tnode = gct__lookup_node( pNode->child, tnode, FIND_ANY, 0, 0, 0, 0, 0);
    }
    if (tree_debug) printf("Return AssigneD ALL\n");
    return (SUCCESS);
  }

 /*
  *   Now tackle the hard case.  A non HW_ROOT CONFIG binding.
  *
  *   First walk up the anscestor chain to the CONFIG binding,
  *   and reject the assignment if any of them are owned by
  *   something other than the Community, or the current Partition
  */
  tnode = pNode->parent;
  pTemp = _GCT_MAKE_ADDRESS( pNode->parent);

  while (tnode != pNode->config) {

   /*
    *  The nodes ancestors must all belong to
    *  same community.
    *
    *  ... and the config node MUST be an ancestor!
    *
    */
    if (pTemp->owner != community) {
        if (tree_debug) printf("owner != community\n");
	return (NOTSHARED);
    }

    pTemp = _GCT_MAKE_ADDRESS( pTemp->parent);
    tnode = pTemp->parent;

    if ((!tnode) || (tnode == hw_root)) {
      if (tree_debug) printf("owner != community\n");
      break;
    }
  }

 /*
  *  OK.  We're fine.
  *
  *  The config binding owner is the community, then
  *  assign the node and its descendents to the partition.
  *
  */
  pConfig = _GCT_MAKE_ADDRESS(pNode->config);

  pNode->owner = partition;
  if (active) pNode->current_owner = partition;
  tnode = pNode->child;
  while (tnode) {
    pTemp = _GCT_MAKE_ADDRESS( tnode);
    pTemp->owner = partition;
    if (tree_debug) printf("assign %x owner %x current_owner %x\n",pTemp->type,pTemp->owner,pTemp->current_owner);
    if (active) pTemp->current_owner = partition;
    tnode = gct__lookup_node( pNode->child, tnode, FIND_ANY, 0, 0, 0, 0, 0);
  }
  if (tree_debug) printf("assigned BOTTOM\n");
  return (SUCCESS);
}

/*
 *	Routine:	gct__assign_hw_to_community
 *
 *	Abstract:
 *
 *	  The community is the basis for sharing resources.  When a
 *	  HW node is assigned to a community, it means that the HW and
 *	  potentially it's children may be shared.
 *
 *	  When the hw node is set to community ownership, the descendents
 *	  do not have ownership set.  Later on, a descendent may be assigned
 *	  to a partition in the community, or to the community.
 *
 *	  We look back up the parent chain to make sure that no parent node
 *	  is assigned to a partition, or to another community.
 *
 *	Inputs:	community		- The handle of the community
 *		hw_node			- The handle of the node to assign
 *
 *	Outputs:			  1 = Success
 *					 -1 = Node already assigned to someone else
 *					 -2 = An anscestor is owned by someone else
 *
 *	Side Effects:
 *
 */

int gct__assign_hw_to_community( GCT_HANDLE community, GCT_HANDLE hw_node, unsigned int active) {

  GCT_NODE *pCommunity, *pNode, *pTemp, *pConfig;
  GCT_HANDLE hw_root, tnode;

  hw_root = gct__lookup_node( 0, 0, FIND_BY_TYPE, NODE_HW_ROOT, 0, 0, 0, 0);

  pCommunity = _GCT_MAKE_ADDRESS(community);
  pNode      = _GCT_MAKE_ADDRESS(hw_node);

 /*
  *   Is the node already owned by this community?
  */
  if (pNode->owner == community) {
    if (tree_debug) printf("already owned by this community\n");
    return (SUCCESS);
  }

 /*
  *   Next cheap test.  Don't allow assignment if it's owned
  */
  if (pNode->owner) {
    if (tree_debug) printf("already owned by someone\n");
    return (ALREADYOWNED);
  }

 /*
  *  Make sure it's not *active* on some other partition or
  *  community (that is, it's been deassigned, but it still
  *  has a current_owner).
  */
  if ((pNode->current_owner) && (pNode->current_owner != community)) {
    if (tree_debug) printf("still active\n");
    return (STILLACTIVE);
  }

 /*
  *   Now, check the next simplest case - a CONFIG
  *   binding to the HW_ROOT, with no owner
  */
  if (pNode->config == hw_root) {

   /*
    *   Success.  The simple case that the config pointer is the HW root
    */
    pNode->owner = community;
    if (active) pNode->current_owner = community;
    if (tree_debug) printf("return GOOD\n");
    return (SUCCESS);

  }

 /*
  *   Not simple case, just validate that the parents between this
  *   node and the CONFIG are not bound to another community or partition
  */
  tnode = hw_node;

  while (tnode != pNode->config) {
    pTemp = _GCT_MAKE_ADDRESS( pNode->parent);
    tnode = pNode->parent;
    if (pTemp->owner != community) {
      if (tree_debug) printf("not shared\n");
      return (NOTSHARED);
    }
  }

 /*
  *   Assign the node to the community
  */
  pNode->owner = community;
  if (active) pNode->current_owner = community;
  if (tree_debug) printf("return good\n");
  return (SUCCESS);
}


/*
 *	Routine:	gct__deassign_hw_from_owner
 *
 *	Abstract:	This routine clears the ownership of a hardware
 *			node and all its children.
 *
 *	Inputs: 	owner		- Current SW owner handle
 *			hw_node		- The handle of the node to deassign
 *			active		- if this value is zero, then also clear current_owner
 *			
 *
 *	Outputs:			  1 = Success
 *
 *	Side Effects:
 *
 */

int gct__deassign_hw_from_owner( GCT_HANDLE sw_owner, GCT_HANDLE hw_node, unsigned int active) {

  GCT_MEM_DESC_NODE *pDesc;
#if !GCT6
  GCT_MEM_DESC      *pFrag;
#endif
  GCT_NODE          *pNode, *pTemp;
  GCT_HANDLE         hw_root, tnode;
  int j;

  pNode = _GCT_MAKE_ADDRESS(hw_node);

 /*
  *   Check the ownership
  */
  if (pNode->owner != sw_owner) {
    return (BADOWNER);
  }

 /*
  *   Just do it.  Any/all checking to see if this is
  *   legal must be done by the caller.  Unless the
  *   structure is corrupt, all children *must* have
  *   the same owner.
  */
  pNode->owner = 0;
  if (!active) pNode->current_owner = 0;

 /*
  *  Memory descriptor nodes need additional work...
  */
#if !GCT6
  if (pNode->type == NODE_MEMORY_DESC) {
    pDesc  = (GCT_MEM_DESC_NODE *) pNode;
    pFrag  = (GCT_MEM_DESC *) ((char *) pDesc + pDesc->mem_frag);
   /*
    *  If going inactive, then set the fragment count to 1.  And
    *  Reset the initial fragment
    */
    if (!active) {
      pDesc->mem_info.desc_count = 1;
      pFrag[0].mem_owner  = 0;
      pFrag[0].mem_current_owner = 0;
      pFrag[0].mem_flags  = 0;
      pFrag[0].pa         = pDesc->mem_info.base_pa;
      pFrag[0].size       = pDesc->mem_info.base_size;
    }
    else {
     /*
      *  Otherwise, just set each fragment to inactive
      */
      for (j = 0; j < pDesc->mem_info.desc_count; j += 1) {
        pFrag[j].mem_owner = 0;
      }
    }
  }
#else
  if (pNode->type == NODE_MEMORY_DESC) {
    pDesc  = (GCT_MEM_DESC_NODE *) pNode;

  }
#endif   /* end GCT6  */

 /*
  *  Deassign all the children
  *
  */
  tnode = pNode->child;
  while (tnode) {
    tnode = gct__lookup_node( pNode->child, tnode, FIND_ANY, 0, 0, 0, 0, 0);
    if (tnode) {
      pTemp = _GCT_MAKE_ADDRESS( tnode);

      pTemp->owner = 0;
      if (!active) pTemp->current_owner = 0;

#if !GCT6
     /*
      *  Memory descriptor nodes need additional work...
      */
      if (pTemp->type == NODE_MEMORY_DESC) {
        pDesc  = (GCT_MEM_DESC_NODE *) pTemp;
        pFrag  = (GCT_MEM_DESC *) ((char *) pDesc + pDesc->mem_frag);
       /*
        *  If going inactive, then set the fragment count to 1.  And
        *  Reset the initial fragment
        */
        if (!active) {
          pDesc->mem_info.desc_count = 1;
          pFrag[0].mem_owner  = 0;
          pFrag[0].mem_current_owner = 0;
          pFrag[0].mem_flags  = 0;
          pFrag[0].pa         = pDesc->mem_info.base_pa;
          pFrag[0].size       = pDesc->mem_info.base_size;
        }
        else {
         /*
          *  Otherwise, just set each fragment to inactive
          */
          for (j = 0; j < pDesc->mem_info.desc_count; j += 1) {
            pFrag[j].mem_owner = 0;
          }
        }
      }
#else

#endif   /* end if GCT6  */

    }
  }
  return (SUCCESS);
}



/*
 *	Routine:	gct$validate_partition
 *
 *	Abstract:	This routine uses the template tree to validate that
 *			a partition has the minimal resources needed to
 *			initialize a console and boot an operating system.
 *
 *	Inputs:		partition	- The HANDLE of the partition
 *
 *	Outputs:	int32
 *
 *			SUCCESS		- Success!
 *			
 *
 *	Side Effects:
 *
 */

int gct$validate_partition(GCT_HANDLE partition) {

  GCT_HANDLE        hw_root, tp_root, node, xnode;
  GCT_NODE          *pNode, *pXNode, *pPartition;
#if !GCT6
  GCT_MEM_DESC      *pFrag;
#endif
  GCT_MEM_DESC_NODE *pDesc;
  GCT_ROOT_NODE     *pRoot;
  int status, j, k, found, required, byteoff;

#ifndef SNODE_BYTES
#define SNODE_BYTES ((SNODE_LAST>>3)+1)
#endif
  char done[NODE_LAST][SNODE_BYTES];

  unsigned __int64 total_memory = 0, req_memory = 0, maxp, temp, base = 0, console = 0;
  char bitval;

  _GCT__LOCK_TREE(LOCK_FOR_READ, GCT__INTERNAL_ID, VALIDATE_PARTITION, GCT__RETRY_MAX);

  status = gct__validate_handle( partition);

  if (status != SUCCESS) {
    gct__unlock_tree(UNLOCK_READ, GCT__INTERNAL_ID);
    return (status);
  }

  pRoot = (GCT_ROOT_NODE *)  _GCT_MAKE_ADDRESS (0);
  pPartition = _GCT_MAKE_ADDRESS (partition);

  maxp = pRoot->max_partition;

  for (j = 0; j < NODE_LAST; j += 1) {
    for (k = 0; k < SNODE_BYTES; k += 1) {
      done[j][k] = 0;
    }
  }

  hw_root = gct__lookup_node( 0, 0, FIND_BY_TYPE, NODE_HW_ROOT, 0, 0, 0, 0);
  tp_root = gct__lookup_node( 0, 0, FIND_BY_TYPE, NODE_TEMPLATE_ROOT, 0, 0, 0, 0);

  xnode = tp_root;
  while (xnode) {

    xnode = gct__lookup_node( tp_root, xnode, FIND_ANY, 0, 0, 0, 0, 0);

    if (xnode) {

      pXNode  = _GCT_MAKE_ADDRESS( xnode);

      bitval  = 1 << (pXNode->subtype & 0x07);
      byteoff = pXNode->subtype >> 3;

      switch (pXNode->type) {

        case NODE_MEMORY_DESC:

          if (done[pXNode->type][byteoff] & bitval) break;

         /*
          *   Find the total memory owned by this partition
          */
          node = hw_root;
          while (node) {
            node = gct__lookup_node( hw_root, node, FIND_BY_TYPE, NODE_MEMORY_DESC, 0, 0, 0, 0);
            if (node) {
              pNode  = _GCT_MAKE_ADDRESS( node);
              pDesc  = (GCT_MEM_DESC_NODE *) pNode;
#if !GCT6
              pFrag = (GCT_MEM_DESC *) ((char *) pDesc + pDesc->mem_frag);
              for (j = 0; j < pDesc->mem_info.desc_count; j += 1) {
                if (pFrag[j].mem_owner == partition) {
                  total_memory += pFrag[j].size;
                  if (pFrag[j].mem_base) {
                    base += pFrag[j].size;
                    if (pFrag[j].mem_console) {
                      console += pFrag[j].size;
                    }
                  }
                }            
              }
#else
              total_memory += pDesc->mem_cluster_desc.mcd_pfn_count * HWRPB$_PAGESIZE;

              if (pDesc->mem_cluster_desc.mcd_usage_flags == MCD_USAGE_FIXED) {
                    base += pDesc->mem_cluster_desc.mcd_pfn_count * HWRPB$_PAGESIZE;
                    if (pDesc->mem_cluster_desc.mcd_usage_flags == MCD_USAGE_CONSOLE)
			console += pDesc->mem_cluster_desc.mcd_pfn_count * HWRPB$_PAGESIZE;
	      }

#endif  /* end if gct6 */
            }
          }
        
         /*
          *   Get total memory needed per partition
          */
          node = tp_root;
          while (node) {
            node = gct__lookup_node( tp_root, node, FIND_BY_TYPE, NODE_MEMORY_DESC, 0, 0, 0, 0);
            if (node) {
              pNode  = _GCT_MAKE_ADDRESS( node);
              pDesc  = (GCT_MEM_DESC_NODE *) pNode;
#if !GCT6
              req_memory += pDesc->mem_info.base_size;
#else
	      printf("mcd_pfn_count %x\n",pDesc->mem_cluster_desc.mcd_pfn_count);
              req_memory += pDesc->mem_cluster_desc.mcd_pfn_count * HWRPB$_PAGESIZE;
#endif
            }
          }
        
          done[pXNode->type][byteoff] |= bitval;

          if ((total_memory < req_memory) || 
              (console < pRoot->console_req) ||
              (base < pRoot->base_alloc)) {

            gct__unlock_tree(UNLOCK_READ, GCT__INTERNAL_ID);
            return (NOTFOUND);
          }
          break;

        default:

          bitval  = 1 << (pXNode->subtype & 0x07);
          byteoff = pXNode->subtype >> 3;

          if (done[pXNode->type][byteoff] & bitval) break;

          found = required = 0;

         /*
          *   How many do we have?
          */
          node = hw_root;
          while (node) {
            node = gct__lookup_node( hw_root, node, FIND_BY_TYPE | FIND_BY_SUBTYPE | FIND_BY_OWNER, pXNode->type, pXNode->subtype, 0, partition, 0);
            if (node) {
              found += 1;
            }
          }
        
         /*
          *   How many do we need?
          */
          node = tp_root;
          while (node) {
            node = gct__lookup_node( tp_root, node,FIND_BY_TYPE | FIND_BY_SUBTYPE, pXNode->type, pXNode->subtype, 0, 0, 0);
            if (node) {
              required += 1;
            }
          }
        
          temp = found / required;

          if (found < required)  {
            gct__unlock_tree(UNLOCK_READ, GCT__INTERNAL_ID);
            return (NOTFOUND);
          }

          done[pXNode->type][byteoff] |= bitval;
          break;

      }
    }
  }
  gct__unlock_tree(UNLOCK_READ, GCT__INTERNAL_ID);
  return (SUCCESS);
}

/*
 *	Routine:	gct$update_gmdb
 *
 *	Abstract:	This routine updates the GMDB data in the community
 *			node.  It uses compare and store quad to accomplish
 *			the store.  The console does not interpret this data
 *			which is used to control access to a communication
 *			region used by community members.
 *
 *	Inputs:		community	- HANDLE of the community.
 *			offset		- The quadword index
 *			old_value	- Old value
 *			new_value	- New value
 *
 *	Outputs: 	int32
 *
 *			SUCCESS		- Success!
 *			BADPARAM	- The old_value did not match the
 *					  contents of the tree
 *
 *	Side Effects:
 *
 */

int32
gct$update_gmdb( GCT_HANDLE community, int32 offset, uint64 new_data, uint64 old_data) {

  int32 status;
  GCT_COMMUNITY_NODE *pNode;

  _GCT__LOCK_TREE(LOCK_FOR_UPDATE, GCT__INTERNAL_ID, UPDATE_GMDB, GCT__RETRY_MAX);

  status = gct__validate_handle(community);

  if (status != SUCCESS) {
    gct__unlock_tree(UNLOCK_NO_UPDATE, GCT__INTERNAL_ID);
    return (status);
  }

  pNode = (GCT_COMMUNITY_NODE *) _GCT_MAKE_ADDRESS(community);

  status = __CMP_STORE_QUAD(&pNode->gmdb[offset], old_data, new_data, &pNode->gmdb[offset]);

  if (status != 1) {
    gct__unlock_tree(UNLOCK_NO_UPDATE, GCT__INTERNAL_ID);
    return (BADPARAM);
  }
  else {
    gct__unlock_tree(UNLOCK_UPDATE, GCT__INTERNAL_ID);
    return (SUCCESS);
  }
}

/*
 *	Routine:	gct$update_galaxy_id
 *
 *	Abstract:	This routine updates or simply returns the Galaxy ID.
 *			The ID is a 16-byte value that is unique to the system
 *			
 *			To update the ID, supply a pointer to a 16-byte string
 *			that represents the old value, and a 16-byte string that
 *			is the new value.  If the old value matches the stored
 *			value, the new value will be written.
 *
 *			To read the existing value, supply a null pointer to the
 *			old value, and a pointer to a 16 byte buffer for the
 *			new value.
 *
 *	Inputs:		old_value	- Pointer to Old value
 *			new_value	- Pointer to New value
 *
 *	Outputs: 	int32
 *
 *			SUCCESS		- Success!
 *			NOTFOUND	- The old_value did not match the
 *					  contents of the tree
 *
 *	Side Effects:
 *
 */

int32
gct$update_galaxy_id( char *new_data, char *old_data) {

  int64 j;
  GCT_ROOT_NODE     *pRoot;

  pRoot = (GCT_ROOT_NODE *)  _GCT_MAKE_ADDRESS (0);

  if (new_data == 0) return (BADPARAM);

 /*
  *   If the old_data is not provided, just return the
  *   current string
  */
  if (old_data == 0) {

    _GCT__LOCK_TREE(LOCK_FOR_READ, GCT__INTERNAL_ID, UPDATE_GALAXY_ID, GCT__RETRY_MAX);

    for (j = 0; j < GALAXY_ID_LENGTH; j += 1) {
      new_data[j] = pRoot->galaxy_id[j];
    }

    gct__unlock_tree(UNLOCK_READ, GCT__INTERNAL_ID);
    return (SUCCESS);
  }

  _GCT__LOCK_TREE(LOCK_FOR_UPDATE, GCT__INTERNAL_ID, UPDATE_GALAXY_ID, GCT__RETRY_MAX);

 /*
  *   Validate the ID
  */
  for (j = 0; j < GALAXY_ID_LENGTH; j += 1) {
    if (old_data[j] != pRoot->galaxy_id[j]) {
      gct__unlock_tree(UNLOCK_NO_UPDATE, GCT__INTERNAL_ID);
      return (NOTFOUND);
    }
  }

 /*
  *   Replace the ID
  */
  for (j = 0; j < GALAXY_ID_LENGTH; j += 1) {
    pRoot->galaxy_id[j] = new_data[j];
  }

  gct__unlock_tree(UNLOCK_UPDATE, GCT__INTERNAL_ID);
  return (SUCCESS);

}

/*
 *	Routine:	gct$update_instance_name
 *
 *	Abstract:	This routine updates or simply returns the Instance
 *			name.  The name is a 127-byte null terminated ASCIZ
 *			string set by the O/S at runtime.
 *			
 *			To update the name, supply a pointer to an ASCIZ string
 *			that represents to the old value, and an ASCIZ string
 *			that is the new value.  If the old value matches the
 *			stored value, the new value will be written.
 *
 *			To read the existing value, supply a null pointer to the
 *			old value, and a pointer to a 128-byte buffer for the
 *			new value.
 *
 *	Inputs:		old_value	- Pointer to Old value
 *			new_value	- Pointer to New value
 *
 *	Outputs: 	int32
 *
 *			SUCCESS		- Success!
 *			NOTFOUND	- The old_value did not match the
 *					  contents of the tree
 *
 *	Side Effects:
 *
 */

int32
gct$update_instance_name( GCT_HANDLE partition, char *new_data, char *old_data) {

  int32 status;
  int64 j;
  GCT_PARTITION_NODE *pNode;

  _GCT__LOCK_TREE(LOCK_FOR_UPDATE, GCT__INTERNAL_ID, UPDATE_INSTANCE_NAME, GCT__RETRY_MAX);

  status = gct__validate_handle(partition);

  if (status != SUCCESS) {
    gct__unlock_tree(UNLOCK_NO_UPDATE, GCT__INTERNAL_ID);
    return (status);
  }

  pNode = (GCT_PARTITION_NODE *) _GCT_MAKE_ADDRESS(partition);

  if (new_data == 0) return (BADPARAM);

 /*
  *   If the old_data is not provided, just return the
  *   current string
  */
  if (old_data == 0) {

    _GCT__LOCK_TREE(LOCK_FOR_READ, GCT__INTERNAL_ID, UPDATE_GALAXY_ID, GCT__RETRY_MAX);

    for (j = 0; j < INSTANCE_NAME_LENGTH; j += 1) {
      new_data[j] = pNode->instance_name[j];
      if (new_data[j] == 0) break;
    }

    gct__unlock_tree(UNLOCK_READ, GCT__INTERNAL_ID);
    return (SUCCESS);
  }

  _GCT__LOCK_TREE(LOCK_FOR_UPDATE, GCT__INTERNAL_ID, UPDATE_GALAXY_ID, GCT__RETRY_MAX);

 /*
  *   Validate the name
  */
  for (j = 0; j < INSTANCE_NAME_LENGTH; j += 1) {
    if (old_data[j] != pNode->instance_name[j]) {
      gct__unlock_tree(UNLOCK_NO_UPDATE, GCT__INTERNAL_ID);
      return (NOTFOUND);
    }
    if (old_data[j] == 0) break;
  }

 /*
  *   Replace the name
  */
  for (j = 0; j < INSTANCE_NAME_LENGTH; j += 1) {
    pNode->instance_name[j] = new_data[j];
    if (new_data[j] == 0) break;
  }

  gct__unlock_tree(UNLOCK_UPDATE, GCT__INTERNAL_ID);
  return (SUCCESS);

}

#if !GCT6
/*
 *	Routine:	gct__create_mem_desc
 *
 *	Abstract:	Creates a memory descriptor
 *
 *	Inputs:		nodeType	- node type
 *
 *	Outputs:	Handle of the memory descriptor
 *
 *	Side Effects:
 *
 */

GCT_HANDLE gct__create_mem_desc(char nodeType, char nodeSubType, unsigned __int64 flags, int extra_size, void *type_specific) {

  GCT_MEM_DESC_NODE *pDesc;
  GCT_MEM_DESC      *pFrag;
  GCT_HANDLE        node;
  GCT_ROOT_NODE     *pRoot;
  int desc_size;
  unsigned __int64 *MemData = (unsigned __int64 *) type_specific;


  pRoot   = (GCT_ROOT_NODE *) _GCT_MAKE_ADDRESS (0);

 /*
  *  The memory descriptor array (fragments) are dynamically sized
  *  depending on the platform data.  Add 8 bytes to allow us to
  *  insure quadword alignment.
  */
  desc_size = (pRoot->max_fragments * sizeof(GCT_MEM_DESC)) + 8;

 /*
  *    Create the node
  */
  node  = gct__alloc_node(nodeType, nodeSubType,flags, desc_size, 0);
  pDesc = (GCT_MEM_DESC_NODE *) _GCT_MAKE_ADDRESS(node);

 /*
  *  Set the offset to the memory descriptor array.  Quadword align it.
  */
  pDesc->mem_frag = ((gct_node_info[nodeType].size + 7) & ~7);
  pFrag = (GCT_MEM_DESC *) ((char *) pDesc + pDesc->mem_frag);

 /*
  *   Get the additional data
  */
  if (type_specific) {

    pDesc->mem_info.base_pa       = MemData[0];
    pDesc->mem_info.base_size     = MemData[1];
    pDesc->bitmap_pa = MemData[2];
    pDesc->mem_info.desc_count	  = 1;

    pFrag[0].mem_owner  = 0;
    pFrag[0].mem_current_owner = 0;
    pFrag[0].mem_flags  = 0;
    pFrag[0].pa         = MemData[0];
    pFrag[0].size       = MemData[1];

   /*
    *   Keep the current size in the root
    */
    if (flags & NODE_HARDWARE)
      pRoot->mem_size += MemData[1];

  }

 /*
  *  If this is a template node, init the required memory
  *
  */
  if (flags & NODE_HW_TEMPLATE) {

    pFrag[0].mem_owner     = 0;
    pFrag[0].mem_current_owner = 0;
    pFrag[0].mem_flags = MEM_PRIVATE | MEM_BASE | MEM_CONSOLE;
    pFrag[0].pa        = MemData[0];
    pFrag[0].size      = pRoot->console_req;

    pFrag[1].mem_owner     = 0;
    pFrag[1].mem_current_owner  = 0;
    pFrag[1].mem_flags = MEM_PRIVATE | MEM_BASE;
    pFrag[1].pa        = MemData[0] + pRoot->console_req;
    pFrag[1].size      = MemData[1] - pRoot->console_req;

    pDesc->mem_info.desc_count = 2;

  }

  return (node);
}

#else    /* else GCT6 style */

/*
 *	Routine:	gct__create_mem_desc
 *
 *	Abstract:	Creates a memory descriptor
 *
 *	Inputs:		nodeType	- node type
 *
 *	Outputs:	Handle of the memory descriptor
 *
 *	Side Effects:
 *
 */

GCT_HANDLE gct__create_mem_desc(char nodeType, char nodeSubType, unsigned __int64 flags, int extra_size, void *type_specific) {
  GCT_MEM_DESC_NODE *pDesc;
  CFG_MCD           *pElement;
  GCT_HANDLE        node;
  GCT_ROOT_NODE     *pRoot;
  int desc_size;
  unsigned __int64 *MemData = (unsigned __int64 *) type_specific;


  pRoot   = (GCT_ROOT_NODE *) _GCT_MAKE_ADDRESS (0);

 /*
  *  Add 8 bytes to allow us to
  *  insure quadword alignment.
  */
  desc_size = sizeof(GCT_MEM_DESC_NODE) + 8;

 /*
  *    Create the node
  */
  node  = gct__alloc_node(nodeType, nodeSubType,flags, desc_size, 0);

  pDesc = (GCT_MEM_DESC_NODE *) _GCT_MAKE_ADDRESS(node);

 /*
  *   Get the additional data
  */
  if (type_specific) { 

    pDesc->mem_cluster_desc.mcd_start_pfn = MemData[0] / HWRPB$_PAGESIZE;
    pDesc->mem_cluster_desc.mcd_pfn_count = MemData[1] / HWRPB$_PAGESIZE;
    pDesc->mem_cluster_desc.mcd_bitmap_pa = MemData[2] / HWRPB$_PAGESIZE;

    pDesc->mem_cluster_desc.mcd_tested_pfn_count = MemData[4];
    pDesc->mem_cluster_desc.mcd_usage_flags = MemData[5];

   /*
    *   Keep the current size in the root
    */
    if (flags & NODE_HARDWARE)
      pRoot->mem_size += MemData[1];

   }

  return (node);
}

#endif   /* end if gct6  */


/*
 *	Routine:	gct__create_mem_sub
 *
 */

GCT_HANDLE gct__create_mem_sub(char nodeType, char nodeSubType, unsigned __int64 flags, int extra_size, void *type_specific) {

  GCT_ROOT_NODE       *pRoot;
  GCT_MEMORY_SUB_NODE *pNode; 

  GCT_HANDLE node;
  unsigned __int64 *mpa = (unsigned __int64 *) type_specific;

  pRoot = (GCT_ROOT_NODE *) _GCT_MAKE_ADDRESS (0);

 /*
  *    Create the node
  */
  node  = gct__alloc_node(nodeType, nodeSubType, flags, 0, 0);

  pNode = (GCT_MEMORY_SUB_NODE *) _GCT_MAKE_ADDRESS (node);
  pNode->min_pa = mpa[0];
  pNode->max_pa = mpa[1];

#if (TURBO || RAWHIDE )
  if (pRoot->max_phys_address < mpa[1]) pRoot->max_phys_address = mpa[1];
#endif

  return (node);
}



/*
 *	Routine:	gct__validate_handle
 *
 *	Abstract:
 *
 *	  This routine validates a handle is a real node, and that
 *	  it is available.
 *
 *	Inputs:	handle		- The handle to validate
 *
 *	Outputs:		SUCCESS
 *				BADHANDLE	- Invalid handle
 *				UNAVAILABLE	- Node is UNAVAILABLE
 *
 *	Side Effects:
 *
 */

#pragma inline gct__validate_handle

int gct__validate_handle(GCT_HANDLE handle) {
  GCT_ROOT_NODE *pRoot;
  GCT_NODE *pNode;

  pRoot = (GCT_ROOT_NODE *) _GCT_MAKE_ADDRESS(0);

 /*
  *  Check the address to make sure that it is within the
  *  memory constraints.
  */
  if ((handle >= pRoot->high_limit) || (handle < 0)) {
	if (tree_debug) printf("Bad Handle range\n");
	return (BADHANDLE);
  }

  pNode = _GCT_MAKE_ADDRESS(handle);

 /*
  *  Now check the "magic" string to make sure it is a node
  */
  if (pNode->magic != NODE_VALID) {
	if (tree_debug) printf("No Magic Marker\n");
	return (BADHANDLE);
  }

 /*
  *  If the node is "unavailable" and it's not a GCT_FRU_DESC node
  *  then fail the check as well
  */
  if ( ( pNode->node_unavailable ) && ( pNode->type != NODE_FRU_DESC ) ) {
	if (tree_debug) printf("Node Unavailable\n");
	return (UNAVAILABLE);
  }

  return (SUCCESS);
}

/*
 *	Routine:	gct__create_iop
 *
 */

GCT_HANDLE gct__create_iop(char nodeType, char nodeSubType, unsigned __int64 flags, int extra_size, void *type_specific) {

  GCT_ROOT_NODE       *pRoot;
  GCT_IOP_NODE *pNode; 

  GCT_HANDLE node;
  unsigned __int64 *mpa = (unsigned __int64 *) type_specific;

  pRoot = (GCT_ROOT_NODE *) _GCT_MAKE_ADDRESS (0);

 /*
  *    Create the node
  */
  node  = gct__alloc_node(nodeType, nodeSubType, flags, 0, 0);

  pNode = (GCT_IOP_NODE *) _GCT_MAKE_ADDRESS (node);
  pNode->min_io_pa = mpa[0];
  pNode->max_io_pa = mpa[1];

  return (node);
}





/*
 *	Routine:	gct__lookup_node
 *
 *	Abstract:
 *
 *	  This routine searches the configuration tree, starting at a specified
 *	  node in the tree, for the first occurance of a node matching the
 *	  search criteria.  Searches may be for any node, for nodes of a specific
 *	  type, nodes owned by a specific instance (or unassigned), or for both
 *	  instance and type.
 *	
 *	  The routine returns the GCT_HANDLE of the matching node, or zero (the
 *	  handle of the root) if no match occurs.  If the routine is repeatedly
 *	  called with the handle returned from the previous call, the entire
 *	  tree will be traversed.
 *	
 *	  The configuration tree is an unbalanced tree (i.e. the first child
 *	  contains no previous sibling) that can be searched by walking down
 *	  the child chain, back up and across the sibling chain, also walking
 *	  down each siblings child chain before walking back up.
 *	
 *	  The tree is searched downward from the starting node (not including
 *	  the starting node).  At each child the match criteria are tested.
 *	  If a match is not made, then the next child is searched.  When the
 *	  last child is reached, the tree is traversed back to the parent
 *	  (which has already been tested) and then to the next sibling.  If
 *	  a sibling is found, it is tested.  The search continues down the
 *	  siblings child pointers and so on.  The search would result in the
 *	  tree returning the following on a match-all search"
 *	
 *	     A
 *	     |
 *	     +----------+----------+
 *	     |          |          |
 *	     B          C          D
 *	     |          |          |
 *	     +---+---+  +---+      +---+---+---+
 *	     |   |   |  |   |      |   |   |   |
 *	     E   F   G  H   I      J   K   L   M
 *	     |              |          |       |
 *	     +---+          +---+      +---+   +---+---+
 *	     |   |          |   |      |   |   |   |   |
 *	     N   O          P   Q      R   S   T   U   V
 *	         |                                 |
 *	         W                                 X
 *	
 *	
 *	  As:  A,B,E,N,O,W,F,G,C,H,I,P,Q,D,J,K,R,S,L,M,T,U,X,V
 *	
 *	Inputs:		root		- This node serves as the root for
 *					  the search.  The search will not
 *					  go "above" this node.
 *			current		- the node to start the search at.
 *			search_criteria	- A bitmask indicating which fields
 *					  are significant in the match.
 *			search_type	- The TYPE of node to search for
 *			search_subType	- The SUBTYPE of the node to search for
 *			search_id	- The ID of the node to search for
 *			search_owner	- The OWNER field of the node to search for
 *
 *	Outputs:	GCT_HANDLE	- The ID of the node that matches, or
 *					  zero.
 *
 *	Side Effects:
 *
 */

GCT_HANDLE gct__lookup_node( GCT_HANDLE root, GCT_HANDLE current,
    int search_criteria, char search_type, char search_subType,
    GCT_ID search_id, GCT_HANDLE search_owner, int *depth)
{

  GCT_NODE *pNode;
  GCT_HANDLE nodeHandle;
  int    match_result = 0;
  int    local_depth = 0;

 /*
  *  If current is 0, make it default to root.
  */
  if (!current) {
    current = root;
  }

 /*
  *  Get the address of the starting node.
  */
  nodeHandle = current;
  pNode = _GCT_MAKE_ADDRESS( nodeHandle);

 /*
  *  If there are no children, just return.
  */
  if (root == nodeHandle && !pNode->child) {
    return (0);
  }

 /*
  *  Do the search.  Always ignore the starting node.
  */
  while (1) {
    if (pNode->child) {
      nodeHandle = pNode->child;
      pNode = _GCT_MAKE_ADDRESS( nodeHandle);
      local_depth += 1;
    }
    else {
      if (pNode->next_sib) {
        nodeHandle = pNode->next_sib;
        pNode = _GCT_MAKE_ADDRESS( nodeHandle);
      }
      else {

       /*
        *  We have reached the end of a branch following the
        *   child and sibling pointers.  It's time to go back
        *   up the tree and look for the next sibling of the
        *   parent
        */
        while (nodeHandle = pNode->prev_sib) {

         /*
          *  Handle the search where the root has siblings.
          *  Don't go *directly* back to the parent when we
          *  hit the end of the child/next_sib chain, instead
          *  loop back through the prev_sib and make sure that
          *  one of *these* is not the root.  End our search
          *  if it is, otherwise we will go up one too many
          *  levels.
          *
          */
          if (nodeHandle == root) {
           if (depth) *depth = local_depth;
            return (0);
          }

          pNode = _GCT_MAKE_ADDRESS( nodeHandle);

        }

        while (nodeHandle = pNode->parent) {

          local_depth -= 1;

         /*
          *  We have reached the root.  We're done.
          */
          if (nodeHandle == root) {
           if (depth) *depth = local_depth;
            return (0);
          }

          pNode = _GCT_MAKE_ADDRESS( nodeHandle);

          if (pNode->next_sib) {
            nodeHandle = pNode->next_sib;
            pNode = _GCT_MAKE_ADDRESS( nodeHandle);
            break;
          }
        }

       /*
        *  We have reached the real root.  We're done.
        */
        if (nodeHandle == 0) {
          return (0);
        }
      }
    }

   /*
    *  We have a new node, see if it's a match
    */

    if (!search_criteria) {
      if (depth) *depth = local_depth;
      return (nodeHandle);
    }

    match_result = 0;

    if ((search_criteria & FIND_BY_ID) && (pNode->id == search_id))
        match_result |= FIND_BY_ID;

    if ((search_criteria & FIND_BY_TYPE) && (pNode->type == search_type))
        match_result |= FIND_BY_TYPE;

    if ((search_criteria & FIND_BY_SUBTYPE) && (pNode->subtype == search_subType))
        match_result |= FIND_BY_SUBTYPE;

    if ((search_criteria & FIND_BY_OWNER) && (pNode->owner == search_owner))
        match_result |= FIND_BY_OWNER;

   /*
    *  Check to see if the node is unavailable.  Only return a node that
    *  is unavailable if explicitly asked.
    */
    if (pNode->node_unavailable) {
      if (search_criteria & FIND_UNAVAILABLE) match_result |= FIND_UNAVAILABLE;
      else match_result = ~search_criteria;
    }

    if (tree_debug == 4)
      printf("  - Result of match = 0x%x, criteria = 0x%x, match = 0x%x   %x %x %x\n", 
      match_result, search_criteria, match_result == search_criteria,search_type,search_subType,search_id);

    if (match_result == search_criteria) {
      if (depth) *depth = local_depth;
      return (nodeHandle);
    }
  }
}


#if !GCT6
/*
 *	Routine:	gct__mem_frag_init
 *
 *	Abstract:	Initialize memory fragements.  When we are building the
 *			Galaxy from a set of tables, each node will be looked up
 *			and initialized using the node-specific information block.
 *			This is the memory fragment initialization.
 *
 *			Note that the init needs to be called in this case AFTER
 *			all partitions and communities are created.  Since a memory
 *			node may be subdivided among multiple sw entities.
 *
 *		NOTE:   The routine is not yet checking the sanity of the
 *			fragments.  It will take anything that is given to
 *			it!
 *
 *	Inputs:		node		- Handle of the node to init
 *			type_specific	- Pointer to the fragment description
 *
 *	Outputs:	int		  1 - Success
 *					  * - Not yet doing checking: always success.
 *
 *	Side Effects:	
 *
 *			Memory fragment is made activer (current_owner is set)
 */

int gct__mem_frag_init( GCT_HANDLE node, GCT_HANDLE community, GCT_HANDLE partition, void *type_specific) {

  int status;
  GCT_HANDLE owner_node;
  GCT_MEM_DESC_NODE *pDesc;
  GCT_MEM_FRAG_DEFS *frags;

  frags = (GCT_MEM_FRAG_DEFS *) type_specific;
  pDesc = (GCT_MEM_DESC_NODE *) _GCT_MAKE_ADDRESS(node);

 /*
  *    Get the partition/community ID.  Note that if the descriptor is owned by the
  *    community, the fragments could be any partition in the community, or the
  *    community itself.
  */
  owner_node = gct__lookup_node( 0, 0, (FIND_BY_TYPE | FIND_BY_ID), (char) frags->type, 0, frags->id, 0, 0);

  gct__assign_fragment( node, owner_node, frags->flags, frags->pa, frags->size, 1);

  return (SUCCESS);
}

/*
 *	Routine:	gct__assign_fragment
 *
 *	Abstract:	This is one of the most complicated calls.
 *			It assigns a owner to a memory fragment.
 *
 *			A memory descriptor node has an array of
 *			fragments.  The memory range of the descriptor
 *			can be broken into a number of fragments
 *			(the number of fragments is platform dependent
 *			and is available as a parameter in the node).
 *			Each "fragment" describes a PA and size, as well
 *			as flags and the owner handle.
 *
 *			If the memory descriptor node is owner by a
 *			partition, then fragments may be only owned
 *			by the partition, or unowned.
 *
 *			If the memory descriptor is owned by a community,
 *			then the memory fragments can be owned by the
 *			community (i.e. shared) or by any partition in the
 *			community, or unowned.
 *
 *			When the descriptor has > 0 fragments, the fragments
 *			describe the entire memory descriptor.  Any unowned
 *			memory is also contained as fragments.
 *
 *			To assign a fragment to an owner, there must be a
 *			fragment that contains the entire requested base and
 *			size, and the fragment MUST be UNOWNED.  If there are
 *			no fragments, then initial fragments are created as if
 *			there had been a single fragment covering the entire
 *			descriptor.
 *
 *			If the requested fragment is smaller than the unowned
 *			fragment being requested, then the fragment is spit into
 *			multiple fragments.
 *
 *			This routine does not validate the inputs, use the public
 *			interface for validation.
 *
 *	Inputs:		node		- Memory descriptor node
 *			PA		- Physical address of the fragment
 *			size		- size of the fragment
 *
 *	Outputs:	int32
 *
 *			SUCCESS		- Success!
 *			OVERLAP		- Memory overlaps with existing fragments
 *
 *	Side Effects:
 *
 */

int gct__assign_fragment( GCT_HANDLE node, GCT_HANDLE newFragOwner,
unsigned int flags, unsigned __int64 pa, unsigned __int64 size, unsigned int active) {

  int  status, sh_type, numFrag, j, k, valid = 0;
  GCT_NODE          *pNode, *pNodeOwner, *pNewFragOwner;
  GCT_MEM_DESC      *pFrag;
  GCT_MEM_DESC_NODE *pDesc;
  GCT_ROOT_NODE *pRoot;
  GCT_MEM_DESC tempMem[MAX_FRAGMENTS];

  if (tree_debug) {
  printf("-------\nAssign memory fragment: node %X, owner %X, pa %LX, size %LX, flags %x\n", node, newFragOwner, pa, size, flags);
  }

  pNode         = _GCT_MAKE_ADDRESS(node);
  pNodeOwner    = _GCT_MAKE_ADDRESS(pNode->owner);
  pNewFragOwner = _GCT_MAKE_ADDRESS(newFragOwner);
  pRoot         = (GCT_ROOT_NODE *) _GCT_MAKE_ADDRESS(0);

  pDesc         = (GCT_MEM_DESC_NODE *) pNode;
  numFrag       = pDesc->mem_info.desc_count;
  pFrag         = (GCT_MEM_DESC *) ((char *) pDesc + pDesc->mem_frag);

  if (tree_debug) {

    printf("  %d current fragments.\n", pDesc->mem_info.desc_count );
    printf("  Descriptor has memory from PA 0x%LX, to 0x%LX\n",
         pDesc->mem_info.base_pa,
         (unsigned __int64) (pDesc->mem_info.base_pa + pDesc->mem_info.base_size));

    j = 0;
    while (j < pDesc->mem_info.desc_count) {
      printf("  - fragment %d, owner 0x%4.4X, current owner 0x%4.4X, flags 0x%4.4X, PA 0x%LX, size 0x%LX\n",
              j,
              pFrag[j].mem_owner,
              pFrag[j].mem_current_owner,
              pFrag[j].mem_flags,
              pFrag[j].pa,
              pFrag[j].size);
      j += 1;
    }

    if (pDesc->mem_info.desc_count) {
      printf("  Last fragment ends at PA 0x%LX\n",
         (pFrag[pDesc->mem_info.desc_count-1].pa + pFrag[pDesc->mem_info.desc_count-1].size));
      }
  }


 /*
  *    Set the flags for the fragment
  */
  if (pNewFragOwner->type == NODE_PARTITION) sh_type = MEM_PRIVATE;
  else if (pNewFragOwner->type == NODE_COMMUNITY) sh_type = MEM_SHARED;

 /*    If there are no fragments, then the memory is free to be carved up
  *    In the end, there will be 1-3 fragments, depending on where the PA
  *    is located.
  */
  if (numFrag == 0) {

  if (tree_debug)
    printf("  Create initial fragments.\n");

    j = 0;

    if (pa > pDesc->mem_info.base_pa) {
      pFrag[j].mem_owner = 0;
      pFrag[j].mem_current_owner = 0;
      pFrag[j].mem_flags = 0;
      pFrag[j].pa    = pDesc->mem_info.base_pa;
      pFrag[j].size  = pa - pDesc->mem_info.base_pa;
      pDesc->mem_info.desc_count += 1;
      j += 1;
    }

    pFrag[j].mem_owner = newFragOwner;
    pFrag[j].mem_current_owner = (active ? newFragOwner : 0);
    pFrag[j].mem_flags = (flags & ~(MEM_PRIVATE | MEM_SHARED)) | sh_type;
    pFrag[j].pa    = pa;
    pFrag[j].size  = size;
    pDesc->mem_info.desc_count += 1;
    j += 1;

    if ((pa + size) < (pDesc->mem_info.base_pa + pDesc->mem_info.base_size)) {
      pFrag[j].mem_owner = 0;
      pFrag[j].mem_current_owner = 0;
      pFrag[j].mem_flags = 0;
      pFrag[j].pa    = pa + size;
      pFrag[j].size  = (pDesc->mem_info.base_pa + pDesc->mem_info.base_size) - (pa + size);
      pDesc->mem_info.desc_count += 1;
    }

  if (tree_debug)
    printf("  Done!\n");

    return (SUCCESS);

  }

 /*    Look for the fragment it fits into.  If it doesn't, then there is an overlap
  *    and we will fall thru with that error.  If we find a fragment it fits into
  *    that is unowned, then we will gobble it.  Note that we create a new fragment
  *    list in local memory that we will copy back into it - if we succeed and we
  *    do not exceed the fragment threshold.
  */

  if (tree_debug)
    printf("  Find unowned fragment that new fragment fits into\n");

  k = 0;

  for (j = 0; j < pDesc->mem_info.desc_count; j += 1) {

  if (tree_debug) {
      printf("  Checking fragment PA 0x%LX, ends at PA 0x%LX\n",
         pFrag[j].pa,
         (pFrag[j].pa + pFrag[j].size));
  }

   /*
    *   Does the new fragment fit into this one?
    */
    if ((pFrag[j].pa <= pa) && (pFrag[j].pa + pFrag[j].size >= (pa + size))) {

     /*
      *   Yes!  We have found a fragment that the request will fit into.  It must be unowned!                  
      */

  if (tree_debug)
    printf("  Found fragment.\n");

      valid = 1;

      if (pFrag[j].mem_owner != 0) {
        return (ALREADYOWNED);
      }
      else {

       /*
        *    We need to create new fragment(s).  Depending on the alignment and sizes.
        */

  if (tree_debug)
    printf("  Creating new fragment list in local memory\n");

        if (pa > pFrag[j].pa) {
          tempMem[k].mem_owner = 0;
          tempMem[k].mem_current_owner = 0;
          tempMem[k].mem_flags = 0;
          tempMem[k].pa    = pFrag[j].pa;
          tempMem[k].size  = pa - pFrag[j].pa;
          k += 1;

  if (tree_debug)
    printf("  Inserted unowned leading fragment\n");
        }

        tempMem[k].mem_owner = newFragOwner;
        tempMem[k].mem_current_owner = newFragOwner;
        tempMem[k].mem_flags = (flags & ~(MEM_PRIVATE | MEM_SHARED)) | sh_type;
        tempMem[k].pa    = pa;
        tempMem[k].size  = size;
        k += 1;


  if (tree_debug)
    printf("  Inserted new fragment\n");

 
        if ((pa + size) < (pFrag[j].pa + pFrag[j].size)) {
          tempMem[k].mem_owner = 0;
          tempMem[k].mem_current_owner = 0;
          tempMem[k].mem_flags = 0;
          tempMem[k].pa    = pa + size;
          tempMem[k].size  = (pFrag[j].pa + pFrag[j].size) - (pa + size);
          k += 1;

  if ( tree_debug)
    printf("  Inserted unowned trailing fragment\n");

        }
      }
    }
    else {
      tempMem[k].mem_owner = pFrag[j].mem_owner;
      tempMem[k].mem_current_owner = pFrag[j].mem_current_owner;
      tempMem[k].mem_flags = pFrag[j].mem_flags;
      tempMem[k].pa    = pFrag[j].pa;
      tempMem[k].size  = pFrag[j].size;
      k += 1;

  if (tree_debug)
    printf("  Copied existing fragment\n");

    }
  }


 /*
  *    If (valid) that means we created new fragments, otherwise
  *    we fail at the end with an overlap!
  */

  if (valid) {
    if (j < pRoot->max_fragments) {

  if (tree_debug)
    printf("  Coping local memory fragments to node.  %d fragments\n", k);


      for (j = 0; j < k; j += 1) {
        pFrag[j].mem_owner = tempMem[j].mem_owner;
        pFrag[j].mem_current_owner = tempMem[j].mem_current_owner;
        pFrag[j].mem_flags = tempMem[j].mem_flags;
        pFrag[j].pa    = tempMem[j].pa;
        pFrag[j].size  = tempMem[j].size;
      }
      pDesc->mem_info.desc_count = k;
    }
    return (SUCCESS);
  }

  return (OVERLAP);
}

/*
 *	Routine:	gct__deassign_fragment
 *
 *	Abstract:	This is a lot simpler than the assignment!
 *			It will walk through the fragments, find the
 *			one being deleted (validation should is done
 *			in the public interface) and then it will
 *			make it unowned.  If the fragment before it
 *			is unowned, then they will be collapsed
 *
 *	Inputs:		node		- Memory descriptor node
 *			PA		- Physical address of the fragment
 *
 *	Outputs:	int32
 *
 *			SUCCESS		- Success!  Always!
 *
 *	Side Effects:
 *
 */

int gct__deassign_fragment( GCT_HANDLE node, unsigned __int64 pa, unsigned int active) {

  int  j, k;
  GCT_NODE          *pNode;
  GCT_MEM_DESC      *pFrag;
  GCT_MEM_DESC_NODE *pDesc;
  GCT_MEM_DESC tempMem[MAX_FRAGMENTS];

  pNode      = _GCT_MAKE_ADDRESS(node);

  pDesc      = (GCT_MEM_DESC_NODE *) pNode;
  pFrag      = (GCT_MEM_DESC *) ((char *) pDesc + pDesc->mem_frag);

 /*
  *   Find the fragment.  Copy into the local memory array as we go.
  */
  for (j = 0, k = 0; j < pDesc->mem_info.desc_count; j += 1) {

   /*
    *   Copy the fragment
    */     
    tempMem[k].mem_owner = pFrag[j].mem_owner;
    tempMem[k].mem_current_owner = pFrag[j].mem_current_owner;
    tempMem[k].mem_flags = pFrag[j].mem_flags;
    tempMem[k].pa    = pFrag[j].pa;
    tempMem[k].size  = pFrag[j].size;

   /*
    *   Is this is the fragment?
    */
    if (pFrag[j].pa == pa) {

    /*
     *  If the memory must remain active, then just clear
     *  the owner field and return
     *
     */
     if (active) {
       pFrag[j].mem_owner = 0;
       return (SUCCESS);
     }

     /*
      *   Zero the owner and flags
      */
      tempMem[k].mem_owner = 0;
      tempMem[k].mem_current_owner = 0;
      tempMem[k].mem_flags = 0;
    }

   /*
    *    See if we can collapse this fragment
    */
    if ((k) && ((tempMem[k-1].mem_owner == 0) && (tempMem[k].mem_owner == 0))) {

     /*
      *   Yup!  Back up one in the local copy and add our size to it.
      */
      k -= 1;
      tempMem[k].size  += tempMem[k+1].size;
    }

    k += 1;

  }

 /*
  *   Copy the local memory to the fragment
  */
  for (j = 0; j <k; j += 1) {
    pFrag[j].mem_owner = tempMem[j].mem_owner;
    pFrag[j].mem_current_owner = tempMem[j].mem_current_owner;
    pFrag[j].mem_flags = tempMem[j].mem_flags;
    pFrag[j].pa    = tempMem[j].pa;
    pFrag[j].size  = tempMem[j].size;
  }

 /*
  *   Update the description count
  */
  pDesc->mem_info.desc_count = k;

  return(SUCCESS);
}

#else   /* GCT6 */

int gct__mem_frag_init( GCT_HANDLE node, GCT_HANDLE community, GCT_HANDLE partition, void *type_specific) {
	return (SUCCESS);  }
int gct__assign_fragment( GCT_HANDLE node, GCT_HANDLE newFragOwner,
unsigned int flags, unsigned __int64 pa, unsigned __int64 size, unsigned int active) {
	return (SUCCESS);  }
int gct__deassign_fragment( GCT_HANDLE node, unsigned __int64 pa, unsigned int active) {
	return (SUCCESS);  }

#endif   /* end if !gct6 */


void galaxy_set_run_bit(int run) {
  GCT_HANDLE        hw_root, cpu, sw_root, partition;
  GCT_ROOT_NODE *pRoot;
  GCT_NODE *pCPU;
  UINT addr,tree;
  char *buf;
  UINT saveflags;
  GCT_NODE_ID_CPU *pNODEID;
  struct HWRPB *hwrpb;

#if (RAWHIDE || TURBO)
  tree = *(UINT *) 0x80;

#else

  hwrpb = HWRPB;
  if (hwrpb && hwrpb->RSVD_HW[0])
    tree = hwrpb->RSVD_HW[0];
  else
    return;	/* no tree yet */
#endif

#if !WILDFIRE
  if (galaxy_node_number && !lp_hard_partition) {
	buf = dyn$_malloc(config_size,DYN$K_ALIGN,0x2000,0,0);
	addr = virt_to_phys(buf);
	spinlock(&spl_kernel);		/* safely use a new gct_root */
	memcpy64(addr,tree,config_size);
	gct_root = buf + GCT__BUFFER_HEADER_SIZE;
  }
#endif
  hw_root = gct__lookup_node( 0, 0, FIND_BY_TYPE, NODE_HW_ROOT, 0, 0, 0, 0);

  /* search through the nodes, beginning at the hw_root, to find */
  /* the cpu whos ID matches this CPU */
  cpu = hw_root; /* begin the search */
  while (cpu) {
    cpu = gct__lookup_node( hw_root, cpu, 
		(FIND_BY_TYPE), NODE_CPU, 0, 0, 0, 0);
    if (cpu) {
      pCPU = _GCT_MAKE_ADDRESS (cpu);
      pNODEID = &(pCPU->id);
      if (pNODEID->node_id_cpu_cpu == whoami())
	break;	/* found it */

    } else {	/* exhausted all nodes */
#if !WILDFIRE
	if (galaxy_node_number && !lp_hard_partition) {
	    spinunlock(&spl_kernel);
	    free(buf);
	}
#endif
	qprintf("gct__lookup_node can't find CPU %d\n", whoami());
	return;
    }
  }
  saveflags = pCPU->node_flags;
  pCPU->node_flags &= ~NODE_IN_CONSOLE;
  if (!run) pCPU->node_flags |= NODE_IN_CONSOLE;
  pNODEID->node_id_cpu_revcnt++;  /* bump */

  sw_root = gct__lookup_node( hw_root, 
				hw_root, FIND_BY_TYPE, NODE_SW_ROOT, 
				0, 0, 0, 0);
  partition = gct__lookup_node( sw_root, 
				sw_root, (FIND_BY_TYPE | FIND_BY_ID), 
				NODE_PARTITION, 0, galaxy_node_number, 0, 0);

  if ((saveflags != pCPU->node_flags) || (pCPU->current_owner != partition)){
    pCPU->current_owner = partition;	/* make this partition the owner */
    pRoot = (GCT_ROOT_NODE *)  _GCT_MAKE_ADDRESS (0);
    pRoot->current_level++;
#if !WILDFIRE
    if (galaxy_node_number && !lp_hard_partition) {
      addr = virt_to_phys(&pRoot->current_level);
#if (RAWHIDE || TURBO)
      tree = *(UINT *) 0x80;
#else
      hwrpb = HWRPB;
      tree = hwrpb->RSVD_HW[0];
#endif
      tree += offsetof(GCT_ROOT_NODE,current_level) + GCT__BUFFER_HEADER_SIZE;
      memcpy64(tree,addr,8);
      addr = virt_to_phys(pCPU);
#if (RAWHIDE || TURBO)
      tree = *(UINT *) 0x80;
#else
      hwrpb = HWRPB;
      tree = hwrpb->RSVD_HW[0];
#endif
      tree += GCT__BUFFER_HEADER_SIZE + cpu;
      memcpy64(tree,addr,sizeof(GCT_NODE));
      spinunlock(&spl_kernel);		    /* give up gct_root */
      free(buf);
    }
#endif
    galaxy_increment_current_level();

  } else {				    /* no tree changes */
#if !WILDFIRE
    if (galaxy_node_number && !lp_hard_partition) {	/* check if we own lock */
	spinunlock(&spl_kernel);	    /* give up gct_root */
    }
#endif
  }
}

void galaxy_increment_incarnation() {
  GCT_HANDLE        hw_root, cpu, sw_root, partition;
  GCT_ROOT_NODE *pRoot;
  GCT_NODE *pCPU;
  GCT_PARTITION_NODE *pPARTITION;
  UINT addr,tree;
  char *buf;
  UINT saveflags;
  struct EVNODE evn,*evp=&evn;
  struct HWRPB *hwrpb;

#if (RAWHIDE || TURBO)
  tree = *(UINT *) 0x80;

#else

  hwrpb = HWRPB;
  if (hwrpb && hwrpb->RSVD_HW[0])
    tree = hwrpb->RSVD_HW[0];
  else
    return;	/* no tree yet */
#endif

  ev_read("os_type", &evp, EV$K_SYSTEM);
#if !WILDFIRE
  if (galaxy_node_number && !lp_hard_partition) {
	buf = dyn$_malloc(config_size,DYN$K_ALIGN,0x2000,0,0);
	addr = virt_to_phys(buf);
	spinlock(&spl_kernel);		/* safely use a new gct_root */
	memcpy64(addr,tree,config_size);
	gct_root = buf + GCT__BUFFER_HEADER_SIZE;
  }
#endif
  hw_root = gct__lookup_node( 0, 0, FIND_BY_TYPE, NODE_HW_ROOT, 0, 0, 0, 0);
  sw_root = gct__lookup_node( hw_root, 
				hw_root, FIND_BY_TYPE, NODE_SW_ROOT, 
				0, 0, 0, 0);
  partition = gct__lookup_node( sw_root, 
				sw_root, (FIND_BY_TYPE | FIND_BY_ID), 
				NODE_PARTITION, 0, galaxy_node_number, 0, 0);

  pPARTITION = _GCT_MAKE_ADDRESS(partition);
  pPARTITION->incarnation++;
  pPARTITION->os_type = gct_os_type(evp->value.string);
  pPARTITION->hwrpb = virt_to_phys(HWRPB);

  pRoot = (GCT_ROOT_NODE *)  _GCT_MAKE_ADDRESS (0);
  pRoot->current_level++;
#if !WILDFIRE
  if (galaxy_node_number && !lp_hard_partition) {
      addr = virt_to_phys(&pRoot->current_level);
#if (RAWHIDE || TURBO)
      tree = *(UINT *) 0x80;
#else
      hwrpb = HWRPB;
      tree = hwrpb->RSVD_HW[0];
#endif
      tree += offsetof(GCT_ROOT_NODE,current_level) + GCT__BUFFER_HEADER_SIZE;
      memcpy64(tree,addr,8);
      addr = virt_to_phys(pPARTITION);
#if (RAWHIDE || TURBO)
      tree = *(UINT *) 0x80;
#else
      hwrpb = HWRPB;
      tree = hwrpb->RSVD_HW[0];
#endif
      tree += GCT__BUFFER_HEADER_SIZE + partition;
      memcpy64(tree,addr,sizeof(GCT_PARTITION_NODE));
      spinunlock(&spl_kernel);		    /* give up gct_root */
      free(buf);
  }
#endif
  galaxy_increment_current_level();

}

void galaxy_increment_current_level() {
  int i;
  int cpumask = all_cpu_mask;

#if TURBO
#undef cpu_mask
  if (lp_hard_partition) cpumask = cpu_mask;
#endif

  mb();
  for (i=0;i<MAX_PROCESSOR_ID;i++) {
    if (cpumask & (1<<i))
      mtpr_ipir(i);
  }
}

void dump_tlv(GCT_TLV *pTLV) {
    switch(pTLV->tlv_tag) {
      case 0:
        printf("Not a TLV (tag == 0)\n");
        break;
      case TLV_TAG_ISOLATIN1:
        printf("ISOLatin-1 size %3.3d <%s>\n", strlen((char*)pTLV->tlv_value), pTLV->tlv_value);
        break;
      case TLV_TAG_QUOTED:
        printf("Quoted Value\n");
        break;
      case TLV_TAG_BINARY:
        printf("Binary Data - ");
        switch(pTLV->tlv_length) {
          case 0:
            break;
          case 1:
            break;
          case 2:
            break;
          case 4:
            printf("integer (4 bytes) - 0x%X\n", (int32) pTLV->tlv_value);
            break;
          default:
            break;
        }
        break;
      case TLV_TAG_UNICODE:
        printf("UNICODE String\n");
        break;
      default:
        printf("Unknown tag type %d\n", pTLV->tlv_tag);
        break;
    }
}

#if !(TURBO || RAWHIDE)
build_fru_table( ) {
    int i;
    unsigned int    gct_base=0;
    struct HWRPB    *hwrpb;
    uint64 addr, data;

    /*
    **  Build the FRU 5.0 Config tree.
    */
    hwrpb = HWRPB;
    if ( !(hwrpb->FRU_OFFSET[0]) ) {
#if MARVEL
	/*
	 * More general...build it if we're the lowest order CPU in the
	 * hard partition, even if it's at a non-zero base.
	 */
	if (! ( ((1<<whoami()) - 1) & all_cpu_mask ) ) {
#else
	if (get_console_base_pa() == 0) {
#endif
	    gct_base = gct();
	    if (gct_base) {
		hwrpb->RSVD_HW[0] = gct_base;
		hwrpb->FRU_OFFSET[0] = gct_base - 0x2000;
		twos_checksum(hwrpb, offsetof(struct HWRPB,CHKSUM)/4,
		    (UINT *)hwrpb->CHKSUM);
		printf( "initializing GCT/FRU at %x\n",hwrpb->RSVD_HW[0]);
#if WILDFIRE
		gct_init$fixup_tree();
#endif
	    }
	} else {
	    addr = (uint64) &(hwrpb->RSVD_HW[0]);
	    for (i=0; i<1200; i++) {
		ldqp(&addr, &data);
		if (data) {
		    break;
		}
		if (i == 100) {
		    printf( "waiting for GCT/FRU to be built...\n");
		}
		krn$_sleep (100);
	    }
	    if (data) {
		hwrpb->RSVD_HW[0] = data;
		*(UINT *)hwrpb->FRU_OFFSET = (UINT) (data - virt_to_phys(HWRPB));
		twos_checksum(hwrpb, offsetof(struct HWRPB,CHKSUM)/4,
		    (UINT *)hwrpb->CHKSUM);

		/* initialize our partition info (hwrpb, os_type) */
		galaxy_increment_incarnation();
		
		printf( "initializing GCT/FRU to %x\n",hwrpb->RSVD_HW[0]);
#if WILDFIRE
		gct_init$fixup_tree();
#endif
	    }
	}
    }
}
#endif

/*
 *	Routine:	gct_init$init_tree
 *
 *	Abstract:	This routine is called to intialize the
 *			configuration tree buffer.  It sets up
 *			the memory pool size and locations, and
 *			the base pointers.  It also initializes
 *			the first two quadwords to be compatable
 *			with what is expected of memory pointed
 *			to by the CONFIG pointer in the HWRPB.
 *
 *			It then creates and initializes the ROOT
 *			node of the tree
 *
 *	Inputs:		platform_data	- An array with platform data
 *			gct_buffer	- a memory pool address.
 *					  It should be page aligned.
 *			size		- the size of the pool.
 *			node_bindings	- This structure array is indexed
 *					  by the node type.  For each type
 *					  it has the type of node that a
 *					  component of that type must be
 *					  bound and affinitized with.  The
 *					  assumption is that the binding
 *					  and affinity are the first
 *					  ancestor found of that type.
 *
 *	Outputs:	int		1 = Success
 *					0 = Failed
 *
 *	Side Effects:	The pointer to where the root node will
 *			be is initialized... but the root has
 *			not yet been allocated.
 *
 */

int gct_init$init_tree(GCT_PLATFORM_DATA *platform_data, 
      GCT_BINDINGS node_bindings[], char *gct_buffer, int size) {

  GCT_ROOT_NODE *pRoot;
  GCT_BINDINGS *root_bindings;
  unsigned __int64 *gct_header;
  int rootsize, bindsize, j;
  int namesize = 0, *t;
  char *pName;
  struct HWRPB    *hwrpb;
  struct DSRDB    *dsrdb;
  char            *name;
#if !GCT6
  GCT_BUFFER_HEADER  *pHeader;
#else
  GCT_NODE        *pHeader;
#endif

 /*
  *  Define the header...
  */
#if !GCT6
  pHeader = (GCT_BUFFER_HEADER *) gct_buffer;
#else
  pHeader = (GCT_NODE *) gct_buffer;
#endif
  gct_header = (unsigned __int64 *) gct_buffer;

  hwrpb = HWRPB;

 /*
  *  Clear the memory
  *
  */
  for (j = (size >> 3); j > 0; j -= 1) {
    *gct_header++  = 0;
  }

 /*
  *   Set the CONFIG header, and the Revision.
  */
#if !GCT6
  pHeader->buffer_size = size;
  pHeader->rev_major   = REVISION_MAJOR;
  pHeader->rev_minor   = REVISION_MINOR;
  pHeader->galaxy_enable = GALAXY;
#else		/* is GCT6 */
  pHeader->owner = size;		/* Buffer_size/rev_maj/rev_minor */
  ((uint16 *)&pHeader->owner)[2] = REVISION_MAJOR;
  ((uint16 *)&pHeader->owner)[3] = REVISION_MINOR;
  pHeader->current_owner = 0;		/* Ptr to discontiguous tree */
#endif

 /*
  *   Assign the base for the tree root.  Everything is based off this, so
  *   we assign it even before we create the root node - since it must *always*
  *   be at the first location. 
  */
#if !GCT6
  GCT_POINTER_TO_ROOT = (GCT_NODE *) (gct_buffer + sizeof(GCT_BUFFER_HEADER));
#else
  GCT_POINTER_TO_ROOT = (GCT_NODE *) gct_buffer;
#endif

 /*
  *  Create the root node inline.  We MUST do it here since all the values
  *  needed to perform allocation or any other function require data in
  *  the root node.  Point to the base we just initialized...
  *
  */
  pRoot = (GCT_ROOT_NODE *) _GCT_MAKE_ADDRESS( 0);
#if GCT6
  pRoot->galaxy_enable = GALAXY;
  pRoot->galaxy_callback = GALAXY_CALLBACKS;
#endif

 /*
  *  Compute the max size of the bindings, and round it to a 64 byte block.
  *
  */
  bindsize = ((NODE_LAST * sizeof(GCT_BINDINGS)) + 63) & ~63;

 /*
  *  Copy the platform name
  */
  pRoot->platform_name = ROOTSIZE + (2 * ID_ARRAY_SIZE) + bindsize;
  pName = (char *) ((char *) pRoot + pRoot->platform_name);
  dsrdb = (int)hwrpb + *hwrpb->DSRDB_OFFSET; 
  name = ((int)dsrdb + dsrdb->NAME_OFFSET[0]) + 8;
  strcpy( pName, name);

 /*
  *  Get the size of the name... add at least a byte and pad to 64
  */
  namesize = (strlen(name) + 64) & ~63;

 /*
  *   Make the size of the root a multiple of 64 bytes... and add in the
  *   partition and community ID arrays.  The root cannot be deleted!
  *   All the parts are aligned to 64-byte boundries.
  */
  rootsize =  ROOTSIZE + (2 * ID_ARRAY_SIZE) + bindsize + namesize;

 /*
  *  The first free byte is relative to the root
  */
  pRoot->first_free = rootsize;

 /*
  *  The bytes left in the unused memory pool
  */
#if !GCT6
  pRoot->available  = size - (sizeof(GCT_BUFFER_HEADER) + rootsize);
#else
  pRoot->available  = size - rootsize;
#endif
 /*
  *  Nothing initially on the lookaside list
  */
  pRoot->lookaside  = 0;

 /*
  *  This allows the public interface to sanity check handle ranges.
  */
  pRoot->high_limit = pRoot->available - sizeof(GCT_NODE);

 /*
  *  Set the type code
  */
  t = &pRoot->platform_type;
  t[0] = platform_data->platform_type;
  t[1] = *(int *)hwrpb->SYSVAR;

 /*
  *  Set the console size
  */
  pRoot->console_req  = platform_data->cons_req;

 /*
  *  Set the console + os BASE memory segment size
  */
  pRoot->base_alloc     = platform_data->base_alloc;
  pRoot->base_align     = platform_data->base_align;

 /*
  *  Minimum memory allocation and alignment, max fragments & partitions
  *
  */
  pRoot->min_alloc     = platform_data->min_alloc;
  pRoot->min_align     = platform_data->min_align;
#if !GCT6
  pRoot->max_fragments = platform_data->max_fragments;
#endif
  pRoot->max_desc      = platform_data->max_desc;

  pRoot->max_phys_address = 0x800000000;
  pRoot->mem_size      = mem_size;

#if 0	/* should be initialized to nulls, not spaces */
 /*
  *   This value is stored by the platform, and is unique
  *   to the system.
  */
  for (j = 0; j < GALAXY_ID_LENGTH; j += 1) {
    pRoot->galaxy_id[j] = ' ';
  }
#endif

 /*
  *  Set the max partitions to platform_data->max_partitions.  It will be
  *  dynamically altered after the tree is populated to reflect the actual
  *  number of partitions that can be created.
  *
  */
  pRoot->max_partition = platform_data->max_partitions;
  pRoot->max_platform_partition = pRoot->max_partition;

 /*
  *  Now fill in the rest of the root data
  */
  ((GCT_NODE *) pRoot)->type      = NODE_ROOT;
  ((GCT_NODE *) pRoot)->subtype   = 0;
  ((GCT_NODE *) pRoot)->size      = rootsize;
  ((GCT_NODE *) pRoot)->parent    = 0;
  ((GCT_NODE *) pRoot)->child     = 0;
  ((GCT_NODE *) pRoot)->next_sib  = 0;
  ((GCT_NODE *) pRoot)->prev_sib  = 0;
  ((GCT_NODE *) pRoot)->config    = 0;
  ((GCT_NODE *) pRoot)->affinity  = 0;
  ((GCT_NODE *) pRoot)->node_flags = 0;
  ((GCT_NODE *) pRoot)->magic     = NODE_VALID;

#if !GCT6
  pRoot->transient_level          = 1;
#endif
  pRoot->current_level            = 1;
  pRoot->lock                     = -1;
  pRoot->partitions               = ROOTSIZE;
  pRoot->communities              = ROOTSIZE + ID_ARRAY_SIZE;
  pRoot->bindings                 = ROOTSIZE + (2 * ID_ARRAY_SIZE);

 /*
  *  Copy the bindings to the root.
  *
  */
  root_bindings = (GCT_BINDINGS *) ((char *) pRoot + pRoot->bindings);

  j = 0;
  while (node_bindings[j].bind_type) {
    root_bindings[j].bind_type     = node_bindings[j].bind_type;
    root_bindings[j].bind_parent   = node_bindings[j].bind_parent;
    root_bindings[j].bind_config   = node_bindings[j].bind_config;
    root_bindings[j].bind_affinity = node_bindings[j].bind_affinity;
    j += 1;
  }

  return (SUCCESS);
}



/*
 *	Routine:	gct_init$populate_tree
 *
 *	Abstract:	This routine is used to simulate the population of
 *			the configuration tree.  It populates the hardware
 *			components, the SW root, and template trees.
 *
 *			The routine also performs binding of hardware
 *			configuration constrains and performance constraints.
 *
 *	Inputs:		tree_outline	- This structure array provides a
 *					  simplified tree which has the
 *					  depth, type, subtype, ID, and
 *					  flags for the creation of a node.
 *					  The depth is used to guide the
 *					  creation of the unbalanced tree.
 *
 *	Outputs:			  None
 *
 *	Side Effects:
 */

void gct_init$populate_tree( GCT_PLATFORM_TREE *tree_outline){

  int j, k, new_depth, current_depth = 0, status = 0;
  GCT_HANDLE node = 0, prev_node = 0, hw_root = 0, fru_root = 0;
  GCT_NODE *pTemp, *tmp_fru;
  GCT_ROOT_NODE *pRoot;

  pRoot = (GCT_ROOT_NODE *) _GCT_MAKE_ADDRESS(0);

  status = gct$find_node( 0, &fru_root, FIND_BY_TYPE, NODE_FRU_ROOT, 0, 0, 0, 0);
  tmp_fru = _GCT_MAKE_ADDRESS(fru_root);
  prev_node = _GCT_MAKE_HANDLE(tmp_fru);

  while (tree_outline->type) {

    new_depth = tree_outline->depth;

   /* 
    *   Create the node
    */
    node = (gct_node_info[tree_outline->type].create_node)(tree_outline->type,
            tree_outline->subtype, tree_outline->flags, 0, &tree_outline->private[0]);

    if (node == 0) {
      return;
    }

    pTemp = _GCT_MAKE_ADDRESS(node);

   /*
    *   Fill in the ID
    */
    pTemp->id               = tree_outline->id;

   /*
    *   Insert it according to it's depth.
    */
    if (new_depth == current_depth) {

     /*
      *   New sibling
      */
      gct__insert_node(prev_node, node, GCT__INSERT_NEXT_SIBLING);

    }
    else {

      if (new_depth > current_depth) {

       /*
        *   New child.  Note depth can only increase by 1
        */
        gct__insert_node(prev_node, node, GCT__INSERT_CHILD);
        current_depth = new_depth;
      }
      else {

       /*
        *   Heading back up the tree.  Follow the parent trail.
        *   And then insert at that level as a sibling.
        */
        while (new_depth != current_depth) {
          pTemp = _GCT_MAKE_ADDRESS(prev_node);
          prev_node = pTemp->parent;
          current_depth -= 1;
        }

        gct__insert_node(prev_node, node, GCT__INSERT_NEXT_SIBLING);

      }
    }

    gct_init$bind_node( node);

    prev_node = node;
    tree_outline++;
  }

#if TURBO || RAWHIDE
 /*
  *  Find out how many partitions can dance on the head of a pin.  Set
  *  the actual result (mimimized with pRoot->max_platform_partition) into
  *  the root.
  *
  */
  j = gct$get_max_partition();

  if (j < pRoot->max_platform_partition) pRoot->max_partition = j;
  else pRoot->max_partition = pRoot->max_platform_partition;
#endif
}

#if (TURBO || RAWHIDE || PC264)
/*
 *	Routine:	gct_init$configure_galaxy
 *
 *	Abstract:	This routine creates the software community and
 *			partitions, and assigns hardware to them.  It
 *			simulates what needs to occur when saved state
 *			such as NVRAM is played back during initialization.
 *
 *	Inputs:		galaxy_config	- This structure array is the
 *					  type of information that would
 *					  be kept in NVRAM to reconstruct
 *					  a galaxy configuration at init.
 *					  It is simple in that it contains
 *					  little more than node types and
 *					  IDs.  Software types are created,
 *					  hardware types are searched for
 *					  (with the ID as well) and the
 *					  component assigned to the partition.
 *
 *	Outputs:	int		1 = SUCCESS
 *					0 = FAILED - Typically the hardware
 *					    was not found.  This is typical
 *					    of what would really happen if
 *					    hardware is removed at power off.
 *
 *					    The failure may not be total -
 *					    it may mean that not all partitions
 *					    are now valid.
 *	Side Effects:
 *
 */

int gct_init$configure_galaxy( GCT_GALAXY_NVRAM galaxy_config[], char *galaxy_id) {

  int j = 0, status, atype = 0;
  GCT_ROOT_NODE *pRoot;
  GCT_HANDLE community = 0, partition = 0, sw_root = 0, hw_root = 0, node = 0;
  GCT_NODE *pCommunity = 0, *pPartition = 0, *pNode = 0;
  GCT_HANDLE *pPartitions, *pCommunities;
  char buffer[80];

  pRoot = (GCT_ROOT_NODE *) _GCT_MAKE_ADDRESS( 0);
  pPartitions  = (GCT_HANDLE *) ((char *) pRoot + pRoot->partitions);
  pCommunities = (GCT_HANDLE *) ((char *) pRoot + pRoot->communities);

  /* fill in ptrs */
  status = gct$find_node( 0, &sw_root,  FIND_BY_TYPE, NODE_SW_ROOT,  0, 0, 0, 0);
  status = gct$find_node( 0, &hw_root,  FIND_BY_TYPE, NODE_HW_ROOT,  0, 0, 0, 0);

 /*
  *   This value is stored by the platform, and is unique
  *   to the system.
  */
  for (j = 0; j < GALAXY_ID_LENGTH; j += 1) {
    pRoot->galaxy_id[j] = galaxy_id[j];
  }

  j = 0;
  while (galaxy_config[j].type) {

   /*
    *  If the definition is a init-only node (i.e. a memory fragment assignment)
    *  then skip it for now.
    *
    */
    if (galaxy_config[j].private_init == INIT_ONLY) {j += 1; continue;};

   /*
    *   Handle community creation
    *
    */
    if (galaxy_config[j].type == NODE_COMMUNITY) {

     /*
      *    Reset the partition ID
      */
      partition = 0;

     /*
      *   Validate the ID is not in use.  This should be fatal, because
      *   it means something is corrupt.
      */
      if (pCommunities[galaxy_config[j].id]) {
        printf("Community ID %Ld used more than once in galaxy_config data\n", galaxy_config[j].id);
        return 0;
      }

     /*
      *   Create a new community node.  Always a child of the SW root.
      */
      community = (gct_node_info[NODE_COMMUNITY].create_node)(NODE_COMMUNITY, 0, 0, 0, 0);
      gct__insert_node( sw_root, community, GCT__INSERT_CHILD_LAST_SIB);

      pCommunity = _GCT_MAKE_ADDRESS(community);

     /*
      *   Fill in the ID
      */
      pCommunities[galaxy_config[j].id] = community;
      pCommunity->id = galaxy_config[j].id;

     /*
      *   Link the owner field.
      */
      pCommunity->owner             = sw_root;
      pCommunity->current_owner     = sw_root;
      pCommunity->config            = sw_root;
      pCommunity->affinity          = sw_root;

     /*
      *   Next device
      */
      j += 1;
      continue;
    }

    if (galaxy_config[j].type == NODE_PARTITION) {

     /*
      *   Validate the ID is not in use.  This should be fatal, because
      *   it means something is corrupt.
      */
      if (pPartitions[galaxy_config[j].id]) {
        printf("Partition ID %Ld used more than once in galaxy_config data\n", galaxy_config[j].id);
        return (0);
      }

     /*
      *   Create a new partition node.  Always a child of the community.
      */
      partition = (gct_node_info[NODE_PARTITION].create_node)(NODE_PARTITION, 0, 0, 0, 0);
      gct__insert_node( community, partition, GCT__INSERT_CHILD_LAST_SIB);

      pPartition = _GCT_MAKE_ADDRESS(partition);

     /*
      *   Fill in the ID
      */
      pPartitions[galaxy_config[j].id] = partition;
      pPartition->id = galaxy_config[j].id;

     /*
      *   Link the owner field to the community.
      */
      pPartition->owner             = community;
      pPartition->current_owner     = community;
      pPartition->config            = community;
      pPartition->affinity          = community;

     /*
      *   Next node
      */
      j += 1;
      continue;
    }


   /*
    *   Not a partition or community... lookup hardware node
    */
    node = hw_root;
    status = gct$find_node( hw_root, &node,
            (FIND_BY_TYPE | FIND_BY_SUBTYPE | FIND_BY_ID), 
             galaxy_config[j].type,
             galaxy_config[j].subtype,
             galaxy_config[j].id, 0, 0);

    if (node && (status == 1)) {

     /*
      *   We have a HW node, now set it's ownership.
      */
      if (partition == 0) {

       /*
        *   Community
        */
        if (community == 0) {
          printf("No Partition or Community in galaxy_config before HW ID %Ld of type %d\n",
              galaxy_config[j].id, galaxy_config[j].type);
          atype = -1;
        }
        else {
          status = SUCCESS;
          if (galaxy_config[j].private_init != INIT_ONLY)
            status = gct$assign_hw( community, node, 1, 0);
          atype = 0;
        }
      }
      else {
       /*
        *   Partition
        */
        status = SUCCESS;
        if (galaxy_config[j].private_init != INIT_ONLY)
          status = gct$assign_hw( partition, node, 1, 0);
        atype  = 1;
      }

      if (status != SUCCESS) {
        if (atype >= 0) {
          printf("Error returned from hardware assignment (config line %d) of node %X (ID %Ld) to %s %X.\n",
                   j+1, node, galaxy_config[j].id, atype ? "partition" : "community", atype ? partition : community);
          printf("type %x  subtype %x  id %x\n",
             galaxy_config[j].type,galaxy_config[j].subtype,galaxy_config[j].id);

        }
      }
    }
    else {
      printf("Hardware ID %Ld of type %d in galaxy_config not in HW tree\n",
              galaxy_config[j].id, galaxy_config[j].type);
    }

    j += 1;
  }

 /*
  *   Now initialize if needed.  Do it *after* all partitions
  *   and communities are created!
  */
  j = 0;
  community = partition = -1;

  while (galaxy_config[j].type) {

   /*
    *   Find the node address
    */
    node = 0;
    status = gct$find_node( 0, &node,
            (FIND_BY_TYPE | FIND_BY_SUBTYPE | FIND_BY_ID), 
             galaxy_config[j].type,
             galaxy_config[j].subtype,
             galaxy_config[j].id, 0, 0);

   /*
    *  If its a community node, then set it, and invalidate the partition
    */
    if (galaxy_config[j].type == NODE_COMMUNITY) {
      community = node;
      partition = -1;
    }

   /*
    *  If its a partition node, then set it.
    */
    if (galaxy_config[j].type == NODE_PARTITION) {
      partition = node;
    }

   /*
    *  If the private_init flag isn't NO_INIT (zero), and if there is a
    *  init routine, then call the node-private initialization
    */
    if (node && (status == 1)) {
      if (galaxy_config[j].private_init && gct_node_info[galaxy_config[j].type].galaxy_init)
        status = (gct_node_info[galaxy_config[j].type].galaxy_init)(node, community, partition, &galaxy_config[j].private);
    }
    j += 1;
  }

 /*
  *   Validate and Init the partitions
  */
  node = sw_root;
  while (node) {

    status = gct$find_node( sw_root, &node,
             FIND_BY_TYPE, 
             NODE_PARTITION,
             0,
             0, 0, 0);

    if (node && (status == 1)) {

      if (gct$validate_partition( node) == SUCCESS) {

      /* Init partition call goes here */

      }
    }
  }

  return (SUCCESS);
}
#endif

#if CLIPPER
/*
 *	Routine:	gct_init$configure_built_galaxy
 */

int gct_init$configure_built_galaxy( char *galaxy_id ) {

  GCT_ROOT_NODE         *pRoot;
  GCT_HANDLE            community = 0, partition = 0, sw_root = 0, hw_root = 0, node = 0;
  GCT_NODE              *pCommunity = 0, *pPartition = 0, *pNode = 0, *pNext;
  GCT_HANDLE		smb_node;
  GCT_NODE_ID_CPU       *pNodeid_cpu;
  GCT_NODE_ID_IOP       *pNodeid_iop;
  GCT_NODE_ID_MEM_DESC  *pNodeid_mem_desc;
  GCT_MEM_FRAG_DEFS     *sw_tmp;
  struct EVNODE         evn, *evp=&evn;
  int                   hard_cpu_mask = 0, hard_iop_mask = 0;
  int                   i, j = 0, status, atype = 0, lp_part = 1;
  unsigned __int64      node_id = 0;
  char                  evname[80];
  int			primary_found;

  pRoot = (GCT_ROOT_NODE *) _GCT_MAKE_ADDRESS( 0);

  /* fill in ptrs */
  status = gct$find_node( 0, &hw_root,  FIND_BY_TYPE, NODE_HW_ROOT,  0, 0, 0, 0);
  status = gct$find_node( 0, &community, (FIND_BY_TYPE | FIND_BY_ID ),NODE_COMMUNITY, 0, 0, 0, 0);
  if (status != SUCCESS) {
    printf("Error locating community.\n");
    return(status);
  }

  smb_node = 0;
  /* assign the system motherboard to the community */
  status = gct$find_node( 0, &smb_node, FIND_BY_TYPE, NODE_SMB, 0, 0, 0, 0);
  if (status != SUCCESS) {
    printf("Error locating system motherboard node.\n");
    return(status);
  }

  status = gct$assign_hw( community, smb_node, 1, 0);
  if (status != SUCCESS) {
    printf("Error assigning system motherboard to community, status: %x\n",
	status);
    return(status);
  }
  
 /*
  *   This value is stored by the platform, and is unique
  *   to the system.
  */
  for (j = 0; j < GALAXY_ID_LENGTH; j += 1) {
    pRoot->galaxy_id[j] = galaxy_id[j];
  }

  /*
   *   Read the number of Partitions to create
   *   Do a big loop on the number
   *
   */
  sprintf(evname,"lp_count");
  if (ev_read(evname, &evp, 0) == msg_success) {
	lp_part = xtoi(evp->value.string);
	if (lp_part == 0)
	    lp_part = 1;    /* process at least one partition */
  }

  for ( i = 0; i < lp_part; i++ ) {

	/* Find the right Partition to use */
	partition = 0;
	status = gct$find_node( 0, &partition, (FIND_BY_TYPE | FIND_BY_ID ),NODE_PARTITION, 0, i, 0, 0);
	if (status != SUCCESS) {
	    printf("Error locating partition node %d\n", i);
	    return(status);
	}
	pNode = _GCT_MAKE_ADDRESS( partition );

	sprintf(evname,"lp_cpu_mask%d",i);
	if (ev_read(evname, &evp, 0) == msg_success)
	    hard_cpu_mask = xtoi(evp->value.string);

	if (hard_cpu_mask == 0)	    /* safety valve */
	    hard_cpu_mask = 0xf;

	hard_cpu_mask &= all_cpu_mask;	/* only look for the present ones */

	sprintf(evname,"lp_io_mask%d",i);
	if (ev_read(evname, &evp, 0) == msg_success)
	    hard_iop_mask = xtoi(evp->value.string);

	if (hard_iop_mask == 0)	    /* safety valve */
	    hard_iop_mask = 0xf;

	/*  Populate CPUs */
	primary_found = 0;	/* first cpu is primary */

	for (j = 0; j < HWRPB$_NPROC; j++) {
	    if (hard_cpu_mask & (1<<j)) {
		node_id = 0;
		pNodeid_cpu = &node_id;
		pNodeid_cpu->node_id_cpu_cpu         = j;
		pNodeid_cpu->node_id_cpu_rsrvd_16_23 = -1;
		pNodeid_cpu->node_id_cpu_hsbb        = -1;
		pNodeid_cpu->node_id_cpu_rsrvd_56_63 = -1;
		node = 0;
		status = gct$find_node( 0, &node, (FIND_BY_TYPE | FIND_BY_ID ),NODE_CPU, 0, node_id, 0, 0);
		if (status != SUCCESS) {
		    printf("Error locating CPU node %d, id %x\n", j, node_id);
		    return(status);
		} else {
		    if (tree_debug) printf("Found CPU node %d, id %x status %x\n", j, node_id, status);
		}

		pNode = _GCT_MAKE_ADDRESS( node );
		if (primary_found == 0) {
		    pNode->node_flags |= NODE_CPU_PRIMARY;
		    primary_found = 1;
		}
		pNode->node_flags |= NODE_IN_CONSOLE;
		if (tree_debug) printf("Assign node %x type %x to Partition %x\n",node,pNode->type,i);
		status = gct__assign_hw_to_partition( partition, node, 1);
		if (status != SUCCESS) {
		    printf("Error assigning CPU node %d, id %x, status %x\n", j, node_id, status);
		    return(status);
		} else {
		    if (tree_debug) printf("Assigned CPU node %d, id %x status %x\n", j, node_id, status);
		}
	    }
	} /* end of CPU loop */

	node_id = 0;
	/*  Populate IOPs */
	for (j = 0; j < 2; j++) {
	    if (hard_iop_mask & (1<<j)) {
		node_id = 0;
		pNodeid_iop = &node_id;
		pNodeid_iop->node_id_iop_iop         = j;
		pNodeid_iop->node_id_iop_rsrvd_8_31  = -1;
		pNodeid_iop->node_id_iop_hsbb        = -1;
		pNodeid_iop->node_id_iop_rsrvd_56_63 = -1;
		node = 0;
		status = gct$find_node( 0, &node, (FIND_BY_TYPE | FIND_BY_ID ),NODE_IOP, 0, node_id, 0, 0);
		if (status != SUCCESS) {
		    printf("Error locating IOP node %d, id %x\n", j, node_id);
		    return(status);
		}
		pNode = _GCT_MAKE_ADDRESS( node );
		if (tree_debug) printf("Assign node %x type %x to Partition %x\n",node,pNode->type,i);
		status = gct__assign_hw_to_partition( partition, node, 1);
		if (status != SUCCESS) {
		    printf("Error assigning IOP node %d, id %x, status %x\n", j, node_id, status);
		    return(status);
		}
	    }
	} /* end of IOP loop */

	if (gct$validate_partition( partition ) == SUCCESS) {
		/* Init partition call goes here */
		if (tree_debug) printf("Valid Partition\n");
	}
  }

  /* do something with these memory descriptors... */
  if ((status = build_mem_fragments( pRoot, community,
			    hw_root, lp_part)) != SUCCESS) {
    return(status);
  }

  /* assign the remaining hardware under the SMB to the community */

  pNode = _GCT_MAKE_ADDRESS( smb_node );
  node = pNode->child;
  while (node) {
    pNode = _GCT_MAKE_ADDRESS(node);
    if (pNode->owner == 0) {
	pNode->owner = community;
	pNode->current_owner = community;
	if (tree_debug) 
	    printf("assign %x owner %x current_owner %x\n",
		pNode->type,pNode->owner,pNode->current_owner);
    }
    node = gct__lookup_node( smb_node, node, FIND_ANY, 0, 0, 0, 0, 0);
  }

  return (SUCCESS);
}
#endif




/*
 *	Routine:	gct_init$bind_node
 *
 *	Abstract:	This routine is used to apply the binding
 *			rules to a hw node.  This routine is called
 *			when populating the HW configuration.
 *
 *		The config rules are:
 *
 *		   1) bindings are only valid on HARDWARE nodes
 *		   2) A node can be bound to the HW_ROOT, or any direct ancestor
 *		   3) A node that is bound to the HW_ROOT can be "owned" by any software entity
 *
 *	Inputs:	pNode		- Pointer to the HW node
 *		node_bindings	- Pointer to the structure with the
 *				  binding rules for the HW node.
 *
 *	Outputs:		  1 = Success
 *				  0 = No anscestor matches the type needed
 *				      to bind the node (CONFIG)
 *				 -1 = Same thing for AFFINITY
 *
 *	Side Effects:
 *
 */

int gct_init$bind_node( GCT_HANDLE node) {

  int found = 0, j;
  GCT_NODE *pNext;
  GCT_NODE *pNode;
  GCT_ROOT_NODE *pRoot;
  GCT_BINDINGS *root_bindings;

  pRoot = (GCT_ROOT_NODE *) _GCT_MAKE_ADDRESS(0);
  root_bindings = (GCT_BINDINGS *) ((char *) pRoot + pRoot->bindings);

  pNode = _GCT_MAKE_ADDRESS(node);

 /*
  *  If it's not hardware, don't bind it.  Just return success.
  */
  if (!pNode->node_hardware) return (1);

 /*
  *   See if the node needs config/affinity binding... return
  *   success if the binding is null.
  */
  j = 0;
  while (root_bindings[j].bind_type) {
    if (root_bindings[j].bind_type == pNode->type) {
      if (!root_bindings[j].bind_config) {
        return (1);
      }
      break;
    }
    j += 1;
  }

 /*
  *    Walking up the parent chain, find the first node type
  *    that matches the config value.
  */
  pNext = pNode;

  while (pNext->parent != 0) {
    pNext = _GCT_MAKE_ADDRESS( pNext->parent);
    if (pNext->type == root_bindings[j].bind_config) {
      pNode->config = _GCT_MAKE_HANDLE( pNext);
      found = 1;
      break;
    }
  }

  if (!found) {
    node = _GCT_MAKE_HANDLE(pNode);
    return 0;
  }

  found = 0;
  pNext = pNode;
  while (pNext->parent) {
    pNext = _GCT_MAKE_ADDRESS( pNext->parent);
    if (pNext->type == root_bindings[j].bind_affinity) {
      pNode->affinity = _GCT_MAKE_HANDLE( pNext);
      found = 1;
      break;
    }
  }

  if (!found) {
    node = _GCT_MAKE_HANDLE(pNode);
    return -1;
  }
  return 1;
}

unsigned int gct_os_type (char *os) {
struct os_table {
  char name[8];
  unsigned int type;
  } os_type_table [] = {
	{"OpenVMS",OS_VMS},
	{"VMS",OS_VMS},
	{"Unix",OS_OSF},
	{"OSF",OS_OSF},
	{"NT",OS_NT},
	{0,OS_LAST}};
struct os_table *os_tbl;

    for (os_tbl=os_type_table;os_tbl->name;os_tbl++)
	if (strcmp_nocase(os, os_tbl->name) == 0) break;
    return os_tbl->type;
}

#if GALAXY_CALLBACKS

#define CALLBACK_DEBUG 1
extern int callback_debug_flag;

void cb_galaxy( struct impure *IMPURE )
    {
    uint64 code;
    uint64 argc;
    uint64 argv[16];
    uint64 sp_va;
    uint64 sp_pa;
    uint64 data;
    int i;
    int t;

    code = *(uint64 *)&IMPURE->cns$gpr[0][2*17];
    argc = *(uint64 *)&IMPURE->cns$gpr[0][2*25] & 255;
#if CALLBACK_DEBUG
    if( callback_debug_flag & 0x1000 )
	pprintf( "cb_galaxy, code = %d, argc = %d\n", code, argc );
#endif

    *(uint64 *)&IMPURE->cns$gpr[0][0] = BADPARAM;

    if( argc > 18 )
	return;

    sp_va = *(uint64 *)&IMPURE->cns$gpr[0][2*28];
    for( i = 0; i < argc - 2; i++ )
	{
	if( i < 4 )
	    argv[i] = *(uint64 *)&IMPURE->cns$gpr[0][2*(i+18)];
	else
	    {
	    if( virtual_to_physical( sp_va, &sp_pa ) != msg_success )
		return;
	    ldqp( &sp_pa, &argv[i] );
	    sp_va += 8;
	    }
	}

    switch( code )
	{
	case FIND_NODE:
	    {
	    GCT_HANDLE root;
	 /* GCT_HANDLE *node; */
	    uint64 node_va;
	    uint64 node_pa;
	    GCT_HANDLE node;
	    int search_criteria;
	    char search_type;
	    char search_subtype;
	    GCT_ID search_id;
	    GCT_HANDLE search_owner;
	 /* int *depth; */
	    uint64 depth_va;
	    uint64 depth_pa;
	    int depth;
	    int status;

#if CALLBACK_DEBUG
	    if( callback_debug_flag & 0x2000 )
		pprintf( "FIND_NODE\n" );
#endif
	    root			= argv[0];
	    node_va			= argv[1];
	    search_criteria		= argv[2];
	    search_type		= argv[3];
	    search_subtype		= argv[4];
	    search_id		= argv[5];
	    search_owner		= argv[6];
	    depth_va		= argv[7];

	    t = virtual_to_physical( node_va, &node_pa );
	    if( t != msg_success )
		{
		pprintf( "virtual_to_physical of node_va = %x failed, status = %x\n", node_va, t );
		return;
		}
	    ldqp( &node_pa, &data );
	    node = data;

	    if( depth_va )
		{
		t = virtual_to_physical( depth_va, &depth_pa );
		if( t != msg_success )
		    {
		    pprintf( "virtual_to_physical of depth_va = %x  failed, status = %x\n", depth_va, t );
		    return;
		    }
		ldqp( &depth_pa, &data );
		depth = data;
		}
	    else
		depth_pa = 0;

#if CALLBACK_DEBUG
	    if( callback_debug_flag & 0x4000 )
		{
		pprintf( " root = %x\n", root );
		pprintf( " node_va = %x\n", node_va );
		pprintf( " *node = %x\n", node );
		pprintf( " search_criteria = %x\n", search_criteria );
		pprintf( " search_type = %d\n", search_type );
		pprintf( " search_subtype = %d\n", search_subtype );
		pprintf( " search_id = %x\n", search_id );
		pprintf( " search_owner = %x\n", search_owner );
		pprintf( " depth_va = %x\n", depth_va );
		if( depth_va )
		    pprintf( " *depth = %x\n", depth );
		pprintf( "calling gct$find_node\n" );
		}
#endif

	    status = gct$find_node( root, &node, search_criteria, search_type, search_subtype, search_id, search_owner, &depth );

	    data = node;
	    stqp( &node_pa, &data );

	    if( depth_va )
		{
		data = depth;
		stqp( &depth_pa, &data );
		}

#if CALLBACK_DEBUG
	    if( callback_debug_flag & 0x4000 )
		{
		pprintf( " status = %d\n", status );
		pprintf( " *node = %x\n", node );
		if( depth )
		    pprintf( " *depth = %x\n", depth );
		}
#endif

	    *(uint64 *)&IMPURE->cns$gpr[0][0] = status;
	    break;
	    }

	case UPDATE_GMDB:
	    {
	    GCT_HANDLE community;
	    int offset;
	    uint64 new_data;
	    uint64 old_data;
	    int status;

#if CALLBACK_DEBUG
	    if( callback_debug_flag & 0x2000 )
		pprintf( "UPDATE_GMDB\n" );
#endif
	    community		= argv[0];
	    offset			= argv[1];
	    new_data		= argv[2];
	    old_data		= argv[3];

#if CALLBACK_DEBUG
	    if( callback_debug_flag & 0x4000 )
		{
		pprintf( " community = %x\n", community );
		pprintf( " offset = %x\n", offset );
		pprintf( " new_data = %x\n", new_data );
		pprintf( " old_data = %x\n", old_data );
		pprintf( "calling gct$update_gmdb\n" );
		}
#endif

	    status = gct$update_gmdb( community, offset, new_data, old_data );

#if CALLBACK_DEBUG
	    if( callback_debug_flag & 0x4000 )
		{
		pprintf( " status = %d\n", status );
		}
#endif

	    *(uint64 *)&IMPURE->cns$gpr[0][0] = status;
	    break;
	    }

	case UPDATE_GALAXY_ID:
	    {
	 /* char *new_data; */
	    uint64 new_data_va;
	    char new_data[GALAXY_ID_LENGTH];
	 /* char *old_data; */
	    uint64 old_data_va;
	    char old_data[GALAXY_ID_LENGTH];
	    int status;
	    struct FILE *vmem_fp;

#if CALLBACK_DEBUG
	    if( callback_debug_flag & 0x2000 )
		pprintf( "UPDATE_GALAXY_ID\n" );
#endif
	    new_data_va		= argv[0];
	    old_data_va		= argv[1];

	    vmem_fp = fopen( "vmem", "sr" );
	    if( old_data_va )
		{
		fseek( vmem_fp, new_data_va, SEEK_SET );
		fread( new_data, 1, sizeof( new_data ), vmem_fp );
		fseek( vmem_fp, old_data_va, SEEK_SET );
		fread( old_data, 1, sizeof( old_data ), vmem_fp );
		}

#if CALLBACK_DEBUG
	    if( callback_debug_flag & 0x4000 )
		{
		pprintf( " new_data_va = %x\n", new_data_va );
		if( old_data_va )
		    pprintf( " new_data = `%-16.16s'\n", new_data );
		pprintf( " old_data_va = %x\n", old_data_va );
		if( old_data_va )
		    pprintf( " old_data = `%-16.16s'\n", old_data );
		pprintf( "calling gct$update_galaxy_id\n" );
		}
#endif

	    status = gct$update_galaxy_id( new_data, old_data_va ? old_data : 0 );

	    if( !old_data_va )
		{
		fseek( vmem_fp, new_data_va, SEEK_SET );
		fwrite( new_data, 1, sizeof( new_data ), vmem_fp );
		}
	    fclose( vmem_fp );

#if CALLBACK_DEBUG
	    if( callback_debug_flag & 0x4000 )
		{
		pprintf( " status = %d\n", status );
		if( !old_data_va )
		    pprintf( " new_data = `%-16.16s'\n", new_data );
		}
#endif

	    *(uint64 *)&IMPURE->cns$gpr[0][0] = status;
	    break;
	    }

	case UPDATE_INSTANCE_NAME:
	    {
	    GCT_HANDLE partition;
	 /* char *new_data; */
	    uint64 new_data_va;
	    char new_data[INSTANCE_NAME_LENGTH];
	 /* char *old_data; */
	    uint64 old_data_va;
	    char old_data[INSTANCE_NAME_LENGTH];
	    int status;
	    struct FILE *vmem_fp;

#if CALLBACK_DEBUG
	    if( callback_debug_flag & 0x2000 )
		pprintf( "UPDATE_INSTANCE_NAME\n" );
#endif
	    partition		= argv[0];
	    new_data_va		= argv[1];
	    old_data_va		= argv[2];

	    vmem_fp = fopen( "vmem", "sr" );
	    if( old_data_va )
		{
		fseek( vmem_fp, new_data_va, SEEK_SET );
		fread( new_data, 1, sizeof( new_data ), vmem_fp );
		fseek( vmem_fp, old_data_va, SEEK_SET );
		fread( old_data, 1, sizeof( old_data ), vmem_fp );
		}

#if CALLBACK_DEBUG
	    if( callback_debug_flag & 0x4000 )
		{
		pprintf( " partition = %x\n", partition );
		pprintf( " new_data_va = %x\n", new_data_va );
		if( old_data_va )
		    pprintf( " new_data = `%-16.16s'\n", new_data );
		pprintf( " old_data_va = %x\n", old_data_va );
		if( old_data_va )
		    pprintf( " old_data = `%-16.16s'\n", old_data );
		pprintf( "calling gct$update_instance_name\n" );
		}
#endif

	    status = gct$update_instance_name( partition, new_data, old_data_va ? old_data : 0 );

	    if( !old_data_va )
		{
		fseek( vmem_fp, new_data_va, SEEK_SET );
		fwrite( new_data, 1, sizeof( new_data ), vmem_fp );
		}
	    fclose( vmem_fp );

#if CALLBACK_DEBUG
	    if( callback_debug_flag & 0x4000 )
		{
		pprintf( " status = %d\n", status );
		if( !old_data_va )
		    pprintf( " new_data = `%-16.16s'\n", new_data );
		}
#endif

	    *(uint64 *)&IMPURE->cns$gpr[0][0] = status;
	    break;
	    }

	default:
	    {
	    *(uint64 *)&IMPURE->cns$gpr[0][0] = NOTIMPLEMENTED;
	    break;
	    }
	}

    }

#endif


/*+
 * ============================================================================
 * = gctverify - Verify the integrity of the Config tree                      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      A shell command to be used by anyone to debug whatever they're
 *	working on at the moment.
 *
 * COMMAND FMT: gctverify 6 0 0 gctverify
 *
 * COMMAND FORM:
 *  
 *	gctverify ( )
 *  
 * COMMAND TAG: gctverify 0 RXBZ gctverify
 *
 * COMMAND ARGUMENT(S):
 *
 * 	None.
 *
 * COMMAND EXAMPLE(S):
 *~
 *	>>>gctverify
 *	>>>
 *~
 * FORM OF CALL:
 *  
 *	gctverify (int argc, char *argv [])
 *  
 * RETURN CODES:
 *
 *	None.
 *       
 * ARGUMENTS:
 *
 * 	argc	- number of words present on command line
 *	argv	- array of pointers to words on command line
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/

gctverify (int argc, char *argv[]) {
  int                      i = 0, bigloop = 0, status, sp;
  GCT_HANDLE               root = 0, node = 0;
  GCT_NODE                 *pNode, *pNext, *pFnode, *pHeader;
  GCT_HD_EXT               *pHd_ext;
  GCT_SUBPACK              *pSubpack;
  unsigned char		   *buf;
  struct HWRPB             *hwrpb;


  buf = hwrpb->FRU_OFFSET[0] + 0x2000;
  pHeader = (GCT_NODE *) buf;
  status = gct$find_node(0,&root, FIND_BY_TYPE, NODE_ROOT,  0, 0, 0, 0);
  pNode = _GCT_MAKE_ADDRESS(root);
  
  pNext = pNode;
  while (pNext && (bigloop < 1)) { 

    /* find the next Node */
    status = gct$find_node(0,&node, FIND_ANY, pNext , 0, 0, 0, 0);
    pNext = _GCT_MAKE_ADDRESS(node);
    pprintf("Node %x ", pNext->type );
    if (pNext->hd_extension != 0) {
	pprintf("has HD_EXT...");
	pHd_ext = &pNext->type + pNext->hd_extension;
	if (pHd_ext->subpkt_offset) {
	    pprintf("has SubPkts...");
	    pSubpack = &pNext->type + pHd_ext->subpkt_offset;
	    for (sp=0; sp < pHd_ext->subpkt_count; sp++) {
		printf("Sub%d...",sp+1);
		pSubpack = (int)pSubpack + pSubpack->subpack_length;
	    }
	    if ( pSubpack != pHd_ext )
		pprintf("BAD (subpkt %x hdext %x)",&pSubpack, &pHd_ext);
	}
	if (pHd_ext->fru) {
	    pprintf("has FRU...");
	    pFnode = _GCT_MAKE_ADDRESS(pHd_ext->fru);
	    if ( pFnode->type != NODE_FRU_DESC ) {
		pprintf("BAD %x",*pFnode);
	    } else
		pprintf("OK");
	}
    }

    pprintf("\n");
    if ( killpending () ) return 0;
    if (pNext->type == 1) bigloop++;
  }

  return 0;
}

