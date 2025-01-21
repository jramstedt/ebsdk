# $Id: Makefile,v 1.4 1999/03/30 18:08:28 paradis Exp $
# $Log: Makefile,v $
# Revision 1.4  1999/03/30 18:08:28  paradis
# Remove IDENTIFY entirely
# Remove memmove/memcpy from all Digital UNIX builds
# Add bcopy support
#
# Revision 1.3  1999/01/28 22:56:14  gries
# Removed -r for fail safe booter
#
# Revision 1.2  1999/01/22 15:42:46  bissen
# Use the values in $(TARGET).h to determine where the default
# location for the rom image should be.
#
# Revision 1.1.1.1  1998/12/29 21:36:15  paradis
# Initial CVS checkin
#
# Revision 1.125  1998/08/21  20:11:08  jerzy
# updated for clubs
#
# Revision 1.124  1998/08/19  17:27:26  jerzy
# Removed fsblib.a (can not be found) from
# command to build fsb.out
#
# Revision 1.123  1998/08/03  17:13:57  gries
# put -r into fsb.rom & dbm.rom
#
# Revision 1.122  1998/08/03  17:11:41  gries
# move default_SOFTBOOTADR from 200000 to 900000
#
# Revision 1.121  1998/04/06  18:24:31  gries
# Get ride of pass1 ieee
#
# Revision 1.120  1997/12/17  15:10:21  pbell
# Cahnged the palcode name and location.
#
# Revision 1.119  1997/12/15  20:52:51  pbell
# Updated for dp264.
#
# Revision 1.118  1997/06/02  17:42:27  fdh
# Updated include file dependencies.
#
# Revision 1.117  1997/05/31  03:26:30  fdh
# Modified the makerom parameter which determines
# the ROM OFFSET field in the FSB Makerom header.
#
# Revision 1.116  1997/05/27  16:07:22  fdh
# Modified the linker rules.
#
# Revision 1.115  1997/05/23  13:16:57  fdh
# Removed the fsboot module.
#
# Revision 1.114  1997/05/19  18:42:14  fdh
# Added fsboot.c to the Debug Monitor for the benefit
# of the fwupdate command.
#
# Revision 1.113  1997/05/19  15:26:00  fdh
# Updated include file dependencies.
#
# Revision 1.112  1997/05/16  02:11:57  fdh
# Modified use of the environment variable used with
# the Debug Monitor sysgen command line.
#
# Revision 1.111  1997/04/29  21:47:28  pbell
# Removed old file ffsrec.c and added xmodem.c
#
# Revision 1.110  1997/04/23  18:17:12  fdh
# Updated include file dependencies.
#
# Revision 1.109  1997/04/23  18:13:24  fdh
# Updated include file dependencies.
#
# Revision 1.108  1997/04/23  17:49:42  pbell
# Added conditions for mouse.c
#
# Revision 1.107  1997/04/11  03:33:37  fdh
# Added rules to build a debug version of the fail-safe booter
# based on the EB_DEBUG_FSBOOT macro.
#
# Revision 1.106  1997/04/10  23:25:11  fdh
# Updated include file dependencies.
#
# Revision 1.105  1997/03/17  19:12:17  fdh
# Updated include file dependencies.
#
# Revision 1.104  1997/03/17  10:28:06  fdh
# Modified to get the object file list from the source file list.
#
# Revision 1.103  1997/03/12  15:56:09  fdh
# Added another instance of lib.a on ld command line to resolve
# references from the stub library stub.a.
#
# Revision 1.102  1997/02/25  21:42:01  fdh
# Modified the $(BOOTDIR): target.
#
# Revision 1.101  1997/02/25  16:52:18  fdh
# Modified to build sft.out linked to 0x200000.
#
# Revision 1.100  1997/02/25  02:42:22  fdh
# Added provisions for optional arguments to be passed
# to sysgen when building the Debug Monitor and Fail-Safe
# Booter through the EB_DBM_SYSGEN and EB_FSB_SYSGEN
# environment variables.
#
# Revision 1.99  1997/02/24  17:30:23  gries
# Modified to use the EB_PALCODE environment variable
# to point to a different PALcode.
#
# Revision 1.98  1997/02/21  19:46:03  fdh
# Added LX164 target.
# Modified compile and link rules.
#
# Revision 1.97  1996/08/17 14:01:00  fdh
# Added call to system date before compiling version (timestamp) modules.
#
# Revision 1.96  1996/08/16  17:35:58  fdh
# Updated include file dependencies.
#
# Revision 1.95  1996/07/11  16:08:19  fdh
# Updated FSBOOTDEBUGFILES macro and added time.o to
# FSB object file list.
#
# Revision 1.94  1996/06/13  03:40:33  fdh
# Updated include file dependencies.
#
# Revision 1.93  1996/06/05  20:13:46  fdh
# Create hex files as well as s-record files.
#
# Revision 1.92  1996/05/26  20:46:12  fdh
# Removed some obsoleted macros.
#
# Revision 1.91  1996/05/26  20:09:25  fdh
# Modified to use the global definitions a little differently.
#
# Revision 1.90  1996/05/22  23:19:15  fdh
# Added machine check files to SRC list.
#
# Revision 1.89  1996/05/22  22:20:57  fdh
# Removed obsoleted modules and added some new ones.
# Updated include file dependencies.
#
# Revision 1.88  1996/02/26  19:36:01  fdh
# Added fsboot.o to the Debug Monitor object file list and
# added fsbmain.o to the Fail-Safe Booter object file list.
#
# Revision 1.87  1996/02/21  21:46:22  fdh
# Added beep.o to Fail-Safe Booter object file list.
#
# Revision 1.86  1995/12/23  23:46:19  fdh
# Updated include file dependencies.
#
# Revision 1.85  1995/12/17  22:52:26  fdh
# Renamed FWUPDATE_ENTRY_ALT to FSBOOT_ENTRY_ALT.
#
# Revision 1.84  1995/12/15  19:19:43  cruz
# Removed -DFWUPDATE_FILE_ALT
#
# Revision 1.83  1995/12/13  00:04:47  cruz
# Enabled biosem for debug version of fsboot.
#
# Revision 1.82  1995/12/07  07:53:52  fdh
# Modified to accomodate changes to the hfcomp tool.
#
# Revision 1.81  1995/12/02  12:53:13  fdh
# Extended FSBOOTOPTIONS definition.
#
# Revision 1.80  1995/12/01  18:52:21  fdh
# Copy the correct xxxdbm.sr file into the boot directory.
#
# Revision 1.79  1995/11/28  15:16:08  fdh
# Added decompression image to version.o dependencies.
#
# Revision 1.78  1995/11/18  15:11:15  fdh
# Changed protection on BOOTDIR files...
#
# Revision 1.77  1995/11/18  14:30:36  fdh
# Copy .sr files to BOOTDIR.
#
# Revision 1.76  1995/11/16  19:08:44  cruz
# Renamed resulting images to include the board name as part of
# the images' names.
#
# Revision 1.75  1995/11/13  20:50:51  cruz
# Added switch to MAKEROM rule for specifying ROM offset of 0 for dbm.rom and fsboot.rom
#
# Revision 1.74  1995/11/13  19:36:10  fdh
# Added the $(DECOMP) dependency to the .cmp files.
#
# Revision 1.73  1995/11/03  22:13:44  cruz
# Added lint library to list of items to remove when
# performing a clean.
#
# Revision 1.72  1995/11/03  18:34:57  cruz
# Removed extra argument from the lint rule.
#
# Revision 1.71  1995/11/03  18:31:21  cruz
# Changed the lint rule to look for the lint sub-libraries
# in the OBJDIR instead of the current directory.
#
# Revision 1.70  1995/10/31  18:58:04  cruz
# Fixed up lint rule.
#
# Revision 1.69  1995/10/30  17:05:55  cruz
# Added lint rule.
#
# Revision 1.68  1995/10/23  22:10:23  fdh
# Updated include file dependencies.
#
# Revision 1.67  1995/10/23  21:11:36  fdh
# Updated include file dependencies.
#
# Revision 1.66  1995/10/22  04:45:27  fdh
# Renamed gpchar.c to io_port.c.
#
# Revision 1.65  1995/10/21  06:21:09  fdh
# Updated include file dependencies.
#
# Revision 1.64  1995/10/20  20:11:56  fdh
# Modified to build in the release version or the debug version
# of the BIOS Emulator Library based on the EB_DEBUG_BIOSEM
# environment variable.
#
# Revision 1.63  1995/10/18  12:59:09  fdh
# Added lint: target.
#
# Revision 1.62  1995/10/14  09:42:53  fdh
# Removed target.o from list of library files...
#
# Revision 1.61  1995/10/13  20:06:06  fdh
# Renamed rom. files to dbm.
#
# Revision 1.60  1995/10/13  18:03:31  fdh
# Updated clean: target.
#
# Revision 1.59  1995/10/10  15:23:44  fdh
# Renamed eb64l to pc64.
#
# Revision 1.58  1995/10/10  14:21:46  fdh
# Renamed ladbx.h to ladebug.h.
#
# Revision 1.57  1995/10/05  02:13:24  fdh
# Updated include file dependencies
#
# Revision 1.56  1995/10/05  01:55:53  fdh
# Updated include file dependencies.
#
# Revision 1.55  1995/10/04  23:33:07  fdh
# Added build for Fail-safe loader (fsboot).
# Added file.c. Added environment variables to
# control compile conditionals.
#
# Revision 1.54  1995/10/03  03:49:42  fdh
# Modified rules for BOOTDIR targets.
# Produce map file when linker is run.
#
# Revision 1.53  1995/09/12  22:00:32  fdh
# Updated include file dependencies.
#
# Revision 1.52  1995/09/05  19:22:56  fdh
# Obsoleted fftty.c
#
# Revision 1.51  1995/09/05  18:23:17  fdh
# Added main.c and console.c
#
# Revision 1.50  1995/09/02  03:27:44  fdh
# Updated include file dependencies.
#
# Revision 1.49  1995/08/31  21:39:59  fdh
# Updated include file dependencies
#
# Revision 1.48  1995/08/25  21:18:23  fdh
# Updated include file dependencies...
#
# Revision 1.47  1995/08/25  20:13:23  fdh
# Added uart.c, io_port.c malloc.c, and rtc.c.  They were
# removed from ebfw/lib to restrict access to them through
# callbacks when user programs are linked with ebfw/lib.
# Link in bios emulation library.
#
# Revision 1.46  1995/03/07  08:40:21  fdh
# Turned off SROM compression (makerom -c).
#
# Revision 1.45  1995/03/05  00:47:22  fdh
# Turn on SROM compression (makerom -c).
#
# Revision 1.44  1995/02/28  15:35:36  fdh
# Minor re-arrangments.
#
# Revision 1.43  1995/02/28  03:26:28  fdh
# Changed offset to compressed image.
#
# Revision 1.42  1995/02/28  03:02:58  fdh
# Updated include file dependencies.
#
# Revision 1.41  1995/02/27  19:26:09  fdh
# Added getver.o to clean target.
#
# Revision 1.40  1995/02/27  19:20:00  fdh
# Fixed up getver build to build completely in object directories.
#
# Revision 1.39  1995/02/27  18:23:53  fdh
# Corrected typo...
#
# Revision 1.38  1995/02/27  18:19:36  cruz
# Added build constructs for getver.c
#
# Revision 1.37  1995/02/24  15:53:41  fdh
# Updated include file dependencies.
#
# Revision 1.36  1995/02/14  18:08:51  fdh
# Updated include file dependencies...
#
# Revision 1.35  1995/02/10  15:51:15  fdh
# Modified all: target a little bit.
#
# Revision 1.34  1995/02/10  03:01:43  fdh
# Added rom.cmp and rom.cpr to clean target.
#
# Revision 1.33  1995/02/08  01:36:56  fdh
# Updated include file dependencies...
#
# Revision 1.32  1995/02/02  21:10:33  fdh
# Added -i switch to makerom command line.
#
# Revision 1.31  1995/02/01  22:07:47  fdh
# Modified to use compress tool.
#
# Revision 1.30  1994/12/08  17:52:36  fdh
# Updated include file dependency list.
#
# Revision 1.29  1994/12/02  14:44:10  fdh
# Modified rules for the .dis targets... again.
#
# Revision 1.28  1994/12/01  18:57:07  fdh
# Corrected rules for soft.dis and rom.dis targets.
#
# Revision 1.27  1994/11/29  20:35:33  cruz
# Added implied targets for rom.dis and soft.dis
#
# Revision 1.26  1994/11/28  19:43:10  fdh
# Updated include file dependency list.
#
# Revision 1.25  1994/11/24  05:24:33  fdh
# Don't create .dis files by default.
#
# Revision 1.24  1994/11/24  04:31:17  fdh
# Updated include file dependency lists.
#
# Revision 1.23  1994/11/24  04:19:40  fdh
# Updated include file dependency list.
#
# Revision 1.22  1994/11/23  20:16:32  fdh
# Updated include file dependencies.
#
# Revision 1.21  1994/11/23  15:43:24  fdh
# Modified include path definitions...
#
# Revision 1.20  1994/11/22  23:15:19  fdh
# Modified to use the INCDIR definition.
#
# Revision 1.19  1994/11/09  21:27:40  fdh
# Put the rom.rom file into the bootp directory for
# the benefit of systems that can program it into flash.
#
# Revision 1.18  1994/11/04  16:38:21  fdh
# Updated include file dependency list.
#
# Revision 1.17  1994/07/01  15:13:41  fdh
# Updated include dependencies...
#
# Revision 1.16  1994/06/28  20:12:32  fdh
# Modified filenames and build precedure to fit into a FAT filesystem.
#
# Revision 1.15  1994/06/19  15:44:52  fdh
# Update include dependencies.
#
# Revision 1.14  1994/06/17  19:36:54  fdh
# Clean-up...
#
# Revision 1.13  1994/06/10  14:45:09  fdh
# Updated include file dependencies.
#
# Revision 1.12  1994/04/06  04:35:53  fdh
# Modified include file dependencies for dbmentry.s
#
# Revision 1.11  1994/04/01  13:56:04  fdh
# Added system.h to dbmentry.o dependency.
#
# Revision 1.10  1994/03/16  00:11:21  fdh
# Updated cmd.o dependencies...
#
# Revision 1.9  1994/03/15  16:27:42  rusling
# Added extra file dependencies for cmd.c
#
# Revision 1.8  1994/03/15  16:22:33  fdh
# Changed to use compressed rom image to make S-records file.
#
# Revision 1.7  1994/03/13  16:47:56  fdh
# Removed references to EB_PALDIR_EBxx environment variables.
# PALcode is now also built within ths biuld tree.
#
# Revision 1.6  1994/03/11  15:37:54  fdh
# Put back cp of ...rom.img to BOOTDIR
#
# Revision 1.5  1994/03/11  07:20:44  fdh
# Oops...
#
# Revision 1.4  1994/03/11  07:01:05  fdh
# Use global variables PALBASE and DBMENTRY.
#
# Revision 1.3  1994/03/11  05:30:45  fdh
# Cleanup...  Removed HWRPB.
#
# Revision 1.1  1994/03/08  17:44:49  fdh
# Initial revision
#

