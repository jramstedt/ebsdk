! GLXYTRAIN Target-specific dependencies
i_objects = -
	cp$obj:callbacks_alpha.obj,-
	cp$obj:kernel_alpha.obj,-
	cp$obj:pc264_util.obj,-
	cp$obj:inst_alpha.obj,-
	cp$obj:memtest_alpha.obj,-
	cp$obj:pke_script.obj,-
	cp$obj:pue_script.obj,-
	cp$obj:encap_add_de205.obj,-
	cp$obj:encap_memexer.obj,-
	cp$obj:encap_testmem.obj,-
	cp$obj:encap_sys_exer.obj,-
	cp$obj:encap_show_status.obj,-
	cp$obj:encap_kill_diags.obj,-
	cp$obj:encap_showit.obj,-
	cp$obj:encap_pwrup.obj,-
	cp$obj:encap_test.obj,-
	cp$obj:encap_mfgtest.obj,-
	cp$obj:encap_set_flag.obj,-
	cp$obj:encap_fwupdate.obj,-
	cp$obj:encap_readme.obj,-
	cp$obj:encap_save_nvram.obj,-
	cp$obj:encap_restore_nvram.obj

.IFDEF opt
objects = $(i_objects), -
 	cp$obj:grp_base.obj,-
 	cp$obj:grp_command.obj,-
 	cp$obj:grp_driver.obj,-
 	cp$obj:grp_data.obj
.ELSE $(opt)
objects = $(i_objects), -
	cp$obj:alphamm.obj,-
	cp$obj:ansi.obj,-
	cp$obj:aputchar.obj,-
	cp$obj:atoi.obj,-
	cp$obj:blex.obj,-
	cp$obj:bshell.obj,-
	cp$obj:bshell_cmd.obj,-
	cp$obj:boot.obj,-
	cp$obj:builtins.obj,-
	cp$obj:build_fru.obj,-
	cp$obj:call_backs.obj,-
	cp$obj:date.obj,-
	cp$obj:date_cmd.obj,-
	cp$obj:dynamic.obj,-
	cp$obj:entry.obj,-
	cp$obj:ev_action.obj,-
	cp$obj:exdep.obj,-
	cp$obj:externdata.obj,-
	cp$obj:filesys.obj,-
	cp$obj:hwrpb.obj,-
	cp$obj:hwrpbtt_driver.obj,-
	cp$obj:ie.obj,-
	cp$obj:iobq_cmd.obj,-
	cp$obj:isacfg.obj,-
	cp$obj:isp1020_edit.obj,-
	cp$obj:kernel.obj,-
	cp$obj:kernel_support.obj,-
	cp$obj:memset.obj,-
	cp$obj:mopdl_driver.obj,-
	cp$obj:moplp_driver.obj,-
	cp$obj:msgtxt.obj,-
	cp$obj:ni_datalink.obj,-
	cp$obj:printf.obj,-
	cp$obj:probe_io.obj,-
	cp$obj:process.obj,-
	cp$obj:powerup.obj,-
	cp$obj:qscan.obj,-
	cp$obj:queue.obj,-
	cp$obj:random.obj,-
	cp$obj:readline.obj,-
	cp$obj:regexp.obj,-
	cp$obj:run_cmd.obj,-
	cp$obj:runbios.obj,-
	cp$obj:scsi.obj,-
	cp$obj:scs.obj,-
	cp$obj:semaphore.obj,-
	cp$obj:show_hwrpb.obj,-
	cp$obj:show_map.obj,-
	cp$obj:setshow.obj,-
	cp$obj:shellsort.obj,-
	cp$obj:startstop.obj,-
	cp$obj:strfunc.obj,-
	cp$obj:timer.obj,-
	cp$obj:tr.obj,-
	cp$obj:uptime.obj,-
	cp$obj:xlate.obj,-
	cp$obj:nautilus.obj,-
	cp$obj:nautilus_init.obj,-
	cp$obj:irongate_io.obj,-
	cp$obj:nautilus_io.obj,-
	cp$obj:nautilus_mcheck.obj,-
	cp$obj:nautilus_nbridge.obj,-
	cp$obj:memconfig_nautilus.obj,-
	cp$obj:show_config_nautilus.obj,-
	cp$obj:amdflash_support.obj,-
	cp$obj:m1543c_sbridge.obj,-
	cp$obj:m1543c_iic.obj,-
	cp$obj:m5229_ide.obj,-
	cp$obj:api_debug.obj,-
	cp$obj:nautilus_info.obj,-
	cp$obj:dp264_fru.obj,-
	cp$obj:allocfree.obj,-
	cp$obj:bpt.obj,-
	cp$obj:cat.obj,-
	cp$obj:check.obj,-
	cp$obj:chmod.obj,-
	cp$obj:cmp.obj,-
	cp$obj:decode_alpha.obj,-
	cp$obj:diag_support.obj,-
	cp$obj:diag_evs.obj,-
	cp$obj:element.obj,-
	cp$obj:echo.obj,-
	cp$obj:edit.obj,-
	cp$obj:eval.obj,-
	cp$obj:exer.obj,-
	cp$obj:find_field.obj,-
	cp$obj:grep.obj,-
	cp$obj:galaxy.obj,-
	cp$obj:galaxy_nautilus.obj,-
	cp$obj:galaxy_config_tree.obj,-
	cp$obj:hd.obj,-
	cp$obj:help.obj,-
	cp$obj:line.obj,-
	cp$obj:login_cmd.obj,-
	cp$obj:ls.obj,-
	cp$obj:moprc_driver.obj,-
	cp$obj:moprctt_driver.obj,-
	cp$obj:more.obj,-
	cp$obj:net.obj,-
	cp$obj:prcache.obj,-
	cp$obj:prcache_pci.obj,-
	cp$obj:ps.obj,-
	cp$obj:rm.obj,-
	cp$obj:secure.obj,-
	cp$obj:sem_cmd.obj,-
	cp$obj:simport.obj,-
	cp$obj:sleep.obj,-
	cp$obj:sort.obj,-
	cp$obj:sound.obj,-
	cp$obj:symbols.obj,-
	cp$obj:truefalse.obj,-
	cp$obj:validate.obj,-
	cp$obj:wc.obj,-
	cp$obj:memtest.obj,-
	cp$obj:memtest_gray_lw.obj,-
	cp$obj:memtest_march.obj,-
	cp$obj:memtest_random.obj,-
	cp$obj:memtest_victim.obj,-
	cp$obj:nettest.obj,-
	cp$obj:lfu.obj,-
	cp$obj:lfu_support_apc.obj,-
	cp$obj:nl_driver.obj,-
	cp$obj:rd_driver.obj,-
	cp$obj:toy_driver.obj,-
	cp$obj:pci_driver.obj,-
	cp$obj:from_driver.obj,-
	cp$obj:nvram_driver.obj,-
	cp$obj:api_iic.obj,-
	cp$obj:tt_driver.obj,-
	cp$obj:dv_driver.obj,-
	cp$obj:vga_bios_driver.obj,-
	cp$obj:kbd_driver.obj,-
	cp$obj:combo_driver.obj,-
	cp$obj:ev_driver.obj,-
	cp$obj:el_driver.obj,-
	cp$obj:pmem_driver.obj,-
	cp$obj:vmem_driver.obj,-
	cp$obj:examine_driver.obj,-
	cp$obj:ev6_ipr_driver.obj,-
	cp$obj:gpr_driver.obj,-
	cp$obj:ps_driver.obj,-
	cp$obj:ndbr_driver.obj,-
	cp$obj:mop_driver.obj,-
	cp$obj:fpr_driver.obj,-
	cp$obj:pi_driver.obj,-
	cp$obj:decode_driver.obj,-
	cp$obj:tee_driver.obj,-
	cp$obj:dup.obj,-
	cp$obj:para_driver.obj,-
	cp$obj:inet_driver.obj,-
	cp$obj:fat_driver.obj,-
	cp$obj:m1543c_driver.obj,-
	cp$obj:dq_driver.obj,-
	cp$obj:de205_driver.obj,-
	cp$obj:n810_driver.obj,-
	cp$obj:pke_driver.obj,-
	cp$obj:pci_nvram_driver.obj,-
	cp$obj:pdq_driver.obj,-
	cp$obj:dac960_driver.obj,-
	cp$obj:pks_driver.obj,-
	cp$obj:isp1020_driver.obj,-
	cp$obj:ew_driver.obj,-
	cp$obj:ei_driver.obj,-
	cp$obj:nport_q.obj,-
	cp$obj:cipca_common.obj,-
	cp$obj:cipca_response.obj,-
	cp$obj:cipca_support.obj,-
	cp$obj:cipca_io.obj,-
	cp$obj:mscp.obj,-
	cp$obj:cipca_driver.obj,-
	cp$obj:kgpsa_driver.obj,-
	cp$obj:wwidmgr.obj,-
	cp$obj:pue_driver.obj,-
	cp$obj:mc_driver.obj,-
	cp$obj:i2o_driver.obj,-
	cp$obj:i2oexec.obj,-
	cp$obj:i2omsg.obj,-
	cp$obj:i2outil.obj,-
	cp$obj:i2o_bsa.obj,-
	cp$obj:i2o_bsa_osm.obj,-
	cp$obj:i2o_scsi_osm.obj,-
	cp$obj:iso9660_driver.obj,-
	cp$obj:aic78xx_driver.obj,-
	cp$obj:aic78xx_him_hwm.obj,-
	cp$obj:aic78xx_him_rsm.obj,-
	cp$obj:aic78xx_him_xlm.obj,-
	cp$obj:bios_disk_driver.obj,-
	cp$obj:lfu_nautilus_driver.obj,-
	cp$obj:lfu_kzpsa_driver.obj,-
	cp$obj:lfu_defpa_defea_driver.obj,-
	cp$obj:lfu_cipca_driver.obj,-
	cp$obj:lfu_mc2_driver.obj
