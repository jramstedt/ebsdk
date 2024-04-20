/*
 * Message definitions
 */
char                     msg_overflow [] = "numeric value too large\n";
char                     msg_unicheck [] = "%s: multiple cpus can respond.\nChange affinity mask using SA command or &p operator.\n";
char                  msg_shell_nocmd [] = "no shell command\n";
char                   msg_shell_noex [] = "not executable\n";
char                 msg_qscan_noqual [] = "unrecognized qualifiers\n";
char                   msg_qscan_conv [] = "qualifier conversion error\n";
char                   msg_qscan_misc [] = "qualifier value not found\n";
char                 msg_qscan_valerr [] = "qualifier conversion specification not recognized\n";
char            msg_poorly_formed_exp [] = "poorly formed regular expression\n";
char                     msg_cmp_seek [] = "cmp: seek error\n";
char                   msg_cmp_length [] = "cmp: files not equal length\n";
char                      msg_cmp_neq [] = "cmp: files not equal at offset %08X %02X %02X\n";
char                  msg_ctx_invalid [] = "Slot context is not valid\n";
char                 msg_sort_scratch [] = "sort: internal scratch space exceeded\n";
char                      msg_success [] = "";
char                      msg_failure [] = "";
char                       msg_killed [] = "";
char                        msg_fatal [] = "fatal error\n";
char                       msg_nofile [] = "no such file\n";
char               msg_file_not_found [] = "file %s not found\n";
char                   msg_ambig_file [] = "ambiguous filename\n";
char                  msg_not_removed [] = "rm: %s not removed\n";
char                     msg_bad_open [] = "file open failed for %s\n";
char                   msg_no_fp_slot [] = "no open file slot\n";
char                   msg_no_ip_slot [] = "no more inodes\n";
char                  msg_file_in_use [] = "file is in use\n";
char              msg_bad_access_mode [] = "invalid access mode\n";
char                 msg_bad_filename [] = "invalid file name\n";
char                  msg_no_infonext [] = "info/next fields not allowed\n";
char                    msg_noprocess [] = "no such process\n";
char                 msg_extra_params [] = "too many parameters\n";
char                msg_insuff_params [] = "insufficient parameters\n";
char                   msg_insuff_mem [] = "insufficient dynamic memory for a request of %d bytes\n";
char                msg_qual_conflict [] = "conflicting qualifiers\n";
char            msg_illegal_operation [] = "illegal operation on driver %s\n";
char               msg_exdep_conflict [] = "conflicting qualifiers - command ignored\n";
char                 msg_exdep_badadr [] = "Illegal target address\n";
char               msg_exdep_badparam [] = "Too many command parameters\n";
char             msg_exdep_badincsize [] = "Illegal value for data size increment\n";
char            msg_dep_param_missing [] = "Required parameters were not specified\n";
char                  msg_dep_baddata [] = "Illegal data parameter\n";
char              msg_ex_devopenrderr [] = "Error opening device %s for read access\n";
char             msg_dep_devopenwrerr [] = "Error opening device %s for write access\n";
char                msg_ex_devreaderr [] = "Error in read of %d bytes at location %08X from device %s\n";
char              msg_dep_devwriteerr [] = "Error in write of %d bytes to location %08X on device %s\n";
char                    msg_dev_found [] = "device %s (%s) found on %s\n";
char                     msg_dev_lost [] = "device %s (%s) lost on %s\n";
char                msg_rec_underflow [] = "record too short (%d bytes expected)\n";
char                 msg_rec_overflow [] = "record too long (%d bytes expected)\n";
char                          msg_eof [] = "end of file encountered\n";
char                          msg_eot [] = "end of tape encountered\n";
char                    msg_bad_inode [] = "file %s no longer exists\n";
char                   msg_bad_device [] = "device %s no longer valid\n";
char                     msg_no_media [] = "%s has no media present or is disabled via the RUN/STOP switch\n";
char                      msg_no_conn [] = "%s is not connected\n";
char             msg_expected_ill_req [] = "Illegal request\n";
char         msg_reservation_conflict [] = "Reservation conflict for %s\n";
char                    msg_not_ready [] = "%s is not ready\n";
char                   msg_write_prot [] = "%s is write protected\n";
char                  msg_bad_request [] = "I/O request to %s is invalid (check type, offset, and size)\n";
char              msg_scsi_bad_status [] = "bad SCSI status (%02x) received from %s\n";
char            msg_scsi_retries_gone [] = "retries to %s exhausted\n";
char               msg_scsi_send_fail [] = "failed to send command to %s\n";
char               msg_scsi_bad_check [] = "invalid or fatal SCSI Check Condition reported\n";
char                 msg_scsi_not_512 [] = "block size is not 512 bytes for %s\n";
char             msg_scsi_ill_dev_typ [] = "illegal Inquiry device type (%d) from %s\n";
char             msg_scsi_ill_err_cod [] = "illegal Request Sense error code (%02x) from %s\n";
char               msg_scsi_sense_key [] = "sense key = '%s' (%02x|%02x) from %s\n";
char             msg_scsi_sense_key_x [] = "sense key = %02x (%02x|%02x) from %s\n";
char                  msg_scsi_bad_su [] = "failed to send Start Unit to %s\n";
char                  msg_scsi_bad_ms [] = "failed to send Mode Select to %s\n";
char                  msg_scsi_bad_rc [] = "failed to send Read Capacity to %s\n";
char              msg_scsi_bad_rewind [] = "failed to send Rewind to %s\n";
char                msg_scsi_bad_read [] = "failed to send Read to %s\n";
char               msg_scsi_bad_write [] = "failed to send Write to %s\n";
char              msg_pi_bad_validate [] = "PI driver failure (%d)\n";
char                 msg_dyn_free_lnk [] = "bad free list detected at %08x\n";
char                  msg_dyn_adj_lnk [] = "bad adjacency list detected at %08x\n";
char                 msg_dyn_free_seq [] = "free list not in ascending order at %08x\n";
char                  msg_dyn_adj_seq [] = "adjacency list not sequential at %08x\n";
char                     msg_dyn_size [] = "non negative size at %08x\n";
char                  msg_dyn_badzone [] = "%08X is not a valid zone address.  Use dynamic to list valid zones.\n";
char                         msg_noev [] = "%s does not exist\n";
char                      msg_ev_prot [] = "%s protected from attempted operation\n";
char                   msg_ev_badname [] = "bad name - %s not created\n";
char                  msg_ev_badvalue [] = "bad value - %s not modified\n";
char                        msg_nodis [] = "diagnostic aborted due to missing dispatch table\n";
char                        msg_quals [] = "diagnostic aborted due to invalid command line qualifiers\n";
char                msg_eval_overflow [] = "expression evaluator overflow\n";
char               msg_eval_underflow [] = "expression evaluator underflow\n";
char                  msg_eval_values [] = "values left on expression stack\n";
char                        msg_radix [] = "illegal digit for specified radix\n";
char                msg_show_iob_hdr1 [] = " ID       Program      Device       Pass  Hard/Soft Bytes Written  Bytes Read  \n";
char                msg_show_iob_hdr2 [] = "-------- ------------ ------------ ------ --------- ------------- -------------\n";
char                     msg_d_header [] = "Diagnostic Name        ID             Device  Pass  Test  Hard/Soft";
char                        msg_d_err [] = "*** Error -";
char                       msg_d_hard [] = "*** Hard Error -";
char                       msg_d_soft [] = "*** Soft Error -";
char                      msg_d_fatal [] = "*** Fatal Error -";
char                       msg_d_pass [] = "* End of Run - Passed *";
char                       msg_d_fail [] = "* End of Run - Failed *";
char                    msg_d_err_end [] = "\n*** End of Error ***\n\n";
char                 msg_d_completion [] = "\n\nTesting Complete\n\n%m  %s\n%-12.12s     %08.8x     %14.14s  %4.4d        %4.4d %4.4d     %8.8s\n\n%-m\n";
char                msg_d_end_of_pass [] = "%-12.12s     %08.8x     %14.14s  %4.4d        %4.4d %4.4d     %8.8s\n";
char                    msg_d_startup [] = "%m  %s\n%-12.12s     %08.8x     %14.14s     0           0    0     %8.8s\n";
char                     msg_d_status [] = "%m  %s\n%-12.12s     %08.8x     %14.14s  %4.4d  %4.4d  %4.4d %4.4d     %8.8s\n\n";
char                 msg_d_test_trace [] = "%-12.12s     %08.8x     %14.14s  %4.4d  %4.4d  %4.4d %4.4d     %8.8s\n";
char                      msg_d_error [] = "\n%m Error #%-d - %m\n%m  %s\n%-12.12s     %08.8x     %14.14s  %4.4d  %4.4d  %4.4d %4.4d     %8.8s\n";
char                msg_d_no_dev_name [] = "No device name was entered for IOB search\n";
char                   msg_d_inv_test [] = "Invalid test selection\n";
char                    msg_d_inv_grp [] = "Invalid group selection\n";
char               msg_d_inv_test_grp [] = "Invalid test selection for group entered\n";
char               msg_d_no_iob_found [] = "No IOB was found corresponding to device %s\n";
char                  msg_d_bad_group [] = "D_GROUP set to %s -- No tests found in that group\n";
char            msg_no_kbd_use_serial [] = "Keyboard error; using serial port terminal\n";
char                    msg_init_port [] = "Initializing driver %s.\n";
char                  msg_port_inited [] = "Driver %s initialized.\n";
char           msg_port_state_running [] = "Running";
char             msg_ez_err_nisa_null [] = "Network station address is a Null address\n";
char          msg_ez_err_nisa_bad_chk [] = "Network station address ROM has a bad checksum\n";
char            msg_ez_err_nisa_n_rpt [] = "Network station address is not repeated\n";
char              msg_ez_err_bad_patt [] = "Pattern ff0055aa does not exist in station address ROM\n";
char              msg_ez_inf_dev_spec [] = "\nDEVICE SPECIFIC:\n";
char               msg_ez_inf_mop_blk [] = "\nMOP BLOCK:\n";
char               msg_ez_inf_net_lst [] = " Network list size: %d\n";
char             msg_ez_err_n_env_var [] = "Couldn't read env variable: %s\n";
char               msg_ez_inf_mop_cnt [] = "\nMOP COUNTERS:\n";
char                 msg_ez_inf_t_s_0 [] = "Time since zeroed (Secs): %d\n";
char                   msg_ez_inf_b_f [] = " Bytes: %d Frames: %d\n";
char              msg_ez_inf_df_oc_mc [] = " Deferred: %d One collision: %d Multi collisions: %d\n";
char                  msg_ez_inf_tx_f [] = "TX Failures:\n";
char              msg_ez_inf_ec_cc_sc [] = " Excessive collisions: %d Carrier check: %d Short circuit: %d\n";
char              msg_ez_inf_oc_lf_rd [] = " Open circuit: %d Long frame: %d Remote defer: %d\n";
char                    msg_ez_inf_cd [] = " Collision detect: %d\n";
char                    msg_ez_inf_tx [] = "\nTX:\n";
char                    msg_ez_inf_rx [] = "\nRX:\n";
char                 msg_ez_inf_mb_mf [] = " Multicast bytes: %d Multicast frames: %d\n";
char                  msg_ez_inf_rx_f [] = "RX Failures:\n";
char                 msg_ez_inf_bc_fe [] = " Block check: %d Framing error: %d Long frame: %d\n";
char              msg_ez_inf_ud_do_ns [] = " Unknown destination: %d Data overrun: %d No system buffer: %d\n";
char                   msg_ez_inf_nub [] = " No user buffers: %d\n";
char                msg_ez_inf_add_sz [] = "Packet address: %08x size: %d\n";
char		       msg_ez_inf_003 [] = "\nPORT INFO:\n";
char		      msg_ndl_err_001 [] = "Invalid enet address: %s\n";
char		      msg_ndl_err_002 [] = "*** Error (%s), ";
char		      msg_ndl_err_003 [] = "*** Error, ";
char                msg_mop_inf_cntrs [] = "Counters\n";
char         msg_mop_wrn_out_o_seq_ml [] = "Out of sequence message (MEM_LOAD)\n";
char               msg_mop_det_sv_msg [] = "Saved: %d message: %d\n";
char       msg_mop_wrn_out_o_seq_mlxf [] = "Out of sequence message (MEM_LOAD_W_XFER)\n";
char       msg_mop_wrn_out_o_seq_plxf [] = "Out of sequence message (PARAM_LOAD_W_XFER)\n";
char             msg_mop_err_n_net_fl [] = "Couldn't read the network file\n";
char            msg_mop_err_nr_net_fl [] = "Couldn't write the network file\n";
char               msg_mop_inf_ld_com [] = "Network load complete.\n";
char             msg_mop_inf_hst_name [] = "Host name: %s\n";
char            msg_mop_inf_retry_net [] = "Retrying...\n";
char                msg_mop_err_lp_to [] = "Mop loop message timed out from: %02x-%02x-%02x-%02x-%02x-%02x\n";
char               msg_mop_err_lp_to1 [] = "*** List index: %d received count: %d expected count %d\n\n";
char             msg_mop_inf_hst_addr [] = "Host address: %02x-%02x-%02x-%02x-%02x-%02x\n\n";
char     msg_moprc_err_rc_already_res [] = "Remote console already reserved.\n";
char           msg_moprc_err_rc_n_res [] = "Remote console is not reserved\n";
char             msg_moprc_inf_rc_rel [] = "\nRemote console released from: %02x-%02x-%02x-%02x-%02x-%02x\n";
char           msg_moprc_err_rc_b_ver [] = "Incorrect remote console verification.\n";
char             msg_moprc_inf_rc_res [] = "\nRemote console reserved from: %02x-%02x-%02x-%02x-%02x-%02x\n";
char           msg_moprc_err_cd_kl_sh [] = "Couldn't kill RC shell.\n";
char                msg_moplp_err_crc [] = "CRC error: rec: %d saved crc: %08x rcvd crc: %08x\n";
char                  msg_mop_err_001 [] = "FDDI not supported\n";
char			  msg_net_003 [] = "File not loaded.\n";
char			  msg_net_008 [] = "Target address: %02x-%02x-%02x-%02x-%02x-%02x\n";
char			  msg_net_010 [] = "Host address: %02x-%02x-%02x-%02x-%02x-%02x\n";
char			msg_moprc_001 [] = "proc=%08X verif_h=%08X  verif_l=%08X \n";
char                msg_vmem_invalpfn [] = "Invalid Page Table Entry for virtual address %08X\n";
char                  msg_vmem_accvio [] = "Access violation found for virtual address %08X\n";
char            msg_vmem_devopenrderr [] = "Error opening pmem device for read access from vmem driver\n";
char                       msg_hlt_hw [] = "operator initiated halt\n";
char                     msg_hlt_ksnv [] = "kernel stack not valid halt\n";
char                     msg_hlt_scbb [] = "invalid SCBB\n";
char                     msg_hlt_ptbr [] = "invalid PTBR\n";
char                       msg_hlt_sw [] = "HALT instruction executed\n";
char                   msg_hlt_dblerr [] = "double error halt\n";
char                 msg_hlt_pal_mchk [] = "machine check while in PAL mode\n";
char                msg_mop_inf_reply [] = "Loop Reply from: %02x-%02x-%02x-%02x-%02x-%02x\n";
char                  msg_ext_message [] = "";
char                          msg_mod [] = "";
char                 msg_shell_nopipe [] = "";
char                          msg_iec [] = "";
char                          msg_imh [] = "";
char                        msg_ctrlc [] = "";
char            msg_lt_brckt_expected [] = "";
char                          msg_ics [] = "";
char                         msg_loop [] = "";
char                         msg_node [] = "";
char                          msg_tmb [] = "";
char                   msg_scsi_retry [] = "";
char           msg_mop_err_cd_kl_rc_t [] = "";
char                          msg_rem [] = "";
char                   msg_dyn_poison [] = "";
char                         msg_rcvd [] = "";
char                      msg_timeout [] = "";
char                      msg_out_len [] = "";
char               msg_bad_devicename [] = "";
char                     msg_post_cnt [] = "";
char                msg_hyphen_prefix [] = "";
char                         msg_halt [] = "";
char                     msg_buf_size [] = "";
char                         msg_post [] = "";
char                          msg_cnt [] = "";
char              msg_null_terminated [] = "";
char               msg_qscan_allocerr [] = "";
char                msg_hyphen_suffix [] = "";
char		      msg_invalid_cmd [] = "Invalid command\n";
char	     msg_cmm_wrn_presto_valid [] = "* Warning * Prestoserve module NVRAM valid\n";
char                 msg_no_file_slot [] = "";
char                       msg_sunday [] = "Sunday";
char	                   msg_monday [] = "Monday";
char	                  msg_tuesday [] = "Tuesday";
char	                msg_wednesday [] = "Wednesday";
char	                 msg_thursday [] = "Thursday";
char	                   msg_friday [] = "Friday";
char	                 msg_saturday [] = "Saturday";
char	                  msg_january [] = "January";
char	                 msg_february [] = "February";
char	                    msg_march [] = "March";
char	                    msg_april [] = "April";
char	                      msg_may [] = "May";
char	                     msg_june [] = "June";
char	                     msg_july [] = "July";
char	                   msg_august [] = "August";
char	                msg_september [] = "September";
char	                  msg_october [] = "October";
char	                 msg_november [] = "November";
char	                 msg_december [] = "December";
char msg_no_boot [] = "\nHalt Button is IN, BOOT NOT POSSIBLE\n\n";
char msg_cnsl_secure []   = "Console is secure. Please login.\n";
char msg_invld_psswd []   = "Invalid password\n";
char msg_val_err []       = "Validation error\n";
char msg_psswd_len_err [] = "Password length must be between %d and %d characters\n";
char msg_curr_psswd_err[] = "Current password incorrect\n";
char msg_cmd_noparams[]   = "Set Password does not allow parameters on the command line\n";
char msg_noboot_params[]  = "Console is secure - parameters are not allowed.\n";
char msg_no_vldpsswd []   = "Secure not set.  Please set the password.\n";
char msg_pwd_cleared []   = "Password successfully cleared.\n";
char	msg_einval		[] = "invalid argument";		/* 7	*/
char msg_pdq_msg1	[] = "%s PortControlCommand failed\n";
char msg_pdq_msg2	[] = "%s StateExpt = 4 StateRcv = %1x \n";
char msg_pdq_msg3	[] = "%s No available transmit descriptor\n";
char msg_pdq_msg4	[] = "%s pdq_fmc_descr_m_fsb_error Status: %x\n";
char msg_pdq_msg5	[] = "%s pdq_state_k_halted\n";
char msg_pdq_msg6	[] = "%s pollCmdRspQueue Failed\n";
char msg_pdq_msg7	[] = "Statuscharsget error: %s: Status: %x Cmd_Type: %x\n";
char msg_pdq_msg8	[] = "Charsset: %s rsp->header.status: %x\n";
char msg_pdq_msg9	[] = "CountersGet1 charsset: %s rsp->header.status\n";
char msg_pdq_msg10	[] = "%s rsp->header.status error\n";
char msg_pdq_msg11	[] = "%s pdq_state_k_link_unavail\n";
char msg_pdq_msg12	[] = "%s StateExpt = 3 StateRcv = %1x \n";
char msg_pdq_msg13	[] = "%s softreset Failed\n";
char msg_pdq_msg14	[] = "Port %s Not Started Yet\n";
/* msg_buf is referenced and not defined */
/* msg_d_unsup_dev is referenced and not defined */
/* msg_ext_modify_dp is referenced and not defined */
/* msg_ext_sdtr is referenced and not defined */
/* msg_faulure is referenced and not defined */
/* msg_found_conflict is referenced and not defined */
/* msg_id_disc is referenced and not defined */
/* msg_identify is referenced and not defined */
/* msg_in is referenced and not defined */
/* msg_in_buf is referenced and not defined */
/* msg_in_continue is referenced and not defined */
/* msg_in_ext is referenced and not defined */
/* msg_in_reject is referenced and not defined */
/* msg_isa_exists is referenced and not defined */
/* msg_isa_noetyp is referenced and not defined */
/* msg_isa_noname is referenced and not defined */
/* msg_isa_notfound is referenced and not defined */
/* msg_lfu_banner is referenced and not defined */
/* msg_lfu_exit_error is referenced and not defined */
/* msg_lfu_exit_error_cont is referenced and not defined */
/* msg_lfu_firm_bad is referenced and not defined */
/* msg_lfu_firm_opn_err is referenced and not defined */
/* msg_lfu_list_header is referenced and not defined */
/* msg_lfu_no_firm is referenced and not defined */
/* msg_lfu_no_support is referenced and not defined */
/* msg_lfu_prompt is referenced and not defined */
/* msg_lfu_prompt_continue_y_n is referenced and not defined */
/* msg_lfu_prompt_y_n is referenced and not defined */
/* msg_lfu_update is referenced and not defined */
/* msg_lfu_update_failed is referenced and not defined */
/* msg_lfu_update_older_rev is referenced and not defined */
/* msg_lfu_update_warn1 is referenced and not defined */
/* msg_lfu_update_warn2 is referenced and not defined */
/* msg_lfu_verify is referenced and not defined */
/* msg_lfu_verify_bad is referenced and not defined */
/* msg_lfu_verify_good is referenced and not defined */
/* msg_nop2 is referenced and not defined */
/* msg_out is referenced and not defined */
/* msg_out_buf is referenced and not defined */
/* msg_out_short is referenced and not defined */
/* msg_permdev_nodel is referenced and not defined */
/* msg_permdev_nomod is referenced and not defined */
/* msg_plist is referenced and not defined */
/* msg_prefix_size is referenced and not defined */
/* msg_ptr is referenced and not defined */
/* msg_qh is referenced and not defined */
/* msg_reject is referenced and not defined */
/* msg_restore_dp is referenced and not defined */
/* msg_save_dp is referenced and not defined */
/* msg_succes is referenced and not defined */
/* msg_to_targ is referenced and not defined */
/* msg_unichck is referenced and not defined */
