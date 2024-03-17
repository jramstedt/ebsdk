#==============================================================================
# Makefile for UnZip, UnZipSFX and fUnZip:  BeOS
# Version:  5.32                                               2 September 1997
#==============================================================================

# INSTRUCTIONS (such as they are):
#
# "make list" - lists all supported compilers (targets)
# "make foo"  - makes UnZip in current directory using the foo compiler
#
# CF are flags for the C compiler.  LF are flags for the loader.  LF2 are more
# flags for the loader, if they need to be at the end of the line instead of at
# the beginning (for example, some libraries).  FL and FL2 are the corre-
# sponding flags for fUnZip.  LOCAL_UNZIP is an environment variable that can
# be used to add default C flags to your compile without editing the Makefile
# (e.g., -DDEBUG_STRUC, or -FPi87 on PCs using Microsoft C).
#
# Be sure to test your new UnZip (and UnZipSFX and fUnZip); successful compila-
# tion does not always imply a working program.
#
# You might want to go down to the "installation" section and set the prefix
# to something else; not everyone is as strange as me, and you might not want
# to put your Info-ZIP executables in /boot/usr/local/bin.  [cjh]

all:
	@echo ''
	@echo 'Make what?  You must say what compiler you want to use -- '
	@echo 'for example, "make -f beos/Makefile foo".'
	@echo ''
	@echo 'The supported compilers are:'
	@echo ''
	@echo '     gcc     GNU C'
	@echo '     mwcc    Metrowerks CodeWarrior'
	@echo ''

list: all

#####################
# MACRO DEFINITIONS #
#####################

MAKE = make -f beos/Makefile

# Defaults most systems use (use LOCAL_UNZIP in environment to add flags,
# such as -DDOSWILD).

# UnZip flags
MW_CC  = mwcc
GNU_CC = gcc

LD = $(CC)
LOC = $(LOCAL_UNZIP) -DPASSWD_FROM_STDIN
AF = $(LOC)

# -ansi strict doesn't work with the AADR9 stdio.h... d'oh!
MW_CF = -w9 -ansi strict -O7 -rostr -I. $(LOC)
MW_LF = -o unzip
MW_LF2 =

GNU_CF = -Wall -ansi -mcpu=604 -O3 -I. -I/boot/develop/headers/be/support $(LOC)
GNU_LF = -o unzip
GNU_LF2 =

# UnZipSFX flags
SL = -o unzipsfx
SL2 = $(LF2)

# fUnZip flags
FL = -o funzip
FL2 = $(LF2)

# general-purpose stuff
CP = cp
RM = rm -f
LN = ln -sf
E =
O = .o
M = beos
SHELL = /bin/sh

# defaults for crc32 stuff and system-dependent headers
CRC32 = crc32
OSDEP_H = beos/beos.h

# object files
OBJS1 = unzip$O $(CRC32)$O crctab$O crypt$O envargs$O explode$O
OBJS2 = extract$O fileio$O globals$O inflate$O list$O match$O
OBJS3 = process$O ttyio$O unreduce$O unshrink$O zipinfo$O
OBJS = $(OBJS1) $(OBJS2) $(OBJS3) $M$O
LOBJS = $(OBJS)
OBJSDLL = $(OBJS) api$O
OBJX = unzipsfx$O $(CRC32)$O crctab$O crypt$O extract_$O fileio$O globals$O \
	inflate$O match$O process_$O ttyio$O $M_$O
LOBJX = $(OBJX)
OBJF = funzip$O $(CRC32)$O crypt_$O globals_$O inflate_$O ttyio_$O
UNZIP_H = unzip.h unzpriv.h globals.h $(OSDEP_H)

# installation
INSTALL = install
# on some systems, manext=l and MANDIR=/usr/man/man$(manext) may be appropriate
manext = 1
prefix = /boot/home/config
BINDIR = $(prefix)/bin#			where to install executables
MANDIR = $(prefix)/man/man$(manext)#	where to install man pages
INSTALLEDBIN = $(BINDIR)/funzip$E $(BINDIR)/zipinfo$E $(BINDIR)/unzipsfx$E \
	$(BINDIR)/unzip$E
INSTALLEDMAN = $(MANDIR)/unzip.$(manext) $(MANDIR)/funzip.$(manext) \
	$(MANDIR)/unzipsfx.$(manext) $(MANDIR)/zipinfo.$(manext)
