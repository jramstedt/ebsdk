/***********************************************************************
//!
//! Copyright (c) Digital Equipment Corporation, 1994,1995
//!
//!   All Rights Reserved.  Unpublished rights  reserved  under
//!   the copyright laws of the United States.
//!
//!   The software contained on this media  is  proprietary  to
//!   and  embodies  the  confidential  technology  of  Digital
//!   Equipment Corporation.  Possession, use,  duplication  or
//!   dissemination of the software and media is authorized only
//!   pursuant to a valid written license from Digital Equipment
//!   Corporation.
//!
//!NAME OF FILE
//!   verif_utils.cxx
//!
//!
//!DESCRIPTION
//!   This module includes all the utility routines and other support
//!   routines used for verification tools.
//!
//!
//!
//!REVISION HISTORY:
//!
//!
// $Log: verif_utils.cxx,v $
// Revision 1.1.1.1  1998/12/29 21:36:22  paradis
// Initial CVS checkin
//
// Revision 1.1  1997/11/12  18:56:16  pbell
// Initial revision
//
// Revision 1.109  1997/06/05  19:37:18  quinn
// fake_srom can now be driven from an srom binary image file
//
// Revision 1.108  1997/03/20  22:55:25  asher
// Changes to allow parallel chango to work.
//
// Revision 1.107  1997/02/28  18:05:57  quinn
// added rand_percent() function (callable from CLI)
//
// Revision 1.106  1997/02/22  00:04:48  quinn
// changed icache FAKE_SROM load for latest bit positions
//
// Revision 1.105  1997/02/18  22:11:31  quinn
// changes to support the Tbox adding 16 to it's Cbox data shift count
//
// Revision 1.104  1997/02/12  20:18:01  dohm
// Fixed out_of_order count component
// in mbox_trap_count
//
// Revision 1.103  1997/02/06  19:02:56  huggins
// update for new pass numbers
//
// Revision 1.102  1997/02/05  22:04:21  hagan
// Added new CBox CSRs to stats output.
//
// Revision 1.101  1997/02/03  15:24:03  quinn
// new Cbox pads
//
// Revision 1.100  1997/01/28  01:01:19  razdan
// added mbox_bc_prb_stall_a to write_once chain
//
// Revision 1.99  1997/01/27  18:15:43  dohm
// Fixed bugs in ldq_inorder_ldx(); fixed maf stats count.
//
// Revision 1.98  1997/01/21  23:31:45  dohm
// Added stats for maf size
//
// Revision 1.97  1997/01/16  23:38:18  dohm
// Added pa_msb__xx statistic
//
// Revision 1.96  1997/01/14  23:07:01  dohm
// Added stats for internal_nxm_count
//
// Revision 1.95  1997/01/13  20:35:15  quinn
// added c_mtv_cfg->write_many_a for regression tests which
// need to init the WRITE_MANY chain before reset
//
// Revision 1.94  1997/01/08  20:15:52  dohm
// Fixed bug in deadlock_stats() which caused LDS and LDT preloads
// not to be noticed; also added stats for LDQ and STQ average sizes.
//
// Revision 1.93  1996/12/31  01:56:13  hagan
// moved m_mrt and i_iq includes to full chip only section
//
// Revision 1.92  1996/12/20  13:56:32  razdan
// match csr chain update in c_csr.mdl
//
// Revision 1.91  1996/12/19  20:40:19  dohm
// Finished functionality for probes in deadlock_stats()
//
// Revision 1.90  1996/12/18  23:53:02  dohm
// Added function deadlock_stats()
//
// Print out enhanced ibox issue stats (Carl)
//
// Revision 1.89  1996/12/13  15:55:24  quinn
// additional test hooks for the PLL-Reset logic
//
// Revision 1.87  1996/12/06  23:24:35  quinn
// minor changes for VMS compile
//
// Revision 1.86  1996/12/06  22:29:56  dohm
// Added counters for LDQ/STQ address matches
//
// Revision 1.85  1996/11/28  03:38:56  hagan
// repaired and enabled STIO data timer
//
// Revision 1.84  1996/11/26  00:01:33  dohm
// Added stats for loads and stores
//
// Revision 1.83  1996/11/25  13:49:19  quinn
// more Cbox csr changes
//
// Revision 1.82  1996/11/20  19:06:51  dohm
// Changed how mbox_count_traps counts
//
// Revision 1.81  1996/11/20  16:45:28  asher
// Add version of mpcmd which takes the number of cpus to use.
//
// Revision 1.80  1996/11/20  16:14:23  hagan
// Temp disabled mbox stIO data delivery checker
//
// Revision 1.79  1996/11/19  00:34:05  dohm
// Count mbox traps and retries per cycle in the signal
// verif_utils->mbox_trap_count.
//
// Revision 1.78  1996/11/13  13:50:50  quinn
// fixed VMS compile bug
//
// Revision 1.77  1996/11/13  00:03:27  hagan
// Added MBOX store data timer.
//
// Revision 1.76  1996/11/07  16:50:20  asher
// CXBOX uses real reset now.  set_cbox_config must not clear out CSRs.
//
// Revision 1.75  1996/11/06  20:58:37  quinn
// changes for expanded/re-arranged SROM icache fill line
//
// Revision 1.74  1996/11/06  15:16:18  quinn
// Changes to support Cbox csr changes and new PLL/reset state machine
//
// Revision 1.73  1996/10/30  20:58:45  asher
// Add Nathan's new LDxL/STxC checker.
// Add stats entry for FP instruction count.
//
// Revision 1.72  1996/10/09  20:51:49  quinn
// added full set of PAL trap counters for STATS
//
// Revision 1.71  1996/10/03  17:04:28  foley
// Moved sysaddoutclk and sysaddinclk to x_utils.mdl
//
// Revision 1.70  1996/10/03  12:19:27  quinn
// fixed VMS build bug
//
// Revision 1.69  1996/10/02  18:49:39  foley
// Added sysaddoutclk_edge and sysaddinclk_edge for sys clock edge detection.
//
// Revision 1.68  1996/09/26  19:09:30  quinn
// added maf_full_retry to stats list
//
// Revision 1.67  1996/09/23  18:54:15  asher
// Add data_valid_dly, jitter_cmd, bc_wr_rd_bubbles, and bc_cpu_clk_delay
// to the ipr write once chain.
//
// Revision 1.66  1996/09/13  14:05:12  quinn
// fixed gen_stats for bclk_ratios 5,6,7
//
// Revision 1.65  1996/09/11  11:45:48  quinn
// added additional ic address checkers for Fake Srom fill
//
// Revision 1.64  1996/09/09  22:43:42  quinn
// changed auto_save to save on A phase (rather than B)
//
// Revision 1.63  1996/09/05  19:51:22  quinn
// fixed the fake_srom()
//
// Revision 1.62  1996/09/05  12:46:11  quinn
// added fake_srom support
//
// Revision 1.60  1996/08/16  14:56:40  huggins
// quick fix to c_reg_csr clearer on multi-cpu model
//
// Revision 1.59  1996/08/13  20:40:26  huggins
// added new mpcmd function
//
// Revision 1.58  1996/08/13  15:04:03  asher
// Move include of i_ipr__cnt.hxx to not be done for CX models.
//
// Revision 1.57  1996/08/07  14:33:02  asher
// Fix cpu number calculation.
//
// Revision 1.56  1996/08/06  19:00:18  asher
// Deposit to i<nn>->c_reg_csr->* for MP_MODELs.
//
// Revision 1.55  1996/07/31  19:26:50  tal
// modified STQ-SDC assertion checker
//
// Revision 1.54  1996/07/29  20:54:48  bhaiwala
// "added single_issue"
//
// Revision 1.53  1996/07/26  13:29:36  jlp
// add warning_count and error_count
//
// Revision 1.52  1996/07/15  17:20:58  huggins
// changed way step until command works.
//
// Revision 1.51  1996/07/02  17:37:08  mikulis
// Fix CX build
//
// Revision 1.50  1996/07/02  13:08:37  quinn
// yet another VMS compile fix
//
// Revision 1.49  1996/07/02  12:45:30  huggins
// VMS fixit
//
// Revision 1.48  1996/07/02  12:40:18  huggins
// changed set_cbox_config() function
//
// Revision 1.47  1996/07/01  18:20:16  tal
// added enable signal condition to ldq checker
//
// Revision 1.46  1996/07/01  18:05:38  tal
// new load queue in order by inum assertion checker
//
// Revision 1.45  1996/06/21  20:46:22  huggins
// added set_cbox_config function
//
// Revision 1.44  1996/06/13  00:19:14  razdan
// rename bunch of csrs
//
// Revision 1.43  1996/06/06  20:56:21  leibholz
// add some more counts and
// hack the tcl stuff to get more trap/stall/bubble info
//
// Revision 1.42  1996/05/30  18:46:34  mikulis
// Add 8x bcache clock to gen_stats
//
// Revision 1.41  1996/05/23  19:30:24  mikulis
// Add ability to capture cycle number of particular probe latency
//
// Revision 1.40  1996/05/22  20:02:22  asher
// Major change to move global objects inside EV6CLI_ObjectHandler or the
// Xbox.  Also made single CPU model allocate the same way as the multi CPU
// models.
//
// Revision 1.39  1996/05/21  15:38:16  huggins
// fixed ibox stats
//
// Revision 1.38  1996/05/16  20:37:27  mikulis
// Add stuff to track probe latencies
//
// Revision 1.37  1996/05/03  19:59:42  mikulis
// Compile out Dan's stuff for the CX model
//
// Revision 1.36  1996/05/02  20:54:38  leibholz
// add performance count artifact logic
//
// Revision 1.35  1996/04/30  17:24:08  quinn
// added missing parentheses to previous change
//
// Revision 1.34  1996/04/30  15:37:42  asher
// Change bit subscript inserts to use explicit .insert member function in
// rand_value().
//
// Revision 1.33  1996/04/25  18:07:25  mikulis
// Add nxm stats
//
// Revision 1.32  1996/04/23  20:08:31  kessler
// changed not __VMS to __osf__
// fixed load_osf_aout alignment bug
//
// Revision 1.31  1996/04/22  13:35:17  mikulis
// Change speculative_rd_enable token generation
//
// Revision 1.30  1996/04/19  12:04:17  kessler
// print filename when loading a.out
//
// Revision 1.29  1996/04/12  14:59:12  mikulis
// Fix CX build problem
//
// Revision 1.28  1996/04/12  12:17:17  kessler
// took out stuff that wouldn't compile on VMS with ifdefs
//
// Revision 1.27  1996/04/11  20:18:47  kessler
// code to load in coff a.out files
//
// Revision 1.26  1996/03/28  19:01:20  mikulis
// Add multi-instantiation stuff
//
// Revision 1.25  1996/03/13  19:02:03  huggins
// added set seed function
//
// Revision 1.24  1996/03/12  20:31:58  huggins
// add rand_value function
// made su cmd smarter
//
// Revision 1.23  1996/03/08  19:00:56  quinn
// fixed VMS build bug
//
// Revision 1.22  1996/03/07  18:42:47  tal
// modified STQ-SDC checker
//
// Revision 1.21  1996/03/01  14:13:27  mikulis
// Add spec read stats
//
// Revision 1.20  1996/02/29  14:20:49  mikulis
// Add CBOX_DEBUG to mbox_checkers
//
// Revision 1.19  1996/02/28  22:48:16  quinn
// added mbox_checkers()
//
// Revision 1.18  1996/02/28  20:55:19  tal
// STQ-SDC assertion checker
//
// Revision 1.17  1996/02/24  00:37:51  razdan
// rename signal
//
// Revision 1.16  1996/02/19  19:32:37  mikulis
// Update zeroblk_enable and set_dirty_enable token generation
//
// Revision 1.15  1995/12/20  18:55:52  mikulis
// Fix Cbox stats output
//
// Revision 1.14  1995/12/05  13:02:36  mikulis
// Add new Cbox stats
//
// Revision 1.13  1995/11/08  16:21:40  asher
// Fix typo "<<" should have been "<".
//
// Revision 1.12  1995/10/25  20:36:16  asher
// Make autosave occur after Xbox calls spot.
//
// Revision 1.11  1995/10/19  13:09:16  mikulis
// Add istream command
//
// Revision 1.10  1995/10/12  11:43:03  quinn
// updated for vms port
//
// Revision 1.9  1995/10/05  22:01:55  asher
// Move function for "su" command to verif_utils
//
// Revision 1.8  1995/09/20  18:51:09  mikulis
// Disable probe latency tokens.
//
// Revision 1.7  1995/09/15  15:36:21  mikulis
// fetch_blk_on/off no longer interesting
//
// Revision 1.6  1995/09/12  17:56:25  mikulis
// Add new Cbox stats
//
// Revision 1.5  1995/09/01  17:16:19  mikulis
// Add some more Cbox statistics
//
// Revision 1.4  1995/08/22  14:20:25  mikulis
// Added Cbox/Xbox statistic collection
//
// Revision 1.3  1995/08/11  20:15:40  huggins
// added auto save feature
//
// Revision 1.2  1995/08/07  13:26:34  huggins
// take out ifdef C_ALL
// include new ief_utils container file
//
// Revision 1.1  1995/08/03  19:50:21  huggins
// Initial revision
//
//
*/


#include <math.h>
#include <mxxio.hxx>
#include <mxxserver.hxx>
#include <mxxdlink.hxx>
#include <fstream.hxx>

#include "ev6_config.h"
#include "MTV_Number.hxx"
#include "verif_utils__cnt.hxx"
#include "c_csr__cnt.hxx"
#include "c_pads__cnt.hxx"
#include "k__cnt.hxx"
#include "XStats.hxx"
#include "Xbox.hxx"
#include "m_stq__cnt.hxx"
#include "m_sdc__cnt.hxx"
#include "m_main__cnt.hxx"
#include "m_trp__cnt.hxx"
#include "c_abus__cnt.hxx"
#include "m__cnt.hxx"
#include "m_maf__cnt.hxx"
#include "m_ipr__cnt.hxx"
#include "spot__cnt.hxx"
#include "i_ret__cnt.hxx"
#include "m_ldq__cnt.hxx"
#include "i__cnt.hxx"
#include "c_dbus__cnt.hxx"
#include "c__cnt.hxx"
#include "c_formater__cnt.hxx"
// Don't include this garp in the CX model
//
#ifndef CBOX_DEBUG
#include "m_mrt__cnt.hxx"
#include "i_iq__cnt.hxx"
#include "i_ipr__cnt.hxx"
#include "t__cnt.hxx"
#include "t_ica__cnt.hxx"
#include "j_main__cnt.hxx"
#ifdef __osf__
#include <stdio.h>
#include <filehdr.h>
#include <aouthdr.h>
#include <scnhdr.h>
#include "EV6_CLI_Objects.hxx"
#include "mtvcli.hxx" // for x_top
#endif
#endif // not CBOX_DEBUG
#define inc(var, val) ((val == 0x1f) ? (0) : (var++))

#define MIN_WRIO_DATA_DELAY 7

MXX_VERSION("@(#) $Id: verif_utils.cxx,v 1.1.1.1 1998/12/29 21:36:22 paradis Exp $");

