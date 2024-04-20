/********************************************************************************************************************************/
/* Created:  9-APR-1997 14:25:22 by OpenVMS SDL EV1-33     */
/* Source:  10-JAN-1997 15:16:54 ALPHA_FW$:[ALPHA_FW.REF]TSUNAMI.SDL;40 */
/********************************************************************************************************************************/
/*** MODULE TSUNAMI_CSRDEF IDENT V1.0 ***/
#ifndef tsunami_h               /* Include file sentry. */
#define	tsunami_h
  
#include "cp$src:platform.h"	/* ALPHA */
#if !ALPHA
  #error "These 64-bit constants are not supported on VAX.\n" /
	 "This code must be built on an Alpha system." 
#else                   
  #define	CSR_CSC		0x801A0000000
  #define	CSR_MTR		0x801A0000040
  #define	CSR_MISC	0x801A0000080
  #define	CSR_MPD		0x801A00000C0
  #define	CSR_AAR0	0x801A0000100
  #define	CSR_AAR1	0x801A0000140
  #define	CSR_AAR2	0x801A0000180
  #define	CSR_AAR3	0x801A00001C0
  #define	CSR_DIM0	0x801A0000200
  #define	CSR_DIM1	0x801A0000240
  #define	CSR_DIR0	0x801A0000280
  #define	CSR_DIR1	0x801A00002C0
                                                    
  #define	CSR_DRIR	0x801A0000300
  #define	CSR_PRBEN	0x801A0000340
  #define	CSR_IIC	       	0x801A0000380
  #define	CSR_WDR	       	0x801A00003C0
  #define	CSR_MPR0	0x801A0000400
  #define	CSR_MPR1	0x801A0000440
  #define	CSR_MPR2	0x801A0000480
  #define	CSR_MPR3	0x801A00004C0
  #define	CSR_TTR		0x801A0000580
  #define	CSR_TDR		0x801A00005C0
  #define	CSR_DIM2	0x801A0000600
  #define	CSR_DIM3	0x801A0000640
  #define	CSR_DIR2	0x801A0000680
  #define	CSR_DIR3	0x801A00006C0

  #define	CSR_DSC	       	0x801B0000800
  #define	CSR_STR		0x801B0000840
  #define	CSR_DREV	0x801B0000880
  #define	CSR_DSC2	0x801B00008C0
                                                     
  #define	PCHIP0_WSBA0  	0x80180000000
  #define	PCHIP0_WSBA1  	0x80180000040
  #define	PCHIP0_WSBA2  	0x80180000080
  #define	PCHIP0_WSBA3  	0x801800000C0
                                                     
  #define	PCHIP0_WSM0  	0x80180000100
  #define	PCHIP0_WSM1  	0x80180000140
  #define	PCHIP0_WSM2  	0x80180000180
  #define	PCHIP0_WSM3  	0x801800001C0
  #define	PCHIP0_TBA0  	0x80180000200
  #define	PCHIP0_TBA1  	0x80180000240
  #define	PCHIP0_TBA2  	0x80180000280
  #define	PCHIP0_TBA3  	0x801800002C0
                                                     
  #define	PCHIP0_PCTL  	0x80180000300
  #define	PCHIP0_PLAT  	0x80180000340
  #define	PCHIP0_RESERVED	0x80180000380
  #define	PCHIP0_PERROR	0x801800003c0
  #define	PCHIP0_PERRMASK	0x80180000400
  #define	PCHIP0_PERRSET 	0x80180000440
  #define	PCHIP0_TLBIV  	0x80180000480
  #define	PCHIP0_TLBIA 	0x801800004C0
  #define	PCHIP0_PMONCTL	0x80180000500
  #define	PCHIP0_PMONCNT	0x80180000540
  #define	PCHIP0_SPRST	0x80180000800
                                                     
  #define	PCHIP1_WSBA0  	0x80380000000
  #define	PCHIP1_WSBA1  	0x80380000040
  #define	PCHIP1_WSBA2  	0x80380000080
  #define	PCHIP1_WSBA3  	0x803800000C0
  #define	PCHIP1_WSM0  	0x80380000100
  #define	PCHIP1_WSM1  	0x80380000140
  #define	PCHIP1_WSM2  	0x80380000180
  #define	PCHIP1_WSM3  	0x803800001C0
                                                    
  #define	PCHIP1_TBA0  	0x80380000200
  #define	PCHIP1_TBA1  	0x80380000240
  #define	PCHIP1_TBA2  	0x80380000280
  #define	PCHIP1_TBA3  	0x803800002C0
                                                     
  #define	PCHIP1_PCTL  	0x80380000300
  #define	PCHIP1_PLAT  	0x80380000340
  #define	PCHIP1_RESERVED	0x80380000380
  #define	PCHIP1_PERROR	0x803800003c0
  #define	PCHIP1_PERRMASK	0x80380000400
  #define	PCHIP1_PERRSET	0x80380000440
  #define	PCHIP1_TLBIV  	0x80380000480
  #define	PCHIP1_TLBIA	0x803800004C0
  #define	PCHIP1_PMONCTL	0x80380000500
  #define	PCHIP1_PMONCNT	0x80380000540
  #define	PCHIP1_SPRST	0x80380000800
#endif    /* #if !ALPHA */
/*                                                                          */
/* TSUNAMI Pchip Error register.                                            */
/*                                                                          */
#define perror_m_lost 0x1
#define perror_m_serr 0x2
#define perror_m_perr 0x4
#define perror_m_dcrto 0x8
#define perror_m_sge 0x10
#define perror_m_ape 0x20
#define perror_m_ta 0x40
#define perror_m_rdpe 0x80
#define perror_m_nds 0x100
#define perror_m_rto 0x200
#define perror_m_uecc 0x400
#define perror_m_cre 0x800
#define perror_m_addrl 0xFFFFFFFF0000
#define perror_m_addrh 0x7000000000000
#define perror_m_cmd 0xF0000000000000
#define perror_m_syn 0xFF00000000000000
union TPchipPERROR {   
    struct  {
        unsigned int perror_v_lost : 1;
        unsigned perror_v_serr : 1;
        unsigned perror_v_perr : 1;
        unsigned perror_v_dcrto : 1;
        unsigned perror_v_sge : 1;
        unsigned perror_v_ape : 1;
        unsigned perror_v_ta : 1;
        unsigned perror_v_rdpe : 1;
        unsigned perror_v_nds : 1;
        unsigned perror_v_rto : 1;
        unsigned perror_v_uecc : 1;
        unsigned perror_v_cre : 1;                 
        unsigned perror_v_rsvd1 : 4;
        unsigned perror_v_addrl : 32;
        unsigned perror_v_addrh : 3;
        unsigned perror_v_rsvd2 : 1;
        unsigned perror_v_cmd : 4;
        unsigned perror_v_syn : 8;
        } perror_r_bits;
    int perror_q_whole [2];
    } ;                       
/*                                                                          */
/* TSUNAMI Pchip Window Space Base Address register.                        */
/*                                                                          */
#define wsba_m_ena 0x1                
#define wsba_m_sg 0x2
#define wsba_m_ptp 0x4
#define wsba_m_addr 0xFFF00000  
#define wmask_k_sz1gb 0x3FF00000                   
union TPchipWSBA {
    struct  {
        unsigned wsba_v_ena : 1;
        unsigned wsba_v_sg : 1;
        unsigned wsba_v_ptp : 1;
        unsigned wsba_v_rsvd1 : 17;
        unsigned wsba_v_addr : 12;
        unsigned wsba_v_rsvd2 : 32;
        } wsba_r_bits;
    int wsba_q_whole [2];
    } ;
/*									    */
/* TSUNAMI Pchip Control Register					    */
/*									    */
#define pctl_m_fdsc 0x1
#define pctl_m_fbtb 0x2
#define pctl_m_thdis 0x4
#define pctl_m_chaindis 0x8
#define pctl_m_tgtlat 0x10
#define pctl_m_hole 0x20
#define pctl_m_mwin 0x40
#define pctl_m_arbena 0x80
#define pctl_m_prigrp 0x7F00
#define pctl_m_ppri 0x8000
#define pctl_m_rsvd1 0x30000
#define pctl_m_eccen 0x40000
#define pctl_m_padm 0x80000
#define pctl_m_cdqmax 0xF00000
#define pctl_m_rev 0xFF000000
#define pctl_m_crqmax 0xF00000000
#define pctl_m_ptpmax 0xF000000000                                                                              
#define pctl_m_pclkx 0x30000000000
#define pctl_m_fdsdis 0x40000000000
#define pctl_m_fdwdis 0x80000000000
#define pctl_m_ptevrfy 0x100000000000
#define pctl_m_rpp 0x200000000000
#define pctl_m_pid 0xC00000000000
#define pctl_m_rsvd2 0xFFFF000000000000

union TPchipPCTL {
    struct {
	unsigned pctl_v_fdsc : 1;
	unsigned pctl_v_fbtb : 1;
	unsigned pctl_v_thdis : 1;
	unsigned pctl_v_chaindis : 1;
	unsigned pctl_v_tgtlat : 1;
	unsigned pctl_v_hole : 1;
	unsigned pctl_v_mwin : 1;
	unsigned pctl_v_arbena : 1;
	unsigned pctl_v_prigrp : 7;
	unsigned pctl_v_ppri : 1;
	unsigned pctl_v_rsvd1 : 2;
	unsigned pctl_v_eccen : 1;
	unsigned pctl_v_padm : 1;
	unsigned pctl_v_cdqmax : 4;
	unsigned pctl_v_rev : 8;
	unsigned pctl_v_crqmax : 4;
	unsigned pctl_v_ptpmax : 4;
	unsigned pctl_v_pclkx : 2;
	unsigned pctl_v_fdsdis : 1;
	unsigned pctl_v_fdwdis : 1;
	unsigned pctl_v_ptevrfy : 1;
	unsigned pctl_v_rpp : 1;
	unsigned pctl_v_pid : 2;
	unsigned pctl_v_rsvd2 : 16;
	} pctl_r_bits;
    int pctl_q_whole [2];
} ;
/*                                                                          */
/* TSUNAMI Pchip Error Mask Register.                                       */
/*                                                                          */
#define perrmask_m_lost 0x1
#define perrmask_m_serr 0x2
#define perrmask_m_perr 0x4
#define perrmask_m_dcrto 0x8
#define perrmask_m_sge 0x10
#define perrmask_m_ape 0x20
#define perrmask_m_ta 0x40
#define perrmask_m_rdpe 0x80
#define perrmask_m_nds 0x100
#define perrmask_m_rto 0x200
#define perrmask_m_uecc 0x400
#define perrmask_m_cre 0x800
#define perrmask_m_rsvd 0xFFFFFFFFFFFFF000
union TPchipPERRMASK {   
    struct  {
        unsigned int perrmask_v_lost : 1;
        unsigned perrmask_v_serr : 1;
        unsigned perrmask_v_perr : 1;
        unsigned perrmask_v_dcrto : 1;
        unsigned perrmask_v_sge : 1;
        unsigned perrmask_v_ape : 1;
        unsigned perrmask_v_ta : 1;
        unsigned perrmask_v_rdpe : 1;
        unsigned perrmask_v_nds : 1;
        unsigned perrmask_v_rto : 1;
        unsigned perrmask_v_uecc : 1;
        unsigned perrmask_v_cre : 1;                 
        unsigned perrmask_v_rsvd1 : 20;
	unsigned perrmask_v_rsvd2 : 32;
        } perrmask_r_bits;
    int perrmask_q_whole [2];
    } ;                       

/*                                                                          */
/* TSUNAMI Cchip Array Address Register.                                    */
/*                                                                          */
#define aar_m_addr  0xFF000000
#define aar_m_asiz  0x00007000
#define aar_m_sa    0x00000100
#define aar_m_rbb   0x0000000C
union TCchipAAR {
    struct {
	unsigned aar_v_rsvd1	: 2;
	unsigned aar_v_rbb	: 2;
	unsigned aar_v_rsvd2	: 4;
	unsigned aar_v_sa	: 1;
	unsigned aar_v_rsvd3	: 3;
	unsigned aar_v_asiz	: 3;
	unsigned aar_v_rsvd4	: 9;
	unsigned aar_v_addr	: 8;
	unsigned aar_v_rsvd5	: 32; 
	} aar_r_bits;
    int aar_q_whole [2];
    } ;

#endif			/* tsunami_h file sentry */
