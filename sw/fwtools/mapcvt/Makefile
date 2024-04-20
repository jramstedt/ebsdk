CC= cc
CFLAGS= -g -I. -I../include -O
LFLAGS=
OFILES= mapcvt.o
TOOLBOX	    	= $(LOCALROOT)/sw/fwtools/bin

tools:          mapcvt

mapcvt:         $(OFILES)
		$(CC) $(CFLAGS) -o mapcvt $(OFILES) $(LFLAGS)
		cp $@ $(TOOLBOX)

mapcvt.o:               mapcvt.c
		$(CC) $(CFLAGS) -c mapcvt.c

install:        mapcvt
		cp -fp mapcvt $(TOOLBOX)/.
		chmod 775 $(TOOLBOX)/mapcvt

clean:
		rm -f *.o *~ \#*\# *.bak *.a core\
		mapcvt

