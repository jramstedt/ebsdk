#
# Borland C++ IDE generated makefile
# Generated 8/12/97 at 9:30:15 AM 
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
LinkerLocalOptsAtC32_DcbUNZIPbLIBSbunzip32dlib =  -v -Tpe -ap -c
ResLocalOptsAtC32_DcbUNZIPbLIBSbunzip32dlib = 
BLocalOptsAtC32_DcbUNZIPbLIBSbunzip32dlib = 
CompInheritOptsAt_DcbUNZIPbLIBSbunzip32dlib = -IC:\BC5\INCLUDE;D:\UNZIP;D:\UNZIP\WINDLL;D:\UNZIP\WIN32 -DWINDLL;USE_EF_UT_TIME;REENTRANT;DLL;WIN32;UNZIPLIB;
LinkerInheritOptsAt_DcbUNZIPbLIBSbunzip32dlib = -x
LinkerOptsAt_DcbUNZIPbLIBSbunzip32dlib = $(LinkerLocalOptsAtC32_DcbUNZIPbLIBSbunzip32dlib)
ResOptsAt_DcbUNZIPbLIBSbunzip32dlib = $(ResLocalOptsAtC32_DcbUNZIPbLIBSbunzip32dlib)
BOptsAt_DcbUNZIPbLIBSbunzip32dlib = $(BLocalOptsAtC32_DcbUNZIPbLIBSbunzip32dlib)

#
# Dependency List
#
Dep_unzlib32 = \
   D:\UNZIP\LIBS\unzip32.lib

unzlib32 : BccW32.cfg $(Dep_unzlib32)
  echo MakeNode

Dep_DcbUNZIPbLIBSbunzip32dlib = \
   D:\UNZIP\UZLIBOBJ\nt.obj\
   D:\UNZIP\UZLIBOBJ\api.obj\
   unzip\windll\unziplib.def\
   D:\UNZIP\UZLIBOBJ\windll.res\
   D:\UNZIP\UZLIBOBJ\windll.obj\
   D:\UNZIP\UZLIBOBJ\win32.obj\
   D:\UNZIP\UZLIBOBJ\crc32.obj\
   D:\UNZIP\UZLIBOBJ\unshrink.obj\
   D:\UNZIP\UZLIBOBJ\unreduce.obj\
   D:\UNZIP\UZLIBOBJ\ttyio.obj\
   D:\UNZIP\UZLIBOBJ\process.obj\
   D:\UNZIP\UZLIBOBJ\match.obj\
   D:\UNZIP\UZLIBOBJ\list.obj\
   D:\UNZIP\UZLIBOBJ\inflate.obj\
   D:\UNZIP\UZLIBOBJ\globals.obj\
   D:\UNZIP\UZLIBOBJ\fileio.obj\
   D:\UNZIP\UZLIBOBJ\extract.obj\
   D:\UNZIP\UZLIBOBJ\explode.obj\
   D:\UNZIP\UZLIBOBJ\crypt.obj\
   D:\UNZIP\UZLIBOBJ\crctab.obj\
   D:\UNZIP\UZLIBOBJ\zipinfo.obj\
   unzip32.lib

D:\UNZIP\LIBS\unzip32.lib : $(Dep_DcbUNZIPbLIBSbunzip32dlib)
  $(TLIB) $< $(IDE_BFLAGS) $(BOptsAt_DcbUNZIPbLIBSbunzip32dlib) @&&|
 -+D:\UNZIP\UZLIBOBJ\nt.obj &
-+D:\UNZIP\UZLIBOBJ\api.obj &
-+D:\UNZIP\UZLIBOBJ\windll.obj &
-+D:\UNZIP\UZLIBOBJ\win32.obj &
-+D:\UNZIP\UZLIBOBJ\crc32.obj &
-+D:\UNZIP\UZLIBOBJ\unshrink.obj &
-+D:\UNZIP\UZLIBOBJ\unreduce.obj &
-+D:\UNZIP\UZLIBOBJ\ttyio.obj &
-+D:\UNZIP\UZLIBOBJ\process.obj &
-+D:\UNZIP\UZLIBOBJ\match.obj &
-+D:\UNZIP\UZLIBOBJ\list.obj &
-+D:\UNZIP\UZLIBOBJ\inflate.obj &
-+D:\UNZIP\UZLIBOBJ\globals.obj &
-+D:\UNZIP\UZLIBOBJ\fileio.obj &
-+D:\UNZIP\UZLIBOBJ\extract.obj &
-+D:\UNZIP\UZLIBOBJ\explode.obj &
-+D:\UNZIP\UZLIBOBJ\crypt.obj &
-+D:\UNZIP\UZLIBOBJ\crctab.obj &
-+D:\UNZIP\UZLIBOBJ\zipinfo.obj
|

D:\UNZIP\UZLIBOBJ\nt.obj :  unzip\win32\nt.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbUNZIPbLIBSbunzip32dlib) $(CompInheritOptsAt_DcbUNZIPbLIBSbunzip32dlib) -o$@ unzip\win32\nt.c
|

