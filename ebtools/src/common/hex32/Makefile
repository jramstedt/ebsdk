CONFIG_DIR=../../../..
include $(CONFIG_DIR)/sdkmake.conf

CC= cc
CFLAGS		= -g -I. -I../include
LFLAGS= -lc
TOOLBOX	    	= $(EB_TOOLBOX)
OFILES= \
	hex32.o

TOUCH = /usr/bin/touch
LINT		= /usr/bin/lint
LINTFLAGS 	=  -1 -wp -a -n -I. -I../include

#
tools:		hex32

hex32:		$(OFILES)
		$(CC) $(CFLAGS) -o hex32 $(OFILES) $(LFLAGS)

hex32.o:	hex32.c
		$(CC) $(CFLAGS) -c hex32.c

install:    	hex32
		cp -fp hex32 $(TOOLBOX)/.
		chmod 775  $(TOOLBOX)/hex32

lint:	
	$(LINT) $(LINTFLAGS) $(INCLUDES) hex32.c

depend: hex32.c
		makedepend -f $(MAKEFILE) -- $(CFLAGS) -- hex32.c

clean:
		rm -f *.o *~ \#*\# *.bak *.a core\
		hex32

# DO NOT DELETE THIS LINE -- make depend depends on it.

hex32.o: /usr/include/stdio.h /usr/include/standards.h
hex32.o: /usr/include/sys/seek.h /usr/include/va_list.h
hex32.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
hex32.o: /usr/include/sys/select.h /usr/include/getopt.h
hex32.o: /usr/include/sys/limits.h /usr/include/sys/machine/machlimits.h
hex32.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
hex32.o: /usr/include/sys/rt_limits.h /usr/include/ctype.h
hex32.o: /usr/include/sys/localedef.h /usr/include/sys/lc_core.h
hex32.o: /usr/include/locale.h /usr/include/nl_types.h /usr/include/mesg.h
hex32.o: /usr/include/limits.h /usr/include/string.h /usr/include/strings.h
hex32.o: /usr/include/stdlib.h
