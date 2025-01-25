#ifndef __HAL_H__
#define __HAL_H__
/*
	Copyright (c) 1994
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
#include <stdio.h> /* need L_tmpnam constant */
#include <errno.h>

#ifndef UNIX
#  if defined(__osf__) || defined(ultrix)
#    define UNIX 1
#  endif
#endif

#ifdef __osf__
/* This uses malloc, calloc, and free to allocate memory on demand
 * instead of allocating large chunks and doling it out. */
#define USE_MALLOC_FREE

/* This allocates token in blocks and dole them out without ever freeing
 * them.  The default use calloc and FREE each time a token is requested or
 * freed. */
/*#define BLOCK_TOKEN_ALLOC*/
#endif

#ifdef ultrix
/*#define USE_MALLOC_FREE*/
#define BLOCK_TOKEN_ALLOC
#endif

#ifdef VMS
#define USE_MALLOC_FREE
/*#define BLOCK_TOKEN_ALLOC*/
#endif

#ifdef __STDC__
#define PROTO(text)	text
#else
#define PROTO(text)	()
#endif

typedef int int32;
typedef unsigned int uint32;

#ifdef __alpha
typedef unsigned __int64        MTV_uint64;
#endif

#ifdef __osf__
#include <alloca.h>
#else
#if defined(VMS)
#ifndef USE_MALLOC_FREE
#define alloca		my_alloc
#else
#define alloca		malloc
#endif
#endif
#endif

#ifndef EXIT_SUCCESS
#if defined(VMS) || defined(__MSDOS__)
#define EXIT_SUCCESS	1
#else
#define EXIT_SUCCESS	0
#endif
#endif

#ifndef EXIT_FAILURE
#if defined(VMS) || defined(__MSDOS__)
#define EXIT_FAILURE	0
#else
#define EXIT_FAILURE	1
#endif
#endif

#ifndef USE_MALLOC_FREE
#define ALLOC(t)          ((t *)my_alloc(sizeof(t)))
#define ALLOCn(b)         ((char *)my_alloc(b))
#define ALLOC0(n,t)       ((n>0)?(t *)my_calloc(n, sizeof(t)):NULL)
#define ALLOC_STR(l)      ((char *)my_alloc((l)+1)) /* allocate one extra char for NUL-termination */
#define FREE(p)            ;
#define FREE_FUNCT         NULL /* this should be the free function name */
#else
#define ALLOC(t)          ((t *)malloc(sizeof(t)))
#define ALLOCn(b)         ((char *)malloc(b))
#define ALLOC0(n,t)       ((n>0)?(t *)calloc(n, sizeof(t)):NULL)
#define ALLOC_STR(l)      ((char *)malloc((l)+1)) /* allocate one extra char for NUL-termination */
#define FREE(p)            free(p);
#define FREE_FUNCT         free /* this should be the free function name */
#endif

#define SECT_FORWARD 1
#define SECT_BACKWARD 0

#include "avl.h"
#include "dll.h"
#include "lexyy.h"

typedef struct quad_ QUAD_T;
struct quad_ {
  uint32 v[2];
};

/*
 * Need forward declaration to allow use in token.
 */
typedef struct refnode RNODE;

union token_ {
    QUAD_T q;
    char *s;
    RNODE* ref;		/* used to chain together symbol references */
};

typedef struct token_list_ TOKEN_LIST;
#define YYSTYPE	TOKEN_LIST *
struct token_list_ {
  int type;
  int lineno;
  union token_ tok;
  TOKEN_LIST *next;
};

typedef struct psect_list_ PSECT_LIST;
struct psect_list_ {
  TOKEN_LIST *psect;
  int blknum;
  PSECT_LIST *next;
};

#define PATH_MAX        1023

#define ADD_PUSH	0
#define ADD_APPEND	1

#define PREP_BUF_SIZE 512
#define MACRO_BUF_SIZE 4096

#define F_FILE		0
#define F_STRING	1
#define F_MACRO		2
#define F_MACRO_PREP	3
#define FILE_DELETE     1
#define PREP_NO       0
#define PREP_IT       1
#define PREP_DONE     2
typedef struct input_file_ INPUT_FILE;
struct input_file_ {
    int type;
    int string_type;	/* IF, IIF, IFF, IFT, IFTF, REPEAT, WHILE -- or FILE_DELETE (for F_FILE type only)*/ 
    int repeat;
    TOKEN_LIST* while_cond;
    int preprocess_me;
    int abort;
    FILE *f;
    int lineno;
    int base_lineno;
    char *name;
    YY_BUFFER_STATE buff;
    int yychar;
    YYSTYPE yylval;
    INPUT_FILE *next;
};

