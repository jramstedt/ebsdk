/*****************************************************************************/
/*FILE: HAL.C	Main Routines	                                             */
/*****************************************************************************
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
 *****************************************************************************/
/* Originally Created by: Homayoon Akhiani	April,1992		     */
/*****************************************************************************
! REVISION HISTORY:
! Who   When            What
!---------------------------------------------------------------
! DHA   06-May-1994     merged with pp.
! HA	01-Oct-1993	fix very obsecure problem with symresolve/. movements 
! HA	14-Aug-1992	Sline assertion detected and fixed
! HA	13-Aug-1992	The 13th: end of PSECT recalculation bug 
! HA	12-Aug-1992	Left over from previous FixBug!
! HA	29-Jul-1992	FixBug: .=100 follow by .=80,Listing,piped input file
! HA	22-May-1992	PSect are added
! HA	03-May-1992	Adding Steve Morris idea for listing
! HA	28-Apr-1992	Add -size option
! HA    23-Apr-1992	Include stdlib.h (for portability)    
! HA    xx-Apr-1992     First pass
 *****************************************************************************/
/*****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#ifdef VMS
#include <unixio.h>
#include <file.h>
#else
#ifdef THINK_C
#include <console.h>
#include <unix.h>
#include <fcntl.h>
#else
#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#else
#include <sys/file.h>
#include <sys/types.h>
#include <unistd.h>
#endif /* WINNT */
#endif /* THINK_C */
#endif /* VMS */

#include "hal.h"

B32		opcode,			/* Assembled instruction */
		hpc, pc, 		/* Program Counter */
		ic;			/* Instruction counter: pc/4 */
B32		Sa[2];			/* Start offset address */
B32		Se[2];			/* End address */
FILE	  	*Olist;			/* Output for Listing */
FILE		*Ot;		        /* Output for Symbol table transaction*/
FILE*		codefile;		/* Output for Machine code */
int             codestart;              /* File offset for start of ouput file */
B32		*m;			/* THE IMAGE */
int 		Trace;
avl_tree        *SymTable;		/* AVL data base for symbols*/
avl_tree        *SecTable;		/* AVL data base for Psects*/
dll             *DSecTable;             /* DLL data base for Dsects*/
char		*output_file;
char            **idirs;                /* List of include directories */
PENTRY		*Cpsect;		/* Current PSECT */

int wrote_memory = 0;

void hal_init_before()
{
    ic = Trace = Sa[1] = Sa[0] = 0;
}

void hal_init_after()
{
    SymTable = avl_init_table(strcmp);
    SecTable = avl_init_table(strcmp);
    DSecTable = dll_init();
    Cpsect = NULL;
    
    cur_psect = make_token("0000", SYMBOL);
    set_psect(cur_psect->tok.s, DEFAULT_PSECT_ALIGN, PENTRY_TYPE_PSECT);
    
    return;
}

void hal_uninit()
{
    avl_foreach(SymTable,symfree,AVL_FORWARD);
}

/*****************************************************************************/
/* Align PC								     */
/*****************************************************************************/
apc(d)
    unsigned int d;
{
    int a[2],b[2],c[2];
    unsigned int tmp;
    d = (1 << d) - 1;  /* 2^d - 1 */
    if(!(pc & d)) return;
    pc &= ~d;
    tmp = pc & d;
    d++;
    a[1]=hpc; a[0]=pc;
    b[1]=0x0; b[0]=d;
    add64(a,b,c);
    hpc=c[1];
    pc=c[0];
/*    ic = pc >> 2;  -- old way that was incorrect with large PC's */
    ic += (d - tmp)>>2;   /* this is the amount the pc is going to be incremented by*/

    /*
     * Increase psect size if "." has moved beyond end of current size.
     */
    if (ic >= Cpsect->lwlen)
        increase_psect_size(ic);

    return;
}