#
UNZIPS = unzip$E funzip$E unzipsfx$E zipinfo$E
# this is a little ugly...well, no, it's a lot ugly:
MANS = unix/unzip.1 unix/unzipsfx.1 unix/zipinfo.1 unix/funzip.1
DOCS = unzip.doc unzipsfx.doc zipinfo.doc funzip.doc

###############################################
# BASIC COMPILE INSTRUCTIONS AND DEPENDENCIES #
###############################################

# this is for GNU make; comment out and notify zip-bugs if it causes errors
.SUFFIXES:	.c .o

# default for compiling C files
.c.o:
	$(CC) -c $(CF) $*.c


unzips:		$(UNZIPS)
objs:		$(OBJS)
objsdll:	$(OBJSDLL)
docs:		$(DOCS)
unzipsman:	unzips docs
unzipsdocs:	unzips docs


unzip$E:	$(OBJS)
	$(LD) $(LF) $(LOBJS) $(LF2)

unzipsfx$E:	$(OBJX)
	$(LD) $(SL) $(LOBJX) $(SL2)

funzip$E:	$(OBJF)
	$(LD) $(FL) $(OBJF) $(FL2)

zipinfo$E:	unzip$E
	$(LN) unzip$E zipinfo$E


crc32$O:	crc32.c $(UNZIP_H) zip.h
crctab$O:	crctab.c $(UNZIP_H) zip.h
crypt$O:	crypt.c $(UNZIP_H) zip.h crypt.h ttyio.h
envargs$O:	envargs.c $(UNZIP_H)
explode$O:	explode.c $(UNZIP_H)
extract$O:	extract.c $(UNZIP_H) crypt.h
fileio$O:	fileio.c $(UNZIP_H) crypt.h ttyio.h ebcdic.h
funzip$O:	funzip.c $(UNZIP_H) crypt.h ttyio.h tables.h
globals$O:	globals.c $(UNZIP_H)
inflate$O:	inflate.c inflate.h $(UNZIP_H)
list$O:		list.c $(UNZIP_H)
match$O:	match.c $(UNZIP_H)
process$O:	process.c $(UNZIP_H)
ttyio$O:	ttyio.c $(UNZIP_H) zip.h crypt.h ttyio.h
unreduce$O:	unreduce.c $(UNZIP_H)
unshrink$O:	unshrink.c $(UNZIP_H)
unzip$O:	unzip.c $(UNZIP_H) crypt.h version.h consts.h
zipinfo$O:	zipinfo.c $(UNZIP_H)

crypt_$O:	crypt.c $(UNZIP_H) zip.h crypt.h ttyio.h	# funzip only
	$(CP) crypt.c crypt_.c
	$(CC) -c $(CF) -DFUNZIP crypt_.c
	$(RM) crypt_.c

extract_$O:	extract.c $(UNZIP_H) crypt.h			# unzipsfx only
	$(CP) extract.c extract_.c
	$(CC) -c $(CF) -DSFX extract_.c
	$(RM) extract_.c

globals_$O:	globals.c $(UNZIP_H)				# funzip only
	$(CP) globals.c globals_.c
	$(CC) -c $(CF) -DFUNZIP globals_.c
	$(RM) globals_.c

inflate_$O:	inflate.c inflate.h $(UNZIP_H) crypt.h		# funzip only
	$(CP) inflate.c inflate_.c
	$(CC) -c $(CF) -DFUNZIP inflate_.c
	$(RM) inflate_.c

ttyio_$O:	ttyio.c $(UNZIP_H) zip.h crypt.h ttyio.h	# funzip only
	$(CP) ttyio.c ttyio_.c
	$(CC) -c $(CF) -DFUNZIP ttyio_.c
	$(RM) ttyio_.c

process_$O:	process.c $(UNZIP_H)				# unzipsfx only
	$(CP) process.c process_.c
	$(CC) -c $(CF) -DSFX process_.c
	$(RM) process_.c

beos$O:		beos/beos.c $(UNZIP_H) version.h		# BeOS only
	$(CC) -c $(CF) beos/beos.c

# version() not used by unzipsfx, so no version.h dependency
beos_$O:		beos/beos.c $(UNZIP_H)				# unzipsfx only
	$(CP) beos/beos.c beos_.c
	$(CC) -c $(CF) -Ibeos -DSFX beos_.c
	$(RM) beos_.c

