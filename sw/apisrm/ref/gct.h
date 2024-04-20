/*
 *
 *  GCT.H    Main structures for GCT implemenation.
 *
 */

#ifndef __GCTDEF_LOADED
#define __GCTDEF_LOADED 1
 
#pragma __nostandard			 /* This file uses non-ANSI-Standard features */
#pragma __member_alignment __save
#pragma __nomember_alignment
#ifdef __INITIAL_POINTER_SIZE			 /* Defined whenever ptr size pragmas supported */
#pragma __required_pointer_size __save		 /* Save the previously-defined required ptr size */
#pragma __required_pointer_size __short		 /* And set ptr size default to 32-bit pointers */
#endif
 
#ifdef __cplusplus
    extern "C" {
#define __unknown_params ...
#define __optional_params ...
#else
#define __unknown_params
#define __optional_params ...
#endif
 
#ifndef __struct
#if !defined(__VAXC)
#define __struct struct
#else
#define __struct variant_struct
#endif
#endif
 
#ifndef __union
#if !defined(__VAXC)
#define __union union
#else
#define __union variant_union
#endif
#endif
 
/*
 * Some handy macros for turning a handle into an address,
 * and an address into a handle
 */
#define _GCT_MAKE_ADDRESS(_gct_o) ((GCT_NODE *)((char *) GCT_POINTER_TO_ROOT + _gct_o));
#define _GCT_MAKE_HANDLE(_gct_a) ((char *) _gct_a - (char *) GCT_POINTER_TO_ROOT);
#define GALAXY_ID_LENGTH 16
/*                                                                          */
/*  typedef the ID and HANDLE                                               */
/*                                                                          */
typedef unsigned __int64 GCT_ID;
typedef int GCT_HANDLE;
/*                                                                          */
/*  Success codes                                                           */
/*                                                                          */
#define SUCCESS 1
#define NOSTATUS 0
/*                                                                          */
/*  Error codes.  All negative, all even (low bit clear)                    */
/*  This allows BLISS tests for errors on the low bit to                    */
/*  work correctly... even though they are not VMS error                    */
/*  codes.                                                                  */
/*                                                                          */
#define BADPARAM -2                     /* Bad parameter in call            */
#define ILLEGAL -4                      /* Operation is illegal             */
#define NOTFOUND -6                     /* Lookup failed                    */
#define BADALIGN -8                     /* Bad PA alignment                 */
#define BADALLOC -10                    /* Invalid size increment           */
#define OVERLAP -12                     /* Fragment overlaps with existing  */
#define NOTINITIALIZED -14              /* Tree not initialized             */
#define BADHANDLE -16                   /* Illegal HANDLE                   */
#define NOTDELETED -18                  /* Node was not deleted             */
#define MAXEXCEEDED -20                 /* Too many memory fragments        */
#define NOTALLOWED -22                  /* Operation is not allowed         */
#define BADOWNER -24                    /* Node does not have the right owner */
#define ALREADYOWNED -26                /* Node already has an owner        */
#define NOTPARTITION -28                /* The node is not a partition      */
#define NOTCOMMUNITY -30                /* The node is not a community      */
#define ILLEGALTREE -32                 /* Corrupt tree state               */
#define NOTHARDWARE -34                 /* The node is not a hardware component */
#define NOMEMORY -36                    /* Failed to allocate the node      */
#define BADPA -38                       /* PA is not in the memory system   */
#define BADSIZE -40                     /* Size of the fragment is not valid */
#define TREELOCKED -42                  /* Attempt to lock a locked tree    */
#define BADUPDATELEVEL -44              /* Illegal update level input       */
#define NOTMEMORYDESC -46               /* Not a memory descriptor node     */
#define NOTLOCKED -48                   /* Tried to unlock an unlocked tree */
#define UNAVAILABLE -50                 /* Node is not available            */
#define STILLACTIVE -52                 /* The HW component is still active */
#define CHILDSTILLACTIVE -54            /* A component part of the HW is still active */
#define CHILDALREADYOWNED -56           /* A child is owned.  This is an internal error! */
#define NOTSHARED -58                   /* The device can't be assigned because an ancestor is not shared */
#define TREEBEINGUPDATED -60            /* Update in progress               */
#define BADTREEINTEGRITY -62            /* Tree corruption detected         */
#define NOTHARDPARTITION -64            /* The node is not a hard partition */
/*                                                                          */
/*  Node TYPE codes                                                         */
/*                                                                          */
#define NODE_ROOT 1                     /* Root node                        */
#define NODE_HW_ROOT 2                  /* Hardware Root                    */
#define NODE_SW_ROOT 3                  /* Software Root                    */
#define NODE_TEMPLATE_ROOT 4            /* Template Root                    */
#define NODE_COMMUNITY 5                /* Community                        */
#define NODE_PARTITION 6                /* Partition                        */
#define NODE_SBB 7                      /* System Building Block            */
#define NODE_PSEUDO 8                   /* Pseudo device                    */
#define NODE_CPU 9                      /* CPU                              */
#define NODE_MEMORY_SUB 10              /* Memory Subsystem                 */
#define NODE_MEMORY_DESC 11             /* Memory Description               */
#define NODE_MEMORY_CTRL 12             /* Memory Controller                */
#define NODE_IOP 13                     /* IO Processor                     */
#define NODE_HOSE 14                    /* IO Hose                          */
#define NODE_BUS 15                     /* Option Bus                       */
#define NODE_IO_CTRL 16                 /* IO Controller                    */
#define NODE_SLOT 17                    /* Option slot                      */
#define NODE_CPU_MODULE 18              /* CPU module board                 */
#define NODE_POWER_ENVIR 19             /* Environmental monitor & control  */
#define NODE_FRU_ROOT 20                /* Root of FRU subtree              */
#define NODE_FRU_DESC 21                /* FRU Descriptor node              */
#define NODE_SMB 22                     /* System Motherboard               */
#define NODE_CAB 23                     /* Cabinet enclosure                */
#define NODE_CHASSIS 24                 /* System chassis or enclosure      */
#define NODE_EXP_CHASSIS 25             /* Expansion chasis or enclosure    */
#define NODE_SYS_INTER_SWITCH 26        /* System Interconnect Switch       */
#define NODE_HARD_PARTITION 27          /* Hard Partition (Firewall)        */
#define NODE_LAST 28                    /* Always Last                      */
/*                                                                          */
/*  Routine index values for console callbacks                              */
/*                                                                          */
#define READ_LOCK 1                     /* Take out a read lock             */
#define READ_UNLOCK 2                   /* Release the read lock            */
#define SET_ACTIVE 3                    /* Set a node active (current_owner) */
#define SET_INACTIVE 4                  /* Set a node inactive (current_owner) */
#define ASSIGN_FRAGMENT 5               /* Assign a memory fragment         */
#define DEASSIGN_FRAGMENT 6             /* Deassign a memory fragment       */
#define CREATE_COMMUNITY 7              /* Create a community node          */
#define DELETE_COMMUNITY 8              /* Delete a community               */
#define CREATE_PARTITION 9              /* Create a partition node          */
#define DELETE_PARTITION 10             /* Delete a partition               */
#define ASSIGN_HW 11                    /* Assign HW to a community or partition */
#define DEASSIGN_HW 12                  /* Deassign HW                      */
#define FIND_PARTITION 13               /* Find Partition                   */
#define FIND_NODE 14                    /* Search for a component           */
#define GET_TEXT 15                     /* Get a text string                */
#define UPDATE_GMDB 16                  /* Update GMDB area in community    */
#define VALIDATE_PARTITION 17           /* Validate a partition             */
#define INITIALIZE_PARTITION 18         /* Initialize a partition           */
#define UPDATE_GALAXY_ID 19             /* Update the Galaxy ID             */
#define GET_MAX_PARTITION 20            /* Get the max partition count      */
#define UPDATE_INSTANCE_NAME 21         /* Read/Write Instance Name         */
#define SAVE_CONFIG 22                  /* Save configuration               */
#define GET_SENSOR_INFO 23              /* Get sensor information           */
#define CREATE_HARD_PARTITION 24        /* Create a hard partition node     */
#define DELETE_HARD_PARTITION 25        /* Delete a hard partition          */
/*                                                                          */
/*  Node subtype codes                                                      */
/*                                                                          */
#define SNODE_UNKNOWN 0                 /* Unknown subtype                  */
/* CPU codes                                                                */
#define SNODE_CPU_NOPRIMARY 1           /* A CPU not capable of being a primary */
/* Bus codes                                                                */
#define SNODE_PCI 2                     /* Peripheral Component Interconnect */
#define SNODE_EISA 3                    /* Extended ISA bus                 */
#define SNODE_ISA 4                     /* Industry Standard Architecture bus */
#define SNODE_XMI 5                     /* XMI bus                          */
#define SNODE_FBUS 6                    /* FutureBus                        */
#define SNODE_XBUS 7                    /* Built in device bus              */
#define SNODE_USB 8                     /* Univeral Serial Bus              */
/* IO controller codes                                                      */
#define SNODE_SERIAL_PORT 9             /* Serial port                      */
#define SNODE_FLOPPY 10                 /* Standard Floppy                  */
#define SNODE_PARALLEL_PORT 11          /* Parallel port                    */
#define SNODE_SCSI 12                   /* SCSI Controller                  */
#define SNODE_IDE 13                    /* IDE Controller                   */
#define SNODE_NI 14                     /* Ethernet Controller              */
#define SNODE_FDDI 15                   /* FDDI                             */
#define SNODE_TOKEN_RING 16             /* Token Ring                       */
#define SNODE_NI_SCSI 17                /* Combo card                       */
#define SNODE_GRAPHICS 18               /* Graphics Controller              */
#define SNODE_ATM 19                    /* ATM Controller                   */
#define SNODE_MEM_CHAN 20               /* Memory Channel                   */
#define SNODE_CI 21                     /* CI adapter                       */
#define SNODE_1394 22                   /*                                  */
#define SNODE_AGP 23                    /* AGP (Graphics Port)              */
#define SNODE_SUPER_HIPPI 24            /* Super Hippi                      */
#define SNODE_FIBRECHANNEL 25           /* Fibrechannel                     */
#define SNODE_CAB 26                    /* Cabinet ???                      */
#define SNODE_CHASSIS 27                /* System chassis ???               */
#define SNODE_EXP_CHASSIS 28            /* Expand system chassis ???        */
#define SNODE_POWER_SUPPLY 29           /* Power Envir - Power Supply       */
#define SNODE_COOLING 30                /* Power Envir - Cooling            */
#define SNODE_SIMM 31                   /* SIMM memory card                 */
#define SNODE_DIMM 32                   /* DIMM memory card                 */
#define SNODE_RIMM 33                   /* RIMM memory card                 */
#define SNODE_CPU_MODULE 34		/*					*/
#define SNODE_CPU_CACHE_MODULE 35	/*					*/
#define SNODE_CPU_MEMORY_MODULE 36	/*					*/
#define SNODE_MEMORY_CARRIER_MODULE 37	/*					*/
#define SNODE_BACKPLANE_ASSY 38		/*					*/
#define SNODE_MOTHER_BOARD 39		/*					*/
#define SNODE_FAN 40			/*					*/
#define SNODE_SYS_INTERCONN_PORT 41	/*					*/
#define SNODE_SYS_INTERCONN_SWITCH 42	/*					*/
#define SNODE_MEMORY_COHERENCY_MODULE 43 /*					*/
#define SNODE_IO_PORT_MODULE 44		/*					*/
#define SNODE_REMOTE_MGNT_MODULE 45	/*					*/
#define SNODE_POWER_ENV_CTRL_MODULE 46	/*					*/
#define SNODE_MULTI_DEVICE 47		/*					*/
#define SNODE_IO_EXP_CAB 48		/*					*/
#define SNODE_EXPANSION_CAB 49		/*					*/
#define SNODE_EXP_CHASSIS 50		/*					*/
#define SNODE_DAUGHTER_CARD 51		/*					*/
#define SNODE_RISER_CARD 52		/*					*/
#define SNODE_LAST 53                   /* Highest                          */

/*                                                                          */
/*  Lookup flags                                                            */
/*                                                                          */
#define FIND_ANY 0
#define FIND_BY_OWNER 1
#define FIND_BY_TYPE 2
#define FIND_BY_SUBTYPE 4
#define FIND_BY_ID 8
#define FIND_UNAVAILABLE 16
/*                                                                          */
/*  Lock types                                                              */
/*                                                                          */
#define LOCK_FOR_READ 1                 /* Just want to read the tree       */
#define LOCK_FOR_UPDATE 2               /* Want to update the tree          */
#define UNLOCK_READ 3                   /* Unlock a read                    */
#define UNLOCK_UPDATE 4                 /* Unlock an update                 */
#define UNLOCK_NO_UPDATE 5              /* Unlock an update lock with no update */
/*                                                                          */
/*  FRU TLV tag codes                                                       */
/*                                                                          */
#define TLV_TAG_ISOLATIN1 1             /* ISO Latin-1 encoded              */
#define TLV_TAG_QUOTED 2                /* Quoted string                    */
#define TLV_TAG_BINARY 3                /* Binary data                      */
#define TLV_TAG_UNICODE 4               /* 16-bit UNICODE                   */
/*                                                                          */
/*  Magic                                                                   */
/*                                                                          */
#define NODE_VALID 1498958919
/*                                                                          */
/*  OS types                                                                */
/*                                                                          */
#define OS_VMS 1                        /* OS is VMS on Partition           */
#define OS_OSF 2                        /* OS is OSF on Partition           */
#define OS_NT 3                         /* OS is NT  on Partition           */
#define OS_LINUX 4                      /* OS is LINUX on Partition         */
#define OS_LAST 5                       /* Always last                      */
/*                                                                          */
/* Subpacket definitions                                                    */
/*                                                                          */
#define CLASS1 1
#define CLASS2 2
#define CLASS_CLIPPER 12
#define CACHE_OTHER 0x01
#define CACHE_UNKNOWN 0x02
#define CACHE_LVL_PRIMARY 0x03
#define CACHE_LVL_SECONDARY 0x04
#define CACHE_LVL_TERTIARY 0x05
#define CACHE_WP_WRITEBACK 0x03
#define CACHE_WP_WRITETHROUGH 0x04
#define CACHE_WP_LATEWRITE 0x05
#define CACHE_EC_NONE 0x03
#define CACHE_EC_PARITY 0x04
#define CACHE_EC_SINGLEBITECC 0x05
#define CACHE_EC_MULTIBITECC 0x06
#define CACHE_TYPE_INSTRUCTION 0x03
#define CACHE_TYPE_DATA 0x04
#define CACHE_TYPE_UNIFIED 0x05
#define CACHE_STAT_ENABLED 0x03
#define CACHE_STAT_DISABLED 0x04
#define CACHE_STAT_NOTAPPLY 0x05

#define BRIDGE_LVL_PRIMARY 0x01
#define BRIDGE_LVL_SECONDARY 0x02
#define BRIDGE_LVL_TERTIARY 0x03
#define BRIDGE_BUS_TYPE_HOSE 0x01
#define BRIDGE_BUS_TYPE_PCI 0x02
#define BRIDGE_BUS_TYPE_XMI 0x03
#define BRIDGE_BUS_TYPE_FBUS 0x04
#define BRIDGE_BUS_TYPE_VME 0x05
#define BRIDGE_BUS_TYPE_ISA 0x06

#define POWER_PS_TYPE_DC 0x01
#define POWER_PS_TYPE_AC 0x02
#define POWER_PS_TYPE_GNDCUR 0x03
#define POWER_PS_TYPE_BBU 0x04
#define POWER_PS_TYPE_UPS 0x05

#define SENSOR_CLASS_FAN 1
#define SENSOR_CLASS_TEMP 2
#define SENSOR_CLASS_ACPWR 3
#define SENSOR_CLASS_DCPWR 4
#define SENSOR_CLASS_BATTERY 5

#define SENSOR_PROP_STATUS 1
#define SENSOR_PROP_VALUE 2
#define SENSOR_PROP_WRITEABLE 4
#define SENSOR_PROP_UPPERLONG_BITSUPPORT 8

#define CSL_RESET_REASON_UNKNOWN 0
#define CSL_RESET_REASON_SOFT_REQ 0x02
#define CSL_RESET_REASON_HARD_RESET 0x04
#define CSL_RESET_REASON_OCP_RESET 0x08
#define CSL_RESET_REASON_REMOTE_RESET 0x10

/*                                                                          */
/*  JEDEC layout DIAG_FLAGs                                                 */
/*                                                                          */
#define DIAG_FLAG_FAILURE             0x01
#define DIAG_FLAG_TDD_ERR             0x02
#define DIAG_FLAG_SDD_ERR             0x04
#define DIAG_FLAG_RESERVED            0X08
#define DIAG_FLAG_CHKSUM_0_62         0x10
#define DIAG_FLAG_CHKSUM_64_126       0x20
#define DIAG_FLAG_CHKSUM_128_254      0x40
#define DIAG_FLAG_SYS_SERIAL_MISMATCH 0x80

/*                                                                          */
/*  Version 5.2                                                             */
/*                                                                          */
#define REVISION_MAJOR 5
#if TURBO | RAWHIDE
#define REVISION_MINOR 1
#else
#define REVISION_MINOR 2
#endif

typedef struct _gct_buffer_header {
    unsigned __int64 buffer_cksum;      /* Placeholder for checksum         */
    unsigned   int   buffer_size;       /* Size of structure                */
    __union  {
        unsigned int rev_full;          /* Structure revision               */
        __struct  {
            unsigned short int rev_major;
            unsigned short int rev_minor;
            } sub;
        } revision;
    unsigned char galaxy_enable;        /* Supports Galaxy                  */
    unsigned char galaxy_callbacks;     /* Supports Galaxy Callbacks        */
    unsigned char buffer_reserved [46]; /* Pad to 64 bytes                  */
    } GCT_BUFFER_HEADER;
 
#if !defined(__VAXC)
#define rev_full revision.rev_full
#define rev_minor revision.sub.rev_minor
#define rev_major revision.sub.rev_major
#endif		/* #if !defined(__VAXC) */
 
/*                                                                          */
/*  TLV strings...  To access them -- you need to use the tag to understand */
/*  the data type, the length to get the number of bytes.  This structure   */
/*  contains:                                                               */
/*                                                                          */
	
typedef struct _gct_tlv {
    unsigned short int tlv_tag;         /* The type of data                 */
    unsigned short int tlv_length;      /* Its length                       */
    unsigned char tlv_value [4];        /* The first byte(s) in the value   */
    } GCT_TLV;
/*                                                                          */
/*  Values for text lookups                                                 */
/*                                                                          */
#define NAME_IS_PRETTY 256              /* Return plain text                */
#define NAME_IS_FLAG 512                /* Input is flags                   */
#define FLAG_NAMES 512                  /* Decode gct$q_node_flags          */
#define MEM_FLAG_NAMES 513              /* Decode mem_flags                 */
#define SUBTYPE_NAMES 2                 /* Decode Subtypes                  */
#define TYPE_NAMES 3                    /* Decode Types                     */
#define OS_NAMES 4                      /* Decode OS name                   */
#define ERROR_TEXT 5                    /* Decode Error value               */
#define RTN_NAMES 6                     /* Callback routine names           */
#define LOCK_TYPES 7                    /* Lock types for calls             */
/*                                                                          */
/*  An array of bindings are used to tell the code that populates a tree    */
/*  what the config and affinity bindings are for any node type.            */
/*                                                                          */
	
typedef struct _gct_bindings {
    unsigned char bind_type;            /* Node type                        */
    unsigned char bind_parent;          /* Type of parent                   */
    unsigned char bind_config;          /* Config binding                   */
    unsigned char bind_affinity;        /* Affinity binding                 */
    } GCT_BINDINGS;
#define DIAG_ST_NA 0x1
#define DIAG_ST_PASSED 0x2
#define DIAG_RSRVD_03_31 0xFFFFFFFC
	
typedef struct _gct_diag_flags {
    __union  {
        unsigned int diag_flags;        /* diagnostic flags                 */
        __struct  {
            unsigned diag_st_na : 1;    /* Diagnostic Test Not Available    */
            unsigned diag_st_passed : 1; /* Diagnostic passed self test     */
            unsigned diag_rsrvd_03_31 : 30; /* Unused bits                  */
            } diag_flag_bits;
        } diag_flag_union;
    } GCT_DIAG_FLAGS;
 
#if !defined(__VAXC)
#define diag_flags diag_flag_union.diag_flags
#define diag_st_na diag_flag_union.diag_flag_bits.diag_st_na
#define diag_st_passed diag_flag_union.diag_flag_bits.diag_st_passed
#define diag_rsrvd_03_31 diag_flag_union.diag_flag_bits.diag_rsrvd_03_31
#endif		/* #if !defined(__VAXC) */
 
	
typedef struct _gct_diag_failure {
    unsigned char failure_type_rev;     /* type nibble (0:3) and revision (4:7) */
    unsigned char failure_test;         /* Test                             */
    unsigned char failure_subtest;      /* Subtest code                     */
    unsigned char failure_error;        /* Error code                       */
    } GCT_DIAG_FAILURE;
/*                                                                          */
/*  System Resource Configuration Subpacket header format                   */
/*                                                                          */
	
typedef struct _gct_subpack {
    unsigned short int subpack_length;  /* Length                           */
    unsigned short int subpack_class;   /* Class                            */
    unsigned short int subpack_type;    /* Type                             */
    unsigned short int subpack_rev;     /* Revision                         */
    unsigned       int subpack_diag_flag;   /* Diagnostic flags             */
    GCT_DIAG_FAILURE subpack_diag_failure_info; /* Diagnostic failure info  */
    } GCT_SUBPACK;
/*                                                                          */
/*  Subpacket TYPE codes for CLASS 1 (System Resource Conf Subpackets).     */
/*                                                                          */
#define SUBPACK_UNUSED_1 1              /* Used to be SYSTEM_PLATFORM       */
#define SUBPACK_PROCESSOR 2             /* Processor                        */
#define SUBPACK_MEMORY 3                /*                                  */
#define SUBPACK_SYS_BUS_BRIDGE 4        /*                                  */
#define SUBPACK_PCI_DEVICE 5            /*                                  */
#define SUBPACK_UNUSED_6 6              /* Used to be EISA_DEVICE           */
#define SUBPACK_CACHE 7                 /*                                  */
#define SUBPACK_POWER 8                 /*                                  */
#define SUBPACK_COOLING 9               /*                                  */
#define SUBPACK_SYS_INIT_LOG 10         /*                                  */
#define SUBPACK_UNUSED_11 11            /* Used to be XMI_DEVICE            */
#define SUBPACK_UNUSED_12 12            /* Used to be FBUS_DEVICE           */
#define SUBPACK_VME 13                  /*                                  */
#define SUBPACK_SBB 14                  /* System Building Block            */
#define SUBPACK_IOP 15                  /*                                  */
#define SUBPACK_HOSE 16                 /*                                  */
#define SUBPACK_BUS 17                  /*                                  */
#define SUBPACK_ISA_DEVICE 18           /*                                  */
#define SUBPACK_USB_DEVICE 19           /*                                  */
#define SUBPACK_CONSOLE 20              /*                                  */
#define SUBPACK_POWER_ENVIR 21          /*                                  */
#define SUBPACK_UNUSED_22 22            /*                                  */
#define SUBPACK_UNUSED_23 23            /*                                  */
#define SUBPACK_UNUSED_24 24            /*                                  */
#define SUBPACK_UNUSED_25 25            /*                                  */
#define SUBPACK_UNUSED_26 26            /*                                  */
#define SUBPACK_UNUSED_27 27            /*                                  */
#define SUBPACK_UNUSED_28 28            /*                                  */
#define SUBPACK_UNUSED_29 29            /*                                  */
#define SUBPACK_PCI_VPD 30              /*                                  */
#define SUBPACK_SMB 31                  /* System Mother Board              */
#define SUBPACK_FIBRECHANNEL 32         /*                                  */
#define SUBPACK_AGP 33                  /*                                  */
#define SUBPACK_IDE 34                  /*                                  */
#define SUBPACK_SCSI 35                 /*                                  */
#define SUBPACK_1394 36                 /*                                  */
#define SUBPACK_SUPER_HIPPI 37          /*                                  */
#define SUBPACK_MEMORY_DIR 38           /*                                  */
#define SUBPACK_NUMA_PORT 39            /*                                  */
#define SUBPACK_NUMA_SWITCH 40          /* Heirarchical switch              */
#define SUBPACK_RMC 41                  /* Remote Management Controller     */
#define SUBPACK_SENSOR 42		/* Sensor info                      */

typedef struct _pci_space {
    unsigned __int64  pci_space0;
    unsigned __int64  pci_space1;
    unsigned __int64  pci_space2;
    unsigned __int64  pci_space3;
    unsigned __int64  pci_space4;
    unsigned __int64  pci_space5;
    unsigned __int64  pci_space6;
    unsigned __int64  pci_space7;
} PCI_SPACE;

/* SUBPACK Class 1 Type 2 */
typedef struct subpkt_cpu_fru5 {
    GCT_SUBPACK      cpu_sub;
    unsigned     int processor_id;
    unsigned     int processor_family;
    unsigned __int64 cpu_state;
    unsigned __int64 ovms_palcode_revision;
    unsigned __int64 dunix_palcode_revision;
    unsigned __int64 wnt_palcode_revision;
    unsigned __int64 alpha_type;
    unsigned __int64 processor_variation;
    GCT_TLV          processor_manufacturer;
    GCT_TLV          processor_serial_number;
    GCT_TLV          processor_revision_level;
} SUBPKT_CPU_FRU5;

/* SUBPACK Class 1 Type 3 */
typedef struct subpkt_memory_fru5 {
    GCT_SUBPACK      memory_sub;
    unsigned __int64 mem_id;
    unsigned     int reserved;
    unsigned     int register_count;
#ifdef PLATFORM_MEM_FRU_EXT
                     PLATFORM_MEM_FRU_EXT
#endif
} SUBPKT_MEMORY_FRU5;

/* SUBPACK Class 1 Type 4 */
typedef struct subpkt_busbridge_fru5 {
    GCT_SUBPACK      busbridge_sub;
    unsigned   short bridge_level;
    unsigned   short bridge_type;
    unsigned     int register_count;
    unsigned __int64 physical_address;
} SUBPKT_BUSBRIDGE_FRU5;

/* SUBPACK Class 1 Type 5 */
typedef struct subpkt_pci_fru5 {
    GCT_SUBPACK      pci_sub;
    unsigned __int64 pci_config_addr;
    PCI_SPACE        pci_head;
} SUBPKT_PCI_FRU5;
    
/* SUBPACK Class 1 Type 7 */
typedef struct subpkt_cache_fru5 {
    GCT_SUBPACK      cache_sub;
    unsigned   short cache_level;
    unsigned   short cache_speed;
    unsigned   short cache_size;
    unsigned   short size_avail;
    unsigned   short cache_wp;
    unsigned   short cache_ec;
    unsigned   short cache_type;
    unsigned   short cache_state;
} SUBPKT_CACHE_FRU5;

/* SUBPACK Class 1 Type 8 */
typedef struct subpkt_power_fru5 {
    GCT_SUBPACK      power_sub;
    unsigned     int fru_count;
    unsigned     int fru;
    unsigned   short ps_type;
    unsigned   short ps_id;
    unsigned     int ps_status;
} SUBPKT_POWER_FRU5;

/* SUBPACK Class 1 Type 9 */
typedef struct subpkt_cooling_fru5 {
    GCT_SUBPACK      cooling_sub;
    unsigned     int fru_count;
    unsigned     int fru;
    unsigned   short cool_type;
    unsigned   short cool_id;
    unsigned     int cool_status;
} SUBPKT_COOLING_FRU5;

/* SUBPACK Class 1 Type 17 */
typedef struct subpkt_bus_fru5 {
    GCT_SUBPACK      bus_sub;
    unsigned __int64 bus_id;
    unsigned     int reserved;
    unsigned     int register_count;
} SUBPKT_BUS_FRU5;

/* SUBPACK Class 1 Type 20 */
typedef struct subpkt_csl_fru5 {
    GCT_SUBPACK      csl_sub;
    unsigned     int reset_reason;
    unsigned     int ev_count;
    GCT_TLV          srm_console_part_number;
    GCT_TLV          srm_console_rev;
    GCT_TLV          alphabios_part_number;
    GCT_TLV          alphabios_rev;
} SUBPKT_CSL_FRU5;

/* SUBPACK Class 1 Type 30 */
typedef struct subpkt_pci_vpd_fru5 {
    GCT_SUBPACK        pci_vpd_sub;
    unsigned   __int64 load_id;
    unsigned   __int64 ROM_level;
    unsigned   __int64 ROM_rev;
    unsigned   __int64 net_addr;
    unsigned   __int64 dev_driv_level;
    unsigned   __int64 diag_level;
    unsigned   __int64 load_ucode_level;
    unsigned   __int64 bin_func_num;
} SUBPKT_PCI_VPD_FRU5;

/* SUBPACK Class 1 Type 31 */
typedef struct subpkt_smb_fru5 {
    GCT_SUBPACK      smb_sub;
    unsigned __int64 smb_id;
    unsigned     int reserved;
    unsigned     int register_count;
} SUBPKT_SMB_FRU5;

/* SUBPACK Class 1 Type 41 */
typedef struct subpkt_rmc_fru5 {
    GCT_SUBPACK      rmc_sub;
    unsigned __int64 rmc_id;
    unsigned     int rmc_pic_hw_rev;
    unsigned     int rmc_pic_fw_rev;
} SUBPKT_RMC_FRU5;

/* SUBPACK Class 1 Type 42 */
typedef struct subpkt_sensor_fru5 {
    GCT_SUBPACK      sensor_sub;
    unsigned     int fru_count;
    unsigned     int fru_offset;
    unsigned __int64 console_id;
    unsigned     int sensor_class;
    unsigned     int sensor_properties;
} SUBPKT_SENSOR_FRU5;


/*                                                                          */
/*  Define the header extension structure.  Offset to by hd_extension       */
/*  in the common header.  If zero, no extended header information is       */
/*  present for the node.                                                   */
/*                                                                          */
	
typedef struct _gct_hd_ext {
    unsigned    int fru_count;          /* # of FRUs for this type          */
    GCT_HANDLE  fru;                    /* Handle of list of FRUs           */
    unsigned    int subpkt_count;       /* Number of subpackets             */
    unsigned    int subpkt_offset;      /* First Subpacket                  */
    } GCT_HD_EXT;

/*                                                                          */
/*  Define the node structure                                               */
/*                                                                          */
#define NODE_HARDWARE 0x1
#define NODE_HOTSWAP 0x2
#define NODE_UNAVAILABLE 0x4
#define NODE_HW_TEMPLATE 0x8
#define NODE_INITIALIZED 0x10
#define NODE_CPU_PRIMARY 0x20
#define NODE_IN_CONSOLE 0x40
#define NODE_RSRVD_7_31 0xFFFFFF80
#define NODE_RSRVD_32_63 0xFFFFFFFF00000000
	
typedef struct _gct_node {
    unsigned char type;                 /* Node type                        */
    unsigned char subtype;              /* Type-specific subtype            */
    unsigned short int size;            /* Size of node including extended header if present */
    int hd_extension;                   /* Offset from start structure to extended header data */
    __union  {
        unsigned __int64 owner_both;    /* Both owner and active owner      */
        __struct  {
            GCT_HANDLE owner;           /* Software owner of node           */
            GCT_HANDLE current_owner;   /* Active user of the node          */
            } owner_each;
        } owner_union;
    GCT_ID id;                          /* Type-specific ID                 */
    __union  {
        unsigned __int64 node_flags;    /* flags                            */
        __struct  {
            unsigned node_hardware    : 1; /* Node represents hardware      */
            unsigned node_hotswap     : 1; /* Hardware can be hitswapped    */
            unsigned node_unavailable : 1; /* Hardware is not avail (power down) */
            unsigned node_hw_template : 1; /* Node is a template device     */
            unsigned node_initialized : 1; /* Partition is initialized      */
            unsigned node_cpu_primary : 1; /* CPU is a primary              */
            unsigned node_in_console  : 1; /* CPU is in console mode        */
            unsigned node_rsrvd_7_31  : 25; /* Unused bits in first longword */
            unsigned node_rsrvd_32_63 : 32; /* Unused bits in second longword */
            } flag_bits;
        } flag_union;
    GCT_HANDLE config;                  /* Config binging                   */
    GCT_HANDLE affinity;                /* Affinity (performance) binding   */
    GCT_HANDLE parent;                  /* Parent node                      */
    GCT_HANDLE next_sib;                /* Next sibling node                */
    GCT_HANDLE prev_sib;                /* Previous sibling node            */
    GCT_HANDLE child;                   /* Child node                       */
    unsigned short int depth;           /* Reserved for SRM use             */
    unsigned short int rsrvd;           /* Reserved for SRM use             */
    unsigned int magic;                 /* Valid bits 'GLXY'                */
    } GCT_NODE;
 
#if !defined(__VAXC)
#define owner_both owner_union.owner_both
#define owner_each	owner_union.owner_each
#define owner owner_each.owner
#define current_owner owner_each.current_owner
#define node_flags flag_union.node_flags
#define node_hardware flag_union.flag_bits.node_hardware
#define node_hotswap flag_union.flag_bits.node_hotswap
#define node_unavailable flag_union.flag_bits.node_unavailable
#define node_hw_template flag_union.flag_bits.node_hw_template
#define node_initialized flag_union.flag_bits.node_initialized
#define node_cpu_primary flag_union.flag_bits.node_cpu_primary
#define node_in_console flag_union.flag_bits.node_in_console
#define node_rsrvd_7_31 flag_union.flag_bits.node_rsrvd_7_31
#define node_rsrvd_32_63 flag_union.flag_bits.node_rsrvd_32_63
#endif		/* #if !defined(__VAXC) */
 
	
typedef struct _gct_root_node {
    GCT_NODE hd_root;
    unsigned __int64 lock;              /* Software lock                    */
    unsigned __int64 transient_level;   /* Update counter (in prog)         */
    unsigned __int64 current_level;     /* Update counter (actual)          */
    unsigned __int64 console_req;       /* Memory required for console      */
    unsigned __int64 min_alloc;         /* Minimum memory allocation        */
    unsigned __int64 min_align;         /* Memory allocation alignment      */
    unsigned __int64 base_alloc;        /* Base memory min allocation       */
    unsigned __int64 base_align;        /* Base memory alloc alignment      */
    unsigned __int64 max_phys_address;  /* Largest Physical Address         */
    unsigned __int64 mem_size;          /* Total current memory size        */
    unsigned __int64 platform_type;     /* Type code of platform            */
    int platform_name;                  /* Offset to name string            */
    GCT_HANDLE primary_instance;        /* Handle of GALAXY Primary Partiion */
    GCT_HANDLE first_free;              /* First free byte of tree pool     */
    GCT_HANDLE high_limit;              /* High address limit for nodes     */
    GCT_HANDLE lookaside;               /* Lookaside list for node deletion */
    int available;                      /* Amount of bytes in pool          */
    unsigned int max_partition;         /* Max partitions                   */
    int partitions;                     /* Offset to partition ID map       */
    int communities;                    /* Offset to community ID map       */
    unsigned int max_platform_partition; /* Max part platform supports      */
    unsigned int max_fragments;         /* Max memory frags per desc        */
    unsigned int max_desc;              /* Max memory descriptors           */
    char galaxy_id [16];                /* Galaxy ID                        */
    char galaxy_id_pad [4];             /* Pad ID with a longword of bytes (ensures a null terminator) */
    int bindings;                       /* Offset to array of bindings      */
    } GCT_ROOT_NODE;
	
typedef struct _gct_sw_root_node {
    GCT_NODE 	     hd_sw_root;
    } GCT_SW_ROOT_NODE;
	
typedef struct _gct_hw_root_node {
    GCT_NODE 	     hd_hw_root;
#if FRU5
    GCT_HD_EXT 	     ext_hroot;
#endif
    } GCT_HW_ROOT_NODE;
	
typedef struct _gct_template_root_node {
    GCT_NODE 	     hd_template_root;
    } GCT_TEMPLATE_ROOT_NODE;
	
typedef struct _gct_sbb_node {
    GCT_NODE 	     hd_sbb;
    } GCT_SBB_NODE;
	
typedef struct _gct_iop_node {
    GCT_NODE                hd_iop;
    unsigned __int64        min_io_pa;         /* lowest possible PA in I/O subsystem */
    unsigned __int64        max_io_pa;         /* highest possible PA in I/O subsystem */
#if ( FRU5 && (!WILDFIRE) )
    SUBPKT_BUSBRIDGE_FRU5   iop_busbridge;
#ifdef PLATFORM_IO_EXTENSION
                            PLATFORM_IO_EXTENSION
#endif
    GCT_HD_EXT              ext_iop;
#endif
    } GCT_IOP_NODE;
	
typedef struct _gct_hose_node {
    GCT_NODE 	     hd_hose;
    } GCT_HOSE_NODE;
	
typedef struct _gct_bus_node {
    GCT_NODE 	     hd_bus;
#if ( FRU5 && (!WILDFIRE) )
    SUBPKT_BUS_FRU5  bus_sub;
    GCT_HD_EXT       ext_bus;
#endif
    } GCT_BUS_NODE;
	
typedef struct _gct_slot_node {
    GCT_NODE 	     hd_slot;
#if ( FRU5 && (!WILDFIRE) )
    GCT_HD_EXT       ext_slot;
#endif
    } GCT_SLOT_NODE;
	

typedef struct _gct_io_ctrl_node {
    GCT_NODE 	            hd_io_ctrl;
#if ( FRU5 && (!WILDFIRE) )
    SUBPKT_PCI_FRU5         pci_sub;
    SUBPKT_PCI_VPD_FRU5     pci_vpd;
    GCT_HD_EXT              ext_io_ctrl;
#endif
    } GCT_IO_CTRL_NODE;


typedef struct _gct_ioctrl_isa_node {
    GCT_NODE 	            hd_ioctrl_isa;
#if ( FRU5 && (!WILDFIRE) )
    SUBPKT_BUSBRIDGE_FRU5   isa_busbridge;
#ifdef PLATFORM_ISA_EXTENSION
                            PLATFORM_ISA_EXTENSION
#endif
    GCT_HD_EXT              ext_ioctrl_isa;
#endif
    } GCT_IOCTRL_ISA_NODE;


	
typedef struct _gct_cpu_module_node {
    GCT_NODE  	     hd_cpu_module;
#if ( FRU5 && (!WILDFIRE) )
    GCT_HD_EXT       ext_cpu_module;
#endif
    } GCT_CPU_MODULE_NODE;

typedef struct _gct_cpu_node {
    GCT_NODE hd_cpu;
#if FRU5
    SUBPKT_CPU_FRU5	    cpu_res;
#endif
    } GCT_CPU_NODE;

#define INSTANCE_NAME_LENGTH 128

typedef struct _gct_partition_node {
    GCT_NODE hd_partition;
    unsigned __int64 hwrpb;             /* HWRPB PA                         */
    unsigned __int64 incarnation;       /* Partition incarnation            */
    unsigned __int64 priority;          /* Partition priority               */
    unsigned int os_type;               /* OS type                          */
    int part_reserved_1;                /* Pad longword available for use   */
    char instance_name [128];           /* Instance Name                    */
#if FRU5
    SUBPKT_CSL_FRU5 csl_res;
#endif
    } GCT_PARTITION_NODE;

#define COMM_BLOCK_SIZE 8
	
typedef struct _gct_community_node {
    GCT_NODE hd_community;
    unsigned __int64 gmdb [8];          /* Communication block              */
    } GCT_COMMUNITY_NODE;
	
typedef struct _gct_memory_sub_node {
    GCT_NODE hd_memory_sub;
    unsigned __int64 min_pa;            /* Lowest possible PA in subsystem  */
    unsigned __int64 max_pa;            /* Highest possible PA in subsys    */
#if ( FRU5 && (!WILDFIRE) )
    GCT_HD_EXT       ext_mem_sub;
#endif
    } GCT_MEMORY_SUB_NODE;
	
typedef struct _gct_hard_partition_node {
    GCT_NODE hd_hard_partition;
    unsigned __int64 incarnation;       /* Hard Partition incarnation       */
    unsigned __int64 priority;          /* Hard Partition priority          */
    unsigned __int64 space1;            /* temp1                            */
    unsigned __int64 space2;            /* temp2                            */
    unsigned __int64 space3;            /* temp3                            */
    unsigned __int64 gmdb [8];          /* Communication block              */
    char instance_name [128];           /* Hard Partition Instance Name     */
    } GCT_HARD_PARTITION_NODE;

typedef struct _gct_memory_ctrl_node {
    GCT_NODE                hd_memory_ctrl;
#if ( FRU5 && (!WILDFIRE) )
    SUBPKT_MEMORY_FRU5      mem_ctrl_sub;
    GCT_HD_EXT              ext_mem_ctrl;
#endif
    } GCT_MEMORY_CTRL_NODE;

#define MEM_CONSOLE 0x1
#define MEM_PRIVATE 0x2
#define MEM_SHARED 0x4
#define MEM_BASE 0x8
#define MEM_VALID 0x10
#define MEM_RSRVD_5_31 0xFFFFFFE0
	
typedef struct _gct_mem_desc {
    unsigned __int64 pa;                /* Base PA of memory fragment       */
    unsigned __int64 size;              /* Size of memory fragment          */
    GCT_HANDLE mem_owner;
    GCT_HANDLE mem_current_owner;
    __union  {
        unsigned int mem_flags;         /* flags                            */
        __struct  {
            unsigned mem_console : 1;   /* Console memory                   */
            unsigned mem_private : 1;   /* Private (non-shared) memory      */
            unsigned mem_shared : 1;    /* Shared memory                    */
            unsigned mem_base : 1;      /* Base Segment memory              */
            unsigned mem_valid : 1;     /* Fragment is valid                */
            unsigned mem_rsrvd_5_31 : 27; /* Unused bits in longword        */
            } flag_bits;
        } flag_union;
    unsigned int mem_reserved_1;        /* Pad to a quad.  Available for use */
    } GCT_MEM_DESC;
 
#if !defined(__VAXC)
#define mem_flags flag_union.mem_flags
#define mem_console flag_union.flag_bits.mem_console
#define mem_private flag_union.flag_bits.mem_private
#define mem_shared flag_union.flag_bits.mem_shared
#define mem_base flag_union.flag_bits.mem_base
#define mem_valid flag_union.flag_bits.mem_valid
#define mem_rsrvd_5_31 flag_union.flag_bits.mem_rsrvd_5_31
#endif		/* #if !defined(__VAXC) */
 
	
typedef struct _gct_mem_info {
    unsigned __int64 base_pa;           /* Base PA of desc                  */
    unsigned __int64 base_size;         /* Size of memory desc              */
    unsigned int desc_count;            /* Number of fragments              */
    unsigned int info_reserved;         /* Pad to a quad                    */
    } GCT_MEM_INFO;
	
typedef struct _gct_mem_desc_node {
    GCT_NODE hd_mem_desc;
    GCT_MEM_INFO mem_info;              /* Mem desc header                  */
    int mem_frag;                       /* Offset to descriptors            */
    int mem_desc_reserved;              /* Pad to a quad                    */
    unsigned __int64 bitmap_pa;         /* PA of memory bitmap or zero      */
#if ( FRU5 && (!WILDFIRE) )
    GCT_HD_EXT ext_mem_desc;
#endif
    } GCT_MEM_DESC_NODE;
	
typedef struct _gct_pseudo_node {
    GCT_NODE hd_pseudo;
#if ( FRU5 && (!WILDFIRE) )
    GCT_HD_EXT ext_pseudo;
#endif
    } GCT_PSEUDO_NODE;

typedef struct _gct_power_envir_node {
    GCT_NODE hd_power_envir;
    } GCT_POWER_ENVIR_NODE;

typedef struct _gct_fru_info {
    unsigned int     fru_diag_flag;         /* Diagnostic flags             */
    GCT_DIAG_FAILURE fru_diag_failure_info; /* Failure information          */
/*                                                                          */
/*  The rest of the structure is variable length "TLV" strings.  To access  */
/*  them -- you need to use the tag to understand the data type, the length */
/*  to get the number of bytes.  This stricture contains:                   */
/*                                                                          */
/*  manufacturer                                                            */
/*  model                                                                   */
/*  part_number                                                             */
/*  serial_number                                                           */
/*  firmware_revision                                                       */
/*                                                                          */
    GCT_TLV fru_info;
    GCT_TLV model;
    GCT_TLV part_num;
    GCT_TLV ssn;
    GCT_TLV firmware_rev;
    } GCT_FRU_INFO;
	
typedef struct _gct_fru_root_node {
    GCT_NODE hd_fru_root;
    GCT_FRU_INFO root_fru_info;
    } GCT_FRU_ROOT_NODE;
	
typedef struct _gct_fru_desc_node {
    GCT_NODE hd_fru_desc;
    GCT_FRU_INFO fru_desc_info;
    } GCT_FRU_DESC_NODE;

typedef struct _gct_smb_node {
    GCT_NODE             hd_smb;
#if ( FRU5 && (!WILDFIRE) )
    SUBPKT_SMB_FRU5      smb_res;
#ifdef PLATFORM_SMB_EXT
                         PLATFORM_SMB_EXT
#endif
    SUBPKT_RMC_FRU5      rmc_res;
    GCT_HD_EXT           ext_smb;
#endif
    } GCT_SMB_NODE;

typedef struct _gct_sbm_node {
    GCT_NODE         hd_sbm;
#if ( FRU5 && (!WILDFIRE) )
    GCT_HD_EXT    ext_sbm;
#endif
} GCT_SBM_NODE;

typedef struct _gct_cab_node {
    GCT_NODE hd_cab;
#if ( FRU5 && (!WILDFIRE) )
    GCT_HD_EXT ext_cab;
#endif
    } GCT_CAB_NODE;

typedef struct _gct_chassis_node {
    GCT_NODE hd_chassis;
#if ( FRU5 && (!WILDFIRE) )
    GCT_HD_EXT ext_chassis;
#endif
    } GCT_CHASSIS_NODE;
	
typedef struct _gct_exp_chassis_node {
    GCT_NODE hd_exp_chassis;
#if ( FRU5 && (!WILDFIRE) )
    GCT_HD_EXT ext_exp_chassis;
#endif
    } GCT_EXP_CHASSIS_NODE;

typedef struct _gct_sys_inter_switch_node {
    GCT_NODE hd_sys_inter_switch;
#if FRU5
    GCT_HD_EXT ext_sys_inter_switch;
#endif
    } GCT_SYS_INTER_SWITCH_NODE;


#define GCT__BUFFER_HEADER_SIZE 64
#define gct_head 0x40;
#define _GCT_MAKE_ADDRESS(_gct_o) ((GCT_NODE *)((char *) GCT_POINTER_TO_ROOT + _gct_o));
#define _GCT_MAKE_HANDLE(_gct_a) ((char *) _gct_a - (char *) GCT_POINTER_TO_ROOT);

#define ROOTSIZE ((sizeof(GCT_ROOT_NODE) + 63) & ~63) 
#define ID_ARRAY_SIZE (((platform_data->max_partitions * sizeof(GCT_HANDLE)) + 63) & ~63)
#define _GCT_MAKE_ADDRESS(_gct_o) ((GCT_NODE *)((char *) GCT_POINTER_TO_ROOT + _gct_o));
#define _GCT_MAKE_HANDLE(_gct_a) ((char *) _gct_a - (char *) GCT_POINTER_TO_ROOT);

#define GCT_POINTER_TO_ROOT gct_root
#define ROOTSIZE ((sizeof(GCT_ROOT_NODE) + 63) & ~63)
#define _GCT_VALIDATE_HANDLE( ___n) {\
  GCT_ROOT_NODE *____pR;\
  GCT_NODE *____pN;\
  ____pR = (GCT_ROOT_NODE *) _GCT_MAKE_ADDRESS(0);\
  if ((___n >= ____pR->high_limit) || (___n < 0)) return (BADHANDLE);\
  ____pN = _GCT_MAKE_ADDRESS( ___n);\
  if (____pN->magic != NODE_VALID) return (BADHANDLE);\
}
#define GCT__INTERNAL_ID 99999
#define GCT__RETRY_MAX   -1
/*
 *  Define a MACRO to do the lock inside the code.  It does a status check and return if
 *  the lock fails.
 */
