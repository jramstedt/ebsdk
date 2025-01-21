# -----------------------------------------------------
# Makefile.rules
# This Makefile contains global settings that are
# included in other Makefiles.
# -----------------------------------------------------


# -----------------------------------------------------
# Figure out what platform we're bulding on; some rules
# change depending on whether we'er buliding under
# NT, Digital UNIX, or Linux
# -----------------------------------------------------

ifeq "$(OS)" "Windows_NT"
    EB_BUILD_SYSTEM=WINNT
else
  ifeq "$(shell uname -s)" "Linux"
      EB_BUILD_SYSTEM=LINUX
  else
    ifeq "$(shell uname -s)" "OSF1"
        EB_BUILD_SYSTEM=UNIX
    else
        EB_BUILD_SYSTEM=UNSUPPORTED
    endif
  endif
endif


ifeq "$(EB_BUILD_SYSTEM)" "WINNT"
.SUFFIXES: .c .s .obj .out .nh .run .sr .nm .dis .lis
OBJ=obj
LIBEXT=lib
else
.SUFFIXES: .c .s .o .out .nh .run .sr .nm .dis .lis
OBJ=o
LIBEXT=a
endif

TARGET_MACHINE = ALPHA
#----------------------------------------------------
# Environment variables reflect your installation,
# using these generate some local macros for the
# builds.
#----------------------------------------------------
# NOTE:
#  You must define the following 3 environment variables.
#       EB_BASE
#       EB_TOOLBOX
#       EB_DEFAULT_TARGET
#----------------------------------------------------
TOP                     = $(EB_BASE)
TOOLBOX                 = $(EB_TOOLBOX)

ifeq "$(TARGET)" ""
TARGET                  = $(EB_DEFAULT_TARGET)
endif


#----------------------------------------------------
# Setting the EB_REVID_EBxx environment variables to
# "_p1" selects builds for Pass 1.0 prototypes.
# This variable is normally left undefined.
#----------------------------------------------------
lx164_REVID             = $(EB_REVID_LX164)
sx164_REVID             = $(EB_REVID_SX164)
dp164_REVID             = $(EB_REVID_DP264)
SYSREVID                = $($(TARGET/$/_REVID))

#----------------------------------------------------
# Determines CPU type based on TARGET board.
#----------------------------------------------------
lx164_CPUTYPE           = 21164
sx164_CPUTYPE           = 21164PC
dp264_CPUTYPE           = 21264
CPUTYPE                 = $($(TARGET)_CPUTYPE)


#----------------------------------------------------------
# Determines the ROM OFFSET field in the FSB Makerom header
#----------------------------------------------------------
sx164_FSB_ROMOFFSET     = 0x10000
DEFAULT_FSB_ROMOFFSET   = 0x00000

ifneq "$($(TARGET)_FSB_ROMOFFSET)" ""
FSB_ROMOFFSET = $($(TARGET)_FSB_ROMOFFSET)
else
FSB_ROMOFFSET = $(DEFAULT_FSB_ROMOFFSET)
endif

#----------------------------------------------------
# Defines the directories needed for builds to work.
#----------------------------------------------------
TARGETDIR       = $(TARGET)$(SYSREVID)

ifneq "$(EB_OBJECT)" ""
OBJDIR          = $(EB_OBJECTS)/$(TARGETDIR)/
else
OBJDIR          = $(TOP)/obj/$(TARGETDIR)/
endif

ifneq "$(EB_BOOTDIR)" ""
BOOTDIR         = $(EB_BOOTDIR)/$(TARGETDIR)/
else
BOOTDIR         = $(TOP)/boot/$(TARGETDIR)/
endif

RELDIR          = release
DBGDIR          = debug

ifeq "$(EB_BUILD_SYSTEM)" "WINNT"
COMMONLIB       = $(TOP)/library/ntalpha/
else
COMMONLIB       = $(TOP)/library/unix/
endif
LIBOBJ          = $(COMMONLIB)$(TARGET)/

#----------------------------------------------------
# Defines the physical location of PALcode and DBM.
#----------------------------------------------------
# Hex values to represent the Base of PALcode and
# the entry point used for the Debug Monitor.
# When these values are changed both PALcode and the
# Debug must be rebuilt.
dp264_PALBASE           = 8000
default_PALBASE         = 4000

ifneq "$($(TARGET)_PALBASE)" ""
PALBASE                 = $($(TARGET)_PALBASE)
else
PALBASE                 = $(default_PALBASE)
endif

default_DBMENTRY        = 10000

ifneq "$($(TARGET)_DBMENTRY)" ""
DBMENTRY                = $($(TARGET)_DBMENTRY)
else
DBMENTRY                = $(default_DBMENTRY)
endif

default_FSBENTRY        = 10000

ifneq "$($(TARGET)_FSBENTRY)" ""
FSBENTRY                = $($(TARGET)_FSBENTRY)
else
FSBENTRY                = $(default_FSBENTRY)
endif

