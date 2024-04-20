/* file:	exer.c
 *
 * Copyright (C) 1991, 1993 by
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
 *      Diagnostic exerciser written primarily to exercise DSSI/SCSI disks but
 *	may be useful in exercising other devices.  There isn't any device
 *	specific code in this module.
 *
 *  AUTHORS:
 *
 *      Paul LaRochelle
 *
 *  CREATION DATE:
 *  
 *      05-Mar-1991
 *
 *  MODIFICATION HISTORY:
 *
 *	jje	26-Feb-1996	Conditionalize for Cortex
 *
 *	rbb	13-Mar-1995	Conditionalize for EB164
 *
 *	er	27-Dec-1994	Conditionalize for EB66
 *
 *	er	07-Oct-1994	Conditionalize for EB64+
 *
 *	cto	31-Aug-1994	Select zone to from which to buffer is 
 *				malloc'ed via set_malloc_zone.  This is for 
 *				PCI devices that don't map to memzone.
 *
 *	rnf	11-Aug-1994	Added call to log_error for error logging to
 *				to NVRAM, Medulla platform only.
 *
 *	hcb	30-nov-1993	make block size (bs) local so each exer_kid
 *				can have it's own.  Have fread read sz not
 *				pkt_sz.
 *
 *	cto	18-nov-1993	Add TURBO conditional for memzone/defzone
 *
 *      dda     23-sep-1993     Fixed bug in exer_loop (divide by zero 
 *                              failure) by making block size a global
 *                              variable calculated in gen_sizes. Changed
 *                              all INT's to UINT's.
 *
 *      dda     16-jul-1993     Continued support for RF/RZ 74 Disk Drives.
 *                              Added umax64 and umin64 functions. Changed
 *                              most 32 unsigned integers to 64 bits.  Leaving
 *                              random_blk_range as int until random.c is
 *                              modified for 64 bits.
 *
 *      dda     01-jul-1993     Changed all max and min functions to be
 *                              umax and umin to support RF/RZ 74 disk.
 *      
 *      dda     30-jun-1993     Use function 'umin' instead of 'min in rtn
 *                              gen_sizes to support RZ/RF 74 disk.  We need
 *                              to use the unsigned function since the disk
 *                              size is so large it would be interpreted as
 *                              a negative number by the 'min' function.
 *
 *	jeg	26-Apr-1992	add -delay qualifier and s to action string to
 *				allow sleeping.
 *
 *	ajb	30-Mar-1993	Scale random block selections with start/stop
 *				blocks.
 *
 *      cbf     27-Jan-1993     add arg to read_with_prompt for echo functn.
 *
 *	pel	17-Dec-1992	INODE len, alloc chgd from lwd to quadwd.
 *
 *	pel	04-Dec-1992	Update header doc; chg prompts from cs> to >>>
 *
 *	pel	26-Oct-1992	use msg_exer_finis instead of string
 *
 *	pel	30-Sep-1992	Leave LEDs on if tests fail
 *
 *	pel	09-Jun-1992	Turn LEDs on/off for modules being tested.
 *
 *	pel	11-May-1992	put this program back on firmware heap but
 *				leave exer_kids on memzone.
 *
 *	pel	07-May-1992	put this program & its children's stacks
 *				on memzone heap.
 *				
 *	pel	16-Apr-1992	Call out err on compare operation if bytes
 *				read not equal number of bytes written.
 *				
 *	pel	13-Apr-1992	make len = spec len * bs not len * pkt_sz when
 *				device has no alloc size attribute.
 *				
 *	pel	09-Apr-1992	Rewrite calculations of pkt_sz,
 *				bytes_to_process to fix tta1 bugs & for
 *				clarity's sake. Use err_printf instead of
 *				report_error before diag_init called.
 *				Change -eb to spec inclusive sb, eb range.
 *	
 *	pel	02-Apr-1992	Change routines to not be static
 *				
 *	pel	24-Mar-1992	Get PID of idle process from idle_pids array.
 *				
 *	pel	18-Mar-1992	Documentation change only, (to routine header).
 *				Remove all refs to dynamic reallocation, size
 *				and initialization of DMA buffers.
 *
 *	ajb	13-Mar-1992	change cleanup to exer_cleanup and make global
 *				so that quilt can see it.
 *
 *	pel	06-Mar-1992	add arg to satisfy new read_with_prompt functn.
 *
 *      pel     27-Feb-1992	Always allow var len reads. Assume getting
 *				fewer bytes than requested is ok unless fread
 *				returns 0 bytes. Display bad qscan status.
 *				
 *      pel     24-Feb-1992	Support var len reads up to 2048 byte packets.
 *				Use -bc 0 to select variable len read option.
 *				Needed for tape read support.
 *
 *      pel     20-Feb-1992	Change action string default to '?r'
 *				
 *      pel     19-Feb-1992	Change -o qual to -v.  Update routine header.
 *				
 *      pel     19-Feb-1992	Update routine header.
 *				
 *      pel     17-Feb-1992	Fix pele compile. Add "<device> exer completed"
 *				line to -m report.  Change -de,dr quals to d1,d2
 *
 *      pel     14-Feb-1992	Major changes. Support mulitple devices to
 *				be exercised concurrently.
 *
 *      pel     12-Feb-1992	Add qscan not called flag to diag_init call.
 *				Update help text.
 *
 *      pel     07-Feb-1992	Malloc buffers from memzone instead of sys heap
 *				Add placeholders for new action string chars:
 *				dDmM
 *
 *      pel     05-Feb-1992	Summary line changes: use new quad printf.
 *				Use quadwords where possible to reduce code
 *				size & speed up main loop.
 *				VAX exer summary line reports innacurate data.
 *
 *      pel     24-Jan-1992	include platform.h; fix alpha code
 *
 *      pel     23-Jan-1992	do quadword io's/bytes per sec; eliminate -i
 *				qualifier, use -m instead.
 *
 *      pel     22-Jan-1992	change summary lines of exer report
 *
 *      hcb     25-Sep-1991	Add call to diag_print_end_of_pass
 *
 *      pel     05-Sep-1991	Fix problem w/ destructive prompt query and
 *				don't use report_error until file fopen'd
 *				cuz it needs an IOB to work.
 *
 *      pel     10-Jun-1991	bugs: -de, -dr were reversed; handle overflow
 *				of -l * -bs
 *
 *      pel     06-Jun-1991	add qualifier i, dr, for metric with idle
 *				ticks and packet size and rec buf init.
 *				Also, malloc only 1 buf if two not needed.
 *
 *      pel     31-May-1991	Use report error code instead of err_printf's.
 *
 *      pel     29-May-1991	Use new hd_helper w/o pmem file. Fix bug where
 *				packet size was 1 instead of a larger value.
 *
 *      pel     15-May-1991	replace byte qualifiers w/ block quals. Seek to
 *				w/in blk range instead of entire file range.
 *				Remove overhead subtraction from timing calc.
 *
 *      pel     09-May-1991	fix bug of opening file as w+ instead of r+
 *
 *      pel     08-May-1991	Finish timing metric code and replace -r -w -v
 *				qualifiers with action string qualifier.
 *
 *      pel     28-Mar-1991	start adding metrics.
 *
 *      pel     26-Mar-1991	fix killpending check, reorganize code.
 *
 *      pel     22-Mar-1991	rename back to exer.
 *
 *      pel     05-Mar-1991	initial entry; not functional yet
 *--
 */

#include	"cp$src:platform.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:parse_def.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:ctype.h"
#include	"cp$src:stddef.h"
#include	"cp$src:common.h"
#include 	"cp$src:msg_def.h"
#include 	"cp$src:diag_def.h"
#include 	"cp$src:leds_def.h"
#include 	"cp$src:ev_def.h"                           
#include	"cp$inc:kernel_entry.h"

#if MEDULLA 
#include	"cp$src:medulla_errors.h"
union error_block error_blk;
#endif

