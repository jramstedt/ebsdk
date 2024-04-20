/* File:	fddi_pb_def.h
 *
 * Copyright (C) 1994 by
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
 * Abstract:	FDDI port block Definitions
 *
 * Author:	John R. Kirchoff
 *
 * Modifications:
 *
 *	jrk	26-Oct-1994	Initial entry.
 */
 
/* port block structure */
/*
 * This port block only defines the beginning of the structure,
 * The first 4 items must be in order for all fddi device port blocks.
 */

volatile struct FDDI_PB {
    struct pb	pb;
    int		(*lrp)();		/* Pointer to the datalink rcv */
    int		(*lwp)();		/* Pointer to send routine */
    int		*saved_first_segment;	/* Saved first segment */
};
