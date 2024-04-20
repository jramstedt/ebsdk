/* file:	tga_test.c
 *                                                                       
 * Copyright (C) 1991, 1992 by
 * Digital Equipment Corporation, Maynard, Massachusetts.
 * All rights reserved.
 *
 * This software is furnished under a license and may be used and copied
 * only  in  accordance  of  the  terms  of  such  license  and with the
 * inclusion of the above copyright notice. This software or  any  other
 * copies thereof may not be provided or otherwise made available to any
 * other person.  No title to and  ownership of the  software is  hereby
 * transferred.                           
 *
 * The information in this software is  subject to change without notice
 * and  should  not  be  construed  as a commitment by digital equipment
 * corporation.           
 *                                                                       
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by digital.
 */                    

/*
 *++
 *  FACILITY: 
 *
 *      Morgan Firmware 
 *              
 *  MODULE DESCRIPTION:
 *	Front end piece of code for the tga diagnostics. This 
 *	is the entry point for the diagnostics.
 *
 *  AUTHORS:
 *      Jim Peacock
 *                     
 *  CREATION DATE:
 *      03-March-1993
 *
 *  MODIFICATION HISTORY:          
 *                  
 *--                   
 */                                                

#include 	"cp$src:prdef.h"
#include	"cp$src:kernel_def.h"          
#include	"cp$src:parse_def.h"    
#include	"cp$src:version.h"
#include	"cp$src:dynamic_def.h"
#include 	"cp$src:ctype.h"                                           
#include      	"cp$src:common.h"                 
#include "cp$inc:prototypes.h"
#include 	"cp$src:diag_def.h"                
#include      	"cp$src:diag_groups.h"
#include 	"cp$src:msg_def.h"                               
#include	"cp$src:pb_def.h"

#include 	"cp$src:tga_sfbp_include.h"
#include   	"cp$src:platform.h"
#include 	"cp$src:tga_sfbp_callbacks.h"
#include 	"cp$src:tga_sfbp_prom_defines.h"
#include 	"cp$src:tga_sfbp_def.h"
#include 	"cp$src:tga_sfbp_protos.h"
#include 	"cp$src:tga_sfbp_463.h"
#include 	"cp$src:tga_sfbp_485.h"
#include 	"cp$src:tga_sfbp_ext.h"
#include 	"cp$src:tga_sfbp_crb_def.h"
#include 	"cp$src:tga_sfbp_ctb_def.h"
#include 	"cp$src:tga_sfbp_test_dispatch.h" 
#include 	"cp$src:tga_sfbp_fault_data.h" 
#include 	"cp$src:tga_sfbp_test_app.h"
#include 	"cp$src:tga_sfbp_test_ops.h"
#include 	"cp$src:tga_sfbp_test_line.h"
#include 	"cp$src:tga_sfbp_test_simple.h"
#include 	"cp$src:tga_sfbp_test_pack.h"
#include 	"cp$src:tga_sfbp_test_patt.h"
#include        "cp$src:tga_sfbp_test_flashd.h"
#include        "cp$src:tga_sfbp_test_ramdac.h"


TABLE_ENTRY app_table_tests [] =
        {
        (TABLE *)app_tests,          	sizeof(app_tests),           0,		"tciapp",
        };

TABLE_ENTRY ops_table_tests [] =
        {
        (TABLE *)ops_tests,             sizeof(ops_tests),           0,		"tciops",
        };

TABLE_ENTRY simple_table_tests [] =
        {
        (TABLE *)simple_tests,          sizeof(simple_tests),        0,	         "tcisimple",
        };

TABLE_ENTRY line_table_tests [] =
        {           
        (TABLE *)dispatch_line_tests,       sizeof(dispatch_line_tests),  0,	"tciline",
        };           
                 
TABLE_ENTRY pack_table_tests [] =
        {        
        (TABLE *)pack_tests,          sizeof(pack_tests),         0,	"tcipack",
        };                                                       
                 
TABLE_ENTRY patt_table_tests [] =
        {        
        (TABLE *)patt_tests,          sizeof(patt_tests),         0,	"tcipatt",
        };
                                                                    
TABLE_ENTRY flash_table_tests [] =
        {
        (TABLE *)flash_tests,          sizeof(flash_tests),           0,        "tciflash",
        };

TABLE_ENTRY ramdac_table_463_tests [] =
        {
        (TABLE *)ramdac_463_tests,          sizeof(ramdac_463_tests),           0,      "tciramdac",
        };

TABLE_ENTRY ramdac_table_485_tests [] =
        {
        (TABLE *)ramdac_485_tests,          sizeof(ramdac_485_tests),           0,      "tciramdac",
        };

extern DIAG_GROUP diag_groups[];                                 
extern int 	  spl_kernel;                            
extern unsigned   int poweron_msec [2];
extern int	  diag_init(int * argc, char *argv[], struct QSTRUCT * quals, char * new_opt_string,int qscan_done);
extern int	  report_error();                
extern void    	  diag_start();
extern int     	  killpending();                     
extern void  	  create_iob ();                 
extern void  	  remq_iob();                          
extern int 	  err_print();                 		
extern int	  report_error_locked();                   
extern int	  report_error_release();
extern  int       TGA_mem_base [];
                                                    
/* FInd out what the real vector is 	*/
#define	SCB$L_TGA	1
  
int tga_test_dispatch (int argc, char *argv[]);
int tgatest_extend ();
int tgatest_cleanup ();
int tga_load_error_info(int err_num, int param []);
int GetParseFlags (int argc , char **argv,char check,int *flag);
int subtest_check (char **argv,int argc,char *ptr);
int sfbp$$signature_mode_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr,
                SFBP * data_ptr, int param [],struct signature_type *signature);
int sfbp$$input_signature_mode_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr,
                SFBP * data_ptr, int param [],struct signature_type *signature);
                                                              
#define	QSA     NUM_QUALS	/* Start address */
#define	QEA     NUM_QUALS + 1	/* End address */
#define	QL     	NUM_QUALS + 2	/* Length in bytes */
#define	QBS     NUM_QUALS + 3	/* block size in bytes */
#define	QF     	NUM_QUALS + 4	/* fast mode don't verify data */
#define	QM     	NUM_QUALS + 5	/* timer report pass time */
#define	QZ     	NUM_QUALS + 6	/* don't malloc or verify start address */
#define	QD     	NUM_QUALS + 7	/* Test pattern */         
#define	QH     	NUM_QUALS + 8	/* malloc from firmware heap */
#define	QRS     NUM_QUALS + 9	/* Random seed */            
#define	QRB     NUM_QUALS + 10	/* Random memory allocations */
#define	QI      NUM_QUALS + 11	/* Increment */
#define	QMB     NUM_QUALS + 12	/* Do mb in alpha memtest -f version */
#define DEF_LEN_SZ 8192                    
#define MORE_QUALS 13
                                       
/*------------------------------------------------------------------------------
  Dispatch Table.
  diag_start will use this table to call each test listed.
------------------------------------------------------------------------------*/
static struct DIAG_DIS_TABLE _align (QUADWORD) tga_dispatch_table[] = 
    {
    {tga_test_dispatch, m_grp_mfg|m_grp_fld},
    {(int)NULL, (int)NULL}
    };                                                  
/*------------------------------------------------------------------------------
  Program External Variables.                                
                                                                 
  Any desired external variables should be placed in a structure and the
  structure should define a typedef.  The initialization code can then
  dynamically allocated space for these variables.
                                                       
------------------------------------------------------------------------------*/
                                                                       
