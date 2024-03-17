/*
 * @DEC_COPYRIGHT@
 */

/*
 * $Log: ev6_ic.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:22  paradis
 * Initial CVS checkin
 *
 * Revision 1.1  1997/11/12  18:56:16  pbell
 * Initial revision
 *
 * Revision 1.12  1997/07/09  05:48:08  fdh
 * Added Win32 support.
 *
 * Revision 1.11  1997/04/08  23:40:24  asher
 * Update for changes to create standlone srom image
 * creator.
 *
 * Revision 1.10  1997/04/08  19:58:51  asher
 * Add include of stdio.h.
 *
 * Revision 1.9  1996/12/13  16:59:41  asher
 * Allow loading a range of addresses into the icache.
 *
 * Revision 1.8  1996/07/17  21:17:08  asher
 * Move trouble_array inside ldic_config structure.
 *
 * Revision 1.7  1996/07/12  16:36:23  jlp
 * change interface ldic_stall()
 *
 * Revision 1.6  1996/06/26  18:07:22  asher
 * Add print_ldic_error callback to get ALL errors/warnings when loading
 * icache.
 *
 * Revision 1.5  1996/05/20  22:43:35  jlp
 * use <> instead of "" for j.h include
 *
 * Revision 1.4  1996/04/28  15:18:55  jlp
 * change icache tag arrangement to match the implementation
 *
 * Revision 1.3  1996/04/12  14:38:36  asher
 * Fix bug in loading correct icache set.
 *
 * Revision 1.2  1996/03/15  19:50:24  asher
 * Add open file error.
 *
 * Revision 1.1  1996/02/22  22:35:45  asher
 * Initial revision
 *
// Revision 1.8  1996/01/30  22:40:32  asher
// Now the portion of the icache loader used by verilog models.
//
 */

/*
 * $Id: ev6_ic.h,v 1.1.1.1 1998/12/29 21:36:22 paradis Exp $
 */

#ifndef __LOAD_ICACHE_H__
#define __LOAD_ICACHE_H__

#include <stdio.h>

/*
//
//   Sized integer data types depending on platform, OS, and compiler.  (For consistent references
//   across platforms, compilers, and operating systems.)
//
//             DECC, Alpha, VMS    DECC, Alpha, OSF/1  DECC, VAX           GNUC                   WIN32
//             ------------------  ------------------  ------------------  ------------------     ----------------------
//  MTV_uint8  unsigned char       unsigned char       unsigned char       unsigned char          unsigned char
//  MTV_int8            char                char                char                char                   char
//  MTV_uint16 unsigned short      unsigned short      unsigned short      unsigned short         unsigned short
//  MTV_int16           short               short               short               short                  short
//  MTV_uint32 unsigned int        unsigned int        unsigned int        unsigned int           unsigned int
//  MTV_int32           int                 int                 int                 int                    int
//  MTV_uint64 unsigned __int64    unsigned long int   MTV_uint32[2]       unsigned long long int unsigned __int64
//  MTV_int64           __int64             long int   MTV_int32[2]          signed long long int          __int64
*/

typedef unsigned char	MTV_uint8;
typedef char		MTV_int8;
typedef unsigned short	MTV_uint16;
typedef short		MTV_int16;
typedef unsigned int	MTV_uint32;
typedef int		MTV_int32;

#ifdef __alpha

#define HAS_QW_ARITH

#ifdef __osf__
typedef unsigned long int	MTV_uint64;
typedef long int		MTV_int64;
#endif

#if defined(__vms) || defined(_WIN32)
typedef unsigned __int64	MTV_uint64;
typedef __int64			MTV_int64;

#define NO_LANGUAGE_EXCEPTIONS
#endif

#else 

#define NO_LANGUAGE_EXCEPTIONS

#if defined(__GNUC_) || defined(_WIN32)

#define HAS_QW_ARITH

#ifdef __GNUC_
typedef unsigned long long int 	MTV_uint64;
typedef long long int		MTV_int64;
#endif

#ifdef _WIN32
typedef unsigned __int64	MTV_uint64;
typedef __int64			MTV_int64;
#endif /* _WIN32 */

#else
typedef MTV_uint32	MTV_uint64[2];
typedef MTV_int32	MTV_int64[2];
#endif /* __GNUC__ || _WIN32 */

#endif /* __alpha */


#ifdef HAS_QW_ARITH
#define BIGUINT		MTV_uint64
#define BIGINT		MTV_int64
#else
#define BIGUINT		MTV_uint32
#define BIGINT		MTV_int32
#endif /* HAS_QW_ARITH */

#include <j.h>

#define J_NUM_BANKS	8
#define J_NUM_INDEX	512

# define DST	1
# define NODST	0

