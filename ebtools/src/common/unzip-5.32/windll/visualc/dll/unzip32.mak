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
!MESSAGE "unzip32 - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "unzip32 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "unzip32 - Win32 Release"

OUTDIR=.\..\Release\app
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\..\Release\app
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\unzip32.dll"

!ELSE 

ALL : "$(OUTDIR)\unzip32.dll"

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
	-@erase "$(INTDIR)\windll.res"
	-@erase "$(INTDIR)\zipinfo.obj"
	-@erase "$(OUTDIR)\unzip32.dll"
	-@erase "$(OUTDIR)\unzip32.exp"
	-@erase "$(OUTDIR)\unzip32.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /Zp4 /MT /W3 /GX /O2 /I "d:\wiz\unzip" /I\
 "d:\wiz\unzip\windll" /I "d:\wiz\unzip\win32" /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "WINDLL" /D "USE_EF_UT_TIME" /D "DLL" /Fp"$(INTDIR)\unzip32.pch"\
 /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
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

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\windll.res" /d "NDEBUG" /d "WIN32" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\unzip32.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)\unzip32.pdb" /machine:I386\
 /def:"..\..\..\..\wiz\unzip\windll\windll32.def" /out:"$(OUTDIR)\unzip32.dll"\
 /implib:"$(OUTDIR)\unzip32.lib" 
DEF_FILE= \
	"..\..\..\..\wiz\unzip\windll\windll32.def"
LINK32_OBJS= \
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
	"$(INTDIR)\windll.res" \
	"$(INTDIR)\zipinfo.obj"

"$(OUTDIR)\unzip32.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "unzip32 - Win32 Debug"

OUTDIR=.\..\Debug\app
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\..\Debug\app
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\unzip32.dll"

!ELSE 

ALL : "$(OUTDIR)\unzip32.dll"

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
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(INTDIR)\win32.obj"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(INTDIR)\windll.res"
	-@erase "$(INTDIR)\zipinfo.obj"
	-@erase "$(OUTDIR)\unzip32.dll"
	-@erase "$(OUTDIR)\unzip32.exp"
	-@erase "$(OUTDIR)\unzip32.ilk"
	-@erase "$(OUTDIR)\unzip32.lib"
	-@erase "$(OUTDIR)\unzip32.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /Zp4 /MTd /W3 /Gm /GX /Zi /Od /I "d:\wiz\unzip" /I\
 "d:\wiz\unzip\windll" /I "d:\wiz\unzip\win32" /D "_DEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "WINDLL" /D "USE_EF_UT_TIME" /D "DLL" /Fp"$(INTDIR)\unzip32.pch"\
 /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
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

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\windll.res" /d "_DEBUG" /d "WIN32" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\unzip32.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)\unzip32.pdb" /debug /machine:I386\
 /def:"..\..\..\..\wiz\unzip\windll\windll32.def" /out:"$(OUTDIR)\unzip32.dll"\
 /implib:"$(OUTDIR)\unzip32.lib" /pdbtype:sept 
DEF_FILE= \
	"..\..\..\..\wiz\unzip\windll\windll32.def"
LINK32_OBJS= \
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
	"$(INTDIR)\windll.res" \
	"$(INTDIR)\zipinfo.obj"

"$(OUTDIR)\unzip32.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "unzip32 - Win32 Release" || "$(CFG)" ==\
 "unzip32 - Win32 Debug"
SOURCE=D:\wiz\unzip\api.c

!IF  "$(CFG)" == "unzip32 - Win32 Release"

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


!ELSEIF  "$(CFG)" == "unzip32 - Win32 Debug"

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


!ENDIF 

SOURCE=D:\wiz\unzip\crc32.c

!IF  "$(CFG)" == "unzip32 - Win32 Release"

DEP_CPP_CRC32=\
	"..\..\..\..\wiz\unzip\globals.h"\
	"..\..\..\..\wiz\unzip\unzip.h"\
	"..\..\..\..\wiz\unzip\unzpriv.h"\
	"..\..\..\..\wiz\unzip\win32\w32cfg.h"\
	"..\..\..\..\wiz\unzip\zip.h"\
	

