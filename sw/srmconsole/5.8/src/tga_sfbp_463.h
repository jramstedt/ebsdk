#define bt463$k_overlay_color           0x200
#define bt463$k_cur_color_0     	0x100
#define bt463$k_cur_color_1     	0x101
#define bt463$k_id_reg          	0x200
#define bt463$k_cmd_reg_0       	0x201
#define bt463$k_cmd_reg_1       	0x202
#define bt463$k_cmd_reg_2       	0x203
#define bt463$k_pixel_mask_reg_p0_p7  	0x205
#define bt463$k_pixel_mask_reg_p8_p15  	0x206
#define bt463$k_pixel_mask_reg_p16_p23 	0x207
#define bt463$k_pixel_mask_reg_p24_p27 	0x208
#define bt463$k_blink_mask_reg_p0_p7  	0x209
#define bt463$k_blink_mask_reg_p8_p15  	0x20A
#define bt463$k_blink_mask_reg_p16_p23 	0x20B
#define bt463$k_blink_mask_reg_p24_p27 	0x20C
#define bt463$k_test_reg        	0x20D
#define bt463$k_input_sig_reg           0x20E
#define bt463$k_output_sig_reg          0x20F
#define bt463$k_revision_reg    	0x220
#define bt463$k_cur_cmd_reg     	0x300
#define bt463$k_window_type_table       0x300
#define bt463$k_color_ram       	0x000


union cmd_463_reg_0 {
    struct  {
        unsigned sfbp$v_cmd_463_mbz1 : 2;
        unsigned sfbp$v_cmd_463_blink_rate : 2;
        unsigned sfbp$v_cmd_463_mbz2 : 2;
        unsigned sfbp$v_cmd_463_mux  : 2;
        } sfbp$r_cmd_463_bits;
    unsigned char sfbp$b_whole;
    } ;

union cmd_463_reg_1 {
    struct  {
        unsigned sfbp$v_cmd_463_ovly  : 2;
        unsigned sfbp$v_cmd_463_unly  : 1;
        unsigned sfbp$v_cmd_463_wtype : 1;
        unsigned sfbp$v_cmd_463_oplane: 1;
        unsigned sfbp$v_cmd_463_cplane: 1;
        unsigned sfbp$v_cmd_463_omap  : 1;
        unsigned sfbp$v_cmd_463_rsvd  : 1;
        } sfbp$r_cmd_463_bits;
    unsigned char sfbp$b_whole;
    } ;

union cmd_463_reg_2 {
    struct  {
        unsigned sfbp$v_cmd_463_test_sel : 1;
        unsigned sfbp$v_cmd_463_clk_cont : 1;
        unsigned sfbp$v_cmd_463_input    : 1;
        unsigned sfbp$v_cmd_463_mbz      : 3;
        unsigned sfbp$v_cmd_463_ped_ena  : 1;
        unsigned sfbp$v_cmd_463_sync_ena : 1;
        } sfbp$r_cmd_463_bits;
    unsigned char sfbp$b_whole;
    } ;


#define sfbp$m_test_463_pixel_mux 7
#define sfbp$m_test_463_result 8
#define sfbp$m_test_463_ref 16
#define sfbp$m_test_463_blue 32
#define sfbp$m_test_463_green 64
#define sfbp$m_test_463_red 128
union test_463_reg {
    struct  {
        unsigned sfbp$v_test_463_pixel_mux : 3;
        unsigned sfbp$v_test_463_result : 1;
        unsigned sfbp$v_test_463_ref : 1;
        unsigned sfbp$v_test_463_blue : 1;
        unsigned sfbp$v_test_463_green : 1;
        unsigned sfbp$v_test_463_red : 1;
        } sfbp$r_test_463_bits;
    unsigned char sfbp$b_whole;
    } ;

#define sfbp$K_SIZE_BT463_COLOR_RAM     0x200
#define sfbp$K_SIZE_BT463_CURSOR_RAM    0x400

union window_463 {
    struct  {
        unsigned sfbp$v_window_463_shift    : 5;
        unsigned sfbp$v_window_463_planes   : 4;
        unsigned sfbp$v_window_463_display  : 3;
        unsigned sfbp$v_window_463_ovl_loc  : 1;
        unsigned sfbp$v_window_463_ovl_mask : 4;
        unsigned sfbp$v_window_463_start    : 6;
        unsigned sfbp$v_window_463_bypass   : 1;
        unsigned sfbp$v_window_463_mbz      : 8;
        } sfbp$r_window_463_bits;
    unsigned int sfbp$b_whole;
    } ;
