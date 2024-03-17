# 1 "platform.s"


















































































































































































































	.data
	.asciz "$Id: platform.s,v 1.1.1.1 1998/12/29 21:36:07 paradis Exp $"


# 1 "./dc21164.h"

























































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































# 216 "platform.s"
# 1 "./osf.h"










































































































































































































































































































































































































































































































































































# 217 "platform.s"
# 1 "./macros.h"













































































































































# 218 "platform.s"
# 1 "./impure.h"











































































































































































































































































































































































































































































# 219 "platform.s"
# 1 "./platform.h"











































































































































































































































































































































































































































































































































# 220 "platform.s"
# 1 "./cserve.h"




















































































































# 221 "platform.s"





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

	.global MchkCommon
	.global HaltDoubleMchk
	.global SaveState
	.global RestoreState
	.global CallPal_Rti

	.global Wr_Bcache
	.text	3





	    .align  5

Sys_Reset:









		ldah	$1	, ((0x1F1E1615+0x8000) >> 16)($31	); 	lda	$1	, (0x1F1E1615&0xffff)($1	)	 
	sll	$1	, 32, $1			 
		ldah	$25	, (( 0x15020100+0x8000) >> 16)($31	); 	lda	$25	, ( 0x15020100&0xffff)($25	)	 
	bis	$1	, $25	, $1			 
	hw_mtpr	$1	, 0x148			 
















	hw_mfpr	$12	, 0x143			 
	lda	$2, 0x418	($12	)	 

		hw_ldq/p	$1	, ((0x548-0x418	)&0x3FF)($2)	 
	srl	$1	, 16, $1			 
		ldah	$12	, ((0xDECB+0x8000) >> 16)($31	); 	lda	$12	, (0xDECB&0xffff)($12	)			 
	cmpeq	$12	, $1	, $12			 
	beq	$12	, 2f			 

		hw_ldq/p	$1	, ((0x510-0x418	)&0x3FF)($2)	 
		ldah	$12	, ((          (((1<<15))       | 	  ((1<<4)))+0x8000) >> 16)($31	); 	lda	$12	, (          (((1<<15))       | 	  ((1<<4)))&0xffff)($12	)		 
	bis	$1	, $12	, $16			 
	bic	$16	, (1<<1), $16		 
	  	hw_stq/p	$16	, ((0x510-0x418	)&0x3FF)($2)	 

	ldah	$0	, 0xFFF0($31	)	 
	zap	$0	, 0xE0, $0			 

		hw_ldq/p	$17, ((0x518-0x418	)&0x3FF)($2)	 

	lda	$8	, 1($31	)		 
	sll	$8	, 39, $8			 

	lda	$9	, 0x75    ($31	)	 
	sll	$9	, 28, $9	
	bis	$9	, $8	, $9			 
	hw_ldl/p	$19, 0($9	)		 

	lda	$10	, 0x75    ($31	)	 
	sll	$10	, 28, $10	
	lda	$10	, 0x200($10	)		 
	bis	$10	, $8	, $10			 
	hw_ldl/p	$20, 0($10	)		 
	
pvc$osf81$5080:
	bsr	$14	, Wr_Bcache		 
