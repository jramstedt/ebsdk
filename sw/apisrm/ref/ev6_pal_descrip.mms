!
! file:	ev6_pal_descrip.mms 
!
! Abstract:
!	This mms file maintains the build procedure for EV6 PALcode from
!	the common console library.  This routine assumes that
!	cp$cfg:macros.mms defines the $(platform) MMSmacro, and that a 
!	corresponding platform name macro is also defined 
!	(e.g. platform = TURBO, TURBO = 1).  Cp$src:setup.mms is also 
!	used to define usage for HAL (Hudson Assembler Linker).
!
! Invocation: 
!
!	This file supports use of the command line macros 'pal_type' and 
!	'noosf'.  The 'pal_type' macro specifies the name of the prefix
!	file that is to be used, where a '.mar' file extension is assumed
!	(e.g. EV6_WILDFIRE.MAR).  All .exe and .lis files created by
!	this procedure include the 'pal_type' in their filenames
!	(e.g. PAL_EV6_WILDFIRE.EXE).  If no 'pal_type' macro is specified,
!	the value defaults to 'EV6_GENERIC'.
!
!	The 'noosf' macro allows the user to control whether OSF PALcode
!	is built in addition to VMS PALcode.  By default, both VMS and OSF 
!	PALcode are built (as two distinct executable images).  When the 
!	'noosf' macro is defined, only VMS PALcode is built.
!
!	The following examples show the syntax for various invocations of
!	this procedure:
!
!	mms/desc=ev6_pal_descrip.mms pal
!		Builds PAL_EV6_GENERIC.EXE and OSFPAL_EV6_GENERIC.EXE
!
!	mms/desc=ev6_pal_descrip.mms/macro=(pal_type=ev6_wildfire) pal
!		Builds PAL_EV6_WILDFIRE.EXE and OSFPAL_EV6_WILDFIRE.EXE
!
!	mms/desc=ev6_pal_descrip.mms/macro=(pal_type=ev6_wildfire,noosf=1) pal
!		Builds PAL_EV6_WILDFIRE.EXE
!
! History:
!
! 30-May-1996	DTM	Initial, from EV5_PAL_DESCRIP.MMS.  No EV6 OSF
!			PALcode available from HLO yet.
!
.suffixes
.suffixes	.exe .obj .req .mar .sdl

.include	cp$cfg:macros.mms
.include	cp$src:setup.mms

.ifdef pal_type
.ifdef system
.else
system = $(platform)
.endif
.else
pal_type = EV6_GENERIC
system = SYSTEM
generic_lib = 1
.endif

!
! Wildfire specific library files
!
.ifdef wildfire
pal_library_files = -
	cp$src:ev6_defs.mar,-
	cp$src:ev6_redefine_defs.mar,-
	cp$src:ev6_pal_temps.mar,-
	cp$src:ev6_alpha_defs.mar,-
	cp$src:ev6_vms_pal_defs.mar,-
	cp$src:ev6_osf_pal_defs.mar,-
	cp$src:ev6_pal_macros.mar,-
	cp$src:pal_def.mar,-
	cp$src:impure_def.mar

cp$src:pal_def.mar :	cp$src:ev6_wildfire_pal_def.sdl
	$(sdl)/lang=macro=cp$src:pal_def cp$src:ev6_wildfire_pal_def.sdl

cp$src:impure_def.mar :	cp$src:ev6_wildfire_impure_def.sdl
	$(sdl)/lang=macro=cp$src:impure_def cp$src:ev6_wildfire_impure_def.sdl
.endif

!
! Turbo specific library files
!
.ifdef turbo
pal_library_files = -
	cp$src:ev6_defs.mar,-
	cp$src:ev6_redefine_defs.mar,-
	cp$src:ev6_pal_temps.mar,-
	cp$src:ev6_alpha_defs.mar,-
	cp$src:ev6_vms_pal_defs.mar,-
	cp$src:ev6_osf_pal_defs.mar,-
	cp$src:ev6_pal_macros.mar,-
	cp$src:impure_def.mar,-
	cp$src:pal_def.mar,-
	cp$src:pal_tlep.mar,-
	cp$src:pal_tlsb.mar
cp$src:pal_def.mar :	cp$src:ev6_tl6_pal_def.sdl
	$(sdl)/lang=macro=cp$src:pal_def cp$src:ev6_tl6_pal_def.sdl

cp$src:impure_def.mar :	cp$src:ev6_tl6_impure_def.sdl
	$(sdl)/lang=macro=cp$src:impure_def cp$src:ev6_tl6_impure_def.sdl
