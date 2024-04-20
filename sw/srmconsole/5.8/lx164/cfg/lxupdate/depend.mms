! File dependencies for the common console firmware

alphamm_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:common.h,-
	cp$src:alphamm_def.h,-
	cp$src:msg_def.h,-
	cp$src:impure_def.h,-
	cp$src:exdep_def.h,-
	cp$src:alphaps_def.h,-
	cp$src:alpha_def.h,-
	cp$src:aprdef.h,-
	cp$src:hwrpb_def.h,-
	cp$src:platform_cpu.h,-
	cp$inc:prototypes.h,-

ansi_depend	= -
	cp$src:platform.h,-
	cp$src:ansi_def.h,-
	cp$src:kernel_def.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-

aputchar_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:ansi_def.h,-
	cp$src:alpha_arc.h,-
	cp$src:chstate.h,-
	cp$src:display.h,-
	cp$inc:prototypes.h,-

atoi_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:ctype.h,-
	cp$src:msg_def.h,-

blex_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:msg_def.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:ctype.h,-
	cp$src:regexp_def.h,-
	cp$src:ev_def.h,-
	cp$src:tt_def.h,-
	cp$src:parse_def.h,-
	cp$src:parser.h,-
	cp$src:blex.h,-
	cp$inc:kernel_entry.h,-
	cp$inc:prototypes.h,-

bshell_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:msg_def.h,-
	cp$src:kernel_def.h,-
	cp$src:tt_def.h,-
	cp$src:blex.h,-
	cp$inc:kernel_entry.h,-
	cp$inc:prototypes.h,-

bshell_cmd_depend	= -
	cp$src:platform.h,-
	cp$src:msg_def.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:ctype.h,-
	cp$src:stddef.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:regexp_def.h,-
	cp$src:ev_def.h,-
	cp$src:tt_def.h,-
	cp$src:parse_def.h,-
	cp$src:parser.h,-
	cp$src:blex.h,-
	cp$inc:kernel_entry.h,-

boot_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:prdef.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:msg_def.h,-
	cp$src:ctype.h,-
	cp$src:hwrpb_def.h,-
	cp$src:parse_def.h,-
	cp$src:ev_def.h,-
	cp$src:btddef.h,-
	cp$src:mem_def.h,-
	cp$src:aprdef.h,-
	cp$src:impure_def.h,-
	cp$src:pal_def.h,-
	cp$inc:platform_io.h,-
	cp$src:platform_cpu.h,-
	cp$src:get_console_base.h,-
	cp$inc:kernel_entry.h,-
	cp$inc:prototypes.h,-

builtins_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:common.h,-
	cp$src:msg_def.h,-

call_backs_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:msg_def.h,-
	cp$src:ev_def.h,-
	cp$src:hwrpb_def.h,-
	cp$src:impure_def.h,-
	cp$src:pal_def.h,-
	cp$src:tt_def.h,-
	cp$src:platform_cpu.h,-
	cp$src:wwid.h,-
	cp$inc:kernel_entry.h,-
	cp$inc:prototypes.h,-
	cp$src:ni_dl_def.h,-
	cp$src:pb_def.h,-
	cp$src:ub_def.h,-
	cp$inc:platform_io.h,-

date_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:parse_def.h,-
	cp$src:msg_def.h,-
	cp$src:time.h,-
	cp$inc:prototypes.h,-

dynamic_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:parse_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:msg_def.h,-
	cp$src:ev_def.h,-
	cp$inc:prototypes.h,-

entry_depend	= -
	cp$src:platform.h,-
	cp$src:prdef.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:common.h,-
	cp$src:stddef.h,-
	cp$src:msg_def.h,-
	cp$src:ev_def.h,-
	cp$src:hwrpb_def.h,-
	cp$src:combo_def.h,-
	cp$src:pal_def.h,-
	cp$src:impure_def.h,-
	cp$src:aprdef.h,-
	cp$src:get_console_base.h,-
	cp$inc:kernel_entry.h,-
	cp$inc:prototypes.h,-

ev_action_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:ctype.h,-
	cp$src:msg_def.h,-
	cp$src:ev_def.h,-
	cp$src:pb_def.h,-
	cp$src:tt_def.h,-
	cp$src:combo_def.h,-
	cp$inc:platform_io.h,-
	cp$src:platform_cpu.h,-
	cp$inc:kernel_entry.h,-
	cp$src:kbd.h,-
	cp$src:hwrpb_def.h,-
	cp$src:mem_def.h,-
	cp$src:wwid.h,-
	cp$src:pal_def.h,-
	cp$src:impure_def.h,-

exdep_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:aprdef.h,-
	cp$src:common.h,-
	cp$src:dynamic_def.h,-
	cp$src:parse_def.h,-
	cp$src:msg_def.h,-
	cp$src:ev_def.h,-
	cp$src:exdep_def.h,-
	cp$src:ctype.h,-
	cp$src:alpha_def.h,-
	cp$src:alphaps_def.h,-
	cp$inc:kernel_entry.h,-
	cp$inc:prototypes.h,-

externdata_depend	= -
	cp$src:platform.h,-

filesys_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:stddef.h,-
	cp$src:common.h,-
	cp$src:wwid.h,-
	cp$src:msg_def.h,-
	cp$src:parse_def.h,-
	cp$src:pb_def.h,-
	cp$src:sb_def.h,-
	cp$src:ub_def.h,-
	cp$src:ni_dl_def.h,-
	cp$src:probe_io_def.h,-
	cp$src:ctype.h,-
	cp$src:kgpsa_emx.h,-
	cp$src:kgpsa_def.h,-
	cp$inc:prototypes.h,-
	cp$src:dst.c,-

hwrpb_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:msg_def.h,-
	cp$src:ev_def.h,-
	cp$src:mem_def.h,-
	cp$src:hwrpb_def.h,-
	cp$src:pal_def.h,-
	cp$src:impure_def.h,-
	cp$src:platform_cpu.h,-
	cp$src:get_console_base.h,-
	cp$inc:kernel_entry.h,-
	cp$src:memory_chunk.h,-
	cp$inc:prototypes.h,-
	cp$src:version.h,-

