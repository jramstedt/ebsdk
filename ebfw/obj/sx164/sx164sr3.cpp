# 1 "srom.s"


























 



































































































































































































































































































# 1 "../../palcode/sx164/dc21164.h"









































































































































































































































































































































































































































































































































































































































































































































































































































































































































































# 320 "srom.s"
# 1 "./io.h"


























































# 1 "./ascitab.h"






















































































































































































# 60 "./io.h"






















































































































					  


































































































































































































				

				
				





















					




















































                                

                                




# 321 "srom.s"
# 1 "../../h/pyxis.h"















































































































































































































































































































































































































































































































































# 322 "srom.s"


# 1 "/sae_users/paradis/sdk-cdbuild/CDIMAGE/ebsdk/ebfw/obj/sx164//rcsv.h"


# 325 "srom.s"

















 






	
.align 3













initIPRs:
	hw_mtpr	$31, 0x210			




























	lda $2, ( ( (1 << (17-16)) | (1 << (25-16)) | (1 << (19-16)) |		   (1 << (26-16)) | (1 << (27-16)) | (0 << (30-16)) | 		   (1 << (33-16)) | (1 << (39-16)))&0xffff)($31)		;  ldah $2, (( ( (1 << (17-16)) | (1 << (25-16)) | (1 << (19-16)) |		   (1 << (26-16)) | (1 << (27-16)) | (0 << (30-16)) | 		   (1 << (33-16)) | (1 << (39-16)))+0x8000)>>16)($2)	; 
	sll	$2, 16, $2
	hw_mtpr	$2, 0x118	

	hw_mtpr	$31, 0x20D				
	lda	$1, 0x1($31)		
	sll	$1, 32, $1		
	hw_mtpr	$1, 0x20E			
	hw_mtpr	$31, 0x10F			
	hw_mtpr	$31, 0x105			
	hw_mtpr	$31, 0x20A			
	hw_mtpr	$31, 0x201			
	hw_mtpr	$31, 0x200			
	hw_mtpr	$31, 0x103	
	hw_mtpr	$31, 0x10A			
	hw_mtpr	$31, 0x109			
	hw_mtpr	$31, 0x108			


	lda		$1, (2 << 1)($31)	
	hw_mtpr	$1, 0x20F			













initGlobals:
	ldah	$0, 0xfff0($31)		
	zap	$0, 0xE0, $13
	    lda	    $14, 0x874($31);	            sll	    $14, 28              , $14;		
	bsr	$28, ClearCPUErrors	

	hw_ldq/p	$2, 0x108	($13)		







InitPCI:
					
	bis	$31, $31, $17		
	    lda	    $16, 0x876	($31);	    sll	    $16, 28	, $16;	
	lda	$16, 0x0400($16)	
	lda	$7, 4($31)		

wbase_loop:
	bsr	$26, srom_wrtest	
	lda	$16, 0x100($16)		
	lda	$7,  -1($7)			
	bgt	$7,  wbase_loop		






	lda $17, ( 0xAE170027&0xffff)($31)		;  ldah $17, (( 0xAE170027+0x8000)>>16)($17)	; 
	ldah    $16, 0x1000($14)	
	lda		$16, 0x500($16)		
	hw_stl/p	$17, 0($16)			
	mb

	bis	$31, $31, $17		
	lda	$16, 0x2000	($14)	
	bsr	$26, srom_wrtest	
	lda	$16, 0x440	($14)	
	bsr	$26, srom_wrtest	

	lda	$16, 0xC0	($14)	
	lda $17, ( 0xFF00&0xffff)($31)		;  ldah $17, (( 0xFF00+0x8000)>>16)($17)	; 		
	bsr	$26, srom_wrtest	





















	lda	$16, 0x100	($14)	
	lda $17, ( ((0 << 0)		| 			(0 << 2) 	| 			(0 << 3) 		| 			(1 << 4)		| 			(1 << 5) 	| 			(1 << 6) 	| 			(1 << 7)	| 			(0 << 8)		| 			(0 << 9)		| 			(0 << 10)		| 			(0 << 11)		| 			(0 << 12)		| 			(0 << 13)	| 			(0 << 20) 		| 			(0 << 24) 		| 			(0 << 28)		)&0xffff)($31)		;  ldah $17, (( ((0 << 0)		| 			(0 << 2) 	| 			(0 << 3) 		| 			(1 << 4)		| 			(1 << 5) 	| 			(1 << 6) 	| 			(1 << 7)	| 			(0 << 8)		| 			(0 << 9)		| 			(0 << 10)		| 			(0 << 11)		| 			(0 << 12)		| 			(0 << 13)	| 			(0 << 20) 		| 			(0 << 24) 		| 			(0 << 28)		)+0x8000)>>16)($17)	; 
	bsr	$26, srom_wrtest	

	lda $16, ( 200000&0xffff)($31)		;  ldah $16, (( 200000+0x8000)>>16)($16)	;  		
	bsr	$27, wait_n_cycles 	

	lda	$16, 0x100	($14)	
	bis	$17, (1 << 0), $17		
	bsr	$26, srom_wrtest	

	lda $16, ( 2000000&0xffff)($31)		;  ldah $16, (( 2000000+0x8000)>>16)($16)	;  		
	bsr	$27, wait_n_cycles 	






	lda	$16, 0x140	($14)	
	bis		$31, (1 << 0), $17
	bsr	$26, srom_wrtest	















	lda	$16, 0x200	($14)	
	lda $17, ( ((0xF << 0)		| 			(7 << 4) 	| 			(0xF << 8) 	| 			(0 << 12) 	| 			(0 << 13))&0xffff)($31)		;  ldah $17, (( ((0xF << 0)		| 			(7 << 4) 	| 			(0xF << 8) 	| 			(0 << 12) 	| 			(0 << 13))+0x8000)>>16)($17)	;    
	bsr	$26, srom_wrtest	












	ldah	$8, 0x6000($14)			
	lda	$16, 0x1C0	($8)	
	lda $17, ( 		   ((3 << 0	) | 			(3 << 4	) 	| 			(0 << 8	) 		| 			(0 << 16	))&0xffff)($31)		;  ldah $17, (( 		   ((3 << 0	) | 			(3 << 4	) 	| 			(0 << 8	) 		| 			(0 << 16	))+0x8000)>>16)($17)	;   
	hw_ldl/p	$0, 0($16)				
	bic		$0, 0x7F, $0			
	bis		$17, $0, $17			
	bsr	$26, srom_wrtest			

	lda		$16, 0x40	($8)	
	bis		$31, $31, $17
	hw_stq/p	$17, 0($16)				
	mb
	hw_ldq/p	$17, 0($16)

	lda		$16, 0x00	($8)	
	lda		$17, -1($31)			
	srl		$17, 1, $17				
	hw_stq/p	$17, 0($16)				
	mb
	hw_ldq/p	$17, 0($16)

	lda		$16, 0xC0	($8)
	bis		$31, 0xC6, $17			
	bsr	$26, srom_wrtest			








	ldah	$8, 0x6000($14)			
	lda	$16, 0x140	($8)	
	bis	$31, ((1 << 2	) | (1 << 1	) | 						   (1 << 6	)), $17	
	bsr	$26, srom_wrtest			

		lda $16, ( 100000&0xffff)($31)		;  ldah $16, (( 100000+0x8000)>>16)($16)	; 	; 	bsr		$27, usdelay	;		 














	    lda	    $14, 0x874($31);	            sll	    $14, 28              , $14;		
	ldah	$8, 0x4000($14)	
	lda	$16, 0x000	($8)		
	lda $17, ( ((1 << 0	)	| 			(3 << 4	) 	| 			(2 << 8	) 		| 			(1 << 10	) 		| 			(1 << 12	) 		| 			(0 << 16	) 		| 			(0 << 17	) 		| 			(1 << 18	) 		| 			(0x29 << 24	)		)&0xffff)($31)		;  ldah $17, (( ((1 << 0	)	| 			(3 << 4	) 	| 			(2 << 8	) 		| 			(1 << 10	) 		| 			(1 << 12	) 		| 			(0 << 16	) 		| 			(0 << 17	) 		| 			(1 << 18	) 		| 			(0x29 << 24	)		)+0x8000)>>16)($17)	; 
	bsr	$26, srom_wrtest		

	lda	$16, 20000($31) 	
	bsr	$27, wait_n_cycles 	





















	bsr	$29, ReadConfJumper	
	srl	$0, 1, $0
	and $0, 7, $1

	bis		$31, 6, $3	
	bis		$31, 0x98, $4	
	cmpeq	$1, 7, $2	
	cmovne	$2, 6, $3
	cmovne	$2, 0x98, $4
 
 
 
	cmpeq	$1, 5, $2	
	cmovne	$2, 8, $3
	cmovne	$2, 0xA0, $4
 
 
 
 
 
 
 
 
 
 
 
 
 
	lda	$4, 0x100($4)	

	ldah	$8, 0x6000($14)			
	hw_ldl/p	$0, 0x1C0	($8)	
	 
	ldah	$2, 1($31)
	and	$0, $2, $2
	bne	$2, NotPass0






	bis	$3, 0x20, $3
	sll	$3, 11, $17
	lda	$16, 0x1C0	($8)	 
	bsr	$26, srom_wrtest		

	
	ldah	$16, 0x5000	($14)	
	lda	$16, 0x140	($16)		
	lda	$17, 1($31)
	bsr	$26, srom_wrtest
		lda $16, ( 100&0xffff)($31)		;  ldah $16, (( 100+0x8000)>>16)($16)	; 	; 	bsr		$27, usdelay	;

	br	$26, SoftReset