/*****************************************************************************/
/* Align PC with filling						     */
/*****************************************************************************/
apcf(d,fv)
    B32 d, fv;
{
    int a[2],b[2],c[2];
    B32	sic;
    d = (1 << d) - 1;  /* 2^d - 1 */
    if(!(pc & d)) return;
    pc &= ~d;
    d++;
    a[1]=hpc; a[0]=pc;
    b[1]=0x0; b[0]=d;
    add64(a,b,c);
    hpc=c[1];
    pc=c[0];
    sic=ic;
    ic = pc >> 2;

    /*
     * write_mem_array() will handle increasing the psect size if needed.
     */
    for(;sic<ic;sic++) write_mem_array(sic, fv);
    return;
}

void increase_psect_size(int numlws)
{
    int oldlwlen = Cpsect->lwlen;

    do {
        Cpsect->len += DEFAULT_PSECT_LENGTH;
        Cpsect->lwlen += DEFAULT_PSECT_LENGTH_LW;
    } while (Cpsect->lwlen <= numlws);
    
    dbgout("increasing size of psect %s to %dKb maximum.\n", Cpsect->name, Cpsect->len/1024);

    Cpsect->mp = realloc(Cpsect->mp, Cpsect->len);
    if (Cpsect->mp == NULL) {
        errout(E_ERR, "get_mem_array", "insufficient virtual memory, unable to increase size of psect %s.\n", Cpsect->name);
        exit(EXIT_FAILURE);
    }

    Cpsect->wrnum = realloc(Cpsect->wrnum, Cpsect->len);
    if (Cpsect->wrnum == NULL) {
        errout(E_ERR, "get_mem_array", "insufficient virtual memory, unable to increase size of psect %s.\n", Cpsect->name);
        exit(EXIT_FAILURE);
    }

    for (; oldlwlen < Cpsect->lwlen; ++oldlwlen) {
        Cpsect->mp[oldlwlen] = 0;
        Cpsect->wrnum[oldlwlen] = 0;
    }

    m = Cpsect->mp;
}

B32 get_mem_array(B32 ic)
{
    if (ic >= Cpsect->lwlen)
        increase_psect_size(ic);

    return m[ic];
}

void write_mem_array(B32 ic, B32 lw)
{
    if (ic >= Cpsect->lwlen)
        increase_psect_size(ic);

    wrote_memory = 1;
    m[ic] = lw;
    ++Cpsect->wrnum[ic];
    dbgout("Write to ic %08X [%08X] psect %s\n", ic, lw, Cpsect->name);
}

int sect_lookup(st, dst, s, e, entry_type)
    avl_tree *st;
    dll *dst;
    char *s;
    PENTRY **e;
    int entry_type;
{
    return(avl_lookup(st, s, e));
}

void sect_free(PENTRY *e)
{
    /* name will be freed by avl_free_table */
    FREE(e->mp);
    FREE(e->wrnum);
    if (e->o_psect != NULL) {
        FREE(e->o_psect->name);	/* this one's not in the avl table */
        sect_free(e->o_psect);
    }
    if (e->link_after_name != NULL)
        FREE(e->link_after_name);
    FREE(e);
}

