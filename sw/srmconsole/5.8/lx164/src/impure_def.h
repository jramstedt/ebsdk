/********************************************************************************************************************************/
/* Created: 21-JUN-2000 10:31:18 by OpenVMS SDL EV1-33     */
/* Source:  16-SEP-1998 15:36:26 AFW_USER$:[CONSOLE.V58.COMMON.SRC]EV5_LX164_IMP */
/********************************************************************************************************************************/
/* file:	ev5_lx164_impure.sdl                                        */
/*                                                                          */
/* Abstract:	PAL impure scratch area and logout area data structure definitions for */
/*		AlphaPC 164/LX SRM console firmware.                        */
/*                                                                          */
/*                                                                          */
/*** MODULE $pal_impure ***/
#pragma member_alignment __save
#pragma nomember_alignment
#ifndef EV5_LX164_IMPURE_DEF
#define EV5_LX164_IMPURE_DEF 1
#define IMPURE$K_REVISION 1             /* Revision number of this file     */
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
#define cns$pt0 536
#define cns$pt1 544
#define cns$pt2 552
#define cns$pt3 560
#define cns$pt4 568
#define cns$pt5 576
#define cns$pt6 584
#define cns$pt7 592
#define cns$pt8 600
#define cns$pt9 608
#define cns$pt10 616
#define cns$pt11 624
#define cns$pt12 632
#define cns$pt13 640
#define cns$pt14 648
#define cns$pt15 656
#define cns$pt16 664
#define cns$pt17 672
#define cns$pt18 680
#define cns$pt19 688
#define cns$pt20 696
#define cns$pt21 704
#define cns$pt22 712
#define cns$pt23 720
#define cns$pt24 728
#define cns$ipr_offset 792
#define cns$size 1152
#define cns$mchksize 624
struct impure {
    int cns$flag [2];
    int cns$hlt [2];
    int cns$gpr [2] [32];
    int cns$fpr [2] [32];
    int cns$mchkflag [2];
    int cns$pt [2] [24];
    int cns$shadow8 [2];
    int cns$shadow9 [2];
    int cns$shadow10 [2];
    int cns$shadow11 [2];
    int cns$shadow12 [2];
    int cns$shadow13 [2];
    int cns$shadow14 [2];
    int cns$shadow25 [2];
    int cns$exc_addr [2];
    int cns$pal_base [2];
    int cns$mm_stat [2];
    int cns$va [2];
    int cns$icsr [2];
    int cns$ipl [2];
    int cns$ps [2];                     /* Ibox current mode                */
    int cns$itb_asn [2];
    int cns$aster [2];
    int cns$astrr [2];
    int cns$isr [2];
    int cns$ivptbr [2];
    int cns$mcsr [2];
    int cns$dc_mode [2];
    int cns$maf_mode [2];
    int cns$sirr [2];
    int cns$fpcsr [2];
    int cns$icperr_stat [2];
    int cns$pmctr [2];
    int cns$exc_sum [2];
    int cns$exc_mask [2];
    int cns$intid [2];
    int cns$dcperr_stat [2];
    int cns$sc_stat [2];
    int cns$sc_addr [2];
    int cns$sc_ctl [2];
    int cns$bc_tag_addr [2];
    int cns$ei_stat [2];
    int cns$ei_addr [2];
    int cns$fill_syn [2];
    int cns$ld_lock [2];
    int cns$bc_ctl [2];                 /* shadow of on chip bc_ctl         */
    int cns$pmctr_ctl [2];              /* saved frequency select info for pmctr */
    int cns$bc_config [2];              /* shadow of on chip bc_config      */
    int cns$pal_type [2];
    int cns$intr_mask [2];              /* shadow of PCI interrupt mask     */
/*                                                                          */
/* Parameters passed in from the SROM                                       */
/*                                                                          */
    int cns$srom_bc_ctl [2];            /* Current BC_CTL value             */
    int cns$srom_bc_cfg [2];            /* Current BC_CONFIG value          */
    int cns$srom_srom_rev [2];          /* SROM revision                    */
    int cns$srom_proc_id [2];           /* Processor identifier             */
    int cns$srom_mem_size [2];          /* Size of contiguous good memory (bytes) */
    int cns$srom_cycle_cnt [2];         /* Cycle count (picoseconds)        */
    int cns$srom_signature [2];         /* Signature and system revision identifier */
    int cns$srom_proc_mask [2];         /* Active processor mask            */
    int cns$srom_sysctx [2];            /* System context value             */
    } ;
struct EV5PMCTRCTL_BITS {
    unsigned PMCTR_CTL$V_SPROCESS : 1;
    unsigned PMCTR_CTL$$$_FILL_0 : 3;
    unsigned PMCTR_CTL$V_FRQ2 : 2;
    unsigned PMCTR_CTL$V_FRQ1 : 2;
    unsigned PMCTR_CTL$V_FRQ0 : 2;
    unsigned PMCTR_CTL$V_CTL2 : 2;
    unsigned PMCTR_CTL$V_CTL1 : 2;
    unsigned PMCTR_CTL$V_CTL0 : 2;
    unsigned PMCTR_CTL$$$_FILL_1 : 16;
    unsigned PMCTR_CTL$$$_FILL_2 : 32;
    } ;
