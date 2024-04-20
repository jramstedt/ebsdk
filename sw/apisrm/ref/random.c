/* file:	random.c
 *
 * Copyright (C) 1990 by
 * Digital Equipment Corporation, Maynard, Massachusetts.
 * All rights reserved.
 *
 * This software is furnished under a license and may be used and copied
 * only  in  accordance  of  the  terms  of  such  license  and with the
 * inclusion of the above copyright notice. This software or  any  other
 * copies thereof may not be provided or otherwise made available to any
 * other person.  No title to and  ownership of the  software is  hereby
 * transferred.
 *
 * The information in this software is  subject to change without notice
 * and  should  not  be  construed  as a commitment by digital equipment
 * corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by digital.
 */

/*
 *++
 *  FACILITY:
 *
 *      Cobra Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *	Random number generator.  This module has a lot of hardware
 *	assumptions in it regarding word size and byte ordering.
 *
 *  AUTHORS:
 *
 *	AJ Beaverson
 *
 *  CREATION DATE:
 *  
 *      27-Aug-1990
 *
 *  MODIFICATION HISTORY:
 *
 *	twp	16-Aug-1991	add more LCG table entries
 *
 *	pel	20-Jun-1991	add more LCG table entries
 *
 *	jad	27-Aug-1990	Generalize random number generator.
 *
 *--
 */  

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$inc:prototypes.h"

