CONFIG_DIR=../../../..
include $(CONFIG_DIR)/sdkmake.conf

CC				= cc
# uses -std=c89 to allow "asm" keyword in variable name.
CFLAGS		= -g -I. -I../include -std=c89 -DSROM_ADVANCED_OPTIONS
LFLAGS		= -lc
LINT			= /usr/bin/lint
LINTFLAGS	=  -1 -wp -av -n -I. -I../include

SRCS = srom.c

DEPDIR := .deps
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

tools : srom

srom : $(SRCS:.c=.o)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OUTPUT_OPTION) $(TARGET_ARCH) $^

install : srom
		cp -fp $< $(EB_TOOLBOX)/.
		chmod 775 $(EB_TOOLBOX)/$<

lint :
	$(LINT) $(LINTFLAGS) $(INCLUDES) $(SRCS)

clean :
		rm -f *.o *~ \#*\# *.bak *.a core \
		srom

COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c

%.o : %.c
%.o : %.c $(DEPDIR)/%.d | $(DEPDIR)
		$(COMPILE.c) $(OUTPUT_OPTION) $<

$(DEPDIR): ; @mkdir -p $@

DEPFILES := $(SRCS:%.c=$(DEPDIR)/%.d)
$(DEPFILES):

include $(wildcard $(DEPFILES))