typedef struct macro_def_ MACRODEF;
struct macro_def_ {
    char *name;
    char *body;
    int narg;
    int var_arg;
    unsigned int skiparglen;
    int *skipargck;
    int *argtypes;
    char** args;
    char** argdefs;
    char *fname;
    int lineno;
};
#define MACRO_ARG_NORMAL 0
#define MACRO_ARG_KWONLY 1

typedef struct macro_stk_ MACRO_STK;
struct macro_stk_ {
    char** argl;
    int var_arg_idx;
    MACRODEF* mdef;
    INPUT_FILE* bfile;
    int prep_return;
    MACRO_STK *next;
};

#define DEFAULT_PSECT_LENGTH	(64*1024)
#define DEFAULT_PSECT_LENGTH_LW (DEFAULT_PSECT_LENGTH>>2)
#define DEFAULT_SECT_INIT_LENGTH      (128)
#define DEFAULT_SECT_INIT_LENGTH_LW   (DEFAULT_SECT_INIT_LENGTH>>2)
#define DEFAULT_PSECT_ALIGN	(2)

#define DEFAULT_DSECT_HEADER_LENGTH (3*8)
#define DSECT_FLAG (1)
#define PSECT_FLAG (0)

#define PHYSICAL_REGION 0
#define ALL_REGIONS 0xffffffff

/* Tags for the modifications*/
#define	TD32	1
#define	TD64	2
#define	TMDS	3
#define	TBDS	4
#define	TIOP	5         
#define	TMHW10	6
#define	TMOD	7
#define TADD	8
#define	TJDS	9
#define	TPDS	10
#define	TMHW12	11

typedef unsigned int B32;
typedef union squad B64;
union squad {
    B32			ulong[2];
    unsigned char       ubyte[8];
};

typedef struct psectable PENTRY;
struct psectable {
    char*	name;		/*Psect Name */
    B64		base;		/*Base of Psect */
    B64         last;           /*Last PC of Dsect (only available after call to Dsectfpc) */
    B64		offset;		/*Offset of Psect (if any)*/
    B64		mpc;		/*Maximum PC for this Psect (really # bytes already in sect, becomes max pc only during gen code)*/
    B64		cpc;		/*Current PC for this Psect */
    B32		len;		/* current max. length in bytes for psect */
    B32		lwlen;		/* current max. length in LWs for psect */
    B32		*mp;		/*Memory pointer for this Psect*/
    B32		*wrnum;		/*number of times each LW written*/
    B64       link_size;        /*Dictated size of section ... 0 means just use real size */ 
    unsigned int align;		/*Alignment of this Psect 2^align*/
    /*
    ** The remaining structure is used only for a Dsect
    */
    int       entry_type;       /*Specifies the type of this structure, (Psect, Dsect, Dsect_Hdr) */
    int       hdr_size;         /*Size of header for a Dsect */
    int       file_offset;      /*File location of code/data for a Dsect */
    int       code_size;        /*Size of code/data (in bytes) for a Dsect */
    PENTRY*   o_psect;          /*Pointer to Dsect that contains other info for this header */
    int       link_attr;        /*Attributes for the way to link the desct -- for types see below*/
    int       link_address;     /*Flag that indicates base address of a Dsect is hard coded via .link_address directive */
    int       link_after;       /*Flag that indicates current Dsect to be linked after Dsect specified by .link_after directive */
    int       link_first;       /*Flag that indicates current Dsect to be linked first in dxe */
    int       link_last;        /*Flag that indicates current Dsect to be linked last in dxe */
    char*     link_after_name;  /*Pointer to name of Dsect that this Dsect should be linked after */
    int       hdr_pos;          /*Output file position of header */
    int       mem_region;       /*region of memory dsect belongs to .... allows HAL to reuse addresses*/
};

/*
 * .link_attribute flags
 */
#define D_OVERLAP_OK     1
#define D_DIRECTLY_AFTER 2


typedef struct dsect_header DSECT_HDR;
struct dsect_header {
    int hdr_size;
    int mbz_0;
    int file_offset;
    int mbz_1;
    int code_size;
    int mbz_2;
};

#define PENTRY_TYPE_PSECT      0
#define PENTRY_TYPE_DSECT      1
#define PENTRY_TYPE_DSECT_HDR  2

typedef struct lineloc LINELOC;
struct lineloc {
    int type, string_type;
    int lineno;
    char* fname;
    char* name;
    LINELOC* next;
};

