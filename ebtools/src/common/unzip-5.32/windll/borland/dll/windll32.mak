#
# Borland C++ IDE generated makefile
# Generated 8/12/97 at 9:31:52 AM 
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
LinkerLocalOptsAtW32_DcbUNZIPbEXE32bunzip32dlib =  -Tpd -aa -V4.0 -c -v
ResLocalOptsAtW32_DcbUNZIPbEXE32bunzip32dlib = 
BLocalOptsAtW32_DcbUNZIPbEXE32bunzip32dlib = 
CompInheritOptsAt_DcbUNZIPbEXE32bunzip32dlib = -IC:\BC5\INCLUDE;D:\UNZIP;D:\UNZIP\WINDLL;D:\UNZIP\WIN32 -DWINDLL;USE_EF_UT_TIME;REENTRANT;DLL;WIN32;
LinkerInheritOptsAt_DcbUNZIPbEXE32bunzip32dlib = -x
LinkerOptsAt_DcbUNZIPbEXE32bunzip32dlib = $(LinkerLocalOptsAtW32_DcbUNZIPbEXE32bunzip32dlib)
ResOptsAt_DcbUNZIPbEXE32bunzip32dlib = $(ResLocalOptsAtW32_DcbUNZIPbEXE32bunzip32dlib)
BOptsAt_DcbUNZIPbEXE32bunzip32dlib = $(BLocalOptsAtW32_DcbUNZIPbEXE32bunzip32dlib)

#
# Dependency List
#
Dep_windll32 = \
   D:\UNZIP\EXE32\unzip32.lib

windll32 : BccW32.cfg $(Dep_windll32)
  echo MakeNode

D:\UNZIP\EXE32\unzip32.lib : unzip32.dll
  $(IMPLIB) $@ unzip32.dll


Dep_unzip32ddll = \
   D:\UNZIP\UNZOBJ32\nt.obj\
   D:\UNZIP\UNZOBJ32\api.obj\
   unzip\windll\windll32.def\
   D:\UNZIP\UNZOBJ32\windll.res\
   D:\UNZIP\UNZOBJ32\windll.obj\
   D:\UNZIP\UNZOBJ32\win32.obj\
   D:\UNZIP\UNZOBJ32\crc32.obj\
   D:\UNZIP\UNZOBJ32\unshrink.obj\
   D:\UNZIP\UNZOBJ32\unreduce.obj\
   D:\UNZIP\UNZOBJ32\ttyio.obj\
   D:\UNZIP\UNZOBJ32\process.obj\
   D:\UNZIP\UNZOBJ32\match.obj\
   D:\UNZIP\UNZOBJ32\list.obj\
   D:\UNZIP\UNZOBJ32\inflate.obj\
   D:\UNZIP\UNZOBJ32\globals.obj\
   D:\UNZIP\UNZOBJ32\fileio.obj\
   D:\UNZIP\UNZOBJ32\extract.obj\
   D:\UNZIP\UNZOBJ32\explode.obj\
   D:\UNZIP\UNZOBJ32\crypt.obj\
   D:\UNZIP\UNZOBJ32\crctab.obj\
   D:\UNZIP\UNZOBJ32\zipinfo.obj

unzip32.dll : $(Dep_unzip32ddll)
  $(ILINK32) @&&|
 /v $(IDE_LinkFLAGS32) $(LinkerOptsAt_DcbUNZIPbEXE32bunzip32dlib) $(LinkerInheritOptsAt_DcbUNZIPbEXE32bunzip32dlib) +
C:\BC5\LIB\c0d32.obj+
D:\UNZIP\UNZOBJ32\nt.obj+
D:\UNZIP\UNZOBJ32\api.obj+
D:\UNZIP\UNZOBJ32\windll.obj+
D:\UNZIP\UNZOBJ32\win32.obj+
D:\UNZIP\UNZOBJ32\crc32.obj+
D:\UNZIP\UNZOBJ32\unshrink.obj+
D:\UNZIP\UNZOBJ32\unreduce.obj+
D:\UNZIP\UNZOBJ32\ttyio.obj+
D:\UNZIP\UNZOBJ32\process.obj+
D:\UNZIP\UNZOBJ32\match.obj+
D:\UNZIP\UNZOBJ32\list.obj+
D:\UNZIP\UNZOBJ32\inflate.obj+
D:\UNZIP\UNZOBJ32\globals.obj+
D:\UNZIP\UNZOBJ32\fileio.obj+
D:\UNZIP\UNZOBJ32\extract.obj+
D:\UNZIP\UNZOBJ32\explode.obj+
D:\UNZIP\UNZOBJ32\crypt.obj+
D:\UNZIP\UNZOBJ32\crctab.obj+
D:\UNZIP\UNZOBJ32\zipinfo.obj
$<,$*
C:\BC5\LIB\import32.lib+
C:\BC5\LIB\cw32.lib
unzip\windll\windll32.def
D:\UNZIP\UNZOBJ32\windll.res

|
D:\UNZIP\UNZOBJ32\nt.obj :  unzip\win32\nt.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbUNZIPbEXE32bunzip32dlib) $(CompInheritOptsAt_DcbUNZIPbEXE32bunzip32dlib) -o$@ unzip\win32\nt.c
|

