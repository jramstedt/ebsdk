/*****************************************************************************

       Copyright � 1993, 1994 Digital Equipment Corporation,
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

.data
.asciz "$Id: micropal.s,v 1.4 1999/03/21 22:48:29 paradis Exp $"
.text

/*
**  MODULE DESCRIPTION:
**
**      Huffman decompression start up file.
**
**      This program runs without the benefit of PALcode, and so it must
**      be careful when accessing memory.  There are no DTB misses as
**      this code establishes an environment that is 8 MB.  Two TBs are
**      set up that map addresses 0 through 8 MB contiguously with 512
**      page entries.  This code runs in PALmode, but data stream misses
**      are normal.
**
**      If necessary, this program will relocate itself so that as we
**      decompress we don't step on ourselves.
**
**      After relocating, we then do the actual decompression.  The
**      compressed image contains a header describing where in memory
**      the decompressed image really wants to live.  This code
**      relocates itself up to 300000 which gives some amount of stack
**      space and plenty of room for expansion.  It actually relocates
**      itself up or down in memory to where the label, Start, is
**      pointing.  Care must be taken not to over write ones self.
**      This code assumes a stack size of 64K, a base heap of 32K, and
**      a work area of 32K.
**
**
** $Log: micropal.s,v $
** Revision 1.4  1999/03/21 22:48:29  paradis
** Fix build of micropal.s for dp264
** Re-build NT utilities
**
** Revision 1.3  1999/01/21 19:51:03  bissen
** Fixed problem with code relocation.
**
** Revision 1.2  1999/01/20 20:55:44  bissen
** Removed #define DEBUG - should only be added if LED output is requied.
**
** Revision 1.1.1.1  1998/12/29 21:36:25  paradis
** Initial CVS checkin
**
** Revision 1.21  1998/04/02  14:46:46  gries
** srm hack
**
** Revision 1.20  1998/03/26  15:35:09  gries
** Donot wait on nt restart
**
** Revision 1.19  1997/12/18  21:38:47  gries
** Fixed everything that I broke while transfering
**
** Revision 1.18  1997/12/15  19:02:09  pbell
** Changed LEDWRITE C0 to CC for clarity.
** /
**
** Revision 1.17  1997/12/15  17:49:23  gries
** changed to Kseg for ldl/l stl/c
**
** Revision 1.16  1997/12/05  21:59:30  pbell
** -
**
** Revision 1.15  1997/11/19  21:48:34  pbell
** Added DP264 support and cleaned up PVC complaints.
**
** Revision 1.15  1997/10/02  18:04:43  fdh
** Added EV6 support.
**
** Revision 1.14  1997/07/31  20:28:48  pbell
** Enhanced to operate when loaded on any long word boundary.
**
** Revision 1.13  1995/12/14  00:17:49  fdh
** Added LEDWRREG macro that is used to print register
** contents to the LED card.
**
** Revision 1.12  1995/12/08  13:34:56  fdh
** Delay setting PALbase until setting up to call decompression code.
** Modified the LEDWRITE macro and removed some associated pvc labels.
**
** Revision 1.11  1995/12/07  07:41:10  fdh
** Modified to accept return value from decompression code
** as the base of the decompressed image.
**
** Revision 1.10  1995/12/07  07:13:10  fdh
** Modified to use HFCOMP header structure... includes header.h.
** The decompressed image destination now comes from this header
** instead of the previous hardcoded value.
**
** Removed the restore from the PALbase register to reestablish
** the pointer to the Data area for restoring register state
** because I can figure it out with a branch instead.
**
** Moved some general MACRO definitions to header.h.
**
** Added some additional debug tracing when compiled with DEBUG defined.
**
** Revision 1.9  1995/12/02  21:19:16  fdh
** Use VMS PALcode cserve (cserve$jump_to_arc) to restart
** in PALmode when booted by the SRM console.
**
** Revision 1.8  1995/11/28  23:16:36  fdh
** If we've been SRM console "booted", and therefore running
** virtual, and not in PALmode, we drop back into PAL mode.
** Otherwise, we've been started in PALmode, just as if we
** were started from the serial ROM.
**
** Revision 1.7  1995/11/16  15:26:34  fdh
** Added the capability for the image to relocate itself
** if necessary before calling the decompression code.
** If the starting location and the desired destination
** causes the image to overlap then the image will be moved
** to an intermediate location first so that it is not
** corrupted in the process.
**
** Fixed up copyright header and log information formats.
** Added defaults for compile-time variables.
**
** Prints out additional information whe built with the
** DEBUG variable defined.
**
** Revision 1.6  1995/05/19  20:24:11  fdh
** Consolidated code duplication.
** Made uncompressed image entry point a compile
** time parameter throught the DECOMP_DESTINATION macro.
**
** Revision 1.5  1995/04/18  20:15:43  fdh
** Separated DC21064 and DC21164 conditionals around
** the #include "dc21x64.h" lines to allow makedepend
** to work properly.
**
** Revision 1.4  1995/03/20  20:48:39  cruz
** Added code for EV5 support.
**
** Revision 1.3  1995/03/05  04:13:31  fdh
** Modified to save state of all gpr's.  Only pt0 state is lost.
**
** Revision 1.2  1995/02/01  19:21:45  curewitz
** added pvc labels for pvc checking
**
** Revision 1.1  1995/02/01  16:40:56  curewitz
** Initial revision
*/