# Hex values to represent the Base of PALcode
# used in the decompressor and the entry point
# used for the decompressor.  The DECOMP_COMPRESSED
# variable represents the base of the compressed
# image. When these values are changed the hfcomp
# tool should be rebuilt.

DECOMP_PALBASE          = 300000
DECOMP_BASE             = 300B00
DECOMP_COMPRESSED       = 303100

#----------------------------------------------------
# BOOTADR defines the default location for
# targeting downloaded programs.
#----------------------------------------------------
default_BOOTADR         = 300000

ifneq "$($(TARGET)_BOOTADR)" ""
BOOTADR                 = $($(TARGET)_BOOTADR)
else
BOOTADR                 = $(default_BOOTADR)
endif

#----------------------------------------------------
# Defines the location of the tools used.
#----------------------------------------------------
# ----------------------------
#!!! OSF/1 and COFF TOOLS !!!!
# ----------------------------
ifeq "$(EB_BUILD_SYSTEM)" "WINNT"
AS		= $(TOOLBOX)/winas
ASFLAGS		= /nologo
CC		= $(TOOLBOX)/wincc /nologo
LINK		= LINK
LD		= ChangeThisToLink
CPP		= CL /nologo /E
CPPOUT		= >
LIBRARY		= lib
STRIP		= $(TOOLBOX)/strip
STRIP_FLAGS	= 
LIST		= $(TOOLBOX)/clist
SREC            = $(TOOLBOX)/srec
HEX             = $(TOOLBOX)/hex32
MAKEROM         = $(TOOLBOX)/makerom
SYSGEN          = $(TOOLBOX)/sysgen
COMPRESS        = $(TOOLBOX)/hfcomp
SROM            = $(TOOLBOX)/srom
HEXPAD          = $(TOOLBOX)/hexpad
GAS             = $(TOOLBOX)/gas
ASTRIP          = $(TOOLBOX)/astrip
ALIST           = $(TOOLBOX)/alist
HAL             = $(TOOLBOX)/hal
CAT		= $(TOOLBOX)/cat
COPY		= $(TOOLBOX)/cat
COPY_TO		= >
DELETE		= $(TOOLBOX)/rm -f
CHMOD		= $(TOOLBOX)/chmod
MAKEDEPEND	= $(TOOLBOX)/makedepend
ARCH_EV4	= /QA21064
ARCH_EV5	= /QA21164
ARCH_EV56	= /QA21164A
ARCH_EV6	= /QA21264
endif

ifeq "$(EB_BUILD_SYSTEM)" "UNIX"
AS              = /usr/bin/cc
ASFLAGS         =
CC              = /usr/bin/cc
LD              = /usr/bin/cc
CPP             = /usr/bin/cpp
CPPOUT		=
STRIP           = $(TOOLBOX)/cstrip
STRIP_FLAGS     = -v -a
LIST            = $(TOOLBOX)/clist
CXX             = cxx
GCC                     = $(TOOLBOX)/xgcc
GLD                     = $(TOOLBOX)/gld
SREC                    = $(TOOLBOX)/srec
HEX                     = $(TOOLBOX)/hex32
MAKEROM                 = $(TOOLBOX)/makerom
SYSGEN                  = $(TOOLBOX)/sysgen
COMPRESS                = $(TOOLBOX)/hfcomp
SROM                    = $(TOOLBOX)/srom
HEXPAD                  = $(TOOLBOX)/hexpad
GAS                     = $(TOOLBOX)/gas
ASTRIP                  = $(TOOLBOX)/astrip
ALIST                   = $(TOOLBOX)/alist
HAL                     = $(TOOLBOX)/hal
COPY			= cp -p
COPY_TO			=
CAT			= cat
DELETE			= rm -f
CHMOD			= chmod
MAKEDEPEND		= makedepend
IDENTIFY                = /usr/bin/ident
AR                      = /usr/bin/ar
NM                      = /usr/bin/nm
DIS                     = /usr/bin/dis

ARCH_EV4	= -arch ev4
ARCH_EV5	= -arch ev5
ARCH_EV56	= -arch ev56
ARCH_EV6	= -arch ev6
endif

#----------------------------------------------------
# Defines global flags to be used.
#----------------------------------------------------
SROM_FLAGS_GLOBAL       = -v -$(CPUTYPE)
HEXPAD_FLAGS_GLOBAL     = -v
ASTRIP_FLAGS            = -v


INCLUDES        = -I. -I../h -I../palcode/$(TARGETDIR)
SROM_INCLUDES   = -I. -I../../h -I../../palcode/$(TARGETDIR)

# --------------------------------------------
# Set global cc flags here for other makefiles
# --------------------------------------------

lx164_OPTION_FLAGS_GLOBAL       = -DEB164 -DLX164
sx164_OPTION_FLAGS_GLOBAL       = -DEB164 -DSX164
dp264_OPTION_FLAGS_GLOBAL       = -DDP264
OPTION_FLAGS_GLOBAL             = -DDC$(CPUTYPE) $($(TARGET)_OPTION_FLAGS_GLOBAL$(SYSREVID))