2:	









































































































			ldah	$1	, (( 	((0x58	<<(28-4)) | ((0x7FFFFFF&0xA0	+0	) << (5-4)))+0x8000) >> 16)($31	); 	lda	$1	, ( 	((0x58	<<(28-4)) | ((0x7FFFFFF&0xA0	+0	) << (5-4)))&0xffff)($1	); 	sll	$1	, 4, $1	; 	lda	$25	, 1($31	); 	sll	$25	, 39, $25	; 	bis	$1	, $25	, $1	; 	lda	$25	, (0x11)($31	); 	insbl	$25	, ((	((0x58	<<(28-4)) | ((0x7FFFFFF&0xA0	+0	) << (5-4)))>>(5-4))&3), $25	; 	hw_stl/p	$25	, 0x00($1	); 	mb			; 	mb
			ldah	$1	, (( 	((0x58	<<(28-4)) | ((0x7FFFFFF&0xA0	+1	) << (5-4)))+0x8000) >> 16)($31	); 	lda	$1	, ( 	((0x58	<<(28-4)) | ((0x7FFFFFF&0xA0	+1	) << (5-4)))&0xffff)($1	); 	sll	$1	, 4, $1	; 	lda	$25	, 1($31	); 	sll	$25	, 39, $25	; 	bis	$1	, $25	, $1	; 	lda	$25	, (0x08)($31	); 	insbl	$25	, ((	((0x58	<<(28-4)) | ((0x7FFFFFF&0xA0	+1	) << (5-4)))>>(5-4))&3), $25	; 	hw_stl/p	$25	, 0x00($1	); 	mb			; 	mb
			ldah	$1	, (( 	((0x58	<<(28-4)) | ((0x7FFFFFF&0xA0	+1	) << (5-4)))+0x8000) >> 16)($31	); 	lda	$1	, ( 	((0x58	<<(28-4)) | ((0x7FFFFFF&0xA0	+1	) << (5-4)))&0xffff)($1	); 	sll	$1	, 4, $1	; 	lda	$25	, 1($31	); 	sll	$25	, 39, $25	; 	bis	$1	, $25	, $1	; 	lda	$25	, (0x02)($31	); 	insbl	$25	, ((	((0x58	<<(28-4)) | ((0x7FFFFFF&0xA0	+1	) << (5-4)))>>(5-4))&3), $25	; 	hw_stl/p	$25	, 0x00($1	); 	mb			; 	mb
			ldah	$1	, (( 	((0x58	<<(28-4)) | ((0x7FFFFFF&0xA0	+1	) << (5-4)))+0x8000) >> 16)($31	); 	lda	$1	, ( 	((0x58	<<(28-4)) | ((0x7FFFFFF&0xA0	+1	) << (5-4)))&0xffff)($1	); 	sll	$1	, 4, $1	; 	lda	$25	, 1($31	); 	sll	$25	, 39, $25	; 	bis	$1	, $25	, $1	; 	lda	$25	, (0x01)($31	); 	insbl	$25	, ((	((0x58	<<(28-4)) | ((0x7FFFFFF&0xA0	+1	) << (5-4)))>>(5-4))&3), $25	; 	hw_stl/p	$25	, 0x00($1	); 	mb			; 	mb

			ldah	$1	, (( 	((0x58	<<(28-4)) | ((0x7FFFFFF&0xA0	+1	) << (5-4)))+0x8000) >> 16)($31	); 	lda	$1	, ( 	((0x58	<<(28-4)) | ((0x7FFFFFF&0xA0	+1	) << (5-4)))&0xffff)($1	); 	sll	$1	, 4, $1	; 	lda	$25	, 1($31	); 	sll	$25	, 39, $25	; 	bis	$1	, $25	, $1	; 	lda	$25	, (0xFF)($31	); 	insbl	$25	, ((	((0x58	<<(28-4)) | ((0x7FFFFFF&0xA0	+1	) << (5-4)))>>(5-4))&3), $25	; 	hw_stl/p	$25	, 0x00($1	); 	mb			; 	mb

			ldah	$1	, (( 	((0x58	<<(28-4)) | ((0x7FFFFFF&0x20	+0	) << (5-4)))+0x8000) >> 16)($31	); 	lda	$1	, ( 	((0x58	<<(28-4)) | ((0x7FFFFFF&0x20	+0	) << (5-4)))&0xffff)($1	); 	sll	$1	, 4, $1	; 	lda	$25	, 1($31	); 	sll	$25	, 39, $25	; 	bis	$1	, $25	, $1	; 	lda	$25	, (0x11)($31	); 	insbl	$25	, ((	((0x58	<<(28-4)) | ((0x7FFFFFF&0x20	+0	) << (5-4)))>>(5-4))&3), $25	; 	hw_stl/p	$25	, 0x00($1	); 	mb			; 	mb
			ldah	$1	, (( 	((0x58	<<(28-4)) | ((0x7FFFFFF&0x20	+1	) << (5-4)))+0x8000) >> 16)($31	); 	lda	$1	, ( 	((0x58	<<(28-4)) | ((0x7FFFFFF&0x20	+1	) << (5-4)))&0xffff)($1	); 	sll	$1	, 4, $1	; 	lda	$25	, 1($31	); 	sll	$25	, 39, $25	; 	bis	$1	, $25	, $1	; 	lda	$25	, (0x00)($31	); 	insbl	$25	, ((	((0x58	<<(28-4)) | ((0x7FFFFFF&0x20	+1	) << (5-4)))>>(5-4))&3), $25	; 	hw_stl/p	$25	, 0x00($1	); 	mb			; 	mb
			ldah	$1	, (( 	((0x58	<<(28-4)) | ((0x7FFFFFF&0x20	+1	) << (5-4)))+0x8000) >> 16)($31	); 	lda	$1	, ( 	((0x58	<<(28-4)) | ((0x7FFFFFF&0x20	+1	) << (5-4)))&0xffff)($1	); 	sll	$1	, 4, $1	; 	lda	$25	, 1($31	); 	sll	$25	, 39, $25	; 	bis	$1	, $25	, $1	; 	lda	$25	, (0x04)($31	); 	insbl	$25	, ((	((0x58	<<(28-4)) | ((0x7FFFFFF&0x20	+1	) << (5-4)))>>(5-4))&3), $25	; 	hw_stl/p	$25	, 0x00($1	); 	mb			; 	mb
			ldah	$1	, (( 	((0x58	<<(28-4)) | ((0x7FFFFFF&0x20	+1	) << (5-4)))+0x8000) >> 16)($31	); 	lda	$1	, ( 	((0x58	<<(28-4)) | ((0x7FFFFFF&0x20	+1	) << (5-4)))&0xffff)($1	); 	sll	$1	, 4, $1	; 	lda	$25	, 1($31	); 	sll	$25	, 39, $25	; 	bis	$1	, $25	, $1	; 	lda	$25	, (0x01)($31	); 	insbl	$25	, ((	((0x58	<<(28-4)) | ((0x7FFFFFF&0x20	+1	) << (5-4)))>>(5-4))&3), $25	; 	hw_stl/p	$25	, 0x00($1	); 	mb			; 	mb

			ldah	$1	, (( 	((0x58	<<(28-4)) | ((0x7FFFFFF&0x20	+1	) << (5-4)))+0x8000) >> 16)($31	); 	lda	$1	, ( 	((0x58	<<(28-4)) | ((0x7FFFFFF&0x20	+1	) << (5-4)))&0xffff)($1	); 	sll	$1	, 4, $1	; 	lda	$25	, 1($31	); 	sll	$25	, 39, $25	; 	bis	$1	, $25	, $1	; 	lda	$25	, (0xFF)($31	); 	insbl	$25	, ((	((0x58	<<(28-4)) | ((0x7FFFFFF&0x20	+1	) << (5-4)))>>(5-4))&3), $25	; 	hw_stl/p	$25	, 0x00($1	); 	mb			; 	mb




				ldah	$1	, (( 	((0x72	<<(28-4)) | ((0x7FFFFFF&0) << (5-4)))+0x8000) >> 16)($31	); 	lda	$1	, ( 	((0x72	<<(28-4)) | ((0x7FFFFFF&0) << (5-4)))&0xffff)($1	); 	sll	$1	, 4, $1	; 	lda	$25	, 1($31	); 	sll	$25	, 39, $25	; 	bis	$1	, $25	, $1	; 	hw_ldl/p	$1	, 0x00($1	); 	srl	$1	, (8*((	((0x72	<<(28-4)) | ((0x7FFFFFF&0) << (5-4)))>>(5-4))&3)), $1	; 	zap	$1	, 0xfe, $1	




			ldah	$1	, (( 	((0x58	<<(28-4)) | ((0x7FFFFFF&0xA0	+0	) << (5-4)))+0x8000) >> 16)($31	); 	lda	$1	, ( 	((0x58	<<(28-4)) | ((0x7FFFFFF&0xA0	+0	) << (5-4)))&0xffff)($1	); 	sll	$1	, 4, $1	; 	lda	$25	, 1($31	); 	sll	$25	, 39, $25	; 	bis	$1	, $25	, $1	; 	lda	$25	, (0x20)($31	); 	insbl	$25	, ((	((0x58	<<(28-4)) | ((0x7FFFFFF&0xA0	+0	) << (5-4)))>>(5-4))&3), $25	; 	hw_stl/p	$25	, 0x00($1	); 	mb			; 	mb
			ldah	$1	, (( 	((0x58	<<(28-4)) | ((0x7FFFFFF&0x20	+0	) << (5-4)))+0x8000) >> 16)($31	); 	lda	$1	, ( 	((0x58	<<(28-4)) | ((0x7FFFFFF&0x20	+0	) << (5-4)))&0xffff)($1	); 	sll	$1	, 4, $1	; 	lda	$25	, 1($31	); 	sll	$25	, 39, $25	; 	bis	$1	, $25	, $1	; 	lda	$25	, (0x20)($31	); 	insbl	$25	, ((	((0x58	<<(28-4)) | ((0x7FFFFFF&0x20	+0	) << (5-4)))>>(5-4))&3), $25	; 	hw_stl/p	$25	, 0x00($1	); 	mb			; 	mb






	lda	$1	, 1($31	)		 
	sll	$1	, 39, $1			 
	lda	$25	, 0x74    ($31	)	 
	sll	$25	, 28, $25	
	lda	$25	, 0x100   ($25	)	 
	bis	$25	, $1	, $25			 
		ldah	$1	, (( 0x1C00+0x8000) >> 16)($31	); 	lda	$1	, ( 0x1C00&0xffff)($1	)	 
	hw_ldl/p	$2, 0($25	)		 
	bic	$2, $1	, $2		 
	hw_stl/p	$2, 0($25	)		 

