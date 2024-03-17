#
# Borland C++ IDE generated makefile
# Generated 8/12/97 at 9:34:00 AM 
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
LinkerLocalOptsAtC32_DcbZIPbLIBSbzip32dlib =  -v -Tpe -ap -c
ResLocalOptsAtC32_DcbZIPbLIBSbzip32dlib = 
BLocalOptsAtC32_DcbZIPbLIBSbzip32dlib = 
CompInheritOptsAt_DcbZIPbLIBSbzip32dlib = -IC:\BC5\INCLUDE;D:\ZIP;D:\ZIP\WINDLL;D:\ZIP\WIN32 -DNO_ASM;WINDLL;MSDOS;WIN32;USE_EF_UT_TIME;USE_ZIPMAIN;ZIPLIB;
LinkerInheritOptsAt_DcbZIPbLIBSbzip32dlib = -x
LinkerOptsAt_DcbZIPbLIBSbzip32dlib = $(LinkerLocalOptsAtC32_DcbZIPbLIBSbzip32dlib)
ResOptsAt_DcbZIPbLIBSbzip32dlib = $(ResLocalOptsAtC32_DcbZIPbLIBSbzip32dlib)
BOptsAt_DcbZIPbLIBSbzip32dlib = $(BLocalOptsAtC32_DcbZIPbLIBSbzip32dlib)

#
# Dependency List
#
Dep_ziplib32 = \
   D:\ZIP\LIBS\zip32.lib

ziplib32 : BccW32.cfg $(Dep_ziplib32)
  echo MakeNode

Dep_DcbZIPbLIBSbzip32dlib = \
   D:\ZIP\ZPLIBOBJ\nt.obj\
   D:\ZIP\ZPLIBOBJ\api.obj\
   D:\ZIP\ZPLIBOBJ\windll.res\
   D:\ZIP\ZPLIBOBJ\windll.obj\
   zip\windll\ziplib.def\
   D:\ZIP\ZPLIBOBJ\win32.obj\
   D:\ZIP\ZPLIBOBJ\win32zip.obj\
   D:\ZIP\ZPLIBOBJ\bits.obj\
   D:\ZIP\ZPLIBOBJ\zipfile.obj\
   D:\ZIP\ZPLIBOBJ\zip.obj\
   D:\ZIP\ZPLIBOBJ\util.obj\
   D:\ZIP\ZPLIBOBJ\ttyio.obj\
   D:\ZIP\ZPLIBOBJ\trees.obj\
   D:\ZIP\ZPLIBOBJ\globals.obj\
   D:\ZIP\ZPLIBOBJ\fileio.obj\
   D:\ZIP\ZPLIBOBJ\deflate.obj\
   D:\ZIP\ZPLIBOBJ\crypt.obj\
   D:\ZIP\ZPLIBOBJ\crctab.obj\
   D:\ZIP\ZPLIBOBJ\crc32.obj\
   D:\ZIP\ZPLIBOBJ\zipup.obj\
   zip32.lib

D:\ZIP\LIBS\zip32.lib : $(Dep_DcbZIPbLIBSbzip32dlib)
  $(TLIB) $< $(IDE_BFLAGS) $(BOptsAt_DcbZIPbLIBSbzip32dlib) @&&|
 -+D:\ZIP\ZPLIBOBJ\nt.obj &
-+D:\ZIP\ZPLIBOBJ\api.obj &
-+D:\ZIP\ZPLIBOBJ\windll.obj &
-+D:\ZIP\ZPLIBOBJ\win32.obj &
-+D:\ZIP\ZPLIBOBJ\win32zip.obj &
-+D:\ZIP\ZPLIBOBJ\bits.obj &
-+D:\ZIP\ZPLIBOBJ\zipfile.obj &
-+D:\ZIP\ZPLIBOBJ\zip.obj &
-+D:\ZIP\ZPLIBOBJ\util.obj &
-+D:\ZIP\ZPLIBOBJ\ttyio.obj &
-+D:\ZIP\ZPLIBOBJ\trees.obj &
-+D:\ZIP\ZPLIBOBJ\globals.obj &
-+D:\ZIP\ZPLIBOBJ\fileio.obj &
-+D:\ZIP\ZPLIBOBJ\deflate.obj &
-+D:\ZIP\ZPLIBOBJ\crypt.obj &
-+D:\ZIP\ZPLIBOBJ\crctab.obj &
-+D:\ZIP\ZPLIBOBJ\crc32.obj &
-+D:\ZIP\ZPLIBOBJ\zipup.obj
|

D:\ZIP\ZPLIBOBJ\nt.obj :  zip\win32\nt.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbZIPbLIBSbzip32dlib) $(CompInheritOptsAt_DcbZIPbLIBSbzip32dlib) -o$@ zip\win32\nt.c
|

