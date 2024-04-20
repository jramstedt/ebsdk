! LX164 Target-specific dependencies
i_objects = -
	cp$obj:callbacks_alpha.obj,-
	cp$obj:pke_script.obj,-
	cp$obj:lx164_util.obj,-
	cp$obj:allocfree.obj,-
	cp$obj:bpt.obj,-
	cp$obj:cat.obj,-
	cp$obj:check.obj,-
	cp$obj:chmod.obj,-
	cp$obj:cmp.obj,-
	cp$obj:decode_alpha.obj,-
	cp$obj:diag_support.obj,-
	cp$obj:diag_evs.obj,-
	cp$obj:echo.obj,-
	cp$obj:edit.obj,-
	cp$obj:eval.obj,-
	cp$obj:exer.obj,-
	cp$obj:find_field.obj,-
	cp$obj:grep.obj,-
	cp$obj:hd.obj,-
	cp$obj:help.obj,-
	cp$obj:inst_alpha.obj,-
	cp$obj:line.obj,-
	cp$obj:login_cmd.obj,-
	cp$obj:ls.obj,-
	cp$obj:moprc_driver.obj,-
	cp$obj:moprctt_driver.obj,-
	cp$obj:more.obj,-
	cp$obj:net.obj,-
	cp$obj:ps.obj,-
	cp$obj:rm.obj,-
	cp$obj:secure.obj,-
	cp$obj:sem_cmd.obj,-
	cp$obj:simport.obj,-
	cp$obj:sleep.obj,-
	cp$obj:sort.obj,-
	cp$obj:sound.obj,-
	cp$obj:symbols.obj,-
	cp$obj:tms380fw.obj,-
	cp$obj:tr.obj,-
	cp$obj:truefalse.obj,-
	cp$obj:validate.obj,-
	cp$obj:wc.obj,-
	cp$obj:memtest.obj,-
	cp$obj:memtest_gray_lw.obj,-
	cp$obj:memtest_alpha.obj,-
	cp$obj:memtest_march.obj,-
	cp$obj:memtest_random.obj,-
	cp$obj:memtest_victim.obj,-
	cp$obj:nettest.obj,-
	cp$obj:encap_fwupdate.obj,-
	cp$obj:encap_add_de205.obj,-
	cp$obj:encap_test.obj,-
	cp$obj:encap_memexer.obj,-
	cp$obj:encap_sys_exer.obj,-
	cp$obj:encap_floppy.obj,-
	cp$obj:encap_floppy_write.obj,-
	cp$obj:encap_network.obj,-
	cp$obj:encap_exer_read.obj,-
	cp$obj:encap_exer_write.obj,-
	cp$obj:encap_tgax.obj,-
	cp$obj:encap_show_status.obj,-
	cp$obj:encap_kill_diags.obj,-
	cp$obj:encap_showit.obj,-
	cp$obj:encap_dqtest.obj

.IFDEF opt
objects = $(i_objects), -
 	cp$obj:grp_base.obj,-
 	cp$obj:grp_driver.obj,-
 	cp$obj:grp_data.obj,-
 	cp$obj:grp_0.obj
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
	cp$obj:call_backs.obj,-
	cp$obj:date.obj,-
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
	cp$obj:kernel_alpha.obj,-
	cp$obj:memset.obj,-
	cp$obj:mopdl_driver.obj,-
	cp$obj:moplp_driver.obj,-
	cp$obj:msgtxt.obj,-
	cp$obj:ni_datalink.obj,-
	cp$obj:pke_driver.obj,-
	cp$obj:printf.obj,-
	cp$obj:probe_io.obj,-
	cp$obj:process.obj,-
	cp$obj:qscan.obj,-
	cp$obj:queue.obj,-
	cp$obj:random.obj,-
	cp$obj:readline.obj,-
	cp$obj:regexp.obj,-
	cp$obj:scsi.obj,-
	cp$obj:semaphore.obj,-
	cp$obj:show_hwrpb.obj,-
	cp$obj:show_map.obj,-
	cp$obj:setshow.obj,-
	cp$obj:shellsort.obj,-
	cp$obj:startstop.obj,-
	cp$obj:strfunc.obj,-
	cp$obj:timer.obj,-
	cp$obj:uptime.obj,-
	cp$obj:xcmd.obj,-
	cp$obj:xlate.obj,-
	cp$obj:lx164.obj,-
	cp$obj:lx164_init.obj,-
	cp$obj:lx164_mcheck.obj,-
	cp$obj:lx164_io.obj,-
	cp$obj:memconfig_lx164.obj,-
	cp$obj:powerup_lx164.obj,-
	cp$obj:show_config_lx164.obj,-
	cp$obj:ebflash_support.obj,-
	cp$obj:tga_driver_port.obj,-
	cp$obj:tga_driver_stip.obj,-
	cp$obj:tga_driver_bt485.obj,-
	cp$obj:tga_driver_bt463.obj,-
	cp$obj:tga_driver_visual.obj,-
	cp$obj:tga_driver_support.obj,-
	cp$obj:tga_driver_busio.obj,-
	cp$obj:nl_driver.obj,-
	cp$obj:rd_driver.obj,-
	cp$obj:toy_driver.obj,-
	cp$obj:pci_driver.obj,-
	cp$obj:from_driver.obj,-
	cp$obj:nvram_driver.obj,-
	cp$obj:tt_driver.obj,-
	cp$obj:tga_driver.obj,-
	cp$obj:vga_bios_driver.obj,-
	cp$obj:kbd_driver.obj,-
	cp$obj:combo_driver.obj,-
	cp$obj:ev_driver.obj,-
	cp$obj:el_driver.obj,-
	cp$obj:pmem_driver.obj,-
	cp$obj:vmem_driver.obj,-
	cp$obj:examine_driver.obj,-
	cp$obj:ev5_ipr_driver.obj,-
	cp$obj:gpr_driver.obj,-
	cp$obj:pt_driver.obj,-
	cp$obj:ps_driver.obj,-
	cp$obj:ndbr_driver.obj,-
	cp$obj:mop_driver.obj,-
	cp$obj:fpr_driver.obj,-
	cp$obj:pi_driver.obj,-
	cp$obj:decode_driver.obj,-
	cp$obj:tee_driver.obj,-
	cp$obj:para_driver.obj,-
	cp$obj:inet_driver.obj,-
	cp$obj:fat_driver.obj,-
	cp$obj:dq_driver.obj,-
	cp$obj:de205_driver.obj,-
	cp$obj:dv_driver.obj,-
	cp$obj:n810_driver.obj,-
	cp$obj:pci_nvram_driver.obj,-
	cp$obj:pdq_driver.obj,-
	cp$obj:dw_driver.obj,-
	cp$obj:dac960_driver.obj,-
	cp$obj:pks_driver.obj,-
	cp$obj:isp1020_driver.obj,-
	cp$obj:ew_driver.obj,-
	cp$obj:ei_driver.obj,-
	cp$obj:aic78xx_driver.obj,-
	cp$obj:aic78xx_him_hwm.obj,-
	cp$obj:aic78xx_him_rsm.obj,-
	cp$obj:aic78xx_him_xlm.obj
