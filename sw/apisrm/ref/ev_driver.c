/*
 * file:	ev_driver.c
 *
 * Copyright (C) 1990 by
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
 * Abstract:	Environment Variable driver.
 *
 *		Driver that implements the environment variables for the
 *		Alpha firmware.
 *
 * Author:	David Mayo
 *
 * Modifications:
 *
 *	pel 	11-Feb-1993	Use exdep_inited flag to separate exdep from 
 *				ev_driver; remove call to exdep_init.
 *
 *	pel	26-Jun-1992	add qprintf msg in ev_init re nvram access
 *
 *	ajb	11-may-1992	Remove ev_scan
 *
 *	ajb	31-Mar-1992	Add ev_inited so premature calls get
 *				caught.
 *
 *      phk     15-Nov-1991     Pass the env variable name to the write 
 *				action routine
 *
 *	jds	 4-Oct-1991	Added increment of new CRB total PAGES field.
 *
 *	jds	23-May-1991	Changes for VAX callbacks.
 *
 *	phk     09-May-1991	Add an ev action on read
 *
 *	ajb	22-Apr-1991	Trnuncate legnthy strings so that they are
 *				still null terminated.
 *
 *	ajb	19-Apr-1991	Use strpbrk to validate environment variable
 *				names instead of regular expressions.
 *				
 *	dtm	15-Feb-1991	Add SRM Environment Variable attributes
 *				to facilitate callbacks.
 *
 *	kl	12-Feb-1991	Make primary a global variable rather than
 *				an environment variable.
 *
 *	dtm	 6-Feb-1991	fix memory leak in ev_validate.  alphabitize
 *				storage of environment variables.  remove itoa.
 *
 *	mrc	14-Jan-1991	modify ev_init() to call diag_ev_init().
 *
 *	dtm	14-Jan-1991	replace abbrev routine with substr.
 *
 *	dtm	20-Dec-1990	add default value field support.
 *
 *	dtm	12-Dec-1990	add version environment variable.
 *
 *	dtm	11-Dec-1990	malloc for hwrpb moved to memconfig().
 *
 *	dtm	28-Nov-1990	malloc address for hwrpb environment variable
 *
 *	dtm	31-Oct-1990	ev_validate_table subroutine created
 *				abbrev routine moved to strfunc.c
 *
 *	dtm	12-Oct-1990	move action routines to ev_action.c
 *				remove local environment lists
 *
 *	dtm	 5-Oct-1990	add verification and actions routines
 *
 *	dtm	21-Sep-1990	add protections and return codes.
 *
 *	dtm	14-Sep-1990	ev_local_init copies parent's local ev list.
 *				EVNODE value field changed to union.
 *				Caller now allocates storage for ev_read data.
 *				other modifications based on techinical review.
 *
 *	dtm	11-Sep-1990	Change ev_read to return copy of the EVNODE.
 *				Remove walk_ev.  Cleanup from CC/STAND=PORT.
 *				Fix variables holding integer values.
 *
 *	cfc	06-Sep-1990	callback routine ev_scan for qscan wild_cards.
 *
 *	dtm	6-Jul-1990	Initial entry.
 */
#include	"cp$src:platform.h"
#include	"cp$src:common.h"
#include	"cp$src:prototypes.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:prdef.h"
#include	"cp$src:version.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:ev_def.h"
#include	"cp$src:hwrpb_def.h"
#include	"cp$src:pal_def.h"
#include	"cp$src:impure_def.h"
#include	"cp$inc:kernel_entry.h"

#if MODULAR
extern int flash_var_major;
extern int flash_seq_minor;
extern int flash_creation_time;
extern struct environ_list *eeprom_env_ptr;
#endif

#define DEBUG 0

#define malloc(x) dyn$_malloc(x,DYN$K_SYNC|DYN$K_NOOWN)
#define realloc(x,y) dyn$_realloc(x,y,DYN$K_SYNC|DYN$K_NOOWN)
#define free(x) dyn$_free(x,DYN$K_SYNC)

