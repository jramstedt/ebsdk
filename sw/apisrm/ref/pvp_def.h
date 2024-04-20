/*============================================================================
 *
 *                      Copyright (C) 1994 by
 *              Dynamic Pictures, Inc., Palo Alto, CA.
 *
 *  This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the inclusion
 * of the above copyright notice.  This software or any other copies there-
 * of may not be provided or otherwise made available to any other person.
 * No title to and ownership of the software is hereby transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by Dynamic Pictures, Inc.
 *
 * Dynamic Pictures, Inc. assumes no responsibility for the use or
 * reliability of its software on equipment which is not supplied by
 * Dynamic Pictures, Inc.
 *
 *==========================================================================*/

typedef struct drv_desc		DRV;
typedef struct cp_func_block	CP;
typedef struct scia		SCIA;
typedef struct isc_data		ISC;
typedef struct monitor_data	MONITOR;
typedef struct sfbp_data	SFBP;
typedef struct sfbp_ioseg	IOSEG;
typedef struct dispatch_test_type DISPATCH_TYPE;
typedef struct table_type	TABLE;
typedef struct table_entry	TABLE_ENTRY;
typedef struct sysid_type	SYSID;
typedef union rom_config_reg	ROM_CONFIG;

struct drv_desc {
    unsigned int	device_id;
    unsigned int	addr;
    unsigned int	size;
    unsigned int	ep_offset;
    unsigned int	data_size;
    unsigned int	*data_ptr;
    unsigned int	*ioseg_ptr;
};

struct function_block_desc {
    int			function_block_size;
    unsigned int	*function_block_address;
};

struct cp_func_block {
    int	(*pv_reset_output)();
    int	(*pv_init_output)();
    int	(*pv_putchar)();
    int	(*pv_reset_input)();
    int	(*pv_init_input)();
    int	(*pv_init_driver)();
    int	(*pv_getchar)();
    int	(*pv_clear_vram)();
    int	(*pv_init_vdac)();
    int	(*pv_scroll_row)();
    int	(*pv_test_module)();
};

struct parse_func_block {
    int	(*pv_parse_command_line) ();
    int	(*pv_parse_check) ();
    int	(*pv_subtest_check) ();
    int	(*pv_test_dispatch) ();
    int	(*pv_subtest_dispatch) ();
    int	(*pv_signature_mode_test) ();
    int	(*pv_input_signature_mode_test) ();
    int	(*pv_load_rom_object) ();
    int	(*pv_verify_line) ();
    int	(*pv_rand) ();
};

struct scia {
    char		*usfont_table_ptr;
    char		*mcsfont_table_ptr;
    int			font_width;
    int			font_height;
    int			window_up_down;
    CP			console_func;
    DRV			gfx_drv_desc;
};

enum compare_error_types {
    CompareOk,
    RegisterTestError,
    InterruptTestError,
    PlaneMaskTestError,
    PixelShiftTestError,
    VramTestError,
    SimzTestError,
    StippleTestError,
    CopyTestError,
    BoolTestError,
    LineTestError,
    LineColorCompareError,
    LineZCompareError,
    PackTestError,
    BoxTestError,
    FontTestError,
    VdacTestError,
    PatternTestError,
    FlashError,
    ChairError,
    ICSError,
    InitError,
    CnfgError,
    EditError,
    PixelMaskTestError,
};

enum color_types {
    red,
    green,
    blue,
    red_green,
    red_blue,
    green_blue,
    red_green_blue,
};

enum sfail_values {
    keep,
    zero,
    replace,
    incr,
    decr,
    invert,
};

enum module_type {
    hx_8_plane			/* 0    */
};

typedef enum sfail_values	SFAIL;
typedef enum color_types	COLOR_TYPES;
typedef enum module_type	MODULE;


/* This is used as a bit table for serial load ISC 1562 */
/* Each byte is interpretted as a bit when written      */
/* so the table size is 56 bytes for 56 bits            */
/* This was done for readability                        */

