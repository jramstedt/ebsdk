/********************************************************************************************************************************/
/* Created 17-JAN-1991 14:11:41 by VAX SDL V3.2-12     Source: 17-JAN-1991 14:09:48 FILY_USER:[DENISCO.COBRA.CP.SRC]SGEC_DEF.SDL; */
/********************************************************************************************************************************/
/* file:	sgec_def.sdl                                                */
/*                                                                          */
/* Copyright (C) 1990 by                                                    */
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
/* Abstract:	SGEC data structure definitions.                            */
/*                                                                          */
/* Author:	John DeNisco                                                */
/*                                                                          */
/* Modifications:                                                           */
/*                                                                          */
/*	phk	11-Nov-1991	Added nicsr6$m_pr  definition               */
/*                                                                          */
/*	jad	18-May-1990	Support EZ driver.                          */
/*                                                                          */
/*	jad	18-May-1990	Initial entry.                              */
/*                                                                          */
 
/*** MODULE $sgecdef ***/
/*General program constants                                                 */
#define XGEC$K_STOPPED 0                /*SGEC states                       */
#define XGEC$K_RUNNING 1
#define XGEC$K_SUSPENDED 2
#define XGEC$K_NORMAL_FRAME 0           /*SGEC frame types                  */
#define XGEC$K_SETUP_FRAME 2
#define XGEC$K_DIAG_FRAME 3
#define XGEC$K_NORMAL_MODE 0            /*SGEC operating modes              */
#define XGEC$K_INTERNAL_LOOPBACK 1
#define XGEC$K_EXTERNAL_LOOPBACK 2
#define XGEC$K_DIAG_MODE 3
#define XGEC$K_NORMAL_FILTER 0          /*SGEC address filters              */
#define XGEC$K_PROMISCUOUS 1
#define XGEC$K_MULTICAST 2
/*Register definitions                                                      */
#define nicsr0$m_sa 536870912
struct xgec_csr_setup {
    unsigned nicsr0$v_mbo : 2;          /*                                  */
    unsigned nicsr0$v_iv : 14;          /* Interrupt Vector                 */
    unsigned nicsr0$v_mbo1 : 13;        /*                                  */
    unsigned nicsr0$v_sa : 1;           /* Sync async                       */
    unsigned nicsr0$v_ip : 2;           /* Interrupt priority               */
    } ;
#define nicsr3$m_pd 1
struct xgec_csr_tpoll {
    unsigned nicsr3$v_pd : 1;           /* Poll Demand                      */
    unsigned nicsr3$v_mbo : 31;         /*                                  */
    } ;
#define nicsr2$m_pd 1
struct xgec_csr_rpoll {
    unsigned nicsr2$v_pd : 1;           /* Poll Demand                      */
    unsigned nicsr2$v_mbo : 31;         /*                                  */
    } ;
struct xgec_csr_rba {
    unsigned nicsr3$v_rba : 32;         /* Address of Receive list          */
    } ;
struct xgec_csr_tba {
    unsigned nicsr4$v_tba : 32;         /* Address of Transmit list         */
    } ;
