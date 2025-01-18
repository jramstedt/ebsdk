CONFIG_DIR=../../../..
include $(CONFIG_DIR)/sdkmake.conf

include $(EB_BASE)/rules.gnu

AS		= as
LC		= $(shell echo '$1' | tr '[:upper:]' '[:lower:]' )
LD		= ld
CC		= gcc
CPP		= cpp
OBJCOPY		= objcopy
OBJDUMP		= objdump
HEX		= $(TOOLBOX)/hex32
SROM		= $(TOOLBOX)/srom
IC4MAT		= $(TOOLBOX)/ic4mat
SREC		= $(TOOLBOX)/srec
MAKEROM		= $(TOOLBOX)/makerom

ALPHA_TOOLS 	= \
	AS=alpha-linux-gnu-as \
	AR=alpha-linux-gnu-ar \
	LD=alpha-linux-gnu-ld \
	CC=alpha-linux-gnu-gcc \
	CPP=alpha-linux-gnu-cpp \
	OBJCOPY=alpha-linux-gnu-objcopy \
	OBJDUMP=alpha-linux-gnu-objdump

INCLUDES	= -I. -I../include

ASFLAGS		= -m$(call LC,$(CPU)) # -acdlnm="$@.lst"

CPU_064		= 21064
CPU_064A	= 21064A
CPU_066		= 21066
CPU_068		= 21068
CPU_164		= 21164
CPU_164PC	= 21164PC
CPU_264		= 21264
CPU		= $(CPU_$(TYPE))

CPP_064		= -DFULL_MDBG	# Enables "PR" command.
CPP_066		=
CPP_164		= -DFULL_MDBG	# Enables "PR" command.
CPP_164PC	= -DFULL_MDBG	# Enables "PR" command.
CPP_264		= -DFULL_MDBG	# Enables "PR" command.

CPP_EXTRA_d	= -DDEBUG
CPP_EXTRA	= $(CPP_$(TYPE)) $(CPP_EXTRA_$(VERSION))

SROMFLAGS	= -v
CPPFLAGS	= -DDC$(CPU) $(CPP_EXTRA) -fdollars-in-identifiers

### Mini debugger
MAKEFILE	= Makefile
PROGRAM		= md$(TYPE)$(VERSION)
CBOX_FILE	= dp264.cbx

BIN_TYPE_264	= ic
ifdef BIN_TYPE_$(TYPE)
BIN_TYPE 	= $(BIN_TYPE_$(TYPE))
else
BIN_TYPE 	= exe
endif

MAKEHEX_264	= $(HEX) 
ifdef MAKEHEX_$(TYPE)
MAKEHEX		= $(MAKEHEX_$(TYPE))
else
MAKEHEX		= $(SROM) $(SROMFLAGS) -$(CPU) 
endif

all : mdbg064 mdbg066 mdbg164 mdbg164PC mdbg264

mdbg% :
	$(MAKE) -f $(MAKEFILE) $(ALPHA_TOOLS) TYPE=$* tools
	$(MAKE) -f $(MAKEFILE) $(ALPHA_TOOLS) TYPE=$* VERSION=d dtools

tools : $(PROGRAM).exe $(PROGRAM).lis $(PROGRAM).hex $(PROGRAM).sr $(PROGRAM).pvc
dtools : $(PROGRAM).exe $(PROGRAM).lis $(PROGRAM).pvc

mini_rcs.h : mini_dbg.s
	$(TOOLBOX)/rcsv -v $< $@

$(PROGRAM).sp : mini_dbg.s mini_rcs.h mini_dbg.h serialio.s ascitab.h sromio.s ../include/dc21064.h ../include/dc21164.h ../include/dc21264.h wm.h
	$(CPP) $(CPPFLAGS) $(INCLUDES) $< > $@

$(PROGRAM).o : $(PROGRAM).sp
	$(AS) $(ASFLAGS) -R -o $@ $^
	
