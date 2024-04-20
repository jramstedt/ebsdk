!
! file:	ev5_pal_descrip.mms 
!
! Abstract:
!	This mms file maintains the build procedure for EV5 PALcode from
!	the common console library.  This routine assumes that newbuild.com
!	and cp_common_login.com have been previously executed.  Specifically,
!	it assumes that cp$cfg:macros.mms defines the $(platform) MMS
!	macro, and that a corresponding platform name macro is also defined
!	(e.g. platform = TURBO, TURBO = 1).  Cp$src:setup.mms is also used
!	to define usage for the MACRO-64 Alpha macro assembler and the
!	Alpha linker.
!
! Invocation: 
!
!	This file supports use of the command line macros 'pal_type' and 
!	'noosf'.  The 'pal_type' macro specifies the name of the prefix
!	file that is to be used, where a '.m64' file extension is assumed
!	(e.g. EV5_TURBO.M64).  All .exe, .obj, and .lis files created by
!	this procedure include the 'pal_type' in their filenames
!	(e.g. PAL_EV5_TURBO.EXE).  If no 'pal_type' macro is specified,
!	the value defaults to 'EV5_GENERIC'.
!
!	The 'noosf' macro allows the user to control whether OSF PALcode
!	is built in addition to VMS PALcode.  By default, both VMS and OSF 
!	PALcode are built (as two distinct executable images).  When the 
!	'noosf' macro is defined, only VMS PALcode is built.
!
!	If the following rules are adhered to, then this procedure does not
!	need to be modified in order to support building PALcode for new
!	platforms:
!
!	1) A new prefix file is created, with filename of the form
!	   'pal_type'.M64.
!	2) The platform-specific PALcode source files are available, with
!	   filenames EV5_VMS_'platform'_PAL.M64 and EV5_OSF_'platform'_PAL.M64.
!	3) The build does not require additional, platform-specific library
!	   or source files.  If additional library files are required, the
!	   macro 'pal_library_files' must be defined conditionally.  If
!	   additional source files must be included in the assembly, an
!	   appropriate platform-specific action line must be added to this 
!	   description file.
!	
!	The following examples show the syntax for various invocations of
!	this procedure:
!
!	mms/desc=ev5_pal_descrip.mms pal
!		Builds PAL_EV5_GENERIC.EXE and OSFPAL_EV5_GENERIC.EXE
!
!	mms/desc=ev5_pal_descrip.mms/macro=(pal_type=ev5_turbo) pal
!		Builds PAL_EV5_TURBO.EXE and OSFPAL_EV5_TURBO.EXE
!
!	mms/desc=ev5_pal_descrip.mms/macro=(pal_type=ev5_turbo,noosf=1) pal
!		Builds PAL_EV5_TURBO.EXE
!
! History:
!
! 15-Jan-1997	ER	Include support for LX164.  Remove SD164.
!
! 10-Jul-1996	JJE	Include support for Yukon
!
! 10-Jul-1996	SM	Include support for TAKARA
!
! 06-Apr-1995	HCB	Add system tag so system doesn't have to equal platform
!
! 10-Mar-1995	RBB	Include support for EB164
!
! 15-Aug-1994	ER	Include support for SD164
!
! 18-Oct-1993	DTM	Initial merge to common console library.  Includes
!			support for ALCOR, GENERIC, TURBO, TSIM. 
!
.suffixes
.suffixes	.exe .obj .req .mar .sdl .m64

.include	cp$cfg:macros.mms
.include	cp$src:setup.mms

.ifdef pal_type
.ifdef system
.else
system = $(platform)
.endif
.else
pal_type = EV5_GENERIC
system = SYSTEM
generic_lib = 1
.endif

!
! Turbo specific library files
!
.ifdef turbo
pal_library_files = -
	cp$src:ev5_paldef.mar,-
	cp$src:ev5_pal_macros.mar,-
	cp$src:ev5_alpha_defs.mar,-
	cp$src:ev5_defs.mar,-
	cp$src:ev5_osfalpha_defs.mar,-
	cp$src:ev5_turbo_impure.mar,-
	cp$src:ev5_turbo_cserve.mar,-
	cp$src:pal_tlsb.mar,-
	cp$src:pal_tlep.mar