NotPass0:

	ldah	$16, 0x5000	($14)	
	lda	$16, 0x140	($16)		
	hw_ldl/p	$17, 0($16)
	blbc	$17, NoReset

	
	xor	$17, 1, $17
	bsr	$26, srom_wrtest

	
	bsr	$28, CpuSetSpeed				




SoftReset:

	ldah	$16, 0x4000($14)		
	lda	$16, 0x900($16)
	lda $17, ( 0xDEAD&0xffff)($31)		;  ldah $17, (( 0xDEAD+0x8000)>>16)($17)	; 
	hw_stl/p	$17, 0($16)			
	mb
	hw_ldl/p	$17, 0($16)			

StallLoop:
	hw_mfpr    $31, 0x140						
	hw_mfpr    $31, 0x140
	hw_mfpr    $31, 0x140
	br	$31, StallLoop

NoReset:

	
	bsr		$26, InitCypress			






	lda $17, ( 0xAE170027&0xffff)($31)		;  ldah $17, (( 0xAE170027+0x8000)>>16)($17)	; 
	ldah    $16, 0x1000($14)	
	lda	$16, 0x500($16)		
	hw_stl/p	$17, 0($16)		
	mb





	ldah	$16, 0x1000($14)	
	lda		$8, 0x40($16)
	lda $17, ( 0x10ff&0xffff)($31)		;  ldah $17, (( 0x10ff+0x8000)>>16)($17)	; 
	hw_stl/p	$17, 0($8)			
	mb


		bis	$31, 0x20, $2	; 		bsr	$29, WriteLEDS	; 	





DetectSpeed:













					
	bsr 	$28, GetCPUSpeed	
	bis	$0, $0, $17			
	lda	$16, 9600($31)		
	bsr	$26, nintdivide		

 
		lda	$29, 5($31);		bis	$31, $0, $28;				bsr	$0, minidebugger ;			
 

		lda	$29, 1($31);		lda $28, (   	( (0x50 << 24) | (0x34 << 16) | (0x36 << 8) | (0x31 << 0) )&0xffff)($31)		;  ldah $28, ((   	( (0x50 << 24) | (0x34 << 16) | (0x36 << 8) | (0x31 << 0) )+0x8000)>>16)($28)	; ; sll  $28, 32, $28; lda  $28, ( 	( (0x31 << 24) | (0x32 << 16) | (0x0d << 8) | (0x0a << 0) )&0xffff)($28); ldah $28, (( 	( (0x31 << 24) | (0x32 << 16) | (0x0d << 8) | (0x0a << 0) )+0x8000)>>16)($28);;			bsr	$0, minidebugger ; 
		bis	$31, 0x01, $2	; 		bsr	$29, WriteLEDS	; 	




	bis	$17, $17, $16		
	lda $3, ( 0xe8d4a510&0xffff)($31)		;  ldah $3, (( 0xe8d4a510+0x8000)>>16)($3)	; 	
	zap	$3, 0xf0, $17		
	sll	$17, 8, $17			
	bsr	$26, nintdivide		
	bis $0, $0, $24			

 		bis	$31, 0x02, $2	; 		bsr	$29, WriteLEDS	; 	


		lda	$29, 6($31);		bsr	$0, minidebugger ;		































































