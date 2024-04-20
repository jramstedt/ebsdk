#ifndef _GCT_TREE_STRINGS_H_
#define _GCT_TREE_STRINGS_H_

typedef struct _gct_names {
    __int64 bits;
    char *name;
    char fill_5 [4];
    } GCT_NAMES;


/*
 *  Some flags, type names, subtype names, and errors have been defined
 *  as both the portion of the string (sans the prefix) as well as a
 *  string suitable only for printing.  These are used for some parsing,
 *  but mostly for return to the user application.
 *
 */

static GCT_NAMES gct_flag_names[] = {
  {NODE_HOTSWAP,		"NODE_HOTSWAP"},
  {NODE_UNAVAILABLE,		"NODE_UNAVAILABLE"},
  {NODE_HARDWARE,		"NODE_HARDWARE"},
  {NODE_HW_TEMPLATE,		"NODE_HW_TEMPLATE"},
  {NODE_INITIALIZED,		"NODE_INITIALIZED"},
  {NODE_CPU_PRIMARY,		"NODE_CPU_PRIMARY"},
  {0, 				"**ERROR_LAST_FLAG**"},
};

static GCT_NAMES gct_flag_names_pretty[] = {
  {NODE_HOTSWAP,		"Hot Swap"},
  {NODE_UNAVAILABLE,		"Unavailable"},
  {NODE_HARDWARE,		"Hardware"},
  {NODE_HW_TEMPLATE,		"Template"},
  {NODE_INITIALIZED,		"Initialized"},
  {NODE_CPU_PRIMARY,		"Primary"},
  {0, 				"**ERROR_LAST_FLAG**"},
};

static GCT_NAMES gct_mem_flag_names[] = {
  {MEM_CONSOLE,		"MEM_CONSOLE"},
  {MEM_PRIVATE,		"MEM_PRIVATE"},
  {MEM_SHARED,		"MEM_SHARED"},
  {MEM_BASE,		"MEM_BASE"},
  {0, 			"**ERROR_LAST_FLAG**"},
};

static GCT_NAMES gct_mem_flag_names_pretty[] = {
  {MEM_CONSOLE,		"Console"},
  {MEM_PRIVATE,		"Private"},
  {MEM_SHARED,		"Shared"},
  {MEM_BASE,		"Base"},
  {0, 			"**ERROR_LAST_FLAG**"},
};

static GCT_NAMES gct_subtype_names[] = {
  {SNODE_CPU_NOPRIMARY,	"CPU_NOPRIMARY"},
  {SNODE_PCI,		"PCI"},
  {SNODE_EISA,		"EISA"},
  {SNODE_ISA,		"ISA"},
  {SNODE_XMI,		"XMI"},
  {SNODE_FBUS,		"FBUS"},
  {SNODE_XBUS,		"XBUS"},
  {SNODE_USB,		"USB"},
  {SNODE_SERIAL_PORT,	"SERIAL_PORT"},
  {SNODE_FLOPPY,	"FLOPPY"},
  {SNODE_PARALLEL_PORT,	"PARALLEL_PORT"},
  {SNODE_SCSI,		"SCSI"},
  {SNODE_IDE,		"IDE"},
  {SNODE_NI,		"NI"},
  {SNODE_FDDI,		"FDDI"},
  {SNODE_TOKEN_RING,	"TOKEN_RING"},
  {SNODE_NI_SCSI,	"NI_SCSI"},
  {SNODE_GRAPHICS,	"GRAPHICS"},
  {SNODE_ATM,		"ATM"},
  {SNODE_MEM_CHAN,	"MEM_CHAN"},
  {SNODE_CI,		"CI"},
  {0, 			"**ERROR_LAST_FLAG**"},
};

