/* file:	memtest.c
 *     
 * Copyright (C) 1991 by
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
 *      Cobra Firmware
 *              
 *  MODULE DESCRIPTION:
 *
 *      This exerciser contains a graycode memory test, a march memory 
 *	test, a random memory test, and a victim block test.  For the
 *	first test a graycode pattern and inverse graycode pattern are 
 *	written, read and verified for the specified address range in 
 *	test 1.  For the second test, a marching pattern and 
 *	inverse marching pattern are written, read, and verified for the 
 *	specified address range.   The third test will test random addresses 
 *	within the specified range with random data of random length.  The 
 *	fourth test will perform block writes of data, victimize the data, and 
 * 	then read back the block and verify it.  This will be performed 
 * 	for the specified address.  (For more details of each test see
 *	the overview under the memtest description.)
 *
 *	The march, random and victim tests are included as stubs by default
 *	and optionally in separate modules under the names MEMTEST*.C.
 *
 *  AUTHORS:
 *
 *      Harold Buckingham               
 *                     
 *  CREATION DATE:
 *                                                          
 *      19-Apr-1991
 *
 *  MODIFICATION HISTORY:          
 *
 *	bobf	 9-Aug-1994	Added call to log_error in Load_Error_Info
 *				to log errors to NVRAM; inserted calls to
 *				Sio_Error and Sio_Ext_Error where err_printf's
 *				displayed failure information so that failures
 *				get logged to NVRAM (Medulla platform only).
 *
 *	cto	24-Jan-1994	Break out the Laser memtests into separate 
 *				files, change mem_ex references.
 *
 *	cto	18-Jan-1994	Consolidate previous comments - merge Laser
 *				memtest with common console.
 *
 *--                   
 */                                       

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include	"cp$src:kernel_def.h"          
#include	"cp$src:dynamic_def.h"
#include 	"cp$src:msg_def.h"
#include	"cp$src:parse_def.h"
#include      	"cp$src:diag_groups.h"
#include 	"cp$src:diag_def.h"
#include 	"cp$src:ev_def.h"
#include	"cp$src:mem_def.h"
#include	"cp$src:memtest_def.h"
#include "cp$inc:kernel_entry.h"
#include "cp$inc:prototypes.h"

#if MEDULLA
#include	"cp$src:medulla_errors.h"
#endif

#define DEBUG 0

/* All defines are now in memtest_def.h   cto 31-Jan */

extern struct ZONE defzone;		/* firmware heap header	*/
extern struct ZONE *memzone;    	/* heap header	*/
extern int spl_kernel;                            
extern int cpu_mask;
extern unsigned int poweron_msec [2];


void load_error_info(int err_num,int data,int read_data,UINT addr);

static void    	   	memtest_extend(void);
static void    		memtest_cleanup(void);
static void    		cleanup_no_completion(void);
             
/* 4 stubbed tests */
extern int		memtest_gray(void);       
extern int		memtest_march(void);       
extern int		memtest_random(void);       
extern int		memtest_victim (void);
#if TURBO
extern int		memtest_gray_lw(void);       
#endif

extern int		stl_ldl(__int64 *wrt_addr, u_int data);
extern int		graycode_memtest(u_int *start_addr, u_int len, int bar,
				int init, int brd_mask, int pass);

extern char *		find_bad_mem(u_int addr_hi, u_int addr_lo);
extern void 		GetCpuName(char *name);

#if !MODE64
extern int		do_stq_ldq(__int64 *wrt_addr, u_int *wrt_data,
			        u_int *read_data_upper, u_int *read_data_lower);
#else
extern int		stq_ldq(u_int *wrt_addr, u_int data);
#endif

#if !MODULAR
extern int		diag_init(int * argc, char *argv[], 
	      			  struct QSTRUCT * quals, 
       	       			  char * new_opt_string,int qscan_done);
extern int	   	report_error();                
extern void	     	diag_start();
#endif

extern void 	       	*dyn$_malloc();                  
extern int     	     	killpending();                     
                                                                
extern void  		create_iob ();
extern void  		remq_iob();             
                                                 
#if !MODULAR
extern int		report_error_locked();                   
extern int		report_error_release();
#endif

extern void		krn$_readtime(INT *);

/*------------------------------------------------------------------------------
  Dispatch Table.
  diag_start will use this table to call each test listed.
------------------------------------------------------------------------------*/
                                                                 
static struct DIAG_DIS_TABLE dispatch_table[] = {
    {memtest_gray, m_grp_mfg|m_grp_fld},       
    {memtest_march, m_grp_mfg},       
    {memtest_random, m_grp_mfg},                      
    {memtest_victim, m_grp_mfg},       
#if TURBO
    {memtest_gray_lw, m_grp_mfg},       
#endif
    {(int)NULL, (int)NULL}};            

