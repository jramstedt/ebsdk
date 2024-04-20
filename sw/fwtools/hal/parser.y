%{
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
%}

%token ENDP
%token INS_FOP INS_FP2 INS_IOP INS_MEM INS_JSR INS_BRN INS_M1A INS_M1B INS_MIS INS_FPCR INS_RPCC
%token INS_SEXT INS_IP2 INS_IP3 INS_FTI INS_LSB INS_SSB INS_ITF INS_UNOP INS_CALLPAL
%token INS_IPR4 INS_HWM4 INS_IPR5 INS_HWM5 INS_REI6 INS_MTI6 INS_MFI6 INS_HWM6
%token INS_1REG
%token IREG FREG
%token ADDR QUAD LONG BLKL BLKQ IDENT TITLE WORD BYTE
%token BODYTEXT
%token K_BYTE K_WORD K_LONG K_QUAD K_PAGE
%token LOCAL_LABEL LOCAL_BLOCK
%token NOLIST LIST MEB MC CND MD ME ASSIGN CMDIN
%token NODEBUG K_DEBUG

%token NUMBER IF WHILE BREAK CONTINUE INCLUDE INCLUDE_DIR MINCLUDE OPEN_ANGLE CLOSE_ANGLE
%token PLUS MINUS SHIFT EQUAL NOT LOG_AND LOG_OR LOG_NOT
%token COLON SYMBOL LABEL STRING EOL EOE DOT EQ GT LT DF NE GE LE NDF ENDC ENDW
%token ALIGN IFF IFT IFTF REPEAT ENDR CMDIN ENDI DOCMD COMMENT MACRO ENDM PRAGMA
%token MACRO_CALL DOTDOTDOT NEXT_ARG ARG_VALUE ASCII ASCIZ SAVE_PSECT SAVE_DSECT RESTORE_PSECT RESTORE_DSECT PSECT
%token DSECT LINK_ADDRESS LINK_AFTER LINK_FIRST LINK_LAST LINK_ATTR LINK_OFFSET LINK_REGION LINK_SIZE HEADER ENDH
%token OVERLAP_OK DIRECTLY_AFTER
%token MAC_NONE MAC_ONCE MAC_UNLIMITED LEXER_DEBUG_ON LEXER_DEBUG_OFF PARSER_DEBUG_ON PARSER_DEBUG_OFF
%token WARN ERROR PRINT
%token B NB IDN DIF
%token IIF
%token ARCH A_EV4 A_EV5 A_EV6

%left '+' '-' '@' '&' '!' '\\' '*' '/' LOG_AND LOG_OR
%right NOT UMINUS LOG_NOT

%%
toplevel:
	sequence
    ;

sequence:
	/* empty */
    |	action
    |	sequence action
    ;

action:
        EOL			{ do_list(L_MISC, cur_ifile->lineno + cur_base_lineno); }
    |   label
    |   label EOL		{ do_list(L_BINARY | L_MISC, cur_ifile->lineno + cur_base_lineno); } 
    |	instruction EOL		{ do_list(L_BINARY | L_MISC, start_line); }
    |   assignment		{ do_list(L_ASSIGN | L_MISC, start_line); saved_absexpr = 0; }
    |	directive		{ /* do_list handled inside directive */ saved_absexpr = 0; }
    |   ENDP EOL		{ do_list(L_MISC, start_line); return; }
    |   error EOL		/* do_list handled by errout ??? */
    ;

label:
	SYMBOL ':'		{ define_label($1, hpc, pc); 
                                  if (used_local_label) { local_label_blknum = ++max_blknum; used_local_label = 0; } 
                                }
    |	SYMBOL ':' ':'		{ define_label($1, hpc, pc); 
                                  if (used_local_label) { local_label_blknum = ++max_blknum; used_local_label = 0; } }
    |   LOCAL_LABEL ':'		{ define_label($1, hpc, pc); used_local_label = 1; }
    |   LOCAL_LABEL ':' ':'	{ define_label($1, hpc, pc); used_local_label = 1; }
    ;

