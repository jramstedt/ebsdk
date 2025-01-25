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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#ifdef _POSIX_C_SOURCE
#include <sys/stat.h>
#endif

#include "hal.h"
#include "operator.h"

#ifdef _WIN32
#include <malloc.h>
#define alloca _alloca
#else
#ifdef VMS
/* for lib$spawn in sys() */
#include <descrip.h>
#include <ssdef.h>
#else
#include <sys/wait.h>
#endif
#endif /* _WIN32 */

int local_label;

MACRO_STK* macstk;

TOKEN_LIST *cur_psect;
PSECT_LIST *psect_stk;

#ifdef __STDC__
void get_tmpnam(char *storage)
#else
void get_tmpnam(storage)
char *storage;
#endif
{
    tmpnam(storage);
    strcat(storage,".tmp"); 
}

#ifdef __STDC__
int do_include(TOKEN_LIST *name_tok)
#else
int do_include(name_tok)
TOKEN_LIST *name_tok;
#endif
{
  char *name;
  INPUT_FILE *temp;
  int status=1;
  FILE *ftmp;

  /* find the file first by going through directories (if any) */
  /* but first, always just try the filename and see if it just works by itself */
  /* This implies that HAL always looks at current directory first before trying other directories */
  name = strcpy(ALLOC_STR(strlen(name_tok->tok.s)),name_tok->tok.s);
#ifdef UNIX
  {
      char tmp_flnm[PATH_MAX + 1];
      UNIX_expand_pathname(name, tmp_flnm);
      ftmp = fopen(tmp_flnm, "r");
  }
#else
  ftmp = fopen(name, "r");
#endif
  if (ftmp) 
    fclose(ftmp);
  else { /* file not found */
    int i=0;
    if (idirs) {
      for(i=0; idirs[i]; ++i) {
        FREE(name);
        name = ALLOC_STR(strlen(name_tok->tok.s) + strlen(idirs[i]));
        sprintf(name, "%s%s", idirs[i], name_tok->tok.s);
#ifdef UNIX
        {
          char tmp_flnm[PATH_MAX + 1];
          UNIX_expand_pathname(name, tmp_flnm);
          ftmp = fopen(tmp_flnm, "r");
        }
#else
        ftmp = fopen(name, "r");
#endif
        if (ftmp) {
          fclose(ftmp);
          break;  /* break out of for loop */
        }
      }
    }
    if (!idirs || !idirs[i]) {/* if no directory name worked, error */
      errout(E_ERR, "do_include", "unable to open include file %s, include command ignored.\n", name_tok->tok.s);
      status = 0;
    }
  }

  if (status) {
    /* push cur_ifile back on ifile stack */
    push_current_ifile();
    
    /* create INPUT_FILE struct for this include file on ifile stack */
    add_input(name, F_FILE, 1, NULL, ADD_PUSH, 0, 0);
    
    /* call next_file() to get input file */
    status = next_file();
    
    /* if we failed to do the include, pop the input we just added, and
     * bring back the old.
     */
    if (!status) {
      ifile = ifile->next;
      next_file();
    }
  }
  
  
  free_tok_s(name_tok);

  return status;
}

#ifdef __STDC__
int do_include_dir(TOKEN_LIST *name_tok)
#else
int do_include_dir(name_tok)
TOKEN_LIST *name_tok;
#endif
{
  char *name = name_tok->tok.s;

  if (!idirs) {
    idirs = ALLOC0(2,char*);
    idirs[0] = strcpy(ALLOC_STR(strlen(name)),name);
    idirs[1] = NULL;
  }
  else {
    int i,j;
    char **new_idirs;
    for (i=1; idirs[i]; ++i)
      ;
    new_idirs = ALLOC0(i+2,char*);
    for (j=0; idirs[j]; ++j)
      new_idirs[j] = idirs[j];
    new_idirs[j] = strcpy(ALLOC_STR(strlen(name)),name);
    new_idirs[j+1] = NULL;
    FREE(idirs);
    idirs = new_idirs;
  }
  
  free_tok_s(name_tok);

  return 1;
}

#ifdef __STDC__
int do_minclude(TOKEN_LIST *name_tok)
#else
int do_minclude(name_tok)
TOKEN_LIST *name_tok;
#endif
{
  FILE *m_file;
  int status;
  
#ifdef UNIX
  {
      char tmp_flnm[PATH_MAX + 1];
      UNIX_expand_pathname(name_tok->tok.s, tmp_flnm);
      m_file = fopen(tmp_flnm, "r");
  }
#else
  m_file = fopen(name_tok->tok.s, "r");
#endif
  if (m_file == NULL) {
      errout(E_ERR, "do_minclude", "%s (file %s)\n", strerror(errno), name_tok->tok.s);
      errout(E_ERR, "do_minclude", "unable to open macro configuration file %s, restoring disabled..\n", name_tok->tok.s);
      status = 0;
  } else {
      errout(E_INFO | E_NO_LNUM, "hal", "opening configuration file %s.\n", name_tok->tok.s);
      rest_macro(m_file);
      status = 1;
  }

  free_tok_s(name_tok);
  return status;
}

