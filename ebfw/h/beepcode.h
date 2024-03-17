#ifndef __BEEPCODE_H_LOADED
#define __BEEPCODE_H_LOADED
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
 *  $Id: beepcode.h,v 1.1.1.1 1998/12/29 21:36:05 paradis Exp $;
 */

/*
 * $Log: beepcode.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:05  paradis
 * Initial CVS checkin
 *
 * Revision 1.2  1996/08/16  17:44:48  fdh
 * Added the following definitions...
 *  beep_k_graphics_failed   333    Graphics initialization failed.
 *  beep_k_init_done         1      Graphics initialization failed.
 *
 * Revision 1.1  1996/02/21  21:44:11  fdh
 * Initial revision
 *
 */

#define beep_k_load_fwupdate     123    /* Loading firmware update tool from floppy. */
#define beep_k_graphics_failed   333    /* Graphics initialization failed. */
#define beep_k_init_done         1      /* Graphics initialization failed. */

#endif /* __BEEPCODE_H_LOADED */
