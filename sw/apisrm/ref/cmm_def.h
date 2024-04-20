/* file:	cmm_def.h                                                   */
/*                                                                          */
/* Copyright (C) 1991, 1993 by                                              */
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
/* Abstract:	Cobra/Sable Memory Module data structure definitions.       */
/*                                                                          */
/* Author:	Harold Buckingham                                           */
/*                                                                          */
/* Modifications:                                                           */
/*                                                                          */
/*	hcb	20-Jul-1993	remove read/write cpu csr macro		    */
/*									    */
/*	hcb	22-Jun-1993	Move EDC structure to edc_diag		    */
/*									    */
/*	hcb	16-Jun-1993	Add READ/WRITE MEM_CSR macros		    */
/*				Split the csr defs to csm_csr_def.h	    */
/*                                                                          */
/*	hcb	10-Jun-1993	define some new CSIC bits		    */
/*                                                                          */
/*	hcb	24-May-1993	define CSIC_PASS1 for Cobra and Sable	    */
/*                                                                          */
/*	hcb	23-Apr-1993	Add CSIC CSR support			    */
/*                                                                          */
/*	hcb	23-Apr-1993	change num_bnk_pair to num_banks to support */
/*				one bank modules.			    */
/*                                                                          */
/*	hcb	16-Mar-1993	Update expected csr2 value with new fields  */
/*				used in the fang/sable asics		    */
/*									    */
/*	hcb	05-Jan-1993	Change all pass 3 labels to pass 4	    */
/*									    */
/*	hcb	21-Dec-1992	Add data_cmp_flag to cmm_test_rec	    */
/*									    */
/*	hcb	17-Dec-1992	Add PrestoServe powrup state	    	    */
/*									    */
/*	hcb	06-Nov-1992	Add bank_incr to CMM_TEST structure	    */
/*                                                                          */
/*	hcb	28-Oct-1992	Add some more bits to csr7		    */
/*                                                                          */
/*	hcb	26-Oct-1992	Add bank select defines for all passes of   */
/*				CMIC chip.  Add chip_rev field to cmm_desc  */
/*				Add error code for CMIC mismatch.           */
/*                                                                          */
/*	hcb	08-Oct-1992	Add codes for bank and DRAM threshold errors*/
/*				Remove cmm inprogress flags		    */
/*                                                                          */
/*	hcb	18-Sep-1992	Add defines for hard coded numbers	    */
/*				Add bank error counters to CMM_ERR_LOG      */
/*                                                                          */
/*	hcb	17-Jul-1992	Modify defines for edc test		    */
/*                                                                          */
/*	hcb	07-Jul-1992	Add defines for edc test		    */
/*                                                                          */
/*	hcb	06-Jul-1992	Add chip revison field to csr3 def	    */
/*				Add graycode counter to CMM_TEST	    */
/*                                                                          */
/*	hcb	24-Jun-1992	remove unconfigured from .present Add	    */
/*				config_state to cmm_desc		    */
/*                                                                          */
/*	hcb	18-Jun-1992	Add unconfigured to .present                */
/*                                                                          */
/*	hcb	02-Jun-1992	Only 44 SDD logs not 45			    */
/*                                                                          */
/*	hcb	01-Jun-1992	Add flag for powerup state to CMM, add      */
/*				failure count to CMM_ERR_LOG		    */
/*                                                                          */
/*	hcb	21-May-1992	created from cmm_def.sdl which is now       */
/*				obsolete				    */
/*                                                                          */
/*	hcb	21-May-1992	add interleave structure                    */
/*                              Remove all iic defs                         */
/*                                                                          */
/*	hcb	12-May-1992	add flag to make cmm structure valid        */
/*                                                                          */
/*	hcb	30-Mar-1992	Define the whole iic eeprom                 */
/*                                                                          */
/*	hcb	19-Mar-1992	Add unsniffed to .present                   */
/*                                                                          */
/*	hcb	17-Mar-1992	Add badram to CMM_TEST                      */
/*                                                                          */
/*	hcb	10-Mar-1992	Define inprogress constants                 */
/*                                                                          */
/*	hcb	21-Feb-1992	Change CSR6_MASK for real memory            */
/*                                                                          */
/*	hcb	05-Feb-1992	Change CSR6_MASK to correct value           */
/*                                                                          */
/*	hcb	03-Jan-1992	Add some fields to CMM_TEST                 */
/*                                                                          */
/*	hcb	12-Dec-1991	Fix some structure alignments               */
/*                                                                          */
/*	hcb	24-Oct-1991	Generate csr slot offset in code. Just define */
/*				the offset here.                            */
/*                                                                          */
/*	hcb	17-Sept-1991	Update CSR defs                             */
/*                                                                          */
/*	hcb	29-July-1991	Added IIC error codes                       */
/*                                                                          */
/*	hcb	17-July-1991	Added error structures                      */
/*                                                                          */
/*	hcb	1-June-1991	Initial entry.                              */
/*                                                                          */