ReadConfigJumpers:











	bsr	$29, ReadConfJumper	
	ornot	$31, $0, $1		
	and	$1, 0x01, $30		
	sll	$30, 7, $30

	sll	$1, 2, $1		
	and	$1, 0x40, $1		
	bis	$30, $1, $30		

	bis	$30, 0x20, $30		
	sll	$30, 17, $30
	bis	$31, 1, $3			
	and	$0, 0xC0, $1		
	cmpeq	$1, 0x80, $2	
	cmovne	$2, 2, $3
	cmpeq	$1, 0x40, $2	
	cmovne	$2, 3, $3
	cmpeq	$1, 0xC0, $2	
	cmovne	$2, 4, $3

	sll	$3, 16, $3			
	bis	$3, $30, $30		



		bis	$31, 0x03, $2	; 		bsr	$29, WriteLEDS	; 	


# 942 "srom.s"





























	
	lda $8, ( 0x4010&0xffff)($31)		;  ldah $8, (( 0x4010+0x8000)>>16)($8)	; 			
	lda $21, ( 0x238430&0xffff)($31)		;  ldah $21, (( 0x238430+0x8000)>>16)($21)	; 			

	lda	$0, 2551($31)				
	lda $1, ( 0x238430&0xffff)($31)		;  ldah $1, (( 0x238430+0x8000)>>16)($1)	; 			
	cmpult	$24, $0, $0				
	cmovne	$0, $1, $21				

	lda	$0, 2189($31)				
	lda $1, ( 0x258740&0xffff)($31)		;  ldah $1, (( 0x258740+0x8000)>>16)($1)	; 			
	cmpult	$24, $0, $0				
	cmovne	$0, $1, $21				

	lda	$0, 1914($31)				
	lda $1, ( 0x348640&0xffff)($31)		;  ldah $1, (( 0x348640+0x8000)>>16)($1)	; 			
	cmpult	$24, $0, $0				
	cmovne	$0, $1, $21				
	
	lda	$0, 1700($31)				
	lda $1, ( 0x27CA50&0xffff)($31)		;  ldah $1, (( 0x27CA50+0x8000)>>16)($1)	; 			
	cmpult	$24, $0, $0				
	cmovne	$0, $1, $21				
	
	lda	$0, 1531($31)				
	lda $1, ( 0x27CB50&0xffff)($31)		;  ldah $1, (( 0x27CB50+0x8000)>>16)($1)	; 			
	cmpult	$24, $0, $0				
	cmovne	$0, $1, $21				



















set_bc_size:	
	srl	$30, 16	, $2 
					
	and	$2, ((((1<<4)-1) << 16	) >> 16	), $2		
	subq	$2, 1, $3		
	sll	$3, 12, $3	
	bis	$21, $3, $21		

	
	lda $3, ( ((1<<23) | (1<<24))&0xffff)($31)		;  ldah $3, (( ((1<<23) | (1<<24))+0x8000)>>16)($3)	; 
	bic $21, $3, $21

	lda $3, ( (1<<31)&0xffff)($31)		;  ldah $3, (( (1<<31)+0x8000)>>16)($3)	; 		
	bis	$21, $3, $21			

	zap	$21, 0xf0, $21		

	bic	$21, $3, $19			
	hw_stq/p	$19, 0x008	($13)		
	mb
	hw_ldl/p	$0,  0x108	($13)	
	mb

		bis	$31, 0x04, $2	; 		bsr	$29, WriteLEDS	; 	

	bis	$8, $8, $20			

		bis	$31, 0x05, $2	; 		bsr	$29, WriteLEDS	; 	


		lda	$29, 6($31);		bsr	$0, minidebugger ;		
























	    lda	    $14, 0x874($31);	            sll	    $14, 28              , $14;		
	ldah	$8, 0x4000($14)	
	lda	$16, 0x000	($8)		
	lda $17, ( ((1 << 0	)	| 			(3 << 4	) 	| 			(2 << 8	) 		| 			(1 << 10	) 		| 			(1 << 12	) 		| 			(0 << 16	) 		| 			(0 << 17	) 		| 			(1 << 18	) 		| 			(0x29 << 24	)		)&0xffff)($31)		;  ldah $17, (( ((1 << 0	)	| 			(3 << 4	) 	| 			(2 << 8	) 		| 			(1 << 10	) 		| 			(1 << 12	) 		| 			(0 << 16	) 		| 			(0 << 17	) 		| 			(1 << 18	) 		| 			(0x29 << 24	)		)+0x8000)>>16)($17)	; 
	bsr	$26, srom_wrtest		

	lda	$16, 20000($31) 	
	bsr	$27, wait_n_cycles 	
					

	lda $17, ( 0x132&0xffff)($31)		;  ldah $17, (( 0x132+0x8000)>>16)($17)	; 		
	ldah    $8, 0x1000($14)		
	lda	$16, 0x200	($8) 		
	bsr	$26, srom_wrtest		

	lda $17, ( 0x750&0xffff)($31)		;  ldah $17, (( 0x750+0x8000)>>16)($17)	; 		
	lda	$16, 0x300	($8) 		
	bsr	$26, srom_wrtest		




	bis	$31, 1, $4				
	sll	$4, 15	, $4
	bis	$17, $4, $17



	lda	$5, 8($31)				
ForceRef:
	lda		$16, 0x300	($8)		
	bsr		$26, srom_wrtest

	lda	$4, 100($31)
ForceRefWait:
	hw_mfpr    $31, 0x140
	subq	$4, 1, $4
	bne		$4, ForceRefWait

	subq	$5, 1, $5
	bne		$5, ForceRef

	bis		$17, $31, $5		







	ldah    $16, 0x1000($14)	
	lda $17, ( 0x3A1C01&0xffff)($31)		;  ldah $17, (( 0x3A1C01+0x8000)>>16)($17)	; 		
	bsr	$26, srom_wrtest		




	bis		$5, $31, $17		
	lda	$5, 8($31)				
ForceRef1:
	lda		$16, 0x300	($8)		
	bsr		$26, srom_wrtest

	lda	$4, 100($31)
