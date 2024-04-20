/*
 * file:	diag_support_modular.c       
 *
 * Copyright (C) 1995 by
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
 *
 *
 * Abstract:	Routines that are used by diagnostics for program control,
 *		and standard message reporting.
 *
 * Author:	Console Firmware Team
 *
 *		tna  	11-Nov-1998	Conditionalized for YUKONA 
 *
 */

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$src:kernel_def.h"
#include "cp$src:prdef.h"
#include  "cp$src:ctype.h"
#include "cp$src:diag_def.h"
#include "cp$src:diag_groups.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:ev_def.h"
#include "cp$src:msg_def.h"
#include "cp$src:parse_def.h"
       
#include "cp$src:time.h"
#include "cp$src:version.h"

#include "cp$src:medulla_errors.h"

#include "cp$inc:kernel_entry.h"

#include "cp$inc:prototypes.h"

/* List of all the diagnostic groups -- Add the group name/mask here */

DIAG_GROUP diag_groups[] = {
    {diag_grp_fld, m_grp_fld}, 
    {diag_grp_mfg, m_grp_mfg}, 
    {diag_grp_exc, m_grp_exc}, 
    {diag_grp_dvt, m_grp_dvt}, 
    {diag_grp_pwr, m_grp_pwr}, 
    {diag_grp_boot, m_grp_boo}, 
    {NULL, 0}
};

/* need to undefine mask so it can be used and not translated as a macro */

#undef mask

/* Defines */

#define DEBUGP 0
#define MAX_OPT_STRG 128
#define VMS_BIAS 1858

static const char *months [] = {" ",
    "JAN","FEB","MAR","APR","MAY","JUN","JUL","AUG","SEP","OCT","NOV","DEC"
};                                         

/* Function prototypes */

int diag_init (int * argc, char *argv[], struct QSTRUCT * quals, 
		char * new_opt_string, int qscan_done);
void diag_print_end_of_pass(void);
void diag_print_startup(void);
#if VALIDATE
void diag_print_status(void);
#endif
void diag_print_test_trace(void);
void diag_start(void);

struct IOB *find_pcb_iob(void);
void diag_rundown(void);

int report_error(void);               
void report_error_lock(void);
int report_error_release(void);
int report_failure (u_int type,u_int number,u_int message,u_int expect,
		u_int rcvd,u_int *address);
void log_failure (u_int type,u_int number,u_int message,u_int expect,
		u_int rcvd,u_int *address);
int report_failure_quad (u_int type,u_int number,u_int message,u__int64 expect,
		u__int64 rcvd,u__int64 address);
static void log_failure_quad (u_int type,u_int number,u_int message,
		u__int64 expect,u__int64 rcvd,u__int64 address);
static void diag_print_ext_error();

static int se_thresh;			/* soft error threshhold */

extern int cpip;
extern int diag$r_evs;

extern int total_error_count;

extern struct LOCK spl_kernel;
extern struct LOCK spl_iobq;
extern struct QUEUE pcbq;

extern unsigned int dyn$_realloc();
extern int dyn$_malloc();
extern int err_printf();

extern int check_kill();
extern int printf();
extern int printf_nolock();
extern int qscan();
extern int sprintf();
extern int strcmp_nocase();
extern int strncmp();
extern char *strncpy();
extern void create_iob();
extern void remq_iob();
extern int find_all_dev();

extern void null_procedure();

#if MODULAR || WILDFIRE
extern struct SEMAPHORE report_lock;
#endif

#if TURBO
extern int cpumem_error_occurred;	/* cpumem failure flag - set  when
					 * report_error called */
#endif

#if MEDULLA || CORTEX || YUKONA || YUKON
extern char *diag_name (int code);
extern char *error_code (int code);
#endif

