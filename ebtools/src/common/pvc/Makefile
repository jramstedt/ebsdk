CONFIG_DIR=../../../..
include $(CONFIG_DIR)/sdkmake.conf

# uses -std=c89 to allow "inline" keyword in variable name.
CFLAGS		= -DEV6 -g3 -std=c89

SRCS = pvc.c body.c common.c memory.c rest.c sched.c ev4.c ev5.c ev6.c ev6_rest.c

pvc : $(SRCS:.c=.o)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OUTPUT_OPTION) $(TARGET_ARCH) $^

install : pvc
		cp -fp $< $(EB_TOOLBOX)/.
		chmod 775 $(EB_TOOLBOX)/$<

lint :
	$(LINT) $(LINTFLAGS) $(INCLUDES) $(SRCS)

clean :
		rm -f *.o *~ \#*\# *.bak *.a core \
		pvc

COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c

%.o : %.c
%.o : %.c $(DEPDIR)/%.d | $(DEPDIR)
		$(COMPILE.c) $(OUTPUT_OPTION) $<

$(DEPDIR): ; @mkdir -p $@

DEPFILES := $(SRCS:%.c=$(DEPDIR)/%.d)
$(DEPFILES):

include $(wildcard $(DEPFILES))