/* Read/write mem csr macro */
/* Address of memory csr c on module in slot s */
/* s - memory slot of desired board */
/* c - offset in bytes into csrs on module */
/* v - value to write to the csr */
#define mem_base_addr(s) ((0x40000000) | ((two)<<32) | (s<<28))
#define READ_MEM_CSR(s,c)  (*(unsigned __int64 *)(mem_base_addr(s)|c))
#define WRITE_MEM_CSR(s,c,v) (*(unsigned __int64 *)(mem_base_addr(s)|c) = v)
                                   
/* Address of csr0 for module x */
#define cmm_base_addr(x) (0x40000000 | (x<<28))

/*** MODULE $cmmdef ***/
/*General program constants                                                 */
#define TEST_OFFSET 32                    
#define CSR6_MASK 0xCFFF4FFF
#define ADDR_SEL_MASK 0x1FF80
#define ADDR_SIZE 34              
#define BASE_ADDR_MPLY 0x1000000  
#define RAM_SIZE_MPLY 0x1000000  
#define MIN_BANK_SIZE 0x800000
#define DRAM_THRESHOLD 10                    
#define BANK_THRESHOLD 15
#define DRAM_PER_BANK 70            
#define BANK_PER_BOARD 4
#define MAX_DRAM_PER_BRD 280
#define BITS_PER_SLICE 140               
#define SLICE_BITS_PER_DRAM 2                
#define CMICP1_BANK_SEL 18
#define CMICP2_BANK_SEL 18
#define CMICP4_BANK_SEL 25         
#define TWOKB 2048                           
#define DATA_SYNDROMES 210                   
#define ADDR_SYNDROMES 3            
                        
        
/*                                                                          */
/* Cobra Memory Module powerup state codes                                  */
/*                                                                          */
#define	CMM$K_PUP_STATE_BASE 2      
#define	CMM$K_PUP_STATE_PRESTO 3      

/*                                                                          */
/* Cobra Memory Module CMIC revisions                                       */
/*                                                                          */
#define CMIC_PASS1 0
#define CMIC_PASS2 1
#define CMIC_PASS4 2               
#define CSIC_PASS1 4               
/*                                                                          */
/* Cobra Memory Module IIC EEPROM error codes                               */
/*                                                                          */
#define CMM$K_MISC_ERR 0
#define CMM$K_CRD_ERR 1  
#define CMM$K_UNCRD_ERR 2
#define CMM$K_DRAM_HARD 3  
/*                                                                          */
/* Cobra Memory Module miscellaneous error codes                            */
/*                                                                          */
#define CMM$K_BANK_THRESHOLD_ERR 70       
#define CMM$K_BOARD_THRESHOLD_ERR 71
#define CMM$K_CMIC1_ERR 72
#define CMM$K_CMIC2_ERR 73
#define CMM$K_ADDR_LOGIC_ERR 74   
#define CMM$K_CA_PRTY_ERR1 75            
#define CMM$K_CA_PRTY_ERR2 76  
#define CMM$K_DATA_PRTY_ERR1 77
#define CMM$K_DATA_PRTY_ERR2 78
#define CMM$K_SYNC_ERR1 79
#define CMM$K_SYNC_ERR2 80
#define CMM$K_CMIC_MISMATCH 81
#define CMM$K_TOTAL_ERR_CNT 82      
#define TOTAL_FAIL_BITS ((CMM$K_TOTAL_ERR_CNT-DRAM_PER_BANK+BITS_PER_SLICE)*\
			 SLICE_BITS_PER_DRAM)  

/*                                                                          */
/* Cobra Memory Module EDC test codes                               	    */
/*                                                                          */
#define INIT_C6_L 	0
#define INIT_C6_H 	1
#define INIT_LW  	2
#define EXP_LW0   	3
#define EXP_LW1   	4
#define EXP_LW2   	5     
#define EXP_LW3   	6
#define EXP_LW4   	7
#define EXP_LW5   	8                        
#define EXP_LW6   	9
#define EXP_LW7   	10                   
#define EXP_C0_L  	11
#define EXP_C0_H  	12 
#define B_EXP_C4_L  	13                                     
#define B_EXP_C4_H    	14                           
#define EXP_C4_L  	15                                     
#define EXP_C4_H  	16 
#define EXP_C5_L  	17
#define EXP_C5_H  	18
#define EXP_INT   	19
#define FLAGS  		20
#define PATT_SIZE	21                       
#define BASE_OFFSET 	(EXP_C4_L - B_EXP_C4_L)
#define EXP_C1_L      	0x9000000b
#define EXP_C1_H  	0x9000000b
#define EXP_C2_L  	0x00000003
#define EXP_C2_H  	0x00000003
#define EXP_C2_ID  	0x00200000
/* flag useage in edc test */
#define CHECK_DATA	1

