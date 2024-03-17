#ifdef DC21064
#include "dc21064.h"
#else
#if defined(DC21164) || defined(DC21164PC)
#include "dc21164.h"
#else
#ifdef DC21264
#include "dc21264.h"
#else
#error Unsupported CPU type in regtest.s
#endif /* DC21264 */
#endif /* DC21164 || DC21164PC */
#endif /* DC21064 */


	.set    noat
	.set    noreorder

/* Set all registers to 0 (except the stack pointer - R30) and jump to address 0 */
Test1:
	and     r31, 0, r0
	and     r31, 1, r1
	and     r31, 2, r2
	and     r31, 3, r3
	and     r31, 4, r4
	and     r31, 5, r5
	and     r31, 6, r6
	and     r31, 7, r7
	and     r31, 8, r8
	and     r31, 9, r9
	and     r31, 10, r10
	and     r31, 11, r11
	and     r31, 12, r12
	and     r31, 13, r13
	and     r31, 14, r14
	and     r31, 15, r15
	and     r31, 16, r16
	and     r31, 17, r17
	and     r31, 18, r18
	and     r31, 19, r19
	and     r31, 20, r20
	and     r31, 21, r21
	and     r31, 22, r22
	and     r31, 23, r23
	and     r31, 24, r24
	and     r31, 25, r25
	and     r31, 26, r26
	and     r31, 27, r27
	and     r31, 28, r28
	and     r31, 29, r29
	jmp     r31, (r31)

Test2:
	bis     r31, 0, r0
	bis     r31, 1, r1
	bis     r31, 2, r2
	bis     r31, 3, r3
	bis     r31, 4, r4
	bis     r31, 5, r5
	bis     r31, 6, r6
	bis     r31, 7, r7
	bis     r31, 8, r8
	bis     r31, 9, r9
	bis     r31, 10, r10
	bis     r31, 11, r11
	bis     r31, 12, r12
	bis     r31, 13, r13
	bis     r31, 14, r14
	bis     r31, 15, r15
	bis     r31, 16, r16
	bis     r31, 17, r17
	bis     r31, 18, r18
	bis     r31, 19, r19
	bis     r31, 20, r20
	bis     r31, 21, r21
	bis     r31, 22, r22
	bis     r31, 23, r23
	bis     r31, 24, r24
	bis     r31, 25, r25
	bis     r31, 26, r26
	bis     r31, 27, r27
	bis     r31, 28, r28
	bis     r31, 29, r29
	jmp     r31, (r31)

Test4:
	and     r31, 0, r0
	and     r31, 1, r1
	and     r31, 2, r2
	and     r31, 3, r3
	and     r31, 4, r4
	and     r31, 5, r5
	and     r31, 6, r6
	and     r31, 7, r7
	and     r31, 8, r8
	and     r31, 9, r9
	and     r31, 10, r10
	and     r31, 11, r11
	and     r31, 12, r12
	and     r31, 13, r13
	and     r31, 14, r14
	and     r31, 15, r15
	and     r31, 16, r16
	and     r31, 17, r17
	and     r31, 18, r18
	and     r31, 19, r19
	and     r31, 20, r20
	and     r31, 21, r21
	and     r31, 22, r22
	and     r31, 23, r23
	and     r31, 24, r24
	and     r31, 25, r25
	and     r31, 26, r26
	and     r31, 27, r27
	and     r31, 28, r28
	and     r31, 29, r29
	ldah    r0, 0x30(r31)
	jmp     r31, (r0)

Test5:
	bis     r31, 0, r0
	bis     r31, 1, r1
	bis     r31, 2, r2
	bis     r31, 3, r3
	bis     r31, 4, r4
	bis     r31, 5, r5
	bis     r31, 6, r6
	bis     r31, 7, r7
	bis     r31, 8, r8
	bis     r31, 9, r9
	bis     r31, 10, r10
	bis     r31, 11, r11
	bis     r31, 12, r12
	bis     r31, 13, r13
	bis     r31, 14, r14
	bis     r31, 15, r15
	bis     r31, 16, r16
	bis     r31, 17, r17
	bis     r31, 18, r18
	bis     r31, 19, r19
	bis     r31, 20, r20
	bis     r31, 21, r21
	bis     r31, 22, r22
	bis     r31, 23, r23
	bis     r31, 24, r24
	bis     r31, 25, r25
	bis     r31, 26, r26
	bis     r31, 27, r27
	bis     r31, 28, r28
	bis     r31, 29, r29
	ldah    r1, 0x30(r31)
	jmp     r31, (r1)


Test6:
	addq    r1, r2, r3
	subq    r3, r2, r1
	addq    r1, r2, r3
	addq    r4, r5, r6
	jmp     r31, (r1)

Test7:
	cvtqs   f5, f1
	cvtqs   f6, f2
	adds    f1, f2, f3
	cvtst   f3, f7
	cvttq   f7, f8
	subs    f3, f2, f1
	adds    f1, f2, f3
	subs    f3, f2, f1
	jmp     r31, (r31)