typedef struct externals_struct {               
	       	int	      	expect;         /* Expected data */
	       	int	    	rcvd;           /* Received data */
	       	int		failadr;        /* Failing address */
	       	int    		len;	 	/* total #of bytes to test */
	       	int    		incr;	 	/* Increment */
	       	int	     	*start_addr;    /* start address */      
	       	int	     	end_addr;       /* end address */      
	       	int		fast;	       	/* skip data compare */      
	       	unsigned int	block_size[2];	/* number of bytes to test */
	       	int		timer;		/* Turn on time reporting */
	       	int	    	no_check;	/* Don't check address */
	       	int		do_mb;		/* Do mb in -f alpha version */
      	       	unsigned int	test_patt;	/* March test pattern to use */
	       	unsigned int	rand_seed;	/* Seed to use to obtain random
	       					   data pattern */     
/******************************/
/* Added for the TGA          */
/******************************/
	       	char		*argv[10]; /*Array of char pointers */
	       	int		argc;
		struct FILE	*fp;
} externals;                                                      
 
int      span$g_section = 0;
#define  MAX_LINES 4
extern   int	 TGAInterruptCount;
extern   int PrintVerify;
void     GetTgaConsole (SFBP *data_ptr);
extern   struct pb *TGA_pb[];

#define	 MaxTgaCount 	3

extern  SFBP  *TGA_sfbp_ptr[];
extern	SCIA  *TGA_scia_ptr[];
extern  IOSEG *TGA_ioseg_ptr[];

extern  struct   function_block_desc   cp_function_block [MaxTgaCount];
extern  int TgaCount;

void    tga_software_init (struct CH_STATE *chs,int x,int y);

extern  struct CH_STATE *tga_ch_state;

                                                 
/*+                                                                  
 * ============================================================================
 * = tgatest - Exercise the tga graphics module.			      =
 * ============================================================================
 *                                                
 * OVERVIEW:                                     
 *
 *	Exercise the tga graphics module.
 *
 *  COMMAND FORM:                                             
 *        
 *   	tgatest ( <test> [?] [-v] [-tn] )
 *                                                         
 *   COMMAND TAG: tgatest 0 RXBZ tgatest
 *
 *   COMMAND ARGUMENT(S):
 *                                                  
 *   COMMAND OPTION(S):                                                
 *                                            
 *   COMMAND EXAMPLE(S):                                                
 *                                            
 *   COMMAND REFERENCES:                                                
 *                                            
 * FORM OF CALL:                                           
 *                                  
 *	tgatest ( argc, *argv[] )                  
 *                                                                        
 * RETURN CODES:                                  
 *                                                           
 * ARGUMENTS:                                                                
 *                                                                
 *     	int argc - number of command line arguments passed by the shell 
 *	char *argv [] - array of pointers to arguments             
 *
 *	When we come into this routine, the TGA driver has already been
 *   	initialized. 
 *                                            
 * SIDE EFFECTS:                                         
 *                                                                
 *                              
-*/                                                                
int tgatest (int argc, char *argv [])
{               
 int 	    i,s=1,passcount;
 externals  *eptr;            
 struct    PCB	 	*pcb;                                           
 struct    IOB	 	*iob_ptr;    	
 struct    QSTRUCT     	qual[NUM_QUALS + MORE_QUALS];           
                                                   
 /* Get process block	*/
 pcb = getpcb();                                          
 pcb->pcb$a_dis_table = tga_dispatch_table;               

              
 /* Cleanup Routine	*/       
 pcb->pcb$a_rundown = (void *)tgatest_cleanup;            
                                                                          
 /* Use "tga" for the device name */                    
 strncpy(pcb->pcb$b_dev_name, "tga", MAX_NAME);   
                                                           
 /* MALLOC space for and create the IOB  */                             
 iob_ptr = (struct IOB *)malloc(sizeof(struct IOB));
                        
 /* Should be checking the malloc stat here actually	*/
 create_iob(iob_ptr, pcb->pcb$b_dev_name);                                
      	                                                                
 /* Allocate memory for the global variables.	*/
 eptr = (externals *)malloc(sizeof(externals));
 pcb->pcb$a_globals = (int *) eptr;
                                                          
 /* TGA extended error routine loaded here */
                        
                                                                          
 eptr->fast  = (qual [QF].present) ? 1 : 0;               
 eptr->timer = (qual [QM].present) ? 1 : 0;                       
 passcount   = (qual [QP].present) ? qual [QP].value.integer : 1;
                         
 /* Save away some info needed by diag's */
 for (i=0;i<10;i++)eptr->argv[i]=argv[i];
 eptr->argc = argc;     
                        
 tga_test_dispatch(argc, argv); 
                        
}                                                               
                      
             
/*++
* ===========================================================================
* = tga_test_dispatch - sfbp Diagnostic Test Dispatcher =
* ===========================================================================
*
* OVERVIEW:
*       This is the T Rom object diagnostic test dispatcher. 
* 
* FORM OF CALL:                                           
*       tga_test_dispatch (argc, argv)
*
* RETURNS:
*       0 for success else -1 for failure
*
* GLOBALS:
*       none
*
* FUNCTIONAL DESCRIPTION:
*
* RETURN CODES:
*               
* CALLS:
*
* SIDE EFFECTS:
*
* ALGORITHM:
*   
--*/
int tga_test_dispatch (int argc, char *argv[])
{
                  
 int   TgaNumber,length,done=0,s,i,j;
 char  *m;
 struct     PCB *pcb;                             
 externals  *eptr;                            

 pcb  		 = getpcb();                             
 eptr 		 = (externals *)pcb->pcb$a_globals; 

 if (TgaCount==0) {printf ("No Tga Options\n");return(0);}

 /* Test Each TGA we find	*/
 /* Make it Sequential		*/
 for (TgaNumber=0;TgaNumber<TgaCount;TgaNumber++)
  {
  ConnectToTga(TgaNumber);
  
  /*Initialize the TGA software*/   
  tga_software_init(tga_ch_state,
  	data_ptr->sfbp$l_cursor_row,data_ptr->sfbp$l_cursor_column);
  
  if (!data_ptr->sfbp$l_device_initialized)
  	{                    
  	(*cp_func_ptr->pv_init_output)(scia_ptr,data_ptr);  
  	data_ptr->sfbp$l_device_initialized = 1;
  	}
  		
  parse_init (scia_ptr,data_ptr);
                                
  (*parse_func_ptr->pv_parse_command_line) (scia_ptr,data_ptr,
  	eptr->argc,eptr->argv,ccv,TgaNumber);
                  
  ParseFlags (data_ptr, eptr->argc,eptr->argv);   
  
  GetTgaConsole (data_ptr);
         
  for (i=0;eptr->argc>1 && i*sizeof(struct  test_dispatch_type)<
  	 	sizeof(overlay_tests);i++)
  {                           
    if (strncmp(eptr->argv[1],overlay_tests[i].name,3) == 0)
    {                                                  
        s= (*overlay_tests[i].rtn) (scia_ptr,data_ptr,
    			eptr->argc,eptr->argv,TgaNumber,ccv);
        done = TRUE;
        break;
    }                          
  }                              
  if (!done && TgaNumber==0)
    for (i=0;i*sizeof(struct  test_dispatch_type)<sizeof(overlay_tests);i++)
                printf ("%s\n",overlay_tests[i].text);
 
 } /* End Testing Each TGA Found	*/
 
  /* Re-establish Connection With Console	*/
  /* This is when you have multiple TGA slots   */
  /* Make sure console still at right slot      */ 
                               
  ConnectToTga(TgaConsoleSlot); 

  /* Connect Up with Character State Structure		*/
  tga_software_init(tga_ch_state,
	data_ptr->sfbp$l_cursor_row,data_ptr->sfbp$l_cursor_column);

  return (1);
 }


/*+
* ===========================================================================
* = ParseFlags - Parse the Flags =
* ===========================================================================
*
* OVERVIEW:
*       Parse command line flags 
* 
* FORM OF CALL:
*       ParseFlags (data_ptr,argc,argv)
*
* ARGUMENTS:
*       argc    - argument counter
*       argv    - argument pointer array
*       
* FUNCTIONAL DESCRIPTION:
*       Parse the flags
*
--*/
int ParseFlags (SFBP *data_ptr, int argc , char **argv)

