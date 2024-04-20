/* file:    tms380.h
 *
 * Copyright (C) 1992 by
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
 */


/*
 *++
 *  FACILITY:
 *
 *      Mikasa Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *  This module contains the structure definitions/bitmasks/constants
 *  used to operate the TMS380 Token Ring controller.
 *
 * AUTHOR:  Kirt Gillum, modified by Lowell Kopp
 *
 * MODIFICATION HISTORY:
 *
 *  lek 14-Oct-1994 Initial entry. Derived from TMS380.h written by Kirt Gillum
 *--
 */

/*
 *
 * IMPORTANT NOTE:
 * ===============
 *
 * The tms380 operates in MC68K mode which means we must byte swap the values
 * in both the SCB and SSB.  This does not apply to xmit & receive buffers.
 */

#define bswp16(w)  ( ((w&0xff)<<8) | ((w>>8)&0xff) )
#define bswp32(l)  ( (bswp16(l)) | (bswp16(l>>16)<<16) )
#define cnv32(l)   ( (bswp16(l)<<16) | (bswp16((l)>>16)) )

#define htons( w ) bswp16( w )
#define ntohs( w ) bswp16( w )
#define htonl( l ) cnv32( l )
#define ntohl( l ) cnv32( l )

/*
 * SIFACL, SIFADR, SIFCMD, SIFSTS (etc) CSR definitions
 * ----------------------------------------------------
 *
 *   The following symbols must be defined.
 *
 *      CSR_BASE        \  The base offset of the CSR set from the start
 *      CSR_SIZE        /  of the bus's option address.
 *
 *      ROM_BASE        \  If TMS380 has onboard code ROM/Flash, the
 *      ROM_SIZE        /  address/size is specified with these constants.
 *
 *      SIFDAT_BASE     \
 *      SIFDIC_BASE      \
 *      SIFAD1_BASE       \
 *      SIFCMD_BASE        \
 *      SIFSTS_BASE         > TMS380 defined registers
 *      SIFACL_BASE        /
 *      SIFADR_BASE       /
 *      SIFADX_BASE      /
 *      DMALEN_BASE     /
 *
 *      RESET_BASE      \
 *      SPEED_BASE       > Hardware defined registers (some optional)
 *      LED_BASE        /
 *
 */


        /*
         * These constants depend on two local variables 'shift' and 'word'
         * to contain the word mask and shift count for converting the offsets
         * to actual bus offsets.
         *
         */

#define CSR_BASE        0

#if FALSE
#define CSR_BASE        (((0x00<<shift)|word)/sizeof(unsigned int))
#define CSR_SIZE        ((0x10<<shift)|word)

#define SIFDAT_BASE     (((0x00<<shift)|word)/sizeof(unsigned int))
#define SIFINC_BASE     (((0x02<<shift)|word)/sizeof(unsigned int))
#define SIFAD1_BASE     (((0x04<<shift)|word)/sizeof(unsigned int))
#define SIFCMD_BASE     (((0x06<<shift)|word)/sizeof(unsigned int))
#define SIFSTS_BASE     (((0x06<<shift)|word)/sizeof(unsigned int))
#define SIFACL_BASE     (((0x08<<shift)|word)/sizeof(unsigned int))
#define SIFADR_BASE     (((0x0a<<shift)|word)/sizeof(unsigned int))
#define SIFADX_BASE     (((0x0c<<shift)|word)/sizeof(unsigned int))
#define DMALEN_BASE     (((0x0e<<shift)|word)/sizeof(unsigned int))
#endif

    /*
     * EISA Configuration Register values
     */

#define CONFIG_BASE     (((0xc84<<shift)|word)/sizeof(unsigned int))
#define ZC86_OFFSET \
        (((0xc86<<shift)|tlsb->lsb$l_tr_byte)/sizeof(unsigned int))
#define ZC87_OFFSET \
        (((0xc87<<shift)|tlsb->lsb$l_tr_byte)/sizeof(unsigned int))
#define DW300_M_ENABLE  0x0001
#define DW300_M_RAM     0x0008
#define DW300_M_SPEED   0x0400
#define DW300_M_MEDIA   0x0800
#define DW300_M_INTEDG  0x1000
#define DW300_M_IRQ     0xE000
#define DW300_C_IRQ5    0x2000
#define DW300_C_IRQ9    0x4000
#define DW300_C_IRQ10   0x6000
#define DW300_C_IRQ11   0x8000
#define DW300_C_IRQ15   0xa000

        /*
         * ISA Configuration Registers / Values
         */

#define P1392_IRQARM    ((0x10<<shift)/sizeof(unsigned int))
#define P1392_PCSR      ((0x11<<shift)/sizeof(unsigned int))
#define P1392_SRAMMAP   ((0x12<<shift)/sizeof(unsigned int))
#define P1392_INTDMA    ((0x13<<shift)/sizeof(unsigned int))
#define P1392_MISC      ((0x14<<shift)/sizeof(unsigned int))
#define P1392_ID        (((0x1E<<shift)|word)/sizeof(unsigned int))

#define P1392_M_8MHZ    0x0     /* INT/DMA bits/values */
#define P1392_M_4MHZ    0x80
#define P1392_M_16MHZ   0x40
#define P1392_M_SYSCLK  0xC0
#define P1392_C_IRQ7    0x0
#define P1392_C_IRQ6    0x1
#define P1392_C_IRQ5    0x2
#define P1392_C_IRQ4    0x3
#define P1392_C_IRQ3    0x4
#define P1392_C_IRQ12   0x5
#define P1392_C_IRQ11   0x6
#define P1392_C_IRQ10   0x7
#define P1392_C_IRQ9    0x8

