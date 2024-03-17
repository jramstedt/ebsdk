# $Id: Makefile,v 1.4 1999/04/27 21:00:02 paradis Exp $
# $Log: Makefile,v $
# Revision 1.4  1999/04/27 21:00:02  paradis
# Added -c flag to regtest build to generate appropriate object file
# format
#
# Revision 1.3  1999/04/19 16:56:13  paradis
# Add ".set noreorder" to regtest.s
# Fix strip so that it can handle NT .obj files
#
# Revision 1.2  1999/04/09 17:39:26  paradis
# Added "regtest" to default build for system exerciser
# Fixed compiler incompatibility in loops1.c
# Made cstrip a bit more verbose
#
# Revision 1.1.1.1  1998/12/29 21:36:04  paradis
# Initial CVS checkin
#
# Revision 1.41  1997/12/15  20:50:23  pbell
# Updated for dp264
#
# Revision 1.40  1997/06/02  17:42:29  fdh
# Updated include file dependencies.
#
# Revision 1.39  1997/05/19  15:26:03  fdh
# Updated include file dependencies.
#
# Revision 1.38  1997/04/23  18:17:14  fdh
# Updated include file dependencie.s
#
# Revision 1.37  1997/04/23  18:13:26  fdh
# Updated include file dependencies.
#
# Revision 1.36  1997/03/17  10:18:10  fdh
# Added the object file for the dpml stub to
# the list of required LIBS.
#
# Revision 1.35  1997/03/12  16:01:16  fdh
# Added another instance of lib.a on ld command line to resolve
# references from the stub library stub.a.
#
# Revision 1.34  1997/02/21  19:56:18  fdh
# Modified compile and link rules.
#
# Revision 1.33  1996/05/26 20:08:13  fdh
# Modified to use more global definitions.
#
# Revision 1.32  1996/05/22  22:29:34  fdh
# Updated include file dependencies.
#
# Revision 1.31  1995/12/12  20:41:03  fdh
# Removed unnecessary directory delimiter "/".
#
# Revision 1.30  1995/12/07  02:57:13  cruz
# Changed assembler for regtest from GAS to AS.
#
# Revision 1.29  1995/12/05  21:21:13  cruz
# Added make rule for regtest.
#
# Revision 1.28  1995/11/18  15:18:59  fdh
# Changed proctection on BOOTDIR files...
#
# Revision 1.27  1995/10/31  19:00:32  cruz
# Added dummy lint rule.
#
# Revision 1.26  1995/10/13  20:45:09  fdh
# Renamed time.c to wait.c.  Changed a few other names.
#
# Revision 1.25  1995/10/10  15:23:47  fdh
# Renamed eb64l to pc64.
#
# Revision 1.24  1995/10/05  01:55:56  fdh
# Updated include file dependencies.
#
# Revision 1.23  1995/10/03  03:50:43  fdh
# Update include file dependencies.
#
# Revision 1.22  1995/09/12  22:00:34  fdh
# Updated include file dependencies.
#
# Revision 1.21  1995/08/31  21:40:02  fdh
# Updated include file dependencies
#
# Revision 1.20  1995/08/25  21:18:26  fdh
# Updated include file dependencies...
#
# Revision 1.19  1995/05/17  03:05:41  fdh
# Removed boot files from clean target.
#
# Revision 1.18  1995/01/23  21:53:19  fdh
# Updated include file dependency lists.
#
# Revision 1.17  1994/11/24  04:31:20  fdh
# Updated include file dependency lists.
#
# Revision 1.16  1994/11/23  19:54:42  fdh
# Updated include file dependencies.
#
# Revision 1.15  1994/11/23  15:43:27  fdh
# Modified include path definitions...
#
# Revision 1.14  1994/11/22  23:15:23  fdh
# Modified to use the INCDIR definition.
#
# Revision 1.13  1994/11/11  20:48:10  fdh
# Updated include file dependency list.
#
# Revision 1.12  1994/11/07  12:54:23  rusling
# Fixed up include file dependencies.
#
# Revision 1.11  1994/07/13  14:33:47  fdh
# Removed some unnecessary example programs.
#
# Revision 1.10  1994/07/01  15:13:43  fdh
# Updated include dependencies...
#
# Revision 1.9  1994/06/28  20:12:40  fdh
# Modified filenames and build precedure to fit into a FAT filesystem.
#
# Revision 1.8  1994/06/27  18:15:25  fdh
# Merged flashmain.c into flash.c.
#
# Revision 1.7  1994/06/22  15:17:45  rusling
# Added lib.h as include file.
#
# Revision 1.6  1994/06/17  19:37:06  fdh
# Clean-up...
#
# Revision 1.5  1994/06/10  14:45:13  fdh
# Updated include file dependencies.
#
# Revision 1.4  1994/06/10  14:37:17  fdh
# Cleaned up dependencies for the hello program.
# Removed xxx.sr rules because they are built into rules.osf.
#
# Revision 1.3  1994/06/03  02:57:39  fdh
# Correct components used to build hello target.
#
# Revision 1.2  1994/04/21  08:06:18  fdh
# A little cleanup.  Probably needs a bit more.
#
# Revision 1.1  1994/03/11  05:13:21  fdh
# Initial revision
#

