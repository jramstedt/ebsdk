/*
 * Copyright (C) 1994 by
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
 * Abstract:	GAMMA CPU CSR Definitions
 *
 * Author:	Marco Ciaffi
 *
 * Modifications:
 *
 */
 
/*Platform Specific Read and Write macros*/

/* Read/write cpu csr macro */
/* Address of cpu csr c on module in slot s */
/* s - CPU slot of desired board */
/* c - offset in bytes into csrs on module */
/* v - value to write to the csr */

#define cpu_base_addr(s) ((0x80000000)+((three+0x80)<<32)+(s<<24))

#define CPU_CSR_BASE(s) cpu_base_addr(s)
#define READ_CPU_CSR(s,c)  (*(volatile unsigned __int64 *)(CPU_CSR_BASE(s)+c))
#define WRITE_CPU_CSR(s,c,v) (*(volatile unsigned __int64 *)(CPU_CSR_BASE(s)+c) = v)
#define _READ_CPU_CSR(c)  (*(volatile unsigned __int64 *)(p_cpu_csr+c))
#define _WRITE_CPU_CSR(c,v) (*(volatile unsigned __int64 *)(p_cpu_csr+c) = v)

/*Bit Definitions*/

/*                                                                          			*/
/* Configuration Register (CREG-CSR00)		                    	    			*/
/*                                                                          			*/
#define CREG_ADDR		0x00000000	/* Address offset for CSR00			*/
#define CREG_ENA_BUS_SIZE0	0x00001000	/* Enable bus sizing support0			*/
#define CREG_ENA_EXCH_DLY0	0x00100000	/* Enable exchange delay0			*/
#define CREG_ENA_FAST_FIL0	0x00800000	/* Enable fast fill0				*/
#define CREG_DIS_IDL_STAL0	0x01000000	/* Disable idlebc-cstall0			*/
#define CREG_ENA_4IDLE0		0x02000000	/* Enable 4idlebc0				*/
#define CREG_ACK_MB0		0x04000000	/* Acknowledge memory barrier			*/
#define CREG_ACK_SET_DIRTY0	0x08000000	/* Acknowledge set_dirty/lock			*/

/*                                                                          			*/
/* Error SUmmary Register (ESREG-CSR01)                    		    			*/
/*                                                                          			*/
#define ESREG_ADDR		0x0020          /* Address offset for CSR01			*/

/*                                                                          			*/
/* EVB Control Register (EVBCR-CSR02)              		    	    			*/
/*                                                                          			*/
#define EVBCR_ADDR		0x0040          /* Address offset for CSR02			*/	
#define EVBCR_ADDR_CMD_PAR_CHK	0x00000001	/* Enable address-cmd bus parity0 checking	*/
#define EVBCR_CORR_ERR_INT	0x00000010	/* Enable correction err interrupt0		*/
#define EVBCR_ECC_CORR		0x00000020	/* Enable ECC correction			*/
#define EVBCR_RATT_ECC_CHK	0x00000040	/* Enable RATTLER ECC0 checking			*/
#define EVBCR_FOR_FIL_SHAR	0x08000000	/* Force fill shared				*/

/*                                                                          			*/
/* EVB Victim Error Address Register (EVBVEAR-CSR03)                        			*/
/*                                                                          			*/
#define EVBVEAR_ADDR	0x0060                  /* Address offset for CSR03			*/

/*                                                                          			*/
/* EVB Correctable Error Register (EVBCER-CSR04)		            			*/
/*                                                                          			*/
#define EVBCER_ADDR	0x0080                  /* Address offset for CSR04			*/

/*                                                                          			*/
/* EVB Correctable Error Address Register (EVBCEAR-CSR05)		    			*/
/*                                                                          			*/
#define EVBCEAR_ADDR	0x00a0                  /* Address offset for CSR05			*/

/*                                                                          			*/
/* EVB Uncorrectable Error Register (EVBUER-CSR06)		            			*/
/*                                                                          			*/
#define EVBUER_ADDR	0x00c0                  /* Address offset for CSR06			*/

/*                                                                          			*/
/* EVB Uncorrectable Error Address Register (EVBUEAR-CSR07)		    			*/
/*                                                                          			*/
#define EVBUEAR_ADDR	0x00e0                  /* Address offset for CSR07			*/

/*                                                                          			*/
/* EVB Reserve Register (EVRESV-CSR08)                       		    			*/
/*                                                                          			*/
#define EVRESV_ADDR	0x0100                  /* Address offset for CSR08			*/

