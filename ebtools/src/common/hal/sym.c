/*****************************************************************************/
/*FILE: SYM.C    Routines for the Symbol table                           */
/****************************************************************************
	Copyright (c) 1992,93,94
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
 *****************************************************************************/
/* Originally Created by: Homayoon Akhiani	April,1992		     */
/*****************************************************************************
! REVISION HISTORY:
! Who   When            What
!---------------------------------------------------------------
! JDH   27-apr-1994     added division operator
! DHA   03-May-1994	converted to merged version. renamed to sym.c
! HA	19-Jan-1994	Add * support in assignments
! HA	06-Oct-1993	fix . = symb problem
! HA	01-Oct-1993	fix very obsecure problem with symresolve/. movements 
! HA	21-Sep-1993	fix problems with label evaluation 
! HA	20-Aug-1992	fix @ on relocatable symbols
! HA	18-Aug-1992	fix bug in resolving addresses in other PSECTs
! HA	13-Aug-1992	The 13th: updating .ADDRESS with .PSECT
! HA	29-Jul-1992	HW_LD,HW_ST
! HA	22-May-1992	Psect support is added
! HA	12-May-1992	forward references resolved
! HA    23-Apr-1992	Include stdlib.h (for portability)    
! HA    xx-Apr-1992     First pass
 *****************************************************************************/
/*****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "hal.h"

TOKEN_LIST* headref = NULL;
TOKEN_LIST** endref = &headref;

TOKEN_LIST* make_value_token(uint32 hi, uint32 lo)
{
  TOKEN_LIST* temp = alloc_tok();

  temp->type = NUMBER;
  temp->tok.q.v[0] = lo;
  temp->tok.q.v[1] = hi;
  
  return temp;
}

/*****************************************************************************/
/* Return symbol structure for named symbol */
/* If symbol not defined, return NULL */
/*****************************************************************************/
SENTRY *symdata(s)
char *s;
{
  SENTRY *e;
  if ((avl_lookup(SymTable,s,&e)) != 0)
    return e;
  else
    return NULL;
}
  

