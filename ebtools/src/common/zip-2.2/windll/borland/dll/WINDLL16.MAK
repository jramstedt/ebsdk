#
# Borland C++ IDE generated makefile
#
.AUTODEPEND


#
# Borland C++ tools
#
IMPLIB  = Implib
BCC     = Bcc +BccW16.cfg 
TLINK   = TLink
TLIB    = TLib
BRC     = Brc
TASM    = Tasm
#
# IDE macros
#


#
# Options
#
IDE_LFLAGS =  -LC:\BC45\LIB -v -f
IDE_RFLAGS =  -31
LLATW16_DcbZIPbEXE16bzip16dlib =  -Twd -c -f -LC:\BC45\LIB
RLATW16_DcbZIPbEXE16bzip16dlib =  -31
BLATW16_DcbZIPbEXE16bzip16dlib = 
CNIEAT_DcbZIPbEXE16bzip16dlib = -IC:\BC45\INCLUDE;D:\ZIP;D:\ZIP\WINDLL;D:\ZIP\MSDOS -DWINDLL;NO_ASM;DOS;NO_SECURE_TESTS;USE_EF_UX_TIME;NDEBUG;USE_ZIPMAIN
LNIEAT_DcbZIPbEXE16bzip16dlib = -x
LEAT_DcbZIPbEXE16bzip16dlib = $(LLATW16_DcbZIPbEXE16bzip16dlib)
REAT_DcbZIPbEXE16bzip16dlib = $(RLATW16_DcbZIPbEXE16bzip16dlib)
BEAT_DcbZIPbEXE16bzip16dlib = $(BLATW16_DcbZIPbEXE16bzip16dlib)

#
# Dependency List
#
Dep_windll16 = \
   D:\ZIP\EXE16\zip16.lib

windll16 : BccW16.cfg $(Dep_windll16)
  echo MakeNode 

D:\ZIP\EXE16\zip16.lib : zip16.dll
  $(IMPLIB) $@ zip16.dll


Dep_zip16ddll = \
   D:\ZIP\ZIPOBJ16\api.obj\
   zip\windll\windll16.def\
   D:\ZIP\ZIPOBJ16\windll.res\
   D:\ZIP\ZIPOBJ16\windll.obj\
   D:\ZIP\ZIPOBJ16\msdos.obj\
   D:\ZIP\ZIPOBJ16\bits.obj\
   D:\ZIP\ZIPOBJ16\crc32.obj\
   D:\ZIP\ZIPOBJ16\crctab.obj\
   D:\ZIP\ZIPOBJ16\crypt.obj\
   D:\ZIP\ZIPOBJ16\deflate.obj\
   D:\ZIP\ZIPOBJ16\fileio.obj\
   D:\ZIP\ZIPOBJ16\globals.obj\
   D:\ZIP\ZIPOBJ16\trees.obj\
   D:\ZIP\ZIPOBJ16\ttyio.obj\
   D:\ZIP\ZIPOBJ16\util.obj\
   D:\ZIP\ZIPOBJ16\zip.obj\
   D:\ZIP\ZIPOBJ16\zipfile.obj\
   D:\ZIP\ZIPOBJ16\zipup.obj

zip16.dll : $(Dep_zip16ddll)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGS) $(LEAT_DcbZIPbEXE16bzip16dlib) $(LNIEAT_DcbZIPbEXE16bzip16dlib) +
C:\BC45\LIB\c0dl.obj+
D:\ZIP\ZIPOBJ16\api.obj+
D:\ZIP\ZIPOBJ16\windll.obj+
D:\ZIP\ZIPOBJ16\msdos.obj+
D:\ZIP\ZIPOBJ16\bits.obj+
D:\ZIP\ZIPOBJ16\crc32.obj+
D:\ZIP\ZIPOBJ16\crctab.obj+
D:\ZIP\ZIPOBJ16\crypt.obj+
D:\ZIP\ZIPOBJ16\deflate.obj+
D:\ZIP\ZIPOBJ16\fileio.obj+
D:\ZIP\ZIPOBJ16\globals.obj+
D:\ZIP\ZIPOBJ16\trees.obj+
D:\ZIP\ZIPOBJ16\ttyio.obj+
D:\ZIP\ZIPOBJ16\util.obj+
D:\ZIP\ZIPOBJ16\zip.obj+
D:\ZIP\ZIPOBJ16\zipfile.obj+
D:\ZIP\ZIPOBJ16\zipup.obj
$<,$*
C:\BC45\LIB\import.lib+
C:\BC45\LIB\mathwl.lib+
C:\BC45\LIB\cwl.lib
zip\windll\windll16.def
|
   $(BRC) D:\ZIP\ZIPOBJ16\windll.res $<

D:\ZIP\ZIPOBJ16\api.obj :  zip\api.c
  $(BCC)   -P- -c @&&|
 $(CEAT_DcbZIPbEXE16bzip16dlib) $(CNIEAT_DcbZIPbEXE16bzip16dlib) -o$@ zip\api.c
|

D:\ZIP\ZIPOBJ16\windll.res :  zip\windll\windll.rc
  $(BRC) $(IDE_RFLAGS) $(REAT_DcbZIPbEXE16bzip16dlib) $(CNIEAT_DcbZIPbEXE16bzip16dlib) -R -FO$@ zip\windll\windll.rc

D:\ZIP\ZIPOBJ16\windll.obj :  zip\windll\windll.c
  $(BCC)   -P- -c @&&|
 $(CEAT_DcbZIPbEXE16bzip16dlib) $(CNIEAT_DcbZIPbEXE16bzip16dlib) -o$@ zip\windll\windll.c
|

D:\ZIP\ZIPOBJ16\msdos.obj :  zip\msdos\msdos.c
  $(BCC)   -P- -c @&&|
 $(CEAT_DcbZIPbEXE16bzip16dlib) $(CNIEAT_DcbZIPbEXE16bzip16dlib) -o$@ zip\msdos\msdos.c
|

D:\ZIP\ZIPOBJ16\bits.obj :  zip\bits.c
  $(BCC)   -P- -c @&&|
 $(CEAT_DcbZIPbEXE16bzip16dlib) $(CNIEAT_DcbZIPbEXE16bzip16dlib) -o$@ zip\bits.c
|

D:\ZIP\ZIPOBJ16\crc32.obj :  zip\crc32.c
  $(BCC)   -P- -c @&&|
 $(CEAT_DcbZIPbEXE16bzip16dlib) $(CNIEAT_DcbZIPbEXE16bzip16dlib) -o$@ zip\crc32.c
|

D:\ZIP\ZIPOBJ16\crctab.obj :  zip\crctab.c
  $(BCC)   -P- -c @&&|
 $(CEAT_DcbZIPbEXE16bzip16dlib) $(CNIEAT_DcbZIPbEXE16bzip16dlib) -o$@ zip\crctab.c
|

D:\ZIP\ZIPOBJ16\crypt.obj :  zip\crypt.c
  $(BCC)   -P- -c @&&|
 $(CEAT_DcbZIPbEXE16bzip16dlib) $(CNIEAT_DcbZIPbEXE16bzip16dlib) -o$@ zip\crypt.c
|

D:\ZIP\ZIPOBJ16\deflate.obj :  zip\deflate.c
  $(BCC)   -P- -c @&&|
 $(CEAT_DcbZIPbEXE16bzip16dlib) $(CNIEAT_DcbZIPbEXE16bzip16dlib) -o$@ zip\deflate.c
|

D:\ZIP\ZIPOBJ16\fileio.obj :  zip\fileio.c
  $(BCC)   -P- -c @&&|
 $(CEAT_DcbZIPbEXE16bzip16dlib) $(CNIEAT_DcbZIPbEXE16bzip16dlib) -o$@ zip\fileio.c
|

D:\ZIP\ZIPOBJ16\globals.obj :  zip\globals.c
  $(BCC)   -P- -c @&&|
 $(CEAT_DcbZIPbEXE16bzip16dlib) $(CNIEAT_DcbZIPbEXE16bzip16dlib) -o$@ zip\globals.c
|

D:\ZIP\ZIPOBJ16\trees.obj :  zip\trees.c
  $(BCC)   -P- -c @&&|
 $(CEAT_DcbZIPbEXE16bzip16dlib) $(CNIEAT_DcbZIPbEXE16bzip16dlib) -o$@ zip\trees.c
|

D:\ZIP\ZIPOBJ16\ttyio.obj :  zip\ttyio.c
  $(BCC)   -P- -c @&&|
 $(CEAT_DcbZIPbEXE16bzip16dlib) $(CNIEAT_DcbZIPbEXE16bzip16dlib) -o$@ zip\ttyio.c
|

D:\ZIP\ZIPOBJ16\util.obj :  zip\util.c
  $(BCC)   -P- -c @&&|
 $(CEAT_DcbZIPbEXE16bzip16dlib) $(CNIEAT_DcbZIPbEXE16bzip16dlib) -o$@ zip\util.c
|

D:\ZIP\ZIPOBJ16\zip.obj :  zip\zip.c
  $(BCC)   -P- -c @&&|
 $(CEAT_DcbZIPbEXE16bzip16dlib) $(CNIEAT_DcbZIPbEXE16bzip16dlib) -o$@ zip\zip.c
|

D:\ZIP\ZIPOBJ16\zipfile.obj :  zip\zipfile.c
  $(BCC)   -P- -c @&&|
 $(CEAT_DcbZIPbEXE16bzip16dlib) $(CNIEAT_DcbZIPbEXE16bzip16dlib) -o$@ zip\zipfile.c
|

D:\ZIP\ZIPOBJ16\zipup.obj :  zip\zipup.c
  $(BCC)   -P- -c @&&|
 $(CEAT_DcbZIPbEXE16bzip16dlib) $(CNIEAT_DcbZIPbEXE16bzip16dlib) -o$@ zip\zipup.c
|

# Compiler configuration file
BccW16.cfg : 
   Copy &&|
-R
-v
-vi
-H
-H=wiz16all.csm
-f-
-ff-
-ml
-Ff
-dc
-y-
-N
-3
-ml
-WD
-d
-N
-H-
-Ff
-dc
-Vf
-A-
| $@


