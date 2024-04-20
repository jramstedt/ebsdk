/* file:	msgtxt.c
 *
 * Copyright (C) 1991 by
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
 * and  should  not  be  construed  as a commitment by digital equipment
 * corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by digital.
 */

/*
 *++
 *  FACILITY:
 *
 *      Alpha Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *	The actual text for the numerous messages in the Alpha console.
 *
 *	This file is run through a pre processor which generates the
 *	header file declaring all messages numbers external.
 *
 *  AUTHORS:
 *
 *      numerous
 *	jds - Jim Sawin
 *	tga - Tom Arnold
 *
 *  CREATION DATE:
 *  
 *      19-Sep-1990
 *                     
 *  MODIFICATION HISTORY:
 *
 *	ajb	11-Dec-1991	Removed previous comments to keep from
 *				"discovering"  old message numbers.
 *
 *--
 */

char		   msg_invalid_devname[] = "invalid device name: %s\n";
char		      msg_invalid_unit[] = "invalid unit for device\n";
char		      msg_invalid_slot[] = "invalid slot for device\n";
char		      msg_invalid_hose[] = "invalid hose for device\n";
char 		     msg_hose_sizing  [] = "Hose sizing error\n\n";
char		 msg_unk_bootdev_class[] = "unknown boot device class\n";
char	      msg_controller_not_found[] = "could not find controller for device %s\n";
char                     msg_overflow [] = "numeric value too large\n";
char                     msg_unicheck [] = "%s: multiple cpus can respond.\nChange affinity mask using SA command or &p operator.\n";
char                  msg_not_enabled [] = "processor not enabled to leave console mode\n";
char		   msg_not_in_console [] = "not modified - all processors must be in console mode\n";
char                  msg_shell_nocmd [] = "no shell command\n";
char                   msg_shell_noex [] = "not executable\n";
char                 msg_qscan_noqual [] = "unrecognized qualifiers\n";
char                   msg_qscan_conv [] = "qualifier conversion error\n";
char                   msg_qscan_misc [] = "qualifier value not found\n";
char                 msg_qscan_valerr [] = "qualifier conversion specification not recognized\n";
char            msg_poorly_formed_exp [] = "poorly formed regular expression\n";
char                msg_mem_adr_align [] = "start address not hexaword aligned\n";
char                msg_mem_len_align [] = "length not hexaword aligned\n";
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
char                     msg_bad_stat [] = "bad file status for file %s\n";
char                   msg_no_fp_slot [] = "no open file slot\n";
char                   msg_no_ip_slot [] = "no more inodes\n";
char                    msg_no_create [] = "can't create file\n";
char                  msg_file_in_use [] = "file is in use\n";
char                    msg_file_full [] = "file system is full\n";
char              msg_bad_access_mode [] = "invalid access mode\n";
char                 msg_bad_filename [] = "invalid file name\n";
char                  msg_no_infonext [] = "info/next fields not allowed\n";
char                        msg_nomsg [] = "no such message\n";
char                    msg_noprocess [] = "no such process\n";
char                  msg_termtimeout [] = "terminal timeout\n";
char                 msg_extra_params [] = "too many parameters\n";
char                msg_insuff_params [] = "insufficient parameters\n";
char                   msg_insuff_mem [] = "insufficient dynamic memory for a request of %d bytes\n";
char 			  msg_no_free [] = "Can't free memory\n";
char                msg_qual_conflict [] = "conflicting qualifiers\n";
char                msg_data_comp_err [] = "data compare error for %d bytes at location %08X from device %s\n";
char                     msg_exer_hd2 [] = "\npacket                                              IOs            elapsed  idle\n";
char                     msg_exer_hd3 [] = " size           IOs     bytes read  bytes written   /sec bytes/sec seconds  secs\n";
char                  msg_exer_counts [] = "%6d %12d %14d %14d %6d %9d %6d %6d\n";
char 		       msg_exer_finis [] = "\n%s exer completed\n";
char                   msg_bad_action [] = "illegal action string character\n";
char                   msg_expect_buf [] = "expected buffer contents: \n";
char                  msg_receive_buf [] = "\nreceived buffer contents: \n";
char		         msg_mem_comp [] = "* Memory error at address:%x  Wrote:%x Read:%x\n";
char            msg_illegal_operation [] = "illegal operation on driver %s\n";
char               msg_exdep_conflict [] = "conflicting qualifiers - command ignored\n";
char                 msg_exdep_badadr [] = "Illegal target address\n";
char               msg_exdep_badparam [] = "Too many command parameters\n";
char             msg_exdep_badincsize [] = "Illegal value for data size increment\n";
char               msg_exdep_outrange [] = "%s out of range\n";
char            msg_dep_param_missing [] = "Required parameters were not specified\n";
char               msg_dep_datatoobig [] = "Deposit data too large for specified data type\n";
char                  msg_dep_baddata [] = "Illegal data parameter\n";
char              msg_ex_devopenrderr [] = "Error opening device %s for read access\n";
char             msg_dep_devopenwrerr [] = "Error opening device %s for write access\n";
char             msg_exdep_devseekerr [] = "Error in seek to location %08X on device %s\n";
char                msg_ex_devreaderr [] = "Error in read of %d bytes at location %08X from device %s\n";
char              msg_dep_devwriteerr [] = "Error in write of %d bytes to location %08X on device %s\n";
char                   msg_ipr_noread [] = "IPR is not readable\n";
char                  msg_ipr_nowrite [] = "IPR is not writeable\n";
char                 msg_ipr_nonexist [] = "Illegal IPR\n";
char                        msg_no_vc [] = "no VC exists to %s\n";
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
char                 msg_mscp_no_conn [] = "no MSCP connection exists to %s\n";
char              msg_mscp_bad_status [] = "bad MSCP status (%d|%d) received\n";
char           msg_mscp_cmd_tmo_check [] = "MSCP command timeout, getting status from %s\n";
char           msg_mscp_cmd_tmo_fatal [] = "MSCP command timeout, fatal error on %s\n";
char             msg_mscp_no_progress [] = "MSCP command timeout, no progress on %s\n";
char             msg_mscp_bad_cmd_rsp [] = "bad MSCP command/response (%02x) from %s\n";
char           msg_mscp_bad_available [] = "failed to send Available to %s\n";
char              msg_mscp_bad_online [] = "failed to send Online to %s\n";
char                msg_mscp_bad_read [] = "failed to send Read to %s\n";
char               msg_mscp_bad_write [] = "failed to send Write to %s\n";
char                  msg_dup_no_conn [] = "no DUP connection exists to %s\n";
char               msg_dup_bad_status [] = "bad DUP status (%d|%d) received\n";
char            msg_dup_cmd_tmo_fatal [] = "DUP command timeout, fatal error on %s\n";
char              msg_dup_bad_cmd_rsp [] = "bad DUP command/response (%02x) from %s\n";
char              msg_dup_ill_msg_typ [] = "illegal Send Data (Immediate) message type (%d) received from %s\n";
char             msg_dup_bad_get_cont [] = "failed to send Get Controller Status to %s\n";
char            msg_dup_bad_exe_local [] = "failed to send Execute Local Program to %s\n";
char                 msg_dup_bad_send [] = "failed to send Send Data (Immediate) to %s\n";
char              msg_dup_bad_receive [] = "failed to send Receive Data to %s\n";
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
char              msg_rd_bad_validate [] = "RD driver failure (%d)\n";
char              msg_pi_bad_validate [] = "PI driver failure (%d)\n";
char                 msg_dyn_free_lnk [] = "bad free list detected at %08x\n";
char                  msg_dyn_adj_lnk [] = "bad adjacency list detected at %08x\n";
char                 msg_dyn_free_seq [] = "free list not in ascending order at %08x\n";
char                  msg_dyn_adj_seq [] = "adjacency list not sequential at %08x\n";
char                     msg_dyn_size [] = "non negative size at %08x\n";
char                  msg_dyn_badzone [] = "%08X is not a valid zone address.  Use dynamic to list valid zones.\n";
char		  msg_mem_len_gt_bs   [] = "Length cannot be larger than block size.";
char		  msg_start_gt_end    [] = "Start address cannot be larger than end address.";
char                         msg_noev [] = "%s does not exist\n";
char			 msg_d_z_warn [] = "-z will not safely malloc memory.  Continue [Y, (N)]? ";
char		      msg_d_no_malloc [] = "Unable to allocate memory of length %x at starting address %x\n";
char 		      msg_malloc_fail [] = "Malloc of %x bytes failed\n";
char		    msg_d_ext_err_exp [] = "Expected value:\t %08x";
char		    msg_d_ext_err_rcd [] = "Received value:\t %08x";
char                      msg_ev_prot [] = "%s protected from attempted operation\n";
char                   msg_ev_badname [] = "bad name - %s not created\n";
char                  msg_ev_badvalue [] = "bad value - %s not modified\n";
char              msg_destruct_prompt [] = "\nDestructive test specified.  Continue [Y, (N)]? ";
char                     msg_destruct [] = "diagnostic aborted because of destructive test\n";
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
char                   msg_d_validate [] = "External loopback error, no packet received";
char                msg_d_no_dev_name [] = "No device name was entered for IOB search\n";
char                   msg_d_inv_test [] = "Invalid test selection\n";
char                    msg_d_inv_grp [] = "Invalid group selection\n";
char               msg_d_inv_test_grp [] = "Invalid test selection for group entered\n";
char               msg_d_no_iob_found [] = "No IOB was found corresponding to device %s\n";
char			msg_d_exp_rcd [] = "Expected value:%x  Received value: %x\n";
char                 msg_d_ext_err_fa [] = "Failing addr:\t %x\n";
char                msg_d_ext_err_fa1 [] = "Failing addr:\t %x";
char               msg_d_ext_node_num [] = "Error detected by node:\t %d\n";
char                msg_d_shared_read [] = "Shared cache data read mismatch\n";
char                 msg_d_arith_trap [] = "Arithmetic exception encountered \n";
char                  msg_d_unx_excep [] = "Unexpected exception encountered\n";
char                   msg_d_ex_excep [] = "Expected exception did not occur\n";
char                 msg_d_sbew_excep [] = "Expected SBE on WR exception did not occur\n";
char                 msg_d_sber_excep [] = "Expected SBE on RD exception did not occur\n";
char                 msg_d_dbew_excep [] = "Expected DBE on WR exception did not occur\n";
char                 msg_d_dber_excep [] = "Expected DBE on RD exception did not occur\n";
char               msg_d_unexpec_mchk [] = "Unexpected Machine Check\n";
char              msg_d_data_comp_err [] = "Data compare error\n";
char                msg_d_timeout_err [] = "Synchronization timeout occurred\n";
char                  msg_d_bad_group [] = "D_GROUP set to %s -- No tests found in that group\n";
char		          msg_d_scram [] = "Script RAM failure\n";
char		   msg_d_scram_parity [] = "Script RAM initialize parity failure\n";
char		msg_d_scram_data_line [] = "Script RAM data line failure\n";
char		 msg_d_scram_adr_line [] = "Script RAM cell or address line failure\n";
char		            msg_d_ncr [] = "Storage Bus %c failure\n";
char	     msg_d_ncr_host_data_line [] = "%s host data lines failure\n";
char	      msg_d_ncr_host_adr_line [] = "%s host address lines failure\n";
char		   msg_d_ncr_scsi_bus [] = "%s SCSI bus failure\n";
char		    msg_d_ncr_dma_err [] = "DMA failure, script source addr:%x, destination addr:%x\n";
char		  msg_d_ncr_dstat_sir [] = "DMA completed but DSTAT:SIR is stuck\n";
char		         msg_d_ncr_id [] = "%s set bus ID failure\n";
char 		    msg_d_ncr_bus_arb [] = "Warning: %s, loopback connector attached  OR\nSCSI bus failure, could not acquire bus; Control Lines:%x Data lines:%x\n";
char		  msg_d_ncr_cant_test [] = "Warning: %s not tested\n";
char		      msg_d_load_only [] = "%s test %d is only run in the 'loadable' console environment\n";
char             msg_ex_shutdown_fail [] = "%s port shutdown failed.\n";
char            msg_ex_reset_retrying [] = "%s node reset failed - retrying...\n";
char                msg_ex_reset_fail [] = "%s node reset failed.\n";
char              msg_ex_init_timeout [] = "%s port initialization failed - timeout on XPST_uninitialized\n";
char                 msg_ex_init_fail [] = "%s port initialization failed.\n";
char                msg_ex_buflen_err [] = "%s:Buffer length error.\n";
char                msg_ex_addr_inval [] = "%s:Buffer address invalid.\n";
char            msg_ex_addr_xlate_err [] = "%s:Address translation error.\n";
char              msg_ex_bufxfer_fail [] = "%s:Buffer transfer failed.\n";
char                 msg_ex_inval_cmd [] = "%s:Invalid command.\n";
char                 msg_ex_inval_opc [] = "%s:Invalid opcode.\n";
char              msg_ex_excd_srcaddr [] = "%s:Invalid command - exceeded maximum number of source addresses\n";
char              msg_ex_excd_dstaddr [] = "%s:Invalid command - exceeded maximum number of destination/multicast addresses\n";
char                msg_ex_frmlen_err [] = "%s:Frame length error.\n";
char              msg_ex_loss_carrier [] = "%s:Transmit failed - loss of carrier.\n";
char            msg_ex_tx_retries_exh [] = "%s:Transmit failed - retries exhausted.\n";
char                 msg_ex_late_coll [] = "%s:Transmit failed - late collision.\n";
char                msg_ex_tx_timeout [] = "%s:Transmit failed - transmit timeout.\n";
char                   msg_ex_tx_fail [] = "%s:Transmit failed.\n";
char          msg_ex_frmlen_incnsstnt [] = "%s:Frame length inconsistent.\n";
char             msg_ex_frmlen_toobig [] = "%s:Frame length > 1518 bytes (1518 bytes delivered).\n";
char                msg_ex_rx_crc_err [] = "%s:Invalid packet - CRC error.\n";
char               msg_ex_invalid_pkt [] = "%s:Invalid packet.\n";
char               msg_ex_cmd_timeout [] = "%s command timeout!\n";
char                msg_ex_err_ustart [] = "Error sending USTART command to %s.\n";
char                 msg_ex_err_ustop [] = "Error sending USTOP command to %s.\n";
char                msg_ex_err_rdcntr [] = "Error sending RDCNTR command to %s.\n";
char                  msg_ex_err_mode [] = "Attempt to set %s into an illegal mode.\n";
char                 msg_ex_err_param [] = "Error sending PARAM command to %s.\n";
char             msg_ex_err_n_env_var [] = "Couldn't read env variable: %s\n";
char                   msg_ex_noerror [] = "%s:No error.\n";
char                  msg_ex_stfailed [] = "%s:Module selftest failed.\n";
char               msg_ex_bad_pdb_adr [] = "%s:Invalid PDB address.\n";
char               msg_ex_bad_pdb_fld [] = "%s:Invalid PDB field.\n";
char                msg_ex_bad_eeprom [] = "%s:XNA EEPROM not valid.\n";
char                msg_ex_not_uninit [] = "%s:Initialization attempted when port not in uninitialized state.\n";
char               msg_ex_bad_cmdring [] = "%s:Invalid command ring.\n";
char               msg_ex_bad_rcvring [] = "%s:Invalid receive ring.\n";
char                   msg_ex_pwrfail [] = "%s:Power failure.\n";
char            msg_no_kbd_use_serial [] = "Keyboard error; using serial port terminal\n";
char               msg_ex_fw_excption [] = "%s:Unexpected XNA firmware exception.\n";
char                  msg_ex_xmi_fail [] = "%s:Unrecoverable XMI failure.\n";
char                  msg_ex_fatal_fw [] = "%s:Fatal XNA firmware internal error.\n";
char           msg_ex_keep_alive_fail [] = "%s:Fatal XNA firmware error - keep-alive counter.\n";
char               msg_ex_fw_upd_done [] = "%s:XNA firmware update complete.\n";
char                    msg_init_port [] = "Initializing driver %s.\n";
char                  msg_port_noerror[] = "%s: No error.\n";
char                  msg_port_inited [] = "Driver %s initialized.\n";
char          msg_port_reset_retrying [] = "%s node reset failed - retrying...\n";
char              msg_port_reset_fail [] = "%s node reset failed.\n";
char            msg_port_xpst_timeout [] = "%s port initialization failed - timeout on XPST_uninitialized\n";
char               msg_port_init_fail [] = "%s port initialization failed.\n";
char           msg_port_shutdown_fail [] = "%s port shutdown failed.\n";
char             msg_port_cmd_timeout [] = "%s command timeout!\n";
char               msg_port_err_param [] = "Error sending PARAM command to %s.\n";
char              msg_port_err_rdcntr [] = "Error sending RDCNTR command to %s.\n";
char              msg_port_err_ustart [] = "Error sending USTART command to %s.\n";
char               msg_port_err_ustop [] = "Error sending USTOP command to %s.\n";
char                msg_port_err_mode [] = "Attempt to set %s into an illegal mode.\n";
char           msg_port_err_n_env_var [] = "Couldn't read env variable: %s\n";
char         msg_port_state_resetting [] = "Resetting";
char            msg_port_state_uninit [] = "Uninitialized";
char              msg_port_state_init [] = "Initialized";
char           msg_port_state_running [] = "Running";
char             msg_port_state_maint [] = "Maintenance";
char            msg_port_state_halted [] = "Halted";
char        msg_port_fail_enter_state [] = "%s: Failed to enter %m state.\n";;
char		    msg_port_stfailed [] = "%s: Port init failed: adapter failed self-test.\n";
char             msg_port_bad_pdb_adr [] = "%s: Port init failed: invalid PDB address.\n";
char             msg_port_bad_pdb_fld [] = "%s: Port init failed: invalid PDB field.\n";
char              msg_port_bad_eeprom [] = "%s: EEPROM contents are invalid.\n";
char              msg_port_not_uninit [] = "%s: Initialization attempted when port not in the Uninitialized state.\n";
char             msg_port_bad_cmdring [] = "%s: Invalid command ring.\n";
char             msg_port_bad_rcvring [] = "%s: Invalid receive ring.\n";
char                msg_port_xmi_fail [] = "%s: Unrecoverable XMI failure, including memory error.\n";
char                 msg_port_pwrfail [] = "%s: Power failure.\n";
char                msg_port_fatal_fw [] = "%s: Fatal firmware internal error.\n";
char            msg_port_nhalt_issued [] = "%s: Node Halt issued.\n";
char                msg_port_fatal_hw [] = "%s: Fatal hardware internal error.\n";
char             msg_port_inval_state [] = "%s: Invalid adapter state.\n";
char               msg_port_wd_to_err [] = "%s: Watchdog timeout error.\n";
char                 msg_port_bus_flt [] = "%s: Bus Fault.\n";
char          msg_port_unexpected_int [] = "%s: Unexpected interrupt - port state = %m.\n";
char         msg_port_unexplained_int [] = "%s: Unexplained interrupt!\n";
char              msg_port_buflen_err [] = "%s: Buffer length error.\n";
char            msg_port_bufxfer_fail [] = "%s: Buffer transfer failed.\n";
char               msg_port_inval_cmd [] = "%s: Invalid command.\n";
char               msg_port_inval_opc [] = "%s: Invalid opcode.\n";
char                msg_ez_err_xmt_to [] = "Transmit time out\n";
char             msg_ez_err_n_xmt_des [] = "Transmits hung, check termination.\n";
char              msg_ez_err_setup_to [] = "Setup frame timed out\n";
char               msg_ez_err_diag_to [] = "Diagnostic framed time out\n";
char             msg_ez_err_setup_des [] = "Setup descriptor error. SDES0: %08x\n";
char               msg_ez_err_init_tl [] = "Initialization took to long\n";
char                  msg_d_self_test [] = "%s self test failed\n";
char             msg_ez_err_self_test [] = "XGEC self test failed. status: %01x\n";
char          msg_ez_err_unk_start_md [] = "Unknown start mode\n";
char             msg_ez_err_nisa_null [] = "Network station address is a Null address\n";
char          msg_ez_err_nisa_bad_chk [] = "Network station address ROM has a bad checksum\n";
char            msg_ez_err_nisa_n_rpt [] = "Network station address is not repeated\n";
char              msg_ez_err_bad_patt [] = "Pattern ff0055aa does not exist in station address ROM\n";
char              msg_ez_inf_dev_spec [] = "\nDEVICE SPECIFIC:\n";
char               msg_ez_inf_mop_blk [] = "\nMOP BLOCK:\n";
char               msg_ez_inf_net_lst [] = " Network list size: %d\n";
char             msg_ez_err_n_env_var [] = "Couldn't read env variable: %s\n";
char            msg_ez_err_mode_n_chg [] = "Mode not changed\n";
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
char		      msg_ez_err_pdes [] = "Processing a descriptor the XGEC owns\n";
char		       msg_ez_inf_001 [] = " TI: %d RI: %d RU: %d ME: %d TW: %d RW: %d BO: %d\n";
char		       msg_ez_inf_002 [] = " HF: %d UF: %d TN: %d LE: %d TO: %d RWT: %d RHF: %d  TC: %d\n";
char		       msg_ez_inf_003 [] = "\nPORT INFO:\n";
char		       msg_ez_inf_004 [] = "tx full: %d tx index in: %d tx index out: %d\n";
char		       msg_ez_inf_005 [] = "rx index in: %d\n";
char		       msg_ez_inf_006 [] = "NICSRS:\n";
char		       msg_ez_inf_007 [] = " csr0:  %08x ";
char		       msg_ez_inf_008 [] = " csr1:  %08x ";
char		       msg_ez_inf_009 [] = " csr2:  %08x ";
char		       msg_ez_inf_010 [] = " csr3:  %08x\n";
char		       msg_ez_inf_011 [] = " csr4:  %08x ";
char		       msg_ez_inf_012 [] = " csr5:  %08x ";
char		       msg_ez_inf_013 [] = " csr6:  %08x ";
char		       msg_ez_inf_014 [] = " csr7:  %08x\n";
char		       msg_ez_inf_015 [] = " csr9:  %08x ";
char		       msg_ez_inf_016 [] = " csr10: %08x ";
char		       msg_ez_inf_017 [] = " csr11: %08x ";
char		       msg_ez_inf_018 [] = " csr12: %08x\n";
char		       msg_ez_inf_019 [] = " csr13: %08x ";
char		       msg_ez_inf_020 [] = " csr14: %08x ";
char		       msg_ez_inf_021 [] = " csr15: %08x\n";
char			 msg_mbx_busy [] = "\n*** unable to write to mailbox pointer register\n";
char           msg_mbx_prev_trans_t_o [] = "\n*** previous mailbox IO timeout, don flag not set\n";
char                msg_mbx_trans_t_o [] = "\n*** mailbox IO timeout, done flag not set on hose %d\n";
char                msg_mbx_trans_err [] = "\n*** Mailbox transaction error, ";
char                   msg_mbx_status [] = "mailbox status %08x %08x\n";
char                msg_mbx_ctr_rbadr [] = "mailbox control %08x %08x,  remote bus address %08x %08x\n";
char                    msg_mbx_rdata [] = "mailbox read data field %08x %08x\n";
char                    msg_mbx_wdata [] = "mailbox write data field %08x %08x\n";
char 		     msg_mbx_err_info [] = "device = %s   pcb = %x (%s)    mailbox = %x\n";
char 		       msg_lbus_reset [] = "lbus has just been reset & enabled\n";
char		      msg_ndl_err_001 [] = "Invalid enet address: %s\n";
char		      msg_ndl_err_002 [] = "*** Error (%s), ";
char		      msg_ndl_err_003 [] = "*** Error, ";
char              msg_mop_err_n_dl_cd [] = "Unrecognized D/L message code from: %02x-%02x-%02x-%02x-%02x-%02x\n";
char                msg_mop_inf_cntrs [] = "Counters\n";
char              msg_mop_err_n_cn_cd [] = "Unrecognized Console message from : %02x-%02x-%02x-%02x-%02x-%02x\n";
char         msg_mop_wrn_out_o_seq_ml [] = "Out of sequence message (MEM_LOAD)\n";
char               msg_mop_det_sv_msg [] = "Saved: %d message: %d\n";
char       msg_mop_wrn_out_o_seq_mlxf [] = "Out of sequence message (MEM_LOAD_W_XFER)\n";
char       msg_mop_wrn_out_o_seq_plxf [] = "Out of sequence message (PARAM_LOAD_W_XFER)\n";
char             msg_mop_err_n_net_fl [] = "Couldn't read the network file\n";
char            msg_mop_err_nr_net_fl [] = "Couldn't write the network file\n";
char            msg_mop_det_good_data [] = "Good data:\n";
char             msg_mop_det_bad_data [] = "Bad data:\n";
char             msg_mop_err_mem_comp [] = "Memory compare error\n";
char             msg_mop_inf_tgt_name [] = "Target name: %s\n";
char               msg_mop_inf_ld_com [] = "Network load complete.\n";
char             msg_mop_inf_hst_name [] = "Host name: %s\n";
char            msg_mop_inf_retry_net [] = "Retrying...\n";
char             msg_mop_err_rd_lp_fl [] = "Read error loop_file\n";
char            msg_mop_err_wt_net_db [] = "Write of network database failed\n";
char             msg_mop_wrn_pkt_late [] = "packet came too late\n";
char         msg_mop_wrn_n_msg_in_slt [] = "No valid message in slot: %d\n";
char            msg_mop_err_n_pre_msg [] = "Couldn't get a preallocated message\n";
char                msg_mop_err_lp_to [] = "Mop loop message timed out from: %02x-%02x-%02x-%02x-%02x-%02x\n";
char               msg_mop_err_lp_to1 [] = "*** List index: %d received count: %d expected count %d\n\n";
char              msg_mop_det_msg_src [] = "Message source: ";
char            msg_mop_err_rply_size [] = "Reply size isn't correct\n";
char              msg_mop_det_mos_rps [] = "mo size %d reply size: %d\n";
char            msg_mop_det_msg_n_vld [] = "Message entry not valid\n";
char                msg_mop_err_b_src [] = "Source wasn't correct\n";
char              msg_mop_err_b_index [] = "Index too large\n";
char             msg_mop_inf_tgt_addr [] = "Target address: %02x-%02x-%02x-%02x-%02x-%02x\n";
char             msg_mop_inf_hst_addr [] = "Host address: %02x-%02x-%02x-%02x-%02x-%02x\n\n";
char     msg_moprc_err_rc_already_res [] = "Remote console already reserved.\n";
char           msg_moprc_err_rc_n_res [] = "Remote console is not reserved\n";
char             msg_moprc_inf_rc_rel [] = "\nRemote console released from: %02x-%02x-%02x-%02x-%02x-%02x\n";
char           msg_moprc_err_rc_b_ver [] = "Incorrect remote console verification.\n";
char             msg_moprc_inf_rc_res [] = "\nRemote console reserved from: %02x-%02x-%02x-%02x-%02x-%02x\n";
char           msg_moprc_err_cd_kl_sh [] = "Couldn't kill RC shell.\n";
char                msg_moplp_err_crc [] = "CRC error: rec: %d saved crc: %08x rcvd crc: %08x\n";
char                msg_moplp_err_run [] = "loopback test already running on port %s\n";
char                  msg_mop_err_001 [] = "FDDI not supported\n";
char			  msg_net_001 [] = "Opening file: %s\n";
char			  msg_net_002 [] = "loading at address: %08x\n";
char			  msg_net_003 [] = "File not loaded.\n";
char			  msg_net_004 [] = "\nLoad complete!\n";
char			  msg_net_005 [] = "Image loaded at address: %08x\n";
char			  msg_net_006 [] = "Image size: %d\n";
char			  msg_net_007 [] = "Target Name: %s\n";
char			  msg_net_008 [] = "Target address: %02x-%02x-%02x-%02x-%02x-%02x\n";
char			  msg_net_009 [] = "Host name: %s\n";
char			  msg_net_010 [] = "Host address: %02x-%02x-%02x-%02x-%02x-%02x\n";
char			msg_moprc_001 [] = "proc=%08X verif_h=%08X  verif_l=%08X \n";
char                  msg_ea_err_n_io [] = "Couldn't find an io module in the system\n";
char                 msg_ea_inf_ti_ri [] = " TI: %d RI: %d\n";
char                 msg_vmem_rsvdreg [] = "Virtual address %08X is in the reserved region\n";
char                  msg_vmem_lenvio [] = "Length violation found while converting virtual address %08X\n";
char               msg_vmem_ptenonsys [] = "Process Page Table Entry at %08X is not in system space\n";
char                msg_vmem_invalpfn [] = "Invalid Page Table Entry for virtual address %08X\n";
char                  msg_vmem_accvio [] = "Access violation found for virtual address %08X\n";
char            msg_vmem_devopenrderr [] = "Error opening pmem device for read access from vmem driver\n";
char            msg_vmem_devopenwrerr [] = "Error opening pmem device for write access from vmem driver\n";
char              msg_vmem_devseekerr [] = "Error seeking to physical address %08X from vmem driver\n";
char                          msg_why [] = "why not?\n";
char                 msg_wait_sem_err [] = "error: wait condition still true after semaphore %s has been posted!\n";
char                   msg_invl_inode [] = "inconsistency in %s_validate of inode entry\n";
char                     msg_invl_ddb [] = "inconsistency in %s_validate of ddb entry\n";
char                     msg_dmag_mis [] = "missing qualifier(s): %s\n";
char                     msg_dmag_inv [] = "%s : invalid dma command\n";
char                     msg_dmag_amb [] = "%s : ambiguous dma command\n";
char           msg_dmag_ctx_open_fail [] = "error: cannot open dma_gen context file %s\n";
char                msg_dmag_link_err [] = "warning -- dma_state %04X:\nno write buffer in linked dma_state %04X\n";
char             msg_dmag_link_bc_err [] = "warning -- dma_state %04X:\nbyte count (%d) != byte count (%d) of linked state %04X\n";
char             msg_dmag_link_ld_err [] = "warning -- dma_state %04X:\nlane_deselect (%02X) != lane_deselect (%02X) of linked state %04X\n";
char             msg_dmag_insuff_pram [] = "warning - dma_state %04X:\ninsufficient packet ram available for a request of %d longwords\n";
char             msg_dmag_insuff_data [] = "warning - dma_state %04X:\ndata for crc generation (%d bytes) < transaction byte_count (%d bytes)\n";
char                 msg_dmag_null_bc [] = "error:  invalid beat_count (%d)  - minimal beat_count for read transaction = 2 \n";
char                     msg_dma_mode [] = "dma in %s mode \n";
char                msg_dma_inv_trans [] = "invalid transition : dma in %s mode \n";
char          msg_dma_not_interactive [] = "dma not in INTERACTIVE mode\n";
char                  msg_dma_started [] = "dma started\n";
char          msg_dma_already_started [] = "dma already started\n";
char                  msg_dma_stopped [] = "dma stopped\n";
char          msg_dma_already_stopped [] = "dma already stopped\n";
char           msg_fbe_master_disable [] = "invalid command while master disable\n";
char                       msg_hlt_hw [] = "operator initiated halt\n";
char                     msg_hlt_ksnv [] = "kernel stack not valid halt\n";
char                     msg_hlt_scbb [] = "invalid SCBB\n";
char                     msg_hlt_ptbr [] = "invalid PTBR\n";
char                       msg_hlt_sw [] = "HALT instruction executed\n";
char                   msg_hlt_dblerr [] = "double error halt\n";
char                 msg_hlt_pal_mchk [] = "machine check while in PAL mode\n";
char              msg_cdp_nn_too_long [] = "warning -- Node Name should not exceed 6 characters\n";
char             msg_cdp_nn_not_alpha [] = "warning -- Node Name should begin with A-Z\n";
char             msg_cdp_nn_not_alnum [] = "warning -- Node Name should contain only A-Z and 0-9\n";
char                 msg_cdp_ac_value [] = "warning -- Allocation Class should be between 0 and 255\n";
char                 msg_cdp_un_value [] = "warning -- Unit Number should be between 0 and 4095\n";
char                 msg_cdp_no_write [] = "warning -- changes will not be saved\n";
char        msg_sethost_not_supported [] = "target %s does not support SET HOST\n";
char              msg_sethost_timeout [] = "remote host response timeout error\n";
char                    msg_no_device [] = "device %s not found\n";
char               msg_missing_device [] = "device name or port name must be given\n";
char              msg_dev_unsupported [] = "Device %s not supported\n";
char                     msg_pkz_timo [] = "%s: adapter / command timed out\n";
char               msg_pkz_wrongstate [] = "%s: adapter is in the wrong state\n";
char                   msg_pkz_badreg [] = "%s: internal error - bad register code\n";
char                   msg_pkz_toobig [] = "%s: data transfer too big\n";
char                msg_pkz_illopcode [] = "%s: illegal operation code\n";
char                   msg_pkz_badarg [] = "%s: bad argument to I/O call\n";
char                   msg_pkz_badcmd [] = "%s: illegal command packet\n";
char                    msg_pkz_nobuf [] = "%s: could not allocate command buffer\n";
char                      msg_pkz_rip [] = "%s: Adapter reset in progress\n";
char                msg_fe_pgm_banner [] = "\t\t\tFEPROM UPDATE UTILITY\n";
char                   msg_fe_caution [] = "\t\t\t-----> CAUTION <-----\n";
char                  msg_fe_caution2 [] = "\tEXECUTING THIS PROGRAM WILL CHANGE YOUR CURRENT ROM!\n\n";
char                msg_fe_pgm        [] = "\t\t\tFEPROM UPDATE UTILITY\n\t\t\t-----> CAUTION <-----\n\tEXECUTING THIS PROGRAM WILL CHANGE YOUR CURRENT ROM!\n\n";
char                msg_fe_areyousure [] = "Do you really want to continue [Y/N] ? : ";
char                    msg_noupdates [] = "No updates performed\n";
char		      msg_pleaseyesno [] = "Please answer yes or no\n";
char                     msg_diffsize [] = "The loaded image is not the same size (%d) as the target device (%d)\n";
char                    msg_conscheck [] = "Consistency checks failed, continuing anyway.\n";
char                   msg_goodupdate [] = "Update successful\n";
char                    msg_badupdate [] = "Update encountered errors\n";
char                 msg_fe_usr_abort [] = "\nUser aborted program\n\n";
char                    msg_fe_no_int [] = "\n\tDO NOT ATTEMPT TO INTERRUPT PROGRAM EXECUTION!\n";
char              msg_fe_lose_opstate [] = "\tDOING SO MAY RESULT IN LOSS OF OPERABLE STATE.\n\n";
char                   msg_fe_sev_min [] = "The program will take at most several minutes.\n";
char               msg_fe_prog_unipgm [] = "Starting uniform programming to 0's of block...\n\t";
char                msg_fe_prog_erase [] = "Erasing block...\n\t";
char               msg_fe_prog_reprog [] = "Programming block...\n\t";
char             msg_fe_block_address [] = "%05x  ";
char                msg_fe_loop_count [] = "\n\tRepeat count (hex): %08x\n";
char                 msg_fe_wrong_sig [] = "Signatures incompatible.  Expected sig: %s, Boot image sig: %s\n";
char               msg_fe_pgm_success [] = "\nFEPROM update was successful!\n";
char                   msg_fe_too_big [] = "\nNew ROM image too big !\n";
char                msg_fe_bad_chksum [] = "Bad Checksum in byte %d\n";
char                msg_fe_unipgm_err [] = "\nROM uniform programming error\n";
char                 msg_fe_erase_err [] = "\nROM erase error (%d error count)\n";
char                   msg_fe_pgm_err [] = "\nROM programming error (%d error count)\n";
char                 msg_fe_spec_file [] = "must specify filename or target and version\n";
char                msg_fe_bad_device [] = "\nfirmware upgrade for device %s is not supported\n";
char                msg_fe_verify_err [] = "\nfirmware upgrade verify error\n";
char                msg_fe_total_errs [] = "\nerror count : %d\n";
char                   msg_rb_err_fmt [] = "%s incorrect format (pp-vvvvvvvv-vvvvvvvv) - %s not modified\n";
char                  msg_rb_err_enbl [] = "%s rm boot message enable - %s not modified\n";
char              msg_rb_err_pwd_fail [] = "error - %s rm boot password initialization failed!\n";
char              msg_rb_err_pwd_init [] = "rm boot password is not initialized - %s not modified\n";
char             msg_rb_err_fltr_init [] = "%s filter is not initialized - %s not modified\n";
char              msg_rb_err_inv_fltr [] = "%s is in inverse filtering mode - %s not modified\n";
char               msg_rb_err_pr_fltr [] = "%s is in promiscuous mode - %s not modified\n";
char                   msg_rb_inc_pwd [] = "incorrect password format (pp-vvvvvvvv-vvvvvvvv)\n";
char                  msg_not_swapped [] = "not all bytes swapped\n";
char                msg_mop_inf_reply [] = "Loop Reply from: %02x-%02x-%02x-%02x-%02x-%02x\n";
char		       msg_ci_bad_path[] = "%s: CI failure on path %c failure to node %d\n";
char                  msg_ext_message [] = "";
char                        msg_input [] = "";
char                          msg_mod [] = "";
char                 msg_shell_nopipe [] = "";
char                       msg_in_len [] = "";
char                  msg_buf_cmp_err [] = "";
char                    msg_qscan_eol [] = "";
char                   msg_dma_in_use [] = "";
char                   msg_csr_in_use [] = "";
char                       msg_number [] = "";
char                          msg_iec [] = "";
char                     msg_queue_to [] = "";
char                   msg_out_offset [] = "";
char                          msg_imh [] = "";
char                    msg_in_offset [] = "";
char                        msg_ctrlc [] = "";
char                       msg_nl_ddb [] = "";
char            msg_lt_brckt_expected [] = "";
char                          msg_ics [] = "";
char                         msg_loop [] = "";
char                         msg_node [] = "";
char                          msg_tmb [] = "";
char                   msg_scsi_retry [] = "";
char                 msg_exdep_badgpr [] = "";
char           msg_mop_err_cd_kl_rc_t [] = "";
char                          msg_rem [] = "";
char                msg_ilv_badsyntax [] = "";
char             msg_exdep_devreaderr [] = "";
char                         msg_size [] = "";
char                      msg_wait_to [] = "";
char                   msg_dyn_poison [] = "";
char                         msg_rcvd [] = "";
char           msg_destructive_prompt [] = "";
char                      msg_timeout [] = "";
char                     msg_read_err [] = "";
char                      msg_out_len [] = "";
char               msg_bad_devicename [] = "";
char                     msg_nl_inode [] = "";
char                     msg_post_cnt [] = "";
char                msg_hyphen_prefix [] = "";
char                         msg_halt [] = "";
char                     msg_lost_arb [] = "";
char                     msg_buf_size [] = "";
char                  msg_buf_cmp_ext [] = "";
char                 msg_ez_err_n_dev [] = "";
char                         msg_post [] = "";
char                          msg_cnt [] = "";
char                         msg_list [] = "";
char              msg_null_terminated [] = "";
char               msg_qscan_allocerr [] = "";
char                msg_hyphen_suffix [] = "";