#define _GCT__LOCK_TREE( ___lock, ___id, ___caller, ___retry) {\
int ____stat;\
____stat = gct__lock_tree( ___lock, ___id, ___caller, ___retry);\
if (____stat != SUCCESS) return (____stat);\
}

/* Round the actual length to a quad... that is, the TLV length starts
   on a longword, not on a quadword, so make sure that the length is padded
   to the nearest quad boundry.
  
   So.  Add 4 (the tag and length field size) to the total length, round to
   the next quadword, and subtract 4 (the tag and length size) and the data
   length should be rounded propoerly.
*/

#define _ROUND_TLV_LENGTH(__VALUE_LENGTH) (((__VALUE_LENGTH + 4 + 7) & ~7) - 4)

/* Move to the next TLV by adding the value length (rounded) + 4 for the tag/length header */

#define _NEXT_TLV(__TLVP) __TLVP = (GCT_TLV *) ((char *) __TLVP + _ROUND_TLV_LENGTH(__TLVP->tlv_length) + 4)

#define _ACCESS_TLV(__BASE, __OFFSET, __RESULT) {\
  int x = __OFFSET;\
  __RESULT = __BASE;\
  while (__RESULT->tlv_tag && x) {\
    x -= 1;\
    _NEXT_TLV(__RESULT);\
  }\
}