instruction:
	INS_FOP three_freg	{ dbug(); write_mem_array(ic++, opcode); ipc(4); }
    |   INS_FP2 two_freg	{ dbug(); write_mem_array(ic++, opcode); ipc(4); }
    |   INS_IOP ireg ','	{ opcode |= regnum<<21;  ipc(4); }
	intreg			{ dbug(); write_mem_array(ic++, opcode); }
    |   INS_FTI freg ','	{ opcode |= regnum<<21;  ipc(4); }
	ireg			{ dbug(); write_mem_array(ic++, opcode | regnum); }
    |   INS_ITF ireg ','	{ opcode |= regnum<<21;  ipc(4); }
	freg			{ dbug(); write_mem_array(ic++, opcode | regnum); }
    |   INS_SEXT intreg	        { dbug(); write_mem_array(ic++, opcode); ipc(4); }
    |   INS_IP2 two_ireg	{ dbug(); write_mem_array(ic++, opcode); ipc(4); }
    |   INS_IP3 ireg ','        { opcode |= regnum<<21; ipc(4); }
        two_ireg	        { dbug(); write_mem_array(ic++, opcode); }
    |   INS_LSB ireg ',' 	{ opcode |= regnum; ipc(4); }
	'(' ireg ')'	        { dbug(); write_mem_array(ic++, opcode | (regnum<<16)); }
    |   INS_SSB ireg ',' 	{ opcode |= regnum<<21; ipc(4); }
        '(' ireg ')'	        { dbug(); write_mem_array(ic++, opcode | (regnum<<16)); }
    |   INS_MEM ifreg ',' 	{ opcode |= regnum<<21; ipc(4); }
	disp16 '(' ireg ')'	{ dbug(); write_mem_array(ic++, opcode | (regnum<<16)); }
    |   INS_UNOP disp16 '(' ireg ')' { dbug(); write_mem_array(ic++, opcode | (regnum<<16)); ipc(4); }
    |   INS_UNOP disp16         { dbug(); write_mem_array(ic++, opcode | (30<<16)); ipc(4); } /* r30 is used by compilers */
    |   INS_JSR ireg ',' 	{ opcode |= regnum<<21; ipc(4); }
	'(' ireg ')' hint	{ dbug(); write_mem_array(ic++, opcode | (regnum<<16)); }
    |   INS_JSR ireg ',' 	{ opcode |= regnum<<21; ipc(4); }
	disp14 '(' ireg ')'	{ dbug(); write_mem_array(ic++, opcode | (regnum<<16)); }
    |	INS_BRN ifreg ','	{ opcode |= regnum<<21; ipc(4); }
        disp21                  { dbug(); write_mem_array(ic++, opcode); }
    |	INS_CALLPAL disp26	{ dbug(); write_mem_array(ic++, opcode); ipc(4); }
    |	INS_RPCC ireg ','	{ opcode |= regnum<<21; ipc(4); }
	'(' ireg ')'	        { dbug(); write_mem_array(ic++, opcode | (regnum<<16)); }
    |	INS_RPCC ireg		{ dbug(); write_mem_array(ic++, opcode | (regnum<<21) | (31<<16)); ipc(4); }
    |	INS_M1A ireg		{ dbug(); write_mem_array(ic++, opcode | (regnum<<21)); ipc(4); }
    |	INS_M1B '(' ireg ')'	{ dbug(); write_mem_array(ic++, opcode | (regnum<<16)); ipc(4); }
    |	INS_MIS			{ dbug(); write_mem_array(ic++, opcode); ipc(4); }
    |   INS_1REG ireg           { dbug(); write_mem_array(ic++, opcode | regnum); ipc(4); }
    |   INS_FPCR freg		{ dbug(); write_mem_array(ic++, opcode | (regnum<<21) | (regnum<<16) | regnum); ipc(4); }
    |   INS_FPCR three_freg	{ dbug();
                                  if ((((opcode >> 21) & 0x1F) != (opcode & 0x1F)) ||
                                      (((opcode >> 16) & 0x1F) != (opcode & 0x1F))) {
                                      errout(E_WARN, "parser", "mt/mf_fpcr instruction specified without the same three registers.\n");
                                  }
                                  write_mem_array(ic++, opcode);
                                  ipc(4);
                                }
    |	INS_IPR4 ireg ',' 	{ opcode |= (regnum<<21) | (regnum<<16); ipc(4); }
	ipr			{ dbug(); write_mem_array(ic++, opcode); }
    |   INS_HWM4 ireg ',' 	{ opcode |= regnum<<21; ipc(4); }
	disp12 '(' ireg ')'     { dbug(); write_mem_array(ic++, opcode | (regnum<<16)); }
    |	INS_IPR5 ireg ',' 	{ opcode |= (regnum<<21) | (regnum<<16); ipc(4); }
	ipr			{ dbug(); write_mem_array(ic++, opcode); }
    |   INS_HWM5 ireg ',' 	{ opcode |= regnum<<21; ipc(4); }
	disp10 '(' ireg ')' mode { dbug(); write_mem_array(ic++, opcode | (regnum<<16)); }
    |	INS_REI6 '(' ireg ')'   { ipc(4); dbug(); write_mem_array(ic++, opcode | (regnum<<16));}
    |	INS_REI6 ireg ','       { opcode = (opcode & ~(0x1f << 21)) | regnum<<21; ipc(4); }
        '(' ireg ')' hint       { dbug(); write_mem_array(ic++, opcode | (regnum<<16));}
    |	INS_MTI6 ireg ',' 	{ opcode |= regnum<<16; ipc(4); }
	ipr scbd6		{ dbug(); write_mem_array(ic++, opcode); }
    |	INS_MFI6 ireg ',' 	{ opcode |= regnum<<21; ipc(4); }
	ipr scbd6		{ dbug(); write_mem_array(ic++, opcode); }
    |   INS_HWM6 ireg ',' 	{ opcode |= regnum<<21; ipc(4); }
	disp12 '(' ireg ')'     { dbug(); write_mem_array(ic++, opcode | (regnum<<16)); }
    ;