/*------------------------------------------------------------------------------
  Program External Variables.                                
                                                                 
  Any desired external variables should be placed in a structure and the
  structure should define a typedef.  The initialization code can then
  dynamically allocated space for these variables.
                                                       
------------------------------------------------------------------------------*/
                                                   
                                                 
/*+                                                                  
 * ============================================================================
 * = memtest - Exercise/test a section of memory.			      =
 * ============================================================================
 *                                                
 * OVERVIEW:                                     
 *                                                  
 *  Memtest may be run on any specified address.  If the -z option is not
 *  included(default) than the address will be verified and allocated from
 *  the firmware's memory zone.  If the -z qualifier is included the test
 *  will be started without verification of the starting address.
 *
 *  When a starting address is specified, the memory is allocated beginning
 *  at the starting address -32 bytes for the length specified.  The extra
 *  32 bytes that are allocated are reserved for the allocation header
 *  information.
 *  Therefore, if a starting address of 0xa00000 and a length of 0x100000 is
 *  requested, the area from 0x9fffe0 through 0xb00000 is reserved.  This is
 *  transpararent to the user, but may be confusing if the user attempts to
 *  begin two memtest processes simultaneously with one beginning at 0xa00000
 *  for a length of 0x100000 and the other at 0xb00000 for a length of
 *  0x100000.  The second memtest process will state to the user that it is
 *  "Unable to allocate memor of length 100000 at starting address b00000".
 *  Instead, the second process should use the starting address of 0xb00020.
 *  
 *  Memtest Test 1 uses a graycode algorithm to test a specified section of
 *  memory.  The graycode algorithm used is:  data = (x>>1)^x. Where x is an 
 *  incrementing value.                                      
 *                                             
 *  Three passes are made of the memory under test.    
 *  The first pass writes alternating graycode inverse graycode to each 
 *  4 longwords. This will cause many data bits to toggle between each 
 *  16 byte write.
 *
 *  For example graycode patterns for a 32 byte block would be:
 *
 *  Graycode(0)			00000000 
 *  Graycode(1)			00000001
 *  Graycode(2)			00000003
 *  Graycode(3)			00000002
 *  Inverse Graycode(4)		FFFFFFF9 
 *  Inverse Graycode(5)		FFFFFFF8
 *  Inverse Graycode(6)		FFFFFFFA
 *  Inverse Graycode(7)		FFFFFFFB
 *                                                          
 *  The second pass reads each location verifies the data and writes the 
 *  inverse of the data.  The read-verify-write is done one longword at a 
 *  time. This will cause all data bits to be written as a one and zero.
 *                                                      
 *  The third pass reads and verifies each location.                        
 *                                                           
 *  The -f "fast" option can be specifed so the explicit data verify sections
 *  of the second and third loops is not performed.  This will not catch
 *  address shorts but will stress memory with a higher throughput.  The
 *  ECC/EDC logic can be used to detect failures.
 *
 *  Memtest Test 2 uses a marching 1's/0's algorithm to test a specified
 *  section of memory.  The same range can be tested as in the graycode,
 *  test 1.  The default data pattern used by this test is 0x55555555 and
 *  its inverse 0xAAAAAAAA.  The data pattern can be altered by using the
 *  -d qualifier.  The pattern entered and its compliment will then be
 *  used instead.
 *
 *  Three passes are also made of the memory under test.  The first pass
 *  will write the data pattern entered (or default) beginning at the 
 *  starting address and marching through for the entire length specified.
 *  
 *  The next pass will begin again at the starting address and read the 
 *  previously written data pattern and write back its inverse.  This
 *  is done a longword at a time for the entire specified length.
 *
 *  The last pass will being at the end of the testing region and again 
 *  read back the previously written inverse pattern and write back 0's.
 *  This will also be performed a longword at a time decrementing up through
 *  memory until the starting address is reached.
 *
 *  The random test, Memtest Test 3, will perform writes with random data
 *  to random addresses using random data size, lengths, and alignments.
 *  The run time of the random test may be noticeable longer than that of
 *  the other tests because it requires two calls to the console firmware's
 *  random number generator every time data is written. 
 *
 *  The random test will access every memory location within the boundaries
 *  specified by the -sa and -l qualifiers (as long as the length is less than
 *  8MEG -- until a 64 bit compiler exists -- with lengths greater than 8 MEG
 *  a modulo function is required on the seed and therefore some addresses may
 *  get repeated and some not tested at all).  It first will obtain an address
 *  index into the Linear Congruential Generator structure dependent on the
 *  length specified.  It will obtain the data index as a function of the
 *  entered random data seed and the maximum 32 bit data pattern.  Using the
 *  address index and an initial address seed of 0, the random number generator
 *  is called to obtain a random address.  It is then called again, using the
 *  data index and initial user entered data seed, (-rs qualifier), or default 
 *  of 0, to get the longword of data to use in testing.  The lower bit of the
 *  random data returned will also be used to determine whether to perform 
 *  longword or quadword transactions.  (Using the lower bit merely saves 
 *  another call to the random function to help speed up the test).  The data 
 *  is then stored to the random address, and memory barrier is performed to 
 *  flush it out to the B-Cache and then a read is done to read it back in.  A 
 *  compare is then done on the data written and the data read.  In the case 
 *  of quadword writes and reads, the longword of random data is shifted left 
 *  by 32 and or'd with the original's compliment to form the quadword.
 *
 *  Memtest test 4 is a DVT test at this time.  Test 4 will require that an
 *  actually memory module be present in the system.  The user must first set
 *  up a block of data he/she wishes to use in the test.  The address of this
 *  block of data will be read as an input to the test using the -ba qualifier.
 *  (default is block of data containing 4 longwords of 0xF's, then 4 
 *  longwords of 0's, then 4 longwords of 0xF's and lastly 4 longwords of 0's.)
 *
 *  First, the test will perform a write of the block of data to the specified 
 *  starting address.  It will then add a bcache offset to the starting address
 *  and perform another write of arbitrary data.  This will cause the original
 *  data to be 'victimized' to memory.  A read will then be performed of the
 *  original starting address and verified that it is correct.  The starting
 *  address is then incremented by a block and the write/write/read procedure
 *  repeated for the specified length of memory.
 *
 *  If MEMTEST is used to test large sections of memory,  it may take a
 *  while for testing to complete.  If a ^c or kill PID is done in the 
 *  middle of testing, MEMTEST may not abort right away.  For speed reasons,
 *  a check for a ^c or kill is done outside of any test loops.  If this
 *  is not satisfactory, the user may run concurrent MEMTEST processes in 
 *  the background with shorter lengths within the target range.
 *
 *   COMMAND FMT: memtest 0 0 0 memtest
 *
 *   COMMAND FORM:                                  
 *        
 *	memtest ( [-sa <start_address>] [-ea <end_address>] [-l <length>]
 *		  [-bs <block_size>] [-i <address_inc>] [-p <pass_count>]
 *		  [-d <data_pattern>] [-rs <random_seed>] [-ba <block_address>]
 *		  [-t <test_mask>] [-se <soft_error_threshold>]
 *		  [-g <group_name>] [-rb] [-f] [-m] [-z] [-h] [-mb] )
 *                                                                           
 *   COMMAND TAG: memtest 0 RXBZ memtest
 *
 *   COMMAND ARGUMENT(S):
 *                                                  
 *      None.                                 
 *                                             
 *   COMMAND OPTION(S):          
 *                                            
 *     -sa    - start address, default is first free space in memzone.
 *                                                           
 *     -ea    - end address, default is start address plus length size.
 *                                                                           
 *     -l     - length of section to test in bytes, default is the zone size
 *		with the -rb option and the block_size for all other tests.
 *     		l has precedence over ea.
 *
 *     -bs    -	Block (packet) size in bytes in hex, default 8192 bytes
 *              This is only used for the random block test. For all other
 *		tests the block size equals the length.
 *                                                                
 *     -i     - Specifies the address increment value in longwords.
 *		This value will be used to increment the address through
 *		the memory to be tested.  The default is 1 (longword).
 *		This is only implemented for the graycode test.
 *		An an address increment of 2 tests every other longword.
 *		This option is useful for multiple CPUs testing the
 *		same physical memory.
 *
 *     -p     - passcount; if = 0 then run forever or until ^C; default = 1 
 *		                                  
 *     -t     - test mask; default = run all tests in selected group
 *		                                  
 *     -g     - group name
 *		                                  
 *     -se    - soft error threshold
 *		                                  
 *     -f     -	fast; If -f is included in the command line the data compare
 *	       	is omitted.  This will only detect ECC/EDC errors.
 *                                                                           
 *     -m     - timer; will printout the runtime of the pass default = off 
 *                                                                      
 *     -z     - will test the specified memory address without allocation.
 * 		This bypasses all checking but will allow testing in addresses
 *		outside of the main memory heap. It will also allow unaligned
 *              input.
 *		WARNING:  This flag can trash anything and everything!!
 *                                                                        
 *     -d     - used only for march test (2) -- will use this pattern as test 
 *		pattern, default = 5's                       
 *                                                             
 *     -h     - allocate test memory from the firmware heap  
 *                                                                     
 *     -rs    - used only for random test (3) -- will use this data as the 
 *		random seed in order to vary random data patterns generated,
 *	    	default = 0                                                  
 *                                                           
 *     -rb    - randomly allocate and test all of the specified memory 
 *		address range.  Allocations are done of block_size.  
 *                       
 *     -mb    - memory barrier flag.  This flag is only used in the Alpha
 *		-f graycode test.  When set an mb will be done after every
 *		memory access.  This will guarantee serial access to memory.
 *              Only for ALPHA based machines.     
 *
 *	-ba   - used only for block test (4) -- will use the data stored at 
 *		this address to write to each block
 *
 *   COMMAND EXAMPLE(S):                                 
 *~                                                   
 *  >>>memtest -sa 200000 -l 1000  	
 *	-write gray code starting at 0x200000 for 0x1000 bytes          
 *	 (The allocation header will reside from 0x1fffe0 to 0x200000)
 *                                   
 *  >>>memtest -sa 300000 -p 10 	
 *	-write gray code from 0x300000 for 8192 bytes do 10 passes. 
 *	 (The allocation header will reside from 0x2fffe0 to 0x300000)
 *                                            
 *  >>>memtest -sa 200000 -l 1000 -f 
 *	-write gray code starting at 0x200000 for 0x1000 bytes. Run the macro
 *	 version which does not verify the data.
 *	 (The allocation header is placed from 0x1fffe0 to 0x200000)
 *
 *  >>>memtest -f -mb
 *	-write gray code starting anywhere in memory for a block size of
 *	 8192 bytes. Run the macro version which does not verify the data.
 *       Do a MB after every read and write to memory.
 *
 *  >>>memtest -sa 200000 -ea 400000 -rb  
 *	-write gray code starting at 0x200000 and ending at 0x3ffffc.
 *	 Randomly allocate blocks of size 8192.  This will attempt to
 *	 allocate and test every block within this range.  The -rb will
 *	 not error if a block within the range can't be allocated.
 *	 (The allocation header will reside from 0x1fffe0 to 0x200000)
 *
 *  >>>memtest -h -rb -bs 100 	
 *	-write gray code test to the firmware heap. Randomly allocate
 *	 blocks of 0x100 bytes. 
 *                 
 *  >>>memtest -rb -p 0 
 *	-write gray code across all of memzone(all memory excluding the
 *	 console and pal area, the firmware heap, the HWRPB). Run until
 *	 ^c or kill.
 *
 *  >>>memtest -sa 300000 -l 200000 -i 1 -p 0& 
 *  >>>memtest -sa 300008 -l 200000 -i 1 -p 0 -z& 
 *	-write gray code starting at 0x300000 for 0x200000 bytes. Write
 *	 the graycode to every other longword.  Run forever in the back-
 *	 ground.  The second memtest will test the same range also every 
 *	 other longword. The -z causes no alignment checking and no allocation 
 *	 of the test area. The allocation will be done by the first memtest. 
 *       These may be run on separate processors to test cache coherency.
 *
 *  >>>memtest -sa 200000 -t 2 -d 12345678 
 *	-run march test using data pattern of 0x12345678 and its compliment.
 *	 (The allocation header will reside from 0x1fffe0 to 0x200000)
 *
 *  >>>memtest -sa 300000 -t 3 -rs 55555555 
 * 	-run random test using the random seed of 0x55555555 to generate
 *	 the random data patterns from.
 *	 (The allocation header will reside from 0x2fffe0 to 0x300000)
 *
 *  >>>dep -l -n 4 1ffc00 ffffffff
 *  >>>dep -l -n 4 1ffc10 0
 *  >>>dep -l -n 4 1ffc20 ffffffff
 *  >>>dep -l -n 4 1ffc30 0
 *  >>>memtest -t 4 -sa 200000 -ba 1ffc00
 *	-Set up memory locations 1ffc00-1ffc40 with a pattern of 4 longwords 
 *	 of f's, then 4 longwords of 0's, then 4 longwords of f's, and 4
 *	 longwords of 0's.  (Sets up a BCACHE blocks worth of data)  Next,
 *	 run test 4 starting at location 2 MEG and use the block of data just
 *	 set up as the block of data to write.
 *	 (The allocation header will reside from 0x1fffe0 to 0x200000)
 *
 *~                                  
 * FORM OF CALL:                                           
 *                                  
 *	memtest( argc, *argv[] )                  
 *                                                              
 * RETURN CODES:                                  
 *                                   
 *	msg_success          - Success                       
 *	msg_failure          - Failure           
 *	msg_mop_err_mem_comp - Data compare error
 *                                               
 *                                                             
 * ARGUMENTS:                                       
 *                                                                
 *	int argc - number of command line arguments passed by the shell
 *	char *argv [] - array of pointers to arguments     
 *                                            
 * SIDE EFFECTS:                                         
 *                                             
 *	The memory section tested will have its data destroyed
-*/                                           
                                                             