#include "header.h"

/*
** Define DEBUG if LED output is required
** #define DEBUG
*/


#ifndef DC21264
#define DualOff
#endif

#ifdef DEBUG

#ifdef DC21264
#define DEBUG_TSUNAMI
#endif /* DC21264 */

#ifdef DC21164
#define DEBUG_ALCOR
#endif /* DC21164 */

#ifdef DC21064
#define DEBUG_APECS
#endif /* DC21064 */

#endif /* DEBUG */

#include "cpu.h"

#if ! (defined(DEBUG_TSUNAMI) || defined(DEBUG_ALCOR) || defined(DEBUG_APECS))
#define LEDWRITE(value, reg1, reg2)
#define LEDWRREG(inreg, reg1, reg2, reg3, reg4)
#else

#ifdef DEBUG_ALCOR
#define LEDPORT         0x80
#define IO_SHIFT        5
#define PCI_SPARSE      0x0858
#define BASE_SHIFT      28
#define WaitDelay       0x800
#endif /* DEBUG_ALCOR */

#ifdef DEBUG_APECS
#define LEDPORT         0x80
#define IO_SHIFT        5
#define PCI_SPARSE      0x1C
#define BASE_SHIFT      28
#define WaitDelay       0x400
#endif /* DEBUG_APECS */

#if defined (DEBUG_ALCOR) || defined (DEBUG_APECS)
#define LEDWRITE(value, reg1, reg2)                     \
    lda         reg1, PCI_SPARSE(r31);                  \
    sll         reg1, (BASE_SHIFT-IO_SHIFT), reg1;      \
    lda         reg1, LEDPORT(reg1);                    \
    sll         reg1, IO_SHIFT, reg1;                   \
    bis         r31, value, reg2;                       \
    stl_p       reg2, 0(reg1);                          \
    mb;                                                 \
    ldah        reg1, WaitDelay(r31);                   \
0:  subq        reg1, 1, reg1;                          \
    bne         reg1, 0b;

#define LEDFLASH(value, reg1, reg2)                     \
    lda         reg1, PCI_SPARSE(r31);                  \
    sll         reg1, (BASE_SHIFT-IO_SHIFT), reg1;      \
    lda         reg1, LEDPORT(reg1);                    \
    sll         reg1, IO_SHIFT, reg1;                   \
    bis         r31, value, reg2;                       \
    stl_p       reg2, 0(reg1);                          \
    mb;                                                 \
    ldah        reg1, WaitDelay(r31);                   \
    srl         reg1, 2, reg1;                          \
0:  subq        reg1, 1, reg1;                          \
    bne         reg1, 0b;

#define LEDWRREG(inreg, reg1, reg2, reg3, reg4) \
	LEDWRITE(0xEE, reg1, reg2)      /* Signal that 8 bytes coming   */\
	bis     r31, 8, reg3;           /* Load up byte counter         */\
	bis     inreg, r31, reg4;       /* Move Address into reg4       */\