CONFIG_DIR=../..
include $(CONFIG_DIR)/sdkmake.conf
include ../rules.gnu

ifdef EB_DISABLE_GRAPHICS
CCFLAGS = -DDISABLE_GRAPHICS $(CCFLAGS_GLOBAL)
else
CCFLAGS = $(CCFLAGS_GLOBAL)
endif

default_BUILD_TARGETS = \
	$(BOOTDIR)$(TARGET)dbm.img \
	$(BOOTDIR)$(TARGET)dbm.cmp \
	$(BOOTDIR)$(TARGET)dbm.rom \
	$(BOOTDIR)$(TARGET)fsb.cmp \
	$(BOOTDIR)$(TARGET)fsb.rom \
	$(BOOTDIR)$(TARGET)fsb.img \
	$(BOOTDIR)$(TARGET)sft.img \
	$(BOOTDIR)$(TARGET)dbm.sr  \
	$(BOOTDIR)$(TARGET)dbm.hex \
	$(BOOTDIR)$(TARGET)fsb.sr  \
	$(BOOTDIR)$(TARGET)fsb.hex \
	$(OBJDIR)sft.sr \
	$(OBJDIR)sft.hex

ifneq "$($(TARGET)_BUILD_TARGETS)" ""
BUILD_TARGETS = $($(TARGET)_BUILD_TARGETS)
else
BUILD_TARGETS = $(default_BUILD_TARGETS)
endif