{
char    *pptr;
int     i,j;


     data_ptr->sfbp$l_list_tests =0;
     for (j=0,pptr=argv[j];j<argc;j++,pptr=argv[j])    
        if (*pptr++=='-'&& *pptr++== 'd' && *pptr++== 'l' && *pptr++== 's'    )
                        {data_ptr->sfbp$l_list_tests=1;break;}

     data_ptr->sfbp$l_loope =0;
     for (j=0,pptr=argv[j];j<argc;j++,pptr=argv[j])    
        if (*pptr++=='-'&& *pptr++== 'd' && *pptr++== 'l' && *pptr++=='e' )
                        {data_ptr->sfbp$l_loope=1;break;}

     data_ptr->sfbp$l_conte =0;
     for (j=0,pptr=argv[j];j<argc;j++,pptr=argv[j])    
        if (*pptr++=='-'&& *pptr++== 'd' && *pptr++== 'c' && *pptr++=='e' )
                        {data_ptr->sfbp$l_conte=1;break;}

     data_ptr->sfbp$l_manufacturing_mode =0;
     for (j=0,pptr=argv[j];j<argc;j++,pptr=argv[j])    
        if (*pptr++=='-'&& *pptr++== 'd' && *pptr++== 'm' )
                        {data_ptr->sfbp$l_manufacturing_mode=1;break;}

     data_ptr->sfbp$l_display_clear =1;
     for (j=0,pptr=argv[j];j<argc;j++,pptr=argv[j])    
        if (*pptr++=='-'&& *pptr++== 'n' && *pptr++== 'c' )
                        {data_ptr->sfbp$l_display_clear=0;break;}
                                                        
     GetParseFlags(argc,argv,'p',&data_ptr->sfbp$l_print);
     GetParseFlags(argc,argv,'x',&data_ptr->sfbp$l_supplemental);
     GetParseFlags(argc,argv,'v',&data_ptr->sfbp$l_verify_lines);
          
     if (data_ptr->sfbp$l_module==hx_32_plane_z)
        GetParseFlags(argc,argv,'z',&data_ptr->sfbp$l_test_zbuffer);

return (1);
}                                


/*+
* ===========================================================================
* = GetParseFlags - Parse the Flags =
* ===========================================================================
*
* OVERVIEW:
*       Parse command line flags 
* 
* FORM OF CALL:
*       GetParseFlags (argc,argv,check)
*
* ARGUMENTS:
*       argc    - argument counter
*       argv    - argument pointer array   
*
* FUNCTIONAL DESCRIPTION:
*       Parse the flags
*                  
--*/
int GetParseFlags (int argc , char **argv,char check,int *flag)

{
char    *pptr;
int     i,j;

for (*flag=0,j=0,pptr= argv[j];j<argc;j++,pptr= argv[j])    
        if (*pptr++=='-' &&*pptr++== 'd' && *pptr++== check){*flag=1;break;}
         
return (1);
}


/*+
* ===========================================================================
* = parse_init  - Initialize parse pointers = 
* ===========================================================================
*
* OVERVIEW:
*       Initialize parse entries
* 
* FORM OF CALL:    
*
*       parse_init (scia_ptr,data_ptr)
*
* ARGUMENTS:
*       scia_ptr     - scia pointer
*       data_ptr     - driver data area pointer
*
* FUNCTIONAL DESCRIPTION:
*       Make function block for common parse routines
*
--*/
int parse_init  (SCIA *scia_ptr,SFBP *data_ptr)
                   
{                  
parse_func_ptr    = (struct parse_func_block *)&data_ptr->sfbp$r_parse_func;
                   
parse_func_ptr->pv_parse_command_line           = parse_command_line ;
parse_func_ptr->pv_parse_check                  = parse_check ;
parse_func_ptr->pv_subtest_check                = subtest_check ;
parse_func_ptr->pv_test_dispatch                = test_dispatch ;
parse_func_ptr->pv_subtest_dispatch             = subtest_dispatch ;
parse_func_ptr->pv_signature_mode_test          = sfbp$$signature_mode_test;
parse_func_ptr->pv_input_signature_mode_test    = sfbp$$input_signature_mode_test;
parse_func_ptr->pv_verify_line                  = sfbp$$verify_draw_line;

return (1);                                          
                                                     
}                                                    

/*++                                       
* ===========================================================================
* = dispatch_app - sfbp Diagnostic Test Dispatcher =
* ===========================================================================
*
* OVERVIEW:
*       This is the T Rom object diagnostic test dispatcher. 
* 
* FORM OF CALL:
*       dispatch_app ( scia_ptr,data_ptr,argc, argv, slot, ccv)
*
* RETURNS:
*       0 for success else -1 for failure
*
* ARGUMENTS:
*       scia_ptr  - driver area pointer
*       data_ptr  - driver data area pointer
*       argc    - argument count >=1
*       argv    - argv [0]="t", argv[1]="#/testname" argv[n] = -v -t -n -f -c -s 
*       slot    - the slot number
*       ccv     - callback vector array 
*
* GLOBALS:
*       none
*
* FUNCTIONAL DESCRIPTION:
*       This is called by the entry point dispatcher after it checked
*       for a T rom object. This is the REX layer of code. Underneath
*       this layer is the CTIS layer as you call it which has 
*       references to driver data areas and the such which will make
*       the test code more portable. The test dispatcher will then
*       call of the tests with pointers to the SCIA area and driver 
*       data area for easy access.
*
* RETURN CODES:
*        0 for success
*       -1 for soft failure
*       -2 for fatal error
*       -3 more fatal than above
*
* CALLS:
*
* SIDE EFFECTS:
*
* ALGORITHM:
*
*        - Look for a "/" in the first 4 characters.
*        - IF no "/" qualifier print an error message and exit.
*        - ELSE if of the format "/?" then print help of tests included.
*        - ELSE 
*           - Parse out the subtest number if -t qualifier present 
*           - Parse out the font character if -f qualifier present
*           - Parse out the line count if -n qualifier present
*           - Parse out the number of copy rows if -c present
*           - Parse out the number of stip rows if -s present
*           - parse out the verbose -v qualifier if present
*           - REPEAT
*              - If the table entry test name is same as command line THEN
*                call the test routine
*              - Else continue
*           - END REPEAT scanning table
*        - If test status is 0, then return -1 to rex,otherwise return 0 for success
*   
--*/
int dispatch_app ( SCIA *scia_ptr,SFBP *data_ptr,
	int argc, char **argv, int slot, CCV *ccv_vector)
{
int     s;

s = (*parse_func_ptr->pv_test_dispatch) (scia_ptr,data_ptr, argc, argv, slot , ccv , 
        app_table_tests ,sizeof (app_table_tests) );

return (s);
}


/*++
* ===========================================================================
* = dispatch_ops - sfbp Diagnostic Test Dispatcher =
* ===========================================================================
*
* OVERVIEW:
*       This is the T Rom object diagnostic test dispatcher. 
* 
* FORM OF CALL:
*       test( scia_ptr,data_ptr,argc, argv, slot, ccv)
4*
* RETURNS:
*       0 for success else -1 for failure
*
* ARGUMENTS:
*       scia_ptr  - driver area pointer
*       data_ptr  - driver data area pointer
*       argc    - argument count >=1
*       argv    - argv [0]="t", argv[1]="#/testname" argv[n] = -v -t -n -f -c -s 
*       slot    - the slot number
*       ccv     - callback vector array 
*
* GLOBALS:
*       none
*
* FUNCTIONAL DESCRIPTION:
*       This is called by the entry point dispatcher after it checked
*       for a T rom object. This is the REX layer of code. Underneath
*       this layer is the CTIS layer as you call it which has 
*       references to driver data areas and the such which will make
*       the test code more portable. The test dispatcher will then
*       call of the tests with pointers to the SCIA area and driver 
*       data area for easy access.
*
* RETURN CODES:
*        0 for success
*       -1 for soft failure
*       -2 for fatal error
*       -3 more fatal than above
*
* CALLS:
*
* SIDE EFFECTS:
*
* ALGORITHM:
*
*        - Look for a "/" in the first 4 characters.
*        - IF no "/" qualifier print an error message and exit.
*        - ELSE if of the format "/?" then print help of tests included.
*        - ELSE 
*           - Parse out the subtest number if -t qualifier present 
*           - Parse out the font character if -f qualifier present
*           - Parse out the line count if -n qualifier present
*           - Parse out the number of copy rows if -c present
*           - Parse out the number of stip rows if -s present
*           - parse out the verbose -v qualifier if present
*           - REPEAT
*              - If the table entry test name is same as command line THEN
*                call the test routine
*              - Else continue
*           - END REPEAT scanning table
*        - If test status is 0, then return -1 to rex,otherwise return 0 for success
*   
--*/
int dispatch_ops ( SCIA *scia_ptr,SFBP *data_ptr,
	int argc, char **argv, int slot, CCV *ccv_vector)
{
int     s;

s = (*parse_func_ptr->pv_test_dispatch) (scia_ptr,data_ptr, argc, argv, slot , ccv , 
        ops_table_tests ,sizeof (ops_table_tests) );
         
return (s);
}


