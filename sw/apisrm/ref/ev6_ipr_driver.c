/* file:	ev6_ipr_driver.c
 *                 
 * Copyright (C) 1996 by
 * Digital Equipment Corporation, Maynard, Massachusetts.
 * All rights reserved.
 *                 
 * This software is furnished under a license and may be used and copied
 * only  in  accordance  of  the  terms  of  such  license  and with the
 * inclusion of the above copyright notice. This software or  any  other
 * copies thereof may not be provided or otherwise made available to any
 * other person.  No title to and  ownership of the  software is  hereby
 * tansferred.     
 *                 
 * The information in this software is  subject to change without notice
 * and  should  not  be  construed  as a commitment by digital equipment
 * corporation.    
 *                 
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by digital.
 *                 
 *                 
 * Abstract:	Internal Processor Register driver for examine/deposit.
 *                 
 * Author:	Eric Goehl                                                
 *                
 * Modifications: 
 *      
 *      egg	30-Jul-1996 	Initial port from ev5_ipr_driver.c
 */    
      
#include    "cp$src:platform.h"		/* build option definitions. 	*/
                                                    
#include    "cp$src:kernel_def.h"   	/* struct DDB, FILE, etc.	*/
#include    "cp$src:common.h" 		/* SEEK_SET 			*/
#include    "cp$inc:prototypes.h"
#include    "cp$src:alpha_def.h"	/* struct REGDATA 		*/
#include    "cp$src:alphaps_def.h"	/* struct PS 			*/
#include    "cp$src:aprdef.h"		/* APR$K_xxxx constants 	*/      
#include    "cp$src:msgnum.h"		/* msg_exdep_badaddr 		*/
#include    "cp$src:filesys.h"		/* fopen() 			*/
#include    "cp$src:get_console_base.h"

/* PLATFORM SPECIFIC OVERRIDES. */
#include    "cp$src:pal_def.h"		
#include    "cp$src:impure_def.h"	/* struct TPAL_P_MISC_REGS  	*/  
#include    "cp$src:platform_cpu.h"	/* HWRPB$_PAGEBITS 		*/
                                   
#define DEBUG 		0
#define msg_success 	0         
                           
#include    "cp$src:assert.h"	/* DEBUG should be defined before this file.*/
                           
  /*===============================*/
  /* External function prototypes. */         
  /* (should be in header files)   */
  /*===============================*/         
int unicheck(char*);	       	/* kernel.c ; should be in kernel.h */
int null_procedure ();
                                          
  /*============================*/
  /* Local function prototypes. */
  /*============================*/
int ipr_read ();
int ipr_write ();
int ipr_open ();
int ipr_close ();

struct DDB ipr_ddb = {
	"ipr",			/* how this routine wants to be called	*/
	ipr_read,		/* read routine				*/
	ipr_write,		/* write routine			*/
	ipr_open,		/* open routine				*/
	null_procedure,		/* close routine			*/
	null_procedure,		/* name expansion routine		*/
	null_procedure,		/* delete routine			*/
	null_procedure,		/* create routine			*/
	null_procedure,		/* setmode				*/
	null_procedure,		/* validation routine			*/
	0,			/* class specific use			*/
	1,			/* allows information			*/
	0,			/* must be stacked			*/
	0,			/* is a flash update driver		*/
	0,			/* is a block device			*/
	0,			/* not seekable				*/
	0,			/* is an ethernet device		*/
	0,			/* is a filesystem driver		*/
};



/*+
 * ============================================================================
 * = ipr_open - open the ipr driver                                       =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	    This routine opens ipr by establishing an exception handler for
 *	the process.  Since iprs are processor specific, and some of them
 *	reference real hardware, this driver is very sensitive to procesor
 *	affinity.  With an affinity set to more than one cpu, two successive
 *	reads are not likely to reference the same cpu, yet they will both
 *	merrily return data.
 *
 *	To catch this insidious situation without a lot of synchronization
 *	overhead, this routine complains if the affinity and active set
 *	indicates there are more than one cpu to run on.
 *
 *
 *
 *  FORMAL PARAMETERS:
 *
 *      struct FILE *fp	    - Ignored.
 *	char *info	    - ipr number to reference
 *	char *next	    - Ignored.
 *	char *mode	    - Ignored.
 *
 *  IMPLICIT INPUTS:
 *
 *      NONE.
 *
 *  IMPLICIT OUTPUTS:
 *
 *      NONE.
 *
 *  FUNCTION VALUE:
 *
 *      msg_success
 *
 *  SIDE EFFECTS:
 *
 *
 *--
 */
