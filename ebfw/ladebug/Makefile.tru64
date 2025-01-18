# $Id: Makefile,v 1.1.1.1 1998/12/29 21:36:14 paradis Exp $
# $Log: Makefile,v $
# Revision 1.1.1.1  1998/12/29 21:36:14  paradis
# Initial CVS checkin
#
# Revision 1.51  1997/12/15  20:45:52  pbell
# Updated for dp264.
#
# Revision 1.50  1997/06/02  17:42:23  fdh
# Updated include file dependencies.
#
# Revision 1.49  1997/05/27  16:08:06  fdh
# Modified the assembler definitions.
#
# Revision 1.48  1997/05/19  15:25:58  fdh
# Updated include file dependencies.
#
# Revision 1.47  1997/04/23  18:17:09  fdh
# Updated include file dependencies.
#
# Revision 1.46  1997/04/23  18:13:20  fdh
# Updated include file dependencies.
#
# Revision 1.45  1997/04/10  17:13:39  fdh
# Modified the rule for making library archive.
#
# Revision 1.44  1997/03/17  10:36:33  fdh
# Modified to get the object file list from the source file list.
#
# Revision 1.43  1997/02/26  19:48:13  fdh
# Modified the CCFLAGS definition.
#
# Revision 1.42  1997/02/23  11:48:59  fdh
# Modified CCFLAGS definition.
#
# Revision 1.41  1997/02/21  19:54:44  fdh
# Updated include file dependencies.
#
# Revision 1.40  1996/05/22 21:53:45  fdh
# Updated include file dependencies.
#
# Revision 1.39  1995/12/23  23:46:16  fdh
# Updated include file dependencies.
#
# Revision 1.38  1995/11/03  22:15:14  cruz
# Added lint library to list of items to remove when
# performing a clean.
#
# Revision 1.37  1995/10/31  18:48:51  cruz
# Fixed up lint rule.
#
# Revision 1.36  1995/10/30  17:06:29  cruz
# Added lint rule.]
#
# Revision 1.35  1995/10/23  22:10:10  fdh
# Updated include file dependencies.
#
# Revision 1.34  1995/10/23  21:11:34  fdh
# Updated include file dependencies.
#
# Revision 1.33  1995/10/23  20:46:21  cruz
# Update dependency list.
#
# Revision 1.32  1995/10/18  12:58:55  fdh
# Added lint: target.
#
# Revision 1.31  1995/10/13  20:10:11  fdh
# Renamed libdbug. files to ladebug. names. i.e. Same as directory name.
#
# Revision 1.30  1995/10/13  18:02:54  fdh
# Updated clean: target.
#
# Revision 1.29  1995/10/10  15:23:41  fdh
# Renamed eb64l to pc64.
#
# Revision 1.28  1995/10/10  14:16:34  fdh
# Renamed ladbx.h to ladebug.h.
#
# Revision 1.27  1995/10/05  01:55:51  fdh
# Updated include file dependencies.
#
# Revision 1.26  1995/10/03  03:50:41  fdh
# Update include file dependencies.
#
# Revision 1.25  1995/09/12  22:00:29  fdh
# Updated include file dependencies.
#
# Revision 1.24  1995/09/03  06:55:10  fdh
# Updated include file dependencies.
#
# Revision 1.23  1995/08/31  21:39:55  fdh
# Updated include file dependencies
#
# Revision 1.22  1995/08/30  23:24:47  fdh
# Update include file dependencies...
#
# Revision 1.21  1995/08/25  21:18:21  fdh
# Updated include file dependencies...
#
# Revision 1.20  1995/08/25  20:09:01  fdh
# Added ladbxusr.s.
#
# Revision 1.19  1995/02/24  15:53:38  fdh
# Updated include file dependencies.
#
# Revision 1.18  1994/12/08  17:52:32  fdh
# Updated include file dependency list.
#
# Revision 1.17  1994/11/24  04:31:14  fdh
# Updated include file dependency lists.
#
# Revision 1.16  1994/11/24  04:19:24  fdh
# Updated include file dependency list.
#
# Revision 1.15  1994/11/23  19:54:20  fdh
# Updated include file dependencies.
#
# Revision 1.14  1994/11/23  15:43:22  fdh
# Modified include path definitions...
#
# Revision 1.13  1994/11/22  23:15:16  fdh
# Modified to use the INCDIR definition.
#
# Revision 1.12  1994/11/11  20:48:06  fdh
# Updated include file dependency list.
#
# Revision 1.11  1994/11/07  12:43:39  rusling
# Fixed up dependencies
#
# Revision 1.10  1994/11/04  16:38:16  fdh
# Updated include file dependency list.
# .,
#
# Revision 1.9  1994/07/21  13:55:10  fdh
# Added -f flag to rm commands.
#
# Revision 1.8  1994/07/11  19:49:05  fdh
# Update include file dependencies.
#
# Revision 1.7  1994/07/01  15:13:39  fdh
# Updated include dependencies...
#
# Revision 1.6  1994/06/28  20:12:18  fdh
# Modified filenames and build precedure to fit into a FAT filesystem.
#
# Revision 1.5  1994/06/27  12:09:06  berent
# Regenerated h file dependencides following changes to   readloop.c
#
# Revision 1.4  1994/06/17  19:36:43  fdh
# Clean-up...
#
# Revision 1.3  1994/06/10  14:45:06  fdh
# Updated include file dependencies.
#
# Revision 1.2  1994/04/06  04:32:34  fdh
# Modified dependencies for kutil.s
#
# Revision 1.1  1994/03/11  05:10:26  fdh
# Initial revision
#

