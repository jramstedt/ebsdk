
int     sfbp$$sfbp_stipple               (SCIA *scia_ptr,SFBP * data_ptr, STIP *stip , COLOR *color, VISUAL *visual,Z *z);
int     stip$$stipple_fg_test            (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);
int     stip$$stipple_bg_test            (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);
int     stip$$stipple_block_test         (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);
int     stip$$stipple_block_fill_test    (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);
int     stip$$stipple_block_stip_align_test   (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);
int     stip$$stipple_block_fill_align_test   (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);
int     stip$$stipple_inc_test           (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);
int     stip$$stipple_vram_test          (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);
int     stip$$transparent_fill_test      (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);
int     stip$$opaque_fill_test           (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);

static  DISPATCH_TYPE stip_test_table []=
        {          
        1, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                StippleTestError, fault_vram, 0xE00,stip$$stipple_fg_test,                "STIP:  1: Stipple Transparent Test",
        2,(int)chip8_table,(int)chip32_table,(int)chip32z_table, 
                StippleTestError, fault_vram, 0xE02,stip$$stipple_bg_test,                "STIP:  2: Stipple Opaque  Test",
        3,(int)chip8_table,(int)chip32_table,(int)chip32z_table, 
                StippleTestError, fault_vram, 0xE04,stip$$stipple_inc_test,               "STIP:  3: Stipple Incremental Test",
        4,(int)chip8_table,(int)chip32_table,(int)chip32z_table, 
                StippleTestError, fault_vram, 0xE06,stip$$stipple_vram_test,              "STIP:  4: Stipple Vram Test ",
        5, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                StippleTestError, fault_vram, 0xE08,stip$$stipple_block_test,             "STIP:  5: Stipple Block Stipple Test",
        6, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                StippleTestError, fault_vram, 0xE0A,stip$$stipple_block_fill_test,        "STIP:  6: Stipple Block Fill Test",
        7, (int)chip8_table,(int)chip32_table,(int)chip32z_table, 
                StippleTestError, fault_vram, 0xE10,stip$$transparent_fill_test,          "STIP:  7: Stipple Transparent Fill Test",
        8, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                StippleTestError, fault_vram, 0xE12,stip$$opaque_fill_test,               "STIP:  8: Stipple Opaque Fill Test",
        9, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                StippleTestError, fault_vram, 0xE0C,stip$$stipple_block_stip_align_test,  "STIP:  9: Stipple Block Stipple Source Alignment Test",
       10, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                StippleTestError, fault_vram, 0xE0E,stip$$stipple_block_fill_align_test,  "STIP: 10: Stipple Block Fill Source Alignment Test",
        };

extern  int bool$$sifb_mode_test (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
extern  int bool$$line_mode_test (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
extern  int bool$$copy_mode_test (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
extern  int bool$$stip_mode_test (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
static DISPATCH_TYPE bool_test_table []=
        {          
        1, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                   BoolTestError, fault_vram, 0xF00,bool$$sifb_mode_test,     "BOOL: 1: Boolean GXcopy Simple  Mode Test",
        2, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                   BoolTestError, fault_vram, 0xF02,bool$$stip_mode_test,     "BOOL: 2: Boolean GXxor  Stipple Mode Test",
        3, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                   BoolTestError, fault_vram, 0xF04,bool$$copy_mode_test,     "BOOL: 3: Boolean GXxor  Copy    Mode Test",
        4, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                   BoolTestError, fault_vram, 0xF06,bool$$line_mode_test,     "BOOL: 4: Boolean GXxor  Line    Mode Test",
       };

int     copy$$row_write_read_test    (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);
int     copy$$src_unaligned_test     (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);
int     copy$$dst_unaligned_test     (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);
int     copy$$src_dst_unaligned_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);
int     copy$$inc_write_read_test    (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);
int     copy$$c64_write_read_test    (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);
int     copy$$dma_write_test         (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);
int     copy$$dma_left_write_test    (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);
int     copy$$dma_read_test          (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);
int     copy$$dma_read_dither_test   (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);

static  DISPATCH_TYPE copy_test_table []=
        {          
        1, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                CopyTestError, fault_vram, 0x1800,copy$$row_write_read_test,         "COPY:  1: Simple Copy Aligned Data  Test",
        2, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                CopyTestError, fault_vram, 0x1808,copy$$inc_write_read_test,         "COPY:  2: Copy Aligned 1 to 32   bytes Test",
        3, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                CopyTestError, fault_vram, 0x1802,copy$$src_unaligned_test,          "COPY:  3: Copy Unaligned Source Test Test",
        4, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                CopyTestError, fault_vram, 0x1804,copy$$dst_unaligned_test,          "COPY:  4: Copy Unaligned Destination Test",
        5, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                CopyTestError, fault_vram, 0x1806,copy$$src_dst_unaligned_test,      "COPY:  5: Copy Unaligned Source and Destination Test",
        6, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                CopyTestError, fault_vram, 0x180A,copy$$c64_write_read_test,         "COPY:  6: Copy 64 Copy Aligned Data  Test",
        7, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                CopyTestError, fault_vram, 0x180C,copy$$dma_read_test,               "COPY:  7: DMA Read  (-->) Copy Test",
        8, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                CopyTestError, fault_vram, 0x180E,copy$$dma_write_test,              "COPY:  8: DMA Write (<--) Copy Test",
        9, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                CopyTestError, fault_vram, 0x1810,copy$$dma_left_write_test,         "COPY:  9: DMA Write (<--) Left Edge Test",
       10, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                CopyTestError, fault_vram, 0x1812,copy$$dma_read_dither_test,        "COPY: 10: DMA Read  (-->) Dither Copy Test",
        };

static struct  ops_type 
        {
        int  clear_memory;
        int  colormap;
	DISPATCH_TYPE *f;					
	int  fsize;						
	char *name;
        } ops_tests[]=
        {
	FALSE,FALSE,      stip_test_table,    sizeof (stip_test_table),       "stip",	
	FALSE,FALSE,      bool_test_table,    sizeof (bool_test_table),       "bool",	
	FALSE,FALSE,      copy_test_table,    sizeof (copy_test_table),       "copy",	 
        };

