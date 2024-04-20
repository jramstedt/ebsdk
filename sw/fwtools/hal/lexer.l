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
#ifdef __STDC__
#include <stdlib.h>
#endif
#include <string.h>
#include <ctype.h>
#include <assert.h>

#define NO_LEXYY
#include "hal.h"
#undef NO_LEXYY
#include "ev5.h"
#include "operator.h"

#undef BEGIN
#define BEGIN(s) { yy_start = 1 + 2 * (s); last_begin_state = s; }
static int last_begin_state = INITIAL;

/*
 * Tells flex to use get_input() instead of its default.
 */
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size)	get_input(buf,&(result),max_size);

/*
 * Start line of a conditionally executed block of code.
 */
int start_line;

/*
 * Register number of the IREG or FREG tokens.
 */
int regnum;

/*
 * Temporary storage for tokens which require extended lexing.
 */
int line_size;
char *line, *lptr;

/*
 * Macro preprocessing.
 */
int macro_line_size;
char* macro_line;  
char* macro_line_ptr;
static int preprocessing_macro = 0;
static int prep_count;
static char** prep_str;
static char* prep_string1;
static char* prep_string2;
static int free_string1 = 0;
static int free_string2 = 0;
static TOKEN_LIST** prep_symb;
static TOKEN_LIST* prep_symbol1;
static TOKEN_LIST* prep_symbol2;
static TOKEN_LIST* had_null = NULL;
static int if_macro_cond = 0;
static int repeat_level = 0, if_level = 0, angle_level = 0, macro_level = 0, cmdin_level = 0, while_level = 0;
static int needed_prep = 0;

/*
 *
 */
int listing_line_size;
char* listing_line;  
char* listing_line_ptr;

int yy_flex_debug = 0;

/*
 * Functions which are prototyped here because they shouldn't be called
 * anywhere else.
 */
static void get_input PROTO((char *buf, int *result, int max_size));
static char* get_macro_arg PROTO((char* text));
static void clear_line PROTO((void));
static void add_to_line PROTO((char *text));
static void add_to_macro_line PROTO((char *text));
static void add_num_to_macro_line PROTO((int num));
static int prep_do_locate PROTO((char* str1, char* str2, TOKEN_LIST* symb));
static void prep_do_extract PROTO((TOKEN_LIST* symb1, TOKEN_LIST* symb2, char* str));

%}

%s kill_to_eol

%s listing_dir link_attr_dir pragma_dir arch_dir

%s get_delim_string ascii_op
%x get_string get_asciiop_string

%s assign

%s expr_s operands align_expr 

%x msg_expr

%s if_direct
%x if_body
%x iif_body
%x if_got_dot

%x repeat_body repeat_got_dot

%x while_body while_got_dot

%x cmdin_body cmdin_got_dot

%s psect_name psect_attr save_psect_attr

%s macro_name macro_args_eat_ws
%x macro_args macro_arg_angle
%x macro_body macro_got_dot

%x prep
%x prep_section prep_section_close
%x prep_length prep_length_close
%x prep_locate_got_1st prep_locate_got_2nd prep_locate_close
%x prep_extract_got_1st prep_extract_got_2nd prep_extract_close
%x prep_count_string prep_get_arg_or_str prep_get_symb prep_get_string

WS	[ \t\f]
REGNUM	[0-9]{1,2}
SYMBOL	([a-zA-Z$_.][a-zA-Z0-9$_./]*|[0-9]+[a-zA-Z$_.][a-zA-Z0-9$_./]+|[0-9]+[a-zA-Z$_.])
%%
	static int string_delim, psect_attr_error;
	static int expr_return = INITIAL, expr_return_comma = INITIAL, expr_return_gtr = expr_s;
        static int psect_attr_name = INITIAL, ascii_op_return = INITIAL;
	static int macro_arg_comma, macro_arg_comma_ign, macro_arg_nl;
        static int prep_string_rtn, prep_symb_rtn;
	static int had_lcont = 0;

 /*
  * Inhale text to end of line.
  *
  * Default rule for \n will put us back in INITIAL.
  */
<kill_to_eol>[^\n]*		;

 /*
  * Mini-scanner for delimited strings.
  * 
  * Uses "get_string" to get the actual string.
  *
  * Note: default handling of \n is fine here.
  */
<get_delim_string>"<"		{ expr_return = expr_return_comma = expr_return_gtr = get_delim_string;
                                  angle_level = 1;
                                  BEGIN(expr_s); return yytext[0]; 
                                }
<get_delim_string>"\n"		{ expr_return_gtr = expr_s; BEGIN(INITIAL); return EOL; }
<get_delim_string>[^ \t;=<\n]	{ string_delim = yytext[0]; add_to_line(yytext); BEGIN(get_string); }

 /*
  * Mini-scanner for the string part of delimited strings.
  *
  * This is an exclusive state.  Line continuation is not permitted within a 
  * delimited string.
  */
<get_string>.			{ add_to_line(yytext);
                                  if (yytext[0] == string_delim) {
                                    yylval = make_token(line, STRING);
                                    clear_line();
                                    BEGIN(get_delim_string);
                                    return STRING;
                                  }
                                }
<get_string>\n			{ errout(E_ERR, "yylex", "string did not end with delimiter.\n"); BEGIN(INITIAL); return EOL; }

 /*
  * "if_direct" is entered for .if and .iif directives.
  */
<if_direct>("eq"|"equal"){WS}*(","{WS}*)?		{ BEGIN(expr_s); return EQ; }
<if_direct>("ne"|"not_equal"){WS}*(","{WS}*)?		{ BEGIN(expr_s); return NE; }
<if_direct>("gt"|"greater"){WS}*(","{WS}*)?		{ BEGIN(expr_s); return GT; }
<if_direct>("ge"|"greater_equal"){WS}*(","{WS}*)? 	{ BEGIN(expr_s); return GE; }
<if_direct>("lt"|"less_than"){WS}*(","{WS}*)?		{ BEGIN(expr_s); return LT; }
<if_direct>("le"|"less_equal"){WS}*(","{WS}*)?		{ BEGIN(expr_s); return LE; }
<if_direct>("df"|"defined"){WS}*(","{WS}*)?		{ BEGIN(expr_s); return DF; }
<if_direct>("ndf"|"not_defined"){WS}*(","{WS}*)? 	{ BEGIN(expr_s); return NDF; }
<if_direct>("b"|"blank")				{ BEGIN(macro_args); return B; }
<if_direct>("nb"|"not_blank")				{ BEGIN(macro_args); return NB; }
<if_direct>("idn"|"identical")				{ macro_arg_comma_ign += 1; BEGIN(macro_args); return IDN; }
<if_direct>("dif"|"different")				{ macro_arg_comma_ign += 1; BEGIN(macro_args); return DIF; }

 /*
  * Exclusive state to grab all the text that makes a body of
  * an .if directive or subdirective.
  *
  * Line continuation isn't bothered with until processing the 
  * body itself.
  */
<if_body>^{WS}*({SYMBOL}:{1,2}{WS}*)?/".if"			 { add_to_line(yytext); BEGIN(if_got_dot); }
<if_body>^{WS}*({SYMBOL}:{1,2}{WS}*)?/".if_false"	 	 { add_to_line(yytext); BEGIN(if_got_dot);
                                                                   if (if_level == 1) {
							               yylval = make_token(line, BODYTEXT);
							               clear_line();
                                                                       return BODYTEXT;
                                                                   }
                                                                 } 
<if_body>^{WS}*({SYMBOL}:{1,2}{WS}*)?/".if_true"		 { add_to_line(yytext); BEGIN(if_got_dot);
                                                                   if (if_level == 1) {
							               yylval = make_token(line, BODYTEXT);
							               clear_line();
                                                                       return BODYTEXT;
                                                                   }
                                                                 } 
<if_body>^{WS}*({SYMBOL}:{1,2}{WS}*)?/".if_true_false"	 { add_to_line(yytext); BEGIN(if_got_dot);
                                                                   if (if_level == 1) {
							               yylval = make_token(line, BODYTEXT);
							               clear_line();
                                                                       return BODYTEXT;
                                                                   }
                                                                 } 
<if_body>^{WS}*({SYMBOL}:{1,2}{WS}*)?/".iff"			 { add_to_line(yytext); BEGIN(if_got_dot);
                                                                   if (if_level == 1) {
							               yylval = make_token(line, BODYTEXT);
							               clear_line();
                                                                       return BODYTEXT;
                                                                   }
                                                                 } 
<if_body>^{WS}*({SYMBOL}:{1,2}{WS}*)?/".ift"			 { add_to_line(yytext); BEGIN(if_got_dot);
                                                                   if (if_level == 1) {
							               yylval = make_token(line, BODYTEXT);
							               clear_line();
                                                                       return BODYTEXT;
                                                                   }
                                                                 } 
<if_body>^{WS}*({SYMBOL}:{1,2}{WS}*)?/".iftf"		 { add_to_line(yytext); BEGIN(if_got_dot);
                                                                   if (if_level == 1) {
							               yylval = make_token(line, BODYTEXT);
							               clear_line();
                                                                       return BODYTEXT;
                                                                   }
                                                                 } 
<if_body>^{WS}*({SYMBOL}:{1,2}{WS}*)?/".repeat"		 { add_to_line(yytext); repeat_level++; /* shut down pre-processing */}
<if_body>^{WS}*({SYMBOL}:{1,2}{WS}*)?/".endr"		 { add_to_line(yytext);
                                                                   if (repeat_level==0)
                                                                     errout(E_ERR, "yylex", "A .endr occured without a matching .repeat within a if body.\n");
                                                                   else
                                                                       repeat_level--;
                                                                 }
<if_body>^{WS}*({SYMBOL}:{1,2}{WS}*)?/".while"		 { add_to_line(yytext); while_level++; /* shut down pre-processing */}
<if_body>^{WS}*({SYMBOL}:{1,2}{WS}*)?/".endw"		 { add_to_line(yytext);
                                                                   if (repeat_level==0)
                                                                     errout(E_ERR, "yylex", "A .endw occured without a matching .while within a if body.\n");
                                                                   else
                                                                       while_level--;
                                                                 }
<if_body>^{WS}*({SYMBOL}:{1,2}{WS}*)?/".endc"		 { add_to_line(yytext); BEGIN(if_got_dot);
                                                                   if (if_level == 1) {
							               yylval = make_token(line, BODYTEXT);
							               clear_line();
                                                                       return BODYTEXT;
                                                                   }
                                                        	 }
<if_body>[^.\n]+						 add_to_line(yytext);
<if_body>"."							 add_to_line(yytext);
<if_body>"\n"							 add_to_line(yytext);

 /*
  * Exclusive substate of "if_body" to determine what needs to be 
  * done with the subdirectives of a .if statement.
  *
  * Line continuation is irrelevant here.
  */
<if_got_dot>(".if_true_false"|".iftf")[^\n]*\n	{ if (if_level == 1) { 
                                                      BEGIN(if_body); 
						      start_line = cur_ifile->lineno + cur_base_lineno;
                                                      return IFTF;
                                                  } else {
	                                              add_to_line(yytext);
                                                      BEGIN(if_body);
                                                  }
                                                }
<if_got_dot>(".if_false"|".iff")[^\n]*\n	{ if (if_level == 1) { 
                                                      BEGIN(if_body); 
					   	      start_line = cur_ifile->lineno + cur_base_lineno;
                                                      return IFF;
                                                  } else {
	                                              add_to_line(yytext);
                                                      BEGIN(if_body);
                                                  }
                                                }
<if_got_dot>(".if_true"|".ift")[^\n]*\n		{ if (if_level == 1) { 
                                                      BEGIN(if_body); 
						      start_line = cur_ifile->lineno + cur_base_lineno;
                                                      return IFT;
                                                  } else {
	                                              add_to_line(yytext);
                                                      BEGIN(if_body);
                                                  }
                                                }
<if_got_dot>".if"[^\n]*\n			{ ++if_level; add_to_line(yytext); BEGIN(if_body); }
<if_got_dot>".endc"[^\n]*			{ if (--if_level == 0) { 
                                                      BEGIN(INITIAL); 
                                                      return ENDC;
                                                  } else {
	                                              add_to_line(yytext);
                                                      BEGIN(if_body);
                                                  }
                                                }

 /* iif_body
  *
  * Return a BODYTEXT macro for the remainder of the line beyond the iif condition
  * and arguments.
  *
  * "-" is separated from other characters to allow the line continuation
  * to catch the token it needs.
  */
<iif_body>"-"			{ add_to_line(yytext); }
<iif_body>[^-\n]*		{ add_to_line(yytext); }
 /*
  * Note: this uses [...]* instead of [...]+ because it needs to handle the case
  * when there is no statement -- a legal (but stupid) thing to do.
  */
<iif_body>\n			{ add_to_line(yytext); yylval = make_token(line, BODYTEXT);
				  clear_line();
                                  BEGIN(INITIAL);
                                  unput('\n');
				  return BODYTEXT;
				}

 /*
  * Exclusive state to grab all the text that makes a body of
  * a .repeat directive.
  *
  * Line continuation isn't bothered with until processing the 
  * body itself.
  */
<repeat_body>[^.\n]+						 add_to_line(yytext);
<repeat_body>^{WS}*({SYMBOL}:{1,2}{WS}*)?/".repeat"		 { add_to_line(yytext); BEGIN(repeat_got_dot); }
<repeat_body>^{WS}*({SYMBOL}:{1,2}{WS}*)?/".endr"		 { add_to_line(yytext); BEGIN(repeat_got_dot);
                                                                   if (repeat_level == 1) {
							               yylval = make_token(line, BODYTEXT);
							               clear_line();
                                                                       return BODYTEXT;
                                                                   }
                                                        	 }
<repeat_body>"."						 add_to_line(yytext);
<repeat_body>"\n"						 add_to_line(yytext);

 /*
  * Exclusive substate of "repeat_body" to determine what needs to be 
  * done with the directives while in a .repeat statement.
  *
  * Line continuation is irrelevant here.
  */
<repeat_got_dot>".repeat"			{ ++repeat_level; add_to_line(yytext); BEGIN(repeat_body); }
<repeat_got_dot>".endr"				{ if (--repeat_level == 0) { 
                                                      BEGIN(INITIAL); 
                                                      return ENDR;
                                                  } else {
	                                              add_to_line(yytext);
                                                      BEGIN(repeat_body);
                                                  }
                                                }

 /*
  * Exclusive state to grab all the text that makes a body of
  * a .while directive.
  *
  * Line continuation isn't bothered with until processing the 
  * body itself.
  */
<while_body>[^.\n]+						 add_to_line(yytext);
<while_body>^{WS}*({SYMBOL}:{1,2}{WS}*)?/".while"		 { add_to_line(yytext); BEGIN(while_got_dot); }
<while_body>^{WS}*({SYMBOL}:{1,2}{WS}*)?/".endw"		 { add_to_line(yytext); BEGIN(while_got_dot);
                                                                   if (while_level == 1) {
							               yylval = make_token(line, BODYTEXT);
							               clear_line();
                                                                       return BODYTEXT;
                                                                   }
                                                        	 }
<while_body>"."						         add_to_line(yytext);
<while_body>"\n"						 add_to_line(yytext);

 /*
  * Exclusive substate of "while_body" to determine what needs to be 
  * done with the directives while in a .while statement.
  *
  * Line continuation is irrelevant here.
  */
<while_got_dot>".while"			       { ++while_level; add_to_line(yytext); BEGIN(while_body); }
<while_got_dot>".endw"				{ if (--while_level == 0) { 
                                                      BEGIN(INITIAL); 
                                                      return ENDW;
                                                  } else {
	                                              add_to_line(yytext);
                                                      BEGIN(while_body);
                                                  }
                                                }

 /*
  * Exclusive state to grab all the text that makes a body of
  * a .cmd_input directive.
  *
  * Line continuation isn't bothered with until processing the 
  * body itself.
  */
<cmdin_body>[^.\n]+					        add_to_line(yytext);
<cmdin_body>^{WS}*({SYMBOL}:{1,2}{WS}*)?/".cmd_input"	        { add_to_line(yytext); BEGIN(cmdin_got_dot); }
<cmdin_body>^{WS}*({SYMBOL}:{1,2}{WS}*)?/".endi"	        { add_to_line(yytext); BEGIN(cmdin_got_dot);
                                                                  if (cmdin_level == 1) {
							             yylval = make_token(line, BODYTEXT);
							             clear_line();
                                                                     return BODYTEXT;
                                                                  }
                                                                }
<cmdin_body>"."						        add_to_line(yytext);
<cmdin_body>"\n"					        add_to_line(yytext);

 /*
  * Exclusive substate of "cmdin_body" to determine what needs to be 
  * done with the directives while in a .cmd_input statement.
  *
  * Line continuation is irrelevant here.
  */
<cmdin_got_dot>".cmd_input"		        { ++cmdin_level; add_to_line(yytext); BEGIN(cmdin_body); }
<cmdin_got_dot>".endi"			        { if (--cmdin_level == 0) { 
                                                      BEGIN(INITIAL); 
                                                      return ENDI;
                                                  } else {
	                                              add_to_line(yytext);
                                                      BEGIN(cmdin_body);
                                                  }
                                                }

 /*
  * Scans the macro name, then transfers to formal argument scanning.
  */
<macro_name>{SYMBOL}		{ yylval = make_token(yytext, SYMBOL); BEGIN(macro_args); return SYMBOL; }

<macro_args_eat_ws>{WS}+	BEGIN(macro_args);
<macro_args_eat_ws>\n		{ BEGIN(macro_arg_nl); return EOL; }
<macro_args_eat_ws>.		{ unput(yytext[0]); BEGIN(macro_args); }

<macro_args>\.{3}({SYMBOL})?	{ had_lcont = 0; yylval = make_token(&yytext[3], SYMBOL); return DOTDOTDOT; }
<macro_args>{SYMBOL}		{ had_lcont = 0; yylval = make_token(yytext, SYMBOL); return SYMBOL; }
<macro_args>"%null%"		{ had_lcont = 0; yylval = make_token("", ARG_VALUE); return ARG_VALUE; }
<macro_args>[^ %,;~?\\=<\t\f\n]* { had_lcont = 0; yylval = make_token(yytext, ARG_VALUE); return ARG_VALUE; }
<macro_args>"<"			{ had_lcont = 0; BEGIN(macro_arg_angle); angle_level = 1; clear_line(); }
<macro_args>{WS}*"\n"		{ if (macro_arg_nl != macro_args)
                                      if_macro_cond = 0;
                                  if (macro_arg_nl == macro_body)
                                     start_line = cur_ifile->lineno + cur_base_lineno;
                                  had_lcont = 0; BEGIN(macro_arg_nl);
                                  return EOL;
                                }
<macro_args>"\\"		{ had_lcont = 0; yylval = make_token(NULL, '\\'); return '\\'; }
<macro_args>"?"			{ had_lcont = 0; yylval = make_token(NULL, '?'); return '?'; }
<macro_args>"~"		        { had_lcont = 0; yylval = make_token(NULL, '~'); return '~'; }
<macro_args>{WS}+		{ if (macro_arg_comma_ign) --macro_arg_comma_ign; if (!had_lcont) return ','; }
<macro_args>{WS}*"-"{WS}*(;[^\n]*)?\n{WS}*  { if (macro_arg_comma_ign) --macro_arg_comma_ign; if (!had_lcont) { had_lcont = 1; return ','; } }
<macro_args>{WS}*","{WS}*"-"{WS}*(;[^\n]*)?\n{WS}*  { if (macro_arg_comma_ign) 
                                                         --macro_arg_comma_ign; 
                                                      else {
                                                          if (macro_arg_comma != macro_args)
                                                              if_macro_cond = 0;
                                                          BEGIN(macro_arg_comma);
                                                      }
                                                      if (!had_lcont) { 
                                                          had_lcont = 1; 
                                                          return ','; 
                                                      } else
                                                          had_lcont = 0;
                                                    }
<macro_args>{WS}*"-"{WS}*(;[^\n]*)?\n{WS}*","{WS}*  { if (macro_arg_comma_ign) --macro_arg_comma_ign; if (!had_lcont) { had_lcont = 1; return ','; } }
<macro_args>{WS}*","{WS}*	{ if (macro_arg_comma_ign)
                                      --macro_arg_comma_ign;
                                  else {
                                      if (macro_arg_comma != macro_args)
                                          if_macro_cond = 0;
                                      BEGIN(macro_arg_comma);
                                  }
				  if (!had_lcont)
                                      return ',';
				  else
				      had_lcont = 0;
                                }
<macro_args>{WS}*"="{WS}*	{ had_lcont = 0; return '='; }
<macro_args>{WS}*;[^\n]*	; /* comments */

<macro_arg_angle>"-"{WS}*(;[^\n]*)?\n	; /* line continuation with comment */
<macro_arg_angle>"%null%"	;		/* null macro argument */
<macro_arg_angle>[^-<>]+	{ add_to_line(yytext); }
<macro_arg_angle>"-"		{ add_to_line(yytext); }
<macro_arg_angle>"<"		{ add_to_line(yytext); ++angle_level; }
<macro_arg_angle>">"		{ if (--angle_level == 0) {
                                      yylval = make_token(line, ARG_VALUE);
                                      clear_line();
                                      BEGIN(macro_args);
                                      return ARG_VALUE;
                                  } else
                                      add_to_line(yytext);
                                }

<macro_body>[^.\n]+					add_to_line(yytext);
<macro_body>^{WS}*({SYMBOL}:{1,2}{WS}*)?/".macro"	{ add_to_line(yytext); BEGIN(macro_got_dot); }
<macro_body>^{WS}*({SYMBOL}:{1,2}{WS}*)?/".endm"	{ add_to_line(yytext); BEGIN(macro_got_dot);
                                                          if (macro_level == 1) {
							      yylval = make_token(line, BODYTEXT);
							      clear_line();
                                                              return BODYTEXT;
                                                          }
                                                        }
<macro_body>"."						add_to_line(yytext);
<macro_body>"\n"				        add_to_line(yytext); 

<macro_got_dot>".macro"				{ ++macro_level; add_to_line(yytext); BEGIN(macro_body); }
<macro_got_dot>".endm"				{ if (--macro_level == 0) { 
                                                      BEGIN(kill_to_eol); 
                                                      return ENDM;
                                                  } else {
	                                              add_to_line(yytext);
                                                      BEGIN(macro_body);
                                                  }
                                                }


 /*
  * Scans the psect name, then transfers to the attribute scanning.
  */
<psect_name>{SYMBOL}		{ yylval = make_token(yytext, SYMBOL); BEGIN(psect_attr_name); return SYMBOL; }

 /*
  * Scans for the psect attributes.
  *
  * Only alignment attribute is legal, so these others defined in macro32 print a warning.
  *
  * The alignment keywords are defined below with the align_expr state.  An absolute
  * expression is also legal for specifying alignment.
  */
<psect_attr>,{WS}*abs		|
<psect_attr>,{WS}*con			|
<psect_attr>,{WS}*exe			|
<psect_attr>,{WS}*gbl			|
<psect_attr>,{WS}*lcl			|
<psect_attr>,{WS}*lib			|
<psect_attr>,{WS}*noexe		|
<psect_attr>,{WS}*nopic		|
<psect_attr>,{WS}*nord		|
<psect_attr>,{WS}*noshr		|
<psect_attr>,{WS}*nowrt		|
<psect_attr>,{WS}*ovr			|
<psect_attr>,{WS}*pic			|
<psect_attr>,{WS}*rd			|
<psect_attr>,{WS}*rel			|
<psect_attr>,{WS}*shr			|
<psect_attr>,{WS}*usr			|
<psect_attr>,{WS}*vec			|
<psect_attr>,{WS}*wrt			{ if (!psect_attr_error)
                                      errout(E_WARN, "lexer", "unsupported attribute in .psect directive (only alignment supported).\n");
                                  psect_attr_error = 1;
                                }

<save_psect_attr>"local_block"          return LOCAL_BLOCK;

 /*
  * The .align directive has special keywords to specify the alignment.
  *
  * These are used by the .psect directive also.
  */
<align_expr,psect_attr>byte|word	{ errout(E_WARN, "lexer", "byte and word alignments not supported, using long.\n");
                                          yylval = make_value_token(0, 2); return K_LONG; }
<align_expr,psect_attr>long		{ yylval = make_value_token(0, 2); return K_LONG; }
<align_expr,psect_attr>quad		{ yylval = make_value_token(0, 3); return K_QUAD; }
<align_expr,psect_attr>page		{ yylval = make_value_token(0, page_size); return K_PAGE; }

<listing_dir>"binary"|"meb"		return MEB;
<listing_dir>"calls"|"mc"		return MC;
<listing_dir>"conditionals"|"cnd"	return CND;
<listing_dir>"definitions"|"md"		return MD;
<listing_dir>"expansions"|"me"		return ME;
<listing_dir>"command_input"|"cmdin"	return CMDIN;
<listing_dir>"assignments"|"assign"	return ASSIGN;
<pragma_dir,link_attr_dir,listing_dir>{WS}*","{WS}*		;
<link_attr_dir>"overlap_ok"             return OVERLAP_OK;
<link_attr_dir>"directly_after"         return DIRECTLY_AFTER;
<pragma_dir>"macro_none"	        return MAC_NONE;
<pragma_dir>"macro_once"		return MAC_ONCE;
<pragma_dir>"macro_unlimited"	        return MAC_UNLIMITED;
<pragma_dir>"lexer_debug_on"	        return LEXER_DEBUG_ON;
<pragma_dir>"lexer_debug_off"	        return LEXER_DEBUG_OFF;
<pragma_dir>"parser_debug_on"	        return PARSER_DEBUG_ON;
<pragma_dir>"parser_debug_off"	        return PARSER_DEBUG_OFF;
<arch_dir>"ev4"                         return A_EV4;
<arch_dir>"ev5"                         return A_EV5;
<arch_dir>"ev6"                         return A_EV6;

 /*
  * Tokens only used in operands to instructions, cont'd.
  *
  * The register tokens.
  */
<operands>"r"{REGNUM}		{ regnum = atoi(&yytext[1]); return(IREG); }
<operands>"f"{REGNUM}		{ regnum = atoi(&yytext[1]); return(FREG); }

 /*
  * Tokens only used in operands to instructions, cont'd.
  *
  * Miscellaneous syntax tokens.
  *
  * Note: default "\n" is fine.
  */
<operands>"("|")"		{ return yytext[0]; }
<operands>","			{ return ','; }
<operands>"#"			{ return '#'; }

<operands,align_expr,psect_attr,msg_expr>"<"|">"	return yytext[0];

 /*
  * Tokens common to both expressions and instruction operands.
  */
<expr_s,operands,align_expr,psect_attr,msg_expr>"^x"[0-9a-fA-F]+	{ yylval = cvtradix(&yytext[2], 16, NUMBER); return NUMBER; }
<expr_s,operands,align_expr,psect_attr,msg_expr>"^a"		{ ascii_op_return = last_begin_state; BEGIN(ascii_op); }
<expr_s,operands,align_expr,psect_attr,msg_expr>"^b"[0-1]+	{ yylval = cvtradix(&yytext[2], 2, NUMBER); return NUMBER; }
<expr_s,operands,align_expr,psect_attr,msg_expr>"^o"[0-7]+	{ yylval = cvtradix(&yytext[2], 8, NUMBER); return NUMBER; }
<expr_s,operands,align_expr,psect_attr,msg_expr>"^d"[0-9]+	{ yylval = cvtradix(&yytext[2], 10, NUMBER); return NUMBER; }
<expr_s,operands,align_expr,psect_attr,msg_expr>"^f""-"?[0-9]*("."[0-9]*)?	{ errout(E_WARN, "yylex", "^f operator not supported, using zero.\n"); yylval = cvtradix("0", 10, NUMBER); return NUMBER; }
<expr_s,operands,align_expr,psect_attr,msg_expr>[0-9]+		{ yylval = cvtradix(yytext, 10, NUMBER); return NUMBER; }
<expr_s,operands,align_expr,psect_attr,msg_expr>"^c"		{ yylval = make_token(NULL, NOT); return NOT; }
<expr_s,operands,align_expr,psect_attr,msg_expr>"^n"		{ yylval = make_token(NULL, LOG_NOT); return LOG_NOT; }
<expr_s,operands,align_expr,psect_attr,msg_expr>"&&"		{ yylval = make_token(NULL, LOG_AND); return LOG_AND; }
<expr_s,operands,align_expr,psect_attr,msg_expr>"!!"		{ yylval = make_token(NULL, LOG_OR); return LOG_OR; }
 /* this DOT rule must precede the generic symbol rule or it won't get used. */
<expr_s,operands,align_expr,psect_attr,msg_expr>"."		{ yylval = make_token(NULL, DOT); return DOT; }
<expr_s,operands,align_expr,psect_attr,msg_expr>[0-9]+"$"		{ yylval = make_token(yytext, LOCAL_LABEL); return LOCAL_LABEL; }
<expr_s,operands,align_expr,psect_attr,msg_expr>{SYMBOL}		{ yylval = make_token(yytext, SYMBOL); return SYMBOL; }
<expr_s,operands,align_expr,psect_attr,msg_expr>("+"|"-"|"&"|"!"|"\\"|"@"|"*"|"/")	{ yylval = make_token(NULL, yytext[0]); return yytext[0]; }

<ascii_op>[^ \t;]	{ string_delim = yytext[0]; add_to_line(yytext); BEGIN(get_asciiop_string); }

<get_asciiop_string>.		{ add_to_line(yytext);
                                  if (yytext[0] == string_delim) {
                                    yylval = cvtasciiop(line);
                                    clear_line();
                                    BEGIN(ascii_op_return);
                                    return NUMBER;
                                  }
                                }
<get_asciiop_string>\n		{ errout(E_ERR, "yylex", "ascii operator did not end with delimiter.\n"); BEGIN(INITIAL); return EOL; }

 /*
  * Tokens only used for expr sessions.
  */
<expr_s,align_expr,psect_attr,msg_expr>","			{ if_macro_cond = 0; BEGIN(expr_return_comma); return ','; }
<expr_s,align_expr,psect_attr,msg_expr>"\n"			{ if_macro_cond = 0; BEGIN(expr_return); return EOL; }

<expr_s>"<"							{ ++angle_level; return yytext[0]; }
<expr_s>">"							{ --angle_level;
                                                                  if (!angle_level)
                                                                      BEGIN(expr_return_gtr);
                                                                  return yytext[0];
                                                                }
<expr_s>"%null%"							;

 /*
  * Message printing directives are the only things which need to see
  * the source file comments.
  */
<msg_expr>;[^\n]*		{ yylval = make_token(yytext, COMMENT); return COMMENT; }
<msg_expr>{WS}*"-"{WS}*\n{WS}*	; /* line continuation */
<msg_expr>{WS}*			; /* whitespace */
<msg_expr>"%null%"		;

 /*
  * Whitespace will be picked up by the main whitespace rule.
  */
<assign>"="|"=="		{ expr_return = INITIAL; expr_return_comma = expr_return_gtr = expr_s; BEGIN(expr_s); return '='; }

 /* THESE MUST BE AFTER ALL STATE ACTIONS
  *
  * these definitions are the default ones, but they must be able to be
  * overridden by actions in the mini scanners above.
  */
<iif_body>{WS}*"-"{WS}*(;[^\n]*)?\n{WS}*	; /* line continuation with comment */
{WS}*"-"{WS}*(;[^\n]*)?\n{WS}*	; /* line continuation with comment */
{WS}*;[^\n]*			; /* comments */
{WS}*				; /* whitespace */
"\n"				{ BEGIN(INITIAL); return EOL; }

 /*
  * Must be first assignment parsed, so that it overrides the symbol
  * assignment below.
  */ 
"."{WS}*/"="			{ start_line = cur_ifile->lineno + cur_base_lineno; BEGIN(assign); return DOT; }

{SYMBOL}{WS}*/"="		{ start_line = cur_ifile->lineno + cur_base_lineno; BEGIN(assign); yylval = make_token(yytext, SYMBOL); return SYMBOL; }

[0-9]+"$"/":"			{ yylval = make_token(yytext, LOCAL_LABEL); return LOCAL_LABEL; }

{SYMBOL}/":"			{ yylval = make_token(yytext, SYMBOL); return SYMBOL; }

":"				{ return yytext[0]; }

"%null%"			;

{SYMBOL}		        { OPID oid = find_operator(yytext, 0/*return_name*/);
                                  if (oid == ID_NULL) {
                                      errout(E_ERR, "lexer", "illegal token (\"%s\") in operator field, ignoring line.\n", yytext);
                                      BEGIN(kill_to_eol);
                                  } else {
                                      start_line = cur_ifile->lineno + cur_base_lineno;

                                      switch (operator[oid].type) {
                                      case INS_MEM:
                                      case INS_IOP:
                                      case INS_JSR:
                                      case INS_BRN:
                                      case INS_FOP:
                                      case INS_FP2:
                                      case INS_MIS:
                                      case INS_M1A:
                                      case INS_M1B:
				      case INS_RPCC:
				      case INS_FPCR:
				      case INS_UNOP:
				      case INS_SEXT:
				      case INS_IP2:
				      case INS_IP3:
				      case INS_FTI:
				      case INS_LSB:
				      case INS_SSB:
				      case INS_ITF:
				      case INS_CALLPAL:
                                      case INS_IPR4:
                                      case INS_HWM4:
                                      case INS_IPR5:
                                      case INS_HWM5:
                                      case INS_REI6:
                                      case INS_MTI6:
                                      case INS_MFI6:
                                      case INS_HWM6:
                                      case INS_1REG:
                                          opcode = operator[oid].value;
                                          BEGIN(operands);
                                          break;
                                      case ENDP:
                                          break;
                                      case INCLUDE:
                                          BEGIN(get_delim_string);
                                          break;
                                      case INCLUDE_DIR:
                                          BEGIN(get_delim_string);
                                          break;
                                      case MINCLUDE:
                                          BEGIN(get_delim_string);
                                          break;
                                      case BREAK:
                                      case CONTINUE:
                                          {
                                             INPUT_FILE *f = cur_ifile;
                                             while (f && (f->type != F_MACRO || operator[oid].type==CONTINUE) &&
                                                    f->string_type != REPEAT && f->string_type != WHILE) {
                                               f->abort = 1;
                                               f = f->next;
                                             }
                                             if (f) {
                                               if (f->preprocess_me || f->type == F_MACRO) {
                                                 INPUT_FILE *m = macstk->bfile;
                                                 while (m && (m->name != f->name)) {
                                                   macstk->bfile = macstk->bfile->next;
                                                   free_file(m);
                                                   m = macstk->bfile;
                                                 }
                                                 if (m) {
                                                   macstk->bfile = macstk->bfile->next;
                                                   free_file(m);
                                                 }
                                                 f->preprocess_me = PREP_DONE;
                                               }
                                               f->abort = 1;
                                               if (operator[oid].type==BREAK) 
                                                 f->repeat = 0;
                                             }
                                             else {
                                               if (operator[oid].type==BREAK) 
                                                 errout(E_WARN, "yylex", ".break directive not used within a repeat or macro block.\n");
                                               else
                                                 errout(E_WARN, "yylex", ".continue directive not used within a repeat block.\n");
                                               for (f = cur_ifile; f; f = f->next)
                                                 f->abort = 0;
                                             }
                                          }
                                          break;
                                      case WHILE:
                                          macro_arg_comma = macro_args; macro_arg_nl = while_body;
                                          macro_arg_comma_ign = 1;
                                          expr_return = expr_return_comma = while_body; 
                                          while_level++;
                                          if_macro_cond = 1;
                                          clear_line();
                                          BEGIN(if_direct);
                                          break;
                                      case IF:
                                          macro_arg_comma = macro_args; macro_arg_nl = if_body;
                                          macro_arg_comma_ign = 1;
                                          expr_return = expr_return_comma = if_body; 
                                          if_level++;
                                          if_macro_cond = 1;
                                          clear_line();
                                          BEGIN(if_direct);
                                          break;
                                      case IIF:
                                          macro_arg_comma = iif_body; macro_arg_nl = INITIAL;
                                          macro_arg_comma_ign = 1;
                                          expr_return_comma = iif_body; expr_return = INITIAL;
                                          if_macro_cond = 1;
                                          BEGIN(if_direct);
                                          break;
                                      case ASCII:
                                      case ASCIZ:
                                          BEGIN(get_delim_string);
                                          break;
                                      case REPEAT:
                                          ++repeat_level;
                                          expr_return = expr_return_comma = repeat_body;
                                          BEGIN(expr_s);
                                          break;
                                      case CMDIN:
                                          ++cmdin_level;
                                          expr_return = expr_return_comma = cmdin_body;
                                          BEGIN(expr_s);
                                          break;
                                      case DOCMD:
                                          BEGIN(get_delim_string);
                                          break;
                                      case ALIGN:
                                          BEGIN(align_expr);
                                          expr_return_comma = align_expr;
                                          expr_return = INITIAL;
                                          break;
                                      case ADDR:
                                      case QUAD:
                                      case LONG:
                                          expr_return = INITIAL; expr_return_comma = expr_s;
                                          BEGIN(expr_s);
                                          break;
                                      case WARN:
                                      case ERROR:
                                      case PRINT:
                                          expr_return = expr_return_comma = INITIAL;
                                          BEGIN(msg_expr);
                                          break;
                                      case PSECT:
				      case DSECT:
                                          psect_attr_error = 0;
                                          expr_return_comma = psect_attr;
                                          expr_return = INITIAL;
					  psect_attr_name = psect_attr;
                                          BEGIN(psect_name);
                                          break;
                                      case SAVE_PSECT:
                                      case SAVE_DSECT:
                                          BEGIN(save_psect_attr);
                                          break;
                                      case RESTORE_PSECT:
				      case RESTORE_DSECT:
                                          BEGIN(kill_to_eol);
                                          break;
                                      case BLKL:
                                      case BLKQ:
                                          expr_return_comma = expr_return = INITIAL;
                                          BEGIN(expr_s);
                                          break;
                                      case IDENT:
                                      case TITLE:
                                          BEGIN(kill_to_eol);
                                          break;
                                      case NOLIST:
                                      case LIST:
                                          BEGIN(listing_dir);
                                          break;
                                      case K_DEBUG:
                                          yydebug++;
                                          yy_flex_debug++;
                                          BEGIN(kill_to_eol);
                                          break;
                                      case NODEBUG:
                                          yydebug--;
                                          yy_flex_debug--;
                                          BEGIN(kill_to_eol);
                                          break;
                                      case MACRO:
                                          macro_arg_comma = macro_args; macro_arg_nl = macro_body;
                                          macro_arg_comma_ign = 0;
                                          macro_level = 1;
                                          BEGIN(macro_name);
                                          break;
				      case MACRO_CALL:
                                          macro_arg_comma = macro_args; macro_arg_nl = INITIAL;
                                          macro_arg_comma_ign = 0;
                                          yylval = make_token(NULL, MACRO_CALL);
                                /*
                                 * record operator id for use in finding macro definition.
                                 */
                                          yylval->tok.q.v[0] = oid;
                                          BEGIN(macro_args_eat_ws);
                                          break;
                                      case ARCH:
                                          BEGIN(arch_dir);
                                          break;
                                      case PRAGMA:
                                          BEGIN(pragma_dir);
                                          break;
                                      case NEXT_ARG:
                                          if (!macstk)
                                            errout(E_WARN, "lexer", "Use of .next_argument directive outside a macro. Directive ignored.\n");
                                          else {
                                              MACRO_STK* tstk = macstk;
                                              while (tstk && tstk->var_arg_idx < 0)
                                                  tstk = tstk->next;
                                              if (!tstk)
                                                  errout(E_WARN, "lexer", "Use of .next_argument directive in a non-variable arg macro. Directive ignored.\n");
                                              else {
                                                  if (tstk->argl[tstk->var_arg_idx] == NULL) {
                                                    if (tstk->var_arg_idx == tstk->mdef->var_arg) /* special case where the macro caller was using the defualt value */
                                                      tstk->var_arg_idx = tstk->mdef->narg; /* want next arg to be null, then wrap back to default value */
                                                    else
                                                      tstk->var_arg_idx = tstk->mdef->var_arg; /* want var arg to go blank when at end, then wrap to begining */
                                                  }
                                                  else {
                                                      if (tstk->var_arg_idx == tstk->mdef->var_arg)
                                                        tstk->var_arg_idx = tstk->mdef->narg;  /* skip over any keyword only args, if any */
                                                      else
                                                        tstk->var_arg_idx += 1;
                                                  }
#ifdef FLEX_DEBUG      
                                                  if (yy_flex_debug) 
                                                    fprintf(stderr, "Going to variable arg %d of %d, in macro %s\n", tstk->var_arg_idx, 
                                                            tstk->mdef->var_arg, tstk->mdef->name);
#endif
                                              }
                                          }
                                          break;
				      case HEADER:
				      case ENDH:
				          break;
                                      case LINK_AFTER:
				          psect_attr_name = INITIAL;
                                          BEGIN(psect_name);
                                          break;
                                      case LINK_ADDRESS:
                                      case LINK_OFFSET:
                                      case LINK_REGION:
                                      case LINK_SIZE:
                                          expr_return = INITIAL; expr_return_comma = expr_s;
                                          BEGIN(expr_s);
                                          break;
                                      case LINK_FIRST:
                                      case LINK_LAST:
				          break;
                                      case LINK_ATTR:
                                          BEGIN(link_attr_dir);
                                          break;
                                      default:
                                          errout(E_ERR, "lexer", "internal error, unknown operator type (%d) for operator \"%s\".\n",
                                                 operator[oid].type, yytext);
                                          break;
                                      }
                                      return operator[oid].type;
                                  }
                                }

 /*
  * Macro preprocessing scanner.
  */  
<prep>{SYMBOL}"'"		{ if (repeat_level || while_level) /* defer preprocessing until doing repeat body */
                                    add_to_macro_line(yytext);
                                  else { 
                                    yytext[yyleng-1] = '\0'; 
                                    add_to_macro_line(get_macro_arg(yytext)); 
                                    yytext[yyleng-1] = '\''; 
                                  }
                                }
<prep>"'"{SYMBOL}		{ if (repeat_level || while_level) /* defer preprocessing until doing repeat body */
                                    add_to_macro_line(yytext);
                                  else
                                    add_to_macro_line(get_macro_arg(yytext+1));
                                }
<prep>"'"{SYMBOL}"'"		{ if (repeat_level || while_level) /* defer preprocessing until doing repeat body */
                                    add_to_macro_line(yytext);
                                  else { 
                                    yytext[yyleng-1] = '\0'; 
                                    add_to_macro_line(get_macro_arg(yytext+1)); 
                                    yytext[yyleng-1] = '\''; 
                                  }
                                }
<prep>{SYMBOL}			add_to_macro_line(get_macro_arg(yytext));
<prep>"%length"{WS}*"("{WS}*	{ if (repeat_level || while_level) /* defer preprocessing until doing repeat body */
                                    add_to_macro_line(yytext);
                                  else 
                                    BEGIN(prep_length);
                                  needed_prep = 1;
                                }
<prep>"%length"			{ add_to_macro_line(yytext); errout(E_ERR, "lexer", "no \"(\" for %%length string operator.\n"); }
<prep>"%section"{WS}*"("{WS}*	{ if (repeat_level || while_level) /* defer preprocessing until doing repeat body */
                                    add_to_macro_line(yytext);
                                  else 
                                    BEGIN(prep_section);
                                  needed_prep = 1;
                                }
<prep>"%section"		{ add_to_macro_line(yytext); errout(E_ERR, "lexer", "no \"(\" for %%section string operator.\n"); }
<prep>"%locate"{WS}*"("{WS}*	{ if (repeat_level || while_level) /* defer preprocessing until doing repeat body */
                                    add_to_macro_line(yytext);
                                  else { 
                                    if (free_string1) { FREE(prep_string1); free_string1 = 0; }
                                    if (free_string2) { FREE(prep_string2); free_string2 = 0; }
                                    free_tok_s(prep_symbol1);
                                    prep_string1 = prep_string2 = NULL; prep_symbol1 = NULL;
                                    prep_str = &prep_string1;
                                    prep_string_rtn = prep_locate_got_1st;
                                    BEGIN(prep_get_arg_or_str);
                                  }
                                  needed_prep = 1;
                                }
<prep>"%locate"			{ add_to_macro_line(yytext); errout(E_ERR, "lexer", "no \"(\" for %%locate string operator.\n"); }
<prep>"%extract"{WS}*"("{WS}*	{ if (repeat_level || while_level) /* defer preprocessing until doing repeat body */
                                    add_to_macro_line(yytext);
                                  else {
                                    if (free_string1) { FREE(prep_string1); free_string1 = 0; }
                                    free_tok_s(prep_symbol1);
                                    free_tok_s(prep_symbol2);
                                    prep_symbol1 = prep_symbol2 = NULL; prep_string1 = NULL;
                                    prep_symb = &prep_symbol1;
                                    prep_symb_rtn = prep_extract_got_1st;
                                    BEGIN(prep_get_symb);
                                  }
                                  needed_prep = 1;
                                }