/*+
 * ======================================================================
 * = diag_init - Initialize the diagnostic environment.                 =
 * ======================================================================
 *
 * OVERVIEW:
 *
 *	This routine initializes the diagnostic environment.  
 *
 *	First, the command line is interpreted and the appropriate fields
 *	of the PCB are updated.  First it is verified that only valid
 *	qualifiers were entered on the command line.  If all qualifiers are
 *	valid, their corresponding value is saved in the appropriate field
 *	of the PCB.  If all qualifiers are not used, then the PCB fields are
 *	set to either the value of the corresponding environment variable or
 *	a default value.
 *
 *	After the qualifiers are evaluated, the argv array is compressed to
 *	remove the qualifiers and their values, and the argc count is updated
 *	accordingly.  Therefore the argv array is left with only the name
 *	of the diagnostic routine in argv[0], and the parameters in argv[1]
 *	through argv[argc-1].
 *
 * FORM OF CALL:
 *
 *      diag_init (int *argc, char *argv[], struct QSTRUCT *quals, char *new_opt_string, int qscan_done)
 *
 * RETURNS:
 *
 *	msg_success -- complete successfully
 *	msg_d_unsup_dev -- unsupported device entered
 *	msg_d_inv_test -- invalid test number/range entered
 *	msg_d_inv_grp -- unknown (invalid) group entered
 *	msg_d_bad_group -- no tests found in group entered
 *	msg_d_inv_test_grp -- invalid test selection for group entered
 *	QSCAN errors -- msg_qscan_noqual, msg_qscan_conv, msg_qscan_allocerr,
 *			msg_qscan_misc (see QSCAN for more details)
 *
 * ARGUMENTS:
 *
 *	int * argc -- pointer to number of command line arguments
 *	char * argv[] -- array of pointers to arguments
 *	struct QSTRUCT * quals -- pointer to previously set up structure that
 *		QSCAN will use to load up qualifier presence/values
 *	char * new_opt_string -- additional qualifiers diagnostic accepts
 *	int qscan_done -- Has a call to qscan been performed on argc, argv
 *
 * IMPLICIT INPUTS:
 *
 *	- The pcb$a_sup_dev field of the PCB.
 *	- The pcb$a_dis_table field of the PCB.
 *	- The pcb$b_name field of the PCB.
 *	- The pcb$l_pid field of the PCB.
 *	- Previously malloc'd IOB.
 *
 * IMPLICIT OUTPUTS:
 *
 *	- The pcb$a_rundown field in the PCB is updated.
 *	- The quals QSTRUCT array is updated.
 *	- The pcb$a_qualp field of the PCB is loaded by QSCAN
 *	- The pcb$a_test_mask field of the PCB is malloc'd and updated.
 *	- The pcb$b_dev_name field of the PCB is updated.
 *	- The diag$b_fru[0] is initialized to NULL.
 *	- The diag$a_ext_error is initialized to NULL.
 *
 * SIDE EFFECTS:
 *
 *	- The diagnostic startup message is NO LONGER printed.
 *	- Caller's ARGC, ARGV array is modified after call
 *	  to QSCAN
 *
-*/
int diag_init (int * argc, char *argv[], struct QSTRUCT * quals, 
		char * new_opt_string, int qscan_done)
{
    struct PCB *pcb;
    struct FILE *fp = 0;
    struct INODE *ip;
    struct EVNODE ev, *evp;
    char *dev_name_p;
    diag_common *diag_ptr;

    unsigned int req_grp_mask = 0;
    unsigned int group_test_mask[MAX_TESTS];
    char total_opt_string[MAX_OPT_STRG];
    char array_last_test[MAX_TESTS];
    char array_begin_test[MAX_TESTS];
    char array_two_char[3];
    char *remain_string;
    int i, j;
    int max_test = 0;
    int string_index = 0;
    int test_index = 0;
    int group_test_index = 0;
    int last_test_num = 0;
    int begin_test_num = 0;
    int last_string = 0;
    int begin_string_index = 0;
    int last_string_index = 0;
    int found_char = 0;
    int found_test = 0;
    int found_dev = 0;
    int status = 0;
    int amount_alloc;

    pcb = getpcb();
    evp = &ev;

/* mad 31-aug-92 malloc space for the diagnostic common structure 
 *  All extended error fields of DIAG_COMMON need to be 0'ed out initially
 */
    if ((diag_ptr = (diag_common *) malloc(sizeof(diag_common))) == NULL) {
	err_printf(msg_insuff_mem, sizeof(diag_common));
	return msg_failure;
    }

    memset(diag_ptr,0,sizeof(diag_common));
    pcb->pcb$a_diag_common = (int *) diag_ptr;

    if (!qscan_done) {
	    /* 
	     * Form the entire qualifier string and store it in total_opt_sting 
	     */
	    strcpy(total_opt_string,QSCAN_QUALIF_STRING);
	    strcat(total_opt_string,new_opt_string);
	    /* 
	     * Pass the argv and argc variables passed by the user to QSCAN 
	     * to parse out the qualifiers
	     */
	    status = qscan(argc, argv, QSCAN_INTRO, total_opt_string, quals);

	    if (status != msg_success) {
	        err_printf (status); 
	        return status; 
	    }
    }
/*
 * get device name if present (if more than one parameter on command line,
 * device is present
 */

/* 
 * if sup_dev is defined, then device is mandatory 
 */

    if (*argc > 1) {
/*
 * check if the device is one of the supported devices
 * which means checking the sup_dev string for it.  
 * if need to check supported devices, only need to check 
 * the first two characters of the string to check
 * if it's a valid device -- if no need to check skip over
 * and just read in device
 */
		/* use last name in protocol tower as device name */
	    if ( (dev_name_p = strrchr( argv[1], '/')) != NULL) {
		strcpy( pcb->pcb$b_dev_name, ++dev_name_p);
	    }
	    else
		dev_name_p = argv[1];
		    /*
		     * expand command line device name to full inode name by 
		     * looking for an inode that matches the possibly
		     * abbreviated argv name
		     */
	        if (allocinode( dev_name_p, 0, &ip) == 1) {
		    INODE_UNLOCK (ip);
		    strcpy( pcb->pcb$b_dev_name, ip->name);
	        }
	    if (pcb->pcb$a_sup_dev != NULL)
	    {
		for (i = 0; i < 2; ++i) 
		    array_two_char[i] = argv[1][i];

		array_two_char[i] = '\0';

		if (strstr(pcb->pcb$a_sup_dev,((char *)array_two_char)) ==NULL)
		{
		    err_printf (msg_quals);
 		    return msg_quals; 
		}
#if MEDULLA || CORTEX || YUKONA || YUKON
		else
		{
		    if (pcb->pcb$b_dev_name[0] == '\0')
		    	strcpy(pcb->pcb$b_dev_name, dev_name_p);
		}
#endif

            }

/* 
 * string compare section
 */

    } /* end if */
    else
	pcb->pcb$b_dev_name[0] = '\0';

/*
 * get pass count 
 */

    if (!quals[QP].present) {
	quals[QP].present = 1;
	ev_read("d_passes", &evp, 0);
	quals[QP].value.integer = (unsigned long int) evp->value.integer;
    }

/*
 * get maximum number of tests
 */
    if (quals[QT].present) {
	if (pcb->pcb$a_dis_table[0].routine != NULL) {
	    for (max_test = 0; pcb->pcb$a_dis_table[max_test].routine != NULL;
	      ++max_test)
		;
	} else
	    max_test = MAX_TESTS;

/*
 * figure out test numbers entered as parameters and build a test mask
 */

	string_index = 0;

/* Instead of malloc'ing 32 bytes at a time -- the maximum number of tests has
   already been figured out so I can just malloc that size and not have to
   worry about reallocating later */

	if (pcb->pcb$a_test_mask == NULL)
	    if ((pcb->pcb$a_test_mask =
	      (unsigned char *) malloc(DYN$K_GRAN)) == NULL) {
		err_printf(msg_insuff_mem, DYN$K_GRAN);
		return msg_failure;
	    }

	amount_alloc = DYN$K_GRAN;

	found_char = 0;
	while (!last_string) {

/* 
 * Extract the tests one by one from the test string
 */
	    while (quals[QT].value.string[string_index] != ',' &&
	      quals[QT].value.string[string_index] != ' ' &&
	      quals[QT].value.string[string_index] != '\0') {

		if (found_char) {
		    array_last_test[last_string_index] =
		      quals[QT].value.string[string_index];
		    ++last_string_index;
		} else {
		    array_begin_test[begin_string_index] =
		      quals[QT].value.string[string_index];
		    ++begin_string_index;
		}

		++string_index;

		if (quals[QT].value.string[string_index] == ':') {
		    found_char = 1;
		    ++string_index;
		}

	    }				/* end while */

/* 
 * Be sure to terminate the array with the NULL character
 */
	    array_last_test[last_string_index] = '\0';
	    array_begin_test[begin_string_index] = '\0';

/* 
 * if no space was found set flag to fall out of loop next time
 */
	    if (quals[QT].value.string[string_index] == '\0')
		last_string = 1;

/*
 * Set char pointers equal to arrays and convert begin test string to interger
 */
	    begin_test_num = atoi(array_begin_test);

/* 
 * make sure last test (or begin test if no test range) requested is less than
 * max tests and then add it to test mask -- increment past checked test and 
 * ',' for next test
 */
	    if (found_char) {
		last_test_num = atoi(array_last_test);

		if (last_test_num <= max_test && begin_test_num <= max_test &&
		  last_test_num > 0 && begin_test_num > 0) {

		    if (last_test_num > begin_test_num) {
			for (i = begin_test_num; i <= last_test_num; ++i) {

			    /* If string_index is larger than amount_alloc,
			     * need to allocate more memory for test_mask */
			    if (test_index >=
			      (amount_alloc / sizeof(char *))) {
				amount_alloc += DYN$K_GRAN;
				pcb->pcb$a_test_mask =
				  (unsigned char *) realloc(pcb->
				  pcb$a_test_mask, amount_alloc);
			    }

			    pcb->pcb$a_test_mask[test_index++] = i;
			}
		    } else {
			for (i = begin_test_num; i >= last_test_num; --i) {

			    /* If string_index is larger than amount_alloc,
			     * need to allocate more memory for test_mask */
			    if (test_index >=
			      (amount_alloc / sizeof(char *))) {
				amount_alloc += DYN$K_GRAN;
				pcb->pcb$a_test_mask =
				  (unsigned char *) realloc(pcb->
				  pcb$a_test_mask, amount_alloc);
			    }

			    pcb->pcb$a_test_mask[test_index++] = i;
			}
		    }

		} else {
		    err_printf(msg_d_inv_test);
		    return msg_d_inv_test;
		}

	    }

	      else {
		if ((begin_test_num <= max_test) && (begin_test_num > 0)) {

		    /* If string_index is larger than amount_alloc, need to
		     * allocate more memory for test_mask */
		    if (test_index >= (amount_alloc / sizeof(char *))) {
			amount_alloc += DYN$K_GRAN;
			pcb->pcb$a_test_mask = (unsigned char *) realloc(pcb->
			  pcb$a_test_mask, amount_alloc);
		    }

		    pcb->pcb$a_test_mask[test_index++] = begin_test_num;
		} else {
		    err_printf(msg_d_inv_test);
		    return msg_d_inv_test;
		}
	    }

	    ++string_index;
	    found_char = 0;
	    begin_string_index = 0;
	    last_string_index = 0;

	}				/* end while */

	/* Signify last test */
	pcb->pcb$a_test_mask[test_index] = 0;

    }					/* end if */

/* 
 * check if group name was entered on the command line, if not
 * set load environment variable or default into quals[QG]
 */
    if (!quals[QG].present) {
/*
 * if no group name on command line, malloc some memory for the quals[QG]
 * and then check if environment variable set
 */
	if ((quals[QG].value.string = malloc(MAX_NAME)) == NULL) {
	    err_printf(msg_insuff_mem, MAX_NAME);
	    return msg_failure;
	}
	ev_read("d_group", &evp, 0);
	if (evp->value.string != NULL)
	    strncpy(quals[QG].value.string, evp->value.string, MAX_NAME);
/*
 * if no environment variable or group qualifier, default to field
 */
	else
	    strncpy(quals[QG].value.string, diag_grp_fld, MAX_NAME);

    }					/* endif */

/*
 * check if the group is one of the supported groups
 * which means checking the diag_groups for it.  
 */

/*
 * string compare section
 */
    for (i = 0; diag_groups[i].grp_name != NULL; ++i) {
/*
 * check that group requested is in supported groups
 */
	if ((strcmp(diag_groups[i].grp_name,
	  ((char *) quals[QG].value.string))) == 0) {
	    req_grp_mask = diag_groups[i].grp_mask;
	    break;
	}

    }					/* end for */

/*
 * if no group found, make error callout
 */
    if (req_grp_mask == 0) {
	err_printf(msg_d_inv_grp);
	return msg_d_inv_grp;
    }

/* 
 * initialize group_test_mask to 0 for later check
 */
    group_test_mask[0] = 0;

    for (i = 0; pcb->pcb$a_dis_table[i].routine != NULL; i++) {
	if (pcb->pcb$a_dis_table[i].groups & req_grp_mask)
	    group_test_mask[group_test_index++] = (i + 1);
    }

/*
 * if no tests were found in that group and dispatch table's test
 * mask != NULL (in case someone doesn't call diag_start and load
 * up a meaningful dispatch table...Paul!)  -- make error callout
 * only make error callout if a -t was not entered on command line
 */

    if (group_test_mask[0] == 0 && pcb->pcb$a_dis_table[0].groups != NULL &&
      quals[QT].present != 1) {
	err_printf(msg_d_bad_group, quals[QG].value.string);
	return msg_d_bad_group;
    }

/* 
 * If -t was entered along with -g check that test mask is within group test 
 * list.  If not within group test list, make error callout.  If -t was
 * entered without -g, run all tests specified regardless of value of D_GROUP
 * environment variable.
 */

    if (quals[QT].present && quals[QG].present) {
	for (i = 0; i < test_index; ++i) {
	    for (j = 0; j < group_test_index; ++j) {

		if (group_test_mask[j] ==
		  (unsigned int) pcb->pcb$a_test_mask[i]) {
		    found_test = 1;
		    break;
		}			/* end if */

	    }				/* end for */

	    if (!found_test) {
		err_printf(msg_d_inv_test_grp, quals[QG].value.string);
		return msg_d_inv_test_grp;
	    }				/* end if */

	    found_test = 0;

	}				/* end for */

    }					/* end if */

/* 
 * If no -t qualifier present, build test mask from group input -- regardless
 * if group input is from -g or D_GROUP, however -g does have precedence over
 * D_GROUP as calculated above
 */
      else if (!quals[QT].present) {

	if (pcb->pcb$a_test_mask == NULL)
	    if ((pcb->pcb$a_test_mask = (unsigned char *) malloc(
	      (group_test_index + 1) * (sizeof(unsigned int)))) == NULL) {
		err_printf(msg_insuff_mem,
		  (group_test_index + 1) * (sizeof(unsigned int)));
		return msg_failure;
	    }

	for (i = 0; i < group_test_index; ++i)
	    pcb->pcb$a_test_mask[i] = group_test_mask[i];

	/* Signify last test */
	pcb->pcb$a_test_mask[i] = 0;

    }					/* end else */

    if (quals[QSE].present) {
	se_thresh = quals[QSE].value.integer;
    }

/* Initialize the FRU field in the PCB in case report_error is called and
 * the coder forgets (or has no need) to load up this field.
 */
    diag_ptr->diag$b_fru[0] = '\0';

/*                                         
 * Initialize the ext_error routine to NULL in case report_summary is set
 * to full and diagnostic does not load any type of ext_error routine --
 */
    diag_ptr->diag$a_ext_error = NULL;
    return msg_success;
}