/*------------------------------------------------------------------------------
  Initialization Code.                           
------------------------------------------------------------------------------*/
                                            
int memtest (int argc, char *argv []) {               
    int			i=0,found_test4=0,cpu_type,cpu_cnt;
    unsigned char  	answer[8],name[8];
    unsigned int     	status,passcount;                                    
    UINT		victim_addr=1;
    UINT		block_addr;
    UINT		no_sign_ext_addr;
    unsigned int	fv_pattern[] = {FV_PATTERN,FV_PATTERN};
    struct PCB	 	*pcb=getpcb();
    diag_common		*gptr;
    externals	     	*eptr;            
    struct IOB		*iob_ptr;    	
    struct QSTRUCT     	qual[NUM_QUALS + MORE_QUALS];           
    struct ZONE		*zp;                                           
    struct EVNODE     	ev, *evp;

/* Load the dispatch table address, supported device list and
   cleanup code pointer into the PCB. Also get pointer to EV's 
   and initialize watch chip pointer */
                                          
	evp = &ev;
    	pcb->pcb$a_dis_table = dispatch_table;               
                                                     
/* Call diag_init to set up the diagnostic environment. */                   
                                                   
    	status = diag_init(&argc,argv,qual,
		       "%ssa %sea %sl %sbs f m z %sd h %srs rb %xi mb %sba",0);
    	if (status != msg_success)               
  	    return (status);                 
                     
	gptr =(diag_common *)pcb->pcb$a_diag_common;

/* Allocate memory for the external variables.	*/
    	if ((eptr = (externals *) malloc(sizeof(externals))) == NULL)
	 {
    		err_printf(msg_insuff_mem,sizeof(externals));
#if MEDULLA
		sio_error (DIAG$K_HARD, FORM_ERROR_VECTOR (0,
                           MED$K_DNC_MEMTEST,
                           1, /* failure point */
                           MED$K_DEC_MALLOC_FAILED),
                           "");
#endif
		return msg_failure;
	 }

    	pcb->pcb$a_globals = (int *) eptr;
                                                          
/* Load rundown with cleanup routine after returning successfully from
   diag_init -- this way won't get IOB errors when diag_rundown tries
   to remove them */
/* TWP -- move this later and see what happens */
    	pcb->pcb$a_rundown = (void *)cleanup_no_completion;          

/* Use "memory" for the device name */                    
    	strncpy(pcb->pcb$b_dev_name, "memory", MAX_NAME);   
                                                           
#if TURBO
/* Get the name of the CPU we're running on */
	GetCpuName(eptr->cpu_name);

/* Adding print for test script -- if d_verbose EV is 1 (means -q was not
   given), print starting message including the CPU name */
	if ((ev_read("d_verbose",&evp,0)) != msg_success)
	    err_printf(msg_noev, "d_verbose");
#endif
/* MALLOC space for and create the IOB  */
    	if ((iob_ptr = (struct IOB *) malloc(sizeof(struct IOB))) == NULL) 
	{
    		err_printf(msg_insuff_mem,sizeof(struct IOB));
#if MEDULLA
		sio_error (DIAG$K_HARD, FORM_ERROR_VECTOR (0,
                           MED$K_DNC_MEMTEST,
                           2, /* failure point */
                           MED$K_DEC_MALLOC_FAILED),
                           "");
#endif
		return msg_failure;
	}
                                     
    	create_iob(iob_ptr, pcb->pcb$b_dev_name);               
      	                                                                
/* Memtest extended error routine loaded here */
    	gptr->diag$a_ext_error = memtest_extend;
                                             
/* Set the priority to 1 if running in the background -- seems to be a CPU
   hog when running simultaneously with dsk_ex or mp_ex which are waiting
   on semaphores.  A foreground process' priority = 3, a background process'
   priority = 2 */
/*
*	bg is really pri of parent - 1, not necessarily pri 2.
*	may want to check parent's priority instead
*/
#if AVANTI || K2 || TAKARA || RAWHIDE
	krn$_setpriority (pcb->pcb$l_pid, 1);
#endif          
#if TURBO                                            
	if (pcb->pcb$l_pri == 3)
	   krn$_setpriority (pcb->pcb$l_pid, 2);
	else if (pcb->pcb$l_pri == 2)
	        krn$_setpriority (pcb->pcb$l_pid, 1);
#endif

/* Get the user inputed parameters */     

	if (!qual[QSA].present && qual[QZ].present)
	 return msg_quals;	/* Either SA is present or no Z qual */

	if (qual [QSA].present && qual [QEA].present)
	{
	    UINT start,end;
	    start = (UINT)xtoi(qual [QSA].value.string);
	    end = (UINT)xtoi(qual [QEA].value.string);
	    if (start > end)
	    {
		err_printf(msg_start_gt_end);
		return msg_start_gt_end;
	    }
	}
    	eptr->already_freed = 0;
    	eptr->start_addr = (UINT)(qual [QSA].present) ?  
		       (UINT)xtoi(qual [QSA].value.string): 0;           
	eptr->block_size = DEF_LEN_SZ;    
    	eptr->len = (qual [QEA].present) ?
    		(UINT)(xtoi(qual [QEA].value.string) - eptr->start_addr)
					 : 0;
#if NONZERO_CONSOLE_BASE
	if (eptr->start_addr)
	    eptr->start_addr = phys_to_virt (eptr->start_addr);
#endif
    	if (qual [QL].present)                                   
       	    eptr->len = (UINT)xtoi(qual [QL].value.string);
    	if (eptr->len != 0)
            eptr->block_size = eptr->len;	 
    	eptr->incr = (qual [QI].present) ? (qual [QI].value.integer): 1;
    	eptr->fast = (qual [QF].present) ? 1 : 0;               
    	eptr->timer = (qual [QM].present) ? 1 : 0;                       
    	eptr->no_check = (qual [QZ].present) ? 1 : 0;
    	eptr->do_mb = (qual [QMB].present) ? 1 : 0;
    	gptr->diag$l_failadr = eptr->start_addr;
	eptr->test_patt = (qual [QD].present) ? 
			(UINT)xtoi(qual [QD].value.string) : 
			*(UINT *)fv_pattern;

    	passcount = (qual [QP].present) ? qual [QP].value.integer : 1;
    	eptr->rand_seed = (qual [QRS].present) ? 
			  (UINT)xtoi(qual [QRS].value.string) : 0;
                         

/* Id the ZONE to use */
    	if (qual [QH].present)
            zp = &defzone;
    	else        
            zp = memzone;

#if DEBUG
    	printf("zone : %x\n",zp);           
#endif                                                              

	if ((zp == NULL) || (zp->size == 0))
	{
	    err_printf(msg_dyn_badzone, zp);
	    return msg_failure;	
	}

/* Check for good alignment of starting address */
	if (((u_int)(eptr->start_addr) & 0x1f) && (!eptr->no_check))
	{
	    err_printf(msg_mem_adr_align);
#if MEDULLA
	    sio_error (DIAG$K_HARD, FORM_ERROR_VECTOR (0,
                       MED$K_DNC_MEMTEST,
                       3, /* failure point */
                       MED$K_DEC_BAD_ALIGNMENT),
                       "");
#endif
	    return msg_mem_adr_align;                      
        }                                    
/* Check for good alignment of length */
	if (((u_int)(eptr->len) & 0x1f) && (!eptr->no_check))
	{
	    err_printf(msg_mem_len_align);       
#if MEDULLA
	    sio_error (DIAG$K_HARD, FORM_ERROR_VECTOR (0,
                       MED$K_DNC_MEMTEST,
                       4, /* failure point */
                       MED$K_DEC_BAD_ALIGNMENT),
                       "");
#endif
	    return msg_mem_len_align;       
        }                                    

/* Test random blocks */                       
    	if (qual [QRB].present)                            
    	{
            eptr->block_size = (qual [QBS].present) ?  
    		           (UINT)xtoi(qual [QBS].value.string): 
			   (UINT)DEF_LEN_SZ;
    	    if (qual [QH].present)                              
            {          
  	        if (eptr->start_addr == 0) 
    	    	    eptr->start_addr = defzone.header.adj_fl; 
  	        if (eptr->len == 0) 
    	    	    eptr->len = defzone.size;          
            }          
    	    else                  
            {                                           
  	        if (eptr->start_addr == 0) 
		    /* TWP--added 32 bytes for header at top of memzone */
    	    	    eptr->start_addr = ((u_int)&memzone->header)+0x20; 
  	        if (eptr->len == 0)                                      
		    /* TWP--subtracted 32 bytes so length won't include header*/
    	      	    eptr->len = ((u_int)memzone->size)-0x20;
            }                   

	    /* Check that block size is not larger than length (will cause
	       rand_block test to infinite loop) */
	    if (eptr->len < eptr->block_size)
	    {
	        /* Print out user error */
	        err_printf(msg_mem_len_gt_bs);       
	        err_printf("\n");  
#if MEDULLA
	        sio_error (DIAG$K_HARD, FORM_ERROR_VECTOR (0,
                           MED$K_DNC_MEMTEST,
                           5, /* failure point */
                           MED$K_DEC_BLKSIZE_GT_LENGTH),
                           "");
#endif
                eptr->start_addr = 0;                   
	        return msg_mem_len_gt_bs;       
	    }

            /* Load rundown with cleanup routine after all user errors so 
               completion message isn't printed out after user errors */
    	        pcb->pcb$a_rundown = (void *)memtest_cleanup;         

	        rand_block(qual, passcount,zp);                  
	        return msg_success;                          
        }                        

	/* Store user entered block off data patterns in patt_block array for
	   safe keeping (user entered address was not malloc'd so may get
	   corrupted) or if not block address entered load in block of patterns
	   to cause maximum toggling of LSB line (taken from memory folks) */
                                                                
    	if (qual[QBA].present)
	{
/* Fix possibility of block_addr being in >2GB memory range */
	    block_addr = (UINT)xtoi(qual[QBA].value.string);

	    for (i=0; i<BCACHE_BLOCK_LW; ++i)
	    {
	        eptr->patt_block[i] = *(UINT *)block_addr;
		block_addr += 4;	
	    }
	}
	
	else
	{
	    for (i=0; i<BCACHE_BLOCK_LW/4; ++i)
	        eptr->patt_block[i] = 0xffffffff;
	    for (i=BCACHE_BLOCK_LW/4; i<BCACHE_BLOCK_LW/2; ++i)
	        eptr->patt_block[i] = 0;
	    for (i=BCACHE_BLOCK_LW/2; i<BCACHE_BLOCK_LW*3/4; ++i)
	        eptr->patt_block[i] = 0xffffffff;
	    for (i=BCACHE_BLOCK_LW*3/4; i<BCACHE_BLOCK_LW; ++i)
	        eptr->patt_block[i] = 0;
	}	    

 /* Allocate of memory to test from main memory heap */                       
 /* use the users starting address if specified, if not */
 /* use any free memory */

	/* Check to see if test 4 is being run */
	for (i=0;(pcb->pcb$a_test_mask[i] != NULL); ++i)
	    if (pcb->pcb$a_test_mask[i] == 4) found_test4 = 1;

	/* Check for test without mallocing --- Can be VERY destructive!!! */
    	if (!eptr->no_check)   
    	{                                                
    	 if (eptr->start_addr == 0)                               
	 {
 	    /* If running test 4, need to malloc an extra area a bcache
	       offset from the starting address */
	    if (found_test4)
            { 
#if (TURBO || RAWHIDE)
		/* If length is less than Bcache size , more efficient to 
		   malloc first chunk and then next chunk + Bcache size */
		if (eptr->len < get_bcache_size())
		{
		    /* Malloc will allocate from the start of memzone first
		       so need to get lowest address first */
                                   
	            eptr->start_addr = (UINT)dyn$_malloc(eptr->block_size,
				   DYN$K_SYNC|DYN$K_ZONE|DYN$K_ALIGN|
	       		           DYN$K_NOWAIT|DYN$K_NOFLOOD,32,0,zp);

		    victim_addr = (UINT)dyn$_malloc(eptr->block_size,
		                   DYN$K_SYNC|DYN$K_ZONE|DYN$K_ALIGN|
		      		   DYN$K_NOWAIT|DYN$K_NOFLOOD,-1,
				   eptr->start_addr + get_bcache_size(),zp);
		}
		else
		{
	            eptr->start_addr = (UINT)dyn$_malloc(eptr->block_size+
				   get_bcache_size(),DYN$K_SYNC|DYN$K_ZONE|
				   DYN$K_ALIGN|DYN$K_NOWAIT|DYN$K_NOFLOOD,32,0,zp);
		}
#endif
            }
	    else
	    {
                eptr->start_addr = (UINT)dyn$_malloc(eptr->block_size,
			  	   DYN$K_SYNC|DYN$K_ZONE|DYN$K_ALIGN|
				   DYN$K_NOWAIT|DYN$K_NOFLOOD,32,0,zp); 
	    }
	 }
    	 else   
         {
	    /* If running test 4, need to malloc an extra area a b-cache
               offset away */
	    if (found_test4)
	    {
#if (TURBO || RAWHIDE)
	    	/* If length is less than Bcache size, more efficient to 
	    	   malloc first chunk and then next chunk + Bcache size */
	    	if (eptr->len < get_bcache_size())
	    	{
	            eptr->start_addr = (UINT)dyn$_malloc(eptr->block_size,
	    		           DYN$K_SYNC|DYN$K_ZONE|DYN$K_ALIGN|
	    	        	   DYN$K_NOWAIT|DYN$K_NOFLOOD,-1,eptr->start_addr,zp); 

	            victim_addr = (UINT)dyn$_malloc(eptr->block_size,
	    	                   DYN$K_SYNC|DYN$K_ZONE|DYN$K_ALIGN|
	    			   DYN$K_NOWAIT|DYN$K_NOFLOOD,-1,
	    			   eptr->start_addr+get_bcache_size(),zp);
	    	}
		else
		{
	            eptr->start_addr = (UINT)dyn$_malloc(eptr->block_size +
				   get_bcache_size(),DYN$K_SYNC|DYN$K_ZONE|DYN$K_ALIGN|
	    			   DYN$K_NOWAIT|DYN$K_NOFLOOD,-1,eptr->start_addr,zp); 
		}
#endif
	    }
	    else 
	    {
                eptr->start_addr = (UINT)dyn$_malloc(eptr->block_size,
	     			   DYN$K_SYNC|DYN$K_ZONE|DYN$K_ALIGN|
	     			   DYN$K_NOWAIT|DYN$K_NOFLOOD,-1,eptr->start_addr,zp);
	    }
	 }
    	}			/* End if no malloc case */

#if DEBUG
        printf("eptr->start_addr : %x\n",eptr->start_addr);         
        printf("victim_addr : %x\n",victim_addr);         
#endif

/* Call diag_start to run the specified tests. */                          
        if ((eptr->start_addr != 0) && (victim_addr != 0))
	{                                                

            /* Load rundown with cleanup routine after all user errors so 
               completion message isn't printed out after user errors */
    	    pcb->pcb$a_rundown = (void *)memtest_cleanup;        

    	    diag_start();                                   
	    return msg_success;                          
	}                                 
        else                         
        {                                               

/* Print out user error instead of full report_error */
	    if (found_test4)
	    {
#if (TURBO || RAWHIDE)
		/* If test four was entered check to see if start address
		   or 4 MEG plus starting address was unable to be malloc'd
		   and give correct error callout */
		if (eptr->start_addr == 0)
	            err_printf(msg_d_no_malloc, eptr->block_size, gptr->diag$l_failadr);
		else
	            err_printf(msg_d_no_malloc, eptr->block_size, gptr->diag$l_failadr+get_bcache_size());
#endif
	    }
	    else
	    {
	        err_printf(msg_d_no_malloc, eptr->block_size, 
				  gptr->diag$l_failadr);
#if MEDULLA
		sio_error (DIAG$K_HARD, FORM_ERROR_VECTOR (0,
                           MED$K_DNC_MEMTEST,
                           6, /* failure point */
                           MED$K_DEC_MALLOC_FAILED),
                           "");
#endif
	    }
	    return msg_d_no_malloc;
        }                  
   
}                                                               
                                                           
