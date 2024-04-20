/*
 * file:	diag_evs.c
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
 * Abstract:	Diagnostic environment varibles
 *
 * Author:	Console Firmware Team
 *
 */

#include "cp$src:platform.h"
#include "cp$src:kernel_def.h"
#include "cp$src:parser.h"
#include "cp$src:stddef.h"
#include "cp$src:common.h"
#include "cp$inc:prototypes.h"
#include "cp$src:diag_def.h"
#include "cp$src:diag_groups.h"
#include "cp$src:ev_def.h"
#include "cp$src:msg_def.h"
#include "cp$src:pal_def.h"

extern struct LOCK spl_kernel;
extern struct QUEUE pcbq;

/* Environment Variable stuff */

extern int diag$r_evs;

char diag_bell_ev[] = "d_bell";
char diag_clean_ev[] = "d_cleanup";
char diag_comp_ev[] = "d_complete";
char diag_eop_ev[] = "d_eop";
char diag_start_ev[] = "d_startup";
char diag_log_soft_ev[] = "d_logsoft";
char diag_log_hard_ev[] = "d_loghard";
char diag_oper_ev[] = "d_oper";
char diag_quick_ev[] = "d_quick";
char diag_stat_ev[] = "d_status";
char diag_tt_ev[] = "d_trace";

char diag_he_ev[] = "d_harderr";
char diag_se_ev[] = "d_softerr";
char diag_rpt_ev[] = "d_report";

char diag_grp_ev[] = "d_group";
char diag_pass_ev[] = "d_passes";
char diag_runtime_ev[] = "d_runtime";
char diag_verbose_ev[] = "d_verbose";
char diag_omit_ev[] = "d_omit";

#if RAWHIDE
char diag_iocrd_ev[] = "d_iocrd";
#endif

char diag_ev_off[] = "off";
char diag_ev_on[] = "on";
char diag_ev_cont[] = "continue";
char diag_ev_halt[] = "halt";
char diag_ev_loop[] = "loop";
char diag_ev_summ[] = "summary";
char diag_ev_full[] = "full";

static struct keycode_s_table offon_table[] = {
    {diag_ev_off, 2, 0, 0, 0}, 
    {diag_ev_on, 2, 0, 0, 1}, 
    {0}
};

static struct keycode_s_table hese_table[] = {
    {diag_ev_cont, 2, 0, 0, 0}, 
    {diag_ev_halt, 2, 0, 0, 1}, 
    {diag_ev_loop, 2, 0, 0, 2}, 
    {0}
};

static struct keycode_s_table rpt_table[] = {
    {"", -1, 0, 0, 0}, 
    {diag_ev_summ, 2, 0, 0, 1}, 
    {diag_ev_full, 2, 0, 0, 2}, 
    {"", -1, 0, 0, 3}, 
    {diag_ev_off, 2, 0, 0, 4}, 
    {0}
};

#if RAWHIDE
#define IOCRD_CODE 21
#endif

struct keycode_s_table diag_ev_table[] = {
    {diag_bell_ev, 3, parse$m_help, 0, 0}, 
    {diag_clean_ev, 3, parse$m_help, 0, 1}, 
    {diag_comp_ev, 3, parse$m_help, 0, 2}, 
    {diag_eop_ev, 3, parse$m_help, 0, 3}, 
    {diag_start_ev, 3, parse$m_help, 0, 4}, 
    {diag_log_soft_ev, 3, parse$m_help, 0, 5}, 
    {diag_log_hard_ev, 3, parse$m_help, 0, 6}, 
    {diag_oper_ev, 3, parse$m_help, 0, 7}, 
    {diag_quick_ev, 3, parse$m_help, 0, 8}, 
    {diag_stat_ev, 3, parse$m_help, 0, 9}, 
    {diag_tt_ev, 3, parse$m_help, 0, 10}, 
    {diag_he_ev, 3, parse$m_help, 1, 11}, 
    {"", -1, 0, 0, 12}, 
    {diag_se_ev, 3, parse$m_help, 1, 13}, 
    {"", -1, 0, 0, 14}, 
    {diag_rpt_ev, 3, parse$m_help, 2, 15}, 
    {diag_grp_ev, 3, parse$m_help, 3, 16},
    {diag_pass_ev, 3, parse$m_help, 3, 17},
    {diag_runtime_ev, 3, parse$m_help, 3, 18},
    {diag_verbose_ev, 3, parse$m_help, 3, 19},
    {diag_omit_ev, 3, parse$m_help, 3, 20},
#if RAWHIDE
    {diag_iocrd_ev, 3, parse$m_help, 0, IOCRD_CODE},
#endif
    {0}
};

