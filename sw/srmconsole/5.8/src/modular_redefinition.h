/* file:	modular_redefinition.h
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
 * Abstract:	Console Overlay call between redefinitions
 *
 * Author:	J. Kirchoff
 *
 * Modifications:
 *
 *	jrk	13-Jul-1995	Initial entry.
 *
 */

/*
 * The following is redefined to change from default passed pararmeters
 * to 64 bit data for arguments d, e and f.
 * The shared_adr assignment must match the order in the ovly_rtn_table
 * in 'patform'_diagsupport_ovly.c.
 */

#undef report_failure_quad

#define report_failure_quad(a,b,c,d,e,f) \
    (*(diagsupport_shared_adr[0]))(a,b,c,(unsigned __int64)(d),(unsigned __int64)(e),(unsigned __int64)(f))