default: $(BUILD_TARGETS)

ifdef EB_DEBUG_BIOSEM
BIOSEMLIB = $(COMMONLIB)$(DBGDIR)/biosem.$(LIBEXT)
else
BIOSEMLIB = $(COMMONLIB)$(RELDIR)/biosem.$(LIBEXT)
endif


LIBS = \
	$(OBJDIR)ether.$(LIBEXT) \
	$(OBJDIR)ladebug.$(LIBEXT) \
	$(BIOSEMLIB) \
	$(OBJDIR)lib.$(LIBEXT) \
	$(OBJDIR)stubs.$(LIBEXT) \
	$(OBJDIR)lib.$(LIBEXT)

TARGET_SRCS = \
	$(SRCDIR)$(TARGET)mck.c

ALL_TARGET_SRCS = $(ALLTARGETS/*/&mck.c)

ifeq "$(EB_BUILD_SYSTEM)" "WINNT"
AS_SRCS  = \
	$(SRCDIR)ntmisc.s \
	$(SRCDIR)dbmentry.s
else
AS_SRCS  = \
	$(SRCDIR)dbmentry.s
endif

C_SRCS   = \
	$(SRCDIR)main.c \
	$(SRCDIR)dis.c \
	$(SRCDIR)ffexec.c \
	$(SRCDIR)cmd.c \
	$(SRCDIR)console.c \
	$(SRCDIR)uart.c \
	$(SRCDIR)sromport.c \
	$(SRCDIR)io_port.c \
	$(SRCDIR)file.c \
	$(SRCDIR)malloc.c \
	$(SRCDIR)rtc.c \
	$(SRCDIR)mouse.c \
	$(SRCDIR)xmodem.c

FSB_SRCS = \
	$(SRCDIR)fsbmain.c \
	$(SRCDIR)file.c \
	$(SRCDIR)malloc.c \
	$(SRCDIR)rtc.c

FSB_DEBUG_SRCS = \
	$(SRCDIR)console.c \
	$(SRCDIR)io_port.c \
	$(SRCDIR)uart.c \
	$(SRCDIR)sromport.c

ifeq "$(EB_BUILD_SYSTEM)" "WINNT"
EXTRA_OBJFILES = \
	$(OBJDIR)ladbxusr.$(OBJ) \
	$(OBJDIR)dpml.$(OBJ) \
	$(OBJDIR)ntmisc.$(OBJ)
else
EXTRA_OBJFILES = \
	$(OBJDIR)ladbxusr.$(OBJ) \
	$(OBJDIR)dpml.$(OBJ)
endif

OBJFILES = $(C_SRCS:%.c=$(OBJDIR)%.$(OBJ)) $(TARGET_SRCS:%.c=$(OBJDIR)%.$(OBJ)) \
		$(EXTRA_OBJFILES)

FSBOBJFILES = $(FSB_SRCS:%.c=$(OBJDIR)%.$(OBJ))

ifdef EB_DEBUG_FSBOOT
FSBDEBUGOBJS = $(FSB_DEBUG_SRCS:%.c=$(OBJDIR)%.$(OBJ))
else
FSDEBUGOBJS =
endif

FSBOBJFILES +=  $(FSDEBUGOBJS)

default_PALCODE         = $(OBJDIR)osfpal
dp264_PALCODE           = ../palcode/dp264/osfpal_dbm.exe
clubs_PALCODE           = ../palcode/clubs/osfpal_dbm.exe

ifneq "$($(TARGET)_PALCODE)" ""
PALCODE                 = $($(TARGET)_PALCODE)
else
PALCODE                 = $(default_PALCODE)
endif

default_SOFTBOOTADR     = 900000
ifneq "$($(TARGET)_SOFTBOOTADR)" ""
SOFTBOOTADR             = $($(TARGET)_SOFTBOOTADR)
else
SOFTBOOTADR             = $(default_SOFTBOOTADR)
endif

vars:
	@echo OBJFILES is $(OBJFILES)

#-------------------------------------------------------------
$(OBJDIR)dbmver.$(OBJ):      version.c $(z) CRT0 $(OBJFILES) $(LIBS)
	$(CC) -o $@ -c  version.c

$(OBJDIR)fsbver.$(OBJ):      version.c $(PALCODE) CRT0 $(FSBOBJFILES) $(LIBS)
	$(CC) -o $@ -c  version.c

$(OBJDIR)dbmentry.$(OBJ): dbmentry.s
	$(AS) $(ASFLAGS) -o $@ -c $(INCLUDES) $<

CRT0: $(OBJDIR)dbmentry.$(OBJ)
	$(COPY) $(OBJDIR)dbmentry.$(OBJ) $(COPY_TO) $(OBJDIR)crt0.$(OBJ)

ifeq "$(EB_BUILD_SYSTEM)" "WINNT"
$(OBJDIR)ntmisc.$(OBJ): ntmisc.s
	$(AS) $(ASFLAGS) -o $@ -c $(INCLUDES) $<
endif


$(OBJDIR)dis.$(OBJ):         $(SOURCEDIR)dis.c
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

$(OBJDIR)ffexec.$(OBJ):      $(SOURCEDIR)ffexec.c
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

$(OBJDIR)file.$(OBJ):        $(SOURCEDIR)file.c
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

$(OBJDIR)cmd.$(OBJ):         $(SOURCEDIR)cmd.c
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

$(OBJDIR)console.$(OBJ):     $(SOURCEDIR)console.c
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

$(OBJDIR)main.$(OBJ):        $(SOURCEDIR)main.c
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

$(OBJDIR)uart.$(OBJ):        $(SOURCEDIR)uart.c
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

$(OBJDIR)sromport.$(OBJ):    $(SOURCEDIR)sromport.c
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

$(OBJDIR)io_port.$(OBJ):     $(SOURCEDIR)io_port.c
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

$(OBJDIR)malloc.$(OBJ):      $(SOURCEDIR)malloc.c
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

$(OBJDIR)$(TARGET)mck.$(OBJ):        $(SOURCEDIR)$(TARGET)mck.c
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

$(OBJDIR)rtc.$(OBJ):         $(SOURCEDIR)rtc.c
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

$(OBJDIR)mouse.$(OBJ):       $(SOURCEDIR)mouse.c
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

$(OBJDIR)xmodem.$(OBJ):      $(SOURCEDIR)xmodem.c
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

ifdef EB_DEBUG_FSBOOT
_FSBCFLAGS = -DEB_DEBUG_FSBOOT
else
_FSBCFLAGS =
endif

$(OBJDIR)fsbmain.$(OBJ):     $(SOURCEDIR)fsbmain.c
	$(CC) $(_FSBCFLAGS) $(CCFLAGS) \
	-o $@ -c $(INCLUDES) $<

#-------------------------------------------------------------

$(BOOTDIR):
	mkdir $(BOOTDIR); $(CHMOD) a+r+x $(BOOTDIR)

#-------------------------------------------------------------
ifeq "$(EB_BUILD_SYSTEM)" "WINNT"
$(OBJDIR)dbm.out: CRT0 $(OBJFILES) $(LIBS) $(OBJDIR)dbmver.$(OBJ)
	$(LINK) $(LINKFLAGS) \
	-OUT:$(OBJDIR)dbm.out -BASE:0x$(DBMENTRY) \
	$(OBJDIR)crt0.$(OBJ) $(OBJFILES) $(LIBS) $(OBJDIR)dbmver.$(OBJ) $(LINKLIBS) \
	-MAP:$(OBJDIR)dbm.map

$(OBJDIR)dbm.nh: $(OBJDIR)dbm.out
	$(STRIP) $(STRIP_FLAGS) -o $(OBJDIR)dbm.nh $(OBJDIR)dbm.out
else
$(OBJDIR)dbm.out: CRT0 $(OBJFILES) $(LIBS) $(OBJDIR)dbmver.$(OBJ)
	$(LD) -m -o $(OBJDIR)dbm.out -N -T $(DBMENTRY) $(LDFLAGS) \
	$(OBJFILES) $(LIBS) $(OBJDIR)dbmver.$(OBJ) $(LDLIBS) > $(OBJDIR)dbm.map
endif

$(OBJDIR)dbm.dis: $(OBJDIR)dbm.out
	$(DIS) -S -I$(PWD) $(OBJDIR)dbm.out > $(OBJDIR)dbm.dis

dbm.dis:        $(OBJDIR)dbm.dis
	$(COPY) $(OBJDIR)dbm.dis $(COPY_TO) dbm.dis

$(OBJDIR)dbm.nm: $(OBJDIR)dbm.out
	$(NM) -Bn $(OBJDIR)dbm.out > $@

$(OBJDIR)dbm.rom: $(OBJDIR)dbm.cmp $(OBJDIR)getver
	$(OBJDIR)getver > $(OBJDIR)timestmp.fw
	$(MAKEROM) -v -iDBM -f$(OBJDIR)timestmp.fw -l$(DECOMP_PALBASE) \
		$(OBJDIR)dbm.cmp -o  $(OBJDIR)dbm.rom       

$(BOOTDIR)$(TARGET)dbm.rom: $(OBJDIR)dbm.rom $(BOOTDIR)
	$(COPY) $(OBJDIR)dbm.rom $(COPY_TO) $(BOOTDIR)$(TARGET)dbm.rom
	$(CHMOD) 664 $(BOOTDIR)$(TARGET)dbm.rom

$(OBJDIR)dbm.sr: $(OBJDIR)dbm.rom
	$(SREC) -i $(OBJDIR)dbm.rom > $@

$(OBJDIR)dbm.hex: $(OBJDIR)dbm.rom
	$(HEX) -v $(OBJDIR)dbm.rom $@

$(BOOTDIR)$(TARGET)dbm.sr: $(OBJDIR)dbm.sr
	$(COPY) $(OBJDIR)dbm.sr $(COPY_TO) $(BOOTDIR)$(TARGET)dbm.sr
	$(CHMOD) 664 $(BOOTDIR)$(TARGET)dbm.sr

$(BOOTDIR)$(TARGET)dbm.hex: $(OBJDIR)dbm.hex
	$(COPY) $(OBJDIR)dbm.hex $(COPY_TO) $(BOOTDIR)$(TARGET)dbm.hex
	$(CHMOD) 664 $(BOOTDIR)$(TARGET)dbm.hex

default_DBM_SYSGEN      = \
	-e$(PALBASE) $(PALCODE) \
	-e$(DBMENTRY) -s $(OBJDIR)dbm.nh
dp264_DBM_SYSGEN        = \
	-e$(PALBASE)  -s $(PALCODE) \
	-e$(DBMENTRY) -s $(OBJDIR)dbm.nh 
clubs_DBM_SYSGEN        = \
	-e$(PALBASE)  -s $(PALCODE) \
	-e$(DBMENTRY) -s $(OBJDIR)dbm.nh

ifneq "$($(TARGET)_DBM_SYSGEN)" ""
DBM_SYSGEN = $($(TARGET)_DBM_SYSGEN)
else
DBM_SYSGEN = $(default_DBM_SYSGEN)
endif

$(OBJDIR)dbm.img: $(PALCODE) $(OBJDIR)dbm.nh
	$(SYSGEN) -v $(DBM_SYSGEN) -o $(OBJDIR)dbm.img

$(BOOTDIR)$(TARGET)dbm.img: $(OBJDIR)dbm.img $(BOOTDIR)
	$(COPY) $(OBJDIR)dbm.img $(COPY_TO) $(BOOTDIR)$(TARGET)dbm.img
	$(CHMOD) 664 $(BOOTDIR)$(TARGET)dbm.img

$(BOOTDIR)$(TARGET)dbm.dis: $(OBJDIR)dbm.dis $(BOOTDIR)
	$(COPY) $(OBJDIR)dbm.dis $(COPY_TO) $(BOOTDIR)$(TARGET)dbm.dis
	$(CHMOD) 664 $(BOOTDIR)$(TARGET)dbm.dis

$(BOOTDIR)$(TARGET)dbm.nm: $(OBJDIR)dbm.nm $(BOOTDIR)
	$(COPY) $(OBJDIR)dbm.nm $(COPY_TO) $(BOOTDIR)$(TARGET)dbm.nm
	$(CHMOD) 664 $(BOOTDIR)$(TARGET)dbm.nm

$(OBJDIR)dbm.cmp: $(OBJDIR)dbm.img
	$(COMPRESS) -v -$(CPUTYPE) -t0x$(PALBASE) $(OBJDIR)dbm.img $(OBJDIR)dbm.cmp

$(BOOTDIR)$(TARGET)dbm.cmp: $(OBJDIR)dbm.cmp $(BOOTDIR)
	$(COPY) $(OBJDIR)dbm.cmp $(COPY_TO) $(BOOTDIR)$(TARGET)dbm.cmp
	$(CHMOD) 664 $(BOOTDIR)$(TARGET)dbm.cmp

#-------------------------------------------------------------

$(BOOTDIR)$(TARGET)sft.img: $(OBJDIR)sft.nh $(BOOTDIR)
	$(COPY) $(OBJDIR)sft.nh $(COPY_TO) $(BOOTDIR)$(TARGET)sft.img
	$(CHMOD) 664 $(BOOTDIR)$(TARGET)sft.img

$(BOOTDIR)$(TARGET)sft.dis: $(OBJDIR)sft.dis $(BOOTDIR)
	$(COPY) $(OBJDIR)sft.dis $(COPY_TO) $(BOOTDIR)$(TARGET)sft.dis
	$(CHMOD) 664 $(BOOTDIR)$(TARGET)sft.dis


$(BOOTDIR)$(TARGET)sft.nm: $(OBJDIR)sft.nm $(BOOTDIR)
	$(COPY) $(OBJDIR)sft.nm $(COPY_TO) $(BOOTDIR)$(TARGET)sft.nm
	$(CHMOD) 664 $(BOOTDIR)$(TARGET)sft.nm

ifeq "$(EB_BUILD_SYSTEM)" "WINNT"

$(OBJDIR)sft.out: CRT0 $(OBJFILES) $(LIBS) $(OBJDIR)dbmver.$(OBJ)
	$(LINK) $(LINKFLAGS) \
	-OUT:$(OBJDIR)sft.out -BASE:0x$(SOFTBOOTADR) \
	$(OBJDIR)crt0.$(OBJ) $(OBJFILES) $(LIBS) $(OBJDIR)dbmver.$(OBJ) $(LINKLIBS) \
	-MAP:$(OBJDIR)sft.map

$(OBJDIR)sft.nh: $(OBJDIR)sft.out
	$(STRIP) $(STRIP_FLAGS) -o $(OBJDIR)sft.nh $(OBJDIR)sft.out

else
$(OBJDIR)sft.out: CRT0 $(OBJFILES) $(LIBS) $(OBJDIR)dbmver.$(OBJ)
	$(LD) -m -o $(OBJDIR)sft.out -N -T $(SOFTBOOTADR) $(LDFLAGS) \
	$(OBJFILES) $(LIBS) $(OBJDIR)dbmver.$(OBJ) $(LDLIBS) > $(OBJDIR)sft.map


$(OBJDIR)sft.nh: $(OBJDIR)sft.out
	$(STRIP) $(STRIP_FLAGS) $< $@
endif

$(OBJDIR)sft.dis: $(OBJDIR)sft.out
	$(DIS) -S -I$(PWD) $(OBJDIR)sft.out > $(OBJDIR)sft.dis

sft.dis:        $(OBJDIR)sft.dis
	$(COPY) $(OBJDIR)sft.dis $(COPY_TO) sft.dis

$(OBJDIR)sft.nm: $(OBJDIR)sft.out
	$(NM) -Bn $(OBJDIR)sft.out > $@

$(OBJDIR)sft.sr: $(OBJDIR)sft.nh
	$(SREC) -i $(OBJDIR)sft.nh > $@

$(OBJDIR)sft.hex: $(OBJDIR)sft.nh
	$(HEX) -v $(OBJDIR)sft.nh $@

#-------------------------------------------------------------
ifeq "$(EB_BUILD_SYSTEM)" "WINNT"

$(OBJDIR)fsb.out: CRT0 $(FSBOBJFILES) $(OBJDIR)fsbver.$(OBJ)
	$(LINK) $(LINKFLAGS) \
	-OUT:$(OBJDIR)fsb.out -BASE:0x$(DBMENTRY) \
	$(OBJDIR)crt0.$(OBJ) $(FSBOBJFILES) $(OBJDIR)fsbver.$(OBJ) $(OBJDIR)lib.$(LIBEXT) \
	$(OBJDIR)stubs.$(LIBEXT) \
	$(OBJDIR)lib.$(LIBEXT) $(LINKLIBS) \
	-MAP:$(OBJDIR)fsb.map

$(OBJDIR)fsb.nh: $(OBJDIR)fsb.out
	$(STRIP) $(STRIP_FLAGS) -o $(OBJDIR)fsb.nh $(OBJDIR)fsb.out
else

$(OBJDIR)fsb.out: CRT0 $(FSBOBJFILES) $(OBJDIR)fsbver.$(OBJ)
	$(LD) -m -o $(OBJDIR)fsb.out -N -T $(DBMENTRY) $(LDFLAGS) \
	$(FSBOBJFILES) $(OBJDIR)fsbver.$(OBJ) $(OBJDIR)lib.$(LIBEXT) \
	$(OBJDIR)stubs.$(LIBEXT) \
	$(OBJDIR)lib.$(LIBEXT) $(LDLIBS) > $(OBJDIR)fsb.map

endif

$(OBJDIR)fsb.dis:
	$(DIS) -S -I$(PWD) $(OBJDIR)fsb.out > $@

fsb.dis:        $(OBJDIR)fsb.dis
	$(COPY) $(OBJDIR)fsb.dis $(COPY_TO) fsb.dis

$(OBJDIR)fsb.nm: $(OBJDIR)fsb.out
	$(NM) -Bn $(OBJDIR)fsb.out > $@

$(OBJDIR)fsb.rom: $(OBJDIR)fsb.cmp $(OBJDIR)getver
	$(OBJDIR)getver > $(OBJDIR)timestmp.fw
	$(MAKEROM) -v -iFSB  -f$(OBJDIR)timestmp.fw \
	-l$(DECOMP_PALBASE) $(OBJDIR)fsb.cmp -o  $(OBJDIR)fsb.rom       

$(BOOTDIR)$(TARGET)fsb.rom: $(OBJDIR)fsb.rom $(BOOTDIR)
	$(COPY) $(OBJDIR)fsb.rom $(COPY_TO) $(BOOTDIR)$(TARGET)fsb.rom
	$(CHMOD) 664 $(BOOTDIR)$(TARGET)fsb.rom

$(OBJDIR)fsb.sr: $(OBJDIR)fsb.rom
	$(SREC) -i $(OBJDIR)fsb.rom > $@

$(OBJDIR)fsb.hex: $(OBJDIR)fsb.rom
	$(HEX) -v $(OBJDIR)fsb.rom $@

$(BOOTDIR)$(TARGET)fsb.sr: $(OBJDIR)fsb.sr
	$(COPY) $(OBJDIR)fsb.sr $(COPY_TO) $(BOOTDIR)$(TARGET)fsb.sr
	$(CHMOD) 664 $(BOOTDIR)$(TARGET)fsb.sr

$(BOOTDIR)$(TARGET)fsb.hex: $(OBJDIR)fsb.hex
	$(COPY) $(OBJDIR)fsb.hex $(COPY_TO) $(BOOTDIR)$(TARGET)fsb.hex
	$(CHMOD) 664 $(BOOTDIR)$(TARGET)fsb.hex

default_FSB_SYSGEN      = \
	-e$(PALBASE) $(PALCODE) \
	-e$(FSBENTRY) -s $(OBJDIR)fsb.nh
dp264_FSB_SYSGEN        = \
	-e$(PALBASE)  -s $(PALCODE) \
	-e$(FSBENTRY) -s $(OBJDIR)fsb.nh 
clubs_FSB_SYSGEN        = \
	-e$(PALBASE)  -s $(PALCODE) \
	-e$(FSBENTRY) -s $(OBJDIR)fsb.nh

ifneq "$($(TARGET)_FSB_SYSGEN)" ""
FSB_SYSGEN = $($(TARGET)_FSB_SYSGEN)
else
FSB_SYSGEN = $(default_FSB_SYSGEN)
endif

$(OBJDIR)fsb.img: $(PALCODE) $(OBJDIR)fsb.nh
	$(SYSGEN) -v $(FSB_SYSGEN) -o $(OBJDIR)fsb.img

$(BOOTDIR)$(TARGET)fsb.img: $(OBJDIR)fsb.img $(BOOTDIR)
	$(COPY) $(OBJDIR)fsb.img $(COPY_TO) $(BOOTDIR)$(TARGET)fsb.img
	$(CHMOD) 664 $(BOOTDIR)$(TARGET)fsb.img

$(OBJDIR)fsb.cmp: $(OBJDIR)fsb.img
	$(COMPRESS) -v -$(CPUTYPE) -t0x$(PALBASE) $(OBJDIR)fsb.img $(OBJDIR)fsb.cmp

$(BOOTDIR)$(TARGET)fsb.cmp: $(OBJDIR)fsb.cmp $(BOOTDIR)
	$(COPY) $(OBJDIR)fsb.cmp $(COPY_TO) $(BOOTDIR)$(TARGET)fsb.cmp
	$(CHMOD) 664 $(BOOTDIR)$(TARGET)fsb.cmp


#-------------------------------------------------------------
#  Version utility...
#-------------------------------------------------------------

$(OBJDIR)getver:  getver.c $(OBJDIR)dbmver.$(OBJ)  $(OBJDIR)fsbver.$(OBJ) 
	$(CC) -c -o $(OBJDIR)getver.$(OBJ) $(INCLUDES) -O2 getver.c
	$(CC) -o $@ -O2 $(OBJDIR)getver.$(OBJ) $(OBJDIR)dbmver.$(OBJ) 

#-------------------------------------------------------------

depend: $(AS_SRCS) $(C_SRCS) $(ALL_TARGET_SRCS)
	$(MAKEDEPEND) -f $(MAKEFILE) -p'$$(OBJDIR)' -- $(DEPENDFLAGS) -- \
	$(AS_SRCS) $(C_SRCS) $(ALL_TARGET_SRCS) $(FSB_SRCS)

clean:
	-rm -f *~ \#*\# core $(MAKEFILE).bak \
	$(OBJDIR)dbmentry.$(OBJ) \
	$(OBJFILES) $(FSBOBJFILES) \
	$(OBJDIR)dbm.nh \
	$(OBJDIR)dbm.img \
	$(OBJDIR)dbm.rom \
	$(OBJDIR)dbm.cmp \
	$(OBJDIR)dbm.sr \
	$(OBJDIR)dbm.hex \
	$(OBJDIR)dbm.dis \
	$(OBJDIR)dbm.out \
	$(OBJDIR)dbm.map \
	$(OBJDIR)dbmver.$(OBJ) \
	$(OBJDIR)fsb.nh \
	$(OBJDIR)fsb.img \
	$(OBJDIR)fsb.rom \
	$(OBJDIR)fsb.cmp \
	$(OBJDIR)fsb.sr \
	$(OBJDIR)fsb.hex \
	$(OBJDIR)fsb.dis \
	$(OBJDIR)fsb.out \
	$(OBJDIR)fsb.map \
	$(OBJDIR)fsbver.$(OBJ) \
	$(OBJDIR)sft.nh \
	$(OBJDIR)sft.dis \
	$(OBJDIR)sft.out \
	$(OBJDIR)sft.map \
	$(OBJDIR)sft.sr \
	$(OBJDIR)sft.hex \
	$(OBJDIR)getver \
	$(OBJDIR)getver.$(OBJ) \
	$(OBJDIR)llib-ldbm.ln \
	$(OBJDIR)timestmp.fw
	$(CLEAN)

distclean:	clean
	$(MAKEDEPEND) -f $(MAKEFILE)
	$(DELETE) $(MAKEFILE).bak

# DO NOT DELETE THIS LINE -- make depend depends on it.


