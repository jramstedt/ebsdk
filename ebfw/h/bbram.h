#ifndef __BBRAM_H_LOADED
#define __BBRAM_H_LOADED
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
 *  $Id: bbram.h,v 1.1.1.1 1998/12/29 21:36:05 paradis Exp $;
 */

/*
 * $Log: bbram.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:05  paradis
 * Initial CVS checkin
 *
 * Revision 1.7  1997/06/30  14:08:57  pbell
 * Moved the location of objects in the toy nvram.
 *
 * Revision 1.6  1995/02/10  02:20:00  fdh
 * Added definitions for OSTYPE location in battary backed RAM.
 *
 * Revision 1.5  1994/08/05  20:13:47  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.4  1994/07/11  19:46:38  fdh
 * Remove typecast used on the value set by the BBRAM_READ macro.
 *
 * Revision 1.3  1994/06/22  15:10:20  rusling
 * Fixed up WNT compile warnings.
 *
 * Revision 1.2  1994/06/20  14:18:59  fdh
 * Fixup header file preprocessor #include conditionals.
 *
 * Revision 1.1  1994/03/16  00:13:48  fdh
 * Initial revision
 *
 */


/*
 * The TOY clock contains some BBRAM that we can use to
 * store useful stuff.  The BBRAM that is available to us
 * is in bytes 14-127.
 *
 * Currently, we use:
 *
 * Bytes	Contents
 *
 * 14:18	bootadr + pattern
 * 21:22	rmode + pattern
 * 23:30 	default mac address + 2 bytes of checksum.
 * 
 * Below some macros and offsets are defined to make programming
 * this area easier.
 */

extern int rtcBase;

#define BBRAM_READ(offset,value)	\
	{				\
	outportb(rtcBase, (offset));		\
	(value) = inportb((ui)rtcBase + 1) & 0xFF;	\
	}

#define BBRAM_WRITE(offset,value)	\
	{				\
	outportb(rtcBase, (offset));		\
	outportb(rtcBase + 1, (value) & 0xFF);			\
	}

#define BBRAM_BOOTADR			0x18
#define BBRAM_BOOTADR_COOKIE( VALUE )	((ub)( ~( (VALUE) +		\
						  ((VALUE)>>8) +	\
					          ((VALUE)>>16) +	\
						  ((VALUE)>>24)   )))
#define BBRAM_RMODE			0x1d
#define BBRAM_RMODE_COOKIE( VALUE )	((ub)( ~(VALUE) ))
#define BBRAM_MACADDR			0x1f
#define BBRAM_BOOTADR_COOKIE( VALUE )	((ub)( ~( (VALUE) +		\
						  ((VALUE)>>8) +	\
					          ((VALUE)>>16) +	\
						  ((VALUE)>>24)   )))
#define BBRAM_MACADDR_COOKIE( VALUE )	((ub)( ~( (VALUE)[0] +		\
						  (VALUE)[1] +		\
						  (VALUE)[2] +		\
						  (VALUE)[3] +		\
						  (VALUE)[4] +		\
						  (VALUE)[5] + )))
#define BBRAM_OSTYPE			0x3f

#endif /* __BBRAM_H_LOADED */
