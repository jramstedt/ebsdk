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
 * Abstract:	NCR53C810 Definitions
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *	sfs	14-Apr-1993	Initial entry.
 */
 
#define n810_k_dat_out 0
#define n810_k_dat_in 1
#define n810_k_cmd 2
#define n810_k_sts 3
#define n810_k_msg_out 6
#define n810_k_msg_in 7

#define n810_k_ok 0
#define n810_k_scsi 1
#define n810_k_dssi 2
#define n810_k_selected 3
#define n810_k_reselected 4
#define n810_k_timeout 5
#define n810_k_busy 6
#define n810_k_got_msg_in 7
#define n810_k_dat_out_done 8
#define n810_k_dat_in_done 9

volatile struct n810_csr {
    unsigned char scntl0;
    unsigned char scntl1;
    unsigned char scntl2;
    unsigned char scntl3;
    unsigned char scid;
    unsigned char sxfer;
    unsigned char sdid;
    unsigned char gpreg;
    unsigned char sfbr;
    unsigned char socl;
    unsigned char ssid;
    unsigned char sbcl;
    unsigned char dstat;
    unsigned char sstat0;
    unsigned char sstat1;
    unsigned char sstat2;
    unsigned long int dsa;
    unsigned char istat;
    unsigned char reserved14;
    unsigned char reserved15;
    unsigned char reserved16;
    unsigned char ctest0;
    unsigned char ctest1;
    unsigned char ctest2;
    unsigned char ctest3;
    unsigned long int temp;
    unsigned char dfifo;
    unsigned char ctest4;
    unsigned char ctest5;
    unsigned char ctest6;
    unsigned long int dcmd_dbc;
    unsigned long int dnad;
    unsigned long int dsp;
    unsigned long int dsps;
    unsigned long int scratcha;
    unsigned char dmode;
    unsigned char dien;
    unsigned char dwt;
    unsigned char dcntl;
    unsigned long int adder;
    unsigned char sien0;
    unsigned char sien1;
    unsigned char sist0;
    unsigned char sist1;
    unsigned char lcrc;
    unsigned char reserved45;
    unsigned char macntl;
    unsigned char gpcntl;
    unsigned char stime0;
    unsigned char stime1;
    unsigned char respid;
    unsigned char reserved4b;
    unsigned char stest0;
    unsigned char stest1;
    unsigned char stest2;
    unsigned char stest3;
    unsigned char sidl;
    unsigned char reserved51;
    unsigned char reserved52;
    unsigned char reserved53;
    unsigned char sodl;
    unsigned char reserved55;
    unsigned char reserved56;
    unsigned char reserved57;
    unsigned char sbdl;
    unsigned char reserved59;
    unsigned char reserved5a;
    unsigned char reserved5b;
    unsigned long int scratchb;
    } ;

#define n810_scntl0_tgt 1
#define n810_scntl0_aap 2
#define n810_scntl0_epc 8
#define n810_scntl0_watn 16
#define n810_scntl0_start 32
#define n810_scntl0_arb0 64
#define n810_scntl0_arb1 128

volatile struct n810_csr_scntl0 {
    unsigned scntl0_trg : 1;
    unsigned scntl0_aap : 1;
    unsigned scntl0_fill1 : 1;
    unsigned scntl0_epc : 1;
    unsigned scntl0_watn : 1;
    unsigned scntl0_start : 1;
    unsigned scntl0_arb0 : 1;
    unsigned scntl0_arb1 : 1;
    } ;

#define n810_scntl1_sst 1
#define n810_scntl1_iarb 2
#define n810_scntl1_aesp 4
#define n810_scntl1_rst 8
#define n810_scntl1_con 16
#define n810_scntl1_dhp 32
#define n810_scntl1_adb 64
#define n810_scntl1_exc 128