pvc$osf41$5010.1:
	ret	$31	, ($13	)		 






Sys_ResetViaSwppal:
		ldah	$1	, ((0x1F1E1615+0x8000) >> 16)($31	); 	lda	$1	, (0x1F1E1615&0xffff)($1	)	 
	sll	$1	, 32, $1			 
		ldah	$25	, ((0x15020100+0x8000) >> 16)($31	); 	lda	$25	, (0x15020100&0xffff)($25	)	 
	bis	$1	, $25	, $1			 
	hw_mtpr	$1	, 0x148			 
pvc$osf48$5060.1:
	ret	$31	, ($0	)		 























	    .align  3
Sys_Interrupt:

	cmpeq	$13	, 23, $12			 
	bne	$12	, Int23Handler

	cmpeq	$13	, 22, $12			 
	bne	$12	, Int22Handler

	cmpeq	$13	, 21, $12			 
	bne	$12	, Int21Handler

	cmpeq	$13	, 20, $12			 
	bne	$12	, Int20Handler

	    bis	    $31, $31, $31				 
	br	$31	, CallPal_Rti

	    .align  3





Int23Handler:
	    bis	    $31, $31, $31
	br	$31	, DismissInterrupt


	    .align  3























Int22Handler:

	hw_mfpr	$12	, 0x148			 
	extbl	$12	, 0x5, $13		 

	bis	$31	, 0x5, $11		 
	hw_mtpr	$13	, 0x110				 

	hw_mfpr	$12	, 0x14B			 

			ldah	$14	, (( 	((0x58	<<(28-4)) | ((0x7FFFFFF&0x70) << (5-4))) +0x8000) >> 16)($31	); 	lda	$14	, ( 	((0x58	<<(28-4)) | ((0x7FFFFFF&0x70) << (5-4))) &0xffff)($14	); 	sll	$14	, 4, $14	; 	lda	$25	, 1($31	); 	sll	$25	, 39, $25	; 	bis	$14	, $25	, $14	; 	lda	$25	, (0x0C)($31	); 	insbl	$25	, ((	((0x58	<<(28-4)) | ((0x7FFFFFF&0x70) << (5-4))) >>(5-4))&3), $25	; 	hw_stl/p	$25	, 0x00($14	); 	mb			; 	mb	 
			ldah	$14	, (( 	((0x58	<<(28-4)) | ((0x7FFFFFF&0x71) << (5-4))) +0x8000) >> 16)($31	); 	lda	$14	, ( 	((0x58	<<(28-4)) | ((0x7FFFFFF&0x71) << (5-4))) &0xffff)($14	); 	sll	$14	, 4, $14	; 	lda	$25	, 1($31	); 	sll	$25	, 39, $25	; 	bis	$14	, $25	, $14	; 	hw_ldl/p	$14	, 0x00($14	); 	srl	$14	, (8*((	((0x58	<<(28-4)) | ((0x7FFFFFF&0x71) << (5-4))) >>(5-4))&3)), $14	; 	zap	$14	, 0xfe, $14		 

	hw_mtpr	$12	, 0x10B			 
	bis	$31	, 0x1, $16		 

	    bis	    $31, $31, $31
	    bis	    $31, $31, $31

	hw_rei

	    .align  3






