static GCT_NAMES gct_subtype_names_pretty[] = {
  {SNODE_CPU_NOPRIMARY,	"Non-Primary CPU"},
  {SNODE_PCI,		"PCI Bus"},
  {SNODE_EISA,		"EISA Bus"},
  {SNODE_ISA,		"ISA Bus"},
  {SNODE_XMI,		"XMI Bus"},
  {SNODE_FBUS,		"Future Bus"},
  {SNODE_XBUS,		"XBus"},
  {SNODE_USB,		"Universal Serial Bus"},
  {SNODE_SERIAL_PORT,	"Serial Port"},
  {SNODE_FLOPPY,	"Floppy"},
  {SNODE_PARALLEL_PORT,	"Parallel Port"},
  {SNODE_SCSI,		"SCSI Controller"},
  {SNODE_IDE,		"IDE Controller"},
  {SNODE_NI,		"Ethernet"},
  {SNODE_FDDI,		"FDDI"},
  {SNODE_TOKEN_RING,	"Token Ring"},
  {SNODE_NI_SCSI,	"SCSI/Ethernet Combo"},
  {SNODE_GRAPHICS,	"Graphics"},
  {SNODE_ATM,		"ATM"},
  {SNODE_MEM_CHAN,	"Memory Channel"},
  {SNODE_CI,		"CI Adapter"},
  {0, 			"**ERROR_LAST_FLAG**"},
};

static GCT_NAMES gct_type_names[] = {
  {NODE_ROOT,		"ROOT"},
  {NODE_HW_ROOT,	"HW_ROOT"},
  {NODE_SW_ROOT,	"SW_ROOT"},
  {NODE_TEMPLATE_ROOT,	"TEMPLATE_ROOT"},
  {NODE_COMMUNITY,	"COMMUNITY"},
  {NODE_PARTITION,	"PARTITION"},
  {NODE_SBB,		"SBB"},
  {NODE_PSEUDO,		"PSEUDO"},
  {NODE_CPU,		"CPU"},
  {NODE_MEMORY_SUB,	"MEMORY_SUB"},
  {NODE_MEMORY_DESC,	"MEMORY_DESC"},
  {NODE_MEMORY_CTRL,	"MEMORY_CTRL"},
  {NODE_IOP,		"IOP"},
  {NODE_HOSE,		"HOSE"},
  {NODE_BUS,		"BUS"},
  {NODE_IO_CTRL,	"IO_CTRL"},
  {NODE_SLOT,		"SLOT"},
  {NODE_CPU_MODULE,	"CPU_MODULE"},
  {NODE_POWER_ENVIR,    "POWER_ENVIR"},
  {NODE_FRU_ROOT,       "FRU_ROOT"},
  {NODE_FRU_DESC,       "FRU_DESC"},
  {NODE_SMB,            "SMB"},
  {NODE_CAB,            "CAB"},
  {NODE_CHASSIS,        "CHASSIS"},
  {NODE_EXP_CHASSIS,    "EXP_CHASSIS"},
  {0, 			"**ERROR_LAST_NAME**"},
};

static GCT_NAMES gct_type_names_pretty[] = {
  {NODE_ROOT,		"Root"},
  {NODE_HW_ROOT,	"Hardware Root"},
  {NODE_SW_ROOT,	"Software Root"},
  {NODE_TEMPLATE_ROOT,	"Template Root"},
  {NODE_COMMUNITY,	"Community"},
  {NODE_PARTITION,	"Partition"},
  {NODE_SBB,		"System Building Block"},
  {NODE_PSEUDO,		"Pseudo"},
  {NODE_CPU,		"CPU"},
  {NODE_MEMORY_SUB,	"Memory Subsystem"},
  {NODE_MEMORY_DESC,	"Memory Descriptor"},
  {NODE_MEMORY_CTRL,	"Memory Controller"},
  {NODE_IOP,		"IO Processor"},
  {NODE_HOSE,		"IO Hose"},
  {NODE_BUS,		"IO Bus"},
  {NODE_IO_CTRL,	"IO Controller"},
  {NODE_SLOT,		"Option Slot"},
  {NODE_CPU_MODULE,	"CPU Module"},
  {NODE_POWER_ENVIR,    "Power Envir"},
  {NODE_FRU_ROOT,       "FRU Root"},
  {NODE_FRU_DESC,       "FRU Descriptor"},
  {NODE_SMB,            "System MotherBoard"},
  {NODE_CAB,            "Cabinate"},
  {NODE_CHASSIS,        "Chassis"},
  {NODE_EXP_CHASSIS,    "Expand Chassis"},
  {0, 			"**ERROR_LAST_NAME**"},
};

static GCT_NAMES gct_os_names[] = {
  {OS_VMS,		"VMS"},
  {OS_OSF,		"OSF"},
  {OS_NT,		"NT"},
  {0, 			"**ERROR_LAST_FLAG**"},
};