/* Parser messages */
char                   msg_par_invnbr [] = "Invalid number\n";
char                      msg_par_itl [] = "Input too long\n";
char                    msg_par_keytl [] = "keyword too long\n";
char                     msg_par_limd [] = "Low limit = %d, High limit = %d\n";
char                     msg_par_limx [] = "Low limit = %x, High limit = %x\n";
char                    msg_par_nbror [] = "Number out of range\n";
char                   msg_par_nohelp [] = "No additional help available\n";
char		   msg_invalid_option [] = "Invalid command option\n";
char		      msg_invalid_cmd [] = "Invalid command\n";
char		msg_illegal_cmd_syntx [] = "Illegal command syntax\n";

/* Cobra Memory messages */
char                   msg_cmm_badram [] = "";
char                 msg_cmm_bad_bank [] = "";
char                     msg_cmm_skip [] = "";
char		        msg_cmm_start [] = "Starting Memory Diagnostics\n";
char		     msg_cmm_complete [] = "Memory Diagnostics completed\n";
char		    msg_cmm_brds_enab [] = "%d boards enabled\n";
char		  msg_cmm_diag_bypass [] = "Memory Diagnostics bypassed\n";  
char		       msg_cmm_presto [] = "Prestoserve module in slot %d\n";
char	     msg_cmm_wrn_presto_valid [] = "* Warning * Prestoserve module NVRAM valid\n";
char		 msg_cmm_presto_clean [] = "Prestoserve module NVRAM not valid\n";
char	      msg_cmm_err_presto_batt [] = " NVRAM battery failure\n";
char		    msg_cmm_test_cmic [] = "Testing CMIC on Memory Module %x\n";
char		    msg_cmm_sbuff_off [] = "Turning off the stream buffers\n";
char		     msg_cmm_sbuff_on [] = "Turning on the stream buffers\n";
char		msg_cmm_test_cmm_bank [] = "Testing Memory Module %x bank %x\n";
char		  msg_cmm_test_cmm_mb [] = "Testing 1st %dMB(s) on memory module %x\n"; 
char		msg_cmm_test_cmm_para [] = "Testing all memory banks in parallel\n";
char		    msg_cmm_test_bank [] = "Testing Memory bank %x\n";
char		     msg_cmm_mem_size [] = "Memory size = %x\n";
char	      msg_cmm_config_mem_size [] = "Configured memory size = %x\n";
char	      msg_cmm_cons_lrgr_cache [] = "* Warning * Console image larger than Bcache\n";
char	      msg_cmm_mem_config_skip [] = "Memory Configuration skipped\n";
char	             msg_cmm_init_mem [] = "Initializing all available memory\n";
char	            msg_show_mem_hrd1 [] = "Module   Size    Base Addr   Intlv Mode  Intlv Unit\n";
char	            msg_show_mem_hrd2 [] = "------   -----   ---------   ----------  ----------\n";
char		    msg_show_mem_frmt [] = "  %d      %3dMB   %08x      %d-Way         %d\n";
char	     msg_show_mem_presto_frmt [] = "  %d      %3dMB   %08x      %d-way      NVRAM";
char	             msg_presto_valid [] = " - valid\n";
char	         msg_presto_not_valid [] = " - not valid\n";
char		msg_show_mem_brd_fail [] = "  %d      %3dMB   Failed testing\n";
char	 msg_show_mem_presto_brd_fail [] = "  %d      %3dMB   NVRAM Failed testing\n";
char	   msg_show_mem_brd_notconfig [] = "  %d      %3dMB   Not Configured\n";
char	msg_show_mem_presto_notconfig [] = "  %d      %3dMB   NVRAM Not Configured\n";
char	    msg_show_mem_brd_not_inst [] = "  %d              Not Installed\n";
char	         msg_show_mem_sb_hrd1 [] = "Module   Size    Base Addr   Intlv Mode  Intlv Unit  Status\n";
char	         msg_show_mem_sb_hrd2 [] = "------   -----   ---------   ----------  ----------  ------\n";
char		 msg_show_mem_sb_frmt [] = "  %d      %3dMB   %08x      %d-Way         %d       ";
char	     msg_show_mem_sb_brd_fail [] = "  %d      %3dMB                                       Failed\n";
char	msg_show_mem_sb_brd_notconfig [] = "  %d      %3dMB                                       Not Configured\n";
char	 msg_show_mem_sb_brd_not_inst [] = "  %d                                                  Not Installed\n";
char	       msg_show_mem_bad_pages [] = "Total Bad Pages %d\n";
char	          msg_dram_err_exceed [] = "*** Error DRAM %d in bank %d on module %d exceeded threshold\n";
char		  msg_bank_err_exceed [] = "*** Error bank %d on module %d exceeded threshold\n";
char		   msg_brd_err_exceed [] = "*** Error module %d exceeded threshold\n";
char            msg_cmm_err_banks_bad [] = "*** Error %d bad banks on module %d\n";
char		     msg_cmm_fail_num [] = "Failure Number: %d\n"; 
char		     msg_cmm_fail_brd [] = "Failing Board:  %d\n";
char	     msg_cmm_fail_addr_offset [] = "Failing RAM Address Offset: %x\n";
char		 msg_cmm_fail_ram_num [] = "Failing RAM Number:         ";
char		 msg_cmm_fail_dev_num [] = "Failing Device Number:      ";
char		msg_cmm_fail_bit_mask [] = "Failing Bitmask:            %04b\n";
char		msg_cmm_fail_err_flag [] = "Multi-bit Error Flag:       %b\n";
char		msg_cmm_fail_err_type [] = "Error Type                  %02b\n";
char	    msg_cmm_fail_edc_patt_num [] = "Pattern Number %d\n";
char	         msg_cmm_err_edc_data [] = "*** Error EDC data failure\n";
char		    msg_cmm_d_exp_rcd [] = "Expected value:%08x%08x  Received value: %016x\n";
char	        msg_cmm_err_edc_latch [] = "*** Error %s latch data failure\n";
char	          msg_cmm_err_edc_int [] = "*** Error Interrupt failure\n";

