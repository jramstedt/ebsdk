CONFIG_DIR=../../../..
include $(CONFIG_DIR)/sdkmake.conf

CC= cc
CFLAGS= -g -I. -I../include
LFLAGS=
OFILES= \
	mapcvt.o
TOOLBOX         = $(EB_TOOLBOX)

TOUCH = /usr/bin/touch
LINT            = /usr/bin/lint
LINTFLAGS       =  -1 -wp -a -n -I. -I../include


#
tools:          mapcvt

mapcvt:         $(OFILES)
		$(CC) $(CFLAGS) -o mapcvt $(OFILES) $(LFLAGS)

mapcvt.o:               mapcvt.c
		$(CC) $(CFLAGS) -c mapcvt.c

install:        mapcvt
		cp -fp mapcvt $(TOOLBOX)/.
		chmod 775 $(TOOLBOX)/mapcvt

depend: mapcvt.c
		makedepend -f $(MAKEFILE) -- $(CFLAGS) -- mapcvt.c
lint:   
	$(LINT) $(LINTFLAGS) $(INCLUDES) mapcvt.c

clean:
		rm -f *.o *~ \#*\# *.bak *.a core\
		mapcvt

# DO NOT DELETE THIS LINE -- make depend depends on it.