struct EXER_ARGS {
	struct IOB	 *iob;
	struct SEMAPHORE *ready;
	struct SEMAPHORE *go;
	struct SEMAPHORE *done;
	UINT     	 pkt_sz; /* packet size in bytes per i/o */
	int		 status; /* set to err status by exer kids if err */
};

#include "cp$inc:prototypes.h"

extern null_procedure ();
extern struct ZONE *set_malloc_zone(struct FILE *fp);
extern unsigned int poweron_msec [2];
extern struct LOCK spl_kernel;
extern struct QUEUE _align (QUADWORD) iobq;  /* system wide IO status block q*/
extern struct LOCK _align (QUADWORD) spl_iobq;
extern int	idle_pids[MAX_PROCESSOR_ID];
extern int diag$r_evs;

#define DEBUG  0
#define DEBUG1 0
#define DEBUG2 0

#define umin64(a,b) (((UINT) (a)) < ((UINT) (b)) ? ((UINT) (a)) : ((UINT) (b)))
#define umax64(a,b) (((UINT) (a)) > ((UINT) (b)) ? ((UINT) (a)) : ((UINT) (b)))


void	exer_cleanup(void);

UINT     gen_pkt_sz (UINT pkt_sz);

void exer_kid (struct QSTRUCT *qual, struct EXER_ARGS *exer_args,
    		char *device_name);

int exer_loop (  
    struct FILE		*fp,		/* filepath to exercise */
    struct QSTRUCT	*qual,
    struct EXER_ARGS    *exer_args,
    UINT           	len,		/* total #of bytes to read/write */
    UINT		pkt_sz,		/* packet size to read/write  */
    unsigned char	*buf2,	/* buffer to read packet into */
    unsigned char	*buf1,	/* buffer to wrt packet from  */
    char		*act_strg_begin	/* beginning address of action string */
);

void gen_sizes( struct QSTRUCT *qual, struct FILE *fp, UINT  *pkt_sz,
	UINT *bytes_to_process);

static struct DIAG_DIS_TABLE dispatch_table[] = { {NULL, NULL} };

static  char    action_table [] = {     /* octal constants */
000,000,000,000,000,000,000,000,  000,000,000,000,000,000,000,000,
000,000,000,000,000,000,000,000,  000,000,000,000,000,000,000,000,
000,000,000,000,000,000,000,000,  000,000,000,000,000,010,000,000, /* -   */
000,000,000,000,000,000,000,000,  000,000,000,000,000,000,000,011, /* ?   */

000,000,021,000,012,000,000,000,  000,000,000,000,000,015,005,000, /* BDMN */
000,000,001,000,000,000,000,003,  000,000,017,000,000,000,000,000, /* RWZ  */
000,000,022,007,013,000,000,000,  000,000,000,000,000,014,006,000, /* bcdmn*/
000,000,002,016,000,000,000,004,  000,000,020,000,000,000,000,000  /* rswz  */
 };