static GCT_NAMES gct_os_names_pretty[] = {
  {OS_VMS,		"OpenVMS Alpha"},
  {OS_OSF,		"Digital UNIX"},
  {OS_NT,		"Microsoft Windows NT"},
  {0, 			"**ERROR_LAST_FLAG**"},
};

static GCT_NAMES gct_error_info[] = {
  { SUCCESS,		"SUCCESS" },
  { BADPARAM,		"BADPARAM" },
  { ILLEGAL,		"ILLEGAL" },
  { NOTFOUND,		"NOTFOUND" },
  { BADALIGN,		"BADALIGN" },
  { BADALLOC,		"BADALLOC" },
  { OVERLAP,		"OVERLAP" },
  { NOTINITIALIZED,	"NOTINITIALIZED" },
  { BADHANDLE,		"BADHANDLE" },
  { NOTDELETED,		"NOTDELETED" },
  { MAXEXCEEDED,	"MAXEXCEEDED" },
  { NOTALLOWED,		"NOTALLOWED" },
  { BADOWNER,		"BADOWNER" },
  { ALREADYOWNED,	"ALREADYOWNED" },
  { NOTPARTITION,	"NOTPARTITION" },
  { NOTCOMMUNITY,	"NOTCOMMUNITY" },
  { ILLEGALTREE,	"ILLEGALTREE" },
  { NOTHARDWARE,	"NOTHARDWARE" },
  { NOMEMORY,		"NOTMEMORY" },
  { BADPA,		"BADPA" },
  { BADSIZE,		"BADSIZE" },
  { TREELOCKED,		"TREELOCKED" },
  { TREEBEINGUPDATED,	"TREEBEINGUPDATED" },
  { BADUPDATELEVEL,	"BADUPDATELEVEL" },
  { BADTREEINTEGRITY,	"BADTREEINTEGRITY" },
  { NOTMEMORYDESC,	"NOTMEMORYDESC" },
  { NOTLOCKED,		"NOTLOCKED" },
  { 0 }
};

static GCT_NAMES gct_error_info_pretty[] = {
  { SUCCESS,		"Normal completion" },
  { BADPARAM,		"Bad parameter" },
  { ILLEGAL,		"Illegal operation" },
  { NOTFOUND,		"Item not found" },
  { BADALIGN,		"Bad Alignment" },
  { BADALLOC,		"Bad Allocation" },
  { OVERLAP,		"Fragment overlaps existing fragments" },
  { NOTINITIALIZED,	"Partition was not initialized" },
  { BADHANDLE,		"The handle does not represent a valid component" },
  { NOTDELETED,		"Not deleted" },
  { MAXEXCEEDED,	"Maximum exceeded" },
  { NOTALLOWED,		"Operation not allowed" },
  { BADOWNER,		"A component had a owner that is incompatable with the request" },
  { ALREADYOWNED,	"Already owned" },
  { NOTPARTITION,	"Not a partition node" },
  { NOTCOMMUNITY,	"Not a community node" },
  { ILLEGALTREE,	"Illegal tree detected" },
  { NOTHARDWARE,	"Not hardware" },
  { NOMEMORY,		"Config tree memory exhausted" },
  { BADPA,		"The PA is invalid or out of range" },
  { BADSIZE,		"The size is invalid or out of range" },
  { TREELOCKED,		"The tree is locked by another user" },
  { TREEBEINGUPDATED,	"The tree is in the process of being updated" },
  { BADUPDATELEVEL,	"The update level is not what was expected" },
  { BADTREEINTEGRITY,	"The tree update level is out of sync, and tree may be corrupt" },
  { NOTMEMORYDESC,	"Not a memory descriptor" },
  { NOTLOCKED,		"The tree is not locked" },
  { 0 }
};


GCT_NAMES *name_array[] = {
    gct_flag_names,
    gct_mem_flag_names,
    gct_subtype_names,
    gct_type_names,
    gct_os_names,
    gct_error_info
};

GCT_NAMES *pretty_name_array[] = {
    gct_flag_names_pretty,
    gct_mem_flag_names_pretty,
    gct_subtype_names_pretty,
    gct_type_names_pretty,
    gct_os_names_pretty,
    gct_error_info_pretty
};


#endif
