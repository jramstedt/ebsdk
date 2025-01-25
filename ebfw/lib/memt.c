/*
 * memtest.c, Robert Nix, December 1993
 *
 * Digital Company Confidential
 *
 * Partially based on the cbash program by Kirk Johnson @ MIT
 *
 * Usage: memtest <machname> <iterations> <max-mem>
 *    machname   - a short indentifier for the machine being tested.
 *    iterations - target number of iterations to run for stable timing.
 *    max-mem    - maximum working set size to test.
 *
 *    Iterations and max-mem can be specified with a "k" or "m" suffix
 *    for kilo or mega iterations/mem. 
 *
 * Example: Test of sandpiper+ (3000/600) with 8m iterations and 16m max-mem.
 *
 * memtest 3600 8m 16m
 *
 * Output:
 * ----------------------------------------------------------------------------------
 *                     4k   8k  16k  32k  64k 128k 256k 512k   1m   2m   4m   8m  16m
 * L 3600           4   17   18   27   27   27   27   27   27   27   28   52   52   52
 * L 3600           8   18   18   35   35   35   35   35   35   36   37   83   84   83
 * L 3600          16   18   18   51   51   51   51   52   52   52   54  146  146  146
 * L 3600          32   18   18   62   63   63   63   64   64   65   68  292  293  292
 * L 3600          64   19   18   63   63   62   62   64   64   65   67  292  293  294
 * L 3600         128   20   19   63   63   62   62   66   66   66   69  293  294  294
 * L 3600         256   24   20   64   63   63   62   68   68   69   71  296  298  298
 * L 3600         512   30   23   65   64   63   63   74   74   74   76  302  304  304
 * L 3600          1k   32   30   69   66   64   63   85   84   84   87  315  316  317
 * L 3600          2k   71   32   76   69   66   64  107  106  107  109  342  344  343
 * L 3600          4k   93   71   78   76   69   66  153  153  152  155  383  388  396
 * L 3600          8k   93   93  115   79   76   69  250  247  245  248  463  465  479
 * L 3600         16k   93   92   92  114   79   76   69  249  247  245  459  461  485
 * L 3600         32k   92   93   92   92  114   78   76   69  249  247  294  354  482
 * 
 *                      4k   8k  16k  32k  64k 128k 256k 512k   1m   2m   4m   8m  16m
 * S 3600           4   16   16   16   16   16   16   16   16   16   16   41   42   42
 * S 3600           8   22   22   22   22   22   22   22   22   22   23  103  103  103
 * S 3600          16   44   44   43   43   43   43   44   44   45   47  207  207  207
 * S 3600          32   58   58   58   58   58   58   58   59   60   66  411  413  414
 * S 3600          64   58   58   58   58   58   58   58   59   59   63  413  414  415
 * S 3600         128   58   58   58   58   58   58   58   58   59   62  414  415  415
 * S 3600         256   58   58   58   58   58   58   58   58   59   62  414  415  417
 * S 3600         512   59   58   58   58   58   58   59   59   59   62  415  415  417
 * S 3600          1k   58   58   58   58   58   58   59   59   59   62  415  416  418
 * S 3600          2k   70   58   58   58   58   58   61   60   60   62  415  417  423
 * S 3600          4k   87   70   58   58   58   58  105  105  104  106  416  419  427
 * S 3600          8k   86   87   69   58   58   58  193  192  191  191  415  419  438
 * S 3600         16k   87   87   87   69   58   58   58  193  192  191  408  413  457
 * S 3600         32k   87   87   87   87   69   58   58   58  193  192  233  289  528
 * 
 *                      4k   8k  16k  32k  64k 128k 256k 512k   1m   2m   4m   8m  16m
 * B 3600    load_8   1025 1025  273  273  273  273  264  273  264  256   90   91   47
 * B 3600    copyi_8   546  546  335  342  342  328  321  335  321  321  140  139   70
 * B 3600    copy_8    656  683  328  321  321  328  321  321  315  309  124  104   49
 * B 3600    store_8   328  315  328  315  328  315  315  328  328  315   91   92   46
 * B 3600    load_4    455  431  186  186  174  191  186  182  186  174   80   80   40
 * B 3600    copyi_4   546  546  238  238  238  238  241  238  238  225  132  133   67
 * B 3600    copy_4    431  410  273  273  278  278  273  273  273  264  118  101   47
 * B 3600    store_4   356  342  342  356  356  356  342  328  342  342   92   92   46
 * B 3600    memchr    182  182  128  128  128  128  128  128  126  126   74   74   37
 * B 3600    memcmp    431  431  288  256  241  231  228  225  225  219  119   97   45
 * B 3600    memcpy    631  631  349  335  342  328  328  335  335  342  123  103   49
 * B 3600    memset    342  356  342  356  356  342  342  342  328  342   92   92   46
 * ----------------------------------------------------------------------------------
 * Explanation of output.
 *
 * There are three kinds of tests.
 *
 * L - Load latency tests
 *     Measures the average repetition rate, in ns, of a latency-oriented load
 *     loop.  The two main variables are:
 *
 *        (1) working set, or the amount of memory touched by the loop.  This
 *            varies across the columns in the output above, from a low of 4k
 *            bytes to a high of max-mem, or 16m bytes.
 *
 *        (2) stride, or the the number of bytes separating successive loads.
 *            This is the number in the 3rd column of each of the "L" rows
 *            in the output above, and varies from 4 bytes to 32k bytes.
 * 
 * S - Store latency tests.
 *     Measures the average repetition rate, in ns, of a store loop. Same
 *     variables as the load latency tests.
 *
 * B - Bandwidth tests.
 *     Measures the bandwidth, in Mb/sec, of a dozen memory bandwidth limited
 *     loops.  The two main variables are:
 *
 *        (1) working set, varying across the columns.
 *     
 *        (2) The type of bandwidth test.  The first eight are hand-software
 *            pipelined loops with source in this file, the last four are
 *            standard functions in libc that are included here mainly to
 *            cross-check the code quality of the first eight.  The eight
 *            loops apply four tests -- load, copy/copyi, store -- to
 *            two data types -- 8-byte floating point, four-byte integer.
 *            The two copy loops both copy memory "a" to memory "b" within
 *            the working set: in a `copyi' "b" is in fact the same memory
 *            as "a", while in a `copy' the two pointers are different.
 *
 *
 * Compilation notes:
 *
 * Timing: for more precise timing set the compilation flag -DGETRUSAGE.
 * If getrusage() is not available, this program will use the less-precise,
 * but Posix-standard, clock() function.  The tests must then be run with
 * higher values of "iters" to get stable timing. OSF supports getrusage();
 * NT does not.
 *
 * Code quality:  These tests are sensitive to the quality of code
 * generated by the compiler.  The test results also include the loop control
 * overhead.  This overhead can be factored out of the latency tests by
 * simply subtracting off a known latency, the L1 latency on a cache-line
 * sized stride, from all results.  The bandwidth results can't be adjusted
 * in such a simple way -- these tests simply require a good optimizing compiler.
 * I've successfully compiled the bandwidth tests to run at ~80% of hardware peak on
 * Alpha OSF, Alpha NT, HP Snake Unix, IBM Power1 Unix, and Pentium NT; but
 * getting that performance required fiddling with source, switches, and careful 
 * checking against expected results.
 */

