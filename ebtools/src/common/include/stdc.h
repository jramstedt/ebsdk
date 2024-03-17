#ifndef _STDC_H_
#define _STDC_H_ 1

/* For Linux */
#ifdef __STDC__
#include <stdint.h>

typedef uint64_t vm_offset_t;
typedef uint64_t vm_size_t;
typedef uint64_t integer_t;

#define int16 uint16_t
#define int32 uint32_t
#define int64 uint64_t

#define	low32(x)	x
#define high32(x)	0
#define zero 		0
#define	neq64(a,b)	((a) != (b))
#define	eq64(a,b)	((a) == (b))
#define lt64(a,b)	((a) < (b))
#define assign64(a,b)	(a) = (b)
#define assign64h(a,b)	a = (a&0xffffffffL) | ((b)<<32)
#define assign64l(a,b)	a = (a&0xffffffff00000000L) | (b)
#define plus(a,b)	(a) + (b)
#define	minus(a,b)	(a) - (b)
#define plus_a_32(a,b)	(a) + (b)

#endif

#endif /* _STDC_H_ */