/*
 * @DEC_COPYRIGHT@
 */

/*
 * $Log: NN_Div.c,v $
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
 * Revision 1.1  1994/02/15  19:18:30  raxdev
 * Initial AIM checkin.
 *
 */

static char rcsid[] = "$Id: NN_Div.c,v 1.1 1999/04/22 15:38:44 stig Exp $";

#include "NN.h"

/*
 *  Computes:
 *      a = c / d and  b = c % d.
 *  Assumes:
 *      d > 0, cDigits < 2*MAX_NN_DIGITS , dDigits < MAX_NN_DIGITS.
 *  Returns:
 *      void
 *  Lengths:
 *      a[cDigits], b[dDigits], c[cDigits], d[dDigits].
 */
void
NN_Div(NN_DIGIT *a,
       NN_DIGIT *b,
       NN_DIGIT *c,
       unsigned int cDigits,
       NN_DIGIT *d,
       unsigned int dDigits)
{
    NN_DIGIT ai, cc[2*MAX_NN_DIGITS+1], dd[MAX_NN_DIGITS], t;
    int i;
    unsigned int ddDigits, shift;
    
    ddDigits = NN_Digits(d, dDigits);
    if(ddDigits == 0)
        return;
    
    /*
     * Normalize operands.
     */
    shift = NN_DIGIT_BITS - NN_DigitBits(d[ddDigits-1]);
    NN_AssignZero(cc,ddDigits);
    cc[cDigits] = NN_LShift(cc,c,shift,cDigits);
    NN_LShift(dd,d,shift,ddDigits);
    t = dd[ddDigits-1];
    
    NN_AssignZero(a,cDigits);
    
    for(i = cDigits - ddDigits; i >= 0; i--)
    {
        /*
         * Underestimate quotient digit and subtract.
         */
        if(t == MAX_NN_DIGIT)
        {
            ai = cc[i+ddDigits];
        }
        else
        {
            NN_DigitDiv(&ai,&cc[i+ddDigits-1],t+1);
        }
        cc[i+ddDigits] -= NN_SubDigitMult(&cc[i], &cc[i], ai, dd, ddDigits);
        /*
         * Correct estimate.
         */
        while(cc[i+ddDigits] || (NN_Cmp(&cc[i], dd, ddDigits) >= 0))
        {
            ai++;
            cc[i+ddDigits] -= NN_Sub(&cc[i], &cc[i], dd, ddDigits);
        }
        a[i] = ai;
    }
    /*
     * Restore result.
     */
    NN_AssignZero(b, dDigits);
    NN_RShift(b, cc, shift, ddDigits);
}
