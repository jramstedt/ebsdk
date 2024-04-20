/*
 *
 *    Galaxy_def_configs.h
 *
 */

#include "cp$src:common.h"

#define GCT__GENERIC     0
#define GCT__COBRA       2
#define GCT__TURBOLASER  12
#define GCT__AVANTI      13
#define GCT__RAWHIDE     22
#define GCT__LYNX        24
#define GCT__TSUNAMI     34
#define GCT__WILDFIRE    35


#if (TURBO || RAWHIDE )

/*Depth     Type    Subtype    ID     Flags    Private */
static GCT_PLATFORM_TREE TL_tree[128] = {
  {1, NODE_HW_ROOT,	0,	0,	0, {0}},
  {0}
}; 

static GCT_GALAXY_NVRAM TL_galaxy_config[128] = {
  {NODE_COMMUNITY,	0,	0,	NO_INIT, {0}},
  {0}
};

/*Depth     Type    Subtype    ID     Flags    Private */
static GCT_PLATFORM_TREE TL_tree_template[] = {
  {2, NODE_IOP,		0,	0,	NODE_HARDWARE, {0}},
  {2, NODE_CPU_MODULE,	0,	0,	NODE_HARDWARE, {0}},
  {3, NODE_CPU,		0,	0,	(NODE_IN_CONSOLE |NODE_HARDWARE), {0}},
  {2, NODE_MEMORY_SUB,	0,	0,	NODE_HARDWARE, {0,FOUR_GIG-1}},
  {3, NODE_MEMORY_CTRL,	0,	0,	NODE_HARDWARE, {0}},
  {3, NODE_MEMORY_DESC,	0,	0,	NODE_HARDWARE, {0,FOUR_GIG}},
  {1, NODE_SW_ROOT,	0,	0,	0, {0}},
  {1, NODE_TEMPLATE_ROOT,0,	0,	0, {0}},
  {2, NODE_IOP,		0,	0,	NODE_HW_TEMPLATE, {0}},
  {2, NODE_CPU,		0,	0,	NODE_HW_TEMPLATE, {0}},
  {2, NODE_MEMORY_DESC,	0,	0,	NODE_HW_TEMPLATE, {0,32*ONE_MEG}},
  {0}
};

static GCT_GALAXY_NVRAM TL_galaxy_config_template[] = {
  {NODE_COMMUNITY,	0,	0,	NO_INIT, {0}},
  {NODE_CPU_MODULE,	0,	0,	NO_INIT, {0}},
  {NODE_MEMORY_SUB,	0,	0,	NO_INIT, {0}},
  {NODE_MEMORY_DESC,	0,	0,	NO_INIT, {0}},
  {NODE_MEMORY_DESC,	0,	0,	INIT_ONLY, {NODE_PARTITION,0,MEM_PRIVATE | MEM_BASE | MEM_CONSOLE,0,TWO_MEG}},
  {NODE_MEMORY_DESC,	0,	0,	INIT_ONLY, {NODE_PARTITION,0,MEM_PRIVATE | MEM_BASE,0,0}},
  {NODE_MEMORY_DESC,	0,	0,	INIT_ONLY, {NODE_PARTITION,0,MEM_PRIVATE | MEM_BASE | MEM_CONSOLE,0,0x41*0x2000}},
  {NODE_MEMORY_DESC,	0,	0,	INIT_ONLY, {NODE_COMMUNITY,0,MEM_SHARED,0,0}},
  {NODE_PARTITION,	0,	0,	INIT, {0x2000, OS_VMS}},
  {NODE_IOP,		0,	0,	NO_INIT, {0}},
  {NODE_CPU,		0,	0,	INIT, {NODE_CPU_PRIMARY}},
  {NODE_CPU,		0,	0,	NO_INIT, {0}},
  {NODE_MEMORY_CTRL,	0,	0,	NO_INIT, {0}},
  {0}
};

