/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */

/*
	Copyright (c) 1992
	Digital Equipment Corporation, Maynard, Massachusetts 01754

	The HAL Alpha Assembler is free software; you can redistribute 
	it and/or modify it with the sole restriction that this copyright 
	notice be attached.

	The information in this software is subject to change without notice 
	and should not be construed as a commitment by Digital Equipment
	Corporation.

	The author disclaims all warranties with regard to this software,
	including all implied warranties of merchantability and fitness, in
	no event shall he be liable for any special, indirect or consequential
	damages or any damages whatsoever resulting from loss of use, data, 
	or profits, whether in an action of contract, negligence or other
	tortious action, arising out of or in connection with the use or
	performance of this software.

*/
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "hal.h"

#ifdef _WIN32
#include <malloc.h>
#define alloca _alloca
#endif


#define CUR_LINENO	(cur_ifile->lineno + cur_base_lineno)

static int if_result;
static int temporary_num = 0;
static int is_while = 0;

ABSEXPR* absexpr_head = NULL;
static int saved_absexpr = 0;

/*
 * used for .ASCII and .ASCIZ
 */
static int nchar, byte_in_lw;

/*
 * used for .addr, .quad, and .long
 */
static int mem_type, num_mem_expr;

/*
 * used for .BLKx directives.
 */
extern int wrote_memory;

/*
 * used for local label blocks
 */
int max_blknum = 0;		        /* Always a unique block number for local labels */
int local_label_blknum = 0;		/* current block number for local labels */
int used_local_label = 0;		/* has this local label block had a local
                                           label defined in it? */
/*
 * used for macro parsing
 */
static int narg;
static TOKEN_LIST* mac_arglist;
static TOKEN_LIST* mac_deflist;
static int var_arg_used;

/*
 * used for macro call parsing
 */
static MACRODEF* macdef;
static char** macargs;
static int macargs_size;
static int macro_arg_pos;

extern int regnum;	/* the register being read */

/*
 * Used for .list/.nolist
 */
static int temp_listing_flags;
static int save_start_line;

/*
 * Used for .link_attribute
 */
static int temp_link_flags;

/*
 * Used for .pragma's
 */
static int temp_pragma_flags;

/*
 * Prototypes defined here for functions used only is this
 * module.
 */
static YYSTYPE chain PROTO((YYSTYPE t1, YYSTYPE t2, YYSTYPE t3));
static void store_expr PROTO((TOKEN_LIST* expr));
static void store_string PROTO((TOKEN_LIST* strtok));
static void fill_to_lw();
static YYSTYPE convert_dot PROTO((TOKEN_LIST* dottok));
static YYSTYPE convert_num2arg PROTO((TOKEN_LIST* dottok));
static void dot_equal PROTO((TOKEN_LIST* expr));
static void define_symbol PROTO((TOKEN_LIST* name, TOKEN_LIST* expr));
static void define_label PROTO((TOKEN_LIST* name, unsigned int hi, unsigned int lo));
static void handle_instr_expr PROTO((TOKEN_LIST* expr, int type));
static void handle_memdir_expr PROTO((TOKEN_LIST* expr, int type));
static void push_bodies PROTO((TOKEN_LIST* bodyseq));
static void push_repeat_body PROTO((TOKEN_LIST* body, TOKEN_LIST* absexpr));
static void push_while_body PROTO((TOKEN_LIST* body, TOKEN_LIST* expr));
static dbug();
static char* create_temporary_label PROTO((unsigned int hi, unsigned int lo));


# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_ENDP = 3,                       /* ENDP  */
  YYSYMBOL_INS_FOP = 4,                    /* INS_FOP  */
  YYSYMBOL_INS_FP2 = 5,                    /* INS_FP2  */
  YYSYMBOL_INS_IOP = 6,                    /* INS_IOP  */
  YYSYMBOL_INS_MEM = 7,                    /* INS_MEM  */
  YYSYMBOL_INS_JSR = 8,                    /* INS_JSR  */
  YYSYMBOL_INS_BRN = 9,                    /* INS_BRN  */
  YYSYMBOL_INS_M1A = 10,                   /* INS_M1A  */
  YYSYMBOL_INS_M1B = 11,                   /* INS_M1B  */
  YYSYMBOL_INS_MIS = 12,                   /* INS_MIS  */
  YYSYMBOL_INS_FPCR = 13,                  /* INS_FPCR  */
  YYSYMBOL_INS_RPCC = 14,                  /* INS_RPCC  */
  YYSYMBOL_INS_SEXT = 15,                  /* INS_SEXT  */
  YYSYMBOL_INS_IP2 = 16,                   /* INS_IP2  */
  YYSYMBOL_INS_IP3 = 17,                   /* INS_IP3  */
  YYSYMBOL_INS_FTI = 18,                   /* INS_FTI  */
  YYSYMBOL_INS_LSB = 19,                   /* INS_LSB  */
  YYSYMBOL_INS_SSB = 20,                   /* INS_SSB  */
  YYSYMBOL_INS_ITF = 21,                   /* INS_ITF  */
  YYSYMBOL_INS_UNOP = 22,                  /* INS_UNOP  */
  YYSYMBOL_INS_CALLPAL = 23,               /* INS_CALLPAL  */
  YYSYMBOL_INS_IPR4 = 24,                  /* INS_IPR4  */
  YYSYMBOL_INS_HWM4 = 25,                  /* INS_HWM4  */
  YYSYMBOL_INS_IPR5 = 26,                  /* INS_IPR5  */
  YYSYMBOL_INS_HWM5 = 27,                  /* INS_HWM5  */
  YYSYMBOL_INS_REI6 = 28,                  /* INS_REI6  */
  YYSYMBOL_INS_MTI6 = 29,                  /* INS_MTI6  */
  YYSYMBOL_INS_MFI6 = 30,                  /* INS_MFI6  */
  YYSYMBOL_INS_HWM6 = 31,                  /* INS_HWM6  */
  YYSYMBOL_INS_1REG = 32,                  /* INS_1REG  */
  YYSYMBOL_IREG = 33,                      /* IREG  */
  YYSYMBOL_FREG = 34,                      /* FREG  */
  YYSYMBOL_ADDR = 35,                      /* ADDR  */
  YYSYMBOL_QUAD = 36,                      /* QUAD  */
  YYSYMBOL_LONG = 37,                      /* LONG  */
  YYSYMBOL_BLKL = 38,                      /* BLKL  */
  YYSYMBOL_BLKQ = 39,                      /* BLKQ  */
  YYSYMBOL_IDENT = 40,                     /* IDENT  */
  YYSYMBOL_TITLE = 41,                     /* TITLE  */
  YYSYMBOL_WORD = 42,                      /* WORD  */
  YYSYMBOL_BYTE = 43,                      /* BYTE  */
  YYSYMBOL_BODYTEXT = 44,                  /* BODYTEXT  */
  YYSYMBOL_K_BYTE = 45,                    /* K_BYTE  */
  YYSYMBOL_K_WORD = 46,                    /* K_WORD  */
  YYSYMBOL_K_LONG = 47,                    /* K_LONG  */
  YYSYMBOL_K_QUAD = 48,                    /* K_QUAD  */
  YYSYMBOL_K_PAGE = 49,                    /* K_PAGE  */
  YYSYMBOL_LOCAL_LABEL = 50,               /* LOCAL_LABEL  */
  YYSYMBOL_LOCAL_BLOCK = 51,               /* LOCAL_BLOCK  */
  YYSYMBOL_NOLIST = 52,                    /* NOLIST  */
  YYSYMBOL_LIST = 53,                      /* LIST  */
  YYSYMBOL_MEB = 54,                       /* MEB  */
  YYSYMBOL_MC = 55,                        /* MC  */
  YYSYMBOL_CND = 56,                       /* CND  */
  YYSYMBOL_MD = 57,                        /* MD  */
  YYSYMBOL_ME = 58,                        /* ME  */
  YYSYMBOL_ASSIGN = 59,                    /* ASSIGN  */
  YYSYMBOL_CMDIN = 60,                     /* CMDIN  */
  YYSYMBOL_NODEBUG = 61,                   /* NODEBUG  */
  YYSYMBOL_K_DEBUG = 62,                   /* K_DEBUG  */
  YYSYMBOL_NUMBER = 63,                    /* NUMBER  */
  YYSYMBOL_IF = 64,                        /* IF  */
  YYSYMBOL_WHILE = 65,                     /* WHILE  */
  YYSYMBOL_BREAK = 66,                     /* BREAK  */
  YYSYMBOL_CONTINUE = 67,                  /* CONTINUE  */
  YYSYMBOL_INCLUDE = 68,                   /* INCLUDE  */
  YYSYMBOL_INCLUDE_DIR = 69,               /* INCLUDE_DIR  */
  YYSYMBOL_MINCLUDE = 70,                  /* MINCLUDE  */
  YYSYMBOL_OPEN_ANGLE = 71,                /* OPEN_ANGLE  */
  YYSYMBOL_CLOSE_ANGLE = 72,               /* CLOSE_ANGLE  */
  YYSYMBOL_PLUS = 73,                      /* PLUS  */
  YYSYMBOL_MINUS = 74,                     /* MINUS  */
  YYSYMBOL_SHIFT = 75,                     /* SHIFT  */
  YYSYMBOL_EQUAL = 76,                     /* EQUAL  */
  YYSYMBOL_NOT = 77,                       /* NOT  */
  YYSYMBOL_LOG_AND = 78,                   /* LOG_AND  */
  YYSYMBOL_LOG_OR = 79,                    /* LOG_OR  */
  YYSYMBOL_LOG_NOT = 80,                   /* LOG_NOT  */
  YYSYMBOL_COLON = 81,                     /* COLON  */
  YYSYMBOL_SYMBOL = 82,                    /* SYMBOL  */
  YYSYMBOL_LABEL = 83,                     /* LABEL  */
  YYSYMBOL_STRING = 84,                    /* STRING  */
  YYSYMBOL_EOL = 85,                       /* EOL  */
  YYSYMBOL_EOE = 86,                       /* EOE  */
  YYSYMBOL_DOT = 87,                       /* DOT  */
  YYSYMBOL_EQ = 88,                        /* EQ  */
  YYSYMBOL_GT = 89,                        /* GT  */
  YYSYMBOL_LT = 90,                        /* LT  */
  YYSYMBOL_DF = 91,                        /* DF  */
  YYSYMBOL_NE = 92,                        /* NE  */
  YYSYMBOL_GE = 93,                        /* GE  */
  YYSYMBOL_LE = 94,                        /* LE  */
  YYSYMBOL_NDF = 95,                       /* NDF  */
  YYSYMBOL_ENDC = 96,                      /* ENDC  */
  YYSYMBOL_ENDW = 97,                      /* ENDW  */
  YYSYMBOL_ALIGN = 98,                     /* ALIGN  */
  YYSYMBOL_IFF = 99,                       /* IFF  */
  YYSYMBOL_IFT = 100,                      /* IFT  */
  YYSYMBOL_IFTF = 101,                     /* IFTF  */
  YYSYMBOL_REPEAT = 102,                   /* REPEAT  */
  YYSYMBOL_ENDR = 103,                     /* ENDR  */
  YYSYMBOL_ENDI = 104,                     /* ENDI  */
  YYSYMBOL_DOCMD = 105,                    /* DOCMD  */
  YYSYMBOL_COMMENT = 106,                  /* COMMENT  */
  YYSYMBOL_MACRO = 107,                    /* MACRO  */
  YYSYMBOL_ENDM = 108,                     /* ENDM  */
  YYSYMBOL_PRAGMA = 109,                   /* PRAGMA  */
  YYSYMBOL_MACRO_CALL = 110,               /* MACRO_CALL  */
  YYSYMBOL_DOTDOTDOT = 111,                /* DOTDOTDOT  */
  YYSYMBOL_NEXT_ARG = 112,                 /* NEXT_ARG  */
  YYSYMBOL_ARG_VALUE = 113,                /* ARG_VALUE  */
  YYSYMBOL_ASCII = 114,                    /* ASCII  */
  YYSYMBOL_ASCIZ = 115,                    /* ASCIZ  */
  YYSYMBOL_SAVE_PSECT = 116,               /* SAVE_PSECT  */
  YYSYMBOL_SAVE_DSECT = 117,               /* SAVE_DSECT  */
  YYSYMBOL_RESTORE_PSECT = 118,            /* RESTORE_PSECT  */
  YYSYMBOL_RESTORE_DSECT = 119,            /* RESTORE_DSECT  */
  YYSYMBOL_PSECT = 120,                    /* PSECT  */
  YYSYMBOL_DSECT = 121,                    /* DSECT  */
  YYSYMBOL_LINK_ADDRESS = 122,             /* LINK_ADDRESS  */
  YYSYMBOL_LINK_AFTER = 123,               /* LINK_AFTER  */
  YYSYMBOL_LINK_FIRST = 124,               /* LINK_FIRST  */
  YYSYMBOL_LINK_LAST = 125,                /* LINK_LAST  */
  YYSYMBOL_LINK_ATTR = 126,                /* LINK_ATTR  */
  YYSYMBOL_LINK_OFFSET = 127,              /* LINK_OFFSET  */
  YYSYMBOL_LINK_REGION = 128,              /* LINK_REGION  */
  YYSYMBOL_LINK_SIZE = 129,                /* LINK_SIZE  */
  YYSYMBOL_HEADER = 130,                   /* HEADER  */
  YYSYMBOL_ENDH = 131,                     /* ENDH  */
  YYSYMBOL_OVERLAP_OK = 132,               /* OVERLAP_OK  */
  YYSYMBOL_DIRECTLY_AFTER = 133,           /* DIRECTLY_AFTER  */
  YYSYMBOL_MAC_NONE = 134,                 /* MAC_NONE  */
  YYSYMBOL_MAC_ONCE = 135,                 /* MAC_ONCE  */
  YYSYMBOL_MAC_UNLIMITED = 136,            /* MAC_UNLIMITED  */
  YYSYMBOL_LEXER_DEBUG_ON = 137,           /* LEXER_DEBUG_ON  */
  YYSYMBOL_LEXER_DEBUG_OFF = 138,          /* LEXER_DEBUG_OFF  */
  YYSYMBOL_PARSER_DEBUG_ON = 139,          /* PARSER_DEBUG_ON  */
  YYSYMBOL_PARSER_DEBUG_OFF = 140,         /* PARSER_DEBUG_OFF  */
  YYSYMBOL_WARN = 141,                     /* WARN  */
  YYSYMBOL_ERROR = 142,                    /* ERROR  */
  YYSYMBOL_PRINT = 143,                    /* PRINT  */
  YYSYMBOL_B = 144,                        /* B  */
  YYSYMBOL_NB = 145,                       /* NB  */
  YYSYMBOL_IDN = 146,                      /* IDN  */
  YYSYMBOL_DIF = 147,                      /* DIF  */
  YYSYMBOL_IIF = 148,                      /* IIF  */
  YYSYMBOL_ARCH = 149,                     /* ARCH  */
  YYSYMBOL_A_EV4 = 150,                    /* A_EV4  */
  YYSYMBOL_A_EV5 = 151,                    /* A_EV5  */
  YYSYMBOL_A_EV6 = 152,                    /* A_EV6  */
  YYSYMBOL_153_ = 153,                     /* '+'  */
  YYSYMBOL_154_ = 154,                     /* '-'  */
  YYSYMBOL_155_ = 155,                     /* '@'  */
  YYSYMBOL_156_ = 156,                     /* '&'  */
  YYSYMBOL_157_ = 157,                     /* '!'  */
  YYSYMBOL_158_ = 158,                     /* '\\'  */
  YYSYMBOL_159_ = 159,                     /* '*'  */
  YYSYMBOL_160_ = 160,                     /* '/'  */
  YYSYMBOL_UMINUS = 161,                   /* UMINUS  */
  YYSYMBOL_162_ = 162,                     /* ':'  */
  YYSYMBOL_163_ = 163,                     /* ','  */
  YYSYMBOL_164_ = 164,                     /* '('  */
  YYSYMBOL_165_ = 165,                     /* ')'  */
  YYSYMBOL_166_ = 166,                     /* '#'  */
  YYSYMBOL_167_ = 167,                     /* '='  */
  YYSYMBOL_168_ = 168,                     /* '<'  */
  YYSYMBOL_169_ = 169,                     /* '>'  */
  YYSYMBOL_170_ = 170,                     /* '~'  */
  YYSYMBOL_171_ = 171,                     /* '?'  */
  YYSYMBOL_YYACCEPT = 172,                 /* $accept  */
  YYSYMBOL_toplevel = 173,                 /* toplevel  */
  YYSYMBOL_sequence = 174,                 /* sequence  */
  YYSYMBOL_action = 175,                   /* action  */
  YYSYMBOL_label = 176,                    /* label  */
  YYSYMBOL_instruction = 177,              /* instruction  */
  YYSYMBOL_178_1 = 178,                    /* $@1  */
  YYSYMBOL_179_2 = 179,                    /* $@2  */
  YYSYMBOL_180_3 = 180,                    /* $@3  */
  YYSYMBOL_181_4 = 181,                    /* $@4  */
  YYSYMBOL_182_5 = 182,                    /* $@5  */
  YYSYMBOL_183_6 = 183,                    /* $@6  */
  YYSYMBOL_184_7 = 184,                    /* $@7  */
  YYSYMBOL_185_8 = 185,                    /* $@8  */
  YYSYMBOL_186_9 = 186,                    /* $@9  */
  YYSYMBOL_187_10 = 187,                   /* $@10  */
  YYSYMBOL_188_11 = 188,                   /* $@11  */
  YYSYMBOL_189_12 = 189,                   /* $@12  */
  YYSYMBOL_190_13 = 190,                   /* $@13  */
  YYSYMBOL_191_14 = 191,                   /* $@14  */
  YYSYMBOL_192_15 = 192,                   /* $@15  */
  YYSYMBOL_193_16 = 193,                   /* $@16  */
  YYSYMBOL_194_17 = 194,                   /* $@17  */
  YYSYMBOL_195_18 = 195,                   /* $@18  */
  YYSYMBOL_196_19 = 196,                   /* $@19  */
  YYSYMBOL_ifreg = 197,                    /* ifreg  */
  YYSYMBOL_three_freg = 198,               /* three_freg  */
  YYSYMBOL_199_20 = 199,                   /* $@20  */
  YYSYMBOL_200_21 = 200,                   /* $@21  */
  YYSYMBOL_two_freg = 201,                 /* two_freg  */
  YYSYMBOL_202_22 = 202,                   /* $@22  */
  YYSYMBOL_intreg = 203,                   /* intreg  */
  YYSYMBOL_204_23 = 204,                   /* $@23  */
  YYSYMBOL_205_24 = 205,                   /* $@24  */
  YYSYMBOL_two_ireg = 206,                 /* two_ireg  */
  YYSYMBOL_207_25 = 207,                   /* $@25  */
  YYSYMBOL_disp10 = 208,                   /* disp10  */
  YYSYMBOL_disp12 = 209,                   /* disp12  */
  YYSYMBOL_disp14 = 210,                   /* disp14  */
  YYSYMBOL_disp16 = 211,                   /* disp16  */
  YYSYMBOL_disp21 = 212,                   /* disp21  */
  YYSYMBOL_disp26 = 213,                   /* disp26  */
  YYSYMBOL_hint = 214,                     /* hint  */
  YYSYMBOL_mode = 215,                     /* mode  */
  YYSYMBOL_ipr = 216,                      /* ipr  */
  YYSYMBOL_scbd6 = 217,                    /* scbd6  */
  YYSYMBOL_ireg = 218,                     /* ireg  */
  YYSYMBOL_freg = 219,                     /* freg  */
  YYSYMBOL_assignment = 220,               /* assignment  */
  YYSYMBOL_expr = 221,                     /* expr  */
  YYSYMBOL_element = 222,                  /* element  */
  YYSYMBOL_absexpr = 223,                  /* absexpr  */
  YYSYMBOL_directive = 224,                /* directive  */
  YYSYMBOL_225_26 = 225,                   /* $@26  */
  YYSYMBOL_226_27 = 226,                   /* $@27  */
  YYSYMBOL_227_28 = 227,                   /* $@28  */
  YYSYMBOL_228_29 = 228,                   /* $@29  */
  YYSYMBOL_229_30 = 229,                   /* $@30  */
  YYSYMBOL_230_31 = 230,                   /* $@31  */
  YYSYMBOL_231_32 = 231,                   /* $@32  */
  YYSYMBOL_232_33 = 232,                   /* $@33  */
  YYSYMBOL_233_34 = 233,                   /* $@34  */
  YYSYMBOL_234_35 = 234,                   /* $@35  */
  YYSYMBOL_235_36 = 235,                   /* $@36  */
  YYSYMBOL_236_37 = 236,                   /* $@37  */
  YYSYMBOL_237_38 = 237,                   /* $@38  */
  YYSYMBOL_238_39 = 238,                   /* $@39  */
  YYSYMBOL_arch_flag = 239,                /* arch_flag  */
  YYSYMBOL_pragma_flags = 240,             /* pragma_flags  */
  YYSYMBOL_pragma_flag = 241,              /* pragma_flag  */
  YYSYMBOL_link_flags = 242,               /* link_flags  */
  YYSYMBOL_link_flag = 243,                /* link_flag  */
  YYSYMBOL_list_flags = 244,               /* list_flags  */
  YYSYMBOL_list_flag = 245,                /* list_flag  */
  YYSYMBOL_string_seq = 246,               /* string_seq  */
  YYSYMBOL_string_ele = 247,               /* string_ele  */
  YYSYMBOL_if_condition = 248,             /* if_condition  */
  YYSYMBOL_while_condition = 249,          /* while_condition  */
  YYSYMBOL_opt_comma = 250,                /* opt_comma  */
  YYSYMBOL_opt_arg_value = 251,            /* opt_arg_value  */
  YYSYMBOL_mainbody = 252,                 /* mainbody  */
  YYSYMBOL_ifsubbody_seq = 253,            /* ifsubbody_seq  */
  YYSYMBOL_ifsubbody = 254,                /* ifsubbody  */
  YYSYMBOL_alignbound = 255,               /* alignbound  */
  YYSYMBOL_opt_alignfill = 256,            /* opt_alignfill  */
  YYSYMBOL_mem_exprseq = 257,              /* mem_exprseq  */
  YYSYMBOL_opt_absexpr = 258,              /* opt_absexpr  */
  YYSYMBOL_psect_args = 259,               /* psect_args  */
  YYSYMBOL_opt_macro_arglist = 260,        /* opt_macro_arglist  */
  YYSYMBOL_macro_arglist = 261,            /* macro_arglist  */
  YYSYMBOL_macro_arg = 262,                /* macro_arg  */
  YYSYMBOL_arg_default = 263,              /* arg_default  */
  YYSYMBOL_opt_macro_call_arglist = 264,   /* opt_macro_call_arglist  */
  YYSYMBOL_arg_value = 265,                /* arg_value  */
  YYSYMBOL_macro_call_arg = 266            /* macro_call_arg  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  227
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   865

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  172
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  95
/* YYNRULES -- Number of rules.  */
#define YYNRULES  292
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  590

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   408


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   157,     2,   166,     2,     2,   156,     2,
     164,   165,   159,   153,   163,   154,     2,   160,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   162,     2,
     168,   167,   169,   171,   155,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,   158,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,   170,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   161
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   152,   152,   155,   157,   158,   162,   163,   164,   165,
     166,   167,   168,   169,   173,   176,   178,   179,   183,   184,
     185,   185,   187,   187,   189,   189,   191,   192,   193,   193,
     195,   195,   197,   197,   199,   199,   201,   202,   203,   203,
     205,   205,   207,   207,   209,   210,   210,   212,   213,   214,
     215,   216,   217,   218,   226,   226,   228,   228,   230,   230,
     232,   232,   234,   235,   235,   237,   237,   239,   239,   241,
     241,   246,   247,   248,   258,   259,   258,   264,   264,   269,
     269,   271,   271,   276,   276,   280,   282,   285,   287,   291,
     294,   296,   300,   304,   307,   309,   312,   314,   318,   325,
     327,   335,   336,   345,   346,   355,   356,   360,   361,   362,
     363,   364,   365,   366,   367,   368,   369,   370,   371,   372,
     373,   374,   378,   379,   380,   381,   385,   395,   396,   397,
     398,   398,   400,   401,   401,   403,   403,   405,   405,   414,
     424,   426,   426,   426,   428,   429,   430,   432,   443,   444,
     444,   446,   446,   448,   448,   450,   451,   452,   453,   454,
     455,   456,   457,   458,   459,   460,   461,   462,   462,   465,
     465,   467,   468,   468,   470,   471,   471,   473,   474,   475,
     476,   477,   478,   479,   480,   481,   482,   483,   484,   485,
     486,   487,   488,   488,   493,   494,   495,   499,   500,   504,
     505,   506,   507,   508,   509,   510,   514,   515,   519,   520,
     524,   525,   529,   530,   531,   532,   533,   534,   535,   539,
     540,   544,   545,   548,   549,   550,   551,   552,   553,   554,
     555,   556,   557,   558,   559,   563,   564,   565,   566,   567,
     568,   569,   570,   571,   572,   573,   574,   577,   579,   583,
     584,   594,   604,   605,   606,   615,   616,   617,   621,   622,
     623,   624,   625,   626,   630,   631,   635,   636,   640,   641,
     645,   646,   649,   651,   655,   656,   660,   664,   668,   672,
     676,   683,   684,   685,   689,   690,   694,   695,   699,   700,
     711,   712,   723
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "ENDP", "INS_FOP",
  "INS_FP2", "INS_IOP", "INS_MEM", "INS_JSR", "INS_BRN", "INS_M1A",
  "INS_M1B", "INS_MIS", "INS_FPCR", "INS_RPCC", "INS_SEXT", "INS_IP2",
  "INS_IP3", "INS_FTI", "INS_LSB", "INS_SSB", "INS_ITF", "INS_UNOP",
  "INS_CALLPAL", "INS_IPR4", "INS_HWM4", "INS_IPR5", "INS_HWM5",
  "INS_REI6", "INS_MTI6", "INS_MFI6", "INS_HWM6", "INS_1REG", "IREG",
  "FREG", "ADDR", "QUAD", "LONG", "BLKL", "BLKQ", "IDENT", "TITLE", "WORD",
  "BYTE", "BODYTEXT", "K_BYTE", "K_WORD", "K_LONG", "K_QUAD", "K_PAGE",
  "LOCAL_LABEL", "LOCAL_BLOCK", "NOLIST", "LIST", "MEB", "MC", "CND", "MD",
  "ME", "ASSIGN", "CMDIN", "NODEBUG", "K_DEBUG", "NUMBER", "IF", "WHILE",
  "BREAK", "CONTINUE", "INCLUDE", "INCLUDE_DIR", "MINCLUDE", "OPEN_ANGLE",
  "CLOSE_ANGLE", "PLUS", "MINUS", "SHIFT", "EQUAL", "NOT", "LOG_AND",
  "LOG_OR", "LOG_NOT", "COLON", "SYMBOL", "LABEL", "STRING", "EOL", "EOE",
  "DOT", "EQ", "GT", "LT", "DF", "NE", "GE", "LE", "NDF", "ENDC", "ENDW",
  "ALIGN", "IFF", "IFT", "IFTF", "REPEAT", "ENDR", "ENDI", "DOCMD",
  "COMMENT", "MACRO", "ENDM", "PRAGMA", "MACRO_CALL", "DOTDOTDOT",
  "NEXT_ARG", "ARG_VALUE", "ASCII", "ASCIZ", "SAVE_PSECT", "SAVE_DSECT",
  "RESTORE_PSECT", "RESTORE_DSECT", "PSECT", "DSECT", "LINK_ADDRESS",
  "LINK_AFTER", "LINK_FIRST", "LINK_LAST", "LINK_ATTR", "LINK_OFFSET",
  "LINK_REGION", "LINK_SIZE", "HEADER", "ENDH", "OVERLAP_OK",
  "DIRECTLY_AFTER", "MAC_NONE", "MAC_ONCE", "MAC_UNLIMITED",
  "LEXER_DEBUG_ON", "LEXER_DEBUG_OFF", "PARSER_DEBUG_ON",
  "PARSER_DEBUG_OFF", "WARN", "ERROR", "PRINT", "B", "NB", "IDN", "DIF",
  "IIF", "ARCH", "A_EV4", "A_EV5", "A_EV6", "'+'", "'-'", "'@'", "'&'",
  "'!'", "'\\\\'", "'*'", "'/'", "UMINUS", "':'", "','", "'('", "')'",
  "'#'", "'='", "'<'", "'>'", "'~'", "'?'", "$accept", "toplevel",
  "sequence", "action", "label", "instruction", "$@1", "$@2", "$@3", "$@4",
  "$@5", "$@6", "$@7", "$@8", "$@9", "$@10", "$@11", "$@12", "$@13",
  "$@14", "$@15", "$@16", "$@17", "$@18", "$@19", "ifreg", "three_freg",
  "$@20", "$@21", "two_freg", "$@22", "intreg", "$@23", "$@24", "two_ireg",
  "$@25", "disp10", "disp12", "disp14", "disp16", "disp21", "disp26",
  "hint", "mode", "ipr", "scbd6", "ireg", "freg", "assignment", "expr",
  "element", "absexpr", "directive", "$@26", "$@27", "$@28", "$@29",
  "$@30", "$@31", "$@32", "$@33", "$@34", "$@35", "$@36", "$@37", "$@38",
  "$@39", "arch_flag", "pragma_flags", "pragma_flag", "link_flags",
  "link_flag", "list_flags", "list_flag", "string_seq", "string_ele",
  "if_condition", "while_condition", "opt_comma", "opt_arg_value",
  "mainbody", "ifsubbody_seq", "ifsubbody", "alignbound", "opt_alignfill",
  "mem_exprseq", "opt_absexpr", "psect_args", "opt_macro_arglist",
  "macro_arglist", "macro_arg", "arg_default", "opt_macro_call_arglist",
  "arg_value", "macro_call_arg", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-426)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-39)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     566,   -67,   -57,   175,   175,   121,    60,   121,    60,   121,
    -107,  -426,   175,   121,    88,   121,   121,   175,   121,   121,
     121,    32,    32,   121,   121,   121,   121,   149,   121,   121,
     121,   121,  -426,  -426,  -426,    32,    32,   -25,    -7,   -63,
      19,    31,    32,    54,    56,   362,  -426,    65,    67,    64,
      69,    72,  -124,  -426,    -6,   245,    32,    82,    85,  -426,
    -426,    79,  -426,  -426,    -2,    37,    84,    98,   105,   108,
      32,   109,   107,   112,  -426,    32,    32,    32,   119,   120,
      32,    32,    32,   362,   -20,   206,   716,  -426,   122,   125,
    -426,  -426,  -426,  -426,  -426,  -426,  -426,    32,    32,  -426,
    -426,    32,    32,  -426,    48,   285,  -426,  -426,  -426,    50,
    -426,    70,  -426,  -426,  -426,    71,  -426,    74,    78,  -426,
     121,  -426,    48,    81,    32,  -426,    83,  -426,    87,    90,
      95,   111,   113,   114,    66,   285,  -426,   285,   115,   117,
     142,   143,   121,   144,   146,   147,   148,  -426,    32,    32,
      32,   150,   158,  -426,  -426,    86,  -426,   -24,  -426,   -24,
     160,  -426,  -426,    32,    32,    32,   145,    32,    32,    32,
     169,   151,   152,   153,   155,   176,   374,  -426,  -426,   186,
     187,   188,   126,    32,    32,  -426,  -426,  -426,  -426,  -426,
    -426,   156,  -426,   199,   227,   157,   343,    36,  -426,   -68,
     -68,   236,  -426,   238,  -426,  -426,  -426,   161,   161,   241,
     243,  -426,  -426,   -93,   246,   248,   257,  -426,  -426,   239,
     242,   244,   191,  -426,  -426,  -426,   264,  -426,  -426,  -426,
    -426,  -426,  -426,  -426,   110,  -426,    32,    32,    32,    32,
      32,    32,    32,    32,    32,    32,  -426,  -426,  -426,    68,
    -426,   190,  -426,   181,  -426,  -426,  -426,  -426,  -426,  -426,
    -426,   121,  -426,  -426,  -426,  -426,   192,  -426,  -426,  -426,
    -426,   285,   -76,   -65,   -58,  -426,  -426,  -426,  -426,  -426,
    -426,  -426,  -426,  -426,  -426,   334,  -426,   416,   307,  -426,
    -426,  -426,  -426,  -426,  -426,  -426,  -426,   -72,   -72,   -72,
     -72,   312,    32,    32,    32,   278,    32,    32,    32,   283,
     203,   204,   205,   207,  -426,  -426,  -426,  -426,  -426,   268,
     286,    32,   287,   329,  -426,  -426,  -426,  -426,  -426,  -426,
    -426,  -426,  -426,  -426,   162,  -426,   211,  -426,   292,   -56,
    -426,  -426,  -426,    32,   -62,  -426,   -60,  -426,  -426,   245,
     294,   296,  -426,  -426,  -426,  -426,   -31,  -426,  -426,  -426,
    -426,   298,   300,   301,   352,  -426,  -426,   175,  -426,  -426,
    -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,   175,    88,
      32,   233,    32,    32,  -426,   234,  -426,   121,   121,   121,
     121,   237,   250,   175,   235,    32,    32,    32,    32,  -426,
     251,    32,    32,    32,  -426,    32,  -426,  -426,  -426,  -426,
    -426,   299,  -426,  -426,  -426,  -426,   249,   253,  -426,  -426,
     285,   285,   285,  -426,   285,   285,   285,  -426,   -72,   -72,
     -72,   -72,   320,  -426,  -426,  -426,  -426,   304,   -74,  -426,
    -426,    44,  -426,  -426,    36,   240,  -426,  -426,  -426,  -426,
    -426,  -426,  -426,  -426,  -426,  -426,  -426,   323,   254,  -426,
    -426,   256,   121,   265,   285,  -426,   285,   121,   121,  -426,
    -426,  -426,  -426,   121,   121,  -426,  -426,  -426,  -426,   270,
     285,  -426,   271,   285,   121,   255,   255,   272,   285,   361,
     -72,   -72,    80,  -426,  -426,   274,   284,   404,   364,   291,
     -75,   377,   378,   376,   321,  -426,   403,  -426,  -426,  -426,
    -426,  -426,   121,   322,   121,   324,  -426,   325,   326,   121,
     121,   327,    32,  -426,  -426,   121,  -426,  -426,  -426,   442,
     444,   449,    59,  -426,   -72,   -72,   397,  -426,   -71,  -426,
     291,  -426,   291,  -426,   451,   -74,  -426,   175,   331,   335,
     332,  -426,  -426,  -426,   337,   338,   335,  -426,   339,  -426,
    -426,  -426,   414,  -426,  -426,  -426,   415,  -426,  -426,  -426,
    -426,   405,  -426,  -426,  -426,    32,  -426,  -426,  -426,   349,
    -426,  -426,  -426,  -426,   429,  -426,    32,  -426,  -426,   285
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    50,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    90,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   149,   151,   153,     0,     0,     0,     0,     0,
     172,   175,     0,     0,     0,     0,   141,     0,     0,     0,
       0,     0,     0,     6,     0,     0,   268,     0,     0,   130,
     169,     0,   133,   135,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   192,     0,     0,     0,     0,     0,
     268,   268,   268,     0,     0,     0,     0,     4,     7,     0,
      10,    11,    13,    12,   103,   124,   122,     0,     0,   123,
     125,     0,     0,    18,     0,   126,   107,   104,    19,     0,
     101,     0,   102,    71,    72,     0,    73,     0,     0,    48,
       0,    53,    52,    47,     0,    26,     0,    27,     0,     0,
       0,     0,     0,     0,    37,    91,    44,    93,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    51,     0,     0,
       0,     0,     0,   180,   181,    16,   174,     0,   177,     0,
       0,   178,   179,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   144,   145,     0,
       0,     0,    14,     0,     0,   259,   260,   261,   262,   263,
     258,   264,   269,     0,     0,   247,     0,   288,   171,     0,
       0,     0,   159,     0,   160,   163,   164,   270,   270,     0,
       0,   190,   191,     0,     0,     0,     0,   183,   184,     0,
       0,     0,     0,   194,   195,   196,     0,     1,     5,     8,
       9,   118,   119,   120,     0,    74,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    77,    20,    34,    40,
      42,     0,    45,     0,    79,    83,    28,    22,    30,    32,
      24,     0,    54,    56,    58,    60,     0,    63,    65,    67,
      69,   266,     0,     0,     0,   165,   166,    17,   212,   213,
     214,   215,   216,   218,   217,     0,   210,     0,     0,   223,
     225,   226,   229,   224,   227,   228,   230,   249,   249,   249,
     249,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   142,   127,   128,   129,    15,     0,
       0,     0,     0,     0,   147,   248,   167,   199,   200,   201,
     202,   203,   204,   205,     0,   197,   286,   287,     0,     0,
     290,   284,   221,     0,     0,   219,     0,   161,   162,     0,
       0,     0,   185,   189,   208,   209,     0,   206,   186,   187,
     188,     0,     0,     0,     0,   132,   121,     0,   116,   117,
     108,   109,   112,   113,   114,   115,   110,   111,     0,     0,
      90,     0,     0,     0,    49,     0,    81,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    87,     0,    85,    62,
       0,     0,     0,    87,   150,     0,   152,   154,   173,   211,
     176,     0,   286,   231,   250,   232,     0,     0,   251,   137,
     235,   237,   238,   241,   236,   239,   240,   242,   249,   249,
     249,   249,     0,   105,   106,   265,   148,     0,   272,   131,
     198,   249,   289,   170,   288,     0,   134,   220,   136,   271,
     158,   182,   193,   207,   155,   156,   157,     0,     0,    78,
      21,     0,     0,     0,    89,    43,    92,     0,     0,    80,
      84,    29,    23,     0,     0,    25,    36,    55,    98,     0,
      88,    59,     0,    86,     0,    99,    99,     0,   267,     0,
     249,   249,   252,   243,   244,     0,     0,     0,     0,   281,
     281,     0,     0,     0,   273,   274,     0,   292,   285,   222,
     139,    75,     0,     0,     0,     0,    82,     0,     0,     0,
       0,     0,     0,    66,    68,     0,   146,   233,   234,     0,
       0,     0,     0,   253,   249,   249,     0,   140,     0,   276,
     281,   279,   281,   280,     0,     0,   291,     0,     0,    94,
       0,    46,    31,    33,     0,     0,    94,   100,     0,   255,
     256,   257,     0,   254,   245,   246,     0,   283,   282,   278,
     277,     0,   275,    76,    35,     0,    39,    41,    57,    96,
      64,    70,   138,   143,     0,    95,     0,    61,   168,    97
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -426,  -426,  -426,   436,  -426,  -426,  -426,  -426,  -426,  -426,
    -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,
    -426,  -426,  -426,  -426,  -426,   507,   511,  -426,  -426,  -426,
    -426,   154,  -426,  -426,   135,  -426,  -426,   123,   -50,   159,
    -426,  -426,   -28,  -426,  -277,    46,    43,     2,  -426,   -21,
    -426,     9,  -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,
    -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,   195,
    -426,   174,   375,  -232,   336,  -233,   452,  -426,  -426,  -295,
    -426,  -426,     5,   189,  -426,  -103,    63,   333,  -426,  -426,
      -5,  -425,  -426,  -195,    99
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    85,    86,    87,    88,    89,   379,   390,   393,   389,
     391,   392,   380,   381,   382,   383,   385,   395,   396,   397,
     398,   400,   401,   402,   403,   115,   103,   367,   547,   108,
     378,   125,   387,   468,   127,   388,   482,   479,   463,   134,
     465,   136,   576,   587,   477,   523,   126,   104,    90,   105,
     106,   112,    91,   196,   199,   200,   492,   176,   432,   148,
     149,   150,   438,   197,   157,   159,   213,   226,   334,   335,
     356,   357,   285,   286,   344,   345,   175,   314,   326,   413,
     419,   532,   533,   191,   322,   272,   193,   350,   503,   504,
     505,   539,   339,   414,   341
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     135,   137,   340,   415,   416,   417,   109,   540,   499,   404,
     412,   567,   107,   107,   122,   116,   342,   116,    92,   130,
     406,   107,   342,   446,   342,   448,   107,   407,    93,   443,
     278,   279,   280,   281,   282,   283,   284,   500,   182,   354,
     355,   337,   568,   183,   151,   152,   273,   274,   111,   201,
     117,   160,   119,   409,   452,   409,   123,   120,   128,   129,
     153,   131,   132,   133,   190,   192,   138,   139,   140,   141,
     143,   144,   145,   146,   147,   541,   231,   232,   154,   209,
     233,   234,    95,   202,   214,   215,   216,   405,   203,   192,
     192,   192,   538,   113,   114,    96,   501,   502,   405,   155,
     343,   354,   355,   253,   156,   405,   343,   444,   343,    97,
      95,   447,    98,   447,    99,   569,   158,   570,   336,   100,
     481,   110,   204,    96,   485,   486,   412,   271,   271,   271,
     223,   224,   225,   493,   494,   495,   496,    97,    95,   161,
      98,   162,    99,   219,   220,   221,   507,   100,   179,   337,
     177,    96,   178,   180,   110,   562,   181,   337,   529,   530,
     531,   184,   319,   251,   198,    97,   194,   195,    98,   205,
      99,    95,   289,   290,   291,   100,   293,   294,   295,   529,
     530,   531,   110,   206,    96,   266,   101,   207,   236,   237,
     208,   210,   211,   320,   338,   527,   528,   212,    97,    95,
     102,    98,   506,    99,   217,   218,   227,   229,   100,    94,
     230,   235,    96,   246,   101,   368,   369,   370,   371,   372,
     373,   374,   375,   376,   377,    95,    97,   292,   102,    98,
     261,    99,   -38,   247,   248,   275,   100,   249,    96,   564,
     565,   250,   101,   276,   252,   288,   254,   439,   277,   340,
     255,   296,    97,   256,   124,    98,   102,    99,   257,   236,
     237,   301,   100,   238,   239,   240,   241,   242,   243,   244,
     245,   315,   316,   317,   258,   101,   259,   260,   262,   366,
     263,   420,   421,   422,   323,   424,   425,   426,   318,   102,
     185,   186,   187,   188,   189,    95,   327,   328,   329,   330,
     331,   332,   333,   101,   394,   264,   265,   267,    96,   268,
     269,   270,   324,   142,   297,   298,   299,   102,   300,   321,
     325,   347,    97,   348,   349,    98,   352,    99,   353,   101,
     435,   358,   100,   359,   238,   239,   240,   241,   242,   243,
     244,   245,   360,   102,   386,   361,   236,   237,   362,   365,
     363,   411,   445,   433,   364,   384,   418,   399,   190,   135,
     423,   464,   466,   236,   237,   427,   428,   429,   430,   458,
     431,   434,   436,   437,   442,   480,   107,   483,   441,   450,
     459,   451,   480,   454,   488,   455,   456,   107,   278,   279,
     280,   281,   282,   283,   284,   475,   457,   462,   467,   101,
     476,   473,   107,   489,   478,   497,   478,   498,   510,   509,
     478,   478,   490,   102,   474,   484,   491,   511,   522,   408,
     512,   238,   239,   240,   241,   242,   243,   244,   245,   514,
     469,   470,   128,   472,   519,   520,   525,   534,   238,   239,
     240,   241,   242,   243,   244,   245,   526,   535,   536,   537,
     163,   164,   165,   166,   167,   168,   169,   170,   538,   542,
     543,   544,   302,   303,   304,   305,   306,   307,   308,   309,
     278,   279,   280,   281,   282,   283,   284,   327,   328,   329,
     330,   331,   332,   333,   545,   546,   559,   549,   560,   551,
     552,   553,   556,   561,   566,   571,   574,   577,   575,   582,
     583,   410,   578,   579,   581,   513,   171,   172,   173,   174,
     515,   516,   586,   584,   588,   118,   517,   518,   310,   311,
     312,   313,   228,   121,   471,   585,   487,   521,   580,   440,
     453,   557,   524,   460,   287,   222,   346,   563,   449,   461,
     572,   351,     0,   508,     0,     0,     0,     0,     0,   573,
       0,     0,     0,     0,   464,   548,   107,   550,     0,     0,
       0,     0,   554,   555,     0,   589,    -3,     1,   558,     2,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,     0,
       0,    32,    33,    34,    35,    36,    37,    38,     0,     0,
       0,     0,     0,     0,     0,     0,    39,     0,    40,    41,
       0,     0,     0,     0,     0,     0,    42,    43,    44,     0,
      45,    46,    47,    48,    49,    50,    51,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    52,     0,
       0,    53,     0,    54,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    55,     0,     0,     0,    56,     0,
       0,    57,     0,    58,     0,    59,    60,     0,    61,     0,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    80,    81,    82,
       0,     0,     0,     0,    83,    84,    -2,     1,     0,     2,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,     0,
       0,    32,    33,    34,    35,    36,    37,    38,     0,     0,
       0,     0,     0,     0,     0,     0,    39,     0,    40,    41,
       0,     0,     0,     0,     0,     0,    42,    43,    44,     0,
      45,    46,    47,    48,    49,    50,    51,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    52,     0,
       0,    53,     0,    54,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    55,     0,     0,     0,    56,     0,
       0,    57,     0,    58,     0,    59,    60,     0,    61,     0,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    80,    81,    82,
       0,     0,     0,     0,    83,    84
};

