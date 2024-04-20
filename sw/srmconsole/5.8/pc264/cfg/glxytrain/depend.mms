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

build_fru_depend	= -
	cp$src:ansi_def.h,-
	cp$src:prdef.h,-
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:common.h,-
	cp$src:pb_def.h,-
	cp$src:probe_io_def.h,-
	cp$inc:prototypes.h,-
	cp$inc:platform_io.h,-
	cp$src:ctype.h,-
	cp$src:msg_def.h,-
	cp$src:qscan_def.h,-
	cp$src:ev_def.h,-
	cp$src:platform_fru.h,-
	cp$src:gct.h,-
	cp$src:gct_tree_init.h,-
	cp$src:gct_routines.h,-
	cp$src:gct_util.h,-
	cp$src:gct_nodeid.h,-
	cp$src:gct_tree.h,-
	cp$src:hwrpb_def.h,-
	cp$src:jedec_def.h,-
	cp$src:pal_def.h,-

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

date_cmd_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:parse_def.h,-
	cp$src:msg_def.h,-
	cp$src:time.h,-
	cp$inc:kernel_entry.h,-
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
	cp$src:nt_types.h,-

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

isp1020_edit_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:prototypes.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:parse_def.h,-
	cp$src:msg_def.h,-
	cp$src:pb_def.h,-
	cp$src:isp1020_def.h,-

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
	cp$src:nt_types.h,-
	cp$src:api_debug.h,-
	cp$src:wwid.h,-

process_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:prdef.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:msg_def.h,-

powerup_depend	= -
	cp$src:platform.h,-
	cp$src:prdef.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:stddef.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:msg_def.h,-
	cp$src:tt_def.h,-
	cp$src:ev_def.h,-
	cp$src:hwrpb_def.h,-
	cp$src:pal_def.h,-
	cp$src:mem_def.h,-
	cp$src:impure_def.h,-
	cp$src:alphascb_def.h,-
	cp$inc:kernel_entry.h,-
	cp$src:platform_cpu.h,-
	cp$src:pb_def.h,-
	cp$src:arc_scb_def.h,-
	cp$src:sable_ocp.h,-

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

run_cmd_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:parse_def.h,-
	cp$src:msg_def.h,-
	cp$inc:kernel_entry.h,-
	cp$inc:prototypes.h,-

runbios_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:parse_def.h,-
	cp$src:msg_def.h,-
	cp$src:tt_def.h,-
	cp$src:pb_def.h,-
	cp$src:vgapb.h,-
	cp$src:probe_io_def.h,-
	cp$inc:kernel_entry.h,-
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

scs_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:msg_def.h,-
	cp$src:stddef.h,-
	cp$src:scs_def.h,-
	cp$src:sb_def.h,-
	cp$src:cb_def.h,-
	cp$src:version.h,-
	cp$inc:prototypes.h,-

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
	cp$src:api_debug.h,-
	cp$inc:prototypes.h,-

tr_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:msg_def.h,-

uptime_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-

xlate_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:kbd.h,-

nautilus_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:pb_def.h,-
	cp$src:pal_def.h,-
	cp$src:common.h,-
	cp$src:ctype.h,-
	cp$src:hwrpb_def.h,-
	cp$src:alphascb_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:msg_def.h,-
	cp$src:ev_def.h,-
	cp$inc:prototypes.h,-
	cp$src:arc_scb_def.h,-
	cp$src:platform_cpu.h,-
	cp$inc:platform_io.h,-
	cp$src:isacfg.h,-
	cp$src:romhead.h,-
	cp$src:impure_def.h,-
	cp$src:impure_def.h,-
	cp$src:iic_devices.h,-
	cp$src:m1543c_SBridge.h,-
	cp$src:m1543c_def.h,-
	cp$src:nt_types.h,-

nautilus_init_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:pb_def.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:msg_def.h,-
	cp$src:hwrpb_def.h,-
	cp$src:ev_def.h,-
	cp$src:alpha_def.h,-
	cp$src:aprdef.h,-
	cp$src:alphascb_def.h,-
	cp$src:parse_def.h,-
	cp$src:pal_def.h,-
	cp$src:tt_def.h,-
	cp$src:probe_io_def.h,-
	cp$src:combo_def.h,-
	cp$src:platform_cpu.h,-
	cp$inc:platform_io.h,-
	cp$src:apc.h,-
	cp$src:tsunami.h,-
	cp$src:m1543c_def.h,-
	cp$src:impure_def.h,-
	cp$src:nt_types.h,-

