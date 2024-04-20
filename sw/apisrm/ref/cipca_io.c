/* file:	cipca_io.c
 *
 * Copyright (C) 1994 by
 * Digital Equipment Corporation, Maynard, Massachusetts.
 * All rights reserved.
 *
 * This software is furnished under a license and may be used and copied
 * only  in  accordance  of  the  terms  of  such  license  and with the
 * inclusion of the above copyright notice. This software or  any  other
 * copies thereof may not be provided or otherwise made available to any
 * other person.  No title to and  ownership of the  software is  hereby
 * transferred.
 *
 * The information in this software is  subject to change without notice
 * and  should  not  be  construed  as a commitment by digital equipment
 * corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by digital.
 *
 *
 * Abstract:	CIPCA io helper.
 *
 * Author: Console firmware team
 *
 */

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:ev_def.h"
#include "cp$src:mem_def.h"
#include "cp$src:msg_def.h"
#include "cp$src:pb_def.h"
#include "cp$src:sb_def.h"
#include "cp$src:nport_q_def.h"
#include "cp$src:nport_def.h"
#include "cp$src:cipca_def.h"
#include "cp$src:ci_cipca_def.h"
#include "cp$src:cipca_sa_ppd_def.h"
#include "cp$src:server_def.h"
#include "cp$src:print_var.h"
#include "cp$src:xmidef.h"
#include "cp$src:xmi_util_def.h"
#include "cp$src:mbx_def.h"
#include "cp$inc:platform_io.h"
#include "cp$inc:prototypes.h"
#include "cp$inc:kernel_entry.h"

#if TURBO || RAWHIDE
extern struct window_head config;
#endif

#define DEBUG 0
#define EMUL_DEBUG 0

#if DEBUG
extern int ci_debug_flag;
#define dqprintf _dqprintf
#define dprintf(fmt, args)	\
    _dprintf(fmt, args)
#define d2printf(fmt, args)	\
    _d2printf(fmt, args)
#else
#define dqprintf qprintf
#define dprintf(fmt, args)
#define d2printf(fmt, args)
#endif

#if DEBUG_MALLOC
#define dm_printf(fmt, args)	\
    _dprintf(fmt, args)
#else
#define dm_printf(fmt, args)
#endif


struct CSRTBL {
    int csr_id;	
    char *csrnam;
    int	base;	
    int offset;
};

#define HARD  0
#define SOFT  1
#define MEM   2
#define CFG   7
#define EMUL  9
#define ADRSPACE MEM
char *basenam[] = {"HARD","SOFT","MEM",0,0,0,0,"CFG",0,"EMUL"};

struct CSRTBL csrtbl[] = {
       {adprst,	 "adprst",  ADRSPACE,  adprst},
       {clrinta, "clrinta", ADRSPACE,  clrinta},
       {clrintb, "clrintb", ADRSPACE,  clrintb},
       {aitcr,   "aitcr",   ADRSPACE,  aitcr},
       {nodests, "nodests", ADRSPACE,  nodests},
       {intena,  "intena",  ADRSPACE,  intena},

       {abbr,    "abbr",    ADRSPACE,  abbr},
       {ccq2ir,  "ccq2ir",  ADRSPACE,  ccq2ir},
       {ccq1ir,  "ccq1ir",  ADRSPACE,  ccq1ir},
       {ccq0ir,  "ccq0ir",  ADRSPACE,  ccq0ir},
       {adfqir,  "adfqir",  ADRSPACE,  adfqir},
       {amfqir,  "amfqir",  ADRSPACE,  amfqir},
       {psr,     "psr",     ADRSPACE,  psr},
       {casr0,   "casr0",   ADRSPACE,  casr0},
       {casr1,   "casr1",   ADRSPACE,  casr1},
       {cafar0,  "cafar0",  ADRSPACE,  cafar0},
       {cafar1,  "cafar1",  ADRSPACE,  cafar1},
       {psrcr,   "psrcr",   ADRSPACE,  psrcr},
       {casrcr,  "casrcr",  ADRSPACE,  casrcr},
       {picr,    "picr",    ADRSPACE,  picr},
       {cicr,    "cicr",    ADRSPACE,  cicr},
       {pecr,    "pecr",    ADRSPACE,  pecr},
       {cecr,    "cecr",    ADRSPACE,  cecr},
       {amtcr,   "amtcr",   ADRSPACE,  amtcr},
       {amtecr,  "amtecr",  ADRSPACE,  amtecr},
       {pmcsr,   "pmcsr/amcsr",   ADRSPACE,  pmcsr},
       {amcsr,   "amcsr/pmcsr",   ADRSPACE,  amcsr},
       {accx,    "accx",    ADRSPACE,  accx},
       {mrev,    "mrev",    ADRSPACE,  mrev},
       {mrev_hw, "mrev_hw", ADRSPACE,  mrev_hw},
       {mrev_fw, "mrev_fw", ADRSPACE,  mrev_fw},
       {musr,    "musr",    ADRSPACE,  musr},
       {mucar,   "mucar",   ADRSPACE,  mucar},
       {alt_clrinta, "alt_clrint1", ADRSPACE, alt_clrinta},
       {alt_aitcr, "alt_aitcr", ADRSPACE, alt_aitcr},
       {alt_intena, "alt_intena", ADRSPACE, alt_intena},


