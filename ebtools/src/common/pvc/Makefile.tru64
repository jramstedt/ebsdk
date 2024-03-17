#
# $log$
CONFIG_DIR=../../../..
include $(CONFIG_DIR)/sdkmake.conf


TOOLBOX	    	= $(EB_TOOLBOX)

CFLAGS		= -DEV6 -g3

LINT		= /usr/bin/lint
LINTFLAGS 	=  -1 -wp -av -Xdollar -n -I. -I../include


SFILES		= pvc.c body.c common.c memory.c rest.c sched.c ev4.c ev5.c  ev6.c ev6_rest.c

OBJS		= $(SFILES:.c=.o)

tools:		pvc

pvc:		$(OBJS)
		$(CC) -DEV6 -o pvc $(OBJS)

lint:	
	$(LINT) $(LINTFLAGS) $(INCLUDES) $(SFILES)

install:	pvc
		cp -fp pvc $(TOOLBOX)
		chmod 775 $(TOOLBOX)/pvc

depend: $(SFILES)
		makedepend -f $(MAKEFILE) -- $(CFLAGS) -- $(SFILES)

clean:
		rm -f *~ \#*\# *.bak *.a core\
		$(OBJS) pvc

# DO NOT DELETE THIS LINE -- make depend depends on it.

pvc.o: /usr/include/signal.h /usr/include/standards.h
pvc.o: /usr/include/sys/siginfo.h /usr/include/sys/sysmisc.h
pvc.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
pvc.o: /usr/include/sys/select.h /usr/include/machine/signal.h
pvc.o: /usr/include/machine/context.h /usr/include/sys/procset.h
pvc.o: /usr/include/ctype.h /usr/include/sys/localedef.h
pvc.o: /usr/include/sys/lc_core.h /usr/include/locale.h
pvc.o: /usr/include/nl_types.h /usr/include/mesg.h /usr/include/stdio.h
pvc.o: /usr/include/sys/seek.h /usr/include/va_list.h /usr/include/getopt.h
pvc.o: /usr/include/sys/limits.h /usr/include/sys/machine/machlimits.h
pvc.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
pvc.o: /usr/include/sys/rt_limits.h /usr/include/limits.h types.h
pvc.o: /usr/include/stdlib.h /usr/include/stdarg.h /usr/include/string.h
pvc.o: /usr/include/strings.h evstruct.h list.h /usr/include/stddef.h pvc.h
body.o: /usr/include/sys/types.h /usr/include/standards.h
body.o: /usr/include/mach/machine/vm_types.h /usr/include/sys/select.h
body.o: /usr/include/sys/stat.h /usr/include/sys/mode.h /usr/include/ctype.h
body.o: /usr/include/sys/localedef.h /usr/include/sys/lc_core.h
body.o: /usr/include/locale.h /usr/include/nl_types.h /usr/include/mesg.h
body.o: /usr/include/stdio.h /usr/include/sys/seek.h /usr/include/va_list.h
body.o: /usr/include/getopt.h /usr/include/sys/limits.h
body.o: /usr/include/sys/machine/machlimits.h /usr/include/sys/syslimits.h
body.o: /usr/include/sys/machine/machtime.h /usr/include/sys/rt_limits.h
body.o: /usr/include/limits.h types.h /usr/include/stdlib.h
body.o: /usr/include/stdarg.h /usr/include/string.h /usr/include/strings.h
body.o: evstruct.h list.h /usr/include/stddef.h body.h memory.h
common.o: /usr/include/stdio.h /usr/include/standards.h
common.o: /usr/include/sys/seek.h /usr/include/va_list.h
common.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
common.o: /usr/include/sys/select.h /usr/include/getopt.h
common.o: /usr/include/sys/limits.h /usr/include/sys/machine/machlimits.h
common.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
common.o: /usr/include/sys/rt_limits.h ispdef.h /usr/include/ctype.h
common.o: /usr/include/sys/localedef.h /usr/include/sys/lc_core.h
common.o: /usr/include/locale.h /usr/include/nl_types.h /usr/include/mesg.h
common.o: /usr/include/limits.h types.h /usr/include/stdlib.h
common.o: /usr/include/stdarg.h /usr/include/string.h /usr/include/strings.h
common.o: evstruct.h list.h /usr/include/stddef.h alpha_op.h opcode.h
memory.o: memory.h types.h /usr/include/stdio.h /usr/include/standards.h
memory.o: /usr/include/sys/seek.h /usr/include/va_list.h
memory.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
memory.o: /usr/include/sys/select.h /usr/include/getopt.h
memory.o: /usr/include/sys/limits.h /usr/include/sys/machine/machlimits.h
memory.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
memory.o: /usr/include/sys/rt_limits.h /usr/include/stdlib.h
memory.o: /usr/include/stdarg.h /usr/include/string.h /usr/include/strings.h
memory.o: evstruct.h list.h /usr/include/stddef.h
rest.o: types.h /usr/include/stdio.h /usr/include/standards.h
rest.o: /usr/include/sys/seek.h /usr/include/va_list.h
rest.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
rest.o: /usr/include/sys/select.h /usr/include/getopt.h
rest.o: /usr/include/sys/limits.h /usr/include/sys/machine/machlimits.h
rest.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
rest.o: /usr/include/sys/rt_limits.h /usr/include/stdlib.h
rest.o: /usr/include/stdarg.h /usr/include/string.h /usr/include/strings.h
rest.o: evstruct.h list.h /usr/include/stddef.h rest.h memory.h
rest.o: /usr/include/ctype.h /usr/include/sys/localedef.h
rest.o: /usr/include/sys/lc_core.h /usr/include/locale.h
rest.o: /usr/include/nl_types.h /usr/include/mesg.h /usr/include/limits.h
sched.o: types.h /usr/include/stdio.h /usr/include/standards.h
sched.o: /usr/include/sys/seek.h /usr/include/va_list.h
sched.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
sched.o: /usr/include/sys/select.h /usr/include/getopt.h
sched.o: /usr/include/sys/limits.h /usr/include/sys/machine/machlimits.h
sched.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
sched.o: /usr/include/sys/rt_limits.h /usr/include/stdlib.h
sched.o: /usr/include/stdarg.h /usr/include/string.h /usr/include/strings.h
sched.o: evstruct.h list.h /usr/include/stddef.h ispdef.h sched.h memory.h
ev4.o: types.h /usr/include/stdio.h /usr/include/standards.h
ev4.o: /usr/include/sys/seek.h /usr/include/va_list.h
ev4.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
ev4.o: /usr/include/sys/select.h /usr/include/getopt.h
ev4.o: /usr/include/sys/limits.h /usr/include/sys/machine/machlimits.h
ev4.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
ev4.o: /usr/include/sys/rt_limits.h /usr/include/stdlib.h
ev4.o: /usr/include/stdarg.h /usr/include/string.h /usr/include/strings.h
ev4.o: evstruct.h list.h /usr/include/stddef.h ispdef.h rest.h sched.h
ev4.o: memory.h ev4.h opcode.h
ev5.o: types.h /usr/include/stdio.h /usr/include/standards.h
ev5.o: /usr/include/sys/seek.h /usr/include/va_list.h
ev5.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
ev5.o: /usr/include/sys/select.h /usr/include/getopt.h
ev5.o: /usr/include/sys/limits.h /usr/include/sys/machine/machlimits.h
ev5.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
ev5.o: /usr/include/sys/rt_limits.h /usr/include/stdlib.h
ev5.o: /usr/include/stdarg.h /usr/include/string.h /usr/include/strings.h
ev5.o: evstruct.h list.h /usr/include/stddef.h ispdef.h rest.h sched.h
ev5.o: memory.h opcode.h ev5.h body.h
ev6.o: types.h /usr/include/stdio.h /usr/include/standards.h
ev6.o: /usr/include/sys/seek.h /usr/include/va_list.h
ev6.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
ev6.o: /usr/include/sys/select.h /usr/include/getopt.h
ev6.o: /usr/include/sys/limits.h /usr/include/sys/machine/machlimits.h
ev6.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
ev6.o: /usr/include/sys/rt_limits.h /usr/include/stdlib.h
ev6.o: /usr/include/stdarg.h /usr/include/string.h /usr/include/strings.h
ev6.o: evstruct.h list.h /usr/include/stddef.h ispdef.h rest.h sched.h
ev6.o: memory.h ev6.h body.h alpha_op.h opcode.h
ev6_rest.o: types.h /usr/include/stdio.h /usr/include/standards.h
ev6_rest.o: /usr/include/sys/seek.h /usr/include/va_list.h
ev6_rest.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
ev6_rest.o: /usr/include/sys/select.h /usr/include/getopt.h
ev6_rest.o: /usr/include/sys/limits.h /usr/include/sys/machine/machlimits.h
ev6_rest.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
ev6_rest.o: /usr/include/sys/rt_limits.h /usr/include/stdlib.h
ev6_rest.o: /usr/include/stdarg.h /usr/include/string.h
ev6_rest.o: /usr/include/strings.h evstruct.h list.h /usr/include/stddef.h
ev6_rest.o: ispdef.h rest.h sched.h memory.h ev6.h body.h alpha_op.h opcode.h
