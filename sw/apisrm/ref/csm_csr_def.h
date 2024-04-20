/* file:	csm_csr_def.h                                                   */
/*                                                                          */
/* Copyright (C) 1993, 1994 by                                              */
/* Digital Equipment Corporation, Maynard, Massachusetts.                   */
/* All rights reserved.                                                     */
/*                                                                          */
/* This software is furnished under a license and may be used and copied    */
/* only  in  accordance  of  the  terms  of  such  license  and with the    */
/* inclusion of the above copyright notice. This software or  any  other    */
/* copies thereof may not be provided or otherwise made available to any    */
/* other person.  No title to and  ownership of the  software is  hereby    */
/* transferred.                                                             */
/*                                                                          */
/* The information in this software is  subject to change without notice    */
/* and  should  not  be  construed  as a commitment by digital equipment    */
/* corporation.                                                             */
/*                                                                          */
/* Digital assumes no responsibility for the use  or  reliability of its    */
/* software on equipment which is not supplied by digital.                  */
/*                                                                          */
/*                                                                          */
/* Abstract:	Cobra/Sable Memory Module CSR structure definitions.        */
/*                                                                          */
/* Author:	Harold Buckingham                                           */
/*                                                                          */
/* Modifications:                                                           */
/*                                                                          */
/*	hcb	10-Jan-1994	Add define for syndrome mask		    */
/*                                                                          */
/*	hcb	16-Jun-1993	Initial entry. Split from cmm_def.h	    */
/*                                                                          */

/*Register definitions						    */
#define csm_error 	0    	/* Error Register 1                 */
#define csm_ctrap1 	0x20    /* Command Trap 1                   */
#define csm_ctrap2 	0x40    /* Command Trap 2                   */
#define csm_config 	0x60    /* Configuration Register           */
#define csm_edc_stat1 	0x80    /* EDC Status 1                     */
#define csm_edc_stat2 	0xa0    /* EDC Status 2                     */
#define csm_edc_ctrl 	0xc0    /* EDC Control                      */
#define csm_sb_ctrl 	0xe0    /* Stream Buffer Control            */
#define csm_ref_ctrl 	0x100   /* Refresh Control                  */
#define csm_fltr_ctrl 	0x120   /* CRD Filter Control		    */

#define memcsr0$m_err1 	  1
#define memcsr0$m_sync1   2
#define memcsr0$m_cape1   4
#define memcsr0$m_mcape1  8
#define memcsr0$m_wdpe1   0x10
#define memcsr0$m_mwdpe1  0x20
#define memcsr0$m_cape_0  0x100
#define memcsr0$m_cape_2  0x200
#define memcsr0$m_dpe_0   0x400
#define memcsr0$m_dpe_2   0x800
#define memcsr0$m_dpe_4   0x1000
#define memcsr0$m_dpe_6   0x2000
#define memcsr0$m_ucerr1  0x10000
#define memcsr0$m_mucerr1 0x20000
#define memcsr0$m_crd1    0x40000
#define memcsr0$m_mcrd1   0x80000
struct MEM_CSR_ERR {
    unsigned memcsr0$v_err1 : 1;        /* Error Summary 1                  */
    unsigned memcsr0$v_sync1 : 1;       /* ASIC Sync error 1		    */
    unsigned memcsr0$v_cape1 : 1;       /* C/A Parity Error 1               */
    unsigned memcsr0$v_mcape1 : 1;      /* Missed C/A Parity Error 1        */
    unsigned memcsr0$v_wdpe1 : 1;       /* Write Data Parity Error 1        */
    unsigned memcsr0$v_mwdpe1 : 1;      /* Missed Write Data PE 1           */
    unsigned memcsr0$v_mbz11 : 2;       /* Unused                           */
    unsigned memcsr0$v_cape_0 : 1;      /* C/A Parity Error LW 0            */
    unsigned memcsr0$v_cape_2 : 1;      /* C/A Parity Error LW 2            */
    unsigned memcsr0$v_dpe_0 : 1;       /* Data Parity Error LW 0           */
    unsigned memcsr0$v_dpe_2 : 1;       /* Data Parity Error LW 2           */
    unsigned memcsr0$v_dpe_4 : 1;       /* Data Parity Error LW 4           */
    unsigned memcsr0$v_dpe_6 : 1;       /* Data Parity Error LW 6           */
    unsigned memcsr0$v_mbz12 : 2;       /* Unused                           */
    unsigned memcsr0$v_ucerr1 : 1;      /* EDC Uncorrectable Error 1        */
    unsigned memcsr0$v_mucerr1 : 1;     /* Missed EDC Uncorr Error 1        */
    unsigned memcsr0$v_crd1 : 1;        /* EDC Correctable Error 1          */
    unsigned memcsr0$v_mcrd1 : 1;       /* Missed EDC Corr Error 1          */
    unsigned memcsr0$v_mbz13 : 12;      /* Unused                           */
    unsigned memcsr0$v_err2 : 1;        /* Error Summary 2                  */
    unsigned memcsr0$v_sync2 : 1;       /* ASIC Sync error 2                */
    unsigned memcsr0$v_cape2 : 1;       /* C/A Parity Error 2               */
    unsigned memcsr0$v_mcape2 : 1;      /* Missed C/A Parity Error 2        */
    unsigned memcsr0$v_wdpe2 : 1;       /* Write Data Parity Error 2        */
    unsigned memcsr0$v_mwdpe2 : 1;      /* Missed Write Data PE 2           */
    unsigned memcsr0$v_mbz21 : 2;       /* Unused                           */
    unsigned memcsr0$v_cape_1 : 1;      /* C/A Parity Error LW 1            */
    unsigned memcsr0$v_cape_3 : 1;      /* C/A Parity Error LW 3            */
    unsigned memcsr0$v_dpe_1 : 1;       /* Data Parity Error LW 1           */
    unsigned memcsr0$v_dpe_3 : 1;       /* Data Parity Error LW 3           */
    unsigned memcsr0$v_dpe_5 : 1;       /* Data Parity Error LW 4           */
    unsigned memcsr0$v_dpe_7 : 1;       /* Data Parity Error LW 7           */
    unsigned memcsr0$v_mbz22 : 2;       /* Unused                           */
    unsigned memcsr0$v_ucerr2 : 1;      /* EDC Uncorrectable Error 2        */
    unsigned memcsr0$v_mucerr2 : 1;     /* Missed EDC Uncorr Error 2        */
    unsigned memcsr0$v_crd2 : 1;        /* EDC Correctable Error 2          */
    unsigned memcsr0$v_mcrd2 : 1;       /* Missed EDC Corr Error 2          */
    unsigned memcsr0$v_mbz23 : 12;      /* Unused                           */
    } ;
