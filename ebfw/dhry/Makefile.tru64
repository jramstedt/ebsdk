# $Id: Makefile,v 1.1.1.1 1998/12/29 21:36:12 paradis Exp $
# $Log: Makefile,v $
# Revision 1.1.1.1  1998/12/29 21:36:12  paradis
# Initial CVS checkin
#
# Revision 1.14  1997/05/28  14:52:58  fdh
# Split up compile and link command lines.
#
# Revision 1.13  1997/02/21  19:56:50  fdh
# Modified compile and link rules.
#
# Revision 1.12  1996/05/26  20:08:44  fdh
# Modified to use more global definitions.
#
# Revision 1.11  1996/05/22  22:31:28  fdh
# Increased DLOOPS.
#
# Revision 1.10  1995/12/12  20:43:03  fdh
# Removed unnecessary subdirectory delimeters "/".
#
# Revision 1.9  1995/10/31  19:00:46  cruz
# Added dummy lint rule.
#
# Revision 1.8  1995/10/13  20:45:58  fdh
# Changed a file name.
#
# Revision 1.7  1995/08/28  21:32:56  fdh
# Turn off verbose switch (-v)
#
# Revision 1.6  1995/08/25  20:28:43  fdh
# Removed compiles for malloc.  Moved to ebfw/lib.
#
# Revision 1.5  1994/07/22  21:01:24  fdh
# Added dhryV1.o to clean target.
#
# Revision 1.4  1994/07/22  20:47:58  fdh
# Added $(SYSTEMOBJECTS) to clean target.
#
# Revision 1.3  1994/07/22  20:26:58  fdh
# Fixed up definitions for OPTIONS and LD_FLAGS
#
# Revision 1.2  1994/07/17  05:20:00  fdh
# *** empty log message ***
#
# Revision 1.1  1994/07/17  03:07:19  fdh
# Initial revision
#
#

CONFIG_DIR=../..
include $(CONFIG_DIR)/sdkmake.conf
include ../rules.gnu

LIBS =	$(OBJDIR)lib.$(LIBEXT) \
	$(OBJDIR)stubs.$(LIBEXT)

ifeq "$(EB_BUILD_SYSTEM)" "WINNT"
CCFLAGS = \
	$($(TARGET)_CC_FLAGS_GLOBAL$(SYSREVID)) \
	-DDEBUG_MONITOR -DREG=register -DLOOPS=50000000 \
	-DHZ=1 -DTIME
else
CCFLAGS = \
	$($(TARGET)_CC_FLAGS_GLOBAL$(SYSREVID)) \
	-DDEBUG_MONITOR -DREG=register -DLOOPS=50000000 \
	-DHZ=1 -DTIME -unsigned
endif

LDFLAGS	= -non_shared -tr -h$(OBJDIR) -B

all:	$(BOOTDIR)dhryV1 $(BOOTDIR)dhryV2

SRCS	=	dhryV1.c dhryV2_1.c dhryV2_2.c

SYSTEMOBJECTS = \
	$(OBJDIR)strcpy.$(OBJ) \
	$(OBJDIR)strcmp.$(OBJ) \
	$(OBJDIR)__divq.$(OBJ) \
	$(OBJDIR)__divl.$(OBJ) \
	$(OBJDIR)__divqu.$(OBJ) \
	$(OBJDIR)__remqu.$(OBJ)

ifeq "$(EB_BUILD_SYSTEM)" "WINNT"
DHRYV2_CFLAGS=
DHRYV1_CFLAGS=
else
DHRYV2_CFLAGS=-std0 -g0 -O3
DHRYV1_CFLAGS=-std0 -g0 -O2
endif

#-----------------

DHRYV2 = $(OBJDIR)dhryV2_1.$(OBJ) $(OBJDIR)dhryV2_2.$(OBJ) # $(SYSTEMOBJECTS)

$(OBJDIR)dhryV2_1.$(OBJ):		dhryV2_1.c 
	$(CC) $(DHRYV2_CFLAGS) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

$(OBJDIR)dhryV2_2.$(OBJ):		dhryV2_2.c 
	$(CC) $(DHRYV2_CFLAGS) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

dhryV2 $(BOOTDIR)dhryV2: $(OBJDIR)dhryV2.nh
	$(COPY) $(OBJDIR)dhryV2.nh $(COPY_TO) $(BOOTDIR)dhryV2
	$(CHMOD) 664 $(BOOTDIR)dhryV2

ifeq "$(EB_BUILD_SYSTEM)" "WINNT"
$(OBJDIR)dhryV2.out: CRT0 ${DHRYV2} ${LIBS}
	$(LINK) -BASE:0x$(BOOTADR) $(LINKFLAGS) -OUT:$@ $(OBJDIR)crt0.$(OBJ) ${DHRYV2} ${LIBS}
else
$(OBJDIR)dhryV2.out: CRT0 ${DHRYV2} ${LIBS}
	$(LD) -g0 -O3 -N -T $(BOOTADR) $(LDFLAGS) -o $@ ${DHRYV2} ${LIBS}
endif

#-----------------

DHRYV1 = $(OBJDIR)dhryV1.$(OBJ) # $(SYSTEMOBJECTS)

$(OBJDIR)dhryV1.$(OBJ):		dhryV1.c 
	$(CC) $(DHRYV1_CFLAGS) $(CCFLAGS) -o $@ -c $(INCLUDES) $<

dhryV1 $(BOOTDIR)dhryV1: $(OBJDIR)dhryV1.nh
	$(COPY) $(OBJDIR)dhryV1.nh $(COPY_TO) $(BOOTDIR)dhryV1
	$(CHMOD) 664 $(BOOTDIR)dhryV1

ifeq "$(EB_BUILD_SYSTEM)" "WINNT"
$(OBJDIR)dhryV1.out: CRT0 ${DHRYV1} ${LIBS}
	$(LINK) -BASE:0x$(BOOTADR) $(LINKFLAGS) -OUT:$@ $(OBJDIR)crt0.$(OBJ) ${DHRYV1} ${LIBS}
else
$(OBJDIR)dhryV1.out: CRT0 ${DHRYV1} ${LIBS}
	$(LD) -std0 -g0 -O2 -N -T $(BOOTADR) $(LDFLAGS) -o $@ ${DHRYV1} ${LIBS}
endif

#-----------------

$(SYSTEMOBJECTS):
	(cd $(OBJDIR); ar x /usr/lib/libc.$(LIBEXT) $*.$(OBJ);)

#-----------------

$(BOOTDIR):
	mkdir $(BOOTDIR)

CRT0: $(OBJDIR)cstartup.$(OBJ)
	$(COPY) $(OBJDIR)cstartup.$(OBJ) $(COPY_TO) $(OBJDIR)crt0.$(OBJ)

#-------------------------------------------------------------
depend:
#-------------------------------------------------------------


clean:	
	-rm -f *~ \#*\# core ${MAKEFILE}.bak \
		$(SYSTEMOBJECTS) \
		$(OBJDIR)dhryV1.* dhryV1.$(OBJ) \
		$(OBJDIR)dhryV2.* \
		$(OBJDIR)dhryV2_1.$(OBJ) dhryV2_1.$(OBJ) \
		$(OBJDIR)dhryV2_2.$(OBJ) dhryV2_2.$(OBJ) \
		$(OBJDIR)dhryV2_1.u dhryV2_1.u \
		$(OBJDIR)dhryV2_2.u dhryV2_2.u \
		$(OBJDIR)crt0.$(OBJ)

	$(CLEAN)