D:\UNZIP\UZLIBOBJ\api.obj :  unzip\api.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbUNZIPbLIBSbunzip32dlib) $(CompInheritOptsAt_DcbUNZIPbLIBSbunzip32dlib) -o$@ unzip\api.c
|

D:\UNZIP\UZLIBOBJ\windll.res :  unzip\windll\windll.rc
  $(BRC) -R @&&|
 $(IDE_ResFLAGS) $(ROptsAt_DcbUNZIPbLIBSbunzip32dlib) $(CompInheritOptsAt_DcbUNZIPbLIBSbunzip32dlib)  -FO$@ unzip\windll\windll.rc
|
D:\UNZIP\UZLIBOBJ\windll.obj :  unzip\windll\windll.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbUNZIPbLIBSbunzip32dlib) $(CompInheritOptsAt_DcbUNZIPbLIBSbunzip32dlib) -o$@ unzip\windll\windll.c
|

D:\UNZIP\UZLIBOBJ\win32.obj :  unzip\win32\win32.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbUNZIPbLIBSbunzip32dlib) $(CompInheritOptsAt_DcbUNZIPbLIBSbunzip32dlib) -o$@ unzip\win32\win32.c
|

D:\UNZIP\UZLIBOBJ\crc32.obj :  unzip\crc32.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbUNZIPbLIBSbunzip32dlib) $(CompInheritOptsAt_DcbUNZIPbLIBSbunzip32dlib) -o$@ unzip\crc32.c
|

D:\UNZIP\UZLIBOBJ\unshrink.obj :  unzip\unshrink.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbUNZIPbLIBSbunzip32dlib) $(CompInheritOptsAt_DcbUNZIPbLIBSbunzip32dlib) -o$@ unzip\unshrink.c
|

D:\UNZIP\UZLIBOBJ\unreduce.obj :  unzip\unreduce.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbUNZIPbLIBSbunzip32dlib) $(CompInheritOptsAt_DcbUNZIPbLIBSbunzip32dlib) -o$@ unzip\unreduce.c
|

D:\UNZIP\UZLIBOBJ\ttyio.obj :  unzip\ttyio.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbUNZIPbLIBSbunzip32dlib) $(CompInheritOptsAt_DcbUNZIPbLIBSbunzip32dlib) -o$@ unzip\ttyio.c
|

D:\UNZIP\UZLIBOBJ\process.obj :  unzip\process.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbUNZIPbLIBSbunzip32dlib) $(CompInheritOptsAt_DcbUNZIPbLIBSbunzip32dlib) -o$@ unzip\process.c
|

D:\UNZIP\UZLIBOBJ\match.obj :  unzip\match.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbUNZIPbLIBSbunzip32dlib) $(CompInheritOptsAt_DcbUNZIPbLIBSbunzip32dlib) -o$@ unzip\match.c
|

D:\UNZIP\UZLIBOBJ\list.obj :  unzip\list.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbUNZIPbLIBSbunzip32dlib) $(CompInheritOptsAt_DcbUNZIPbLIBSbunzip32dlib) -o$@ unzip\list.c
|

D:\UNZIP\UZLIBOBJ\inflate.obj :  unzip\inflate.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbUNZIPbLIBSbunzip32dlib) $(CompInheritOptsAt_DcbUNZIPbLIBSbunzip32dlib) -o$@ unzip\inflate.c
|

D:\UNZIP\UZLIBOBJ\globals.obj :  unzip\globals.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbUNZIPbLIBSbunzip32dlib) $(CompInheritOptsAt_DcbUNZIPbLIBSbunzip32dlib) -o$@ unzip\globals.c
|

D:\UNZIP\UZLIBOBJ\fileio.obj :  unzip\fileio.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbUNZIPbLIBSbunzip32dlib) $(CompInheritOptsAt_DcbUNZIPbLIBSbunzip32dlib) -o$@ unzip\fileio.c
|

D:\UNZIP\UZLIBOBJ\extract.obj :  unzip\extract.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbUNZIPbLIBSbunzip32dlib) $(CompInheritOptsAt_DcbUNZIPbLIBSbunzip32dlib) -o$@ unzip\extract.c
|

D:\UNZIP\UZLIBOBJ\explode.obj :  unzip\explode.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbUNZIPbLIBSbunzip32dlib) $(CompInheritOptsAt_DcbUNZIPbLIBSbunzip32dlib) -o$@ unzip\explode.c
|

D:\UNZIP\UZLIBOBJ\crypt.obj :  unzip\crypt.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbUNZIPbLIBSbunzip32dlib) $(CompInheritOptsAt_DcbUNZIPbLIBSbunzip32dlib) -o$@ unzip\crypt.c
|

D:\UNZIP\UZLIBOBJ\crctab.obj :  unzip\crctab.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbUNZIPbLIBSbunzip32dlib) $(CompInheritOptsAt_DcbUNZIPbLIBSbunzip32dlib) -o$@ unzip\crctab.c
|

D:\UNZIP\UZLIBOBJ\zipinfo.obj :  unzip\zipinfo.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbUNZIPbLIBSbunzip32dlib) $(CompInheritOptsAt_DcbUNZIPbLIBSbunzip32dlib) -o$@ unzip\zipinfo.c
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
-WC
-H-
| $@


