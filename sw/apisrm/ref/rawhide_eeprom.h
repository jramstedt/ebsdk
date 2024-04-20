/*
 * rawhide_eeprom.h
 */

/*
Facility:

	Rawhide Console Firmware

Abstract:

	This module declares the EEPROM definitions and constants.

Environment:

	Console Mode, Privileged

Author:

	Firmware Project Team

Creation Date:

	06-May-1998

Revisions:
	pas	06-may-1998	Adapted from turbo_eeprom.hm initial code for
				NT error logging.  Note: turbo code left in that
				covers other O.S.s (OVMS, DUNIX) - in case
				in the future they also request this logging

*/
 
#ifndef eeprom_loaded
#define eeprom_loaded
#endif

/*
 * EEPROM Area Definitions
 * Define as 1 to include area in EEPROM
 */

#define EEPROM_PRODUCT		0
#define EEPROM_CONSOLE		0
#define EEPROM_BOOT_SPEC	0
#define EEPROM_BOOT_PRIM	0
#define EEPROM_ENVIRON		0
#define EEPROM_CONS_PATCH	0
#define EEPROM_DIAG_PATCH	0
#define EEPROM_BOOT_SCRIPT	0
#define EEPROM_PWRUP_SCRIPT	0
#define EEPROM_TEST_SCRIPT	0
#define EEPROM_BOOT_ARGS	0
#define EEPROM_RMT_SUPPORT	0
#define EEPROM_FRU_DESCRIP	0
#define EEPROM_SERIAL		0
#define EEPROM_CONFIG		0
#define EEPROM_DIAG_LOG		0
#define EEPROM_DIAG_TDD		0
#define EEPROM_DIAG_SDD		0
#define EEPROM_SYM_LOG		1
#define EEPROM_HALT_COUNT	0
#define EEPROM_SERVICE_CALL	0
#define EEPROM_MOD_HIST		0
#define EEPROM_HALT_DATA	1
#define EEPROM_OS_LOG		1

/* EEPROM Hardware definitions */

#define RAWHIDE_EEPROM_ADDRESS	0x1F0000	/* Last sector of flash */

/* EEPROM ClassType Code Assignments */
                                                           
#define eeprom_k_version	8		/* Version 8 */
#define eeprom_k_offset_table	((eeprom_k_version << 16) | 0)	
#define eeprom_k_cons_params	1
#define eeprom_k_boot_specs	2
#define eeprom_k_boot_prims	3
#define eeprom_k_environ	4
#define eeprom_k_product	5

#define eeprom_k_cons_patch	16
#define eeprom_k_diag_patch	17
#define eeprom_k_boot_script	18
#define eeprom_k_pwrup_script	((1 << 16) | 18)
#define eeprom_k_test_script	((2 << 16) | 18)
#define eeprom_k_rmt_support	19
#define eeprom_k_boot_args	20

#define eeprom_k_fru_descrip	32
#define eeprom_k_sys_serial	33
#define eeprom_k_sys_config	34
#define eeprom_k_se_config	34
#define eeprom_k_se_config_ver	((1 << 16) | 18)

#define eeprom_k_diag_log	48
#define eeprom_k_diag_tdd	((1 << 16) | 48)
#define eeprom_k_diag_sdd	((2 << 16) | 48)
#define eeprom_k_sym_log	49
#define eeprom_k_halt_code	50
#define eeprom_k_se_memory	51
#define eeprom_k_se_log		52
#define eeprom_k_service_call	53
#define eeprom_k_mod_hist	54
#define eeprom_k_se_perf	55
#define eeprom_k_halt_data	56
#define eeprom_k_os_log		57

/* EEPROM Area Type Messages */

#define eeprom_msg_offset_table	"offset table"
#define eeprom_msg_cons_params	"console parameters"
#define eeprom_msg_boot_specs	"boot specs"
#define eeprom_msg_boot_prims	"boot primitives"
#define eeprom_msg_environ	"environment"
#define eeprom_msg_product	"product"

#define eeprom_msg_cons_patch	"console patch"
#define eeprom_msg_diag_patch	"diagnostic patch"
#define eeprom_msg_boot_script	"boot script"
#define eeprom_msg_pwrup_script	"powerup script"
#define eeprom_msg_test_script	"test script"
#define eeprom_msg_rmt_support	"remote support"
#define eeprom_msg_boot_args	"boot args"

