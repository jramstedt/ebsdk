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
 * Abstract:	KZPSA Flash ROM Definitions
 *
 * Author:	Judith Gold
 *
 * Creation Date: 01-Sep-1994
 *
 * Modifications:
 *
 *	 jeg	01-Sep-1994	Initial version ported from Frank Russell's
 *				code.       
 */
#ifndef PARAM_H
#define PARAM_H

/*
**  Product Support Data Logging (PSDL) structures
*/

typedef struct PSDLHeaderType {
    unsigned int   Checksum;
    unsigned short  ClassType;
    unsigned short  SubClassVersion;
    unsigned int   length;            /* Length of the entire region    */
} PSDLHeaderType;


/******************************************************************************
**
**    Environmental Variables (i.e., Configuration Parameters)
*/

/*
**  Default Configuration
*/

#define DEFAULT_CLASSTYPE             4
#define DEFAULT_SUB_CLASS_VERSION     1
#define DEFAULT_LENGTH                sizeof(PSDLClassType4)        
#define DEFAULT_SCSIID                7
#define DEFAULT_STIME0                0xc
#define DEFAULT_CACHE_BURST_DISABLE   0
#define DEFAULT_BURST_LENGTH          0x10000000
#define DEFAULT_RSVD                  0
#define DEFAULT_KEEPALIVE             5
#define DEFAULT_CMDTO                 5
#define DEFAULT_ABORT_SEL_TO_RETRIES  1
#define DEFAULT_DMA_ENTRIES_PER_QPS   30
#define DEFAULT_NUM_FQE               2
#define DEFAULT_NUM_FREEQ             3
#define DEFAULT_NUM_IPS               4
#define DEFAULT_TICCCR                0
#define DEFAULT_DMAC_THROTTLE         0
#define DEFAULT_ABORT_PHASE_COUNT     2
#define DEFAULT_PRCB_ACIM          0x00000100   /* PRCB.ACIM = AC.OM   */
#define DEFAULT_PRCB_FCW           0x40000001   /* PRCB.FCW  = Disable */
#define DEFAULT_PRCB_ICCW          0x00000000   /* PRCB.ICCW = Enable  */
#define DEFAULT_PRCB_RCCW          0x0000000a   /* PRCB.RCCW = 10      */
#define DEFAULT_DEVICE_CNFG        0            /*  all enabled        */
#define DEFAULT_BUSRESET           1
#define DEFAULT_FAST               1
#define DEFAULT_TERMPWR            1

/* This will change in the final pass kzpsa */
#define FLASH_VERSION		   0x8000	/* revision number     */      
/*#define FLASH_VERSION		   0x10000	/* final revision number */ 
/*
**  Per-channel array (indexed by chan_id) holding SCSI ID and STIME0 value
**  and burst length as found in DMODE register.
*/

typedef struct NVChanType {
    unsigned short SCSIID;      /* SCSI node ID of adapter                   */
                                /*                                           */
    unsigned char stime0;       /* Contents of the STIME0 Register in the    */
                                /*   NCR 53C720 (Initial default HTH Timer   */
                                /*   Period and Selection Timeout Period)    */
    unsigned char CacheBurstDisable; /*If non-zero, cache burst is disabled  */
    unsigned char BurstLength;  /*   Burst length if cache burst is enabled  */ 
                                /*   Burst Length: [7:6]                     */
                                /*                  00   2-transfer burst    */
                                /*                  01   4-transfer burst    */
                                /*                  10   8-transfer burst    */
                                /*                  11   16-transfer burst   */
    union {
      struct {
	unsigned BusReset : 1;	/* Bus reset on boot			     */
	unsigned TermPwr  : 1;	/* Enable term power			     */
	unsigned Fast     : 1;	/* Enable Fast, if synch		     */
	unsigned reserved : 5;	/* not used				     */
      } ChanParams;
      unsigned char AllParams;   /* union reference                           */
    }u;
} NVChanType;

typedef struct PSDLClassType4 {            /* PSDL ClassType 4 Env Variables */
    PSDLHeaderType  Header;

/*
**  NOTE - must be reproduced for each channel.
*/
    NVChanType      Chan;

/*
**  The minimum time (in seconds) the host must allow the adapter for 'Verify
**  Adapter Sanity' command execution.  This value is returned in the 'Adapter
**  State Set Response' Message.
*/
    unsigned char KeepAlive;

/*
**  Default command timeout (5 seconds), used:
**    a.  when command timer field in 'Execute SCSI I/O CCB is zero, and
**    b.  for Request Sense command when issued during autosense.
*/
    unsigned char CmdTO;

/*
**  Number of retries on selection timeout when attempting to send an abort
**  message or BDR.
*/
    unsigned char AbortSelTORetries;

/*
**  Number of 16-byte DMA Entries to reserve per Queued Process Slot.
*/
    unsigned char DMAEntriesPerQPS;

/*
**  The next 3 config parameters are interrelated:
**
**    a.  NumFQE = the number of Free Queue Elements to be cached by the
**        adapter.  A Free Queue Element consists of a host DAFQ element paired
**        up with an Immediate Process Slot (IPS).
**    b.  NumFreeQ = the minimum number of free queue entries that the host
**        should allocate to the DAFQ.  This value is returned in the 'Adapter
**        State Set Response' Message.
**    c.  NumIPS = the number of Immediate Process Slots (IPS) to be
**        dynamically allocated at init time.
**
**  Care must be taken in assigning values to these parameters.  Their
**  relationship must be as follows:
**
**        NumFQE <= NumFreeQ < NumIPS
*/
    unsigned char NumFQE;
    unsigned char NumFreeQ;
    unsigned char NumIPS;
    unsigned char rsvd;                 /* Fill to LW boundary */


/*
**  Phase Change Count Limit.  This is used during abort attempts by the 
**  scripts to determine if a reset needs to be done.  The target may change
**  phases this many times without responding to the asserted ATN; else, a
**  bus reset will be requested.
*/
    unsigned char AbortPhaseCount;
    unsigned char rsvd1;

/*
**  Processor Control Block.  These parameters take effect after the SYSCTL
**  instruction from Diagnostic Mode to Functional Firmware Mode.
*/

    unsigned int PRCBACIM ;    /* PRCB AC Register Initial Image */
    unsigned int PRCBFCW  ;    /* PRCB Fault Configuration Word  */
    unsigned int PRCBICCW ;    /* PRCB Instruction Cache Configuration Word */
    unsigned int PRCBRCCW ;    /* PRCB Register Cache Configuration Word */

/*
**  Device specific configurable parameters
**  NOTE - must be reproduced for each channel.
*/
    unsigned short DevCnfg[16];

} PSDLClassType4;