#define nicsr5$m_is 1
#define nicsr5$m_ti 2
#define nicsr5$m_ri 4
#define nicsr5$m_ru 8
#define nicsr5$m_me 16
#define nicsr5$m_rw 32
#define nicsr5$m_tw 64
#define nicsr5$m_bo 128
#define nicsr5$m_dn 65536
#define nicsr5$m_rs 12582912
#define nicsr5$m_ts 50331648
#define nicsr5$m_tsus 33554432
#define nicsr5$m_sf 1073741824
#define nicsr5$m_id -2147483648
struct xgec_csr_status {
    unsigned nicsr5$v_is : 1;           /* Interrupt summary                */
    unsigned nicsr5$v_ti : 1;           /* Transmit interrupt               */
    unsigned nicsr5$v_ri : 1;           /* Recieve interrupt                */
    unsigned nicsr5$v_ru : 1;           /* Receive buffer unavailable       */
    unsigned nicsr5$v_me : 1;           /* Memory error                     */
    unsigned nicsr5$v_rw : 1;           /* Receive watchdog timeout         */
    unsigned nicsr5$v_tw : 1;           /* Transmit watchdog timeout        */
    unsigned nicsr5$v_bo : 1;           /* Boot message                     */
    unsigned nicsr5$v_mbo : 8;          /*                                  */
    unsigned nicsr5$v_dn : 1;           /* Done                             */
    unsigned nicsr5$v_som : 2;          /* Operating Mode                   */
    unsigned nicsr5$v_mbo1 : 3;         /*                                  */
    unsigned nicsr5$v_rs : 2;           /* Reception process state          */
    unsigned nicsr5$v_ts : 2;           /* Transmit process state           */
    unsigned nicsr5$v_ss : 4;           /* Self test status                 */
    unsigned nicsr5$v_sf : 1;           /* Self test fail                   */
    unsigned nicsr5$v_id : 1;           /* Initialization done              */
    } ;
#define nicsr6$m_pr 2
#define nicsr6$m_pb 8
#define nicsr6$m_fc 64
#define nicsr6$m_dc 128
#define nicsr6$m_mom 768
#define nicsr6$m_sr 1024
#define nicsr6$m_st 2048
#define nicsr6$m_se 524288
#define nicsr6$m_be 1048576
#define nicsr6$m_ie 1073741824
#define nicsr6$m_re -2147483648
struct xgec_csr_mode {
    unsigned nicsr6$v_r : 1;            /*                                  */
    unsigned nicsr6$v_af : 2;           /* Address filtering mode           */
    unsigned nicsr6$v_pb : 1;           /* Pass bad frames                  */
    unsigned nicsr6$v_r1 : 2;           /*                                  */
    unsigned nicsr6$v_fc : 1;           /* Force collision mode             */
    unsigned nicsr6$v_dc : 1;           /* Disable data chaining mode       */
    unsigned nicsr6$v_mom : 2;          /* Operating Mode                   */
    unsigned nicsr6$v_sr : 1;           /* Start reception                  */
    unsigned nicsr6$v_st : 1;           /* Start transmit                   */
    unsigned nicsr6$v_mbo : 4;          /*                                  */
    unsigned nicsr6$v_r2 : 3;           /*                                  */
    unsigned nicsr6$v_se : 1;           /* Single cycle enable              */
    unsigned nicsr6$v_be : 1;           /* Boot message enable              */
    unsigned nicsr6$v_mbo1 : 4;         /*                                  */
    unsigned nicsr6$v_bl : 4;           /* Burst limit mode                 */
    unsigned nicsr6$v_r3 : 1;           /*                                  */
    unsigned nicsr6$v_ie : 1;           /* Interrupt enable                 */
    unsigned nicsr6$v_re : 1;           /* Reset command                    */
    } ;
struct xgec_csr_sbr {
    unsigned nicsr7$v_sba : 32;         /* System base address              */
    } ;
struct xgec_csr_watchdog {
    unsigned nicsr9$v_tt : 16;          /* Transmit watchdog timeout        */
    unsigned nicsr9$v_rt : 16;          /* Receive watchdog timeout         */
    } ;
struct xgec_csr_ms_fr_cnt {
    unsigned nicsr10$v_mfc : 16;        /* Missed frame count               */
    unsigned nicsr10$v_rn : 4;          /* Revision number                  */
    unsigned nicsr10$v_mbz : 12;        /*                                  */
    } ;
struct xgec_csr_verlo {
    unsigned nicsr11$v_vrf : 32;        /*Boot message verification 31:00   */
    } ;
struct xgec_csr_verhi {
    unsigned nicsr12$v_vrf : 32;        /*Boot message verification 63:32   */
    } ;
