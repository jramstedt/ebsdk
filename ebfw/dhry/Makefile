CONFIG_DIR=../..
include $(CONFIG_DIR)/sdkmake.conf

include $(EB_BASE)/rules.gnu

CC				= gcc
LD				= ld
COPY			= cp -p
CHMOD			= chmod
DELETE		= rm -f
OBJCOPY		= objcopy
SREC			= $(TOOLBOX)/srec

CPPFLAGS	= $(CCFLAGS_GLOBAL) -fdollars-in-identifiers -DDEBUG_MONITOR -DREG=register -DLOOPS=50000000 -DHZ=1 -DTIME
CCFLAGS		= -c -Wall -Wextra -fno-builtin -ffreestanding -funsigned-char
LDFLAGS		= --orphan-handling=warn -L$(OBJDIR) -T ../dbmboot.ld -z noexecstack -s --no-dynamic-linker -static
LDLIBS		= -l:lib.a -l:stubs.a -lc

ALPHA_TOOLS = \
	AS=alpha-linux-gnu-as \
	AR=alpha-linux-gnu-ar \
	LD=alpha-linux-gnu-ld \
	CC=alpha-linux-gnu-gcc \
	CPP=alpha-linux-gnu-cpp \
	OBJCOPY=alpha-linux-gnu-objcopy \
	OBJDUMP=alpha-linux-gnu-objdump

.PHONY : all dhry clean

all :
	$(MAKE) -f Makefile $(ALPHA_TOOLS) dhry

dhry : \
	$(BOOTDIR)dhryV1 \
	$(BOOTDIR)dhryV2 \
	$(BOOTDIR)dhryV1.sr \
	$(BOOTDIR)dhryV2.sr

# Common
$(BOOTDIR):
	mkdir $(BOOTDIR); $(CHMOD) a+r+x $(BOOTDIR)

$(BOOTDIR)%.sr : $(OBJDIR)%.nh
	$(SREC) -v -i $< $@

%.nh : %.elf
	$(OBJCOPY) -O binary $< $@

# V2
$(OBJDIR)dhryV2%.o : dhryV2%.c
	$(CC) -O3 $(CCFLAGS) $(CPPFLAGS) $(INCLUDES) $(OUTPUT_OPTION) -c $^

$(OBJDIR)dhryV2.elf : $(OBJDIR)dhryV2_1.o $(OBJDIR)dhryV2_2.o
	$(LD) -O3 $(LDFLAGS) -Map=$(OBJDIR)dhryV2.map -o $@ $^ $(LDLIBS)

dhryV2 $(BOOTDIR)dhryV2 : $(OBJDIR)dhryV2.nh $(BOOTDIR)
	$(COPY) $< $@
	$(CHMOD) 664 $@

# V1
$(OBJDIR)dhryV1.o : dhryV1.c
	$(CC) -O2 $(CCFLAGS) $(CPPFLAGS) $(INCLUDES) $(OUTPUT_OPTION) -c $^

$(OBJDIR)dhryV1.elf : $(OBJDIR)dhryV1.o
	$(LD) -O2 $(LDFLAGS) -Map=$(OBJDIR)dhryV1.map -o $@ $^ $(LDLIBS)

dhryV1 $(BOOTDIR)dhryV1 : $(OBJDIR)dhryV1.nh $(BOOTDIR)
	$(COPY) $< $@
	$(CHMOD) 664 $@

clean :
	$(DELETE) *.o *~ \#*\# *.bak *.a core \
	$(OBJDIR)dhryV1* \
	$(OBJDIR)dhryV2* \
