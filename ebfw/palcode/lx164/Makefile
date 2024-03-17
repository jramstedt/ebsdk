# 
# $Id: Makefile,v 1.2 1999/03/24 17:44:10 paradis Exp $
#
# Revision History:
#
# $Log: Makefile,v $
# Revision 1.2  1999/03/24 17:44:10  paradis
# Added .ent file for pvc
#
# Revision 1.1.1.1  1998/12/29 21:36:07  paradis
# Initial CVS checkin
#
# Revision 1.18  1997/05/23  20:51:29  fdh
# Modified to create .obj rather than .o files to
# avoid a problem with the nmake files.
#
# Revision 1.17  1996/06/24  14:58:03  fdh
# Removed obsolete definitions.
#
# Revision 1.16  1996/06/17  16:15:40  fdh
# Modified DEFINES definition used to build the PALcode.
#
# Revision 1.15  1995/12/13  18:37:54  fdh
# Added rcsclean to the clean: target.
#
# Revision 1.14  1995/11/30  21:01:15  fdh
# Added junk files to clean: target
#
# Revision 1.13  1995/02/06  18:01:28  samberg
# Create ISP_EGORE for vice, eliminate config.h
#
# Revision 1.12  1994/09/07  18:08:54  samberg
# No KDEBUG, use Makefile.kdebug
#
# Revision 1.11  1994/09/07  16:50:23  samberg
# Add DISABLE_CRD
#
# Revision 1.10  1994/09/07  15:25:20  samberg
# Modified for use with Makefile.vpp, take out -DOSF
#
# Revision 1.9  1994/07/08  17:06:28  samberg
# Changes for platform specific additions
#
# Revision 1.8  1994/05/20  18:06:56  ericr
# Changed TOOLDIR macro to EB_TOOLBOX
#
# Revision 1.7  1994/01/31  19:13:48  ericr
# Define CPP to use /usr/bin/cpp
#
# Revision 1.6  1994/01/26  15:17:02  ericr
# Regenerate dependencies - logout.h incorporated into impure.h
#
# Revision 1.5  1994/01/18  20:26:44  ericr
# Regenerate dependencies
#
# Revision 1.4  1994/01/17  15:33:53  ericr
# Added impure.h and logout.h header files
#
# Revision 1.3  1993/12/23  16:46:41  ericr
# Removed rcsget dependency on target depend
#
# Revision 1.2  1993/12/20  16:36:06  eric
# Added PVC_CPU environment variable to pvc target
#
# Revision 1.1  1993/12/16  22:42:57  eric
# Initial revision
#
#

CONFIG_DIR=../../..
include $(CONFIG_DIR)/sdkmake.conf

#
# Set environment variables to point to various things:
#
#	EB_TOOLBOX - Where your toolset is located
#

# if these aren't included in rules.gnu, we're in trouble!
# CPP	  =	/usr/bin/cpp
# AS	  =	$(EB_TOOLBOX)/gas
# LD	  =	$(EB_TOOLBOX)/gld
# DIS	  =	$(EB_TOOLBOX)/alist
# STRIP	  =	$(EB_TOOLBOX)/astrip
# PVC	  =	$(EB_TOOLBOX)/pvc
# MAKEDEP	  =	$(CPP) -MM

#
# Define KDEBUG if you want a special unprivileged CALL_PAL 
# breakpoint trap handler for remote kernel debugging.
#
# Define CONSOLE_ENTRY to change the sys$enter_console
# transfer address.  Default CONSOLE_ENTRY value is 0x10000.
#
# Define DISABLE_CRD to disable CRD. Note that reset sets MCES so that
# correctable errors are ignored anyway, but this actually stops the
# interrupt.
#

DEFINES   =	-DKDEBUG -DCONSOLE_ENTRY=0x210000
CPPFLAGS  =
ASFLAGS   =	-21164
LDFLAGS   =	-Tstrip 0 -Thdr -N

# Source files:
#
#   This is the only block in which the list of source files should change.
#
#	SFILES - assembler source files
#	HFILES - header files
#

SFILES	= osfpal.s \
	  platform.s

HFILES	= dc21164.h \
	  osf.h \
	  macros.h \
	  impure.h \
	  cserve.h \
	  platform.h

# Intermediate files:
#
#   This block should not change.
#

IFILES	= $(SFILES:.s=.i)

# Object files:
#
#   This block should not change.
#

OFILES 	= $(IFILES:.i=.obj)

.DEFAULT:
	co -u $<

.SUFFIXES:
.SUFFIXES: .s .i .obj

.s.i:
	$(CPP) $(CPPFLAGS) $(DEFINES) $< $(CPPOUT) $*.i

osfpal.nh: osfpal
	$(STRIP) -a osfpal $@

osfpal: $(OFILES) 
	$(LD) $(LDFLAGS) -o $@ $(OFILES)

osfpal.obj: osfpal.i
	$(AS) $(ASFLAGS) -o $@ osfpal.i

platform.obj: platform.i
	$(AS) $(ASFLAGS) -o $@ platform.i

pvc: osfpal.lis osfpal.nh osfpal.ent osfpal.map
	(export PVC_PAL PVC_ENTRY PVC_MAP PVC_CPU;	\
	 PVC_PAL=osfpal.nh;				\
	 PVC_ENTRY=osfpal.ent;				\
	 PVC_MAP=osfpal.map;				\
	 PVC_CPU=ev5;					\
	 $(PVC);)

osfpal.lis: osfpal
	$(DIS) osfpal > $@

osfpal.map: osfpal
	$(DIS) -m osfpal > $@
	
depend:
	$(MAKEDEPEND) -f $(MAKEFILE) $(SFILES)

clean:
	rm -f *~ \#*\# core $(OFILES) $(IFILES) osfpal osfpal.lis osfpal.map osfpal.nh osfpal.i

clobber: clean
	rm -f osfpal.lis osfpal.nh osfpal.map osfpal

rcsinfo:
	rlog RCS/*

rcsget:
	co -u $(HFILES) $(SFILES)