char                 msg_no_file_slot [] = "";
char			msg_mopdl_001 [] = "";
char                    msg_fbus_busy [] = "fbus busy\n";
                                         
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
char		      msg_sz_mismatch [] = "Buffer counts differ - buf1:%d, buf2:%d, location:%x\n";
char                     msg_fbus_nxm [] = "non-existent F-bus address\n";

/* PSC Command Messages */
char	                msg_inv_param [] = "\nInvalid parameter specified\n";
char                   msg_devreaderr [] = "Error in read from device %s\n";
char                msg_psc_statuserr [] = "\nPSC command status error status = %02x\n";
char                msg_psc_eventlogh [] = "\nPSC Event Log\n";
char                msg_psc_eventloge [] = "PSC Event %02x\n";
char               msg_psc_statushead [] = "\nPSC Model %c%c%c%c%c-%c%c Version: %c%d.%d\n\n";
char              msg_psc_powerstatus [] = "Power Status:\n";
char               msg_psc_tempstatus [] = "Temperature Sensor: %m\n";
char                           msg_ok [] = "ok";
char                       msg_failed [] = "Failed";
char                     msg_psc_temp [] = "Temperature: ";
char                   msg_psc_greenz [] = "Green Zone\n";
char                  msg_psc_yellowz [] = "Yellow Zone\n";
char                     msg_psc_redz [] = "Red Zone\n";
char               msg_psc_invalid_ev [] = "Invalid(%02x)\n";
char                msg_psc_upsstatus [] = "\nUPS Status:\n";
char               msg_psc_upspresent [] = "\nUPS%mpresent\n";
char               msg_psc_upsavai    [] = "\nUPS%mavailable\n";
char                    msg_psc_blank [] = " ";
char                      msg_psc_not [] = " not ";
char                      msg_psc_off [] = "off";
char                       msg_psc_on [] = "on";
char                     msg_psc_weak [] = "weak";
char                msg_psc_upsonline [] = "\nUPS %m-line\n";
char                msg_psc_upsonbatt [] = "\nUPS %m battery\n";
char               msg_psc_upsbattery [] = "\nUPS battery %m\n";
char               msg_psc_ldc_status [] = "\nLDC Status\n\n";
char               msg_psc_notpresent [] = "Not Present\n";
char                 msg_psc_disabled [] = "Disabled\n";
char                  msg_psc_enabled [] = "Enabled\n";
char                  msg_psc_lastpwd [] = "\nLast Power Down Due to: ";
char                   msg_psc_acrmvd [] = "AC Power Removed\n";
char                  msg_psc_ocprqst [] = "OCP Requested DC Off\n";
char                  msg_psc_pcbrqst [] = "PCB Requested DC Off\n";
char                 msg_psc_hostrqst [] = "Host Requested power to be cycled\n";
char                    msg_psc_acinp [] = "AC input current out of bounds\n";
char                msg_psc_unkncause [] = "Unknown Cause\n";
char               msg_psc_parametric [] = "\nPSC PARAMETRIC Values\n";
char                 msg_psc_unknresp [] = "\nUnknown PSC Response Packet Received: %02x\n";
char                     msg_psc_2_1v [] = "2.1V\t\t";
char                     msg_psc_3_3v [] = "3.3V\t\t";
char                     msg_psc_5_1v [] = "5.1V\t\t";
char                    msg_psc_12_1v [] = "12.1V\t\t";
char                    msg_psc_fan_v [] = "Fan Voltage(V)\t";
char                   msg_psc_fan1_s [] = "FAN 1 Speed(RPM)";
char                   msg_psc_fan2_s [] = "FAN 2 Speed(RPM)";
char                   msg_psc_fan3_s [] = "FAN 3 Speed(RPM)";
char                   msg_psc_fan4_s [] = "FAN 4 Speed(RPM)";
char                      msg_psc_ldc [] = "LDC %c:\t";
char                msg_fname_unknown [] = "Unknown Parameter Entered: %s\n";
char               msg_fname_ununique [] = "Non-unique Parameter entered: %s\n";
/* End of PSC Command Messages */      
/* io_diag messages */
char               msg_scsi_up_err    [] = "Upper SCSI Continuity Card Missing (connector J6)\n";
char               msg_scsi_low_err   [] = "Lower SCSI Continuity Card Missing (connector J7)\n";
char               msg_toy_error      [] = "TOY Clock Valid bit not set\n";
/* End of io_diag messages */
/* Show error messages */
char               msg_smem_title      [] = "\n%s Module EEROM Event Log\n";
char               msg_smem_head1      [] = "\nEntry\tOffset\tRAM #\tBit Mask\tMulti-Chip\tEvent Type\n";
char               msg_smem_head2      [] = "\nEntry\tError Mask\tDevice #\tEvent Type\n";
char               msg_smem_none       []  = "\nNo Entries Found\n";
char               msg_smem_detail1    [] = "  %d\t%x\t%d\t%04b\t\t      %d\t\t  %02b\n";
char               msg_smem_detail2    [] = "  %d\t%x\t\t%d\t\t    %d\n";
char               msg_smem_head_sb    [] = "\nEntry   Fail Address   Bits/Syndrome   Bank #   ASIC #   Source   Event Type\n";
char               msg_smem_detail_sb  [] = "  %02d     %08x";
char               msg_smem_detail1_sb [] = "          %04x      ";
char               msg_simm_detail1_sb [] = "        %04x(J%d)   ";
char               msg_smem_detail2_sb [] = "       %3d,%3d      ";
char               msg_simm_detail2_sb [] = "   %3d(J%d),";
char               msg_simm_detail3_sb [] = "%3d(J%d)";
char               msg_smem_detail3_sb [] = "    %d        %d        %d          %02b\n";
/* End of Show error messages */
/* Show fru messages */
char               msg_sfru_title      [] = "\n\t\t\t\t Rev\t\t\tEvents logged\n Slot\tOption\tPart#\t\tHw Sw\tSerial#\t\tSDD\tTDD\n";
char               msg_sfru_detail     [] = "   %01d\t%s\t%s\t%s %01d\t%s\t%02d\t%02d\n";
char               msg_sfru_detail1    [] = "   %01d\n";
/* End of Show fru messages */
/* Additional Show error messages */
char msg_scpu_head_tdd [] = "\nTest Directed Errors\n";
char msg_scpu_head_sdd [] = "\nSymptom Directed Errors\n";
char msg_scpu_detail_tdd [] = "\nEntry: %d  Test Number: %02x\tSubtest Number: %02x\nAddress:       %08x,%08x\nExpected Data: %08x,%08x\nReceived Data: %08x,%08x\n";
char msg_scpu_detail_tdd_sb [] = "\nEntry: %d  Test: %02x\tSubtest: %02x\nP1: %08x,%08x\nP2: %08x,%08x\nP3: %08x,%08x\n";
char msg_scpu_head_bc [] = "\nB-Cache Correctable Errors\n\nEntry\tSyndrome\tOffset L\tOffset H\tCount\n";
char msg_scpu_detail_bc [] = "   %01d\t%x\t%x\t%x\t%d\n";
char msg_scpu_head_cnt [] = "\nCPU Event Counters\n";
char msg_scpu_detail_cnt [] = "%s\t%d\n";
char msg_sio_head_cnt [] = "\nI/O Event Counters\n";
char msg_sio_detail_cnt [] = "%s\t%d\n";
char msg_scpu_ecnt_msg0 [] = "C3_CA_NOACK";
char msg_scpu_ecnt_msg1 [] = "";
char msg_scpu_ecnt_msg2 [] = "C3_WD_NOACK";
char msg_scpu_ecnt_msg3 [] = "";
char msg_scpu_ecnt_msg4 [] = "C3_RD_PAR_E";
char msg_scpu_ecnt_msg5 [] = "C3_RD_PAR_O";
char msg_scpu_ecnt_msg6 [] = "EV_C_UNCORR_0";
char msg_scpu_ecnt_msg7 [] = "EV_C_UNCORR_1";
char msg_scpu_ecnt_msg24 [] = "EV_C_UNCORR_2";
char msg_scpu_ecnt_msg25 [] = "EV_C_UNCORR_3";
char msg_scpu_ecnt_msg8 [] = "EV_TC_PAR";
char msg_scpu_ecnt_msg9 [] = "EV_T_PAR";
char msg_scpu_ecnt_msg10 [] = "C3_TC_PAR_E";
char msg_scpu_ecnt_msg11 [] = "C3_T_PAR_O";
char msg_scpu_ecnt_msg12 [] = "C3_EV_E\t";
char msg_scpu_ecnt_msg13 [] = "C3_EV_O\t";
char msg_scpu_ecnt_msg14 [] = "C3_CA_PAR_E";
char msg_scpu_ecnt_msg15 [] = "C3_CA_PAR_O";
char msg_scpu_ecnt_msg16 [] = "C3_C_UNCORR_0";
char msg_scpu_ecnt_msg17 [] = "C3_C_UNCORR_1";
char msg_scpu_ecnt_msg26 [] = "C3_C_UNCORR_2";
char msg_scpu_ecnt_msg27 [] = "C3_C_UNCORR_3";
char msg_scpu_ecnt_msg18 [] = "C3_WD_PAR_E";
char msg_scpu_ecnt_msg19 [] = "C3_WD_PAR_O";
char msg_scpu_ecnt_msg20 [] = "C3_T_PAR_E";
char msg_scpu_ecnt_msg21 [] = "C3_TC_PAR_O";
char msg_scpu_ecnt_msg22 [] = "C3_DT_PAR_E";
char msg_scpu_ecnt_msg23 [] = "C3_DT_PAR_O";
char msg_sio_ecnt_msg0 [] = "IO_CA_NOACK_E";
char msg_sio_ecnt_msg1 [] = "IO_CA_NOACK_O";
char msg_sio_ecnt_msg2 [] = "IO_WD_NOACK_E";
char msg_sio_ecnt_msg3 [] = "IO_WD_NOACK_O";
char msg_sio_ecnt_msg4 [] = "IO_RD_PAR_E";
char msg_sio_ecnt_msg5 [] = "IO_RD_PAR_O";
char msg_sio_ecnt_msg6 [] = "IO_CA_PAR_E";
char msg_sio_ecnt_msg7 [] = "IO_CA_PAR_O";
char msg_sio_ecnt_msg8 [] = "IO_LB_DMA_PAR_E";
char msg_sio_ecnt_msg9 [] = "IO_LB_DMA_PAR_O";
char msg_sio_ecnt_msg10 [] = "IO_FB_DMA_PAR_E";
char msg_sio_ecnt_msg11 [] = "IO_FB_DMA_PAR_O";
char msg_sio_ecnt_msg12 [] = "IO_SCSTALL_E";
char msg_sio_ecnt_msg13 [] = "IO_SCSTALL_O";
char msg_sio_ecnt_msg14 [] = "IO_FB_MB_PAR";
char msg_sio_ecnt_msg15 [] = "IO_BUSSYNC";
char msg_sio_ecnt_msg16 [] = "TGEC0_A\t";
char msg_sio_ecnt_msg17 [] = "TGEC0_B\t";
char msg_sio_ecnt_msg18 [] = "TGEC1_A\t";
char msg_sio_ecnt_msg19 [] = "TGEC1_B\t";
char msg_sio_ecnt_msg20 [] = "SCRIPT_RAM_A";
char msg_sio_ecnt_msg21 [] = "SCRIPT_RAM_B";
char msg_sio_ecnt_msg22 [] = "NCR0_A\t";
char msg_sio_ecnt_msg23 [] = "NCR0_B\t";
char msg_sio_ecnt_msg24 [] = "NCR1_A\t";
char msg_sio_ecnt_msg25 [] = "NCR1_B\t";
char msg_sio_ecnt_msg26 [] = "NCR2_A\t";
char msg_sio_ecnt_msg27 [] = "NCR2_B\t";
char msg_sio_ecnt_msg28 [] = "NCR3_A\t";
char msg_sio_ecnt_msg29 [] = "NCR3_B\t";
char msg_sio_ecnt_msg30 [] = "NCR4_A\t";
char msg_sio_ecnt_msg31 [] = "NCR4_B\t";
char msg_sio_ecnt_msg32 [] = "IO_WD_PAR_E";
char msg_sio_ecnt_msg33 [] = "IO_WD_PAR_O";
/* sable SDD counts */
char msg_sio_ecnt_sb_msg0 [] = "IO_CA_NOACK";
char msg_sio_ecnt_sb_msg1 [] = "IO_WD_NOACK";
char msg_sio_ecnt_sb_msg2 [] = "IO_WD_PAR";
char msg_sio_ecnt_sb_msg3 [] = "IO_RD_PAR";
char msg_sio_ecnt_sb_msg4 [] = "IO_CA_PAR";
char msg_sio_ecnt_sb_msg5 [] = "IO_BUSSYNC";
char msg_sio_ecnt_sb_msg6 [] = "PCI_WR_PAR";
char msg_sio_ecnt_sb_msg7 [] = "PCI_ADD_PAR";
char msg_sio_ecnt_sb_msg8 [] = "PCI_RD_PAR";
char msg_sio_ecnt_sb_msg9 [] = "PCI_DEV_PAR";
char msg_sio_ecnt_sb_msg10 [] = "PCI_SYS_ERR";
char msg_sio_ecnt_sb_msg11 [] = "PCI_NMI ";
char msg_sio_ecnt_sb_msg12 [] = "TGEC0_A ";
char msg_sio_ecnt_sb_msg13 [] = "TGEC0_B ";
char msg_sio_ecnt_sb_msg14 [] = "NCR_A   ";
char msg_sio_ecnt_sb_msg15 [] = "NCR_B   ";
char msg_sio_ecnt_sb_msg16 [] = "EISA_BRIDGE_A";
char msg_sio_ecnt_sb_msg17 [] = "EISA_BRIDGE_B";