/*****************************************************************************/
/* SET_PSECT:								     */
/* If align = 0 then it is a default allignment				     */
/*****************************************************************************/
set_psect(s,align,entry_type)
char s[];
unsigned int align;
int entry_type;
{
    PENTRY *e;
    int    i;
    static int did_psect = 0;

    if (entry_type==PENTRY_TYPE_PSECT && SecTable->root) /* HAL always does a psect called "0000" first, so ignore that one */
      did_psect = 1;

    if (did_psect && (entry_type==PENTRY_TYPE_DSECT_HDR || entry_type==PENTRY_TYPE_DSECT))
      errout(E_ERR | E_NO_LNUM, "set_psect", "Both DSECTs and PSECTs specified.  Only one type can be used.\n");

    if ((entry_type==PENTRY_TYPE_DSECT_HDR && (e = Cpsect->o_psect)) ||
        sect_lookup(SecTable, DSecTable, s, &e, entry_type) == 0) {
            /* Create a new Psect */
            psect_save(Cpsect);
            
            if(align == 0) align = DEFAULT_PSECT_ALIGN;

            if ((e = ALLOC(PENTRY)) == 0) {
                errout(E_ERR, "set_psect", "insufficient memory, allocating psect structure.\n");
                exit(EXIT_FAILURE);
            }

            e->len = DEFAULT_SECT_INIT_LENGTH;
            e->lwlen = DEFAULT_SECT_INIT_LENGTH_LW;

#if defined(VMS) || defined(sun)
            m = (B32 *)malloc(e->len);
            e->wrnum = (B32 *)malloc(e->len);
#else
            m = (B32 *)realloc(NULL, e->len);
            e->wrnum = (B32 *)realloc(NULL, e->len);
#endif
            if ((m == NULL) || (e->wrnum == NULL)) {
                errout(E_ERR, "set_psect", "insufficient memory, allocating psect memory.\n");
                exit(EXIT_FAILURE);
            }

            /*
             * zero out psect memory.
             */
            for (i=0; i < e->lwlen; ++i) {
                m[i] = 0;
                e->wrnum[i] = 0;
            }
            
            e->name         = strcpy(ALLOC_STR(strlen(s)), s);
            e->mp           = m;
            e->align        = align;
            e->base.ulong[0]= 0;
            e->base.ulong[1]= 0;
            e->offset.ulong[0]= 0;
            e->offset.ulong[1]= 0;
            e->mpc.ulong[0] = 0;
            e->mpc.ulong[1] = 0;
            e->cpc.ulong[0] = 0;
            e->cpc.ulong[1] = 0;
            e->entry_type = entry_type;
            e->hdr_size = DEFAULT_DSECT_HEADER_LENGTH;
            e->file_offset = 0;
            e->code_size = 0;
            e->o_psect = NULL;
            e->link_address = 0;
            e->link_attr = 0;
            e->link_after = 0;
            e->link_after_name = NULL;
            e->link_first = 0;
            e->link_last = 0;
            e->link_size.ulong[0] = 0;
            e->link_size.ulong[1] = 0;
            e->mem_region = 0;

            if (entry_type == PENTRY_TYPE_DSECT_HDR)
                Cpsect->o_psect = e;
                        
            Cpsect = e;
            hpc = pc = ic = Se[0] = Se[1] = 0;
            if (entry_type != PENTRY_TYPE_DSECT_HDR) {
                avl_insert(SecTable, e->name, e);
            }
            if (entry_type == PENTRY_TYPE_DSECT) {
                dll_insert(DSecTable, e);
            }
            dbgout("InsertPSectDef %s\n",e->name);
        }
    else
        {
            if ((e->entry_type != entry_type)) {
                errout(E_ERR, "set_psect", "redefinition of section type on section %s.\n",e->name);
                exit(EXIT_FAILURE);
            }
                
            if(align != 0 && e->align != align)
                {
                    errout(E_WARN, "set_psect", "attempting to change alignment on psect %s\n",s);
                }
            psect_save(Cpsect);
            Cpsect = e;
            psect_restore(Cpsect);
            ic = pc >> 2;
        }
}
/*****************************************************************************/
/* PSECT_SAVE:								     */
/*****************************************************************************/
psect_save(ps)
PENTRY *ps;
{
 if(ps == NULL) return;

 if(Se[1] <= hpc)   /* Make sure the Se are indeed Max PC */
  {
   Se[1]=hpc;
   if(Se[0] <= pc) Se[0]=pc;
  };
 ps->mpc.ulong[0] = Se[0];
 ps->mpc.ulong[1] = Se[1];
 ps->cpc.ulong[0] = pc;
 ps->cpc.ulong[1] = hpc;
}
/*****************************************************************************/
/* PSECT_RESTORE:							     */
/*****************************************************************************/
psect_restore(ps)
PENTRY *ps;
{
 if(ps == NULL) return;
 Se[0] = ps->mpc.ulong[0];
 Se[1] = ps->mpc.ulong[1];
  pc   = ps->cpc.ulong[0];
 hpc   = ps->cpc.ulong[1];
 m     = ps->mp;
}
/*****************************************************************************/
/* PSECTCALC:								     */
/*  Calculate the FINAL address of the PSects				     */
/*****************************************************************************/
void psectcalc(s,e)
    char   *s;
    PENTRY *e;
{
    static B64 pre;
    static char *pre_name;
    static int pre_rgn = -1;  /* no region can ever be -1 */

    if (e->entry_type == PENTRY_TYPE_DSECT_HDR) 
        return;

    psect_restore(e);

    if (e->mem_region!=pre_rgn && (e->entry_type!= PENTRY_TYPE_DSECT || !e->link_address)) { 
        e->base.ulong[0] = Sa[0];
        e->base.ulong[1] = Sa[1];
    }
    else {
          /* Check if DSECT with hard-coded base address */
          if (e->entry_type==PENTRY_TYPE_DSECT && e->link_address) {
              pc = e->base.ulong[0];
              hpc = e->base.ulong[1];
              apc(e->align);
              /* check if link_address was changed by the align argument of .dsect, give warning */
              if (pc != e->base.ulong[0] || hpc != e->base.ulong[1]) {
                  errout(E_WARN | E_NO_LNUM, "psectcalc",
                         "Dsect %s link address of %08x%08x was changed to %08x%08x to obey its alignment argument\n",
                         e->name,e->base.ulong[1],e->base.ulong[0],hpc,pc);
              }
              /* also check for overlap errors */
              if (!(e->link_attr & D_OVERLAP_OK) &&
                   (hpc < pre.ulong[1] || (hpc == pre.ulong[1] && pc < pre.ulong[0]))) {
                  errout(E_ERR | E_NO_LNUM, "psectcalc",
                         "Dsect '%s', with a .link_address of %08x%08x, overlaps the last Dsect '%s' who ended at %08x%08x\n",
                         e->name,hpc,pc,pre_name,pre.ulong[1],pre.ulong[0]);
             }
          }
          else {
             pc = pre.ulong[0];
             hpc= pre.ulong[1];
             apc(e->align);
          }
          e->base.ulong[0] = pc;
          e->base.ulong[1] = hpc;
    }

    dbgout("ReCalculating Psect %s position ->Base(rgn:%d):[%08X %08X] ",
           e->name,e->mem_region,e->base.ulong[1],e->base.ulong[0]);
 
    pre_name = e->name;
    pre_rgn = e->mem_region;
    if (e->link_size.ulong[1] || (e->link_size.ulong[0] > e->mpc.ulong[0]))
      add64((int32*)e->base.ulong, (int32*)e->link_size.ulong, (int32*)pre.ulong);
    else
      add64((int32*)e->base.ulong, (int32*)e->mpc.ulong, (int32*)pre.ulong);

    /* now place in the offset -- notice how the offset doesn't affect other D/PSECT */
    /* address calculations -- the desired affect */
    e->base.ulong[0] |= e->offset.ulong[0];
    e->base.ulong[1] |= e->offset.ulong[1];

    dbgout("End:[%08X %08X]\n",pre.ulong[1],pre.ulong[0]);
}
/*****************************************************************************/
/* PSECTLIST:								     */
/*****************************************************************************/
void psectlist(s,e)
char   *s;
PENTRY *e;
{
    int32 result[2];

    fprintf(listing_file,"%-20s  %-9d  %8d  [%08X %08X]\n",
            e->name,e->align,e->mpc.ulong[0],e->base.ulong[1],e->base.ulong[0]);
}
/*****************************************************************************/
/* SPC:	switch pc, change the pc to h,l	and update ic			     */
/*****************************************************************************/
spc(h,l)
    B32 h,l;
{
    dbgout("******DOT MOVING[%08X %08X]->[%08X %08X]\n",hpc,pc,h,l);
    if(Se[1] <= hpc)   /* remember the Max PC */
        {
            Se[1]=hpc;
            if(Se[0] <= pc) Se[0]=pc;
        };
    hpc=h;
    pc=l;
    ic = pc >> 2;

    /*
     * Increase psect size if "." has moved beyond end of current size.
     */
    if (ic >= Cpsect->lwlen)
        increase_psect_size(ic);

}

 /*
 * Increment PC
 *
 * update of ic must be done elsewhere.
 */
