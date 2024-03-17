OBJFILES=lexer.lex.o parser.tab.o avl_mod.o dll.o main.o hal.o sym.o utl.o \
    dir.o memmgt.o operator.o osf.o lib.o NN_Div.o NN_Util.o NN_DigitDiv.o \
    NN_SubDigitMult.o NN_Sub.o NN_DigitMult.o yywrap.o

CFLAGS=-I. -DHALSRC -D__int64=long

hal: $(OBJFILES)
	$(CC) -o hal $(OBJFILES)

FLEX=flex
FLEXFLAGS=-iIL

BISON=bison
BISONFLAGS=-dtvl

install: hal
	cp hal ../../../unix

lexer.lex.c:	lexer.l parser.tab.c
	$(FLEX) $(FLEXFLAGS) lexer.l
	mv lex.yy.c lexer.lex.c

parser.tab.c: parser.y
	$(BISON) $(BISONFLAGS) parser.y

clean:
	rm -f $(OBJFILES) hal

realclean:	clean
	rm -f parser.tab.c parser.tab.h lexer.lex.c lex.yy.c
