/********************************************************************************************************************************/
/* Created: 21-AUG-2000 16:04:56 by OpenVMS SDL EV1-31     */
/* Source:   7-JUL-2000 13:42:58 NIGHTRIDER:[CONSOLE.V58.COMMON.SRC]EV6_PC264_PA */
/********************************************************************************************************************************/
/* file:	ev6_pc264_impure_def.sdl                                    */
/*                                                                          */
/* Abstract:	PAL impure scratch area and logout area data structure      */
/* 		definitions for Alpha firmware.                             */
/*                                                                          */
/*                                                                          */
/*** MODULE $pal_impure ***/
#ifndef IMPURE_DEF_H
#define IMPURE_DEF_H
#define IMPURE$K_REVISION 1             /* Revision number of this file     */
union TMCES {
    struct  {
        unsigned mchk : 1;
        unsigned sce : 1;
        unsigned pce : 1;
        unsigned dpc : 1;
        unsigned dsc : 1;
        unsigned fields$$v_fill_0 : 3;
        } fields;
    char whole;
    } ;
struct TPAL_P_MISC_REG {
    unsigned sw : 2;
    unsigned ip : 1;
    unsigned cm : 2;
    unsigned rsvd1 : 2;
    unsigned vmm : 1;
    unsigned ipl : 5;
    unsigned rsvd2 : 3;
    union TMCES mces;
    unsigned scbv : 16;
    unsigned mchkCode : 16;
    unsigned switch1 : 1;
    unsigned phys : 1;
    unsigned TPAL_P_M$$V_FILL_1 : 6;
    } ;
struct TPAL_PCTX_REG {
    unsigned rsvd1 : 1;
    unsigned ppce : 1;
    unsigned fpe : 1;
    unsigned rsvd2 : 2;
    unsigned aster : 4;
    unsigned astrr : 4;
    unsigned rsvd3a : 19;
    unsigned rsvd3b : 7;
    unsigned asn : 8;
    unsigned rsvd4 : 17;
    } ;