ifreg:
	IREG
    |   FREG
    |   absexpr			{ regnum = $1->tok.q.v[0];
    				  if (regnum<0 || regnum>31) {
                                    errout(E_WARN,"parser","Register expression value was not 0 >= reg >= 31. Value will be masked.\n");
                                    regnum &= 0x1f;
                                  }
				  free_tok($1);
        			}
    ;

three_freg:
	freg ','		{ opcode |= regnum<<21; }
	freg ','		{ opcode |= regnum<<16; }
	freg			{ opcode |= regnum; }
    ;

two_freg:
	freg ','		{ opcode |= (31<<21) | (regnum<<16); }
	freg			{ opcode |= regnum; }
    ;

intreg:
	ireg ','		{ opcode |= (regnum<<16); }
	ireg			{ opcode |= (regnum);     }
    |	'#' expr ','		{ opcode |= 0x1000; handle_instr_expr($2, TIOP); }
	ireg			{ opcode |= regnum; }
    ;

two_ireg:
	ireg ','		{ opcode |= (regnum<<16); }
	ireg			{ opcode |= (regnum);     }
    ;

disp10:
	/* empty */
    |	expr			{ handle_instr_expr($1, TMHW10); }
    ;

disp12:
	/* empty */
    |	expr			{ handle_instr_expr($1, TMHW12); }
    ;

disp14:
	expr			{ handle_instr_expr($1, TJDS); }
    ;

disp16:
	/* empty */		
    |	expr			{ handle_instr_expr($1, TMDS); }
    ;

disp21:
	expr			{ handle_instr_expr($1, TBDS); }
    ;

disp26:
	expr			{ handle_instr_expr($1, TPDS); }
    ;

hint:
	/*nothing*/
    |	',' disp14
    ;

mode:
	/*nothing*/
    |	',' expr		{ handle_instr_expr($2, TMOD); }
    ;

ipr:
	absexpr			{ opcode |= ($1->tok.q.v[0] & 0xffff);
        			  if (($1->tok.q.v[0] & ~0xFFFF) || $1->tok.q.v[1])
                		    errout(E_WARN, "parser", "constant larger than IPR number (16 bits), truncating upper bits.\n");
				  free_tok($1);
        			}
    ;

scbd6:
        /*nothing*/
    |   ',' absexpr		{ opcode |= ($2->tok.q.v[0] & 0xff);
        			  if (($2->tok.q.v[0] & ~0xFF) || $2->tok.q.v[1])
                		    errout(E_WARN, "parser", "constant larger than scoreboard field (8 bits), truncating upper bits.\n");
    				  free_tok($2);
    				}
    ;

ireg:
	IREG
    |   absexpr			{ regnum = $1->tok.q.v[0]; free_tok($1);
                                  if (regnum<0 || regnum>31) {
                                      errout(E_WARN,"parse","Register expression value was not 0 >= reg >= 31. Value will be masked.\n");
                                      regnum &= 0x1f;
                                  }
                                }
    ;

freg:
	FREG
    |	absexpr			{ regnum = $1->tok.q.v[0]; free_tok($1); 
                                  if (regnum<0 || regnum>31) {
                                      errout(E_WARN,"parse","Register expression value was not 0 >= reg >= 31. Value will be masked.\n");
                                      regnum &= 0x1f;
                                  }
                                }
    ;

assignment:
	SYMBOL '=' expr EOL	{ define_symbol($1, $3); }
    |   DOT '=' absexpr EOL	{ dot_equal($3); qual_absexpr(DOT, 0); }
    ;

