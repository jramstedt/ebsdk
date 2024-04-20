# platform_pc264.mms
#                    
#       
# - egg 15-Jul-1996 -- Initial implementation taken from platform_eb164.mms
#       
.include $(src)platform_support.mms                        
                                                           
$(platform)$(opt), $(config)$(opt) : do_x86, cp$exe:pal_ev6_$(platform).exe,-
	cp$exe:osfpal_ev6_$(platform).exe,-
	cp$exe:$(config)$(opt).exe,-                       
	cp$exe:ev6_huf_decom.exe                           
	!                                                  
	! Generate a list of where all the psects in the image live.  This list is
	! fed to the compressor, which saves us an addition 4-5 percentage points.
	!                                                  
	gawk -f cp$src:psect.awk cp$exe:$(config)$(opt).map >cp$src:psects$(opt).lis
	exename = f$edit(f$getjpi(0,"username"),"collapse")
	show symb exename                                  
	! combine VMS and OSF PALcode                      
	set file/att=(rfm=fix,rat=none,mrs:512,lrl:512)-
		cp$exe:pal_ev6_$(platform).exe,-
		cp$exe:osfpal_ev6_$(platform).exe

	copy cp$exe:pal_ev6_$(platform).exe,cp$exe:osfpal_ev6_$(platform).exe -
		cp$exe:combined_pal_$(platform).exe       

	! combine pal code(s) with console image 
	copy cp$exe:combined_pal_$(platform).exe,cp$exe:$(config)$(opt).exe -
		cp$exe:$(config)_ev6$(opt).exe

	mcr cp$kits:[huffman]hfcomp -
	    -prepend cp$exe:ev6_huf_decom.exe -
	    -target 8000 -      
	    cp$exe:$(config)_ev6$(opt).exe -
	    cp$exe:$(config)_ev6$(opt).sys    
	set prot=w:R cp$exe:$(config)_ev6$(opt).sys                  

        @cp$cmd:getversion cp$src:version.c
	mcr cp$kits:[rom_header]rom_header -
	    -verbose -rev "''cp$version'" -srm -target 900000 -roffset 60000 -
	    cp$exe:$(config)_ev6$(opt).sys -
	    cp$exe:$(config)_ev6$(opt).rom

        mcr cp$kits:[add_header]add_header -
	    cp$exe:$(config)_ev6$(opt).sys -
            cp$exe:grsrmrom.sys


dpupdate$(opt) : do_x86, cp$exe:pal_ev6_$(platform).exe,-
	cp$exe:$(config)$(opt).exe,-
	cp$exe:ev6_huf_decom.exe,-
	cp$exe:pc264_ev6.rom, -
	cp$src:pc264nt.rom,-
	cp$src:pc264fsb.rom
	!
	! Generate a list of where all the psects in the image live.  This list is
	! fed to the compressor, which saves us an addition 4-5 percentage points.
	!
	gawk -f cp$src:psect.awk cp$exe:$(config)$(opt).map >cp$src:psects$(opt).lis
	exename = f$edit(f$getjpi(0,"username"),"collapse")
	show symb exename
	! combine VMS and OSF PALcode
	copy cp$exe:pal_ev6_$(platform).exe,cp$exe:osfpal_ev6_$(platform).exe -
		cp$exe:combined_pal_$(platform).exe       
	! combine pal code(s) with console image 
	copy cp$exe:combined_pal_$(platform).exe,cp$exe:$(config)$(opt).exe -
		cp$exe:$(config)_ev6$(opt).exe
	! combine PALcode/console image with rom image(s)
	copy cp$exe:$(config)_ev6$(opt).exe,cp$exe:pc264_ev6.rom,cp$src:pc264nt.rom,cp$src:pc264fsb.rom -
		cp$exe:$(config)_ev6$(opt)_upd.exe
	mcr cp$kits:[huffman]hfcomp -
	    -prepend cp$exe:ev6_huf_decom.exe -
	    -target 8000 -      
	    cp$exe:$(config)_ev6$(opt)_upd.exe -
	    cp$exe:$(config)_ev6$(opt).sys    

        mcr cp$kits:[add_header]add_header -
	    cp$exe:$(config)_ev6$(opt).sys -
            cp$exe:grupdate.sys

