/*****************************************************************************

       Copyright © 1993, 1994 Digital Equipment Corporation,
                       Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, provided  
that the copyright notice and this permission notice appear in all copies  
of software and supporting documentation, and that the name of Digital not  
be used in advertising or publicity pertaining to distribution of the software 
without specific, written prior permission. Digital grants this permission 
provided that you prominently mark, as not part of the original, any 
modifications made to this software or documentation.

Digital Equipment Corporation disclaims all warranties and/or guarantees  
with regard to this software, including all implied warranties of fitness for 
a particular purpose and merchantability, and makes no representations 
regarding the use of, or the results of the use of, the software and 
documentation in terms of correctness, accuracy, reliability, currentness or
otherwise; and you rely on the software, documentation and results solely at 
your own risk. 

******************************************************************************/
#ifndef LINT
static char *rcsid = "$Id: ev6_chain.c,v 1.2 1999/01/21 19:06:25 gries Exp $";
#endif

/*
 * Author: Franklin Hooker, Digital Equipment Corporation, July 1997
 *
 * $Log: ev6_chain.c,v $
 * Revision 1.2  1999/01/21 19:06:25  gries
 * First Release to cvs gries
 *
 * Revision 1.4  1998/08/10  15:05:46  gries
 * closed test "}"
 *
 * Revision 1.3  1998/08/10  14:14:28  gries
 * Added write_one field
 *
 * Revision 1.2  1997/12/22  17:17:54  gries
 * Quinn's changes from pass 2-3
 *
 * Revision 1.1  1997/11/12  18:51:39  pbell
 * Initial revision
 *
 * Revision 1.4  1997/07/09  04:19:55  fdh
 * Removed skewed_fill_mode_a_h from table of CSRs.
 * This is now an alias to bc_clk_ratio_a<1> instead.
 *
 * Revision 1.3  1997/07/08  15:24:53  fdh
 * Stringized the CSR macro.
 *
 * Revision 1.2  1997/07/07  14:56:47  fdh
 * Comments...
 *
 * Revision 1.1  1997/07/07  00:50:05  fdh
 * Initial revision
 *
 */

#include <stdio.h>
#include "ic4mat.h"

