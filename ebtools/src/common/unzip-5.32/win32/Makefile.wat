# WMAKE makefile for Windows 95 and Windows NT (Intel only)
# using Watcom C/C++ v11.0+, by Paul Kienitz, last revised 29 Sep 97.
# Makes UnZip.exe, fUnZip.exe, and UnZipSFX.exe.
#
# Invoke from UnZip source dir with "WMAKE -F WIN32\MAKEFILE.WAT [targets]"
# To build with debug info use "WMAKE DEBUG=1 ..."
# To build with no assembly modules use "WMAKE NOASM=1 ..."
# To support unshrinking and unreducing use "WMAKE LAWSUIT=1 ..."
#
# Other options to be fed to the compiler can be specified in an environment
# variable called LOCAL_UNZIP.

variation = $(%LOCAL_UNZIP)

# Stifle annoying "Delete this file?" questions when errors occur:
.ERASE

.EXTENSIONS:
.EXTENSIONS: .exe .obj .obx .c .h .asm

# We maintain multiple sets of object files in different directories so that
# we can compile msdos, dos/4gw or pmode/w, and win32 versions of UnZip without
# their object files interacting.  The following var must be a directory name
# ending with a backslash.  All object file names must include this macro
# at the beginning, for example "$(O)foo.obj".

!ifdef DEBUG
O = od32w\  # comment here so backslash won't continue the line
!else
O = ob32w\  # likewise
!endif

!ifdef LAWSUIT
cvars = $+$(cvars)$- -DUSE_SMITH_CODE -DUSE_UNSHRINK
avars = $+$(avars)$- -DUSE_SMITH_CODE -DUSE_UNSHRINK
# "$+$(foo)$-" means expand foo as it has been defined up to now; normally,
# this Make defers inner expansion until the outer macro is expanded.
!endif

# The assembly hot-spot code in crc_i386.asm is optional.  This section
# controls its usage.

!ifdef NOASM
crcob = $(O)crc32.obj
!else   # !NOASM
cvars = $+$(cvars)$- -DASM_CRC
crcob = $(O)crc_i386.obj
!endif

# Our object files.  OBJS is for UnZip, OBJX for UnZipSFX, OBJF for fUnZip:

OBJS1 = $(O)unzip.obj $(crcob) $(O)crctab.obj $(O)crypt.obj $(O)envargs.obj
OBJS2 = $(O)explode.obj $(O)extract.obj $(O)fileio.obj $(O)globals.obj
OBJS3 = $(O)inflate.obj $(O)list.obj $(O)match.obj $(O)process.obj
OBJS4 = $(O)ttyio.obj $(O)unreduce.obj $(O)unshrink.obj $(O)zipinfo.obj
OBJS  = $(OBJS1) $(OBJS2) $(OBJS3) $(OBJS4) $(O)win32.obj $(O)nt.obj

OBJX1 = $(O)unzip.obx $(crcob) $(O)crctab.obx $(O)crypt.obx $(O)extract.obx
OBJX2 = $(O)fileio.obx $(O)globals.obx $(O)inflate.obx $(O)match.obx
OBJX3 = $(O)process.obx $(O)ttyio.obx
OBJX  = $(OBJX1) $(OBJX2) $(OBJX3) $(O)win32.obx $(O)nt.obx

OBJF1 = $(O)funzip.obj $(crcob) $(O)cryptf.obj $(O)globalsf.obj
OBJF  = $(OBJF1) $(O)inflatef.obj $(O)ttyiof.obj

UNZIP_H = unzip.h unzpriv.h globals.h win32\w32cfg.h

# Now we have to pick out the proper compiler and options for it.

cc     = wcc386
link   = wlink
asm    = wasm
# Use Pentium Pro timings, register args, static strings in code, high strictness:
cflags = -bt=NT -6r -zt -zq -wx
aflags = -bt=NT -mf -3 -zq
lflags = sys NT
cvars  = $+$(cvars)$- -DWIN32 $(variation)
avars  = $+$(avars)$- $(variation)

# Specify optimizations, or a nonoptimized debugging version:

!ifdef DEBUG
cdebug = -od -d2
cdebux = -od -d2
ldebug = d w all op symf
!else
cdebug = -s -obhikl+rt -oe=100 -zp8
cdebux = -s -obhiklrs
# -oa helps slightly but might be dangerous.
ldebug = op el
!endif