void ipc(int d)
{
    int a[2],b[2],c[2];
    
    a[1]=hpc; a[0]=pc;
    b[1]=0x0; b[0]=d;
    
    add64(a,b,c);
    
    hpc=c[1];
    pc=c[0];
    
    /*
     * Increase psect size if "." has moved beyond end of current size.
     */
    if ((pc >> 2) >= Cpsect->lwlen)
        increase_psect_size(pc >> 2);

    return;
}

void sect_foreach(st, dst, func_p, func_d, dir)
    avl_tree *st;
    dll *dst;
    void (*func_p)();
    void (*func_d)();
    int dir;
{
  if (dst->head == NULL) 
    avl_foreach(st, func_p, dir ? AVL_FORWARD : AVL_BACKWARD);
  else 
    dll_foreach(dst, func_d, dir ? DLL_FORWARD : DLL_BACKWARD);
}
            

/*****************************************************************************/
/* DGENHDR:								     */
/*****************************************************************************/
void dgenhdr(s,p)
char   *s;
PENTRY *p;
{
    int status;
    DSECT_HDR hdr;
    int temp;
    char zero = '\0';
  
    if (p == NULL) return;
    if (p->entry_type != PENTRY_TYPE_DSECT) return;  /* Only create header for DSect PENTRYs */

    /*
    ** Save file location of header
    */
    p->hdr_pos = ftell(codefile);
    if (p->hdr_pos == -1) {
        errout(E_ERR | E_NO_LNUM, "dgenhdr", "image seek problem, aborting seek of header for dsect %s: %s\n",
               p->name, strerror(errno));
        return;
    }

    /*
    ** Write header place holders
    */
    hdr.mbz_0 = hdr.mbz_1 = hdr.mbz_2 = 0;
    hdr.hdr_size = p->hdr_size;
    hdr.file_offset = p->file_offset;
    hdr.code_size = p->code_size;
    if ((status = fwrite((char *)(&hdr), sizeof(hdr), 1, codefile)) == -1) {
        errout(E_ERR | E_NO_LNUM, "dgenhdr", "image write problem, aborting write of header for dsect %s: %s\n",
               p->name, strerror(errno));
        return;
    }

    /*
    ** Write DSect name
    */
    temp = strlen(s) + 1;
    if (s != NULL) {
        status = fwrite(s, temp, 1, codefile);
        if (status == -1) {
            errout(E_ERR | E_NO_LNUM, "dgenhdr", "image write problem, aborting write of header name for dsect %s: %s\n",
                   p->name, strerror(errno));
            return;
        }
        /* zero fill file until quadword aligned */
        while (temp++ % 8 != 0) {
            status = fwrite(&zero, 1, 1, codefile);
            if (status == -1) {
                errout(E_ERR | E_NO_LNUM, "dgenhdr", "image write problem, aborting write of header name for dsect %s: %s\n",
                       p->name, strerror(errno));
                return;
            }
        }
        p->hdr_size += temp-1;
    }
        
    /*
    ** Write other header information if required
    */
    if (p->o_psect != NULL) {
        p->hdr_size += p->o_psect->mpc.ulong[0];     /* this is OK, since alignment is always zero */
        status = fwrite((char *)p->o_psect->mp, p->o_psect->mpc.ulong[0], 1, codefile);
        if (status == -1) {
            errout(E_ERR | E_NO_LNUM, "dgenhdr", "image write problem, aborting write of header other info. for dsect %s: %s\n",
                   p->name, strerror(errno));
            return;
        }
    }
}