#if !MODULAR
static struct set_param_table nmod_onoff_table[] = {
    {diag_ev_on, 0}, 
    {diag_ev_off, 1}, 
    {0}
};
static struct set_param_table nmod_hese_table[] = {
    {diag_ev_cont, 0}, 
    {diag_ev_halt, 1}, 
    {diag_ev_loop, 2}, 
    {0}
};
static struct set_param_table nmod_rpt_table[] = {
    {diag_ev_summ, 0}, 
    {diag_ev_full, 1}, 
    {diag_ev_off, 2}, 
    {0}
};
#endif

static struct diag_ev_struct {
    char *name;
    char *string;
    long int integer;
    long int attributes;
    int *table;
} evs[] = {
#if !MODULAR
    {diag_bell_ev,	diag_ev_off, 0, EV$K_NODELETE, nmod_onoff_table},
    {diag_clean_ev, 	diag_ev_on, 0, EV$K_NODELETE, nmod_onoff_table},
    {diag_comp_ev, 	diag_ev_off, 0, EV$K_NODELETE, nmod_onoff_table},
    {diag_eop_ev, 	diag_ev_off, 0, EV$K_NODELETE, nmod_onoff_table},
    {diag_start_ev, 	diag_ev_off, 0, EV$K_NODELETE, nmod_onoff_table},
    {diag_log_soft_ev,  diag_ev_off, 0, EV$K_NODELETE, nmod_onoff_table},
    {diag_log_hard_ev, 	diag_ev_on, 0, EV$K_NODELETE, nmod_onoff_table},
    {diag_oper_ev, 	diag_ev_on, 0, EV$K_NODELETE, nmod_onoff_table},
    {diag_quick_ev, 	diag_ev_off, 0, EV$K_NODELETE, nmod_onoff_table},
    {diag_stat_ev,	diag_ev_off, 0, EV$K_NODELETE, nmod_onoff_table},
    {diag_tt_ev, 	diag_ev_off, 0, EV$K_NODELETE, nmod_onoff_table},
    {diag_he_ev, 	diag_ev_halt, 0, EV$K_NODELETE, nmod_hese_table},
    {diag_se_ev,	diag_ev_cont, 0, EV$K_NODELETE, nmod_hese_table},
    {diag_rpt_ev, 	diag_ev_full, 0, EV$K_NODELETE, nmod_rpt_table},
#endif
    {diag_grp_ev,	diag_grp_fld, 0, EV$K_NODELETE}, 
    {diag_pass_ev,	NULL, 1, EV$K_INTEGER | EV$K_NODELETE}, 
    {diag_runtime_ev,	NULL, 0, EV$K_INTEGER | EV$K_NODELETE}, 
    {diag_verbose_ev,	NULL, 0, EV$K_INTEGER | EV$K_NODELETE}, 
    {diag_omit_ev,	"", 0, EV$K_NODELETE},
    {0}
};


