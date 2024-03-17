# 1 "osfpal.s"




























































































































































































































































	.data
	.asciz "$Id: osfpal.s,v 1.2 1999/04/15 20:56:25 paradis Exp $"


# 1 "./dc21164.h"









































































































































































































































































































































































































































































































































































































































































































































































































































































































































































# 258 "osfpal.s"
# 1 "./osf.h"










































































































































































































































































































































































































































































































































































# 259 "osfpal.s"
# 1 "./macros.h"













































































































































# 260 "osfpal.s"
# 1 "./impure.h"





































































































































































































































































































































































































































































# 261 "osfpal.s"



























	.global Sys_Reset
	.global Sys_ResetViaSwppal
	.global Sys_Interrupt
	.global Sys_IntMchkHandler
	.global	Sys_MchkLogOut
	.global Sys_EnterConsole
	.global	Sys_Cflush
	.global	Sys_Cserve
	.global	Sys_WrIpir
	.global Pal_End

	.global SaveState
	.global RestoreState

	.global CallPal_Rti
	.global HaltDoubleMchk
	.global MchkCommon






















































	.text	0

	. = 0x0000

Pal_Base:



































 
            . = 0x0000

Pal_Reset:

	hw_mtpr	$1	, 0x144			 
        br	$1	, ResetContinue	 




        .long	(1<<8) | (16<<0)
        .long	(0<<0)
        .long	_end
	.long	Pal_End		 
        .long	0








































 
            . = 0x0080

Pal_iAccVio:
	    bis	    $31, $31, $31				 
	hw_mtpr	$31	, 0x10F			 

	bis	$31	, $11	, $12			 
	hw_mfpr	$13	, 0x10B			 

	hw_mtpr	$31	, 0x201			 
	bic	$13	, 3, $13			 

		sll	$11	, 63-3, $25	;		bge	$25	, 0f;			bis	$31	, $31	, $11	;			hw_mtpr	$30	, 0x152	;			hw_mfpr	$30	, 0x153	;		0:	lda	$30	, 0-48($30	);		stq	$12	, 0x0000($30	);		stq	$13	, 0x0008($30	);		stq	$29	, 0x0010($30	);		stq	$16	, 0x0018($30	);		stq	$17, 0x0020($30	);		stq	$18, 0x0028($30	)

	hw_mfpr	$14	, 0x155			 

	lda	$17, 0x1($31	)	 

	bis	$31	, $13	, $16			 
	hw_mtpr	$14	, 0x10B			 
	subq	$31	, 1, $18		 
	hw_mfpr	$29	, 0x156			 

	hw_rei				 






























































 
            . = 0x0100

Pal_Interrupt:
	hw_mfpr	$25	, 0x100				 
	srl	$25	, 34, $9		 

	hw_mfpr	$14	, 0x110				 
	    bis	    $31, $31, $31				 

	hw_mfpr	$13	, 0x111			 
	    bis	    $31, $31, $31				 








	hw_mtpr	$31	, 0x201			 
	blbs	$9	, HaltHandler		 





	cmple	$13	, $14	, $8			 
	bne	$8	, PassiveRelease	 

	hw_mfpr	$14	, 0x10B			 
	bis	$31	, $11	, $9			 

		sll	$11	, 63-3, $12	;		bge	$12	, 0f;			bis	$31	, $31	, $11	;			hw_mtpr	$30	, 0x152	;			hw_mfpr	$30	, 0x153	;		0:	lda	$30	, 0-48($30	);		stq	$9	, 0x0000($30	);		stq	$14	, 0x0008($30	);		stq	$29	, 0x0010($30	);		stq	$16	, 0x0018($30	);		stq	$17, 0x0020($30	);		stq	$18, 0x0028($30	)		 

					 
					 

                                         
pvc$osf00$354:				 
	hw_mtpr	$31	, 0x10F			 

	br	$31	, InterruptContinue	 







































 
            . = 0x0180

Pal_ItbMiss:
	hw_mfpr	$8	, 0x112		 
	hw_mfpr	$10	, 0x10B			 

	    bis	    $31, $31, $31
	hw_mtpr	$10	, 0x146			 

Itb_LdVpte:
	hw_ldq/v	$8	, 0($8	)		 
	hw_mtpr	$10	, 0x10B			 

	hw_mfpr	$31	, 0x206			 
	hw_mfpr	$9	, 0x154			 

	and	$8	, ((1<<3) | (1<<0)), $25	  
	blbs	$9	, 1f			 

	cmpeq	$25	, (1<<0), $25		 
	beq	$25	, 2f			 

	    bis	    $31, $31, $31
	hw_mtpr	$8	, 0x102			 

	hw_rei_stall			 



	    .align  3

1:	srl	$10	, 13, $9		 
	sll	$9	, 32, $9		 

	lda	$9	, 0x3371($9	)		 
	hw_mtpr	$9	, 0x102			 

	hw_rei_stall			 

	    .align  3

2:	and	$8	, (1<<0), $9		 
	beq	$9	, PteInvalid		 

	br	$31	, ItbFoe		 








































 
            . = 0x0200

Pal_DtbMiss:
	hw_mfpr	$8	, 0x207			 
	hw_mfpr	$10	, 0x10B			 

	hw_mfpr	$9	, 0x205			 
	hw_mtpr	$10	, 0x146			 

Dtb_LdVpte:
	hw_ldq/v	$8	, 0($8	)		 
	hw_mfpr	$10	, 0x154			 

	blbs	$10	, 1f			 

	hw_mfpr	$10	, 0x206				 

	hw_mtpr	$8	, 0x203			 
	blbc	$8	, 2f			 

	hw_mtpr	$10	, 0x202			 
	hw_mfpr	$10	, 0x146			 

	hw_mtpr	$10	, 0x10B			 
	hw_mfpr    $31, 0x140

	hw_rei				 



	    .align  3

1:	hw_mfpr	$8	, 0x206				 
	srl	$8	, 13, $9		 

	sll	$9	, 32, $9		 
	lda	$9	, 0x3371($9	)		 

	hw_mtpr	$9	, 0x203			 
	    bis	    $31, $31, $31				 

	hw_mtpr	$8	, 0x202			 
	hw_mfpr	$10	, 0x146			 

	hw_mtpr	$10	, 0x10B			 
	hw_mfpr    $31, 0x140

	hw_rei				 

	    .align  3

2:	    bis	    $31, $31, $31
	br	$31	, PteInvalid	 





































 
            . = 0x0280

Pal_DoubleMiss:








	    bis	    $31, $31, $31				 
	hw_mtpr	$22	, 0x144			 


















	sll	$8	, 64-(2*10+13), $22	  
	hw_mtpr	$23, 0x145			 

	srl	$22	, 61-10, $22		 
	hw_mfpr	$23, 0x154			 

	blbs	$23, 2f			 
	addq	$23, $22	, $23		 

	sll	$8	, 64-(1*10+13), $22	  
	hw_ldq/p	$23, 0($23)		 

	srl	$22	, 61-10, $22		 

	blbc	$23, FakePte 		 












	srl	$23, 32, $23	 

	sll	$23, 13, $23	 

	addq	$23, $22	, $23		 

	hw_ldq/p	$23, 0($23)		 
	blbc	$23, FakePte 		 









	hw_mtpr	$23, 0x203			 

	hw_mfpr	$23, 0x145			 
	hw_mtpr	$8	, 0x202			 

1:	hw_mfpr	$22	, 0x144 
	    bis	    $31, $31, $31				 

	    bis	    $31, $31, $31				 
	    bis	    $31, $31, $31

	    bis	    $31, $31, $31
	    bis	    $31, $31, $31

	hw_rei				 

2:	hw_mfpr	$22	, 0x10B			 
	addq	$22	, 4, $22			 
	hw_mtpr	$22	, 0x10B			 
	hw_mfpr	$23, 0x145			 

	br	$31	, 1b







































 
            . = 0x0300

Pal_Unalign:
	bis	$31	, $11	, $12			 
	hw_mtpr	$31	, 0x10F			 

	hw_mfpr	$13	, 0x205			 
	hw_mfpr	$14	, 0x10B			 

	srl	$13	, 6, $13		 
	blbs	$14	, MchkBugCheck	 

	hw_mfpr	$25	, 0x206				 
	addq	$14	, 4, $14			 

	hw_mtpr	$31	, 0x201			 

	hw_mtpr	$25	, 0x140			 

		sll	$11	, 63-3, $25	;		bge	$25	, 0f;			bis	$31	, $31	, $11	;			hw_mtpr	$30	, 0x152	;			hw_mfpr	$30	, 0x153	;		0:	lda	$30	, 0-48($30	);		stq	$12	, 0x0000($30	);		stq	$14	, 0x0008($30	);		stq	$29	, 0x0010($30	);		stq	$16	, 0x0018($30	);		stq	$17, 0x0020($30	);		stq	$18, 0x0028($30	)

	hw_mfpr	$25	, 0x142			 

	and	$13	, 0x1F, $18		 

	hw_mfpr	$16	, 0x140			 
	srl	$13	, (11-6), $17  

	hw_mtpr	$25	, 0x10B			 
	and	$17, 0x3F, $17		 

	hw_mfpr	$29	, 0x156			 

	hw_rei				 











































 
            . = 0x0380

Pal_Dfault:
	    bis	    $31, $31, $31				 
	hw_mtpr	$31	, 0x10F			 

	hw_mfpr	$13	, 0x205			 
	hw_mfpr	$8	, 0x10B			 

	srl	$13	, 11, $9		 
	bis	$31	, $11	, $12			 

	hw_mtpr	$31	, 0x201			 
	and	$9	, 0x3F, $9			 

	and	$13	, 0xF, $13			 
	br	$31, DfaultContinue





























 
            . = 0x0400

Pal_Mchk:

	hw_mtpr	$31	, 0x119			 
	br	$31	, MchkContinue	 




































 
            . = 0x0480

Pal_OpcDec:
	bis	$31	, $11	, $12			 
	hw_mtpr	$31	, 0x10F			 

	hw_mfpr	$14	, 0x10B			 

	blbs	$14	, MchkBugCheck	 

	hw_mtpr	$31	, 0x201			 
	addq	$14	, 4, $14			 

		sll	$11	, 63-3, $25	;		bge	$25	, 0f;			bis	$31	, $31	, $11	;			hw_mtpr	$30	, 0x152	;			hw_mfpr	$30	, 0x153	;		0:	lda	$30	, 0-48($30	);		stq	$12	, 0x0000($30	);		stq	$14	, 0x0008($30	);		stq	$29	, 0x0010($30	);		stq	$16	, 0x0018($30	);		stq	$17, 0x0020($30	);		stq	$18, 0x0028($30	)

	hw_mfpr	$13	, 0x147			 

	bis	$31	, 0x4, $16		 

	hw_mtpr	$13	, 0x10B			 

	hw_mfpr	$29	, 0x156			 

	hw_rei				 






































 
            . = 0x0500

Pal_Arith:
	    bis	    $31, $31, $31				 
	hw_mtpr	$31	, 0x10F			 

	bis	$31	, $11	, $12			 
	hw_mfpr	$31	, 0x206			 

	hw_mtpr	$31	, 0x201			 
	hw_mfpr	$14	, 0x10B			 

	    bis	    $31, $31, $31				 
	blbs	$14	, MchkBugCheck	 
	
		sll	$11	, 63-3, $25	;		bge	$25	, 0f;			bis	$31	, $31	, $11	;			hw_mtpr	$30	, 0x152	;			hw_mfpr	$30	, 0x153	;		0:	lda	$30	, 0-48($30	);		stq	$12	, 0x0000($30	);		stq	$14	, 0x0008($30	);		stq	$29	, 0x0010($30	);		stq	$16	, 0x0018($30	);		stq	$17, 0x0020($30	);		stq	$18, 0x0028($30	)

	hw_mfpr	$13	, 0x10C			 

	hw_mfpr	$17, 0x10D			 

	hw_mfpr	$12	, 0x14C			 
	srl 	$13	, 10, $16		 

	hw_mtpr	$31	, 0x10C			 

	hw_mtpr	$12	, 0x10B			 

	hw_mfpr	$29	, 0x156			 

	hw_rei				 








































 
            . = 0x0580

Pal_Fen:
	hw_mtpr	$31	, 0x201			 
	hw_mtpr	$31	, 0x10F			 

	hw_mfpr	$14	, 0x10B			 

	blbs	$14	, MchkBugCheck	 

	hw_mfpr	$13	, 0x118			 
	bis	$31	, $11	, $12			 

	srl	$13	, 26, $13		 
	addq	$14	, 4, $25			 

	cmovlbs	$13	, $25	, $14			 

		sll	$11	, 63-3, $25	;		bge	$25	, 0f;			bis	$31	, $31	, $11	;			hw_mtpr	$30	, 0x152	;			hw_mfpr	$30	, 0x153	;		0:	lda	$30	, 0-48($30	);		stq	$12	, 0x0000($30	);		stq	$14	, 0x0008($30	);		stq	$29	, 0x0010($30	);		stq	$16	, 0x0018($30	);		stq	$17, 0x0020($30	);		stq	$18, 0x0028($30	)

	hw_mfpr	$14	, 0x147			 

	and	$13	, 1, $13			 

	addq	$13	, 0x3, $16		 

	hw_mtpr	$14	, 0x10B			 

	hw_mfpr	$29	, 0x156			 

	hw_rei				 

































	.text	1

	. = 0x2000





























 
            . = (0x2000+(0x0000<<6))

CallPal_Halt:

	hw_mfpr    $31, 0x140
	hw_mfpr    $31, 0x140

	hw_mfpr	$12	, 0x10B			 
	subq	$12	, 4, $12			 
	hw_mtpr	$12	, 0x10B			 

	hw_mtpr	$0	, 0x140			 
pvc$osf01$5000:				 
	bsr	$0	, UpdatePCB		 

	lda	$0	, 0x0005($31	)	 
	br	$31	, Sys_EnterConsole	 






























 
            . = (0x2000+(0x0001<<6))

CallPal_Cflush:
	br	$31	, Sys_Cflush


































 
            . = (0x2000+(0x0002<<6))

CallPal_Draina:
	ldah	$14	, 0x100($31	)		 
	    bis	    $31, $31, $31				 

1:	subq	$14	, 1, $14			 
	hw_mfpr	$13	, 0x217			 

	srl	$13	, 6, $13	  
	ble	$14	, CallPal_Halt	 

	    bis	    $31, $31, $31				 
	blbs	$13	, 1b			 

	hw_rei

	    . = (0x2000+(0x0003<<6))

CallPal_OpcDec03:
	br	$31	, CallPal_OpcDec

	    . = (0x2000+(0x0004<<6))

CallPal_OpcDec04:
	br	$31	, CallPal_OpcDec

	    . = (0x2000+(0x0005<<6))

