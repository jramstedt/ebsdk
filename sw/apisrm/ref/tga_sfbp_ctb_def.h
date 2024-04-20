#define ctb$k_none 0
#define ctb$k_detached_service 1
#define ctb$k_serial_line 2
#define ctb$k_graphics 3
#define ctb$k_ni 192
#define ctb$k_ctb_data_size 12248
struct ctb {
    long ctb$q_cons_type;
    long ctb$q_cons_unit;
    long ctb$q_reserved;
    long ctb$q_length;
    long ctb$q_dev_ipl;
    long ctb$q_txint;
    long ctb$q_rxint;
    long ctb$q_terminal_type;
    long ctb$q_keybd_typ;
    long ctb$q_keybd_trans_ptr;
    long ctb$q_keybd_map_ptr;
    long ctb$q_kbdstate;
    long ctb$q_last_key;
    long ctb$q_usfont_table_ptr;
    long ctb$q_mcsfont_table_ptr;
    long ctb$q_font_width;
    long ctb$q_font_height;
    long ctb$q_mon_width;
    long ctb$q_mon_height;
    long ctb$q_mon_density;
    long ctb$q_planes;
    long ctb$q_cursor_width;
    long ctb$q_cursor_height;
    long ctb$q_number_of_heads;
    long ctb$q_window_up_down;
    long ctb$q_head_offset;
    long ctb$q_reserved1;
    long ctb$q_iostate;
    long ctb$q_listener_state;
    long ctb$q_extended_info;
    long ctb$q_reserved2;
    long ctb$q_server_offset;
    long ctb$q_line_param;
   /*                                                                          
    struct scc_driver_data ctb$r_scc_data;
    struct NI_DRIVER_DATA_BLOCK_TAG ctb$r_ni_data;
    struct scsi_driver_data ctb$r_scsi_data;
    struct cxt_data ctb$r_cxt_data_block;
   */
    } ;

/* Descriptor flags                                                         */
#define m_pass_through 1
#define m_waitfor_input 2
#define m_noecho_input 4
#define m_input_until_terminator 8
#define m_process_controls 16
#define m_xon_detected 32
#define m_xoff_detected 64
#define m_control_c_detected 128
#define m_control_u_detected 256
#define m_control_o_detected 512
#define m_control_r_detected 1024
#define m_string_terminated 2048
union desc_flags {
    unsigned int by_word;
    struct  {
        unsigned pass_through : 1;
        unsigned waitfor_input : 1;
        unsigned noecho_input : 1;
        unsigned input_until_terminator : 1;
        unsigned process_controls : 1;
        unsigned xon_detected : 1;
        unsigned xoff_detected : 1;
        unsigned control_c_detected : 1;
        unsigned control_u_detected : 1;
        unsigned control_o_detected : 1;
        unsigned control_r_detected : 1;
        unsigned string_terminated : 1;
        unsigned by_field$$v_fill_0 : 4;
        } by_field;
    } ;
#define m_run_synch 1
union out_desc_flags {
    unsigned int by_word;
    struct  {
        unsigned run_synch : 1;
        unsigned by_field$$v_fill_1 : 7;
        } by_field;
    } ;
/*                                                                          */
/* Parameter ID values --                                                   */
/*                                                                          */
/*  these values will give you the longword offset into the CTB of the      */
/*  given parameter. The last values should be NVR based values. An entry must */
/*  be entered into the CTB before you add a parameter ID here.             */
/*                                                                          */
#define pid$k_cons_type 0
#define pid$k_cons_unit 2
#define pid$k_terminal_type 8
#define pid$k_keybd_typ 9
#define pid$k_keybd_trans_ptr 10
#define pid$k_keybd_map_ptr 11
#define pid$k_kbdstate 12
#define pid$k_last_key 13
#define pid$k_usfont_table_ptr 14
#define pid$k_mcsfont_table_ptr 15
#define pid$k_font_width 16
#define pid$k_font_height 17
#define pid$k_mon_width 18
#define pid$k_mon_height 19
#define pid$k_mon_density 20
#define pid$k_planes 21
#define pid$k_cursor_width 22
#define pid$k_cursor_height 23
#define pid$k_number_of_heads 24
#define pid$k_head_offset 25
#define pid$k_window_up_down 26
#define pid$k_iostate 28
#define pid$k_listener_state 29
#define pid$k_port_data_ptr 30
/* Beginning of the NVR based values                                        */
#define pid$k_cons_mbx 31
#define pid$k_cons_flags 32
#define pid$k_set_term_int 100
#define pid$k_cursor_column 101
#define pid$k_cursor_row 102
#define pid$k_set_term_ctl 103

typedef struct ctb CTB;
