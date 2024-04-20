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
 However, all copies of the program and its derivative works must contain the
 I2O SIG copyright notice.
**************************************************************************/

/*
 * This template provides place holders for architecture and compiler
 * dependencies. It should be filled in and renamed as i2odep.h.
 * i2odep.h is included by i2otypes.h. <xxx> marks the places to fill.
 */

#ifndef __INCi2odeph
#define __INCi2odeph

#define I2ODEP_REV 1_5_4

/*
 * Pragma macros. These are to assure appropriate alignment between
 * host/IOP as defined by the I2O Specification. Each one of the shared
 * header files includes these macros.
 */

#if 0 
#define PRAGMA_ALIGN_PUSH   <xxx>
#define PRAGMA_ALIGN_POP    <xxx>
#define PRAGMA_PACK_PUSH    <xxx>
#define PRAGMA_PACK_POP     <xxx>
#endif


#define PRAGMA_ALIGN_PUSH   
#define PRAGMA_ALIGN_POP    
#define PRAGMA_PACK_PUSH    
#define PRAGMA_PACK_POP     


/* Setup the basics */

typedef    char   S8;
typedef    short  S16;

typedef    unsigned char  U8;
typedef    unsigned short U16;

typedef    unsigned long U32;
typedef    long  S32;


/* Bitfields */

typedef    unsigned  long BF;


/* VOID */

#ifndef __VOID
typedef    void  VOID;
#define __VOID
#endif


/* Boolean */

#ifndef __BOOL
#define __BOOL

typedef unsigned long BOOL;
#endif


/* NULL */

#define NULL  (void *) 0

#endif /* __INCi2odeph */
