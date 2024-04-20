OBJS = parser_tab.obj$(alpha) lexyy.obj$(alpha) avl_mod.obj$(alpha) main.obj$(alpha) hal.obj$(alpha) sym.obj$(alpha) utl.obj$(alpha) dir.obj$(alpha) memmgt.obj$(alpha) operator.obj$(alpha) lib.obj$(alpha) dll.obj$(alpha) osf.obj$(alpha) nn_div.obj$(alpha) nn_util.obj$(alpha) nn_digitdiv.obj$(alpha) nn_subdigitmult.obj$(alpha) nn_sub.obj$(alpha) nn_digitmult.obj$(alpha) yywrap.obj$(alpha)

.suffixes
.suffixes .exe .olb .obj$(ALPHA) .c .l .h

CC = cc
FLEX = flex

GENERIC_CFLAGS = $(DEBUG)

.ifdef ALPHA
EXTRA_CFLAGS = /reentrancy=none/ansi_alias/nowarn/stand=vaxc
.else
EXTRA_CFLAGS = 
.endif

CFLAGS = $(GENERIC_CFLAGS)$(EXTRA_CFLAGS)

.ifdef DEBUG
LINKFLAGS = /DEBUG
.else
LINKFLAGS = 
.endif

.c.obj$(ALPHA) :
	$(CC) /obj=.obj$(ALPHA)$(CFLAGS) $<

.first
	@ define/nolog sys sys$library

#
# DO NOT REMOVE THE "-I" from FLEXFLAGS
#
# 1. It is required for the macro preprocessing function to work correctly.
# 2. It is required for the error processing to have the correct line numbers.
#
FLEXFLAGS = -iId

BISONFLAGS = /verbose/define/debug

hal$(alpha).exe : $(OBJS)
	link $(LINKFLAGS)/exe=hal$(alpha) $+

.ifdef alpha
lexyy.c : lexer.l
parser_tab.c : parser.y
	@ write sys$output "FLEX and BISON must be run on VAX/VMS"
	@ exit 0
.else
lexyy.c : lexer.l
	@ if f$trnlnm("FLEX$SYS") .eqs. "" then @v3$:[asher.flex_v2_3_6]setup_flex
	$(FLEX) $(FLEXFLAGS) $<

parser_tab.c : parser.y
	@ if f$trnlnm("BISON$SYS") .eqs. "" then @v3$:[asher.bison_v1_14]setup_bison
	bison $(BISONFLAGS) $<
.endif

parser_tab.obj$(alpha) : hal.h avl.h lexyy.h parser_tab.h
lexyy.obj$(alpha) : hal.h avl.h lexyy.h parser_tab.h ev5.h operator.h
avl_mod.obj$(alpha) : avl.h
main.obj$(alpha) : hal.h avl.h lexyy.h parser_tab.h version.h
hal.obj$(alpha) : hal.h avl.h lexyy.h parser_tab.h
sym.obj$(alpha) : hal.h avl.h lexyy.h parser_tab.h
utl.obj$(alpha) : hal.h avl.h lexyy.h parser_tab.h operator.h
dir.obj$(alpha) : hal.h avl.h lexyy.h parser_tab.h operator.h
memmgt.obj$(alpha) : hal.h avl.h lexyy.h parser_tab.h
operator.obj$(alpha) : alpha_opcode.h hal.h avl.h lexyy.h parser_tab.h operator.h defoper.h
lib.obj$(alpha) : hal.h lib.h operator.h
dll.obj$(alpha) : dll.h
osf.obj$(alpha) : hal.h version.h
nn_div.obj$(alpha) : nn.h
nn_util.obj$(alpha) : nn.h
nn_digitdiv.obj$(alpha) : nn.h
nn_subdigitmult.obj$(alpha) : nn.h
nn_sub.obj$(alpha) : nn.h
nn_digitmult.obj$(alpha) : nn.h