1:      LEDWRITE(reg4,reg1,reg2)        /* Write it out to the port     */\
	LEDFLASH(0x88,reg1,reg2)        /* Flash a delimiter            */\
	subq    reg3, 1, reg3;          /* Decrement counter            */\
	srl     reg4, 8, reg4;          /* Shift next byte down         */\
	bne     reg3, 1b;               /* Are we done yet?             */\
	LEDWRITE(0xEE, reg1, reg2)      /* Output Done Signal           */
#endif

#ifdef DEBUG_TSUNAMI
#define PCI0_IO 0x801FC 


#define LDLI(reg,val,base_reg) \
	lda reg, (val&0xffff)(base_reg)         ; /* Mask off upper word        */ \
	ldah reg, ((val+0x8000)>>16)(reg)       ; /* Mask off lower word and    */

/*
 * Macro to write a byte literal to a specified port using 48 bit Kseg.
 */
#define OutPortByte(port, val, tmp0, tmp1)      \
	LDLI(tmp0, PCI0_IO, r31);               \
	sll     tmp0, 24, tmp0  ;               \
	ldah    tmp1, 0x8000(r31);              \
	sll     tmp1, 16, tmp1  ;               \
	bis     tmp0, tmp1, tmp0;               \
	bis     tmp0, port, tmp0;               \
	bis     r31, val, tmp1  ;               \
	stb     tmp1, 0(tmp0)   ;               \
	mb                      ;               \
	mb                      ; 

/*
 * Macro to write values to the LED diagnostic card.
 */
#define LEDDPORT        0x80
#define WaitDelay       0x100
/* #define WaitDelay    0x1 */


#define LEDWRITE(value, reg1, reg2)                     \
	OutPortByte (LEDDPORT,value,reg1,reg2);         \
	ldah    reg1, WaitDelay(r31);                   \
1:      subq    reg1, 1, reg1   ;                       \
	bne     reg1, 1b        ;

#define LEDFLASH(value, reg1, reg2)                     \
	OutPortByte (LEDDPORT,value,reg1,reg2);         \
	ldah    reg1, WaitDelay(r31);                   \
	srl     reg1, 2, reg1 ;                         \
1:      subq    reg1, 1, reg1   ;                       \
	bne     reg1, 1b        ;

/*
 * Macro to write values to the LED diagnostic card
 * with delimiters and delays.
 */
#define LEDWRREG(inreg, reg1, reg2, reg3, reg4) \
	LEDWRITE(0xEE, reg1, reg2)      ; /* Signal that 8 bytes coming */ \
	bis     r31, 8, reg3            ; /* Load up byte counter */ \
	bis     inreg, r31, reg4        ; /* Move Address into reg4 */ \
2:      LEDWRITE(reg4,reg1,reg2)        ; /* Write it out to the port */ \
	LEDFLASH(0x88,reg1,reg2)        ; /* Write out a delimiter */ \
	subq    reg3, 1, reg3           ; /* Decrement counter */ \
	srl     reg4, 8, reg4           ; /* Shift next byte down */ \
	bne     reg3, 2b                ; /* Are we done yet? */ \
	LEDWRITE(0xEE, reg1, reg2)      ; /* Output Done Signal */
#endif /* DEBUG_TSUNAMI */
#endif /* ! (defined(DEBUG_TSUNAMI) || defined(DEBUG_ALCOR) || defined(DEBUG_APECS)) */

/* this application may be loaded into memory at an address that is ONLY
 * longword alligned.  Therefore all calculations reguarding the location
 * for the Data area relative to Start are not known
 */
	.text   0
Start:
	bis     r31, r31, r31           // NOP
	bis     r31, r31, r31           // NOP
/*
**      Since we can't be guaranteed to be running in PALmode at this
**      point general purpose register state can not be saved to
**      PALtemp registers with the mtpr instructions. We do not yet
**      know if there is PALcode present that can provide cserve
**      functions to do what is needed.  Therefore, by the time that
**      we are guaranteed to be in PALmode the original state of
**      registers R27, R28, R30 and R29 could have been lost.
*/
	br      r27, CheckMode          // Branch over static data.
