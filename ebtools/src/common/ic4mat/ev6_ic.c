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
//!   load_icache.c
//!
//!
//!DESCRIPTION
//!   This module includes all the utility routines and other support
//!   routines used for icache loading.
//!
//!
//!
//!REVISION HISTORY:
//!
//!
// $Log: ev6_ic.c,v $
// Revision 1.2  1999/01/21 19:06:26  gries
// First Release to cvs gries
//
 * Revision 1.3  1997/12/05  14:03:29  quinn
 * Franklin's last changes
 *
 * Revision 1.2  1997/07/07  14:57:42  fdh
 * Modified include file name.
 *
 * Revision 1.1  1997/07/07  01:18:37  fdh
 * Initial revision
 *
 * Revision 1.39  1997/06/12  20:35:51  asher
 * Fix bug in previous night's change.
 * Update next_misc_or_ldxl for instructions that AREN'T written in a given
 * line.
 *
 * Revision 1.38  1997/06/11  16:20:35  asher
 * Update to match FPCR slotting change in j.mdl.
 *
 * Revision 1.37  1997/05/16  17:58:20  asher
 * Fix preload start/end to always do at least one block (if enabled).
 *
 * Revision 1.36  1997/04/08  23:40:19  asher
 * Update for changes to create standlone srom image
 * creator.
 *
 * Revision 1.35  1997/01/21  18:03:24  asher
 * Make set predict preloaded be the set we're preloading instead of bit 15
 * of the address.
 *
 * Revision 1.34  1996/12/13  16:59:36  asher
 * Allow loading a range of addresses into the icache.
 *
 * Revision 1.33  1996/10/30  22:51:37  asher
 * MT_FPCR does set DSTV.
 *
 * Revision 1.32  1996/10/23  19:08:21  asher
 * Update to match most recent changes to j.mdl: trouble looks ahead for
 * stxc's as well as ldxl's.
 *
 * Revision 1.31  1996/10/17  19:34:49  asher
 * Fix slot_predecode of opcodes 0x15,16,17 to match j.mdl
 *
 * Revision 1.30  1996/10/07  20:33:42  asher
 * Don't look at call_pal_r23 to determine which register to load up for
 * call pal's.
 *
 * Revision 1.29  1996/08/16  21:14:56  asher
 * Oops.  Must have some default of nothing was specified for the set.
 *
 * Revision 1.28  1996/08/16  15:03:58  asher
 * Don't force to Bit15 for set selection if in other set if valid mode.
 *
 * Revision 1.27  1996/08/15  16:11:51  jlp
 * flip pal mode bit on write and read
 * to match implementation
 *
 * Revision 1.26  1996/08/07  16:32:44  jlp
 * set stall/trouble for hw_ld locked
 *
 * Revision 1.25  1996/08/01  23:03:35  asher
 * Update for changes to j.mdl new ways of decoding stuff related to misc
 * instructions.
 *
 * Revision 1.24  1996/07/17  21:16:52  asher
 * Move trouble_array inside ldic_config structure.
 * Make initialization of a block calculate trouble bits and parity
 * correctly.
 * Fix problem with the stall bit for LDxL instruction at position 3.
 *
 * Revision 1.23  1996/07/13  02:56:57  rcsadmin
 * fix problem with fetch instr and trouble/stall bits
 *
 * Revision 1.22  1996/07/12  16:36:48  jlp
 * make this work more like the RTL and the implementation
 *
 * Revision 1.21  1996/06/26  18:07:18  asher
 * Add print_ldic_error callback to get ALL errors/warnings when loading
 * icache.
 *
 * Revision 1.20  1996/06/07  13:51:30  asher
 * Stop stall propagation for partial icache loads.
 *
 * Revision 1.19  1996/06/06  20:41:41  asher
 * Fix merging with an already valid block.  EASRC and stall values would
 * not propagate from a new instruction through old instruction.
 *
 * Revision 1.18  1996/06/06  18:28:48  asher
 * Fix initializing icache blocks.
 *
 * Revision 1.17  1996/06/04  21:13:12  asher
 * Make opcode 0x14 and 0x1c instructions always issue.
 *
 * Revision 1.16  1996/05/30  18:10:15  asher
 * HW_ST/C to R31 should be NODST.
 *
 * Revision 1.15  1996/05/30  15:12:26  asher
 * Fix EASRC, STALL, and TROUBLE bits in preload of IMPLVER instructions.
 *
 * Revision 1.14  1996/05/29  17:56:13  asher
 * Fix swapping of IMPLVER instruction.
 *
 * Revision 1.13  1996/05/28  20:20:59  asher
 * Make HW_ST with /C set the DST bit in the icache.
 *
 * Revision 1.12  1996/05/28  15:17:53  ejm
 * convert old sign,carry bits into new inc_byp,dec_byp for idp tag adder use
 *
 * Revision 1.11  1996/05/20  23:44:09  jlp
 * add tag parity too
 *
 * Revision 1.10  1996/05/20  22:43:16  jlp
 * calculate and write parity when the icache is loaded
 *
 * Revision 1.9  1996/05/16  14:54:28  asher
 * Fix update of trouble 6 to also include newly written inst and filler
 * inst in calculation.
 *
 * Revision 1.8  1996/05/15  19:28:14  ejm
 * update coding for trouble(6) to indicate NO unconditional branches or call pals (used to qualify jsr training of line predictor)
 *
 * Revision 1.7  1996/04/28  15:18:51  jlp
 * change icache tag arrangement to match the implementation
 *
 * Revision 1.6  1996/04/12  14:38:40  asher
 * Fix bug in loading correct icache set.
 *
 * Revision 1.5  1996/04/03  22:41:16  quinn
 * added stall,trouble,easrc for HW_STxC
 *
 * Revision 1.4  1996/03/15  19:50:16  asher
 * Close file after completing load.
 *
 * Revision 1.3  1996/02/28  20:02:37  jlp
 * make FTOI slotting like ITOF -- check dest_valid
 *
 * Revision 1.2  1996/02/26  18:31:16  quinn
 * src1 now gets ra for all MISC instructions
 *
 * Revision 1.1  1996/02/22  22:35:21  asher
 * Initial revision
 *
// Revision 1.60  1996/02/13  14:59:19  ejm
// reflect valid bit change (valid-asms, valid-asmc)
//
// Revision 1.59  1996/02/02  23:18:23  quinn
// set STALL bit for CALL_PAL's
//
// Revision 1.58  1996/02/02  21:01:11  asher
// Fix infinite loop when NoWarn specified for load.
//
// Revision 1.57  1996/02/01  23:17:56  asher
// Make work on VMS>
//
// Revision 1.56  1996/02/01  23:10:39  asher
// Minor bug fix.
//
// Revision 1.55  1996/01/31  19:33:04  asher
// Fix stupid bugs.
//
// Revision 1.54  1996/01/30  22:40:27  asher
// Now the portion of the icache loader used by verilog models.
//
// Revision 1.53  1995/07/11  21:36:01  asher
// Make sure halt_pc starts off equal to 0xfffc.
//
// Revision 1.52  1995/06/26  20:11:59  quinn
// added load_icache->ld_ic_no_ic_load (used to load program to
// fake_cbox instead of icache)
//
// Revision 1.51  1995/06/23  16:29:11  asher
// First working version.
//
// Revision 1.50  1995/06/16  11:19:03  quinn
// shifted CALL_PAL displacement 2 places to the right
//
// Revision 1.49  1995/06/15  20:48:59  jlp
// Swap src0/src1 fields in CMOV
//
// Revision 1.48  1995/06/15  15:30:17  quinn
// added break to CASE(0) (call_pal) in ldic_ic_predecode()
//
// Revision 1.47  1995/06/14  15:24:26  webb
// - fix ld_dtb_internal() to fill-in the DTB properly
//   based on the GH bit settings (set/clear bits, va XOR pa)
//
// Revision 1.46  1995/06/12  15:33:40  quinn
// add FPCR initialization from Stim-P DXE file and
// modified the set_stall function for LDx_L, MB, RC and RS
//
// Revision 1.45  1995/06/12  14:39:53  huggins
// added new ld_dtb() function
//
// Revision 1.44  1995/06/06  21:55:45  asher
// Split ITB and DTB into separate classes.
//
// Revision 1.43  1995/06/06  14:47:33  asher
// Fix typo in ldic_ic_swap to explicitly give width of local Signal
// variables.
//
// Revision 1.42  1995/06/05  19:18:21  jlp
// Changed instruction modification on icache fill of CALL_PAL instructions.
//
// Revision 1.41  1995/06/01  11:49:02  quinn
// added load_icache->no_ic_load signal
//
// Revision 1.40  1995/05/31  15:20:43  jlp
// tag signal name changes plus don't propigate ea_src backwards in a fill line if trouble bit is set.
//
// Revision 1.39  1995/05/24  23:58:56  asher
// Add load_icache->ld_ic_no_dc_hit to allow option to not cause ld_ic to
// load the dcache.
//
// Revision 1.38  1995/05/24  19:01:32  leibholz
// fixed stxc predecode
//
// Revision 1.36  1995/05/11  22:23:23  asher
// Add full chip stuff under FULL_CHIP conditional compilation.
//
// Revision 1.35  1995/05/10  19:52:00  quinn
// added signal load_icache->no_dc_hit
//
// Revision 1.34  1995/05/05  17:56:39  asher
// Remove #ifdef SPOT stuff.
//
// Revision 1.33  1995/05/05  13:06:12  asher
// Changes to support load misses, spot handling the halt pc, and
// a couple of bug fixes.
//
// Revision 1.32  1995/05/01  15:32:44  quinn
// fixed bug in last change to DXE file loader (addr calculation)
//
// Revision 1.31  1995/04/27  14:49:51  quinn
// fixed DXE file loader (now loads non-longword aligned data)
//
// Revision 1.30  1995/04/24  21:31:43  quinn
// added byte and word loads from Stim-P DXE files
//
// Revision 1.29  1995/04/16  01:14:27  quinn
// changed detection of halt PC to use i_dpe->fetch_vpc_1a_h
//
// Revision 1.28  1995/04/14  04:03:47  quinn
// changed ld_ic() to also load dcache
//
// Revision 1.27  1995/04/13  18:02:01  asher
// Add spot calls under #ifdef SPOT.
//
// Revision 1.26  1995/04/11  14:03:02  quinn
// cast load_icache->debug to (int)
//
// Revision 1.25  1995/04/10  15:00:48  quinn
// added standard DVT completion message
//
// Revision 1.24  1995/04/07  21:30:18  quinn
// modified HW_RET to match JSR
//
// Revision 1.23  1995/04/06  12:44:57  quinn
// corrected coding of default background instruction in icache
//
// Revision 1.22  1995/04/05  15:44:52  quinn
// added DTB load for Stim-P loops and changed FCMOV icache encoding
//
// Revision 1.21  1995/04/04  12:09:20  quinn
// fixed ea_src for FCMOV
//
// Revision 1.20  1995/04/03  21:08:04  quinn
// made initial changes for FCMOV
//
// Revision 1.19  1995/04/03  13:58:39  leibholz
// iq empty and MB stuff
//
// Revision 1.18  1995/03/29  22:33:20  leibholz
// fix rtl_bug1 (fcmov decoded for 0x17 only)
//
// Revision 1.17  1995/03/29  20:05:06  ejm
// add decode for fbr
//
// Revision 1.16  1995/03/22  18:51:56  quinn
// changed CMOV decode only to use inst-bit 7
//
// Revision 1.15  1995/03/17  14:35:56  quinn
// really fixed set_cout function
//
// Revision 1.14  1995/03/17  14:10:58  ejm
// fix set_cout interface
//
// Revision 1.13  1995/03/15  22:17:02  quinn
// fixed DXE loader for non-zero icache tags
//
// Revision 1.12  1995/03/15  16:28:42  quinn
// added load/store-byte/word support
//
// Revision 1.11  1995/03/12  01:13:06  ejm
// fix jsr decodes for ea_src
//
// Revision 1.10  1995/03/10  17:55:30  ejm
// fix cmov decode in ea_src field, should propagate thru previous non trouble ops
//
// Revision 1.9  1995/03/09  19:22:22  quinn
// forced bit 5 from icache to zero for ICMOV
//
// Revision 1.8  1995/03/09  16:00:02  ejm
// typo
//
// Revision 1.7  1995/03/09  15:53:02  ejm
// func bit 5 starts unmodified in icache (0)
//
// Revision 1.6  1995/03/08  20:18:54  quinn
// added (Signal) declaration to j_dat_gdp->row_col_sel_n1bc_h[bank]
//
// Revision 1.5  1995/03/08  18:54:03  quinn
// corrected indexing into local instruction array
//
// Revision 1.4  1995/03/08  14:08:43  quinn
// modified the Stim-P DXE loader
//
// Revision 1.3  1995/03/07  21:51:14  quinn
// merged stim-p dxe loader
//
// Revision 1.2  1995/03/07  19:41:09  ejm
// cmov, mxpr and new jbox model integration updates.
//
// Revision 1.1  1995/03/03  15:09:42  ejm
// Initial revision
//
*/

