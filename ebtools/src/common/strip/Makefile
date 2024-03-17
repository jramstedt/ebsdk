CONFIG_DIR=../../../..
include $(CONFIG_DIR)/sdkmake.conf

CC				= cc
CFLAGS		= -g -I. -I../include -DLANGUAGE_C
LFLAGS		= -lc
LINT		= /usr/bin/lint
LINTFLAGS 	=  -1 -wp -av -n -I. -I../include

COMMONLIB	= ../common/commlib.a

CSTRIP_SRCS = cstrip.c
ASTRIP_SRCS = astrip.c
SRCS = $(CSTRIP_SRCS) $(ASTRIP_SRCS)

DEPDIR := .deps
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

tools :	cstrip astrip

cstrip : $(CSTRIP_SRCS:.c=.o) $(COMMONLIB)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OUTPUT_OPTION) $(TARGET_ARCH) $^

astrip : $(ASTRIP_SRCS:.c=.o) $(COMMONLIB)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OUTPUT_OPTION) $(TARGET_ARCH) $^

install : cstrip astrip
		cp -fp $^ $(EB_TOOLBOX)/.
		chmod 775 $(addprefix $(EB_TOOLBOX)/, $^)

lint :
	$(LINT) $(LINTFLAGS) $(INCLUDES) $(SRCS)

clean :
		rm -f *.o *~ \#*\# *.bak *.a core \
		cstrip astrip

COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c

%.o : %.c
%.o : %.c $(DEPDIR)/%.d | $(DEPDIR)
		$(COMPILE.c) $(OUTPUT_OPTION) $<

$(DEPDIR): ; @mkdir -p $@

DEPFILES := $(SRCS:%.c=$(DEPDIR)/%.d)
$(DEPFILES):

include $(wildcard $(DEPFILES))