CONFIG_DIR=../..
include $(CONFIG_DIR)/sdkmake.conf
include ../rules.gnu

CCFLAGS = $(CCFLAGS_GLOBAL)
LIBS =  $(OBJDIR)dpml.$(OBJ) \
	$(OBJDIR)lib.$(LIBEXT) \
	$(OBJDIR)stubs.$(LIBEXT) \
	$(OBJDIR)lib.$(LIBEXT)

default:        $(BOOTDIR)hello \
		$(BOOTDIR)size \
		$(BOOTDIR)regtest \
		$(BOOTDIR)yellow

all:            $(BOOTDIR)hello \
		$(BOOTDIR)size \
		$(BOOTDIR)yellow \
		$(BOOTDIR)loops \
		$(BOOTDIR)regtest \
		$(BOOTDIR)sum \
		$(BOOTDIR)mtest \
		$(BOOTDIR)pic \
		$(BOOTDIR)sound \
		$(BOOTDIR)nvram \
		$(BOOTDIR)vga \
		$(BOOTDIR)time \
		$(BOOTDIR)lp \
		$(BOOTDIR)flash \
		$(BOOTDIR)mfs \
		$(BOOTDIR)fd \
		$(BOOTDIR)comtrol \
		$(BOOTDIR)mouse_demo \
		$(BOOTDIR)avp

SRCS    =       avp.c           \
		comtrol.c       \
		fd.c            \
		flash.c         \
		halt.s          \
		hello.c         \
		loops1.c        \
		loops2.c        \
		lp.c            \
		mfs.c           \
		mouse_demo.c            \
		mtest.c         \
		nvram.c         \
		pic.c           \
		size.c          \
		sound.c         \
		sum.c           \
		wait.c          \
		vga.c           \
		vp.c            \
		yellow.c


# -------------------------------------------------
# Append local flags here for only this Makefile
# -------------------------------------------------
INCLUDES                =  -I. -I../h -I../palcode/$(TARGETDIR)


#-----------------

HELLO = $(OBJDIR)hello.$(OBJ)

$(OBJDIR)hello.$(OBJ):          hello.c 
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

hello $(BOOTDIR)hello: $(OBJDIR)hello.nh
	$(COPY) $(OBJDIR)hello.nh $(COPY_TO) $(BOOTDIR)hello
	$(CHMOD) 664 $(BOOTDIR)hello

ifeq "$(EB_BUILD_SYSTEM)" "WINNT"
$(OBJDIR)hello.out: CRT0 ${HELLO} ${LIBS}
	$(LINK) $(LINKFLAGS) -BASE:0x$(BOOTADR) -OUT:$@ $(OBJDIR)crt0.$(OBJ) ${HELLO} ${LIBS} $(LINKLIBS)
else
$(OBJDIR)hello.out: CRT0 ${HELLO} ${LIBS}
	$(LD) -N -T $(BOOTADR) $(LDFLAGS) -o $@ ${HELLO} ${LIBS} $(LDLIBS)
endif

#-----------------

SIZE = $(OBJDIR)size.$(OBJ)

$(OBJDIR)size.$(OBJ):           size.c 
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

size $(BOOTDIR)size: $(OBJDIR)size.nh
	$(COPY) $(OBJDIR)size.nh $(COPY_TO) $(BOOTDIR)size
	$(CHMOD) 664 $(BOOTDIR)size

