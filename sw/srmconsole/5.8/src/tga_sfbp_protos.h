struct  pci_devs_found *CheckForTga (int slot);
int 	sfbp$$bt_459_check_id           (SCIA *scia_ptr, SFBP *data_ptr );
int 	sfbp$$bt_459_check_rev          (SCIA *scia_ptr, SFBP *data_ptr );
int 	sfbp$$bt_459_set_cmd_reg0    	(SCIA *scia_ptr, SFBP *data_ptr );
int 	sfbp$$bt_459_set_cmd_reg1    	(SCIA *scia_ptr, SFBP *data_ptr );
int 	sfbp$$bt_459_set_cmd_reg2    	(SCIA *scia_ptr, SFBP *data_ptr );
int 	sfbp$$bt_459_set_pixel_mask  	(SCIA *scia_ptr, SFBP *data_ptr );
int 	sfbp$$bt_459_set_interleave 	(SCIA *scia_ptr, SFBP *data_ptr );
int 	sfbp$$bt_459_set_pixel_blink 	(SCIA *scia_ptr, SFBP *data_ptr );
int 	sfbp$$bt_459_set_overlay_pixel_mask	(SCIA *scia_ptr, SFBP *data_ptr );
int 	sfbp$$bt_459_set_overlay_blink_mask	(SCIA *scia_ptr, SFBP *data_ptr );
int     sfbp$$bt_459_cursor_ram         (SCIA *scia_ptr, SFBP *data_ptr );
int     sfbp$$bt_459_color              (SCIA *scia_ptr, SFBP *data_ptr );
int     sfbp$$bt_459_overlay            (SCIA *scia_ptr, SFBP *data_ptr );
int 	sfbp$$bt_459_cursor_command     (SCIA *scia_ptr, SFBP *data_ptr );
int     sfbp$$bt_459_set_overlay_blink_mask	(SCIA *scia_ptr, SFBP *data_ptr );
int     sfbp$$bt_459_update_cursor      (SCIA *scia_ptr, SFBP *data_ptr );
int     sfbp$$bt_459_pos_cursor         (SCIA *scia_ptr, SFBP *data_ptr ,int row,int column,int flag);
int     sfbp$$bt_459_init               (SCIA *scia_ptr, SFBP *data_ptr );
int	sfbp$$bt_459_write_reg 		(SCIA *scia_ptr,int address,int data );
int	sfbp$$bt_459_read_reg 		(SCIA *scia_ptr,int address );
int	sfbp$$bt_459_read_sig           (SCIA *scia_ptr,int address ,int sig[]);
int	sfbp$$bt_459_read_lo 		(SCIA *scia_ptr);
int	sfbp$$bt_459_read_hi		(SCIA *scia_ptr);
int	sfbp$$bt_459_load_address       (SCIA *scia_ptr,int address);
int	sfbp$$bt_459_read_color 	(SCIA *scia_ptr,int address,char color[]);
int	sfbp$$bt_459_read_cur_color 	(SCIA *scia_ptr,int address,char color[]);
int	sfbp$$bt_459_read_ovl_color 	(SCIA *scia_ptr,int address,char color[]);
int	sfbp$$bt_459_write_color 	(SCIA *scia_ptr,int address,char color[]);
int	sfbp$$bt_459_write_cur_color 	(SCIA *scia_ptr,int address,char color[]);
int	sfbp$$bt_459_write_ovl_color 	(SCIA *scia_ptr,int address,char color[]);
int	sfbp$$bt_459_write_sig 		(SCIA *scia_ptr,char sig[]);
int     sfbp$$bt_wait_459_interrupt     (SCIA *scia_ptr,SFBP *data_ptr,int load,int pixel,INT_TYPE value ,int input);
int     sfbp$$bt_459_dma_checksum       (SCIA *scia_ptr,SFBP *data_ptr);
int     sfbp$$bt_463_dma_checksum       (SCIA *scia_ptr,SFBP *data_ptr);
int     sfbp$$bt_485_dma_checksum       (SCIA *scia_ptr,SFBP *data_ptr);