/*++
* ===========================================================================
* = dispatch_simple - sfbp Diagnostic Test Dispatcher =
* ===========================================================================
*
* OVERVIEW:
*       This is the T Rom object diagnostic test dispatcher. 
* 
* FORM OF CALL:
*       dispatch_simple ( scia_ptr,data_ptr,argc, argv, slot, ccv)
*
* RETURNS:
*       0 for success else -1 for failure
*
* ARGUMENTS:
*       scia_ptr  - driver area pointer
*       data_ptr  - driver data area pointer
*       argc    - argument count >=1
*       argv    - argv [0]="t", argv[1]="#/testname" argv[n] = -v -t -n -f -c -s 
*       slot    - the slot number
*       ccv     - callback vector array 
*
* GLOBALS:
*       none
*
* FUNCTIONAL DESCRIPTION:
*       This is called by the entry point dispatcher after it checked
*       for a T rom object. This is the REX layer of code. Underneath
*       this layer is the CTIS layer as you call it which has 
*       references to driver data areas and the such which will make
*       the test code more portable. The test dispatcher will then
*       call of the tests with pointers to the SCIA area and driver 
*       data area for easy access.
*                                   
* RETURN CODES:
*        0 for success
*       -1 for soft failure
*       -2 for fatal error
*       -3 more fatal than above
*
* CALLS:
*
* SIDE EFFECTS:
*
* ALGORITHM:
*   
*        - Look for a "/" in the first 4 characters.
*        - IF no "/" qualifier print an error message and exit.
*        - ELSE if of the format "/?" then print help of tests included.
*        - ELSE 
*           - Parse out the subtest number if -t qualifier present 
*           - Parse out the font character if -f qualifier present
*           - Parse out the line count if -n qualifier present
*           - Parse out the number of copy rows if -c present
*           - Parse out the number of stip rows if -s present
*           - parse out the verbose -v qualifier if present
*           - REPEAT
*              - If the table entry test name is same as command line THEN
*                call the test routine
*              - Else continue
*           - END REPEAT scanning table
*        - If test status is 0, then return -1 to rex,otherwise return 0 for success
*   
--*/
int dispatch_simple ( SCIA *scia_ptr,SFBP *data_ptr,
	int argc, char **argv, int slot,CCV *ccv_vector)
{   
int      s; 
            
s = (*parse_func_ptr->pv_test_dispatch) (scia_ptr,data_ptr, argc, argv, slot , ccv , 
        simple_table_tests ,sizeof (simple_table_tests) );
                                   
return (s);
}


/*++
* ===========================================================================
* = dispatch_line - sfbp Diagnostic Test Dispatcher =
* ===========================================================================
*
* OVERVIEW:
*       This is the T Rom object diagnostic test dispatcher. 
* 
* FORM OF CALL:
*       dispatch_line ( scia_ptr,data_ptr,argc, argv, slot, ccv)
*
* RETURNS:
*       0 for success else -1 for failure
*
* ARGUMENTS:
*       scia_ptr  - driver area pointer
*       data_ptr  - driver data area pointer
*       argc    - argument count >=1
*       argv    - argv [0]="t", argv[1]="#/testname" argv[n] = -v -t -n -f -c -s 
*       slot    - the slot number
*       ccv     - callback vector array 
*
* GLOBALS:
*       none
*
* FUNCTIONAL DESCRIPTION:
*       This is called by the entry point dispatcher after it checked
*       for a T rom object. This is the REX layer of code. Underneath
*       this layer is the CTIS layer as you call it which has 
*       references to driver data areas and the such which will make
*       the test code more portable. The test dispatcher will then
*       call of the tests with pointers to the SCIA area and driver 
*       data area for easy access.
*
* RETURN CODES:
*        0 for success
*       -1 for soft failure
*       -2 for fatal error
*       -3 more fatal than above
*
* CALLS:
*
* SIDE EFFECTS:
*
* ALGORITHM:
*
*        - Look for a "/" in the first 4 characters.
*        - IF no "/" qualifier print an error message and exit.
*        - ELSE if of the format "/?" then print help of tests included.
*        - ELSE 
*           - Parse out the subtest number if -t qualifier present 
*           - Parse out the font character if -f qualifier present
*           - Parse out the line count if -n qualifier present
*           - Parse out the number of copy rows if -c present
*           - Parse out the number of stip rows if -s present
*           - parse out the verbose -v qualifier if present
*           - REPEAT
*              - If the table entry test name is same as command line THEN
*                call the test routine
*              - Else continue
*           - END REPEAT scanning table
*        - If test status is 0, then return -1 to rex,otherwise return 0 for success
*   
--*/
int dispatch_line ( SCIA *scia_ptr,SFBP *data_ptr,
	int argc, char **argv, int slot, CCV *ccv_vector)
{
int     s;

s = (*parse_func_ptr->pv_test_dispatch) (scia_ptr,data_ptr, argc, argv, slot , ccv , 
        line_table_tests ,sizeof (line_table_tests) );
         
return (s);
}


/*++
* ===========================================================================
* = dispatch_pack - sfbp Diagnostic Test Dispatcher =
* ===========================================================================
*
* OVERVIEW:
*       This is the T Rom object diagnostic test dispatcher. 
* 
* FORM OF CALL:
*       dispatch_pack ( scia_ptr,data_ptr,argc, argv, slot, ccv)
*
* RETURNS:
*       0 for success else -1 for failure
*
* ARGUMENTS:
*       scia_ptr  - driver area pointer
*       data_ptr  - driver data area pointer
*       argc    - argument count >=1
*       argv    - argv [0]="t", argv[1]="#/testname" argv[n] = -v -t -n -f -c -s 
*       slot    - the slot number
*       ccv     - callback vector array 
*
* GLOBALS:
*       none
*
* FUNCTIONAL DESCRIPTION:
*       This is called by the entry point dispatcher after it checked
*       for a T rom object. This is the REX layer of code. Underneath
*       this layer is the CTIS layer as you call it which has 
*       references to driver data areas and the such which will make
*       the test code more portable. The test dispatcher will then
*       call of the tests with pointers to the SCIA area and driver 
*       data area for easy access.
*
* RETURN CODES:
*        0 for success
*       -1 for soft failure
*       -2 for fatal error
*       -3 more fatal than above
*
* CALLS:
*
* SIDE EFFECTS:
*
* ALGORITHM:
*
*        - Look for a "/" in the first 4 characters.
*        - IF no "/" qualifier print an error message and exit.
*        - ELSE if of the format "/?" then print help of tests included.
*        - ELSE 
*           - Parse out the subtest number if -t qualifier present 
*           - Parse out the font character if -f qualifier present
*           - Parse out the line count if -n qualifier present
*           - Parse out the number of copy rows if -c present
*           - Parse out the number of stip rows if -s present
*           - parse out the verbose -v qualifier if present
*           - REPEAT
*              - If the table entry test name is same as command line THEN
*                call the test routine
*              - Else continue
*           - END REPEAT scanning table
*        - If test status is 0, then return -1 to rex,otherwise return 0 for success
*   
--*/
int dispatch_pack ( SCIA *scia_ptr,SFBP *data_ptr,
		int argc, char **argv, int slot, CCV *ccv_vector)
{
int     s;

s = (*parse_func_ptr->pv_test_dispatch) (scia_ptr,data_ptr, argc, argv, slot , ccv , 
        pack_table_tests ,sizeof (pack_table_tests) );

return (s);
}