struct refnode {
    PENTRY*  sect;		/* the psect of this reference */
    int	     ic;		/*index to memory array */
    B32	     wrnum; 		/* value of wrnum when this reference was created */
    LINELOC* lloc;		/*full fledged location information*/
    int	     type;		/*format of the instruction (Tags)*/
    RNODE*   next;		/*->next reference */
    fpos_t   lpos[2];		/*listing file position(s)*/
    int	     listused;		/* used in listing file? */
};

typedef struct symboltable  SENTRY;
struct symboltable {
    char*		symbol;		/*Symbol name */
    int			valid;		/*value is valid */
    int			reloc;		/*Relocatable */
    int			pc;		/* PC that label was defined */
    int			fwd_ref;	/* was a forward reference error printed? */
    unsigned int	value[2];	/*value of the symbol*/
    RNODE*		ref;		/*->refnode s */
    PENTRY*		sect;		/*->Psect Info*/
    TOKEN_LIST*		expr;
};

#define COND_EQ(t)	(((t)->tok.q.v[0] == 0) && ((t)->tok.q.v[1] == 0))
#define COND_GT(t)	((((t)->tok.q.v[1] & 0x80000000) == 0) && (((t)->tok.q.v[0] != 0) || ((t)->tok.q.v[1] != 0)))
#define COND_LT(t)	((((t)->tok.q.v[1] & 0x80000000) != 0) && (((t)->tok.q.v[0] != 0) || ((t)->tok.q.v[1] != 0)))

typedef struct absexpr ABSEXPR;
struct absexpr {
    int qual;
    int qual_result;
    TOKEN_LIST* expr;
    TOKEN_LIST* result;
    LINELOC* lloc;
    ABSEXPR* next;
};

extern ABSEXPR* absexpr_head;

/*
 * Command line switches.  Defined in main.c
 */
extern int allow_core_dump;
extern int warn_reloc_absexpr;
extern int quiet;
extern int page_size;
extern int yydebug;
extern int yy_flex_debug;
extern int chip;
extern int print_no_trace;

/* reconized chip types */
#define EV4 0
#define EV5 1
#define EV6 2

extern char* output_file;
extern char** idirs;
extern int Trace;
extern B32 Sa[2];
extern FILE		*Ot;		/* Output for Symbol table transaction*/
extern FILE	  	*Olist;			/* Output for Listing */
extern avl_tree        *SymTable;		/* AVL data base for symbols*/
extern avl_tree        *SecTable;		/* AVL data base for Psects*/
extern dll             *DSecTable;              /* DLL data base for Dsects*/
extern PENTRY		*Cpsect;		/* Current PSECT */

extern B32 opcode;
extern B32  hpc, pc, ic;
extern B32  *m;

/*
 * CMD directive stuff
 */
typedef struct cmdinfo CMDINFO;
struct cmdinfo {
    int number;
    char filename[L_tmpnam + 10];
    FILE *fileptr;
    CMDINFO *next;
};
extern CMDINFO *cmd_list;

/*
 * Listing file stuff.
 */
#define L_MISC		(1<<0)	/* lines that are not any of the below */
#define L_BINARY	(1<<1)	/* also: MEB */
#define L_CALLS		(1<<2)	/* also: MC */
#define L_CONDITIONALS	(1<<3)	/* also: CND */
#define L_DEFINITIONS	(1<<4)	/* also: MD */
#define L_EXPANSIONS	(1<<5)	/* also: ME */
#define L_NOTALL	(1<<6)  /* don't print all binary code generated. (used by .blkx) */
#define L_CMDIN         (1<<7)  /* Lines that make up the input file(s) to .docmd */
#define L_ASSIGN        (1<<8)  /* Lines that are assignments */
extern FILE* listing_file;
extern int listing_enable;
extern char* listing_fname;
extern int listing_flags;
extern char* listing_line;
extern TOKEN_LIST* headref;
extern TOKEN_LIST** endref;
extern TOKEN_LIST* fname_list;

/*
 * Current created local label number.
 */
extern int local_label;

/*
 * Input file stack.
 */
extern INPUT_FILE *ifile;
extern INPUT_FILE *cur_ifile;

/*
 * Macro call stack
 */
extern MACRO_STK *macstk;

/*
 * PRAGMA flag definitions
 *
 */
#define PRAGMA_MAC_UNLIMITED 0
#define PRAGMA_MAC_NONE      1
#define PRAGMA_MAC_ONCE      2
extern unsigned int global_pragma;


/*
 * Current psect name, and stack of previous names.
 *
 * Needed for .save and .restore directives.
 */
extern TOKEN_LIST *cur_psect;
extern PSECT_LIST *psect_stk;

/*
 * Start line (in current file) of a block of code scanned to be
 * conditionally executed. (e.g. .repeat, .if, .macro, etc...)
 *
 * Only used to hold value until the directive gets executed.
 */
