/* File:	8530_diag_def.h
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
 * Abstract:	8530 Diagnostic Header file
 *
 * Author:	M. Robert
 *
 * Modifications:
 *
 *	mdr	22-Nov-1993	Initial Entry
 *
*/
 
/* LOCAL SYMBOLS and Macro definitions
*/

/* Z8530 Register offsets
*/

#define CHAN_A_RDWR     (UART_BASE_ADDR + 0x08)
#define CHAN_A_RXTX     (UART_BASE_ADDR + 0x0C)
#define CHAN_B_RDWR     (UART_BASE_ADDR + 0x00)
#define CHAN_B_RXTX     (UART_BASE_ADDR + 0x04)

#define CHAN_RDWR_OFF	0x04

#define Z8530_TO 10000
