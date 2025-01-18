CONFIG_DIR=../..
include $(CONFIG_DIR)/sdkmake.conf

include $(EB_BASE)/rules.gnu

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

CCFLAGS		= $(CCFLAGS_GLOBAL) -mcpu=$(CPUTYPE) -mtune=$(CPUTYPE) -Os -Wall

C_SRCS	= bios_dbg.c  biosem.c  cxx.c  dpml.c  fflush.c  sflash.c  stdlib.c  stub_pci.c

.PHONY : all stubs

all :
	$(MAKE) -f Makefile $(ALPHA_TOOLS) stubs

stubs : $(OBJDIR)stubs.a

$(OBJDIR)%.o : %.c
	$(CC) $(CCFLAGS) $(CPPFLAGS) $(INCLUDES) $(OUTPUT_OPTION) -c $^

$(OBJDIR)stubs.a : $(addprefix $(OBJDIR), $(C_SRCS:.c=.o))
	$(DELETE) $@
	$(AR) rcs $@ $^

clean :
	$(DELETE) *.o *~ \#*\# *.bak *.a core \
		$(addprefix $(OBJDIR), $(C_SRCS:.c=.o)) \
		$(OBJDIR)stubs.a