irongate_io_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:pb_def.h,-
	cp$src:probe_io_def.h,-
	cp$src:common.h,-
	cp$src:msg_def.h,-
	cp$inc:prototypes.h,-
	cp$src:platform_cpu.h,-
	cp$inc:platform_io.h,-
	cp$src:irongate.h,-

nautilus_io_depend	= -
	cp$src:platform.h,-
	cp$inc:platform_io.h,-
	cp$src:stddef.h,-
	cp$src:kernel_def.h,-
	cp$src:pb_def.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:probe_io_def.h,-
	cp$src:common_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:eisa_def.h,-
	cp$src:ctype.h,-
	cp$src:platform_cpu.h,-
	cp$src:m1543c_sbridge.h,-
	cp$src:m1543c_def.h,-
	cp$src:pic8259.h,-
	cp$src:nt_types.h,-

nautilus_mcheck_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:pal_def.h,-
	cp$src:common.h,-
	cp$src:hwrpb_def.h,-
	cp$src:alphascb_def.h,-
	cp$src:msg_def.h,-
	cp$inc:prototypes.h,-
	cp$src:ev6_pc264_logout_def.h,-
	cp$src:platform_cpu.h,-
	cp$inc:platform_io.h,-
	cp$src:irongate.h,-

nautilus_nbridge_depend	= -
	cp$src:platform.h,-
	cp$inc:platform_io.h,-
	cp$src:kernel_def.h,-
	cp$src:common_def.h,-
	cp$src:pb_def.h,-
	cp$src:ev_def.h,-
	cp$src:common.h,-
	cp$src:nautilus_nbridge.h,-
	cp$src:m1543c_sbridge.h,-
	cp$src:irongate.h,-
	cp$src:nt_types.h,-

memconfig_nautilus_depend	= -
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
	cp$src:irongate.h,-
	cp$src:memory_chunk.h,-

show_config_nautilus_depend	= -
	cp$src:platform.h,-
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
	cp$src:platform_cpu.h,-
	cp$src:irongate.h,-

amdflash_support_depend	= -
	cp$src:platform.h,-
	cp$inc:platform_io.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:stddef.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:msg_def.h,-
	cp$src:amdflash_def.h,-
	cp$src:romhead.h,-

m1543c_sbridge_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:platform_cpu.h,-
	cp$src:pb_def.h,-
	cp$src:common.h,-
	cp$src:isacfg.h,-
	cp$src:parse_def.h,-
	cp$src:m1543c_sbridge.h,-
	cp$src:eisa_def.h,-
	cp$src:nt_types.h,-
	cp$src:m1543c_def.h,-
	cp$src:combo_def.h,-
	cp$src:iic_devices.h,-
	cp$inc:platform_io.h,-

m1543c_iic_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:prdef.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:ansi_def.h,-
	cp$src:stddef.h,-
	cp$src:srom_def.h,-
	cp$src:msg_def.h,-
	cp$inc:platform_io.h,-
	cp$inc:kernel_entry.h,-
	cp$inc:prototypes.h,-
	cp$src:ev_def.h,-
	cp$src:m1543c_SBridge.h,-
	cp$src:Nt_Types.h,-
	cp$src:m1543c_def.h,-

m5229_ide_depend	= -
	cp$src:platform.h,-
	cp$inc:platform_io.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:kernel_def.h,-
	cp$src:msg_def.h,-
	cp$src:m1543c_def.h,-
	cp$src:pb_def.h,-
	cp$src:nt_types.h,-
	cp$src:api_debug.h,-

api_debug_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:common_def.h,-
	cp$src:parse_def.h,-
	cp$src:kernel_def.h,-
	cp$src:ev_def.h,-
	cp$src:pb_def.h,-
	cp$src:api_debug.h,-