CallPal_OpcDec05:
	br	$31	, CallPal_OpcDec

	    . = (0x2000+(0x0006<<6))

CallPal_OpcDec06:
	br	$31	, CallPal_OpcDec

	    . = (0x2000+(0x0007<<6))

CallPal_OpcDec07:
	br	$31	, CallPal_OpcDec

	    . = (0x2000+(0x0008<<6))

CallPal_OpcDec08:
	br	$31	, CallPal_OpcDec














































 
            . = (0x2000+(0x0009<<6))

CallPal_Cserve:
	br	$31	, Sys_Cserve










































































 
            . = (0x2000+(0x000A<<6))

CallPal_SwpPal:
	cmpule	$16	, 255, $0			 
	cmoveq	$16	, $16	, $0			 

	blbc	$0	, CallPal_SwpPalCont	 
	cmpeq	$16	, 2, $0			 

	blbc	$0	, 1f			 
	hw_mfpr	$16	, 0x10E			 

	br	$31	, CallPal_SwpPalCont

1:	addq	$0	, 1, $0			 
	hw_rei

	
	    . = (0x2000+(0x000B<<6))

CallPal_OpcDec0B:
	br	$31	, CallPal_OpcDec

	    . = (0x2000+(0x000C<<6))

CallPal_OpcDec0C:
	br	$31	, CallPal_OpcDec

































 
            . = (0x2000+(0x000D<<6))

CallPal_WrIpir:
	br	$31	, Sys_WrIpir

	
	    . = (0x2000+(0x000E<<6))

CallPal_OpcDec0E:
	br	$31	, CallPal_OpcDec
	
	    . = (0x2000+(0x000F<<6))

CallPal_OpcDec0F:
	br	$31	, CallPal_OpcDec






























 
            . = (0x2000+(0x0010<<6))

CallPal_RdMces:
	hw_mfpr	$0	, 0x150			 
	and	$0	, ((1<<0) | (1<<1) | (1<<2) | (1<<3)                          | (1<<4)), $0		 

	hw_rei




































 
            . = (0x2000+(0x0011<<6))

CallPal_WrMces:
	and	$16	, ((1<<0) | (1<<1) | (1<<2)), $13			
	hw_mfpr	$14	, 0x150			 

	ornot	$31	, $13	, $13			 
	and	$16	, ((1<<3) | (1<<4)), $12	  

	and	$14	, $13	, $1			 
	bic	$1	, ((1<<3) | (1<<4)), $1	  

	or	$1	, $12	, $1			 
	hw_mtpr	$1	, 0x150			 

	    bis	    $31, $31, $31
	    bis	    $31, $31, $31

	hw_rei
	
	    . = (0x2000+(0x0012<<6))

CallPal_OpcDec12:
	br	$31	, CallPal_OpcDec
	
	    . = (0x2000+(0x0013<<6))

CallPal_OpcDec13:
	br	$31	, CallPal_OpcDec
	
	    . = (0x2000+(0x0014<<6))

CallPal_OpcDec14:
	br	$31	, CallPal_OpcDec
	
	    . = (0x2000+(0x0015<<6))

CallPal_OpcDec15:
	br	$31	, CallPal_OpcDec
	
	    . = (0x2000+(0x0016<<6))

CallPal_OpcDec16:
	br	$31	, CallPal_OpcDec
	
	    . = (0x2000+(0x0017<<6))

CallPal_OpcDec17:
	br	$31	, CallPal_OpcDec
	
	    . = (0x2000+(0x0018<<6))

CallPal_OpcDec18:
	br	$31	, CallPal_OpcDec
	
	    . = (0x2000+(0x0019<<6))

CallPal_OpcDec19:
	br	$31	, CallPal_OpcDec
	
	    . = (0x2000+(0x001A<<6))

CallPal_OpcDec1A:
	br	$31	, CallPal_OpcDec
	
	    . = (0x2000+(0x001B<<6))

CallPal_OpcDec1B:
	br	$31	, CallPal_OpcDec
	
	    . = (0x2000+(0x001C<<6))

CallPal_OpcDec1C:
	br	$31	, CallPal_OpcDec
	
	    . = (0x2000+(0x001D<<6))

CallPal_OpcDec1D:
	br	$31	, CallPal_OpcDec
	
	    . = (0x2000+(0x001E<<6))

CallPal_OpcDec1E:
	br	$31	, CallPal_OpcDec
	
	    . = (0x2000+(0x001F<<6))

CallPal_OpcDec1F:
	br	$31	, CallPal_OpcDec
	
	    . = (0x2000+(0x0020<<6))

CallPal_OpcDec20:
	br	$31	, CallPal_OpcDec
	
	    . = (0x2000+(0x0021<<6))

CallPal_OpcDec21:
	br	$31	, CallPal_OpcDec
	
	    . = (0x2000+(0x0022<<6))

CallPal_OpcDec22:
	br	$31	, CallPal_OpcDec
	
	    . = (0x2000+(0x0023<<6))

CallPal_OpcDec23:
	br	$31	, CallPal_OpcDec
	
	    . = (0x2000+(0x0024<<6))

CallPal_OpcDec24:
	br	$31	, CallPal_OpcDec
	
	    . = (0x2000+(0x0025<<6))

CallPal_OpcDec25:
	br	$31	, CallPal_OpcDec
	
	    . = (0x2000+(0x0026<<6))

CallPal_OpcDec26:
	br	$31	, CallPal_OpcDec
	
	    . = (0x2000+(0x0027<<6))

CallPal_OpcDec27:
	br	$31	, CallPal_OpcDec
	
	    . = (0x2000+(0x0028<<6))

CallPal_OpcDec28:
	br	$31	, CallPal_OpcDec
	
	    . = (0x2000+(0x0029<<6))

CallPal_OpcDec29:
	br	$31	, CallPal_OpcDec
	
	    . = (0x2000+(0x002A<<6))

CallPal_OpcDec2A:
	br	$31	, CallPal_OpcDec
































 
            . = (0x2000+(0x002B<<6))

CallPal_WrFen:
	or	$31	, 1, $13			 
	hw_mfpr	$1	, 0x118			 

	sll	$13	, 26, $13		 
	and	$16	, 1, $16			 

	sll	$16	, 26, $12		 
	bic	$1	, $13	, $1			 

	or	$1	, $12	, $1			 
	hw_mfpr	$12	, 0x157			 

	hw_mtpr	$1	, 0x118			 
	hw_stl/p	$16	, 0x0028($12	)	 

	hw_mfpr    $31, 0x140
	hw_mfpr    $31, 0x140
	hw_mfpr    $31, 0x140
					 
pvc$osf02$225:				 
	hw_rei				 

	
	    . = (0x2000+(0x002C<<6))

CallPal_OpcDec2C:
	br	$31	, CallPal_OpcDec































 
            . = (0x2000+(0x002D<<6))

CallPal_WrVptPtr:
	hw_mtpr	$16	, 0x208			 
	hw_mtpr	$16	, 0x113			 

	    bis	    $31, $31, $31
	    bis	    $31, $31, $31

	hw_rei
	
	    . = (0x2000+(0x002E<<6))

CallPal_OpcDec2E:
	br	$31	, CallPal_OpcDec
	
	    . = (0x2000+(0x002F<<6))

CallPal_OpcDec2F:
	br	$31	, CallPal_OpcDec


































 
            . = (0x2000+(0x0030<<6))

CallPal_SwpCtx:
	rpcc	$13				 
	hw_mfpr	$0	, 0x157			 

	hw_ldq/p	$22	, 0x0028($16	)	 
	hw_ldq/p	$23, 0x0018($16	)	 

	srl	$13	, 32, $25			 
	hw_mfpr	$24, 0x152			 

	hw_stq/p	$30	, 0x0000($0	)	 

                                         
                                         
pvc$osf03$379:				 
	hw_mtpr	$16	, 0x157			 

	hw_stq/p	$24, 0x0008($0	)	 
	addl	$13	, $25	, $25			 

	hw_stl/p	$25	, 0x0018($0	)	 
	bis	$31	, 1, $24		 

	sll	$24, 26, $24	 
	hw_mfpr	$25	, 0x118			 

	and	$22	, 1, $22			 
	br	$31	, CallPal_SwpCtxCont

































 
            . = (0x2000+(0x0031<<6))

CallPal_WrVal:
	    bis	    $31, $31, $31
	hw_mtpr	$16	, 0x151	

	    bis	    $31, $31, $31
	    bis	    $31, $31, $31

	hw_rei

































 
            . = (0x2000+(0x0032<<6))

CallPal_RdVal:
	    bis	    $31, $31, $31
	hw_mfpr	$0	, 0x151	

	    bis	    $31, $31, $31
	hw_rei










































 
            . = (0x2000+(0x0033<<6))

CallPal_Tbi:
	addq	$16	, 2, $16			 
	br	$23, 1f			 

1:	cmpult	$16	, 6, $22			 
	lda	$23, Tbi_Table-1b($23)	 

	sll	$16	, 4, $16			 
	blbc	$22	, 2f			 

	addq	$23, $16	, $23		 

pvc$osf04$2000:				 
	jmp	$31	, ($23)		 

2:	hw_rei
	    bis	    $31, $31, $31





































 
            . = (0x2000+(0x0034<<6))

CallPal_WrEnt:


	cmpult	$17, 7, $22			 



	br	$23, 1f			 

1:	bic	$16	, 3, $16			 
	blbc	$22	, 2f			 

	lda	$23, WrEnt_Table-1b($23)	 
	sll	$17, 4, $17		 

	addq	$17, $23, $23		 

pvc$osf05$3000:				 
	jmp	$31	, ($23)		 

2:	hw_rei


































 
            . = (0x2000+(0x0035<<6))

CallPal_SwpIpl:
	and	$16	, (7<<0), $16		 
	hw_mfpr	$22	, 0x148			 

	extbl	$22	, $16	, $22			 
	bis	$31	, $11	, $0			 

	bis	$31	, $16	, $11			 
	    bis	    $31, $31, $31				 

	hw_mtpr	$22	, 0x110				 

pvc$osf06$217:				 
	hw_rei
































 
            . = (0x2000+(0x0036<<6))

CallPal_RdPs:
	bis	$31	, $11	, $0			 
	    bis	    $31, $31, $31

	hw_rei



































 
            . = (0x2000+(0x0037<<6))

CallPal_WrKgp:
	    bis	    $31, $31, $31
	hw_mtpr	$16	, 0x156			 

	    bis	    $31, $31, $31				 
	    bis	    $31, $31, $31

	hw_rei

































 
            . = (0x2000+(0x0038<<6))

CallPal_WrUsp:
	    bis	    $31, $31, $31
	hw_mtpr	$16	, 0x152			 

	    bis	    $31, $31, $31				 
	    bis	    $31, $31, $31

	hw_rei

	
	    . = (0x2000+(0x0039<<6))

CallPal_OpcDec39:
	br	$31	, CallPal_OpcDec

































 
            . = (0x2000+(0x003A<<6))

CallPal_RdUsp:
	    bis	    $31, $31, $31
	hw_mfpr	$0	, 0x152			 

	hw_rei

	
	    . = (0x2000+(0x003B<<6))

CallPal_OpcDec3B:
	br	$31	, CallPal_OpcDec

































 
            . = (0x2000+(0x003C<<6))

CallPal_Whami:
	    bis	    $31, $31, $31
	hw_mfpr	$0	, 0x150			 

	extbl	$0	, 1, $0			 
	hw_rei




































 
            . = (0x2000+(0x003D<<6))

CallPal_RetSys:
	lda	$25	, 48($30	)	 
	bis	$31	, $25	, $14			 

	hw_mfpr	$14	, 0x10B			 
	ldq	$23, 0x0008($30	)	 

	ldq	$29	, 0x0010($30	)	 
	stl_c	$31	, -4($30	)		 

	lda	$11	, (1<<3)($31	)	 
	hw_mfpr	$30	, 0x152			 

	bic	$23, 3, $23		 
	hw_mtpr	$25	, 0x153			 

	hw_mtpr	$11	, 0x201			 
	hw_mtpr	$11	, 0x10F				 

	rc	$31				 
	hw_mtpr	$23, 0x10B			 

	hw_mtpr	$31	, 0x110			 

pvc$osf07$217:				 
	hw_rei 

	
	    . = (0x2000+(0x003E<<6))

CallPal_OpcDec3E:
	br	$31	, CallPal_OpcDec






































 
            . = (0x2000+(0x003F<<6))

CallPal_Rti:
	lda	$25	, 48($30	)	 
	bis	$31	, $25	, $14			 

	hw_mfpr	$14	, 0x10B			 
	rc	$31				 

	ldq	$12	, -6*8($25	)		 
	ldq	$13	, -5*8($25	)		 

	ldq	$18, -1*8($25	)		 
	ldq	$17, -2*8($25	)		 

	ldq	$16	, -3*8($25	)		 
	ldq	$29	, -4*8($25	)		 

	bic	$13	, 3, $13			 
	stl_c	$31	, -4($25	)		 

	and	$12	, (1<<3), $11			 
	hw_mtpr	$13	, 0x10B			 

	beq	$11	, CallPal_Rti_ToKern	 
	br	$31	, CallPal_Rti_ToUser 

















	.text	1

	. = 0x3000









































 
            . = (0x3000+((0x80&0x3F)<<6))

CallPal_Bpt:
	bis	$31	, $11	, $12			 
	hw_mtpr	$31	, 0x10F			 

	hw_mtpr	$31	, 0x201			 
	br	$31	, CallPal_BptCont








































 
            . = (0x3000+((0x81&0x3F)<<6))

CallPal_BugChk:
	bis	$31	, $11	, $12			 
	hw_mtpr	$31	, 0x10F			 

	hw_mtpr	$31	, 0x201			 
	br	$31	, CallPal_BugChkCont
	
	    . = (0x3000+((0x0082&0x3F)<<6))

CallPal_OpcDec82:
	br	$31	, CallPal_OpcDec





































 
            . = (0x3000+((0x83&0x3F)<<6))

CallPal_CallSys:
	and	$11	, (1<<3), $24	 
	hw_mfpr	$22	, 0x153			 

	beq	$24, CalPal_CallSys_FromKern
					 
	hw_mfpr	$23, 0x10B			 

	br	$31	, CallPal_CallSysCont
	
	    . = (0x3000+((0x0084&0x3F)<<6))

CallPal_OpcDec84:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x0085&0x3F)<<6))

CallPal_OpcDec85:
	br	$31	, CallPal_OpcDec





























 
            . = (0x3000+((0x86&0x3F)<<6))

CallPal_Imb:
	mb				 
	hw_mfpr	$31	, 0x20F			 

	    bis	    $31, $31, $31
	    bis	    $31, $31, $31

	br	$31	, CallPal_IcFlushCont	 

	
	    . = (0x3000+((0x0087&0x3F)<<6))

