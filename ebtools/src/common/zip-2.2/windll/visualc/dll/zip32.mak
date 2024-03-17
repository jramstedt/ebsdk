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
!MESSAGE "zip32 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "zip32 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "zip32 - Win32 Release"

OUTDIR=.\..\Release\app
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\..\Release\app
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\zip32.dll"

!ELSE 

ALL : "$(OUTDIR)\zip32.dll"

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
	-@erase "$(INTDIR)\windll.res"
	-@erase "$(INTDIR)\zip.obj"
	-@erase "$(INTDIR)\zipfile.obj"
	-@erase "$(INTDIR)\zipup.obj"
	-@erase "$(OUTDIR)\zip32.dll"
	-@erase "$(OUTDIR)\zip32.exp"
	-@erase "$(OUTDIR)\zip32.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /Zp4 /MT /W3 /GX /O2 /I "D:\WIZ\ZIP" /I "D:\WIZ\ZIP\WINDLL" /I\
 "D:\WIZ\WIN32" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "NO_ASM" /D "WINDLL" /D\
 "MSDOS" /D "USE_EF_UX_TIME" /D "USE_ZIPMAIN" /Fp"$(INTDIR)\zip32.pch" /YX\
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

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\windll.res" /d "NDEBUG" /d "WIN32" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\zip32.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)\zip32.pdb" /machine:I386\
 /def:"..\..\..\..\wiz\zip\windll\windll32.def" /out:"$(OUTDIR)\zip32.dll"\
 /implib:"$(OUTDIR)\zip32.lib" 
DEF_FILE= \
	"..\..\..\..\wiz\zip\windll\windll32.def"
LINK32_OBJS= \
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
	"$(INTDIR)\windll.res" \
	"$(INTDIR)\zip.obj" \
	"$(INTDIR)\zipfile.obj" \
	"$(INTDIR)\zipup.obj"

"$(OUTDIR)\zip32.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "zip32 - Win32 Debug"

OUTDIR=.\..\Debug\app
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\..\Debug\app
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\zip32.dll"

!ELSE 

ALL : "$(OUTDIR)\zip32.dll"

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
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(INTDIR)\win32.obj"
	-@erase "$(INTDIR)\win32zip.obj"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(INTDIR)\windll.res"
	-@erase "$(INTDIR)\zip.obj"
	-@erase "$(INTDIR)\zipfile.obj"
	-@erase "$(INTDIR)\zipup.obj"
	-@erase "$(OUTDIR)\zip32.dll"
	-@erase "$(OUTDIR)\zip32.exp"
	-@erase "$(OUTDIR)\zip32.ilk"
	-@erase "$(OUTDIR)\zip32.lib"
	-@erase "$(OUTDIR)\zip32.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /Zp4 /MTd /W3 /Gm /GX /Zi /Od /I "D:\WIZ\ZIP" /I\
 "D:\WIZ\ZIP\WINDLL" /I "D:\WIZ\WIN32" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "NO_ASM" /D "WINDLL" /D "MSDOS" /D "USE_EF_UX_TIME" /D "USE_ZIPMAIN"\
 /Fp"$(INTDIR)\zip32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\zip32.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)\zip32.pdb" /debug /machine:I386\
 /def:"..\..\..\..\wiz\zip\windll\windll32.def" /out:"$(OUTDIR)\zip32.dll"\
 /implib:"$(OUTDIR)\zip32.lib" /pdbtype:sept 
DEF_FILE= \
	"..\..\..\..\wiz\zip\windll\windll32.def"
LINK32_OBJS= \
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
	"$(INTDIR)\windll.res" \
	"$(INTDIR)\zip.obj" \
	"$(INTDIR)\zipfile.obj" \
	"$(INTDIR)\zipup.obj"

"$(OUTDIR)\zip32.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "zip32 - Win32 Release" || "$(CFG)" == "zip32 - Win32 Debug"
SOURCE=D:\wiz\zip\api.c
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


SOURCE=D:\wiz\zip\bits.c

!IF  "$(CFG)" == "zip32 - Win32 Release"

DEP_CPP_BITS_=\
	"..\..\..\..\wiz\zip\api.h"\
	"..\..\..\..\wiz\zip\crypt.h"\
	"..\..\..\..\wiz\zip\tailor.h"\
	"..\..\..\..\wiz\zip\win32\osdep.h"\
	"..\..\..\..\wiz\zip\zip.h"\
	"..\..\..\..\wiz\zip\ziperr.h"\
	