#ifndef GET_U64
#ifdef DONT_HAVE_GET_U64
#define GET_U64(sig, ind)	sig[ind]
#else
#define GET_U64(sig, ind)	sig.get_u64(ind)
#endif
#endif

int verif_utils__beh::save(FILE* fptr)
{
    fwrite(&max_ic_addr, sizeof(max_ic_addr), 1, fptr);
    fwrite(&fake_srom_curbit, sizeof(fake_srom_curbit), 1, fptr);
    fwrite(&fake_srom_numqws, sizeof(fake_srom_numqws), 1, fptr);
    if (fake_srom_numqws != 0)
        fwrite(fake_srom_data_p, 8, fake_srom_numqws, fptr);
    
    // nothing to do here
    return 1;
}

int verif_utils__beh::restore(FILE* fptr)
{
    fread(&max_ic_addr, sizeof(max_ic_addr), 1, fptr);
    fread(&fake_srom_curbit, sizeof(fake_srom_curbit), 1, fptr);
    fread(&fake_srom_numqws, sizeof(fake_srom_numqws), 1, fptr);
    if (fake_srom_numqws != 0) {
        
        if (fake_srom_data_p)
            delete [] fake_srom_data_p;

        fake_srom_data_p = new MTV_uint64[fake_srom_numqws];
        
        fread(fake_srom_data_p, 8, fake_srom_numqws, fptr);
    }

    auto_save_enable = 0;  // force this to off on restore.
    auto_save_last_time = 0;
    auto_save_index = 0;
    for (int i = 0; i < 50; ++i)
        auto_save_times[i] = 0;
    stop_cycle[0] = '\0';
    return 1;
}
int verif_utils__beh::init()
{
    sprintf(auto_save_base_name,"ev6");
    for (int i = 0; i < 50; ++i)
        auto_save_times[i] = 0;
    stop_cycle[0] = '\0';

    fake_srom_numqws = 0;
    
    return 1;
}
int verif_utils__beh::run(int)
{

    assertion_checkers();

    if (k->clk) 
    {
      perf_counters();

      if (verif_utils->fake_srom_enable)
        {
          fake_srom();
        }
      
      if (verif_utils->drv_pll_enable)
        {
          drv_pll();
        }
      
    }

    warning_count = MXX_warning_count();
    error_count   = MXX_error_count();
    
    if (stop_cycle[0]!='\0' && ef_stop_at_cycle_phase(stop_cycle)) {
      stop_cycle[0] = '\0';
      MXX_stop(mxxinform);
    }
    
    return 1;     
}

void verif_utils__beh::assertion_checkers()
{
  if (verif_utils->mbox_checkers_enable)
    {
      mbox_checkers();
      ldq_inorder_ldx();
      count_mbox_traps();   // Not really an assertion checker
    }
  if (verif_utils->deadlock_stats_enable) {
      deadlock_stats();     // This isn't either
  }
}

void verif_utils__beh::perf_counters()
{
#ifndef CBOX_DEBUG

  x_stats->cycles      += 1;
  x_stats->traps      += i_perf_artifact->traps;
  x_stats->retired_instr      += i_perf_artifact->retired_instr;
  
  x_stats->issued_l0_instr    += (int) i_iq_int->issue_l0_4a_h;
  x_stats->issued_l1_instr    += (int) i_iq_int->issue_l1_4a_h;
  x_stats->issued_u0_instr    += (int) i_iq_int->issue_u0_4a_h;
  x_stats->issued_u1_instr    += (int) i_iq_int->issue_u1_4a_h;
  x_stats->issued_f0_instr    += (int) i->iss_f0_4a_h;
  x_stats->issued_f1_instr    += (int) i->iss_f1_4a_h;

  x_stats->train_ghist      += i_perf_artifact->train_ghist;

  x_stats->retired_ghist_mispr  += i_perf_artifact->retired_ghist_mispr    ;
  x_stats->retired_lhist_mispr   += i_perf_artifact->retired_lhist_mispr    ;
  x_stats->retired_choose_ghist   += i_perf_artifact->retired_choose_ghist   ;
  x_stats->retired_choose_lhist   += i_perf_artifact->retired_choose_lhist   ;
  x_stats->retired_choose_wrong   += i_perf_artifact->retired_choose_wrong   ;
  x_stats->retired_br_mispr   += i_perf_artifact->retired_br_mispr       ;
  x_stats->retired_cbr   += i_perf_artifact->retired_cbr            ;
  x_stats->unretired_br_mispr   += i_perf_artifact->unretired_br_mispr     ;
  x_stats->ld_st_order_trap   += i_perf_artifact->ld_st_order_trap      ;
  x_stats->ld_ld_order_trap   += i_perf_artifact->ld_ld_order_trap      ;
  x_stats->ldq_full_trap   += i_perf_artifact->ldq_full_trap         ;
  x_stats->litmus_trap   += i_perf_artifact->litmus_trap           ;
  x_stats->ldq_troll_trap   += i_perf_artifact->ldq_troll_trap        ;
  x_stats->ldq_stxc_trap   += i_perf_artifact->ldq_stxc_trap         ;
  x_stats->simul_order_trap   += i_perf_artifact->simul_order_trap      ;
  x_stats->ld_wrong_stdat_trap   += i_perf_artifact->ld_wrong_stdat_trap   ;
  x_stats->ld_wrong_stsiz_trap   += i_perf_artifact->ld_wrong_stsiz_trap   ;
  x_stats->dc_ld_st_trap   += i_perf_artifact->dc_ld_st_trap         ;
  x_stats->troll_st_trap   += i_perf_artifact->troll_st_trap         ;
  x_stats->stq_full_trap   += i_perf_artifact->stq_full_trap         ;
  x_stats->simul_troll_trap   += i_perf_artifact->simul_troll_trap      ;
  x_stats->simul_dc_ld_st_trap   += i_perf_artifact->simul_dc_ld_st_trap   ;

  x_stats->dtb_miss_double        += i_perf_artifact->dtb_miss_double        ;
  x_stats->fen_retired_trap       += i_perf_artifact->fen_retired_trap       ;
  x_stats->unaligned_retired_trap += i_perf_artifact->unaligned_retired_trap ;
  x_stats->dtb_miss_single        += i_perf_artifact->dtb_miss_single        ;
  x_stats->dfault_retired_trap    += i_perf_artifact->dfault_retired_trap    ;
  x_stats->opcdec_retired_trap    += i_perf_artifact->opcdec_retired_trap    ;
  x_stats->iacv_retired_trap      += i_perf_artifact->iacv_retired_trap      ;
  x_stats->mchk_retired_trap      += i_perf_artifact->mchk_retired_trap      ;
  x_stats->itb_miss               += i_perf_artifact->itb_miss               ;
  x_stats->arith_retired_trap     += i_perf_artifact->arith_retired_trap     ;
  x_stats->interrupt_retired_trap += i_perf_artifact->interrupt_retired_trap ;
  x_stats->mt_fpcr_retired_trap   += i_perf_artifact->mt_fpcr_retired_trap   ;
  x_stats->reset_retired_trap     += i_perf_artifact->reset_retired_trap     ;

  x_stats->maf_full_retry   += i_perf_artifact->maf_full_retry         ;

  x_stats->jsr_mispr   += i_perf_artifact->jsr_mispr              ;
  x_stats->jsr_mispr_call   += i_perf_artifact->jsr_mispr_call         ;
  x_stats->jsr_mispr_jmp   += i_perf_artifact->jsr_mispr_jmp          ;
  x_stats->jsr_mispr_ret   += i_perf_artifact->jsr_mispr_ret          ;
  x_stats->frontend_stall   += i_perf_artifact->frontend_stall         ;
  x_stats->ic_fill_stall   += i_perf_artifact->ic_fill_stall          ;
  x_stats->ic_fetch   += i_perf_artifact->ic_fetch               ;
  x_stats->line_code_mispr   += i_perf_artifact->line_code_mispr        ;
  x_stats->line_addr_mispr   += i_perf_artifact->line_addr_mispr        ;
  x_stats->fill_under_stall_bbl   += i_perf_artifact->fill_under_stall_bbl   ;
  x_stats->set_mispr   += i_perf_artifact->set_mispr              ;
  x_stats->fill_bank_confl_bbl   += i_perf_artifact->fill_bank_confl_bbl      ;
  x_stats->iq_full_stall   += i_perf_artifact->iq_full_stall          ;
  x_stats->fq_full_stall   += i_perf_artifact->fq_full_stall          ;
  x_stats->int_map_stall   += i_perf_artifact->int_map_stall          ;
  x_stats->fp_map_stall   += i_perf_artifact->fp_map_stall           ;
  x_stats->inum_stall   += i_perf_artifact->inum_stall             ;
  x_stats->stop_retire   += i_perf_artifact->stop_retire             ;
  x_stats->mbx_stop_retire   += i_perf_artifact->mbx_stop_retire             ;
  x_stats->mbx_stop_overload   += i_perf_artifact->mbx_stop_overload             ;
  x_stats->ipr_q_stall   += i_perf_artifact->ipr_q_stall            ;
  x_stats->backend_stall   += i_perf_artifact->backend_stall          ;
  x_stats->cmov_stall   += i_perf_artifact->cmov_stall            ;

  // Internal NXM count
  x_stats->internal_nxm_count += (int)c_adc_smx->internal_nxm_err_9a;
  
  // stx_c replayed because MAF was full
  if ((m_maf_int->stc0_8a_h & m_maf_int->st_full0_8a_h) ||
      (m_maf_int->stc1_8a_h & m_maf_int->st_full1_8a_h))
      x_stats->maf_full_stxc_replay_trap++;

  // Number of stx_c that succeeded, number that failed
  if (m_ipr_int->stc_state_known_6a_h) {
      if (m_ipr_int->stc_lock_flag0_6a_h)
          x_stats->stxc_success++;
      else {
          x_stats->stxc_failure++;
          if (spot->stxc_probe_evicted_flag)
              x_stats->stxc_fail_due_to_evict++;
      }
  }                                                                 

  // Add number of valid entries in LDQ, STQ, and MAF this cycle
  int i;
  int ldq_valid_vector = (int)(GET_U64(m_ldq_stg->valid_a_h, 0));
  int stq_valid_vector = (int)(GET_U64(m_stq_stg->valid_a_h, 0));
  int maf_valid_vector = (int)(GET_U64(m_maf_stg->valid_a_h, 0));
  for (i = 0; i < 32; i++) {
      if (ldq_valid_vector & (1 << i))
          x_stats->ldq_size++;
      if (stq_valid_vector & (1 << i))
          x_stats->stq_size++;
  }
  for (i = 0; i < 8; i++) {
      if (maf_valid_vector & (1 << i))
          switch ((Signal) m_maf_stg->type_a_h[i]) {
          case M_MAF_LD:
          case M_MAF_LD_IO:
          case M_MAF_ST:
          case M_MAF_ZERO:
          case M_MAF_EVICT:
          case M_MAF_RDERR:
              x_stats->maf_dstream_size++;
              break;
          case M_MAF_IMISS:
          case M_MAF_IPREF:
              x_stats->maf_istream_size++;
              break;
          }
  }
                      
  // Figure out how many stores have retired since last cycle.
  // Stores are retired in B phase using a complex equation so
  // instead of replicating it here I just XOR the STQ retired
  // vector with the one from last time to capture any changes.
  static int last_ret_stores = 0;
  int ret_stores, new_stores;

  new_stores = GET_U64(m_stq_stg->retired_a_h, 0);
  ret_stores = last_ret_stores ^ new_stores;

  // Do similar thing for loads.
  static int last_ret_loads = 0;
  int ret_loads, new_loads;

  new_loads = GET_U64(m_ldq_stg->retired_a_h, 0);
  ret_loads = last_ret_loads ^ new_loads;
  
  // Count up the bits set
  for (i = 0; i <= 31; i++) {
      if ((ret_loads & 1) && (((last_ret_loads >> i) & 1) == 0)) {
          x_stats->total_loads++;
          x_stats->io_loads += m_ldq_stg->pa_43_a_h(i); // Count I/O space loads
      }
      if ((ret_stores & 1) && (((last_ret_stores >> i) & 1) == 0)) {
          x_stats->total_stores++;
          x_stats->io_stores += m_stq_stg->pa_43_a_h(i);// Count I/O space stores
      }
      ret_loads >>= 1;
      ret_stores >>= 1;
  }
  last_ret_stores = new_stores;
  last_ret_loads = new_loads;
      
  
#endif // not CBOX_DEBUG
}

// This is a simple tuple used in deadlock_stats()
class prb_node {
public:
    int addr;  // Block address
    int cycle; // Cycle it first appeared
    int type;  // Type of op that matched
};

// We got several bugs late in the project which caused deadlocks; these
// were distinguished by special cache manipulation instructions (WH64,
// ECB, and prefetches) interacting with other instructions to the same
// address in the STQ and LDQ, and also interacting with probes.
// This statistics-gathering function keeps track of the following info:
//
//   verif_utils->xxx_match[A][B]
//      An xxx is present in the LDQ or STQ; "A" other instructions are
//      present in the STQ and LDQ combined, all with the same address
//      as the xxx.  "A" is in the range 0..5 with 0 representing more
//      than five.  (There is no count for zero matches.)  The "B"
//      represents the retry states of the instructions:
//        0 : Nothing was in retry
//        1 : xxx was in retry
//        2 : At least one of the matches in the LDQ or STQ was in retry
//        3 : They both were in retry
//      These counts are incremented for each occurence (not per cycle.)
//
//   verif_utils->prb_xxx_before
//      These counts keep track of how many times a probe became valid
//      in the PRQ within a certain number of cycles of xxx appearing
//      in the LDQ or STQ.  In addition to xxx appearing in the queue,
//      it must address-match a certain number of other entries in the
//      LDQ and STQ.  The first parameter is user-settable in
//      verif_utils->prb_cycles, and the second one is user-settable in
//      verif_utils->prb_match_amount.  verif_utils->prb_match_amount
//      may be set to zero.
//
//   verif_utils->prb_xxx_after
//      Same as prb_xxx_before except keeps track of probes after the
//      given event.  Uses the same parameters.
//
//   Use the signal verif_utils->deadlock_stats_enable to enable this
//   function.  This will also dump all data to the stats database.