int ipr_open (struct FILE *fp, char *info, char *next, char *mode) {

	/*
	 * Complain if we're eligible to run on more than one
	 * processor.
	 */
	unicheck ("ipr_driver");

	/*
	 * Use the explicit offset if present.  We complain about
	 * bad offsets until read/write time.  Treat the offset
	 * as a register number.  A null info field results in 
	 * setting the offset to 0.
	 */               
	fp->offset = fp->local_offset;
	*fp->offset = xtoi (info);
                          
	return msg_success;
}                         
                         
/*+                       
 * ============================================================================
 * = ipr_read - read from the ipr driver                                      =
 * ============================================================================
 *                        
 * OVERVIEW:              
 *                        
 *	This routine reads an IPR.  The address is stored in the FILE
 * structure, and the data is written to the specified buffer.  If the IPR
 * is stored in the saved entry context, then the value is read from there.
 * Otherwise, the value is read directly from the IPR.
 *
 * If a reserved operand exception occurs in an attempt to read directly
 * from an IPR, then the handler set up by the ipr_open routine will
 * longjmp back to ipr_read, which then loads an error status into the
 * file structure and returns 0 to indicate the error.
 *
 *
 *  FORMAL PARAMETERS:
 *                   
 *      struct FILE *fp	- Pointer to FILE structure.  This should
 *			correspond to the IPR device.
 *	int size	- size of items to read
 *	int len		- number of items to read (ignored).
 *	char *buf	Address to write the data to.
 *
 *  RETURN VALUE:  
 *          
 *	Number of bytes read.
 *
 *  IMPLICIT INPUTS:
 *
 *      NONE
 *                     
 *  IMPLICIT OUTPUTS:  
 *                     
 *      NONE                                                            
 *                                                                      
 *  SIDE EFFECTS:                                                       
 *                                                                      
 *      NONE                                                            
 *                                                                      
 *--                                                                    
 */                                                                     
int ipr_read (struct FILE *fp, int size, int len, char *buf) {              
    int ipr_num;                                                        
    int stat;                                                           
    struct impure *impurePtr;                                           
    int xfer_length;                                                    
    __int64 data;      	/* aligned quadword data */                     
    union  {                      
      struct TPAL_P_MISC_REG fields;
      __int64 whole;              
    } p_misc;                     
    union {                       
      struct TPAL_PCTX_REG fields; 
      __int64 whole;              
    } pctx;                       
                       
    /* get the ipr number and advance */
    ipr_num = *fp->offset;
    *fp->offset += 1;  
    fp->status = msg_success;
                       
                       
	/*                                                           
	 * Set up some generic pointers and information
 	 */            
    (int)impurePtr = PAL$IMPURE_BASE + PAL$IMPURE_COMMON_SIZE +
    	       	       	whoami() * PAL$IMPURE_SPECIFIC_SIZE;
    p_misc.whole =  *(__int64 *)&impurePtr->cns$p_misc;          
    pctx.whole = *(__int64 *)&impurePtr->cns$process_context;
    	/*                                                       
    	 * dispatch on the ipr number                            
    	 */                                                      
    switch (ipr_num) {                                           
     	case APR$K_ASN:	     	  	                         
            data = pctx.fields.asn;
 	    /* -- Clean, but generates inefficient code.
              data = impurePtr->cns$process_context.asn;         
            */
	    break;                                               
                                                                 
    	case APR$K_ASTEN:	      	                         
    	    data = pctx.fields.aster;
    	    break;                                               
                                                                        
    	case APR$K_ASTSR:{	     	                         
            data = pctx.fields.astrr;
    	    break;
          }                                     
                                                                 
        case APR$K_FEN:			                         
    	    data = pctx.fields.fpe;
    	    break;                                               
                                                                 
	case APR$K_IPL:
	    data = p_misc.fields.ipl;
	    break;
               
	case APR$K_MCES: 		
	    data = p_misc.fields.mces.whole;
	    break;              
                                
	case APR$K_PCBB:        
    	    data = *(__int64 *)&impurePtr->cns$pcbb;
    	    break;              
                                
	case APR$K_PRBR:        
	    data = *(__int64 *)&impurePtr->cns$prbr;
	    break;              
                                
	case APR$K_PTBR:        
	    data = *(__int64 *)&impurePtr->cns$ptbr;
            data >>= HWRPB$_PAGEBITS;
	    break;             
                               
	case APR$K_SCBB:       
	    data = *(__int64 *)&impurePtr->cns$scbb;
	    data >>= HWRPB$_PAGEBITS;
	    break;

	case APR$K_SISR:
	    data = ((*(__int64 *)&impurePtr->cns$sirr) >> 13) & 0xfffe;
	    break;

	/*  
	 * when reading stack pointers, we have to look at the
	 * current mode.  If the mode implied by the stack pointer
	 * and the saved mode are the same, the PCBB is not valid
	 * and the saved sp must be used.  Otherwise, we use the
	 * sp in the PCBB.
	 */
	case APR$K_KSP:
	case APR$K_ESP:
	case APR$K_SSP:
	case APR$K_USP:
	    if (p_misc.fields.cm == (ipr_num - APR$K_KSP)) {
	       	read_gpr (30, &data);
	    } else {
                __int64 src;       
   	        src = *(__int64 *)&impurePtr->cns$pcbb;
	       	src += (ipr_num - APR$K_KSP) * sizeof (REGDATA);
	       	ldqp (&src, &data);
	    }  
	    break;                  
               
	case APR$K_VPTB: 
	    data = *(__int64 *)&impurePtr->cns$vptb;
	    break; 	    
               
	case APR$K_WHAMI:
	    data = *(__int64 *)&impurePtr->cns$whami;
	    break;
               
	case APR$K_PS:
	    data = p_misc.whole & 0xFFFF;
	    break;                                      
                                                        
    	default:                                             
    	    fp->status = (int)msg_exdep_badadr;                               
    	    return 0;                                                    
    	    break;                                                       
    } /* End of switch */                                                
                                                                         
    	/*                                                               
    	 * block copy the data to the user's buffer without              
	 * exceeding it.                                                 
	 */                                                              
	xfer_length = umin(sizeof (REGDATA), size * len);                
                                                                         
      assert(xfer_length <= sizeof(data));                                
                                                                         
     	memcpy (buf, &data, xfer_length);                                
                                                                         
	return xfer_length;                                              
}

