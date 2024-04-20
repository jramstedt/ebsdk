# platform_support.mms
#
# This is the standard support library for Common Console platforms.
# It is off in a separate file to provide an "escape" for doing wierd
# things for a platform.  The PLATFORM_<platform>.MMS file can omit the
# include of this file and use its own rules for building files.  Only
# useful if there is a naming conflict, and should be discouraged.
#
# This needs more cleanup and further parameterization through macros to
# be really useful.
#
# Modified this on 11-Apr-1994 to include the steps needed to to include the
# x86 emulator.


.suffixes
.suffixes	.exe .slis .obj .lis .c .h .b32 .req .mar .sdl

stubs1	= -
	cp$obj:stub_build_config.obj,-
	cp$obj:stub_build_dsrdb.obj,-
	cp$obj:stub_lad_new_dl.obj, -
	cp$obj:stub_showmem.obj,-
	cp$obj:stub_find_bad_mem.obj,-
	cp$obj:stub_flag_cleanup.obj,-
	cp$obj:stub_flag_init.obj,-
	cp$obj:stub_get_date_time.obj,-
	cp$obj:stub_getcpuname.obj,-
	cp$obj:stub_checksum_config.obj,-
	cp$obj:stub_clear_password.obj,-
	cp$obj:stub_initialize_hardware.obj,-
	cp$obj:stub_reset_hardware.obj,-
	cp$obj:stub_sbfsafe.obj,-
	cp$obj:stub_machine_handle_nxm.obj,-
	cp$obj:stub_nvram_sev.obj,-
	cp$obj:stub_show_fru.obj,-
	cp$obj:stub_show_error.obj,-
	cp$obj:stub_show_iobq.obj,-
	cp$obj:stub_lfu_start.obj,-
	cp$obj:stub_mop_send_req_id.obj,-
	cp$obj:stub_platform_init2.obj,-
	cp$obj:stub_halt_switch_in.obj,-
	cp$obj:stub_memtest_gray.obj,-
	cp$obj:stub_memtest_march.obj,-
	cp$obj:stub_memtest_victim.obj,-
	cp$obj:stub_memtest_random.obj,-
	cp$obj:stub_tu_ev_write.obj,-
	cp$obj:stub_iic_reinit.obj,-
	cp$obj:stub_set_malloc_zone.obj,-
	cp$obj:stub_mop_handle_loopback.obj

stubs2	= -
	cp$obj:stub_show_config.obj,-
        cp$obj:stub_decode_sn_scv2.obj,-
        cp$obj:stub_decode_sn_scv3.obj,-
	cp$obj:stub_id_simm_fru.obj,-
	cp$obj:stub_smm_id_simm.obj,-
	cp$obj:stub_sable_ocp_write.obj,-
	cp$obj:stub_prcache_cbus.obj,-
	cp$obj:stub_prcache_eisa.obj,-
	cp$obj:stub_prcache_pci.obj,-
	cp$obj:stub_callbacks.obj,-
	cp$obj:stub_io_eisa_primitives.obj,-
	cp$obj:stub_io_interrupts.obj,-
	cp$obj:stub_io_eisa_interrupts.obj,-
	cp$obj:stub_probe_pci_setup.obj,-
	cp$obj:stub_show_isa.obj,-
	cp$obj:stub_show_eisa.obj,-
	cp$obj:stub_show_drom_version.obj,-
	cp$obj:stub_get_cpu_speed.obj,-
	cp$obj:stub_eisa_slot_disable.obj,-
	cp$obj:stub_probe_pci_cleanup.obj,-
	cp$obj:stub_inet_set_env.obj,-
	cp$obj:stub_hwrpbtt_driver.obj,-
	cp$obj:stub_show_hwrpb.obj,-
	cp$obj:stub_show_map.obj,-
	cp$obj:stub_sys$native_to_translated.obj,-
	cp$obj:stub_dac960_show.obj,-
	cp$obj:stub_mc_reinit.obj,-
	cp$obj:stub_vga_support.obj,-
	cp$obj:stub_vga_reinit.obj,-
	cp$obj:stub_kbd_driver.obj,-
	cp$obj:stub_validate_flash_checksum.obj