hwrpbtt_driver_depend	= -
	cp$src:platform.h,-
	cp$src:stddef.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:common.h,-
	cp$src:msg_def.h,-
	cp$src:tt_def.h,-
	cp$src:hwrpb_def.h,-
	cp$inc:kernel_entry.h,-
	cp$inc:prototypes.h,-

ie_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:stddef.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:msg_def.h,-
	cp$src:pd_def.h,-
	cp$src:alphascb_def.h,-
	cp$src:ctype.h,-
	cp$src:platform_cpu.h,-

iobq_cmd_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:kernel_def.h,-
	cp$src:diag_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:msg_def.h,-
	cp$src:parse_def.h,-

isacfg_depend	= -
	cp$src:msgnum.h,-
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$inc:kernel_entry.h,-
	cp$src:dynamic_def.h,-
	cp$src:parse_def.h,-
	cp$src:parser.h,-
	cp$src:eisa.h,-
	cp$src:ev_def.h,-
	cp$src:pb_def.h,-
	cp$src:nvram_def.h,-
	cp$src:common.h,-
	cp$src:isacfg.h,-
	cp$inc:platform_io.h,-
	cp$inc:prototypes.h,-

kernel_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:prdef.h,-
	cp$src:pal_def.h,-
	cp$src:impure_def.h,-
	cp$src:alphascb_def.h,-
	cp$src:version.h,-
	cp$src:dynamic_def.h,-
	cp$src:msg_def.h,-
	cp$src:ev_def.h,-
	cp$src:tt_def.h,-
	cp$src:hwrpb_def.h,-
	cp$src:ansi_def.h,-
	cp$src:platform_cpu.h,-
	cp$src:kbd.h,-
	cp$src:get_console_base.h,-
	cp$src:alpha_arc.h,-
	cp$src:ctype.h,-
	cp$inc:kernel_entry.h,-
	cp$inc:prototypes.h,-

kernel_support_depend	= -
	cp$src:platform.h,-
	cp$src:alpha_def.h,-
	cp$src:ctype.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:msg_def.h,-
	cp$src:common.h,-
	cp$src:wwid.h,-
	cp$src:pb_def.h,-
	cp$src:sb_def.h,-
	cp$src:cb_def.h,-
	cp$src:ub_def.h,-
	cp$src:pal_def.h,-
	cp$src:platform_cpu.h,-
	cp$inc:kernel_entry.h,-
	cp$inc:prototypes.h,-

memset_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-

mopdl_driver_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:inet.h,-
	cp$src:prdef.h,-
	cp$src:dynamic_def.h,-
	cp$src:msg_def.h,-
	cp$src:ni_env.h,-
	cp$src:mop_def.h,-
	cp$src:mb_def.h,-
	cp$src:mop_counters.h,-
	cp$src:ev_def.h,-
	cp$src:ctype.h,-
	cp$inc:kernel_entry.h,-
	cp$inc:prototypes.h,-

moplp_driver_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:inet.h,-
	cp$src:dynamic_def.h,-
	cp$src:msg_def.h,-
	cp$src:ni_env.h,-
	cp$src:ni_dl_def.h,-
	cp$src:mop_def.h,-
	cp$src:mb_def.h,-
	cp$src:mop_counters.h,-
	cp$src:ev_def.h,-
	cp$inc:kernel_entry.h,-
	cp$inc:prototypes.h,-

ni_datalink_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:inet.h,-
	cp$src:prototypes.h,-
	cp$src:msg_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:ni_env.h,-
	cp$src:ni_dl_def.h,-
	cp$src:mop_counters.h,-
	cp$inc:kernel_entry.h,-

pke_driver_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:ev_def.h,-
	cp$src:msg_def.h,-
	cp$src:n810_def.h,-
	cp$src:pb_def.h,-
	cp$src:n810_pb_def.h,-
	cp$src:scsi_def.h,-
	cp$src:sb_def.h,-
	cp$src:ub_def.h,-
	cp$inc:platform_io.h,-
	cp$inc:kernel_entry.h,-
	cp$inc:prototypes.h,-

printf_depend	= -
	cp$src:platform.h,-
	cp$src:ctype.h,-
	cp$src:stddef.h,-
	cp$src:kernel_def.h,-
	cp$src:msg_def.h,-
	cp$src:common.h,-
	cp$src:ovly_def.h,-
	cp$src:printf.h,-
	cp$src:varargs.h,-
	cp$src:tt_def.h,-
	cp$inc:kernel_entry.h,-
	cp$inc:prototypes.h,-

probe_io_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:pb_def.h,-
	cp$src:probe_io_def.h,-
	cp$inc:platform_io.h,-
	cp$src:eisa.h,-
	cp$inc:kernel_entry.h,-

process_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:prdef.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:msg_def.h,-

qscan_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:parse_def.h,-
	cp$src:msg_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:ctype.h,-
	cp$src:stddef.h,-
	cp$inc:prototypes.h,-

queue_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$inc:prototypes.h,-

random_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-

readline_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:ansi_def.h,-
	cp$src:ctype.h,-
	cp$src:common.h,-
	cp$src:msg_def.h,-
	cp$src:tt_def.h,-
	cp$inc:prototypes.h,-

regexp_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:version.h,-
	cp$src:stddef.h,-
	cp$src:ctype.h,-
	cp$src:limits.h,-
	cp$src:regexp_def.h,-
	cp$src:ansi_def.h,-
	cp$src:msg_def.h,-
	cp$src:parse_def.h,-
	cp$inc:prototypes.h,-

scsi_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:msg_def.h,-
	cp$src:scsi_def.h,-
	cp$src:ev_def.h,-
	cp$src:pb_def.h,-
	cp$src:sb_def.h,-
	cp$src:ub_def.h,-
	cp$src:ctype.h,-
	cp$src:common.h,-
	cp$src:wwid.h,-
	cp$inc:prototypes.h,-
	cp$src:print_var.h,-
	cp$inc:kernel_entry.h,-

semaphore_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:prdef.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:msg_def.h,-
	cp$src:platform_cpu.h,-
	cp$inc:prototypes.h,-