/*+
 * ======================================================================
 * = diag_print_end_of_pass - Print diagnostic end-of-pass message.      =
 * ======================================================================
 *
 * OVERVIEW:
 *
 *	This routine prints the diagnostic end-of-pass message.  This routine
 *	is automatically called at the end of each pass of the diagnostic by
 *	diag_start(), and should therefore not be called if diag_start() is
 *	used.
 *
 * FORM OF CALL:
 *
 *	diag_print_end_of_pass();
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	None.
 *
 * IMPLICIT INPUTS:
 *
 *	- The pcb$b_name field of the PCB.
 *	- The pcb$l_pid field of the PCB.
 *	- The diag$l_pass_count field of the DIAG_COMMON structure.
 *	- The pcb$b_dev_name field of the PCB.
 *	- The hard_errors field of the IOB.
 *	- The soft_errors field of the IOB.
 *
 * IMPLICIT OUTPUTS:
 *
 *	None.
 *
 * SIDE EFFECTS:
 *
 *	None.
-*/

void diag_print_end_of_pass(void) 
{
    char			time[32];
    struct toy_date_time toy;	/* Date structure */
    struct PCB *pcb;
    diag_common *diag;
    int		pass;
    struct IOB *iob_ptr;

    pcb = getpcb();
    iob_ptr = find_pcb_iob();
    diag = (diag_common *)pcb->pcb$a_diag_common;

    if (iob_ptr == NULL) {
	pcb->pcb$l_killpending = 1;
#if VALIDATE
	return;
#else
	check_kill();
#endif
    }


	/* 
	 * If D_EOP environment variable is ON, print end of pass message
	 */
    if (!(diag$r_evs & m_eop))
	return;

    get_date_time (&toy);

    /*
    /* Format the date and time strings...
    */
    sprintf (time, "%2d:%02d:%02d",
            toy.hours & 0x7f, /* mask off am/pm indicator */
            toy.minutes,
            toy.seconds);

	    /* Check for valid diag pointer, stuff with dummy info if so */
    if (diag == 0)
	pass = 0;
    else
	pass = diag->diag$l_pass_count;

    printf(msg_d_end_of_pass, 
    		pcb->pcb$b_name, 
    		pcb->pcb$l_pid,	 
    		pcb->pcb$b_dev_name, 
    		pass,
	       	iob_ptr->hard_errors, 
    		iob_ptr->soft_errors, 
    		time);
    return;
}

/*+
 * ======================================================================
 * = diag_print_startup - Print diagnostic startup message.      =
 * ======================================================================
 *
 * OVERVIEW:
 *
 *	This routine prints the diagnostic startup message.  This routine
 *	is automatically called at the beginning of the diagnostic by
 *	diag_start(), and should therefore not be called if diag_start() is
 *	used.
 *
 * FORM OF CALL:
 *
 *	diag_print_startup();
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	None.
 *
 * IMPLICIT INPUTS:
 *
 *	- The pcb$b_name field of the PCB.
 *	- The pcb$l_pid field of the PCB.
 *	- The pcb$b_dev_name field of the PCB.
 *	- The hard_errors field of the IOB.
 *	- The soft_errors field of the IOB.
 *
 * IMPLICIT OUTPUTS:
 *
 *	None.
 *
 * SIDE EFFECTS:
 *
 *	None.
-*/

void diag_print_startup(void) 
{
    char		 time[32],ddate[32];
    struct toy_date_time toy;	/* Date structure */
    struct PCB *pcb;
    struct IOB *iob_ptr;

    pcb = getpcb();
    iob_ptr = find_pcb_iob();

    if (iob_ptr == NULL) {
	 pcb->pcb$l_killpending = 1;
#if VALIDATE
	 return;
#else
	 check_kill();
#endif
    }

	/* 
	 * If D_START environment variable is ON, print startup message
	 */
    if (!(diag$r_evs & m_start))
	return;

    get_date_time (&toy);

    /*
    /* Format the date and time strings...
    */
    sprintf (ddate, "%2d-%s-%04d",
	   toy.day_of_month,
           months [toy.month % ((sizeof months) /  sizeof (months [0]))],
           genyear (toy.year));

    sprintf (time, "%2d:%02d:%02d",
            toy.hours & 0x7f, /* mask off am/pm indicator */
            toy.minutes,
            toy.seconds);

    printf(msg_d_startup, 
    		msg_d_header, 
    		ddate, 
    		pcb->pcb$b_name, 
    		pcb->pcb$l_pid,
	        pcb->pcb$b_dev_name, 
    		time);
    return;
}


