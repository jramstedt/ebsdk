#
# Borland C++ IDE generated makefile
# Generated 8/12/97 at 9:37:41 AM 
#
.AUTODEPEND


#
# Borland C++ tools
#
IMPLIB  = Implib
BCC32   = Bcc32 +BccW32.cfg 
BCC32I  = Bcc32i +BccW32.cfg 
TLINK32 = TLink32
ILINK32 = Ilink32
TLIB    = TLib
BRC32   = Brc32
TASM32  = Tasm32
#
# IDE macros
#


#
# Options
#
IDE_LinkFLAGS32 =  -LC:\BC5\LIB
IDE_ResFLAGS32 = 
LinkerLocalOptsAtW32_DcbZIPbEXE32bzip32dlib =  -Tpd -aa -V4.0 -c -v
ResLocalOptsAtW32_DcbZIPbEXE32bzip32dlib = 
BLocalOptsAtW32_DcbZIPbEXE32bzip32dlib = 
CompInheritOptsAt_DcbZIPbEXE32bzip32dlib = -IC:\BC5\INCLUDE;D:\ZIP;D:\ZIP\WINDLL;D:\ZIP\WIN32 -DNO_ASM;WINDLL;MSDOS;WIN32;USE_EF_UT_TIME;USE_ZIPMAIN;
LinkerInheritOptsAt_DcbZIPbEXE32bzip32dlib = -x
LinkerOptsAt_DcbZIPbEXE32bzip32dlib = $(LinkerLocalOptsAtW32_DcbZIPbEXE32bzip32dlib)
ResOptsAt_DcbZIPbEXE32bzip32dlib = $(ResLocalOptsAtW32_DcbZIPbEXE32bzip32dlib)
BOptsAt_DcbZIPbEXE32bzip32dlib = $(BLocalOptsAtW32_DcbZIPbEXE32bzip32dlib)

#
# Dependency List
#
Dep_windll32 = \
   D:\ZIP\EXE32\zip32.lib

windll32 : BccW32.cfg $(Dep_windll32)
  echo MakeNode

D:\ZIP\EXE32\zip32.lib : zip32.dll
  $(IMPLIB) $@ zip32.dll


Dep_zip32ddll = \
   D:\ZIP\ZIPOBJ32\nt.obj\
   D:\ZIP\ZIPOBJ32\api.obj\
   D:\ZIP\ZIPOBJ32\windll.res\
   D:\ZIP\ZIPOBJ32\windll.obj\
   zip\windll\windll32.def\
   D:\ZIP\ZIPOBJ32\win32.obj\
   D:\ZIP\ZIPOBJ32\win32zip.obj\
   D:\ZIP\ZIPOBJ32\bits.obj\
   D:\ZIP\ZIPOBJ32\zipfile.obj\
   D:\ZIP\ZIPOBJ32\zip.obj\
   D:\ZIP\ZIPOBJ32\util.obj\
   D:\ZIP\ZIPOBJ32\ttyio.obj\
   D:\ZIP\ZIPOBJ32\trees.obj\
   D:\ZIP\ZIPOBJ32\globals.obj\
   D:\ZIP\ZIPOBJ32\fileio.obj\
   D:\ZIP\ZIPOBJ32\deflate.obj\
   D:\ZIP\ZIPOBJ32\crypt.obj\
   D:\ZIP\ZIPOBJ32\crctab.obj\
   D:\ZIP\ZIPOBJ32\crc32.obj\
   D:\ZIP\ZIPOBJ32\zipup.obj

zip32.dll : $(Dep_zip32ddll)
  $(ILINK32) @&&|
 /v $(IDE_LinkFLAGS32) $(LinkerOptsAt_DcbZIPbEXE32bzip32dlib) $(LinkerInheritOptsAt_DcbZIPbEXE32bzip32dlib) +
C:\BC5\LIB\c0d32.obj+
D:\ZIP\ZIPOBJ32\nt.obj+
D:\ZIP\ZIPOBJ32\api.obj+
D:\ZIP\ZIPOBJ32\windll.obj+
D:\ZIP\ZIPOBJ32\win32.obj+
D:\ZIP\ZIPOBJ32\win32zip.obj+
D:\ZIP\ZIPOBJ32\bits.obj+
D:\ZIP\ZIPOBJ32\zipfile.obj+
D:\ZIP\ZIPOBJ32\zip.obj+
D:\ZIP\ZIPOBJ32\util.obj+
D:\ZIP\ZIPOBJ32\ttyio.obj+
D:\ZIP\ZIPOBJ32\trees.obj+
D:\ZIP\ZIPOBJ32\globals.obj+
D:\ZIP\ZIPOBJ32\fileio.obj+
D:\ZIP\ZIPOBJ32\deflate.obj+
D:\ZIP\ZIPOBJ32\crypt.obj+
D:\ZIP\ZIPOBJ32\crctab.obj+
D:\ZIP\ZIPOBJ32\crc32.obj+
D:\ZIP\ZIPOBJ32\zipup.obj
$<,$*
C:\BC5\LIB\import32.lib+
C:\BC5\LIB\cw32.lib
zip\windll\windll32.def
D:\ZIP\ZIPOBJ32\windll.res