volatile struct n810_csr_scntl1 {
    unsigned scntl1_sst : 1;
    unsigned scntl1_iarb : 1;
    unsigned scntl1_aesp : 1;
    unsigned scntl1_rst : 1;
    unsigned scntl1_con : 1;
    unsigned scntl1_dhp : 1;
    unsigned scntl1_adb : 1;
    unsigned scntl1_exc : 1;
    } ;

#define n810_scntl2_sdu 128

volatile struct n810_csr_scntl2 {
    unsigned scntl2_fill1 : 7;
    unsigned scntl2_sdu : 1;
    } ;

#define n810_scntl3_ccf 7
#define n810_scntl3_scf 112

volatile struct n810_csr_scntl3 {
    unsigned scntl3_ccf : 3;
    unsigned scntl3_fill1 : 1;
    unsigned scntl3_scf : 3;
    unsigned scntl1_fill2 : 1;
    } ;

#define n810_scid_enc 7
#define n810_scid_sre 32
#define n810_scid_rre 64

volatile struct n810_csr_scid {
    unsigned scid_enc : 3;
    unsigned scid_fill1 : 2;
    unsigned scid_sre : 1;
    unsigned scid_rre : 1;
    unsigned scid_fill2 : 1;
    } ;

#define n810_sxfer_mo 15
#define n810_sxfer_tp 224

volatile struct n810_csr_sxfer {
    unsigned sxfer_mo : 4;
    unsigned sxfer_fill1 : 1;
    unsigned sxfer_tp : 3;
    } ;

#define n810_sdid_enc 7

volatile struct n810_csr_sdid {
    unsigned sdid_enc : 3;
    unsigned sdid_fill1 : 5;
    } ;

#define n810_gpreg_gpio0 1
#define n810_gpreg_gpio1 2

volatile struct n810_csr_gpreg {
    unsigned gpreg_gpio0 : 1;
    unsigned gpreg_gpio1 : 1;
    unsigned gpreg_fill1 : 6;
    } ;

#define n810_socl_phase 7
#define n810_socl_atn 8
#define n810_socl_sel 16
#define n810_socl_bsy 32
#define n810_socl_ack 64
#define n810_socl_req 128

volatile struct n810_csr_socl {
    unsigned socl_phase : 3;
    unsigned socl_atn : 1;
    unsigned socl_sel : 1;
    unsigned socl_bsy : 1;
    unsigned socl_ack : 1;
    unsigned socl_req : 1;
    } ;

#define n810_ssid_enc 7
#define n810_ssid_val 128

volatile struct n810_csr_ssid {
    unsigned ssid_enc : 3;
    unsigned ssid_fill1 : 4;
    unsigned ssid_val : 1;
    } ;

#define n810_sbcl_phase 7
#define n810_sbcl_atn 8
#define n810_sbcl_sel 16
#define n810_sbcl_bsy 32
#define n810_sbcl_ack 64
#define n810_sbcl_req 128

volatile struct n810_csr_sbcl {
    unsigned sbcl_phase : 3;
    unsigned sbcl_atn : 1;
    unsigned sbcl_sel : 1;
    unsigned sbcl_bsy : 1;
    unsigned sbcl_ack : 1;
    unsigned sbcl_req : 1;
    } ;

#define n810_dstat_iid 1
#define n810_dstat_sir 4
#define n810_dstat_ssi 8
#define n810_dstat_abrt 16
#define n810_dstat_bf 32
#define n810_dstat_mdpe 64
#define n810_dstat_dfe 128

struct n810_csr_dstat {
    unsigned dstat_iid : 1;
    unsigned dstat_fill1 : 1;
    unsigned dstat_sir : 1;
    unsigned dstat_ssi : 1;
    unsigned dstat_abrt : 1;
    unsigned dstat_bf : 1;
    unsigned dstat_mdpe : 1;
    unsigned dstat_dfe : 1;
    } ;

#define n810_sstat0_sdp 1
#define n810_sstat0_rst 2
#define n810_sstat0_woa 4
#define n810_sstat0_loa 8
#define n810_sstat0_aip 16
#define n810_sstat0_olf 32
#define n810_sstat0_orf 64
#define n810_sstat0_ilf 128

