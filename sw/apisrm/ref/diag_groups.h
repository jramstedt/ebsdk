/*
 * file:	diag_groups.h
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
 *
 *
 * Abstract:	Definitions of all diagnostic groups.
 *
 * Author:	Mark Cacciapouti (mrc)
 *
 * Modifications:
 *
 *	twp 	07-Jan-1992	Added new group for dvt tests -- needed for
 *				CB_PROTO, ADU, and PELE builds (must have 
 *				been added to Cobra local CMS library but
 *				never merged)  MEMTEST ADU builds won't work
 *				without it.
 *
 *	twp 	06-May-1991	Added new group for test that causes the
 *				unexpected exception
 *
 *	twp	07-Mar-1991	Changed variable group names to constants
 *				Will now need to include this file in the
 *				diagnostics along with diag_def.h
 *
 *	mrc	14-Feb-1991	Removed conditional compiles and externs.
 *
 *	mrc	07-Feb-1991	Added conditional compiles.
 *
 *	mrc	05-Feb-1991	Initial entry.
 *
 */

/*
 * Diagnostic Groups.
 *
 * This header file describes all diagnostic groups used by all diagnostics.
 * To add a new group, three things must be done:
 *
 *	1. Define a new mask for the group in the form 'm_grp_xxx' where
 *	   'xxx' is a mnemonic describing the group name.  The mask value
 *	   should simply be a left shift of the last defined mask.
 *
 *	2. Define the string that is the group name.  The string variable
 *	   name should be of the form 'diag_grp_xxx' where 'xxx' is a mnemonic
 *	   describing the group name.
 *
 *	3. Add the group name/mask pair to the diag_groups structure in
 *	   diag_support.c.
 *
 */

/*------------------------------------------------------------------------------
	Group masks.  Define the new group mask here (1).
------------------------------------------------------------------------------*/

#define	m_grp_fld	0x00000001
#define	m_grp_mfg	0x00000002
#define	m_grp_exc	0x00000004
#define	m_grp_dvt	0x00000008
#define	m_grp_pwr	0x00000010
#define	m_grp_boo	0x00000020


/*------------------------------------------------------------------------------
	Group names.  Add the new group name here (2).
------------------------------------------------------------------------------*/

#define diag_grp_fld 	"field"
#define diag_grp_mfg	"mfg"
#define diag_grp_exc	"exception"
/* Added for CB_PROTO, ADU and PELE builds */
#define diag_grp_dvt	"dvt"
#define diag_grp_pwr	"powerup"
#define diag_grp_boot	"boot"

/*------------------------------------------------------------------------------
	Group structure.  Add the group name/mask to structure in 
        diag_support.c (3).
------------------------------------------------------------------------------*/

typedef struct 
{
	char			*grp_name;
	unsigned long int	grp_mask;
} DIAG_GROUP;

