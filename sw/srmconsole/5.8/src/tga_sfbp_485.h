#define bt485$k_overlay_color   0x100
#define bt485$k_cur_color_1     0x181
#define bt485$k_cur_color_2     0x181
#define bt485$k_cur_color_3     0x181
#define bt485$k_id_reg          0x200
#define bt485$k_cmd_reg_0       0x201
#define bt485$k_cmd_reg_1       0x202
#define bt485$k_cmd_reg_2       0x203
#define bt485$k_pixel_mask_reg  0x204
#define bt485$k_pixel_blink_reg 0x206
#define bt485$k_ovl_mask_reg    0x208
#define bt485$k_ovl_blink_reg   0x209
#define bt485$k_interleave_reg  0x20A
#define bt485$k_test_reg        0x20B
#define bt485$k_red_sig_reg     0x20C
#define bt485$k_green_sig_reg   0x20D
#define bt485$k_blue_sig_reg    0x20E
#define bt485$k_revision_reg    0x220
#define bt485$k_cur_cmd_reg     0x300
#define bt485$k_cur_x_lo_reg    0x301
#define bt485$k_cur_x_hi_reg    0x302
#define bt485$k_cur_y_lo_reg    0x303
#define bt485$k_cur_y_hi_reg    0x304
#define bt485$k_win_x_lo_reg    0x305
#define bt485$k_win_x_hi_reg    0x306
#define bt485$k_win_y_lo_reg    0x307
#define bt485$k_win_y_hi_reg    0x308
#define bt485$k_win_w_lo_reg    0x309
#define bt485$k_win_w_hi_reg    0x30A
#define bt485$k_win_h_lo_reg    0x30B
#define bt485$k_win_h_hi_reg    0x30C
#define bt485$k_cursor_ram      0x400
#define bt485$k_color_ram       0x000

#define sfbp$m_cmd_485_block_mode 3
#define sfbp$m_cmd_485_blink_rate 12
#define sfbp$m_cmd_485_mbz 16
#define sfbp$m_cmd_485_overlay_ena 32
#define sfbp$m_cmd_485_mux 192
union cmd_485_reg_0 {
    struct  {
        unsigned sfbp$v_cmd_485_block_mode : 2;
        unsigned sfbp$v_cmd_485_blink_rate : 2;
        unsigned sfbp$v_cmd_485_mbz : 1;
        unsigned sfbp$v_cmd_485_overlay_ena : 1;
        unsigned sfbp$v_cmd_485_mux : 2;
        } sfbp$r_cmd_485_bits;
    unsigned char sfbp$b_whole;
    } ;

#define sfbp$m_cmd_485_zoom 15
#define sfbp$m_cmd_485_mbz 16
#define sfbp$m_cmd_485_pan_sel 224
union cmd_485_reg_1 {
    struct  {
        unsigned sfbp$v_cmd_485_zoom : 4;
        unsigned sfbp$v_cmd_485_mbz : 1;
        unsigned sfbp$v_cmd_485_pan_sel : 3;
        } sfbp$r_cmd_485_bits;
    unsigned char sfbp$b_whole;
    } ;

#define sfbp$m_cmd_485_test_sel 1
#define sfbp$m_cmd_485_xcur_sel 2
#define sfbp$m_cmd_485_xovl_sel 4
#define sfbp$m_cmd_485_pll_sel 8
#define sfbp$m_cmd_485_ram_sel 48
#define sfbp$m_cmd_485_ped_ena 64
#define sfbp$m_cmd_485_sync_ena 128
union cmd_485_reg_2 {
    struct  {
        unsigned sfbp$v_cmd_485_test_sel : 1;
        unsigned sfbp$v_cmd_485_xcur_sel : 1;
        unsigned sfbp$v_cmd_485_xovl_sel : 1;
        unsigned sfbp$v_cmd_485_pll_sel : 1;
        unsigned sfbp$v_cmd_485_ram_sel : 2;
        unsigned sfbp$v_cmd_485_ped_ena : 1;
        unsigned sfbp$v_cmd_485_sync_ena : 1;
        } sfbp$r_cmd_485_bits;
    unsigned char sfbp$b_whole;
    } ;

#define sfbp$m_int_485_uly_ena 1
#define sfbp$m_int_485_ovl_ena 2
#define sfbp$m_int_485_pix_sel 28
#define sfbp$m_int_485_int_sel 224
union interleave_485_reg {
    struct  {
        unsigned sfbp$v_int_485_uly_ena : 1;
        unsigned sfbp$v_int_485_ovl_ena : 1;
        unsigned sfbp$v_int_485_pix_sel : 3;
        unsigned sfbp$v_int_485_int_sel : 3;
        } sfbp$r_interleave_485_bits;
    unsigned char sfbp$b_whole;
    } ;

#define sfbp$m_cur_485_blink_enable 1
#define sfbp$m_cur_485_thickness 6
#define sfbp$m_cur_485_format 8
#define sfbp$m_cur_485_ch2_enable 16
#define sfbp$m_cur_485_ch1_enable 32
#define sfbp$m_cur_485_pl0_enable 64
#define sfbp$m_cur_485_pl1_enable 128
union cur_485_cmd_reg {
    struct  {
        unsigned sfbp$v_cur_485_blink_enable : 1;
        unsigned sfbp$v_cur_485_thickness : 2;
        unsigned sfbp$v_cur_485_format : 1;
        unsigned sfbp$v_cur_485_ch2_enable : 1;
        unsigned sfbp$v_cur_485_ch1_enable : 1;
        unsigned sfbp$v_cur_485_pl0_enable : 1;
        unsigned sfbp$v_cur_485_pl1_enable : 1;
        } sfbp$r_cur_cmd_485_bits;
    unsigned char sfbp$b_whole;
    } ;

#define sfbp$m_test_485_pixel_mux 7
#define sfbp$m_test_485_result 8
#define sfbp$m_test_485_ref 16
#define sfbp$m_test_485_blue 32
#define sfbp$m_test_485_green 64
#define sfbp$m_test_485_red 128
union test_485_reg {
    struct  {
        unsigned sfbp$v_test_485_pixel_mux : 3;
        unsigned sfbp$v_test_485_result : 1;
        unsigned sfbp$v_test_485_ref : 1;
        unsigned sfbp$v_test_485_blue : 1;
        unsigned sfbp$v_test_485_green : 1;
        unsigned sfbp$v_test_485_red : 1;
        } sfbp$r_test_485_bits;
    unsigned char sfbp$b_whole;
    } ;

#define sfbp$K_SIZE_BT485_COLOR_RAM     0x100
#define sfbp$K_SIZE_BT485_CURSOR_RAM    0x400

enum ramdac_reg_ops
        {
        sfbp$k_rs_addr_pallete_write,
        sfbp$k_rs_pallete_ram,
        sfbp$k_rs_pixel_mask,
        sfbp$k_rs_addr_pallete_read,
        sfbp$k_rs_addr_cur_color_write,
        sfbp$k_rs_cursor_data,
        sfbp$k_rs_cmd_reg0,
        sfbp$k_rs_addr_cur_color_read,
        sfbp$k_rs_cmd_reg1,
        sfbp$k_rs_cmd_reg2,
        sfbp$k_rs_status,
        sfbp$k_rs_cursor_ram,
        sfbp$k_rs_cursor_x_lo,
        sfbp$k_rs_cursor_x_hi,
        sfbp$k_rs_cursor_y_lo,
        sfbp$k_rs_cursor_y_hi,
        };
                