.ENDIF

.IFDEF opt
!
! Group "base" (52 files)
!
cp$obj:grp_base.obj : -
	cp$src:alphamm.c, $(alphamm_depend),-
	cp$src:ansi.c, $(ansi_depend),-
	cp$src:aputchar.c, $(aputchar_depend),-
	cp$src:atoi.c, $(atoi_depend),-
	cp$src:blex.c, $(blex_depend),-
	cp$src:bshell.c, $(bshell_depend),-
	cp$src:bshell_cmd.c, $(bshell_cmd_depend),-
	cp$src:boot.c, $(boot_depend),-
	cp$src:builtins.c, $(builtins_depend),-
	cp$src:call_backs.c, $(call_backs_depend),-
	cp$src:date.c, $(date_depend),-
	cp$src:dynamic.c, $(dynamic_depend),-
	cp$src:entry.c, $(entry_depend),-
	cp$src:ev_action.c, $(ev_action_depend),-
	cp$src:exdep.c, $(exdep_depend),-
	cp$src:externdata.c, $(externdata_depend),-
	cp$src:filesys.c, $(filesys_depend),-
	cp$src:hwrpb.c, $(hwrpb_depend),-
	cp$src:ie.c, $(ie_depend),-
	cp$src:iobq_cmd.c, $(iobq_cmd_depend),-
	cp$src:isacfg.c, $(isacfg_depend),-
	cp$src:isp1020_edit.c, $(isp1020_edit_depend),-
	cp$src:kernel.c, $(kernel_depend),-
	cp$src:kernel_support.c, $(kernel_support_depend),-
	cp$src:kernel_alpha.c, $(kernel_alpha_depend),-
	cp$src:memset.c, $(memset_depend),-
	cp$src:printf.c, $(printf_depend),-
	cp$src:probe_io.c, $(probe_io_depend),-
	cp$src:process.c, $(process_depend),-
	cp$src:qscan.c, $(qscan_depend),-
	cp$src:queue.c, $(queue_depend),-
	cp$src:random.c, $(random_depend),-
	cp$src:readline.c, $(readline_depend),-
	cp$src:regexp.c, $(regexp_depend),-
	cp$src:semaphore.c, $(semaphore_depend),-
	cp$src:show_hwrpb.c, $(show_hwrpb_depend),-
	cp$src:show_map.c, $(show_map_depend),-
	cp$src:setshow.c, $(setshow_depend),-
	cp$src:shellsort.c, $(shellsort_depend),-
	cp$src:startstop.c, $(startstop_depend),-
	cp$src:strfunc.c, $(strfunc_depend),-
	cp$src:timer.c, $(timer_depend),-
	cp$src:uptime.c, $(uptime_depend),-
	cp$src:xcmd.c, $(xcmd_depend),-
	cp$src:xlate.c, $(xlate_depend),-
	cp$src:lx164.c, $(lx164_depend),-
	cp$src:lx164_init.c, $(lx164_init_depend),-
	cp$src:lx164_mcheck.c, $(lx164_mcheck_depend),-
	cp$src:lx164_io.c, $(lx164_io_depend),-
	cp$src:memconfig_lx164.c, $(memconfig_lx164_depend),-
	cp$src:powerup_lx164.c, $(powerup_lx164_depend),-
	cp$src:show_config_lx164.c, $(show_config_lx164_depend)
 $(cc) $(cdefs) $(cquals) $(copt) $(list) -
	/plus/obj=cp$obj:grp_base.obj-
	/lis=cp$lis:grp_base.lis-
	cp$src:alphamm.c+ansi+aputchar+atoi+blex+bshell+bshell_cmd+boot+builtins+call_backs+date+dynamic+entry+ev_action+exdep+externdata+filesys+hwrpb+ie+iobq_cmd+isacfg+isp1020_edit+kernel+kernel_support+kernel_alpha+memset+printf+probe_io+process-
	+qscan+queue+random+readline+regexp+semaphore+show_hwrpb+show_map+setshow+shellsort+startstop+strfunc+timer+uptime+xcmd+xlate+lx164+lx164_init+lx164_mcheck+lx164_io+memconfig_lx164+powerup_lx164+show_config_lx164
	! Group "base" command length was ~572

