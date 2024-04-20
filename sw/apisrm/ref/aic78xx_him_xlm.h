/*
 * Copyright (C) 1998 by
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
 * Abstract:	Adaptec AIC-78xx Definitions
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *	sfs	27-Mar-1998	Initial entry.
 */

/*$Header:   Y:/source/chimscsi/src/xlm.hV_   1.4.20.0   31 Jan 1998 10:38:48   SPAL3094  $*/
/***************************************************************************
*                                                                          *
* Copyright 1995,1996,1997,1998 Adaptec, Inc.,  All Rights Reserved.       *
*                                                                          *
* This software contains the valuable trade secrets of Adaptec.  The       *
* software is protected under copyright laws as an unpublished work of     *
* Adaptec.  Notice is for informational purposes only and does not imply   *
* publication.  The user of this software may make copies of the software  *
* for use with parts manufactured by Adaptec or under license from Adaptec *
* and for no other use.                                                    *
*                                                                          *
***************************************************************************/

/***************************************************************************
*
*  Module Name:   XLM.C
*
*  Description:   Include files for translation management 
*
*  Owners:        ECX IC Firmware Team
*    
*  Notes:         NONE
*
***************************************************************************/

#if 0
#include "rsm.h"	/* exclude */

#include "chimdef.h"	/* exclude */
#endif

/*$Header:   Y:/source/chimscsi/src/XLMREF.HV_   1.31.3.4   07 Apr 1998 11:00:58   THU  $*/
/***************************************************************************
*                                                                          *
* Copyright 1995,1996,1997,1998,1999 Adaptec, Inc.,  All Rights Reserved.  *
*                                                                          *
* This software contains the valuable trade secrets of Adaptec.  The       *
* software is protected under copyright laws as an unpublished work of     *
* Adaptec.  Notice is for informational purposes only and does not imply   *
* publication.  The user of this software may make copies of the software  *
* for use with parts manufactured by Adaptec or under license from Adaptec *
* and for no other use.                                                    *
*                                                                          *
***************************************************************************/

/***************************************************************************
*
*  Module Name:   XLMREF.H
*
*  Description:   Definitions for data structure and function call
*                 reference under Translation Layer.
*    
*  Owners:        ECX IC Firmware Team
*    
*  Notes:
*
***************************************************************************/

/***************************************************************************
* Miscellaneous
***************************************************************************/
typedef struct SCSI_ADAPTER_TSCB_ SCSI_ADAPTER_TSCB;
typedef struct SCSI_TARGET_TSCB_ SCSI_TARGET_TSCB;
#define  SCSI_DOMAIN_VALIDATION  SCSI_DOMAIN_VALIDATION_BASIC+SCSI_DOMAIN_VALIDATION_ENHANCED

/****************************************************************************
* Definitions for BIOS/OPTIMA mode firmware (backward compatible)
* (The offset are relative to the base of scratch ram)
****************************************************************************/
#define SCSI_BIOS_XFER_OPTION             0x0000
#define SCSI_BIOS_FAST20_OPTION           0x0010
#define SCSI_BIOS_DISCON_OPTION           0x0012
#define SCSI_BIOS_BAY_OFFSET              0x0040
#define SCSI_OPTIMA_NEXT_SCB_ARRAY        0x0026
#define SCSI_OPTIMA_QIN_PTR_ARRAY         0x0022
#define SCSI_OPTIMA_QOUT_PTR_ARRAY        0x0036

/****************************************************************************
* Definitions for scontrol values to be used in OSDBuildSCB routines
****************************************************************************/
#define SCSI_SIMPLETAG                    0x00
#define SCSI_HEADTAG                      0x01
#define SCSI_ORDERTAG                     0x02
#define SCSI_TAGENB                       0x20
#define SCSI_DISCENB                      0x40

/***************************************************************************
* Macros for general purposes
***************************************************************************/
#define  SCSI_INITSH(initializationTSH)  \
               ((SCSI_INITIALIZATION_TSCB HIM_PTR) (initializationTSH))
#define  SCSI_ADPTSH(adapterTSH) ((SCSI_ADAPTER_TSCB HIM_PTR)(adapterTSH))
#define  SCSI_TRGTSH(targetTSH)  ((SCSI_TARGET_TSCB HIM_PTR)(targetTSH))
#define  SCSI_NODETSH(nodeTSH)   ((SCSI_NODE HIM_PTR)(nodeTSH))
#define  SCSI_NEXUSTSH(nexusTSH) ((SCSI_NEXUS HIM_PTR)(nexusTSH))
#define  SCSI_OSMFP(adapterTSH) SCSI_ADPTSH((adapterTSH))->osmFuncPtrs
#define  SCSI_IOBRSV(iob)  ((SCSI_IOB_RESERVE HIM_PTR) (iob)->iobReserve.virtualAddress)
#define  SCSI_GETIOB(hiob) *((HIM_IOB HIM_PTR HIM_PTR) (((SCSI_UEXACT8 HIM_PTR)(hiob)) - \
                              OSMoffsetof(SCSI_IOB_RESERVE,hiob) + \
                              OSMoffsetof(SCSI_IOB_RESERVE,iob)))
#define  SCSI_GETADP(hhcb)    ((SCSI_ADAPTER_TSCB HIM_PTR) \
                              (((SCSI_UEXACT8 HIM_PTR)(hhcb)) - \
                              OSMoffsetof(SCSI_ADAPTER_TSCB,hhcb)))
#define  SCSI_IS_EXCALIBUR_ID(productID)  \
                              (((productID) & 0xFFF0FFFF) == 0x78909004)
#define  SCSI_IS_BAYONET_ID(productID)  \
                              (((productID) & 0xFF80FFFF) == 0x00009005)
#define  SCSI_IS_TRIDENT_ID(productID)  \
                              (((productID) & 0xFF80FFFF) == 0x00809005)
/* in the future this will need to change to #if !SCSI_ARO_SUPPORT for 9005 */
/* and w/in the #else we will have a check for 9006 */

#if SCSI_SCAM_ENABLE
#define  SCSI_xSAVE_CURRENT_WWIDS(adapterTSH)   SCSIxSaveCurrentScamWWIDs((adapterTSH))
#define  SCSI_xVALIDATE_LUN(targetTSH) SCSIxScamValidateLun((targetTSH))
#else
#define  SCSI_xSAVE_CURRENT_WWIDS(adapterTSH)
#define  SCSI_xVALIDATE_LUN(targetTSH) SCSIxNonScamValidateLun((targetTSH))
#endif

#if SCSI_DMA_SUPPORT
#define  SCSI_xSETUP_DMA_TSCB(adapterTSH)  SCSIxSetupDmaTSCB((adapterTSH))
#else
#define  SCSI_xSETUP_DMA_TSCB(adapterTSH)
#endif /* SCSI_DMA_SUPPORT */

/***************************************************************************
* Macros for operating mode independent
***************************************************************************/

/***************************************************************************
* Enable dual address cycle for 64 bits bus addressing
***************************************************************************/
#if OSM_DMA_SWAP_WIDTH == 64
#  define SCSI_ENABLE_DAC(adapterTSH)  SCSIxEnableDAC((adapterTSH))
#else
#  define SCSI_ENABLE_DAC(adapterTSH)
#endif

/***************************************************************************
*  SCSI_HOST_TYPE definitions for host devices supported
***************************************************************************/
typedef struct SCSI_HOST_TYPE_
{
   SCSI_UINT32 idHost;                       /* host id supported    */
   SCSI_UINT32 idMask;                       /* mask for the host id */
} SCSI_HOST_TYPE;

/***************************************************************************
*  SCSI_NVM_LAYOUT definitions
*     This structures must be 64 bytes in length exactly for backward
*     compatible reason. 
***************************************************************************/
typedef struct SCSI_NVM_LAYOUT_
{
   union
   {
      SCSI_UEXACT16 aic78xxTC[SCSI_MAXDEV];
      SCSI_UEXACT16 bayonetTC[SCSI_MAXDEV];
   } targetControl;
   SCSI_UEXACT16 biosControl;
   SCSI_UEXACT16 generalControl;
   SCSI_UEXACT16 word19;                     /* id information             */
   SCSI_UEXACT16 word20;               
   SCSI_UEXACT16 reserved[10];               /* reserved and padding       */
   SCSI_UEXACT16 signature;                  /* signature = 0250           */
   SCSI_UEXACT16 checkSum;                   /* check sum value            */
} SCSI_NVM_LAYOUT;

/* Bitmap for aic78xxTC[] */

#define SCSI_AIC78XXTC_SYNCHRONOUSRATE  (SCSI_UEXACT16)0x0007    /* sync transfer rate         */
#define SCSI_AIC78XXTC_INITIATESYNC     (SCSI_UEXACT16)0X0008    /* initiate sync nego         */
#define SCSI_AIC78XXTC_DISCONNECTENABLE (SCSI_UEXACT16)0x0010    /* disconnect enable          */
#define SCSI_AIC78XXTC_INITIATEWIDE     (SCSI_UEXACT16)0x0020    /* initiate wide nego         */
#define SCSI_AIC78XXTC_ULTRAENABLE      (SCSI_UEXACT16)0x0040    /* ultra enable               */
#define SCSI_AIC78XXTC_DONOTCARE7_10    (SCSI_UEXACT16)0x0780    /* dont care                  */
#define SCSI_AIC78XXTC_MULTIPLELUN      (SCSI_UEXACT16)0x0800    /* multiple lun enable        */
#define SCSI_AIC78XXTC_DONOTCARE12_15   (SCSI_UEXACT16)0xF000    /* don't care                 */

/* Bitmap for BayonetTC[] */

#define SCSI_BAYONETTC_LOWBITSYNCRATE   (SCSI_UEXACT16)0x0007    /* lower 3-bit sync rate      */
#define SCSI_BAYONETTC_INITIATESYNC     (SCSI_UEXACT16)0x0008    /* initiate sync nego         */
#define SCSI_BAYONETTC_DISCONNECTENABLE (SCSI_UEXACT16)0x0010    /* disconnect enable          */
#define SCSI_BAYONETTC_INITIATEWIDE     (SCSI_UEXACT16)0x0020    /* initiate wide nego         */
#define SCSI_BAYONETTC_HIGHBITSYNCRATE  (SCSI_UEXACT16)0x00C0    /* upper 2-bit sync rate      */
#define SCSI_BAYONETTC_DONOTCARE8_9     (SCSI_UEXACT16)0x0300    /* dont care                  */
#define SCSI_BAYONETTC_HOSTMANAGED      (SCSI_UEXACT16)0x0400    /* Host Managed (NTC)         */
#define SCSI_BAYONETTC_MULTIPLELUN      (SCSI_UEXACT16)0x0800    /* multiple lun enable        */
#define SCSI_BAYONETTC_DONOTCARE12_15   (SCSI_UEXACT16)0xF000    /* don't care                 */

/* Bitmap for biosControl */

#define SCSI_BIOSCONTROL_DONOTCARE0_3     (SCSI_UEXACT16)0x000F  /* don't care                 */
#define SCSI_BIOSCONTROL_STPWLEVEL        (SCSI_UEXACT16)0x0010  /* termination power level    */
#define SCSI_BIOSCONTROL_DONOTCARE5_7     (SCSI_UEXACT16)0x00E0  /* don't care                 */
#define SCSI_BIOSCONTROL_SCAMENABLE       (SCSI_UEXACT16)0x0100  /* scam enable                */
#define SCSI_BIOSCONTROL_DONOTCARE9_15    (SCSI_UEXACT16)0xFE00  /* don't care                 */

