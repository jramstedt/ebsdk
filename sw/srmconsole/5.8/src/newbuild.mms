# file: newbuild.mms
#
# +-----------------------------------------------------------------------+
# | Copyright (C) 1993 by						  |
# | Digital Equipment Corporation, Maynard, Massachusetts.		  |
# | All rights reserved.						  |
# |									  |
# | This software is furnished under a license and may be used and copied |
# | only  in  accordance  of  the  terms  of  such  license  and with the |
# | inclusion of the above copyright notice. This software or  any  other |
# | copies thereof may not be provided or otherwise made available to any |
# | other person.  No title to and  ownership of the  software is  hereby |
# | transferred.							  |
# |									  |
# | The information in this software is  subject to change without notice |
# | and  should  not  be  construed  as a commitment by digital equipment |
# | corporation.							  |
# |									  |
# | Digital assumes no responsibility for the use  or  reliability of its |
# | software on equipment which is not supplied by digital.		  |
# +-----------------------------------------------------------------------+
#
#++
# FACILITY:
#
#	Common Console Development Environment
#
# MODULE DESCRIPTION:
#
#	MMS file to control new build setup.  The output of this phase is a set
#	of MMS files and other files to control the build.
#
# Examples:
#
#   Build COBRA
#
#	MMS/DESCRIP=NEWBUILD.MMS/MAC=("platform=cobra","opt=_opt") cobra_opt
#
#  AUTHORS:
#
#	Peter H. Smith, based on A.J. Beaverson's build_generic.awk
#
#  CREATION DATE:
#
#	07-JUN-1993
#
#  MODIFICATION HISTORY:
#
#	phs	21-Jun-1993	Clean up, use srcexp for all source scanners.
#	phs	07-Jun-1993	Initial revision, based on build_generic.awk.
#--

#
# Input parameters
#
#   platform	: Platform to build
#   config	: Configuration to build
#
platform =
config =
#
#   File suffix for optimized files.
#
opt = _opt
#
# Categories of files and their locations
#
#  awk : awk programs
#  cfg : configuration files
#  exe : executable file from last successful build.
#        NOTE: The build of pdlist.c should be changed so that the old .MAP
#	       file doesn't need to be scanned.  Use a mechanism similar to
#	       that used for Quilt/modlist.c
#  inc : include files
#  src : source files
#
awk = cp$src:
cfg = cp$cfg:
exe = cp$exe:
inc = cp$inc:
src = cp$src:
#
# Locations of specific files
#
# buildfile	: Build configuration file
# expfile	: Expansion of include statements in build file
#
buildfile = $(src)$(config)_files.bld
expfile = $(cfg)config.bld

#
# Default behavior
#
all : $(cfg)sources.txt $(cfg)objects.mms -
	$(cfg)options.opt $(cfg)options$(opt).opt -
	$(cfg)objects.txt $(cfg)objects$(opt).txt -
	$(inc)prototypes.h -
	$(inc)kernel_entry.h -
	$(src)platform_cpu.sdl -
	$(inc)platform_io.h -
	$(src)platform_fru.h -
	$(src)__decc_include_prologue.h -
	$(cfg)dst.c $(cfg)encap.c -
	$(cfg)platform.h $(cfg)platform.mar -
	$(cfg)macros.mms -
	$(cfg)builtins.c $(cfg)help_text.h -
	$(cfg)depend.mms $(cfg)msgtxt.c $(cfg)msgnum.h -
	$(cfg)sym_xxclusions.dat -
	$(cfg)pdlist.c
        copy $(src)__decc_include_prologue.h $(src)
	@cp$src:delete_duplicate $(cfg)*.*
	@cp$src:delete_duplicate $(inc)*.*
	@cp$src:delete_duplicate $(src)platform_cpu*.sdl
	@cp$src:delete_duplicate $(src)platform_fru*.h
	@cp$src:delete_duplicate $(src)__decc_include_prologue*.h
	@ write sys$output "newbuild completed"

