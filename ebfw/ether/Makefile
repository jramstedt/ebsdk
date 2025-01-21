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

CPPFLAGS	= $(CCFLAGS_GLOBAL) -fdollars-in-identifiers
CCFLAGS		= -c -Os -Wall -Wextra

C_SRCS	= DEC21040.c  am79c960.c  arp.c  bootp.c  eaddr.c  edevice.c  ethernet.c  ip.c  net_buff.c  netboot.c  netman.c  tftp.c  udp.c

.PHONY : all ether

all :
	$(MAKE) -f Makefile $(ALPHA_TOOLS) ether

ether : $(OBJDIR)ether.a

$(OBJDIR)%.o : %.c
	$(CC) $(CCFLAGS) $(CPPFLAGS) $(INCLUDES) $(OUTPUT_OPTION) -c $^

$(OBJDIR)ether.a : $(addprefix $(OBJDIR), $(C_SRCS:.c=.o))
	$(DELETE) $@
	$(AR) rcs $@ $^

clean :
	$(DELETE) *.o *~ \#*\# *.bak *.a core \
		$(addprefix $(OBJDIR), $(C_SRCS:.c=.o)) \
		$(OBJDIR)ether.a
