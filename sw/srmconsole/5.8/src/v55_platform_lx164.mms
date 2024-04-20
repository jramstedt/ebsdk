#
# platform_lx164.mms - MMS build description file for the AlphaPC 164/LX
#

.include $(src)platform_support.mms

$(platform)$(opt), $(config)$(opt) : do_x86_v3, cp$exe:pal_ev5_$(platform).exe,-
	cp$exe:osfpal_ev5_$(platform).exe,-
	cp$exe:$(config)$(opt).exe,-
	cp$exe:ev5_huf_decom.exe
	!
	! Generate a list of where all the psects in the image live.  This list is
	! fed to the compressor, which saves us an addition 4-5 percentage points.
	!
	gawk -f cp$src:psect.awk cp$exe:$(config).map >cp$src:psects.lis
	!
	! combine VMS and OSF PALcode
	copy cp$exe:pal_ev5_$(platform).exe,cp$exe:osfpal_ev5_$(platform).exe -
		cp$exe:combined_pal_$(platform).exe       

	! combine pal code(s) with console image 
	copy cp$exe:combined_pal_$(platform).exe,cp$exe:$(config).exe -
		cp$exe:$(config)_ev5.exe

	mcr cp$kits:[huffman]hfcomp -
	    -prepend cp$exe:ev5_huf_decom.exe -
	    -target c000 -
	    cp$exe:$(config)_ev5.exe -
	    cp$exe:$(config)_ev5.cmp
	set prot=w:R cp$exe:$(config)_ev5.cmp

        mcr cp$kits:[add_header]add_header -
	    cp$exe:$(config)_ev5.cmp -
            cp$exe:$(config)_ev5.sys

        @cp$cmd:getversion cp$sync:version.c
	mcr cp$kits:[rom_header]rom_header -
	    -verbose -rev "''cp$version'" -srm -target 900000 -roffset 10000 -
	    cp$exe:$(config)_ev5.cmp -
	    cp$exe:$(config)srm.rom

lxupdate$(opt) : do_x86_v3, cp$exe:pal_ev5_$(platform).exe,-
	cp$exe:$(config)$(opt).exe,-
	cp$exe:ev5_huf_decom.exe,-
	cp$src:lx164fsb.rom,-
	cp$exe:lx164srm.rom
	!
	! Generate a list of where all the psects in the image live.  This list is
	! fed to the compressor, which saves us an addition 4-5 percentage points.
	!
	gawk -f cp$src:psect.awk cp$exe:$(config).map >cp$src:psects.lis
	!
	! combine VMS and OSF PALcode
	copy cp$exe:pal_ev5_$(platform).exe,cp$exe:osfpal_ev5_$(platform).exe -
		cp$exe:combined_pal_$(platform).exe

	! combine pal code(s) with console image 
	copy cp$exe:combined_pal_$(platform).exe,cp$exe:$(config).exe -
	    cp$exe:$(config)_ev5.exe

	copy cp$exe:$(config)_ev5.exe,cp$exe:lx164srm.rom,cp$src:lx164fsb.rom -
	    cp$exe:$(config)_ev5_upd.exe

	mcr cp$kits:[huffman]hfcomp -
	    -prepend cp$exe:ev5_huf_decom.exe -
	    -target c000 -
	    cp$exe:$(config)_ev5_upd.exe -
	    cp$exe:$(config)_ev5.cmp
	set prot=w:R cp$exe:$(config)_ev5.cmp

        mcr cp$kits:[add_header]add_header -
	    cp$exe:$(config)_ev5.cmp -
            cp$exe:$(config)_ev5.sys

pal_library_files = -
	cp$src:ev5_paldef.mar,-
	cp$src:ev5_pal_macros.mar,-
	cp$src:ev5_alpha_defs.mar,-
	cp$src:ev5_defs.mar,-
	cp$src:impure_def.mar,-
	cp$src:pal_def.mar,-
	cp$src:ev5_osfalpha_defs.mar,-
	cp$src:pyxis_main_csr_def.mar,-
	cp$src:pyxis_mem_csr_def.mar,-
	cp$src:pyxis_addr_trans_def.mar

cp$exe:pal_ev5_$(platform).exe : cp$src:ev5_$(platform).m64,-
                                cp$src:ev5_vms_pal.m64,-
                                cp$src:ev5_vms_callpal.m64,-
                                cp$src:ev5_vms_$(platform)_pal.m64,-
				cp$src:ev5_osf_pal.m64,-
                                cp$src:ev5_osf_$(platform)_pal.m64,-
                                $(pal_library_files)
	mms := mms
	mms /descrip=cp$src:ev5_pal_descrip.mms-
	/macro=("pal_type=EV5_$(platform)")  pal

cp$exe:$(config)$(opt).exe      : $(objects),cp$obj:stubs.olb, cp$obj:x86_v3.olb
	@cp$src:build_version
	$(cc)/obj=cp$obj cp$sync:version
	open/write output cp$src:link_opts.opt
	write output "symbol=max_proc,1"
	close output
	$(link)-
		/map=cp$exe:$(config).map-
		/exe=cp$exe:$(config).exe-
		/sym=cp$exe:$(config).stb-
		/full-
       		/system=%x20000-
		$(cfg)options$(opt)/opt,-
		cp$src:link_opts/opt,-
		cp$obj:version,-
		cp$obj:x86_v3.olb/lib,-
		cp$obj:stubs/lib,-
		$(ots)/lib/nosyslib

cp$obj:$(platform)_util.obj :	cp$src:$(platform)_util.mar,-
				cp$src:ev5_alphamac.mlb
	$(macro64) -
		cp$src:$(platform)_util.mar/lis=cp$lis/obj=cp$obj -
		+ev5_alphamac.mlb/lib

cp$src:pal_def.mar :	cp$src:ev5_$(platform)_pal_def.sdl
	$(sdl)/lang=macro=cp$src:pal_def cp$src:ev5_$(platform)_pal_def.sdl

cp$src:pal_def.h :	cp$src:ev5_$(platform)_pal_def.sdl
	$(sdl)/lang=cc=cp$src:pal_def cp$src:ev5_$(platform)_pal_def.sdl

cp$src:impure_def.mar :	cp$src:ev5_$(platform)_impure.sdl
	$(sdl)/lang=macro=cp$src:impure_def cp$src:ev5_$(platform)_impure.sdl

cp$src:impure_def.h :	cp$src:ev5_$(platform)_impure.sdl
	$(sdl)/lang=cc=cp$src:impure_def cp$src:ev5_$(platform)_impure.sdl

cp$obj:apu_test_end.obj :	cp$src:apu_test_end.mar
	macro/alpha/lis=cp$lis cp$src:apu_test_end.mar/obj=cp$obj