#if !MODULAR
/*+
 * ======================================================================
 * = non_modular_diag_write - Non Modular Diag Bit Write Routine        =
 * ======================================================================
 *
 * OVERVIEW:
 *
 *	These routines set the proper diag bit for non-modular consoles.
 *
 * RETURNS:
 *
 * ARGUMENTS:
-*/
static int non_modular_key_search (struct keycode_s_table *table, char *name)
{
int i;
struct keycode_s_table *ct;

    for (ct = table; ct->kct_string; ct++) {
	if (strcmp_nocase(ct->kct_string, name) == 0)
	    break;
    }
    if (!ct->kct_string)
	return -1;		/* not expected to get here */
    else
	return (ct->kct_code);
}
int non_modular_diag_write (char *ev_name, struct EVNODE *evnode) {

    int bit, status;
    char set_string[32];
    int code;

    if ((evnode->attributes & EV$M_TYPE) == EV$K_STRING)
	sprintf(set_string,"%s",evnode->value.string);
    else
	sprintf(set_string,"%d",evnode->value.integer);

    code = non_modular_key_search(diag_ev_table, ev_name);

    if (diag_ev_table[code].kct_misc == 0) {
	bit = non_modular_key_search(offon_table, set_string);
	diag$r_evs &= ~(1 << code);
	diag$r_evs |= bit << code;
    }
    if (diag_ev_table[code].kct_misc == 1) {
	bit = non_modular_key_search(hese_table, set_string);
	diag$r_evs &= ~(3 << code);
	diag$r_evs |= bit << code;
    }

    if (diag_ev_table[code].kct_misc == 2) {
	bit = non_modular_key_search(rpt_table, set_string);
	diag$r_evs &= ~(7 << code);
	diag$r_evs |= (bit << code);
    }

    return msg_success;
}
#endif
/*+
 * ======================================================================
 * = diag_ev_init - Diagnostic Environment Initialization Routine       =
 * ======================================================================
 *
 * OVERVIEW:
 *
 *	This routine initializes the global diagnostic environment variables. 
 *	It is called by the ev_init() routine when the kernel is initialized.
 *
 * RETURNS:
 *
 * ARGUMENTS:
-*/

int diag_ev_init(void) 
{
    int i, s;

    diag$r_evs = m_clean | m_logh | m_halt_hard | m_rpt_full;

/* Create the diag environment variables */

    for (i = 0; evs[i].name != NULL; i++) {

	if (evs[i].attributes & EV$K_INTEGER)
	    s = ev_write(evs[i].name, evs[i].integer, evs[i].attributes);
	else
	    s = ev_write(evs[i].name, evs[i].string, evs[i].attributes);
	if (s != msg_success)
	    return (s);

#if !MODULAR
	/* Installing EV's table (defined at top) */
	if (evs[i].table)
	    s = ev_install_table(evs[i].name, evs[i].table);
	if (s != msg_success)
	    return (s);

	/* Install environment variable action routine and run it as well.
	 * Instead of running the write action routine directly (which requires
	 * an EV pointer passed to it) just call EV_WRITE again which will run 
	 * the action routine for you */
	s = ev_install_action(evs[i].name, non_modular_diag_write, NULL);

	if (evs[i].attributes & EV$K_INTEGER)
	    s = ev_write(evs[i].name, evs[i].integer, evs[i].attributes);
	else
	    s = ev_write(evs[i].name, evs[i].string, evs[i].attributes);
	if (s != msg_success)
	    return (s);
#endif
    }
    return msg_success;
}

#if MODULAR
/*+
 * ======================================================================
 * = set_diag_evs - Diagnostic Environment Set Routine			=
 * ======================================================================
 *
 * OVERVIEW:
 *
 *	This routine sets the global diagnostic environment variables. 
 *
 * RETURNS:
 *
 * ARGUMENTS:
-*/

int set_diag_evs(int argc, char **argv)
{
    int status, code;

    code = parse_keycode(diag_ev_table, argv[1], argc, argv);
    if (code == -1)
	return msg_success;		/* help key */
    if (code == -2)
	return code;			/* not found */

    if (code >= 0)
	return set_diag_ev(code, argv[2], argc, argv);

    return msg_failure;
}

/*+
 * ======================================================================
 * = show diag_evs - Diagnostic Environment Show Routine		=
 * ======================================================================
 *
 * OVERVIEW:
 *
 *	This routine displays the global diagnostic environment variables. 
 *
 * RETURNS:
 *
 * ARGUMENTS:
-*/