void verif_utils__beh::deadlock_stats()
{
#ifndef CBOX_DEBUG

#define ECB          1
#define WH64         2
#define PRE_EVN      3
#define PRE_MOD      4
#define PRE_NOCACHE  5
#define PRE_NORM     6
    
    static DLList<prb_node> prb_before_list, prb_after_list;
    static int prb_events[8] = {0,0,0,0,0,0,0,0};         // Used for keeping track of events instead of cycles
    static int prb_events_after[8] = {0,0,0,0,0,0,0,0};   // Likewise
    static int ldq_events[32][4], stq_events[32][4];      // Likewise
    static int address_events[64];
    static int probe_events2[64];
    int ldq_matches, stq_matches;       // Keep track of how many matches found this iteration
    int x, y;                           // Loop counters
    int z;                              // Temp
    prb_node p1, p2;                    // Temporary var for traversing lists
    Pix ind;                            // Index for traversing lists
    int current_cycle;                  // Holds the number of the current cycle since run start
    int cmp_address, new_address;       // Addresses to compare against
    int retry_code;                     // See above for encoding

    // Snapshot these once per cycle
    int ldq_valid_vector = (int)(GET_U64(m_ldq_stg->valid_a_h, 0));
    int ldq_retry_vector = (int)(GET_U64(m_ldq_stg->retry_a_h, 0));
    int stq_valid_vector = (int)(GET_U64(m_stq_stg->valid_a_h, 0));
    int stq_retry_vector = (int)(GET_U64(m_stq_stg->retry_trigger_a_h, 0));
    int evict_blk_vector = (int)(GET_U64(m_stq_stg->evict_blk_a_h, 0));
    int zero_blk_vector  = (int)(GET_U64(m_stq_stg->zero_blk_a_h, 0));

    // Check for new probes
    current_cycle = (int)(GET_U64(x_in_u->cycle_h,0));
    int prb_valid_vector = (int)(GET_U64(c_adc_prq->valid_a, 0));
    for (x = 0; x < 8; x++) {
        if ((prb_valid_vector & (1 << x))) {    // Probe is valid
            if (prb_events[x] == 0) {           // and it wasn't last cycle
                // Add new address to before list
                p1.addr = ((int)(GET_U64(c_adp_prq->pa_index[x],0)) << 6)
                    | ((int)(GET_U64(c_adp_prq->pa_tag[x],0)) << 20);
                p1.cycle = current_cycle;
                prb_before_list.append(p1);
                // Search after list for addresses
                for (ind = prb_after_list.first(); ind; prb_after_list.next(ind)) {
                    p2 = prb_after_list(ind);
                    if (p2.addr == p1.addr) {
                        switch (p2.type) {
                        case ECB: verif_utils->prb_ecb_after++; break;
                        case WH64: verif_utils->prb_wh64_after++; break;
                        case PRE_EVN: verif_utils->prb_pre_evn_after++; break;
                        case PRE_MOD: verif_utils->prb_pre_mod_after++; break;
                        case PRE_NOCACHE: verif_utils->prb_pre_nocache_after++; break;
                        case PRE_NORM: verif_utils->prb_pre_norm_after++; break;
                        }
                    }
                }
                prb_events[x] = 1;               // Mark as new probe appearing this cycle
            }
        }
        else                                 // If probe is invalid
            prb_events[x] = 0;               // then clear event widget
    }
    
    // Go through probe list and delete probes that are too old to be interesting
    for (ind = prb_before_list.first(); ind; prb_before_list.next(ind)) {
        p1 = prb_before_list(ind);
        if ((current_cycle - (int)(verif_utils->prb_cycles)) > p1.cycle)
            prb_before_list.del(ind);
    }

    // Purge address matches that are too old to be interesting
    for (ind = prb_after_list.first(); ind; prb_after_list.next(ind)) {
        p1 = prb_after_list(ind);
        if ((current_cycle - p1.cycle) > (int)(verif_utils->prb_cycles))
            prb_after_list.del(ind);
    }
                
    for (x = 0; x < 32; x++) {   // For every entry in LDQ
        if ((ldq_valid_vector & (1 << x))                           // If entry is valid
            && (((int)GET_U64(m_mrt_stg->rnum_b_h[x], 0) & 0x7f) == 0x68)) { // and it's a prefetch
            // Note: rnum 0x68 "maps" to f31/r31; the upper bit (bit 7) is set if floating point
            ldq_matches = 0;

            if (ldq_retry_vector & (1 << x))
                retry_code = 1;
            else
                retry_code = 0;
            
            // Get address (block) of current entry to compare against
            cmp_address = (int)((GET_U64(m_ldq_stg->pa_43_20_a_h[x], 0) << 20)
                                | (GET_U64(m_ldq_stg->pa_19_13_a_h[x], 0) << 13)
                                | (GET_U64(m_ldq_stg->pa_12_6_a_h[x], 0) << 6));

            for (y = 0; y < 32; y++) {
                // Look for address matches in LDQ
                if (ldq_valid_vector & (1 << y)) {
                    if (x == y)       // Don't count itself
                        break;
                    new_address = (int)((GET_U64(m_ldq_stg->pa_43_20_a_h[y], 0) << 20)
                                        | (GET_U64(m_ldq_stg->pa_19_13_a_h[y], 0) << 13)
                                        | (GET_U64(m_ldq_stg->pa_12_6_a_h[y], 0) << 6));
                    if (new_address == cmp_address) {
                        ldq_matches++;
                        if (ldq_retry_vector & (1 << y))
                            retry_code = retry_code | 2;
                    }
                }
                // Look for address matches in STQ
                if (stq_valid_vector & (1 << y)) {
                    new_address = (int)((GET_U64(m_stq_stg->pa_43_20_a_h[y], 0) << 20)
                                        | (GET_U64(m_stq_stg->pa_19_13_a_h[y], 0) << 13)
                                        | (GET_U64(m_stq_stg->pa_12_6_a_h[y], 0) << 6));
                    if (new_address == cmp_address) {
                        ldq_matches++;
                        if (stq_retry_vector & (1 << y))
                            retry_code = retry_code | 2;
                    }
                }
            }

            if (ldq_matches != 0) {     // If no matches were found we don't want to increment any xxx_match counters
                if (ldq_matches > 5)    // For more than five matches, we store it in xxx_match[0]
                    z = 0;
                else
                    z = ldq_matches;

                // We want to count events rather than incrementing a count every cycle.
                // So we keep track of whether we saw this same event last cycle.
                if (ldq_events[x][retry_code] != z) {
                    // Update counters according to what type of prefetch it was
                    switch ((int)(GET_U64(m_mrt_stg->format_b_h[x],0))) {
                    case M_QINT : verif_utils->pre_evn_match[z][retry_code]++; break;
                    case M_SFLT : verif_utils->pre_mod_match[z][retry_code]++; break;
                    case M_TFLT : verif_utils->pre_nocache_match[z][retry_code]++; break;
                    default : verif_utils->pre_norm_match[z][retry_code]++; break;
                    }
                    ldq_events[x][retry_code] = z;
                }
            }
            // Check if there was a probe to this address recently
            if ((int)(verif_utils->prb_match_amount) <= ldq_matches) {
                for (ind = prb_before_list.first(); ind; prb_before_list.next(ind)) {
                    p1 = prb_before_list(ind);
                    if ((p1.addr == cmp_address)
                        && (probe_events2[x] == 0)) {
                        switch ((int)(GET_U64(m_mrt_stg->format_b_h[x],0))) {
                        case M_QINT : verif_utils->prb_pre_evn_before++; break;
                        case M_SFLT : verif_utils->prb_pre_mod_before++; break;
                        case M_TFLT : verif_utils->prb_pre_nocache_before++; break;
                        default : verif_utils->prb_pre_norm_before++; break;
                        }
                        probe_events2[x] = 1;
                    }
                 }
            }
            else
                probe_events2[x] = 0;
            
            // Add data to probe_after list if it meets our spec
            if ((ldq_matches >= (int)(GET_U64(verif_utils->prb_match_amount,0)))
                && (address_events[x] == 0)) {
                p1.addr = cmp_address;
                p1.cycle = current_cycle;
                switch ((int)(GET_U64(m_mrt_stg->format_b_h[x],0))) {
                case M_QINT: p1.type = PRE_EVN; break;
                case M_SFLT: p1.type = PRE_MOD; break;
                case M_TFLT: p1.type = PRE_NOCACHE; break;
                default: p1.type = PRE_NORM; break;
                }
                prb_after_list.append(p1);
                address_events[x] = 1;
            }
        }
        else {  // if entry is not valid or is not a prefetch, update our event vector
            ldq_events[x][0] = -1;
            ldq_events[x][1] = -1;
            ldq_events[x][2] = -1;
            ldq_events[x][3] = -1;
            address_events[x] = 0;
            probe_events2[x] = 0;
        }
        
        // Now go through STQ looking for ECBs and WH64s
        if ((stq_valid_vector & (1 << x))       // If entry is valid...
            && ((evict_blk_vector & (1 << x))       // and it's an ECB
                || zero_blk_vector & (1 << x))) {   // or it's a WH64

            stq_matches = 0;

            if (stq_retry_vector & (1 << x))
                retry_code = 1;
            else
                retry_code = 0;
            
            // Get address of current entry to compare against
            cmp_address = (int)((GET_U64(m_stq_stg->pa_43_20_a_h[x], 0) << 20)
                                | (GET_U64(m_stq_stg->pa_19_13_a_h[x], 0) << 13)
                                | (GET_U64(m_stq_stg->pa_12_6_a_h[x], 0) << 6));
            
            for (y = 0; y < 32; y++) {
                if (ldq_valid_vector & (1 << y)) {
                    new_address = (int)((GET_U64(m_ldq_stg->pa_43_20_a_h[y], 0) << 20)
                                        | (GET_U64(m_ldq_stg->pa_19_13_a_h[y], 0) << 13)
                                        | (GET_U64(m_ldq_stg->pa_12_6_a_h[y], 0) << 6));
                    if (new_address == cmp_address) {
                        stq_matches++;
                        if (ldq_retry_vector & (1 << y))
                            retry_code = retry_code | 2;
                    }
                }
                if (stq_valid_vector & (1 << y)) {
                    if (x == y)    // Don't count itself
                        break;
                    new_address = (int)((GET_U64(m_stq_stg->pa_43_20_a_h[y], 0) << 20)
                                        | (GET_U64(m_stq_stg->pa_19_13_a_h[y], 0) << 13)
                                        | (GET_U64(m_stq_stg->pa_12_6_a_h[y], 0) << 6));
                    if (new_address == cmp_address) {
                        stq_matches++;
                        if (stq_retry_vector & (1 << y))
                            retry_code = retry_code | 2;
                    }
                }               
            }

            if (stq_matches != 0) {
                if (stq_matches > 5)  // For more than five matches, we store it in xxx_match[0]
                    z = 0;
                else
                    z = stq_matches;

                if (stq_events[x][retry_code] != z) {
                    // Update counters according to which op it was
                    if (evict_blk_vector & (1 << x)) {
                        verif_utils->ecb_match[z][retry_code]++;
                    }
                    else {
                        verif_utils->wh64_match[z][retry_code]++;
                    }
                    stq_events[x][retry_code] = z;
                }
            }
            // Check if there was a probe to this address recently
            if  ((int)(verif_utils->prb_match_amount) <= stq_matches) {
                for (ind = prb_before_list.first(); ind; prb_before_list.next(ind)) {
                    p1 = prb_before_list(ind);
                    if ((p1.addr == cmp_address)
                        && (probe_events2[32+x] == 0)) {
                     
                        if (evict_blk_vector & (1 << x)) {
                            verif_utils->prb_ecb_before++;
                        }
                        else {
                            verif_utils->prb_wh64_before++;
                        }
                        probe_events2[32+x] = 1;
                    }
                }
            }
            else
                probe_events2[32+x] = 0;
            
            // Add data to probe_after list if it meets our spec
            if ((stq_matches >= (int)(GET_U64(verif_utils->prb_match_amount,0)))
                && (address_events[32+x] == 0)) {
                p1.addr = cmp_address;
                p1.cycle = current_cycle;
                if (evict_blk_vector & (1 << x)) {
                    p1.type = ECB;
                }
                else {
                    p1.type = WH64;
                }
                
                prb_after_list.append(p1);
                address_events[32+x] = 1;
            }
        }
        else {
            stq_events[x][0] = -1;
            stq_events[x][1] = -1;
            stq_events[x][2] = -1;
            stq_events[x][3] = -1;
            address_events[32+x] = 0;
            probe_events2[32+x] = 0;
        }
    }
#endif // not CBOX_DEBUG
}