struct n810_csr_sstat0 {
    unsigned sstat0_sdp : 1;
    unsigned sstat0_rst : 1;
    unsigned sstat0_woa : 1;
    unsigned sstat0_loa : 1;
    unsigned sstat0_aip : 1;
    unsigned sstat0_olf : 1;
    unsigned sstat0_orf : 1;
    unsigned sstat0_ilf : 1;
    } ;

#define n810_sstat1_io 1
#define n810_sstat1_cd 2
#define n810_sstat1_msg 4
#define n810_sstat1_sdp 8
#define n810_sstat1_ff 240

struct n810_csr_sstat1 {
    unsigned sstat1_io : 1;
    unsigned sstat1_cd : 1;
    unsigned sstat1_msg : 1;
    unsigned sstat1_sdp : 1;
    unsigned sstat1_ff : 4;
    } ;

#define n810_sstat2_ldsc 2

struct n810_csr_sstat2 {
    unsigned sstat2_fill1 : 1;
    unsigned sstat2_ldsc : 1;
    unsigned sstat2_fill2 : 6;
    } ;

#define n810_istat_dip 1
#define n810_istat_sip 2
#define n810_istat_intf 4
#define n810_istat_con 8
#define n810_istat_sem 16
#define n810_istat_sigp 32
#define n810_istat_rst 64
#define n810_istat_abrt 128

volatile struct n810_csr_istat {
    unsigned istat_dip : 1;
    unsigned istat_sip : 1;
    unsigned istat_intf : 1;
    unsigned istat_con : 1;
    unsigned istat_sem : 1;
    unsigned istat_sigp : 1;
    unsigned istat_rst : 1;
    unsigned istat_abrt : 1;
    } ;

#define n810_ctest1_ffl 15
#define n810_ctest1_fmt 240

volatile struct n810_csr_ctest1 {
    unsigned ctest1_ffl : 4;
    unsigned ctest1_fmt : 4;
    } ;

#define n810_ctest2_dack 1
#define n810_ctest2_dreq 2
#define n810_ctest2_teop 4
#define n810_ctest2_cm 16
#define n810_ctest2_cio 32
#define n810_ctest2_sigp 64
#define n810_ctest2_ddir 128

volatile struct n810_csr_ctest2 {
    unsigned ctest2_dack : 1;
    unsigned ctest2_dreq : 1;
    unsigned ctest2_teop : 1;
    unsigned ctest2_fill1 : 1;
    unsigned ctest2_cm : 1;
    unsigned ctest2_cio : 1;
    unsigned ctest2_sigp : 1;
    unsigned ctest2_ddir : 1;
    } ;

#define n810_ctest3_fm 2
#define n810_ctest3_clf 4
#define n810_ctest3_flf 8
#define n810_ctest3_v 240

volatile struct n810_csr_ctest3 {
    unsigned ctest3_fill1 : 1;
    unsigned ctest3_fm : 1;
    unsigned ctest3_clf : 1;
    unsigned ctest3_flf : 1;
    unsigned ctest3_v : 4;
    } ;

#define n810_dfifo_bo 127

volatile struct n810_csr_dfifo {
    unsigned dfifo_bo : 7;
    unsigned dfifo_fill1 : 1;
    } ;

#define n810_ctest4_fbl 7
#define n810_ctest4_mpee 8
#define n810_ctest4_srtm 16
#define n810_ctest4_zsd 32
#define n810_ctest4_zmod 64
#define n810_ctest4_bdis 128

volatile struct n810_csr_ctest4 {
    unsigned ctest4_fbl : 3;
    unsigned ctest4_mpee : 1;
    unsigned ctest4_srtm : 1;
    unsigned ctest4_zsd : 1;
    unsigned ctest4_zmod : 1;
    unsigned ctest4_mux : 1;
    } ;

