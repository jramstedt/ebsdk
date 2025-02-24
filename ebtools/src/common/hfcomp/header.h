#ifndef __HEADER_H_LOADED
#define __HEADER_H_LOADED
/*****************************************************************************

       Copyright � 1993, 1994 Digital Equipment Corporation,
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
 *  $Id: header.h,v 1.1.1.1 1998/12/29 21:36:25 paradis Exp $;
 *
 * $Log: header.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:25  paradis
 * Initial CVS checkin
 *
 * Revision 1.4  1996/12/22  12:31:13  fdh
 * Modified to use a different header signature.  The old
 * header signature is also present for backward compatibility
 * with the old header.  The new signature indicates the
 * presence of additional header fields.
 *
 * Revision 1.3  1996/07/23  22:43:01  fdh
 * Conditionalized for _WIN32 because we can't use the
 * linker defined _end symbol.
 *
 * Revision 1.2  1996/06/13  03:14:32  fdh
 * Modified to use the linker defined variable _end instead
 * of the harwired precessor definition.
 *
 * Revision 1.1  1995/12/07  07:15:50  fdh
 * Initial revision
 *
 */

/*
**	DECOMP_PALBASE is where this code is expected to live to
**	allow the DECOMP_BASE location to fall into the proper
**	place.  Once this code is running in PALmode the PALbase
**	is updated with this value to insure that the DTB miss
**	entry points are available.
*/
#ifndef DECOMP_PALBASE
#define DECOMP_PALBASE 0x300000
#endif

/*
**	DECOMP_BASE must agree with the address that the
**	decompressor code is linked to.  This is an external
**	linkage that can't be resolved automagically with the
**	current toolset.
*/
#ifndef DECOMP_BASE
#define DECOMP_BASE 0x300B00
#endif

#ifdef _WIN32
/*
**	DECOMP_COMPRESSED is the address where the compressed image
**	is located.  This also points to the first element (ID stamp)
**	in the header.
*/
#ifndef DECOMP_COMPRESSED
#define DECOMP_COMPRESSED 0x304000
#endif
#endif

/* Define a signature stamp "WimC" */
#define SIGNATURE_STAMP 0x436D6957U

/* Define a different signature stamp "FdhJ" */
#define SIGNATURE_STAMP_REV2 0x4A686446U

#ifndef ASSEMBLY_CODE
/*
 * The following header is prepended to every compressed image
 */
struct header {
  unsigned int ID_rev2;		/* Revision 2 Header validation signature */
  unsigned int dsize;		/* size of decompressed image in bytes */
  unsigned int stack;		/* stack size to use */
  unsigned int target;		/* decompress to this address */
  unsigned int ID_stamp;	/* Header validation signature stamp */
  unsigned int csize;		/* size of compressed image in bytes */
};
#endif

/*
 * These offsets must point to the elements
 * in the header C data structure.
 */
#define HEAD_ID_rev2  0x00
#define HEAD_dsize    0x04
#define HEAD_stack    0x08
#define HEAD_target   0x0c
#define HEAD_ID_stamp 0x10
#define HEAD_csize    0x14

#define SEXT(val) ((val ^ 0x8000) - 0x8000)

#endif /* __HEADER_H_LOADED */