/*+
 * ============================================================================
 * = exer - Exercise one or more devices.				      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      Exercise one or more devices by performing specified read, write, 
 *	and compare operations. Optionally, report performance statistics.
 *
 *      A 'read' operation reads from a device that you specify into a buffer.  
 *	A 'write' operation writes from a buffer to a device that you specify.  
 *	A 'compare' operation compares the contents of the two buffers.
 *	
 *	The exer command uses two buffers, 'buffer1' and 'buffer2', to 
 *	carry out the operations. A read or write operation can be 
 *	performed using either buffer. A compare operation uses both buffers. 
 *
 *	You can tailor the behavior of exer by using options to specify 
 *	the following:
 *
 *##
 *	1 an address range to test within the test device(s),
 * 	2 the packet size, also known as the IO size, which is the number 
 *	  of bytes read or written in one IO operation, 
 * 	3 the number of passes to run, 
 * 	4 how many seconds to run for,
 *	5 a sequence of individual operations performed on the test device(s) 
 *	  The qualifier used to specify this is called the action string 
 *	  qualifier.
 *#
 *   COMMAND FMT: exer 4 DZ 0 exer
 *                            
 *   COMMAND FORM:
 *  
 *      exer ( [-sb <start_block>] [-eb <end_block>] [-p <pass_count>]
 *             [-l <blocks>] [-bs <block_size>] [-bc <block_per_io>]
 *             [-d1 <buf1_string>] [-d2 <buf2_string>] [-a <action_string>]
 *             [-sec <seconds>] [-m] [-v] [-delay <milliseconds>]
 *             <device_name>... )
 *
 *   COMMAND TAG: exer 0 RXBZ exer
 *                            
 *   COMMAND ARGUMENT(S):
 *
 * 	<device_name>... - Specifies the name(s) of the device(s)
 *		or filestream(s) to be exercised.
 *
 *   COMMAND OPTION(S):
 *
 *      -sb <start_block> - Specifies the starting block number (hex)
 *		within filestream.  The default is 0.
 *
 *      -eb <end_block> - Specifies the ending block number (hex)
 *		within filestream.  The default is 0.
 *
 *      -p <pass_count> - Specifies the number of passes to run the exerciser.
 *		If 0, then run forever or until Ctrl-C.  The default is 1.
 *
 *      -l <blocks> - Specifies the number of blocks (hex) to exercise.
 *		l has precedence over eb.  If only reading, then specifying
 *		neither l nor eb defaults to read till eof. If writing, and 
 *		neither l nor eb are specified then exer will write for 
 *		the size of device.  The default is 1.
 *
 *      -bs <block_size> - Specifies the block size (hex) in bytes.
 *		The default is 200 (hex).
 *
 *      -bc <block_per_io> - Specifies the number of blocks (hex) per I/O.
 *		On devices without length (tape) use the specified packet size
 *		or default to 2048.  The maximum block size allowed with
 *		variable length block reads is 2048 bytes.
 *		The default is 1.
 *
 *      -d1 <buf1_string> - string arg for eval to gen buffer1 data pattern from.
 *	    buffer1 is iniitialized only once and that is before any IO occurs.
 *		default = all bytes set to hex 5A's
 *
 *      -d2 buf2_string - string arg for eval to gen buffer2 data pattern from.
 *	    buffer2 is iniitialized only once and that is before any IO occurs.
 *		default = all bytes set to hex 5A's
 *
 *      -a <action_string> - Specifies an exerciser 'action string', which
 *		determines the sequence of reads, writes, and compares to
 *		various buffers.  The default action string is '?r'.
 *		The action string characters are:
 *#o
 *           o  r -  Read into buffer1.
 *      
 *           o	w -  Write from buffer1.
 *
 *           o	R -  Read into buffer2.
 *      
 *           o	W -  Write from buffer2.
 *
 *           o	n -  Write without lock from buffer1.
 *
 *           o	N -  Write without lock from buffer2.
 *
 *           o	c -  Compare buffer1 with buffer2.
 *
 *           o	- -  Seek to file offset prior to last read or write.
 *
 *           o	? -  Seek to a random block offset within the specified range
 *                   of blocks.  exer calls the program, random, to 'deal' each
 *                   of a set of  numbers once.  exer chooses a set which is a
 *		     power of two and is greater than or equal to the block
 *		     range.  Each call to random results in a number
 *                   which is then mapped to the set of numbers that are in
 *                   the block range and exer seeks to that location in the
 *                   filestream.  Since exer starts with the same random number
 *                   seed, the set of random numbers generated will always be
 *		     over the same set of block range numbers.
 *
 *	    o   s -  sleep for a number of milliseconds specified by the 
 *		     delay qualifier.  If no delay qualifier is present, sleep
 *		     for 1 millisecond.  NOTE: times as reported in verbose 
 *		     mode will not necessarily be accurate when this action
 *		     character is used.
 *
 *          o   z -  Zero buffer 1
 *
 *          o   Z -  Zero buffer 2
 *
 *          o   b -  Add constant to buffer 1
 *
 *          o   B -  Add constant to buffer 2
 *		     
 *#
 *      -sec <seconds> - Specifies to terminate the exercise after the number
 *		of seconds have elapsed.  By default the exerciser continues
 *		until the specified number of blocks or passcount are processed.
 *
 *      -m - Specifies metrics mode.  At the end of the exerciser a total
 *		throughput line is displayed.
 *
 *      -v - Specifies verbose mode, data read is also written to stdout.
 *		This is not applicable on writes or compares.
 *		The default is verbose mode off.
 *
 *	-delay <millisecs> - Specifies the number of milliseconds to delay when 
 *			     "s" appears as a character in the action string.
 *
 *   COMMAND EXAMPLE(S):
 *~
 *      >>>exer du*.* dk*.* -p 0 -secs 36000
 *~      
 *      Read all DSSI and SCSI type disks for the entire length of each disk.
 *	Repeat this until 36000 seconds, 10 hours, have elapsed.  All disks 
 *	will be read concurrently.  Each block read will occur at a random 
 *	block number on each disk.
 *~      
 *      >>>exer -l 2 duc0
 *~      
 *      Read block numbers 0 and 1 from device duc0.
 *~      
 *      >>>exer -sb 1 -eb 3 -bc 4 -a 'w' -d1 '0x5a' duc0   
 *~      
 *      Write hex 5a's to every byte of blocks 1, 2, and 3.
 *      The packet size is bc * bs, 4 * 512, 2048 for all writes.
 *~      
 *	>>>ls -l du*.* dk*.*
 *	d**.* no such file
 *	r---   dk                  0/0              0   dka0.0.0.0.0
 *	>>>exer dk*.* -bc 10 -sec 20 -m -a 'r'
 *	dka0.0.0.0.0 exer completed
 *
 * 	packet                                    IOs            elapsed idle
 *  	 size    IOs  bytes read  bytes written   /sec bytes/sec seconds secs
 *        8192  3325    27238400              0    166   1360288     20    19
 *~      
 *~      
 *      >>>exer -eb 64 -bc 4 -a '?w-Rc' duc0   
 *~      
 *      A destructive write test over block numbers 0 thru 100 on disk duc0.
 *	The packet size is 2048 bytes. The action string specifies the 
 *	following sequence of operations:
 *##
 *	1 Set the current block address to a random block number on the disk
 *	  between 0 and 97.  A four block packet starting at block numbers
 *	  98, 99, or 100 would access blocks beyond the end of the length to
 *	  be processed so 97 is the largest possible starting block address
 *	  of a packet.
 *	2 Write a packet of hex 5a's from buffer1 to the current block address.
 *	3 Set the current block address to what it was just prior to the
 *	  previous write operation.
 *	4 From the current block address read a packet into buffer2.
 *	5 Compare buffer1 with buffer2 and report any discrepancies.
 *	6 Repeat steps 1 thru 5 until enough packets have been written to
 *	  satisfy the length requirement of 101 blocks.
 *#
 *~      
 *      >>>exer -a '?r-w-Rc' duc0   
 *~      
 *      A non-destructive write test with packet sizes of 512 bytes.  The
 *	action string specifies the following sequence of operations:
 *##
 *	1 Set the current block address to a random block number on the disk.
 *	2 From the current block address on the disk, read a packet into
 *	  buffer1.
 *	3 Set the current block address to the device address where
 *	  it was just before the previous read operation occurred.
 *	4 Write a packet of hex 5a's from buffer1 to the current block address.
 *	5 Set the current block address to what it was just prior to the
 *	  previous write operation.
 *	6 From the current block address on the disk, read a packet into
 *	  buffer2.
 *	7 Compare buffer1 with buffer2 and report any discrepancies.
 *	8 Repeat the above steps until each block on the disk has been 
 *	  written once and read twice.
 *#
 *~
 *      >>>set myd 0
 *      >>>exer -bs 1 -bc a -l a -a 'w' -d1 'myd myd ~ =' foo
 *      >>>clear myd
 *      >>>hd foo -l a
 *      00000000  ff 00 ff 00 ff 00 ff 00 ff 00 ..........
 *~
 *	Use an environment variable, myd, as a counter.
 *      Write 10 bytes of the pattern ff 00 ff 00... to RAM disk file foo.
 *	A packet size of 10 bytes is used.  Since the length specified is also
 *	10 bytes then only one write occurs.
 *	Delete the environment variable, myd.
 *	The hd, hex dump of foo shows the contents of foo after exer is run.
 *~      
 *      >>>set myd 0
 *      >>>exer -bs 1 -bc a -l a -a 'w' -d1 'myd myd 1 + =' foo
 *      >>>hd foo -l a
 *      00000000  01 02 03 04 05 06 07 08 09 0a ..........
 *~
 *      Write a pattern of 01 02 03 ... 0a to file foo.
 *~
 *      >>>set myd 0
 *      >>>exer -bs 1 -bc 4 -l a -a 'w' -d1 'myd myd 1 + =' foo -m
 *	foo exer completed
 *	
 * 	packet                                    IOs            elapsed idle
 *  	 size    IOs  bytes read  bytes written   /sec bytes/sec seconds secs
 *           4     3           0             10   3001     10001      0     0
 *      >>>hd foo
 *      00000000  01 02 03 04 01 02 03 04 01 02 ..........
 *      >>>show myd
 *      myd			4
 *~      
 *~      
 *      >>>echo '0123456789abcdefghijklmnopqrstAB' -n >foo3
 *	>>>exer -bs 1 -v -m foo3
 *	b2lkfmp8jatsnA1gri54B69o3qdc7eh0foo3 exer completed
 *
 * 	packet                                    IOs            elapsed idle
 *  	 size    IOs  bytes read  bytes written   /sec bytes/sec seconds secs
 *           1    32          32              0   5333      5333      0     0
 *~
 *   COMMAND REFERENCES:
 *
 *	exer_read, exer_write, memexer, netexer
 *
 * FORM OF CALL:
 *  
 *	exer( argc, *argv[] )
 *  
 * RETURN CODES:
 *
 *	msg_success         - Success
 *	msg_missing_device  - number of command line arguments not equal to 1
 *	msg_bad_open        - problem opening the specified filepath
 *	msg_ex_devreaderr   - device read error
 *	msg_dep_devwriteerr - device write error
 *	msg_data_comp_err   - buffer1 didn't match buffer2
 *	msg_fatal	    - illegal action string
 *       
 * ARGUMENTS:
 *
 *	int argc - number of command line arguments passed by the shell
 *	char *argv [] - array of pointers to arguments
 *
 * SIDE EFFECTS:
 *
 *	A device will have its data destroyed if the -a qualifier string has
 *	any of the 'w', 'W', 'n', 'N' specifiers.
 * 
 * FUNCTIONAL DESCRIPTION:
 * 
 *      Exercise one or more devices.  As described in the preceding overiew 
 *	section, exer uses two buffers, buffer1 and buffer2.
 *      Buffer1 and buffer2 are in main memory in the 'memzone' heap if 
 *	it is possible to do so for the device specified.  For some devices,
 *	it may be necessary to point it elsewhere.
 *
 *      Both buffer1 and buffer2 are initialized to a data pattern before any 
 *	IO operations occur.  These buffers are never reinitialized, even after
 *	completing one or more passes.
 *      The data patterns that the buffers are initialized with are either a
 *	hex 5A in every byte of each buffer or it is specified via the string 
 *	arguments to the optional data pattern qualifiers, -d1, -d2
 *
 *	The d1, d2 qualifiers use a postfix string argument to initialize
 *	a buffer's contents as follows.
 *	For each byte in the specified buffer, starting with the 
 *	first byte, this postfix string is passed to the eval command which 
 *	returns a byte value which is then written to the specified buffer.
 *      
 *      Several exer qualifiers are used to specify the amount of device data 
 *      to be processed.  The qualifiers -sb, -eb, -l, -bs, and -bc specify,
 *      respectively: starting block, ending block, number of blocks,
 *      block size in bytes, and the number of blocks in a packet, where a
 *      packet is the amount  of data transferred in one IO operation. 
 *      
 *      Reading, writing, comparing buffers, and other operations can be
 *      specified to occur in various combinations and sequences.  These
 *      operations are specified by a string of one-character command
 *      codes known as the  'action string'.  The action string is
 *      specified as an argument to the action string qualifier, -a.
 *      
 *      Each command code character in the action string is processed in a
 *      sequence from left to right. Each time that exer completes all of
 *      the operations specified by the action string, exer will reduce
 *      the remaining amount of device data to be processed by the size of
 *      the last packet processed by the action string.  The action string
 *      is repeatedly processed until the specified amount of device data
 *      has been processed. 
 *      
 *      Lower-case action string characters, 'rwn', specify operations
 *      that involve buffer1.  Upper-case action string characters, RWN,
 *      specify operations that involve buffer2.  The action string
 *      character, 'c', involves both buffer1 and buffers. The action
 *      string characters, '-?,' do not involve either buffer1 or buffer2.
 *      
 *      A random number generator can be used to seek to varying device
 *      locations before performing either a read or write operation.
 *      Randomization is acheived by calling the function, 'random',
 *      which uses a linear congruential generator (LCG) to
 *      generate the numbers.  This algorithm isn't truly random, but it
 *      comes closest to meeting the needs of exer.  Each time that
 *      'random' is called, it returns a number from a specified range. 
 *      If the range of numbers is a power of two, then each subsequent
 *      call to  'random', is guaranteed to return a different number from
 *      the range until all possible numbers within the range have been
 *      chosen.  If the range of numbers isn't a power of two, then '
 *      random,' is used  with an upper bounds that is greater than the
 *      actual range size but is  a power of two. Then a modulus operation
 *      with the range size is done to the number that 'random' returns,
 *      thereby ensuring that a random number is generated within the
 *      random range size.
 *      
 *      The total number of bytes read or written on each pass of the
 *      exerciser is specified by the length in blocks or the
 *      starting/ending block  address option arguments.  If  neither the
 *      ending address nor the  length options are specified, then on each
 *      pass the number of bytes processed could vary depending on whether
 *      or not the filestream is being written to or just being read.  If
 *      the filestream is not being written to by exer, then exer will
 *      read until EOF is reached. If exer will be writing to the file (as
 *      specified in the action string), then the number of bytes
 *      processed per pass is equal  to the allocation size of the file
 *      which is usually larger than the length of the file for RAM disk
 *      files, but equal to the length for disk devices.
 *      
 *      Note that disk device I/O will fail, if the blocksize is not equal
 *      to  1 or a multiple of 512.  Partial block read/writes are not
 *      supported  so a length which is not a multiple of the blocksize
 *      will result in no errors, but the last partial block I/O of data
 *      won't occur.
 *      
 *      Any combination of writing, reading, or comparing the buffer1 and
 *      buffer2  can be executed in the sequence as specified in the 
 *      action string.  Depending on the option arguments, one or two of
 *      these three operations (read/write/compare) may be omitted without
 *      affecting the execution of the other operations.
 *      
 *      The exer command will return an error code  immediately after a
 *      read, write,  or compare error, if the 'd_harderr' environment 
 *      variable is set to 'halt'. When an error occurs and 'continue' or
 *      'loop on error' is specified, then  subsequent operations
 *       specified by the action string qualifier will occur except for
 *      compares.  For instance, if a read error occurs, a subsequent
 *      compare operations will be skipped since a read failure preceding
 *      a compare operation guarantees that the compare will fail. If
 *      subsequent block I/O's succeed, then compares of those blocks will
 *      occur.  When exer terminates because of completing all  passes or
 *      by operator termination, then the status returned will be that of
 *      the last failed write, read or compare operation, regardless of
 *      subsequent successful IO's.
-*/