/*++
* ===========================================================================
* = dispatch_patt - sfbp Diagnostic Test Dispatcher =
* ===========================================================================
*
* OVERVIEW:
*       This is the T Rom object diagnostic test dispatcher. 
* 
* FORM OF CALL:
*       dispatch_patt ( scia_ptr,data_ptr,argc, argv, slot, ccv)
*
* RETURNS:
*       0 for success else -1 for failure
*
* ARGUMENTS:
*       scia_ptr  - driver area pointer
*       data_ptr  - driver data area pointer
*       argc    - argument count >=1
*       argv    - argv [0]="t", argv[1]="#/testname" argv[n] = -v -t -n -f -c -s 
*       slot    - the slot number
*       ccv     - callback vector array 
*
* GLOBALS:
*       none
*
* FUNCTIONAL DESCRIPTION:
*       This is called by the entry point dispatcher after it checked
*       for a T rom object. This is the REX layer of code. Underneath
*       this layer is the CTIS layer as you call it which has 
*       references to driver data areas and the such which will make
*       the test code more portable. The test dispatcher will then
*       call of the tests with pointers to the SCIA area and driver 
*       data area for easy access.
*
* RETURN CODES:
*        0 for success
*       -1 for soft failure
*       -2 for fatal error
*       -3 more fatal than above
*
* CALLS:
*
* SIDE EFFECTS:
*
* ALGORITHM:
*
*        - Look for a "/" in the first 4 characters.
*        - IF no "/" qualifier print an error message and exit.
*        - ELSE if of the format "/?" then print help of tests included.
*        - ELSE 
*           - Parse out the subtest number if -t qualifier present 
*           - Parse out the font character if -f qualifier present
*           - Parse out the line count if -n qualifier present
*           - Parse out the number of copy rows if -c present
*           - Parse out the number of stip rows if -s present
*           - parse out the verbose -v qualifier if present
*           - REPEAT
*              - If the table entry test name is same as command line THEN
*                call the test routine
*              - Else continue
*           - END REPEAT scanning table
*        - If test status is 0, then return -1 to rex,otherwise return 0 for success
*   
--*/
int dispatch_patt ( SCIA *scia_ptr,SFBP *data_ptr,
		int argc, char **argv, int slot, CCV *ccv_vector)
{
int     s;
                         
s = (*parse_func_ptr->pv_test_dispatch) (scia_ptr,data_ptr, argc, argv, slot , ccv , 
        patt_table_tests ,sizeof (patt_table_tests) );

return (s);
}


/*++
* ===========================================================================
* = dispatch_ramdac - sfbp Diagnostic Test Dispatcher =
* ===========================================================================
*
* OVERVIEW:
*       This is anm overlay dispatcher.
* 
* FORM OF CALL:
*       dispatch_ramdac ( scia_ptr,argc, argv, slot, ccv)
*
* RETURNS:
*       0 for success else -1 for failure
*
* ARGUMENTS:
*       scia_ptr  - driver area pointer
*       argc      - argument count >=1
*       argv      - argv 
*       slot      - the slot number
*       ccv     -   callback vector array 
*
* GLOBALS:
*       none
*
* FUNCTIONAL DESCRIPTION:
*       This is the front end test dispatcher for our test modules. 
*       Since I am using an overlay structure, I have to reconnect
*       my pointers and the such the first time into the overlay
*       module. After that, I then dispatch (callback) to the 
*       common test routine dispatcher which is part of the 
*       t rom object and is always resident. Each overlay 
*       requires this module.
*
--*/


int dispatch_ramdac ( SCIA *scia_ptr,SFBP *data_ptr,
	int argc, char **argv, int slot, CCV *ccv_vector)
{
int     s=0;

drv_desc_ptr    = (struct drv_desc   *)&scia_ptr->gfx_drv_desc;
sfbp_ioseg_ptr  = (IOSEG *)drv_desc_ptr->ioseg_ptr ;
data_ptr        = (SFBP *)drv_desc_ptr->data_ptr;
control         = (struct sfbp_control *)sfbp_ioseg_ptr->sfbp$a_reg_space;
gfx_func_ptr    = (struct gfx_func_block *)&data_ptr->sfbp$r_graphics_func;
parse_func_ptr  = (struct parse_func_block *)&data_ptr->sfbp$r_parse_func;
bt_func_ptr     = (struct bt_func_block *)&data_ptr->sfbp$r_bt_func;
cp_func_ptr     = (struct cp_func_block *)&scia_ptr->console_func;
ccv                  = (CCV *)ccv_vector;

if (data_ptr->sfbp$l_ramdac == bt463)
  s = (*parse_func_ptr->pv_test_dispatch) (scia_ptr,data_ptr, argc, argv, slot , ccv , 
                ramdac_table_463_tests ,sizeof (ramdac_table_463_tests) );
    
else if (data_ptr->sfbp$l_ramdac == bt485)
  s = (*parse_func_ptr->pv_test_dispatch) (scia_ptr,data_ptr, argc, argv, slot , ccv , 
                ramdac_table_485_tests ,sizeof (ramdac_table_485_tests) );
    
return (s);
}

/*++
* ===========================================================================
* = tgatest_cleanup - sfbp Diagnostic Test Dispatcher =
* ===========================================================================
*
* OVERVIEW:
* 
* FORM OF CALL:
*
* RETURNS:
*
* ARGUMENTS:
*
* GLOBALS:
*       none
*
* FUNCTIONAL DESCRIPTION:
*
* RETURN CODES:
*
* CALLS:
*      
* SIDE EFFECTS:
*
* ALGORITHM:
*   
--*/

int tgatest_cleanup()    
 {                                                         
 struct IOB	*iob_ptr;        
 struct PCB 	*pcb;
 externals  	*eptr;
	         
 pcb = getpcb();
 eptr= (externals *)pcb->pcb$a_globals;                               

 diag_rundown();   
                                                
 if ((iob_ptr = (struct IOB *)find_pcb_iob()) != NULL)           
    {                                                 
    remq (&iob_ptr->pcbq);                             
    remq_iob (&iob_ptr->sysq);           
    }                 
 
 return (1);

 }                            


/*++
* ===========================================================================
* = tgatest_extend - =
* ===========================================================================
*
* OVERVIEW:
* 
* FORM OF CALL:
*
* RETURNS:
*
* ARGUMENTS:
*
* GLOBALS:
*       none
*
* FUNCTIONAL DESCRIPTION:
*
* RETURN CODES:
*
* CALLS:
*
* SIDE EFFECTS:
*
* ALGORITHM:
*   
--*/
int tgatest_extend ()
 {                                            
 struct PCB *pcb;
 externals *eptr;
 int expect,actual,failadr;
                                     
 pcb     = getpcb(); 
 eptr    = (externals  *)pcb->pcb$a_globals;
 expect  = eptr->expect;
 actual  = eptr->rcvd;
 failadr = eptr->failadr;                

 err_printf (msg_d_exp_rcd,expect,actual);
 err_printf (msg_d_ext_err_fa,failadr);

 return (1);
 
 }
                  

/*++
* ===========================================================================
* = tga_load_error_info - =
* ===========================================================================
*
* OVERVIEW:
* 
* FORM OF CALL:
*
* RETURNS:
*
* ARGUMENTS:
*
* GLOBALS:
*       none
*
* FUNCTIONAL DESCRIPTION:
*
* RETURN CODES:                        
*
* CALLS:
*
* SIDE EFFECTS:
*
* ALGORITHM:
*                                     
--*/

