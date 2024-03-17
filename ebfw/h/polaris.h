#ifndef __POLARIS_H_LOADED
#define __POLARIS_H_LOADED
/*****************************************************************************

       Copyright 1993, 1994, 1995 Digital Equipment Corporation,
                       Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, provided  
that the copyright notice and this permission notice appear in all copies  
of software and supporting documentation, and that the name of Digital not  
be used in advertising or publicity pertaining to distribution of the software 
without specific, written prior permission. Digital grants this permission 
provided that you prominently mark, as not part of the original, any 
modifications made to this software or documentation.

Digital Equipment Corporation disclaims all warranties and/or guarantees  
with regard to this software, including all implied warranties of fitness for 
a particular purpose and merchantability, and makes no representations 
regarding the use of, or the results of the use of, the software and 
documentation in terms of correctness, accuracy, reliability, currentness or
otherwise; and you rely on the software, documentation and results solely at 
your own risk. 

******************************************************************************/

/*
 *  $Id: polaris.h,v 1.1.1.1 1998/12/29 21:36:07 paradis Exp $;
 */

/*
 * $Log: polaris.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:07  paradis
 * Initial CVS checkin
 *
 * Revision 1.2  1997/05/01  20:05:49  pbell
 * *** empty log message ***
 *
 * Initial revision
 *
 *
 */

#define PSC_IDSEL           11          /* Polaris use PCI AD11 */

/* 
 *  Polaris Register Offset 
 */
#define PSC_COMMAND_REG     0x04        /* W - Command Register */
#define PSC_STATUS_REG      0x06        /* W - Status Register */
#define PSC_PCICTL0         0x42        /* B - PCI Control Register 0 */
#define PSC_PCICTL1         0x43        /* B - PCI Control Register 1 */
#define PSC_PCICTL2         0x44        /* B - PCI Control Register 2 */
#define PSC_I2CCTL          0x45        /* B - PCI Control Register 3 */
#define PSC_RAMCFG0         0x50        /* B - DRAM Bank Config Register 0 */
#define PSC_RAMCFG1         0x51        /* B - DRAM Bank Config Register 1 */
#define PSC_RAMCFG2         0x52        /* B - DRAM Bank Config Register 2 */
#define PSC_RAMCFG3         0x53        /* B - DRAM Bank Config Register 3 */
#define PSC_CASCTL0         0x54        /* B - DRAM CAS Control Register 0 */
#define PSC_CASCTL1         0x55        /* B - DRAM CAS Control Register 1 */
#define PSC_CASCTL2         0x56        /* B - DRAM CAS Control Register 2 */
#define PSC_CASCTL3         0x57        /* B - DRAM CAS Control Register 3 */
#define PSC_RAMCTL          0x58        /* B - DRAM Misc Control Register */
#define PSC_RASCTL          0x59        /* B - DRAM RAS Control Register */
#define PSC_HOSTCTL         0x5a        /* B - Host Interface Control Register */
#define PSC_SDCTL           0x5b        /* B - SDRAM Control Register */
#define PSC_WBCTL           0x5c        /* B - DRAM Write Buffer Control Register */
#define PSC_IRQCTL          0x6c        /* B - Serial Interrupt Bus Control Register */
#define PSC_DIER_L          0x70        /* LW - Device Interrupt Enable Register */
#define PSC_DIER_H          0x74        /* LW - Device Interrupt Enable Register */
#define PSC_DRIRR_L         0x78        /* LW - Device Interrupt Raw Request Register */
#define PSC_DRIRR_H         0x7c        /* LW - Device Interrupt Raw Request Register */
#define PSC_DIRR_L          0x80        /* LW - Device Interrupt Masked Request Register */
#define PSC_DIRR_H          0x84        /* LW - Device Interrupt Masked Request Register */
#define PSC_DVCRR_L         0x88        /* LW - Device Interrupt Vector Request Register */
#define PSC_DVCRR_H         0x8c        /* LW - Device Interrupt Vector Request Register */


/* ======================================================================
 * =   		BIT EXTENT FOR PSC_ REGISTERS                               =
 * ======================================================================
*/