show_hwrpb_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:hwrpb_def.h,-
	cp$inc:kernel_entry.h,-
	cp$inc:prototypes.h,-

show_map_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:common.h,-
	cp$src:alphamm_def.h,-
	cp$src:msg_def.h,-
	cp$src:impure_def.h,-
	cp$src:exdep_def.h,-
	cp$src:alphaps_def.h,-
	cp$src:alpha_def.h,-
	cp$src:aprdef.h,-
	cp$src:hwrpb_def.h,-
	cp$src:platform_cpu.h,-
	cp$inc:prototypes.h,-

setshow_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:parse_def.h,-
	cp$src:parser.h,-
	cp$src:version.h,-
	cp$src:dynamic_def.h,-
	cp$src:ctype.h,-
	cp$src:stddef.h,-
	cp$src:prdef.h,-
	cp$src:msg_def.h,-
	cp$src:ev_def.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:pb_def.h,-
	cp$src:sb_def.h,-
	cp$src:cb_def.h,-
	cp$src:ub_def.h,-
	cp$src:tt_def.h,-
	cp$src:regexp_def.h,-
	cp$inc:kernel_entry.h,-

shellsort_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-

startstop_depend	= -
	cp$src:platform.h,-
	cp$src:prdef.h,-
	cp$src:kernel_def.h,-
	cp$src:parse_def.h,-
	cp$src:version.h,-
	cp$src:dynamic_def.h,-
	cp$src:ctype.h,-
	cp$src:stddef.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:msg_def.h,-
	cp$src:ev_def.h,-
	cp$src:tt_def.h,-
	cp$src:pal_def.h,-
	cp$inc:kernel_entry.h,-
	cp$src:hwrpb_def.h,-
	cp$src:impure_def.h,-
	cp$src:aprdef.h,-

strfunc_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:ctype.h,-

timer_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:pal_def.h,-
	cp$src:prdef.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:impure_def.h,-
	cp$src:msg_def.h,-
	cp$src:tt_def.h,-
	cp$src:parse_def.h,-
	cp$inc:platform_io.h,-
	cp$src:platform_cpu.h,-
	cp$src:alphascb_def.h,-
	cp$inc:prototypes.h,-

uptime_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-

xcmd_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:impure_def.h,-
	cp$src:stddef.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:msg_def.h,-
	cp$src:tt_def.h,-
	cp$src:parse_def.h,-
	cp$inc:platform_io.h,-
	cp$src:dynamic_def.h,-
	cp$src:eisa.h,-
	cp$src:parser.h,-
	cp$src:pal_def.h,-

xlate_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:kbd.h,-

lx164_depend	= -
	cp$inc:platform_io.h,-
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:pb_def.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:alphascb_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:ev_def.h,-
	cp$src:alpha_def.h,-
	cp$src:aprdef.h,-
	cp$src:hwrpb_def.h,-
	cp$src:pal_def.h,-
	cp$src:msg_def.h,-
	cp$src:parse_def.h,-
	cp$src:nvram_def.h,-
	cp$src:platform_cpu.h,-
	cp$src:apc.h,-
	cp$src:isacfg.h,-
	cp$src:impure_def.h,-
	cp$src:i82378_def.h,-
	cp$src:pyxis_main_csr_def.h,-
	cp$src:romhead.h,-
	cp$src:ev5_defs.h,-

lx164_init_depend	= -
	cp$src:kernel_def.h,-
	cp$src:pb_def.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:alphascb_def.h,-
	cp$src:ev_def.h,-
	cp$src:alpha_def.h,-
	cp$src:aprdef.h,-
	cp$src:hwrpb_def.h,-
	cp$src:pal_def.h,-
	cp$src:msg_def.h,-
	cp$src:parse_def.h,-
	cp$src:eisa.h,-
	cp$src:tt_def.h,-
	cp$src:probe_io_def.h,-
	cp$src:combo_def.h,-
	cp$src:platform_cpu.h,-
	cp$inc:platform_io.h,-
	cp$src:platform.h,-
	cp$src:apc.h,-
	cp$src:pyxis_main_csr_def.h,-
	cp$src:pyxis_addr_trans_def.h,-
	cp$src:impure_def.h,-
	cp$src:smc_def.h,-
	cp$src:i82378_def.h,-
	cp$src:ev5_defs.h,-

lx164_mcheck_depend	= -
	cp$inc:platform_io.h,-
	cp$src:kernel_def.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:alphascb_def.h,-
	cp$src:alpha_def.h,-
	cp$src:aprdef.h,-
	cp$src:hwrpb_def.h,-
	cp$src:pal_def.h,-
	cp$src:msg_def.h,-
	cp$src:platform.h,-
	cp$src:platform_cpu.h,-
	cp$src:pyxis_main_csr_def.h,-
	cp$src:pyxis_mem_csr_def.h,-
	cp$src:ev5_defs.h,-

lx164_io_depend	= -
	cp$src:stddef.h,-
	cp$src:kernel_def.h,-
	cp$src:pb_def.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:msg_def.h,-
	cp$src:probe_io_def.h,-
	cp$src:pyxis_main_csr_def.h,-
	cp$src:pyxis_addr_trans_def.h,-
	cp$inc:platform_io.h,-
	cp$src:platform.h,-
	cp$src:platform_cpu.h,-
	cp$src:i82378_def.h,-

memconfig_lx164_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:msg_def.h,-
	cp$src:stddef.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:dynamic_def.h,-
	cp$src:mem_def.h,-
	cp$src:hwrpb_def.h,-
	cp$src:ev_def.h,-
	cp$src:ctype.h,-
	cp$src:pal_def.h,-
	cp$inc:platform_io.h,-
	cp$src:platform_cpu.h,-
	cp$src:impure_def.h,-
	cp$src:pyxis_mem_csr_def.h,-
	cp$src:memory_chunk.h,-

powerup_lx164_depend	= -
	cp$inc:platform_io.h,-
	cp$src:kernel_def.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:dynamic_def.h,-
	cp$src:msg_def.h,-
	cp$src:hwrpb_def.h,-
	cp$src:prdef.h,-
	cp$src:platform.h,-
	cp$src:pyxis_main_csr_def.h,-