// Add up all MBOX traps and retries taken in this cycle
void verif_utils__beh::count_mbox_traps()
{
#ifndef CBOX_DEBUG
    int count = 0;
    int index = 0, bit;
    int signals[50];

    // First count traps caused by STQ and LDQ -- these are stored in
    // 32 bit vectors, one bit for each entry causing the trap or retry
    signals[index++] = GET_U64(m_ldq_int->full0_7b_h, 0);
    signals[index++] = GET_U64(m_ldq_int->full1_7b_h, 0);
    signals[index++] = GET_U64(m_ldq_int->ld_st_retry0_7b_h, 0);
    signals[index++] = GET_U64(m_ldq_int->ld_st_retry1_7b_h, 0);
    signals[index++] = GET_U64(m_ldq_int->litmus_trap0_7b_h, 0);
    signals[index++] = GET_U64(m_ldq_int->litmus_trap1_7b_h, 0);
    signals[index++] = GET_U64(m_ldq_int->ldq_lock_troll_trap0_7b_h, 0);
    signals[index++] = GET_U64(m_ldq_int->ldq_lock_troll_trap1_7b_h, 0);
    signals[index++] = GET_U64(m_ldq_int->order_troll_st_trap0_7b_h, 0);
    signals[index++] = GET_U64(m_ldq_int->order_troll_st_trap1_7b_h, 0);
    //signals[index++] = GET_U64(m_ldq_int->out_of_order0_7b_h, 0);
    //signals[index++] = GET_U64(m_ldq_int->out_of_order1_7b_h, 0);
    signals[index++] = GET_U64(m_ldq_int->ldq_stxc0_7b_h, 0);
    signals[index++] = GET_U64(m_ldq_int->ldq_stxc1_7b_h, 0);
    signals[index++] = GET_U64(m_ldq_int->ldq_troll_trap0_7b_h, 0);
    signals[index++] = GET_U64(m_ldq_int->ldq_troll_trap1_7b_h, 0);
    signals[index++] = GET_U64(m_ldq_int->ldq_troll_retry0_7b_h, 0);
    signals[index++] = GET_U64(m_ldq_int->ldq_troll_retry1_7b_h, 0);

    signals[index++] = GET_U64(m_stq_int->dc_ld_st_trap0_7b_h, 0);
    signals[index++] = GET_U64(m_stq_int->dc_ld_st_trap1_7b_h, 0);
    signals[index++] = GET_U64(m_stq_int->full0_7b_h, 0);
    signals[index++] = GET_U64(m_stq_int->full1_7b_h, 0);
    signals[index++] = GET_U64(m_stq_int->ld_wrong_nohit0_7b_h, 0);
    signals[index++] = GET_U64(m_stq_int->ld_wrong_nohit1_7b_h, 0);
    signals[index++] = GET_U64(m_stq_int->ld_wrong_stdat0_7b_h, 0);
    signals[index++] = GET_U64(m_stq_int->ld_wrong_stdat1_7b_h, 0);
    signals[index++] = GET_U64(m_stq_int->ld_wrong_stsiz0_7b_h, 0);
    signals[index++] = GET_U64(m_stq_int->ld_wrong_stsiz1_7b_h, 0);
    signals[index++] = GET_U64(m_stq_int->troll_st0_7b_h, 0);
    signals[index] = GET_U64(m_stq_int->troll_st1_7b_h, 0);

    // The following code counts one trap for every entry in the LDQ/STQ,
    // so if 10 entries troll trap against an incoming load, there will be
    // 10 traps counted.  It is more interesting to just count types of
    // traps occuring, so a simultaneous troll trap and litmus trap will
    // record as two traps, but two troll traps will not.  The exception to
    // this is out_of_order traps, which trap against the operation already in
    // the LDQ/STQ rather than the incoming operation.

    // Don't do this anymore
    //for (bit = 31; bit >= 0; bit--) {
    //  for (int x = index; x >= 0; x--) {
    //      count += (signals[x] >>= 1) & 1;
    //  }
    //}

    for (int x =  index; x >= 0; x--) {
        if (signals[x] != 0)
            count++;
    }

    // Count out-of-order traps individually
    signals[0] = GET_U64(m_ldq_int->out_of_order0_7b_h, 0);
    signals[1] = GET_U64(m_ldq_int->out_of_order1_7b_h, 0);
    for (x = 0; x < 32; x++) {
        count += (signals[0] >> x) & 1;
        count += (signals[1] >> x) & 1;
    }
    
    // Now count traps caused by the MAF -- this is similar to above,
    // but there are only 8 entries in the MAF to cause traps
    index = 0;
    // This is the only MAF trap
    signals[index++] = GET_U64(m_maf_int->lck_stc_trap0_8a_h, 0);
    signals[index++] = GET_U64(m_maf_int->lck_stc_trap1_8a_h, 0);
    // These are MAF retries -- note that m_maf_int->ld_hit?_8a_h must be zero also
    if (~m_maf_int->ld_hit0_8a_h) {  // Pipe 0
        signals[index++] = GET_U64(m_maf_int->troll_trap0_8a_h, 0);
        signals[index++] = GET_U64(m_maf_int->type_trap0_8a_h, 0);
    }
    if (~m_maf_int->ld_hit1_8a_h) {  // Pipe 1
        signals[index++] = GET_U64(m_maf_int->troll_trap1_8a_h, 0);
        signals[index++] = GET_U64(m_maf_int->type_trap1_8a_h, 0);
    }
    index--;
    
    for (x = index; x >= 0; x--) {
        if (signals[x] != 0)
            count++;
    }

    // Now count miscellaneous traps and retries not in vectors
    // LDQ simul traps
    count += m_ldq_int->simul_order_trap0_7b_h;
    count += m_ldq_int->simul_order_trap1_7b_h;
    // LDQ simul retries
    count += m_ldq_int->simul_order_retry0_7b_h;
    count += m_ldq_int->simul_order_retry1_7b_h;
    count += m_ldq_int->simul_troll_retry0_7b_h;
    count += m_ldq_int->simul_troll_retry1_7b_h;
    // MAF stuff
    if (~m_maf_int->ld_hit0_8a_h) {
        count += m_maf_int->force_retry0_8a_h;
        count += m_maf_int->maf_full0_8a_h;
        count += m_maf_int->st_full0_8a_h;
        count += m_maf_int->ld_st_io_order_trap_8a_h;
    }
    if (~m_maf_int->ld_hit1_8a_h) {
        count += m_maf_int->st_full1_8a_h;
        count += m_maf_int->force_retry1_8a_h;
        count += m_maf_int->simul_troll_trap1_8a_h;
        count += m_maf_int->simul_type_trap1_8a_h;
    }
    // STQ simul traps
    count += m_stq_int->simul_dc_ld_st_trap0_7b_h;
    count += m_stq_int->simul_dc_ld_st_trap1_7b_h;
    count += m_stq_int->simul_troll0_7b_h;
    count += m_stq_int->simul_troll1_7b_h;
    // STQ retries
    count += m_stq_int->st_rty_inval_rty0_8a_h;
    count += m_stq_int->stc_zblk_rty0_8a_h;
    count += m_stq_int->stc_zblk_rty1_8a_h;
    // Architectural traps
    count += m_trp_int->acv_err0_8a_h;
    count += m_trp_int->acv_err1_8a_h;
    count += m_trp_int->alignment_err0_8a_h;
    count += m_trp_int->alignment_err1_8a_h;
    count += m_trp_int->dtb_miss0_8a_h;
    count += m_trp_int->dtb_miss1_8a_h;
    count += m_trp_int->fen_trap0_8a_h;
    count += m_trp_int->fen_trap1_8a_h;
    count += m_trp_int->tag_perr0_8a_h;
    count += m_trp_int->tag_perr1_8a_h;

    verif_utils->mbox_trap_count = count;
#endif
}

void verif_utils__beh::mbox_checkers()
{
#ifndef CBOX_DEBUG
  // STQ-SDC no-hit bit checker

  static unsigned int nh_mask = 0, stq_nh = 0;
  
  // if store & trap, set no hit mask bit for stq with corrosponding inum

  nh_mask = nh_mask & (MXX_uint64)(m_stq_stg->valid_a_h & m_sdc_stg->valid_a_h);
  
  
  if (~m->mbox_trp0_8ac_l & m_stq_int->st0_8a_h)
    for (int i=0; i<32; i++) {
      if (m_stq_stg->inum_a_h[i] == m_trp_int->inum0_8a_h)
        nh_mask = nh_mask | (1 << i);
    } 
  if (~m->mbox_trp1_8ac_l & m_stq_int->st1_8a_h)
    for (int i=0; i<32; i++) {
      if (m_stq_stg->inum_a_h[i] == m_trp_int->inum1_8a_h)
        nh_mask = nh_mask | (1 << i);
    }

  stq_nh = (MXX_uint64)m_stq_stg->no_hit_a_h;
  if((k->clk) &&
     ((~nh_mask & stq_nh & (MXX_uint64)(m_stq_stg->valid_a_h & m_sdc_stg->valid_a_h)) !=
      (~nh_mask & stq_nh & (MXX_uint64)(m_stq_stg->valid_a_h & m_sdc_stg->valid_a_h & m_sdc_stg->no_hit_a_h)))) {
    BehErr << "STQ and SDC NH bit missmatch" << mxxerror("X_ERROR");
    MxxOut << "       STQ V [" << m_stq_stg->valid_a_h << "] NH [" << stq_nh << "]" << endl;
    MxxOut << "       SDC V [" << m_sdc_stg->valid_a_h << "] NH [" << m_sdc_stg->no_hit_a_h << "]" << endl;
  }

#endif  // CBOX_DEBUG
}

// This assertion checker checks that all valid, non-retired LDQ entries are
// in ascending order.  Note that it is possible to break this assertion checker
// legally -- if a very old load is issued after 32 younger loads have issued and
// filled the queue.  The machine will full-trap everything and be happy but this
// checker will fire anyway.
void verif_utils__beh::ldq_inorder_ldx()
{
#ifndef CBOX_DEBUG
  int ptr = m_ldq_stg->done_pntr_a_h, i = ptr, j = ptr;
  
  while ( (((ptr-i)&31)!=1) && (MXX_uint64)m_ldq_stg->valid_a_h(i) && !(MXX_uint64)m_ldq_stg->retired_a_h(i)) {
      j = (i+1)&31;
      if (((MXX_uint64)m_ldq_stg->valid_a_h(j) && !(MXX_uint64)m_ldq_stg->retired_a_h(j)) &&
          ((((MXX_uint64)(Signal)m_ldq_stg->inum_a_h[i] - (MXX_uint64)(Signal)m_ldq_stg->inum_a_h[j]) & 0x80) == (MXX_uint64)0)) {
          BehErr << "LDQ entries not in ascending inum order" << mxxerror("X_ERROR");
          MxxOut << "LDQ[" << i << "] inum#" << hex << m_ldq_stg->inum_a_h[i] << endl;
          MxxOut << "LDQ[" << j << "] inum#" << hex << m_ldq_stg->inum_a_h[j] << endl;
      }
      i = j;
  }
  
#endif  // not CBOX_DEBUG
}
  
void verif_utils__beh::check_autosave()
{
    char cmd[256];

    int currentCycle;
    currentCycle = MXX_TOP->my_cycle();
    const char *currentCyclePhase;
    currentCyclePhase = MXX_TOP->my_cycle_phase();
    
    if (((int)auto_save_enable!=0) && ((int)k->clk==1) &&
        (currentCycle>=(auto_save_last_time + auto_save_period))) {
        MxxOut << "%I-auto_save{" << currentCyclePhase << "}: Saving model... " << endl;

        if (auto_save_index==auto_save_limit ||
            auto_save_index>=50)
            auto_save_index = 0;
        if (auto_save_times[auto_save_index] != 0) {
            sprintf(cmd, "%s_%d.nsv",auto_save_base_name,auto_save_times[auto_save_index]);
            remove(cmd);
        }
        auto_save_times[auto_save_index] = currentCycle;
        sprintf(cmd, "save %s_%d",auto_save_base_name,auto_save_times[auto_save_index]);
        MXX_command(cmd);
        auto_save_last_time = currentCycle;
        ++auto_save_index;      
    }
}    