#ifdef __STDC__
int do_if_condition(int cond, TOKEN_LIST* expr1, TOKEN_LIST* expr2, int cleanup)
#else
int do_if_condition(cond, expr1, expr2, cleanup)
int cond;
TOKEN_LIST *expr1, *expr2;
int cleanup;
#endif
{
    int is_true;
    
    switch (cond) {
    case EQ:
        is_true = COND_EQ(expr1);
        break;
    case GT:
        is_true = COND_GT(expr1);
        break;
    case LT:
        is_true = COND_LT(expr1);
        break;
    case NE:
        is_true = !COND_EQ(expr1);
        break;
    case GE:
        is_true = (COND_EQ(expr1) || COND_GT(expr1));
        break;
    case LE:
        is_true = (COND_EQ(expr1) || COND_LT(expr1));
        break;
    case DF:
        is_true = avl_lookup(SymTable, expr1->tok.s, NULL);
        break;
    case NDF:
        is_true = !avl_lookup(SymTable, expr1->tok.s, NULL);
        break;
    case B:
        if (expr1 == NULL)
            is_true = 1;
        else {
            is_true = (*expr1->tok.s == '\0');
        }
        break;
    case NB:
        if (expr1 == NULL)
            is_true = 0;
        else {
            is_true = (*expr1->tok.s != '\0');
        }
        break;
    case IDN:
        if ((expr1 == NULL) && (expr2 == NULL))
            /* both args blank, that makes IDN true. */
            is_true = 1;
        else
            if ((expr1 != NULL) && (expr2 != NULL)) {
                /* both args non-blank, IDN is true if the values are same */
                is_true = (strcmp(expr1->tok.s, expr2->tok.s) == 0);
            } else
                /* otherwise one blank, one not.  IDN is false. */
                is_true = 0;
        break;
    case DIF:
        if ((expr1 == NULL) && (expr2 == NULL))
            /* both args blank, that makes DIF false. */
            is_true = 0;
        else
            if ((expr1 != NULL) && (expr2 != NULL)) {
                /* both args non-blank, DIF is true if values are different. */
                is_true = (strcmp(expr1->tok.s, expr2->tok.s) != 0);
            } else
                /* one arg blank, one not, DIF is true. */
                is_true = 1;
        break;

    }

    if (cleanup) {
      switch (cond) {
      case DF:
      case NDF:
      case B:
      case NB:
      case IDN:
      case DIF:
        free_tok_s(expr1);
        free_tok_s(expr2);
        break;
      default:
        free_tok(expr1);
        free_tok(expr2);
        break;
      }
    }
    
    return is_true;
}

/*
 * Align pc to requested granularity, possibly filling intervening space
 * with longword given by fill (otherwise filled with 0).
 *
 * It is assumed that bound and fill are both absolute expressions, and
 * therefore must be single tokens (not a list of tokens).
 */
void do_align(TOKEN_LIST* bound, TOKEN_LIST* fill)
{
    assert(bound->next == NULL);

    if (fill == NULL)
        apcf(bound->tok.q.v[0], 0);
    else {
        assert(fill->next == NULL);
        
        apcf(bound->tok.q.v[0], fill->tok.q.v[0]);
        
        free_tok(fill);
    }

    free_tok(bound);
    
    
    return;
}

#ifdef __STDC__
void do_genmsg(int type, TOKEN_LIST *exp, TOKEN_LIST *cmt)
#else
void do_genmsg(type, exp, cmt)
int type;
TOKEN_LIST *exp, *cmt;
#endif
{
    /* lose the leading ";" */
    char *text = cmt->tok.s + 1;

    if (exp != NULL) {
        /*
         * If an expression was specified, print it's value.
         *
         * The expression is assumed to be an absolute expression.
         */
        assert(exp->next == NULL);
        
        switch (type) {
        case WARN:
            errout(E_WARN, "hal", "Generated WARNING: ^x%08X%08X %s\n", exp->tok.q.v[1], exp->tok.q.v[0], text);
            break;
        case ERROR:
            errout(E_ERR, "hal", "Generated ERROR: ^x%08X%08X %s\n", exp->tok.q.v[1], exp->tok.q.v[0], text);
            break;
        case PRINT:
            if (print_no_trace)
              errout(E_INFO | E_NO_LNUM, "hal", "Generated PRINT: ^x%08X%08X %s\n", exp->tok.q.v[1], exp->tok.q.v[0], text);
            else
              errout(E_INFO, "hal", "Generated PRINT: ^x%08X%08X %s\n", exp->tok.q.v[1], exp->tok.q.v[0], text);
            break;
        default:
            errout(E_ERR, "do_genmsg", "internal error: unknown message type (%d).\n", type);
            break;
        }

        free_tok(exp);
    } else {
        switch (type) {
        case WARN:
            errout(E_WARN, "hal", "Generated WARNING: %s\n", text);
            break;
        case ERROR:
            errout(E_ERR, "hal", "Generated ERROR: %s\n", text);
            break;
        case PRINT:
            if (print_no_trace)
              errout(E_INFO | E_NO_LNUM, "hal", "Generated PRINT: %s\n", text);
            else
              errout(E_INFO, "hal", "Generated PRINT: %s\n", text);
            break;
        default:
            errout(E_ERR, "do_genmsg", "internal error: unknown message type (%d).\n", type);
            break;
        }
    }

    free_tok_s(cmt);

    return;    
}

