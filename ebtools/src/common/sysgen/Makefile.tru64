CONFIG_DIR=../../../..
include $(CONFIG_DIR)/sdkmake.conf

CC= cc
CFLAGS= -g -I. -I../include
LFLAGS= -lc
OFILES= \
	sysgen.o
TOOLBOX	    	= $(EB_TOOLBOX)

TOUCH = /usr/bin/touch
LINT		= /usr/bin/lint
LINTFLAGS 	=  -1 -wp -a -n -I. -I../include


#
tools:		sysgen

sysgen:		$(OFILES)
		$(CC) $(CFLAGS) -o sysgen $(OFILES) $(LFLAGS)

sysgen.o:		sysgen.c
		$(CC) $(CFLAGS) -c sysgen.c

install:    	sysgen
		cp -fp sysgen $(TOOLBOX)/.
		chmod 775 $(TOOLBOX)/sysgen

depend: sysgen.c
		makedepend -f $(MAKEFILE) -- $(CFLAGS) -- sysgen.c
lint:	
	$(LINT) $(LINTFLAGS) $(INCLUDES) sysgen.c

clean:
		rm -f *.o *~ \#*\# *.bak *.a core\
		sysgen

# DO NOT DELETE THIS LINE -- make depend depends on it.

sysgen.o: /usr/include/stdio.h /usr/include/standards.h
sysgen.o: /usr/include/sys/seek.h /usr/include/va_list.h
sysgen.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
sysgen.o: /usr/include/sys/select.h /usr/include/getopt.h
sysgen.o: /usr/include/sys/limits.h /usr/include/sys/machine/machlimits.h
sysgen.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
sysgen.o: /usr/include/sys/rt_limits.h /usr/include/ctype.h
sysgen.o: /usr/include/sys/localedef.h /usr/include/sys/lc_core.h
sysgen.o: /usr/include/locale.h /usr/include/nl_types.h /usr/include/mesg.h
sysgen.o: /usr/include/limits.h /usr/include/string.h /usr/include/strings.h
sysgen.o: /usr/include/stdlib.h /usr/include/sys/stat.h
sysgen.o: /usr/include/sys/mode.h ../include/a_out.h
sysgen.o: /usr/include/mach/std_types.h /usr/include/mach/boolean.h
sysgen.o: /usr/include/mach/machine/boolean.h /usr/include/mach/kern_return.h
sysgen.o: /usr/include/mach/machine/kern_return.h /usr/include/mach/port.h
sysgen.o: ../include/c_32_64.h