ForceRefWait1:
	hw_mfpr    $31, 0x140
	subq	$4, 1, $4
	bne		$4, ForceRefWait1

	subq	$5, 1, $5
	bne		$5, ForceRef1




	bis	$31, $31, $17			
	lda	$16, 0x600	($8) 		
	bsr	$26, srom_wrtest		

	lda	$17, 0x100($31)			
	lda	$16, 0x640	($8) 		
	bsr	$26, srom_wrtest		

	lda	$17, 0x22($31)			
	lda	$16, 0xA00	($8) 		
	bsr	$26, srom_wrtest		

	lda	$16, 0xA40	($8) 		
	bsr	$26, srom_wrtest		

	lda	$17, 0x28($31)			
	lda	$16, 0x800	($8) 		
	bsr	$26, srom_wrtest		

	lda	$16, 0x840	($8) 		
	bsr	$26, srom_wrtest		

	bis	$17, 1, $17				
	lda	$16, 0x800	($8) 		
	bsr	$26, srom_wrtest		

	lda	$16, 0x840	($8) 		
	bsr	$26, srom_wrtest		

	lda $23, ( 0x8000000&0xffff)($31)		;  ldah $23, (( 0x8000000+0x8000)>>16)($23)	; 		

		bis	$31, 0x07, $2	; 		bsr	$29, WriteLEDS	; 	


		lda	$29, 6($31);		bsr	$0, minidebugger ;		


# 2236 "srom.s"







		bis	$31, 0x0C, $2	; 		bsr	$29, WriteLEDS	; 	


















	lda	$2, (1<<0)($31)	
	hw_mtpr	$2, 0x216			
	hw_mfpr    $31, 0x140				
	hw_mfpr    $31, 0x140				
	hw_mfpr    $31, 0x140
	hw_mfpr    $31, 0x140
	hw_mfpr    $31, 0x140

		bis	$31, 0x0D, $2	; 		bsr	$29, WriteLEDS	; 	

# 2439 "srom.s"































	mb
	hw_stq/p	$20, 0x188	($13)	
	mb
	hw_stq/p	$21, 0x008	($13)		
	mb
	hw_ldl/p	$0,  0x108	($13)	
	mb

		bis	$31, 0x13, $2	; 		bsr	$29, WriteLEDS	; 	


		lda	$29, 6($31);		bsr	$0, minidebugger ;		









	bsr	$28, ClearMemContErrors
	bsr	$28, ClearCPUErrors

		bis	$23, $23, $7
SweepMemory2:
		subq	$7, 8, $7			
		hw_ldq/p   $2, 0x00($7)		
		hw_stq/p   $2, 0x00($7)		
		bgt     $7, SweepMemory2	

		bis	$31, 0x14, $2	; 		bsr	$29, WriteLEDS	; 	


		lda	$29, 6($31);		bsr	$0, minidebugger ;		









	bis	$30, $30, $16
	bsr	$14, LoadSystemCode
		bis	$31, 0x17, $2	; 		bsr	$29, WriteLEDS	; 	











































	lda	$2, 0x4000($31)		
	cmpult $8, $2, $0		
	cmovne $0, $2, $8		

	bis	$8, 1, $0		
	hw_mtpr	$0, 0x10B			
					
	lda $2, ( 0x77FF0119&0xffff)($31)		;  ldah $2, (( 0x77FF0119+0x8000)>>16)($2)	; 		
	hw_stl/p	$2, 0($8)		

	lda $2, ( 0xC0000001&0xffff)($31)		;  ldah $2, (( 0xC0000001+0x8000)>>16)($2)	; 		
	hw_stl/p	$2,  4($8)		
	hw_stl/p	$25, 8($8)		
	lda $2, ( 0x6C008000&0xffff)($31)		;  ldah $2, (( 0x6C008000+0x8000)>>16)($2)	; 		
	hw_stl/p	$2, 12($8)		
	
	lda $2, ( 0x47FF041F&0xffff)($31)		;  ldah $2, (( 0x47FF041F+0x8000)>>16)($2)	; 		
	lda	$5, 44($31)		
	lda	$8, 16($8)		
loadInst:				
	hw_stl/p	$2, 0($8)		
	subq	$5, 1, $5		
	lda	$8, 4($8)		
	bne	$5, loadInst		

	lda $2, ( 0x6BE00000&0xffff)($31)		;  ldah $2, (( 0x6BE00000+0x8000)>>16)($2)	; 		
	hw_stl/p	$2, 0($8)		
	mb				

		bis	$31, 0x18, $2	; 		bsr	$29, WriteLEDS	; 	

	bsr	$28, ClearMemContErrors
	bsr	$28, ClearCPUErrors	

	srl	$0, 8, $2		
	and	$2, 0xF, $2		



	lda $1, (  0x00000001&0xffff)($31)		;  ldah $1, ((  0x00000001+0x8000)>>16)($1)	; ; sll  $1, 32, $1; lda  $1, ( 0x00000000&0xffff)($1); ldah $1, (( 0x00000000+0x8000)>>16)($1); 
	extbl	$1, $2, $1		

EV56_Detected:
	sll	$1, 32, $1		
	bis	$1, 9, $25		

































NextLevel:
	lda	$2, 0x19($31)
	srl	$30, 23	, $7	
	cmovlbs	$7, 0x3A, $2		
		bis	$31, $2, $2	; 		bsr	$29, WriteLEDS	; 	

	bis	$20, $20, $1		
	bis	$21, $21, $2		
	bis	$22, $22, $3		
	lda $15, (  0x56000000&0xffff)($31)		;  ldah $15, ((  0x56000000+0x8000)>>16)($15)	; ; sll  $15, 32, $15; lda  $15, ( 0x01010101&0xffff)($15); ldah $15, (( 0x01010101+0x8000)>>16)($15);
	bis	$25, $25, $16		
	bis	$23, $23, $17		
	bis	$24, $24, $18		
	lda $19, ( 0xDECB0001&0xffff)($31)		;  ldah $19, (( 0xDECB0001+0x8000)>>16)($19)	; 	
	zap	$19, 0xf0, $19		
	lda	$20, 1($31)		
	lda	$21, 0($31)		


	blbc	$7, no_minidbg
		lda	$29, 0($31);		bsr	$0, minidebugger ;		
no_minidbg:

	hw_rei				

































CpuSetSpeed:
	    lda	    $7, 0x874($31);	            sll	    $7, 28              , $7;		
	ldah	$8, 0x6000($7)		
	lda	$16, 0x180($8)		

	lda	$17, 0x0($31)
	bsr	$26, srom_wrtest	
		lda $16, ( 100&0xffff)($31)		;  ldah $16, (( 100+0x8000)>>16)($16)	; 	; 	bsr		$27, usdelay	;

	lda	$5, 11($31)		
	zap	$5, 0xfe, $5
