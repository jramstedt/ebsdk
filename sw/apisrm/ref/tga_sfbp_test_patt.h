
extern  int patt$$fill_vram             (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,  int param [] );
extern  int patt$$fill_gray             (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,  int param [] );
extern  int patt$$red_test              (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,  int param [] );
extern  int patt$$green_test            (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,  int param [] );
extern  int patt$$blue_test             (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,  int param [] );
extern  int patt$$white_test            (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,  int param [] );
extern  int patt$$black_test            (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,  int param [] );
extern  int patt$$color_test            (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,  int param [] );
extern  int patt$$shade_test            (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,  int param [] );
extern  int patt$$mux_visual_test       (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,  int param [] );
extern  int patt$$grayline_test         (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,  int param [] );

static  DISPATCH_TYPE  patt_test_table []=
         {
        1, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                PatternTestError, fault_ramdac, 0x1100,patt$$fill_vram,                 "PATT:  1: Color Bar ",
        2, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                PatternTestError, fault_ramdac, 0x1102,patt$$fill_gray,                 "PATT:  2: Gray Scale ",
        3, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                PatternTestError, fault_ramdac, 0x1104,patt$$red_test,                  "PATT:  3: Red screen ",
        4, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                PatternTestError, fault_ramdac, 0x1106,patt$$green_test,                "PATT:  4: Green screen ",
        5, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                PatternTestError, fault_ramdac, 0x1108,patt$$blue_test,                 "PATT:  5: Blue screen ",
        6, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                PatternTestError, fault_ramdac, 0x110A,patt$$white_test,                "PATT:  6: White Screen ",
        7, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                PatternTestError, fault_ramdac, 0x1110,patt$$black_test,                "PATT:  7: Black Screen ",
        8, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                PatternTestError, fault_ramdac, 0x110C,patt$$color_test,                "PATT:  8: Color Spans ",
        9, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                PatternTestError, fault_ramdac, 0x110E,patt$$shade_test,                "PATT:  9: Shade Test ",
       10, (int)chip8_table,(int)chip32_table,(int)chip32z_table,
                PatternTestError, fault_ramdac, 0x1110,patt$$grayline_test,             "PATT: 10: Grayline Test ",
        };


extern  int box$$load_grid              (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,  int param [] );
extern  int box$$load_circle            (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,  int param [] );
static  DISPATCH_TYPE box_test_table []=
        {
         1, 0,0,0,BoxTestError,nofault, 0x1600,box$$load_grid,             "BOX: 1: Load the grid map",
         2, 0,0,0,BoxTestError,nofault, 0x1602,box$$load_circle,           "BOX: 2: Load the circle  ",
        };


extern   int font$$flood_screen         (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,  int param [] );
static   DISPATCH_TYPE font_test_table []=
        {
        1, 0,0,0,FontTestError,nofault, 0x1700,font$$flood_screen ,        "FONT: 1: Font Test [-b] [-fH] ",
        };

extern   int test_init (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,  int param [] );
static   DISPATCH_TYPE init_test_table[]=
        {
        1, 0,0,0,InitError,nofault, 0x1700,test_init ,                      "INIT: 1: Init Device [e] [-s] [?]",
        };

extern   int test_cnfg (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,  int param [] );
static   DISPATCH_TYPE cnfg_test_table[]=
        {
        1,0,0,0,CnfgError, nofault, 0x1700,test_cnfg ,         "CNFG: 1: Configure Device ",
        };

int test_edit (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr, int param []);
static   DISPATCH_TYPE edit_test_table []=
        {
        1,0,0,0,EditError, nofault, 0x9998,test_edit ,        "EDIT: Examine and Deposit",
        };

extern   int test_chair (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,  int param [] );
static   DISPATCH_TYPE chair_test_table []=
  {
  1, 0,0,0,InitError,nofault, 0x1700,test_chair,  "Chair: 1: Chair Test ",
  };


int test_stereo (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr, int param []);
static   DISPATCH_TYPE stereo_test_table []=
        {
        1,0,0,0,EditError, nofault, 0x999A,test_stereo ,      "Stereo: Set Stereo Bit ",
        };

/* Set Color Map True For Pattern Test When Done        */
static struct  patt_test_type {
        int     clear_memory;
        int     colormap;
	DISPATCH_TYPE *f;					
	int  fsize;						
	char *name;
        } patt_tests[]={
	TRUE,TRUE,    patt_test_table,     sizeof(patt_test_table),           "patt",	  
	TRUE,FALSE,   box_test_table,      sizeof(box_test_table),            "box",	  
	FALSE,FALSE,  font_test_table,     sizeof(font_test_table),           "font",	 
	FALSE,FALSE,  init_test_table,     sizeof(init_test_table),           "init",	 
	FALSE,FALSE,  cnfg_test_table,     sizeof(cnfg_test_table),           "cnfg",	 
	FALSE,FALSE,  edit_test_table,     sizeof(edit_test_table),           "edit",	 
	FALSE,FALSE,  chair_test_table,    sizeof(chair_test_table),          "chair",	 
	FALSE,FALSE,  stereo_test_table,   sizeof(stereo_test_table),         "stereo",	 
       };

