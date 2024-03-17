# $Id: Makefile,v 1.1.1.1 1998/12/29 21:36:22 paradis Exp $
# $Log: Makefile,v $
# Revision 1.1.1.1  1998/12/29 21:36:22  paradis
# Initial CVS checkin
#
# Revision 1.2  1995/11/14  20:38:15  cruz
# Added rule for lint.
#
# Revision 1.1  1995/03/07  13:49:26  fdh
# Initial revision
#
CONFIG_DIR=../../../..
include $(CONFIG_DIR)/sdkmake.conf


CC= cc
CFLAGS= -g
LFLAGS=
TOOLBOX	    	= $(EB_TOOLBOX)
OFILES= \
	hexpad.o

TOUCH = /usr/bin/touch
LINT		= /usr/bin/lint
LINTFLAGS 	=  -1 -wp -av -n -I. -I../include

#
tools:		hexpad

hexpad:		$(OFILES)
		$(CC) $(CFLAGS) -o hexpad $(OFILES) $(LFLAGS)

hexpad.o:		hexpad.c
		$(CC) $(CFLAGS) -c hexpad.c

install:    	hexpad
		cp -fp hexpad $(TOOLBOX)/.
lint:	
	$(LINT) $(LINTFLAGS) $(INCLUDES) hexpad.c

depend: hexpad.c
		makedepend -f $(MAKEFILE) -- $(CFLAGS) -- hexpad.c

clean:
		rm -f *.o *~ \#*\# *.bak *.a core\
		hexpad

# DO NOT DELETE THIS LINE -- make depend depends on it.

hexpad.o: /usr/include/stdio.h /usr/include/standards.h
hexpad.o: /usr/include/sys/seek.h /usr/include/va_list.h
hexpad.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
hexpad.o: /usr/include/sys/select.h /usr/include/getopt.h
hexpad.o: /usr/include/sys/limits.h /usr/include/sys/machine/machlimits.h
hexpad.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
hexpad.o: /usr/include/sys/rt_limits.h /usr/include/ctype.h
hexpad.o: /usr/include/sys/localedef.h /usr/include/sys/lc_core.h
hexpad.o: /usr/include/locale.h /usr/include/nl_types.h /usr/include/mesg.h
hexpad.o: /usr/include/limits.h /usr/include/string.h /usr/include/strings.h
hexpad.o: /usr/include/stdlib.h