int tga_load_error_info(int err_num, int param [])
 {
 struct PCB		*pcb;                    
  externals	      	*eptr;
 struct IOB		*iob_ptr;    	
                                   
 pcb = getpcb();
 eptr= (externals *)pcb->pcb$a_globals;
                 
 if ((iob_ptr = (struct IOB *)find_pcb_iob()) != NULL)           
       {
        sprintf(pcb->pcb$b_dev_name,"brd%d",addr_to_brd(param[0]));
	strncpy(iob_ptr->name, pcb->pcb$b_dev_name, MAX_NAME);
        }	
                                              
 eptr->failadr = param[0];            
 eptr->expect  = param[1];                   
 eptr->rcvd    = param[2];

 return (1);
}


/*+
* ===========================================================================
* = parse_command_line - Parse the command line =
* ===========================================================================
*
* OVERVIEW:
*       This is used to parse the command line.
* 
* FORM OF CALL:
*       parse_command_line (scia_ptr,data_ptr,argc,argv,ccv,slot)
*
* ARGUMENTS:
*       scia_ptr  - driver area pointer
*       data_ptr  - driver data area pointer
*       argc      - argument count
*       argv      - argument array of ptrs
*       ccv       - callback vector
*       slot      - my slot
*
* FUNCTIONAL DESCRIPTION:
*       Look for - qualifiers for the diagnostics.
*
*   
--*/

int parse_command_line (SCIA *scia_ptr,SFBP *data_ptr,int argc,char **argv,CCV *ccv_vector,int slot)

{
char     test_console[20];
char    *ptr;
register int     i,j,flash_flag=0,crtslot;


ccv = (CCV *)ccv_vector;

data_ptr->sfbp$l_verbose         = 0;
data_ptr->sfbp$l_font_char       = 0;
data_ptr->sfbp$l_subtest_start   = 0;
data_ptr->sfbp$l_subtest_end     = 0;
data_ptr->sfbp$l_test_lines      = 0;
data_ptr->sfbp$l_white_on_black  = TRUE;
data_ptr->sfbp$l_test_loop	 = FALSE;
    
if
 (argc>2)
 {
 if (parse_check(argv,argc,"t",&ptr))subtest_check(argv,argc,ptr);	
 if (parse_check(argv,argc,"v",&ptr))data_ptr->sfbp$l_verbose=1;
 if (parse_check(argv,argc,"f",&ptr))data_ptr->sfbp$l_font_char=(int)*ptr;
 if (parse_check(argv,argc,"n",&ptr))data_ptr->sfbp$l_test_lines=strtol(ptr,0,0);
 if (parse_check(argv,argc,"c",&ptr))data_ptr->sfbp$l_line_test_mode=strtol(ptr,0,0);
 if (parse_check(argv,argc,"b",&ptr))data_ptr->sfbp$l_white_on_black=0;
 if (parse_check(argv,argc,"l",&ptr))data_ptr->sfbp$l_test_loop=TRUE;
 }                                   
                                     
 if (!data_ptr->sfbp$l_test_lines || 
     data_ptr->sfbp$l_test_lines>300)data_ptr->sfbp$l_test_lines=MAX_LINES;                    

 return (1);
}



/*+
* ===========================================================================
* = parse_check - Parse the command line =
* ===========================================================================
*
* OVERVIEW:
*       This is used to parse the command line.
* 
* FORM OF CALL:
*       parse_check (argv,argc,param,fptr)
*
* RETURNS:
*       0 for success else -1 for failure
*
* ARGUMENTS:
*       argc      - argument count
*       argv      - argument array 
*       param     - parameter to pass
*       fptr      - address to save pointer to token
*
* FUNCTIONAL DESCRIPTION:
*       Look for "-x" qualifier and set ptr to it.
*
--*/

int parse_check (char **argv,int argc,char *param,char **fptr)
{
register int    i,found=0;
char    *ptr;

for (i=0,ptr=argv[i];i<argc;i++,ptr=argv[i])    
  if (*ptr++=='-'&&*ptr++== *param){found=TRUE;*fptr = ptr;break;}

return (found);
}


/*+
* ===========================================================================
* = subtest_check - Parse the command line =
* ===========================================================================
*
* OVERVIEW:
*       This is used to get subtest start and end.
* 
* FORM OF CALL:
*       subtest_check (argv,argc,param,fptr)
*
* ARGUMENTS:
*       argc      - argument count
*       argv      - argv [0]="t", argv[1]="#/testname" argv[n] = -v -t -n -f -c -s -d -b
*       param     - parameter to pass
*       fptr      - address to save pointer to token
*
* FUNCTIONAL DESCRIPTION:
*       This is used to get subtest start and end.
*
*   
--*/

int subtest_check (char **argv,int argc,char *ptr)
{
register int    i,found=0;
char    *save_ptr;
char    temp[20];

save_ptr = ptr;

for (i=0;i<sizeof(temp);i++)temp[i]=0;

for (i=0;i<argc;i++)
  if (*ptr++ == ':')break;

if (i<argc){strncpy (temp,save_ptr,i);}
else {strcpy (temp,save_ptr);}
data_ptr->sfbp$l_subtest_start = strtol (temp,0,0);
data_ptr->sfbp$l_subtest_end   = strtol (ptr,0,0);
if (data_ptr->sfbp$l_subtest_end==0)
        data_ptr->sfbp$l_subtest_end=data_ptr->sfbp$l_subtest_start;

}



/*+
* ===========================================================================
* = test_dispatch - sfbp Diagnostic Test Dispatcher =
* ===========================================================================
*
* OVERVIEW:
*       This is the test dispatcher
* 
* FORM OF CALL:
*       test_dispatch (SCIA *scia_ptr,SFBP *data_ptr, int argc, char **argv, 
*                int slot, CCV *ccv_vector, TABLE_ENTRY *test_ptr,int entry_size)
*
* ARGUMENTS:
*       scia_ptr  - driver area pointer
*       data_ptr  - driver data area pointer
*       argc      - argument count >=1
*       argv      - argument array 
*       slot      - the slot number
*       ccv       - callback vector array 
*       test_ptr  - pointer to subtest entry point.
*       entry_size- size of data structure table with subtests
*
--*/
int test_dispatch (SCIA *scia_ptr,SFBP *data_ptr, int argc, char **argv, 
                int slot, CCV *ccv_vector, TABLE_ENTRY *test_ptr,int entry_size)
{
register  int       head,help,length,done=0,s=1,i,j;
register  TABLE    *ptr;
register  char     *pos;

drv_desc_ptr    = (struct drv_desc   *)&scia_ptr->gfx_drv_desc;
sfbp_ioseg_ptr  = (IOSEG *)drv_desc_ptr->ioseg_ptr ;
control         = (struct sfbp_control *)sfbp_ioseg_ptr->sfbp$a_reg_space;
gfx_func_ptr    = (struct gfx_func_block *)&data_ptr->sfbp$r_graphics_func;
parse_func_ptr  = (struct parse_func_block *)&data_ptr->sfbp$r_parse_func;
bt_func_ptr     = (struct bt_func_block *)&data_ptr->sfbp$r_bt_func;
cp_func_ptr     = (struct cp_func_block *)&scia_ptr->console_func;

data_ptr->sfbp$l_current_head= 0;

/* Clear any left over signature errors */
for (i=0;i<4;i++)data_ptr->sfbp$l_signature_error[i]=0;


for (head=0;head<data_ptr->sfbp$l_heads;head++)
{
 data_ptr->sfbp$l_current_head=head;
 for (j=0;!done && j*sizeof(TABLE_ENTRY)<sizeof(entry_size);j++) 
  {
    ptr = test_ptr[j].ptr;
    length = test_ptr->size;
    for(i=0,s=1;!done && s&1 && i* sizeof(TABLE)<length;i++,ptr++) 
        {
        /* Check for test name          */
        /* Also check for init or cnfg  */
        pos = (char *)argv[1];
        if ( (argc>1 && strncmp(ptr->name,argv[1],3)==0) ||
             strncmp(ptr->name,argv[0],3)==0)
                {
                s=  subtest_dispatch(scia_ptr,data_ptr,argc,argv,slot,ccv,		                
	                ptr->f,sizeof(DISPATCH_TYPE),ptr->fsize,ptr->colormap,ptr->clear_memory);
                done=1;
                }
        }
  }
}

return (s);
}