#define n810_ctest5_ddir 8
#define n810_ctest5_masr 16
#define n810_ctest5_bbck 64
#define n810_ctest5_adck 128

volatile struct n810_csr_ctest5 {
    unsigned ctest5_fill1 : 3;
    unsigned ctest5_ddir : 1;
    unsigned ctest5_masr : 1;
    unsigned ctest5_fill2 : 1;
    unsigned ctest5_bbck : 1;
    unsigned ctest5_adck : 1;
    } ;

#define n810_dbc_byte_count 16777215
#define n810_dcmd_phase 117440512
#define n810_dcmd_opcode 134217728
#define n810_dcmd_tab 268435456
#define n810_dcmd_ind 536870912
#define n810_dcmd_type -1073741824

volatile struct n810_dcmd_dbc {
    unsigned dbc_byte_count : 24;
    unsigned dcmd_phase : 3;
    unsigned dcmd_opcode : 1;
    unsigned dcmd_tab : 1;
    unsigned dcmd_ind : 1;
    unsigned dcmd_type : 2;
    } ;

#define n810_dmode_man 1
#define n810_dmode_er 8
#define n810_dmode_diom 16
#define n810_dmode_siom 32
#define n810_dmode_bl0 64
#define n810_dmode_bl1 128

volatile struct n810_csr_dmode {
    unsigned dmode_man : 1;
    unsigned dmode_fill1 : 2;
    unsigned dmode_er : 1;
    unsigned dmode_diom : 1;
    unsigned dmode_siom : 1;
    unsigned dmode_bl0 : 1;
    unsigned dmode_bl1 : 1;
    } ;

#define n810_dien_iid 1
#define n810_dien_sir 4
#define n810_dien_ssi 8
#define n810_dien_abrt 16
#define n810_dien_bf 32
#define n810_dien_mdpe 64

volatile struct n810_csr_dien {
    unsigned dien_iid : 1;
    unsigned dien_fill1 : 1;
    unsigned dien_sir : 1;
    unsigned dien_ssi : 1;
    unsigned dien_abrt : 1;
    unsigned dien_bf : 1;
    unsigned dien_mdpe : 1;
    unsigned dien_fill2 : 1;
    } ;

#define n810_dcntl_com 1
#define n810_dcntl_sa 2
#define n810_dcntl_std 4
#define n810_dcntl_irqm 8
#define n810_dcntl_ssm 16

volatile struct n810_csr_dcntl {
    unsigned dcntl_com : 1;
    unsigned dcntl_sa : 1;
    unsigned dcntl_std : 1;
    unsigned dcntl_irqm : 1;
    unsigned dcntl_ssm : 1;
    unsigned dcntl_fill1 : 3;
    } ;

#define n810_sien0_par 1
#define n810_sien0_rst 2
#define n810_sien0_udc 4
#define n810_sien0_sge 8
#define n810_sien0_rsl 16
#define n810_sien0_sel 32
#define n810_sien0_fc 64
#define n810_sien0_ma 128

volatile struct n810_csr_sien0 {
    unsigned sien0_par : 1;
    unsigned sien0_rst : 1;
    unsigned sien0_udc : 1;
    unsigned sien0_sge : 1;
    unsigned sien0_rsl : 1;
    unsigned sien0_sel : 1;
    unsigned sien0_fc : 1;
    unsigned sien0_ma : 1;
    } ;

#define n810_sien1_hth 1
#define n810_sien1_gen 2
#define n810_sien1_sto 4

volatile struct n810_csr_sien1 {
    unsigned sien1_hth : 1;
    unsigned sien1_gen : 1;
    unsigned sien1_sto : 1;
    unsigned sien1_fill1 : 5;
    } ;

#define n810_sist0_par 1
#define n810_sist0_rst 2
#define n810_sist0_udc 4
#define n810_sist0_sge 8
#define n810_sist0_rsl 16
#define n810_sist0_sel 32
#define n810_sist0_fc 64
#define n810_sist0_ma 128