.endif

!
! Regatta library files
! 
.ifdef regatta
pal_library_files = -
	cp$src:ev6_defs.mar,-
	cp$src:ev6_redefine_defs.mar,-
	cp$src:ev6_pal_temps.mar,-
	cp$src:ev6_alpha_defs.mar,-
	cp$src:ev6_vms_pal_defs.mar,-
	cp$src:ev6_osf_pal_defs.mar,-
	cp$src:ev6_pal_macros.mar,-
	cp$src:pal_def.mar,-
	cp$src:impure_def.mar,-
	cp$src:clipper_dpr_def.mar
.endif
!
! PC/DP264 library files
! 
.ifdef pc264
pal_library_files = -
	cp$src:ev6_defs.mar,-
	cp$src:ev6_redefine_defs.mar,-
	cp$src:ev6_pal_temps.mar,-
	cp$src:ev6_alpha_defs.mar,-
	cp$src:ev6_vms_pal_defs.mar,-
	cp$src:ev6_osf_pal_defs.mar,-
	cp$src:ev6_pal_macros.mar,-
	cp$src:ev6_pc264_pal_macros.mar,-
	cp$src:pal_def.mar,-
	cp$src:impure_def.mar

cp$src:pal_def.mar :	cp$src:ev6_pc264_pal_defs.sdl
	$(sdl)/lang=macro=cp$src:pal_def cp$src:ev6_pc264_pal_def.sdl
                                                         
cp$src:impure_def.mar :	cp$src:ev6_pc264_pal_impure.sdl
       	$(sdl)/lang=macro=cp$src:impure_def cp$src:ev6_pc264_pal_impure.sdl
.endif                                                       
                                                             
!      
! generic library files
!      
.ifdef generic_lib
pal_library_files = -
	cp$src:ev6_defs.mar,-
	cp$src:ev6_redefine_defs.mar,-
	cp$src:ev6_pal_temps.mar,-
	cp$src:ev6_pal_impure.mar,-
	cp$src:ev6_alpha_defs.mar,-
	cp$src:ev6_vms_pal_defs.mar,-
	cp$src:ev6_osf_pal_defs.mar,-
	cp$src:ev6_pal_macros.mar
.endif

.first
	@ show time
	@ write sys$output "Building $(pal_type) variant of $(system) PALcode"
	@cp$src:console_tools

pal_exes = cp$exe:pal_$(pal_type).exe,cp$exe:osfpal_$(pal_type).exe

first_target : 
	@ write sys$output "You have to explicitly specify the PAL target"

pal : $(pal_exes)
	@ continue

.sdl.mar
	sdl/lang=mac=cp$src $(mms$source)

!
! Generic build procedure for VMS PALcode
!
cp$exe:pal_$(pal_type).exe : cp$src:$(pal_type).mar,-
				$(pal_library_files),-
				cp$src:ev6_vms_pal.mar,-
				cp$src:ev6_vms_callpal.mar,-
				cp$src:ev6_vms_$(system)_pal.mar
	$(hal) -h EV6 -e ".SHOW MEB" -
		-o cp$exe:pal_$(pal_type).exe-
		-l cp$lis:pal_$(pal_type).lis-
		cp$src:$(pal_type).mar-
		cp$src:ev6_vms_pal.mar-
		cp$src:ev6_vms_callpal.mar-
		cp$src:ev6_vms_$(system)_pal.mar
	$set file/att=(rfm:fix,lrl:512,mrs:512,rat:none) cp$exe:pal_$(pal_type).exe
	$(mapcvt) cp$lis:pal_$(pal_type).lis cp$exe:pal_$(pal_type).map
!
! Generic build procedure for OSF PALcode
!
cp$exe:osfpal_$(pal_type).exe : cp$src:$(pal_type).mar,-
				$(pal_library_files),-
				cp$src:ev6_osf_pal.mar,-
				cp$src:ev6_osf_$(system)_pal.mar
	$(hal) -h EV6 -e ".SHOW MEB" -e "osfpal = 1" -
		-o cp$exe:osfpal_$(pal_type).exe-
		-l cp$lis:osfpal_$(pal_type).lis-
		cp$src:$(pal_type).mar-
		cp$src:ev6_osf_pal.mar-
		cp$src:ev6_osf_$(system)_pal.mar
	$set file/att=(rfm:fix,lrl:512,mrs:512,rat:none) cp$exe:osfpal_$(pal_type).exe
	$(mapcvt) cp$lis:osfpal_$(pal_type).lis cp$exe:osfpal_$(pal_type).map