Int21Handler:
	hw_mfpr	$12	, 0x148			 
	extbl	$12	, 0x3, $13		 

	bis	$31	, 0x3, $11		 
	hw_mtpr	$13	, 0x110				 

	hw_mfpr	$12	, 0x14B			 
	lda	$17, 0x800($31	)		 

	hw_mtpr	$12	, 0x10B			 
	bis	$31	, 0x3, $16		 

	    bis	    $31, $31, $31
	    bis	    $31, $31, $31

	hw_rei

	    .align  3










Int20Handler:

	hw_mfpr	$12	, 0x148				 
	extbl	$12	, 0x7, $13			 

	bis	$31	, 0x7, $11			 
	hw_mtpr	$13	, 0x110					 

	lda	$14	, 0x021E       ($31	)		 
	br	$31	, Sys_IntMchkHandlerMerge	 

	    .align  3






DismissInterrupt:
	    bis	    $31, $31, $31				 
	br	$31	, CallPal_Rti


































	    .align  3





Sys_IntMchkHandler:
			ldah	$12	, (( 	((0x58	<<(28-4)) | ((0x7FFFFFF& 0x61	) << (5-4)))+0x8000) >> 16)($31	); 	lda	$12	, ( 	((0x58	<<(28-4)) | ((0x7FFFFFF& 0x61	) << (5-4)))&0xffff)($12	); 	sll	$12	, 4, $12	; 	lda	$25	, 1($31	); 	sll	$25	, 39, $25	; 	bis	$12	, $25	, $12	; 	hw_ldl/p	$12	, 0x00($12	); 	srl	$12	, (8*((	((0x58	<<(28-4)) | ((0x7FFFFFF& 0x61	) << (5-4)))>>(5-4))&3)), $12	; 	zap	$12	, 0xfe, $12		 

	srl	$12	, 7, $13		 
	blbc	$13	, 3f				 
	srl	$12	, 2, $13		 
	blbs	$13	, 3f				 

	bis	$12	, (1<<2), $13		 
	and	$13	, 0x0F, $13				 

			ldah	 $14	, (( 	((0x58	<<(28-4)) | ((0x7FFFFFF& 0x61	) << (5-4)))+0x8000) >> 16)($31	); 	lda	 $14	, ( 	((0x58	<<(28-4)) | ((0x7FFFFFF& 0x61	) << (5-4)))&0xffff)( $14	); 	sll	 $14	, 4,  $14	; 	lda	 $25	, 1($31	); 	sll	 $25	, 39,  $25	; 	bis	 $14	,  $25	,  $14	; 	and	 $13	, 0xFF,  $25	; 	insbl	 $25	, ((	((0x58	<<(28-4)) | ((0x7FFFFFF& 0x61	) << (5-4)))>>(5-4))&3),  $25	; 	hw_stl/p	 $25	, 0x00( $14	); 	mb			; 	mb	 

	bis	$31	, $12	, $13				 
	and	$13	, 0x0F, $13				 

			ldah	 $14	, (( 	((0x58	<<(28-4)) | ((0x7FFFFFF& 0x61	) << (5-4)))+0x8000) >> 16)($31	); 	lda	 $14	, ( 	((0x58	<<(28-4)) | ((0x7FFFFFF& 0x61	) << (5-4)))&0xffff)( $14	); 	sll	 $14	, 4,  $14	; 	lda	 $25	, 1($31	); 	sll	 $25	, 39,  $25	; 	bis	 $14	,  $25	,  $14	; 	and	 $13	, 0xFF,  $25	; 	insbl	 $25	, ((	((0x58	<<(28-4)) | ((0x7FFFFFF& 0x61	) << (5-4)))>>(5-4))&3),  $25	; 	hw_stl/p	 $25	, 0x00( $14	); 	mb			; 	mb	 

	lda	$14	, 0x0204($31	)	 
	br	$31	, Sys_IntMchkHandlerMerge	 

	    .align  3