int 	sfbp$$bt_463_check_id           (SCIA *scia_ptr, SFBP *data_ptr );
int 	sfbp$$bt_463_check_rev          (SCIA *scia_ptr, SFBP *data_ptr );
int 	sfbp$$bt_463_set_cmd_reg0    	(SCIA *scia_ptr, SFBP *data_ptr );
int 	sfbp$$bt_463_set_cmd_reg1    	(SCIA *scia_ptr, SFBP *data_ptr );
int 	sfbp$$bt_463_set_cmd_reg2    	(SCIA *scia_ptr, SFBP *data_ptr );
int 	sfbp$$bt_463_set_pixel_mask  	(SCIA *scia_ptr, SFBP *data_ptr );
int 	sfbp$$bt_463_set_interleave 	(SCIA *scia_ptr, SFBP *data_ptr );
int 	sfbp$$bt_463_set_pixel_blink 	(SCIA *scia_ptr, SFBP *data_ptr );
int 	sfbp$$bt_463_set_overlay_pixel_mask	(SCIA *scia_ptr, SFBP *data_ptr );
int 	sfbp$$bt_463_set_overlay_blink_mask	(SCIA *scia_ptr, SFBP *data_ptr );
int     sfbp$$bt_463_cursor_ram         (SCIA *scia_ptr, SFBP *data_ptr );
int     sfbp$$bt_463_color              (SCIA *scia_ptr, SFBP *data_ptr );
int     sfbp$$bt_463_window             (SCIA *scia_ptr, SFBP *data_ptr );
int     sfbp$$bt_463_overlay            (SCIA *scia_ptr, SFBP *data_ptr );
int 	sfbp$$bt_463_cursor_command     (SCIA *scia_ptr, SFBP *data_ptr );
int     sfbp$$bt_463_set_overlay_blink_mask	(SCIA *scia_ptr, SFBP *data_ptr );
int     sfbp$$bt_463_update_cursor      (SCIA *scia_ptr, SFBP *data_ptr );
int     sfbp$$bt_463_pos_cursor         (SCIA *scia_ptr, SFBP *data_ptr ,int row,int column,int flag);
int 	sfbp$$bt_463_set_test_reg       (SCIA *scia_ptr, SFBP *data_ptr );
int     sfbp$$bt_463_init               (SCIA *scia_ptr, SFBP *data_ptr );

int	sfbp$$bt_463_write_reg 		(SCIA *scia_ptr,int address,int data );
int	sfbp$$bt_463_read_reg 		(SCIA *scia_ptr,int address );
int	sfbp$$bt_463_read_sig 		(SCIA *scia_ptr,int address ,int sig[]);
int	sfbp$$bt_463_read_lo 		(SCIA *scia_ptr);
int	sfbp$$bt_463_read_hi		(SCIA *scia_ptr);
int	sfbp$$bt_463_load_address       (SCIA *scia_ptr,int address);
int	sfbp$$bt_463_read_color 	(SCIA *scia_ptr,int address,char color[]);
int	sfbp$$bt_463_read_cur_color 	(SCIA *scia_ptr,int address,char color[]);
int	sfbp$$bt_463_read_ovl_color 	(SCIA *scia_ptr,int address,char color[]);
int	sfbp$$bt_463_write_color 	(SCIA *scia_ptr,int address,char color[]);
int	sfbp$$bt_463_write_cur_color 	(SCIA *scia_ptr,int address,char color[]);
int	sfbp$$bt_463_write_ovl_color 	(SCIA *scia_ptr,int address,char color[]);
int	sfbp$$bt_463_write_sig 		(SCIA *scia_ptr,char sig[]);
int     sfbp$$bt_wait_463_interrupt  	(SCIA *scia_ptr,SFBP *data_ptr,int load,int pixel,INT_TYPE value,int input );