#ifdef __STDC__
void do_psect(TOKEN_LIST *name, TOKEN_LIST *args, int entry_type)
#else
void do_psect(name, args, entry_type)
TOKEN_LIST *name, *args;
int entry_type;
#endif
{
    int align = 0;

    if (args) {
        assert(args->next == NULL);
        
        align = args->tok.q.v[0];

        free_tok(args);
    }
    
    set_psect(name->tok.s, align, entry_type);

    free_tok_s(cur_psect);
    cur_psect = name;
    
    return;
}

void do_header()
{
    TOKEN_LIST *name;
    
    if (Cpsect->entry_type == PENTRY_TYPE_DSECT_HDR)
        {
            errout(E_WARN, "do_header", "nested .header directives for dsect %s -- action ignored\n",Cpsect->name);
            return;
        }
    else if (Cpsect->entry_type != PENTRY_TYPE_DSECT)
        {
            errout(E_WARN, "do_header", ".header directive is illegal for a non-dsect definition, it will be ignored\n");
            return;
        }
    else
        {
            /* Build dummy name of .header section, dsect_name%hdr */
            char* temp = alloca(strlen(Cpsect->name)+4+1);

            sprintf(temp,"%s%%hdr",Cpsect->name);
            name = make_token(temp, SYMBOL);
                
            set_psect(temp, 0, PENTRY_TYPE_DSECT_HDR);

            free_tok_s(cur_psect);
            cur_psect = name;
        }
    return;
}

#ifdef __STDC__
void do_link_address(TOKEN_LIST *addr_tok)
#else
void do_link_address(addr_tok)
TOKEN_LIST *addr_tok;
#endif
{
    
    if (Cpsect->entry_type != PENTRY_TYPE_DSECT) {
        errout(E_WARN, "do_link_address", ".link_address directive is illegal for non-dsect %s, it will be ignored\n",Cpsect->name);
        return;
    }
    if (Cpsect->link_after != 0) {
        errout(E_WARN, "do_link_address", ".link_address directive is overriding previous .link_after directive for dsect %s\n",
               Cpsect->name);
        Cpsect->link_after = 0;
    }

    Cpsect->base.ulong[0] = addr_tok->tok.q.v[0];
    Cpsect->base.ulong[1] = addr_tok->tok.q.v[1];
    Cpsect->link_address = 1;

    free_tok(addr_tok);

}

#ifdef __STDC__
void do_link_offset(TOKEN_LIST *addr_tok)
#else
void do_link_offset(addr_tok)
TOKEN_LIST *addr_tok;
#endif
{
    Cpsect->offset.ulong[0] = addr_tok->tok.q.v[0];
    Cpsect->offset.ulong[1] = addr_tok->tok.q.v[1];

    free_tok(addr_tok);

}

#ifdef __STDC__
void do_link_size(TOKEN_LIST *sz_tok)
#else
void do_link_size(sz_tok)
TOKEN_LIST *sz_tok;
#endif
{
    Cpsect->link_size.ulong[0] = sz_tok->tok.q.v[0];
    Cpsect->link_size.ulong[1] = sz_tok->tok.q.v[1];
    free_tok(sz_tok);

}

#ifdef __STDC__
void do_link_region(TOKEN_LIST *rgn_tok)
#else
void do_link_region(rgn_tok)
TOKEN_LIST *rgn_tok;
#endif
{
    if (Cpsect->entry_type != PENTRY_TYPE_DSECT) {
        errout(E_WARN, "do_link_region", ".link_region directive is illegal for non-dsect %s, it will be ignored\n",Cpsect->name);
        return;
    }
    
    Cpsect->mem_region = rgn_tok->tok.q.v[0];
    free_tok(rgn_tok);

}

#ifdef __STDC__
void do_link_attr(int attr)
#else
void do_link_attr(attr)
int attr;
#endif
{
    
    if (Cpsect->entry_type != PENTRY_TYPE_DSECT) {
        errout(E_WARN, "do_link_attr", ".link_attribute directive is illegal for non-dsect %s, it will be ignored\n",Cpsect->name);
        return;
    }

    Cpsect->link_attr = attr;

}