ifeq "$(EB_BUILD_SYSTEM)" "WINNT"
$(OBJDIR)size.out: CRT0 ${SIZE} ${LIBS}
	$(LINK) -BASE:0x$(BOOTADR) $(LINKFLAGS) -OUT:$@ $(OBJDIR)crt0.$(OBJ) ${SIZE} ${LIBS} $(LINKLIBS)
else
$(OBJDIR)size.out: CRT0 ${SIZE} ${LIBS}
	$(LD) -N -T $(BOOTADR) $(LDFLAGS) -o $@ ${SIZE} ${LIBS} $(LDLIBS)
endif

#-----------------

YELLOW = $(OBJDIR)yellow.$(OBJ)

$(OBJDIR)yellow.$(OBJ):         yellow.c 
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

yellow $(BOOTDIR)yellow: $(OBJDIR)yellow.nh
	$(COPY) $(OBJDIR)yellow.nh $(COPY_TO) $(BOOTDIR)yellow
	$(CHMOD) 664 $(BOOTDIR)yellow

ifeq "$(EB_BUILD_SYSTEM)" "WINNT"
$(OBJDIR)yellow.out: CRT0 ${YELLOW} ${LIBS}
	$(LINK) -BASE:0x$(BOOTADR) $(LINKFLAGS) -OUT:$@ $(OBJDIR)crt0.$(OBJ) ${YELLOW} ${LIBS} $(LINKLIBS)
else
$(OBJDIR)yellow.out: CRT0 ${YELLOW} ${LIBS}
	$(LD) -N -T $(BOOTADR) $(LDFLAGS) -o $@ ${YELLOW} ${LIBS} $(LDLIBS)
endif

#-----------------

LOOPS = $(OBJDIR)loops1.$(OBJ) $(OBJDIR)loops2.$(OBJ)

$(OBJDIR)loops1.$(OBJ):         loops1.c 
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

$(OBJDIR)loops2.$(OBJ):         loops2.c 
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

loops $(BOOTDIR)loops: $(OBJDIR)loops.nh
	$(COPY) $(OBJDIR)loops.nh $(COPY_TO) $(BOOTDIR)loops
	$(CHMOD) 664 $(BOOTDIR)loops

ifeq "$(EB_BUILD_SYSTEM)" "WINNT"
$(OBJDIR)loops.out: CRT0 ${LOOPS} ${LIBS}
	$(LINK) -BASE:0x$(BOOTADR) $(LINKFLAGS) -OUT:$@ $(OBJDIR)crt0.$(OBJ) ${LOOPS} ${LIBS} $(LINKLIBS)
else
$(OBJDIR)loops.out: CRT0 ${LOOPS} ${LIBS}
	$(LD) -N -T $(BOOTADR) $(LDFLAGS) -o $@ ${LOOPS} ${LIBS} $(LDLIBS)
endif

#-----------------

MOUSE = $(OBJDIR)mouse_demo.$(OBJ)

$(OBJDIR)mouse_demo.$(OBJ):             mouse_demo.c 
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

mouse_demo $(BOOTDIR)mouse_demo: $(OBJDIR)mouse_demo.nh
	$(COPY) $(OBJDIR)mouse_demo.nh $(COPY_TO) $(BOOTDIR)mouse_demo
	$(CHMOD) 664 $(BOOTDIR)mouse_demo

ifeq "$(EB_BUILD_SYSTEM)" "WINNT"
$(OBJDIR)mouse_demo.out: CRT0 ${MOUSE} ${LIBS}
	$(LINK) -BASE:0x$(BOOTADR) $(LINKFLAGS) -OUT:$@ $(OBJDIR)crt0.$(OBJ) ${MOUSE} $(OBJDIR)/uart.$(OBJ) ${LIBS} $(LINKLIBS)
else
$(OBJDIR)mouse_demo.out: CRT0 ${MOUSE} ${LIBS}
	$(LD) -N -T $(BOOTADR) $(LDFLAGS) -o $@ ${MOUSE} $(OBJDIR)/uart.$(OBJ) ${LIBS} $(LDLIBS)
endif

#-----------------
SOURCE = regtest.s
PROGRAM = regtest

$(PROGRAM) $(BOOTDIR)$(PROGRAM): $(OBJDIR)$(PROGRAM).img
	$(COPY) $(OBJDIR)$(PROGRAM).img $(COPY_TO) $(BOOTDIR)$(PROGRAM)
	$(CHMOD) 664 $(BOOTDIR)$(PROGRAM)