/*****************************************************************************/
/* DREGENHDR:								     */
/*****************************************************************************/
void dregenhdr(s,p)
char   *s;
PENTRY *p;
{
    int status;
    DSECT_HDR hdr;
  
    if (p == NULL) return;
    if (p->entry_type != PENTRY_TYPE_DSECT) return;  /* Only update header for DSect PENTRYs */

    /*
    ** Get file location of header
    */
    status = fseek(codefile, p->hdr_pos, SEEK_SET);
    if (status == -1) {
        errout(E_ERR | E_NO_LNUM, "dregenhdr", "image seek problem, aborting seek of header for dsect %s: %s\n",
               p->name, strerror(errno));
        return;
    }

    /*
    ** Write real header values 
    */
    hdr.mbz_0 = hdr.mbz_1 = hdr.mbz_2 = 0;
    hdr.hdr_size = p->hdr_size;
    hdr.file_offset = p->file_offset;
    hdr.code_size = p->code_size;
    if ((status = fwrite((char *)(&hdr), sizeof(hdr), 1, codefile)) == -1) {
        errout(E_ERR | E_NO_LNUM, "dregenhdr", "image write problem, aborting write of header for dsect %s: %s\n",
               p->name, strerror(errno));
        return;
    }
}

/*****************************************************************************/
/* DGENCODE:								     */
/*****************************************************************************/
void dgencode(s,p)
char   *s;
PENTRY *p;
{
    int	     status;

    if (p->entry_type != PENTRY_TYPE_DSECT) return;
    
    /*
    ** Determine code/data offset
    */
    p->file_offset = ftell(codefile);
    if (p->file_offset == -1) {
        errout(E_ERR | E_NO_LNUM, "dgencode", "image seek problem, aborting seek for dsect %s: %s\n",
               p->name, strerror(errno));
        return;
    }
    p->file_offset -= codestart;
    

    if (p->mpc.ulong[0] != 0) {
        dbgout("Writing %d bytes from DSECT %s into %s\n", p->mpc.ulong[0], p->name, output_file);
        status = fwrite((char *)p->mp, p->mpc.ulong[0], 1, codefile);
    
        if(status != 1) {
            errout(E_ERR | E_NO_LNUM, "pgencode", "image write problem, aborting write of dsect %s: %s\n",
                   p->name, strerror(errno));
            return;
        }
    }
    
    p->code_size = p->mpc.ulong[0];
    return;
}


