/*
 *      VID: [2.0] PT: [Thu Aug 21 16:25:38 1997] SF: [cserve.h]
 *       TI: [/sae_share/apps/bin/vice -iplatform.s -l// -p# -DEB164 -DSX164 -DDC21164PC -h -m -Xlint -XDEBUG -XPAL_K_REV -XKDEBUG -XDISABLE_CRD -XDISABLE_MACHINE_CHECKS -XSROM_SERIAL_PORT -XCONSOLE_ENTRY -asx164 ]
 */
#define	__CSERVE_LOADED	1
/*
*****************************************************************************
**                                                                          *
**  Copyright © 1993, 1994	       					    *
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
**	DECchip 21164 OSF/1 PALcode
**
**  MODULE:
**
**	cserve.h
**
**  MODULE DESCRIPTION:
**
**      Platform specific cserve definitions.
**
**  AUTHOR: ES
**
**  CREATION DATE:  21-JUN-1994
**
**  $Id: cserve.h,v 1.1 1999/06/30 20:37:54 stig Exp $
**
**  MODIFICATION HISTORY:
**
**  $Log: cserve.h,v $
**  Revision 1.1  1999/06/30 20:37:54  stig
**  Reorganisation for common PALcode source for API platforms
**
**  Revision 1.1  1999/06/21 19:00:45  stig
**  reorganisation of PALcode structure
**
**  Revision 1.3  1999/03/19 20:32:52  stig
**  Nautilus OSF PALcode, latest edition...
**
**  Revision 1.1.1.1  1998/12/29 21:36:09  paradis
**  Initial CVS checkin
**
**  Revision 1.8  1997/08/18  18:30:29  fdh
**  Merged in dc21164pc support.
**
**  Revision 1.7  1996/06/17 16:05:00  fdh
**  Added definitions to support the SROM Serial
**  port cserve functions.
**
**  Revision 1.6  1995/04/03  17:29:52  samberg
**  Add rd_bccfg_off
**
**  Revision 1.5  1995/02/02  19:31:34  samberg
**  Added WR_BCACHE, deleted WR_BCCFG and WR_BCCTL
**
**  Revision 1.4  1994/12/08  17:13:34  samberg
**  Add CSERVE_K_WR_BCCTL and CSERVE_K_WR_BCCFG
**
**  Revision 1.3  1994/11/30  15:59:30  samberg
**  Use c-style comments for c compiler use
**
**  Revision 1.2  1994/11/22  19:02:46  samberg
**  Add constants for ev4 backward compatibility
**
**  Revision 1.2  1994/11/22  19:02:46  samberg
**  Add constants for ev4 backward compatibility
**
**  Revision 1.1  1994/07/08  17:01:40  samberg
**  Initial revision
**
**
*/

/*
** Console Service (cserve) sub-function codes:
*/
#define CSERVE_K_LDQP           0x01
#define CSERVE_K_STQP           0x02
#define CSERVE_K_JTOPAL         0x09
#define CSERVE_K_WR_INT         0x0A
#define CSERVE_K_RD_IMPURE      0x0B
#define CSERVE_K_PUTC           0x0F
#define CSERVE_K_WR_ICSR	0x10
#define CSERVE_K_WR_ICCSR	0x10    /* for ev4 backwards compatibility */
#define CSERVE_K_RD_ICSR	0x11
#define CSERVE_K_RD_ICCSR	0x11    /* for ev4 backwards compatibility */
#define CSERVE_K_RD_BCCFG2	0x12
#define CSERVE_K_RD_BCCFG	0x13

#define CSERVE_K_WR_BCACHE      0x16

#define CSERVE_K_RD_BCCFG_OFF   0x17

#define CSERVE_K_SROM_INIT      0x18
#define CSERVE_K_SROM_PUTC      0x19
#define CSERVE_K_SROM_GETC      0x20

#define CSERVE_K_RD_MAF_MODE	0x21
#define CSERVE_K_WR_MAF_MODE	0x22