       {xdev,    "xdev",    EMUL,  xdev-emubase},
       {xber,    "xber",    EMUL,  xber-emubase},
       {xfadr,   "xfadr",   EMUL,  xfadr-emubase},
       {xcomm,   "xcomm",   EMUL,  xcomm-emubase},
       {pscr,    "pscr",    EMUL,  pscr-emubase},
       {psdr,    "psdr",    EMUL,  psdr-emubase},
       {pdcsr,   "pdcsr",   EMUL,  pdcsr-emubase},
       {xfaer,   "xfaer",   EMUL,  xfaer-emubase},

       {asnr,    "asnr ",   EMUL,  asnr-emubase },
       {aidr ,   "aidr ",   EMUL,  aidr-emubase },
       {amivr,   "amivr",   EMUL,  amivr-emubase},
       {acivr,   "acivr",   EMUL,  acivr-emubase},
       {compirr, "compirr", EMUL,  compirr-emubase},
       {asubr,   "asubr",   EMUL,  asubr-emubase},
       {aitcr,   "aitcr",   EMUL,  aitcr-emubase},

       {-1,0,0,0}
};

int lookup_csr(int csr_id, int *idx){
  int i=0;
  int status = msg_failure;

  while (csrtbl[i].csr_id != -1) { 
    if (csrtbl[i].csr_id == csr_id){
      *idx = i;
      status = msg_success;
      break;
    }
    i++;
  }

  return status;
}

void dump_csrtbl(int i){
#if DEBUG
  if (ci_debug_flag){
    pprintf("id: %x    %s   %s   offset:%x\n",
                  csrtbl[i].csr_id,
		  csrtbl[i].csrnam,
		  basenam[csrtbl[i].base],
		  csrtbl[i].offset);
  };
#endif
}

unsigned int cipca_write_csr(struct cixcd_blk *blk_ptr, int csr, 
                             unsigned int val){
  int i;
  int status = msg_success;
/*  dprintf("cipca_write_csr\n",p_args0);*/
  if (lookup_csr(csr,&i) == msg_failure){
     pprintf("Failed to find csr: %x\n", csr);
  } else {
     dump_csrtbl(i);
  }  


  if (blk_ptr->pb == 0) {
    pprintf("cipca_write_csr blank pb\n");
    sysfault(0);
  }


  stack_check();


  switch (csrtbl[i].base) {
    unsigned int adr;

    case HARD:    
               adr = blk_ptr->hard_base + csrtbl[i].offset;
#if DEBUG
	       if (ci_debug_flag)
	       pprintf("HARD:outmeml(%x,%x,%x);\n",blk_ptr->pb,adr,val);
#endif
	       outmeml(blk_ptr->pb,adr,val);
               break;

    case SOFT:    
               /*
	       dump_cixcd_blk(blk_ptr);
	       if(blk_ptr->pb)dump_pb(blk_ptr->pb);
               */
               adr = blk_ptr->soft_base + csrtbl[i].offset;
#if DEBUG
	       if (ci_debug_flag)
	       pprintf("SOFT:outmeml(%x,%x,%x);\n",blk_ptr->pb,adr,val);
#endif
	       outmeml(blk_ptr->pb,adr,val);
               break;

    case MEM:
               adr = blk_ptr->mem_base + csrtbl[i].offset;
#if DEBUG
	       if (ci_debug_flag)
	       pprintf("MEM:outmeml(%x,%x,%x);\n",blk_ptr->pb,adr,val);
#endif
	       outmeml(blk_ptr->pb,adr,val);
               break;

    case CFG:
	       pprintf("CFG write: Not yet implemented\n",p_args0);
               break;

    case EMUL:
#if DEBUG
	       pprintf("EMUL - %s no CIPCA parallel\n",csrtbl[i].csrnam);
#endif
#if ( TURBO || RAWHIDE ) && EMUL_DEBUG
	       emul_write(csrtbl[i].offset,val);
#endif
	       break;

    default: 
	       pprintf("Not yet implemented\n");
               break;
  }
               
  return status;
}