.endif

!
! Alcor specific library files
!
.ifdef alcor
pal_library_files = -
	cp$src:ev5_paldef.mar,-
	cp$src:ev5_pal_macros.mar,-
	cp$src:ev5_alpha_defs.mar,-
	cp$src:ev5_defs.mar,-
	cp$src:ev5_alcor_impure.mar,-
	cp$src:ev5_alcor_paldef.mar,-
	cp$src:ev5_osfalpha_defs.mar,-
	cp$src:alcor_srom_mailbox.mar,-
	cp$src:cia_error_def.mar,-
	cp$src:cia_rom_def.mar,-
        cp$src:sport_macros.m64
.endif

!
! Pinnacle specific library files
!
.ifdef pinnacle
.ifdef pal_library_files
.else
pal_library_files = -
	cp$src:ev5_paldef.mar,-
	cp$src:ev5_pal_macros.mar,-
	cp$src:ev5_alpha_defs.mar,-
	cp$src:ev5_defs.mar,-
	cp$src:ev5_pinnacle_impure.mar,-
	cp$src:ev5_pinnacle_paldef.mar,-
	cp$src:ev5_osfalpha_defs.mar,-
	cp$src:alcor_srom_mailbox.mar,-
	cp$src:cia_error_def.mar,-
        cp$src:sport_macros.m64
.endif
.endif

!
! EB164 specific library files
!
.ifdef eb164
pal_library_files = -
	cp$src:ev5_paldef.mar,-
	cp$src:ev5_pal_macros.mar,-
	cp$src:ev5_alpha_defs.mar,-
	cp$src:ev5_defs.mar,-
	cp$src:ev5_eb164_impure.mar,-
	cp$src:ev5_eb164_paldef.mar,-
	cp$src:ev5_osfalpha_defs.mar,-
	cp$src:cia_error_def.mar
.endif

!
! AlphaPC 164/LX specific library files
!
.ifdef lx164
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
.endif

!
! AlphaPC 164/SX specific library files
!
.ifdef sx164
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
	cp$src:pyxis_addr_trans_def.mar,-
	cp$src:pyxis_rom_def.mar
.endif

!
! TAKARA specific library files
!
.ifdef takara
pal_library_files = -
	cp$src:ev5_paldef.mar,-
	cp$src:ev5_pal_macros.mar,-
	cp$src:ev5_alpha_defs.mar,-
	cp$src:ev5_defs.mar,-
	cp$src:ev5_takara_impure.mar,-
	cp$src:ev5_takara_paldef.mar,-
	cp$src:ev5_osfalpha_defs.mar,-
	cp$src:cia_error_def.mar
.endif

!
! Yukon specific library files
!
.ifdef yukon
pal_library_files = -
	cp$src:ev5_paldef.mar,-
	cp$src:ev5_pal_macros.mar,-
	cp$src:ev5_alpha_defs.mar,-
	cp$src:ev5_defs.mar,-
	cp$src:ev5_yukon_impure.mar,-
	cp$src:ev5_yukon_paldef.mar,-
	cp$src:ev5_osfalpha_defs.mar,-
	cp$src:cia_error_def.mar
.endif
!
! Yukon A specific library files
!
.ifdef yukona
pal_library_files = -
	cp$src:ev5_paldef.mar,-
	cp$src:ev5_pal_macros.mar,-
	cp$src:ev5_alpha_defs.mar,-
	cp$src:ev5_defs.mar,-
	cp$src:ev5_yukona_impure.mar,-
	cp$src:ev5_yukona_paldef.mar,-
	cp$src:ev5_osfalpha_defs.mar,-
	cp$src:cia_error_def.mar
.endif

!
! Gamma specific library files
!
.ifdef gamma
pal_library_files = -
	cp$src:ev5_paldef.mar,-
	cp$src:ev5_pal_macros.mar,-
	cp$src:ev5_alpha_defs.mar,-
	cp$src:ev5_defs.mar,-
	cp$src:ev5_osfalpha_defs.mar,-
	cp$src:ev5_gamma_pal_def.mar,-
	cp$src:ev5_gamma_impure_def.mar
.endif