!
! Group "driver" (52 files)
!
cp$obj:grp_driver.obj : -
	cp$src:hwrpbtt_driver.c, $(hwrpbtt_driver_depend),-
	cp$src:mopdl_driver.c, $(mopdl_driver_depend),-
	cp$src:moplp_driver.c, $(moplp_driver_depend),-
	cp$src:ni_datalink.c, $(ni_datalink_depend),-
	cp$src:pke_driver.c, $(pke_driver_depend),-
	cp$src:scsi.c, $(scsi_depend),-
	cp$src:ebflash_support.c, $(ebflash_support_depend),-
	cp$src:nl_driver.c, $(nl_driver_depend),-
	cp$src:rd_driver.c, $(rd_driver_depend),-
	cp$src:toy_driver.c, $(toy_driver_depend),-
	cp$src:pci_driver.c, $(pci_driver_depend),-
	cp$src:from_driver.c, $(from_driver_depend),-
	cp$src:nvram_driver.c, $(nvram_driver_depend),-
	cp$src:tt_driver.c, $(tt_driver_depend),-
	cp$src:tga_driver.c, $(tga_driver_depend),-
	cp$src:vga_bios_driver.c, $(vga_bios_driver_depend),-
	cp$src:kbd_driver.c, $(kbd_driver_depend),-
	cp$src:combo_driver.c, $(combo_driver_depend),-
	cp$src:ev_driver.c, $(ev_driver_depend),-
	cp$src:el_driver.c, $(el_driver_depend),-
	cp$src:pmem_driver.c, $(pmem_driver_depend),-
	cp$src:vmem_driver.c, $(vmem_driver_depend),-
	cp$src:examine_driver.c, $(examine_driver_depend),-
	cp$src:ev5_ipr_driver.c, $(ev5_ipr_driver_depend),-
	cp$src:gpr_driver.c, $(gpr_driver_depend),-
	cp$src:pt_driver.c, $(pt_driver_depend),-
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
	cp$src:dq_driver.c, $(dq_driver_depend),-
	cp$src:de205_driver.c, $(de205_driver_depend),-
	cp$src:dv_driver.c, $(dv_driver_depend),-
	cp$src:n810_driver.c, $(n810_driver_depend),-
	cp$src:pci_nvram_driver.c, $(pci_nvram_driver_depend),-
	cp$src:pdq_driver.c, $(pdq_driver_depend),-
	cp$src:dw_driver.c, $(dw_driver_depend),-
	cp$src:dac960_driver.c, $(dac960_driver_depend),-
	cp$src:pks_driver.c, $(pks_driver_depend),-
	cp$src:isp1020_driver.c, $(isp1020_driver_depend),-
	cp$src:ew_driver.c, $(ew_driver_depend),-
	cp$src:ei_driver.c, $(ei_driver_depend),-
	cp$src:aic78xx_driver.c, $(aic78xx_driver_depend),-
	cp$src:aic78xx_him_hwm.c, $(aic78xx_him_hwm_depend),-
	cp$src:aic78xx_him_rsm.c, $(aic78xx_him_rsm_depend),-
	cp$src:aic78xx_him_xlm.c, $(aic78xx_him_xlm_depend)
 $(cc) $(cdefs) $(cquals) $(copt) $(list) -
	/plus/obj=cp$obj:grp_driver.obj-
	/lis=cp$lis:grp_driver.lis-
	cp$src:hwrpbtt_driver.c+mopdl_driver+moplp_driver+ni_datalink+pke_driver+scsi+ebflash_support+nl_driver+rd_driver+toy_driver+pci_driver+from_driver+nvram_driver+tt_driver+tga_driver+vga_bios_driver+kbd_driver+combo_driver+ev_driver+el_driver-
	+pmem_driver+vmem_driver+examine_driver+ev5_ipr_driver+gpr_driver+pt_driver+ps_driver+ndbr_driver+mop_driver+fpr_driver+pi_driver+decode_driver+tee_driver+para_driver+inet_driver+fat_driver+dq_driver+de205_driver+dv_driver+n810_driver-
	+pci_nvram_driver+pdq_driver+dw_driver+dac960_driver+pks_driver+isp1020_driver+ew_driver+ei_driver+aic78xx_driver+aic78xx_him_hwm+aic78xx_him_rsm+aic78xx_him_xlm
	! Group "driver" command length was ~762

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

!
! Group "0" (7 files)
!
cp$obj:grp_0.obj : -
	cp$src:tga_driver_port.c, $(tga_driver_port_depend),-
	cp$src:tga_driver_stip.c, $(tga_driver_stip_depend),-
	cp$src:tga_driver_bt485.c, $(tga_driver_bt485_depend),-
	cp$src:tga_driver_bt463.c, $(tga_driver_bt463_depend),-
	cp$src:tga_driver_visual.c, $(tga_driver_visual_depend),-
	cp$src:tga_driver_support.c, $(tga_driver_support_depend),-
	cp$src:tga_driver_busio.c, $(tga_driver_busio_depend)
 $(cc) $(cdefs) $(cquals) $(copt) $(list) -
	/plus/obj=cp$obj:grp_0.obj-
	/lis=cp$lis:grp_0.lis-
	cp$src:tga_driver_port.c+tga_driver_stip+tga_driver_bt485+tga_driver_bt463+tga_driver_visual+tga_driver_support+tga_driver_busio
	! Group "0" command length was ~238

.ENDIF

