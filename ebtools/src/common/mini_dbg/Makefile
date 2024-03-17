# $Id: Makefile,v 1.1.1.1 1998/12/29 21:36:25 paradis Exp $
# $Log: Makefile,v $
# Revision 1.1.1.1  1998/12/29 21:36:25  paradis
# Initial CVS checkin
#
# Revision 1.21  1997/12/05  16:28:00  pbell
# removed sub4, turned off pvc check.
#
# Revision 1.20  1997/12/04  21:39:37  pbell
# Merged 21264 changes.
#
# Revision 1.19  1997/07/26  09:19:23  fdh
# Modified to use the builtin $(MAKE) definition instead of make directly.
#
# Revision 1.18  1997/05/30  02:07:37  fdh
# Merged in support for the DC21164PC.
#
# Revision 1.17  1996/06/30  21:12:14  fdh
# Updated dependencies.
#
# Revision 1.16  1996/04/29  16:02:52  cruz
# Enabled full-mini for 21064.
#
# Revision 1.15  1995/12/11  01:50:18  fdh
# Shortened a filename.
#
# Revision 1.14  1995/11/27  22:12:06  fdh
# Added $(PROGRAM).hex dependency to the install2: target.
#
# Revision 1.13  1995/11/27  21:58:39  cruz
# Updated install rule so it doesn't copy hex file for debugging version.
#
# Revision 1.12  1995/11/27  21:23:14  cruz
# Don't make hex file for debug version.
#
# Revision 1.11  1995/11/14  23:46:11  cruz
# Updated rules so we can also build the debugging version
# of the minidebugger.
#
# Revision 1.10  1995/05/24  14:33:27  cruz
# Removed -P switch from GASSWITCHES
#
# Revision 1.8  1995/04/19  21:17:58  cruz
# Changed compilation constant
#
# Revision 1.7  1995/04/19  20:01:50  cruz
# Corrected the install rule.
#
# Revision 1.6  1995/04/19  19:57:51  cruz
# Added new compilation switch to enable new commands for
# 21164 (since EB164 uses the larger SROM and we know it
# will have enough space for the new commands)
#
# Revision 1.5  1995/04/19  04:29:20  fdh
# Modified final target.
# Bypassed creation of $(PROGRAM).nh.
# Added an install: target.
# Cleaned up the depend: target.
#
# Revision 1.4  1995/04/18  23:10:11  cruz
# Changed file to build all 3 versions at once.
#
# Revision 1.3  1995/04/18  21:04:46  cruz
# Changed name of object directory.
#
# Revision 1.2  1995/04/18  18:50:50  cruz
# Run rcsv tool on mini_dbg.s
#
# Revision 1.1  1995/04/12  23:16:40  cruz
# Initial revision
#
# Revision 1.3  1995/01/21  13:42:53  fdh
# Fixed up include file dependencies.
#
# Revision 1.2  1995/01/20  21:47:31  cruz
# Minor changes to CPP switches.
#
# Revision 1.1  1994/12/19  18:39:21  cruz
# Initial revision
#

CONFIG_DIR=../../../..
include $(CONFIG_DIR)/sdkmake.conf

include $(EB_BASE)/rules.gnu

MAKEFILE	= Makefile

TOOLOBJ		= ./

IC4MAT		= $(TOOLBOX)/ic4mat

CPU_064		= 21064
CPU_066		= 21066
CPU_164		= 21164
CPU_164PC	= 21164PC
CPU_264		= 21264
CPU 		= $(CPU_$(TYPE))

INCLUDES 	= -I. -I../include

CPP_064		= -DFULL_MDBG	# Enables "PR" command.
CPP_066		=
CPP_164		= -DFULL_MDBG	# Enables "PR" command.
CPP_164PC	= -DFULL_MDBG	# Enables "PR" command.
CPP_264		= -DFULL_MDBG	# Enables "PR" command.

CPP_EXTRA_d	= -DDEBUG
CPP_EXTRA	= $(CPP_$(TYPE)) $(CPP_EXTRA_$(VERSION))

CPPSWITCHES	= -DDC$(CPU) $(INCLUDES) $(CPP_EXTRA)
GASSWITCHES	= -v -$(CPU) 
PVC		= $(TOOLBOX)/pvc
SROM 		= $(TOOLBOX)/srom
SROM_TOOL_OPTIONS = -v

PROGRAM		= $(TOOLOBJ)md$(TYPE)$(VERSION)

BIN_TYPE_264	= ic
ifdef BIN_TYPE_$(TYPE)
BIN_TYPE = $(BIN_TYPE_$(TYPE))
else
BIN_TYPE = exe
endif

MAKEHEX_264	= $(HEX) 

