/*****************************************************************************/
/*FILE: OSF.C    Routines for the OSF Object module                          */
/****************************************************************************
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
 *****************************************************************************/
/* Originally Created by: Homayoon Akhiani       June,1994                   */
/*****************************************************************************
! REVISION HISTORY:
! Who   When            What
!-----------------------------------------------------------------------------
! CM    xx-Jul-1994     First pass taken over by Chris Mikulis
! HA    22-Jun-1994     First pass
 *****************************************************************************/
/*****************************************************************************/
#ifdef __osf__  /* don't do whole file unless its compiled on OSF */

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
#include <sys/file.h>
#include <sys/types.h>
#include <unistd.h>
#endif
#endif
#include <a.out.h>
#include <stamp.h>
#include <time.h>
#include "hal.h"
#include "version.h"

#define PTBD	NULL
#define TBD	0

FILHDR		OSFhdr;		/*File header*/
AOUTHDR		OSFah;		/*Optional Header*/
SCNHDR		OSFsh;		/*Section Header */
RELOC           OSFreloc;       /*Relocation Table Entry */
HDRR            OSFshdr;        /*Symbolic Header */

void osf_symtableit();
/*****************************************************************************/
int             Ocode;                  /* Output for Object code */
/*****************************************************************************/
/*****************************************************************************/
void osf_object(void)
{ 
  int status, i;
  char version[32];
  int hdr_pos, ah_pos, sh_pos;

  /*STEP 0: Open output file ##################################################*/
  if (output_file) {
#ifdef THINK_C
    Ocode = open(output_file,O_CREAT|O_TRUNC|O_WRONLY);
#else
#  ifdef UNIX
    {
        char tmp_flnm[PATH_MAX + 1];
        UNIX_expand_pathname(output_file, tmp_flnm);
        Ocode = open(tmp_flnm,O_CREAT|O_TRUNC|O_WRONLY,0664);
    }
#  else
    Ocode = open(output_file,O_CREAT|O_TRUNC|O_WRONLY,0664);
#  endif
#endif
  }
  else {
    errout(E_INFO | E_NO_LNUM, "hal", "no output object file specified, no object generated.\n");
  }

  /*STEP 1: Generate Headers ##################################################*/
  OSFhdr.f_magic = ALPHAMAGIC; 
  OSFhdr.f_nscns = 1;
  OSFhdr.f_timdat = time(NULL);
  OSFhdr.f_symptr = PTBD;
  OSFhdr.f_nsyms = TBD;
  OSFhdr.f_opthdr = sizeof(OSFah);
  OSFhdr.f_flags = F_RELFLG | F_EXEC | F_LNNO | F_LSYMS | F_MIPS_NO_REORG | F_MIPS_NO_REMOVE;

  hdr_pos = lseek(Ocode, 0, SEEK_CUR);
  status = write(Ocode, (char *)&OSFhdr, sizeof(OSFhdr));
  
  OSFah.magic = OMAGIC;
  OSFah.vstamp = (MS_STAMP << 8) | LS_STAMP;
  OSFah.bldrev = 0;
  OSFah.tsize = TBD;
  OSFah.dsize = 0;
  OSFah.bsize = 0;
  OSFah.entry = 0;
  OSFah.text_start = 0;
  OSFah.data_start = 0;
  OSFah.bss_start = 0;
  OSFah.gprmask = 0;
  OSFah.fprmask = 0;
  OSFah.gp_value = 0;

  ah_pos = lseek(Ocode, 0, SEEK_CUR);
  status = write(Ocode, (char*)&OSFah, sizeof(OSFah));
  
  strcpy(OSFsh.s_name,".text");
  OSFsh.s_paddr = 0;
  OSFsh.s_vaddr = 0;
  OSFsh.s_size = TBD;
  OSFsh.s_scnptr = PTBD;
  OSFsh.s_relptr = PTBD;
  OSFsh.s_lnnoptr = NULL;
  OSFsh.s_nreloc = TBD;
  OSFsh.s_nlnno = 0;
  OSFsh.s_flags = STYP_TEXT;
  sh_pos = lseek(Ocode, 0, SEEK_CUR);
  status = write(Ocode, (char*)&OSFsh, sizeof(OSFsh));
  

  /*STEP 2: Ouput Section Code ################################################*/
  OSFsh.s_scnptr = lseek(Ocode, 0, SEEK_CUR);
  sect_foreach(SecTable, DSecTable, pgencode, NULL, SECT_FORWARD);
  OSFah.tsize = OSFsh.s_size = lseek(Ocode, 0, SEEK_CUR) - OSFsh.s_scnptr;
  while ((OSFah.tsize % 16) != 0) OSFah.tsize++; 
  
  
  /*STEP 3: Generate Symbol Table #############################################*/
  OSFsh.s_relptr = lseek(Ocode, 0, SEEK_CUR);
  avl_foreach(SymTable,osf_symtableit,AVL_FORWARD);


  /*STEP 4: Generate Symbol Table #############################################*/
  OSFshdr.magic = magicSym;
  OSFshdr.vstamp = OSFah.vstamp;
  OSFshdr.ilineMax = 0;
  OSFshdr.idnMax = 0;
  OSFshdr.ipdMax = 0;
  OSFshdr.isymMax = 0;
  OSFshdr.ioptMax = 0;
  OSFshdr.iauxMax = 0;
  OSFshdr.issMax = 0;
  OSFshdr.issExtMax = 0;
  OSFshdr.ifdMax = 0;
  OSFshdr.crfd = 0;
  OSFshdr.iextMax = 0;
  OSFshdr.cbLine = 0;
  OSFshdr.cbLineOffset = 0;
  OSFshdr.cbDnOffset = 0;
  OSFshdr.cbPdOffset = 0;
  OSFshdr.cbSymOffset = 0;
  OSFshdr.cbOptOffset = 0;
  OSFshdr.cbAuxOffset = 0;
  OSFshdr.cbSsOffset = 0;
  OSFshdr.cbSsExtOffset = 0;
  OSFshdr.cbFdOffset = 0;
  OSFshdr.cbRfdOffset = 0;
  OSFshdr.cbExtOffset = 0;

  OSFhdr.f_symptr = lseek(Ocode, 0, SEEK_CUR);
  OSFhdr.f_nsyms = write(Ocode, (char *)&OSFshdr, sizeof(OSFshdr));

  

  /*STEP 5: Update Headers ####################################################*/
  lseek(Ocode, hdr_pos, SEEK_SET);
  status = write(Ocode, (char *)&OSFhdr, sizeof(OSFhdr));
  lseek(Ocode, ah_pos, SEEK_SET);
  status = write(Ocode, (char*)&OSFah, sizeof(OSFah));
  lseek(Ocode, sh_pos, SEEK_SET);
  status = write(Ocode, (char*)&OSFsh, sizeof(OSFsh));
  close(Ocode);


}
/*****************************************************************************/
/*****************************************************************************/