# How to compile sources:
.c.obx:
	$(cc) $(cdebux) $(cflags) $(cvars) -DSFX $[@ -fo=$@

.c.obj:
	$(cc) $(cdebug) $(cflags) $(cvars) $[@ -fo=$@

# Here we go!  By default, make all targets:
all: UnZip.exe fUnZip.exe UnZipSFX.exe

# Convenient shorthand options for single targets:
u:   UnZip.exe     .SYMBOLIC
f:   fUnZip.exe    .SYMBOLIC
x:   UnZipSFX.exe  .SYMBOLIC

UnZip.exe:	$(OBJS)
	$(link) $(lflags) $(ldebug) name $@ file {$(OBJS)}

UnZipSFX.exe:	$(OBJX)
	$(link) $(lflags) $(ldebug) name $@ file {$(OBJX)}

fUnZip.exe:	$(OBJF)
	$(link) $(lflags) $(ldebug) name $@ file {$(OBJF)}

# Source dependencies:

#       generic (UnZip, fUnZip):

$(O)crc32.obj:    crc32.c $(UNZIP_H) zip.h
$(O)crctab.obj:   crctab.c $(UNZIP_H) zip.h
$(O)crypt.obj:    crypt.c $(UNZIP_H) zip.h crypt.h ttyio.h
$(O)envargs.obj:  envargs.c $(UNZIP_H)
$(O)explode.obj:  explode.c $(UNZIP_H)
$(O)extract.obj:  extract.c $(UNZIP_H) crypt.h
$(O)fileio.obj:   fileio.c $(UNZIP_H) crypt.h ttyio.h ebcdic.h
$(O)funzip.obj:   funzip.c $(UNZIP_H) crypt.h ttyio.h tables.h
$(O)globals.obj:  globals.c $(UNZIP_H)
$(O)inflate.obj:  inflate.c inflate.h $(UNZIP_H)
$(O)list.obj:     list.c $(UNZIP_H)
$(O)match.obj:    match.c $(UNZIP_H)
$(O)process.obj:  process.c $(UNZIP_H)
$(O)ttyio.obj:    ttyio.c $(UNZIP_H) zip.h crypt.h ttyio.h
$(O)unreduce.obj: unreduce.c $(UNZIP_H)
$(O)unshrink.obj: unshrink.c $(UNZIP_H)
$(O)unzip.obj:    unzip.c $(UNZIP_H) crypt.h version.h consts.h
$(O)zipinfo.obj:  zipinfo.c $(UNZIP_H)

#       UnZipSFX variants:

$(O)crc32.obx:    crc32.c $(UNZIP_H) zip.h
$(O)crctab.obx:   crctab.c $(UNZIP_H) zip.h
$(O)crypt.obx:    crypt.c $(UNZIP_H) zip.h crypt.h ttyio.h
$(O)extract.obx:  extract.c $(UNZIP_H) crypt.h
$(O)fileio.obx:   fileio.c $(UNZIP_H) crypt.h ttyio.h ebcdic.h
$(O)globals.obx:  globals.c $(UNZIP_H)
$(O)inflate.obx:  inflate.c inflate.h $(UNZIP_H)
$(O)match.obx:    match.c $(UNZIP_H)
$(O)process.obx:  process.c $(UNZIP_H)
$(O)ttyio.obx:    ttyio.c $(UNZIP_H) zip.h crypt.h ttyio.h
$(O)unzip.obx:    unzip.c $(UNZIP_H) crypt.h version.h consts.h

# Special case object files:

$(O)win32.obj:    win32\win32.c $(UNZIP_H)
	$(cc) $(cdebug) $(cflags) $(cvars) win32\win32.c -fo=$@

$(O)win32.obx:    win32\win32.c $(UNZIP_H)
	$(cc) $(cdebux) $(cflags) $(cvars) -DSFX win32\win32.c -fo=$@

$(O)nt.obj:    win32\nt.c $(UNZIP_H) win32\nt.h
	$(cc) $(cdebug) $(cflags) $(cvars) win32\nt.c -fo=$@

$(O)nt.obx:    win32\nt.c $(UNZIP_H) win32\nt.h
	$(cc) $(cdebux) $(cflags) $(cvars) -DSFX win32\nt.c -fo=$@

$(O)crc_i386.obj: win32\crc_i386.asm
	$(asm) $(aflags) $(avars) win32\crc_i386.asm -fo=$@

# Variant object files for fUnZip:

$(O)cryptf.obj:   crypt.c $(UNZIP_H) zip.h crypt.h ttyio.h
	$(cc) $(cdebug) $(cflags) $(cvars) -DFUNZIP crypt.c -fo=$@

$(O)globalsf.obj: globals.c $(UNZIP_H)
	$(cc) $(cdebug) $(cflags) $(cvars) -DFUNZIP globals.c -fo=$@

$(O)inflatef.obj: inflate.c inflate.h $(UNZIP_H) crypt.h
	$(cc) $(cdebug) $(cflags) $(cvars) -DFUNZIP inflate.c -fo=$@

$(O)ttyiof.obj:   ttyio.c $(UNZIP_H) zip.h crypt.h ttyio.h
	$(cc) $(cdebug) $(cflags) $(cvars) -DFUNZIP ttyio.c -fo=$@

# Unwanted file removal:

clean:     .SYMBOLIC
	del $(O)*.ob?

cleaner:   clean  .SYMBOLIC
	del UnZip.exe
	del fUnZip.exe
	del UnZipSFX.exe

