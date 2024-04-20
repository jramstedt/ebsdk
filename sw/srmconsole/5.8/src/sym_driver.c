/* file:	sym_driver.c
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
 * Abstract:	Symbol table driver.  This driver handles all of the
 *		firmware's internal symbols.  It keeps one large symbol
 *		table for all global symbols in the console.  Additionally,
 *		it keeps track of module information, which is used by
 *		quilt.
 *
 *		Enumerating the table may be done by using wildcards,
 *		such as "ls sym:*".
 *
 * Author:	AJ Beaverson
 *
 * Modifications:
 *
 */

#include	"cp$src:platform.h"
#include   	"cp$src:kernel_def.h"
#include	"cp$src:common.h"
#include "cp$inc:prototypes.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:sym_def.h"
#include	"cp$src:regexp_def.h"
#include	"cp$src:ovly_def.h"

#if !SYMBOLS

int sym_init (void)
{
    return msg_success;
}

int sym_lookup (char *s, int *value)
{
    return msg_failure;
}

int sym_table_lookup(int address)
{
    return 0;
}

int sym_expand(char *name, char *info, char *next, int longformat,
  int (*callback)(), int p0, int p1)
{
    return 0;
}
#endif

#if SYMBOLS

#if MODULAR
extern struct master_sym_s_table *sym_table_ptr[];
extern int sym_table_max_val;
extern int load_symbols_flag;
extern int ovly_cpu;
#endif

#if !MODULAR
#if PINNACLE*0
extern struct SYM symlist [];
# define vlist symlist
#else
extern struct SYM vlist [];
#endif
#endif

extern int null_procedure ();
int sym_read ();
int sym_write ();
int sym_open ();
int sym_expand ();

struct DDB sym_ddb = {
	"sym",			/* how this routine wants to be called	*/
	sym_read,		/* read routine				*/
	sym_write,		/* write routine			*/
	sym_open,		/* open routine				*/
	null_procedure,		/* close routine			*/
	sym_expand,		/* name expansion routine		*/
	null_procedure,		/* delete routine			*/
	null_procedure,		/* create routine			*/
	null_procedure,		/* setmode				*/
	null_procedure,		/* validation routine			*/
	0,			/* class specific use			*/
	1,			/* allows information			*/
	0,			/* must be stacked			*/
	0,			/* is a flash update driver		*/
	0,			/* is a block device			*/
	0,			/* not seekable				*/
	0,			/* is an ethernet device		*/
	0,			/* is a filesystem driver		*/
};

/*+
 * ============================================================================
 * = sym_open - open the symbol driver                                        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Open the symbol driver for access.  The info field specified which symbol
 * is to be accessed, and the inode implicitly defines the name space.
 *
 * FORM OF CALL:
 *  
 *	sym_open (fp, info, next, mode)
 *  
 * RETURNS:
 *
 *	msg_success
 *       
 * ARGUMENTS:
 *
 * 	struct FILE *fp - address of file descriptor
 *	char *info	- information that follows the colon.  Use this
 *			field as the prefered block size.
 *	char *next	- information that follows the slash.  This is the
 *			remaining portion of the protocol tower.  The buffer
 *			driver must always be stacked.
 *	char *mode	- ignored.
-*/
int sym_open (struct FILE *fp, char *info, char *next, char *mode) {

	/*
	 * The info field must be present.
	 */
	if (!info || !*info) {
	    err_printf ("sym: info field must be present\n");
	    return msg_failure;
	}
	return sym_lookup (info, fp->offset);
}

/* 
 * Look up the symbol.  An exact match, or one and only one proper
 * substring will do.
 */
#if MODULAR