! This needs work.  Should derive the pdlist from sources, objects, or
! listings, rather than scanning the previous map file.
$(cfg)pdlist.c : ALWAYS
	@ x = f$search("$(exe)$(config).map")
	@ p = x .nes. ""
	@ if p then write sys$output -
		"gawk -f $(awk)build_pdlist ", -
		"''f$search("$(exe)$(config).map")' ", -
		"> $(cfg)pdlist.c ", -
		"by=''f$search("$(awk)build_pdlist.awk")'"
	@ if p then gawk -f $(awk)build_pdlist -
		'f$search("$(exe)$(config).map")' -
		> $(cfg)pdlist.c -
		"by=''f$search("$(awk)build_pdlist.awk")'"
	@ q = .not. p
	@ if q then q = (f$search("$(cfg)pdlist.c") .eqs. "")
	@ if q then write sys$output "Creating null $(cfg)pdlist.c"
	@ if q then open/write out $(cfg)pdlist.c
	@ if q then write out "#include	""$(src)pd_def.h"""
	@ if q then write out ""
	@ if q then write out "struct PDLIST pdlist [] = {"
	@ if q then write out "	{0, """"}"
	@ if q then write out "};"
	@ if q then close out

ALWAYS :
	@ ! no action

$(cfg)msgnum.h : $(cfg)msgtxt.c -
		 $(cfg)msgdef.awk $(awk)isort.awk $(awk)build_msgh.awk
	gawk -f $(cfg)msgdef -f $(awk)isort -f $(awk)build_msgh -
	     'f$search("$(mms$source)")' > $(mms$target)

$(cfg)msgtxt.c : $(cfg)sources.txt -
		 $(awk)srcexp.awk $(awk)isort.awk -
		 $(cfg)msgdef.awk $(awk)build_msg.awk
	gawk -f $(awk)srcexp -f $(awk)isort -f $(cfg)msgdef -f $(awk)build_msg -
	     "exc=^($(cfg)|$(src))(modlist|msgtxt)\.c$" -
 	     'f$search("$(mms$source)")' > $(mms$target)

$(cfg)msgdef.awk : $(expfile) $(awk)bldscn.awk $(awk)bldmsg.awk
	gawk -f $(awk)bldscn -f $(awk)bldmsg -
		'f$search("$(mms$source)")' > $(mms$target)

$(cfg)depend.mms : $(cfg)sources.txt $(awk)srcexp.awk $(awk)build_depend.awk
	gawk -f $(awk)srcexp -f $(awk)build_depend -
	     "exc=^$(cfg)(modlist|msgtxt)\.c$" -
	     "exch=(stdio|platform)\.h$" -
	     'f$search("$(mms$source)")' > $(mms$target)

# Prototype definitions
$(inc)prototypes.h : $(src)prototypes.h
	copy $(src)prototypes.h $(inc)prototypes.h

# Platform specific call between definitions
$(inc)kernel_entry.h : $(src)kernel_entry.h
	copy $(src)kernel_entry.h $(inc)kernel_entry.h

# Platform specific cpu definitions
$(src)platform_cpu.sdl : $(src)$(platform).sdl
        copy $(src)$(platform).sdl $(src)platform_cpu.sdl

# Platform specific io definitions
$(inc)platform_io.h : $(src)$(platform)_io.h
        copy $(src)$(platform)_io.h $(inc)platform_io.h

# Platform specific FRU definitions
$(src)platform_fru.h : $(src)$(platform)_fru.h
        if f$search("$(src)$(platform)_fru.h") .nes. "" then $ copy $(src)$(platform)_fru.h $(src)platform_fru.h
        if f$search("$(src)$(platform)_fru.h") .eqs. "" then $ copy nla0: $(src)platform_fru.h

$(cfg)help_text.h : $(cfg)sources.txt $(awk)srcexp.awk $(awk)build_helptext.awk
	gawk -f $(awk)srcexp -f $(awk)build_helptext -
	     "exc=^$(cfg)(modlist|msgtxt)\.c$" -
	     'f$search("$(mms$source)")' > $(mms$target)

$(cfg)builtins.c : $(cfg)sources.txt $(awk)srcexp.awk $(awk)build_builtins.awk
	gawk -f $(awk)srcexp -f $(awk)build_builtins -
	     "exc=^(($(src)|$(cfg))builtins\.c|$(cfg)(modlist|msgtxt)\.c)$" -
	     'f$search("$(mms$source)")' > $(mms$target)

$(cfg)macros.mms : $(expfile) $(awk)bldscn.awk $(awk)isort.awk $(awk)bldmms.awk
	gawk -f $(awk)bldscn -f $(awk)isort -f $(awk)bldmms -
	     'f$search("$(mms$source)")' > $(mms$target)

# Complete platform header file.  For compatability only...
$(cfg)platform.h : $(expfile) $(awk)bldscn.awk $(awk)isort.awk $(awk)bldplh.awk
	gawk -f $(awk)bldscn -f $(awk)isort -f $(awk)bldplh -
	     'f$search("$(mms$source)")' > $(mms$target) -

# Platform macro header file.  For compatability only...
$(cfg)platform.mar : $(expfile) -
		     $(awk)bldscn.awk $(awk)isort.awk $(awk)bldplm.awk
	gawk -f $(awk)bldscn -f $(awk)isort -f $(awk)bldplm -
		'f$search("$(mms$source)")' > $(mms$target)

# Build ddb file

# Build Encapsulated Files list.
$(cfg)encap.c : $(expfile) $(awk)bldscn.awk $(awk)bldenc.awk
	gawk -f $(awk)bldscn -f $(awk)bldenc -
		'f$search("$(mms$source)")' > $(mms$target)

# Build symbol inclusion/exclusion file.
$(cfg)sym_xxclusions.dat : $(expfile) $(awk)bldscn.awk $(awk)bldsym.awk
	gawk -f $(awk)bldscn -f $(awk)bldsym -
		'f$search("$(mms$source)")' > $(mms$target)

# Build driver startup table list.
$(cfg)dst.c : $(expfile) $(awk)bldscn.awk $(awk)blddst.awk
	gawk -f $(awk)bldscn -f $(awk)blddst -
		'f$search("$(mms$source)")' > $(mms$target)

# Build object list file.
$(cfg)objects.txt : $(expfile) $(awk)bldscn.awk $(awk)bldobj.awk
	gawk -f $(awk)bldscn -f $(awk)bldobj  -
		'f$search("$(mms$source)")' > $(mms$target)

# Build object list file.
$(cfg)objects$(opt).txt : $(expfile) $(awk)bldscn.awk $(awk)bldobj.awk
	gawk -f $(awk)bldscn -f $(awk)bldobj  -
		"opt=$(opt)" -
		'f$search("$(mms$source)")' > $(mms$target)

# Build Linker options file for nonoptimized build.
$(cfg)options.opt : $(expfile) $(awk)bldscn.awk $(awk)bldopt.awk
	gawk -f $(awk)bldscn -f $(awk)bldopt -
		'f$search("$(mms$source)")' > $(mms$target)

# Build Linker options file for optimized build.
$(cfg)options$(opt).opt : $(expfile) $(awk)bldscn.awk $(awk)bldopt.awk
	gawk -f $(awk)bldscn -f $(awk)bldopt  -
		"opt=$(opt)" -
		'f$search("$(mms$source)")' > $(mms$target)

# Build Object dependencies for inclusion by MMS.
$(cfg)objects.mms : $(expfile) $(awk)bldscn.awk $(awk)blddep.awk
	gawk -f $(awk)bldscn -f $(awk)blddep  -
		'f$search("$(mms$source)")' > $(mms$target)

# Build list of source files.
$(cfg)sources.txt : $(expfile) $(awk)bldscn.awk $(awk)bldsrc.awk
	gawk -f $(awk)bldscn -f $(awk)bldsrc  -
		'f$search("$(mms$source)")' > $(mms$target)

# Process include directives in the build file.
#
# NOTE: This still needs work to be accurate.
#	Need to find out what files are included by $(buildfile), and depend
#	on all of them (requires multiple phases of MMS for newbuild).  For
#	now, put OPTIONS.BLD on the dependency line.

$(expfile) : $(buildfile) $(awk)include.awk $(src)options.bld -
		$(src)$(platform)_files.bld
	gawk -f $(awk)include $(mms$source) > $(mms$target)