nautilus_info_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:alpha_def.h,-
	cp$src:kernel_def.h,-
	cp$src:pb_def.h,-
	cp$src:sb_def.h,-
	cp$src:ub_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:stddef.h,-
	cp$src:msg_def.h,-
	cp$src:ev_def.h,-
	cp$src:mem_def.h,-
	cp$src:hwrpb_def.h,-
	cp$src:pal_def.h,-
	cp$src:impure_def.h,-
	cp$src:alphascb_def.h,-
	cp$src:ctype.h,-
	cp$src:gct.h,-
	cp$src:platform_cpu.h,-
	cp$inc:platform_io.h,-
	cp$src:tsunami.h,-
	cp$src:tt_def.h,-
	cp$src:sym_def.h,-
	cp$src:aprdef.h,-

dp264_fru_depend	= -
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:msg_def.h,-
	cp$src:gct.h,-
	cp$src:gct_tree.h,-
	cp$src:gct_nodeid.h,-
	cp$src:platform_fru.h,-
	cp$src:iic_def.h,-
	cp$src:jedec_def.h,-

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

element_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:parse_def.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:msg_def.h,-

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

exer_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:parse_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:ctype.h,-
	cp$src:stddef.h,-
	cp$src:common.h,-
	cp$src:msg_def.h,-
	cp$src:diag_def.h,-
	cp$src:leds_def.h,-
	cp$src:ev_def.h,-
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

galaxy_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:ctype.h,-
	cp$src:parse_def.h,-
	cp$src:msg_def.h,-
	cp$inc:kernel_entry.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:console_hal_macros.h,-
	cp$src:ev_def.h,-
	cp$src:impure_def.h,-
	cp$src:pal_def.h,-
	cp$src:hwrpb_def.h,-
	cp$src:memory_chunk.h,-

galaxy_nautilus_depend	= -
	cp$src:platform.h,-
	cp$src:ctype.h,-
	cp$src:parse_def.h,-
	cp$src:msg_def.h,-
	cp$inc:kernel_entry.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:console_hal_macros.h,-
	cp$src:ev_def.h,-
	cp$src:impure_def.h,-
	cp$src:pal_def.h,-
	cp$src:hwrpb_def.h,-
	cp$src:version.h,-
	cp$src:pb_def.h,-
	cp$src:probe_io_def.h,-
	cp$src:platform_fru.h,-
	cp$src:gct.h,-
	cp$src:gct_nodeid.h,-
	cp$src:gct_tree.h,-
	cp$src:gct_tree_init.h,-
	cp$src:galaxy_def_configs.h,-
	cp$src:irongate.h,-
	cp$src:platform_cpu.h,-
	cp$src:jedec_def.h,-
	cp$inc:platform_io.h,-
	cp$src:nautilus_nbridge.h,-
	cp$src:nt_types.h,-

galaxy_config_tree_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:ctype.h,-
	cp$src:parse_def.h,-
	cp$src:msg_def.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:console_hal_macros.h,-
	cp$src:ev_def.h,-
	cp$src:impure_def.h,-
	cp$src:hwrpb_def.h,-
	cp$src:pal_def.h,-
	cp$src:platform_fru.h,-
	cp$src:gct.h,-
	cp$src:gct_tree_init.h,-
	cp$src:gct_routines.h,-
	cp$src:gct_util.h,-
	cp$src:gct_nodeid.h,-
	cp$src:gct_tree.h,-
	cp$src:galaxy_def_configs.h,-
	cp$src:get_console_base.h,-
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

help_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:parser.h,-
	cp$src:regexp_def.h,-
	cp$src:msg_def.h,-
	cp$src:ctype.h,-
	cp$src:tt_def.h,-
	cp$inc:kernel_entry.h,-
	cp$src:help_text.h,-
	cp$inc:prototypes.h,-

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

net_depend	= -
	cp$src:platform.h,-
	cp$src:prdef.h,-
	cp$src:stddef.h,-
	cp$src:kernel_def.h,-
	cp$src:common.h,-
	cp$src:msg_def.h,-
	cp$src:parse_def.h,-
	cp$src:net_def.h,-
	cp$src:ni_env.h,-
	cp$src:ni_dl_def.h,-
	cp$src:mop_def.h,-
	cp$src:mop_counters.h,-
	cp$src:mb_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:ev_def.h,-
	cp$inc:kernel_entry.h,-
	cp$inc:prototypes.h,-

