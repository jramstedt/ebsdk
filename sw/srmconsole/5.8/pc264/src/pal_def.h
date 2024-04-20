/********************************************************************************************************************************/
/* Created: 21-AUG-2000 16:05:06 by OpenVMS SDL EV1-31     */
/* Source:  30-NOV-1999 10:38:03 NIGHTRIDER:[CONSOLE.V58.COMMON.SRC]EV6_PC264_PA */
/********************************************************************************************************************************/
/* file:     	ev6_pc264_pal_defs.sdl	                                    */
/*                                                                          */
/* History:	                                                            */
/*	E. Goehl	5-Oct-1996 	Port of ev5_eb164_paldef.sdl        */
/*                                                                          */
/* Abstract:	Cserve defintions  for                                      */
/*		the ev6 PAlcode on PC264 .....                              */
/*                                                                          */
/*                                                                          */
/*** MODULE $pal_def ***/
#define PAL$CPU0_START_BASE 256
#define PAL$CPU1_START_BASE 264
#define PAL$PRIMARY 512
#define PAL$CALLBACK 528
#define PAL$HALT_SWITCH_IN 544
#define PAL$HWRPB_BASE 8192
#define PAL$IMPURE_BASE 16384
#define PAL$IMPURE_COMMON_SIZE 512
#define PAL$IMPURE_SPECIFIC_SIZE 1536
#define PAL__IMPURE_BASE 16384
#define PAL__IMPURE_COMMON_SIZE 512
#define PAL__IMPURE_SPECIFIC_SIZE 1536
#define PAL$LOGOUT_BASE 24576
#define PAL$LOGOUT_SPECIFIC_SIZE 1024
#define PAL$TEMPS_BASE 28672
#define PAL$TEMPS_SPECIFIC_SIZE 512
#define PAL__TEMPS_BASE 28672
#define PAL__TEMPS_SPECIFIC_SIZE 512
#define PAL$PAL_BASE 32768
#define PAL$OSFPAL_BASE 98304
#define PAL$CONSOLE_BASE 147456
#define PAL$PAL_SIZE 65536
#define PAL$OSFPAL_SIZE 49152
#define PAL__PAL_BASE 32768
#define PAL__OSFPAL_BASE 98304
#define PAL__CONSOLE_BASE 147456
/*                                                                          */
/* DEFINE ADDRESSES OF LOCATIONS IN THE IMPURE                              */
/* COMMON AREA. ASSUMES BASE AT 0X4000 AND LOCATIONS                        */
/* STARTING AT 0X100.                                                       */
#define PAL$EVMSVERSION 16640
#define PAL$EVMSBUILD_BITMASK 16648
#define PAL$OSFVERSION 16656
#define PAL$ENTER_CONSOLE_PTR 16704
#define PAL$ENTER_OSF 16768
#define PAL__ENTER_OSF 16768
/*                                                                          */
/*  DEFINE DEVICE INTERRUPT IRQ(S) AS A MASK IN ISUM<EI> FIELD              */
/*                                                                          */
#define IRQ_DEV__M 2                    /* IRQ1                             */
#define IMPURE$PCB_OFFSET 16
#define IMPURE$PROF_OFFSET 64
#define IMPURE$QUILT_OFFSET 64
#define IMPURE$UART_OFFSET 112
#define IMPURE$CPU_TYPE_OFFSET 128
#define MP$RESTART 1
#define MP$HALT 2
#define MP$CONTROL_P 3
#define MP$BREAK 4
/*=======================                                                   */
/* Define cserve funtions                                                   */
/*=======================                                                   */
 
/*** MODULE $cserve_def ***/
#define CSERVE$SET_HWE 8
#define CSERVE$CLEAR_HWE 9
#define CSERVE$LDLP 16
#define CSERVE$STLP 17
#define CSERVE$LDBP 18
#define CSERVE$STBP 19
#define CSERVE$HALT 64
#define CSERVE$WHAMI 65
#define CSERVE$START 66
#define CSERVE$CALLBACK 67
#define CSERVE$MTPR_EXC_ADDR 68
#define CSERVE$JUMP_TO_ARC 69
#define CSERVE$IIC_WRITE 70
#define CSERVE$SROM_WRITE 71
#define CSERVE$SROM_INIT 72
#define CSERVE$MP_WORK_REQUEST 101
#define CSERVE__START 66
#define CSERVE__CALLBACK 67
 