/*+
 * ============================================================================
 * = ipr_write - write an ipr                                                 =
 * ============================================================================
 *                        
 * OVERVIEW:
 *
 *	This routine writes an IPR.  The address is stored in the FILE
 * structure, and the data is read from the specified buffer.  If the IPR
 * is stored in the saved entry context, then the value is written there.
 * Otherwise, the value is written directly to the IPR.
 *
 * If a reserved operand exception occurs in an attempt to write directly
 * to an IPR, then the handler set up by the ipr_open routine will longjmp
 * back to ipr_write, which then loads an error status into the file structure
 * and returns 0 to indicate the error.
 *
 *
 *  FORMAL PARAMETERS:
 *
 *      struct FILE *fp -Pointer to FILE structure.  This should correspond
 *			 to the IPR device.
 *	int size	- size of items to write
 *	int len		- number of items to write (ignored).
 *	char *buf	Address to read the data from.
 *
 *  RETURN VALUE:
 *
 *	Number of bytes written, or 0 on error.
 *
 *  IMPLICIT INPUTS:
 *
 *      NONE
 *
 *  IMPLICIT OUTPUTS:
 *         
 *      NONE
 *         
 *  SIDE EFFECTS:
 *         
 *      NONE
 *         
 *--       
 */        
int ipr_write (struct FILE *fp, int size, int len, char *buf) {
    int ipr_num;
    int stat;
    struct impure *impurePtr;                 
    int xfer_length;
    __int64 data;
    __int64 temp;
    union  {
      struct TPAL_P_MISC_REG fields;
      __int64 whole;
    } p_misc;                   
    union  {
      struct TPAL_PCTX_REG fields;
      __int64 whole;  
    } pctx;                   
                      
    /*                
     * fetch the ipr number and advance
     */               
    ipr_num = *fp->offset;
    *fp->offset += 1; 
    fp->status = msg_success;
                      
    /*                
     * Copy the data into an aligned local buffer and
     * zero extend it.
     */               
    memset (&data, 0, sizeof (data));
    xfer_length = umin (sizeof (REGDATA), size * len);
    memcpy (&data, buf, xfer_length);
                      
                      
    	/*            
	 * Set up some generic pointers
 	 */           
    (int)impurePtr = PAL$IMPURE_BASE + PAL$IMPURE_COMMON_SIZE +
                 whoami () * PAL$IMPURE_SPECIFIC_SIZE;
                
    p_misc.whole =  *(__int64 *)&impurePtr->cns$p_misc;          
    pctx.whole =  *(__int64 *)&impurePtr->cns$process_context;          
                                                  
	/*      
    	 * dispatch on the ipr number
	 */     
    switch (ipr_num) {
                                                
	case APR$K_ASTEN:   
            pctx.fields.aster = data;
            *(__int64 *)&impurePtr->cns$process_context = pctx.whole;
            break;
      
	case APR$K_ASTSR:
            pctx.fields.astrr = data;
            *(__int64 *)&impurePtr->cns$process_context = pctx.whole;
            break;                             
                                               
	case APR$K_FEN:                        
            pctx.fields.fpe = data;
            *(__int64 *)&impurePtr->cns$process_context = pctx.whole;
            break;                             
                                               
	case APR$K_IPIR:                           
	    mtpr64 (&data, APR$K_IPIR);            
	    break;  

	case APR$K_IPL: 
	    p_misc.fields.ipl = data;
            *(__int64 *)&impurePtr->cns$p_misc = p_misc.whole;
            break;                                       
                        
	case APR$K_MCES:
            p_misc.fields.mces.whole &= ~(data & 7 | 0x18);
            p_misc.fields.mces.whole |= (data & 0x18);
            *(__int64 *)&impurePtr->cns$p_misc = p_misc.whole;
	    break;

	case APR$K_PCBB:
            *(__int64 *)&impurePtr->cns$pcbb = data;
	    break;

	case APR$K_PRBR:
            *(__int64 *)&impurePtr->cns$prbr = data;
	    break;
	  
	case APR$K_PTBR :
	    data <<= HWRPB$_PAGEBITS;
            *(__int64 *)&impurePtr->cns$ptbr = data;
	    break;

	case APR$K_SCBB: 
	    data <<= HWRPB$_PAGEBITS;
            *(__int64 *)&impurePtr->cns$scbb = data;
	    break;            

	case APR$K_SIRR:
	    data &= 0xf;
	    if (data == 0)
		break;
	    temp = *(__int64 *)&impurePtr->cns$sirr;
	    temp |= (1 << (data - 1)) << 14;
	    *(__int64 *)&impurePtr->cns$sirr = temp;
	    break;

	case APR$K_TBIA:
	    mtpr64 (&data, APR$K_TBIA);
	    break;

	case APR$K_TBIAP:
	    mtpr64 (&data, APR$K_TBIAP);
	    break;

	case APR$K_TBIS:
	    mtpr64 (&data, APR$K_TBIS);
	    break;

	/*
	 * when writing stack pointers, we have to look at the
	 * current mode.  If the mode implied by the stack pointer
	 * and the saved mode are the same, the PCBB is not valid
	 * and the saved sp must be used.  Otherwise, we use the
	 * sp in the PCBB.                                   
	 */                                           
	case APR$K_KSP:
	case APR$K_ESP:
	case APR$K_SSP:
	case APR$K_USP:
	    if (p_misc.fields.cm == (ipr_num - APR$K_KSP)) {
		write_gpr (30, &data);
	    } else {
                __int64 src;
  	        src = *(__int64 *)&impurePtr->cns$pcbb;
		src += (ipr_num - APR$K_KSP) * sizeof (REGDATA);
		stqp (&src, &data);
	    }
	    break;

	case APR$K_VPTB:
	    *(__int64 *)&impurePtr->cns$vptb = data;
	    break;

	case APR$K_PS:
	    p_misc.whole &= ~0xFFFF;                      
            p_misc.whole |= data & 0xFFFF;
	    *(__int64 *)&impurePtr->cns$p_misc = p_misc.whole;
	    /* the IPL table is at offset 0x0D00 for all platforms */
	    temp = *(__int64 *)&impurePtr->cns$pal_base;
	    temp = phys_to_virt (temp);
	    temp += 0x0D00;
	    if (data == 0x1f00) {
		temp += 0x1f * 8;
		*(__int64 *)&impurePtr->cns$ier_cm = *(__int64 *)temp;
	    }
	    if (data == 0x7) {
		temp += 0x7 * 8;
		*(__int64 *)&impurePtr->cns$ier_cm = *(__int64 *)temp;
	    }
            break;

	case IBX$K_PAL_BASE:
	    *(__int64 *)&impurePtr->cns$pal_base = data;
	    break;                                        

	case APR$K_ICSR:
            goto APR_NOT_IMPLEMENTED;
                                                   
	case APR$K_MCSR:                    
            goto APR_NOT_IMPLEMENTED;

 	  /* Calls to unimplemented IPRs get here. */
        APR_NOT_IMPLEMENTED:                                        
          err_printf("APR register %0x write not implemented.\n", ipr_num);
	default:
	    fp->status = (int)msg_exdep_badadr;
	    return 0;
	    break;
    } /* end of switch */
    
    return sizeof(REGDATA);
}   
   
