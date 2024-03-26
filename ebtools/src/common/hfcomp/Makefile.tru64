# $Id: Makefile,v 1.4 1999/03/21 22:48:29 paradis Exp $
# $Log: Makefile,v $
# Revision 1.4  1999/03/21 22:48:29  paradis
# Fix build of micropal.s for dp264
# Re-build NT utilities
#
# Revision 1.3  1999/02/17 22:36:54  paradis
# Defined -DROM only for those files that are going into ROMable code
# Undefined it for main hfcomp executable
#
# Revision 1.2  1999/01/26 20:14:46  paradis
# Fixed duplicate .c.o rule
#
# Revision 1.1.1.1  1998/12/29 21:36:25  paradis
# Initial CVS checkin
#
# Revision 1.41  1997/12/18  15:14:51  pbell
# Added cstartup back to the link and fixed a typo.
#
# Revision 1.40  1997/12/17  16:39:33  pbell
# -
#
# Revision 1.39  1997/12/05  16:29:49  pbell
# turned off pvc check for the unix build.  added install target list.
#
# Revision 1.38  1997/11/19  21:48:33  pbell
# Updated for DC21264 and streamlined the chip specific version
# dependency statements for simpler maintenance.
#
# Revision 1.38  1997/10/02  18:03:10  fdh
# Modified to use the TYPE definition to support various CPU types.
#
# Revision 1.37  1997/08/10  20:34:27  fdh
# Moved the DECOMP_... definitions to rules.osf.
#
# Revision 1.36  1997/05/30  01:43:36  fdh
# Modified CCFLAGS and ASFLAGS.
#
# Revision 1.35  1997/05/27  19:23:30  fdh
# Added the -c switch to the assembly rule for cstartup.s.
#
# Revision 1.34  1997/05/23  19:47:10  fdh
# Removed an obsoleted definition.
#
# Revision 1.33  1997/05/08  17:16:41  fdh
# Updated include file dependencies.
#
# Revision 1.32  1997/05/06  15:42:51  fdh
# Corrected the definition for building cstartup.o.
# Removed some obsoleted definitions.
#
# Revision 1.31  1997/02/22  21:11:07  fdh
# Modified to build from a local copy of cstartup.s.
#
# Revision 1.30  1997/02/22  20:47:45  fdh
# Updated include file dependencies.
#
# Revision 1.29  1997/02/21  20:57:57  fdh
# Modified assembler rule.
#
# Revision 1.28  1996/12/22  12:57:49  fdh
# Added switch to disable the dpml stub.
# Turned on the -segment_page 16 switch to
# work around a linker bug in Unix 4.0.
#
# Revision 1.27  1996/07/07  16:39:14  fdh
# The -segment_page option is an undocumented option that can be
# used to get around a problem with the version of ld that first
# shipped with Digital Unix 4.0 where the origin of the text segment
# is automatically forced to a 64KB boundary. This problem will be
# fixed in a future release of Digital Unix.
#
# Revision 1.26  1996/06/13  03:12:26  fdh
# Removed obsolete preprocessor definition.
#
# Revision 1.25  1996/05/27  15:02:43  fdh
# Changed DECOMP_COMPRESSED from 304000 to 303100.
#
# Revision 1.24  1995/12/13  16:21:47  fdh
# Added missing .img extension.
#
# Revision 1.23  1995/12/09  13:32:38  fdh
# Added .img extension to decompression library filenames.
#
# Revision 1.22  1995/12/07  07:26:32  fdh
# Modified to accomodate changes in the way that the hfcomp
# tools works.
# Added the DECOMP_COMPRESSED and ASSEMBLY_CODE definitions.
#
# Revision 1.21  1995/12/02  21:21:41  fdh
# Added .lis files to clean: target.
#
# Revision 1.20  1995/11/16  15:31:32  fdh
# Added DECOMP_PALBASE and DECOMP_BASE definitions.
# Added LINT definitions.
# Added targets for Creating .lis files.
# Changed label used for PVC.
# Fixed up depend: target to do the right thing.
#
# Revision 1.19  1995/11/06  21:01:06  cruz
# Updated compile rule define NO_INITDATA instead of NO_DOINITDATA
#
# Revision 1.18  1995/05/19  20:26:10  fdh
# Made uncompressed image entry point a compile-
# time parameter throught the DECOMP_DESTINATION macro.
#
# Revision 1.17  1995/04/20  01:40:54  fdh
# Set file permissions during install.
#
# Revision 1.16  1995/04/18  20:15:43  fdh
# Added micropal.s to the depend target.
# Added $(INCLUDES) to the $(CPP) command lines.
# Updated include file dependencies.
#
# Revision 1.15  1995/04/13  20:26:22  fdh
# Modified to use decmp64 and decmp164 filenames instead of
# decmpev4 and decmpev5.  Also removed other instances of ev4
# and ev5.
#
# Revision 1.14  1995/03/20  20:35:45  cruz
# Added PVC checking
# Added compilation switch for EV5.
#
# Revision 1.13  1995/03/05  00:00:49  fdh
# Added -g switch to the cc command line...
#
# Revision 1.12  1995/02/27  20:06:10  fdh
# Updated depend target and include file dependencies.
#
# Revision 1.11  1995/02/24  20:45:51  fdh
# Local definition for the INCLUDES Macro.
#
# Revision 1.10  1995/02/13  21:50:50  cruz
# Replaced sysgen invokation.
#
# Revision 1.9  1995/02/07  00:58:29  fdh
# Updated include file dependencies...
#
# Revision 1.8  1995/02/06  12:52:10  curewitz
# ran make depend after eliminating use of qscan.c and qscan_def.h
#
# Revision 1.7  1995/02/02  20:58:25  curewitz
# simplified command parsing
#
# Revision 1.6  1995/02/02  14:09:39  fdh
# Also changed compress.c to hfcomp.c
#
# Revision 1.5  1995/02/01  19:59:34  curewitz
# changed compressor program name to hfcomp,
# changed object file name to inflate_s.o for stand-alone
# decompressor
#
# Revision 1.4  1995/02/01  18:06:38  fdh
# Fixups.
#
# Revision 1.3  1995/02/01  16:58:46  fdh
# Added rcsclean to clean target.
#
# Revision 1.2  1995/02/01  16:56:22  fdh
# Added micropal.s to dependency lists.
#
# Revision 1.1  1995/02/01  16:40:56  curewitz
# Initial revision
#
CONFIG_DIR=../../../..
include $(CONFIG_DIR)/sdkmake.conf