/***********************************************************************
//!
//!DESCRIPTION
//!
//! The verif_utils__serv::gen_stats() function will do the following task:
//!
//! 1). open a ascii file whose name is passed in 
//! 2). write into the file all intesting stats
//! 3). if testname is passed in, a second cycle stat token will be created
//!     using the testname.
//! 4). close file
//!
//!
//!RETURN VALUE:
//!
//! none
//!
//!SPECIAL NOTES:
//!
//! The first one just calls the second if only one arg passed in.
//! MERLINXX doesn't let callable function have a default arg value.
//!
*/
void verif_utils__beh::gen_stats(const char *filename)
{
    gen_stats(filename, NULL);
}
void verif_utils__beh::gen_stats(const char *filename, const char *testname)
{
    double sysclk_ratio;
    int currentCycle;
    currentCycle = MXX_TOP->my_cycle();
    
    /*
    ** open the file
    */
    ofstream file(filename);
    if (!file) { // couldn't open it
        MxxOut << "%E-gen_stats: Couldn't open stats token file " << filename << endl;
        return;
    }

    /*
    ** spit out statistics
    */
    
    // first up is the cycle count
    // if testname given then do that cycle token too
    file << "overall_cycle=" << currentCycle << endl;
    if (testname)
        file << testname << "_cycle=" << currentCycle << endl;

    if (g->pass1_h & g->disable_fp_h) {
        file << "cpu_pass__1.0" << endl;
    }
    else if (g->pass2_h) {
        file << "cpu_pass__2.0" << endl;
    }
    else {
        file << "cpu_pass__3.0" << endl;
    }

    // Now do CBOX config stats
    switch (c_reg_csr->sys_clk_ratio_a) {
    case    0x2:
        file << "sysclk_ratio__1.5" << endl;
        sysclk_ratio = 1.5;
        break;                        
    case    0x4:
        file << "sysclk_ratio__2.0" << endl;
        sysclk_ratio = 2.0;
        break;                        
    case    0x8:
        file << "sysclk_ratio__2.5" << endl;
        sysclk_ratio = 2.5;
        break;                        
    case   0x10:
        file << "sysclk_ratio__3.0" << endl;
        sysclk_ratio = 3.0;
        break;                        
    case   0x20:
        file << "sysclk_ratio__3.5" << endl;
        sysclk_ratio = 3.5;
        break;                        
    case   0x40:
        file << "sysclk_ratio__4.0" << endl;
        sysclk_ratio = 4.0;
        break;                        
    case  0x100:
        file << "sysclk_ratio__5.0" << endl;
        sysclk_ratio = 5.0;
        break;                        
    case  0x400:
        file << "sysclk_ratio__6.0" << endl;
        sysclk_ratio = 6.0;
        break;                        
    case 0x1000:
        file << "sysclk_ratio__7.0" << endl;
        sysclk_ratio = 7.0;
        break;                        
    case 0x4000: file << "sysclk_ratio__8.0" << endl;
        sysclk_ratio = 8.0;
        break;
    default:
       MxxOut << "%E-gen_stats: Unkown sysclk ratio used. Value was " << c_reg_csr->sys_clk_ratio_a  << endl; 
    }
    if (c_reg_csr->bc_enable_a) {
        file << "bcache__on" << endl;
        switch (c_reg_csr->bc_clk_ratio_a) {
        case    0x2: file << "bcclk_ratio__1.5" << endl;
            file << "bc_latency__" <<
                ((c_reg_csr->bc_rd_wr_bubbles_a - (MXX_uint64)9 + (c_reg_csr->bc_late_write_num_a * MXX_uint64(3) / MXX_uint64(2)))) << endl;
            break;                     
        case    0x4: file << "bcclk_ratio__2.0" << endl;
            file << "bc_latency__" <<
                ((c_reg_csr->bc_rd_wr_bubbles_a - (MXX_uint64)11 + (c_reg_csr->bc_late_write_num_a * MXX_uint64(2)))) << endl;
            break;                     
        case    0x8: file << "bcclk_ratio__2.5" << endl;
            file << "bc_latency__" <<
                ((c_reg_csr->bc_rd_wr_bubbles_a - (MXX_uint64)13 + (c_reg_csr->bc_late_write_num_a * MXX_uint64(5) / MXX_uint64(2)))) << endl;
            break;                     
        case   0x10: file << "bcclk_ratio__3.0" << endl;
            file << "bc_latency__" <<
                ((c_reg_csr->bc_rd_wr_bubbles_a - (MXX_uint64)15 + (c_reg_csr->bc_late_write_num_a * MXX_uint64(3)))) << endl;
            break;                     
        case   0x20: file << "bcclk_ratio__3.5" << endl;
            file << "bc_latency__" <<
                ((c_reg_csr->bc_rd_wr_bubbles_a - (MXX_uint64)17 + (c_reg_csr->bc_late_write_num_a * MXX_uint64(7) / MXX_uint64(2)))) << endl;
            break;
        case   0x40: file << "bcclk_ratio__4.0" << endl;
            file << "bc_latency__" <<
                ((c_reg_csr->bc_rd_wr_bubbles_a - (MXX_uint64)19 + (c_reg_csr->bc_late_write_num_a * MXX_uint64(4)))) << endl;
            break;
        case   0x100: file << "bcclk_ratio__5.0" << endl;
            file << "bc_latency__" <<
                ((c_reg_csr->bc_rd_wr_bubbles_a - (MXX_uint64)23 + (c_reg_csr->bc_late_write_num_a * MXX_uint64(4)))) << endl;
            break;
        case   0x400: file << "bcclk_ratio__6.0" << endl;
            file << "bc_latency__" <<
                ((c_reg_csr->bc_rd_wr_bubbles_a - (MXX_uint64)27 + (c_reg_csr->bc_late_write_num_a * MXX_uint64(4)))) << endl;
            break;
        case   0x1000: file << "bcclk_ratio__7.0" << endl;
            file << "bc_latency__" <<
                ((c_reg_csr->bc_rd_wr_bubbles_a - (MXX_uint64)31 + (c_reg_csr->bc_late_write_num_a * MXX_uint64(4)))) << endl;
            break;
        case   0x4000: file << "bcclk_ratio__8.0" << endl;
            file << "bc_latency__" <<
                ((c_reg_csr->bc_rd_wr_bubbles_a - (MXX_uint64)35 + (c_reg_csr->bc_late_write_num_a * MXX_uint64(4)))) << endl;
            break;
        default:
            MxxOut << "%E-gen_stats: Unkown bcclk ratio used. Value was " << c_reg_csr->bc_clk_ratio_a  << endl; 
        }
        switch (c_reg_csr->bc_size_a) {
        case 0x0: file << "bc_size__1M" << endl;
            break;                     
        case 0x1: file << "bc_size__2M" << endl;
            break;                     
        case 0x3: file << "bc_size__4M" << endl;
            break;                     
        case 0x7: file << "bc_size__8M" << endl;
            break;                     
        case 0xf: file << "bc_size__16M" << endl;
            break;
        default:
            MxxOut << "%E-gen_stats: Unkown bc size used. Value was " << c_reg_csr->bc_size_a  << endl; 
        }
        file << "bcburst_mode__" << (c_reg_csr->bc_burst_mode_enable_a ? "on" : "off") << endl;
        file << "bc_late_write_num__" << c_reg_csr->bc_late_write_num_a << endl;
        file << "bc_rd_wr_bubbles__" << c_reg_csr->bc_rd_wr_bubbles_a << endl;
        file << "bc_rd_rd_bubbles__" << (unsigned int)c_reg_csr->bc_rd_rd_bubbles_a << endl;
        file << "bc_clk_delay__" << (unsigned int)c_reg_csr->bc_clk_delay_a << endl;
        file << "bc_cpu_late_write_num__" << (unsigned int)c_reg_csr->bc_cpu_late_write_num_a << endl;
        file << "bc_cpu_clk_delay__" << (unsigned int)c_reg_csr->bc_cpu_clk_delay_a << endl;
        file << "bc_cf_double_clk__" << (c_reg_csr->bc_cf_double_clk_a ? "on" : "off") << endl;
        file << "bc_clkfwd_enable__" << (c_reg_csr->bc_clkfwd_enable_a ? "on" : "off") << endl;
        file << "bc_ddm_fall_en__" << (c_reg_csr->bc_ddm_fall_en_a ? "on" : "off") << endl;
        file << "bc_ddm_rise_en__" << (c_reg_csr->bc_ddm_rise_en_a ? "on" : "off") << endl;
        file << "bc_ddmf_enable__" << (c_reg_csr->bc_ddmf_enable_a ? "on" : "off") << endl;
        file << "bc_ddmr_enable__" << (c_reg_csr->bc_ddmr_enable_a ? "on" : "off") << endl;
        file << "bc_ddm_rd_fall_en__" << (c_reg_csr->bc_ddm_rd_fall_en_a ? "on" : "off") << endl;
        file << "bc_ddm_rd_rise_en__" << (c_reg_csr->bc_ddm_rd_rise_en_a ? "on" : "off") << endl;
        file << "bc_tag_ddm_fall_en__" << (c_reg_csr->bc_tag_ddm_fall_en_a ? "on" : "off") << endl;
        file << "bc_tag_ddm_rise_en__" << (c_reg_csr->bc_tag_ddm_rise_en_a ? "on" : "off") << endl;
        file << "bc_tag_ddm_rd_fall_en__" << (c_reg_csr->bc_tag_ddm_rd_fall_en_a ? "on" : "off") << endl;
        file << "bc_tag_ddm_rd_rise_en__" << (c_reg_csr->bc_tag_ddm_rd_rise_en_a ? "on" : "off") << endl;
        file << "bc_pentium_mode__" << (c_reg_csr->bc_pentium_mode_a ? "on" : "off") << endl;
    }
    else
        file << "bcache__off" << endl;
    file << "bc_clean_victim__" << (c_reg_csr->bc_clean_victim_a ? "on" : "off") << endl;
    file << "bc_read_victim__" << (c_reg_csr->bc_rdvictim_a ? "on" : "off") << endl;
    file << "zeroblk_enable__" << (unsigned int)c_reg_csr->zeroblk_enable_a << endl;
    file << "set_dirty_enable__" << (unsigned int)c_reg_csr->set_dirty_enable_a << endl;
    switch (c_reg_csr->frame_select_a) {
    case 0x0: file << "sys_cmds_on__any" << endl;
        break;                     
    case 0x1: file << "sys_cmds_on__even" << endl;
        break;                     
    case 0x2: file << "sys_cmds_on__quad" << endl;
        break;                     
    default:
        MxxOut << "%E-gen_stats: Unkown add frame select used. Value was " << c_reg_csr->frame_select_a  << endl; 
    }
    file << "dcvic_thresh__" << c_reg_csr->dcvic_threshold_a  << endl;
    file << "sysbus_ack_limit__" << (unsigned int)c_reg_csr->sysbus_ack_limit_a << endl;
    file << "sysbus_vic_limit__" << (unsigned int)c_reg_csr->sysbus_vic_limit_a << endl;
    file << "dup_tag_enable__" << (c_reg_csr->dup_tag_enable_a ? "on" : "off") << endl;
    file << "speculative_rd_enable__" << (c_reg_csr->spec_read_enable_a ? "on" : "off") << endl;
    file << "sysbus_format__" << c_reg_csr->sysbus_format_a << endl;
    file << "mb_enable__" << (c_reg_csr->sysbus_mb_enable_a ? "on" : "off") << endl;
    file << "enable_evict__" << (c_reg_csr->enable_evict_a ? "on" : "off") << endl;
    file << "prb_tag_only__" << (c_reg_csr->prb_tag_only_a ? "on" : "off") << endl;
    file << "tlaser_stio_mode__" << (c_reg_csr->tlaser_stio_mode_a ? "on" : "off") << endl;
    file << "fast_mode_disable__" << (c_reg_csr->fast_mode_disable_a ? "on" : "off") << endl;
    file << "rdvic_ack_inhibit__" << (c_reg_csr->rdvic_ack_inhibit_a ? "on" : "off") << endl;
    file << "enable_stc_command__" << (c_reg_csr->enable_stc_command_a ? "on" : "off") << endl;
    file << "bc_frm_clk__" << (c_reg_csr->bc_frm_clk_a ? "on" : "off") << endl;
    file << "sys_cf_double_clk__" << (c_reg_csr->sys_cf_double_clk_a ? "on" : "off") << endl;
    file << "sys_ddm_fall_en__" << (c_reg_csr->sys_ddm_fall_en_a ? "on" : "off") << endl;
    file << "sys_ddm_rise_en__" << (c_reg_csr->sys_ddm_rise_en_a ? "on" : "off") << endl;
    file << "sys_ddmf_enable__" << (c_reg_csr->sys_ddmf_enable_a ? "on" : "off") << endl;
    file << "sys_ddmr_enable__" << (c_reg_csr->sys_ddmr_enable_a ? "on" : "off") << endl;
    file << "sys_ddm_rd_fall_en__" << (c_reg_csr->sys_ddm_rd_fall_en_a ? "on" : "off") << endl;
    file << "sys_ddm_rd_rise_en__" << (c_reg_csr->sys_ddm_rd_rise_en_a ? "on" : "off") << endl;

    if (c_reg_csr->sysbus_format_a) {
        switch (c_reg_csr->sys_bus_size_a) {
        case 0: file << "pa_msb__38" << endl; break;
        case 1: file << "pa_msb__36" << endl; break;
        case 3: file << "pa_msb__34" << endl; break;
        default: file << "pa_msb__illegal" << endl; break;
        }
    }
    else {
        switch (c_reg_csr->sys_bus_size_a) {
        case 0: file << "pa_msb__42" << endl; break;
        case 1: file << "pa_msb__36" << endl; break;
        case 3: file << "pa_msb__34" << endl; break;
        default: file << "pa_msb__illegal" << endl; break;
        }
    }

    file << "cfr_frmclk_delay__" << (unsigned int)c_reg_csr->cfr_frmclk_delay_a << endl;
    file << "cfr_ev6clk_delay__" << (unsigned int)c_reg_csr->cfr_ev6clk_delay_a << endl;
    file << "cfr_gclk_delay__" << (unsigned int)c_reg_csr->cfr_gclk_delay_a << endl;
    file << "sys_clk_delay__" << (unsigned int)c_reg_csr->sys_clk_delay_a << endl;
    file << "sys_cpu_clk_delay__" << (unsigned int)c_reg_csr->sys_cpu_clk_delay_a << endl;
    file << "sysdc_delay__" << (unsigned int)c_reg_csr->sysdc_delay_a << endl;
    file << "sys_rcv_mux_cnt_preset__" << (unsigned int)c_reg_csr->sys_rcv_mux_cnt_preset_a << endl;
    file << "bc_rcv_mux_cnt_preset__" << (unsigned int)c_reg_csr->bc_rcv_mux_cnt_preset_a << endl;
    file << "frame_clock_offset__" << (unsigned int)c_reg_csr->frame_clock_offset_a << endl;
    file << "data_valid_dly__" << (unsigned int)c_reg_csr->data_valid_dly_a << endl;
    file << "jitter_cmd__" << (unsigned int)c_reg_csr->jitter_cmd_a << endl;

    file << "dc0_rd_hits=" << x_stats->num_of_dc0_hits << endl;
    file << "dc0_rd_misses=" << x_stats->num_of_dc0_misses << endl;
    file << "dc1_rd_hits=" << x_stats->num_of_dc1_hits << endl;
    file << "dc1_rd_misses=" << x_stats->num_of_dc1_misses << endl;
    file << "dc_synonyms=" << x_stats->num_of_synonyms << endl;

    file << "bc_rd_hits=" << x_stats->num_of_bc_hits << endl;
    file << "bc_rd_misses=" << (x_stats->num_of_bc_reads - x_stats->num_of_bc_hits) << endl;
    file << "bc_writes=" << x_stats->num_of_bc_writes << endl;
    file << "bc_subsets=" << x_stats->num_of_subsets << endl;

    file << "num_of_rd_blk=" << x_stats->num_of_rd_blk << endl;
    file << "num_of_rd_blk_spec=" << x_stats->num_of_rd_blk_spec << endl;
    file << "num_of_rd_blk_spec_fail=" << x_stats->num_of_rd_blk_spec_fail << endl;
    file << "num_of_rd_blk_vic=" << x_stats->num_of_rd_blk_vic << endl;
    file << "num_of_rd_blk_mod=" << x_stats->num_of_rd_blk_mod << endl;
    file << "num_of_rd_blk_mod_spec=" << x_stats->num_of_rd_blk_mod_spec << endl;
    file << "num_of_rd_blk_mod_spec_fail=" << x_stats->num_of_rd_blk_mod_spec_fail << endl;
    file << "num_of_rd_blk_mod_vic=" << x_stats->num_of_rd_blk_mod_vic << endl;
    file << "num_of_fetch_blk=" << x_stats->num_of_fetch_blk << endl;
    file << "num_of_fetch_blk_spec=" << x_stats->num_of_fetch_blk_spec << endl;
    file << "num_of_fetch_blk_spec_fail=" << x_stats->num_of_fetch_blk_spec_fail << endl;
    file << "num_of_evict=" << x_stats->num_of_evict << endl;
    file << "num_of_wr_vic_blk=" << x_stats->num_of_wr_vic_blk << endl;
    file << "num_of_clean_vic_blk=" << x_stats->num_of_clean_vic_blk << endl;
    file << "num_of_io_rd_byte=" << x_stats->num_of_io_rd_byte << endl;
    file << "num_of_io_rd_lw=" << x_stats->num_of_io_rd_lw << endl;
    file << "num_of_io_rd_qw=" << x_stats->num_of_io_rd_qw << endl;
    file << "num_of_io_wr_byte=" << x_stats->num_of_io_wr_byte << endl;
    file << "num_of_io_wr_lw=" << x_stats->num_of_io_wr_lw << endl;
    file << "num_of_io_wr_qw=" << x_stats->num_of_io_wr_qw << endl;
    file << "num_of_mb=" << x_stats->num_of_mb << endl;
    file << "num_of_set_dirty_clean=" << x_stats->num_of_set_dirty_clean << endl;
    file << "num_of_set_dirty_shared=" << x_stats->num_of_set_dirty_shared << endl;
    file << "num_of_stc_set_dirty=" << x_stats->num_of_stc_set_dirty << endl;
    file << "num_of_zero_blk=" << x_stats->num_of_zero_blk << endl;
    file << "num_of_zero_blk_vic=" << x_stats->num_of_zero_blk_vic << endl;
    file << "num_of_istream=" << x_stats->num_of_istream << endl;
    file << "num_of_istream_spec=" << x_stats->num_of_istream_spec << endl;
    file << "num_of_istream_spec_fail=" << x_stats->num_of_istream_spec_fail << endl;
    file << "num_of_istream_vic=" << x_stats->num_of_istream_vic << endl;
    file << "num_of_prb_resp=" << x_stats->num_of_prb_resp << endl;
    file << "num_of_vdb_flush_req=" << x_stats->num_of_vdb_flush_req << endl;

    file << "num_of_io_rd_merge=" << x_stats->num_of_io_rd_merge << endl;
    file << "num_of_io_wr_merge=" << x_stats->num_of_io_wr_merge << endl;

    file << "probe_hits=" << x_stats->num_of_probe_hit << endl;
    file << "probe_misses=" << x_stats->num_of_probe_miss << endl;

    file << "num_of_nxm_rdblk=" << x_stats->num_of_nxm_rdblk << endl;
    file << "num_of_nxm_rdblkmod=" << x_stats->num_of_nxm_rdblkmod << endl;
    file << "num_of_nxm_rdio=" << x_stats->num_of_nxm_rdio << endl;
    file << "num_of_nxm_istrm=" << x_stats->num_of_nxm_istrm << endl;
    file << "num_of_nxm_fetch=" << x_stats->num_of_nxm_fetch << endl;

    file << "internal_nxm_count=" << x_stats->internal_nxm_count << endl;

    // Performance Count Dump
  file << "Instructions_(R)="<< x_stats->retired_instr<<endl;
  int issued_int_instr = x_stats->issued_l0_instr + x_stats->issued_l1_instr +
                         x_stats->issued_u0_instr + x_stats->issued_u1_instr;
  int issued_flt_instr = x_stats->issued_f0_instr + x_stats->issued_f1_instr;
  file << "Instructions_(I)="<< (issued_int_instr + issued_flt_instr) <<endl;
  file << "Int_Instructions_(I)="<< issued_int_instr <<endl;
  file << "Flt_Instructions_(I)="<< issued_flt_instr <<endl;
  file << "L0_Instructions_(I)="<< x_stats->issued_l0_instr<<endl;
  file << "L1_Instructions_(I)="<< x_stats->issued_l1_instr<<endl;
  file << "U0_Instructions_(I)="<< x_stats->issued_u0_instr<<endl;
  file << "U1_Instructions_(I)="<< x_stats->issued_u1_instr<<endl;
  file << "F0_Instructions_(I)="<< x_stats->issued_f0_instr<<endl;
  file << "F1_Instructions_(I)="<< x_stats->issued_f1_instr<<endl;

  // x_stats->retired_fp_instr comes from spot.
  file << "FP_Instructions_(R)="<< x_stats->retired_fp_instr<<endl;
  file << "ICycles="<< x_stats->cycles<<endl;
  file << "All_Traps="<< x_stats->traps<<endl;
  
  file << "CBR_Mispr_(R)="<< x_stats->retired_br_mispr       <<endl;
  file << "CBR_(ghist)_(R)="<< x_stats->retired_ghist_mispr    <<endl;
  file << "CBR_(lhist)_(R)="<< x_stats->retired_lhist_mispr    <<endl;
  file << "Retired_CBRs_(R)="<< x_stats->retired_cbr            <<endl;
  file << "Chose_Ghist_(R)="<< x_stats->retired_choose_ghist   <<endl;
  file << "Chose_LHIST_(R)="<< x_stats->retired_choose_lhist   <<endl;
  file << "Chose_Wrong_(R)="<< x_stats->retired_choose_wrong   <<endl;
  file << "Mispr_Traps="<< x_stats->unretired_br_mispr     <<endl;
  file << "train_ghist_cycles="<< x_stats->train_ghist      <<endl;

  file << "Load-Store_Order="<< x_stats->ld_st_order_trap      <<endl;
  file << "Load-Load_Order="<< x_stats->ld_ld_order_trap      <<endl;
  file << "Load_Q_Full="<< x_stats->ldq_full_trap         <<endl;
  file << "Litmus="<< x_stats->litmus_trap           <<endl;
  file << "Load_Q_Troll="<< x_stats->ldq_troll_trap        <<endl;
  file << "Load-STxC="<< x_stats->ldq_stxc_trap         <<endl;
  file << "Simul_Order="<< x_stats->simul_order_trap      <<endl;
  file << "Wrong_ST_Data="<< x_stats->ld_wrong_stdat_trap   <<endl;
  file << "Wrong_ST_Size="<< x_stats->ld_wrong_stsiz_trap   <<endl;
  file << "DC_Load-Store="<< x_stats->dc_ld_st_trap         <<endl;
  file << "Store_Q_Troll="<< x_stats->troll_st_trap         <<endl;
  file << "STORE_Q_Full="<< x_stats->stq_full_trap         <<endl;
  file << "Simul_Troll="<< x_stats->simul_troll_trap      <<endl;
  file << "Simul_DC_LD_ST="<< x_stats->simul_dc_ld_st_trap   <<endl;

  file << "DTB_Dbl_(R)="    << x_stats->dtb_miss_double        <<endl;
  file << "FEN_(R)="        << x_stats->fen_retired_trap       <<endl;
  file << "Unaligned_(R)="  << x_stats->unaligned_retired_trap <<endl;
  file << "DTB_Single_(R)=" << x_stats->dtb_miss_single        <<endl;
  file << "DFault_(R)="     << x_stats->dfault_retired_trap    <<endl;
  file << "OPCDEC_(R)="     << x_stats->opcdec_retired_trap    <<endl;
  file << "IACV_(R)="       << x_stats->iacv_retired_trap      <<endl;
  file << "MCHK_(R)="       << x_stats->mchk_retired_trap      <<endl;
  file << "ITB_Miss_(R)="   << x_stats->itb_miss               <<endl;
  file << "Arith_(R)="      << x_stats->arith_retired_trap     <<endl;
  file << "Interrupt_(R)="  << x_stats->interrupt_retired_trap <<endl;
  file << "MT_FPCR_(R)="    << x_stats->mt_fpcr_retired_trap   <<endl;
  file << "Reset_(R)="      << x_stats->reset_retired_trap     <<endl;

  file << "MAF_Full="<< x_stats->maf_full_retry         <<endl;

  file << "JSR_Mispr="<< x_stats->jsr_mispr              <<endl;
  file << "JSR_Call="<< x_stats->jsr_mispr_call         <<endl;
  file << "JSR_Jump="<< x_stats->jsr_mispr_jmp          <<endl;
  file << "JSR_Ret="<< x_stats->jsr_mispr_ret          <<endl;

  file << "Fetcher_Stall="<< x_stats->frontend_stall         <<endl;
  file << "IC_Miss_Cycles="<< x_stats->ic_fill_stall          <<endl;
  file << "Fetch_Inval_Cycles="<< x_stats->ic_fetch               <<endl;
  file << "Line-code_Mispr="<< x_stats->line_code_mispr        <<endl;
  file << "Line-addr_Mispr="<< x_stats->line_addr_mispr        <<endl;
  file << "Fill_under_stall="<< x_stats->fill_under_stall_bbl   <<endl;
  file << "Set_Mispr="<< x_stats->set_mispr              <<endl;
  file << "Fill_Bank_Confl="<< x_stats->fill_bank_confl_bbl      <<endl;
  file << "CMOV_Stall="<< x_stats->cmov_stall             <<endl;

  file << "BackEnd_Stall="<< x_stats->backend_stall          <<endl;
  file << "INT_Map_Stall="<< x_stats->int_map_stall          <<endl;
  file << "FP_Map_Stall="<< x_stats->fp_map_stall           <<endl;
  file << "Inum_Stall="<< x_stats->inum_stall             <<endl;
  file << "Stop_Retire="<< x_stats->stop_retire             <<endl;
  file << "Mbx_Stop_Retire="<< x_stats->mbx_stop_retire             <<endl;
  file << "Mbx_Stop_Overload="<< x_stats->mbx_stop_overload             <<endl;
  file << "IQ_Full_Stall="<< x_stats->iq_full_stall          <<endl;
  file << "IPR_Stall="<< x_stats->ipr_q_stall            <<endl;
  file << "FQ_Full_Stall="<< x_stats->fq_full_stall          <<endl;

  file << "num_of_Single_Issue_cycles="<< x_stats->num_of_single_issue          <<endl;

  file << "stxc_failure="<< x_stats->stxc_failure << endl;
  file << "stxc_success="<< x_stats->stxc_success << endl;
  file << "maf_full_stxc_replay_trap="<< x_stats->maf_full_stxc_replay_trap << endl;
  file << "stxc_fail_due_to_evict="<< x_stats->stxc_fail_due_to_evict << endl;

  file << "total_loads="<< x_stats->total_loads << endl;
  file << "io_loads="<< x_stats->io_loads << endl;
  file << "total_stores="<< x_stats->total_stores << endl;
  file << "io_stores="<< x_stats->io_stores << endl;

  if (currentCycle != 0) {
      file << "ldq_ave_size="<< ((double)(x_stats->ldq_size) / (double)currentCycle) << endl;
      file << "stq_ave_size="<< ((double)(x_stats->stq_size) / (double)currentCycle) << endl;
      file << "maf_dstream_ave_size="<< ((double)(x_stats->maf_dstream_size) / (double)currentCycle) << endl;
      file << "maf_istream_ave_size="<< ((double)(x_stats->maf_istream_size) / (double)currentCycle) << endl;
      
  }
  
  if (verif_utils->deadlock_stats_enable) {
      for (int x = 0; x <= 5; x++) {
          for (int y = 0; y <= 3; y++) {
              file << "ecb_match_"<< x << "_" << y << "=" << verif_utils->ecb_match[x][y] << endl;
              file << "wh64_match_"<< x << "_" << y << "=" << verif_utils->wh64_match[x][y] << endl;
              file << "pre_evn_match_"<< x << "_" << y << "=" << verif_utils->pre_evn_match[x][y] << endl;
              file << "pre_mod_match_"<< x << "_" << y << "=" << verif_utils->pre_mod_match[x][y] << endl;
              file << "pre_nocache_match_"<< x << "_" << y << "=" << verif_utils->pre_nocache_match[x][y] << endl;
              file << "pre_norm_match_"<< x << "_" << y << "=" << verif_utils->pre_norm_match[x][y] << endl;
          }
      }
      file << "prb_ecb_before=" << verif_utils->prb_ecb_before << endl;
      file << "prb_wh64_before=" << verif_utils->prb_wh64_before << endl;
      file << "prb_pre_evn_before=" << verif_utils->prb_pre_evn_before << endl;
      file << "prb_pre_mod_before=" << verif_utils->prb_pre_mod_before << endl;
      file << "prb_pre_nocache_before=" << verif_utils->prb_pre_nocache_before << endl;
      file << "prb_pre_norm_before=" << verif_utils->prb_pre_norm_before << endl;
      
      file << "prb_ecb_after=" << verif_utils->prb_ecb_after << endl;
      file << "prb_wh64_after=" << verif_utils->prb_wh64_after << endl;
      file << "prb_pre_evn_after=" << verif_utils->prb_pre_evn_after << endl;
      file << "prb_pre_mod_after=" << verif_utils->prb_pre_mod_after << endl;
      file << "prb_pre_nocache_after=" << verif_utils->prb_pre_nocache_after << endl;
      file << "prb_pre_norm_after=" << verif_utils->prb_pre_norm_after << endl;
  }
      
    // That's all folks
    return;
}