.ENDIF

.IFDEF opt
!
! Group "base" (55 files)
!
cp$obj:grp_base.obj : -
	cp$src:alphamm.c, $(alphamm_depend),-
	cp$src:ansi.c, $(ansi_depend),-
	cp$src:aputchar.c, $(aputchar_depend),-
	cp$src:atoi.c, $(atoi_depend),-
	cp$src:call_backs.c, $(call_backs_depend),-
	cp$src:date.c, $(date_depend),-
	cp$src:dynamic.c, $(dynamic_depend),-
	cp$src:entry.c, $(entry_depend),-
	cp$src:ev_action.c, $(ev_action_depend),-
	cp$src:externdata.c, $(externdata_depend),-
	cp$src:filesys.c, $(filesys_depend),-
	cp$src:hwrpb.c, $(hwrpb_depend),-
	cp$src:ie.c, $(ie_depend),-
	cp$src:kernel.c, $(kernel_depend),-
	cp$src:kernel_support.c, $(kernel_support_depend),-
	cp$src:memset.c, $(memset_depend),-
	cp$src:printf.c, $(printf_depend),-
	cp$src:probe_io.c, $(probe_io_depend),-
	cp$src:process.c, $(process_depend),-
	cp$src:powerup.c, $(powerup_depend),-
	cp$src:qscan.c, $(qscan_depend),-
	cp$src:queue.c, $(queue_depend),-
	cp$src:random.c, $(random_depend),-
	cp$src:regexp.c, $(regexp_depend),-
	cp$src:runbios.c, $(runbios_depend),-
	cp$src:scsi.c, $(scsi_depend),-
	cp$src:scs.c, $(scs_depend),-
	cp$src:semaphore.c, $(semaphore_depend),-
	cp$src:shellsort.c, $(shellsort_depend),-
	cp$src:strfunc.c, $(strfunc_depend),-
	cp$src:timer.c, $(timer_depend),-
	cp$src:nautilus.c, $(nautilus_depend),-
	cp$src:nautilus_init.c, $(nautilus_init_depend),-
	cp$src:irongate_io.c, $(irongate_io_depend),-
	cp$src:nautilus_io.c, $(nautilus_io_depend),-
	cp$src:nautilus_mcheck.c, $(nautilus_mcheck_depend),-
	cp$src:memconfig_nautilus.c, $(memconfig_nautilus_depend),-
	cp$src:show_config_nautilus.c, $(show_config_nautilus_depend),-
	cp$src:amdflash_support.c, $(amdflash_support_depend),-
	cp$src:m1543c_sbridge.c, $(m1543c_sbridge_depend),-
	cp$src:m1543c_iic.c, $(m1543c_iic_depend),-
	cp$src:m5229_ide.c, $(m5229_ide_depend),-
	cp$src:decode_alpha.c, $(decode_alpha_depend),-
	cp$src:diag_support.c, $(diag_support_depend),-
	cp$src:diag_evs.c, $(diag_evs_depend),-
	cp$src:secure.c, $(secure_depend),-
	cp$src:symbols.c, $(symbols_depend),-
	cp$src:memtest.c, $(memtest_depend),-
	cp$src:memtest_gray_lw.c, $(memtest_gray_lw_depend),-
	cp$src:memtest_march.c, $(memtest_march_depend),-
	cp$src:memtest_random.c, $(memtest_random_depend),-
	cp$src:memtest_victim.c, $(memtest_victim_depend),-
	cp$src:nettest.c, $(nettest_depend),-
	cp$src:lfu_support_apc.c, $(lfu_support_apc_depend),-
	cp$src:dup.c, $(dup_depend)
 $(cc) $(cdefs) $(cquals) $(copt) $(list) -
	/plus/obj=cp$obj:grp_base.obj-
	/lis=cp$lis:grp_base.lis-
	cp$src:alphamm.c+ansi+aputchar+atoi+call_backs+date+dynamic+entry+ev_action+externdata+filesys+hwrpb+ie+kernel+kernel_support+memset+printf+probe_io+process+powerup+qscan+queue+random+regexp+runbios+scsi+scs+semaphore+shellsort+strfunc+timer-
	+nautilus+nautilus_init+irongate_io+nautilus_io+nautilus_mcheck+memconfig_nautilus+show_config_nautilus+amdflash_support+m1543c_sbridge+m1543c_iic+m5229_ide+decode_alpha+diag_support+diag_evs+secure+symbols+memtest+memtest_gray_lw-
	+memtest_march+memtest_random+memtest_victim+nettest+lfu_support_apc+dup
	! Group "base" command length was ~665

!
! Group "command" (59 files)
!
cp$obj:grp_command.obj : -
	cp$src:blex.c, $(blex_depend),-
	cp$src:bshell.c, $(bshell_depend),-
	cp$src:bshell_cmd.c, $(bshell_cmd_depend),-
	cp$src:boot.c, $(boot_depend),-
	cp$src:builtins.c, $(builtins_depend),-
	cp$src:build_fru.c, $(build_fru_depend),-
	cp$src:date_cmd.c, $(date_cmd_depend),-
	cp$src:exdep.c, $(exdep_depend),-
	cp$src:iobq_cmd.c, $(iobq_cmd_depend),-
	cp$src:isacfg.c, $(isacfg_depend),-
	cp$src:isp1020_edit.c, $(isp1020_edit_depend),-
	cp$src:readline.c, $(readline_depend),-
	cp$src:run_cmd.c, $(run_cmd_depend),-
	cp$src:show_hwrpb.c, $(show_hwrpb_depend),-
	cp$src:show_map.c, $(show_map_depend),-
	cp$src:setshow.c, $(setshow_depend),-
	cp$src:startstop.c, $(startstop_depend),-
	cp$src:tr.c, $(tr_depend),-
	cp$src:uptime.c, $(uptime_depend),-
	cp$src:xlate.c, $(xlate_depend),-
	cp$src:nautilus_nbridge.c, $(nautilus_nbridge_depend),-
	cp$src:api_debug.c, $(api_debug_depend),-
	cp$src:nautilus_info.c, $(nautilus_info_depend),-
	cp$src:dp264_fru.c, $(dp264_fru_depend),-
	cp$src:allocfree.c, $(allocfree_depend),-
	cp$src:bpt.c, $(bpt_depend),-
	cp$src:cat.c, $(cat_depend),-
	cp$src:check.c, $(check_depend),-
	cp$src:chmod.c, $(chmod_depend),-
	cp$src:cmp.c, $(cmp_depend),-
	cp$src:element.c, $(element_depend),-
	cp$src:echo.c, $(echo_depend),-
	cp$src:edit.c, $(edit_depend),-
	cp$src:eval.c, $(eval_depend),-
	cp$src:exer.c, $(exer_depend),-
	cp$src:find_field.c, $(find_field_depend),-
	cp$src:grep.c, $(grep_depend),-
	cp$src:galaxy.c, $(galaxy_depend),-
	cp$src:galaxy_nautilus.c, $(galaxy_nautilus_depend),-
	cp$src:galaxy_config_tree.c, $(galaxy_config_tree_depend),-
	cp$src:hd.c, $(hd_depend),-
	cp$src:help.c, $(help_depend),-
	cp$src:line.c, $(line_depend),-
	cp$src:login_cmd.c, $(login_cmd_depend),-
	cp$src:ls.c, $(ls_depend),-
	cp$src:more.c, $(more_depend),-
	cp$src:net.c, $(net_depend),-
	cp$src:prcache.c, $(prcache_depend),-
	cp$src:prcache_pci.c, $(prcache_pci_depend),-
	cp$src:ps.c, $(ps_depend),-
	cp$src:rm.c, $(rm_depend),-
	cp$src:sem_cmd.c, $(sem_cmd_depend),-
	cp$src:sleep.c, $(sleep_depend),-
	cp$src:sort.c, $(sort_depend),-
	cp$src:sound.c, $(sound_depend),-
	cp$src:truefalse.c, $(truefalse_depend),-
	cp$src:validate.c, $(validate_depend),-
	cp$src:wc.c, $(wc_depend),-
	cp$src:lfu.c, $(lfu_depend)
 $(cc) $(cdefs) $(cquals) $(copt) $(list) -
	/plus/obj=cp$obj:grp_command.obj-
	/lis=cp$lis:grp_command.lis-
	cp$src:blex.c+bshell+bshell_cmd+boot+builtins+build_fru+date_cmd+exdep+iobq_cmd+isacfg+isp1020_edit+readline+run_cmd+show_hwrpb+show_map+setshow+startstop+tr+uptime+xlate+nautilus_nbridge+api_debug+nautilus_info+dp264_fru+allocfree+bpt+cat-
	+check+chmod+cmp+element+echo+edit+eval+exer+find_field+grep+galaxy+galaxy_nautilus+galaxy_config_tree+hd+help+line+login_cmd+ls+more+net+prcache+prcache_pci+ps+rm+sem_cmd+sleep+sort+sound+truefalse+validate+wc+lfu
	! Group "command" command length was ~578