/*+     
 * ============================================================================
 * = ev6_ipr_init - init the ipr driver                                           =
 * ============================================================================
 *  
 * OVERVIEW:
 *
 *	This routine initializes the IPR driver entry in the global DDB
 * (Driver DataBase) and creates an associated INODE so that the file
 * system recognizes the IPR device (Internal Processor Registers) for
 * examines and deposits.
 *
 *  FORMAL PARAMETERS:
 *
 *      NONE
 *
 *  RETURN VALUE:
 *
 *	Always returns msg_success.
 *
 *  IMPLICIT INPUTS:
 *
 *      NONE
 *
 *  IMPLICIT OUTPUTS:
 *
 *      NONE
 *
 *  SIDE EFFECTS:
 *
 *      Modifies the Driver DataBase and creates an INODE for the IPR device.
 *
 *--
 */
int ev6_ipr_init (void) {
    struct INODE *ip;			/* Pointer to INODE for this driver */


/* Create an inode entry, thus making the device visible as a file: */

    allocinode (ipr_ddb.name, 1, &ip);
    ip->dva = & ipr_ddb;			/* Device is IPR space */
    ip->attr = ATTR$M_READ | ATTR$M_WRITE;	/* Read and writes allowed */
    ip->loc = 0;
    ip->bs = sizeof (REGDATA);
    INODE_UNLOCK (ip);

    return msg_success;
}

