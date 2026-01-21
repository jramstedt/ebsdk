
PLATFORM =
CONFIG =
OPT = _opt

SRC = $(firstword $(subst :, ,$(CP_SRC)))/
PSRC = $(word 2, $(subst :, ,$(CP_SRC)))/
CFG = $(firstword $(subst :, ,$(CP_CFG)))/
INC = $(firstword $(subst :, ,$(CP_INC)))/
LIS = $(firstword $(subst :, ,$(CP_LIS)))/
OBJ = $(firstword $(subst :, ,$(CP_OBJ)))/

# https://www.gnu.org/software/make/manual/html_node/General-Search.html
VPATH = $(CP_SRC)

AWK	= awk
COPY	= cp
TOUCH	= touch

AWKSR = $(patsubst %,-f %,$(filter %.awk, $^))

BUILDFILE = $(CONFIG)_files.bld
EXPFILE = $(CFG)config.bld

#CCFLAGS += $(patsubst %,-I%,$(subst :, ,$(CP_INC)))

.PHONY : all

all : $(CFG)sources.txt $(CFG)objects.make \
			$(CFG)options.opt $(CFG)options$(OPT).opt \
			$(CFG)objects.txt $(CFG)objects$(OPT).txt \
			$(INC)prototypes.h $(INC)kernel_entry.h \
			$(SRC)platform_cpu.sdl \
			$(INC)platform_io.h \
			$(SRC)platform_fru.h \
			$(SRC)__decc_include_prologue.h \
			$(CFG)dst.c $(CFG)encap.c \
			$(CFG)platform.h $(CFG)platform.S \
			$(CFG)macros.make \
			$(CFG)builtins.c $(CFG)help_text.h \
			$(CFG)depend.make \
			$(CFG)msgtxt.c $(CFG)msgnum.h \
			$(CFG)sym_xxclusions.dat \
			$(CFG)pdlist.c
	$(info newbuild completed)

$(CFG)pdlist.c : 
	echo 'TODO pdlist.c'

# DECC
$(SRC)__decc_include_prologue.h : __decc_include_prologue.h
	$(COPY) $^ $@

# Platform specific FRU definitions.
$(PLATFORM)_fru.h : 
$(SRC)platform_fru.h : $(PLATFORM)_fru.h
	$(COPY) $^ $@ 2>/dev/null || $(TOUCH) $@

# Platform specific io definitions.
$(INC)platform_io.h : $(PLATFORM)_io.h
	$(COPY) $^ $@

# Platform specific cpu definitions.
$(SRC)platform_cpu.sdl : $(PLATFORM).sdl
	$(COPY) $^ $@

# Prototype definitions, Platform specific call between definitions.
$(INC)%.h : %.h
	$(COPY) $^ $@

# build_*.awk for sources.txt

# External message declarations
$(CFG)msgnum.h : $(CFG)msgtxt.c $(CFG)msgdef.awk isort.awk build_msgh.awk
	$(AWK) $(AWKSR) $< > $@

# Message definitions
$(CFG)msgtxt.c : sources.txt srcexp.awk isort.awk $(CFG)msgdef.awk build_msg.awk
	$(AWK) $(AWKSR) $< > $@

# Dependencies
$(CFG)depend.make $(CFG)full_depend.make &: sources.txt srcexp.awk build_depend.awk
	$(AWK) $(AWKSR) "exc=^(modlist|msgtxt)\.c$$" "exch=(stdio|platform)\.h$$" $< > $@

# Help text
$(CFG)help_text.h : sources.txt srcexp.awk build_helptext.awk
	$(AWK) $(AWKSR) "exc=^(modlist|msgtxt)\.c$$" $< > $@

# Builtins
$(CFG)builtins.c : sources.txt srcexp.awk build_builtins.awk
	$(AWK) $(AWKSR) 'exc=^((builtins|modlist|msgtxt)\.c)$$' $< > $@

# bld*.awk for EXPFILE

# Message definitions
$(CFG)msgdef.awk : $(EXPFILE) bldscn.awk bldmsg.awk
	$(AWK) $(AWKSR) $< > $@

# Build symbol inclusion/exclusion file. ??
$(CFG)sym_xxclusions.dat : $(EXPFILE) bldscn.awk bldsym.awk
	$(AWK) $(AWKSR) $< > $@

# Macro definitions
$(CFG)macros.make : $(EXPFILE) bldscn.awk isort.awk bldmakefile.awk
	$(AWK) $(AWKSR) $< > $@

# Platform assembler header file.  For compatability only...
$(CFG)platform.S : $(EXPFILE) bldscn.awk isort.awk bldpls.awk
	$(AWK) $(AWKSR) $< > $@

# Complete platform header file.  For compatability only...
$(CFG)platform.h : $(EXPFILE) bldscn.awk isort.awk bldplh.awk
	$(AWK) $(AWKSR) $< > $@

# Build Encapsulated Files list.
$(CFG)encap.c : $(EXPFILE) bldscn.awk bldenc.awk
	$(AWK) $(AWKSR) $< > $@

# Build driver startup table list.
$(CFG)dst.c : $(EXPFILE) bldscn.awk blddst.awk
	$(AWK) $(AWKSR) $< > $@

# Build optimized object list file.
$(CFG)objects$(OPT).txt : $(EXPFILE) bldscn.awk bldobj.awk
	$(AWK) $(AWKSR) "opt=$(OPT)" $< > $@

# Build nonoptimized object list file.
$(CFG)objects.txt : $(EXPFILE) bldscn.awk bldobj.awk
	$(AWK) $(AWKSR) $< > $@

# Build Linker options file for optimized build.
$(CFG)options$(OPT).opt : $(EXPFILE) bldscn.awk bldopt.awk
	$(AWK) $(AWKSR) "opt=$(OPT)" $< > $@

# Build Linker options file for nonoptimized build.
$(CFG)options.opt : $(EXPFILE) bldscn.awk bldopt.awk
	$(AWK) $(AWKSR) $< > $@

# Build Object dependencies for inclusion.
$(CFG)objects.make : $(EXPFILE) bldscn.awk blddep.awk
	$(AWK) $(AWKSR) $< > $@

# Build list of source files.
$(CFG)sources.txt : $(EXPFILE) bldscn.awk bldsrc.awk
	$(AWK) $(AWKSR) $< > $@

# Process include directives in the build file.
$(EXPFILE) : $(BUILDFILE) include.awk options.bld $(PLATFORM)_files.bld
	$(AWK) $(AWKSR) $< > $@