int rand_block(struct QSTRUCT *qual, unsigned int passcount, struct ZONE *zp)
{                  
    int i,j;
    int status;
    unsigned int lp_cnt;                                     
    UINT rand_index;   	      	   /* index into LCG table */
    UINT seed;		     	   /* random LCG table seed */
    int num_of_blks;           
    externals *eptr;                       
    struct PCB *pcb=getpcb();            
    diag_common	*diag=(diag_common *)pcb->pcb$a_diag_common;
    UINT	begin_addr;
    int found_mem;
                                                            
    eptr=(externals *)pcb->pcb$a_globals;                               
                                                       
    /* Because the startup message I no longer printed in diag_init but now
       moved to diag_Start and this routine does not call diag_start, it needs
       a call to the routine to output the startup message */
    diag_print_startup();

#if DEBUG
    printf("rand_block\n");                                  
#endif                                                      
    begin_addr = (UINT)eptr->start_addr;         
    num_of_blks = (UINT) ((UINT) eptr->len / (UINT) eptr->block_size);
                                     
/* round the loop count up to the next highest power of 2 */
    for (lp_cnt=1; lp_cnt<=0x80000000; lp_cnt=lp_cnt<<1)
    	if (lp_cnt >= num_of_blks) break;
                 
#if DEBUG                                                            
    printf("eptr->len : %x\n",eptr->len);         
    printf("eptr->block_size : %x\n",eptr->block_size);     
    printf("eptr->start_addr : %x\n",eptr->start_addr);  
    printf("num_of_blks: %x\n",num_of_blks);         
    printf("lp_cnt: %x\n",lp_cnt);         
    printf("passcount: %x\n",passcount);         
    printf("last addr : %x\n",begin_addr+eptr->len-eptr->block_size);
    /*krn$_sleep(100000);*/                         
#endif                     
                                                            
    status = msg_success;
    seed = (UINT)eptr->rand_seed;
    rand_index = log2(lp_cnt);

#if DEBUG                                             
    printf("rand_index : %d\n",rand_index);
#endif                                          
/* main loop; repeat for passcount */     
    i = 0;                                            
    while ((i <= passcount) || (passcount == 0))
    {                                              
#if AVANTI || K2 || TAKARA
	krn$_sleep(1000);
#endif
	/* need to load up pass count into the PCB manually because
	   diag_start is not being called */
	diag->diag$l_pass_count = i+1;

	/* need to load in test number and print test trace
	   manually because diag_start is not called */
	diag->diag$l_test_num = 1;
	diag_print_test_trace();

        for (j=0; j<lp_cnt; j++)                                      
        {	                  
	    seed = random( seed, rand_index);
	    eptr->start_addr = (seed % lp_cnt) * 
			       (eptr->block_size) + begin_addr;
                                                                     
#if DEBUG                
    	    printf("eptr->start_addr : %x\n",eptr->start_addr);         
    	    printf("j : %x\n",j);               
    	    printf("i : %x\n",i);         
    	    printf("last addr : %x\n",begin_addr+eptr->len-eptr->block_size);
    	    printf("seed : %x\n",seed);         
#endif                                                     
            if (eptr->start_addr > (begin_addr + eptr->len - eptr->block_size))
	    {	                                                     
#if DEBUG
    	        printf("out of range = %x\n",eptr->start_addr);
#endif                                    
		eptr->start_addr = 0;                    
            }
/* Allocate memory to test */                       
	    if (!qual[QZ].present) {
		eptr->start_addr = (UINT)dyn$_malloc(eptr->block_size,
				     DYN$K_SYNC|DYN$K_ZONE|DYN$K_ALIGN|
		    	      	     DYN$K_NOWAIT|DYN$K_NOFLOOD,
		      	       	     -1, eptr->start_addr, zp);
		eptr->already_freed = 0;
	    }

            if (eptr->start_addr != 0)                       
	    {
    		found_mem = 1;                                            
	    	status = memtest_gray();                       
	        if (!qual[QZ].present)            
		{
            	 dyn$_free(eptr->start_addr,DYN$K_SYNC|DYN$K_FLOOD); 
		 eptr->already_freed = 1;
		}
            }                                      
	    if (killpending() || (status == msg_halt))
	        return msg_failure;	
        }                         
    	if (found_mem)
	{       
/* want to increment i even though no memory was tested to prevent infinite
   looping if couldn't malloc memory  -- should make error callout if unable to 
   malloc memory */
            i++;
    	    diag->diag$l_pass_count++;
	    diag_print_end_of_pass();
	}                         
    }                                        
    return msg_success;	                                             
}                                     

