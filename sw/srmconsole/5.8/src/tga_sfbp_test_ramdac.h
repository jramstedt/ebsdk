
extern 	 vdac_485$$cmd_reg_test 	(int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
extern   vdac_485$$cur_ram_test 	(int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
extern   vdac_485$$col_ram_test 	(int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
extern   vdac_485$$cur_col_test 	(int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
extern   vdac_485$$fill_vram 	        (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
static   DISPATCH_TYPE vdac_485_test_table []=
        {
        1, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
		VdacTestError, fault_ramdac, 0x1200,vdac_485$$fill_vram,         "VDAC_485:  1:  Fill Video Ram ",
	2, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
		VdacTestError, fault_ramdac, 0x1202,vdac_485$$cmd_reg_test,      "VDAC_485:  2:  Command register test",
        3, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
		VdacTestError, fault_ramdac, 0x1204,vdac_485$$col_ram_test,      "VDAC_485:  3:  Color ram test ",
        4, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
		VdacTestError, fault_ramdac, 0x1206,vdac_485$$cur_ram_test,      "VDAC_485:  4:  Cursor ram test",
        5, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
		VdacTestError, fault_ramdac, 0x1208,vdac_485$$cur_col_test,      "VDAC_485:  5:  Cursor color test",
       };

extern 	 vdac_463$$cmd_reg_test 	(int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
extern   vdac_463$$cur_ram_test 	(int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
extern   vdac_463$$col_ram_test 	(int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
extern   vdac_463$$cur_col_test 	(int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
extern   vdac_463$$cur_sig_test 	(int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
extern   vdac_463$$fill_vram 	        (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
extern   vdac_463$$control_plane_test 	(int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,   int param [] );
static   DISPATCH_TYPE vdac_463_test_table []=
        {
        1, 0,0,0,VdacTestError, fault_ramdac, 0x1200,vdac_463$$fill_vram,         "VDAC_463:  1:  Fill Video Ram ",
	2, 0,0,0,VdacTestError, fault_ramdac, 0x1202,vdac_463$$cmd_reg_test,      "VDAC_463:  2:  Command register test",
        3, 0,0,0,VdacTestError, fault_ramdac, 0x1206,vdac_463$$col_ram_test,      "VDAC_463:  3:  Color ram test ",
        4, 0,0,0,VdacTestError, fault_ramdac, 0x120A,vdac_463$$cur_ram_test,      "VDAC_463:  4:  Cursor ram test",
        5, 0,0,0,VdacTestError, fault_ramdac, 0x120C,vdac_463$$cur_col_test,      "VDAC_463:  5:  Cursor color test",
        6, 0,0,0,VdacTestError, fault_ramdac, 0x1210,vdac_463$$cur_sig_test,      "VDAC_463:  6:  Cursor signature test",
        7, 0,0,0,VdacTestError, fault_ramdac, 0x1212,vdac_463$$control_plane_test,"VDAC_463:  7:  Control and Overlay RAM Test",
       };

static struct  ramdac_test_463_type {
        int     clear_memory;
        int     colormap;
	DISPATCH_TYPE *f;					
	int  fsize;						
	char *name;
        } ramdac_463_tests[]={
	TRUE,TRUE,  vdac_463_test_table, sizeof (vdac_463_test_table),      "vdac",	  
       };

static struct  ramdac_test_485_type {
        int     clear_memory;
        int     colormap;
	DISPATCH_TYPE *f;					
	int  fsize;						
	char *name;
        } ramdac_485_tests[]={
	TRUE,TRUE,  vdac_485_test_table, sizeof (vdac_485_test_table),      "vdac",	  
       };