stubs3	= -
	cp$obj:stub_fru5_pci_vpd.obj,-
	cp$obj:stub_powerupprogress.obj,-
	cp$obj:stub_use_eisa_ecu_data.obj,-
	cp$obj:stub_smc37c669_init_irq_drq_tables.obj,-
	cp$obj:stub_pci_check_primary.obj,-
        cp$obj:stub_reset_cpu.obj,-
	cp$obj:stub_show_power.obj,-
	cp$obj:stub_show_cpu.obj,-
	cp$obj:stub_queue_validate.obj,-
	cp$obj:stub_report_powerup_errors.obj,-
	cp$obj:stub_migrate.obj,-
	cp$obj:stub_galaxy.obj,-
	cp$obj:stub_gct.obj,-
	cp$obj:stub_build_fru_table.obj,-
	cp$obj:stub_gct_sys_serial_write.obj,-
	cp$obj:stub_clear_platform_errors.obj,-
	cp$obj:stub_bios_sg_map_on_off.obj

cp$obj:stub_build_config.obj		: cp$src:stub_build_config.c
cp$obj:stub_build_dsrdb.obj		: cp$src:stub_build_dsrdb.c
cp$obj:stub_lad_new_dl.obj		: cp$src:stub_lad_new_dl.c
cp$obj:stub_showmem.obj			: cp$src:stub_showmem.c
cp$obj:stub_find_bad_mem.obj		: cp$src:stub_find_bad_mem.c
cp$obj:stub_flag_cleanup.obj		: cp$src:stub_flag_cleanup.c
cp$obj:stub_flag_init.obj		: cp$src:stub_flag_init.c
cp$obj:stub_getcpuname.obj		: cp$src:stub_getcpuname.c
cp$obj:stub_get_date_time.obj		: cp$src:stub_get_date_time.c
cp$obj:stub_memtest_gray.obj		: cp$src:stub_memtest_gray.c
cp$obj:stub_memtest_march.obj		: cp$src:stub_memtest_march.c
cp$obj:stub_memtest_random.obj		: cp$src:stub_memtest_random.c
cp$obj:stub_memtest_victim.obj		: cp$src:stub_memtest_victim.c
cp$obj:stub_tu_ev_write.obj		: cp$src:stub_tu_ev_write.c
cp$obj:stub_checksum_config.obj		: cp$src:stub_checksum_config.c
cp$obj:stub_clear_password.obj		: cp$src:stub_clear_password.c
cp$obj:stub_initialize_hardware.obj	: cp$src:stub_initialize_hardware.c
cp$obj:stub_reset_hardware.obj		: cp$src:stub_reset_hardware.c
cp$obj:stub_sbfsafe.obj			: cp$src:stub_sbfsafe.c
cp$obj:stub_machine_handle_nxm.obj	: cp$src:stub_machine_handle_nxm.c
cp$obj:stub_nvram_sev.obj		: cp$src:stub_nvram_sev.c
cp$obj:stub_show_fru.obj		: cp$src:stub_show_fru.c
cp$obj:stub_mc_reinit.obj		: cp$src:stub_mc_reinit.c
cp$obj:stub_show_error.obj		: cp$src:stub_show_error.c
cp$obj:stub_mop_send_req_id.obj		: cp$src:stub_mop_send_req_id.c
cp$obj:stub_platform_init2.obj		: cp$src:stub_platform_init2.c
cp$obj:stub_halt_switch_in.obj		: cp$src:stub_halt_switch_in.c
cp$obj:stub_iic_reinit.obj		: cp$src:stub_iic_reinit.c
cp$obj:stub_set_malloc_zone.obj		: cp$src:stub_set_malloc_zone.c
cp$obj:stub_mop_handle_loopback.obj	: cp$src:stub_mop_handle_loopback.c
cp$obj:stub_show_config.obj		: cp$src:stub_show_config.c
cp$obj:stub_decode_sn_scv2.obj          : cp$src:stub_decode_sn_scv2.c
cp$obj:stub_decode_sn_scv3.obj          : cp$src:stub_decode_sn_scv3.c
cp$obj:stub_smm_id_simm.obj		: cp$src:stub_smm_id_simm.c
cp$obj:stub_id_simm_fru.obj		: cp$src:stub_id_simm_fru.c
cp$obj:stub_sable_ocp_write.obj		: cp$src:stub_sable_ocp_write.c
cp$obj:stub_prcache_cbus.obj		: cp$src:stub_prcache_cbus.c
cp$obj:stub_prcache_eisa.obj		: cp$src:stub_prcache_eisa.c
cp$obj:stub_prcache_pci.obj		: cp$src:stub_prcache_pci.c
cp$obj:stub_callbacks.obj		: cp$src:stub_callbacks.c
cp$obj:stub_io_eisa_primitives.obj	: cp$src:stub_io_eisa_primitives.c
cp$obj:stub_io_interrupts.obj		: cp$src:stub_io_interrupts.c
cp$obj:stub_io_eisa_interrupts.obj	: cp$src:stub_io_eisa_interrupts.c
cp$obj:stub_probe_pci_setup.obj		: cp$src:stub_probe_pci_setup.c
cp$obj:stub_show_iobq.obj		: cp$src:stub_show_iobq.c
cp$obj:stub_lfu_start.obj		: cp$src:stub_lfu_start.c
cp$obj:stub_show_isa.obj		: cp$src:stub_show_isa.c
cp$obj:stub_show_eisa.obj		: cp$src:stub_show_eisa.c
cp$obj:stub_show_drom_version.obj	: cp$src:stub_show_drom_version.c
cp$obj:stub_get_cpu_speed.obj		: cp$src:stub_get_cpu_speed.c
cp$obj:stub_eisa_slot_disable.obj	: cp$src:stub_eisa_slot_disable.c
cp$obj:stub_probe_pci_cleanup.obj	: cp$src:stub_probe_pci_cleanup.c
cp$obj:stub_inet_set_env.obj		: cp$src:stub_inet_set_env.c
cp$obj:stub_hwrpbtt_driver.obj		: cp$src:stub_hwrpbtt_driver.c
cp$obj:stub_show_hwrpb.obj		: cp$src:stub_show_hwrpb.c
cp$obj:stub_vga_support.obj		: cp$src:stub_vga_support.c
cp$obj:stub_vga_reinit.obj		: cp$src:stub_vga_reinit.c
cp$obj:stub_kbd_driver.obj		: cp$src:stub_kbd_driver.c
cp$obj:stub_show_map.obj		: cp$src:stub_show_map.c
cp$obj:stub_sys$native_to_translated.obj -
					: cp$src:stub_sys$native_to_translated.c