/* These are defined in i.cnt, which we can't include.

   Symbol I_IC_SLOT_PREDEC_BOTH_L     =	0;
   Symbol I_IC_SLOT_PREDEC_IQ_L       =	1;
   Symbol I_IC_SLOT_PREDEC_IQ_U	      =	2;
   Symbol I_IC_SLOT_PREDEC_FQ_X	      =	4;
   Symbol I_IC_SLOT_PREDEC_NEITHER_X  =	5;
   Symbol I_IC_SLOT_PREDEC_IQ_X	      =	6;
*/   
# define BOTH_L		0
# define IQ_L		1
# define IQ_U		2
# define FQ_X		4
# define NEITHER_X	5
# define IQ_X		6

# define ldicSet0		1
# define ldicSet1		2
# define ldicOtherSetIfValid	4
# define ldicBit15		8
# define ldicNoWarn		16

# define ldicNoError		0
# define ldicErrCrossBlk	1
# define ldicWarnBlkValid	2
# define ldicOpenError		3

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
    
struct ldic_config {
    int error;
    MTV_uint64 pc;	/* pc attempting to load for any error. */
    int num_instr;	/* Number of inst attempted to load in ldicErrCrossBlk */
    MTV_uint64 vaddr[2];/* addresses for ldicWarnBlkValid. 
                           only vaddr[set] is valid if !ldicOtherSetIfValid. */
    int set_data;	/* set for ldicWarnBlkValid with !ldicOtherSetIfValid */

    MTV_uint32 filler_lw;
    int asm_bit, asn_value, kseu_value, pal_bit, debug;
    int set;
    int call_pal_r23;	/* Must be set to one if call pal linkage register is r23 */

    int enable_range;
    MTV_uint64 start_addr, end_addr;

    MTV_uint32 trouble_array[512]; /* storage array used in ldic_write_lw/line */
    
    MTV_uint32* data_p;		/* j_dat_gdp->data_array[8ic_bank][512ic_idx](J_BITS_PER_DATA_FB - 1, 0) */
    
    MTV_uint32* lp_data_p;	/* j_dat_gdp->lp_data_array[ic_bank][ic_idx](J_LP_BITS_PER_FB - 1, 0) */
    MTV_uint32* lp_ssp_p;	/* j_dat_gdp->lp_ssp_array[ic_bank][ic_idx](4, 0) */
    
    MTV_uint32* s0_tag_lay_p;	/* j_tag_tay->s0_n1bc_h */
    MTV_uint32* s1_tag_lay_p;	/* j_tag_tay->s1_n1bc_h */
    MTV_uint32* s0_tag_ray_p;	/* j_tag_ray->s0_n1bc_h */
    MTV_uint32* s1_tag_ray_p;	/* j_tag_ray->s1_n1bc_h */

    void (*print_error_p)(struct ldic_config*);
    void (*print_srom_data_p)(struct ldic_config*, MTV_uint64, int, MTV_uint32, MTV_uint64, MTV_uint64, MTV_uint64, MTV_uint64);
};

/* Main entry point.
 */
int ldic_load_exe(struct ldic_config* cnfg_p, MTV_uint64 addr, const char* fname_p);

int ldic_load_file(struct ldic_config* cnfg_p, MTV_uint64 addr, FILE *file_p, int numbytes);
MTV_uint32 ldic_swap(struct ldic_config* cnfg_p, MTV_uint32 inst, MTV_uint64 pc);

MTV_uint32 ldic_dstv_predecode(MTV_uint32 inst);
MTV_uint32 ldic_ic_cout(MTV_uint32 inst, MTV_uint64 pc);
MTV_uint32 ldic_easrc(MTV_uint32 inst,
                      MTV_uint32 next_easrc,
                      int trouble_bit);
MTV_uint32 ldic_stall(int inst_num, MTV_uint32 inst, MTV_uint32 next_stall, MTV_uint32* num_early_exit_paths_p, MTV_uint32 trbl);
MTV_uint32 ldic_trouble(MTV_uint32 inst);
MTV_uint32 ldic_slot_predecode(MTV_uint32 inst, MTV_uint32 dstv);

void ldic_write_lw(struct ldic_config* cnfg_p,
                   MTV_uint64 base_pc,
                   MTV_uint32 ic_idx,
                   MTV_uint32 ic_bank,
                   int ind, MTV_uint32 inst,
                   MTV_uint32* next_easrc_p, MTV_uint32* next_stall_p, MTV_uint32 *next_misc_or_ldx_l,
                   MTV_uint32* num_early_exit_paths_p);
void ldic_write_line(struct ldic_config* cnfg_p, MTV_uint64 pc, int num_instr, const MTV_uint32* inst);
void ldic_output_srom_data(struct ldic_config* cnfg_p, int set, MTV_uint64 addr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __LOAD_ICACHE_H__ */