unsigned int lcg_coefficients [] = {
/*	             A                                 C			*/
/*	--------------------------	--------------------------		*/
	0x00000001,	0x00000000,	0x00000001,	0x00000000,	/*  0	*/
	0x00000001,	0x00000000,	0x00000001,	0x00000000,	/*  1	*/
	0x00000005,	0x00000000,	0x00000001,	0x00000000,	/*  2	*/
	0x00000005,	0x00000000,	0x00000007,	0x00000000,	/*  3	*/
	0x00000005,	0x00000000,	0x00000007,	0x00000000,	/*  4	*/
	0x00000005,	0x00000000,	0x0000000B,	0x00000000,	/*  5	*/
	0x00000025,	0x00000000,	0x00000015,	0x00000000,	/*  6	*/
	0x00000025,	0x00000000,	0x00000039,	0x00000000,	/*  7	*/
	0x00000031,	0x00000000,	0x00000077,	0x00000000,	/*  8	*/
	0x0000001D,	0x00000000,	0x000000F7,	0x00000000,	/*  9	*/
	0x00000025,	0x00000000,	0x00000119,	0x00000000,	/* 10	*/
	0x0000002D,	0x00000000,	0x000002E9,	0x00000000,	/* 11 	*/
	0x00000045,	0x00000000,	0x000004AD,	0x00000000,	/* 12 	*/
	0x00000065,	0x00000000,	0x000006AD,	0x00000000,	/* 13 	*/
	0x00000095,	0x00000000,	0x00000D79,	0x00000000,	/* 14 	*/
	0x000000B5,	0x00000000,	0x00001AE3,	0x00000000,	/* 15 	*/
	0x00000105,	0x00000000,	0x000035E7,	0x00000000,	/* 16 	*/
	0x00000175,	0x00000000,	0x00006C5F,	0x00000000,	/* 17 	*/
	0x0000021D,	0x00000000,	0x0000D88F,	0x00000000,	/* 18 	*/
	0x000002F5,	0x00000000,	0x0001B09D,	0x00000000,	/* 19 	*/
	0x00000425,	0x00000000,	0x000361B5,	0x00000000,	/* 20 	*/
	0x000005AD,	0x00000000,	0x0006C2ED,	0x00000000,	/* 21 	*/
	0x00000815,	0x00000000,	0x000D8623,	0x00000000,	/* 22 	*/
	0x00000B5D,	0x00000000,	0x001B0C85,	0x00000000,	/* 23 	*/
	0x00001025,	0x00000000,	0x00361927,	0x00000000,	/* 24 	*/
	0x000016B5,	0x00000000,	0x006C32AB,	0x00000000,	/* 25 	*/
	0x0000201D,	0x00000000,	0x00D86557,	0x00000000,	/* 26 	*/
	0x00002D4D,	0x00000000,	0x01B0BE0D,	0x00000000,	/* 27 	*/
	0x00004025,	0x00000000,	0x03617C1B,	0x00000000,	/* 28 	*/
	0x00005A85,	0x00000000,	0x06C2F837,	0x00000000,	/* 29 	*/
	0x00008015,	0x00000000,	0x0D85F06F,	0x00000000,	/* 30 	*/
	0x0001000D,	0x00000000,	0x1fffff49,	0x00000000,	/* 31 	*/
	0x00010015,	0x00000000,	0x3617C1BD,	0x00000000,	/* 32 	*/
	0x00016A45,	0x00000000,	0x6C2F833B,	0x00000000,	/* 33 	*/
	0x0002003D,	0x00000000,	0xD85F06B7,	0x00000000,	/* 34 	*/
	0x0002D44D,	0x00000000,	0xB0BE0DAD,	0x00000001,	/* 35 	*/
	0x0004003D,	0x00000000,	0x617C1B9B,	0x00000003,	/* 36 	*/
	0x0005A85D,	0x00000000,	0xC2F83775,	0x00000006,	/* 37 	*/
	0x0008003D,	0x00000000,	0x85F06F29,	0x0000000D,	/* 38 	*/
	0x000B5085,	0x00000000,	0x0BE0DE93,	0x0000001B,	/* 39 	*/
	0x0010003D,	0x00000000,	0x17C1BD65,	0x00000036,	/* 40 	*/
	0x0016A0D5,	0x00000000,	0x2F837B0B,	0x0000006C,	/* 41 	*/
	0x0020003D,	0x00000000,	0x5F06F655,	0x000000D8,	/* 42 	*/
	0x002D4175,	0x00000000,	0xBE0DECE9,	0x000001B0,	/* 43 	*/
	0x0040003D,	0x00000000,	0x7C1BDA11,	0x00000361,	/* 44 	*/
	0x005A82B5,	0x00000000,	0xF837B463,	0x000006C2,	/* 45 	*/
	0x0080003D,	0x00000000,	0xF06F6905,	0x00000D85,	/* 46 	*/
	0x00B5052D,	0x00000000,	0xE0DED249,	0x00001B0B,	/* 47 	*/
	0x0100003D,	0x00000000,	0xC1BDA4D3,	0x00003617,	/* 48 	*/
	0x016A0A1D,	0x00000000,	0x837B49E3,	0x00006C2F,	/* 49 	*/
	0x0200003D,	0x00000000,	0x06F69407,	0x0000D85F,	/* 50 	*/
	0x02D41405,	0x00000000,	0x0DED284D,	0x0001B0BE,	/* 51 	*/
	0x0400003D,	0x00000000,	0x1BDA50DB,	0x0003617C,	/* 52 	*/
	0x05A827D5,	0x00000000,	0x37B4A1F5,	0x0006C2F8,	/* 53 	*/
	0x0800003D,	0x00000000,	0x6F694427,	0x000D85F0,	/* 54 	*/
	0x0B504F6D,	0x00000000,	0xDED2888F,	0x001B0BE0,	/* 55 	*/
	0x1000003D,	0x00000000,	0xBDA5115D,	0x003617C1,	/* 56 	*/
	0x16A09E9D,	0x00000000,	0x7B4A22FB,	0x006C2F83,	/* 57 	*/
	0x2000003D,	0x00000000,	0xF6944635,	0x00D85F06,	/* 58 	*/
	0x2D413CFD,	0x00000000,	0xED288CA7,	0x01B0BE0D,	/* 59 	*/
	0x4000003D,	0x00000000,	0xDA51198F,	0x03617C1B,	/* 60 	*/
	0x5A8279BD,	0x00000000,	0xB4A2335D,	0x06C2F837,	/* 61 	*/
	0x8000003D,	0x00000000,	0x694466F9,	0x0D85F06F,	/* 62 	*/
	0xB504F32D,	0x00000000,	0xD288CE31,	0x1B0BE0DE,	/* 63	*/
};