cp$obj:stub_dac960_show.obj		: cp$src:stub_dac960_show.c
cp$obj:stub_validate_flash_checksum.obj : cp$src:stub_validate_flash_checksum.c
cp$obj:stub_powerupprogress.obj		: cp$src:stub_powerupprogress.c
cp$obj:stub_use_eisa_ecu_data.obj	: cp$src:stub_use_eisa_ecu_data.c
cp$obj:stub_smc37c669_init_irq_drq_tables.obj -
					: cp$src:stub_smc37c669_init_irq_drq_tables.c
cp$obj:stub_pci_check_primary.obj	: cp$src:stub_pci_check_primary.c
cp$obj:stub_reset_cpu.obj		: cp$src:stub_reset_cpu.c
cp$obj:stub_show_power.obj		: cp$src:stub_show_power.c
cp$obj:stub_show_cpu.obj		: cp$src:stub_show_cpu.c
cp$obj:stub_queue_validate.obj		: cp$src:stub_queue_validate.c
cp$obj:stub_report_powerup_errors.obj	: cp$src:stub_report_powerup_errors.c
cp$obj:stub_migrate.obj			: cp$src:stub_migrate.c
cp$obj:stub_galaxy.obj   		: cp$src:stub_galaxy.c
cp$obj:stub_gct.obj   			: cp$src:stub_gct.c
cp$obj:stub_build_fru_table.obj		: cp$src:stub_build_fru_table.c
cp$obj:stub_gct_sys_serial_write.obj	: cp$src:stub_gct_sys_serial_write.c
cp$obj:stub_fru5_pci_vpd.obj		: cp$src:stub_fru5_pci_vpd.c
cp$obj:stub_clear_platform_errors.obj	: cp$src:stub_clear_platform_errors.c
cp$obj:stub_bios_sg_map_on_off.obj	: cp$src:stub_bios_sg_map_on_off.c

cp$obj:stubs.olb : $(stubs1), $(stubs2), $(stubs3)
	$(libr)/cre cp$obj:stubs.olb
	$(libr)/ins cp$obj:stubs.olb $(stubs1)
	$(libr)/ins cp$obj:stubs.olb $(stubs2)
	$(libr)/ins cp$obj:stubs.olb $(stubs3)

depend :
	@ write sys$output "Use CP$REF:NEWBUILD.COM to build ", -
		"the GEMCC dependencies"

