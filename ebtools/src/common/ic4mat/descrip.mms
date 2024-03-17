! Macro definitions
LIBRARYMODULES = ic4mat.obj ev6.obj ev6_cbox.obj ev6_chain.obj -
	ev6_srom.obj ev6_ic.obj getopt.obj

LIBRARIES = ic4mat.olb($(LIBRARYMODULES)) - ! project library
	SYS$LIBRARY:VAXCRTL.OLB          ! C Runtime Library

CFLAGS = /INCLUDE_DIRECTORY=[]

! Dependencies
ic4mat.exe : $(LIBRARIES)
	LINK/EXEC=ic4mat ic4mat.obj, ic4mat/LIB

clean :
	delete/noconfirm ic4mat.olb;*, ic4mat.obj;*, ev6.obj;*, -
	ev6_cbox.obj;*, ev6_chain.obj;*, ev6_srom.obj;*, -
	ev6_ic.obj;*, getopt.obj;*

ic4mat.obj : ic4mat.c
ev6.obj : ev6.c
ev6_cbox.obj : ev6_cbox.c
ev6_chain.obj : ev6_chain.c
ev6_srom.obj : ev6_srom.c
ev6_ic.obj : ev6_ic.c
getopt.obj : getopt.c