i_vlist_objects = -
	cp$obj:callbacks_alpha.obj,-
	cp$obj:pke_script.obj,-
	cp$obj:lx164_util.obj,-
	cp$obj:allocfree.obj,-
	cp$obj:bpt.obj,-
	cp$obj:cat.obj,-
	cp$obj:check.obj,-
	cp$obj:chmod.obj,-
	cp$obj:cmp.obj,-
	cp$obj:decode_alpha.obj,-
	cp$obj:diag_support.obj,-
	cp$obj:diag_evs.obj,-
	cp$obj:echo.obj,-
	cp$obj:edit.obj,-
	cp$obj:eval.obj,-
	cp$obj:exer.obj,-
	cp$obj:find_field.obj,-
	cp$obj:grep.obj,-
	cp$obj:hd.obj,-
	cp$obj:help.obj,-
	cp$obj:inst_alpha.obj,-
	cp$obj:line.obj,-
	cp$obj:login_cmd.obj,-
	cp$obj:ls.obj,-
	cp$obj:moprc_driver.obj,-
	cp$obj:moprctt_driver.obj,-
	cp$obj:more.obj,-
	cp$obj:net.obj,-
	cp$obj:ps.obj,-
	cp$obj:rm.obj,-
	cp$obj:secure.obj,-
	cp$obj:sem_cmd.obj,-
	cp$obj:simport.obj,-
	cp$obj:sleep.obj,-
	cp$obj:sort.obj,-
	cp$obj:sound.obj,-
	cp$obj:symbols.obj,-
	cp$obj:tms380fw.obj,-
	cp$obj:tr.obj,-
	cp$obj:truefalse.obj,-
	cp$obj:validate.obj,-
	cp$obj:wc.obj,-
	cp$obj:memtest.obj,-
	cp$obj:memtest_gray_lw.obj,-
	cp$obj:memtest_alpha.obj,-
	cp$obj:memtest_march.obj,-
	cp$obj:memtest_random.obj,-
	cp$obj:memtest_victim.obj,-
	cp$obj:nettest.obj


