
#ifndef _msgnum_
#define _msgnum_
/*
 * External message declarations
 */
extern char msg_overflow [];
extern char msg_unicheck [];
extern char msg_shell_nocmd [];
extern char msg_shell_noex [];
extern char msg_qscan_noqual [];
extern char msg_qscan_conv [];
extern char msg_qscan_misc [];
extern char msg_qscan_valerr [];
extern char msg_poorly_formed_exp [];
extern char msg_cmp_seek [];
extern char msg_cmp_length [];
extern char msg_cmp_neq [];
extern char msg_ctx_invalid [];
extern char msg_sort_scratch [];
extern char msg_success [];
extern char msg_failure [];
extern char msg_killed [];
extern char msg_fatal [];
extern char msg_nofile [];
extern char msg_file_not_found [];
extern char msg_ambig_file [];
extern char msg_not_removed [];
extern char msg_bad_open [];
extern char msg_no_fp_slot [];
extern char msg_no_ip_slot [];
extern char msg_file_in_use [];
extern char msg_bad_access_mode [];
extern char msg_bad_filename [];
extern char msg_no_infonext [];
extern char msg_noprocess [];
extern char msg_extra_params [];
extern char msg_insuff_params [];
extern char msg_insuff_mem [];
extern char msg_qual_conflict [];
extern char msg_illegal_operation [];
extern char msg_exdep_conflict [];
extern char msg_exdep_badadr [];
extern char msg_exdep_badparam [];
extern char msg_exdep_badincsize [];
extern char msg_dep_param_missing [];
extern char msg_dep_baddata [];
extern char msg_ex_devopenrderr [];
extern char msg_dep_devopenwrerr [];
extern char msg_ex_devreaderr [];
extern char msg_dep_devwriteerr [];
extern char msg_dev_found [];
extern char msg_dev_lost [];
extern char msg_rec_underflow [];
extern char msg_rec_overflow [];
extern char msg_eof [];
extern char msg_eot [];
extern char msg_bad_inode [];
extern char msg_bad_device [];
extern char msg_no_media [];
extern char msg_no_conn [];
extern char msg_expected_ill_req [];
extern char msg_reservation_conflict [];
extern char msg_not_ready [];
extern char msg_write_prot [];
extern char msg_bad_request [];
extern char msg_scsi_bad_status [];
extern char msg_scsi_retries_gone [];
extern char msg_scsi_send_fail [];
extern char msg_scsi_bad_check [];
extern char msg_scsi_not_512 [];
extern char msg_scsi_ill_dev_typ [];
extern char msg_scsi_ill_err_cod [];
extern char msg_scsi_sense_key [];
extern char msg_scsi_sense_key_x [];
extern char msg_scsi_bad_su [];
extern char msg_scsi_bad_ms [];
extern char msg_scsi_bad_rc [];
extern char msg_scsi_bad_rewind [];
extern char msg_scsi_bad_read [];
extern char msg_scsi_bad_write [];
extern char msg_pi_bad_validate [];
extern char msg_dyn_free_lnk [];
extern char msg_dyn_adj_lnk [];
extern char msg_dyn_free_seq [];
extern char msg_dyn_adj_seq [];
extern char msg_dyn_size [];
extern char msg_dyn_badzone [];
extern char msg_noev [];
extern char msg_ev_prot [];
extern char msg_ev_badname [];
extern char msg_ev_badvalue [];
extern char msg_nodis [];
extern char msg_quals [];
extern char msg_eval_overflow [];
extern char msg_eval_underflow [];
extern char msg_eval_values [];
extern char msg_radix [];
extern char msg_show_iob_hdr1 [];
extern char msg_show_iob_hdr2 [];
extern char msg_d_header [];
extern char msg_d_err [];
extern char msg_d_hard [];
extern char msg_d_soft [];
extern char msg_d_fatal [];
extern char msg_d_pass [];
extern char msg_d_fail [];
extern char msg_d_err_end [];
extern char msg_d_completion [];
extern char msg_d_end_of_pass [];
extern char msg_d_startup [];
extern char msg_d_status [];
extern char msg_d_test_trace [];
extern char msg_d_error [];
extern char msg_d_no_dev_name [];
extern char msg_d_inv_test [];
extern char msg_d_inv_grp [];
extern char msg_d_inv_test_grp [];
extern char msg_d_no_iob_found [];
extern char msg_d_bad_group [];
extern char msg_no_kbd_use_serial [];
extern char msg_init_port [];
extern char msg_port_inited [];
extern char msg_port_state_running [];
extern char msg_ez_err_nisa_null [];
extern char msg_ez_err_nisa_bad_chk [];
extern char msg_ez_err_nisa_n_rpt [];
extern char msg_ez_err_bad_patt [];
extern char msg_ez_inf_dev_spec [];
extern char msg_ez_inf_mop_blk [];
extern char msg_ez_inf_net_lst [];
extern char msg_ez_err_n_env_var [];
extern char msg_ez_inf_mop_cnt [];
extern char msg_ez_inf_t_s_0 [];
extern char msg_ez_inf_b_f [];
extern char msg_ez_inf_df_oc_mc [];
extern char msg_ez_inf_tx_f [];
extern char msg_ez_inf_ec_cc_sc [];
extern char msg_ez_inf_oc_lf_rd [];
extern char msg_ez_inf_cd [];
extern char msg_ez_inf_tx [];
extern char msg_ez_inf_rx [];
extern char msg_ez_inf_mb_mf [];
extern char msg_ez_inf_rx_f [];
extern char msg_ez_inf_bc_fe [];
extern char msg_ez_inf_ud_do_ns [];
extern char msg_ez_inf_nub [];
extern char msg_ez_inf_add_sz [];
extern char msg_ez_inf_003 [];
extern char msg_ndl_err_001 [];
extern char msg_ndl_err_002 [];
extern char msg_ndl_err_003 [];
extern char msg_mop_inf_cntrs [];
extern char msg_mop_wrn_out_o_seq_ml [];
extern char msg_mop_det_sv_msg [];
extern char msg_mop_wrn_out_o_seq_mlxf [];
extern char msg_mop_wrn_out_o_seq_plxf [];
extern char msg_mop_err_n_net_fl [];
extern char msg_mop_err_nr_net_fl [];
extern char msg_mop_inf_ld_com [];
extern char msg_mop_inf_hst_name [];
extern char msg_mop_inf_retry_net [];
extern char msg_mop_err_lp_to [];
extern char msg_mop_err_lp_to1 [];
extern char msg_mop_inf_hst_addr [];
extern char msg_moprc_err_rc_already_res [];
extern char msg_moprc_err_rc_n_res [];
extern char msg_moprc_inf_rc_rel [];
extern char msg_moprc_err_rc_b_ver [];
extern char msg_moprc_inf_rc_res [];
extern char msg_moprc_err_cd_kl_sh [];
extern char msg_moplp_err_crc [];
extern char msg_mop_err_001 [];
extern char msg_net_003 [];
extern char msg_net_008 [];
extern char msg_net_010 [];
extern char msg_moprc_001 [];
extern char msg_vmem_invalpfn [];
extern char msg_vmem_accvio [];
extern char msg_vmem_devopenrderr [];
extern char msg_hlt_hw [];
extern char msg_hlt_ksnv [];
extern char msg_hlt_scbb [];
extern char msg_hlt_ptbr [];
extern char msg_hlt_sw [];
extern char msg_hlt_dblerr [];
extern char msg_hlt_pal_mchk [];
extern char msg_mop_inf_reply [];
extern char msg_ext_message [];
extern char msg_mod [];
extern char msg_shell_nopipe [];
extern char msg_iec [];
extern char msg_imh [];
extern char msg_ctrlc [];
extern char msg_lt_brckt_expected [];
extern char msg_ics [];
extern char msg_loop [];
extern char msg_node [];
extern char msg_tmb [];
extern char msg_scsi_retry [];
extern char msg_mop_err_cd_kl_rc_t [];
extern char msg_rem [];
extern char msg_dyn_poison [];
extern char msg_rcvd [];
extern char msg_timeout [];
extern char msg_out_len [];
extern char msg_bad_devicename [];
extern char msg_post_cnt [];
extern char msg_hyphen_prefix [];
extern char msg_halt [];
extern char msg_buf_size [];
extern char msg_post [];
extern char msg_cnt [];
extern char msg_null_terminated [];
extern char msg_qscan_allocerr [];
extern char msg_hyphen_suffix [];
extern char msg_invalid_cmd [];
extern char msg_cmm_wrn_presto_valid [];
extern char msg_no_file_slot [];
extern char msg_sunday [];
extern char msg_monday [];
extern char msg_tuesday [];
extern char msg_wednesday [];
extern char msg_thursday [];
extern char msg_friday [];
extern char msg_saturday [];
extern char msg_january [];
extern char msg_february [];
extern char msg_march [];
extern char msg_april [];
extern char msg_may [];
extern char msg_june [];
extern char msg_july [];
extern char msg_august [];
extern char msg_september [];
extern char msg_october [];
extern char msg_november [];
extern char msg_december [];
extern char msg_no_boot [];
extern char msg_cnsl_secure [];
extern char msg_invld_psswd [];
extern char msg_val_err [];
extern char msg_psswd_len_err [];
extern char msg_curr_psswd_err [];
extern char msg_cmd_noparams [];
extern char msg_noboot_params [];
extern char msg_no_vldpsswd [];
extern char msg_pwd_cleared [];
extern char msg_einval [];
extern char msg_pdq_msg1 [];
extern char msg_pdq_msg2 [];
extern char msg_pdq_msg3 [];
extern char msg_pdq_msg4 [];
extern char msg_pdq_msg5 [];
extern char msg_pdq_msg6 [];
extern char msg_pdq_msg7 [];
extern char msg_pdq_msg8 [];
extern char msg_pdq_msg9 [];
extern char msg_pdq_msg10 [];
extern char msg_pdq_msg11 [];
extern char msg_pdq_msg12 [];
extern char msg_pdq_msg13 [];
extern char msg_pdq_msg14 [];
#define msg_success 0
#define msg_failure 1
#define msg_def 2
#define msg_halt 3
#define msg_loop 4
#define msg_error 5
#endif