show_config_lx164_depend	= -
	cp$inc:platform_io.h,-
	cp$src:kernel_def.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:msg_def.h,-
	cp$src:parse_def.h,-
	cp$src:ev_def.h,-
	cp$src:pal_def.h,-
	cp$src:impure_def.h,-
	cp$src:version.h,-
	cp$src:pb_def.h,-
	cp$src:probe_io_def.h,-
	cp$src:ub_def.h,-
	cp$src:hwrpb_def.h,-
	cp$src:platform.h,-
	cp$src:platform_cpu.h,-
	cp$src:apc.h,-

ebflash_support_depend	= -
	cp$src:platform.h,-
	cp$inc:platform_io.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:stddef.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:msg_def.h,-
	cp$src:ebflash_def.h,-
	cp$src:romhead.h,-

allocfree_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:parse_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:msg_def.h,-
	cp$inc:prototypes.h,-

bpt_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$inc:kernel_entry.h,-

cat_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:parse_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:ansi_def.h,-
	cp$src:msg_def.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:limits.h,-

check_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:msg_def.h,-
	cp$src:parse_def.h,-

chmod_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:parse_def.h,-
	cp$src:msg_def.h,-
	cp$src:dynamic_def.h,-

cmp_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:parse_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:stddef.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:msg_def.h,-

decode_alpha_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:instops_alpha.h,-
	cp$inc:prototypes.h,-

diag_support_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:prdef.h,-
	cp$src:ctype.h,-
	cp$src:diag_def.h,-
	cp$src:diag_groups.h,-
	cp$src:dynamic_def.h,-
	cp$src:ev_def.h,-
	cp$src:msg_def.h,-
	cp$src:parse_def.h,-
	cp$src:time.h,-
	cp$src:version.h,-
	cp$src:medulla_errors.h,-
	cp$inc:kernel_entry.h,-
	cp$inc:prototypes.h,-

diag_evs_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:parser.h,-
	cp$src:stddef.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:diag_def.h,-
	cp$src:diag_groups.h,-
	cp$src:ev_def.h,-
	cp$src:msg_def.h,-
	cp$src:pal_def.h,-

echo_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:parse_def.h,-
	cp$src:msg_def.h,-

edit_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:msg_def.h,-
	cp$src:common.h,-
	cp$src:ctype.h,-
	cp$inc:prototypes.h,-

eval_depend	= -
	cp$src:platform.h,-
	cp$src:ev_def.h,-
	cp$src:parse_def.h,-
	cp$src:msg_def.h,-
	cp$src:kernel_def.h,-
	cp$src:common.h,-
	cp$inc:kernel_entry.h,-
	cp$inc:prototypes.h,-

find_field_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:parse_def.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:msg_def.h,-

grep_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:common.h,-
	cp$src:ctype.h,-
	cp$src:regexp_def.h,-
	cp$src:msg_def.h,-
	cp$src:parse_def.h,-
	cp$inc:prototypes.h,-

hd_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:parse_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:msg_def.h,-
	cp$src:ctype.h,-

line_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:msg_def.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-

login_cmd_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:msg_def.h,-
	cp$src:ev_def.h,-
	cp$src:ctype.h,-
	cp$inc:kernel_entry.h,-

ls_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:stddef.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:msg_def.h,-
	cp$src:parse_def.h,-

moprc_driver_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:inet.h,-
	cp$src:prdef.h,-
	cp$src:dynamic_def.h,-
	cp$src:msg_def.h,-
	cp$src:ni_env.h,-
	cp$src:mop_def.h,-
	cp$src:mb_def.h,-
	cp$src:mop_counters.h,-
	cp$src:ev_def.h,-
	cp$inc:prototypes.h,-

moprctt_driver_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:inet.h,-
	cp$src:dynamic_def.h,-
	cp$src:msg_def.h,-
	cp$src:tt_def.h,-
	cp$src:mop_def.h,-
	cp$src:mb_def.h,-
	cp$inc:prototypes.h,-

more_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:parse_def.h,-
	cp$src:ctype.h,-
	cp$src:msg_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:tt_def.h,-
	cp$inc:prototypes.h,-

ps_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:msg_def.h,-

rm_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:msg_def.h,-

secure_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:msg_def.h,-
	cp$src:ev_def.h,-
	cp$src:ctype.h,-
	cp$inc:kernel_entry.h,-
	cp$inc:prototypes.h,-

sem_cmd_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:msg_def.h,-

simport_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:ev_def.h,-
	cp$src:msg_def.h,-
	cp$src:common.h,-
	cp$src:stddef.h,-
	cp$src:pb_def.h,-
	cp$src:simport.h,-
	cp$src:sim_pb_def.h,-
	cp$src:cam.h,-
	cp$src:scsi_def.h,-
	cp$src:sb_def.h,-
	cp$src:ub_def.h,-
	cp$src:hwrpb_def.h,-
	cp$inc:platform_io.h,-
	cp$inc:kernel_entry.h,-
	cp$inc:prototypes.h,-

sleep_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:msg_def.h,-
	cp$src:parse_def.h,-

sort_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:parse_def.h,-
	cp$src:version.h,-
	cp$src:dynamic_def.h,-
	cp$src:ctype.h,-
	cp$src:stddef.h,-
	cp$src:prdef.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:msg_def.h,-

sound_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:parse_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:ctype.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:msg_def.h,-

symbols_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:parse_def.h,-
	cp$src:sym_def.h,-

tr_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:msg_def.h,-

truefalse_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:msg_def.h,-

validate_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:stddef.h,-
	cp$src:dynamic_def.h,-
	cp$src:parse_def.h,-
	cp$src:ev_def.h,-
	cp$src:msg_def.h,-
	cp$inc:prototypes.h,-

wc_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:parse_def.h,-
	cp$src:version.h,-
	cp$src:ctype.h,-
	cp$src:stddef.h,-
	cp$src:prdef.h,-
	cp$src:msg_def.h,-
	cp$inc:prototypes.h,-