ifeq "$(EB_BUILD_SYSTEM)" "WINNT"
$(OBJDIR)$(PROGRAM).img: $(OBJDIR)$(PROGRAM).$(OBJ)
	$(STRIP) $(STRIP_FLAGS) $(OBJDIR)$(PROGRAM).$(OBJ) -o $(OBJDIR)$(PROGRAM).img
else
$(OBJDIR)$(PROGRAM).img: $(OBJDIR)$(PROGRAM).$(OBJ)
	$(STRIP) $(STRIP_FLAGS) $(OBJDIR)$(PROGRAM).$(OBJ) $(OBJDIR)$(PROGRAM).img
endif

$(OBJDIR)$(PROGRAM).$(OBJ): $(SOURCE)
	$(AS) -c $(CCFLAGS) $(INCLUDES) $(SOURCE) -o $(OBJDIR)$(PROGRAM).$(OBJ)

#-----------------

SUM = $(OBJDIR)sum.$(OBJ)

$(OBJDIR)sum.$(OBJ):            sum.c 
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

sum $(BOOTDIR)sum: $(OBJDIR)sum.nh
	$(COPY) $(OBJDIR)sum.nh $(COPY_TO) $(BOOTDIR)sum
	$(CHMOD) 664 $(BOOTDIR)sum

ifeq "$(EB_BUILD_SYSTEM)" "WINNT"
$(OBJDIR)sum.out: CRT0 ${SUM} ${LIBS}
	$(LINK) -BASE:0x$(BOOTADR) $(LINKFLAGS) -OUT:$@ $(OBJDIR)crt0.$(OBJ) ${SUM} ${LIBS} $(LINKLIBS)
else
$(OBJDIR)sum.out: CRT0 ${SUM} ${LIBS}
	$(LD) -N -T $(BOOTADR) $(LDFLAGS) -o $@ ${SUM} ${LIBS} $(LDLIBS)
endif

#-----------------

MTEST = $(OBJDIR)mtest.$(OBJ)

$(OBJDIR)mtest.$(OBJ):          mtest.c 
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

mtest $(BOOTDIR)mtest: $(OBJDIR)mtest.nh
	$(COPY) $(OBJDIR)mtest.nh $(COPY_TO) $(BOOTDIR)mtest
	$(CHMOD) 664 $(BOOTDIR)mtest

ifeq "$(EB_BUILD_SYSTEM)" "WINNT"
$(OBJDIR)mtest.out: CRT0 ${MTEST} ${LIBS}
	$(LINK) -BASE:0x$(BOOTADR) $(LINKFLAGS) -OUT:$@ $(OBJDIR)crt0.$(OBJ) ${MTEST} $(OBJDIR)/uart.$(OBJ) ${LIBS} $(LINKLIBS)
else
$(OBJDIR)mtest.out: CRT0 ${MTEST} ${LIBS}
	$(LD) -N -T $(BOOTADR) $(LDFLAGS) -o $@ ${MTEST} $(OBJDIR)/uart.$(OBJ) ${LIBS} $(LDLIBS)
endif

#-----------------

PIC = $(OBJDIR)pic.$(OBJ)

$(OBJDIR)pic.$(OBJ):            pic.c 
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

pic $(BOOTDIR)pic: $(OBJDIR)pic.nh
	$(COPY) $(OBJDIR)pic.nh $(COPY_TO) $(BOOTDIR)pic
	$(CHMOD) 664 $(BOOTDIR)pic

ifeq "$(EB_BUILD_SYSTEM)" "WINNT"
$(OBJDIR)pic.out: CRT0 ${PIC} ${LIBS}
	$(LINK) -BASE:0x$(BOOTADR) $(LINKFLAGS) -OUT:$@ $(OBJDIR)crt0.$(OBJ) ${PIC} ${LIBS} $(LINKLIBS)
else
$(OBJDIR)pic.out: CRT0 ${PIC} ${LIBS}
	$(LD) -N -T $(BOOTADR) $(LDFLAGS) -o $@ ${PIC} ${LIBS} $(LDLIBS)
endif

#-----------------

SOUND = $(OBJDIR)sound.$(OBJ)

