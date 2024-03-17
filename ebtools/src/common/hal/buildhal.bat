del /q/f *.obj
del /q/f hal.exe
CL /nologo /c    lexer.lex.c 
CL /nologo /c    parser.tab.c 
CL /nologo /c    avl_mod.c 
CL /nologo /c    dll.c 
CL /nologo /c    main.c 
CL /nologo /c    hal.c 
CL /nologo /c    sym.c 
CL /nologo /c    utl.c 
CL /nologo /c    dir.c 
CL /nologo /c    memmgt.c 
CL /nologo /c    operator.c 
CL /nologo /c    osf.c 
CL /nologo /c    lib.c 
CL /nologo /c    NN_Div.c 
CL /nologo /c    NN_Util.c 
CL /nologo /c    NN_DigitDiv.c 
CL /nologo /c    NN_SubDigitMult.c 
CL /nologo /c    NN_Sub.c 
CL /nologo /c    NN_DigitMult.c 
CL /nologo /c	yywrap.c
del /q/f hal.exe
link /out:hal.exe lexer.lex.obj parser.tab.obj avl_mod.obj dll.obj main.obj hal.obj sym.obj utl.obj dir.obj memmgt.obj operator.obj osf.obj lib.obj NN_Div.obj NN_Util.obj NN_DigitDiv.obj NN_SubDigitMult.obj NN_Sub.obj NN_DigitMult.obj yywrap.obj
