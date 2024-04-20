/*
 * @DEC_COPYRIGHT@
 */

/*
 * $Log: NN_SubDigitMult.c,v $
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

static char rcsid[] = "$Id: NN_SubDigitMult.c,v 1.1 1999/04/22 15:38:44 stig Exp $";

#include "NN.h"

/*
 *  Computes:
 *      a = b - c * d, where c is a digit.
 *  Returns:
 *      borrow.
 *  Lengths:
 *      a[digits], b[digits], d[digits].
 */
NN_DIGIT
NN_SubDigitMult(NN_DIGIT *a,
                NN_DIGIT *b,
                NN_DIGIT c,
                NN_DIGIT *d,
                unsigned int digits)
{
    NN_DIGIT borrow, t[2];
    unsigned int i;
    
    borrow = 0;
    for(i = 0; i < digits; i++)
    {
        NN_DigitMult(t,c,d[i]);
        if((a[i] = b[i] - borrow) > (MAX_NN_DIGIT - borrow))
        {
            borrow = 1;
        }
        else
        {
            borrow = 0;
        }
        if((a[i] -= t[0]) > (MAX_NN_DIGIT - t[0]))
        {
            borrow++;
        }
        borrow += t[1];
    }
    return(borrow);
}
