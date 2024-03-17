CONFIG_DIR=../../../..
include $(CONFIG_DIR)/sdkmake.conf

CC= cc
CFLAGS= -g -I. -I../include
LFLAGS=
OFILES= \
	uload.o
TOOLBOX         = $(EB_TOOLBOX)

TOUCH = /usr/bin/touch
LINT            = /usr/bin/lint
LINTFLAGS       =  -1 -wp -a -n -I. -I../include


#
tools:          uload xload

uload:         $(OFILES)
		$(CC) $(CFLAGS) -o uload $(OFILES) $(LFLAGS)

uload.o:               uload.c
		$(CC) $(CFLAGS) -c uload.c

xload:         $(OFILES)
		$(CC) $(CFLAGS) -o xload $(OFILES) $(LFLAGS)

xload.o:               xload.c
		$(CC) $(CFLAGS) -c xload.c


install:        uload xload
		cp -fp uload $(TOOLBOX)/.
		chmod 775 $(TOOLBOX)/uload
		cp -fp xload $(TOOLBOX)/.
		chmod 775 $(TOOLBOX)/xload

depend: uload.c
		makedepend -f $(MAKEFILE) -- $(CFLAGS) -- uload.c
lint:   
	$(LINT) $(LINTFLAGS) $(INCLUDES) uload.c

clean:
		rm -f *.o *~ \#*\# *.bak *.a core\
		uload xload

# DO NOT DELETE THIS LINE -- make depend depends on it.

