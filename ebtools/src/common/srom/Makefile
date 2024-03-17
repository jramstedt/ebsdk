CONFIG_DIR=../../../..
include $(CONFIG_DIR)/sdkmake.conf

CC= cc
CFLAGS= -g
LFLAGS= -lc
TOOLBOX	    	= $(EB_TOOLBOX)
OFILES= \
	srom.o

TOUCH = /usr/bin/touch
LINT		= /usr/bin/lint
LINTFLAGS 	=  -1 -wp -a -n -I. -I../include

#
tools:		srom

srom:		$(OFILES)
		$(CC) $(CFLAGS) -o srom $(OFILES) $(LFLAGS)

srom.o:		srom.c
		$(CC) $(CFLAGS) -c srom.c

install:    	srom
		cp -fp srom $(TOOLBOX)/.
		chmod 775  $(TOOLBOX)/srom

lint:	
	$(LINT) $(LINTFLAGS) $(INCLUDES) srom.c

depend: srom.c
		makedepend -f $(MAKEFILE) -- $(CFLAGS) -- srom.c

clean:
		rm -f *.o *~ \#*\# *.bak *.a core\
		srom

# DO NOT DELETE THIS LINE -- make depend depends on it.

srom.o: /usr/include/stdio.h /usr/include/standards.h /usr/include/sys/seek.h
srom.o: /usr/include/va_list.h /usr/include/sys/types.h
srom.o: /usr/include/mach/machine/vm_types.h /usr/include/sys/select.h
srom.o: /usr/include/getopt.h /usr/include/sys/limits.h
srom.o: /usr/include/sys/machine/machlimits.h /usr/include/sys/syslimits.h
srom.o: /usr/include/sys/machine/machtime.h /usr/include/sys/rt_limits.h
srom.o: /usr/include/ctype.h /usr/include/sys/localedef.h
srom.o: /usr/include/sys/lc_core.h /usr/include/locale.h
srom.o: /usr/include/nl_types.h /usr/include/mesg.h /usr/include/limits.h
srom.o: /usr/include/string.h /usr/include/strings.h /usr/include/stdlib.h
