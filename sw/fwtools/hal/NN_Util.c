/*
 * @DEC_COPYRIGHT@
 */

/*
 * $Log: NN_Util.c,v $
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
 * Revision 1.1  1994/02/15  19:18:29  raxdev
 * Initial AIM checkin.
 *
 */

static char rcsid[] = "$Id: NN_Util.c,v 1.1 1999/04/22 15:38:44 stig Exp $";

#include "NN.h"

/*
 *  Assigns a = 0.
 *  Lengths:
 *      a[digits], b[digits].
 */
void
NN_Assign(NN_DIGIT *a,
          NN_DIGIT *b,
          unsigned int digits)
{
    unsigned int i;
    
    for(i = 0; i < digits; i++)
    {
        a[i] = b[i];
    }   
}

/*
 *  Assigns a = 0.
 *  Lengths:
 *      a[digits].
 */
void
NN_AssignZero(NN_DIGIT *a,
              unsigned int digits)
{
    unsigned int i;
    
    for(i = 0; i < digits; i++)
    {
        a[i] = 0;
    }
}

/*
 *  Returns sign of a - b.
 *  Lengths:
 *      a[digits], b[digits].
 */
int
NN_Cmp(NN_DIGIT *a,
       NN_DIGIT *b,
       unsigned int digits)
{
    int i;
    
    for(i = digits - 1; i >= 0; i--)
    {
        if(a[i] > b[i])
            return(1);
        if(a[i] < b[i])
            return(-1);
    }
    return(0);
}

/*
 *  Returns the significant length of a in digits.
 *  Lengths:
 *      a[digits].
 */
unsigned int
NN_Digits(NN_DIGIT *a,
          unsigned int digits)
{
    int i;
    
    for(i = digits - 1; i >= 0; i--)
    {
        if(a[i])
            break;
    }
    return(i+1);
}

/*
 *  Computes:
 *      a = b * 2^c. (i.e shifts left c bits).
 *  Returns:
 *      carry.
 *  Lengths:
 *      a[digits], b[digits]
 *  Requires:
 *      c < NN_DIGIT_BITS.
 */
NN_DIGIT
NN_LShift(NN_DIGIT *a,
          NN_DIGIT *b,
          unsigned int c,
          unsigned int digits)
{
    NN_DIGIT bi, carry;
    unsigned int i, t;
    
    if(c >= NN_DIGIT_BITS)
        return(0);
    t = NN_DIGIT_BITS - c;
    carry = 0;
    for(i = 0; i < digits; i++)
    {
        bi = b[i];
        a[i] = (bi << c) | carry;
        carry = c ? (bi >> t) : 0;
    }
    return(carry);
}

/*
 *  Computes:
 *      a = b / 2^c. (i.e shifts right c bits).
 *  Returns:
 *      carry.
 *  Lengths:
 *      a[digits], b[digits]
 *  Requires:
 *      c < NN_DIGIT_BITS.
 */
NN_DIGIT
NN_RShift(NN_DIGIT *a,
          NN_DIGIT *b,
          unsigned int c,
          unsigned int digits)
{
    NN_DIGIT bi, carry;
    int i;
    unsigned int t;
    
    if(c >= NN_DIGIT_BITS)
        return(0);
    t = NN_DIGIT_BITS - c;
    carry = 0;
    for(i = digits - 1; i >= 0; i--)
    {
        bi = b[i];
        a[i] = (bi >> c) | carry;
        carry = c ? (bi << t) : 0;
    }
    return(carry);
}

/*
 *  Returns the significant length of a in bits, where a is a digit.
 */
unsigned int
NN_DigitBits(NN_DIGIT a)
{
    unsigned int i;
    
    for(i = 0; i < NN_DIGIT_BITS; i++, a >>= 1)
    {
        if(a == 0)
            break;
    }
    return(i);
}
