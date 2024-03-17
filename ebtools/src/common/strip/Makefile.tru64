CONFIG_DIR=../../../..
include $(CONFIG_DIR)/sdkmake.conf

CC		= cc 
CFLAGS		= -g -I. -I../include
LFLAGS		= -lc
COMMON		= ../common
INCLUDE		= ../include
TOOLBOX	    	= $(EB_TOOLBOX)
LINT		= /usr/bin/lint
LINTFLAGS 	=  -1 -wp -a -n -I. -I../include

AOFILES= \
	astrip.o

COFILES= \
	cstrip.o

TOUCH = /usr/bin/touch
#

tools:		astrip\
		cstrip

astrip:		$(AOFILES)
		$(CC) $(CFLAGS) -o astrip $(AOFILES) $(LFLAGS)

astrip.o:	astrip.c
		$(CC) $(CFLAGS) -c astrip.c

cstrip:		$(COFILES)
		$(CC) $(CFLAGS) -o cstrip $(COFILES) $(LFLAGS)

cstrip.o:	cstrip.c
		$(CC) $(CFLAGS) -c cstrip.c

install:    	astrip cstrip
		cp -fp astrip cstrip $(TOOLBOX)/.
		chmod 775 $(TOOLBOX)/astrip $(TOOLBOX)/cstrip

depend: cstrip.c astrip.c
		makedepend -f $(MAKEFILE) -- $(CFLAGS) -- cstrip.c astrip.c

lint:	
	$(LINT) $(LINTFLAGS) $(INCLUDES) cstrip.c
	$(LINT) $(LINTFLAGS) $(INCLUDES) astrip.c

clean:		
		rm -f *.o *~ \#*\# *.bak *.a core\
		astrip cstrip

# DO NOT DELETE THIS LINE -- make depend depends on it.

cstrip.o: /usr/include/stdio.h /usr/include/standards.h
cstrip.o: /usr/include/sys/seek.h /usr/include/va_list.h
cstrip.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
cstrip.o: /usr/include/sys/select.h /usr/include/getopt.h
cstrip.o: /usr/include/sys/limits.h /usr/include/sys/machine/machlimits.h
cstrip.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
cstrip.o: /usr/include/sys/rt_limits.h /usr/include/stdlib.h
cstrip.o: /usr/include/ctype.h /usr/include/sys/localedef.h
cstrip.o: /usr/include/sys/lc_core.h /usr/include/locale.h
cstrip.o: /usr/include/nl_types.h /usr/include/mesg.h /usr/include/limits.h
cstrip.o: /usr/include/strings.h /usr/include/string.h ../include/coff.h
cstrip.o: ../include/syms.h ../include/reloc.h
astrip.o: /usr/include/stdio.h /usr/include/standards.h
astrip.o: /usr/include/sys/seek.h /usr/include/va_list.h
astrip.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
astrip.o: /usr/include/sys/select.h /usr/include/getopt.h
astrip.o: /usr/include/sys/limits.h /usr/include/sys/machine/machlimits.h
astrip.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
astrip.o: /usr/include/sys/rt_limits.h /usr/include/ctype.h
astrip.o: /usr/include/sys/localedef.h /usr/include/sys/lc_core.h
astrip.o: /usr/include/locale.h /usr/include/nl_types.h /usr/include/mesg.h
astrip.o: /usr/include/limits.h /usr/include/string.h /usr/include/strings.h
astrip.o: /usr/include/stdlib.h ../include/a_out.h
astrip.o: /usr/include/mach/std_types.h /usr/include/mach/boolean.h
astrip.o: /usr/include/mach/machine/boolean.h /usr/include/mach/kern_return.h
astrip.o: /usr/include/mach/machine/kern_return.h /usr/include/mach/port.h
astrip.o: ../include/c_32_64.h