/*****************************************************************************/
/* look if the symbol is in, (YES) update the ref, (NO) create  */
/*****************************************************************************/
void
symuse(s,type)
char *s;    
int  type;
{
    SENTRY *e;
    RNODE  *n, *p;
    
    if ((avl_lookup(SymTable,s,&e)) == 0) {
        /* Create the symbol entry and add reference */
        symcreate(&e);
        if((n = ALLOC(RNODE)) == 0) {
            errout(E_ERR, "symuse", "insufficient memory, allocating reference node.\n");
            exit(EXIT_FAILURE);
        }
        n->next = NULL;
        n->lloc = save_location();
        n->type = type;
        n->ic   = ic;
        n->wrnum = Cpsect->wrnum[ic] + 1;
        n->sect = Cpsect;
        n->listused = 0;
        if (listing_file != NULL) {
            TOKEN_LIST* ttok = alloc_tok();
            ttok->tok.ref = n;
            *endref = ttok;
            endref = &ttok->next;
        }
        e->ref  = n;
        e->sect = Cpsect;
        e->symbol = s;
        e->reloc = 1;	/* if symbol is used prior to definition, it must be a label */
        avl_insert(SymTable,e->symbol,e); 
        dbgout("InsertSymbolUse %s at line %d type=%d\n",
                e->symbol,(cur_ifile->lineno + cur_base_lineno),type);
    } else {
        /* entry already exists*/

        FREE(s);
        
        dbgout("SymbolUse %s at line %d type=%d\n",
                e->symbol,(cur_ifile->lineno + cur_base_lineno),type);
        if(e->ref == NULL) {
            if((n = ALLOC(RNODE)) == 0) {
                errout(E_ERR, "symuse", "insufficient memory, allocating reference node.\n");
                exit(EXIT_FAILURE);
            }
            n->next = NULL;
            n->lloc = save_location();
            n->type = type;
            n->ic   = ic;
            n->wrnum = Cpsect->wrnum[ic] + 1;
            n->sect = Cpsect;
            n->listused = 0;
            if (listing_file != NULL) {
                TOKEN_LIST* ttok = alloc_tok();
                ttok->tok.ref = n;
                *endref = ttok;
                endref = &ttok->next;
            }
            e->ref  = n;
        } else {
            for(p=e->ref;p->next != NULL ;p = p->next)
                ;   /*goto the end of the list*/

            if((n = ALLOC(RNODE)) == 0) {
                errout(E_ERR, "symuse", "insufficient memory, allocating reference node.\n");
                exit(EXIT_FAILURE);
            }
            n->next = NULL;
            n->lloc = save_location();
            n->type = type;
            n->ic   = ic;
            n->wrnum = Cpsect->wrnum[ic] + 1;
            n->sect = Cpsect;
            n->listused = 0;
            if (listing_file != NULL) {
                TOKEN_LIST* ttok = alloc_tok();
                ttok->tok.ref = n;
                *endref = ttok;
                endref = &ttok->next;
            }
            p->next = n;
        }
    }
}
/*****************************************************************************/
/* look if the symbol is in, (YES) check! and update the value, (NO) create  */
/*****************************************************************************/
void symdef(s,hv,lv,expr,reloc)
char         *s;
unsigned int hv, lv;
TOKEN_LIST* expr;
int reloc;
{
    SENTRY *e;
    if((avl_lookup(SymTable,s,&e)) == 0) {
        /*
         * Create new symbol.
         */
        symcreate(&e);
        e->symbol = s;

        /*
         * Insert symbol into symbol table.
         */
        avl_insert(SymTable,e->symbol,e); 

        dbgout("InsertSymbolDef %s at line %d [%08X %08X]\n",
                e->symbol,(cur_ifile->lineno + cur_base_lineno),hv,lv);
    } else {

        FREE(s);
        
        /*
         * If label already defined, and we are attempting to redefine it,
         * that is an error.
         */
        if(e->valid && e->reloc && reloc)
            errout(E_ERR, "symdef", "multiple definition of label \"%s\", using new definition.\n", e->symbol);

        /*
         * If relocatability has changed, that is an error.
         */
        if (e->reloc != reloc)
            errout(E_ERR, "symdef", "\"%s\" defined as both a label and a symbol, using new definition.\n", e->symbol);

        dbgout("ExistSymbolDef %s at line %d [%08X %08X]\n",
                e->symbol,(cur_ifile->lineno + cur_base_lineno),hv,lv);
    }

    /*
     * Set up info about symbol.
     */     
    e->pc       = pc;
    e->valid    = 1;
    e->reloc    = reloc;
    e->value[0] = lv;
    e->value[1] = hv;
    if(expr != NULL) {
        free_tok_list_s(e->expr);
        e->expr = expr;
        e->valid = 0;
        dbgout("Defered %s at line %d\n",
                e->symbol,(cur_ifile->lineno + cur_base_lineno));
    }
    e->sect = Cpsect;

    return;
}
/*****************************************************************************/
/* SYMVAL:								     */
/*	resolve the value of the symbol					     */
/*****************************************************************************/
void symval(e)
SENTRY *e;
{
 dbgout("Resolving %s ->[%08X %08X] valid=%d\n",
          e->symbol,e->value[1],e->value[0],e->valid);
 if (e->expr != NULL) {
     TOKEN_LIST* result = eval_expr(e->expr, NULL);
     e->valid = (result != NULL);

     if (e->valid == 0) {
         errout(E_ERR, "symval", "undefined symbol: %s \n",e->symbol);
         dbgout("unable to resolve %s\n", e->symbol);
         return;
     }
     e->value[1] = result->tok.q.v[1];
     e->value[0] = result->tok.q.v[0];

     free_tok(result);
 }
 dbgout("ResolvED %s ->[%08X %08X]\n",
          e->symbol,e->value[1],e->value[0],e->valid);
 return;
}
/*****************************************************************************/
/* SYMRESOLVE:								     */
/*****************************************************************************/
void symresolve(s,e)
char   *s;
SENTRY *e;
{
    RNODE *n;
    
    symval(e);

    if (!e->valid) {
         errout(E_ERR | E_NO_LNUM, "symresolve", "undefined symbol: %s \n",e->symbol);
         dbgout("unable to resolve %s\n", e->symbol);
    }

    for(n=e->ref;n != NULL ;n = n->next) {
        
        if (!e->valid) {
            print_location(E_ERR, "symresolve", n->lloc);
            continue;
        }

        /* If write number of this reference is less than the write
         * number of the location, then this reference was overwritten
         * and we should not update the location.
         */
        
        switch(n->type) {
        case TD32:	
            dbgout("D32(%d) at %08X(%d) [%08X]->",n->wrnum,n->ic*4, n->sect->wrnum[n->ic],n->sect->mp[n->ic]);
            
            if (n->wrnum < n->sect->wrnum[n->ic]) {
                dbgout("Ref at %08X in %s ignored because overwritten\n", n->ic*4, n->sect->name);
                
                if (n->listused) {
                    fsetpos(listing_file, &n->lpos[0]);
                    fprintf(listing_file, "xxxxxxxx");
                }
                break;
            }
            
            n->sect->mp[n->ic] = e->value[0];
            if (n->listused) {
                fsetpos(listing_file, &n->lpos[0]);
                fprintf(listing_file, "%08X", e->value[0]);
            }
            dbgout("[%08X]\n",n->sect->mp[n->ic]);
            break;
        case TADD:
        case TD64:	
            dbgout("D64(%d) at %08X(%d) [%08X %08X]->", n->wrnum, n->ic*4, n->sect->wrnum[n->ic], n->sect->mp[n->ic+1],n->sect->mp[n->ic]);
            
            if (n->wrnum < n->sect->wrnum[n->ic]) {
                dbgout("Ref(low LW) at %08X in %s ignored because overwritten\n", n->ic*4, n->sect->name);
                
                if (n->listused) {
                    fsetpos(listing_file, &n->lpos[1]);
                    fprintf(listing_file, "xxxxxxxx");
                }
            } else {
                n->sect->mp[n->ic]=e->value[0];
                if (n->listused) {
                    fsetpos(listing_file, &n->lpos[1]);
                    fprintf(listing_file, "%08X", e->value[0]);
                }
            }
            
            if (n->wrnum < n->sect->wrnum[n->ic+1]) {
                dbgout("Ref(hi LW) at %08X in %s ignored because overwritten\n", n->ic*4, n->sect->name);
                
                if (n->listused) {
                    fsetpos(listing_file, &n->lpos[0]);
                    fprintf(listing_file, "xxxxxxxx");
                }
            } else {
                n->sect->mp[n->ic+1]=e->value[1];
            
                if (n->listused) {
                    fsetpos(listing_file, &n->lpos[0]);
                    fprintf(listing_file, "%08X", e->value[1]);
                }
            }
            dbgout("[%08X %08X]\n",n->sect->mp[n->ic+1],n->sect->mp[n->ic]);
            break;
        case TMDS:	/* Memory: Do not clean the m[] (HA)*/
            dbgout("MDS(%d) at %08X(%d) [%08X]->",n->wrnum,n->ic*4, n->sect->wrnum[n->ic],n->sect->mp[n->ic]);
            
            if (n->wrnum < n->sect->wrnum[n->ic]) {
                dbgout("Ref at %08X in %s ignored because overwritten\n", n->ic*4, n->sect->name);
                
                if (n->listused) {
                    fsetpos(listing_file, &n->lpos[0]);
                    fprintf(listing_file, "xxxxxxxx");
                }
                break;
            }
            
            n->sect->mp[n->ic] |= (e->value[0] & 0xFFFF);
            if (n->listused) {
                fsetpos(listing_file, &n->lpos[0]);
                fprintf(listing_file, "%08X", n->sect->mp[n->ic]);
            }
            dbgout("[%08X]\n",n->sect->mp[n->ic]);
            break;
        case TPDS:	/* CALLPAL: Do not clean the m[] (HA)*/
            dbgout("PDS(%d) at %08X(%d) [%08X]->",n->wrnum,n->ic*4, n->sect->wrnum[n->ic],n->sect->mp[n->ic]);
            
            if (n->wrnum < n->sect->wrnum[n->ic]) {
                dbgout("Ref at %08X in %s ignored because overwritten\n", n->ic*4, n->sect->name);
                
                if (n->listused) {
                    fsetpos(listing_file, &n->lpos[0]);
                    fprintf(listing_file, "xxxxxxxx");
                }
                break;
            }
            
            n->sect->mp[n->ic] |= (e->value[0] & 0x3FFFFFF);
            if (n->listused) {
                fsetpos(listing_file, &n->lpos[0]);
                fprintf(listing_file, "%08X", n->sect->mp[n->ic]);
            }
            dbgout("[%08X]\n",n->sect->mp[n->ic]);
            break;
        case TJDS:
            dbgout("JDS(%d) at %08X(%d) [%08X]->",n->wrnum,n->ic*4, n->sect->wrnum[n->ic],n->sect->mp[n->ic]);
            
            if (n->wrnum < n->sect->wrnum[n->ic]) {
                dbgout("Ref at %08X in %s ignored because overwritten\n", n->ic*4, n->sect->name);
                
                if (n->listused) {
                    fsetpos(listing_file, &n->lpos[0]);
                    fprintf(listing_file, "xxxxxxxx");
                }
                break;
            }
            
            n->sect->mp[n->ic] |= (e->value[0] & 0x3FFF);
            if (n->listused) {
                fsetpos(listing_file, &n->lpos[0]);
                fprintf(listing_file, "%08X", n->sect->mp[n->ic]);
            }
            dbgout("[%08X]\n",n->sect->mp[n->ic]);
            break;
        case TMHW10:	/* HW_LD/ST: Do not clean the m[] (HA)*/
            dbgout("MHW10(%d) at %08X(%d) [%08X]->",n->wrnum,n->ic*4, n->sect->wrnum[n->ic],n->sect->mp[n->ic]);
            
            if (n->wrnum < n->sect->wrnum[n->ic]) {
                dbgout("Ref at %08X in %s ignored because overwritten\n", n->ic*4, n->sect->name);
                
                if (n->listused) {
                    fsetpos(listing_file, &n->lpos[0]);
                    fprintf(listing_file, "xxxxxxxx");
                }
                break;
            }
            
            n->sect->mp[n->ic] |= (e->value[0] & 0x3FF);
            if (n->listused) {
                fsetpos(listing_file, &n->lpos[0]);
                fprintf(listing_file, "%08X", n->sect->mp[n->ic]);
            }
            dbgout("[%08X]\n",n->sect->mp[n->ic]);
            break;
        case TMHW12:	/* HW_LD/ST: Do not clean the m[] (HA)*/
            dbgout("MHW12(%d) at %08X(%d) [%08X]->",n->wrnum,n->ic*4, n->sect->wrnum[n->ic],n->sect->mp[n->ic]);
            
            if (n->wrnum < n->sect->wrnum[n->ic]) {
                dbgout("Ref at %08X in %s ignored because overwritten\n", n->ic*4, n->sect->name);
                
                if (n->listused) {
                    fsetpos(listing_file, &n->lpos[0]);
                    fprintf(listing_file, "xxxxxxxx");
                }
                break;
            }
            
            n->sect->mp[n->ic] |= (e->value[0] & 0xFFF);
            if (n->listused) {
                fsetpos(listing_file, &n->lpos[0]);
                fprintf(listing_file, "%08X", n->sect->mp[n->ic]);
            }
            dbgout("[%08X]\n",n->sect->mp[n->ic]);
            break;
        case TMOD:	/* HW_LD/ST: Do not clean the m[] (HA)*/
            dbgout("MOD(%d) at %08X(%d) [%08X]->",n->wrnum,n->ic*4, n->sect->wrnum[n->ic],n->sect->mp[n->ic]);
            
            if (n->wrnum < n->sect->wrnum[n->ic]) {
                dbgout("Ref at %08X in %s ignored because overwritten\n", n->ic*4, n->sect->name);
                
                if (n->listused) {
                    fsetpos(listing_file, &n->lpos[0]);
                    fprintf(listing_file, "xxxxxxxx");
                }
                break;
            }
            
            n->sect->mp[n->ic] |= (e->value[0] & 0xF)<< 12;
            if (n->listused) {
                fsetpos(listing_file, &n->lpos[0]);
                fprintf(listing_file, "%08X", n->sect->mp[n->ic]);
            }
            dbgout("[%08X]\n",n->sect->mp[n->ic]);
            break;
        case TBDS:	
            {
                B32 offset, psaddr;
                dbgout("BDS(%d) at %08X(%d) (psbase=%08X) [%08X]->",n->wrnum,n->ic*4, n->sect->wrnum[n->ic],n->sect->base.ulong[0],n->sect->mp[n->ic]);
            
                if (n->wrnum < n->sect->wrnum[n->ic]) {
                    dbgout("Ref at %08X in %s ignored because overwritten\n", n->ic*4, n->sect->name);
                
                    if (n->listused) {
                        fsetpos(listing_file, &n->lpos[0]);
                        fprintf(listing_file, "xxxxxxxx");
                    }
                    break;
                }
            
                n->sect->mp[n->ic] &= ~0x1FFFFF; /*clean first */
                psaddr = n->sect->base.ulong[0];
                offset = (e->value[0] - ((n->ic+1)*4 + psaddr))>>2;
                n->sect->mp[n->ic] |= (offset & 0x1FFFFF);
                if (n->listused) {
                    fsetpos(listing_file, &n->lpos[0]);
                    fprintf(listing_file, "%08X", n->sect->mp[n->ic]);
                }
                dbgout("[%08X] offset=%X\n",n->sect->mp[n->ic],offset);
            };
            break;
        case TIOP:	
            dbgout("IOP(%d) at %08X(%d) [%08X]->",n->wrnum,n->ic*4, n->sect->wrnum[n->ic],n->sect->mp[n->ic]);
            
            if (n->wrnum < n->sect->wrnum[n->ic]) {
                dbgout("Ref at %08X in %s ignored because overwritten\n", n->ic*4, n->sect->name);
                
                if (n->listused) {
                    fsetpos(listing_file, &n->lpos[0]);
                    fprintf(listing_file, "xxxxxxxx");
                }
                break;
            }
            
            n->sect->mp[n->ic] &= 0xFFE01FFF; /*clean first */
            n->sect->mp[n->ic] |= ((e->value[0] & 0xFF) << 13);
            if (n->listused) {
                fsetpos(listing_file, &n->lpos[0]);
                fprintf(listing_file, "%08X", n->sect->mp[n->ic]);
            }
            dbgout("[%08X]\n",n->sect->mp[n->ic]);
            break;
        default:
            errout(E_ERR | E_NO_LNUM, "symresolve", "internal error: bad tag, symbol %s\n",
                        e->symbol);
            print_location(E_ERR, "symresolve", n->lloc);
            break;
        }
    }
}
/*****************************************************************************/
/* SYMUPDATE:								     */
/*****************************************************************************/
void symupdate(s,e)
char   *s;
SENTRY *e;
{
RNODE *n;
int32   s1v[2], s2v[2], v[2];
int   status;
 if(e->valid == 0) return;
 dbgout("Updating %s [%08X %08X]%d + ",
		    e->symbol,e->value[1],e->value[0],e->valid);
 if(e->reloc)
  {
   e->reloc = 2;
   s1v[0] = e->value[0];
   s1v[1] = e->value[1];
   v[0]   = e->value[0];
   v[1]   = e->value[1];
   s2v[0] = e->sect->base.ulong[0];
   s2v[1] = e->sect->base.ulong[1];
   dbgout("Base[%08X %08X] ->",
		e->sect->base.ulong[1],e->sect->base.ulong[0]);
   add64(s1v,s2v,v);
   e->value[0] = v[0];
   e->value[1] = v[1];
   dbgout("New:[%08X %08X]\n",e->value[1],e->value[0]);
  }
 else
   dbgout("None Relocatable\n");
 return;
}
/*****************************************************************************/
/*****************************************************************************/
void symtableit(s,e)
char   *s;
SENTRY *e;
{
    RNODE *n;
    int   i;
    i = 0;
    if (strchr(e->symbol, '%') != NULL)
        return;
    
    if(e->valid)
        fprintf(listing_file,"%-30s = [%08X %08X]  ",e->symbol,e->value[1],e->value[0]);
    else
        fprintf(listing_file,"%-30s = <No Value!>  ",e->symbol);
    for(n=e->ref;n != NULL ;n = n->next,i++) {
        if (i != 0)
            fprintf(listing_file, "%-54s", " ");
        switch(n->type) {
        case TD32:
            fprintf(listing_file,"32bit data at %08X in PSECT %s\n",n->ic << 2,n->sect->name);
            break;
        case TADD:
        case TD64:
            fprintf(listing_file,"64bit data at %08X in PSECT %s\n",n->ic << 2,n->sect->name);
            break;
        case TMDS:
            fprintf(listing_file,"Displacement at %08X in PSECT %s\n",n->ic << 2,n->sect->name);
            break;
        case TPDS:
            fprintf(listing_file,"Call_pal function at %08X in PSECT %s\n",n->ic << 2,n->sect->name);
            break;
        case TJDS:
            fprintf(listing_file,"JSR hint at %08X in PSECT %s\n",n->ic << 2,n->sect->name);
            break;
        case TBDS:
            fprintf(listing_file,"Branch Displacement at %08X in PSECT %s\n",
                    n->ic << 2,n->sect->name);
            break;
        case TIOP:
            fprintf(listing_file,"Immediate data at %08X in PSECT %s\n",
                    n->ic << 2,n->sect->name);
            break;
        case TMHW10:
            fprintf(listing_file,"Disp. for HW_LD/ST at %08X in PSECT %s\n",
                    n->ic << 2,n->sect->name);
            break;
        case TMHW12:
            fprintf(listing_file,"Disp. for HW_LD/ST at %08X in PSECT %s\n",
                    n->ic << 2,n->sect->name);
            break;
        case TMOD:
            fprintf(listing_file,"Modifier for HW_LD/ST at %08X in PSECT %s\n",
                    n->ic << 2,n->sect->name);
            break;
        default: 
            fprintf(listing_file,"Bad Symbol! at %08X in PSECT %s\n",n->ic << 2,n->sect->name);
            break;
        }
    }
    if (i == 0)
        fprintf(listing_file, "\n");
}
/*****************************************************************************/
void symcreate(pe)
SENTRY **pe;
{
    if((*pe = ALLOC(SENTRY)) == 0) {
        errout(E_ERR, "symcreate", "insufficient memory, allocating symbol structure.\n");
        exit(EXIT_FAILURE);
    }
    (*pe)->pc	    = 0;
    (*pe)->fwd_ref	    = 0;
    (*pe)->valid	    = 0;
    (*pe)->value[0]    = 0;
    (*pe)->value[1]    = 0;
    (*pe)->expr        = NULL;
    (*pe)->symbol   = NULL;
    (*pe)->ref	    = NULL;
}