/*+
 * ============================================================================
 * = set_spe - enable/disable superpage mode in EV6                           =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine selectively enables or disables EV6 Superpage mode
 *	for the operating system.  Changes are made to the appropriate
 *	EV6 IPR bits in the IMPURE area.
 *
 *  FORMAL PARAMETERS:
 *
 *      int	t	- data written to SPE<2:0>
 *
 *  RETURN VALUE:
 *
 *	Always returns msg_success.
 *
 *  IMPLICIT INPUTS:
 *
 *      NONE
 *
 *  IMPLICIT OUTPUTS:
 *
 *      NONE
 *
 *  SIDE EFFECTS:
 *
 *	The next time the processor leaves console mode, the new value
 *	of SPE will take effect.
 *
 *--
 */
#define I_CTL$SPE 3
#define M_CTL$SPE 1

void set_spe(int spe) {
    struct impure *impurePtr;
    (int)impurePtr = PAL$IMPURE_BASE + PAL$IMPURE_COMMON_SIZE +
    	       	       	whoami() * PAL$IMPURE_SPECIFIC_SIZE;

    impurePtr->cns$i_ctl[0] &= ~(7<<I_CTL$SPE);
    impurePtr->cns$i_ctl[0] |= (spe<<I_CTL$SPE);

    impurePtr->cns$m_ctl[0] &= ~(7<<M_CTL$SPE);
    impurePtr->cns$m_ctl[0] |= (spe<<M_CTL$SPE);
}

/*+
 * ============================================================================
 * = set_va_48 - enable/disable 48-bit addressing mode in EV6                 =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine selectively enables or disables EV6 48-bit addressing
 *	mode for the operating system.  Changes are made to the appropriate
 *	EV6 IPR bits in the IMPURE area.
 *
 *  FORMAL PARAMETERS:
 *
 *      int	t	- 1 Enable VA_48
 *			- 0 Disable VA_48
 *
 *  RETURN VALUE:
 *
 *	None.
 *
 *  IMPLICIT INPUTS:
 *
 *      NONE
 *
 *  IMPLICIT OUTPUTS:
 *
 *      NONE
 *
 *  SIDE EFFECTS:
 *
 *	The next time the processor leaves console mode, the new value
 *	of VA_48 will take effect.
 *
 *--
 */
#define I_CTL$VA_48 15
#define VA_CTL$VA_48 1

void set_va_48(int va_48) {
    struct impure *impurePtr;
    (int)impurePtr = PAL$IMPURE_BASE + PAL$IMPURE_COMMON_SIZE +
    	       	       	whoami() * PAL$IMPURE_SPECIFIC_SIZE;

    impurePtr->cns$i_ctl[0] &= ~(1<<I_CTL$VA_48);
    impurePtr->cns$i_ctl[0] |= (va_48<<I_CTL$VA_48);

    impurePtr->cns$va_ctl[0] &= ~(1<<VA_CTL$VA_48);
    impurePtr->cns$va_ctl[0] |= (va_48<<VA_CTL$VA_48);
}