/*                                                                          */
/* Structure of the processor-specific impure area                          */
/*                                                                          */
#define cns$r0 16
#define cns$r1 24
#define cns$r2 32
#define cns$r3 40
#define cns$r4 48
#define cns$r5 56
#define cns$r6 64
#define cns$r7 72
#define cns$r8 80
#define cns$r9 88
#define cns$r10 96
#define cns$r11 104
#define cns$r12 112
#define cns$r13 120
#define cns$r14 128
#define cns$r15 136
#define cns$r16 144
#define cns$r17 152
#define cns$r18 160
#define cns$r19 168
#define cns$r20 176
#define cns$r21 184
#define cns$r22 192
#define cns$r23 200
#define cns$r24 208
#define cns$r25 216
#define cns$r26 224
#define cns$r27 232
#define cns$r28 240
#define cns$r29 248
#define cns$r30 256
#define cns$r31 264
#define cns$f0 272
#define cns$f1 280
#define cns$f2 288
#define cns$f3 296
#define cns$f4 304
#define cns$f5 312
#define cns$f6 320
#define cns$f7 328
#define cns$f8 336
#define cns$f9 344
#define cns$f10 352
#define cns$f11 360
#define cns$f12 368
#define cns$f13 376
#define cns$f14 384
#define cns$f15 392
#define cns$f16 400
#define cns$f17 408
#define cns$f18 416
#define cns$f19 424
#define cns$f20 432
#define cns$f21 440
#define cns$f22 448
#define cns$f23 456
#define cns$f24 464
#define cns$f25 472
#define cns$f26 480
#define cns$f27 488
#define cns$f28 496
#define cns$f29 504
#define cns$f30 512
#define cns$f31 520
#define cns$pt_offset 536
#define PT$_IMPURE 0
#define PT$_WHAMI 2
#define PT$_SCC 4
#define PT$_PRBR 6
#define PT$_PTBR 8
#define PT$_PCBB 18
#define PT$_PS 60
#define cns$size 1072
#define pal$impure_size 1072
#define cns$mchksize 544
struct impure {
    int cns$flag [2];
    int cns$hlt [2];
    int cns$gpr [2] [32];
    int cns$fpr [2] [32];
    int cns$mchkflag [2];
    int cns$pt [2] [1];
    int cns$whami [2];
    int cns$scc [2];
    int cns$prbr [2];
    int cns$ptbr [2];
    int cns$trap [2];
    int cns$halt_code [2];
    int cns$ksp [2];
    int cns$scbb [2];
    int cns$pcbb [2];
    int cns$vptb [2];
    int cns$pt_pad [2] [13];
    int cns$shadow4 [2];
    int cns$shadow5 [2];
    int cns$shadow6 [2];
    int cns$shadow7 [2];
    int cns$shadow20 [2];
    int cns$p_temp [2];
    struct TPAL_P_MISC_REG cns$p_misc;
    int cns$shadow23 [2];
    int cns$fpcr [2];
    int cns$va [2];
    int cns$va_ctl [2];
    int cns$exc_addr [2];
    int cns$ier_cm [2];
    int cns$sirr [2];
    int cns$isum [2];
    int cns$exc_sum [2];
    int cns$pal_base [2];
    int cns$i_ctl [2];
    int cns$pctr_ctl [2];
    struct TPAL_PCTX_REG cns$process_context;
    int cns$i_stat [2];
    int cns$dtb_alt_mode [2];
    int cns$mm_stat [2];
    int cns$m_ctl [2];
    int cns$dc_ctl [2];
    int cns$dc_stat [2];
    int cns$write_many [2];
    int cns$virbnd [2];
    int cns$sysptbr [2];
    int cns$report_lam [2];
    int cns$exit_console [2];
    int cns$work_request [2];
    int cns$cchip_dirx [2];
    int cns$cchip_misc [2];
    int cns$pchip0_err [2];
    int cns$pchip1_err [2];
    int cns$srom_srom_rev [2];          /* Parameters passed up from srom   */
    int cns$srom_proc_id [2];
    int cns$srom_mem_size [2];
    int cns$srom_cycle_cnt [2];
    int cns$srom_signature [2];
    int cns$srom_proc_mask [2];
    int cns$srom_sysctx [2];
    } ;