#define PSC_COMMAND_REG_V_MEM_ENB   1   /* PCI access to PSC's memory space */
#define PSC_COMMAND_REG_M_MEM_ENB   (1 << PSC_COMMAND_REG_V_MEM_ENB)
#define PSC_COMMAND_REG_V_SERR_ENB  1   /* SERR# enable */
#define PSC_COMMAND_REG_M_SERR_ENB  (1 << PSC_COMMAND_REG_V_SERR_ENB)
#define PSC_COMMAND_REG_V_FBACK_ENB 1   /* Fast back-to-back enable */
#define PSC_COMMAND_REG_M_FBACK_ENB (1 << PSC_COMMAND_REG_V_FBACK_ENB)

#define PSC_PCICTL0_V_PM            0   /* Arbitration parking */
#define PSC_PCICTL0_M_PM            (1 << PSC_PCICTL0_V_PM)
#define PSC_PCICTL0_V_IOBRST_EN     1   /* IO Burst Enable */
#define PSC_PCICTL0_M_IOBRST_EN     (1 << PSC_PCICTL0_M_IOBRST_EN)
#define PSC_PCICTL0_V_SWRST         2   /* Software Reset */
#define PSC_PCICTL0_M_SWRST         (1 << PSC_PCICTL0_V_SWRST)

#define PSC_PCICTL1_V_SBLT_DIS      0   /* PCI-DRAM 8 clock burst cycle latency timeout */
#define PSC_PCICTL1_M_SBLT_DIS      (1 << PSC_PCICTL1_V_SBLT_DIS)
#define PSC_PCICTL1_V_SILT_DIS      1   /* PCI-DRAM 32 clock initial cycle latency timeout */
#define PSC_PCICTL1_M_SILT_DIS      (1 << PSC_PCICTL1_V_SILT_DIS)
#define PSC_PCICTL1_V_SRA_DIS       2   /* PCI-DRAM Read-Ahead acrosss multiple transactions */
#define PSC_PCICTL1_M_SRA_DIS       (1 << PSC_PCICTL1_V_SRA_DIS)
#define PSC_PCICTL1_V_SCRA_DIS      3   /* PCI-DRAM Continue Read-Ahead after disconnect */
#define PSC_PCICTL1_M_SCRA_DIS      (1 << PSC_PCICTL1_V_SCRA_DIS)
#define PSC_PCICTL1_V_DUALMD_EN     4   /* Dual Pin Arbitration Enable */
#define PSC_PCICTL1_M_DUALMD_EN     (1 << PSC_PCICTL1_V_DUALMD_EN)
#define PSC_PCICTL1_V_512KHOLE_DIS  5   /* 512K Hole Disable */
#define PSC_PCICTL1_M_512KHOLE_DIS  (1 << PSC_PCICTL1_V_512KHOLE_DIS)
#define PSC_PCICTL1_V_INTERR_EN     6   /* Interrupt Errpr Enable */
#define PSC_PCICTL1_M_INTERR_EN     (1 << PSC_PCICTL1_V_INTERR_EN)
#define PSC_PCICTL1_V_DRM_ERR_EN    7   /* DRAM Parity Error Enable */
#define PSC_PCICTL1_M_DRM_ERR_EN    (1 << PSC_PCICTL1_V_DRM_ERR_EN)

#define PSC_PCICTL2_V_INTERR_STAT   6   /* Interrupt Error Status */
#define PSC_PCICTL2_M_INTERR_STAT   (1 << PSC_PCICTL2_V_INTERR_STAT)
#define PSC_PCICTL2_V_DRM_PAR_STAT  7   /* DRAM Parity Status */
#define PSC_PCICTL2_M_DRM_PAR_STAT  (1 << PSC_PCICTL2_V_DRM_PAR_STAT)