!
! Group "driver" (77 files)
!
cp$obj:grp_driver.obj : -
	cp$src:hwrpbtt_driver.c, $(hwrpbtt_driver_depend),-
	cp$src:mopdl_driver.c, $(mopdl_driver_depend),-
	cp$src:moplp_driver.c, $(moplp_driver_depend),-
	cp$src:ni_datalink.c, $(ni_datalink_depend),-
	cp$src:moprc_driver.c, $(moprc_driver_depend),-
	cp$src:moprctt_driver.c, $(moprctt_driver_depend),-
	cp$src:simport.c, $(simport_depend),-
	cp$src:nl_driver.c, $(nl_driver_depend),-
	cp$src:rd_driver.c, $(rd_driver_depend),-
	cp$src:toy_driver.c, $(toy_driver_depend),-
	cp$src:pci_driver.c, $(pci_driver_depend),-
	cp$src:from_driver.c, $(from_driver_depend),-
	cp$src:nvram_driver.c, $(nvram_driver_depend),-
	cp$src:api_iic.c, $(api_iic_depend),-
	cp$src:tt_driver.c, $(tt_driver_depend),-
	cp$src:dv_driver.c, $(dv_driver_depend),-
	cp$src:vga_bios_driver.c, $(vga_bios_driver_depend),-
	cp$src:kbd_driver.c, $(kbd_driver_depend),-
	cp$src:combo_driver.c, $(combo_driver_depend),-
	cp$src:ev_driver.c, $(ev_driver_depend),-
	cp$src:el_driver.c, $(el_driver_depend),-
	cp$src:pmem_driver.c, $(pmem_driver_depend),-
	cp$src:vmem_driver.c, $(vmem_driver_depend),-
	cp$src:examine_driver.c, $(examine_driver_depend),-
	cp$src:ev6_ipr_driver.c, $(ev6_ipr_driver_depend),-
	cp$src:gpr_driver.c, $(gpr_driver_depend),-
	cp$src:ps_driver.c, $(ps_driver_depend),-
	cp$src:ndbr_driver.c, $(ndbr_driver_depend),-
	cp$src:mop_driver.c, $(mop_driver_depend),-
	cp$src:fpr_driver.c, $(fpr_driver_depend),-
	cp$src:pi_driver.c, $(pi_driver_depend),-
	cp$src:decode_driver.c, $(decode_driver_depend),-
	cp$src:tee_driver.c, $(tee_driver_depend),-
	cp$src:para_driver.c, $(para_driver_depend),-
	cp$src:inet_driver.c, $(inet_driver_depend),-
	cp$src:fat_driver.c, $(fat_driver_depend),-
	cp$src:m1543c_driver.c, $(m1543c_driver_depend),-
	cp$src:dq_driver.c, $(dq_driver_depend),-
	cp$src:de205_driver.c, $(de205_driver_depend),-
	cp$src:n810_driver.c, $(n810_driver_depend),-
	cp$src:pke_driver.c, $(pke_driver_depend),-
	cp$src:pci_nvram_driver.c, $(pci_nvram_driver_depend),-
	cp$src:pdq_driver.c, $(pdq_driver_depend),-
	cp$src:dac960_driver.c, $(dac960_driver_depend),-
	cp$src:pks_driver.c, $(pks_driver_depend),-
	cp$src:isp1020_driver.c, $(isp1020_driver_depend),-
	cp$src:ew_driver.c, $(ew_driver_depend),-
	cp$src:ei_driver.c, $(ei_driver_depend),-
	cp$src:nport_q.c, $(nport_q_depend),-
	cp$src:cipca_common.c, $(cipca_common_depend),-
	cp$src:cipca_response.c, $(cipca_response_depend),-
	cp$src:cipca_support.c, $(cipca_support_depend),-
	cp$src:cipca_io.c, $(cipca_io_depend),-
	cp$src:mscp.c, $(mscp_depend),-
	cp$src:cipca_driver.c, $(cipca_driver_depend),-
	cp$src:kgpsa_driver.c, $(kgpsa_driver_depend),-
	cp$src:wwidmgr.c, $(wwidmgr_depend),-
	cp$src:pue_driver.c, $(pue_driver_depend),-
	cp$src:mc_driver.c, $(mc_driver_depend),-
	cp$src:i2o_driver.c, $(i2o_driver_depend),-
	cp$src:i2oexec.c, $(i2oexec_depend),-
	cp$src:i2omsg.c, $(i2omsg_depend),-
	cp$src:i2outil.c, $(i2outil_depend),-
	cp$src:i2o_bsa.c, $(i2o_bsa_depend),-
	cp$src:i2o_bsa_osm.c, $(i2o_bsa_osm_depend),-
	cp$src:i2o_scsi_osm.c, $(i2o_scsi_osm_depend),-
	cp$src:iso9660_driver.c, $(iso9660_driver_depend),-
	cp$src:aic78xx_driver.c, $(aic78xx_driver_depend),-
	cp$src:aic78xx_him_hwm.c, $(aic78xx_him_hwm_depend),-
	cp$src:aic78xx_him_rsm.c, $(aic78xx_him_rsm_depend),-
	cp$src:aic78xx_him_xlm.c, $(aic78xx_him_xlm_depend),-
	cp$src:bios_disk_driver.c, $(bios_disk_driver_depend),-
	cp$src:lfu_nautilus_driver.c, $(lfu_nautilus_driver_depend),-
	cp$src:lfu_kzpsa_driver.c, $(lfu_kzpsa_driver_depend),-
	cp$src:lfu_defpa_defea_driver.c, $(lfu_defpa_defea_driver_depend),-
	cp$src:lfu_cipca_driver.c, $(lfu_cipca_driver_depend),-
	cp$src:lfu_mc2_driver.c, $(lfu_mc2_driver_depend)
 $(cc) $(cdefs) $(cquals) $(copt) $(list) -
	/plus/obj=cp$obj:grp_driver.obj-
	/lis=cp$lis:grp_driver.lis-
	cp$src:hwrpbtt_driver.c+mopdl_driver+moplp_driver+ni_datalink+moprc_driver+moprctt_driver+simport+nl_driver+rd_driver+toy_driver+pci_driver+from_driver+nvram_driver+api_iic+tt_driver+dv_driver+vga_bios_driver+kbd_driver+combo_driver-
	+ev_driver+el_driver+pmem_driver+vmem_driver+examine_driver+ev6_ipr_driver+gpr_driver+ps_driver+ndbr_driver+mop_driver+fpr_driver+pi_driver+decode_driver+tee_driver+para_driver+inet_driver+fat_driver+m1543c_driver+dq_driver+de205_driver-
	+n810_driver+pke_driver+pci_nvram_driver+pdq_driver+dac960_driver+pks_driver+isp1020_driver+ew_driver+ei_driver+nport_q+cipca_common+cipca_response+cipca_support+cipca_io+mscp+cipca_driver+kgpsa_driver+wwidmgr+pue_driver+mc_driver+i2o_driver-
	+i2oexec+i2omsg+i2outil+i2o_bsa+i2o_bsa_osm+i2o_scsi_osm+iso9660_driver+aic78xx_driver+aic78xx_him_hwm+aic78xx_him_rsm+aic78xx_him_xlm+bios_disk_driver+lfu_nautilus_driver+lfu_kzpsa_driver+lfu_defpa_defea_driver+lfu_cipca_driver-
	+lfu_mc2_driver
	! Warning: Group "driver" command length was ~1084