3:	srl	$12	, 6, $13		 
	blbc	$13	, 4f				 
	srl	$12	, 3, $13		 
	blbs	$13	, 4f				 

	bis	$12	, (1<<3), $13		 
	and	$13	, 0x0F, $13				 

			ldah	 $14	, (( 	((0x58	<<(28-4)) | ((0x7FFFFFF& 0x61	) << (5-4)))+0x8000) >> 16)($31	); 	lda	 $14	, ( 	((0x58	<<(28-4)) | ((0x7FFFFFF& 0x61	) << (5-4)))&0xffff)( $14	); 	sll	 $14	, 4,  $14	; 	lda	 $25	, 1($31	); 	sll	 $25	, 39,  $25	; 	bis	 $14	,  $25	,  $14	; 	and	 $13	, 0xFF,  $25	; 	insbl	 $25	, ((	((0x58	<<(28-4)) | ((0x7FFFFFF& 0x61	) << (5-4)))>>(5-4))&3),  $25	; 	hw_stl/p	 $25	, 0x00( $14	); 	mb			; 	mb	 

	bis	$31	, $12	, $13				 
	and	$13	, 0x0F, $13				 

			ldah	 $14	, (( 	((0x58	<<(28-4)) | ((0x7FFFFFF& 0x61	) << (5-4)))+0x8000) >> 16)($31	); 	lda	 $14	, ( 	((0x58	<<(28-4)) | ((0x7FFFFFF& 0x61	) << (5-4)))&0xffff)( $14	); 	sll	 $14	, 4,  $14	; 	lda	 $25	, 1($31	); 	sll	 $25	, 39,  $25	; 	bis	 $14	,  $25	,  $14	; 	and	 $13	, 0xFF,  $25	; 	insbl	 $25	, ((	((0x58	<<(28-4)) | ((0x7FFFFFF& 0x61	) << (5-4)))>>(5-4))&3),  $25	; 	hw_stl/p	 $25	, 0x00( $14	); 	mb			; 	mb	 

	lda	$14	, 0x0206($31	)	 
	br	$31	, Sys_IntMchkHandlerMerge	 

	    .align  3




4:	lda	$14	, 0x0202($31	)	 
	br	$31	, Sys_IntMchkHandlerMerge	 

	    .align  3