#define PSDL_LENGTH_4 sizeof(PSDLClassType4)




/*  Device Configuration Parameters  */
#define DEVCNFG_DISABLE_WIDE       0x01
#define DEVCNFG_DISABLE_DISCONNECT 0x02
#define DEVCNFG_DISABLE_START_UNIT 0x04   /*  not used on TZA          */   
#define DEVCNFG_DISABLE_SYNC       0x08
#define DEVCNFG_DISABLE_BOOT_SCAN  0x10   /*  not used on TZA          */   
#define DEVCNFG_DISABLE_TAGQ       0x20
#define DEVCNFG_DISABLE_TERMPWR    0x40   /*  not used on TZA          */


/* Status register bit mask definitions                                       */
#define FLASH_STATUS_M_WSMR             0x80
#define FLASH_STATUS_M_ES               0x40
#define FLASH_STATUS_M_EF               0x20
#define FLASH_STATUS_M_PF               0x10
#define FLASH_STATUS_M_VPPL             0x08

/* Command set                                                                */
    #define FLASH_SETUP_ERASE               (0x20202020 & mask)
    #define FLASH_SETUP_PROGRAM             (0x40404040 & mask)
    #define FLASH_CLEAR_STATUS              (0x50505050 & mask)
    #define FLASH_READ_STATUS               (0x70707070 & mask)
    #define FLASH_ERASE_SUSPEND             (0xB0B0B0B0 & mask)
    #define FLASH_ERASE                     (0xD0D0D0D0 & mask)
    #define FLASH_READ_ARRAY                (0xFFFFFFFF & mask)

/* Possible errors                                                            */
    #define FLASH_OP_CMP                    (0x80808080 & mask)
    #define FLASH_VPP_ERROR                 (0x08080808 & mask)
    #define FLASH_CMD_SEQ_ERROR             (0x30303030 & mask)
    #define FLASH_ERASE_ERROR               (0x20202020 & mask)
    #define FLASH_PROG_ERROR                (0x10101010 & mask)

   #define FLASH_ADDR_MASK                 0xFFF80000
   #define FLASH_BYTE_LEN                  0x00080000
   
   #define RESERVED_PRCB_LEN              0x00000100

/* Flash Blocks */

   #define FLASH_BLOCK_0        0x00000         /* 448 KByte Main Block */
   #define FLASH_BLOCK_1        0x70000         /* 16 KByte Parameter Block */ 
   #define FLASH_BLOCK_2        0x74000         /* 16 KByte Parameter Block */ 
   #define FLASH_BLOCK_3        0x78000         /* 32 KByte Boot Block */ 

   #define FLASH_BANK_M_ALL     0xFFFFFFFF      /* Perform flash function on all
                                              banks */
   #define FLASH_BANK_V_CNFG             0      /* Perform flash function on 
                                              CNFG bank */
   #define FLASH_BANK_V_HWLOG            8      /* Perform flash function on 
                                              HWLog bank */
   #define FLASH_BANK_V_FWLOG           24      /* Perform flash function on 
                                              FWLog bank */

#define FLASH_MAX_PROG   3             /* Only Blocks 0-2 are updateable */

/* Retry counts and timeouts                                                  */
#define MAX_FLASH_RETRY                 0x02
#define MAX_FLASH_BYTE_PROGRAM          0x05000000        /* 5 seconds */
#define MAX_FLASH_ERASE                 0x1a000000        /* 25 seconds */


       /* Updateable image size = total - parameter region - bootblock        */
       #define BOOT_BLOCK_LEN    (32L * 1024L)
       #define PARAM_BLOCK_LEN    (16L * 1024L)
       #define FLASH_IMAGE_SIZE  (FLASH_BYTE_LEN - PARAM_BLOCK_LEN - BOOT_BLOCK_LEN)

#define SEGMENT_SIZE       1024L

#define SEGMENT_CNT  FLASH_IMAGE_SIZE / SEGMENT_SIZE





#endif /* #ifndef PARAM_H */

