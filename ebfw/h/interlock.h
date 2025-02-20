#ifndef __INTERLOCK_H_LOADED
#define __INTERLOCK_H_LOADED
/*****************************************************************************

       Copyright 1993, 1994, 1995 Digital Equipment Corporation,
                       Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, provided  
that the copyright notice and this permission notice appear in all copies  
of software and supporting documentation, and that the name of Digital not  
be used in advertising or publicity pertaining to distribution of the software 
without specific, written prior permission. Digital grants this permission 
provided that you prominently mark, as not part of the original, any 
modifications made to this software or documentation.

Digital Equipment Corporation disclaims all warranties and/or guarantees  
with regard to this software, including all implied warranties of fitness for 
a particular purpose and merchantability, and makes no representations 
regarding the use of, or the results of the use of, the software and 
documentation in terms of correctness, accuracy, reliability, currentness or
otherwise; and you rely on the software, documentation and results solely at 
your own risk. 

******************************************************************************/

/*
 * $Id: interlock.h,v 1.1.1.1 1998/12/29 21:36:06 paradis Exp $;
 *
 * $Log: interlock.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:06  paradis
 * Initial CVS checkin
 *
 * Revision 1.3  1997/12/17  15:11:22  pbell
 * Changed defines for unix build.
 *
 * Revision 1.2  1997/12/15  20:48:25  pbell
 * Updated for dp264.
 *
 */

#ifdef _WIN32

#define INLINE __inline
#define ASM __asm

#elif __GNUC__

#define INLINE static
#define ASM __asm__ 

#else /* unix */

#define INLINE static
#define ASM asm
#include <c_asm.h>

#endif

#if !defined(_WIN32) && !defined(__GNUC__)
#pragma inline (atomic_inc)
#endif
INLINE int atomic_inc(int source, volatile int *dest)
{
    unsigned int temp = 0, ret = -1;
    ASM ("mb;");
    while (temp == 0)
    {
#if __GNUC__
/*  Might be more correct, but doesn't match the original...
        __asm__ (
            "ldl_l   %0, %2;"   // <- | (dest) => "ret", 1 => lock_flag
            "addl    %0, %3, %1;"  // -> | "ret" + 1 => "temp"
            "stl_c   %1, %2;"   // -> | "temp" => (dest), lock_flag => temp
            : "+r" (ret), "=r" (temp), "=m" (dest)
            : "r" (source)
        );
*/
        __asm__ (
            "ldl_l   %0, %2;"   // <- | (dest) => "ret", 1 => lock_flag
            "addl    %0, 1, %1;"  // -> | "ret" + 1 => "temp"
            "stl_c   %1, %2;"   // -> | "temp" => (dest), lock_flag => temp
            : "+&r" (ret), "+r" (temp), "+m" (*dest)
        );
#else
        /*
            http://odl.sysworks.biz/disk$axpdocsep992/progtool/deccv60/5492p024.htm#index_x_856
            
            a0-a5 argument registers: R16-R21
            v0 return value: R0 or F0, depending on type
        */

       ret= ASM("ldl_l   %v0,(%a0);",dest);
       temp= ASM("addl   %a0,1,%v0;\
                  stl_c   %v0,(%a1) ;",ret,dest);
#endif
    }
    ASM ("mb;");
    return ret;
}

#if !defined(_WIN32) && !defined(__GNUC__)
#pragma inline (test_set_low)
#endif
static int test_set_low(volatile long *dest)
{
    long temp = 0, ret = -1;
    ASM ("mb;");
//    printf("test_set_low start %d %p %lx\n",gh_task.pid,dest,*dest);
    while (temp==0)
    {
#if __GNUC__
        __asm__ (
            "ldq_l  %0, %1;"
            : "=&r" (ret)
            : "m" (*dest)
        );

        if (ret & 1) return 1;

        __asm__ (
            "bis    %2, 1, %0;"
            "stq_c  %0, %1;"
            : "+r" (temp), "=m" (*dest)
            : "r" (ret)
        );
#else
       ret= ASM("ldq_l   %v0,(%a0) ;",dest);
       if (ret&1) return 1;
       temp= ASM("bis   %a0,1,%v0;\
                  stq_c   %v0,(%a1) ;",ret,dest);
#endif
    }

    ASM ("mb;");
//    printf("test_set_low %d %p %lx %lx\n",gh_task.pid,dest,*dest,ret);
    return 0;
}

#if !defined(_WIN32) && !defined(__GNUC__)
#pragma inline (write_atomic)
#endif
static long write_atomic(long *dest, long source)
{
//    printf("write_atomic start %d %p %ld %ld\n",gh_task.pid,dest,*dest,source);
    ASM ("mb;");
    *dest = source;
//    printf("write_atomic %d %p %ld %ld\n",gh_task.pid,dest,*dest,source);
    return 0;
}

#endif /* __INTERLOCK_H_LOADED */