static GCT_BINDINGS TL_bindings[] = {
  {NODE_CPU_MODULE,	NODE_HW_ROOT,		NODE_HW_ROOT,	NODE_HW_ROOT},
  {NODE_CPU,		NODE_CPU_MODULE,	NODE_HW_ROOT,	NODE_CPU_MODULE},
  {NODE_MEMORY_SUB,	NODE_HW_ROOT,		NODE_HW_ROOT,	NODE_HW_ROOT},
  {NODE_MEMORY_DESC,	NODE_MEMORY_SUB,	NODE_HW_ROOT,	NODE_HW_ROOT},
  {NODE_MEMORY_CTRL,	NODE_MEMORY_SUB,	NODE_HW_ROOT,	NODE_HW_ROOT},
  {NODE_IOP,		NODE_HW_ROOT,		NODE_HW_ROOT,	NODE_HW_ROOT},
  {NODE_HOSE,		NODE_IOP,		NODE_IOP,	NODE_IOP},
  {NODE_BUS,		NODE_HOSE,		NODE_HOSE,	NODE_HOSE},
  {NODE_IO_CTRL,	NODE_BUS,		NODE_BUS,	NODE_BUS},
  {NODE_SLOT,		NODE_BUS,		NODE_BUS,	NODE_BUS},
  {0}
};

static GCT_PLATFORM_DATA TL_data[] = {
  TWO_MEG,
  32*ONE_MEG,
  32*ONE_MEG,
  TWO_MEG,
  TWO_MEG,
  4,
  16,
  7,
  "TurboLaser",
  GCT__TURBOLASER
};

static GCT_PLATFORM_DATA rawhide_data[] = {
  TWO_MEG,
  32*ONE_MEG,
  32*ONE_MEG,
  TWO_MEG,
  TWO_MEG,
  2,
  16,
  7,
  "Rawhide",
  GCT__RAWHIDE
};

#endif





#if (CLIPPER || PC264)

/*Depth     Type      Subtype   ID     Flags          Private */
static GCT_PLATFORM_TREE regatta_tree[] = {
  {1, NODE_HW_ROOT,	0,	0,	0, {0}},
  {1, NODE_SW_ROOT,	0,	0,	0, {0}},
#if (CLIPPER)
  {1, NODE_TEMPLATE_ROOT,0,	0,	0, {0}},
  {2, NODE_CPU,		0,	0,	NODE_HW_TEMPLATE, {0}},
  {2, NODE_IOP,		0,	0,	NODE_HW_TEMPLATE, {0}},
  {2, NODE_MEMORY_DESC,	0,	0,	NODE_HW_TEMPLATE, {0,32*ONE_MEG}},
#endif
  {0}
};


#if (PC264)
static GCT_GALAXY_NVRAM generic_galaxy_config[] = {
  {NODE_COMMUNITY,      0,      0,      NO_INIT, {0}},
  {0}
};
#endif

#if (CLIPPER)
static GCT_MEM_FRAG_DEFS regatta_mem_frag_config[] = {
/*       Type          ID  Flags   PA   Size  */
  { NODE_PARTITION,     0,   0,     0,  0 },
  {0}
};
#endif

static GCT_BINDINGS generic_bindings[] = {
  {NODE_CPU,		NODE_HW_ROOT,		NODE_HW_ROOT,	NODE_HW_ROOT},
  {NODE_MEMORY_SUB,	NODE_HW_ROOT,		NODE_HW_ROOT,	NODE_HW_ROOT},
  {NODE_MEMORY_DESC,	NODE_MEMORY_SUB,	NODE_HW_ROOT,	NODE_HW_ROOT},
  {NODE_MEMORY_CTRL,	NODE_MEMORY_SUB,	NODE_HW_ROOT,	NODE_HW_ROOT},
  {NODE_IOP,		NODE_HW_ROOT,		NODE_HW_ROOT,	NODE_HW_ROOT},
  {NODE_HOSE,		NODE_IOP,		NODE_IOP,	NODE_IOP},
  {NODE_BUS,		NODE_HOSE,		NODE_HOSE,	NODE_HOSE},
  {NODE_IO_CTRL,	NODE_BUS,		NODE_BUS,	NODE_BUS},
  {NODE_SLOT,		NODE_BUS,		NODE_BUS,	NODE_BUS},
  {NODE_POWER_ENVIR,	NODE_HW_ROOT,		NODE_HW_ROOT,	NODE_HW_ROOT},
  {0}
};