.IFDEF opt
vlist_objects = $(i_vlist_objects), -
	cp$obj:grp_base.obj,-
	cp$obj:grp_driver.obj,-
	cp$obj:grp_data.obj,-
	cp$obj:grp_0.obj
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
	cp$obj:call_backs.obj,-
	cp$obj:date.obj,-
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
	cp$obj:kernel_alpha.obj,-
	cp$obj:memset.obj,-
	cp$obj:mopdl_driver.obj,-
	cp$obj:moplp_driver.obj,-
	cp$obj:msgtxt.obj,-
	cp$obj:ni_datalink.obj,-
	cp$obj:pke_driver.obj,-
	cp$obj:printf.obj,-
	cp$obj:probe_io.obj,-
	cp$obj:process.obj,-
	cp$obj:qscan.obj,-
	cp$obj:queue.obj,-
	cp$obj:random.obj,-
	cp$obj:readline.obj,-
	cp$obj:regexp.obj,-
	cp$obj:scsi.obj,-
	cp$obj:semaphore.obj,-
	cp$obj:show_hwrpb.obj,-
	cp$obj:show_map.obj,-
	cp$obj:setshow.obj,-
	cp$obj:shellsort.obj,-
	cp$obj:startstop.obj,-
	cp$obj:strfunc.obj,-
	cp$obj:timer.obj,-
	cp$obj:uptime.obj,-
	cp$obj:xcmd.obj,-
	cp$obj:xlate.obj,-
	cp$obj:lx164.obj,-
	cp$obj:lx164_init.obj,-
	cp$obj:lx164_mcheck.obj,-
	cp$obj:lx164_io.obj,-
	cp$obj:memconfig_lx164.obj,-
	cp$obj:powerup_lx164.obj,-
	cp$obj:show_config_lx164.obj,-
	cp$obj:ebflash_support.obj,-
	cp$obj:tga_driver_port.obj,-
	cp$obj:tga_driver_stip.obj,-
	cp$obj:tga_driver_bt485.obj,-
	cp$obj:tga_driver_bt463.obj,-
	cp$obj:tga_driver_visual.obj,-
	cp$obj:tga_driver_support.obj,-
	cp$obj:tga_driver_busio.obj,-
	cp$obj:nl_driver.obj,-
	cp$obj:rd_driver.obj,-
	cp$obj:toy_driver.obj,-
	cp$obj:pci_driver.obj,-
	cp$obj:from_driver.obj,-
	cp$obj:nvram_driver.obj,-
	cp$obj:tt_driver.obj,-
	cp$obj:tga_driver.obj,-
	cp$obj:vga_bios_driver.obj,-
	cp$obj:kbd_driver.obj,-
	cp$obj:combo_driver.obj,-
	cp$obj:ev_driver.obj,-
	cp$obj:el_driver.obj,-
	cp$obj:pmem_driver.obj,-
	cp$obj:vmem_driver.obj,-
	cp$obj:examine_driver.obj,-
	cp$obj:ev5_ipr_driver.obj,-
	cp$obj:gpr_driver.obj,-
	cp$obj:pt_driver.obj,-
	cp$obj:ps_driver.obj,-
	cp$obj:ndbr_driver.obj,-
	cp$obj:mop_driver.obj,-
	cp$obj:fpr_driver.obj,-
	cp$obj:pi_driver.obj,-
	cp$obj:decode_driver.obj,-
	cp$obj:tee_driver.obj,-
	cp$obj:para_driver.obj,-
	cp$obj:inet_driver.obj,-
	cp$obj:fat_driver.obj,-
	cp$obj:dq_driver.obj,-
	cp$obj:de205_driver.obj,-
	cp$obj:dv_driver.obj,-
	cp$obj:n810_driver.obj,-
	cp$obj:pci_nvram_driver.obj,-
	cp$obj:pdq_driver.obj,-
	cp$obj:dw_driver.obj,-
	cp$obj:dac960_driver.obj,-
	cp$obj:pks_driver.obj,-
	cp$obj:isp1020_driver.obj,-
	cp$obj:ew_driver.obj,-
	cp$obj:ei_driver.obj,-
	cp$obj:aic78xx_driver.obj,-
	cp$obj:aic78xx_him_hwm.obj,-
	cp$obj:aic78xx_him_rsm.obj,-
	cp$obj:aic78xx_him_xlm.obj
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
	cp$lis:call_backs.slis,-
	cp$lis:callbacks_alpha.slis,-
	cp$lis:date.slis,-
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
	cp$lis:pke_driver.slis,-
	cp$lis:pke_script.slis,-
	cp$lis:printf.slis,-
	cp$lis:probe_io.slis,-
	cp$lis:process.slis,-
	cp$lis:qscan.slis,-
	cp$lis:queue.slis,-
	cp$lis:random.slis,-
	cp$lis:readline.slis,-
	cp$lis:regexp.slis,-
	cp$lis:scsi.slis,-
	cp$lis:semaphore.slis,-
	cp$lis:show_hwrpb.slis,-
	cp$lis:show_map.slis,-
	cp$lis:setshow.slis,-
	cp$lis:shellsort.slis,-
	cp$lis:startstop.slis,-
	cp$lis:strfunc.slis,-
	cp$lis:timer.slis,-
	cp$lis:uptime.slis,-
	cp$lis:xcmd.slis,-
	cp$lis:xlate.slis,-
	cp$lis:lx164.slis,-
	cp$lis:lx164_init.slis,-
	cp$lis:lx164_mcheck.slis,-
	cp$lis:lx164_io.slis,-
	cp$lis:lx164_util.slis,-
	cp$lis:memconfig_lx164.slis,-
	cp$lis:powerup_lx164.slis,-
	cp$lis:show_config_lx164.slis,-
	cp$lis:ebflash_support.slis,-
	cp$lis:allocfree.slis,-
	cp$lis:bpt.slis,-
	cp$lis:cat.slis,-
	cp$lis:check.slis,-
	cp$lis:chmod.slis,-
	cp$lis:cmp.slis,-
	cp$lis:decode_alpha.slis,-
	cp$lis:diag_support.slis,-
	cp$lis:diag_evs.slis,-
	cp$lis:echo.slis,-
	cp$lis:edit.slis,-
	cp$lis:eval.slis,-
	cp$lis:exer.slis,-
	cp$lis:find_field.slis,-
	cp$lis:grep.slis,-
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
	cp$lis:ps.slis,-
	cp$lis:rm.slis,-
	cp$lis:secure.slis,-
	cp$lis:sem_cmd.slis,-
	cp$lis:simport.slis,-
	cp$lis:sleep.slis,-
	cp$lis:sort.slis,-
	cp$lis:sound.slis,-
	cp$lis:symbols.slis,-
	cp$lis:tms380fw.slis,-
	cp$lis:tr.slis,-
	cp$lis:truefalse.slis,-
	cp$lis:validate.slis,-
	cp$lis:wc.slis,-
	cp$lis:tga_driver_port.slis,-
	cp$lis:tga_driver_stip.slis,-
	cp$lis:tga_driver_bt485.slis,-
	cp$lis:tga_driver_bt463.slis,-
	cp$lis:tga_driver_visual.slis,-
	cp$lis:tga_driver_support.slis,-
	cp$lis:tga_driver_busio.slis,-
	cp$lis:memtest.slis,-
	cp$lis:memtest_gray_lw.slis,-
	cp$lis:memtest_alpha.slis,-
	cp$lis:memtest_march.slis,-
	cp$lis:memtest_random.slis,-
	cp$lis:memtest_victim.slis,-
	cp$lis:nettest.slis,-
	cp$lis:nl_driver.slis,-
	cp$lis:rd_driver.slis,-
	cp$lis:toy_driver.slis,-
	cp$lis:pci_driver.slis,-
	cp$lis:from_driver.slis,-
	cp$lis:nvram_driver.slis,-
	cp$lis:tt_driver.slis,-
	cp$lis:tga_driver.slis,-
	cp$lis:vga_bios_driver.slis,-
	cp$lis:kbd_driver.slis,-
	cp$lis:combo_driver.slis,-
	cp$lis:ev_driver.slis,-
	cp$lis:el_driver.slis,-
	cp$lis:pmem_driver.slis,-
	cp$lis:vmem_driver.slis,-
	cp$lis:examine_driver.slis,-
	cp$lis:ev5_ipr_driver.slis,-
	cp$lis:gpr_driver.slis,-
	cp$lis:pt_driver.slis,-
	cp$lis:ps_driver.slis,-
	cp$lis:ndbr_driver.slis,-
	cp$lis:mop_driver.slis,-
	cp$lis:fpr_driver.slis,-
	cp$lis:pi_driver.slis,-
	cp$lis:decode_driver.slis,-
	cp$lis:tee_driver.slis,-
	cp$lis:para_driver.slis,-
	cp$lis:inet_driver.slis,-
	cp$lis:fat_driver.slis,-
	cp$lis:dq_driver.slis,-
	cp$lis:de205_driver.slis,-
	cp$lis:dv_driver.slis,-
	cp$lis:n810_driver.slis,-
	cp$lis:pci_nvram_driver.slis,-
	cp$lis:pdq_driver.slis,-
	cp$lis:dw_driver.slis,-
	cp$lis:dac960_driver.slis,-
	cp$lis:pks_driver.slis,-
	cp$lis:isp1020_driver.slis,-
	cp$lis:ew_driver.slis,-
	cp$lis:ei_driver.slis,-
	cp$lis:aic78xx_driver.slis,-
	cp$lis:aic78xx_him_hwm.slis,-
	cp$lis:aic78xx_him_rsm.slis,-
	cp$lis:aic78xx_him_xlm.slis