Signal *verif_utils__beh::rand_value()
{
  static Signal data(511,0);
  data.insert(63,0,	 the_xbox_p->getRand()->getRandomR(make64_bit(0xffffffff,0xffffffff),(MTV_uint64)0));
  data.insert(127,64,	 the_xbox_p->getRand()->getRandomR(make64_bit(0xffffffff,0xffffffff),(MTV_uint64)0));
  data.insert(191,128,	 the_xbox_p->getRand()->getRandomR(make64_bit(0xffffffff,0xffffffff),(MTV_uint64)0));
  data.insert(255,192,	 the_xbox_p->getRand()->getRandomR(make64_bit(0xffffffff,0xffffffff),(MTV_uint64)0));
  data.insert(319,256,	 the_xbox_p->getRand()->getRandomR(make64_bit(0xffffffff,0xffffffff),(MTV_uint64)0));
  data.insert(383,320,	 the_xbox_p->getRand()->getRandomR(make64_bit(0xffffffff,0xffffffff),(MTV_uint64)0));
  data.insert(447,384,	 the_xbox_p->getRand()->getRandomR(make64_bit(0xffffffff,0xffffffff),(MTV_uint64)0));
  data.insert(511,448,	 the_xbox_p->getRand()->getRandomR(make64_bit(0xffffffff,0xffffffff),(MTV_uint64)0));
  return(&data);
}

void verif_utils__beh::set_xbox_seed(const unsigned *seed)
{
  the_xbox_p->getRand()->setSeed(*seed);
}

int verif_utils__beh::rand_percent(const unsigned *percent)
{
  MTV_uint64 max_val = (MTV_uint64)(*percent);
  MTV_uint64 rnd_val = the_xbox_p->getRand()->getRandomR((MTV_uint64)99,(MTV_uint64)0);
  if (rnd_val < max_val)
    return(1);
  else
    return(0);
}

int verif_utils__beh::ef_stop_at_cycle_phase(const char *string)
{
    const char *currentCyclePhase;
    currentCyclePhase = MXX_TOP->my_cycle_phase();
    
  const char *curr_cycle = currentCyclePhase;
  int j = strlen(curr_cycle) - 1;
  int i = strlen(string) - 1;
  if ((string[i] != 'a') & (string[i] != 'A') & (string[i] != 'b') & (string[i] != 'B'))
    --j;

  // need to compare what is passed in
  // This way if current cycle was 175209
  // and user typed cmd>su 20
  // we stop at 175220
  for(; i >= 0; --i,--j) 
    if (string[i]!=curr_cycle[j])
      return(0);
  return(1);
}

#ifdef CBOX_DEBUG
void verif_utils__beh::load_osf_aout(const char*){}
#endif // CBOX_DEBUG

#ifndef CBOX_DEBUG
#ifdef __osf__
static void copy_aout_file(long ptr, long address, long length, FILE *coff, isp_memory_t* isp_memory_p, main_memory_t* main_memory_p);
#define ENTRY_POINT_ADDRESS 0x7FFFFFF0000
#endif

void verif_utils__beh::load_osf_aout(const char *filename)
{
#ifdef __osf__
FILE *coff;
struct filehdr filehdr;
AOUTHDR aouthdr;
struct scnhdr scnhdr[100];
int i;
isp_memory_t* isp_memory_p = mtvcli->getISP_Memory();
main_memory_t* main_memory_p = mtvcli->getMainMemory();

if((coff = fopen(filename, "r")) != NULL)
{
	fread((void *) &filehdr, sizeof(struct filehdr), 1, coff);
	fread((void *) &aouthdr, sizeof(AOUTHDR), 1, coff);
}

if(coff != NULL && filehdr.f_nscns <= 100)
{
	MxxOut << "loading a.out " << filename << " 0x" << hex << filehdr.f_magic << " 0x" << aouthdr.magic
		<< " (" << filehdr.f_opthdr << "=" << sizeof(AOUTHDR)
		<< ") flags 0x" << filehdr.f_flags
		<< " text 0x" << aouthdr.text_start << "(0x" << aouthdr.tsize
		<< ") data 0x" << aouthdr.data_start << "(0x" << aouthdr.dsize
		<< ") bss 0x" << aouthdr.bss_start << "(0x" << aouthdr.bsize
		<< ") entry 0x" << aouthdr.entry
		<< " gp 0x" << aouthdr.gp_value
		<< endl;
	fread((void *) scnhdr, sizeof(struct scnhdr), filehdr.f_nscns, coff);
	for(i = 0; i < filehdr.f_nscns; i++)
	{
	   if(scnhdr[i].s_flags != STYP_COMMENT)
	   {
		MxxOut << " loading section " << scnhdr[i].s_name << " (0x" << scnhdr[i].s_flags
			<< ") size 0x" << scnhdr[i].s_size << "(" << dec << scnhdr[i].s_size
			<< ") from 0x" << hex << scnhdr[i].s_scnptr
			<< " to address 0x" << scnhdr[i].s_paddr << "(0x" << scnhdr[i].s_vaddr << ")" << endl;
		copy_aout_file(scnhdr[i].s_scnptr, scnhdr[i].s_paddr, scnhdr[i].s_size, coff, isp_memory_p, main_memory_p);
	   }
	}
	/* store the entry point address so we can jump there from the init code */
        /* put it at the hard-coded address ENTRY_POINT_ADDRESS */
	isp_memory_p->write_qw(ENTRY_POINT_ADDRESS, aouthdr.entry);
	main_memory_p->write_qw(ENTRY_POINT_ADDRESS, aouthdr.entry);
}
else
    MxxOut << "Couldn't open/handle " << filename << "!" << endl;

if(coff != NULL) fclose(coff);

#endif /* __osf__ */
}

#ifdef __osf__
static void copy_aout_file(long ptr, long address, long length, FILE *coff, isp_memory_t* isp_memory_p, main_memory_t* main_memory_p)
{
long qw;
int len, quad;

if(ptr != 0)
{
	/* read the data from the file into the memory locations */
	if(fseek(coff, ptr, SEEK_SET) != 0) fprintf(stderr, "Couldn't seek!\n");
}
else
	/* zero out the memory locations */
	qw = 0;

quad = ((address & 0x7) == 0) && ((length & 0x7) == 0);
if(quad)
	len = 8;
else
	len = 4;

while(length > 0)
{
	if(ptr != 0) fread((void *) &qw, len, 1, coff);
	if(quad) {
		isp_memory_p->write_qw(address, qw);
		main_memory_p->write_qw(address, qw);
	}
	else {
		isp_memory_p->write_lw(address, *((int *) &qw));
		main_memory_p->write_lw(address, *((int *) &qw));
	}
	address += len;
	length -= len;
}

}
#endif /* __osf__ */
#endif // not CBOX_DEBUG