prcache_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:ev_def.h,-
	cp$src:parse_def.h,-
	cp$src:msg_def.h,-
	cp$inc:kernel_entry.h,-
	cp$inc:prototypes.h,-

prcache_pci_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:kernel_def.h,-
	cp$src:msg_def.h,-
	cp$src:pb_def.h,-
	cp$src:pci_nvram_pb_def.h,-
	cp$inc:kernel_entry.h,-

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

memtest_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:msg_def.h,-
	cp$src:parse_def.h,-
	cp$src:diag_groups.h,-
	cp$src:diag_def.h,-
	cp$src:ev_def.h,-
	cp$src:mem_def.h,-
	cp$src:memtest_def.h,-
	cp$inc:kernel_entry.h,-
	cp$inc:prototypes.h,-

memtest_gray_lw_depend	= -
	cp$src:prdef.h,-
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:parse_def.h,-
	cp$src:version.h,-
	cp$src:dynamic_def.h,-
	cp$src:ctype.h,-
	cp$src:stddef.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:diag_def.h,-
	cp$src:diag_groups.h,-
	cp$src:msg_def.h,-
	cp$src:ev_def.h,-
	cp$src:mem_def.h,-
	cp$src:memtest_def.h,-
	cp$inc:kernel_entry.h,-

memtest_march_depend	= -
	cp$src:prdef.h,-
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:parse_def.h,-
	cp$src:version.h,-
	cp$src:dynamic_def.h,-
	cp$src:ctype.h,-
	cp$src:stddef.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:diag_def.h,-
	cp$src:diag_groups.h,-
	cp$src:msg_def.h,-
	cp$src:ev_def.h,-
	cp$src:mem_def.h,-
	cp$src:memtest_def.h,-
	cp$inc:kernel_entry.h,-

memtest_random_depend	= -
	cp$src:prdef.h,-
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:parse_def.h,-
	cp$src:version.h,-
	cp$src:dynamic_def.h,-
	cp$src:ctype.h,-
	cp$src:stddef.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:diag_def.h,-
	cp$src:diag_groups.h,-
	cp$src:msg_def.h,-
	cp$src:ev_def.h,-
	cp$src:mem_def.h,-
	cp$src:memtest_def.h,-
	cp$inc:kernel_entry.h,-

memtest_victim_depend	= -
	cp$src:prdef.h,-
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:parse_def.h,-
	cp$src:version.h,-
	cp$src:dynamic_def.h,-
	cp$src:ctype.h,-
	cp$src:stddef.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:diag_def.h,-
	cp$src:diag_groups.h,-
	cp$src:msg_def.h,-
	cp$src:ev_def.h,-
	cp$src:mem_def.h,-
	cp$src:memtest_def.h,-
	cp$inc:kernel_entry.h,-

nettest_depend	= -
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
	cp$src:diag_def.h,-
	cp$src:diag_groups.h,-
	cp$src:msg_def.h,-
	cp$src:ni_env.h,-
	cp$src:ni_dl_def.h,-
	cp$src:mop_def.h,-
	cp$src:mb_def.h,-
	cp$src:mop_counters.h,-
	cp$src:ev_def.h,-
	cp$inc:kernel_entry.h,-

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

api_iic_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:prdef.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:ansi_def.h,-
	cp$src:stddef.h,-
	cp$src:srom_def.h,-
	cp$src:msg_def.h,-
	cp$inc:platform_io.h,-
	cp$inc:kernel_entry.h,-
	cp$inc:prototypes.h,-
	cp$src:ev_def.h,-
	cp$src:iic_devices.h,-
	cp$src:api_iic.h,-
	cp$src:Nt_Types.h,-
	cp$src:m1543c_def.h,-
	cp$src:M1543c_SBridge.h,-

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
	cp$src:api_debug.h,-

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
	cp$src:platform_cpu.h,-

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

ev6_ipr_driver_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:alpha_def.h,-
	cp$src:alphaps_def.h,-
	cp$src:aprdef.h,-
	cp$src:msgnum.h,-
	cp$src:filesys.h,-
	cp$src:get_console_base.h,-
	cp$src:pal_def.h,-
	cp$src:impure_def.h,-
	cp$src:platform_cpu.h,-
	cp$src:assert.h,-

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