/*****************************************************************************/
/* PGENCODE:								     */
/*****************************************************************************/
void pgencode(s,p)
char   *s;
PENTRY *p;
{
    static int   cb = 0;
    B32	     v[2], zero;
    int	     status;

/* SUB64(p->cpc.ulong,p->base.ulong,v);
   assert(v[1] == 0);  */

    zero= 0;
    pc  = cb;
    hpc = 0;
    apc(p->align);
    if(pc != cb) {
        int oldcb = cb;

        for(;pc != cb;cb += 4) {
            status = fwrite((char *)&zero, 4, 1, codefile);
            if(status != 1) {
                errout(E_ERR | E_NO_LNUM, "pgencode", "image write problem, aborting write of psect %s: %s\n", p->name, strerror(errno));
                return;
            }
            
            assert(pc >= cb);
        }
        
        errout(E_INFO | E_NO_LNUM, "hal", "advancing PC from %08X to %08X in PSECT %s\n", oldcb, pc, p->name);
    }

    if (p->mpc.ulong[0] != 0) {
        dbgout("Writing %d bytes from PSECT %s into %s\n", p->mpc.ulong[0], p->name, output_file);
        status = fwrite((char *)p->mp, p->mpc.ulong[0], 1, codefile);
    
        if(status != 1) {
            errout(E_ERR | E_NO_LNUM, "pgencode", "image write problem, aborting write of psect %s: %s\n", p->name, strerror(errno));
            return;
        }
        
        cb += p->mpc.ulong[0];
    }
    
    return;
}
/*****************************************************************************/
/* GENERATE_CODE:							     */
/*****************************************************************************/
generate_code()
{
    int status;
    char zero = '\0';

#ifdef UNIX
    {
        char tmp_flnm[PATH_MAX + 1];
        UNIX_expand_pathname(output_file, tmp_flnm);
        codefile = fopen(tmp_flnm, "w");
    }
#else
#ifdef _WIN32
    codefile = fopen(output_file, "wb");
#else
    codefile = fopen(output_file, "w");
#endif
#endif

    if (codefile == NULL) {
        errout(E_ERR | E_NO_LNUM, "hal", "unable to open output file, aborting write: %s\n", strerror(errno));
        return;
    }
    
    if (DSecTable->head != NULL) {
        codestart = ftell(codefile);
        if (codestart == -1) {
            errout(E_ERR | E_NO_LNUM, "hal", "unable to seek output file, aborting seek: %s\n", strerror(errno));
            return;
        }
        dll_foreach(DSecTable, dgenhdr, DLL_FORWARD);
        /* write dummy header record to signify end */
        status = fwrite((char *)&zero, 8, 1, codefile);
        if(status != 1) {
            errout(E_ERR | E_NO_LNUM, "generate_code", "image write problem, aborting write of header end record: %s\n", strerror(errno));
            return;
        }
    }

    sect_foreach(SecTable, DSecTable, pgencode, dgencode, SECT_FORWARD);

    if (DSecTable->head != NULL) {
        dll_foreach(DSecTable, dregenhdr, DLL_FORWARD);
    }
            
    fclose(codefile);
    return;
}
/*****************************************************************************/