/* Bitmap for generalControl */

#define SCSI_GENERALCONTROL_AUTOTERMINATION   (SCSI_UEXACT16)0x0001 /* auto termination           */
#define SCSI_GENERALCONTROL_ULTRAENABLE       (SCSI_UEXACT16)0x0002 /* ultra enable               */
#define SCSI_GENERALCONTROL_TERMINATIONLOW    (SCSI_UEXACT16)0x0004 /* low termination enable     */
#define SCSI_GENERALCONTROL_TERMINATIONHIGH   (SCSI_UEXACT16)0x0008 /* high termination enable    */
#define SCSI_GENERALCONTROL_PARITYENABLE      (SCSI_UEXACT16)0x0010 /* parity checking enable     */
#define SCSI_GENERALCONTROL_MULTIPLELUN       (SCSI_UEXACT16)0x0020 /* multiple lun support       */
#define SCSI_GENERALCONTROL_RESETSCSI         (SCSI_UEXACT16)0x0040 /* reset scsi bus             */
#define SCSI_GENERALCONTROL_CLUSTERENABLE     (SCSI_UEXACT16)0x0080 /* cluster enable (NTC)       */
#define SCSI_GENERALCONTROL_DONOTCARE9_8      (SCSI_UEXACT16)0x0300 /* don't care                 */
#define SCSI_GENERALCONTROL_SECONDARYAUTOTERM (SCSI_UEXACT16)0x0400 /* secondary auto termination */
#define SCSI_GENERALCONTROL_SECONDARYTERMLOW  (SCSI_UEXACT16)0x0800 /* secondary low termination  */
#define SCSI_GENERALCONTROL_SECONDARYTERMHIGH (SCSI_UEXACT16)0x1000 /* secondary high termination */
#define SCSI_GENERALCONTROL_DONOTCARE13       (SCSI_UEXACT16)0x2000 /* don't care                 */
#define SCSI_GENERALCONTROL_DOMAINVALIDATION  (SCSI_UEXACT16)0x4000 /* domain validation          */
#define SCSI_GENERALCONTROL_DONOTCARE15       (SCSI_UEXACT16)0x8000 /* don't care                 */

/* Bitmap for word19 */

#define SCSI_WORD19_SCSIID       (SCSI_UEXACT16)0x000F /* host scsi id  */
#define SCSI_WORD19_DONOTCARE4_7 (SCSI_UEXACT16)0x00F0 /* don't care    */
#define SCSI_WORD19_BUSRELEASE   (SCSI_UEXACT16)0xFF00 /* bus release   */

/* Bitmap for word20 */

#define SCSI_WORD20_MAXTARGETS   (SCSI_UEXACT16)0x00FF /* maximum targets     */
#define SCSI_WORD20_BOOTLUN      (SCSI_UEXACT16)0x0F00 /* boot lun id         */
#define SCSI_WORD20_BOOTID       (SCSI_UEXACT16)0xF000 /* boot device scsi id */

/* Signature */

#define SCSI_NVM_SIGNATURE       (SCSI_UEXACT16)0x0250 /* signature           */

/***************************************************************************
*  SCSI_BIOS_SCB2 definitions
*     This structures must be exactly 32 bytes
***************************************************************************/
typedef struct SCSI_BIOS_SCB2_
{
   SCSI_UEXACT32 bios13Vector;               /* bios int 13 vector      */
   SCSI_UEXACT8 biosDrive[8];                /* bios drives table       */
   SCSI_UEXACT16 biosGlobal;
   SCSI_UEXACT8 driveRange;                  /* first and last drive    */
   SCSI_UEXACT8 ignore15;                    /* ignored field           */
   SCSI_UEXACT32 biosIntVector;              /* bios interrupt vector   */
   SCSI_UEXACT8 ignore20_31[12];             /* ignored field           */
} SCSI_BIOS_SCB2;

/* Bitmap for biosDrive[] */
#define SCSI_BIOSDRIVE_TARGETID (SCSI_UEXACT8)0x0F /* target id               */
#define SCSI_BIOSDRIVE_LUNID    (SCSI_UEXACT8)0xF0 /* lun id                  */

/* Bitmap for biosGlobal */
#define SCSI_BIOSGLOBAL_IGNORE5_0   (SCSI_UEXACT16)0x003F /* ignored field           */
#define SCSI_BIOSGLOBAL_DOS5        (SCSI_UEXACT16)0x0040 /* dos 5 or after          */
#define SCSI_BIOSGLOBAL_IGNORE7     (SCSI_UEXACT16)0x0080 /* ignored fields          */
#define SCSI_BIOSGLOBAL_GIGA        (SCSI_UEXACT16)0x0100 /* giga byte translation   */
#define SCSI_BIOSGLOBAL_IGNORE12_9  (SCSI_UEXACT16)0x1E00 /* ignored field           */
#define SCSI_BIOSGLOBAL_SCAMENABLE  (SCSI_UEXACT16)0x2000 /* scam enable             */
#define SCSI_BIOSGLOBAL_IGNORE15_14 (SCSI_UEXACT16)0xC000 /* ignored field           */

/* Bitmap for driveRange */
#define SCSI_DRIVERANGE_FIRSTDRIVE (SCSI_UEXACT8)0x0F /* first bios drive */
#define SCSI_DRIVERANGE_LASTDRIVE  (SCSI_UEXACT8)0xF0 /* last bios drive  */

/***************************************************************************
*  SCSI_BIOS_INFO definitions
***************************************************************************/
typedef struct SCSI_BIOS_INFORMATION_
{
   struct
   {
      SCSI_UINT8 biosActive:1;               /* bios active          */
      SCSI_UINT8 dos5orAfter:1;              /* dos 5 or after       */
      SCSI_UINT8 extendedTranslation:1;      /* extended translation */
      SCSI_UINT8 scamenable:1;               /* scam enable          */
      SCSI_PAD_BIT unused:28;
   } biosFlags;
   SCSI_UINT8 firstBiosDrive;                /* first bios drive     */
   SCSI_UINT8 lastBiosDrive;                 /* last bios drive      */
   SCSI_UINT8 numberDrives;                  /* number of bios drives*/
   struct
   {
      SCSI_UINT8 targetID;                   /* drive target id      */
      SCSI_UINT8 lunID;                      /* drive lun id         */
   } biosDrive[8];                           /* bios drives          */
} SCSI_BIOS_INFORMATION;

