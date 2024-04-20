/* file:	eisa.h
 *
 * Copyright (C) 1992, 1993 by
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
 * Abstract:	Eisa definitions.
 *
 * Author:	JDS - John DeNisco
 *		LKF - Lisa Frye
 *		pel - Paul E LaRochelle
 *		dwn - D.W. Neale
 *
 * Modifications:
 *
 *	jmp	11-29-93	Add Avanti support
 *
 *	jeg	12-Nov-1993	broke out platform-specific definitions.  Put
 *				them into <platform>_io.h
 *      cbf     27-aug-1993     add qaud and x for sable
 *                              other platforms: x==byte q==long
 *	jeg	16-Jul-1993	add expansion board control reg
 *      dwn     14-Jul-1993     Added Sable Support
 *      pel     08-Mar-1993     Morgan, set bit 33 not 32 for Mem space.
 *      pel     25-Feb-1993     conditionalize for morgan, jensen
 *      LKF     17-Feb-1993     Convert to Morgan specific addressing
 *	JAD	23-Sep-1992	Initial entry
 */

/*EISA bus constants*/
#define EISA_K_ID_REG  0xc80
#define EISA_K_EBC_REG 0xc84
#define EISA_K_ID_MASK 0xf0ffffff
#define EISA_K_MAX_VIR_SLOTS 16
#define EISA_K_ID_NOT_READY 0x70
#define EISA_K_ID_NOT_READY_MASK 0xf0
#define EISA_K_UNREADABLE_ID 0xff

/* Expansion Board Control Register, 0xzC84  */
#define EBC$M_ENABLE 1		/* expansion board enable */
#define EBC$M_IOCHKERR 2        /* error detected by expansion board */
#define EBC$M_IOCHKRST 4        /* expansion board is reset */