int 	sfbp$$bt_485_check_id           (SCIA *scia_ptr, SFBP *data_ptr );
int 	sfbp$$bt_485_check_rev          (SCIA *scia_ptr, SFBP *data_ptr );
int 	sfbp$$bt_485_set_cmd_reg0    	(SCIA *scia_ptr, SFBP *data_ptr );
int 	sfbp$$bt_485_set_cmd_reg1    	(SCIA *scia_ptr, SFBP *data_ptr );
int 	sfbp$$bt_485_set_cmd_reg2    	(SCIA *scia_ptr, SFBP *data_ptr );
int 	sfbp$$bt_485_set_cmd_reg3    	(SCIA *scia_ptr, SFBP *data_ptr );
int 	sfbp$$bt_485_set_pixel_mask  	(SCIA *scia_ptr, SFBP *data_ptr );
int 	sfbp$$bt_485_set_interleave 	(SCIA *scia_ptr, SFBP *data_ptr );
int 	sfbp$$bt_485_set_pixel_blink 	(SCIA *scia_ptr, SFBP *data_ptr );
int 	sfbp$$bt_485_set_overlay_pixel_mask	(SCIA *scia_ptr, SFBP *data_ptr );
int 	sfbp$$bt_485_set_overlay_blink_mask	(SCIA *scia_ptr, SFBP *data_ptr );
int     sfbp$$bt_485_cursor_ram         (SCIA *scia_ptr, SFBP *data_ptr );
int     sfbp$$bt_485_color              (SCIA *scia_ptr, SFBP *data_ptr );
int     sfbp$$bt_485_overlay            (SCIA *scia_ptr, SFBP *data_ptr );
int 	sfbp$$bt_485_cursor_command     (SCIA *scia_ptr, SFBP *data_ptr );
int     sfbp$$bt_485_set_overlay_blink_mask	(SCIA *scia_ptr, SFBP *data_ptr );
int     sfbp$$bt_485_update_cursor      (SCIA *scia_ptr, SFBP *data_ptr );
int     sfbp$$bt_485_pos_cursor         (SCIA *scia_ptr, SFBP *data_ptr ,int row,int column,int flag);
int     sfbp$$bt_485_init               (SCIA *scia_ptr, SFBP *data_ptr );
int	sfbp$$bt_485_write_reg 		(SCIA *scia_ptr,int address,int data );
int	sfbp$$bt_485_read_reg 		(SCIA *scia_ptr,int address );
int	sfbp$$bt_485_read_sig 		(SCIA *scia_ptr,int address,int sig[] );
int	sfbp$$bt_485_read_lo 		(SCIA *scia_ptr);
int	sfbp$$bt_485_read_hi		(SCIA *scia_ptr);
int	sfbp$$bt_485_load_address       (SCIA *scia_ptr,int address);
int	sfbp$$bt_485_read_color 	(SCIA *scia_ptr,int address,char color[]);
int	sfbp$$bt_485_read_cur_color 	(SCIA *scia_ptr,int address,char color[]);
int	sfbp$$bt_485_read_ovl_color 	(SCIA *scia_ptr,int address,char color[]);
int	sfbp$$bt_485_write_color 	(SCIA *scia_ptr,int address,char color[]);
int	sfbp$$bt_485_write_cur_color 	(SCIA *scia_ptr,int address,char color[]);
int	sfbp$$bt_485_write_ovl_color 	(SCIA *scia_ptr,int address,char color[]);
int	sfbp$$bt_485_write_sig 		(SCIA *scia_ptr,char sig[]);
int     sfbp$$bt_wait_485_interrupt     (SCIA *scia_ptr,SFBP *data_ptr,int load,int pixel,INT_TYPE value ,int input );