static const yytype_int16 yycheck[] =
{
      21,    22,   197,   298,   299,   300,     4,    82,    82,    85,
      82,    82,     3,     4,    12,     6,    84,     8,    85,    17,
      85,    12,    84,    85,    84,    85,    17,    85,    85,    85,
      54,    55,    56,    57,    58,    59,    60,   111,   162,   132,
     133,   113,   113,   167,    35,    36,   149,   150,     5,    51,
       7,    42,     9,   285,    85,   287,    13,   164,    15,    16,
      85,    18,    19,    20,    55,    56,    23,    24,    25,    26,
      27,    28,    29,    30,    31,   500,    97,    98,    85,    70,
     101,   102,    50,    85,    75,    76,    77,   163,    51,    80,
      81,    82,   167,    33,    34,    63,   170,   171,   163,   162,
     168,   132,   133,   124,    85,   163,   168,   163,   168,    77,
      50,   344,    80,   346,    82,   540,    85,   542,    82,    87,
     397,    33,    85,    63,   401,   402,    82,   148,   149,   150,
     150,   151,   152,   428,   429,   430,   431,    77,    50,    85,
      80,    85,    82,    80,    81,    82,   441,    87,    84,   113,
      85,    63,    85,    84,    33,    96,    84,   113,    99,   100,
     101,   167,   183,   120,    85,    77,    84,    82,    80,    85,
      82,    50,   163,   164,   165,    87,   167,   168,   169,    99,
     100,   101,    33,    85,    63,   142,   154,    82,    78,    79,
      82,    82,    85,   184,   158,   490,   491,    85,    77,    50,
     168,    80,   158,    82,    85,    85,     0,    85,    87,    34,
      85,   163,    63,   163,   154,   236,   237,   238,   239,   240,
     241,   242,   243,   244,   245,    50,    77,    82,   168,    80,
     164,    82,   164,   163,   163,    85,    87,   163,    63,   534,
     535,   163,   154,    85,   163,    85,   163,    85,   162,   444,
     163,    82,    77,   163,   166,    80,   168,    82,   163,    78,
      79,    85,    87,   153,   154,   155,   156,   157,   158,   159,
     160,    85,    85,    85,   163,   154,   163,   163,   163,   169,
     163,   302,   303,   304,    85,   306,   307,   308,   162,   168,
      45,    46,    47,    48,    49,    50,   134,   135,   136,   137,
     138,   139,   140,   154,   261,   163,   163,   163,    63,   163,
     163,   163,    85,   164,   163,   163,   163,   168,   163,   163,
     163,    85,    77,    85,   163,    80,    85,    82,    85,   154,
     321,    85,    87,    85,   153,   154,   155,   156,   157,   158,
     159,   160,    85,   168,   163,   106,    78,    79,   106,    85,
     106,    44,   343,    85,   163,   165,    44,   165,   349,   380,
      82,   382,   383,    78,    79,    82,   163,   163,   163,   367,
     163,    85,    85,    44,    82,   396,   367,   398,   167,    85,
     378,    85,   403,    85,   405,    85,    85,   378,    54,    55,
      56,    57,    58,    59,    60,   393,    44,   164,   164,   154,
     165,   164,   393,   104,   395,    85,   397,   103,    85,   169,
     401,   402,   163,   168,   164,   164,   163,   163,   163,    85,
     164,   153,   154,   155,   156,   157,   158,   159,   160,   164,
     387,   388,   389,   390,   164,   164,   164,   163,   153,   154,
     155,   156,   157,   158,   159,   160,    85,   163,    44,    85,
      88,    89,    90,    91,    92,    93,    94,    95,   167,    82,
      82,    85,    88,    89,    90,    91,    92,    93,    94,    95,
      54,    55,    56,    57,    58,    59,    60,   134,   135,   136,
     137,   138,   139,   140,   163,    82,    44,   165,    44,   165,
     165,   165,   165,    44,    97,    44,   165,   165,   163,    85,
      85,    85,   165,   165,   165,   462,   144,   145,   146,   147,
     467,   468,   163,   108,    85,     8,   473,   474,   144,   145,
     146,   147,    86,    12,   389,   575,   403,   484,   556,   334,
     356,   522,   486,   379,   159,    83,   200,   532,   349,   380,
     545,   208,    -1,   444,    -1,    -1,    -1,    -1,    -1,   547,
      -1,    -1,    -1,    -1,   575,   512,   547,   514,    -1,    -1,
      -1,    -1,   519,   520,    -1,   586,     0,     1,   525,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    -1,
      -1,    35,    36,    37,    38,    39,    40,    41,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    50,    -1,    52,    53,
      -1,    -1,    -1,    -1,    -1,    -1,    60,    61,    62,    -1,
      64,    65,    66,    67,    68,    69,    70,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    82,    -1,
      -1,    85,    -1,    87,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    98,    -1,    -1,    -1,   102,    -1,
      -1,   105,    -1,   107,    -1,   109,   110,    -1,   112,    -1,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   141,   142,   143,
      -1,    -1,    -1,    -1,   148,   149,     0,     1,    -1,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    -1,
      -1,    35,    36,    37,    38,    39,    40,    41,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    50,    -1,    52,    53,
      -1,    -1,    -1,    -1,    -1,    -1,    60,    61,    62,    -1,
      64,    65,    66,    67,    68,    69,    70,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    82,    -1,
      -1,    85,    -1,    87,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    98,    -1,    -1,    -1,   102,    -1,
      -1,   105,    -1,   107,    -1,   109,   110,    -1,   112,    -1,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   141,   142,   143,
      -1,    -1,    -1,    -1,   148,   149
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     1,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    35,    36,    37,    38,    39,    40,    41,    50,
      52,    53,    60,    61,    62,    64,    65,    66,    67,    68,
      69,    70,    82,    85,    87,    98,   102,   105,   107,   109,
     110,   112,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     141,   142,   143,   148,   149,   173,   174,   175,   176,   177,
     220,   224,    85,    85,    34,    50,    63,    77,    80,    82,
      87,   154,   168,   198,   219,   221,   222,   223,   201,   219,
      33,   218,   223,    33,    34,   197,   223,   218,   197,   218,
     164,   198,   219,   218,   166,   203,   218,   206,   218,   218,
     219,   218,   218,   218,   211,   221,   213,   221,   218,   218,
     218,   218,   164,   218,   218,   218,   218,   218,   231,   232,
     233,   223,   223,    85,    85,   162,    85,   236,    85,   237,
     223,    85,    85,    88,    89,    90,    91,    92,    93,    94,
      95,   144,   145,   146,   147,   248,   229,    85,    85,    84,
      84,    84,   162,   167,   167,    45,    46,    47,    48,    49,
     223,   255,   223,   258,    84,    82,   225,   235,    85,   226,
     227,    51,    85,    51,    85,    85,    85,    82,    82,   223,
      82,    85,    85,   238,   223,   223,   223,    85,    85,   258,
     258,   258,   248,   150,   151,   152,   239,     0,   175,    85,
      85,   221,   221,   221,   221,   163,    78,    79,   153,   154,
     155,   156,   157,   158,   159,   160,   163,   163,   163,   163,
     163,   218,   163,   221,   163,   163,   163,   163,   163,   163,
     163,   164,   163,   163,   163,   163,   218,   163,   163,   163,
     163,   221,   257,   257,   257,    85,    85,   162,    54,    55,
      56,    57,    58,    59,    60,   244,   245,   244,    85,   223,
     223,   223,    82,   223,   223,   223,    82,   163,   163,   163,
     163,    85,    88,    89,    90,    91,    92,    93,    94,    95,
     144,   145,   146,   147,   249,    85,    85,    85,   162,   221,
     223,   163,   256,    85,    85,   163,   250,   134,   135,   136,
     137,   138,   139,   140,   240,   241,    82,   113,   158,   264,
     265,   266,    84,   168,   246,   247,   246,    85,    85,   163,
     259,   259,    85,    85,   132,   133,   242,   243,    85,    85,
      85,   106,   106,   106,   163,    85,   169,   199,   221,   221,
     221,   221,   221,   221,   221,   221,   221,   221,   202,   178,
     184,   185,   186,   187,   165,   188,   163,   204,   207,   181,
     179,   182,   183,   180,   218,   189,   190,   191,   192,   165,
     193,   194,   195,   196,    85,   163,    85,    85,    85,   245,
      85,    44,    82,   251,   265,   251,   251,   251,    44,   252,
     221,   221,   221,    82,   221,   221,   221,    82,   163,   163,
     163,   163,   230,    85,    85,   223,    85,    44,   234,    85,
     241,   167,    82,    85,   163,   223,    85,   247,    85,   255,
      85,    85,    85,   243,    85,    85,    85,    44,   219,   219,
     203,   211,   164,   210,   221,   212,   221,   164,   205,   218,
     218,   206,   218,   164,   164,   219,   165,   216,   223,   209,
     221,   216,   208,   221,   164,   216,   216,   209,   221,   104,
     163,   163,   228,   251,   251,   251,   251,    85,   103,    82,
     111,   170,   171,   260,   261,   262,   158,   251,   266,   169,
      85,   163,   164,   218,   164,   218,   218,   218,   218,   164,
     164,   218,   163,   217,   217,   164,    85,   251,   251,    99,
     100,   101,   253,   254,   163,   163,    44,    85,   167,   263,
      82,   263,    82,    82,    85,   163,    82,   200,   218,   165,
     218,   165,   165,   165,   218,   218,   165,   223,   218,    44,
      44,    44,    96,   254,   251,   251,    97,    82,   113,   263,
     263,    44,   262,   219,   165,   163,   214,   165,   165,   165,
     214,   165,    85,    85,   108,   210,   163,   215,    85,   221
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   172,   173,   174,   174,   174,   175,   175,   175,   175,
     175,   175,   175,   175,   176,   176,   176,   176,   177,   177,
     178,   177,   179,   177,   180,   177,   177,   177,   181,   177,
     182,   177,   183,   177,   184,   177,   177,   177,   185,   177,
     186,   177,   187,   177,   177,   188,   177,   177,   177,   177,
     177,   177,   177,   177,   189,   177,   190,   177,   191,   177,
     192,   177,   177,   193,   177,   194,   177,   195,   177,   196,
     177,   197,   197,   197,   199,   200,   198,   202,   201,   204,
     203,   205,   203,   207,   206,   208,   208,   209,   209,   210,
     211,   211,   212,   213,   214,   214,   215,   215,   216,   217,
     217,   218,   218,   219,   219,   220,   220,   221,   221,   221,
     221,   221,   221,   221,   221,   221,   221,   221,   221,   221,
     221,   221,   222,   222,   222,   222,   223,   224,   224,   224,
     225,   224,   224,   226,   224,   227,   224,   228,   224,   224,
     224,   229,   230,   224,   224,   224,   224,   224,   224,   231,
     224,   232,   224,   233,   224,   224,   224,   224,   224,   224,
     224,   224,   224,   224,   224,   224,   224,   234,   224,   235,
     224,   224,   236,   224,   224,   237,   224,   224,   224,   224,
     224,   224,   224,   224,   224,   224,   224,   224,   224,   224,
     224,   224,   238,   224,   239,   239,   239,   240,   240,   241,
     241,   241,   241,   241,   241,   241,   242,   242,   243,   243,
     244,   244,   245,   245,   245,   245,   245,   245,   245,   246,
     246,   247,   247,   248,   248,   248,   248,   248,   248,   248,
     248,   248,   248,   248,   248,   249,   249,   249,   249,   249,
     249,   249,   249,   249,   249,   249,   249,   250,   250,   251,
     251,   252,   253,   253,   253,   254,   254,   254,   255,   255,
     255,   255,   255,   255,   256,   256,   257,   257,   258,   258,
     259,   259,   260,   260,   261,   261,   262,   262,   262,   262,
     262,   263,   263,   263,   264,   264,   265,   265,   266,   266,
     266,   266,   266
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     0,     1,     2,     1,     1,     2,     2,
       1,     1,     2,     2,     2,     3,     2,     3,     2,     2,
       0,     5,     0,     5,     0,     5,     2,     2,     0,     5,
       0,     7,     0,     7,     0,     8,     5,     2,     0,     8,
       0,     8,     0,     5,     2,     0,     7,     2,     2,     4,
       1,     2,     2,     2,     0,     5,     0,     8,     0,     5,
       0,     9,     4,     0,     8,     0,     6,     0,     6,     0,
       8,     1,     1,     1,     0,     0,     7,     0,     4,     0,
       4,     0,     5,     0,     4,     0,     1,     0,     1,     1,
       0,     1,     1,     1,     0,     2,     0,     2,     1,     0,
       2,     1,     1,     1,     1,     4,     4,     1,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     2,     2,
       2,     3,     1,     1,     1,     1,     1,     3,     3,     3,
       0,     4,     3,     0,     4,     0,     4,     0,     8,     5,
       6,     0,     0,     8,     2,     2,     6,     3,     4,     0,
       4,     0,     4,     0,     4,     4,     4,     4,     4,     2,
       2,     3,     3,     2,     2,     3,     3,     0,     9,     0,
       4,     2,     0,     4,     2,     0,     4,     2,     2,     2,
       2,     2,     4,     2,     2,     3,     3,     3,     3,     3,
       2,     2,     0,     4,     1,     1,     1,     1,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     1,     1,
       1,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     1,     3,     2,     2,     2,     2,     2,     2,     2,
       2,     3,     3,     5,     5,     2,     2,     2,     2,     2,
       2,     2,     2,     3,     3,     5,     5,     0,     1,     0,
       1,     1,     0,     1,     2,     2,     2,     2,     1,     1,
       1,     1,     1,     1,     0,     2,     1,     3,     0,     1,
       0,     2,     0,     1,     1,     3,     2,     3,     3,     2,
       2,     0,     2,     2,     1,     3,     1,     1,     0,     2,
       1,     4,     3
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 6: /* action: EOL  */
                                { do_list(L_MISC, cur_ifile->lineno + cur_base_lineno); }
    break;

  case 8: /* action: label EOL  */
                                { do_list(L_BINARY | L_MISC, cur_ifile->lineno + cur_base_lineno); }
    break;

  case 9: /* action: instruction EOL  */
                                { do_list(L_BINARY | L_MISC, start_line); }
    break;

  case 10: /* action: assignment  */
                                { do_list(L_ASSIGN | L_MISC, start_line); saved_absexpr = 0; }
    break;

  case 11: /* action: directive  */
                                { /* do_list handled inside directive */ saved_absexpr = 0; }
    break;

  case 12: /* action: ENDP EOL  */
                                { do_list(L_MISC, start_line); return; }
    break;

  case 14: /* label: SYMBOL ':'  */
                                { define_label(yyvsp[-1], hpc, pc); 
                                  if (used_local_label) { local_label_blknum = ++max_blknum; used_local_label = 0; } 
                                }
    break;

  case 15: /* label: SYMBOL ':' ':'  */
                                { define_label(yyvsp[-2], hpc, pc); 
                                  if (used_local_label) { local_label_blknum = ++max_blknum; used_local_label = 0; } }
    break;

  case 16: /* label: LOCAL_LABEL ':'  */
                                { define_label(yyvsp[-1], hpc, pc); used_local_label = 1; }
    break;

  case 17: /* label: LOCAL_LABEL ':' ':'  */
                                { define_label(yyvsp[-2], hpc, pc); used_local_label = 1; }
    break;

  case 18: /* instruction: INS_FOP three_freg  */
                                { dbug(); write_mem_array(ic++, opcode); ipc(4); }
    break;

  case 19: /* instruction: INS_FP2 two_freg  */
                                { dbug(); write_mem_array(ic++, opcode); ipc(4); }
    break;

  case 20: /* $@1: %empty  */
                                { opcode |= regnum<<21;  ipc(4); }
    break;

  case 21: /* instruction: INS_IOP ireg ',' $@1 intreg  */
                                { dbug(); write_mem_array(ic++, opcode); }
    break;

  case 22: /* $@2: %empty  */
                                { opcode |= regnum<<21;  ipc(4); }
    break;

  case 23: /* instruction: INS_FTI freg ',' $@2 ireg  */
                                { dbug(); write_mem_array(ic++, opcode | regnum); }
    break;

  case 24: /* $@3: %empty  */
                                { opcode |= regnum<<21;  ipc(4); }
    break;

  case 25: /* instruction: INS_ITF ireg ',' $@3 freg  */
                                { dbug(); write_mem_array(ic++, opcode | regnum); }
    break;

  case 26: /* instruction: INS_SEXT intreg  */
                                { dbug(); write_mem_array(ic++, opcode); ipc(4); }
    break;

  case 27: /* instruction: INS_IP2 two_ireg  */
                                { dbug(); write_mem_array(ic++, opcode); ipc(4); }
    break;

  case 28: /* $@4: %empty  */
                                { opcode |= regnum<<21; ipc(4); }
    break;

  case 29: /* instruction: INS_IP3 ireg ',' $@4 two_ireg  */
                                { dbug(); write_mem_array(ic++, opcode); }
    break;

  case 30: /* $@5: %empty  */
                                { opcode |= regnum; ipc(4); }
    break;

  case 31: /* instruction: INS_LSB ireg ',' $@5 '(' ireg ')'  */
                                { dbug(); write_mem_array(ic++, opcode | (regnum<<16)); }
    break;

  case 32: /* $@6: %empty  */
                                { opcode |= regnum<<21; ipc(4); }
    break;

  case 33: /* instruction: INS_SSB ireg ',' $@6 '(' ireg ')'  */
                                { dbug(); write_mem_array(ic++, opcode | (regnum<<16)); }
    break;

  case 34: /* $@7: %empty  */
                                { opcode |= regnum<<21; ipc(4); }
    break;

  case 35: /* instruction: INS_MEM ifreg ',' $@7 disp16 '(' ireg ')'  */
                                { dbug(); write_mem_array(ic++, opcode | (regnum<<16)); }
    break;

  case 36: /* instruction: INS_UNOP disp16 '(' ireg ')'  */
                                     { dbug(); write_mem_array(ic++, opcode | (regnum<<16)); ipc(4); }
    break;

  case 37: /* instruction: INS_UNOP disp16  */
                                { dbug(); write_mem_array(ic++, opcode | (30<<16)); ipc(4); }
    break;

  case 38: /* $@8: %empty  */
                                { opcode |= regnum<<21; ipc(4); }
    break;

  case 39: /* instruction: INS_JSR ireg ',' $@8 '(' ireg ')' hint  */
                                { dbug(); write_mem_array(ic++, opcode | (regnum<<16)); }
    break;

  case 40: /* $@9: %empty  */
                                { opcode |= regnum<<21; ipc(4); }
    break;

  case 41: /* instruction: INS_JSR ireg ',' $@9 disp14 '(' ireg ')'  */
                                { dbug(); write_mem_array(ic++, opcode | (regnum<<16)); }
    break;

  case 42: /* $@10: %empty  */
                                { opcode |= regnum<<21; ipc(4); }
    break;

  case 43: /* instruction: INS_BRN ifreg ',' $@10 disp21  */
                                { dbug(); write_mem_array(ic++, opcode); }
    break;

  case 44: /* instruction: INS_CALLPAL disp26  */
                                { dbug(); write_mem_array(ic++, opcode); ipc(4); }
    break;

  case 45: /* $@11: %empty  */
                                { opcode |= regnum<<21; ipc(4); }
    break;

  case 46: /* instruction: INS_RPCC ireg ',' $@11 '(' ireg ')'  */
                                { dbug(); write_mem_array(ic++, opcode | (regnum<<16)); }
    break;

  case 47: /* instruction: INS_RPCC ireg  */
                                { dbug(); write_mem_array(ic++, opcode | (regnum<<21) | (31<<16)); ipc(4); }
    break;

  case 48: /* instruction: INS_M1A ireg  */
                                { dbug(); write_mem_array(ic++, opcode | (regnum<<21)); ipc(4); }
    break;

  case 49: /* instruction: INS_M1B '(' ireg ')'  */
                                { dbug(); write_mem_array(ic++, opcode | (regnum<<16)); ipc(4); }
    break;

  case 50: /* instruction: INS_MIS  */
                                { dbug(); write_mem_array(ic++, opcode); ipc(4); }
    break;

  case 51: /* instruction: INS_1REG ireg  */
                                { dbug(); write_mem_array(ic++, opcode | regnum); ipc(4); }
    break;

  case 52: /* instruction: INS_FPCR freg  */
                                { dbug(); write_mem_array(ic++, opcode | (regnum<<21) | (regnum<<16) | regnum); ipc(4); }
    break;

  case 53: /* instruction: INS_FPCR three_freg  */
                                { dbug();
                                  if ((((opcode >> 21) & 0x1F) != (opcode & 0x1F)) ||
                                      (((opcode >> 16) & 0x1F) != (opcode & 0x1F))) {
                                      errout(E_WARN, "parser", "mt/mf_fpcr instruction specified without the same three registers.\n");
                                  }
                                  write_mem_array(ic++, opcode);
                                  ipc(4);
                                }
    break;

  case 54: /* $@12: %empty  */
                                { opcode |= (regnum<<21) | (regnum<<16); ipc(4); }
    break;

  case 55: /* instruction: INS_IPR4 ireg ',' $@12 ipr  */
                                { dbug(); write_mem_array(ic++, opcode); }
    break;

  case 56: /* $@13: %empty  */
                                { opcode |= regnum<<21; ipc(4); }
    break;

  case 57: /* instruction: INS_HWM4 ireg ',' $@13 disp12 '(' ireg ')'  */
                                { dbug(); write_mem_array(ic++, opcode | (regnum<<16)); }
    break;

  case 58: /* $@14: %empty  */
                                { opcode |= (regnum<<21) | (regnum<<16); ipc(4); }
    break;

  case 59: /* instruction: INS_IPR5 ireg ',' $@14 ipr  */
                                { dbug(); write_mem_array(ic++, opcode); }
    break;

  case 60: /* $@15: %empty  */
                                { opcode |= regnum<<21; ipc(4); }
    break;

  case 61: /* instruction: INS_HWM5 ireg ',' $@15 disp10 '(' ireg ')' mode  */
                                 { dbug(); write_mem_array(ic++, opcode | (regnum<<16)); }
    break;

  case 62: /* instruction: INS_REI6 '(' ireg ')'  */
                                { ipc(4); dbug(); write_mem_array(ic++, opcode | (regnum<<16));}
    break;

  case 63: /* $@16: %empty  */
                                { opcode = (opcode & ~(0x1f << 21)) | regnum<<21; ipc(4); }
    break;

  case 64: /* instruction: INS_REI6 ireg ',' $@16 '(' ireg ')' hint  */
                                { dbug(); write_mem_array(ic++, opcode | (regnum<<16));}
    break;

  case 65: /* $@17: %empty  */
                                { opcode |= regnum<<16; ipc(4); }
    break;

  case 66: /* instruction: INS_MTI6 ireg ',' $@17 ipr scbd6  */
                                { dbug(); write_mem_array(ic++, opcode); }
    break;

  case 67: /* $@18: %empty  */
                                { opcode |= regnum<<21; ipc(4); }
    break;

  case 68: /* instruction: INS_MFI6 ireg ',' $@18 ipr scbd6  */
                                { dbug(); write_mem_array(ic++, opcode); }
    break;

  case 69: /* $@19: %empty  */
                                { opcode |= regnum<<21; ipc(4); }
    break;

  case 70: /* instruction: INS_HWM6 ireg ',' $@19 disp12 '(' ireg ')'  */
                                { dbug(); write_mem_array(ic++, opcode | (regnum<<16)); }
    break;

  case 73: /* ifreg: absexpr  */
                                { regnum = yyvsp[0]->tok.q.v[0];
    				  if (regnum<0 || regnum>31) {
                                    errout(E_WARN,"parser","Register expression value was not 0 >= reg >= 31. Value will be masked.\n");
                                    regnum &= 0x1f;
                                  }
				  free_tok(yyvsp[0]);
        			}
    break;

  case 74: /* $@20: %empty  */
                                { opcode |= regnum<<21; }
    break;

  case 75: /* $@21: %empty  */
                                { opcode |= regnum<<16; }
    break;

  case 76: /* three_freg: freg ',' $@20 freg ',' $@21 freg  */
                                { opcode |= regnum; }
    break;

  case 77: /* $@22: %empty  */
                                { opcode |= (31<<21) | (regnum<<16); }
    break;

  case 78: /* two_freg: freg ',' $@22 freg  */
                                { opcode |= regnum; }
    break;

  case 79: /* $@23: %empty  */
                                { opcode |= (regnum<<16); }
    break;

  case 80: /* intreg: ireg ',' $@23 ireg  */
                                { opcode |= (regnum);     }
    break;

  case 81: /* $@24: %empty  */
                                { opcode |= 0x1000; handle_instr_expr(yyvsp[-1], TIOP); }
    break;

  case 82: /* intreg: '#' expr ',' $@24 ireg  */
                                { opcode |= regnum; }
    break;

  case 83: /* $@25: %empty  */
                                { opcode |= (regnum<<16); }
    break;

  case 84: /* two_ireg: ireg ',' $@25 ireg  */
                                { opcode |= (regnum);     }
    break;

  case 86: /* disp10: expr  */
                                { handle_instr_expr(yyvsp[0], TMHW10); }
    break;

  case 88: /* disp12: expr  */
                                { handle_instr_expr(yyvsp[0], TMHW12); }
    break;

  case 89: /* disp14: expr  */
                                { handle_instr_expr(yyvsp[0], TJDS); }
    break;

  case 91: /* disp16: expr  */
                                { handle_instr_expr(yyvsp[0], TMDS); }
    break;

  case 92: /* disp21: expr  */
                                { handle_instr_expr(yyvsp[0], TBDS); }
    break;

  case 93: /* disp26: expr  */
                                { handle_instr_expr(yyvsp[0], TPDS); }
    break;

  case 97: /* mode: ',' expr  */
                                { handle_instr_expr(yyvsp[0], TMOD); }
    break;

  case 98: /* ipr: absexpr  */
                                { opcode |= (yyvsp[0]->tok.q.v[0] & 0xffff);
        			  if ((yyvsp[0]->tok.q.v[0] & ~0xFFFF) || yyvsp[0]->tok.q.v[1])
                		    errout(E_WARN, "parser", "constant larger than IPR number (16 bits), truncating upper bits.\n");
				  free_tok(yyvsp[0]);
        			}
    break;

  case 100: /* scbd6: ',' absexpr  */
                                { opcode |= (yyvsp[0]->tok.q.v[0] & 0xff);
        			  if ((yyvsp[0]->tok.q.v[0] & ~0xFF) || yyvsp[0]->tok.q.v[1])
                		    errout(E_WARN, "parser", "constant larger than scoreboard field (8 bits), truncating upper bits.\n");
    				  free_tok(yyvsp[0]);
    				}
    break;

  case 102: /* ireg: absexpr  */
                                { regnum = yyvsp[0]->tok.q.v[0]; free_tok(yyvsp[0]);
                                  if (regnum<0 || regnum>31) {
                                      errout(E_WARN,"parse","Register expression value was not 0 >= reg >= 31. Value will be masked.\n");
                                      regnum &= 0x1f;
                                  }
                                }
    break;

  case 104: /* freg: absexpr  */
                                { regnum = yyvsp[0]->tok.q.v[0]; free_tok(yyvsp[0]); 
                                  if (regnum<0 || regnum>31) {
                                      errout(E_WARN,"parse","Register expression value was not 0 >= reg >= 31. Value will be masked.\n");
                                      regnum &= 0x1f;
                                  }
                                }
    break;

  case 105: /* assignment: SYMBOL '=' expr EOL  */
                                { define_symbol(yyvsp[-3], yyvsp[-1]); }
    break;

  case 106: /* assignment: DOT '=' absexpr EOL  */
                                { dot_equal(yyvsp[-1]); qual_absexpr(DOT, 0); }
    break;

  case 107: /* expr: element  */
                                        { yyval = yyvsp[0]; }
    break;

  case 108: /* expr: expr '+' expr  */
                                        { yyval = eval_subexpr(yyvsp[-2], yyvsp[0], yyvsp[-1]); if (yyval == NULL) yyval = chain(yyvsp[-2], yyvsp[0], yyvsp[-1]); }
    break;

  case 109: /* expr: expr '-' expr  */
                                        { yyval = eval_subexpr(yyvsp[-2], yyvsp[0], yyvsp[-1]); if (yyval == NULL) yyval = chain(yyvsp[-2], yyvsp[0], yyvsp[-1]); }
    break;

  case 110: /* expr: expr '*' expr  */
                                        { yyval = eval_subexpr(yyvsp[-2], yyvsp[0], yyvsp[-1]); if (yyval == NULL) yyval = chain(yyvsp[-2], yyvsp[0], yyvsp[-1]); }
    break;

  case 111: /* expr: expr '/' expr  */
                                        { yyval = eval_subexpr(yyvsp[-2], yyvsp[0], yyvsp[-1]); if (yyval == NULL) yyval = chain(yyvsp[-2], yyvsp[0], yyvsp[-1]); }
    break;

  case 112: /* expr: expr '@' expr  */
                                        { yyval = eval_subexpr(yyvsp[-2], yyvsp[0], yyvsp[-1]); if (yyval == NULL) yyval = chain(yyvsp[-2], yyvsp[0], yyvsp[-1]); }
    break;

  case 113: /* expr: expr '&' expr  */
                                        { yyval = eval_subexpr(yyvsp[-2], yyvsp[0], yyvsp[-1]); if (yyval == NULL) yyval = chain(yyvsp[-2], yyvsp[0], yyvsp[-1]); }
    break;

  case 114: /* expr: expr '!' expr  */
                                        { yyval = eval_subexpr(yyvsp[-2], yyvsp[0], yyvsp[-1]); if (yyval == NULL) yyval = chain(yyvsp[-2], yyvsp[0], yyvsp[-1]); }
    break;

  case 115: /* expr: expr '\\' expr  */
                                        { yyval = eval_subexpr(yyvsp[-2], yyvsp[0], yyvsp[-1]); if (yyval == NULL) yyval = chain(yyvsp[-2], yyvsp[0], yyvsp[-1]); }
    break;

  case 116: /* expr: expr LOG_AND expr  */
                                        { yyval = eval_subexpr(yyvsp[-2], yyvsp[0], yyvsp[-1]); if (yyval == NULL) yyval = chain(yyvsp[-2], yyvsp[0], yyvsp[-1]); }
    break;

  case 117: /* expr: expr LOG_OR expr  */
                                        { yyval = eval_subexpr(yyvsp[-2], yyvsp[0], yyvsp[-1]); if (yyval == NULL) yyval = chain(yyvsp[-2], yyvsp[0], yyvsp[-1]); }
    break;

  case 118: /* expr: NOT expr  */
                                        { yyval = eval_subexpr(yyvsp[0], NULL, yyvsp[-1]); if (yyval == NULL) yyval = chain(yyvsp[0], yyvsp[-1], NULL); }
    break;

  case 119: /* expr: LOG_NOT expr  */
                                        { yyval = eval_subexpr(yyvsp[0], NULL, yyvsp[-1]); if (yyval == NULL) yyval = chain(yyvsp[0], yyvsp[-1], NULL); }
    break;

  case 120: /* expr: '-' expr  */
                                        { yyvsp[-1]->type = UMINUS; yyval = eval_subexpr(yyvsp[0], NULL, yyvsp[-1]); if (yyval == NULL) yyval = chain(create_zero_tok(), yyvsp[0], yyvsp[-1]); }
    break;

  case 121: /* expr: '<' expr '>'  */
                                        { yyval = yyvsp[-1]; }
    break;

  case 122: /* element: NUMBER  */
                                        { yyval = yyvsp[0]; }
    break;

  case 123: /* element: SYMBOL  */
                                        { yyval = get_symbol_value(yyvsp[0], is_while); }
    break;

  case 124: /* element: LOCAL_LABEL  */
                                        { yyval = get_symbol_value(yyvsp[0], is_while); }
    break;

  case 125: /* element: DOT  */
                                        { yyval = convert_dot(yyvsp[0]); }
    break;

  case 126: /* absexpr: expr  */
                                        { int reloc_used;
                                          yyval = eval_expr(yyvsp[0], &reloc_used);
                                          if (reloc_used && (yyval != NULL))
                                              save_absexpr(yyvsp[0], yyval);
                                          else
                                              free_tok_list_s(yyvsp[0]);
                                        }
    break;

  case 127: /* directive: INCLUDE STRING EOL  */
                                        { do_include(yyvsp[-1]); do_list(L_MISC, start_line); }
    break;

  case 128: /* directive: INCLUDE_DIR STRING EOL  */
                                        { do_include_dir(yyvsp[-1]); do_list(L_MISC, start_line); }
    break;

  case 129: /* directive: MINCLUDE STRING EOL  */
                                        { do_minclude(yyvsp[-1]); do_list(L_MISC, start_line); }
    break;

  case 130: /* $@26: %empty  */
                                        { temp_pragma_flags = 0; }
    break;

  case 131: /* directive: PRAGMA $@26 pragma_flags EOL  */
                                        { do_list(L_MISC, start_line); global_pragma = temp_pragma_flags; }
    break;

  case 132: /* directive: ARCH arch_flag EOL  */
                                        { do_list(L_MISC, start_line); }
    break;

  case 133: /* $@27: %empty  */
                                        { byte_in_lw = nchar = 0; }
    break;

  case 134: /* directive: ASCII $@27 string_seq EOL  */
                                        { fill_to_lw(); ipc(nchar); do_list(L_BINARY | L_MISC, start_line); }
    break;

  case 135: /* $@28: %empty  */
                                        { byte_in_lw = nchar = 0; }
    break;

  case 136: /* directive: ASCIZ $@28 string_seq EOL  */
                                        { store_expr(create_zero_tok()); fill_to_lw(); ipc(nchar); do_list(L_BINARY | L_MISC, start_line); }
    break;

  case 137: /* $@29: %empty  */
                                        { save_start_line = start_line; }
    break;

  case 138: /* directive: IF if_condition EOL mainbody $@29 ifsubbody_seq ENDC EOL  */
                                        { do_list(L_CONDITIONALS, save_start_line); 
					  if (yyvsp[-4] || yyvsp[-2]) { 
					      push_current_ifile(); 
                                              push_bodies(chain(yyvsp[-4], yyvsp[-2], NULL)); 
                                              (void)next_file(); 
                                          } 
                                        }
    break;

  case 139: /* directive: IIF if_condition ',' BODYTEXT EOL  */
                                        {  do_list(L_CONDITIONALS, start_line); 
                                           if (if_result) { 
                                               yyvsp[-1]->type = IIF; yyvsp[-1]->lineno--;	/* subtract 1 because there wasn't really a \n before the body */
                                               push_current_ifile(); 
                                               push_bodies(yyvsp[-1]); 
                                               (void)next_file(); 
                                           } else
                                               free_tok_s(yyvsp[-1]);
                                        }
    break;

  case 140: /* directive: REPEAT opt_absexpr EOL BODYTEXT ENDR EOL  */
                                        { do_list(L_DEFINITIONS, start_line); push_repeat_body(yyvsp[-2], yyvsp[-4]);}
    break;

  case 141: /* $@30: %empty  */
              {is_while =1;}
    break;

  case 142: /* $@31: %empty  */
                                             {is_while=0;}
    break;

  case 143: /* directive: WHILE $@30 while_condition $@31 EOL BODYTEXT ENDW EOL  */
                                        { do_list(L_DEFINITIONS, start_line); push_while_body(yyvsp[-2], yyvsp[-5]);}
    break;

  case 144: /* directive: BREAK EOL  */
                                        { do_list(L_DEFINITIONS, start_line); }
    break;

  case 145: /* directive: CONTINUE EOL  */
                                        { do_list(L_DEFINITIONS, start_line); }
    break;

  case 146: /* directive: CMDIN absexpr EOL BODYTEXT ENDI EOL  */
                                        { do_list(L_CMDIN, start_line); do_cmdin(yyvsp[-4]->tok.q.v[0], yyvsp[-2]); free_tok(yyvsp[-4]); }
    break;

  case 147: /* directive: DOCMD STRING EOL  */
                                        { char *outnm;
                                          do_list(L_CALLS | L_MISC, start_line); 
                                          outnm = do_cmd(yyvsp[-1]); 
                                          if (outnm!=NULL) {
                                             push_current_ifile();
                                             /* create INPUT_FILE struct for this cmd output file on ifile stack */
                                             /*NOTE: string_type means this is a temp file and delete it once done */
                                             add_input(outnm, F_FILE, 1, NULL, ADD_PUSH, 0, FILE_DELETE); 
                                             (void)next_file();
                                          }
                                        }
    break;

  case 148: /* directive: ALIGN alignbound opt_alignfill EOL  */
                                           { do_align(yyvsp[-2], yyvsp[-1]); do_list(L_BINARY | L_MISC, start_line); }
    break;

  case 149: /* $@32: %empty  */
                                        { mem_type = TADD; num_mem_expr = 0; }
    break;

  case 150: /* directive: ADDR $@32 mem_exprseq EOL  */
                                        { ipc(8*num_mem_expr); do_list(L_BINARY | L_MISC, start_line); }
    break;

  case 151: /* $@33: %empty  */
                                        { mem_type = TD64; num_mem_expr = 0; }
    break;

  case 152: /* directive: QUAD $@33 mem_exprseq EOL  */
                                        { ipc(8*num_mem_expr); do_list(L_BINARY | L_MISC, start_line); }
    break;

  case 153: /* $@34: %empty  */
                                        { mem_type = TD32; num_mem_expr = 0; }
    break;

  case 154: /* directive: LONG $@34 mem_exprseq EOL  */
                                        { ipc(4*num_mem_expr); do_list(L_BINARY | L_MISC, start_line); }
    break;

  case 155: /* directive: WARN opt_absexpr COMMENT EOL  */
                                        { do_genmsg(WARN, yyvsp[-2], yyvsp[-1]); do_list(L_MISC, start_line); }
    break;

  case 156: /* directive: ERROR opt_absexpr COMMENT EOL  */
                                        { do_genmsg(ERROR, yyvsp[-2], yyvsp[-1]); do_list(L_MISC, start_line); }
    break;

  case 157: /* directive: PRINT opt_absexpr COMMENT EOL  */
                                        { do_genmsg(PRINT, yyvsp[-2], yyvsp[-1]); do_list(L_MISC, start_line); }
    break;

  case 158: /* directive: PSECT SYMBOL psect_args EOL  */
                                        { do_psect(yyvsp[-2], yyvsp[-1], PENTRY_TYPE_PSECT); do_list(L_BINARY | L_MISC, start_line); }
    break;

  case 159: /* directive: SAVE_PSECT EOL  */
                                        { do_save_psect(max_blknum++); do_list(L_BINARY | L_MISC, start_line); }
    break;

  case 160: /* directive: SAVE_DSECT EOL  */
                                        { do_save_psect(max_blknum++); do_list(L_BINARY | L_MISC, start_line); }
    break;

  case 161: /* directive: SAVE_PSECT LOCAL_BLOCK EOL  */
                                        { do_save_psect(local_label_blknum); do_list(L_BINARY | L_MISC, start_line); }
    break;

  case 162: /* directive: SAVE_DSECT LOCAL_BLOCK EOL  */
                                        { do_save_psect(local_label_blknum); do_list(L_BINARY | L_MISC, start_line); }
    break;

  case 163: /* directive: RESTORE_PSECT EOL  */
                                        { local_label_blknum = do_rest_psect(PENTRY_TYPE_PSECT); do_list(L_BINARY | L_MISC, start_line); }
    break;

  case 164: /* directive: RESTORE_DSECT EOL  */
                                        { local_label_blknum = do_rest_psect(PENTRY_TYPE_DSECT); do_list(L_BINARY | L_MISC, start_line); }
    break;

  case 165: /* directive: BLKL absexpr EOL  */
                                        { ipc(yyvsp[-1]->tok.q.v[0] << 2); ic = pc >> 2; do_list(L_BINARY | L_MISC | L_NOTALL, start_line); free_tok(yyvsp[-1]); }
    break;

  case 166: /* directive: BLKQ absexpr EOL  */
                                        { ipc(yyvsp[-1]->tok.q.v[0] << 3); ic = pc >> 2; do_list(L_BINARY | L_MISC | L_NOTALL, start_line); free_tok(yyvsp[-1]); }
    break;

  case 167: /* $@35: %empty  */
                                        { mac_arglist = mac_deflist = NULL; var_arg_used = narg = 0; save_start_line = start_line; }
    break;

  case 168: /* directive: MACRO SYMBOL opt_comma $@35 opt_macro_arglist EOL BODYTEXT ENDM EOL  */
                                        { define_macro(yyvsp[-7], narg, var_arg_used, mac_arglist, mac_deflist, yyvsp[-2]); do_list(L_DEFINITIONS, save_start_line); }
    break;

  case 169: /* $@36: %empty  */
                                        { macargs = setup_macro_call_args(yyvsp[0], &macdef, &macargs_size); macro_arg_pos = 0; }
    break;

  case 170: /* directive: MACRO_CALL $@36 opt_macro_call_arglist EOL  */
                                        { expand_macro(macdef, macargs); /* do_list handled inside */}
    break;

  case 171: /* directive: NEXT_ARG EOL  */
                                        { do_list(L_DEFINITIONS, start_line); }
    break;

  case 172: /* $@37: %empty  */
                                        { temp_listing_flags = 0; }
    break;

  case 173: /* directive: NOLIST $@37 list_flags EOL  */
                                        { listing_flags &= ~temp_listing_flags; do_list(L_MISC, start_line); }
    break;

  case 174: /* directive: NOLIST EOL  */
                                        { do_list(L_MISC, start_line); --listing_enable; }
    break;

  case 175: /* $@38: %empty  */
                                        { temp_listing_flags = 0; }
    break;

  case 176: /* directive: LIST $@38 list_flags EOL  */
                                        { listing_flags |= temp_listing_flags; do_list(L_MISC, start_line); }
    break;

  case 177: /* directive: LIST EOL  */
                                        { ++listing_enable; do_list(L_MISC, start_line); }
    break;

  case 178: /* directive: NODEBUG EOL  */
                                        { do_list(L_MISC, start_line); }
    break;

  case 179: /* directive: K_DEBUG EOL  */
                                        { do_list(L_MISC, start_line); }
    break;

  case 180: /* directive: IDENT EOL  */
                                        { do_list(L_MISC, start_line); }
    break;

  case 181: /* directive: TITLE EOL  */
                                        { do_list(L_MISC, start_line); }
    break;

  case 182: /* directive: DSECT SYMBOL psect_args EOL  */
                                        { do_psect(yyvsp[-2], yyvsp[-1], PENTRY_TYPE_DSECT); do_list(L_BINARY | L_MISC, start_line); }
    break;

  case 183: /* directive: HEADER EOL  */
                                        { do_save_psect(local_label_blknum); do_header(); do_list(L_BINARY | L_MISC, start_line); }
    break;

  case 184: /* directive: ENDH EOL  */
                                        { do_rest_psect(PENTRY_TYPE_DSECT); do_list(L_BINARY | L_MISC, start_line); }
    break;

  case 185: /* directive: LINK_ADDRESS absexpr EOL  */
                                        { do_link_address(yyvsp[-1]); do_list(L_BINARY | L_MISC, start_line); }
    break;

  case 186: /* directive: LINK_OFFSET absexpr EOL  */
                                        { do_link_offset(yyvsp[-1]); do_list(L_BINARY | L_MISC, start_line); }
    break;

  case 187: /* directive: LINK_REGION absexpr EOL  */
                                        { do_link_region(yyvsp[-1]); do_list(L_BINARY | L_MISC, start_line); }
    break;

  case 188: /* directive: LINK_SIZE absexpr EOL  */
                                        { do_link_size(yyvsp[-1]); do_list(L_BINARY | L_MISC, start_line); }
    break;

  case 189: /* directive: LINK_AFTER SYMBOL EOL  */
                                        { do_link_after(yyvsp[-1]); do_list(L_BINARY | L_MISC, start_line); }
    break;

  case 190: /* directive: LINK_FIRST EOL  */
                                        { do_link_first(); do_list(L_BINARY | L_MISC, start_line); }
    break;

  case 191: /* directive: LINK_LAST EOL  */
                                        { do_link_last(); do_list(L_BINARY | L_MISC, start_line); }
    break;

  case 192: /* $@39: %empty  */
                                        { temp_link_flags = 0; }
    break;

  case 193: /* directive: LINK_ATTR $@39 link_flags EOL  */
                                        { do_link_attr(temp_link_flags); do_list(L_BINARY | L_MISC, start_line); }
    break;

  case 194: /* arch_flag: A_EV4  */
                                       { chip = EV4; errout(E_INFO| E_NO_LNUM, "hal", "Hardware mode changed to EV4\n"); }
    break;

  case 195: /* arch_flag: A_EV5  */
                                       { chip = EV5; errout(E_INFO| E_NO_LNUM, "hal", "Hardware mode changed to EV5\n"); }
    break;

  case 196: /* arch_flag: A_EV6  */
                                       { chip = EV6; errout(E_INFO| E_NO_LNUM, "hal", "Hardware mode changed to EV6\n"); }
    break;

  case 199: /* pragma_flag: MAC_NONE  */
                                       { temp_pragma_flags &= ~3; temp_pragma_flags |= PRAGMA_MAC_NONE;}
    break;

  case 200: /* pragma_flag: MAC_ONCE  */
                                       { temp_pragma_flags &= ~3; temp_pragma_flags |= PRAGMA_MAC_ONCE;}
    break;

  case 201: /* pragma_flag: MAC_UNLIMITED  */
                                       { temp_pragma_flags &= ~3; temp_pragma_flags |= PRAGMA_MAC_UNLIMITED;}
    break;

  case 202: /* pragma_flag: LEXER_DEBUG_ON  */
                                       { yy_flex_debug = 1; }
    break;

  case 203: /* pragma_flag: LEXER_DEBUG_OFF  */
                                       { yy_flex_debug = 0; }
    break;

  case 204: /* pragma_flag: PARSER_DEBUG_ON  */
                                       { yydebug = 1; }
    break;

  case 205: /* pragma_flag: PARSER_DEBUG_OFF  */
                                       { yydebug = 0; }
    break;

  case 208: /* link_flag: OVERLAP_OK  */
                                        { temp_link_flags |= D_OVERLAP_OK; }
    break;

  case 209: /* link_flag: DIRECTLY_AFTER  */
                                        { temp_link_flags |= D_DIRECTLY_AFTER; }
    break;

  case 212: /* list_flag: MEB  */
                                        { temp_listing_flags |= L_BINARY; }
    break;

  case 213: /* list_flag: MC  */
                                        { temp_listing_flags |= L_CALLS; }
    break;

  case 214: /* list_flag: CND  */
                                        { temp_listing_flags |= L_CONDITIONALS; }
    break;

  case 215: /* list_flag: MD  */
                                        { temp_listing_flags |= L_DEFINITIONS; }
    break;

  case 216: /* list_flag: ME  */
                                        { temp_listing_flags |= L_EXPANSIONS; }
    break;

  case 217: /* list_flag: CMDIN  */
                                        { temp_listing_flags |= L_CMDIN; }
    break;

  case 218: /* list_flag: ASSIGN  */
                                        { temp_listing_flags |= L_ASSIGN; }
    break;

  case 221: /* string_ele: STRING  */
                                        { store_string(yyvsp[0]); }
    break;

  case 222: /* string_ele: '<' absexpr '>'  */
                                        { store_expr(yyvsp[-1]); }
    break;

  case 223: /* if_condition: EQ absexpr  */
                                        { if_result = do_if_condition(EQ, yyvsp[0], NULL, 1); qual_absexpr(EQ, if_result); }
    break;

  case 224: /* if_condition: NE absexpr  */
                                        { if_result = do_if_condition(NE, yyvsp[0], NULL, 1); qual_absexpr(NE, if_result); }
    break;

  case 225: /* if_condition: GT absexpr  */
                                        { if_result = do_if_condition(GT, yyvsp[0], NULL, 1); qual_absexpr(GT, if_result); }
    break;

  case 226: /* if_condition: LT absexpr  */
                                        { if_result = do_if_condition(LT, yyvsp[0], NULL, 1); qual_absexpr(LT, if_result); }
    break;

  case 227: /* if_condition: GE absexpr  */
                                        { if_result = do_if_condition(GE, yyvsp[0], NULL, 1); qual_absexpr(GE, if_result); }
    break;

  case 228: /* if_condition: LE absexpr  */
                                        { if_result = do_if_condition(LE, yyvsp[0], NULL, 1); qual_absexpr(LE, if_result); }
    break;

  case 229: /* if_condition: DF SYMBOL  */
                                        { if_result = do_if_condition(DF, yyvsp[0], NULL, 1); }
    break;

  case 230: /* if_condition: NDF SYMBOL  */
                                        { if_result = do_if_condition(NDF,yyvsp[0], NULL, 1); }
    break;

  case 231: /* if_condition: B ',' opt_arg_value  */
                                        { if_result = do_if_condition(B , yyvsp[0], NULL, 1); }
    break;

  case 232: /* if_condition: NB ',' opt_arg_value  */
                                        { if_result = do_if_condition(NB, yyvsp[0], NULL, 1); }
    break;

  case 233: /* if_condition: IDN ',' opt_arg_value ',' opt_arg_value  */
                                                { if_result = do_if_condition(IDN,yyvsp[-2], yyvsp[0], 1); }
    break;

  case 234: /* if_condition: DIF ',' opt_arg_value ',' opt_arg_value  */
                                                { if_result = do_if_condition(DIF,yyvsp[-2], yyvsp[0], 1); }
    break;

  case 235: /* while_condition: EQ expr  */
                                        { yyval = chain(make_token(NULL, EQ), yyvsp[0], NULL); }
    break;

  case 236: /* while_condition: NE expr  */
                                        { yyval = chain(make_token(NULL, NE), yyvsp[0], NULL); }
    break;

  case 237: /* while_condition: GT expr  */
                                        { yyval = chain(make_token(NULL, GT), yyvsp[0], NULL); }
    break;

  case 238: /* while_condition: LT expr  */
                                        { yyval = chain(make_token(NULL, LT), yyvsp[0], NULL); }
    break;

  case 239: /* while_condition: GE expr  */
                                        { yyval = chain(make_token(NULL, GE), yyvsp[0], NULL); }
    break;

  case 240: /* while_condition: LE expr  */
                                        { yyval = chain(make_token(NULL, LE), yyvsp[0], NULL); }
    break;

  case 241: /* while_condition: DF SYMBOL  */
                                        { yyval = chain(make_token(NULL, DF), yyvsp[0], NULL); }
    break;

  case 242: /* while_condition: NDF SYMBOL  */
                                        { yyval = chain(make_token(NULL, NDF), yyvsp[0], NULL); }
    break;

  case 243: /* while_condition: B ',' opt_arg_value  */
                                        { yyval = chain(make_token(NULL, B), yyvsp[0], NULL); }
    break;

  case 244: /* while_condition: NB ',' opt_arg_value  */
                                        { yyval = chain(make_token(NULL, NB), yyvsp[0], NULL); }
    break;

  case 245: /* while_condition: IDN ',' opt_arg_value ',' opt_arg_value  */
                                                { yyval = chain(make_token(NULL, IDN), yyvsp[-2], yyvsp[0]); }
    break;

  case 246: /* while_condition: DIF ',' opt_arg_value ',' opt_arg_value  */
                                                { yyval = chain(make_token(NULL, DIF), yyvsp[-2], yyvsp[0]); }
    break;

  case 249: /* opt_arg_value: %empty  */
                                        { yyval = NULL; }
    break;

  case 250: /* opt_arg_value: arg_value  */
                                        { yyval = yyvsp[0]; }
    break;

  case 251: /* mainbody: BODYTEXT  */
                                        { if (if_result) { yyvsp[0]->type = IF; yyval = yyvsp[0]; } else { yyval = NULL; free_tok_s(yyvsp[0]); } }
    break;

  case 252: /* ifsubbody_seq: %empty  */
                                        { yyval = NULL; }
    break;

  case 253: /* ifsubbody_seq: ifsubbody  */
                                        { yyval = yyvsp[0]; }
    break;

  case 254: /* ifsubbody_seq: ifsubbody_seq ifsubbody  */
                                        { yyval = chain(yyvsp[-1], yyvsp[0], NULL); }
    break;

  case 255: /* ifsubbody: IFF BODYTEXT  */
                                        { if (!if_result) { yyvsp[0]->type = IFF; yyval = yyvsp[0]; } else { yyval = NULL; free_tok_s(yyvsp[0]); } }
    break;

  case 256: /* ifsubbody: IFT BODYTEXT  */
                                        { if (if_result) { yyvsp[0]->type = IFT; yyval = yyvsp[0]; } else { yyval = NULL; free_tok_s(yyvsp[0]); } }
    break;

  case 257: /* ifsubbody: IFTF BODYTEXT  */
                                        { yyvsp[0]->type = IFTF; yyval = yyvsp[0]; }
    break;

  case 258: /* alignbound: absexpr  */
                                { yyval = yyvsp[0]; }
    break;

  case 259: /* alignbound: K_BYTE  */
                                { yyval = yyvsp[0]; }
    break;

  case 260: /* alignbound: K_WORD  */
                                { yyval = yyvsp[0]; }
    break;

  case 261: /* alignbound: K_LONG  */
                                { yyval = yyvsp[0]; }
    break;

  case 262: /* alignbound: K_QUAD  */
                                { yyval = yyvsp[0]; }
    break;

  case 263: /* alignbound: K_PAGE  */
                                { yyval = yyvsp[0]; }
    break;

  case 264: /* opt_alignfill: %empty  */
                                { yyval = NULL; }
    break;

  case 265: /* opt_alignfill: ',' absexpr  */
                                { yyval = yyvsp[0]; }
    break;

  case 266: /* mem_exprseq: expr  */
                                        { ++num_mem_expr; handle_memdir_expr(yyvsp[0], mem_type); }
    break;

  case 267: /* mem_exprseq: mem_exprseq ',' expr  */
                                        { ++num_mem_expr; handle_memdir_expr(yyvsp[0], mem_type); }
    break;

  case 268: /* opt_absexpr: %empty  */
                                { yyval = NULL; }
    break;

  case 269: /* opt_absexpr: absexpr  */
                                { yyval = yyvsp[0]; }
    break;

  case 270: /* psect_args: %empty  */
                                { yyval = NULL; }
    break;

  case 271: /* psect_args: ',' alignbound  */
                                { yyval = yyvsp[0]; }
    break;

  case 276: /* macro_arg: SYMBOL arg_default  */
                                        { yyvsp[-1]->type = MACRO_ARG_NORMAL;
                                          mac_arglist = chain(mac_arglist, yyvsp[-1], NULL);
                                          mac_deflist = chain(mac_deflist, yyvsp[0], NULL); ++narg;
                                        }
    break;

  case 277: /* macro_arg: '~' SYMBOL arg_default  */
                                        { yyvsp[-1]->type = MACRO_ARG_KWONLY;
                                          mac_arglist = chain(mac_arglist, yyvsp[-1], NULL);
                                          mac_deflist = chain(mac_deflist, yyvsp[0], NULL); ++narg;
                                        }
    break;

  case 278: /* macro_arg: DOTDOTDOT SYMBOL arg_default  */
                                        { yyvsp[-1]->type = MACRO_ARG_NORMAL;
                                          mac_arglist = chain(mac_arglist, yyvsp[-1], NULL); 
                                          mac_deflist = chain(mac_deflist, yyvsp[0], NULL); var_arg_used=++narg;
                                        }
    break;

  case 279: /* macro_arg: DOTDOTDOT arg_default  */
                                        { yyvsp[-1]->type = MACRO_ARG_NORMAL;
                                          mac_arglist = chain(mac_arglist, yyvsp[-1], NULL); 
                                          mac_deflist = chain(mac_deflist, yyvsp[0], NULL); var_arg_used=++narg;
                                        }
    break;

  case 280: /* macro_arg: '?' SYMBOL  */
                                        { yyvsp[0]->type = MACRO_ARG_NORMAL;
                                          mac_arglist = chain(mac_arglist, yyvsp[0], NULL); 
                                          mac_deflist = chain(mac_deflist, yyvsp[-1], NULL); ++narg;
                                        }
    break;

  case 281: /* arg_default: %empty  */
                                        { yyval = make_token("", ARG_VALUE); }
    break;

  case 282: /* arg_default: '=' ARG_VALUE  */
                                        { yyval = yyvsp[0]; }
    break;

  case 283: /* arg_default: '=' SYMBOL  */
                                        { yyvsp[0]->type = ARG_VALUE; yyval = yyvsp[0]; }
    break;

  case 286: /* arg_value: SYMBOL  */
                                        { yyval = yyvsp[0]; }
    break;

  case 287: /* arg_value: ARG_VALUE  */
                                        { yyval = yyvsp[0]; }
    break;

  case 288: /* macro_call_arg: %empty  */
                                        { ++macro_arg_pos; }
    break;

  case 289: /* macro_call_arg: '\\' SYMBOL  */
                                        { int reloc_used;
					  TOKEN_LIST *tmp1;
                                          TOKEN_LIST *tmp = eval_expr(tmp1 = get_symbol_value(copy_expr(yyvsp[0]),0/*is_while*/), &reloc_used);
                                          free_tok_s(tmp1);
                                          free_tok(yyvsp[-1]);
                                          if (reloc_used && (tmp != NULL))
                                             save_absexpr(yyvsp[0], tmp);
                                          else
                                             free_tok_s(yyvsp[0]);
                                          if (!assign_macro_arg(macdef, &macargs, &macargs_size, NULL, convert_num2arg(tmp), &macro_arg_pos)) YYERROR;
                                        }
    break;

  case 290: /* macro_call_arg: arg_value  */
                                        { if (!assign_macro_arg(macdef, &macargs, &macargs_size, NULL, yyvsp[0], &macro_arg_pos)) YYERROR;}
    break;

  case 291: /* macro_call_arg: SYMBOL '=' '\\' SYMBOL  */
                                        { int reloc_used;
					  TOKEN_LIST *tmp1;
                                          TOKEN_LIST *tmp = eval_expr(tmp1 = get_symbol_value(copy_expr(yyvsp[0]),0/*is_while*/), &reloc_used);
                                          free_tok_s(tmp1);
                                          free_tok(yyvsp[-1]);
                                          if (reloc_used && (tmp != NULL))
                                             save_absexpr(yyvsp[0], tmp);
                                          else
                                             free_tok_s(yyvsp[0]);
                                          if (!assign_macro_arg(macdef, &macargs, &macargs_size, yyvsp[-3], convert_num2arg(tmp), NULL)) YYERROR;
                                        }
    break;

  case 292: /* macro_call_arg: SYMBOL '=' opt_arg_value  */
                                        { if (!assign_macro_arg(macdef, &macargs, &macargs_size, yyvsp[-2], yyvsp[0], NULL)) YYERROR; }
    break;



      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}