dup_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:ctype.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:msg_def.h,-
	cp$src:dup_def.h,-
	cp$src:sb_def.h,-
	cp$src:cb_def.h,-
	cp$src:ub_def.h,-
	cp$src:print_var.h,-
	cp$inc:prototypes.h,-

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

m1543c_driver_depend	= -
	cp$src:platform.h,-
	cp$inc:platform_io.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:kernel_def.h,-
	cp$src:msg_def.h,-
	cp$src:m1543c_def.h,-

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
	cp$src:nt_types.h,-

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

nport_q_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:nport_q_def.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:msg_def.h,-
	cp$src:print_var.h,-
	cp$inc:prototypes.h,-

cipca_common_depend	= -
	cp$src:platform.h,-
	cp$src:ctype.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:msg_def.h,-
	cp$src:common.h,-
	cp$src:pb_def.h,-
	cp$src:cb_def.h,-
	cp$src:sb_def.h,-
	cp$src:nport_q_def.h,-
	cp$src:nport_def.h,-
	cp$src:cipca_def.h,-
	cp$src:ci_cipca_def.h,-
	cp$src:print_var.h,-
	cp$inc:kernel_entry.h,-
	cp$inc:prototypes.h,-
	cp$src:cipca_debug_rtns.h,-

cipca_response_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:msg_def.h,-
	cp$src:pb_def.h,-
	cp$src:scs_def.h,-
	cp$src:sb_def.h,-
	cp$src:cb_def.h,-
	cp$src:ub_def.h,-
	cp$src:server_def.h,-
	cp$src:mscp_def.h,-
	cp$src:cipca_sa_ppd_def.h,-
	cp$src:print_var.h,-
	cp$src:nport_q_def.h,-
	cp$src:nport_def.h,-
	cp$src:cipca_def.h,-
	cp$src:ci_cipca_def.h,-
	cp$src:cipca_debug_rtns.h,-
	cp$inc:prototypes.h,-
	cp$inc:kernel_entry.h,-

cipca_support_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:msg_def.h,-
	cp$src:pb_def.h,-
	cp$src:nport_q_def.h,-
	cp$src:nport_def.h,-
	cp$src:cipca_def.h,-
	cp$src:ci_cipca_def.h,-
	cp$src:scs_def.h,-
	cp$src:sb_def.h,-
	cp$src:cb_def.h,-
	cp$src:ub_def.h,-
	cp$src:mscp_def.h,-
	cp$src:cipca_sa_ppd_def.h,-
	cp$src:print_var.h,-
	cp$inc:kernel_entry.h,-
	cp$inc:prototypes.h,-

cipca_io_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:ev_def.h,-
	cp$src:mem_def.h,-
	cp$src:msg_def.h,-
	cp$src:pb_def.h,-
	cp$src:sb_def.h,-
	cp$src:nport_q_def.h,-
	cp$src:nport_def.h,-
	cp$src:cipca_def.h,-
	cp$src:ci_cipca_def.h,-
	cp$src:cipca_sa_ppd_def.h,-
	cp$src:server_def.h,-
	cp$src:print_var.h,-
	cp$src:mbx_def.h,-
	cp$inc:platform_io.h,-
	cp$inc:prototypes.h,-
	cp$inc:kernel_entry.h,-

mscp_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:msg_def.h,-
	cp$src:mscp_def.h,-
	cp$src:pb_def.h,-
	cp$src:sb_def.h,-
	cp$src:cb_def.h,-
	cp$src:ub_def.h,-
	cp$src:ctype.h,-
	cp$src:common.h,-
	cp$inc:kernel_entry.h,-
	cp$inc:prototypes.h,-

cipca_driver_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:ev_def.h,-
	cp$src:mem_def.h,-
	cp$src:msg_def.h,-
	cp$src:nport_q_def.h,-
	cp$src:nport_def.h,-
	cp$src:pb_def.h,-
	cp$src:cipca_def.h,-
	cp$src:ci_cipca_def.h,-
	cp$src:cipca_sa_ppd_def.h,-
	cp$src:server_def.h,-
	cp$src:sb_def.h,-
	cp$src:cb_def.h,-
	cp$src:print_var.h,-
	cp$src:ctype.h,-
	cp$src:cipca_debug_rtns.h,-
	cp$inc:platform_io.h,-
	cp$inc:prototypes.h,-
	cp$inc:kernel_entry.h,-
	cp$src:mscp_def.h,-