CpuSetSpeedLoop:
	bic	$17, 0xC, $17
	srl	$4, $5, $1
	blbc	$1, DataBitClear
	bis	$17, 4, $17
DataBitClear:
	    lda	    $7, 0x874($31);	            sll	    $7, 28              , $7;		
	ldah	$8, 0x6000($7)		
	lda	$16, 0x180($8)		
	bsr	$26, srom_wrtest	
		lda $16, ( 100&0xffff)($31)		;  ldah $16, (( 100+0x8000)>>16)($16)	; 	; 	bsr		$27, usdelay	;

	    lda	    $7, 0x874($31);	            sll	    $7, 28              , $7;		
	ldah	$8, 0x6000($7)		
	lda	$16, 0x180($8)		
	bis	$17, 8, $17
	bsr	$26, srom_wrtest	
		lda $16, ( 100&0xffff)($31)		;  ldah $16, (( 100+0x8000)>>16)($16)	; 	; 	bsr		$27, usdelay	;
	subq	$5, 1, $5
	bge	$5, CpuSetSpeedLoop

	    lda	    $7, 0x874($31);	            sll	    $7, 28              , $7;		
	ldah	$8, 0x6000($7)		
	lda	$16, 0x180($8)		
	lda	$17, 0x2($31)
	bsr	$26, srom_wrtest	
		lda $16, ( 100&0xffff)($31)		;  ldah $16, (( 100+0x8000)>>16)($16)	; 	; 	bsr		$27, usdelay	;

	    lda	    $7, 0x874($31);	            sll	    $7, 28              , $7;		
	ldah	$8, 0x6000($7)		
	lda	$16, 0x180($8)		
	lda	$17, 0x0($31)
	bsr	$26, srom_wrtest	
		lda $16, ( 100&0xffff)($31)		;  ldah $16, (( 100+0x8000)>>16)($16)	; 	; 	bsr		$27, usdelay	;

	ret	$31, ($28)


# 3209 "srom.s"



 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 

srom_usdelay:
	bis		$16, $16, $20		 
	 
	 
	 
	lda		$0, 667($31)		 
	mulq	$0, $16, $0			 
usdelay_init:
	lda		$20, 1($31)			 
	sll		$20, 32, $20		 
	hw_mtpr	$20, 0x20E				 
	hw_mfpr	$31, 0x140			 
	hw_mfpr	$31, 0x140
	hw_mfpr	$31, 0x140
	hw_mfpr	$31, 0x140
usdelay_loop:					 
	rpcc	$20					 
	zapnot	$20, 0x0f, $20		 
	cmplt	$0, $20, $20		 
	beq		$20, usdelay_loop	 
	ret		$31, ($30)			 

usdelay:						 
	bsr	$0,  saveRegisters
	bsr	$30, srom_usdelay
	br	$31, restoreRegisters

 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 

saveRegisters:			 
    hw_mtpr	$30, 0x141	 
    hw_mtpr	$2,  0x142	 
	hw_mtpr	$3,  0x143
	hw_mtpr	$4,  0x144
	hw_mtpr	$5,  0x145
	hw_mtpr	$6,  0x146
	hw_mtpr	$7,  0x147
	hw_mtpr	$8,  0x148
    hw_mtpr	$9,  0x149
    hw_mtpr	$16, 0x14A
	hw_mtpr	$17, 0x14B
    hw_mtpr	$18, 0x14C
    hw_mtpr	$19, 0x155
	hw_mtpr	$20, 0x14E
	hw_mtpr	$21, 0x14F
	hw_mtpr	$22, 0x150
    hw_mtpr	$26, 0x151
    hw_mtpr	$27, 0x152
    hw_mtpr	$28, 0x153
    hw_mtpr	$29, 0x154
	ret	$31, ($0)        

restoreRegisters:		 
    hw_mfpr	$30, 0x141	 
    hw_mfpr	$2,  0x142	 
	hw_mfpr	$3,  0x143
	hw_mfpr	$4,  0x144
	hw_mfpr	$5,  0x145
	hw_mfpr	$6,  0x146
	hw_mfpr	$7,  0x147
	hw_mfpr	$8,  0x148
    hw_mfpr	$9,  0x149
    hw_mfpr	$16, 0x14A
    hw_mfpr	$17, 0x14B
    hw_mfpr	$18, 0x14C
    hw_mfpr	$19, 0x155
    hw_mfpr	$20, 0x14E
    hw_mfpr	$21, 0x14F
    hw_mfpr	$22, 0x150
    hw_mfpr	$26, 0x151
    hw_mfpr	$27, 0x152
    hw_mfpr	$28, 0x153
    hw_mfpr	$29, 0x154
	nop
    ret     $31, ($27)




















ClearCPUErrors:
	hw_mtpr	$31, 0x10C				
					
	hw_mfpr	$31, 0x206					


	lda	$2, ((1<<11) | (1<<12)  | (1<<13))($31)	
	hw_mtpr	$2, 0x11A				


	lda	$2, ((1<<1) | (1<<0))($31)	
	hw_mtpr	$2, 0x212				






	ldah	$2, (( (1<<27) | (1<<28) | (1<<29) | 			 (1<<32) | (1<<33)) >> 16 )($31)
	hw_mtpr	$2, 0x115	

	hw_ldq/p	$0, 0x108	($13)		
	ret	$31, ($28)




















ClearMemContErrors:
	mb
	    lda	    $14, 0x874($31);	            sll	    $14, 28              , $14;		
	lda	$2, 0x4000($14)		
	lda	$2, 0x4200($2)		
	hw_ldl/p	$1, 0x0($2)		
	hw_stl/p	$1, 0x0($2)		
	mb
	hw_ldl/p	$1, 0x0($2)		
	ret	$31, ($28)
