#include <stdio.h>

#include "ev6_ic.h"

static MTV_uint64 make64_bit(MTV_uint32 hi, MTV_uint32 lo)
{
    return ((MTV_uint64)hi << 32) | lo;
}

/*
 * functions to access the data area for a signal.
 *
 * these all work on 32 bits or less because the icache loading functions
 * never need more.
 */
static int insert_data(MTV_uint32* data_p, int hbit, int lbit, MTV_uint64 new_data)
{
    int start_ind, start_bit, bit_width;
    
    /*
     * move to correct LW for starting bit position.
     */
    data_p += lbit >> 5;
    start_bit = lbit & 0x1f;

    bit_width = hbit - lbit + 1;

    /*
     * do portion of unaligned LW.
     */
    if (start_bit != 0) {
        int numbits = 32 - start_bit;
        MTV_uint32 mask;

        if (numbits > bit_width)
            numbits = bit_width;

        mask = ((1 << numbits) - 1) << start_bit;

        *data_p &= ~mask;
        *data_p |= (new_data << start_bit) & mask;

        ++data_p;
        start_bit = 0;
        bit_width -= numbits;
        new_data >>= numbits;
    }

    while (bit_width >= 32) {

        *data_p &= ~0xffffffffu;
        *data_p |= new_data & 0xffffffffu;

        ++data_p;
        bit_width -= 32;
        new_data >>= 32;
    }
    
    /*
     * if any remaining, do it.
     */
    if (bit_width != 0) {
        MTV_uint32 mask = (1 << bit_width) - 1;

        *data_p &= ~mask;
        *data_p |= new_data & mask;
    }

    return 1;
}

static int insert_2d_array(MTV_uint32* data_base_p, int width, int maxind2, int ind1, int ind2, int hbit, int lbit, MTV_uint64 new_data)
{
    MTV_uint32* data_p;
    int ele_size = (width >> 5) + ((width & 0x1f) != 0);

    /*
     * data is quadword aligned if the width of an individual element is >64 bits.
     */
    if ((width > 64) && ((ele_size & 1) != 0))
        ele_size++;

    /*
     * calculate pointer to beginning of the data for this element in the array.
     */
    data_p = &data_base_p[(ele_size * maxind2 * ind1) + (ele_size * ind2)];

    return insert_data(data_p, hbit, lbit, new_data);
}

static int insert_array(MTV_uint32* data_base_p, int width, int ind, int hbit, int lbit, MTV_uint64 new_data)
{
    MTV_uint32* data_p;
    int ele_size = (width >> 5) + ((width & 0x1f) != 0);

    /*
     * data is quadword aligned if the width of an individual element is >64 bits.
     */
    if ((width > 64) && ((ele_size & 1) != 0))
        ele_size++;

    /*
     * calculate pointer to beginning of the data for this element in the array.
     */
    data_p = &data_base_p[(ele_size * ind)];

    return insert_data(data_p, hbit, lbit, new_data);
}

static MTV_uint64 extract_data(MTV_uint32* data_p, int hbit, int lbit)
{
    MTV_uint64 ret_data = 0;
    int start_ind, start_bit, bit_width;
    int new_start_bit = 0;
    
    /*
     * move to correct LW for starting bit position.
     */
    data_p += lbit >> 5;
    start_bit = lbit & 0x1f;

    bit_width = hbit - lbit + 1;

    /*
     * do portion of unaligned LW.
     */
    if (start_bit != 0) {
        int numbits = 32 - start_bit;
        MTV_uint32 mask;

        if (numbits > bit_width)
            numbits = bit_width;

        mask = (1 << numbits) - 1;

        ret_data |= (*data_p >> start_bit) & mask;

        ++data_p;
        start_bit = 0;
        new_start_bit = numbits;
        bit_width -= numbits;
    }

    while (bit_width >= 32) {
        
        ret_data |= *data_p << new_start_bit;

        ++data_p;
        new_start_bit += 32;
        bit_width -= 32;
    }
    
    /*
     * if any remaining, do it.
     */
    if (bit_width != 0) {
        MTV_uint32 mask = (1 << bit_width) - 1;

        ret_data |= (*data_p & mask) << new_start_bit;
    }

    return ret_data;
}

static MTV_uint64 extract_2d_array(MTV_uint32* data_base_p, int width, int maxind1, int ind1, int ind2, int hbit, int lbit)
{
    MTV_uint32* data_p;
    int ele_size = (width >> 5) + ((width & 0x1f) != 0);

    /*
     * data is quadword aligned if the width of an individual element is >64 bits.
     */
    if ((width > 64) && ((ele_size & 1) != 0))
        ele_size++;

    /*
     * calculate pointer to beginning of the data for this element in the array.
     */
    data_p = data_base_p + (ele_size * maxind1 * ind1) + (ele_size * ind2);

    return extract_data(data_p, hbit, lbit);
}

static int extract_array(MTV_uint32* data_base_p, int width, int ind, int hbit, int lbit)
{
    MTV_uint32* data_p;
    int ele_size = (width >> 5) + ((width & 0x1f) != 0);

    /*
     * data is quadword aligned if the width of an individual element is >64 bits.
     */
    if ((width > 64) && ((ele_size & 1) != 0))
        ele_size++;

    /*
     * calculate pointer to beginning of the data for this element in the array.
     */
    data_p = data_base_p + (ele_size * ind);

    return extract_data(data_p, hbit, lbit);
}

int ldic_load_exe(struct ldic_config* cnfg_p, MTV_uint64 addr, const char* fname_p)
{
#ifdef _WIN32
    FILE* f_p = fopen(fname_p, "rb");
#else
    FILE* f_p = fopen(fname_p, "r");
#endif
    int status;
    
    if (f_p == NULL) {
        cnfg_p->error = ldicOpenError;
        return 0;
    }

    status = ldic_load_file(cnfg_p, addr, f_p, -1);

    fclose(f_p);

    return status;
}

/* load_exe_icache
//
// This function loads data into the EV6 model's icache.  Everything is
// done in blocks of 4 LWs.
//
// addr - 64 bit unsigned address to load the binary data at.
// file_p - stdio file pointer positioned to the binary data to load.
// numbytes - number of byte of binary data to load (-1 means till EOF).
*/
int ldic_load_file(struct ldic_config* cnfg_p, MTV_uint64 addr, FILE* file_p, int numbytes)
{
    /* We're going to do everything in groups of 4 LWs.
     */
    MTV_uint32 inst[4];
    MTV_uint8* byte_data = (MTV_uint8*)inst;

    int return_status = 1;

    int do_all = (numbytes == -1);

    int byte_offset = addr & 0xF;
    int bytes_to_align = 16 - byte_offset;

    int lws_to_align, lws_to_read;
    size_t status;
    
    inst[0] = inst[1] = inst[2] = inst[3] = cnfg_p->filler_lw;
    
    if (!do_all && (bytes_to_align > numbytes))
	bytes_to_align = numbytes;

    /* read first group */
    status = fread(&byte_data[byte_offset], 1, bytes_to_align, file_p);

    if (status != bytes_to_align) {
	/* if we ran out of data, and we were only asking for the
	// amount passed in, then that is an error.
	// (the user asked for more than was in the file)
        */
	if (!do_all)
	    return_status = 0;
	/* get amount actually read */
	bytes_to_align = status;
    }

    numbytes -= bytes_to_align;

    lws_to_align = (bytes_to_align >> 2) + ((bytes_to_align & 0x3) != 0);
    
    if (status != 0)
        ldic_write_line(cnfg_p, (addr & ~make64_bit(0, 0x3)), lws_to_align, &inst[(addr >> 2) & 3]);

    if (!do_all && (numbytes == 0))
	return return_status;

    /* update addr for next read. */
    addr = (addr + 16) & ~make64_bit(0, 0xF);

    while (!ferror(file_p) && !feof(file_p) && (do_all || (numbytes > 0))) {
	int bytes_to_read = (do_all || (numbytes >= 16)) ? 16 : numbytes;

        inst[0] = inst[1] = inst[2] = inst[3] = cnfg_p->filler_lw;

        status = fread(inst, 1, bytes_to_read, file_p);

	if (status != bytes_to_read) {
	    /* if we ran out of data, and we were only asking for the
	    // amount passed in, then that is an error.
	    // (the user asked for more than was in the file)
            */
	    if (!do_all)
		return_status = 0;

	    /* get amount actually read */
	    bytes_to_read = status;
	}

        lws_to_read = (bytes_to_read >> 2) + ((bytes_to_read & 0x3) != 0);
        
        if (status != 0)
            ldic_write_line(cnfg_p, addr, lws_to_read, inst);
        
	numbytes -= bytes_to_read;
	addr += (lws_to_read * 4);
    }

    return return_status;
}