char msg_ffru_title     [] = "\nFuturebus+ Nodes\n\t\t\t   Rev\n Slot\tOption\tPart#\t Hw   Fw   Serial#\t\tDescription\n";
/* End of Additional Show error messages */
/* Fbus sizer messages */
char msg_fbus_sizer_0 [] = "Start of FBUS sizer\n";
char msg_fbus_sizer_1 [] = "Fbus sizer halted due to error\n";
char msg_fbus_sizer_2 [] = "Fbus sizer completed\n";
char msg_fbus_sizer_3 [] = "Running extended tests on Fbus+ node %d\n";
char msg_fbus_sizer_4 [] = "* Warning * Extended FBUS+ tests may have not completed successfully\n";
char msg_fbus_sizer_5 [] = "FBUS+ node %d did not complete testing in allotted time\n";
char msg_fbus_sizer_6 [] = "FBUS+ testing has been terminated\n";
char msg_fbus_sizer_7 [] = "Extended FBUS+ tests PASSED\n";
char msg_fbus_sizer_8 [] = "Extended test running on FBUS+ node %d timed out\n";
char msg_fbus_sizer_9 [] = "FBUS+ testing has been terminated\n";
char msg_fbus_sizer_a [] = "Fbus error during sizing routine\n";
char msg_fbus_sizer_b [] = "Bad status returned from read of Fbus address %x during Fbus sizer\n";
char msg_fbus_sizer_c [] = "* Warning * Attempt at reading Fbus+ space resulted in busy or timeout status\n";
/* End of Fbus sizer messages */
/* iic_driver messages */
char msg_iic_bad_status []  = "\nIIC bus Status (%02x) error\n";
char msg_iic_bad_int_set [] = "\nInterrupt Set Failed";
char msg_iic_bus_timeout [] = "\nIIC Bus Busy Timeout\n";
char msg_iic_reg [] = "\nRegister: %s\n";
char msg_psc_parm_v [] = "%m\t%02d.%02d\n";
char msg_driver_stopped [] = "\n%m driver not enabled, use 'start -driver' or 'init' to enable\n";
/* End of iic_driver messages */
/*Fbus diag messages */
char msg_fbus_err [] = "\nFbus diag error\n";
char msg_fbus_bit [] = "\nFbus Test_Status CSR bit error\n";
char msg_fbus_time [] = "\nFbus Test_Status CSR timeout error\n";
char msg_fbus_bit_time [] = "\nFbus Test_Status CSR bit and timeout error\n";
char msg_fbus_not_im [] = "\nWarning: Fbus+ device %s test not implemented\n";
char msg_fbus_illeg [] = "\nFbus Test_Status CSR, illegal value\n";
char msg_d_fbus_exterr_hd [] = "EXTENDED ERROR INFORMATION\n**************************\n\n";
char msg_d_fbus_ascii [] = "ascii format specified\n\n\n%x";
char msg_d_fbus_hex [] = "hex format specified\n\n\n";
char msg_d_fbus_format [] = "print format not ascii or hex: illegal format\n";
/* End of Fbus diag messages */
/*Fbus miscellaneous messages */
char msg_fbus_enable [] = "enable Fbus\n";
char msg_fbus_mbx_trans [] = "Mailbox transaction";
char msg_fbus_dma_trans [] = "DMA transaction";
char msg_fbus_parity_err [] = "\nParity Error during F-bus+ %m\n";
char msg_fbus_reset [] = "FMBPR and Fbus devices have been reset\n";
char msg_d_fbus_vect_err [] = "Error; Fbus interrupt expected via vector %x, received via %x\nfbus interrupts received since -seq option:%d\n";
char msg_d_fbus_int_cnt [] = "interrupt count:%d, number of vectors:%d, check seq flag:%d\n";
char msg_d_fbus_vect_reenter [] = "reenter command after reducing vector list to a max of %d vectors\n";
char msg_d_fbus_bad_vect [] = "invalid vector\n";
char msg_d_fbus_prev_err [] = "Test of FBUS+ device, %s, halted due to previous device error\n";
/* End of Fbus miscellaneous messages */
char msg_no_auto_action [] = "\nHalt Button is IN, AUTO_ACTION ignored\n\n";
char msg_no_boot [] = "\nHalt Button is IN, BOOT NOT POSSIBLE\n\n";
char msg_boot_delay [] = "\nWaiting until most devices come online\n";
char msg_fbus_stmd_invld [] = "Invalid data width for partial transfer - use fbus_setmode first";
/* secure messages */
char msg_cnsl_secure []   = "Console is secure. Please login.\n";
char msg_invld_psswd []   = "Invalid password\n";
char msg_val_err []       = "Validation error\n";
char msg_psswd_len_err [] = "Password length must be between %d and %d characters\n";
char msg_curr_psswd_err[] = "Current password incorrect\n";
char msg_cmd_noparams[]   = "Set Password does not allow parameters on the command line\n";
char msg_noboot_params[]  = "Console is secure - parameters are not allowed.\n";
char msg_no_vldpsswd []   = "Secure not set.  Please set the password.\n";
char msg_pwd_cleared []   = "Password successfully cleared.\n";
char msg_no_help []       = "You must login to the console to view help.\n";
/* end of secure messages */

