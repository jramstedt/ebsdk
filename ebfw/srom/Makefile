CONFIG_DIR=../..
include $(CONFIG_DIR)/sdkmake.conf

include $(EB_BASE)/rules.gnu

ALPHA_TOOLS 	= \
	AS=alpha-linux-gnu-as \
	AR=alpha-linux-gnu-ar \
	LD=alpha-linux-gnu-ld \
	CC=alpha-linux-gnu-gcc \
	CPP=alpha-linux-gnu-cpp \
	OBJCOPY=alpha-linux-gnu-objcopy \
	OBJDUMP=alpha-linux-gnu-objdump

default_STYLE_TARGETS = tardef
lx164_STYLE_TARGETS   = $(default_STYLE_TARGETS) tar0
sx164_STYLE_TARGETS   = tardefrom tar1 tar2 tar3

ifdef $(TARGET)_STYLE_TARGETS
STYLE_TARGETS = $($(TARGET)_STYLE_TARGETS)
else
STYLE_TARGETS = $(default_STYLE_TARGETS)
endif

default_SROMMAKEFILE	= $(TOP)/srom/Makefile.srm
dp264_SROMMAKEFILE	= Makefile

ifdef $(TARGET)_SROMMAKEFILE
SROMMAKEFILE = $($(TARGET)_SROMMAKEFILE)
else
SROMMAKEFILE = $(default_SROMMAKEFILE)
endif

.PHONY = all tardef tardefrom tar0 tar1 tar2 tar3 clean distclean depend

all : $(STYLE_TARGETS)

tardef :
	$(MAKE) -C $(TARGET) $(ALPHA_TOOLS) SUBTARGET=_sr SUBTARGETEXT=pad -f $(SROMMAKEFILE) $(GOAL)

tardefrom:
	$(MAKE) -C $(TARGET) $(ALPHA_TOOLS) SUBTARGET=_sr SUBTARGETEXT=rom -f $(SROMMAKEFILE) $(GOAL)

tar0:
	$(MAKE) -C $(TARGET) $(ALPHA_TOOLS) EB_DEBUG_SROM=0 SUBTARGET=sr0 SUBTARGETEXT=pad -f $(SROMMAKEFILE) $(GOAL)

tar1:
	$(MAKE) -C $(TARGET) $(ALPHA_TOOLS) EB_DEBUG_SROM=1 SUBTARGET=sr1 SUBTARGETEXT=rom -f $(SROMMAKEFILE) $(GOAL)

tar2:
	$(MAKE) -C $(TARGET) $(ALPHA_TOOLS) EB_DEBUG_SROM=2 SUBTARGET=sr2 SUBTARGETEXT=pad -f $(SROMMAKEFILE) $(GOAL)

tar3:
	$(MAKE) -C $(TARGET) $(ALPHA_TOOLS) EB_DEBUG_SROM=3 SUBTARGET=sr3 SUBTARGETEXT=pad -f $(SROMMAKEFILE) $(GOAL)

clean distclean :
	@$(MAKE) GOAL=$@ -f Makefile
	rm -f *.o *~ \#*\# *.bak *.a core