#ifndef LINT
static char *rcsid = "$Id: memt.c,v 1.1.1.1 1998/12/29 21:36:11 paradis Exp $";
#endif

/*
 * $Log: memt.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:11  paradis
 * Initial CVS checkin
 *
 * Revision 1.1  1997/12/09  20:36:52  pbell
 * nre
 *
 * Revision 1.3  1997/10/01  10:07:51  fdh
 * Converted floating point calculations to integer
 * calculations inside of timing loops.
 *
 * Revision 1.2  1997/10/01  09:21:13  fdh
 * Added Debug Monitor conditionals.
 * Modified output to fit onto an 80 column screen.
 * Free malloced buffer.
 *
 */

#include "lib.h"
#include "system.h"

#define	WAIT		2		/* measurement interval	  					*/
#define	MAX_TIME        10		/* max allowed actual time, well less than 2**32 cycles		*/
#define	BIG_NUMBER	4294967295	/* 2**32 - 1 , largest unsigned 32 bit integer			*/

long	measure_clock_();		/* Function in this file to measure the cpu clock period	*/

static	long	clock_period = -1;	/* CPU clock period (picosecs). Negative indicates not yet measured	*/
static	unsigned long	cpu_start;	/* pcc reading at CPU time zero					*/
static	unsigned long	wall_start;	/* pcc reading at wall clock time zero				*/

#ifdef __GNUC__
#define __RPCC __builtin_alpha_rpcc
#else
unsigned long __RPCC();
#endif

long	cpu_time_()
{

  static   int	init = 0;	/* Identifies the first call to this routine		*/	
  unsigned long	pcc;		/* value from read of the process cycle counter		*/
  unsigned int	upper_pcc;	/* Bits <63:32> of pcc					*/
  unsigned int	lower_pcc;	/* Bits <31:0 > of pcc					*/
  unsigned long	cycles;		/* current cycles used					*/
  unsigned long	diff;		/* cycle difference from first read of pcc		*/
  long status;			/* return status from measure_clock_()			*/

  if ( init != 0 ) {
    pcc = __RPCC();               				/* read pcc		*/
    upper_pcc = pcc>>32;					/* copy upper 32 bits	*/
    lower_pcc = pcc & 0XFFFFFFFF;				/* copy lower 32 bits	*/
    cycles = (upper_pcc+lower_pcc) & 0XFFFFFFFF;		/* current cycles	*/

    if (cycles > cpu_start)
      diff = cycles - cpu_start;
    else
    {
      diff = cpu_start - cycles;
      diff = BIG_NUMBER - diff;
    }
    cpu_start = cycles;
    return (diff * clock_period);		/* return cycles timed		*/
	
  }

  else {
    init = 1;
    if ( clock_period < 0 )
    {
      status = measure_clock_();	/* return 0 on error		*/
      if (status < 0 )
      {
        printf("\n The system is too busy for an accurate measurement\n");
        exit(0);			/* ERROR. TERMINATE PROGRAM	*/
      }
      
      else
        printf( "\nCycle time %5.2fns\n\n", clock_period/1.0e3);	/* print the period		*/
    }


    pcc = __RPCC();               				/* read pcc		*/
    upper_pcc = pcc>>32;					/* copy upper 32 bits	*/
    lower_pcc = pcc & 0XFFFFFFFF;				/* copy lower 32 bits	*/
    cpu_start = ((upper_pcc+lower_pcc) & 0XFFFFFFFF);	/* current cycles	*/
    return	0;
  }
}

