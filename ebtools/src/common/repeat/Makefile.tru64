CONFIG_DIR=../../../..
include $(CONFIG_DIR)/sdkmake.conf

CC		= cc
CFLAGS		= -g -I. -I../include
LFLAGS		= -lc
COMMON		= ../common
INCLUDE		= ../include
TOOLBOX	    	= $(EB_TOOLBOX)
LINT		= /usr/bin/lint
LINTFLAGS 	=  -1 -wp -av -n -I. -I../include

TOUCH = /usr/bin/touch
#
tools:		rpp

rpp:		rpp.o 
		$(CC) $(CFLAGS) -g -o rpp rpp.o $(COMMONLIB) $(LFLAGS)

rpp.o:	    	rpp.c
		$(CC) $(CFLAGS) -c rpp.c

install:    	rpp
		cp -fp rpp $(TOOLBOX)/.
		chmod 775 $(TOOLBOX)/rpp

lint:	
	$(LINT) $(LINTFLAGS) $(INCLUDES) rpp.c

depend: rpp.c
		makedepend -f $(MAKEFILE) -- $(CFLAGS) -- rpp.c

clean:
		rm -f *.o *~ \#*\# *.bak *.a core\
		rpp

# DO NOT DELETE THIS LINE -- make depend depends on it.

rpp.o: /usr/include/stdio.h /usr/include/standards.h /usr/include/sys/seek.h
rpp.o: /usr/include/va_list.h /usr/include/sys/types.h
rpp.o: /usr/include/mach/machine/vm_types.h /usr/include/sys/select.h
rpp.o: /usr/include/getopt.h /usr/include/sys/limits.h
rpp.o: /usr/include/sys/machine/machlimits.h /usr/include/sys/syslimits.h
rpp.o: /usr/include/sys/machine/machtime.h /usr/include/sys/rt_limits.h
rpp.o: /usr/include/ctype.h /usr/include/sys/localedef.h
rpp.o: /usr/include/sys/lc_core.h /usr/include/locale.h
rpp.o: /usr/include/nl_types.h /usr/include/mesg.h /usr/include/limits.h
rpp.o: /usr/include/string.h /usr/include/strings.h /usr/include/stdlib.h