int sym_lookup(char *s, int *value)
{
    int i;
    int id = whoami();
    int matches;
    struct SYM *sp;

    if (load_symbols_flag == 0) {
	if (mfpr_ipl() == IPL_RUN) {
		ovly_symbol_load();
	} else {
	    if ((ovly_cpu / 2) == (id / 2))
		ovly_symbol_load();
	}
    }
    matches = 0;

    for (i = 0; i < sym_table_max_val; i++) {

	if ((*sym_table_ptr)[i].symptr != 0) {

/* Go through the symbol list and look for the value */

	    for (sp = (*sym_table_ptr)[i].symptr;; sp++) {

		if (sp->name == 0)
		    break;

		switch (substr_nocase(s, sp->name)) {

		    case 1: 		/* exact match */
			*value = sp->value;
			return msg_success;

		    case 2: 		/* ambiguous */
			*value = sp->value;
			matches++;
			break;
		}
	    }
	}
    }

/* If there was only one ambiguous match, then use it */

    if (matches == 1)
	return msg_success;
    else
	return msg_failure;
}
#endif
#if !MODULAR

int sym_lookup(char *s, int *value)
{
    int i;
    int matches;
    struct SYM *sp;

    matches = 0;

    sp = &vlist[0];

    while (sp->name) {

	switch (substr_nocase(s, sp->name)) {

	    case 1: 			/* exact match */
		*value = sp->value;
		return msg_success;

	    case 2: 			/* ambiguous */
		*value = sp->value;
		matches++;
		break;
	}
	sp++;
    }

/* If there was only one ambiguous match, then use it */

    if (matches == 1)
	return msg_success;
    else
	return msg_failure;
}
#endif

#if MODULAR
/*+
 * ============================================================================
 * = sym_table_lookup - find symbol associated with address                   =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Find the symbol associated with the address given in the symbol tables.
 * 
 * FORM OF CALL:
 *  
 *  sym_table_lookup (address)
 *  
 * RETURN CODES:
 *
 *	0 - symbol not found
 *	adr - address of found symbol name string
 *       
 * ARGUMENTS:
 *
 *  int address - symbol desired.
 *
 * SIDE EFFECTS:
 *
-*/

int sym_table_lookup(int address)
{
    int i;
    int id = whoami();
    struct sym_s_table *symp;

    if (load_symbols_flag == 0) {
	if (mfpr_ipl() == IPL_RUN) {
		ovly_symbol_load();
	} else {
	    if ((ovly_cpu / 2) == (id / 2))
		ovly_symbol_load();
	}
    }

    for (i = 0; i < sym_table_max_val; i++) {

	if ((*sym_table_ptr)[i].symptr != 0) {

/* Go through the symbol list and look for the value */

	    for (symp = (*sym_table_ptr)[i].symptr;; symp++) {

		if (symp->name == 0)
		    break;

/*If we found the address, return the symbol */

		if (symp->value == address) {
		    return symp->name;
		    break;
		}
	    }
	}
    }
    return 0;
}
#endif

#if !MODULAR

int sym_table_lookup(int address)
{
    int i;
    struct SYM *sp;

    for (sp = &vlist[0];; sp++) {

	if (sp->name == 0)
	    break;

/*If we found the address, return the symbol */

	if (sp->value == address) {
	    return sp->name;
	    break;
	}
    }
    return 0;
}
#endif

/*+
 * ============================================================================
 * = sym_read - read from a symbol in the table				      =
 * ============================================================================
 *
 * OVERVIEW:
 *	Read the value of a symbol in the table.
 *
 *
 *  FORMAL PARAMETERS:
 *
 *      struct FILE *fp	Pointer to FILE structure.
 *	int size	size of item in bytes
 *	int number 	number of items to read
 *	char *buf	Address to write the data to.
 *
 *  RETURN VALUE:
 *
 *	Number of bytes read, or 0 on error.
 *
 *  IMPLICIT INPUTS:
 *
 *      NONE
 *
 *  IMPLICIT OUTPUTS:
 *
 *      NONE
 *
 *--
 */