#ifdef __STDC__
void do_link_after(TOKEN_LIST *name_tok)
#else
void do_link_after(name_tok)
TOKEN_LIST *name_tok;
#endif
{

    if (Cpsect->entry_type != PENTRY_TYPE_DSECT) {
        errout(E_WARN, "do_link_after", ".link_after directive is illegal for non-dsect %s, it will be ignored\n", Cpsect->name);
        return;
    }
    if (Cpsect->link_address != 0) {
        errout(E_WARN, "do_link_after", ".link_after directive is overriding previous .link_address directive for dsect %s\n",
               Cpsect->name);
        Cpsect->link_address = 0;
    }
    if (Cpsect->link_first != 0) {
        errout(E_WARN, "do_link_after", ".link_after directive is overriding previous .link_first directive for dsect %s\n",
               Cpsect->name);
        Cpsect->link_first = 0;
    }
    if (Cpsect->link_last != 0) {
        errout(E_WARN, "do_link_after", ".link_after directive is overriding previous .link_last directive for dsect %s\n",
               Cpsect->name);
        Cpsect->link_last = 0;
    }

    if (Cpsect->link_after_name != NULL) FREE(Cpsect->link_after_name);
    Cpsect->link_after_name = name_tok->tok.s;
    Cpsect->link_after = 1;

    free_tok(name_tok);
}

void do_link_first(void)
{

    if (Cpsect->entry_type != PENTRY_TYPE_DSECT) {
        errout(E_WARN, "do_link_first", ".link_first directive is illegal for non-dsect %s, it will be ignored\n", Cpsect->name);
        return;
    }
    if (Cpsect->link_address != 0) {
        errout(E_WARN, "do_link_first", ".link_first directive is overriding previous .link_address directive for dsect %s\n",
               Cpsect->name);
        Cpsect->link_address = 0;
    }
    if (Cpsect->link_after != 0) {
        errout(E_WARN, "do_link_first", ".link_first directive is overriding previous .link_after directive for dsect %s\n",
               Cpsect->name);
        Cpsect->link_after = 0;
    }
    if (Cpsect->link_last != 0) {
        errout(E_WARN, "do_link_first", ".link_first directive is overriding previous .link_last directive for dsect %s\n",
               Cpsect->name);
        Cpsect->link_last = 0;
    }

    Cpsect->link_first = 1;

}

void do_link_last(void)
{

    if (Cpsect->entry_type != PENTRY_TYPE_DSECT) {
        errout(E_WARN, "do_link_last", ".link_last directive is illegal for non-dsect %s, it will be ignored\n", Cpsect->name);
        return;
    }
    if (Cpsect->link_address != 0) {
        errout(E_WARN, "do_link_last", ".link_last directive is overriding previous .link_address directive for dsect %s\n",
               Cpsect->name);
        Cpsect->link_address = 0;
    }
    if (Cpsect->link_after != 0) {
        errout(E_WARN, "do_link_last", ".link_last directive is overriding previous .link_after directive for dsect %s\n",
               Cpsect->name);
        Cpsect->link_after = 0;
    }
    if (Cpsect->link_first != 0) {
        errout(E_WARN, "do_link_last", ".link_last directive is overriding previous .link_first directive for dsect %s\n",
               Cpsect->name);
        Cpsect->link_first = 0;
    }

    Cpsect->link_last = 1;

}

void do_save_psect(int blknum)
{
    /*
     * Add current psect name to psect stack.
     */
    if (cur_psect != NULL) {
        PSECT_LIST *ptmp = ALLOC(PSECT_LIST);

        /*
         * Copy the psect name token.
         * This allows the same psect to be on the stack multiple times.
         */
        ptmp->psect = copy_expr(cur_psect);
        ptmp->blknum = blknum;
        ptmp->next = psect_stk;
        psect_stk = ptmp;
    } else
        errout(E_ERR, "do_save_psect", "current psect not defined.\n");
  
    return;
}

#ifdef __STDC__
int do_rest_psect(int type)
#else
int do_rest_psect(type)
int type;
#endif
{
    TOKEN_LIST *ttmp;
    PSECT_LIST *ptmp;
    int blknum;
    
    if (psect_stk == NULL) {
        errout(E_ERR, "do_rest_psect", "no psects defined on psect stack.\n");
	exit(EXIT_FAILURE);
    }

    /*
     * Pop a psect off the psect stack.
     */
    ptmp = psect_stk;
    psect_stk = psect_stk->next;

    /*
     * Free up current psect's name token
     */
    free_tok_s(cur_psect);
    
    cur_psect = ptmp->psect;
    blknum = ptmp->blknum;
    cur_psect->next = NULL;
    FREE(ptmp);
    
    /*
     * set the psect, 0 alignment means don't change what was there.
     */
    set_psect(cur_psect->tok.s, 0, type);

    return(blknum);
}