CallPal_OpcDec87:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x0088&0x3F)<<6))

CallPal_OpcDec88:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x0089&0x3F)<<6))

CallPal_OpcDec89:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x008A&0x3F)<<6))

CallPal_OpcDec8A:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x008B&0x3F)<<6))

CallPal_OpcDec8B:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x008C&0x3F)<<6))

CallPal_OpcDec8C:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x008D&0x3F)<<6))

CallPal_OpcDec8D:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x008E&0x3F)<<6))

CallPal_OpcDec8E:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x008F&0x3F)<<6))

CallPal_OpcDec8F:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x0090&0x3F)<<6))

CallPal_OpcDec90:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x0091&0x3F)<<6))

CallPal_OpcDec91:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x0092&0x3F)<<6))

CallPal_OpcDec92:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x0093&0x3F)<<6))

CallPal_OpcDec93:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x0094&0x3F)<<6))

CallPal_OpcDec94:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x0095&0x3F)<<6))

CallPal_OpcDec95:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x0096&0x3F)<<6))

CallPal_OpcDec96:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x0097&0x3F)<<6))

CallPal_OpcDec97:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x0098&0x3F)<<6))

CallPal_OpcDec98:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x0099&0x3F)<<6))

CallPal_OpcDec99:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x009A&0x3F)<<6))

CallPal_OpcDec9A:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x009B&0x3F)<<6))

CallPal_OpcDec9B:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x009C&0x3F)<<6))

CallPal_OpcDec9C:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x009D&0x3F)<<6))

CallPal_OpcDec9D:
	br	$31	, CallPal_OpcDec
































 
            . = (0x3000+((0x9E&0x3F)<<6))

CallPal_RdUnique:
	hw_mfpr	$0	, 0x157			 
	hw_ldq/p	$0	, 0x0020($0	)	 

	hw_rei

































 
            . = (0x3000+((0x9F&0x3F)<<6))

CallPal_WrUnique:
	    bis	    $31, $31, $31				 
	hw_mfpr	$12	, 0x157			 

	hw_stq/p	$16	, 0x0020($12	)	 
	    bis	    $31, $31, $31

	hw_rei

	
	    . = (0x3000+((0x00A0&0x3F)<<6))

CallPal_OpcDecA0:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x00A1&0x3F)<<6))

CallPal_OpcDecA1:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x00A2&0x3F)<<6))

CallPal_OpcDecA2:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x00A3&0x3F)<<6))

CallPal_OpcDecA3:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x00A4&0x3F)<<6))

CallPal_OpcDecA4:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x00A5&0x3F)<<6))

CallPal_OpcDecA5:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x00A6&0x3F)<<6))

CallPal_OpcDecA6:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x00A7&0x3F)<<6))

CallPal_OpcDecA7:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x00A8&0x3F)<<6))

CallPal_OpcDecA8:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x00A9&0x3F)<<6))

CallPal_OpcDecA9:
	br	$31	, CallPal_OpcDec








































 
            . = (0x3000+((0xAA&0x3F)<<6))

CallPal_GenTrap:
	bis	$31	, $11	, $12			 
	hw_mtpr	$31	, 0x10F			 

	hw_mtpr	$31	, 0x201			 
	br	$31	, CallPal_GenTrapCont
	
	    . = (0x3000+((0x00AB&0x3F)<<6))

CallPal_OpcDecAB:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x00AC&0x3F)<<6))

CallPal_OpcDecAC:
	br	$31	, CallPal_OpcDec
	









































	    . = (0x3000+((0xAD&0x3F)<<6))

CallPal_DbgStop:
	bis	$31	, $11	, $12			 
	hw_mtpr	$31	, 0x10F			 

	hw_mtpr	$31	, 0x201			 
	br	$31	, CallPal_DbgStopCont









	
	    . = (0x3000+((0x00AE&0x3F)<<6))

CallPal_OpcDecAE:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x00AF&0x3F)<<6))

CallPal_OpcDecAF:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x00B0&0x3F)<<6))

CallPal_OpcDecB0:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x00B1&0x3F)<<6))

CallPal_OpcDecB1:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x00B2&0x3F)<<6))

CallPal_OpcDecB2:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x00B3&0x3F)<<6))

CallPal_OpcDecB3:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x00B4&0x3F)<<6))

CallPal_OpcDecB4:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x00B5&0x3F)<<6))

CallPal_OpcDecB5:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x00B6&0x3F)<<6))

CallPal_OpcDecB6:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x00B7&0x3F)<<6))

CallPal_OpcDecB7:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x00B8&0x3F)<<6))

CallPal_OpcDecB8:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x00B9&0x3F)<<6))

CallPal_OpcDecB9:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x00BA&0x3F)<<6))

CallPal_OpcDecBA:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x00BB&0x3F)<<6))

CallPal_OpcDecBB:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x00BC&0x3F)<<6))

CallPal_OpcDecBC:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x00BD&0x3F)<<6))

CallPal_OpcDecBD:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x00BE&0x3F)<<6))

CallPal_OpcDecBE:
	br	$31	, CallPal_OpcDec
	
	    . = (0x3000+((0x00BF&0x3F)<<6))

CallPal_OpcDecBF:
	br	$31	, CallPal_OpcDec





	.text	2

	. = 0x4000




CallPal_SwpPalCont:
	hw_mfpr	$1	, 0x150    		 
	zap	$1	, 0x40, $1			 

	lda	$0	, 0x53    ($31	)	 
	sll	$0	, 48, $0		 

	bis	$1	, $0	, $1			 
	hw_mtpr	$1	, 0x150    		 

	or	$31	, $31	, $0			 
pvc$osf08$1007:				 
	jmp	$31	, ($16	)		 






	    .align  5

CallPal_SwpCtxCont:
	sll	$22	, 26, $22		 
	bic	$25	, $24, $25			 

	srl	$23, 32, $24		 
	hw_ldq/p	$14	, 0x0010($16	)	 

	sll	$24, 4, $12			 
	bis	$25	, $22	, $25			 

	sll	$24, 57, $24		 
	subl	$23, $13	, $13			 

	hw_mtpr	$24, 0x200			 
	hw_mtpr	$12	, 0x103	





	srl	$14	, 63, $12	
	sll	$14	, 13, $14		 
	bis	$14	, $12	, $14	
	hw_mtpr	$25	, 0x118			 

	insll	$13	, 4, $13			 
	hw_ldq/p	$25	, 0x0008($16	)	 

	hw_mtpr	$13	, 0x20D				 

                                         
                                         
pvc$osf09$379:				 
	hw_mtpr	$14	, 0x154			 

	hw_ldq/p	$30	, 0x0000($16	)	 

                                         
                                         
pvc$osf10$379:				 
	hw_mtpr	$25	, 0x152			 

	hw_rei_stall






	    .align  5

Tbi_Table:



pvc$osf11$2000.1:			 
	hw_mtpr	$31	, 0x20A			 
	hw_mtpr	$31	, 0x105			 
	hw_rei_stall
	    bis	    $31, $31, $31				 



pvc$osf12$2000.2:			 
	hw_mtpr	$31	, 0x209			 
	hw_mtpr	$31	, 0x106			 
	hw_rei_stall
	    bis	    $31, $31, $31				 



pvc$osf13$2000.3:			 
	hw_rei				 
	    bis	    $31, $31, $31				 
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31



pvc$osf14$2000.4:			 
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	hw_mtpr	$17, 0x107			 
	hw_rei_stall



pvc$osf15$2000.5:			 
	hw_mtpr	$17, 0x20B			 
	    bis	    $31, $31, $31				 
	    bis	    $31, $31, $31
	br	$31	, OneCycle_Then_hw_rei



pvc$osf16$2000.6:			 
	hw_mtpr	$17, 0x20B			 
	    bis	    $31, $31, $31
	hw_mtpr	$17, 0x107			 
	hw_rei_stall





	    .align  5

CallPal_IcFlushCont:
	    bis	    $31, $31, $31
	hw_mtpr	$31	, 0x119			 
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31



	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31

	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31

	    bis	    $31, $31, $31
	    bis	    $31, $31, $31				 

	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31

	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31				 

	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31

	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31				 

	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31

	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31

	    bis	    $31, $31, $31
	    bis	    $31, $31, $31				 

	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
OneCycle_Then_hw_rei:
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	hw_rei





	    .align  5

WrEnt_Table:
pvc$osf17$3000.1:			 
	    bis	    $31, $31, $31
	hw_mtpr	$16	, 0x14B			 
	hw_mfpr    $31, 0x140
	hw_rei

pvc$osf18$3000.2:			 
	    bis	    $31, $31, $31
	hw_mtpr	$16	, 0x14C			 
	hw_mfpr    $31, 0x140
	hw_rei

pvc$osf19$3000.3:			 
	    bis	    $31, $31, $31
	hw_mtpr	$16	, 0x155			 
	hw_mfpr    $31, 0x140
	hw_rei

pvc$osf20$3000.4:			 
	    bis	    $31, $31, $31
	hw_mtpr	$16	, 0x147			 
	hw_mfpr    $31, 0x140
	hw_rei

pvc$osf21$3000.5:			 
	    bis	    $31, $31, $31
	hw_mtpr	$16	, 0x142			 
	hw_mfpr    $31, 0x140
	hw_rei

pvc$osf22$3000.6:			 
	    bis	    $31, $31, $31
	hw_mtpr	$16	, 0x149			 
	hw_mfpr    $31, 0x140
	hw_rei



pvc$osf23$3000.7:			 
	    bis	    $31, $31, $31
	hw_mtpr	$16	, 0x14D			 
	hw_mfpr    $31, 0x140
	hw_rei








	    .align  5

CallPal_Rti_ToKern:
	and	$12	, (7<<0), $11		 
	hw_mfpr	$12	, 0x148			 

	extbl	$12	, $11	, $12			 
	hw_mtpr	$25	, 0x153			 

	hw_mtpr	$12	, 0x110				 
	bis	$31	, $25	, $30			 

pvc$osf24$217:				 
	hw_rei

	    .align  5

CallPal_Rti_ToUser:
	hw_mtpr	$11	, 0x201			 
	hw_mtpr	$11	, 0x10F				 

	hw_mfpr	$30	, 0x152			 
	hw_mtpr	$31	, 0x110			 

	hw_mtpr	$25	, 0x153			 

pvc$osf25$248:				 
	hw_rei






	    .align  5

CallPal_BptCont:
	hw_mfpr	$14	, 0x10B			 

		sll	$11	, 63-3, $25	;		bge	$25	, 0f;			bis	$31	, $31	, $11	;			hw_mtpr	$30	, 0x152	;			hw_mfpr	$30	, 0x153	;		0:	lda	$30	, 0-48($30	);		stq	$12	, 0x0000($30	);		stq	$14	, 0x0008($30	);		stq	$29	, 0x0010($30	);		stq	$16	, 0x0018($30	);		stq	$17, 0x0020($30	);		stq	$18, 0x0028($30	)

	hw_mfpr	$13	, 0x147			 

	bis	$31	, 0x0, $16		 
	hw_mtpr	$13	, 0x10B			 

	hw_mfpr	$29	, 0x156			 
	hw_rei





	    .align  5

CallPal_BugChkCont:
	hw_mfpr	$14	, 0x10B			 

		sll	$11	, 63-3, $25	;		bge	$25	, 0f;			bis	$31	, $31	, $11	;			hw_mtpr	$30	, 0x152	;			hw_mfpr	$30	, 0x153	;		0:	lda	$30	, 0-48($30	);		stq	$12	, 0x0000($30	);		stq	$14	, 0x0008($30	);		stq	$29	, 0x0010($30	);		stq	$16	, 0x0018($30	);		stq	$17, 0x0020($30	);		stq	$18, 0x0028($30	)

	hw_mfpr	$13	, 0x147			 

	bis	$31	, 0x1, $16		 
	hw_mtpr	$13	, 0x10B			 

	hw_mfpr	$29	, 0x156			 
	hw_rei






	    .align  5

CallPal_CallSysCont:

	hw_mtpr	$31	, 0x10F			 

	hw_mtpr	$31	, 0x201			 

		sll	$11	, 63-3, $25	;		bge	$25	, 0f;			bis	$31	, $31	, $11	;			hw_mtpr	$30	, 0x152	;			hw_mfpr	$30	, 0x153	;		0:	lda	$30	, 0-48($30	);		stq	$24, 0x0000($30	);		stq	$23, 0x0008($30	);		stq	$29	, 0x0010($30	);		stq	$16	, 0x0018($30	);		stq	$17, 0x0020($30	);		stq	$18, 0x0028($30	)

	hw_mfpr	$12, 0x149			 

	hw_mfpr	$29	, 0x156			 
	hw_mtpr	$12, 0x10B			 
	hw_mfpr    $31, 0x140
	hw_rei

	    .align  5

CalPal_CallSys_FromKern:

	hw_mfpr	$14	, 0x10B			 
	subq	$14	, 4, $14			 

	br	$31	, MchkOSBugCheck






	    .align  5

CallPal_GenTrapCont:
	hw_mfpr	$14	, 0x10B			 

		sll	$11	, 63-3, $25	;		bge	$25	, 0f;			bis	$31	, $31	, $11	;			hw_mtpr	$30	, 0x152	;			hw_mfpr	$30	, 0x153	;		0:	lda	$30	, 0-48($30	);		stq	$12	, 0x0000($30	);		stq	$14	, 0x0008($30	);		stq	$29	, 0x0010($30	);		stq	$16	, 0x0018($30	);		stq	$17, 0x0020($30	);		stq	$18, 0x0028($30	)

	hw_mfpr	$13	, 0x147			 

	bis	$31	, 0x2, $16		 
	hw_mtpr	$13	, 0x10B			 

	hw_mfpr	$29	, 0x156			 
	hw_rei






	    .align  5

CallPal_DbgStopCont:
	hw_mfpr	$14	, 0x10B			 

		sll	$11	, 63-3, $25	;		bge	$25	, 0f;			bis	$31	, $31	, $11	;			hw_mtpr	$30	, 0x152	;			hw_mfpr	$30	, 0x153	;		0:	lda	$30	, 0-48($30	);		stq	$12	, 0x0000($30	);		stq	$14	, 0x0008($30	);		stq	$29	, 0x0010($30	);		stq	$16	, 0x0018($30	);		stq	$17, 0x0020($30	);		stq	$18, 0x0028($30	)

	hw_mfpr	$13	, 0x14D			 

	bis	$31	, $31	, $16			 
	hw_mtpr	$13	, 0x10B			 

	hw_mfpr	$29	, 0x156			 
	hw_rei
































 
            .align  5