extern struct EVNODE evlist;		/* header of the list of variables */
extern struct SEMAPHORE evlock;	/* serialize access for variable modification */
extern int ev_inited;		/* has driver been initialized */
extern int ev_initing;		/* driver is initializing */

extern struct EVNODE *evmem;
extern struct env_table env_table[];
extern struct env_table *env_table_ptr;
extern char ENV_VAR_TABLE[];
extern char ENV_VAR_TABLE_END[];

char *variant_ptr = &v_variant;
struct SEMAPHORE evlock_init_ev;	/* serialize access ev_init_ev */

#if GALAXY && WILDFIRE
extern int galaxy_node_number;
#endif

/*+
 * ============================================================================
 * = ev_init - initialize the environment variable subsystem                  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * 	Create and initialize the header to the environment variable
 *	list.  Create the global semaphore.
 *
 * FORM OF CALL:
 *
 *	ev_init()
 *
 * RETURN CODES:
 *
 *	msg_success	- environment variable subsystem initialized
 *
 * ARGUMENTS:
 *
 *	None
 *
 * SIDE EFFECTS:
 *
 *	environment variable list and semaphore created.
 *
-*/
int ev_init () {
    struct env_table *et;
    struct EVNODE *evp;
    char value[EV_VALUE_LEN];
    char buf[80];
    char *ev_value;
    int i,*j,k;
    int len;
    struct HWRPB *hwrpb;
    extern int HWRPB;
    struct CRB *crb;
    struct PALVERSION *palversion;
    struct PALVERSION *osfpalversion;
    struct FILE *fp = 0;

	env_table_ptr = &env_table;

	/*
	 * Set up the serialization semaphore. Used to serialize
	 * evlist modifications.
	 */
	krn$_seminit (&evlock, 1, "evlock");

	/* Plus do one for the ev_init_ev routine. */

	krn$_seminit (&evlock_init_ev, 1, "evlock_init_ev");

	/*
	 * Initialize the environment variable list.
	 */
	evlist.flink = &evlist;
	evlist.blink = &evlist;

	ev_initing = 1;
#if TURBO
	fp = eeprom_open_init();
#endif

	/*
	 * Initialize SRM and pre-defined environment variables.
	 */
	j = &ENV_VAR_TABLE;
	k = ((int)&ENV_VAR_TABLE_END - (int)j) / 16;
	evmem = malloc(k*sizeof(struct EVNODE));

	for (i=0,et = env_table; et->ev_name ; et++,i++) {

#if TURBO
    write_leds(i);
#endif

#if GALAXY && WILDFIRE
	    if (galaxy_node_number && !strncmp (et->ev_name, "lp_", 3)) {
		continue;
	    }
#endif

	    if (et->ev_init) {
		ev_value = et->ev_init (et, value);
	    } else {
		ev_value = et->ev_value;
	    }
	    if (i < k) {
		ev_write (et->ev_name, ev_value, et->ev_options|EV$K_NOMALLOC, &evmem[i]);
	    } else {
	    	ev_write (et->ev_name, ev_value, et->ev_options, NULL);
	    }
	    ev_install_table (et->ev_name, et->validate);
	    ev_install_action (et->ev_name, et->ev_wr_action, et->ev_rd_action);

#if 0
	    if (i < k) {
		map_via_crb ((UINT)(evmem + i), (UINT)(j + i*4));
	    }
#endif
	}

	evp = malloc (sizeof(*evp));
	if (ev_read("bootdef_dev", &evp, EV$K_SYSTEM) == msg_success)
	    ev_write ("boot_dev", evp->value.string, EV$K_SYSTEM|EV$K_STRING);
	free (evp);

#if TURBO
/*
 * Go thru eeprom environment variables, if it is not in the environment
 * table, create an in memory copy.
 */
	eeprom_update_env();
#endif
#if GALAXY && RAWHIDE
/*
 * Same as TURBO case, above.
 */
	nvram_update_env();
#endif

#if MODULAR
    len = sprintf(value, "%c%d.%d-%d, ",
	flash_var_major >> 16, flash_var_major & 0xffff,
	flash_seq_minor & 0xffff, flash_seq_minor >> 16);
    print_time(buf, flash_creation_time);
    strcpy(value+len, buf);
#else
	sprintf (value, "%c%d.%d-%d %s %s",
		*variant_ptr,
		v_major,
		v_minor,
		v_sequence,
		v_date,
		v_time
	);
#endif
	ev_write ("version", value, EV$K_STRING|EV$K_NOWRITE|EV$K_NODELETE);

	palversion = PAL$PAL_BASE + 8;
	osfpalversion = PAL$OSFPAL_BASE + 8;
#if EV6
	sprintf (value, "OpenVMS PALcode %c%d.%02d-%d, Tru64 UNIX PALcode %c%d.%02d-%d",
#else
	sprintf (value, "OpenVMS PALcode %c%d.%02d-%d, Digital UNIX PALcode %c%d.%02d-%d",
#endif
		palversion->is_v ? 'V' : 'X',
		palversion->major,
		palversion->minor,
		palversion->xxx,
		osfpalversion->is_v ? 'V' : 'X',
		osfpalversion->major,
		osfpalversion->minor,
		osfpalversion->xxx
	);
	ev_write ("pal", value, EV$K_STRING|EV$K_NOWRITE|EV$K_NODELETE);

#if DIAG_SUPPORT
/* 
 * Call diag_ev_init to initialize environment variables used by
 * diagnostics.
 */
	diag_ev_init();
#endif

#if !(TURBO || RAWHIDE)
#if ARC_SUPPORT
	arc_evinit ();
#endif
#endif

	ev_initing = 0;
	ev_inited = 1;

#if TURBO
	if (fp) {
	    eeprom_file_close(fp, FALSE);
	}
#endif
	return msg_success;
}

void ev_init_ev (struct env_table *et, int misc)
{
    int status;
    char value[EV_VALUE_LEN];
    char *ev_value;
    char *name;
    struct EVNODE *evp;

	krn$_wait (&evlock_init_ev);
	ev_initing = 1;
	name = et->ev_name;

	if (et->ev_init) {
	    ev_value = et->ev_init (et, value, misc);
	} else {
#if TURBO || RAWHIDE
	    if (et->ev_options & EV$K_NONVOLATILE) {
		ev_value = ev_sev_init (et, value);
	    } else {
		ev_value = et->ev_value;
	    }
#else
	    ev_value = et->ev_value;
#endif
	}

	status = ev_write (name, ev_value, et->ev_options);
	if (status != msg_success) {
	    err_printf("ev_init_ev: ev_write failure\n");
	    return;
	}

	krn$_wait (&evlock);

	if (ev_locate (&evlist, name, &evp) != msg_success) {
	    krn$_post (&evlock);
	    err_printf("ev_init_ev: ev_locate failure\n");
	    return;
	}

	evp->misc = misc;
	krn$_post (&evlock);

	ev_install_table (name, et->validate);
	ev_install_action (name, et->ev_wr_action, et->ev_rd_action);

	ev_initing = 0;

#if TURBO || RAWHIDE
	if (et->ev_options & EV$K_NONVOLATILE)
	     ev_sev(name, evp);		/* create non-volatile copy */
#endif
	krn$_post (&evlock_init_ev);
}

/*+
 * ============================================================================
 * = ev_read - read an environment variable                                   =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * 	Read a name-value pair from the environment variable data structure.
 *	Reading of the variable is subject to the constraints imposed by the 
 *	saved attributes of the variable (user read protection) only if the 
 *	EV$K_USER option is specified.
 *
 *	If the environment variable name is found, all information about
 *	the environment variable is copied from the environment variable
 *	database into a buffer area.  The address of the pointer to this 
 *	buffer is passed to the routine by the caller, who must also allocate 
 *	the memory associated with the buffer.  The buffer should be allocated
 *	of size: sizeof(struct EVNODE)
 *
 * FORM OF CALL:
 *
 *	ev_read (name, evptr, option)
 *
 * RETURN CODES:
 *
 *	msg_success	- environment variable read
 *	msg_noev	- environment variable not found
 *	msg_ev_prot	- environment variable protected from read access
 *
 * ARGUMENTS:
 *
 *      char *name	- string containing name of the environment variable
 *	struct EVNODE **evptr - address of a pointer to an evnode.
 *			Points to a buffer area, to which all environment
 *			variable information is copied.
 *	int option	- longword options to environment variable read
 *
 * SIDE EFFECTS:
 *
 *      environment variable list referenced.
 *	EVLOCK semaphore is used to serialize environment variable accesses.
 *
-*/
int ev_read (char *name, struct EVNODE **evptr, int option) {
	struct EVNODE *evp,*list;
	int (*ovly)();
	int ovly_name = 0;
	int i,status;
	
	/*
	 * Synchronize access
	 */
	krn$_wait (&evlock);

	/* 
	 * Search the EVLIST
	*/
	status = ev_locate(&evlist,name,&evp);
	if ((evp->attributes & EV$M_UNUSED) == EV$K_UNUSED)
	    status = msg_noev;
	if (status == msg_success) {
	/*
	 * found, check protections then make a copy of the EVNODE entry 
	 * and return 
	 */
	    if (((option & EV$M_USER) == EV$K_SYSTEM) ||
	  	    ((evp->attributes & EV$M_READ) == EV$K_READ)) {

		memcpy(*evptr,evp,sizeof(struct EVNODE));
		ovly = evp->rd_action;
#if MODULAR
		if ((int)ovly & 0x80000000) {
		    ovly_name = (int)ovly & ~0x80000000;
		    ovly = ovly_call_setup(ovly);
		}
#endif
	        if (ovly)
		    (ovly)(evp->name,evp,*evptr);
#if MODULAR
		if (ovly_name)
		    ovly_remove(ovly_name);
#endif
            }
	    else status = msg_ev_prot;
	}

	krn$_post (&evlock);
	return status;
}


/*+
 * ============================================================================
 * = ev_write - write an environment variable                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * 	Enter a name-value pair into the environment variable data structure.
 *	If the name already exists, overwrite the previous value.  If the name
 *	does not exist, create a new evnode.  
 *
 *	If the variable already exists, writing is subject to the constraints 
 *	imposed by the saved attributes of the variable.  If the variable is 
 *	created, it is given the attributes specified by the caller.  Valid 
 *	attribute options are:
 *~
 *	EV$K_INTEGER : Save the value of the environment variable as an
 *	integer, rather than in ascii string form.  By default, values are
 *	stored as ascii strings.
 *
 *	EV$K_NONVOLATILE : Save the environment variable definition in
 *	non-volatile memory.  This will allow the variable to be preserved
 *	across system resets.  Be default, variables are saved only in
 *	system memory, and disappear when the system is initialized.
 *
 *	EV$K_NOREAD : Disable user read access of the environment variable.
 *	By default, environment variables are given read access.
 *
 *	EV$K_NOWRITE : Disable user write access of the environment variable.
 *	By default, environment variables are given write access.
 *
 *	EV$K_NODELETE : Disable user delete access of the environment variable.
 *	By default, environment variables are given delete access.
 *
 *	EV$K_USER : Perform the operation, contingent upon the user protections.
 *	This flag allows the firmware to operate freely upon the environment
 *	variables, while restricting access from the SET command.  By default,
 *	access is not subject to the user write protection.
 *
 *	EV$K_ROUTINE : If validation of the environment variable value is to
 *	be performed, setting this flag indicates that the validation is in
 *	the form of a supplied routine, as opposed to the built-in table-based
 *	validation.
 *
 * 	EV$K_NOMALLOC : The new environment variable should not be malloc'd
 *	by ev_write.  The space for the new EVNODE is passed in via the evnode
 *	argument.
 *~
 *
 * FORM OF CALL:
 *
 *	ev_write (name, value, option, evnode)
 *
 * RETURN CODES:
 *
 *	msg_success	- environment variable written
 *	msg_noev	- environment variable not found (bad name)
 *	msg_ev_prot	- environment variable protected from write access
 *
 * ARGUMENTS:
 *
 *      char *name	- string containing name of the environment variable
 *	void *value	- pointer to value to be assigned to the environment 
 *			  variable
 *	int  option	- longword containing environment variable attributes
 *	struct EVNODE *evnode	- pointer to space for new evnode; used when
 *				  EV$K_NOMALLOC is specified.
 *
 * SIDE EFFECTS:
 *
 *      Modified environment variable list.
 *	EVLOCK semaphore is used to serialize environment variable accesses.
 *
-*/

int ev_write (char *name, int value, int option, protoarg int evnode) {
	int i,status,found;
	int (*ovly)();
	int ovly_name = 0;
	struct EVNODE *evp,*list,*new;
	extern char idset []; /* defined in the shell */

	/*
	 * Validate the specified environment variable name
	 */
	if (strpbrk (name, idset) == 0) {
	    return msg_ev_badname;
	}

	/*
	 * Synchronize access
	 */
	krn$_wait (&evlock);

	/*
	 * Determine whether the environment variable currently exists.  Then
	 * either free the old value space or create a new entry.  In either
	 * case, evp is left pointing to a blank entry.
	 */

	if ((status = ev_locate(&evlist,name,&evp)) == msg_success) {
	    if (((option & EV$M_USER) == EV$K_USER) &&
	      ((evp->attributes & EV$M_WRITE) == EV$K_NOWRITE)) {

	    /*
	     * Environment variable has been found, but is write protected
	     */
	 	krn$_post (&evlock);
		return msg_ev_prot;
	    }

	    /*
	     * Environment variable has been found, and is writeable
	     */
	    else {
		/*
		 * Validate the value, if a table is specified
		 */
		ovly = evp->validate.table;
#if MODULAR
		if ((int)ovly & 0x80000000) {
		    ovly_name = (int)ovly & ~0x80000000;
		    ovly = ovly_call_setup(ovly);
		}
#endif
		if (ovly) {
		    if ((evp->attributes & EV$M_VALIDATE) == EV$K_TABLE)
			status = ev_validate_table(&value,ovly);
		    else
			status = (ovly)(&value);
		}
		if (status == msg_success)
		    evp->attributes &= ~EV$M_UNUSED;
		else {
#if MODULAR
		    if (ovly_name)
			ovly_remove(ovly_name);
#endif
		    krn$_post (&evlock);
		    return msg_ev_badvalue;
		}
	    }
	    if ((evp->attributes & EV$M_NOMALLOC) != EV$K_NOMALLOC) {
		new = evp->blink;
		remq(evp);
		evp = (void *)realloc(evp, sizeof(struct EVNODE));
		insq(evp, new);
	    }
	}

	/* 
	 * Environment variable does not exist.  Create and initialize
	 * the evnode.
	 */
	else {
	    if ((option & EV$M_NOMALLOC) == EV$K_NOMALLOC)
	    	new = (struct EVNODE *)evnode;
	    else
		new = (void *)malloc(sizeof(struct EVNODE));
	    strncpy(new->name, name, sizeof (new->name) - 1);
	    insq(new, evp);
	    evp = new;
	    evp->validate.table = 0;
	    evp->wr_action = 0;
	    evp->rd_action = 0;
	    if ((option & EV$M_TYPE) == EV$K_STRING) {
		option |= EV$K_DEF_STRING;
	    }
	    else {
		option |= EV$K_DEF_INTEGER;
	    }
	    evp->attributes = option;
	    status = msg_success;
	}

	/*
	 * Now fill in the value.  If the variable already existed as an
	 * integer, save the new value as an integer, even if not specified
	 * as an option. If the option specifies an integer, make sure
	 * the evnode is set as an integer.
	 */
	if (status == msg_success) {
	    if ((option & EV$M_TYPE) == EV$K_STRING)
		if ((evp->attributes & EV$M_TYPE) == EV$K_INTEGER) {
		    evp->value.integer = xtoi(value);
		    evp->size = sizeof(evp->value.integer);
		} else {
		    strncpy(evp->value.string,value,sizeof (evp->value.string) - 1);
		    evp->size = strlen(evp->value.string);
		}
	    else if ((option & EV$M_TYPE) == EV$K_INTEGER) {
		evp->value.integer = (int)value;
		evp->size = sizeof(evp->value.integer);
		evp->attributes |= EV$K_INTEGER;
	    }
	    evp->attributes &= ~EV$M_MODIFIED;
	}
#if TURBO || RAWHIDE
	if ((!evp->wr_action) && (status == msg_success)) {
	    if ((evp->attributes & EV$M_VOLATILE) == EV$K_NONVOLATILE)
		status = ev_sev (name,evp);
	}
#endif
#if MODULAR
	if (ovly_name)
	    ovly_remove(ovly_name);
#endif
	/*
	 * Release the serialization lock
	 */
	krn$_post (&evlock);

	ovly = evp->wr_action;
#if MODULAR
	if ((int)ovly & 0x80000000) {
	    ovly_name = (int)ovly & ~0x80000000;
	    ovly = ovly_call_setup(ovly);
	}
#endif
	if (ovly && (status == msg_success))
	    status = (ovly)(name,evp);
#if MODULAR
	if (ovly_name)
	    ovly_remove(ovly_name);
#endif

	if ((evp->attributes & EV$M_NOMALLOC) != EV$K_NOMALLOC) {
	    new = evp->blink;
	    remq(evp);
	    i = sizeof(struct EVNODE) - EV_VALUE_LEN + evp->size + 1;
	    evp = (void *)realloc(evp, i);
	    insq(evp, new);
	}

	return status;
}


/*+
 * ============================================================================
 * = ev_delete - delete an environment variable                               =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * 	Delete a name-value pair from the environment variable data structure.
 *	Deleting of the variable is subject to the constraints imposed by the 
 *	saved attributes of the	variable (user delete protection) only if the 
 *	EV$K_USER option is specified.
 *
 * FORM OF CALL:
 *
 *	ev_delete (name, option)
 *
 * RETURN CODES:
 *
 *	msg_success	- environment variable deleted
 *	msg_noev	- environment variable not found (bad name)
 *	msg_ev_prot	- environment variable protected from delete access
 *
 * ARGUMENTS:
 *
 *      char *name	- string containing name of the environment variable
 *	int option	- longword option to environment variable delete
 *
 * SIDE EFFECTS:
 *
 *      Modified environment variable lists.
 *	EVLOCK semaphore is used to serialize environment variable accesses.
 *
-*/                  
int ev_delete (char *name, int option) {
	struct EVNODE *evp,*list;    
	int i,status;

	krn$_wait (&evlock);

	/*
	 * Determine whether the environment variable exists.
	 */
	if ((status = ev_locate(&evlist,name,&evp)) == msg_success)

	    /*
	     * found, check protections then delete entry if allowed
	     */
	    if (((option & EV$M_USER) == EV$K_SYSTEM) |
	      ((evp->attributes & EV$M_DELETE) == EV$K_DELETE)) {
		if ((evp->attributes & EV$M_SRM) == EV$K_SRM_DEF)
		    evp->attributes |= EV$K_UNUSED;
		else {
#if TURBO                          
		    if (evp->attributes & EV$K_NONVOLATILE)
			nvram_delete_sev(name);
#endif
		    remq(evp);
		    free (evp);
		}
	    }
	    else status = msg_ev_prot;

 	/*
	 * release the semaphore and return
	 */
	krn$_post (&evlock);
	return status;
}


#if 0
/*+
 * ============================================================================
 * = ev_chmod - change environment variable attributes                        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine modifies the user read, write, and delete protection
 *	fields of the specified environment variable attributes.
 *
 * FORM OF CALL:
 *
 *	ev_chmod (name, option)
 *
 * RETURN CODES:
 *
 *	msg_success	- environment variable found
 *
 * ARGUMENTS:
 *
 *      char *name	- string containing name of the environment variable
 *	int option	- new protections to environment variable
 *
 * SIDE EFFECTS:
 *
 *      Modified environment variable lists.
 *	EVLOCK semaphore is used to serialize environment variable accesses.
 *
-*/
int ev_chmod (char *name, int option) {
	struct EVNODE *evp,*list;
	int i,status;

	krn$_wait (&evlock);

	/*
	 * Determine whether the environment variable exists.
	 */
	if ((status = ev_locate(&evlist,name,&evp)) == msg_success) {

	    /*
	     * found, modify user protections
	     */
	    evp->attributes &= ~(EV$M_READ | EV$M_WRITE | EV$M_DELETE);
	    evp->attributes |= (option & (EV$M_READ | EV$M_WRITE | EV$M_DELETE));
	}

	krn$_post (&evlock);
	return status;
}
#endif


/*+
 * ============================================================================
 * = ev_locate - search environment variable list for specific name           =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Search the specified environment variable list for the specified
 *	variable name.  If the name is found in the list, return a pointer
 *	to the corresponding evnode.  If the name is not found, return a
 *	pointer to the node after which this name would be inserted.
 *
 * FORM OF CALL:
 *
 *	ev_locate (evlist, name, evptr)
 *
 * RETURN CODES:
 *
 *	msg_success	- environment variable found
 *	msg_noev	- environment variable not found
 *
 * ARGUMENTS:
 *
 *	struct EVNODE *evlist - pointer to environment variable list
 *			to be searched
 *      char *name	- string containing name of the environment variable
 *	struct EVNODE **evpp - pointer to pointer to the corresponding evnode
 *
 * SIDE EFFECTS:
 *
 *      environment variable lists referenced.
 *
-*/
int ev_locate (struct EVNODE *evlist, char *name, struct EVNODE **evpp) {
	struct EVNODE *evp;
	
	/*
     	 * Search the evlist 
	 */
	evp = evlist->flink;
	while (evp != evlist) {
	    if (strcmp_nocase(name,evp->name) == 0) {
		*evpp = evp;
		return msg_success;
	    }
	    if (strcmp_nocase(name,evp->name) < 0) {
		*evpp = evp->blink;
		return msg_noev;
	    }
	    evp = evp->flink;
	}

	*evpp = evp->blink;
	return msg_noev;
}


/*+
 * ============================================================================
 * = ev_install_table - install environment variable value validation table   =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Insert the address of a table to be used to validate supplied values
 *	when an environment variable is written.
 *
 * FORM OF CALL:
 *
 *	ev_install_table (name, table)
 *
 * RETURN CODES:
 *
 *	msg_success	- environment variable found
 *	msg_noev	- environment variable not found
 *
 * ARGUMENTS:
 *
 *      char *name	- string containing name of the environment variable
 *	struct set_param_table *table - address of validation table
 *
 * SIDE EFFECTS:
 *
 *      environment variable lists referenced.
 *	EVLOCK semaphore is used to serialize environment variable accesses.
 *
-*/
int ev_install_table (char *name, struct set_param_table *table) {
	struct EVNODE *evp;
	int status;
	
	krn$_wait (&evlock);

	/*
	 * Determine whether the environment variable exists.
	 */
	if ((status = ev_locate(&evlist,name,&evp)) == msg_success) 
		evp->validate.table = table;

	krn$_post (&evlock);
	return status;
}


/*+
 * ============================================================================
 * = ev_install_action - install an environment variable action routine       =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Insert the address of a routine to be executed when the specified
 *	environment variable is written.
 *
 * FORM OF CALL:
 *
 *	ev_install_action (name, wr_action,rd_action)
 *
 * RETURN CODES:
 *
 *	msg_success	- environment variable found
 *	msg_noev	- environment variable not found
 *
 * ARGUMENTS:
 *
 *      char *name         - string containing name of the environment variable
 *	int (*wr_action)() - address of routine to be executed on write
 *	int (*rd_action)() - address of routine to be executed on read
 *
 * SIDE EFFECTS:
 *
 *      environment variable lists referenced.
 *	EVLOCK semaphore is used to serialize environment variable accesses.
 *
-*/
int ev_install_action (char *name, int (*wr_action)(), int (*rd_action)()) {
	struct EVNODE *evp;
	int status;
	int (*ovly)();
	int ovly_name = 0;
	
	krn$_wait (&evlock);

	/*
	 * Determine whether the environment variable exists.
	 */
	if ((status = ev_locate(&evlist,name,&evp)) == msg_success) {
		evp->wr_action = wr_action;
		evp->rd_action = rd_action;
        }

	krn$_post (&evlock);

	ovly = evp->wr_action;
#if MODULAR
	if ((int)ovly & 0x80000000) {
	    ovly_name = (int)ovly & ~0x80000000;
	    ovly = ovly_call_setup(ovly);
	}
#endif
	if ((status == msg_success) && ovly) {
	    (ovly)(evp->name, evp);
	}
#if MODULAR
	if (ovly_name)
	    ovly_remove(ovly_name);
#endif
	return status;
}

/*+
 * ============================================================================
 * = ev_validate_table - compare a string against a table of good values      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Walk down a table, looking for the specified value.  Comparisons
 *	are insensitive to case.  The translation is performed as long as
 *	there is a unique translation for the parameter.
 *
 * FORM OF CALL:
 *
 *	ev_validate_table (value, table)
 *
 * RETURN CODES:
 *
 *	msg_success	- value found in table
 *	msg_ev_badvalue	- value not found in table
 *
 * ARGUMENTS:
 *
 *	char **value	- address of pointer to string
 *	struct set_param_table *table
 *			- pointer to table of known values
 *
 * SIDE EFFECTS:
 *
 *	none
 *
-*/
int ev_validate_table(char **value, struct set_param_table *table) {
	struct set_param_table *tptr, *last;
	int matches;
	int status;

	matches = 0;
	status = 0;

	for (tptr = table; (status != 1) && tptr->param; tptr++) {
	    switch (status = substr_nocase (*value, tptr->param)) {
	    case 0: 		/* no match */
		break;
	    case 1:		/* exact match */
		matches = 1;
		last = tptr;
		break;
	    case 2:		/* substring */
		matches++;
		last = tptr;
		break;
	    }
	}

	/* we can have one and only one unique match */
	if (matches != 1) {
	    err_printf("bad value - valid selections:\n");
	    for (tptr = table; (status != 1) && tptr->param; tptr++) {
		err_printf("        %s\n",tptr->param);
	    }
	    return msg_ev_badvalue;
	} else {
	    *value = last->param;
	    return msg_success;
	}
}

#if 0
/*
 * Generate a list of environment variables whose attributes match
 * the mask value.
 */
void gen_arcevlist (int *argc, int *arglist, unsigned int mask) {
#define	MAX_ARC_EVS	64
	char	**argv;
	struct EVNODE *evp;

	*arglist = argv = malloc (MAX_ARC_EVS * sizeof (char *));
	*argc = 0;

	/* iterate over all of the environment variables */
    	krn$_wait (&evlock);
    	evp = evlist.flink;
    	while ((evp != &evlist) && (*argc < MAX_ARC_EVS)) {
            if ((evp->attributes & mask) == mask) {
		argv [*argc] = mk_dynamic (evp->name);
		(*argc)++;
	    }
	    evp = evp->flink;
    	}
    	krn$_post (&evlock);
#undef MAX_ARC_EVS
}
#endif