#define cns__r0 16
#define cns__r1 24
#define cns__r2 32
#define cns__r3 40
#define cns__r4 48
#define cns__r5 56
#define cns__r6 64
#define cns__r7 72
#define cns__r8 80
#define cns__r9 88
#define cns__r10 96
#define cns__r11 104
#define cns__r12 112
#define cns__r13 120
#define cns__r14 128
#define cns__r15 136
#define cns__r16 144
#define cns__r17 152
#define cns__r18 160
#define cns__r19 168
#define cns__r20 176
#define cns__r21 184
#define cns__r22 192
#define cns__r23 200
#define cns__r24 208
#define cns__r25 216
#define cns__r26 224
#define cns__r27 232
#define cns__r28 240
#define cns__r29 248
#define cns__r30 256
#define cns__r31 264
#define cns__f0 272
#define cns__f1 280
#define cns__f2 288
#define cns__f3 296
#define cns__f4 304
#define cns__f5 312
#define cns__f6 320
#define cns__f7 328
#define cns__f8 336
#define cns__f9 344
#define cns__f10 352
#define cns__f11 360
#define cns__f12 368
#define cns__f13 376
#define cns__f14 384
#define cns__f15 392
#define cns__f16 400
#define cns__f17 408
#define cns__f18 416
#define cns__f19 424
#define cns__f20 432
#define cns__f21 440
#define cns__f22 448
#define cns__f23 456
#define cns__f24 464
#define cns__f25 472
#define cns__f26 480
#define cns__f27 488
#define cns__f28 496
#define cns__f29 504
#define cns__f30 512
#define cns__f31 520
#define cns__pt_offset 536
#define cns__impure 536
#define cns__whami 544
#define cns__scc 552
#define cns__prbr 560
#define cns__ptbr 568
#define cns__trap 576
#define cns__halt_code 584
#define cns__ksp 592
#define cns__scbb 600
#define cns__pcbb 608
#define cns__vptb 616
#define cns__pt11 624
#define cns__pt12 632
#define cns__pt13 640
#define cns__pt14 648
#define cns__pt15 656
#define cns__pt16 664
#define cns__pt17 672
#define cns__pt18 680
#define cns__pt19 688
#define cns__pt20 696
#define cns__pt21 704
#define cns__pt22 712
#define cns__pt23 720
#define cns__ipr_offset 792
#define cns__size 1072
struct __impure {
    int cns__flag [2];
    int cns__halt [2];
    int cns__gpr [2] [32];
    int cns__fpr [2] [32];
    int cns__mchkflag [2];
    int cns__pt [2] [24];
    int cns__p4 [2];
    int cns__p5 [2];
    int cns__p6 [2];
    int cns__p7 [2];
    int cns__p20 [2];
    int cns__p_temp [2];
    int cns__p_misc [2];
    int cns__p23 [2];
    int cns__fpcr [2];
    int cns__va [2];
    int cns__va_ctl [2];
    int cns__exc_addr [2];
    int cns__ier_cm [2];
    int cns__sirr [2];
    int cns__isum [2];
    int cns__exc_sum [2];
    int cns__pal_base [2];
    int cns__i_ctl [2];
    int cns__pctr_ctl [2];
    int cns__process_context [2];
    int cns__i_stat [2];
    int cns__dtb_alt_mode [2];
    int cns__mm_stat [2];
    int cns__m_ctl [2];
    int cns__dc_ctl [2];
    int cns__dc_stat [2];
    int cns__write_many [2];
    int cns__virbnd [2];
    int cns__sysptbr [2];
    int cns__report_lam [2];
    int cns__exit_console [2];
    int cns__work_request [2];
    int cns__cchip_dirx [2];
    int cns__cchip_misc [2];
    int cns__pchip0_err [2];
    int cns__pchip1_err [2];
    int cns__srom_rev [2];              /* Parameters passed up from srom   */
    int cns__proc_id [2];
    int cns__mem_size [2];
    int cns__cycle_cnt [2];
    int cns__signature [2];
    int cns__proc_mask [2];
    int cns__sysctx [2];
    } ;
/* PALVERSION is used in hwrpb.c                                            */
struct PALVERSION {
    unsigned char minor;                /* ev_driver.c                      */
    unsigned char major;
    unsigned short int is_v;
    unsigned short int xxx;
    unsigned short int max_cpu;
    } ;
 
/*** MODULE $pal_logout ***/
/*                                                                          */
/* Start definition of Corrected Error Frame                                */
/*                                                                          */
#define MCHK_CRD__REV 1
#define mchk$crd_cpu_base 24
#define mchk_crd_system_base 88
#define mchk$crd_size 128
struct crd_logout {
    int mchk$crd_flag [2];
    int mchk$crd_offsets [2];
/* Pal-specific information	                                            */
    int mchk$crd_mchk_code;
    int mchk$crd_frame_rev;
/* CPU-specific information                                                 */
    int mchk$crd_i_stat [2];
    int mchk$crd_dc_stat [2];
    int mchk$crd_c_addr [2];
    int mchk$crd_dc1_syndrome [2];
    int mchk$crd_dc0_syndrome [2];
    int mchk$crd_c_stat [2];
    int mchk$crd_c_sts [2];
    int mchk$crd_mm_stat [2];
/* system specific information                                              */
    int mchk$crd_os_flags [2];
    int mchk$crd_cchip_dir [2];
    int mchk$crd_cchip_misc [2];
    int mchk$crd_pchip0_err [2];
    int mchk$crd_pchip1_err [2];
    } ;
#define mchk_crd__cpu_base 24
#define mchk_crd__system_base 88
#define mchk_crd__size 128
struct crd__logout {
    int mchk_crd__flag_frame [2];
    int mchk_crd__offsets [2];
/* pal-specific information	                                            */
    int mchk_crd__mchk_code [2];
/* cpu-specific information                                                 */
    int mchk_crd__i_stat [2];
    int mchk_crd__dc_stat [2];
    int mchk_crd__c_addr [2];
    int mchk_crd__dc1_syndrome [2];
    int mchk_crd__dc0_syndrome [2];
    int mchk_crd__c_stat [2];
    int mchk_crd__c_sts [2];
    int mchk_crd__mm_stat [2];
/* system-specific information                                              */
    int mchk_crd__os_flags [2];
    int mchk_crd__cchip_dir [2];
    int mchk_crd__cchip_misc [2];
    int mchk_crd__pchip0_err [2];
    int mchk_crd__pchip1_err [2];
    } ;
