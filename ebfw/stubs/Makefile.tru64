# $Id: Makefile,v 1.1.1.1 1998/12/29 21:36:15 paradis Exp $
# $Log: Makefile,v $
# Revision 1.1.1.1  1998/12/29 21:36:15  paradis
# Initial CVS checkin
#
# Revision 1.8  1997/12/15  19:03:58  pbell
# Added sflash.
#
# Revision 1.7  1997/06/02  17:42:19  fdh
# Updated include file dependencies.
#
# Revision 1.6  1997/05/19  15:25:53  fdh
# Updated include file dependencies.
#
# Revision 1.5  1997/04/23  18:17:06  fdh
# Updated include file dependencies.
#
# Revision 1.4  1997/04/10  17:07:12  fdh
# Modified the rule for making library archive.
#
# Revision 1.3  1997/03/17  10:33:42  fdh
# Create library (stub.a) containing stubs.
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
	$(SRCDIR)bios_dbg.c \
	$(SRCDIR)biosem.c \
	$(SRCDIR)cxx.c \
	$(SRCDIR)dpml.c \
	$(SRCDIR)fflush.c \
	$(SRCDIR)stdlib.c \
	$(SRCDIR)sflash.c \
	$(SRCDIR)stub_pci.c

OBJFILES = $(AS_SRCS:%.s=$(OBJDIR)%.$(OBJ)) $(TARGET_SRCS:%.c=$(OBJDIR)%.$(OBJ)) $(C_SRCS:%.c=$(OBJDIR)%.$(OBJ))

all:  $(OBJDIR)stubs.$(LIBEXT)

vars:
	@echo OBJFILES is $(OBJFILES)

#----------------------------------------------------------------------------
$(OBJDIR)stdlib.$(OBJ):		stdlib.c
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

$(OBJDIR)sflash.$(OBJ):		sflash.c
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

$(OBJDIR)dpml.$(OBJ):		dpml.c
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

$(OBJDIR)biosem.$(OBJ):		biosem.c
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

$(OBJDIR)bios_dbg.$(OBJ):		bios_dbg.c
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

$(OBJDIR)cxx.$(OBJ):			cxx.c
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

$(OBJDIR)fflush.$(OBJ):		fflush.c
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

$(OBJDIR)stub_pci.$(OBJ):		stub_pci.c
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<


#----------------

ifeq "$(EB_BUILD_SYSTEM)" "WINNT"
$(OBJDIR)stubs.$(LIBEXT): $(OBJFILES)
	$(DELETE) $(OBJDIR)stubs.$(LIBEXT)
	$(LIBRARY) -VERBOSE -OUT:$(OBJDIR)stubs.$(LIBEXT) $(OBJFILES)
else
$(OBJDIR)stubs.$(LIBEXT): $(OBJFILES)
	$(DELETE) $(OBJDIR)stubs.$(LIBEXT)
	$(AR) rcs $(OBJDIR)stubs.$(LIBEXT) $(OBJFILES)
endif

#----------------------------------------------------------------------------

depend: $(SRCS)
	$(MAKEDEPEND) -p'$$(OBJDIR)' -- $(DEPENDFLAGS) -- \
	$(AS_SRCS) $(C_SRCS) $(ALL_TARGET_SRCS)

clean:
	$(DELETE) *~ \#*\# core ${MAKEFILE}.bak \
		$(OBJFILES) \
		$(OBJDIR)cstartup.$(OBJ) \
		$(OBJDIR)llib-lstubs.ln \
		$(OBJDIR)stubs.$(LIBEXT)
	$(CLEAN)

distclean: clean
	$(MAKEDEPEND) -f $(MAKEFILE)
	$(DELETE) Makefile.bak

#----------------------------------------------------------------------------
# DO NOT DELETE THIS LINE -- make depend depends on it.