pal_library_files = -                                      
       	cp$src:ev6_defs.mar,-
       	cp$src:ev6_redefine_defs.mar,-
       	cp$src:ev6_pal_temps.mar,-
       	cp$src:ev6_alpha_defs.mar,-
	cp$src:ev6_vms_pal_defs.mar,-
	cp$src:ev6_osf_pal_defs.mar,-
	cp$src:ev6_pal_macros.mar,-
	cp$src:ev6_$(platform)_pal_macros.mar,-
        cp$src:pal_def.mar,-
       	cp$src:impure_def.mar
                                                           
cp$exe:pal_ev6_$(platform).exe : $(pal_library_files),- 
				cp$src:ev6_$(platform).mar,-
                                cp$src:ev6_vms_pal.mar,-    
                                cp$src:ev6_vms_callpal.mar,-
                                cp$src:ev6_vms_$(platform)_pal.mar,-
				cp$src:ev6_osf_pal.mar,-
				cp$src:ev6_osf_$(platform)_pal.mar
	mms := mms        
	mms /descrip=cp$src:ev6_pal_descrip.mms-
	/macro=("pal_type=EV6_$(platform)","system=$(platform)")  pal

cp$exe:$(config)$(opt).exe      : $(objects),cp$obj:stubs.olb, cp$obj:x86.olb
	@cp$cmd:setversion                              
	$(cc)/obj=cp$obj cp$src:version
	open/write output cp$src:link_opts.opt
	close output
	$(link)-
		/map=cp$exe:$(config)$(opt).map-
		/exe=cp$exe:$(config)$(opt).exe-
		/sym=cp$exe:$(config)$(opt).stb-
		/full- 
       		/system=%x20000-
		$(cfg)options$(opt)/opt,-
		cp$src:link_opts/opt,-
		cp$obj:version,-
		cp$obj:x86.olb/lib,-                                                                            
		cp$obj:stubs/lib+-
		$(ots)/lib/nosyslib

cp$src:pal_def.mar :	cp$src:ev6_$(platform)_pal_defs.sdl
	$(sdl)/symbols=(ev6_p1=0)/lang=macro=cp$src:pal_def cp$src:ev6_$(platform)_pal_defs.sdl

cp$src:pal_def.h : cp$src:ev6_$(platform)_pal_defs.sdl
 	$(sdl)/symbols=(ev6_p1=0)/lang=c=cp$lis:__temp__ cp$src:ev6_$(platform)_pal_defs.sdl
        search cp$lis:__temp__.h "#pragma","member_alignment"/match=nand/out=$(mms$target)
        delete cp$lis:__temp__.h;*
                                                                                                             
cp$src:impure_def.mar :	cp$src:ev6_$(platform)_pal_impure.sdl
	$(sdl)/symbols=(ev6_p1=0)/lang=macro=cp$src:impure_def cp$src:ev6_$(platform)_pal_impure.sdl

cp$src:impure_def.h :	cp$src:ev6_$(platform)_pal_impure.sdl                         
 	$(sdl)/symbols=(ev6_p1=0)/lang=c=cp$lis:__temp__ cp$src:ev6_$(platform)_pal_impure.sdl
        search cp$lis:__temp__.h "#pragma","member_alignment"/match=nand/out=$(mms$target)
        delete cp$lis:__temp__.h;*
              
cp$obj:apu_test_end.obj	: cp$src:apu_test_end.mar
	macro/alpha/lis=cp$lis cp$src:apu_test_end.mar/obj=cp$obj 
               
cp$obj:$(platform)_util.obj	: cp$src:$(platform)_util.mar,-
				  cp$src:alphamac.mlb
	$(macro64) -                
	       	cp$src:$(platform)_util.mar+alphamac.mlb/lib/lis=cp$lis/obj=cp$obj
