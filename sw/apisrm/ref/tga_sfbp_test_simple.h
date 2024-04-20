
extern 	 int$$assert_test       (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
extern 	 int$$assert_post_test  (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
extern 	 int$$noassert_test  (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
extern 	 int$$scanline_test  (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
extern 	 int$$timer_test     (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );

static   DISPATCH_TYPE int_test_table []=
   {
   1, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                InterruptTestError, fault_interrupt, 0x400,int$$assert_test,      "INT: 1: Poll EOF Interrupt Test",
   2, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                InterruptTestError, fault_interrupt, 0x402,int$$noassert_test,    "INT: 2: Poll EOF No Interrupt Test",
   3, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                InterruptTestError, fault_interrupt, 0x404,int$$timer_test,       "INT: 3: Poll Timer Interrupt Test ",
   4, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                InterruptTestError, fault_interrupt, 0x406,int$$assert_post_test, "INT: 4: PCI Int Signal EOF Interrupt Test",
   5, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                InterruptTestError, fault_interrupt, 0x408,int$$scanline_test,    "INT: 5: Poll EOS Interrupt Test",
   };

extern  int plane$$write_read_test (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
extern  int plane$$pixel_mask_test (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
extern  int plane$$pers_pixel_mask_test (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
static  DISPATCH_TYPE plane_test_table []=
        {          
        1,0,0,0,PlaneMaskTestError, fault_asic, 0x500,plane$$write_read_test,        "PLANE: 1: Plane Write Test ",
        2,0,0,0,PixelMaskTestError, fault_asic, 0x502,plane$$pixel_mask_test,        "PLANE: 2: One Shot Pixel Mask Test ",
        3,0,0,0,PixelMaskTestError, fault_asic, 0x504,plane$$pers_pixel_mask_test,   "PLANE: 3: Persistent Pixel Mask Test ",
        };

extern  int pshift$$pos_shift_test (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
extern  int pshift$$neg_shift_test (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
extern  int pshift$$neg_push_test  (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
static  DISPATCH_TYPE pshift_test_table []=
        {          
        1,0,0,0,PixelShiftTestError,fault_asic, 0x600,pshift$$pos_shift_test,  "PSHIFT: 1: 32  Byte Positive Shift   Test ",
        2,0,0,0,PixelShiftTestError,fault_asic, 0x602,pshift$$neg_shift_test,  "PSHIFT: 2: 32  Byte Negative Shift   Test ",
        3,0,0,0,PixelShiftTestError,fault_asic, 0x604,pshift$$neg_push_test,   "PSHIFT: 3: 128 Unaligned Destination Test ",
        };

extern   int reg$$write_read_test      (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
extern   int reg$$read_slope_nogo_test (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
extern   int reg$$read_mode_copy_test  (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
extern   int reg$$read_mode_b3_test    (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
extern   int reg$$read_mode_addr_test  (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
extern   int reg$$read_span_test       (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
static  DISPATCH_TYPE reg_test_table []=
        {
        1,0,0,0,RegisterTestError,fault_asic, 0x700,reg$$write_read_test,            "REG: 1: Register Write/Read and Access Test",
        2,0,0,0,RegisterTestError,fault_asic, 0x702,reg$$read_slope_nogo_test,       "REG: 2: Register Read Slope Nogo Test",
        3,0,0,0,RegisterTestError,fault_asic, 0x704,reg$$read_span_test,             "REG: 3: Register Read Span Test",
        4,0,0,0,RegisterTestError,fault_asic, 0x706,reg$$read_mode_copy_test,        "REG: 4: Register Mode Copy Bit Test",
        5,0,0,0,RegisterTestError,fault_asic, 0x708,reg$$read_mode_b3_test,          "REG: 5: Register Mode B3 Bit Test",
        6,0,0,0,RegisterTestError,fault_asic, 0x70A,reg$$read_mode_addr_test,        "REG: 6: Register Mode Addr Bit Test",
        };


extern   int vram$$data_path_test (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
extern   int vram$$address_path_test (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
extern   int vram$$write_test (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
extern   int vram$$read_compliment_test (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
extern   int vram$$compare_compliment_test (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
static   DISPATCH_TYPE vram_test_table []=
 {
 1,(int)chip8_table,(int)chip32_table,(int)chip32z_table,
        VramTestError, fault_vram, 0x900,vram$$data_path_test,         "VRAM: 1: Data Path Test",
 2,(int)chip8_table,(int)chip32_table,(int)chip32z_table,
        VramTestError, fault_vram, 0x902,vram$$address_path_test,      "VRAM: 2: Address Path Test ",
 3, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
        VramTestError, fault_vram, 0x904,vram$$write_test,             "VRAM: 3: Area Fill 55555555 ",
 4,(int)chip8_table,(int)chip32_table,(int)chip32z_table,
        VramTestError, fault_vram, 0x906,vram$$read_compliment_test,   "VRAM: 4: GXInvert 55555555",
 5,(int)chip8_table,(int)chip32_table,(int)chip32z_table,
        VramTestError, fault_vram, 0x908,vram$$compare_compliment_test,"VRAM: 5: Check 55555555 Test",
 };

static struct  simple_type 
        {
        int  clear_memory;
        int  colormap;
	DISPATCH_TYPE *f;					
	int  fsize;						
	char *name;
        } simple_tests[]={
	FALSE,FALSE,      reg_test_table,     sizeof (reg_test_table),        "reg",	 
	FALSE,FALSE,      vram_test_table,    sizeof (vram_test_table),       "vram",	 
	FALSE,FALSE,      int_test_table,     sizeof (int_test_table),        "int",	 
	FALSE,FALSE,      plane_test_table,   sizeof (plane_test_table),      "plane", 
	FALSE,FALSE,      pshift_test_table,  sizeof (pshift_test_table),     "pshift",
       };

