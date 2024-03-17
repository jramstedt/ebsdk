/*
 * @DEC_COPYRIGHT@
 */

/*
 * $Log: NN.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:20  paradis
 * Initial CVS checkin
 *
 * Revision 1.1  1995/04/27  18:42:24  huggins
 * Initial revision
 *
 * Revision 1.1  1995/01/31  20:05:53  asher
 * Initial revision
 *
 * Revision 1.3  1994/07/01  18:36:05  santiago
 * Added info about source. (Dr Dobbs Journal)
 *
 * Revision 1.2  1994/06/15  21:11:58  asher
 * Changed NN_Mult to return an integer representing whether the
 * multiply overflowed the number of digits specified.
 *
 * Revision 1.1  1994/02/15  19:18:28  raxdev
 * Initial AIM checkin.
 *
 */

/*
 * $Id: NN.h,v 1.1.1.1 1998/12/29 21:36:20 paradis Exp $
 */

/*
 * Arbitrary Natural Number Arithmetic.
 *
 *     This is from the August, 1992, issue of Dr. Dobb's Journal.
 *     (public domain)
 */
#ifndef __NN_H__
#define __NN_H__ 1

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/* UINT2 defines a two byte word */
typedef unsigned short UINT2;

/* UINT4 defines a four byte word */
typedef unsigned int UINT4;

/* RSA key lengths */
#define MAX_RSA_MODULUS_BITS    1024
#define MAX_RSA_MODULUS_LEN ((MAX_RSA_MODULUS_BITS + 7) / 8)

typedef UINT4 NN_DIGIT;
typedef UINT2 NN_HALF_DIGIT;

/* Length of digit in bits */
#define NN_DIGIT_BITS       32
#define NN_HALF_DIGIT_BITS  16

/* Length of digit in bytes */
#define NN_DIGIT_LEN (NN_DIGIT_BITS / 8)

/* Maximum length in digits */
#define MAX_NN_DIGITS \
        ((MAX_RSA_MODULUS_LEN + NN_DIGIT_LEN -1) / NN_DIGIT_LEN + 1)

/* Maximum digits */
#define MAX_NN_DIGIT    0xffffffff
#define MAX_NN_HALF_DIGIT   0xffff

/* Macros */
#define LOW_HALF(x) (NN_HALF_DIGIT) ((x) & MAX_NN_HALF_DIGIT)
#define HIGH_HALF(x)\
    (NN_HALF_DIGIT) (((x) >> NN_HALF_DIGIT_BITS) & MAX_NN_HALF_DIGIT)
#define TO_HIGH_HALF(x) (((NN_DIGIT)(x)) << NN_HALF_DIGIT_BITS)
#define DIGIT_MSB(x) (unsigned int) (((x) >> (NN_DIGIT_BITS - 1)) & 1)
#define DIGIT_2MSB(x) (unsigned int) (((x) >> (NN_DIGIT_BITS - 2)) & 3)

/* Some General Functions */
void NN_Assign(NN_DIGIT *, NN_DIGIT *, unsigned int);
void NN_AssignZero(NN_DIGIT *, unsigned int);

/* Some Arithmetic Support functions */
NN_DIGIT NN_Add(NN_DIGIT *, NN_DIGIT *, NN_DIGIT *, unsigned int);
NN_DIGIT NN_Sub(NN_DIGIT *, NN_DIGIT *, NN_DIGIT *, unsigned int);
int NN_Mult(NN_DIGIT *, NN_DIGIT *, NN_DIGIT *, unsigned int);
int NN_Cmp(NN_DIGIT *, NN_DIGIT *, unsigned int);
unsigned int NN_Bits(NN_DIGIT *, unsigned int);
unsigned int NN_Digits(NN_DIGIT *, unsigned int);

NN_DIGIT NN_LShift(NN_DIGIT *, NN_DIGIT *, unsigned int, unsigned int);
NN_DIGIT NN_RShift(NN_DIGIT *, NN_DIGIT *, unsigned int, unsigned int);
void NN_Div(NN_DIGIT *, NN_DIGIT *, NN_DIGIT *, unsigned int, NN_DIGIT *, unsigned int);
NN_DIGIT NN_AddDigitMult(NN_DIGIT *, NN_DIGIT *, NN_DIGIT, NN_DIGIT *, unsigned int);
NN_DIGIT NN_SubDigitMult(NN_DIGIT *, NN_DIGIT *, NN_DIGIT, NN_DIGIT *, unsigned int);
unsigned int NN_DigitBits(NN_DIGIT);
void NN_DigitMult(NN_DIGIT[2], NN_DIGIT, NN_DIGIT);
void NN_DigitDiv(NN_DIGIT *, NN_DIGIT[2], NN_DIGIT);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* __NN_H__ */