TOKEN_LIST* get_symbol_value(TOKEN_LIST* name, int is_while)
{
    SENTRY* stemp;

    /*
     * get symbol out of symbol table.
     *
     * if not there yet, return the symbol token itself.
     */
    if (is_while || (avl_lookup(SymTable, name->tok.s, &stemp) == 0))
        return name;

    /*
     * if this is a relocateable symbol, return the symbol token itself.
     *
     * it will be chained with other tokens to form a full expression.  the
     * separation is maintained to allow the psect relocation algorithm to
     * update any expression containing the symbol.
     */
    if (stemp->reloc)
        return name;

    free_tok_s(name);
    
    /*
     * if the value is valid, make a NUMBER token for the value
     */
    if (stemp->valid)
        return make_value_token(stemp->value[1], stemp->value[0]);

    /*
     * non-valid, non relocatable symbol.
     *
     * This means that somewhere in the expression a relocateble symbol is
     * referenced.  It will not be able to be resolved until all assembly is
     * complete.
     *
     * Therefore, we return a copy of the expression that defines the symbol.
     */
    return copy_expr(stemp->expr);
}

/*****************************************************************************/
/*****************************************************************************/
/* Symbolics Evaluation Routines:					     */
/*****************************************************************************/
/*****************************************************************************/
TOKEN_LIST* eval_subexpr(TOKEN_LIST *op1, TOKEN_LIST* op2, TOKEN_LIST* oper)
{
    SENTRY *op1s, *op2s;
    TOKEN_LIST *retval, *temp1, *temp2;

    if ((op1->next != NULL) || (op2 && (op2->next != NULL))) {
        /*
         * not single element expressions.
         */
        return NULL;
    }
    
    if ((op1->type == NUMBER) && (!op2 || (op2->type == NUMBER))) {
        /* both operands are constants, let's evaluate it */
        switch (oper->type) {
        case '+':
            retval = addq(op1, op2);
            break;
        case '-':
            retval = subq(op1, op2);
            break;
        case '*':
            retval = mulq(op1, op2);
            break;
        case '/':
            retval = divq(op1, op2);
            break;
        case '@':
            retval = shiftq(op1, op2);
            break;
        case '&':
            retval = andq(op1, op2);
            break;
        case '!':
            retval = orq(op1, op2);
            break;
        case '\\':
            retval = xorq(op1, op2);
            break;
        case LOG_AND:
            retval = log_andq(op1, op2);
            break;
        case LOG_OR:
            retval = log_orq(op1, op2);
            break;
        case NOT:
            retval = notq(op1);
            break;
        case LOG_NOT:
            retval = log_notq(op1);
            break;
        case UMINUS:
            retval = subq(op2 = create_zero_tok(), op1);
            break;
        }

        free_tok(op1);
        free_tok(op2);
        free_tok(oper);
        
        return retval;
    }

    if ((oper->type != '-') || (op2 == NULL)) {
        /*
         * operation is not subtraction.
         */
        return NULL;
    }
    
    if (((op1->type != SYMBOL) && (op1->type != LOCAL_LABEL)) ||
        ((op2->type != SYMBOL) && (op2->type != LOCAL_LABEL))) {
        /*
         * one of the operands is not a symbol.
         */
        return NULL;
    }

    if (avl_lookup(SymTable, op1->tok.s, &op1s) == 0) {
        /*
         * op1 is undefined.
         */
        return NULL;
    }
    
    if (avl_lookup(SymTable, op2->tok.s, &op2s) == 0) {
        /*
         * op2 is undefined.
         */
        return NULL;
    }
    
    if (!op1s->reloc || !op2s->reloc || (op1s->sect != op2s->sect)) {
        /*
         * if both symbols are not relocateable, or they are from different
         * psects, unable to evaluate.
         */
        return NULL;
    }

    /*
     * Relocateable symbols should always be valid. (define_label()
     * makes sure of that).
     */
    assert(op1s->valid);
    assert(op2s->valid);
    
    /*
     * difference of two reolcateable symbols from the same psect is
     * a constant.
     */
    temp1 = make_value_token(op1s->value[1], op1s->value[0]);
    temp2 = make_value_token(op2s->value[1], op2s->value[0]);
    retval = subq(temp1, temp2);

    free_tok(temp1);
    free_tok(temp2);
    free_tok_s(op1);
    free_tok_s(op2);
    free_tok(oper);
    
    return retval;
}

