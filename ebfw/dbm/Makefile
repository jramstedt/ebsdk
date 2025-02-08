CONFIG_DIR=../..
include $(CONFIG_DIR)/sdkmake.conf

include $(EB_BASE)/rules.gnu

AS			= as
LC			= $(shell echo '$1' | tr '[:upper:]' '[:lower:]' )
CC			= gcc
LD			= ld
CPP			= cpp
COPY		= cp -p
CHMOD		= chmod
DELETE	= rm -f
OBJCOPY	= objcopy
SYSGEN	= $(TOOLBOX)/sysgen

ASFLAGS		= -m$(call LC,$(CPUTYPE))
CPPFLAGS	= $(CCFLAGS_GLOBAL) -fdollars-in-identifiers
CCFLAGS		= -c -Os -Wall -Wextra -fno-builtin -ffreestanding
ifdef EB_DISABLE_GRAPHICS
CCFLAGS		+= -DDISABLE_GRAPHICS
endif
LDFLAGS			= --orphan-handling=warn -T elf-coff.ld -s --no-dynamic-linker -static
LDLIBS			= -lc

ALPHA_TOOLS 	= \
	AS=alpha-linux-gnu-as \
	AR=alpha-linux-gnu-ar \
	LD=alpha-linux-gnu-ld \
	CC=alpha-linux-gnu-gcc \
	CPP=alpha-linux-gnu-cpp \
	OBJCOPY=alpha-linux-gnu-objcopy \
	OBJDUMP=alpha-linux-gnu-objdump

default_BUILD_TARGETS	= \
	$(BOOTDIR)$(TARGET)dbm.img \
	$(BOOTDIR)$(TARGET)dbm.cmp \
	$(BOOTDIR)$(TARGET)dbm.rom \
	$(BOOTDIR)$(TARGET)fsb.cmp \
	$(BOOTDIR)$(TARGET)fsb.rom \
	$(BOOTDIR)$(TARGET)fsb.img \
	$(BOOTDIR)$(TARGET)sft.img \
	$(BOOTDIR)$(TARGET)dbm.sr  \
	$(BOOTDIR)$(TARGET)dbm.hex \
	$(BOOTDIR)$(TARGET)fsb.sr  \
	$(BOOTDIR)$(TARGET)fsb.hex \
	$(OBJDIR)sft.sr \
	$(OBJDIR)sft.hex

ifdef $(TARGET)_BUILD_TARGETS
BUILD_TARGETS	= $($(TARGET)_BUILD_TARGETS)
else
BUILD_TARGETS	= $(default_BUILD_TARGETS)
endif

ifdef EB_DEBUG_BIOSEM
BIOSEMLIB = $(COMMONLIB)$(DBGDIR)/biosem.a
else
BIOSEMLIB = $(COMMONLIB)$(RELDIR)/biosem.a
endif

LIBS	= \
	$(OBJDIR)ether.a\
	$(OBJDIR)ladebug.a \
	$(BIOSEMLIB) \
	$(OBJDIR)lib.a \
	$(OBJDIR)stubs.a

TARGET_SRCS = $(TARGET)mck.c

S_SRCS	= dbmentry.s

C_SRCS	= main.c  dis.c  ffexec.c  cmd.c  console.c  uart.c  sromport.c  io_port.c  file.c  malloc.c  rtc.c  mouse.c  xmodem.c

FSB_SRCS = fsbmain.c  file.c  malloc.c  rtc.c

OBJFILES	= $(C_SRCS:%.c=$(OBJDIR)%.o) $(TARGET_SRCS:%.c=$(OBJDIR)%.o) $(OBJDIR)ladbxusr.o $(OBJDIR)dpml.o

FSBOBJFILES	= $(FSB_SRCS:%.c=$(OBJDIR)%.o)

ifdef EB_DEBUG_FSBOOT
FSB_DEBUG_SRCS = console.c  io_port.c  uart.c  sromport.c
FSBOBJFILES += $(FSB_DEBUG_SRCS:%.c=$(OBJDIR)%.o)
endif

default_PALCODE	= $(OBJDIR)osfpal
dp264_PALCODE		= ../palcode/dp264/osfpal_dbm.exe
clubs_PALCODE		= ../palcode/clubs/osfpal_dbm.exe
ifdef $(TARGET)_PALCODE
PALCODE					= $($(TARGET)_PALCODE)
else
PALCODE					= $(default_PALCODE)
endif

default_SOFTBOOTADR	= 900000
ifdef $(TARGET)_SOFTBOOTADR
SOFTBOOTADR					= $($(TARGET)_SOFTBOOTADR)
else
SOFTBOOTADR					= $(default_SOFTBOOTADR)
endif

