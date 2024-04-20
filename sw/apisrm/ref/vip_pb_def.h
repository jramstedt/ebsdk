/*
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
 * and  should  not  be  construed  as a commitment by Digital Equipment
 * Corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by Digital.
 *
 *
 * Abstract:	VIP port block Definitions
 *
 * Author:	Joe Smyth
 *
 * Modifications:
 *
 *	joes	07-Feb-1994	Initial entry.
 */
 

/* VIP port block structure*/
volatile struct VIP_PB
{
    struct pb pb;                       /*System port block             */
    unsigned long int csr;
    unsigned long int sgram;
    unsigned long int vmewin64;
    unsigned long int vmewin512;   
};