/* Can be deleted */
char                   msg_ez_err_mem [] = "";
char             msg_ez_err_rcv_wd_to [] = "";
char             msg_ez_err_xmt_wd_to [] = "";
char               msg_ez_err_bad_sts [] = "";

/* Messages for the ARC front end */
char	msg_esuccess		[] = "no error";			/* 0	*/
char	msg_e2big		[] = "argument list too long";		/* 1	*/
char	msg_eacces		[] = "permission denied";		/* 2	*/
char	msg_eagain		[] = "resource temporarily unavailable";/* 3	*/
char	msg_ebadf		[] = "bad file descriptor";		/* 4	*/
char	msg_ebusy		[] = "resource busy";			/* 5	*/
char	msg_efault		[] = "bad address";			/* 6	*/
char	msg_einval		[] = "invalid argument";		/* 7	*/
char	msg_eio			[] = "input/output error";		/* 8	*/
char	msg_eisdir		[] = "is a directory";			/* 9	*/
char	msg_emfile		[] = "too many open files";		/* 10	*/
char	msg_emlink		[] = "too many links";			/* 11	*/
char	msg_enametoolong	[] = "filename too long";		/* 12	*/
char	msg_enodev		[] = "no such device";			/* 13	*/
char	msg_enoent		[] = "no such file or directory";	/* 14	*/
char	msg_enoexec		[] = "execute format error";		/* 15	*/
char	msg_enomem		[] = "not enough space";		/* 16	*/
char	msg_enospc		[] = "no space left on device";		/* 17	*/
char	msg_enotdir		[] = "not a directory";			/* 18	*/
char	msg_enotty		[] = "inappropriate I/O control operation";		/* 19	*/
char	msg_enxio		[] = "media not loaded";		/* 20	*/
char	msg_erofs		[] = "read-only file system";		/* 21	*/