volatile struct n810_csr_sist0 {
    unsigned sist0_par : 1;
    unsigned sist0_rst : 1;
    unsigned sist0_udc : 1;
    unsigned sist0_sge : 1;
    unsigned sist0_rsl : 1;
    unsigned sist0_sel : 1;
    unsigned sist0_fc : 1;
    unsigned sist0_ma : 1;
    } ;

#define n810_sist1_hth 1
#define n810_sist1_gen 2
#define n810_sist1_sto 4

volatile struct n810_csr_sist1 {
    unsigned sist1_hth : 1;
    unsigned sist1_gen : 1;
    unsigned sist1_sto : 1;
    unsigned sist1_fill1 : 5;
    } ;

#define n810_macntl_scpts 1
#define n810_macntl_pscpt 2
#define n810_macntl_drd 4
#define n810_macntl_dwr 8

volatile struct n810_csr_macntl {
    unsigned macntl_scpts : 1;
    unsigned macntl_pscpt : 1;
    unsigned macntl_drd : 1;
    unsigned macntl_drw : 1;
    unsigned macntl_fill1 : 4;
    } ;

#define n810_gpcntl_gpio0 1
#define n810_gpcntl_gpio1 2
#define n810_gpcntl_fe 64
#define n810_gpcntl_me 128

volatile struct n810_csr_gpcntl {
    unsigned gpcntl_gpio0 : 1;
    unsigned gpcntl_gpio1 : 1;
    unsigned gpcntl_fill1 : 4;
    unsigned gpcntl_fe : 1;
    unsigned gpcntl_me : 1;
    } ;

#define n810_stime0_sel 15
#define n810_stime0_hth 240

volatile struct n810_csr_stime0 {
    unsigned stime0_sel : 4;
    unsigned stime0_hth : 4;
    } ;

#define n810_stime1_gen 15

volatile struct n810_csr_stime1 {
    unsigned stime1_gen : 4;
    unsigned stime1_fill1 : 4;
    } ;

#define n810_stest0_som 1
#define n810_stest0_soz 2
#define n810_stest0_art 4
#define n810_stest0_slt 8

volatile struct n810_csr_stest0 {
    unsigned stest0_som : 1;
    unsigned stest0_soz : 1;
    unsigned stest0_art : 1;
    unsigned stest0_slt : 1;
    unsigned stest0_fill1 : 4;
    } ;

#define n810_stest1_sclk 128

volatile struct n810_csr_stest1 {
    unsigned stest1_fill1 : 7;
    unsigned stest1_sclk : 1;
    } ;

#define n810_stest2_llm 1
#define n810_stest2_erf 2
#define n810_stest2_szm 8
#define n810_stest2_slbe 16
#define n810_stest2_dif 32
#define n810_stest2_rof 64
#define n810_stest2_sce 128

volatile struct n810_csr_stest2 {
    unsigned stest2_llm : 1;
    unsigned stest2_erf : 1;
    unsigned stest2_fill1 : 1;
    unsigned stest2_szm : 1;
    unsigned stest2_slbe : 1;
    unsigned stest2_dif : 1;
    unsigned stest2_rof : 1;
    unsigned stest2_sce : 1;
    } ;

#define n810_stest3_stw 1
#define n810_stest3_csf 2
#define n810_stest3_ttm 4
#define n810_stest3_dsi 16
#define n810_stest3_hsc 32
#define n810_stest3_str 64
#define n810_stest3_ean 128

volatile struct n810_csr_stest3 {
    unsigned stest3_stw : 1;
    unsigned stest3_csf : 1;
    unsigned stest3_ttm : 1;
    unsigned stest3_fill1 : 1;
    unsigned stest3_dsi : 1;
    unsigned stest3_hsc : 1;
    unsigned stest3_str : 1;
    unsigned stest3_ean : 1;
    } ;

