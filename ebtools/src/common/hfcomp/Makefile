CONFIG_DIR=../../../..
include $(CONFIG_DIR)/sdkmake.conf

include $(EB_BASE)/rules.gnu

AS				= as
LC				= $(shell echo '$1' | tr '[:upper:]' '[:lower:]' )
LD				= ld
CC				= gcc
CPP				= cpp
LINT			= /usr/bin/lint
OBJCOPY		= objcopy
OBJDUMP		= objdump
SYSGEN		= $(TOOLBOX)/sysgen

ALPHA_TOOLS 	= \
	AS=alpha-linux-gnu-as \
	AR=alpha-linux-gnu-ar \
	LD=alpha-linux-gnu-ld \
	CC=alpha-linux-gnu-gcc \
	CPP=alpha-linux-gnu-cpp \
	OBJCOPY=alpha-linux-gnu-objcopy \
	OBJDUMP=alpha-linux-gnu-objdump

INCLUDES	= -I. -I../include

ASFLAGS		= -m$(call LC,$(CPU))
CPPFLAGS	= -DPVCCHECK $(HEADER_DEFS) -fdollars-in-identifiers
CCFLAGS		= -mcpu=$(CCPU) -c -Os -Wall -Wextra -fno-builtin -ffreestanding
CFLAGS		= -g -Wall -Wextra
LDFLAGS	  = --orphan-handling=warn -nostdlib -T$(EB_BASE)/palcode/palcode.ld -z noexecstack --no-dynamic-linker -static
LINTFLAGS	=  -1 -wp -av -n $(INCLUDES)

HEADER_DEFS 	= \
	-DDECOMP_PALBASE=0x$(DECOMP_PALBASE) \
	-DDECOMP_BASE=0x$(DECOMP_BASE)
#	-DDECOMP_COMPRESSED=0x$(DECOMP_COMPRESSED)

SRCS  		= bits.c deflate.c hfcomp.c inflate.c trees.c

DEPDIR 		:= .deps
DEPFLAGS	= -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

INSTALL_TARGETS	= hfcomp decmp064.img decmp164.img decmp264.img

.PHONY : all install clean

all : hfcomp decomp064 decomp164 decomp264

# hcomp
hfcomp : $(SRCS)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(OUTPUT_OPTION) $(TARGET_ARCH) $^

### Decompressor image
MAKEFILE	= Makefile

CPU_064		= 21064
CPU_164		= 21164
CPU_264		= 21264
CPU		= $(CPU_$(TYPE))

CCPU_064	= $(CPU)
CCPU_164	= $(CPU)a
CCPU_264	= $(CPU)
CCPU		= $(CCPU_$(TYPE))

CPU_DTBMISS_064	= 9E0
CPU_DTBMISS_164	= 200
CPU_DTBMISS_264	= 300
CPU_DTBMISS	= $(CPU_DTBMISS_$(TYPE))

decomp% :
	$(MAKE) -f $(MAKEFILE) $(ALPHA_TOOLS) TYPE=$* decmpimg

decmpimg : cstartup.o decmp$(TYPE).img upal$(TYPE).lis

### Decompressor

cstartup.sp : cstartup.s
	$(CPP) $(CPPFLAGS) $(INCLUDES) -DNO_INITDATA -DNO_EXITDATA $^ > $@

cstartup.o : cstartup.sp
	$(AS) $(ASFLAGS) -o $@ $^

decmp$(TYPE).o : decomp.c
	$(CC) $(CCFLAGS) $(CPPFLAGS) -DROM -o $@ $^

inf_s$(TYPE).o : inflate.c
	$(CC) $(CCFLAGS) $(CPPFLAGS) -DROM -o $@ $^

dbmmn$(TYPE).o : dbmmain.c
	$(CC) $(CCFLAGS) $(CPPFLAGS) -DROM -o $@ $^

decmp$(TYPE).elf : decmp$(TYPE).o inf_s$(TYPE).o dbmmn$(TYPE).o
	$(LD) -Map=decmp$(TYPE).map -nostdlib -Tdecomp.ld -z noexecstack -s --no-dynamic-linker -static -o $@ $^

### micropal

# Preprocesses micropal.s to upal$(TYPE).sp
upal$(TYPE).sp : micropal.s
	$(CPP) $(CPPFLAGS) $(INCLUDES) -DDC$(CPU) -DASSEMBLY_CODE $^ > $@

upal$(TYPE).o : upal$(TYPE).sp
	$(AS) $(ASFLAGS) -o $@ $^

upal$(TYPE).elf : upal$(TYPE).o
	$(LD) $(LDFLAGS)  -Map=upal$(TYPE).map -o $@ $^

# Strip all header sections (text and data)
upal$(TYPE).nh : upal$(TYPE).elf
	$(OBJCOPY) -O binary $< $@

upal$(TYPE).ent :
	echo 0 uPalReset > $@
	echo $(CPU_DTBMISS) PalDtbMiss >> $@

upal$(TYPE).lis : upal$(TYPE).o
#	$(ALIST) $< > $@
	$(OBJDUMP) -x $< > $@

upal$(TYPE).map : upal$(TYPE).o
#	$(ALIST) -m $< > $@
	$(OBJDUMP) -x $< | grep -i "pvc\\$$" > $@

### decmp

decmp$(TYPE).nh : decmp$(TYPE).elf
	$(OBJCOPY) -O binary $< $@

decmp$(TYPE).img : upal$(TYPE).nh decmp$(TYPE).nh pvc$(TYPE).pvc
	$(SYSGEN) -v \
		-e0	-s upal$(TYPE).nh \
		-eb00	-s decmp$(TYPE).nh \
		-o $@
#	$(PVC) < pvc$(TYPE).pvc

### pvc
pvc$(TYPE).pvc : upal$(TYPE).nh upal$(TYPE).ent upal$(TYPE).map
	echo set code_file upal$(TYPE).nh > $@
	echo set entry_file upal$(TYPE).ent >> $@
	echo set cpu $(CPU) >> $@
	echo go >> $@
	echo quit >> $@

install : all
	cp -fp $(INSTALL_TARGETS) $(EB_TOOLBOX)/.
	chmod 775 $(addprefix $(EB_TOOLBOX)/, $(INSTALL_TARGETS))

lint :
	$(LINT) $(LINTFLAGS) $(INCLUDES) $(SRCS)

clean :
	rm -f *.o *~ \#*\# *.bak *.a core \
	*.nh decmp*.elf upal*.elf *.lis *.dis \
	*.map *.pvc *.ent *.sp \
	decmp*.img \
	hfcomp

# Dependencies
#DEPDIR := .deps
#DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

#COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c

#%.o : %.c
#%.o : %.c $(DEPDIR)/%.d | $(DEPDIR)
#		$(COMPILE.c) $(OUTPUT_OPTION) $<

#$(DEPDIR): ; @mkdir -p $@

#DEPFILES := $(SRCS:%.c=$(DEPDIR)/%.d)
#$(DEPFILES):

#include $(wildcard $(DEPFILES))
