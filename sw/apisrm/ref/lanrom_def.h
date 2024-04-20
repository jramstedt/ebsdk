/* File:	lanrom_diag_def.h
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
 * and  should  not  be  construed  as a commitment by Digital Equipment
 * Corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by Digital.
 *
 *
 * Abstract:	Sable's LAN ADDRESS ROM Diagnostic Header file
 *
 * Author:	D.W. Neale
 *
 * Modifications:
 *
 *	mdr	01-Nov-1993	Conditioned for use for Medulla platform.
 *
 *	dwn	24-June-1993	Initial Entry
*/
 
/* LOCAL SYMBOLS and Macro definitions
*/


/* LAN ADDRESS ROM offsets
*/
#if MEDULLA                     /*  lw aligned accesses  */
#define LANROM_OffBgn	  0x00	/* Beginning byte offset */
#define LANROM_OffEnd	  0x7C	/* Ending    byte offset */

#else
#define LANROM_OffBgn	  0x500	/* Beginning byte offset */
#define LANROM_OffEnd	  0x51F	/* Ending    byte offset */

#define LANROM_MirrorUpBgn  15	/* Mirror SA[0:5] copy UP  starting byte */
#define LANROM_MirrorDwnBgn 16	/* Mirror SA[0:5] copy DWN starting byte */
#define LANROM_FillerBgn    24	/* 0xff0055aa beginning byte */

#endif