#define n810_bm_byte_count 16777215
#define n810_bm_phase 117440512
#define n810_k_move 0
#define n810_k_wait_move 1
#define n810_bm_opcode 134217728
#define n810_bm_tab 268435456
#define n810_bm_ind 536870912
#define n810_k_bm 0
#define n810_bm_type -1073741824

volatile struct n810_bm {
    unsigned bm_byte_count : 24;
    unsigned bm_phase : 3;
    unsigned bm_opcode : 1;
    unsigned bm_tab : 1;
    unsigned bm_ind : 1;
    unsigned bm_type : 2;
    unsigned long int bm_addr;
    } ;

#define n810_io_atn 8
#define n810_io_ack 64
#define n810_io_tgt 512
#define n810_io_id 983040
#define n810_io_watn 16777216
#define n810_io_tab 33554432
#define n810_io_rel 68108864
#define n810_k_resel 0
#define n810_k_sel 0
#define n810_k_disc 1
#define n810_k_wait_disc 1
#define n810_k_wait_sel 2
#define n810_k_wait_resel 2
#define n810_k_set 3
#define n810_k_clear 4
#define n810_io_opcode 939524096
#define n810_k_io 1
#define n810_io_type -1073741824

volatile struct n810_io {
    unsigned io_fill1 : 3;
    unsigned io_atn : 1;
    unsigned io_fill2 : 2;
    unsigned io_ack : 1;
    unsigned io_fill3 : 2;
    unsigned io_tgt : 1;
    unsigned io_fill4 : 6;
    unsigned io_id : 4;
    unsigned io_fill5 : 4;
    unsigned io_watn : 1;
    unsigned io_tab : 1;
    unsigned io_rel : 1;
    unsigned io_opcode : 3;
    unsigned io_type : 2;
    unsigned long int io_addr;
    } ;

#define n810_rw_data 65280
#define n810_rw_reg 16711680
#define n810_k_copy 0
#define n810_k_or 2
#define n810_k_and 4
#define n810_k_add 6
#define n810_rw_operation 117440512
#define n810_k_write 5
#define n810_k_read 6
#define n810_k_modify 7
#define n810_rw_opcode 939524096
#define n810_k_rw 1
#define n810_rw_type -1073741824

volatile struct n810_rw {
    unsigned rw_fill1 : 8;
    unsigned rw_data : 8;
    unsigned rw_reg : 8;
    unsigned rw_operation : 3;
    unsigned rw_opcode : 3;
    unsigned rw_type : 2;
    long int rw_fill2;
    } ;

#define n810_tc_data 255
#define n810_tc_mask 65280
#define n810_tc_wait 65536
#define n810_tc_cmp_phase 131072
#define n810_tc_cmp_data 262144
#define n810_tc_jmp_true 524288
#define n810_tc_rel 8388608
#define n810_tc_phase 117440512
#define n810_k_jmp 0
#define n810_k_call 1
#define n810_k_ret 2
#define n810_k_int 3
#define n810_tc_opcode 939524096
#define n810_k_tc 2
#define n810_tc_type -1073741824

volatile struct n810_tc {
    unsigned tc_data : 8;
    unsigned tc_mask : 8;
    unsigned tc_wait : 1;
    unsigned tc_cmp_phase : 1;
    unsigned tc_cmp_data : 1;
    unsigned tc_jmp_true : 1;
    unsigned tc_fill1 : 3;
    unsigned tc_rel : 1;
    unsigned tc_phase : 3;
    unsigned tc_opcode : 3;
    unsigned tc_type : 2;
    unsigned long int tc_addr;
    } ;

#define n810_mm_byte_count 16777215
#define n810_k_mm 3
#define n810_mm_type -1073741824

volatile struct n810_mm {
    unsigned mm_byte_count : 24;
    unsigned mm_fill1 : 6;
    unsigned mm_type : 2;
    unsigned long int mm_src;
    unsigned long int mm_dst;
    } ;

struct n810_speed {
    int	scf;
    int sxfer;
    int stp;
    int speed;
    } ;