#define _upd_subpkt(__SUBPACK, __LENGTH, __CLASS, __TYPE, __REV) \
	((GCT_SUBPACK *) (__SUBPACK))->subpack_length = __LENGTH; \
	((GCT_SUBPACK *) (__SUBPACK))->subpack_class  = __CLASS; \
	((GCT_SUBPACK *) (__SUBPACK))->subpack_type   = __TYPE; \
	((GCT_SUBPACK *) (__SUBPACK))->subpack_rev    = __REV;

#define _mk_hd_ext(__HDEXT, __FRUCNT, __FRUHANDLE, __SUBCNT, __SUBOFF) \
	((GCT_HD_EXT *) (__HDEXT))->fru_count     = __FRUCNT; \
	((GCT_HD_EXT *) (__HDEXT))->fru           = __FRUHANDLE; \
	((GCT_HD_EXT *) (__HDEXT))->subpkt_count  = __SUBCNT; \
	((GCT_HD_EXT *) (__HDEXT))->subpkt_offset = __SUBOFF;

	
/*
 *  Define the GCT_FILE type as char.  Used by GCU/GCON I/O routines.
 */
#define GCT_FILE char
/*
 *  Define some typedefs
 */
#ifdef	__INITIAL_POINTER_SIZE
#pragma __required_pointer_size __save				/* Save current pointer size   		*/
#pragma __required_pointer_size __long				/* Pointers are 64-bit	   		*/	
typedef struct _gct_buffer_header *GCT_BUFFER_HEADER_PQ;	/* Long pointer to a GCT buffer header	*/
typedef struct _gct_node 	*GCT_NODE_PQ;			/* Long pointer to a GCT node 		*/
typedef struct _gct_root_node 	*GCT_ROOT_NODE_PQ;		/* Long pointer to a root node 		*/
typedef struct _gct_sw_root_node *GCT_SW_ROOT_NODE_PQ;		/* Long pointer to a SW root node 	*/
typedef struct _gct_hw_root_node *GCT_HW_ROOT_NODE_PQ;		/* Long pointer to a HW root node 	*/
typedef struct _gct_template_root_node *GCT_TEMPLATE_ROOT_NODE_PQ; /* Long pointer to a TEPLATE root node */
typedef struct _gct_sbb_node *GCT_SBB_NODE_PQ;			/* Long pointer to a SBB node 		*/
typedef struct _gct_iop_node *GCT_IOP_NODE_PQ;			/* Long pointer to a IOP node 		*/
typedef struct _gct_io_ctrl_node *GCT_IO_CTRL_NODE_PQ;		/* Long pointer to a IO_CTRL node 	*/
typedef struct _gct_slot_node *GCT_SLOT_NODE_PQ;		/* Long pointer to a SLOT node 		*/
typedef struct _gct_bus_node *GCT_BUS_NODE_PQ;			/* Long pointer to a BUS node 		*/
typedef struct _gct_hose_node *GCT_HOSE_NODE_PQ;		/* Long pointer to a HOSE node 		*/
typedef struct _gct_cpu_node 	*GCT_CPU_NODE_PQ;		/* Long pointer to a CPU node 		*/
typedef struct _gct_cpu_module_node *GCT_CPU_MODULE_NODE_PQ;	/* Long pointer to a CPU_MODULE node 	*/
typedef struct _gct_partition_node *GCT_PARTITION_NODE_PQ; 	/* Long pointer to a partition node 	*/
typedef struct _gct_community_node *GCT_COMMUNITY_NODE_PQ; 	/* Long pointer to a community node 	*/
typedef struct _gct_memory_sub_node *GCT_MEMORY_SUB_NODE_PQ;	/* Long pointer to a mememory subsystem node */
typedef struct _gct_memory_ctrl_node *GCT_MEMORY_CTRL_NODE_PQ;	/* Long pointer to a mememory ctrl node */
typedef struct _gct_mem_desc 	*GCT_MEM_DESC_PQ;		/* Long pointer to a mem descriptor 	*/
typedef struct _gct_mem_info 	*GCT_MEM_INFO_PQ;		/* Long pointer to a mem info structure */
typedef struct _gct_mem_desc_node *GCT_MEM_DESC_NODE_PQ; 	/* Long pointer to a memory descriptor node */
typedef struct _gct_names	*GCT_NAMES_PQ;			/* Long pointer to a names structure 	*/
typedef struct _gct_power_envir_node 	*GCT_POWER_ENVIR_NODE_PQ; /* Long pointer to a Power & Envir node */
typedef struct _gct_fru_root_node 	*GCT_FRU_ROOT_NODE_PQ;	/* Long pointer to a FRU Root node */
typedef struct _gct_fru_desc_node 	*GCT_FRU_DESC_NODE_PQ;	/* Long pointer to a Fru Descripter node */
typedef struct _gct_smb_node 	*GCT_SMB_NODE_PQ;		/* Long pointer to a system mother bd node */
typedef struct _gct_cab_node 	*GCT_CAB_NODE_PQ;		/* Long pointer to a cab node 		*/
typedef struct _gct_chassis_node 	*GCT_CHASSIS_NODE_PQ;	/* Long pointer to a Chassis node	*/
typedef struct _gct_exp_chassis_node 	*GCT_EXP_CHASSIS_NODE_PQ;    /* Long pointer to a expanded chass node */
typedef struct _gct_sys_inter_switch_node *GCT_SYS_INTER_SWITCH_NODE_PQ; /* Long pointer to a Inter Conn Switch */
typedef struct _gct_hard_partition_node *GCT_HARD_PARTITION_NODE_PQ; /* Long pointer to a Hard Partition */
#pragma __required_pointer_size __restore			/* Return to previous pointer size 	*/
#else
typedef unsigned __int64 GCT_BUFFER_HEADER_PQ;
typedef unsigned __int64 GCT_NODE_PQ;
typedef unsigned __int64 GCT_ROOT_NODE_PQ;	
typedef unsigned __int64 GCT_SW_ROOT_NODE_PQ;	
typedef unsigned __int64 GCT_HW_ROOT_NODE_PQ;	
typedef unsigned __int64 GCT_TEMPLATE_ROOT_NODE_PQ;	
typedef unsigned __int64 GCT_CPU_NODE_PQ;	
typedef unsigned __int64 GCT_CPU_MODULE_NODE_PQ;	
typedef unsigned __int64 GCT_SBB_NODE_PQ;	
typedef unsigned __int64 GCT_IOP_NODE_PQ;	
typedef unsigned __int64 GCT_IO_CTRL_NODE_PQ;	
typedef unsigned __int64 GCT_SLOT_NODE_PQ;	
typedef unsigned __int64 GCT_BUS_NODE_PQ;	
typedef unsigned __int64 GCT_HOSE_NODE_PQ;	
typedef unsigned __int64 GCT_PARTITION_NODE_PQ; 
typedef unsigned __int64 GCT_COMMUNITY_NODE_PQ; 
typedef unsigned __int64 GCT_MEMORY_SUB_NODE_PQ;	
typedef unsigned __int64 GCT_MEMORY_CRTL_NODE_PQ;	
typedef unsigned __int64 GCT_MEM_DESC_PQ;	
typedef unsigned __int64 GCT_MEM_INFO_PQ;	
typedef unsigned __int64 GCT_MEM_DESC_NODE_PQ; 
typedef unsigned __int64 GCT_NAMES_PQ;		
typedef unsigned __int64 GCT_POWER_ENVIR_NODE_PQ;
typedef unsigned __int64 GCT_FRU_ROOT_NODE_PQ;
typedef unsigned __int64 GCT_FRU_DESC_NODE_PQ;
typedef unsigned __int64 GCT_SMB_NODE_PQ;
typedef unsigned __int64 GCT_CAB_NODE_PQ;
typedef unsigned __int64 GCT_CHASSIS_NODE_PQ;
typedef unsigned __int64 GCT_EXP_CHASSIS_NODE_PQ;
typedef unsigned __int64 GCT_SYS_INTER_SWITCH_NODE_PQ;
typedef unsigned __int64 GCT_HARD_PARTITION_NODE_PQ;
#endif /* __INITIAL_POINTER_SIZE */
 
#pragma __member_alignment __restore
#ifdef __INITIAL_POINTER_SIZE			 /* Defined whenever ptr size pragmas supported */
#pragma __required_pointer_size __restore		 /* Restore the previously-defined required ptr size */
#endif
#ifdef __cplusplus
    }
#endif
#pragma __standard
 
#endif /* __GCTDEF_LOADED */
 