/*+
 * ============================================================================
 * = random - generate a random number                                        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	There are several occasions where the firmware will need a random
 *	number generator.  There are other occasions where we will need to
 *	"deal" from  a set of numbers, and after N deals, where N is size of
 *	the set, have every number dealt.  Two simple examples come to mind: 
 *	First, if we wanted to test all addresses between 0 and N inclusive,
 *	but not in sequential order, a function that generates all addresses
 *	once and only once before repeating would be useful. Another example
 *	would be if we wanted to generate randomly sized device packets within
 *	a given range and be confident that all sizes were generated.
 *	
 *	Knuth's volume II states that "randomly generating random number
 *	generators doesn't work".  In light of this, we have developed some
 *	simple generators that have known periods and properties.  These
 *	generators are based on a linear congruential generator, or LCG for
 *	short.  These generators are not the best generators available,
 *	however they are easy to study and implement.
 *	
 *	LCGs are iterative algorithms and have the following generic form:
 *~	
 *	        X (n=1) = (X (n) * a + b) mod m
 *~	
 *	The trick is to choose values of a, b and m such that the generator
 *	cycles through all of the values 0 to m inclusive.  If these numbers
 *	are picked randomly a very small percentage of the generators would
 *	generate random sequences.
 *	
 *	Knuth offers the following criteria for picking a, b, and m.
 *## 
 *	1  A, B and M should be relatively prime, i.e. the greatest common
 *	   divisor of A, B, and M should be 1.
 *	2  M should be large and prime if a random sequence is desired.
 *	   If dealing from  a range that has a modulus M = 2**n for some
 *	   integer N, then use 2**n+1 as the modulus.
 *	3  The computation of ax+m mod M should be done exactly.  In the
 *         case of 32 bit arithmetic, A, B and M must be less that 2**16 to
 *	   avoid roundoff errors.
 *	4  If M is a power of 2, pick A so that A mod 8 = 5.
 *	5  A >= sqrt (M) and A <= (M - sqrt(M)).  The digits in A should
 *	   not have a simple, regular pattern.
 *	6  C/M approximates 1/2 - 1/6 * sqrt (3) = 0.2113
 *	7  C should be odd.
 *#
 *	
 *	There are occasions where a random permutation on a set of numbers is
 *	required instead of random selections on a set.  The former case is
 *	called a deal.  LCGs can be used for a deal if the period is as large
 *	as the modulus m.
 *	
 *	Following are some generators for various values of M.  The generators
 *	that have a period of M can also be used for dealing. All arithmetic
 *	is assumed to be done modulo 2**32.
 *	
 *~
 *	                 Linear Congruential Generators
 *	
 *	        A           C              M              Period
 *	        ------------------------------------------------
 *	        37          21             2**6           2**6
 *	        37          57             2**7           2**7
 *	        49          119            2**8           2**8
 *	        29          247            2**9           2**9
 *	        37          281            2**10          2**10
 *	        45          745            2**11          2**11
 *	        69          1197           2**12          2**12
 *	        101         1709           2**13          2**13
 *	        149         3449           2**14          2**14
 *	        181         6883           2**15          2**15
 *	        261         13799          2**16          2**16
 *	        373         27743          2**17          2**17
 *	        541         55439          2**18          2**18
 *	        757         110749         2**19          2**19
 *	        1061        221621         2**20          2**20
 *	        1453        443117         2**21          2**21
 *	        2069        886307         2**22          2**22
 *	        2909        1772677        2**23          2**23
 *	        4133        3545383        2**24          2**24
 *	        5813        7090859        2**25          2**25
 *	        8221        14181719       2**26          2**26
 *	        11597       28360205       2**27          2**27
 *	        16421       56720411       2**28          2**28
 *	        23173       113440823      2**29          2**29
 *	        32789       226881647      2**30          2**30
 *	        46349       453763294      2**31          2**31
 *	        1           1              2**32          2**32
 *	        69069       0              2**32          2**32/4
 *	 
 *~
 * FORM OF CALL:
 *  
 *	random(k,i);
 *  
 * RETURNS:
 *
 *	n - a random number
 *       
 * ARGUMENTS:
 *
 * 	k - Previous random number or seed.
 *	i - Index into the LCG table.
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/

unsigned __int64 random(unsigned __int64 k, int i) {
	unsigned __int64 a;
	unsigned __int64 c;
	unsigned __int64 m;
    
	/* keep the index in range */
	i &= (sizeof (lcg_coefficients) / (sizeof (lcg_coefficients [0]) * 4)) - 1;
	m = 1;
	m <<= i;
	m--;

	a = *(unsigned __int64 *) &lcg_coefficients [i*4 + 0];
	c = *(unsigned __int64 *) &lcg_coefficients [i*4 + 2];
	
	return (k*a + c) & m;
}