expr:
	element				{ $$ = $1; }
    |	expr '+' expr			{ $$ = eval_subexpr($1, $3, $2); if ($$ == NULL) $$ = chain($1, $3, $2); }
    |	expr '-' expr			{ $$ = eval_subexpr($1, $3, $2); if ($$ == NULL) $$ = chain($1, $3, $2); }
    |	expr '*' expr			{ $$ = eval_subexpr($1, $3, $2); if ($$ == NULL) $$ = chain($1, $3, $2); }
    |	expr '/' expr			{ $$ = eval_subexpr($1, $3, $2); if ($$ == NULL) $$ = chain($1, $3, $2); }
    |	expr '@' expr			{ $$ = eval_subexpr($1, $3, $2); if ($$ == NULL) $$ = chain($1, $3, $2); }
    |	expr '&' expr			{ $$ = eval_subexpr($1, $3, $2); if ($$ == NULL) $$ = chain($1, $3, $2); }
    |	expr '!' expr			{ $$ = eval_subexpr($1, $3, $2); if ($$ == NULL) $$ = chain($1, $3, $2); }
    |	expr '\\' expr			{ $$ = eval_subexpr($1, $3, $2); if ($$ == NULL) $$ = chain($1, $3, $2); }
    |	expr LOG_AND expr		{ $$ = eval_subexpr($1, $3, $2); if ($$ == NULL) $$ = chain($1, $3, $2); }
    |	expr LOG_OR expr		{ $$ = eval_subexpr($1, $3, $2); if ($$ == NULL) $$ = chain($1, $3, $2); }
    |	NOT expr			{ $$ = eval_subexpr($2, NULL, $1); if ($$ == NULL) $$ = chain($2, $1, NULL); }
    |	LOG_NOT expr			{ $$ = eval_subexpr($2, NULL, $1); if ($$ == NULL) $$ = chain($2, $1, NULL); }
    |	'-' expr %prec UMINUS		{ $1->type = UMINUS; $$ = eval_subexpr($2, NULL, $1); if ($$ == NULL) $$ = chain(create_zero_tok(), $2, $1); }
    |	'<' expr '>'			{ $$ = $2; }
    ;

element:
	NUMBER				{ $$ = $1; }
    |	SYMBOL				{ $$ = get_symbol_value($1, is_while); }
    |	LOCAL_LABEL			{ $$ = get_symbol_value($1, is_while); }
    |	DOT				{ $$ = convert_dot($1); }
    ;

absexpr:
    expr				{ int reloc_used;
                                          $$ = eval_expr($1, &reloc_used);
                                          if (reloc_used && ($$ != NULL))
                                              save_absexpr($1, $$);
                                          else
                                              free_tok_list_s($1);
                                        }
    ;

