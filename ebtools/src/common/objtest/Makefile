CONFIG_DIR=../../../..
include $(CONFIG_DIR)/sdkmake.conf

CC				= cc
CFLAGS		= -g -I. -I../include -DLANGUAGE_C
LFLAGS		= -lc
LINT			= /usr/bin/lint
LINTFLAGS	=  -1 -wp -av -n -I. -I../include

COMMONLIB	= ../common/commlib.a

CCHECK_SRCS = ccheck.c
NEWSTRIP_SRCS = newstrip.c
SRCS = $(CCHECK_SRCS) $(NEWSTRIP_SRCS)

DEPDIR := .deps
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

tools :	ccheck newstrip

ccheck : $(CCHECK_SRCS:.c=.o) $(COMMONLIB)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OUTPUT_OPTION) $(TARGET_ARCH) $^

newstrip : $(NEWSTRIP_SRCS:.c=.o) $(COMMONLIB)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OUTPUT_OPTION) $(TARGET_ARCH) $^

install : ccheck newstrip
#		cp -fp $^ $(EB_TOOLBOX)/.
#		chmod 775 $(addprefix $(EB_TOOLBOX)/, $^)

lint :
	$(LINT) $(LINTFLAGS) $(INCLUDES) $(SRCS)

clean :
		rm -f *.o *~ \#*\# *.bak *.a core \
		ccheck newstrip

COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c

%.o : %.c
%.o : %.c $(DEPDIR)/%.d | $(DEPDIR)
		$(COMPILE.c) $(OUTPUT_OPTION) $<

$(DEPDIR): ; @mkdir -p $@

DEPFILES := $(SRCS:%.c=$(DEPDIR)/%.d)
$(DEPFILES):

include $(wildcard $(DEPFILES))