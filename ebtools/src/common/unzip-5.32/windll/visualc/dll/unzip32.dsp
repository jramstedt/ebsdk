# Microsoft Developer Studio Project File - Name="unzip32" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=unzip32 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "unzip32.mak".
!MESSAGE 
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

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "unzip32 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Release\app"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Zp4 /MT /W3 /GX /O2 /I "d:\wiz\unzip" /I "d:\wiz\unzip\windll" /I "d:\wiz\unzip\win32" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WINDLL" /D "USE_EF_UT_TIME" /D "DLL" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "WIN32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386

!ELSEIF  "$(CFG)" == "unzip32 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Debug\app"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Zp4 /MTd /W3 /Gm /GX /Zi /Od /I "d:\wiz\unzip" /I "d:\wiz\unzip\windll" /I "d:\wiz\unzip\win32" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WINDLL" /D "USE_EF_UT_TIME" /D "DLL" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "WIN32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "unzip32 - Win32 Release"
# Name "unzip32 - Win32 Debug"
# Begin Source File

SOURCE=D:\wiz\unzip\api.c
# End Source File
# Begin Source File

SOURCE=D:\wiz\unzip\crc32.c
# End Source File
# Begin Source File

SOURCE=D:\wiz\unzip\crctab.c
# End Source File
# Begin Source File

SOURCE=D:\wiz\unzip\crypt.c
# End Source File
# Begin Source File

SOURCE=D:\wiz\unzip\explode.c
# End Source File
# Begin Source File

SOURCE=D:\wiz\unzip\extract.c
# End Source File
# Begin Source File

SOURCE=D:\wiz\unzip\fileio.c
# End Source File
# Begin Source File

SOURCE=D:\wiz\unzip\globals.c
# End Source File
# Begin Source File

SOURCE=D:\wiz\unzip\inflate.c
# End Source File
# Begin Source File

SOURCE=D:\wiz\unzip\list.c
# End Source File
# Begin Source File

SOURCE=D:\wiz\unzip\match.c
# End Source File
# Begin Source File

SOURCE=D:\wiz\unzip\Win32\nt.c
# End Source File
# Begin Source File

SOURCE=D:\wiz\unzip\process.c
# End Source File
# Begin Source File

SOURCE=D:\wiz\unzip\ttyio.c
# End Source File
# Begin Source File

SOURCE=D:\wiz\unzip\unreduce.c
# End Source File
# Begin Source File

SOURCE=D:\wiz\unzip\unshrink.c
# End Source File
# Begin Source File

SOURCE=D:\wiz\unzip\Win32\win32.c
# End Source File
# Begin Source File

SOURCE=D:\wiz\unzip\windll\windll.c
# End Source File
# Begin Source File

SOURCE=D:\wiz\unzip\windll\windll.rc
# End Source File
# Begin Source File

SOURCE=D:\wiz\unzip\windll\windll32.def
# End Source File
# Begin Source File

SOURCE=D:\wiz\unzip\zipinfo.c
# End Source File
# End Target
# End Project