directive:
	INCLUDE STRING EOL		{ do_include($2); do_list(L_MISC, start_line); }
    |	INCLUDE_DIR STRING EOL		{ do_include_dir($2); do_list(L_MISC, start_line); }
    |	MINCLUDE STRING EOL		{ do_minclude($2); do_list(L_MISC, start_line); }
    |   PRAGMA                          { temp_pragma_flags = 0; }
        pragma_flags EOL                { do_list(L_MISC, start_line); global_pragma = temp_pragma_flags; }
    |   ARCH arch_flag EOL              { do_list(L_MISC, start_line); }
    |	ASCII				{ byte_in_lw = nchar = 0; }
            string_seq EOL		{ fill_to_lw(); ipc(nchar); do_list(L_BINARY | L_MISC, start_line); }
    |	ASCIZ				{ byte_in_lw = nchar = 0; }
 	    string_seq EOL		{ store_expr(create_zero_tok()); fill_to_lw(); ipc(nchar); do_list(L_BINARY | L_MISC, start_line); } 
    |	IF if_condition EOL mainbody	{ save_start_line = start_line; }
	    ifsubbody_seq
	    ENDC EOL 			{ do_list(L_CONDITIONALS, save_start_line); 
					  if ($4 || $6) { 
					      push_current_ifile(); 
                                              push_bodies(chain($4, $6, NULL)); 
                                              (void)next_file(); 
                                          } 
                                        } 
    |	IIF if_condition ','
    	    BODYTEXT EOL 		{  do_list(L_CONDITIONALS, start_line); 
                                           if (if_result) { 
                                               $4->type = IIF; $4->lineno--;	/* subtract 1 because there wasn't really a \n before the body */
                                               push_current_ifile(); 
                                               push_bodies($4); 
                                               (void)next_file(); 
                                           } else
                                               free_tok_s($4)
                                        }
    |	REPEAT opt_absexpr EOL
            BODYTEXT ENDR EOL		{ do_list(L_DEFINITIONS, start_line); push_repeat_body($4, $2);}
    |   WHILE {is_while =1;} while_condition {is_while=0;} EOL
            BODYTEXT ENDW EOL		{ do_list(L_DEFINITIONS, start_line); push_while_body($6, $3);}
    |   BREAK EOL                       { do_list(L_DEFINITIONS, start_line); }
    |   CONTINUE EOL                    { do_list(L_DEFINITIONS, start_line); }
    |	CMDIN absexpr EOL               
            BODYTEXT ENDI EOL		{ do_list(L_CMDIN, start_line); do_cmdin($2->tok.q.v[0], $4); free_tok($2); }
    |   DOCMD STRING EOL                { char *outnm;
                                          do_list(L_CALLS | L_MISC, start_line); 
                                          outnm = do_cmd($2); 
                                          if (outnm!=NULL) {
                                             push_current_ifile();
                                             /* create INPUT_FILE struct for this cmd output file on ifile stack */
                                             /*NOTE: string_type means this is a temp file and delete it once done */
                                             add_input(outnm, F_FILE, 1, NULL, ADD_PUSH, 0, FILE_DELETE); 
                                             (void)next_file();
                                          }
                                        }
    |	ALIGN alignbound opt_alignfill EOL { do_align($2, $3); do_list(L_BINARY | L_MISC, start_line); }
    |	ADDR 				{ mem_type = TADD; num_mem_expr = 0; }
	    mem_exprseq EOL		{ ipc(8*num_mem_expr); do_list(L_BINARY | L_MISC, start_line); }
    |	QUAD 				{ mem_type = TD64; num_mem_expr = 0; }
	    mem_exprseq EOL		{ ipc(8*num_mem_expr); do_list(L_BINARY | L_MISC, start_line); }
    |	LONG 				{ mem_type = TD32; num_mem_expr = 0; }
	    mem_exprseq EOL		{ ipc(4*num_mem_expr); do_list(L_BINARY | L_MISC, start_line); }
    |	WARN opt_absexpr COMMENT EOL	{ do_genmsg(WARN, $2, $3); do_list(L_MISC, start_line); }
    |	ERROR opt_absexpr COMMENT EOL	{ do_genmsg(ERROR, $2, $3); do_list(L_MISC, start_line); }
    |	PRINT opt_absexpr COMMENT EOL	{ do_genmsg(PRINT, $2, $3); do_list(L_MISC, start_line); }
    |	PSECT SYMBOL psect_args EOL	{ do_psect($2, $3, PENTRY_TYPE_PSECT); do_list(L_BINARY | L_MISC, start_line); }
    |	SAVE_PSECT EOL			{ do_save_psect(max_blknum++); do_list(L_BINARY | L_MISC, start_line); }
    |	SAVE_DSECT EOL			{ do_save_psect(max_blknum++); do_list(L_BINARY | L_MISC, start_line); }
    |	SAVE_PSECT LOCAL_BLOCK EOL	{ do_save_psect(local_label_blknum); do_list(L_BINARY | L_MISC, start_line); }
    |	SAVE_DSECT LOCAL_BLOCK EOL	{ do_save_psect(local_label_blknum); do_list(L_BINARY | L_MISC, start_line); }
    |	RESTORE_PSECT EOL		{ local_label_blknum = do_rest_psect(PENTRY_TYPE_PSECT); do_list(L_BINARY | L_MISC, start_line); }
    |	RESTORE_DSECT EOL		{ local_label_blknum = do_rest_psect(PENTRY_TYPE_DSECT); do_list(L_BINARY | L_MISC, start_line); }
    |	BLKL absexpr EOL		{ ipc($2->tok.q.v[0] << 2); ic = pc >> 2; do_list(L_BINARY | L_MISC | L_NOTALL, start_line); free_tok($2); }
    |   BLKQ absexpr EOL		{ ipc($2->tok.q.v[0] << 3); ic = pc >> 2; do_list(L_BINARY | L_MISC | L_NOTALL, start_line); free_tok($2); }
    |	MACRO SYMBOL opt_comma		{ mac_arglist = mac_deflist = NULL; var_arg_used = narg = 0; save_start_line = start_line; }
            opt_macro_arglist EOL
    	    BODYTEXT ENDM EOL		{ define_macro($2, narg, var_arg_used, mac_arglist, mac_deflist, $7); do_list(L_DEFINITIONS, save_start_line); }
    |	MACRO_CALL 			{ macargs = setup_macro_call_args($1, &macdef, &macargs_size); macro_arg_pos = 0; }
    	    opt_macro_call_arglist EOL	{ expand_macro(macdef, macargs); /* do_list handled inside */}
    |   NEXT_ARG EOL                    { do_list(L_DEFINITIONS, start_line); }
    |   NOLIST				{ temp_listing_flags = 0; }
    	    list_flags EOL		{ listing_flags &= ~temp_listing_flags; do_list(L_MISC, start_line); }
    |   NOLIST EOL			{ do_list(L_MISC, start_line); --listing_enable; }
    |   LIST 				{ temp_listing_flags = 0; }
	    list_flags EOL		{ listing_flags |= temp_listing_flags; do_list(L_MISC, start_line); }
    |   LIST EOL			{ ++listing_enable; do_list(L_MISC, start_line); }
    |   NODEBUG EOL			{ do_list(L_MISC, start_line); }
    |   K_DEBUG EOL			{ do_list(L_MISC, start_line); }
    |   IDENT EOL			{ do_list(L_MISC, start_line); }
    |   TITLE EOL			{ do_list(L_MISC, start_line); }
    |	DSECT SYMBOL psect_args EOL	{ do_psect($2, $3, PENTRY_TYPE_DSECT); do_list(L_BINARY | L_MISC, start_line); }
    |   HEADER EOL                      { do_save_psect(local_label_blknum); do_header(); do_list(L_BINARY | L_MISC, start_line); }
    |   ENDH EOL                        { do_rest_psect(PENTRY_TYPE_DSECT); do_list(L_BINARY | L_MISC, start_line); }
    |   LINK_ADDRESS absexpr EOL        { do_link_address($2); do_list(L_BINARY | L_MISC, start_line); }
    |   LINK_OFFSET absexpr EOL         { do_link_offset($2); do_list(L_BINARY | L_MISC, start_line); }
    |   LINK_REGION absexpr EOL         { do_link_region($2); do_list(L_BINARY | L_MISC, start_line); }
    |   LINK_SIZE absexpr EOL           { do_link_size($2); do_list(L_BINARY | L_MISC, start_line); }
    |   LINK_AFTER SYMBOL EOL           { do_link_after($2); do_list(L_BINARY | L_MISC, start_line); }
    |   LINK_FIRST EOL                  { do_link_first(); do_list(L_BINARY | L_MISC, start_line); }
    |   LINK_LAST EOL                   { do_link_last(); do_list(L_BINARY | L_MISC, start_line); }
    |   LINK_ATTR                       { temp_link_flags = 0; }
            link_flags EOL              { do_link_attr(temp_link_flags); do_list(L_BINARY | L_MISC, start_line); }
    ;