#ifdef __STDC__
TOKEN_LIST *pop_token(TOKEN_LIST **head)
#else
TOKEN_LIST *pop_token(head)
TOKEN_LIST **head;
#endif
{
  TOKEN_LIST *temp = *head;

  if (*head != NULL)
    *head = (*head)->next;

  return temp;
}

#ifdef __STDC__
void push_token(TOKEN_LIST *temp, TOKEN_LIST **stack)
#else
void push_token(temp, stack)
TOKEN_LIST *temp, **stack;
#endif
{
  if (temp != NULL) {
    temp->next = *stack;
    *stack = temp;
  }

  return;
}

/* This function will convert all non-relocateable sym's to number tokens within an expression */
TOKEN_LIST* sym2num(TOKEN_LIST *expr)
{
  SENTRY *stemp;
  TOKEN_LIST *temp;
  for (temp = expr; temp; temp = temp->next)
    if (temp->type == SYMBOL && avl_lookup(SymTable, temp->tok.s, &stemp) && !stemp->reloc) {
      FREE(temp->tok.s);
      temp->type = NUMBER;
      temp->tok.q.v[0] = stemp->value[0];
      temp->tok.q.v[1] = stemp->value[1];
    }
  return expr;
}
      
  

TOKEN_LIST* eval_expr(TOKEN_LIST *expr, int* reloc_used)
{
    TOKEN_LIST *stack = NULL;

    if (reloc_used != NULL)
        *reloc_used = 0;
    
    while (expr != NULL) {
        SENTRY *stemp;
        TOKEN_LIST *op1, *op2, *opresult;

        switch (expr->type) {
        case SYMBOL:
        case LOCAL_LABEL:
            /*
             * get the symbol out of the symbol table.
             */
            if (avl_lookup(SymTable, expr->tok.s, &stemp) == 0) {
                errout(E_ERR, "eval_absexpr", "undefined symbol(%s) in absolute expression.\n", expr->tok.s);
                return NULL;
            }

            /*
             * If not valid, then bad things have happened.
             *
             * The only symbols that can remain in an expression are
             * labels and undefined symbols.  Labels are always marked valid
             * at definition time, and undefined symbols were handled above.
             */
            if (!stemp->valid) {
                errout(E_ERR, "eval_absexpr", "internal error, \"%s\" is not valid.\n", stemp->symbol);
                return NULL;
            }

            /*
             * Mark that a relocateable symbol was used in this expression.
             */
            if (reloc_used != NULL)
                *reloc_used = 1;
    
            /*
             * push a NUMBER token on the operation stack.
             */
            opresult = alloc_tok();
            opresult->type = NUMBER;
            opresult->tok.q.v[1] = stemp->value[1];
            opresult->tok.q.v[0] = stemp->value[0];
            
            push_token(opresult, &stack);
            break;
        case NUMBER:
            /* push operand on stack */
            opresult = alloc_tok();
            memcpy(opresult, expr, sizeof(TOKEN_LIST));
            
            push_token(opresult, &stack);
            break;
        case '+':
        case '-':
        case '*':
        case '/':
        case '@':
        case '&':
        case '!':
        case '\\':
        case LOG_AND:
        case LOG_OR:
            op2 = pop_token(&stack);
            op1 = pop_token(&stack);
      
            if ((op1 == NULL) || (op2 == NULL)) {
                errout (E_ERR, "eval_absexpr", "Illegal number of operands for two operand operator\n");
                break;
            }
      
            if ((op1->type == NUMBER) && (op2->type == NUMBER)) {
                /* both operands are constants, let's evaluate it */
                switch (expr->type) {
                case '+':
                    opresult = addq(op1, op2);
                    break;
                case '-':
                    opresult = subq(op1, op2);
                    break;
                case '*':
                    opresult = mulq(op1, op2);
                    break;
                case '/':
                    opresult = divq(op1, op2);
                    break;
                case '@':
                    opresult = shiftq(op1, op2);
                    break;
                case '&':
                    opresult = andq(op1, op2);
                    break;
                case '!':
                    opresult = orq(op1, op2);
                    break;
                case '\\':
                    opresult = xorq(op1, op2);
                    break;
                case LOG_AND:
                    opresult = log_andq(op1, op2);
                    break;
                case LOG_OR:
                    opresult = log_orq(op1, op2);
                    break;
                default:
                    errout(E_ERR, "eval_absexpr", "this couldn't have happened.\n");
                    return NULL;
                }
                /* put result back on stack */
                push_token(opresult, &stack);
            } else {
                errout(E_ERR, "eval_absexpr", "one or both operands are not NUMBER tokens.\n");
                return NULL;
            }

            free_tok(op1);
            free_tok(op2);
      
            break;
        case LOG_NOT:
        case NOT:
            op1 = pop_token(&stack);

            if (op1 == NULL) {
                errout (E_ERR, "eval_absexpr", "no operands for a one operand operator\n");
                break;
            }

            if (op1->type == NUMBER) {
                /* operand is a constant, let's evaluate it */
                switch (expr->type) {
                case LOG_NOT:
                    opresult = log_notq(op1);
                    break;
                case NOT:
                    opresult = notq(op1);
                    break;
                default:
                    errout(E_ERR, "eval_absexpr", "this couldn't have happened either.\n");
                    return NULL;
                }
                /* put result back on stack */
                push_token(opresult, &stack);
            } else {
                errout(E_ERR, "eval_absexpr", "operand is not a NUMBER token.\n");
                return NULL;
            }

            free_tok(op1);

            break;
        default:
            break;
        }
        expr = expr->next;
    }  

    assert(stack->next == NULL);

    return stack;
}

void symfree(e)
SENTRY *e;
{
    RNODE* tref;
    RNODE* next_ref;
    for (tref = e->ref; tref != NULL; tref = next_ref) {
        next_ref = tref->next;
        free_rnode(tref);
    }
    free_tok_list_s(e->expr);
    /* name (e->symbol) will be freed by avl_free_table call */
    FREE(e);
}
