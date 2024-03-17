#
# Common include file for nt makefiles.
#
SRCDIR          = .
OBJDIR          = ..\obj

# The include file <NTWIN32.MAK> requires the CPU environment
# variable to be set.  However, on systems that have never had
# the win32 SDK installed, this variable will unlikely be defined.
!IFNDEF CPU
CPU	= $(PROCESSOR_ARCHITECTURE)
!ENDIF

!INCLUDE <NTWIN32.MAK>

!IF "$(PROCESSOR_ARCHITECTURE)" == "ALPHA"
EXEDIR          = ..\..\..\ntalpha
!ELSE
EXEDIR          = ..\..\..\ntx86
!ENDIF


INCDIR          = ..\include
COMDIR          = ..\common
COMMONLIB   	= $(OBJDIR)\COMLIB.LIB
#
DEBUG_FLAGS     =  -Zi 
LINK_DEBUG_FLAGS = /DEBUG:FULL
OPT_FLAGS =  -Od
#
EXTRA_CFLAGS    =  -nologo -VERBOSE -DCROSS_COMPILE_32_TO_64_BITS=1 -c -I$(INCDIR) -I. $(EXTRA_CFLAGS)
EXTRA_LINKFLAGS =  -VERBOSE
#
CLEANFLAGS	= /f /q

.c.obj:                                   
  $(cc) $(cdebug) $(cflags) $(EXTRA_CFLAGS) $(cvars) $*.c 

{$(SRCDIR)}.c{$(OBJDIR)}.obj:                                   
  $(cc) $(cdebug) $(cflags) $(EXTRA_CFLAGS) $(cvars) $(SRCDIR)\$(@B).c -Fo$@
