/*
 *        Copyright (c) Digital Equipment Corporation, 1991
 *        All Rights Reserved.  Unpublished-rights reserved
 *        under the copyright laws of the United States.
 *
 *        The software contained on this media is proprietary to"
 *        and embodies confidential technology of Digital.  Possession,
 *        use, duplication or dissemination of the software and media
 *        is authorized only pursuant to a valid written license from
 *        Digital.
 *
 *        RESTRICTED RIGHTS LEGEND  Use, duplication, or disclosure"
 *        by the U.S. Government is subject to restrictions as set 
 *        forth in subparagraph (c)(1)(ii) of DFARS 252.227-7013, or 
 *        in FAR 52.227-19, as applicable.
 */


/*
 *++
 *  FACILITY:
 *
 *      FLAMINGO Firmware
 *
 *  ABSTRACT:
 *	Definitions for callback vectors to make call easier
 *
 *  AUTHORS:
 *
 *      Jim Peacock
 *
 *  CREATION DATE:      14-November 1991
 *
 *  MODIFICATION HISTORY:
 *
 *--
*/

#define CREATE_JOB (*stcv->_ker$create_job )
#define CREATE_DEVICE (*stcv->_ker$create_device )
#define CREATE_SEMAPHORE (*stcv->_ker$create_semaphore )
#define CREATE_AREA  (*stcv->_ker$create_area)
#define CREATE_EVENT (*stcv->_ker$create_event)
#define KER_EXIT (*stcv->_ker$exit)
#define WAIT (*stcv->_ker$wait )
#define DELETE (*stcv->_ker$delete )
#define HALT (*stcv->_ker$halt )
#define GET_ELAPSED (*stcv->_ker$get_elapsed )
#define GET_TIME_STRING (*stcv->_ker$get_time_string )
#define SET_TIME (*stcv->_ker$set_time )
#define SIGNAL (*stcv->_ker$signal )
#define SUSPEND_JOB (*stcv->_ker$suspend_job )
#define RESUME_JOB (*stcv->_ker$resume_job )
#define SET_PRIORITY (*stcv->_ker$set_priority )
#define SET_IPL (*stcv->_ker$set_ipl )
#define ALLOCATE_PHYSICAL (*stcv->_ker$allocate_physical )
#define ALLOCATE_PHYSICAL_64K (*stcv->_ker$allocate_physical_64K )
#define ALLOCATE_VIRTUAL (*stcv->_ker$allocate_virtual )
#define MAP_PHYSICAL (*stcv->_ker$map_physical )
#define FREE_PHYSICAL (*stcv->_ker$free_physical )
#define FREE_VIRTUAL (*stcv->_ker$free_virtual)
#define GET_FREE (*stcv->_ker$get_free )
#define GET_USED (*stcv->_ker$get_used )
#define SYS_INIT (*stcv->_rtl$init )
#define SYS_STATUS (*stcv->_rtl$status )
#define SYS_SUMMARY (*stcv->_rtl$summary )
#define SYS_CHECK (*stcv->_rtl$check )
#define SYS_EXIT (*stcv->_rtl$exit )
#define GET_ENV (*stcv->_srm$dispatch )
#define MAP_REGISTER (*stcv->_ker$map_register)
#define UNMAP_REGISTER (*stcv->_ker$unmap_register)

struct  save_info 
	{
	int	saved_pc;
	int	saved_ps;
	int	saved_vector;
	int	saved_sp;
	int	saved_perf;
	};

typedef struct stcv STCV ;
typedef struct jcb  JCB ; 
typedef struct sys_idb  IDB ; 
typedef struct job_context CONTEXT;
typedef struct object_type AREA;
typedef struct object_type SEMAPHORE;
typedef struct object_type DEVICE;
typedef struct object_type EVENT;
typedef struct sched SCHED;
typedef struct fifo FIFO;
typedef struct history HISTORY;
typedef struct job_param JOB_PARAM;
typedef struct isr_param ISR_PARAM;
typedef struct int_fifo INT_FIFO;
typedef struct hist_fifo HIST_FIFO;
typedef struct object_type OBJECT;
typedef struct sys_status  SYS_TEST_STATUS;
typedef struct sys_summary SYS_TEST_SUMMARY;
typedef struct system_test_module SYS_TEST;
typedef struct system_monitor_info SYS_INFO;
typedef struct local_stack  STACK;
typedef struct local_buffer BUFFER;
typedef struct job_list JOB_LIST;
typedef struct config CONFIG;
typedef struct comm   COMM;
typedef struct cxt_data CXT_DATA;
typedef struct term_function_block TERM;
typedef struct test_mode TEST_MODE;
typedef struct perf PERF;
typedef struct save_info SAVE_LIST;
