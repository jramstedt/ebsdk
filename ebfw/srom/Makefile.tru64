# $Id: Makefile,v 1.4 1999/04/15 21:30:49 paradis Exp $
# $Log: Makefile,v $
# Revision 1.4  1999/04/15 21:30:49  paradis
# Scrubbed out RX164 references
#
# Revision 1.3  1999/01/29 13:31:55  bissen
# Made changes to build in the correct mini-debuger image at
# the end of the srom code.
#
# Revision 1.2  1999/01/19 19:01:30  bissen
# Changed build targets for RX164
#
# Revision 1.1.1.1  1998/12/29 21:36:12  paradis
# Initial CVS checkin
#
# Revision 1.10  1998/08/19  20:02:55  thulin
# Added clubs module
#
# Revision 1.9  1997/12/17  15:17:20  pbell
# Changed the DP264 Makefile name
#
# Revision 1.8  1997/12/15  19:09:53  pbell
# Enhanced for dp264.
#
# Revision 1.7  1997/08/21  15:32:08  pbell
# Fixed sx srom build extensions.
#
# Revision 1.6  1997/07/21  18:44:47  pbell
# Added support for lx and sx multiple configuration srom builds.
#
# Revision 1.5  1995/11/15  22:30:05  cruz
# Updated this makefile so it uses a common Makefile to
# build all of the SROM files.
#
# Revision 1.4  1995/10/31  15:40:43  cruz
# Added lint target
#
# Revision 1.3  1994/06/28  20:10:51  fdh
# Modified filenames and build precedure to fit into a FAT filesystem.
#
# Revision 1.2  1994/06/27  18:02:35  fdh
# Fix clean target.
#
# Revision 1.1  1994/03/11  05:17:15  fdh
# Initial revision
#

CONFIG_DIR=../..
include $(CONFIG_DIR)/sdkmake.conf
include ../rules.gnu

default_STYLE_TARGETS = tardef
lx164_STYLE_TARGETS   = $(default_STYLE_TARGETS) tar0
sx164_STYLE_TARGETS   = tardefrom tar1 tar2 tar3

ifneq "$($(TARGET)_STYLE_TARGETS)" ""
STYLE_TARGETS = $($(TARGET)_STYLE_TARGETS)
else
STYLE_TARGETS = $(default_STYLE_TARGETS)
endif


default_SROMMAKEFILE	= $(TOP)/srom/Makefile.srm
dp264_SROMMAKEFILE	= Makefile

# For GNU Make
ifneq "$($(TARGET)_SROMMAKEFILE)" ""
SROMMAKEFILE = $($(TARGET)_SROMMAKEFILE)
else
SROMMAKEFILE = $(default_SROMMAKEFILE)
endif

all: $(STYLE_TARGETS)

tardef:
	  $(EB_MAKE) -C $(TARGET) SUBTARGET=_sr SUBTARGETEXT=pad MAKEFILE=$(SROMMAKEFILE) -f $(SROMMAKEFILE) $(CLEANUP)

tardefrom:
	  $(EB_MAKE) -C $(TARGET) SUBTARGET=_sr SUBTARGETEXT=rom MAKEFILE=$(SROMMAKEFILE) -f $(SROMMAKEFILE) $(CLEANUP)

tar0:
	  $(EB_MAKE) -C $(TARGET) EB_DEBUG_SROM=0 SUBTARGET=sr0 SUBTARGETEXT=pad MAKEFILE=$(SROMMAKEFILE) -f $(SROMMAKEFILE) $(CLEANUP)

tar1:
	  $(EB_MAKE) -C $(TARGET) EB_DEBUG_SROM=1 SUBTARGET=sr1 SUBTARGETEXT=rom MAKEFILE=$(SROMMAKEFILE) -f $(SROMMAKEFILE) $(CLEANUP)

tar2:
	  $(EB_MAKE) -C $(TARGET) EB_DEBUG_SROM=2 SUBTARGET=sr2 SUBTARGETEXT=pad MAKEFILE=$(SROMMAKEFILE) -f $(SROMMAKEFILE) $(CLEANUP)

tar3:
	  $(EB_MAKE) -C $(TARGET) EB_DEBUG_SROM=3 SUBTARGET=sr3 SUBTARGETEXT=pad MAKEFILE=$(SROMMAKEFILE) -f $(SROMMAKEFILE) $(CLEANUP)

clean:
	$(EB_MAKE) CLEANUP=clean MAKEFILE=Makefile -f Makefile
	$(DELETE) *~ \#*\# core ${MAKEFILE}.bak
	$(CLEAN)

depend:
	$(EB_MAKE) CLEANUP=depend MAKEFILE=Makefile -f Makefile

distclean:	clean
	$(EB_MAKE) CLEANUP=distclean MAKEFILE=Makefile -f Makefile