#define P1392_M_SIZE16  0x00    /* PCSR bits */
#define P1392_M_BMASTER 0x00
#define P1392_M_RESET   0x04
#define P1392_M_INTEN   0x08
#define P1392_M_SAEN    0x10
#define P1392_M_SPEED_4 0x20
#define P1392_M_SPEED16 0x00
#define P1392_M_BUSRL   0x40
#define P1392_M_MED_UTP 0x00
#define P1392_M_MED_STP 0x80

#if FALSE   /* may need for 1392 */
#define READ_IRQARM(lx, in) in = \
        *((LSB *)(lx))->lsb$a_tr_irqarm
#define WRITE_PCSR(lx, out) *((LSB *)(lx))->lsb$a_tr_pcsr = ((out) << 8)
#define READ_PCSR(lx, in) in = \
        (*((LSB *)(lx))->lsb$a_tr_pcsr) >> 8
#define WRITE_SRAMMAP(lx, out) *((LSB *)(lx))->lsb$a_tr_srammap = ((out) << 16)
#define READ_SRAMMAP(lx, in) in = \
        (*((LSB *)(lx))->lsb$a_tr_srammap) >> 16
#define WRITE_INTDMA(lx, out) *((LSB *)(lx))->lsb$a_tr_intdma = ((out) << 24)
#define READ_INTDMA(lx, in) in = \
        (*((LSB *)(lx))->lsb$a_tr_intdma) >> 24
#define WRITE_MISC(lx, out)    *((LSB *)(lx))->lsb$a_tr_misc = out
#define READ_MISC(lx, in) in = \
        *((LSB *)(lx))->lsb$a_tr_misc
#define READ_ID(lx, in) in = \
        (*((LSB *)(lx))->lsb$a_tr_id) >> 16
#endif

        /*
         * DW300 MMR reg definition.
         *
         * For some systems, the dma addresses may be in the upper 4 gig range
         * due to the use of mapping registers.  The DW300 can only directly
         * address 30 bits worth of addresses.
         *
         * Note; this device cannot operate on systems that can frequently
         * have dma addresses that change bit30 & 31.   Most systems with
         * mapping regs will have dma addresses that don't change bit30 & 31.
         */
#define DW300_M_BIT30   0x55
#define DW300_M_BIT31   0xAA

#define MMR_BASE (((0x080<<tlsb->lsb$l_tr_shift)|tlsb->lsb$l_tr_byte)/sizeof(unsigned int))

    /*
     * Init Command Vector Assigments.
     */

#define CMDXMT_VEC 0x0      /* Not required for DW300 */
#define RCVSTS_VEC 0x0
#define SCBCHK_VEC 0x0
        /*
         * Extra DMA options
         */

#define DMA_OPTS \
   INIT_M_BRSTSB|INIT_M_BRSTLST|INIT_M_BRSTLSS|INIT_M_BRSTRCV|INIT_M_BRSTXMT
#define DMA_RCV 20
#define DMA_XMT 20

    /*
     * CSR access.
     */
#if FALSE   /* using a different access mech, eisa_readx(), eisa_writex() */
#define WRITE_SIFDAT(lx, out)   *((LSB *)(lx))->lsb$a_tr_sifdat = out
#define WRITE_SIFINC(lx, out)   *((LSB *)(lx))->lsb$a_tr_sifinc = \
        ((out) << 16)
#define READ_SIFINC(lx, in)     in = \
        (*((LSB *)(lx))->lsb$a_tr_sifinc) >> 16
#define READ_FAST_SIFINC(in)    in = *sifinc >> 16
#define WRITE_FAST_SIFINC(out)  *sifinc = ((out) << 16)
#define WRITE_SIFCMD(lx, out)   *((LSB *)(lx))->lsb$a_tr_sifcmd = \
        ((out) << 16)
#define READ_SIFSTS(lx, in)     in = \
        (*((LSB *)(lx))->lsb$a_tr_sifsts) >> 16
#define WRITE_SIFACL(lx, out)   *((LSB *)(lx))->lsb$a_tr_sifacl = out
#define READ_SIFACL(lx, in)     in = *((LSB *)(lx))->lsb$a_tr_sifacl
#define WRITE_SIFADR(lx, out)   *((LSB *)(lx))->lsb$a_tr_sifadr = \
        ((out) << 16)
#define WRITE_SIFADX(lx, out)   *((LSB *)(lx))->lsb$a_tr_sifadx = out
#define WRITE_DMALEN(lx, out)   *((LSB *)(lx))->lsb$a_tr_dmalen = \
        ((out) << 16)

/*
 * Macros for manipulating the DMA controller. (From Walt Arbo)
 * modified for use...
 */

#define put_lane(_data,_offset) {_data <<= ((_offset & 3) * 8);}

/*
 * Write to the DMA engine (Intel 82357 or 82378IB).  This macro copies
 * the input data into a temporary variable, and shifts it into
 * the correct byte lane.  The original data is not modified.
 * The value must be in bits 7:0 of _data.
 *
 */
#define write_dma( _adp, _iohandle, _reg_offset, _data )                \
{                                                                       \
    int _status,__data;                                                 \
    __data=_data;                                                       \
    put_lane(__data, _reg_offset);                                      \
    _status = ioc$write_io (_adp,                                       \
                            _iohandle,                                  \
                            _reg_offset,                                \
                            1,                                          \
                            &__data );                                  \
}
#endif

