/*
 * @DEC_COPYRIGHT@
 */

/*
 * $Log: NN_DigitDiv.c,v $
 * Revision 1.1  1999/04/22 15:38:44  stig
 * Re-structured for addition of hal, executables now go to bin subdir
 *
 * Revision 1.1.1.1  1998/12/29 21:36:20  paradis
 * Initial CVS checkin
 *
 * Revision 1.1  1995/04/27  18:42:24  huggins
 * Initial revision
 *
 * Revision 1.1  1995/01/31  20:05:53  asher
 * Initial revision
 *
 * Revision 1.1  1994/02/15  19:18:31  raxdev
 * Initial AIM checkin.
 *
 */

static char rcsid[] = "$Id: NN_DigitDiv.c,v 1.1 1999/04/22 15:38:44 stig Exp $";

#include "NN.h"
/*
 *  Computes:
 *      a = b / c.
 *  Assumes:
 *      b[1] < c and HIGH_HALF(c) > 0. For efficiency, c should be normalized.
 *  Returns:
 *      void    
 *  Lengths:
 *      b[2].
 */
void
NN_DigitDiv(NN_DIGIT *a,
            NN_DIGIT b[2],
            NN_DIGIT c)
{
    NN_DIGIT t[2], u, v;
    NN_HALF_DIGIT aHigh, aLow, cHigh, cLow;
    
    cHigh = HIGH_HALF(c);
    cLow = LOW_HALF(c);
    
    t[0] = b[0];
    t[1] = b[1];
    
    /*
     * Underestimate high half of quotient and subtract product of estimate
     * and divisor from dividend.
     */
    if(cHigh == MAX_NN_HALF_DIGIT)
        aHigh = HIGH_HALF(t[1]);
    else
        aHigh = (NN_HALF_DIGIT) (t[1] / (cHigh +1));
    u = (NN_DIGIT) aHigh * (NN_DIGIT) cLow;
    v = (NN_DIGIT) aHigh * (NN_DIGIT) cHigh;
    if((t[0] -= TO_HIGH_HALF(u)) > (MAX_NN_DIGIT - TO_HIGH_HALF(u)))
        t[1]--;
    t[1] -= HIGH_HALF(u);
    t[1] -= v;
    
    /*
     * Correct estimate.
     */
    while((t[1] > cHigh) ||
          ((t[1] == cHigh) && (t[0] >= TO_HIGH_HALF(cLow))))
    {
        if((t[0] -= TO_HIGH_HALF(cLow)) > MAX_NN_DIGIT - TO_HIGH_HALF(cLow))
        {
            t[1]--;
        }
        t[1] -= cHigh;
        aHigh++;
    }
    
    /*
     * Underestimate low half of quotient and subtract product of estimate
     * and divisor from what remains of dividend.
     */
    if(cHigh == MAX_NN_HALF_DIGIT)
    {
        aLow = LOW_HALF(t[1]);
    }
    else
    {
        aLow = (NN_HALF_DIGIT)
            ((NN_DIGIT)(TO_HIGH_HALF(t[1]) + HIGH_HALF(t[0]))/ (cHigh + 1));
    }
    u = (NN_DIGIT) aLow * (NN_DIGIT) cLow;
    v = (NN_DIGIT) aLow * (NN_DIGIT) cHigh;
    if((t[0] -= u) > (MAX_NN_DIGIT - u))
    {
        t[1]--;
    }
    if((t[0] -= TO_HIGH_HALF(v)) > (MAX_NN_DIGIT - TO_HIGH_HALF(v)))
    {
        t[1]--;
    }
    t[1] -= HIGH_HALF(v);

    /*
     * Correct estimate.
     */
    while((t[1] > 0) || ((t[1] == 0) && t[0] >= c))
    {
        if((t[0]  -= c) > (MAX_NN_DIGIT -c))
        {
            t[1]--;
        }
        aLow++;
    }
    *a = TO_HIGH_HALF(aHigh) + aLow;
}