void check_for_ic_ref(TOKEN_LIST** tref_p, int ic)
{
    TOKEN_LIST* tref = *tref_p;
    RNODE* rn;
    
    if (tref == NULL)
        return;

    rn = tref->tok.ref;

    /* If this longword, or the second LW in a qw reference. */
    if ((ic == rn->ic) ||
        (((rn->type == TADD) || (rn->type == TD64)) && (ic == (rn->ic+1)))) {
        fgetpos(listing_file, &rn->lpos[rn->listused++]);
        if (((rn->type != TADD) && (rn->type != TD64)) ||
            (rn->listused == 2)) {
            *tref_p = tref->next;
        }
    }

    return;
}

void do_list(int type, int baseline)
{
    static int first_call = 1;
    static char* last_fname;
    static PENTRY* last_psect = NULL;
    static int last_ic = 0;
    char* new_fname;
    char* tstr;
    char* ntstr;
    int cnt;
    int in_macro, in_repeat, in_if, in_while;
    char ch;
    int delta_ic;
    TOKEN_LIST* tref;
    RNODE* rn;
    INPUT_FILE *tfil;
    
    if (listing_file == NULL)
        return;

    if (listing_enable <= 0) {
        goto save_lasts;
    }

    /*
     * If caller told us to only print certain times, and none of those
     * times are enabled, return.
     */
    if (!(listing_flags & type)) {
        goto save_lasts;
    }
    
    /*
     * If we are processing a macro, and expansions are not
     * enabled, then return.
     */
    in_macro = (macstk != NULL);
    in_repeat = in_body(REPEAT);
    in_while = in_body(WHILE);
    if ((in_macro || in_repeat || in_while) &&/* processing a macro or .repeat block AND */
        !(listing_flags & type & L_BINARY) &&	/* !(BINARY is requested AND this line was a binary) AND */
        !(listing_flags & type & L_CALLS) &&	/* !(CALLS is requested AND this line was a call) AND */
        !(listing_flags & type & L_ASSIGN) &&	/* !(Assignments is requested AND this line was one) AND */
        !(listing_flags & type & L_CMDIN) &&	/* !(Command input is requested AND this line was one) AND */
        !(listing_flags & L_EXPANSIONS)) {	/* !(EXPANSIONS is requested) */
        goto save_lasts;
    }

    in_if = in_body(IF) || in_body(IFF) || in_body(IFT) || in_body(IFTF) || in_body(IIF);

#if 1
    new_fname = find_fname(NULL);
    if (first_call || (last_fname != new_fname)) {
        fprintf(listing_file, "File: %s\n", (new_fname != NULL) ? new_fname : "(none)");
    }
#else
    new_fname = NULL;
#endif

    ch = 0;
    for (tfil=cur_ifile; tfil && !ch; tfil = tfil->next)
      if (tfil->string_type == REPEAT)
        ch = in_macro ? 'r' : 'R';
      else if (tfil->string_type == WHILE)
        ch = in_macro ? 'w' : 'W';
      else if (tfil->string_type == IF || tfil->string_type == IFF || tfil->string_type == IFT ||
               tfil->string_type == IFTF ||tfil->string_type == IIF)
        ch = in_macro ? 'i' : 'I';
    if (!ch) { /* i.e. none of the types were found on the program stack */
      if (in_macro)
        ch = 'M';
      else 
        ch = ' ';
    }

    /* compute change in ic from last call */
    delta_ic = ic - last_ic;
    
    tstr = listing_line;
    cnt = 0;
    tref = headref;
    while ((ntstr = strchr(tstr, '\n')) != NULL) {

        /* check for memory allocation */
        if (wrote_memory && (delta_ic > 0) && (last_psect == Cpsect)) {
            
            /* had memory allocation */
            if (delta_ic > 1) {
                check_for_ic_ref(&tref, last_ic+1);
                fprintf(listing_file, "%08X ", m[last_ic+1]);
                --delta_ic;
            } else
                fprintf(listing_file, "         ");
            
            check_for_ic_ref(&tref, last_ic);
            fprintf(listing_file, "%08X [%06X]", m[last_ic], last_ic << 2);
            --delta_ic;
            last_ic += 2;
        } else {
            /*
             * Did not have memory allocation.  Print blank space.
             */
            fprintf(listing_file, "                  [%06X]", ic << 2);
        }

        
        *ntstr = '\0';
        fprintf(listing_file, "%6d%c %s\n", baseline + cnt, ch, tstr);
        tstr = ntstr + 1;
        ++cnt;
    }

    /*
     * Only print remaining longwords if allowed to.
     *
     * This flag is used by the .BLKx directives to prevent the listing
     * file from becoming huge.
     */
    if (!(type & L_NOTALL)) {
        while (wrote_memory && (delta_ic > 0) && (last_psect == Cpsect)) {
            
            if (delta_ic > 1) {
                check_for_ic_ref(&tref, last_ic);
                fprintf(listing_file, "%08X ", m[last_ic+1]);
                --delta_ic;
            } else
                fprintf(listing_file, "         ");
            
            check_for_ic_ref(&tref, last_ic);
            fprintf(listing_file, "%08X [%06X]\n", m[last_ic], last_ic << 2);
            --delta_ic;
            last_ic += 2;
        }
    } else {
        /*
         * Indeed, L_NOTALL was specified.  If it actually prohibited something
         * from going to the listing file, print a couple of ".." lines to
         * let the listing file reader know that.
         */
        if (wrote_memory && (delta_ic > 0) && (last_psect == Cpsect)) {
            int i;

            for (i=0; i<3; ++i)
                fprintf(listing_file, "                     ..\n");
        }
    }
    
    first_call = 0;
    last_fname = new_fname;

  save_lasts:    
    clear_listing_line();

    free_tok_list(headref);
    headref = NULL;
    endref = &headref;

    wrote_memory = 0;

    last_psect = Cpsect;
    last_ic = ic;
    
    return;
}