arch_flag:
        A_EV4                          { chip = EV4; errout(E_INFO| E_NO_LNUM, "hal", "Hardware mode changed to EV4\n"); }
    |   A_EV5                          { chip = EV5; errout(E_INFO| E_NO_LNUM, "hal", "Hardware mode changed to EV5\n"); }
    |   A_EV6                          { chip = EV6; errout(E_INFO| E_NO_LNUM, "hal", "Hardware mode changed to EV6\n"); }
    ;

pragma_flags:
	pragma_flag
    |	pragma_flags pragma_flag
    ;

pragma_flag:
        MAC_NONE                       { temp_pragma_flags &= ~3; temp_pragma_flags |= PRAGMA_MAC_NONE;}
    |   MAC_ONCE                       { temp_pragma_flags &= ~3; temp_pragma_flags |= PRAGMA_MAC_ONCE;}
    |   MAC_UNLIMITED                  { temp_pragma_flags &= ~3; temp_pragma_flags |= PRAGMA_MAC_UNLIMITED;}
    |   LEXER_DEBUG_ON                 { yy_flex_debug = 1; } 
    |   LEXER_DEBUG_OFF                { yy_flex_debug = 0; } 
    |   PARSER_DEBUG_ON                { yydebug = 1; } 
    |   PARSER_DEBUG_OFF               { yydebug = 0; } 
    ;

link_flags:
	link_flag
    |	link_flags link_flag
    ;

link_flag:
	OVERLAP_OK	       		{ temp_link_flags |= D_OVERLAP_OK; }
    |   DIRECTLY_AFTER	       		{ temp_link_flags |= D_DIRECTLY_AFTER; }
    ;

list_flags:
	list_flag
    |	list_flags list_flag
    ;

list_flag:
	MEB				{ temp_listing_flags |= L_BINARY; }
    |	MC				{ temp_listing_flags |= L_CALLS; }
    |	CND				{ temp_listing_flags |= L_CONDITIONALS; }
    | 	MD				{ temp_listing_flags |= L_DEFINITIONS; }
    |	ME				{ temp_listing_flags |= L_EXPANSIONS; }
    |	CMDIN				{ temp_listing_flags |= L_CMDIN; }
    |	ASSIGN				{ temp_listing_flags |= L_ASSIGN; }
    ;

string_seq:
	string_ele
    |   string_seq string_ele
    ;