/*
 *   cons
 *   base_alloc
 *   base_align
 *   min_alloc
 *   min_align
 *   max_part
 *   max_frag
 *   max_desc
 *   name
 *   system type cod
 */

#if CLIPPER
static GCT_PLATFORM_DATA generic_data[] = {
  TWO_MEG,
  32*ONE_MEG,
  32*ONE_MEG,
  ONE_MEG,
  ONE_MEG,
  2,
  16,
  4,
  "Compaq AlphaServer ES40",
  GCT__TSUNAMI
};
#endif

#if PC264
static GCT_PLATFORM_DATA generic_data[] = {
  TWO_MEG,
  32*ONE_MEG,
  32*ONE_MEG,
  ONE_MEG,
  ONE_MEG,
  1,
  1,
  1,
  "Compaq AlphaServer PC264",
  GCT__TSUNAMI
};
#endif

#endif   /* end if (clipper || pc264)  */




#if (WILDFIRE )
/*Depth     Type      Subtype   ID     Flags          Private */

static GCT_PLATFORM_TREE wildfire_tree[] = {
  {1, NODE_HW_ROOT,	0,	0,	0, {0}},
  {1, NODE_SW_ROOT,	0,	0,	0, {0}},
  {1, NODE_FRU_ROOT,	0,	0,	0, {0}},
  {0}
}; 

static GCT_GALAXY_NVRAM generic_galaxy_config[] = {
  {NODE_HARD_PARTITION,	0,	0,	NO_INIT, {0}},
  {NODE_COMMUNITY,	0,	0,	NO_INIT, {0}},
  {0}
};


static GCT_PLATFORM_TREE wildfire_tree_template[] = {
/*Depth     Type     Subtype   ID     Flags       Private */
  {1, NODE_HW_ROOT,	0,	0,	0, {0}},
  {0}
};

static char wildfire_id[] = "GALAXY_ID_WILD01";

static GCT_GALAXY_NVRAM wildfire_galaxy_config[] = {
  /*      type	    subtype	id	init		private */
  {NODE_HARD_PARTITION,	0,	0,	NO_INIT, {0}},
  {NODE_COMMUNITY,	0,	0,	NO_INIT, {0}},
  {0}
};

/*
 *   cons
 *   base_alloc
 *   base_align
 *   min_alloc
 *   min_align
 *   max_part
 *   max_frag
 *   max_desc
 *   name
 *   system type cod
 */

static GCT_PLATFORM_DATA wildfire_data[] = {
  TWO_MEG,
  64*ONE_MEG,
  64*ONE_MEG,
  8*ONE_MEG,
  8*ONE_MEG,
  8,
  16,
  8,
  "Wildfire",
  GCT__WILDFIRE 
};

static GCT_BINDINGS wildfire_bindings[] = {
  {NODE_SBB,		NODE_HW_ROOT,		NODE_HW_ROOT,	NODE_HW_ROOT},
  {NODE_CPU,		NODE_SBB,		NODE_HW_ROOT,	NODE_SBB},
  {NODE_MEMORY_SUB,	NODE_SBB,		NODE_HW_ROOT,	NODE_SBB},
  {NODE_MEMORY_DESC,	NODE_MEMORY_SUB,	NODE_HW_ROOT,	NODE_SBB},
  {NODE_MEMORY_CTRL,	NODE_MEMORY_SUB,	NODE_HW_ROOT,	NODE_SBB},
  {NODE_IOP,		NODE_SBB,		NODE_HW_ROOT,	NODE_SBB},
  {NODE_HOSE,		NODE_IOP,		NODE_IOP,	NODE_IOP},
  {NODE_BUS,		NODE_HOSE,		NODE_HOSE,	NODE_HOSE},
  {NODE_IO_CTRL,	NODE_BUS,		NODE_BUS,	NODE_BUS},
  {NODE_SLOT,		NODE_BUS,		NODE_BUS,	NODE_BUS},
  {0}
};

#endif
