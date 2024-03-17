######################################################################
# HISTORY
#  7-Dec-91  Alessandro Forin (af) at Carnegie-Mellon University
#	Created.
#
# $Log: makedep.x,v $
# Revision 1.1.1.1  1998/12/29 21:36:30  paradis
# Initial CVS checkin
#
# Revision 1.1  1995/05/04  20:51:16  fdh
# Initial revision
#
# 
######################################################################

ALPHA_CFLAGS	= -DEXPR_TAKES_BIGNUMS
ALPHA_OFILES	= alpha.o atof-ieee.o atof-vax.o alloca.o

alpha_CFLAGS	= -g3 -Dalpha -Umips -DMIPS_HOST=1 -DCROSS_COMPILE_32_TO_64_BITS=1 -DEXPR_TAKES_BIGNUMS -DFIRST_32BIT_QUADRANT=1
alpha_OFILES	= alpha.o atof-ieee.o atof-vax.o alloca.o cross_32_to_64.o

# for cross
TARGET_MACHINE_CFLAGS = ${alpha_CFLAGS}
TARGET_MACHINE_OFILES = ${alpha_OFILES}
# for native, if we could
#TARGET_MACHINE_CFLAGS = ${ALPHA_CFLAGS}
#TARGET_MACHINE_OFILES = ${ALPHA_OFILES}


alpha.o:	as.h read.h flonum.h bignum.h md.h \
		obstack.h frags.h struc-symbol.h a_out.h expr.h \
		symbols.h a_out.h alpha-opcode.h
atof-ieee.o:	flonum.h bignum.h
atof-vax.o:	as.h read.h flonum.h bignum.h