unzipsfx$O:	unzip.c $(UNZIP_H) crypt.h version.h consts.h	# unzipsfx only
	$(CP) unzip.c unzipsfx.c
	$(CC) -c $(CF) -DSFX unzipsfx.c
	$(RM) unzipsfx.c


# this really only works for Unix targets, unless E and O specified on cmd line
clean:
	-rm -f $(UNZIPS) $(OBJS) $(OBJF) $(OBJX) api$O apihelp$O crc_gcc$O \
	  unzipstb$O

install:	$(UNZIPS) $(MANS)
	$(INSTALL) -m 755 $(UNZIPS) $(BINDIR)
	$(RM) $(BINDIR)/zipinfo$E
	$(LN) unzip$E $(BINDIR)/zipinfo$E
	$(RM) $(BINDIR)/zipgrep$E
	$(INSTALL) -m 755 unix/zipgrep $(BINDIR)/zipgrep$E
	$(INSTALL) -m 644 unix/unzip.1 $(MANDIR)/unzip.$(manext)
	$(INSTALL) -m 644 unix/unzipsfx.1 $(MANDIR)/unzipsfx.$(manext)
	$(INSTALL) -m 644 unix/zipinfo.1 $(MANDIR)/zipinfo.$(manext)
	$(INSTALL) -m 644 unix/funzip.1 $(MANDIR)/funzip.$(manext)
	$(INSTALL) -m 644 $(DOCS) $(MANDIR)

# alternatively, could use zip method:  -cd $(BINDIR); rm -f $(UNZIPS)  [etc.]
uninstall:
	rm -f $(INSTALLEDBIN) $(INSTALLEDMAN)


TESTZIP = testmake.zip	# the test zipfile

# test some basic features of the build
test:		check

check:	unzips
	@echo '  This is a Unix-specific target.  (Just so you know.)'
	@echo '  (Should work ok on BeOS... [cjh])'
	if test ! -f $(TESTZIP); then \
		echo "  error:  can't find test file $(TESTZIP)"; exit 1; fi
#
	echo "  testing extraction"
	./unzip -b $(TESTZIP) testmake.zipinfo
	if test $? ; then \
	    echo "  error:  file extraction from $(TESTZIP) failed"; exit 1; fi
#
	echo '  testing zipinfo (unzip -Z)'
	./unzip -Z $(TESTZIP) > testmake.unzip-Z
	if diff testmake.unzip-Z testmake.zipinfo; then ;; else \
	    echo '  error:  zipinfo output doesn't match stored version'; fi
	$(RM) testmake.unzip-Z testmake.zipinfo
#
	echo '  testing unzip -d exdir option'
	./unzip -b $(TESTZIP) -d testun
	cat testun/notes
#
	echo '  testing unzip -o and funzip (ignore funzip warning)'
	./unzip -boq $(TESTZIP) notes -d testun
	./funzip < $(TESTZIP) > testun/notes2
	if diff testun/notes testun/notes2; then ;; else \
	    echo 'error:  funzip output disagrees with unzip'; fi
#
	echo '  testing unzipsfx (self-extractor)'
	cat unzipsfx $(TESTZIP) > testsfx
	$(CHMOD) 0700 testsfx
	./testsfx -b notes
	if diff notes testun/notes; then ;; else \
	    echo '  error:  unzipsfx file disagrees with unzip'; fi
	$(RM) testsfx notes testun/notes testun/notes2
	rmdir testun
#
	echo '  testing complete.'

######################################################################
# Make rules for the supported compilers

gnu: gcc

gnuc: gcc

gcc:
	@echo 'Making with GNU C...'
	@echo ''
	$(MAKE) $(UNZIPS) CC=$(GNU_CC) CF="$(GNU_CF)" LF="$(GNU_LF)" \
			LF2="$(GNU_LF2)"

mwcc:
	@echo 'Making with Metrowerks CodeWarrior...'
	@echo ''
	$(MAKE) $(UNZIPS) CC=$(MW_CC) CF="$(MW_CF)" LF="$(MW_LF)" \
			LF2="$(MW_LF2)"

foo: list
	@echo 'I was kidding about the "foo" compiler.'
	@echo ''
