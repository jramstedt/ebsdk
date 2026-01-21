#
# platform_lx164.make - Makefile for the AlphaPC 164/LX
#

include platform_support.make

lc_platform = $(call LC,$(platform))
lc_config = $(call LC,$(config))

$(lc_platform)$(opt), $(lc_config)$(opt) : do_x86 $(EXE)pal_ev5_$(lc_platform).exe $(EXE)osfpal_ev5_$(lc_platform).exe $(EXE)$(lc_config)$(opt).exe $(EXE)ev5_huf_decom.exe
	$(info common $^ -> $@)

lxupdate$(opt) : do_x86 $(EXE)pal_ev5_$(lc_platform).exe $(EXE)$(lc_config)$(opt).exe $(EXE)ev5_huf_decom.exe lx164fsb.rom $(EXE)lx164srm.rom
	$(info exact $^ -> $@)

$(EXE)pal_ev5_$(lc_platform).exe : ev5_$(lc_platform).m64 ev5_vms_pal.m64 ev5_vms_callpal.m64 ev5_vms_$(lc_platform)_pal.m64 ev5_osf_pal.m64 ev5_osf_$(lc_platform)_pal.m64 $(pal_library_files)
	$(info $^ -> $@)

# Config
$(EXE)$(lc_config)$(opt).exe : $(objects) $(OBJ)stubs.olb $(OBJ)x86.olb
	$(info $^ -> $@)
