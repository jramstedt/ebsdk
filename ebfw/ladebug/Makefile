CONFIG_DIR=../..
include $(CONFIG_DIR)/sdkmake.conf

include $(EB_BASE)/rules.gnu

AS				= as
AR				= ar
LC				= $(shell echo '$1' | tr '[:upper:]' '[:lower:]' )
CC				= gcc
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

ASFLAGS		=  -m$(call LC,$(CPUTYPE))
CPPFLAGS	= $(CCFLAGS_GLOBAL) -fdollars-in-identifiers
CCFLAGS		= -c -Os -std=c99 -Wpedantic -Wall -Wextra -fanalyzer -fno-builtin -ffreestanding

S_SRCS	= kutil.s  ladbxusr.s

C_SRCS	= bptable.c  kernel.c  pkthandl.c  pktutil.c  readloop.c

.PHONY : all ladebug

all :
	$(MAKE) -f Makefile $(ALPHA_TOOLS) ladebug

ladebug : $(OBJDIR)ladebug.a

%.sp : %.s
	$(CPP) $(CPPFLAGS) $(INCLUDES) $^ > $@

$(OBJDIR)%.o : %.sp
	$(AS) $(ASFLAGS) $(INCLUDES) -o $@ $^

$(OBJDIR)%.o : %.c
	$(CC) $(CCFLAGS) $(CPPFLAGS) $(INCLUDES) $(OUTPUT_OPTION) -c $^

$(OBJDIR)ladebug.a : $(addprefix $(OBJDIR), $(S_SRCS:.s=.o) $(C_SRCS:.c=.o))
	$(DELETE) $@
	$(AR) rcs $@ $^

clean :
	$(DELETE) *.o *~ \#*\# *.bak *.a core \
		$(addprefix $(OBJDIR), $(S_SRCS:.s=.o) $(C_SRCS:.c=.o)) \
		$(OBJDIR)ladebug.a
