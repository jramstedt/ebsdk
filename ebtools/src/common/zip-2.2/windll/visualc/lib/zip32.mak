# Microsoft Developer Studio Generated NMAKE File, Based on zip32.dsp
!IF "$(CFG)" == ""
CFG=zip32 - Win32 Debug
!MESSAGE No configuration specified. Defaulting to zip32 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "zip32 - Win32 Release" && "$(CFG)" != "zip32 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "zip32.mak" CFG="zip32 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "zip32 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "zip32 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "zip32 - Win32 Release"

OUTDIR=.\..\Release\libs
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\..\Release\libs
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\zip32.lib"

!ELSE 

ALL : "$(OUTDIR)\zip32.lib"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\api.obj"
	-@erase "$(INTDIR)\bits.obj"
	-@erase "$(INTDIR)\crc32.obj"
	-@erase "$(INTDIR)\crctab.obj"
	-@erase "$(INTDIR)\crypt.obj"
	-@erase "$(INTDIR)\deflate.obj"
	-@erase "$(INTDIR)\fileio.obj"
	-@erase "$(INTDIR)\globals.obj"
	-@erase "$(INTDIR)\nt.obj"
	-@erase "$(INTDIR)\trees.obj"
	-@erase "$(INTDIR)\ttyio.obj"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\win32.obj"
	-@erase "$(INTDIR)\win32zip.obj"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(INTDIR)\zip.obj"
	-@erase "$(INTDIR)\zipfile.obj"
	-@erase "$(INTDIR)\zipup.obj"
	-@erase "$(OUTDIR)\zip32.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "D:\WIZ\ZIP" /I "D:\WIZ\ZIP\WIN32" /I\
 "D:\WIZ\ZIP\WINDLL" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "NO_ASM" /D\
 "WINDLL" /D "MSDOS" /D "USE_EF_UX_TIME" /D "USE_ZIPMAIN" /D "ZIPLIB"\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\zip32.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\zip32.lib" 
LIB32_OBJS= \
	"$(INTDIR)\api.obj" \
	"$(INTDIR)\bits.obj" \
	"$(INTDIR)\crc32.obj" \
	"$(INTDIR)\crctab.obj" \
	"$(INTDIR)\crypt.obj" \
	"$(INTDIR)\deflate.obj" \
	"$(INTDIR)\fileio.obj" \
	"$(INTDIR)\globals.obj" \
	"$(INTDIR)\nt.obj" \
	"$(INTDIR)\trees.obj" \
	"$(INTDIR)\ttyio.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\win32.obj" \
	"$(INTDIR)\win32zip.obj" \
	"$(INTDIR)\windll.obj" \
	"$(INTDIR)\zip.obj" \
	"$(INTDIR)\zipfile.obj" \
	"$(INTDIR)\zipup.obj"

"$(OUTDIR)\zip32.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "zip32 - Win32 Debug"

OUTDIR=.\..\Debug\libs
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\..\Debug\libs
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\zip32.lib"

!ELSE 

ALL : "$(OUTDIR)\zip32.lib"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\api.obj"
	-@erase "$(INTDIR)\bits.obj"
	-@erase "$(INTDIR)\crc32.obj"
	-@erase "$(INTDIR)\crctab.obj"
	-@erase "$(INTDIR)\crypt.obj"
	-@erase "$(INTDIR)\deflate.obj"
	-@erase "$(INTDIR)\fileio.obj"
	-@erase "$(INTDIR)\globals.obj"
	-@erase "$(INTDIR)\nt.obj"
	-@erase "$(INTDIR)\trees.obj"
	-@erase "$(INTDIR)\ttyio.obj"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\win32.obj"
	-@erase "$(INTDIR)\win32zip.obj"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(INTDIR)\zip.obj"
	-@erase "$(INTDIR)\zipfile.obj"
	-@erase "$(INTDIR)\zipup.obj"
	-@erase "$(OUTDIR)\zip32.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /GX /Z7 /Od /I "D:\WIZ\ZIP" /I "D:\WIZ\ZIP\WIN32" /I\
 "D:\WIZ\ZIP\WINDLL" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "NO_ASM" /D\
 "WINDLL" /D "MSDOS" /D "USE_EF_UX_TIME" /D "USE_ZIPMAIN" /D "ZIPLIB"\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\zip32.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\zip32.lib" 
LIB32_OBJS= \
	"$(INTDIR)\api.obj" \
	"$(INTDIR)\bits.obj" \
	"$(INTDIR)\crc32.obj" \
	"$(INTDIR)\crctab.obj" \
	"$(INTDIR)\crypt.obj" \
	"$(INTDIR)\deflate.obj" \
	"$(INTDIR)\fileio.obj" \
	"$(INTDIR)\globals.obj" \
	"$(INTDIR)\nt.obj" \
	"$(INTDIR)\trees.obj" \
	"$(INTDIR)\ttyio.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\win32.obj" \
	"$(INTDIR)\win32zip.obj" \
	"$(INTDIR)\windll.obj" \
	"$(INTDIR)\zip.obj" \
	"$(INTDIR)\zipfile.obj" \
	"$(INTDIR)\zipup.obj"

"$(OUTDIR)\zip32.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "zip32 - Win32 Release" || "$(CFG)" == "zip32 - Win32 Debug"
SOURCE=..\..\..\..\wiz\zip\api.c
DEP_CPP_API_C=\
	"..\..\..\..\wiz\zip\api.h"\
	"..\..\..\..\wiz\zip\crypt.h"\
	"..\..\..\..\wiz\zip\revision.h"\
	"..\..\..\..\wiz\zip\tailor.h"\
	"..\..\..\..\wiz\zip\win32\osdep.h"\
	"..\..\..\..\wiz\zip\windll\structs.h"\
	"..\..\..\..\wiz\zip\windll\windll.h"\
	"..\..\..\..\wiz\zip\zip.h"\
	"..\..\..\..\wiz\zip\ziperr.h"\
	

"$(INTDIR)\api.obj" : $(SOURCE) $(DEP_CPP_API_C) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\wiz\zip\bits.c
DEP_CPP_BITS_=\
	"..\..\..\..\wiz\zip\api.h"\
	"..\..\..\..\wiz\zip\crypt.h"\
	"..\..\..\..\wiz\zip\tailor.h"\
	"..\..\..\..\wiz\zip\win32\osdep.h"\
	"..\..\..\..\wiz\zip\zip.h"\
	"..\..\..\..\wiz\zip\ziperr.h"\
	

"$(INTDIR)\bits.obj" : $(SOURCE) $(DEP_CPP_BITS_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\wiz\zip\crc32.c
DEP_CPP_CRC32=\
	"..\..\..\..\wiz\zip\api.h"\
	"..\..\..\..\wiz\zip\tailor.h"\
	"..\..\..\..\wiz\zip\win32\osdep.h"\
	"..\..\..\..\wiz\zip\zip.h"\
	"..\..\..\..\wiz\zip\ziperr.h"\
	

"$(INTDIR)\crc32.obj" : $(SOURCE) $(DEP_CPP_CRC32) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\wiz\zip\crctab.c
DEP_CPP_CRCTA=\
	"..\..\..\..\wiz\zip\api.h"\
	"..\..\..\..\wiz\zip\tailor.h"\
	"..\..\..\..\wiz\zip\win32\osdep.h"\
	"..\..\..\..\wiz\zip\zip.h"\
	"..\..\..\..\wiz\zip\ziperr.h"\
	

"$(INTDIR)\crctab.obj" : $(SOURCE) $(DEP_CPP_CRCTA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\wiz\zip\crypt.c
DEP_CPP_CRYPT=\
	"..\..\..\..\wiz\zip\api.h"\
	"..\..\..\..\wiz\zip\crypt.h"\
	"..\..\..\..\wiz\zip\tailor.h"\
	"..\..\..\..\wiz\zip\ttyio.h"\
	"..\..\..\..\wiz\zip\win32\osdep.h"\
	"..\..\..\..\wiz\zip\zip.h"\
	"..\..\..\..\wiz\zip\ziperr.h"\
	

"$(INTDIR)\crypt.obj" : $(SOURCE) $(DEP_CPP_CRYPT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\wiz\zip\deflate.c
DEP_CPP_DEFLA=\
	"..\..\..\..\wiz\zip\api.h"\
	"..\..\..\..\wiz\zip\tailor.h"\
	"..\..\..\..\wiz\zip\win32\osdep.h"\
	"..\..\..\..\wiz\zip\zip.h"\
	"..\..\..\..\wiz\zip\ziperr.h"\
	

"$(INTDIR)\deflate.obj" : $(SOURCE) $(DEP_CPP_DEFLA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\wiz\zip\fileio.c
DEP_CPP_FILEI=\
	"..\..\..\..\wiz\zip\api.h"\
	"..\..\..\..\wiz\zip\tailor.h"\
	"..\..\..\..\wiz\zip\win32\osdep.h"\
	"..\..\..\..\wiz\zip\zip.h"\
	"..\..\..\..\wiz\zip\ziperr.h"\
	

"$(INTDIR)\fileio.obj" : $(SOURCE) $(DEP_CPP_FILEI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\wiz\zip\globals.c
DEP_CPP_GLOBA=\
	"..\..\..\..\wiz\zip\api.h"\
	"..\..\..\..\wiz\zip\tailor.h"\
	"..\..\..\..\wiz\zip\win32\osdep.h"\
	"..\..\..\..\wiz\zip\zip.h"\
	"..\..\..\..\wiz\zip\ziperr.h"\
	

"$(INTDIR)\globals.obj" : $(SOURCE) $(DEP_CPP_GLOBA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\wiz\zip\Win32\nt.c

!IF  "$(CFG)" == "zip32 - Win32 Release"

DEP_CPP_NT_C10=\
	"..\..\..\..\wiz\zip\api.h"\
	"..\..\..\..\wiz\zip\tailor.h"\
	"..\..\..\..\wiz\zip\win32\nt.h"\
	"..\..\..\..\wiz\zip\win32\osdep.h"\
	"..\..\..\..\wiz\zip\zip.h"\
	"..\..\..\..\wiz\zip\ziperr.h"\
	

"$(INTDIR)\nt.obj" : $(SOURCE) $(DEP_CPP_NT_C10) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zip32 - Win32 Debug"

DEP_CPP_NT_C10=\
	"..\..\..\..\wiz\zip\api.h"\
	"..\..\..\..\wiz\zip\tailor.h"\
	"..\..\..\..\wiz\zip\win32\nt.h"\
	"..\..\..\..\wiz\zip\win32\osdep.h"\
	"..\..\..\..\wiz\zip\zip.h"\
	"..\..\..\..\wiz\zip\ziperr.h"\
	

"$(INTDIR)\nt.obj" : $(SOURCE) $(DEP_CPP_NT_C10) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\..\wiz\zip\trees.c
DEP_CPP_TREES=\
	"..\..\..\..\wiz\zip\api.h"\
	"..\..\..\..\wiz\zip\tailor.h"\
	"..\..\..\..\wiz\zip\win32\osdep.h"\
	"..\..\..\..\wiz\zip\zip.h"\
	"..\..\..\..\wiz\zip\ziperr.h"\
	

"$(INTDIR)\trees.obj" : $(SOURCE) $(DEP_CPP_TREES) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\wiz\zip\ttyio.c
DEP_CPP_TTYIO=\
	"..\..\..\..\wiz\zip\api.h"\
	"..\..\..\..\wiz\zip\crypt.h"\
	"..\..\..\..\wiz\zip\tailor.h"\
	"..\..\..\..\wiz\zip\ttyio.h"\
	"..\..\..\..\wiz\zip\win32\osdep.h"\
	"..\..\..\..\wiz\zip\zip.h"\
	"..\..\..\..\wiz\zip\ziperr.h"\
	

"$(INTDIR)\ttyio.obj" : $(SOURCE) $(DEP_CPP_TTYIO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\wiz\zip\util.c

!IF  "$(CFG)" == "zip32 - Win32 Release"

DEP_CPP_UTIL_=\
	"..\..\..\..\wiz\zip\api.h"\
	"..\..\..\..\wiz\zip\ebcdic.h"\
	"..\..\..\..\wiz\zip\tailor.h"\
	"..\..\..\..\wiz\zip\win32\osdep.h"\
	"..\..\..\..\wiz\zip\zip.h"\
	"..\..\..\..\wiz\zip\ziperr.h"\
	

"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zip32 - Win32 Debug"

DEP_CPP_UTIL_=\
	"..\..\..\..\wiz\zip\api.h"\
	"..\..\..\..\wiz\zip\ebcdic.h"\
	"..\..\..\..\wiz\zip\tailor.h"\
	"..\..\..\..\wiz\zip\win32\osdep.h"\
	"..\..\..\..\wiz\zip\zip.h"\
	"..\..\..\..\wiz\zip\ziperr.h"\
	

"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\..\wiz\zip\Win32\win32.c

!IF  "$(CFG)" == "zip32 - Win32 Release"

DEP_CPP_WIN32=\
	"..\..\..\..\wiz\zip\api.h"\
	"..\..\..\..\wiz\zip\tailor.h"\
	"..\..\..\..\wiz\zip\win32\osdep.h"\
	"..\..\..\..\wiz\zip\win32\win32zip.h"\
	"..\..\..\..\wiz\zip\zip.h"\
	"..\..\..\..\wiz\zip\ziperr.h"\
	

"$(INTDIR)\win32.obj" : $(SOURCE) $(DEP_CPP_WIN32) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zip32 - Win32 Debug"

DEP_CPP_WIN32=\
	"..\..\..\..\wiz\zip\api.h"\
	"..\..\..\..\wiz\zip\tailor.h"\
	"..\..\..\..\wiz\zip\win32\osdep.h"\
	"..\..\..\..\wiz\zip\win32\win32zip.h"\
	"..\..\..\..\wiz\zip\zip.h"\
	"..\..\..\..\wiz\zip\ziperr.h"\
	

"$(INTDIR)\win32.obj" : $(SOURCE) $(DEP_CPP_WIN32) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\..\wiz\zip\Win32\win32zip.c

!IF  "$(CFG)" == "zip32 - Win32 Release"

DEP_CPP_WIN32Z=\
	"..\..\..\..\wiz\zip\api.h"\
	"..\..\..\..\wiz\zip\tailor.h"\
	"..\..\..\..\wiz\zip\win32\nt.h"\
	"..\..\..\..\wiz\zip\win32\osdep.h"\
	"..\..\..\..\wiz\zip\win32\win32zip.h"\
	"..\..\..\..\wiz\zip\zip.h"\
	"..\..\..\..\wiz\zip\ziperr.h"\
	

"$(INTDIR)\win32zip.obj" : $(SOURCE) $(DEP_CPP_WIN32Z) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zip32 - Win32 Debug"

DEP_CPP_WIN32Z=\
	"..\..\..\..\wiz\zip\api.h"\
	"..\..\..\..\wiz\zip\tailor.h"\
	"..\..\..\..\wiz\zip\win32\nt.h"\
	"..\..\..\..\wiz\zip\win32\osdep.h"\
	"..\..\..\..\wiz\zip\win32\win32zip.h"\
	"..\..\..\..\wiz\zip\zip.h"\
	"..\..\..\..\wiz\zip\ziperr.h"\
	

"$(INTDIR)\win32zip.obj" : $(SOURCE) $(DEP_CPP_WIN32Z) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\..\wiz\zip\windll\windll.c
DEP_CPP_WINDL=\
	"..\..\..\..\wiz\zip\api.h"\
	"..\..\..\..\wiz\zip\tailor.h"\
	"..\..\..\..\wiz\zip\win32\osdep.h"\
	"..\..\..\..\wiz\zip\windll\structs.h"\
	"..\..\..\..\wiz\zip\windll\windll.h"\
	"..\..\..\..\wiz\zip\zip.h"\
	"..\..\..\..\wiz\zip\ziperr.h"\
	

"$(INTDIR)\windll.obj" : $(SOURCE) $(DEP_CPP_WINDL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\wiz\zip\zip.c
DEP_CPP_ZIP_C=\
	"..\..\..\..\wiz\zip\api.h"\
	"..\..\..\..\wiz\zip\crypt.h"\
	"..\..\..\..\wiz\zip\revision.h"\
	"..\..\..\..\wiz\zip\tailor.h"\
	"..\..\..\..\wiz\zip\ttyio.h"\
	"..\..\..\..\wiz\zip\win32\osdep.h"\
	"..\..\..\..\wiz\zip\windll\structs.h"\
	"..\..\..\..\wiz\zip\windll\windll.h"\
	"..\..\..\..\wiz\zip\zip.h"\
	"..\..\..\..\wiz\zip\ziperr.h"\
	

"$(INTDIR)\zip.obj" : $(SOURCE) $(DEP_CPP_ZIP_C) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\wiz\zip\zipfile.c
DEP_CPP_ZIPFI=\
	"..\..\..\..\wiz\zip\api.h"\
	"..\..\..\..\wiz\zip\revision.h"\
	"..\..\..\..\wiz\zip\tailor.h"\
	"..\..\..\..\wiz\zip\win32\osdep.h"\
	"..\..\..\..\wiz\zip\zip.h"\
	"..\..\..\..\wiz\zip\ziperr.h"\
	

"$(INTDIR)\zipfile.obj" : $(SOURCE) $(DEP_CPP_ZIPFI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\wiz\zip\zipup.c
DEP_CPP_ZIPUP=\
	"..\..\..\..\wiz\zip\api.h"\
	"..\..\..\..\wiz\zip\crypt.h"\
	"..\..\..\..\wiz\zip\revision.h"\
	"..\..\..\..\wiz\zip\tailor.h"\
	"..\..\..\..\wiz\zip\win32\osdep.h"\
	"..\..\..\..\wiz\zip\win32\zipup.h"\
	"..\..\..\..\wiz\zip\zip.h"\
	"..\..\..\..\wiz\zip\ziperr.h"\
	

"$(INTDIR)\zipup.obj" : $(SOURCE) $(DEP_CPP_ZIPUP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