/*------------------------------------------------------------------------------
  Cleanup code.                      
                                                                    
  This code is automatically called during rundown of the diagnostic process.
  This code should leave the hardware in a known state, as well as cleaning
  up any software state.  This will be the commonly used cleanup code and WILL
  call diag_rundown to print out the completion message.
------------------------------------------------------------------------------*/
                          
static void memtest_cleanup()    
{                                                         
    struct IOB		*iob_ptr;        
    struct PCB 		*pcb;
    externals  	      	*eptr;
    struct EVNODE     	ev, *evp;

/*  Get pointer to EV's */
	evp = &ev;
	         
   	pcb = getpcb(); 
   	eptr=(externals *)pcb->pcb$a_globals;

#if (TURBO || RAWHIDE)
	/* If memory wasn't already free and -z wasn't given, free memory */
	if ((!eptr->already_freed) && (!eptr->no_check))
            dyn$_free(eptr->start_addr,DYN$K_SYNC|DYN$K_NOFLOOD); 
#endif

#if TURBO
/* Adding print for test script -- if d_verbose EV is 1 (means -q was not
   given), print stopping message including the CPU name */
	if ((ev_read("d_verbose",&evp,0)) != msg_success)
	    err_printf(msg_noev, "d_verbose");
/*	if (evp->value.integer == 1) 
	    printf(msg_mem_ex_done, eptr->cpu_name, pcb->pcb$l_pid);
*/
#endif
	diag_rundown();   
                          
 /* Removing IOB from queues ... */
                                             
	                                       
	if ((iob_ptr = (struct IOB *)find_pcb_iob()) != NULL)           
	{                                                 
	 remq (&iob_ptr->pcbq);                             
	 remq_iob (&iob_ptr->sysq);           
	}                 
}                            