include $(EB_BASE)/rules.gnu

MAKEFILE = Makefile

CPU_64          = 21064
CPU_164         = 21164
CPU_264         = 21264
CPU             = $(CPU_$(TYPE))

CPU_DTBMISS_64  = 9E0
CPU_DTBMISS_164 = 200
CPU_DTBMISS_264 = 300
CPU_DTBMISS     = $(CPU_DTBMISS_$(TYPE))

HEADER_DEFS =   -DDECOMP_PALBASE=0x$(DECOMP_PALBASE) \
		-DDECOMP_BASE=0x$(DECOMP_BASE)

#               -DDECOMP_COMPRESSED=0x$(DECOMP_COMPRESSED)

TOOLFLAGS_264   = -arch ev56

CCFLAGS = -g $(HEADER_DEFS) -DNT_UP $(TOOLFLAGS_$(TYPE))
ASFLAGS = -g $(TOOLFLAGS_$(TYPE))

CPPFLAGS =  -DPVCCHECK
MAKEDEP =       $(CPP) -MM
DEFINES =       $(HEADER_DEFS)
CFLAGS  =       -g
GASFLAGS =      -P
TOOLBOX =       $(EB_TOOLBOX)
TOOLOBJ =       ./
INCLUDES = -I. -I../include
PVC = $(TOOLBOX)/pvc
LINT            = /usr/bin/lint
LINTFLAGS       =  -1 -wp -av -n -I. -I../include