#ifdef __STDC__
yyerror(char *s)
#else
yyerror(s)
char *s;
#endif
{
  errout(E_ERR, "parser", "%s\n", s);

  return;
}

static dbug()
{
    dbgout("opcode=%08X\n", opcode); 
}

/*
 * Function to convert a DOT token into a temporary relocateable label.
 * This allows the psect relocation algorithm to correctly relocate the
 * expression dot is part of.
 */
static YYSTYPE convert_dot(TOKEN_LIST* dottok)
{
    char* tstr = create_temporary_label(hpc, pc);
    
    dottok->type = SYMBOL;
    dottok->tok.s = tstr;

    return dottok;
}

/*
 * Function to convert a NUMBER token into a ARG_VALUE token.
 * This allows the macro arg to use '\SYMBOL' format.
 */
static YYSTYPE convert_num2arg(TOKEN_LIST* numtok)
{
    char* tstr = ALLOC_STR(20);  

    if (numtok->tok.q.v[1]!=0) /* return in hex */
        sprintf(tstr,"^x%08x%08x", numtok->tok.q.v[1], numtok->tok.q.v[0]);
    else /* return as decimal */
        sprintf(tstr,"%u", numtok->tok.q.v[0]);
        
    numtok->type = ARG_VALUE;
    numtok->tok.s = tstr;

    return numtok;
}