D:\ZIP\ZPLIBOBJ\api.obj :  zip\api.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbZIPbLIBSbzip32dlib) $(CompInheritOptsAt_DcbZIPbLIBSbzip32dlib) -o$@ zip\api.c
|

D:\ZIP\ZPLIBOBJ\windll.res :  zip\windll\windll.rc
  $(BRC) -R @&&|
 $(IDE_ResFLAGS) $(ROptsAt_DcbZIPbLIBSbzip32dlib) $(CompInheritOptsAt_DcbZIPbLIBSbzip32dlib)  -FO$@ zip\windll\windll.rc
|
D:\ZIP\ZPLIBOBJ\windll.obj :  zip\windll\windll.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbZIPbLIBSbzip32dlib) $(CompInheritOptsAt_DcbZIPbLIBSbzip32dlib) -o$@ zip\windll\windll.c
|

D:\ZIP\ZPLIBOBJ\win32.obj :  zip\win32\win32.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbZIPbLIBSbzip32dlib) $(CompInheritOptsAt_DcbZIPbLIBSbzip32dlib) -o$@ zip\win32\win32.c
|

D:\ZIP\ZPLIBOBJ\win32zip.obj :  zip\win32\win32zip.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbZIPbLIBSbzip32dlib) $(CompInheritOptsAt_DcbZIPbLIBSbzip32dlib) -o$@ zip\win32\win32zip.c
|

D:\ZIP\ZPLIBOBJ\bits.obj :  zip\bits.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbZIPbLIBSbzip32dlib) $(CompInheritOptsAt_DcbZIPbLIBSbzip32dlib) -o$@ zip\bits.c
|

D:\ZIP\ZPLIBOBJ\zipfile.obj :  zip\zipfile.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbZIPbLIBSbzip32dlib) $(CompInheritOptsAt_DcbZIPbLIBSbzip32dlib) -o$@ zip\zipfile.c
|

D:\ZIP\ZPLIBOBJ\zip.obj :  zip\zip.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbZIPbLIBSbzip32dlib) $(CompInheritOptsAt_DcbZIPbLIBSbzip32dlib) -o$@ zip\zip.c
|

D:\ZIP\ZPLIBOBJ\util.obj :  zip\util.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbZIPbLIBSbzip32dlib) $(CompInheritOptsAt_DcbZIPbLIBSbzip32dlib) -o$@ zip\util.c
|

D:\ZIP\ZPLIBOBJ\ttyio.obj :  zip\ttyio.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbZIPbLIBSbzip32dlib) $(CompInheritOptsAt_DcbZIPbLIBSbzip32dlib) -o$@ zip\ttyio.c
|

D:\ZIP\ZPLIBOBJ\trees.obj :  zip\trees.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbZIPbLIBSbzip32dlib) $(CompInheritOptsAt_DcbZIPbLIBSbzip32dlib) -o$@ zip\trees.c
|

D:\ZIP\ZPLIBOBJ\globals.obj :  zip\globals.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbZIPbLIBSbzip32dlib) $(CompInheritOptsAt_DcbZIPbLIBSbzip32dlib) -o$@ zip\globals.c
|

D:\ZIP\ZPLIBOBJ\fileio.obj :  zip\fileio.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbZIPbLIBSbzip32dlib) $(CompInheritOptsAt_DcbZIPbLIBSbzip32dlib) -o$@ zip\fileio.c
|

D:\ZIP\ZPLIBOBJ\deflate.obj :  zip\deflate.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbZIPbLIBSbzip32dlib) $(CompInheritOptsAt_DcbZIPbLIBSbzip32dlib) -o$@ zip\deflate.c
|

D:\ZIP\ZPLIBOBJ\crypt.obj :  zip\crypt.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbZIPbLIBSbzip32dlib) $(CompInheritOptsAt_DcbZIPbLIBSbzip32dlib) -o$@ zip\crypt.c
|

D:\ZIP\ZPLIBOBJ\crctab.obj :  zip\crctab.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbZIPbLIBSbzip32dlib) $(CompInheritOptsAt_DcbZIPbLIBSbzip32dlib) -o$@ zip\crctab.c
|

D:\ZIP\ZPLIBOBJ\crc32.obj :  zip\crc32.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbZIPbLIBSbzip32dlib) $(CompInheritOptsAt_DcbZIPbLIBSbzip32dlib) -o$@ zip\crc32.c
|

D:\ZIP\ZPLIBOBJ\zipup.obj :  zip\zipup.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbZIPbLIBSbzip32dlib) $(CompInheritOptsAt_DcbZIPbLIBSbzip32dlib) -o$@ zip\zipup.c
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
-wcln
-w-sig
-wdef
-wnod
-wuse
-wstv
-wobs
-d
-WC
-H-
| $@