#if VALIDATE
/*+
 * ======================================================================
 * = diag_print_status - Print diagnostic status message.               =
 * ======================================================================
 *
 * OVERVIEW:
 *
 *	This routine prints the diagnostic status message.  This routine
 *	should be called within tests that have a long run-time to inform
 *	the user that the test is still running.
 *
 * FORM OF CALL:
 *
 *	diag_print_status();
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	None.
 *
 * IMPLICIT INPUTS:
 *
 *	- The pcb$b_name field of the PCB.
 *	- The pcb$l_pid field of the PCB.
 *	- The diag$l_pass_count field of the DIAG_COMMON structure.
 *	- The diag$l_test_num field of the DIAG_COMMON structure.
 *	- The hard_errors field of the IOB.
 *	- The soft_errors field of the IOB.
 *
 * IMPLICIT OUTPUTS:
 *
 *	None.
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/

void diag_print_status(void) 
{
    char			time[32], ddate[32];
    struct toy_date_time toy;	/* Date structure */
    struct PCB *pcb = getpcb();
    diag_common *diag = (diag_common *) pcb->pcb$a_diag_common;
    int		pass,test;
    struct IOB *iob_ptr;

    iob_ptr = find_pcb_iob();

    if (iob_ptr == NULL) {
	pcb->pcb$l_killpending = 1;
#if VALIDATE
	return;
#else
	check_kill();
#endif
    }

	/* 
	 * If d_stat environment variable is ON, print status message
	 */
    if (!(diag$r_evs & m_status))
	return;

    get_date_time (&toy);

    /*
    /* Format the date and time strings...
    */
    sprintf (time, "%2d:%02d:%02d",
            toy.hours & 0x7f, /* mask off am/pm indicator */
            toy.minutes,
            toy.seconds);

    sprintf (ddate, "%2d-%s-%04d",
	   toy.day_of_month,
           months [toy.month % ((sizeof months) /  sizeof (months [0]))],
           genyear (toy.year));

	    /* Check for valid diag pointer, stuff with dummy info if so */
    if (diag == 0)
    {
	pass = 0;
	test = 0;
    }
    else
    {
	pass = diag->diag$l_pass_count;
	test = diag->diag$l_test_num;
    }

    printf(msg_d_status, 
    		msg_d_header, 
    		ddate, 
    		pcb->pcb$b_name, 
    		pcb->pcb$l_pid,
		pcb->pcb$b_dev_name, 
    		pass, 
		test, 
    		iob_ptr->hard_errors, 
	        iob_ptr->soft_errors, 
    		time);
    return;
}
#endif


/*+
 * ======================================================================
 * = diag_print_test_trace - Print diagnostic test trace message.       =
 * ======================================================================
 *
 * OVERVIEW:
 *
 *	This routine prints the diagnostic test trace message.  This routine
 *	is automatically called at the beginning of each test of the diagnostic
 *	by diag_start(), and should therefore not be called if diag_start() is
 *	used.
 *
 * FORM OF CALL:
 *
 *	diag_print_test_trace();
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	None.
 *
 * IMPLICIT INPUTS:
 *
 *	- The pcb$b_name field of the PCB.
 *	- The pcb$l_pid field of the PCB.
 *	- The diag$l_test_num field of the DIAG_COMMON structure.
 *	- The diag$l_pass_count field of the DIAG_COMMON structure.
 *	- The hard_errors field of the IOB.
 *	- The soft_errors field of the IOB.
 *
 *
 * IMPLICIT OUTPUTS:
 *
 *	None.
 *
 * SIDE EFFECTS:
 *
 *	None.
-*/

void diag_print_test_trace(void) 
{
    char time[32];
    struct toy_date_time toy;	/* Date structure */
    struct PCB *pcb = getpcb();
    diag_common *diag = (diag_common *) pcb->pcb$a_diag_common;
    int		pass,test;
    struct IOB *iob_ptr;

    iob_ptr = find_pcb_iob();

    if (iob_ptr == NULL) {
	pcb->pcb$l_killpending = 1;
#if VALIDATE
	return;
#else
	check_kill();
#endif
    }

	/* If D_TRACE environment variable is ON, print test trace  */
    if (!(diag$r_evs & m_tt))
      { krn$_sleep(10);
	return;
      }

    get_date_time (&toy);

    /*
    /* Format the date and time strings...
    */
    sprintf (time, "%2d:%02d:%02d",
            toy.hours & 0x7f, /* mask off am/pm indicator */
            toy.minutes,
            toy.seconds);

	/* Check for valid diag pointer, stuff with dummy info if so */
    if (diag == 0)
    {
	pass = 0;
	test = 0;
    }
    else
    {
	pass = diag->diag$l_pass_count;
	test = diag->diag$l_test_num;
    }

    printf(msg_d_test_trace, 
    		pcb->pcb$b_name, 
    		pcb->pcb$l_pid,
    		pcb->pcb$b_dev_name, 
    		pass,
    		test,
    		iob_ptr->hard_errors, 
    		iob_ptr->soft_errors, 
    		time);
    return;
}


/*+
 * ======================================================================
 * = diag_start - Start the diagnostic.                                 =
 * ======================================================================
 *
 * OVERVIEW:
 *
 *	This routine controls the execution of the diagnostic tests.  Each
 *	test specified by the test mask field of the PCB will be serially
 *	executed for the requested number of passes.  Startup, test trace and
 *	end-of-pass messages will be automatically issued.  The diag$l_test_num
 *	and diag$l_pass_count fields of the PCB will be automatically updated.
 *	Loop-on-error control will be automatically done as determined by
 *	the message value returned from a test.
 *
 *	After each test, the routine check_kill() is called to check if
 *   	an abort has been requested through a ^C or kill command.  If an abort
 *	has been requested, then the diagnostic is terminated.  Therefore,
 *	if the diagnostic creates any sub-processes, it must make sure that
 *	these sub-processes are also killed in the event that the controlling
 *	process is killed, or that these sub-processes are killed off in the
 * 	rundown routine.
 *	
 *
 * FORM OF CALL:
 *
 *	diag_start();
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	None.
 *
 * IMPLICIT INPUTS:
 *
 *	- The pcb$a_dis_table field of the PCB.
 *	- The pcb$b_name field of the PCB.
 *	- The pcb$l_pid field of the PCB.
 *	- The diag$l_pass_count field of the DIAG_COMMON structure.
 *	- The diag$l_test_num field of the DIAG_COMMON structure.
 *	- The pcb$a_qualp field of the PCB.
 *	- The hard_errors field of the IOB.
 *	- The soft_errors field of the IOB.
 *
 *
 * IMPLICIT OUTPUTS:
 *
 *	- The diag$l_pass_count field of the DIAG_COMMON structure is updated 
 *	  before each pass.
 *	- The diag$l_test_num field of the DIAG_COMMON structure is updated 
 *	  before the test dispatch.
 *
 * SIDE EFFECTS:
 *
 *	- The diagnostic startup message is printed (diag_print_startup())
 *	  if D_STARTUP is set to ON.
 *      - The diagnostic test trace message is printed (diag_print_test_trace())
 *        before the test dispatch if D_TRACE is set to ON.
 *      - The diagnostic end-of-pass message is printed
 *        (diag_print_end_of_pass()) after each pass if D_EOP is set to ON.
 *      - The caller may not be returned to if a ^P was entered.
 *
-*/