/* the standard diagnostic qualifiers precede these exer specific ones */
#define	QV	NUM_QUALS +0	/* verbose, dump what's read to stdout */
#define	QD1	NUM_QUALS +1	/*string arg for eval to gen buf1 data pattern*/
#define	QD2	NUM_QUALS +2	/*string arg for eval to gen buf2 data pattern*/
#define	QM	NUM_QUALS +3	/* report timing metrics */
#define	QA	NUM_QUALS +4	/* action string */

#define	QSB	NUM_QUALS +5	/* Starting block number in filestream */
#define	QEB	NUM_QUALS +6	/* Ending   block number in filestream*/
#define	QL	NUM_QUALS +7	/* Total # of blocks to read, write, compare */
#define	QBS	NUM_QUALS +8	/* Block (packet) size in bytes */
#define	QBC	NUM_QUALS +9 	/* Block count, number of blocks per I/O */
#define	QSEC	NUM_QUALS +10	/* number of seconds to run */
#define QDEL	NUM_QUALS +11   /* number of milliseconds to delay with "s" in
				 * action string */
#define	OPT_QUALS	12
#define	EXER_QUALS	NUM_QUALS + OPT_QUALS

#define QSTRING	" v %sd1 %sd2 m %sa %xsb %xeb %xl %xbs %xbc %dsec %ddelay "

#define DEF_PKT_SZ 	512
#define SIX_MONTHS 1024*1024*16
#define ON  1
#define OFF 0

static    char		read_action[] = "?r";

/*----------*/
UINT gen_pkt_sz (UINT pkt_sz) {
    return ( (pkt_sz) ? pkt_sz : DEF_PKT_SZ);
}

/*----------*/
static struct EXER_LED_TBL {
	int	mask;
	char	*name;
    } exer_led_tbl [] = {
	{LEDS$M_CPU1,	"cpu1"},
	{LEDS$M_CPU2,	"cpu2"},
	{LEDS$M_MEM0,	"mem0"},
	{LEDS$M_MEM1,	"mem1"},
	{LEDS$M_MEM2,	"mem2"},
	{LEDS$M_MEM3,	"mem3"},
	{LEDS$M_FBUS,	"fbus"},
	{LEDS$M_IO,	""}
    };


/*
 *	EXER control flow:
 *		spawn (start) exer_kids, one per device
 *		wait for all kids to be ready
 *		start elapsed seconds timer
 *		bpost all kids to go
 *		wait for all kids done sems to be posted.
 *		  while waiting if our killpending bit set, kill the kids
 *		stop elapsed seconds timer
 *		compute and report IO summary if option is selected
 *		wait for all kids completion sems to be posted.
 *		exit
 *
 *	EXER_KID control flow:
 *  		fopen device
 *			if (error) 
 *			    post ready then done then completion sems & exit
 *		post this kid ready w/ our id#
 *		wait on go
 *		do exer_loop
 *		add our iob counts to grand iob
 *		post done semaphore w/ our id#
 *		always return our id# causing a post of completion sem w/ id#
 *		exit
 */

