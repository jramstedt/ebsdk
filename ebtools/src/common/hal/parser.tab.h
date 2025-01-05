/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_PARSER_TAB_H_INCLUDED
# define YY_YY_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    ENDP = 258,                    /* ENDP  */
    INS_FOP = 259,                 /* INS_FOP  */
    INS_FP2 = 260,                 /* INS_FP2  */
    INS_IOP = 261,                 /* INS_IOP  */
    INS_MEM = 262,                 /* INS_MEM  */
    INS_JSR = 263,                 /* INS_JSR  */
    INS_BRN = 264,                 /* INS_BRN  */
    INS_M1A = 265,                 /* INS_M1A  */
    INS_M1B = 266,                 /* INS_M1B  */
    INS_MIS = 267,                 /* INS_MIS  */
    INS_FPCR = 268,                /* INS_FPCR  */
    INS_RPCC = 269,                /* INS_RPCC  */
    INS_SEXT = 270,                /* INS_SEXT  */
    INS_IP2 = 271,                 /* INS_IP2  */
    INS_IP3 = 272,                 /* INS_IP3  */
    INS_FTI = 273,                 /* INS_FTI  */
    INS_LSB = 274,                 /* INS_LSB  */
    INS_SSB = 275,                 /* INS_SSB  */
    INS_ITF = 276,                 /* INS_ITF  */
    INS_UNOP = 277,                /* INS_UNOP  */
    INS_CALLPAL = 278,             /* INS_CALLPAL  */
    INS_IPR4 = 279,                /* INS_IPR4  */
    INS_HWM4 = 280,                /* INS_HWM4  */
    INS_IPR5 = 281,                /* INS_IPR5  */
    INS_HWM5 = 282,                /* INS_HWM5  */
    INS_REI6 = 283,                /* INS_REI6  */
    INS_MTI6 = 284,                /* INS_MTI6  */
    INS_MFI6 = 285,                /* INS_MFI6  */
    INS_HWM6 = 286,                /* INS_HWM6  */
    INS_1REG = 287,                /* INS_1REG  */
    IREG = 288,                    /* IREG  */
    FREG = 289,                    /* FREG  */
    ADDR = 290,                    /* ADDR  */
    QUAD = 291,                    /* QUAD  */
    LONG = 292,                    /* LONG  */
    BLKL = 293,                    /* BLKL  */
    BLKQ = 294,                    /* BLKQ  */
    IDENT = 295,                   /* IDENT  */
    TITLE = 296,                   /* TITLE  */
    WORD = 297,                    /* WORD  */
    BYTE = 298,                    /* BYTE  */
    BODYTEXT = 299,                /* BODYTEXT  */
    K_BYTE = 300,                  /* K_BYTE  */
    K_WORD = 301,                  /* K_WORD  */
    K_LONG = 302,                  /* K_LONG  */
    K_QUAD = 303,                  /* K_QUAD  */
    K_PAGE = 304,                  /* K_PAGE  */
    LOCAL_LABEL = 305,             /* LOCAL_LABEL  */
    LOCAL_BLOCK = 306,             /* LOCAL_BLOCK  */
    NOLIST = 307,                  /* NOLIST  */
    LIST = 308,                    /* LIST  */
    MEB = 309,                     /* MEB  */
    MC = 310,                      /* MC  */
    CND = 311,                     /* CND  */
    MD = 312,                      /* MD  */
    ME = 313,                      /* ME  */
    ASSIGN = 314,                  /* ASSIGN  */
    CMDIN = 315,                   /* CMDIN  */
    NODEBUG = 316,                 /* NODEBUG  */
    K_DEBUG = 317,                 /* K_DEBUG  */
    NUMBER = 318,                  /* NUMBER  */
    IF = 319,                      /* IF  */
    WHILE = 320,                   /* WHILE  */
    BREAK = 321,                   /* BREAK  */
    CONTINUE = 322,                /* CONTINUE  */
    INCLUDE = 323,                 /* INCLUDE  */
    INCLUDE_DIR = 324,             /* INCLUDE_DIR  */
    MINCLUDE = 325,                /* MINCLUDE  */
    OPEN_ANGLE = 326,              /* OPEN_ANGLE  */
    CLOSE_ANGLE = 327,             /* CLOSE_ANGLE  */
    PLUS = 328,                    /* PLUS  */
    MINUS = 329,                   /* MINUS  */
    SHIFT = 330,                   /* SHIFT  */
    EQUAL = 331,                   /* EQUAL  */
    NOT = 332,                     /* NOT  */
    LOG_AND = 333,                 /* LOG_AND  */
    LOG_OR = 334,                  /* LOG_OR  */
    LOG_NOT = 335,                 /* LOG_NOT  */
    COLON = 336,                   /* COLON  */
    SYMBOL = 337,                  /* SYMBOL  */
    LABEL = 338,                   /* LABEL  */
    STRING = 339,                  /* STRING  */
    EOL = 340,                     /* EOL  */
    EOE = 341,                     /* EOE  */
    DOT = 342,                     /* DOT  */
    EQ = 343,                      /* EQ  */
    GT = 344,                      /* GT  */
    LT = 345,                      /* LT  */
    DF = 346,                      /* DF  */
    NE = 347,                      /* NE  */
    GE = 348,                      /* GE  */
    LE = 349,                      /* LE  */
    NDF = 350,                     /* NDF  */
    ENDC = 351,                    /* ENDC  */
    ENDW = 352,                    /* ENDW  */
    ALIGN = 353,                   /* ALIGN  */
    IFF = 354,                     /* IFF  */
    IFT = 355,                     /* IFT  */
    IFTF = 356,                    /* IFTF  */
    REPEAT = 357,                  /* REPEAT  */
    ENDR = 358,                    /* ENDR  */
    ENDI = 359,                    /* ENDI  */
    DOCMD = 360,                   /* DOCMD  */
    COMMENT = 361,                 /* COMMENT  */
    MACRO = 362,                   /* MACRO  */
    ENDM = 363,                    /* ENDM  */
    PRAGMA = 364,                  /* PRAGMA  */
    MACRO_CALL = 365,              /* MACRO_CALL  */
    DOTDOTDOT = 366,               /* DOTDOTDOT  */
    NEXT_ARG = 367,                /* NEXT_ARG  */
    ARG_VALUE = 368,               /* ARG_VALUE  */
    ASCII = 369,                   /* ASCII  */
    ASCIZ = 370,                   /* ASCIZ  */
    SAVE_PSECT = 371,              /* SAVE_PSECT  */
    SAVE_DSECT = 372,              /* SAVE_DSECT  */
    RESTORE_PSECT = 373,           /* RESTORE_PSECT  */
    RESTORE_DSECT = 374,           /* RESTORE_DSECT  */
    PSECT = 375,                   /* PSECT  */
    DSECT = 376,                   /* DSECT  */
    LINK_ADDRESS = 377,            /* LINK_ADDRESS  */
    LINK_AFTER = 378,              /* LINK_AFTER  */
    LINK_FIRST = 379,              /* LINK_FIRST  */
    LINK_LAST = 380,               /* LINK_LAST  */
    LINK_ATTR = 381,               /* LINK_ATTR  */
    LINK_OFFSET = 382,             /* LINK_OFFSET  */
    LINK_REGION = 383,             /* LINK_REGION  */
    LINK_SIZE = 384,               /* LINK_SIZE  */
    HEADER = 385,                  /* HEADER  */
    ENDH = 386,                    /* ENDH  */
    OVERLAP_OK = 387,              /* OVERLAP_OK  */
    DIRECTLY_AFTER = 388,          /* DIRECTLY_AFTER  */
    MAC_NONE = 389,                /* MAC_NONE  */
    MAC_ONCE = 390,                /* MAC_ONCE  */
    MAC_UNLIMITED = 391,           /* MAC_UNLIMITED  */
    LEXER_DEBUG_ON = 392,          /* LEXER_DEBUG_ON  */
    LEXER_DEBUG_OFF = 393,         /* LEXER_DEBUG_OFF  */
    PARSER_DEBUG_ON = 394,         /* PARSER_DEBUG_ON  */
    PARSER_DEBUG_OFF = 395,        /* PARSER_DEBUG_OFF  */
    WARN = 396,                    /* WARN  */
    ERROR = 397,                   /* ERROR  */
    PRINT = 398,                   /* PRINT  */
    B = 399,                       /* B  */
    NB = 400,                      /* NB  */
    IDN = 401,                     /* IDN  */
    DIF = 402,                     /* DIF  */
    IIF = 403,                     /* IIF  */
    ARCH = 404,                    /* ARCH  */
    A_EV4 = 405,                   /* A_EV4  */
    A_EV5 = 406,                   /* A_EV5  */
    A_EV6 = 407,                   /* A_EV6  */
    UMINUS = 408                   /* UMINUS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PARSER_TAB_H_INCLUDED  */