D:\UNZIP\UNZOBJ32\api.obj :  unzip\api.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbUNZIPbEXE32bunzip32dlib) $(CompInheritOptsAt_DcbUNZIPbEXE32bunzip32dlib) -o$@ unzip\api.c
|

D:\UNZIP\UNZOBJ32\windll.res :  unzip\windll\windll.rc
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_DcbUNZIPbEXE32bunzip32dlib) $(CompInheritOptsAt_DcbUNZIPbEXE32bunzip32dlib)  -FO$@ unzip\windll\windll.rc
|
D:\UNZIP\UNZOBJ32\windll.obj :  unzip\windll\windll.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbUNZIPbEXE32bunzip32dlib) $(CompInheritOptsAt_DcbUNZIPbEXE32bunzip32dlib) -o$@ unzip\windll\windll.c
|

D:\UNZIP\UNZOBJ32\win32.obj :  unzip\win32\win32.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbUNZIPbEXE32bunzip32dlib) $(CompInheritOptsAt_DcbUNZIPbEXE32bunzip32dlib) -o$@ unzip\win32\win32.c
|

D:\UNZIP\UNZOBJ32\crc32.obj :  unzip\crc32.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbUNZIPbEXE32bunzip32dlib) $(CompInheritOptsAt_DcbUNZIPbEXE32bunzip32dlib) -o$@ unzip\crc32.c
|

D:\UNZIP\UNZOBJ32\unshrink.obj :  unzip\unshrink.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbUNZIPbEXE32bunzip32dlib) $(CompInheritOptsAt_DcbUNZIPbEXE32bunzip32dlib) -o$@ unzip\unshrink.c
|

D:\UNZIP\UNZOBJ32\unreduce.obj :  unzip\unreduce.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbUNZIPbEXE32bunzip32dlib) $(CompInheritOptsAt_DcbUNZIPbEXE32bunzip32dlib) -o$@ unzip\unreduce.c
|

D:\UNZIP\UNZOBJ32\ttyio.obj :  unzip\ttyio.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbUNZIPbEXE32bunzip32dlib) $(CompInheritOptsAt_DcbUNZIPbEXE32bunzip32dlib) -o$@ unzip\ttyio.c
|

D:\UNZIP\UNZOBJ32\process.obj :  unzip\process.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbUNZIPbEXE32bunzip32dlib) $(CompInheritOptsAt_DcbUNZIPbEXE32bunzip32dlib) -o$@ unzip\process.c
|

D:\UNZIP\UNZOBJ32\match.obj :  unzip\match.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbUNZIPbEXE32bunzip32dlib) $(CompInheritOptsAt_DcbUNZIPbEXE32bunzip32dlib) -o$@ unzip\match.c
|

D:\UNZIP\UNZOBJ32\list.obj :  unzip\list.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbUNZIPbEXE32bunzip32dlib) $(CompInheritOptsAt_DcbUNZIPbEXE32bunzip32dlib) -o$@ unzip\list.c
|

D:\UNZIP\UNZOBJ32\inflate.obj :  unzip\inflate.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbUNZIPbEXE32bunzip32dlib) $(CompInheritOptsAt_DcbUNZIPbEXE32bunzip32dlib) -o$@ unzip\inflate.c
|

D:\UNZIP\UNZOBJ32\globals.obj :  unzip\globals.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbUNZIPbEXE32bunzip32dlib) $(CompInheritOptsAt_DcbUNZIPbEXE32bunzip32dlib) -o$@ unzip\globals.c
|

D:\UNZIP\UNZOBJ32\fileio.obj :  unzip\fileio.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbUNZIPbEXE32bunzip32dlib) $(CompInheritOptsAt_DcbUNZIPbEXE32bunzip32dlib) -o$@ unzip\fileio.c
|

D:\UNZIP\UNZOBJ32\extract.obj :  unzip\extract.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbUNZIPbEXE32bunzip32dlib) $(CompInheritOptsAt_DcbUNZIPbEXE32bunzip32dlib) -o$@ unzip\extract.c
|

D:\UNZIP\UNZOBJ32\explode.obj :  unzip\explode.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbUNZIPbEXE32bunzip32dlib) $(CompInheritOptsAt_DcbUNZIPbEXE32bunzip32dlib) -o$@ unzip\explode.c
|

D:\UNZIP\UNZOBJ32\crypt.obj :  unzip\crypt.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbUNZIPbEXE32bunzip32dlib) $(CompInheritOptsAt_DcbUNZIPbEXE32bunzip32dlib) -o$@ unzip\crypt.c
|

D:\UNZIP\UNZOBJ32\crctab.obj :  unzip\crctab.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbUNZIPbEXE32bunzip32dlib) $(CompInheritOptsAt_DcbUNZIPbEXE32bunzip32dlib) -o$@ unzip\crctab.c
|

D:\UNZIP\UNZOBJ32\zipinfo.obj :  unzip\zipinfo.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_DcbUNZIPbEXE32bunzip32dlib) $(CompInheritOptsAt_DcbUNZIPbEXE32bunzip32dlib) -o$@ unzip\zipinfo.c
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
-H-
| $@