!
! The module description list, used by quilt.
!
$(cfg)modlist.c : $(slis)
	gawk -f $(src)build_modlist.awk $(cfg)sources.txt >$(cfg)modlist.c

!
! Make .slis depend on .obj, even though it really depends on the listing file.
! This way, .slis scans will not occur until after the files have been
! compiled.  WARNING:  You can really screw up the build if you compile an
! object file without /lis=cp$lis
!

.obj.slis
	@ mms_source = f$parse("$(mms$source)",,,"NAME")
	@ mms_source = "CP$LIS:" + mms_source + ".LIS"	
	@ write sys$output -
	  "search/output=$(mms$target) ''mms_source' ", -
	  """::"",""$code"",""routine size""
	@ search/output=$(mms$target) 'mms_source' "::","$code","routine size"

!
! The variable list as generated by scansym contains ALL the symbols, so
! further processing is required to include/exclude the symbols the user has
! requested (via commands in the build file).  We take these two steps to
! generate vlist.c so that the amount of code that has to be touched to
! accomodate a new object file is minimal.
!
cp$src:vlist.c : $(vlist_objects)
	mcr cp$kits:[scansym]scansym.exe cp$src:ref_symbols.txt $(cfg)objects.txt
	gawk -f cp$src:build_symtab.awk -f cp$cfg:sym_xxclusions.dat cp$src:ref_symbols.txt >cp$src:vlist.c
	
cp$exe:add_header.exe	: cp$src:add_header.c
 	cc/obj=cp$obj:   cp$src:add_header.c 
	link/exe=cp$exe  cp$obj:add_header,-
                         sys$library:vaxcrtl/lib  

cp$exe:convert_xilinx.exe	: cp$src:convert_xilinx.c
 	cc/obj=cp$obj:   cp$src:convert_xilinx.c 
	link/exe=cp$exe  cp$obj:convert_xilinx,-
                         sys$library:vaxcrtl/lib              

cp$obj:fpscreen_ieee.obj	: cp$src:fpscreen_ieee.c,-
				$(fpscreen_ieee_depend)
	$(cc_fp)/nomemb/assume=noalign/float=ieee_float-
	/mach/obj=cp$obj/opt=(inline=none,unroll=1)/lis=cp$lis cp$src:fpscreen_ieee.c

cp$obj:fpscreen_vaxd.obj	: cp$src:fpscreen_vaxd.c,-
				$(fpscreen_vaxd_depend)
	$(cc_fp)/nomemb/assume=noalign/float=d_float-
	/mach/obj=cp$obj/opt=(inline=none,unroll=1)/lis=cp$lis cp$src:fpscreen_vaxd.c

cp$obj:fpscreen_vaxf.obj	: cp$src:fpscreen_vaxf.c,-
				$(fpscreen_vaxf_depend)
	$(cc_fp)/nomemb/assume=noalign/float=g_float-
	/mach/obj=cp$obj/opt=(inline=none,unroll=1)/lis=cp$lis cp$src:fpscreen_vaxf.c

cp$obj:fpscreen_vaxg.obj	: cp$src:fpscreen_vaxg.c,-
				$(fpscreen_vaxg_depend)
	$(cc_fp)/nomemb/assume=noalign/float=g_float-
	/mach/obj=cp$obj/opt=(inline=none,unroll=1)/lis=cp$lis cp$src:fpscreen_vaxg.c

cp$obj:float_ieee.obj	: cp$src:float_ieee.c,-
			$(float_ieee_depend)
	$(cc_fp)/nomemb/assume=noalign/float=ieee_float-
	/mach/obj=cp$obj/opt=(inline=none,unroll=1)/lis=cp$lis cp$src:float_ieee.c

cp$obj:float_d.obj	: cp$src:float_d.c,-
			$(float_d_depend)
	$(cc_fp)/nomemb/assume=noalign/float=d_float-
	/mach/obj=cp$obj/opt=(inline=none,unroll=1)/lis=cp$lis cp$src:float_d.c

cp$obj:float_f.obj	: cp$src:float_f.c,-
			$(float_f_depend)
	$(cc_fp)/nomemb/assume=noalign/float=g_float-
	/mach/obj=cp$obj/opt=(inline=none,unroll=1)/lis=cp$lis cp$src:float_f.c

cp$obj:float_g.obj	: cp$src:float_g.c,-
			$(float_g_depend)
	$(cc_fp)nomemb/assume=noalign/float=g_float-
	/mach/obj=cp$obj/opt=(inline=none,unroll=1)/lis=cp$lis cp$src:float_g.c

cp$obj:msgtxt.obj	: cp$cfg:msgtxt.c,$(msgtxt_depend)
	$(cc) $(cdefs) $(cquals) $(copt_inl) -
		cp$cfg:msgtxt
	copy nl: cp$lis:msgtxt.lis

cp$obj:dma_driver.obj :    cp$src:dma_driver.c,-
			       $(dma_driver_depend)
	$(cc) $(cdefs) $(cquals) $(copt_inl) $(list) cp$src:dma_driver

cp$obj:csr_driver.obj :    cp$src:csr_driver.c,-
			       $(csr_driver_depend)
	$(cc) $(cdefs) $(cquals) $(copt_inl) $(list) cp$src:csr_driver

.sdl.h
	$(sdl)/lang=cc=cp$lis:__temp__ $(mms$source)
	search cp$lis:__temp__.h "#pragma","member_alignment"/match=nand/out=$(mms$target)
	delete cp$lis:__temp__.h;*

.sdl.mar
	$(sdl)/lang=mac=cp$src $(mms$source)


!
!	Macro libraries
!
cp$src:alpha_fw.mlb	: cp$src:kernel_def.mar,-
			  cp$src:hwrpb_def.mar,-
			  cp$src:vaxpsl_def.mar,-
			  cp$src:vaxscb_def.mar,-
			  cp$src:pr1701def.mar
	library/create/macro cp$src:alpha_fw -
		cp$src:kernel_def,-
		cp$src:hwrpb_def,-
		cp$src:vaxpsl_def,-
		cp$src:vaxscb_def,-
		cp$src:pr1701def        

cp$src:cfw.mlb		: cp$src:cfw_macros.mar,-
			  cp$src:paldef.mar,-
			  cp$src:aprdef.mar,-
			  cp$src:ev_def.mar,-
			  cp$src:kernel_def.mar,-
			  cp$src:hwrpb_def.mar,-
			  cp$src:impure_def.mar,-
			  cp$src:pal_def.mar,-
			  cp$src:alphascb_def.mar,-
			  cp$src:starlet.mar
        library/create/macro cp$src:cfw -
		cp$src:cfw_macros.mar,-
		cp$src:paldef.mar,-
		cp$src:aprdef.mar,-
		cp$src:ev_def.mar,-
		cp$src:kernel_def.mar,-
		cp$src:hwrpb_def.mar,-
		cp$src:impure_def.mar,-
		cp$src:pal_def.mar,-
		cp$src:alphascb_def.mar,-
		cp$src:starlet.mar

cp$src:diag_mac_ev5.mlb		: cp$src:ev5_paldef.mar,-
				  cp$src:paldef.mar,-
				  cp$src:pal_def.mar,-
			  	  cp$src:ev5_defs.mar
        library/create/macro cp$src:diag_mac_ev5 cp$src:ev5_paldef,-
			     cp$src:paldef,cp$src:pal_def,cp$src:ev5_defs,-
			     cp$src:starlet

cp$src:fbe_hw.mlb		: cp$src:fbe_hw.mar
	library/create/macro cp$src:fbe_hw cp$src:fbe_hw.mar  

cp$src:n710_script.mlb		: cp$src:n710_script_def.mar, -
				  cp$src:n710_script_macros.mar
	library/create/macro cp$src:n710_script -
		cp$src:n710_script_def+cp$src:n710_script_macros

cp$src:n810_script.mlb		: cp$src:n810_script_def.mar, -
				  cp$src:n810_script_macros.mar
	library/create/macro cp$src:n810_script -
		cp$src:n810_script_def+cp$src:n810_script_macros

cp$src:alphamac.mlb		: cp$src:starlet.mar,-
				  cp$src:paldef.mar,-
				  cp$src:alpha_defs.mar,-
				  cp$src:osfalpha_defs.mar,-
				  cp$src:impure.mar,-
				  cp$src:pal_macros.mar,-
				  cp$src:logout.mar
	library/create/macro cp$src:alphamac -
		cp$src:starlet.mar,paldef,alpha_defs,osfalpha_defs,impure,pal_macros,logout

cp$src:instops_alpha.mlb	: cp$src:instops_alpha.mar
	library/create/macro cp$src:instops_alpha -
		cp$src:instops_alpha

.c.obj
	$(cc) $(cdefs) $(cquals) $(copt) $(list) $(mms$source)

.mar.obj
	$(macro)/obj=cp$obj $(list) $(mms$source)

.b32.obj
	$(bliss)/obj=cp$obj $(list) $(mms$source)

cp$obj:n710_script.obj		: cp$src:n710_script.mar, cp$src:n710_script.mlb
cp$obj:n810_script.obj		: cp$src:n810_script.mar, cp$src:n810_script.mlb
cp$obj:memtest_vax.obj		: cp$src:memtest_vax.mar
cp$obj:pkt_script.obj		: cp$src:pkt_script.mar, cp$src:n710_script.mlb
cp$obj:pke_script.obj		: cp$src:pke_script.mar, cp$src:n810_script.mlb
cp$obj:put_script.obj		: cp$src:put_script.mar, cp$src:n710_script.mlb
cp$obj:pue_script.obj		: cp$src:pue_script.mar, cp$src:n810_script.mlb
cp$obj:queue_interlock.obj	: cp$src:queue_interlock.mar
cp$obj:shac_sshma.obj		: cp$src:shac_sshma.mar
cp$obj:syscvrtim.obj		: cp$src:syscvrtim.mar
cp$obj:st_ld_vax.obj		: cp$src:st_ld_vax.mar
cp$obj:decode.obj		: cp$src:decode.b32, cp$src:vaxops.req
cp$obj:evax_decode.obj		: cp$src:evax_decode.b32
cp$obj:vaxinst.obj		: cp$src:vaxinst.b32, cp$src:vaxops.req

cp$obj:kernel_vax.obj		: cp$src:kernel_vax.mar,-
				  $(cfg)platform.mar,-
				  cp$src:alpha_fw.mlb
	macro/lis=cp$lis:kernel_vax.lis/obj=cp$obj:kernel_vax.obj -
		$(cfg)platform.mar+cp$src:kernel_vax.mar

cp$obj:vaxscb.obj		: cp$src:vaxscb.mar,-
				  $(cfg)platform.mar,-
				  cp$src:alpha_fw.mlb
	macro/lis=cp$lis:vaxscb.lis/obj=cp$obj:vaxscb.obj -
		$(cfg)platform.mar+cp$src:vaxscb.mar

cp$obj:callbacks_vax.obj        : cp$src:callbacks_vax.mar,-
				  $(cfg)platform.mar,-
				  cp$src:alpha_fw.mlb
	macro/lis=cp$lis:callbacks_vax.lis/obj=cp$obj:callbacks_vax.obj -
		$(cfg)platform.mar+cp$src:callbacks_vax.mar

cp$obj:inst_alpha.obj	: cp$src:inst_alpha.mar,-
			  cp$src:instops_alpha.mlb
	$(macro64) -
		cp$src:inst_alpha.mar/lis=cp$lis/obj=cp$obj

cp$obj:fbe_bootstrap.obj	: cp$src:fbe_bootstrap.mar, -
                                  cp$src:fbe_hw.mlb

cp$obj:fbe_xilinx.obj		: cp$src:fbe_xilinx.mar

cp$obj:callbacks_alpha.obj	: cp$src:callbacks_alpha.mar,-
				  cp$src:cfw.mlb,-
				  $(cfg)platform.mar
	$(macro64) -
		$(cfg)platform.mar -
		+cp$src:callbacks_alpha.mar/lis=cp$lis/obj=cp$obj -
		+cp$src:cfw/lib

cp$obj:kernel_alpha.obj		: cp$src:kernel_alpha.mar,-
				  cp$src:cfw.mlb,-
				  $(cfg)platform.mar
	$(macro64) -
		$(cfg)platform.mar -
		+cp$src:kernel_alpha.mar/lis=cp$lis/obj=cp$obj -
		+cp$src:cfw/lib

cp$obj:kernel_load_turbo.obj    : cp$src:kernel_load_turbo.mar,-
                                  cp$src:cfw.mlb,-
                                  $(cfg)platform.mar
        $(macro64) -
                $(cfg)platform.mar -
                +cp$src:kernel_load_turbo.mar/lis=cp$lis/obj=cp$obj -
                +cp$src:cfw/lib
 
cp$obj:memtest_alpha.obj	: cp$src:memtest_alpha.mar,-
				  cp$src:cfw.mlb
	$(macro64) -
		cp$src:memtest_alpha.mar/lis=cp$lis/obj=cp$obj -
		+cp$src:cfw/lib

cp$obj:memtest_alpha_turbo.obj	: cp$src:memtest_alpha_turbo.mar,-
				  cp$src:cfw.mlb
	$(macro64) -
		cp$src:memtest_alpha_turbo.mar/lis=cp$lis/obj=cp$obj -
		+cp$src:cfw/lib

cp$obj:turbo_diag_macro.obj	: cp$src:turbo_diag_macro.mar,-
			  $(cfg)platform.mar,-
			  cp$src:diag_mac_ev5.mlb,-
			  cp$src:tlep_misc.mar,-
			  cp$src:diag_bcache.mar,-
			  cp$src:tlep_macro.mar
	$(macro64) -
	$(cfg)platform.mar +-
	cp$src:tlep_misc.mar +-
	cp$src:diag_bcache.mar +-
	cp$src:tlep_macro.mar +-
	cp$src:diag_mac_ev5/library +-
	cp$src:turbo_diag_macro.mar -
	/obj=cp$obj:turbo_diag_macro.obj /lis=cp$lis:turbo_diag_macro.lis

cp$obj:arc_cb.obj 		: cp$src:arc_cb.mar,-
				  $(cfg)platform.mar,-
				  cp$src:cfw.mlb
	$(macro64) -
		$(cfg)platform.mar -
		+cp$src:arc_cb.mar/lis=cp$lis/obj=cp$obj -
		+cp$src:cfw/lib

cp$obj:console_uart.obj		: cp$src:console_uart.mar,-
				  cp$src:cfw.mlb
	$(macro64) -
		cp$src:console_uart.mar/lis=cp$lis/obj=cp$obj -
		+cp$src:cfw/lib

cp$src:diag_mac.mlb		: cp$src:paldef.mar,-
				  cp$src:lep_misc.mar,-
				  cp$src:alpha_defs.mar,-
				  cp$src:cobra_regs.mar,-
			  	  cp$src:starlet.mar,-
			  	  cp$src:lepdef.mar,-
			  	  cp$src:impure.mar
        library/create/macro cp$src:diag_mac -
		cp$src:paldef.mar,cp$src:lep_misc.mar,cp$src:alpha_defs,-
		cp$src:cobra_regs.mar,cp$src:starlet,cp$src:lepdef,-
		cp$src:impure/ins $(list)

.IFDEF ALPHA
cp$obj:xdelta.obj		: cp$src:xdelta.bli
	define/user src$ alpha$library,cp$kits:[alpha-bliss.new]
	define/user lib$ alpha$library,cp$kits:[alpha-bliss.new]
	define/user sys$library alpha$library,cp$kits:[alpha-bliss.new]
	bliss/a64/env=nofp/notrace/lis=cp$lis/obj=cp$obj cp$src:xdelta
cp$obj:xdelta_isrs.obj		: cp$src:xdelta_isrs.mar
	$(macro64) -
		cp$src:xdelta_isrs.mar/lis=cp$lis/obj=cp$obj -
		+alpha$library:lib/lib
.ELSE
cp$obj:xdelta.obj		: cp$src:xdelta.mar
.ENDIF

huf_decom_objects = -
	cp$obj:huf_decom.obj,-
	cp$obj:romdecom.obj,-
	cp$obj:rominflate.obj,-

ev5_huf_decom_objects = -
	cp$obj:ev5_huf_decom.obj,-
	cp$obj:romdecom.obj,-
	cp$obj:rominflate.obj,-

ev6_huf_decom_objects = -
	cp$obj:ev6_huf_decom.obj,-
	cp$obj:romdecom.obj,-
	cp$obj:rominflate.obj,-

ev6_huf_decom_2m_objects = -
	cp$obj:ev6_huf_decom_2m.obj,-
	cp$obj:romdecom.obj,-
	cp$obj:rominflate.obj,-

romdecom_depends = -
	cp$src:decom.c,-
	cp$src:decomp.h

rominflate_depends = -
	cp$src:inflate.c,-
	cp$src:decomp.h

cp$obj:huf_decom.obj : cp$src:huf_decom.m64, cp$src:ev4_instructions.m64
	$(macro64) -
		/list=cp$lis:huf_decom/show=binary/obj=cp$obj:huf_decom -
		cp$src:platform.mar+cp$src:ev4_instructions.m64+cp$src:huf_decom+cp$src:cfw/lib

cp$obj:ev5_huf_decom.obj : cp$src:ev5_huf_decom.m64
	$(macro64) -
		/list=cp$lis:ev5_huf_decom/show=binary/obj=cp$obj:ev5_huf_decom -
		cp$src:platform.mar+cp$src:ev5_huf_decom.m64+cp$src:ev5_alphamac/lib+cp$src:cfw.mlb/lib

cp$obj:ev6_huf_decom.obj : cp$src:ev6_huf_decom.m64
	$(macro64) -
		/list=cp$lis:ev6_huf_decom/show=binary/obj=cp$obj:ev6_huf_decom -
		cp$src:ev6_vms_pal_defs.mar+cp$src:ev6_pal_macros.mar+ -
		cp$src:platform.mar+cp$src:pal_def.mar+cp$src:ev6_huf_decom.m64+cp$src:cfw.mlb/lib

cp$obj:ev6_huf_decom_2m.obj : cp$src:ev6_huf_decom.m64
	$(macro64) -
		/list=cp$lis:ev6_huf_decom_2m/show=binary/obj=cp$obj:ev6_huf_decom_2m/define=two_meg -
		cp$src:ev6_vms_pal_defs.mar+cp$src:ev6_pal_macros.mar+ -
		cp$src:platform.mar+cp$src:pal_def.mar+cp$src:ev6_huf_decom.m64+cp$src:cfw.mlb/lib

cp$obj:rominflate.obj : $(rominflate_depends)
	$(cc)/nowarn/opt=(inline=none,unroll=1)/extern=strict/mach/define=rom-
	/obj=cp$obj:rominflate/include_dir=(cp$inc,cp$src)-
	/lis=cp$lis:rominflate cp$src:inflate

cp$obj:romdecom.obj : $(romdecom_depends)
	$(cc)/nowarn/opt=(inline=none,unroll=1)/extern=strict/mach/define=rom-
	/obj=cp$obj:romdecom/include_dir=(cp$inc,cp$src)-
	/lis=cp$lis:romdecom cp$src:decom

cp$exe:huf_decom.exe : $(huf_decom_objects)
	link/alpha/system=%x400000/nosysshr-
	/exe=cp$exe:huf_decom/map=cp$exe:huf_decom -
	cp$obj:huf_decom+cp$obj:romdecom+cp$obj:rominflate

cp$exe:ev5_huf_decom.exe : $(ev5_huf_decom_objects)
	link/alpha/system=%x400000/nosysshr-
	/exe=cp$exe:ev5_huf_decom/map=cp$exe:ev5_huf_decom -
	cp$obj:ev5_huf_decom+cp$obj:romdecom+cp$obj:rominflate

cp$exe:ev6_huf_decom.exe : $(ev6_huf_decom_objects)
	link/alpha/system=%x400000/nosysshr-
	/exe=cp$exe:ev6_huf_decom/map=cp$exe:ev6_huf_decom -
	cp$obj:ev6_huf_decom+cp$obj:romdecom+cp$obj:rominflate

cp$exe:ev6_huf_decom_2m.exe : $(ev6_huf_decom_2m_objects)
	link/alpha/system=%x200000/nosysshr-
	/exe=cp$exe:ev6_huf_decom_2m/map=cp$exe:ev6_huf_decom_2m -
	cp$obj:ev6_huf_decom_2m+cp$obj:romdecom+cp$obj:rominflate

cp$exe:lzw_decom.exe 		: cp$obj:lzw_decom.obj
	link/alpha/system=0/exe=cp$exe:lzw_decom cp$obj:lzw_decom/nosyslib

cp$obj:lzw_decom.obj		: cp$src:lzw_decom.mar,-
				  cp$src:cfw.mlb
	$(macro64) -
		cp$src:platform.mar+cp$src:lzw_decom.mar/lis=cp$lis/obj=cp$obj -
		+cp$src:cfw/lib

cp$obj:x86.olb : cp$src:x86.olb
	copy cp$src:x86.olb cp$obj:x86.olb/nolog/noconf

do_x86 : 
.IFDEF	AVMS_HOST	
	continue
!	@ mms := mms
!	@ mms/desc=cp$src:x86.mms/macro="alpha=AVMS_HOST=1" cp$obj:x86.olb
.ELSE
	continue
.ENDIF

cp$obj:x86.depend : 
	continue
