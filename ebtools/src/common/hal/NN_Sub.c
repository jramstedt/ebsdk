/*
 * @DEC_COPYRIGHT@
 */

/*
 * $Log: NN_Sub.c,v $
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

static char rcsid[] = "$Id: NN_Sub.c,v 1.1.1.1 1998/12/29 21:36:20 paradis Exp $";

#include "NN.h"

/*
 *  Computes:
 *      a = b - c.
 *  Returns:
 *      borrow.
 *  Lengths:
 *      a[digits], b[digits], c[digits]
 */
NN_DIGIT
NN_Sub(NN_DIGIT *a,
       NN_DIGIT *b,
       NN_DIGIT *c,
       unsigned int digits)
{
    NN_DIGIT ai, borrow;
    unsigned int i;
    
    borrow = 0;
    
    for(i = 0; i < digits; i++)
    {
        if((ai = b[i] - borrow) > (MAX_NN_DIGIT - borrow))
        {
            ai = MAX_NN_DIGIT - c[i];
        }
        else if((ai -= c[i]) > (MAX_NN_DIGIT - c[i]))
        {
            borrow = 1;
        }
        else
        {
            borrow = 0;
        }
        a[i] = ai;
    }
    return(borrow);
}
