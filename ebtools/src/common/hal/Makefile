CONFIG_DIR=../../../..
include $(CONFIG_DIR)/sdkmake.conf

CC				= cc
CFLAGS		= -g -I.
FLEX			= flex
FLEXFLAGS	= -iIL
BISON			= bison
BISONFLAGS= -dtvl

SRCS =	lexer.lex.c parser.tab.c avl_mod.c dll.c main.c hal.c sym.c utl.c \
	dir.c memmgt.c operator.c osf.c lib.c NN_Div.c NN_Util.c NN_DigitDiv.c \
	NN_SubDigitMult.c NN_Sub.c NN_DigitMult.c yywrap.c

hal : $(SRCS:.c=.o)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OUTPUT_OPTION) $(TARGET_ARCH) $^

lexer.lex.c : lexer.l parser.tab.c
	$(FLEX) $(FLEXFLAGS) $<
	mv lex.yy.c $@

parser.tab.c: parser.y
	$(BISON) $(BISONFLAGS) $<

install : hal
		cp -fp $^ $(EB_TOOLBOX)/.
		chmod 775 $(addprefix $(EB_TOOLBOX)/, $^)

clean :
		rm -f *.o *~ \#*\# *.bak *.a core \
		hal

realclean:	clean
	rm -f parser.tab.c parser.tab.h lexer.lex.c lex.yy.c

# Dependencies
DEPDIR := .deps
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c

%.o : %.c
%.o : %.c $(DEPDIR)/%.d | $(DEPDIR)
		$(COMPILE.c) $(OUTPUT_OPTION) $<

$(DEPDIR): ; @mkdir -p $@

DEPFILES := $(SRCS:%.c=$(DEPDIR)/%.d)
$(DEPFILES):

include $(wildcard $(DEPFILES))