long	measure_clock_()
{
  if (sysdata.valid) {
    clock_period = (*sysdata.cycle_cnt);
  }

/* Return the clock_period	*/
  return clock_period;
}


#define DEF_MAXMEM 16777216
#define MINMEM 4096
#define ITYPE signed int
signed long     max_mem;
char           *mach_name;

#define MAXSTRIDE 32768
#define MINSTRIDE 4

char           *version_string = "1.0 (20 Dec 1993)";
extern ITYPE  arg_to_int(char *);
extern double bash(char *, long, long, long);
extern double store_bash(char *, long, long, long);
extern int bash_loop(char *, long, long, long);
extern int store_bash_loop(char *, long, long, long);
extern void allocate_memory(char *, long);
extern void usage(char *);
extern void bw_driver(ITYPE((*f) (char *, ITYPE, ITYPE)), char *, char *, ITYPE, ITYPE);
#ifndef NOLIBS
extern ITYPE bw_memset(char *, ITYPE, ITYPE);
extern ITYPE bw_memcpy(char *, ITYPE, ITYPE);
extern ITYPE bw_memchr(char *, ITYPE, ITYPE);
extern ITYPE bw_memcmp(char *, ITYPE, ITYPE);
#endif
extern ITYPE bw_int_ld(char *, ITYPE, ITYPE);
extern ITYPE bw_double_ld(char *, ITYPE, ITYPE);
extern ITYPE bw_int_store(char *, ITYPE, ITYPE);
extern ITYPE bw_double_store(char *, ITYPE, ITYPE);
extern ITYPE bw_int_copyi(char *, ITYPE, ITYPE);
extern ITYPE bw_double_copyi(char *, ITYPE, ITYPE);
extern ITYPE bw_int_copy(char *, ITYPE, ITYPE);
extern ITYPE bw_double_copy(char *, ITYPE, ITYPE);