int	sfbp$$sfbp_write_reg            (SCIA *scia_ptr,int address,int data,int mask );
int	sfbp$$sfbp_read_reg             (SCIA *scia_ptr,int address );
int	sfbp$$sfbp_write_vram 		(SCIA *scia_ptr,int address,int data,int mask );
int	sfbp$$sfbp_read_vram		(SCIA *scia_ptr,int address );
int	sfbp$$sfbp_write_rom 		(SCIA *scia_ptr,int address,int data,int mask );
int	sfbp$$sfbp_read_rom		(SCIA *scia_ptr,int address );
int	sfbp$$sfbp_write_pci_config     (SCIA *scia_ptr,int address,int data,int mask );
int	sfbp$$sfbp_read_pci_config      (SCIA *scia_ptr,int address );
int     sfbp$$disable_interrupt         (SCIA *scia_ptr,SFBP *data_ptr,INT_TYPE value);
int     sfbp$$enable_interrupt 		(SCIA *scia_ptr,SFBP *data_ptr,INT_TYPE value);
int     sfbp$$clear_int_req 		(SCIA *scia_ptr, SFBP *data_ptr,INT_TYPE value);
int     sfbp$$wait_retrace 		(SCIA *scia_ptr, SFBP *data_ptr ,int load,int pixel,INT_TYPE value,int input);
int     sfbp$$test_int                  (SCIA *scia_ptr, SFBP *data_ptr ,INT_TYPE value);

int     sfbp$$get_expect 		(int *src,int *dst,int mode);
int     sfbp$$sfbp_error_handler        (SCIA *scia_ptr,SFBP *data_ptr,int slot,char *string,int param[],int class,int fault,
                                                        CHIP8 fault8_data[],CHIP32 fault32_data[],CHIP32Z fault32z_data[]);
int     sfbp$$sfbp_fault_handler        (SCIA *scia_ptr,SFBP *data_ptr,int slot,char *string,int param[],int class,int fault,
                                                        CHIP8 fault8_data[],CHIP32 fault32_data[],CHIP32Z fault32z_data[]);
int     sfbp$$sfbp_fault_vram           (SCIA *scia_ptr,SFBP *data_ptr,int slot,char *string,int param[],int class,int fault,
                                                        CHIP8 fault8_data[],CHIP32 fault32_data[],CHIP32Z fault32z_data[]);
int     sfbp$$sfbp_fault_ramdac         (SCIA *scia_ptr,SFBP *data_ptr,int slot,char *string,int param[],int class,int fault,
                                                        CHIP8 fault8_data[],CHIP32 fault32_data[],CHIP32Z fault32z_data[]);
int     sfbp$$sfbp_fault_asic           (SCIA *scia_ptr,SFBP *data_ptr,int slot,char *string,int param[],int class,int fault,
                                                        CHIP8 fault8_data[],CHIP32 fault32_data[],CHIP32Z fault32z_data[]);
int     sfbp$$sfbp_fault_interrupt      (SCIA *scia_ptr,SFBP *data_ptr,int slot,char *string,int param[],int class,int fault,
                                                        CHIP8 fault8_data[],CHIP32 fault32_data[],CHIP32Z fault32z_data[]);

int	sfbp$$bus_write                 ();
int	sfbp$$bus_read                  ();

int	sfbp$init_driver( struct sfbp_ioseg *sfbp_ioseg_ptr,SFBP *data_ptr,
                        struct function_block_desc *cp_func_ptr_desc,SCIA *scia_ptr);

int	sfbp$init_gfx_driver		(SCIA *scia_ptr,SFBP *data_ptr);
int	sfbp$init_bt_459_driver		(SCIA *scia_ptr,CCV *ccv);
int	sfbp$init_bt_463_driver		(SCIA *scia_ptr,CCV *ccv);
int	sfbp$init_bt_485_driver		(SCIA *scia_ptr,CCV *ccv);
int	sfbp$reset_output		(SCIA *scia_ptr,SFBP *data_ptr);
int	sfbp$init_output		(SCIA *scia_ptr,SFBP *data_ptr);
int	sfbp$$init_param 		(SCIA *scia_ptr,SFBP *data_ptr);
int	sfbp$reset_input 		(SCIA *scia_ptr,SFBP *data_ptr);
int	sfbp$init_input 		(SCIA *scia_ptr,SFBP *data_ptr);
/*int     sfbp$putchar    		(SCIA *scia_ptr,struct buffer_desc *buffer_desc_ptr );*/
sfbp$putchar  (struct CH_STATE *chs,char ch,unsigned char attribute,
    	       						int xpos,int ypos);