CallPal_OpcDec:
	bis	$31	, $11	, $12			 
	hw_mtpr	$31	, 0x10F			 

	hw_mtpr	$31	, 0x201			 
	hw_mfpr	$14	, 0x10B			 

		sll	$11	, 63-3, $25	;		bge	$25	, 0f;			bis	$31	, $31	, $11	;			hw_mtpr	$30	, 0x152	;			hw_mfpr	$30	, 0x153	;		0:	lda	$30	, 0-48($30	);		stq	$12	, 0x0000($30	);		stq	$14	, 0x0008($30	);		stq	$29	, 0x0010($30	);		stq	$16	, 0x0018($30	);		stq	$17, 0x0020($30	);		stq	$18, 0x0028($30	)

	hw_mfpr	$13	, 0x147			 

	bis	$31	, 0x4, $16		 
	hw_mtpr	$13	, 0x10B			 

	hw_mfpr	$29	, 0x156			 
	hw_rei				 




	
	.text 3

	. = 0x5000

ResetContinue:
	hw_mtpr	$0	, 0x140			 

	hw_mtpr	$31	, 0x105			 
	hw_mtpr	$31	, 0x20A			 

	lda	$1	, -8($1	)		 
	hw_mtpr	$1	, 0x10E			 
	hw_mfpr    $31, 0x140
	hw_mfpr    $31, 0x140





	hw_mfpr	$1	, 0x10E			 
	hw_ldl/p	$0	, 0x14($1	)		 
	addq	$0	, $1	, $0			 

	lda	$0	, 0x1FFF($0	)		 
	srl	$0	, 13, $0			 
	sll	$0	, 13, $0			 

	hw_mtpr	$0	, 0x143			 

	lda	$0	, 0x0000($31	)	 
	hw_mtpr	$0	, 0x145			 
	hw_mfpr    $31, 0x140
	hw_mfpr    $31, 0x140
	hw_mfpr	$1	, 0x143			 
		lda	$1	, 0x200($1	);                  	hw_mfpr	$0	, 0x140;                        	 	hw_stq/p	$0	, ((0x110	+0x00-0x200)&0x3FF)($1	);         	hw_mfpr	$0	, 0x144;                        	 	hw_stq/p	$0	, ((0x110	+0x08-0x200)&0x3FF)($1	);         	 	hw_stq/p	$2, ((0x110	+0x10-0x200)&0x3FF)($1	);         	 	hw_stq/p	$3, ((0x110	+0x18-0x200)&0x3FF)($1	);         	 	hw_stq/p	$4, ((0x110	+0x20-0x200)&0x3FF)($1	);         	 	hw_stq/p	$5, ((0x110	+0x28-0x200)&0x3FF)($1	);          	hw_mfpr	$5, 0x118	;                       	ldah	$4, (1<<(30-16))($31	); 	bic	$5, $4, $4;                     	hw_mtpr	$4, 0x118	;                       	hw_mfpr    $31, 0x140;                                  	hw_mfpr    $31, 0x140;                                  	hw_mfpr    $31, 0x140;                                  	    bis	    $31, $31, $31;                                    	 	hw_stq/p	$6, ((0x110	+0x30-0x200)&0x3FF)($1	);       	 	hw_stq/p	$7, ((0x110	+0x38-0x200)&0x3FF)($1	);       	 	hw_stq/p	$8, ((0x110	+0x40-0x200)&0x3FF)($1	);       	 	hw_stq/p	$9, ((0x110	+0x48-0x200)&0x3FF)($1	);       	 	hw_stq/p	$10, ((0x110	+0x50-0x200)&0x3FF)($1	);      	 	hw_stq/p	$11, ((0x110	+0x58-0x200)&0x3FF)($1	);      	 	hw_stq/p	$12, ((0x110	+0x60-0x200)&0x3FF)($1	);      	 	hw_stq/p	$13, ((0x110	+0x68-0x200)&0x3FF)($1	);      	 	hw_stq/p	$14, ((0x110	+0x70-0x200)&0x3FF)($1	);      	 	hw_stq/p	$15, ((0x110	+0x78-0x200)&0x3FF)($1	);      	 	hw_stq/p	$16, ((0x110	+0x80-0x200)&0x3FF)($1	);      	 	hw_stq/p	$17, ((0x110	+0x88-0x200)&0x3FF)($1	);      	 	hw_stq/p	$18, ((0x110	+0x90-0x200)&0x3FF)($1	);      	 	hw_stq/p	$19, ((0x110	+0x98-0x200)&0x3FF)($1	);      	 	hw_stq/p	$20, ((0x110	+0xA0-0x200)&0x3FF)($1	);      	 	hw_stq/p	$21, ((0x110	+0xA8-0x200)&0x3FF)($1	);      	 	hw_stq/p	$22, ((0x110	+0xB0-0x200)&0x3FF)($1	);      	 	hw_stq/p	$23, ((0x110	+0xB8-0x200)&0x3FF)($1	);      	 	hw_stq/p	$24, ((0x110	+0xC0-0x200)&0x3FF)($1	);      	 	hw_stq/p	$25, ((0x110	+0xC8-0x200)&0x3FF)($1	);      	 	hw_stq/p	$26, ((0x110	+0xD0-0x200)&0x3FF)($1	);      	 	hw_stq/p	$27, ((0x110	+0xD8-0x200)&0x3FF)($1	);      	 	hw_stq/p	$28, ((0x110	+0xE0-0x200)&0x3FF)($1	);      	 	hw_stq/p	$29, ((0x110	+0xE8-0x200)&0x3FF)($1	);      	 	hw_stq/p	$30, ((0x110	+0xF0-0x200)&0x3FF)($1	);      	 	hw_stq/p	$31, ((0x110	+0xF8-0x200)&0x3FF)($1	);      	hw_mfpr    $31, 0x140;                                  	hw_mfpr    $31, 0x140;                                  	hw_mtpr	$5, 0x118	;                       	hw_mfpr    $31, 0x140;                                  	hw_mfpr    $31, 0x140;                                  	hw_mfpr    $31, 0x140;                                  	    bis	    $31, $31, $31;                                    	lda	$1	, -0x200($1	)			 

	hw_mtpr	$31	, 0x109			 
	hw_mtpr	$31	, 0x10A			 

	hw_mtpr	$31	, 0x108			 
	ldah	$1	, 	((1<<(30-16)) | 	 (2<<(28-16)) | 	 (1<<(26-16)) | 	 (1<<(19-16)) | 	 (1<<(17-16)))($31	)	 

	hw_mtpr	$1	, 0x118			 
	br	$31	, set_icsr

	    .align  5



set_icsr:
	    bis	    $31, $31, $31
	hw_mtpr	$31	, 0x119			 
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31

	    bis	    $31, $31, $31                            
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31

	    bis	    $31, $31, $31                            
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31

	    bis	    $31, $31, $31                            
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31

	    bis	    $31, $31, $31                            
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31



	    bis	    $31, $31, $31				 
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31


	lda	$1	, (2<<1)($31	)  
	hw_mtpr	$1	, 0x20F			 
	lda	$1	, (1<<0)($31	)	 
	lda	$1	, (1<<3)($1	)	 
	hw_mtpr	$1	, 0x216			 
	    bis	    $31, $31, $31

	hw_mfpr    $31, 0x140				 
	hw_mfpr    $31, 0x140
	hw_mfpr    $31, 0x140
	hw_mfpr    $31, 0x140

	hw_mtpr	$31	, 0x210			 

	lda	$11	, 0x7($31	)	 
	lda	$1	, 0x1F($31	)		 
	hw_mtpr	$1	, 0x110	
	hw_mtpr	$31	, 0x10F			 
	hw_mtpr	$31	, 0x201			 

	hw_mtpr	$31	, 0x10C			 
	hw_mfpr	$31	, 0x206			 

	lda	$1	, ((1<<13) | (1<<12) | (1<<11))($31	)
	hw_mtpr	$1	, 0x11A			 
	lda	$1	, ((1<<1) | (1<<0))($31	)	
	hw_mtpr	$1	, 0x212			 

	rc	$0				 
	hw_mtpr	$31	, 0x14B



	hw_mfpr	$0	, 0x150    		 
	srl	$0	, 48, $1		 
	zap	$0	, 0x40, $0			 
	hw_mtpr	$0	, 0x150    		 
	lda	$12	, 0x53    ($31	)	 
	cmpeq	$1	, $12	, $1			 

	blbs	$1	, 1f			 



	hw_mtpr	$31	, 0x200	

	hw_mtpr	$31	, 0x103	
	lda	$1	, 0x67($31	)

	sll	$1	, 27, $1	
	hw_mtpr	$1	, 0x115			 

	lda	$0	, (1<<3)($31	)  
	hw_mtpr	$0	, 0x150    		 
	hw_mfpr    $31, 0x140
	hw_mfpr    $31, 0x140

	or	$31	, 1, $1			 
	sll	$1	, 32, $1			 
	hw_mtpr	$31	, 0x20D			 

	hw_mtpr	$1	, 0x20E			 
	hw_mtpr	$31	, 0x151			 

	hw_mtpr	$31	, 0x217			 
	hw_mtpr	$31	, 0x152			 
	bis	$31	, 1, $1			 
	hw_mtpr	$1	, 0x156			 

	lda	$1	, 0x2($31	)		 
	sll	$1	, 32, $1			 
	hw_mtpr	$1	, 0x208			 
	hw_mtpr	$1	, 0x113	
	hw_mtpr	$31	, 0x154	

	ldah	$0	, 0xFFF0($31	)	 
	zap	$0	, 0xE0, $0			 

	hw_ldq/p	$31	, 0x108	($0	)	 




















	srl	$19, 16, $12			 
	zap	$12	, 0xFC, $12			 
		ldah	$13	, ((0xDECB+0x8000) >> 16)($31	); 	lda	$13	, (0xDECB&0xffff)($13	)			 
	cmpeq	$12	, $13	, $12			 
	beq	$12	, 2f			 
	hw_mfpr	$12	, 0x143			 
	hw_mfpr	$1	, 0x144			 
	lda	$13	, 0x418	($12	)	 

	  	hw_stq/p	$1	, ((0x518-0x418	)&0x3FF)($13	)	 
	  	hw_stq/p	$2, ((0x510-0x418	)&0x3FF)($13	)		 
	  	hw_stq/p	$3, ((0x560-0x418	)&0x3FF)($13	)	 

	  	hw_stq/p	$15, ((0x528-0x418	)&0x3FF)($13	)	 
	  	hw_stq/p	$16	, ((0x530-0x418	)&0x3FF)($13	)	 
	  	hw_stq/p	$17, ((0x538-0x418	)&0x3FF)($13	)	 
	  	hw_stq/p	$18, ((0x540-0x418	)&0x3FF)($13	)	 
	  	hw_stq/p	$19, ((0x548-0x418	)&0x3FF)($13	)	 
	  	hw_stq/p	$20, ((0x550-0x418	)&0x3FF)($13	)	 
	  	hw_stq/p	$21, ((0x558-0x418	)&0x3FF)($13	)		 

	bis	$31, $31, $19			 




2:
	hw_mfpr	$12	, 0x143			 

	lda	$13	, 0xA0($12	)	 
	hw_mtpr	$13	, 0x157			 

pvc$osf27$5010:				 
	bsr	$13	, Sys_Reset		 

	hw_mfpr	$14	, 0x143	
	hw_stq/pc	$31	, 0x100($14	)	 

	hw_mfpr	$1	, 0x143			 
		lda	$1	, 0x200($1	);                  	hw_mfpr	$0	, 0x118	;                       	ldah	$2, (1<<(30-16))($31	); 	bic	$0	, $2, $2;                     	hw_mtpr	$2, 0x118	;                       	hw_mfpr    $31, 0x140;                                  	hw_mfpr    $31, 0x140;                                  	hw_mfpr    $31, 0x140;                                  	    bis	    $31, $31, $31;                                    		hw_ldq/p	$2, ((0x110	+0x10-0x200)&0x3FF)($1	);       		hw_ldq/p	$3, ((0x110	+0x18-0x200)&0x3FF)($1	);       		hw_ldq/p	$4, ((0x110	+0x20-0x200)&0x3FF)($1	);       		hw_ldq/p	$5, ((0x110	+0x28-0x200)&0x3FF)($1	);       		hw_ldq/p	$6, ((0x110	+0x30-0x200)&0x3FF)($1	);       		hw_ldq/p	$7, ((0x110	+0x38-0x200)&0x3FF)($1	);       		hw_ldq/p	$8, ((0x110	+0x40-0x200)&0x3FF)($1	);       		hw_ldq/p	$9, ((0x110	+0x48-0x200)&0x3FF)($1	);       		hw_ldq/p	$10, ((0x110	+0x50-0x200)&0x3FF)($1	);      		hw_ldq/p	$11, ((0x110	+0x58-0x200)&0x3FF)($1	);      		hw_ldq/p	$12, ((0x110	+0x60-0x200)&0x3FF)($1	);      		hw_ldq/p	$13, ((0x110	+0x68-0x200)&0x3FF)($1	);      		hw_ldq/p	$14, ((0x110	+0x70-0x200)&0x3FF)($1	);      		hw_ldq/p	$15, ((0x110	+0x78-0x200)&0x3FF)($1	);      		hw_ldq/p	$16, ((0x110	+0x80-0x200)&0x3FF)($1	);      		hw_ldq/p	$17, ((0x110	+0x88-0x200)&0x3FF)($1	);      		hw_ldq/p	$18, ((0x110	+0x90-0x200)&0x3FF)($1	);      		hw_ldq/p	$19, ((0x110	+0x98-0x200)&0x3FF)($1	);      		hw_ldq/p	$20, ((0x110	+0xA0-0x200)&0x3FF)($1	);      		hw_ldq/p	$21, ((0x110	+0xA8-0x200)&0x3FF)($1	);      		hw_ldq/p	$22, ((0x110	+0xB0-0x200)&0x3FF)($1	);      		hw_ldq/p	$23, ((0x110	+0xB8-0x200)&0x3FF)($1	);      		hw_ldq/p	$24, ((0x110	+0xC0-0x200)&0x3FF)($1	);      		hw_ldq/p	$25, ((0x110	+0xC8-0x200)&0x3FF)($1	);      		hw_ldq/p	$26, ((0x110	+0xD0-0x200)&0x3FF)($1	);      		hw_ldq/p	$27, ((0x110	+0xD8-0x200)&0x3FF)($1	);      		hw_ldq/p	$28, ((0x110	+0xE0-0x200)&0x3FF)($1	);      		hw_ldq/p	$29, ((0x110	+0xE8-0x200)&0x3FF)($1	);      		hw_ldq/p	$30, ((0x110	+0xF0-0x200)&0x3FF)($1	);      		hw_ldq/p	$31, ((0x110	+0xF8-0x200)&0x3FF)($1	);      	hw_mfpr    $31, 0x140;                                   	hw_mfpr    $31, 0x140;                                   	hw_mtpr	$0	, 0x118	;                        	hw_mfpr    $31, 0x140;                                   	hw_mfpr    $31, 0x140;                                   	hw_mfpr    $31, 0x140;                                   	    bis	    $31, $31, $31;                                     		hw_ldq/p	$0, ((0x110	+0x00-0x200)&0x3FF)($1	);       		hw_ldq/p	$1, ((0x110	+0x08-0x200)&0x3FF)($1	);			 
					 
	hw_mfpr	$0	, 0x145			 
	br	$31	, Sys_EnterConsole











	    .align  3