#define eeprom_msg_fru_descrip	"fru descriptor"
#define eeprom_msg_sys_serial	"system serial number"
#define eeprom_msg_sys_config	"system configuration"
#define eeprom_msg_se_config	"storage configuration"

#define eeprom_msg_diag_log	"diagnostic log"
#define eeprom_msg_diag_tdd	"diagnostic tdd"
#define eeprom_msg_diag_sdd	"diagnostic sdd"
#define eeprom_msg_sym_log	"OS error log"
#define eeprom_msg_halt_code	"halt code counters"
#define eeprom_msg_se_memory	"storage element memory"
#define eeprom_msg_se_log	"storage element log"
#define eeprom_msg_service_call	"service call"
#define eeprom_msg_mod_hist	"module history"
#define eeprom_msg_se_perf	"storage element performance"
#define eeprom_msg_halt_data	"fatal error halt data"
#define eeprom_msg_os_log	"OS log"

/* EEPROM parameters */

#define eeprom_k_nbr_diag_logs	5
#define eeprom_k_nbr_diag_tdds	5
#define eeprom_k_nbr_diag_sdds	3
#define eeprom_k_nbr_sym_logs	2
#define eeprom_k_nbr_630_logs	16
#define eeprom_k_nbr_halt_data_logs	2
#define eeprom_k_nbr_boot_specs	4

#define eeprom_k_bootfile_size	40
#define eeprom_k_bootargs_size	64
#define eeprom_k_bootspec_size	(eeprom_k_bootfile_size + eeprom_k_bootargs_size)

/* EEPROM Part Sizes, in bytes */
#define eeprom_size			(64*1024)
#define eeprom_size_diag_log		(14*4)
#define eeprom_size_diag_tdd		(13*4)
#define eeprom_size_diag_sdd		(139*4)
#define eeprom_size_sym_header		100
#define eeprom_size_670_log		800
#define eeprom_size_660_log		2400
#define eeprom_size_630_log		100
#define eeprom_size_620_log		1500
#define eeprom_size_sym_log		(eeprom_size_670_log + eeprom_size_660_log + eeprom_size_620_log)
#define eeprom_size_boot_spec		((1*4) + eeprom_k_bootspec_size)
#define eeprom_size_diag_patch		64
#define eeprom_size_cons_patch		64
#define eeprom_size_boot_prims		64
#define eeprom_size_boot_script		1024
#define eeprom_size_pwrup_script	1024
#define eeprom_size_test_script		2048
#define eeprom_size_rmt_support		2048
#define eeprom_size_boot_args		512 
#define eeprom_k_nbr_evs		128
#define eeprom_size_evdata		4*1024
#define eeprom_size_environ		((eeprom_k_nbr_evs*4)+             \
                                         (eeprom_size_evdata)+(1*4)+(1*4))
#define eeprom_size_halt_code		64
#define eeprom_size_product		64
#define eeprom_size_sys_config		(((9*(1+3))+(4*(1+3+16)))*4)
#define eeprom_size_service_call	64

#define eeprom_k_nbr_tlsbreg		48      
#define eeprom_s_tlsbreg		(eeprom_k_nbr_tlsbreg * 4)
#define eeprom_s_watchreg		8
#define eeprom_s_swerrflags		8

#define eeprom_k_nbr_nodereg		14      
#define eeprom_s_nodereg		(eeprom_k_nbr_nodereg * 4)
#define eeprom_k_nbr_pcireg		3      
#define eeprom_s_pcireg			(eeprom_k_nbr_pcireg * 4)

#define eeprom_size_halt_data		((2*4) + eeprom_s_watchreg + eeprom_s_swerrflags + pal$logout_specific_size  + (mchk$iod_size * 4))


#define eeprom_size_os_log		(4 * 1024)

/* EEPROM Area Sizes, in bytes */