#if 0
void rtest (int argc, char *argv[]) {
	unsigned __int64	a;
	unsigned __int64	c;
	unsigned __int64	m;
	unsigned __int64	period;
	unsigned __int64	log_period;
	unsigned __int64	seed;
	int	i;

	/* dump the coefficients */
	for (i=0; i<64; i++) {
	    a = *(unsigned __int64 *) &lcg_coefficients [i*4 + 0];
	    c = *(unsigned __int64 *) &lcg_coefficients [i*4 + 2];
	    m = 1;
	    m <<= i;
	    m--;
	    printf ("%2d %016x %016x %016x\n", i, (UINT) a, (UINT) c, (UINT) m);
	}	

	/* test the period of all the generators */
	for (i=0; i<64; i++) {
	    log_period = i;
	    period = 0;

	    seed = 0;
	    do {
		seed = random (seed, log_period);
		period++;
	    } while (seed);
	    printf ("%2d %016x %016x\n", log_period, period, seed);
	}
}

/*----------*/
UINT gcd2 (UINT a, UINT b) {
	UINT	temp;

	if (a==0 || b==0) {
	    return 0;
	}

	while (1) {
	    switch (a % b) {
	    case 0: return b;
	    case 1: return 1;
	    default:
		temp = a;
		a = b;
		b = temp % b;
	    }
	}
}

void generate_coefficients (int argc, char *argv[]) {
	UINT	a, c, log_m, m, x;
	UINT	seed;

	UINT	argix;
	UINT	ax, cx;
	int	i;
	UINT	cm_fuzz;
	UINT	a_upper;
	UINT	c_lower, c_upper;

	cm_fuzz = 256;

	/*
	 * Treat each number as an log2(M) and calculate possible
	 * generators, filtering out values of A, C and M that conflict with
	 * Knuth's criteria.
	 */
	for (argix=1; argix<argc; argix++) {

	    log_m = atoi (argv [argix]);
	    m = 1;
	    m <<= log_m;
	    a = m >> (log_m/2);	 /* a = sqrt (m) */
	    a_upper = m - a;

	    /* insure that a is odd */
	    a |= 1; 

	    /* only visit odd numbers */
	    for (;a <= a_upper; a += 2) {
		c = 0;
		if ((a % 8) != 5) continue;

		c_lower =  m / 4 - cm_fuzz;
		c_upper =  m / 4 + cm_fuzz;
		printf ("%d %d\n", c_lower, c_upper);

		if (c_lower >= m) c_lower = 1;
		if (c_upper >= m) c_upper = m-1;
		c = c_lower - 1;

		/* only check odd values */
		c |= 1;
		while (c < c_upper) {
		    c += 2;
		    if (gcd2 (a, c) != 1) continue;
		    printf ("%2d %016x %016x %016x\n", log_m, a, c);
		}
	    }
        }
}

#endif