1:
	hw_mfpr	$1	, 0x10E			 
	hw_ldl/p	$12	, 0x14($1	)		 
	addq	$12	, $1	, $12			 
	
	lda	$12	, 0x1FFF($12	)		 
	srl	$12	, 13, $12			 
	sll	$12	, 13, $12			 

	hw_mtpr	$12	, 0x143			 

	rpcc	$1				 

	hw_ldq/p	$22	, 0x0028($18)	 
	hw_ldl/p	$23, 0x0018($18)	 
	hw_ldl/p	$24, 0x001C($18)	 





	hw_ldq/p	$25	, 0x0010($18)	 
	srl	$25	, 63, $13	
	sll	$25	, 13, $25		 
	bis	$25	, $13	, $25	
	hw_mtpr	$25	, 0x154			 
	hw_mtpr	$18, 0x157			 

	bic	$17, 3, $17		 
	hw_mtpr	$17, 0x10B			 
	hw_mtpr	$19, 0x113	
	hw_mtpr	$19, 0x208	

	hw_ldq/p	$30	, 0x0008($18)	 
	hw_mtpr	$30	, 0x152			 

	sll	$24, 57, $8	
	hw_mtpr	$8	, 0x200	
	sll	$24, 4, $24
	hw_mtpr	$24, 0x103	

	hw_mfpr	$25	, 0x118			 
	lda	$24, 1($31	)		 
	sll	$24, 26, $24	 
	bic	$25	, $24, $25			 
	and	$22	, 1, $22			 
	sll	$22	, 26, $22		 
	bis	$22	, $25	, $25			 
	hw_mtpr	$25	, 0x118			 

	subl	$23, $1	, $1			 
	insll	$1	, 4, $1			 
	hw_mtpr	$1	, 0x20D				 

pvc$osf48$5060:				 
	bsr	$0	, Sys_ResetViaSwppal	 

	bis	$31	, $31	, $0			 
	hw_ldq/p	$30	, 0x0000($18)	 

	hw_mfpr    $31, 0x140
	hw_rei_stall











































 
	    .align  5

PteInvalid:
	bis	$31	, $11	, $12			 
	hw_mtpr	$31	, 0x10F			 

	hw_mfpr	$14	, 0x146			 

	blbs	$14	, KspNotValid		 

	hw_mtpr	$31	, 0x201			 
	beq	$9	, 1f			 

	hw_mfpr	$9	, 0x205			 
	srl	$9	, 11, $25		 

	cmpeq	$25	, 0x18, $25			 

	    bis	    $31, $31, $31				 
	blbs	$25	, 5f			 

1:	bis	$31	, $10	, $13			 
	    bis	    $31, $31, $31				 

		sll	$11	, 63-3, $25	;		bge	$25	, 0f;			bis	$31	, $31	, $11	;			hw_mtpr	$30	, 0x152	;			hw_mfpr	$30	, 0x153	;		0:	lda	$30	, 0-48($30	);		stq	$12	, 0x0000($30	);		stq	$14	, 0x0008($30	);		stq	$29	, 0x0010($30	);		stq	$16	, 0x0018($30	);		stq	$17, 0x0020($30	);		stq	$18, 0x0028($30	)

	bge	$25	, 2f			 
	srl	$8	, 9-8, $8	  

2:	lda	$17, 0x1($31	)	 
	srl	$8	, 8, $8		 

	hw_mfpr	$25	, 0x155			 
	    bis	    $31, $31, $31				 

	blbc	$9	, 3f			 
	srl	$8	, 12-8, $8	  

3:	cmovlbs $8	, 0x0, $17	 
	bis	$31	, $13	, $16			 

	and	$9	, 1, $18		 
	    bis	    $31, $31, $31				 

	bne	$9	, 4f			 
	subq	$31	, 1, $18		 

4:	hw_mtpr	$25	, 0x10B			 

	hw_mfpr	$29	, 0x156			 

	hw_rei










	    .align  3

5:	addq	$12	, 4, $12			 
	hw_mtpr	$12	, 0x10B			 
	hw_mfpr    $31, 0x140
	hw_rei




































 
	    .align  5

ItbFoe:
	bis	$31	, $11	, $12			 
	hw_mtpr	$31	, 0x10F			 

	hw_mtpr	$31	, 0x201			 
	bis	$31	, $10	, $13			 
	
		sll	$11	, 63-3, $25	;		bge	$25	, 0f;			bis	$31	, $31	, $11	;			hw_mtpr	$30	, 0x152	;			hw_mfpr	$30	, 0x153	;		0:	lda	$30	, 0-48($30	);		stq	$12	, 0x0000($30	);		stq	$13	, 0x0008($30	);		stq	$29	, 0x0010($30	);		stq	$16	, 0x0018($30	);		stq	$17, 0x0020($30	);		stq	$18, 0x0028($30	)
	
	bge	$25	, 1f			 
	srl	$8	, 9-8, $8	  

1:	lda	$17, 0x1($31	)	 
	srl	$8	, 8, $8		 

	hw_mfpr	$25	, 0x155			 
	bis	$31	, $13	, $16			 

	cmovlbs	$8	, 0x3, $17	 
	subq	$31	, 1, $18		 

	hw_mtpr	$25	, 0x10B			 

	hw_mfpr	$29	, 0x156			 

	hw_rei






































 
	    .align  5

FakePte:
	srl	$23, 8, $23	 
	hw_mfpr	$22	, 0x10B			 

	lda	$22	, 4($22	)		 
	lda	$8	, 0x3300($31	)	 

	cmovlbc	$23, $31	, $8			 
	hw_mtpr	$22	, 0x10B			 

	hw_mfpr	$22	, 0x144			 
	hw_mfpr	$23, 0x145

	hw_rei				 
















	    .align  5

DfaultContinue:
	blbs	$8	, 1f			 
	bis	$31	, $8	, $14			 

	cmpeq	$9	, 0x18, $10			 
	bne	$10	, 2f			 

	hw_mfpr	$9	, 0x206				 

		sll	$11	, 63-3, $25	;		bge	$25	, 0f;			bis	$31	, $31	, $11	;			hw_mtpr	$30	, 0x152	;			hw_mfpr	$30	, 0x153	;		0:	lda	$30	, 0-48($30	);		stq	$12	, 0x0000($30	);		stq	$14	, 0x0008($30	);		stq	$29	, 0x0010($30	);		stq	$16	, 0x0018($30	);		stq	$17, 0x0020($30	);		stq	$18, 0x0028($30	)

	and	$13	, (1<<0), $18	 

	bis	$31	, $9	, $16			 
	srl	$13	, 1, $17	 

	hw_mfpr	$25	, 0x155			 
	cmovlbs	$17, 0x1, $17	 

	hw_mtpr	$25	, 0x10B			 

	hw_mfpr	$29	, 0x156			 

	hw_rei



















	    .align  3

1:	bic	$14	, 3, $8		 
	hw_mfpr	$10	, 0x10E			 

	hw_mfpr	$31	, 0x206			 

	subq	$10	, $8, $8		 




	lda	$10	, (Itb_LdVpte-Pal_Base)($8)  
	    bis	    $31, $31, $31

	beq	$10	, MchkBugCheck
	lda	$10	, (Dtb_LdVpte-Pal_Base)($8)  

	beq	$10	, MchkBugCheck






KspNotValid:
	bic	$11	, (1<<3), $11			 

	hw_mtpr	$0	, 0x140			 
	hw_mtpr	$11	, 0x201			 

	hw_mtpr	$11	, 0x10F				 

	hw_mtpr	$14	, 0x10B			 

pvc$osf29$5000:				 
	bsr	$0	, UpdatePCB		 

	lda	$0	, 0x0002($31	)  

	br	$31	, Sys_EnterConsole












	    .align  3

2:	hw_mtpr	$11	, 0x201			 
	hw_mtpr	$11	, 0x10F				 

	hw_mfpr	$31	, 0x206			 
	addq	$14	, 4, $14			 

	hw_mtpr	$14	, 0x10B			 
	hw_mfpr    $31, 0x140

	hw_rei





	    .align  5

InterruptContinue:
	    bis	    $31, $31, $31				 
	hw_mfpr	$29	, 0x156			 




	cmpeq	$13	, 31, $12			 
	bne	$12	, ErrorHandler



	cmpeq	$13	, 30, $12			 
	bne	$12	, PowerFailHandler



	cmpeq	$13	, 29, $12			 
	bne	$12	, PerfHandler



	br	$31	, Sys_Interrupt




	    .align  3

PassiveRelease:
	hw_mtpr	$11	, 0x201			 
	    bis	    $31, $31, $31
	hw_mfpr    $31, 0x140				 
	hw_rei				 




	    .align  3

HaltHandler:
	hw_mtpr	$11	, 0x201			 
	hw_mtpr	$0	, 0x140			 

pvc$osf28$5000:
	bsr	$0	, UpdatePCB		 
	lda	$0	, 0x0001($31	)	 
	br	$31	, Sys_EnterConsole	 

	    .align  3

ErrorHandler:
	hw_mfpr	$12	, 0x148			 
	extbl	$12	, 0x7, $13		 

	bis	$31	, 0x7, $11		 
	hw_mtpr	$13	, 0x110				 

	srl	$25	, 31, $12		 
	blbs	$12	, Sys_IntMchkHandler	 

	srl	$25	, 32, $12		 
	blbs	$12	, IntCrdHandler	 

	br	$31	, CallPal_Rti	 






	    .align  3

IntCrdHandler:
	mb				 

	hw_mtpr	$0	, 0x140			 
	hw_mtpr	$1	, 0x141			 

	ldah	$14	, 0xFFF0($31	)
	zap	$14	, 0xE0, $14			 

	hw_ldq/p	$0	, 0x088	($14	)		 
	bis	$0	, $10	, $31			 



pvc$osf100$5100:			 
	bsr	$13	, CrdScrubMemory
					 
	hw_ldq/p	$9	, 0x108	($14	)		 
	hw_mfpr	$13	, 0x150			 
	srl	$13	, 3, $14		 
	blbs	$14	, CrdAck		 

	bis	$13	, (1<<2), $14		 
	lda	$1	, 0x0630($31	)	 
	sll	$1	, 16, $1		 
	
	zap	$14	, 0x3C, $14			 
	or	$14	, $1	, $14			 

	lda	$1	, 0x0086($31	)	 
	sll	$1	, 32, $1		 
	or	$14	, $1	, $14			 
	hw_mtpr	$14	, 0x150    		 




















	hw_mfpr	$14	, 0x143			 

		ldah	$14	, ((0x568+0x8000) >> 16)($31	); 	lda	$14	, (0x568&0xffff)($14	)		 

	srl	$13	, 2, $1		 
	blbc	$1	, 1f			 
	lda	$1	, 3($31	)		 
	sll	$1	, 30, $1			 
	hw_stl/p	$1	, 0x0004($14	)	 
	br	$31	, CrdAck		 



1:	lda	$1	, 0x1($31	)		   
	sll	$1	, 63, $1			   
	lda	$1	, 0x0038($1	)	   
	hw_stq/p	$1	, 0x0000($14	)	   
	lda	$1	, 0x0038($31	)   
	sll	$1	, 32, $1			   
	lda	$1	, 0x0018($1	)	   
	hw_stq/p	$1	, 0x0008($14	)  




	hw_mfpr	$1	, 0x150    		 
	extwl	$1	, 4, $1			 

	hw_stq/p	$1	, 0x0010($14	)	 

	hw_stq/p	$0	, 0x0018($14	)	 
	hw_stq/p	$9	, 0x0028($14	)	 
	hw_stq/p	$25	, 0x0030($14	)	 

	br	$31	, CrdAck		 

	    .align  3

CrdAck:
	hw_mfpr	$0	, 0x140			 
	hw_mfpr	$1	, 0x141			 

	lda	$10	, 0x1($31	)		 
	sll	$10	, 32, $10		 
	hw_mtpr	$10	, 0x115			 

	srl	$13	, 3, $8		 
	blbs	$8	, CrdDismissInterrupt	 

	lda	$16	, 0x2($31	)	 
	lda	$17, 0x0630($31	)  
	
	subq	$31	, 1, $18		 
	hw_mfpr	$25	, 0x14B			 

	srl	$18, (43-1), $18	 
	hw_mtpr	$25	, 0x10B			 

	sll	$18, (43-1), $18	 
	bis	$14	, $18, $18		 

	    bis	    $31, $31, $31
	hw_rei				 

	    .align  3

CrdDismissInterrupt:
	    bis	    $31, $31, $31
	br	$31	, CallPal_Rti	 









	    .align  5

CrdScrubMemory:
	srl	$0	, 39, $9			 
	blbs	$9	, 1f			 
	
	    bis	    $31, $31, $31				 
	    bis	    $31, $31, $31

	hw_ldq/pl	$9	, 0($0	)		 
	    bis	    $31, $31, $31				 

	hw_stq/pc	$9	, 0($0	)		 





pvc$osf101$5100.1:	
1:	ret	$31	, ($13	)		 





	    .align  3

PowerFailHandler:
	hw_mfpr	$12	, 0x148			 
	extbl	$12	, 0x6, $13		 

	bis	$31	, 0x6, $11		 
	hw_mtpr	$13	, 0x110				 

	lda	$17, 0x0640($31	)	 
	hw_mfpr	$25	, 0x14B			 

	lda	$16	, 0x3($31	)	 
	hw_mtpr	$25	, 0x10B			 

	    bis	    $31, $31, $31				 
	    bis	    $31, $31, $31

	hw_rei




	    .align  3

PerfHandler:
	hw_mfpr	$12	, 0x148			 
	extbl	$12	, 0x6, $13		 

	bis	$31	, 0x6, $11		 
	hw_mtpr	$13	, 0x110				 

	lda	$17, 0x0650($31	)	 
	hw_mfpr	$25	, 0x14B			 

	lda	$16	, 0x4($31	)	 
	hw_mtpr	$25	, 0x10B			 

	    bis	    $31, $31, $31				 
	    bis	    $31, $31, $31

	hw_rei


















































 
	    .align  3

MchkBugCheck:
	lda	$25	, 0x008E($31	)
	br	$31	, 1f

	    .align  3