ifdef MAKEHEX_$(TYPE)
MAKEHEX		= $(MAKEHEX_$(TYPE))
else
MAKEHEX		= $(SROM) $(SROM_TOOL_OPTIONS) -$(CPU) 
endif

all:	mdbg064 mdbg066 mdbg164 mdbg164pc mdbg264

mdbg064:
	$(EB_MAKE) $(MFLAGS) -f $(MAKEFILE) TYPE=064 tools
	$(EB_MAKE) $(MFLAGS) -f $(MAKEFILE) TYPE=064 VERSION=d dtools

mdbg066:
	$(EB_MAKE) $(MFLAGS) -f $(MAKEFILE) TYPE=066 tools
	$(EB_MAKE) $(MFLAGS) -f $(MAKEFILE) TYPE=066 VERSION=d dtools

mdbg164:
	$(EB_MAKE) $(MFLAGS) -f $(MAKEFILE) TYPE=164 tools
	$(EB_MAKE) $(MFLAGS) -f $(MAKEFILE) TYPE=164 VERSION=d dtools

mdbg164pc:
	$(EB_MAKE) $(MFLAGS) -f $(MAKEFILE) TYPE=164PC tools
	$(EB_MAKE) $(MFLAGS) -f $(MAKEFILE) TYPE=164PC VERSION=d dtools

mdbg264:
	$(EB_MAKE) $(MFLAGS) -f $(MAKEFILE) TYPE=264 tools
	$(EB_MAKE) $(MFLAGS) -f $(MAKEFILE) TYPE=264 VERSION=d dtools


tools:	$(PROGRAM).exe $(PROGRAM).hex $(PROGRAM).lis $(PROGRAM).sr $(PROGRAM).pvc
dtools:	$(PROGRAM).exe $(PROGRAM).lis $(PROGRAM).pvc

$(PROGRAM).lis: $(PROGRAM).o
	$(ALIST) $(PROGRAM).o > $*.lis

$(PROGRAM).pvc:	$(MAKEFILE)
	echo set code_file $(PROGRAM).exe > $@
	echo set entry_file dbg$(TYPE).ent >> $@
	echo set cpu $(CPU) >> $@
#	echo set flag all >> $@
	echo go >> $@
	echo quit >> $@

$(PROGRAM).rom: $(PROGRAM).exe
	$(MAKEROM) -v -s"MINIDBGR" -i"4" -l200000 $(PROGRAM).exe -o $(PROGRAM).rom

$(PROGRAM).sr: $(PROGRAM).$(BIN_TYPE)
	$(SREC) -v -i $(PROGRAM).$(BIN_TYPE) $@

$(PROGRAM).hex: $(PROGRAM).$(BIN_TYPE)
	$(MAKEHEX) $(PROGRAM).$(BIN_TYPE) $@

CBOX_FILE	= dp264.cbx

$(PROGRAM).ic: $(PROGRAM).exe $(CBOX_FILE)
	echo "$(IC4MAT) -21264 -v -l cbox.lis -m -4 \
	$(CBOX_FILE) \
	$(PROGRAM).exe $(PROGRAM).ic" > ic4mat.log

	$(IC4MAT) -21264 -v -l cbox.lis -m -4 \
	$(CBOX_FILE) \
	$(PROGRAM).exe $(PROGRAM).ic >> ic4mat.log

$(PROGRAM).exe: $(PROGRAM).o $(PROGRAM).pvc
	$(ASTRIP) $(ASTRIP_FLAGS) $(PROGRAM).o $(PROGRAM).exe
#	$(PVC) < $(PROGRAM).pvc

$(PROGRAM).map:	$(PROGRAM).o
		$(ALIST) -m $(PROGRAM).o > $@

$(PROGRAM).o: $(PROGRAM).cpp
	$(GAS) $(GASSWITCHES) $(PROGRAM).cpp -o $(PROGRAM).o

$(PROGRAM).cpp: mini_dbg.s $(TOOLOBJ)mini_rcs.h mini_dbg.h serialio.s ascitab.h sromio.s \
	../include/dc21064.h ../include/dc21164.h ../include/dc21264.h wm.h
	-rm -f $(PROGRAM).cpp
	$(CPP) -v $(OPTIONS) $(CPPSWITCHES) mini_dbg.s $(PROGRAM).cpp

$(TOOLOBJ)mini_rcs.h: mini_dbg.s
	$(TOOLBOX)/rcsv -v mini_dbg.s $@

install: install264 install164PC install164 install064 install066

install264:
	$(EB_MAKE) $(MFLAGS) -f $(MAKEFILE) TYPE=264 install2
	$(EB_MAKE) $(MFLAGS) -f $(MAKEFILE) TYPE=264 VERSION=d install2d

install164PC:
	$(EB_MAKE) $(MFLAGS) -f $(MAKEFILE) TYPE=164PC install2
	$(EB_MAKE) $(MFLAGS) -f $(MAKEFILE) TYPE=164PC VERSION=d install2d

