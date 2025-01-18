# $Id: Makefile,v 1.1.1.1 1998/12/29 21:36:05 paradis Exp $
# $Log: Makefile,v $
# Revision 1.1.1.1  1998/12/29 21:36:05  paradis
# Initial CVS checkin
#
# Revision 1.41  1997/12/15  20:49:47  pbell
# Updated for dp264.
#
# Revision 1.40  1997/06/02  17:42:21  fdh
# Updated include file dependencies.
#
# Revision 1.39  1997/05/19  15:25:56  fdh
# Updated include file dependencies.
#
# Revision 1.38  1997/04/23  18:17:07  fdh
# Updated include file dependencies.
#
# Revision 1.37  1997/04/23  18:13:18  fdh
# Updated include file dependencies.
#
# Revision 1.36  1997/04/10  17:13:21  fdh
# Modified the rule for making library archive.
#
# Revision 1.35  1997/03/17  10:35:52  fdh
# Modified to get the object file list from the source file list.
#
# Revision 1.34  1997/02/21  19:53:59  fdh
# Updated include file dependencies.
#
# Revision 1.33  1996/05/26 19:52:40  fdh
# Updated include file dependencies.
#
# Revision 1.32  1996/05/22  21:45:46  fdh
# Updated include file dependency list.
#
# Revision 1.31  1995/11/03  22:14:35  cruz
# Added lint library to list of items to remove when
# performing a clean.
#
# Revision 1.30  1995/10/31  18:48:05  cruz
# Fixed up lint rule.
#
# Revision 1.29  1995/10/30  17:06:20  cruz
# Added lint rule..
#
# Revision 1.28  1995/10/23  21:11:17  fdh
# Updated include file dependencies.
#
# Revision 1.27  1995/10/23  20:54:47  cruz
# Minor ordering changes.
#
# Revision 1.26  1995/10/18  12:59:02  fdh
# Added lint: target.
#
# Revision 1.25  1995/10/13  20:07:23  fdh
# Renamed libnet.a to ether.a. i.e. Same as directory name.
#
# Revision 1.24  1995/10/10  15:23:37  fdh
# Renamed eb64l to pc64.
#
# Revision 1.23  1995/10/05  01:55:48  fdh
# Updated include file dependencies.
#
# Revision 1.22  1995/10/03  03:50:38  fdh
# Update include file dependencies.
#
# Revision 1.21  1995/09/12  22:00:26  fdh
# Updated include file dependencies.
#
# Revision 1.20  1995/08/31  21:39:53  fdh
# Updated include file dependencies
#
# Revision 1.19  1995/08/25  21:18:19  fdh
# Updated include file dependencies...
#
# Revision 1.18  1994/12/08  17:52:15  fdh
# Updated include file dependency list.
#
# Revision 1.17  1994/11/24  04:31:11  fdh
# Updated include file dependency lists.
#
# Revision 1.16  1994/11/23  19:53:52  fdh
# Updated include file dependencies.
#
# Revision 1.15  1994/11/23  15:43:20  fdh
# Modified include path definitions...
#
# Revision 1.14  1994/11/22  23:15:12  fdh
# Modified to use the INCDIR definition.
#
# Revision 1.13  1994/11/11  20:47:47  fdh
# Updated include file dependency list.
#
# Revision 1.12  1994/11/04  16:37:55  fdh
# Updated include file dependency list.
#
# Revision 1.11  1994/11/01  13:10:12  rusling
# DEC21040.h and .c are now dependent on pci.h
#
# Revision 1.10  1994/07/21  17:48:10  fdh
# Added ethernet.h to dependencies for udp.c.
#
# Revision 1.9  1994/07/21  13:54:08  fdh
# Added -f flag to rm commands.
#
# Revision 1.8  1994/07/01  15:13:36  fdh
# Updated include dependencies...
#
# Revision 1.7  1994/06/28  20:11:47  fdh
# Modified filenames and build precedure to fit into a FAT filesystem.
#
# Revision 1.6  1994/06/20  14:59:47  fdh
# Fixup include file dependencies.
#
# Revision 1.5  1994/06/17  19:36:25  fdh
# Clean-up...
#
# Revision 1.4  1994/06/10  14:45:03  fdh
# Updated include file dependencies.
#
# Revision 1.3  1994/04/06  05:12:29  fdh
# Removed a driver.
#
# Revision 1.2  1994/03/16  00:16:17  fdh
# Added eaddr.c to build.
#
# Revision 1.1  1994/03/11  05:09:17  fdh
# Initial revision
#