void diag_start(void) 
{
    int loop = 0;
    int status = 0;
    int i, j;
    unsigned char *test_mask;
    struct PCB *pcb = getpcb();
    diag_common *diag = (diag_common *) pcb->pcb$a_diag_common;
    struct QSTRUCT *quals;
#if RAWHIDE
    int id = whoami();
    struct FILE *fp = 0;
    char	tmess[24];
#endif

/* First check for a valid dispatch table.				*/

    quals = (struct QSTRUCT *) pcb->pcb$a_qualp;
    if (pcb->pcb$a_dis_table == NULL) {
	err_printf(msg_nodis);
	pcb->pcb$l_killpending = 1;
#if VALIDATE
	return;
#else
	check_kill();
#endif
    }


/* 
 * Print out startup message before beginning the tests
 */
    diag_print_startup();

/* This is the main dispatch loop.  This outer loop will control the	*/
/* number of passes run based on the number requested.			*/

    while ((quals[QP].value.integer == 0) ||
      (diag->diag$l_pass_count < quals[QP].value.integer)) {
	diag->diag$l_pass_count++;
	test_mask = pcb->pcb$a_test_mask;

	/* The field for pass_count is only 8 digits (999999999) -- need to
	 * check if pass_count is greater than this and reset */
	if (diag->diag$l_pass_count > 99999999)
	    diag->diag$l_pass_count = 0;

/* This loop will run all of the tests specified in the test mask.  The	*/
/* completion of this loop represents the completion of one pass.	*/

	for (i = 0; test_mask[i] != 0; ++i) {
	    diag->diag$l_test_num = (unsigned int) test_mask[i];
	    diag_print_test_trace();
#if RAWHIDE
	    /* Trace messages in powerup on OCP. Check for test number less */
            /* than 99 - two digits. Don't need to check device name string */
            /* since will simply only display first five characters of dev. */
            /* Always print dev_name in uppercase since XSROM does the same. */

	    if (cpip)
	       if (diag->diag$l_test_num <= 99) {
		  sprintf(tmess, "P%d TEST %2d %s", id, diag->diag$l_test_num,
		     pcb->pcb$b_dev_name);
		  for (j=0; tmess[j]=toupper(tmess[j]); j++);
	     	  sable_ocp_write(fp, strlen(tmess), 1, tmess);
	       }
#endif
	    if (pcb->pcb$a_dis_table[(test_mask[i]) - 1].routine == 0)
		break;

	    status = pcb->pcb$a_dis_table[(test_mask[i]) - 1].routine();

	    if (status == msg_loop)
	       loop = 1;
	    else if (status == msg_halt) {
	       pcb->pcb$l_killpending = 1;
#if RAWHIDE
	       /* Print to OCP on halt-on-error failure if power-up mode */
	       if (cpip) {
	          if (diag->diag$l_test_num <= 99) {
		     sprintf(tmess, "P%d FAIL %2d %s", id, diag->diag$l_test_num,
		        pcb->pcb$b_dev_name);
		     for (j=0; tmess[j]=toupper(tmess[j]); j++);
		     sable_ocp_write(fp, strlen(tmess), 1, tmess);
		     krn$_sleep(5000);       /* freeze display for five seconds */
	          }
	       }
#endif
            }

/* This is the re-entry point to which the test will return.  Here	*/
/* check_kill() will be called to check if a ^C or kill command has	*/
/* been entered.  Also, loop-on-error control will be done.		*/

#if VALIDATE
	    if (pcb->pcb$l_killpending)
	       return;
#else
	    check_kill();
#endif
	    if (loop) {
	       diag->diag$l_pass_count++;
	       test_mask--;
	    }

	    /* Clear out this field between passes */
	    diag->diag$l_error_num = 0;

	    /* The field for pass_count is only 8 digits (999999999) -- need to
	     * check if pass_count is greater than this and reset */
	    if (diag->diag$l_pass_count > 99999999)
	       diag->diag$l_pass_count = 0;
	}
	diag_print_end_of_pass();
    }
    return;
}

/*+
 * ======================================================================
 * = diag_rundown - Perform diagnostic common rundown tasks.            =
 * ======================================================================
 *
 * OVERVIEW:
 *
 *	This routine should be called before a diagnostic process is 
 *	terminated to execute rundown code common to all diagnostics.  
 *	This routine will then call the diagnostic-specific cleanup code.  
 *	The diagnostic common rundown code consists of printing the 
 *	diagnostic completion message.
 *
 * FORM OF CALL:
 *
 *	diag_rundown();
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	None.
 *
 * IMPLICIT INPUTS:
 *
 *	- The pcb$b_name field of the PCB.
 *	- The pcb$l_pid field of the PCB.
 *	- The diag$l_pass_count field of the DIAG_COMMON structure.
 *	- The pcb$b_dev_name field of the PCB.
 *	- The hard_errors field of the IOB.
 *	- The soft_errors field of the IOB.
 *	- The pcb$r_evs field of the PCB.
 *
 * IMPLICIT OUTPUTS:
 *
 *	None.
 *
 * SIDE EFFECTS:
 *
 *	- The diagnostic completion message is printed.
-*/

void diag_rundown(void) 
{
    int msg,pass;
    char			time[32],ddate[32];
    struct toy_date_time toy;	/* Date structure */
    struct IOB *iob_ptr;
    struct PCB *pcb;
    diag_common *diag;

    pcb = getpcb();
    iob_ptr = find_pcb_iob();

	    /* If no IOB was found, set kill pending and return */
    if (iob_ptr == NULL) {
	pcb->pcb$l_killpending = 1;
	return;
    }

    /* If D_COMPLETE environment variable is set, print out completion msg */
    if (!(diag$r_evs & m_comp))
	return;


    if (iob_ptr->hard_errors || iob_ptr->soft_errors)
	msg = msg_d_fail;
    else
	msg = msg_d_pass;

    get_date_time (&toy);

    /*
    /* Format the date and time strings...
    */
    sprintf (time, "%2d:%02d:%02d",
            toy.hours & 0x7f, /* mask off am/pm indicator */
            toy.minutes,
            toy.seconds);

    sprintf (ddate, "%2d-%s-%04d",
	   toy.day_of_month,
           months [toy.month % ((sizeof months) /  sizeof (months [0]))],
           genyear (toy.year));

	/* Check for valid diag pointer, stuff with dummy info if so */
    diag = (diag_common *)pcb->pcb$a_diag_common;
    if (diag == 0)
	pass = 0;
    else
	pass = diag->diag$l_pass_count;

    printf(msg_d_completion, 
    		msg_d_header, 
    		ddate, 
		pcb->pcb$b_name, 
		pcb->pcb$l_pid,
		pcb->pcb$b_dev_name, 
		pass, 
		iob_ptr->hard_errors, 
		iob_ptr->soft_errors, 
		time, msg);
    return;
}


/*+
 * ======================================================================
 * = report_error - Report an error.                                    =
 * ======================================================================
 *
 * OVERVIEW:
 *
 *	This routine is used to report an error.  It does this by simply
 *	printing the error information provided in the PCB.
 *
 * FORM OF CALL:
 *
 *	report_error();
 *
 * RETURNS:
 *
 *	Status from report_error_release.
 *
 * ARGUMENTS:
 *
 *	None.
 *
 * IMPLICIT INPUTS:
 *
 *	- The diag$a_ext_error field in the PCB.
 *	- All implicit inputs of report_error_lock().
 *
 * IMPLICIT OUTPUTS:
 *
 *	None.
 *
 * SIDE EFFECTS:                                        
 *
 *	None.
 *
-*/

int report_error(void) 
{
    struct PCB *pcb = getpcb();
    diag_common *diag_ptr = (diag_common *) pcb->pcb$a_diag_common;
    int s = msg_success;
#if TURBO
    u_int	exp,rcvd,failadr,exp2=0,rcvd2=0,failadr2=0;

/* This global flag will be checked in light_testnum_leds() routine and
 *   in clear_gbus_leds_stp() routine
 */
    if (strstr(pcb->pcb$b_name, "cpu_mem") != NULL)
	cpumem_error_occurred = 1;
#endif

    report_error_lock();

#if RAWHIDE
    /* Don't want to print out extended error routine/info if powerup
       unless the mfg bit is set (for Rawhide) */
    if ((!cpip) || ((cpip & (rtc_read(0x22) == 0x22))))
#endif
    if ((diag_ptr->diag$a_ext_error != NULL) &&
      (diag$r_evs & m_rpt_full))
    {
	diag_ptr->diag$a_ext_error();
    }

/* mad 02-sep-92  ALWAYS log ALL extended error information regardles of 
 *                state of "d_report"!
 *                Also, need to check the state of the "disable_logging" LW.
 *                If this LW=1, do NOT log any error information to EEPROM.
 *		  After error logging has been completed, this routine should
 *                then 0 out all extended error reporting fields AND the
 *                "disable_logging" flag.
 */
#if TURBO
    exp = diag_ptr->diag$l_expect;
    rcvd = diag_ptr->diag$l_rcvd;
    failadr = diag_ptr->diag$l_failadr;
#ifndef MODE64
    exp2 = diag_ptr->diag$l_expect2;
    rcvd2 = diag_ptr->diag$l_rcvd2;
    failadr2 = diag_ptr->diag$l_failadr2;
#endif

    if (eep_log_enabled() && !diag_ptr->diag$l_disable_logging) {
	log_eep_ext(exp2, exp, rcvd2, rcvd, failadr2, failadr);
    }
#endif

    s = report_error_release();

    /* reset error information here */
    diag_ptr->diag$l_expect = 0;
    diag_ptr->diag$l_expect2 = 0;
    diag_ptr->diag$l_rcvd = 0;
    diag_ptr->diag$l_rcvd2 = 0;
    diag_ptr->diag$l_failadr = 0;
    diag_ptr->diag$l_failadr2 = 0;
    diag_ptr->diag$l_lmerr = 0;
    diag_ptr->diag$l_lber = 0;

    /* If we used our generic extended error printer then free it up. */
    if (diag_ptr->diag$a_ext_error == diag_print_ext_error)
	diag_ptr->diag$a_ext_error = NULL;

    return (s);
}


