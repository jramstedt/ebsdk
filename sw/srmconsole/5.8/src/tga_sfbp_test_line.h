
int     line$$xa_lt_xb_m_lt_1_test (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
int     line$$xa_gt_xb_m_lt_1_test (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
int     line$$xa_lt_xb_m_gt_1_test (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
int     line$$xa_gt_xb_m_gt_1_test (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
int     line$$xa_lt_xb_m_lt_m1_test (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
int     line$$xa_gt_xb_m_lt_m1_test (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
int     line$$xa_lt_xb_m_gt_m1_test (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
int     line$$xa_gt_xb_m_gt_m1_test (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
int     line$$incremental_test      (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
int     line$$h_cap_test            (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
int     line$$h_span_test           (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
int     line$$h_align_test          (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
int     line$$h_address_data_test   (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
int     line$$output_signature_mode_test  (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
int     line$$input_signature_mode_test   (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );

static  DISPATCH_TYPE dispatch_line_test_table []=
   {
   1, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
        LineTestError, fault_vram, 0x100,line$$xa_lt_xb_m_lt_1_test,      "LINE:  1: M < 1  xa < xb  ",
   2, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
        LineTestError, fault_vram, 0x102,line$$xa_gt_xb_m_lt_1_test,      "LINE:  2: M < 1  xa > xb  ",
   3, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
        LineTestError, fault_vram, 0x104,line$$xa_lt_xb_m_gt_1_test,      "LINE:  3: M > 1  xa < xb  ",
   4, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
        LineTestError, fault_vram, 0x106,line$$xa_gt_xb_m_gt_1_test,      "LINE:  4: M > 1  xa > xb  ",
   5, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
        LineTestError, fault_vram, 0x108,line$$xa_lt_xb_m_lt_m1_test,     "LINE:  5: M < -1 xa < xb  ",
   6, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
        LineTestError, fault_vram, 0x10A,line$$xa_gt_xb_m_lt_m1_test,     "LINE:  6: M < -1 xa > xb  ",
   7, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
        LineTestError, fault_vram, 0x10C,line$$xa_lt_xb_m_gt_m1_test,     "LINE:  7: M > -1 xa < xb  ",
   8, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
        LineTestError, fault_vram, 0x10E,line$$xa_gt_xb_m_gt_m1_test,     "LINE:  8: M > -1 xa > xb  ",
   9, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
        LineTestError, fault_vram, 0x110,line$$h_cap_test,                "LINE:  9: Capend Test ",
  10, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
        LineTestError, fault_vram, 0x112,line$$h_span_test,               "LINE: 10: Span Alias ",
  11, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
        LineTestError, fault_vram, 0x114,line$$h_address_data_test,       "LINE: 11: Line Address/Data Slow + Fast ",
  12, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
        LineTestError, fault_vram, 0x116,line$$h_align_test,              "LINE: 12: Line Address Register ",
  13, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
        LineTestError, fault_vram, 0x11A,line$$output_signature_mode_test,"LINE: 13: Output Signature Test ",
  14, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
        LineTestError, fault_vram, 0x11C,line$$input_signature_mode_test, "LINE: 14: Input  Signature Test ",
  };

static struct  dispatch_line_type 
        {
        int  clear_memory;
        int  colormap;
	DISPATCH_TYPE *f;					
	int  fsize;						
	char *name;
        } dispatch_line_tests[]=
        {
	TRUE,TRUE,dispatch_line_test_table,sizeof (dispatch_line_test_table), "line", 
        };

