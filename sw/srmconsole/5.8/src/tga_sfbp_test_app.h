

int     simz$$simple_test  (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);
int     simz$$stest_test   (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);
int     simz$$sfail_test   (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);
int     simz$$zfail_test   (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);
int     simz$$zpass_test   (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);
int     simz$$rmask_test   (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);
int     simz$$wmask_test   (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[]);
static  DISPATCH_TYPE simz_test_table []=
        {
        1, 0,0,0,SimzTestError, fault_asic, 0xA00,simz$$simple_test ,                     "SIMZ: 1: Simple Z Mode Ztest Test ",
        2, 0,0,0,SimzTestError, fault_asic, 0xA02,simz$$stest_test ,                      "SIMZ: 2: Simple Z Mode Stest Test ",
        3, 0,0,0,SimzTestError, fault_asic, 0xA04,simz$$sfail_test ,                      "SIMZ: 3: Simple Z Mode Sfail Test ",
        4, 0,0,0,SimzTestError, fault_asic, 0xA06,simz$$zfail_test ,                      "SIMZ: 4: Simple Z Mode Zfail Test ",
        5, 0,0,0,SimzTestError, fault_asic, 0xA08,simz$$zpass_test ,                      "SIMZ: 5: Simple Z Mode ZPass Test ",
        6, 0,0,0,SimzTestError, fault_asic, 0xA0A,simz$$rmask_test ,                      "SIMZ: 6: Simple Z Mode Rmask Test ",
        7, 0,0,0,SimzTestError, fault_asic, 0xA0C,simz$$wmask_test ,                      "SIMZ: 7: Simple Z Mode Wmask Test ",
        };

static struct  app_type 
        {
        int  clear_memory;
        int  colormap;
	DISPATCH_TYPE *f;					
	int  fsize;						
	char *name;
        } app_tests [] =
        {
	FALSE, FALSE,   simz_test_table,    sizeof (simz_test_table),       "simz",  
        };