/*+
 * ======================================================================
 * = report_error_lock - Report an error with lock.                     =
 * ======================================================================
 *
 * OVERVIEW:
 *
 *	This routine is used to report an error.  It does this by simply
 *	printing the error information provided in the PCB.
 *
 *	This routine differs from report_error() in that the write_lock
 *	on the standard error channel is left taken out.  This is done to
 *	facilitate the printing of extended error information immediately
 *	after the call to report_error_lock(), as opposed to having the
 *	extended error reporting done in a routine.
 *
 *	After extended error reporting is complete, the report_error_release()
 *	routine must be called to complete the error report and unlock the
 *	standard error channel.
 *
 * FORM OF CALL:
 *
 *	report_error_lock();
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	None.
 *
 * IMPLICIT INPUTS:
 *
 *	- The pcb$a_stderr field in the PCB.
 *	- The pcb$b_name field in the PCB.
 *	- The pcb$l_pid field in the PCB.
 *	- The diag$l_error_type field in DIAG_COMMON.
 *	- The diag$l_error_msg field in DIAG_COMMON.
 *	- The diag$l_pass_count field in DIAG_COMMON.
 *	- The pcb$b_dev_name field in the PCB.
 *	- The hard_errors field in the IOB.
 *	- The soft_errors field in the IOB.
 *	- The diag$l_test_num field in DIAG_COMMON
 *	- The diag$l_error_num field in DIAG_COMMON.
 *
 * IMPLICIT OUTPUTS:
 *
 *	None.
 *
 * SIDE EFFECTS:
 *
 *	- The standard error channel is left locked.
 *
-*/

void report_error_lock(void) 
{
    struct PCB *pcb = getpcb();
    diag_common *diag_ptr = (diag_common *) pcb->pcb$a_diag_common;
    struct IOB *iob_ptr;
    int	msg,error_num,error_msg,test_num,pass;
    char *fru;
    char time [32], ddate [32];
    struct toy_date_time toy;

    iob_ptr = find_pcb_iob();

    if (iob_ptr == NULL) {
	pcb->pcb$l_killpending = 1;
#if VALIDATE
	return;
#else
	check_kill();
#endif
    }


/* need to take out kernel spinlock before updating fields in the IOB */
/* 
 * cto 30-jan-92
 *	use IOBQ lock!!! 
 */
    spinlock(&spl_iobq);

    total_error_count++;

    switch (diag_ptr->diag$l_error_type) {

	case DIAG$K_FATAL: 
	    msg = msg_d_fatal;

/* Increment hard error count so when error is reported both soft/hard error
   counts aren't 0 when fatal error occurs */
	    iob_ptr->hard_errors++;
	    break;

	case DIAG$K_HARD: 
	    iob_ptr->hard_errors++;
	    msg = msg_d_hard;
	    break;

	case DIAG$K_SOFT: 
	    iob_ptr->soft_errors++;

/* If -se qualifier present, check to see if soft_error count has exceded
   threshold */
	    if (iob_ptr->soft_errors >= se_thresh) {
		iob_ptr->hard_errors++;
		msg = msg_d_hard;
	    } else
		msg = msg_d_soft;
	    break;

	default: 
	    msg = msg_d_err;
	    break;

    }

/* Hard/Soft Error count field will only hold up to  4 digits (9999) so need 
   to reset to 0 after that -- check here */
    if (iob_ptr->hard_errors == 10000)
	iob_ptr->hard_errors = 0;
    if (iob_ptr->soft_errors == 10000)
	iob_ptr->soft_errors = 0;

/* Clear spinlock */
    spinunlock(&spl_iobq);

    get_date_time (&toy);

    /*
    /* Format the date and time strings...
    */
    sprintf (ddate, "%2d-%s-%04d",
	   toy.day_of_month,
           months [toy.month % ((sizeof months) /  sizeof (months [0]))],
           genyear (toy.year));

    sprintf (time, "%2d:%02d:%02d",
            toy.hours & 0x7f, /* mask off am/pm indicator */
            toy.minutes,
            toy.seconds);


    if (diag_ptr != NULL)
    {
/* Store timestamp into diag$l_scratch register so logging routines can 
   access it */
    	diag_ptr->diag$l_scratch = (toy.hours << 16) | (toy.minutes << 8) 
			| toy.seconds;

	error_num = diag_ptr->diag$l_error_num;
	error_msg = diag_ptr->diag$l_error_msg;
	fru = diag_ptr->diag$b_fru;
	test_num = diag_ptr->diag$l_test_num;
	pass = diag_ptr->diag$l_pass_count;
    }
    else 
    {
	error_num = 0;
	error_msg = 0;
	fru = months;		/* Blank */                    
	test_num = 0;
	pass = 0;
    }

#if MODULAR || WILDFIRE
    krn$_wait(&report_lock);
#endif

/* 
 * If D_BELL environment variable is set, sound bell
 * before printing error 
 */
    if (diag$r_evs & m_bell)
	    err_printf("%c", 7);

#if MEDULLA || CORTEX || YUKONA || YUKON
    /* 
     * #### See if the new error reporting format is being used. 
     */
    if (error_num & MED$M_EVFL_NEW_FORMAT)
    {
        /*
	 * New format
	 */
        err_printf("\n\n%m %m error%m%m\n***\t%m: %m\n***\tSW Failure Point: %d\n***\tError Vector: %08x\n\n%m  %s\n%-12.12s     %08.8x     %14.14s  %4.4d  %4.4d  %4.4d %4.4d     %8.8s\n", 
            msg, 
	    error_code ((error_num & MED$M_EV_ERROR_CODE) >> MED$V_EV_ERROR_CODE),
	    strcmp (error_msg, "") == 0 ? "" : " - ",    /* do we need to print the delimiter? */
	    error_msg,    /* This is additional info for Medulla errors */
	    (error_num & MED$M_EVFL_NON_DIAGNOSTIC) ? "Utility" : "Diagnostic",
	    diag_name ((error_num & MED$M_EV_DIAG_NAME_CODE) >> MED$V_EV_DIAG_NAME_CODE),
	    (error_num & MED$M_EV_FAILURE_POINT) >> MED$V_EV_FAILURE_POINT,
	    error_num, /* Print the whole error vector */
            msg_d_header, 
    	    ddate, 
    	    pcb->pcb$b_name, 
    	    pcb->pcb$l_pid,
    	    fru,
    	    pass, 
	    test_num,
    	    iob_ptr->hard_errors, 
    	    iob_ptr->soft_errors, 
    	    time);
    }
    else 
    {
        /*
	 * Old format
	 *
	 *  #### temporarily print print warning message
	 */
	pprintf ("*** Warning: Using old error message format!!!\n"); 
        err_printf(msg_d_error, 
    	    msg, 
	    error_num,
	    error_msg,
            msg_d_header, 
    	    ddate, 
    	    pcb->pcb$b_name, 
    	    pcb->pcb$l_pid,
    	    fru,
    	    pass, 
	    test_num,
    	    iob_ptr->hard_errors, 
    	    iob_ptr->soft_errors, 
    	    time);
    }
#else
#if RAWHIDE
    if ((!cpip) || ((cpip & (rtc_read(0x22) == 0x22))))
#endif
    err_printf(msg_d_error, 
    	msg, 
	error_num,
	error_msg,
    	msg_d_header, 
    	ddate, 
    	pcb->pcb$b_name, 
    	pcb->pcb$l_pid,
    	fru,
    	pass, 
	test_num,
    	iob_ptr->hard_errors, 
    	iob_ptr->soft_errors, 
    	time);

#endif

    return;
}