kgpsa_driver_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:ev_def.h,-
	cp$src:msg_def.h,-
	cp$src:scsi_def.h,-
	cp$src:common.h,-
	cp$src:stddef.h,-
	cp$src:pb_def.h,-
	cp$src:sb_def.h,-
	cp$src:ub_def.h,-
	cp$inc:platform_io.h,-
	cp$src:print_var.h,-
	cp$src:ctype.h,-
	cp$src:wwid.h,-
	cp$src:mem_def.h,-
	cp$src:kgpsa_emx.h,-
	cp$src:fc.h,-
	cp$src:kgpsa_def.h,-
	cp$src:kgpsa_pb_def.h,-
	cp$src:kgpsa_literals.h,-
	cp$inc:kernel_entry.h,-

wwidmgr_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:ev_def.h,-
	cp$src:msg_def.h,-
	cp$src:scsi_def.h,-
	cp$src:common.h,-
	cp$src:stddef.h,-
	cp$src:pb_def.h,-
	cp$src:sb_def.h,-
	cp$src:ub_def.h,-
	cp$inc:platform_io.h,-
	cp$src:print_var.h,-
	cp$src:ctype.h,-
	cp$src:wwid.h,-
	cp$src:mem_def.h,-
	cp$src:kgpsa_emx.h,-
	cp$src:fc.h,-
	cp$src:kgpsa_def.h,-
	cp$src:kgpsa_pb_def.h,-
	cp$src:parse_def.h,-
	cp$inc:kernel_entry.h,-
	cp$src:regexp_def.h,-

pue_driver_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:prototypes.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:msg_def.h,-
	cp$src:ev_def.h,-
	cp$src:n810_def.h,-
	cp$src:pb_def.h,-
	cp$src:n810_pb_def.h,-
	cp$src:dssi_def.h,-
	cp$src:pue_ppd_def.h,-
	cp$src:sb_def.h,-
	cp$src:cb_def.h,-
	cp$src:print_var.h,-
	cp$inc:platform_io.h,-
	cp$src:version.h,-
	cp$inc:kernel_entry.h,-

mc_driver_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:parse_def.h,-
	cp$inc:platform_io.h,-
	cp$src:dynamic_def.h,-
	cp$src:msg_def.h,-
	cp$src:ctype.h,-
	cp$src:pb_def.h,-
	cp$src:pcimc_def.h,-
	cp$inc:kernel_entry.h,-
	cp$inc:prototypes.h,-

i2o_driver_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:ev_def.h,-
	cp$src:msg_def.h,-
	cp$src:common.h,-
	cp$src:print_var.h,-
	cp$src:stddef.h,-
	cp$src:pb_def.h,-
	cp$src:sb_def.h,-
	cp$src:ub_def.h,-
	cp$inc:platform_io.h,-
	cp$inc:kernel_entry.h,-
	cp$src:i2o_pb_def.h,-
	cp$src:i2oadptr.h,-
	cp$src:i2oexec.h,-
	cp$src:i2omsg.h,-
	cp$src:i2o_def.h,-
	cp$inc:prototypes.h,-

i2oexec_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:ev_def.h,-
	cp$src:msg_def.h,-
	cp$src:common.h,-
	cp$src:stddef.h,-
	cp$src:pb_def.h,-
	cp$src:mem_def.h,-
	cp$src:sb_def.h,-
	cp$src:ub_def.h,-
	cp$inc:platform_io.h,-
	cp$src:print_var.h,-
	cp$src:ctype.h,-
	cp$inc:kernel_entry.h,-
	cp$src:i2odep.h,-
	cp$src:i2oexec.h,-
	cp$src:i2o_def.h,-
	cp$src:i2o_pb_def.h,-
	cp$inc:prototypes.h,-