tga_driver_port_depend	= -
	cp$src:display.h,-
	cp$src:platform.h,-
	cp$src:ansi_def.h,-
	cp$src:alpha_arc.h,-
	cp$src:chstate.h,-
	cp$src:tga_sfbp_callbacks.h,-
	cp$src:tga_sfbp_prom_defines.h,-
	cp$src:tga_sfbp_def.h,-
	cp$src:tga_sfbp_protos.h,-
	cp$src:tga_sfbp_463.h,-
	cp$src:tga_sfbp_485.h,-
	cp$src:tga_sfbp_ext.h,-
	cp$src:tga_sfbp_crb_def.h,-
	cp$src:tga_sfbp_ctb_def.h,-
	cp$src:tga_sfbp_driver_crystal.h,-
	cp$src:tga_sfbp_pci_def.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:stddef.h,-
	cp$src:prdef.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:pb_def.h,-
	cp$src:eisa.h,-
	cp$inc:platform_io.h,-
	cp$src:vgapb.h,-

tga_driver_stip_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:tga_sfbp_include.h,-
	cp$src:tga_sfbp_callbacks.h,-
	cp$src:tga_sfbp_prom_defines.h,-
	cp$src:tga_sfbp_def.h,-
	cp$src:tga_sfbp_protos.h,-
	cp$src:tga_sfbp_463.h,-
	cp$src:tga_sfbp_485.h,-
	cp$src:tga_sfbp_ext.h,-
	cp$src:tga_sfbp_crb_def.h,-
	cp$src:tga_sfbp_ctb_def.h,-

tga_driver_bt485_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:tga_sfbp_include.h,-
	cp$src:tga_sfbp_callbacks.h,-
	cp$src:tga_sfbp_prom_defines.h,-
	cp$src:tga_sfbp_def.h,-
	cp$src:tga_sfbp_protos.h,-
	cp$src:tga_sfbp_463.h,-
	cp$src:tga_sfbp_485.h,-
	cp$src:tga_sfbp_ext.h,-
	cp$src:tga_sfbp_crb_def.h,-
	cp$src:tga_sfbp_ctb_def.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:ev_def.h,-
	cp$src:msg_def.h,-

tga_driver_bt463_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:tga_sfbp_include.h,-
	cp$src:tga_sfbp_callbacks.h,-
	cp$src:tga_sfbp_prom_defines.h,-
	cp$src:tga_sfbp_def.h,-
	cp$src:tga_sfbp_protos.h,-
	cp$src:tga_sfbp_463.h,-
	cp$src:tga_sfbp_485.h,-
	cp$src:tga_sfbp_ext.h,-
	cp$src:tga_sfbp_crb_def.h,-
	cp$src:tga_sfbp_ctb_def.h,-
	cp$src:msg_def.h,-
	cp$src:ev_def.h,-

tga_driver_visual_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:tga_sfbp_include.h,-
	cp$src:tga_sfbp_callbacks.h,-
	cp$src:tga_sfbp_prom_defines.h,-
	cp$src:tga_sfbp_def.h,-
	cp$src:tga_sfbp_protos.h,-
	cp$src:tga_sfbp_463.h,-
	cp$src:tga_sfbp_485.h,-
	cp$src:tga_sfbp_ext.h,-
	cp$src:tga_sfbp_crb_def.h,-
	cp$src:tga_sfbp_ctb_def.h,-

tga_driver_support_depend	= -
	cp$src:platform.h,-
	cp$src:tga_sfbp_callbacks.h,-
	cp$src:tga_sfbp_prom_defines.h,-
	cp$src:tga_sfbp_def.h,-
	cp$src:tga_sfbp_protos.h,-
	cp$src:tga_sfbp_463.h,-
	cp$src:tga_sfbp_485.h,-
	cp$src:tga_sfbp_ext.h,-
	cp$src:tga_sfbp_crb_def.h,-
	cp$src:tga_sfbp_ctb_def.h,-
	cp$src:tga_sfbp_driver_crystal.h,-
	cp$src:tga_sfbp_pci_def.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:stddef.h,-
	cp$src:prdef.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:pb_def.h,-
	cp$src:eisa.h,-
	cp$inc:platform_io.h,-

tga_driver_busio_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:stddef.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:msg_def.h,-
	cp$src:tt_def.h,-
	cp$src:ev_def.h,-
	cp$src:time.h,-
	cp$src:pb_def.h,-
	cp$src:eisa.h,-
	cp$inc:platform_io.h,-
	cp$src:display.h,-
	cp$src:hwrpb_def.h,-
	cp$src:tga_sfbp_include.h,-
	cp$src:tga_sfbp_callbacks.h,-
	cp$src:tga_sfbp_prom_defines.h,-
	cp$src:tga_sfbp_def.h,-
	cp$src:tga_sfbp_protos.h,-
	cp$src:tga_sfbp_463.h,-
	cp$src:tga_sfbp_485.h,-
	cp$src:tga_sfbp_ext.h,-
	cp$src:tga_sfbp_crb_def.h,-
	cp$src:tga_sfbp_ctb_def.h,-

nl_driver_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:msg_def.h,-

rd_driver_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:stddef.h,-
	cp$src:msg_def.h,-
	cp$src:encap.c,-
	cp$inc:prototypes.h,-

toy_driver_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:stddef.h,-
	cp$src:common.h,-
	cp$src:msg_def.h,-
	cp$src:tt_def.h,-
	cp$src:combo_def.h,-
	cp$src:pb_def.h,-
	cp$inc:prototypes.h,-

pci_driver_depend	= -
	cp$src:kernel_def.h,-
	cp$src:pb_def.h,-
	cp$src:probe_io_def.h,-
	cp$src:common.h,-
	cp$src:msg_def.h,-
	cp$inc:prototypes.h,-
	cp$inc:platform_io.h,-

from_driver_depend	= -
	cp$src:platform.h,-
	cp$inc:platform_io.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:stddef.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:msg_def.h,-
	cp$src:from_def.h,-

nvram_driver_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:msg_def.h,-
	cp$src:common.h,-
	cp$src:stddef.h,-
	cp$src:alpha_arc.h,-
	cp$src:alpha_arc_eisa.h,-
	cp$src:nvram_def.h,-
	cp$inc:prototypes.h,-