OBJDIR = $(TOOLOBJ)

CFILES  = bits.c \
	  deflate.c \
	  hfcomp.c \
	  inflate.c \
	  trees.c

OFILES  = $(CFILES:.c=.o)

CCFLAGS += $(DEFINES)

#.c.o:
#       $(CC) $(CFLAGS) $(DEFINES) -c $<

INSTALL_TARGETS = hfcomp decmp64.img decmp164.img decmp264.img

all: $(TOOLOBJ)hfcomp decomp64 decomp164 decomp264

$(TOOLOBJ)hfcomp:       $(OFILES)
	$(CC) $(CFLAGS) $(DEFINES) -o $(TOOLOBJ)hfcomp $(OFILES)

#-----------------

decomp64:
	$(EB_MAKE) -f $(MAKEFILE) TYPE=64 decomp_target

decomp164:
	$(EB_MAKE) -f $(MAKEFILE) TYPE=164 decomp_target

decomp264:                                 
	$(EB_MAKE) -f $(MAKEFILE) TYPE=264 decomp_target

decomp_target: $(TOOLOBJ)decmp$(TYPE).img $(TOOLOBJ)upal$(TYPE).lis

#-----------------

DECOMP = $(TOOLOBJ)decmp$(TYPE).o \
	 $(TOOLOBJ)inf_s$(TYPE).o \
	 $(TOOLOBJ)dbmmn$(TYPE).o 

$(TOOLOBJ)decmp$(TYPE).o: decomp.c decomp.h
	$(CC) $(CCFLAGS) -DROM -o $@ -c decomp.c

$(TOOLOBJ)inf_s$(TYPE).o: inflate.c decomp.h
	$(CC) $(CCFLAGS) -DROM -o $@ -c inflate.c

$(TOOLOBJ)dbmmn$(TYPE).o: dbmmain.c decomp.h
	$(CC) $(CCFLAGS) -o $@ -c dbmmain.c

$(TOOLOBJ)cstartup.o: cstartup.s
	$(AS) -DNO_INITDATA -DNO_EXITDATA $(ASFLAGS) -o $@ \
	-c $(INCLUDES) cstartup.s

# The -segment_page option is an undocumented option that can be
# used to get around a problem with the version of ld that first
# shipped with Digital Unix 4.0 where the origin of the text segment
# is automatically forced to a 64KB boundary. This problem will be
# fixed in a future release of Digital Unix.
$(TOOLOBJ)decmp$(TYPE).out: $(TOOLOBJ)cstartup.o ${DECOMP}
	ld -N -segment_page 16 -T $(DECOMP_BASE) -non_shared -o $@ $(TOOLOBJ)cstartup.o ${DECOMP}

# Older versions of the c compiler don't recognize the -segment_page 16
# switch. The C front end doesn't always pass the argument along with the
# -segment_page switch to the linker.
#$(TOOLOBJ)decmp$(TYPE).out: CRT0 ${DECOMP}
#       $(LD) -N -T $(DECOMP_BASE) $(LDFLAGS) -o $@ ${DECOMP}

#-------
$(TOOLOBJ)decmp$(TYPE).nh: $(TOOLOBJ)decmp$(TYPE).out

$(TOOLOBJ)decmp$(TYPE).img: $(TOOLOBJ)upal$(TYPE).nh $(TOOLOBJ)decmp$(TYPE).nh $(TOOLOBJ)pvc$(TYPE).pvc
	$(SYSGEN) -v -e0      -s $(TOOLOBJ)upal$(TYPE).nh       \
	       -eb00    -s $(TOOLOBJ)decmp$(TYPE).nh    \
	       -o $@
#       $(PVC) < $(TOOLOBJ)pvc$(TYPE).pvc

