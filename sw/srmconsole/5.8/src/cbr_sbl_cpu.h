/*
 * Copyright (C) 1993 by
 * Digital Equipment Corporation, Maynard, Massachusetts.
 * All rights reserved.
 *
 * This software is furnished under a license and may be used and copied
 * only  in  accordance  of  the  terms  of  such  license  and with the
 * inclusion of the above copyright notice. This software or  any  other
 * copies thereof may not be provided or otherwise made available to any
 * other person.  No title to and  ownership of the  software is  hereby
 * transferred.
 *
 * The information in this software is  subject to change without notice
 * and  should  not  be  construed  as a commitment by Digital Equipment
 * Corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by Digital.
 *
 *
 * Abstract:	COBRA/SABLE cpu Definitions
 *
 * Author:	John A. DeNisco
 *
 * Modifications:
 *
 *	hcb	20-Jul-1993	Add the rest of the CSR offsets
 *
 *	jad	15-Jul-1993	Initial entry.
 */
 
/*Platform Specific Read and Write macros*/

/* Read/write cpu csr macro */
/* Address of cpu csr c on module in slot s */
/* s - CPU slot of desired board */
/* c - offset in bytes into csrs on module */
/* v - value to write to the csr */

#if SABLE
#define cpu_base_addr(s) ((0x80000000)+(three<<32)+(s<<24))
#endif

#define CPU_CSR_BASE(s) cpu_base_addr(s)
#define READ_CPU_CSR(s,c) (*(volatile unsigned __int64 *)(CPU_CSR_BASE(s)+c))
#define WRITE_CPU_CSR(s,c,v) (*(volatile unsigned __int64 *)(CPU_CSR_BASE(s)+c) = v)
#define _READ_CPU_CSR(c) (*(volatile unsigned __int64 *)(p_cpu_csr+c))
#define _WRITE_CPU_CSR(c,v) (*(volatile unsigned __int64 *)(p_cpu_csr+c) = v)

/*Bit Definitions*/
/*                                                                          */
/* B-Cache Control Register (BCC-CSR0)		                            */
/*                                                                          */
#define BCC_ADDR	0x0000		/* BCC Address			    */
#define BCC_ENA_ALLOC	0x00000001	/* Enable allocate                  */
#define BCC_FF_SH	0x00000002	/* Force Fill shared                */
#define BCC_ENA_TAG	0x00000004	/* Enable Tag & Dup Tag Par Chk     */
#define BCC_FL_WRG_TAG	0x00000008	/* Fill Wrong Tag Par               */
#define BCC_FL_WRG_CTL	0x00000010	/* Fill Wrong Control Par           */
#define BCC_FL_WRG_DTAG	0x00000020	/* Fill Wrong Dup Tag Store Par     */
#define BCC_ENA_CINT	0x00000040	/* Enable Corr err Int              */
#define BCC_ENA_ECC_COR	0x00000080	/* Enable ECC Correction            */
#define BCC_ENA_EDC	0x00000100	/* Enable EDC Check                 */
#define BCC_ENA_BC_CIOU	0x00000200	/* Enable Cond I/O Updates          */
#define BCC_ENA_BLK_WRT	0x00000400	/* Enable Block Write Around        */
#define BCC_ENA_BC_INIT	0x00000800	/* Enable B-Cache Init              */
#define BCC_FRC_EDC  	0x00001000	/* Force EDC Control                */

/*                                                                          */
/* B-Cache Correctable Error (BCCE-CSR1)                       		    */
/*                                                                          */
#define BCCE_ADDR	0x0020		/* BCCE Address			    */
#define BCCE_MSD_COR_ERR 0x00000004	/* Missed Correctable Error	    */
#define BCCE_COR_ERR 	0x00000008	/* Correctable Error		    */
#define BCCE_CTL_BT_PAR 0x00000100	/* Control Bit Parity		    */
#define BCCE_SHARED 	0x00000200	/* Shared			    */
#define BCCE_DIRTY 	0x00000400	/* Dirty			    */
#define BCCE_VALID 	0x00000800	/* Valid			    */
#define BCCE_BC_EDC_ERR 0x00002000	/* BC EDC Error 		    */

/*                                                                          */
/* B-Cache Correctable Error Address (BCCEA-CSR2)              		    */
/*                                                                          */
#define BCCEA_ADDR	0x0040		/* BCCEA Address	      	    */
#define BCCEA_TAG_PAR	0x00040000	/* Tag Parity			    */

/*                                                                          */
/* B-Cache Uncorrectable Error (BCUE-CSR3)                     		    */
/*                                                                          */
#define BCUE_ADDR	0x0060		/* BCUE Address			    */
#define BCUE_MSD_PAR_ERR 0x00000001	/* Missed Parity Error		    */
#define BCUE_PAR_ERR 	0x00000002	/* Parity Error			    */
#define BCUE_MSD_UCR_ERR 0x00000004	/* Missed Uncorrectable Error	    */
#define BCUE_UNCOR_ERR	0x00000008	/* Uncorrectable Error		    */
#define BCUE_CTL_BT_PAR 0x00000100	/* Control Bit Parity		    */
#define BCUE_SHARED 	0x00000200	/* Shared			    */
#define BCUE_DIRTY 	0x00000400	/* Dirty			    */
#define BCUE_VALID 	0x00000800	/* Valid			    */
#define BCUE_BC_EDC_ERR 0x00020000	/* BC EDC error			    */

