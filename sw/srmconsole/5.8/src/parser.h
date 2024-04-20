/*
 * parser.h 	J. Kirchoff
 */

/*
Facility:

	LASER Console, Diagnostics and System

Abstract:

	This module declares the Parser definitions and constants.

Environment:

	Console Mode, Privileged

Author:

	LASER Project Team

Creation Date:

	July 1991

Modification Date:

	 3-Apr-93	jrk	add overlays

	18-Jun-92	jrk	add help to cmd tables

	 4-Jun-92	jrk	add modes to cmd tables

	 3-Dec-91	jrk	expand cmd table

	22-Nov-91	jrk	change help from ? to ^h
*/
 
#ifndef parser_loaded
#define parser_loaded

#define cmd_table_max	16	/* allow for 16 entries for now */

/*
 * keyword command table
 *	these tables must be of identical length
 *	with the first four entries being of the same type
 *	 note:
 *		abrv = 0, no abbreviation allowed
 *		     = -1, don't include in help
 */

/*
 * cmd_s_table -- top level command table
 */
struct cmd_s_table {
	char *ct_string;		/* command name */
	char ct_abrv;			/* minimum abreviation */
	char ct_flags;			/* command flags */
	short ct_misc;			/* miscellaneous */
	int (*ct_routine)();		/* implementing routine */
	char *ct_usage;			/* usage syntax */
};

/*
 * cmdlist_s_table -- command to overlay table
 */
struct cmdlist_s_table {
	char *cl_string;		/* command name */
	char cl_abrv;			/* minimum abreviation */
	char cl_flags;			/* command flags */
	short cl_misc;			/* miscellaneous */
	char **cl_ovly;			/* associated overlay */
	char *cl_usage;			/* usage syntax */
};

/*
 * key_s_table -- keyword table
 */
struct key_s_table {
	char *kt_string;		/* keyword */
	char kt_abrv;			/* minimum abreviation */
	char kt_flags;			/* command flags */
	short kt_misc;			/* miscellaneous */
	int (*kt_routine)();		/* implementing routine */
	int kt_unused;
};

/*
 * keycode_s_table -- keyword code table
 */
struct keycode_s_table {
	char *kct_string;		/* keyword */
	char kct_abrv;			/* minimum abreviation */
	char kct_flags;			/* command flags */
	short kct_misc;			/* miscellaneous */
	int kct_code;			/* keyword code */
	int kct_unused;
};

/*
 * code_s_table -- standard code/string table
 */
struct code_s_table {
	int t_code;			/* code */
	char *t_string;			/* string */
};

/*
 * buffer sizes
 */
#define parser_linesize	80		/* input buffer size */
#define	parser_keysize	50		/* keyword buffer size */

/* Control keys */

#define HELP_KEY	0x8		/* ^h */

/* Flags */

#define parse$m_expand	0x1
#define parse$m_basic	0x2
#define parse$m_adv	0x4
#define parse$m_help	0x8
#define parse$m_secure	0x10
#define parse$m_arc	0x20
#define parse$m_noqual	0x40
#define parse$m_diag	0x80

#endif		/* parser_loaded */