#ifdef _WIN32
void do_cmdin(int number, TOKEN_LIST *textblock)
{
    errout(E_ERR, "cmd_input", ".cmd_input not supported in this version");
}

#else

#ifdef __STDC__
void do_cmdin(int number, TOKEN_LIST *textblock)
#else
void do_cmdin(number,textblock)
int number;
TOKEN_LIST *textblock;
#endif
{
    CMDINFO *cmd_p;
    int max,i;
    char symbol[256];

    /* have we seen the number before? */
    for (cmd_p = cmd_list; cmd_p && cmd_p->number != number; cmd_p = cmd_p->next)
        ;

    /* if not, then allocate a new object, initalize it, and add to master list*/
    if (!cmd_p) { 
        cmd_p = ALLOC(CMDINFO);
        cmd_p->number = number;
        get_tmpnam(cmd_p->filename); 
        cmd_p->fileptr = fopen(cmd_p->filename,"w");
        if (cmd_p->fileptr==NULL) {
            errout(E_ERR, "cmd_input", "Couldn't open cmd input file %s\n",cmd_p->filename);
            FREE(cmd_p);
            free_tok_s(textblock);
            return;
        }
        cmd_p->next = cmd_list;
        cmd_list = cmd_p;
    }

    /* make sure file is opened -- do_cmd will close it before using it */
    if (!cmd_p->fileptr)
        cmd_p->fileptr = fopen(cmd_p->filename,"a"); /* this should work since it did before */
    
    /* Append the new information to the input file */
    max = strlen(textblock->tok.s);
    for (i=0; i<max; ++i) {    
        if (sscanf(&(textblock->tok.s[i]),"[[[[%[^] \t\n]]]]]",symbol) == 1) {
          fprintf(cmd_p->fileptr,"[[%s]]",symbol);
          i += strlen(symbol) + 7; 
        }
        else if (sscanf(&(textblock->tok.s[i]),"[[%[^] \t\n]]]",symbol) == 1) {
            SENTRY *syminfo;
            if (!avl_lookup(SymTable,strlow(symbol),&syminfo)) {
                errout(E_ERR, "cmd_input", "Substitution symbol '%s' wasn't defined - Using 0.\n",symbol);
                fputc('0',cmd_p->fileptr); /* just use 0 as result */
            }
            else {
                if (!syminfo->valid) {
                    TOKEN_LIST* result = eval_expr(syminfo->expr, NULL);
                    if (!result) {
                        errout(E_ERR, "cmd_input", "Substitution symbol '%s' couldn't be resolved - Using 0.\n",symbol);
                        fputc('0',cmd_p->fileptr); /* just use 0 as result */
                    }
                    else {
                        if (result->tok.q.v[1])
                            fprintf(cmd_p->fileptr,"%X%08X",result->tok.q.v[1],result->tok.q.v[0]);
                        else
                            fprintf(cmd_p->fileptr,"%X",result->tok.q.v[0]);
                        save_absexpr(syminfo->expr, result);
                        free_tok(result);
                    }
                }
                else {
                    if (syminfo->value[1])
                        fprintf(cmd_p->fileptr,"%X%08X",syminfo->value[1],syminfo->value[0]);
                    else
                        fprintf(cmd_p->fileptr,"%X",syminfo->value[0]);
                }
            }
            i += strlen(symbol) + 3; /* skip over entire substitution */
            
        }
        else
            fputc(textblock->tok.s[i],cmd_p->fileptr);
    }

    free_tok_s(textblock);

}

#endif /* _WIN32 */


#ifdef _WIN32
char *do_cmd(TOKEN_LIST *cmd_string)
{
    errout(E_ERR, "do_cmd", ".do_cmd not supported in this version");
}

#else