unsigned int cipca_read_csr(struct cixcd_blk *blk_ptr, int csr){

  int i;
  int status = msg_success;
  unsigned int retval;

/*  dprintf("cipca_read_csr\n",p_args0);*/
  if (lookup_csr(csr,&i) == msg_failure){
     pprintf("Failed to find csr: %x\n", csr);
  } else {
     dump_csrtbl(i);
  }  


  if (blk_ptr->pb == 0) {
    pprintf("cipca_read_csr blank pb\n");
    sysfault(0);
  }

  stack_check();

  switch (csrtbl[i].base) {
    unsigned int adr;

    case HARD:    
               adr = blk_ptr->hard_base + csrtbl[i].offset;
	       retval = inmeml(blk_ptr->pb,adr);
#if DEBUG
	       if (ci_debug_flag)
	       pprintf("HARD: %x = inmeml(%x,%x);\n",retval,blk_ptr->pb,adr);
#endif
               break;

    case SOFT:    
               /*
	       dump_cixcd_blk(blk_ptr);
	       if(blk_ptr->pb)dump_pb(blk_ptr->pb);
               */
               adr = blk_ptr->soft_base + csrtbl[i].offset;
	       retval = inmeml(blk_ptr->pb,adr);
#if DEBUG
	       if (ci_debug_flag)
	       pprintf("SOFT: %x = inmeml(%x,%x);\n",retval,blk_ptr->pb,adr);
#endif
               break;

    case MEM:
               adr = blk_ptr->mem_base + csrtbl[i].offset;
	       retval = inmeml(blk_ptr->pb,adr);
#if DEBUG
	       if (ci_debug_flag)
	       pprintf("MEM: %x = inmeml(%x,%x);\n",retval,blk_ptr->pb,adr);
#endif
               break;

    case CFG:
	       pprintf("CFG write: Not yet implemented\n");
               break;

    case EMUL:
#if DEBUG
	       pprintf("EMUL - %s no CIPCA parallel\n",csrtbl[i].csrnam);
#endif

#if ( TURBO || RAWHIDE ) && EMUL_DEBUG
	       retval = emul_read(csrtbl[i].offset);
#endif
	       break;

    default: 
	       pprintf("Not yet implemented\n");
               break;
  }
               
  return retval;

}



#if EMUL_DEBUG
/*
 * cixcd_read_csr
 */

int my_mbx_read_write_reg (int bus, char rw, U_INT_32 *reg, int size, 
     U_INT_8 *buf, int hose, struct MBX *mbx){

#if TURBO 
   mbx_read_write_reg (bus, rw, reg, size, buf, hose, mbx);
#else
  *reg = 0;
   return 0;
#endif

}

#if TURBO || RAWHIDE
int emul_read(int offset)
{
    struct MBX *cipca_mbx;
    u_long value;
    unsigned int base;  
    int hose,slot;   
    struct device *cixcd_dev[1];
    int cixcdcnt = 0;
    cixcdcnt = find_all_dev(&config,XMI$K_DTYPE_CIMNA,1,cixcd_dev,0);
    if (cixcdcnt==0) {
      pprintf("No CIXCD found\n");
      return;
    } else {
      hose = cixcd_dev[0]->hose;
      slot = cixcd_dev[0]->slot;
    }

    cipca_mbx = (struct MBX *) malloc_az(sizeof(struct MBX), 64, 0);
                                                     
    base = (XMI$A_NODE_SPACE + XMI$K_NODE_SPACE_SIZE * slot) & 0x6FFFFFFF;
    my_mbx_read_write_reg(MBX_XMI, 'r', base+offset, sizeof(U_INT_32), 
                         &value, hose, cipca_mbx);
#if 0
    printf("  CIXCD read:  \t base=%x offset = %x, data = %08x\n", 
                               base, offset, value);
#endif
    free(cipca_mbx);
    return (value);
}

/*
 * cixcd_write_csr
 */
int emul_write(int offset, u_long value)
{
    struct MBX *cipca_mbx;
    int base;
    int hose,slot;   
    struct device *cixcd_dev[1];
    int cixcdcnt = 0;
    cixcdcnt = find_all_dev(&config,XMI$K_DTYPE_CIMNA,1,cixcd_dev,0);
    if (cixcdcnt==0) {
      pprintf("No CIXCD found\n");
      return;
    } else {
      hose = cixcd_dev[0]->hose;
      slot = cixcd_dev[0]->slot;
    }

    cipca_mbx = (struct MBX *) malloc_az(sizeof(struct MBX), 64, 0);

    base = (XMI$A_NODE_SPACE + XMI$K_NODE_SPACE_SIZE * slot) & 0x6FFFFFFF;
#if 0
    printf("  CIXCD write: \t base=%x   offset = %x, data = %08x\n", 
                               base, offset, value);
#endif
    my_mbx_read_write_reg(MBX_XMI, 'w', base+offset, sizeof(U_INT_32), 
                        &value, hose, cipca_mbx);

    free(cipca_mbx);
    return 0;
}

#endif

/*EMUL_DEBUG*/
#endif   