$(OBJDIR)sound.$(OBJ):          sound.c 
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

sound $(BOOTDIR)sound: $(OBJDIR)sound.nh
	$(COPY) $(OBJDIR)sound.nh $(COPY_TO) $(BOOTDIR)sound
	$(CHMOD) 664 $(BOOTDIR)sound

ifeq "$(EB_BUILD_SYSTEM)" "WINNT"
$(OBJDIR)sound.out: CRT0 ${SOUND} ${LIBS}
	$(LINK) -BASE:0x$(BOOTADR) $(LINKFLAGS) -OUT:$@ $(OBJDIR)crt0.$(OBJ) ${SOUND} ${LIBS} $(LINKLIBS)
else
$(OBJDIR)sound.out: CRT0 ${SOUND} ${LIBS}
	$(LD) -N -T $(BOOTADR) $(LDFLAGS) -o $@ ${SOUND} ${LIBS} $(LDLIBS)
endif

#-----------------

NVRAM = $(OBJDIR)nvram.$(OBJ)

$(OBJDIR)nvram.$(OBJ):          nvram.c 
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

nvram $(BOOTDIR)nvram: $(OBJDIR)nvram.nh
	$(COPY) $(OBJDIR)nvram.nh $(COPY_TO) $(BOOTDIR)nvram
	$(CHMOD) 664 $(BOOTDIR)nvram

ifeq "$(EB_BUILD_SYSTEM)" "WINNT"
$(OBJDIR)nvram.out: CRT0 ${NVRAM} ${LIBS}
	$(LINK) -BASE:0x$(BOOTADR) $(LINKFLAGS) -OUT:$@ $(OBJDIR)crt0.$(OBJ) ${NVRAM} ${LIBS} $(LINKLIBS)
else
$(OBJDIR)nvram.out: CRT0 ${NVRAM} ${LIBS}
	$(LD) -N -T $(BOOTADR) $(LDFLAGS) -o $@ ${NVRAM} ${LIBS} $(LDLIBS)
endif

#-----------------

VGA = $(OBJDIR)vga.$(OBJ)

$(OBJDIR)vga.$(OBJ):            vga.c 
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

vga $(BOOTDIR)vga: $(OBJDIR)vga.nh
	$(COPY) $(OBJDIR)vga.nh $(COPY_TO) $(BOOTDIR)vga
	$(CHMOD) 664 $(BOOTDIR)vga

ifeq "$(EB_BUILD_SYSTEM)" "WINNT"
$(OBJDIR)vga.out: CRT0 ${VGA} ${LIBS}
	$(LINK) -BASE:0x$(BOOTADR) $(LINKFLAGS) -OUT:$@ $(OBJDIR)crt0.$(OBJ) ${VGA} ${LIBS} $(LINKLIBS)
else
$(OBJDIR)vga.out: CRT0 ${VGA} ${LIBS}
	$(LD) -N -T $(BOOTADR) $(LDFLAGS) -o $@ ${VGA} ${LIBS} $(LDLIBS)
endif

#-----------------

TIME = $(OBJDIR)wait.$(OBJ)

$(OBJDIR)wait.$(OBJ):           wait.c 
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

time $(BOOTDIR)time: $(OBJDIR)wait.nh
	$(COPY) $(OBJDIR)wait.nh $(COPY_TO) $(BOOTDIR)time
	$(CHMOD) 664 $(BOOTDIR)time

ifeq "$(EB_BUILD_SYSTEM)" "WINNT"
$(OBJDIR)wait.out: CRT0 ${TIME} ${LIBS}
	$(LINK) -BASE:0x$(BOOTADR) $(LINKFLAGS) -OUT:$@ $(OBJDIR)crt0.$(OBJ) ${TIME} ${LIBS} $(LINKLIBS)
else
$(OBJDIR)wait.out: CRT0 ${TIME} ${LIBS}
	$(LD) -N -T $(BOOTADR) $(LDFLAGS) -o $@ ${TIME} ${LIBS} $(LDLIBS)
endif

#-----------------

LP = $(OBJDIR)lp.$(OBJ)

$(OBJDIR)lp.$(OBJ):             lp.c 
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

lp $(BOOTDIR)lp: $(OBJDIR)lp.nh
	$(COPY) $(OBJDIR)lp.nh $(COPY_TO) $(BOOTDIR)lp
	$(CHMOD) 664 $(BOOTDIR)lp