void ldic_write_lw(struct ldic_config* cnfg_p,
                   MTV_uint64 base_pc,
                   MTV_uint32 ic_idx,
                   MTV_uint32 ic_bank,
                   int ind, MTV_uint32 inst,
                   MTV_uint32* next_easrc_p, MTV_uint32* next_stall_p, MTV_uint32 *next_misc_or_ldx_l_p,
                   MTV_uint32* num_early_exit_paths_p)
{
    static int inst_msb[] = { J_I0_MSB, J_I1_MSB, J_I2_MSB, J_I3_MSB };
    static int inst_lsb[] = { J_I0_LSB, J_I1_LSB, J_I2_LSB, J_I3_LSB };
    
    MTV_uint32 dstv, new_easrc, old_easrc, stall_bit, predecode_bits;
    int msb, lsb, trouble_bit;
    
    insert_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX,
                    ic_bank, ic_idx, inst_msb[ind], inst_lsb[ind],
                    ldic_swap(cnfg_p, inst, base_pc + (4*ind)));
    
    dstv = ldic_dstv_predecode(inst);
    insert_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX,
                    ic_bank, ic_idx, (J_DEST_VALID_LSB + ind), (J_DEST_VALID_LSB + ind), dstv);
    
    lsb = (J_COUT_LSB+ind);
    insert_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX,
                    ic_bank, ic_idx, lsb, lsb, ldic_ic_cout(inst, base_pc + (4*ind)));

    trouble_bit = ldic_trouble(inst) | *next_misc_or_ldx_l_p;
    {
        int opc = (inst >> 26) & 0x3f;

#if 0
   	instr <15:8>
  
        1111 1100
   	5432 1098
  	---------
   	0000 0000	trapb
   	0000 0100	excb
   	0100 0000	mb
   	0100 0100	wmb
   	1000 0000	fetch
   	1010 0000	fetch_m
   	1100 0000	rpcc
   	1110 0000	rc
   	1111 0000	rs	
  
   	1110 1000	ecb   
   	1111 1000	wh64  
            ;
#endif 

        if (opc == 0x18) {
            /* Do the decode the same as is j.mdl */
            int type = (inst >> 11) & 0x09;	    /* <13:12> are don't cares */
            
            if (type == 0x8) {   		/* rpcc, rc, rs, mb/wmb */
                *next_misc_or_ldx_l_p = 1;
            } else {
                *next_misc_or_ldx_l_p = 0;
            }
        } else if ((opc == 0x2a) ||						/* opcode LDL_L */
                   (opc == 0x2b) ||						/* opcode LDQ_L */
                   ((opc == 0x1b) && (((inst >> 13) & 0x7) == 1)) ||		/* hw_ldxl */
                   (opc == 0x2E) ||						/* opcode STL_C */
                   (opc == 0x2F) ||						/* opcode STQ_C */
                   ((opc == 0x1F) && (((inst >> 13) & 0x7) == 1))) {		/* HW_STxC */
            *next_misc_or_ldx_l_p = 1;
        } else {
            *next_misc_or_ldx_l_p = 0;
        }
    }
    new_easrc = ldic_easrc(inst, *next_easrc_p, trouble_bit);
    *next_easrc_p = new_easrc;
    stall_bit = ldic_stall(ind, inst, *next_stall_p, num_early_exit_paths_p, trouble_bit);

    /* Second term is because we don't want to stall the ldxl/misc if it
    // is in position 3, but we do want to stall all remaining ones.
    */
    *next_stall_p = stall_bit || (*next_misc_or_ldx_l_p && (ind == 3));
    
    /* Clear old ea_src and stall values for this longword.  */
    old_easrc = extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX,
                                 ic_bank, ic_idx, J_EA_SRC_MSB, J_EA_SRC_LSB);
    old_easrc &= (MTV_uint64)~(0x7 << (3 * ind));
    
    /* Set new ea_src and stall values.
     */
    insert_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX,
                    ic_bank, ic_idx, J_EA_SRC_MSB, J_EA_SRC_LSB,
                    old_easrc | (MTV_uint64)(((stall_bit << 2) | new_easrc) << (3 * ind)));

    /* Clear old trouble bit.
     */
    cnfg_p->trouble_array[ic_idx] = cnfg_p->trouble_array[ic_idx] & ~(1 << ((ic_bank * 4) + ind));

    /* Set new trouble bit.
     */
    cnfg_p->trouble_array[ic_idx] = cnfg_p->trouble_array[ic_idx] | (trouble_bit << ((ic_bank * 4) + ind));

    /* Set slot predecode bits 
     */
    predecode_bits = ldic_slot_predecode(inst,dstv);
    lsb = J_PREDEC_LSB + (3 * ind);
    msb = lsb + (J_PREDEC_BITS / 4) - 1;
    insert_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX,
                    ic_bank, ic_idx, msb, lsb, predecode_bits);

}