#define PSC_I2CCTL_V_I2CCLKOUT      0   /* Drive I2CCLK output */
#define PSC_I2CCTL_M_I2CCLKOUT      (1 << PSC_I2CCTL_V_I2CCLKOUT)
#define PSC_I2CCTL_V_I2CDATOUT      1   /* Drive I2CDAT output */
#define PSC_I2CCTL_M_I2CDATOUT      (1 << PSC_I2CCTL_V_I2CDATOUT)
#define PSC_I2CCTL_V_I2CCLKIN       2   /* State of the I2CCLK pad */
#define PSC_I2CCTL_M_I2CCLKIN       (1 << PSC_I2CCTL_V_I2CCLKIN)
#define PSC_I2CCTL_V_I2CDATIN       3   /* State of the I2CDAT pad */
#define PSC_I2CCTL_M_I2CDATIN       (1 << PSC_I2CCTL_V_I2CDATIN)

#define PSC_CASCTL_V_TWLASC         0   
#define PSC_CASCTL_M_TWLASC         (1 << PSC_CASCTL_V_TWLASC)
#define PSC_CASCTL_V_TWBASC         1  
#define PSC_CASCTL_M_TWBASC         (1 << PSC_CASCTL_V_TWBASC)
#define PSC_CASCTL_V_TWCAS          2   /* CAS pulse width for writes */
#define PSC_CASCTL_M_TWCAS          (1 << PSC_CASCTL_V_TWCAS)
#define PSC_CASCTL_V_TRLASC         3
#define PSC_CASCTL_M_TRLASC         (1 << PSC_CASCTL_V_TRLASC)
#define PSC_CASCTL_V_TRCAS          4
#define PSC_CASCTL_M_TRCAS          (3 << PSC_CASCTL_V_TRCAS)
#define PSC_CASCTL_V_RAMTYP         6
#define PSC_CASCTL_M_RAMTYP         (3 << PSC_CASCTL_V_RAMTYP)

#define PSC_RAMCTL_V_WTDIS          0   /* DRAM Write Inhibit */
#define PSC_RAMCTL_M_WTDIS          (1 << PSC_RAMCTL_V_WTDIS)
#define PSC_RAMCTL_V_REFRATE        1   /* Refresh Rate Selector */
#define PSC_RAMCTL_M_REFRATE        (1 << PSC_RAMCTL_V_REFRATE)
#define PSC_RAMCTL_V_REREF_EN       2   /* Refresh Enabled */
#define PSC_RAMCTL_M_REREF_EN       (1 << PSC_RAMCTL_V_REREF_EN)
#define PSC_RAMCTL_V_PAGE_EN        3   /* Page Mode Enable */
#define PSC_RAMCTL_M_PAGE_EN        (1 << PSC_RAMCTL_V_PAGE_EN)
#define PSC_RAMCTL_V_DMB_IBC        4   /* Dumb IDLEBC */
#define PSC_RAMCTL_M_DMB_IBC        (1 << PSC_RAMCTL_V_DMB_IBC)
#define PSC_RAMCTL_V_AUTOCLS        5   /* Auto Page Closure for SDRAM */
#define PSC_RAMCTL_M_AUTOCLS        (1 << PSC_RAMCTL_V_AUTOCLS)
#define PSC_RAMCTL_V_ODDPAR         6   /* Odd/Even Parity */
#define PSC_RAMCTL_M_ODDPAR         (1 << PSC_RAMCTL_V_ODDPAR)
#define PSC_RAMCTL_V_ADRTST         7   /* DRAM Address Decoder Test Mode */
#define PSC_RAMCTL_M_ADRTST         (1 << PSC_RAMCTL_V_ADRTST)

#define PSC_RASCTL_V_TRCD           0   /* RAS - CAS delay */
#define PSC_RASCTL_M_TRCD           (3 << PSC_RASCTL_V_TRCD)
#define PSC_RASCTL_V_TRP            2   /* RAS Precharge time */
#define PSC_RASCTL_M_TRP            (3 << PSC_RASCTL_V_TRP)
#define PSC_RASCTL_V_TRAH           4   /* Row Addr hold time */
#define PSC_RASCTL_M_TRAH           (1 << PSC_RASCTL_V_TRAH)