/*----------*/
int exer (int argc, char *argv []) {
    UINT                elapsed_msecs, io_per_sec, byte_per_sec;
    UINT     		start, stop; /* elapsed time millisecs counters */
    UINT     		idle_stop, idle_start; /* msecs of idle process */
    UINT     		time, i;
    struct QSTRUCT	qual[EXER_QUALS];
    struct PCB		*pcb, *idle_pcb;

    struct IOB		iob;	/* exer kids tally their counts here */
    struct TIMERQ 	tqe;
    struct SEMAPHORE	ready;  /* posted by exer kids after device open */
    struct SEMAPHORE	go;	/* signals exer children to start main loops */
    struct SEMAPHORE	kids_gone; /* process completion sem of exer kids */
    struct EXER_ARGS	exer_args; /*struct of arguments used by exer kids*/

    int			*kids_pid; /* array of exer kids PIDs */
    int			kill_sent; /* ==1 after telling exer kids to quit */
    int			write_mode;	/* == 0 if not writing to the device */
    unsigned int	status;
    char		answer[8];
    char		devltr[32];
    char		*act_strg;	/* action string pointer */

    int			kid_aff;
    int			kid_pri;
    int			tmp;
    int			exit_count;

    status = qscan(&argc, argv, "-", QSCAN_QUALIF_STRING QSTRING, qual);
    if (status != msg_success) {
	err_printf( status);
	return status;
    }
                               
#if MODULAR
	    diagsupport_shared_adr = ovly_load("DIAGSUPPORT");
	    if (!diagsupport_shared_adr)
		return msg_failure;
#endif

    if ( argc < 2 )
	status = msg_missing_device;

    kids_pid = malloc( sizeof(int) * argc );

    act_strg = (qual[QA].present) ? qual[QA].value.string : read_action;

    pcb = getpcb();

    write_mode = 0;
    if ((strchr( act_strg, 'w') != NUL) || (strchr( act_strg, 'W') != NUL) ||
        (strchr( act_strg, 'n') != NUL) || (strchr( act_strg, 'N') != NUL))
	write_mode = 1;

    if (status != msg_success) {
	err_printf (status); 
	return status; 
    }

    create_iob( &iob, "exer");
    krn$_seminit (&ready, 0, "exer_dvx_open");
    krn$_seminit (&go, 0, "exer_kid_go");
    krn$_seminit (&kids_gone, 0, "exer_kid_complete");

	/*
	 * Setup TQE & enqueue it on timer Q
	 */
    krn$_seminit (&tqe.sem, 0, "exer_tqe");
    krn$_init_timer( &tqe);

    exer_args.ready  = &ready;
    exer_args.go     = &go;
    exer_args.done   = &tqe.sem;
    exer_args.iob    = &iob;
    exer_args.status = msg_success;
    exer_args.pkt_sz = 0;

	/*
	 * Spawn exer children processes & store their PIDs.
	 */
    for (i=0; i<argc-1; i++) {
	kid_pri = pcb->pcb$l_pri;
	kid_aff = pcb->pcb$l_affinity;
#if RAWHIDE
        /* If device isn't one of DU, DR, DK, DV, MU, MR, or MK, drop the
	   priorities to 2/1 and set affinity to the primary only */
	devltr[0] = *(argv[i+1]);
        if ((devltr[0] != 'd') && (devltr[0] != 'm')) {
	    krn$_setpriority(pcb->pcb$l_pid, 2);	/* Parent's priority */
	    kid_pri = 1;				/* Kidlet's priority */
	    krn$_setaffinity(pcb->pcb$l_pid, pcb->pcb$l_affinity);
							/* Parent's affinity */
	} else {
	    krn$_setaffinity(pcb->pcb$l_pid, 0xf);	/* Parent's affinity */
	    kid_aff = 0xf;				/* Kidlet's affinity */
	}
#endif
    	kids_pid[i] = krn$_create( 
				exer_kid,		/* addr of proccess  */
				null_procedure,		/* startup routine   */
				&kids_gone,             /* completion semaphr*/
				kid_pri,                /* process priority  */
				kid_aff,                /* cpu affinity mask */
				0x2000,         	/* stack size        */
				"exer_kid",    		/* process name      */
				"nl", 0, 		/* stdin channel     */
				0, 0, 0, 0,		/* use our stdout/err*/
				&qual, &exer_args, 
				argv[i+1]); 		/* device name */
    }                                                       
    kids_pid[i] = 0;		/* mark end of array */

	/*
	 * Loop waiting for each exer kid to open its test device/file
	 */
    for (i=0; i<argc-1; i++) {
	krn$_wait (&ready);
    }
    krn$_semrelease (&ready);

	/* set number of seconds to run exer for */
    time = (qual[QSEC].present) ? qual[QSEC].value.integer : 0;
    if (time <= 0)
	time = SIX_MONTHS;

	/*
	 * Start timing and signal all exer kids to start their main loop
	 */
    krn$_start_timer( &tqe, 1000);	/* wakeup every second */
    idle_pcb = krn$_findpcb( idle_pids[whoami()]); /* ptr to idle process pcb */
    *(UINT *) &start = *(UINT *) &poweron_msec;
    *(UINT *) &idle_start = *(UINT *) &idle_pcb->pcb$q_cputime;
    spinunlock( &spl_kernel);	/* release spinlock acquired by krn$_findpcb */
    krn$_bpost( &go);

	/*
	 * Loop waiting for for all exer_kids to post done.
	 * Wakeup at timer intervals also.
	 * If this process is killed (such as by ^C) then kill the kids.
	 */
    kill_sent = 0;
    exit_count = 0;
    while (1) {
	tmp = krn$_wait (&tqe.sem);
	if (!(tmp & TIMEOUT$K_SEM_VAL))
	    exit_count++;
#if DEBUG2
	printf( "back from wait tqe, value:%x, kids:%d, exit_count:%d, time:%d\n", 
			tmp, argc-1, exit_count, time);
#endif
	krn$_start_timer( &tqe, 1000);	/* wakeup every second */
	if (exit_count == argc-1)
	    break;		/* all children processes expired */
	if ( killpending() || (--time == 0) ) {
#if DEBUG2
	printf( "killpending or time==0; time:%d\n", time);
#endif
	    if (!kill_sent) {
		kill_sent = 1;
		i = 0;
		while (kids_pid[i]) {
		    krn$_delete( kids_pid[i]);
		    i++;
		}
	    }
	}
    }

    spinlock( &spl_kernel);
    *(UINT *) &stop = *(UINT *) &poweron_msec;
    *(UINT *) &idle_stop = *(UINT *) &idle_pcb->pcb$q_cputime;
    spinunlock( &spl_kernel);
    krn$_stop_timer( &tqe);
    krn$_semrelease (&go);
#if DEBUG2
printf( "out of interval loop; kill_sent:%d, time:%d\n", kill_sent, time);
if ( killpending() ) {
	printf( "    ... and killpending is set\n");
	krn$_sleep( 200);
}
#endif

	/*
	 * Output summary counts if option selected.
	 */
    if (qual[QM].present) {
	printf( msg_exer_hd2);
	printf( msg_exer_hd3);

		/* compute elapsed msecs and # of idle process msecs */
	*(UINT *) &stop      -= *(UINT *) &start;
	*(UINT *) &idle_stop -= *(UINT *) &idle_start;
	elapsed_msecs = *( (UINT *) &stop);
	if (elapsed_msecs == 0)
	    elapsed_msecs = 1;

	i = *((UINT *) &iob.io_count) * 1000;
	io_per_sec = i / elapsed_msecs;
	if ( (i % elapsed_msecs) >= (elapsed_msecs / 2) )
	    io_per_sec++;

	byte_per_sec = *((UINT *) &iob.bytes_read) + *((UINT *) &iob.bytes_written);
	i = byte_per_sec * 1000;
	byte_per_sec = i / elapsed_msecs;
	if ( (i % elapsed_msecs) >= (elapsed_msecs / 2) )
	    byte_per_sec++;

	printf(msg_exer_counts, exer_args.pkt_sz, *(UINT *) &iob.io_count,
	    *(UINT *) &iob.bytes_read, *(UINT *) &iob.bytes_written,
	    io_per_sec, byte_per_sec, 
	    (*(UINT *) &stop + 500) / 1000, (*(UINT *) &idle_stop + 500) / 1000);
    }

	/*
	 * Loop waiting for for all exer_kids processes to vaporize.
	 */
    for (i=0; i<argc-1; i++) {
	krn$_wait (&kids_gone);
    }

    krn$_semrelease (&tqe.sem);
    krn$_release_timer( &tqe);
    krn$_semrelease (&kids_gone);	/* kids completion semaphore */
    remq( &iob.pcbq);
    remq_iob (&iob.sysq);

#if MODULAR
    ovly_remove("DIAGSUPPORT");
#endif
    return exer_args.status;
}

/*
 * Calculate packet size and bytes_to_process.
 * Take into consideration that files such as tapes may have
 * 0 length.  If bytes_to_process == 0 then process to EOF.
 */
