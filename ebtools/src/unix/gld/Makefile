#
#
CONFIG_DIR=../../../..
include $(CONFIG_DIR)/sdkmake.conf

IDIR			= $(EB_TOOLBOX)

#include ${MAKETOP}Makefile-common

PROGRAM			= gld
OBJECTS			= gld.o cplus-dem.o alloca.o
#LIBRARIES		= CMUCS

#CFLAGS			= -g3 -Dalpha -Umips -DCMUCS -Dnounderscore -DUSE_BASE_REGISTER
CFLAGS			= -I. -g -Dalpha -Umips -Dnounderscore -DUSE_BASE_REGISTER

#include ${MAKETOP}Makefile-bin
#include ${MAKETOP}Makefile-man

install:	$(PROGRAM)
	cp $(PROGRAM) $(EB_TOOLBOX)

include ../Makefile.bin
-include Makedep