MchkOSBugCheck:
	lda	$25	, 0x0090($31	)
	br	$31	, 1f

	    .align  3

1:	sll	$25	, 32, $25			 
	hw_mtpr	$14	, 0x14A		 

	hw_mtpr	$14	, 0x10B	
	hw_mfpr	$12	, 0x150			 

	zap	$12	, 0x3C, $12			 

	bis	$12	, $25	, $12			 
	lda	$25	, 0x0670($31	)  

	sll	$25	, 16, $25			 

	or	$12	, $25	, $25			 

	hw_mtpr	$0	, 0x140			 
	bis	$25	, (1<<0), $25		 

	hw_mtpr	$25	, 0x150			 
	ldah	$14	, 0xFFF0($31	)	 

	hw_mtpr	$1	, 0x141			 
	zap	$14	, 0xE0, $14			 

	hw_mtpr	$4, 0x144			 

	hw_mtpr	$5, 0x145			 

	hw_mtpr	$6, 0x146			 

	blbs	$12	, HaltDoubleMchk	 

	br	$31	, MchkCommon	 


















































 

	    .align  5

MchkContinue:





	    bis	    $31, $31, $31
	hw_mtpr	$0	, 0x140			 

	    bis	    $31, $31, $31
	    bis	    $31, $31, $31

	    bis	    $31, $31, $31
	    bis	    $31, $31, $31

	    bis	    $31, $31, $31
	    bis	    $31, $31, $31

	    bis	    $31, $31, $31
	    bis	    $31, $31, $31				 

	    bis	    $31, $31, $31
	    bis	    $31, $31, $31

	    bis	    $31, $31, $31
	    bis	    $31, $31, $31				 




	lda	$0	, 0x0670($31	) 
	hw_mfpr	$13	, 0x150			 
	sll	$0	, 16, $0		 
	bis	$13	, (1<<0), $14		 
	zap	$14	, 0x3C, $14			 
	hw_mtpr	$14	, 0x150			 
	    bis	    $31, $31, $31
	or	$14	, $0	, $14			 
	lda	$0	, 0x0098($31	)  
	hw_mfpr	$12	, 0x10B			 
	sll	$0	, 32, $0		 
	hw_mtpr	$4, 0x144			 
	or	$14	, $0	, $14			 
	hw_mtpr	$14	, 0x150    		 
	ldah	$14	, 0xFFF0($31	)	 
	hw_mtpr	$1	, 0x141			 
	zap	$14	, 0xE0, $14			 
	hw_mtpr	$12	, 0x14A		 

	hw_mtpr	$31	, 0x119			 

	blbs	$13	, HaltDoubleMchk	 
	blbs	$12	, MchkFromPal		 

	hw_mtpr	$6, 0x146			 
	hw_mtpr	$5, 0x145			 


























	
MchkCommon:
	mb				 
	hw_mfpr	$1	, 0x11A			 

                                         
pvc$osf30$379:				 
	hw_mfpr	$8	, 0x212			 

	hw_mtpr	$31	, 0x210			 

	hw_mfpr    $31, 0x140				 
	hw_mfpr    $31, 0x140
	hw_mfpr    $31, 0x140
	    bis	    $31, $31, $31

	hw_ldq/p	$12	, 0x088	($14	)		 
	or	$12	, $31	, $31			 
	or	$0	, $31	, $31			 

	hw_ldq/p	$25	, 0x108	($14	)		 
	hw_ldq/p	$31	, 0x108	($14	)	 







	bis	$31	, $31	, $5		 
	hw_mfpr	$6, 0x150    		 
	extwl	$6, 4, $6
	bic	$6, 1, $6		 

	br	$31	, MchkLogOut	 





	    .align  3

HaltDoubleMchk:
pvc$osf31$5000:
	bsr	$0	, UpdatePCB

	lda	$0	, 0x0006($31	)
	br	$31	, Sys_EnterConsole

	    .align  3

MchkFromPal:
pvc$osf32$5000:
	bsr	$0	, UpdatePCB

	lda	$0	, 0x0007($31	)
	br	$31	, Sys_EnterConsole

















































	    .align  3

MchkLogOut:

	hw_mfpr	$14	, 0x143			 
	lda	$14	, (0x568+0x0038)($14	)	 




	lda	$4, 0x01B8($5)	   
	hw_stq/p	$4, 0x0000($14	)	   
	lda	$4, 0x0160($31	)   
	sll	$4, 32, $4		   
	lda	$4, 0x00D8($4)	   
	hw_stq/p	$4, 0x0008($14	)  




	hw_mfpr	$5, 0x145			 
	hw_stq/p	$6, 0x0010($14	)	 
	hw_mfpr	$4, 0x144			 
	hw_stq/p	$1	, 0x0108($14	)	 
	hw_mfpr	$6, 0x146			 
	hw_stq/p	$8	, 0x0110($14	)	 
	hw_mfpr	$1	, 0x141			 

	hw_stq/p	$12	, 0x0140($14	)	 
	hw_stq/p	$25	, 0x0150($14	)	 
	hw_mfpr	$25	, 0x14A		 
	hw_stq/p	$25	, 0x00D8($14	)	 



		hw_mfpr	$0	, 0x205	;	    	hw_stq/p	$0	, ((0x0120)&0x3FF)($14	)
		hw_mfpr	$0	, 0x206	;	    	hw_stq/p	$0	, ((0x0118)&0x3FF)($14	)
		hw_mfpr	$0	, 0x100	;	    	hw_stq/p	$0	, ((0x00F8)&0x3FF)($14	)
		hw_mfpr	$0	, 0x118	;	    	hw_stq/p	$0	, ((0x0100)&0x3FF)($14	)
		hw_mfpr	$0	, 0x10E	;	    	hw_stq/p	$0	, ((0x00F0)&0x3FF)($14	)
		hw_mfpr	$0	, 0x10D	;	    	hw_stq/p	$0	, ((0x00E8)&0x3FF)($14	)
		hw_mfpr	$0	, 0x10C	;	    	hw_stq/p	$0	, ((0x00E0)&0x3FF)($14	)

	ldah	$13	, 0xFFF0($31	)	 
	zap	$13	, 0xE0, $13			 




		hw_mfpr	$0	, 0x140;	    	hw_stq/p	$0	, ((0x0018+0x00)&0x3FF)($14	)
		hw_mfpr	$0	, 0x141;	    	hw_stq/p	$0	, ((0x0018+0x08)&0x3FF)($14	)
		hw_mfpr	$0	, 0x142;	    	hw_stq/p	$0	, ((0x0018+0x10)&0x3FF)($14	)
		hw_mfpr	$0	, 0x143;	    	hw_stq/p	$0	, ((0x0018+0x18)&0x3FF)($14	)
		hw_mfpr	$0	, 0x144;	    	hw_stq/p	$0	, ((0x0018+0x20)&0x3FF)($14	)
		hw_mfpr	$0	, 0x145;	    	hw_stq/p	$0	, ((0x0018+0x28)&0x3FF)($14	)
		hw_mfpr	$0	, 0x146;	    	hw_stq/p	$0	, ((0x0018+0x30)&0x3FF)($14	)
		hw_mfpr	$0	, 0x147;	    	hw_stq/p	$0	, ((0x0018+0x38)&0x3FF)($14	)
		hw_mfpr	$0	, 0x148;	    	hw_stq/p	$0	, ((0x0018+0x40)&0x3FF)($14	)
		hw_mfpr	$0	, 0x149;	    	hw_stq/p	$0	, ((0x0018+0x48)&0x3FF)($14	)
		hw_mfpr	$0	, 0x14A;	    	hw_stq/p	$0	, ((0x0018+0x50)&0x3FF)($14	)
		hw_mfpr	$0	, 0x14B;	    	hw_stq/p	$0	, ((0x0018+0x58)&0x3FF)($14	)
		hw_mfpr	$0	, 0x14C;	    	hw_stq/p	$0	, ((0x0018+0x60)&0x3FF)($14	)
		hw_mfpr	$0	, 0x14D;	    	hw_stq/p	$0	, ((0x0018+0x68)&0x3FF)($14	)
		hw_mfpr	$0	, 0x14E;	    	hw_stq/p	$0	, ((0x0018+0x70)&0x3FF)($14	)
		hw_mfpr	$0	, 0x14F;	    	hw_stq/p	$0	, ((0x0018+0x78)&0x3FF)($14	)
		hw_mfpr	$0	, 0x150;	    	hw_stq/p	$0	, ((0x0018+0x80)&0x3FF)($14	)
		hw_mfpr	$0	, 0x151;	    	hw_stq/p	$0	, ((0x0018+0x88)&0x3FF)($14	)
		hw_mfpr	$0	, 0x152;	    	hw_stq/p	$0	, ((0x0018+0x90)&0x3FF)($14	)
		hw_mfpr	$0	, 0x153;	    	hw_stq/p	$0	, ((0x0018+0x98)&0x3FF)($14	)
		hw_mfpr	$0	, 0x154;	    	hw_stq/p	$0	, ((0x0018+0xA0)&0x3FF)($14	)
		hw_mfpr	$0	, 0x155;	    	hw_stq/p	$0	, ((0x0018+0xA8)&0x3FF)($14	)
		hw_mfpr	$0	, 0x156;	    	hw_stq/p	$0	, ((0x0018+0xB0)&0x3FF)($14	)
		hw_mfpr	$0	, 0x157;	    	hw_stq/p	$0	, ((0x0018+0xB8)&0x3FF)($14	)



pvc$osf33$5050:
	bsr	$0	, Sys_MchkLogOut

	hw_mfpr	$0	, 0x140			 




	hw_mfpr    $31, 0x140
	lda	$8	, ((1<<11) | (1<<12) | (1<<13))($31	)
	hw_mtpr	$8	, 0x11A	




	lda	$8	, ((1<<1) | (1<<0))($31	)
	hw_mtpr	$8	, 0x212	

	hw_mtpr	$31	, 0x10C			 






	hw_mfpr	$25	, 0x150    		 
	extwl	$25	, 4, $25			 
	blbs	$25	, 1f			 

	bis	$31	, $14	, $12			 
	bis	$31	, $11	, $13			 
	hw_mfpr	$14	, 0x14A		 
	hw_mtpr	$31	, 0x201			 
	hw_mtpr	$31	, 0x10F			 

		sll	$11	, 63-3, $25	;		bge	$25	, 0f;			bis	$31	, $31	, $11	;			hw_mtpr	$30	, 0x152	;			hw_mfpr	$30	, 0x153	;		0:	lda	$30	, 0-48($30	);		stq	$13	, 0x0000($30	);		stq	$14	, 0x0008($30	);		stq	$29	, 0x0010($30	);		stq	$16	, 0x0018($30	);		stq	$17, 0x0020($30	);		stq	$18, 0x0028($30	)

	hw_mfpr	$25	, 0x148			 
	extbl	$25	, 0x7, $25		 

	bis	$31	, 0x7, $11		 
	hw_mtpr	$25	, 0x110				 

	hw_mfpr	$25	, 0x14B			 
	lda	$16	, 0x2($31	)	 
	hw_mfpr	$17, 0x150    		 
	hw_mtpr	$25	, 0x10B			 
	subq	$31	, 1, $18		 
	extwl	$17, 2, $17		 
	srl	$18, (43-1), $18	 
	sll	$18, (43-1), $18	 
	hw_mfpr	$29	, 0x156			 
	bis	$12	, $18, $18		 
	hw_rei				 

	    .align  3

1:	lda	$16	, 0x2($31	)	 
	lda	$17, 0x0660($31	)	 
	subq	$31	, 1, $18		 
	hw_mfpr	$25	, 0x14B			 
	srl	$18, (43-1), $18	 
	hw_mtpr	$25	, 0x10B			 
	sll	$18, (43-1), $18	 
	bis	$14	, $18, $18		 
	hw_rei				 





























 
	    .align  5

UpdatePCB:
	hw_mfpr	$12	, 0x157			 
	and	$11	, (1<<3), $25			 

	rpcc	$13				 
	beq	$25	, 1f			 

	hw_mtpr	$30	, 0x152			 
	hw_stq/p	$30	, 0x0008($12	)	 

	br	$31	, 2f		 

1:	hw_stq/p	$30	, 0x0000($12	)	 

2:	srl	$13	, 32, $14			 
	addl	$13	, $14	, $14			 

	hw_stl/p	$14	, 0x0018($12	)	 

pvc$osf34$5000.1:			 
	ret	$31	, ($0	)







































 
	    .align  5

