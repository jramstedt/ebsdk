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

default_PALMAKEFILE	= ../Makefile.dbm
dp264_PALMAKEFILE	= Makefile.dbm

ifdef $(TARGET)_PALMAKEFILE
PALMAKEFILE = $($(TARGET)_PALMAKEFILE)
else
PALMAKEFILE = $(default_PALMAKEFILE)
endif

.PHONY = all vars clean distclean pvc

all :
	$(MAKE) -C $(TARGETDIR) $(ALPHA_TOOLS) -f $(PALMAKEFILE)

pvc : 
	$(MAKE) -C $(TARGETDIR) $(ALPHA_TOOLS) -f $(PALMAKEFILE) pvc

vars :
	@echo TARGET is $(TARGET)
	@echo $(TARGET)_PALMAKEFILE is $($(TARGET)_PALMAKEFILE)
	@echo PALMAKEFILE is $(PALMAKEFILE)

clean distclean :
	$(MAKE) -C $(TARGETDIR) $(ALPHA_TOOLS) -f $(PALMAKEFILE) $@
	rm -f *.o *~ \#*\# *.bak *.a core
