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
LLATW16_DcbUNZIPbEXE16bunzip16dlib =  -Twd -c -LC:\BC45\LIB -v
RLATW16_DcbUNZIPbEXE16bunzip16dlib =  -31
BLATW16_DcbUNZIPbEXE16bunzip16dlib = 
CNIEAT_DcbUNZIPbEXE16bunzip16dlib = -IC:\BC45\INCLUDE;D:\UNZIP;D:\UNZIP\MSDOS;D:\UNZIP\WINDLL -DWINDLL;USE_EF_UT_TIME;NDEBUG;REENTRANT;DLL
LNIEAT_DcbUNZIPbEXE16bunzip16dlib = -x
LEAT_DcbUNZIPbEXE16bunzip16dlib = $(LLATW16_DcbUNZIPbEXE16bunzip16dlib)
REAT_DcbUNZIPbEXE16bunzip16dlib = $(RLATW16_DcbUNZIPbEXE16bunzip16dlib)
BEAT_DcbUNZIPbEXE16bunzip16dlib = $(BLATW16_DcbUNZIPbEXE16bunzip16dlib)

#
# Dependency List
#
Dep_windll16 = \
   D:\UNZIP\EXE16\unzip16.lib

windll16 : BccW16.cfg $(Dep_windll16)
  echo MakeNode 

D:\UNZIP\EXE16\unzip16.lib : unzip16.dll
  $(IMPLIB) $@ unzip16.dll


Dep_unzip16ddll = \
   D:\UNZIP\UNZOBJ16\api.obj\
   D:\UNZIP\UNZOBJ16\windll.obj\
   D:\UNZIP\UNZOBJ16\windll.res\
   unzip\windll\windll16.def\
   D:\UNZIP\UNZOBJ16\msdos.obj\
   D:\UNZIP\UNZOBJ16\crc32.obj\
   D:\UNZIP\UNZOBJ16\crctab.obj\
   D:\UNZIP\UNZOBJ16\crypt.obj\
   D:\UNZIP\UNZOBJ16\explode.obj\
   D:\UNZIP\UNZOBJ16\extract.obj\
   D:\UNZIP\UNZOBJ16\fileio.obj\
   D:\UNZIP\UNZOBJ16\globals.obj\
   D:\UNZIP\UNZOBJ16\inflate.obj\
   D:\UNZIP\UNZOBJ16\list.obj\
   D:\UNZIP\UNZOBJ16\match.obj\
   D:\UNZIP\UNZOBJ16\process.obj\
   D:\UNZIP\UNZOBJ16\ttyio.obj\
   D:\UNZIP\UNZOBJ16\unreduce.obj\
   D:\UNZIP\UNZOBJ16\unshrink.obj\
   D:\UNZIP\UNZOBJ16\zipinfo.obj

unzip16.dll : $(Dep_unzip16ddll)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGS) $(LEAT_DcbUNZIPbEXE16bunzip16dlib) $(LNIEAT_DcbUNZIPbEXE16bunzip16dlib) +
C:\BC45\LIB\c0dl.obj+
D:\UNZIP\UNZOBJ16\api.obj+
D:\UNZIP\UNZOBJ16\windll.obj+
D:\UNZIP\UNZOBJ16\msdos.obj+
D:\UNZIP\UNZOBJ16\crc32.obj+
D:\UNZIP\UNZOBJ16\crctab.obj+
D:\UNZIP\UNZOBJ16\crypt.obj+
D:\UNZIP\UNZOBJ16\explode.obj+
D:\UNZIP\UNZOBJ16\extract.obj+
D:\UNZIP\UNZOBJ16\fileio.obj+
D:\UNZIP\UNZOBJ16\globals.obj+
D:\UNZIP\UNZOBJ16\inflate.obj+
D:\UNZIP\UNZOBJ16\list.obj+
D:\UNZIP\UNZOBJ16\match.obj+
D:\UNZIP\UNZOBJ16\process.obj+
D:\UNZIP\UNZOBJ16\ttyio.obj+
D:\UNZIP\UNZOBJ16\unreduce.obj+
D:\UNZIP\UNZOBJ16\unshrink.obj+
D:\UNZIP\UNZOBJ16\zipinfo.obj
$<,$*
C:\BC45\LIB\import.lib+
C:\BC45\LIB\mathwl.lib+
C:\BC45\LIB\cwl.lib
unzip\windll\windll16.def
|
   $(BRC) D:\UNZIP\UNZOBJ16\windll.res $<

D:\UNZIP\UNZOBJ16\api.obj :  unzip\api.c
  $(BCC)   -P- -c @&&|
 $(CEAT_DcbUNZIPbEXE16bunzip16dlib) $(CNIEAT_DcbUNZIPbEXE16bunzip16dlib) -o$@ unzip\api.c
|

D:\UNZIP\UNZOBJ16\windll.obj :  unzip\windll\windll.c
  $(BCC)   -P- -c @&&|
 $(CEAT_DcbUNZIPbEXE16bunzip16dlib) $(CNIEAT_DcbUNZIPbEXE16bunzip16dlib) -o$@ unzip\windll\windll.c
|

D:\UNZIP\UNZOBJ16\windll.res :  unzip\windll\windll.rc
  $(BRC) $(IDE_RFLAGS) $(REAT_DcbUNZIPbEXE16bunzip16dlib) $(CNIEAT_DcbUNZIPbEXE16bunzip16dlib) -R -FO$@ unzip\windll\windll.rc

D:\UNZIP\UNZOBJ16\msdos.obj :  unzip\msdos\msdos.c
  $(BCC)   -P- -c @&&|
 $(CEAT_DcbUNZIPbEXE16bunzip16dlib) $(CNIEAT_DcbUNZIPbEXE16bunzip16dlib) -o$@ unzip\msdos\msdos.c
|

D:\UNZIP\UNZOBJ16\crc32.obj :  unzip\crc32.c
  $(BCC)   -P- -c @&&|
 $(CEAT_DcbUNZIPbEXE16bunzip16dlib) $(CNIEAT_DcbUNZIPbEXE16bunzip16dlib) -o$@ unzip\crc32.c
|

D:\UNZIP\UNZOBJ16\crctab.obj :  unzip\crctab.c
  $(BCC)   -P- -c @&&|
 $(CEAT_DcbUNZIPbEXE16bunzip16dlib) $(CNIEAT_DcbUNZIPbEXE16bunzip16dlib) -o$@ unzip\crctab.c
|

D:\UNZIP\UNZOBJ16\crypt.obj :  unzip\crypt.c
  $(BCC)   -P- -c @&&|
 $(CEAT_DcbUNZIPbEXE16bunzip16dlib) $(CNIEAT_DcbUNZIPbEXE16bunzip16dlib) -o$@ unzip\crypt.c
|

D:\UNZIP\UNZOBJ16\explode.obj :  unzip\explode.c
  $(BCC)   -P- -c @&&|
 $(CEAT_DcbUNZIPbEXE16bunzip16dlib) $(CNIEAT_DcbUNZIPbEXE16bunzip16dlib) -o$@ unzip\explode.c
|

D:\UNZIP\UNZOBJ16\extract.obj :  unzip\extract.c
  $(BCC)   -P- -c @&&|
 $(CEAT_DcbUNZIPbEXE16bunzip16dlib) $(CNIEAT_DcbUNZIPbEXE16bunzip16dlib) -o$@ unzip\extract.c
|

D:\UNZIP\UNZOBJ16\fileio.obj :  unzip\fileio.c
  $(BCC)   -P- -c @&&|
 $(CEAT_DcbUNZIPbEXE16bunzip16dlib) $(CNIEAT_DcbUNZIPbEXE16bunzip16dlib) -o$@ unzip\fileio.c
|

D:\UNZIP\UNZOBJ16\globals.obj :  unzip\globals.c
  $(BCC)   -P- -c @&&|
 $(CEAT_DcbUNZIPbEXE16bunzip16dlib) $(CNIEAT_DcbUNZIPbEXE16bunzip16dlib) -o$@ unzip\globals.c
|

D:\UNZIP\UNZOBJ16\inflate.obj :  unzip\inflate.c
  $(BCC)   -P- -c @&&|
 $(CEAT_DcbUNZIPbEXE16bunzip16dlib) $(CNIEAT_DcbUNZIPbEXE16bunzip16dlib) -o$@ unzip\inflate.c
|

D:\UNZIP\UNZOBJ16\list.obj :  unzip\list.c
  $(BCC)   -P- -c @&&|
 $(CEAT_DcbUNZIPbEXE16bunzip16dlib) $(CNIEAT_DcbUNZIPbEXE16bunzip16dlib) -o$@ unzip\list.c
|

D:\UNZIP\UNZOBJ16\match.obj :  unzip\match.c
  $(BCC)   -P- -c @&&|
 $(CEAT_DcbUNZIPbEXE16bunzip16dlib) $(CNIEAT_DcbUNZIPbEXE16bunzip16dlib) -o$@ unzip\match.c
|

D:\UNZIP\UNZOBJ16\process.obj :  unzip\process.c
  $(BCC)   -P- -c @&&|
 $(CEAT_DcbUNZIPbEXE16bunzip16dlib) $(CNIEAT_DcbUNZIPbEXE16bunzip16dlib) -o$@ unzip\process.c
|

D:\UNZIP\UNZOBJ16\ttyio.obj :  unzip\ttyio.c
  $(BCC)   -P- -c @&&|
 $(CEAT_DcbUNZIPbEXE16bunzip16dlib) $(CNIEAT_DcbUNZIPbEXE16bunzip16dlib) -o$@ unzip\ttyio.c
|

D:\UNZIP\UNZOBJ16\unreduce.obj :  unzip\unreduce.c
  $(BCC)   -P- -c @&&|
 $(CEAT_DcbUNZIPbEXE16bunzip16dlib) $(CNIEAT_DcbUNZIPbEXE16bunzip16dlib) -o$@ unzip\unreduce.c
|

D:\UNZIP\UNZOBJ16\unshrink.obj :  unzip\unshrink.c
  $(BCC)   -P- -c @&&|
 $(CEAT_DcbUNZIPbEXE16bunzip16dlib) $(CNIEAT_DcbUNZIPbEXE16bunzip16dlib) -o$@ unzip\unshrink.c
|

D:\UNZIP\UNZOBJ16\zipinfo.obj :  unzip\zipinfo.c
  $(BCC)   -P- -c @&&|
 $(CEAT_DcbUNZIPbEXE16bunzip16dlib) $(CNIEAT_DcbUNZIPbEXE16bunzip16dlib) -o$@ unzip\zipinfo.c
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
| $@