struct xgec_csr_proc {
    unsigned nicsr13$v_prc : 8;         /*Boot message processor            */
    unsigned nicsr13$v_mbz : 26;        /*                                  */
    unsigned nicsr13$v_fill_0 : 6;
    } ;
struct xgec_csrs {
    U_INT_32 xgec$l_setup;	     	/* Vector address etc.              */
    U_INT_32 xgec$l_tpoll;     		/* Transmit polling demand          */
    U_INT_32 xgec$l_rpoll;     		/* Receive polling demand           */
    U_INT_32 xgec$l_rba;       		/* Start of Receive list            */
    U_INT_32 xgec$l_tba;       		/* Start of transmit list           */
    U_INT_32 xgec$l_status;    		/* Status                           */
    U_INT_32 xgec$l_mode;      		/* Mode                             */
    U_INT_32 xgec$l_sbr;       		/* System base register             */
    U_INT_32 xgec$l_rsvd;      		/* Reserved register                */
    U_INT_32 xgec$l_watch_dog; 		/* Watchdog timers                  */
    U_INT_32 xgec$l_ms_fr_cnt; 		/* Missed frame count               */
    U_INT_32 xgec$l_verlo;     		/* VRF <31:00>                      */
    U_INT_32 xgec$l_verhi;     		/* VRF <63:32>                      */
    U_INT_32 xgec$l_proc;      		/* Processor - PRC                  */
    U_INT_32 xgec$l_diag0;     		/* Diagnostic register 0            */
    U_INT_32 xgec$l_diag1;     		/* Diagnostic register 1            */
    } ;
/*Descriptor definitions                                                    */
#define rdes0$m_of 1
#define rdes0$m_ce 2
#define rdes0$m_db 4
#define rdes0$m_tn 8
#define rdes0$m_mbz 16
#define rdes0$m_ft 32
#define rdes0$m_cs 64
#define rdes0$m_tl 128
#define rdes0$m_ls 256
#define rdes0$m_fs 512
#define rdes0$m_bo 1024
#define rdes0$m_rf 2048
#define rdes0$m_le 16384
#define rdes0$m_es 32768
#define rdes0$m_ow -2147483648
struct xgec_rdes0 {
    unsigned rdes0$v_of : 1;            /* Overflow                         */
    unsigned rdes0$v_ce : 1;            /* CRC error                        */
    unsigned rdes0$v_db : 1;            /* Dribbling bits                   */
    unsigned rdes0$v_tn : 1;            /* Translation not valid            */
    unsigned rdes0$v_mbz : 1;           /*                                  */
    unsigned rdes0$v_ft : 1;            /* Frame type                       */
    unsigned rdes0$v_cs : 1;            /* Collision seen                   */
    unsigned rdes0$v_tl : 1;            /* Frame too long                   */
    unsigned rdes0$v_ls : 1;            /* Last segment                     */
    unsigned rdes0$v_fs : 1;            /* First segment                    */
    unsigned rdes0$v_bo : 1;            /* Buffer overflow                  */
    unsigned rdes0$v_rf : 1;            /* Runt frame                       */
    unsigned rdes0$v_dt : 2;            /* Data type                        */
    unsigned rdes0$v_le : 1;            /* Length error                     */
    unsigned rdes0$v_es : 1;            /* Error summary                    */
    unsigned rdes0$v_fl : 15;           /* Frame length                     */
    unsigned rdes0$v_ow : 1;            /* Own                              */
    } ;
#define rdes1$m_va 1073741824
#define rdes1$m_ca -2147483648
struct xgec_rdes1 {
    unsigned rdes1$v_u : 30;            /*                                  */
    unsigned rdes1$v_va : 1;            /* Virtual Addressing               */
    unsigned rdes1$v_ca : 1;            /* Chain address                    */
    } ;