InitCypress:




	lda	$16, 0xF($31)			
	bsr	$29, WriteCY_CMD_REG	

	lda	$16, 0x80($31)			
	bsr	$29, WriteCY_INTA_ROUTE	

	lda	$16, 0x80($31)			
	bsr	$29, WriteCY_INTB_ROUTE	

	lda	$16, 0x80($31)			
	bsr	$29, WriteCY_INTC_ROUTE	

	lda	$16, 0x80($31)			
	bsr	$29, WriteCY_INTD_ROUTE	

	lda	$16, 0x27($31)			
								
								
	bsr	$29, WriteCY_CTRL_REG	

	lda	$16, 0xE0($31)			
	bsr	$29, WriteCY_ERR_CTRL_REG	

	lda	$16, 0xF0($31)			
	bsr	$29, WriteCY_TOP_OF_MEM	

	lda	$16, 0x40($31)				
	bsr	$29, WriteCY_ATCTRL_REG1	

	lda	$16, 0x80($31)			
	bsr	$29, WriteCY_IDE0_ROUTE	

	lda	$16, 0x80($31)			
	bsr	$29, WriteCY_IDE1_ROUTE	

	lda	$16, 0x70($31)			
	bsr	$29, WriteCY_SA_USB_CTRL	

	ret	$31, ($26)



























wait_n_cycles:	
	lda	$1, 1($31)		
	sll	$1, 32, $1		
	rpcc	$3			
time_loop:
	rpcc	$0			
	subl	$0, $3, $0		
	bge 	$0, nowrap		
	addq	$1, $0, $0		
nowrap:
	cmpult	$0, $16, $2		
	bne 	$2, time_loop		
	ret	$31, ($27)
























WriteCY_CMD_REG:
	lda	$1, 0x04($31) 
	br	$31, WriteConf

WriteCY_INTA_ROUTE:
	lda	$1, 0x40($31) 
	br	$31, WriteConf

WriteCY_INTB_ROUTE:
	lda	$1, 0x41($31) 
	br	$31, WriteConf

WriteCY_INTC_ROUTE:
	lda	$1, 0x42($31) 
	br	$31, WriteConf

WriteCY_INTD_ROUTE:
	lda	$1, 0x43($31) 
	br	$31, WriteConf

WriteCY_CTRL_REG:
	lda	$1, 0x44($31) 
	br	$31, WriteConf

WriteCY_ERR_CTRL_REG:
	lda	$1, 0x45($31) 
	br	$31, WriteConf

WriteCY_TOP_OF_MEM:
	lda	$1, 0x48($31) 
	br	$31, WriteConf

WriteCY_ATCTRL_REG1:
	lda	$1, 0x49($31) 
	br	$31, WriteConf

WriteCY_IDE0_ROUTE:
	lda	$1, 0x4B($31) 
	br	$31, WriteConf

WriteCY_IDE1_ROUTE:
	lda	$1, 0x4C($31) 
	br	$31, WriteConf

WriteCY_SA_USB_CTRL:
	lda	$1, 0x4D($31) 
	br	$31, WriteConf

WriteConf:
	and	$1, 3, $3					
	insbl	$16, $3, $2				
	sll	$1, 5, $1	
	    lda	    $3, 0x870($31);	    sll	    $3, 28              , $3;		
	ldah	$16, (19-11)($31)		
	bis	$3, $16, $3					
	br	$31, WriteData

WriteLEDS:
	cmpeq	$2, 0x20, $1		
	bne	$1, skipSromPort	

	bis	$29, $29, $1
		lda	$29, 3($31);		lda	$28, 0x2e($31);				bsr	$0, minidebugger ;
		lda	$29, 7($31);		bis	$31, $2, $28;				bsr	$0, minidebugger ;
	bis	$1, $1, $29
skipSromPort:
	lda	$1, 0x80($31)	
	br	$31, WriteCont

WriteSMC_Config1:
WriteSMC_Index1:
	lda	$1, 0x370($31) 
	br	$31, WriteCont_std	
WriteSMC_Data1:
	lda	$1, 0x371($31)	
	br	$31, WriteCont_std

WriteRTCAddr:
	lda	$1, 0x70($31) 
	br	$31, WriteCont_std
WriteRTCData:
	lda	$1, 0x71($31)	
	br	$31, WriteCont_std
WriteSIO_NMISC:
	lda	$1, 0x61($31)	
	br	$31, WriteCont_std
WriteSIO_TMR2:
	lda	$1, 0x42($31) 
	br	$31, WriteCont_std
WriteSIO_TCW:
	lda	$1, 0x43($31)	

WriteCont_std:
	bis	$16, $16, $2		
WriteCont:

	and	$1, 3, $3		
	insbl	$2, $3, $2		
	sll	$1, 5, $1 
	    lda	    $3, 0x858($31);		    sll	    $3, 28              , $3;		

WriteData:
	bis	$1, $3, $1		
	hw_stl/p	$2, 0x00($1)		
	mb				
	ret	$31, ($29)		
























ReadRTCData:
	lda	$1, 0x71($31)	
	sll	$1, 5, $2 
	    lda	    $3, 0x858($31);		    sll	    $3, 28              , $3;		
ReadData:
	bis	$2, $3, $2			
	hw_ldl/p	$0, 0x00($2)		
	and	$1, 3, $1		
	extbl	$0, $1, $0		
	ret	$31, ($29)		




















ReadConfJumper:
	    lda	    $3, 0x874($31);	            sll	    $3, 28              , $3;		
	ldah	$3, 0x6000($3)	
	hw_ldl/p	$0, 0x00	($3)	
	srl		$0, 24, $0
	zap		$0, 0xFE, $0
	ret	$31, ($29)			







































intdivide:
	bis	$31, $31, $0	
	cmplt	$17, $16, $1	
	bne	$1, divide_done	
divide:
	lda	$1, 1($31)	
divide1:
	addq	$0, $1, $4	
	mulq	$4, $16, $2	
	sll	$1, 1, $1	
	cmplt	$2, $17, $3	
	bne	$3, divide1	
	srl	$1, 2, $1	
	addq	$0, $1, $0	
	cmplt	$1, 2, $3	
	beq	$3, divide	
divide_done:
	ret	$31, ($27)














updivide:
	bsr	$27, intdivide	
	mulq	$16, $0, $1	
	cmplt	$1, $17, $2	
	blbc	$2, updivide_done
	addq	$0, 1, $0	
updivide_done:
	ret	$31, ($26)
















nintdivide:
	bsr	$27, intdivide	
	mulq	$16, $0, $1	
	cmplt	$1, $17, $2	
	blbc	$2, nintdivide_done
	addq	$0, 1, $0	
	mulq	$16, $0, $2	
	subq	$2, $17, $2
	subq	$17, $1, $3
	cmplt	$3, $2, $1	
	blbc	$1, nintdivide_done
	subq	$0, 1, $0	
nintdivide_done:
	ret	$31, ($26)






