! Encapsulation dependencies

cp$obj:encap_fwupdate.obj : cp$src:eb_fwupdate
	mcr cp$kits:[encapsulate.new]encapsulate cp$src:eb_fwupdate cp$src:encap_fwupdate.mar encap_fwupdate -script
	$(macro64)/nolis/obj=cp$obj cp$src:encap_fwupdate.mar

cp$obj:encap_add_de205.obj : cp$src:isacfg_de205
	mcr cp$kits:[encapsulate.new]encapsulate cp$src:isacfg_de205 cp$src:encap_add_de205.mar encap_add_de205 -script
	$(macro64)/nolis/obj=cp$obj cp$src:encap_add_de205.mar

cp$obj:encap_test.obj : cp$src:test_eb164
	mcr cp$kits:[encapsulate.new]encapsulate cp$src:test_eb164 cp$src:encap_test.mar encap_test -script
	$(macro64)/nolis/obj=cp$obj cp$src:encap_test.mar

cp$obj:encap_memexer.obj : cp$src:memexer_eb164
	mcr cp$kits:[encapsulate.new]encapsulate cp$src:memexer_eb164 cp$src:encap_memexer.mar encap_memexer -script
	$(macro64)/nolis/obj=cp$obj cp$src:encap_memexer.mar

cp$obj:encap_sys_exer.obj : cp$src:sys_exer_eb164
	mcr cp$kits:[encapsulate.new]encapsulate cp$src:sys_exer_eb164 cp$src:encap_sys_exer.mar encap_sys_exer -script
	$(macro64)/nolis/obj=cp$obj cp$src:encap_sys_exer.mar

cp$obj:encap_floppy.obj : cp$src:floppy_eb164
	mcr cp$kits:[encapsulate.new]encapsulate cp$src:floppy_eb164 cp$src:encap_floppy.mar encap_floppy -script
	$(macro64)/nolis/obj=cp$obj cp$src:encap_floppy.mar

cp$obj:encap_floppy_write.obj : cp$src:floppy_write
	mcr cp$kits:[encapsulate.new]encapsulate cp$src:floppy_write cp$src:encap_floppy_write.mar encap_floppy_write -script
	$(macro64)/nolis/obj=cp$obj cp$src:encap_floppy_write.mar

cp$obj:encap_network.obj : cp$src:network_eb164
	mcr cp$kits:[encapsulate.new]encapsulate cp$src:network_eb164 cp$src:encap_network.mar encap_network -script
	$(macro64)/nolis/obj=cp$obj cp$src:encap_network.mar

cp$obj:encap_exer_read.obj : cp$src:exer_read
	mcr cp$kits:[encapsulate.new]encapsulate cp$src:exer_read cp$src:encap_exer_read.mar encap_exer_read -script
	$(macro64)/nolis/obj=cp$obj cp$src:encap_exer_read.mar

cp$obj:encap_exer_write.obj : cp$src:exer_write
	mcr cp$kits:[encapsulate.new]encapsulate cp$src:exer_write cp$src:encap_exer_write.mar encap_exer_write -script
	$(macro64)/nolis/obj=cp$obj cp$src:encap_exer_write.mar

cp$obj:encap_tgax.obj : cp$src:tgax
	mcr cp$kits:[encapsulate.new]encapsulate cp$src:tgax cp$src:encap_tgax.mar encap_tgax -script
	$(macro64)/nolis/obj=cp$obj cp$src:encap_tgax.mar

cp$obj:encap_show_status.obj : cp$src:show_status
	mcr cp$kits:[encapsulate.new]encapsulate cp$src:show_status cp$src:encap_show_status.mar encap_show_status -script
	$(macro64)/nolis/obj=cp$obj cp$src:encap_show_status.mar

cp$obj:encap_kill_diags.obj : cp$src:kill_diags_eb164
	mcr cp$kits:[encapsulate.new]encapsulate cp$src:kill_diags_eb164 cp$src:encap_kill_diags.mar encap_kill_diags -script
	$(macro64)/nolis/obj=cp$obj cp$src:encap_kill_diags.mar

cp$obj:encap_showit.obj : cp$src:showit
	mcr cp$kits:[encapsulate.new]encapsulate cp$src:showit cp$src:encap_showit.mar encap_showit -script
	$(macro64)/nolis/obj=cp$obj cp$src:encap_showit.mar

