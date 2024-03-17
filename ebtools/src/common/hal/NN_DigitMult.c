/*
 * @DEC_COPYRIGHT@
 */

/*
 * $Log: NN_DigitMult.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:20  paradis
 * Initial CVS checkin
 *
 * Revision 1.1  1995/04/27  18:55:09  huggins
 * Initial revision
 *
 * Revision 1.1  1995/01/31  20:05:53  asher
 * Initial revision
 *
 * Revision 1.1  1994/02/15  19:18:30  raxdev
 * Initial AIM checkin.
 *
 */

static char rcsid[] = "$Id: NN_DigitMult.c,v 1.1.1.1 1998/12/29 21:36:20 paradis Exp $";

#include "NN.h"
/*
 *  Computes:
 *      a = b * c.
 *  Returns:
 *      void    
 *  Lengths:
 *      a[2].
 */
void
NN_DigitMult(NN_DIGIT a[2],
             NN_DIGIT b,
             NN_DIGIT c)
{
    NN_DIGIT t, u;
    NN_HALF_DIGIT bHigh, bLow, cHigh, cLow;
    
    bHigh = HIGH_HALF(b);
    bLow = LOW_HALF(b);
    cHigh = HIGH_HALF(c);
    cLow = LOW_HALF(c);
    
    a[0] = (NN_DIGIT) bLow * (NN_DIGIT) cLow;
    t = (NN_DIGIT) bLow * (NN_DIGIT) cHigh;
    u = (NN_DIGIT) bHigh * (NN_DIGIT) cLow;
    a[1] = (NN_DIGIT) bHigh * (NN_DIGIT) cHigh;
    
    if((t += u) < u)
    {
        a[1] += TO_HIGH_HALF(1);
    }
    u = TO_HIGH_HALF(t);
    
    if((a[0] += u) < u)
    {
        a[1]++;
    }
    a[1] += HIGH_HALF(t);
}