/*+
* ===========================================================================
* = subtest_dispatch - Subtest Dispatcher = 
* ===========================================================================
*
* OVERVIEW:
*       Subtest Dispatch 
* 
* FORM OF CALL:
*       subtest_dispatch ( scia_ptr, data_ptr, argc, argv, slot, ccv,table,type_size,table_size)
*
* ARGUMENTS:
*       scia_ptr   - shared driver area pointer
*       data_ptr  - driver data area pointer
*       argc      - argument count >=1
*       argv      - argv [0]="t", argv[1]="#/testname" argv[2]=flags
*       slot      - the slot number
*       ccv       - callback ccv array 
*	type_size - Size of data structure type
*	table_size- Size of table (test dependent)
*	
* FUNCTIONAL DESCRIPTION:
*       Call Subtests from given table
*
--*/
int subtest_dispatch (SCIA *scia_ptr,SFBP *data_ptr,int argc,char **argv,int slot,CCV *ccv_vector,
      DISPATCH_TYPE test_table[],int type_size,int table_size,int colormap,int clear_memory )

{
int      first=1,i,s=1,param[3],subtest,subtest_start,subtest_end;
int      NumBytes,InitCommand=0,offset,CnfgCommand=0,TotalLen=0,help=0,flags,print_text=0,continue_flag=1,current_subtest;
char     *ptr,inchar;
char     local[20];
static   int gray_color[3]={1,1,1};
ccv = (CCV *)ccv_vector;

/* t 2/int -dls */
print_text = data_ptr->sfbp$l_alt_console ? data_ptr->sfbp$l_list_tests : 0;

if (argc==1 && strncmp(argv[0],"cnfg",4)==0)
        CnfgCommand=1;

if (argc>2)
  {
  if (parse_check(argv,argc,"x",&ptr)  ||
      data_ptr->sfbp$l_print    ||
      (data_ptr->sfbp$l_verbose&&data_ptr->sfbp$l_alt_console) )print_text=1;

  if (argv[2] && *argv[2]=='?')
   {
   help=1;
   for (i=0; i*type_size < table_size ; i++ )
        printf ("%s \n",test_table[i].text);
   }
  } 

do 
{
if (!help)
 {
  subtest_start   = data_ptr->sfbp$l_subtest_start;
  subtest_end     = data_ptr->sfbp$l_subtest_end;
  subtest         = current_subtest = subtest_start;
  if (current_subtest==0)current_subtest=1;
  if (subtest >=0 && (subtest-1) *type_size < table_size )
   {
    for (first=1,i=current_subtest-1,s=1;continue_flag && 
        s&1&&i*type_size < table_size ;i++,current_subtest++,first=0)
    {
    if ((current_subtest==test_table[i].subtest||subtest==0) &&
        (current_subtest <= subtest_end  || subtest==0))
    {

    if((data_ptr->sfbp$l_verbose&&data_ptr->sfbp$l_alt_console)||print_text)
        {printf ("%s\n",test_table[i].text);}

    param[0]=param[1]=param[2]=0;
   
    /* Line Test uses an 888 colormap	    */
    if ( first && 
   	 data_ptr->sfbp$l_module != hx_8_plane && 
         strncmp (argv[1],"line",3)==0 )
	        {
		(*bt_func_ptr->pv_bt_load_888_color)(scia_ptr,data_ptr,gray_color);
		(*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_plane_mask,M1,LW);
		(*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);
		}
		
    /* Some Test Modules Require Vram Clear */
    /* Which are signature testers          */
    /* So clear a screen worth              */
    /* Just use 1284x1024 worst case        */


    if (clear_memory && first && !CnfgCommand &&
        !InitCommand && data_ptr->sfbp$l_display_clear)
         {
         /* Some Test Modules Require Vram Clear */
         /* Which are signature testers          */
         /* So clear a screen worth              */
         /* Just use 1284x1024 worst case        */
         offset   = data_ptr->sfbp$l_cursor_offchip ? 1024 : data_ptr->sfbp$l_console_offset;
         NumBytes = (1284*(1024+10)*data_ptr->sfbp$l_bytes_per_pixel),
         (*gfx_func_ptr->pv_fill_vram)(scia_ptr,data_ptr,offset,0,NumBytes);

	 /* Flush 8 Mbytes Z if testing that     */
         if (data_ptr->sfbp$l_module==hx_32_plane_z &&
             data_ptr->sfbp$l_test_zbuffer)
                sfbp$$fill_video_memory(scia_ptr,data_ptr,
                        (char *)1024,0,(data_ptr->sfbp$l_vram_size/2)-1024);
         }                        
    do
     {
     if (test_table[i].rtn)
              {
              data_ptr->sfbp$l_fault_class = test_table[i].fault_class;
              data_ptr->sfbp$l_error_type  = test_table[i].error_class;
              s= (*test_table[i].rtn)(argc,argv,ccv,scia_ptr,data_ptr,param);
              }
             
     /* Restore the Mode Register */
     (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_mode,data_ptr->sfbp$l_default_mode,LW);
     (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_boolean_op,data_ptr->sfbp$l_default_rop,LW);
     (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_plane_mask,M1,LW);
     (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);

     if (!(s&1))
        s=(*gfx_func_ptr->pv_sfbp_error)(scia_ptr,data_ptr,slot,test_table[i].text,
                        param,data_ptr->sfbp$l_fault_class,data_ptr->sfbp$l_error_type,
                        (CHIP8 *)test_table[i].fault8_data_ptr,
                        (CHIP32 *)test_table[i].fault32_data_ptr,
                        (CHIP32Z *)test_table[i].fault32z_data_ptr);
        

     if (data_ptr->sfbp$l_verbose)
        {
        if (data_ptr->sfbp$l_alt_console)
                {
                {printf("Continue ? ");}
                gets (local);
                }else {getchar();printf ("\n");}
         }
     if (local[0]=='n' || local[0]=='N')continue_flag=0;
     } while ( (!(s&1) && data_ptr->sfbp$l_loope) && (continue_flag==1) ) ;
   }
 }
 
 /* Ramdac Tests Require ColorMap Reload Since They Test the  */
 /* Cursor Colors and All of that Stuff                       */
 if (colormap) 
        {
        (*bt_func_ptr->pv_bt_init) (scia_ptr,data_ptr);
        (*bt_func_ptr->pv_bt_load_332_color)(scia_ptr,data_ptr);
        (*bt_func_ptr->pv_bt_pos_cursor)(scia_ptr,data_ptr,
                data_ptr->sfbp$l_cursor_row,
                data_ptr->sfbp$l_cursor_column,FALSE);
        }

  (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);

  }else {printf ("Subtest Out of Range\n");return(0);}
 }
} while (!help && data_ptr->sfbp$l_test_loop && (!io_poll()) ); 

 (*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_int_status,0x1f,0,LW);
 (*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);

return (s);
 
}


/*+
* ===========================================================================
* = sfbp$$signature_mode_test - The signature mode analysis test =
* ===========================================================================
*
* OVERVIEW:
*       Signature Analysis Test 
* 
* FORM OF CALL:
*       sfbp$$signature_mode_test (argc,argv,ccv,scia_ptr,data_ptr,param)
*
* ARGUMENTS:                                                            
*       argc        - argument count
*       argv        - argument array
*       ccv         - callback vector
*       scia_ptr    - shared driver pointer
*       data_ptr    - driver data pointer
*       param       - parameter array for error information
*
* FUNCTIONAL DESCRIPTION:
*       Common signature invokation. This is only for 130
*
--*/
int     sfbp$$signature_mode_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr,
                SFBP * data_ptr, int param [],struct signature_type *signature)