$(TOOLOBJ)pvc$(TYPE).pvc:       $(TOOLOBJ)upal$(TYPE).ent $(TOOLOBJ)upal$(TYPE).map $(TOOLOBJ)upal$(TYPE).nh
	echo set code_file $(TOOLOBJ)upal$(TYPE).nh > $@
	echo set entry_file $(TOOLOBJ)upal$(TYPE).ent >> $@
	echo set cpu $(CPU) >> $@
	echo go >> $@
	echo quit >> $@

$(TOOLOBJ)upal$(TYPE).ent:
	echo 0 uPalReset > $@
	echo $(CPU_DTBMISS) PalDtbMiss >> $@

$(TOOLOBJ)upal$(TYPE).map:      $(TOOLOBJ)upal$(TYPE).o
	$(ALIST) -m $(TOOLOBJ)upal$(TYPE).o > $@

$(TOOLOBJ)upal$(TYPE).nh:       $(TOOLOBJ)upal$(TYPE).o
	$(ASTRIP) -a $(TOOLOBJ)upal$(TYPE).o $@

$(TOOLOBJ)upal$(TYPE).o:        $(TOOLOBJ)upal$(TYPE).cpp
	$(GAS) $(GASFLAGS) -$(CPU) -o $(TOOLOBJ)upal$(TYPE).o upal$(TYPE).cpp

$(TOOLOBJ)upal$(TYPE).cpp:      micropal.s
	$(CPP)  $(CPPFLAGS) $(INCLUDES) -DDC$(CPU) -DASSEMBLY_CODE \
	$(HEADER_DEFS) micropal.s > $(TOOLOBJ)upal$(TYPE).cpp 

$(TOOLOBJ)upal$(TYPE).lis: $(TOOLOBJ)upal$(TYPE).o
	$(ALIST) $*.o > $*.lis

#-------

CRT0: $(TOOLOBJ)cstartup.o
	cp -fp $(TOOLOBJ)cstartup.o $(TOOLOBJ)crt0.o

lint:   
	$(LINT) $(LINTFLAGS) $(INCLUDES) $(CFILES)

install:    all
	chmod 775 $(INSTALL_TARGETS)
	cp -fp $(INSTALL_TARGETS) $(TOOLBOX)

depend: $(CFILES) dbmmain.c $(TOOLOBJ)cstartup.s micropal.s
	makedepend -f $(MAKEFILE) -- $(CFLAGS) -DDC21264 -DDC21164 -DDC21064 $(INCLUDES) -- \
	    $(CFILES) dbmmain.c $(TOOLOBJ)cstartup.s 
	makedepend -a -f $(MAKEFILE) \
	    -p'$$(TOOLOBJ)upal264.cpp $$(TOOLOBJ)upal164.cpp $$(TOOLOBJ)upal64.cpp ' \
	    -- $(CFLAGS) -DDC21264 -DDC21164 -DDC21064 $(INCLUDES) -- \
	    micropal.s
clean:
	rm -f *~ \#*\# core $(MAKEFILE).bak 
	rm -f $(TOOLOBJ)*.o  $(TOOLOBJ)*.nh $(TOOLOBJ)decmp*.out $(TOOLOBJ)*.lis $(TOOLOBJ)*.dis
	rm -f $(TOOLOBJ)*.map $(TOOLOBJ)*.pvc $(TOOLOBJ)*.ent $(TOOLOBJ)*.cpp
	rm -f $(TOOLOBJ)decmp*.img $(TOOLOBJ)hfcomp

clobber: clean
	rm -f $(TOOLBOX)decmp*.img $(TOOLBOX)hfcomp

# DO NOT DELETE THIS LINE -- make depend depends on it.