!
! Group "data" (1 files)
!
cp$obj:grp_data.obj : -
	cp$src:msgtxt.c, $(msgtxt_depend)
 $(cc) $(cdefs) $(cquals) $(copt) $(list) -
	/plus/obj=cp$obj:grp_data.obj-
	/lis=cp$lis:grp_data.lis-
	cp$src:msgtxt.c
	! Group "data" command length was ~131

.ENDIF

i_vlist_objects = -
	cp$obj:callbacks_alpha.obj,-
	cp$obj:kernel_alpha.obj,-
	cp$obj:pc264_util.obj,-
	cp$obj:inst_alpha.obj,-
	cp$obj:memtest_alpha.obj,-
	cp$obj:pke_script.obj,-
	cp$obj:pue_script.obj


.IFDEF opt
vlist_objects = $(i_vlist_objects), -
	cp$obj:grp_base.obj,-
	cp$obj:grp_command.obj,-
	cp$obj:grp_driver.obj,-
	cp$obj:grp_data.obj
.ELSE $(opt)
vlist_objects = $(i_vlist_objects), -
	cp$obj:alphamm.obj,-
	cp$obj:ansi.obj,-
	cp$obj:aputchar.obj,-
	cp$obj:atoi.obj,-
	cp$obj:blex.obj,-
	cp$obj:bshell.obj,-
	cp$obj:bshell_cmd.obj,-
	cp$obj:boot.obj,-
	cp$obj:builtins.obj,-
	cp$obj:build_fru.obj,-
	cp$obj:call_backs.obj,-
	cp$obj:date.obj,-
	cp$obj:date_cmd.obj,-
	cp$obj:dynamic.obj,-
	cp$obj:entry.obj,-
	cp$obj:ev_action.obj,-
	cp$obj:exdep.obj,-
	cp$obj:externdata.obj,-
	cp$obj:filesys.obj,-
	cp$obj:hwrpb.obj,-
	cp$obj:hwrpbtt_driver.obj,-
	cp$obj:ie.obj,-
	cp$obj:iobq_cmd.obj,-
	cp$obj:isacfg.obj,-
	cp$obj:isp1020_edit.obj,-
	cp$obj:kernel.obj,-
	cp$obj:kernel_support.obj,-
	cp$obj:memset.obj,-
	cp$obj:mopdl_driver.obj,-
	cp$obj:moplp_driver.obj,-
	cp$obj:msgtxt.obj,-
	cp$obj:ni_datalink.obj,-
	cp$obj:printf.obj,-
	cp$obj:probe_io.obj,-
	cp$obj:process.obj,-
	cp$obj:powerup.obj,-
	cp$obj:qscan.obj,-
	cp$obj:queue.obj,-
	cp$obj:random.obj,-
	cp$obj:readline.obj,-
	cp$obj:regexp.obj,-
	cp$obj:run_cmd.obj,-
	cp$obj:runbios.obj,-
	cp$obj:scsi.obj,-
	cp$obj:scs.obj,-
	cp$obj:semaphore.obj,-
	cp$obj:show_hwrpb.obj,-
	cp$obj:show_map.obj,-
	cp$obj:setshow.obj,-
	cp$obj:shellsort.obj,-
	cp$obj:startstop.obj,-
	cp$obj:strfunc.obj,-
	cp$obj:timer.obj,-
	cp$obj:tr.obj,-
	cp$obj:uptime.obj,-
	cp$obj:xlate.obj,-
	cp$obj:nautilus.obj,-
	cp$obj:nautilus_init.obj,-
	cp$obj:irongate_io.obj,-
	cp$obj:nautilus_io.obj,-
	cp$obj:nautilus_mcheck.obj,-
	cp$obj:nautilus_nbridge.obj,-
	cp$obj:memconfig_nautilus.obj,-
	cp$obj:show_config_nautilus.obj,-
	cp$obj:amdflash_support.obj,-
	cp$obj:m1543c_sbridge.obj,-
	cp$obj:m1543c_iic.obj,-
	cp$obj:m5229_ide.obj,-
	cp$obj:api_debug.obj,-
	cp$obj:nautilus_info.obj,-
	cp$obj:dp264_fru.obj,-
	cp$obj:allocfree.obj,-
	cp$obj:bpt.obj,-
	cp$obj:cat.obj,-
	cp$obj:check.obj,-
	cp$obj:chmod.obj,-
	cp$obj:cmp.obj,-
	cp$obj:decode_alpha.obj,-
	cp$obj:diag_support.obj,-
	cp$obj:diag_evs.obj,-
	cp$obj:element.obj,-
	cp$obj:echo.obj,-
	cp$obj:edit.obj,-
	cp$obj:eval.obj,-
	cp$obj:exer.obj,-
	cp$obj:find_field.obj,-
	cp$obj:grep.obj,-
	cp$obj:galaxy.obj,-
	cp$obj:galaxy_nautilus.obj,-
	cp$obj:galaxy_config_tree.obj,-
	cp$obj:hd.obj,-
	cp$obj:help.obj,-
	cp$obj:line.obj,-
	cp$obj:login_cmd.obj,-
	cp$obj:ls.obj,-
	cp$obj:moprc_driver.obj,-
	cp$obj:moprctt_driver.obj,-
	cp$obj:more.obj,-
	cp$obj:net.obj,-
	cp$obj:prcache.obj,-
	cp$obj:prcache_pci.obj,-
	cp$obj:ps.obj,-
	cp$obj:rm.obj,-
	cp$obj:secure.obj,-
	cp$obj:sem_cmd.obj,-
	cp$obj:simport.obj,-
	cp$obj:sleep.obj,-
	cp$obj:sort.obj,-
	cp$obj:sound.obj,-
	cp$obj:symbols.obj,-
	cp$obj:truefalse.obj,-
	cp$obj:validate.obj,-
	cp$obj:wc.obj,-
	cp$obj:memtest.obj,-
	cp$obj:memtest_gray_lw.obj,-
	cp$obj:memtest_march.obj,-
	cp$obj:memtest_random.obj,-
	cp$obj:memtest_victim.obj,-
	cp$obj:nettest.obj,-
	cp$obj:lfu.obj,-
	cp$obj:lfu_support_apc.obj,-
	cp$obj:nl_driver.obj,-
	cp$obj:rd_driver.obj,-
	cp$obj:toy_driver.obj,-
	cp$obj:pci_driver.obj,-
	cp$obj:from_driver.obj,-
	cp$obj:nvram_driver.obj,-
	cp$obj:api_iic.obj,-
	cp$obj:tt_driver.obj,-
	cp$obj:dv_driver.obj,-
	cp$obj:vga_bios_driver.obj,-
	cp$obj:kbd_driver.obj,-
	cp$obj:combo_driver.obj,-
	cp$obj:ev_driver.obj,-
	cp$obj:el_driver.obj,-
	cp$obj:pmem_driver.obj,-
	cp$obj:vmem_driver.obj,-
	cp$obj:examine_driver.obj,-
	cp$obj:ev6_ipr_driver.obj,-
	cp$obj:gpr_driver.obj,-
	cp$obj:ps_driver.obj,-
	cp$obj:ndbr_driver.obj,-
	cp$obj:mop_driver.obj,-
	cp$obj:fpr_driver.obj,-
	cp$obj:pi_driver.obj,-
	cp$obj:decode_driver.obj,-
	cp$obj:tee_driver.obj,-
	cp$obj:dup.obj,-
	cp$obj:para_driver.obj,-
	cp$obj:inet_driver.obj,-
	cp$obj:fat_driver.obj,-
	cp$obj:m1543c_driver.obj,-
	cp$obj:dq_driver.obj,-
	cp$obj:de205_driver.obj,-
	cp$obj:n810_driver.obj,-
	cp$obj:pke_driver.obj,-
	cp$obj:pci_nvram_driver.obj,-
	cp$obj:pdq_driver.obj,-
	cp$obj:dac960_driver.obj,-
	cp$obj:pks_driver.obj,-
	cp$obj:isp1020_driver.obj,-
	cp$obj:ew_driver.obj,-
	cp$obj:ei_driver.obj,-
	cp$obj:nport_q.obj,-
	cp$obj:cipca_common.obj,-
	cp$obj:cipca_response.obj,-
	cp$obj:cipca_support.obj,-
	cp$obj:cipca_io.obj,-
	cp$obj:mscp.obj,-
	cp$obj:cipca_driver.obj,-
	cp$obj:kgpsa_driver.obj,-
	cp$obj:wwidmgr.obj,-
	cp$obj:pue_driver.obj,-
	cp$obj:mc_driver.obj,-
	cp$obj:i2o_driver.obj,-
	cp$obj:i2oexec.obj,-
	cp$obj:i2omsg.obj,-
	cp$obj:i2outil.obj,-
	cp$obj:i2o_bsa.obj,-
	cp$obj:i2o_bsa_osm.obj,-
	cp$obj:i2o_scsi_osm.obj,-
	cp$obj:iso9660_driver.obj,-
	cp$obj:aic78xx_driver.obj,-
	cp$obj:aic78xx_him_hwm.obj,-
	cp$obj:aic78xx_him_rsm.obj,-
	cp$obj:aic78xx_him_xlm.obj,-
	cp$obj:bios_disk_driver.obj,-
	cp$obj:lfu_nautilus_driver.obj,-
	cp$obj:lfu_kzpsa_driver.obj,-
	cp$obj:lfu_defpa_defea_driver.obj,-
	cp$obj:lfu_cipca_driver.obj,-
	cp$obj:lfu_mc2_driver.obj