"$(INTDIR)\bits.obj" : $(SOURCE) $(DEP_CPP_BITS_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zip32 - Win32 Debug"

DEP_CPP_BITS_=\
	"..\..\..\..\wiz\zip\api.h"\
	"..\..\..\..\wiz\zip\crypt.h"\
	"..\..\..\..\wiz\zip\tailor.h"\
	"..\..\..\..\wiz\zip\win32\osdep.h"\
	"..\..\..\..\wiz\zip\zip.h"\
	"..\..\..\..\wiz\zip\ziperr.h"\
	

"$(INTDIR)\bits.obj" : $(SOURCE) $(DEP_CPP_BITS_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=D:\wiz\zip\crc32.c

!IF  "$(CFG)" == "zip32 - Win32 Release"

DEP_CPP_CRC32=\
	"..\..\..\..\wiz\zip\api.h"\
	"..\..\..\..\wiz\zip\tailor.h"\
	"..\..\..\..\wiz\zip\win32\osdep.h"\
	"..\..\..\..\wiz\zip\zip.h"\
	"..\..\..\..\wiz\zip\ziperr.h"\
	

"$(INTDIR)\crc32.obj" : $(SOURCE) $(DEP_CPP_CRC32) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zip32 - Win32 Debug"

DEP_CPP_CRC32=\
	"..\..\..\..\wiz\zip\api.h"\
	"..\..\..\..\wiz\zip\tailor.h"\
	"..\..\..\..\wiz\zip\win32\osdep.h"\
	"..\..\..\..\wiz\zip\zip.h"\
	"..\..\..\..\wiz\zip\ziperr.h"\
	

"$(INTDIR)\crc32.obj" : $(SOURCE) $(DEP_CPP_CRC32) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=D:\wiz\zip\crctab.c

!IF  "$(CFG)" == "zip32 - Win32 Release"

DEP_CPP_CRCTA=\
	"..\..\..\..\wiz\zip\api.h"\
	"..\..\..\..\wiz\zip\tailor.h"\
	"..\..\..\..\wiz\zip\win32\osdep.h"\
	"..\..\..\..\wiz\zip\zip.h"\
	"..\..\..\..\wiz\zip\ziperr.h"\
	

"$(INTDIR)\crctab.obj" : $(SOURCE) $(DEP_CPP_CRCTA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zip32 - Win32 Debug"

DEP_CPP_CRCTA=\
	"..\..\..\..\wiz\zip\api.h"\
	"..\..\..\..\wiz\zip\tailor.h"\
	"..\..\..\..\wiz\zip\win32\osdep.h"\
	"..\..\..\..\wiz\zip\zip.h"\
	"..\..\..\..\wiz\zip\ziperr.h"\
	

"$(INTDIR)\crctab.obj" : $(SOURCE) $(DEP_CPP_CRCTA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=D:\wiz\zip\crypt.c

!IF  "$(CFG)" == "zip32 - Win32 Release"

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


!ELSEIF  "$(CFG)" == "zip32 - Win32 Debug"

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


!ENDIF 

SOURCE=D:\wiz\zip\deflate.c

!IF  "$(CFG)" == "zip32 - Win32 Release"

DEP_CPP_DEFLA=\
	"..\..\..\..\wiz\zip\api.h"\
	"..\..\..\..\wiz\zip\tailor.h"\
	"..\..\..\..\wiz\zip\win32\osdep.h"\
	"..\..\..\..\wiz\zip\zip.h"\
	"..\..\..\..\wiz\zip\ziperr.h"\
	

"$(INTDIR)\deflate.obj" : $(SOURCE) $(DEP_CPP_DEFLA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zip32 - Win32 Debug"

DEP_CPP_DEFLA=\
	"..\..\..\..\wiz\zip\api.h"\
	"..\..\..\..\wiz\zip\tailor.h"\
	"..\..\..\..\wiz\zip\win32\osdep.h"\
	"..\..\..\..\wiz\zip\zip.h"\
	"..\..\..\..\wiz\zip\ziperr.h"\
	

"$(INTDIR)\deflate.obj" : $(SOURCE) $(DEP_CPP_DEFLA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=D:\wiz\zip\fileio.c

!IF  "$(CFG)" == "zip32 - Win32 Release"

DEP_CPP_FILEI=\
	"..\..\..\..\wiz\zip\api.h"\
	"..\..\..\..\wiz\zip\tailor.h"\
	"..\..\..\..\wiz\zip\win32\osdep.h"\
	"..\..\..\..\wiz\zip\zip.h"\
	"..\..\..\..\wiz\zip\ziperr.h"\
	

"$(INTDIR)\fileio.obj" : $(SOURCE) $(DEP_CPP_FILEI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zip32 - Win32 Debug"

DEP_CPP_FILEI=\
	"..\..\..\..\wiz\zip\api.h"\
	"..\..\..\..\wiz\zip\tailor.h"\
	"..\..\..\..\wiz\zip\win32\osdep.h"\
	"..\..\..\..\wiz\zip\zip.h"\
	"..\..\..\..\wiz\zip\ziperr.h"\
	

"$(INTDIR)\fileio.obj" : $(SOURCE) $(DEP_CPP_FILEI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=D:\wiz\zip\globals.c

!IF  "$(CFG)" == "zip32 - Win32 Release"

DEP_CPP_GLOBA=\
	"..\..\..\..\wiz\zip\api.h"\
	"..\..\..\..\wiz\zip\tailor.h"\
	"..\..\..\..\wiz\zip\win32\osdep.h"\
	"..\..\..\..\wiz\zip\zip.h"\
	"..\..\..\..\wiz\zip\ziperr.h"\
	

"$(INTDIR)\globals.obj" : $(SOURCE) $(DEP_CPP_GLOBA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zip32 - Win32 Debug"

DEP_CPP_GLOBA=\
	"..\..\..\..\wiz\zip\api.h"\
	"..\..\..\..\wiz\zip\tailor.h"\
	"..\..\..\..\wiz\zip\win32\osdep.h"\
	"..\..\..\..\wiz\zip\zip.h"\
	"..\..\..\..\wiz\zip\ziperr.h"\
	

"$(INTDIR)\globals.obj" : $(SOURCE) $(DEP_CPP_GLOBA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=D:\wiz\zip\Win32\nt.c

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

SOURCE=D:\wiz\zip\trees.c

!IF  "$(CFG)" == "zip32 - Win32 Release"

DEP_CPP_TREES=\
	"..\..\..\..\wiz\zip\api.h"\
	"..\..\..\..\wiz\zip\tailor.h"\
	"..\..\..\..\wiz\zip\win32\osdep.h"\
	"..\..\..\..\wiz\zip\zip.h"\
	"..\..\..\..\wiz\zip\ziperr.h"\
	

"$(INTDIR)\trees.obj" : $(SOURCE) $(DEP_CPP_TREES) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zip32 - Win32 Debug"

DEP_CPP_TREES=\
	"..\..\..\..\wiz\zip\api.h"\
	"..\..\..\..\wiz\zip\tailor.h"\
	"..\..\..\..\wiz\zip\win32\osdep.h"\
	"..\..\..\..\wiz\zip\zip.h"\
	"..\..\..\..\wiz\zip\ziperr.h"\
	

"$(INTDIR)\trees.obj" : $(SOURCE) $(DEP_CPP_TREES) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=D:\wiz\zip\ttyio.c

!IF  "$(CFG)" == "zip32 - Win32 Release"

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


!ELSEIF  "$(CFG)" == "zip32 - Win32 Debug"

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


!ENDIF 

SOURCE=D:\wiz\zip\util.c

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

SOURCE=D:\wiz\zip\Win32\win32.c

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

SOURCE=D:\wiz\zip\Win32\win32zip.c

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

SOURCE=D:\wiz\zip\windll\windll.c
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


SOURCE=D:\wiz\zip\windll\windll.rc

!IF  "$(CFG)" == "zip32 - Win32 Release"


"$(INTDIR)\windll.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\windll.res" /i "\wiz\zip\windll" /d "NDEBUG" /d\
 "WIN32" $(SOURCE)


!ELSEIF  "$(CFG)" == "zip32 - Win32 Debug"


"$(INTDIR)\windll.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\windll.res" /i "\wiz\zip\windll" /d "_DEBUG" /d\
 "WIN32" $(SOURCE)


!ENDIF 

SOURCE=D:\wiz\zip\zip.c

!IF  "$(CFG)" == "zip32 - Win32 Release"

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


!ELSEIF  "$(CFG)" == "zip32 - Win32 Debug"

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


!ENDIF 

SOURCE=D:\wiz\zip\zipfile.c

!IF  "$(CFG)" == "zip32 - Win32 Release"

DEP_CPP_ZIPFI=\
	"..\..\..\..\wiz\zip\api.h"\
	"..\..\..\..\wiz\zip\revision.h"\
	"..\..\..\..\wiz\zip\tailor.h"\
	"..\..\..\..\wiz\zip\win32\osdep.h"\
	"..\..\..\..\wiz\zip\zip.h"\
	"..\..\..\..\wiz\zip\ziperr.h"\
	

"$(INTDIR)\zipfile.obj" : $(SOURCE) $(DEP_CPP_ZIPFI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zip32 - Win32 Debug"

DEP_CPP_ZIPFI=\
	"..\..\..\..\wiz\zip\api.h"\
	"..\..\..\..\wiz\zip\revision.h"\
	"..\..\..\..\wiz\zip\tailor.h"\
	"..\..\..\..\wiz\zip\win32\osdep.h"\
	"..\..\..\..\wiz\zip\zip.h"\
	"..\..\..\..\wiz\zip\ziperr.h"\
	

"$(INTDIR)\zipfile.obj" : $(SOURCE) $(DEP_CPP_ZIPFI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=D:\wiz\zip\zipup.c

!IF  "$(CFG)" == "zip32 - Win32 Release"

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


!ELSEIF  "$(CFG)" == "zip32 - Win32 Debug"

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


!ENDIF 