Sys_IntMchkHandlerMerge:
	hw_mfpr	$12	, 0x10B			 
	addq	$14	, 1, $14			 

	sll	$14	, 32, $14		 
	hw_mtpr	$12	, 0x14A		 

	hw_mfpr	$12	, 0x150			 

	hw_mtpr	$0	, 0x140			 
	zap	$12	, 0x3C, $12			 

	blbs	$12	, HaltDoubleMchk	 

	bis	$12	, $14	, $12			 
	lda	$14	, 0x0660($31	)	 

	sll	$14	, 16, $14		 

	bis	$12	, $14	, $14			 

	bis	$14	, (1<<0), $14		 

	hw_mtpr	$14	, 0x150			 

	ldah	$14	, 0xFFF0($31	)	 
	hw_mtpr	$1	, 0x141			 

	zap	$14	, 0xE0, $14			 
	hw_mtpr	$4, 0x144			 

	hw_mtpr	$5, 0x145			 

	hw_mtpr	$6, 0x146			 

	br	$31	, MchkCommon	 
































	    .align  5

Sys_MchkLogOut:






		ldah	$13	, (( 0x8740008	+0x8000) >> 16)($31	); 	lda	$13	, ( 0x8740008	&0xffff)($13	)	 
	sll	$13	, 12, $13			 

	hw_ldl/p	$25	, 0($13	)		 
	hw_stq/p	$25	, 0x160($14	)	 

	lda	$25	, 0x040($13	)	 
	hw_ldl/p	$25	, 0($25	)		 
	hw_stq/p	$25	, 0x168($14	)	 

	lda	$25	, 0x200($13	)	 
	hw_ldl/p	$25	, 0($25	)		 
	hw_stq/p	$25	, 0x170($14	)	 

	lda	$25	, 0x240($13	)	 
	hw_ldl/p	$25	, 0($25	)		 
	hw_stq/p	$25	, 0x178($14	)	 

	lda	$25	, 0x280($13	)	 
	hw_ldl/p	$25	, 0($25	)		 
	hw_stq/p	$25	, 0x180($14	)	 

	lda	$25	, 0x300($13	)	 
	hw_ldl/p	$25	, 0($25	)		 
	hw_stq/p	$25	, 0x188($14	)	 

	lda	$25	, 0x400($13	)	 
	hw_ldl/p	$25	, 0($25	)		 
	hw_stq/p	$25	, 0x190($14	)	 

	lda	$25	, 0x440($13	)	 
	hw_ldl/p	$25	, 0($25	)		 
	hw_stq/p	$25	, 0x198($14	)	 

	lda	$25	, 0x800($13	)	 
	hw_ldl/p	$25	, 0($25	)		 
	hw_stq/p	$25	, 0x1A0($14	)	 

	lda	$25	, 0x840($13	)	 
	hw_ldl/p	$25	, 0($25	)		 
	hw_stq/p	$25	, 0x1A8($14	)	 

	lda	$25	, 0x880($13	)	 
	hw_ldl/p	$25	, 0($25	)		 
	hw_stq/p	$25	, 0x1B0($14	)	 
pvc$osf42$5050.1:
	ret	$31	, ($0	)		 





	    .align  5

Sys_Cflush:
	hw_rei

	    .align  5

Sys_WrIpir:
	hw_rei


























	    .align  5

Sys_Cserve:

	cmpeq	$18, 0x01, $0	
	bne	$0	, Sys_Cserve_Ldqp

	cmpeq	$18, 0x02, $0	
	bne	$0	, Sys_Cserve_Stqp

	cmpeq	$18, 0x09, $0	
	bne	$0	, Sys_Cserve_Jtopal

	cmpeq	$18, 0x0A, $0	
	bne	$0	, Sys_Cserve_Wr_Int

	cmpeq	$18, 0x0B, $0	
	bne	$0	, Sys_Cserve_Rd_Impure

	cmpeq	$18, 0x0F, $0	
	bne	$0	, Sys_Cserve_Putc

	cmpeq	$18, 0x10, $0	
	bne	$0	, Sys_Cserve_Wr_Icsr

	cmpeq	$18, 0x11, $0	
	bne	$0	, Sys_Cserve_Rd_Icsr

	cmpeq	$18, 0x12, $0	
	bne	$0	, Sys_Cserve_Rd_BcCtl
	cmpeq	$18, 0x13, $0	
	bne	$0	, Sys_Cserve_Rd_BcCfg

	cmpeq	$18, 0x16, $0	
	bne	$0	, Sys_Cserve_Wr_Bcache

	cmpeq	$18, 0x17, $0	
	bne	$0	, Sys_Cserve_Rd_BcCfgOff

	cmpeq	$18, 0x18, $0	
	bne	$0	, Sys_Cserve_Srom_Init

	cmpeq	$18, 0x19, $0	
	bne	$0	, Sys_Cserve_Srom_Putc

	cmpeq	$18, 0x20, $0	
	bne	$0	, Sys_Cserve_Srom_Getc


	hw_rei                           


















	    .align  3