.ENDIF

slis = -
	cp$lis:alphamm.slis,-
	cp$lis:ansi.slis,-
	cp$lis:aputchar.slis,-
	cp$lis:atoi.slis,-
	cp$lis:blex.slis,-
	cp$lis:bshell.slis,-
	cp$lis:bshell_cmd.slis,-
	cp$lis:boot.slis,-
	cp$lis:builtins.slis,-
	cp$lis:build_fru.slis,-
	cp$lis:call_backs.slis,-
	cp$lis:callbacks_alpha.slis,-
	cp$lis:date.slis,-
	cp$lis:date_cmd.slis,-
	cp$lis:dynamic.slis,-
	cp$lis:entry.slis,-
	cp$lis:ev_action.slis,-
	cp$lis:exdep.slis,-
	cp$lis:externdata.slis,-
	cp$lis:filesys.slis,-
	cp$lis:hwrpb.slis,-
	cp$lis:hwrpbtt_driver.slis,-
	cp$lis:ie.slis,-
	cp$lis:iobq_cmd.slis,-
	cp$lis:isacfg.slis,-
	cp$lis:isp1020_edit.slis,-
	cp$lis:kernel.slis,-
	cp$lis:kernel_support.slis,-
	cp$lis:kernel_alpha.slis,-
	cp$lis:memset.slis,-
	cp$lis:mopdl_driver.slis,-
	cp$lis:moplp_driver.slis,-
	cp$lis:msgtxt.slis,-
	cp$lis:ni_datalink.slis,-
	cp$lis:printf.slis,-
	cp$lis:probe_io.slis,-
	cp$lis:process.slis,-
	cp$lis:powerup.slis,-
	cp$lis:qscan.slis,-
	cp$lis:queue.slis,-
	cp$lis:random.slis,-
	cp$lis:readline.slis,-
	cp$lis:regexp.slis,-
	cp$lis:run_cmd.slis,-
	cp$lis:runbios.slis,-
	cp$lis:scsi.slis,-
	cp$lis:scs.slis,-
	cp$lis:semaphore.slis,-
	cp$lis:show_hwrpb.slis,-
	cp$lis:show_map.slis,-
	cp$lis:setshow.slis,-
	cp$lis:shellsort.slis,-
	cp$lis:startstop.slis,-
	cp$lis:strfunc.slis,-
	cp$lis:timer.slis,-
	cp$lis:tr.slis,-
	cp$lis:uptime.slis,-
	cp$lis:xlate.slis,-
	cp$lis:nautilus.slis,-
	cp$lis:nautilus_init.slis,-
	cp$lis:irongate_io.slis,-
	cp$lis:nautilus_io.slis,-
	cp$lis:nautilus_mcheck.slis,-
	cp$lis:nautilus_nbridge.slis,-
	cp$lis:pc264_util.slis,-
	cp$lis:memconfig_nautilus.slis,-
	cp$lis:show_config_nautilus.slis,-
	cp$lis:amdflash_support.slis,-
	cp$lis:m1543c_sbridge.slis,-
	cp$lis:m1543c_iic.slis,-
	cp$lis:m5229_ide.slis,-
	cp$lis:api_debug.slis,-
	cp$lis:nautilus_info.slis,-
	cp$lis:dp264_fru.slis,-
	cp$lis:allocfree.slis,-
	cp$lis:bpt.slis,-
	cp$lis:cat.slis,-
	cp$lis:check.slis,-
	cp$lis:chmod.slis,-
	cp$lis:cmp.slis,-
	cp$lis:decode_alpha.slis,-
	cp$lis:diag_support.slis,-
	cp$lis:diag_evs.slis,-
	cp$lis:element.slis,-
	cp$lis:echo.slis,-
	cp$lis:edit.slis,-
	cp$lis:eval.slis,-
	cp$lis:exer.slis,-
	cp$lis:find_field.slis,-
	cp$lis:grep.slis,-
	cp$lis:galaxy.slis,-
	cp$lis:galaxy_nautilus.slis,-
	cp$lis:galaxy_config_tree.slis,-
	cp$lis:hd.slis,-
	cp$lis:help.slis,-
	cp$lis:inst_alpha.slis,-
	cp$lis:line.slis,-
	cp$lis:login_cmd.slis,-
	cp$lis:ls.slis,-
	cp$lis:moprc_driver.slis,-
	cp$lis:moprctt_driver.slis,-
	cp$lis:more.slis,-
	cp$lis:net.slis,-
	cp$lis:prcache.slis,-
	cp$lis:prcache_pci.slis,-
	cp$lis:ps.slis,-
	cp$lis:rm.slis,-
	cp$lis:secure.slis,-
	cp$lis:sem_cmd.slis,-
	cp$lis:simport.slis,-
	cp$lis:sleep.slis,-
	cp$lis:sort.slis,-
	cp$lis:sound.slis,-
	cp$lis:symbols.slis,-
	cp$lis:truefalse.slis,-
	cp$lis:validate.slis,-
	cp$lis:wc.slis,-
	cp$lis:memtest.slis,-
	cp$lis:memtest_gray_lw.slis,-
	cp$lis:memtest_alpha.slis,-
	cp$lis:memtest_march.slis,-
	cp$lis:memtest_random.slis,-
	cp$lis:memtest_victim.slis,-
	cp$lis:nettest.slis,-
	cp$lis:lfu.slis,-
	cp$lis:lfu_support_apc.slis,-
	cp$lis:nl_driver.slis,-
	cp$lis:rd_driver.slis,-
	cp$lis:toy_driver.slis,-
	cp$lis:pci_driver.slis,-
	cp$lis:from_driver.slis,-
	cp$lis:nvram_driver.slis,-
	cp$lis:api_iic.slis,-
	cp$lis:tt_driver.slis,-
	cp$lis:dv_driver.slis,-
	cp$lis:vga_bios_driver.slis,-
	cp$lis:kbd_driver.slis,-
	cp$lis:combo_driver.slis,-
	cp$lis:ev_driver.slis,-
	cp$lis:el_driver.slis,-
	cp$lis:pmem_driver.slis,-
	cp$lis:vmem_driver.slis,-
	cp$lis:examine_driver.slis,-
	cp$lis:ev6_ipr_driver.slis,-
	cp$lis:gpr_driver.slis,-
	cp$lis:ps_driver.slis,-
	cp$lis:ndbr_driver.slis,-
	cp$lis:mop_driver.slis,-
	cp$lis:fpr_driver.slis,-
	cp$lis:pi_driver.slis,-
	cp$lis:decode_driver.slis,-
	cp$lis:tee_driver.slis,-
	cp$lis:dup.slis,-
	cp$lis:para_driver.slis,-
	cp$lis:inet_driver.slis,-
	cp$lis:fat_driver.slis,-
	cp$lis:m1543c_driver.slis,-
	cp$lis:dq_driver.slis,-
	cp$lis:de205_driver.slis,-
	cp$lis:n810_driver.slis,-
	cp$lis:pke_driver.slis,-
	cp$lis:pke_script.slis,-
	cp$lis:pci_nvram_driver.slis,-
	cp$lis:pdq_driver.slis,-
	cp$lis:dac960_driver.slis,-
	cp$lis:pks_driver.slis,-
	cp$lis:isp1020_driver.slis,-
	cp$lis:ew_driver.slis,-
	cp$lis:ei_driver.slis,-
	cp$lis:nport_q.slis,-
	cp$lis:cipca_common.slis,-
	cp$lis:cipca_response.slis,-
	cp$lis:cipca_support.slis,-
	cp$lis:cipca_io.slis,-
	cp$lis:mscp.slis,-
	cp$lis:cipca_driver.slis,-
	cp$lis:kgpsa_driver.slis,-
	cp$lis:wwidmgr.slis,-
	cp$lis:pue_driver.slis,-
	cp$lis:pue_script.slis,-
	cp$lis:mc_driver.slis,-
	cp$lis:i2o_driver.slis,-
	cp$lis:i2oexec.slis,-
	cp$lis:i2omsg.slis,-
	cp$lis:i2outil.slis,-
	cp$lis:i2o_bsa.slis,-
	cp$lis:i2o_bsa_osm.slis,-
	cp$lis:i2o_scsi_osm.slis,-
	cp$lis:iso9660_driver.slis,-
	cp$lis:aic78xx_driver.slis,-
	cp$lis:aic78xx_him_hwm.slis,-
	cp$lis:aic78xx_him_rsm.slis,-
	cp$lis:aic78xx_him_xlm.slis,-
	cp$lis:bios_disk_driver.slis,-
	cp$lis:lfu_nautilus_driver.slis,-
	cp$lis:lfu_kzpsa_driver.slis,-
	cp$lis:lfu_defpa_defea_driver.slis,-
	cp$lis:lfu_cipca_driver.slis,-
	cp$lis:lfu_mc2_driver.slis