ifeq "$(EB_BUILD_SYSTEM)" "WINNT"
$(OBJDIR)lp.out: CRT0 ${LP} ${LIBS}
	$(LINK) -BASE:0x$(BOOTADR) $(LINKFLAGS) -OUT:$@ $(OBJDIR)crt0.$(OBJ) ${LP} ${LIBS} $(LINKLIBS)
else
$(OBJDIR)lp.out: CRT0 ${LP} ${LIBS}
	$(LD) -N -T $(BOOTADR) $(LDFLAGS) -o $@ ${LP} ${LIBS} $(LDLIBS)
endif

#-----------------

FLASH = $(OBJDIR)flash.$(OBJ)

$(OBJDIR)flash.$(OBJ):          flash.c 
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

flash $(BOOTDIR)flash: $(OBJDIR)flash.nh
	$(COPY) $(OBJDIR)flash.nh $(COPY_TO) $(BOOTDIR)flash
	$(CHMOD) 664 $(BOOTDIR)flash

ifeq "$(EB_BUILD_SYSTEM)" "WINNT"
$(OBJDIR)flash.out: CRT0 ${FLASH} ${LIBS}
	$(LINK) -BASE:0x$(BOOTADR) $(LINKFLAGS) -OUT:$@ $(OBJDIR)crt0.$(OBJ) ${FLASH} ${LIBS} $(LINKLIBS)
else
$(OBJDIR)flash.out: CRT0 ${FLASH} ${LIBS}
	$(LD) -N -T $(BOOTADR) $(LDFLAGS) -o $@ ${FLASH} ${LIBS} $(LDLIBS)
endif

#-----------------

MFS = $(OBJDIR)mfs.$(OBJ)

$(OBJDIR)mfs.$(OBJ):            mfs.c 
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

mfs $(BOOTDIR)mfs: $(OBJDIR)mfs.nh
	$(COPY) $(OBJDIR)mfs.nh $(COPY_TO) $(BOOTDIR)mfs
	$(CHMOD) 664 $(BOOTDIR)mfs

ifeq "$(EB_BUILD_SYSTEM)" "WINNT"
$(OBJDIR)mfs.out: CRT0 ${MFS} ${LIBS}
	$(LINK) -BASE:0x$(BOOTADR) $(LINKFLAGS) -OUT:$@ $(OBJDIR)crt0.$(OBJ) ${MFS} ${LIBS} $(LINKLIBS)
else
$(OBJDIR)mfs.out: CRT0 ${MFS} ${LIBS}
	$(LD) -N -T $(BOOTADR) $(LDFLAGS) -o $@ ${MFS} ${LIBS} $(LDLIBS)
endif

#-----------------

FD = $(OBJDIR)fd.$(OBJ)

$(OBJDIR)fd.$(OBJ):             fd.c 
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

fd $(BOOTDIR)fd: $(OBJDIR)fd.nh
	$(COPY) $(OBJDIR)fd.nh $(COPY_TO) $(BOOTDIR)fd
	$(CHMOD) 664 $(BOOTDIR)fd

ifeq "$(EB_BUILD_SYSTEM)" "WINNT"
$(OBJDIR)fd.out: CRT0 ${FD} ${LIBS}
	$(LINK) -BASE:0x$(BOOTADR) $(LINKFLAGS) -OUT:$@ ${FD} $(OBJDIR)crt0.$(OBJ) ${LIBS} $(LINKLIBS)
else
$(OBJDIR)fd.out: CRT0 ${FD} ${LIBS}
	$(LD) -N -T $(BOOTADR) $(LDFLAGS) -o $@ ${FD} ${LIBS} $(LDLIBS)
endif

#-----------------

COMTROL = $(OBJDIR)comtrol.$(OBJ)

$(OBJDIR)comtrol.$(OBJ):                comtrol.c 
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

comtrol $(BOOTDIR)comtrol: $(OBJDIR)comtrol.nh
	$(COPY) $(OBJDIR)comtrol.nh $(COPY_TO) $(BOOTDIR)comtrol
	$(CHMOD) 664 $(BOOTDIR)comtrol

