# $Id: Makefile,v 1.8 1999/02/11 20:29:58 gries Exp $
#
# $Log: Makefile,v $
# Revision 1.8  1999/02/11 20:29:58  gries
# we not testing
#
# Revision 1.7  1999/02/11 19:59:46  gries
# Changes for White
#
# Revision 1.6  1999/02/11 17:49:27  gries
# update for nt
#
# Revision 1.5  1999/01/25 18:57:29  gries
# Goldrush makefile (not brick)
#
# Revision 1.3  1999/01/21 20:55:07  gries
# dd for new build
#
# Revision 1.2  1999/01/21 19:05:49  gries
# First Release to cvs gries
#
# Revision 1.1.1.1  1998/12/29 21:36:13  paradis
# Initial CVS checkin
#
# Revision 1.8  1998/07/30  21:21:39  gries
# mike quinn as defualt
#
# Revision 1.7  1998/07/30  14:46:38  gries
# current Makefile
#
# Revision 1.6  1998/04/13  14:05:51  gries
# Makefile for srm hack
#
# Revision 1.5  1998/03/26  14:37:57  gries
# Makefile to build with jim's cbox files
#
# Revision 1.4  1998/03/06  15:32:50  gries
# lastest Makefile
#
# Revision 1.3  1998/02/23  19:55:20  gries
# lastest build
#
# Revision 1.2  1997/12/17  15:16:38  pbell
# Fixed build name conversions.
#
# Revision 1.1  1997/12/15  19:22:05  pbell
# -
#
# Revision 1.12  1997/11/19  19:50:10  gries
# Quin'new Cache timing Writeonce chain
#
# Revision 1.11  1997/10/31  19:37:23  fdh
# Trying other cbox files...
#
# Revision 1.10  1997/09/22  18:47:42  fdh
# Builds all files for default builds (s), DEBUG_BOARD builds (d)
# and ISP_SIMULATION builds (x) and then merge the appropriate
# files into a single EPROM.
#
# Revision 1.9  1997/08/13  16:21:45  fdh
# Added the BIT definition.
#
# Revision 1.8  1997/08/13  16:14:31  fdh
# Added the IC4MAT definition.
#
# Revision 1.7  1997/08/13  15:32:56  fdh
# Corrected the link to rules.osf.
#
# Revision 1.6  1997/07/26  22:24:53  fdh
# Added push: target to push files out for others to see.
#
# Revision 1.5  1997/07/26  21:36:46  fdh
# Added $(MFLAGS) to $(MAKE) lines.
#
# Revision 1.4  1997/07/26  12:24:10  fdh
# Added rules to build a -DISP_SIMULATION version XXXXXxsr.XXX.
#
# Revision 1.3  1997/07/24  17:55:52  fdh
# Added the .bit and .bsr targets.
#
# Revision 1.2  1997/07/22  20:24:54  fdh
# Modified to use debug.cbox.
#
# Revision 1.1  1997/07/22  13:53:10  fdh
# Initial revision
#

CONFIG_DIR=../../..
include $(CONFIG_DIR)/sdkmake.conf

include $(EB_BASE)/rules.gnu

IC4MAT          = $(TOOLBOX)/ic4mat
BIT             = $(TOOLBOX)/bit
SOURCE          = srom.s
FULLPROGRAM     = $(TARGET)_sr
#BUILD_MODE     = $(BUILD_MODE ? $(BUILD_MODE) :x)
BUILD_MODE      = x
PROGRAM         = $(BUILD_MODE)

DEFAULT_TARGETS = \
	$(BOOTDIR)$(PROGRAM).img \
	$(BOOTDIR)$(PROGRAM).hex  \
	$(OBJDIR)$(PROGRAM).lis  \
	$(BOOTDIR)$(PROGRAM).pad

sx164_TARGETS = \
	$(DEFAULT_TARGETS) \
	$(BOOTDIR)$(PROGRAM).rom

CBXFILES        = \
	25x_y3p2.cbx 

ICFILES         = $(CBXFILES:.cbx=.ic)

FULLICOBJFILES  =       \
	$(OBJDIR)s25x_y3p2.ic \
	$(OBJDIR)s25x_y3p2.ic \
	$(OBJDIR)s25x_y3p2.ic \
	$(OBJDIR)s25x_y3p2.ic \
	$(OBJDIR)s25x_y3p2.ic \
	$(OBJDIR)s25x_y3p2.ic \
	$(OBJDIR)s25x_y3p2.ic \
	$(OBJDIR)d25x_y3p2.ic 



dp264_TARGETS = \
	$(BOOTDIR)$(PROGRAM).img \
	$(OBJDIR)$(PROGRAM).lis \
	$(ICFILES)