void gen_sizes( struct QSTRUCT *qual, struct FILE *fp,
		UINT *pkt_sz,   UINT *bytes_to_process) {
    UINT                i;
    UINT                sb;		/* starting block number */
    UINT                bc;		/* block count per i/o */
    UINT		bs;		/* Device block size */
    UINT		bs_times_bc;	/* bs * bc */
    UINT		specified_blk_len;	/* set to 0 if qualifiers not present */
    UINT		file_size;

	/*
	 * 1st pass at calculating the number of blocks to process
	 * if (neither -eb nor -l present) then read for len of file
	 * or until eof, unless only compare has been specified
	 */
	sb = (qual[QSB].present) ? qual[QSB].value.integer : 0;

	specified_blk_len = 0;
	if (qual[QEB].present) {
	    specified_blk_len = qual[QEB].value.integer - sb + 1;

	}
	if (qual[QL].present)
	    specified_blk_len = qual[QL].value.integer;

	/* disallow values of 0 to bs or bc */
	bs = qual[QBS].present ? qual[QBS].value.integer :DEF_PKT_SZ;
	bs = bs ? bs : DEF_PKT_SZ;

	bc = qual[QBC].present ? qual[QBC].value.integer :1;
	bc = bc ? bc : 1;

	bs_times_bc = bs * bc;
	*pkt_sz = bs_times_bc;

	/* We don't know the length of sequential devices
	 * eg. tapes, uarts, ethernets.
	 */
	if (fp->ip->dva->sequential) {
	    if (specified_blk_len) {
		*bytes_to_process = bs * specified_blk_len;
	    } else {
		*bytes_to_process = 0;
	    }


	/* For disks and some other devices, the length is only valid
	 * when the file is open.
	 */
	} else {
	    file_size = * (UINT *) fp->ip->len;
	    if (file_size != 0) {
		*pkt_sz = umin64( file_size, bs_times_bc);
	    }
	    if (specified_blk_len) {
		*bytes_to_process = bs * specified_blk_len;
	    } else {
		*bytes_to_process = file_size - sb * bs;	
	    }
	}
#if DEBUG1       
printf( "pkt_sz %d bytes_to_proc %d\n", *pkt_sz, *bytes_to_process);
#endif
}

/*
 * There is exactly one exer kid process created for every device
 * specified on the command line.  The firwmare supports only
 * synchronous I/O within a process, so the way we get parallel
 * activity is to have multiple processes.
 */
void exer_kid (struct QSTRUCT *qual, struct EXER_ARGS *exer_args,
    		char *device_name) {
    int 		argc =2;
    char 		*argv [3];
    struct FILE		*fp;		/* filepath to exercise */
    struct PCB		*pcb;
    UINT                i;
    unsigned int	malloc_status, status;
    UINT                pkt_sz;		/* packet size in bytes per i/o    */
    UINT		bytes_to_process; /* total #of bytes to read/write   */
    UINT		rand_index;	/* index into LCG table */
    unsigned char	*bufs [2];
    char		mode[4];
    char		*act_strg;	/* action string pointer */
    UINT		tmp;
    int			bx,	
			write_mode;
    struct EVNODE     	ev, *evp;

    
    status = msg_success;
    argv[0] = "exer";
    argv[1] = device_name;
    argv[2] = 0;
    act_strg = (qual[QA].present) ? qual[QA].value.string : read_action;

    write_mode = 0;
    mode[0] = 'r';  mode[1] = '\0';  mode[2] = '\0';
    if (  (strchr( act_strg, 'w') != NUL) || (strchr( act_strg, 'W') != NUL)
       || (strchr( act_strg, 'n') != NUL) || (strchr( act_strg, 'N') != NUL) ) {
	mode[1] = '+';
	write_mode = 1;
    }

    if ( (fp = fopen (argv [1], mode)) == NULL)
	status = msg_bad_open;
    else {
	struct ZONE *zp;

	/* Get pointer to zone to malloc memory buffer from */
	zp = set_malloc_zone(fp);

	if ((zp == NULL) || (zp->size == 0)) {
	    err_printf(msg_dyn_badzone, zp);
	    krn$_post( exer_args->ready);
	    exer_args->status = msg_failure;
	    krn$_post( exer_args->done);
	    return;
	}

 	/* calculate pkt_sz, bytes_to_process */
	gen_sizes( qual, fp, &pkt_sz, &bytes_to_process);

	   /*
	    * Malloc and initialize buffer1 and buffer2.
	    * Malloc only 1 buf if that's all that's specified in action strg
	    */
	malloc_status = msg_success;

	for (bx=0; bx<2; bx++) {
	    bufs [bx] = dyn$_malloc( pkt_sz,
		DYN$K_SYNC | DYN$K_NOWAIT | DYN$K_ZONE | DYN$K_NOFLOOD,
		DYN$K_GRAN,
		0,
		zp
	    );
	    if (!bufs [bx]) {
		malloc_status = 1;
		break;
	    }

	    if (qual[QD1 + bx].present) {
		for (i=0; i < pkt_sz; i++) { /* use supplied data pattern */
		    if ((status = eval( qual[QD1 + bx].value.string, &tmp, 10)) 
			!= msg_success) {
		        err_printf( status);
			exer_args->status = status;
			break;
		    }
	            tmp &= 255;
	            bufs [bx] [i] = tmp;
		}
	    } else {
		memset (bufs [bx], 0x5A+i, pkt_sz);
	    }
	}

	if (malloc_status != msg_success) {
	    err_printf( msg_insuff_mem, pkt_sz);
	    status = exer_args->status = msg_insuff_mem;
#if MEDULLA 
	   /*
	    * Load up structure of known information, then log to NVRAM
	    *
	    */
	    error_blk.sio_blk.error_vector = 
			FORM_ERROR_VECTOR (0, 
					   MED$K_DNC_EXER,
					   1,
					   MED$K_DEC_MALLOC_FAILED);
	    error_blk.sio_blk.pass_count  = 0;
	    error_blk.sio_blk.reserved[0] = 0;
	    error_blk.sio_blk.reserved[1] = 0;
	    error_blk.sio_blk.reserved[2] = 0;
	    error_blk.sio_blk.reserved[3] = 0;
	    error_blk.sio_blk.reserved[4] = 0;
	    error_blk.sio_blk.reserved[5] = 0;
	    log_error (MED$K_SIO_ERROR, &error_blk, 0);
#endif
	}
    }	/* end else file opened successfully */

    pcb = getpcb();
    pcb->pcb$a_dis_table  = dispatch_table;
    pcb->pcb$a_sup_dev    = NULL;	/* all devices are legal */
    if (status == msg_success)
	status = diag_init (&argc, argv, qual, "", 1);

    if (status != msg_success) {
	krn$_post( exer_args->ready);
	exer_args->status = status;
	krn$_post( exer_args->done);
	return;
    }

#if TURBO 
/* Adding print for test script -- if d_verbose EV is 1 (means -q was not
   given), print starting message */
    evp = &ev;
    ev_read("d_verbose",&evp,0);
    if (evp->value.integer == 1) 
    { 
        if (write_mode == 1)
            printf(msg_dsk_ex_begin_dest, pcb->pcb$b_dev_name, pcb->pcb$l_pid);
        else
            printf(msg_dsk_ex_begin, pcb->pcb$b_dev_name, pcb->pcb$l_pid);
    }
#endif

	/* for passcount, loop doing i/o &/or buf compares */
    status = exer_loop( fp, qual, exer_args, bytes_to_process, pkt_sz,
		bufs [0], bufs [1], act_strg);
    return;
}

