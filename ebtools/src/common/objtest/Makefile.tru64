CONFIG_DIR=../../../..
include $(CONFIG_DIR)/sdkmake.conf

CC		= cc
CFLAGS		= -g -I. -I../include
LFLAGS		= -lc
COMMON		= ../common
INCLUDE		= ../include
COMMONLIB	= $(COMMON)/commlib.a

TOUCH = /usr/bin/touch
#
tools:		ccheck newstrip

ccheck:		ccheck.o \
    	    	$(COMMONLIB)
		$(CC) $(CFLAGS) -o ccheck ccheck.o $(COMMONLIB) $(LFLAGS)

newstrip:	newstrip.o\
    	    	$(COMMONLIB)
		$(CC) $(CFLAGS) -o newstrip newstrip.o $(COMMONLIB) $(LFLAGS)

preloc:		preloc.c
		$(CC) $(CFLAGS) -o preloc preloc.c $(LFLAGS)

ccheck.o:	ccheck.c
		$(CC) $(CFLAGS) -c ccheck.c

newstrip.o:	newstrip.c
		$(CC) $(CFLAGS) -c newstrip.c

install:	tools

depend: ccheck.c newstrip.c
		makedepend -f $(MAKEFILE) -- $(CFLAGS) -- ccheck.c newstrip.c

clean:
		rm -f *.o *~ \#*\# *.bak *.a core\
		ccheck newstrip preloc

# DO NOT DELETE THIS LINE -- make depend depends on it.

ccheck.o: /usr/include/stdio.h /usr/include/standards.h
ccheck.o: /usr/include/sys/seek.h /usr/include/va_list.h
ccheck.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
ccheck.o: /usr/include/sys/select.h /usr/include/getopt.h
ccheck.o: /usr/include/sys/limits.h /usr/include/sys/machine/machlimits.h
ccheck.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
ccheck.o: /usr/include/sys/rt_limits.h /usr/include/ctype.h
ccheck.o: /usr/include/sys/localedef.h /usr/include/sys/lc_core.h
ccheck.o: /usr/include/locale.h /usr/include/nl_types.h /usr/include/mesg.h
ccheck.o: /usr/include/limits.h ../include/coff.h ../include/syms.h
ccheck.o: ../include/reloc.h
newstrip.o: /usr/include/stdio.h /usr/include/standards.h
newstrip.o: /usr/include/sys/seek.h /usr/include/va_list.h
newstrip.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
newstrip.o: /usr/include/sys/select.h /usr/include/getopt.h
newstrip.o: /usr/include/sys/limits.h /usr/include/sys/machine/machlimits.h
newstrip.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
newstrip.o: /usr/include/sys/rt_limits.h /usr/include/ctype.h
newstrip.o: /usr/include/sys/localedef.h /usr/include/sys/lc_core.h
newstrip.o: /usr/include/locale.h /usr/include/nl_types.h /usr/include/mesg.h
newstrip.o: /usr/include/limits.h ../include/coff.h ../include/syms.h
newstrip.o: ../include/reloc.h