#define MCHK__REV 1
#define mchk$cpu_base 24
#define mchk$sys_base 160
#define mchk$size 200
#define mchk$crd_base 0
#define mchk$mchk_base 128
#define pal$logout_area 24576
#define pal$logout_base 24576
#define PAL__LOGOUT_BASE 24576
#define pal$logout_specific_size 1024
#define PAL__LOGOUT_SPECIFIC_SIZE 1024
struct logout {
    int mchk$flag [2];
    int mchk$offsets [2];
/* Pal-specific information                                                 */
    int mchk$mchk_code [2];
/* CPU-specific information                                                 */
    int mchk$i_stat [2];
    int mchk$dc_stat [2];
    int mchk$c_addr [2];
    int mchk$dc1_syndrome [2];
    int mchk$dc0_syndrome [2];
    int mchk$c_stat [2];
    int mchk$c_sts [2];
    int mchk$mm_stat [2];
    int mchk$exc_addr [2];
    int mchk$ier_cm [2];
    int mchk$isum [2];
    int mchk$reserved_0 [2];
    int mchk$pal_base [2];
    int mchk$i_ctl [2];
    int mchk$process_context [2];
    int mchk$reserved_1 [2];
    int mchk$reserved_2 [2];
/* System-specific information                                              */
    int mchk$os_flags [2];
    int mchk$cchip_dir [2];
    int mchk$cchip_misc [2];
    int mchk$pchip0_err [2];
    int mchk$pchip1_err [2];
    } ;
#define mchk__cpu_base 24
#define mchk__system_base 160
#define mchk__size 200
#define mchk_crd__base 0
#define mchk__base 128
struct __logout {
    int mchk__flag_frame [2];
    int mchk__offsets [2];
/* pal-specific information                                                 */
    int mchk__mchk_code [2];
/* cpu-specific information                                                 */
    int mchk__i_stat [2];
    int mchk__dc_stat [2];
    int mchk__c_addr [2];
    int mchk__dc1_syndrome [2];
    int mchk__dc0_syndrome [2];
    int mchk__c_stat [2];
    int mchk__c_sts [2];
    int mchk__mm_stat [2];
    int mchk__exc_addr [2];
    int mchk__ier_cm [2];
    int mchk__isum [2];
    int mchk__reserved_0 [2];
    int mchk__pal_base [2];
    int mchk__i_ctl [2];
    int mchk__process_context [2];
    int mchk__reserved_1 [2];
    int mchk__reserved_2 [2];
/* system-specific information                                              */
    int mchk__os_flags [2];
    int mchk__cchip_dir [2];
    int mchk__cchip_misc [2];
    int mchk__pchip0_err [2];
    int mchk__pchip1_err [2];
    } ;
#define SYS_EVENT__REV 1
#define sys_event__cpu_base 24
/* System-specific information                                              */
#define sys_event__system_base 24
#define sys_event__size 112
#define sys_event__base 328
struct sys_event__logout {
    int sys_event__flag_frame [2];
    int sys_event__offsets [2];
/* pal-specific information                                                 */
    int sys_event__mchk_code [2];
/* cpu-specific information                                                 */
    int sys_event__os_flags [2];
    int sys_event__dir [2];
    int sys_event__system_0 [2];
    int sys_event__system_1 [2];
    int sys_event__reserved_0 [2];
    int sys_event__reserved_1 [2];
    int sys_event__reserved_2 [2];
    int sys_event__reserved_3 [2];
    int sys_event__reserved_4 [2];
    int sys_event__reserved_5 [2];
    int sys_event__reserved_6 [2];
/* system-specific information                                              */
    } ;
#endif
 
