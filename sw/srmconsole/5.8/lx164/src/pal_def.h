/********************************************************************************************************************************/
/* Created: 21-JUN-2000 10:27:28 by OpenVMS SDL EV1-33     */
/* Source:  20-NOV-1996 12:07:29 AFW_USER$:[CONSOLE.V58.COMMON.SRC]EV5_LX164_PAL */
/********************************************************************************************************************************/
/* file:	ev5_lx164_pal_def.sdl                                       */
/*                                                                          */
/* Abstract:	Platform specific PALcode definitions and Cserve defintions  */
/*		for the AlphaPC 164/LX.                                     */
/*                                                                          */
/*** MODULE $pal_def ***/
#pragma member_alignment __save
#pragma nomember_alignment
#define PAL$PAL_BASE 49152
#define PAL$PAL_SIZE 49152
#define PAL$OSFPAL_BASE 98304
#define PAL$OSFPAL_SIZE 32768
#define PAL$IPL_TABLE_BASE 0
#define PAL$HWRPB_BASE 8192
#define PAL$HWRPB_SIZE 16384
#define PAL$IMPURE_BASE 20480
#define PAL$IMPURE_COMMON_SIZE 512
#define PAL$IMPURE_SPECIFIC_SIZE 1536
#define PAL$LOGOUT_BASE 24576
#define PAL$CONSOLE_BASE 131072
#define PAL$ENTER_CONSOLE_PTR 20800
#define PAL$ENTER_OSF 20864
#define SLOT$_PAL_LOGOUT 24576
#define SLOT$_PAL_LOGOUT_LEN 1536
#define IMPURE$PCB_OFFSET 16
#define IMPURE$PROF_OFFSET 64
#define IMPURE$QUILT_OFFSET 64
#define IMPURE$UART_OFFSET 112
#define IMPURE$CPU_TYPE_OFFSET 128
#define MP$RESTART 1
#define MP$HALT 2
#define MP$CONTROL_P 3
#define MP$BREAK 4
#define MP$BLOCK_INTERRUPTS 5
#define MP$UNBLOCK_INTERRUPTS 6
 
#pragma member_alignment __restore
/*** MODULE $cserve_def ***/
#pragma member_alignment __save
#pragma nomember_alignment
#define CSERVE$SET_HWE 8
#define CSERVE$CLEAR_HWE 9
#define CSERVE$LDLP 16
#define CSERVE$STLP 17
#define CSERVE$PCI_INTR_ENABLE 52
#define CSERVE$PCI_INTR_DISABLE 53
#define CSERVE$HALT 64
#define CSERVE$WHAMI 65
#define CSERVE$START 66
#define CSERVE$CALLBACK 67
#define CSERVE$MTPR_EXC_ADDR 68
#define CSERVE$JUMP_TO_ARC 69
 
#pragma member_alignment __restore