tt_driver_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:ansi_def.h,-
	cp$src:msg_def.h,-
	cp$src:tt_def.h,-
	cp$inc:kernel_entry.h,-
	cp$inc:prototypes.h,-

tga_driver_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:stddef.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:msg_def.h,-
	cp$src:tt_def.h,-
	cp$src:ev_def.h,-
	cp$src:time.h,-
	cp$src:pb_def.h,-
	cp$src:eisa.h,-
	cp$inc:platform_io.h,-
	cp$src:display.h,-
	cp$src:hwrpb_def.h,-
	cp$src:vgafont.h,-
	cp$src:tga_sfbp_callbacks.h,-
	cp$src:tga_sfbp_prom_defines.h,-
	cp$src:tga_sfbp_def.h,-
	cp$src:tga_sfbp_protos.h,-
	cp$src:tga_sfbp_463.h,-
	cp$src:tga_sfbp_485.h,-
	cp$src:tga_sfbp_ext.h,-
	cp$src:tga_sfbp_crb_def.h,-
	cp$src:tga_sfbp_ctb_def.h,-
	cp$src:tga_sfbp_driver_crystal.h,-
	cp$src:tga_sfbp_pci_def.h,-
	cp$src:tga_sfbp_data_ptr.h,-
	cp$src:tga_sfbp_data.h,-
	cp$src:ansi_def.h,-
	cp$src:alpha_arc.h,-
	cp$src:chstate.h,-
	cp$src:vgapb.h,-

vga_bios_driver_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:common.h,-
	cp$src:msg_def.h,-
	cp$src:tt_def.h,-
	cp$src:ansi_def.h,-
	cp$src:alpha_arc.h,-
	cp$src:chstate.h,-
	cp$src:display.h,-
	cp$src:vgag.h,-
	cp$src:pb_def.h,-
	cp$src:vgapb.h,-
	cp$inc:platform_io.h,-
	cp$src:hwrpb_def.h,-
	cp$src:mem_def.h,-
	cp$src:probe_io_def.h,-
	cp$inc:kernel_entry.h,-
	cp$inc:prototypes.h,-

kbd_driver_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:stddef.h,-
	cp$src:common.h,-
	cp$src:ctype.h,-
	cp$src:ev_def.h,-
	cp$src:msg_def.h,-
	cp$src:hwrpb_def.h,-
	cp$src:tt_def.h,-
	cp$src:kbd.h,-
	cp$src:pb_def.h,-
	cp$src:vgapb.h,-
	cp$src:kbd_pb_def.h,-
	cp$inc:platform_io.h,-
	cp$src:platform_cpu.h,-
	cp$inc:kernel_entry.h,-
	cp$inc:prototypes.h,-

combo_driver_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:stddef.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:msg_def.h,-
	cp$src:tt_def.h,-
	cp$src:combo_def.h,-
	cp$src:hwrpb_def.h,-
	cp$src:isacfg.h,-
	cp$src:pb_def.h,-
	cp$src:platform_cpu.h,-
	cp$src:ansi_def.h,-
	cp$inc:kernel_entry.h,-

ev_driver_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:prototypes.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:prdef.h,-
	cp$src:version.h,-
	cp$src:msg_def.h,-
	cp$src:ev_def.h,-
	cp$src:hwrpb_def.h,-
	cp$src:pal_def.h,-
	cp$src:impure_def.h,-
	cp$inc:kernel_entry.h,-

el_driver_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:dynamic_def.h,-
	cp$src:msg_def.h,-

pmem_driver_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:common.h,-
	cp$src:dynamic_def.h,-
	cp$src:exdep_def.h,-
	cp$src:msg_def.h,-
	cp$src:alphascb_def.h,-
	cp$inc:prototypes.h,-

vmem_driver_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:aprdef.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:msg_def.h,-
	cp$src:exdep_def.h,-
	cp$src:alpha_def.h,-
	cp$src:alphaps_def.h,-
	cp$src:alphamm_def.h,-
	cp$src:get_console_base.h,-
	cp$src:alphascb_def.h,-

examine_driver_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:kernel_def.h,-
	cp$src:msg_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:exdep_def.h,-
	cp$src:alpha_def.h,-
	cp$inc:kernel_entry.h,-

ev5_ipr_driver_depend	= -
	cp$src:platform.h,-
	cp$src:prdef.h,-
	cp$src:stddef.h,-
	cp$src:kernel_def.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:msg_def.h,-
	cp$src:exdep_def.h,-
	cp$src:alpha_def.h,-
	cp$src:alphaps_def.h,-
	cp$src:aprdef.h,-
	cp$src:pal_def.h,-
	cp$src:impure_def.h,-
	cp$src:platform_cpu.h,-

gpr_driver_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:prdef.h,-
	cp$src:kernel_def.h,-
	cp$src:msg_def.h,-
	cp$src:exdep_def.h,-
	cp$src:alphaps_def.h,-
	cp$src:alpha_def.h,-
	cp$src:pal_def.h,-
	cp$src:impure_def.h,-

pt_driver_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:kernel_def.h,-
	cp$src:msg_def.h,-
	cp$src:exdep_def.h,-
	cp$src:alpha_def.h,-
	cp$src:pal_def.h,-
	cp$src:impure_def.h,-

ps_driver_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:prdef.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:msg_def.h,-
	cp$src:exdep_def.h,-
	cp$src:alpha_def.h,-
	cp$src:pal_def.h,-
	cp$src:impure_def.h,-

ndbr_driver_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:msg_def.h,-
	cp$src:parse_def.h,-
	cp$src:mop_def.h,-
	cp$src:mb_def.h,-
	cp$inc:kernel_entry.h,-

mop_driver_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:inet.h,-
	cp$src:dynamic_def.h,-
	cp$src:msg_def.h,-
	cp$src:ni_env.h,-
	cp$src:mop_def.h,-
	cp$src:mb_def.h,-
	cp$src:mop_counters.h,-
	cp$src:ev_def.h,-
	cp$inc:kernel_entry.h,-
	cp$inc:prototypes.h,-

fpr_driver_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:kernel_def.h,-
	cp$src:prdef.h,-
	cp$src:msg_def.h,-
	cp$src:exdep_def.h,-
	cp$src:alpha_def.h,-
	cp$src:impure_def.h,-
	cp$src:pal_def.h,-

