#ifndef _common_def_
#define _common_def_

/* file:	common_def.h
 *
 * Copyright (C) 1993 by
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
 * Abstract:	C definitions for "COMMON" console modules
 *		These definition also appear in other files so make sure
 *		they stay in sync. They are here so common build procedures
 *		work.
 *
 * Author:	J. Kirchoff
 *
 */

/* External message declarations */

#define msg_success 0
#define msg_failure 1
#define msg_def 2
#define msg_halt 3
#define msg_loop 4
#define msg_error 5

/* File attributes */

#define ATTR$M_READ 1                   /* for compatability */
#define ATTR$M_WRITE 2                  /* writes (and deletes) allowed */
#define ATTR$M_EXECUTE 4                /* executable file */
#define ATTR$M_BINARY 8                 /* binary file */
#define ATTR$M_EXPAND 16                /* shell expands wildcards */
#define ATTR$M_SECURE 32                /* access allowed in secure mode */
#define ATTR$M_NOFREE 64                /* don't free file space */

/* comamnd attributes */

#define parse$m_expand	0x1
#define parse$m_basic	0x2
#define parse$m_adv	0x4
#define parse$m_help	0x8
#define parse$m_secure	0x10
#define parse$m_arc	0x20
#define parse$m_noqual	0x40
#define parse$m_diag	0x80

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

#endif