bits.o: /usr/include/stdio.h /usr/include/standards.h /usr/include/sys/seek.h
bits.o: /usr/include/va_list.h /usr/include/sys/types.h
bits.o: /usr/include/mach/machine/vm_types.h /usr/include/sys/select.h
bits.o: /usr/include/getopt.h /usr/include/sys/limits.h
bits.o: /usr/include/sys/machine/machlimits.h /usr/include/sys/syslimits.h
bits.o: /usr/include/sys/machine/machtime.h /usr/include/sys/rt_limits.h
bits.o: compress.h /usr/include/string.h /usr/include/strings.h
deflate.o: /usr/include/stdio.h /usr/include/standards.h
deflate.o: /usr/include/sys/seek.h /usr/include/va_list.h
deflate.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
deflate.o: /usr/include/sys/select.h /usr/include/getopt.h
deflate.o: /usr/include/sys/limits.h /usr/include/sys/machine/machlimits.h
deflate.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
deflate.o: /usr/include/sys/rt_limits.h /usr/include/stdlib.h
deflate.o: /usr/include/string.h /usr/include/strings.h compress.h
hfcomp.o: /usr/include/stdio.h /usr/include/standards.h
hfcomp.o: /usr/include/sys/seek.h /usr/include/va_list.h
hfcomp.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
hfcomp.o: /usr/include/sys/select.h /usr/include/getopt.h
hfcomp.o: /usr/include/sys/limits.h /usr/include/sys/machine/machlimits.h
hfcomp.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
hfcomp.o: /usr/include/sys/rt_limits.h /usr/include/string.h
hfcomp.o: /usr/include/strings.h /usr/include/math.h /usr/include/stdlib.h
hfcomp.o: /usr/include/sys/stat.h /usr/include/sys/mode.h
hfcomp.o: /usr/include/ctype.h /usr/include/sys/localedef.h
hfcomp.o: /usr/include/sys/lc_core.h /usr/include/locale.h
hfcomp.o: /usr/include/nl_types.h /usr/include/mesg.h /usr/include/limits.h
hfcomp.o: compress.h header.h
inflate.o: /usr/include/stdlib.h /usr/include/standards.h
inflate.o: /usr/include/getopt.h /usr/include/sys/types.h
inflate.o: /usr/include/mach/machine/vm_types.h /usr/include/sys/select.h
inflate.o: /usr/include/stdio.h /usr/include/sys/seek.h
inflate.o: /usr/include/va_list.h /usr/include/sys/limits.h
inflate.o: /usr/include/sys/machine/machlimits.h /usr/include/sys/syslimits.h
inflate.o: /usr/include/sys/machine/machtime.h /usr/include/sys/rt_limits.h
inflate.o: /usr/include/string.h /usr/include/strings.h decomp.h
trees.o: /usr/include/ctype.h /usr/include/standards.h
trees.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
trees.o: /usr/include/sys/select.h /usr/include/sys/localedef.h
trees.o: /usr/include/sys/lc_core.h /usr/include/locale.h
trees.o: /usr/include/nl_types.h /usr/include/mesg.h /usr/include/stdio.h
trees.o: /usr/include/sys/seek.h /usr/include/va_list.h /usr/include/getopt.h
trees.o: /usr/include/sys/limits.h /usr/include/sys/machine/machlimits.h
trees.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
trees.o: /usr/include/sys/rt_limits.h /usr/include/limits.h compress.h
dbmmain.o: decomp.h
./cstartup.o: ../include/regdefs.h ../include/paldefs.h

$(TOOLOBJ)upal264.cpp $(TOOLOBJ)upal164.cpp $(TOOLOBJ)upal64.cpp micropal.o: header.h
$(TOOLOBJ)upal264.cpp $(TOOLOBJ)upal164.cpp $(TOOLOBJ)upal64.cpp micropal.o: ../include/cpu.h
$(TOOLOBJ)upal264.cpp $(TOOLOBJ)upal164.cpp $(TOOLOBJ)upal64.cpp micropal.o: ../include/dc21064.h
$(TOOLOBJ)upal264.cpp $(TOOLOBJ)upal164.cpp $(TOOLOBJ)upal64.cpp micropal.o: ../include/dc21164.h
$(TOOLOBJ)upal264.cpp $(TOOLOBJ)upal164.cpp $(TOOLOBJ)upal64.cpp micropal.o: ../include/dc21264.h