pi_driver_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:stddef.h,-
	cp$src:msg_def.h,-
	cp$inc:prototypes.h,-

decode_driver_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:msg_def.h,-

tee_driver_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:msg_def.h,-

para_driver_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:platform_cpu.h,-
	cp$src:dynamic_def.h,-
	cp$src:stddef.h,-
	cp$src:msg_def.h,-
	cp$src:combo_def.h,-
	cp$src:isacfg.h,-
	cp$inc:platform_io.h,-
	cp$inc:prototypes.h,-

inet_driver_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:inet.h,-
	cp$src:prdef.h,-
	cp$src:dynamic_def.h,-
	cp$src:msg_def.h,-
	cp$src:ni_env.h,-
	cp$src:ni_dl_def.h,-
	cp$src:ev_def.h,-
	cp$src:ctype.h,-
	cp$inc:kernel_entry.h,-
	cp$inc:prototypes.h,-

fat_driver_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:common.h,-
	cp$src:alpha_arc.h,-
	cp$src:msg_def.h,-
	cp$src:fat.h,-
	cp$src:ctype.h,-
	cp$src:limits.h,-
	cp$src:regexp_def.h,-
	cp$src:time.h,-
	cp$inc:prototypes.h,-

dq_driver_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:msg_def.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:eisa.h,-
	cp$src:scsi_def.h,-
	cp$src:pb_def.h,-
	cp$src:dq_pb_def.h,-
	cp$src:sb_def.h,-
	cp$src:ub_def.h,-
	cp$src:probe_io_def.h,-
	cp$inc:platform_io.h,-
	cp$inc:kernel_entry.h,-

de205_driver_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:msg_def.h,-
	cp$src:prdef.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:dynamic_def.h,-
	cp$src:ni_env.h,-
	cp$src:ev_def.h,-
	cp$src:de205.h,-
	cp$src:ni_dl_def.h,-
	cp$src:pb_def.h,-
	cp$src:de205_pb_def.h,-
	cp$src:mop_counters.h,-
	cp$src:eisa.h,-
	cp$inc:platform_io.h,-
	cp$src:isacfg.h,-
	cp$src:parse_def.h,-
	cp$src:parser.h,-
	cp$src:platform_cpu.h,-
	cp$inc:kernel_entry.h,-

dv_driver_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:prototypes.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:ev_def.h,-
	cp$src:msg_def.h,-
	cp$src:stddef.h,-
	cp$src:ide_def.h,-
	cp$src:ide_csr_def.h,-
	cp$src:hwrpb_def.h,-
	cp$src:probe_io_def.h,-
	cp$src:platform_cpu.h,-
	cp$src:eisa.h,-
	cp$src:isp_def.h,-
	cp$src:ctype.h,-
	cp$src:ub_def.h,-
	cp$src:pb_def.h,-
	cp$src:ide_pb_def.h,-
	cp$inc:platform_io.h,-
	cp$src:isacfg.h,-
	cp$src:mem_def.h,-
	cp$inc:kernel_entry.h,-

n810_driver_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:platform_cpu.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:ev_def.h,-
	cp$src:msg_def.h,-
	cp$src:ctype.h,-
	cp$src:n810_def.h,-
	cp$src:pb_def.h,-
	cp$src:sb_def.h,-
	cp$src:n810_pb_def.h,-
	cp$inc:platform_io.h,-
	cp$inc:kernel_entry.h,-
	cp$inc:prototypes.h,-

pci_nvram_driver_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:msg_def.h,-
	cp$src:pb_def.h,-
	cp$src:pci_nvram_pb_def.h,-
	cp$src:mem_def.h,-
	cp$src:dynamic_def.h,-
	cp$inc:platform_io.h,-
	cp$inc:prototypes.h,-

pdq_driver_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:mop_def.h,-
	cp$src:mb_def.h,-
	cp$src:parse_def.h,-
	cp$src:pci_def.h,-
	cp$src:hwrpb_def.h,-
	cp$src:ni_dl_def.h,-
	cp$src:pb_def.h,-
	cp$src:mop_counters64.h,-
	cp$src:dynamic_def.h,-
	cp$src:ni_env.h,-
	cp$src:msg_def.h,-
	cp$src:ev_def.h,-
	cp$src:pdq_def.h,-
	cp$src:pdq_pb_def.h,-
	cp$inc:platform_io.h,-
	cp$src:probe_io_def.h,-
	cp$src:ctype.h,-
	cp$inc:kernel_entry.h,-

dw_driver_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:msg_def.h,-
	cp$src:prdef.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:dynamic_def.h,-
	cp$src:ni_env.h,-
	cp$src:ev_def.h,-
	cp$src:ni_dl_def.h,-
	cp$src:pb_def.h,-
	cp$src:mop_counters.h,-
	cp$src:eisa.h,-
	cp$inc:platform_io.h,-
	cp$src:probe_io_def.h,-
	cp$src:dw300.h,-
	cp$src:tms380.h,-
	cp$src:tms380_def.h,-
	cp$src:dw_pb.h,-
	cp$inc:kernel_entry.h,-

dac960_driver_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:prototypes.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:msg_def.h,-
	cp$src:eisa.h,-
	cp$src:scsi_def.h,-
	cp$src:pb_def.h,-
	cp$src:dac960_pb_def.h,-
	cp$src:sb_def.h,-
	cp$src:ub_def.h,-
	cp$src:probe_io_def.h,-
	cp$inc:platform_io.h,-
	cp$src:platform_cpu.h,-
	cp$inc:kernel_entry.h,-

pks_driver_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:ev_def.h,-
	cp$src:msg_def.h,-
	cp$src:common.h,-
	cp$src:stddef.h,-
	cp$src:pb_def.h,-
	cp$src:sim_pb_def.h,-
	cp$src:pks_flash_def.h,-
	cp$src:pks_pb_def.h,-
	cp$src:simport.h,-
	cp$src:cam.h,-
	cp$src:scsi_def.h,-
	cp$src:sb_def.h,-
	cp$src:ub_def.h,-
	cp$src:hwrpb_def.h,-
	cp$src:eisa.h,-
	cp$inc:platform_io.h,-
	cp$src:mem_def.h,-
	cp$src:probe_io_def.h,-
	cp$inc:kernel_entry.h,-
	cp$inc:prototypes.h,-

