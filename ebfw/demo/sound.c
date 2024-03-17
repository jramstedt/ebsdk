
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
static char *rcsid = "$Id: sound.c,v 1.1.1.1 1998/12/29 21:36:04 paradis Exp $";
#endif

/*
 * $Log: sound.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:04  paradis
 * Initial CVS checkin
 *
 * Revision 1.3  1995/08/25  20:27:27  fdh
 * Removed calls to uart_init() and included "lib.h".
 *
 * Revision 1.2  1994/08/05  20:18:18  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.1  1993/06/08  19:56:53  fdh
 * Initial revision
 *
 */




void Beep( int frequency, int duration );
void Sleep( int wait );

enum NOTES {
  C0 = 262, D0 = 296, E0 = 330, F0 = 349, G0 = 392, A0 = 440, B0 = 494,
  C1 = 523, D1 = 587, E1 = 659, F1 = 698, G1 = 784, A1 = 880, B1 = 988,
  EIGHTH = 125, QUARTER = 250, HALF = 500, WHOLE = 1000, END = 0
} song[] = {
  C1, HALF, G0, HALF, A0, HALF, E0, HALF, F0, HALF, E0, QUARTER,
  D0, QUARTER, C0, WHOLE, END
};

int initial;

void main ()
{
  int note;

  initial = inportb( 0x61 );
  for( note = 0; song[note]; note += 2 )
    Beep( song[note], song[note + 1] );
  outportb( 0x61, initial );
}

void Beep( int frequency, int duration )
{
  int control; 

  if (frequency) {
    if( duration < 75 ) duration = 75;

    outportb( 0x43, 0xb6 );

    frequency = (unsigned)(1193180L / frequency);
    outportb( 0x42, (char)frequency );
    outportb( 0x42, (char)(frequency >> 8) );

    control = inportb( 0x61 );

    outportb( 0x61, control | 0x3 );
  }

  Sleep (duration);

  outportb( 0x61, control );

}

void Sleep( int wait )
{
  wait*= 0x4000;
  while (--wait);
}
