#define	TgaPass1		1
#define	TgaPass2		2
#define	TgaPass2Plus		3
#define	TgaPass3    		4

#define PIXEL_BIAS 0

#define SPARSE_FACTOR 1
#define sfbp$k_font_width                 8
#define sfbp$k_font_size                 15

#define MAXCOLORS                       32
#define sfbp$k_console_text_lines        6
#define sfbp$k_cursor_start_row          0
#define sfbp$k_cursor_start_column       0
#define sfbp$k_space_index               0
#define sfbp$k_under_score_index         63

#define sfbp$k_mono              1
#define sfbp$k_multi             8
#define sfbp$k_start_screen      1
#define sfbp$k_begin_line        2
#define sfbp$k_begin_next_line   3
#define sfbp$k_next_line         4
#define sfbp$k_forward           5
#define sfbp$k_backward          6        
#define sfbp$k_end_last_line     7
#define sfbp$k_current_position  8
#define sfbp$k_same              9
#define sfbp$k_one_mill          1024*1024
#define sfbp$k_two_mill          1024*1024*2

#define sfbp$k_backspace           8
#define sfbp$k_delete            127
#define sfbp$k_tab                 9
#define sfbp$k_line_feed          10
#define sfbp$k_carriage_return    13
#define sfbp$k_control_c 3

#define sfbp$k_a_sfbp_reg_space         0x100000
#define sfbp$k_a_sfbp_rom_space         0x000000
#define sfbp$k_a_sfbp_rom_debug         0x630000
#define sfbp$k_a_sfbp_pci_space         0x010000
#define sfbp$k_a_sfbp_pci_config_vnd    0x00
#define sfbp$k_a_sfbp_pci_config_cmd    0x04
#define sfbp$k_a_sfbp_pci_config_rev    0x08
#define sfbp$k_a_sfbp_pci_config_cac    0x0C
#define sfbp$k_a_sfbp_pci_config_bas    0x10
#define sfbp$k_a_sfbp_pci_config_rom    0x30
#define sfbp$k_a_sfbp_pci_config_int    0x3C

#define sfbp$k_vram_start_address       0x200000
#define sfbp$k_a_sfbp_vram_mem_8        0x200000
#define sfbp$k_a_sfbp_vram_mem_alias    0x400000
#define sfbp$k_a_sfbp_vram_mem_overlay  0x400000

#define sfbp$k_a_sfbp_ofs_mem           0x800000
#define sfbp$k_a_sfbp_vram_mem_dh       0xA00000
#define sfbp$k_a_sfbp_vram_mem_dh_alias 0xC00000

#define sfbp$k_a_sfbp_vram_mem_32_noz   0x0800000
#define sfbp$k_a_sfbp_vram_mem_32_z     0x1000000
#define sfbp$k_a_sfbp_zbuf_mem_32       0x1800000

#ifdef HX
#define sfbp$k_a_sfbp_bt_space          0x1C0000
#else
#define sfbp$k_a_sfbp_bt_space          0x1001F0
#endif

#define  sfbp$k_copy_reg_0		0x00 
#define  sfbp$k_copy_reg_1		0x04 
#define  sfbp$k_copy_reg_2		0x08 
#define  sfbp$k_copy_reg_3		0x0C 
#define	 sfbp$k_copy_reg_4		0x10 
#define	 sfbp$k_copy_reg_5		0x14 
#define	 sfbp$k_copy_reg_6		0x18 
#define	 sfbp$k_copy_reg_7		0x1C 
#define	 sfbp$k_foreground		0x20 
#define	 sfbp$k_background		0x24 
#define	 sfbp$k_plane_mask		0x28 
#define	 sfbp$k_pixel_mask		0x2C 
#define	 sfbp$k_mode		        0x30 
#define	 sfbp$k_boolean_op		0x34 
#define	 sfbp$k_pixel_shift		0x38 
#define	 sfbp$k_line_address		0x3C 
#define	 sfbp$k_bresh_reg1		0x40 
#define	 sfbp$k_bresh_reg2		0x44 
#define	 sfbp$k_bresh_reg3		0x48 
#define	 sfbp$k_bresh_continue		0x4C 
#define	 sfbp$k_deep		        0x50 
#define	 sfbp$k_start		        0x54 
#define	 sfbp$k_stencil_mode            0x58 
#define	 sfbp$k_persistent_pmask        0x5C 
#define	 sfbp$k_cursor_base		0x60 
#define	 sfbp$k_horizontal		0x64 
#define	 sfbp$k_vertical		0x68 
#define	 sfbp$k_refresh_addr		0x6C 
#define	 sfbp$k_video_valid		0x70 
#define	 sfbp$k_cursor_xy		0x74
#define	 sfbp$k_video_status		0x78 
#define	 sfbp$k_int_status              0x7C
#define	 sfbp$k_data_register		0x80 
#define	 sfbp$k_red_increment		0x84 
#define	 sfbp$k_green_increment		0x88 
#define	 sfbp$k_blue_increment		0x8C 
#define	 sfbp$k_z_increment_fraction	0x90 
#define	 sfbp$k_z_increment_whole	0x94 
#define	 sfbp$k_dma_base_address	0x98 
#define	 sfbp$k_bresh_width		0x9C 
#define	 sfbp$k_z_value_fraction	0xA0 
#define	 sfbp$k_z_value_whole		0xA4 
#define	 sfbp$k_z_base_addr		0xA8 
#define	 sfbp$k_address		        0xAC 
#define	 sfbp$k_red_value		0xB0 
#define	 sfbp$k_green_value		0xB4 
#define	 sfbp$k_blue_value		0xB8 
#define	 sfbp$k_span_width		0xBC 
#define	 sfbp$k_ramdac_setup            0xC0     



#define	 sfbp$k_slope_nogo_0		0x100 
#define	 sfbp$k_slope_nogo_1		0x104 
#define	 sfbp$k_slope_nogo_2		0x108 
#define	 sfbp$k_slope_nogo_3		0x10C 
#define	 sfbp$k_slope_nogo_4		0x110 
#define	 sfbp$k_slope_nogo_5		0x114 
#define	 sfbp$k_slope_nogo_6		0x118 
#define	 sfbp$k_slope_nogo_7		0x11C 
#define	 sfbp$k_slope_go_0		0x120 
#define	 sfbp$k_slope_go_1		0x124 
#define	 sfbp$k_slope_go_2		0x128 
#define	 sfbp$k_slope_go_3		0x12C 
#define	 sfbp$k_slope_go_4		0x130 
#define	 sfbp$k_slope_go_5		0x134 
#define	 sfbp$k_slope_go_6		0x138 
#define	 sfbp$k_slope_go_7		0x13C 