/* Messages for Turbo EEPROM */

char msg_eep_pwrscrpt		[] = "executing script: %s\n";
char msg_eep_noti		[] = "%s has not been initialized\n";
char msg_eep_ena_log		[] = "Enable Logging [Y/N]> ";
char msg_eep_rst_log		[] = "Reset Logging Error Count [Y/N]> ";
char msg_eep_fverify		[] = "%s failed to verify\n";
char msg_eep_iversion		[] = "%s is incorrect version\n";
char msg_eep_create		[] = "\nCreating new %s\n";
char msg_eep_addr		[] = "\n%s Address:\t%08x\n";
char msg_eep_hcsum		[] = "Header - Checksum: %08X  Length: %04X  Version:%d\n";
char msg_eep_hdr2		[] = "\nArea  Offset  Length     Cksum    Key\n";
char msg_eep_badhcs		[] = "%s bad header checksum\n";
char msg_eep_badacs		[] = "%s bad checksum Area %d.%d\n";
char msg_eep_keynf		[] = "%s area key %d.%d not found\n";
char msg_eep_badacs1		[] = "%s area key %d.%d checksum error\n";
char msg_eep_badcs		[] = "%s bad checksum\n";
char msg_eep_wrt		[] = "  %s write, dst = %x, src = %x, len = %d,";
char msg_eep_noup		[] = "%s not updated\n";
char msg_eep_wrtf		[] = "%s write failure, ";
char msg_eep_wrtf1		[] = "adr = %x, exp = %2x, act = %2x\n";
char msg_eep_pare		[] = "parameters are:\n";
char msg_eep_notset		[] = "%s environment parameters not setup\n";
char msg_eep_spec		[] = "    or specific environment variable\n";
char msg_eep_notclr		[] = "Predefined variables are not clearable\n";
char msg_eep_nopar		[] = "  %s, No such parameter\n";
char msg_eep_tms		[] = "Too many strings\n";
char msg_eep_nsl		[] = "No space left in string table\n";
char msg_eep_ssn		[] = "\nSystem Serial Number = %.10s\n";
char msg_eep_msn		[] = "\nModule Serial Number = %.10s\n";
char msg_eep_mtyp		[] = "Module Unified 2-5-2.4 Part Number = %.16s\n";
char msg_eep_mrev		[] = "Module Firmware Revision = %.4s\n";
char msg_eep_pscrpt		[] = "Powerup Script:\n";
char msg_eep_tscrpt		[] = "Test Script:\n";
char msg_eep_scrptl		[] = "Input script too long\n";
char msg_eep_lars		[] = "\nLARS # = %.8s\n";
char msg_eep_msg		[] = "Message = %.68s\n";
char msg_eep_log		[] = "Logging is ";
char msg_eep_dis		[] = "disabled\n";
char msg_eep_ena		[] = "enabled\n";
char msg_eep_otf		[] = "OTF set\n";
char msg_eep_cfield		[] = "Logging Control Field value = %04x\n";
char msg_eep_alog		[] = "Actions logged = %d\n";
char msg_eep_log_fail		[] = "EEPROM Logging Failed\n";
char msg_eep_diag		[] = "\nDiagnostic\n";
char msg_eep_diag_tdd		[] = "\nDiagnostic TDD ";
char msg_eep_diag_sdd		[] = "\nDiagnostic SDD ";
char msg_eep_tddlnbr		[] = "\nDiagnostic TDD Log number %d\n";
char msg_eep_tddl1		[] = "Diag = %s, SSN = %s, Detecting CPU Node = %1X,Failing FRU Node = %1X, ";
char msg_eep_tddl2		[] = "Firmware Rev = %c%d.%d-%d, Time = %02d/%02d/%02d %02d:%02d:%02d, ";
char msg_eep_tddl3		[] = "Test = %d, Mode = %d, Error Number = %d, ErrorType = %d, Error Count = %d\n";
char msg_eep_tddlud		[] = "Data1/%08X%08X, Data2/%08X%08X,Data3/%08X%08X\n";
char msg_eep_tdddis		[] = "Console TDD Error Logging is disabled\n";
char msg_eep_tddfull		[] = "Five TDD Error Logs already logged\n";
char msg_eep_sddlnbr		[] = "\nDiagnostic SDD Log number %d\n";
char msg_eep_sdddis		[] = "Console SDD Error Logging is disabled\n";
char msg_eep_sddfull		[] = "Five SDD Error Logs already logged\n";
char msg_eep_sym		[] = "\nSymptom ";
char msg_eep_slnbr		[] = "\nSymptom Log number %d\n";