BrData:
/* the actual address for the following variables must be calculated based on
 * BrData plus an alignment adjustment.  The code may be located on any long
 * word boundary and therefore the assembler can not align it.
 */
	.quad   0                       // 0
	.quad   0
	.quad   0
	.quad   0
	.quad   0
	.quad   0
	.quad   0
	.quad   0
	.quad   0
	.quad   0
	.quad   0                       // 10
	.quad   0
	.quad   0
	.quad   0
	.quad   0
	.quad   0
	.quad   0
	.quad   0
	.quad   0
	.quad   0
	.quad   0                       // 20
	.quad   0
	.quad   0
	.quad   0
	.quad   0
	.quad   0
	.quad   0
	.quad   0
	.quad   0
	.quad   0
	.quad   0                       // 30
	.quad   0                       // 31
	.quad   0   /* this is dead space to insure room for alignment of this area */
	.quad   0   /* this is dead space to insure room for alignment of this area */
	.quad   0   /* this is dead space to insure room for alignment of this area */

CheckMode:
	lda     r27, (Start-BrData)(r27)// r27 now points to the Start

/*
**      If we've been SRM console "booted", and therefore running
**      virtual, and not in PALmode, we drop back into PAL mode.
**      Otherwise, we've been started in PALmode, just as if we
**      were started from the serial ROM.
**
**      If we're at any of the following addresses, we assume we're
**      virtual.  All other addreses we assume we're physical:
**
**      00000000        virtual, loaded by APB
**      20000000        virtual, standard SRM boot
**      all else        assume physical (PALmode)
*/
	beq     r27, RunningVirtual     // check for 00000000
	ldah    r28, 0x2000(r31)        // check for 20000000
	subq    r27, r28, r28
	bne     r28, PALContinue

// Use VMS call_pal to read virtual page table base address
#define PAL_FUNC_MFPR_VPTBASE   (41)
#define mfpr_vptb       call_pal        PAL_FUNC_MFPR_VPTBASE

RunningVirtual:
	bis     r0, r31, r29            // Save r0 into scratch r29
pvc$huf16$1002:
	mfpr_vptb                       // R0 = virtual page table base address.

	sll     r27, (64-43), r28       // get rid of sign extension
	srl     r28, ((64-43)+10), r28  // make bits 32:0 of va of pte
	bic     r28, 7, r28             // quad word align
	bis     r0, r28, r0             // make the real address

	ldq     r30, 00(r0)             // pick up the pte.

	srl     r30, 32, r30            // Convert PTE to PFN
	sll     r30, 13, r30            // and then to Physical Address

	bis     r16, r31, r28           // Save r16 into scratch r28
	bis     r17, r31, r27           // Save r17 into scratch r27

#define PAL_FUNC_CSERVE         (9)
#define cserve_jump_to_arc      (69)

	bis     r30, 1, r17
	lda     r16, cserve_jump_to_arc(r31)
pvc$huf15$1002:
	call_pal PAL_FUNC_CSERVE        // Load the new EXC_ADDR, return in PAL
	bis     r28, r31, r16           // restore r16 from scratch r28
	bis     r27, r31, r17           // restore r17 from scratch r27

	br      r31, PALContinue

#ifdef DC21164
/*
**      DC21164 PALcode entry point for DTB miss flow
*/
	. = 0x200

PalDtbMiss:

	mtpr    r0, pt6                 // Save r0
	mtpr    r1, pt7                 // Save r1

	mfpr    r0, va                  // Get VA of target reference
	srl     r0, 13, r1              // Get rid of byte-within-page bits.
	sll     r1, 32, r1              // Fabricate PFN
	lda     r1, 0x1101(r1)          // Fabricate protection bits

	mtpr    r1, dtbPte              // Write PTE into TB and set valid
	STALL
	mtpr    r0, dtbTag              // Set up the tag (writes PTE and tag simultaneously)

	mfpr    r0, pt6                 // Restore scratch registers
	mfpr    r1, pt7                 //
	STALL                           // Stall due to tbTag.
	hw_rei                          // Return and try access again

