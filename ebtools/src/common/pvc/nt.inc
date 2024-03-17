#
# Common include file for nt makefiles.
#
SRCDIR          = .
OBJDIR          = .

!IF "$(PROCESSOR_ARCHITECTURE)" == "ALPHA"
EXEDIR          = ..\..\..\ntalpha
!ELSE
EXEDIR          = ..\..\..\ntx86
!ENDIF

INCDIR          = ..\include
COMDIR          = ..\common
COMMONLIB   	= $(COMDIR)\COMLIB.LIB
#
DEBUG_FLAGS     =  -Zi 
LINK_DEBUG_FLAGS = /DEBUG:FULL
OPT_FLAGS =  -Od
#
EXTRA_CFLAGS    =  -nologo -VERBOSE -DCROSS_COMPILE_32_TO_64_BITS=1 -c -I$(INCDIR) -I.
EXTRA_LINKFLAGS =  -VERBOSE
#
CLEANFLAGS	= /f /q

.c.obj:                                   
  $(cc) $(cdebug) $(cflags) $(EXTRA_CFLAGS) $(cvars) $*.c 
