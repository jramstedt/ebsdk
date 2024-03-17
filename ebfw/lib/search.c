
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

#ifndef LINT
static char *rcsid = "$Id: search.c,v 1.1.1.1 1998/12/29 21:36:11 paradis Exp $";
#endif

/*
 * $Log: search.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:11  paradis
 * Initial CVS checkin
 *
 * Revision 1.17  1997/05/01  20:56:55  pbell
 * Cleaned up warnings on ReadB and ReadW usage.
 *
 * Revision 1.16  1996/05/26  02:11:02  fdh
 * Removed junk character.
 *
 * Revision 1.15  1995/12/12  00:20:33  cruz
 * Fixed NT warning.
 *
 * Revision 1.14  1995/12/07  16:38:48  cruz
 * Forgot to print 64-bit values with leading zeroes.
 *
 * Revision 1.13  1995/12/07  16:37:21  cruz
 * Improved the printing of a matched value.
 *
 * Revision 1.12  1995/12/07  16:18:47  cruz
 * Print addresses at 64-bit values.
 * Rewrote the match code to save space by getting rid of common
 * code.
 *
 * Revision 1.11  1995/12/07  15:58:38  cruz
 * Print value and mask as 64-bit variables.
 * Removed masking of memval in print statements since they are
 * no longer needed.
 * Print match value for "sq" as a 64-bit value.
 *
 * Revision 1.9  1995/10/26  23:47:38  cruz
 * Added casting.
 *
 * Revision 1.8  1995/09/02  02:56:16  fdh
 * Replaced call to IsDigit with isdigit.
 *
 * Revision 1.7  1995/05/24  18:37:48  cruz
 * Fixed bug in search routine which caused it to pause for
 * user input following each unit search IF and only IF the
 * number of matches so far was a multiple of 20.
 *
 * Revision 1.6  1995/03/06  05:23:31  fdh
 * Corrected data types to prevent sign extensions
 * from printing out inadvertenly.
 *
 * Revision 1.5  1994/08/08  00:33:23  fdh
 * Added call to kbdcontinue().
 *
 * Revision 1.4  1994/08/05  20:16:23  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.3  1994/06/17  19:35:37  fdh
 * Clean-up...
 *
 * Revision 1.2  1994/01/19  10:40:08  rusling
 * Ported to Alpha Windows NT.
 *
 * Revision 1.1  1993/06/08  19:56:25  fdh
 * Initial revision
 *
 */


#include "system.h"
#include "lib.h"
#include "console.h"

/* simple pattern searcher for the rom */

/* jn  20-Nov-1991 */


/* search searches through a memory range delineated by first and last
 *  for valstr
 *
 *
 *   first  - beginning of memory range to search
 *   last   - end of memory range to search
 *   size   - size of items to search, byte, short, longword, quadword
 *            3 - quadword    (numbers used to match other places in rom)
 *            2 - longword
 *            1 - short
 *            0 - byte
 *   valstr - string which is value to search has following format
 *
 *		[*..][#..][*..]   
 *		   	where # is a hex digit and * represents a
 *                              a wildcard to match any hex digit in that
 *                              position
 *        eg:
 *		84   - match number 84
 *              84*  - match 840, 841 ... 84f
 *              84** - match 8400, 8401 ... 84ff
 *              *84  - match 084, 184 ... f84
 *              **84 - match 0084, 0184 ... ff84
 *              *84* - match 0840, 0841, ... f84f
 *              etc.
 *
 *   inverse - true value indicates to find all values that don't match
 *               the value string, while a false value indicates a request
 *		 to find matching values
 */
search( ul first, ul last, int size, char *valstr, int inverse )
{
   char *fmt[] = {"%02x\n", "%04x\n", "%08x\n", "%016lx\n"};
   ul i, addr;
   ul val, mask;
   ul memval;
   int keycnt = 0;
   int found = 0;
   void ParseVal( char *, ul *, ul *, int );

#define MATCH( x )   ( ( (( ((x)&mask) == val) && !inverse) || \
		         (( ((x)&mask) != val) && inverse) ) ? 1 : 0 )

   ParseVal( valstr, &val, &mask, size );
   val &= mask;
   printf( "val = %lx  mask = %lx\n", val, mask );

   first &= (ul)(-1<<size);   /* ensure proper alignment */

   i = first;
   while( i <= last ){
       addr = i;
       switch (size ) {
       case 0:
           memval = ReadB( i );
           i += 1;
           break;
       case 1:
           memval = ReadW( i );
           i += 2;
           break;
       case 2:
           memval = ReadL( i );
           i += 4;
           break;
       case 3:
           memval = ReadQ( i );
           i += 8;
           break;
       }
       
       if(MATCH(memval)) {
           printf( "occurrence at %lx ", addr);
           printf(fmt[size], memval); 
           found++; 
           keycnt++;
       }

       if (keycnt == 20) {
           keycnt = 0;
           if (!kbdcontinue()) return(found);
       }
   } /* while */

   if( !found ){
       printf( "value not found\n" );
       return( 0 );
   } else
     return( found );
}


void
ParseVal( char *s, ul *val, ul *mask, int size )
{
   ul tmpval;
   ul tmpmask;
   char c;

   *val  = 0;
   *mask = 0xffffffffffffffff;

   while( c = *s++ ){
	   tmpval = 0;
	   tmpmask = 0xffffffffffffffff;
	   if( isdigit((int) c ) ) tmpval = c - '0';
	   else{
		   c = tolower((int)c);
		   if( (c >= 'a') && (c <= 'f') ) tmpval = 10 + c - 'a';
	   }
	   if( c == '*' ) tmpmask = 0xfffffffffffffff0;
	   *mask = ( (*mask << 4) | 0xf ) & tmpmask;
	   *val = (*val * 0x10) + tmpval;
   }

   switch ( size ){
	 case 0:
	   *mask &= 0xff;
	   break;
	 case 1:
	   *mask &= 0xffff;
	   break;
	 case 2:
	   *mask &= 0xffffffff;
	   break;
	 case 3:
	   *mask &= 0xffffffffffffffff;
	   break;
   }


}
