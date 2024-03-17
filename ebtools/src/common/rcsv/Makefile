CONFIG_DIR=../../../..
include $(CONFIG_DIR)/sdkmake.conf


CC		= cc
CFLAGS		= -g -I. -I../include
LFLAGS		=
COMMON		= ../common
INCLUDE		= ../include
TOOLBOX	    	= $(EB_TOOLBOX)
LINT		= /usr/bin/lint
LINTFLAGS 	=  -1 -wp -av -n -I. -I../include

TOUCH = /usr/bin/touch
#
tools:		rcsv

rcsv:		rcsv.o 
		$(CC) $(CFLAGS) -g -o rcsv rcsv.o $(COMMONLIB) $(LFLAGS)

rcsv.o:	    	rcsv.c
		$(CC) $(CFLAGS) -c rcsv.c

lint:	
	$(LINT) $(LINTFLAGS) $(INCLUDES) rcsv.c

install:    	rcsv
		cp -fp rcsv $(TOOLBOX)/.
		chmod 775 $(TOOLBOX)/rcsv

depend: rcsv.c
		makedepend -f $(MAKEFILE) -- $(CFLAGS) -- rcsv.c

clean:
		rm -f *.o *~ \#*\# ${MAKEFILE}.bak *.a core \
		rcsv

# DO NOT DELETE THIS LINE -- make depend depends on it.

rcsv.o: /usr/include/stdio.h /usr/include/standards.h /usr/include/sys/seek.h
rcsv.o: /usr/include/va_list.h /usr/include/sys/types.h
rcsv.o: /usr/include/mach/machine/vm_types.h /usr/include/sys/select.h
rcsv.o: /usr/include/getopt.h /usr/include/sys/limits.h
rcsv.o: /usr/include/sys/machine/machlimits.h /usr/include/sys/syslimits.h
rcsv.o: /usr/include/sys/machine/machtime.h /usr/include/sys/rt_limits.h
rcsv.o: /usr/include/ctype.h /usr/include/sys/localedef.h
rcsv.o: /usr/include/sys/lc_core.h /usr/include/locale.h
rcsv.o: /usr/include/nl_types.h /usr/include/mesg.h /usr/include/limits.h
rcsv.o: /usr/include/string.h /usr/include/strings.h /usr/include/stdlib.h