struct MEM_CSR_CTRAP1 {
    unsigned memcsr1$v_ca31_0 : 32;     /* Cmd/Addr <31:0>                  */
    unsigned memcsr1$v_ca63_32 : 32;    /* Cmd/Addr <63:32>                 */
    } ;
struct MEM_CSR_CTRAP2 {  
    unsigned memcsr2$v_ca95_64 : 32;    /* Cmd/Addr <95:64>                 */
    unsigned memcsr2$v_ca127_96 : 32;   /* Cmd/Addr <127:96>                */
    } ;
#define memcsr3$m_mid1 3 
#define memcsr3$m_rev1 0xf000
#define memcsr3$m_acc_sel1 8
#define memcsr3$m_siz1 0xf0
#define memcsr3$m_diag1 0x100
#define memcsr3$m_dis_alt_csr1 0x10000
#define memcsr3$m_ilvm1 0xc0000
#define memcsr3$m_ilvu1 0x300000
#define memcsr3$m_badd1 0x7fc00000
#define memcsr3$m_enab1 0x80000000
struct MEM_CSR_CONFIG {
    unsigned memcsr3$v_mid1 : 2;        /* Backplane Slot ID 1              */
    unsigned memcsr3$v_mbz10 : 1;       /* Unused                           */
    unsigned memcsr3$v_acc_sel1 : 1;    /* DRAM Access select 1             */
    unsigned memcsr3$v_siz1 : 4;        /* Module Size 1                    */
    unsigned memcsr3$v_diag1 : 1;       /* Diagnostic Mode 1                */
    unsigned memcsr3$v_mbz12 : 3;       /* Unused                           */
    unsigned memcsr3$v_rev1 : 4;        /* CMIC revision 1                  */
    unsigned memcsr3$v_dis_alt_csr1 : 1;/* Disable Alternate CSR map 1      */
    unsigned memcsr3$v_mbz13 : 1;       /* Unused                           */
    unsigned memcsr3$v_ilvm1 : 2;       /* Interleave Mode 1                */
    unsigned memcsr3$v_ilvu1 : 2;       /* Interleave Unit 1                */
    unsigned memcsr3$v_badd1 : 9;       /* base Address 1                   */
    unsigned memcsr3$v_enab1 : 1;       /* Memory Enable 1                  */
    unsigned memcsr3$v_mid2 : 2;        /* Backplane Slot ID 2              */
    unsigned memcsr3$v_mbz20 : 1;       /* Unused                           */
    unsigned memcsr3$v_acc_sel2 : 1;    /* DRAM Access select 2             */
    unsigned memcsr3$v_siz2 : 4;        /* Module Size 2                    */
    unsigned memcsr3$v_diag2 : 1;       /* Diagnostic Mode 2                */
    unsigned memcsr3$v_mbz22 : 3;       /* Unused                           */
    unsigned memcsr3$v_rev2 : 4;        /* CMIC revision 2                  */
    unsigned memcsr3$v_dis_alt_csr2 : 1;/* Disable Alternate CSR map 2      */
    unsigned memcsr3$v_mbz23 : 1;       /* Unused                           */
    unsigned memcsr3$v_ilvm2 : 2;       /* Interleave Mode 2                */
    unsigned memcsr3$v_ilvu2 : 2;       /* Interleave Unit 2                */
    unsigned memcsr3$v_badd2 : 9;       /* base Address 2                   */
    unsigned memcsr3$v_enab2 : 1;       /* Memory Enable 2                  */
    } ;