ifeq "$(EB_BUILD_SYSTEM)" "WINNT"
$(OBJDIR)comtrol.out: CRT0 ${COMTROL} ${LIBS}
	$(LINK) -BASE:0x$(BOOTADR) $(LINKFLAGS) -OUT:$@ $(OBJDIR)crt0.$(OBJ) ${COMTROL} ${LIBS} $(LINKLIBS)
else
$(OBJDIR)comtrol.out: CRT0 ${COMTROL} ${LIBS}
	$(LD) -N -T $(BOOTADR) $(LDFLAGS) -o $@ ${COMTROL} ${LIBS} $(LDLIBS)
endif

#-----------------

AVP = $(OBJDIR)avp.$(OBJ) $(OBJDIR)vp.$(OBJ)

$(OBJDIR)avp.$(OBJ):            avp.c 
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

$(OBJDIR)vp.$(OBJ):             vp.c 
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

avp $(BOOTDIR)avp: $(OBJDIR)avp.nh
	$(COPY) $(OBJDIR)avp.nh $(COPY_TO) $(BOOTDIR)avp
	$(CHMOD) 664 $(BOOTDIR)avp

ifeq "$(EB_BUILD_SYSTEM)" "WINNT"
$(OBJDIR)avp.out: CRT0 ${AVP} ${LIBS}
	$(LINK) -BASE:0x$(BOOTADR) $(LINKFLAGS) -OUT:$@ $(OBJDIR)crt0.$(OBJ) ${AVP} ${LIBS} $(LINKLIBS)
else
$(OBJDIR)avp.out: CRT0 ${AVP} ${LIBS}
	$(LD) -N -T $(BOOTADR) $(LDFLAGS) -o $@ ${AVP} ${LIBS} $(LDLIBS)
endif

#-----------------

TEST = $(OBJDIR)test.$(OBJ)

$(OBJDIR)test.$(OBJ):           test.c 
	$(CC) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

test $(BOOTDIR)test: $(OBJDIR)test.nh
	$(COPY) $(OBJDIR)test.nh $(COPY_TO) $(BOOTDIR)test
	$(CHMOD) 664 $(BOOTDIR)test

ifeq "$(EB_BUILD_SYSTEM)" "WINNT"
$(OBJDIR)test.out: CRT0 ${TEST} $(OBJDIR)uart.$(OBJ) ${LIBS}
	$(LINK) -BASE:0x$(BOOTADR) $(LINKFLAGS) -OUT:$@ $(OBJDIR)crt0.$(OBJ) ${TEST} $(OBJDIR)uart.$(OBJ) ${LIBS} $(LINKLIBS)
else
$(OBJDIR)test.out: CRT0 ${TEST} $(OBJDIR)uart.$(OBJ) ${LIBS}
	$(LD) -N -T $(BOOTADR) $(LDFLAGS) -o $@ ${TEST} $(OBJDIR)uart.$(OBJ) ${LIBS} $(LDLIBS)
endif

#-----------------

$(BOOTDIR):
	mkdir $(BOOTDIR)

CRT0: $(OBJDIR)cstartup.$(OBJ)
	$(COPY) $(OBJDIR)cstartup.$(OBJ) $(COPY_TO) $(OBJDIR)crt0.$(OBJ)

#-------------------------------------------------------------

depend: $(SRCS)
	$(MAKEDEPEND) -p'$$(OBJDIR)' -- $(DEPENDFLAGS) -- $(SRCS)

clean:  
	-rm -f *~ \#*\# core ${MAKEFILE}.bak \
		$(OBJDIR)hello* \
		$(OBJDIR)size* \
		$(OBJDIR)yellow* \
		$(OBJDIR)loops* \
		$(OBJDIR)mouse* \
		$(OBJDIR)regtest* \
		$(OBJDIR)sum* \
		$(OBJDIR)mtest* \
		$(OBJDIR)pic* \
		$(OBJDIR)sound* \
		$(OBJDIR)nvram* \
		$(OBJDIR)vga* \
		$(OBJDIR)time* \
		$(OBJDIR)lp* \
		$(OBJDIR)flash* \
		$(OBJDIR)mfs* \
		$(OBJDIR)fd* \
		$(OBJDIR)comtrol* \
		$(OBJDIR)avp*

	$(CLEAN)

distclean:      clean
	$(MAKEDEPEND) -f $(MAKEFILE) 
	$(DELETE) $(MAKEFILE).bak


# DO NOT DELETE THIS LINE -- make depend depends on it.
