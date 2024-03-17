CONFIG_DIR=../../../..
include $(CONFIG_DIR)/sdkmake.conf

CC		= cc
CFLAGS		= -g -I. -I../include
LFLAGS		= -lc
INCLUDE 	= ../include
COMMON		= ../common
COMMONLIB	= $(COMMON)/commlib.a
TOOLBOX	    	= $(EB_TOOLBOX)
OFILES		= srec.o $(COMMONLIB)
LINT		= /usr/bin/lint
LINTFLAGS 	=  -1 -wp -a -n -I. -I../include

tools:	    	srec

srec:		$(OFILES)
		$(CC) $(CFLAGS) -o srec $(OFILES) $(LFLAGS)

srec.o:	    	srec.c
		$(CC) $(CFLAGS) -c srec.c

lint:	
	$(LINT) $(LINTFLAGS) $(INCLUDES) srec.c

install:    	srec
		cp -fp srec $(TOOLBOX)/.
		chmod 775  $(TOOLBOX)/srec

depend: srec.c
		makedepend -f $(MAKEFILE) -- $(CFLAGS) -- srec.c

clean:
		rm -f *.o *~ \#*\# *.bak *.a core\
		srec

# DO NOT DELETE THIS LINE -- make depend depends on it.

srec.o: /usr/include/stdio.h /usr/include/standards.h /usr/include/sys/seek.h
srec.o: /usr/include/va_list.h /usr/include/sys/types.h
srec.o: /usr/include/mach/machine/vm_types.h /usr/include/sys/select.h
srec.o: /usr/include/getopt.h /usr/include/sys/limits.h
srec.o: /usr/include/sys/machine/machlimits.h /usr/include/sys/syslimits.h
srec.o: /usr/include/sys/machine/machtime.h /usr/include/sys/rt_limits.h
srec.o: /usr/include/ctype.h /usr/include/sys/localedef.h
srec.o: /usr/include/sys/lc_core.h /usr/include/locale.h
srec.o: /usr/include/nl_types.h /usr/include/mesg.h /usr/include/limits.h
srec.o: /usr/include/string.h /usr/include/strings.h /usr/include/stdlib.h
srec.o: /usr/include/sys/stat.h /usr/include/sys/mode.h ../include/a_out.h
srec.o: /usr/include/mach/std_types.h /usr/include/mach/boolean.h
srec.o: /usr/include/mach/machine/boolean.h /usr/include/mach/kern_return.h
srec.o: /usr/include/mach/machine/kern_return.h /usr/include/mach/port.h
srec.o: ../include/c_32_64.h