default_DBM_SYSGEN	= -e$(PALBASE) $(PALCODE) -e$(DBMENTRY) -s $(OBJDIR)dbm.nh
dp264_DBM_SYSGEN		= -e$(PALBASE) -s $(PALCODE) -e$(DBMENTRY) -s $(OBJDIR)dbm.nh 
clubs_DBM_SYSGEN		= -e$(PALBASE) -s $(PALCODE) -e$(DBMENTRY) -s $(OBJDIR)dbm.nh
ifdef $(TARGET)_DBM_SYSGEN
DBM_SYSGEN					= $($(TARGET)_DBM_SYSGEN)
else
DBM_SYSGEN					= $(default_DBM_SYSGEN)
endif

# TODO getver.c  

.PHONY : all dbm vars

all :
	$(MAKE) -f Makefile $(ALPHA_TOOLS) dbm

dbm : $(BUILD_TARGETS)

vars:
	@echo OBJFILES is $(OBJFILES)
	@echo BUILD_TARGETS is $(BUILD_TARGETS)

# build targets

$(BOOTDIR):
	mkdir $(BOOTDIR); $(CHMOD) a+r+x $(BOOTDIR)

$(BOOTDIR)$(TARGET)% : $(OBJDIR)% $(BOOTDIR)
	$(COPY) $< $@
	$(CHMOD) 664 $@

# targets

$(OBJDIR)dbm.img : $(PALCODE) $(OBJDIR)dbm.nh
	$(SYSGEN) -v $(DBM_SYSGEN) -o $@

#

%.sp : %.s
	$(CPP) $(CPPFLAGS) $(INCLUDES) $^ > $@

$(OBJDIR)%.o : %.sp
	$(AS) $(ASFLAGS) -o $@ $^

$(OBJDIR)%.o : %.c
	$(CC) $(CCFLAGS) $(CPPFLAGS) $(INCLUDES) $(OUTPUT_OPTION) -c $^

$(OBJDIR)%.nh : $(OBJDIR)%.elf
	$(OBJCOPY) -O binary $< $@

$(OBJDIR)crt0.o : $(OBJDIR)dbmentry.o
	$(COPY) $< $(OBJDIR)crt0.o

$(OBJDIR)dbmver.o : version.c
	$(CC) $(CCFLAGS) $(CPPFLAGS) $(INCLUDES) $(OUTPUT_OPTION) -c $^

$(OBJDIR)dbm.elf : $(OBJDIR)crt0.o $(OBJFILES) $(LIBS) $(OBJDIR)dbmver.o
	$(LD) -Ttext-segment=$(DBMENTRY) -z noexecstack $(LDFLAGS) -Map=$(OBJDIR)dbm.map -o $@ $^ $(LDLIBS)

# https://web.archive.org/web/20070226182734/http://h30097.www3.hp.com:80/docs/base_doc/DOCUMENTATION/V50_HTML/MAN/MAN1/0241____.HTM
# $(LD) -m -o $(OBJDIR)dbm.out -N -T $(DBMENTRY) -taso -non_shared -tr -h$(OBJDIR) -B $(OBJFILES) $(LIBS) $(OBJDIR)dbmver.$(OBJ) -lm > $(OBJDIR)dbm.map
#  -tr ??????????????????????
#  -h  ??

$(OBJDIR)%.nh : $(OBJDIR)%.elf
	$(OBJCOPY) -O binary $< $@

clean :
	$(DELETE) *.o *~ \#*\# *.bak *.a core \
		$(S_SRCS:.s=.sp) \
		$(addprefix $(OBJDIR), $(S_SRCS:.s=.o) $(C_SRCS:.c=.o) $(TARGET_SRCS:.c=.o) $(FSB_SRCS:.c=.o)) \
		$(OBJDIR)crt0.o \
		$(OBJDIR)dbm.nh \
		$(OBJDIR)dbm.img \
		$(OBJDIR)dbm.rom \
		$(OBJDIR)dbm.cmp \
		$(OBJDIR)dbm.sr \
		$(OBJDIR)dbm.hex \
		$(OBJDIR)dbm.dis \
		$(OBJDIR)dbm.out \
		$(OBJDIR)dbm.map \
		$(OBJDIR)dbmver.o \
		$(OBJDIR)fsb.nh \
		$(OBJDIR)fsb.img \
		$(OBJDIR)fsb.rom \
		$(OBJDIR)fsb.cmp \
		$(OBJDIR)fsb.sr \
		$(OBJDIR)fsb.hex \
		$(OBJDIR)fsb.dis \
		$(OBJDIR)fsb.out \
		$(OBJDIR)fsb.map \
		$(OBJDIR)fsbver.o \
		$(OBJDIR)sft.nh \
		$(OBJDIR)sft.dis \
		$(OBJDIR)sft.out \
		$(OBJDIR)sft.map \
		$(OBJDIR)sft.sr \
		$(OBJDIR)sft.hex \
		$(OBJDIR)getver \
		$(OBJDIR)getver.o \
		$(OBJDIR)llib-ldbm.ln \
		$(OBJDIR)timestmp.fw