dp264s_COMPILE_OPTIONS  = -DSRM_HACK -DREVD   -DLCD -Dwhite
dp264d_COMPILE_OPTIONS  = -DDEBUG_BOARD -DSRM_HACK -DREVD -DLCD -Dwhite
dp264x_COMPILE_OPTIONS  = -DISP_SIMULATION -DSRM_HACK -DREVD -DLCD -Dwhite
COMPILE_OPTIONS                 = $($(TARGET)$(BUILD_MODE)_COMPILE_OPTIONS)

ifneq "$($(TARGET)_TARGETS)" ""
BUILD_TARGETS = $($(TARGET)_TARGETS)
else
BUILD_TARGETS = $(DEFAULT_TARGETS)
endif

# -------------------------------------------------
# Append local flags here for only this Makefile
# -------------------------------------------------

MDBG_dp264              = $(TOOLBOX)/md264d.exe
DMDBG_dp264             = $(TOOLBOX)/md264d.exe

ifdef EB_DEBUG_SROM
MINIDBG                 = $(DMDBG_$(TARGET))
SROM_CPP                = -DDEBUG_BOARD=1
else
MINIDBG                 = $(MDBG_$(TARGET))
SROM_CPP                = 
endif

GASSWITCHES             = $(GASFLAGS_GLOBAL) -P
CPPSWITCHES             = $(COMPILE_OPTIONS) \
			  $(SROM_INCLUDES) \
			  $(OPTION_FLAGS_GLOBAL) \
			  $(SROM_CPP) 
SROM_TOOL_OPTIONS       = $(SROM_FLAGS_GLOBAL)

dp264_SROMSIZE          = -x0x8004
HEXPAD_TOOL_OPTIONS     = $(HEXPAD_FLAGS_GLOBAL) $($(TARGET)_SROMSIZE)

.SUFFIXES: .cbx .ic


%.ic: %.cbx
	$(IC4MAT) -21264 -p 3 -v -l $(OBJDIR)$(PROGRAM)$*.lis -m -4 $< $(OBJDIR)$(PROGRAM).img $(OBJDIR)$(PROGRAM)$*.ic >> $(OBJDIR)$(PROGRAM)$*.log
	$(SREC) -i $(OBJDIR)$(PROGRAM)$*.ic > $(BOOTDIR)$(PROGRAM)$*.sr
	$(CHMOD) 664 $(BOOTDIR)$(PROGRAM)$*.sr


# -------------------------------------------------
# Main targets
# -------------------------------------------------
all: $(BOOTDIR) $(OBJDIR) _sr dsr xsr \
	$(BOOTDIR)$(FULLPROGRAM).bsr \
	$(BOOTDIR)$(FULLPROGRAM).bit

#-----------------------------------------------------------
_sr:
	$(EB_MAKE) $(MFLAGS) MAKEFILE=$(MAKEFILE) -f $(MAKEFILE) BUILD_MODE=s targets

dsr:
	$(EB_MAKE) $(MFLAGS) MAKEFILE=$(MAKEFILE) -f $(MAKEFILE) BUILD_MODE=d targets

xsr:
	$(EB_MAKE) $(MFLAGS) MAKEFILE=$(MAKEFILE) -f $(MAKEFILE) BUILD_MODE=x targets

targets:        $(BUILD_TARGETS)

#-----------------------------------------------------------
# Rules for creating main targets.
#-----------------------------------------------------------
$(BOOTDIR)$(PROGRAM).img: $(OBJDIR)$(PROGRAM).img $(BOOTDIR)
	$(COPY) $< $(COPY_TO) $@
	$(CHMOD) 664 $@

$(BOOTDIR)$(PROGRAM).hex: $(OBJDIR)$(PROGRAM).hex $(BOOTDIR)
	$(COPY) $< $(COPY_TO) $@
	$(CHMOD) 664 $@

$(BOOTDIR)$(PROGRAM).pad: $(OBJDIR)$(PROGRAM).pad $(BOOTDIR)
	$(COPY) $< $(COPY_TO) $@
	$(CHMOD) 664 $@

$(BOOTDIR)$(PROGRAM).rom: $(OBJDIR)$(PROGRAM).rom $(BOOTDIR)
	$(COPY) $< $(COPY_TO) $@
	$(CHMOD) 664 $@

$(BOOTDIR)$(PROGRAM).ic: $(OBJDIR)$(PROGRAM).ic
	$(COPY) $< $(COPY_TO) $@
	$(CHMOD) 664 $@

$(BOOTDIR)$(PROGRAM).sr: $(OBJDIR)$(PROGRAM).sr
	$(COPY) $< $(COPY_TO) $@
	$(CHMOD) 664 $@

$(BOOTDIR)$(FULLPROGRAM).bsr: $(OBJDIR)$(FULLPROGRAM).bsr
	$(COPY) $< $(COPY_TO) $@
	$(CHMOD) 664 $@

$(BOOTDIR)$(FULLPROGRAM).bit: $(OBJDIR)$(FULLPROGRAM).bit
	$(COPY) $< $(COPY_TO) $@
	$(CHMOD) 664 $@