#define eeprom_s_patterns	(4 * 4)
#define eeprom_s_offset_table	((3*4) + (17 * 4) + (1 * 4))
#define eeprom_s_fru_descrip	((3*4) + (8 * 4))
#define eeprom_s_cons_params	((3*4) + (4 * 4))
#define eeprom_s_boot_specs	((3*4) + (eeprom_size_boot_spec * eeprom_k_nbr_boot_specs))
#define eeprom_s_diag_patch	((3*4) + eeprom_size_diag_patch)
#define eeprom_s_cons_patch	((3*4) + eeprom_size_cons_patch)
#define eeprom_s_diag_log	((3*4) + (1*4) + (eeprom_k_nbr_diag_logs * eeprom_size_diag_log))
#define eeprom_s_diag_tdd	((3*4) + (1*4) + (eeprom_k_nbr_diag_tdds * eeprom_size_diag_tdd))
#define eeprom_s_diag_sdd	((3*4) + (1*4) + (eeprom_k_nbr_diag_sdds * eeprom_size_diag_sdd))
#define eeprom_s_boot_prims	((3*4) + eeprom_size_boot_prims)
#define eeprom_s_sys_serial	((3*4) + 10 + 2)
#define eeprom_s_sym_log	((3*4) + (2 * (eeprom_size_sym_header + \
					(eeprom_k_nbr_sym_logs * eeprom_size_sym_log) + \
					(eeprom_k_nbr_630_logs * eeprom_size_630_log))))
#define eeprom_s_mod_hist	((3*4) + (1*4)+8+68)
#define eeprom_s_boot_script	((3*4) + eeprom_size_boot_script)
#define eeprom_s_pwrup_script	((3*4) + eeprom_size_pwrup_script)
#define eeprom_s_test_script	((3*4) + eeprom_size_test_script)
#define eeprom_s_rmt_support	((3*4) + eeprom_size_rmt_support)
#define eeprom_s_boot_args	((3*4) + eeprom_size_boot_args)
#define eeprom_s_product	((3*4) + eeprom_size_product)
#define eeprom_s_environ	((3*4) + eeprom_size_environ)
#define eeprom_s_halt_code	((3*4) + eeprom_size_halt_code)
#define eeprom_s_product	((3*4) + eeprom_size_product)
#define eeprom_s_sys_config	((3*4) + eeprom_size_sys_config)
#define eeprom_s_service_call	((3*4) + eeprom_size_service_call)
#define eeprom_s_halt_data_cpu	((2*4) + (eeprom_k_nbr_halt_data_logs * eeprom_size_halt_data))
#define eeprom_s_halt_data	((3*4) + (1*4) + (4 * eeprom_s_halt_data_cpu))
#define eeprom_s_os_log		((3*4) + eeprom_size_os_log)

/* Definition of EEPROM parameters */

#define conf_k_nnex		16

#define eeprom_m_disable_log	0x80000000
#define eeprom_m_otf		0x40000000

#define eeprom_shift_disable_baudadj	0	/* FLAGS bit shifts */
#define eeprom_shift_disable_confdif	1
#define eeprom_shift_conf_valid		2
#define eeprom_shift_bypass_selftest	16
#define eeprom_shift_bypass_cpumem	17
#define eeprom_shift_bypass_io		18

#define eeprom_k_cons_flags ((0<<eeprom_shift_disable_baudadj) |	\
			     (0<<eeprom_shift_disable_confdif) |	\
			     (0<<eeprom_shift_conf_valid)      |	\
			     (0<<eeprom_shift_bypass_selftest) |	\
			     (0<<eeprom_shift_bypass_cpumem)   |	\
			     (0<<eeprom_shift_bypass_io) )

#define eeprom_k_cons_flags1	0	/* reserved */

#define eeprom_k_selftest_tmo	60	/* 60 seconds */

#ifndef u_int
#define u_int unsigned int
#endif
#ifndef u_char
#define u_char unsigned char
#endif

/* EEPROM Array */

struct EEPROM_S_DEF {
  u_int PATTERNS[eeprom_s_patterns/4];   		/* Test patterns */

  union  {
    int OFFSET_TABLE[eeprom_s_offset_table/4];    	/* Offset table */
    struct  {
	int	CHECKSUM;
	int	CLASSTYPE;		/* SUBCLASSVERSION,,CLASSTYPE */

	int	LENGTH;

	int	OFFSETS[16];

	int	TERM;
    } offset;
  } offset_table;

