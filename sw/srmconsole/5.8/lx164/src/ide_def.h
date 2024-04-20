/********************************************************************************************************************************/
/* Created: 21-JUN-2000 10:42:01 by OpenVMS SDL EV1-33     */
/* Source:  21-JAN-1994 13:46:46 AFW_USER$:[CONSOLE.V58.COMMON.SRC]IDE_DEF.SDL;1 */
/********************************************************************************************************************************/
/*                                                                          */
/* Copyright (C) 1992 by                                                    */
/* Digital Equipment Corporation, Maynard, Massachusetts.                   */
/* All rights reserved.                                                     */
/*                                                                          */
/* This software is furnished under a license and may be used and copied    */
/* only  in  accordance  of  the  terms  of  such  license  and with the    */
/* inclusion of the above copyright notice. This software or  any  other    */
/* copies thereof may not be provided or otherwise made available to any    */
/* other person.  No title to and  ownership of the  software is  hereby    */
/* transferred.                                                             */
/*                                                                          */
/* The information in this software is  subject to change without notice    */
/* and  should  not  be  construed  as a commitment by Digital Equipment    */
/* Corporation.                                                             */
/*                                                                          */
/* Digital assumes no responsibility for the use  or  reliability of its    */
/* software on equipment which is not supplied by Digital.                  */
/*                                                                          */
/*                                                                          */
/* Abstract:	IDE (Intelligent Drive Electronics) Definitions             */
/*                                                                          */
/* Author:	Judith Gold                                                 */
/*                                                                          */
/* Modifications:                                                           */
/*                                                                          */
/*	jeg	19-Nov-1992	Initial entry.                              */
/*                                                                          */
/*** MODULE $idedef ***/
#define IDE_REG_BASE 1008
#define IDE_MAX_RESULT_BYTES 10
#define IDE_MAX_COMMAND_BYTES 10        /* add one for null terminator      */
#define IDE_MAX_UNITS 4
#define IDE_BUS_NODE_NUM 0              /* bus node number - arbitrary value */
#define MAX_RETRIES 3                   /* number of times to appempt read/write */
#define UNIT_POSITION 3                 /* unit number offset in filename   */
#define IDE_INT_MASK 64                 /* mask indicating interrupt received */
#define HLT 2                           /* head load time                   */
#define ND 0                            /* do allow DMAs                    */
#define GPL 27                          /* assumes 512 sector size          */
#define SC 5                            /* number of sectors to be formatted	 */
#define FL_TO_MEM 0                     /* disk read to memory              */
#define MEM_TO_FL 1                     /* memory write to disk             */
#define BYTES_PER_TRACK 9216            /* maximum number of bytes per track */
#define BYTES_PER_SECTOR 512            /* MS-DOS format                    */
#define MAX_DMA_BYTES 65536             /* maximum number of bytes to dma   */
#define N 2                             /* for N in read/write_data 512 assumed */
#define SK 1                            /* set skip bit in read/write commands */
#define MFM 1                           /* set double density bit in "" ""  */
#define FM 0                            /* don't set the MFM bit            */
#define MT 1                            /* multi-track operating mode       */
/* -----------------status, data and control register bit definitions------- */
/* SRA -- Status Register A -- 3F0                                          */
#define SRA$M_DIR 0x1
#define SRA$M_N_WP 0x2
#define SRA$M_N_INDX 0x4
#define SRA$M_HDSEL 0x8
#define SRA$M_N_TRK0 0x10
#define SRA$M_STEP 0x20
#define SRA$M_N_DRV2 0x40
#define SRA$M_INT_PENDING 0x80
struct SRA {
    unsigned SRA$V_DIR : 1;
    unsigned SRA$V_N_WP : 1;
    unsigned SRA$V_N_INDX : 1;
    unsigned SRA$V_HDSEL : 1;
    unsigned SRA$V_N_TRK0 : 1;
    unsigned SRA$V_STEP : 1;
    unsigned SRA$V_N_DRV2 : 1;
    unsigned SRA$V_INT_PENDING : 1;
    } ;
/* SRB -- Status Register B -- 3F1                                          */
#define SRB$M_MOT_EN0 0x1
#define SRB$M_MOT_EN1 0x2
#define SRB$M_WE 0x4
#define SRB$M_RDDATA_TOGGLE 0x8
#define SRB$M_WRDATA_TOGGLE 0x10
#define SRB$M_DRIVER_SEL_0 0x20
#define SRB$m_filler 0xC0
struct SRB {
    unsigned SRB$V_MOT_EN0 : 1;
    unsigned SRB$V_MOT_EN1 : 1;
    unsigned SRB$V_WE : 1;
    unsigned SRB$V_RDDATA_TOGGLE : 1;
    unsigned SRB$V_WRDATA_TOGGLE : 1;
    unsigned SRB$V_DRIVER_SEL_0 : 1;
    unsigned SRB$v_filler : 2;
    } ;
