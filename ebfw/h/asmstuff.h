#ifndef __ASMSTUFF_H_LOADED
#define __ASMSTUFF_H_LOADED
/*****************************************************************************

       Copyright © 1993, 1994 Digital Equipment Corporation,
                       Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, provided  
that the copyright notice and this permission notice appear in all copies  
of software and supporting documentation, and that the name of Digital not  
be used in advertising or publicity pertaining to distribution of the software 
without specific, written prior permission. Digital grants this permission 
provided that you prominently mark, as not part of the original, any 
modifications made to this software or documentation.

Digital Equipment Corporation disclaims all warranties and/or guarantees  
with regard to this software, including all implied warranties of fitness for 
a particular purpose and merchantability, and makes no representations 
regarding the use of, or the results of the use of, the software and 
documentation in terms of correctness, accuracy, reliability, currentness or
otherwise; and you rely on the software, documentation and results solely at 
your own risk. 

******************************************************************************/

/*
 *  $Id: asmstuff.h,v 1.1.1.1 1998/12/29 21:36:05 paradis Exp $;
 *
 * $Log: asmstuff.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:05  paradis
 * Initial CVS checkin
 *
 * Revision 1.9  1997/12/15  20:48:23  pbell
 * Updated for dp264.
 *
 * Revision 1.8  1997/06/10  18:17:31  fdh
 * Added the imb declaration.
 *
 * Revision 1.7  1997/05/01  19:47:35  pbell
 * Added SX support
 *
 * Revision 1.6  1997/03/26  15:11:39  pbell
 * Added Read/Write declarations.
 *
 * Revision 1.5  1997/02/25  16:31:34  fdh
 * Modified declaration for wrfen to include an ieee
 * emulation signature in the second argument.
 *
 * Revision 1.4  1997/02/21  04:13:14  fdh
 * Added new definitions.
 *
 * Revision 1.3  1996/05/22  21:33:25  fdh
 * Added wrmces and rdmces.
 *
 * Revision 1.2  1995/10/26  23:54:21  cruz
 * Changed argument type of wait_cycles to ul from ui.
 *
 * Revision 1.1  1995/10/05  01:02:33  fdh
 * Initial revision
 *
 */

extern void mb(void);
extern void imb(void);
extern ui rpcc(void);
extern ul getsp(void);
extern ul cServe(ul, ul, ul, ...);
extern void wrfen(ui, ul);
extern void swppal(ul, ul, ul, ul);
extern void halt(void);
extern void wait_cycles(ul cycles);
extern int swpipl(ui);
extern void wrmces(ul);
extern ui rdmces(void);

extern void CleanBCache(ul);

#ifdef _WIN32
extern ub ReadB(ul adr);
extern uw ReadW(ul adr);
extern ui ReadL(ul adr);
extern ul ReadQ(ul adr);
extern void WriteB(ul adr, ub data);
extern void WriteW(ul adr, uw data);
extern void WriteL(ul adr, ui data);
extern void WriteQ(ul adr, ul data);
extern float ReadFloat(ul adr);
extern double ReadDouble(ul adr);
extern void WriteFloat(ul adr, float data);
extern void WriteDouble(ul adr, double data);
#endif

extern ub UnalignedReadB(ub *adr);
extern uw UnalignedReadW(uw *adr);
extern ui UnalignedReadL(ui *adr);
extern ul UnalignedReadQ(ul *adr);

extern void UnalignedWriteB(ub *adr, ub data);
extern void UnalignedWriteW(uw *adr, uw data);
extern void UnalignedWriteL(ui *adr, ui data);
extern void UnalignedWriteQ(ul *adr, ul data);

extern float UnalignedReadFloat(float *adr);
extern double UnalignedReadDouble(double *adr);

extern void UnalignedWriteFloat(float *adr, ul data);
extern void UnalignedWriteDouble(double *adr, ul data);

extern void WriteFlash( ul FlashAddress, ub Value, ul CBOX_CONFIG );
extern ub   ReadFlash ( ul FlashAddress );

extern ul ReadIOQ(ul adr);
extern void WriteIOQ(ul adr, ul data);



#endif /* __ASMSTUFF_H_LOADED */