#endif /* DC21164 */

#ifdef DC21264
/*
**      DC21264 PALcode entry point for DTB miss flow
*/
	. = 0x300

PalDtbMiss:
	mfpr    r23, EV6__EXC_ADDR              /* (0L,1) get exception address */

trap__dtbm_single_vpte:
	mfpr    r6, EV6__VA                     /* (4-7,1L,3) get original va   */

trap__d1to1:
	lda     r5, 0x3301(r31)                 /* all r/w enable               */
	srl     r6, 13, r4                      /* shift out the byte offset    */
	sll     r4, EV6__DTB_PTE0__PFN__S, r4   /* get pfn into position        */
	bis     r4, r5, r4                      /* produce the pte              */

	mtpr    r6, EV6__DTB_TAG0               /* (2&6,0L) write tag0          */
	mtpr    r6, EV6__DTB_TAG1               /* (1&5,1L) write tag1          */
	mtpr    r4, EV6__DTB_PTE0               /* (0&4,0L) write pte0          */
	mtpr    r4, EV6__DTB_PTE1               /* (3&7,1L) write pte1          */
pvc$huf20$1007:
	hw_jmp/stall    (r23)                   /* (0L) return                  */
#endif /* DC21264 */

PALContinue:
/*
**      Save the General Purpose Registers in memory
*/
	lda     r27, (BrData-Start)(r27)// adjust r27 back to BrData
	addq    r27, 4, r27             // Skip any alignment padding
	bic     r27, 7, r27             // r27 now points to the data area
#ifndef DualOff
	mfpr    r29,EV6__I_CTL          /* get old value */
	ldah    r28,0x002c(r31)         /* .....TB_MB_EN=1, CALL_PAL_R23=1, SL_XM*/
	bic     r29, r28, r29           // clear PCT0_EN, PCT1_EN & MCHK_EN
	lda     r28,0x2006(r31)         /* load I_CTL.....*/
	ldah    r28,0x0050(r28)         /* .....TB_MB_EN=1, CALL_PAL_R23=1, SL_XM*/
	bis     r28, r29, r28           // or old bit with new bits
	mtpr    r28,EV6__I_CTL          /* value = 0x00000000005020x6 (SCRBRD=4)*/
	bis     r31, 8, r28             // 47 bit Kseg
	mtpr    r28, EV6__M_CTL         /* Turn on 47 bit Kseg */

	ldah    r28, -32768(r31)        /* Kseg = 0xffff8000.00000000   */
	sll     r28, 16, r28            /* Shift into place             */
	bis     r28, r27, r27

GetCpuNumber:
	lda     r28, 100(r31)           // lda 100 to r28
GetCpuNumberwait:
	subq    r28, 1, r28
	bne     r28, GetCpuNumberwait
	ecb     (r27)
	mb
	ldl_l r28, 0(r27)           // See if we're the first CPU here
	addl     r28, 1, r29            // increament processor id
	stl_c r29, 0(r27)           //  and prevent others from doing i`
	beq     r29, GetCpuNumber

	mb
	addq    r27, 8, r27             // Skip lock
	bne     r28, Not_primary

	sll     r27,  16, r27
	srl     r27,  16, r27           // clear Kseq address
	lda     r29, (DECOMP_PALBASE&0xffff)(r31)
	ldah    r29, ((DECOMP_PALBASE+0x8000)>>16)(r29) // r29 = image destination
	subq    r29, 248, r29
	stq_p   r31, 8(r29)             // zero new address


	stq_p   r0,  0x00(r27)          // save gpr's
	stq_p   r1,  0x08(r27)
	stq_p   r2,  0x10(r27)
	stq_p   r3,  0x18(r27)

	beq     r20, wait_timed_out
	srl     r20, 1, r30
	and     r20, 1, r28
	and     r30, 1, r30
	addq    r30, r28, r30


	lda     r0, 0x100(r31)          // SYNCHRONIZE_CPU0_TRANSFER_ADDRESS
	stq_p   r25, 0(r0)
	stq_p   r25, 8(r0)
	lda     r0, (0x8013)(r31)       // r0 SYNCHRONIZE_CPU
	sll     r0, 28, r0              // 0x80130000000
	lda     r0, 0x0a00(r0)          // 0x80130000A00
	stq_p   r31, 0(r0)
	stq_p   r31, 0x40(r0)

	ldah    r0, 1(r31)