#ifdef __STDC__
char *do_cmd(TOKEN_LIST *cmd_string)
#else
char *do_cmd(cmd_string)
TOKEN_LIST *cmd_string;
#endif
{
    int status, inum;
    char *real_cmd, *sptr;
    char symbol[256];
    CMDINFO *cmd_p;
    char *out_file = ALLOC_STR(L_tmpnam + 10);
    int good_result = 0;
    int use_output = 0;

    /* generate a tmp filename for the output */
    get_tmpnam(out_file);
    
    /* create real cmd string by substituting input/output filenames  and symbols into cmd string*/
    real_cmd = ALLOC_STR(strlen(cmd_string->tok.s) + 1024); /* need more room for substitutions -- hopefully thats enough*/
    real_cmd[0] = '\0';
    for (sptr=cmd_string->tok.s; *sptr!='\0'; ++sptr) {    
        if (sscanf(sptr,"[[input%d]]",&inum)==1) {
            for (cmd_p = cmd_list; cmd_p && cmd_p->number!=inum; cmd_p = cmd_p->next)
                ;
            if (!cmd_p) {
                errout(E_ERR, "do_cmd", "There is no data for input %d -- command aborted.\n",inum);
                goto do_cmd_end;
            }
            fclose(cmd_p->fileptr); /* need to close file before we can read it */
            cmd_p->fileptr = NULL;  /* this indicates it's closed */
            strcat(real_cmd, cmd_p->filename);
            sptr = strstr(sptr,"]]"); ++sptr; /* skip over symbol and first ']'*/
        }
        else if (strstr(sptr,"[[output]]")==sptr) {
            strcat(real_cmd, out_file);
            sptr = strstr(sptr,"]]"); ++sptr; /* skip over symbol and first ']'*/
            use_output = 1;
        }
        else if (sscanf(sptr,"[[%[^] \t\n]]]",symbol) == 1) {
            SENTRY *syminfo;
            if (!avl_lookup(SymTable,strlow(symbol),&syminfo)) {
                errout(E_ERR, "do_cmd", "Substitution symbol '%s' wasn't defined - Using 0.\n",symbol);
                strcat(real_cmd, "0"); /* just use 0 as result */
            }
            else {
                if (!syminfo->valid) {
                    TOKEN_LIST* result = eval_expr(syminfo->expr, NULL);
                    if (!result) {
                        errout(E_ERR, "do_cmd", "Substitution symbol '%s' couldn't be resolved - Using 0.\n",symbol);
                        strcat(real_cmd, "0"); /* just use 0 as result */
                    }
                    else {
                        if (result->tok.q.v[1])
                            sprintf(real_cmd,"%s%X%08X",real_cmd,result->tok.q.v[1],result->tok.q.v[0]);
                        else
                            sprintf(real_cmd,"%s%X",real_cmd,result->tok.q.v[0]);
                        save_absexpr(syminfo->expr, result);
                        free_tok(result);
                    }
                }
                else {
                    if (syminfo->value[1])
                        sprintf(real_cmd,"%s%X%08X",real_cmd,syminfo->value[1],syminfo->value[0]);
                    else
                        sprintf(real_cmd,"%s%X",real_cmd,syminfo->value[0]);
                }
            }
            sptr = strstr(sptr,"]]"); ++sptr; /* skip over symbol and first ']'*/
        }
        else
            strncat(real_cmd, sptr, 1);
    }
    
    /* and finally -- do the command string as a system call */
    errout(E_INFO | E_NO_LNUM, "hal", "Executing shell command...\n");
    good_result = sys(real_cmd);
    if (!good_result) {
        errout(E_ERR, "do_cmd", "CMD '%s' return bad status -- output will be ignored.\n",real_cmd);
        if (use_output)
          remove(out_file);
    }


  do_cmd_end:
    /* clean-up and return result file name */
    free_tok_s(cmd_string);
    FREE(real_cmd);
    if (good_result && use_output)
        return(out_file);  
    else {
        FREE(out_file);
        return(NULL);
    }
}

#endif /* _WIN32 */

void define_macro(TOKEN_LIST *name, int narg, int var_arg, TOKEN_LIST* args, TOKEN_LIST* argdefs, TOKEN_LIST *body)
{
  MACRODEF *mtmp = ALLOC0(1, MACRODEF);
  OPID oid = ID_NULL, mid;
  int i;

#ifdef DEBUG
  fprintf(stderr, "******** defining macro %s\n", name->tok.s);
  fprintf(stderr, "******** body:%s\n", body->tok.s);
#endif

  mtmp->fname = find_fname(NULL);
  mtmp->lineno = start_line;
  mtmp->name = name->tok.s;
  mtmp->narg = narg;
  mtmp->body = body->tok.s;
  mtmp->var_arg = var_arg - 1;

  mtmp->skiparglen = cur_ifile->lineno + cur_ifile->base_lineno - start_line + 1; 
  mtmp->skipargck = ALLOC0(mtmp->skiparglen, int);
  mtmp->argtypes = ALLOC0(narg, int);  
  mtmp->args = ALLOC0(narg, char*);
  mtmp->argdefs = ALLOC0(narg, char*);

  for (i=0; i<narg; ++i) {
      TOKEN_LIST* ttok;
      
      assert(args != NULL);
      assert(argdefs != NULL);

      if (var_arg  && (i > var_arg) && (args->type != MACRO_ARG_KWONLY)) {
        errout(E_ERR, "define_macro",
               "In MACRO '%s', argument '%s' was defined after a variable length argument and wasn't 'keyword only'. It will be ignored.\n",name->tok.s,args->tok.s);
        FREE(args->tok.s);
        if (argdefs->type != '?')
          FREE(argdefs->tok.s);
        mtmp->narg -= 1;
      }
      else {
        mtmp->argtypes[i] = args->type;
        mtmp->args[i] = args->tok.s;
        if (argdefs->type == '?') {
          mtmp->argdefs[i] = (char*) '?';
        } else
          mtmp->argdefs[i] = argdefs->tok.s;
      }
      
      ttok = args;
      args = args->next;
      free_tok(ttok);

      ttok = argdefs;
      argdefs = argdefs->next;
      free_tok(ttok);
  }

  create_macro(mtmp, name->tok.s);
  
  free_tok(name);
  free_tok(body);
  
  return;
}

