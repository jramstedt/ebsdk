/****************************************************************************
 All software on this website is made available under the following terms and
 conditions.  By downloading this software, you agree to abide by these terms
 and conditions with respect to this software.

 I2O SIG All rights reserved.

 These header files are provided, pursuant to your I2O SIG membership agreement,
 free of charge on an as-is basis without warranty of any kind, either express
 or implied, including but not limited to, implied warranties or merchantability
 and fitness for a particular purpose.  I2O SIG does not warrant that this
 program will meet the user's requirements or that the operation of these
 programs will be uninterrupted or error-free. Acceptance and use of this
 program constitutes the user's understanding that he will have no recourse
 to I2O SIG for any actual or consequential damages including, but not limited
 to, loss profits arising out of use or inability to use this program.

 Member is permitted to create derivative works to this header-file program.
 However, all copies of the program and its derivative works must contain
 the I2O SIG copyright notice.
**************************************************************************/

#ifndef __INCi2otypesh
#define __INCi2otypesh

#define I2OTYPES_REV 1_5_4

/* include architecture/compiler dependencies */

#include "cp$src:i2odep.h"

/* 64 bit defines */

typedef struct _S64 {
   U32                         LowPart;
   S32                         HighPart;
} S64;

typedef struct _U64 {
   U32                         LowPart;
   U32                         HighPart;
} U64;

/* Pointer to Basics */

typedef    VOID                *PVOID;
typedef    S8                  *PS8;
typedef    S16                 *PS16;
typedef    S32                 *PS32;
typedef    S64                 *PS64;

/* Pointer to Unsigned Basics */

typedef    U8                  *PU8;
typedef    U16                 *PU16;
typedef    U32                 *PU32;
typedef    U64                 *PU64;

/* misc */

typedef S32             I2O_ARG;
typedef U32             I2O_COUNT;
typedef U32             I2O_USECS;
typedef U32             I2O_ADDR32;
typedef U32             I2O_SIZE;

#endif /* __INCi2otypesh */