/* Turbo TIOP/LAMB/FLAG messages */
char msg_xber 			[] = "XBER:\t %08x\n";
char msg_xfadr 			[] = "XFADR:\t %08x\n";
char msg_xfaer 			[] = "XFAER:\t %08x\n";
char msg_lerr 			[] = "LERR:\t %08x\n";
char msg_d_tlber		[] = "\nLBER:\t %08x";
char msg_d_xct_act_f		[] = "XCT action failed\n";
char msg_d_xct_dev_hung		[] = "Test %d, device %s is hung\n";
char msg_d_xct_data_chk		[] = "XCT DATA_CHECK_ERR\n";
char msg_d_xct_eoff		[] = "EOFF is %x\n";
char msg_d_xct_xor_r		[] = "XOR_RESULT is %x\n";
char msg_d_xct_ex_buf		[] = "Examine XCT buffers and MBUFF for more information \n";
char msg_d_xct_resp		[] = "XCT RESPONDER_ERR\n";
char msg_d_xct_resp_csr		[] = "XCT responder CSR is %x\n";
char msg_d_xct_mov		[] = "XCT XMOV_ERR\n";
char msg_d_xct_movr		[] = "XCT Mover %s CSR: %x\n";
char msg_d_xct_mvbcr		[] = "XCT Mover %s byte count CSR: %x\n";
char msg_d_xct_mvnpr		[] = "XCT Mover %s next page CSR: %x\n";
char msg_d_xct_mvapr		[] = "XCT Mover %s address CSR: %x\n";
char msg_d_xct_mcwi_es		[] = "XCT MCWI_ES CSR: %x\n";
char msg_d_xct_hw		[] = "XCT hardware error\n";
char msg_d_xct_ucode_trap	[] = "XCT trapped through uadd 100\n";
char msg_d_xct_ucode_san	[] = "XCT ucode sanity check\n";
char msg_d_xct_ucode_san2	[] = "XCT ucode sanity check 2 - mover flags error\n";
char msg_d_xct_undoc		[] = "Undocumented XCT error\n";
char msg_xct_ex_begi		[] = "Starting xct exerciser on all XCTs (id #%x)\n";
char msg_xct_ex_don		[] = "Stopping xct exerciser on all XCTs (id #%x)\n";
char msg_dev_no_stop		[] = "Can't stop device %s\n";
char msg_dev_running		[] = "%s running, need to stop first\n";
char msg_tiop_err		[] = "KFTxA error";
char msg_tiop_hose_err		[] = "KFTxA HOSE ERROR: FATAL CONDITION - REQUIRES RESET\n";
char msg_lamb_err		[] = "DWLMA error";
char msg_xmi_number		[] = "XMI number: %x\n";