i2omsg_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:ev_def.h,-
	cp$src:msg_def.h,-
	cp$src:common.h,-
	cp$src:stddef.h,-
	cp$src:pb_def.h,-
	cp$src:mem_def.h,-
	cp$src:sb_def.h,-
	cp$src:ub_def.h,-
	cp$inc:platform_io.h,-
	cp$src:print_var.h,-
	cp$src:ctype.h,-
	cp$inc:kernel_entry.h,-
	cp$src:i2o_def.h,-
	cp$src:i2o_pb_def.h,-
	cp$src:i2omsg.h,-
	cp$inc:prototypes.h,-

i2outil_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:ev_def.h,-
	cp$src:msg_def.h,-
	cp$src:common.h,-
	cp$src:stddef.h,-
	cp$src:pb_def.h,-
	cp$src:mem_def.h,-
	cp$src:sb_def.h,-
	cp$src:ub_def.h,-
	cp$inc:platform_io.h,-
	cp$src:print_var.h,-
	cp$src:ctype.h,-
	cp$inc:kernel_entry.h,-
	cp$src:i2odep.h,-
	cp$src:i2o_def.h,-
	cp$src:i2o_pb_def.h,-
	cp$src:i2outil.h,-
	cp$inc:prototypes.h,-

i2o_bsa_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:msg_def.h,-
	cp$inc:kernel_entry.h,-
	cp$src:pb_def.h,-
	cp$src:sb_def.h,-
	cp$src:ub_def.h,-
	cp$inc:prototypes.h,-

i2o_bsa_osm_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:msg_def.h,-
	cp$src:eisa.h,-
	cp$src:scsi_def.h,-
	cp$src:pb_def.h,-
	cp$src:sb_def.h,-
	cp$src:ub_def.h,-
	cp$src:probe_io_def.h,-
	cp$inc:platform_io.h,-
	cp$src:platform_cpu.h,-
	cp$inc:kernel_entry.h,-
	cp$src:print_var.h,-
	cp$src:i2oexec.h,-
	cp$src:i2o_def.h,-
	cp$src:i2o_pb_def.h,-
	cp$src:i2omstor.h,-
	cp$src:i2outil.h,-
	cp$inc:prototypes.h,-

i2o_scsi_osm_depend	= -
	cp$src:platform.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:ev_def.h,-
	cp$src:msg_def.h,-
	cp$src:scsi_def.h,-
	cp$src:common.h,-
	cp$src:stddef.h,-
	cp$src:pb_def.h,-
	cp$src:sb_def.h,-
	cp$src:ub_def.h,-
	cp$inc:platform_io.h,-
	cp$inc:kernel_entry.h,-
	cp$src:i2o_pb_def.h,-
	cp$src:i2obscsi.h,-
	cp$src:i2oadptr.h,-
	cp$src:i2oexec.h,-
	cp$src:i2omsg.h,-
	cp$src:i2o_def.h,-
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

bios_disk_driver_depend	= -
	cp$src:platform.h,-
	cp$src:common.h,-
	cp$src:prototypes.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:msg_def.h,-
	cp$src:eisa.h,-
	cp$src:scsi_def.h,-
	cp$src:pb_def.h,-
	cp$src:sb_def.h,-
	cp$src:ub_def.h,-
	cp$src:probe_io_def.h,-
	cp$inc:platform_io.h,-
	cp$src:platform_cpu.h,-
	cp$src:ev_def.h,-
	cp$inc:kernel_entry.h,-

lfu_nautilus_driver_depend	= -
	cp$src:platform.h,-
	cp$inc:platform_io.h,-
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:stddef.h,-
	cp$src:common.h,-
	cp$inc:prototypes.h,-
	cp$src:msg_def.h,-
	cp$src:pal_def.h,-
	cp$src:impure_def.h,-
	cp$src:romhead.h,-
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

lfu_cipca_driver_depend	= -
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
	cp$src:prototypes.h,-

lfu_mc2_driver_depend	= -
	cp$src:kernel_def.h,-
	cp$src:dynamic_def.h,-
	cp$src:mem_def.h,-
	cp$src:common.h,-
	cp$src:msg_def.h,-
	cp$src:ctype.h,-
	cp$inc:platform_io.h,-
	cp$inc:kernel_entry.h,-
	cp$src:pb_def.h,-