wait_for_checkin:
	subq    r0, 1, r0
	beq     r0, wait_timed_out
	lda     r1, 1000(r31)           // lda 100 to r1
checkinwait_r1:
	subq    r1, 1, r1
	bne     r1, checkinwait_r1
	mb
	ldl_p r28, -8(r27)           // See if we're the first CPU here
	subl    r28, r30, r28
	bne     r28, wait_for_checkin

wait_timed_out:
#else
	addq    r27, 8, r27             // Skip lock
	stq_p   r0,  0x00(r27)          // save gpr's
	stq_p   r1,  0x08(r27)
	stq_p   r2,  0x10(r27)
	stq_p   r3,  0x18(r27)
	LEDWRITE(0xc0,r0,r1)

#endif
	stq_p   r4,  0x20(r27)
	stq_p   r5,  0x28(r27)
	stq_p   r6,  0x30(r27)
	stq_p   r7,  0x38(r27)
	stq_p   r8,  0x40(r27)
	stq_p   r9,  0x48(r27)
	stq_p   r10, 0x50(r27)
	stq_p   r11, 0x58(r27)
	stq_p   r12, 0x60(r27)
	stq_p   r13, 0x68(r27)
	stq_p   r14, 0x70(r27)
	stq_p   r15, 0x78(r27)
	stq_p   r16, 0x80(r27)
	stq_p   r17, 0x88(r27)
	stq_p   r18, 0x90(r27)
	stq_p   r19, 0x98(r27)
	stq_p   r20, 0xa0(r27)
	stq_p   r21, 0xa8(r27)
	stq_p   r22, 0xb0(r27)
	stq_p   r23, 0xb8(r27)
	stq_p   r24, 0xc0(r27)
	stq_p   r25, 0xc8(r27)
	stq_p   r26, 0xd0(r27)
	stq_p   r27, 0xd8(r27)          // Probably trashed
	stq_p   r28, 0xe0(r27)          // Probably trashed
	stq_p   r29, 0xe8(r27)          // Probably trashed
	stq_p   r30, 0xf0(r27)

	br      r6, MovePgm
MovePgm:
	lda     r6, (Start-MovePgm)(r6) // r6 = current image location
	lda     r29, (DECOMP_PALBASE&0xffff)(r31)
	ldah    r29, ((DECOMP_PALBASE+0x8000)>>16)(r29) // r29 = image destination
	subq    r29, 248, r30
	stq_p   r31, 8(r30)
	mb
	stq_p   r30, 0xf0(r27)

//      LEDWRREG(r6,r16,r17,r19,r24)    // Print out current location to LED Port

	cmpeq   r6, r29, r7             // If r29 != r6 then move.
	blbs    r7, SkipMove            // else don't move

//      LEDWRITE(0xc1,r16,r17)
	bis     r29, r31, r19           // r19 Gets updated during copy
	ldah    r15, 0x40(r31)          // Image size counter. Assume 4MB for now
	subq    r29, r6, r5             // r5 = Destination-Source
	subq    r31, r5, r4             // Load neg(Destination-Source)
	cmovlt  r5, r4, r5              // r5 = abs(Destination-Source)
	cmple   r15, r5, r3             // ImageSize <= abs(Destination-Source)
	blbs    r3, NoOverlap           // Will source & destination overlap
	cmplt   r6, r29, r3             // r3 = (Source < Destination)
	subq    r6, r15, r4             // Move down from source?
	subq    r29, r15, r19           // Move down from destination?
	cmovlbs r3, r4, r19             // Use the lower address
	bge     r19, NoOverlap          // Throw away destination if < 0
	addq    r6, r15, r4             // Move up from source?
	addq    r29, r15, r19           // Move up from destination?
	cmovlbc r3, r4, r19             // Use the higher address

