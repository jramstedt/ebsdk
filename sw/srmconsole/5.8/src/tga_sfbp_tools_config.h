#ifndef _H_config
#define _H_config

typedef int TEMP;

/*
 * TURBOchannel info block.
 */
typedef struct{
  int arch_revision;
  int clk_period;
  int slot_size;
  int timeout;
  int dma_range;
  int max_burst;
  int parity;
  int reserved[4];
} tcinfo_t;

/*
 * Option module ROM header.
 */
typedef struct{
  char     reserved[992];
  unsigned char width;		/* 1, 2, 4 */
  char skip1[3];
  unsigned char stride;		/* 4 */
  char skip2[3];
  unsigned char size;		/* Size of ROM divided by 8192 */
  char skip3[3];
  unsigned char slot_size;	/* Size of TURBOchannel slot in MB */
  char skip4[3];
  unsigned char patterns[16];   /* 0x55555555 0x0 0xAAAAAAAA 0xFFFFFFFF */
} rom_header_t;

/*
 * Module Identifier Block
 */
#define MID_VER_LEN 8
#define MID_VEN_LEN 8
#define MID_NAM_LEN 8
#define MID_FRM_LEN 4
typedef struct{
  char	version[MID_VER_LEN];
  char	vendor[MID_VEN_LEN];
  char  name[MID_NAM_LEN];
  char	firmware_type[MID_FRM_LEN];
  short	rom_stride;
  short	rom_width;
} module_id_t;

/*
 * Object offset definition.
 *
 * Defines offset from rom base to beginning of objects in option module.
 */
#define OBJECT_OFFSET 0x480

/*
 * Memory module configuration entry.  
 * 
 * If mem_size is zero, no module is present.
 */
typedef struct{
  int		mem_size;				
  long		base_addr;
  TEMP		test_status;
} mem_cnfg_t;

/*
 * Option module configuration entry.
 *
 * If module_id.version[0] is zero, no module is present.
 *
 */
typedef struct{
  module_id_t 	module_id;
  TEMP		test_status;
} opt_cnfg_t;

typedef struct{
  unsigned  version : 8,
          data_type : 4,
               mbz0 : 4,
        code_region : 2,
               mbz1 : 14;
  int		length;
  char		name[12];
} object_t;
#define MIPS_I_CODE 0
#define TEXT_DATA   4
#define SYMBOLIC_LINK 5

typedef struct{
  object_t	*ram_list;
  object_t	*rom_list;
  short		rom_stride;
  short		rom_width;
} obj_dir_t;


#define BASE_MODULE (max_option_modules-1)


#endif