#ifdef __STDC__
static YYSTYPE chain(YYSTYPE t1, YYSTYPE t2, YYSTYPE t3)
#else
static YYSTYPE chain(t1, t2, t3)
YYSTYPE t1;
YYSTYPE t2;
YYSTYPE t3;
#endif
{
  YYSTYPE ttmp;

  /*
   * if t1 non null, and either t2 or t3 is non null, chain
   * the first non null of t2 or t3 to the end of t1
   */
  if (t1 && (t2 || t3)) {
    for (ttmp = t1; ttmp->next; ttmp = ttmp->next)
      ;
    ttmp->next = t2 ? t2 : t3;
  }

  /*
   * if t2 non null, and t3 is non null, chain
   * the first non null of t3 to the end of t2
   */
  if (t2 && t3) {
    for (ttmp = t2; ttmp->next; ttmp = ttmp->next)
      ;
    ttmp->next = t3;
  }

  /*
   * return first non null pointer
   */
  return t1 ? t1 : (t2 ? t2 : t3);
}

YYSTYPE create_zero_tok()
{
  YYSTYPE temp = alloc_tok();

  temp->type = NUMBER;

  return temp;
}

static char* create_temporary_symbol(TOKEN_LIST* expr)
{
    char* tstr = ALLOC_STR(4+10);	/* "%tmp__________" */

    sprintf(tstr, "%%tmp%010d", temporary_num++);

    symdef(tstr, 0, 0, expr, 0);

    return strcpy(ALLOC_STR(strlen(tstr)), tstr);
}