/*+
 * ======================================================================
 * = report_error_release - Complete report_error_lock().               =
 * ======================================================================
 *
 * OVERVIEW:
 *
 *	This routine is used to complete the error report started with the
 *	report_error_lock() routine.  This routine will complete the error
 *	report by printing the ending delimiter and releasing the lock on
 *	the standard error channel.  This routine will also make a call to
 *	the check_kill() routine to check if the kill_pending bit has been 
 *	set in the controlling process.  If an abort has been requested, 
 *	then the diagnostic is terminated.  If the diagnostic creates any 
 *	sub-processes and these sub-processes call report_error_release(),
 *	this routine will not likely see the ^c because the killpending bit
 *	is set in the controlling processes PCB and not the sub-processes.  
 *	In this case, the diagnostic will not be aborted and the diagnostic 
 *	must make sure to check the controlling processes killpending bit 
 *	periodically.
 *
 * FORM OF CALL:
 *
 *	report_error_release();
 *
 * RETURNS:
 *
 *	msg_success -- continue with remainder of diagnostic.
 *	msg_halt -- halt on error has been set -- stop execution of diagnostic
 *	 	    after returning.
 *	msg_loop -- loop on error has been set -- rerun this test again.
 *
 * ARGUMENTS:
 *
 *	None.
 *
 * IMPLICIT INPUTS:
 *
 *	- The pcb$a_stderr field in the PCB.
 *	- The pcb$r_ev structure in the PCB.
 *	- The diag$l_error_type field in the PCB.
 *
 * IMPLICIT OUTPUTS:
 *
 *	None.
 *
 * SIDE EFFECTS:
 *
 *	- The standard error channel is unlocked.
 *
-*/

int report_error_release(void) 
{
    struct toy_date_time toy;
    char ddate[32];
    struct PCB *pcb = getpcb();
    diag_common *diag_ptr = (diag_common *) pcb->pcb$a_diag_common;

#if RAWHIDE
    if ((!cpip) || ((cpip & (rtc_read(0x22) == 0x22))))
#endif
    err_printf(msg_d_err_end);

    if ((diag_ptr->diag$l_error_type == DIAG$K_SOFT) &&
      (diag$r_evs & m_halt_soft) ||
      (diag_ptr->diag$l_error_type == DIAG$K_HARD) &&
      (diag$r_evs & m_halt_hard) ||
      (diag_ptr->diag$l_error_type == DIAG$K_FATAL)) {

#if MODULAR || WILDFIRE
	krn$_post(&report_lock);
#endif
	return (msg_halt);

    }

    get_date_time (&toy);

    /*
    /* Format the date and time strings...
    */
    sprintf (ddate, "%2d-%s-%04d",
	   toy.day_of_month,
           months [toy.month % ((sizeof months) /  sizeof (months [0]))],
           genyear (toy.year));

    /* If the end of pass message or test trace is enabled and loop or continue
     * on error is set, will need to print out header  again for next EOP or TT
     * message */
    if (diag$r_evs & m_eop || diag$r_evs & m_tt)
#if RAWHIDE
    if ((!cpip) || ((cpip & (rtc_read(0x22) == 0x22))))
#endif
    	printf("%m  %s\n", msg_d_header, ddate);

#if MODULAR || WILDFIRE
    krn$_post(&report_lock);
#endif

    /* Check to see if kill_pending bit was set if looping or continuing on
     * error */

    check_kill();

    if ((diag_ptr->diag$l_error_type == DIAG$K_SOFT) &&
      (diag$r_evs & m_loop_soft) ||
      (diag_ptr->diag$l_error_type == DIAG$K_HARD) &&
      (diag$r_evs & m_loop_hard)) {

	return (msg_loop);
    }

    return (msg_success);
}


/*+
 * ======================================================================
 * = report_failure - Report a failure of a diagnostic                  =
 * ======================================================================
 *
 * OVERVIEW:
 *
 *	This routine calls report_error after stuffing the appropriate  
 *	fields in the PCB with expected, received and failing address
 *	information.
 *
 * FORM OF CALL:
 *
 *	report_failure();
 *
 * RETURNS:
 *
 *	Status from report_error
 *
 * ARGUMENTS:
 *
 * 	type - error type (DIAG$K_HARD, DIAG$K_SOFT, DIAG$K_FATAL)
 * 	number - error number (0,1,2,3.....)
 *      message - error message number
 *      expect - expected data 
 * 	rcvd - received data
 * 	address - failing address
 *
 * IMPLICIT INPUTS:
 *
 *	- A diag_common structure in the PCB
 *
 * IMPLICIT OUTPUTS:
 *
 *	- Initialized elements in the diag_common struct
 * 	- Calls log_failure with same args.
 *
 * SIDE EFFECTS:                                        
 *
 *	Calls out an error using report_error
 *
-*/
int report_failure (u_int type,u_int number,u_int message,u_int expect,
		u_int rcvd,u_int *address)
{
	/* Log error information */
	log_failure(type,number,message,expect,rcvd,address);
	return(report_error());		/* Report error, return status */
}	


/*+
 * ======================================================================
 * = log_failure - log failure information of a diagnostic              =
 * ======================================================================
 *
 * OVERVIEW:
 *
 * 	This routine will accept error information as parameters and load 
 *	up the proper error fields in the pcb and diag_common structure.
 *
 * FORM OF CALL:
 *
 *	log_failure();
 *
 * RETURNS:
 *
 *	void 
 *
 * ARGUMENTS:
 *
 * 	type - error type (DIAG$K_HARD, DIAG$K_SOFT, DIAG$K_FATAL)
 * 	number - error number (0,1,2,3.....)
 *      message - error message number
 *      expect - expected data 
 * 	rcvd - received data
 * 	address - failing address
 *
 * IMPLICIT INPUTS:
 *
 *	- A diag_common structure in the PCB
 *
 * IMPLICIT OUTPUTS:
 *
 *	- Initialized elements in the diag_common struct
 *
 * SIDE EFFECTS:                                        
 *
-*/
void log_failure (u_int type,u_int number,u_int message,u_int expect,
		  u_int rcvd,u_int *address)
{
	struct PCB	   	*pcb=getpcb();
	diag_common		*gptr=(diag_common *)pcb->pcb$a_diag_common;

	/* load up necessary error fields for report_error */
	gptr->diag$l_error_type = type;
	gptr->diag$l_error_num = number;
	gptr->diag$l_error_msg = message;      
	/* define a fru entry if we haven't allready */
	if (gptr->diag$b_fru[0] == '\0')
	    strcpy(gptr->diag$b_fru, pcb->pcb$b_dev_name);

	/* Extended error info */
	gptr->diag$l_expect = expect; 
	gptr->diag$l_rcvd = rcvd;     
	gptr->diag$l_failadr = (u_int)address; 
}	

/*+
 * ======================================================================
 * = report_failure_quad - Report a failure of a diagnostic             =
 * ======================================================================
 *
 * OVERVIEW:
 *
 *	This routine calls report_error after stuffing the appropriate  
 *	fields in the PCB with expected, received and failing address
 *	information.
 *
 * FORM OF CALL:
 *
 *	report_failure_quad();
 *
 * RETURNS:
 *
 *	Status from report_error
 *
 * ARGUMENTS:
 *
 * 	type - error type (DIAG$K_HARD, DIAG$K_SOFT, DIAG$K_FATAL)
 * 	number - error number (0,1,2,3.....)
 *      message - error message number
 *      expect - expected data 
 * 	rcvd - received data
 * 	address - failing address
 *
 * IMPLICIT INPUTS:
 *
 *	- A diag_common structure in the PCB
 *
 * IMPLICIT OUTPUTS:
 *
 *	- Initialized elements in the diag_common struct
 * 	- Calls log_failure with same args.
 *
 * SIDE EFFECTS:                                        
 *
 *	Calls out an error using report_error
 *
-*/
int report_failure_quad (u_int type,u_int number,u_int message,u__int64 expect,
		u__int64 rcvd,u__int64 address)
{
	/* Log error information */
	log_failure_quad(type,number,message,expect,rcvd,address);
	return(report_error());		/* Report error, return status */
}	