void create_macro(MACRODEF *mtmp, char *name) {
    OPID oid, mid;

    if (global_pragma & PRAGMA_MAC_NONE) {
        errout(E_ERR, "create_macro", "unable to define macro %s, macro definitions are disabled.\n", name);
        return;
    }
    
    /*
     * Check if operator exists.  Note, that the call to find_operator will
     * hack around the name to not include a "/", and to be all lowercase.
     */
    if ((oid = find_operator(name, 1/*return_name*/)) != ID_NULL) {

        if (operator[oid].type == MACRO_CALL) {
            
            if (global_pragma & PRAGMA_MAC_ONCE) {
                errout(E_ERR, "create_macro",
                       "illegal redefinition of macro %s, old definition remains in place.\n", name);
                return;
            } else {
                /* if it is legal to redfine a macro, free the old definition, and add the new. */
                mid = create_macro_defn(mtmp);

                free_macrodef(macro_defn[operator[oid].value]);
                macro_defn[operator[oid].value] = NULL;  /* we don't reuse macro ids,
                                                            this will prevent freeing
                                                            again at end. */
                
                operator[oid].name = name;
                operator[oid].value = mid;
            }
        } else {
            /* it was a default operator, create a new one. */
            mid = create_macro_defn(mtmp);

            oid = create_operator(name, MACRO_CALL, mid);
        }

        /* operator already exists, remove it from hash */
        delete_operator(oid);

    } else {
        /* create a new operator. */
        mid = create_macro_defn(mtmp);

        oid = create_operator(name, MACRO_CALL, mid);
    }
  
    insert_operator(oid);
    return;
}

char** setup_macro_call_args(TOKEN_LIST* mactok, MACRODEF** macdef_p, int *size)
{
    OPID oid = mactok->tok.q.v[0];
    OPID mid = operator[oid].value;
    MACRODEF* mtmp = macro_defn[mid];
    char** argl;

    /* pass back macro definition for later use. */
    *macdef_p = mtmp;

    /*
     * allocate array of default arguments, ALLOC0 causes them
     * to be initialized to NULL.
     */
    if (mtmp->var_arg < 0) {
      argl = ALLOC0(mtmp->narg, char*);
      *size = mtmp->narg;
    }
    else {
      argl = ALLOC0((mtmp->narg + 10), char*);
      *size = mtmp->narg + 10;
    }

    free_tok(mactok);

    return argl;
}

int assign_macro_arg(MACRODEF* mtmp, char*** margs, int *margs_size, TOKEN_LIST* symb, TOKEN_LIST* value, int *pos)
{
    
    if (symb != NULL) {
        int i;
        
        /*
         * Macro argument is a keyword argument.
         *
         * Look for matching keyword.
         */
        for (i=0; i<mtmp->narg; ++i)
            if (strcmp(mtmp->args[i], symb->tok.s) == 0)
                break;

        if (i == mtmp->narg) {
            errout(E_ERR, "assign_macro_arg", "in macro \"%s\", keyword argument \"%s\" does not exist.\n", mtmp->name, symb->tok.s);
            free_tok_s(symb);
            free_tok_s(value);
            return 0;
        }

        free_tok_s(symb);
        
        if (value != NULL) {
            /* free last arg_value (if any) */
            if ((*margs)[i] != NULL)
                FREE((*margs)[i]);
            
            (*margs)[i] = value->tok.s;
            free_tok(value);
        }

        return 1;
    }

    /*
     * Macro argument is a positional argument.
     *
     * Check if the macro has this argument declared.
     */
    if ((*pos < 0) || ((*pos >= mtmp->narg) && mtmp->var_arg<0)) {
        errout(E_ERR, "assign_macro_arg", "macro \"%s\" only has %d arguments.\n", mtmp->name, mtmp->narg);
        free_tok_s(value);
        return 0;
    }

    while ((*pos < mtmp->narg) && (mtmp->argtypes[*pos] == MACRO_ARG_KWONLY))
      *pos += 1;

    if (*pos >= mtmp->narg && mtmp->var_arg < 0) {
        errout(E_ERR, "assign_macro_arg", "attempted to positionally assign to a keyword only argument in macro \"%s\".\n", mtmp->name);
        free_tok_s(value);
        return 0;
    }

    if (value != NULL) {
      if (*pos >= (*margs_size - 1)) { /* variable args --> always leave last NULL so we know when there are no more args */
        char** argl = ALLOC0(*margs_size + 10, char*);
        int i=0;
        *margs_size += 10;
        for (; i<*pos; ++i)
          argl[i] = (*margs)[i];
        FREE(*margs);
        *margs = argl;
      }
      
      if ((*margs)[*pos] != NULL)
        FREE((*margs)[*pos]);
      
      (*margs)[*pos] = value->tok.s;
      free_tok(value);
      *pos += 1;
    }
    
    return 1;
}