srom_wrtest:
	zap	$17, 0xF0, $17		
	hw_stl/p	$17, 0($16)		
	mb				
srom_rdtest:
	hw_ldl/p	$18, 0($16)		
	zap	$18, 0xF0, $18		
	xor	$18, $17, $19		



















	ret	$31, ($26)




























FatalError:
		bis	$31, 0x3F, $2	; 		bsr	$29, WriteLEDS	; 			
	lda	$17, 500($31)		
	bsr	$27, wait_for_pif
		bis	$31, $8, $2	; 		bsr	$29, WriteLEDS	; 				
	bis	$8, $8, $19
	bsr	$28, Beep		
	lda	$17, 1000($31)		
	bsr	$27, wait_for_pif

	srl	$30, 23	, $1	
	blbs	$1, FatalError		

		lda	$29, 0($31);		bsr	$0, minidebugger ;		
	br	$31, FatalError























Beep:
	lda	$16, 3($31)		
	bsr	$29, WriteSIO_NMISC
	lda	$16, 0xB6($31)		
	bsr	$29, WriteSIO_TCW	
	lda	$16, 0xE2($31)		
					
	bsr	$29, WriteSIO_TMR2	
	lda	$16, 3($31)		
	bsr	$29, WriteSIO_TMR2

	lda	$17, 100($31)		
	bsr	$27, wait_for_pif	

	lda	$16, 0xB2($31)		
	bsr	$29, WriteSIO_TCW
	lda	$16, 0($31)
	bsr	$29, WriteSIO_TMR2	
	bsr	$29, WriteSIO_TMR2	
	bsr	$29, WriteSIO_NMISC	

	lda	$17, 100($31)		
	bsr	$27, wait_for_pif	
	subq	$19, 1, $19		
	bne	$19, Beep		
	ret	$31, ($28)






























GetCPUSpeed:










# 4091 "srom.s"





	lda	$16, 0xA	($31)	
	bsr	$29, WriteRTCAddr
	lda	$16, ((2 << 4	) | (7 << 0	))($31)
	bsr	$29, WriteRTCData	






	lda	$16, 0xB	($31)	
	bsr	$29, WriteRTCAddr
 
	lda	$16, ((1 << 6	) | (1 << 2	) | (1 << 1	))($31)
	bsr	$29, WriteRTCData	

	bsr	$29, ReadConfJumper	
	srl	$0, 1, $0
	and $0, 7, $1

	lda $0, ( 399984*1000&0xffff)($31)		;  ldah $0, (( 399984*1000+0x8000)>>16)($0)	; 
	cmpeq	$1, 7, $2	
	lda $4, ( 399984*1000&0xffff)($31)		;  ldah $4, (( 399984*1000+0x8000)>>16)($4)	; 
	cmovne	$2, $4, $0
 
 
 
	cmpeq	$1, 5, $2	
	lda $4, ( 533312*1000&0xffff)($31)		;  ldah $4, (( 533312*1000+0x8000)>>16)($4)	; 
	cmovne	$2, $4, $0
 
 
 
 
 
 
 
 
 




	ret	$31, ($28)






















wait_for_pif:
	lda	$16, 0xC	($31)
	bsr	$29, WriteRTCAddr
	bsr	$29, ReadRTCData	
	srl	$0, 6, $0	
	blbs	$0, wait_for_pif	

wait_assert:
	bsr	$29, WriteRTCAddr
	bsr	$29, ReadRTCData	
	srl	$0, 6, $0	
	blbc	$0, wait_assert		
	subq	$17, 1, $17		
	bne	$17, wait_for_pif
	ret	$31, ($27)







































































































































































































































































LoadSystemCode:
					
	lda	$10, ((1 << 8) | 4)($31) 
	lda	$1, ((2 << 8) | 4)($31) 
	srl	$16, 24	, $0	
	cmovlbc	$0, $1, $10		

scanROM:	
	bis	$31, $31, $12		
	bis	$31, $31, $6		

rescanROM:
	bis	$6, $6, $17		
	bis	$31, $31, $7		
	bic	$10, 1, $10		

	lda $0, ( 0x100000                &0xffff)($31)		;  ldah $0, (( 0x100000                +0x8000)>>16)($0)	; 		
	cmpult	$17, $0, $0		
	blbs	$0, ValidationPattern







DoneWithROM:
	






	and	$10, 4, $0		
	beq	$0, NoHeader		

	




	and	$10, 8, $0
	lda	$10, ((1 << 8) | 8) ($31)
	bne	$0, scanROM		
	bis	$10, 4, $10		
	br	$31, scanROM		

ValidationPattern:
	bsr	$28, read_flash		
					
	bis	$17, $17, $6		
	lda $1, ( 0x5a5ac3c3&0xffff)($31)		;  ldah $1, (( 0x5a5ac3c3+0x8000)>>16)($1)	; 	
	cmpeq	$0, $1, $1		
	bne	$1, InversePattern	
	lda $1, ( 0xc3e0&0xffff)($31)		;  ldah $1, (( 0xc3e0+0x8000)>>16)($1)	; 		
	extwl	$0, 2, $0			
	cmpeq	$0, $1, $0		
	beq	$0, rescanROM		

InversePattern:
	bsr	$28, read_flash		
					
	lda $1, ( 0xa5a53c3c&0xffff)($31)		;  ldah $1, (( 0xa5a53c3c+0x8000)>>16)($1)	; 	
	zap	$1, 0xF0, $1		
	cmpeq	$0, $1, $0		
	beq	$0, rescanROM		

HeaderSize:
	bsr	$28, read_flash		
					
	cmplt	$0, 0x34, $1		
	bis	$10, $1, $10		
	blbs	$10, HeaderIsValid	

	subq	$0, 0x10, $5		
	srl	$5, 2, $5		

HeaderCheckSum:
	bsr	$28, read_flash		
	subq	$5, 1, $5		
	bne	$5, HeaderCheckSum	

	bis	$7, $7, $5		
	bsr	$28, read_flash		
	addq	$6, (0xC-0x4), $17	

	srl	$10, 2, $1		
	blbc	$1, HeaderIsValid	
	cmpeq	$0, $5, $0		
	beq	$0, rescanROM		

HeaderIsValid:
	addq	$12, 1, $12		

	bsr	$28, read_flash		
	bis	$0, $0, $11		
	bsr	$28, read_flash		
	bis	$0, $0, $9		
	bsr	$28, read_flash		
	bis	$0, $0, $5		
	bsr	$28, read_flash		
	bis	$0, $0, $8		
	bsr	$28, read_flash		
	sll	$0, 32, $0		
	bis	$8, $0, $8		

	blbs	$10, SkipFirmwareID	