install164:
	$(EB_MAKE) $(MFLAGS) -f $(MAKEFILE) TYPE=164 install2
	$(EB_MAKE) $(MFLAGS) -f $(MAKEFILE) TYPE=164 VERSION=d install2d

install064:
	$(EB_MAKE) $(MFLAGS) -f $(MAKEFILE) TYPE=064 install2
	$(EB_MAKE) $(MFLAGS) -f $(MAKEFILE) TYPE=064 VERSION=d install2d

install066:
	$(EB_MAKE) $(MFLAGS) -f $(MAKEFILE) TYPE=066 install2
	$(EB_MAKE) $(MFLAGS) -f $(MAKEFILE) TYPE=066 VERSION=d install2d

install2: tools
	cp -fp $(PROGRAM).exe $(TOOLBOX)
	cp -fp $(PROGRAM).hex $(TOOLBOX)

install2d: dtools
	cp -fp $(PROGRAM).exe $(TOOLBOX)

depend:

boot: boot264 boot164PC boot164 boot064 boot066

boot264:
	$(EB_MAKE) $(MFLAGS) -f $(MAKEFILE) TYPE=264 boot2
	$(EB_MAKE) $(MFLAGS) -f $(MAKEFILE) TYPE=264 VERSION=d boot2d

boot164PC:
	$(EB_MAKE) $(MFLAGS) -f $(MAKEFILE) TYPE=164PC boot2
	$(EB_MAKE) $(MFLAGS) -f $(MAKEFILE) TYPE=164PC VERSION=d boot2d

boot164:
	$(EB_MAKE) $(MFLAGS) -f $(MAKEFILE) TYPE=164 boot2
	$(EB_MAKE) $(MFLAGS) -f $(MAKEFILE) TYPE=164 VERSION=d boot2d

boot064:
	$(EB_MAKE) $(MFLAGS) -f $(MAKEFILE) TYPE=064 boot2
	$(EB_MAKE) $(MFLAGS) -f $(MAKEFILE) TYPE=064 VERSION=d boot2d

boot066:
	$(EB_MAKE) $(MFLAGS) -f $(MAKEFILE) TYPE=066 boot2
	$(EB_MAKE) $(MFLAGS) -f $(MAKEFILE) TYPE=066 VERSION=d boot2d

boot2:    	install2
		cp -fp $(PROGRAM).exe $(EB_BOOTDIR)/$(PROGRAM).exe
		chmod a+r $(EB_BOOTDIR)/$(PROGRAM).exe
		cp -fp $(PROGRAM).hex $(EB_BOOTDIR)/$(PROGRAM).hex
		chmod a+r $(EB_BOOTDIR)/$(PROGRAM).hex

boot2d:    	install2d
		cp -fp $(PROGRAM).exe $(EB_BOOTDIR)/$(PROGRAM).exe
		chmod a+r $(EB_BOOTDIR)/$(PROGRAM).exe


clean: clean264 clean164PC clean164 clean064 clean066

clean264:
	$(EB_MAKE) $(MFLAGS) -f $(MAKEFILE) TYPE=264 clean2
	$(EB_MAKE) $(MFLAGS) -f $(MAKEFILE) TYPE=264 VERSION=d clean2

clean164PC:
	$(EB_MAKE) $(MFLAGS) -f $(MAKEFILE) TYPE=164PC clean2
	$(EB_MAKE) $(MFLAGS) -f $(MAKEFILE) TYPE=164PC VERSION=d clean2

clean164:
	$(EB_MAKE) $(MFLAGS) -f $(MAKEFILE) TYPE=164 clean2
	$(EB_MAKE) $(MFLAGS) -f $(MAKEFILE) TYPE=164 VERSION=d clean2

clean064:
	$(EB_MAKE) $(MFLAGS) -f $(MAKEFILE) TYPE=064 clean2
	$(EB_MAKE) $(MFLAGS) -f $(MAKEFILE) TYPE=064 VERSION=d clean2

clean066:
	$(EB_MAKE) $(MFLAGS) -f $(MAKEFILE) TYPE=066 clean2
	$(EB_MAKE) $(MFLAGS) -f $(MAKEFILE) TYPE=066 VERSION=d clean2

clean2:
	-rm -f *~ \#*\# core ${MAKEFILE}.bak \
		$(PROGRAM).ic \
		$(PROGRAM).sr \
		$(PROGRAM).lis \
		$(PROGRAM).exe \
		$(PROGRAM).rom \
		$(PROGRAM).hex \
		$(PROGRAM).o \
		$(PROGRAM).cpp \
		$(PROGRAM).pvc \
		$(PROGRAM).map \
		$(TOOLOBJ)mini_rcs.h

	$(CLEAN)

depend:
