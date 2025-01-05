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
#include <stdlib.h>
#ifndef __STDC__
#include <varargs.h>
#else
#include <stdarg.h>
#endif
#include <signal.h>

#include "hal.h"
#include "operator.h"
#include "NN.h"

int num_errors = 0;
int num_warnings = 0;

INPUT_FILE* ifile, *cur_ifile;
char* cur_fname;
int cur_base_lineno;


typedef struct buffer_list_ BUFFER_LIST;
struct buffer_list_ {
  YY_BUFFER_STATE buff;
  BUFFER_LIST *next;
};
BUFFER_LIST *macro_buff_free_list = NULL;
BUFFER_LIST *macro_prep_buff_free_list = NULL;

#ifdef __STDC__
void UNIX_expand_pathname(const char *inpath, char *outpath)
#else
void UNIX_expand_pathname(inpath, outpath)
const char *inpath;
char * outpath;
#endif
{
  char head[PATH_MAX+1], tail[PATH_MAX+1];
  char temp[PATH_MAX+1];
  char *envptr, *env_endptr;
  const char *env_var;

  if (inpath==NULL) {
      outpath[0] = '\0';
      return;
  }

  if ((envptr = strchr(inpath,'$')) == NULL) {
    strcpy(outpath,inpath);
  } else {
    *head = '\0';
    strncat(head,inpath,envptr - inpath);
    if ((env_endptr = strchr(++envptr,'/')) == NULL) {
      env_var = getenv(envptr);
      *tail = '\0';
    } else {
      *temp = '\0';
      strncat(temp,envptr,env_endptr - envptr);
      env_var = getenv(temp);
      strcpy(tail,env_endptr);
    }
    sprintf(outpath,"%s%s%s",head,env_var,tail);
  }
}

#ifdef __STDC__
void free_file(INPUT_FILE *tfil)
#else
void free_file(tfil)
INPUT_FILE *tfil;
#endif
{
  BUFFER_LIST *temp;

  /*
   * MACRO_PREP's are the macro body, so don't free them, and for
   * FILE's, the name might have been recorded for informational purposes.
   */
  if ((tfil->type != F_MACRO_PREP) && (tfil->type != F_FILE) && (tfil->name != NULL))
    FREE(tfil->name);
  
  if (tfil->type == F_FILE) {
      TOKEN_LIST* ttok = alloc_tok();
      ttok->type = STRING;
      ttok->tok.s = tfil->name;
      ttok->next = fname_list;
      fname_list = ttok;
  }

  switch(tfil->type){
  case F_MACRO:
    temp = ALLOC(BUFFER_LIST);
    temp->buff = tfil->buff;
    temp->next = macro_buff_free_list;
    macro_buff_free_list = temp;
    break;
  case F_STRING:
  case F_MACRO_PREP:
    temp = ALLOC(BUFFER_LIST);
    temp->buff = tfil->buff;
    temp->next = macro_prep_buff_free_list;
    macro_prep_buff_free_list = temp;
    break;
  default:
    yy_delete_buffer(tfil->buff);
  }
  
  FREE(tfil);

  return;
}

#ifdef __STDC__
void free_macstk(MACRO_STK *tstk)
#else
void free_macstk(tstk)
MACRO_STK *tstk;
#endif
{
    int i;

    if (tstk == NULL)
        return;
    
    if (tstk->mdef->narg != 0) {
        for (i = 0; i < tstk->mdef->narg; ++i)
            if (tstk->argl[i] != NULL)
                FREE(tstk->argl[i]);
        
        FREE(tstk->argl);
    }
    
    FREE(tstk);
    
    return;
}

#ifdef __STDC__
void free_macrodef(MACRODEF *tmac)
#else
void free_macrodef(tmac)
MACRODEF *tmac;
#endif
{
    if (tmac == NULL)
        return;
    
    if (tmac->narg != 0) {
        int i;
        
        for (i = 0; i < tmac->narg; ++i) {
            if (tmac->args[i] != NULL)
                FREE(tmac->args[i]);
            if ((tmac->argdefs[i] != (char*)'?') &&
                (tmac->argdefs[i] != NULL))
                FREE(tmac->argdefs[i]);
        }
    }
    if (tmac->args != NULL)
        FREE(tmac->args);
    if (tmac->argdefs != NULL)
        FREE(tmac->argdefs);
    FREE(tmac->name);
    FREE(tmac->body);
    FREE(tmac);
    return;
}

#ifdef __STDC__
void free_tok_list(TOKEN_LIST *tok)
#else
void free_tok_list(tok)
TOKEN_LIST *tok;
#endif
{

  if (tok == NULL)
    return;

  free_tok_list(tok->next);
  free_tok(tok);

  return;
}

#ifdef __STDC__
void free_tok_list_s(TOKEN_LIST *tok)
#else
void free_tok_list_s(tok)
TOKEN_LIST *tok;
#endif
{

  if (tok == NULL)
    return;

  free_tok_list_s(tok->next);
  free_tok_s(tok);

  return;
}

void free_absexpr(ABSEXPR* tabs)
{

    free_location(tabs->lloc);
    free_tok_list_s(tabs->expr);
    free_tok_s(tabs->result);

    FREE(tabs);
    
    return;
}

#ifdef __STDC__
TOKEN_LIST *copy_expr (TOKEN_LIST *expr)
#else
TOKEN_LIST *copy_expr (expr)
TOKEN_LIST *expr;
#endif
{
  TOKEN_LIST *texpr = NULL;

  if (expr != NULL) {
    texpr = alloc_tok();
    texpr->type = expr->type;
    texpr->lineno = expr->lineno;
    switch (expr->type) {
    case STRING:
    case SYMBOL:
    case COMMENT:
    case ARG_VALUE:
    case LOCAL_LABEL:
    case BODYTEXT:
        texpr->tok.s = strcpy(ALLOC_STR(strlen(expr->tok.s)), expr->tok.s);
        break;
    default:
        texpr->tok.q = expr->tok.q;
        break;
    }
    texpr->next = copy_expr(expr->next);
  }

  return texpr;
}

int next_file()
{
  INPUT_FILE *temp;
      
  if (ifile == NULL)
    return 0;

  if ((ifile->type == F_FILE) && (ifile->f == NULL)) {
#ifdef UNIX
     { char tmp_flnm[PATH_MAX + 1];
       UNIX_expand_pathname(ifile->name, tmp_flnm);
       ifile->f = fopen(tmp_flnm, "r");
     }
#else
       ifile->f = fopen(ifile->name, "r");
#endif
    if (ifile->f == NULL) {
      errout(E_ERR | E_NO_LNUM, "hal", "%s (file %s)\n", strerror(errno), ifile->name);
      errout(E_ERR | E_NO_LNUM, "hal", "error opening %s for input, aborting.\n", ifile->name);
      return 0;
    }
    if (!quiet)
      errout(E_INFO | E_NO_LNUM, "hal", "opening %s for input.\n", ifile->name);
  }

  if (ifile->buff == NULL) {
    switch(ifile->type) {
      case F_STRING:
      case F_MACRO_PREP:
        if (!macro_prep_buff_free_list) {
          ifile->buff = yy_create_buffer(NULL, PREP_BUF_SIZE);
          yy_switch_to_buffer(ifile->buff);
        } else {
          BUFFER_LIST *temp = macro_prep_buff_free_list;
          macro_prep_buff_free_list = temp->next;
          ifile->buff = temp->buff;

          yy_switch_to_buffer(ifile->buff);
          yyrestart(NULL);

          FREE(temp);
        }
        
        break;
      case F_MACRO:
        if (!macro_buff_free_list) {
          ifile->buff = yy_create_buffer(NULL, MACRO_BUF_SIZE);
          yy_switch_to_buffer(ifile->buff);
        } else {
          BUFFER_LIST *temp = macro_buff_free_list;
          macro_buff_free_list = temp->next;
          ifile->buff = temp->buff;

          yy_switch_to_buffer(ifile->buff);
          yyrestart(NULL);

          FREE(temp);
        }
        break;
      default:
        ifile->buff = yy_create_buffer((ifile->type == F_FILE) ? ifile->f : NULL, YY_BUF_SIZE);
        yy_switch_to_buffer(ifile->buff);
    }
  } else {
    yy_switch_to_buffer(ifile->buff);
  }

  yychar = ifile->yychar;
  yylval = ifile->yylval;
  
  cur_ifile = ifile;
  ifile = ifile->next;

  /* if we just switched to a file, tell HAL which one */
  if (cur_ifile->type == F_FILE) {
    cur_base_lineno = 0;
    if ((cur_fname == NULL) || (strcmp(cur_fname, cur_ifile->name) != 0)) {
      cur_fname = cur_ifile->name;
    }
  }

  /* if we just switched to a macro, tell HAL which file it is in
     and set the line number of the first macro line */
  if (cur_ifile->type == F_MACRO) {
    cur_base_lineno = cur_ifile->base_lineno;
    if ((cur_fname == NULL) || (strcmp(cur_fname, macstk->mdef->fname) != 0)) {
      cur_fname = macstk->mdef->fname;
    }
  }

  /* if we just switched to a STRING, reset base line number */
  if (cur_ifile->type == F_STRING) {
    cur_base_lineno = cur_ifile->base_lineno;
  }

  return 1;
}

int eval_while_cond(TOKEN_LIST* expr)
{
  int reloc_used;
  int if_result = 0;
  TOKEN_LIST* result;

  switch (expr->type) {
  case EQ:
  case NE:
  case LT:
  case GT:
  case LE:
  case GE:
    result = eval_expr(expr->next, &reloc_used);
    if (reloc_used && (result != NULL)) {
      save_absexpr(sym2num(copy_expr(expr->next)), result); 
      qual_absexpr(expr->type, result->tok.q.v[0]);
    } 
    if_result = do_if_condition(expr->type, result, NULL, 0);
    break;
  case DF:
  case NDF:
  case B:
  case NB:
    if_result = do_if_condition(expr->type, expr->next, NULL, 0);
    break;
  case IDN:
  case DIF:
    if_result = do_if_condition(expr->type, expr->next, expr->next->next, 0);
    break;
  }

  return if_result;
}

#ifdef __STDC__
void add_input(char *name, int type, int repeat, TOKEN_LIST *while_cond, int where, int base_lineno, int string_type)
#else
void add_input(name, type, repeat, while_cond, where, base_lineno, string_type)
char *name;
TOKEN_LIST *while_cond;
int type, repeat, base_lineno, where, string_type;
#endif
{
    INPUT_FILE **temp, *temp1;

    switch(type) {
    case F_FILE:
        switch (where) {
        case ADD_PUSH:
            temp1 = ALLOC0(1, INPUT_FILE);
            temp1->next = ifile;
            ifile = temp1;
            break;
        case ADD_APPEND:
            for (temp = &ifile; *temp != NULL;)
                temp = &(*temp)->next;
      
            temp1 = *temp = ALLOC0(1, INPUT_FILE);
            break;
        default:
            errout(E_ERR | E_NO_LNUM, "add_input", "invalid value for where argument (%d).\n", where);
            break;
        }
    
        temp1->type = F_FILE;
        temp1->base_lineno = 0;
        temp1->repeat = repeat;
        temp1->while_cond = while_cond;
        temp1->preprocess_me = PREP_NO;
        temp1->abort = 0;
        temp1->name = name;
        temp1->yychar = YYEMPTY;
        temp1->string_type = string_type; 
        break;
    case F_MACRO:
        switch (where) {
        case ADD_PUSH:
            temp1 = ALLOC0(1, INPUT_FILE);
            temp1->next = ifile;
            ifile = temp1;
            break;
        case ADD_APPEND:
            for (temp = &ifile; *temp != NULL;)
                temp = &(*temp)->next;
      
            temp1 = *temp = ALLOC0(1, INPUT_FILE);
            break;
        default:
            errout(E_ERR | E_NO_LNUM, "add_input", "invalid value for where argument (%d).\n", where);
            break;
        }
    
        temp1->type = type;
        temp1->base_lineno = base_lineno;
        temp1->repeat = repeat;
        temp1->while_cond = while_cond;
        temp1->preprocess_me = PREP_NO;
        temp1->abort = 0;
        temp1->name = NULL;
        temp1->f = NULL;
        temp1->yychar = YYEMPTY;
        break;
    case F_MACRO_PREP:
    case F_STRING:
        switch (where) {
        case ADD_PUSH:
            temp1 = ALLOC0(1, INPUT_FILE);
            temp1->next = ifile;
            ifile = temp1;
            break;
        case ADD_APPEND:
            for (temp = &ifile; *temp != NULL;)
                temp = &(*temp)->next;
      
            temp1 = *temp = ALLOC0(1, INPUT_FILE);
            break;
        default:
            errout(E_ERR | E_NO_LNUM, "add_input", "invalid value for where argument (%d).\n", where);
            break;
        }
    
        temp1->type = type;
        temp1->base_lineno = base_lineno;
        temp1->repeat = repeat;
        temp1->while_cond = while_cond;
        temp1->abort = 0;
        temp1->name = name;
        temp1->string_type = (type == F_MACRO_PREP) ? 0 : string_type;
        if (macstk && ((string_type == REPEAT) || (string_type == WHILE) || (type == F_MACRO_PREP)))
            temp1->preprocess_me = PREP_IT;
        else
            temp1->preprocess_me = PREP_NO;
        temp1->f = (FILE *)temp1->name;
        temp1->yychar = YYEMPTY;
        break;
    default:
        break;
    }
  
    return;
}

void add64(a,b,c)
int32 *a,*b,*c;
{
#ifdef __alpha

  MTV_uint64 c64 = ((((MTV_uint64)(uint32)a[1])<<32) | ((MTV_uint64)(uint32)a[0])) +
                   ((((MTV_uint64)(uint32)b[1])<<32) | ((MTV_uint64)(uint32)b[0]));
  c[1] = (c64>>32) & 0xffffffff;
  c[0] = c64 & 0xffffffff;

#else

 int32 t,t1;
 int32 temp[2];
	
 t = (a[0] & 0xFFFF) + (b[0] & 0xFFFF);
 temp[0] = t & 0xFFFF;

 t1 = ((a[0]>>16) & 0x7FFF) + ((b[0]>>16) & 0x7FFF) + (t >>16);
 t  = ((a[0]>>16) & 0xFFFF) + ((b[0]>>16) & 0xFFFF) + (t >>16);
 temp[0] |= t<<16;

 t = (a[1] & 0xFFFF) + (b[1] & 0xFFFF) + (t >>16);
 temp[1] = t & 0xFFFF;

 t1 = ((a[1]>>16) & 0x7FFF) + ((b[1]>>16) & 0x7FFF) + (t >>16);
 t  = ((a[1]>>16) & 0xFFFF) + ((b[1]>>16) & 0xFFFF) + (t >>16);
 temp[1] |= t<<16;
    
 c[0] = temp[0];
 c[1] = temp[1];
#endif
}

/* op1 and op2 must be NUMBERs */
#ifdef __STDC__
TOKEN_LIST *addq(TOKEN_LIST *op1, TOKEN_LIST *op2)
#else
TOKEN_LIST *addq(op1, op2)
TOKEN_LIST *op1, *op2;
#endif
{
  TOKEN_LIST *temp = alloc_tok();

#ifdef __alpha

  MTV_uint64 c64 = ((((MTV_uint64)(uint32)(op1->tok.q.v[1]))<<32) | ((MTV_uint64)(uint32)(op1->tok.q.v[0]))) +
                   ((((MTV_uint64)(uint32)(op2->tok.q.v[1]))<<32) | ((MTV_uint64)(uint32)(op2->tok.q.v[0])));
  temp->tok.q.v[1] = (c64>>32) & 0xffffffff;
  temp->tok.q.v[0] = c64 & 0xffffffff;

#else

  add64((int32*)op1->tok.q.v, (int32*)op2->tok.q.v, (int32*)temp->tok.q.v);

#endif

  temp->type = NUMBER;
  return temp;
}

void sub64(a,b,c)
int32 *a,*b,*c;
{
#ifdef __alpha

  MTV_uint64 c64 = ((((MTV_uint64)(uint32)(a[1]))<<32) | ((MTV_uint64)(uint32)(a[0]))) -
                   ((((MTV_uint64)(uint32)(b[1]))<<32) | ((MTV_uint64)(uint32)(b[0])));
  c[1] = (c64>>32) & 0xffffffff;
  c[0] = c64 & 0xffffffff;

#else

  int32 t,t1;
  int32 bb[2];
  int32 temp[2];
  
  bb[0] = b[0] ^ 0xFFFFFFFF;
  bb[1] = b[1] ^ 0xFFFFFFFF;
  
  t = (a[0] & 0xFFFF) + (bb[0] & 0xFFFF) +1;
  temp[0] = t & 0xFFFF;
  
  t1 = ((a[0]>>16) & 0x7FFF) + ((bb[0]>>16) & 0x7FFF) + (t >>16);
  t = ((a[0]>>16) & 0xFFFF) + ((bb[0]>>16) & 0xFFFF) + (t >>16);
  temp[0] |= t<<16;
  
  t = (a[1] & 0xFFFF) + (bb[1] & 0xFFFF) + (t >>16);
  temp[1] = t & 0xFFFF;
  
  t1 = ((a[1]>>16) & 0x7FFF) + ((bb[1]>>16) & 0x7FFF) + (t >>16);
  t = ((a[1]>>16) & 0xFFFF) + ((bb[1]>>16) & 0xFFFF) + (t >>16);
  temp[1] |= t<<16;
  
  c[0] = temp[0];
  c[1] = temp[1];
#endif  
}

/* op1 and op2 must be NUMBERs */
#ifdef __STDC__
TOKEN_LIST *subq(TOKEN_LIST *op1, TOKEN_LIST *op2)
#else
TOKEN_LIST *subq(op1, op2)
TOKEN_LIST *op1, *op2;
#endif
{
  TOKEN_LIST *temp = alloc_tok();

#ifdef __alpha

  MTV_uint64 c64 = ((((MTV_uint64)(uint32)(op1->tok.q.v[1]))<<32) | ((MTV_uint64)(uint32)(op1->tok.q.v[0]))) -
                   ((((MTV_uint64)(uint32)(op2->tok.q.v[1]))<<32) | ((MTV_uint64)(uint32)(op2->tok.q.v[0])));
  temp->tok.q.v[1] = (c64>>32) & 0xffffffff;
  temp->tok.q.v[0] = c64 & 0xffffffff;

#else

  sub64(op1->tok.q.v, op2->tok.q.v, temp->tok.q.v);

#endif

  temp->type = NUMBER;
  return temp;
}

/* UNSIGNED 64 bit number times 32 bit number -> 64bit number.*/
#ifdef __STDC__
void mul64(uint32 *a, uint32 b)
#else
void mul64(a, b)
uint32 *a, b;
#endif
{
#ifdef __alpha

  MTV_uint64 c64 = ((((MTV_uint64)(uint32)(a[1]))<<32) | ((MTV_uint64)(uint32)(a[0]))) * ((MTV_uint64)(uint32)b);
  a[1] = (c64>>32) & 0xffffffff;
  a[0] = c64 & 0xffffffff;

#else
  uint32 d1, d2, d3, d4, i1, i2;
  uint32 temp[2], junk;
  uint32 result[2];

  i1 = b >> 16;
  i2 = b & 0xFFFF;

  d1 = a[1] >> 16;
  d2 = a[1] & 0xFFFF;
  d3 = a[0] >> 16;
  d4 = a[0] & 0xFFFF;

  result[0] = result[1] = 0;

  result[0] = i2 * d4;

  junk = i2 * d3;
  temp[1] = junk >> 16;
  temp[0] = (junk & 0xFFFF) << 16;
  add64((int32*)result, (int32*)temp, (int32*)result);

  temp[0] = 0;
  temp[1] = i2 * d2;
  add64((int32*)result, (int32*)temp, (int32*)result);

  junk = i2 * d1;
  temp[0] = 0;
  temp[1] = (junk & 0xFFFF) << 16;
  add64((int32*)result, (int32*)temp, (int32*)result);

  junk = i1 * d4;
  temp[1] = junk >> 16;
  temp[0] = (junk & 0xFFFF) << 16;
  add64((int32*)result, (int32*)temp, (int32*)result);

  temp[0] = 0;
  temp[1] = i1 * d3;
  add64((int32*)result, (int32*)temp, (int32*)result);

  junk = i1 * d2;
  temp[0] = 0;
  temp[1] = (junk & 0xFFFF) << 16;
  add64((int32*)result, (int32*)temp, (int32*)result);

  a[1] = result[1];
  a[0] = result[0];
#endif
}

/* op1 and op2 must be NUMBERs */
#ifdef __STDC__
TOKEN_LIST *mulq(TOKEN_LIST *op1, TOKEN_LIST *op2)
#else
TOKEN_LIST *mulq(op1, op2)
TOKEN_LIST *op1, *op2;
#endif
{
  TOKEN_LIST *temp = alloc_tok();

#ifdef __alpha

  MTV_uint64 c64 = ((((MTV_uint64)(uint32)(op1->tok.q.v[1]))<<32) | ((MTV_uint64)(uint32)(op1->tok.q.v[0]))) *
                   ((((MTV_uint64)(uint32)(op2->tok.q.v[1]))<<32) | ((MTV_uint64)(uint32)(op2->tok.q.v[0])));
  temp->tok.q.v[1] = (c64>>32) & 0xffffffff;
  temp->tok.q.v[0] = c64 & 0xffffffff;

#else

  QUAD_T t;

  temp->tok.q.v[0] = op1->tok.q.v[0];
  temp->tok.q.v[1] = op1->tok.q.v[1];
  t.v[0] = op1->tok.q.v[0];
  t.v[1] = op1->tok.q.v[1];

  mul64(&temp->tok.q.v[0], op2->tok.q.v[0]);
  mul64(&t.v[0], op2->tok.q.v[1]);
  temp->tok.q.v[1] =  temp->tok.q.v[1] + t.v[0];

#endif

  temp->type = NUMBER;
  return temp;
}

/* op1 and op2 must be NUMBERs */
#ifdef __STDC__
TOKEN_LIST *divq(TOKEN_LIST *op1, TOKEN_LIST *op2)
#else
TOKEN_LIST *divq(op1, op2)
TOKEN_LIST *op1, *op2;
#endif
{
  TOKEN_LIST *result = alloc_tok();
#ifdef __alpha

  MTV_uint64 c64 = ((((MTV_uint64)(uint32)(op1->tok.q.v[1]))<<32) | ((MTV_uint64)(uint32)(op1->tok.q.v[0]))) /
                   ((((MTV_uint64)(uint32)(op2->tok.q.v[1]))<<32) | ((MTV_uint64)(uint32)(op2->tok.q.v[0])));
  result->tok.q.v[1] = (c64>>32) & 0xffffffff;
  result->tok.q.v[0] = c64 & 0xffffffff;

#else

  unsigned int divresult[2],modresult[2],a[2],b[2];

  a[0] = op1->tok.q.v[0];
  a[1] = op1->tok.q.v[1];
  b[0] = op2->tok.q.v[0];
  b[1] = op2->tok.q.v[1];

  NN_Div(divresult, modresult, a, 2, b, 2);
  
  result->tok.q.v[0] = divresult[0];
  result->tok.q.v[1] = divresult[1];

#endif
  
  result->type = NUMBER;
  return result;
}

/* op1 must be a NUMBER */
#ifdef __STDC__
TOKEN_LIST *notq(TOKEN_LIST *op1)
#else
TOKEN_LIST *notq(op1)
TOKEN_LIST *op1;
#endif
{
  TOKEN_LIST *temp = alloc_tok();
  temp->type = NUMBER;

  temp->tok.q.v[0] = ~op1->tok.q.v[0];
  temp->tok.q.v[1] = ~op1->tok.q.v[1];

  return temp;
}

/* op1 must be a NUMBER */
#ifdef __STDC__
TOKEN_LIST *log_notq(TOKEN_LIST *op1)
#else
TOKEN_LIST *log_notq(op1)
TOKEN_LIST *op1;
#endif
{
  TOKEN_LIST *temp = alloc_tok();
  temp->type = NUMBER;

  temp->tok.q.v[0] = !(op1->tok.q.v[0] | op1->tok.q.v[1]);
  temp->tok.q.v[1] = 0;

  return temp;
}

/* op1 and op2 must be NUMBERs */
#ifdef __STDC__
TOKEN_LIST *shiftq(TOKEN_LIST *op1, TOKEN_LIST *op2)
#else
TOKEN_LIST *shiftq(op1, op2)
TOKEN_LIST *op1, *op2;
#endif
{
  TOKEN_LIST *temp = alloc_tok();
  int32 num = (int32)op2->tok.q.v[0];

#ifdef __alpha
  MTV_uint64 c64;

  if (num >=0)
    c64 = ((((MTV_uint64)(uint32)(op1->tok.q.v[1]))<<32) | ((MTV_uint64)(uint32)(op1->tok.q.v[0]))) << num;
  else
    c64 = ((((MTV_uint64)(uint32)(op1->tok.q.v[1]))<<32) | ((MTV_uint64)(uint32)(op1->tok.q.v[0]))) >> -num;

  temp->tok.q.v[1] = (c64>>32) & 0xffffffff;
  temp->tok.q.v[0] = c64 & 0xffffffff;

#else


  temp->tok.q.v[1] = op1->tok.q.v[1];
  temp->tok.q.v[0] = op1->tok.q.v[0];

  if ((num != 0) && (num <= 63)) {
    /* do the shift, this is REALLY inefficient, but easy */
    int dir = 1, i;

    if (num < 0) {
      dir = 0;
      num = num * -1;
    }

    for (i = 0; i < num; ++i) {
      if (dir) {
	int32 out = temp->tok.q.v[0] >> 31;

	temp->tok.q.v[1] = (temp->tok.q.v[1] << 1) | out;
	temp->tok.q.v[0] = (temp->tok.q.v[0] << 1);
      } else {
	int32 out = temp->tok.q.v[1] << 31;

	temp->tok.q.v[1] = (temp->tok.q.v[1] >> 1);
	temp->tok.q.v[0] = (temp->tok.q.v[0] >> 1) | out;
      }
    }
  }

#endif

  temp->type = NUMBER;
  return temp;
}

/* op1 and op2 must be NUMBERs */
#ifdef __STDC__
TOKEN_LIST *andq(TOKEN_LIST *op1, TOKEN_LIST *op2)
#else
TOKEN_LIST *andq(op1, op2)
TOKEN_LIST *op1, *op2;
#endif
{
  TOKEN_LIST *temp = alloc_tok();
  temp->type = NUMBER;

  temp->tok.q.v[0] = op1->tok.q.v[0] & op2->tok.q.v[0];
  temp->tok.q.v[1] = op1->tok.q.v[1] & op2->tok.q.v[1];

  return temp;
}

/* op1 and op2 must be NUMBERs */
#ifdef __STDC__
TOKEN_LIST *log_andq(TOKEN_LIST *op1, TOKEN_LIST *op2)
#else
TOKEN_LIST *log_andq(op1, op2)
TOKEN_LIST *op1, *op2;
#endif
{
  TOKEN_LIST *temp = alloc_tok();
  temp->type = NUMBER;

  temp->tok.q.v[0] = (op1->tok.q.v[0] || op1->tok.q.v[1]) && (op2->tok.q.v[0] || op2->tok.q.v[1]);
  temp->tok.q.v[1] = 0;

  return temp;
}

/* op1 must be NUMBER */
#ifdef __STDC__
int sign_ext_ok(TOKEN_LIST *op1, int sign_bit)
#else
int sign_ext_ok(op1, sign_bit)
TOKEN_LIST *op1;
int sign_bit;
#endif
{
  /* if everything above sign bit is 0 then its ok  -- let something like "ldq r0,^xffff(r31)" still work*/
  if ((sign_bit<=31 && !((op1->tok.q.v[0]>>(sign_bit+1)) || op1->tok.q.v[1])) ||
      (sign_bit>31 && !(op1->tok.q.v[1]>>(sign_bit-31))))                                                                      
    return(1);

  /* otherwise check that all bits above highest is same as highest */
  if ((sign_bit<=31 && op1->tok.q.v[1]==0xffffffffu && (op1->tok.q.v[0]>>sign_bit)==(0xffffffffu>>sign_bit)) ||
      (sign_bit>31 && (op1->tok.q.v[1]>>(sign_bit-32))==(0xffffffffu>>(sign_bit-32))))
    return(1);
  else
    return(0);
}

/* op1 and op2 must be NUMBERs */
#ifdef __STDC__
TOKEN_LIST *orq(TOKEN_LIST *op1, TOKEN_LIST *op2)
#else
TOKEN_LIST *orq(op1, op2)
TOKEN_LIST *op1, *op2;
#endif
{
  TOKEN_LIST *temp = alloc_tok();
  temp->type = NUMBER;

  temp->tok.q.v[0] = op1->tok.q.v[0] | op2->tok.q.v[0];
  temp->tok.q.v[1] = op1->tok.q.v[1] | op2->tok.q.v[1];

  return temp;
}

/* op1 and op2 must be NUMBERs */
#ifdef __STDC__
TOKEN_LIST *log_orq(TOKEN_LIST *op1, TOKEN_LIST *op2)
#else
TOKEN_LIST *log_orq(op1, op2)
TOKEN_LIST *op1, *op2;
#endif
{
  TOKEN_LIST *temp = alloc_tok();
  temp->type = NUMBER;

  temp->tok.q.v[0] = op1->tok.q.v[0] || op2->tok.q.v[0] || op1->tok.q.v[1] || op2->tok.q.v[1];
  temp->tok.q.v[1] = 0;

  return temp;
}

/* op1 and op2 must be NUMBERs */
#ifdef __STDC__
TOKEN_LIST *xorq(TOKEN_LIST *op1, TOKEN_LIST *op2)
#else
TOKEN_LIST *xorq(op1, op2)
TOKEN_LIST *op1, *op2;
#endif
{
  TOKEN_LIST *temp = alloc_tok();
  temp->type = NUMBER;

  temp->tok.q.v[0] = op1->tok.q.v[0] ^ op2->tok.q.v[0];
  temp->tok.q.v[1] = op1->tok.q.v[1] ^ op2->tok.q.v[1];

  return temp;
}

static void print_location_line(INPUT_FILE* tfil, char lchar, char* rtn, MACRO_STK** mstk)
{
    char* block_type;
    
    switch(tfil->type) {
    case F_FILE:
        /* check to see if file has been used yet. */
        if (tfil->lineno == 0)
            break;

        fprintf(stderr, "%%%c-%s, on line %d of file %s.\n", lchar, rtn,
                tfil->lineno, tfil->name);
        break; 
    case F_MACRO:
        fprintf(stderr, "%%%c-%s, in macro %s on line %d of file %s.\n", lchar, rtn,
                (*mstk)->mdef->name, tfil->lineno + tfil->base_lineno,
                (*mstk)->mdef->fname);
        *mstk = (*mstk)->next;
        break;
    case F_MACRO_PREP:
        fprintf(stderr, "%%%c-%s, while preprocessing macro %s on line %d of file %s.\n", lchar, rtn,
                (*mstk)->mdef->name, tfil->lineno + tfil->base_lineno,
                (*mstk)->mdef->fname);
        /* we are going to skip the F_MACRO file because we printed everything here. */
        *mstk = (*mstk)->next;
        break;
    case F_STRING:
        switch(tfil->string_type) {
        case IF: block_type = ".if block"; break;
        case IFF: block_type = ".if_false block"; break;
        case IFT: block_type = ".if_true block"; break;
        case IFTF: block_type = ".if_true_false block"; break;
        case IIF: block_type = ".iif statement"; break;
        case REPEAT: block_type = ".repeat block"; break;
        case WHILE: block_type = ".while block"; break;
        default: block_type = "<unknown>"; break;
        }
        if (tfil->string_type == 0)
            fprintf(stderr, "%%%c-%s, in command line string:\"%s\".\n", lchar, rtn,
                    tfil->name);
        else
            fprintf(stderr, "%%%c-%s, in %s on line %d of file %s.\n", lchar, rtn,
                    block_type, tfil->lineno + tfil->base_lineno, find_fname(tfil));
        break;
    }
    return;
}

#ifndef __STDC__
void errout(level, rtn, s, va_alist)
int level;
char *rtn, *s;
va_dcl
#else
void errout(int level, char *rtn, char *s, ...)
#endif
{
    char temp[2048], lchar;
    va_list the_args;
  
    if (quiet && ((level & ~E_NO_LNUM) == E_INFO))
        return;

#ifndef __STDC__
    va_start(the_args);
#else
    va_start(the_args, s);
#endif

    switch (level & ~E_NO_LNUM) {
    case E_INFO:
        lchar = 'I';
        break;
    case E_WARN:
        num_warnings++;
        lchar = 'W';
        break;
    case E_ERR:
        num_errors++;
        lchar = 'E';
        break;
    default:
        lchar = '?';
        break;
    }

    sprintf(temp, "%%%%%c-%s, %s", lchar, rtn, s);
    vfprintf(stderr, temp, the_args);

    if (!(level & E_NO_LNUM)) {
        MACRO_STK *mstk = macstk;
        INPUT_FILE* tfil = cur_ifile->next;

        /* do cur_ifile, then the entire input file stack */
        print_location_line(cur_ifile, lchar, rtn, &mstk);

        if ((tfil && (tfil->type == F_FILE) &&
             (cur_ifile->type == F_STRING)) ||
            (cur_ifile->type == F_MACRO_PREP))
            tfil = tfil->next;

        for (; tfil != NULL; tfil = tfil->next) {
            print_location_line(tfil, lchar, rtn, &mstk);

            if ((tfil->next && (tfil->next->type == F_FILE) &&
                 (tfil->type == F_STRING)) ||
                (tfil->type == F_MACRO_PREP))
                tfil = tfil->next;
        }

        fprintf(stderr, "\n");
    }

    return;
}

static void add_location(LINELOC** lloc, INPUT_FILE* tfil, MACRO_STK** mstk)
{
    char* block_type;
    LINELOC* tloc = ALLOC(LINELOC);

    tloc->type = tfil->type;
    tloc->next = NULL;
    
    switch(tfil->type) {
    case F_FILE:
        tloc->lineno = tfil->lineno;
        tloc->fname = tfil->name;
        break; 
    case F_MACRO:
    case F_MACRO_PREP:
        tloc->lineno = tfil->lineno + tfil->base_lineno;
        tloc->fname = (*mstk)->mdef->fname;
        tloc->name = (*mstk)->mdef->name;
        *mstk = (*mstk)->next;
        break;
    case F_STRING:
        tloc->lineno = tfil->lineno + tfil->base_lineno;
        tloc->string_type = tfil->string_type;
        tloc->name = tfil->name;
        tloc->fname = find_fname(tfil);
        break;
    }

    *lloc = tloc;
    return;
}

LINELOC* save_location()
{
    MACRO_STK *mstk = macstk;
    INPUT_FILE* tfil = cur_ifile->next;

    LINELOC* tloc = NULL;
    LINELOC** useloc = &tloc;
    
    /* do cur_ifile, then the entire input file stack */
    add_location(useloc, cur_ifile, &mstk);

    if ((tfil && (tfil->type == F_FILE) &&
         (cur_ifile->type == F_STRING)) ||
        (cur_ifile->type == F_MACRO_PREP))
        tfil = tfil->next;

    for (; tfil != NULL; tfil = tfil->next) {
        useloc = &(*useloc)->next;
        add_location(useloc, tfil, &mstk);

        if ((tfil->next && (tfil->next->type == F_FILE) &&
             (tfil->type == F_STRING)) ||
            (tfil->type == F_MACRO_PREP))
            tfil = tfil->next;
    }

    return tloc;
}

void print_location(int level, char* rtn, LINELOC* lloc)
{
    char lchar;
    
    switch (level & ~E_NO_LNUM) {
    case E_INFO:
        lchar = 'I';
        break;
    case E_WARN:
        lchar = 'W';
        break;
    case E_ERR:
        lchar = 'E';
        break;
    default:
        lchar = '?';
        break;
    }

    while (lloc != NULL) {
        char* block_type;
    
        switch(lloc->type) {
        case F_FILE:
            /* check to see if file has been used yet. */
            if (lloc->lineno == 0)
                break;

            fprintf(stderr, "%%%c-%s, on line %d of file %s.\n", lchar, rtn,
                    lloc->lineno, lloc->fname);
            break; 
        case F_MACRO:
            fprintf(stderr, "%%%c-%s, in macro %s on line %d of file %s.\n", lchar, rtn,
                    lloc->name, lloc->lineno, lloc->fname);
            break;
        case F_MACRO_PREP:
            fprintf(stderr, "%%%c-%s, while preprocessing macro %s on line %d of file %s.\n", lchar, rtn,
                    lloc->name, lloc->lineno, lloc->fname);
            break;
        case F_STRING:
            switch(lloc->string_type) {
            case IF: block_type = ".if block"; break;
            case IFF: block_type = ".if_false block"; break;
            case IFT: block_type = ".if_true block"; break;
            case IFTF: block_type = ".if_true_false block"; break;
            case IIF: block_type = ".iif statement"; break;
            case REPEAT: block_type = ".repeat block"; break;
            default: block_type = "<unknown>"; break;
            }
            if (lloc->string_type == 0)
                fprintf(stderr, "%%%c-%s, in command line string:\"%s\".\n", lchar, rtn,
                        lloc->name);
            else
                fprintf(stderr, "%%%c-%s, in %s on line %d of file %s.\n", lchar, rtn,
                        block_type, lloc->lineno, lloc->fname);
            break;
        }

        lloc = lloc->next;
    }
    
    fprintf(stderr, "\n");
    
    return;
}
            
void free_location(LINELOC* lloc)
{
    while (lloc != NULL) {
        LINELOC* next_lloc = lloc->next;
        
        FREE(lloc);

        lloc = next_lloc;
    }
}

void free_rnode(RNODE* r)
{
    if (r == NULL)
        return;
    
    /*
     * sect will be freed when freeing the section tables.
     */
    free_location(r->lloc);
    FREE(r);
}

int in_body(int type)
{
    INPUT_FILE *tfil;

    if ((cur_ifile->type == F_STRING) && (cur_ifile->string_type == type))
        return 1;

    for (tfil=ifile; tfil != NULL; tfil = tfil->next)
        if ((tfil->type == F_STRING) && (tfil->string_type == type))
            return 1;

    return 0;        
}

char *find_fname(INPUT_FILE* start_file)
{
    INPUT_FILE *tfil = start_file;

    if (tfil == NULL) {
        if (cur_ifile->type == F_FILE)
            return cur_ifile->name;
        
        if ((cur_ifile->type == F_MACRO) || (cur_ifile->type == F_MACRO_PREP))
            return macstk->mdef->fname;

        tfil = cur_ifile->next;
    }
  
    for (; tfil != NULL; tfil = tfil->next) {
        if ((tfil->type == F_FILE) && (tfil->f != NULL))
            return tfil->name;
    
        if ((tfil->type == F_MACRO) || (tfil->type == F_MACRO_PREP))
            return macstk->mdef->fname;
    }

    return NULL;
}

#ifndef __STDC__
void dbgout(s, va_alist)
char *s;
va_dcl
#else
void dbgout(char *s, ...)
#endif
{
    va_list the_args;
  
    if (!Trace)
        return;

#ifndef __STDC__
    va_start(the_args);
#else
    va_start(the_args, s);
#endif

    vfprintf(Ot, s, the_args);

    return;
}