int     sfbp$getchar	        	(SCIA *scia_ptr,struct buffer_desc *buffer_desc_ptr);
int	sfbp$$test_sfbp 		(SCIA *scia_ptr,SFBP *data_ptr);

int	sfbp$$display_character	  (SCIA *scia_ptr,int char_index,char *font_ptr);
int	sfbp$$display_a_character (SCIA *scia_ptr,int char_index,char *font_ptr,int erase, int attributes);
int	sfbp$$print_scanline	  ( SCIA *scia_ptr,SFBP *data_ptr,char *font_ptr,char *vram_ptr);
int     calc$$screen_bytes 	  ( SCIA *scia_ptr, SFBP *data_ptr );
char    *calc$$character_position ( SCIA *scia_ptr, SFBP *data_ptr );
int	calc$$cell_font_size 	  ( SCIA *scia_ptr, SFBP *data_ptr );
int	sfbp$$scroll 	          ( SCIA *scia_ptr, SFBP *data_ptr );
int	sfbp$$zero_line 	  ( SCIA *scia_ptr, SFBP *data_ptr , int count );
int     sfbp$$copy_video_memory   ( SCIA *scia_ptr, SFBP *data_ptr ,char *source,char *dest,unsigned int size);
int     sfbp$$fill_video_memory   (SCIA *scia_ptr,SFBP *data_ptr,char *source,int pattern,int size);
int	sfbp$$move_cursor 	  ( SCIA *scia_ptr, SFBP *data_ptr ,int direction);
int	sfbp$$init_cursor 	  ( SCIA *scia_ptr, SFBP *data_ptr );
int	sfbp$$delete_character 	  ( SCIA *scia_ptr, SFBP *data_ptr );
int	sfbp$$cr_return 	  ( SCIA *scia_ptr, SFBP *data_ptr );
int	sfbp$$line_feed 	  ( SCIA *scia_ptr, SFBP *data_ptr );
int     sfbp$$configure_video 	  ( SCIA *scia_ptr, SFBP *data_ptr );
int	sfbp$$get_module_type	  ( SCIA *scia_ptr, SFBP *data_ptr);
int	sfbp$$get_vram_type	  ( SCIA *scia_ptr, SFBP *data_ptr);
int	sfbp$$connect_ramdac_type ( SCIA *scia_ptr, SFBP *data_ptr);
int     copy_crystal_data 	  (SFBP *data_ptr,int frequency,int oscill);
int	sfbp$$erase_memory 	  ( SCIA *scia_ptr, SFBP *data_ptr );
int     sfbp$$upbox 	          ( SCIA *scia_ptr, SFBP *data_ptr );
int     sfbp$$downbox   	  ( SCIA *scia_ptr, SFBP *data_ptr );
int	sfbp$$clear_vram 	  ( SCIA *scia_ptr, SFBP *data_ptr );
int	sfbp$$fill_vram 	  (SCIA *scia_ptr,SFBP *data_ptr,char *src,int fill_character,int size);
int	sfbp$$scroll_row 	  ( SCIA *scia_ptr, SFBP *data_ptr );
int	sfbp$$update_cursor 	  (SCIA *scia_ptr,SFBP *data_ptr);

int     sfbp$$video_reset 		(SCIA *scia_ptr,SFBP *data_ptr);
int	sfbp$$vt_set_horizontal 	(SCIA *scia_ptr,SFBP *data_ptr);
int	sfbp$$vt_set_vertical 		(SCIA *scia_ptr,SFBP *data_ptr);
int	sfbp$$vt_set_refresh_base 	(SCIA *scia_ptr,SFBP *data_ptr);
int	sfbp$$vt_set_mode 		(SCIA *scia_ptr,SFBP *data_ptr);
int	sfbp$$vt_set_planemask 		(SCIA *scia_ptr,SFBP *data_ptr);
int	sfbp$$vt_set_valid 		(SCIA *scia_ptr,SFBP *data_ptr);
int	sfbp$$vt_set_deep 		(SCIA *scia_ptr,SFBP *data_ptr);
int	sfbp$$vt_set_fg 		(SCIA *scia_ptr,SFBP *data_ptr);
int	sfbp$$vt_set_bg 		(SCIA *scia_ptr,SFBP *data_ptr);
int	sfbp$$vt_set_boolean 		(SCIA *scia_ptr,SFBP *data_ptr);
int	sfbp$$init_refresh 		(SCIA *scia_ptr,SFBP *data_ptr);
int	sfbp$$vt_set_bwidth 		(SCIA *scia_ptr,SFBP *data_ptr);


