
TOOLBOX	    	= $(LOCALROOT)/sw/fwtools/bin

CFLAGS		= -DEV6 -g3

SFILES		= pvc.c body.c common.c memory.c rest.c sched.c ev4.c ev5.c  ev6.c ev6_rest.c

OBJS		= $(SFILES:.c=.o)

tools:		pvc

pvc:		$(OBJS)
		$(CC) -DEV6 -o pvc $(OBJS)
		cp $@ $(TOOLBOX)

lint:	
	$(LINT) $(LINTFLAGS) $(INCLUDES) $(SFILES)

install:	pvc
		cp -fp pvc $(TOOLBOX)
		chmod 775 $(TOOLBOX)/pvc

clean:
		rm -f *~ \#*\# *.bak *.a core\
		$(OBJS) pvc

# DO NOT DELETE THIS LINE -- make depend depends on it.