void verif_utils__beh::dump_probe_miss_stats(const char *filename) {
    dump_probe_miss_stats(filename, NULL);
}
void verif_utils__beh::dump_probe_miss_stats(const char *filename, const int *verbose)
{
    /*
    ** open the file
    */
    ofstream file(filename, ios::out | ios::app);
    if (!file) { // couldn't open it
        MxxOut << "%E-dump_probe_miss_stats: Couldn't open file " << filename << endl;
        return;
    }

    /*
    ** dump the stats
    */
    for (int i=0; i<1024; i++) {
        if (verbose==NULL) {
            file << x_stats->probe_miss_latency[i] << " ";
        }
        else {
            if (i == 0)
                file << "*****************************************************\n";
            file << "latency=" << i << ", number seen=" << x_stats->probe_miss_latency[i] << ", first seen=" << x_stats->probe_miss_latency_start[i];
            file << "\n";
        }
    }
    file << "\n";
}


void verif_utils__beh::dump_probe_hit_stats(const char *filename) {
    dump_probe_hit_stats(filename, NULL);
}
void verif_utils__beh::dump_probe_hit_stats(const char *filename, const int *verbose)
{
    /*
    ** open the file
    */
    ofstream file(filename, ios::out | ios::app);
    if (!file) { // couldn't open it
        MxxOut << "%E-dump_probe_hit_stats: Couldn't open file " << filename << endl;
        return;
    }

    /*
    ** dump the stats
    */
    for (int i=0; i<1024; i++) {
        if (verbose==NULL) {
            file << x_stats->probe_hit_latency[i] << " ";
        }
        else {
            if (i == 0)
                file << "*****************************************************\n";
            file << "latency=" << i << ", number seen=" << x_stats->probe_hit_latency[i] << ", first seen=" << x_stats->probe_hit_latency_start[i];
            file << "\n";
        }
    }
    file << "\n";
}

void verif_utils__beh::set_cbox_config()
{
  
  int i, j;
  //	Backfeed into WRITE ONCE chain  (DXE environment only deposits to CSR directly)

  j = 0;
  // C_IOA_VLD
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->tlaser_stio_mode_a); }
  for( i=1  ; i < 2  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_clk_ratio_a(i)); }
  // C_VAF_VIC
  for( i=1  ; i < 2  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_clk_ratio_a(i)); }
  for( i=0  ; i < 8  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->dcvic_threshold_a(i)); }
  // C_ADC_BCT
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_clean_victim_a); }
  // C_ADC_SMX
  for( i=0  ; i < 2  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_bus_size_a(i)); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sysbus_format_a); }
  for( i=1  ; i < 5  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_clk_ratio_a(i)); }
  // C_PRQ_ACT
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->dup_tag_enable_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->prb_tag_only_a); }
  // C_SYS_ARO
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->fast_mode_disable_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_rdvictim_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_clean_victim_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->rdvic_ack_inhibit_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sysbus_mb_enable_a); }
  // C_SYS_CNT
  for( i=4  ; i >= 0 ; i--,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sysbus_ack_limit_a(i)); }
  for( i=2  ; i >= 0 ; i--,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sysbus_vic_limit_a(i)); }
  // C_ADC_TAG
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_clean_victim_a); }
  // C_BCC_ALC
  for( i=5  ; i >= 0 ; i--,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_rd_wr_bubbles_a(i)); }
  for( i=1  ; i >= 0 ; i--,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_rd_rd_bubbles_a(i)); }
  for( i=3  ; i >= 0 ; i--,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_wr_rd_bubbles_a(i)); }
  // C_ISB_CT6
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->dup_tag_enable_a); }  
  for( i=1  ; i < 2  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_clk_ratio_a(i)); }

  // C_ISB_CT4
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_rdvictim_a); }
  for( i=1  ; i < 2  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_clk_ratio_a(i)); }
  // C_ISB_CT3
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_rdvictim_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_clean_victim_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->dup_tag_enable_a); }
  // C_ISB_CT2
  for( i=1  ; i < 2  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_clk_ratio_a(i)); }
  // C_ISB_CT1
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->spec_read_enable_a); }
  for( i=1  ; i < 2  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_clk_ratio_a(i)); }
  // C_ISB_CT0
  for( i=1  ; i < 2  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_clk_ratio_a(i)); }
  // C_VAF_DCE/C_BCS_BMP
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->mbox_bc_prb_stall_a); }  
  for( i=31 ; i >= 0 ; i--,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_lat_data_pattern_a(i)); }
  // C_BCS_BMP
  for( i=23 ; i >= 0 ; i--,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_lat_tag_pattern_a(i)); }
  // C_CMF_CTN
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_rdvictim_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->enable_stc_command_a); }
  // C_BCW_BCS
  for( i=2  ; i >= 0 ; i--,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_late_write_num_a(i)); }
  for( i=1  ; i >= 0 ; i--,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_cpu_late_write_num_a(i)); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_burst_mode_enable_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_pentium_mode_a); }
  for( i=1  ; i < 2  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_clk_ratio_a(i)); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_frm_clk_a); }
  // C_BIX_PAD/C_BTG_PAD
  for( i=1  ; i >= 0 ; i--,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_clk_delay_a(i)); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_ddmr_enable_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_ddmf_enable_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_late_write_upper_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_tag_ddm_fall_en_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_tag_ddm_rise_en_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_clkfwd_enable_a); }
  for( i=1  ; i >= 0 ; i--,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_rcv_mux_cnt_preset_a(i)); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_tag_ddm_rd_fall_en_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_tag_ddm_rd_rise_en_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_late_write_upper_a); }
  // C_BNW_PAD/C_BNE_PAD
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_ddm_fall_en_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_ddm_rise_en_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_clkfwd_enable_a); }
  for( i=1  ; i >= 0 ; i--,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_rcv_mux_cnt_preset_a(i)); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_ddm_rd_fall_en_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_ddm_rd_rise_en_a); }
  for( i=1  ; i >= 0 ; i--,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_clk_delay_a(i)); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_ddmr_enable_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_ddmf_enable_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_ddm_fall_en_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_ddm_rise_en_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_clkfwd_enable_a); }
  for( i=1  ; i >= 0 ; i--,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_rcv_mux_cnt_preset_a(i)); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_ddm_rd_fall_en_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_ddm_rd_rise_en_a); }
  // C_BWN_PAD/C_BEN_PAD
  for( i=1  ; i >= 0 ; i--,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_clk_delay_a(i)); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_ddmr_enable_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_ddmf_enable_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_ddm_fall_en_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_ddm_rise_en_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_clkfwd_enable_a); }
  for( i=1  ; i >= 0 ; i--,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_rcv_mux_cnt_preset_a(i)); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_ddm_rd_fall_en_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_ddm_rd_rise_en_a); }
  for( i=1  ; i >= 0 ; i--,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_clk_delay_a(i)); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_ddmr_enable_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_ddmf_enable_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_ddm_fall_en_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_ddm_rise_en_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_clkfwd_enable_a); }
  for( i=1  ; i >= 0 ; i--,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_rcv_mux_cnt_preset_a(i)); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_ddm_rd_fall_en_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_ddm_rd_rise_en_a); }
  // C_BWS_PAD/C_BES_PAD
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_ddm_fall_en_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_ddm_rise_en_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_clkfwd_enable_a); }
  for( i=1  ; i >= 0 ; i--,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_rcv_mux_cnt_preset_a(i)); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_ddm_rd_fall_en_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_ddm_rd_rise_en_a); }
  for( i=1  ; i >= 0 ; i--,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_clk_delay_a(i)); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_ddmr_enable_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_ddmf_enable_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_ddm_fall_en_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_ddm_rise_en_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_clkfwd_enable_a); }
  for( i=1  ; i >= 0 ; i--,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_rcv_mux_cnt_preset_a(i)); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_ddm_rd_fall_en_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_ddm_rd_rise_en_a); }
  // C_BSW_PAD/C_BSE_PAD
  for( i=1  ; i >= 0 ; i--,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_clk_delay_a(i)); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_ddmr_enable_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_ddmf_enable_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_ddm_fall_en_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_ddm_rise_en_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_clkfwd_enable_a); }
  for( i=1  ; i >= 0 ; i--,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_rcv_mux_cnt_preset_a(i)); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_ddm_rd_fall_en_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_ddm_rd_rise_en_a); }
  for( i=1  ; i >= 0 ; i--,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_clk_delay_a(i)); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_ddmr_enable_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_ddmf_enable_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_ddm_fall_en_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_ddm_rise_en_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_clkfwd_enable_a); }
  for( i=1  ; i >= 0 ; i--,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_rcv_mux_cnt_preset_a(i)); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_ddm_rd_fall_en_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_ddm_rd_rise_en_a); }
  // C_BSO_PAD
  for( i=1  ; i >= 0 ; i--,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_clk_delay_a(i)); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_ddmr_enable_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_ddmf_enable_a); }
  // C_BSI_PAD
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_ddm_fall_en_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_ddm_rise_en_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_clkfwd_enable_a); }
  for( i=1  ; i >= 0 ; i--,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_rcv_mux_cnt_preset_a(i)); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_ddm_rd_fall_en_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_ddm_rd_rise_en_a); }
  // C_BRS_CFR
  for( i=2  ; i >= 0 ; i--,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->cfr_gclk_delay_a(i)); }
  for( i=2  ; i >= 0 ; i--,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->cfr_ev6clk_delay_a(i)); }
  for( i=1  ; i >= 0 ; i--,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->cfr_frmclk_delay_a(i)); }
  // C_OSB_CT1
  for( i=2  ; i >= 0 ; i--,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_late_write_num_a(i)); }
  for( i=0  ; i < 2  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_cpu_late_write_num_a(i)); }
  // C_OSB_CT2
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->jitter_cmd_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->fast_mode_disable_a); }
  // C_OSB_ARB
  for( i=0  ; i < 4  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sysdc_delay_a(i)); }
  for( i=0  ; i < 2  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->data_valid_dly_a(i)); }
  // C_CLK_BCA
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_ddm_fall_en_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_ddm_rise_en_a); }
  for( i=1  ; i >= 0 ; i--,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_cpu_clk_delay_a(i)); }
  for( i=7  ; i >= 0 ; i--,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_fdbk_en_a(i)); }
  for( i=15 ; i >= 0 ; i--,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_clk_ld_vector_a(i)); }
  for( i=3  ; i >= 0 ; i--,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_bphase_ld_vector_a(i)); }
  // C_CLK_SYS
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_ddm_fall_en_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_ddm_rise_en_a); }
  for( i=1  ; i >= 0 ; i--,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_cpu_clk_delay_a(i)); }
  for( i=7  ; i >= 0 ; i--,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_fdbk_en_a(i)); }
  for( i=15 ; i >= 0 ; i--,j++) { c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_clk_ld_vector_a(i)); }
  for( i=3  ; i >= 0 ; i--,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_bphase_ld_vector_a(i)); }
  for( i=4  ; i >= 0 ; i--,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_frame_ld_vector_a(i)); }
  // additional old signals
  for( i=0  ; i < 2  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->frame_select_a(i)); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_cf_double_clk_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_cf_double_clk_a); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_clk_ratio_a(i)); }
  for( i=2  ; i < 16 ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->bc_clk_ratio_a(i)); }
  for( i=0  ; i < 1  ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_clk_ratio_a(i)); }
  for( i=5  ; i < 16 ; i++,j++)	{ c_mtv_cfg->shift_data_b_h.insert(j, j, c_reg_csr->sys_clk_ratio_a(i)); }

  c_mtv_cfg->shift_data_a_h.insert(WRITE_ONCE_MSB, 0, c_mtv_cfg->shift_data_b_h(WRITE_ONCE_MSB,0)); 
  for (i=0; i <= WRITE_ONCE_MSB; i++) {
    c_mtv_cfg->shift_valid_a_h.insert(i, i, 1); 
    c_mtv_cfg->shift_valid_b_h.insert(i, i, 1); 
  }

  for (i=0; i <= WRITE_MANY_MSB; i++) {
    c_mtv_cfg->write_many_a.insert(i, i, c_reg_ipr->write_many_a(i)); 
    c_mtv_cfg->write_many_b.insert(i, i, c_reg_ipr->write_many_a(i)); 
    c_mtv_cfg->write_many_val_a.insert(i, i, 0); 
    c_mtv_cfg->write_many_val_b.insert(i, i, 0); 
    c_reg_ipr->write_many_a.insert(i, i, 0); 
    c_reg_ipr->write_many_b.insert(i, i, 0); 
  }

#ifndef CBOX_DEBUG
  
  // Extra blank spaces are placeholders for "iNNN->" in the case of
  // multi models.
  //
  char dep_cmd_str[] = "dep -silent       c_reg_csr->* = 0";
#ifdef MP_MODEL
  dep_cmd_str[12] = 'i';
  int loc = 13;
  int cpuid = the_xbox_p->getId();
  int digit;

  if (cpuid >= 1000) {
      MxxOut << "%E-set_cbox_config, cpuid is too large (" << dec << cpuid << ")." << endl;
      cpuid = cpuid % 1000;
  }
  
  if (cpuid >= 100) {
      digit = (cpuid / 100);
      dep_cmd_str[loc++] = digit + '0';
      cpuid -= (digit * 100);
  }
  if (cpuid >= 10) {
      digit = (cpuid / 10);
      dep_cmd_str[loc++] = digit + '0';
      cpuid -= (digit * 10);
  }

  dep_cmd_str[loc++] = cpuid + '0';

  strncpy(&dep_cmd_str[loc], "->", 2);
#endif
  MXX_command(dep_cmd_str);

#endif
}

void mp_functions__serv::mpcmd(const char *cmd_fmt)
{
    int numcpu = NUM_OF_XBOX;
    mpcmd(cmd_fmt, &numcpu);
}

void mp_functions__serv::mpcmd(const char *cmd_fmt, int* numcpu_in)
{
  int numcpu = ((*numcpu_in > NUM_OF_XBOX) ? NUM_OF_XBOX : *numcpu_in);

  char *real_cmd = new char[strlen(cmd_fmt) + 200];  // Assume replacing %'s with cpuid will never exceed 199 characters
  
  real_cmd[0] = '\0';
  
  for(int cpuid=0; cpuid < numcpu; ++cpuid) {

    for (int i=0,j=0; cmd_fmt[i] != '\0'; ++i) {
      if (cmd_fmt[i]=='%') {
        char fmt[5];
        sprintf(fmt,"%%s%%%c",cmd_fmt[++i]);
        j = sprintf(real_cmd,fmt,real_cmd,cpuid);
      }
      else {
        real_cmd[j++] = cmd_fmt[i];
        real_cmd[j] = '\0';
      }
    }

    MxxOut << "%I- Executing Cmd: " << real_cmd << endl;
    MXX_command(real_cmd);
  }

  delete [] real_cmd;
}