/*                                                                          */
/* Hacks to make builds work.                                               */
/*                                                                          */
#define PT$_IMPURE 3                    /* entry.c                          */
#define PT$_INTMASK 8                   /* entry.c                          */
#define PT$_PS 9                        /* hwrpb.c                          */
#define PT$_MCES 16                     /* ipr_driver.c                     */
#define PT$_WHAMI 16                    /* ipr_driver.c                     */
#define PT$_SCC 17                      /* entry.c                          */
#define PT$_PRBR 18                     /* ipr_driver.c                     */
#define PT$_PTBR 20                     /* alphamm.c                        */
#define PT$_L3VIRT 21                   /* boot.c                           */
#define PT$_SCBB 22                     /* ipr_driver.c                     */
#define PT$_PCBB 23                     /* hwrpb.c                          */
struct PALVERSION {
    unsigned char minor;
    unsigned char major;
    unsigned char is_v;
    unsigned char xxx;
    } ;
 
#pragma member_alignment __restore
/*** MODULE $pal_logout ***/
#pragma member_alignment __save
#pragma nomember_alignment
/*                                                                          */
/* Start definition of Corrected Error Frame                                */
/*                                                                          */
#define mchk$crd_cpu_base 24
#define mchk$crd_sys_base 56
#define mchk$crd_size 104
struct crd_logout {
    int mchk$crd_flag [2];
    int mchk$crd_offsets [2];
/* Pal-specific information	                                            */
    int mchk$crd_mchk_code [2];
/* CPU-specific information                                                 */
    int mchk$crd_ei_addr [2];
    int mchk$crd_fill_syn [2];
    int mchk$crd_ei_stat [2];
    int mchk$crd_isr [2];
/* System-specific information                                              */
    int mchk$crd_pyxis_syn [2];         /* Syndrome register                */
    int mchk$crd_pyxis_mear [2];        /* Memory error address register    */
    int mchk$crd_pyxis_mesr [2];        /* Memory error status register     */
    int mchk$crd_pyxis_stat [2];        /* Status register                  */
    int mchk$crd_pyxis_err [2];         /* Error register                   */
    int mchk$crd_pyxis_err_data [2];    /* Error data CSR                   */
    } ;
/*                                                                          */
/* Start definition of Machine check logout Frame                           */
/*                                                                          */
#define mchk$cpu_base 280
#define mchk$sys_base 416
#define mchk$size 616
#define pal$logout_area 24576
#define pal$logout_base 24576
#define mchk$crd_base 0
#define mchk$mchk_base 104
struct logout {
    int mchk$flag [2];
    int mchk$offsets [2];
/* Pal-specific information                                                 */
    int mchk$mchk_code [2];
    int mchk$shadow [2] [8];
    int mchk$pt [2] [24];
/* CPU-specific information                                                 */
    int mchk$exc_addr [2];
    int mchk$exc_sum [2];
    int mchk$exc_mask [2];
    int mchk$pal_base [2];
    int mchk$isr [2];
    int mchk$icsr [2];
    int mchk$ic_perr_stat [2];
    int mchk$dc_perr_stat [2];
    int mchk$va [2];
    int mchk$mm_stat [2];
    int mchk$sc_addr [2];
    int mchk$sc_stat [2];
    int mchk$bc_tag_addr [2];
    int mchk$ei_addr [2];
    int mchk$fill_syn [2];
    int mchk$ei_stat [2];
    int mchk$ld_lock [2];
/* System-specific information                                              */
    int mchk$sys_reserved [2];
    int mchk$pyxis_err_data [2];        /* Error data CSR                   */
    int mchk$pyxis_err [2];             /* Error register                   */
    int mchk$pyxis_stat [2];            /* Status register                  */
    int mchk$pyxis_err_mask [2];        /* Error mask register              */
    int mchk$pyxis_syn [2];             /* Syndrome register                */
    int mchk$pyxis_mear [2];            /* Memory error address register    */
    int mchk$pyxis_mesr [2];            /* Memory error status register     */
    int mchk$pci_err0 [2];              /* PCI error status register 0      */
    int mchk$pci_err1 [2];              /* PCI error status register 1      */
    int mchk$sio_nmisc [2];             /* NMI status and control register  */
    int mchk$pci_err2 [2];              /* PCI error status register 2      */
    int mchk$pyxis_hae_mem [2];         /* Host Address Extension register  */
    int mchk$sio_rev [2];               /* Revision identification          */
    int mchk$sio_ubcsa [2];             /* Utility bus chip select enable A */
    int mchk$sio_ubcsb [2];             /* Utility bus chip select enable B */
    int mchk$sio_imr0 [2];              /* Interrupt mask register (master) */
    int mchk$sio_imr1 [2];              /* Interrupt mask register (slave)  */
    int mchk$sio_irr0 [2];              /* Interrupt request register (master) */
    int mchk$sio_irr1 [2];              /* Interrupt request register (slave) */
    int mchk$sio_isr0 [2];              /* Interrupt status register (master) */
    int mchk$sio_isr1 [2];              /* Interrupt status register (slave) */
    int mchk$sio_dma_stat0 [2];         /* DMA status register 0            */
    int mchk$sio_dma_stat1 [2];         /* DMA status register 1            */
    int mchk$sio_status [2];            /* Device status                    */
    } ;
#endif
 
#pragma member_alignment __restore