#define	 sfbp$k_block_color_0		0x140
#define	 sfbp$k_block_color_1		0x144
#define	 sfbp$k_block_color_2		0x148
#define	 sfbp$k_block_color_3		0x14C
#define	 sfbp$k_block_color_4		0x150
#define	 sfbp$k_block_color_5		0x154
#define	 sfbp$k_block_color_6		0x158
#define	 sfbp$k_block_color_7		0x15c
#define	 sfbp$k_copy64_src	        0x160
#define	 sfbp$k_copy64_dst              0x164
#define	 sfbp$k_copy64_src_alias	0x168
#define	 sfbp$k_copy64_dst_alias        0x16C

#define	 sfbp$k_eprom                   0x1E0
#define	 sfbp$k_ics_1562                0x1E8 
#define	 sfbp$k_ramdac                  0x1F0 
#define	 sfbp$k_command_status		0x1FC 

union control_reg {
    struct  {
        unsigned sfbp$v_con_mode   :  8;
        unsigned sfbp$v_con_depth  :  3;                        /* source depth */
        unsigned sfbp$v_con_rotate :  2;                        /* source rotate*/
        unsigned sfbp$v_linet      :  1;                        /* Windows linet*/
        unsigned sfbp$v_con_zsize  :  1;                        /* 16 bit Z     */
        unsigned sfbp$v_con_cap    :  1;                        /* capend       */
        unsigned sfbp$v_con_mbz2   :  4;
        unsigned sfbp$v_con_copy_flag   :  1;                     
        unsigned sfbp$v_con_berror_flag :  1;                     
        unsigned sfbp$v_con_addr_flag   :  1;                     
        unsigned sfbp$v_con_persistent_flag :  1;                     
        unsigned sfbp$v_con_mbz3   : 8;
        } sfbp$r_control_bits;
    unsigned int sfbp$l_whole;
    } ;

union boolean_reg {
    struct  {
        unsigned sfbp$v_bool_code   :  4;
        unsigned sfbp$v_bool_mbz    :  4;
        unsigned sfbp$v_bool_visual :  2;
        unsigned sfbp$v_bool_rotate :  2;
        } sfbp$r_boolean_bits;
    unsigned int sfbp$l_whole;
    } ;

union stencil_reg {
    struct  {
        unsigned sfbp$v_stencil_wmask  :  8;
        unsigned sfbp$v_stencil_rmask  :  8;
        unsigned sfbp$v_stencil_stest  :  3;
        unsigned sfbp$v_stencil_sfail  :  3;
        unsigned sfbp$v_stencil_zfail  :  3;
        unsigned sfbp$v_stencil_zpass  :  3;
        unsigned sfbp$v_stencil_ztest  :  3;
        unsigned sfbp$v_stencil_z      :  1;
        } sfbp$r_stencil_bits;
    unsigned int sfbp$l_whole;
    } ;

#define sfbp$m_plane -1
union plane_reg {
    struct  {
        unsigned sfbp$v_plane : 32;
        } sfbp$r_plane_bits;
    unsigned int sfbp$l_whole;
    } ;

union color_value_reg {
    struct  {
        unsigned sfbp$v_fraction : 12;
        unsigned sfbp$v_value    :  8;
        unsigned sfbp$v_ignore2  :  7;
        unsigned sfbp$v_dither   :  5;
        } sfbp$r_color_value_bits;
    unsigned int sfbp$l_whole;
    } ;


#define sfbp$m_foreground -1
union fg_reg {
    struct  {
        unsigned sfbp$v_foreground : 32;
        } sfbp$r_fg_bits;
    unsigned int sfbp$l_whole;
    } ;
#define sfbp$m_background -1
union bg_reg {
    struct  {
        unsigned sfbp$v_background : 32;
        } sfbp$r_bg_bits;
    unsigned int sfbp$l_whole;
    } ;

union deep_reg {
    struct  {
        unsigned sfbp$v_deep    : 1;
        unsigned sfbp$v_zero    : 1;
        unsigned sfbp$v_mask    : 3;
        unsigned sfbp$v_blk0    : 1;
        unsigned sfbp$v_blk1    : 1;
        unsigned sfbp$v_blk2    : 1;
        unsigned sfbp$v_blk3    : 1;
        unsigned sfbp$v_col     : 1;
        unsigned sfbp$v_sam     : 1;
        unsigned sfbp$v_parityf : 1;
        unsigned sfbp$v_flashe  : 1;
        unsigned sfbp$v_block   : 1;
        unsigned sfbp$v_slodac  : 1;
        unsigned sfbp$v_dma_size: 1;
        unsigned sfbp$v_hsync   : 1;
        unsigned sfbp$v_mbz     :15;
        } sfbp$r_deep_bits;
    unsigned int sfbp$l_whole;
    } ;
union int_reg {
    struct  {
        unsigned sfbp$v_end_frame   :  1;
        unsigned sfbp$v_end_scan    :  1;
        unsigned sfbp$v_dma_error   :  1;
        unsigned sfbp$v_parity      :  1;
        unsigned sfbp$v_timer       :  1;
        unsigned sfbp$v_int_mbz1    : 11;
        unsigned sfbp$v_e_end_frame :  1;
        unsigned sfbp$v_e_end_scan  :  1;
        unsigned sfbp$v_e_dma_error :  1;
        unsigned sfbp$v_e_parity    :  1;
        unsigned sfbp$v_e_timer     :  1;
        unsigned sfbp$v_int_mbz2    : 11;
        } sfbp$r_int_bits;
    unsigned int sfbp$l_whole;
    } ;

#define sfbp$m_pshift_shift 15
#define sfbp$m_pshift_mbz -16
union pixel_shift_reg {
    struct  {
        unsigned sfbp$v_pshift_shift : 4;
        unsigned sfbp$v_pshift_mbz : 28;
        } sfbp$r_pixel_shift_bits;
    unsigned int sfbp$l_whole;
    } ;