/*                                                                          */
/* B-Cache Uncorrectable Error Address (BCUEA-CSR4)		            */
/*                                                                          */
#define BCUEA_ADDR	0x0080		/* BCUEA Address	      	    */
#define BCUEA_P_TAG_PAR	0x00020000	/* Predicted Tag Parity		    */
#define BCUEA_TAG_PAR	0x00040000	/* Tag Parity			    */

/*                                                                          */
/* Duplicate Tag Error Register (DTER-CSR5)		                    */
/*                                                                          */
#define DTER_ADDR	0x00a0		/* DTER Address			    */
#define DTER_MSD_ERR_OC 0x00000001	/* Missed Error Occurred	    */
#define DTER_ERROR 	0x00000002	/* Error			    */
#define DTER_DUP_TAG_PR	0x40000000	/* Dup Tag Parity		    */

/*                                                                          */
/* Cobra-bus Control Register (CBCTL-CSR6)                     		    */
/*                                                                          */
#define CBCTL_ADDR	0x00c0		/* CBCTL Address	      	    */
#define CBCTL_DAT_WR_PR 0x00000001	/* Data Wrong Parity		    */
#define CBCTL_ENA_PR_CK	0x00000008	/* C/A Wrong Parity		    */
#define CBCTL_FOR_SH	0x00000010	/* Forced Shared		    */
#define CBCTL_ENB_ERINT	0x00000800	/* Enable C-Bus Error Interrupt	    */
#define CBCTL_RES_DIAG 	0x00001000	/* Reserved Diagnostic		    */

/*                                                                          */
/* Cobra-bus Error Register (CBE-CSR7)                         		    */
/*                                                                          */
#define CBE_ADDR	0x00e0		/* CBE Address			    */
#define CBE_RES_DIAG 	0x00000002	/* Reserved Diagnostic		    */
#define CBE_CA_PAR_ERR 	0x00000004	/* C/A Parity Error		    */
#define CBE_MSD_CA_ERR 	0x00000008	/* Missed C/A Error		    */
#define CBE_PAR_ERR_WR 	0x00000010	/* Parity Error	on write	    */
#define CBE_MSD_ERR_WR	0x00000020	/* Missed error on write	    */
#define CBE_PAR_ERR_RD	0x00000040	/* Parity error on read		    */
#define CBE_MSD_ERR_RD	0x00000080	/* Missed error on read		    */
#define CBE_CA_PR_ER_L0 0x00000100	/* C/A Parity Error on LW0	    */
#define CBE_CA_PR_ER_L2 0x00000200	/* C/A Parity Error on LW2	    */
#define CBE_DT_PR_ER_L0 0x00000400	/* C/A Data Parity Error on LW0	    */
#define CBE_DT_PR_ER_L2 0x00000800	/* C/A Data Parity Error on LW2	    */
#define CBE_DT_PR_ER_L4 0x00001000	/* C/A Data Parity Error on LW4	    */
#define CBE_DT_PR_ER_L6 0x00002000	/* C/A Data Parity Error on LW6	    */
#define CBE_CA_NO_ACK	0x00004000	/* C/A Not Acked		    */
#define CBE_WR_DAT_NACK	0x00008000	/* Write data not acked		    */
#define CBE_MADR_VD	0x80000000	/* MADR vaild			    */

/*                                                                          */
/* Cobra-bus Error Register (CBEAL-CSR8)                       		    */
/*                                                                          */
#define CBEAL_ADDR	0x0100		/* CBEAL Address		    */

/*                                                                          */
/* Cobra-bus Error Register (CBEAH-CSR9)                       		    */
/*                                                                          */
#define CBEAH_ADDR	0x0120		/* CBEAH Address		    */

/*                                                                          */
/* Cobra-bus Error Register (PMBX-CSR10)                      		    */
/*                                                                          */
#define PMBX_ADDR	0x0140		/* PMBX Address			    */

/*                                                                          */
/* Cobra-bus Error Register (IPIR-CSR11)                       		    */
/*                                                                          */
#define IPIR_ADDR	0x0160		/* IPIR Address			    */

/*                                                                          */
/* System Interrupt Clear (SIC-CSR12, 2.0x00.0180)                          */
/*                                                                          */
#define SIC_ADDR	0x0180		/* SIC Address			    */
#define SIC_INT_CLR	0x00000004	/* Interrupt Clear		    */

/*                                                                          */
/* Cobra-bus Error Register (ADLK-CSR13)                       		    */
/*                                                                          */
#define ADLK_ADDR	0x01a0		/* ADLK Address			    */

/*                                                                          */
/* Cobra-bus Error Register (MADRL-CSR14)                     		    */
/*                                                                          */
#define MADRL_ADDR	0x01c0		/* MADRL Address		    */

/*                                                                          */
/* Cobra-bus Error Register (CRR-CSR15)                        		    */
/*                                                                          */
#define CRR_ADDR	0x01e0		/* CRR Address			    */