# TODO EB_BUILD_SYSTEM LINUX and use ARCH_XXXX
lx164_CC_FLAGS_GLOBAL						= -mcpu=21164a
sx164_CC_FLAGS_GLOBAL           = $(ARCH_EV56)
dp264_CC_FLAGS_GLOBAL           = -DPCIDEBUG $(ARCH_EV56)
default_CC_FLAGS_GLOBAL         = 


ifneq "$($(TARGET)_CC_FLAGS_GLOBAL$(SYSREVID))" ""
CCFLAGS_GLOBAL                  = $(OPTION_FLAGS_GLOBAL) \
	$($(TARGET)_CC_FLAGS_GLOBAL$(SYSREVID)) \
	$(EB_USER_CC_FLAGS)
else
CCFLAGS_GLOBAL                  = $(OPTION_FLAGS_GLOBAL) \
	$(default_CC_FLAGS_GLOBAL) \
	$(EB_USER_CC_FLAGS)
endif

ifeq "$(EB_BUILD_SYSTEM)" "WINNT"
CCFLAGS_GLOBAL += -nologo /Zi
endif

ifeq "$(EB_BUILD_SYSTEM)" "WINNT"
LINKFLAGS		= -MACHINE:ALPHA -ROM -FIXED -ALIGN:0x100 -DEBUG:FULL \
			  -DEBUGTYPE:COFF -ENTRY:__start -nodefaultlib:oldnames
LINKLIBS		=
LDFLAGS			= UseLINKFLAGSinstead
LDLIBS			= UseLINKLIBSinstead
else
LDFLAGS                 = -taso -non_shared -tr -h$(OBJDIR) -B
LDLIBS                  = -lm
endif

GASFLAGS_GLOBAL         = -v -$(CPUTYPE) $(EB_USER_GAS_FLAGS)
ASFLAGS                 += $(OPTION_FLAGS_GLOBAL) \
			$($(TARGET)_CC_FLAGS_GLOBAL$(SYSREVID)) \
			$(EB_USER_CC_FLAGS)

default_ALLTARGETS      = sx164 lx164 dp264

ifneq "$(EB_ALLTARGETS)" ""
ALLTARGETS              = $(EB_ALLTARGETS)
else
ALLTARGETS              = $(default_ALLTARGETS)
endif

default_DEFALLTARGETS   = -DSX164 -DLX164 -DDP264

ifneq "$(EB_DEFALLTARGETS)" ""
DEFALLTARGETS           = $(EB_DEFALLTARGETS)
else
DEFALLTARGETS           = $(default_DEFALLTARGETS)
endif

DEPENDFLAGS     = $(CCFLAGS) -DMAKEDEPEND $(DEFALLTARGETS) -DDC21164 -DDC21264 $(INCLUDES)

# ------------------------------------------------
# Inference rules.
# ------------------------------------------------

.nh.sr:
	$(SREC) -i -o$(BOOTADR) $< > $*.sr

ifeq "$(EB_BUILD_SYSTEM)" "WINNT"
.obj.nh:
	$(STRIP) $(STRIP_FLAGS) -o $*.nh $*.obj

.out.nh:
	$(STRIP) $(STRIP_FLAGS) -o $*.nh $*.out
else
.o.nh:
	$(STRIP) $(STRIP_FLAGS) $*.o $*.nh

.out.nh:
	$(STRIP) $(STRIP_FLAGS) $*.out $*.nh
endif

.c.s:
	$(CC) $(CCFLAGS) -S $*.c

$(OBJDIR)%.$(OBJ): %.c
	$(CC) $(CCFLAGS) -c -o $@ $(INCLDUES) $<

.s.$(OBJ):
	$(AS) $(ASFLAGS) $(INCLUDES) -o $(OBJDIR)$*.$(OBJ) $(SOURCEDIR)$*.s

.as.s:
	$(COPY) $*.as $*.s

.as.$(OBJ):
	$(COPY) $*.as $*.s
	$(AS) $(ASFLAGS) -o $(OBJDIR)$*.$(OBJ) $(SOURCEDIR)$*.s

.c.$(OBJ):
	$(CC) $(CCFLAGS) -o $(OBJDIR)$*.$(OBJ) $(INCLUDES) -c $(SOURCEDIR)$*.c

.out.nm:
	$(NM) -Bn $*.out > $*.nm
#       $(NM) $*.out | sort -n -t'|' +1 > $*.nm2

.$(OBJ).nm:
	$(NM) -Bn $*.$(OBJ) > $*.nm
#       $(NM) $*.$(OBJ) | sort -n -t'|' +1 > $*.nm2

.$(OBJ).dis:
	$(DIS) -S -I$(PWD) $*.$(OBJ) > $*.dis

.out.dis:
	$(DIS) -S -I$(PWD) $*.out > $*.dis

.$(OBJ).lis:
	$(LIST) $*.$(OBJ) > $*.lis

.out.lis:
	$(LIST) $*.out > $*.lis






