CONFIG_DIR=../..
include $(CONFIG_DIR)/sdkmake.conf

include $(EB_BASE)/rules.gnu

AS				= as
LC				= $(shell echo '$1' | tr '[:upper:]' '[:lower:]' )
CC				= gcc
LD				= ld
CPP				= cpp
COPY			= cp -p
CHMOD			= chmod
DELETE		= rm -f
OBJCOPY		= objcopy
SREC			= $(TOOLBOX)/srec

ASFLAGS		= -m$(call LC,$(CPUTYPE))
CPPFLAGS	= $(CCFLAGS_GLOBAL) -fdollars-in-identifiers
CCFLAGS		= -c -Os -std=c99 -Wpedantic -Wall -Wextra -fanalyzer -fno-builtin -ffreestanding
LDFLAGS		= --orphan-handling=warn -L$(OBJDIR) -T../dbmboot.ld -z noexecstack -s --no-dynamic-linker -static
LDLIBS		= -l:lib.a -l:stubs.a -lc

ALPHA_TOOLS = \
	AS=alpha-linux-gnu-as \
	AR=alpha-linux-gnu-ar \
	LD=alpha-linux-gnu-ld \
	CC=alpha-linux-gnu-gcc \
	CPP=alpha-linux-gnu-cpp \
	OBJCOPY=alpha-linux-gnu-objcopy \
	OBJDUMP=alpha-linux-gnu-objdump

.PHONY : default all cross-default cross-all clean

default :
	$(MAKE) -f Makefile $(ALPHA_TOOLS) cross-default

all :
	$(MAKE) -f Makefile $(ALPHA_TOOLS) cross-all

cross-default : \
	$(BOOTDIR)hello \
	$(BOOTDIR)size \
	$(BOOTDIR)yellow \
	$(BOOTDIR)regtest \
	$(BOOTDIR)hello.sr \
	$(BOOTDIR)size.sr \
	$(BOOTDIR)yellow.sr \
	$(BOOTDIR)regtest.sr

cross-all : \
	$(BOOTDIR)hello \
	$(BOOTDIR)size \
	$(BOOTDIR)yellow \
	$(BOOTDIR)loops \
	$(BOOTDIR)regtest \
	$(BOOTDIR)sum \
	$(BOOTDIR)mtest \
	$(BOOTDIR)pic \
	$(BOOTDIR)sound \
	$(BOOTDIR)time \
	$(BOOTDIR)flash \
	$(BOOTDIR)comtrol

#	$(BOOTDIR)nvram # missing outVti inVti
#	$(BOOTDIR)vga # missing inportbxt
#	$(BOOTDIR)lp # missing outVti inVti
#	$(BOOTDIR)mfs # missing inportbxt
#	$(BOOTDIR)fd # missing inportbxt
#	$(BOOTDIR)mouse_demo # missing outVti inVti
#	$(BOOTDIR)avp # requires edit_cmdline that is currently internal

# Common
$(BOOTDIR):
	mkdir $(BOOTDIR); $(CHMOD) a+r+x $(BOOTDIR)

%.sp : %.s
	$(CPP) $(CPPFLAGS) $(INCLUDES) $^ > $@

$(OBJDIR)%.o : %.sp
	$(AS) $(ASFLAGS) -o $@ $^

$(OBJDIR)%.o : %.c
	$(CC) $(CCFLAGS) $(CPPFLAGS) $(INCLUDES) $(OUTPUT_OPTION) -c $^

$(BOOTDIR)%.sr : $(OBJDIR)%.nh
	$(SREC) -v -i $< $@

$(BOOTDIR)%.sr : $(OBJDIR)%.img
	$(SREC) -v -i $< $@

%.nh : %.elf
	$(OBJCOPY) -O binary $< $@

%.img : %.o
	$(OBJCOPY) -O binary $< $@

# Hello
$(OBJDIR)hello.elf : $(OBJDIR)hello.o
	$(LD) $(LDFLAGS) -Map=$(OBJDIR)hello.map -o $@ $^ $(LDLIBS)

hello $(BOOTDIR)hello : $(OBJDIR)hello.nh $(BOOTDIR)
	$(COPY) $< $@
	$(CHMOD) 664 $@

# Size
$(OBJDIR)size.elf : $(OBJDIR)size.o
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

size $(BOOTDIR)size : $(OBJDIR)size.nh $(BOOTDIR)
	$(COPY) $< $@
	$(CHMOD) 664 $@

# Yellow
$(OBJDIR)yellow.elf : $(OBJDIR)yellow.o
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

yellow $(BOOTDIR)yellow : $(OBJDIR)yellow.nh $(BOOTDIR)
	$(COPY) $< $@
	$(CHMOD) 664 $@

# Register test
regtest $(BOOTDIR)regtest : $(OBJDIR)regtest.img $(BOOTDIR)
	$(COPY) $< $@
	$(CHMOD) 664 $@