struct xgec_rdes2 {
    unsigned rdes2$v_po : 9;            /* Page offset                      */
    unsigned rdes2$v_u : 7;             /*                                  */
    unsigned rdes2$v_bs : 15;           /* Buffer size                      */
    unsigned rdes2$v_u1 : 1;            /*                                  */
    } ;
struct xgec_rdes3 {
    unsigned rdes3$v_svpa : 32;         /* Buffer SVAPTE or physical address */
    } ;
struct xgec_rdes {
    U_INT_32 xgec$l_rdes0;     		/* RDES0                            */
    U_INT_32 xgec$l_rdes1;     		/* RDES1                            */
    U_INT_32 xgec$l_rdes2;     		/* RDES2                            */
    U_INT_32 xgec$l_rdes3;     		/* RDES3                            */
    } ;
#define tdes0$m_de 1
#define tdes0$m_uf 2
#define tdes0$m_tn 4
#define tdes0$m_hf 128
#define tdes0$m_ec 256
#define tdes0$m_lc 512
#define tdes0$m_nc 1024
#define tdes0$m_lo 2048
#define tdes0$m_le 4096
#define tdes0$m_mbz 8192
#define tdes0$m_to 16384
#define tdes0$m_es 32768
#define tdes0$m_ow -2147483648
struct xgec_tdes0 {
    unsigned tdes0$v_de : 1;            /* Deferred                         */
    unsigned tdes0$v_uf : 1;            /* Underflow error                  */
    unsigned tdes0$v_tn : 1;            /* Translation not valid            */
    unsigned tdes0$v_cc : 4;            /* Collision count                  */
    unsigned tdes0$v_hf : 1;            /* Heartbeat fail                   */
    unsigned tdes0$v_ec : 1;            /* Excessive collisions             */
    unsigned tdes0$v_lc : 1;            /* Late collisions                  */
    unsigned tdes0$v_nc : 1;            /* No carrier                       */
    unsigned tdes0$v_lo : 1;            /* Loss of carrier                  */
    unsigned tdes0$v_le : 1;            /* Length error                     */
    unsigned tdes0$v_mbz : 1;           /*                                  */
    unsigned tdes0$v_to : 1;            /* Transmit watchdog timeout        */
    unsigned tdes0$v_es : 1;            /* Error summary                    */
    unsigned tdes0$v_tdr : 15;          /* Time Domain Reflectometer        */
    unsigned tdes0$v_ow : 1;            /* Own                              */
    } ;
#define tdes1$m_ic 16777216
#define tdes1$m_ls 33554432
#define tdes1$m_fs 67108864
#define tdes1$m_ac 134217728
#define tdes1$m_va 1073741824
#define tdes1$m_ca -2147483648
struct xgec_tdes1 {
    unsigned tdes1$v_u : 24;            /*                                  */
    unsigned tdes1$v_ic : 1;            /* Interrupt on completion          */
    unsigned tdes1$v_ls : 1;            /* Last segment                     */
    unsigned tdes1$v_fs : 1;            /* First segment                    */
    unsigned tdes1$v_ac : 1;            /* Add CRC disable                  */
    unsigned tdes1$v_dt : 2;            /* Data type                        */
    unsigned tdes1$v_va : 1;            /* Virtual Addressing               */
    unsigned tdes1$v_ca : 1;            /* Chain address                    */
    } ;
struct xgec_tdes2 {
    unsigned tdes2$v_po : 9;            /* Page offset                      */
    unsigned tdes2$v_u : 7;             /*                                  */
    unsigned tdes2$v_bs : 15;           /* Buffer size                      */
    unsigned tdes2$v_u1 : 1;            /*                                  */
    } ;
struct xgec_tdes3 {
    unsigned tdes3$v_svpa : 32;         /* Buffer SVAPTE or physical address */
    } ;