/*+
 * ======================================================================
 * = log_failure_quad - log failure information of a diagnostic         =
 * ======================================================================
 *
 * OVERVIEW:
 *
 * 	This routine will accept error information as parameters and load 
 *	up the proper error fields in the pcb and diag_common structure.
 *
 * FORM OF CALL:
 *
 *	log_failure_quad();
 *
 * RETURNS:
 *
 *	void 
 *
 * ARGUMENTS:
 *
 * 	type - error type (DIAG$K_HARD, DIAG$K_SOFT, DIAG$K_FATAL)
 * 	number - error number (0,1,2,3.....)
 *      message - error message number
 *      expect - expected data 
 * 	rcvd - received data
 * 	address - failing address
 *
 * IMPLICIT INPUTS:
 *
 *	- A diag_common structure in the PCB
 *
 * IMPLICIT OUTPUTS:
 *
 *	- Initialized elements in the diag_common struct
 *
 * SIDE EFFECTS:                                        
 *
-*/
static void log_failure_quad (u_int type,u_int number,u_int message,u__int64 expect,
		  u__int64 rcvd,u__int64 address)
{
    struct PCB	   	*pcb=getpcb();
    diag_common		*gptr=(diag_common *)pcb->pcb$a_diag_common;

    /* load up necessary error fields for report_error */
    gptr->diag$l_error_type = type;
    gptr->diag$l_error_num = number;
    gptr->diag$l_error_msg = message;      

    /* define a fru entry if we haven't allready */
    if (gptr->diag$b_fru[0] == '\0')
	strcpy(gptr->diag$b_fru, pcb->pcb$b_dev_name);

    /* Extended error info */
    *(u__int64 *)(&gptr->diag$l_expect) = expect; 
    *(u__int64 *)(&gptr->diag$l_rcvd) = rcvd;     
    *(u__int64 *)(&gptr->diag$l_failadr) = address; 

    /* If the upper diagnostic has not defined an extended error 
     * printer for this then use this generic one. */
    if (gptr->diag$a_ext_error == NULL)
	gptr->diag$a_ext_error = diag_print_ext_error;
}	

/*+
 * ============================================================================
 * = diag_print_ext_error - Prints Extended error info loaded in PCB          =
 * ============================================================================
 *
 * Function Description:	
 *   The address of this routine is loaded into diag->diag$a_ext_error by the
 *   load_address_quad routine if there has not been one defined before.
 *   During a SUMMARY LEVEL error report, this routined will get called to
 *   print EXTENDED LEVEL errors.  If this pointer is NULL, no EXTENDED LEVEL 
 *   errors are printed.
 *
 * Implicit Inputs:
 *   If called,  the PCB is loaded with EXTENDED LEVEL error information.
 *
 * Implicit Outputs:
 *   none
 * Return Values:
 *   none
-*/
static void diag_print_ext_error()
{
    struct PCB            *pcb;         /* Process Control Block pointer */
    diag_common           *cptr;        /* ptr to struct diag_common_struct */
                                        /* found in the file diag_def.h */
    u__int64  expect, rcvd, address;

    pcb  = getpcb();             	
    cptr = pcb->pcb$a_diag_common;

    /* Load up the Extended Level error fields
    */
    expect  = *(u__int64 *)(&cptr->diag$l_expect);
    rcvd    = *(u__int64 *)(&cptr->diag$l_rcvd);    
    address = *(u__int64 *)(&cptr->diag$l_failadr);

    /* When all of these fields are NULL means a NON-Extended error 
     * was called therefore defining no extended data. */
#if RAWHIDE
    if ((!cpip) || ((cpip & (rtc_read(0x22) == 0x22))))
#endif
    if (!((expect == NULL) & (rcvd == NULL) & (address == NULL))) {
        err_printf("Expected value:\t %16x\n", expect);
        err_printf("Received value:\t %16x\n", rcvd);
#if MEDULLA || CORTEX || YUKONA || YUKON
	/*
	 * Do not print address information if none is contained in the 
	 * extended error information. 
	 */
	if ( (cptr->diag$l_error_num & MED$M_EVFL_ADDR_SPACE)
			!= MED$K_EVFL_NO_ADDR)
#endif
        err_printf("Failing addr:\t %16x\n",  address);
    }
}    

/*+
 * ======================================================================
 * = FIND_PCB_IOB - Find the IOB Corresponding To Device Under Test    =
 * ======================================================================
 *
 * OVERVIEW:
 *
 *	This routine will walk down the IOB's specific to this process
 *	and find a match for the device name under test.  The routine 
 *	will return the IOB pointer that can be referenced later to
 *	retrieve necessary summary and error information.
 *
 * FORM OF CALL:
 *
 * 	find_pcb_iob(void)
 *
 * RETURNS:
 *
 *	struct IOB *iob_ptr -- pointer to iob corresponding to device under test
 *	NULL pointer -- pointer to NULL if no pcb device name filled in or no
 *		match found
 *
 * ARGUMENTS:
 *
 *	None.
 *
 * IMPLICIT INPUTS:
 *
 *	- The pcb$b_dev_name field of the PCB.
 *
 * IMPLICIT OUTPUTS:
 *
 *	- None.
 *
 * SIDE EFFECTS:
 *
-*/
struct IOB *find_pcb_iob(void) 
{
    int dev_match_found = 0;
    struct PCB *pcb;
    struct QUEUE *pcb_iobq_ptr;
    struct IOB *iob_ptr;

    pcb = getpcb();

/* First check that pcb->pcb$b_dev_name exists -- if it doesn't make
   error callout and return bad status */
    if (pcb->pcb$b_dev_name[0] == '\0') {
/*	    err_printf(msg_d_no_dev_name); */
	return (NULL);
    }

/* Search down pcb$r_iobq to find IOB with name that matches device type */

    pcb_iobq_ptr = pcb->pcb$r_iobq.flink;
    if (pcb_iobq_ptr == 0)
	return NULL;

    while (pcb_iobq_ptr != &(pcb->pcb$r_iobq)) {

/* Start at beginning of PCBQ */

	iob_ptr = (struct IOB *) ((unsigned int) pcb_iobq_ptr - offsetof(struct IOB, pcbq));

/* If find device name match, end */

	if (strcmp(iob_ptr->name, pcb->pcb$b_dev_name) == 0) {
	    dev_match_found = 1;
	    break;
	}

/* If don't find device_name, search next IOB */

	pcb_iobq_ptr = pcb_iobq_ptr->flink;
    }

    if (dev_match_found)
	return (iob_ptr);

    else {
/*	    err_printf(msg_d_no_iob_found,pcb->pcb$b_dev_name);		*/
	return (NULL);
    }
}

#if 0
/*+
 * ============================================================================
 * = KILL_DIAG - = Kills off diagnostics/exercisers running in system
 * ============================================================================
 *
 * OVERVIEW:
 *	This routine will go down the PCB list and check for any process
 *	name ending in EXACTLY _ex or _tst.  It will then set the kill
 *	pending bits in these processes to kill them off.
 *
 * FORM OF CALL:
 *
 * 	kill_diag()
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	None.
 *
 * IMPLICIT INPUTS:
 *
 *	None.
 *
 * IMPLICIT OUTPUTS:
 *
 *	None.
 *
 * SIDE EFFECTS:
 *	
 *	All diagnostics/exercisers running in the system will be 'killed'.
 *
-*/
void kill_diag()
{
    char check_string[10];
    char *string;
    struct QUEUE *p;
    struct PCB *pcb = getpcb();

    /* Set address of pointer to already malloc'd array */
    string = check_string;

    spinlock(&spl_kernel);
    p = pcbq.flink;
    while (p != &pcbq) {
	pcb = (struct PCB *) ((int) p - offsetof(struct PCB, pcb$r_pq));

	/* Kill off all diagnostic processes (tests and exercisers) */

	/* Check if _ex ONLY not _ex* which may mean a polling process  or
	 * something other than an exerciser */
	if ((string = strstr(pcb->pcb$b_name, "_ex")) != NULL) {
	    if (string[3] == NULL) {
/*             pprintf("    Process : %08x : %s\n",pcb,pcb->pcb$b_name); */
		pcb->pcb$l_killpending = 1;
	    }
	}

	if ((string = strstr(pcb->pcb$b_name, "_tst")) != NULL) {
	    if (string[4] == NULL) {
/*          pprintf("    Process : %08x : %s\n",pcb,pcb->pcb$b_name); */
		pcb->pcb$l_killpending = 1;
	    }
	}

	p = p->flink;
    }
    spinunlock(&spl_kernel);
}
#endif

/*
 * Generate the proper year given the limitations of the chip and the different
 * ways VMS and OSF want to store the year.
 */
int genyear (int year) {
	if (year < 100) {
	    if (year < 70) {
		return year + 2000;
	    } else {
		return year + 1900;
	    }
	} else {
	    return year + VMS_BIAS;
	}
}

/* redefine mask */
#define mask(x,y) ((x)<<(y))