void osf_symtableit(s,e)
char   *s;
SENTRY *e;
{
  RNODE *n;
  int status;

  /*
  ** If symbol is not valid or not relocatable then return
  */
  if(!e->valid || !e->reloc) return;

  for(n=e->ref; n != NULL; n = n->next) {
    OSFreloc.r_vaddr = (n->sect->base.ulong[1] << 32);
    OSFreloc.r_vaddr |= (n->sect->base.ulong[0]);
    OSFreloc.r_vaddr += (long)(n->ic << 2);

    /* 
    ** For now, we only handle local relocation entries
    */
    OSFreloc.r_symndx = 1;
    OSFreloc.r_extern = 0;
    
    /*
    ** Initilize these fields, they could be changed later
    */
    OSFreloc.r_offset = 0;
    OSFreloc.r_reserved = 0;
    OSFreloc.r_size = 0;
    

    switch(n->type) {
    case TD32:
      /* 32 bit data */
      OSFreloc.r_type = R_REFLONG;
      break;
    case TADD:
    case TD64:
      /* 64 bit data */
      OSFreloc.r_type = R_REFQUAD;
      break;
    case TMDS:
      /* 16 bit displacement */
      /* What about JSR hint??? */
      OSFreloc.r_type = R_SREL16;
      break;
    case TJDS:
      /* 14 bit displacement */
      OSFreloc.r_type = R_HINT;
      break;
    case TBDS:
      /* 21 bit branch displacement */
      OSFreloc.r_type = R_BRADDR;
      break;
    case TPDS:
      /* 26 bit CALLPAL function */
      OSFreloc.r_type = R_OP_PUSH;
      status = write(Ocode, (char*)&OSFreloc, sizeof(OSFreloc));
      OSFsh.s_nreloc++;
      OSFreloc.r_offset = 0;
      OSFreloc.r_size = 26;
      OSFreloc.r_type = R_OP_STORE;
      break;
    case TIOP:
      /* 8 bit immediate data, special case */
      OSFreloc.r_type = R_OP_PUSH;
      status = write(Ocode, (char*)&OSFreloc, sizeof(OSFreloc));
      OSFsh.s_nreloc++;
      OSFreloc.r_offset = 13;
      OSFreloc.r_size = 8;
      OSFreloc.r_type = R_OP_STORE;
      break;
    case TMHW10:
      /* 10 bit HW_LD/ST displacement, special case */
      OSFreloc.r_type = R_OP_PUSH;
      status = write(Ocode, (char*)&OSFreloc, sizeof(OSFreloc));
      OSFsh.s_nreloc++;
      OSFreloc.r_offset = 0;
      OSFreloc.r_size = 10;
      OSFreloc.r_type = R_OP_STORE;
      break;
    case TMHW12:
      /* 12 bit HW_LD/ST displacement, special case */
      OSFreloc.r_type = R_OP_PUSH;
      status = write(Ocode, (char*)&OSFreloc, sizeof(OSFreloc));
      OSFsh.s_nreloc++;
      OSFreloc.r_offset = 0;
      OSFreloc.r_size = 12;
      OSFreloc.r_type = R_OP_STORE;
      break;
    case TMOD:
      /* 6 bit HW_LD/ST mode, special case */
      OSFreloc.r_type = R_OP_PUSH;
      status = write(Ocode, (char*)&OSFreloc, sizeof(OSFreloc));
      OSFsh.s_nreloc++;
      OSFreloc.r_offset = 10;
      OSFreloc.r_size = 6;
      OSFreloc.r_type = R_OP_STORE;
      break;
    default:
      errout(E_INFO | E_NO_LNUM, "osf", "bad symbol at %08X in PSECT %s.\n",n->ic << 2, n->sect->name);
      break;

    }  /* end switch */

    status = write(Ocode, (char*)&OSFreloc, sizeof(OSFreloc));
    OSFsh.s_nreloc++;

  }  /* end for */
}

#endif /* __osf__ */
