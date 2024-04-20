
extern   int test_flash                 (int argc,char ** argv,CCV *ccv, SCIA * scia_ptr, SFBP *data_ptr,  int param [] );
static   DISPATCH_TYPE flash_test_table []=
        {
        1,0,0,0,FlashError,fault_asic,0,test_flash ,               "FLASH: SFB+ Flash Utility",
        };


static struct  flash_test_type {
        int     clear_memory;
        int     colormap;
	DISPATCH_TYPE *f;					
	int  fsize;						
	char *name;
        } flash_tests[]={
	FALSE,FALSE,  flash_test_table,    sizeof(flash_test_table),          "flash" ,
       };
