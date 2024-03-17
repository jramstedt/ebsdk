#ifndef _CROSS_H_
#define _CROSS_H_ 1

#ifdef	CROSS_COMPILE_32_TO_64_BITS

#define int32	int
typedef struct {
	unsigned int32	low;
	unsigned int32	high;
} int64;
#define	low32(x)	(x).low
#define high32(x)	(x).high

#define	_MACHINE_VM_TYPES_H_ 1
typedef int64 vm_offset_t;
typedef int64 vm_size_t;
typedef int64 integer_t;

extern int64	zero;
#define	neq64(a,b)	(((a).low != (b).low) || ((a).high != (b).high))
#define	eq64(a,b)	(((a).low == (b).low) && ((a).high == (b).high))
extern  int64 plus( int64, int64);
extern  int64 minus( int64, int64);
extern	int64 plus_a_32( int64, int32);

#else	/* CROSS_COMPILE_32_TO_64_BITS */

#include <mach/machine/vm_types.h>

#define int32 int

#define	low32(x)	x
#define high32(x)	0
#define zero 		0
#define	neq64(a,b)	((a) != (b))
#define	eq64(a,b)	((a) == (b))
#define plus(a,b)	(a) + (b)
#define	minus(a,b)	(a) - (b)
#define plus_a_32(a,b)	(a) + (b)

#endif	/* CROSS_COMPILE_32_TO_64_BITS */

#endif  /* _CROSS_H_ */
