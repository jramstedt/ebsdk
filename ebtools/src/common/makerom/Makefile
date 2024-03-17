CONFIG_DIR=../../../..
include $(CONFIG_DIR)/sdkmake.conf

CC		= cc
CFLAGS		= -g -I. -I../include
LFLAGS		= -lc
COMMON		= ../common
INCLUDE		= ../include
COMMONLIB	= $(COMMON)/commlib.a
TOOLBOX	    	= $(EB_TOOLBOX)
OFILES		= makerom.o compress.o
LINT		= /usr/bin/lint
LINTFLAGS 	=  -1 -wp -av -n -I. -I../include

tools:		makerom

makerom:	$(OFILES) $(COMMONLIB)
		$(CC) $(CFLAGS) -o makerom $(OFILES) $(COMMONLIB) $(LFLAGS)

makerom.o:	makerom.c \
		../include/c_32_64.h 
		$(CC) $(CFLAGS) -c makerom.c

compress.o:	compress.c \
		../include/c_32_64.h 
		$(CC) $(CFLAGS) -c compress.c

lint:	
	$(LINT) $(LINTFLAGS) $(INCLUDES) makerom.c compress.c

install:    	makerom
		cp -fp makerom $(TOOLBOX)
		chmod 775 $(TOOLBOX)/makerom

depend: makerom.c compress.c
		makedepend -f $(MAKEFILE) -- $(CFLAGS) -- makerom.c compress.c

clean:
		rm -f *.o *~ \#*\# *.bak *.a core\
		makerom

# DO NOT DELETE THIS LINE -- make depend depends on it.

makerom.o: /usr/include/stdio.h /usr/include/standards.h
makerom.o: /usr/include/sys/seek.h /usr/include/va_list.h
makerom.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
makerom.o: /usr/include/sys/select.h /usr/include/getopt.h
makerom.o: /usr/include/sys/limits.h /usr/include/sys/machine/machlimits.h
makerom.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
makerom.o: /usr/include/sys/rt_limits.h /usr/include/ctype.h
makerom.o: /usr/include/sys/localedef.h /usr/include/sys/lc_core.h
makerom.o: /usr/include/locale.h /usr/include/nl_types.h /usr/include/mesg.h
makerom.o: /usr/include/limits.h /usr/include/string.h /usr/include/strings.h
makerom.o: /usr/include/stdlib.h ../include/c_32_64.h ../include/romhead.h
makerom.o: ../include/local.h
compress.o: /usr/include/stdio.h /usr/include/standards.h
compress.o: /usr/include/sys/seek.h /usr/include/va_list.h
compress.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
compress.o: /usr/include/sys/select.h /usr/include/getopt.h
compress.o: /usr/include/sys/limits.h /usr/include/sys/machine/machlimits.h
compress.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
compress.o: /usr/include/sys/rt_limits.h /usr/include/ctype.h
compress.o: /usr/include/sys/localedef.h /usr/include/sys/lc_core.h
compress.o: /usr/include/locale.h /usr/include/nl_types.h /usr/include/mesg.h
compress.o: /usr/include/limits.h /usr/include/string.h
compress.o: /usr/include/strings.h /usr/include/stdlib.h ../include/c_32_64.h
compress.o: ../include/romhead.h ../include/local.h