/* DOR -- Digital Output Register -- 3F2                                    */
#define DOR$M_DRIVE_SEL0 0x1
#define DOR$M_DRIVE_SEL1 0x2
#define DOR$M_N_RESET 0x4
#define DOR$M_N_DMA_GATE 0x8
#define DOR$M_MOT_EN0 0x10
#define DOR$M_MOT_EN1 0x20
#define DOR$M_MOT_EN2 0x40
#define DOR$M_MOT_EN3 0x80
struct DOR {
    unsigned DOR$V_DRIVE_SEL0 : 1;
    unsigned DOR$V_DRIVE_SEL1 : 1;
    unsigned DOR$V_N_RESET : 1;
    unsigned DOR$V_N_DMA_GATE : 1;
    unsigned DOR$V_MOT_EN0 : 1;         /* motor enables                    */
    unsigned DOR$V_MOT_EN1 : 1;
    unsigned DOR$V_MOT_EN2 : 1;
    unsigned DOR$V_MOT_EN3 : 1;
    } ;
/* TDR  -- Tape Drive Register -- 3F3                                       */
#define TDR$M_TAPE_SEL0 0x1
#define TDR$M_TAPE_SEL1 0x2
#define TDR$m_filler 0xFC
struct TDR {
    unsigned TDR$V_TAPE_SEL0 : 1;
    unsigned TDR$V_TAPE_SEL1 : 1;
    unsigned TDR$v_filler : 6;
    } ;
/* DSR -- Datarate Select Register -- 3F4 (write)                           */
#define DSR$M_DRATE_SEL0 0x1
#define DSR$M_DRATE_SEL1 0x2
#define DSR$M_PRECOMP0 0x4
#define DSR$M_PRECOMP1 0x8
#define DSR$M_PRECOMP2 0x10
#define DSR$m_filler 0x20
#define DSR$M_POWERDOWN 0x40
#define DSR$M_SW_RESET 0x80
struct DSR {
    unsigned DSR$V_DRATE_SEL0 : 1;
    unsigned DSR$V_DRATE_SEL1 : 1;
    unsigned DSR$V_PRECOMP0 : 1;
    unsigned DSR$V_PRECOMP1 : 1;
    unsigned DSR$V_PRECOMP2 : 1;
    unsigned DSR$v_filler : 1;
    unsigned DSR$V_POWERDOWN : 1;
    unsigned DSR$V_SW_RESET : 1;
    } ;
/* MSR_DSR -- Main Status Register -- 3F4 (read)                            */
#define MSR_DSR$M_DRV0_BUSY 0x1
#define MSR_DSR$M_DRV1_BUSY 0x2
#define MSR_DSR$M_DRV2_BUSY 0x4
#define MSR_DSR$M_DRV3_BUSY 0x8
#define MSR_DSR$M_CMD_BUSY 0x10
#define MSR_DSR$M_NON_DMA 0x20
#define MSR_DSR$M_DIO 0x40
#define MSR_DSR$M_RQM 0x80
struct MSR_DSR {
    unsigned MSR_DSR$V_DRV0_BUSY : 1;   /* drive in seek portion of cmd     */
    unsigned MSR_DSR$V_DRV1_BUSY : 1;
    unsigned MSR_DSR$V_DRV2_BUSY : 1;
    unsigned MSR_DSR$V_DRV3_BUSY : 1;
    unsigned MSR_DSR$V_CMD_BUSY : 1;    /* command in progress              */
    unsigned MSR_DSR$V_NON_DMA : 1;
    unsigned MSR_DSR$V_DIO : 1;         /* direction of data xfer           */
    unsigned MSR_DSR$V_RQM : 1;         /* ready bit for CPU                */
    } ;
/* DIR -- Digital Input Register -- 3F7 (read)                              */
#define DIR$M_N_HIGH_DENS 0x1
#define DIR$M_DRATE_SEL0 0x2
#define DIR$M_DRATE_SEL1 0x4
#define DIR$m_filler 0x78
#define DIR$M_DSK_CHG 0x80
struct DIR {
    unsigned DIR$V_N_HIGH_DENS : 1;
    unsigned DIR$V_DRATE_SEL0 : 1;
    unsigned DIR$V_DRATE_SEL1 : 1;
    unsigned DIR$v_filler : 4;
    unsigned DIR$V_DSK_CHG : 1;
    } ;
/* CCR -- Configuration Control Register -- 3F7 (write)                     */
#define CCR$M_DRATE_SEL0 0x1
#define CCR$M_DRATE_SEL1 0x2
#define CCR$m_filler 0xFC
struct CCR {
    unsigned CCR$V_DRATE_SEL0 : 1;
    unsigned CCR$V_DRATE_SEL1 : 1;
    unsigned CCR$v_filler : 6;
    } ;