int sym_read (struct FILE *fp, int size, int number, char *buf) {
	unsigned int len;

	len = size*number;
	memcpy (buf, *fp->offset, len);
	*fp->offset += len;
	return len;
}


/*+
 * ============================================================================
 * = sym_write - write a block of data to the underlying symbol tower        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *  FORMAL PARAMETERS:
 *
 *      struct FILE *fp	Pointer to FILE structure.
 *	int size	size of item in bytes
 *	int number 	number of items to write
 *	char *buf	Address to read the data from.
 *
 *  RETURN VALUE:
 *
 *	Number of bytes written, or 0 on error.
 *
 *  IMPLICIT INPUTS:
 *
 *      NONE
 *
 *  IMPLICIT OUTPUTS:
 *
 *      NONE
 *
 *  SIDE EFFECTS:
 *
 *      NONE
 *
 *--
 */
int sym_write (struct FILE *fp, int size, int number, char *buf) {
	unsigned int len;

	len = size*number;
	memcpy (*fp->offset, buf, len);
	*fp->offset += len;
	return len;
}

/*
 * Expand a regular expression over the symbol table.
 */
int sym_expand(char *name, char *info, char *next, int longformat,
  int (*callback)(), int p0, int p1)
{
    int i;
    int id = whoami();
    struct sym_s_table *symp;
    struct SYM *sp;
    struct REGEXP *re;
    char buf[64];

#if MODULAR
    if (load_symbols_flag == 0) {
	if (mfpr_ipl() == IPL_RUN) {
		ovly_symbol_load();
	} else {
	    if ((ovly_cpu / 2) == (id / 2))
		ovly_symbol_load();
	}
    }
#endif

    re = regexp_shell(info, 1);
    if (re == 0) {
	err_printf("sym: poorly formed regular expression\n");
	return msg_failure;
    }

#if MODULAR

    for (i = 0; i < sym_table_max_val; i++) {

	if ((*sym_table_ptr)[i].symptr != 0) {

/* Go through the symbol list and look for the value */

	    for (symp = (*sym_table_ptr)[i].symptr;; symp++) {

		if (killpending())
		    return msg_ctrlc;

		if (symp->name == 0)
		    break;

		if (regexp_match(re, symp->name)) {

		    if (longformat)
			sprintf (buf, "%08X    %s", symp->value, symp->name);
		    else
			sprintf (buf, "%s", symp->name);

		    (*callback)(buf, p0, p1);
		}
	    }
	}
    }
#endif
#if !MODULAR
    sp = &vlist[0];
    while (sp->name) {
	if (killpending())
	    return msg_ctrlc;
	if (regexp_match(re, sp->name)) {
	    if (longformat) {
		sprintf(buf, "%08X    %s", sp->value, sp->name);
	    } else {
		sprintf(buf, "%s", sp->name);
	    }
	    (*callback)(buf, p0, p1);
	}
	sp++;
    }
#endif
    regexp_release(re);
    return msg_success;
}

/*+
 * ============================================================================
 * = sym_init - initialize the symbol driver                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine initializes the SYMBOL driver entry in the global DDB
 *	(Driver DataBase) and create an inode for the procedure, variable
 *	and module list respectively.  Callers use whatever name space is
 *	appropriate.
 *
 *
 *  FORMAL PARAMETERS:
 *
 *      NONE
 *
 *  RETURN VALUE:
 *
 *	Always returns msg_success.
 *
 *  IMPLICIT INPUTS:
 *
 *      NONE
 *
 *  IMPLICIT OUTPUTS:
 *
 *      NONE
 *
 *
 *--
 */

int sym_init (void) {
	struct INODE *ip;
	
	/* Create an inode entry, thus making the device visible as a file: */
	allocinode (sym_ddb.name, 1, &ip);
	ip->dva = & sym_ddb;
	ip->attr = ATTR$M_READ | ATTR$M_WRITE;
	INODE_UNLOCK (ip);

	return msg_success;
}
#endif

