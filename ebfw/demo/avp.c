
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

#ifndef LINT
__attribute__((unused)) static const char *rcsid = "$Id: avp.c,v 1.1.1.1 1998/12/29 21:36:04 paradis Exp $";
#endif

/*
 * $Log: avp.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:04  paradis
 * Initial CVS checkin
 *
 * Revision 1.5  1995/09/12  21:39:32  fdh
 * Replaced call to MonGetChar() with call to edit_cmdline().
 *
 * Revision 1.4  1995/09/02  03:00:14  fdh
 * Replaced call to IsDigit with a call to isdigit.
 *
 * Revision 1.3  1995/08/25  20:27:27  fdh
 * Removed calls to uart_init() and included "lib.h".
 *
 * Revision 1.2  1994/08/05  20:18:18  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.1  1993/06/08  19:56:45  fdh
 * Initial revision
 *
 */

#include "lib.h"

extern void  vp(long,long);
void argvize( char *, char ** );
void gets( char * );
long hex();

#define MBUFS 10
#define MSIZE 30
void main()
{
  char *s;
  char inbuf[80];
  char **argv;
  char *argvs[MBUFS];
  char bufs[MBUFS*MSIZE];
  int i;

  s = inbuf;
  for( i=0; i<MBUFS; i++ )
     argvs[i] = &bufs[i*MSIZE];
  argv = argvs;

  printf("Hello world\n");
  for (;;)
  {
    printf("virtual  parms: " );
    gets( s );
    argvize( s, argv );

    printf( "argv0 = %s  argv1 = %s\n", argv[0], argv[1] );
    vp( hex( argv[0] ), hex( argv[1] ) );

  }
}

void
gets( char *s )
{
   char c;
   int chars = 0;

#define DEL 0177
#define RET 0xd

   while( c != '\n' ){
	   c = edit_cmdline();
	   switch( c ){
		 case DEL:
		   if( chars == 0 ) continue;
		   s--;
		   break;
		 case RET:
		   c = '\n';
		   break;
		 default:
		   chars++;
		   *s++ = c;
	   }
   }
   *s = '\0';

}

long
hex( char *s )
{
   long val = 0;
   char c;
   int v;

   while( c = *s++ ){
     v = 0;
     if( isdigit( c ) ) v = c - '0';
     else{
           c = tolower(c);
	   if( ( c>='a') && (c <= 'f') ) v = 10 + c -'a';
     }
     val = (val * 16) + v;
   }

   return( val );
}

/* assume argv already points to pre-allocated strings */
void
argvize( char *s, char **argv )
{
   char *args;
   int  i=0;

   while( *s ){
     args = argv[i];
     while( (*s != ' ') && (*s != '\n' ) )
	     *args++ = *s++;
     *args = '\0';
     s++;
     i++;
   }

}


	     