cp$obj:encap_dqtest.obj : cp$src:dqtest
	mcr cp$kits:[encapsulate.new]encapsulate cp$src:dqtest cp$src:encap_dqtest.mar encap_dqtest -script
	$(macro64)/nolis/obj=cp$obj cp$src:encap_dqtest.mar


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
cp$obj:call_backs.obj : cp$src:call_backs.c, $(call_backs_depend)
cp$obj:date.obj : cp$src:date.c, $(date_depend)
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
cp$obj:pke_driver.obj : cp$src:pke_driver.c, $(pke_driver_depend)
cp$obj:printf.obj : cp$src:printf.c, $(printf_depend)
cp$obj:probe_io.obj : cp$src:probe_io.c, $(probe_io_depend)
cp$obj:process.obj : cp$src:process.c, $(process_depend)
cp$obj:qscan.obj : cp$src:qscan.c, $(qscan_depend)
cp$obj:queue.obj : cp$src:queue.c, $(queue_depend)
cp$obj:random.obj : cp$src:random.c, $(random_depend)
cp$obj:readline.obj : cp$src:readline.c, $(readline_depend)
cp$obj:regexp.obj : cp$src:regexp.c, $(regexp_depend)
cp$obj:scsi.obj : cp$src:scsi.c, $(scsi_depend)
cp$obj:semaphore.obj : cp$src:semaphore.c, $(semaphore_depend)
cp$obj:show_hwrpb.obj : cp$src:show_hwrpb.c, $(show_hwrpb_depend)
cp$obj:show_map.obj : cp$src:show_map.c, $(show_map_depend)
cp$obj:setshow.obj : cp$src:setshow.c, $(setshow_depend)
cp$obj:shellsort.obj : cp$src:shellsort.c, $(shellsort_depend)
cp$obj:startstop.obj : cp$src:startstop.c, $(startstop_depend)
cp$obj:strfunc.obj : cp$src:strfunc.c, $(strfunc_depend)
cp$obj:timer.obj : cp$src:timer.c, $(timer_depend)
cp$obj:uptime.obj : cp$src:uptime.c, $(uptime_depend)
cp$obj:xcmd.obj : cp$src:xcmd.c, $(xcmd_depend)
cp$obj:xlate.obj : cp$src:xlate.c, $(xlate_depend)
cp$obj:lx164.obj : cp$src:lx164.c, $(lx164_depend)
cp$obj:lx164_init.obj : cp$src:lx164_init.c, $(lx164_init_depend)
cp$obj:lx164_mcheck.obj : cp$src:lx164_mcheck.c, $(lx164_mcheck_depend)
cp$obj:lx164_io.obj : cp$src:lx164_io.c, $(lx164_io_depend)
cp$obj:memconfig_lx164.obj : cp$src:memconfig_lx164.c, $(memconfig_lx164_depend)
cp$obj:powerup_lx164.obj : cp$src:powerup_lx164.c, $(powerup_lx164_depend)
cp$obj:show_config_lx164.obj : cp$src:show_config_lx164.c, $(show_config_lx164_depend)
cp$obj:ebflash_support.obj : cp$src:ebflash_support.c, $(ebflash_support_depend)
cp$obj:allocfree.obj : cp$src:allocfree.c, $(allocfree_depend)
cp$obj:bpt.obj : cp$src:bpt.c, $(bpt_depend)
cp$obj:cat.obj : cp$src:cat.c, $(cat_depend)
cp$obj:check.obj : cp$src:check.c, $(check_depend)
cp$obj:chmod.obj : cp$src:chmod.c, $(chmod_depend)
cp$obj:cmp.obj : cp$src:cmp.c, $(cmp_depend)
cp$obj:decode_alpha.obj : cp$src:decode_alpha.c, $(decode_alpha_depend)
cp$obj:diag_support.obj : cp$src:diag_support.c, $(diag_support_depend)
cp$obj:diag_evs.obj : cp$src:diag_evs.c, $(diag_evs_depend)
cp$obj:echo.obj : cp$src:echo.c, $(echo_depend)
cp$obj:edit.obj : cp$src:edit.c, $(edit_depend)
cp$obj:eval.obj : cp$src:eval.c, $(eval_depend)
cp$obj:exer.obj : cp$src:exer.c, $(exer_depend)
cp$obj:find_field.obj : cp$src:find_field.c, $(find_field_depend)
cp$obj:grep.obj : cp$src:grep.c, $(grep_depend)
cp$obj:hd.obj : cp$src:hd.c, $(hd_depend)
cp$obj:help.obj : cp$src:help.c, $(help_depend)
cp$obj:line.obj : cp$src:line.c, $(line_depend)
cp$obj:login_cmd.obj : cp$src:login_cmd.c, $(login_cmd_depend)
cp$obj:ls.obj : cp$src:ls.c, $(ls_depend)
cp$obj:moprc_driver.obj : cp$src:moprc_driver.c, $(moprc_driver_depend)
cp$obj:moprctt_driver.obj : cp$src:moprctt_driver.c, $(moprctt_driver_depend)
cp$obj:more.obj : cp$src:more.c, $(more_depend)
cp$obj:net.obj : cp$src:net.c, $(net_depend)
cp$obj:ps.obj : cp$src:ps.c, $(ps_depend)
cp$obj:rm.obj : cp$src:rm.c, $(rm_depend)
cp$obj:secure.obj : cp$src:secure.c, $(secure_depend)
cp$obj:sem_cmd.obj : cp$src:sem_cmd.c, $(sem_cmd_depend)
cp$obj:simport.obj : cp$src:simport.c, $(simport_depend)
cp$obj:sleep.obj : cp$src:sleep.c, $(sleep_depend)
cp$obj:sort.obj : cp$src:sort.c, $(sort_depend)
cp$obj:sound.obj : cp$src:sound.c, $(sound_depend)
cp$obj:symbols.obj : cp$src:symbols.c, $(symbols_depend)
cp$obj:tms380fw.obj : cp$src:tms380fw.c, $(tms380fw_depend)
cp$obj:tr.obj : cp$src:tr.c, $(tr_depend)
cp$obj:truefalse.obj : cp$src:truefalse.c, $(truefalse_depend)
cp$obj:validate.obj : cp$src:validate.c, $(validate_depend)
cp$obj:wc.obj : cp$src:wc.c, $(wc_depend)
cp$obj:tga_driver_port.obj : cp$src:tga_driver_port.c, $(tga_driver_port_depend)
cp$obj:tga_driver_stip.obj : cp$src:tga_driver_stip.c, $(tga_driver_stip_depend)
cp$obj:tga_driver_bt485.obj : cp$src:tga_driver_bt485.c, $(tga_driver_bt485_depend)
cp$obj:tga_driver_bt463.obj : cp$src:tga_driver_bt463.c, $(tga_driver_bt463_depend)
cp$obj:tga_driver_visual.obj : cp$src:tga_driver_visual.c, $(tga_driver_visual_depend)
cp$obj:tga_driver_support.obj : cp$src:tga_driver_support.c, $(tga_driver_support_depend)
cp$obj:tga_driver_busio.obj : cp$src:tga_driver_busio.c, $(tga_driver_busio_depend)
cp$obj:memtest.obj : cp$src:memtest.c, $(memtest_depend)
cp$obj:memtest_gray_lw.obj : cp$src:memtest_gray_lw.c, $(memtest_gray_lw_depend)
cp$obj:memtest_march.obj : cp$src:memtest_march.c, $(memtest_march_depend)
cp$obj:memtest_random.obj : cp$src:memtest_random.c, $(memtest_random_depend)
cp$obj:memtest_victim.obj : cp$src:memtest_victim.c, $(memtest_victim_depend)
cp$obj:nettest.obj : cp$src:nettest.c, $(nettest_depend)
cp$obj:nl_driver.obj : cp$src:nl_driver.c, $(nl_driver_depend)
cp$obj:rd_driver.obj : cp$src:rd_driver.c, $(rd_driver_depend)
cp$obj:toy_driver.obj : cp$src:toy_driver.c, $(toy_driver_depend)
cp$obj:pci_driver.obj : cp$src:pci_driver.c, $(pci_driver_depend)
cp$obj:from_driver.obj : cp$src:from_driver.c, $(from_driver_depend)
cp$obj:nvram_driver.obj : cp$src:nvram_driver.c, $(nvram_driver_depend)
cp$obj:tt_driver.obj : cp$src:tt_driver.c, $(tt_driver_depend)
cp$obj:tga_driver.obj : cp$src:tga_driver.c, $(tga_driver_depend)
cp$obj:vga_bios_driver.obj : cp$src:vga_bios_driver.c, $(vga_bios_driver_depend)
cp$obj:kbd_driver.obj : cp$src:kbd_driver.c, $(kbd_driver_depend)
cp$obj:combo_driver.obj : cp$src:combo_driver.c, $(combo_driver_depend)
cp$obj:ev_driver.obj : cp$src:ev_driver.c, $(ev_driver_depend)
cp$obj:el_driver.obj : cp$src:el_driver.c, $(el_driver_depend)
cp$obj:pmem_driver.obj : cp$src:pmem_driver.c, $(pmem_driver_depend)
cp$obj:vmem_driver.obj : cp$src:vmem_driver.c, $(vmem_driver_depend)
cp$obj:examine_driver.obj : cp$src:examine_driver.c, $(examine_driver_depend)
cp$obj:ev5_ipr_driver.obj : cp$src:ev5_ipr_driver.c, $(ev5_ipr_driver_depend)
cp$obj:gpr_driver.obj : cp$src:gpr_driver.c, $(gpr_driver_depend)
cp$obj:pt_driver.obj : cp$src:pt_driver.c, $(pt_driver_depend)
cp$obj:ps_driver.obj : cp$src:ps_driver.c, $(ps_driver_depend)
cp$obj:ndbr_driver.obj : cp$src:ndbr_driver.c, $(ndbr_driver_depend)
cp$obj:mop_driver.obj : cp$src:mop_driver.c, $(mop_driver_depend)
cp$obj:fpr_driver.obj : cp$src:fpr_driver.c, $(fpr_driver_depend)
cp$obj:pi_driver.obj : cp$src:pi_driver.c, $(pi_driver_depend)
cp$obj:decode_driver.obj : cp$src:decode_driver.c, $(decode_driver_depend)
cp$obj:tee_driver.obj : cp$src:tee_driver.c, $(tee_driver_depend)
cp$obj:para_driver.obj : cp$src:para_driver.c, $(para_driver_depend)
cp$obj:inet_driver.obj : cp$src:inet_driver.c, $(inet_driver_depend)
cp$obj:fat_driver.obj : cp$src:fat_driver.c, $(fat_driver_depend)
cp$obj:dq_driver.obj : cp$src:dq_driver.c, $(dq_driver_depend)
cp$obj:de205_driver.obj : cp$src:de205_driver.c, $(de205_driver_depend)
cp$obj:dv_driver.obj : cp$src:dv_driver.c, $(dv_driver_depend)
cp$obj:n810_driver.obj : cp$src:n810_driver.c, $(n810_driver_depend)
cp$obj:pci_nvram_driver.obj : cp$src:pci_nvram_driver.c, $(pci_nvram_driver_depend)
cp$obj:pdq_driver.obj : cp$src:pdq_driver.c, $(pdq_driver_depend)
cp$obj:dw_driver.obj : cp$src:dw_driver.c, $(dw_driver_depend)
cp$obj:dac960_driver.obj : cp$src:dac960_driver.c, $(dac960_driver_depend)
cp$obj:pks_driver.obj : cp$src:pks_driver.c, $(pks_driver_depend)
cp$obj:isp1020_driver.obj : cp$src:isp1020_driver.c, $(isp1020_driver_depend)
cp$obj:ew_driver.obj : cp$src:ew_driver.c, $(ew_driver_depend)
cp$obj:ei_driver.obj : cp$src:ei_driver.c, $(ei_driver_depend)
cp$obj:aic78xx_driver.obj : cp$src:aic78xx_driver.c, $(aic78xx_driver_depend)
cp$obj:aic78xx_him_hwm.obj : cp$src:aic78xx_him_hwm.c, $(aic78xx_him_hwm_depend)
cp$obj:aic78xx_him_rsm.obj : cp$src:aic78xx_him_rsm.c, $(aic78xx_him_rsm_depend)
cp$obj:aic78xx_him_xlm.obj : cp$src:aic78xx_him_xlm.c, $(aic78xx_him_xlm_depend)