SaveState:

	lda	$1	, 0x200($1	)		 

	 	hw_stq/p	$31	, ((0x100-0x200)&0x3FF)($1	)	 
	 	hw_stq/p	$0	, ((0x108-0x200)&0x3FF)($1	)	 

	hw_mfpr	$0	, 0x140			 
	 	hw_stq/p	$0	, ((0x110	+0x00-0x200)&0x3FF)($1	)	 

	hw_mfpr	$0	, 0x144			 
	 	hw_stq/p	$0	, ((0x110	+0x08-0x200)&0x3FF)($1	)	 

	 	hw_stq/p	$2, ((0x110	+0x10-0x200)&0x3FF)($1	)	 

	hw_mfpr	$0	, 0x145			 
	 	hw_stq/p	$0	, ((0x110	+0x18-0x200)&0x3FF)($1	)	 

	 	hw_stq/p	$4, ((0x110	+0x20-0x200)&0x3FF)($1	)	 
	 	hw_stq/p	$5, ((0x110	+0x28-0x200)&0x3FF)($1	)	 













	hw_mfpr	$5, 0x118			 
	ldah	$4, (1<<(30-16))($31	)
	bic	$5, $4, $4		 
	hw_mtpr	$4, 0x118			 

	hw_mfpr    $31, 0x140				 
	hw_mfpr    $31, 0x140				 
	hw_mfpr    $31, 0x140				 
	    bis	    $31, $31, $31



	 	hw_stq/p	$6, ((0x110	+0x30-0x200)&0x3FF)($1	)
	 	hw_stq/p	$7, ((0x110	+0x38-0x200)&0x3FF)($1	)
	 	hw_stq/p	$8, ((0x110	+0x40-0x200)&0x3FF)($1	)
	 	hw_stq/p	$9, ((0x110	+0x48-0x200)&0x3FF)($1	)
	 	hw_stq/p	$10, ((0x110	+0x50-0x200)&0x3FF)($1	)
	 	hw_stq/p	$11, ((0x110	+0x58-0x200)&0x3FF)($1	)
	 	hw_stq/p	$12, ((0x110	+0x60-0x200)&0x3FF)($1	)
	 	hw_stq/p	$13, ((0x110	+0x68-0x200)&0x3FF)($1	)
	 	hw_stq/p	$14, ((0x110	+0x70-0x200)&0x3FF)($1	)
	 	hw_stq/p	$15, ((0x110	+0x78-0x200)&0x3FF)($1	)
	 	hw_stq/p	$16, ((0x110	+0x80-0x200)&0x3FF)($1	)
	 	hw_stq/p	$17, ((0x110	+0x88-0x200)&0x3FF)($1	)
	 	hw_stq/p	$18, ((0x110	+0x90-0x200)&0x3FF)($1	)
	 	hw_stq/p	$19, ((0x110	+0x98-0x200)&0x3FF)($1	)
	 	hw_stq/p	$20, ((0x110	+0xA0-0x200)&0x3FF)($1	)
	 	hw_stq/p	$21, ((0x110	+0xA8-0x200)&0x3FF)($1	)
	 	hw_stq/p	$22, ((0x110	+0xB0-0x200)&0x3FF)($1	)
	 	hw_stq/p	$23, ((0x110	+0xB8-0x200)&0x3FF)($1	)
	 	hw_stq/p	$24, ((0x110	+0xC0-0x200)&0x3FF)($1	)
	 	hw_stq/p	$25, ((0x110	+0xC8-0x200)&0x3FF)($1	)
	 	hw_stq/p	$26, ((0x110	+0xD0-0x200)&0x3FF)($1	)
	 	hw_stq/p	$27, ((0x110	+0xD8-0x200)&0x3FF)($1	)
	 	hw_stq/p	$28, ((0x110	+0xE0-0x200)&0x3FF)($1	)
	 	hw_stq/p	$29, ((0x110	+0xE8-0x200)&0x3FF)($1	)
	 	hw_stq/p	$30, ((0x110	+0xF0-0x200)&0x3FF)($1	)
	 	hw_stq/p	$31, ((0x110	+0xF8-0x200)&0x3FF)($1	)

	lda	$1	, -0x200($1	)		 
	lda	$1	, 0x418	($1	)	 


	 
		hw_mfpr	$0	, 0x140;	    	hw_stq/p	$0	, ((0x318	+0x00-0x418	)&0x3FF)($1	)
		hw_mfpr	$0	, 0x141;	    	hw_stq/p	$0	, ((0x318	+0x08-0x418	)&0x3FF)($1	)
		hw_mfpr	$0	, 0x142;	    	hw_stq/p	$0	, ((0x318	+0x10-0x418	)&0x3FF)($1	)
		hw_mfpr	$0	, 0x143;	    	hw_stq/p	$0	, ((0x318	+0x18-0x418	)&0x3FF)($1	)
		hw_mfpr	$0	, 0x144;	    	hw_stq/p	$0	, ((0x318	+0x20-0x418	)&0x3FF)($1	)
		hw_mfpr	$0	, 0x145;	    	hw_stq/p	$0	, ((0x318	+0x28-0x418	)&0x3FF)($1	)
		hw_mfpr	$0	, 0x146;	    	hw_stq/p	$0	, ((0x318	+0x30-0x418	)&0x3FF)($1	)
		hw_mfpr	$0	, 0x147;	    	hw_stq/p	$0	, ((0x318	+0x38-0x418	)&0x3FF)($1	)
		hw_mfpr	$0	, 0x148;	    	hw_stq/p	$0	, ((0x318	+0x40-0x418	)&0x3FF)($1	)
		hw_mfpr	$0	, 0x149;	    	hw_stq/p	$0	, ((0x318	+0x48-0x418	)&0x3FF)($1	)
		hw_mfpr	$0	, 0x14A;	    	hw_stq/p	$0	, ((0x318	+0x50-0x418	)&0x3FF)($1	)
		hw_mfpr	$0	, 0x14B;	    	hw_stq/p	$0	, ((0x318	+0x58-0x418	)&0x3FF)($1	)
		hw_mfpr	$0	, 0x14C;	    	hw_stq/p	$0	, ((0x318	+0x60-0x418	)&0x3FF)($1	)
		hw_mfpr	$0	, 0x14D;	    	hw_stq/p	$0	, ((0x318	+0x68-0x418	)&0x3FF)($1	)
		hw_mfpr	$0	, 0x14E;	    	hw_stq/p	$0	, ((0x318	+0x70-0x418	)&0x3FF)($1	)
		hw_mfpr	$0	, 0x14F;	    	hw_stq/p	$0	, ((0x318	+0x78-0x418	)&0x3FF)($1	)
		hw_mfpr	$0	, 0x150;	    	hw_stq/p	$0	, ((0x318	+0x80-0x418	)&0x3FF)($1	)
		hw_mfpr	$0	, 0x151;	    	hw_stq/p	$0	, ((0x318	+0x88-0x418	)&0x3FF)($1	)
		hw_mfpr	$0	, 0x152;	    	hw_stq/p	$0	, ((0x318	+0x90-0x418	)&0x3FF)($1	)
		hw_mfpr	$0	, 0x153;	    	hw_stq/p	$0	, ((0x318	+0x98-0x418	)&0x3FF)($1	)
		hw_mfpr	$0	, 0x154;	    	hw_stq/p	$0	, ((0x318	+0xA0-0x418	)&0x3FF)($1	)
		hw_mfpr	$0	, 0x155;	    	hw_stq/p	$0	, ((0x318	+0xA8-0x418	)&0x3FF)($1	)
		hw_mfpr	$0	, 0x156;	    	hw_stq/p	$0	, ((0x318	+0xB0-0x418	)&0x3FF)($1	)
		hw_mfpr	$0	, 0x157;	    	hw_stq/p	$0	, ((0x318	+0xB8-0x418	)&0x3FF)($1	)

	hw_mfpr    $31, 0x140
	hw_mfpr    $31, 0x140
	hw_mtpr	$5, 0x118			 
	
	hw_mfpr    $31, 0x140				 
	hw_mfpr    $31, 0x140				 
	hw_mfpr    $31, 0x140				 
	    bis	    $31, $31, $31



	  	hw_stq/p	$8	, ((0x3D8	+0x00-0x418	)&0x3FF)($1	)
	  	hw_stq/p	$9	, ((0x3D8	+0x08-0x418	)&0x3FF)($1	)
	  	hw_stq/p	$10	, ((0x3D8	+0x10-0x418	)&0x3FF)($1	)
	  	hw_stq/p	$11	, ((0x3D8	+0x18-0x418	)&0x3FF)($1	)
	  	hw_stq/p	$12	, ((0x3D8	+0x20-0x418	)&0x3FF)($1	)
	  	hw_stq/p	$13	, ((0x3D8	+0x28-0x418	)&0x3FF)($1	)
	  	hw_stq/p	$14	, ((0x3D8	+0x30-0x418	)&0x3FF)($1	)
	  	hw_stq/p	$25	, ((0x3D8	+0x38-0x418	)&0x3FF)($1	)



		hw_mfpr	$0	, 0x10B	;	    	hw_stq/p	$0	, ((0x418-0x418	)&0x3FF)($1	)
		hw_mfpr	$0	, 0x10E	;	    	hw_stq/p	$0	, ((0x420-0x418	)&0x3FF)($1	)
		hw_mfpr	$0	, 0x205	;	    	hw_stq/p	$0	, ((0x428-0x418	)&0x3FF)($1	)
		hw_mfpr	$0	, 0x206	;	    	hw_stq/p	$0	, ((0x430-0x418	)&0x3FF)($1	)
		hw_mfpr	$0	, 0x118	;	    	hw_stq/p	$0	, ((0x438-0x418	)&0x3FF)($1	)
		hw_mfpr	$0	, 0x110	;	    	hw_stq/p	$0	, ((0x440-0x418	)&0x3FF)($1	)
		hw_mfpr	$0	, 0x10F	;	    	hw_stq/p	$0	, ((0x448-0x418	)&0x3FF)($1	)
		hw_mfpr	$0	, 0x103	;	    	hw_stq/p	$0	, ((0x450-0x418	)&0x3FF)($1	)
		hw_mfpr	$0	, 0x10A	;	    	hw_stq/p	$0	, ((0x458-0x418	)&0x3FF)($1	)
		hw_mfpr	$0	, 0x109	;	    	hw_stq/p	$0	, ((0x460-0x418	)&0x3FF)($1	)
		hw_mfpr	$0	, 0x108	;	    	hw_stq/p	$0	, ((0x490-0x418	)&0x3FF)($1	)
		hw_mfpr	$0	, 0x100	;	    	hw_stq/p	$0	, ((0x468-0x418	)&0x3FF)($1	)
		hw_mfpr	$0	, 0x113	;	    	hw_stq/p	$0	, ((0x470-0x418	)&0x3FF)($1	)
		hw_mfpr	$0	, 0x20F	;	    	hw_stq/p	$0	, ((0x478-0x418	)&0x3FF)($1	)
		hw_mfpr	$0	, 0x216	;	    	hw_stq/p	$0	, ((0x480-0x418	)&0x3FF)($1	)
                                             
pvc$osf35$379:				     
		hw_mfpr	$0	, 0x217	;	    	hw_stq/p	$0	, ((0x488-0x418	)&0x3FF)($1	)  




		hw_mfpr	$0	, 0x11A	;	    	hw_stq/p	$0	, ((0x4A0-0x418	)&0x3FF)($1	)
		hw_mfpr	$0	, 0x11C	;	    	hw_stq/p	$0	, ((0x4A8-0x418	)&0x3FF)($1	)
		hw_mfpr	$0	, 0x10C	;	    	hw_stq/p	$0	, ((0x4B0-0x418	)&0x3FF)($1	)
		hw_mfpr	$0	, 0x10D	;	    	hw_stq/p	$0	, ((0x4B8-0x418	)&0x3FF)($1	)
		hw_mfpr	$0	, 0x111	;	    	hw_stq/p	$0	, ((0x4C0-0x418	)&0x3FF)($1	)

	ldah	$14	, 0xFFF0($31	)
	zap	$14	, 0xE0, $14			 
	    bis	    $31, $31, $31				 
	    bis	    $31, $31, $31				 
	    bis	    $31, $31, $31
		hw_mfpr	$0	, 0x212	;	    	hw_stq/p	$0	, ((0x4C8-0x418	)&0x3FF)($1	)



	mb

	hw_ldq/p	$5, 0x088	($14	)
	bis	$4, $5, $31			 
	bis	$6, $7, $31			 

	hw_ldq/p	$9	, 0x108	($14	)		 
	hw_ldq/p	$31	, 0x108	($14	)	 
	mb




	lda	$1	, -0x418	($1	)	 

	hw_mfpr	$0	, 0x118			 
	bis	$31	, 1, $2		 
	sll	$2, 26, $2	 
	bis	$2, $0	, $0			 
	hw_mtpr	$0	, 0x118			 



	hw_mtpr	$31	, 0x20A			 
	srl	$1	, 13, $0		 
	sll	$0	, 32, $0		 
	lda	$0	, 0x3300($0	)	 
	hw_mtpr	$0	, 0x203			 
	hw_mtpr	$1	, 0x202			 



	lda	$4, (1<<13)($1	)	 
	srl	$4, 13, $0		 
	sll	$0	, 32, $0		 
	lda	$0	, 0x3300($0	)	 
	hw_mtpr	$0	, 0x203			 
	hw_mtpr	$4, 0x202			 

	hw_mfpr    $31, 0x140
	hw_mfpr    $31, 0x140
	hw_mfpr    $31, 0x140
	    bis	    $31, $31, $31

	lda	$1	, 0x200($1	)		 



		stt	$f0, ((0x210	+0x00-0x200)&0x3FF)($1	)
		stt	$f1, ((0x210	+0x08-0x200)&0x3FF)($1	)
		stt	$f2, ((0x210	+0x10-0x200)&0x3FF)($1	)
		stt	$f3, ((0x210	+0x18-0x200)&0x3FF)($1	)
		stt	$f4, ((0x210	+0x20-0x200)&0x3FF)($1	)
		stt	$f5, ((0x210	+0x28-0x200)&0x3FF)($1	)
		stt	$f6, ((0x210	+0x30-0x200)&0x3FF)($1	)
		stt	$f7, ((0x210	+0x38-0x200)&0x3FF)($1	)
		stt	$f8, ((0x210	+0x40-0x200)&0x3FF)($1	)
		stt	$f9, ((0x210	+0x48-0x200)&0x3FF)($1	)
		stt	$f10, ((0x210	+0x50-0x200)&0x3FF)($1	)
		stt	$f11, ((0x210	+0x58-0x200)&0x3FF)($1	)
		stt	$f12, ((0x210	+0x60-0x200)&0x3FF)($1	)
		stt	$f13, ((0x210	+0x68-0x200)&0x3FF)($1	)
		stt	$f14, ((0x210	+0x70-0x200)&0x3FF)($1	)
		stt	$f15, ((0x210	+0x78-0x200)&0x3FF)($1	)
		stt	$f16, ((0x210	+0x80-0x200)&0x3FF)($1	)
		stt	$f17, ((0x210	+0x88-0x200)&0x3FF)($1	)
		stt	$f18, ((0x210	+0x90-0x200)&0x3FF)($1	)
		stt	$f19, ((0x210	+0x98-0x200)&0x3FF)($1	)
		stt	$f20, ((0x210	+0xA0-0x200)&0x3FF)($1	)
		stt	$f21, ((0x210	+0xA8-0x200)&0x3FF)($1	)
		stt	$f22, ((0x210	+0xB0-0x200)&0x3FF)($1	)
		stt	$f23, ((0x210	+0xB8-0x200)&0x3FF)($1	)
		stt	$f24, ((0x210	+0xC0-0x200)&0x3FF)($1	)
		stt	$f25, ((0x210	+0xC8-0x200)&0x3FF)($1	)
		stt	$f26, ((0x210	+0xD0-0x200)&0x3FF)($1	)
		stt	$f27, ((0x210	+0xD8-0x200)&0x3FF)($1	)
		stt	$f28, ((0x210	+0xE0-0x200)&0x3FF)($1	)
		stt	$f29, ((0x210	+0xE8-0x200)&0x3FF)($1	)
		stt	$f30, ((0x210	+0xF0-0x200)&0x3FF)($1	)
		stt	$f31, ((0x210	+0xF8-0x200)&0x3FF)($1	)

	lda	$1	, -0x200($1	)		 
	lda	$1	, 0x418	($1	)	 



	hw_mtpr	$31	, 0x20A			 

	lda	$1	, -0x418	($1	)	 
	lda	$1	, 0x200($1	)		 

	bis	$31	, 1, $0			 
	 	hw_stq/p	$0	, ((0x100-0x200)&0x3FF)($1	)	 
		hw_ldq/p	$0	, ((0x108-0x200)&0x3FF)($1	)	 

	lda	$1	, -0x200($1	)		 

pvc$osf36$5020.1:
	ret	$31	, ($3)		 

























 
	    .align  5