#define PSC_HOSTCTL_V_BOOT_DIS      0   /* Boot Mode Disable */
#define PSC_HOSTCTL_M_BOOT_DIS      (1 << PSC_HOSTCTL_V_BOOT_DIS)
#define PSC_HOSTCTL_V_REG_FLOW      1   /* reg-flow thru SRAM */
#define PSC_HOSTCTL_M_REG_FLOW      (1 << PSC_HOSTCTL_V_REG_FLOW)
#define PSC_HOSTCTL_V_BYTE_WORD     2   /* byte/word decode */
#define PSC_HOSTCTL_M_BYTE_WORD     (1 << PSC_HOSTCTL_V_BYTE_WORD)
#define PSC_HOSTCTL_V_IDLEBC_DLY    3   /* IDLEBC to FILL delay */
#define PSC_HOSTCTL_M_IDLEBC_DLY    (0xf << PSC_HOSTCTL_V_IDLEBD_DLY)
#define PSC_HOSTCTL_V_SLOW_BC       7   /* delay read with victim by 1 clock */
#define PSC_HOSTCTL_M_SLOW_BC       (1 << PSC_HOSTCTL_V_SLOW_BC)

#define PSC_SDCTL_V_SDCL            0   /* SDRAM CAS Read Latency */
#define PSC_SDCTL_M_SDCL            (1 << PSC_SDCTL_V_SDCL)
#define PSC_SDCTL_V_TRWL            1   /* Min Last Data in to Precharge */
#define PSC_SDCTL_M_TRWL            (1 << PSC_SDCTL_V_TRWL)
#define PSC_SDCTL_V_TRAS            2   /* Row Active Time */
#define PSC_SDCTL_M_TRAS            (3 << PSC_SDCTL_V_TRAS)
#define PSC_SDCTL_V_TRC             4   /* Row Cycle Clock Count */
#define PSC_SDCTL_M_TRC             (1 << PSC_SDCTL_V_TRC)
#define PSC_SDCTL_V_DIS2N           5   /* Ignore SDRAM 2N rule */
#define PSC_SDCTL_M_DIS2N           (1 << PSC_SDCTL_V_DIS2N)
#define PSC_SDCTL_V_MRSCMD          6   /* Issue Mode Register Set Command */
#define PSC_SDCTL_M_MRSCMD          (1 << PSC_SDCTL_V_MRSCMD)
#define PSC_SDCTL_V_SDRAM_EN        7   /* SDRAM Enable */
#define PSC_SDCTL_M_SDRAM_EN        (1 << PSC_SDCTL_V_SDRAM_EN)

#define PSC_WBCTL_V_FWF             7   /* Flush when full */
#define PSC_WBCTL_M_FWF             (1 << PSC_WBCTL_V_FWF)

#define PSC_IRQCTL_V_SMPL_EN        0   /* Serial Bus Enable */
#define PSC_IRQCTL_M_SMPL_EN        (1 << PSC_IRQCTL_V_SMPL_EN)
#define PSC_IRQCTL_V_QUIET_EN       1   /* Serial Bus Operating Mode */
#define PSC_IRQCTL_M_QUIET_EN       (1 << PSC_IRQCTL_V_QUIET_EN)
#define PSC_IRQCTL_V_ST_SIZE        2   /* Start Pulse Size */
#define PSC_IRQCTL_M_ST_SIZE        (3 << PSC_IRQCTL_V_ST_SIZE)


/* DRAM Configuration for EDO/FP */
#define RAMCFG_EDO_0MB              0x00
#define RAMCFG_EDO_16MB_A           0x02
#define RAMCFG_EDO_16MB_S           0x04
#define RAMCFG_EDO_32MB_A1          0x08
#define RAMCFG_EDO_32MB_A2          0x10
#define RAMCFG_EDO_64MB_A           0x20
#define RAMCFG_EDO_64MB_S           0x40
#define RAMCFG_EDO_256MB_S          0x80

/* DRAM Configuration for SDRAM */
#define RAMCFG_SDRAM_0MB            0x00
#define RAMCFG_SDRAM_16MB           0x01
#define RAMCFG_SDRAM_32MB_1         0x02
#define RAMCFG_SDRAM_32MB_2         0x04
#define RAMCFG_SDRAM_64MB           0x08
#define RAMCFG_SDRAM_128MB          0x10

#endif /* __POLARIS_H_LOADED */