! Encapsulation dependencies

cp$obj:encap_add_de205.obj : cp$src:isacfg_de205
	mcr cp$kits:[encapsulate.new]encapsulate cp$src:isacfg_de205 cp$src:encap_add_de205.mar encap_add_de205 -script
	$(macro64)/nolis/obj=cp$obj cp$src:encap_add_de205.mar

cp$obj:encap_memexer.obj : cp$src:memexer_dp264
	mcr cp$kits:[encapsulate.new]encapsulate cp$src:memexer_dp264 cp$src:encap_memexer.mar encap_memexer -script
	$(macro64)/nolis/obj=cp$obj cp$src:encap_memexer.mar

cp$obj:encap_testmem.obj : cp$src:testmem_dp264
	mcr cp$kits:[encapsulate.new]encapsulate cp$src:testmem_dp264 cp$src:encap_testmem.mar encap_testmem -script
	$(macro64)/nolis/obj=cp$obj cp$src:encap_testmem.mar

cp$obj:encap_sys_exer.obj : cp$src:sys_exer_eb164
	mcr cp$kits:[encapsulate.new]encapsulate cp$src:sys_exer_eb164 cp$src:encap_sys_exer.mar encap_sys_exer -script
	$(macro64)/nolis/obj=cp$obj cp$src:encap_sys_exer.mar

cp$obj:encap_show_status.obj : cp$src:show_status
	mcr cp$kits:[encapsulate.new]encapsulate cp$src:show_status cp$src:encap_show_status.mar encap_show_status -script
	$(macro64)/nolis/obj=cp$obj cp$src:encap_show_status.mar

cp$obj:encap_kill_diags.obj : cp$src:kill_diags_eb164
	mcr cp$kits:[encapsulate.new]encapsulate cp$src:kill_diags_eb164 cp$src:encap_kill_diags.mar encap_kill_diags -script
	$(macro64)/nolis/obj=cp$obj cp$src:encap_kill_diags.mar

cp$obj:encap_showit.obj : cp$src:showit
	mcr cp$kits:[encapsulate.new]encapsulate cp$src:showit cp$src:encap_showit.mar encap_showit -script
	$(macro64)/nolis/obj=cp$obj cp$src:encap_showit.mar

cp$obj:encap_pwrup.obj : cp$src:pwrup_nautilus
	mcr cp$kits:[encapsulate.new]encapsulate cp$src:pwrup_nautilus cp$src:encap_pwrup.mar encap_pwrup -script
	$(macro64)/nolis/obj=cp$obj cp$src:encap_pwrup.mar

cp$obj:encap_test.obj : cp$src:dp264_test
	mcr cp$kits:[encapsulate.new]encapsulate cp$src:dp264_test cp$src:encap_test.mar encap_test -script
	$(macro64)/nolis/obj=cp$obj cp$src:encap_test.mar

cp$obj:encap_mfgtest.obj : cp$src:nautilus_mfgtest
	mcr cp$kits:[encapsulate.new]encapsulate cp$src:nautilus_mfgtest cp$src:encap_mfgtest.mar encap_mfgtest -script
	$(macro64)/nolis/obj=cp$obj cp$src:encap_mfgtest.mar

cp$obj:encap_set_flag.obj : cp$src:set_flag_apc
	mcr cp$kits:[encapsulate.new]encapsulate cp$src:set_flag_apc cp$src:encap_set_flag.mar encap_set_flag -script
	$(macro64)/nolis/obj=cp$obj cp$src:encap_set_flag.mar

cp$obj:encap_fwupdate.obj : cp$src:dp_fwupdate
	mcr cp$kits:[encapsulate.new]encapsulate cp$src:dp_fwupdate cp$src:encap_fwupdate.mar encap_fwupdate -script
	$(macro64)/nolis/obj=cp$obj cp$src:encap_fwupdate.mar

cp$obj:encap_readme.obj : cp$src:pc264_readme
	mcr cp$kits:[encapsulate.new]encapsulate cp$src:pc264_readme cp$src:encap_readme.mar encap_readme -script
	$(macro64)/nolis/obj=cp$obj cp$src:encap_readme.mar

cp$obj:encap_save_nvram.obj : cp$src:save_nvram_pc264
	mcr cp$kits:[encapsulate.new]encapsulate cp$src:save_nvram_pc264 cp$src:encap_save_nvram.mar encap_save_nvram -script
	$(macro64)/nolis/obj=cp$obj cp$src:encap_save_nvram.mar

cp$obj:encap_restore_nvram.obj : cp$src:restore_nvram_pc264
	mcr cp$kits:[encapsulate.new]encapsulate cp$src:restore_nvram_pc264 cp$src:encap_restore_nvram.mar encap_restore_nvram -script
	$(macro64)/nolis/obj=cp$obj cp$src:encap_restore_nvram.mar


! C file dependencies