static void ldic_internal_write_line(struct ldic_config* cnfg_p,
                                     MTV_uint64 pc,
                                     MTV_uint32 ic_idx,
                                     MTV_uint32 ic_bank,
                                     int num_instr,
                                     const MTV_uint32* inst)
{
    static int inst_msb[] = { J_I0_MSB, J_I1_MSB, J_I2_MSB, J_I3_MSB };
    static int inst_lsb[] = { J_I0_LSB, J_I1_LSB, J_I2_LSB, J_I3_LSB };

    MTV_uint32 next_easrc, next_stall, num_early_exit_paths, br_or_call_pal, next_misc_or_ldx_l;
    MTV_uint32 old_trouble, new_trouble, mask;
    int loop_ctr;
    MTV_uint32 *left_tag_set_p, *right_tag_set_p;
    MTV_uint64 tag_part2, tag_part1, tag_part0, tag_data;
    MTV_uint64 pc_3_2 = ((pc >> 2) & 3);

    /* Clear out pc<3:0>.
     */
    pc &= ~make64_bit(0, 0xF);
    
    /* Form mask of the instructions in this line to write.
     */
    mask = (1 << num_instr) - 1;

    /* Place mask in correct position based on pc<3:2>.
     */
    mask = mask << pc_3_2;

    /* LWs are processed from high (3) to low (0).
     */
    next_easrc = 0;	/* the next higher LW's easrc value (used for current LW easrc calc) */
    next_stall = 0;	/* the next higher LW's stall bit (used for stall bit calc) */
    next_misc_or_ldx_l = 0; /* the (n+1)th instr is a misc or ldx_l */
    num_early_exit_paths = 0;
    br_or_call_pal = 0;    /* set this bit if any unconditional branches or call_pals are in the block (for trouble(6)) */

    /* Loop through the 4 LWs in the icache block.
     */
    for (loop_ctr = 0; loop_ctr < 4; ++loop_ctr) {
        int ind = 3 - loop_ctr;
        int opcode;
        
        /* Is this an instruction provided?
         */
        if ((mask >> ind) & 1) {
            ldic_write_lw(cnfg_p, pc, ic_idx, ic_bank, ind, inst[--num_instr], &next_easrc, &next_stall, &next_misc_or_ldx_l, &num_early_exit_paths);
            opcode = (inst[num_instr] >> 26) & 0x3f;
        } else {
            
            MTV_uint32 swapped_inst;
            int old_stall;
            int old_trouble = ((cnfg_p->trouble_array[ic_idx] >> (ic_bank * 4)) >> ind) & 1;

            /* If next instruction is misc or ldx_l, this instruction
            // needs to be marked trouble.
            */
            int new_trouble = old_trouble || next_misc_or_ldx_l;

            /* The block is already valid, update next_easrc,
            // next_stall, and num_early_exit_paths from the icache
            // array.
            */
            int msb = J_EA_SRC_LSB + 1 + (3 * ind);
            int lsb = J_EA_SRC_LSB + (3 * ind);
            int old_easrc = extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX,
                                             ic_bank, ic_idx, msb, lsb);

            if ((old_easrc != 0x0) || new_trouble)
                next_easrc = old_easrc;
                
            lsb = (msb += 1);
            old_stall = extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX,
                                         ic_bank, ic_idx, msb, lsb);

            /* Determine if this index alters the number of early
            // exit paths (conditional or unconditional branch).
            */
            swapped_inst = extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX,
                                            ic_bank, ic_idx, inst_msb[ind], inst_lsb[ind]);
            opcode = (swapped_inst >> 26) & 0x3F;

            if (opcode == 0x18) {
                /* Do the decode the same as is j.mdl */
                int type = (swapped_inst >> 11) & 0x09;	    /* <13:12> are don't cares */
                
                if (type == 0x8) { 			/* rpcc, rc, rs, mb/wmb */
                    next_misc_or_ldx_l = 1;
                } else {
                    next_misc_or_ldx_l = 0;
                }
            } else if ((opcode == 0x2a) ||						/* opcode LDL_L */
                       (opcode == 0x2b) ||						/* opcode LDQ_L */
                       ((opcode == 0x03) && (((swapped_inst >> 13) & 0x7) == 1)) ||	/* swapped hw_ld locked */
                       (opcode == 0x2E) ||						/* opcode STL_C */
                       (opcode == 0x2F) ||						/* opcode STQ_C */
                       ((opcode == 0x07) && (((swapped_inst >> 13) & 0x7) == 1))) {	/* swapped opcode HW_STxC */
                next_misc_or_ldx_l = 1;
            } else {
                next_misc_or_ldx_l = 0;
            }

            /* This instruction isn't changing, so if the stall bit
            // is set, it probably will remain so (see JSR, HW_RET, etc... below)
            */
            if (old_stall)
                next_stall = 1;

            /* Look for conditional branch.
             */
            if (opcode == 0x38 ||       /* BLBC (unchanged by swap) */
                opcode == 0x39 ||       /* BEQ (unchanged by swap) */
                opcode == 0x3A ||       /* BLT (unchanged by swap) */
                opcode == 0x3B ||       /* BLE (unchanged by swap) */
                opcode == 0x3C ||       /* BLBS (unchanged by swap) */
                opcode == 0x3D ||       /* BNE (unchanged by swap) */
                opcode == 0x3E ||       /* BGE (unchanged by swap) */
                opcode == 0x3F ||       /* BGT (unchanged by swap) */
                opcode == 0x31 ||       /* FBEQ (unchanged by swap) */
                opcode == 0x32 ||       /* FBLT (unchanged by swap) */
                opcode == 0x33 ||       /* FBLE (unchanged by swap) */
                opcode == 0x35 ||       /* FBNE (unchanged by swap) */
                opcode == 0x36 ||       /* FBGE (unchanged by swap) */
                opcode == 0x37)         /* FBGT (unchanged by swap) */
                num_early_exit_paths++;

            /* Look for unconditional branch.
             */
            if ((opcode == 0x00) ||		/* CALL_PAL (unchanged by swap) */
                (opcode == 0x02) ||		/* JSR (0x1A converted to 0x02 in swap) */
                (opcode == 0x06) ||		/* HW_RET (0x1A converted to 0x02 in swap) */
                (opcode == 0x30) ||		/* BR (unchanged by swap) */
                (opcode == 0x34)) {		/* BSR (unchagned by swap) */
                num_early_exit_paths = 1;

                /* JSR, HW_RET, BR, and BSR clear stall
                 */
                if (opcode != 0x0)
                    next_stall = 0;
            }

            /* If we've added an extra early exit path, then stall gets set.
             */
            if (num_early_exit_paths > 1)
                next_stall = 1;

            /* Write new easrc/stall values.
            //
            // Clear old ea_src and stall values for this longword.
            */
            old_easrc = extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX,
                                         ic_bank, ic_idx, J_EA_SRC_MSB, J_EA_SRC_LSB);
            old_easrc &= (MTV_uint64)~(0x7 << (3 * ind));
    
            /* Set new ea_src and stall values.
             */
            insert_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX,
                            ic_bank, ic_idx, J_EA_SRC_MSB, J_EA_SRC_LSB,
                            old_easrc | (MTV_uint64)(((next_stall << 2) | next_easrc) << (3 * ind)));
                
            /* Update trouble array (since it will only be set, never
            // cleared, we just or it in.
            */
            cnfg_p->trouble_array[ic_idx] |= new_trouble << (ic_bank * 4 + ind);
            
        }

        /* Look for unconditional branch
         */
        if ((opcode == 0x00) ||		/* CALL_PAL (unchanged by swap) */
            (opcode == 0x30) ||		/* BR (unchanged by swap) */
            (opcode == 0x34))		/* BSR (unchagned by swap) */
            br_or_call_pal = 1;
    }

    /* store line predictor icache data */
    insert_2d_array(cnfg_p->lp_data_p, J_LP_BITS_PER_FB, J_NUM_INDEX,
                    ic_bank, ic_idx, J_LP_IDX_MSB, J_LP_IDX_LSB, ((pc>> 4) & 0x7ff) + 1);
    insert_2d_array(cnfg_p->lp_data_p, J_LP_BITS_PER_FB, J_NUM_INDEX,
                    ic_bank, ic_idx, J_LP_SP_LSB, J_LP_SP_LSB, (ic_bank>>2) & 0x1);
    insert_2d_array(cnfg_p->lp_ssp_p, 5, J_NUM_INDEX,
                    ic_bank, ic_idx, 3, 0, 0);
    insert_2d_array(cnfg_p->lp_data_p, J_LP_BITS_PER_FB, J_NUM_INDEX,
                    ic_bank, ic_idx, J_LP_SRC_MSB, J_LP_SRC_LSB, 1);
    insert_2d_array(cnfg_p->lp_data_p, J_LP_BITS_PER_FB, J_NUM_INDEX,
                    ic_bank, ic_idx, J_LP_TRAIN_MSB, J_LP_TRAIN_MSB, 1);

    /* store tag data */
    if (((ic_bank >> 2) & 1) == 0) {
        left_tag_set_p  = cnfg_p->s0_tag_lay_p;
        right_tag_set_p = cnfg_p->s0_tag_ray_p;
    }
    else {
        left_tag_set_p  = cnfg_p->s1_tag_lay_p;
        right_tag_set_p = cnfg_p->s1_tag_ray_p;
    }
    /* catch the tag data from pc <47:15> */
    tag_data = (pc >> 15) & 0x1ffffffff;
    tag_part2 = tag_data >> 19;
    tag_part1 = (tag_data >> 10) & 0x1ff;
    tag_part0 = tag_data & 0x3ff;

    insert_array(left_tag_set_p,  34, ic_idx, 33, 20, tag_part2);
    insert_array(left_tag_set_p,  34, ic_idx, 11, 3, tag_part1);
    insert_array(right_tag_set_p, 15, ic_idx, 14, 5, tag_part0);
    insert_array(left_tag_set_p,  34, ic_idx, 0, 0, !cnfg_p->asm_bit);
    insert_array(left_tag_set_p,  34, ic_idx, 1, 1, cnfg_p->asm_bit);
    insert_array(left_tag_set_p,  34, ic_idx, 19, 12, cnfg_p->asn_value);
    insert_array(right_tag_set_p, 15, ic_idx, 4, 1, cnfg_p->kseu_value);
    insert_array(left_tag_set_p,  34, ic_idx, 2, 2, !cnfg_p->pal_bit);

    /* 
    // Read back the tag data written for this line a bit at a time and calculate parity.  Slow but simple.
    */
    {
      int bit_pos;
      MTV_uint64 data_bit;
      MTV_uint64 parity = 0;

      for (bit_pos = 2; bit_pos <= 33; bit_pos++)         /* parity doesn't cover VASMS or VASMC */
      {
        data_bit = extract_array(left_tag_set_p, 34, ic_idx, bit_pos, bit_pos);
        parity   = data_bit ^ parity;
      }
      for (bit_pos = 1; bit_pos <= 14; bit_pos++)         /* bit right<0> is the parity bit itself */
      {
        data_bit = extract_array(right_tag_set_p, 15, ic_idx, bit_pos, bit_pos);
        parity   = data_bit ^ parity;
      }

      /* 
      // Write the parity bit now. 
      */
      insert_array(right_tag_set_p, 15, ic_idx, 0, 0, parity);
    }

    /* generate the new trouble bit priority encodings */

    old_trouble = (cnfg_p->trouble_array[ic_idx] >> (ic_bank * 4)) & 0x0f;
    new_trouble = 0;

    if (old_trouble & 0x01) {                       /* new trouble is priority encoded such that new(3,0) = old(3,0) */
	new_trouble = 0x01;                         /* and new(5,4) = i2_trouble,i1_trouble (from old(2,1))          */
    }                                               /* and new(7)   = i2_trouble (from old(2))                       */
    else if (old_trouble & 0x02) {                  /* BUT new(6) now means there are no ubr or call_pals on the line*/
	new_trouble = 0x02;
    }
    else if (old_trouble & 0x04) {
	new_trouble = 0x04;
    }
    else if (old_trouble & 0x08) {
	new_trouble = 0x08;
    }

    if (old_trouble & 0x02) {
	new_trouble |= 0x10;
    }
    else if (old_trouble & 0x04) {
	new_trouble |= 0x20;
    }


    if (!br_or_call_pal) {             /* bit 6 = NO branches or call_pals to confuse the jsr_trained line predictor */
	new_trouble |= 0x40;
    }

    if (old_trouble & 0x04) {
	new_trouble |= 0x80;
    }

    insert_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX,
                    ic_bank, ic_idx, J_TROUBLE_MSB, J_TROUBLE_LSB, new_trouble);
    /* 
    // Read back the data written for this line a bit at a time and calculate parity.  Slow but simple.
    */
    {
      int bit_pos;
      MTV_uint64 data_bit;
      MTV_uint64 parity = 0;

      for (bit_pos = 0; bit_pos <= 65; bit_pos++)
      {
        data_bit = extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, bit_pos, bit_pos);      
        parity   = data_bit ^ parity;
      }
      for (bit_pos = 68; bit_pos <= 69; bit_pos++)
      {
        data_bit = extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, bit_pos, bit_pos);      
        parity   = data_bit ^ parity;
      }
      for (bit_pos = 72; bit_pos <= 77; bit_pos++)
      {
        data_bit = extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, bit_pos, bit_pos);      
        parity   = data_bit ^ parity;
      }
      for (bit_pos = 84; bit_pos <= 87; bit_pos++)
      {
        data_bit = extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, bit_pos, bit_pos);      
        parity   = data_bit ^ parity;
      }
      for (bit_pos = 92; bit_pos <= 97; bit_pos++)
      {
        data_bit = extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, bit_pos, bit_pos);      
        parity   = data_bit ^ parity;
      }
      /* 
      // Write the left (bit<169>) parity bit now. 
      */
      insert_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, J_PARITY_MSB, J_PARITY_MSB, parity);


      parity = 0;
      for (bit_pos = 66; bit_pos <= 67; bit_pos++)
      {
        data_bit = extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, bit_pos, bit_pos);      
        parity   = data_bit ^ parity;
      }
      for (bit_pos = 70; bit_pos <= 71; bit_pos++)
      {
        data_bit = extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, bit_pos, bit_pos);      
        parity   = data_bit ^ parity;
      }
      for (bit_pos = 78; bit_pos <= 83; bit_pos++)
      {
        data_bit = extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, bit_pos, bit_pos);      
        parity   = data_bit ^ parity;
      }
      for (bit_pos = 88; bit_pos <= 91; bit_pos++)
      {
        data_bit = extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, bit_pos, bit_pos);      
        parity   = data_bit ^ parity;
      }
      for (bit_pos = 98; bit_pos <= 167; bit_pos++)
      {
        data_bit = extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, bit_pos, bit_pos);      
        parity   = data_bit ^ parity;
      }
      /* 
      // Write the right (bit<168>) parity bit now. 
      */
      insert_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, J_PARITY_LSB, J_PARITY_LSB, parity);
    }
}