/*------------------------------------------------------------------------------
  Cleanup code.                      
                                                                    
  This code is automatically called during rundown of the diagnostic process.
  This code should leave the hardware in a known state, as well as cleaning
  up any software state.  This will NOT call diag_rundown and will not print
  out the completion message -- this should be used to load up in the beginning
  of memtest in case of user errors before any tests are actually executed
------------------------------------------------------------------------------*/
                          
static void cleanup_no_completion()    
{                                                         
    struct IOB		*iob_ptr;        
    struct PCB 		*pcb;
    externals  	      	*eptr;
	         
                                                            
   	pcb = getpcb(); 
   	eptr=(externals *)pcb->pcb$a_globals;

/*	diag_rundown();   */
                          
 /* Removing IOB from queues ... */
                                             
	                                       
	if ((iob_ptr = (struct IOB *)find_pcb_iob()) != NULL)           
	{                                                 
	 remq (&iob_ptr->pcbq);                             
	 remq_iob (&iob_ptr->sysq);           
	}                 
}                            

/* Function to print out expected/recvd and failing address for a given
*  diagnostic test.  
*       
*/                       
static void memtest_extend()
{                                            
	struct PCB 	*pcb=getpcb();
    	diag_common	*gptr=(diag_common *)pcb->pcb$a_diag_common;
    	UINT 		expect, rcvd, address;
	int		i;
                                     
    	/* Extended error output */
                                                            
    	expect = *(UINT *) (&gptr->diag$l_expect);
    	rcvd = *(UINT *) (&gptr->diag$l_rcvd);
    	address = *(UINT *) (&gptr->diag$l_failadr);

        err_printf("Expected value:\t %16x\n", expect);
    	err_printf("Received value:\t %16x\n", rcvd);
    	err_printf("Failing addr:  \t %16x\n", address);

	/* For all SIMM fru mask was found display at the end */
	id_simm_fru(address, expect, rcvd, err_printf);
}
                  