cp$obj:alphamm.obj : cp$src:alphamm.c, $(alphamm_depend)
cp$obj:ansi.obj : cp$src:ansi.c, $(ansi_depend)
cp$obj:aputchar.obj : cp$src:aputchar.c, $(aputchar_depend)
cp$obj:atoi.obj : cp$src:atoi.c, $(atoi_depend)
cp$obj:blex.obj : cp$src:blex.c, $(blex_depend)
cp$obj:bshell.obj : cp$src:bshell.c, $(bshell_depend)
cp$obj:bshell_cmd.obj : cp$src:bshell_cmd.c, $(bshell_cmd_depend)
cp$obj:boot.obj : cp$src:boot.c, $(boot_depend)
cp$obj:builtins.obj : cp$src:builtins.c, $(builtins_depend)
cp$obj:build_fru.obj : cp$src:build_fru.c, $(build_fru_depend)
cp$obj:call_backs.obj : cp$src:call_backs.c, $(call_backs_depend)
cp$obj:date.obj : cp$src:date.c, $(date_depend)
cp$obj:date_cmd.obj : cp$src:date_cmd.c, $(date_cmd_depend)
cp$obj:dynamic.obj : cp$src:dynamic.c, $(dynamic_depend)
cp$obj:entry.obj : cp$src:entry.c, $(entry_depend)
cp$obj:ev_action.obj : cp$src:ev_action.c, $(ev_action_depend)
cp$obj:exdep.obj : cp$src:exdep.c, $(exdep_depend)
cp$obj:externdata.obj : cp$src:externdata.c, $(externdata_depend)
cp$obj:filesys.obj : cp$src:filesys.c, $(filesys_depend)
cp$obj:hwrpb.obj : cp$src:hwrpb.c, $(hwrpb_depend)
cp$obj:hwrpbtt_driver.obj : cp$src:hwrpbtt_driver.c, $(hwrpbtt_driver_depend)
cp$obj:ie.obj : cp$src:ie.c, $(ie_depend)
cp$obj:iobq_cmd.obj : cp$src:iobq_cmd.c, $(iobq_cmd_depend)
cp$obj:isacfg.obj : cp$src:isacfg.c, $(isacfg_depend)
cp$obj:isp1020_edit.obj : cp$src:isp1020_edit.c, $(isp1020_edit_depend)
cp$obj:kernel.obj : cp$src:kernel.c, $(kernel_depend)
cp$obj:kernel_support.obj : cp$src:kernel_support.c, $(kernel_support_depend)
cp$obj:memset.obj : cp$src:memset.c, $(memset_depend)
cp$obj:mopdl_driver.obj : cp$src:mopdl_driver.c, $(mopdl_driver_depend)
cp$obj:moplp_driver.obj : cp$src:moplp_driver.c, $(moplp_driver_depend)
cp$obj:msgtxt.obj : cp$src:msgtxt.c, $(msgtxt_depend)
cp$obj:ni_datalink.obj : cp$src:ni_datalink.c, $(ni_datalink_depend)
cp$obj:printf.obj : cp$src:printf.c, $(printf_depend)
cp$obj:probe_io.obj : cp$src:probe_io.c, $(probe_io_depend)
cp$obj:process.obj : cp$src:process.c, $(process_depend)
cp$obj:powerup.obj : cp$src:powerup.c, $(powerup_depend)
cp$obj:qscan.obj : cp$src:qscan.c, $(qscan_depend)
cp$obj:queue.obj : cp$src:queue.c, $(queue_depend)
cp$obj:random.obj : cp$src:random.c, $(random_depend)
cp$obj:readline.obj : cp$src:readline.c, $(readline_depend)
cp$obj:regexp.obj : cp$src:regexp.c, $(regexp_depend)
cp$obj:run_cmd.obj : cp$src:run_cmd.c, $(run_cmd_depend)
cp$obj:runbios.obj : cp$src:runbios.c, $(runbios_depend)
cp$obj:scsi.obj : cp$src:scsi.c, $(scsi_depend)
cp$obj:scs.obj : cp$src:scs.c, $(scs_depend)
cp$obj:semaphore.obj : cp$src:semaphore.c, $(semaphore_depend)
cp$obj:show_hwrpb.obj : cp$src:show_hwrpb.c, $(show_hwrpb_depend)
cp$obj:show_map.obj : cp$src:show_map.c, $(show_map_depend)
cp$obj:setshow.obj : cp$src:setshow.c, $(setshow_depend)
cp$obj:shellsort.obj : cp$src:shellsort.c, $(shellsort_depend)
cp$obj:startstop.obj : cp$src:startstop.c, $(startstop_depend)
cp$obj:strfunc.obj : cp$src:strfunc.c, $(strfunc_depend)
cp$obj:timer.obj : cp$src:timer.c, $(timer_depend)
cp$obj:tr.obj : cp$src:tr.c, $(tr_depend)
cp$obj:uptime.obj : cp$src:uptime.c, $(uptime_depend)
cp$obj:xlate.obj : cp$src:xlate.c, $(xlate_depend)
cp$obj:nautilus.obj : cp$src:nautilus.c, $(nautilus_depend)
cp$obj:nautilus_init.obj : cp$src:nautilus_init.c, $(nautilus_init_depend)
cp$obj:irongate_io.obj : cp$src:irongate_io.c, $(irongate_io_depend)
cp$obj:nautilus_io.obj : cp$src:nautilus_io.c, $(nautilus_io_depend)
cp$obj:nautilus_mcheck.obj : cp$src:nautilus_mcheck.c, $(nautilus_mcheck_depend)
cp$obj:nautilus_nbridge.obj : cp$src:nautilus_nbridge.c, $(nautilus_nbridge_depend)
cp$obj:memconfig_nautilus.obj : cp$src:memconfig_nautilus.c, $(memconfig_nautilus_depend)
cp$obj:show_config_nautilus.obj : cp$src:show_config_nautilus.c, $(show_config_nautilus_depend)
cp$obj:amdflash_support.obj : cp$src:amdflash_support.c, $(amdflash_support_depend)
cp$obj:m1543c_sbridge.obj : cp$src:m1543c_sbridge.c, $(m1543c_sbridge_depend)
cp$obj:m1543c_iic.obj : cp$src:m1543c_iic.c, $(m1543c_iic_depend)
cp$obj:m5229_ide.obj : cp$src:m5229_ide.c, $(m5229_ide_depend)
cp$obj:api_debug.obj : cp$src:api_debug.c, $(api_debug_depend)
cp$obj:nautilus_info.obj : cp$src:nautilus_info.c, $(nautilus_info_depend)
cp$obj:dp264_fru.obj : cp$src:dp264_fru.c, $(dp264_fru_depend)
cp$obj:allocfree.obj : cp$src:allocfree.c, $(allocfree_depend)
cp$obj:bpt.obj : cp$src:bpt.c, $(bpt_depend)
cp$obj:cat.obj : cp$src:cat.c, $(cat_depend)
cp$obj:check.obj : cp$src:check.c, $(check_depend)
cp$obj:chmod.obj : cp$src:chmod.c, $(chmod_depend)
cp$obj:cmp.obj : cp$src:cmp.c, $(cmp_depend)
cp$obj:decode_alpha.obj : cp$src:decode_alpha.c, $(decode_alpha_depend)
cp$obj:diag_support.obj : cp$src:diag_support.c, $(diag_support_depend)
cp$obj:diag_evs.obj : cp$src:diag_evs.c, $(diag_evs_depend)
cp$obj:element.obj : cp$src:element.c, $(element_depend)
cp$obj:echo.obj : cp$src:echo.c, $(echo_depend)
cp$obj:edit.obj : cp$src:edit.c, $(edit_depend)
cp$obj:eval.obj : cp$src:eval.c, $(eval_depend)
cp$obj:exer.obj : cp$src:exer.c, $(exer_depend)
cp$obj:find_field.obj : cp$src:find_field.c, $(find_field_depend)
cp$obj:grep.obj : cp$src:grep.c, $(grep_depend)
cp$obj:galaxy.obj : cp$src:galaxy.c, $(galaxy_depend)
cp$obj:galaxy_nautilus.obj : cp$src:galaxy_nautilus.c, $(galaxy_nautilus_depend)
cp$obj:galaxy_config_tree.obj : cp$src:galaxy_config_tree.c, $(galaxy_config_tree_depend)
cp$obj:hd.obj : cp$src:hd.c, $(hd_depend)
cp$obj:help.obj : cp$src:help.c, $(help_depend)
cp$obj:line.obj : cp$src:line.c, $(line_depend)
cp$obj:login_cmd.obj : cp$src:login_cmd.c, $(login_cmd_depend)
cp$obj:ls.obj : cp$src:ls.c, $(ls_depend)
cp$obj:moprc_driver.obj : cp$src:moprc_driver.c, $(moprc_driver_depend)
cp$obj:moprctt_driver.obj : cp$src:moprctt_driver.c, $(moprctt_driver_depend)
cp$obj:more.obj : cp$src:more.c, $(more_depend)
cp$obj:net.obj : cp$src:net.c, $(net_depend)
cp$obj:prcache.obj : cp$src:prcache.c, $(prcache_depend)
cp$obj:prcache_pci.obj : cp$src:prcache_pci.c, $(prcache_pci_depend)
cp$obj:ps.obj : cp$src:ps.c, $(ps_depend)
cp$obj:rm.obj : cp$src:rm.c, $(rm_depend)
cp$obj:secure.obj : cp$src:secure.c, $(secure_depend)
cp$obj:sem_cmd.obj : cp$src:sem_cmd.c, $(sem_cmd_depend)
cp$obj:simport.obj : cp$src:simport.c, $(simport_depend)
cp$obj:sleep.obj : cp$src:sleep.c, $(sleep_depend)
cp$obj:sort.obj : cp$src:sort.c, $(sort_depend)
cp$obj:sound.obj : cp$src:sound.c, $(sound_depend)
cp$obj:symbols.obj : cp$src:symbols.c, $(symbols_depend)
cp$obj:truefalse.obj : cp$src:truefalse.c, $(truefalse_depend)
cp$obj:validate.obj : cp$src:validate.c, $(validate_depend)
cp$obj:wc.obj : cp$src:wc.c, $(wc_depend)
cp$obj:memtest.obj : cp$src:memtest.c, $(memtest_depend)
cp$obj:memtest_gray_lw.obj : cp$src:memtest_gray_lw.c, $(memtest_gray_lw_depend)
cp$obj:memtest_march.obj : cp$src:memtest_march.c, $(memtest_march_depend)
cp$obj:memtest_random.obj : cp$src:memtest_random.c, $(memtest_random_depend)
cp$obj:memtest_victim.obj : cp$src:memtest_victim.c, $(memtest_victim_depend)
cp$obj:nettest.obj : cp$src:nettest.c, $(nettest_depend)
cp$obj:lfu.obj : cp$src:lfu.c, $(lfu_depend)
cp$obj:lfu_support_apc.obj : cp$src:lfu_support_apc.c, $(lfu_support_apc_depend)
cp$obj:nl_driver.obj : cp$src:nl_driver.c, $(nl_driver_depend)
cp$obj:rd_driver.obj : cp$src:rd_driver.c, $(rd_driver_depend)
cp$obj:toy_driver.obj : cp$src:toy_driver.c, $(toy_driver_depend)
cp$obj:pci_driver.obj : cp$src:pci_driver.c, $(pci_driver_depend)
cp$obj:from_driver.obj : cp$src:from_driver.c, $(from_driver_depend)
cp$obj:nvram_driver.obj : cp$src:nvram_driver.c, $(nvram_driver_depend)
cp$obj:api_iic.obj : cp$src:api_iic.c, $(api_iic_depend)
cp$obj:tt_driver.obj : cp$src:tt_driver.c, $(tt_driver_depend)
cp$obj:dv_driver.obj : cp$src:dv_driver.c, $(dv_driver_depend)
cp$obj:vga_bios_driver.obj : cp$src:vga_bios_driver.c, $(vga_bios_driver_depend)
cp$obj:kbd_driver.obj : cp$src:kbd_driver.c, $(kbd_driver_depend)
cp$obj:combo_driver.obj : cp$src:combo_driver.c, $(combo_driver_depend)
cp$obj:ev_driver.obj : cp$src:ev_driver.c, $(ev_driver_depend)
cp$obj:el_driver.obj : cp$src:el_driver.c, $(el_driver_depend)
cp$obj:pmem_driver.obj : cp$src:pmem_driver.c, $(pmem_driver_depend)
cp$obj:vmem_driver.obj : cp$src:vmem_driver.c, $(vmem_driver_depend)
cp$obj:examine_driver.obj : cp$src:examine_driver.c, $(examine_driver_depend)
cp$obj:ev6_ipr_driver.obj : cp$src:ev6_ipr_driver.c, $(ev6_ipr_driver_depend)
cp$obj:gpr_driver.obj : cp$src:gpr_driver.c, $(gpr_driver_depend)
cp$obj:ps_driver.obj : cp$src:ps_driver.c, $(ps_driver_depend)
cp$obj:ndbr_driver.obj : cp$src:ndbr_driver.c, $(ndbr_driver_depend)
cp$obj:mop_driver.obj : cp$src:mop_driver.c, $(mop_driver_depend)
cp$obj:fpr_driver.obj : cp$src:fpr_driver.c, $(fpr_driver_depend)
cp$obj:pi_driver.obj : cp$src:pi_driver.c, $(pi_driver_depend)
cp$obj:decode_driver.obj : cp$src:decode_driver.c, $(decode_driver_depend)
cp$obj:tee_driver.obj : cp$src:tee_driver.c, $(tee_driver_depend)
cp$obj:dup.obj : cp$src:dup.c, $(dup_depend)
cp$obj:para_driver.obj : cp$src:para_driver.c, $(para_driver_depend)
cp$obj:inet_driver.obj : cp$src:inet_driver.c, $(inet_driver_depend)
cp$obj:fat_driver.obj : cp$src:fat_driver.c, $(fat_driver_depend)
cp$obj:m1543c_driver.obj : cp$src:m1543c_driver.c, $(m1543c_driver_depend)
cp$obj:dq_driver.obj : cp$src:dq_driver.c, $(dq_driver_depend)
cp$obj:de205_driver.obj : cp$src:de205_driver.c, $(de205_driver_depend)
cp$obj:n810_driver.obj : cp$src:n810_driver.c, $(n810_driver_depend)
cp$obj:pke_driver.obj : cp$src:pke_driver.c, $(pke_driver_depend)
cp$obj:pci_nvram_driver.obj : cp$src:pci_nvram_driver.c, $(pci_nvram_driver_depend)
cp$obj:pdq_driver.obj : cp$src:pdq_driver.c, $(pdq_driver_depend)
cp$obj:dac960_driver.obj : cp$src:dac960_driver.c, $(dac960_driver_depend)
cp$obj:pks_driver.obj : cp$src:pks_driver.c, $(pks_driver_depend)
cp$obj:isp1020_driver.obj : cp$src:isp1020_driver.c, $(isp1020_driver_depend)
cp$obj:ew_driver.obj : cp$src:ew_driver.c, $(ew_driver_depend)
cp$obj:ei_driver.obj : cp$src:ei_driver.c, $(ei_driver_depend)
cp$obj:nport_q.obj : cp$src:nport_q.c, $(nport_q_depend)
cp$obj:cipca_common.obj : cp$src:cipca_common.c, $(cipca_common_depend)
cp$obj:cipca_response.obj : cp$src:cipca_response.c, $(cipca_response_depend)
cp$obj:cipca_support.obj : cp$src:cipca_support.c, $(cipca_support_depend)
cp$obj:cipca_io.obj : cp$src:cipca_io.c, $(cipca_io_depend)
cp$obj:mscp.obj : cp$src:mscp.c, $(mscp_depend)
cp$obj:cipca_driver.obj : cp$src:cipca_driver.c, $(cipca_driver_depend)
cp$obj:kgpsa_driver.obj : cp$src:kgpsa_driver.c, $(kgpsa_driver_depend)
cp$obj:wwidmgr.obj : cp$src:wwidmgr.c, $(wwidmgr_depend)
cp$obj:pue_driver.obj : cp$src:pue_driver.c, $(pue_driver_depend)
cp$obj:mc_driver.obj : cp$src:mc_driver.c, $(mc_driver_depend)
cp$obj:i2o_driver.obj : cp$src:i2o_driver.c, $(i2o_driver_depend)
cp$obj:i2oexec.obj : cp$src:i2oexec.c, $(i2oexec_depend)
cp$obj:i2omsg.obj : cp$src:i2omsg.c, $(i2omsg_depend)
cp$obj:i2outil.obj : cp$src:i2outil.c, $(i2outil_depend)
cp$obj:i2o_bsa.obj : cp$src:i2o_bsa.c, $(i2o_bsa_depend)
cp$obj:i2o_bsa_osm.obj : cp$src:i2o_bsa_osm.c, $(i2o_bsa_osm_depend)
cp$obj:i2o_scsi_osm.obj : cp$src:i2o_scsi_osm.c, $(i2o_scsi_osm_depend)
cp$obj:iso9660_driver.obj : cp$src:iso9660_driver.c, $(iso9660_driver_depend)
cp$obj:aic78xx_driver.obj : cp$src:aic78xx_driver.c, $(aic78xx_driver_depend)
cp$obj:aic78xx_him_hwm.obj : cp$src:aic78xx_him_hwm.c, $(aic78xx_him_hwm_depend)
cp$obj:aic78xx_him_rsm.obj : cp$src:aic78xx_him_rsm.c, $(aic78xx_him_rsm_depend)
cp$obj:aic78xx_him_xlm.obj : cp$src:aic78xx_him_xlm.c, $(aic78xx_him_xlm_depend)
cp$obj:bios_disk_driver.obj : cp$src:bios_disk_driver.c, $(bios_disk_driver_depend)
cp$obj:lfu_nautilus_driver.obj : cp$src:lfu_nautilus_driver.c, $(lfu_nautilus_driver_depend)
cp$obj:lfu_kzpsa_driver.obj : cp$src:lfu_kzpsa_driver.c, $(lfu_kzpsa_driver_depend)
cp$obj:lfu_defpa_defea_driver.obj : cp$src:lfu_defpa_defea_driver.c, $(lfu_defpa_defea_driver_depend)
cp$obj:lfu_cipca_driver.obj : cp$src:lfu_cipca_driver.c, $(lfu_cipca_driver_depend)
cp$obj:lfu_mc2_driver.obj : cp$src:lfu_mc2_driver.c, $(lfu_mc2_driver_depend)