/* Write the number of instructions specified to the icache line given
// by pc (and in the position given by pc<3:2>).
*/
void ldic_write_line(struct ldic_config* cnfg_p,
                     MTV_uint64 pc,
                     int num_instr,
                     const MTV_uint32* inst)
{
    MTV_uint64 pc_3_2, tag_data, cur_tag, vaddr[2], tag_part2, tag_part1, tag_part0;
    MTV_uint32 ic_idx, set_data, ic_bank;
    MTV_uint32 *left_tag_set_p, *right_tag_set_p;
    int vasmc, vasms, block_valid, cur_pal, check_other_set;
    
    if (num_instr == 0)
        return;

    /* Don't preload if out of range and the range is enabled.
     */
    if (cnfg_p->enable_range &&
        (((pc & ~make64_bit(0,0x3f)) < (cnfg_p->start_addr & ~make64_bit(0,0x3f))) ||
         ((pc | make64_bit(0,0x3f)) > (cnfg_p->end_addr | make64_bit(0,0x3f)))))
        return;
    
    pc_3_2 = ((pc >> 2) & 3);
    
    if (num_instr > (4 - pc_3_2)) {
        cnfg_p->pc = pc;
        cnfg_p->num_instr = num_instr;
        cnfg_p->error = ldicErrCrossBlk;

        if (cnfg_p->print_error_p != NULL)
            (*cnfg_p->print_error_p)(cnfg_p);
                
        num_instr = 4 - pc_3_2;
    }
    
    /* catch the icache index from pc <15:6> */
    ic_idx  = (pc >> 6) & 0x1ff;
    
    /* Determine set to load.  First pick the initial set to try.
     */
    if (cnfg_p->set & ldicBit15)
        set_data = (pc >> 15) & 0x0001;
    else if (cnfg_p->set & ldicSet0)
        set_data = 0;
    else if (cnfg_p->set & ldicSet1)
        set_data = 1;
    else
        set_data = 0;
    
    /* catch the tag data from pc <47:15> */
    tag_data = (pc >> 15) & 0x1ffffffff;

    block_valid = 0;

    check_other_set = 2;	/* two chances to find the correct set. */
    do {
        if (set_data) {
            left_tag_set_p  = cnfg_p->s1_tag_lay_p;
            right_tag_set_p = cnfg_p->s1_tag_ray_p;
        } else {
            left_tag_set_p  = cnfg_p->s0_tag_lay_p;
            right_tag_set_p = cnfg_p->s0_tag_ray_p;
        }
        tag_part2 = extract_array(left_tag_set_p,  34, ic_idx, 33, 20);
        tag_part1 = extract_array(left_tag_set_p,  34, ic_idx, 11, 3);
        tag_part0 = extract_array(right_tag_set_p, 15, ic_idx, 14, 5);
        cur_tag = (tag_part2 << 19) | (tag_part1 << 10) | tag_part0;
        
        vasms = extract_array(left_tag_set_p, 34, ic_idx, 1, 1);
        vasmc = extract_array(left_tag_set_p, 34, ic_idx, 0, 0);
        block_valid = vasms | vasmc;

        cur_pal     = !extract_array(left_tag_set_p, 34, ic_idx, 2, 2);

        if (block_valid && !((cur_tag == tag_data) && (cur_pal == cnfg_p->pal_bit))) {

            vaddr[set_data] = (cur_tag << 15) | (ic_idx << 6);

            if ((cnfg_p->set & ldicOtherSetIfValid) && (check_other_set != 1)/*not last chance*/) {

                set_data = !set_data;
                --check_other_set;

            } else if ((cnfg_p->set & ldicNoWarn) == 0) {

                cnfg_p->error = ldicWarnBlkValid;
                cnfg_p->pc = pc;
                cnfg_p->vaddr[0] = vaddr[0];
                cnfg_p->vaddr[1] = vaddr[1];
                cnfg_p->set_data = set_data;

                if (cnfg_p->print_error_p != NULL)
                    (*cnfg_p->print_error_p)(cnfg_p);
                
                check_other_set = 0;
            } else
                check_other_set = 0;
        } else
            check_other_set = 0;
    } while (check_other_set);

    ic_bank = (set_data << 2) | ((pc >> 4) & 3);       /* (set, 5,4) */
    
    /* If the block isn't yet valid, fill the entire block with filler_lw data.
    // This has to be done because we are only going to write a maximum of 4 LWs
    // here, but we are going to validate the entire 64 byte block.  If we don't
    // prefill the data, later write_ic_line calls will grab data from the valid
    // block, but that data will be bogus.
    */
    if (!block_valid || (cur_tag != tag_data) || (cur_pal != cnfg_p->pal_bit)) {
        /* Get pc of start of 64 byte block.
         */
        MTV_uint64 temp_pc = pc & make64_bit(0xFFFFFFFF, 0xFFFFFFC0);  /* clear out <5:0> */
        MTV_uint32 temp_ic_bank = (set_data << 2);
        int outer;

        MTV_uint32 temp_inst[4];
        int ctr;

        for (ctr = 0; ctr < 4; ++ctr)
            temp_inst[ctr] = cnfg_p->filler_lw;
        
        for (outer = 0; outer < 4; ++outer) {

            ldic_internal_write_line(cnfg_p, temp_pc, ic_idx, temp_ic_bank, 4/*num_instr*/, temp_inst);
                                     
            temp_pc += 16;
            temp_ic_bank += 1;
        }
    }
    
    /* Write the instructions.
     */
    ldic_internal_write_line(cnfg_p, pc, ic_idx, ic_bank, num_instr, inst);

}

void ldic_output_srom_data(struct ldic_config* cnfg_p, int set, MTV_uint64 addr)
{
    MTV_uint32 addr_14_3  = ((addr & 0x7ff0) >> 4) | (set << 11);
    MTV_uint32 ic_idx  = (addr >> 6) & 0x1ff;
    MTV_uint32 ic_bank = ((set << 2) | ((addr >> 4) & 3));       // (set, 5,4)

    MTV_uint64 srom_192, srom_191_128, srom_127_64, srom_63_0;
    
    srom_192     = (extract_2d_array(cnfg_p->lp_ssp_p, 5, J_NUM_INDEX, ic_bank, ic_idx, 0, 0));

    srom_191_128 = (extract_2d_array(cnfg_p->lp_ssp_p, 5, J_NUM_INDEX, ic_bank, ic_idx, 1, 1) << 63) |
                   (extract_2d_array(cnfg_p->lp_ssp_p, 5, J_NUM_INDEX, ic_bank, ic_idx, 2, 2) << 62) |
                   (extract_2d_array(cnfg_p->lp_ssp_p, 5, J_NUM_INDEX, ic_bank, ic_idx, 3, 3) << 61) |
                   (extract_2d_array(cnfg_p->lp_ssp_p, 5, J_NUM_INDEX, ic_bank, ic_idx, 4, 4) << 60) |
                   (extract_2d_array(cnfg_p->lp_data_p, J_LP_BITS_PER_FB, J_NUM_INDEX, ic_bank, ic_idx, 11, 11) << 59) |
                   (extract_2d_array(cnfg_p->lp_data_p, J_LP_BITS_PER_FB, J_NUM_INDEX, ic_bank, ic_idx, 0, 0) << 58) |
                   (extract_2d_array(cnfg_p->lp_data_p, J_LP_BITS_PER_FB, J_NUM_INDEX, ic_bank, ic_idx, 1, 1) << 57) |
                   (extract_2d_array(cnfg_p->lp_data_p, J_LP_BITS_PER_FB, J_NUM_INDEX, ic_bank, ic_idx, 2, 2) << 56) |
                   (extract_2d_array(cnfg_p->lp_data_p, J_LP_BITS_PER_FB, J_NUM_INDEX, ic_bank, ic_idx, 3, 3) << 55) |
                   (extract_2d_array(cnfg_p->lp_data_p, J_LP_BITS_PER_FB, J_NUM_INDEX, ic_bank, ic_idx, 4, 4) << 54) |
                   (extract_2d_array(cnfg_p->lp_data_p, J_LP_BITS_PER_FB, J_NUM_INDEX, ic_bank, ic_idx, 5, 5) << 53) |
                   (extract_2d_array(cnfg_p->lp_data_p, J_LP_BITS_PER_FB, J_NUM_INDEX, ic_bank, ic_idx, 6, 6) << 52) |
                   (extract_2d_array(cnfg_p->lp_data_p, J_LP_BITS_PER_FB, J_NUM_INDEX, ic_bank, ic_idx, 7, 7) << 51) |
                   (extract_2d_array(cnfg_p->lp_data_p, J_LP_BITS_PER_FB, J_NUM_INDEX, ic_bank, ic_idx, 8, 8) << 50) |
                   (extract_2d_array(cnfg_p->lp_data_p, J_LP_BITS_PER_FB, J_NUM_INDEX, ic_bank, ic_idx, 9, 9) << 49) |
                   (extract_2d_array(cnfg_p->lp_data_p, J_LP_BITS_PER_FB, J_NUM_INDEX, ic_bank, ic_idx, 10, 10) << 48) |
                   (extract_2d_array(cnfg_p->lp_data_p, J_LP_BITS_PER_FB, J_NUM_INDEX, ic_bank, ic_idx, 12, 12) << 47) |
                   (extract_2d_array(cnfg_p->lp_data_p, J_LP_BITS_PER_FB, J_NUM_INDEX, ic_bank, ic_idx, 13, 13) << 46) |
                   (extract_2d_array(cnfg_p->lp_data_p, J_LP_BITS_PER_FB, J_NUM_INDEX, ic_bank, ic_idx, 14, 14) << 45) |
                   (extract_2d_array(cnfg_p->lp_data_p, J_LP_BITS_PER_FB, J_NUM_INDEX, ic_bank, ic_idx, 15, 15) << 44) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 87, 84) << 40) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 75, 75) << 39) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 76, 76) << 38) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 77, 77) << 37) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 63, 58) << 31) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 95, 95) << 30) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 96, 96) << 29) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 97, 97) << 28) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 65, 65) << 27) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 72, 72) << 26) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 73, 73) << 25) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 74, 74) << 24) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 31, 31) << 23) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 30, 26) << 18) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 92, 92) << 17) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 93, 93) << 16) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 94, 94) << 15) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 64, 64) << 14) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 32, 32) << 13) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 33, 33) << 12) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 34, 34) << 11) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 35, 35) << 10) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 36, 36) << 9) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 37, 37) << 8) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 38, 38) << 7) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 39, 39) << 6) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 40, 40) << 5) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 41, 41) << 4) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 42, 42) << 3) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 43, 43) << 2) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 169, 169) << 1) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 44, 44));

    srom_127_64  = (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 45, 45) << 63) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 46, 46) << 62) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 47, 47) << 61) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 48, 48) << 60) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 53, 53) << 59) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 49, 49) << 58) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 54, 54) << 57) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 50, 50) << 56) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 55, 55) << 55) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 51, 51) << 54) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 56, 56) << 53) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 52, 52) << 52) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 57, 57) << 51) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 69, 69) << 50) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 0, 0) << 49) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 1, 1) << 48) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 2, 2) << 47) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 3, 3) << 46) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 4, 4) << 45) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 5, 5) << 44) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 6, 6) << 43) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 7, 7) << 42) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 8, 8) << 41) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 9, 9) << 40) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 10, 10) << 39) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 11, 11) << 38) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 12, 12) << 37) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 13, 13) << 36) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 14, 14) << 35) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 15, 15) << 34) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 16, 16) << 33) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 21, 21) << 32) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 17, 17) << 31) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 22, 22) << 30) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 18, 18) << 29) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 23, 23) << 28) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 19, 19) << 27) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 24, 24) << 26) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 20, 20) << 25) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 25, 25) << 24) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 68, 68) << 23) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 169, 169) << 22) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 88, 88) << 21) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 89, 89) << 20) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 90, 90) << 19) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 91, 91) << 18) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 78, 78) << 17) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 79, 79) << 16) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 80, 80) << 15) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 135, 130) << 9) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 98, 98) << 8) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 99, 99) << 7) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 100, 100) << 6) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 66, 66) << 5) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 81, 81) << 4) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 82, 82) << 3) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 83, 83) << 2) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 167, 166));

    srom_63_0    = (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 165, 162) << 60) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 101, 101) << 59) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 102, 102) << 58) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 103, 103) << 57) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 67, 67) << 56) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 104, 104) << 55) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 105, 105) << 54) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 106, 106) << 53) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 107, 107) << 52) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 108, 108) << 51) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 109, 109) << 50) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 110, 110) << 49) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 111, 111) << 48) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 112, 112) << 47) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 113, 113) << 46) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 114, 114) << 45) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 115, 115) << 44) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 168, 168) << 43) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 116, 116) << 42) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 117, 117) << 41) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 118, 118) << 40) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 119, 119) << 39) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 120, 120) << 38) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 125, 125) << 37) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 121, 121) << 36) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 126, 126) << 35) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 122, 122) << 34) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 127, 127) << 33) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 123, 123) << 32) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 128, 128) << 31) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 124, 124) << 30) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 129, 129) << 29) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 70, 70) << 28) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 136, 136) << 27) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 137, 137) << 26) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 138, 138) << 25) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 139, 139) << 24) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 140, 140) << 23) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 141, 141) << 22) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 142, 142) << 21) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 143, 143) << 20) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 144, 144) << 19) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 145, 145) << 18) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 146, 146) << 17) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 147, 147) << 16) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 148, 148) << 15) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 149, 149) << 14) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 150, 150) << 13) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 151, 151) << 12) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 152, 152) << 11) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 157, 157) << 10) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 153, 153) << 9) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 158, 158) << 8) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 154, 154) << 7) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 159, 159) << 6) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 155, 155) << 5) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 160, 160) << 4) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 156, 156) << 3) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 161, 161) << 2) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 71, 71) << 1) |
                   (extract_2d_array(cnfg_p->data_p, J_BITS_PER_DATA_FB, J_NUM_INDEX, ic_bank, ic_idx, 168, 168));

    (*cnfg_p->print_srom_data_p)(cnfg_p, addr, set, addr_14_3, srom_192, srom_191_128, srom_127_64, srom_63_0);
}