FirmwareID:				
	addq	$6, (0x20-0x4), $17	
	bsr	$28, read_flash		
	extbl	$0, 1, $1		

SROMImageCheck:
	subq	$12, 1, $12		
	cmpeq	$1, 10, $0		
	bne	$0, rescanROM		
	addq	$12, 1, $12		

SkipFirmwareID:
	srl	$10, 1, $2
	cmovlbc $2, $12, $1		
	blbc	$2, PositionDependent	

					
	blbs	$10, rescanROM		

PositionDependent:
	extbl	$10, 1, $0		
	cmpeq	$1, $0, $0		
	beq	$0, rescanROM		

	addq	$6, (0x24-0x4), $17	
	bsr	$28, read_flash		
	cmovlbc $10, $0, $9		

	addq	$6, (0x8-0x4), $17	
	bsr	$28, read_flash
	subq	$6, 0x4, $17		
	addq	$0, $17, $17		
	addq	$17, $9, $9		
	br	$31, headerEnd		

NoHeader:
	lda	$19, 0x4     ($31)
	bsr	$28, Beep		

 	bis	$31, $31, $8		
	bis	$31, $31, $5		
	bis	$31, $31, $17		
	lda $9, ( 0x100000                &0xffff)($31)		;  ldah $9, (( 0x100000                +0x8000)>>16)($9)	; 		
	bic	$10, 4, $10		

headerEnd:
	bis	$8, $8, $25		
	bis	$31, $31, $7		
	blbs	$5, LoadComp		















		bis	$31, 0x15, $2	; 		bsr	$29, WriteLEDS	; 			
LoadUncomp:
	bsr	$28, read_flash		
					
	hw_stl/p	$0, 0($8)		
	addq	$8, 4, $8		
	cmpult	$17, $9, $0		
	bne	$0, LoadUncomp		
					
					
	br	$31, ImageCheckSums	

LoadComp:
		bis	$31, 0x16, $2	; 		bsr	$29, WriteLEDS	; 			
	bis	$31, 4, $18		
 	bsr	$26, getRecord		

LoadCInit:
	bis	$31, $31, $27		
	bis	$31, $31, $5		

LoadCLoop:
	bsr	$26, getByte
	insbl	$16, $5, $0		
	bis	$0, $27, $27		
	addq	$5, 1, $5		
	cmpeq	$5, 4, $0		
	beq	$0, LoadCLoop		

	hw_stl/p	$27, 0($8)		
	addq	$8, 4, $8		
	bne	$19, LoadCInit		

DecompressionDone:
	lda	$5, 1($31)		

ImageCheckSums:
	blbs	$10, codeLoaded		
	srl	$10, 2, $0		
	blbc	$0, codeLoaded		
	cmpeq	$7, $11, $1		
	beq	$1, SumMismatch		
					
					
	blbs	$5, codeLoaded		

	ldah	$0, 0x100($31)		
flush:
	hw_ldq/p	$31, 0x0($0)		
	subq	$0, 0x20, $0		
	bge	$0, flush		

	bis	$31, $31, $7		
	bis	$25, $25, $17		
secondcheck:
	hw_ldl/p	$0, 0x0($17)		
	bsr	$28, ComputeCheckSum	
	addq	$17, 4, $17		
	cmpult	$17, $8, $1		
	bne	$1, secondcheck
	cmpeq	$7, $11, $1		
	bne	$1, codeLoaded		

	lda	$8, 0x06	($31)
	br	$31, FatalError		

SumMismatch:
	subq	$12, 1, $12		
	br	$31, rescanROM		
codeLoaded:
	ret	$31, ($14)





























read_flash:
	ldah	$0, (0x100000                >>16)($31)	
	cmpult	$17, $0, $0		
	cmoveq	$0, $17, $6
	beq	$0, rescanROM		

	    lda	    $0, 0x87C($31);	    sll	    $0, 28              , $0;		
	lda		$3, 0xFC00($31)	
	sll		$3, 32, $3
	addq	$3, $0, $0
	addq	$0, $17, $4		

	ldbu	$2, 0($4)		
	ldbu	$3, 1($4)		
	insbl	$3, 1, $3
	bis		$3, $2, $2
	ldbu	$3, 2($4)		
	insbl	$3, 2, $3
	bis		$3, $2, $2
	ldbu	$3, 3($4)		
	insbl	$3, 3, $3
	bis		$3, $2, $2
	zap		$2, 0xF0, $0		
	addq	$17, 4, $17		
					

























ComputeCheckSum:
	lda	$2, 0($31)		
CheckSumLoop:
	srl	$7, 1, $3		
	lda $4, ( 0x8000&0xffff)($31)		;  ldah $4, (( 0x8000+0x8000)>>16)($4)	; 		
	addq	$3, $4, $4		
	cmovlbs $7, $4, $3
	extbl	$0, $2, $4		
	addq	$3, $4, $7		
	zapnot	$7, 0x03, $7		
	addq	$2, 1, $2		
	cmpeq	$2, 4, $4		
	beq	$4, CheckSumLoop
	ret	$31, ($28)






































getByte:
	blt	$19, repByte		
	bsr	$28, ReadFByte		
	bis	$2, $2, $16		
	lda	$19, -1($19)		
	beq	$19, getRecord		
	ret	$31, ($26)		

repByte:
	bis	$2, $2, $16
	lda	$19, 1($19)		
	beq	$19, getRecord		
	ret	$31, ($26)		

getRecord:
	bsr	$28, ReadFByte		
	sll	$2, 56, $19		
	sra	$19, 56, $19		
	bge	$19, norepeat		
	bsr	$28, ReadFByte		

norepeat:	
	ret	$31, ($26)		































ReadFByte:
	cmpeq	$18, 4, $0		
	beq	$0, skipRd		

	cmpult	$17, $9, $0		
	beq	$0, DecompressionDone	

	bis	$31, $31, $18		
					
	bis	$28, $28, $15		 
	bsr	$28, read_flash		
	bis	$15, $15, $28		
					
	bis	$0, $0, $15		
skipRd:	
	extbl	$15, $18, $2		
	lda	$18, 1($18)		
	ret	$31, ($28)

.align 3
minidebugger:				



.end