int     sfbp$$sfbp_aligned_copy   (SCIA *scia_ptr,SFBP *data_ptr,SCOPY *copy, VISUAL *visual,Z *z );
int     sfbp$$sfbp_console_aligned_copy   (SCIA *scia_ptr,SFBP *data_ptr,SCOPY *copy, VISUAL *visual,Z *z );
int     sfbp$$sfbp_unaligned_copy (SCIA *scia_ptr,SFBP *data_ptr,SCOPY *copy, VISUAL *visual,Z *z );
int     sfbp$$sfbp_draw_line (SCIA *scia_ptr,SFBP *data_ptr,LINE *line ,COLOR *color, VISUAL *visual,Z *z);
int     sfbp$$sfbp_draw_point (SCIA *scia_ptr,SFBP *data_ptr,LINE *line ,COLOR *color, VISUAL *visual,Z *z);
int     sfbp$$sfbp_sweep_y (SCIA *scia_ptr,SFBP *data_ptr,LINE *line,COLOR *color, VISUAL *visual, Z *z);
int     sfbp$$sfbp_sweep_x (SCIA *scia_ptr,SFBP *data_ptr,LINE *line,COLOR *color, VISUAL *visual, Z *z);
int     sfbp$$verify_draw_line (SCIA *scia_ptr,SFBP *data_ptr,LINE *line ,COLOR *color,
                         VISUAL *visual,Z *z,int param[], int DROM[32][32]);
int     sfbp$$verify_sweep_y (SCIA *scia_ptr,SFBP *data_ptr,LINE *line,COLOR *color, VISUAL *visual, Z *z,int param[]);
int     sfbp$$verify_sweep_x (SCIA *scia_ptr,SFBP *data_ptr,LINE *line,COLOR *color, VISUAL *visual, Z *z,int param[]);
int     sfbp$$calc_slow_line (SCIA *scia_ptr,SFBP *data_ptr,LINE *line,COLOR *color, VISUAL *visual, Z *z);
int     sfbp$$calc_fast_line (SCIA *scia_ptr,SFBP *data_ptr,LINE *line,COLOR *color, VISUAL *visual, Z *z);
int     sfbp$$mode_setup (SCIA *scia_ptr,SFBP *data_ptr,LINE *line,COLOR *color, VISUAL *visual, Z *z);

int     sfbp$$sfbp_wait_csr_ready (SCIA *scia_ptr,SFBP * data_ptr);
int     sfbp$$sfbp_stipple (SCIA *scia_ptr,SFBP * data_ptr, STIP *stip , COLOR *color, VISUAL *visual,Z *z);
int     sfbp$$sfbp_console_stipple (SCIA *scia_ptr,SFBP * data_ptr, STIP *stip , COLOR *color, VISUAL *visual,Z *z);
int     sfbp$$load_block_color (SCIA *scia_ptr,SFBP *data_ptr,int value );
int     sfbp$$load_visual (SCIA *scia_ptr,SFBP *data_ptr,VISUAL *visual , Z *z);
int     sfbp$$load_ztype  (SCIA *scia_ptr,SFBP *data_ptr,VISUAL *visual , Z *z);
int     sfbp$$load_stype  (SCIA *scia_ptr,SFBP *data_ptr,VISUAL *visual , Z *z);
int     sfbp$$sfbp_cint_setup (SCIA *scia_ptr,SFBP *data_ptr,LINE *line,COLOR *color, VISUAL *visual, Z *z);
int     sfbp$$sfbp_zbuff_setup (SCIA *scia_ptr,SFBP *data_ptr,LINE *line,COLOR *color, VISUAL *visual, Z *z);