MTV_uint32 ldic_swap(struct ldic_config* cnfg_p, MTV_uint32 inst, MTV_uint64 pc)
{
   unsigned int ic_inst,opcode,src0,src1,dst,ra,rb,rc;
   unsigned int ic_inst15_13,ic_inst12_11,ic_inst10_8,ic_inst7_5;
   unsigned int target_addr,disp19_0,pc21_2,sign,cout,dec_byp;

   ic_inst = inst;
   opcode = (inst>>26) & 0x3f;

   target_addr = (inst & 0xfffff) + ((pc >> 2) & 0xfffff) + 1;
 
   ic_inst15_13 =(inst>>13) & 0x7;
   ic_inst12_11 =(inst>>11) & 0x3;
   ic_inst10_8 =(inst>>8) & 0x7;
   ic_inst7_5 =(inst>>5) & 0x7;
 
   ra = (inst>>21) & 0x1f;
   rb = (inst>>16) & 0x1f;
   rc = (inst>>0) & 0x1f;

   sign = (inst>>20) & 0x1;

   disp19_0 = inst & 0xfffff;
   pc21_2 = (pc >> 2) & 0xfffff;
   cout = (((disp19_0 + pc21_2) + 1) >> 20) & 0x1;

   /* dec_byp = S.~C to replace old sign of disp bit in bit 20 */
   dec_byp = sign & !cout;

   switch((inst>>26) & 0x3f) {	     
   case 0x0: /*callpal*/
       if (1/*cnfg_p->call_pal_r23 -- always needs to be set now*/)
           dst 	       = 0x17;							/* <25:21> for NT-mode */
       else
           dst         = 0x1B;							/* <25:21> for VMS/OSF */
       src0            = 0;							/* <20:16> */
       ic_inst15_13    = 0;							/* <15:13> */
       ic_inst12_11    = !((inst & 0x40) | (inst & 0x3ffff00));		        /* <12:11>   <25:8> and <6> mbz or cause resopc */
       ic_inst10_8     = ((inst & 0x080) >> 5) | ((inst & 0x030) >> 4);		/* <10:8> */
       ic_inst7_5      =  (inst & 0x00E) >> 1;					/* <7:5>    */
       src1            =  (inst & 0x001) << 4;					/* <4:0> */
       break;									/* shifted displacement 2 places to the right */
   case 0x1:/*dha, opcdec*/
   case 0x3:
   case 0x5:
   case 0x7:
       src0 = rb;
       src1 = rc;
       dst = rc;
       opcode = 0x1B;
       break;
   case 0x2:/*dha, opcdec*/
   case 0x4:
   case 0x6:
       src0 = rb;
       src1 = rc;
       dst = rc;
       opcode = 0x1A;
       break;
   case 0x8:/*lda*/
   case 0x9:/*ldah*/
       src0 = rb;
       src1 = rc;
       dst = ra;
       break;
   case 0xA:/*LDB*/
       src0 = rb;
       src1 = rc;
       dst = ra;
       break;
   case 0xB:/*LDQ_U*/       
       src0 = rb;
       src1 = rc;
       dst = ra;
       break;
   case 0xC:/*LDW*/
       src0 = rb;
       src1 = rc;
       dst = ra;
       break;
   case 0xD:/*STW*/
   case 0xE:/*STB*/ 
       src0 = rb;
       src1 = ra;
       dst = rc;
       break;
   case  0xF:/*stq_u*/
       src0 = rb;
       src1 = ra;
       dst = rc;
       break;       
   case 0x10:/*IOP.IOPL*/ 
       src0 = ra;
       src1 = rb;
       dst = rc;
       break;
   case  0x11:
       /* check for  icmov */
       if (((inst >> 7) & 0x3) == 0x1)
           {
               /* opcode should be 0x7<12><6> */
               opcode = (0x7 << 2) | ((inst >> 11) & 0x2) | ((inst >> 6) & 0x1);
               src0 = ra;
               src1 = rc;
               dst  = rc;
               ic_inst15_13 = (inst >> 13) & 0x7;					/*mdq     was >> 18 */
               ic_inst12_11 = (inst >> 19) & 0x3;					/*mdq     was >> 16 */
               ic_inst10_8 = (inst >> 16) & 0x7;					/*mdq     was >> 13 */
               ic_inst7_5 = (inst >> 9) & 0x6 ;				        /*mdq     was & 0x7 */
           }
       else
           {
               src0 = ra;
               src1 = rb;
               dst = rc;
           }
       break;
   case 0x12: 
   case 0x13:
       src0 = ra;
       src1 = rb;
       dst = rc;
       break;
   case 0x14:/*new fop*/
       if (((inst & 0xf9e0) == 0x0080) && ((inst & 0x0600) != 0x0600))		/* itof   (mtg) */
           {
               src0 = rb;
               src1 = ra;
               dst = rc;
           }
       else					/* sqrt */
           {
               src0 = ra;
               src1 = rb;
               dst = rc;
           }
       break;
   case 0x15:/*fp*/
   case 0x16:/*fp*/
       src0 = ra;
       src1 = rb;
       dst = rc;
       break;
   case 0x17:/*fp*/
       /* check for  fcmov */
       if (((inst >> 8) & 0x1) == 0x1)
           {
               src0 = ra;
               src1 = rc;
               dst  = rc;
               ic_inst15_13 = (inst >> 8) & 0x7;		/* <10:8> */
               ic_inst12_11 = (inst >> 19) & 0x3;		/* <20:19> */
               ic_inst10_8 = (inst >> 16) & 0x7;		/* <18:16> */
               ic_inst7_5 = (inst >> 5) & 0x7;		/* <7:5> */
           }
       else
           {
               src0 = ra;
               src1 = rb;
               dst = rc;
           }
       break;
   case 0x18:/*misc*/
       src0 = rb;
       src1 = ra;                 /* this fixes bug with ECB and WH64 */
       dst = ra;
       break;
   case 0x19:/*HW_MFPR*/
       src0 = rb;
       src1 = rc;
       dst = ra;
       opcode = 0x01;
       break;
   case 0x1A:/*JSR*/
   case 0x1E:/*HW_RET*/
       src0 = rb;
       src1 = target_addr & 0x1f;
       dst  = ra;
       ic_inst12_11 = (target_addr >> 11) & 0x3;
       ic_inst10_8 = (target_addr >> 8) & 0x7;
       ic_inst7_5 = (target_addr >> 5) & 0x7;
       opcode &= ~0x18;
       break;
   case 0x1B:/*HW_LD*/
       src0 = rb;
       src1 = rc;
       dst = ra;
       opcode = 0x03;
       break;
   case 0x1C:/*new iop*/
       if ((inst & 0x0800) == 0x0800)		/* ftoi */
           {
               src0 = rb;
               src1 = ra;
               dst = rc;
           }
       else					/* others */
           {
               src0 = ra;
               src1 = rb;
               dst = rc;
           }
       opcode = 0x04;
       break;
   case 0x1D:/*HW_MTPR*/
       src0 = rb;
       src1 = rc;
       dst = ra;
       opcode = 0x05;
       break;
   case 0x1F:/*HW_ST*/
       src0 = rb;
       src1 = ra;
       dst = rc;
       opcode = 0x07;
       break;
   case 0x20:/*ldf*/
   case 0x21:/*ldf*/
   case 0x22:/*ldf*/
   case 0x23:/*ldf*/
       src0 = rb;
       src1 = rc;
       dst = ra;
       break;
   case 0x24:/*stf*/
   case 0x25:/*stf*/
   case 0x26:/*stf*/
   case 0x27:/*stf*/
       src0 = rb;
       src1 = ra;
       dst = rc;
       break;
   case 0x28:/*ldl*/
   case 0x29:/*ldl*/
   case 0x2A:/*ldl*/
   case 0x2B:/*ldl*/
       src0 = rb;
       src1 = rc;
       dst = ra;
       break;
   case 0x2C:/*stl*/
   case 0x2D:/*stl*/
   case 0x2E:/*stl*/
   case 0x2F:/*stl*/
       src0 = rb;
       src1 = ra;
       dst = rc;
       break;
   case 0x30:/*br*/
   case 0x34:/*bsr*/
       src0 = (dec_byp << 4) | (target_addr >> 16) & 0xf;                /* bit 20 is dec_byp = S.~C */
       src1 = target_addr & 0x1f;
       dst  = ra;
       ic_inst15_13 = (target_addr >> 13) & 0x7;
       ic_inst12_11 = (target_addr >> 11) & 0x3;
       ic_inst10_8 = (target_addr >> 8) & 0x7;
       ic_inst7_5 = (target_addr >> 5) & 0x7;
       break;
   case 0x31:/*fbr*/
   case 0x32:/*fbr*/
   case 0x33:/*fbr*/
   case 0x35:/*fbr*/
   case 0x36:/*fbr*/
   case 0x37:/*fbr*/
   case 0x38:/*cbr*/
   case 0x39:/*cbr*/
   case 0x3A:/*cbr*/
   case 0x3B:/*cbr*/
   case 0x3C:/*cbr*/
   case 0x3D:/*cbr*/
   case 0x3E:/*cbr*/
   case 0x3F:/*cbr*/
       src0 = ra;
       src1 = target_addr & 0x1f;
       dst  = (dec_byp << 4) | (target_addr >> 16) & 0xf;
       ic_inst15_13 = (target_addr >> 13) & 0x7;
       ic_inst12_11 = (target_addr >> 11) & 0x3;
       ic_inst10_8 = (target_addr >> 8) & 0x7;
       ic_inst7_5 = (target_addr >> 5) & 0x7;
       break;
   default:
       src0 = ra;
       src1 = rb;
       dst = rc;
       break;
   }

   /* construct the scramble format of instruction */ 
   ic_inst = ((opcode & 0x3f) << 26) |
             ((dst & 0x1f) << 21) |
             ((src0 & 0x1f) << 16) |
             ((ic_inst15_13 & 0x7) << 13) |
             ((ic_inst12_11 & 0x3) << 11) |
             ((ic_inst10_8 & 0x7) << 8) |
             ((ic_inst7_5 & 0x7) << 5) |
             (src1 & 0x1f);

   return(ic_inst);
}