  u_char fill[(eeprom_size) - (eeprom_s_patterns*2) - eeprom_s_offset_table];

  u_int TOP_PATTERNS[eeprom_s_patterns/4];   		/* Test patterns */
};

/* AREA_HEADER defines the three longword EEPROM area header */

struct AREA_HEADER {
    u_int	ah_l_checksum;
    int		ah_l_classtype;		/* SUBCLASSVERSION,,CLASSTYPE */
    int		ah_l_length;
} ;

#if EEPROM_PRODUCT

/* EEPROM Product Specific Array */

struct EEPROM_S_PRODUCT_DEF {
  union  {
    int PRODUCT[eeprom_s_product/4];    /* Product Area */
    struct  {
	int	CHECKSUM;
	int	CLASSTYPE;
	int	LENGTH;

	u_char	PRODUCT[eeprom_size_product];
    } product;
  } _product;
};
#endif

#if EEPROM_CONSOLE

/* EEPROM Console Parameters Array */

struct EEPROM_S_CONS_PARAMS_DEF {
  union  {
    int CONS_PARAMS[eeprom_s_cons_params/4];    /* Console Parameter Area */
    struct  {
	int	CHECKSUM;
	int	CLASSTYPE;
	int	LENGTH;

	union {
	u_int	FLAGS;
	    struct {
		unsigned DISABLE_BAUDADJ : 1;
		unsigned DISABLE_CONFDIF : 1;
		unsigned CONF_VALID : 1;
		unsigned rsvd1 : 13;
		unsigned BYPASS_SELFTEST : 1;
		unsigned BYPASS_CPUMEM : 1;
		unsigned BYPASS_IO : 1;
		unsigned rsvd2 : 13;
	    } flag_bits;
	} flags;
	u_int	FLAGS1;
	u_int	VERSION;
	u_char	SELFTEST_TMO;
	u_char	rsvd2[3];
    } params;
  } cons_params;
};
#endif

#if EEPROM_BOOT_SPEC

/* EEPROM Boot Specs Array */

struct EEPROM_S_BOOT_SPECS_DEF {
  union  {
    int BOOT_SPECS[eeprom_s_boot_specs/4];    /* Boot Specification Area */
    struct  {
	int	CHECKSUM;
	int	CLASSTYPE;
	int	LENGTH;

	u_int	SPEC0[eeprom_size_boot_spec];
	u_int	SPEC1[eeprom_size_boot_spec];
	u_int	SPEC2[eeprom_size_boot_spec];
	u_int	SPEC3[eeprom_size_boot_spec];
    } specs;
  } boot_specs;
};

struct EEPROM_S_BOOT_SPEC {
	u_int	bootflags;
	char	bootfile[40];
	char	bootargs[64];
};
#endif

#if EEPROM_BOOT_PRIM

/* EEPROM Boot Primitives Array */

struct EEPROM_S_BOOT_PRIMS_DEF {
  union  {
    int BOOT_PRIMS[eeprom_s_boot_prims/4];    /* Boot Primitive Area */
    struct  {
	int	CHECKSUM;
	int	CLASSTYPE;
	int	LENGTH;

	u_char	PRIMS[eeprom_size_boot_prims];
    } prims;
  } boot_prims;
};

/*
 * PRIM_HEADER defines the header for a boot primitive
 */
struct PRIM_HEADER {
    u_int	ph_l_checksum;
    u_short	ph_w_brb;
    u_short	ph_w_dvrbytes;
    u_short	ph_w_dvrver;
    u_short	ph_w_devspec;
} ;
#endif

#if EEPROM_ENVIRON

/* EEPROM Environment Variable Array */

struct EEPROM_S_ENVIRON_DEF {
  union  {
    int ENVIRONMENT[eeprom_s_environ/4];    /* Environment Area */
    struct  {
	int	CHECKSUM;
	int	CLASSTYPE;
	int	LENGTH;

	u_char	ENVIRON[eeprom_size_environ];
    } environ;
  } environment;
};
#endif

#if EEPROM_CONS_PATCH

/* EEPROM Console Patch Array */

struct EEPROM_S_CONS_PATCH_DEF {
  union  {
    int CONS_PATCH[eeprom_s_cons_patch/4];    /* Console Patch Area */
    struct  {
	int	CHECKSUM;
	int	CLASSTYPE;
	int	LENGTH;

