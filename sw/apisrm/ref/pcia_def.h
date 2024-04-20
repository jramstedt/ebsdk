/* file:	pcia_def.h
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
 * Abstract:	Pcia definitions.
 *
 * Author: William C Clemence 
 *
 * Modifications:
 *
 *	wcc	15-Apr-1994 First release
 *
 *
 */

/* pcia csr register offsets  they are the address >> 5 */
struct PCIA_HPC {
    unsigned long ctl;
    unsigned long mretry;
    unsigned long gpr;
    unsigned long err;
    unsigned long fadr;
    unsigned long imask;
    unsigned long diag;
    unsigned long ipend;
    unsigned long iprog;
    unsigned long wmask_a;
    unsigned long wbase_a;
    unsigned long tbase_a;
    unsigned long wmask_b;
    unsigned long wbase_b;
    unsigned long tbase_b;
    unsigned long wmask_c;
    unsigned long wbase_c;
    unsigned long tbase_c;
    unsigned long errvec;
    unsigned long rsvd[13];
    unsigned long devvec[16];
};

#define PCIA_MAX_HPC 3
#define PCIA_MAX_HPC_DEV_VECTOR 16
#define PCIA_MAX_VECTOR 51
#define PCIA_SIO_PRESENT 0x01000000
#define PCIA_SLOT123_NO_MODULE 0x000000FC

/* These are used with the above struct to get the proper address. */
#define PCIA_HPC0_BASE  0x0
#define PCIA_HPC1_BASE  0x10000
#define PCIA_HPC2_BASE  0x20000

#define PCI_WINDOW_TYPE		0x3
#define PCI_WINDOW_DIRECT	0x2

#define PCIA_SCYCLE	0x100
#define PCIA_IACK	0x104
#define PCIA_PRESENT	0x40000
#define PCIA_TBIT	0x50000
#define PCIA_MCTL	0x60000
#define PCIA_IBR	0x70000

#define PCIA_MAP_BASE	0x80000
#define PCIA_SMALL_MAP_SIZE (128*1024)
#define PCIA_LARGE_MAP_SIZE (512*1024)

