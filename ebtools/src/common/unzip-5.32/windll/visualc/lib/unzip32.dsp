# Microsoft Developer Studio Project File - Name="unzip32" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

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
!MESSAGE "unzip32 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "unzip32 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "unzip32 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Release\libs"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "D:\WIZ\UNZIP" /I "D:\WIZ\UNZIP\WINDLL" /I "D:\WIZ\UNZIP\WIN32" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WINDLL" /D "USE_EF_UT_TIME" /D "DLL" /D "UNZIPLIB" /FD /c
# SUBTRACT CPP /YX
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "unzip32 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Debug\libs"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Z7 /Od /I "D:\WIZ\UNZIP" /I "D:\WIZ\UNZIP\WINDLL" /I "D:\WIZ\UNZIP\WIN32" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WINDLL" /D "USE_EF_UT_TIME" /D "DLL" /D "UNZIPLIB" /FD /c
# SUBTRACT CPP /YX
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "unzip32 - Win32 Release"
# Name "unzip32 - Win32 Debug"
# Begin Source File

SOURCE=..\..\..\..\wiz\unzip\api.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wiz\unzip\crc32.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wiz\unzip\crctab.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wiz\unzip\crypt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wiz\unzip\explode.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wiz\unzip\extract.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wiz\unzip\fileio.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wiz\unzip\globals.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wiz\unzip\inflate.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wiz\unzip\list.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wiz\unzip\match.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wiz\unzip\Win32\nt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wiz\unzip\process.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wiz\unzip\ttyio.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wiz\unzip\unreduce.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wiz\unzip\unshrink.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wiz\unzip\windll\Unziplib.def
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wiz\unzip\Win32\win32.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wiz\unzip\windll\windll.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wiz\unzip\windll\windll.rc
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wiz\unzip\zipinfo.c
# End Source File
# End Target
# End Project
