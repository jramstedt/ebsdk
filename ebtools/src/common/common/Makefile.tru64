CONFIG_DIR=../../../..
include $(CONFIG_DIR)/sdkmake.conf

CC= cc
CFLAGS= -g -I. -I../include
LFLAGS= -lc
LINT		= /usr/bin/lint
LINTFLAGS 	=  -1 -wp -av -n -I. -I../include

TOUCH = /usr/bin/touch

#
install:	commlib.a

commlib.a:	disassm.o romhead.o
		-rm -f commlib.a
		ar cr commlib.a disassm.o romhead.o
		ar ts commlib.a >/dev/null

disassm.o: disassm.c
		$(CC) $(CFLAGS) -c disassm.c

romhead.o: romhead.c
		$(CC) $(CFLAGS) -c romhead.c

depend: disassm.c romhead.c
		makedepend -f $(MAKEFILE) -- $(CFLAGS) -- disassm.c romhead.c

lint:	
	$(LINT) $(LINTFLAGS) $(INCLUDES) romhead.c disassm.c 
clean:
		rm -f *.o *~ \#*\# *.bak *.a core

# DO NOT DELETE THIS LINE -- make depend depends on it.

disassm.o: /usr/include/stdio.h /usr/include/standards.h
disassm.o: /usr/include/sys/seek.h /usr/include/va_list.h
disassm.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
disassm.o: /usr/include/sys/select.h /usr/include/getopt.h
disassm.o: /usr/include/sys/limits.h /usr/include/sys/machine/machlimits.h
disassm.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
disassm.o: /usr/include/sys/rt_limits.h /usr/include/ctype.h
disassm.o: /usr/include/sys/localedef.h /usr/include/sys/lc_core.h
disassm.o: /usr/include/locale.h /usr/include/nl_types.h /usr/include/mesg.h
disassm.o: /usr/include/limits.h /usr/include/string.h /usr/include/strings.h
disassm.o: /usr/include/stdlib.h ../include/alpha_op.h
romhead.o: ../include/romhead.h ../include/local.h /usr/include/stdio.h
romhead.o: /usr/include/standards.h /usr/include/sys/seek.h
romhead.o: /usr/include/va_list.h /usr/include/sys/types.h
romhead.o: /usr/include/mach/machine/vm_types.h /usr/include/sys/select.h
romhead.o: /usr/include/getopt.h /usr/include/sys/limits.h
romhead.o: /usr/include/sys/machine/machlimits.h /usr/include/sys/syslimits.h
romhead.o: /usr/include/sys/machine/machtime.h /usr/include/sys/rt_limits.h
romhead.o: /usr/include/string.h /usr/include/strings.h /usr/include/ctype.h
romhead.o: /usr/include/sys/localedef.h /usr/include/sys/lc_core.h
romhead.o: /usr/include/locale.h /usr/include/nl_types.h /usr/include/mesg.h
romhead.o: /usr/include/limits.h ../include/c_32_64.h