char msg_d_islamb		[] = "Is_Lamb return status error\n";
char msg_d_isflag		[] = "Is_FLAG Return Status Error\n";
char msg_d_flag_err		[] = "DWLAA error";
char msg_d_fbe_err		[] = "FBE error";
char msg_d_script_fail		[] = "Device %s failed to start script EXT_%04x\n";
char msg_d_fbe_int_err		[] = "FBE Interrupt Error\n";
char msg_d_fbe_script_err	[] = "FBE Script Failure\n";
char msg_d_lpbk_err		[] = "Loopback transaction error\n";
char msg_d_errhi		[] = "ERRHI:\t %08x\n";
char msg_d_errlo		[] = "ERRLO:\t %08x\n";
char msg_d_fadrhi		[] = "FADRHI:\t %08x\n";
char msg_d_fadrlo		[] = "FADRLO:\t %08x\n";
char msg_d_ferr			[] = "FERR:\t %08x\n";

char msg_d_inter		[] = "Interrupt failed\n";

/* End TIOP/LAMB/FLAG messages */

/* KFMSB messages */
char msg_kfmsb_timo		[] = "KFMSB: adapter / command timed out\n";
char msg_kfmsb_wrongstate	[] = "KFMSB: adapter is in the wrong state\n";
char msg_kfmsb_badreg		[] = "KFMSB: internal error - bad register code\n";
char msg_kfmsb_toobig		[] = "KFMSB: data transfer too big\n";
char msg_kfmsb_illopcode	[] = "KFMSB: illegal operation code\n";
char msg_kfmsb_badarg		[] = "KFMSB: bad argument to I/O call\n";
char msg_kfmsb_badcmd		[] = "KFMSB: illegal command packet\n";
char msg_kfmsb_nobuf		[] = "KFMSB: could not allocate command buffer\n";
char msg_kfmsb_rip		[] = "KFMSB: Adapter reset in progress\n";

/* KZMSA messages */
char msg_kzmsa_timo		[] = "%s: adapter / command timed out\n";
char msg_kzmsa_wrongstate	[] = "%s: adapter is in the wrong state\n";
char msg_kzmsa_badreg		[] = "%s: internal error - bad register code\n";
char msg_kzmsa_toobig		[] = "%s: data transfer too big\n";
char msg_kzmsa_illopcode	[] = "%s: illegal operation code\n";
char msg_kzmsa_badarg		[] = "%s: bad argument to I/O call\n";
char msg_kzmsa_badcmd		[] = "%s: illegal command packet\n";
char msg_kzmsa_nobuf		[] = "%s: could not allocate command buffer\n";
char msg_kzmsa_rip		[] = "%s: Adapter reset in progress\n";

/* PDQ messages xxxx/fddi */
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
char msg_pdq_msg15	[] = "%02X-%02X-%02X-%02X-%02X-%02X";
char msg_pdq_msg22	[] = "\t%x: \t%x\n";
char msg_pdq_msg70	[] = "%-24s   %-8s   %24s";

/* DEMFA messages */
char msg_fx_shutdown_fail	[] = "%s: Shutdown attempted when port not in the Initialized, Running, or Maintenance state.\n";
char msg_fx_bad_xmtring		[] = "%s: Invalid transmit ring.\n";
char msg_fx_bad_rcvring		[] = "%s: Invalid unsolicited ring.\n";
char msg_fx_esp_failed		[] = "%s: ESP special test failed.\n";
char msg_fx_fddi_broken		[] = "%s: FDDI State has gone to Broken.\n";
char msg_fx_prs_db_err		[] = "%s: Parser Database error.\n";
char msg_fx_pc_tr_path		[] = "%s: PC TRACE PATH test.\n";
char msg_fx_cam_del_err		[] = "%s: CAM delete error.\n";
char msg_fx_cam_rd_err		[] = "%s: CAM read error.\n";
char msg_fx_cam_wr_err		[] = "%s: CAM write error.\n";
char msg_fx_cam_cmp_err		[] = "%s: CAM compare error.\n";
char msg_fx_cam_sw_res		[] = "%s: CAM software reset failed.\n";
char msg_fx_ami_parity		[] = "%s: AMI parity error.\n";
char msg_fx_dparom_crc		[] = "%s: DPA ROM CRC error.\n";
char msg_fx_inval_loopback	[] = "%s: Invalid loopback command.\n";
char msg_fx_val_cmd_fail	[] = "%s: Valid command failed.\n";
char msg_fx_ring_init_init	[] = "Ring Init Initiated";
char msg_fx_ring_init_rcvd	[] = "Ring Init Received";
char msg_fx_ring_beac_init	[] = "Ring Beacon Initiated";
char msg_fx_dat_fail		[] = "Duplicate Address Test Failure";
char msg_fx_dup_tok_rcvd	[] = "Duplicate Token Received";
char msg_fx_ring_purg_err	[] = "Ring Purger Error";
char msg_fx_bridge_strip_err	[] = "Bridge Strip Error";
char msg_fx_ring_op_osc		[] = "Ring Op Oscillation";
char msg_fx_dir_beac_rcvd	[] = "Directed Beacon Received";
char msg_fx_pc_trace_init	[] = "PC Trace Initiated";
char msg_fx_pc_trace_rcvd	[] = "PC Trace Received";
char msg_fx_xmt_underrun	[] = "Transmit Underrun";
char msg_fx_xmt_failures	[] = "Transmit Failures";
char msg_fx_rcv_overruns	[] = "Receive Overruns";
char msg_fx_lem_reject		[] = "LEM Reject";
char msg_fx_ebuff_err		[] = "EBUFF Error";
char msg_fx_lct_reject		[] = "LCT Reject";
char msg_fx_unsol_pkt_rcvd	[] = "%s: Unsolicited packet received.  Opcode=%04x, %m.\n";
char msg_fx_unsol_data		[] = "Unsolicited Data #%d: %08x\n";
char msg_fx_rcvring_err		[] = "%s: Error in Receive Ring:\n";
char msg_fx_rcvring_nio		[] = "        NIO = %1x\n";
char msg_fx_rcvring_fsc		[] = "        FSC = %1x\n";
char msg_fx_rcvring_fsb		[] = "        FSB = %2x\n";
char msg_fx_rcvring_rcc		[] = "        RCC = %3x\n";
char msg_fx_chained_rcv_pkt	[] = "%s: Port driver error - cannot handle chained receive packet!\n";
char msg_fx_multiseg_rcv	[] = "%s: Port driver error - cannot handle multi-segment receive ring entry!\n";
char msg_fx_xmtring_err		[] = "%s: Error in transmit ring entry.\n";
char msg_fx_64_txt		[] = "    %-28s%08x %08x\n";
char msg_int_set_err		[] = "%s: Error setting interrupt vector. Vector=%04x\n";

char msg_sys_reset		[] = "System will be reset prior to boot...\n";
char msg_not_on_this_platform   [] = "Not available on this platform: %s\n";

/*PCI_diag diag messages */
char msg_pci_err [] = "\nPCI diag error\n";
char msg_pci_bit [] = "\nPCI Test_Status CSR bit error\n";
char msg_pci_time [] = "\nPCI Test_Status CSR timeout error\n";
char msg_pci_bit_time [] = "\nPCI Test_Status CSR bit and timeout error\n";
char msg_pci_not_im [] = "\nWarning: PCI device %s test not implemented\n";
char msg_pci_illeg [] = "\nPCI Test_Status CSR, illegal value\n";
char msg_d_pci_exterr_hd [] = "EXTENDED ERROR INFORMATION\n**************************\n\n";
char msg_d_pci_ascii [] = "ascii format specified\n\n\n%x";
char msg_d_pci_hex [] = "hex format specified\n\n\n";
char msg_d_pci_format [] = "print format not ascii or hex: illegal format\n";
/* End of PCI_diag messages */

/* TEST msg for exer */
char msg_dsk_ex_begin [] = "Starting device exerciser on %s (id #%x) in READ-ONLY mode \n";
char msg_dsk_ex_begin_dest [] = "Starting device exerciser on %s (id #%x) in WRITE-READ mode\n";
char msg_dsk_ex_done [] = "Stopping device exerciser on %s (id #%x)\n";
