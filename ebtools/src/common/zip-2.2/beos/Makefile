######################################################################
# Makefile for Zip, ZipNote, ZipCloak and ZipSplit on BeOS
#
# This new improved Makefile unifies the build process for all known
# BeOS C/C++ compilers.

all:
	@echo ''
	@echo 'Make what?  You must say what compiler you want to use -- '
	@echo 'for example, "make -f beos/Makefile mwcc".'
	@echo ''
	@echo 'The supported compilers are:'
	@echo ''
	@echo '     gcc     GNU C'
	@echo '     mwcc    Metrowerks CodeWarrior'
	@echo ''

list: all

######################################################################
# And now, on with the Makefile...

MAKE = make -f beos/Makefile
SHELL = /bin/sh

# For now, we don't have anything like links.  DR9 should fix that,
# with support for symlinks.
LN = ln -s

# Supported compilers
MW_CC  = mwcc
GNU_CC = gcc

# flags
#   CFLAGS    flags for C compile
#   LFLAGS1   flags after output file spec, before obj file list
#   LFLAGS2   flags after obj file list (libraries, etc)
MW_CFLAGS  = -O7 -rostr -w9 -ansi strict -I. -DHAVE_DIRENT_H -DNO_MKTEMP \
			 -DPASSWD_FROM_STDIN
MW_LFLAGS1 =
MW_LFLAGS2 =

GNU_CFLAGS  = -O3 -mcpu=604 -Wall -ansi \
			  -I. -I/boot/develop/headers/be/support \
			  -I/boot/develop/headers/be/storage \
			  -DHAVE_DIRENT_H -DNO_MKTEMP -DPASSWD_FROM_STDIN
GNU_LFLAGS1 =
GNU_LFLAGS2 =

# Compiler-related stuff.
BIND = $(CC)
AS = $(CC) -c
CPP = $(CC) -E

# Our executables know they're executables, and don't require some
# magic file extension.
E =

# probably can change this to 'install' if you have it
INSTALL = install

# target directories - where to install executables and man pages to
prefix = /boot/home/config
BINDIR = $(prefix)/bin
manext=1
MANDIR = $(prefix)/man/man$(manext)
ZIPMANUAL = MANUAL
VERSION = Version 2.2 of 7 August 1996

# object file lists
OBJZ = zip.o zipfile.o zipup.o fileio.o util.o globals.o crypt.o ttyio.o \
       beos.o crc32.o crctab.o
OBJI = deflate.o trees.o bits.o
OBJA =
OBJU = zipfile_.o fileio_.o util_.o globals.o beos_.o
OBJN = zipnote.o  $(OBJU)
OBJC = zipcloak.o $(OBJU) crctab.o crypt_.o ttyio.o
OBJS = zipsplit.o $(OBJU)

ZIP_H = zip.h ziperr.h tailor.h beos/osdep.h

# What to build?
ZIPS = zip$E zipnote$E zipsplit$E zipcloak$E

# suffix rules
.SUFFIXES:
.SUFFIXES: _.o .o .c .doc .1
.c_.o:
	rm -f $*_.c; $(LN) $< $*_.c
	$(CC) -c $(CFLAGS) -DUTIL $*_.c
	rm -f $*_.c
.c.o:
	$(CC) -c $(CFLAGS) $<

.1.doc:
	nroff -man $< | col -b | uniq > $@

# rules for zip, zipnote, zipcloak, zipsplit, and the Zip MANUAL.
$(OBJZ): $(ZIP_H)
$(OBJI): $(ZIP_H)
$(OBJN): $(ZIP_H)
$(OBJS): $(ZIP_H)
$(OBJC): $(ZIP_H)
zip.o zipup.o crypt.o ttyio.o bits.o zipcloak.o crypt_.o: crypt.h
zip.o zipup.o zipnote.o zipcloak.o zipsplit.o: revision.h
zip.o crypt.o ttyio.o zipcloak.o crypt_.o: ttyio.h
zipup.o: beos/zipup.h

beos.o: beos/beos.c
	$(CC) -c $(CFLAGS) beos/beos.c

beos_.o: beos/beos.c
	rm -f $*_.c; $(LN) beos/beos.c $*_.c
	$(CC) -c $(CFLAGS) -DUTIL $*_.c
	rm -f $*_.c

zips: $(ZIPS)
zipsman: $(ZIPS) $(ZIPMANUAL)

zip$E: $(OBJZ) $(OBJI) $(OBJA)
	$(BIND) -o zip$E $(LFLAGS1) $(OBJZ) $(OBJI) $(OBJA) $(LFLAGS2)
zipnote$E: $(OBJN)
	$(BIND) -o zipnote$E $(LFLAGS1) $(OBJN) $(LFLAGS2)
zipcloak$E: $(OBJC)
	$(BIND) -o zipcloak$E $(LFLAGS1) $(OBJC) $(LFLAGS2)
zipsplit$E: $(OBJS)
	$(BIND) -o zipsplit$E $(LFLAGS1) $(OBJS) $(LFLAGS2)

$(ZIPMANUAL): man/zip.1
	nroff -man man/zip.1 | col -b | uniq > $(ZIPMANUAL)

# install
install:        $(ZIPS)
	$(INSTALL) -m755 $(ZIPS) $(BINDIR)
	mkdir -p $(MANDIR)
	$(INSTALL) -m644 man/zip.1 $(MANDIR)/zip.$(manext)

uninstall:
	-cd $(BINDIR); rm -f $(ZIPS)
	-cd $(MANDIR); rm -f zip.$(manext)

dist: $(ZIPMANUAL)
	zip -u9T zip`sed -e '/VERSION/!d' -e 's/.*"\(.*\)".*/\1/' \
			  -e s/[.]//g -e q revision.h` \
	  `awk '/^Makefile/,/vms_zip.rnh/ {print $$1}' < contents`

# These symbols, when #defined using -D have these effects on compilation:
# ZMEM                  - includes C language versions of memset(), memcpy(),
#                         and memcmp() (util.c).
# HAVE_DIRENT_H         - use <dirent.h> instead of <sys/dir.h>
# NODIR                 - for 3B1, which has neither getdents() nor opendir().
# HAVE_NDIR_H           - use <ndir.h> (unix/unix.c).
# HAVE_SYS_DIR_H        - use <sys/dir.h>
# HAVE_SYS_NDIR_H       - use <sys/ndir.h>
# UTIL                  - select routines for utilities (note, cloak, split)
# NO_RMDIR              - remove directories using a system("rmdir ...") call.
# NO_PROTO              - cannot handle ANSI prototypes
# NO_CONST              - cannot handle ANSI const

######################################################################
# Targets for the various compilers.
gcc:
	@echo 'Making zip with GNU C...'
	@echo ''
	$(MAKE) $(ZIPS) CC=$(GNU_CC) CFLAGS="$(GNU_CFLAGS)" \
			LFLAGS1="$(GNU_LFLAGS1)" LFLAGS2="$(GNU_LFLAGS2)"

mwcc:
	@echo 'Making zip with Metrowerks CodeWarrior...'
	@echo ''
	$(MAKE) $(ZIPS) CC=$(MW_CC) CFLAGS="$(MW_CFLAGS)" \
			LFLAGS1="$(MW_LFLAGS1)" LFLAGS2="$(MW_LFLAGS2)"

# clean up after making stuff and installing it
clean:
	rm -f *.o $(ZIPS) flags

# end of Makefile
