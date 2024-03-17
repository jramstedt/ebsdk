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
static char *rcsid = "$Id: ev6_cbox.c,v 1.2 1999/01/21 19:06:25 gries Exp $";
#endif

/*
 * Author: Franklin Hooker, Digital Equipment Corporation, July 1997
 *
 * $Log: ev6_cbox.c,v $
 * Revision 1.2  1999/01/21 19:06:25  gries
 * First Release to cvs gries
 *
 * Revision 1.5  1997/12/22  18:19:21  quinn
 * changes for PASS-2 Cbox write_once chain
 *
 * Revision 1.4  1997/07/09  04:17:45  fdh
 * Removed skewed_fill_mode_a_h from table of CSRs.
 * This is now an alias to bc_clk_ratio_a<1> instead.
 *
 * Revision 1.3  1997/07/08  15:28:53  fdh
 * Prints out the csr name when an error is reported.
 *
 * Revision 1.2  1997/07/07  14:56:34  fdh
 * Comments...
 *
 * Revision 1.1  1997/07/07  00:50:05  fdh
 * Initial revision
 *
 */

#include "ic4mat.h"

csr_t csr_array[] = {
  {"sys_clk_ratio_a", 0, "alias_sys_clk_ratio_a", 0},
  {"frame_select_a", 0, "alias_frame_select_a", 0},
  {"cfr_frmclk_delay_a", 0, "alias_cfr_frmclk_delay_a", 0},
  {"cfr_ev6clk_delay_a", 0, "alias_cfr_ev6clk_delay_a", 0},
  {"cfr_gclk_delay_a", 0, "alias_cfr_gclk_delay_a", 0},
  {"sys_clk_delay_a", 0, "alias_sys_clk_delay_a", 0},
  {"bc_clk_delay_a", 0, "alias_bc_clk_delay_a", 0},
  {"sys_clk_ld_vector_a", 0, "alias_sys_clk_ld_vector_a", 0},
  {"sys_fdbk_en_a", 0, "alias_sys_fdbk_en_a", 0},
  {"sys_bphase_ld_vector_a", 0, "alias_sys_bphase_ld_vector_a", 0},
  {"sys_frame_ld_vector_a", 0, "alias_sys_frame_ld_vector_a", 0},
  {"sys_cpu_clk_delay_a", 0, "alias_sys_cpu_clk_delay_a", 0},
  {"dup_tag_enable_a", 0, "alias_dup_tag_enable_a", 0},
  {"prb_tag_only_a", 0, "alias_prb_tag_only_a", 0},
  {"spec_read_enable_a", 0, "alias_spec_read_enable_a", 0},
  {"mbox_bc_prb_stall_a", 0, "alias_mbox_bc_prb_stall_a", 0},
  {"sysbus_format_a", 0, "alias_sysbus_format_a", 0},
  {"sysbus_mb_enable_a", 0, "alias_sysbus_mb_enable_a", 0},
  {"sysbus_ack_limit_a", 0, "alias_sysbus_ack_limit_a", 0},
  {"sysbus_vic_limit_a", 0, "alias_sysbus_vic_limit_a", 0},
  {"tlaser_stio_mode_a", 0, "alias_tlaser_stio_mode_a", 0},
  {"sysdc_delay_a", 0, "alias_sysdc_delay_a", 0},
  {"fast_mode_disable_a", 0, "alias_fast_mode_disable_a", 0},
  {"bc_clean_victim_a", 0, "alias_bc_clean_victim_a", 0},
  {"bc_rdvictim_a", 0, "alias_bc_rdvictim_a", 0},
  {"sys_bus_size_a", 0, "alias_sys_bus_size_a", 0},
  {"rdvic_ack_inhibit_a", 0, "alias_rdvic_ack_inhibit_a", 0},
  {"enable_stc_command_a", 0, "alias_enable_stc_command_a", 0},
  {"bc_frm_clk_a", 0, "alias_bc_frm_clk_a", 0},
  {"dcvic_threshold_a", 0, "alias_dcvic_threshold_a", 0},
  {"bc_rd_rd_bubbles_a", 0, "alias_bc_rd_rd_bubbles_a", 0},
  {"bc_rd_rd_bubble_a", 0, "alias_bc_rd_rd_bubble_a", 0},
  {"bc_wr_wr_bubble_a", 0, "alias_bc_wr_wr_bubble_a", 0},
  {"bc_clk_ratio_a", 0, "alias_bc_clk_ratio_a", 0},
  {"bc_clk_ld_vector_a", 0, "alias_bc_clk_ld_vector_a", 0},
  {"bc_fdbk_en_a", 0, "alias_bc_fdbk_en_a", 0},
  {"bc_bphase_ld_vector_a", 0, "alias_bc_bphase_ld_vector_a", 0},
  {"bc_rd_wr_bubbles_a", 0, "alias_bc_rd_wr_bubbles_a", 0},
  {"bc_late_write_num_a", 0, "alias_bc_late_write_num_a", 0},
  {"bc_wr_rd_bubbles_a", 0, "alias_bc_wr_rd_bubbles_a", 0},
  {"bc_cpu_late_write_num_a", 0, "alias_bc_cpu_late_write_num_a", 0},
  {"bc_cpu_clk_delay_a", 0, "alias_bc_cpu_clk_delay_a", 0},
  {"bc_burst_mode_enable_a", 0, "alias_bc_burst_mode_enable_a", 0},
  {"bc_lat_data_pattern_a", 0, "alias_bc_lat_data_pattern_a", 0},
  {"bc_lat_tag_pattern_a", 0, "alias_bc_lat_tag_pattern_a", 0},
  {"bc_cf_double_clk_a", 0, "alias_bc_cf_double_clk_a", 0},
  {"bc_clkfwd_enable_a", 0, "alias_bc_clkfwd_enable_a", 0},
  {"bc_ddm_fall_en_a", 0, "alias_bc_ddm_fall_en_a", 0},
  {"bc_ddm_rise_en_a", 0, "alias_bc_ddm_rise_en_a", 0},
  {"bc_ddmf_enable_a", 0, "alias_bc_ddmf_enable_a", 0},
  {"bc_ddmr_enable_a", 0, "alias_bc_ddmr_enable_a", 0},
  {"bc_ddm_rd_fall_en_a", 0, "alias_bc_ddm_rd_fall_en_a", 0},
  {"bc_ddm_rd_rise_en_a", 0, "alias_bc_ddm_rd_rise_en_a", 0},
  {"bc_tag_ddm_fall_en_a", 0, "alias_bc_tag_ddm_fall_en_a", 0},
  {"bc_tag_ddm_rise_en_a", 0, "alias_bc_tag_ddm_rise_en_a", 0},
  {"bc_tag_ddm_rd_fall_en_a", 0, "alias_bc_tag_ddm_rd_fall_en_a", 0},
  {"bc_tag_ddm_rd_rise_en_a", 0, "alias_bc_tag_ddm_rd_rise_en_a", 0},
  {"bc_pentium_mode_a", 0, "alias_bc_pentium_mode_a", 0},
  {"sys_cf_double_clk_a", 0, "alias_sys_cf_double_clk_a", 0},
  {"sys_clkfwd_enable_a", 0, "alias_sys_clkfwd_enable_a", 0},
  {"sys_ddm_fall_en_a", 0, "alias_sys_ddm_fall_en_a", 0},
  {"sys_ddm_rise_en_a", 0, "alias_sys_ddm_rise_en_a", 0},
  {"sys_ddmf_enable_a", 0, "alias_sys_ddmf_enable_a", 0},
  {"sys_ddmr_enable_a", 0, "alias_sys_ddmr_enable_a", 0},
  {"sys_ddm_rd_fall_en_a", 0, "alias_sys_ddm_rd_fall_en_a", 0},
  {"sys_ddm_rd_rise_en_a", 0, "alias_sys_ddm_rd_rise_en_a", 0},
  {"sys_rcv_mux_cnt_preset_a", 0, "alias_sys_rcv_mux_cnt_preset_a", 0},
  {"bc_rcv_mux_cnt_preset_a", 0, "alias_bc_rcv_mux_cnt_preset_a", 0},
  {"data_valid_dly_a", 0, "alias_data_valid_dly_a", 0},
  {"jitter_cmd_a", 0, "alias_jitter_cmd_a", 0},
  {"bc_late_write_upper_a", 0, "alias_bc_late_write_upper_a", 0},
  {"bc_sj_bank_enable_a", 0, "alias_bc_sj_bank_enable_a", 0},
  {"enable_probe_chk_a", 0, "alias_enable_probe_chk_a", 0},
  {NULL, 0, NULL, 0}
};

csr_t *lookup_csr(char *name)
{
  csr_t *p;
  p = csr_array;
  while (p->name != NULL) {
    if ((strcmp(name, p->name) == 0)
	|| (strcmp(name, p->alias) == 0)) {
      return(p);
    }
    p += 1;
  }

  report_error(NULL,
	       "Error:",
	       "Unknown csr name \"%s\" specified\n", name);
  ++errors;
  return(NULL);
}