int mt(
     int argc,
     char *argv[]
)
{
    ITYPE           nbytes;
    ITYPE           stride;
    ITYPE           iters;
    char           *region, *buffer;

    printf("mt - version (%s)\n", rcsid);

    if ((argc > 1) && (strcmp(argv[1], "-v") == 0)) {
	fprintf(stderr, "This is memtest version %s.\n", version_string);
	exit(1);
    }
    mach_name = TARGET_NAME;
    iters = 1;
    if (argc < 4) {
	max_mem = DEF_MAXMEM;
    } else {
	max_mem = arg_to_int(argv[3]);
    }
    buffer = (char *) malloc(max_mem+(128*1024));
    region = (char *) ((((long) buffer) + (128*1024-1)) & ~((128*1024)-1));
    if (region == NULL) {
	perror("malloc failed");
	exit(1);
    }
    (void) cpu_time_();

    printf("  %6s", "");
    printf("%7s", "");
    for (nbytes = MINMEM; nbytes <= max_mem; nbytes += nbytes) {
	if (nbytes >= (1024 * 1024))
	    printf("%4dm", nbytes / (1024 * 1024));
	else if (nbytes >= 1024)
	    printf("%4dk", nbytes / 1024);
	else
	    printf("%5d", nbytes);
    }
    printf("\n");
    for (stride = MINSTRIDE; stride <= MAXSTRIDE; stride += stride) {
	printf("L %-6s", mach_name);
	if (stride >= (1024 * 1024))
	    printf("%6dm", stride / (1024 * 1024));
	else if (stride >= 1024)
	    printf("%6dk", stride / 1024);
	else
	    printf("%7d", stride);
	for (nbytes = MINMEM; nbytes <= max_mem; nbytes += nbytes) {
            double ns_ref = bash(region, nbytes, stride, iters);
	    printf(" %4.0f", ns_ref);
	    fflush(stdout);
	}
	printf("\n");
    }
    printf("\n");
    printf("  %6s", "");
    printf("%7s", "");
    for (nbytes = MINMEM; nbytes <= max_mem; nbytes += nbytes) {
	if (nbytes >= (1024 * 1024))
	    printf("%4dm", nbytes / (1024 * 1024));
	else if (nbytes >= 1024)
	    printf("%4dk", nbytes / 1024);
	else
	    printf("%5d", nbytes);
    }
    printf("\n");
    for (stride = MINSTRIDE; stride <= MAXSTRIDE; stride += stride) {
	printf("S %-6s", mach_name);
	if (stride >= (1024 * 1024))
	    printf("%6dm", stride / (1024 * 1024));
	else if (stride >= 1024)
	    printf("%6dk", stride / 1024);
	else
	    printf("%7d", stride);
	for (nbytes = MINMEM; nbytes <= max_mem; nbytes += nbytes) {
            double  ns_ref = store_bash(region, nbytes, stride, iters);
	    printf(" %4.0f", ns_ref);
	    fflush(stdout);
	}
	printf("\n");
    }
    printf("\n");
    printf("  %6s", "");
    printf("%7s", "");
    for (nbytes = MINMEM; nbytes <= max_mem; nbytes += nbytes) {
	if (nbytes >= (1024 * 1024))
	    printf("%4dm", nbytes / (1024 * 1024));
	else if (nbytes >= 1024)
	    printf("%4dk", nbytes / 1024);
	else
	    printf("%5d", nbytes);
    }
    printf("\n");
    iters = iters / ((1024 * 1024)/8);
    if (iters <= 0)
	iters = 1;
    bw_driver(bw_double_ld, "load_8", region, iters, 1);
    bw_driver(bw_double_copyi, "copyi_8", region, iters, 2);
    bw_driver(bw_double_copy, "copy_8", region, iters, 2);
    bw_driver(bw_double_store, "store_8", region, iters, 1);
    bw_driver(bw_int_ld, "load_4", region, iters, 1);
    bw_driver(bw_int_copyi, "copyi_4", region, iters, 2);
    bw_driver(bw_int_copy, "copy_4", region, iters, 2);
    bw_driver(bw_int_store, "store_4", region, iters, 1);
#ifndef NOLIBS
    bw_driver(bw_memchr, "memchr", region, iters, 1);
    bw_driver(bw_memcmp, "memcmp", region, iters, 2);
    bw_driver(bw_memcpy, "memcpy", region, iters, 2);
    bw_driver(bw_memset, "memset", region, iters, 1);
#endif

    free(buffer);
    return 0;
}

ITYPE
arg_to_int(char *arg)
{
    ITYPE           rslt = 0;
    ITYPE           mult = 1;

    switch (arg[strlen(arg) - 1]) {
    case 'k':
    case 'K':
	mult = 1024;
	break;

    case 'm':
    case 'M':
	mult = 1024 * 1024;
	break;

    default:
	mult = 1;
	break;
    }
    if (!((arg[0] >= '0') && arg[0] <= '9')) {
	fprintf(stderr, "Argument %s not a number\n", arg);
	usage("memtest");
	exit(1);
    }
    sscanf(arg, "%ld", &rslt);
    rslt *= mult;
    return rslt;
}

void start_timing ()
{
    cpu_time_();
}

long end_timing()
{
  long utime = 0, stime = 0;
    utime = cpu_time_();
    return utime + stime;
}


double
bash(
     char *region,
     long nbytes,	/* size of region to bash (bytes) */
     long stride,	/* stride through region (bytes)  */
     long iters		/* target # of loop iterations    */
)
{
signed long     count;
signed long     reps;
long time;
    count = ((nbytes - sizeof(int)) / stride) + 1;
    if (! (((count - 1) * stride + sizeof(int)) <= nbytes)) {
	fprintf(stderr, "trip count problem\n");
	exit(1);
    }
    reps = (iters + count - 1) / count;
    if (reps <= 0)
	reps = 1;
    iters = reps * count;

    /* make sure the memory is allocated */
    memset(region, 0, nbytes);
    memset(region, 1, nbytes);
    allocate_memory(region, nbytes);
    memset(region, 0, nbytes);
    /* warm up the cache */
    (void) bash_loop(region, count, stride, 1L);

    start_timing();
    (void) bash_loop(region, count, stride, reps);
    time = end_timing();

    return (time/(iters*1e3));
}

double
store_bash(
	   char *region,
	   long nbytes,	/* size of region to bash (bytes) */
	   long stride,	/* stride through region (bytes)  */
	   long iters	/* target # of loop iterations    */
)
{
signed long     count;
signed long     reps;
double time;

    count = ((nbytes - sizeof(int)) / stride) + 1;
    if (!(((count - 1) * stride + sizeof(int)) <= nbytes)) {
	fprintf(stderr, "trip count problem\n");
	exit(1);
    }
    reps = (iters + count - 1) / count;
    if (reps <= 0)
	reps = 1;
    iters = reps * count;

    /* make sure the memory is allocated */
    memset(region, 0, nbytes);
    memset(region, 1, nbytes);
    allocate_memory(region, nbytes);
    memset(region, 0, nbytes);
    /* warm up the cache */
    (void) store_bash_loop(region, count, stride, 1L);

    /* run the bash loop */
    start_timing();

    (void) store_bash_loop(region, count, stride, reps);

    time = end_timing();

    return (time/(iters*1e3));
}