static char* create_temporary_label(unsigned int hi, unsigned int lo)
{
    char* tstr = ALLOC_STR(4+10);	/* "%tmp__________" */

    sprintf(tstr, "%%tmp%010d", temporary_num++);

    symdef(tstr, hi, lo, NULL, 1);

    return strcpy(ALLOC_STR(strlen(tstr)), tstr);
}

static void handle_instr_expr(TOKEN_LIST* expr, int type)
{
    char* tname;

    if ((expr->type == NUMBER) && (expr->next == NULL) && (type != TBDS)) {
        
        /*
         * expression resolved to a constant number, just insert it
         * straight into memory.
         */
        switch (type) {
        case TPDS:	/* CALL_PAL: Do not clean the m[] (HA)*/
            opcode |= (expr->tok.q.v[0] & 0x3FFFFFF);

            if (!sign_ext_ok(expr, 25))
                errout(E_WARN, "handle_instr_expr", "constant larger than callpal displacement (26 bits), truncating upper bits.\n");
            break;
        case TMDS:	/* Memory: Do not clean the m[] (HA)*/
            opcode |= (expr->tok.q.v[0] & 0xFFFF);

            if (!sign_ext_ok(expr, 15))
                errout(E_WARN, "handle_instr_expr", "constant larger than memory displacement (16 bits), truncating upper bits.\n");
            break;
        case TJDS:	/* Memory: Do not clean the m[] (HA)*/
            opcode |= (expr->tok.q.v[0] & 0x3FFF);

            if (!sign_ext_ok(expr, 13))
                errout(E_WARN, "handle_instr_expr", "constant larger than JSR hint field (14 bits), truncating upper bits.\n");
            break;
        case TMHW10:	/* HW_LD/ST: Do not clean the m[] (HA)*/
            opcode |= (expr->tok.q.v[0] & 0x3FF);

            if (!sign_ext_ok(expr, 9))
                errout(E_WARN, "handle_instr_expr", "constant larger than EV5 HW memory displacement (10 bits), truncating upper bits.\n");
            break;
        case TMHW12:	/* HW_LD/ST: Do not clean the m[] (HA)*/
            opcode |= (expr->tok.q.v[0] & 0xFFF);

            if (!sign_ext_ok(expr, 11))
                errout(E_WARN, "handle_instr_expr", "constant larger than EV4/6 HW memory displacement (12 bits), truncating upper bits.\n");
            break;
        case TMOD:	/* HW_LD/ST: Do not clean the m[] (HA)*/
            opcode |= (expr->tok.q.v[0] & 0x3F) << 10;

            if (!sign_ext_ok(expr, 5))
                errout(E_WARN, "handle_instr_expr", "constant larger than EV5 HW mode (6 bits), truncating upper bits.\n");
            break;
        case TIOP:	
            opcode &= 0xFFE01FFF; /*clean first */
            opcode |= ((expr->tok.q.v[0] & 0xFF) << 13);

            if (!sign_ext_ok(expr, 7))
                errout(E_WARN, "handle_instr_expr", "constant larger than integer immediate (8 bits), truncating upper bits.\n");
            break;
        }

        free_tok(expr);
        
        return;
    }

    if ((expr->type == SYMBOL) && (expr->next == NULL)) {
        tname = expr->tok.s;
        free_tok(expr);
    } else {
        /*
         * create a temporary variable for this expression.
         */
        tname = create_temporary_symbol(expr);
    }
    
    /*
     * mark the variable as being used here.
     */
    symuse(tname, type);

    return;
}