	u_char	PATCH[eeprom_size_cons_patch];
    } patch;
  } cons_patch;
};
#endif

#if EEPROM_DIAG_PATCH

/* EEPROM Diagnostic Patch Array */

struct EEPROM_S_DIAG_PATCH_DEF {
  union  {
    int DIAG_PATCH[eeprom_s_diag_patch/4];    /* Diagnostic Patch Area */
    struct  {
	int	CHECKSUM;
	int	CLASSTYPE;
	int	LENGTH;

	u_char	PATCH[eeprom_size_diag_patch];
    } patch;
  } diag_patch;
};
#endif

#if EEPROM_BOOT_SCRIPT

/* EEPROM Boot Script Array */

struct EEPROM_S_BOOT_SCRIPT_DEF {
  union  {
    int BOOT_SCRIPT[eeprom_s_boot_script/4];    /* Boot Script Area */
    struct  {
	int	CHECKSUM;
	int	CLASSTYPE;
	int	LENGTH;

	char	SCRIPT[eeprom_size_boot_script];
    } script;
  } boot_script;
};
#endif


#if EEPROM_PWRUP_SCRIPT

/* EEPROM Power-up Script Array */

struct EEPROM_S_PWRUP_SCRIPT_DEF {
  union  {
    int PWRUP_SCRIPT[eeprom_s_pwrup_script/4];    /* Powerup Script Area */
    struct  {
	int	CHECKSUM;
	int	CLASSTYPE;
	int	LENGTH;

	char	SCRIPT[eeprom_size_pwrup_script];
    } script;
  } pwrup_script;
};
#endif

#if EEPROM_TEST_SCRIPT

/* EEPROM Test Script Array */

struct EEPROM_S_TEST_SCRIPT_DEF {
  union  {
    int TEST_SCRIPT[eeprom_s_test_script/4];    /* Test Script Area */
    struct  {
	int	CHECKSUM;
	int	CLASSTYPE;
	int	LENGTH;

	char	SCRIPT[eeprom_size_test_script];
    } script;
  } test_script;
};
#endif

#if EEPROM_RMT_SUPPORT

/* EEPROM Remote Support Array */

struct EEPROM_S_RMT_SUPPORT_DEF {
  union  {
    int RMT_SUPPORT[eeprom_s_rmt_support/4];    /* Remote Support Area */
    struct  {
	int	CHECKSUM;
	int	CLASSTYPE;
	int	LENGTH;

	u_char	SUPPORT[eeprom_size_rmt_support];
    } support;
  } rmt_support;
};
#endif

#if EEPROM_BOOT_SCRIPT

/* EEPROM Boot Script Array */

struct EEPROM_S_BOOT_ARGS_DEF {
  union  {
    int BOOT_ARGS[eeprom_s_boot_args/4];    /* Boot Args Area */
    struct  {
	int	CHECKSUM;
	int	CLASSTYPE;
	int	LENGTH;

	char	ARGS[eeprom_size_boot_args];
    } args;
  } boot_args;
};
#endif


#if EEPROM_FRU_DESCRIP

/* EEPROM Module Description Array */

struct EEPROM_S_FRU_DESCRIP_DEF {
  union  {
    int FRU_DESCRIP[eeprom_s_fru_descrip/4];    	/* Fru Descriptor */
    struct  {
	int	CHECKSUM;
	int	CLASSTYPE;
	int	LENGTH;

	char	FRU_SERIAL[10];
	char	rsvd1[2];
	char	FRU_PART[8];
	char	FRU_VARIANT[4];
	char	FRU_HW_REVISION[4];
	char	FRU_SW_REVISION[4];
    } fru;
  } fru_area;
};
#endif

#if EEPROM_SERIAL

/* EEPROM System Serial Number Array */

struct EEPROM_S_SYS_SERIAL_DEF {
  union  {
    int SYS_SERIAL[eeprom_s_sys_serial/4];    /* System Serial Area */
    struct  {
	int	CHECKSUM;
	int	CLASSTYPE;
	int	LENGTH;

	char	SERIAL[10];
	char	rsvd2[2];
    } serial;
  } sys_serial;
};
#endif

