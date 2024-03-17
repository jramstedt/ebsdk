CONFIG_DIR=../../../..
include $(CONFIG_DIR)/sdkmake.conf

CC				= cc
CFLAGS		= -g -I. -I../include
LFLAGS		= 
LINT			= /usr/bin/lint
LINTFLAGS	=  -1 -wp -av -n -I. -I../include

C_SRCS = \
	ev6.c  \
	ev6_cbox.c \
	ev6_chain.c \
	ev6_srom.c \
	ev6_ic.c

IC4MAT_SRCS	= ic4mat.c $(C_SRCS)
CHAIN_SRCS	= chain2cbox.c $(C_SRCS)
SRCS = $(IC4MAT_SRCS) $(CHAIN_SRCS)

DEPDIR := .deps
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

tools :	ic4mat

ic4mat : $(IC4MAT_SRCS:.c=.o)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OUTPUT_OPTION) $(TARGET_ARCH) $^

chain2cbox : $(CHAIN_SRCS:.c=.o)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OUTPUT_OPTION) $(TARGET_ARCH) $^

install : ic4mat
		cp -fp $< $(EB_TOOLBOX)/.
		chmod 775 $(EB_TOOLBOX)/$<

lint :$(IC4MAT_SRCS) $(CHAIN_SRCS)
	$(LINT) $(LINTFLAGS) $(INCLUDES) $(CHAIN_SRCS)
	$(LINT) $(LINTFLAGS) $(INCLUDES) $(IC4MAT_SRCS)

clean :
		rm -f *.o *~ \#*\# *.bak *.a core \
		ic4mat chain2cbox

COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c

%.o : %.c
%.o : %.c $(DEPDIR)/%.d | $(DEPDIR)
		$(COMPILE.c) $(OUTPUT_OPTION) $<

$(DEPDIR): ; @mkdir -p $@

DEPFILES := $(SRCS:%.c=$(DEPDIR)/%.d)
$(DEPFILES):

include $(wildcard $(DEPFILES))