void expand_macro(MACRODEF* mtmp, char** argl)
{
    INPUT_FILE *ftmp;
    char *tstr;
    MACRO_STK* stktmp;
    int i, status;
    
    if (yychar != YYEMPTY)
        errout(E_ERR, "expand_macro", "internal error, bison has a lookahead token on macro expansion.\n");

    /*
     * Send current listing line to listing file.
     */
    do_list(L_CALLS, start_line);
    
    /*
     * First, push the current input file back onto the input file stack.
     */
    push_current_ifile();

    /*
     * allocate a new set of macro arguments on the macro stack.
     */
    stktmp = ALLOC0(1, MACRO_STK);

    stktmp->argl = argl;
    stktmp->var_arg_idx = mtmp->var_arg;
    stktmp->mdef = mtmp;
    stktmp->bfile = NULL;	/* will hold the input file for macro preprocessing */
    stktmp->next = macstk;
    macstk = stktmp;

    /*
     * Setup values for any created local labels.
     */
    for (i=0; i<mtmp->narg; ++i)
        if ((argl[i] == NULL) && ((long)mtmp->argdefs[i] == '?')) {
            /* allocate space for the string */
            char* tstr = ALLOC_STR(12);

            /* create the label in the string just allocated */
            sprintf(tstr, "%d$", local_label++);

            /* place it in the argument list */
            argl[i] = tstr;
        }
    
    /*
     * Push the macro body on the input file stack, preparing to preprocess it
     * using a special mode in the lexer.
     */
    add_input(NULL, F_MACRO, 1, NULL, ADD_PUSH, mtmp->lineno, 0);

    /*
     * Add an input for the preprocessing of the macro body.
     */
    add_input(mtmp->body, F_MACRO_PREP, 1, NULL, ADD_PUSH, mtmp->lineno, 0);

    /*
     * Remove the input file for preprocessing, and place it in the macro stack
     */
    ftmp = ifile->next;
    ifile->next = macstk->bfile;
    macstk->bfile = ifile;
    ifile = ftmp;
    
    /* setup the macro input file as the input file */
    (void)next_file();

    /*
     * The first attempt to get input from the macro will result in EOF.
     *
     * The EOF actions will see that it is a macro and set up the input
     * file from the macro stack to preprocess each individual line of
     * the macro.
     */
    
    return;
}

#ifndef _WIN32
/************************************************************
**
** sys("command to execute");
**
** This function will execute the given command on
** the system.  This was needed so that the environment
** would be executed before running the command on ULTRIX.
**
*************************************************************/
int sys(char *sys_cmd)
{
  char cmd[1024];
  int status;
  FILE *script;
  char script_fn[L_tmpnam + 10];

#ifdef VMS
  struct dsc$descriptor_d command;
  int libstatus, flags;
#else
  int spid;
# ifdef _BSD
  union wait p_stat;
# else
  int p_stat;
# endif
#endif

  (void) get_tmpnam(script_fn);
  script = fopen(script_fn,"w");

#ifdef VMS
  fprintf(script,"$ set verify\n");
  fprintf(script,"$ %s\n",sys_cmd);
  fprintf(script,"$ exit $status\n");
  fclose(script);

  sprintf(cmd, "@%s", script_fn);
  command.dsc$w_length = strlen(cmd);
  command.dsc$b_dtype = DSC$K_DTYPE_T;
  command.dsc$b_class = DSC$K_CLASS_S;
  command.dsc$a_pointer = cmd;

  flags = 0;
  libstatus = lib$spawn(&command/*command*/, 0/*input-file*/, 0/*output-file*/,
                     &flags, 0/*prcnam*/, 0, &status, 0, 0, 0, 0, 0);

  if (libstatus != SS$_NORMAL) 
      status = 0;
  else
      status = (status & 3) == 1;

#else
  fprintf(script,"#!/bin/sh -v\n");
  fprintf(script,"%s\n",sys_cmd);
  fprintf(script,"exit $?\n");
  fclose(script);

  chmod(script_fn,0x1c0);

  /* create a child process */
  spid = fork();

  if (spid == 0) {
    /* in child process */
    /* execute the script file */
    execl(script_fn, script_fn, (char *)0);
    /* NOT REACHED */
  }

  /* get resulting status */
  if (wait(&p_stat) == -1) {
      /* error returned */
      status = 0;
  } else
  status = WIFEXITED(p_stat) && WEXITSTATUS(p_stat)==0;
#endif

  (void)remove(script_fn);
  return(status);
}
#endif /* _WIN32 */