#if EEPROM_CONFIG

/* EEPROM System Configuration Array */

struct EEPROM_S_SYS_CONFIG_DEF {
  union  {
    int SYS_CONFIG[eeprom_s_sys_config/4];    /* System Configuration Area */
    struct  {
	int	CHECKSUM;
	int	CLASSTYPE;
	int	LENGTH;

	int	LSBNODE_0[4];		/* LDEV + Serial number(12) */
	int	LSBNODE_1[4];
	int	LSBNODE_2[4];
	int	LSBNODE_3[4];
	int	LSBNODE_4[4];
	int	LSBNODE_5[4];
	int	LSBNODE_6[4];
	int	LSBNODE_7[4];
	int	LSBNODE_8[4];

	int	BUS_TYPE_0;
	int	BUS_SERIAL_0[3];
	int	NODEID_0[16];

	int	BUS_TYPE_1;
	int	BUS_SERIAL_1[3];
	int	NODEID_1[16];

	int	BUS_TYPE_2;
	int	BUS_SERIAL_2[3];
	int	NODEID_2[16];

	int	BUS_TYPE_3;
	int	BUS_SERIAL_3[3];
	int	NODEID_3[16];
    } config;
  } sys_config;
};
#endif

#if EEPROM_DIAG_LOG

/* EEPROM Diagnostic Log Array */

struct EEPROM_S_DIAG_LOG_DEF {
  union  {
    int DIAG_LOG[eeprom_s_diag_log/4];    	/* Diagnostic Log Area */
    struct  {
	int	CHECKSUM;
	int	CLASSTYPE;
	int	LENGTH;

	u_int	CONTROL;
	u_int	LOG1[eeprom_size_diag_log/4];
	u_int	LOG2[eeprom_size_diag_log/4];
	u_int	LOG3[eeprom_size_diag_log/4];
	u_int	LOG4[eeprom_size_diag_log/4];
	u_int	LOG5[eeprom_size_diag_log/4];
    } log;
  } diag_log;
};

struct EEPROM_S_DIAG_LOG {
	union {
	u_int	HEADER;
	    struct {
	    unsigned NODE : 4; 		/* Node Number */
	    unsigned MODE : 1; 		/* Clr = Powerup, Set = RBD */
	    unsigned rsvd : 3; 		/* */
	    unsigned TEST_NUM : 8; 	/* Failing Test Number */
	    unsigned ERROR_NUM : 8; 	/* Failing Error Number */
	    unsigned ERROR_TYPE : 8; 	/* Error Type */
	    } log_header;
	} header;
	union {
	u_int	HEADER1;
	    struct {
	    unsigned DIAG_NUM : 4; 	/* Diag Number */
	    unsigned COUNT : 12; 	/* Count */
	    unsigned REV : 12; 		/* Revision */
	    unsigned rsvd : 4; 		/* */
	    } log_header1;
	} header1;
	u_int	TIMESTAMP[2];		/* Time */
	char	DESCRIP[8];		/* Description of failing device */
	u_int	LWORD0;
	u_int	LWORD1;
	u_int	LWORD2;
	u_int	LWORD3;
	u_int	LWORD4;
	u_int	LWORD5;
	u_int	LWORD6;
	u_int	LWORD7;
};
#endif

#if EEPROM_DIAG_TDD

/* EEPROM Diagnostic Log Array */

struct EEPROM_S_DIAG_TDD_DEF {
  union  {
    int DIAG_TDD[eeprom_s_diag_tdd/4];    	/* Diagnostic TDD Area */
    struct  {
	int	CHECKSUM;
	int	CLASSTYPE;
	int	LENGTH;

	u_int	CONTROL;
	u_int	LOG1[eeprom_size_diag_tdd/4];
	u_int	LOG2[eeprom_size_diag_tdd/4];
	u_int	LOG3[eeprom_size_diag_tdd/4];
	u_int	LOG4[eeprom_size_diag_tdd/4];
	u_int	LOG5[eeprom_size_diag_tdd/4];
    } tdd;
  } diag_tdd;
};