CONFIG_DIR=../..
include $(CONFIG_DIR)/sdkmake.conf
include ../rules.gnu

# -------------------------------------------------
# Append local cc flags here for only this Makefile
# -------------------------------------------------
CCFLAGS = $(CCFLAGS_GLOBAL)
#CCFLAGS = $(CCFLAGS) -DDUMP_PACKETS
#CCFLAGS = $(CCFLAGS) -DTRACE_REGISTERS
#CCFLAGS = $(CCFLAGS) -DPRTRACE

TARGET_SRCS = 

ALL_TARGET_SRCS =

AS_SRCS  = \
	$(SRCDIR)kutil.s \
	$(SRCDIR)ladbxusr.s

C_SRCS   = \
	$(SRCDIR)bptable.c \
	$(SRCDIR)kernel.c \
	$(SRCDIR)pkthandl.c \
	$(SRCDIR)pktutil.c \
	$(SRCDIR)readloop.c

OBJFILES = $(AS_SRCS:%.s=$(OBJDIR)%.$(OBJ)) $(TARGET_SRCS:%.c=$(OBJDIR)%.$(OBJ)) $(C_SRCS:%.c=$(OBJDIR)%.$(OBJ))

all:  $(OBJDIR)ladebug.$(LIBEXT)

#----------------
ifeq "$(EB_BUILD_SYSTEM)" "WINNT"
$(OBJDIR)ladebug.$(LIBEXT): $(OBJFILES)
	$(DELETE) $(OBJDIR)ladebug.$(LIBEXT)
	$(LIBRARY) -VERBOSE -OUT:$(OBJDIR)ladebug.$(LIBEXT) $(OBJFILES)
else
$(OBJDIR)ladebug.$(LIBEXT): $(OBJFILES)
	$(DELETE) $(OBJDIR)ladebug.$(LIBEXT)
	$(AR) rcs $(OBJDIR)ladebug.$(LIBEXT) $(OBJFILES)
endif

#----------------------------------------------------------------------------
$(OBJDIR)bptable.$(OBJ): $(SOURCEDIR)bptable.c
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

$(OBJDIR)kutil.$(OBJ):   $(SOURCEDIR)kutil.s
	$(AS) $(ASFLAGS) -o $@ -c $(INCLUDES) $<

$(OBJDIR)kernel.$(OBJ):  $(SOURCEDIR)kernel.c
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

$(OBJDIR)pkthandl.$(OBJ): $(SOURCEDIR)pkthandl.c
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

$(OBJDIR)pktutil.$(OBJ): $(SOURCEDIR)pktutil.c
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

$(OBJDIR)readloop.$(OBJ):    $(SOURCEDIR)readloop.c
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

$(OBJDIR)ladbxusr.$(OBJ):   $(SOURCEDIR)ladbxusr.s
	$(AS) $(ASFLAGS) -o $@ -c $(INCLUDES) $<


#----------------------------------------------------------------------------

depend: $(AS_SRCS) $(C_SRCS) $(ALL_TARGET_SRCS)
	$(MAKEDEPEND) -p'$$(OBJDIR)' -- $(DEPENDFLAGS) -- \
	$(AS_SRCS) $(C_SRCS) $(ALL_TARGET_SRCS)

clean:
	$(DELETE) *~ \#*\# core ${MAKEFILE}.bak \
		$(OBJFILES) \
		$(OBJDIR)cstartup.$(OBJ) \
		$(OBJDIR)llib-lladebug.ln \
		$(OBJDIR)ladebug.$(LIBEXT)
	$(CLEAN)

#----------------------------------------------------------------------------
# DO NOT DELETE THIS LINE -- make depend depends on it.