# Loops
$(OBJDIR)loops.elf : $(OBJDIR)loops1.o $(OBJDIR)loops2.o
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

loops $(BOOTDIR)loops : $(OBJDIR)loops.nh $(BOOTDIR)
	$(COPY) $< $@
	$(CHMOD) 664 $@

# Sum
$(OBJDIR)sum.elf : $(OBJDIR)sum.o
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

sum $(BOOTDIR)sum : $(OBJDIR)sum.nh $(BOOTDIR)
	$(COPY) $< $@
	$(CHMOD) 664 $@

# Memory test
$(OBJDIR)mtest.elf : $(OBJDIR)mtest.o
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

mtest $(BOOTDIR)mtest : $(OBJDIR)mtest.nh $(BOOTDIR)
	$(COPY) $< $@
	$(CHMOD) 664 $@

# Pic
$(OBJDIR)pic.elf : $(OBJDIR)pic.o
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

pic $(BOOTDIR)pic : $(OBJDIR)pic.nh $(BOOTDIR)
	$(COPY) $< $@
	$(CHMOD) 664 $@

# Sound
$(OBJDIR)sound.elf : $(OBJDIR)sound.o
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

sound $(BOOTDIR)sound : $(OBJDIR)sound.nh $(BOOTDIR)
	$(COPY) $< $@
	$(CHMOD) 664 $@

# NVRAM
$(OBJDIR)nvram.elf : $(OBJDIR)nvram.o
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

nvram $(BOOTDIR)nvram : $(OBJDIR)nvram.nh $(BOOTDIR)
	$(COPY) $< $@
	$(CHMOD) 664 $@

# VGA
$(OBJDIR)vga.elf : $(OBJDIR)vga.o
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

vga $(BOOTDIR)vga : $(OBJDIR)vga.nh $(BOOTDIR)
	$(COPY) $< $@
	$(CHMOD) 664 $@

# Time
$(OBJDIR)time.elf : $(OBJDIR)wait.o
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

time $(BOOTDIR)time : $(OBJDIR)time.nh $(BOOTDIR)
	$(COPY) $< $@
	$(CHMOD) 664 $@

# lp
$(OBJDIR)lp.elf : $(OBJDIR)lp.o
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

lp $(BOOTDIR)lp : $(OBJDIR)lp.nh $(BOOTDIR)
	$(COPY) $< $@
	$(CHMOD) 664 $@

# flash
$(OBJDIR)flash.elf : $(OBJDIR)flash.o
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

flash $(BOOTDIR)flash : $(OBJDIR)flash.nh $(BOOTDIR)
	$(COPY) $< $@
	$(CHMOD) 664 $@

# mfs
$(OBJDIR)mfs.elf : $(OBJDIR)mfs.o
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

mfs $(BOOTDIR)mfs : $(OBJDIR)mfs.nh $(BOOTDIR)
	$(COPY) $< $@
	$(CHMOD) 664 $@

# fd
$(OBJDIR)fd.elf : $(OBJDIR)fd.o
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

fd $(BOOTDIR)fd : $(OBJDIR)fd.nh $(BOOTDIR)
	$(COPY) $< $@
	$(CHMOD) 664 $@

# comtrol
$(OBJDIR)comtrol.elf : $(OBJDIR)comtrol.o
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

comtrol $(BOOTDIR)comtrol : $(OBJDIR)comtrol.nh $(BOOTDIR)
	$(COPY) $< $@
	$(CHMOD) 664 $@

# mouse demo
$(OBJDIR)mouse_demo.elf : $(OBJDIR)mouse_demo.o
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

mouse_demo $(BOOTDIR)mouse_demo : $(OBJDIR)mouse_demo.nh $(BOOTDIR)
	$(COPY) $< $@
	$(CHMOD) 664 $@

# avp
$(OBJDIR)avp.elf : $(OBJDIR)avp.o $(OBJDIR)vp.o $(OBJDIR)console.o
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

avp $(BOOTDIR)avp : $(OBJDIR)avp.nh $(BOOTDIR)
	$(COPY) $< $@
	$(CHMOD) 664 $@

clean :
	$(DELETE) *.o *~ \#*\# *.bak *.a core \
	$(OBJDIR)hello* \
	$(OBJDIR)size* \
	$(OBJDIR)yellow* \
	$(OBJDIR)loops* \
	$(OBJDIR)regtest* \
	$(OBJDIR)sum* \
	$(OBJDIR)mtest* \
	$(OBJDIR)pi*c \
	$(OBJDIR)sound* \
	$(OBJDIR)nvram* \
	$(OBJDIR)vga* \
	$(OBJDIR)time* \
	$(OBJDIR)lp* \
	$(OBJDIR)flash* \
	$(OBJDIR)mfs* \
	$(OBJDIR)fd* \
	$(OBJDIR)comtrol* \
	$(OBJDIR)mouse_demo* \
	$(OBJDIR)avp*