NoOverlap:
	LEDWRITE(0xc2,r16,r17)
	bis     r19, r31, r29           // r29 = intermediate image destination
Move:   ldl_p   r18, 00(r6)             // Copy from address in r6
	subq    r15, 4, r15             // Decrement image size counter
	stl_p   r18, 00(r19)            // Copy to address in r19
	addq    r6,  4, r6              // increment source pointer
	addq    r19, 4, r19             // increment destination pointer
	bge         r15, Move           // Are we done yet?
    br      r31, RestoreState           // Go decompress at new destination

#ifndef DualOff
	bis     r31, r31, r31
	bis     r31, r31, r31
	bis     r31, r31, r31
	bis     r31, r31, r31
Not_primary:
	bis     r31, r31, r31
	bis     r31, r31, r31
	bis     r31, r31, r31
	bis     r31, r31, r31
Not_primary_1:
	LEDWRITE(0xc8,r28,r30)
	ecb     (r27)
	ldq     r28, 0xf0(r27)
	mb
	beq     r28, Not_primary_1
 
ParkCpus:
	lda     r29, (DECOMP_PALBASE&0xffff)(r31)
	ldah    r29, ((DECOMP_PALBASE+0x8000)>>16)(r29) // r29 = image destination
	subq    r29, 248, r30
	br      r31, Fillicache
ParkLoop:
	mb
	ldq_p   r29, 8(r30)             // Get new address      
	beq     r29, ParkLoop           //
#ifdef DC21264
	mtpr    r31, EV6__IC_FLUSH      // (4,0L) flush the icache
	bne     r31, Parkret_flush              // pvc #24
Parkret_flush:
pvc$huf24$1007:
	hw_jmp/stall    (r29)           // return with stall
#endif /* DC21264 */
Fillicache:
	br      r31, ParkLoop
#endif
	

	
SkipMove:
/*
**      Setup to call decompression code.
**      r30 is stack pointer.
**      r26 is Return Address.
*/
	mb
	mb
#ifdef DC21264
	mtpr    r29, EV6__PAL_BASE      /* set up pal_base register */
#else
	mtpr    r29, palBase            // set up palbase for dtb miss code
#endif /* DC21264 */
	
	STALL
	STALL
	STALL
	STALL
	STALL
	STALL
	STALL

	LEDWRITE(0xc3,r16,r17)
	LEDWRREG(r29,r16,r17,r19,r24)   // Print out destination to LED Port
	
					// Init stack pointer to be the PAL_BASE - 248
	br      r20, CallInfo           // Branch over static data
	.align 3                        // Insure quadword alignment
	.quad   DECOMP_BASE             // address of decompressor
CallInfo:
	addq    r20, 4, r20             // Skip any alignment padding
	bic     r20, 7, r20
	ldq_p   r21, 0(r20)             // r21 = Pointer to decompression code

	LEDWRITE(0xc4,r16,r17)
	LEDWRREG(r21,r16,r17,r19,r24)   // Print out destination to LED Port

pvc$huf1$2001:
pvc$huf22$29:
	jsr     r26, (r21)              // jump to (r21) decompression code
pvc$huf4$2002.1:
	bis     r0, 0x1, r0             // Enable palmode
	stq_p   r0, 8(r30)
	bis     r0, r31, r29            // Load Decompressed image location

	LEDWRITE(0xc5,r16,r17)

RestoreState:
	bis     r29, 0x1, r29           // Enable palmode
	LEDWRREG(r29,r16,r17,r19,r24)   // Print out destination to LED Port
	
#ifndef DC21264
	mtpr    r29, excAddr
	STALL
	STALL
	STALL
	STALL
#endif /* DC21264 */
	
	br      r27, Reload
Reload: lda     r27, (BrData-Reload)(r27)//adjust r27 back to BrData
	addq    r27, 4, r27             // Skip any alignment padding
	bic     r27, 7, r27             // r27 now points to the data area
	LEDWRITE(0xc6,r16,r17)
	addq    r27, 8, r27             // Skip lock

	ldq_p   r0, 0x00(r27)           // begin restore registers
	ldq_p   r1, 0x08(r27)
	ldq_p   r2, 0x10(r27)
	ldq_p   r3, 0x18(r27)
	ldq_p   r4, 0x20(r27)
	ldq_p   r5, 0x28(r27)
	ldq_p   r6, 0x30(r27)
	ldq_p   r7, 0x38(r27)
	ldq_p   r8, 0x40(r27)
	ldq_p   r9, 0x48(r27)
	ldq_p   r10,0x50(r27)
	ldq_p   r11,0x58(r27)
	ldq_p   r12,0x60(r27)
	ldq_p   r13,0x68(r27)
	ldq_p   r14,0x70(r27)
	ldq_p   r15,0x78(r27)
	ldq_p   r16,0x80(r27)
	ldq_p   r17,0x88(r27)
	ldq_p   r18,0x90(r27)
	ldq_p   r19,0x98(r27)
	ldq_p   r20,0xa0(r27)
	ldq_p   r21,0xa8(r27)
	ldq_p   r22,0xb0(r27)
	ldq_p   r23,0xb8(r27)
	ldq_p   r24,0xc0(r27)
	ldq_p   r25,0xc8(r27)
	ldq_p   r26,0xd0(r27)
	// Don't restore r27.  It's been trashed anyway
	ldq_p   r28,0xe0(r27)           // Probably trashed
#ifndef DC21264
	ldq_p   r29,0xe8(r27)           // Probably trashed
#endif /* DC21264 */
	ldq_p   r30,0xf0(r27)           // end restore registers


FlushIcache:
	mb
	mb
	STALL
	STALL

#ifdef DC21064
	mtpr    r31, flushIc            // Flush the I-cache
	mtpr    r31, xtbZap             // Flush the TBs

	mb
	STALL
	STALL
	STALL
	STALL
	STALL
	STALL
	STALL
	STALL
	STALL
	STALL
	hw_rei
#endif /* DC21064 */

#ifdef DC21164
	mtpr    r31, icFlush            // Flush the I-cache
	mtpr    r31, itbIa              // Flush the TBs
	mtpr    r31, dtbIa              // Flush the TBs
	mtpr    r31, ips                // set current mode to kernel
	mtpr    r31, dtbCm              // set current mode to kernel
	mtpr    r31, dtbAsn             // set asn to 0
	mfpr    r31, va                 // unlock va

	mb
	STALL
	STALL
	STALL
	STALL
	STALL
	STALL
	STALL
	STALL
	STALL
	STALL
	hw_rei
#endif /* DC21164 */

#ifdef DC21264
	mtpr    r31, EV6__IC_FLUSH      // (4,0L) flush the icache
	bne     r31, ret_flush          // pvc #24
ret_flush:
pvc$huf21$1007:
	hw_jmp/stall    (r29)           // return with stall
#endif /* DC21264 */

#ifdef PVCCHECK
/*
**      Dummy routine to simulate decompression code for pvc checking.
*/
dummy1:
pvc$huf2$2001.1:
pvc$huf3$2002:
pvc$huf23$29:
	ret     r31, (r26)
#endif


#ifdef DC21064
/*
**      DC21064 PALcode entry point for DTB miss flow
*/
	. = 0x9E0

PalDtbMiss:

	mtpr    r0, pt6                 // Save r0
	mtpr    r1, pt7                 // Save r1
	mfpr    r0, va                  // Get VA of target reference
	mtpr    r0, tbTag               // Set up the tag
	sll     r0, (32-13), r0         // Fabricate PFN
	lda     r1, 0xFF7(r31)          // Fabricate protection bits
	sll     r1, 4, r1
	bis     r1, r0, r1              // Form PTE
	mfpr    r0, excAddr             // Save exception address
	mtpr    r1, dtbCtl              // Select large or small page size
	mtpr    r1, dtbPte              // Write PTE into TB and set valid
	mtpr    r0, excAddr             // Restore exception address
	mfpr    r0, pt6                 // Restore scratch registers
	mfpr    r1, pt7
	hw_rei                          // Return and try access again
#endif /* DC21064 */