struct xgec_tdes {
    U_INT_32 xgec$l_tdes0;	     	/* TDES0                            */
    U_INT_32 xgec$l_tdes1;     		/* TDES1                            */
    U_INT_32 xgec$l_tdes2;     		/* TDES2                            */
    U_INT_32 xgec$l_tdes3;     		/* TDES3                            */
    } ;
#define sdes0$m_se 8192
#define sdes0$m_es 32768
#define sdes0$m_ow -2147483648
struct xgec_sdes0 {
    unsigned sdes0$v_mbz : 13;          /*                                  */
    unsigned sdes0$v_se : 1;            /* Setup error                      */
    unsigned sdes0$v_mbz1 : 1;          /*                                  */
    unsigned sdes0$v_es : 1;            /* Error summary                    */
    unsigned sdes0$v_mbz2 : 15;         /*                                  */
    unsigned sdes0$v_ow : 1;            /* Own                              */
    } ;
#define sdes1$m_ic 16777216
#define sdes1$m_hp 33554432
#define sdes1$m_if 67108864
struct xgec_sdes1 {
    unsigned sdes1$v_u : 24;            /*                                  */
    unsigned sdes1$v_ic : 1;            /* Interrupt on completion          */
    unsigned sdes1$v_hp : 1;            /* Hash Perfect filtering mode      */
    unsigned sdes1$v_if : 1;            /* Inverse filtering                */
    unsigned sdes1$v_u1 : 1;            /*                                  */
    unsigned sdes1$v_dt : 2;            /* Data type                        */
    unsigned sdes1$v_u2 : 1;            /*                                  */
    unsigned sdes1$v_mbz : 1;           /*                                  */
    } ;
struct xgec_sdes2 {
    unsigned sdes2$v_u : 16;            /*                                  */
    unsigned sdes2$v_bs : 15;           /* Buffer size                      */
    unsigned sdes2$v_u1 : 1;            /*                                  */
    } ;
struct xgec_sdes3 {
    unsigned sdes3$v_svpa : 32;         /* Buffer SVAPTE or physical address */
    } ;
struct xgec_sdes {
    U_INT_32 xgec$l_sdes0;     		/* SDES0                            */
    U_INT_32 xgec$l_sdes1;     		/* SDES1                            */
    U_INT_32 xgec$l_sdes2;     		/* SDES2                            */
    U_INT_32 xgec$l_sdes3;     		/* SDES3                            */
    } ;
#define ddes0$m_ow -2147483648
struct xgec_ddes0 {
    unsigned ddes0$v_mbz : 31;          /*                                  */
    unsigned ddes0$v_ow : 1;            /* Own                              */
    } ;
#define ddes1$m_wd 8388608
struct xgec_ddes1 {
    unsigned ddes0$v_u  : 21;           /*                                  */
    unsigned ddes1$v_st : 2;            /*                                  */
    unsigned ddes1$v_wd : 1;            /* Read/write dump.                 */
    unsigned ddes0$v_u2 : 4;            /*                                  */
    unsigned ddes1$v_dt : 2;            /* Data type                        */
    unsigned ddes1$v_u3 : 1;            /*                                  */
    unsigned ddes1$v_mbz : 1;           /*                                  */
    } ;
struct xgec_ddes2 {
    unsigned ddes2$v_da : 14;           /* Dump address                     */
    unsigned ddes2$v_u : 2;             /*                                  */
    unsigned ddes2$v_ds : 15;           /* Buffer size                      */
    unsigned ddes2$v_u1 : 1;            /*                                  */
    } ;
struct xgec_ddes3 {
    unsigned ddes3$v_pa : 30;           /* Buffer SVAPTE or physical address */
    } ;
struct xgec_ddes {
    U_INT_32 xgec$l_ddes0;     		/* DDES0                            */
    U_INT_32 xgec$l_ddes1;     		/* DDES1                            */
    U_INT_32 xgec$l_ddes2;     		/* DDES2                            */
    U_INT_32 xgec$l_ddes3;     		/* DDES3                            */
    } ;
