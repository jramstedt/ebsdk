CONFIG_DIR=../..
include $(CONFIG_DIR)/sdkmake.conf

include $(EB_BASE)/rules.gnu

AS		= as
AR		= ar
CC		= gcc
DELETE		= rm -f

ALPHA_TOOLS 	= \
	AS=alpha-linux-gnu-as \
	AR=alpha-linux-gnu-ar \
	LD=alpha-linux-gnu-ld \
	CC=alpha-linux-gnu-gcc \
	CPP=alpha-linux-gnu-cpp \
	OBJCOPY=alpha-linux-gnu-objcopy \
	OBJDUMP=alpha-linux-gnu-objdump

ASFLAGS		=
CCFLAGS		= $(CCFLAGS_GLOBAL)

TARGET_SRCS	= $(TARGET)_io.c $(TARGET)mem.c

S_SRCS	= asmstuff.s cstartup.s setjmp.s ulcd.s

C_SRCS 	= 28f008sa.c  29f040.c  acersio.c  am29v800.c  amdflash.c  assert.c  atof.c \
					beep.c  c8514.c  callback.c  calloc.c  ctype.c  exit.c  fatdrv.c  fileio.c  flash.c  floppy.c \
					fsboot.c  ftype.c  halp.c  ident.c  initdata.c  int.c  iobusini.c  isa_dma.c  kbd.c \
					mcheck.c  memt.c  memtest.c  mflash.c  p8514.c  pci.c  pflash.c  pr.c  printf.c  prtrace.c \
					puts.c  rand.c  rom.c  romhead.c  scanf.c  search.c  smc.c  smcc669.c  string.c  strtod.c  strtoul.c \
					sum.c  tga.c  time.c  vga.c  vgafont.c  xlate.c

# ???
#C_SRCS	+= fpu.c

.PHONY : all lib vars

all :
	$(MAKE) -f Makefile $(ALPHA_TOOLS) lib

lib : $(OBJDIR)lib.a $(OBJDIR)cstartup.o

vars:
	@echo TARGET is $(TARGET)
	@echo TARGETDIR is $(TARGETDIR)
	@echo OBJDIR is $(OBJDIR)
	@echo SRCDIR is $(SRCDIR)
	@echo OBJFILES is $(OBJFILES)


%.sp : %.s
	$(CPP) $(CPPFLAGS) $(INCLUDES) $^ > $@

$(OBJDIR)%.o : %.sp
	$(AS) $(ASFLAGS) $(INCLUDES) -o $@ $^

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
