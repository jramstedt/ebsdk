/*
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
 * and  should  not  be  construed  as a commitment by Digital Equipment
 * Corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by Digital.
 *
 *
 * Abstract:	VGA port block Definitions
 *
 * Author:	John A. DeNisco
 *
 * Modifications:
 *
 *	jad	22-Sep-1993	Initial entry.
 */

/* Port Block								    */

volatile struct VGA_PB {
	struct pb pb;
	struct TTPB *ttpb;		/* Back pointer to the ttpb	    */
	struct CH_STATE *chs;           /* Pointer to the state structure   */
	unsigned int vcard;		/* video card type 		    */
};