CONFIG_DIR=../..
include $(CONFIG_DIR)/sdkmake.conf
include ../rules.gnu

# -------------------------------------------------
# Append local cc flags here for only this Makefile
# -------------------------------------------------
CCFLAGS = $(CCFLAGS_GLOBAL)

TARGET_SRCS = 

ALL_TARGET_SRCS =

AS_SRCS	 = 

C_SRCS	 = \
        $(SRCDIR)DEC21040.c \
	$(SRCDIR)am79c960.c \
	$(SRCDIR)arp.c \
	$(SRCDIR)bootp.c \
	$(SRCDIR)eaddr.c \
	$(SRCDIR)edevice.c \
	$(SRCDIR)ethernet.c \
	$(SRCDIR)ip.c \
	$(SRCDIR)net_buff.c \
	$(SRCDIR)netboot.c \
	$(SRCDIR)netman.c \
	$(SRCDIR)tftp.c \
	$(SRCDIR)udp.c

OBJFILES = $(AS_SRCS:%.s=$(OBJDIR)%.$(OBJ)) $(TARGET_SRCS:%.c=$(OBJDIR)%.$(OBJ)) $(C_SRCS:%.c=$(OBJDIR)%.$(OBJ))

all:  $(OBJDIR)ether.$(LIBEXT)

#----------------

ifeq "$(EB_BUILD_SYSTEM)" "WINNT"
$(OBJDIR)ether.$(LIBEXT): $(OBJFILES)
	$(DELETE) $(OBJDIR)ether.$(LIBEXT)
	$(LIBRARY) -VERBOSE -OUT:$(OBJDIR)ether.$(LIBEXT) $(OBJFILES)
else
$(OBJDIR)ether.$(LIBEXT): $(OBJFILES)
	$(DELETE) $(OBJDIR)ether.$(LIBEXT)
	$(AR) rcs $(OBJDIR)ether.$(LIBEXT) $(OBJFILES)
endif

#----------------------------------------------------------------------------
$(OBJDIR)eaddr.$(OBJ):  	    	$(SOURCEDIR)eaddr.c
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

$(OBJDIR)netman.$(OBJ):  	    	$(SOURCEDIR)netman.c
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

$(OBJDIR)netboot.$(OBJ) :   	    	$(SOURCEDIR)netboot.c
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

$(OBJDIR)tftp.$(OBJ) :   	    	$(SOURCEDIR)tftp.c
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

$(OBJDIR)bootp.$(OBJ) :   	    	$(SOURCEDIR)bootp.c
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

$(OBJDIR)udp.$(OBJ) :    	    	$(SOURCEDIR)udp.c
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

$(OBJDIR)ip.$(OBJ) :     	    	$(SOURCEDIR)ip.c
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

$(OBJDIR)arp.$(OBJ) :    	    	$(SOURCEDIR)arp.c
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

$(OBJDIR)ethernet.$(OBJ) :       	$(SOURCEDIR)ethernet.c
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

$(OBJDIR)edevice.$(OBJ) :   	$(SOURCEDIR)edevice.c
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

$(OBJDIR)am79c960.$(OBJ): 	$(SOURCEDIR)am79c960.c
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

$(OBJDIR)net_buff.$(OBJ) :     	$(SOURCEDIR)net_buff.c
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

$(OBJDIR)DEC21040.$(OBJ):	$(SOURCEDIR)DEC21040.c
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<


#----------------------------------------------------------------------------

depend: $(AS_SRCS) $(C_SRCS) $(ALL_TARGET_SRCS)
	$(MAKEDEPEND) -p'$$(OBJDIR)' -- $(DEPENDFLAGS) -- \
	$(AS_SRCS) $(C_SRCS) $(ALL_TARGET_SRCS)

clean:
	$(DELETE) *~ \#*\# core ${MAKEFILE}.bak \
		$(OBJFILES) \
		$(OBJDIR)llib-lether.ln \
		$(OBJDIR)ether.$(LIBEXT)
	$(CLEAN)

distclean: clean
	$(MAKEDEPEND) -f $(MAKEFILE)
	$(DELETE) $(MAKEFILE).bak

#----------------------------------------------------------------------------
# DO NOT DELETE THIS LINE -- make depend depends on it.
