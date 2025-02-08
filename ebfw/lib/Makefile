CONFIG_DIR=../..
include $(CONFIG_DIR)/sdkmake.conf

include $(EB_BASE)/rules.gnu

AS		= as
AR		= ar
LC		= $(shell echo '$1' | tr '[:upper:]' '[:lower:]' )
CC		= gcc
CPP				= cpp
DELETE		= rm -f

ALPHA_TOOLS 	= \
	AS=alpha-linux-gnu-as \
	AR=alpha-linux-gnu-ar \
	LD=alpha-linux-gnu-ld \
	CC=alpha-linux-gnu-gcc \
	CPP=alpha-linux-gnu-cpp \
	OBJCOPY=alpha-linux-gnu-objcopy \
	OBJDUMP=alpha-linux-gnu-objdump

ASFLAGS		= -m$(call LC,$(CPUTYPE))
CPPFLAGS	= $(CCFLAGS_GLOBAL) -fdollars-in-identifiers
CCFLAGS		= -c -Os -Wall -Wextra -fno-builtin -ffreestanding

TARGET_SRCS	= $(TARGET)_io.c $(TARGET)mem.c

S_SRCS	= asmstuff.s ulcd.s setjmp.s

C_SRCS	= iobusini.c  beep.c  c8514.c  time.c  acersio.c  initdata.c  fsboot.c  floppy.c \
					fileio.c  fatdrv.c  ident.c  int.c  kbd.c  memtest.c  p8514.c  pr.c  printf.c \
					scanf.c  puts.c  strtoul.c  strtod.c  atof.c  rom.c  romhead.c  search.c  vga.c \
					vgafont.c  xlate.c  pci.c  halp.c  flash.c  mflash.c  pflash.c  28f008sa.c  29f040.c \
					tga.c  string.c  prtrace.c  ctype.c  calloc.c  exit.c  ftype.c  rand.c  assert.c \
					smc.c  sum.c  mcheck.c  isa_dma.c  smcc669.c  am29v800.c  amdflash.c  memt.c  callback.c \
					fpu.c

.PHONY : all lib vars

all :
	$(MAKE) -f Makefile $(ALPHA_TOOLS) lib

lib : $(OBJDIR)lib.a $(OBJDIR)cstartup.o

vars:
	@echo TARGET is $(TARGET)
	@echo TARGETDIR is $(TARGETDIR)
	@echo OBJDIR is $(OBJDIR)
	@echo SRCDIR is $(SRCDIR)


%.sp : %.s
	$(CPP) $(CPPFLAGS) $(INCLUDES) $^ > $@

$(OBJDIR)%.o : %.sp
	$(AS) $(ASFLAGS) -o $@ $^

$(OBJDIR)%.o : %.c
	$(CC) $(CCFLAGS) $(CPPFLAGS) $(INCLUDES) $(OUTPUT_OPTION) -c $^

$(OBJDIR)initdata.o : initdata.c
	$(CC) $(CCFLAGS) $(CPPFLAGS) $(INCLUDES) -DDBMENTRY=0x$(DBMENTRY) $(OUTPUT_OPTION) -c $<

$(OBJDIR)memt.o : memt.c
	$(CC) $(CCFLAGS) $(CPPFLAGS) $(INCLUDES) -DRPCC_TIMING $(OUTPUT_OPTION) -c $^

$(OBJDIR)lib.a : $(addprefix $(OBJDIR), $(S_SRCS:.s=.o) $(TARGET_SRCS:.c=.o) $(C_SRCS:.c=.o))
	$(DELETE) $@
	$(AR) rcs $@ $^

clean :
	$(DELETE) *.o *~ \#*\# *.bak *.a core \
		$(S_SRCS:.s=.sp) \
		$(addprefix $(OBJDIR), $(S_SRCS:.s=.o) $(TARGET_SRCS:.c=.o) $(C_SRCS:.c=.o)) \
		$(OBJDIR)cstartup.o \
		$(OBJDIR)lib.a \
		$(addprefix $(OBJDIR), $(C_SRCS:.c=.i) $(C_SRCS:.c=.s) $(C_SRCS:.c=.cpp) $(TARGET_SRCS:.c=.i) $(TARGET_SRCS:.c=.s) $(TARGET_SRCS:.c=.cpp))
