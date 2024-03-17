CONFIG_DIR=../../../..
include $(CONFIG_DIR)/sdkmake.conf

CC		= cc
CFLAGS		= -g -I. -I../include
LFLAGS		= -lc
COMMON		= ../common
INCLUDE		= ../include
COMMONLIB	= $(COMMON)/commlib.a
TOOLBOX	    	= $(EB_TOOLBOX)
LINT		= /usr/bin/lint
LINTFLAGS 	=  -1 -wp -av -n -I. -I../include

TOUCH = /usr/bin/touch
#
tools:		clist\
    	        alist

clist:		clist.o \
    	    	$(COMMONLIB)
		$(CC) $(CFLAGS) -o clist clist.o $(COMMONLIB) $(LFLAGS)

alist:		alist.o \
    	    	$(COMMONLIB)
		$(CC) $(CFLAGS) -o alist alist.o $(COMMONLIB) $(LFLAGS)

preloc:		preloc.c
		$(CC) $(CFLAGS) -o preloc preloc.c $(LFLAGS)

clist.o:	clist.c
		$(CC) $(CFLAGS) -c clist.c

alist.o:	alist.c
		$(CC) $(CFLAGS) -c alist.c

lint:	
	$(LINT) $(LINTFLAGS) $(INCLUDES) clist.c   
	$(LINT) $(LINTFLAGS) $(INCLUDES) alist.c   

install:	alist clist
		cp -fp alist clist $(TOOLBOX)/.
		chmod 775 $(TOOLBOX)/alist $(TOOLBOX)/clist
    	    	
depend: clist.c alist.c
		makedepend -f $(MAKEFILE) -- $(CFLAGS) -- clist.c alist.c

clean:
		rm -f *.o *~ \#*\# *.bak *.a core\
		clist alist

# DO NOT DELETE THIS LINE -- make depend depends on it.

clist.o: /usr/include/stdio.h /usr/include/standards.h
clist.o: /usr/include/sys/seek.h /usr/include/va_list.h
clist.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
clist.o: /usr/include/sys/select.h /usr/include/getopt.h
clist.o: /usr/include/sys/limits.h /usr/include/sys/machine/machlimits.h
clist.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
clist.o: /usr/include/sys/rt_limits.h /usr/include/stdlib.h
clist.o: /usr/include/ctype.h /usr/include/sys/localedef.h
clist.o: /usr/include/sys/lc_core.h /usr/include/locale.h
clist.o: /usr/include/nl_types.h /usr/include/mesg.h /usr/include/limits.h
clist.o: /usr/include/string.h /usr/include/strings.h ../include/coff.h
clist.o: ../include/syms.h ../include/reloc.h
alist.o: /usr/include/stdio.h /usr/include/standards.h
alist.o: /usr/include/sys/seek.h /usr/include/va_list.h
alist.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
alist.o: /usr/include/sys/select.h /usr/include/getopt.h
alist.o: /usr/include/sys/limits.h /usr/include/sys/machine/machlimits.h
alist.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
alist.o: /usr/include/sys/rt_limits.h /usr/include/ctype.h
alist.o: /usr/include/sys/localedef.h /usr/include/sys/lc_core.h
alist.o: /usr/include/locale.h /usr/include/nl_types.h /usr/include/mesg.h
alist.o: /usr/include/limits.h /usr/include/string.h /usr/include/strings.h
alist.o: /usr/include/stdlib.h ../include/a_out.h
alist.o: /usr/include/mach/std_types.h /usr/include/mach/boolean.h
alist.o: /usr/include/mach/machine/boolean.h /usr/include/mach/kern_return.h
alist.o: /usr/include/mach/machine/kern_return.h /usr/include/mach/port.h
alist.o: ../include/c_32_64.h