|
D:\ZIP\ZIPOBJ32\nt.obj :  zip\win32\nt.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbZIPbEXE32bzip32dlib) $(CompInheritOptsAt_DcbZIPbEXE32bzip32dlib) -o$@ zip\win32\nt.c
|

D:\ZIP\ZIPOBJ32\api.obj :  zip\api.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbZIPbEXE32bzip32dlib) $(CompInheritOptsAt_DcbZIPbEXE32bzip32dlib) -o$@ zip\api.c
|

D:\ZIP\ZIPOBJ32\windll.res :  zip\windll\windll.rc
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_DcbZIPbEXE32bzip32dlib) $(CompInheritOptsAt_DcbZIPbEXE32bzip32dlib)  -FO$@ zip\windll\windll.rc
|
D:\ZIP\ZIPOBJ32\windll.obj :  zip\windll\windll.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbZIPbEXE32bzip32dlib) $(CompInheritOptsAt_DcbZIPbEXE32bzip32dlib) -o$@ zip\windll\windll.c
|

D:\ZIP\ZIPOBJ32\win32.obj :  zip\win32\win32.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbZIPbEXE32bzip32dlib) $(CompInheritOptsAt_DcbZIPbEXE32bzip32dlib) -o$@ zip\win32\win32.c
|

D:\ZIP\ZIPOBJ32\win32zip.obj :  zip\win32\win32zip.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbZIPbEXE32bzip32dlib) $(CompInheritOptsAt_DcbZIPbEXE32bzip32dlib) -o$@ zip\win32\win32zip.c
|

D:\ZIP\ZIPOBJ32\bits.obj :  zip\bits.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbZIPbEXE32bzip32dlib) $(CompInheritOptsAt_DcbZIPbEXE32bzip32dlib) -o$@ zip\bits.c
|

D:\ZIP\ZIPOBJ32\zipfile.obj :  zip\zipfile.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbZIPbEXE32bzip32dlib) $(CompInheritOptsAt_DcbZIPbEXE32bzip32dlib) -o$@ zip\zipfile.c
|

D:\ZIP\ZIPOBJ32\zip.obj :  zip\zip.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbZIPbEXE32bzip32dlib) $(CompInheritOptsAt_DcbZIPbEXE32bzip32dlib) -o$@ zip\zip.c
|

D:\ZIP\ZIPOBJ32\util.obj :  zip\util.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbZIPbEXE32bzip32dlib) $(CompInheritOptsAt_DcbZIPbEXE32bzip32dlib) -o$@ zip\util.c
|

D:\ZIP\ZIPOBJ32\ttyio.obj :  zip\ttyio.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbZIPbEXE32bzip32dlib) $(CompInheritOptsAt_DcbZIPbEXE32bzip32dlib) -o$@ zip\ttyio.c
|

D:\ZIP\ZIPOBJ32\trees.obj :  zip\trees.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbZIPbEXE32bzip32dlib) $(CompInheritOptsAt_DcbZIPbEXE32bzip32dlib) -o$@ zip\trees.c
|

D:\ZIP\ZIPOBJ32\globals.obj :  zip\globals.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbZIPbEXE32bzip32dlib) $(CompInheritOptsAt_DcbZIPbEXE32bzip32dlib) -o$@ zip\globals.c
|

D:\ZIP\ZIPOBJ32\fileio.obj :  zip\fileio.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbZIPbEXE32bzip32dlib) $(CompInheritOptsAt_DcbZIPbEXE32bzip32dlib) -o$@ zip\fileio.c
|

D:\ZIP\ZIPOBJ32\deflate.obj :  zip\deflate.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbZIPbEXE32bzip32dlib) $(CompInheritOptsAt_DcbZIPbEXE32bzip32dlib) -o$@ zip\deflate.c
|

D:\ZIP\ZIPOBJ32\crypt.obj :  zip\crypt.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbZIPbEXE32bzip32dlib) $(CompInheritOptsAt_DcbZIPbEXE32bzip32dlib) -o$@ zip\crypt.c
|

D:\ZIP\ZIPOBJ32\crctab.obj :  zip\crctab.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbZIPbEXE32bzip32dlib) $(CompInheritOptsAt_DcbZIPbEXE32bzip32dlib) -o$@ zip\crctab.c
|

D:\ZIP\ZIPOBJ32\crc32.obj :  zip\crc32.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbZIPbEXE32bzip32dlib) $(CompInheritOptsAt_DcbZIPbEXE32bzip32dlib) -o$@ zip\crc32.c
|

D:\ZIP\ZIPOBJ32\zipup.obj :  zip\zipup.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbZIPbEXE32bzip32dlib) $(CompInheritOptsAt_DcbZIPbEXE32bzip32dlib) -o$@ zip\zipup.c
|

# Compiler configuration file
BccW32.cfg : 
   Copy &&|
-w
-R
-v
-WM-
-vi
-H
-H=wiz32all.csm
-f-
-ff-
-d
-wucp
-w-obs
-H-
-WD
-wcln
-w-sig
-wdef
-wnod
-wuse
-wstv
-wobs
-d
-H-
| $@


