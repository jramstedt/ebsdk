CONFIG_DIR=../../../..
include $(CONFIG_DIR)/sdkmake.conf

CC= cc
CFLAGS		= -g -I. -I../include
LFLAGS		=
TOOLBOX	    	= $(EB_TOOLBOX)

C_SRCS = \
	ev6.c  \
	ev6_cbox.c \
	ev6_chain.c \
	ev6_srom.c \
	ev6_ic.c

IC4MAT_SRCS	= ic4mat.c $(C_SRCS)
CHAIN_SRCS	= chain2cbox.c $(C_SRCS)

IC4MAT_OFILES	= $(IC4MAT_SRCS:.c=.o)
CHAIN_OFILES	= $(CHAIN_SRCS:.c=.o)

LINT		= /usr/bin/lint
LINTFLAGS 	= -Nt2000 -1 -wp -a -n -I. -I../include

tools:		ic4mat

ic4mat:		$(IC4MAT_OFILES)
		$(CC) $(CFLAGS) -o ic4mat $(IC4MAT_OFILES) $(LFLAGS)

chain2cbox:	$(CHAIN_OFILES)
		$(CC) $(CFLAGS) -o chain2cbox $(CHAIN_OFILES) $(LFLAGS)

install:    	ic4mat
		cp -fp ic4mat $(TOOLBOX)/.
		chmod 775  $(TOOLBOX)/ic4mat

lint:	$(IC4MAT_SRCS) $(CHAIN_SRCS)
	$(LINT) $(LINTFLAGS) $(INCLUDES) $(CHAIN_SRCS)
	$(LINT) $(LINTFLAGS) $(INCLUDES) $(IC4MAT_SRCS)

depend: $(IC4MAT_SRCS) $(CHAIN_SRCS)
		makedepend -f $(MAKEFILE) -- $(CFLAGS) -- $(IC4MAT_SRCS) $(CHAIN_SRCS)

clean:
		rm -f *.o *~ \#*\# *.bak *.a core \
		$(IC4MAT_OFILES) \
		$(CHAIN_OFILES) \
		ic4mat


# DO NOT DELETE THIS LINE -- make depend depends on it.