int     sfbp$$bt_459_alloc_color (SCIA *scia_ptr,SFBP *data_ptr,int *color);
int     sfbp$$bt_459_free_all_color (SCIA *scia_ptr,SFBP *data_ptr);
int     sfbp$$bt_459_free_color (SCIA *scia_ptr,SFBP *data_ptr,int color);
int     sfbp$$bt_459_load_def_color (SCIA *scia_ptr,SFBP *data_ptr);
int     sfbp$$bt_459_load_888_color (SCIA *scia_ptr,SFBP *data_ptr,int color[]);
int     sfbp$$bt_459_load_332_color (SCIA *scia_ptr,SFBP *data_ptr);
int     sfbp$$bt_459_load_444_color (SCIA *scia_ptr,SFBP *data_ptr);

int     sfbp$$bt_463_alloc_color (SCIA *scia_ptr,SFBP *data_ptr,int *color);
int     sfbp$$bt_463_free_all_color (SCIA *scia_ptr,SFBP *data_ptr);
int     sfbp$$bt_463_free_color (SCIA *scia_ptr,SFBP *data_ptr,int color);
int     sfbp$$bt_463_load_def_color (SCIA *scia_ptr,SFBP *data_ptr);
int     sfbp$$bt_463_load_888_color (SCIA *scia_ptr,SFBP *data_ptr,int color []);
int     sfbp$$bt_463_load_332_color (SCIA *scia_ptr,SFBP *data_ptr);
int     sfbp$$bt_463_load_444_color (SCIA *scia_ptr,SFBP *data_ptr);

int     sfbp$$bt_485_alloc_color (SCIA *scia_ptr,SFBP *data_ptr,int *color);
int     sfbp$$bt_485_free_all_color (SCIA *scia_ptr,SFBP *data_ptr);
int     sfbp$$bt_485_free_color (SCIA *scia_ptr,SFBP *data_ptr,int color);
int     sfbp$$bt_485_load_def_color (SCIA *scia_ptr,SFBP *data_ptr);
int     sfbp$$bt_485_load_888_color (SCIA *scia_ptr,SFBP *data_ptr,int color[]);
int     sfbp$$bt_485_load_332_color (SCIA *scia_ptr,SFBP *data_ptr);
int     sfbp$$bt_485_load_444_color (SCIA *scia_ptr,SFBP *data_ptr);

int     putc ( SCIA *scia_ptr, SFBP *data_ptr,char c, CCV *ccv);
int     driver_initc (SCIA *scia_ptr,SFBP *data_ptr,IOSEG *sfbp_ioseg_ptr,
                struct function_block_desc *cp_function_ptr,
                int argc,char **argv, int slot, CCV *ccv);
int     init_scia_table  (SCIA *scia_ptr, SFBP *data_ptr, IOSEG *sfbp_ioseg_ptr, struct function_block_desc *cp_function, int slot , CCV *ccv);
int     init_data_table  (SFBP *data_ptr,int slot,CCV *ccv );
int     init_ioseg_table (SFBP *data_ptr,IOSEG *sfbp_ioseg_ptr,int slot,CCV *ccv );

int     load$$rom_object (SCIA *scia_ptr,SFBP *data_ptr,unsigned char *myname,unsigned char *dst,int load_flag,int maxlength);
int     load$$rom_data ( unsigned char *dst,unsigned char *src,int size,int stride);
int     load$$fixup (SCIA *scia_ptr,SFBP *data_ptr,int load_address, int fixup_address, int length );


int     cnfg (SCIA *scia_ptr,SFBP *data_ptr,int argc, char **argv, int slot, CCV *ccv);
int     test (SCIA *scia_ptr,SFBP *data_ptr,int argc, char **argv, int slot, CCV *ccv);
int     init (SCIA *scia_ptr,SFBP *data_ptr,int argc, char **argv, int slot, CCV *ccv);