string_ele:
	STRING				{ store_string($1); }
    |   '<' absexpr '>'			{ store_expr($2); }

if_condition:
	EQ absexpr			{ if_result = do_if_condition(EQ, $2, NULL, 1); qual_absexpr(EQ, if_result); }
    |	NE absexpr			{ if_result = do_if_condition(NE, $2, NULL, 1); qual_absexpr(NE, if_result); }
    |	GT absexpr			{ if_result = do_if_condition(GT, $2, NULL, 1); qual_absexpr(GT, if_result); }
    |	LT absexpr			{ if_result = do_if_condition(LT, $2, NULL, 1); qual_absexpr(LT, if_result); }
    |	GE absexpr			{ if_result = do_if_condition(GE, $2, NULL, 1); qual_absexpr(GE, if_result); }
    |	LE absexpr			{ if_result = do_if_condition(LE, $2, NULL, 1); qual_absexpr(LE, if_result); }
    |	DF SYMBOL			{ if_result = do_if_condition(DF, $2, NULL, 1); }
    |	NDF SYMBOL			{ if_result = do_if_condition(NDF,$2, NULL, 1); }
    |	B ',' opt_arg_value		{ if_result = do_if_condition(B , $3, NULL, 1); }
    |	NB ',' opt_arg_value		{ if_result = do_if_condition(NB, $3, NULL, 1); }
    |	IDN ',' opt_arg_value ',' opt_arg_value	{ if_result = do_if_condition(IDN,$3, $5, 1); }
    |	DIF ',' opt_arg_value ',' opt_arg_value	{ if_result = do_if_condition(DIF,$3, $5, 1); }
    ;

while_condition:
	EQ expr				{ $$ = chain(make_token(NULL, EQ), $2, NULL); }
    |	NE expr				{ $$ = chain(make_token(NULL, NE), $2, NULL); }
    |	GT expr				{ $$ = chain(make_token(NULL, GT), $2, NULL); }
    |	LT expr				{ $$ = chain(make_token(NULL, LT), $2, NULL); }
    |	GE expr				{ $$ = chain(make_token(NULL, GE), $2, NULL); }
    |	LE expr				{ $$ = chain(make_token(NULL, LE), $2, NULL); }
    |	DF SYMBOL			{ $$ = chain(make_token(NULL, DF), $2, NULL); }
    |	NDF SYMBOL			{ $$ = chain(make_token(NULL, NDF), $2, NULL); }
    |	B ',' opt_arg_value		{ $$ = chain(make_token(NULL, B), $3, NULL); }
    |	NB ',' opt_arg_value		{ $$ = chain(make_token(NULL, NB), $3, NULL); }
    |	IDN ',' opt_arg_value ',' opt_arg_value	{ $$ = chain(make_token(NULL, IDN), $3, $5); }
    |	DIF ',' opt_arg_value ',' opt_arg_value	{ $$ = chain(make_token(NULL, DIF), $3, $5); }
    ;

opt_comma:
	/* empty */
    |   ','
    ;

opt_arg_value:
    	/* empty */			{ $$ = NULL; }
    | 	arg_value			{ $$ = $1; }
    ;

/* mainbody
 *
 * This non-terminal simply takes in a BODYTEXT token, and passes it up a level.
 * It checks if_result first, however, and will pass up NULL if if_result was
 * false.
 */
mainbody:
	BODYTEXT			{ if (if_result) { $1->type = IF; $$ = $1; } else { $$ = NULL; free_tok_s($1); } }
    ;

/* ifsubbody_seq
 *
 * Pass up to the next higher level the chain of all the .iff, .ift, .iftf
 * BODYTEXT tokens which should be executed based on the current if_result
 * value.
 */
ifsubbody_seq:
	/* empty */			{ $$ = NULL; }
    |	ifsubbody			{ $$ = $1; }
    |   ifsubbody_seq ifsubbody		{ $$ = chain($1, $2, NULL); }
    ;

/* ifsubbody
 *
 * Get BODYTEXT tokens for .iff, .ift, or .iftf portions of a .if directive.
 * Checks if_result first, and passes up NULL if the body should not be executed.
 */
ifsubbody:
    	IFF BODYTEXT			{ if (!if_result) { $2->type = IFF; $$ = $2; } else { $$ = NULL; free_tok_s($2); } }
    |	IFT BODYTEXT			{ if (if_result) { $2->type = IFT; $$ = $2; } else { $$ = NULL; free_tok_s($2); } }
    |	IFTF BODYTEXT			{ $2->type = IFTF; $$ = $2; }
    ;

