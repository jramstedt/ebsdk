/*
 * Copyright (C) 1990 by
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
 * Abstract:	SHAC Definitions
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *	sfs	27-Jun-1990	Initial entry.
 */
 
volatile struct shac_csr {
    long int fill1 [12];
    unsigned long int sswcr;
    long int fill2 [4];
    unsigned long int sshma;
    unsigned long int pqbbr;
    unsigned long int psr;
    unsigned long int pesr;
    unsigned long int pfar;
    unsigned long int ppr;
    unsigned long int pmcsr;
    long int fill3 [8];
    unsigned long int pcq0cr;
    unsigned long int pcq1cr;
    unsigned long int pcq2cr;
    unsigned long int pcq3cr;
    unsigned long int pdfqcr;
    unsigned long int pmfqcr;
    unsigned long int psrcr;
    unsigned long int pecr;
    unsigned long int pdcr;
    unsigned long int picr;
    unsigned long int pmtcr;
    unsigned long int pmtecr;
    long int fill4 [84];
    } ;

#define shac_psr_rqa 1
#define shac_psr_mfqe 2
#define shac_psr_pdc 4
#define shac_psr_pic 8
#define shac_psr_dse 16
#define shac_psr_mse 32
#define shac_psr_mte 64
#define shac_psr_misc 128
#define shac_psr_shme 65536
#define shac_psr_smpe 131072
#define shac_psr_isn 262144
#define shac_psr_de 524288
#define shac_psr_qde 1048576
#define shac_psr_ii 2097152
#define shac_psr_me -2147483648

volatile struct shac_csr_psr {
    unsigned psr_rqa : 1;
    unsigned psr_mfqe : 1;
    unsigned psr_pdc : 1;
    unsigned psr_pic : 1;
    unsigned psr_dse : 1;
    unsigned psr_mse : 1;
    unsigned psr_mte : 1;
    unsigned psr_misc : 1;
    unsigned psr_fill1 : 8;
    unsigned psr_shme : 1;
    unsigned psr_smpe : 1;
    unsigned psr_isn : 1;
    unsigned psr_de : 1;
    unsigned psr_qde : 1;
    unsigned psr_ii : 1;
    unsigned psr_fill2 : 9;
    unsigned psr_me : 1;
    } ;

#define shac_ppr_port 255
#define shac_ppr_isdi 32512
#define shac_ppr_ibuf_len 536805376
#define shac_ppr_csz -536870912

volatile struct shac_csr_ppr {
    unsigned ppr_port : 8;
    unsigned ppr_isdi : 7;
    unsigned ppr_fill1 : 1;
    unsigned ppr_ibuf_len : 13;
    unsigned ppr_csz : 3;
    } ;

#define shac_pmcsr_min 1
#define shac_pmcsr_mtd 2
#define shac_pmcsr_ie 4
#define shac_pmcsr_simp 8
#define shac_pmcsr_hac 16

volatile struct shac_csr_pmcsr {
    unsigned pmcsr_min : 1;
    unsigned pmcsr_mtd : 1;
    unsigned pmcsr_ie : 1;
    unsigned pmcsr_simp : 1;
    unsigned pmcsr_hac : 1;
    unsigned pmcsr_fill1 : 27;
    } ;

volatile struct sshma {
    long int fill1 [8];
    unsigned long int params_offset;
    unsigned short int num_patchs;
    short int fill2;
    unsigned long int patchs_offset;
    unsigned short int num_extern;
    short int fill3;
    unsigned long int extern_offset;
    unsigned long int checksum;
    } ;

volatile struct sshma_params {
    unsigned long int revision;
    unsigned short int vector;
    unsigned char burst;
    unsigned octa_enable : 1;
    unsigned fill1 : 5;
    unsigned sync_enable : 1;
    unsigned fill2 : 1;
    unsigned char node;
    unsigned fill3 : 2;
    unsigned sx20 : 1;
    unsigned fill4 : 5;
    unsigned char ipl;
    unsigned char fill5;
    unsigned char rtc_tmo;
    unsigned char fill6;
    unsigned char ini_tmo;
    unsigned char fill7;
    unsigned char tgt_tmo;
    unsigned char fill8;
    unsigned char sel_tmo;
    unsigned char fill9;
    unsigned short int delay_retries;
    unsigned short int immed_retries;
    unsigned short int idle_count;
    unsigned short int coin_flips;
    unsigned long int seed;
    unsigned long int diag_flags;
    unsigned long int checksum;
    } ;

volatile struct pqb {
    struct QUEUE cmdq0;
    struct QUEUE cmdq1;
    struct QUEUE cmdq2;
    struct QUEUE cmdq3;
    struct QUEUE rspq;
    struct QUEUE *dfq_head;
    struct QUEUE *mfq_head;
    unsigned long int dqe_size;
    unsigned long int mqe_size;
    unsigned long int pqb_base;
    unsigned long int bdt_base;
    unsigned long int bdt_size;
    unsigned long int spt_base;
    unsigned long int spt_size;
    unsigned long int gpt_base;
    unsigned long int gpt_size;
    long int fill1 [107];
    } ;

volatile struct bdt {
    unsigned byte_offset : 9;
    unsigned fill1 : 6;
    unsigned valid : 1;
    unsigned short int key;
    unsigned long int byte_count;
    unsigned long int svapte;
    long int fill2;
    } ;

volatile struct pdt {
    struct pqb pqb;
    struct bdt bdt [32];
    struct QUEUE dfq;
    struct QUEUE mfq;
    } ;