void verif_utils__beh::fake_srom()
{
#ifndef CBOX_DEBUG

  if (t_ica_sat->bcst_dosrom_3a_h & ~t_ica_agc->lastidxfill_a_h & c_mtv_cfg->fake_srom_enable_h) {
    switch (t_ica_srm->sq_state_a_h) {
    case 0x1: fake_srom_address = ((MTV_uint64) t->ica_idx_n4b_h(14,3) << 3);
              if (fake_srom_file_enable && (fake_srom_address != max_ic_addr)) {
                MxxOut << "%E-fake_srom: Icache address mis-match" << endl;
                MxxOut << "%E-fake_srom: file_srom_address = 0x" << max_ic_addr << endl;
                MxxOut << "%E-fake_srom: t->ica_idx_n4b_h  = 0x" << fake_srom_address << endl;
                BehErr << "  Address from srom file does not match t->ica_idx_n4b_h(14,3) at first icache write.";
                BehErr << mxxerror("FAKE_SROM_ERROR");
                return;
              }
              break;
    case 0x2: if (fake_srom_address != ((MTV_uint64) t->ica_idx_n4b_h(14,3) << 3)) {
                MxxOut << "%E-fake_srom: Icache address mis-match" << endl;
                MxxOut << "%E-fake_srom: fake_srom_address = 0x" << fake_srom_address << endl;
                MxxOut << "%E-fake_srom: t->ica_idx_n4b_h  = 0x" << ((MTV_uint64) t->ica_idx_n4b_h(14,3) << 3) << endl;
                BehErr << "  Fake srom address does not match t->ica_idx_n4b_h(14,3) during icache write.";
                BehErr << mxxerror("FAKE_SROM_ERROR");
                return;
              }
              if (fake_srom_address == 8) {
                fake_srom_address = 0x07ff0;
              }
              else if (fake_srom_address == 0){
                MxxOut << "%E-fake_srom: Icache address underflow" << endl;
                MxxOut << "%E-fake_srom: t->ica_idx_n4b_h  = 0x" << ((MTV_uint64) t->ica_idx_n4b_h(14,3) << 3) << endl;
                BehErr << "  Fake srom address has been decremented below zero.";
                BehErr << mxxerror("FAKE_SROM_ERROR");
                return;
              }
              else {
                fake_srom_address -= 16;
              }
              break;
    default:
              return;
    }
  }
  else {
    return;
  }

  MxxOut << "%I-fake_srom: Loading icache at address 0x" << fake_srom_address << endl;

  if (fake_srom_file_enable) {
    for (int this_bit_num = (WRITE_ONCE_MSB + 16); this_bit_num >= (WRITE_ONCE_MSB + 16 - SROM_ICDATA_MSB); this_bit_num--) {
      int this_bit_val = (fake_srom_data_p[fake_srom_curbit / 64] >> (fake_srom_curbit % 64)) & 1;
            
      c_mtv_cfg->shift_data_a_h.insert(this_bit_num, this_bit_num, this_bit_val);
      c_mtv_cfg->shift_data_b_h.insert(this_bit_num, this_bit_num, this_bit_val);

      fake_srom_curbit++;
    }
  }
  else {
    MTV_uint32 ic_idx  = (fake_srom_address >> 6) & 0x1ff;                                      // (14-6)  = index
    MTV_uint32 ic_bank = (((fake_srom_address >> 1) & 4) | ((fake_srom_address >> 4) & 3));     // (3,5,4) = set,blk

    MTV_uint64 srom_192, srom_191_128, srom_127_64, srom_63_0;
    
    srom_192     = ((MTV_uint64) j_dat_gdp->lp_ssp_array[ic_bank][ic_idx](0));

    srom_191_128 = ((MTV_uint64) j_dat_gdp->lp_ssp_array[ic_bank][ic_idx](1) << 63) |
                   ((MTV_uint64) j_dat_gdp->lp_ssp_array[ic_bank][ic_idx](2) << 62) |
                   ((MTV_uint64) j_dat_gdp->lp_ssp_array[ic_bank][ic_idx](3) << 61) |
                   ((MTV_uint64) j_dat_gdp->lp_ssp_array[ic_bank][ic_idx](4) << 60) |
                   ((MTV_uint64) j_dat_gdp->lp_data_array[ic_bank][ic_idx](11) << 59) |
                   ((MTV_uint64) j_dat_gdp->lp_data_array[ic_bank][ic_idx](0) << 58) |
                   ((MTV_uint64) j_dat_gdp->lp_data_array[ic_bank][ic_idx](1) << 57) |
                   ((MTV_uint64) j_dat_gdp->lp_data_array[ic_bank][ic_idx](2) << 56) |
                   ((MTV_uint64) j_dat_gdp->lp_data_array[ic_bank][ic_idx](3) << 55) |
                   ((MTV_uint64) j_dat_gdp->lp_data_array[ic_bank][ic_idx](4) << 54) |
                   ((MTV_uint64) j_dat_gdp->lp_data_array[ic_bank][ic_idx](5) << 53) |
                   ((MTV_uint64) j_dat_gdp->lp_data_array[ic_bank][ic_idx](6) << 52) |
                   ((MTV_uint64) j_dat_gdp->lp_data_array[ic_bank][ic_idx](7) << 51) |
                   ((MTV_uint64) j_dat_gdp->lp_data_array[ic_bank][ic_idx](8) << 50) |
                   ((MTV_uint64) j_dat_gdp->lp_data_array[ic_bank][ic_idx](9) << 49) |
                   ((MTV_uint64) j_dat_gdp->lp_data_array[ic_bank][ic_idx](10) << 48) |
                   ((MTV_uint64) j_dat_gdp->lp_data_array[ic_bank][ic_idx](12) << 47) |
                   ((MTV_uint64) j_dat_gdp->lp_data_array[ic_bank][ic_idx](13) << 46) |
                   ((MTV_uint64) j_dat_gdp->lp_data_array[ic_bank][ic_idx](14) << 45) |
                   ((MTV_uint64) j_dat_gdp->lp_data_array[ic_bank][ic_idx](15) << 44) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](87,84) << 40) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](75) << 39) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](76) << 38) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](77) << 37) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](63,58) << 31) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](95) << 30) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](96) << 29) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](97) << 28) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](65) << 27) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](72) << 26) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](73) << 25) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](74) << 24) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](31) << 23) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](30,26) << 18) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](92) << 17) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](93) << 16) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](94) << 15) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](64) << 14) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](32) << 13) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](33) << 12) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](34) << 11) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](35) << 10) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](36) << 9) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](37) << 8) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](38) << 7) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](39) << 6) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](40) << 5) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](41) << 4) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](42) << 3) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](43) << 2) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](169) << 1) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](44));

    srom_127_64  = ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](45) << 63) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](46) << 62) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](47) << 61) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](48) << 60) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](53) << 59) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](49) << 58) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](54) << 57) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](50) << 56) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](55) << 55) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](51) << 54) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](56) << 53) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](52) << 52) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](57) << 51) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](69) << 50) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](0) << 49) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](1) << 48) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](2) << 47) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](3) << 46) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](4) << 45) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](5) << 44) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](6) << 43) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](7) << 42) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](8) << 41) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](9) << 40) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](10) << 39) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](11) << 38) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](12) << 37) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](13) << 36) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](14) << 35) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](15) << 34) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](16) << 33) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](21) << 32) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](17) << 31) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](22) << 30) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](18) << 29) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](23) << 28) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](19) << 27) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](24) << 26) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](20) << 25) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](25) << 24) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](68) << 23) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](169) << 22) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](88) << 21) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](89) << 20) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](90) << 19) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](91) << 18) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](78) << 17) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](79) << 16) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](80) << 15) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](135,130) << 9) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](98) << 8) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](99) << 7) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](100) << 6) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](66) << 5) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](81) << 4) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](82) << 3) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](83) << 2) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](167,166));

    srom_63_0    = ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](165,162) << 60) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](101) << 59) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](102) << 58) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](103) << 57) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](67) << 56) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](104) << 55) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](105) << 54) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](106) << 53) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](107) << 52) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](108) << 51) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](109) << 50) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](110) << 49) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](111) << 48) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](112) << 47) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](113) << 46) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](114) << 45) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](115) << 44) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](168) << 43) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](116) << 42) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](117) << 41) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](118) << 40) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](119) << 39) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](120) << 38) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](125) << 37) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](121) << 36) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](126) << 35) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](122) << 34) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](127) << 33) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](123) << 32) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](128) << 31) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](124) << 30) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](129) << 29) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](70) << 28) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](136) << 27) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](137) << 26) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](138) << 25) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](139) << 24) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](140) << 23) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](141) << 22) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](142) << 21) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](143) << 20) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](144) << 19) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](145) << 18) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](146) << 17) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](147) << 16) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](148) << 15) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](149) << 14) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](150) << 13) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](151) << 12) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](152) << 11) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](157) << 10) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](153) << 9) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](158) << 8) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](154) << 7) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](159) << 6) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](155) << 5) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](160) << 4) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](156) << 3) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](161) << 2) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](71) << 1) |
                   ((MTV_uint64) j_dat_gdp->data_array[ic_bank][ic_idx](168));

    // BIT<192>
    c_mtv_cfg->shift_data_a_h.insert(WRITE_ONCE_MSB + 16, WRITE_ONCE_MSB + 16, srom_192 & 1);
    c_mtv_cfg->shift_data_b_h.insert(WRITE_ONCE_MSB + 16, WRITE_ONCE_MSB + 16, srom_192 & 1);
    // BITS<191:128>
    c_mtv_cfg->shift_data_a_h.insert(WRITE_ONCE_MSB + 15, WRITE_ONCE_MSB - 48, srom_191_128);
    c_mtv_cfg->shift_data_b_h.insert(WRITE_ONCE_MSB + 15, WRITE_ONCE_MSB - 48, srom_191_128);
    // BITS<127:64>
    c_mtv_cfg->shift_data_a_h.insert(WRITE_ONCE_MSB - 49, WRITE_ONCE_MSB - 112, srom_127_64);
    c_mtv_cfg->shift_data_b_h.insert(WRITE_ONCE_MSB - 49, WRITE_ONCE_MSB - 112, srom_127_64);
    // BITS<63:0>
    c_mtv_cfg->shift_data_a_h.insert(WRITE_ONCE_MSB - 113, WRITE_ONCE_MSB - 176, srom_63_0);
    c_mtv_cfg->shift_data_b_h.insert(WRITE_ONCE_MSB - 113, WRITE_ONCE_MSB - 176, srom_63_0);

    j_dat_gdp->data_array[ic_bank][ic_idx] = 0;
    j_dat_gdp->lp_data_array[ic_bank][ic_idx] = 0;
    j_dat_gdp->lp_ssp_array[ic_bank][ic_idx] = 0;

  }

#endif // not CBOX_DEBUG
}

    
void verif_utils__beh::fake_srom_file(const char* filename)
{
    struct stat statbuf;
    
    if (stat(filename, &statbuf) != 0) {
        BehErr << "Unable to stat srom binary file: " << filename << mxxerror("X_ERROR");
        return;
    }
    
    FILE* ifil = fopen(filename, "r");

    if (ifil == NULL) {
        BehErr << "Unable to open srom binary file: " << filename << mxxerror("X_ERROR");
        return;
    }

    fake_srom_numqws = (statbuf.st_size / 8) + ((statbuf.st_size % 8) != 0);
    fake_srom_data_p = new MTV_uint64[fake_srom_numqws];

    // Read in data.
    fread(fake_srom_data_p, 1, statbuf.st_size, ifil);

    fclose(ifil);
    
    fake_srom_file_enable = 1;

    max_ic_addr = 0;
    for (fake_srom_curbit = (WRITE_ONCE_MSB + 16 + 1); fake_srom_curbit <= (WRITE_ONCE_MSB + 16 + 1 + SROM_ICADDR_MSB); fake_srom_curbit++) {
      int this_bit = (fake_srom_data_p[fake_srom_curbit / 64] >> (fake_srom_curbit % 64)) & 1;
      max_ic_addr |= this_bit << (fake_srom_curbit - (WRITE_ONCE_MSB + 16 + 1));
    }

#ifndef CBOX_DEBUG

    c_mtv_cfg->fake_srom_icaddr_a_h = max_ic_addr;
    c_mtv_cfg->fake_srom_icaddr_b_h = max_ic_addr;

#endif // not CBOX_DEBUG

    max_ic_addr = (((max_ic_addr & 0x7ff) << 1) + (max_ic_addr >> 11)) << 3;
    BehErr << "Maximum icache address is 0x" << hex(max_ic_addr) << mxxinform("FAKE_SROM");

}

void verif_utils__beh::drv_pll()
{
#ifndef CBOX_DEBUG

    x_out_p->reset_l.insert(0, 0, ~verif_utils->drv_pll_reset_h);
    x_out_p->dcok_h.insert(0, 0, verif_utils->drv_pll_dcok_h);
    x_out_p->clkfwdrst_h.insert(0, 0, verif_utils->drv_pll_clkfwdrst_h);
    x_out_p->pll_test0_h.insert(0, 0, verif_utils->drv_pll_pll_test0_h);
    x_out_p->irq_h.insert(5, 0, verif_utils->drv_pll_irq_h(5,0));

#endif // not CBOX_DEBUG
}


int io_data_deliver__beh::init()
{
  ioa_insert_index  = 0;
  ioa_top_index     = 0;
  m__st_io_7a_h     = 0;
  m__st_io_8a_h     = 0;
  c__wrio_full_8a_h = 0;
  return 1;
}

int io_data_deliver__beh::save(FILE* fptr)
{
   // nothing to do here
   return 1;
}

int io_data_deliver__beh::restore(FILE* fptr)
{
   // nothing to do here
   return 1;
}

int io_data_deliver__beh::run(int)
{
  if( A_CLK ) return 1; // only concerned with phase B
  
  for( int counter = 0; counter < 32; ++counter )
    ++address_queue_time[counter];

  if(~(MXX_uint64)c->reset_a_h & m__st_io_8a_h & ~(MXX_uint64)m->st_io_abort_8a_h & ~c__wrio_full_8a_h )
    {
      address_queue_time[ioa_insert_index] = 0;
      address_queue_address[ioa_insert_index].insert( M_PA_MSB,3, m->pa0_8a_h(M_PA_MSB,3));
      if( (MXX_uint64)c_dump->dump_ioa )
        {
          BehErr << "Instert at [" << ioa_insert_index << "] for address 0x" << address_queue_address[ioa_insert_index]
                 << mxxinform("IO_DATA");
        }
      ioa_insert_index = ( ioa_insert_index + 1 ) % 32;
    }

  if(~(MXX_uint64)c->reset_a_h & (MXX_uint64)m->st_io_dat_val_6a_h)
    {
      if( ioa_top_index == ioa_insert_index )
        {
          BehErr << "Yikes!  Got STQ data with no address.  ioa_top_index =" << ioa_top_index << mxxerror("IO_DATA");
        }
      if( address_queue_time[ioa_top_index] < MIN_WRIO_DATA_DELAY )
        {
          BehErr << "MBox delivered data for address 0x" << address_queue_address[ioa_top_index];
          BehErr << " too soon.  Data delivered in " << address_queue_time[ioa_top_index] << " cycles.  Minimum is " << MIN_WRIO_DATA_DELAY;
          BehErr << mxxerror("IO_DATA");
        }

      if( (MXX_uint64)c_dump->dump_ioa )
        {
          BehErr << "Data Delay for [" << ioa_top_index << "] address 0x" << address_queue_address[ioa_top_index];
          BehErr << " was delivered in " << address_queue_time[ioa_top_index] << " cycles." << mxxinform("IO_DATA");
        }
      
      ioa_top_index = ( ioa_top_index + 1 ) % 32;
    }

  m__st_io_8a_h = m__st_io_7a_h;
  m__st_io_7a_h = (MXX_uint64)m->st_io_6a_h;

  c__wrio_full_8a_h = (MXX_uint64)c->wrio_full_7a_h;
  return 1;
}