/* Your virtual memory pagesize must be at least this big */
#define MIN_PAGESIZE	256

void
allocate_memory(
		char *region,	/* memory region to be bashed       */
		long nbytes)
{			/* size of region (bytes)	    */
long            i;

    for (i = 0; i < nbytes; i += MIN_PAGESIZE)
	*((int *) (region + i)) = 0;
}


int
bash_loop(
	  char *region,	/* memory region to be bashed       */
	  long count,	/* number of locations to bash      */
	  long stride,	/* stride between locations (bytes) */
	  long reps	/* number of passes through region  */
)
{
long            i;
int             rslt;
int             *tmp;

    stride = stride/sizeof(int);

    rslt = 0;
    for (; reps > 0; reps--) {
	tmp = (int *)region;
	for (i = count; i > 0; i--) {
	    rslt ^= *((int *) tmp);
	    tmp += stride;
	}
    }

    return rslt;
}


int
store_bash_loop(
		char *region,	/* memory region to be bashed       */
		long count,	/* number of locations to bash      */
		long stride,	/* stride between locations (bytes) */
		long reps	/* number of passes through region  */
)
{
long            i;
int             rslt;
char           *tmp;

    rslt = 0;
    for (; reps > 0; reps--) {
	tmp = region;
	for (i = count; i > 0; i--) {
	    *((int *) tmp) = 0;
	    tmp += stride;
	}
    }

    return rslt;
}


void
usage(char *progname)
{
    fprintf(stderr, "usage: %s <machname> <iters> [<maxmem>]\n", progname);
    fprintf(stderr, "  <machname>   machine name\n");
    fprintf(stderr, "  <iters>      target # of accesses\n");
    fprintf(stderr, "  <maxmem>     maximum amount of mem to touch (def 16 Mb)\n");
    exit(1);
}

void
bw_driver(
	  ITYPE((*f) (char *, ITYPE, ITYPE)),
	  char *name,
	  char *region,
	  ITYPE mbytes,
	  ITYPE o_iter
)
{
long time;
double          mb_sec;
ITYPE           nbytes;
ITYPE           reps;
    printf("B %-6s", mach_name);
    printf("%-7s", name);
    fflush(stdout);
    for (nbytes = MINMEM; nbytes <= max_mem; nbytes += nbytes) {
	/* warm up the cache */
	memset(region, 0, nbytes);
	allocate_memory(region, nbytes);
	memset(region, 0, nbytes);

	if (nbytes >= (1024 * 1024)) {
	    reps = mbytes / (nbytes / (1024 * 1024));
	} else {
	    reps = mbytes * ((1024 * 1024) / nbytes);
	}
	if (reps <= 0)
	    reps = 1;

	/* run the bw loop */
        start_timing();
	(*f) (region, nbytes, reps);
        time = end_timing();

	if ((time) > 0)
	    mb_sec = 1e12 * (((double) mbytes * (double) o_iter) / (double) (time));
	else
	    mb_sec = 0.0;
	printf(" %4.0f", mb_sec);
	fflush(stdout);
    }
    printf("\n");
    fflush(stdout);
}


ITYPE
bw_int_ld(
	  char *region,	/* memory region to be bwed       */
	  ITYPE count,	/* number of locations to bw      */
	  ITYPE reps	/* number of passes through region  */
)
{
register ITYPE  i;
register ITYPE  rslt0 = 0;
register ITYPE  rslt1 = 0;
register ITYPE  rslt2 = 0;
register ITYPE  rslt3 = 0;
register ITYPE  tmp0;
register ITYPE  tmp1;
register ITYPE  tmp2;
register ITYPE  tmp3;
register ITYPE  tmp4;
register ITYPE  tmp5;
register ITYPE  tmp6;
register ITYPE  tmp7;
register ITYPE *tmp;
    count = (ITYPE) (((unsigned long) count) >> 5);
    for (; reps; reps--) {
	tmp = (ITYPE *) region;
	tmp0 = tmp[0];
	tmp1 = tmp[1];
	tmp2 = tmp[2];
	tmp3 = tmp[3];
	tmp4 = tmp[4];
	tmp5 = tmp[5];
	tmp6 = tmp[6];
	tmp7 = tmp[7];
	tmp += 8;
	for (i = count - 1; i; i--) {
	    rslt0 ^= tmp0;
	    tmp0 = tmp[0];
	    rslt1 ^= tmp1;
	    tmp1 = tmp[1];
	    rslt2 ^= tmp2;
	    tmp2 = tmp[2];
	    rslt3 ^= tmp3;
	    tmp3 = tmp[3];
	    rslt0 ^= tmp4;
	    tmp4 = tmp[4];
	    rslt1 ^= tmp5;
	    tmp5 = tmp[5];
	    rslt2 ^= tmp6;
	    tmp6 = tmp[6];
	    rslt3 ^= tmp7;
	    tmp7 = tmp[7];
	    tmp = (ITYPE *) (((char *) tmp)  + 32);
	}
    }
    if (rslt0 > 0)
	return rslt0^rslt1^rslt2^rslt3^tmp0^tmp1^tmp2^tmp3^tmp4^tmp5^tmp6^tmp7;
    else
	return 0;
}