extern int start_line;

/*
 * Information used to determine where an error occured.
 *
 * errout() uses these.
 */
extern char *cur_fname;
extern int cur_base_lineno;

/*
 * Bison variables.  Used when pushing input files.
 */
extern int yychar;
extern YYSTYPE yylval;

/*
 * The symbol table.  Contains a bunch of SENTRY structures.
 * Defined in hal.c
 */
extern avl_tree* SymTable;

/*
 * The opcode being assembled.
 */
extern B32 opcode;

/*
 * The block number of the current set of local labels.
 */
extern int local_label_blknum;

#if defined(VMS) || defined(__MSDOS__)
#include "parser_tab.h"
#else
#include "parser.tab.h"
#endif

#define E_INFO		0
#define E_WARN		1
#define E_ERR		2
#define E_NO_LNUM	0x8000
void errout PROTO((int, char *, char *, ...));
void dbgout PROTO((char *, ...));

/* prototypes */
void UNIX_expand_pathname PROTO((const char *inpath, char *outpath));
int next_file PROTO((void));
void add_input PROTO((char *name, int type, int repeat, TOKEN_LIST* while_cond, int where, int base_lineno, int string_type));
YYSTYPE cvtradix PROTO((char *text, int radix, int type));
YYSTYPE cvtasciiop PROTO((char *text));
int do_include PROTO((TOKEN_LIST *name_tok));
int do_include_dir PROTO((TOKEN_LIST *name_tok));
int do_minclude PROTO((TOKEN_LIST *name_tok));
int eval_while_cond PROTO((TOKEN_LIST *expr));
void mul64 PROTO((uint32 *a, uint32 b));
void sub64 PROTO((int32 *a, int32 *b, int32 *c));
TOKEN_LIST *addq PROTO((TOKEN_LIST *op1, TOKEN_LIST *op2));
TOKEN_LIST *subq PROTO((TOKEN_LIST *op1, TOKEN_LIST *op2));
TOKEN_LIST *mulq PROTO((TOKEN_LIST *op1, TOKEN_LIST *op2));
TOKEN_LIST *divq PROTO((TOKEN_LIST *op1, TOKEN_LIST *op2));
TOKEN_LIST *notq PROTO((TOKEN_LIST *op1));
TOKEN_LIST *log_notq PROTO((TOKEN_LIST *op1));
TOKEN_LIST *shiftq PROTO((TOKEN_LIST *op1, TOKEN_LIST *op2));
TOKEN_LIST *andq PROTO((TOKEN_LIST *op1, TOKEN_LIST *op2));
TOKEN_LIST *log_andq PROTO((TOKEN_LIST *op1, TOKEN_LIST *op2));
TOKEN_LIST *orq PROTO((TOKEN_LIST *op1, TOKEN_LIST *op2));
TOKEN_LIST *log_orq PROTO((TOKEN_LIST *op1, TOKEN_LIST *op2));
TOKEN_LIST *xorq PROTO((TOKEN_LIST *op1, TOKEN_LIST *op2));
YYSTYPE make_token PROTO((char *text, int type));

void *my_alloc PROTO((unsigned int size));
void *my_calloc PROTO((int n, int size));

TOKEN_LIST *copy_expr PROTO((TOKEN_LIST *expr));
char *find_fname PROTO((INPUT_FILE*));

TOKEN_LIST* eval_subexpr PROTO((TOKEN_LIST *op1, TOKEN_LIST* op2, TOKEN_LIST* oper));
TOKEN_LIST* get_symbol_value PROTO((TOKEN_LIST* name, int is_while));
TOKEN_LIST* eval_abssubexpr PROTO((TOKEN_LIST *op1, TOKEN_LIST* op2, TOKEN_LIST* oper));

void		pgencode PROTO((char*, PENTRY*));
void		psectcalc PROTO((char*, PENTRY*));
void		dsectfpc PROTO((PENTRY*, PENTRY*));
SENTRY          *symdata PROTO((char*));
void            symuse PROTO((char*, int));
void		symupdate PROTO((char*, SENTRY*));
void		symresolve PROTO((char*, SENTRY*));
void		symtableit PROTO((char*, SENTRY*));
void            symdef PROTO((char* s, unsigned int hv, unsigned int lv, TOKEN_LIST* expr, int reloc));
void             symcreate PROTO((SENTRY **pe));

void		sect_free PROTO((PENTRY*));

TOKEN_LIST* make_value_token(uint32 hi, uint32 lo);