MTV_uint32 ldic_dstv_predecode(MTV_uint32 inst)
{
    int predec=0;
    switch((inst>>26) & 0x3f)
        {
        case 0x0:	/*callpal*/
            predec=DST;
            break;

        case 0x1: /*opcdec*/
        case 0x2: /*opcdec*/
        case 0x3: /*opcdec*/
        case 0x4: /*opcdec*/
        case 0x5: /*opcdec*/
        case 0x6: /*opcdec*/
        case 0x7: /*opcdec*/
            predec=NODST;
            break;

        case 0x8: /*lda*/
            if (((inst>>21) & 0x1f)==0x1f)
                predec=NODST;
            else
                predec=DST;
            break;

        case 0x9: /*ldah*/
            if (((inst>>21) & 0x1f)==0x1f)
                predec=NODST;
            else
                predec=DST;
            break;

        case 0xA: /*LDB*/
            if (((inst>>21) & 0x1f)==0x1f)
                predec=NODST;
            else
                predec=DST;
            break;

        case 0xB: /*LDQ_U*/
            if (((inst>>21) & 0x1f)==0x1f)
                predec=NODST;
            else
                predec=DST;
            break;

        case 0xC: /*LDW*/
            if (((inst>>21) & 0x1f)==0x1f)
                predec=NODST;
            else
                predec=DST;
            break;

        case 0xD: /*STW*/
        case 0xE: /*STB*/
            predec=NODST;
            break;

        case 0xF: /*stq_u*/
            predec=NODST;
            break;

        case 0x10: /*IOP.IOPL*/
        case 0x11:
        case 0x12:
        case 0x13:
            if ((inst & 0x1f)==0x1f)
                predec=NODST;
            else
                predec=DST;
            break;

        case 0x14: /*fp sqrt*/
            if ((inst & 0x1f)==0x1f)
                predec=NODST;
            else
                predec=DST;
            break;


        case 0x15: /*fp */
            if ((inst & 0x1f)==0x1f)
                predec=NODST;
            else
                predec=DST;
            break;

        case 0x16: /*fp*/
            if ((inst & 0x1f)==0x1f)
                predec=NODST;
            else
                predec=DST;
            break;
        case 0x17: /*fp cvt,fcmov,MX_FPCR */
            if ((inst & 0x1f)==0x1f)
                predec=NODST;
            else
                predec=DST;
            break;

        case 0x18: /*misc*/
            if ((((inst >> 21) & 0x1f) != 0x1f) &&	// this is how the chip decodes it.
                (((inst >> 11) & 0x19) == 0x18))
                predec = DST;
            else
                predec = NODST;
	    break;

        case 0x19: /*HW_MFPR*/
            if (((inst>>21) & 0x1f)==0x1f)
                predec=NODST;
            else
                predec=DST;
            break;

        case 0x1A: /*JSR*/
            if (((inst>>21) & 0x1f)==0x1f)
                predec=NODST;
            else
                predec=DST;
            break;

        case 0x1B: /*HW_LD*/
            if (((inst>>21) & 0x1f)==0x1f)
                predec=NODST;
            else
                predec=DST;
            break;

        case 0x1C: /*new IOP*/
            if (((inst>>0) & 0x1f)==0x1f)
                predec=NODST;
            else
                predec=DST;
            break;

        case 0x1D: /*HW_MTPR*/
            predec=NODST;
            break;

        case 0x1E: /*HW_RET*/
            if (((inst>>21) & 0x1f)==0x1f)
                predec=NODST;
            else
                predec=DST;
            break;

        case 0x1F: /*HW_ST*/
            if ((((inst >> 13) & 0x7) == 1) &&	// HW_ST/C
                (((inst>>21) & 0x1f)!=0x1f))	// ...NOT to R31
                predec = DST;
            else
                predec = NODST;
            break;

        case 0x20: /*ldf*/
        case 0x21: /*ldf*/
        case 0x22: /*ldf*/
        case 0x23: /*ldf*/
            if (((inst>>21) & 0x1f)==0x1f)
                predec=NODST;
            else
                predec=DST;
            break;

        case 0x24: /*stf*/
        case 0x25: /*stf*/
        case 0x26: /*stf*/
        case 0x27: /*stf*/
            predec=NODST;
            break;

        case 0x28: /*ldl*/
        case 0x29: /*ldl*/
        case 0x2A: /*ldl*/
        case 0x2B: /*ldl*/
            if (((inst>>21) & 0x1f)==0x1f)
                predec=NODST;
            else
                predec=DST;
            break;

        case 0x2C: /*stl*/
        case 0x2D: /*stl*/
            predec=NODST;
            break;

        case 0x2E: /*stl_c*/
        case 0x2F: /*stq_c*/
            if (((inst>>21) & 0x1f)==0x1f)
                predec=NODST;
            else
                predec=DST;
            break;
        case 0x30: /*br*/
            if (((inst>>21) & 0x1f)==0x1f)
                predec=NODST;
            else
                predec=DST;
            break;

        case 0x31: /*fbr*/
        case 0x32: /*fbr*/
        case 0x33: /*fbr*/
            predec=NODST;
            break;

        case 0x34: /*br*/
            if (((inst>>21) & 0x1f)==0x1f)
                predec=NODST;
            else
                predec=DST;
            break;

        case 0x35: /*fbr*/
        case 0x36: /*fbr*/
        case 0x37: /*fbr*/
            predec=NODST;
            break;

        case 0x38:/*cbr*/
        case 0x39:/*cbr*/
        case 0x3A:/*cbr*/
        case 0x3B:/*cbr*/
        case 0x3C:/*cbr*/
        case 0x3D:/*cbr*/
        case 0x3E:/*cbr*/
        case 0x3F:/*cbr*/
            predec=NODST;
            break;

        default:
            break;
        }
    return(predec);
}  

MTV_uint32 ldic_slot_predecode(MTV_uint32 inst, MTV_uint32 dstv)
{
    int predec=0;
    switch((inst>>26) & 0x3f)
        {
        case 0x0:	/*callpal*/
            predec=IQ_L;
            break;

        case 0x1: /*opcdec*/
        case 0x2: /*opcdec*/
        case 0x3: /*opcdec*/
        case 0x4: /*opcdec*/
        case 0x5: /*opcdec*/
        case 0x6: /*opcdec*/
        case 0x7: /*opcdec*/
        case 0x8: /*lda*/
        case 0x9: /*ldah*/
            predec=IQ_X;
            break;

        case 0xA: /*LDB*/
            predec=IQ_L;
            break;

        case 0xB: /*LDQ_U*/
            if (dstv)
                predec=IQ_L;
            else
                predec=NEITHER_X;
            break;

        case 0xC: /*LDW*/
        case 0xD: /*STW*/
        case 0xE: /*STB*/
        case 0xF: /*stq_u*/
            predec=IQ_L;
            break;

        case 0x10: /*IOP.IOPL*/
        case 0x11:
            if (dstv)
                predec=IQ_X;
            else
                predec=NEITHER_X;
            break;

        case 0x12:
        case 0x13:
            if (dstv)
                predec=IQ_U;
            else
                predec=NEITHER_X;
            break;

/* OPC 0x14  new Fop
// -----------------
// 	inst<15:5>
// 	-------------
// 	000 0000 0100   ItoF_S
// 	000 0001 0100   ItoF_F
// 	000 0010 0100   ItoF_T
//
// 	x0x x0x0 1010	VAX SQRT
//
// 	xxx xxx0 1011	IEEE SQRT
//
// ItoF: <15:9>==0, <7>==1, <6:5>==0
*/
        case 0x14: /*fp sqrt*/
            if (((inst & 0xf9e0) == 0x0080) && ((inst & 0x0600) != 0x0600))
                predec=IQ_L;
	    else
		predec=FQ_X;
            break;

        case 0x15: /*fp */
        case 0x16: /*fp*/
        case 0x17: /*fp cvt,fcmov,MX_FPCR */
            if (dstv ||
                ((((inst & 0x0ffe0) == 0x00080) ||
                  ((inst & 0x0ffe0) == 0x00280) ||
                  ((inst & 0x0ffe0) == 0x00480)) &&	//dha: valid_itof_func (see j.mdl)
                 (((inst >> 26) & 0x3) == 0x3)))
                predec=FQ_X;
            else
	      predec=NEITHER_X;
            break;
	    
/*						MPD     SST     ADD     LOG
//	instr <15:8>				cclk<3>	cclk<2>	cclk<1>	cclk<0>	
//
//      1111 1100
//	5432 1098	
//	---------	
//	0000 0000	trapb			0/0	0/0	0/0	0/1
//	0000 0100	excb			0/0	0/0	0/0	0/1
//	0100 0000	mb			0/0	0/0	0/0	0/1
//	0100 0100	wmb			0/0	0/0	0/0	0/1
//	1000 0000	fetch			0/0	0/0	0/0	0/1
//	1010 0000	fetch_m			0/0	0/0	0/0	0/1
//	1100 0000	rpcc			0/0	0/0	0/0	0/1
//	1110 0000	rc			0/0	0/0	0/0	1/1						
//	1111 0000	rs			0/0	0/0	0/0	1/1
//
//	1110 1000	ecb			0/0	0/1	1/1	0/0
//	1111 1000	wcbh			0/0	0/1	1/1	0/0
*/
	case 0x18: /*misc*/
	    /* set TRAPB, EXCB to NEITHER, the way the model really decodes it */
	    if ((((inst >> 11) & 0x18) == 0x00) ||
                (((inst >> 11) & 0x11) == 0x01))
	      predec=NEITHER_X;
	    else
	      predec=IQ_L;
	    break;
	case 0x19: /*HW_MFPR*/
	case 0x1A: /*JSR*/
        case 0x1B: /*HW_LD*/
            predec=IQ_L;
            break;

        case 0x1C: /*new IOP*/
            if ((inst & 0x0800) == 0x0800)         /* only check func<11> for FTOI */
                predec=BOTH_L;
            else if ((inst & 0x0e00) == 0x0c00)
                predec=IQ_L;
            else
                predec=IQ_U;
            break;

        case 0x1D: /*HW_MTPR*/
        case 0x1E: /*HW_RET*/
        case 0x1F: /*HW_ST*/
        case 0x20: /*ldf*/
        case 0x21: /*ldf*/
        case 0x22: /*ldf*/
        case 0x23: /*ldf*/
            predec=IQ_L;
            break;

        case 0x24: /*stf*/
        case 0x25: /*stf*/
        case 0x26: /*stf*/
        case 0x27: /*stf*/
            predec=BOTH_L;
            break;

        case 0x28: /*ldl*/
        case 0x29: /*ldl*/
        case 0x2A: /*ldl*/
        case 0x2B: /*ldl*/
        case 0x2C: /*stl*/
        case 0x2D: /*stl*/
        case 0x2E: /*stl_c*/
        case 0x2F: /*stq_c*/
        case 0x30: /*br*/
            predec=IQ_L;
            break;

        case 0x31: /*fbr*/
        case 0x32: /*fbr*/
        case 0x33: /*fbr*/
            predec=FQ_X;
            break;

        case 0x34: /*br*/
            predec=IQ_L;
            break;

        case 0x35: /*fbr*/
        case 0x36: /*fbr*/
        case 0x37: /*fbr*/
            predec=FQ_X;
            break;

        case 0x38:/*cbr*/
        case 0x39:/*cbr*/
        case 0x3A:/*cbr*/
        case 0x3B:/*cbr*/
        case 0x3C:/*cbr*/
        case 0x3D:/*cbr*/
        case 0x3E:/*cbr*/
        case 0x3F:/*cbr*/
            predec=IQ_U;
            break;

        default:
            predec=NEITHER_X;
            break;
        }
    return(predec);
}  

