CONFIG_DIR=../../../..
include $(CONFIG_DIR)/sdkmake.conf

include $(EB_BASE)/rules.gnu

AS				= as
LD				= ld
CC				= gcc
CPP				= cpp
ALIST			= $(TOOLBOX)/alist
ASTRIP		= $(TOOLBOX)/astrip
SYSGEN		= $(TOOLBOX)/sysgen

ALPHA_TOOLS = \
	AS=alpha-linux-gnu-as \
	LD=alpha-linux-gnu-ld \
	CC=alpha-linux-gnu-gcc \
	CPP=alpha-linux-gnu-cpp

INCLUDES	= -I. -I../include

CCFLAGS		= -g $(HEADER_DEFS) -DNT_UP -mcpu=$(CPU) -mtune=$(CPU) -Os
ASFLAGS		= -g $(INCLUDES) -mcpu=$(CPU) -mtune=native -x assembler-with-cpp
CFLAGS		= -g $(INCLUDES)
ASFLAGS		= # -P
LINT			= /usr/bin/lint
LINTFLAGS	=  -1 -wp -av -n $(INCLUDES)

HEADER_DEFS = \
	-DDECOMP_PALBASE=0x$(DECOMP_PALBASE) \
	-DDECOMP_BASE=0x$(DECOMP_BASE)
#	-DDECOMP_COMPRESSED=0x$(DECOMP_COMPRESSED)

SRCS  = bits.c deflate.c hfcomp.c inflate.c trees.c

DEPDIR := .deps
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

INSTALL_TARGETS = hfcomp decmp64.img decmp164.img decmp264.img

all : hfcomp decomp64 decomp164 decomp264

hfcomp : $(SRCS:.c=.o)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(HEADER_DEFS) $(OUTPUT_OPTION) $(TARGET_ARCH) $^

### Decompressor image
MAKEFILE = Makefile

CPU_64          = 21064
CPU_164         = 21164
CPU_264         = 21264
CPU             = $(CPU_$(TYPE))

CPU_DTBMISS_64  = 9E0
CPU_DTBMISS_164 = 200
CPU_DTBMISS_264 = 300
CPU_DTBMISS     = $(CPU_DTBMISS_$(TYPE))

decomp64 :
	$(MAKE) -f $(MAKEFILE) $(ALPHA_TOOLS) TYPE=64 decomp_target

decomp164 :
	$(MAKE) -f $(MAKEFILE) $(ALPHA_TOOLS) TYPE=164 decomp_target

decomp264 :
	$(MAKE) -f $(MAKEFILE) $(ALPHA_TOOLS) TYPE=264 decomp_target

### Decompressor

cstartup.sp : cstartup.s
	$(CPP) $(CPPFLAGS) $(INCLUDES) -DNO_INITDATA -DNO_EXITDATA $^ > $@

cstartup.o : cstartup.sp
	$(AS) $(ASFLAGS) -m$(CPU) -o $@ $^

decmp$(TYPE).o : decomp.c
	$(CC) $(CCFLAGS) -DROM -o $@ -c $^

inf_s$(TYPE).o : inflate.c
	$(CC) $(CCFLAGS) -DROM -o $@ -c $^

dbmmn$(TYPE).o : dbmmain.c
	$(CC) $(CCFLAGS) -o $@ -c $^

decmp$(TYPE).out : cstartup.o decmp$(TYPE).o inf_s$(TYPE).o dbmmn$(TYPE).o
	$(LD) -N -Ttext=$(DECOMP_BASE) -z noexecstack -e __start -o $@ $^
#	$(LD) -N -segment_page 16 -T $(DECOMP_BASE) -non_shared -o $@ $^
# https://web.archive.org/web/20070226182734/http://h30097.www3.hp.com:80/docs/base_doc/DOCUMENTATION/V50_HTML/MAN/MAN1/0241____.HTM

### micropal

# Preprocesses micropal.s to upal$(TYPE).sp
upal$(TYPE).sp : micropal.s
	$(CPP) $(CPPFLAGS) $(INCLUDES) -DDC$(CPU) -DPVCCHECK -DASSEMBLY_CODE $(HEADER_DEFS) $^ > $@

upal$(TYPE).o : upal$(TYPE).sp
	$(AS) $(ASFLAGS) -m$(CPU) -o $@ $^
#	$(AS) $(ASFLAGS) -$(CPU) -o $@ $^

# Strip all header sections (text and data)
upal$(TYPE).nh : upal$(TYPE).o
	/usr/alpha-linux-gnu/bin/objcopy --dump-section .text=$@ $<
#	$(ASTRIP) -a -v $< $@

upal$(TYPE).ent :
	echo 0 uPalReset > $@
	echo $(CPU_DTBMISS) PalDtbMiss >> $@

upal$(TYPE).lis : upal$(TYPE).o
#	$(ALIST) $< > $@
	/usr/alpha-linux-gnu/bin/objdump -x $< > $@

upal$(TYPE).map : upal$(TYPE).o
#	$(ALIST) -m $< > $@
	/usr/alpha-linux-gnu/bin/objdump -x $< | grep -i "pvc\\$$" > $@

### decmp

decmp$(TYPE).nh : decmp$(TYPE).out
	/usr/alpha-linux-gnu/bin/objcopy --dump-section .text=$@ $<

decmp$(TYPE).img : upal$(TYPE).nh decmp$(TYPE).nh pvc$(TYPE).pvc
	$(SYSGEN) -v \
		-e0		-s upal$(TYPE).nh \
		-eb00	-s decmp$(TYPE).nh \
		-o $@
#	$(PVC) < pvc$(TYPE).pvc

### pvc
pvc$(TYPE).pvc : upal$(TYPE).nh upal$(TYPE).ent upal$(TYPE).map
	echo set code_file upal$(TYPE).nh > $@
	echo set entry_file upal$(TYPE).ent >> $@
	echo set cpu $(CPU) >> $@
	echo go >> $@
	echo quit >> $@

### Image and listing
decomp_target : decmp$(TYPE).img upal$(TYPE).lis


install : all
	cp -fp $(INSTALL_TARGETS) $(EB_TOOLBOX)/.
	chmod 775 $(addprefix $(EB_TOOLBOX)/, $(INSTALL_TARGETS))

lint :
	$(LINT) $(LINTFLAGS) $(INCLUDES) $(SRCS)

clean :
	rm -f *.o *~ \#*\# *.bak *.a core \
	*.nh decmp*.out *.lis *.dis \
	*.map *.pvc *.ent *.sp \
	decmp*.img \
	hfcomp

# Dependencies
DEPDIR := .deps
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c

%.o : %.c
%.o : %.c $(DEPDIR)/%.d | $(DEPDIR)
		$(COMPILE.c) $(OUTPUT_OPTION) $<

$(DEPDIR): ; @mkdir -p $@

DEPFILES := $(SRCS:%.c=$(DEPDIR)/%.d)
$(DEPFILES):

include $(wildcard $(DEPFILES))