"$(INTDIR)\crc32.obj" : $(SOURCE) $(DEP_CPP_CRC32) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "unzip32 - Win32 Debug"

DEP_CPP_CRC32=\
	"..\..\..\..\wiz\unzip\globals.h"\
	"..\..\..\..\wiz\unzip\unzip.h"\
	"..\..\..\..\wiz\unzip\unzpriv.h"\
	"..\..\..\..\wiz\unzip\win32\w32cfg.h"\
	"..\..\..\..\wiz\unzip\zip.h"\
	

"$(INTDIR)\crc32.obj" : $(SOURCE) $(DEP_CPP_CRC32) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=D:\wiz\unzip\crctab.c

!IF  "$(CFG)" == "unzip32 - Win32 Release"

DEP_CPP_CRCTA=\
	"..\..\..\..\wiz\unzip\globals.h"\
	"..\..\..\..\wiz\unzip\unzip.h"\
	"..\..\..\..\wiz\unzip\unzpriv.h"\
	"..\..\..\..\wiz\unzip\win32\w32cfg.h"\
	"..\..\..\..\wiz\unzip\zip.h"\
	

"$(INTDIR)\crctab.obj" : $(SOURCE) $(DEP_CPP_CRCTA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "unzip32 - Win32 Debug"

DEP_CPP_CRCTA=\
	"..\..\..\..\wiz\unzip\globals.h"\
	"..\..\..\..\wiz\unzip\unzip.h"\
	"..\..\..\..\wiz\unzip\unzpriv.h"\
	"..\..\..\..\wiz\unzip\win32\w32cfg.h"\
	"..\..\..\..\wiz\unzip\zip.h"\
	

"$(INTDIR)\crctab.obj" : $(SOURCE) $(DEP_CPP_CRCTA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=D:\wiz\unzip\crypt.c

"$(INTDIR)\crypt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=D:\wiz\unzip\explode.c
DEP_CPP_EXPLO=\
	"..\..\..\..\wiz\unzip\globals.h"\
	"..\..\..\..\wiz\unzip\unzip.h"\
	"..\..\..\..\wiz\unzip\unzpriv.h"\
	"..\..\..\..\wiz\unzip\win32\w32cfg.h"\
	

"$(INTDIR)\explode.obj" : $(SOURCE) $(DEP_CPP_EXPLO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=D:\wiz\unzip\extract.c

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

SOURCE=D:\wiz\unzip\fileio.c

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

SOURCE=D:\wiz\unzip\globals.c
DEP_CPP_GLOBA=\
	"..\..\..\..\wiz\unzip\globals.h"\
	"..\..\..\..\wiz\unzip\unzip.h"\
	"..\..\..\..\wiz\unzip\unzpriv.h"\
	"..\..\..\..\wiz\unzip\win32\w32cfg.h"\
	

"$(INTDIR)\globals.obj" : $(SOURCE) $(DEP_CPP_GLOBA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=D:\wiz\unzip\inflate.c

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

SOURCE=D:\wiz\unzip\list.c

!IF  "$(CFG)" == "unzip32 - Win32 Release"

DEP_CPP_LIST_=\
	"..\..\..\..\wiz\unzip\globals.h"\
	"..\..\..\..\wiz\unzip\unzip.h"\
	"..\..\..\..\wiz\unzip\unzpriv.h"\
	"..\..\..\..\wiz\unzip\win32\w32cfg.h"\
	"..\..\..\..\wiz\unzip\windll\structs.h"\
	"..\..\..\..\wiz\unzip\windll\windll.h"\
	

"$(INTDIR)\list.obj" : $(SOURCE) $(DEP_CPP_LIST_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "unzip32 - Win32 Debug"

DEP_CPP_LIST_=\
	"..\..\..\..\wiz\unzip\globals.h"\
	"..\..\..\..\wiz\unzip\unzip.h"\
	"..\..\..\..\wiz\unzip\unzpriv.h"\
	"..\..\..\..\wiz\unzip\win32\w32cfg.h"\
	"..\..\..\..\wiz\unzip\windll\structs.h"\
	"..\..\..\..\wiz\unzip\windll\windll.h"\
	

"$(INTDIR)\list.obj" : $(SOURCE) $(DEP_CPP_LIST_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=D:\wiz\unzip\match.c
DEP_CPP_MATCH=\
	"..\..\..\..\wiz\unzip\globals.h"\
	"..\..\..\..\wiz\unzip\unzip.h"\
	"..\..\..\..\wiz\unzip\unzpriv.h"\
	"..\..\..\..\wiz\unzip\win32\w32cfg.h"\
	

"$(INTDIR)\match.obj" : $(SOURCE) $(DEP_CPP_MATCH) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=D:\wiz\unzip\Win32\nt.c

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

SOURCE=D:\wiz\unzip\process.c

!IF  "$(CFG)" == "unzip32 - Win32 Release"

DEP_CPP_PROCE=\
	"..\..\..\..\wiz\unzip\globals.h"\
	"..\..\..\..\wiz\unzip\unzip.h"\
	"..\..\..\..\wiz\unzip\unzpriv.h"\
	"..\..\..\..\wiz\unzip\win32\w32cfg.h"\
	"..\..\..\..\wiz\unzip\windll\structs.h"\
	"..\..\..\..\wiz\unzip\windll\windll.h"\
	

"$(INTDIR)\process.obj" : $(SOURCE) $(DEP_CPP_PROCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "unzip32 - Win32 Debug"

DEP_CPP_PROCE=\
	"..\..\..\..\wiz\unzip\globals.h"\
	"..\..\..\..\wiz\unzip\unzip.h"\
	"..\..\..\..\wiz\unzip\unzpriv.h"\
	"..\..\..\..\wiz\unzip\win32\w32cfg.h"\
	"..\..\..\..\wiz\unzip\windll\structs.h"\
	"..\..\..\..\wiz\unzip\windll\windll.h"\
	

"$(INTDIR)\process.obj" : $(SOURCE) $(DEP_CPP_PROCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=D:\wiz\unzip\ttyio.c

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

SOURCE=D:\wiz\unzip\unreduce.c
DEP_CPP_UNRED=\
	"..\..\..\..\wiz\unzip\globals.h"\
	"..\..\..\..\wiz\unzip\unzip.h"\
	"..\..\..\..\wiz\unzip\unzpriv.h"\
	"..\..\..\..\wiz\unzip\win32\w32cfg.h"\
	

"$(INTDIR)\unreduce.obj" : $(SOURCE) $(DEP_CPP_UNRED) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=D:\wiz\unzip\unshrink.c
DEP_CPP_UNSHR=\
	"..\..\..\..\wiz\unzip\globals.h"\
	"..\..\..\..\wiz\unzip\unzip.h"\
	"..\..\..\..\wiz\unzip\unzpriv.h"\
	"..\..\..\..\wiz\unzip\win32\w32cfg.h"\
	

"$(INTDIR)\unshrink.obj" : $(SOURCE) $(DEP_CPP_UNSHR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=D:\wiz\unzip\Win32\win32.c

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

SOURCE=D:\wiz\unzip\windll\windll.c

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

SOURCE=D:\wiz\unzip\windll\windll.rc

!IF  "$(CFG)" == "unzip32 - Win32 Release"


"$(INTDIR)\windll.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\windll.res" /i "\wiz\unzip\windll" /d "NDEBUG"\
 /d "WIN32" $(SOURCE)


!ELSEIF  "$(CFG)" == "unzip32 - Win32 Debug"


"$(INTDIR)\windll.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\windll.res" /i "\wiz\unzip\windll" /d "_DEBUG"\
 /d "WIN32" $(SOURCE)


!ENDIF 

SOURCE=D:\wiz\unzip\zipinfo.c
DEP_CPP_ZIPIN=\
	"..\..\..\..\wiz\unzip\globals.h"\
	"..\..\..\..\wiz\unzip\unzip.h"\
	"..\..\..\..\wiz\unzip\unzpriv.h"\
	"..\..\..\..\wiz\unzip\win32\w32cfg.h"\
	

"$(INTDIR)\zipinfo.obj" : $(SOURCE) $(DEP_CPP_ZIPIN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