#define DUMP_CSR(CSR)   \
  csr_p = lookup_csr(#CSR);			\
  dump_csr(file, csr_p);

#define WRITE_ONE(Start, Test, End, Operation, CSR)	\
  csr_p = lookup_csr(#CSR);				\
  for( i=Start  ; i Test End  ; i Operation, j++) {	\
    write_one(csr_p, write_once_a, reverse, i, j);	\
  }							\
  dump_csr(stdout, csr_p); fprintf(stdout, "\n");
  
#define WRITE_DUP(Start, Test, End, Operation, CSR)	\
  csr_p = lookup_csr(#CSR);				\
  for( i=Start  ; i Test End  ; i Operation, j++) {	\
    write_dup(csr_p, write_once_a, reverse, i, j);	\
  }							\
  dump_csr(stdout, csr_p); fprintf(stdout, "\n");


static void dump_csr(FILE *file, csr_t *csr_p);
static void write_one(csr_t *csr_p, bit_t *write_once_a, int reverse, int i, int j);
static void write_dup(csr_t *csr_p, bit_t *write_once_a, int reverse, int i, int j);

static void dump_csr(FILE *file, csr_t *csr_p)
{
  fprintf(file, "%-50s = 0x%x\n", csr_p->name, csr_p->value);
}

static void write_one(csr_t *csr_p, bit_t *write_once_a, int reverse, int i, int j)
{
  if (reverse) {
    printf("%s(%d) = write_once_a[%d] = %x\n",
	   csr_p->name, i, j, write_once_a[j]);
    csr_p->value |= write_once_a[j] << i;
  }
  else {
    write_once_a[j] = (bit_t) (csr_p->value >> i) & 1;
    printf("write_once_a[%d] = %s(%d) = %x\n",
	   j, csr_p->name, i, write_once_a[j]);
  }
}

static void write_dup(csr_t *csr_p, bit_t *write_once_a, int reverse, int i, int j)
{
  if (reverse) {
    printf("%s(%d) = write_once_a[%d]\n", csr_p->name, i, j);
    if ((bit_t)((csr_p->value >> i) & 1) != write_once_a[j]) {
      csr_p->value |= write_once_a[j] << i;
      printf("Duplicate error for %s[%d]\n", csr_p->name, i);
      ++errors;
    }
    printf("%s(%d) = write_once_a[%d] = %x\n",
	   csr_p->name, i, j, write_once_a[j]);
  }
  else {
    write_once_a[j] = (bit_t) (csr_p->value >> i) & 1;
    printf("write_once_a[%d] = %s(%d) = %x\n",
	   j, csr_p->name, i, write_once_a[j]);
  }
}

int setup_chain_data(bit_t *write_once_a, int reverse, int leader)
{
  int i;
  int j;
  csr_t *csr_p;

  /*	CBOX WRITE ONCE chain	*/
  /*	Begins with LSB	*/
  j = 0;
  /*	C_IOA_VLD	*/
  WRITE_ONE(0, <, 1, ++, tlaser_stio_mode_a);
  WRITE_ONE(1, <, 2, ++, bc_clk_ratio_a);
  /*	C_VAF_VIC	*/
  WRITE_DUP(1, <, 2, ++, bc_clk_ratio_a);
  WRITE_ONE(0, <, 8, ++, dcvic_threshold_a);
  /*	C_ADC_BCT	*/
  WRITE_ONE(0, <, 1, ++, bc_clean_victim_a);
  /*	C_ADC_SMX	*/
  WRITE_ONE(0, <, 2, ++, sys_bus_size_a);
  WRITE_ONE(0, <, 1, ++, sysbus_format_a);
  WRITE_ONE(1, <, 5, ++, sys_clk_ratio_a);
  /*	C_PRQ_ACT	*/
  WRITE_ONE(0, <, 1, ++, dup_tag_enable_a);
  WRITE_ONE(0, <, 1, ++, prb_tag_only_a);
  /*	C_SYS_ARO	*/
  WRITE_ONE(0, <, 1, ++, fast_mode_disable_a);
  WRITE_ONE(0, <, 1, ++, bc_rdvictim_a);
  WRITE_DUP(0, <, 1, ++, bc_clean_victim_a);
  WRITE_ONE(0, <, 1, ++, rdvic_ack_inhibit_a);
  WRITE_ONE(0, <, 1, ++, sysbus_mb_enable_a);
  /*	C_SYS_CNT	*/
  WRITE_ONE(4, >=, 0, --, sysbus_ack_limit_a);
  WRITE_ONE(2, >=, 0, --, sysbus_vic_limit_a);
  /*	C_ADC_TAG	*/
  WRITE_DUP(0, <, 1, ++, bc_clean_victim_a);
  /*	C_BCC_ALC	*/
  if (ev6_passnum < 2) {
    WRITE_ONE(5, >=, 0, --, bc_rd_wr_bubbles_a);
    WRITE_ONE(0, <, 1, ++, bc_rd_rd_bubble_a);
    WRITE_ONE(0, <, 1, ++, bc_wr_wr_bubble_a);
    WRITE_ONE(3, >=, 0, --, bc_wr_rd_bubbles_a);
  }
  else {
    WRITE_ONE(0, <, 1, ++, bc_wr_wr_bubble_a);
    WRITE_ONE(5, >=, 0, --, bc_rd_wr_bubbles_a);
    WRITE_ONE(0, <, 1, ++, bc_rd_rd_bubble_a);
    WRITE_ONE(0, <, 1, ++, bc_sj_bank_enable_a);
    WRITE_ONE(3, >=, 0, --, bc_wr_rd_bubbles_a);
  }
  /*	C_ISB_CT6	*/
  WRITE_DUP(0, <, 1, ++, dup_tag_enable_a);
  WRITE_DUP(1, <, 2, ++, bc_clk_ratio_a);
  
  /*	C_ISB_CT4	*/
  WRITE_DUP(0, <, 1, ++, bc_rdvictim_a);
  WRITE_DUP(1, <, 2, ++, bc_clk_ratio_a);
  /*	C_ISB_CT3	*/
  WRITE_DUP(0, <, 1, ++, bc_rdvictim_a);
  WRITE_DUP(0, <, 1, ++, bc_clean_victim_a);
  WRITE_DUP(0, <, 1, ++, dup_tag_enable_a);
  /*	C_ISB_CT2	*/
  WRITE_DUP(1, <, 2, ++, bc_clk_ratio_a);
  if (ev6_passnum >= 2) {
    WRITE_ONE(0, <, 1, ++, enable_probe_chk_a);
  }
  /*	C_ISB_CT1	*/
  WRITE_ONE(0, <, 1, ++, spec_read_enable_a);
  WRITE_DUP(1, <, 2, ++, bc_clk_ratio_a);
  /*	C_BCS_CT0	*/
  WRITE_DUP(1, <, 2, ++, bc_clk_ratio_a);
  /*	C_VAF_DCE/C_BCS_BMP	*/
  WRITE_ONE(0, <, 1, ++, mbox_bc_prb_stall_a);
  WRITE_ONE(31, >=, 0, --, bc_lat_data_pattern_a);
  /*	C_BCS_BMP	*/
  WRITE_ONE(23, >=, 0, --, bc_lat_tag_pattern_a);
  /*	C_CMF_CTN	*/
  WRITE_DUP(0, <, 1, ++, bc_rdvictim_a);
  WRITE_ONE(0, <, 1, ++, enable_stc_command_a);
  /*	C_BCW_BCS	*/
  WRITE_ONE(2, >=, 0, --, bc_late_write_num_a);
  WRITE_ONE(1, >=, 0, --, bc_cpu_late_write_num_a);
  WRITE_ONE(0, <, 1, ++, bc_burst_mode_enable_a);
  WRITE_ONE(0, <, 1, ++, bc_pentium_mode_a);
  WRITE_DUP(1, <, 2, ++, bc_clk_ratio_a);
  WRITE_ONE(0, <, 1, ++, bc_frm_clk_a);
  /*	C_BIX_PAD/C_BTG_PAD	*/
  WRITE_ONE(1, >=, 0, --, bc_clk_delay_a);
  WRITE_ONE(0, <, 1, ++, bc_ddmr_enable_a);
  WRITE_ONE(0, <, 1, ++, bc_ddmf_enable_a);
  WRITE_ONE(0, <, 1, ++, bc_late_write_upper_a);
  WRITE_ONE(0, <, 1, ++, bc_tag_ddm_fall_en_a);
  WRITE_ONE(0, <, 1, ++, bc_tag_ddm_rise_en_a);
  WRITE_ONE(0, <, 1, ++, bc_clkfwd_enable_a);
  WRITE_ONE(1, >=, 0, --, bc_rcv_mux_cnt_preset_a);
  if (ev6_passnum < 2) {
    WRITE_ONE(0, <, 1, ++, bc_tag_ddm_rd_fall_en_a);
    WRITE_ONE(0, <, 1, ++, bc_tag_ddm_rd_rise_en_a);
  }
  WRITE_DUP(0, <, 1, ++, bc_late_write_upper_a);
  /*	C_BNW_PAD/C_BNE_PAD	*/
  WRITE_ONE(0, <, 1, ++, sys_ddm_fall_en_a);
  WRITE_ONE(0, <, 1, ++, sys_ddm_rise_en_a);
  WRITE_ONE(0, <, 1, ++, sys_clkfwd_enable_a);
  WRITE_ONE(1, >=, 0, --, sys_rcv_mux_cnt_preset_a);
  if (ev6_passnum < 2) {
    WRITE_ONE(0, <, 1, ++, sys_ddm_rd_fall_en_a);
    WRITE_ONE(0, <, 1, ++, sys_ddm_rd_rise_en_a);
  }
  WRITE_ONE(1, >=, 0, --, sys_clk_delay_a);
  WRITE_ONE(0, <, 1, ++, sys_ddmr_enable_a);
  WRITE_ONE(0, <, 1, ++, sys_ddmf_enable_a);
  WRITE_ONE(0, <, 1, ++, bc_ddm_fall_en_a);
  WRITE_ONE(0, <, 1, ++, bc_ddm_rise_en_a);
  WRITE_DUP(0, <, 1, ++, bc_clkfwd_enable_a);
  WRITE_DUP(1, >=, 0, --, bc_rcv_mux_cnt_preset_a);
  if (ev6_passnum < 2) {
    WRITE_ONE(0, <, 1, ++, bc_ddm_rd_fall_en_a);
    WRITE_ONE(0, <, 1, ++, bc_ddm_rd_rise_en_a);
  }
  /*	C_BWN_PAD/C_BEN_PAD	*/
  WRITE_DUP(1, >=, 0, --, bc_clk_delay_a);
  WRITE_DUP(0, <, 1, ++, bc_ddmr_enable_a);
  WRITE_DUP(0, <, 1, ++, bc_ddmf_enable_a);
  WRITE_DUP(0, <, 1, ++, sys_ddm_fall_en_a);
  WRITE_DUP(0, <, 1, ++, sys_ddm_rise_en_a);
  WRITE_DUP(0, <, 1, ++, sys_clkfwd_enable_a);
  WRITE_DUP(1, >=, 0, --, sys_rcv_mux_cnt_preset_a);
  if (ev6_passnum < 2) {
    WRITE_DUP(0, <, 1, ++, sys_ddm_rd_fall_en_a);
    WRITE_DUP(0, <, 1, ++, sys_ddm_rd_rise_en_a);
  }
  WRITE_DUP(1, >=, 0, --, sys_clk_delay_a);
  WRITE_DUP(0, <, 1, ++, sys_ddmr_enable_a);
  WRITE_DUP(0, <, 1, ++, sys_ddmf_enable_a);
  WRITE_DUP(0, <, 1, ++, bc_ddm_fall_en_a);
  WRITE_DUP(0, <, 1, ++, bc_ddm_rise_en_a);
  WRITE_DUP(0, <, 1, ++, bc_clkfwd_enable_a);
  WRITE_DUP(1, >=, 0, --, bc_rcv_mux_cnt_preset_a);
  if (ev6_passnum < 2) {
    WRITE_DUP(0, <, 1, ++, bc_ddm_rd_fall_en_a);
    WRITE_DUP(0, <, 1, ++, bc_ddm_rd_rise_en_a);
  }
  /*	C_BWS_PAD/C_BES_PAD	*/
  WRITE_DUP(0, <, 1, ++, sys_ddm_fall_en_a);
  WRITE_DUP(0, <, 1, ++, sys_ddm_rise_en_a);
  WRITE_DUP(0, <, 1, ++, sys_clkfwd_enable_a);
  WRITE_DUP(1, >=, 0, --, sys_rcv_mux_cnt_preset_a);
  if (ev6_passnum < 2) {
    WRITE_DUP(0, <, 1, ++, sys_ddm_rd_fall_en_a);
    WRITE_DUP(0, <, 1, ++, sys_ddm_rd_rise_en_a);
  }
  WRITE_DUP(1, >=, 0, --, sys_clk_delay_a);
  WRITE_DUP(0, <, 1, ++, sys_ddmr_enable_a);
  WRITE_DUP(0, <, 1, ++, sys_ddmf_enable_a);
  WRITE_DUP(0, <, 1, ++, bc_ddm_fall_en_a);
  WRITE_DUP(0, <, 1, ++, bc_ddm_rise_en_a);
  WRITE_DUP(0, <, 1, ++, bc_clkfwd_enable_a);
  WRITE_DUP(1, >=, 0, --, bc_rcv_mux_cnt_preset_a);
  if (ev6_passnum < 2) {
    WRITE_DUP(0, <, 1, ++, bc_ddm_rd_fall_en_a);
    WRITE_DUP(0, <, 1, ++, bc_ddm_rd_rise_en_a);
  }
  /*	C_BSW_PAD/C_BSE_PAD	*/
  WRITE_DUP(1, >=, 0, --, bc_clk_delay_a);
  WRITE_DUP(0, <, 1, ++, bc_ddmr_enable_a);
  WRITE_DUP(0, <, 1, ++, bc_ddmf_enable_a);
  WRITE_DUP(0, <, 1, ++, sys_ddm_fall_en_a);
  WRITE_DUP(0, <, 1, ++, sys_ddm_rise_en_a);
  WRITE_DUP(0, <, 1, ++, sys_clkfwd_enable_a);
  WRITE_DUP(1, >=, 0, --, sys_rcv_mux_cnt_preset_a);
  if (ev6_passnum < 2) {
    WRITE_DUP(0, <, 1, ++, sys_ddm_rd_fall_en_a);
    WRITE_DUP(0, <, 1, ++, sys_ddm_rd_rise_en_a);
  }
  WRITE_DUP(1, >=, 0, --, sys_clk_delay_a);
  WRITE_DUP(0, <, 1, ++, sys_ddmr_enable_a);
  WRITE_DUP(0, <, 1, ++, sys_ddmf_enable_a);
  WRITE_DUP(0, <, 1, ++, bc_ddm_fall_en_a);
  WRITE_DUP(0, <, 1, ++, bc_ddm_rise_en_a);
  WRITE_DUP(0, <, 1, ++, bc_clkfwd_enable_a);
  WRITE_DUP(1, >=, 0, --, bc_rcv_mux_cnt_preset_a);
  if (ev6_passnum < 2) {
    WRITE_DUP(0, <, 1, ++, bc_ddm_rd_fall_en_a);
    WRITE_DUP(0, <, 1, ++, bc_ddm_rd_rise_en_a);
  }
  /*	C_BSO_PAD	*/
  WRITE_DUP(1, >=, 0, --, sys_clk_delay_a);
  WRITE_DUP(0, <, 1, ++, sys_ddmr_enable_a);
  WRITE_DUP(0, <, 1, ++, sys_ddmf_enable_a);
  /*	C_BSI_PAD	*/
  WRITE_DUP(0, <, 1, ++, sys_ddm_fall_en_a);
  WRITE_DUP(0, <, 1, ++, sys_ddm_rise_en_a);
  WRITE_DUP(0, <, 1, ++, sys_clkfwd_enable_a);
  WRITE_DUP(1, >=, 0, --, sys_rcv_mux_cnt_preset_a);
  if (ev6_passnum < 2) {
    WRITE_DUP(0, <, 1, ++, sys_ddm_rd_fall_en_a);
    WRITE_DUP(0, <, 1, ++, sys_ddm_rd_rise_en_a);
  }
  /*	C_BRS_CFR	*/
  if (ev6_passnum < 2) {
    WRITE_ONE(2, >=, 0, --, cfr_gclk_delay_a);
  }
  else {
    WRITE_ONE(3, >=, 0, --, cfr_gclk_delay_a);
  }
  WRITE_ONE(2, >=, 0, --, cfr_ev6clk_delay_a);
  WRITE_ONE(1, >=, 0, --, cfr_frmclk_delay_a);
  /*	C_OSB_CT1	*/
  WRITE_DUP(2, >=, 0, --, bc_late_write_num_a);
  WRITE_DUP(0, <, 2, ++, bc_cpu_late_write_num_a);
  /*	C_OSB_CT2	*/
  WRITE_ONE(0, <, 1, ++, jitter_cmd_a);
  WRITE_DUP(0, <, 1, ++, fast_mode_disable_a);
  /*	C_OSB_ARB	*/
  WRITE_ONE(0, <, 4, ++, sysdc_delay_a);
  WRITE_ONE(0, <, 2, ++, data_valid_dly_a);
  /*	C_CLK_BCA	*/
  WRITE_DUP(0, <, 1, ++, bc_ddm_fall_en_a);
  WRITE_DUP(0, <, 1, ++, bc_ddm_rise_en_a);
  WRITE_ONE(1, >=, 0, --, bc_cpu_clk_delay_a);
  WRITE_ONE(7, >=, 0, --, bc_fdbk_en_a);
  WRITE_ONE(15, >=, 0, --, bc_clk_ld_vector_a);
  WRITE_ONE(3, >=, 0, --, bc_bphase_ld_vector_a);
  /*	C_CLK_SYS	*/
  WRITE_DUP(0, <, 1, ++, sys_ddm_fall_en_a);
  WRITE_DUP(0, <, 1, ++, sys_ddm_rise_en_a);
  WRITE_ONE(1, >=, 0, --, sys_cpu_clk_delay_a);
  WRITE_ONE(7, >=, 0, --, sys_fdbk_en_a);
  WRITE_ONE(15, >=, 0, --, sys_clk_ld_vector_a);
  WRITE_ONE(3, >=, 0, --, sys_bphase_ld_vector_a);
  WRITE_ONE(4, >=, 0, --, sys_frame_ld_vector_a);
  if (ev6_passnum > 2) {
    WRITE_ONE(4, <, 5, ++, sysdc_delay_a);
  }
  if (_SEEN('s')) {
    /* 31 additional_old_signals for simulation only */
    WRITE_ONE(0, <, 2, ++, frame_select_a);
    WRITE_ONE(0, <, 1, ++, bc_cf_double_clk_a);
    WRITE_ONE(0, <, 1, ++, sys_cf_double_clk_a);
    WRITE_ONE(0, <, 1, ++, bc_clk_ratio_a);
    WRITE_ONE(2, <, 16, ++, bc_clk_ratio_a);
    WRITE_ONE(0, <, 1, ++, sys_clk_ratio_a);
    WRITE_ONE(5, <, 16, ++, sys_clk_ratio_a);
  }
  else {
    /* zero out 31 additional_old_signals for simulation only */
    j += 31;
  }
  j += leader;

  printf("\n");
  return(j);
}

void dump_cbox(FILE *file)
{
  csr_t *csr_p;
  puts("; Dumping CBOX CSRs");
  DUMP_CSR(sys_clk_ratio_a);
  DUMP_CSR(frame_select_a);
  DUMP_CSR(cfr_frmclk_delay_a);
  DUMP_CSR(cfr_ev6clk_delay_a);
  DUMP_CSR(cfr_gclk_delay_a);
  DUMP_CSR(sys_clk_delay_a);
  DUMP_CSR(bc_clk_delay_a);
  DUMP_CSR(sys_clk_ld_vector_a);
  DUMP_CSR(sys_fdbk_en_a);
  DUMP_CSR(sys_bphase_ld_vector_a);
  DUMP_CSR(sys_frame_ld_vector_a);
  DUMP_CSR(sys_cpu_clk_delay_a);
  DUMP_CSR(dup_tag_enable_a);
  DUMP_CSR(prb_tag_only_a);
  DUMP_CSR(spec_read_enable_a);
  DUMP_CSR(mbox_bc_prb_stall_a);
  DUMP_CSR(sysbus_format_a);
  DUMP_CSR(sysbus_mb_enable_a);
  DUMP_CSR(sysbus_ack_limit_a);
  DUMP_CSR(sysbus_vic_limit_a);
  DUMP_CSR(tlaser_stio_mode_a);
  DUMP_CSR(sysdc_delay_a);
  DUMP_CSR(fast_mode_disable_a);
  DUMP_CSR(bc_clean_victim_a);
  DUMP_CSR(bc_rdvictim_a);
  DUMP_CSR(sys_bus_size_a);
  DUMP_CSR(rdvic_ack_inhibit_a);
  DUMP_CSR(enable_stc_command_a);
  DUMP_CSR(bc_frm_clk_a);
  DUMP_CSR(dcvic_threshold_a);
  if (ev6_passnum < 2) {
    DUMP_CSR(bc_rd_rd_bubbles_a);
  }
  DUMP_CSR(bc_rd_rd_bubble_a);
  DUMP_CSR(bc_wr_wr_bubble_a);
  DUMP_CSR(bc_clk_ratio_a);
  DUMP_CSR(bc_clk_ld_vector_a);
  DUMP_CSR(bc_fdbk_en_a);
  DUMP_CSR(bc_bphase_ld_vector_a);
  DUMP_CSR(bc_rd_wr_bubbles_a);
  DUMP_CSR(bc_late_write_num_a);
  DUMP_CSR(bc_wr_rd_bubbles_a);
  DUMP_CSR(bc_cpu_late_write_num_a);
  DUMP_CSR(bc_cpu_clk_delay_a);
  DUMP_CSR(bc_burst_mode_enable_a);
  DUMP_CSR(bc_lat_data_pattern_a);
  DUMP_CSR(bc_lat_tag_pattern_a);
  DUMP_CSR(bc_cf_double_clk_a);
  DUMP_CSR(bc_clkfwd_enable_a);
  DUMP_CSR(bc_ddm_fall_en_a);
  DUMP_CSR(bc_ddm_rise_en_a);
  DUMP_CSR(bc_ddmf_enable_a);
  DUMP_CSR(bc_ddmr_enable_a);
  if (ev6_passnum < 2) {
    DUMP_CSR(bc_ddm_rd_fall_en_a);
    DUMP_CSR(bc_ddm_rd_rise_en_a);
  }
  DUMP_CSR(bc_tag_ddm_fall_en_a);
  DUMP_CSR(bc_tag_ddm_rise_en_a);
  if (ev6_passnum < 2) {
    DUMP_CSR(bc_tag_ddm_rd_fall_en_a);
    DUMP_CSR(bc_tag_ddm_rd_rise_en_a);
  }
  DUMP_CSR(bc_pentium_mode_a);
  DUMP_CSR(sys_cf_double_clk_a);
  DUMP_CSR(sys_clkfwd_enable_a);
  DUMP_CSR(sys_ddm_fall_en_a);
  DUMP_CSR(sys_ddm_rise_en_a);
  DUMP_CSR(sys_ddmf_enable_a);
  DUMP_CSR(sys_ddmr_enable_a);
  if (ev6_passnum < 2) {
    DUMP_CSR(sys_ddm_rd_fall_en_a);
    DUMP_CSR(sys_ddm_rd_rise_en_a);
  }
  DUMP_CSR(sys_rcv_mux_cnt_preset_a);
  DUMP_CSR(bc_rcv_mux_cnt_preset_a);
  DUMP_CSR(data_valid_dly_a);
  DUMP_CSR(jitter_cmd_a);
  DUMP_CSR(bc_late_write_upper_a);
  if (ev6_passnum >= 2) {
    DUMP_CSR(bc_sj_bank_enable_a);
    DUMP_CSR(enable_probe_chk_a);
  }
}