static void handle_memdir_expr(TOKEN_LIST* expr, int type)
{
    char* tname;

    if ((expr->type == NUMBER) && (expr->next == NULL)) {
        /*
         * expression resolved to a constant number, just insert it
         * straight into memory.
         */
        write_mem_array(ic++, expr->tok.q.v[0]);

        if (type == TD32) {

            if (expr->tok.q.v[1] != 0)
                errout(E_WARN, "handle_memdir_expr", "constant larger than memory type, upper bits truncated.\n");

            free_tok(expr);
            return;
        }

        write_mem_array(ic++, expr->tok.q.v[1]);

        free_tok(expr);
        
        return;
    }
    
    if ((expr->type == SYMBOL) && (expr->next == NULL)) {
        tname = expr->tok.s;
        free_tok(expr);
    } else {
        /*
         * create a temporary variable for this expression.
         */
        tname = create_temporary_symbol(expr);
    }
    
    /*
     * mark the variable as being used here.
     */
    symuse(tname, type);

    write_mem_array(ic++, 0xADD);

    if (type == TD32)
        return;

    write_mem_array(ic++, 0xADD);

    return;
}

static void define_symbol(TOKEN_LIST* name, TOKEN_LIST* expr)
{
    if ((expr->type == NUMBER) && (expr->next == NULL)) {
        /*
         * The expression is a simple constant, define the symbol with
         * the correct value, and valid.
         */
        symdef(name->tok.s, expr->tok.q.v[1], expr->tok.q.v[0], NULL, 0);

        free_tok(expr);
    } else {
        /*
         * The expression is not a simple constant, define the symbol with
         * the expression that makes it up.
         */
        symdef(name->tok.s, 0, 0, expr, 0);
    }

    free_tok(name);
    
    return;
}