struct EEPROM_S_DIAG_TDD {
	union {
	u_int	HEADER;
	    struct {
	    unsigned CPU_NODE : 4; 	/* CPU Node Number */
	    unsigned FRU_NODE : 4; 	/* FRU Node Number */
	    unsigned MODE : 1; 		/* Clr = RBD, Set = Powerup */
	    unsigned TEST_NUM : 7; 	/* Failing Test Number */
	    unsigned ERROR_NUM : 6; 	/* Failing Error Number */
	    unsigned ERROR_TYPE : 2; 	/* Error Type */
	    unsigned ERROR_CNT : 7; 	/* Error Count */
	    unsigned MULT_FRU : 1; 	/* Multiple FRU Flag */
	    } tdd_header;
	} header;
	char	NAME[8];		/* Program Name */
	u_int	rsvd1;
/*	u_int	rsvd2;*/
	union {
	u_int	TIMESTAMP0;		/* Time */
	    struct {
	    unsigned SEC : 8; 	/* Seconds */
	    unsigned MINUTE : 8; /* Minutes */
	    unsigned HOUR : 8; 	/* Hours */
	    unsigned YEAR : 8; /* Year */
	    } tdd_time0;
	} timestamp0;
	union {
	u_int	TIMESTAMP1;		/* Second Part of Timestamp */
	    struct {
	    unsigned DAY : 8; 	/* Day */
	    unsigned MONTH : 8; /* Month */
	    unsigned rsvd3 : 16; 		/* */
	    } tdd_time1;
	} timestamp1;
	union {
	u_int	REVISION;		/* Firmware Revision */
	    struct {
	    char  VAR : 8; 	/* Rev Variant*/
	    unsigned  MAJOR : 4; /* Rev Major */
	    unsigned  MINOR : 4; /* Rev Minor */
	    unsigned  SEQ  : 16; /* Rev Sequence*/
	    } tdd_rev;
	} revision;
	u_int	UDATA1[2];		/* User Data 1 */
	u_int	UDATA2[2];		/* User Data 2 */
	u_int	UDATA3[2];		/* User Data 3 */
};
#endif

#if EEPROM_DIAG_SDD

/* EEPROM Diagnostic SDD Array */

struct EEPROM_S_DIAG_SDD_DEF {
  union  {
    int DIAG_SDD[eeprom_s_diag_sdd/4];    	/* Diagnostic SDD Area */
    struct  {
	int	CHECKSUM;
	int	CLASSTYPE;
	int	LENGTH;

	u_int	CONTROL;
	u_int	LOG1[eeprom_size_diag_sdd/4];
	u_int	LOG2[eeprom_size_diag_sdd/4];
	u_int	LOG3[eeprom_size_diag_sdd/4];
    } sdd;
  } diag_sdd;
};

/*+                                                                         */
/* CPU EEPROM Diagnostic SDD log member                                   */
/*-                                                                         */
struct EEPROM_S_DIAG_SDD {
    union  {
        unsigned int sddheader;
        struct  {
            unsigned scb_vector : 16;
            unsigned cpu_node : 4;
			unsigned lsb_sub  : 1;
			unsigned lma_sub  : 1;
			unsigned iop_sub  : 1;
            unsigned reserved : 9;
            } sdd_header;
        } header;
    char process_name [8];
    unsigned int log_length;
    union  {
        unsigned int ltimestamp0;
        struct  {
            unsigned sec : 8;           /* Seconds                          */
            unsigned minute : 8;        /* Minutes                          */
            unsigned hour : 8;          /* Hours                            */
            unsigned year : 8;          /* Year                             */
            } sdd_time0;
        } timestamp0;
    union  {
        unsigned int ltimestamp1;
        struct  {
            unsigned day : 8;           /* Day                              */
            unsigned month : 8;         /* Month                            */
            unsigned rsvd3 : 16;
            } sdd_time1;
        } timestamp1;
    union  {
        unsigned int lrev;
        struct  {
            unsigned var : 8;           /* variant rev                      */
            unsigned major : 4;         /* major rev                        */
            unsigned minor : 4;         /* minor                            */
            unsigned seq : 16;          /* sequence                         */
            } sdd_rev;
        } rev;
	unsigned int failing_pc[2]; 	/* Contents of PC at fail time 	    */
	unsigned int sw_flags[6];	/* 6 Longwords of software flags    */
    unsigned int cpu_data [124];        /* 124 longwords of CPU specific data*/
    } ;