ic4mat.o: ic4mat.h /usr/include/stdarg.h /usr/include/standards.h
ic4mat.o: /usr/include/va_list.h /usr/include/stdio.h /usr/include/sys/seek.h
ic4mat.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
ic4mat.o: /usr/include/sys/select.h /usr/include/getopt.h
ic4mat.o: /usr/include/sys/limits.h /usr/include/sys/machine/machlimits.h
ic4mat.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
ic4mat.o: /usr/include/sys/rt_limits.h /usr/include/ctype.h
ic4mat.o: /usr/include/sys/localedef.h /usr/include/sys/lc_core.h
ic4mat.o: /usr/include/locale.h /usr/include/nl_types.h /usr/include/mesg.h
ic4mat.o: /usr/include/limits.h /usr/include/string.h /usr/include/strings.h
ic4mat.o: /usr/include/stdlib.h ebtools.h ev6.h
ev6.o: ic4mat.h /usr/include/stdarg.h /usr/include/standards.h
ev6.o: /usr/include/va_list.h /usr/include/stdio.h /usr/include/sys/seek.h
ev6.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
ev6.o: /usr/include/sys/select.h /usr/include/getopt.h
ev6.o: /usr/include/sys/limits.h /usr/include/sys/machine/machlimits.h
ev6.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
ev6.o: /usr/include/sys/rt_limits.h /usr/include/ctype.h
ev6.o: /usr/include/sys/localedef.h /usr/include/sys/lc_core.h
ev6.o: /usr/include/locale.h /usr/include/nl_types.h /usr/include/mesg.h
ev6.o: /usr/include/limits.h /usr/include/string.h /usr/include/strings.h
ev6.o: /usr/include/stdlib.h ebtools.h ev6.h
ev6_cbox.o: ic4mat.h /usr/include/stdarg.h /usr/include/standards.h
ev6_cbox.o: /usr/include/va_list.h /usr/include/stdio.h
ev6_cbox.o: /usr/include/sys/seek.h /usr/include/sys/types.h
ev6_cbox.o: /usr/include/mach/machine/vm_types.h /usr/include/sys/select.h
ev6_cbox.o: /usr/include/getopt.h /usr/include/sys/limits.h
ev6_cbox.o: /usr/include/sys/machine/machlimits.h
ev6_cbox.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
ev6_cbox.o: /usr/include/sys/rt_limits.h /usr/include/ctype.h
ev6_cbox.o: /usr/include/sys/localedef.h /usr/include/sys/lc_core.h
ev6_cbox.o: /usr/include/locale.h /usr/include/nl_types.h /usr/include/mesg.h
ev6_cbox.o: /usr/include/limits.h /usr/include/string.h
ev6_cbox.o: /usr/include/strings.h /usr/include/stdlib.h ebtools.h ev6.h
ev6_chain.o: /usr/include/stdio.h /usr/include/standards.h
ev6_chain.o: /usr/include/sys/seek.h /usr/include/sys/types.h
ev6_chain.o: /usr/include/mach/machine/vm_types.h /usr/include/sys/select.h
ev6_chain.o: /usr/include/getopt.h /usr/include/sys/limits.h
ev6_chain.o: /usr/include/sys/machine/machlimits.h
ev6_chain.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
ev6_chain.o: /usr/include/sys/rt_limits.h ic4mat.h /usr/include/stdarg.h
ev6_chain.o: /usr/include/va_list.h /usr/include/ctype.h
ev6_chain.o: /usr/include/sys/localedef.h /usr/include/sys/lc_core.h
ev6_chain.o: /usr/include/locale.h /usr/include/nl_types.h
ev6_chain.o: /usr/include/mesg.h /usr/include/limits.h /usr/include/string.h
ev6_chain.o: /usr/include/strings.h /usr/include/stdlib.h ebtools.h ev6.h
ev6_srom.o: /usr/include/stdio.h /usr/include/standards.h
ev6_srom.o: /usr/include/sys/seek.h /usr/include/sys/types.h
ev6_srom.o: /usr/include/mach/machine/vm_types.h /usr/include/sys/select.h
ev6_srom.o: /usr/include/getopt.h /usr/include/sys/limits.h
ev6_srom.o: /usr/include/sys/machine/machlimits.h
ev6_srom.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
ev6_srom.o: /usr/include/sys/rt_limits.h /usr/include/stdlib.h
ev6_srom.o: /usr/include/sys/stat.h /usr/include/sys/mode.h ic4mat.h
ev6_srom.o: /usr/include/stdarg.h /usr/include/va_list.h /usr/include/ctype.h
ev6_srom.o: /usr/include/sys/localedef.h /usr/include/sys/lc_core.h
ev6_srom.o: /usr/include/locale.h /usr/include/nl_types.h /usr/include/mesg.h
ev6_srom.o: /usr/include/limits.h /usr/include/string.h
ev6_srom.o: /usr/include/strings.h ebtools.h ev6.h ev6_ic.h j.h
ev6_ic.o: /usr/include/stdio.h /usr/include/standards.h
ev6_ic.o: /usr/include/sys/seek.h /usr/include/sys/types.h
ev6_ic.o: /usr/include/mach/machine/vm_types.h /usr/include/sys/select.h
ev6_ic.o: /usr/include/getopt.h /usr/include/sys/limits.h
ev6_ic.o: /usr/include/sys/machine/machlimits.h /usr/include/sys/syslimits.h
ev6_ic.o: /usr/include/sys/machine/machtime.h /usr/include/sys/rt_limits.h
ev6_ic.o: ev6_ic.h j.h
chain2cbox.o: ic4mat.h /usr/include/stdarg.h /usr/include/standards.h
chain2cbox.o: /usr/include/va_list.h /usr/include/stdio.h
chain2cbox.o: /usr/include/sys/seek.h /usr/include/sys/types.h
chain2cbox.o: /usr/include/mach/machine/vm_types.h /usr/include/sys/select.h
chain2cbox.o: /usr/include/getopt.h /usr/include/sys/limits.h
chain2cbox.o: /usr/include/sys/machine/machlimits.h
chain2cbox.o: /usr/include/sys/syslimits.h
chain2cbox.o: /usr/include/sys/machine/machtime.h
chain2cbox.o: /usr/include/sys/rt_limits.h /usr/include/ctype.h
chain2cbox.o: /usr/include/sys/localedef.h /usr/include/sys/lc_core.h
chain2cbox.o: /usr/include/locale.h /usr/include/nl_types.h
chain2cbox.o: /usr/include/mesg.h /usr/include/limits.h /usr/include/string.h
chain2cbox.o: /usr/include/strings.h /usr/include/stdlib.h ebtools.h ev6.h
ev6.o: ic4mat.h /usr/include/stdarg.h /usr/include/standards.h
ev6.o: /usr/include/va_list.h /usr/include/stdio.h /usr/include/sys/seek.h
ev6.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
ev6.o: /usr/include/sys/select.h /usr/include/getopt.h
ev6.o: /usr/include/sys/limits.h /usr/include/sys/machine/machlimits.h
ev6.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
ev6.o: /usr/include/sys/rt_limits.h /usr/include/ctype.h
ev6.o: /usr/include/sys/localedef.h /usr/include/sys/lc_core.h
ev6.o: /usr/include/locale.h /usr/include/nl_types.h /usr/include/mesg.h
ev6.o: /usr/include/limits.h /usr/include/string.h /usr/include/strings.h
ev6.o: /usr/include/stdlib.h ebtools.h ev6.h
ev6_cbox.o: ic4mat.h /usr/include/stdarg.h /usr/include/standards.h
ev6_cbox.o: /usr/include/va_list.h /usr/include/stdio.h
ev6_cbox.o: /usr/include/sys/seek.h /usr/include/sys/types.h
ev6_cbox.o: /usr/include/mach/machine/vm_types.h /usr/include/sys/select.h
ev6_cbox.o: /usr/include/getopt.h /usr/include/sys/limits.h
ev6_cbox.o: /usr/include/sys/machine/machlimits.h
ev6_cbox.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
ev6_cbox.o: /usr/include/sys/rt_limits.h /usr/include/ctype.h
ev6_cbox.o: /usr/include/sys/localedef.h /usr/include/sys/lc_core.h
ev6_cbox.o: /usr/include/locale.h /usr/include/nl_types.h /usr/include/mesg.h
ev6_cbox.o: /usr/include/limits.h /usr/include/string.h
ev6_cbox.o: /usr/include/strings.h /usr/include/stdlib.h ebtools.h ev6.h
ev6_chain.o: /usr/include/stdio.h /usr/include/standards.h
ev6_chain.o: /usr/include/sys/seek.h /usr/include/sys/types.h
ev6_chain.o: /usr/include/mach/machine/vm_types.h /usr/include/sys/select.h
ev6_chain.o: /usr/include/getopt.h /usr/include/sys/limits.h
ev6_chain.o: /usr/include/sys/machine/machlimits.h
ev6_chain.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
ev6_chain.o: /usr/include/sys/rt_limits.h ic4mat.h /usr/include/stdarg.h
ev6_chain.o: /usr/include/va_list.h /usr/include/ctype.h
ev6_chain.o: /usr/include/sys/localedef.h /usr/include/sys/lc_core.h
ev6_chain.o: /usr/include/locale.h /usr/include/nl_types.h
ev6_chain.o: /usr/include/mesg.h /usr/include/limits.h /usr/include/string.h
ev6_chain.o: /usr/include/strings.h /usr/include/stdlib.h ebtools.h ev6.h
ev6_srom.o: /usr/include/stdio.h /usr/include/standards.h
ev6_srom.o: /usr/include/sys/seek.h /usr/include/sys/types.h
ev6_srom.o: /usr/include/mach/machine/vm_types.h /usr/include/sys/select.h
ev6_srom.o: /usr/include/getopt.h /usr/include/sys/limits.h
ev6_srom.o: /usr/include/sys/machine/machlimits.h
ev6_srom.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
ev6_srom.o: /usr/include/sys/rt_limits.h /usr/include/stdlib.h
ev6_srom.o: /usr/include/sys/stat.h /usr/include/sys/mode.h ic4mat.h
ev6_srom.o: /usr/include/stdarg.h /usr/include/va_list.h /usr/include/ctype.h
ev6_srom.o: /usr/include/sys/localedef.h /usr/include/sys/lc_core.h
ev6_srom.o: /usr/include/locale.h /usr/include/nl_types.h /usr/include/mesg.h
ev6_srom.o: /usr/include/limits.h /usr/include/string.h
ev6_srom.o: /usr/include/strings.h ebtools.h ev6.h ev6_ic.h j.h
ev6_ic.o: /usr/include/stdio.h /usr/include/standards.h
ev6_ic.o: /usr/include/sys/seek.h /usr/include/sys/types.h
ev6_ic.o: /usr/include/mach/machine/vm_types.h /usr/include/sys/select.h
ev6_ic.o: /usr/include/getopt.h /usr/include/sys/limits.h
ev6_ic.o: /usr/include/sys/machine/machlimits.h /usr/include/sys/syslimits.h
ev6_ic.o: /usr/include/sys/machine/machtime.h /usr/include/sys/rt_limits.h
ev6_ic.o: ev6_ic.h j.h