/* -----------------least significant byte of command definitions-------    */
#define IDE_READ_DATA 6
#define IDE_READ_DLTD_DATA 12
#define IDE_WRITE_DATA 5
#define IDE_WRITE_DLTD_DATA 9
#define IDE_READ_TRACK 2
#define IDE_VERIFY 6
#define IDE_FORMAT_TRACK 13
#define IDE_RECALIBRATE 7
#define IDE_SENSE_INT_STATUS 8
#define IDE_SPECIFY 3
#define IDE_SENSE_DRIVE_STATUS 4
#define IDE_SEEK 15
#define IDE_CONFIGURE 19
#define IDE_RELATIVE_SEEK 15
#define IDE_DUMPREG 14
#define IDE_READID 10
#define IDE_PERPENDICULAR_MODE 18
#define IDE_LOCK 4
/* -----------------status registers 0 - 3, read as command result only------- */
#define SR0$M_DS0 0x1
#define SR0$M_DS1 0x2
#define SR0$M_H 0x4
#define SR0$m_filler 0x8
#define SR0$M_EC 0x10
#define SR0$M_SE 0x20
#define SR0$M_IC 0xC0
struct SR0 {
    unsigned SR0$V_DS0 : 1;             /* drive select                     */
    unsigned SR0$V_DS1 : 1;             /*                                  */
    unsigned SR0$V_H : 1;               /* head address                     */
    unsigned SR0$v_filler : 1;          /*                                  */
    unsigned SR0$V_EC : 1;              /* equipment check                  */
    unsigned SR0$V_SE : 1;              /* seek end                         */
    unsigned SR0$V_IC : 2;              /* interrupt code                   */
    } ;
#define SR1$M_MA 0x1
#define SR1$M_NW 0x2
#define SR1$M_ND 0x4
#define SR1$m_filler1 0x8
#define SR1$M_OR 0x10
#define SR1$M_DE 0x20
#define SR1$m_filler2 0x40
#define SR1$M_EN 0x80
struct SR1 {
    unsigned SR1$V_MA : 1;              /* missing address mark             */
    unsigned SR1$V_NW : 1;              /* not writable                     */
    unsigned SR1$V_ND : 1;              /* no data                          */
    unsigned SR1$v_filler1 : 1;         /*                                  */
    unsigned SR1$V_OR : 1;              /* overrun/underrun                 */
    unsigned SR1$V_DE : 1;              /* data error                       */
    unsigned SR1$v_filler2 : 1;         /*                                  */
    unsigned SR1$V_EN : 1;              /* end of cylinder                  */
    } ;
#define SR2$M_MD 0x1
#define SR2$M_BC 0x2
#define SR2$m_filler 0xC
#define SR2$M_WC 0x10
#define SR2$M_DD 0x20
#define SR2$M_CM 0x40
#define SR2$m_filler2 0x80
struct SR2 {
    unsigned SR2$V_MD : 1;              /* missing data address mark        */
    unsigned SR2$V_BC : 1;              /* bad cylinder                     */
    unsigned SR2$v_filler : 2;          /*                                  */
    unsigned SR2$V_WC : 1;              /* wrong cylinder                   */
    unsigned SR2$V_DD : 1;              /* data error in data field         */
    unsigned SR2$V_CM : 1;              /* control mark                     */
    unsigned SR2$v_filler2 : 1;         /*                                  */
    } ;
#define SR3$M_DS0 0x1
#define SR3$M_DS1 0x2
#define SR3$M_HD 0x4
#define SR3$m_filler1 0x8
#define SR3$M_T0 0x10
#define SR3$m_filler2 0x20
#define SR3$M_WP 0x40
#define SR3$m_filler3 0x80
struct SR3 {
    unsigned SR3$V_DS0 : 1;             /* drive select                     */
    unsigned SR3$V_DS1 : 1;             /*                                  */
    unsigned SR3$V_HD : 1;              /* head address                     */
    unsigned SR3$v_filler1 : 1;         /*                                  */
    unsigned SR3$V_T0 : 1;              /* TRACK0 pin                       */
    unsigned SR3$v_filler2 : 1;         /*                                  */
    unsigned SR3$V_WP : 1;              /* write protect pin                */
    unsigned SR3$v_filler3 : 1;         /*                                  */
    } ;
/* media descriptors   F == floppy, H = hard disk                           */
#define FMEGA 240                       /* 1.44 or 2.88 MB 3.5" or          */
/* 1.2 MB 5.25" floppy                                                      */
#define HDISK 248                       /* hard disk, any size              */
#define F720_3 249                      /* 720 K, 3.5" or                   */
/* 1.2 MB 5.25" floppy                                                      */
#define F640_5 250                      /* 640 K, 5.25" floppy              */
#define F640_3 251                      /* 640 K, 3.5" floppy               */
#define F180_5 252                      /* 180 K, 4.25" floppy              */
#define F360_5 253                      /* 360 K, 5.25" floppy              */
#define F160_5 254                      /* 160 K, 5.25" floppy              */
#define F320_5 255                      /* 320 K, 5.25" floppy              */
#define _2_88_MEG 3019898               /* 2.88 Mb                          */
 