#endif

#if EEPROM_SYM_LOG

/* EEPROM Symptom Array */

struct EEPROM_S_SYM_LOG_DEF {
  union  {
    int SYM_LOG[eeprom_s_sym_log/4];    	/* Symptom Log Area */
    struct  {
	int	CHECKSUM;
	int	CLASSTYPE;
	int	LENGTH;

	u_int	CONTROL;
	u_int	header[eeprom_size_sym_header/4 - 1];
	struct {
	    union {
		u_int buffer[eeprom_size_670_log/4];
		struct {
		    u_int data[(eeprom_size_670_log-4)/4];
		    short int pad;
		    short int lu;
		} log;
	    } log_670[eeprom_k_nbr_sym_logs];
	    union {
		u_int buffer[eeprom_size_660_log/4];
		struct {
		    u_int data[(eeprom_size_660_log-4)/4];
		    short int pad;
		    short int lu;
		} log;
	    } log_660[eeprom_k_nbr_sym_logs];
	    union {
		u_int buffer[eeprom_size_620_log/4];
		struct {
		    u_int data[(eeprom_size_620_log-4)/4];
		    short int pad;
		    short int lu;
		} log;
	    } log_620[eeprom_k_nbr_sym_logs];
	    union {
		u_int buffer[eeprom_size_630_log/4];
		struct {
		    u_int data[(eeprom_size_630_log-4)/4];
		    short int pad;
		    short int lu;
		} log;
	    } log_630[eeprom_k_nbr_630_logs];
	} cpu[2];
    } log;
  } sym_log;
};
#endif

#if EEPROM_HALT_COUNT

/* EEPROM Halt Code Counters Array */

struct EEPROM_S_HALT_CODE_DEF {
  union  {
    int HALT_CODE[eeprom_s_halt_code/4];    /* Halt Code Counter Area */
    struct  {
	int	CHECKSUM;
	int	CLASSTYPE;
	int	LENGTH;

	u_char	HALTCODE[64];
    } haltcode;
  } halt_code;
};
#endif

#if EEPROM_SERVICE_CALL

/* EEPROM service Call Array */

struct EEPROM_S_SERVICE_CALL_DEF {
  union  {
    int SERVICE_CALL[eeprom_s_service_call/4];    /* Service Call Area */
    struct  {
	int	CHECKSUM;
	int	CLASSTYPE;
	int	LENGTH;

	char	SERVICE[64];
    } service;
  } service_call;
};
#endif

#if EEPROM_MOD_HIST

/* EEPROM Module History Array */

struct EEPROM_S_MOD_HIST_DEF {
  union  {
    int MOD_HIST[eeprom_s_mod_hist/4];    	/* Module History Area */
    struct  {
	int	CHECKSUM;
	int	CLASSTYPE;
	int	LENGTH;

	char	CALL[8];
	char	MESSAGE[68];
    } hist;
  } mod_hist;
};

#endif

#if EEPROM_HALT_DATA

struct HALT_LOG {
  int haltcode;
  int rsvd;
  int watchreg[2];
  int SWERRFLAGS[2];
  struct logout LOGOUT;
  struct iod_logout IOD_LOGOUT[4];
 };


/* EEPROM Halt Code Data Array */

struct EEPROM_S_HALT_DATA_DEF {
  union  {
    int HALT_DATA[eeprom_s_halt_data/4];    /* Halt Code Counter Area */
    struct  {
	int	CHECKSUM;
	int	CLASSTYPE;
	int	LENGTH;

	u_int	CONTROL;

	struct {
	    int count;
	    int pass_to_os;
	    struct HALT_LOG log_halt[eeprom_k_nbr_halt_data_logs];
	}cpu[4];
    } log;
  } hlt_data;
};
#endif

#if EEPROM_OS_LOG

/* EEPROM OS log Array */

struct EEPROM_S_OS_LOG_DEF {
  union  {
    int OS_LOG[eeprom_s_os_log/4];    /* OS log Area */
    struct  {
	int	CHECKSUM;
	int	CLASSTYPE;
	int	LENGTH;

	char	OSLOG[eeprom_size_os_log];
    } oslog;
  } os_log;
};
#endif
