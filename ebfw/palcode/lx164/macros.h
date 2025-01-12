/*
 *      VID: [2.0] PT: [Thu Aug 21 16:25:43 1997] SF: [macros.h]
 *       TI: [/sae_share/apps/bin/vice -iplatform.s -l// -p# -DEB164 -DLX164 -h -m -Xlint -XDEBUG -XPAL_K_REV -XKDEBUG -XDISABLE_CRD -XDISABLE_MACHINE_CHECKS -XSROM_SERIAL_PORT -XCONSOLE_ENTRY -alx164 ]
 */
#define	__MACROS_LOADED	    1
/*
*****************************************************************************
**                                                                          *
**  Copyright � 1993, 1994						    *
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
**	DECchip 21164 PALcode
**
**  MODULE:
**
**	macros.h
**
**  MODULE DESCRIPTION:
**
**      DECchip 21164 PALcode macro definitions
**
**  AUTHOR: ER
**
**  CREATION DATE:  29-Nov-1993
**
**  $Id: macros.h,v 1.1.1.1 1998/12/29 21:36:07 paradis Exp $
**
**  MODIFICATION HISTORY:
**
**  $Log: macros.h,v $
**  Revision 1.1.1.1  1998/12/29 21:36:07  paradis
**  Initial CVS checkin
**
**  Revision 1.6  1996/06/17  16:08:46  fdh
**  Moved the cpu specific STALL definition to the cpu specific files.
**
**  Renamed the ALIGN_BRANCH macro to ALIGN_BRANCH_TARGET to be
**  consistent with the definition used with the 2106x PALcode
**  to be used in common code shared between to two.
**
**  Revision 1.5  1994/07/08  17:03:12  samberg
**  Changes to support platform specific additions
**
**  Revision 1.4  1994/05/20  19:24:19  ericr
**  Moved STALL macro from osfpal.s to here
**  Added LDLI macro
**
**  Revision 1.3  1994/05/20  18:08:14  ericr
**  Changed line comments to C++ style comment character
**
**  Revision 1.2  1994/02/28  18:45:51  ericr
**  Fixed EGORE related bugs
**
**  Revision 1.1  1993/12/16  21:55:05  eric
**  Initial revision
**
**
**--
*/

#define	NOP \
    bis	    $31, $31, $31

/*
** Align code on an 8K byte page boundary.
*/

#define	ALIGN_PAGE \
    .align  13

/*
** Align code on a 32 byte block boundary.
*/

#define	ALIGN_BLOCK \
    .align  5

/*
** Align code on a quadword boundary.
*/

#define ALIGN_BRANCH_TARGET \
    .align  3

/*
** Hardware vectors go in .text 0 sub-segment.
*/

#define	HDW_VECTOR(offset) \
    . = offset

/*
** Privileged CALL_PAL functions are in .text 1 sub-segment.
*/

#define	CALL_PAL_PRIV(vector) \
    . = (PAL_CALL_PAL_PRIV_ENTRY+(vector<<6))

/*
** Unprivileged CALL_PAL functions are in .text 1 sub-segment,
** the privileged bit is removed from these vectors.
*/

#define CALL_PAL_UNPRIV(vector) \
    . = (PAL_CALL_PAL_UNPRIV_ENTRY+((vector&0x3F)<<6))

#define SEXT(val) ((val ^ 0x8000) - 0x8000)
#define SEXT10b(val) ((val ^ 0x200) - 0x200)

/* 
** Implements a load "immediate" longword function 
*/
#define LDLI(reg,val) \
	ldah	reg, SEXT((val+0x8000) >> 16)(zero); \
	lda	reg, SEXT(val&0xffff)(reg)