Sys_Cserve_Ldqp:
	hw_ldq/p	$0	, 0($16	)
	hw_rei



















	    .align  3

Sys_Cserve_Stqp:
	hw_stq/p	$17, 0($16	)
	hw_rei

































	    .align  3

Sys_Cserve_Jtopal:
	bic	$16	, 3, $22			 
	bis	$22	, 1, $22			 

	hw_mfpr	$23, 0x143			 
        lda     $23, 0x418	($23)    	 

		hw_ldq/p	$19, ((0x548-0x418	)&0x3FF)($23)	 
        srl     $19, 16, $1	               
	zap	$1	, 0xFC, $1			 
		ldah	$24, ((0xDECB+0x8000) >> 16)($31	); 	lda	$24, (0xDECB&0xffff)($24)		 

        cmpeq   $1	, $24, $1			 
        blbc    $1	, 1f                   



		hw_ldq/p	$1	, ((0x510-0x418	)&0x3FF)($23)	 
		hw_ldq/p	$2, ((0x518-0x418	)&0x3FF)($23)	 
		hw_ldq/p	$3, ((0x560-0x418	)&0x3FF)($23)	 



		hw_ldq/p	$15, ((0x528-0x418	)&0x3FF)($23)	 
		hw_ldq/p	$16	, ((0x530-0x418	)&0x3FF)($23)	 
		hw_ldq/p	$17, ((0x538-0x418	)&0x3FF)($23)	 
		hw_ldq/p	$18, ((0x540-0x418	)&0x3FF)($23)	 
		hw_ldq/p	$20, ((0x550-0x418	)&0x3FF)($23)	 
		hw_ldq/p	$21, ((0x558-0x418	)&0x3FF)($23)	 

	hw_mfpr    $31, 0x140
	hw_mfpr    $31, 0x140

1:	hw_mtpr	$31	, 0x150			 
	hw_mtpr	$22	, 0x10B			 
	br	$31	, 2f

	    .align  5

2:	    bis	    $31, $31, $31
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

	hw_rei				 


















	    .align  3

Sys_Cserve_Wr_Int:
	hw_mtpr	$16	, 0x148			 

	hw_mfpr    $31, 0x140
	hw_mfpr    $31, 0x140

	hw_rei



















	    .align  3

Sys_Cserve_Rd_Impure:
	hw_mfpr	$0	, 0x143			 
	hw_rei


















	    .align  3

Sys_Cserve_Putc:

			ldah	$8	, (( 	((0x58	<<(28-4)) | ((0x7FFFFFF&0x3F8	+0x5	) << (5-4)))+0x8000) >> 16)($31	); 	lda	$8	, ( 	((0x58	<<(28-4)) | ((0x7FFFFFF&0x3F8	+0x5	) << (5-4)))&0xffff)($8	); 	sll	$8	, 4, $8	; 	lda	$25	, 1($31	); 	sll	$25	, 39, $25	; 	bis	$8	, $25	, $8	; 	hw_ldl/p	$8	, 0x00($8	); 	srl	$8	, (8*((	((0x58	<<(28-4)) | ((0x7FFFFFF&0x3F8	+0x5	) << (5-4)))>>(5-4))&3)), $8	; 	zap	$8	, 0xfe, $8	

	srl     $8	, 5	,$8	
	blbc    $8	, Sys_Cserve_Putc

			ldah	$8	, (( 	((0x58	<<(28-4)) | ((0x7FFFFFF&0x3F8	+0	) << (5-4)))+0x8000) >> 16)($31	); 	lda	$8	, ( 	((0x58	<<(28-4)) | ((0x7FFFFFF&0x3F8	+0	) << (5-4)))&0xffff)($8	); 	sll	$8	, 4, $8	; 	lda	$0	, 1($31	); 	sll	$0	, 39, $0	; 	bis	$8	, $0	, $8	; 	and	$16	, 0xFF, $0	; 	insbl	$0	, ((	((0x58	<<(28-4)) | ((0x7FFFFFF&0x3F8	+0	) << (5-4)))>>(5-4))&3), $0	; 	hw_stl/p	$0	, 0x00($8	); 	mb			; 	mb

	lda     $0	, 1($31	)
	hw_rei





















	    .align  3

Sys_Cserve_Rd_Icsr:
        hw_mfpr	$0	, 0x118				 
	hw_rei
























	    .align  3