#define sfbp$m_pmask_mask -1
union pixel_mask_reg {
    struct  {
        unsigned sfbp$v_pmask_mask : 32;
        } sfbp$r_pixel_mask_bits;
    unsigned int sfbp$l_whole;
    } ;

#define sfbp$m_bresh_err_inc 65535
#define sfbp$m_bresh_add_inc -65536
union bresh_reg1 {
    struct  {
        unsigned sfbp$v_bresh_err_inc : 16;
        unsigned sfbp$v_bresh_add_inc : 16;
        } sfbp$r_bresh_reg1_bits;
    unsigned int sfbp$l_whole;
    } ;

union bresh_reg2 {
    struct  {
        unsigned sfbp$v_bresh_err_inc : 16;
        unsigned sfbp$v_bresh_add_inc : 16;
        } sfbp$r_bresh_reg2_bits;
    unsigned int sfbp$l_whole;
    } ;

union bresh_reg3 {
    struct  {
        unsigned sfbp$v_bresh_length : 4;
        unsigned sfbp$v_bresh_mbz : 11;
        unsigned sfbp$v_bresh_ierror : 17;
        } sfbp$r_bresh_reg3_bits;
    unsigned int sfbp$l_whole;
    } ;

union bresh_width {
    struct  {
        unsigned sfbp$v_bresh_width : 16;
        unsigned sfbp$v_bresh_zwidth : 16;
        } sfbp$r_bresh_width_bits;
    unsigned int sfbp$l_whole;
    } ;

union line_addr_reg {
    struct  {
        unsigned sfbp$v_line_address : 32;
        } sfbp$r_line_addr_bits;
    unsigned int sfbp$l_whole;
    } ;

#define sfbp$m_refresh_address 1023
#define sfbp$m_refresh_mbz 64512
union refresh_addr_reg {
    struct  {
        unsigned sfbp$v_refresh_address : 10;
        unsigned sfbp$v_refresh_mbz : 6;
        } sfbp$r_refresh_addr_bits;
    unsigned int sfbp$l_whole;
    } ;

#define sfbp$m_htime_pixels 511
#define sfbp$m_htime_front_porch 15872
#define sfbp$m_htime_sync 2080768
#define sfbp$m_htime_back_porch 266338304
union video_htiming_reg {
    struct  {
        unsigned sfbp$v_htime_pixels : 9;
        unsigned sfbp$v_htime_front_porch : 5;
        unsigned sfbp$v_htime_sync : 7;
        unsigned sfbp$v_htime_back_porch : 7;
        unsigned sfbp$v_fill_0 : 3;
        unsigned sfbp$v_odd    : 1;
        } sfbp$r_video_h_timing_bits;
    unsigned int sfbp$l_whole;
    } ;

#define sfbp$m_vtime_scanlines 2047
#define sfbp$m_vtime_front_porch 63488
#define sfbp$m_vtime_sync 4128768
#define sfbp$m_vtime_back_porch 264241152
union video_vtiming_reg {
    struct  {
        unsigned sfbp$v_vtime_scanlines : 11;
        unsigned sfbp$v_vtime_front_porch : 5;
        unsigned sfbp$v_vtime_sync : 6;
        unsigned sfbp$v_vtime_back_porch : 6;
        unsigned sfbp$v_fill_1 : 3;
        unsigned sfbp$v_stereo : 1;
        } sfbp$r_video_v_timing_bits;
    unsigned int sfbp$l_whole;
    } ;


struct buffer_desc {
    unsigned short int size;
    unsigned short int flags;
    unsigned char *ADDRESS;
    } ;

struct drv_desc {
    unsigned int device_id;
    unsigned int addr;
    unsigned int size;
    unsigned int ep_offset;
    unsigned int data_size;
    unsigned int *data_ptr;
    unsigned int *ioseg_ptr;
    } ;

struct function_block_desc 
    {
    int function_block_size;
    unsigned int *function_block_address;
    } ;

struct cp_func_block {
    int (*pv_reset_output)();
    int (*pv_init_output)();
    int (*pv_putchar)();
    int (*pv_reset_input)();
    int (*pv_init_input)();
    int (*pv_init_driver)();
    int (*pv_getchar)();
    int (*pv_clear_vram)();
    int (*pv_init_vdac)();
    int (*pv_scroll_row)();
    int (*pv_test_module)();
    } ;

struct gfx_func_block {
    int (*pv_init_param)();
    int (*pv_sfbp_write_reg)();
    int (*pv_sfbp_read_reg)();
    int (*pv_sfbp_write_vram)();
    int (*pv_sfbp_read_vram)();
    int (*pv_sfbp_write_rom)();
    int (*pv_sfbp_read_rom)();
    int (*pv_sfbp_error)();
    int (*pv_disable_interrupt)();
    int (*pv_enable_interrupt)();
    int (*pv_clear_int_req)();
    int (*pv_wait_retrace)();
    int (*pv_test_int)();
    int (*pv_clear_vram)();
    int (*pv_fill_vram)();
    int (*pv_scroll_row)();
    int (*pv_configure_video)();
    int (*pv_get_expect)();
    int (*pv_sfbp_drawline)();
    int (*pv_sfbp_unaligned_copy)();
    int (*pv_sfbp_aligned_copy)();
    int (*pv_sfbp_stipple)();
    int (*pv_sfbp_wait_csr_ready)();
    int (*pv_sfbp_write_pci_config)();
    int (*pv_sfbp_read_pci_config)();
    } ;


struct bt_func_block {
    int (*pv_bt_init)();
    int (*pv_bt_write_reg)();
    int (*pv_bt_read_reg)();
    int (*pv_bt_read_sig)();
    int (*pv_bt_read_lo)();
    int (*pv_bt_read_hi)();
    int (*pv_bt_load_address)();
    int (*pv_bt_load_def_color)();
    int (*pv_bt_load_888_color)();
    int (*pv_bt_load_444_color)();
    int (*pv_bt_load_332_color)();
    int (*pv_bt_write_color)();
    int (*pv_bt_write_cur_color)();
    int (*pv_bt_write_ovl_color)();
    int (*pv_bt_write_sig)();
    int (*pv_bt_read_color)();
    int (*pv_bt_read_cur_color)();
    int (*pv_bt_read_ovl_color)();
    int (*pv_bt_update_cursor)();
    int (*pv_bt_pos_cursor)();
    int (*pv_bt_alloc_color)();
    int (*pv_bt_free_color)();
    int (*pv_bt_free_all_color)();
    int (*pv_bt_wait_interrupt)();
    int (*pv_bt_dma_checksum)();
    int (*pv_bt_cursor_ram)();
    } ;