RestoreState:




	hw_mtpr	$31	, 0x20A			 
	srl	$1	, 13, $0		 
	sll	$0	, 32, $0		 
	lda	$0	, 0x3300($0	)	 
	hw_mtpr	$0	, 0x203			 
	hw_mtpr	$1	, 0x202			 



	lda	$4, (1<<13)($1	)	 
	srl	$4, 13, $0		 
	sll	$0	, 32, $0		 
	lda	$0	, 0x3300($0	)	 
	hw_mtpr	$0	, 0x203			 
	hw_mtpr	$4, 0x202			 



	hw_mfpr	$0	, 0x118			 
	bis	$31	, 1, $2		 
	or	$2, (1<<(30-26)), $2
	sll	$2, 26, $2	 
	bis	$0	, $2, $0			 
	hw_mtpr	$2, 0x118			 

	hw_mfpr    $31, 0x140				 
	hw_mfpr    $31, 0x140				 
	hw_mfpr    $31, 0x140				 

	lda	$1	, 0x200($1	)		 



		ldt	$f0, ((0x210	+0x00-0x200)&0x3FF)($1	)
		ldt	$f1, ((0x210	+0x08-0x200)&0x3FF)($1	)
		ldt	$f2, ((0x210	+0x10-0x200)&0x3FF)($1	)
		ldt	$f3, ((0x210	+0x18-0x200)&0x3FF)($1	)
		ldt	$f4, ((0x210	+0x20-0x200)&0x3FF)($1	)
		ldt	$f5, ((0x210	+0x28-0x200)&0x3FF)($1	)
		ldt	$f6, ((0x210	+0x30-0x200)&0x3FF)($1	)
		ldt	$f7, ((0x210	+0x38-0x200)&0x3FF)($1	)
		ldt	$f8, ((0x210	+0x40-0x200)&0x3FF)($1	)
		ldt	$f9, ((0x210	+0x48-0x200)&0x3FF)($1	)
		ldt	$f10, ((0x210	+0x50-0x200)&0x3FF)($1	)
		ldt	$f11, ((0x210	+0x58-0x200)&0x3FF)($1	)
		ldt	$f12, ((0x210	+0x60-0x200)&0x3FF)($1	)
		ldt	$f13, ((0x210	+0x68-0x200)&0x3FF)($1	)
		ldt	$f14, ((0x210	+0x70-0x200)&0x3FF)($1	)
		ldt	$f15, ((0x210	+0x78-0x200)&0x3FF)($1	)
		ldt	$f16, ((0x210	+0x80-0x200)&0x3FF)($1	)
		ldt	$f17, ((0x210	+0x88-0x200)&0x3FF)($1	)
		ldt	$f18, ((0x210	+0x90-0x200)&0x3FF)($1	)
		ldt	$f19, ((0x210	+0x98-0x200)&0x3FF)($1	)
		ldt	$f20, ((0x210	+0xA0-0x200)&0x3FF)($1	)
		ldt	$f21, ((0x210	+0xA8-0x200)&0x3FF)($1	)
		ldt	$f22, ((0x210	+0xB0-0x200)&0x3FF)($1	)
		ldt	$f23, ((0x210	+0xB8-0x200)&0x3FF)($1	)
		ldt	$f24, ((0x210	+0xC0-0x200)&0x3FF)($1	)
		ldt	$f25, ((0x210	+0xC8-0x200)&0x3FF)($1	)
		ldt	$f26, ((0x210	+0xD0-0x200)&0x3FF)($1	)
		ldt	$f27, ((0x210	+0xD8-0x200)&0x3FF)($1	)
		ldt	$f28, ((0x210	+0xE0-0x200)&0x3FF)($1	)
		ldt	$f29, ((0x210	+0xE8-0x200)&0x3FF)($1	)
		ldt	$f30, ((0x210	+0xF0-0x200)&0x3FF)($1	)
		ldt	$f31, ((0x210	+0xF8-0x200)&0x3FF)($1	)

	lda	$1	, -0x200($1	)		 
	lda	$1	, 0x418	($1	)	 







	  	hw_ldq/p	$0	, ((0x318	+0x00-0x418	)&0x3FF)($1	); 	hw_mtpr	$0	, 0x140
	  	hw_ldq/p	$0	, ((0x318	+0x08-0x418	)&0x3FF)($1	); 	hw_mtpr	$0	, 0x141
	  	hw_ldq/p	$0	, ((0x318	+0x10-0x418	)&0x3FF)($1	); 	hw_mtpr	$0	, 0x142
	  	hw_ldq/p	$0	, ((0x318	+0x18-0x418	)&0x3FF)($1	); 	hw_mtpr	$0	, 0x143
	  	hw_ldq/p	$0	, ((0x318	+0x20-0x418	)&0x3FF)($1	); 	hw_mtpr	$0	, 0x144
	  	hw_ldq/p	$0	, ((0x318	+0x28-0x418	)&0x3FF)($1	); 	hw_mtpr	$0	, 0x145
	  	hw_ldq/p	$0	, ((0x318	+0x30-0x418	)&0x3FF)($1	); 	hw_mtpr	$0	, 0x146
	  	hw_ldq/p	$0	, ((0x318	+0x38-0x418	)&0x3FF)($1	); 	hw_mtpr	$0	, 0x147
	  	hw_ldq/p	$0	, ((0x318	+0x40-0x418	)&0x3FF)($1	); 	hw_mtpr	$0	, 0x148
	  	hw_ldq/p	$0	, ((0x318	+0x48-0x418	)&0x3FF)($1	); 	hw_mtpr	$0	, 0x149
	  	hw_ldq/p	$0	, ((0x318	+0x50-0x418	)&0x3FF)($1	); 	hw_mtpr	$0	, 0x14A
	  	hw_ldq/p	$0	, ((0x318	+0x58-0x418	)&0x3FF)($1	); 	hw_mtpr	$0	, 0x14B
	  	hw_ldq/p	$0	, ((0x318	+0x60-0x418	)&0x3FF)($1	); 	hw_mtpr	$0	, 0x14C
	  	hw_ldq/p	$0	, ((0x318	+0x68-0x418	)&0x3FF)($1	); 	hw_mtpr	$0	, 0x14D
	  	hw_ldq/p	$0	, ((0x318	+0x70-0x418	)&0x3FF)($1	); 	hw_mtpr	$0	, 0x14E
	  	hw_ldq/p	$0	, ((0x318	+0x78-0x418	)&0x3FF)($1	); 	hw_mtpr	$0	, 0x14F
	  	hw_ldq/p	$0	, ((0x318	+0x80-0x418	)&0x3FF)($1	); 	hw_mtpr	$0	, 0x150
	  	hw_ldq/p	$0	, ((0x318	+0x88-0x418	)&0x3FF)($1	); 	hw_mtpr	$0	, 0x151
	  	hw_ldq/p	$0	, ((0x318	+0x90-0x418	)&0x3FF)($1	); 	hw_mtpr	$0	, 0x152
	  	hw_ldq/p	$0	, ((0x318	+0x98-0x418	)&0x3FF)($1	); 	hw_mtpr	$0	, 0x153
	  	hw_ldq/p	$0	, ((0x318	+0xA0-0x418	)&0x3FF)($1	); 	hw_mtpr	$0	, 0x154
	  	hw_ldq/p	$0	, ((0x318	+0xA8-0x418	)&0x3FF)($1	); 	hw_mtpr	$0	, 0x155
	  	hw_ldq/p	$0	, ((0x318	+0xB0-0x418	)&0x3FF)($1	); 	hw_mtpr	$0	, 0x156
	  	hw_ldq/p	$0	, ((0x318	+0xB8-0x418	)&0x3FF)($1	); 	hw_mtpr	$0	, 0x157





	  	hw_ldq/p	$0	, ((0x418-0x418	)&0x3FF)($1	); 	hw_mtpr	$0	, 0x10B	
	  	hw_ldq/p	$0	, ((0x420-0x418	)&0x3FF)($1	); 	hw_mtpr	$0	, 0x10E	
	  	hw_ldq/p	$0	, ((0x440-0x418	)&0x3FF)($1	); 	hw_mtpr	$0	, 0x110	
	  	hw_ldq/p	$0	, ((0x448-0x418	)&0x3FF)($1	); 	hw_mtpr	$0	, 0x10F	
	hw_mtpr	$0	, 0x201				 
	  	hw_ldq/p	$0	, ((0x450-0x418	)&0x3FF)($1	); 	hw_mtpr	$0	, 0x103	
	srl	$0	, 4, $0	
	sll	$0	, 57, $0	
	hw_mtpr	$0	, 0x200				 
	  	hw_ldq/p	$0	, ((0x470-0x418	)&0x3FF)($1	); 	hw_mtpr	$0	, 0x113	
	hw_mtpr	$0	, 0x208				 
	  	hw_ldq/p	$0	, ((0x478-0x418	)&0x3FF)($1	); 	hw_mtpr	$0	, 0x20F	
	  	hw_ldq/p	$0	, ((0x458-0x418	)&0x3FF)($1	); 	hw_mtpr	$0	, 0x10A	
	  	hw_ldq/p	$0	, ((0x460-0x418	)&0x3FF)($1	); 	hw_mtpr	$0	, 0x109	
	  	hw_ldq/p	$0	, ((0x490-0x418	)&0x3FF)($1	); 	hw_mtpr	$0	, 0x108	
	  	hw_ldq/p	$0	, ((0x488-0x418	)&0x3FF)($1	); 	hw_mtpr	$0	, 0x217	
	hw_mfpr    $31, 0x140
	hw_mfpr    $31, 0x140
	hw_mfpr    $31, 0x140
	hw_mfpr    $31, 0x140
	hw_mfpr    $31, 0x140



		hw_ldq/p	$8	, ((0x3D8	+0x00-0x418	)&0x3FF)($1	)
		hw_ldq/p	$9	, ((0x3D8	+0x08-0x418	)&0x3FF)($1	)
		hw_ldq/p	$10	, ((0x3D8	+0x10-0x418	)&0x3FF)($1	)
		hw_ldq/p	$11	, ((0x3D8	+0x18-0x418	)&0x3FF)($1	)
		hw_ldq/p	$12	, ((0x3D8	+0x20-0x418	)&0x3FF)($1	)
		hw_ldq/p	$13	, ((0x3D8	+0x28-0x418	)&0x3FF)($1	)
		hw_ldq/p	$14	, ((0x3D8	+0x30-0x418	)&0x3FF)($1	)
		hw_ldq/p	$25	, ((0x3D8	+0x38-0x418	)&0x3FF)($1	)

	  	hw_ldq/p	$0	, ((0x480-0x418	)&0x3FF)($1	); 	hw_mtpr	$0	, 0x216	

	hw_mfpr    $31, 0x140
	hw_mfpr    $31, 0x140
	hw_mfpr	$0	, 0x118			 
	ldah	$2, (1<<(30-16))($31	)
	bic	$0	, $2, $2		 
	hw_mtpr	$2, 0x118			 

	hw_mfpr    $31, 0x140				 
	hw_mfpr    $31, 0x140				 
	hw_mfpr    $31, 0x140				 
	    bis	    $31, $31, $31



	lda	$1	, -0x418	($1	)	 
	lda	$1	, 0x200($1	)		 

		hw_ldq/p	$4, ((0x110	+0x20-0x200)&0x3FF)($1	)
		hw_ldq/p	$5, ((0x110	+0x28-0x200)&0x3FF)($1	)
		hw_ldq/p	$6, ((0x110	+0x30-0x200)&0x3FF)($1	)
		hw_ldq/p	$7, ((0x110	+0x38-0x200)&0x3FF)($1	)
		hw_ldq/p	$8, ((0x110	+0x40-0x200)&0x3FF)($1	)
		hw_ldq/p	$9, ((0x110	+0x48-0x200)&0x3FF)($1	)
		hw_ldq/p	$10, ((0x110	+0x50-0x200)&0x3FF)($1	)
		hw_ldq/p	$11, ((0x110	+0x58-0x200)&0x3FF)($1	)
		hw_ldq/p	$12, ((0x110	+0x60-0x200)&0x3FF)($1	)
		hw_ldq/p	$13, ((0x110	+0x68-0x200)&0x3FF)($1	)
		hw_ldq/p	$14, ((0x110	+0x70-0x200)&0x3FF)($1	)
		hw_ldq/p	$15, ((0x110	+0x78-0x200)&0x3FF)($1	)
		hw_ldq/p	$16, ((0x110	+0x80-0x200)&0x3FF)($1	)
		hw_ldq/p	$17, ((0x110	+0x88-0x200)&0x3FF)($1	)
		hw_ldq/p	$18, ((0x110	+0x90-0x200)&0x3FF)($1	)
		hw_ldq/p	$19, ((0x110	+0x98-0x200)&0x3FF)($1	)
		hw_ldq/p	$20, ((0x110	+0xA0-0x200)&0x3FF)($1	)
		hw_ldq/p	$21, ((0x110	+0xA8-0x200)&0x3FF)($1	)
		hw_ldq/p	$22, ((0x110	+0xB0-0x200)&0x3FF)($1	)
		hw_ldq/p	$23, ((0x110	+0xB8-0x200)&0x3FF)($1	)
		hw_ldq/p	$24, ((0x110	+0xC0-0x200)&0x3FF)($1	)
		hw_ldq/p	$25, ((0x110	+0xC8-0x200)&0x3FF)($1	)
		hw_ldq/p	$26, ((0x110	+0xD0-0x200)&0x3FF)($1	)
		hw_ldq/p	$27, ((0x110	+0xD8-0x200)&0x3FF)($1	)
		hw_ldq/p	$28, ((0x110	+0xE0-0x200)&0x3FF)($1	)
		hw_ldq/p	$29, ((0x110	+0xE8-0x200)&0x3FF)($1	)
		hw_ldq/p	$30, ((0x110	+0xF0-0x200)&0x3FF)($1	)
		hw_ldq/p	$31, ((0x110	+0xF8-0x200)&0x3FF)($1	)

	lda	$1	, -0x200($1	)		 
	lda	$1	, 0x418	($1	)	 

	  	hw_ldq/p	$0	, ((0x438-0x418	)&0x3FF)($1	); 	hw_mtpr	$0	, 0x118	

	hw_mtpr	$31	, 0x20A			 

	lda	$1	, -0x418	($1	)	 
	lda	$1	, 0x200($1	)		 

	 	hw_stq/p	$31	, ((0x100-0x200)&0x3FF)($1	)	 
	mb

		hw_ldq/p	$2, ((0x110	+0x10-0x200)&0x3FF)($1	)
		hw_ldq/p	$0, ((0x110	+0x00-0x200)&0x3FF)($1	)

	lda	$1	, -0x200($1	)		 

pvc$osf37$5030.1:
	ret	$31	, ($3)		 

Pal_OsfpalEnd:







