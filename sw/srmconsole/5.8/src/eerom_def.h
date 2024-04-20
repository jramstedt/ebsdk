/********************************************************************************************************************************/
/* Created: 14-DEC-1993 17:03:12 by OpenVMS SDL EV1-19     */
/* Source:  14-DEC-1993 17:02:58 SK8SAV_USR:[BUCKINGHAM.V5CP.SRC]EEROM_DEF.SDL;4 */
/********************************************************************************************************************************/
/* file:    eerom_def.sdl                                                   */
/*                                                                          */
/* Copyright (C) 1991, 1993 by                                              */
/* Digital Equipment Corporation, Maynard, Massachusetts.                   */
/* All rights reserved.                                                     */
/*                                                                          */
/* This software is furnished under a license and may be used and copied    */
/* only  in  accordance  of  the  terms  of  such  license  and with the    */
/* inclusion of the above copyright notice. This software or  any  other    */
/* copies thereof may not be provided or otherwise made available to any    */
/* other person.  No title to and  ownership of the  software is  hereby    */
/* transferred.                                                             */
/*                                                                          */
/* The information in this software is  subject to change without notice    */
/* and  should  not  be  construed  as a commitment by digital equipment    */
/* corporation.                                                             */
/*                                                                          */
/* Digital assumes no responsibility for the use  or  reliability of its    */
/* software on equipment which is not supplied by digital.                  */
/*                                                                          */
/*                                                                          */
/* Abstract:    Cobra Error Log ROM data structure definitions.             */
/*                                                                          */
/* Author:  David Baird                                                     */
/*                                                                          */
/* Modifications:                                                           */
/*                                                                          */
/*  hcb	21-Dec-1993    Created from eerom_def.sdl  Added Sable specific     */
/*		       error logs					    */	
/*                                                                          */
/*  hcb	23-Mar-1993    Change config defs to support pass 4 CPU ASICs       */
/*                                                                          */
/*  hcb	17-Dec-1992    Add prestoserve defs                                 */
/*                                                                          */
/*  hcb	26-Oct-1992    Add some more offsets into the cmm eerom             */
/*                                                                          */
/*  hcb	18-Sep-1992    Add some offsets into the cmm eerom                  */
/*                                                                          */
/*  db	17-Jul-1992    Update for eerom loging spec version 1.1 changes     */
/*                                                                          */
/*  db	16-Jul-1992    Update to fix the CPU TDD entry format               */
/*                                                                          */
/*  db  07-Jul-1992    Update to reflect the latest spec.                   */
/*                                                                          */
/*  db  09-Jun-1992    Add define for Valid SDD Counter number              */
/*                                                                          */
/*  db  08-May-1992    Added default SDD and TDD packet lenghts             */
/*                                                                          */
/*  db  28-Apr-1992    Initial entry.                                       */
/*                                                                          */
/*** MODULE $eeromdef ***/

/* Define packet count constaints                                           */
#define EEROM_CPU_TDD_CNT 2
#define EEROM_CPU_SDD_CNT1 28           /* Number of byte counters          */
#define EEROM_CPU_SDD_CNT2 14           /* Number of B-Cache error logs     */
#define EEROM_CPU_SDD_CNT3 26           /* Number of valid byte counters    */
#define EEROM_CPU_SDD_LEN 32            /* Default CPU SDD sym_log_len      */
#define EEROM_CPU_TDD_LEN 28            /* Default CPU TDD diag_log_len     */
#define EEROM_IO_TDD_CNT 7              /* Number of TDD packetes available */
#define EEROM_IO_SDD_CNT 0              /* Number of SDD packetes available */
#define EEROM_IO_SDD_CNT1 68            /* Number of byte counters          */
#define EEROM_IO_TDD_LEN 28             /* Default IO TDD diag_log_len      */
#if SABLE || RAWHIDE
#define EEROM_IO_SDD_CNT3 18            /* Number of valid byte counters    */
#define EEROM_IO_SDD_LEN 28             /* Default IO SDD sym_log_len       */
#define TOTAL_STM_PKT 15                /* Number of SDD packetes available */
#endif
#define EEROM_MEM_SDD_LEN 4             /* Default MEM SDD sym_log_len      */
#define EEROM_MEM_TDD_LEN 28            /* Default MEM TDD sym_log_len      */
#define EEROM_MEM_SDD_START 56          /* Location of start of SDD structure */
#define EEROM_MEM_LOG_LEN 72            /* Location of SDD log counter      */
#define EEROM_MEM_SDD_ENTRY 76          /* Location of start of SDD entries */
#define EEROM_MEM_PRESTO_ID 253         /* Location of PrestoServe ID       */
#define EEROM_MEM_SIMM_TYPE 4		/* SIMM Module type ID		    */
#define EEROM_MEM_60NS_RAM 16           /* 60ns DRAM type ID		    */
#define EEROM_MOD_TYPE 254              /* Location of Module type info	    */
/* Define General Error log data structures                                 */
struct EEPROM_HEAD {
    int checksum;
    short int class_type;
    short int subclass_ver;
    int len;
    int fru_ptr;
    int sdd_ptr;
    int tdd_ptr;
    int reserved;
    } ;
struct EEPROM_FRU {
    int checksum;
    short int class_type;
    short int subclass_ver;
    int len;
    char serial_num [5];
    char partnum1 [1];
    char partnum2 [7];
    char sw_rev;
    char hw_rev [2];
#if RAWHIDE
    char sys_serial[5];
    char module_type[15];
#endif
    } ;
/* Define CPU specific packet structures                                    */
struct EEROM_CPU_TDD_PKT {
    unsigned int addr_l;
    unsigned short int addr_h;
    unsigned char test;
    unsigned char sub_test;
    unsigned int expected_l;
    unsigned int expected_h;
    unsigned int actual_l;
    unsigned int actual_h;
    } ;
struct EEROM_CPU_SDD_PKT {
    unsigned char syndrome;
    unsigned char offset_l;
    unsigned char offset_h;
    unsigned char count;
    } ;
struct EEPROM_CPU_TDD {
    int checksum;
    short int class_type;
    short int subclass_ver;
    int len;
    unsigned int diag_log_ctrl;
    int diag_log_len;
    struct EEROM_CPU_TDD_PKT diag_pkt [2];
    unsigned char reserved [16];
    } ;
struct EEPROM_CPU_SDD {
    int checksum;
    short int class_type;
    short int subclass_ver;
    int len;
    unsigned int sym_log_ctrl;
    int sym_log_len;
    char err_counts [28];
    struct EEROM_CPU_SDD_PKT sym_pkt [14];
    } ;
struct EEPROM_CPU_DIAG {
    struct EEPROM_CPU_SDD sdd_pkt;
    struct EEPROM_CPU_TDD tdd_pkt;
    } ;
struct CPU_IIC_CONF {
    unsigned char reserved1;
    unsigned char cbus_speed;
    unsigned char biu_ctl [8];
    unsigned mod_type : 3;
    unsigned reserved : 3;
    unsigned run_test : 1;
    unsigned unload : 1;
    unsigned char checksum;
    } ;
struct CPU_IIC_EEPROM {
    struct EEPROM_HEAD header;
    struct EEPROM_FRU fru_desc;
    struct EEPROM_CPU_DIAG diag;
    struct CPU_IIC_CONF conf_info;
    } ;
/* Define IO specific packet structures                                     */
struct EEROM_IO_TDD_PKT {
    unsigned int data1;
    unsigned int data2;
    } ;
struct EEPROM_IO_TDD {
    int checksum;
    short int class_type;
    short int subclass_ver;
    int len;
    unsigned int diag_log_ctrl;
    int diag_log_len;
    struct EEROM_CPU_TDD_PKT srom_err;
    struct EEROM_IO_TDD_PKT diag_pkt [7];
    } ;
struct EEPROM_IO_SDD {
    int checksum;
    short int class_type;
    short int subclass_ver;
    int len;
    unsigned int sym_log_ctrl;
    int sym_log_len;
    char err_counts [68];
    } ;
struct EEPROM_IO_DIAG {
    struct EEPROM_IO_SDD sdd_pkt;
    struct EEPROM_IO_TDD tdd_pkt;
    } ;
struct IO_IIC_CONF {
    unsigned char reserved [8];
    unsigned char reserved0;
    unsigned char reserved1;
    unsigned char reserved2;
    unsigned char checksum;
    } ;
struct IO_IIC_EEPROM {
    struct EEPROM_HEAD header;
    struct EEPROM_FRU fru_desc;
    struct EEPROM_IO_DIAG diag;
    struct IO_IIC_CONF conf_info;
    } ;
/* Define Memory specific packet structures                                 */
#if SABLE || RAWHIDE
struct BAD_BIT_DESCRIP {
    unsigned int fail_address;
    unsigned char reserved;
    unsigned error_type : 2;
    unsigned asic_id : 1;
    unsigned bank_num : 2;
    unsigned source_id : 3;
    union {
	unsigned short corr_synd;
	struct {
	    unsigned char bit1;
	    unsigned char bit2;
	    } uncor;
	} bits; 
    } ;
struct CMM_IIC_EEPROM_SDD {
    int checksum;
    short int class_type;
    short int subclass_ver;
    int len;
    unsigned int sym_log_ctrl;
    int sym_log_len;
    struct BAD_BIT_DESCRIP symptom_pkt [TOTAL_STM_PKT];
    int reserved;
    } ;
#endif
struct EEPROM_MEM_TDD {
    int checksum;
    short int class_type;
    short int subclass_ver;
    int len;
    unsigned int diag_log_ctrl;
    int diag_log_len;
    struct EEROM_CPU_TDD_PKT srom_err;
    } ;
struct CMM_IIC_CONF {
    unsigned char reserved [8];
    unsigned char mod_size;
    unsigned char presto_size;
    unsigned char reserved2;
    unsigned char checksum;
    } ;
struct EEPROM_MEM_DIAG {
    struct CMM_IIC_EEPROM_SDD sdd_pkt;
    struct EEPROM_MEM_TDD tdd_pkt;
    } ;
struct CMM_IIC_EEPROM {
    struct EEPROM_HEAD header;
    struct EEPROM_FRU fru_desc;
    struct CMM_IIC_EEPROM_SDD symp_log;
    struct EEPROM_MEM_TDD diag_log;
    struct CMM_IIC_CONF conf_info;
    } ;
/* Union of all IIC EEROM formats                                           */
struct IIC_EEPROM {
    struct EEPROM_HEAD header;
    struct EEPROM_FRU fru_desc;
#if !RAWHIDE
    union  {
        struct EEPROM_CPU_DIAG cpu;
        struct EEPROM_IO_DIAG io;
        struct EEPROM_MEM_DIAG mem;
        } diag;
    union  {
        struct CPU_IIC_CONF cpu;
        struct IO_IIC_CONF io;
        struct CMM_IIC_CONF mem;
        } conf;
#endif
    } ;
 
