/*
*****************************************************************************
**                                                                          *
**  Copyright © 1994							    *
**  by Digital Equipment Corporation, Maynard, Massachusetts.		    *
**                                                                          *
**  All Rights Reserved							    *
**                                                                          *
**  Permission  is  hereby  granted  to  use, copy, modify and distribute   *
**  this  software  and  its  documentation,  in  both  source  code  and   *
**  object  code  form,  and without fee, for the purpose of distribution   *
**  of this software  or  modifications  of this software within products   *
**  incorporating  an  integrated   circuit  implementing  Digital's  AXP   *
**  architecture,  regardless  of the  source of such integrated circuit,   *
**  provided that the  above copyright  notice and this permission notice   *
**  appear  in  all copies,  and  that  the  name  of  Digital  Equipment   *
**  Corporation  not  be  used  in advertising or publicity pertaining to   *
**  distribution of the  document  or  software without specific, written   *
**  prior permission.							    *
**                                                                          *
**  Digital  Equipment  Corporation   disclaims  all   warranties  and/or   *
**  guarantees  with  regard  to  this  software,  including  all implied   *
**  warranties of fitness for  a  particular purpose and merchantability,   *
**  and makes  no  representations  regarding  the use of, or the results   *
**  of the use of, the software and documentation in terms of correctness,  *
**  accuracy,  reliability,  currentness  or  otherwise;  and you rely on   *
**  the software, documentation and results solely at your own risk.	    *
**                                                                          *
**  AXP is a trademark of Digital Equipment Corporation.		    *
**                                                                          *
*****************************************************************************
**
**  FACILITY:	
**
**	DECchip 21064/21066/211164 OSF/1 PALcode
**
**  MODULE:
**
** 	sromcsrv.s
** 
**  MODULE DESCRIPTION:
** 
**	Interface to SROM Serial Port I/O routines.
** 
**  AUTHOR: Franklin Hooker
** 
**  CREATION-DATE: 13-May-1996
**
**  $Id: sromcsrv.s,v 1.1.1.1 1998/12/29 21:36:07 paradis Exp $
**
**  MODIFICATION HISTORY: 
**
**  $Log: sromcsrv.s,v $
**  Revision 1.1.1.1  1998/12/29 21:36:07  paradis
**  Initial CVS checkin
**
# Revision 1.4  1997/05/02  13:45:24  pbell
# Updated to use the cpu.h include file instead of all
# the cpu type switches.
#
# Revision 1.3  1997/02/21  04:25:35  fdh
# Modified to save line status in a global variable
# instead of a PALtemp.
#
# Revision 1.2  1996/08/22  14:07:35  fdh
# Save/restore r15 to/from pt11.
#
# Revision 1.1  1996/05/22  22:25:47  fdh
# Initial revision
#
*/

#include "macros.h"
#include "cpu.h"
#include "mini_dbg.h"

	.global Sys_Cserve_Srom_Init
	.global Sys_Cserve_Srom_Putc
	.global Sys_Cserve_Srom_Getc

	.text	3

/*
**
** FUNCTIONAL DESCRIPTION:
**
**      Initialize the SROM serial port for character I/O.
**
** INPUT PARAMETERS:
**
**      None
**
** OUTPUT PARAMETERS:
**
**	None
**
** SIDE EFFECTS:
**
*/
	ALIGN_BRANCH_TARGET

Sys_Cserve_Srom_Init:
	bsr	r21, autobaud_sync	/* do the autobaud thing	*/
	bis	r23, mc_m_echo, r23	/* Enable echo 	*/
	bis	r23, r31, r0		/* Return Line Status	*/
	hw_rei


/*
**
** FUNCTIONAL DESCRIPTION:
**
**      Output a character to the SROM serial port.
**
** INPUT PARAMETERS:
**
**      a1 (r17) - Output Character
**
** OUTPUT PARAMETERS:
**
**	None
**
** SIDE EFFECTS:
**
*/
	ALIGN_BRANCH_TARGET

Sys_Cserve_Srom_Putc:
	bis	r19, r31, r23
	bsr	r22, putChar
	hw_rei


/*
**
** FUNCTIONAL DESCRIPTION:
**
**      Get a character from the SROM serial port
**
** INPUT PARAMETERS:
**
**      None
**
** OUTPUT PARAMETERS:
**
**	v0 (r0) - Input Character
**
** SIDE EFFECTS:
**
*/
	ALIGN_BRANCH_TARGET

Sys_Cserve_Srom_Getc:
	bis	r19, r31, r23
	bsr	r22, getChar
	hw_rei
