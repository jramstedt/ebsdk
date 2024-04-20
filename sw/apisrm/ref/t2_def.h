/*
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
 * and  should  not  be  construed  as a commitment by Digital Equipment
 * Corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by Digital.
 *
 *
 * Abstract:	T2 CSR Definitions
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *	sfs	20-May-1994	Initial entry.
 */

#if GAMMA
#define sio_base_addr (((three+0x80)<<32)+(0x8e000000))
#else
#define sio_base_addr ((three<<32)+(0x8e000000))
#endif
#define io_base_off 0x1000000
#define xio_base_addr (sio_base_addr+io_base_off)
#define SIO_CSR_BASE sio_base_addr
#define XIO_CSR_BASE xio_base_addr
#define IO_CSR_BASE(s) (sio_base_addr+(s?io_base_off:0))
#define READ_SIO_CSR(c) (*(volatile unsigned __int64 *)(SIO_CSR_BASE+c))
#define READ_XIO_CSR(c) (*(volatile unsigned __int64 *)(XIO_CSR_BASE+c))
#define READ_IO_CSR(s,c) (*(volatile unsigned __int64 *)(IO_CSR_BASE(s)+c))
#define WRITE_SIO_CSR(c,v) (*(volatile unsigned __int64 *)(SIO_CSR_BASE+c) = v)
#define WRITE_XIO_CSR(c,v) (*(volatile unsigned __int64 *)(XIO_CSR_BASE+c) = v)
#define WRITE_IO_CSR(s,c,v) (*(volatile unsigned __int64 *)(IO_CSR_BASE(s)+c) = v)
#define _READ_IO_CSR(c) (*(volatile unsigned __int64 *)(p_io_csr+c))
#define _WRITE_IO_CSR(c,v) (*(volatile unsigned __int64 *)(p_io_csr+c) = v)
#define IOCSR_ADDR 0x0
#define CERR1_ADDR 0x20
#define CERR2_ADDR 0x40
#define CERR3_ADDR 0x60
#define PERR1_ADDR 0x80
#define PERR2_ADDR 0xa0
#define PSCR_ADDR 0xc0
#define HAEO_1_ADDR 0xe0
#define HAEO_2_ADDR 0x100
#define HBASE_ADDR 0x120
#define WBASE1_ADDR 0x140
#define WMASK1_ADDR 0x160
#define TBASE1_ADDR 0x180
#define WBASE2_ADDR 0x1a0
#define WMASK2_ADDR 0x1c0
#define TBASE2_ADDR 0x1e0
#define TLBBR_ADDR 0x200
#define IVRPR_ADDR 0x220
#define IVIAR_ADDR 0x220
#define HAEO_3_ADDR 0x240
#define HAEO_4_ADDR 0x260
#define WBASE3_ADDR 0x280
#define WMASK3_ADDR 0x2a0
#define TBASE3_ADDR 0x2c0
#define TDR0_ADDR 0x300
#define TDR1_ADDR 0x320
#define TDR2_ADDR 0x340
#define TDR3_ADDR 0x360
#define TDR4_ADDR 0x380
#define TDR5_ADDR 0x3a0
#define TDR6_ADDR 0x3c0
#define TDR7_ADDR 0x3e0
#define WBASE4_ADDR 0x400
#define WMASK4_ADDR 0x420
#define TBASE4_ADDR 0x440
#define AIR_ADDR 0x460
#define VAR_ADDR 0x480
#define DIR_ADDR 0x4a0
#define ICIC_ADDR 0x4c0

#define ICIC_MASK 0x40
#define ICIC_ELCR 0x50
#define ICIC_EISA 0x60
#define ICIC_MODE 0x70