int show_diag_evs(int argc, char **argv)
{
    int i, status, code;

    if (argc == 1) {
	for (i = 0; diag_ev_table[i].kct_string; i++) {
	    code = diag_ev_table[i].kct_code;
	    if (diag_ev_table[i].kct_abrv >= 0)
		display_diag_ev(code);
	}
	return msg_success;
    }

    code = parse_keycode(diag_ev_table, argv[1], argc, argv);
    if (code == -1)
	return msg_success;		/* help key */
    if (code == -2)
	return code;			/* not found */

    if (code >= 0)
	return display_diag_ev(code);

    return msg_failure;
}

int display_diag_ev(int code)
{
    int value, bit;
    struct EVNODE *evp, evn;

    evp = &evn;

    if (diag_ev_table[code].kct_misc == 0) {
	bit = 1 << code;
	value = (diag$r_evs & bit) >> code;
	printf("%10s = %s\n", diag_ev_table[code].kct_string,
	  offon_table[value].kct_string);
    }
    if (diag_ev_table[code].kct_misc == 1) {
	bit = 3 << code;
	value = (diag$r_evs & bit) >> code;
	printf("%10s = %s\n", diag_ev_table[code].kct_string,
	  hese_table[value].kct_string);
    }
    if (diag_ev_table[code].kct_misc == 2) {
	bit = 7 << code;
	value = (diag$r_evs & bit) >> (code);
	printf("%10s = %s\n", diag_ev_table[code].kct_string,
	  rpt_table[value].kct_string);
    }
    if (diag_ev_table[code].kct_misc == 3) {
	ev_read(diag_ev_table[code].kct_string, &evp, 0);
	if (evp->attributes & EV$K_INTEGER)
	    printf("%10s = %x\n", diag_ev_table[code].kct_string, evp->value.integer);
	else
	    printf("%10s = %s\n", diag_ev_table[code].kct_string, evp->value.string);
    }
    return msg_success;
}
/* Used to report input errors. */
void set_diag_ev_failure(int *table)
{
    struct keycode_s_table *ct;
    err_printf("bad value - valid selections:\n");
    for (ct = table; ct->kct_string; ct++) {
	 if (ct->kct_abrv >= 0)
             err_printf("        %s\n",ct->kct_string);
    }
}
int set_diag_ev(int code, char *string, int argc, char **argv)
{
    int bit, status;
    struct EVNODE *evp, evn;

    evp = &evn;

    if (diag_ev_table[code].kct_misc == 0) {
	bit = parse_keycode(offon_table, string, argc, argv);
	if (bit < 0) {
	    set_diag_ev_failure(offon_table);
	    return msg_failure;
	}
	diag$r_evs &= ~(1 << code);
	diag$r_evs |= bit << code;
#if RAWHIDE
	if (code == IOCRD_CODE)
	    cserve(CSERVE$GET_MDP_REG, bit);
#endif
    }
    if (diag_ev_table[code].kct_misc == 1) {
	bit = parse_keycode(hese_table, string, argc, argv);
	if (bit < 0) {
	    set_diag_ev_failure(hese_table);
	    return msg_failure;
	}
	diag$r_evs &= ~(3 << code);
	diag$r_evs |= bit << code;
    }

    if (diag_ev_table[code].kct_misc == 2) {
	bit = parse_keycode(rpt_table, string, argc, argv);
	if (bit < 0) {
	    set_diag_ev_failure(rpt_table);
	    return msg_failure;
	}
	diag$r_evs &= ~(7 << code);
	diag$r_evs |= (bit << code);
    }
    if (diag_ev_table[code].kct_misc == 3) {
	status = ev_read(diag_ev_table[code].kct_string, &evp, 0);
	if (status != msg_success) {
	    err_printf(msg_noev, diag_ev_table[code].kct_string);
	    return msg_failure;
	}
	if ((evp->attributes & EV$K_INTEGER) == EV$K_INTEGER)
	    status = ev_write(diag_ev_table[code].kct_string, xtoi(string), EV$K_INTEGER | EV$K_USER);
	else
	    status = ev_write(diag_ev_table[code].kct_string, string, EV$K_STRING | EV$K_USER);
	err_printf(status, diag_ev_table[code].kct_string);
	return msg_success;
    }
    return msg_success;
}
#endif