Sys_Cserve_Wr_Icsr:
	ldah	$8	, 	((1<<(30-16)) | 	 (2<<(28-16)))($31	)	 
	bis	$16	, $8	, $8			 
	
	hw_mtpr	$8	, 0x118			 

	hw_mfpr    $31, 0x140                            
        hw_mfpr    $31, 0x140
	hw_mfpr    $31, 0x140
	hw_mfpr    $31, 0x140
	hw_mfpr    $31, 0x140

	hw_rei



















	    .align  3

Sys_Cserve_Rd_BcCtl:
	hw_mfpr	$8	, 0x143	             
	lda	$8	, 0x418	($8	)	 

		hw_ldq/p	$0	, ((0x510-0x418	)&0x3FF)($8	)	 

	hw_mfpr    $31, 0x140

	hw_rei



















	    .align  3

Sys_Cserve_Rd_BcCfg:
	hw_mfpr	$8	, 0x143	             
	lda	$8	, 0x418	($8	)	 

		hw_ldq/p	$0	, ((0x518-0x418	)&0x3FF)($8	)	 

	hw_mfpr    $31, 0x140

	hw_rei




















	    .align  3

Sys_Cserve_Rd_BcCfgOff:
	hw_mfpr	$8	, 0x143	             
	lda	$8	, 0x418	($8	)	 

		hw_ldq/p	$0	, ((0x560-0x418	)&0x3FF)($8	)	 

	hw_mfpr    $31, 0x140

	hw_rei
























	    .align  3

Sys_Cserve_Wr_Bcache:
	ldah	$0	, 0xFFF0($31	)	 
	zap	$0	, 0xE0, $0			 
	bic	$16	, (1<<1), $16		 

	lda	$8	, 1($31	)		 
	sll	$8	, 39, $8			 

	lda	$9	, 0x75    ($31	)	 
	sll	$9	, 28, $9	
	bis	$9	, $8	, $9			 
	
	lda	$10	, 0x75    ($31	)	 
	sll	$10	, 28, $10	
	lda $10	, 0x200($10	)	 
	bis	$10	, $8	, $10			 
	
pvc$osf80$5080:
	bsr	$14	, Wr_Bcache		 

	hw_mfpr	$8	, 0x143	             
	lda	$8	, 0x418	($8	)	 

	  	hw_stq/p	$16	, ((0x510-0x418	)&0x3FF)($8	)	 
	  	hw_stq/p	$17, ((0x518-0x418	)&0x3FF)($8	)	 

	bis	$31	, 1, $0			 

	hw_mfpr    $31, 0x140
	hw_mfpr    $31, 0x140

	hw_rei







	    .align  5

Wr_Bcache:
	    bis	    $31, $31, $31
	    bis	    $31, $31, $31
	lda	$12	, 100($31	)		 
1:	subq	$12	, 1, $12			 

	bgt	$12	, 2f			 
	mb
	hw_stq/p	$16	, 0x128	($0	)		 
	mb
2:	bgt	$12	, 3f
	hw_stq/p	$17, 0x1C8	($0	)		 
	mb
	hw_stl/p	$19, 0($9	)		 
3:	bgt	$12	, 4f
	mb
	hw_stl/p	$20, 0($10	)
	mb
4:	bgt	$12	, 1b			 
					 
					 
pvc$osf80$5080.1:
	ret	$31	, ($14	)		 





































 
	    .align  5
	
Sys_EnterConsole:
	hw_mtpr	$1	, 0x144			 
	hw_mtpr	$3, 0x145			 





	hw_mfpr	$1	, 0x143			 

pvc$osf43$5020:
	bsr	$3, SaveState		 

	hw_mfpr	$1	, 0x154			 
	bis	$1	, 1, $1			 
	hw_mtpr	$1	, 0x154			 

		ldah	$30	, ((0x00FFE000+0x8000) >> 16)($31	); 	lda	$30	, (0x00FFE000&0xffff)($30	)		 

	hw_mtpr	$30	, 0x153			 

	lda	$11	, 0x7($31	)	 
	lda	$1	, 0x1F($31	)		 
	hw_mtpr	$1	, 0x110	
	hw_mtpr	$31	, 0x10F			 
	hw_mtpr	$31	, 0x201			 

	hw_mtpr	$31	, 0x20A			 
	hw_mtpr	$31	, 0x105			 
	hw_mtpr	$31	, 0x109			 
	hw_mtpr	$31	, 0x10A	
	hw_mtpr	$31	, 0x108			 

	br	$1	, 1f			 
	.long	0x10000
1:	hw_ldl/p	$1	, 0($1	)		 
	hw_mtpr	$1	, 0x10B			 
	hw_mfpr	$1	, 0x144			 
	hw_mfpr	$3, 0x145
	bis	$0	, $0	, $1			 

	hw_rei_stall			 





	.data
Pal_End:
