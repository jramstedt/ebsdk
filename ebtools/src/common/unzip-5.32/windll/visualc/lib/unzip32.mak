# Microsoft Developer Studio Generated NMAKE File, Based on unzip32.dsp
!IF "$(CFG)" == ""
CFG=unzip32 - Win32 Debug
!MESSAGE No configuration specified. Defaulting to unzip32 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "unzip32 - Win32 Release" && "$(CFG)" !=\
 "unzip32 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "unzip32.mak" CFG="unzip32 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "unzip32 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "unzip32 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "unzip32 - Win32 Release"

OUTDIR=.\..\Release\libs
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\..\Release\libs
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\unzip32.lib"

!ELSE 

ALL : "$(OUTDIR)\unzip32.lib"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\api.obj"
	-@erase "$(INTDIR)\crc32.obj"
	-@erase "$(INTDIR)\crctab.obj"
	-@erase "$(INTDIR)\crypt.obj"
	-@erase "$(INTDIR)\explode.obj"
	-@erase "$(INTDIR)\extract.obj"
	-@erase "$(INTDIR)\fileio.obj"
	-@erase "$(INTDIR)\globals.obj"
	-@erase "$(INTDIR)\inflate.obj"
	-@erase "$(INTDIR)\list.obj"
	-@erase "$(INTDIR)\match.obj"
	-@erase "$(INTDIR)\nt.obj"
	-@erase "$(INTDIR)\process.obj"
	-@erase "$(INTDIR)\ttyio.obj"
	-@erase "$(INTDIR)\unreduce.obj"
	-@erase "$(INTDIR)\unshrink.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\win32.obj"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(INTDIR)\zipinfo.obj"
	-@erase "$(OUTDIR)\unzip32.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "D:\WIZ\UNZIP" /I "D:\WIZ\UNZIP\WINDLL" /I\
 "D:\WIZ\UNZIP\WIN32" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WINDLL" /D\
 "USE_EF_UT_TIME" /D "DLL" /D "UNZIPLIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD\
 /c 
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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\unzip32.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\unzip32.lib" 
LIB32_OBJS= \
	"$(INTDIR)\api.obj" \
	"$(INTDIR)\crc32.obj" \
	"$(INTDIR)\crctab.obj" \
	"$(INTDIR)\crypt.obj" \
	"$(INTDIR)\explode.obj" \
	"$(INTDIR)\extract.obj" \
	"$(INTDIR)\fileio.obj" \
	"$(INTDIR)\globals.obj" \
	"$(INTDIR)\inflate.obj" \
	"$(INTDIR)\list.obj" \
	"$(INTDIR)\match.obj" \
	"$(INTDIR)\nt.obj" \
	"$(INTDIR)\process.obj" \
	"$(INTDIR)\ttyio.obj" \
	"$(INTDIR)\unreduce.obj" \
	"$(INTDIR)\unshrink.obj" \
	"$(INTDIR)\win32.obj" \
	"$(INTDIR)\windll.obj" \
	"$(INTDIR)\zipinfo.obj"

"$(OUTDIR)\unzip32.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "unzip32 - Win32 Debug"

OUTDIR=.\..\Debug\libs
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\..\Debug\libs
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\unzip32.lib"

!ELSE 

ALL : "$(OUTDIR)\unzip32.lib"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\api.obj"
	-@erase "$(INTDIR)\crc32.obj"
	-@erase "$(INTDIR)\crctab.obj"
	-@erase "$(INTDIR)\crypt.obj"
	-@erase "$(INTDIR)\explode.obj"
	-@erase "$(INTDIR)\extract.obj"
	-@erase "$(INTDIR)\fileio.obj"
	-@erase "$(INTDIR)\globals.obj"
	-@erase "$(INTDIR)\inflate.obj"
	-@erase "$(INTDIR)\list.obj"
	-@erase "$(INTDIR)\match.obj"
	-@erase "$(INTDIR)\nt.obj"
	-@erase "$(INTDIR)\process.obj"
	-@erase "$(INTDIR)\ttyio.obj"
	-@erase "$(INTDIR)\unreduce.obj"
	-@erase "$(INTDIR)\unshrink.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\win32.obj"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(INTDIR)\zipinfo.obj"
	-@erase "$(OUTDIR)\unzip32.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /GX /Z7 /Od /I "D:\WIZ\UNZIP" /I\
 "D:\WIZ\UNZIP\WINDLL" /I "D:\WIZ\UNZIP\WIN32" /D "_DEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "WINDLL" /D "USE_EF_UT_TIME" /D "DLL" /D "UNZIPLIB"\
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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\unzip32.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\unzip32.lib" 
LIB32_OBJS= \
	"$(INTDIR)\api.obj" \
	"$(INTDIR)\crc32.obj" \
	"$(INTDIR)\crctab.obj" \
	"$(INTDIR)\crypt.obj" \
	"$(INTDIR)\explode.obj" \
	"$(INTDIR)\extract.obj" \
	"$(INTDIR)\fileio.obj" \
	"$(INTDIR)\globals.obj" \
	"$(INTDIR)\inflate.obj" \
	"$(INTDIR)\list.obj" \
	"$(INTDIR)\match.obj" \
	"$(INTDIR)\nt.obj" \
	"$(INTDIR)\process.obj" \
	"$(INTDIR)\ttyio.obj" \
	"$(INTDIR)\unreduce.obj" \
	"$(INTDIR)\unshrink.obj" \
	"$(INTDIR)\win32.obj" \
	"$(INTDIR)\windll.obj" \
	"$(INTDIR)\zipinfo.obj"

"$(OUTDIR)\unzip32.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "unzip32 - Win32 Release" || "$(CFG)" ==\
 "unzip32 - Win32 Debug"
SOURCE=..\..\..\..\wiz\unzip\api.c
DEP_CPP_API_C=\
	"..\..\..\..\wiz\unzip\globals.h"\
	"..\..\..\..\wiz\unzip\unzip.h"\
	"..\..\..\..\wiz\unzip\unzpriv.h"\
	"..\..\..\..\wiz\unzip\version.h"\
	"..\..\..\..\wiz\unzip\win32\w32cfg.h"\
	"..\..\..\..\wiz\unzip\windll\structs.h"\
	"..\..\..\..\wiz\unzip\windll\windll.h"\
	

"$(INTDIR)\api.obj" : $(SOURCE) $(DEP_CPP_API_C) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\wiz\unzip\crc32.c
DEP_CPP_CRC32=\
	"..\..\..\..\wiz\unzip\globals.h"\
	"..\..\..\..\wiz\unzip\unzip.h"\
	"..\..\..\..\wiz\unzip\unzpriv.h"\
	"..\..\..\..\wiz\unzip\win32\w32cfg.h"\
	"..\..\..\..\wiz\unzip\zip.h"\
	

"$(INTDIR)\crc32.obj" : $(SOURCE) $(DEP_CPP_CRC32) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\wiz\unzip\crctab.c
DEP_CPP_CRCTA=\
	"..\..\..\..\wiz\unzip\globals.h"\
	"..\..\..\..\wiz\unzip\unzip.h"\
	"..\..\..\..\wiz\unzip\unzpriv.h"\
	"..\..\..\..\wiz\unzip\win32\w32cfg.h"\
	"..\..\..\..\wiz\unzip\zip.h"\
	