int     dispatch_simple  ( SCIA *scia_ptr, SFBP *data_ptr,int argc, char **argv, int slot, CCV *ccv);
#if TGA8MMIN
#else
int     dispatch_ops     ( SCIA *scia_ptr, SFBP *data_ptr,int argc, char **argv, int slot, CCV *ccv);
int     dispatch_line    ( SCIA *scia_ptr, SFBP *data_ptr,int argc, char **argv, int slot, CCV *ccv);
#endif
int     dispatch_color   ( SCIA *scia_ptr, SFBP *data_ptr,int argc, char **argv, int slot, CCV *ccv);
int     dispatch_z       ( SCIA *scia_ptr, SFBP *data_ptr,int argc, char **argv, int slot, CCV *ccv);
int     dispatch_stencil ( SCIA *scia_ptr, SFBP *data_ptr,int argc, char **argv, int slot, CCV *ccv);
#if TGA8MMIN
#else
int     dispatch_ramdac  ( SCIA *scia_ptr, SFBP *data_ptr,int argc, char **argv, int slot, CCV *ccv);
#endif
int     dispatch_ray     ( SCIA *scia_ptr, SFBP *data_ptr,int argc, char **argv, int slot, CCV *ccv);
int     dispatch_flash   ( SCIA *scia_ptr, SFBP *data_ptr,int argc, char **argv, int slot, CCV *ccv);
int     dispatch_util    ( SCIA *scia_ptr, SFBP *data_ptr,int argc, char **argv, int slot, CCV *ccv);
int     dispatch_app     ( SCIA *scia_ptr, SFBP *data_ptr,int argc, char **argv, int slot, CCV *ccv);
int     dispatch_patt    ( SCIA *scia_ptr, SFBP *data_ptr,int argc, char **argv, int slot, CCV *ccv);
int     dispatch_pack    ( SCIA *scia_ptr, SFBP *data_ptr,int argc, char **argv, int slot, CCV *ccv);
int     dispatch_span    ( SCIA *scia_ptr, SFBP *data_ptr,int argc, char **argv, int slot, CCV *ccv);

int     parse_command_line (SCIA *scia_ptr,SFBP *data_ptr,int argc,char **argv,CCV *ccv,int slot);
int     parse_check        (char **argv,int argc,char *param,char **fptr);
int     ParseFlags         (SFBP *data_ptr,int  argc,char **argv);
int     test_dispatch      (SCIA *scia_ptr,SFBP *data_ptr, int argc, char **argv, 
                                int slot, CCV *ccv, TABLE_ENTRY *test_ptr,int entry_size);
int     subtest_dispatch   (SCIA *scia_ptr,SFBP *data_ptr,int argc,char **argv,int slot,CCV *ccv,
                                DISPATCH_TYPE test_table[],int type_size,int table_size,int colormap,int clear_memory );

int     sfbp$$signature_mode_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr,
                SFBP * data_ptr, int param [],struct signature_type *signature);

int     test_flash (int argc,char **argv,CCV *ccv,
                struct scia *scia_ptr,struct sfbp_data *data_ptr,int param[]);

int     flash$$help       (SCIA *scia_ptr,SFBP *data_ptr);
int     flash$$resetrom   (SCIA *scia_ptr,SFBP *data_ptr);
int     flash$$intelid    (SCIA *scia_ptr,SFBP *data_ptr);
int     flash$$program    (SCIA *scia_ptr,SFBP *data_ptr,int mode,unsigned char *checksum);
int     flash$$do_program (int address,int data);
int     flash$$erase      (SCIA *scia_ptr,SFBP *data_ptr);
int     flash$$do_erase   (int address);
int     flash$$readrom    (SCIA *scia_ptr,SFBP *data_ptr);
int     wait_ten_usec     ();
long    get_slot_address  (CCV *ccv,int slot);
int     sfbp$$tga_reset   (SCIA *scia_ptr,SFBP *data_ptr);
int     sfbp$$rand        (int  *seed,int low,int high);


int     copy$$dma_scroll (SCIA *scia_ptr,SFBP *data_ptr,SCOPY *copy, VISUAL
*visual,Z *z,int byteCount);