struct parse_func_block {
    int (*pv_parse_command_line) ();
    int (*pv_parse_check) ();
    int (*pv_subtest_check) ();
    int (*pv_test_dispatch) ();
    int (*pv_subtest_dispatch) ();
    int (*pv_signature_mode_test) ();
    int (*pv_input_signature_mode_test) ();
    int (*pv_load_rom_object) ();
    int (*pv_verify_line) ();
    int (*pv_rand) ();
    } ;

struct scia {
    char *usfont_table_ptr;
    char *mcsfont_table_ptr;
    int font_width;
    int font_height;
    int window_up_down;
    struct cp_func_block console_func;
    struct drv_desc gfx_drv_desc;
    } ;

/* Output Signature     */
/* 1 for each 4:1 mux   */
/* 1 for each color     */
struct signature_pixel_type {
    int pixel [4];
    } ;

struct signature_type {
    int sfbp$l_test;
    struct signature_pixel_type sfbp$r_sig [3];
    } ;

/* Input Signature      */
/* 2 reads              */
/* 1 for each color     */
struct input_signature_pixel_type {
    int pixel [4];
    } ;

/* And then 1 read for color    */
struct input_signature_type {
    int sfbp$l_test;
    struct signature_pixel_type sfbp$r_sig [3];
    } ;

enum  sam_types
    {
    sam_512,
    sam_256,
    };
    
enum block_types
    {
    column4,
    column8,
    };
    