#ifndef NOLIBS
ITYPE
bw_memset(
	  char *region,	/* memory region to be bwed       */
	  ITYPE count,	/* number of locations to bw      */
	  ITYPE reps	/* number of passes through region  */
)
{
    for (; reps; reps--) {
	memset(region, 0, count);
    }
    return 0;
}


ITYPE
bw_memchr(
	  char *region,	/* memory region to be bwed       */
	  ITYPE count,	/* number of locations to bw      */
	  ITYPE reps	/* number of passes through region  */
)
{
    for (; reps; reps--) {
	(void) memchr(region, 'a', count);
    }
    return 0;
}


ITYPE
bw_memcmp(
	  char *region,	/* memory region to be bwed       */
	  ITYPE count,	/* number of locations to bw      */
	  ITYPE reps	/* number of passes through region  */
)
{
ITYPE           iters = (ITYPE) (((unsigned long) count) >> 11);
ITYPE           stride = 1 << 11;
ITYPE           i_from;
ITYPE           i_to;
ITYPE           i;
    for (; reps; reps--) {
	i_from = 0;
	i_to = (iters - 1) << 11;
	for (i = 0; i < iters; ++i) {
	    memcmp(&region[i_from], &region[i_to], stride);
	    i_from += stride;
	    i_to -= stride;
	}
    }
    return 0;
}


ITYPE
bw_memcpy(
	  char *region,	/* memory region to be bwed       */
	  ITYPE count,	/* number of locations to bw      */
	  ITYPE reps)
{			/* number of passes through region  */
ITYPE           iters = (ITYPE) (((unsigned long) count) >> 11);
ITYPE           stride = 1 << 11;
ITYPE           i_from;
ITYPE           i_to;
ITYPE           i;
    for (; reps; reps--) {
	i_from = 0;
	i_to = (iters - 1) << 11;
	for (i = 0; i < iters; ++i) {
	    memcpy(&region[i_from], &region[i_to], stride);
	    i_from += stride;
	    i_to -= stride;
	}
    }
    return 0;
}
#endif


ITYPE
bw_int_store(
	     char *region,	/* memory region to be bwed       */
	     register ITYPE count,	/* number of locations to bw      */
	     register ITYPE reps	/* number of passes through region  */
)
{
register ITYPE  i;
register ITYPE  tmp0 = 0;
register ITYPE  tmp1 = 1;
register ITYPE  tmp2 = 2;
register ITYPE  tmp3 = 3;
register ITYPE  tmp4 = 4;
register ITYPE  tmp5 = 5;
register ITYPE  tmp6 = 6;
register ITYPE  tmp7 = 7;
register ITYPE *tmp;
    count = (ITYPE) (((unsigned long) count) >> 5);
    for (; reps; reps--) {
	tmp0++;
	tmp1++;
	tmp2++;
	tmp3++;
	tmp4++;
	tmp5++;
	tmp6++;
	tmp7++;
	tmp = (ITYPE *) region;
	for (i = count; i; i--) {
	    tmp[0] = tmp0;
	    tmp[1] = tmp1;
	    tmp[2] = tmp2;
	    tmp[3] = tmp3;
	    tmp[4] = tmp4;
	    tmp[5] = tmp5;
	    tmp[6] = tmp6;
	    tmp[7] = tmp7;
	    tmp = (ITYPE *) (((char *) tmp) + 32);
	}
    }
    return 0;
}


ITYPE
bw_int_copyi(
	     char *region,	/* memory region to be bwed       */
	     register ITYPE count,	/* number of locations to bw      */
	     register ITYPE reps	/* number of passes through region  */
)
{
register ITYPE  i;
register ITYPE  tmp0;
register ITYPE  tmp1;
register ITYPE  tmp2;
register ITYPE  tmp3;
register ITYPE  tmp4;
register ITYPE  tmp5;
register ITYPE  tmp6;
register ITYPE  tmp7;
register ITYPE *tmp;
    count = (ITYPE) (((unsigned long) count) >> 5);
    for (; reps; reps--) {
	tmp = (ITYPE *) region;
	tmp0 = tmp[0];
	tmp1 = tmp[1];
	tmp2 = tmp[2];
	tmp3 = tmp[3];
	tmp4 = tmp[4];
	tmp5 = tmp[5];
	tmp6 = tmp[6];
	tmp7 = tmp[7];
	for (i = count - 1; i; i--) {
	    tmp[0] = tmp1;
	    tmp[1] = tmp0;
	    tmp[2] = tmp3;
	    tmp[3] = tmp2;
	    tmp[4] = tmp5;
	    tmp[5] = tmp4;
	    tmp[6] = tmp7;
	    tmp[7] = tmp6;
	    tmp = (ITYPE *) (((char *) tmp) + 32);
	    tmp0 = tmp[0];
	    tmp1 = tmp[1];
	    tmp2 = tmp[2];
	    tmp3 = tmp[3];
	    tmp4 = tmp[4];
	    tmp5 = tmp[5];
	    tmp6 = tmp[6];
	    tmp7 = tmp[7];
	}
	tmp[0] = tmp1;
	tmp[1] = tmp0;
	tmp[2] = tmp3;
	tmp[3] = tmp2;
	tmp[4] = tmp5;
	tmp[5] = tmp4;
	tmp[6] = tmp7;
	tmp[7] = tmp6;
    }
    return 0;
}