/*                                                                          			*/
/* Duptag Control Register (DTCTR-CSR09)                       		    			*/
/*                                                                          			*/
#define DTCTR_ADDR		0x0120          /* Address offset for CSR09			*/
#define DTCTR_DUP_TAG_ENA	0x00000001	/* Duplicate tag enable 			*/
#define DTCTR_ENA_CNTRL_PAR_CHK	0x00000010	/* Enable control parity checking0		*/
#define DTCTR_FIL_BAD_CNTRL_PAR	0x00000020	/* Fill bad control parity			*/
#define DTCTR_ENA_TAG_PAR_CHK	0x00000100	/* Enable tag parity checking0			*/
#define DTCTR_FIL_BAD_TAG_PAR	0x00000200	/* Fill bad tag parity				*/
#define DTCTR_DUP_TAG_DIAG_MODE	0x00001000	/* Duplicate tag diagnostic mode0		*/

/*                                                                          			*/
/* Duptag Error Register (DTER-CSR10)                      		    			*/
/*                                                                          			*/
#define DTER_ADDR		0x0140          /* Address offset for CSR10			*/

/*                                                                          			*/
/* Duptag Test Control Regiser (DTTCR-CSR11)                       	    			*/
/*                                                                          			*/
#define DTTCR_ADDR		0x0160          /* Address offset for CSR11			*/

/*                                                                          			*/
/* Duptag Test Regiser (DTTR-CSR12)                       		    			*/
/*                                                                          			*/
#define DTTR_ADDR		0x0180          /* Address offset for CSR12			*/

/*                                                                          			*/
/* Duptag Reserve Register (DTRESV-CSR13)                       	    			*/
/*                                                                          			*/
#define DTRESV_ADDR		0x01a0          /* Address offset for CSR13			*/

/*                                                                          			*/
/* I-bus Control and Status Register (IBCSR-CSR14)                          			*/
/*                                                                          			*/
#define IBCSR_ADDR		0x01c0          /* Address offset for CSR14			*/
#define IBCSR_IBUS_PAR_ERR0	0x00000010	/* I-bus parity err0				*/
#define IBCSR_SNP_IBUS_PAR_ERR0	0x00000020	/* Snoop cycle during I-bus parity err0		*/
#define IBCSR_CMD_IBUS_PAR_ERR0	0x00000040	/* Commander cycle during I-bus parity err0	*/
#define IBCSR_ENA_IBUS_PAR_CHK	0x00001000	/* Enable I-bus parity checking0		*/
#define IBCSR_DRV_BAD_IBUS_PAR0	0x00002000	/* Drive bad I-bus parity0			*/

/*                                                                          			*/
/* I-bus Error Address Register (IBEAR-CSR15)                        	    			*/
/*                                                                          			*/
#define IBEAR_ADDR		0x01e0          /* Address offset for CSR15			*/

/*                                                                          			*/
/* Arbitration Control Register (ACR-CSR16)                        	    			*/
/*                                                                          			*/
#define ACR_ADDR		0x0200          /* Address offset for CSR16			*/
#define ACR_CBUS2_EQUAL0	0x00000001	/* Cbus2 equalizer0				*/
#define ACR_BAD_MOD_ENA0	0x00000010	/* Bad mode enable0				*/
#define ACR_DON_MOD_ENA0	0x00000020	/* Donate mode enable0				*/
#define ACR_PWN_MOD_ENA0	0x00000100	/* Pawn mode enable0				*/
#define ACR_BCREQ_ENA0		0x00000200	/* BCREQ enable0				*/
#define ACR_DIS_CBUS2_REQ0	0x00001000	/* Disable Cbus2 request0			*/

/*                                                                          			*/
/* Cobra-bus2 Control Register (CBCR-CSR17)                        	    			*/
/*                                                                          			*/
#define CBCR_ADDR		0x0220          /* Address offset for CSR17			*/
#define CBCR_ENA_PAR_CHK0	0x00000001	/* Enable parity checking0			*/
#define CBCR_DATA_WRG_PAR0	0x00000002	/* Data wrong parity0				*/
#define CBCR_CA_WRG_PAR0	0x00000004	/* C/A wrong parity0				*/
#define CBCR_FRC_SHR		0x00000010	/* Force shared					*/
#define CBCR_ENA_CBUS_ERR_INT0	0x00001000	/* Enable cbus error interrupt0			*/
#define CBCR_DIS_SHR_RES_CHK	0x00010000	/* Disable shared response checking		*/