"$(INTDIR)\crctab.obj" : $(SOURCE) $(DEP_CPP_CRCTA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\wiz\unzip\crypt.c

"$(INTDIR)\crypt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\wiz\unzip\explode.c
DEP_CPP_EXPLO=\
	"..\..\..\..\wiz\unzip\globals.h"\
	"..\..\..\..\wiz\unzip\unzip.h"\
	"..\..\..\..\wiz\unzip\unzpriv.h"\
	"..\..\..\..\wiz\unzip\win32\w32cfg.h"\
	

"$(INTDIR)\explode.obj" : $(SOURCE) $(DEP_CPP_EXPLO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\wiz\unzip\extract.c

!IF  "$(CFG)" == "unzip32 - Win32 Release"

DEP_CPP_EXTRA=\
	"..\..\..\..\wiz\unzip\crypt.h"\
	"..\..\..\..\wiz\unzip\globals.h"\
	"..\..\..\..\wiz\unzip\unzip.h"\
	"..\..\..\..\wiz\unzip\unzpriv.h"\
	"..\..\..\..\wiz\unzip\win32\w32cfg.h"\
	"..\..\..\..\wiz\unzip\windll\structs.h"\
	"..\..\..\..\wiz\unzip\windll\windll.h"\
	

"$(INTDIR)\extract.obj" : $(SOURCE) $(DEP_CPP_EXTRA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "unzip32 - Win32 Debug"

DEP_CPP_EXTRA=\
	"..\..\..\..\wiz\unzip\crypt.h"\
	"..\..\..\..\wiz\unzip\globals.h"\
	"..\..\..\..\wiz\unzip\unzip.h"\
	"..\..\..\..\wiz\unzip\unzpriv.h"\
	"..\..\..\..\wiz\unzip\win32\w32cfg.h"\
	"..\..\..\..\wiz\unzip\windll\structs.h"\
	"..\..\..\..\wiz\unzip\windll\windll.h"\
	

"$(INTDIR)\extract.obj" : $(SOURCE) $(DEP_CPP_EXTRA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\..\wiz\unzip\fileio.c

!IF  "$(CFG)" == "unzip32 - Win32 Release"

DEP_CPP_FILEI=\
	"..\..\..\..\wiz\unzip\crypt.h"\
	"..\..\..\..\wiz\unzip\ebcdic.h"\
	"..\..\..\..\wiz\unzip\globals.h"\
	"..\..\..\..\wiz\unzip\ttyio.h"\
	"..\..\..\..\wiz\unzip\unzip.h"\
	"..\..\..\..\wiz\unzip\unzpriv.h"\
	"..\..\..\..\wiz\unzip\win32\w32cfg.h"\
	

"$(INTDIR)\fileio.obj" : $(SOURCE) $(DEP_CPP_FILEI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "unzip32 - Win32 Debug"

DEP_CPP_FILEI=\
	"..\..\..\..\wiz\unzip\crypt.h"\
	"..\..\..\..\wiz\unzip\ebcdic.h"\
	"..\..\..\..\wiz\unzip\globals.h"\
	"..\..\..\..\wiz\unzip\ttyio.h"\
	"..\..\..\..\wiz\unzip\unzip.h"\
	"..\..\..\..\wiz\unzip\unzpriv.h"\
	"..\..\..\..\wiz\unzip\win32\w32cfg.h"\
	

"$(INTDIR)\fileio.obj" : $(SOURCE) $(DEP_CPP_FILEI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\..\wiz\unzip\globals.c
DEP_CPP_GLOBA=\
	"..\..\..\..\wiz\unzip\globals.h"\
	"..\..\..\..\wiz\unzip\unzip.h"\
	"..\..\..\..\wiz\unzip\unzpriv.h"\
	"..\..\..\..\wiz\unzip\win32\w32cfg.h"\
	

"$(INTDIR)\globals.obj" : $(SOURCE) $(DEP_CPP_GLOBA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\wiz\unzip\inflate.c

!IF  "$(CFG)" == "unzip32 - Win32 Release"

DEP_CPP_INFLA=\
	"..\..\..\..\wiz\unzip\globals.h"\
	"..\..\..\..\wiz\unzip\inflate.h"\
	"..\..\..\..\wiz\unzip\unzip.h"\
	"..\..\..\..\wiz\unzip\unzpriv.h"\
	"..\..\..\..\wiz\unzip\win32\w32cfg.h"\
	

"$(INTDIR)\inflate.obj" : $(SOURCE) $(DEP_CPP_INFLA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "unzip32 - Win32 Debug"

DEP_CPP_INFLA=\
	"..\..\..\..\wiz\unzip\globals.h"\
	"..\..\..\..\wiz\unzip\inflate.h"\
	"..\..\..\..\wiz\unzip\unzip.h"\
	"..\..\..\..\wiz\unzip\unzpriv.h"\
	"..\..\..\..\wiz\unzip\win32\w32cfg.h"\
	

"$(INTDIR)\inflate.obj" : $(SOURCE) $(DEP_CPP_INFLA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\..\wiz\unzip\list.c
DEP_CPP_LIST_=\
	"..\..\..\..\wiz\unzip\globals.h"\
	"..\..\..\..\wiz\unzip\unzip.h"\
	"..\..\..\..\wiz\unzip\unzpriv.h"\
	"..\..\..\..\wiz\unzip\win32\w32cfg.h"\
	"..\..\..\..\wiz\unzip\windll\structs.h"\
	"..\..\..\..\wiz\unzip\windll\windll.h"\
	

"$(INTDIR)\list.obj" : $(SOURCE) $(DEP_CPP_LIST_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\wiz\unzip\match.c
DEP_CPP_MATCH=\
	"..\..\..\..\wiz\unzip\globals.h"\
	"..\..\..\..\wiz\unzip\unzip.h"\
	"..\..\..\..\wiz\unzip\unzpriv.h"\
	"..\..\..\..\wiz\unzip\win32\w32cfg.h"\
	

"$(INTDIR)\match.obj" : $(SOURCE) $(DEP_CPP_MATCH) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\wiz\unzip\Win32\nt.c

!IF  "$(CFG)" == "unzip32 - Win32 Release"

DEP_CPP_NT_C14=\
	"..\..\..\..\wiz\unzip\globals.h"\
	"..\..\..\..\wiz\unzip\unzip.h"\
	"..\..\..\..\wiz\unzip\unzpriv.h"\
	"..\..\..\..\wiz\unzip\win32\nt.h"\
	"..\..\..\..\wiz\unzip\win32\w32cfg.h"\
	

"$(INTDIR)\nt.obj" : $(SOURCE) $(DEP_CPP_NT_C14) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "unzip32 - Win32 Debug"

DEP_CPP_NT_C14=\
	"..\..\..\..\wiz\unzip\globals.h"\
	"..\..\..\..\wiz\unzip\unzip.h"\
	"..\..\..\..\wiz\unzip\unzpriv.h"\
	"..\..\..\..\wiz\unzip\win32\nt.h"\
	"..\..\..\..\wiz\unzip\win32\w32cfg.h"\
	

"$(INTDIR)\nt.obj" : $(SOURCE) $(DEP_CPP_NT_C14) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\..\wiz\unzip\process.c
DEP_CPP_PROCE=\
	"..\..\..\..\wiz\unzip\globals.h"\
	"..\..\..\..\wiz\unzip\unzip.h"\
	"..\..\..\..\wiz\unzip\unzpriv.h"\
	"..\..\..\..\wiz\unzip\win32\w32cfg.h"\
	"..\..\..\..\wiz\unzip\windll\structs.h"\
	"..\..\..\..\wiz\unzip\windll\windll.h"\
	

"$(INTDIR)\process.obj" : $(SOURCE) $(DEP_CPP_PROCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\wiz\unzip\ttyio.c

!IF  "$(CFG)" == "unzip32 - Win32 Release"

DEP_CPP_TTYIO=\
	"..\..\..\..\wiz\unzip\crypt.h"\
	"..\..\..\..\wiz\unzip\globals.h"\
	"..\..\..\..\wiz\unzip\ttyio.h"\
	"..\..\..\..\wiz\unzip\unzip.h"\
	"..\..\..\..\wiz\unzip\unzpriv.h"\
	"..\..\..\..\wiz\unzip\win32\w32cfg.h"\
	"..\..\..\..\wiz\unzip\zip.h"\
	

"$(INTDIR)\ttyio.obj" : $(SOURCE) $(DEP_CPP_TTYIO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "unzip32 - Win32 Debug"

DEP_CPP_TTYIO=\
	"..\..\..\..\wiz\unzip\crypt.h"\
	"..\..\..\..\wiz\unzip\globals.h"\
	"..\..\..\..\wiz\unzip\ttyio.h"\
	"..\..\..\..\wiz\unzip\unzip.h"\
	"..\..\..\..\wiz\unzip\unzpriv.h"\
	"..\..\..\..\wiz\unzip\win32\w32cfg.h"\
	"..\..\..\..\wiz\unzip\zip.h"\
	

"$(INTDIR)\ttyio.obj" : $(SOURCE) $(DEP_CPP_TTYIO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\..\wiz\unzip\unreduce.c
DEP_CPP_UNRED=\
	"..\..\..\..\wiz\unzip\globals.h"\
	"..\..\..\..\wiz\unzip\unzip.h"\
	"..\..\..\..\wiz\unzip\unzpriv.h"\
	"..\..\..\..\wiz\unzip\win32\w32cfg.h"\
	

"$(INTDIR)\unreduce.obj" : $(SOURCE) $(DEP_CPP_UNRED) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\wiz\unzip\unshrink.c
DEP_CPP_UNSHR=\
	"..\..\..\..\wiz\unzip\globals.h"\
	"..\..\..\..\wiz\unzip\unzip.h"\
	"..\..\..\..\wiz\unzip\unzpriv.h"\
	"..\..\..\..\wiz\unzip\win32\w32cfg.h"\
	

"$(INTDIR)\unshrink.obj" : $(SOURCE) $(DEP_CPP_UNSHR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\wiz\unzip\Win32\win32.c

!IF  "$(CFG)" == "unzip32 - Win32 Release"

DEP_CPP_WIN32=\
	"..\..\..\..\wiz\unzip\globals.h"\
	"..\..\..\..\wiz\unzip\unzip.h"\
	"..\..\..\..\wiz\unzip\unzpriv.h"\
	"..\..\..\..\wiz\unzip\win32\nt.h"\
	"..\..\..\..\wiz\unzip\win32\w32cfg.h"\
	

"$(INTDIR)\win32.obj" : $(SOURCE) $(DEP_CPP_WIN32) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "unzip32 - Win32 Debug"

DEP_CPP_WIN32=\
	"..\..\..\..\wiz\unzip\globals.h"\
	"..\..\..\..\wiz\unzip\unzip.h"\
	"..\..\..\..\wiz\unzip\unzpriv.h"\
	"..\..\..\..\wiz\unzip\win32\nt.h"\
	"..\..\..\..\wiz\unzip\win32\w32cfg.h"\
	

"$(INTDIR)\win32.obj" : $(SOURCE) $(DEP_CPP_WIN32) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\..\wiz\unzip\windll\windll.c

!IF  "$(CFG)" == "unzip32 - Win32 Release"

DEP_CPP_WINDL=\
	"..\..\..\..\wiz\unzip\consts.h"\
	"..\..\..\..\wiz\unzip\crypt.h"\
	"..\..\..\..\wiz\unzip\globals.h"\
	"..\..\..\..\wiz\unzip\unzip.h"\
	"..\..\..\..\wiz\unzip\unzpriv.h"\
	"..\..\..\..\wiz\unzip\version.h"\
	"..\..\..\..\wiz\unzip\win32\w32cfg.h"\
	"..\..\..\..\wiz\unzip\windll\structs.h"\
	"..\..\..\..\wiz\unzip\windll\windll.h"\
	

"$(INTDIR)\windll.obj" : $(SOURCE) $(DEP_CPP_WINDL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "unzip32 - Win32 Debug"

DEP_CPP_WINDL=\
	"..\..\..\..\wiz\unzip\consts.h"\
	"..\..\..\..\wiz\unzip\crypt.h"\
	"..\..\..\..\wiz\unzip\globals.h"\
	"..\..\..\..\wiz\unzip\unzip.h"\
	"..\..\..\..\wiz\unzip\unzpriv.h"\
	"..\..\..\..\wiz\unzip\version.h"\
	"..\..\..\..\wiz\unzip\win32\w32cfg.h"\
	"..\..\..\..\wiz\unzip\windll\structs.h"\
	"..\..\..\..\wiz\unzip\windll\windll.h"\
	

"$(INTDIR)\windll.obj" : $(SOURCE) $(DEP_CPP_WINDL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\..\wiz\unzip\zipinfo.c
DEP_CPP_ZIPIN=\
	"..\..\..\..\wiz\unzip\globals.h"\
	"..\..\..\..\wiz\unzip\unzip.h"\
	"..\..\..\..\wiz\unzip\unzpriv.h"\
	"..\..\..\..\wiz\unzip\win32\w32cfg.h"\
	

"$(INTDIR)\zipinfo.obj" : $(SOURCE) $(DEP_CPP_ZIPIN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