ITYPE
bw_double_ld(
	     char *region,	/* memory region to be bwed       */
	     register ITYPE count,	/* number of locations to bw      */
	     register ITYPE reps	/* number of passes through region  */
)
{
register ITYPE  i;
register double rslt0 = 0.0;
register double rslt1 = 0.0;
register double rslt2 = 0.0;
register double rslt3 = 0.0;
register double rslt4 = 0.0;
register double rslt5 = 0.0;
register double rslt6 = 0.0;
register double rslt7 = 0.0;
register double tmp0;
register double tmp1;
register double tmp2;
register double tmp3;
register double tmp4;
register double tmp5;
register double tmp6;
register double tmp7;
register double *tmp;
    count = (ITYPE) (((unsigned long) count) >> 6);
    for (; reps; reps--) {
	tmp = (double *) region;
	tmp0 = tmp[0];
	tmp1 = tmp[1];
	tmp2 = tmp[2];
	tmp3 = tmp[3];
	tmp4 = tmp[4];
	tmp5 = tmp[5];
	tmp6 = tmp[6];
	tmp7 = tmp[7];
	tmp += 8;
	for (i = count - 1; i; i--) {
	    rslt0 *= tmp0;
	    tmp0 = tmp[0];
	    rslt1 *= tmp1;
	    tmp1 = tmp[1];
	    rslt2 *= tmp2;
	    tmp2 = tmp[2];
	    rslt3 *= tmp3;
	    tmp3 = tmp[3];
	    rslt4 *= tmp4;
	    tmp4 = tmp[4];
	    rslt5 *= tmp5;
	    tmp5 = tmp[5];
	    rslt6 *= tmp6;
	    tmp6 = tmp[6];
	    rslt7 *= tmp7;
	    tmp7 = tmp[7];
	    tmp += 8;
	}
    }
    if (rslt0 > 0) {
	rslt0 += tmp0;
	rslt1 += tmp1;
	rslt2 += tmp2;
	rslt3 += tmp3;
	rslt4 += tmp4;
	rslt5 += tmp5;
	rslt6 += tmp6;
	rslt7 += tmp7;
	rslt0 += rslt1;
	rslt2 += rslt3;
	rslt4 += rslt5;
	rslt6 += rslt7;
	rslt0 += rslt2;
	rslt4 += rslt6;
	rslt0 += rslt4;
	return rslt0;
    }
    return 0.0;
}

ITYPE
bw_double_store(
		char *region,	/* memory region to be bwed       */
		register ITYPE count,	/* number of locations to bw      */
		register ITYPE reps	/* number of passes through region  */
)
{
register ITYPE  i;
register double rslt0 = 0.0;
register double rslt1 = 0.0;
register double rslt2 = 0.0;
register double rslt3 = 0.0;
register double rslt4 = 0.0;
register double rslt5 = 0.0;
register double rslt6 = 0.0;
register double rslt7 = 0.0;
register double *tmp;
    count = (ITYPE) (((unsigned long) count) >> 6);
    for (; reps; reps--) {
	tmp = (double *) region;
	for (i = count; i; i--) {
	    tmp[0] = rslt0;
	    tmp[1] = rslt1;
	    tmp[2] = rslt2;
	    tmp[3] = rslt3;
	    tmp[4] = rslt4;
	    tmp[5] = rslt5;
	    tmp[6] = rslt6;
	    tmp[7] = rslt7;
	    tmp += 8;
	}
    }
    return 0;
}