void init_memmgt PROTO((void));
void end_memmgt PROTO((void));
YYSTYPE make_token PROTO((char *text, int type));
YYSTYPE create_zero_tok PROTO((void));
TOKEN_LIST* alloc_tok PROTO((void));
void free_tok PROTO((TOKEN_LIST* ttok));
void free_tok_s PROTO((TOKEN_LIST* ttok));
void free_tok_list PROTO((TOKEN_LIST* ttok));
void free_tok_list_s PROTO((TOKEN_LIST* ttok));
void free_macstk PROTO((MACRO_STK* tstk));
void free_macrodef PROTO((MACRODEF* tmac));
void free_location PROTO((LINELOC* lloc));
void free_rnode PROTO((RNODE* r));
void free_absexpr PROTO((ABSEXPR* tabs));

void push_current_ifile PROTO((void));
char** setup_macro_call_args PROTO((TOKEN_LIST* mactok, MACRODEF** macdef_p, int *));

void		symfree PROTO(( SENTRY*));

char *strlow PROTO((char *str));

void write_mem_array PROTO((B32 ic, B32 lw));
B32 get_mem_array PROTO((B32 ic));

void clear_listing_line PROTO((void));
void do_list PROTO((int, int));

void apcf PROTO((B32 d, B32 fv));
void set_psect PROTO((char* s, unsigned int align, int entry_type));
void psect_save PROTO((PENTRY* ps));
void psect_restore PROTO((PENTRY* ps));
void add64 PROTO((int32* a, int32* b, int32* c));
int in_body PROTO((int type));
void free_file PROTO((INPUT_FILE *tfil));
void hal_init_before PROTO((void));
void hal_init_after PROTO((void));
void load_default_operators PROTO((void));
int yyparse PROTO((void));
void generate_code PROTO((void));
void uninit PROTO((void));
void yyerror PROTO((char *s));
int yylex PROTO((void));
void do_align PROTO((TOKEN_LIST* bound, TOKEN_LIST* fill));
void do_genmsg PROTO((int type, TOKEN_LIST *exp, TOKEN_LIST *cmt));
void do_psect PROTO((TOKEN_LIST *name, TOKEN_LIST *args, int entry_type));
void do_save_psect PROTO((int blknum));
int do_rest_psect PROTO((int type));
void do_cmdin PROTO((int number, TOKEN_LIST *textblock));
char *do_cmd PROTO((TOKEN_LIST *cmd_string));
void define_macro PROTO((TOKEN_LIST *name, int narg, int var_arg, TOKEN_LIST* args, TOKEN_LIST* argdefs, TOKEN_LIST *body));
void expand_macro PROTO((MACRODEF* mtmp, char** argl));
int do_if_condition PROTO((int cond, TOKEN_LIST* expr1, TOKEN_LIST* expr2, int cleanup));
int assign_macro_arg PROTO((MACRODEF* mtmp, char*** margs,  int *margs_size,TOKEN_LIST* symb, TOKEN_LIST* value, int *pos));
TOKEN_LIST* sym2num PROTO((TOKEN_LIST *expr));
TOKEN_LIST* eval_expr PROTO((TOKEN_LIST *expr, int* reloc_used));
void save_absexpr PROTO((TOKEN_LIST* expr, TOKEN_LIST* result));
void spc PROTO((B32, B32));
void ipc PROTO((int d));
void increase_psect_size PROTO((int numlws));
void do_header PROTO((void));
void do_link_address PROTO((TOKEN_LIST* addr_tok));
void do_link_offset PROTO((TOKEN_LIST* addr_tok));
void do_link_attr PROTO((int attr));
void do_link_after PROTO((TOKEN_LIST* name_tok));
void do_link_first PROTO((void));
void do_link_last PROTO((void));
void do_link_size PROTO((TOKEN_LIST* sz_tok));
void do_link_region PROTO((TOKEN_LIST* rgn_tok));
void sect_foreach PROTO((avl_tree *st, dll *dst, void (*func_p)(), void (*func_d)(), int dir));
int sect_lookup PROTO((avl_tree *st, dll *dst, char *s, PENTRY **e, int entry_type));
void sort_dsect PROTO((dll *dst));
int sign_ext_ok PROTO((TOKEN_LIST *op1, int sign_bit));
void qual_absexpr PROTO((int qual, int result));

void start_up_preprocessor(void);
void save_macro(void);
void rest_macro(FILE *mfile);
void create_macro(MACRODEF *mtmp, char *name);

int sys PROTO((char *sys_cmd));

void yy_delete_buffer PROTO(( YY_BUFFER_STATE b ));

void print_location PROTO((int level, char* rtn, LINELOC* lloc));
LINELOC* save_location();
#endif