$(OBJDIR)$(PROGRAM).lis: $(OBJDIR)$(PROGRAM).out
	$(ALIST) $< > $@

$(OBJDIR)$(PROGRAM).pad: $(OBJDIR)$(PROGRAM).hex
	$(HEXPAD) $(HEXPAD_TOOL_OPTIONS) $< $@

#-----------------------------------------------------------
# Rules for creating files that are needed to build the
# main targets.
#-----------------------------------------------------------

$(OBJDIR)$(PROGRAM).img: $(OBJDIR)$(PROGRAM).nh $(MINIDBG)
	$(CAT) $(OBJDIR)$(PROGRAM).nh $(MINIDBG) > $@

$(OBJDIR)$(PROGRAM).nh: $(OBJDIR)$(PROGRAM).out
	$(ASTRIP) $(ASTRIP_FLAGS) $< $@

$(OBJDIR)$(PROGRAM).out: $(OBJDIR)$(PROGRAM).cpp
	$(GAS) $(GASSWITCHES) $< -o $@

$(OBJDIR)$(PROGRAM).cpp: $(SOURCE) $(OBJDIR)rcsv.h
	$(DELETE) $@
	$(CPP) -v $(CPPSWITCHES) -I$(OBJDIR) $(SOURCE) $(CPPOUT) $@

$(OBJDIR)rcsv.h: $(SOURCE)
	$(TOOLBOX)/rcsv -v -x $(SOURCE) $@

$(OBJDIR)$(PROGRAM).rom: $(OBJDIR)$(PROGRAM).img
	$(MAKEROM) -v -e -iSROM -l0 $< -o $@

$(OBJDIR)$(PROGRAM).ic: $(OBJDIR)$(PROGRAM).img

$(ICFILES): $(CBXFILES) $(OBJDIR)$(PROGRAM).img

$(OBJDIR)$(PROGRAM).sr: $(OBJDIR)$(PROGRAM).ic
	$(SREC) -i $< > $@

$(OBJDIR)$(PROGRAM).hex: $(OBJDIR)$(PROGRAM).ic
	$(HEX) $< $@

$(OBJDIR)$(FULLPROGRAM).bsr: $(OBJDIR)$(FULLPROGRAM).bit
	$(SREC) -i $< > $@

$(OBJDIR)$(FULLPROGRAM).bit: $(FULLICOBJFILES)
	$(BIT) $(FULLICOBJFILES) -o $@

$(OBJDIR):
	mkdir $(OBJDIR)
	$(CHMOD) 755 $(OBJDIR)

$(BOOTDIR):
	mkdir $(BOOTDIR)
	$(CHMOD) 755 $(BOOTDIR)

#-----------------------------------------------------------
# For erasing the files created by this makefile.
#-----------------------------------------------------------
clean:
	$(EB_MAKE) $(MFLAGS) MAKEFILE=$(MAKEFILE) -f $(MAKEFILE) BUILD_MODE=s clean_targets
	$(EB_MAKE) $(MFLAGS) MAKEFILE=$(MAKEFILE) -f $(MAKEFILE) BUILD_MODE=d clean_targets
	$(EB_MAKE) $(MFLAGS) MAKEFILE=$(MAKEFILE) -f $(MAKEFILE) BUILD_MODE=x clean_targets

clean_targets:
	-rm -f *~ \#*\# core dependcy.lis.bak \
		`basename $(CBOX_FILE)`.lis \
		$(OBJDIR)$(PROGRAM).lis \
		$(OBJDIR)$(PROGRAM).bit \
		$(OBJDIR)$(PROGRAM).bsr \
		$(OBJDIR)$(PROGRAM).sr \
		$(OBJDIR)$(PROGRAM).ic \
		$(OBJDIR)$(PROGRAM).img \
		$(OBJDIR)$(PROGRAM).nh  \
		$(OBJDIR)$(PROGRAM).hex \
		$(OBJDIR)$(PROGRAM).pad \
		$(OBJDIR)$(PROGRAM).out \
		$(OBJDIR)$(PROGRAM).cpp \
		$(OBJDIR)$(PROGRAM).rom \
		$(OBJDIR)rcsv.h

	$(CLEAN)

#-----------------------------------------------------------
# For creating the dependency list included below.
#-----------------------------------------------------------
ifneq "$(EB_BUILD_SYSTEM)" "WINNT"
depend:
	makedepend -f dependcy.lis  -o.cpp -p'$$(OBJDIR)$(TARGET)_sr.cpp $$(OBJDIR)' \
		-- -DMAKEDEPEND $(CPPSWITCHES) -- $(SOURCE)
	makedepend -a -f dependcy.lis  -o.cpp -p'$$(OBJDIR)$(TARGET)xsr.cpp $$(OBJDIR)' \
		-- -DMAKEDEPEND $(CPPSWITCHES) -- $(SOURCE)

include dependcy.lis

endif