enum  compare_error_types
    {
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
    
enum  blk_types
     {
     blk_ti_4m,
     blk_nec_4m,
     blk_nec_2m,
     };

enum dma_size_types
     {
     dma_64,
     dma_128,
     };

enum depth_types
     {
     depth_8,
     depth_32,
     };

enum entry_types
     {
     MaxEntries,                        /* 8 entries for block mode     */
     MinEntries,                        /* 2 entries for block mode     */
     };
              
#define mask_4MB        0
#define mask_8MB        1
#define mask_16MB       3
#define mask_32MB       7
        
enum  enable_types
        {
        disable,
        enable,
        };
        
enum  slope_type
        {
        go,
        nogo,
        };

typedef enum slope_type SLOPE;
      
enum  ramdac_type
        {
        bt459,
        bt463,
        bt485,
        };
        
typedef enum ramdac_type RAMDAC;

enum    module_type 
        {
        hx_8_plane,                             /* 0    */
        hx_32_plane_no_z,                       /* 1    */
        hx_8_plane_dual_head,                   /* 2    */
        hx_32_plane_z,                          /* 3    */
        };
        
typedef enum module_type MODULE;

/* This is used as a bit table for serial load ISC 1562 */
/* Each byte is interpretted as a bit when written      */
/* so the table size is 56 bytes for 56 bits            */
/* This was done for readability                        */

struct isc_data 
        {                      
        char     data[56];
        };
        
typedef struct isc_data ISC;

struct monitor_data {                      
    int sfbp$l_option;
    int sfbp$l_monitor_rows;               
    int sfbp$l_monitor_columns;            
    int sfbp$l_sold_freq;                  
    int sfbp$l_refresh_rate;               
    int sfbp$l_v_scanlines;                
    int sfbp$l_v_front_porch;              
    int sfbp$l_v_sync;                     
    int sfbp$l_v_back_porch;               
    int sfbp$l_d_pixels;                   
    int sfbp$l_h_pixels;                   
    int sfbp$l_h_front_porch;              
    int sfbp$l_h_sync;                     
    int sfbp$l_h_back_porch;               
    int sfbp$l_cursor_x;                   
    int sfbp$l_cursor_y;                   
    ISC sfbp$r_isc_data;
    } ;
         
struct sfbp_data {
    int  sfbp$l_scia_address;                       
    int  sfbp$l_vram_size;                  
    int  sfbp$l_planes;                     
    int  sfbp$l_cursor_max_column;                  
    int  sfbp$l_cursor_row;                         
    int  sfbp$l_cursor_column;                      
    int  sfbp$l_cursor_max_row;                     
    int  sfbp$l_bytes_per_pixel;                      
    int  sfbp$l_bytes_per_row;                      
    int  sfbp$l_bytes_per_scanline;                 
    int  sfbp$l_bytes_per_column;                   
    int  sfbp$l_foregroundvalue;
    int  sfbp$l_backgroundvalue; 
    int  sfbp$l_cell_font_width;                    
    int  sfbp$l_cell_byte_offset;                   
    int  sfbp$l_slot_number;                        
    int  sfbp$l_ccv_vector;                         
    int  sfbp$l_dct_ptr;                            
    int  sfbp$l_rom_stride;
    int  sfbp$l_flash_rom_stride;
    int  sfbp$l_rom_size;
    int  sfbp$l_conte;
    int  sfbp$l_loope;
    int  sfbp$l_test_loop;
    int  sfbp$l_subtest_start;
    int  sfbp$l_subtest_end;
    int  sfbp$l_verbose;
    int  sfbp$l_font_char;
    int  sfbp$l_test_lines;
    int  sfbp$l_copy_rows;
    int  sfbp$l_stip_rows;
    int  sfbp$l_alt_console;
    int  sfbp$l_double_buffer;
    int  sfbp$l_cursor_update;
    int  sfbp$l_white_on_black;
    int  sfbp$l_mcs_font;
    int  sfbp$l_display_clear;
    int  sfbp$l_cursor_offset;
    int  sfbp$l_console_offset;
    int  sfbp$l_server_offset;
    int *sfbp$l_start_crt_code;
    int *sfbp$l_start_driver_code;
    int *sfbp$l_start_utility_code;
    int *sfbp$l_start_ramdac_code;
    int  sfbp$l_flash_slot;
    int  sfbp$l_test_slot;
    int  sfbp$l_crtslot;
    int  sfbp$l_etch_revision;
    int  sfbp$l_span_flag;
    int  sfbp$l_go_flag;
    int  sfbp$l_print;
    int  sfbp$l_ramdac_print;
    int  sfbp$l_supplemental;
    int  sfbp$l_signature_test;
    int  sfbp$l_verify_lines;
    int  sfbp$l_manufacturing_mode;
    int  sfbp$l_block[4];
    int  sfbp$l_sam;
    int  sfbp$l_col;
    int  sfbp$l_mask;
    int  sfbp$l_tci_driver_loaded;
    int  sfbp$l_crt_driver_loaded;
    int  sfbp$l_tci_ramdac_loaded;
    int  sfbp$l_crt_ramdac_loaded;
    int  sfbp$l_console_driver;
    int  sfbp$l_device_initialized;
    int  sfbp$l_pixel_mask;
    int  sfbp$l_rotate_src_mask;
    int  sfbp$l_rotate_dst_mask;
    int  sfbp$l_hardware_assist;
    int  sfbp$l_current_head;
    int  sfbp$l_dma_read_dither;
    int  sfbp$l_dma_read_initial;
    int  sfbp$l_heads;
    int  sfbp$l_vram_base;
    int  sfbp$l_line_test_mode;
    int  sfbp$l_line_type_mode;
    int  sfbp$l_rgb_format;
    int  sfbp$l_error_type;
    int  sfbp$l_fault_class;
    int  sfbp$l_fault_isolate;
    int  sfbp$l_default_mode;
    int  sfbp$l_default_rop;
    int  sfbp$l_interactive;
    int  sfbp$l_font_size;
    int  sfbp$l_quick_verify;
    int  sfbp$l_cursor_offchip;
    int  sfbp$l_test_zbuffer;
    int  sfbp$l_nocint_change;
    int  sfbp$l_doblock_mode;
    int  sfbp$l_doparity;
    int  sfbp$l_dostereo;
    int  sfbp$l_doodd;
    int  sfbp$l_dopixel_bias;
    int  sfbp$l_signature_error[4];
    int  sfbp$l_test_error;
    int  sfbp$l_vga_font;
    int  sfbp$l_int_poll;
    int  sfbp$l_dma_scroll;
    int  sfbp$l_dma_sync;
    int  sfbp$l_alpha_platform;
    int  sfbp$l_list_tests;
    int  sfbp$l_test_offchip;
    int  sfbp$l_diagnostics;
    int  sfbp$l_8bit_visual;
    int  sfbp$l_override;
    int  sfbp$l_no_dma_verify;
    int  sfbp$l_color [MAXCOLORS];
    char sfbp$a_overlay_name[20];
    RAMDAC   sfbp$l_ramdac;                     
    MODULE   sfbp$l_module;                     
    struct signature_type sfbp$r_snap;
    struct monitor_data sfbp$r_mon_data;
    struct gfx_func_block sfbp$r_graphics_func;
    struct parse_func_block sfbp$r_parse_func;
    struct bt_func_block  sfbp$r_bt_func;
    } ;

struct sfbp_ioseg {
    char *sfbp$a_rom_debug_mem_ptr;
    char *sfbp$a_rom_mem_ptr;
    char *sfbp$a_flash_rom_mem_ptr;
    char *sfbp$a_reg_space;
    char *sfbp$a_bt_space;
    char *sfbp$a_vram_mem_ptr;
    char *sfbp$a_vram_mem_alias_ptr;
    char *sfbp$a_vram_mem_dh_ptr;
    char *sfbp$a_vram_mem_dh_alias_ptr;
    char *sfbp$l_rom_slot_address;                   
    char *sfbp$a_pci_config_ptr;
    } ;

#define sfbp$k_simple                           	0x00
#define sfbp$k_simple_z                         	0x10
#define sfbp$k_opaque_stipple                   	0x01
#define sfbp$k_opaque_fill                      	0x21
#define sfbp$k_transparent_stipple              	0x05
#define sfbp$k_transparent_fill                 	0x25
#define sfbp$k_transparent_block_stipple        	0x0D
#define sfbp$k_transparent_block_fill           	0x2D
#define sfbp$k_opaque_line                      	0x02
#define sfbp$k_transparent_line                 	0x06
#define sfbp$k_cint_trans_non_dither_line       	0x0E
#define sfbp$k_cint_trans_dither_line           	0x2E
#define sfbp$k_cint_trans_non_dither_line_red       	0x4E
#define sfbp$k_zbuff_opaque_line                	0x12
#define sfbp$k_zbuff_transparent_line           	0x16
#define sfbp$k_zbuff_cint_opaq_non_dither_line  	0x1A
#define sfbp$k_zbuff_cint_opaque_non_dither_line_red  	0x5A
#define sfbp$k_zbuff_cint_opaq_dither_line      	0x3A
#define sfbp$k_zbuff_cint_trans_non_dither_line 	0x1E
#define sfbp$k_zbuff_cint_trans_non_dither_line_red   	0x5E
#define sfbp$k_zbuff_cint_trans_dither_line     	0x3E
#define sfbp$k_copy                             	0x07
#define sfbp$k_dma_read_copy_non_dithered       	0x17
#define sfbp$k_dma_read_copy_dithered           	0x37
#define sfbp$k_dma_write_copy                   	0x1F

#define sfbp$K_DEVICE_TYPE 2
#define sfbp$K_AREA_COPY_SIZE 64
#define sfbp$K_SIZE_COLOR_RAM 256
#define sfbp$K_SIZE_CURSOR_RAM 1024
#define sfbp$K_SIZE_OVL_RAM 16

#define START_CRT_CODE            0xA0018000
#define START_CRT_RAMDAC_CODE     0xA001E400
#define START_TEST_CODE           0xA0020000
#define START_DRIVER_RAMDAC_TEST  0xA002E400
#define START_UTIL_CODE           0xA0030000
#define START_DRIVER_DEBUG_CODE   0xA0040000

#define SS$K_NODATA 0
#define SS$K_SUCCESS 1
#define SS$K_BADPARAM 2
#define SS$K_ERROR 0

#define sfbp$k_black     	0x00
#define sfbp$k_white     	0xff
#define sfbp$k_blue     	0x01
               
#define sfbp$k_def_lw_black     0x00000000
#define sfbp$k_def_lw_white     0xffffffff
#define sfbp$k_def_lw_red       0xe0e0e0e0
#define sfbp$k_def_lw_green     0x1c1c1c1c
#define sfbp$k_def_lw_blue      0x03030303 

#if AVANTI || ALCOR || K2 || MTU || TAKARA
 #define sfbp$k_console_blue     0x00000000     /* Black screen */
#else                                                             
 #define sfbp$k_console_blue     0x01010101     /* Blue screen */
#endif                                          

#define sfbp$k_def_lw_yellow    0x1f1f1f1f
#define sfbp$k_def_lw_purple    0xe3e3e3e3
#define sfbp$k_def_lw_gray_0    0x00000000
#define sfbp$k_def_lw_gray_1    0x25252525
#define sfbp$k_def_lw_gray_2    0x49494949
#define sfbp$k_def_lw_gray_3    0x6E6E6E6E
#define sfbp$k_def_lw_gray_4    0x92929292
#define sfbp$k_def_lw_gray_5    0xB7B7B7B7
#define sfbp$k_def_lw_gray_6    0xDBDBDBDB
#define sfbp$k_def_lw_gray_7    0xFFFFFFFF

#define TURBO_HEADER_OFFSET 992
#define TURBO_OBJ_OFFSET 1152
#define TURBO_DEBUG_OBJ_OFFSET 288
#define TURBO_DEBUG_ROM_SIZE 65536
#define TURBO_ROMSIZE_OFFSET 8
#define TURBO_LENGTH_OFFSET 4
#define TURBO_NAME_OFFSET 8
#define TURBO_CODE_OFFSET 20
#define TURBO_OBJECT_MAX 10
#define TURBO_SLOT_MAX 6
#define TURBO_BASE_OLD_ROM 3932160
#define TURBO_DCT_DEVICE_ID 256
#define TRUE 1
#define FALSE 0   
#define LINE_DEFAULT 10

#define MAGIC_NUMBER   0x87654321
#define CONSOLE_OFFSET 0x1000
#define SERVER_OFFSET  0x1000
#define CURSOR_OFFSET  0x0000

#define  LW     0xf
#define  LWMASK 0xf
#define  M1     0xffffffff

enum     stip_mode
        {
        transparent_stipple,
        opaque_stipple,
        transparent_block_stipple,
        transparent_block_fill,
        opaque_fill,
        transparent_fill,
        };
        
typedef  enum stip_mode STIP_MODE;

struct   stip_type 
        {
        int  src_address;
        int  size;
        int  mode;
        int  wait_buffer;
        STIP_MODE stipple;
        int       foreground_value;
        int       background_value;
        int       boolean_op;
        };
        
enum    copy_mode
        {
        simple,
        wicked_fast,
        copy64,
        dma_read_non_dithered,
        dma_read_dithered,
        dma_write,
        };

typedef enum copy_mode COPY_MODE;

struct  copy_type
        {
        int       src_address;
        int       dst_address;
        int       size;
        int       wait_buffer;
        COPY_MODE copy;
        int       system_address;
        int       copymask;
        int       boolean_op;
        int       pixel_shift;
        int       left1;
        int       left2;
        int       right1;
        int       right2;
        };

enum    line_mode
        {
        transparent_line_mode,
        opaque_line_mode,
        cint_nd_line_mode,
        cint_d_line_mode,
        z_transparent_line_mode,
        z_opaque_line_mode,
        z_cint_trans_nd_line_mode,
        z_cint_trans_d_line_mode,
        z_cint_opaq_nd_line_mode,
        z_cint_opaq_d_line_mode,
        cint_nd_trans_line_mode_red,
        cint_nd_opaque_line_mode_red,
        z_cint_trans_nd_line_mode_red,
        z_cint_opaq_nd_line_mode_red,
        };

typedef enum line_mode LINE_MODE;

enum line_method 
        {
        address_data,
        line_address,
        };
        
typedef enum line_method METHOD;

enum  bresh_method
        {
        slow,
        fast
        };

typedef enum bresh_method BEE;

enum  window_method
        {
        Xwindow,
        NTwindow
        };

typedef enum window_method WINDOW;
     
struct  line_type    
        {
        int       xa;
        int       ya;
        int       xb;
        int       yb; 
        int       a1;
        int       a2;
        int       e1;
        int       e2;
        int       e;
        int       len;
        int       dx;
        int       dy;
        LINE_MODE line;
        METHOD    draw_method;
        BEE       speed;
        int       boolean_op;
        int       nogo;
        int       span_line;
        int       verify;
        int       verify_z;
        int       polyline;
        int       wait_buffer;
        int       color_change;
        WINDOW    window_type;
        };

enum  fill_method 
        {
        foreground,
        background,
        };
        
typedef enum fill_method FILL;

struct   color_type
        {
        int     red_value;
        int     green_value;
        int     blue_value;
        int     red_increment;
        int     green_increment;
        int     blue_increment;
        FILL    method;
        int     fg_value;
        int     bg_value;
        };
        
struct  z_type
        {
        int     base_addr;
        int     offset;
        int     value_fraction;
        int     value_whole;
        int     increment_fraction;
        int     increment_whole;
        int     stencil_value;
        int     wmask;
        int     rmask;
        int     stest;
        int     sfail;
        int     ztest;
        int     zpass;
        int     zfail;
        int     replace;
        int     zwidth;
        };        


typedef struct stip_type    STIP;
typedef struct copy_type    SCOPY;
typedef struct line_type    LINE;
typedef struct color_type   COLOR;
typedef struct z_type       Z;
typedef struct scia         SCIA;
typedef struct sfbp_data    SFBP;
typedef struct monitor_data MONITOR;
                       
#define BLOCK_FILL_SIZE     2048
                              

typedef struct   drv_desc         DRV ;
typedef struct   sfbp_ioseg     IOSEG ;
typedef struct   gfx_func_block   GFX ;
typedef struct   bt_func_block     BT ;
typedef struct   sfbp_control CONTROL ;
typedef struct   cp_func_block     CP ;

typedef struct  dispatch_test_type  
        {
        int     subtest;                                        
        int     fault8_data_ptr;
        int     fault32_data_ptr;
        int     fault32z_data_ptr;
        int     error_class;
        int     fault_class;
       	int	error_code;					
	int     (*rtn)();                                       
        char    *text;
        } DISPATCH_TYPE;


enum zop_values {
        geq,
        always,
        never,
        less,
        equal,
        leq,
        greater,
        notequal
        };

enum    fault_types
        {
        nofault,
        fault_asic,
        fault_vram,
        fault_ramdac,
        fault_interrupt,
        fault_checksum,
        };
        
enum sfail_values 
         {
         keep,
         zero,
         replace,
         incr,
         decr,
         invert,
         };
                 
typedef enum zop_values   ZTEST;
typedef enum zop_values   STEST;
typedef enum sfail_values SFAIL;

#define bt$k_color_ram 0

enum depth_values
        {
        packed_8_packed,                        /* 000  */
        packed_8_unpacked,                      /* 001  */       
        unpacked_8_packed,                      /* 002  */
        unpacked_8_unpacked,                    /* 003  */
        source_12_low_dest,                     /* 004  */
        source_12_hi_dest,                      /* 005  */
        source_reserved,                        /* 006  */
        source_24_dest,                         /* 007  */
        };
        
typedef enum depth_values DEPTH;

enum rotate_source_values
        {
        rotate_source_0,
        rotate_source_1,
        rotate_source_2,
        rotate_source_3,
        };

enum rotate_dest_values
        {
        rotate_dest_0,
        rotate_dest_1,
        rotate_dest_2,
        rotate_dest_3,
        };
                
typedef enum rotate_source_values ROTATE_SOURCE;
typedef enum rotate_dest_values   ROTATE_DEST;

struct  visual_type
        {
        DEPTH   depth;
        ROTATE_SOURCE  rotation;
        int     capend;
        int     doz;
        int     line_type;
        int     boolean;
        };

typedef struct visual_type  VISUAL;

enum    int_values 
        {
        end_of_frame,
        end_of_scan,
        dma_error,
        parity_error,
        timer,
        };
        
typedef enum int_values INT_TYPE;

static struct  table_type 
        {
        int  clear_memory;
        int  colormap;
	DISPATCH_TYPE *f;					
	int  fsize;						
	char *name;
        } ;
        
typedef struct table_type TABLE;

struct  table_entry 
        {
        TABLE *ptr;
        int    size;
        int    reserved;
        char   *overlay_name;
        };

typedef struct table_entry TABLE_ENTRY;

enum    line_type_values 
        {
        xlinetype,
        ntlinetype
        };
        
enum    state_type_values
        {
        disabled,
        enabled
        };

enum   col_type_values
        {
        Column256,
        Column128,
        };
        
enum  rasterops
        {
        GXclear,
        GXand,
        GXandReverse,
        GXcopy,
        GXandInverted,
        GXnoop,
        GXxor,
        GXor,
        GXnor,
        GXequiv,
        GXinvert,
        GXorReverse,
        GXcopyInverted,
/*      GXnand,
        GXset,
*/        };

enum color_types
        {
        red,
        green,
        blue,
        red_green,
        red_blue,
        green_blue,
        red_green_blue,
        };
        
typedef enum color_types COLOR_TYPES;

#define  myiabs(x)(((x<0) ? -x : x))

typedef enum rasterops ROPS;

#define sfbp$k_control 0x80000000


/* This is the number of bits used for color fraction    */
/* This is supposed to change to 12 sometime             */

#define color$m_value  12



/* ramdac control bits are in bits 11:8                  */

#ifdef HX


#define sfbp$k_bt_ctl_address_lo_read   0x0   
#define sfbp$k_bt_ctl_address_hi_read   0x4   
#define sfbp$k_bt_ctl_register_read     0x8   
#define sfbp$k_bt_ctl_color_read        0xC   
#define sfbp$k_bt_ctl_address_lo_write  0x0  
#define sfbp$k_bt_ctl_address_hi_write  0x4  
#define sfbp$k_bt_ctl_register_write    0x8  
#define sfbp$k_bt_ctl_color_write       0xC  

#else

/* R/W must be 1 for read       */
#define sfbp$k_bt_ctl_address_lo_read  0x2 
#define sfbp$k_bt_ctl_address_hi_read  0x6 
#define sfbp$k_bt_ctl_register_read    0xA 
#define sfbp$k_bt_ctl_color_read       0xE 

/* R/W must be 0 for write      */
#define sfbp$k_bt_ctl_address_lo_write   0x0
#define sfbp$k_bt_ctl_address_hi_write   0x4
#define sfbp$k_bt_ctl_register_write     0x8
#define sfbp$k_bt_ctl_color_write        0xC

#endif

enum option_types
        {
        OPTION_130,             /* 0    */
        OPTION_119,             /* 1    */
        OPTION_108,             /* 2    */
        OPTION_104,             /* 3    */
        OPTION_92,              /* 4    */
        OPTION_75,              /* 5    */
        OPTION_74,              /* 6    */
        OPTION_69,              /* 7    */
        OPTION_65,              /* 8    */
        OPTION_50,              /* 9    */
        OPTION_40,              /* 10   */
        OPTION_32,              /* 11   */
        OPTION_25,              /* 12   */
        OPTION_135,             /* 13  */
        OPTION_110,             /* 14  */
        };

enum sync_types
        {
        composite_sync,
        vertical_sync,
        };

struct sfbp_control {
    int sfbp$l_copy_reg_0;
    int sfbp$l_copy_reg_1;
    int sfbp$l_copy_reg_2;
    int sfbp$l_copy_reg_3;
    int sfbp$l_copy_reg_4;
    int sfbp$l_copy_reg_5;
    int sfbp$l_copy_reg_6;
    int sfbp$l_copy_reg_7;
    int sfbp$l_foreground;
    int sfbp$l_background;
    int sfbp$l_plane_mask;
    int sfbp$l_pixel_mask;
    int sfbp$l_mode;
    int sfbp$l_boolean_op;
    int sfbp$l_pixel_shift;
    int sfbp$l_line_address;
    int sfbp$l_bresh_reg1;
    int sfbp$l_bresh_reg2;
    int sfbp$l_bresh_reg3;
    int sfbp$l_bresh_continue;
    int sfbp$l_deep;
    int sfbp$l_start;
    int sfbp$l_stencil_mode;
    int sfbp$l_cursor_xy;
    int sfbp$l_cursor_base;
    int sfbp$l_horizontal;
    int sfbp$l_vertical;
    int sfbp$l_refresh_addr;
    int sfbp$l_video_valid;
    int sfbp$l_int_status;
    int sfbp$l_video_status;
    int sfbp$l_video_count;
    int sfbp$l_data_register;
    int sfbp$l_red_increment;
    int sfbp$l_green_increment;
    int sfbp$l_blue_increment;
    int sfbp$l_z_increment_fraction;
    int sfbp$l_z_increment_whole;
    int sfbp$l_dma_base_address;
    int sfbp$l_bresh_width;
    int sfbp$l_z_value_fraction;
    int sfbp$l_z_value_whole;
    int sfbp$l_z_base_addr;
    int sfbp$l_address;
    int sfbp$l_red_value;
    int sfbp$l_green_value;
    int sfbp$l_blue_value;
    int sfbp$l_span_width;
    int sfbp$l_copy64_src;
    int sfbp$l_copy64_dst;
    int sfbp$l_copy64_src_alias;
    int sfbp$l_copy64_dst_alias;
    int sfbp$l_block_color_0;
    int sfbp$l_block_color_1;
    int sfbp$l_block_color_2;
    int sfbp$l_block_color_3;
    int sfbp$l_block_color_4;
    int sfbp$l_block_color_5;
    int sfbp$l_block_color_6;
    int sfbp$l_block_color_7;
    int sfbp$l_slope_nogo_0;
    int sfbp$l_slope_nogo_1;
    int sfbp$l_slope_nogo_2;
    int sfbp$l_slope_nogo_3;
    int sfbp$l_slope_nogo_4;
    int sfbp$l_slope_nogo_5;
    int sfbp$l_slope_nogo_6;
    int sfbp$l_slope_nogo_7;
    int sfbp$l_slope_go_0;
    int sfbp$l_slope_go_1;
    int sfbp$l_slope_go_2;
    int sfbp$l_slope_go_3;
    int sfbp$l_slope_go_4;
    int sfbp$l_slope_go_5;
    int sfbp$l_slope_go_6;
    int sfbp$l_slope_go_7;
    int sfbp$l_eeprom;
    int sfbp$l_rsvd1;     
    int sfbp$l_ramdac_0;
    int sfbp$l_rsvd2;     
    int sfbp$l_ics;
    int sfbp$l_rsvd3;
    int sfbp$l_end_reserve[40];
    int sfbp$l_command_status;
    };

union zfraction_reg {
    struct  {
        unsigned sfbp$v_fraction : 12;
        unsigned sfbp$v_whole    : 20;
        } sfbp$r_zfraction_bits;
    unsigned int sfbp$l_whole;
    } ;

union zvalue_reg {
    struct  {
        unsigned sfbp$v_whole   : 4;
        unsigned sfbp$v_ignore  : 20;
        unsigned sfbp$v_stencil : 8;
        } sfbp$r_zvalue_bits;
    unsigned int sfbp$l_whole;
    } ;

#define RamdacReadAccess  1
#define RamdacWriteAccess 0

enum font_size 
        {
        BigFont,
        SmallFont,
        };

enum video_valid_type 
        {
        NoVideo,                        /* 00   */
        ActiveVideo,                    /* 01   */
        NoVideoToo,                     /* 02   */
        SyncVideo,                      /* 03   */
        };

enum erase_char_type
        {
        NoEraseChar,
        EraseChar,
        };

enum sig_test_types
        {
        OutPutSignature,
        InPutSignature,
        };
        
#define CursorEnable 4                  /* 100  */

struct  tcinfo_type
        {
        int     revision;
        int     clk_period;
        int     slot_size;
        int     io_timeout;
        int     dma_range;
        int     max_dma_burst;
        int     parity;
        int     reserved[4];
        };
        
typedef struct tcinfo_type TCINFO;

struct  sysid_type
        {
        char    hardware_revision;
        char    firmware_revision;
        char    system_type;
        char    processor_type;
        };
        
typedef struct sysid_type SYSID;

union span_reg {
   struct 
       {
       unsigned sfbp$v_dyge0    :1;
       unsigned sfbp$v_dxge0    :1;
       unsigned sfbp$v_dxgedy   :1;
       unsigned sfbp$v_dcol     :5;
       unsigned sfbp$v_drow     :5;
       unsigned sfbp$v_fill     :19;
       } sfbp$r_span_reg_bits;
    unsigned int sfbp$l_whole;
   };

typedef union span_reg SPAN_BITS;

union rom_config_reg {
    struct  {
        unsigned sfbp$v_data    : 8;
        unsigned sfbp$v_switch  : 4;
        unsigned sfbp$v_module  : 4;
        unsigned sfbp$v_ignore  : 16;
        } sfbp$r_rom_config_bits;
    unsigned int sfbp$l_whole;
    } ;

#define RomWriteData        0
#define Dac0ReadData       16
#define Dac1ReadData       24

typedef union rom_config_reg ROM_CONFIG;

#define TCF0_REVISION         2
#define TCF1_REVISION         3

#define SourceVisualPacked    0
#define SourceVisualUnPacked  1
#define SourceVisualLow       2
#define SourceVisualHi        6
#define SourceVisualTrue      3

#define DestVisualPacked      0
#define DestVisualUnPacked    1
#define DestVisual12bit       2
#define DestVisualTrue        3

#define OddPixels             4

struct chip8_type 
        {
        int     pixel;
        int     data_byte;
        char    *text;
        char    *mux_text;
        };
        
typedef struct chip8_type CHIP8;

struct chip32_type 
        {
        int     pixel;
        int     data_byte;
        char    *text;
        char    *mux_text;
        };

typedef struct chip32_type CHIP32;

struct chip32z_type 
        {
        int     pixel;
        int     data_byte;
        char    *text;
        char    *mux_text;
        };

typedef struct chip32z_type CHIP32Z;

#define DMA_BYTES (8192-256)
#define DMA_READ_BYTES DMA_BYTES
#define DMA_WRITE_BYTES (16384-256)


               