/* Function to load expected/received data, failing address and error number
*  for report_error                          
*          
*/                       
void load_error_info(int err_num, int data, int read_data, UINT addr)
{
    struct PCB 		*pcb=getpcb();
    diag_common		*gptr=(diag_common *)pcb->pcb$a_diag_common;
    char		fru[32];
    unsigned int	brd;

#if MEDULLA
    union error_block error_blk;
    unsigned int test_number;
#endif

    gptr->diag$l_error_type = DIAG$K_HARD;
    gptr->diag$l_error_num = err_num;                
    gptr->diag$l_error_msg = msg_mop_err_mem_comp;
                      
#if DEBUG
    printf("data expected: %x\n",data);
    printf("read data: %x\n",read_data);
    printf("addr: %x\n",addr);  
#endif

#if MEDULLA
    /*
     * Load up structure of known information, then log to NVRAM
     *
     */
    error_blk.sio_ext_blk.error_vector = 
			FORM_ERROR_VECTOR (MED$K_EVFL_PHYS_ADDR, 
					   MED$K_DNC_MEMTEST,
					   err_num,
					   MED$K_DEC_DATA_COMPARE);
    error_blk.sio_ext_blk.pass_count   = 
	(unsigned int)(gptr->diag$l_pass_count);
    error_blk.sio_ext_blk.exp_data[0]  = (unsigned int)data;
    error_blk.sio_ext_blk.exp_data[1]  = (unsigned int)0;
    error_blk.sio_ext_blk.rec_data[0]  = (unsigned int)read_data;
    error_blk.sio_ext_blk.rec_data[1]  = (unsigned int)0;
    error_blk.sio_ext_blk.address[0]   = (unsigned int)addr;
    error_blk.sio_ext_blk.address[1]   = (unsigned int)0;
    test_number = (unsigned int)(gptr->diag$l_test_num);

    log_error (MED$K_SIO_EXT_ERROR, &error_blk, test_number);

    /*
     * Overwrite Error_Num with Medulla Error Vector
     *
     */
    gptr->diag$l_error_num = 
			FORM_ERROR_VECTOR (MED$K_EVFL_PHYS_ADDR, 
					   MED$K_DNC_MEMTEST,
					   err_num,
					   MED$K_DEC_DATA_COMPARE);
#endif

/* Call find_bad_mem to isolate address to failing memory FRU */
#if TURBO
/*
 * Get CPU name we're running on for FRU callout 
 */
    GetCpuName(fru);

/* 
 * Concatenate the two FRUs into the FRU field
 */
    strcat(gptr->diag$b_fru,fru);

#else
    brd = addr_to_brd(addr);
#if RAWHIDE
    sprintf(gptr->diag$b_fru,"mem%d",brd);
#else
    sprintf(gptr->diag$b_fru,"brd%d",brd);
#endif
#endif

/* Extended error info */            
    gptr->diag$l_expect = data;                   
    gptr->diag$l_rcvd = read_data;
    gptr->diag$l_failadr = addr;            
}