/*                                                                          			*/
/* Cobra-bus2 Error Register (CBER-CSR18)                        	    			*/
/*                                                                          			*/
#define CBER_ADDR		0x0240          /* Address offset for CSR18			*/
#define CBER_UCORR_READ_ERR0	0x00000001	/* Uncorrectable read err0			*/
#define CBER_CA_LOW_LW_PAR_ERR0	0x00000010	/* C/A low longword parity err0			*/
#define CBER_FIL_BAD_CNTRL_PAR	0x00000020	/* Fill bad control parity			*/
#define CBER_ENA_TAG_PAR_CHK	0x00000100	/* Enable tag parity checking0			*/
#define CBER_FIL_BAD_TAG_PAR	0x00000200	/* Fill bad tag parity				*/
#define CBER_DUP_TAG_DIAG_MODE	0x00001000	/* Duplicate tag diagnostic mode0		*/
#define CBER_DUP_TAG_ENA	0x00000001	/* Duplicate tag enable 			*/
#define CBER_ENA_CNTRL_PAR_CHK	0x00000010	/* Enable control parity checking0		*/
#define CBER_FIL_BAD_CNTRL_PAR	0x00000020	/* Fill bad control parity			*/
#define CBER_ENA_TAG_PAR_CHK	0x00000100	/* Enable tag parity checking0			*/
#define CBER_FIL_BAD_TAG_PAR	0x00000200	/* Fill bad tag parity				*/
#define CBER_DUP_TAG_DIAG_MODE	0x00001000	/* Duplicate tag diagnostic mode0		*/

/*                                                                          			*/
/* Cobra-bus2 Error Address Low Register (CBEALR-CSR19)                     			*/
/*                                                                          			*/
#define CBEALR_ADDR		0x0260          /* Address offset for CSR19			*/

/*                                                                          			*/
/* Cobra-bus2 Error Address High Register (CBEAHR-CSR20)                    			*/
/*                                                                          			*/
#define CBEAHR_ADDR		0x0280          /* Address offset for CSR20			*/

/*                                                                          			*/
/* Cobra-bus2 Reserve Register (CBRESV-CSR21)                        	    			*/
/*                                                                          			*/
#define CBRESV_ADDR		0x02a0          /* Address offset for CSR21			*/

/*                                                                          			*/
/* Address Lock Register (ALR-CSR22)                        		    			*/
/*                                                                          			*/
#define ALR_ADDR		0x02c0          /* Address offset for CSR22			*/

/*                                                                          			*/
/* Processor Mailbox Register (PMBR-CSR23)                        	    			*/
/*                                                                          			*/
#define PMBR_ADDR		0x02e0          /* Address offset for CSR23			*/

/*                                                                          			*/
/* Interprocessor Interrupt Request Register (IIRR-CSR24)                   			*/
/*                                                                          			*/
#define IIRR_ADDR		0x0300          /* Address offset for CSR24			*/

/*                                                                          			*/
/* System Interrupt Clear Register (SICR-CSR25)                        	    			*/
/*                                                                          			*/
#define SICR_ADDR		0x0320          /* Address offset for CSR25			*/
#define SICR_CBUS2_ERR_INT_CLR	0x00000001	/* Cobra-bus2 error interrupt clear		*/

/*                                                                          			*/
/* Performance Monitor Control Register (PMCR-CSR26)                        			*/
/*                                                                          			*/
#define PMCR_ADDR		0x0340          /* Address offset for CSR26			*/

/*                                                                          			*/
/* Performance Register1 (PMR1-CSR27)                        		    			*/
/*                                                                          			*/
#define PMR1_ADDR		0x0360          /* Address offset for CSR27			*/

/*                                                                          			*/
/* Performance Register2 (PMR2-CSR28)                        		    			*/
/*                                                                          			*/
#define PMR2_ADDR		0x0380          /* Address offset for CSR28			*/

/*                                                                          			*/
/* Performance Register3 (PMR3-CSR29)                        		    			*/
/*                                                                          			*/
#define PMR3_ADDR		0x03a0          /* Address offset for CSR29			*/

/*                                                                          			*/
/* Performance Register1 (PMR4-CSR30)                        		    			*/
/*                                                                          			*/
#define PMR4_ADDR		0x03c0          /* Address offset for CSR30			*/

/*                                                                          			*/
/* Performance Register5 (PMR1-CSR31)                        		    			*/
/*                                                                          			*/
#define PMR5_ADDR		0x03e0          /* Address offset for CSR31			*/