#define PCIA_CTL0	0x0
#define PCIA_MRETRY0	0x4
#define PCIA_GPR0	0x8
#define PCIA_ERR0	0xC
#define PCIA_FADR0	0x10
#define PCIA_IMASK0	0x14
#define PCIA_DIAG0	0x18
#define PCIA_IPEND0	0x1C
#define PCIA_IPROG0	0x20
#define PCIA_WMASK_A0	0x24
#define PCIA_WBASE_A0	0x28
#define PCIA_TBASE_A0	0x2C
#define PCIA_WMASK_B0	0x30
#define PCIA_WBASE_B0	0x34
#define PCIA_TBASE_B0	0x38
#define PCIA_WMASK_C0	0x3C
#define PCIA_WBASE_C0	0x40
#define PCIA_TBASE_C0	0x44
#define PCIA_ERRVEC0	0x48
#define PCIA_DEVVEC0A0	0x80
#define PCIA_DEVVEC0B0	0x84
#define PCIA_DEVVEC0C0	0x88
#define PCIA_DEVVEC0D0	0x8C
#define PCIA_DEVVEC1A0	0x90
#define PCIA_DEVVEC1B0	0x94
#define PCIA_DEVVEC1C0	0x98
#define PCIA_DEVVEC1D0	0x9C
#define PCIA_DEVVEC2A0	0xA0
#define PCIA_DEVVEC2B0	0xA4
#define PCIA_DEVVEC2C0	0xA8
#define PCIA_DEVVEC2D0	0xAC
#define PCIA_DEVVEC3A0	0xB0
#define PCIA_DEVVEC3B0	0xB4
#define PCIA_DEVVEC3C0	0xB8
#define PCIA_DEVVEC3D0	0xBC
#define PCIA_CTL1	0x10000
#define PCIA_MRETRY1	0x10004
#define PCIA_GPR1	0x10008
#define PCIA_ERR1	0x1000C
#define PCIA_FADR1	0x10010
#define PCIA_IMASK1	0x10014
#define PCIA_DIAG1	0x10018
#define PCIA_IPEND1	0x1001C
#define PCIA_IPROG1	0x10020
#define PCIA_WMASK_A1	0x10024
#define PCIA_WBASE_A1	0x10028
#define PCIA_TBASE_A1	0x1002C
#define PCIA_WMASK_B1	0x10030
#define PCIA_WBASE_B1	0x10034
#define PCIA_TBASE_B1	0x10038
#define PCIA_WMASK_C1	0x1003C
#define PCIA_WBASE_C1	0x10040
#define PCIA_TBASE_C1	0x10044
#define PCIA_ERRVEC1	0x10048
#define PCIA_DEVVEC0A1	0x10080
#define PCIA_DEVVEC0B1	0x10084
#define PCIA_DEVVEC0C1	0x10088
#define PCIA_DEVVEC0D1	0x1008C
#define PCIA_DEVVEC1A1	0x10090
#define PCIA_DEVVEC1B1	0x10094
#define PCIA_DEVVEC1C1	0x10098
#define PCIA_DEVVEC1D1	0x1009C
#define PCIA_DEVVEC2A1	0x100A0
#define PCIA_DEVVEC2B1	0x100A4
#define PCIA_DEVVEC2C1	0x100A8
#define PCIA_DEVVEC2D1	0x100AC
#define PCIA_DEVVEC3A1	0x100B0
#define PCIA_DEVVEC3B1	0x100B4
#define PCIA_DEVVEC3C1	0x100B8
#define PCIA_DEVVEC3D1	0x100BC
#define PCIA_CTL2	0x20000
#define PCIA_MRETRY2	0x20004
#define PCIA_GPR2	0x20008
#define PCIA_ERR2	0x2000C
#define PCIA_FADR2	0x20010
#define PCIA_IMASK2	0x20014
#define PCIA_DIAG2	0x20018
#define PCIA_IPEND2	0x2001C
#define PCIA_IPROG2	0x20020
#define PCIA_WMASK_A2	0x20024
#define PCIA_WBASE_A2	0x20028
#define PCIA_TBASE_A2	0x2002C
#define PCIA_WMASK_B2	0x20030
#define PCIA_WBASE_B2	0x20034
#define PCIA_TBASE_B2	0x20038
#define PCIA_WMASK_C2	0x2003C
#define PCIA_WBASE_C2	0x20040
#define PCIA_TBASE_C2	0x20044
#define PCIA_ERRVEC2	0x20048
#define PCIA_DEVVEC0A2	0x20080
#define PCIA_DEVVEC0B2	0x20084
#define PCIA_DEVVEC0C2	0x20088
#define PCIA_DEVVEC0D2	0x2008C
#define PCIA_DEVVEC1A2	0x20090
#define PCIA_DEVVEC1B2	0x20094
#define PCIA_DEVVEC1C2	0x20098
#define PCIA_DEVVEC1D2	0x2009C
#define PCIA_DEVVEC2A2	0x200A0
#define PCIA_DEVVEC2B2	0x200A4
#define PCIA_DEVVEC2C2	0x200A8
#define PCIA_DEVVEC2D2	0x200AC
#define PCIA_DEVVEC3A2	0x200B0
#define PCIA_DEVVEC3B2	0x200B4
#define PCIA_DEVVEC3C2	0x200B8
#define PCIA_DEVVEC3D2	0x200BC

/* PCIA register console initialization values */

#define PCIA_LMAP_CONTROL_VALUE 0x07e00100  /* 3 up buffers, MM prefetch 4, MM enabled, cut enable, cut thresh 0 */
#define PCIA_SMAP_CONTROL_VALUE 0x01e00100  /* 3 up buffers, MM prefetch 4, MM enabled, cut enable, cut thresh 0 */
#define PCIA_MRETRY_VALUE  0x400000    /* approximately 1 second */
#define PCIA_IMASK_VALUE   0x01020000  /* error IPL 17, device IPL 14, disable error interrupts */
#define PCIA_ERROR_VALUE   0x0003ffff  /* clear all errors */

/* pcia mailbox definitions */
#define MBOX_CSR_BASE	0x4000000
#define MBOX_PCIA_GPR0	0x4000008
#define MBOX_PCI_RD	9
#define MBOX_PCI_WR	10

#define PCIA_TEST_MODE 5
#define PCIA_NON_TEST_MODE 6

struct HPCLIST {
    int tlsbslot;
    int hose;
    unsigned __int64 bb;		/* base address */
    struct device *dev;			/* pci device structure */
};