$(PROGRAM).pvc :
	echo set code_file $(PROGRAM).exe > $@
	echo set entry_file dbg$(TYPE).ent >> $@
	echo set cpu $(CPU) >> $@
#	echo set flag all >> $@
	echo go >> $@
	echo quit >> $@

$(PROGRAM).exe : $(PROGRAM).o
	$(OBJCOPY) -O binary $< $@
#	$(ASTRIP) $(ASTRIP_FLAGS) $(PROGRAM).o $(PROGRAM).exe
#	$(PVC) < $(PROGRAM).pvc

$(PROGRAM).lis : $(PROGRAM).o
#	$(ALIST) $(PROGRAM).o > $*.lis
	$(OBJDUMP) -x $< > $@

$(PROGRAM).hex : $(PROGRAM).$(BIN_TYPE)
	$(MAKEHEX) $(PROGRAM).$(BIN_TYPE) $@

$(PROGRAM).sr : $(PROGRAM).$(BIN_TYPE)
	$(SREC) -v -i $(PROGRAM).$(BIN_TYPE) $@
#	$(OBJCOPY) -S -O srec $(PROGRAM).o $@.gnu

$(PROGRAM).ic: $(CBOX_FILE) $(PROGRAM).exe 
	echo "$(IC4MAT) -21264 -v -l cbox.lis -m -4 $^ $@" > ic4mat.log
	$(IC4MAT) -21264 -v -l cbox.lis -m -4 $^ $@ >> ic4mat.log

$(PROGRAM).rom : $(PROGRAM).exe
	$(MAKEROM) -v -s"MINIDBGR" -i"4" -l200000 $< -o $@

### Install
install : install264 install164PC install164 install064 install066

install% :
	$(MAKE) -f $(MAKEFILE) $(ALPHA_TOOLS) TYPE=$* toolsinstall
	$(MAKE) -f $(MAKEFILE) $(ALPHA_TOOLS) TYPE=$* VERSION=d dtoolsinstall

toolsinstall : tools
	cp -fp $(PROGRAM).exe $(TOOLBOX)
	cp -fp $(PROGRAM).hex $(TOOLBOX)

dtoolsinstall : dtools
	cp -fp $(PROGRAM).exe $(TOOLBOX)

### Boot
boot : boot264 boot164PC boot164 boot064 boot066

boot% :
	$(MAKE) -f $(MAKEFILE) $(ALPHA_TOOLS) TYPE=$* bootinstall
	$(MAKE) -f $(MAKEFILE) $(ALPHA_TOOLS) TYPE=$* VERSION=d dbootinstall

bootinstall : toolsinstall
	cp -fp $(PROGRAM).exe $(EB_BOOTDIR)/$(PROGRAM).exe
	chmod a+r $(EB_BOOTDIR)/$(PROGRAM).exe
	cp -fp $(PROGRAM).hex $(EB_BOOTDIR)/$(PROGRAM).hex
	chmod a+r $(EB_BOOTDIR)/$(PROGRAM).hex

dbootinstall : dtoolsinstall
	cp -fp $(PROGRAM).exe $(EB_BOOTDIR)/$(PROGRAM).exe
	chmod a+r $(EB_BOOTDIR)/$(PROGRAM).exe

### Clean
clean : clean264 clean164PC clean164 clean064 clean066

clean% :
	$(MAKE) -f $(MAKEFILE) $(ALPHA_TOOLS) TYPE=$* doclean
	$(MAKE) -f $(MAKEFILE) $(ALPHA_TOOLS) TYPE=$* VERSION=d doclean

doclean :
	rm -f *.o *~ \#*\# *.bak *.a core \
		$(PROGRAM).ic \
		$(PROGRAM).sr \
		$(PROGRAM).lis \
		$(PROGRAM).exe \
		$(PROGRAM).rom \
		$(PROGRAM).hex \
		$(PROGRAM).o \
		$(PROGRAM).sp \
		$(PROGRAM).pvc \
		$(PROGRAM).map \
		mini_rcs.h

.PHONY : all install boot clean