struct MEM_CSR_STAT1 {
    unsigned memcsr4$v_rcb1 : 12;       /* Read CBits 1                     */
    unsigned memcsr4$v_mbz10 : 4;       /* Unused                           */
    unsigned memcsr4$v_wcb1 : 12;       /* Write CBits 1                    */
    unsigned memcsr4$v_mbz11 : 4;       /* Unused                           */
    unsigned memcsr4$v_rcb2 : 12;       /* Read CBits 2                     */
    unsigned memcsr4$v_mbz20 : 4;       /* Unused                           */
    unsigned memcsr4$v_wcb2 : 12;       /* Write CBits 2                    */
    unsigned memcsr4$v_mbz21 : 4;       /* Unused                           */
    } ;                                    
#define memcsr5$m_synd1 	0xfff
struct MEM_CSR_STAT2 {
    unsigned memcsr5$v_synd1 : 12;      /* Syndrome 1                       */
    unsigned memcsr5$v_mbz10 : 20;      /* Unused                           */
    unsigned memcsr5$v_synd2 : 12;      /* Syndrome 2                       */
    unsigned memcsr5$v_mbz20 : 20;      /* Unused                           */
    } ;
#define memcsr6$m_usrcb1 	0x1000
#define memcsr6$m_uswcb1 	0x2000
#define memcsr6$m_disinp1 	0x4000
#define memcsr6$m_enoutp1 	0x8000
#define memcsr6$m_cmprdp1 	0x10000000
#define memcsr6$m_crdrpt1 	0x20000000
#define memcsr6$m_edc1 		0x40000000
#define memcsr6$m_edcrpt1 	0x80000000
struct MEM_CSR_EDC_CTRL {                  
    unsigned memcsr6$v_srcb1 : 12;      /* Sub Read CBits 1                 */
    unsigned memcsr6$v_usrcb1 : 1;      /* Use Sub Read CBits 1             */
    unsigned memcsr6$v_uswcb1 : 1;      /* Use Sub Write CBits 1            */
    unsigned memcsr6$v_disinp1 : 1;     /* Disable inbound Parity Chk 1     */
    unsigned memcsr6$v_enoutp1 : 1;     /* Enable outbound Parity Chk 1     */
    unsigned memcsr6$v_swcb1 : 12;      /* Sub Write CBits 1                */
    unsigned memcsr6$v_cmprdp1 : 1;     /* Compare Read Data Parity 1       */
    unsigned memcsr6$v_crdrpt1 : 1;     /* Enable Crd Reporting 1           */
    unsigned memcsr6$v_edc1 : 1;        /* Disable EDC Correction 1         */
    unsigned memcsr6$v_edcrpt1 : 1;     /* Disable EDC Reporting 1          */
    unsigned memcsr6$v_srcb2 : 12;      /* Sub Read CBits 2                 */
    unsigned memcsr6$v_usrcb2 : 1;      /* Use Sub Read CBits 2             */
    unsigned memcsr6$v_uswcb2 : 1;      /* Use Sub Write CBits 2            */
    unsigned memcsr6$v_disinp2 : 1;     /* Disable inbound Parity Chk 2     */
    unsigned memcsr6$v_enoutp2 : 1;     /* Enable outbound Parity Chk 2     */
    unsigned memcsr6$v_swcb2 : 12;      /* Sub Write CBits 2                */
    unsigned memcsr6$v_cmprdp2 : 1;     /* Compare Read Data Parity 2       */
    unsigned memcsr6$v_crdrpt2 : 1;     /* Enable Crd Reporting 2           */
    unsigned memcsr6$v_edc2 : 1;        /* Disable EDC Correction 2         */
    unsigned memcsr6$v_edcrpt2 : 1;     /* Disable EDC Reporting 2          */
    } ;
#define memcsr7$m_strdet1 	1
#define memcsr7$m_strhit1 	2
#define memcsr7$m_strfil1 	4
#define memcsr7$m_strinv1 	8
#define memcsr7$m_rwdir1 	0x10                                            
#define memcsr7$m_frzhis1 	0x20
#define memcsr7$m_enahisinval1 	0x100
#define memcsr7$m_enahisstall1  0x200
#define memcsr7$m_enahishitfil1 0x400
#define memcsr7$m_enafillim1 	0x800
struct MEM_CSR_SB_CTRL {
    unsigned memcsr7$v_strdet1 : 1;     /* Disable Stream Detect 1          */
    unsigned memcsr7$v_strhit1 : 1;     /* Disable Stream Hit 1             */
    unsigned memcsr7$v_strfil1 : 1;     /* Disable Stream Fill 1            */
    unsigned memcsr7$v_strinv1 : 1;     /* Disable Stream Invalidate 1      */
    unsigned memcsr7$v_rwdir1 : 1;      /* Allow Read/Write Direct 1        */
    unsigned memcsr7$v_frzhis1 : 1;     /* Freeze History Buffer 1          */
    unsigned memcsr7$v_mbz10 : 2;       /* Unused                           */
    unsigned memcsr7$v_enahisinval1 : 1;/* Enable His Buf write invalidates 1 */
    unsigned memcsr7$v_enahisstall1 : 1;/* Enable His Buf Cycle 2 stall 1   */
    unsigned memcsr7$v_enahishitfil1 : 1;/* Enable His Buf Hit filter 1     */
    unsigned memcsr7$v_enafillim1 : 1;  /* Enable Fill limit 1   	    */
    unsigned memcsr7$v_mbz11 : 20;      /* Unused                           */
    unsigned memcsr7$v_strdet2 : 1;     /* Disable Stream Detect 2          */
    unsigned memcsr7$v_strhit2 : 1;     /* Disable Stream Hit 2             */
    unsigned memcsr7$v_strfil2 : 1;     /* Disable Stream Fill 2            */
    unsigned memcsr7$v_strinv2 : 1;     /* Disable Stream Invalidate 2      */
    unsigned memcsr7$v_rwdir2 : 1;      /* Allow Read/Write Direct 2        */
    unsigned memcsr7$v_frzhis2 : 1;     /* Freeze History Buffer 2          */
    unsigned memcsr7$v_mbz20 : 2;       /* Unused                           */
    unsigned memcsr7$v_enahisinval2 : 1;/* Enable His Buf write invalidates 2 */
    unsigned memcsr7$v_enahisstall2 : 1;/* Enable His Buf Cycle 2 stall 2   */
    unsigned memcsr7$v_enahishitfil2 : 1;/* Enable His Buf Hit filter 2     */
    unsigned memcsr7$v_enafillim2 : 1;  /* Enable Fill limit 2   	    */
    unsigned memcsr7$v_mbz21 : 20;      /* Unused                           */
    } ;
#define memcsr8$m_ena1 	   0x100  
#define memcsr8$m_enanut1  0x1000
#define memcsr8$m_enahit1  0x2000
struct MEM_CSR_REF_CTRL {
    unsigned memcsr8$v_const1 : 8;      /* Refresh Constant 1               */
    unsigned memcsr8$v_ena1 : 1;        /* Refresh Enable 1                 */
    unsigned memcsr8$v_mbz10 : 3;       /* Unused                           */
    unsigned memcsr8$v_enanut1 : 1;     /* Enable NUT refresh opportunity 1 */
    unsigned memcsr8$v_enahit1 : 1;     /* Enable stream hit refresh opp 1  */
    unsigned memcsr8$v_mbz11 : 18;      /* Unused                           */
    unsigned memcsr8$v_const2 : 8;      /* Refresh Constant 2               */
    unsigned memcsr8$v_ena2 : 1;        /* Refresh Enable 2                 */
    unsigned memcsr8$v_mbz20 : 3;       /* Unused                           */
    unsigned memcsr8$v_enanut2 : 1;     /* Enable NUT refresh opportunity 2 */
    unsigned memcsr8$v_enahit2 : 1;     /* Enable stream hit refresh opp 2  */
    unsigned memcsr8$v_mbz21 : 18;      /* Unused                           */
    } ;                
#define memcsr9$m_ena1  0x4000
struct MEM_CSR_FLTR_CTRL {
    unsigned memcsr9$v_synmask1 : 12;   /* Syndrome mask 1                  */
    unsigned memcsr9$v_banksel1 : 2;    /* Bank select 1                    */
    unsigned memcsr9$v_ena1 : 1;        /* CRD filter enable 1              */
    unsigned memcsr9$v_mbz10 : 17;      /* Unused                           */
    unsigned memcsr9$v_synmask2 : 12;   /* Syndrome mask 2                  */
    unsigned memcsr9$v_banksel2 : 2;    /* Bank select 2                    */
    unsigned memcsr9$v_ena2 : 1;        /* CRD filter enable 2              */
    unsigned memcsr9$v_mbz20 : 17;      /* Unused                           */
    } ;                