alignbound:
	absexpr			{ $$ = $1; }
    |	K_BYTE			{ $$ = $1; }
    |	K_WORD			{ $$ = $1; }
    |	K_LONG			{ $$ = $1; }
    |	K_QUAD			{ $$ = $1; }
    |	K_PAGE			{ $$ = $1; }
    ;

opt_alignfill:
	/* empty */		{ $$ = NULL; }
    |	',' absexpr		{ $$ = $2; }
    ;

mem_exprseq:
	expr				{ ++num_mem_expr; handle_memdir_expr($1, mem_type); }
    |	mem_exprseq ',' expr		{ ++num_mem_expr; handle_memdir_expr($3, mem_type); }
    ;

opt_absexpr:
    	/* empty */		{ $$ = NULL; }
    |	absexpr			{ $$ = $1; }
    ;

psect_args:
	/* empty */		{ $$ = NULL; }
    |	',' alignbound		{ $$ = $2; }
    ;

opt_macro_arglist:
	/* empty */
    |	macro_arglist
    ;

macro_arglist:
	macro_arg
    |	macro_arglist ',' macro_arg
    ;

macro_arg:
    	SYMBOL arg_default		{ $1->type = MACRO_ARG_NORMAL;
                                          mac_arglist = chain(mac_arglist, $1, NULL);
                                          mac_deflist = chain(mac_deflist, $2, NULL); ++narg;
                                        }
    |	'~' SYMBOL arg_default		{ $2->type = MACRO_ARG_KWONLY;
                                          mac_arglist = chain(mac_arglist, $2, NULL);
                                          mac_deflist = chain(mac_deflist, $3, NULL); ++narg;
                                        }
    |   DOTDOTDOT SYMBOL arg_default    { $2->type = MACRO_ARG_NORMAL;
                                          mac_arglist = chain(mac_arglist, $2, NULL); 
                                          mac_deflist = chain(mac_deflist, $3, NULL); var_arg_used=++narg;
                                        }
    |   DOTDOTDOT arg_default    	{ $1->type = MACRO_ARG_NORMAL;
                                          mac_arglist = chain(mac_arglist, $1, NULL); 
                                          mac_deflist = chain(mac_deflist, $2, NULL); var_arg_used=++narg;
                                        }
    |	'?' SYMBOL			{ $2->type = MACRO_ARG_NORMAL;
                                          mac_arglist = chain(mac_arglist, $2, NULL); 
                                          mac_deflist = chain(mac_deflist, $1, NULL); ++narg;
                                        }
    ;

arg_default:
	/* empty */			{ $$ = make_token("", ARG_VALUE); }
    |	'=' ARG_VALUE			{ $$ = $2; }
    |	'=' SYMBOL			{ $2->type = ARG_VALUE; $$ = $2; }
    ;

opt_macro_call_arglist:
        macro_call_arg
    |   opt_macro_call_arglist ',' macro_call_arg
    ;

arg_value:
        SYMBOL				{ $$ = $1; }
    |	ARG_VALUE			{ $$ = $1; }
    ;

macro_call_arg:
        /* empty */			{ ++macro_arg_pos; }
    |    '\\' SYMBOL                    { int reloc_used;
					  TOKEN_LIST *tmp1;
                                          TOKEN_LIST *tmp = eval_expr(tmp1 = get_symbol_value(copy_expr($2),0/*is_while*/), &reloc_used);
                                          free_tok_s(tmp1);
                                          free_tok($1);
                                          if (reloc_used && (tmp != NULL))
                                             save_absexpr($2, tmp);
                                          else
                                             free_tok_s($2);
                                          if (!assign_macro_arg(macdef, &macargs, &macargs_size, NULL, convert_num2arg(tmp), &macro_arg_pos)) YYERROR;
                                        }
    |	arg_value		        { if (!assign_macro_arg(macdef, &macargs, &macargs_size, NULL, $1, &macro_arg_pos)) YYERROR;}
    |	SYMBOL '=' '\\' SYMBOL	        { int reloc_used;
					  TOKEN_LIST *tmp1;
                                          TOKEN_LIST *tmp = eval_expr(tmp1 = get_symbol_value(copy_expr($4),0/*is_while*/), &reloc_used);
                                          free_tok_s(tmp1);
                                          free_tok($3);
                                          if (reloc_used && (tmp != NULL))
                                             save_absexpr($4, tmp);
                                          else
                                             free_tok_s($4);
                                          if (!assign_macro_arg(macdef, &macargs, &macargs_size, $1, convert_num2arg(tmp), NULL)) YYERROR;
                                        } 
    |	SYMBOL '=' opt_arg_value	{ if (!assign_macro_arg(macdef, &macargs, &macargs_size, $1, $3, NULL)) YYERROR; }
    ;
	
%%

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