<prep>"%extract"		{ add_to_macro_line(yytext); errout(E_ERR, "lexer", "no \"(\" for %%extract string operator.\n"); }
<prep>[^\'%a-zA-Z$_.\n]+	add_to_macro_line(yytext);
<prep>.				add_to_macro_line(yytext);
<prep>\n			{ add_to_macro_line(yytext); 
                                  if (!needed_prep) {
                                    macstk->mdef->skipargck[cur_ifile->lineno + cur_ifile->base_lineno - macstk->mdef->lineno] = 1;
#ifdef FLEX_DEBUG
                                    if (yy_flex_debug) 
                                      fprintf(stderr, "Turning off preprocessing for macro line: %d + %d - %d of %s\n", cur_ifile->lineno,cur_ifile->base_lineno,macstk->mdef->lineno, macstk->mdef->name);
#endif
                                  }
                                  cur_ifile->next = macstk->bfile; 
                                  macstk->bfile = cur_ifile; 
                                  macro_line_ptr = macro_line; 
                                  preprocessing_macro = 0; 
                                  BEGIN(macstk->prep_return); 
                                  (void)next_file();
                                }
<prep><<EOF>>			{ add_to_macro_line("\n");
                                  free_file(cur_ifile);
                                  macro_line_ptr = macro_line;
                                  preprocessing_macro = 0;
                                  BEGIN(macstk->prep_return);
                                  (void)next_file();
                                  cur_ifile->preprocess_me = PREP_DONE;
                                }

<prep_section>{SYMBOL}		{ char *snm = get_macro_arg(yytext); 
                                  if (strcmp(snm,".")==0) 
                                    add_to_macro_line(Cpsect->name);
                                  else {
                                    SENTRY *s = symdata(snm);
                                    if (!s) add_to_macro_line("");
                                    else add_to_macro_line(s->sect->name); 
                                  }
                                  BEGIN(prep_section_close); 
                                }
<prep_section>")"		{ errout(E_ERR, "lexer", "no string specified in %%section string operator.\n"); BEGIN(prep); }
<prep_section>.			{ errout(E_ERR, "lexer", "illegal character in %%section string operator: \"%c\".\n", yytext[0]);
                                  BEGIN(prep_length_close);
                                }
<prep_section>\n       		{ errout(E_ERR, "lexer", "end of line found in %%section string operator.\n"); unput('\n'); BEGIN(prep); }

<prep_section_close>{WS}*")"	BEGIN(prep);
<prep_section_close>.		{ errout(E_ERR, "lexer", "illegal character in %%section string operator: \"%c\".\n", yytext[0]);
                                  BEGIN(prep);
                                }
<prep_section_close>\n		{ errout(E_ERR, "lexer", "end of line found in %%section string operator.\n"); unput('\n'); BEGIN(prep); }

<prep_length>{SYMBOL}		{ add_num_to_macro_line(strlen(get_macro_arg(yytext))); BEGIN(prep_length_close); }
<prep_length>"^".		{ prep_count = 0; string_delim = yytext[1]; prep_string_rtn = prep_length_close; BEGIN(prep_count_string); }
<prep_length>"<"		{ prep_count = 0; string_delim = '>'; prep_string_rtn = prep_length_close; BEGIN(prep_count_string); }
<prep_length>")"		{ errout(E_ERR, "lexer", "no string specified in %%length string operator.\n"); BEGIN(prep); }
<prep_length>.			{ errout(E_ERR, "lexer", "illegal character in %%length string operator: \"%c\".\n", yytext[0]);
                                  BEGIN(prep_length_close);
                                }
<prep_length>\n			{ errout(E_ERR, "lexer", "end of line found in %%length string operator.\n"); unput('\n'); BEGIN(prep); }

<prep_length_close>{WS}*")"	BEGIN(prep);
<prep_length_close>.		{ errout(E_ERR, "lexer", "illegal character in %%length string operator: \"%c\".\n", yytext[0]);
                                  BEGIN(prep);
                                }
<prep_length_close>\n		{ errout(E_ERR, "lexer", "end of line found in %%length string operator.\n"); unput('\n'); BEGIN(prep); }

<prep_count_string>.		{ if (yytext[0] == string_delim) {
                                      add_num_to_macro_line(prep_count);
                                      BEGIN(prep_string_rtn);
                                  } else
                                      ++prep_count;
                                }
<prep_count_string>\n		{ errout(E_ERR, "lexer", "end of line found in delimited string.\n"); unput('\n'); BEGIN(prep); }

<prep_locate_got_1st>{WS}+	;
<prep_locate_got_1st>","{WS}*	{ prep_str = &prep_string2; prep_string_rtn = prep_locate_got_2nd; BEGIN(prep_get_arg_or_str); }
<prep_locate_got_1st>")"	{ errout(E_ERR, "lexer", "string2 not specified in %%locate string operator.\n"); BEGIN(prep); }

<prep_locate_got_2nd>{WS}+	;
<prep_locate_got_2nd>","{WS}*	{ prep_symb = &prep_symbol1; prep_symb_rtn = prep_locate_close; BEGIN(prep_get_symb); }
<prep_locate_got_2nd>")"	{ BEGIN(prep_locate_close); unput(')'); }

<prep_locate_close>{WS}*")"	{ add_num_to_macro_line(prep_do_locate(prep_string1, prep_string2, prep_symbol1)); BEGIN(prep); }

<prep_locate_got_1st,prep_locate_got_2nd,prep_locate_close>.	{ errout(E_ERR, "lexer", "illegal character in %%locate string operator: \"%c\".\n", yytext[0]);
                                  				  BEGIN(prep_locate_close);
                                				}
<prep_locate_got_1st,prep_locate_got_2nd,prep_locate_close>\n	{ errout(E_ERR, "lexer", "end of line found in %%locate string operator.\n"); unput('\n');
                                  				  BEGIN(prep);
                                				}

<prep_extract_got_1st>{WS}+	;
<prep_extract_got_1st>","{WS}*	{ prep_symb = &prep_symbol2; prep_symb_rtn = prep_extract_got_2nd; BEGIN(prep_get_symb); }
<prep_extract_got_1st>")"	{ errout(E_ERR, "lexer", "symbol2 not specified in %%extract string operator.\n"); BEGIN(prep); }

<prep_extract_got_2nd>{WS}+	;
<prep_extract_got_2nd>","{WS}*	{ prep_str = &prep_string1; prep_string_rtn = prep_extract_close; BEGIN(prep_get_arg_or_str); }
<prep_extract_got_2nd>")"	{ errout(E_ERR, "lexer", "string1 not specified in %%extract string operator.\n"); BEGIN(prep); }

<prep_extract_close>{WS}*")"	{ prep_do_extract(prep_symbol1, prep_symbol2, prep_string1); BEGIN(prep); }

<prep_extract_got_1st,prep_extract_got_2nd,prep_extract_close>.	{ errout(E_ERR, "lexer", "illegal character in %%extract string operator: \"%c\".\n", yytext[0]);
                                  				  BEGIN(prep_extract_close);
                                				}
<prep_extract_got_1st,prep_extract_got_2nd,prep_extract_close>\n { errout(E_ERR, "lexer", "end of line found in %%extract string operator.\n"); unput('\n');
                                  				   BEGIN(prep);
                                				 }

<prep_get_symb>{SYMBOL}		{ *prep_symb = get_symbol_value(make_token(yytext, SYMBOL),0/*is_while*/); BEGIN(prep_symb_rtn); }
<prep_get_symb>[0-9]+		{ *prep_symb = cvtradix(yytext, 10, NUMBER); BEGIN(prep_symb_rtn); }
<prep_get_symb>.		{ errout(E_ERR, "lexer", "illegal character in string operator symbol argument: \"%c\".\n", yytext[0]);
                                  BEGIN(prep_symb_rtn);
				}
<prep_get_symb>\n		{ errout(E_ERR, "lexer", "end of line found in string operator symbol argument.\n"); unput('\n');
                                   BEGIN(prep);
                                }

<prep_get_arg_or_str>{SYMBOL}	{ *prep_str = get_macro_arg(yytext); BEGIN(prep_string_rtn); }
<prep_get_arg_or_str>"^".	{ string_delim = yytext[1]; BEGIN(prep_get_string); }
<prep_get_arg_or_str>"<"	{ string_delim = '>'; BEGIN(prep_get_string); }

<prep_get_string>.		{ if (yytext[yyleng-1] == string_delim) {
    				      *prep_str = strncpy(ALLOC_STR(yyleng), yytext, yyleng-1);
                                      (*prep_str)[yyleng-1] = '\0';
                                      if (prep_str == &prep_string1)
                                          free_string1 = 1;
                                      else
                                          free_string2 = 1;
                                      BEGIN(prep_string_rtn);
                                  } else
                                      yymore();
                                }

<prep_get_arg_or_str,prep_get_string>.		{ errout(E_ERR, "lexer", "illegal character in string operator string argument: \"%c\".\n", yytext[0]);
                                  		  BEGIN(prep_string_rtn);
						}
<prep_get_arg_or_str,prep_get_string>\n		{ errout(E_ERR, "lexer", "end of line found in string operator string argument.\n"); unput('\n');
                                  		   BEGIN(prep);
                                		}

 /*
  * Global unmatched character rule.
  */
.				{ errout(E_ERR, "lexer", "illegal character \"%c\" found.\n", yytext[0]); }

 /*
  * Global EOF rule.
  */
<<EOF>>				{  int was_prep_ifile = cur_ifile->preprocess_me; 
                                   assert(cur_ifile->type != F_MACRO_PREP);
                                   if (cur_ifile->type == F_FILE) { 
				    fclose(cur_ifile->f);   /* close the file we just finished */
                                    if (cur_ifile->string_type == FILE_DELETE)
				     remove(cur_ifile->name); /* it was a tmp file created by HAL */
				   }

				  if ((cur_ifile->preprocess_me != PREP_IT) && (cur_ifile->type == F_STRING) && 
                                      (((cur_ifile->string_type == REPEAT) && ((cur_ifile->repeat < 0) || (--(cur_ifile->repeat) > 0))) ||
                                       ((cur_ifile->string_type == WHILE) && cur_ifile->repeat && eval_while_cond(cur_ifile->while_cond)))) {
				    /* repeat count non-zero, push input back on stack */
                                    cur_ifile->abort = 0;
				    cur_ifile->lineno = 0;
				    cur_ifile->next = ifile;
				    ifile = cur_ifile;
				    ifile->f = (FILE *)ifile->name;
				    yy_delete_buffer(ifile->buff);
				    ifile->buff = NULL;
				    ifile->yylval = NULL;
				    ifile->yychar = YYEMPTY;
                                    if (cur_ifile->preprocess_me) {
                                        /* If we were preprocessing, the next time we put this file on
                                           the stack we still want to be preprocessing. */
                                        cur_ifile->preprocess_me = PREP_IT;
                                        add_input(ifile->name, F_MACRO_PREP, 1, NULL, ADD_PUSH, cur_ifile->base_lineno, cur_ifile->string_type);
                                    }
				  } else {
                                    /* if this was a macro, pop macro arguments off stack. */
                                    if (cur_ifile->type == F_MACRO || (cur_ifile->preprocess_me == PREP_IT)) {
                                        if (macstk == NULL) {
                                            errout(E_ERR, "lexer", "internal error, macro stack NULL with MACRO as input file.\n");
                                        } else {
                                            /*
                                             * macro reached EOF, check for more lines in macro body to
                                             * preprocess.
                                             */
                                            if (macstk->bfile == NULL) {
                                                MACRO_STK* next_macstk = macstk->next;
                                                /*
                                                 * All text of macro has been preprocessed, and the
                                                 * macro has reached EOF.  We are finally done processing
                                                 * the macro.
                                                 */
                                              
                                                /*
                                                 * Pop macro args off stack
                                                 */
                                                free_macstk(macstk);
                                                macstk = next_macstk;

                                                /* STORAGE?? (for macstk) */
                                              
                                                free_file(cur_ifile);
                                            } else {
                                                INPUT_FILE *ftmp;
                                                /*
                                                 * Setup the input file on the macro stack for preprocessing
                                                 * of the next line of the macro.
                                                 */

                                                /*
                                                 * Mark this buffer as not being at EOF any more.
                                                 */
                                                YY_NEW_FILE;
                                              
                                                /* first push the current input back on the input stack */
                                                push_current_ifile();
                                                
                                                /* now place the macro stack file on the input file stack */
                                                ftmp = macstk->bfile;
                                                macstk->bfile = macstk->bfile->next;
                                                ftmp->next = ifile;
                                                ifile = ftmp;
                                            }
                                        }
                                    } else
                                        free_file(cur_ifile);
                                  }

                                  if (!next_file())
                                    yyterminate();

                                  if (cur_ifile->preprocess_me == PREP_IT) {
                                    /* old check for below: (cur_ifile->abort || (!was_prep_ifile && (cur_ifile->type != F_MACRO_PREP)) */
                                      if (cur_ifile->type != F_MACRO_PREP) { /* need to put PREP on stack if not already there */
                                          INPUT_FILE *ftmp;
                                          /*
                                           * Setup the input file on the macro stack for preprocessing
                                           * of the next line.
                                           */

                                          /*
                                           * Mark this buffer as not being at EOF any more.
                                           */
                                          YY_NEW_FILE;
                                              
                                          /* first push the current input back on the input stack */
                                          push_current_ifile();

                                          ftmp = macstk->bfile;
                                          macstk->bfile = macstk->bfile->next;
                                          ftmp->next = ifile;
                                          ifile = ftmp;
                                          
                                          next_file();
                                      }
                                      
                                      /* Do we really need to preprocess this with the lexer? */
                                      if (macstk->mdef->skipargck[cur_ifile->lineno + cur_ifile->base_lineno - macstk->mdef->lineno +1]) { 
                                        /* No, just copy the line into macro_line and process the stack as if we
                                           just left the preprocesser */
                                        char buf[1024];
                                        int result;
#ifdef FLEX_DEBUG
                                       if (yy_flex_debug) 
                                          fprintf(stderr, "No preprocessing needed for macro line: %d of %s\n", cur_ifile->lineno + cur_ifile->base_lineno - macstk->mdef->lineno +1, macstk->mdef->name);
#endif
                                        macro_line_ptr = macro_line;
                                        macro_line[0] = '\0';
                                        for(get_input(buf, &result, 1024); result!=YY_NULL; get_input(buf, &result, 1024)) {
                                          add_to_macro_line(buf);
                                          if (buf[result-1]=='\n')
                                            /* below change could be added to above condition to do multiple lines.
                                               But we need to fix the case of one of those lines calling a macro which
                                               then corrupts the macro_line and, also, when we are done with that macro we
                                               don't want to start up preprocessor again ... need to check it macro_line is empty 
                                              &&
                                              (macstk->mdef->skiparglen == (cur_ifile->lineno + cur_ifile->base_lineno - macstk->mdef->lineno + 1) ||
                                               !macstk->mdef->skipargck[cur_ifile->lineno + cur_ifile->base_lineno - macstk->mdef->lineno +1])) */
                                            break;
                                        }
                                        if (result==YY_NULL) { /* i.e. if we reached EOF, do the normal <prep><<EOF>> rule */
                                          free_file(cur_ifile);
                                          macro_line_ptr = macro_line;
                                          (void)next_file();
                                          cur_ifile->preprocess_me = PREP_DONE;
                                        }
                                        else { /* do the normal <prep>\n rule */
                                          cur_ifile->next = macstk->bfile; 
                                          macstk->bfile = cur_ifile; 
                                          macro_line_ptr = macro_line; 
                                          (void)next_file();
                                        }
                                      }
                                      else   
                                        start_up_preprocessor();
                                  }

                                  /* if there was a token pending, return it
                                     otherwise just keep going */
                                  if (yychar != YYEMPTY)
                                     return yychar;
                                }

%%

#ifndef _tolower
#define _tolower(s) tolower(s)
#endif

char *strlow(char *str)
{
  register char *tmp=str;

  for (; *tmp; ++tmp)
    if (isupper(*tmp))
      *tmp = _tolower(*tmp);  /* use the macro, it's faster */

  return str;
}

static char* get_macro_arg(char* text)
{
    static char local_label_str[32];
    static char *null_str = "";
    int i, narg = macstk->mdef->narg;

    if (repeat_level || while_level) {/* defer preprocessing of these blocks until BODYTEXT actually is executed */
      needed_prep = 1;  /* Can't be sure if prep was needed for these cases ... so do the safe thing */
      return text;
    }

    for (i=0; i<narg; ++i) {
        /* initialize temporary character pointers to do string compare */
        char* ttmp = text;
        char* atmp = macstk->mdef->args[i];

        /* loop while both strings still has text in it */
        while (*ttmp && *atmp) {
            /* get a lowercase copy of the character in text */
            char c = isupper(*ttmp)?_tolower(*ttmp):*ttmp;

            /* if they are different, fail. */
            if (c != *atmp)
                break;

            /* move to next character */
            ++atmp;
            ++ttmp;
        }

        /* if both strings reached end, then we have a match */
        if (!*ttmp && !*atmp)
            break;

        /* otherwise, do it again */
    }

    if (i == narg)
        return text;

    /* Remember that we found a substitiution */
    needed_prep = 1;
    
    if (i == macstk->mdef->var_arg && i != macstk->var_arg_idx) {
      if (macstk->argl[macstk->var_arg_idx])
        return macstk->argl[macstk->var_arg_idx];
      else {
        return (null_str);
      }
    }
    else if (macstk->argl[i] != NULL)
        return macstk->argl[i];

    return macstk->mdef->argdefs[i];
}

static int prep_do_locate(char* str1, char* str2, TOKEN_LIST* symb)
{
    char* ctmp = str2;
    char* cpos;
    int slen = strlen(str2);

    if (symb != NULL) {
        if (slen <= symb->tok.q.v[0])
            return slen;
        
        ctmp += symb->tok.q.v[0];
    }

    cpos = strstr(ctmp, str1);
    if (cpos == NULL)
        return slen;

    return cpos - str2;
}

static void prep_do_extract(TOKEN_LIST* symb1, TOKEN_LIST* symb2, char* str)
{
    /*
     * Only non-null if starting within the string and length requested
     * != 0.
     */
    if ((strlen(str) > symb1->tok.q.v[0]) && (symb2->tok.q.v[0] != 0)) {
        char save_char;
        char* ctmp = str + symb1->tok.q.v[0];
        int slen = strlen(ctmp);
        int num = (symb2->tok.q.v[0] > slen)? slen : symb2->tok.q.v[0];
        /* debug    errout(E_INFO,"extract","extract(%d, %d, \"%s\")\n",symb1->tok.q.v[0],symb2->tok.q.v[0],str); */
        
        save_char = *(ctmp+num);
        *(ctmp + num) = '\0';
        
        add_to_macro_line(ctmp);
        
        *(ctmp + num) = save_char;
    }

    return;
}

#ifdef __STDC__
YYSTYPE cvtradix(char *text, int radix, int type)
#else
YYSTYPE cvtradix(text, radix, type)
char *text;
int radix, type;
#endif
{
  YYSTYPE temp = alloc_tok();
  int overflow = 0;

#ifdef __alpha
  MTV_uint64 old64, c64;

  for (c64=0; *text; text++) {
    old64 = c64;
    c64 *= radix;
    overflow |= old64 > c64;

    if (isdigit(*text)) 
      c64 += *text - '0';
    else 
      c64 += toupper(*text) - 'A' + 10;
  }

  temp->tok.q.v[1] = (c64>>32) & 0xffffffff;
  temp->tok.q.v[0] = c64 & 0xffffffff;

#else
  int32 *num = (int32*)temp->tok.q.v;
  int32 digit[2];

  digit[1] = 0;
  while (*text) {
    mul64((uint32*)num, radix);

    if (isdigit(*text)) {
      digit[0] = *text - '0';
      add64(num, digit, num);
    } else {
      digit[0] = toupper(*text) - 'A' + 10;
      add64(num, digit, num);
    }
    text++;
  }

#endif

  if (overflow)
    errout(E_WARN, "cvtradix", "integer constant overflow: truncating upper bits.\n");
  temp->type = type;
  return temp;
}

#ifdef __STDC__
YYSTYPE cvtasciiop(char *text)
#else
YYSTYPE cvtasciiop(text)
char *text;
#endif
{
  YYSTYPE temp = alloc_tok();
  int32 *num = (int32*)temp->tok.q.v;
  int overflow = 0;
  int len = strlen(text) - 2;
  char *cptr = &text[1] + len - 1;

  temp->type = NUMBER;

  for (; len > 0; --len) {
      
      /* shift num up by 1 byte. */
      if (num[1] & 0xFF000000)
          overflow = 1;

      num[1] = (num[1] << 8) | ((num[0] >> 24) & 0xFF);
      num[0] = (num[0] << 8) | *cptr;
             
      cptr--;
  }

  if (overflow)
    errout(E_WARN, "cvtasciiop", "ascii constant overflow: truncating last characters.\n");
  return temp;
}

/*
 * Function to strip the quotes off a quoted string.  No checking is performed,
 * but that's okay because we won't execute unless the token matches.
 */
#ifdef __STDC__
char *unquote(char *o, char *i)
#else
char *unquote(o, i)
char *o, *i;
#endif
{
    int l = strlen(i) - 2;
    
    strncpy(o, &i[1], l);
    o[l] = '\0';

    return o;
}

#ifdef __STDC__
YYSTYPE make_token(char *text, int type)
#else
YYSTYPE make_token(text, type)
char *text;
int type;
#endif
{
  int text_size;
  char *ctmp;
  YYSTYPE temp = alloc_tok();

  temp->type = type;
  switch (type) {
  case STRING:
    temp->tok.s = unquote(ALLOC_STR(strlen(text)-2), text);
    break;
  case SYMBOL:
    /* convert to lowercase first */
    for (ctmp = text; *ctmp && (*ctmp != ' ') && (*ctmp != '\t'); ++ctmp)
        if (isupper(*ctmp))
            *ctmp = _tolower(*ctmp);
    if (*ctmp != '\0')
        *ctmp = '\0';
    /* fall through */
  case COMMENT:
  case ARG_VALUE:
    temp->tok.s = strcpy(ALLOC_STR(strlen(text)), text);
    break;
  case LOCAL_LABEL:
      {
          char tstr[32];
          
          sprintf(tstr, "llb%d%%", local_label_blknum);

          temp->tok.s = strcpy(ALLOC_STR(strlen(tstr) + strlen(text)), tstr);
          strcpy(temp->tok.s+strlen(tstr), text);
      }
    break;
  case BODYTEXT:
    temp->lineno = start_line;
    temp->tok.s = strcpy(ALLOC_STR(strlen(text)), text);
    break;

  default:
    /* For all others, type is the important part */
    break;
  }

  return temp;
}

#ifdef __STDC__
static void add_to_line(char *text)
#else
static void add_to_line(text)
char *text;
#endif
{
  if ((lptr + strlen(text) - line) > line_size) {
    int diff = lptr - line;

    line_size += 2048;
    line = (char *)realloc(line, line_size + 1);
    lptr = line + diff;
  }
    
  strcpy(lptr, text);
  lptr += strlen(text);

  return;
}

#ifdef __STDC__
static void add_to_macro_line(char *text)
#else
static void add_to_macro_line(text)
char *text;
#endif
{
    if (*text == '\0') {
        /* null string, mark it and return */
        TOKEN_LIST** ttok = &had_null;

        /* find the end of the null mark list */
        while (*ttok != NULL)
            ttok = &(*ttok)->next;

        /* record location within line at the end of the list */
        *ttok = make_value_token(0, macro_line_ptr - macro_line);
        
        return;
    }
    
    if ((macro_line_ptr + strlen(text) - macro_line) > macro_line_size) {
        int diff = macro_line_ptr - macro_line;

        macro_line_size += 2048;
        macro_line = (char *)realloc(macro_line, macro_line_size + 1);
        macro_line_ptr = macro_line + diff;
    }
    
    strcpy(macro_line_ptr, text);
    macro_line_ptr += strlen(text);

    return;
}

#ifdef __STDC__
static void add_num_to_macro_line(int num)
#else
static void add_num_to_macro_line(num)
int num;
#endif
{
    char text[32];

    sprintf(text, "%d", num);
    
    if ((macro_line_ptr + strlen(text) - macro_line) > macro_line_size) {
        int diff = macro_line_ptr - macro_line;
        
        macro_line_size += 2048;
        macro_line = (char *)realloc(macro_line, macro_line_size + 1);
        macro_line_ptr = macro_line + diff;
    }
    
    strcpy(macro_line_ptr, text);
    macro_line_ptr += strlen(text);

    return;
}

static void clear_line()
{
  /* reset line */
  lptr = line;
  *lptr = '\0';

  return;
}

#ifdef __STDC__
static void add_to_listing_line(char *text)
#else
static void add_to_listing_line(text)
char *text;
#endif
{
    if ((listing_line_ptr + strlen(text) - listing_line) > listing_line_size) {
        int diff = listing_line_ptr - listing_line;

        listing_line_size += 2048;
        listing_line = (char *)realloc(listing_line, listing_line_size + 1);
        listing_line_ptr = listing_line + diff;
    }
    
    strcpy(listing_line_ptr, text);
    listing_line_ptr += strlen(text);

    return;
}

void clear_listing_line()
{
    listing_line_ptr = listing_line;
    *listing_line_ptr = '\0';

    return;
}

void start_up_preprocessor()
{
  /* put lexer in preprocessing state */
  macstk->prep_return = last_begin_state;
  BEGIN(prep);
  macro_line_ptr = macro_line;
  needed_prep = 0;
  preprocessing_macro = 1;
}

static void get_input(buf,result,max_size)
     char *buf;
     int *result;
     int max_size;
{
    static int sent_nl = 1;
    char tstr[256];
    char *sptr, *origbuf = buf;
    int num;
  
    *buf = 0;		/* preset so end-of-file is detected... */

    /* force EOF */
    if (cur_ifile->abort) {
        *result = YY_NULL;
/*        sent_nl = 0; */
        return;
    }

    switch (cur_ifile->type) {
    case F_FILE:
        fgets(buf,max_size,cur_ifile->f);
        
        if (sent_nl) {
            cur_ifile->lineno++;
#ifdef FLEX_DEBUG
            if (yy_flex_debug) 
              fprintf(stderr, "Incrementing current line number to %d (base =  %d)\n", cur_ifile->lineno,cur_ifile->base_lineno);
#endif
        }
        
        *result = strlen(buf);
        
        sent_nl = buf[*result-1] =='\n';
        break;
    case F_MACRO:
    case F_STRING:
        if (cur_ifile->type==F_MACRO || cur_ifile->preprocess_me) {
          if (*macro_line_ptr == '\0') {
            /*
             * Need to get next line.
             */
            *result = YY_NULL;
            return;
          }
          
          if (sent_nl) {
            cur_ifile->lineno++;
#ifdef FLEX_DEBUG
            if (yy_flex_debug) 
              fprintf(stderr, "Incrementing current line number to %d (base =  %d)\n", cur_ifile->lineno,cur_ifile->base_lineno);
#endif
        }
          
          /* get amount of line to return */
          num = strlen(macro_line_ptr);
          
          /* must initialize because we use increment below.*/
          *result = 0;
          
          /* default sent_nl to 1 */
          sent_nl = 1;
          
          /*
           * Check to see if there is was a null macro replacement in this macro line.
           */
          while (had_null != NULL) {
            /* only send the characters prior to the null replacement. */
            int lloc = had_null->tok.q.v[0];	/* line location for the null */
            int cloc = macro_line_ptr - macro_line; /* current line location */
            
            if (cloc == lloc) {
              char* null_str = "%null%";
              int null_len = 6/*strlen(null_str)*/;
              TOKEN_LIST* ttok = had_null->next;
              
              /*
               * We are at the null replacement point, check to see if we need
               * to send a special token for parsing purposes.
               */
              if (if_macro_cond) {
                int locnum;
                
                locnum = null_len - had_null->tok.q.v[1];
                null_str += had_null->tok.q.v[1];
                
                /*
                 * We are in a .if macro arg condition, null args must not be
                 * null or we can't parse it.  So we'll send "%null%" instead.
                 */
                if (locnum > max_size) {
                  strncpy(buf, null_str, max_size);
                  locnum = max_size;
                  had_null->tok.q.v[1] += max_size;
                } else {
                  strncpy(buf, null_str, locnum);
                  had_null->tok.q.v[1] += locnum;
                }
                
                /*
                 * Move buffer point ahead so later strncpy won't overwrite what
                 * we just did.  Mark our six characters also.
                 */
                buf += locnum;
                *result = locnum;
                
                /* must reduce max_size so we don't overflow the buffer. */
                max_size -= locnum;
                
                if (max_size == 0)
                  break;
              } else
                had_null->tok.q.v[1] = null_len;
              
              if (had_null->tok.q.v[1] == null_len) {
                /* token used/not needed, free it */
                free_tok(had_null);
                had_null = ttok;
              }
            } else {
              /*
               * We are not to the null point yet, return only text to that point.
               *
               * Note: no +1 because the null point takes up no space itself.
               */
              num = lloc - cloc;
              sent_nl = 0;		/* we are not completing the macro line. */
              break;
            }
          }
          
          if (num > max_size-1) {
            strncpy(buf, macro_line_ptr, max_size-1);
            macro_line_ptr += max_size-1;
            *result += max_size-1;
            sent_nl = 0;
          } else {
            strncpy(buf, macro_line_ptr, num);
            macro_line_ptr += num;
            *result += num;
          }

          origbuf[*result]='\0';
        
#ifdef FLEX_DEBUG
          if (yy_flex_debug) {
            fprintf(stderr, "macro line: %d \"%s\"\n", *result, origbuf);
          }
#endif
          break;
        }
    case F_MACRO_PREP:
        sptr = (char *)cur_ifile->f;
        if (sptr == NULL) {
            *result = YY_NULL;
            return;
        } else {
            /* return body one line at a time, passing through the preprocessor first (if F_MACRO)
               NOTE: macro will always end in a "\n" because of parsing considerations */
            char *ctmp, *tline;
            int tlen;
      
            if ((ctmp = strchr(sptr, '\n')) == NULL) {
                /*
                 * If there is no NL at the end of the macro, just use what's there.
                 */
                ctmp = sptr + strlen(sptr) - 1;
            }
      
            if (sent_nl) {
                cur_ifile->lineno++;
#ifdef FLEX_DEBUG
            if (yy_flex_debug) 
              fprintf(stderr, "Incrementing current line number to %d (base =  %d)\n", cur_ifile->lineno,cur_ifile->base_lineno);
#endif
        }

            /* figure out length of the line we are returning. */
            tlen = ctmp - sptr + 1;

            /*
             * if the length is greater than the maximum buffer size, fill the
             * buffer, but don't overflow it.
             */
            if (tlen > max_size - 1) {
                num = max_size - 1;
                sent_nl = 0;
            } else {
                num = tlen;
                sent_nl = 1;
            }

            /* copy line into buffer */
            strncpy(buf, sptr, num);
            *result = num;
            buf[num] = '\0';

            /* recalculate end position based on the maximum buffer size */
            ctmp = sptr + num;
            
            /*
             * If we are pointing at a NUL, then mark ourselves as done.
             * Otherwise, place ourselves at the character after the last character
             * we just sent back.
             */
            if (*ctmp == '\0')
                cur_ifile->f = NULL;
            else
                cur_ifile->f = (FILE *)ctmp;
        }
        break;
    default:
        errout(E_ERR, "get_input", "illegal input file type (%d).\n", cur_ifile->type);
        break;
    }

    /*
     * Add return data to the listing file line, unless the file type is
     * MACRO_PREP, in which case we want to ignore it.  The listing file should contain
     * the already processed line.
     *
     * Also, if the listing is not currently enabled, then we will not even look at the
     * line, so why both to spend the time copying it?
     */
    if ((listing_file != NULL) && (cur_ifile->type != F_MACRO_PREP) &&
        (listing_enable > 0))
        add_to_listing_line(buf);
    
    return;
}
