CONFIG_DIR=../../../..
include $(CONFIG_DIR)/sdkmake.conf

CC= cc
CFLAGS		= -g -I. -I../include
LFLAGS		=
TOOLBOX	    	= $(EB_TOOLBOX)

BIT_SRCS	= bit.c

BIT_OFILES		= $(BIT_SRCS:.c=.o)

LINT		= /usr/bin/lint
LINTFLAGS 	= -Nt2000 -1 -wp -a -n -I. -I../include

tools:		bit

bit:		$(BIT_OFILES) getopt.c
		$(CC) $(CFLAGS) -o bit $(BIT_OFILES) getopt.c $(LFLAGS)

install:    	bit
		cp -fp bit $(TOOLBOX)/.
		chmod 775  $(TOOLBOX)/bit

lint:	$(BIT_SRCS)
	$(LINT) $(LINTFLAGS) $(INCLUDES) $(BIT_SRCS)

depend: $(BIT_SRCS)
		makedepend -f $(MAKEFILE) -- $(CFLAGS) -- $(BIT_SRCS)

clean:
		rm -f *.o *~ \#*\# *.bak *.a core \
		$(BIT_OFILES) \
		bit


# DO NOT DELETE THIS LINE -- make depend depends on it.

bit.o: /usr/include/stdio.h /usr/include/standards.h /usr/include/sys/seek.h
bit.o: /usr/include/va_list.h /usr/include/sys/types.h
bit.o: /usr/include/mach/machine/vm_types.h /usr/include/sys/select.h
bit.o: /usr/include/getopt.h /usr/include/sys/limits.h
bit.o: /usr/include/sys/machine/machlimits.h /usr/include/sys/syslimits.h
bit.o: /usr/include/sys/machine/machtime.h /usr/include/sys/rt_limits.h
bit.o: /usr/include/ctype.h /usr/include/sys/localedef.h
bit.o: /usr/include/sys/lc_core.h /usr/include/locale.h
bit.o: /usr/include/nl_types.h /usr/include/mesg.h /usr/include/limits.h
bit.o: /usr/include/string.h /usr/include/strings.h /usr/include/stdlib.h
bit.o: /usr/include/errno.h ebtools.h