static void dot_equal(TOKEN_LIST* expr)
{
    /*
     * will only get here if the expression evaluated to
     * an absolute expression.
     */
    assert(expr->next == NULL);

    if (expr->tok.q.v[0] & 3) {
        errout(E_WARN, "dot_equal", "attempt to unalign dot, aligning to next lw.\n");
        expr->tok.q.v[0] += 4 - (expr->tok.q.v[0] & 3);
    }
    
    spc(expr->tok.q.v[1], expr->tok.q.v[0]);

    free_tok(expr);
    return;
}

static void define_label(TOKEN_LIST* name, unsigned int hi, unsigned int lo)
{
    symdef(name->tok.s, hi, lo, NULL, 1);

    free_tok(name);
    
    return;
}

void push_current_ifile()
{
    /* push cur_ifile back on ifile stack */
    cur_ifile->yychar = yychar;
    cur_ifile->yylval = yylval;
    cur_ifile->next = ifile;
    ifile = cur_ifile;

    return;
}
      
/*
 * recursively push the sequence of BODYTEXT tokens onto the input stack,
 * in reverse order.  this will cause the sequence to executed in the order
 * there are in the sequence.
 *
 * Note: assumes that the current input file has already been pushed.
 * otherwise, the next body won't be executed until after the input file hits
 * EOF.
 */
static void push_bodies(TOKEN_LIST* bodyseq)
{
    if (bodyseq == NULL)
        return;

    /*
     * push next body first.
     */
    push_bodies(bodyseq->next);

    /*
     * push this body
     */
    add_input(bodyseq->tok.s, F_STRING, 1, NULL, ADD_PUSH, bodyseq->lineno, bodyseq->type);

    free_tok(bodyseq);

    return;
}

/*
 * Push body onto input file list with a repeat factor taken from
 * absexpr.  It is assumed that absexpr is an absolute expression
 * and therefore is only one token.
 */
static void push_repeat_body(TOKEN_LIST* body, TOKEN_LIST* absexpr)
{
    /*
     * absolute expressions should consist of only one token.
     */
    assert(!absexpr || absexpr->next == NULL);


    /*
     * push this body -- only if absolute expression != 0.
     */
    if (!absexpr || ((signed int)(absexpr->tok.q.v[0]) > 0)) {
      push_current_ifile(); 
      add_input(body->tok.s, F_STRING, (absexpr ? absexpr->tok.q.v[0] : -1), NULL, ADD_PUSH, body->lineno, REPEAT);
      if (ifile->preprocess_me)
        add_input(body->tok.s, F_MACRO_PREP, 1, NULL, ADD_PUSH, body->lineno, REPEAT); /* 2nd one needed so we preprocess it first */
      (void)next_file();
      if (cur_ifile->preprocess_me)
        start_up_preprocessor();
    }

    free_tok(body);
    free_tok(absexpr);

    return;
}

/*
 * Push body onto input file list with a while condition factor taken from
 * expr.  
 */
static void push_while_body(TOKEN_LIST* body, TOKEN_LIST* expr)
{
  int if_result = eval_while_cond(expr);
  
  /*
   * push this body -- only if absolute expression != 0.
   */
  if (if_result) {
    push_current_ifile();
    add_input(body->tok.s, F_STRING, 1, expr, ADD_PUSH, body->lineno, WHILE);
    if (ifile->preprocess_me)
      add_input(body->tok.s, F_MACRO_PREP, 1, NULL, ADD_PUSH, body->lineno, WHILE); /* 2nd one needed so we preprocess it first */
    (void)next_file();
    if (cur_ifile->preprocess_me)
      start_up_preprocessor();
  }

  free_tok(body);

  return;
}

static void store_string(TOKEN_LIST* strtok)
{
    char* tstr = strtok->tok.s;

    while (*tstr) {
        B32 temp = get_mem_array(ic);
        
        /* clear the byte to store to */
        temp = temp & ~(0xFF << (8*byte_in_lw));

        /* store the byte */
        write_mem_array(ic, temp | (*tstr << (8*byte_in_lw)));

        /* skip to next char */
        ++tstr;

        /* count the characters stored. */
        ++nchar;

        /* point to destination for next byte to store */
        ++byte_in_lw;
        if (byte_in_lw > 3) {
            byte_in_lw = 0;
            ic++;
        }
    }
 
    free_tok_s(strtok);

    return;
}

static void store_expr(TOKEN_LIST* expr)
{
    int temp = get_mem_array(ic);
    
    /* clear the byte to store to */
    temp = temp & ~(0xFF << (8*byte_in_lw));

    /* store the byte */
    write_mem_array(ic, temp | ((expr->tok.q.v[0] & 0xFF) << (8*byte_in_lw)));

    /* count the characters stored. */
    ++nchar;

    /* point to destination for next byte to store */
    ++byte_in_lw;
    if (byte_in_lw > 3) {
        byte_in_lw = 0;
        ic++;
    }

    free_tok(expr);

    return;
}

static void fill_to_lw()
{
    /*
     * if we aren't pointing to the first byte in the lw,
     * zero fill the longword.
     */
    if (byte_in_lw != 0) {
        unsigned int mask = 0xFFFFFFFF;
        
        mask = mask << (8*byte_in_lw);
        /*
         * No need to use write_mem_array because we can only get here if
         * the psect is already large enough for the remainder of this longword.
         */
        m[ic++] &= ~mask;

        nchar += 4 - byte_in_lw;
    }

    return;
}

void save_absexpr(TOKEN_LIST* expr, TOKEN_LIST* result)
{
    ABSEXPR* tabs;
    
    if (warn_reloc_absexpr) {
        errout(E_WARN, "parser", "absolute expression was actually relocatable.\n");
    }
    
    tabs = ALLOC(ABSEXPR);

    tabs->qual = 0;
    tabs->expr = expr;
    tabs->result = copy_expr(result);
    tabs->lloc = save_location();
    tabs->next = absexpr_head;

    absexpr_head = tabs;

    saved_absexpr = 1;
    
    return;
}

void qual_absexpr(int qual, int result)
{
    if (!saved_absexpr)
        return;
    
    absexpr_head->qual = qual;
    absexpr_head->qual_result = result;

    return;
}
