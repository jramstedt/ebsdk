# $Id: Makefile,v 1.1.1.1 1998/12/29 21:36:07 paradis Exp $
# $Log: Makefile,v $
# Revision 1.1.1.1  1998/12/29 21:36:07  paradis
# Initial CVS checkin
#
# Revision 1.10  1998/08/20  19:40:17  thulin
# Add clubs defines
#
# Revision 1.9  1997/12/17  15:12:13  pbell
# Added the HAL definition to the secondary make call.
#
# Revision 1.8  1997/12/15  19:36:06  pbell
# Added the dp264 MAKEFILE default.
#
# Revision 1.7  1997/08/12  04:52:45  fdh
# Added hooks for overriding which makefile is used to
# build PALcode.
#
# Revision 1.6  1995/10/31  15:27:44  cruz
# Added lint target.
#
# Revision 1.5  1995/02/23  21:55:50  fdh
# No longer uses the SYSREVID Macro.
#
# Revision 1.4  1994/06/28  20:10:21  fdh
# Modified filenames and build precedure to fit into a FAT filesystem.
#
# Revision 1.3  1994/06/23  15:59:07  fdh
# Fix clean: target.
#
# Revision 1.2  1994/06/20  15:32:15  fdh
# Use path to ../Makefile.dbm instead of soft link.
#
# Revision 1.1  1994/06/10  14:44:23  fdh
# Initial revision
#

CONFIG_DIR=../..
include $(CONFIG_DIR)/sdkmake.conf
include ../rules.gnu

default_PALMAKEFILE	= ../Makefile.dbm
dp264_PALMAKEFILE	= Makefile.dbm

# For GNU Make
ifneq "$($(TARGET)_PALMAKEFILE)" ""
PALMAKEFILE = $($(TARGET)_PALMAKEFILE)
else
PALMAKEFILE = $(default_PALMAKEFILE)
endif

all:
	$(EB_MAKE) -C $(TARGETDIR) MAKEFILE=$(PALMAKEFILE) -f $(PALMAKEFILE) HAL=$(HAL)

vars:
	@echo TARGET is $(TARGET)
	@echo $(TARGET)_PALMAKEFILE is $($(TARGET)_PALMAKEFILE)
	@echo PALMAKEFILE is $(PALMAKEFILE)

clean:
	$(EB_MAKE) -C $(TARGETDIR) MAKEFILE=$(PALMAKEFILE) -f $(PALMAKEFILE) clean
	$(DELETE) *~ \#*\# core ${MAKEFILE}.bak
	$(CLEAN)

distclean:
	$(EB_MAKE) -C $(TARGETDIR) MAKEFILE=$(PALMAKEFILE) -f $(PALMAKEFILE) distclean

depend:
	$(EB_MAKE) -C $(TARGETDIR) MAKEFILE=$(PALMAKEFILE) -f $(PALMAKEFILE) depend