MTV_uint32 ldic_ic_cout(MTV_uint32 inst, MTV_uint64 pc)
{
    unsigned int inc_byp_bit = 0;
    unsigned int opcode,funcode,disp19_0,pc21_2,sign;

    opcode = (inst>>26) & 0x3f;
    disp19_0 = inst & 0xfffff;
    sign = (inst>>20) & 0x1;
    pc21_2 = (pc >> 2) & 0xfffff;

    inc_byp_bit = (((disp19_0 + pc21_2) + 1) >> 20) & 0x1 & !sign;

    /* return - cout_bit */
    return(inc_byp_bit);
}

MTV_uint32 ldic_easrc(MTV_uint32 inst,
                      MTV_uint32 next_easrc,
                      int trouble_bit)
{
    MTV_uint32 ea_src;
    MTV_uint32 opcode,funcode;

    opcode = (inst >> 26) & 0x3f;
    funcode = (inst >> 5) & 0x7f;

    /* initialize to zero */
    ea_src = 0;

    /* set ea_src for cbr */
    if (opcode == 0x38 ||       /* opcode BLBC */
        opcode == 0x39 ||       /* opcode BEQ */
        opcode == 0x3A ||       /* opcode BLT */
        opcode == 0x3B ||       /* opcode BLE */
        opcode == 0x3C ||       /* opcode BLBS */
        opcode == 0x3D ||       /* opcode BNE */
        opcode == 0x3E ||       /* opcode BGE */
        opcode == 0x3F ||       /* opcode BGT */
        opcode == 0x31 ||       /* opcode FBEQ */
        opcode == 0x32 ||       /* opcode FBLT */
        opcode == 0x33 ||       /* opcode FBLE */
        opcode == 0x35 ||       /* opcode FBNE */
        opcode == 0x36 ||       /* opcode FBGE */
        opcode == 0x37)         /* opcode FBGT */
        ea_src = 0x1;

        /* set ea_src for ret and hw_ret */
    if (((opcode == 0x1A) && (((inst >> 14) & 0x2) == 0x2)) ||  /* opcode JSR (pop bit set, JCO,RET) */
        ((opcode == 0x1E) && (((inst >> 14) & 0x2) == 0x2)))    /* opcode HW_RET (pop bit set) */
        ea_src = 0x2;

        /* override the ea_src with 0x2 if current instr is cmov  */
    if ((opcode == 0x11) && ((funcode & 0x0C) == 0x04))
        ea_src = 0x2;

        /* override the ea_src with 0x2 if current instr is fcmov */
    if ((opcode == 0x17) && ((funcode & 0x08) == 0x08))
        ea_src = 0x2;

        /* set ea_src for br, bsr, jsr and call_pal */
    if (opcode == 0x00 ||       /* CALL_PAL */
        opcode == 0x30 ||       /* opcode BR */
        opcode == 0x34 ||       /* opcode BSR */
        ((opcode == 0x1A) && (((inst >> 14) & 0x2) == 0)) ||     /* opcode JSR, not RET(pop bit clr) */
        ((opcode == 0x1E) && (((inst >> 14) & 0x2) == 0)))       /* opcode HW_RET (pop bit clr) */
        ea_src = 0x3;

        /* override the ea_src with 0x3 if current instr is stq_c, stl_c, hw_stx_c */
    if (((opcode == 0x1F) && (((inst >> 13) & 0x1) == 1)) ||	/* opcode HW_STxC */
        opcode == 0x2f ||
        opcode == 0x2e)
        ea_src = 0x3;
	
        /* set current ea_src to next ea_src if current ea_src equal 0  */
    if ((ea_src == 0x0) && !trouble_bit)
        ea_src = next_easrc;
	
    return ea_src;
}


/* Stall bit is set if:
//	1. this or later instruction is LDx_L or STx_C
//	2. this or later instruction is CMOV/FCMOV
//	3. this or later instruction is MB/WMB (opcode 0x18 w/fcn=0x4xxx)
//	4. 2 or more CBR's show up after (or including) this instruction.
//	5. this instruction is a CBR and an unconditional branch is later in the block.
//
// 4 & 5 basically come down to the following rule: the stall bit is set
// if entering the block at this instruction gives the possibility of
// more than one early exit path from the block.  "early" means that the
// fall through path is not considered.
*/
MTV_uint32 ldic_stall(int inst_num, MTV_uint32 inst, MTV_uint32 next_stall, MTV_uint32* num_early_exit_paths_p, MTV_uint32 trouble_bit)
{           
    unsigned int i;
    unsigned int opcode,funcode;

    opcode = (inst >> 26) & 0x3f;
    funcode = (inst >> 5) & 0x7f;

    /* check stall condition from ld_l */
    if (((opcode == 0x2A)                                 ||  /* opcode LDL_L */
        ((opcode == 0x1b) && (((inst >> 13) & 0x7) == 1)) ||  /* hw_ld locked <15:13> == 1 */
         opcode == 0x2B) &&                                   /* opcode LDQ_L */
        (inst_num != 3))                               	      /* don't stall if it's the 4th instruction in the fetch block */
        return 1;
    
    /* check stall condition from st_c */
    if (((opcode == 0x1F) && ((inst & 0x00002000) == 0x00002000)) ||  /* opcode HW_STxC */
        opcode == 0x2E ||       /* opcode STL_C */
        opcode == 0x2F)         /* opcode STQ_C */
        return 1;
      
    /* check stall condition from CMOV */
    if ((opcode == 0x11) && ((funcode & 0x0C) == 0x04))
        return 1;

    /* check stall condition from FCMOV */
    if ((opcode == 0x17) && ((funcode & 0x08) == 0x08))
        return 1;
    
    /* if CALL_PAL, reset num_exit_paths, and set the stall bit.
    */
    if (opcode == 0x00) {	 /* CALL_PAL */
        *num_early_exit_paths_p = 1;
        return 1;
    }

    /* if unconditional branch, reset num_exit_paths, and don't set the stall bit.
    */
    if ((opcode == 0x1A) ||	 /* JSR */
        (opcode == 0x1E) ||	 /* HW_RET */
        (opcode == 0x30) ||      /* BR */
        (opcode == 0x34)) {       /* BSR */
        *num_early_exit_paths_p = 1;
        return 0;
    }

    if ((opcode == 0x18) &                                      /* MISC */ 
        (inst_num != 3)  &					/* don't stall if it's the 4th instruction in the fetch block */
        trouble_bit)
    {
      return 1;
    }
    
    /* Check for conditional branch.
    */
    if (opcode == 0x38 ||       /* BLBC */
        opcode == 0x39 ||       /* BEQ */
        opcode == 0x3A ||       /* BLT */
        opcode == 0x3B ||       /* BLE */
        opcode == 0x3C ||       /* BLBS */
        opcode == 0x3D ||       /* BNE */
        opcode == 0x3E ||       /* BGE */
        opcode == 0x3F ||       /* BGT */
        opcode == 0x31 ||       /* FBEQ */
        opcode == 0x32 ||       /* FBLT */
        opcode == 0x33 ||       /* FBLE */
        opcode == 0x35 ||       /* FBNE */
        opcode == 0x36 ||       /* FBGE */
        opcode == 0x37)         /* FBGT */
        (*num_early_exit_paths_p)++;
    
    /* set stall bit if find more than one early exit path from this
    // block.
    */
    if (*num_early_exit_paths_p > 1)
        return 1;
    
    return next_stall;
}

MTV_uint32 ldic_trouble(MTV_uint32 inst)
{
    unsigned int trouble_bit = 0;
    unsigned int opcode,funcode;

    opcode = (inst >> 26) & 0x3f;
    funcode = (inst >> 5) & 0x7f;

    /* check trouble condition from ld_l and st_c */
    if (((opcode == 0x1F) && (((inst >> 13) & 0x1) == 1)) ||  /* opcode HW_STxC */
        opcode == 0x2A ||                                     /* opcode LDL_L */
        opcode == 0x2B ||                                     /* opcode LDQ_L */
        ((opcode == 0x1b) && (((inst >> 13) & 0x5) == 1)) ||  /* hw_ld locked ~<15> && <13> */
        opcode == 0x2E ||                                     /* opcode STL_C */
        opcode == 0x2F)                                       /* opcode STQ_C */
        trouble_bit = 1;

    /* check trouble condition from cmov */
    if ((opcode == 0x11) && ((funcode & 0x0C) == 0x04))
        trouble_bit = 1;

    /* check trouble condition from fcmov */
    if ((opcode == 0x17) && ((funcode & 0x08) == 0x08))
        trouble_bit = 1;

    /* check trouble condition from unconditional branch */
    if (opcode == 0x30 ||       /* opcode BR */
        opcode == 0x34)         /* opcode BSR */
        trouble_bit = 1;

    /* check trouble condition from conditional branch */
    if (opcode == 0x38 ||       /* opcode BLBC */
        opcode == 0x39 ||       /* opcode BEQ */
        opcode == 0x3A ||       /* opcode BLT */
        opcode == 0x3B ||       /* opcode BLE */
        opcode == 0x3C ||       /* opcode BLBS */
        opcode == 0x3D ||       /* opcode BNE */
        opcode == 0x3E ||       /* opcode BGE */
        opcode == 0x3F ||       /* opcode BGT */
        opcode == 0x31 ||       /* opcode FBEQ */
        opcode == 0x32 ||       /* opcode FBLT */
        opcode == 0x33 ||       /* opcode FBLE */
        opcode == 0x35 ||       /* opcode FBNE */
        opcode == 0x36 ||       /* opcode FBGE */
        opcode == 0x37)         /* opcode FBGT */
        trouble_bit = 1;

    /* check trouble condition from jsr, ret and call_pal */
    if ((opcode == 0x1A) || (opcode == 0x1E) || (opcode == 0x00))  /* opcode jsr, hw_ret and call_pal */
        trouble_bit = 1;

    /* check trouble condition from misc */
    if ((opcode == 0x18) && (((inst >> 11) & 0x9) == 0x8))
        trouble_bit = 1;
    
    return(trouble_bit);
}
