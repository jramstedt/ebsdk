int     stip$$stipple_8_pack_vis_test    (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);
int     stip$$stipple_8_unpack_rot_test   (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);
int     stip$$stipple_8_unpack_vis_test  (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);
int     stip$$stipple_12_lo_vis_test     (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);
int     stip$$stipple_12_hi_vis_test     (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);

int     copy$$pack_8_unpack_test     (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);
int     copy$$unpack_8_pack_test     (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);
int     copy$$unpack_8_unpack_test   (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);
int     copy$$pack_8_dst_rot_test    (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);
int     copy$$src_lo_12_test         (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);
int     copy$$src_hi_12_test         (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);

int     copy$$dmar_pack_8_unpack_test    (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);
int     copy$$dmar_pack_8_pack_test      (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);
int     copy$$dmar_pack_8_unpack_rot_test(int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);

int     copy$$dmaw_unpack_8_pack_test    (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);
int     copy$$dmaw_pack_8_pack_test      (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);

int     line$$line_8_pack_vis_test     (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);
int     line$$line_8_unpack_rot_test   (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);
int     line$$line_8_unpack_vis_test   (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);
int     line$$line_12_packlo_vis_test  (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);
int     line$$line_12_packhi_vis_test  (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);

static  DISPATCH_TYPE pack_test_table []=
        {          
        1, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                PackTestError, fault_vram, 0x1900,stip$$stipple_8_pack_vis_test,        "PACK:  1: Stipple 8  bit   packed visual test      ",
        2, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                PackTestError, fault_vram, 0x1902,stip$$stipple_8_unpack_vis_test,      "PACK:  2: Stipple 8  bit unpacked visual test      ",
        3, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                PackTestError, fault_vram, 0x1904,stip$$stipple_8_unpack_rot_test,      "PACK:  3: Stipple 8  bit unpacked rotate test      ",
        4, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                PackTestError, fault_vram, 0x1906,stip$$stipple_12_lo_vis_test,         "PACK:  4: Stipple 12 bit low visual test           ",
        5, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                PackTestError, fault_vram, 0x1908,stip$$stipple_12_hi_vis_test,         "PACK:  5: Stipple 12 bit hi visual test            ",
        6, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                PackTestError, fault_vram, 0x190A,copy$$pack_8_unpack_test,             "PACK:  6: Copy 8  bit packed to 8 bit unpacked     ",
        7, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                PackTestError, fault_vram, 0x190C,copy$$unpack_8_pack_test,             "PACK:  7: Copy 8  bit unpacked to 8 bit packed     ",
        8, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                PackTestError, fault_vram, 0x190E,copy$$unpack_8_unpack_test,           "PACK:  8: Copy 8  bit unpacked to 8 bit unpacked   ",
        9, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                PackTestError, fault_vram, 0x1912,copy$$pack_8_dst_rot_test,            "PACK:  9: Copy 8  bit unpacked dest rotation       ",
       10, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                PackTestError, fault_vram, 0x1914,copy$$src_lo_12_test,                 "PACK: 10: Copy 12 bit source lo 12 bit destinat    ",
       11, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                PackTestError, fault_vram, 0x1916,copy$$src_hi_12_test,                 "PACK: 11: Copy 12 bit source hi 12 bit destinat    ",
       12, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                PackTestError, fault_vram, 0x1918,copy$$dmar_pack_8_pack_test,          "PACK: 12: DMA Read  (-->) 8 pack to 8 pack         ",
       13, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                PackTestError, fault_vram, 0x191A,copy$$dmar_pack_8_unpack_test,        "PACK: 13: DMA Read  (-->) 8 pack to 8 unpack       ",
       14, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                PackTestError, fault_vram, 0x191C,copy$$dmar_pack_8_unpack_rot_test,    "PACK: 14: DMA Read  (-->) 8 pack to 8 unpack rot   ",
       15, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                PackTestError, fault_vram, 0x191E,copy$$dmaw_pack_8_pack_test,          "PACK: 15: DMA Write (<--) 8 pack to 8 pack         ",
       16, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                PackTestError, fault_vram, 0x1920,copy$$dmaw_unpack_8_pack_test,        "PACK: 16: DMA Write (<--) 8 unpack to 8 pack       ",
       17, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                PackTestError, fault_vram, 0x1922,line$$line_8_pack_vis_test,           "PACK: 17: Line 8  bit   packed visual test         ",
       18, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                PackTestError, fault_vram, 0x1924,line$$line_8_unpack_vis_test,         "PACK: 18: Line 8  bit unpacked visual test         ",
       19, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                PackTestError, fault_vram, 0x1926,line$$line_8_unpack_rot_test,         "PACK: 19: Line 8  bit unpacked rotate test         ",
       20, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                PackTestError, fault_vram, 0x1924,line$$line_12_packlo_vis_test,        "PACK: 20: Line 12 bit lo visual test   ",
       21, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                PackTestError, fault_vram, 0x1926,line$$line_12_packhi_vis_test,        "PACK: 21: Line 12 bit hi visual test   ",
       };


static struct  pack_type 
        {
        int  clear_memory;
        int  colormap;
	DISPATCH_TYPE *f;					
	int  fsize;						
	char *name;
        } pack_tests [] =
        {
	FALSE,FALSE,   pack_test_table,   sizeof (pack_test_table),      "pack",
        };