/***************************************************************************
*  Function prototype internal to translation management layer
***************************************************************************/
void SCSIxMaximizeMemory(SCSI_MEMORY_TABLE HIM_PTR,SCSI_MEMORY_TABLE HIM_PTR);
void OSDBuildSCB(SCSI_HIOB SCSI_IPTR);
void OSDStandard64BuildSCB(SCSI_HIOB HIM_PTR,HIM_IOB HIM_PTR,SCSI_TARGET_TSCB HIM_PTR,SCSI_ADAPTER_TSCB HIM_PTR);
void OSDSwapping32BuildSCB(SCSI_HIOB HIM_PTR,HIM_IOB HIM_PTR,SCSI_TARGET_TSCB HIM_PTR,SCSI_ADAPTER_TSCB HIM_PTR);
void OSDSwapping64BuildSCB(SCSI_HIOB HIM_PTR,HIM_IOB HIM_PTR,SCSI_TARGET_TSCB HIM_PTR,SCSI_ADAPTER_TSCB HIM_PTR); 
void OSDStandardAdvBuildSCB(SCSI_HIOB HIM_PTR,HIM_IOB HIM_PTR,SCSI_TARGET_TSCB HIM_PTR,SCSI_ADAPTER_TSCB HIM_PTR);
void OSDSwappingAdvBuildSCB(SCSI_HIOB HIM_PTR,HIM_IOB HIM_PTR,SCSI_TARGET_TSCB HIM_PTR,SCSI_ADAPTER_TSCB HIM_PTR);
void OSDStandard160mBuildSCB(SCSI_HIOB HIM_PTR,HIM_IOB HIM_PTR,SCSI_TARGET_TSCB HIM_PTR,SCSI_ADAPTER_TSCB HIM_PTR);
void OSDSwapping160mBuildSCB(SCSI_HIOB HIM_PTR,HIM_IOB HIM_PTR,SCSI_TARGET_TSCB HIM_PTR,SCSI_ADAPTER_TSCB HIM_PTR);
SCSI_BUFFER_DESCRIPTOR HIM_PTR OSDGetSGList (SCSI_HIOB SCSI_IPTR);
void OSDCompleteHIOB(SCSI_HIOB SCSI_IPTR);
void OSDCompleteSpecialHIOB(SCSI_HIOB SCSI_IPTR);
void SCSIxTranslateError(HIM_IOB HIM_PTR,SCSI_HIOB HIM_PTR);
SCSI_BUS_ADDRESS OSDGetBusAddress(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,void SCSI_IPTR);
void OSDAdjustBusAddress(SCSI_HHCB SCSI_HPTR,SCSI_BUS_ADDRESS SCSI_IPTR,SCSI_UINT);
SCSI_UEXACT32 OSDReadPciConfiguration(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void OSDWritePciConfiguration(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT32);
void OSDAsyncEvent(SCSI_HHCB SCSI_HPTR,SCSI_UINT16,...);
SCSI_HOST_ADDRESS SCSI_LPTR OSDGetHostAddress(SCSI_HHCB SCSI_HPTR);
SCSI_UINT8 SCSIxApplyNVData(HIM_TASK_SET_HANDLE);
SCSI_UINT8 SCSIxBiosExist(HIM_TASK_SET_HANDLE);
SCSI_UINT8 SCSIxOptimaExist(HIM_TASK_SET_HANDLE);
SCSI_UINT8 SCSIxOptimaExist(HIM_TASK_SET_HANDLE);
void SCSIxGetBiosInformation(HIM_TASK_SET_HANDLE);
SCSI_UINT8 SCSIxChkLunExist(SCSI_UEXACT8 HIM_PTR,SCSI_UINT8,SCSI_UINT8);
void SCSIxSetLunExist(SCSI_UEXACT8 HIM_PTR,SCSI_UINT8,SCSI_UINT8);
void SCSIxClearLunExist(SCSI_UEXACT8 HIM_PTR,SCSI_UINT8,SCSI_UINT8);
SCSI_UINT8 SCSIxSetupBusScan(SCSI_ADAPTER_TSCB HIM_PTR);
void SCSIxSetupLunProbe (SCSI_ADAPTER_TSCB HIM_PTR,SCSI_UEXACT8,SCSI_UEXACT8);
#if SCSI_NSX_REPEATER + SCSI_PAC_NSX_REPEATER + SCSI_PAC_SEND_SSU
void SCSIxQueueSSU(SCSI_ADAPTER_TSCB HIM_PTR);
HIM_UINT32 SCSIxPostSSU(HIM_IOB HIM_PTR);
void SCSIxQueueTUR(SCSI_ADAPTER_TSCB HIM_PTR);
HIM_UINT32 SCSIxPostTUR(HIM_IOB HIM_PTR);
#endif /* SCSI_NSX_REPEATER + SCSI_PAC_NSX_REPEATER + SCSI_PAC_SEND_SSU  */
void SCSIxQueueBusScan(SCSI_ADAPTER_TSCB HIM_PTR);
HIM_UINT32 SCSIxPostBusScan(HIM_IOB HIM_PTR);
#if SCSI_DOMAIN_VALIDATION
void SCSIxRestoreDefaultXfer(SCSI_ADAPTER_TSCB HIM_PTR,SCSI_TARGET_TSCB HIM_PTR);
void SCSIxDvForceNegotiation(SCSI_ADAPTER_TSCB HIM_PTR,SCSI_TARGET_TSCB HIM_PTR);
void SCSIxSetupDomainValidation(SCSI_ADAPTER_TSCB HIM_PTR);
void SCSIxQueueDomainValidation(SCSI_ADAPTER_TSCB HIM_PTR);
void SCSIxQueueDomainValidationBasic (SCSI_ADAPTER_TSCB HIM_PTR);
void SCSIxQueueDomainValidationEnhanced (SCSI_ADAPTER_TSCB HIM_PTR);
HIM_UINT32 SCSIxPostDomainValidationBasic(HIM_IOB HIM_PTR);
HIM_UINT32 SCSIxPostDomainValidationEnhanced(HIM_IOB HIM_PTR);
HIM_UINT32 SCSIxThrottleTransfer (SCSI_ADAPTER_TSCB HIM_PTR,SCSI_TARGET_TSCB HIM_PTR);
#endif /* SCSI_DOMAIN_VALIDATION */
void SCSIxQueueSpecialIOB(SCSI_ADAPTER_TSCB HIM_PTR,SCSI_UINT8);
HIM_UINT32 SCSIxPostQueueSpecialIOB(HIM_IOB HIM_PTR);
void SCSIxSaveCurrentScamWWIDs(SCSI_ADAPTER_TSCB HIM_PTR);
SCSI_UEXACT8 SCSIxDetermineFirmwareMode(SCSI_ADAPTER_TSCB HIM_PTR);
void SCSIxEnableDAC(HIM_TASK_SET_HANDLE);
HIM_HOST_ID SCSIxGetNextSubSystemSubVendorID (HIM_UINT16,HIM_HOST_ID HIM_PTR);
HIM_HOST_ID SCSIxBayonetGetNextSubSystemSubVendorID (HIM_UINT16,HIM_HOST_ID HIM_PTR);
HIM_HOST_ID SCSIxTridentGetNextSubSystemSubVendorID (HIM_UINT16,HIM_HOST_ID HIM_PTR);
SCSI_UEXACT8 SCSIxDetermineValidNVData(SCSI_NVM_LAYOUT HIM_PTR);

#if SCSI_TARGET_OPERATION
void OSDBuildTargetSCB(SCSI_HIOB SCSI_IPTR);
void OSDSwappingAdvBuildTargetSCB(SCSI_HIOB HIM_PTR,HIM_IOB HIM_PTR,SCSI_NEXUS SCSI_XPTR,SCSI_ADAPTER_TSCB HIM_PTR);
void OSDSwapping160mBuildTargetSCB(SCSI_HIOB HIM_PTR,HIM_IOB HIM_PTR,SCSI_NEXUS SCSI_XPTR,SCSI_ADAPTER_TSCB HIM_PTR);
void SCSIxTargetTaskManagementRequest(HIM_IOB HIM_PTR,SCSI_HIOB HIM_PTR);
#if SCSI_MULTIPLEID_SUPPORT
HIM_BOOLEAN SCSIxEnableID(HIM_IOB HIM_PTR);
HIM_BOOLEAN SCSIxDisableID(HIM_IOB HIM_PTR);
#endif /* SCSI_MULTIPLEID_SUPPORT */
#endif /* SCSI_TARGET_OPERATION */
#if SCSI_PAC_NSX_REPEATER || SCSI_NSX_REPEATER
SCSI_BUFFER_DESCRIPTOR SCSI_LPTR OSDGetNSXBuffer(SCSI_HIOB HIM_PTR);
void OSDReturnNSXStatus(SCSI_HIOB HIM_PTR,SCSI_UEXACT8,SCSI_UINT16);
#endif /* SCSI_PAC_NSX_REPEATER || SCSI_NSX_REPEATER */

#if SCSI_DMA_SUPPORT
void SCSIxSetupDmaTSCB(SCSI_ADAPTER_TSCB HIM_PTR);
#endif /* SCSI_DMA_SUPPORT */

/*$Header:   Y:/source/chimscsi/src/XLMDEF.HV_   1.45.8.4   15 Apr 1998 15:29:32   THU  $*/
/***************************************************************************
*                                                                          *
* Copyright 1995,1996,1997,1998,1999 Adaptec, Inc.,  All Rights Reserved.  *
*                                                                          *
* This software contains the valuable trade secrets of Adaptec.  The       *
* software is protected under copyright laws as an unpublished work of     *
* Adaptec.  Notice is for informational purposes only and does not imply   *
* publication.  The user of this software may make copies of the software  *
* for use with parts manufactured by Adaptec or under license from Adaptec *
* and for no other use.                                                    *
*                                                                          *
***************************************************************************/

/***************************************************************************
*
*  Module Name:   XLMDEF.H
*
*  Description:   Definitions for Translation Layer data structures.
*    
*  Owners:        ECX IC Firmware Team
*    
*  Notes:         1. This file is referenced by all three layers of Common
*                    HIM implementation. 
*
***************************************************************************/

/***************************************************************************
* Macros for accessing OSM routines
***************************************************************************/
#define  OSMxMapIOHandle       SCSI_OSMFP(adapterTSH).OSMMapIOHandle
#define  OSMxReleaseIOHandle   SCSI_OSMFP(adapterTSH).OSMReleaseIOHandle
#define  OSMxEvent             SCSI_OSMFP(adapterTSH).OSMEvent
#define  OSMxGetBusAddress     SCSI_OSMFP(adapterTSH).OSMGetBusAddress
#define  OSMxAdjustBusAddress  SCSI_OSMFP(adapterTSH).OSMAdjustBusAddress
#define  OSMxGetNVSize         SCSI_OSMFP(adapterTSH).OSMGetNVSize
#define  OSMxPutNVData         SCSI_OSMFP(adapterTSH).OSMPutNVData
#define  OSMxGetNVData         SCSI_OSMFP(adapterTSH).OSMGetNVData
#define  OSMxReadUExact8       SCSI_OSMFP(adapterTSH).OSMReadUExact8
#define  OSMxReadUExact16      SCSI_OSMFP(adapterTSH).OSMReadUExact16
#define  OSMxReadUExact32      SCSI_OSMFP(adapterTSH).OSMReadUExact32
#define  OSMxReadStringUExact8 SCSI_OSMFP(adapterTSH).OSMReadStringUExact8
#define  OSMxReadStringUExact16 SCSI_OSMFP(adapterTSH).OSMReadStringUExact16
#define  OSMxReadStringUExact32 SCSI_OSMFP(adapterTSH).OSMReadStringUExact32
#define  OSMxWriteUExact8      SCSI_OSMFP(adapterTSH).OSMWriteUExact8
#define  OSMxWriteUExact16     SCSI_OSMFP(adapterTSH).OSMWriteUExact16
#define  OSMxWriteUExact32     SCSI_OSMFP(adapterTSH).OSMWriteUExact32
#define  OSMxWriteStringUExact8 SCSI_OSMFP(adapterTSH).OSMWriteStringUExact8
#define  OSMxWriteStringUExact16 SCSI_OSMFP(adapterTSH).OSMWriteStringUExact16
#define  OSMxWriteStringUExact32 SCSI_OSMFP(adapterTSH).OSMWriteStringUExact32
#define  OSMxSynchronizeRange  SCSI_OSMFP(adapterTSH).OSMSynchronizeRange
#define  OSMxWatchdog          SCSI_OSMFP(adapterTSH).OSMWatchdog
#define  OSMxSaveInterruptState   SCSI_OSMFP(adapterTSH).OSMSaveInterruptState
#define  OSMxSetInterruptState SCSI_OSMFP(adapterTSH).OSMSetInterruptState
#define  OSMxReadPCIConfigurationDword  SCSI_OSMFP(adapterTSH).OSMReadPCIConfigurationDword
#define  OSMxReadPCIConfigurationWord  SCSI_OSMFP(adapterTSH).OSMReadPCIConfigurationWord
#define  OSMxReadPCIConfigurationByte  SCSI_OSMFP(adapterTSH).OSMReadPCIConfigurationByte
#define  OSMxWritePCIConfigurationDword  SCSI_OSMFP(adapterTSH).OSMWritePCIConfigurationDword
#define  OSMxWritePCIConfigurationWord  SCSI_OSMFP(adapterTSH).OSMWritePCIConfigurationWord
#define  OSMxWritePCIConfigurationByte  SCSI_OSMFP(adapterTSH).OSMWritePCIConfigurationByte
#define  OSMxDelay                      SCSI_OSMFP(adapterTSH).OSMDelay              

/***************************************************************************
* Definitions for type of handle
***************************************************************************/
#define  SCSI_TSCB_TYPE_INITIALIZATION  0
#define  SCSI_TSCB_TYPE_ADAPTER         1
#define  SCSI_TSCB_TYPE_TARGET          2

/***************************************************************************
* SCSI_INITIALIZATION_TSCB structures definitions
***************************************************************************/
typedef struct SCSI_INITIALIZATION_TSCB_
{
   SCSI_UINT8 typeTSCB;                      /* type of handle           */
   SCSI_MEMORY_TABLE memoryTable[7];         /* requirement of HW layer  */
   HIM_CONFIGURATION configuration;          /* area for init HIM config */
} SCSI_INITIALIZATION_TSCB;

/***************************************************************************
* HIM_TARGET_TSCB structure definition
***************************************************************************/
struct SCSI_TARGET_TSCB_
{
   SCSI_UINT8 typeTSCB;                      /* type of handle           */
   SCSI_ADAPTER_TSCB HIM_PTR adapterTSH;     /* adapter task set handle  */
   SCSI_UINT8 scsiID;                        /* device scsi id           */
   SCSI_UINT8 lunID;                         /* target lun id            */
   struct
   {
      SCSI_UINT8 tagEnable:1;                /* tag enable               */
      SCSI_PAD_BIT unused:31;
   } targetAttributes;
   SCSI_UNIT_CONTROL unitControl;            /* SCSI target unit control */
   struct SCSI_TARGET_TSCB HIM_PTR nextTargetTSCB; /* next target TSCB   */
};

/***************************************************************************
* HIM_ADAPTER_TSCB structure definition
***************************************************************************/
struct SCSI_ADAPTER_TSCB_
{
   SCSI_UINT8 typeTSCB;                      /* type of handle             */
   HIM_HOST_ID productID;                    /* host id                    */
   HIM_HOST_ADDRESS hostAddress;             /* host address               */
   SCSI_HHCB   hhcb;                         /* hhcb associated            */
   SCSI_UINT16 multipleLun;                  /* multiple lun support       */
#if SCSI_BIOS_ASPI8DOS
   SCSI_BIOS_INFORMATION biosInformation;    /* bios related information   */
#endif /* SCSI_BIOS_ASPI8DOS */
   SCSI_MEMORY_TABLE memoryTable;            /* requirement of HW layer    */
   HIM_OSM_FUNC_PTRS osmFuncPtrs;            /* functional pointers to OSM */
   void HIM_PTR osmAdapterContext;           /* osm adapter context ptr    */
   SCSI_IO_HANDLE scsiHandle;                /* io handle for reg access   */
                                             /* routine to build SCB       */
      
   HIM_UINT8 memoryMapped;                   /* Allows OSM to choose:      */
                                             /*    0 - I/O mapped          */
                                             /*    1 - mem mapped          */
                                             /*    2 - no choice           */
   void (*OSDBuildSCB)(SCSI_HIOB HIM_PTR,HIM_IOB HIM_PTR,SCSI_TARGET_TSCB HIM_PTR,
                                             SCSI_ADAPTER_TSCB HIM_PTR);   
#if SCSI_TARGET_OPERATION  
                                             /* Pointer to routine to      */
                                             /* build a Target SCB         */    
   void (*OSDBuildTargetSCB)(SCSI_HIOB HIM_PTR,HIM_IOB HIM_PTR,SCSI_NEXUS SCSI_XPTR,
                                             SCSI_ADAPTER_TSCB HIM_PTR);
#endif /* SCSI_TARGET_OPERATION */ 
   SCSI_UEXACT8 NumberLuns[SCSI_MAXDEV];     /* number of luns to scan in prot auto conf */
   SCSI_UEXACT8 lunExist[SCSI_MAXDEV*SCSI_MAXLUN/8]; /* lun exist table    */
   SCSI_UEXACT8 tshExist[SCSI_MAXDEV*SCSI_MAXLUN/8]; /* tsh exist table    */
   SCSI_UEXACT16 scsi1OrScsi2Device;                 /* the Inquiry data   */
                                                     /* indicates this is  */
                                                     /* SCSI2 or earlier   */
                                                     /* device. Bit map    */
                                                     /* indexed by SCSI ID */

#if SCSI_SCAM_ENABLE
   SCSI_UEXACT16 prevScamDevice;             /* scam device                */
   SCSI_UEXACT8 prevScamWWID[SCSI_MAXDEV][SCSI_SIZEWWID]; /* world wide id */
#endif /* SCSI_SCAM_ENABLE */
   SCSI_UEXACT16 tagEnable;                  /* tag enable flags           */
   void HIM_PTR moreLocked;                  /* more locked memory         */
   void HIM_PTR moreUnlocked;                /* more unlocked memory       */
   SCSI_TARGET_TSCB targetTSCBBusScan;       /* target TSCB for bus scan   */
   HIM_IOB HIM_PTR iobProtocolAutoConfig;    /* iob with protocol auto cfg */
   SCSI_UINT16 retryBusScan;                 /* current retry count        */
#if SCSI_DOMAIN_VALIDATION
   union
   {
      SCSI_UINT16 u16;
      struct 
      {
         SCSI_UINT8 dvLevel:2;               /* level of domain validation */
         SCSI_UINT8 dvState:5;               /* executing inquiry */
         SCSI_UINT8 dvPassed:1;              /* device passed domain validation */
         SCSI_UINT8 dvThrottle:3;            /* throttle state */
         SCSI_UINT8 dvFallBack:1;            /* fell back to lower xfer rate */
      } bits;
   } dvFlags[SCSI_MAXDEV];
#endif /* SCSI_DOMAIN_VALIDATION */
   SCSI_UINT8 allocBusAddressSize;           /* max bus address size for   */
                                             /* locked memory allocated in */
                                             /* HIMSetMemoryPointer()      */
#if !SCSI_DISABLE_PROBE_SUPPORT
   SCSI_UINT8 lastScsiIDProbed;              /* SCSI ID from last HIM_PROBE */
   SCSI_UINT8 lastScsiLUNProbed;             /* LUN from last HIM_PROBE    */
#endif /* !SCSI_DISABLE_PROBE_SUPPORT */   
   
   union
   {
      SCSI_UINT8 u8;
      struct 
      {
         SCSI_UINT8 initialized:1;           /* HIMInitialize has been called */
         SCSI_UINT8 osmFrozen:1;             /* OSM has been frozen by OSMEvent  */
#if SCSI_NSX_REPEATER + SCSI_PAC_NSX_REPEATER + SCSI_PAC_SEND_SSU
         SCSI_UINT8 ssuInProgress:1;               /* SSU command has been done or not */
#endif /* SCSI_NSX_REPEATER + SCSI_PAC_NSX_REPEATER + SCSI_PAC_SEND_SSU */
      } bits;
   } flags;
   SCSI_UINT8 domainValidationMethod;        /* specifies domain validation */
                                             /* method currently used.      */
#if SCSI_PAC_NSX_REPEATER
   SPI_SPEED_CONTROLLER speedController;
   SPI_SPEED_DEVICE speedDeviceTable[SCSI_MAXDEV];
   SCSI_UINT8 speedDeviceIndex; 
   SEGMENT Segments[SEGMENT_COUNT];
   int Segment_count;
#endif /* SCSI_PAC_NSX_REPEATER */
#if SCSI_DMA_SUPPORT
   SCSI_TARGET_TSCB dmaTargetTSCB;           /* target TSCB for DMA I/Os    */
#endif /* SCSI_DMA_SUPPORT */     
     
};
#define  SCSI_AF_initialized                 flags.bits.initialized 
#define  SCSI_AF_osmFrozen                   flags.bits.osmFrozen
#if SCSI_NSX_REPEATER + SCSI_PAC_NSX_REPEATER + SCSI_PAC_SEND_SSU
#define  SCSI_AF_ssuInProgress               flags.bits.ssuInProgress
#endif /* SCSI_NSX_REPEATER + SCSI_PAC_NSX_REPEATER + SCSI_PAC_SEND_SSU */

#if SCSI_DOMAIN_VALIDATION
#define  SCSI_AF_dvLevel(targetID)           dvFlags[(targetID)].bits.dvLevel
#define  SCSI_AF_dvState(targetID)           dvFlags[(targetID)].bits.dvState
#define  SCSI_AF_dvPassed(targetID)          dvFlags[(targetID)].bits.dvPassed
#define  SCSI_AF_dvThrottle(targetID)        dvFlags[(targetID)].bits.dvThrottle
#define  SCSI_AF_dvFallBack(targetID)        dvFlags[(targetID)].bits.dvFallBack
#endif /* SCSI_DOMAIN_VALIDATION */

/***************************************************************************
* SCSI_IOB_RESERVE structure definition
***************************************************************************/
#define  SCSI_MAX_CDBLEN   32
typedef struct SCSI_IOB_RESERVE_
{
   HIM_UEXACT8 cdb[SCSI_MAX_CDBLEN];         /* cdb buffer              */
   SCSI_HIOB hiob;                           /* hiob embedded           */
   HIM_IOB HIM_PTR iob;                      /* pointer to iob          */
}  SCSI_IOB_RESERVE;

/***************************************************************************
* More memory definitions for translation management layer
***************************************************************************/
#define  SCSI_INDEX_MORE_LOCKED     SCSI_MAX_MEMORY
#define  SCSI_INDEX_MORE_UNLOCKED   SCSI_MAX_MEMORY+1
#define  SCSI_SIZE_INQUIRY          64
#define  SCSI_SIZE_RWB              128

#define  SCSI_MORE_INQSG            0
#define  SCSI_MORE_RWBSG            0
#define  SCSI_MORE_INQDATA          sizeof(SCSI_BUS_ADDRESS)*2
#define  SCSI_MORE_RWBDATA          SCSI_MORE_INQDATA

#if   SCSI_DOMAIN_VALIDATION_ENHANCED
#define  SCSI_MORE_SNSDATA          SCSI_MORE_INQDATA+SCSI_SIZE_RWB
#else
#define  SCSI_MORE_SNSDATA          SCSI_MORE_INQDATA+SCSI_SIZE_INQUIRY
#endif   /* SCSI_DOMAIN_VALIDATION_ENHANCED */

#define  SCSI_MORE_IOBRSV           SCSI_MORE_SNSDATA+32
#define  SCSI_MORE_IOB              0
#define  SCSI_MORE_SPECIAL_IOB      sizeof(HIM_IOB)
#define  SCSI_MORE_INQCDB           SCSI_MORE_SPECIAL_IOB+sizeof(HIM_IOB)
#define  SCSI_MORE_RWBCDB           SCSI_MORE_INQCDB
#define  SCSI_MORE_LOCKED           SCSI_MORE_IOBRSV+sizeof(SCSI_IOB_RESERVE)

#if SCSI_DOMAIN_VALIDATION_ENHANCED
#define  SCSI_MORE_REFDATA          SCSI_MORE_INQCDB+10
#else
#define  SCSI_MORE_REFDATA          SCSI_MORE_INQCDB+6
#endif   /* SCSI_DOMAIN_VALIDATION_ENHANCED */

#if SCSI_DOMAIN_VALIDATION
#define  SCSI_MORE_DV_TRANSPORT_SPECIFIC  SCSI_MORE_REFDATA+16*SCSI_SIZE_INQUIRY
#define  SCSI_MORE_TRANSPORT_SPECIFIC   SCSI_MORE_DV_TRANSPORT_SPECIFIC+sizeof(HIM_TS_SCSI)
#else
#define  SCSI_MORE_TRANSPORT_SPECIFIC   SCSI_MORE_REFDATA+0
#endif   /* SCSI_DOMAIN_VALIDATION */

#if SCSI_PAC_NSX_REPEATER || SCSI_NSX_REPEATER
#define  NSX_REPEATER_PACKET_SZ     8    /* NSX returns an 8 byte packet per repeater */
#define  MAX_NSX_PACKETS            8    /* Set to eight to match SMGR */
#define  MAX_NSX_REPEATER_DATA      (NSX_REPEATER_PACKET_SZ * MAX_NSX_PACKETS)     
#define  SCSI_MORE_NSX_DATA         (SCSI_MORE_TRANSPORT_SPECIFIC + sizeof(HIM_TS_SCSI))
#define  SCSI_MORE_UNLOCKED         (SCSI_MORE_NSX_DATA + MAX_NSX_REPEATER_DATA)
#else
#define  SCSI_MORE_UNLOCKED         (SCSI_MORE_TRANSPORT_SPECIFIC + 0)
#endif /* SCSI_PAC_NSX_REPEATER || SCSI_NSX_REPEATER */

#define  SCSI_MORE_LOCKED_ALIGN     2*sizeof(SCSI_BUS_ADDRESS)-1
#define  SCSI_MORE_UNLOCKED_ALIGN   2*sizeof(SCSI_BUS_ADDRESS)-1

/***************************************************************************
* SCSI_AF_dvLevel (per device)
***************************************************************************/
#define  SCSI_DV_DISABLE            0        /* disable domain validation  */
#define  SCSI_DV_BASIC              1        /* enable Domain Validation level 1 */
#define  SCSI_DV_ENHANCED           2        /* enable Domain Validation level 2  */

/***************************************************************************
* SCSI_AF_dvState (per device)
***************************************************************************/
#define  SCSI_DV_READY              0        /* ready to exercise          */
#define  SCSI_DV_INQUIRY            1        /* executing inquiry          */
#define  SCSI_DV_WRITEBUFFER        2        /* executing write buffer     */
#define  SCSI_DV_READBUFFER         3        /* executing read buffer      */
#define  SCSI_DV_DONE               4        /* done with DV exercise      */

/***************************************************************************
* SCSI_AF_dvThrottle
***************************************************************************/
#define SCSI_DE_WIDE                0        /* DE/wide stage (initial)    */
#define SCSI_SE_WIDE                1        /* SE/wide stage              */
#define SCSI_SE_NARROW              2        /* SE/narrow stage            */
#define SCSI_SE_WIDE_REPEAT         3        /* SE/wide stage repeat (initial) */
#define SCSI_SE_NARROW_REPEAT       4        /* SE/narrow stage repeat (initial) */
#define SCSI_ASYNC_NARROW           5        /* async/narrow               */

/***************************************************************************
*  CHIM SCSI interface function prototypes
***************************************************************************/
/* XLMINIT.C */
void SCSIGetFunctionPointers(HIM_FUNC_PTRS HIM_PTR,HIM_UINT16);
HIM_HOST_ID SCSIGetNextHostDeviceType(HIM_UINT16,HIM_UINT8 HIM_PTR,HIM_HOST_ID HIM_PTR);
HIM_TASK_SET_HANDLE SCSICreateInitializationTSCB(void HIM_PTR);
void SCSIGetConfiguration (HIM_TASK_SET_HANDLE,HIM_CONFIGURATION HIM_PTR,HIM_HOST_ID);
HIM_UINT8 SCSISetConfiguration (HIM_TASK_SET_HANDLE,HIM_CONFIGURATION HIM_PTR,HIM_HOST_ID);
HIM_UINT32 SCSISizeAdapterTSCB (HIM_TASK_SET_HANDLE,HIM_HOST_ID);
HIM_TASK_SET_HANDLE SCSICreateAdapterTSCB (HIM_TASK_SET_HANDLE,void HIM_PTR,void HIM_PTR,HIM_HOST_ADDRESS,HIM_HOST_ID);
HIM_UINT8 SCSISetupAdapterTSCB(HIM_TASK_SET_HANDLE,HIM_OSM_FUNC_PTRS HIM_PTR,HIM_UINT16);
HIM_UINT32 SCSICheckMemoryNeeded(HIM_TASK_SET_HANDLE,HIM_TASK_SET_HANDLE,HIM_HOST_ID,HIM_UINT16,HIM_UINT8 HIM_PTR,HIM_UINT32 HIM_PTR,HIM_UINT32 HIM_PTR,HIM_UEXACT32 HIM_PTR);
HIM_UINT8 SCSISetMemoryPointer(HIM_TASK_SET_HANDLE,HIM_UINT16,HIM_UINT8,void HIM_PTR,HIM_UINT32);
HIM_UINT8 SCSIVerifyAdapter(HIM_TASK_SET_HANDLE);
HIM_UINT8 SCSIInitialize (HIM_TASK_SET_HANDLE);
HIM_UINT32 SCSISizeTargetTSCB(HIM_TASK_SET_HANDLE);
HIM_UINT8   SCSICheckTargetTSCBNeeded(HIM_TASK_SET_HANDLE,HIM_UINT16);
HIM_TASK_SET_HANDLE SCSICreateTargetTSCB(HIM_TASK_SET_HANDLE,HIM_UINT16,void HIM_PTR);

/* XLMTASK.C */
void SCSIDisableIRQ (HIM_TASK_SET_HANDLE);
void SCSIEnableIRQ (HIM_TASK_SET_HANDLE);
HIM_UINT8 SCSIPollIRQ (HIM_TASK_SET_HANDLE);
HIM_UINT8 SCSIFrontEndISR (HIM_TASK_SET_HANDLE);
void SCSIBackEndISR (HIM_TASK_SET_HANDLE);
void SCSIQueueIOB (HIM_IOB HIM_PTR);
HIM_UINT8 SCSIPowerEvent (HIM_TASK_SET_HANDLE,HIM_UINT8);
HIM_UINT8 SCSIValidateTargetTSH (HIM_TASK_SET_HANDLE);
HIM_UINT8 SCSIxScamValidateLun(SCSI_TARGET_TSCB HIM_PTR);
HIM_UINT8 SCSIxNonScamValidateLun(SCSI_TARGET_TSCB HIM_PTR);
HIM_UINT8 SCSIClearTargetTSH (HIM_TASK_SET_HANDLE);
void SCSISaveState (HIM_TASK_SET_HANDLE,void HIM_PTR);
void SCSIRestoreState (HIM_TASK_SET_HANDLE,void HIM_PTR);
HIM_UINT8 SCSIProfileAdapter (HIM_TASK_SET_HANDLE,HIM_ADAPTER_PROFILE HIM_PTR);
HIM_UINT8 SCSIReportAdjustableAdapterProfile (HIM_TASK_SET_HANDLE,HIM_ADAPTER_PROFILE HIM_PTR);
HIM_UINT8 SCSIAdjustAdapterProfile (HIM_TASK_SET_HANDLE,HIM_ADAPTER_PROFILE HIM_PTR);
HIM_UINT8 SCSIProfileTarget(HIM_TASK_SET_HANDLE,HIM_TARGET_PROFILE HIM_PTR);
HIM_UINT8 SCSIReportAdjustableTargetProfile(HIM_TASK_SET_HANDLE,HIM_TARGET_PROFILE HIM_PTR);
HIM_UINT8 SCSIAdjustTargetProfile(HIM_TASK_SET_HANDLE,HIM_TARGET_PROFILE HIM_PTR);
HIM_UINT32 SCSIGetNVSize(HIM_TASK_SET_HANDLE);
HIM_UINT8 SCSIGetNVOSMSegment(HIM_TASK_SET_HANDLE,HIM_UINT32 HIM_PTR,HIM_UINT32 HIM_PTR);
HIM_UINT8 SCSIPutNVData(HIM_TASK_SET_HANDLE,HIM_UINT32,void HIM_PTR,HIM_UINT32);
HIM_UINT8 SCSIGetNVData(HIM_TASK_SET_HANDLE,void HIM_PTR,HIM_UINT32,HIM_UINT32);
HIM_UINT8 SCSIProfileNexus(HIM_TASK_SET_HANDLE,HIM_NEXUS_PROFILE HIM_PTR);
HIM_UINT8 SCSIClearNexusTSH(HIM_TASK_SET_HANDLE);
HIM_UINT8 SCSIProfileNode(HIM_TASK_SET_HANDLE,HIM_NODE_PROFILE HIM_PTR);
HIM_UINT8 SCSIReportAdjustableNodeProfile(HIM_TASK_SET_HANDLE,HIM_NODE_PROFILE HIM_PTR);
HIM_UINT8 SCSIAdjustNodeProfile(HIM_TASK_SET_HANDLE,HIM_NODE_PROFILE HIM_PTR);
HIM_UINT8 SCSISetOSMNodeContext(HIM_TASK_SET_HANDLE,void HIM_PTR);

#if defined(SCSI_XLM_DEFINE)
/***************************************************************************
*  SCSI_HOST_TYPE table setup
***************************************************************************/
#if (! SCSI_ARO_SUPPORT)
SCSI_HOST_TYPE SCSIHostType[] =
{
#if SCSI_AIC78XX
   { 0x50789004, 0xF8FFFFFF },                  /* Dagger plus etc.  */
   { 0x50759004, 0xF8FFFFFF },                  /* Dagger etc.       */
   { 0x60789004, 0xFFFFFFFF },                  /* Talon etc.        */
   { 0x61789004, 0xFFFFFFFF },                  /* Talon etc.        */
   { 0x62789004, 0xFFFFFFFF },                  /* Talon etc.        */
   { 0x64789004, 0xFFFFFFFF },                  /* Talon etc.        */
   { 0x65789004, 0xFFFFFFFF },                  /* Talon etc.        */
   { 0x66789004, 0xFFFFFFFF },                  /* Talon etc.        */
   { 0x67789004, 0xFFFFFFFF },                  /* Talon etc.        */
   { 0x60759004, 0xFFFFFFFF },                  /* Talon Cardbus mode*/
   { 0x78609004, 0xFFFFFFFF },                  /* Talon CHIM mode   */
#if SCSI_2930CVAR_SUPPORT   
   { 0x38609004, 0xFFFFFFFF },                  /* 2930CVAR          */
#endif
   { 0x70789004, 0xFFFFFFFF },                  /* Lance etc.        */
   { 0x71789004, 0xFFFFFFFF },                  /* Lance etc.        */
   { 0x72789004, 0xFFFFFFFF },                  /* Lance etc.        */
   { 0x74789004, 0xFFFFFFFF },                  /* Lance etc.        */
   { 0x75789004, 0xFFFFFFFF },                  /* Lance etc.        */
   { 0x76789004, 0xFFFFFFFF },                  /* Lance etc.        */
#if SCSI_4944IDFIX_ENABLE
   { 0x3B789004, 0xFFFFFFFF },                  /* 4944 ID fix       */
   { 0xEC789004, 0xFFFFFFFF },                  /* 4944 ID fix       */
#endif
   { 0x77789004, 0xFFFFFFFF },                  /* Lance etc.        */
   { 0x80789004, 0xFFFFFFFF },                  /* Katana etc.       */
   { 0x81789004, 0xFFFFFFFF },                  /* Katana etc.       */
   { 0x82789004, 0xFFFFFFFF },                  /* Katana etc.       */
   { 0x84789004, 0xFFFFFFFF },                  /* Katana etc.       */
   { 0x85789004, 0xFFFFFFFF },                  /* Katana etc.       */
   { 0x86789004, 0xFFFFFFFF },                  /* Katana etc.       */
   { 0x87789004, 0xFFFFFFFF },                  /* Katana etc.       */
#if SCSI_2930UW_SUPPORT   
   { 0x88789004, 0xFFFFFFFF },                  /* 2930UW            */
#endif   
   { 0x78959004, 0xFFFFFFFF },                  /* Excalibur etc.    */
#endif
#if SCSI_AICBAYONET
   { 0x00109005, 0xFFFFFFFF },                  /* Bayonet ultra2, type field 0 etc.   */
   { 0x00209005, 0xFFFFFFFF },                  /* Bayonet ultra, type field 0 etc.    */
   { 0x00309005, 0xFFFFFFFF },                  /* Bayonet fast, type field 0 etc.     */
   { 0x001F9005, 0xFFFFFFFF },                  /* Bayonet ultra2, type field F etc.   */
   { 0x002F9005, 0xFFFFFFFF },                  /* Bayonet ultra, type field F etc.    */
   { 0x003F9005, 0xFFFFFFFF },                  /* Bayonet fast, type field F etc.     */
   { 0x00509005, 0xFFFFFFFF },                  /* Scimitar ultra2, type field 0 etc.  */
   { 0x00519005, 0xFFFFFFFF },                  /* Scimitar ultra2, type field 1 etc.  */
   { 0x00609005, 0xFFFFFFFF },                  /* Scimitar ultra, type field 0 etc.   */
   { 0x00709005, 0xFFFFFFFF },                  /* Scimitar fast, type field 0 etc.    */
   { 0x005F9005, 0xFFFFFFFF },                  /* Scimitar ultra2, type field F etc.  */
   { 0x006F9005, 0xFFFFFFFF },                  /* Scimitar ultra, type field F etc.   */
   { 0x007F9005, 0xFFFFFFFF },                  /* Scimitar fast, type field F etc.    */
#endif
#if SCSI_AICTRIDENT
   { 0x008F9005, 0xFFFFFFFF },                  /* Trident u160m, type field F etc.    */
   { 0x009F9005, 0xFFFFFFFF },                  /* Trident ultra2, type field F etc.   */
   { 0x00AF9005, 0xFFFFFFFF },                  /* Trident ultra, type field F etc.    */
   { 0x00BF9005, 0xFFFFFFFF },                  /* Trident fast, type field F etc.     */
   { 0x00809005, 0xFFFFFFFF },                  /* Trident u160m, type field 0 etc.    */
   { 0x00819005, 0xFFFFFFFF },                  /* Trident u160m, type field 1 etc.    */
   { 0x00909005, 0xFFFFFFFF },                  /* Trident ultra2, type field 0 etc.   */
   { 0x00A09005, 0xFFFFFFFF },                  /* Trident ultra, type field 0 etc.    */
   { 0x00B09005, 0xFFFFFFFF },                  /* Trident fast, type field 0 etc.     */
   { 0x00CF9005, 0xFFFFFFFF },                  /* Trident II u160m, type field F etc. */
   { 0x00DF9005, 0xFFFFFFFF },                  /* Trident II ultra2, type field F etc.*/
   { 0x00EF9005, 0xFFFFFFFF },                  /* Trident II ultra, type field F etc. */
   { 0x00FF9005, 0xFFFFFFFF },                  /* Trident II fast, type field F etc.  */
   { 0x00C09005, 0xFFFFFFFF },                  /* Trident II u160m, type field 0 etc. */
   { 0x00D09005, 0xFFFFFFFF },                  /* Trident II ultra2, type field 0 etc.*/
   { 0x00E09005, 0xFFFFFFFF },                  /* Trident II ultra, type field 0 etc. */
   { 0x00F09005, 0xFFFFFFFF },                  /* Trident II fast, type field 0 etc.  */
#endif
   { 0x0, 0x0 }            /* This initializer serves as the delimeter id for */
                           /* the scsi host type.  And for C syntax, the last */
                           /* initializer of the initializer-list does not    */
                           /* need to have a comma.                           */
};
#else
SCSI_HOST_TYPE SCSIHostType[] =
{
   { 0x83789004, 0xFFFFFFFF },                  /* Katana etc.       */
   { 0x78959004, 0xFFFFFFFF },                  /* Excalibur etc.    */
   { 0x78939004, 0xFFFFFFFF }                   /* Excalibur etc.    */
};
#endif

/* Sub System Sub Vendor ID's for Talon and Excalibur.  Both of these   */
/* chips have fixed Device ID's (Talon if its in "CHIM" mode) and       */
/* programmable subsystem ids as specified in the following table.      */
/* Note that the extern reference to this array must be updated whenever*/
/* adding an id here.                                                   */
#if (! SCSI_ARO_SUPPORT)
SCSI_HOST_TYPE SCSISubSystemSubVendorType[] =
{
   { 0x78609004, 0xFFFFFFFF },                  /* Talon CHIM mode   */
   { 0x78619004, 0xFFFFFFFF },                  /* Talon CHIM mode   */
   { 0x78629004, 0xFFFFFFFF },                  /* Talon CHIM mode   */
   { 0x78649004, 0xFFFFFFFF },                  /* Talon CHIM mode   */
   { 0x78659004, 0xFFFFFFFF },                  /* Talon CHIM mode   */
   { 0x78669004, 0xFFFFFFFF },                  /* Talon CHIM mode   */
   { 0x78679004, 0xFFFFFFFF },                  /* Talon CHIM mode   */
#if SCSI_2930CVAR_SUPPORT   
   { 0x38689004, 0xFFFFFFFF },                  /* 2930CVAR          */
   { 0x38699004, 0xFFFFFFFF },                  /* 2930CVAR          */
#endif
   { 0x78909004, 0xFFFFFFFF },                  /* Excalibur etc.    */
   { 0x78919004, 0xFFFFFFFF },                  /* Excalibur etc.    */
   { 0x78929004, 0xFFFFFFFF },                  /* Excalibur etc.    */
   { 0x78949004, 0xFFFFFFFF },                  /* Excalibur etc.    */
   { 0x78959004, 0xFFFFFFFF },                  /* Excalibur etc.    */
   { 0x78969004, 0xFFFFFFFF },                  /* Excalibur etc.    */
   { 0x78979004, 0xFFFFFFFF }                   /* Excalibur etc.    */
};
#else
SCSI_HOST_TYPE SCSISubSystemSubVendorType[] =
{
   { 0x78939004, 0xFFFFFFFF },                  /* Excalibur etc.    */
};
#endif

/* Sub System Sub Vendor ID's for Bayonet type that we want to exclude  */
/* for support.  For all Bayonet and later type devices then the        */
/* subsystem ID is programable according to the new PCI device ID spec. */
/* This subsystem ID value can be various combination depending on the  */
/* feature of the hardware.  We use the following table to filter out   */
/* a device in which we don't want to support this hardware.            */
/* Note that the extern reference to this array must be updated whenever*/
/* adding an id not to support here.                                    */
#if (! SCSI_ARO_SUPPORT)
SCSI_HOST_TYPE SCSIBayonetSubSystemSubVendorType[] =
{
#if (! SCSI_2930U2_SUPPORT)
   { 0x01809005, 0xFFFFFFFF },                  /* AHA-2930U2        */
#endif /* (! SCSI_2930U2_SUPPORT) */
   {        0x0,        0x0 }                   /* delimeter id      */
};
#else /* SCSI_ARO_SUPPORT */
SCSI_HOST_TYPE SCSIBayonetSubSystemSubVendorType[] =
{
   {        0x0,        0x0 }                   /* delimeter id      */
};
#endif /* (! SCSI_ARO_SUPPORT) */

/* Sub System Sub Vendor ID's for Trident type that we want to exclude  */
/* for support.  For Trident device, the subsystem ID is programable    */
/* according to the new PCI device ID spec.  This subsystem ID value    */
/* can be various combination depending on the feature of the hardware. */
/* We use the following table to filter out a device in which we don't  */
/* want to support this hardware.  Note that the extern reference to    */
/* this array must be updated whenever adding an id not to support here.*/
#if (!SCSI_ARO_SUPPORT)
SCSI_HOST_TYPE SCSITridentSubSystemSubVendorType[] =
{
#if (!SCSI_OEM1_SUPPORT)
   { 0x00000E11, 0x0000FFFF },                  /* Exclude OEM1's SubVendor ID*/
#endif
   {        0x0,        0x0 }                   /* delimeter id         */
};
#else /* SCSI_ARO_SUPPORT */
SCSI_HOST_TYPE SCSITridentSubSystemSubVendorType[] =
{
   {        0x0,        0x0 }                   /* delimeter id      */
};
#endif /* (! SCSI_ARO_SUPPORT) */

/* Macros to identify which device vendor id's require the subsystem id */
/* double checking above. */
#define     SCSI_EXCALIBUR_EXACT       0x78959004  /* Id for excalibur */
#define     SCSI_TALON_CHIM_MODE_EXACT 0x78609004  /* Id for Talon in CHIM mode */
#if SCSI_2930CVAR_SUPPORT   
#define     SCSI_2930CVAR              0x38609004  /* Id for 2930CVAR */
#endif
#if SCSI_2930UW_SUPPORT   
#define     SCSI_2930UW                0x78889004  /* Id for 2930UW */
#endif

/***************************************************************************
*  CHIM SCSI interface function pointer table setup
***************************************************************************/
HIM_FUNC_PTRS SCSIFuncPtrs =
{
   4,                                           /* version number */
   SCSIGetNextHostDeviceType,
   SCSICreateInitializationTSCB,
   SCSIGetConfiguration,
   SCSISetConfiguration,
   SCSISizeAdapterTSCB,
   SCSICreateAdapterTSCB,
   SCSISetupAdapterTSCB,
   SCSICheckMemoryNeeded,
   SCSISetMemoryPointer,
   SCSIVerifyAdapter,
   SCSIInitialize,
   SCSISizeTargetTSCB,
   SCSICheckTargetTSCBNeeded,
   SCSICreateTargetTSCB,
   SCSIDisableIRQ,
   SCSIEnableIRQ,
   SCSIPollIRQ,
   SCSIFrontEndISR,
   SCSIBackEndISR,
   SCSIQueueIOB,
   SCSIPowerEvent,
   SCSIValidateTargetTSH,
   SCSIClearTargetTSH,
   SCSISaveState,
   SCSIRestoreState,
   SCSIProfileAdapter,
   SCSIReportAdjustableAdapterProfile,
   SCSIAdjustAdapterProfile,
   SCSIProfileTarget,
   SCSIReportAdjustableTargetProfile,
   SCSIAdjustTargetProfile,
   SCSIGetNVSize,
   SCSIGetNVOSMSegment,
   SCSIPutNVData,
   SCSIGetNVData,
   SCSIProfileNexus,
   SCSIClearNexusTSH,
   SCSIProfileNode,
   SCSIReportAdjustableNodeProfile,
   SCSIAdjustNodeProfile,
   SCSISetOSMNodeContext
};

/***************************************************************************
*  Default configuration setup
***************************************************************************/
HIM_CONFIGURATION SCSIConfiguration =
{
   5,                                     /* versionNumber              */
   SCSI_MAXSCBS,                          /* maxInternalIOBlocks        */
   SCSI_MAXDEV,                           /* maxTargets                 */
   0xFFFFFFFF,                            /* maxSGDescriptors           */
   0xFFFFFFFF,                            /* maxTransferSize            */
   HIM_IOSPACE,                           /* memoryMapped               */
#if SCSI_MULTIPLEID_SUPPORT
   HIM_MAX_SCSI_ADAPTER_IDS,              /* targetNumIDs               */    
#else
   1,                                     /* targetNumIDs               */
#endif /* SCSI_MULTIPLEID_SUPPORT */
#if SCSI_TARGET_OPERATION
   SCSI_MAXNEXUSHANDLES,                  /* targetNumNexusTaskSetHandles */
   SCSI_MAXNODES,                         /* targetNumNodeTaskSetHandles  */
#else
   0,                                     /* targetNumNexusTaskSetHandles */
   0,                                     /* targetNumNodeTaskSetHandles  */
#endif /* SCSI_TARGET_OPERATION */ 
#if SCSI_INITIATOR_OPERATION
   HIM_TRUE,                              /* initiatorMode              */
#else
   HIM_FALSE,                             /* initiatorMode              */
#endif
#if SCSI_TARGET_OPERATION
   HIM_TRUE,                              /* targetMode                 */
#else
   HIM_FALSE,                             /* targetMode                 */
#endif
   sizeof(SCSI_IOB_RESERVE),              /* iobReserveSize             */
   sizeof(SCSI_STATE),                    /* State Size                 */
   2,                                     /* maxIOHandles               */
   HIM_TRUE,                              /* virtualDataAccess          */
   HIM_TRUE,                              /* needPhysicalAddr           */
   HIM_TRUE,                              /* busMaster                  */
   32                                     /* allocBusAddressSize        */
};

/***************************************************************************
*  Default adapter profile setup
***************************************************************************/
HIM_ADAPTER_PROFILE SCSIDefaultAdapterProfile =
{
   HIM_VERSION_ADAPTER_PROFILE,           /* AP_Version                 */
   HIM_TRANSPORT_SCSI,                    /* AP_Transport               */
   {
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,    /* AP_WorldWideID             */
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
   },
   HIM_MAX_WWID_SIZE,                     /* AP_WWIDLength              */
   1,                                     /* AP_NumBuses                */
   HIM_TRUE,                              /* AP_VirtualDataAccess       */
   HIM_TRUE,                              /* AP_NeedPhysicalAddr        */
   HIM_TRUE,                              /* AP_BusMaster               */
   64-1,                                  /* AP_AlignmentMask           */
   OSM_BUS_ADDRESS_SIZE,                  /* AP_AddressableRange        */
   0,                                     /* AP_GroupNum                */
   15,                                    /* AP_AutoConfigTimeout       */
   2,                                     /* AP_MaxIOHandles            */
#if SCSI_TARGET_OPERATION
   HIM_TRUE,                              /* AP_TargetMode              */
#else
   HIM_FALSE,                             /* AP_TargetMode              */
#endif
#if SCSI_INITIATOR_OPERATION
   HIM_TRUE,                              /* AP_InitiatorMode           */
#else
   HIM_FALSE,                             /* AP_InitiatorMode           */
#endif
   HIM_TRUE,                              /* AP_CleanSG                 */
   HIM_FALSE,                             /* AP_Graphing                */
   HIM_TRUE,                              /* AP_CannotCross4G           */
   HIM_FALSE,                             /* AP_BiosActive              */
   HIM_FALSE,                             /* AP_CacheLineStreaming      */
   HIM_EXTENDED_TRANS,                    /* AP_ExtendedTrans           */
   HIM_IOSPACE,                           /* AP_MemoryMapped            */
   SCSI_MAXDEV,                           /* AP_MaxTargets              */
   SCSI_MAXSCBS,                          /* AP_MaxInternalIOBlocks     */
   0xFFFFFFFF,                            /* AP_MaxSGDescriptors        */
   0xFFFFFFFF,                            /* AP_MaxTransferSize         */
   sizeof(SCSI_STATE),                    /* AP_StateSize               */
   sizeof(SCSI_IOB_RESERVE),              /* AP_IOBReserveSize          */
   HIM_TRUE,                              /* AP_FIFOSeparateRWThreshold */
   HIM_FALSE,                             /* AP_FIFOSeparateRWThresholdEnable*/
   100,                                   /* AP_FIFOWriteThreshold      */
   100,                                   /* AP_FIFOReadThreshold       */
   100,                                   /* AP_FIFOThreshold           */
   SCSI_RESET_DELAY_DEFAULT,              /* AP_ResetDelay              */
   0,                                     /* AP_HIMVersion              */
   0,                                     /* AP_HardwareVersion         */
   0,                                     /* AP_HardwareVariant         */
   0,                                     /* AP_LowestScanTarget        */
   32,                                    /* AP_AllocBusAddressSize     */
   0,                                     /* AP_indexWithinGroup        */
   HIM_FALSE,                             /* AP_CmdCompleteIntrThresholdSupport*/
   0,                                     /* AP_IntrThreshold           */
   HIM_TRUE,                              /* AP_SaveRestoreSequencer    */
   HIM_FALSE,                             /* AP_ClusterEnabled          */  
   0,                                     /* AP_TargetNumNexusTaskSetHandles  */
   0,                                     /* AP_TargetNumNodeTaskSetHandles   */
   HIM_TRUE,                              /* AP_TargetDisconnectAllowed       */
   HIM_TRUE,                              /* AP_TargetTagEnable               */
   HIM_FALSE,                             /* AP_OutOfOrderTransfers           */
   0,                                     /* AP_NexusHandleThreshold          */
   0,                                     /* AP_EC_IOBThreshold               */
   0,                                     /* AP_TargetAvailableEC_IOBCount    */
   0,                                     /* AP_TargetAvailableNexusCount     */
   {
      HIM_TRUE,                           /* AP_SCSITargetAbortTask           */
      HIM_TRUE,                           /* AP_SCSITargetClearTaskSet        */
      HIM_FALSE,                          /* AP_SCSITargetTerminateTask       */
      HIM_FALSE,                          /* AP_SCSI3TargetClearACA           */
      HIM_FALSE                           /* AP_SCSI3TargetLogicalUnitReset   */
   },
#if SCSI_TARGET_OPERATION
   1,                                     /* AP_TargetNumIDs            */   
#else
   0,                                     /* AP_TargetNumIDs            */
#endif /* SCSI_TARGET_OPERATION */
   0,                                     /* AP_TargetInternalEstablishConnectionIOBlocks */
   {
      {
         0,                                  /* AP_SCSIForceWide           */
         0,                                  /* AP_SCSIForceNoWide         */
         0,                                  /* AP_SCSIForceSynch          */
         0,                                  /* AP_SCSIForceNoSynch        */
         7,                                  /* AP_SCSIAdapterID           */
         HIM_SCSI_NORMAL_SPEED,              /* AP_SCSISpeed               */
         8,                                  /* AP_SCSIWidth               */
         {                                   /* AP_SCSINumberLuns[32]      */
            SCSI_MAXLUN,SCSI_MAXLUN,SCSI_MAXLUN,SCSI_MAXLUN,
            SCSI_MAXLUN,SCSI_MAXLUN,SCSI_MAXLUN,SCSI_MAXLUN,
            SCSI_MAXLUN,SCSI_MAXLUN,SCSI_MAXLUN,SCSI_MAXLUN,
            SCSI_MAXLUN,SCSI_MAXLUN,SCSI_MAXLUN,SCSI_MAXLUN,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
         },                        
         HIM_FALSE,                          /* AP_SCSIDisableParityErrors */
         256,                                /* AP_SCSISelectionTimeout    */
         HIM_SCSI_UNKNOWN_MODE,              /* AP_SCSITransceiverMode     */
         HIM_SCSI_DISABLE_DOMAIN_VALIDATION, /* AP_SCSIDomainValidationMethod */
         HIM_FALSE,                          /* AP_SCSIPPRSupport          */                  
         HIM_FALSE,                          /* AP_SCSIExpanderDetection   */
/* Target mode specific fields */
         HIM_SCSI_2,                         /* AP_SCSIHostTargetVersion      */
         HIM_TRUE,                           /* AP_SCSI2_IdentifyMsgRsv       */
         HIM_FALSE,                          /* AP_SCSI2_TargetRejectLuntar   */
         12,                                 /* AP_SCSIGroup6CDBSize          */
         12,                                 /* AP_SCSIGroup7CDBSize          */
         HIM_TRUE,                           /* AP_SCSITargetIgnoreWideResidue */
         HIM_FALSE,                          /* AP_SCSITargetEnableSCSI1Selection */
         HIM_FALSE,                          /* AP_SCSITargetInitNegotiation  */
         200,                                /* AP_SCSITargetMaxSpeed         */
         200,                                /* AP_SCSITargetDefaultSpeed     */
         15,                                 /* AP_SCSITargetMaxOffset        */
         15,                                 /* AP_SCSITargetDefaultOffset    */
         16,                                 /* AP_SCSITargetMaxWidth         */
         16,                                 /* AP_SCSITargetDefaultWidth     */
         0                                   /* AP_SCSITargetAdapterIDMask    */
      }
   },
   HIM_FALSE                                 /* AP_OverrideOSMNVRAMRoutines   */
};

/***************************************************************************
*  Adjustable adapter profile mask setup
***************************************************************************/
HIM_ADAPTER_PROFILE SCSIAdjustableAdapterProfile =
{
   0,                                     /* AP_Version                 */
   0,                                     /* AP_Transport               */
   {
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,    /* AP_WorldWideID             */
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
   },
   0,                                     /* AP_WWIDLength              */
   0,                                     /* AP_NumBuses                */
   HIM_FALSE,                             /* AP_VirtualDataAccess       */
   HIM_FALSE,                             /* AP_NeedPhysicalAddr        */
   HIM_FALSE,                             /* AP_BusMaster               */
   0,                                     /* AP_AlignmentMask           */
   0,                                     /* AP_AddressableRange        */
   0,                                     /* AP_GroupNum                */
   0,                                     /* AP_AutoConfigTimeout       */
   0,                                     /* AP_MaxIOHandles            */
   HIM_FALSE,                             /* AP_TargetMode              */
   HIM_FALSE,                             /* AP_InitiatorMode           */
   HIM_FALSE,                             /* AP_CleanSG                 */
   HIM_FALSE,                             /* AP_Graphing                */
   HIM_FALSE,                             /* AP_CannotCross4G           */
   HIM_FALSE,                             /* AP_BiosActive              */
   HIM_FALSE,                             /* AP_CacheLineStreaming      */
   0,                                     /* AP_ExtendedTrans           */
   0,                                     /* AP_MemoryMapped            */
   0,                                     /* AP_MaxTargets              */
   0,                                     /* AP_MaxInternalIOBlocks     */
   0,                                     /* AP_MaxSGDescriptors        */
   0,                                     /* AP_MaxTransferSize         */
   0,                                     /* AP_StateSize               */
   0,                                     /* AP_IOBReserveSize          */
   HIM_FALSE,                             /* AP_FIFOSeparateRWThreshold */
   HIM_TRUE,                              /* AP_FIFOSeparateRWThresholdEnable */
   1,                                     /* AP_FIFOWriteThreshold      */
   1,                                     /* AP_FIFOReadThreshold       */ 
   1,                                     /* AP_FIFOThreshold           */
   1,                                     /* AP_ResetDelay              */
   0,                                     /* AP_HIMVersion              */
   0,                                     /* AP_HardwareVersion         */
   0,                                     /* AP_HardwareVariant         */
   0,                                     /* AP_LowestScanTarget        */
   0,                                     /* AP_AllocBusAddressSize     */
#if SCSI_ARO_SUPPORT
   1,                                     /* AP_indexWithinGroup        */
#else /* !SCSI_ARO_SUPPORT */
   0,                                     /* AP_indexWithinGroup        */
#endif /* SCSI_ARO_SUPPORT */
   HIM_TRUE,                              /* AP_CmdCompleteIntrThresholdSupport*/
   1,                                     /* AP_IntrThreshold           */
   HIM_TRUE,                              /* AP_SaveRestoreSequencer    */
   HIM_FALSE,                             /* AP_ClusterEnabled          */
   0,                                     /* AP_TargetNumNexusTaskSetHandles  */
   0,                                     /* AP_TargetNumNodeTaskSetHandles   */
   HIM_TRUE,                              /* AP_TargetDisconnectAllowed       */
   HIM_TRUE,                              /* AP_TargetTagEnable               */
   HIM_FALSE,                             /* AP_OutOfOrderTransfers           */
   0,                                     /* AP_NexusHandleThreshold          */
   0,                                     /* AP_EC_IOBThreshold               */
   0,                                     /* AP_TargetAvailableEC_IOBCount    */
   0,                                     /* AP_TargetAvailableNexusCount     */
   {
      HIM_TRUE,                           /* AP_SCSITargetAbortTask           */
      HIM_TRUE,                           /* AP_SCSITargetClearTaskSet        */
      HIM_TRUE,                           /* AP_SCSITargetTerminateTask       */
      HIM_TRUE,                           /* AP_SCSI3TargetClearACA           */
      HIM_TRUE                            /* AP_SCSI3TargetLogicalUnitReset   */
   },
   0,                                     /* AP_TargetNumIDs            */
   1,                                     /* AP_TargetInternalEstablishConnectionIOBlocks */
   {
      {
         0,                               /* AP_SCSIForceWide           */
         0,                               /* AP_SCSIForceNoWide         */
         0,                               /* AP_SCSIForceSynch          */
         0,                               /* AP_SCSIForceNoSynch        */
         1,                               /* AP_SCSIAdapterID           */
         0,                               /* AP_SCSISpeed               */
         0,                               /* AP_SCSIWidth               */
         {                                /* AP_SCSINumberLuns[32]      */
            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
         },                        
         HIM_TRUE,                        /* AP_SCSIDisableParityErrors */
         1,                               /* AP_SCSISelectionTimeout    */
         0,                               /* AP_SCSITransceiverMode     */ 
         1,                               /* AP_SCSIDomainValidationMethod */
         HIM_FALSE,                       /* AP_SCSIPPRSupport          */          
         HIM_FALSE,                       /* AP_SCSIExpanderDetection   */
         1,                               /* AP_SCSIHostTargetVersion   */
         HIM_TRUE,                        /* AP_SCSI2_IdentifyMsgRsv    */
         HIM_TRUE,                        /* AP_SCSI2_TargetRejectLuntar*/
         1,                               /* AP_SCSIGroup6CDBSize       */
         1,                               /* AP_SCSIGroup7CDBSize       */
         HIM_TRUE,                        /* AP_SCSITargetIgnoreWideResidue */
         HIM_TRUE,                        /* AP_SCSITargetEnableSCSI1Selection */
         HIM_FALSE,                       /* AP_SCSITargetInitNegotiation  */
         0,                               /* AP_SCSITargetMaxSpeed         */
         0,                               /* AP_SCSITargetDefaultSpeed     */
         0,                               /* AP_SCSITargetMaxOffset        */
         0,                               /* AP_SCSITargetDefaultOffset    */
         0,                               /* AP_SCSITargetMaxWidth         */
         0,                               /* AP_SCSITargetDefaultWidth     */
         1,                               /* AP_SCSITargetAdapterIDMask    */
      }
   },
   HIM_TRUE                               /* AP_OverrideOSMNVRAMRoutines   */
};

/***************************************************************************
*  Default target profile mask setup
***************************************************************************/
HIM_TARGET_PROFILE SCSIDefaultTargetProfile =
{
   6,                               /* TP_Version                    */
   HIM_TRANSPORT_SCSI,              /* TP_Transport                  */
   HIM_PROTOCOL_SCSI,               /* TP_Protocol                   */
   {                                /* TP_WorldWideID                */
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,    
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
   },
   HIM_MAX_WWID_SIZE,               /* TP_WWIDLength                 */
   0,                               /* TP_ScanOrder                  */
   0,                               /* TP_BusNumber                  */
   0,                               /* TP_SortMethod                 */
   2,                               /* TP_MaxActiveCommands          */
   HIM_TRUE,                        /* TP_MaxActiveShared            */
   HIM_FALSE,                       /* TP_TaggedQueuing              */
   HIM_TRUE,                        /* TP_HostManaged                */
   {
      {
         0,                         /* TP_SCSI_ID                    */
         0,                         /* TP_SCSILun                    */
         SCSI_SCAM_ENABLE,          /* TP_SCSIScamSupport            */
         200,                       /* TP_SCSIMaxSpeed               */
         200,                       /* TP_SCSIDefaultSpeed           */
         0,                         /* TP_SCSICurrentSpeed           */
         15,                        /* TP_SCSIMaxOffset              */
         15,                        /* TP_SCSIDefaultOffset          */
         0,                         /* TP_SCSICurrentOffset          */
         16,                        /* TP_SCSIMaxWidth               */
         8,                         /* TP_SCSIDefaultWidth           */
         8,                         /* TP_SCSICurrentWidth           */
         HIM_TRUE,                  /* TP_SCSIDisconnectAllowed      */
         HIM_SCSI_DISABLE_DOMAIN_VALIDATION, /* TP_SCSIDomainValidationMethod */
         HIM_FALSE,                 /* TP_SCSIDomainValidationFallBack */
         HIM_SCSI_ST_CLOCKING,      /* TP_SCSITransitionClocking     */
         HIM_SCSI_NO_PROTOCOL_OPTION,     /* TP_SCSIDefaultProtocolOption */
         HIM_SCSI_PROTOCOL_OPTION_UNKNOWN, /* TP_SCSICurrentProtocolOption */
         HIM_FALSE                  /* TP_SCSIConnectedViaExpander   */
      }
   }
};

/***************************************************************************
*  Adjust target profile mask setup
***************************************************************************/
HIM_TARGET_PROFILE SCSIAdjustableTargetProfile =
{
   0,                               /* TP_Version                    */
   0,                               /* TP_Transport                  */
   0,                               /* TP_Protocol                   */
   {                                /* TP_WorldWideID                */
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,    
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
   },
   0,                               /* TP_WWIDLength                 */
   0,                               /* TP_ScanOrder                  */
   0,                               /* TP_BusNumber                  */
   0,                               /* TP_SortMethod                 */
   1,                               /* TP_MaxActiveCommands          */
   HIM_FALSE,                       /* TP_MaxActiveShared            */
   HIM_TRUE,                        /* TP_TaggedQueuing              */
   HIM_FALSE,                       /* TP_HostManaged                */ 
   {
      {
         0,                         /* TP_SCSI_ID                    */
         0,                         /* TP_SCSILun                    */
         0,                         /* TP_SCSIScamSupport            */
         0,                         /* TP_SCSIMaxSpeed               */
         1,                         /* TP_SCSIDefaultSpeed           */
         0,                         /* TP_SCSICurrentSpeed           */
         0,                         /* TP_SCSIMaxOffset              */
         1,                         /* TP_SCSIDefaultOffset          */
         0,                         /* TP_SCSICurrentOffset          */
         0,                         /* TP_SCSIMaxWidth               */
         1,                         /* TP_SCSIDefaultWidth           */
         0,                         /* TP_SCSICurrentWidth           */
         HIM_TRUE,                  /* TP_SCSIDisconnectAllowed      */
         0,                         /* TP_SCSIDomainValidationMethod */
         HIM_FALSE,                 /* TP_SCSIDomainValidationFallBack */
         0,                         /* TP_SCSITransitionClocking     */
         0,                         /* TP_SCSIDefaultProtocolOption  */
                                    /* set to 1, once this has been  */
                                    /*   implemented.                */
         0,                         /* TP_SCSICurrentProtocolOption  */
         HIM_FALSE                  /* TP_SCSIConnectedViaExpander   */
      }
   }
};

/***************************************************************************
*  Default node profile mask setup
***************************************************************************/
HIM_NODE_PROFILE SCSIDefaultNodeProfile =
{
   1,                               /* NP_Version                    */
   HIM_TRANSPORT_SCSI,              /* NP_Transport                  */
   HIM_PROTOCOL_SCSI,               /* NP_Protocol                   */
   0,                               /* NP_BusNumber                  */
   {
      0,                            /* NP_SCSI_ID                    */
      200,                          /* NP_SCSIMaxSpeed               */
      200,                          /* NP_SCSIDefaultSpeed           */
      0,                            /* NP_SCSICurrentSpeed           */
      15,                           /* NP_SCSIMaxOffset              */
      15,                           /* NP_SCSIDefaultOffset          */
      0,                            /* NP_SCSICurrentOffset          */
      16,                           /* NP_SCSIMaxWidth               */
      8,                            /* NP_SCSIDefaultWidth           */
      8                             /* NP_SCSICurrentWidth           */
   }
};

/***************************************************************************
*  Adjust node profile mask setup
***************************************************************************/
HIM_NODE_PROFILE SCSIAdjustableNodeProfile =
{
   0,                               /* NP_Version                    */
   0,                               /* NP_Transport                  */
   0,                               /* NP_Protocol                   */
   0,                               /* NP_BusNumber                  */
   {
      0,                            /* TP_SCSI_ID                    */
      0,                            /* TP_SCSIMaxSpeed               */
      0,                            /* TP_SCSIDefaultSpeed           */
      0,                            /* TP_SCSICurrentSpeed           */
      0,                            /* TP_SCSIMaxOffset              */
      0,                            /* TP_SCSIDefaultOffset          */
      0,                            /* TP_SCSICurrentOffset          */
      0,                            /* TP_SCSIMaxWidth               */
      0,                            /* TP_SCSIDefaultWidth           */
      0                             /* TP_SCSICurrentWidth           */
   }
};

#if HIM_TASK_SIMPLE == 0 && HIM_TASK_ORDERED == 1 \
      && HIM_TASK_HEAD_OF_QUEUE == 2 && HIM_TASK_RECOVERY == 3
SCSI_UEXACT8 SCSIscontrol[5] =
{
   SCSI_TAGENB | SCSI_SIMPLETAG,
   SCSI_TAGENB | SCSI_ORDERTAG,
   SCSI_TAGENB | SCSI_HEADTAG,
   0,
};
#else
      ****** Inconsistant task attribute translation table ******
#endif

#else /* !defined(SCSI_XLM_DEFINE) */

extern HIM_ADAPTER_PROFILE SCSIDefaultAdapterProfile;
extern HIM_TARGET_PROFILE SCSIDefaultTargetProfile;
extern HIM_ADAPTER_PROFILE SCSIAdjustableAdapterProfile;
extern HIM_TARGET_PROFILE SCSIAdjustableTargetProfile;
extern HIM_NODE_PROFILE SCSIAdjustableNodeProfile;
extern SCSI_UEXACT8 SCSIscontrol[5];

#if (!SCSI_ARO_SUPPORT)
#if SCSI_2930CVAR_SUPPORT   
extern SCSI_HOST_TYPE SCSISubSystemSubVendorType[16];
#else /* ! SCSI_2930CVAR_SUPPORT */
extern SCSI_HOST_TYPE SCSISubSystemSubVendorType[14];
#endif /* SCSI_2930CVAR_SUPPORT */
#else
extern SCSI_HOST_TYPE SCSISubSystemSubVendorType[1];
#endif

#if (!SCSI_ARO_SUPPORT)
#if (!SCSI_2930U2_SUPPORT)
extern SCSI_HOST_TYPE SCSIBayonetSubSystemSubVendorType[2];
#else /* SCSI_2930U2_SUPPORT */
extern SCSI_HOST_TYPE SCSIBayonetSubSystemSubVendorType[1];
#endif /* (! SCSI_2930U2_SUPPORT) */
#else /* SCSI_ARO_SUPPORT */
extern SCSI_HOST_TYPE SCSIBayonetSubSystemSubVendorType[1];
#endif /* (! SCSI_ARO_SUPPORT) */

#if (!SCSI_ARO_SUPPORT)
#if SCSI_OEM1_SUPPORT
extern SCSI_HOST_TYPE SCSITridentSubSystemSubVendorType[1];
#else /* !SCSI_OEM1_SUPPORT */
extern SCSI_HOST_TYPE SCSITridentSubSystemSubVendorType[2];
#endif /* SCSI_OEM1_SUPPORT */
#else /* SCSI_ARO_SUPPORT */
extern SCSI_HOST_TYPE SCSITridentSubSystemSubVendorType[1];
#endif /* (!SCSI_ARO_SUPPORT) */

#endif /* defined(SCSI_XLM_DEFINE) */