isp1020_driver_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:ev_def.h,-
	cp$src:msg_def.h,-
	cp$src:scsi_def.h,-
	cp$src:common.h,-
	cp$src:stddef.h,-
	cp$src:pb_def.h,-
	cp$src:isp1020_def.h,-
	cp$src:isp1020_pb_def.h,-
	cp$src:sb_def.h,-
	cp$src:ub_def.h,-
	cp$inc:platform_io.h,-
	cp$inc:kernel_entry.h,-
	cp$inc:prototypes.h,-

ew_driver_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:platform_cpu.h,-
	cp$src:kernel_def.h,-
	cp$src:msg_def.h,-
	cp$src:prdef.h,-
	cp$src:dynamic_def.h,-
	cp$src:pb_def.h,-
	cp$src:ni_dl_def.h,-
	cp$src:ni_env.h,-
	cp$src:ev_def.h,-
	cp$src:mop_counters64.h,-
	cp$src:tu.h,-
	cp$src:tupb.h,-
	cp$src:probe_io_def.h,-
	cp$inc:platform_io.h,-
	cp$src:ctype.h,-
	cp$src:if_tureg.h,-
	cp$inc:kernel_entry.h,-
	cp$inc:prototypes.h,-

ei_driver_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:msg_def.h,-
	cp$src:prdef.h,-
	cp$src:dynamic_def.h,-
	cp$src:pb_def.h,-
	cp$src:ni_dl_def.h,-
	cp$src:ni_env.h,-
	cp$src:ev_def.h,-
	cp$src:mop_counters64.h,-
	cp$src:i82558.h,-
	cp$src:i82558_pb_def.h,-
	cp$src:probe_io_def.h,-
	cp$inc:platform_io.h,-
	cp$src:ctype.h,-
	cp$inc:kernel_entry.h,-
	cp$inc:prototypes.h,-

aic78xx_driver_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:ev_def.h,-
	cp$src:msg_def.h,-
	cp$src:ub_def.h,-
	cp$src:sb_def.h,-
	cp$src:pb_def.h,-
	cp$src:scsi_def.h,-
	cp$src:aic78xx_def.h,-
	cp$src:aic78xx_chim.h,-
	cp$src:aic78xx_scsi.h,-
	cp$src:aic78xx_pb_def.h,-
	cp$src:aic78xx_him_xlm.h,-
	cp$src:aic78xx_driver.h,-
	cp$inc:platform_io.h,-
	cp$inc:prototypes.h,-

aic78xx_him_hwm_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:pb_def.h,-
	cp$src:aic78xx_def.h,-
	cp$src:aic78xx_chim.h,-
	cp$src:aic78xx_scsi.h,-
	cp$src:aic78xx_pb_def.h,-
	cp$src:aic78xx_him_hwm.h,-
	cp$src:aic78xx_him_xlm.h,-
	cp$src:aic78xx_driver.h,-
	cp$inc:prototypes.h,-

aic78xx_him_rsm_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:pb_def.h,-
	cp$src:aic78xx_def.h,-
	cp$src:aic78xx_chim.h,-
	cp$src:aic78xx_scsi.h,-
	cp$src:aic78xx_pb_def.h,-
	cp$src:aic78xx_him_hwm.h,-
	cp$src:aic78xx_him_rsm.h,-
	cp$src:aic78xx_him_xlm.h,-
	cp$src:aic78xx_driver.h,-
	cp$inc:prototypes.h,-

aic78xx_him_xlm_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:pb_def.h,-
	cp$src:aic78xx_def.h,-
	cp$src:aic78xx_chim.h,-
	cp$src:aic78xx_scsi.h,-
	cp$src:aic78xx_pb_def.h,-
	cp$src:aic78xx_him_hwm.h,-
	cp$src:aic78xx_him_rsm.h,-
	cp$src:aic78xx_him_xlm.h,-
	cp$src:aic78xx_driver.h,-
	cp$inc:prototypes.h,-

lfu_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:parser.h,-
	cp$src:mem_def.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:msg_def.h,-
	cp$src:ctype.h,-
	cp$src:parse_def.h,-
	cp$src:ub_def.h,-
	cp$src:ev_def.h,-

lfu_support_apc_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:mem_def.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:msg_def.h,-
	cp$inc:kernel_entry.h,-
	cp$src:pb_def.h,-
	cp$src:probe_io_def.h,-
	cp$src:ctype.h,-
	cp$inc:platform_io.h,-
	cp$src:eisa.h,-
	cp$src:romhead.h,-

lfu_lx164_driver_depend	= -
	cp$inc:platform_io.h,-
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:stddef.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:msg_def.h,-
	cp$src:pal_def.h,-
	cp$src:impure_def.h,-
	cp$src:romhead.h,-
	cp$src:ebflash_def.h,-
	cp$src:from_def.h,-

lfu_kzpsa_driver_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:mem_def.h,-
	cp$src:msg_def.h,-
	cp$src:ctype.h,-
	cp$inc:platform_io.h,-
	cp$inc:kernel_entry.h,-
	cp$src:pb_def.h,-
	cp$src:pks_flash_def.h,-
	cp$src:nport_q_def.h,-
	cp$src:nport_def.h,-
	cp$inc:prototypes.h,-

lfu_defpa_defea_driver_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:mem_def.h,-
	cp$src:msg_def.h,-
	cp$inc:kernel_entry.h,-
	cp$inc:platform_io.h,-
	cp$src:pb_def.h,-
	cp$src:ctype.h,-
	cp$src:probe_io_def.h,-
	cp$src:nport_q_def.h,-
	cp$src:nport_def.h,-
	cp$inc:prototypes.h,-

iso9660_driver_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:alpha_arc.h,-
	cp$src:prototypes.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:regexp_def.h,-
	cp$src:msg_def.h,-
	cp$src:f11_cd_def.h,-

lfu_startup_driver_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:msg_def.h,-

