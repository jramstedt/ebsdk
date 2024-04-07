CONFIG_DIR=../../../..
include $(CONFIG_DIR)/sdkmake.conf

CC		= cc
AR		= ar
CFLAGS		= -g -I. -I../include -Wall
LFLAGS		= -lc
LINT		= /usr/bin/lint
LINTFLAGS	=  -1 -wp -av -n -I. -I../include

SRCS = romhead.c disassm.c

DEPDIR := .deps
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

commlib.a : $(SRCS:.c=.o)
	-rm -f $@
	$(AR) rcs $@ $^

install : commlib.a

lint :	
	$(LINT) $(LINTFLAGS) $(INCLUDES) $(SRCS)

clean :
	rm -f *.o *~ \#*\# *.bak *.a core

COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c

%.o : %.c
%.o : %.c $(DEPDIR)/%.d | $(DEPDIR)
	$(COMPILE.c) $(OUTPUT_OPTION) $<

$(DEPDIR): ; @mkdir -p $@

DEPFILES := $(SRCS:%.c=$(DEPDIR)/%.d)
$(DEPFILES):

include $(wildcard $(DEPFILES))