/*                                                                          */
/* These structures are used for error logging and recording the error      */
/* information into the IIC EEPROM.                                         */
/*                                                                          */
struct BAD_RAM_DESCRIP {
    long int crd_err;
    long int uncrd_err;
    long int bad_ram;
    } ;                                            
struct CMM_ERR_LOG {
    struct BAD_BIT_DESCRIP bit_lst [TOTAL_STM_PKT];
    struct BAD_RAM_DESCRIP ram_lst [MAX_DRAM_PER_BRD];
    long int failing_bits [TOTAL_FAIL_BITS];
    long int bank_err[BANK_PER_BOARD];                                 
    long int tot_bank_err;
    long int log_len;
    } ;                
/*+                                                                         */
/* ===============================================                          */
/* = CMM - Cobra Memory Module structure         =                          */
/* ===============================================                          */
/*                                                                          */
/* STRUCTURE OVERVIEW:                                                      */
/*                                                                          */
/*  This structure is used by the Cobra Memory Modules to pass information  */
/*  to routines. There is one structure for each of the 4 CMMs. It contains */
/*  a flag to indicate the installation of the module, the size of the RAMs */
/*  installed and pointers to the error stuctures needed to record error    */
/*  for the module.    	                                                    */
/*                                                                          */
/*                                                                          */
/* STRUCTURE MEMBERS:                                                       */
/*                                                                          */
/*:                                                                         */
/*   ---------------------------------------------------------------------- */
/*   Structure Members    | Data Type  		| Description               */
/*   ---------------------------------------------------------------------- */
/*      cmm_init_flag     |	int  	   	| set to the address of the */
/*		       	  |			| structure when valid      */
/*                        |                     |                           */
/*     	inprogress    	  |	int	   	| 0 not tested yet          */
/*                        |  	       	   	| 1 being tested            */
/*                        |	       	   	| 2 good                    */
/*                        |	       	   	| 3 failed                  */
/*                        |	       	   	| 4 not installed	    */
/*                        |	       	   	|                           */
/*      present    	  |	int	   	| Set if board is installed */
/*   			  |			| 0 FALSE                   */
/*                        |	       	   	| 1 TRUE                    */
/*      chip_rev          |     int             | Value of the CMIC revision*/
/*		       	  |		   	| 0 Pass 1 		    */
/*		       	  |		   	| 1 Pass 2 		    */
/*		       	  |		   	| 2 Pass 3 		    */
/*      powerup_state     |     int             | Set if board is enabled   */
/*		       	  |		   	| at powerup		    */
/*   		       	  |			| 0 FALSE                   */
/*                        |	       	   	| 1 TRUE                    */
/*      config_state      |	int	   	| Set if board is configured*/
/*			  |			| into the system	    */	
/*   			  |			| 0 FALSE                   */
/*                        |	       	   	| 1 TRUE                    */
/*                        |	       	   	|                           */
/*      ramsize        	  |	int  	   	| size of DRAMS installed   */
/*                        |                     |                           */
/*      num_banks	  |	int  	   	| Number of banks installed */
/*                        |                	|                           */
/*      err_log_ptr    	  |	*int  		| pointer to the error log  */
/*			  |			| stucture 		    */
/*             		  |                   	|                           */
/*:  ---------------------------------------------------------------------- */
/*                                                                          */
/*-                                                                         */
/*                                                                          */
struct CMM { 
    long int cmm_init_flag;
    long int inprogress;
    long int present;                                                       
    long int chip_rev;
    long int powerup_state;
    long int config_state;
    long int ramsize;
    long int num_banks;
    struct CMM_ERR_LOG *err_log_ptr;
    } ;
struct CMM_SYN_TABLE { 
    long int bad_bits [2];
    long int syndrome;
    } ;
struct CMM_TEST {
    unsigned long int start_addr;
    unsigned long int fail_addr;
    long int incr;     
    long int bank_incr;     
    long int len;
    long int pass;       
    long int data_cmp_flag;       
    long int fail_brd;
    long int base_mod;
    };
struct INTLV {
    unsigned long int size;
    long int slot;
    unsigned long int base_addr;
    long int intlv_mode;
    long int intlv_unit;
    };