ITYPE
bw_double_copyi(
		char *region,	/* memory region to be bwed       */
		register ITYPE count,	/* number of locations to bw      */
		register ITYPE reps	/* number of passes through region  */
)
{
register ITYPE  i;
register double tmp0;
register double tmp1;
register double tmp2;
register double tmp3;
register double tmp4;
register double tmp5;
register double tmp6;
register double tmp7;
register double *tmp;
    count = (ITYPE) (((unsigned long) count) >> 6);
    for (; reps; reps--) {
	tmp = (double *) region;
	tmp0 = tmp[0];
	tmp1 = tmp[1];
	tmp2 = tmp[2];
	tmp3 = tmp[3];
	tmp4 = tmp[4];
	tmp5 = tmp[5];
	tmp6 = tmp[6];
	tmp7 = tmp[7];
	for (i = count - 1; i; i--) {
	    tmp[0] = tmp1;
	    tmp[1] = tmp0;
	    tmp[2] = tmp3;
	    tmp[3] = tmp2;
	    tmp[4] = tmp5;
	    tmp[5] = tmp4;
	    tmp[6] = tmp7;
	    tmp[7] = tmp6;
	    tmp += 8;
	    tmp0 = tmp[0];
	    tmp1 = tmp[1];
	    tmp2 = tmp[2];
	    tmp3 = tmp[3];
	    tmp4 = tmp[4];
	    tmp5 = tmp[5];
	    tmp6 = tmp[6];
	    tmp7 = tmp[7];
	}
	tmp[0] = tmp1;
	tmp[1] = tmp0;
	tmp[2] = tmp3;
	tmp[3] = tmp2;
	tmp[4] = tmp5;
	tmp[5] = tmp4;
	tmp[6] = tmp7;
	tmp[7] = tmp6;
    }
    return 0;
}


ITYPE
bw_double_copy(
	       char *region,	/* memory region to be bwed       */
	       register ITYPE count,	/* number of locations to bw      */
	       register ITYPE reps	/* number of passes through region  */
)
{
register ITYPE  i;
register double tmp0;
register double tmp1;
register double tmp2;
register double tmp3;
register double tmp4;
register double tmp5;
register double tmp6;
register double tmp7;
register double *front;
register double *back;
    count = (ITYPE) (((unsigned long) count) >> 6);
    for (; reps; reps--) {
	front = (double *) region;
	back = (double *) (region + (count << 6) - sizeof(double));
	tmp0 = front[0];
	tmp1 = front[1];
	tmp2 = front[2];
	tmp3 = front[3];
	tmp4 = front[4];
	tmp5 = front[5];
	tmp6 = front[6];
	tmp7 = front[7];
	for (i = count - 1; i; i--) {
	    back[0] = tmp1;
	    back[-1] = tmp0;
	    back[-2] = tmp3;
	    back[-3] = tmp2;
	    back[-4] = tmp5;
	    back[-5] = tmp4;
	    back[-6] = tmp7;
	    back[-7] = tmp6;
	    front += 8;
	    back -= 8;
	    tmp0 = front[0];
	    tmp1 = front[1];
	    tmp2 = front[2];
	    tmp3 = front[3];
	    tmp4 = front[4];
	    tmp5 = front[5];
	    tmp6 = front[6];
	    tmp7 = front[7];
	}
	back[0] = tmp1;
	back[-1] = tmp0;
	back[-2] = tmp3;
	back[-3] = tmp2;
	back[-4] = tmp5;
	back[-5] = tmp4;
	back[-6] = tmp7;
	back[-7] = tmp6;
    }
    return 0;
}


ITYPE
bw_int_copy(
	    char *region,	/* memory region to be bwed       */
	    register ITYPE count,	/* number of locations to bw      */
	    register ITYPE reps	/* number of passes through region  */
)
{
register ITYPE  i;
register ITYPE  tmp0;
register ITYPE  tmp1;
register ITYPE  tmp2;
register ITYPE  tmp3;
register ITYPE  tmp4;
register ITYPE  tmp5;
register ITYPE  tmp6;
register ITYPE  tmp7;
register ITYPE *front;
register ITYPE *back;
    count = (ITYPE) (((unsigned long) count) >> 5);
    for (; reps; reps--) {
	front = (ITYPE *) region;
	back = (ITYPE *) (region + (count << 5) - sizeof(ITYPE));
	tmp0 = front[0];
	tmp1 = front[1];
	tmp2 = front[2];
	tmp3 = front[3];
	tmp4 = front[4];
	tmp5 = front[5];
	tmp6 = front[6];
	tmp7 = front[7];
	for (i = count - 1; i; i--) {
	    back[0] = tmp1;
	    back[-1] = tmp0;
	    back[-2] = tmp3;
	    back[-3] = tmp2;
	    back[-4] = tmp5;
	    back[-5] = tmp4;
	    back[-6] = tmp7;
	    back[-7] = tmp6;
	    front = (ITYPE *) (((char *) front) + 32);
	    back = (ITYPE *) (((char *) back) - 32);
	    tmp0 = front[0];
	    tmp1 = front[1];
	    tmp2 = front[2];
	    tmp3 = front[3];
	    tmp4 = front[4];
	    tmp5 = front[5];
	    tmp6 = front[6];
	    tmp7 = front[7];
	}
	back[0] = tmp1;
	back[-1] = tmp0;
	back[-2] = tmp3;
	back[-3] = tmp2;
	back[-4] = tmp5;
	back[-5] = tmp4;
	back[-6] = tmp7;
	back[-7] = tmp6;
    }
    return 0;
}