/*----------*/
int exer_loop (  
    struct FILE		*fp,		/* opened filepath to exercise */
    struct QSTRUCT	*qual,
    struct EXER_ARGS    *exer_args,
    UINT		len,		/* total #of bytes to read/write */
    UINT         	pkt_sz,		/* packet size in bytes */
    unsigned char	*buf1,
    unsigned char	*buf2,
    char		*act_strg_begin	/* beginning address of action string */
) {

    UINT 		elapsed_msecs, io_per_sec, byte_per_sec;
    UINT		i, passcount;
    UINT		len_left;	/* # of bytes left to process */
    struct PCB		*pcb;
    diag_common 	*diag;
    UINT		rand_index;	/* index into LCG table */
    UINT		seed;		/* random LCG table seed */
    UINT		status, rtn_status, bytes_written, bytes_rd;
    UINT		offset, start_offset;
    UINT         	bs;		/* device block size */
    UINT         	sz; /* # of bytes to read/write/compare for one i/o */
    UINT         	quit, no_err_yet, eof;
    UINT         	go_till_eof;
	/* pointers to quadwords to store cpu ticks in */
    char		*act_strg;	/* action string pointer */
    char		*buf;		/* buffer pointer */
    char		c;


    go_till_eof = 0;
    if (len == 0) {
	go_till_eof = 1; 	/* handle tape which may have no alloc size */
	len = DEF_PKT_SZ;	/* need any non-zero len to kludge for loop */
    }

    /* disallow values of 0 to bs or bc */
    bs = qual[QBS].present ? qual[QBS].value.integer :DEF_PKT_SZ;
    bs = bs ? bs : DEF_PKT_SZ;

    start_offset = ( (qual[QSB].present) ? qual[QSB].value.integer : 0) * bs;

    /* Determine the number of passes we need to do.  Default to a pass of
     * of 1.  If the user specifies a passcount of 0, interpret that as 
     * forever. (In practice, this results in testing for INT_MAX passes,
     * which is a LONG TIME with 64 bit integers).
     */
    passcount = 1;
    if (qual[QP].present) {
	passcount = qual[QP].value.integer;
    }


    /* choose an lcg table index for random lbn seeks within a block range */
    /* rand# = (seed % (eb - sb - bc + 1) + sb) * bs  */
    seed = 0;
    rand_index = log2 (len / bs);

    quit = 0;
    rtn_status = msg_success;
    pcb = getpcb();
    diag = (diag_common *) pcb->pcb$a_diag_common;
    diag->diag$a_ext_error  = NULL;	/* extended error reporting not used */
    pcb->pcb$a_rundown    = exer_cleanup;
    diag->diag$l_error_type = DIAG$K_HARD;

    krn$_post( exer_args->ready);
    krn$_wait( exer_args->go);	/* wait for all other exer_kids to be ready */

    /*
     * main loop; repeat for what passcount is set to
     */
    do {
	eof = 0;
	offset = start_offset;
	fseek (fp, start_offset, SEEK_SET);

	for (len_left =len; len_left!=0; (go_till_eof) ? 0 : (len_left -= sz)) {
	 bytes_rd = bytes_written = sz = umin64( gen_pkt_sz( pkt_sz), len_left);
	    no_err_yet = 1;
	/* perform each action as specified by a char in the action string */
	    act_strg = act_strg_begin;
	    while ( (c = *(act_strg++)) && (!quit) ) {
		int *b_ptr;
		int b_cnt;
		int v;
#if DEBUG
		err_printf( "%c",c);
#endif
		switch ((v=action_table [c & 0x7f])) {

		case 1:	/* R - read into buffer2 */
		case 2:	/* r - read into buffer1 */
		    if (action_table[c & 0x7f] == 1)
			 buf = buf2;
		    else buf = buf1;

		    offset = *(UINT *)fp->offset;
		    bytes_rd = fread( buf, sz, 1, fp);
		    if ( (qual[QV].present) && (bytes_rd > 0) )
			fwrite( buf, bytes_rd, 1, pcb->pcb$a_stdout);
		    if (bytes_rd == 0) {
#if MODULAR
		        if ( (fp->status == msg_success) 
			  || (!strcmp(fp->status,msg_eof))
			  || (!strcmp(fp->status,msg_eot)) ) 
#else
		        if ( (fp->status == msg_success) 
			  || (fp->status == msg_eof)
			  || (fp->status == msg_eot) ) 
#endif
			    	eof = 1;
		        else {				/* error */
			    	no_err_yet = 0;
   			    	diag->diag$l_error_num = 5;
   			    	diag->diag$l_error_msg = msg_success;
			    	rtn_status = msg_ex_devreaderr;
#if MEDULLA 
	   			/*
	    			 * Load up structure then log to NVRAM
	    			 *
	    			 */
	    			error_blk.sio_blk.error_vector = 
				  FORM_ERROR_VECTOR (0, 
						     MED$K_DNC_EXER,
					   	     2,
					   	     MED$K_DEC_NO_BYTES_READ);
	    			error_blk.sio_blk.pass_count  = 
					diag->diag$l_pass_count;
	    			error_blk.sio_blk.reserved[0] = 0;
	    			error_blk.sio_blk.reserved[1] = 0;
	    			error_blk.sio_blk.reserved[2] = 0;
	    			error_blk.sio_blk.reserved[3] = 0;
	    			error_blk.sio_blk.reserved[4] = 0;
	    			error_blk.sio_blk.reserved[5] = 0;
	    			log_error (MED$K_SIO_ERROR, &error_blk, 0);

				/*
				 * Fill in fields since Sio_Error isn't called
				 *
				 */
				diag->diag$l_error_type = DIAG$K_HARD;
   			    	diag->diag$l_error_num  = 
				  FORM_ERROR_VECTOR (0, 
						     MED$K_DNC_EXER,
					   	     2,
					   	     MED$K_DEC_NO_BYTES_READ);
   			    	diag->diag$l_error_msg = NULL;

#endif
			    	report_error_lock( );
			    	err_printf( msg_ex_devreaderr, bytes_rd,
			    	        *fp->offset, fp->ip->name);
			    	status = report_error_release( );
			    	if (status == msg_halt)
				    quit = 1;
			    }
		    }
		    break;

	        case 3: case 4: case 5: case 6: /* write or write_nolock */
		    offset = *(UINT *)fp->offset;

		    switch (action_table[c & 0x7f]) {
		    case 3:	/* W - write from buffer2 */
		    case 5:	/* N - write w/o lock from buffer2 */
		        bytes_written = fwrite( buf2, sz, 1, fp);
		        break;
		    case 4:	/* w - write from buffer1 */
		    case 6:	/* n - write w/o lock from buffer1 */
		        bytes_written = fwrite( buf1, sz, 1, fp);
		        break;
		    } /* end inner switch */

		    if (bytes_written == 0)
		        if ((fp->status == msg_eof) || (fp->status == msg_eot))
		            eof = 1;
		        else {
			    no_err_yet = 0;
   			    diag->diag$l_error_num = 6;
   			    diag->diag$l_error_msg = msg_success;
			    rtn_status = msg_dep_devwriteerr;
#if MEDULLA
	   		    /*
	    		     * Load up structure then log to NVRAM
	    		     *
	    		     */
	    		    error_blk.sio_blk.error_vector = 
			    FORM_ERROR_VECTOR (0, 
			    		     MED$K_DNC_EXER,
			    	   	     3,
			    	   	     MED$K_DEC_NO_BYTES_WRITTEN);
	    		    error_blk.sio_blk.pass_count  = 
			    	diag->diag$l_pass_count;
	    		    error_blk.sio_blk.reserved[0] = 0;
	    		    error_blk.sio_blk.reserved[1] = 0;
	    		    error_blk.sio_blk.reserved[2] = 0;
	    		    error_blk.sio_blk.reserved[3] = 0;
	    		    error_blk.sio_blk.reserved[4] = 0;
	    		    error_blk.sio_blk.reserved[5] = 0;
	    		    log_error (MED$K_SIO_ERROR, &error_blk, 0);

			    /*
			     * Fill in fields since Sio_Error isn't called
			     *
			     */
			    diag->diag$l_error_type = DIAG$K_HARD;
   			    diag->diag$l_error_num  = 
				 FORM_ERROR_VECTOR (0, 
						    MED$K_DNC_EXER,
					   	    3,
					   	    MED$K_DEC_NO_BYTES_WRITTEN);
   			    diag->diag$l_error_msg = NULL;
#endif
			    report_error_lock( );
			    err_printf( msg_dep_devwriteerr,
				      bytes_written, *fp->offset, fp->ip->name);
			    status = report_error_release( );
			    if (status == msg_halt)
				quit = 1;
		        }
		    break;

	        case 7:	/* c - compare buffer1 and buffer2 */
		    if (no_err_yet) /* skip compare if read or write failed */
			if (bytes_rd != bytes_written) {
   			    diag->diag$l_error_num = 8;
   			    diag->diag$l_error_msg = msg_success;
			    rtn_status = msg_sz_mismatch;
#if MEDULLA 
	   		    /*
	    		     * Load up structure then log to NVRAM
	    		     *
	    		     */
	    		    error_blk.sio_blk.error_vector = 
			    FORM_ERROR_VECTOR (0, 
			    		     MED$K_DNC_EXER,
			    	   	     4,
			    	   	     MED$K_DEC_NOBYTES_MATCH);
	    		    error_blk.sio_blk.pass_count  = 
			    	diag->diag$l_pass_count;
	    		    error_blk.sio_blk.reserved[0] = 0;
	    		    error_blk.sio_blk.reserved[1] = 0;
	    		    error_blk.sio_blk.reserved[2] = 0;
	    		    error_blk.sio_blk.reserved[3] = 0;
	    		    error_blk.sio_blk.reserved[4] = 0;
	    		    error_blk.sio_blk.reserved[5] = 0;
	    		    log_error (MED$K_SIO_ERROR, &error_blk, 0);

			    /*
			     * Fill in fields since Sio_Error isn't called
			     *
			     */
			    diag->diag$l_error_type = DIAG$K_HARD;
   			    diag->diag$l_error_num  = 
				 FORM_ERROR_VECTOR (0, 
						    MED$K_DNC_EXER,
					   	    4,
					   	    MED$K_DEC_NOBYTES_MATCH);
   			    diag->diag$l_error_msg = NULL;
#endif
			    report_error_lock( );
			    err_printf( msg_sz_mismatch,
			          bytes_rd, bytes_written, *fp->offset);
			    status = report_error_release( );
			    if (status == msg_halt)
				quit = 1;
			} else 
			if (memcmp( buf1, buf2, bytes_rd)) {
   			    diag->diag$l_error_num = 7;
   			    diag->diag$l_error_msg=rtn_status=msg_d_data_comp_err;
#if MEDULLA
	   		    /*
	    		     * Load up structure then log to NVRAM
	    		     *
	    		     */
	    		    error_blk.sio_blk.error_vector = 
			    FORM_ERROR_VECTOR (0, 
			    		     MED$K_DNC_EXER,
			    	   	     5,
			    	   	     MED$K_DEC_DATA_COMPARE);
	    		    error_blk.sio_blk.pass_count  = 
			    	diag->diag$l_pass_count;
	    		    error_blk.sio_blk.reserved[0] = 0;
	    		    error_blk.sio_blk.reserved[1] = 0;
	    		    error_blk.sio_blk.reserved[2] = 0;
	    		    error_blk.sio_blk.reserved[3] = 0;
	    		    error_blk.sio_blk.reserved[4] = 0;
	    		    error_blk.sio_blk.reserved[5] = 0;
	    		    log_error (MED$K_SIO_ERROR, &error_blk, 0);

			    /*
			     * Fill in fields since Sio_Error isn't called
			     *
			     */
		 	    diag->diag$l_error_type = DIAG$K_HARD;
   			    diag->diag$l_error_num  = 
			 	FORM_ERROR_VECTOR (0, 
						   MED$K_DNC_EXER,
					   	   5,
					   	   MED$K_DEC_DATA_COMPARE);
   			    diag->diag$l_error_msg = NULL;
#endif
			    report_error_lock( );
			    err_printf( msg_data_comp_err,
			          bytes_rd, *fp->offset, fp->ip->name);
			    err_printf( msg_expect_buf );
			    hd_helper( pcb->pcb$a_stderr, 0, buf1, bytes_rd, 1, 0);
			    err_printf( msg_receive_buf );
			    hd_helper( pcb->pcb$a_stderr, 0, buf2, bytes_rd, 1, 0);

			    status = report_error_release( );
			    if (status == msg_halt)
				quit = 1;
			}
		    break;

	        case 8:	/* - - seek to file offset prior to last IO */
		    fseek (fp, offset, SEEK_SET);
		    break;

	        case 9:	/* ? - seek to random lbn within legitimate range  */
			/* map new seed from larger random # range into  */
			/* (random_blk_range - 1).  If the range is not a */
			/* power of 2, then this code won't cover the entire */
			/* range.	*/
		    seed = random( seed, rand_index);
		    fseek( fp, seed * bs + start_offset, SEEK_SET);
		    break;

	        case 10:	/* D - */
		    break;	/* D not implemented yet */

	        case 11:	/* d - */
		    break;	/* d not implemented yet */

	        case 12:	/* m - */
		    break;	/* m not implemented yet */

	        case 13:	/* M - */
		    break;	/* M not implemented yet */

		case 14: /* s - delay a number of milliseconds specified by */
			 /* the -delay qualifier.  Default is 1 millisecond */
		    if(qual[QDEL].present)
		        krn$_sleep(qual[QDEL].value.integer);
		    else
			krn$_sleep(1);
		    break;

		case 15:	/* Z - zero buffer2 */
		case 16:	/* z - zero buffer1 */
		    if (action_table[c & 0x7f] == 16)
		      buf = buf1;
		    else
		      buf = buf2;

		    /* err_printf("zero buffer = %x(ptr)\n",buf); */

		    memset( buf , 0 , pkt_sz );
		    break;

		case 17:       /* B - increment buffer 2 */
		case 18:       /* b - increment buffer 1 */
		    if (action_table[c & 0x7f] == 18)
		      buf = buf1;
		    else
		      buf = buf2;
		    b_ptr = (int *)buf;
		    b_cnt = pkt_sz/sizeof(int);
		    while ( b_cnt-- > 0 )
			*b_ptr++ += 0x01020304;
		    break;


  	        default:	/* error, character not recognized */
		    err_printf( msg_bad_action);
		    rtn_status = msg_bad_action;
		    quit = 1;
		    break;

	        } /* end switch */

	    } /* end while  */

	    if (killpending() )
		quit = 1;
	    if ( (eof) || (quit) ) break;
	} /* end inner for loop */
	if (quit) break;
	diag->diag$l_pass_count++;
	diag_print_end_of_pass();
    } while (--passcount);


	/* add this exer_kid's counts to grand iob count */
    spinlock( &spl_iobq);
    *((UINT *) exer_args->iob->io_count)      += *((UINT *) &fp->iob.io_count);
    *((UINT *) exer_args->iob->bytes_read)    += *((UINT *) &fp->iob.bytes_read);
    *((UINT *) exer_args->iob->bytes_written) += *((UINT *) &fp->iob.bytes_written);
    exer_args->pkt_sz = pkt_sz;
    if (rtn_status != msg_success)
	exer_args->status = rtn_status;
    spinunlock( &spl_iobq);

    if (qual[QM].present)
	printf( msg_exer_finis, fp->ip->name);

    krn$_post( exer_args->done);

    return rtn_status;
}

#undef	QV
#undef	QD1
#undef	QD2
#undef	QM
#undef	QA
#undef	QSB 
#undef	QEB 
#undef	QL 
#undef	QBS 
#undef	QBC
#undef	QSEC
#undef  QDEL
#undef	OPT_QUALS
#undef	EXER_QUALS
#undef  DEBUG
#undef  DEBUG1
#undef  DEBUG2
#undef SIX_MONTHS
#undef ON
#undef OFF
/*------------------------------------------------------------------------------
  Cleanup code.

  This code is automatically called during rundown of the diagnostic process.
  This code should leave the hardware in a known state, as well as cleaning
  up any software state.
------------------------------------------------------------------------------*/

void exer_cleanup ( ) {
#if TURBO
    struct EVNODE     	ev, *evp;
    struct PCB		*pcb=getpcb();
    
    evp = &ev;
	ev_read("d_verbose",&evp,0);
	if (evp->value.integer == 1) 
	    printf(msg_dsk_ex_done, pcb->pcb$b_dev_name, pcb->pcb$l_pid);
#endif
	diag_rundown();
}