struct isc_data {
    char	data[56];
};

struct monitor_data {
    int	sfbp$l_option;
    int	sfbp$l_monitor_rows;
    int	sfbp$l_monitor_columns;
    int	sfbp$l_sold_freq;
    int	sfbp$l_refresh_rate;
    int	sfbp$l_v_scanlines;
    int	sfbp$l_v_front_porch;
    int	sfbp$l_v_sync;
    int	sfbp$l_v_back_porch;
    int	sfbp$l_d_pixels;
    int	sfbp$l_h_pixels;
    int	sfbp$l_h_front_porch;
    int	sfbp$l_h_sync;
    int	sfbp$l_h_back_porch;
    int	sfbp$l_cursor_x;
    int	sfbp$l_cursor_y;
    ISC	sfbp$r_isc_data;
};

struct sfbp_data {
    int				sfbp$l_scia_address;
    MONITOR			sfbp$r_mon_data;
    struct parse_func_block	sfbp$r_parse_func;
};

struct sfbp_ioseg {
    char	*sfbp$a_rom_debug_mem_ptr;
    char	*sfbp$a_rom_mem_ptr;
    char	*sfbp$a_flash_rom_mem_ptr;
    char	*sfbp$a_reg_space;
    char	*sfbp$a_bt_space;
    char	*sfbp$a_vram_mem_ptr;
    char	*sfbp$a_vram_mem_alias_ptr;
    char	*sfbp$a_vram_mem_dh_ptr;
    char	*sfbp$a_vram_mem_dh_alias_ptr;
    char	*sfbp$l_rom_slot_address;
    char	*sfbp$a_pci_config_ptr;
};

#define SS$K_NODATA	0
#define SS$K_SUCCESS	1
#define SS$K_BADPARAM	2
#define SS$K_ERROR	0

#define BLOCK_FILL_SIZE		2048


struct dispatch_test_type {
    int		subtest;
    int		fault8_data_ptr;
    int		fault32_data_ptr;
    int		fault32z_data_ptr;
    int		error_class;
    int		fault_class;
    int		error_code;
    int		(*rtn)();
    char	*text;
};


enum fault_types {
    nofault,
    fault_asic,
    fault_vram,
    fault_ramdac,
    fault_interrupt,
    fault_checksum,
};

static struct table_type {
    int			clear_memory;
    int			colormap;
    DISPATCH_TYPE	*f;
    int			fsize;
    char		*name;
};

struct  table_entry {
    TABLE	*ptr;
    int		size;
    int		reserved;
    char	*overlay_name;
};

enum line_type_values {
    xlinetype,
    ntlinetype
};

enum state_type_values {
    disabled,
    enabled
};

enum col_type_values {
    Column256,
    Column128,
};

#define myiabs(x)	(((x<0) ? -x : x))

enum font_size {
    BigFont,
    SmallFont,
};

enum video_valid_type {
    NoVideo,                        /* 00   */
    ActiveVideo,                    /* 01   */
    NoVideoToo,                     /* 02   */
    SyncVideo,                      /* 03   */
};

enum erase_char_type {
    NoEraseChar,
    EraseChar,
};

enum sig_test_types {
    OutPutSignature,
    InPutSignature,
};

#define CursorEnable 4                  /* 100  */

struct  sysid_type {
    char	hardware_revision;
    char	firmware_revision;
    char	system_type;
    char	processor_type;
};


union rom_config_reg {
    struct {
        unsigned sfbp$v_data    : 8;
        unsigned sfbp$v_switch  : 4;
        unsigned sfbp$v_module  : 4;
        unsigned sfbp$v_ignore  : 16;
    } sfbp$r_rom_config_bits;
    unsigned int sfbp$l_whole;
};

#define RomWriteData	0
#define Dac0ReadData	16
#define Dac1ReadData	24


#define DMA_BYTES (8192-256)
#define DMA_READ_BYTES DMA_BYTES
#define DMA_WRITE_BYTES (16384-256)