{
unsigned char     *value,*xlo,temp;
int      SavePoll,temp1,temp2,ipl;
register int Odd,NewPixel=0,PixelCount,error=0,i,j,s=1,pixel,sigcnt=0;
static   unsigned actual [3]; 
static   unsigned char seed [3] = {0xff,0xff,0xff};
static   char     *pixel_state[4]={"A","B","C","D"};
static   char     *color[3]     = {"R","G","B"};
                              
if (data_ptr->sfbp$l_ramdac==bt463)
 {                            
 SavePoll = data_ptr->sfbp$l_int_poll;
                              
 if (  (data_ptr->sfbp$r_mon_data.sfbp$l_sold_freq == 119 ||
        data_ptr->sfbp$r_mon_data.sfbp$l_sold_freq == 130 ) && 
        data_ptr->sfbp$r_mon_data.sfbp$l_v_scanlines>512) 
                                 
  {                              
  data_ptr->sfbp$l_int_poll = 1; 
  (*bt_func_ptr->pv_bt_pos_cursor)(scia_ptr,data_ptr,
        data_ptr->sfbp$l_cursor_max_row/2,                      
        data_ptr->sfbp$l_cursor_max_column/2,FALSE);            
                                    
  ipl = mfpr_ipl();                 
  mtpr_ipl(IPL_SYNC);                          
                                    
 /* Wait for Vertical Retrace twice to make sure */
 for (i=0;i<2;i++)
  {
  (*gfx_func_ptr->pv_clear_int_req)   (scia_ptr,data_ptr,end_of_frame);
  (*bt_func_ptr->pv_bt_wait_interrupt)(scia_ptr,data_ptr,FALSE,0,end_of_frame,FALSE);
  }

  for (pixel=error=0;pixel<4;pixel++)
   {                             
   mtpr_ipl(IPL_SYNC);                          
   /* Get a signature for each pixel load	*/
   (*gfx_func_ptr->pv_clear_int_req)   (scia_ptr,data_ptr,end_of_frame);
   (*bt_func_ptr->pv_bt_wait_interrupt) (scia_ptr,data_ptr,TRUE,pixel,
                 end_of_frame,OutPutSignature);                               
   mtpr_ipl(ipl);
   data_ptr->sfbp$l_signature_error[pixel]=FALSE;
   for (i=0;i<3;i++)             
   {                             
   if (data_ptr->sfbp$r_snap.sfbp$r_sig[i].pixel[pixel] !=
       (signature->sfbp$r_sig[i].pixel[pixel]) )
         {                       
         temp1     = (*bt_func_ptr->pv_bt_read_lo)(scia_ptr);
         temp2     = (*bt_func_ptr->pv_bt_read_hi)(scia_ptr);
         param[0]  = sfbp$k_a_sfbp_reg_space | ((temp1|(temp2<<8))-1);
         param[1]  = signature->sfbp$r_sig[i].pixel[pixel];
         param[2]  = data_ptr->sfbp$r_snap.sfbp$r_sig[i].pixel[pixel];
         error     = 1;                                                 
         data_ptr->sfbp$l_signature_error[pixel]=TRUE;
         printf ("\tSignature pixel %s[%s]: exp 0x%2x act 0x%2x\n",
                                pixel_state[pixel],color[i],param[1]&0xff,param[2]&0xff);
         }else sigcnt++;
    }                    
   if (error)printf ("\n");
   }
 }
 data_ptr->sfbp$l_int_poll = SavePoll;
 }
 
return (error ? 0 : 1);
}


/*+
* ===========================================================================
* = sfbp$$input_signature_mode_test - The signature mode analysis test =
* ===========================================================================
*
* OVERVIEW:
*       Input Signature Analysis Test (463 only)
* 
* FORM OF CALL:
*       sfbp$$input_signature_mode_test (argc,argv,ccv,scia_ptr,data_ptr,param)
*
* ARGUMENTS:
*       argc        - argument count
*       argv        - argument array
*       ccv         - callback vector
*       scia_ptr    - shared driver pointer
*       data_ptr    - driver data pointer
*       param       - parameter array for error information
*
* FUNCTIONAL DESCRIPTION:
*       Common input signature testing.
*
--*/
int     sfbp$$input_signature_mode_test (int argc,char **argv,CCV *ccv,SCIA *scia_ptr,
                SFBP * data_ptr, int param [],struct signature_type *signature)
                              
{                             
unsigned char     *value,*xlo,temp;
int      SavePoll,temp1,temp2,ipl;
register int Odd,NewPixel=0,PixelCount,error=0,i,j,s=1,pixel,sigcnt=0;
static   unsigned actual [3];
static   unsigned char seed [3] = {0xff,0xff,0xff};
static   char     *pixel_state[4]={"A","B","C","D"};
static   char     *color[3]     = {"R","G","B"};
               
if (data_ptr->sfbp$l_ramdac==bt463)
 {             
 SavePoll = data_ptr->sfbp$l_int_poll;
   
  /* Input Signatures Only Work up to about 100 Mhz	*/
  /* So I may just comment out this functionality now   */
  if ( data_ptr->sfbp$r_mon_data.sfbp$l_sold_freq == 74 )
  {
  data_ptr->sfbp$l_int_poll = 1;
  (*bt_func_ptr->pv_bt_pos_cursor)(scia_ptr,data_ptr,
        data_ptr->sfbp$l_cursor_max_row/2,
        data_ptr->sfbp$l_cursor_max_column/2,FALSE);
  
  ipl = mfpr_ipl();              
  mtpr_ipl(IPL_SYNC);                          
   
 /* Wait for Vertical Retrace twice to make sure */
 for (i=0;i<2;i++)
  {
  (*gfx_func_ptr->pv_clear_int_req)   (scia_ptr,data_ptr,end_of_frame);
  (*bt_func_ptr->pv_bt_wait_interrupt)(scia_ptr,data_ptr,FALSE,0,end_of_frame,FALSE);
  }
  for (pixel=error=0;pixel<4;pixel++)
   {                          
   mtpr_ipl(IPL_SYNC);                          
   /* Get a signature for each pixel load	*/
  (*gfx_func_ptr->pv_clear_int_req)   (scia_ptr,data_ptr,end_of_frame);
   (*bt_func_ptr->pv_bt_wait_interrupt) (scia_ptr,data_ptr,TRUE,pixel,
                 end_of_frame,InPutSignature);                               
   mtpr_ipl(ipl);
   data_ptr->sfbp$l_signature_error[pixel]=FALSE;
   for (i=0;i<3;i++)          
   {                          
   if (data_ptr->sfbp$r_snap.sfbp$r_sig[i].pixel[pixel] !=
       (signature->sfbp$r_sig[i].pixel[pixel]) )
         {                    
         temp1     = (*bt_func_ptr->pv_bt_read_lo)(scia_ptr);
         temp2     = (*bt_func_ptr->pv_bt_read_hi)(scia_ptr);
         param[0]  = sfbp$k_a_sfbp_reg_space | ((temp1|(temp2<<8))-1);
         param[1]  = signature->sfbp$r_sig[i].pixel[pixel];
         param[2]  = data_ptr->sfbp$r_snap.sfbp$r_sig[i].pixel[pixel];
         error     = 1;
         data_ptr->sfbp$l_signature_error[pixel]=TRUE;
         printf ("\tSignature pixel %s[%s]: exp 0x%2x act 0x%2x\n",
                                pixel_state[pixel],color[i],param[1]&0xff,param[2]&0xff);
         }else sigcnt++;
    }
   if (error)printf ("\n");
   }
 }
 data_ptr->sfbp$l_int_poll = SavePoll;
 }

return (error ? 0 : 1);
}