!
! Rawhide specific library files
!
.ifdef rawhide
pal_library_files = -
	cp$src:ev5_paldef.mar,-
	cp$src:ev5_pal_macros.mar,-
	cp$src:ev5_alpha_defs.mar,-
	cp$src:ev5_defs.mar,-
	cp$src:ev5_osfalpha_defs.mar,-
	cp$src:ev5_rawhide_impure.mar,-
	cp$src:ev5_rawhide_cserve.mar,-
	cp$src:pal_iod.mar
.endif

!
! generic library files
!
.ifdef generic_lib
pal_library_files = -
	cp$src:ev5_paldef.mar,-
	cp$src:ev5_pal_macros.mar,-
	cp$src:ev5_alpha_defs.mar,-
	cp$src:ev5_defs.mar,-
	cp$src:ev5_impure.mar,-
	cp$src:ev5_osfalpha_defs.mar
.endif

.first
	@ show time
	@ write sys$output "Building $(pal_type) variant of $(system) PALcode"
	@cp$src:console_tools


.ifdef noOSF 
pal_exes = cp$exe:pal_$(pal_type).exe
.else
pal_exes = cp$exe:pal_$(pal_type).exe,cp$exe:osfpal_$(pal_type).exe
.endif

first_target : 
	@ write sys$output "You have to explicitly specify the PAL target"

pal : $(pal_exes)
	@ continue

.obj.exe
	$(link)/system=%x0/nosyslib -
		$(mms$source)/map=cp$exe:/exe=cp$exe:

.sdl.mar
	sdl/lang=mac=cp$src $(mms$source)

!
!	Macro libraries
!
cp$src:ev5_alphamac.mlb	: $(pal_library_files)
	library/create/macro cp$src:ev5_alphamac -
		$(pal_library_files)+cp$src:starlet

.ifdef turbo
.else
!
! Generic build procedure for VMS PALcode
!
cp$obj:pal_$(pal_type).obj : cp$src:$(pal_type).m64,-
				cp$src:ev5_vms_pal.m64,-
				cp$src:ev5_vms_callpal.m64,-
				cp$src:ev5_vms_$(system)_pal.m64,-
				cp$src:ev5_alphamac.mlb
	$(macro64)/nooptimize -
		cp$src:$(pal_type).m64+-
		cp$src:ev5_vms_pal.m64+-
		cp$src:ev5_vms_callpal.m64+-
		cp$src:ev5_vms_$(system)_pal.m64+-
		cp$src:ev5_alphamac/lib-
		/obj=cp$obj:pal_$(pal_type)-
		/lis=cp$lis:pal_$(pal_type)/machine
.endif
!
! Generic build procedure for OSF PALcode
!
cp$obj:osfpal_$(pal_type).obj : cp$src:$(pal_type).m64,-
				cp$src:ev5_osf_pal.m64,-
				cp$src:ev5_osf_$(system)_pal.m64,-
				cp$src:ev5_alphamac.mlb
	$(macro64)/nooptimize -
		cp$src:$(pal_type).m64+-
		cp$src:ev5_osf_pal.m64+-
		cp$src:ev5_osf_$(system)_pal.m64+-
		cp$src:ev5_alphamac/lib-
		/obj=cp$obj:osfpal_$(pal_type)-
		/lis=cp$lis:osfpal_$(pal_type)/machine

!
! Build procedure for TurboLaser VMS PALcode
!
cp$obj:pal_ev5_tsim.obj,-
cp$obj:pal_ev5_turbo.obj : cp$src:$(pal_type).m64,-
				cp$src:ev5_vms_pal.m64,-
				cp$src:ev5_$(system)_init.m64,-
				cp$src:ev5_vms_callpal.m64,-
				cp$src:ev5_vms_$(system)_pal.m64,-
				cp$src:ev5_alphamac.mlb
	$(macro64)/nooptimize -
		cp$src:$(pal_type).m64+-
		cp$src:ev5_vms_pal.m64+-
		cp$src:ev5_vms_callpal.m64+-
		cp$src:ev5_$(system)_init.m64+-
		cp$src:ev5_vms_$(system)_pal.m64+-
		cp$src:ev5_alphamac/lib-
		/obj=cp$obj:pal_$(pal_type)-
		/lis=cp$lis:pal_$(pal_type)/machine
