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

/*$Header: /home/CVSROOT/sw/srmconsole/5.8/SRC/AIC78XX_HIM_HWM.H,v 1.2 2001/01/13 05:23:41 chris Exp $*/
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
*  Module Name:   HWM.C
*
*  Description:   Include files for hardware management 
*
*  Owners:        ECX IC Firmware Team
*    
*  Notes:         NONE
*
***************************************************************************/

#if 0
#include "hardequ.h"	/* exclude */
#include "harddef.h"	/* exclude */
#include "firmequ.h"	/* exclude */
#include "firmdef.h"	/* exclude */
#include "scsiref.h"	/* exclude */
#include "scsidef.h"	/* exclude */
#endif

/*$Header: /home/CVSROOT/sw/srmconsole/5.8/SRC/AIC78XX_HIM_HWM.H,v 1.2 2001/01/13 05:23:41 chris Exp $*/
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
*  Module Name:   HWMREF.H
*
*  Description:   Definitions internal to Hardware management layer
*
*  Owners:        ECX IC Firmware Team
*
*  Notes:         1. This file should only be referenced by hardware
*                    management layer.
*
***************************************************************************/

/***************************************************************************
* Macros for general purposes
***************************************************************************/
#define  SCSI_hPAUSE(scsiRegister)  \
            OSD_OUTEXACT8(SCSI_AICREG(SCSI_HCNTRL),\
            OSD_INEXACT8(SCSI_AICREG(SCSI_HCNTRL)) | SCSI_PAUSE)
#define  SCSI_hWAITFORPAUSEACK(scsiRegister) \
            while(!(OSD_INEXACT8(SCSI_AICREG(SCSI_HCNTRL)) & SCSI_PAUSEACK))
#define  SCSI_hSAVEANDPAUSE(scsiRegister,var) \
            (var) = OSD_INEXACT8(SCSI_AICREG(SCSI_HCNTRL)); \
            SCSI_hWAITFORPAUSEACK((scsiRegister))
#define  SCSI_hPAUSEANDWAIT(scsiRegister) \
            SCSI_hPAUSE((scsiRegister));\
            SCSI_hWAITFORPAUSEACK((scsiRegister))
#define  SCSI_hUNPAUSE(scsiRegister) \
            OSD_OUTEXACT8(SCSI_AICREG(SCSI_HCNTRL),\
               OSD_INEXACT8(SCSI_AICREG(SCSI_HCNTRL)) & ~SCSI_PAUSE)
#define  SCSI_hWRITEHCNTRL(scsiRegister,value) \
            OSD_OUTEXACT8(SCSI_AICREG(SCSI_HCNTRL),(value))
#define  SCSI_hREADINTSTAT(scsiRegister) \
            OSD_INEXACT8(SCSI_AICREG(SCSI_INTSTAT))

/***************************************************************************
* Macros for Target Mode Multiple ID support
***************************************************************************/
#if SCSI_MULTIPLEID_SUPPORT
#define  SCSI_hGETDEVICETABLEXFEROPT(deviceTable,i) \
            (deviceTable)->xferOptionHost[(i)]
#else
#define  SCSI_hGETDEVICETABLEXFEROPT(deviceTable,i) \
            (deviceTable)->xferOptionHost
#endif /* SCSI_MULTIPLEID_SUPPORT */             
#if SCSI_MULTI_MODE
/***************************************************************************
* Macros for all modes
***************************************************************************/
#define  SCSI_hSIZEOFSCBBUFFER(mode) SCSIFirmware[(mode)]->ScbSize
#define  SCSI_hALIGNMENTSCBBUFFER(mode) SCSI_hSIZEOFSCBBUFFER((mode))-1
#define  SCSI_hSETUPFIRMWAREDESC(hhcb) (hhcb)->SCSI_HP_firmwareDescriptor = \
                                    SCSIFirmware[(hhcb)->firmwareMode]
#define  SCSI_hSETUPSEQUENCER(hhcb) (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhSetupSequencer((hhcb))
#define  SCSI_hRESETSOFTWARE(hhcb)   (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhResetSoftware((hhcb))
#define  SCSI_hDELIVERSCB(hhcb,hiob)  (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhDeliverScb((hhcb),(hiob))
#define  SCSI_hPATCH_XFER_OPT(hhcb,hiob) \
            if ((hhcb)->SCSI_HP_firmwareDescriptor->SCSIhPatchXferOpt) \
            {\
               (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhPatchXferOpt((hhcb),(hiob));\
            }
#define  SCSI_hENQUEHEADBDR(hhcb,hiob)  (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhEnqueHeadBDR((hhcb),(hiob))
#define  SCSI_hRETRIEVESCB(hhcb)  (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhRetrieveScb((hhcb))
#define  SCSI_hQOUTCNT(hhcb) (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhQoutcnt((hhcb))
#define  SCSI_hTARGETCLEARBUSY(hhcb,hiob) (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhTargetClearBusy((hhcb),(hiob))
#define  SCSI_hSIZEQOUTPTRARRAY(hhcb) (256 * (hhcb)->SCSI_HP_firmwareDescriptor->QoutfifoSize)
#define  SCSI_hREQUESTSENSE(hhcb,hiob) (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhRequestSense((hhcb),(hiob))
#define  SCSI_hRESETBTA(hhcb) (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhResetBTA((hhcb))
#define  SCSI_hGETCONFIGURATION(hhcb) (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhGetConfig((hhcb))
#define  SCSI_hCONFIGURESCBRAM(hhcb) \
            if ((hhcb)->SCSI_HP_firmwareDescriptor->SCSIhConfigureScbRam) \
            {\
               (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhConfigureScbRam(hhcb);\
            }
#define  SCSI_hSETUP_ASSIGN_SCB_BUFFER(hhcb) \
            if ((hhcb)->SCSI_HP_firmwareDescriptor->SCSIhSetupAssignScbBuffer)\
            {\
               (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhSetupAssignScbBuffer((hhcb));\
            }
#define  SCSI_hASSIGN_SCB_BUFFER(hhcb,hiob) \
            if ((hhcb)->SCSI_HP_firmwareDescriptor->SCSIhAssignScbBuffer)\
            {\
               (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhAssignScbBuffer((hhcb),(hiob));\
            }
#define  SCSI_hCHKCONDXFERASSIGN(hhcb,i,xfer_option) \
            (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhChkCondXferAssign((hhcb),(i),(xfer_option))
#define  SCSI_hXFEROPTASSIGN(hhcb,i,xfer_option) \
            (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhXferAssign((hhcb),(i),(xfer_option))

#define  SCSI_hCOMMON_XFER_ASSIGN(hhcb,targetID,value) \
            if ((hhcb)->SCSI_HP_firmwareDescriptor->SCSIhCommonXferAssign)\
            {\
               (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhCommonXferAssign((hhcb),(targetID),(value));\
            }
#define  SCSI_hUPDATE_XFER_OPTION_HOST_MEM(hhcb,hiob,value) \
            if ((hhcb)->SCSI_HP_firmwareDescriptor->SCSIhUpdateXferOptionHostMem)\
            {\
               (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhUpdateXferOptionHostMem((hhcb),(hiob),(value));\
            }

#define  SCSI_hGETXFEROPTION(hhcb,i) \
            (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhGetXfer((hhcb),(i))
#define  SCSI_hLOGFAST20MAP(hhcb,targetID,period) \
            if ((hhcb)->SCSI_HP_firmwareDescriptor->SCSIhLogFast20Map) \
            { \
               (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhLogFast20Map((hhcb),(targetID),(period)); \
            }
#define  SCSI_hGETFAST20REG(hhcb,targetID) \
            (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhGetFast20Reg((hhcb),(targetID))
#define  SCSI_hCLEARFAST20REG(hhcb,targetID) \
            if ((hhcb)->SCSI_HP_firmwareDescriptor->SCSIhClearFast20Reg)\
            {\
               (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhClearFast20Reg((hhcb),(targetID));\
            }
#define  SCSI_hUPDATE_FAST20_HOST_MEM(hhcb,hiob,flag) \
            if ((hhcb)->SCSI_HP_firmwareDescriptor->SCSIhUpdateFast20HostMem)\
            {\
               (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhUpdateFast20HostMem((hhcb),(hiob),(flag));\
            }
#define  SCSI_hUPDATE_FAST20_HW(hhcb,flag) \
            if ((hhcb)->SCSI_HP_firmwareDescriptor->SCSIhUpdateFast20HW)\
            {\
               (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhUpdateFast20HW((hhcb),(flag));\
            }

#define  SCSI_hCLEAR_ALL_FAST20_REG(hhcb) \
            if ((hhcb)->SCSI_HP_firmwareDescriptor->SCSIhClearAllFast20Reg)\
            {\
               (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhClearAllFast20Reg((hhcb));\
            }

#define  SCSI_hMODIFYDATAPTR(hhcb,hiob) \
            (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhModifyDataPtr((hhcb),(hiob))
#define  SCSI_hACTIVEABORT(hhcb,hiob) \
            (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhActiveAbort((hhcb),(hiob))
#define  SCSI_hABORTHIOB(hhcb,hiob,haStatus) \
            (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhAbortHIOB((hhcb),(hiob),(haStatus))
#define  SCSI_hSEARCHSEQDONEQ(hhcb,hiob,postHiob) \
            (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhSearchSeqDoneQ((hhcb),(hiob),(postHiob))
            /*if ((hhcb)->SCSI_HP_firmwareDescriptor->SCSIhSearchSeqDoneQ)\
            {\
               (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhSearchSeqDoneQ((hhcb),(hiob),(postHiob));\
            }*/
#define  SCSI_hSEARCHDONEQ(hhcb,hiob) \
            (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhSearchDoneQ((hhcb),(hiob))
#define  SCSI_hSEARCHNEWQ(hhcb,hiob,haStatus) \
            (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhSearchNewQ((hhcb),(hiob),(haStatus))
#define  SCSI_hSEARCHEXEQ(hhcb,hiob,haStatus,postHiob) \
            (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhSearchExeQ((hhcb),(hiob),(haStatus),(postHiob))

#define  SCSI_hUPDATE_ABORT_BIT_HOST_MEM(hhcb,hiob) \
            (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhUpdateAbortBitHostMem((hhcb),(hiob))
            /*if ((hhcb)->SCSI_HP_firmwareDescriptor->SCSIhUpdateAbortBitHostMem)\
            {\
               (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhUpdateAbortBitHostMem((hhcb),(hiob));\
            }*/

#define  SCSI_hOBTAIN_NEXT_SCB_NUM(hhcb,hiob) \
            (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhObtainNextScbNum((hhcb),(hiob))
            /*if ((hhcb)->SCSI_HP_firmwareDescriptor->SCSIhObtainNextScbNum)\
            {\
               (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhObtainNextScbNum((hhcb),(hiob));\
            }*/

#define  SCSI_hUPDATE_NEXT_SCB_NUM(hhcb,hiob,scbNumber) \
            (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhUpdateNextScbNum((hhcb),(hiob),(scbNumber))
            /*if ((hhcb)->SCSI_HP_firmwareDescriptor->SCSIhUpdateNextScbNum)\
            {\
               (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhUpdateNextScbNum((hhcb),(hiob),(scbNumber));\
            }*/

#define  SCSI_hOBTAIN_NEXT_SCB_ADDRESS(hhcb,hiob)\
            (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhObtainNextScbAddress((hhcb),(hiob))
            /*if ((hhcb)->SCSI_HP_firmwareDescriptor->SCSIhObtainNextScbAddress)\
            {\
               (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhObtainNextScbAddress((hhcb),(hiob));\
            }*/

#define  SCSI_hUPDATE_NEXT_SCB_ADDRESS(hhcb,hiob,ScbBusAddress) \
            if ((hhcb)->SCSI_HP_firmwareDescriptor->SCSIhUpdateNextScbAddress)\
            {\
               (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhUpdateNextScbAddress((hhcb),(hiob),(ScbBusAddress));\
            }

#define  SCSI_hRESETCCCTL(hhcb) \
            if ((hhcb)->SCSI_HP_firmwareDescriptor->SCSIhResetCCCtl)\
            {\
               (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhResetCCCtl((hhcb));\
            }

#define  SCSI_hINITCCHHADDR(hhcb) \
            if ((hhcb)->SCSI_HP_firmwareDescriptor->SCSIhInitCCHHAddr)\
            {\
               (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhInitCCHHAddr((hhcb));\
            }

#define  SCSI_hREMOVEACTIVEABORT(hhcb,hiob) \
            if ((hhcb)->SCSI_HP_firmwareDescriptor->SCSIhRemoveActiveAbort)\
            {\
               (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhRemoveActiveAbort((hhcb),(hiob));\
            }

#define  SCSI_hSTACKBUGFIX(hhcb) \
            if ((hhcb)->SCSI_HP_firmwareDescriptor->SCSIhStackBugFix)\
            {\
               (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhStackBugFix((hhcb));\
            }

#define  SCSI_hSETUPTARGETRESET(hhcb,hiob) \
            (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhSetupBDR((hhcb),(hiob))

#define  SCSI_hRESIDUECALC(hhcb,hiob) \
            (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhResidueCalc((hhcb),(hiob))

#define  SCSI_hSETBREAKPOINT(hhcb,entryBitMap) \
            (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhSetBreakPoint((hhcb),(entryBitMap))

#define  SCSI_hCLEARBREAKPOINT(hhcb,entryBitMap) \
            (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhClearBreakPoint((hhcb),(entryBitMap))

#define  SCSI_hUPDATESGLENGTH(hhcb,regValue) \
            if ((hhcb)->SCSI_HP_firmwareDescriptor->SCSIhUpdateSGLength)\
            {\
               (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhUpdateSGLength((hhcb),(regValue));\
            }

#define  SCSI_hUPDATESGADDRESS(hhcb,regValue) \
            if ((hhcb)->SCSI_HP_firmwareDescriptor->SCSIhUpdateSGAddress)\
            {\
               (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhUpdateSGAddress((hhcb),(regValue));\
            }

#define  SCSI_hFREEZEHWQUEUE(hhcb,hiob) \
            if ((hhcb)->SCSI_HP_firmwareDescriptor->SCSIhFreezeHWQueue)\
            {\
               (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhFreezeHWQueue((hhcb),(hiob));\
            }

#define  SCSI_hSETINTRTHRESHOLD(hhcb,threshold) \
            if ((hhcb)->SCSI_HP_firmwareDescriptor->SCSIhSetIntrThreshold)\
            {\
               (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhSetIntrThreshold((hhcb),(threshold));\
            }

#define  SCSI_hGETINTRTHRESHOLD(hhcb) \
            (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhGetIntrThreshold((hhcb))

#define  SCSI_hENABLEAUTORATEOPTION(hhcb) \
            if ((hhcb)->SCSI_HP_firmwareDescriptor->SCSIhEnableAutoRateOption)\
            {\
               (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhEnableAutoRateOption((hhcb));\
            }

#define  SCSI_hDISABLEAUTORATEOPTION(hhcb,xferOption) \
            if ((hhcb)->SCSI_HP_firmwareDescriptor->SCSIhDisableAutoRateOption)\
            {\
               (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhDisableAutoRateOption((hhcb),(xferOption));\
            }

#define  SCSI_hSETPHASEDELAYVALUES(hhcb) \
            if ((hhcb)->SCSI_HP_firmwareDescriptor->SCSIhSetPhaseDelayValues)\
            {\
               (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhSetPhaseDelayValues((hhcb));\
            }

#if SCSI_TARGET_OPERATION 
#define  SCSI_hTARGETPATCHXFERRATE(hhcb,hiob) \
            if ((hhcb)->SCSI_HP_firmwareDescriptor->SCSIhTargetPatchXferRate)\
            {\
               (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhTargetPatchXferRate((hhcb),(hiob));\
            }
 
#define  SCSI_hTARGETSETIGNOREWIDEMSG(hhcb,hiob) \
            if ((hhcb)->SCSI_HP_firmwareDescriptor->SCSIhTargetSetIgnoreWideMsg)\
            {\
               (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhTargetSetIgnoreWideMsg((hhcb),(hiob));\
            }

#define  SCSI_hTARGET_SEND_HIOB_SPECIAL(hhcb,hiob) \
            if ((hhcb)->SCSI_HP_firmwareDescriptor->SCSIhTargetSendHiobSpecial)\
            {\
               (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhTargetSendHiobSpecial((hhcb),(hiob));\
            }

#define  SCSI_hTARGETGETESTSCBFIELDS(hhcb,hiob,flag) \
            if ((hhcb)->SCSI_HP_firmwareDescriptor->SCSIhTargetGetEstScbFields)\
            {\
               (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhTargetGetEstScbFields((hhcb),(hiob),(flag));\
            }

#define  SCSI_hTARGETDELIVERESTSCB(hhcb,hiob) \
            if ((hhcb)->SCSI_HP_firmwareDescriptor->SCSIhTargetDeliverEstScb)\
            {\
               (hhcb)->SCSI_HP_firmwareDescriptor->SCSIhTargetDeliverEstScb((hhcb),(hiob));\
            }

#endif   /* SCSI_TARGET_OPERATION */

#define  SCSI_hFIRMWARE_VERSION(hhcb)  (hhcb)->SCSI_HP_firmwareDescriptor->firmwareVersion
#define  SCSI_hBTATABLE(hhcb)          (hhcb)->SCSI_HP_firmwareDescriptor->BtaTable
#define  SCSI_hRESCNT_BASE(hhcb)       (hhcb)->SCSI_HP_firmwareDescriptor->RescntBase

#define  SCSI_hDISCON_OPTION(hhcb)   (hhcb)->SCSI_HP_firmwareDescriptor->DisconOption      
#define  SCSI_hSIOSTR3_ENTRY(hhcb)   (hhcb)->SCSI_HP_firmwareDescriptor->Siostr3Entry      
#define  SCSI_hATN_TMR_ENTRY(hhcb)   (hhcb)->SCSI_HP_firmwareDescriptor->AtnTmrEntry
#define  SCSI_hTARG_LUN_MASK0(hhcb)  (hhcb)->SCSI_HP_firmwareDescriptor->TargLunMask0
#define  SCSI_hQ_DONE_ELEMENT(hhcb)  (hhcb)->SCSI_HP_firmwareDescriptor->QDoneElement
#define  SCSI_hPASS_TO_DRIVER(hhcb)  (hhcb)->SCSI_HP_firmwareDescriptor->PassToDriver
#define  SCSI_hSCSIDATL_IMAGE(hhcb)  (hhcb)->SCSI_HP_firmwareDescriptor->ScsiDatlImage
#define  SCSI_hSTART_LINK_CMD_ENTRY(hhcb)  (hhcb)->SCSI_HP_firmwareDescriptor->StartLinkCmdEntry
#define  SCSI_hIDLE_LOOP_ENTRY(hhcb) (hhcb)->SCSI_HP_firmwareDescriptor->IdleLoopEntry
#define  SCSI_hIDLE_LOOP0(hhcb)      (hhcb)->SCSI_HP_firmwareDescriptor->IdleLoop0

#if SCSI_TARGET_OPERATION 
#define SCSI_hTARGET_DATA_ENTRY(hhcb)  (hhcb)->SCSI_HP_firmwareDescriptor->TargetDataEntry
#define SCSI_hTARGET_HELD_BUS_ENTRY(hhcb)  (hhcb)->SCSI_HP_firmwareDescriptor->TargetHeldBusEntry
#endif 
#define  SCSI_hACTIVE_SCB(hhcb)      (hhcb)->SCSI_HP_firmwareDescriptor->ActiveScb
#define  SCSI_hQ_EXE_HEAD(hhcb)      (hhcb)->SCSI_HP_firmwareDescriptor->QExeHead
#define  SCSI_hARRAY_PARTITION0(hhcb)   (hhcb)->SCSI_HP_firmwareDescriptor->ArrayPartition0
#define  SCSI_hQ_NEW_HEAD(hhcb)      (hhcb)->SCSI_HP_firmwareDescriptor->QNewHead
#define  SCSI_hQ_NEW_POINTER(hhcb)      (hhcb)->SCSI_HP_firmwareDescriptor->QNewPointer
#define  SCSI_hQ_NEW_TAIL(hhcb)      (hhcb)->SCSI_HP_firmwareDescriptor->QNewTail
#define  SCSI_hQ_DONE_HEAD(hhcb)     (hhcb)->SCSI_HP_firmwareDescriptor->QDoneHead
#define  SCSI_hQ_NEXT_SCB(hhcb)         (hhcb)->SCSI_HP_firmwareDescriptor->QNextScb
#define  SCSI_hWAITING_SCB(hhcb)     (hhcb)->SCSI_HP_firmwareDescriptor->WaitingScb
#define  SCSI_hQ_DONE_BASE(hhcb)     (hhcb)->SCSI_HP_firmwareDescriptor->QDoneBase
#define  SCSI_hSEQUENCER_PROGRAM(hhcb)     (hhcb)->SCSI_HP_firmwareDescriptor->SequencerProgram
#define  SCSI_hQ_DONE_PASS(hhcb)     (hhcb)->SCSI_HP_firmwareDescriptor->QDonePass
#define  SCSI_hXFER_OPTION(hhcb)     (hhcb)->SCSI_HP_firmwareDescriptor->XferOption     
#define  SCSI_hSIO204_ENTRY(hhcb)    (hhcb)->SCSI_HP_firmwareDescriptor->Sio204Entry
#define  SCSI_hEXPANDER_BREAK(hhcb)  (hhcb)->SCSI_HP_firmwareDescriptor->ExpanderBreak
#define  SCSI_hSIO215(hhcb)          (hhcb)->SCSI_HP_firmwareDescriptor->Sio215
#define  SCSI_hFAST20_LOW(hhcb)      (hhcb)->SCSI_HP_firmwareDescriptor->Fast20Low
#define  SCSI_hFAST20_HIGH(hhcb)     (hhcb)->SCSI_HP_firmwareDescriptor->Fast20High
#define  SCSI_hMWI_CACHE_MASK(hhcb)  (hhcb)->SCSI_HP_firmwareDescriptor->MwiCacheMask
#define  SCSI_hENT_PT_BITMAP(hhcb)   (hhcb)->SCSI_HP_firmwareDescriptor->EntPtBitmap
#define  SCSI_hAUTO_RATE_ENABLE(hhcb)(hhcb)->SCSI_HP_firmwareDescriptor->AutoRateEnable
#if SCSI_TARGET_OPERATION
#define  SCSI_hQ_EST_HEAD(hhcb)      (hhcb)->SCSI_HP_firmwareDescriptor->QEstHead
#define  SCSI_hQ_EST_TAIL(hhcb)      (hhcb)->SCSI_HP_firmwareDescriptor->QEstTail
#define  SCSI_hQ_EST_NEXT_SCB(hhcb)  (hhcb)->SCSI_HP_firmwareDescriptor->QEstNextScb
#endif /* SCSI_TARGET_OPERATION */
#else
#if SCSI_STANDARD64_MODE
/***************************************************************************
* Macros for standard 64 mode exclusively
***************************************************************************/
#define  SCSI_hSIZEOFSCBBUFFER(mode) SCSI_S64_SIZE_SCB
#define  SCSI_hALIGNMENTSCBBUFFER(mode) SCSI_hSIZEOFSCBBUFFER((mode))-1
#define  SCSI_hSETUPFIRMWAREDESC(hhcb)
#define  SCSI_hSETUPSEQUENCER(hhcb) SCSIhStandard64SetupSequencer((hhcb))
#define  SCSI_hRESETSOFTWARE(hhcb)   SCSIhStandard64ResetSoftware((hhcb))
#define  SCSI_hDELIVERSCB(hhcb,hiob)  SCSIhDeliverScb((hhcb),(hiob))
#define  SCSI_hPATCH_XFER_OPT(hhcb,hiob)
#define  SCSI_hENQUEHEADBDR(hhcb,hiob)  SCSIhStandard64QHeadBDR((hhcb),(hiob))
#define  SCSI_hRETRIEVESCB(hhcb)  SCSIhStandardRetrieveScb(hhcb)
#define  SCSI_hQOUTCNT(hhcb) SCSIhStandardQoutcnt(hhcb)
#define  SCSI_hTARGETCLEARBUSY(hhcb,hiob) SCSIhStandardTargetClearBusy((hhcb),(hiob))
#define  SCSI_hSIZEQOUTPTRARRAY(hhcb) (256 * sizeof(QOUTFIFO_NEW))
#define  SCSI_hREQUESTSENSE(hhcb,hiob) SCSIhStandard64RequestSense((hhcb),(hiob))
#define  SCSI_hRESETBTA(hhcb) SCSIhStandardResetBTA((hhcb))
#define  SCSI_hGETCONFIGURATION(hhcb) SCSIhStandardGetConfig((hhcb))
#define  SCSI_hCONFIGURESCBRAM(hhcb) SCSIhStandardConfigureScbRam((hhcb))
#if SCSI_SCBBFR_BUILTIN
#define  SCSI_hSETUP_ASSIGN_SCB_BUFFER(hhcb) SCSIhSetupAssignScbBuffer((hhcb))
#define  SCSI_hASSIGN_SCB_BUFFER(hhcb,hiob)
#else
#define  SCSI_hSETUP_ASSIGN_SCB_BUFFER(hhcb)
#define  SCSI_hASSIGN_SCB_BUFFER(hhcb,hiob)
#endif
#define  SCSI_hCHKCONDXFERASSIGN(hhcb,i,xfer_option) \
            SCSIhStandardXferOptAssign(hhcb,i,xfer_option)
#define  SCSI_hXFEROPTASSIGN(hhcb,i,xfer_option) \
            SCSIhStandardXferOptAssign(hhcb,i,xfer_option)

#define  SCSI_hCOMMON_XFER_ASSIGN(hhcb,targetID,value)
#define  SCSI_hUPDATE_XFER_OPTION_HOST_MEM(hhcb,hiob,value)

#define  SCSI_hGETXFEROPTION(hhcb,i) SCSIhStandardGetOption(hhcb,i)
#define  SCSI_hGETFAST20REG(hhcb,targetID) SCSIhStandardGetFast20Reg(hhcb,targetID)
#define  SCSI_hCLEARFAST20REG(hhcb,targetID) SCSIhStandardClearFast20Reg(hhcb,targetID)
#define  SCSI_hLOGFAST20MAP(hhcb,targetID,period) \
            SCSIhStandardLogFast20Map(hhcb,targetID,period)
#define  SCSI_hUPDATE_FAST20_HOST_MEM(hhcb,hiob,flag)
#define  SCSI_hUPDATE_FAST20_HW(hhcb,flag)
#define  SCSI_hCLEAR_ALL_FAST20_REG(hhcb) SCSIhStandard64ClearAllFast20Reg(hhcb)
#define  SCSI_hMODIFYDATAPTR(hhcb,hiob) SCSIhStandard64ModifyDataPtr(hhcb,hiob)
#define  SCSI_hACTIVEABORT(hhcb,hiob) \
            SCSIhStandard64ActiveAbort(hhcb,hiob)
#define  SCSI_hABORTHIOB(hhcb,hiob,haStatus) \
            SCSIhStandard64AbortHIOB(hhcb,hiob,haStatus)
#define  SCSI_hSEARCHSEQDONEQ(hhcb,hiob,postHiob) \
            SCSIhStandardSearchSeqDoneQ(hhcb,hiob,postHiob)
#define  SCSI_hSEARCHDONEQ(hhcb,hiob) \
            SCSIhStandardSearchDoneQ(hhcb,hiob)
#define  SCSI_hSEARCHNEWQ(hhcb,hiob,haStatus) \
            SCSIhStandardSearchNewQ(hhcb,hiob,haStatus)
#define  SCSI_hSEARCHEXEQ(hhcb,hiob,haStatus,postHiob) \
            SCSIhStandard64SearchExeQ(hhcb,hiob,haStatus,postHiob)

#define  SCSI_hUPDATE_ABORT_BIT_HOST_MEM(hhcb,hiob) \
            SCSIhStandard64UpdateAbortBitHostMem(hhcb,hiob)

#define  SCSI_hOBTAIN_NEXT_SCB_NUM(hhcb,hiob) \
           SCSIhStandard64ObtainNextScbNum(hhcb,hiob)
#define  SCSI_hUPDATE_NEXT_SCB_NUM(hhcb,hiob,scbNumber) \
           SCSIhStandard64UpdateNextScbNum(hhcb,hiob,scbNumber)
#define  SCSI_hOBTAIN_NEXT_SCB_ADDRESS(hhcb,hiob) \
           SCSIhStandard64ObtainNextScbAddress(hhcb,hiob)
#define  SCSI_hUPDATE_NEXT_SCB_ADDRESS(hhcb,hiob,ScbBusAddress) \
           SCSIhStandard64UpdateNextScbAddress(hhcb,hiob,ScbBusAddress)

#define  SCSI_hRESETCCCTL(hhcb) SCSIhStandardResetCCCtl(hhcb)
#define  SCSI_hINITCCHHADDR(hhcb) SCSIhStandardInitCCHHAddr(hhcb)
#define  SCSI_hREMOVEACTIVEABORT(hhcb,hiob) \
            SCSIhStandardRemoveActiveAbort(hhcb,hiob)

#define  SCSI_hSTACKBUGFIX(hhcb) SCSIhStandard64StackBugFix(hhcb)

#define  SCSI_hSETUPTARGETRESET(hhcb,hiob) SCSIhStandard64SetupBDR(hhcb,hiob)
#define  SCSI_hRESIDUECALC(hhcb,hiob) SCSIhStandard64ResidueCalc(hhcb,hiob)

#define  SCSI_hSETBREAKPOINT(hhcb,entryBitMap) \
            SCSIhStandard64SetBreakPoint(hhcb,entryBitMap)
#define  SCSI_hCLEARBREAKPOINT(hhcb,entryBitMap) \
            SCSIhStandard64ClearBreakPoint(hhcb,entryBitMap)

#define  SCSI_hUPDATESGLENGTH(hhcb,regValue)
#define  SCSI_hUPDATESGADDRESS(hhcb,regValue)
#define  SCSI_hFREEZEHWQUEUE(hhcb,hiob)

#define  SCSI_hSETINTRTHRESHOLD(hhcb,threshold)
#define  SCSI_hGETINTRTHRESHOLD(hhcb) SCSIhStandard64GetIntrThreshold(hhcb)

#define  SCSI_hENABLEAUTORATEOPTION(hhcb)
#define  SCSI_hDISABLEAUTORATEOPTION(hhcb,xferOption)

#define  SCSI_hSETPHASEDELAYVALUES(hhcb)

#if SCSI_TARGET_OPERATION 
#define  SCSI_hTARGETPATCHXFERRATE(hhcb,hiob)
#define  SCSI_hTARGETSETIGNOREWIDEMSG(hhcb,hiob) 
#define  SCSI_hTARGET_SEND_HIOB_SPECIAL(hhcb,hiob)
#define  SCSI_hTARGETGETESTSCBFIELDS(hhcb,hiob,flag)
#define  SCSI_hTARGETDELIVERESTSCB(hhcb,hiob)
#endif   /* SCSI_TARGET_OPERATION */

#define  SCSI_hFIRMWARE_VERSION(hhcb)        SCSI_S64_FIRMWARE_VERSION
#define  SCSI_hBTATABLE(hhcb)                SCSI_S64_BTATABLE
#define  SCSI_hRESCNT_BASE(hhcb)             SCSI_S64_RESCNT_BASE

#define  SCSI_hDISCON_OPTION(hhcb)           SCSI_S64_DISCON_OPTION
#define  SCSI_hSIOSTR3_ENTRY(hhcb)           SCSI_S64_SIOSTR3_ENTRY
#define  SCSI_hATN_TMR_ENTRY(hhcb)           SCSI_S64_ATN_TMR_ENTRY
#define  SCSI_hTARG_LUN_MASK0(hhcb)          SCSI_S64_TARG_LUN_MASK0
#define  SCSI_hQ_DONE_ELEMENT(hhcb)          SCSI_S64_Q_DONE_ELEMENT
#define  SCSI_hPASS_TO_DRIVER(hhcb)          SCSI_S64_PASS_TO_DRIVER
#define  SCSI_hSCSIDATL_IMAGE(hhcb)          SCSI_S64_PASS_TO_DRIVER
#define  SCSI_hSTART_LINK_CMD_ENTRY(hhcb)    SCSI_S64_START_LINK_CMD_ENTRY
#define  SCSI_hIDLE_LOOP_ENTRY(hhcb)         SCSI_S64_IDLE_LOOP_ENTRY
#define  SCSI_hIDLE_LOOP0(hhcb)              SCSI_S64_IDLE_LOOP0
#if SCSI_TARGET_OPERATION 
#define  SCSI_hTARGET_DATA_ENTRY(hhcb)       SCSI_S64_TARGET_DATA_ENTRY
#define  SCSI_hTARGET_HELD_BUS_ENTRY(hhcb)   SCSI_S64_TARGET_HELD_BUS_ENTRY
#endif /* SCSI_TARGET_OPERATION */
#define  SCSI_hACTIVE_SCB(hhcb)              SCSI_S64_ACTIVE_SCB
#define  SCSI_hQ_EXE_HEAD(hhcb)              SCSI_S64_Q_EXE_HEAD
#define  SCSI_hARRAY_PARTITION0(hhcb)        SCSI_S64_ARRAY_PARTITION0
#define  SCSI_hQ_NEW_HEAD(hhcb)              SCSI_S64_Q_NEW_HEAD
#define  SCSI_hQ_NEW_POINTER(hhcb)           SCSI_S64_Q_NEW_POINTER
#define  SCSI_hQ_NEW_TAIL(hhcb)              SCSI_S64_Q_NEW_TAIL
#define  SCSI_hQ_NEXT_SCB(hhcb)              SCSI_S64_Q_NEXT_SCB 
#define  SCSI_hQ_DONE_HEAD(hhcb)             SCSI_S64_Q_DONE_HEAD
#define  SCSI_hWAITING_SCB(hhcb)             SCSI_S64_WAITING_SCB
#define  SCSI_hQ_DONE_BASE(hhcb)             SCSI_S64_Q_DONE_BASE
#define  SCSI_hSEQUENCER_PROGRAM(hhcb)       SCSI_S64_SEQUENCER_PROGRAM
#define  SCSI_hQ_DONE_PASS(hhcb)             SCSI_S64_Q_DONE_PASS
#define  SCSI_hXFER_OPTION(hhcb)             SCSI_S64_XFER_OPTION
#define  SCSI_hSIO204_ENTRY(hhcb)            SCSI_S64_SIO204_ENTRY
#define  SCSI_hSIO215(hhcb)                  SCSI_S64_SIO215
#define  SCSI_hFAST20_LOW(hhcb)              SCSI_S64_FAST20_LOW
#define  SCSI_hFAST20_HIGH(hhcb)             SCSI_S64_FAST20_HIGH
#define  SCSI_hMWI_CACHE_MASK(hhcb)          SCSI_S64_MWI_CACHE_MASK
#define  SCSI_hENT_PT_BITMAP(hhcb)           SCSI_S64_ENT_PT_BITMAP
#define  SCSI_hAUTO_RATE_ENABLE(hhcb)        SCSI_S64_AUTO_RATE_ENABLE
#define  SCSI_hEXPANDER_BREAK(hhcb)          SCSI_S64_EXPANDER_BREAK
#if SCSI_TARGET_OPERATION
#define  SCSI_hQ_EST_HEAD(hhcb)              SCSI_S64_Q_EST_HEAD
#define  SCSI_hQ_EST_TAIL(hhcb)              SCSI_S64_Q_EST_TAIL
#define  SCSI_hQ_EST_NEXT_SCB(hhcb)          SCSI_S64_Q_EST_NEXT_SCB
#endif /* SCSI_TARGET_OPERATION */
#endif /* SCSI_STANDARD64_MODE */

#if SCSI_STANDARD32_MODE
/***************************************************************************
* Macros for standard 32 mode exclusively
***************************************************************************/
#endif


#if SCSI_SWAPPING64_MODE
/***************************************************************************
* Macros for swapping 64 mode exclusively
***************************************************************************/
#define  SCSI_hSIZEOFSCBBUFFER(mode)  SCSI_W64_SIZE_SCB
#define  SCSI_hALIGNMENTSCBBUFFER(mode) SCSI_hSIZEOFSCBBUFFER((mode))-1
#define  SCSI_hSETUPFIRMWAREDESC(hhcb)
#define  SCSI_hSETUPSEQUENCER(hhcb) SCSIhSwapping64SetupSequencer((hhcb))
#define  SCSI_hRESETSOFTWARE(hhcb)   SCSIhSwappingResetSoftware((hhcb))
#define  SCSI_hDELIVERSCB(hhcb,hiob)  SCSIhDeliverScb((hhcb),(hiob))
#define  SCSI_hPATCH_XFER_OPT(hhcb,hiob) SCSIhSwapping64PatchXferOpt((hhcb),(hiob))
#define  SCSI_hENQUEHEADBDR(hhcb,hiob)  SCSIhSwapping64QHeadBDR((hhcb),(hiob))
#define  SCSI_hRETRIEVESCB(hhcb)  SCSIhStandardRetrieveScb(hhcb)
#define  SCSI_hQOUTCNT(hhcb) SCSIhStandardQoutcnt(hhcb)
#define  SCSI_hTARGETCLEARBUSY(hhcb,hiob) SCSIhSwappingTargetClearBusy((hhcb),(hiob))
#define  SCSI_hSIZEQOUTPTRARRAY(hhcb) (256 * sizeof(QOUTFIFO_NEW))
#define  SCSI_hREQUESTSENSE(hhcb,hiob) SCSIhSwapping64RequestSense((hhcb),(hiob))
#define  SCSI_hRESETBTA(hhcb) SCSIhSwappingResetBTA(hhcb)
#define  SCSI_hGETCONFIGURATION(hhcb) SCSIhNonStandardGetConfig((hhcb))
#define  SCSI_hCONFIGURESCBRAM(hhcb)
#define  SCSI_hSETUP_ASSIGN_SCB_BUFFER(hhcb) SCSIhSetupAssignScbBuffer((hhcb))
#define  SCSI_hASSIGN_SCB_BUFFER(hhcb,hiob)
#define  SCSI_hCHKCONDXFERASSIGN(hhcb,i,xfer_option) \
            SCSIhChkCondXferAssign(hhcb,i,xfer_option)
#define  SCSI_hXFEROPTASSIGN(hhcb,i,xfer_option) \
            SCSIhHostXferOptAssign(hhcb,i,xfer_option)
#define  SCSI_hCOMMON_XFER_ASSIGN(hhcb,targetID,value) \
            SCSIhSwapping64CommonXferAssign(hhcb,targetID,value)
#define  SCSI_hUPDATE_XFER_OPTION_HOST_MEM(hhcb,hiob,value) \
            SCSIhSwapping64UpdateXferOptionHostMem(hhcb,hiob,value)
#define  SCSI_hGETXFEROPTION(hhcb,i) SCSIhHostGetOption(hhcb,i)
#define  SCSI_hGETFAST20REG(hhcb,targetID) SCSIhSwappingGetFast20Reg(hhcb,targetID)
#define  SCSI_hCLEARFAST20REG(hhcb,targetID) SCSIhSwappingClearFast20Reg(hhcb,targetID)
#define  SCSI_hLOGFAST20MAP(hhcb,targetID,period) \
            SCSIhSwappingLogFast20Map((hhcb),(targetID),(period))
#define  SCSI_hUPDATE_FAST20_HOST_MEM(hhcb,hiob,flag) \
            SCSIhSwapping64UpdateFast20HostMem(hhcb,hiob,flag)
#define  SCSI_hUPDATE_FAST20_HW(hhcb,flag) \
            SCSIhSwapping64UpdateFast20HW(hhcb,flag)
#define  SCSI_hCLEAR_ALL_FAST20_REG(hhcb) \
            SCSIhSwappingClearAllFast20Reg(hhcb)
#define  SCSI_hMODIFYDATAPTR(hhcb,hiob) SCSIhSwapping64ModifyDataPtr(hhcb,hiob)
#define  SCSI_hACTIVEABORT(hhcb,hiob) \
            SCSIhSwapping64ActiveAbort(hhcb,hiob)
#define  SCSI_hABORTHIOB(hhcb,hiob,haStatus) \
            SCSIhSwappingAbortHIOB(hhcb,hiob,haStatus)
#define  SCSI_hSEARCHSEQDONEQ(hhcb,hiob,postHiob)
#define  SCSI_hSEARCHDONEQ(hhcb,hiob) \
            SCSIhStandardSearchDoneQ(hhcb,hiob)
#define  SCSI_hSEARCHNEWQ(hhcb,hiob,haStatus) \
            SCSIhSwappingSearchNewQ(hhcb,hiob,haStatus)
#define  SCSI_hSEARCHEXEQ(hhcb,hiob,haStatus,postHiob)\
            SCSIhSwappingSearchExeQ(hhcb,hiob,haStatus,postHiob)

#define  SCSI_hUPDATE_ABORT_BIT_HOST_MEM(hhcb,hiob) \
            SCSIhSwapping64UpdateAbortBitHostMem(hhcb,hiob)

#define  SCSI_hOBTAIN_NEXT_SCB_NUM(hhcb,hiob) \
           SCSIhSwapping64ObtainNextScbNum(hhcb,hiob)
#define  SCSI_hUPDATE_NEXT_SCB_NUM(hhcb,hiob,scbNumber) \
           SCSIhSwapping64UpdateNextScbNum(hhcb,hiob,scbNumber)
#define  SCSI_hOBTAIN_NEXT_SCB_ADDRESS(hhcb,hiob)
#define  SCSI_hUPDATE_NEXT_SCB_ADDRESS(hhcb,hiob,ScbBusAddress)

#define  SCSI_hRESETCCCTL(hhcb)
#define  SCSI_hINITCCHHADDR(hhcb)
#define  SCSI_hREMOVEACTIVEABORT(hhcb,hiob) \
            SCSIhSwappingRemoveActiveAbort(hhcb,hiob)

#define  SCSI_hSTACKBUGFIX(hhcb)

#define  SCSI_hSETUPTARGETRESET(hhcb,hiob) SCSIhSwapping64SetupBDR(hhcb,hiob)
#define  SCSI_hRESIDUECALC(hhcb,hiob) SCSIhSwapping64ResidueCalc(hhcb,hiob)
#define  SCSI_hSETBREAKPOINT(hhcb,entryBitMap) \
            SCSIhSwapping64SetBreakPoint(hhcb,entryBitMap)
#define  SCSI_hCLEARBREAKPOINT(hhcb,entryBitMap) \
            SCSIhSwapping64ClearBreakPoint(hhcb,entryBitMap)

#define  SCSI_hUPDATESGLENGTH(hhcb,regValue)
#define  SCSI_hUPDATESGADDRESS(hhcb,regValue)
#define  SCSI_hFREEZEHWQUEUE(hhcb,hiob)

#define  SCSI_hSETINTRTHRESHOLD(hhcb,threshold)
#define  SCSI_hGETINTRTHRESHOLD(hhcb) SCSIhSwappingGetIntrThreshold(hhcb)

#define  SCSI_hENABLEAUTORATEOPTION(hhcb)
#define  SCSI_hDISABLEAUTORATEOPTION(hhcb,xferOption)

#define  SCSI_hSETPHASEDELAYVALUES(hhcb)

#if SCSI_TARGET_OPERATION 
#define  SCSI_hTARGETPATCHXFERRATE(hhcb,hiob)
#define  SCSI_hTARGETSETIGNOREWIDEMSG(hhcb,hiob) 
#define  SCSI_hTARGET_SEND_HIOB_SPECIAL(hhcb,hiob)
#define  SCSI_hTARGETGETESTSCBFIELDS(hhcb,hiob,flag)
#define  SCSI_hTARGETDELIVERESTSCB(hhcb,hiob)
#endif /* SCSI_TARGET_OPERATION */

#define  SCSI_hFIRMWARE_VERSION(hhcb)        SCSI_W64_FIRMWARE_VERSION
#define  SCSI_hBTATABLE(hhcb)                SCSI_W64_BTATABLE
#define  SCSI_hRESCNT_BASE(hhcb)             SCSI_W64_RESCNT_BASE

#define  SCSI_hDISCON_OPTION(hhcb)           SCSI_W64_DISCON_OPTION
#define  SCSI_hSIOSTR3_ENTRY(hhcb)           SCSI_W64_SIOSTR3_ENTRY
#define  SCSI_hATN_TMR_ENTRY(hhcb)           SCSI_W64_ATN_TMR_ENTRY
#define  SCSI_hTARG_LUN_MASK0(hhcb)          SCSI_W64_TARG_LUN_MASK0
#define  SCSI_hQ_DONE_ELEMENT(hhcb)          SCSI_W64_Q_DONE_ELEMENT
#define  SCSI_hPASS_TO_DRIVER(hhcb)          SCSI_W64_PASS_TO_DRIVER
#define  SCSI_hSCSIDATL_IMAGE(hhcb)          SCSI_W64_PASS_TO_DRIVER
#define  SCSI_hSTART_LINK_CMD_ENTRY(hhcb)    SCSI_W64_START_LINK_CMD_ENTRY
#define  SCSI_hIDLE_LOOP_ENTRY(hhcb)         SCSI_W64_IDLE_LOOP_ENTRY
#define  SCSI_hIDLE_LOOP0(hhcb)              SCSI_W64_IDLE_LOOP0
#if SCSI_TARGET_OPERATION 
#define  SCSI_hTARGET_DATA_ENTRY(hhcb)       SCSI_W64_TARGET_DATA_ENTRY
#define  SCSI_hTARGET_HELD_BUS_ENTRY(hhcb)   SCSI_W64_TARGET_HELD_BUS_ENTRY
#endif /* SCSI_TARGET_OPERATION */
#define  SCSI_hACTIVE_SCB(hhcb)              SCSI_W64_ACTIVE_SCB
#define  SCSI_hQ_EXE_HEAD(hhcb)              SCSI_W64_Q_EXE_HEAD
#define  SCSI_hARRAY_PARTITION0(hhcb)        SCSI_W64_ARRAY_PARTITION0
#define  SCSI_hQ_NEW_HEAD(hhcb)              SCSI_W64_Q_NEW_HEAD
#define  SCSI_hQ_NEW_POINTER(hhcb)           SCSI_W64_Q_NEW_BASE
#define  SCSI_hQ_NEW_TAIL(hhcb)              SCSI_W64_Q_NEW_TAIL
#define  SCSI_hQ_NEXT_SCB(hhcb)              SCSI_W64_Q_NEXT_SCB 
#define  SCSI_hQ_DONE_HEAD(hhcb)             SCSI_W64_Q_DONE_HEAD
#define  SCSI_hWAITING_SCB(hhcb)             SCSI_W64_WAITING_SCB
#define  SCSI_hQ_DONE_BASE(hhcb)             SCSI_W64_Q_DONE_BASE
#define  SCSI_hSEQUENCER_PROGRAM(hhcb)       SCSI_W64_SEQUENCER_PROGRAM
#define  SCSI_hQ_DONE_PASS(hhcb)             SCSI_W64_Q_DONE_PASS
#define  SCSI_hXFER_OPTION(hhcb)             SCSI_W64_XFER_OPTION
#define  SCSI_hSIO204_ENTRY(hhcb)            SCSI_W64_SIO204_ENTRY
#define  SCSI_hSIO215(hhcb)                  SCSI_W64_SIO215
#define  SCSI_hFAST20_LOW(hhcb)              SCSI_W64_FAST20_LOW
#define  SCSI_hFAST20_HIGH(hhcb)             SCSI_W64_FAST20_HIGH
#define  SCSI_hMWI_CACHE_MASK(hhcb)          SCSI_W64_MWI_CACHE_MASK
#define  SCSI_hENT_PT_BITMAP(hhcb)           SCSI_W64_ENT_PT_BITMAP
#define  SCSI_hAUTO_RATE_ENABLE(hhcb)        SCSI_W64_AUTO_RATE_ENABLE
#define  SCSI_hEXPANDER_BREAK(hhcb)          SCSI_W64_EXPANDER_BREAK
#if SCSI_TARGET_OPERATION
#define  SCSI_hQ_EST_HEAD(hhcb)              SCSI_W64_Q_EST_HEAD
#define  SCSI_hQ_EST_TAIL(hhcb)              SCSI_W64_Q_EST_TAIL
#define  SCSI_hQ_EST_NEXT_SCB(hhcb)          SCSI_W64_Q_EST_NEXT_SCB
#endif /* SCSI_TARGET_OPERATION */
#endif /* SCSI_SWAPPING64_MODE */


#if SCSI_SWAPPING32_MODE
/***************************************************************************
* Macros for swapping 32 mode exclusively
***************************************************************************/
#define  SCSI_hSIZEOFSCBBUFFER(mode)  SCSI_W32_SIZE_SCB
#define  SCSI_hALIGNMENTSCBBUFFER(mode) SCSI_hSIZEOFSCBBUFFER((mode))-1
#define  SCSI_hSETUPFIRMWAREDESC(hhcb)
#define  SCSI_hSETUPSEQUENCER(hhcb) SCSIhSwapping32SetupSequencer((hhcb))
#define  SCSI_hRESETSOFTWARE(hhcb)   SCSIhSwappingResetSoftware((hhcb)) 
#define  SCSI_hDELIVERSCB(hhcb,hiob)  SCSIhDeliverScb((hhcb),(hiob))
#define  SCSI_hPATCH_XFER_OPT(hhcb,hiob) SCSIhSwapping32PatchXferOpt((hhcb),(hiob))
#define  SCSI_hENQUEHEADBDR(hhcb,hiob)  SCSIhSwapping32QHeadBDR((hhcb),(hiob)) 
#define  SCSI_hRETRIEVESCB(hhcb)  SCSIhStandardRetrieveScb(hhcb)
#define  SCSI_hQOUTCNT(hhcb) SCSIhStandardQoutcnt(hhcb)
#define  SCSI_hTARGETCLEARBUSY(hhcb,hiob) SCSIhSwappingTargetClearBusy((hhcb),(hiob)) 
#define  SCSI_hSIZEQOUTPTRARRAY(hhcb) (256 * sizeof(QOUTFIFO_NEW))
#define  SCSI_hREQUESTSENSE(hhcb,hiob) SCSIhSwapping32RequestSense((hhcb),(hiob))
#define  SCSI_hRESETBTA(hhcb) SCSIhSwappingResetBTA(hhcb) 
#define  SCSI_hGETCONFIGURATION(hhcb) SCSIhNonStandardGetConfig((hhcb))
#define  SCSI_hCONFIGURESCBRAM(hhcb)
#define  SCSI_hSETUP_ASSIGN_SCB_BUFFER(hhcb) SCSIhSetupAssignScbBuffer((hhcb))
#define  SCSI_hASSIGN_SCB_BUFFER(hhcb,hiob)
#define  SCSI_hCHKCONDXFERASSIGN(hhcb,i,xfer_option) \
            SCSIhChkCondXferAssign(hhcb,i,xfer_option) 
#define  SCSI_hXFEROPTASSIGN(hhcb,i,xfer_option) \
            SCSIhHostXferOptAssign(hhcb,i,xfer_option)

#define  SCSI_hCOMMON_XFER_ASSIGN(hhcb,targetID,value) \
            SCSIhSwapping32CommonXferAssign(hhcb,targetID,value)
#define  SCSI_hUPDATE_XFER_OPTION_HOST_MEM(hhcb,hiob,value) \
            SCSIhSwapping32UpdateXferOptionHostMem(hhcb,hiob,value)

#define  SCSI_hGETXFEROPTION(hhcb,i) SCSIhHostGetOption(hhcb,i)
#define  SCSI_hGETFAST20REG(hhcb,targetID) SCSIhSwappingGetFast20Reg(hhcb,targetID) 
#define  SCSI_hCLEARFAST20REG(hhcb,targetID) SCSIhSwappingClearFast20Reg(hhcb,targetID) 
#define  SCSI_hLOGFAST20MAP(hhcb,targetID,period) \
            SCSIhSwappingLogFast20Map((hhcb),(targetID),(period)) 

#define  SCSI_hUPDATE_FAST20_HOST_MEM(hhcb,hiob,flag) \
            SCSIhSwapping32UpdateFast20HostMem(hhcb,hiob,flag)
#define  SCSI_hUPDATE_FAST20_HW(hhcb,flag) \
            SCSIhSwapping32UpdateFast20HW(hhcb,flag)
#define  SCSI_hCLEAR_ALL_FAST20_REG(hhcb) \
            SCSIhSwappingClearAllFast20Reg(hhcb)

#define  SCSI_hMODIFYDATAPTR(hhcb,hiob) SCSIhSwapping32ModifyDataPtr(hhcb,hiob)
#define  SCSI_hACTIVEABORT(hhcb,hiob) \
            SCSIhSwapping32ActiveAbort(hhcb,hiob)
#define  SCSI_hABORTHIOB(hhcb,hiob,haStatus) \
            SCSIhSwappingAbortHIOB(hhcb,hiob,haStatus) 
#define  SCSI_hSEARCHSEQDONEQ(hhcb,hiob,postHiob)
#define  SCSI_hSEARCHDONEQ(hhcb,hiob) \
            SCSIhStandardSearchDoneQ(hhcb,hiob)
#define  SCSI_hSEARCHNEWQ(hhcb,hiob,haStatus) \
            SCSIhSwappingSearchNewQ(hhcb,hiob,haStatus) 
#define  SCSI_hSEARCHEXEQ(hhcb,hiob,haStatus,postHiob) \
            SCSIhSwappingSearchExeQ(hhcb,hiob,haStatus,postHiob)

#define  SCSI_hUPDATE_ABORT_BIT_HOST_MEM(hhcb,hiob) \
            SCSIhSwapping32UpdateAbortBitHostMem(hhcb,hiob)

#define  SCSI_hOBTAIN_NEXT_SCB_NUM(hhcb,hiob) \
           SCSIhSwapping32ObtainNextScbNum(hhcb,hiob)
#define  SCSI_hUPDATE_NEXT_SCB_NUM(hhcb,hiob,scbNumber) \
            SCSIhSwapping32UpdateNextScbNum(hhcb,hiob,scbNumber)
#define  SCSI_hOBTAIN_NEXT_SCB_ADDRESS(hhcb,hiob)
#define  SCSI_hUPDATE_NEXT_SCB_ADDRESS(hhcb,hiob,ScbBusAddress)

#define  SCSI_hRESETCCCTL(hhcb)
#define  SCSI_hINITCCHHADDR(hhcb)
#define  SCSI_hREMOVEACTIVEABORT(hhcb,hiob) \
            SCSIhSwappingRemoveActiveAbort(hhcb,hiob)

#define  SCSI_hSTACKBUGFIX(hhcb)

#define  SCSI_hSETUPTARGETRESET(hhcb,hiob) SCSIhSwapping32SetupBDR(hhcb,hiob)
#define  SCSI_hRESIDUECALC(hhcb,hiob) SCSIhSwapping32ResidueCalc(hhcb,hiob)

#define  SCSI_hSETBREAKPOINT(hhcb,entryBitMap) \
            SCSIhSwapping32SetBreakPoint(hhcb,entryBitMap)
#define  SCSI_hCLEARBREAKPOINT(hhcb,entryBitMap) \
            SCSIhSwapping32ClearBreakPoint(hhcb,entryBitMap)

#define  SCSI_hUPDATESGLENGTH(hhcb,regValue)
#define  SCSI_hUPDATESGADDRESS(hhcb,regValue)
#define  SCSI_hFREEZEHWQUEUE(hhcb,hiob) \
            SCSIhSwapping32FreezeHWQueue(hhcb,hiob)

#define  SCSI_hSETINTRTHRESHOLD(hhcb,threshold)
#define  SCSI_hGETINTRTHRESHOLD(hhcb) SCSIhSwappingGetIntrThreshold(hhcb)

#define  SCSI_hENABLEAUTORATEOPTION(hhcb)
#define  SCSI_hDISABLEAUTORATEOPTION(hhcb,xferOption)

#define  SCSI_hSETPHASEDELAYVALUES(hhcb)

#if SCSI_TARGET_OPERATION 
#define  SCSI_hTARGETPATCHXFERRATE(hhcb,hiob)
#define  SCSI_hTARGETSETIGNOREWIDEMSG(hhcb,hiob) 
#define  SCSI_hTARGET_SEND_HIOB_SPECIAL(hhcb,hiob)
#define  SCSI_hTARGETGETESTSCBFIELDS(hhcb,hiob,flag)
#define  SCSI_hTARGETDELIVERESTSCB(hhcb,hiob)
#endif /* SCSI_TARGET_OPERATION */

#define  SCSI_hFIRMWARE_VERSION(hhcb)        SCSI_W32_FIRMWARE_VERSION
#define  SCSI_hBTATABLE(hhcb)                SCSI_W32_BTATABLE
#define  SCSI_hRESCNT_BASE(hhcb)             SCSI_W32_RESCNT_BASE

#define  SCSI_hDISCON_OPTION(hhcb)           SCSI_W32_DISCON_OPTION
#define  SCSI_hSIOSTR3_ENTRY(hhcb)           SCSI_W32_SIOSTR3_ENTRY
#define  SCSI_hATN_TMR_ENTRY(hhcb)           SCSI_W32_ATN_TMR_ENTRY
#define  SCSI_hTARG_LUN_MASK0(hhcb)          SCSI_W32_TARG_LUN_MASK0
#define  SCSI_hQ_DONE_ELEMENT(hhcb)          SCSI_W32_Q_DONE_ELEMENT
#define  SCSI_hPASS_TO_DRIVER(hhcb)          SCSI_W32_PASS_TO_DRIVER
#define  SCSI_hSCSIDATL_IMAGE(hhcb)          SCSI_W32_PASS_TO_DRIVER
#define  SCSI_hSTART_LINK_CMD_ENTRY(hhcb)    SCSI_W32_START_LINK_CMD_ENTRY
#define  SCSI_hIDLE_LOOP_ENTRY(hhcb)         SCSI_W32_IDLE_LOOP_ENTRY
#define  SCSI_hIDLE_LOOP0(hhcb)              SCSI_W32_IDLE_LOOP0
#if SCSI_TARGET_OPERATION 
#define  SCSI_hTARGET_DATA_ENTRY(hhcb)       SCSI_W32_TARGET_DATA_ENTRY
#define  SCSI_hTARGET_HELD_BUS_ENTRY(hhcb)   SCSI_W32_TARGET_HELD_BUS_ENTRY
#endif /* SCSI_TARGET_OPERATION */
#define  SCSI_hACTIVE_SCB(hhcb)              SCSI_W32_ACTIVE_SCB
#define  SCSI_hQ_EXE_HEAD(hhcb)              SCSI_W32_Q_EXE_HEAD
#define  SCSI_hARRAY_PARTITION0(hhcb)        SCSI_W32_ARRAY_PARTITION0
#define  SCSI_hQ_NEW_HEAD(hhcb)              SCSI_W32_Q_NEW_HEAD
#define  SCSI_hQ_NEW_POINTER(hhcb)           SCSI_W32_Q_NEW_BASE
#define  SCSI_hQ_NEW_TAIL(hhcb)              SCSI_W32_Q_NEW_TAIL
#define  SCSI_hQ_NEXT_SCB(hhcb)              SCSI_W32_Q_NEXT_SCB 
#define  SCSI_hQ_DONE_HEAD(hhcb)             SCSI_W32_Q_DONE_HEAD
#define  SCSI_hWAITING_SCB(hhcb)             SCSI_W32_WAITING_SCB
#define  SCSI_hQ_DONE_BASE(hhcb)             SCSI_W32_Q_DONE_BASE
#define  SCSI_hSEQUENCER_PROGRAM(hhcb)       SCSI_W32_SEQUENCER_PROGRAM
#define  SCSI_hQ_DONE_PASS(hhcb)             SCSI_W32_Q_DONE_PASS
#define  SCSI_hXFER_OPTION(hhcb)             SCSI_W32_XFER_OPTION
#define  SCSI_hSIO204_ENTRY(hhcb)            SCSI_W32_SIO204_ENTRY
#define  SCSI_hSIO215(hhcb)                  SCSI_W32_SIO215
#define  SCSI_hFAST20_LOW(hhcb)              SCSI_W32_FAST20_LOW
#define  SCSI_hFAST20_HIGH(hhcb)             SCSI_W32_FAST20_HIGH
#define  SCSI_hMWI_CACHE_MASK(hhcb)          SCSI_W32_MWI_CACHE_MASK
#define  SCSI_hENT_PT_BITMAP(hhcb)           SCSI_W32_ENT_PT_BITMAP
#define  SCSI_hAUTO_RATE_ENABLE(hhcb)        SCSI_W32_AUTO_RATE_ENABLE
#define  SCSI_hEXPANDER_BREAK(hhcb)          SCSI_W32_EXPANDER_BREAK
#if SCSI_TARGET_OPERATION
#define  SCSI_hQ_EST_HEAD(hhcb)              SCSI_W32_Q_EST_HEAD
#define  SCSI_hQ_EST_TAIL(hhcb)              SCSI_W32_Q_EST_TAIL
#define  SCSI_hQ_EST_NEXT_SCB(hhcb)          SCSI_W32_Q_EST_NEXT_SCB
#endif /* SCSI_TARGET_OPERATION */
#endif /* SCSI_SWAPPING32_MODE */

#if SCSI_STANDARD_ADVANCED_MODE
/***************************************************************************
* Macros for standard advanced mode exclusively
***************************************************************************/
#define  SCSI_hSIZEOFSCBBUFFER(mode) SCSI_SADV_SIZE_SCB
#define  SCSI_hALIGNMENTSCBBUFFER(mode) SCSI_hSIZEOFSCBBUFFER((mode))-1
#define  SCSI_hSETUPFIRMWAREDESC(hhcb)
#define  SCSI_hSETUPSEQUENCER(hhcb) SCSIhStandardAdvSetupSequencer((hhcb))
#define  SCSI_hRESETSOFTWARE(hhcb)   SCSIhStandardAdvResetSoftware((hhcb))
#define  SCSI_hDELIVERSCB(hhcb,hiob)  SCSIhDeliverScb((hhcb),(hiob))
#define  SCSI_hPATCH_XFER_OPT(hhcb,hiob) SCSIhStandardAdvPatchXferOpt((hhcb),(hiob))
#define  SCSI_hENQUEHEADBDR(hhcb,hiob)  SCSIhStandardAdvQHeadBDR((hhcb),(hiob))
#define  SCSI_hRETRIEVESCB(hhcb)  SCSIhStandardRetrieveScb(hhcb)
#define  SCSI_hQOUTCNT(hhcb) SCSIhStandardQoutcnt(hhcb)
#define  SCSI_hTARGETCLEARBUSY(hhcb,hiob) SCSIhStandardTargetClearBusy((hhcb),(hiob))
#define  SCSI_hSIZEQOUTPTRARRAY(hhcb) (256 * sizeof(QOUTFIFO_NEW))
#define  SCSI_hREQUESTSENSE(hhcb,hiob) SCSIhStandardAdvRequestSense((hhcb),(hiob))
#define  SCSI_hRESETBTA(hhcb) SCSIhStandardResetBTA((hhcb))
#define  SCSI_hGETCONFIGURATION(hhcb) SCSIhStandardGetConfig((hhcb))
#define  SCSI_hCONFIGURESCBRAM(hhcb) SCSIhStandardConfigureScbRam((hhcb))
#if SCSI_SCBBFR_BUILTIN
#define  SCSI_hSETUP_ASSIGN_SCB_BUFFER(hhcb) SCSIhSetupAssignScbBuffer((hhcb))
#define  SCSI_hASSIGN_SCB_BUFFER(hhcb,hiob)
#else
#define  SCSI_hSETUP_ASSIGN_SCB_BUFFER(hhcb)
#define  SCSI_hASSIGN_SCB_BUFFER(hhcb,hiob)
#endif
#define  SCSI_hCHKCONDXFERASSIGN(hhcb,i,xfer_option) \
            SCSIhChkCondXferAssign(hhcb,i,xfer_option)
#define  SCSI_hXFEROPTASSIGN(hhcb,i,xfer_option) \
            SCSIhHostXferOptAssign(hhcb,i,xfer_option)

#define  SCSI_hCOMMON_XFER_ASSIGN(hhcb,targetID,value) \
            SCSIhStandardAdvCommonXferAssign(hhcb,targetID,value)
#define  SCSI_hUPDATE_XFER_OPTION_HOST_MEM(hhcb,hiob,value) \
            SCSIhStandardAdvUpdateXferOptionHostMem(hhcb,hiob,value)
#define  SCSI_hGETXFEROPTION(hhcb,i) SCSIhHostGetOption(hhcb,i)

#define  SCSI_hGETFAST20REG(hhcb,targetID) SCSIhStandardAdvGetFast20Reg(hhcb,targetID)
#define  SCSI_hCLEARFAST20REG(hhcb,targetID)
#define  SCSI_hLOGFAST20MAP(hhcb,targetID,period)
#define  SCSI_hUPDATE_FAST20_HOST_MEM(hhcb,hiob,flag)
#define  SCSI_hUPDATE_FAST20_HW(hhcb,flag)
#define  SCSI_hCLEAR_ALL_FAST20_REG(hhcb)
#define  SCSI_hMODIFYDATAPTR(hhcb,hiob) SCSIhStandardAdvModifyDataPtr(hhcb,hiob)
#define  SCSI_hACTIVEABORT(hhcb,hiob) \
            SCSIhStandardAdvActiveAbort(hhcb,hiob)
#define  SCSI_hABORTHIOB(hhcb,hiob,haStatus) \
            SCSIhStandardAdvAbortHIOB(hhcb,hiob,haStatus)
#define  SCSI_hSEARCHSEQDONEQ(hhcb,hiob,postHiob) \
            SCSIhStandardAdvSearchSeqDoneQ(hhcb,hiob,postHiob)
#define  SCSI_hSEARCHDONEQ(hhcb,hiob) \
            SCSIhStandardSearchDoneQ(hhcb,hiob)
#define  SCSI_hSEARCHNEWQ(hhcb,hiob,haStatus) \
            SCSIhStandardSearchNewQ(hhcb,hiob,haStatus)
#define  SCSI_hSEARCHEXEQ(hhcb,hiob,haStatus,postHiob) \
            SCSIhStandardAdvSearchExeQ(hhcb,hiob,haStatus,postHiob)

#define  SCSI_hUPDATE_ABORT_BIT_HOST_MEM(hhcb,hiob) \
            SCSIhStandardAdvUpdateAbortBitHostMem(hhcb,hiob)

#define  SCSI_hOBTAIN_NEXT_SCB_NUM(hhcb,hiob) \
           SCSIhStandardAdvObtainNextScbNum(hhcb,hiob)
#define  SCSI_hUPDATE_NEXT_SCB_NUM(hhcb,hiob,scbNumber) \
            SCSIhStandardAdvUpdateNextScbNum(hhcb,hiob,scbNumber)
#define  SCSI_hOBTAIN_NEXT_SCB_ADDRESS(hhcb,hiob) \
           SCSIhStandardAdvObtainNextScbAddress(hhcb,hiob)
#define  SCSI_hUPDATE_NEXT_SCB_ADDRESS(hhcb,hiob,ScbBusAddress) \
           SCSIhStandardAdvUpdateNextScbAddress(hhcb,hiob,ScbBusAddress)

#define  SCSI_hRESETCCCTL(hhcb) SCSIhStandardResetCCCtl(hhcb)
#define  SCSI_hINITCCHHADDR(hhcb) SCSIhStandardInitCCHHAddr(hhcb)
#define  SCSI_hREMOVEACTIVEABORT(hhcb,hiob) \
            SCSIhStandardRemoveActiveAbort(hhcb,hiob)

#define  SCSI_hSTACKBUGFIX(hhcb)

#define  SCSI_hSETUPTARGETRESET(hhcb,hiob) SCSIhStandardAdvSetupBDR(hhcb,hiob)
#define  SCSI_hRESIDUECALC(hhcb,hiob) SCSIhStandardAdvResidueCalc(hhcb,hiob)

#define  SCSI_hSETBREAKPOINT(hhcb,entryBitMap) \
            SCSIhStandardAdvSetBreakPoint(hhcb,entryBitMap)
#define  SCSI_hCLEARBREAKPOINT(hhcb,entryBitMap) \
            SCSIhStandardAdvClearBreakPoint(hhcb,entryBitMap)

#define  SCSI_hUPDATESGLENGTH(hhcb,regValue) \
            SCSIhStandardAdvUpdateSGLength(hhcb,regValue)
#define  SCSI_hUPDATESGADDRESS(hhcb,regValue) \
            SCSIhStandardAdvUpdateSGAddress(hhcb,regValue)
#define  SCSI_hFREEZEHWQUEUE(hhcb,hiob) \
            SCSIhStandardAdvFreezeHWQueue(hhcb,hiob)

#define  SCSI_hSETINTRTHRESHOLD(hhcb,threshold) \
            SCSIhStandardAdvSetIntrThreshold(hhcb,threshold)
#define  SCSI_hGETINTRTHRESHOLD(hhcb) \
            SCSIhStandardAdvGetIntrThreshold(hhcb)

#define  SCSI_hENABLEAUTORATEOPTION(hhcb)
#define  SCSI_hDISABLEAUTORATEOPTION(hhcb,xferOption)

#define  SCSI_hSETPHASEDELAYVALUES(hhcb)

#if SCSI_TARGET_OPERATION 
#define  SCSI_hTARGETPATCHXFERRATE(hhcb,hiob)
#define  SCSI_hTARGETSETIGNOREWIDEMSG(hhcb,hiob) 
#define  SCSI_hTARGET_SEND_HIOB_SPECIAL(hhcb,hiob)
#define  SCSI_hTARGETGETESTSCBFIELDS(hhcb,hiob,flag)
#define  SCSI_hTARGETDELIVERESTSCB(hhcb,hiob)
#endif /* SCSI_TARGET_OPERATION */

#define  SCSI_hFIRMWARE_VERSION(hhcb)        SCSI_SADV_FIRMWARE_VERSION
#define  SCSI_hBTATABLE(hhcb)                SCSI_SADV_BTATABLE
#define  SCSI_hRESCNT_BASE(hhcb)             SCSI_SADV_RESCNT_BASE

#define  SCSI_hDISCON_OPTION(hhcb)           SCSI_SADV_DISCON_OPTION
#define  SCSI_hSIOSTR3_ENTRY(hhcb)           SCSI_SADV_SIOSTR3_ENTRY
#define  SCSI_hATN_TMR_ENTRY(hhcb)           SCSI_SADV_ATN_TMR_ENTRY
#define  SCSI_hTARG_LUN_MASK0(hhcb)          SCSI_SADV_TARG_LUN_MASK0
#define  SCSI_hQ_DONE_ELEMENT(hhcb)          SCSI_SADV_Q_DONE_ELEMENT
#define  SCSI_hPASS_TO_DRIVER(hhcb)          SCSI_SADV_PASS_TO_DRIVER
#define  SCSI_hSCSIDATL_IMAGE(hhcb)          SCSI_SADV_PASS_TO_DRIVER
#define  SCSI_hSTART_LINK_CMD_ENTRY(hhcb)    SCSI_SADV_START_LINK_CMD_ENTRY
#define  SCSI_hIDLE_LOOP_ENTRY(hhcb)         SCSI_SADV_IDLE_LOOP_ENTRY
#define  SCSI_hIDLE_LOOP0(hhcb)              SCSI_SADV_IDLE_LOOP0
#if SCSI_TARGET_OPERATION 
#define  SCSI_hTARGET_DATA_ENTRY(hhcb)       SCSI_SADV_TARGET_DATA_ENTRY
#define  SCSI_hTARGET_HELD_BUS_ENTRY(hhcb)   SCSI_SADV_TARGET_HELD_BUS_ENTRY
#endif /* SCSI_TARGET_OPERATION */
#define  SCSI_hACTIVE_SCB(hhcb)              SCSI_SADV_ACTIVE_SCB
#define  SCSI_hQ_EXE_HEAD(hhcb)              SCSI_SADV_Q_EXE_HEAD
#define  SCSI_hARRAY_PARTITION0(hhcb)        SCSI_SADV_ARRAY_PARTITION0
#define  SCSI_hQ_NEW_HEAD(hhcb)              SCSI_SADV_Q_NEW_HEAD
#define  SCSI_hQ_NEW_POINTER(hhcb)           SCSI_SADV_Q_NEW_POINTER
#define  SCSI_hQ_NEW_TAIL(hhcb)              SCSI_SADV_Q_NEW_TAIL
#define  SCSI_hQ_DONE_HEAD(hhcb)             SCSI_SADV_Q_DONE_HEAD
#define  SCSI_hQ_NEXT_SCB(hhcb)              SCSI_SADV_Q_NEXT_SCB  
#define  SCSI_hWAITING_SCB(hhcb)             SCSI_SADV_WAITING_SCB
#define  SCSI_hQ_DONE_BASE(hhcb)             SCSI_SADV_Q_DONE_BASE
#define  SCSI_hSEQUENCER_PROGRAM(hhcb)       SCSI_SADV_SEQUENCER_PROGRAM
#define  SCSI_hQ_DONE_PASS(hhcb)             SCSI_SADV_Q_DONE_PASS
#define  SCSI_hXFER_OPTION(hhcb)             SCSI_SADV_XFER_OPTION
#define  SCSI_hSIO204_ENTRY(hhcb)            SCSI_SADV_SIO204_ENTRY
#define  SCSI_hSIO215(hhcb)                  SCSI_SADV_SIO215
#define  SCSI_hFAST20_LOW(hhcb)              SCSI_SADV_FAST20_LOW
#define  SCSI_hFAST20_HIGH(hhcb)             SCSI_SADV_FAST20_HIGH
#define  SCSI_hMWI_CACHE_MASK(hhcb)          SCSI_SADV_MWI_CACHE_MASK
#define  SCSI_hENT_PT_BITMAP(hhcb)           SCSI_SADV_ENT_PT_BITMAP
#define  SCSI_hAUTO_RATE_ENABLE(hhcb)        SCSI_SADV_AUTO_RATE_ENABLE
#define  SCSI_hEXPANDER_BREAK(hhcb)          SCSI_SADV_EXPANDER_BREAK
#if SCSI_TARGET_OPERATION
#define  SCSI_hQ_EST_HEAD(hhcb)              SCSI_SADV_Q_EST_HEAD
#define  SCSI_hQ_EST_TAIL(hhcb)              SCSI_SADV_Q_EST_TAIL
#define  SCSI_hQ_EST_NEXT_SCB(hhcb)          SCSI_SADV_Q_EST_NEXT_SCB
#endif /* SCSI_TARGET_OPERATION */
#endif /* SCSI_STANDARD_ADVANCED_MODE */


#if SCSI_SWAPPING_ADVANCED_MODE
/***************************************************************************
* Macros for swapping advanced mode exclusively
***************************************************************************/
#define  SCSI_hSIZEOFSCBBUFFER(mode)  SCSI_WADV_SIZE_SCB
#define  SCSI_hALIGNMENTSCBBUFFER(mode) SCSI_hSIZEOFSCBBUFFER((mode))-1
#define  SCSI_hSETUPFIRMWAREDESC(hhcb)
#define  SCSI_hSETUPSEQUENCER(hhcb) SCSIhSwappingAdvSetupSequencer((hhcb))
#define  SCSI_hRESETSOFTWARE(hhcb)   SCSIhSwappingAdvResetSoftware((hhcb))
#define  SCSI_hDELIVERSCB(hhcb,hiob)  SCSIhDeliverScb((hhcb),(hiob)) 
#define  SCSI_hPATCH_XFER_OPT(hhcb,hiob) SCSIhSwappingAdvPatchXferOpt((hhcb),(hiob))
#define  SCSI_hENQUEHEADBDR(hhcb,hiob)  SCSIhSwappingAdvQHeadBDR((hhcb),(hiob))
#define  SCSI_hRETRIEVESCB(hhcb)  SCSIhStandardRetrieveScb(hhcb)
#define  SCSI_hQOUTCNT(hhcb) SCSIhStandardQoutcnt(hhcb)
#define  SCSI_hTARGETCLEARBUSY(hhcb,hiob) SCSIhSwappingTargetClearBusy((hhcb),(hiob))
#define  SCSI_hSIZEQOUTPTRARRAY(hhcb) (256 * sizeof(QOUTFIFO_NEW))
#define  SCSI_hREQUESTSENSE(hhcb,hiob) SCSIhSwappingAdvRequestSense((hhcb),(hiob))
#define  SCSI_hRESETBTA(hhcb) SCSIhSwappingResetBTA(hhcb)
#define  SCSI_hGETCONFIGURATION(hhcb) SCSIhNonStandardGetConfig((hhcb))
#define  SCSI_hCONFIGURESCBRAM(hhcb)
#define  SCSI_hSETUP_ASSIGN_SCB_BUFFER(hhcb) SCSIhSetupAssignScbBuffer((hhcb))

#if SCSI_RESOURCE_MANAGEMENT 
#define  SCSI_hASSIGN_SCB_BUFFER(hhcb,hiob)
#else
#define  SCSI_hASSIGN_SCB_BUFFER(hhcb,hiob) SCSIhSwappingAssignScbBuffer((hhcb),(hiob))
#endif /* SCSI_RESOURCE_MANAGEMENT */

#define  SCSI_hCHKCONDXFERASSIGN(hhcb,i,xfer_option) \
            SCSIhChkCondXferAssign(hhcb,i,xfer_option)

#if SCSI_TARGET_OPERATION && SCSI_MULTIPLEID_SUPPORT
/* Need to do some work for multiple H/W support - assumes only Bayonet with */
/* multiple id support.                                                      */ 
#define  SCSI_hXFEROPTASSIGN(hhcb,i,xfer_option) \
            SCSIhMultipleHostIdXferOptAssign((hhcb),(i),(hhcb->hostScsiID),(xfer_option))
#define  SCSI_hCOMMON_XFER_ASSIGN(hhcb,targetID,value) \
            SCSIhMultipleHostIdCommonXferAssign((hhcb),(targetID),(hhcb->hostScsiID),(value))
#define  SCSI_hGETXFEROPTION(hhcb,i) SCSIhMultipleHostIdGetOption((hhcb),(i),(hhcb->hostScsiID))
#define  SCSI_hMULTIPLEIDXFEROPTASSIGN(hhcb,i,j,xfer_option) \
            SCSIhMultipleHostIdXferOptAssign((hhcb),(i),(j),(xfer_option))
#define  SCSI_hMULTIID_COMMON_XFER_ASSIGN(hhcb,targetID,hostID,value) \
            SCSIhMultipleHostIdCommonXferAssign((hhcb),(targetID),(hostID),(value))
#define  SCSI_hGETMULTIPLEIDXFEROPTION(hhcb,i,j) SCSIhMultipleHostIdGetOption((hhcb),(i),(j)) 
#else
#define  SCSI_hXFEROPTASSIGN(hhcb,i,xfer_option) \
            SCSIhHostXferOptAssign(hhcb,i,xfer_option)
#define  SCSI_hCOMMON_XFER_ASSIGN(hhcb,targetID,value) \
            SCSIhSwappingAdvCommonXferAssign((hhcb),(targetID),(value))
#define  SCSI_hGETXFEROPTION(hhcb,i) SCSIhHostGetOption(hhcb,i)
#endif /* SCSI_TARGET_OPERATION && SCSI_MULTIPLEID_SUPPORT */

#define  SCSI_hUPDATE_XFER_OPTION_HOST_MEM(hhcb,hiob,value) \
            SCSIhSwappingAdvUpdateXferOptionHostMem(hhcb,hiob,value)
#define  SCSI_hGETFAST20REG(hhcb,targetID) SCSIhSwappingAdvGetFast20Reg(hhcb,targetID)
#define  SCSI_hCLEARFAST20REG(hhcb,targetID)
#define  SCSI_hLOGFAST20MAP(hhcb,targetID,period)
#define  SCSI_hUPDATE_FAST20_HOST_MEM(hhcb,hiob,flag)
#define  SCSI_hUPDATE_FAST20_HW(hhcb,flag)
#define  SCSI_hCLEAR_ALL_FAST20_REG(hhcb)
#define  SCSI_hMODIFYDATAPTR(hhcb,hiob) SCSIhSwappingAdvModifyDataPtr(hhcb,hiob)
#define  SCSI_hACTIVEABORT(hhcb,hiob) \
            SCSIhSwappingAdvActiveAbort(hhcb,hiob)
#define  SCSI_hABORTHIOB(hhcb,hiob,haStatus) \
            SCSIhSwappingAdvAbortHIOB(hhcb,hiob,haStatus)
#define  SCSI_hSEARCHSEQDONEQ(hhcb,hiob,postHiob)
#define  SCSI_hSEARCHDONEQ(hhcb,hiob) \
            SCSIhStandardSearchDoneQ(hhcb,hiob)
#define  SCSI_hSEARCHNEWQ(hhcb,hiob,haStatus) \
            SCSIhSwappingSearchNewQ(hhcb,hiob,haStatus)
#define  SCSI_hSEARCHEXEQ(hhcb,hiob,haStatus,postHiob) \
            SCSIhSwappingSearchExeQ(hhcb,hiob,haStatus,postHiob)

#define  SCSI_hUPDATE_ABORT_BIT_HOST_MEM(hhcb,hiob) \
            SCSIhSwappingAdvUpdateAbortBitHostMem(hhcb,hiob)
#define  SCSI_hOBTAIN_NEXT_SCB_NUM(hhcb,hiob) \
           SCSIhSwappingAdvObtainNextScbNum(hhcb,hiob)
#define  SCSI_hUPDATE_NEXT_SCB_NUM(hhcb,hiob,scbNumber) \
            SCSIhSwappingAdvUpdateNextScbNum(hhcb,hiob,scbNumber)
#define  SCSI_hOBTAIN_NEXT_SCB_ADDRESS(hhcb,hiob)
#define  SCSI_hUPDATE_NEXT_SCB_ADDRESS(hhcb,hiob,ScbBusAddress)

#define  SCSI_hRESETCCCTL(hhcb) SCSIhStandardResetCCCtl(hhcb)
#define  SCSI_hINITCCHHADDR(hhcb) SCSIhStandardInitCCHHAddr(hhcb)
#define  SCSI_hREMOVEACTIVEABORT(hhcb,hiob) \
            SCSIhSwappingRemoveActiveAbort(hhcb,hiob)

#define  SCSI_hSTACKBUGFIX(hhcb)

#define  SCSI_hSETUPTARGETRESET(hhcb,hiob) SCSIhSwappingAdvSetupBDR(hhcb,hiob)
#define  SCSI_hRESIDUECALC(hhcb,hiob) SCSIhSwappingAdvResidueCalc(hhcb,hiob)

#define  SCSI_hSETBREAKPOINT(hhcb,entryBitMap) \
            SCSIhSwappingAdvSetBreakPoint(hhcb,entryBitMap)
#define  SCSI_hCLEARBREAKPOINT(hhcb,entryBitMap) \
            SCSIhSwappingAdvClearBreakPoint(hhcb,entryBitMap)

#define  SCSI_hUPDATESGLENGTH(hhcb,regValue) \
            SCSIhSwappingAdvUpdateSGLength(hhcb,regValue)
#define  SCSI_hUPDATESGADDRESS(hhcb,regValue) \
            SCSIhSwappingAdvUpdateSGAddress(hhcb,regValue)
#define  SCSI_hFREEZEHWQUEUE(hhcb,hiob) \
            SCSIhSwappingAdvFreezeHWQueue(hhcb,hiob)

#define  SCSI_hSETINTRTHRESHOLD(hhcb,threshold) \
            SCSIhSwappingAdvSetIntrThreshold(hhcb,threshold)
#define  SCSI_hGETINTRTHRESHOLD(hhcb) \
            SCSIhSwappingAdvGetIntrThreshold(hhcb)

#define  SCSI_hENABLEAUTORATEOPTION(hhcb)
#define  SCSI_hDISABLEAUTORATEOPTION(hhcb,xferOption)

#define  SCSI_hSETPHASEDELAYVALUES(hhcb)

#if SCSI_TARGET_OPERATION 
#define  SCSI_hTARGETPATCHXFERRATE(hhcb,hiob)  \
            SCSIhTargetSwappingAdvPatchXferRate((hhcb),(hiob)) 
#define  SCSI_hTARGETSETIGNOREWIDEMSG(hhcb,hiob)  \
            SCSIhTargetSwappingAdvSetIgnoreWideMsg((hhcb),(hiob)) 
#define  SCSI_hTARGET_SEND_HIOB_SPECIAL(hhcb,hiob) \
            SCSIhTargetSwappingSendHiobSpecial((hhcb),(hiob))
#define  SCSI_hTARGETDELIVERESTSCB(hhcb,hiob) \
            SCSIhTargetSwappingAdvDeliverEstScb((hhcb),(hiob))
#define  SCSI_hTARGETGETESTSCBFIELDS(hhcb,hiob,flag) \
            SCSIhTargetSwappingAdvGetEstScbFields((hhcb),(hiob),(flag))
#endif /* SCSI_TARGET_OPERATION */

#define  SCSI_hFIRMWARE_VERSION(hhcb)        SCSI_WADV_FIRMWARE_VERSION
#define  SCSI_hBTATABLE(hhcb)                SCSI_WADV_BTATABLE
#define  SCSI_hRESCNT_BASE(hhcb)             SCSI_WADV_RESCNT_BASE

#define  SCSI_hDISCON_OPTION(hhcb)           SCSI_WADV_DISCON_OPTION
#define  SCSI_hSIOSTR3_ENTRY(hhcb)           SCSI_WADV_SIOSTR3_ENTRY
#define  SCSI_hATN_TMR_ENTRY(hhcb)           SCSI_WADV_ATN_TMR_ENTRY
#define  SCSI_hTARG_LUN_MASK0(hhcb)          SCSI_WADV_TARG_LUN_MASK0
#define  SCSI_hQ_DONE_ELEMENT(hhcb)          SCSI_WADV_Q_DONE_ELEMENT
#define  SCSI_hPASS_TO_DRIVER(hhcb)          SCSI_WADV_PASS_TO_DRIVER
#define  SCSI_hSCSIDATL_IMAGE(hhcb)          SCSI_WADV_PASS_TO_DRIVER
#define  SCSI_hSTART_LINK_CMD_ENTRY(hhcb)    SCSI_WADV_START_LINK_CMD_ENTRY
#define  SCSI_hIDLE_LOOP_ENTRY(hhcb)         SCSI_WADV_IDLE_LOOP_ENTRY   
#define  SCSI_hIDLE_LOOP0(hhcb)              SCSI_WADV_IDLE_LOOP0
#if SCSI_TARGET_OPERATION 
#define  SCSI_hTARGET_DATA_ENTRY(hhcb)       SCSI_WADV_TARGET_DATA_ENTRY
#define  SCSI_hTARGET_HELD_BUS_ENTRY(hhcb)   SCSI_WADV_TARGET_HELD_BUS_ENTRY
#endif /* SCSI_TARGET_OPERATION */
#define  SCSI_hACTIVE_SCB(hhcb)              SCSI_WADV_ACTIVE_SCB
#define  SCSI_hQ_EXE_HEAD(hhcb)              SCSI_WADV_Q_EXE_HEAD
#define  SCSI_hARRAY_PARTITION0(hhcb)        SCSI_WADV_ARRAY_PARTITION0
#define  SCSI_hQ_NEW_HEAD(hhcb)              SCSI_WADV_Q_NEW_HEAD
#define  SCSI_hQ_NEW_POINTER(hhcb)           SCSI_WADV_Q_NEW_BASE
#define  SCSI_hQ_NEW_TAIL(hhcb)              SCSI_WADV_Q_NEW_TAIL
#define  SCSI_hQ_NEXT_SCB(hhcb)              SCSI_WADV_Q_NEXT_SCB  
#define  SCSI_hQ_DONE_HEAD(hhcb)             SCSI_WADV_Q_DONE_HEAD
#define  SCSI_hWAITING_SCB(hhcb)             SCSI_WADV_WAITING_SCB
#define  SCSI_hQ_DONE_BASE(hhcb)             SCSI_WADV_Q_DONE_BASE
#define  SCSI_hSEQUENCER_PROGRAM(hhcb)       SCSI_WADV_SEQUENCER_PROGRAM
#define  SCSI_hQ_DONE_PASS(hhcb)             SCSI_WADV_Q_DONE_PASS
#define  SCSI_hXFER_OPTION(hhcb)             SCSI_WADV_XFER_OPTION
#define  SCSI_hSIO204_ENTRY(hhcb)            SCSI_WADV_SIO204_ENTRY
#define  SCSI_hSIO215(hhcb)                  SCSI_WADV_SIO215
#define  SCSI_hFAST20_LOW(hhcb)              SCSI_WADV_FAST20_LOW
#define  SCSI_hFAST20_HIGH(hhcb)             SCSI_WADV_FAST20_HIGH
#define  SCSI_hMWI_CACHE_MASK(hhcb)          SCSI_WADV_MWI_CACHE_MASK
#define  SCSI_hENT_PT_BITMAP(hhcb)           SCSI_WADV_ENT_PT_BITMAP
#define  SCSI_hAUTO_RATE_ENABLE(hhcb)        SCSI_WADV_AUTO_RATE_ENABLE
#define  SCSI_hEXPANDER_BREAK(hhcb)          SCSI_WADV_EXPANDER_BREAK
#if SCSI_TARGET_OPERATION
#define  SCSI_hQ_EST_HEAD(hhcb)              SCSI_WADV_Q_EST_HEAD
#define  SCSI_hQ_EST_TAIL(hhcb)              SCSI_WADV_Q_EST_TAIL
#define  SCSI_hQ_EST_NEXT_SCB(hhcb)          SCSI_WADV_Q_EST_NEXT_SCB
#endif /* SCSI_TARGET_OPERATION */
#endif

#if SCSI_STANDARD_160M_MODE
/***************************************************************************
* Macros for standard ultra 160m mode exclusively
***************************************************************************/
#define  SCSI_hSIZEOFSCBBUFFER(mode) SCSI_S160M_SIZE_SCB
#define  SCSI_hALIGNMENTSCBBUFFER(mode) SCSI_hSIZEOFSCBBUFFER((mode))-1
#define  SCSI_hSETUPFIRMWAREDESC(hhcb)
#define  SCSI_hSETUPSEQUENCER(hhcb) SCSIhStandard160mSetupSequencer((hhcb))
#define  SCSI_hRESETSOFTWARE(hhcb) SCSIhStandard160mResetSoftware((hhcb))
#define  SCSI_hDELIVERSCB(hhcb,hiob) SCSIhDeliverScb((hhcb),(hiob))
#define  SCSI_hPATCH_XFER_OPT(hhcb,hiob) SCSIhStandard160mPatchXferOpt((hhcb),(hiob))
#define  SCSI_hENQUEHEADBDR(hhcb,hiob) SCSIhStandard160mQHeadBDR((hhcb),(hiob))
#define  SCSI_hRETRIEVESCB(hhcb) SCSIhStandardRetrieveScb(hhcb)
#define  SCSI_hQOUTCNT(hhcb) SCSIhStandardQoutcnt(hhcb)
#define  SCSI_hTARGETCLEARBUSY(hhcb,hiob) SCSIhStandardTargetClearBusy((hhcb),(hiob))
#define  SCSI_hSIZEQOUTPTRARRAY(hhcb) (256 * sizeof(QOUTFIFO_NEW))
#define  SCSI_hREQUESTSENSE(hhcb,hiob) SCSIhStandard160mRequestSense((hhcb),(hiob))
#define  SCSI_hRESETBTA(hhcb) SCSIhStandardResetBTA((hhcb))
#define  SCSI_hGETCONFIGURATION(hhcb) SCSIhStandardGetConfig((hhcb))
#define  SCSI_hCONFIGURESCBRAM(hhcb) SCSIhStandardConfigureScbRam((hhcb))
#if SCSI_SCBBFR_BUILTIN
#define  SCSI_hSETUP_ASSIGN_SCB_BUFFER(hhcb) SCSIhSetupAssignScbBuffer((hhcb))
#define  SCSI_hASSIGN_SCB_BUFFER(hhcb,hiob)
#else
#define  SCSI_hSETUP_ASSIGN_SCB_BUFFER(hhcb)
#define  SCSI_hASSIGN_SCB_BUFFER(hhcb,hiob)
#endif
#define  SCSI_hCHKCONDXFERASSIGN(hhcb,i,xfer_option) \
            SCSIhChkCondXferAssign(hhcb,i,xfer_option)
#define  SCSI_hXFEROPTASSIGN(hhcb,i,xfer_option) \
            SCSIhHostXferOptAssign(hhcb,i,xfer_option)

#define  SCSI_hCOMMON_XFER_ASSIGN(hhcb,targetID,value) \
            SCSIhStandard160mCommonXferAssign(hhcb,targetID,value)
#define  SCSI_hUPDATE_XFER_OPTION_HOST_MEM(hhcb,hiob,value) \
            SCSIhStandard160mUpdateXferOptionHostMem(hhcb,hiob,value)
#define  SCSI_hGETXFEROPTION(hhcb,i) SCSIhHostGetOption(hhcb,i)

#define  SCSI_hGETFAST20REG(hhcb,targetID) SCSIhStandardAdvGetFast20Reg(hhcb,targetID)
#define  SCSI_hCLEARFAST20REG(hhcb,targetID)
#define  SCSI_hLOGFAST20MAP(hhcb,targetID,period)
#define  SCSI_hUPDATE_FAST20_HOST_MEM(hhcb,hiob,flag)
#define  SCSI_hUPDATE_FAST20_HW(hhcb,flag)
#define  SCSI_hCLEAR_ALL_FAST20_REG(hhcb)
#define  SCSI_hMODIFYDATAPTR(hhcb,hiob) SCSIhStandard160mModifyDataPtr(hhcb,hiob)
#define  SCSI_hACTIVEABORT(hhcb,hiob) \
            SCSIhStandard160mActiveAbort(hhcb,hiob)
#define  SCSI_hABORTHIOB(hhcb,hiob,haStatus) \
            SCSIhStandard160mAbortHIOB(hhcb,hiob,haStatus)
#define  SCSI_hSEARCHSEQDONEQ(hhcb,hiob,postHiob) \
            SCSIhStandard160mSearchSeqDoneQ(hhcb,hiob,postHiob)
#define  SCSI_hSEARCHDONEQ(hhcb,hiob) \
            SCSIhStandardSearchDoneQ(hhcb,hiob)
#define  SCSI_hSEARCHNEWQ(hhcb,hiob,haStatus) \
            SCSIhStandardSearchNewQ(hhcb,hiob,haStatus)
#define  SCSI_hSEARCHEXEQ(hhcb,hiob,haStatus,postHiob) \
            SCSIhStandard160mSearchExeQ(hhcb,hiob,haStatus,postHiob)

#define  SCSI_hUPDATE_ABORT_BIT_HOST_MEM(hhcb,hiob) \
            SCSIhStandard160mUpdateAbortBitHostMem(hhcb,hiob)

#define  SCSI_hOBTAIN_NEXT_SCB_NUM(hhcb,hiob) \
           SCSIhStandard160mObtainNextScbNum(hhcb,hiob)
#define  SCSI_hUPDATE_NEXT_SCB_NUM(hhcb,hiob,scbNumber) \
            SCSIhStandard160mUpdateNextScbNum(hhcb,hiob,scbNumber)
#define  SCSI_hOBTAIN_NEXT_SCB_ADDRESS(hhcb,hiob) \
           SCSIhStandard160mObtainNextScbAddress(hhcb,hiob)
#define  SCSI_hUPDATE_NEXT_SCB_ADDRESS(hhcb,hiob,ScbBusAddress) \
           SCSIhStandard160mUpdateNextScbAddress(hhcb,hiob,ScbBusAddress)

#define  SCSI_hRESETCCCTL(hhcb) SCSIhStandardResetCCCtl(hhcb)
#define  SCSI_hINITCCHHADDR(hhcb) SCSIhStandardInitCCHHAddr(hhcb)
#define  SCSI_hREMOVEACTIVEABORT(hhcb,hiob) \
            SCSIhStandardRemoveActiveAbort(hhcb,hiob)

#define  SCSI_hSTACKBUGFIX(hhcb)

#define  SCSI_hSETUPTARGETRESET(hhcb,hiob) SCSIhStandard160mSetupBDR(hhcb,hiob)
#define  SCSI_hRESIDUECALC(hhcb,hiob) SCSIhStandard160mResidueCalc(hhcb,hiob)

#define  SCSI_hSETBREAKPOINT(hhcb,entryBitMap) \
            SCSIhStandard160mSetBreakPoint(hhcb,entryBitMap)
#define  SCSI_hCLEARBREAKPOINT(hhcb,entryBitMap) \
            SCSIhStandard160mClearBreakPoint(hhcb,entryBitMap)

#define  SCSI_hUPDATESGLENGTH(hhcb,regValue) \
            SCSIhStandard160mUpdateSGLength(hhcb,regValue)
#define  SCSI_hUPDATESGADDRESS(hhcb,regValue) \
            SCSIhStandard160mUpdateSGAddress(hhcb,regValue)
#define  SCSI_hFREEZEHWQUEUE(hhcb,hiob) \
            SCSIhStandard160mFreezeHWQueue(hhcb,hiob)

#define  SCSI_hSETINTRTHRESHOLD(hhcb,threshold) \
            SCSIhStandard160mSetIntrThreshold(hhcb,threshold)
#define  SCSI_hGETINTRTHRESHOLD(hhcb) \
            SCSIhStandard160mGetIntrThreshold(hhcb)

#define  SCSI_hENABLEAUTORATEOPTION(hhcb) SCSIhEnableAutoRateOption(hhcb);
#define  SCSI_hDISABLEAUTORATEOPTION(hhcb,xferOption) \
            SCSIhDisableAutoRateOption(hhcb,xferOption);

#define  SCSI_hSETPHASEDELAYVALUES(hhcb)

#if SCSI_TARGET_OPERATION 
#define  SCSI_hTARGETPATCHXFERRATE(hhcb,hiob)
#define  SCSI_hTARGETSETIGNOREWIDEMSG(hhcb,hiob) 
#define  SCSI_hTARGET_SEND_HIOB_SPECIAL(hhcb,hiob)
#define  SCSI_hTARGETGETESTSCBFIELDS(hhcb,hiob,flag)
#define  SCSI_hTARGETDELIVERESTSCB(hhcb,hiob)
#endif   /* SCSI_TARGET_OPERATION */

#define  SCSI_hFIRMWARE_VERSION(hhcb)        SCSI_S160M_FIRMWARE_VERSION
#define  SCSI_hBTATABLE(hhcb)                SCSI_S160M_BTATABLE
#define  SCSI_hRESCNT_BASE(hhcb)             SCSI_S160M_RESCNT_BASE

#define  SCSI_hDISCON_OPTION(hhcb)           SCSI_S160M_DISCON_OPTION
#define  SCSI_hSIOSTR3_ENTRY(hhcb)           SCSI_S160M_SIOSTR3_ENTRY
#define  SCSI_hATN_TMR_ENTRY(hhcb)           SCSI_S160M_ATN_TMR_ENTRY
#define  SCSI_hTARG_LUN_MASK0(hhcb)          SCSI_S160M_TARG_LUN_MASK0
#define  SCSI_hQ_DONE_ELEMENT(hhcb)          SCSI_S160M_Q_DONE_ELEMENT
#define  SCSI_hPASS_TO_DRIVER(hhcb)          SCSI_S160M_PASS_TO_DRIVER
#define  SCSI_hSCSIDATL_IMAGE(hhcb)          SCSI_S160M_SCSIDATL_IMAGE
#define  SCSI_hSTART_LINK_CMD_ENTRY(hhcb)    SCSI_S160M_START_LINK_CMD_ENTRY
#define  SCSI_hIDLE_LOOP_ENTRY(hhcb)         SCSI_S160M_IDLE_LOOP_ENTRY
#define  SCSI_hIDLE_LOOP0(hhcb)              SCSI_S160M_IDLE_LOOP0
#if SCSI_TARGET_OPERATION 
#define  SCSI_hTARGET_DATA_ENTRY(hhcb)       SCSI_S160M_TARGET_DATA_ENTRY
#define  SCSI_hTARGET_HELD_BUS_ENTRY(hhcb)   SCSI_S160M_TARGET_HELD_BUS_ENTRY
#endif /* SCSI_TARGET_OPERATION */
#define  SCSI_hACTIVE_SCB(hhcb)              SCSI_S160M_ACTIVE_SCB
#define  SCSI_hQ_EXE_HEAD(hhcb)              SCSI_S160M_Q_EXE_HEAD
#define  SCSI_hARRAY_PARTITION0(hhcb)        SCSI_S160M_ARRAY_PARTITION0
#define  SCSI_hQ_NEW_HEAD(hhcb)              SCSI_S160M_Q_NEW_HEAD
#define  SCSI_hQ_NEW_POINTER(hhcb)           SCSI_S160M_Q_NEW_POINTER
#define  SCSI_hQ_NEW_TAIL(hhcb)              SCSI_S160M_Q_NEW_TAIL
#define  SCSI_hQ_DONE_HEAD(hhcb)             SCSI_S160M_Q_DONE_HEAD
#define  SCSI_hQ_NEXT_SCB(hhcb)              SCSI_S160M_Q_NEXT_SCB  
#define  SCSI_hWAITING_SCB(hhcb)             SCSI_S160M_WAITING_SCB
#define  SCSI_hQ_DONE_BASE(hhcb)             SCSI_S160M_Q_DONE_BASE
#define  SCSI_hSEQUENCER_PROGRAM(hhcb)       SCSI_S160M_SEQUENCER_PROGRAM
#define  SCSI_hQ_DONE_PASS(hhcb)             SCSI_S160M_Q_DONE_PASS
#define  SCSI_hXFER_OPTION(hhcb)             SCSI_S160M_XFER_OPTION
#define  SCSI_hSIO204_ENTRY(hhcb)            SCSI_S160M_SIO204_ENTRY
#define  SCSI_hSIO215(hhcb)                  SCSI_S160M_SIO215
#define  SCSI_hFAST20_LOW(hhcb)              SCSI_S160M_FAST20_LOW
#define  SCSI_hFAST20_HIGH(hhcb)             SCSI_S160M_FAST20_HIGH
#define  SCSI_hMWI_CACHE_MASK(hhcb)          SCSI_S160M_MWI_CACHE_MASK
#define  SCSI_hENT_PT_BITMAP(hhcb)           SCSI_S160M_ENT_PT_BITMAP
#define  SCSI_hAUTO_RATE_ENABLE(hhcb)        SCSI_S160M_AUTO_RATE_ENABLE
#define  SCSI_hEXPANDER_BREAK(hhcb)          SCSI_S160M_EXPANDER_BREAK
#if SCSI_TARGET_OPERATION
#define  SCSI_hQ_EST_HEAD(hhcb)              SCSI_S160M_Q_EST_HEAD
#define  SCSI_hQ_EST_TAIL(hhcb)              SCSI_S160M_Q_EST_TAIL
#define  SCSI_hQ_EST_NEXT_SCB(hhcb)          SCSI_S160M_Q_EST_NEXT_SCB
#endif /* SCSI_TARGET_OPERATION */
#endif /* SCSI_STANDARD_160M_MODE */


#if SCSI_SWAPPING_160M_MODE
/***************************************************************************
* Macros for swapping ultra 160m mode exclusively
***************************************************************************/
#define  SCSI_hSIZEOFSCBBUFFER(mode)  SCSI_W160M_SIZE_SCB
#define  SCSI_hALIGNMENTSCBBUFFER(mode) SCSI_hSIZEOFSCBBUFFER((mode))-1
#define  SCSI_hSETUPFIRMWAREDESC(hhcb)
#define  SCSI_hSETUPSEQUENCER(hhcb) SCSIhSwapping160mSetupSequencer((hhcb))
#define  SCSI_hRESETSOFTWARE(hhcb)   SCSIhSwapping160mResetSoftware((hhcb))
#define  SCSI_hDELIVERSCB(hhcb,hiob)  SCSIhDeliverScb((hhcb),(hiob)) 
#define  SCSI_hPATCH_XFER_OPT(hhcb,hiob) SCSIhSwapping160mPatchXferOpt((hhcb),(hiob))
#define  SCSI_hENQUEHEADBDR(hhcb,hiob)  SCSIhSwapping160mQHeadBDR((hhcb),(hiob))
#define  SCSI_hRETRIEVESCB(hhcb)  SCSIhStandardRetrieveScb(hhcb)
#define  SCSI_hQOUTCNT(hhcb) SCSIhStandardQoutcnt(hhcb)
#define  SCSI_hTARGETCLEARBUSY(hhcb,hiob) SCSIhSwappingTargetClearBusy((hhcb),(hiob))
#define  SCSI_hSIZEQOUTPTRARRAY(hhcb) (256 * sizeof(QOUTFIFO_NEW))
#define  SCSI_hREQUESTSENSE(hhcb,hiob) SCSIhSwapping160mRequestSense((hhcb),(hiob))
#define  SCSI_hRESETBTA(hhcb) SCSIhSwappingResetBTA(hhcb)
#define  SCSI_hGETCONFIGURATION(hhcb) SCSIhNonStandardGetConfig((hhcb))
#define  SCSI_hCONFIGURESCBRAM(hhcb)
#define  SCSI_hSETUP_ASSIGN_SCB_BUFFER(hhcb) SCSIhSetupAssignScbBuffer((hhcb))
#if SCSI_RESOURCE_MANAGEMENT 
#define  SCSI_hASSIGN_SCB_BUFFER(hhcb,hiob)
#else
#define  SCSI_hASSIGN_SCB_BUFFER(hhcb,hiob) SCSIhSwappingAssignScbBuffer((hhcb),(hiob))
#endif /* SCSI_RESOURCE_MANAGEMENT */
/*#define  SCSI_hASSIGN_SCB_BUFFER(hhcb,hiob) SCSIhAssignScbBuffer((hhcb),(hiob))
*/
#define  SCSI_hCHKCONDXFERASSIGN(hhcb,i,xfer_option) \
            SCSIhChkCondXferAssign(hhcb,i,xfer_option)
#define  SCSI_hXFEROPTASSIGN(hhcb,i,xfer_option) \
            SCSIhHostXferOptAssign(hhcb,i,xfer_option)
#define  SCSI_hCOMMON_XFER_ASSIGN(hhcb,targetID,value) \
            SCSIhSwapping160mCommonXferAssign(hhcb,targetID,value)
#define  SCSI_hUPDATE_XFER_OPTION_HOST_MEM(hhcb,hiob,value) \
            SCSIhSwapping160mUpdateXferOptionHostMem(hhcb,hiob,value)
#define  SCSI_hGETXFEROPTION(hhcb,i) SCSIhHostGetOption(hhcb,i)

#define  SCSI_hGETFAST20REG(hhcb,targetID) SCSIhSwappingAdvGetFast20Reg(hhcb,targetID)
#define  SCSI_hCLEARFAST20REG(hhcb,targetID)
#define  SCSI_hLOGFAST20MAP(hhcb,targetID,period)
#define  SCSI_hUPDATE_FAST20_HOST_MEM(hhcb,hiob,flag)
#define  SCSI_hUPDATE_FAST20_HW(hhcb,flag)
#define  SCSI_hCLEAR_ALL_FAST20_REG(hhcb)
#define  SCSI_hMODIFYDATAPTR(hhcb,hiob) SCSIhSwapping160mModifyDataPtr(hhcb,hiob)
#define  SCSI_hACTIVEABORT(hhcb,hiob) \
            SCSIhSwapping160mActiveAbort(hhcb,hiob)
#define  SCSI_hABORTHIOB(hhcb,hiob,haStatus) \
            SCSIhSwapping160mAbortHIOB(hhcb,hiob,haStatus)
#define  SCSI_hSEARCHSEQDONEQ(hhcb,hiob,postHiob)
#define  SCSI_hSEARCHDONEQ(hhcb,hiob) \
            SCSIhStandardSearchDoneQ(hhcb,hiob)
#define  SCSI_hSEARCHNEWQ(hhcb,hiob,haStatus) \
            SCSIhSwappingSearchNewQ(hhcb,hiob,haStatus)
#define  SCSI_hSEARCHEXEQ(hhcb,hiob,haStatus,postHiob) \
            SCSIhSwappingSearchExeQ(hhcb,hiob,haStatus,postHiob)
#define  SCSI_hUPDATE_ABORT_BIT_HOST_MEM(hhcb,hiob) \
            SCSIhSwapping160mUpdateAbortBitHostMem(hhcb,hiob)

#define  SCSI_hOBTAIN_NEXT_SCB_NUM(hhcb,hiob) \
           SCSIhSwapping160mObtainNextScbNum(hhcb,hiob)
#define  SCSI_hUPDATE_NEXT_SCB_NUM(hhcb,hiob,scbNumber) \
            SCSIhSwapping160mUpdateNextScbNum(hhcb,hiob,scbNumber)
#define  SCSI_hOBTAIN_NEXT_SCB_ADDRESS(hhcb,hiob)
#define  SCSI_hUPDATE_NEXT_SCB_ADDRESS(hhcb,hiob,ScbBusAddress)

#define  SCSI_hRESETCCCTL(hhcb) SCSIhStandardResetCCCtl(hhcb)
#define  SCSI_hINITCCHHADDR(hhcb) SCSIhStandardInitCCHHAddr(hhcb)
#define  SCSI_hREMOVEACTIVEABORT(hhcb,hiob) \
            SCSIhSwappingRemoveActiveAbort(hhcb,hiob)

#define  SCSI_hSTACKBUGFIX(hhcb)

#define  SCSI_hSETUPTARGETRESET(hhcb,hiob) SCSIhSwapping160mSetupBDR(hhcb,hiob)
#define  SCSI_hRESIDUECALC(hhcb,hiob) SCSIhSwapping160mResidueCalc(hhcb,hiob)

#define  SCSI_hSETBREAKPOINT(hhcb,entryBitMap) \
            SCSIhSwapping160mSetBreakPoint(hhcb,entryBitMap)
#define  SCSI_hCLEARBREAKPOINT(hhcb,entryBitMap) \
            SCSIhSwapping160mClearBreakPoint(hhcb,entryBitMap)

#define  SCSI_hUPDATESGLENGTH(hhcb,regValue) \
            SCSIhSwapping160mUpdateSGLength(hhcb,regValue)
#define  SCSI_hUPDATESGADDRESS(hhcb,regValue) \
            SCSIhSwapping160mUpdateSGAddress(hhcb,regValue)
#define  SCSI_hFREEZEHWQUEUE(hhcb,hiob) \
            SCSIhSwapping160mFreezeHWQueue(hhcb,hiob)

#define  SCSI_hSETINTRTHRESHOLD(hhcb,threshold) \
            SCSIhSwapping160mSetIntrThreshold(hhcb,threshold)
#define  SCSI_hGETINTRTHRESHOLD(hhcb) \
            SCSIhSwapping160mGetIntrThreshold(hhcb)

#define  SCSI_hENABLEAUTORATEOPTION(hhcb) SCSIhEnableAutoRateOption(hhcb);
#define  SCSI_hDISABLEAUTORATEOPTION(hhcb,xferOption) \
            SCSIhDisableAutoRateOption(hhcb,xferOption);

#define  SCSI_hSETPHASEDELAYVALUES(hhcb) \
            SCSIhSwapping160mSetPhaseDelayValues(hhcb)

#if SCSI_TARGET_OPERATION 
#define  SCSI_hTARGETPATCHXFERRATE(hhcb,hiob)
/* This doesn't appear to be used */
#define  SCSI_hTARGETSETIGNOREWIDEMSG(hhcb,hiob)  \
            SCSIhTargetSwapping160mSetIgnoreWideMsg((hhcb),(hiob)) 
#define  SCSI_hTARGET_SEND_HIOB_SPECIAL(hhcb,hiob) \
            SCSIhTargetSwappingSendHiobSpecial((hhcb),(hiob))
#define  SCSI_hTARGETDELIVERESTSCB(hhcb,hiob) \
            SCSIhTargetSwapping160mDeliverEstScb((hhcb),(hiob))
#define  SCSI_hTARGETGETESTSCBFIELDS(hhcb,hiob,flag) \
            SCSIhTargetSwapping160mGetEstScbFields((hhcb),(hiob),(flag))
#endif   /* SCSI_TARGET_OPERATION */

#define  SCSI_hFIRMWARE_VERSION(hhcb)        SCSI_W160M_FIRMWARE_VERSION
#define  SCSI_hBTATABLE(hhcb)                SCSI_W160M_BTATABLE
#define  SCSI_hRESCNT_BASE(hhcb)             SCSI_W160M_RESCNT_BASE

#define  SCSI_hDISCON_OPTION(hhcb)           SCSI_W160M_DISCON_OPTION
#define  SCSI_hSIOSTR3_ENTRY(hhcb)           SCSI_W160M_SIOSTR3_ENTRY
#define  SCSI_hATN_TMR_ENTRY(hhcb)           SCSI_W160M_ATN_TMR_ENTRY
#define  SCSI_hTARG_LUN_MASK0(hhcb)          SCSI_W160M_TARG_LUN_MASK0
#define  SCSI_hQ_DONE_ELEMENT(hhcb)          SCSI_W160M_Q_DONE_ELEMENT
#define  SCSI_hPASS_TO_DRIVER(hhcb)          SCSI_W160M_PASS_TO_DRIVER
#define  SCSI_hSCSIDATL_IMAGE(hhcb)          SCSI_W160M_SCSIDATL_IMAGE
#define  SCSI_hSTART_LINK_CMD_ENTRY(hhcb)    SCSI_W160M_START_LINK_CMD_ENTRY
#define  SCSI_hIDLE_LOOP_ENTRY(hhcb)         SCSI_W160M_IDLE_LOOP_ENTRY   
#define  SCSI_hIDLE_LOOP0(hhcb)              SCSI_W160M_IDLE_LOOP0
#if SCSI_TARGET_OPERATION 
#define  SCSI_hTARGET_DATA_ENTRY(hhcb)       SCSI_W160M_TARGET_DATA_ENTRY
#define  SCSI_hTARGET_HELD_BUS_ENTRY(hhcb)   SCSI_W160M_TARGET_HELD_BUS_ENTRY
#endif /* SCSI_TARGET_OPERATION */
#define  SCSI_hACTIVE_SCB(hhcb)              SCSI_W160M_ACTIVE_SCB
#define  SCSI_hQ_EXE_HEAD(hhcb)              SCSI_W160M_Q_EXE_HEAD
#define  SCSI_hARRAY_PARTITION0(hhcb)        SCSI_W160M_ARRAY_PARTITION0
#define  SCSI_hQ_NEW_HEAD(hhcb)              SCSI_W160M_Q_NEW_HEAD
#define  SCSI_hQ_NEW_POINTER(hhcb)           SCSI_W160M_Q_NEW_BASE
#define  SCSI_hQ_NEW_TAIL(hhcb)              SCSI_W160M_Q_NEW_TAIL
#define  SCSI_hQ_NEXT_SCB(hhcb)              SCSI_W160M_Q_NEXT_SCB  
#define  SCSI_hQ_DONE_HEAD(hhcb)             SCSI_W160M_Q_DONE_HEAD
#define  SCSI_hWAITING_SCB(hhcb)             SCSI_W160M_WAITING_SCB
#define  SCSI_hQ_DONE_BASE(hhcb)             SCSI_W160M_Q_DONE_BASE
#define  SCSI_hSEQUENCER_PROGRAM(hhcb)       SCSI_W160M_SEQUENCER_PROGRAM
#define  SCSI_hQ_DONE_PASS(hhcb)             SCSI_W160M_Q_DONE_PASS
#define  SCSI_hXFER_OPTION(hhcb)             SCSI_W160M_XFER_OPTION
#define  SCSI_hSIO204_ENTRY(hhcb)            SCSI_W160M_SIO204_ENTRY
#define  SCSI_hSIO215(hhcb)                  SCSI_W160M_SIO215
#define  SCSI_hFAST20_LOW(hhcb)              SCSI_W160M_FAST20_LOW
#define  SCSI_hFAST20_HIGH(hhcb)             SCSI_W160M_FAST20_HIGH
#define  SCSI_hMWI_CACHE_MASK(hhcb)          SCSI_W160M_MWI_CACHE_MASK
#define  SCSI_hENT_PT_BITMAP(hhcb)           SCSI_W160M_ENT_PT_BITMAP
#define  SCSI_hAUTO_RATE_ENABLE(hhcb)        SCSI_W160M_AUTO_RATE_ENABLE
#define  SCSI_hEXPANDER_BREAK(hhcb)          SCSI_W160M_EXPANDER_BREAK
#if SCSI_TARGET_OPERATION
#define  SCSI_hQ_EST_HEAD(hhcb)              SCSI_W160M_Q_EST_HEAD
#define  SCSI_hQ_EST_TAIL(hhcb)              SCSI_W160M_Q_EST_TAIL
#define  SCSI_hQ_EST_NEXT_SCB(hhcb)          SCSI_W160M_Q_EST_NEXT_SCB
#endif /* SCSI_TARGET_OPERATION */
#endif /* SCSI_SWAPPING_160M_MODE */

#endif /* SCSI_MULTIPLEID_SUPPORT */

                                   
#if SCSI_GROUP_DEVICES
/***************************************************************************
* Macros exclusive for chainning support
***************************************************************************/
#define  SCSI_hNEXTHOSTINGROUP(hhcb)  ((hhcb))->groupNext;
#define  SCSI_hNEXTHIOBINGROUP(hiob)  ((hiob))->groupNext;

#else
/***************************************************************************
* Macros exclusive for no support chainning
***************************************************************************/
#define  SCSI_hNEXTHOSTINGROUP(hhcb)  SCSI_NULL_HHCB
#define  SCSI_hNEXTHIOBGROUP(hhcb)  SCSI_NULL_HIOB

#endif

#if SCSI_SCAM_ENABLE == 1
#define  SCSI_hPROTOCOLAUTOCONFIG(hhcb)   if ((hhcb)->SCSI_HF_scamLevel == 1)\
                                          {\
                                             SCSIhScamLevel1((hhcb));\
                                          }
#else
#if SCSI_SCAM_ENABLE == 2
#define  SCSI_hPROTOCOLAUTOCONFIG(hhcb)   if ((hhcb)->SCSI_HF_scamLevel == 1)\
                                          {\
                                             SCSIhScamLevel1((hhcb));\
                                          }\
                                          else if ((hhcb)->SCSI_HF_scamLevel == 2)\
                                          {\
                                             SCSIhScamLevel2((hhcb));\
                                          }
#else 
#define  SCSI_hPROTOCOLAUTOCONFIG(hhcb)
#endif
#endif

#if SCSI_SCAM_ENABLE
#define  SCSI_hRESETDELAY(hhcb,count)  if ((hhcb)->SCSI_HF_scamLevel != 1)\
                                       {\
                                          SCSIhDelayCount500us((hhcb),(count));\
                                       }
#else
#define  SCSI_hRESETDELAY(hhcb,count)  SCSIhDelayCount500us((hhcb),(count));
#endif

#if SCSI_LOOPBACK_OPERATION
#define  SCSI_hSETXFERRATE(hhcb,scsiID) SCSIhSetXferRate((hhcb),(scsiID))
#else
#define  SCSI_hSETXFERRATE(hhcb,scsiID)
#endif /* SCSI_LOOPBACK_OPERATION */

/***************************************************************************
* Macros dependent on disable/enable firmware
***************************************************************************/
/* originally -       hhcb->SCSI_HF_autoTermination = 1;       */
/* change the default to the same as non-SCSI_AUTO_TERMINATION */
/* to cover ARO and no SEEPROM application case                */
/* default -   autotermination disabled                        */
/*             high termination enabled                        */
/*             low termination enabled                         */
/*             secondary autotermination disabled              */
/*             secondary high termination enabled              */
/*             secondary low termination enabled               */
#define  SCSI_hSETDEFAULTAUTOTERM(hhcb)   \
            hhcb->SCSI_HF_terminationLow = hhcb->SCSI_HF_terminationHigh = 1; \
            hhcb->SCSI_HF_autoTermination = !(hhcb->SCSI_SUBID_autoTerm); \
            hhcb->SCSI_HF_secondaryTermLow = hhcb->SCSI_SUBID_legacyConnector; \
            hhcb->SCSI_HF_secondaryTermHigh = hhcb->SCSI_SUBID_legacyConnector; \
            hhcb->SCSI_HF_secondaryAutoTerm = (hhcb->SCSI_SUBID_legacyConnector && !(hhcb->SCSI_SUBID_autoTerm));

#if OSD_BUS_ADDRESS_SIZE == 32
#define  SCSI_hSETADDRESSSCRATCH(hhcb,scratchAddress,virtualAddress) \
              SCSIhSetAddressScratchBA32((hhcb),(scratchAddress),\
                           (void SCSI_HPTR) (virtualAddress))
#define  SCSI_hSETADDRESSSCB(hhcb,scbAddress,virtualAddress) \
              SCSIhSetAddressScbBA32((hhcb),(scbAddress),(virtualAddress))
#define  SCSI_hGETADDRESSSCB(hhcb,scbAddress) \
              SCSIhGetAddressScbBA32((hhcb),(scbAddress))
#define  SCSI_hPUTBUSADDRESS(hhcb,dmaStruct,offset,value) \
              SCSI_PUT_LITTLE_ENDIAN32((hhcb),(dmaStruct),(offset),(value))
#define  SCSI_hGETBUSADDRESS(hhcb,pValue,dmaStruct,offset) \
              SCSI_GET_LITTLE_ENDIAN32((hhcb),(pValue),(dmaStruct),(offset))
#else
#define  SCSI_hSETADDRESSSCRATCH(hhcb,scratchAddress,virtualAddress) \
              SCSIhSetAddressScratchBA64((hhcb),(scratchAddress),\
                           (void SCSI_HPTR) (virtualAddress))
#define  SCSI_hSETADDRESSSCB(hhcb,scbAddress,virtualAddress) \
              SCSIhSetAddressScbBA64((hhcb),(scbAddress),(virtualAddress))
#define  SCSI_hGETADDRESSSCB(hhcb,scbAddress) \
              SCSIhGetAddressScbBA64((hhcb),(scbAddress))
#define  SCSI_hPUTBUSADDRESS(hhcb,dmaStruct,offset,value) \
              SCSI_PUT_LITTLE_ENDIAN64((hhcb),(dmaStruct),(offset),(value))
#define  SCSI_hGETBUSADDRESS(hhcb,pValue,dmaStruct,offset) \
              SCSI_GET_LITTLE_ENDIAN64((hhcb),(pValue),(dmaStruct),(offset))
#endif

/***************************************************************************
* Macros dependent on IDs
***************************************************************************/
#if SCSI_AIC787X + SCSI_AIC785X
#define  SCSI_hWRITE_HCNTRL(hhcb,scsiRegister,value) SCSIhWriteHCNTRL((hhcb),(SCSI_UEXACT8)(value))
#define  SCSI_hWRITE_HCNTRL_FAST(hhcb,scsiRegister,value)\
               if ((hhcb)->SCSI_HP_wwWriteHCNTRL)\
               {\
                  SCSIhWWWriteHCNTRL((scsiRegister),(value));\
               }\
               else\
               {\
                  OSD_OUTEXACT8(SCSI_AICREG(SCSI_HCNTRL),(value));\
               }
#define  SCSI_hREAD_INTSTAT(hhcb,scsiRegister) SCSIhReadINTSTAT((hhcb))
#define  SCSI_hREAD_INTSTAT_FAST(hhcb,scsiRegister) \
               (((hhcb)->SCSI_HP_wwReadINTSTAT) ?\
                  SCSIhWWReadINTSTAT((scsiRegister)) :\
                  OSD_INEXACT8(SCSI_AICREG(SCSI_INTSTAT)))
#else
#define  SCSI_hWRITE_HCNTRL(hhcb,scsiRegister,value) OSD_OUTEXACT8(SCSI_AICREG(SCSI_HCNTRL),(value))
#define  SCSI_hWRITE_HCNTRL_FAST(hhcb,scsiRegister,value) OSD_OUTEXACT8(SCSI_AICREG(SCSI_HCNTRL),(value))
#define  SCSI_hREAD_INTSTAT(hhcb,scsiRegister) OSD_INEXACT8(SCSI_AICREG(SCSI_INTSTAT))
#define  SCSI_hREAD_INTSTAT_FAST(hhcb,scsiRegister) OSD_INEXACT8(SCSI_AICREG(SCSI_INTSTAT))
#endif

#if SCSI_MULTI_HARDWARE
#define  SCSI_hSETUPHARDWAREDESC(hhcb) (hhcb)->SCSI_HP_hardwareDescriptor = \
                                    SCSIHardware[(hhcb)->hardwareMode]
#define  SCSI_hLOADSCSIRATE(hhcb,targetID,value) \
            (hhcb)->SCSI_HP_hardwareDescriptor->SCSIhLoadScsiRate((hhcb),(targetID),(value))
#define  SCSI_hRESETDFIFO(hhcb) \
            (hhcb)->SCSI_HP_hardwareDescriptor->SCSIhResetDFifo((hhcb))
#define  SCSI_hENIOERR(hhcb) \
            (hhcb)->SCSI_HP_hardwareDescriptor->SCSIhEnableIOErr((hhcb))
#define  SCSI_hGETWIDESCSIOFFSET(hhcb,hiob) \
            (hhcb)->SCSI_HP_hardwareDescriptor->SCSIhGetWideScsiOffset((hiob))
#define  SCSI_hGETNARROWSCSIOFFSET(hhcb,hiob) \
            (hhcb)->SCSI_HP_hardwareDescriptor->SCSIhGetNarrowScsiOffset((hiob))
#define  SCSI_hGETXFERPERIOD(hhcb,hiob) \
            (hhcb)->SCSI_HP_hardwareDescriptor->SCSIhGetXferPeriod((hhcb),(hiob))
#define  SCSI_hCALCSCSIRATE(hhcb,targetID,period,offset) \
            (hhcb)->SCSI_HP_hardwareDescriptor->SCSIhCalcScsiRate((hhcb),(targetID),(period),(offset))
#define  SCSI_hCLEARSCSIOFFSET(hhcb,hiob) \
            (hhcb)->SCSI_HP_hardwareDescriptor->SCSIhClearScsiOffset((hiob))
#define  SCSI_hWASSYNCXFER(hhcb,offset) \
            (hhcb)->SCSI_HP_hardwareDescriptor->SCSIhWasSyncXfer((offset))
#define  SCSI_hSCSIID(hhcb) \
            (hhcb)->SCSI_HP_hardwareDescriptor->ScsiID
#define  SCSI_hPROCESSAUTOTERM(hhcb) \
            (hhcb)->SCSI_HP_hardwareDescriptor->SCSIhProcessAutoTerm((hhcb))
#define  SCSI_hUPDATEEXTTERM(hhcb) \
            (hhcb)->SCSI_HP_hardwareDescriptor->SCSIhUpdateExtTerm((hhcb))
#if SCSI_CURRENT_SENSING
#define SCSI_hGETCURRENTSENSING(hhcb, channel_select) \
            (hhcb)->SCSI_HP_hardwareDescriptor->SCSIhGetCurrentSensing((hhcb), (channel_select))
#endif
#define  SCSI_hGETSCBRAMCTRLS(hhcb) \
            (hhcb)->SCSI_HP_hardwareDescriptor->SCSIhGetScbRamCtrls((hhcb))
#define  SCSI_hPUTSCBRAMCTRLS(hhcb, value) \
            (hhcb)->SCSI_HP_hardwareDescriptor->SCSIhPutScbRamCtrls((hhcb),(value))
#define  SCSI_hSETDATAFIFOTHRSHDEFAULT(hhcb) \
            (hhcb)->SCSI_HP_hardwareDescriptor->SCSIhSetDataFifoThrshDefault((hhcb))
#define  SCSI_hREADDATAFIFOTHRSH(hhcb) \
            (hhcb)->SCSI_HP_hardwareDescriptor->SCSIhReadDataFifoThrsh((hhcb))
#define  SCSI_hUPDATEDATAFIFOTHRSH(hhcb) \
            (hhcb)->SCSI_HP_hardwareDescriptor->SCSIhUpdateDataFifoThrsh((hhcb))
#define  SCSI_hGETCACHETHEN(hhcb) \
            (hhcb)->SCSI_HP_hardwareDescriptor->SCSIhGetCacheThEn((hhcb))
#define  SCSI_hUPDATECACHETHEN(hhcb) \
            (hhcb)->SCSI_HP_hardwareDescriptor->SCSIhUpdateCacheThEn((hhcb))
#define  SCSI_hGETPROFILEPARAMETERS(hwInfo,hhcb) \
            (hhcb)->SCSI_HP_hardwareDescriptor->SCSIhGetProfileParameters((hwInfo), (hhcb))
#define  SCSI_hPUTPROFILEPARAMETERS(hwInfo,hhcb) \
            (hhcb)->SCSI_HP_hardwareDescriptor->SCSIhPutProfileParameters((hwInfo), (hhcb))
#define  SCSI_hCHECKEXTARBACKENBIT(hhcb)\
            (hhcb)->SCSI_HP_hardwareDescriptor->SCSIhCheckExtArbAckEnBit((hhcb))
#define  SCSI_hGETQNEWTAIL(hhcb)\
            (hhcb)->SCSI_HP_hardwareDescriptor->SCSIhGetQNewTail((hhcb))
#define  SCSI_hWAITFORBUSSTABLE(hhcb)\
            if ((hhcb)->SCSI_HP_hardwareDescriptor->SCSIhWaitForBusStable)\
            {\
               (hhcb)->SCSI_HP_hardwareDescriptor->SCSIhWaitForBusStable((hhcb));\
            }
#define  SCSI_hSEQSTRETCHWORKAROUND(hhcb)\
            if ((hhcb)->SCSI_HP_hardwareDescriptor->SCSIhSeqStretchWorkaround)\
            {\
               (hhcb)->SCSI_HP_hardwareDescriptor->SCSIhSeqStretchWorkaround((hhcb));\
            }
#define  SCSI_hIGNOREWIDERESCALC(hhcb,hiob,ignoreXfer)\
            (hhcb)->SCSI_HP_hardwareDescriptor->SCSIhIgnoreWideResCalc((hhcb),(hiob),(ignoreXfer));
#define  SCSI_hLOADTARGETIDS(hhcb) \
            if ((hhcb)->SCSI_HP_hardwareDescriptor->SCSIhTargetLoadTargetIds)\
            {\
               (hhcb)->SCSI_HP_hardwareDescriptor->SCSIhTargetLoadTargetIds((hhcb));\
            }            
#define  SCSI_hRESETSCSIRATE(hhcb) \
            (hhcb)->SCSI_HP_hardwareDescriptor->SCSIhResetScsiRate((hhcb))
#define  SCSI_hCLEARSCSIRATE(hhcb) \
            (hhcb)->SCSI_HP_hardwareDescriptor->SCSIhClearScsiRate((hhcb))
#define  SCSI_hMAXSYNCPERIOD(hhcb,targetID) \
            (hhcb)->SCSI_HP_hardwareDescriptor->SCSIhMaxSyncPeriod((hhcb),(targetID))
#if SCSI_NEGOTIATION_PER_IOB
#define  SCSI_hCHANGEXFEROPTION(hiob)\
            (hhcb)->SCSI_HP_hardwareDescriptor->SCSIhChangeXferOption((hiob));
#endif
#define  SCSI_hRAMPSM(hhcb) \
            (hhcb)->SCSI_HP_hardwareDescriptor->RAMPSM
#define  SCSI_hSCBRAMSEL(hhcb) \
            (hhcb)->SCSI_HP_hardwareDescriptor->SCBRAMSEL
#define  SCSI_hSCBSIZE32(hhcb) \
            (hhcb)->SCSI_HP_hardwareDescriptor->SCBSIZE32
#define  SCSI_hSEQCODE_SIZE(hhcb) \
            (hhcb)->SCSI_HP_hardwareDescriptor->SeqCodeSize
#define  SCSI_hCACHETHENDEFAULT(hhcb) \
            (hhcb)->SCSI_HP_hardwareDescriptor->defaultCacheThEn
#define  SCSI_hEXPACTIVEDEFAULT(hhcb) \
            (hhcb)->SCSI_HP_hardwareDescriptor->defaultExpActive
#define  SCSI_hDELAYFACTOR(hhcb) \
            (hhcb)->SCSI_HP_hardwareDescriptor->delayFactor

#else /* !SCSI_MULTI_HARDWARE */

#if SCSI_AICTRIDENT
#define  SCSI_hSETUPHARDWAREDESC(hhcb)
#define  SCSI_hLOADSCSIRATE(hhcb,targetID,value) SCSIhAICTridentLoadScsiRate((hhcb),(targetID),(value))
#define  SCSI_hRESETDFIFO(hhcb) SCSIhAICBayonetResetDFifo((hhcb))
#define  SCSI_hENIOERR(hhcb) SCSIhAICBayonetEnableIOErr((hhcb))
#define  SCSI_hGETWIDESCSIOFFSET(hhcb,hiob) SCSIhAICBayonetGetScsiOffset((hiob))
#define  SCSI_hGETNARROWSCSIOFFSET(hhcb,hiob) SCSIhAICBayonetGetScsiOffset((hiob))
#define  SCSI_hGETXFERPERIOD(hhcb,hiob) SCSIhAICTridentGetXferPeriod((hhcb),(hiob))
#define  SCSI_hCALCSCSIRATE(hhcb,targetID,period,offset) SCSIhAICTridentCalcScsiRate((hhcb),(targetID),(period),(offset))
#define  SCSI_hCLEARSCSIOFFSET(hhcb,hiob) SCSIhAICBayonetClearScsiOffset((hiob))
#define  SCSI_hWASSYNCXFER(hhcb,offset) SCSIhAICBayonetWasSyncXfer((offset))
#define  SCSI_hSCSIID(hhcb) SCSI_BAYONET_SCSIID
#if SCSI_AUTO_TERMINATION
#define  SCSI_hPROCESSAUTOTERM(hhcb) SCSIhAICBayonetProcessAutoTerm((hhcb))
#else
#define  SCSI_hPROCESSAUTOTERM(hhcb)
#endif
#define  SCSI_hUPDATEEXTTERM(hhcb) SCSIhAICBayonetUpdateExtTerm((hhcb))
#if SCSI_CURRENT_SENSING
#define  SCSI_hGETCURRENTSENSING(hhcb, channel_select) SCSIhGetCurrentSensing160((hhcb), (channel_select))
#endif
#define  SCSI_hGETSCBRAMCTRLS(hhcb) SCSIhAICBayonetGetScbRamCtrls((hhcb))
#define  SCSI_hPUTSCBRAMCTRLS(hhcb,value) SCSIhAICBayonetPutScbRamCtrls((hhcb),(value))
#define  SCSI_hSETDATAFIFOTHRSHDEFAULT(hhcb) SCSIhAICBayonetSetDataFifoThrshDefault((hhcb))  
#define  SCSI_hREADDATAFIFOTHRSH(hhcb) SCSIhAICBayonetReadDataFifoThrsh ((hhcb))
#define  SCSI_hUPDATEDATAFIFOTHRSH(hhcb) SCSIhAICBayonetUpdateDataFifoThrsh ((hhcb))
#define  SCSI_hGETCACHETHEN(hhcb) SCSIhGetCacheThEn((hhcb))
#define  SCSI_hUPDATECACHETHEN(hhcb) SCSIhAICTridentUpdateCacheThEn((hhcb))
#define  SCSI_hGETPROFILEPARAMETERS(hwInfo,hhcb) SCSIhAICTridentGetProfileParameters((hwInfo),(hhcb))
#define  SCSI_hPUTPROFILEPARAMETERS(hwInfo,hhcb) SCSIhAICTridentPutProfileParameters((hwInfo),(hhcb))
#define  SCSI_hCHECKEXTARBACKENBIT(hhcb) SCSIhAICBayonetCheckExtArbAckEnBit((hhcb))
#define  SCSI_hGETQNEWTAIL(hhcb) SCSIhAICBayonetGetQNewTail((hhcb))
#define  SCSI_hWAITFORBUSSTABLE(hhcb) SCSIhAICBayonetWaitForBusStable((hhcb))
#define  SCSI_hSEQSTRETCHWORKAROUND(hhcb)
#define  SCSI_hIGNOREWIDERESCALC(hhcb,hiob,ignoreXfer) SCSIhAICBayonetIgnoreWideResCalc((hhcb),(hiob),(ignoreXfer))
#define  SCSI_hLOADTARGETIDS(hhcb)
#define  SCSI_hRESETSCSIRATE(hhcb) SCSIhAICTridentResetScsiRate((hhcb))
#define  SCSI_hCLEARSCSIRATE(hhcb) SCSIhAICTridentClearScsiRate((hhcb))
#define  SCSI_hMAXSYNCPERIOD(hhcb,targetID) SCSIhAICTridentMaxSyncPeriod((hhcb),(targetID))
#if SCSI_NEGOTIATION_PER_IOB
#define  SCSI_hCHANGEXFEROPTION(hiob) SCSIhAICTridentChangeXferOption((hiob))
#endif
#define  SCSI_hRAMPSM(hhcb) SCSI_BAYONET_RAMPSM
#define  SCSI_hSCBRAMSEL(hhcb) SCSI_BAYONET_INTSCBRAMSEL
#define  SCSI_hSCBSIZE32(hhcb) SCSI_BAYONET_SCBSIZE32
#define  SCSI_hCACHETHENDEFAULT(hhcb) SCSI_BAYONET_CACHETHEEN_DEFAULT
#define  SCSI_hEXPACTIVEDEFAULT(hhcb) SCSI_BAYONET_EXPACTIVE_DEFAULT
#if (SCSI_STANDARD_160M_MODE || SCSI_SWAPPING_160M_MODE)
#define  SCSI_hSEQCODE_SIZE(hhcb) SCSI_TRIDENT_SEQCODE_SIZE
#else
#define  SCSI_hSEQCODE_SIZE(hhcb) SCSI_BAYONET_SEQCODE_SIZE
#endif
#define  SCSI_hDELAYFACTOR(hhcb) SCSI_DELAY_FACTOR_20MIPS
#endif /* SCSI_AICTRIDENT */

#if SCSI_AICBAYONET
#define  SCSI_hSETUPHARDWAREDESC(hhcb)
#define  SCSI_hLOADSCSIRATE(hhcb,targetID,value) SCSIhAICBayonetLoadScsiRate((hhcb),(targetID),(value))
#define  SCSI_hRESETDFIFO(hhcb) SCSIhAICBayonetResetDFifo((hhcb))
#define  SCSI_hENIOERR(hhcb) SCSIhAICBayonetEnableIOErr((hhcb))
#define  SCSI_hGETWIDESCSIOFFSET(hhcb,hiob) SCSIhAICBayonetGetScsiOffset((hiob))
#define  SCSI_hGETNARROWSCSIOFFSET(hhcb,hiob) SCSIhAICBayonetGetScsiOffset((hiob))
#define  SCSI_hGETXFERPERIOD(hhcb,hiob) SCSIhAICBayonetGetXferPeriod((hhcb),(hiob))
#define  SCSI_hCALCSCSIRATE(hhcb,targetID,period,offset) SCSIhAICBayonetCalcScsiRate((hhcb),(targetID),(period),(offset))
#define  SCSI_hCLEARSCSIOFFSET(hhcb,hiob) SCSIhAICBayonetClearScsiOffset((hiob))
#define  SCSI_hWASSYNCXFER(hhcb,offset) SCSIhAICBayonetWasSyncXfer((offset))
#define  SCSI_hSCSIID(hhcb) SCSI_BAYONET_SCSIID
#define  SCSI_hPROCESSAUTOTERM(hhcb) SCSIhAICBayonetProcessAutoTerm((hhcb))
#define  SCSI_hUPDATEEXTTERM(hhcb) SCSIhAICBayonetUpdateExtTerm((hhcb))
#if SCSI_CURRENT_SENSING
#define  SCSI_hGETCURRENTSENSING(hhcb, channel_select)
#endif
#define  SCSI_hGETSCBRAMCTRLS(hhcb) SCSIhAICBayonetGetScbRamCtrls((hhcb))
#define  SCSI_hPUTSCBRAMCTRLS(hhcb,value) SCSIhAICBayonetPutScbRamCtrls((hhcb),(value))
#define  SCSI_hSETDATAFIFOTHRSHDEFAULT(hhcb) SCSIhAICBayonetSetDataFifoThrshDefault((hhcb))  
#define  SCSI_hREADDATAFIFOTHRSH(hhcb) SCSIhAICBayonetReadDataFifoThrsh ((hhcb))
#define  SCSI_hUPDATEDATAFIFOTHRSH(hhcb) SCSIhAICBayonetUpdateDataFifoThrsh ((hhcb))
#define  SCSI_hGETCACHETHEN(hhcb) SCSIhGetCacheThEn((hhcb))
#define  SCSI_hUPDATECACHETHEN(hhcb) SCSIhAICBayonetUpdateCacheThEn((hhcb))
#define  SCSI_hGETPROFILEPARAMETERS(hwInfo,hhcb) SCSIhAICBayonetGetProfileParameters((hwInfo),(hhcb))
#define  SCSI_hPUTPROFILEPARAMETERS(hwInfo,hhcb) SCSIhAICBayonetPutProfileParameters((hwInfo),(hhcb))
#define  SCSI_hCHECKEXTARBACKENBIT(hhcb) SCSIhAICBayonetCheckExtArbAckEnBit((hhcb))
#define  SCSI_hGETQNEWTAIL(hhcb) SCSIhAICBayonetGetQNewTail((hhcb))
#define  SCSI_hWAITFORBUSSTABLE(hhcb) SCSIhAICBayonetWaitForBusStable((hhcb))
#define  SCSI_hSEQSTRETCHWORKAROUND(hhcb) SCSIhAICBayonetSeqStretchWorkaround((hhcb))
#define  SCSI_hIGNOREWIDERESCALC(hhcb,hiob,ignoreXfer) SCSIhAICBayonetIgnoreWideResCalc((hhcb),(hiob),(ignoreXfer))
#if SCSI_TARGET_OPERATION && SCSI_MULTIPLEID_SUPPORT
#define  SCSI_hLOADTARGETIDS(hhcb) SCSIhTargetAICBayonetLoadTargetIDs((hhcb))
#else
#define  SCSI_hLOADTARGETIDS(hhcb)
#endif /* SCSI_TARGET_OPERATION && SCSI_MULTIPLEID_SUPPORT */
#define  SCSI_hRESETSCSIRATE(hhcb) SCSIhAICBayonetResetScsiRate((hhcb))
#define  SCSI_hCLEARSCSIRATE(hhcb) SCSIhAICBayonetResetScsiRate((hhcb))
#define  SCSI_hMAXSYNCPERIOD(hhcb,targetID) SCSIhAICBayonetMaxSyncPeriod((hhcb),(targetID))
#if SCSI_NEGOTIATION_PER_IOB
#define  SCSI_hCHANGEXFEROPTION(hiob) SCSIhAICBayonetChangeXferOption((hiob))
#endif
#define  SCSI_hRAMPSM(hhcb) SCSI_BAYONET_RAMPSM
#define  SCSI_hSCBRAMSEL(hhcb) SCSI_BAYONET_INTSCBRAMSEL
#define  SCSI_hSCBSIZE32(hhcb) SCSI_BAYONET_SCBSIZE32
#define  SCSI_hCACHETHENDEFAULT(hhcb) SCSI_BAYONET_CACHETHEEN_DEFAULT
#define  SCSI_hEXPACTIVEDEFAULT(hhcb) SCSI_BAYONET_EXPACTIVE_DEFAULT
#define  SCSI_hSEQCODE_SIZE(hhcb) SCSI_BAYONET_SEQCODE_SIZE
#define  SCSI_hDELAYFACTOR(hhcb) SCSI_DELAY_FACTOR_20MIPS
#endif /* SCSI_AICBAYONET */

#if SCSI_AIC78XX
#define  SCSI_hSETUPHARDWAREDESC(hhcb)
#define  SCSI_hLOADSCSIRATE(hhcb,targetID,value) SCSIhAIC78XXLoadScsiRate((hhcb),(targetID),(value))
#define  SCSI_hRESETDFIFO(hhcb) SCSIhAIC78XXResetDFifo((hhcb))
#define  SCSI_hENIOERR(hhcb) SCSIhAIC78XXEnableIOErr((hhcb))
#define  SCSI_hGETWIDESCSIOFFSET(hhcb,hiob) SCSIhAIC78XXGetWideScsiOffset((hiob))
#define  SCSI_hGETNARROWSCSIOFFSET(hhcb,hiob) SCSIhAIC78XXGetNarrowScsiOffset((hiob))
#define  SCSI_hGETXFERPERIOD(hhcb,hiob) SCSIhAIC78XXGetXferPeriod((hhcb),(hiob))
#define  SCSI_hCALCSCSIRATE(hhcb,targetID,period,offset) SCSIhAIC78XXCalcScsiRate((hhcb),(targetID),(period),(offset))
#define  SCSI_hCLEARSCSIOFFSET(hhcb,hiob) SCSIhAIC78XXClearScsiOffset((hiob))
#define  SCSI_hWASSYNCXFER(hhcb,offset) SCSIhAIC78XXWasSyncXfer((offset))
#define  SCSI_hSCSIID(hhcb) SCSI_SCSIID
#define  SCSI_hPROCESSAUTOTERM(hhcb) SCSIhAIC78XXProcessAutoTerm((hhcb))
#define  SCSI_hUPDATEEXTTERM(hhcb) SCSIhAIC78XXUpdateExtTerm((hhcb))
#if SCSI_CURRENT_SENSING
#define SCSI_hGETCURRENTSENSING(hhcb, channel_select) SCSIhGetCurrentSensing((hhcb), (channel_select))
#endif
#define  SCSI_hGETSCBRAMCTRLS(hhcb) SCSIhAIC78XXGetScbRamCtrls((hhcb))
#define  SCSI_hPUTSCBRAMCTRLS(hhcb,value) SCSIhAIC78XXPutScbRamCtrls((hhcb),(value))
#define  SCSI_hSETDATAFIFOTHRSHDEFAULT(hhcb) SCSIhAIC78XXSetDataFifoThrshDefault((hhcb))
#define  SCSI_hREADDATAFIFOTHRSH(hhcb) SCSIhAIC78XXReadDataFifoThrsh ((hhcb))
#define  SCSI_hUPDATEDATAFIFOTHRSH(hhcb) SCSIhAIC78XXUpdateDataFifoThrsh ((hhcb))
#define  SCSI_hGETCACHETHEN(hhcb) SCSIhGetCacheThEn((hhcb))
#define  SCSI_hUPDATECACHETHEN(hhcb) SCSIhAIC78XXUpdateCacheThEn((hhcb))
#define  SCSI_hGETPROFILEPARAMETERS(hwInfo,hhcb) SCSIhAIC78XXGetProfileParameters((hwInfo),(hhcb))
#define  SCSI_hPUTPROFILEPARAMETERS(hwInfo,hhcb) SCSIhAIC78XXPutProfileParameters((hwInfo),(hhcb))
#define  SCSI_hCHECKEXTARBACKENBIT(hhcb) SCSIhAIC78XXCheckExtArbAckEnBit((hhcb))
#define  SCSI_hGETQNEWTAIL(hhcb) SCSIhAIC78XXGetQNewTail((hhcb))
#define  SCSI_hWAITFORBUSSTABLE(hhcb)
#define  SCSI_hSEQSTRETCHWORKAROUND(hhcb)
#define  SCSI_hIGNOREWIDERESCALC(hhcb,hiob,ignoreXfer) SCSIhAIC78XXIgnoreWideResCalc((hhcb),(hiob),(ignoreXfer))
#define  SCSI_hLOADTARGETIDS(hhcb)
#define  SCSI_hRESETSCSIRATE(hhcb) SCSIhAIC78XXResetScsiRate((hhcb))
#define  SCSI_hCLEARSCSIRATE(hhcb) SCSIhAIC78XXResetScsiRate((hhcb))
#define  SCSI_hMAXSYNCPERIOD(hhcb,targetID) SCSIhAIC78XXMaxSyncPeriod((hhcb),(targetID))
#if SCSI_NEGOTIATION_PER_IOB
#define  SCSI_hCHANGEXFEROPTION(hiob) SCSIhAIC78XXChangeXferOption((hiob))
#endif
#define  SCSI_hRAMPSM(hhcb) SCSI_RAMPSM
#define  SCSI_hSCBRAMSEL(hhcb) SCSI_SCBRAMSEL
#define  SCSI_hCACHETHENDEFAULT(hhcb) SCSI_AIC78XX_CACHETHEEN_DEFAULT
#define  SCSI_hEXPACTIVEDEFAULT(hhcb) SCSI_AIC78XX_EXPACTIVE_DEFAULT
#define  SCSI_hSCBSIZE32(hhcb) SCSI_SCBSIZE32
#define  SCSI_hSEQCODE_SIZE(hhcb) SCSI_AIC78XX_SEQCODE_SIZE
#define  SCSI_hDELAYFACTOR(hhcb) SCSI_DELAY_FACTOR_10MIPS
#endif /* SCSI_AIC78XX */

/* for future expansion */
#if SCSI_AIC785X
#endif
#if SCSI_AIC787X
#endif
#if SCSI_AIC788X
#endif
#if SCSI_AIC789X
#endif
#endif /* SCSI_MULTI_HARDWARE */

#if SCSI_RESOURCE_MANAGEMENT
#define  SCSI_hFRESH_HIOB(hiob)
#if SCSI_TARGET_OPERATION
#define  SCSI_hTARGETMODE_HIOB(hiob)
#endif /* SCSI_TARGET_OPERATION */
#else
#if SCSI_TARGET_OPERATION
#define  SCSI_hTARGETMODE_HIOB(hiob)   (hiob)->SCSI_IP_targetMode = 1;

#define  SCSI_hFRESH_HIOB(hiob)  (hiob)->stat = 0;\
                                 (hiob)->haStat = 0;\
                                 (hiob)->trgStatus = 0;\
                                 (hiob)->SCSI_IP_targetMode = 0; \
                                 (hiob)->SCSI_IP_mgrStat = 0; \
                                 (hiob)->SCSI_IP_negoState = 0; 
#else
#define  SCSI_hFRESH_HIOB(hiob)  (hiob)->stat = 0;\
                                 (hiob)->haStat = 0;\
                                 (hiob)->trgStatus = 0;\
                                 (hiob)->SCSI_IP_mgrStat = 0;\
                                 (hiob)->SCSI_IP_negoState = 0;
#endif /* SCSI_TARGET_OPERATION */                                 

#endif /* SCSI_RESOURCE_MANAGEMENT */

/***************************************************************************
* Macros dependent on SCSI_TARGET_OPERATION
***************************************************************************/
#if SCSI_TARGET_OPERATION
#if SCSI_MULTIPLEID_SUPPORT
#define SCSI_hTARGETMODEENABLE(hhcb)\
           if (((hhcb)->SCSI_HF_targetMode) &&\
               ((hhcb)->SCSI_HF_targetAdapterIDMask != 0))\
           {\
              SCSIhTargetModeEnable((hhcb));\
           } 
#else
#define  SCSI_hTARGETMODEENABLE(hhcb)\
            if ((hhcb)->SCSI_HF_targetMode)\
            {\
               SCSIhTargetModeEnable((hhcb));\
            } 
#endif /* SCSI_MULTIPLEID_SUPPORT */

/* RSM allowed to issue Establish Connection HIOBs */
#define  SCSI_hTARGETHOLDESTHIOBS(hhcb,value)\
            if ((hhcb)->SCSI_HF_targetMode)\
            {\
               (hhcb)->SCSI_HF_targetHoldEstHiobs = (value);\
            }
#define  SCSI_hGETTARGETIDMASK(hwinfo,hhcb) \
            if ((hhcb)->SCSI_HF_targetMode)\
            {\
               (hwInfo)->targetIDMask = (hhcb)->SCSI_HF_targetAdapterIDMask;\
            }
#define  SCSI_hNEGOTIATE(hhcb,hiob)\
            if ((hiob) != SCSI_NULL_HIOB && (hiob)->SCSI_IP_targetMode)\
            {\
               SCSIhTargetNegotiate((hhcb),(hiob),1);\
            }\
            else\
            {\
               SCSIhNegotiate((hhcb),(hiob));\
            }
#define  SCSI_hTERMINATECOMMAND(hiob) \
            if ((hiob)->SCSI_IP_targetMode)\
            {\
               SCSIhTerminateTargetCommand((hiob));\
            }\
            else\
            {\
               SCSIhTerminateCommand((hiob));\
            }
#define  SCSI_hSETNEGOTIATEDSTATE(hhcb,i,value) \
            if (hhcb->SCSI_HF_targetMode)\
            {\
               SCSIhTargetSetNegotiatedState((hhcb),(i),(value));\
            }
#define  SCSI_hGETDEVICETABLEPTR(hiob,deviceTable) \
            if ((hiob)->SCSI_IP_targetMode)\
            {\
               deviceTable = SCSI_GET_NODE(SCSI_NEXUS_UNIT(hiob))->deviceTable;\
            }\
            else\
            {\
               deviceTable = SCSI_TARGET_UNIT(hiob)->deviceTable;\
            }

#define SCSI_hTARGETGETCONFIG(hhcb) \
            if (hhcb->SCSI_HF_targetMode)\
            {\
               SCSIhTargetGetConfiguration((hhcb));\
            }            

#if SCSI_MULTIPLEID_SUPPORT
#define  SCSI_hTARGETDISABLEID(hhcb) \
            if ((hhcb)->SCSI_HF_targetMode)\
            {\
               SCSIhTargetDisableId((hhcb));\
            }
#else
#define  SCSI_hTARGETDISABLEID(hhcb)

#endif /* SCSI_MULTIPLEID_SUPPORT */
#else
#define  SCSI_hTARGETMODEENABLE(hhcb)
#define  SCSI_hTARGETHOLDESTHIOBS(hhcb,value)
#define  SCSI_hTARGETDISABLEID(hhcb)
#define  SCSI_hGETTARGETIDMASK(hwinfo,hhcb)
#define  SCSI_hNEGOTIATE(hhcb,hiob)     SCSIhNegotiate((hhcb),(hiob))
#define  SCSI_hTERMINATECOMMAND(hiob)   SCSIhTerminateCommand((hiob))
#define  SCSI_hSETNEGOTIATEDSTATE(hhcb,i,value)
#define  SCSI_hGETDEVICETABLEPTR(hiob,deviceTable)\
            deviceTable = SCSI_TARGET_UNIT((hiob))->deviceTable
#define SCSI_hTARGETGETCONFIG(hhcb)
#endif /* SCSI_TARGET_OPERATION */

/***************************************************************************
* Function prototypes for Common HIM internal reference
***************************************************************************/
/* HWMTASK.C */
void SCSIhAbortTask(SCSI_HIOB SCSI_IPTR);
void SCSIhAbortTaskSet(SCSI_HIOB SCSI_IPTR);

/* HWMDLVR.C */
void SCSIhDeliverScb(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwapping32DeliverScb(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwapping64DeliverScb(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); 
void SCSIhSwapping32PatchXferOpt(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); 
void SCSIhSwapping64PatchXferOpt(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); 
void SCSIhStandardAdvPatchXferOpt(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwappingAdvPatchXferOpt(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhStandard160mPatchXferOpt(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwapping160mPatchXferOpt(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhStandard64QHead(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwapping64QHead(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); 
void SCSIhSwapping32QHead(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); 
void SCSIhSwappingAdvQHead(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); 
void SCSIhStandardAdvQHead(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwapping160mQHead(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); 
void SCSIhStandard160mQHead(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhStandard64QHeadBDR(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwapping64QHeadBDR(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); 
void SCSIhSwapping32QHeadBDR(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); 
void SCSIhSwappingAdvQHeadBDR(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); 
void SCSIhStandardAdvQHeadBDR(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwapping160mQHeadBDR(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); 
void SCSIhStandard160mQHeadBDR(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIhStandardRetrieveScb(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8 SCSIhStandardQoutcnt(SCSI_HHCB SCSI_HPTR);
               
/* HWMDIAG.C */

/* HWMUTIL.C */
void SCSIhPauseAndWait(SCSI_HHCB SCSI_HPTR);
void SCSIhUnPause(SCSI_HHCB SCSI_HPTR);
int SCSIhLoadSequencer(SCSI_REGISTER,SCSI_UEXACT8 SCSI_LPTR,int);
SCSI_UEXACT8 SCSIhWait4Req(SCSI_HHCB SCSI_HPTR);
void SCSIhDelayCount500us(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT32);
void SCSIhSetupAssignScbBuffer(SCSI_HHCB SCSI_HPTR);
void SCSIhAssignScbBuffer(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIhAIC78XXAutoTermCable(SCSI_HHCB SCSI_HPTR);
#if SCSI_CURRENT_SENSING
SCSI_UEXACT8 SCSIhGetCurrentSensing(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhGetCurrentSensing160(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
#endif
SCSI_UEXACT8 SCSIhCableSense(SCSI_HHCB SCSI_HPTR);
void SCSIhWriteHCNTRL(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIhWWWriteHCNTRL(SCSI_REGISTER,SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhReadINTSTAT(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8 SCSIhWWReadINTSTAT(SCSI_REGISTER);
void SCSIhSetAddressScbBA32(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,void SCSI_HPTR);
void SCSIhSetAddressScbBA64(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,void SCSI_HPTR);
SCSI_BUS_ADDRESS SCSIhGetAddressScbBA32(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
SCSI_UEXACT32 SCSIhGetEntity32FromScb(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8); 
SCSI_BUS_ADDRESS SCSIhGetAddressScbBA64(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
SCSI_UINT8 SCSIhCompareBusAddress(SCSI_BUS_ADDRESS,SCSI_BUS_ADDRESS); 
void SCSIhAIC78XXLoadScsiRate(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT16);
void SCSIhAICBayonetLoadScsiRate(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT16);
void SCSIhAICTridentLoadScsiRate(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT16);
void SCSIhAIC78XXResetScsiRate(SCSI_HHCB SCSI_HPTR);
void SCSIhAICBayonetResetScsiRate(SCSI_HHCB SCSI_HPTR);
void SCSIhAICTridentResetScsiRate(SCSI_HHCB SCSI_HPTR);
void SCSIhAICTridentClearScsiRate(SCSI_HHCB SCSI_HPTR);
void SCSIhAIC78XXResetDFifo(SCSI_HHCB SCSI_HPTR);
void SCSIhAICBayonetResetDFifo(SCSI_HHCB SCSI_HPTR);
void SCSIhAIC78XXEnableIOErr(SCSI_HHCB SCSI_HPTR);
void SCSIhAICBayonetEnableIOErr(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT32 SCSIhAICBayonetGetScbRamCtrls(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT32 SCSIhAIC78XXGetScbRamCtrls(SCSI_HHCB SCSI_HPTR);
void SCSIhAICBayonetPutScbRamCtrls(SCSI_HHCB SCSI_HPTR, SCSI_UEXACT32);
void SCSIhAIC78XXPutScbRamCtrls(SCSI_HHCB SCSI_HPTR, SCSI_UEXACT32);
SCSI_UEXACT8 SCSIhGetCacheThEn(SCSI_HHCB SCSI_HPTR);
void SCSIhAIC78XXUpdateCacheThEn(SCSI_HHCB SCSI_HPTR);
void SCSIhAICBayonetUpdateCacheThEn(SCSI_HHCB SCSI_HPTR);
void SCSIhAICTridentUpdateCacheThEn(SCSI_HHCB SCSI_HPTR);
void SCSIhStandardResetCCCtl(SCSI_HHCB SCSI_HPTR);
void SCSIhStandardInitCCHHAddr(SCSI_HHCB SCSI_HPTR);
void SCSIhAIC78XXCheckExtArbAckEnBit(SCSI_HHCB SCSI_HPTR);
void SCSIhAICBayonetCheckExtArbAckEnBit(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT16 SCSIhAIC78XXGetSpeed(SCSI_UEXACT16,SCSI_UEXACT8);
SCSI_UEXACT16 SCSIhAICBayonetGetSpeed(SCSI_UEXACT16,SCSI_UEXACT8);
SCSI_UEXACT16 SCSIhAICTridentGetSpeed(SCSI_UEXACT16,SCSI_UEXACT8);
SCSI_UEXACT16 SCSIhAIC78XXCalcScsiOption(SCSI_UEXACT16,SCSI_UEXACT8,SCSI_UEXACT8,SCSI_UEXACT8);
SCSI_UEXACT16 SCSIhAICBayonetCalcScsiOption(SCSI_UEXACT16,SCSI_UEXACT8,SCSI_UEXACT8,SCSI_UEXACT8);
SCSI_UEXACT16 SCSIhAICTridentCalcScsiOption(SCSI_UEXACT16,SCSI_UEXACT8,SCSI_UEXACT8,SCSI_UEXACT8);
void SCSIhAIC78XXGetProfileParameters(SCSI_HW_INFORMATION SCSI_LPTR,SCSI_HHCB SCSI_HPTR);
void SCSIhAICBayonetGetProfileParameters(SCSI_HW_INFORMATION SCSI_LPTR,SCSI_HHCB SCSI_HPTR);
void SCSIhAICTridentGetProfileParameters(SCSI_HW_INFORMATION SCSI_LPTR,SCSI_HHCB SCSI_HPTR);
void SCSIhAIC78XXPutProfileParameters(SCSI_HW_INFORMATION SCSI_LPTR,SCSI_HHCB SCSI_HPTR);
void SCSIhAICBayonetPutProfileParameters(SCSI_HW_INFORMATION SCSI_LPTR,SCSI_HHCB SCSI_HPTR);
void SCSIhAICTridentPutProfileParameters(SCSI_HW_INFORMATION SCSI_LPTR,SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8 SCSIhAIC78XXGetQNewTail(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8 SCSIhAICBayonetGetQNewTail(SCSI_HHCB SCSI_HPTR);
void SCSIhAICBayonetWaitForBusStable(SCSI_HHCB SCSI_HPTR);
void SCSIhSwapping32SetBreakPoint(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIhSwapping64SetBreakPoint(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIhSwappingAdvSetBreakPoint(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIhSwapping160mSetBreakPoint(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIhStandard64SetBreakPoint(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIhStandardAdvSetBreakPoint(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIhStandard160mSetBreakPoint(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIhSwapping32ClearBreakPoint(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIhSwapping64ClearBreakPoint(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIhSwappingAdvClearBreakPoint(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIhSwapping160mClearBreakPoint(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIhStandard64ClearBreakPoint(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIhStandardAdvClearBreakPoint(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIhStandard160mClearBreakPoint(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIhAICBayonetSeqStretchWorkaround(SCSI_HHCB SCSI_HPTR);
void SCSIhSwappingAdvSetIntrThreshold(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIhStandardAdvSetIntrThreshold(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIhSwapping160mSetIntrThreshold(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIhStandard160mSetIntrThreshold(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhSwappingGetIntrThreshold(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8 SCSIhSwappingAdvGetIntrThreshold(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8 SCSIhSwapping160mGetIntrThreshold(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8 SCSIhStandard64GetIntrThreshold(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8 SCSIhStandardAdvGetIntrThreshold(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8 SCSIhStandard160mGetIntrThreshold(SCSI_HHCB SCSI_HPTR);
void SCSIhEnableAutoRateOption(SCSI_HHCB SCSI_HPTR);
void SCSIhDisableAutoRateOption(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT16);
void SCSIhSwapping160mSetPhaseDelayValues(SCSI_HHCB SCSI_HPTR);

#if SCSI_NEGOTIATION_PER_IOB
void SCSIhAIC78XXChangeXferOption(SCSI_HIOB SCSI_IPTR);
void SCSIhAICBayonetChangeXferOption(SCSI_HIOB SCSI_IPTR);
void SCSIhAICTridentChangeXferOption(SCSI_HIOB SCSI_IPTR);
void SCSIhChangeNegotiation(SCSI_UNIT_CONTROL SCSI_UPTR);
#endif

/* HWMPTCL.C */
void SCSIhNegotiate(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhNegotiateWideOrSync(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhExtMsgi(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIhInitiatePPR(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhVerifyPPRResponse(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIhInitiateWide(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhVerifyWideResponse(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhInitiateSync(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
void SCSIhVerifySyncResponse(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhRespondToPPR(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhRespondToWide(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhRespondToSync(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIhSendMessage(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,int);
SCSI_UEXACT8 SCSIhReceiveMessage(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
void SCSIhHandleMsgOut(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhHandleMsgIn(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT16 SCSIhAIC78XXCalcScsiRate(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT8,SCSI_UEXACT8);
SCSI_UEXACT16 SCSIhAICBayonetCalcScsiRate(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT8,SCSI_UEXACT8);
SCSI_UEXACT16 SCSIhAICTridentCalcScsiRate(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT8,SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhAIC78XXGetXferPeriod(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIhAICBayonetGetXferPeriod(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIhAICTridentGetXferPeriod(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT16 SCSIhAIC78XXMaxSyncPeriod(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
SCSI_UEXACT16 SCSIhAICBayonetMaxSyncPeriod(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
SCSI_UEXACT16 SCSIhAICTridentMaxSyncPeriod(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhStandardGetFast20Reg(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhStandardAdvGetFast20Reg(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIhStandardClearFast20Reg(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIhStandardLogFast20Map(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhSwappingGetFast20Reg(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8); 
SCSI_UEXACT8 SCSIhSwappingAdvGetFast20Reg(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8); 
void SCSIhSwappingClearFast20Reg(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8); 
void SCSIhSwappingLogFast20Map(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT8); 
void SCSIhSwapping32UpdateFast20HostMem(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
void SCSIhSwapping64UpdateFast20HostMem(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
void SCSIhSwapping32UpdateFast20HW(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8); 
void SCSIhSwapping64UpdateFast20HW(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8); 
void SCSIhCheckSyncNego(SCSI_HHCB SCSI_HPTR);
void SCSIhStandardXferOptAssign(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT16);
void SCSIhHostXferOptAssign(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT16);
void SCSIhChkCondXferAssign(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT16);
void SCSIhSwapping32CommonXferAssign(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT16); 
void SCSIhSwapping64CommonXferAssign(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT16); 
void SCSIhStandardAdvCommonXferAssign(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT16);
void SCSIhSwappingAdvCommonXferAssign(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT16);
void SCSIhStandard160mCommonXferAssign(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT16);
void SCSIhSwapping160mCommonXferAssign(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT16);
void SCSIhSwapping32UpdateXferOptionHostMem(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT16); 
void SCSIhSwapping64UpdateXferOptionHostMem(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT16); 
void SCSIhStandardAdvUpdateXferOptionHostMem(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT16);
void SCSIhSwappingAdvUpdateXferOptionHostMem(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT16);
void SCSIhStandard160mUpdateXferOptionHostMem(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT16);
void SCSIhSwapping160mUpdateXferOptionHostMem(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT16);
SCSI_UEXACT16 SCSIhStandardGetOption(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
SCSI_UEXACT16 SCSIhHostGetOption(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIhStandard64ModifyDataPtr(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwapping32ModifyDataPtr(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwapping64ModifyDataPtr(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); 
void SCSIhStandardAdvModifyDataPtr(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); 
void SCSIhSwappingAdvModifyDataPtr(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); 
void SCSIhStandard160mModifyDataPtr(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); 
void SCSIhSwapping160mModifyDataPtr(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); 
SCSI_UEXACT8 SCSIhAIC78XXGetWideScsiOffset(SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIhAIC78XXGetNarrowScsiOffset(SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIhAICBayonetGetScsiOffset(SCSI_HIOB SCSI_IPTR);
void SCSIhAIC78XXClearScsiOffset(SCSI_HIOB SCSI_IPTR);
void SCSIhAICBayonetClearScsiOffset(SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIhAIC78XXWasSyncXfer(SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhAICBayonetWasSyncXfer(SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhAIC78XXSetDataFifoThrshDefault(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8 SCSIhAIC78XXReadDataFifoThrsh(SCSI_HHCB SCSI_HPTR);                  
void SCSIhAIC78XXUpdateDataFifoThrsh(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8 SCSIhAICBayonetSetDataFifoThrshDefault(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8 SCSIhAICBayonetReadDataFifoThrsh(SCSI_HHCB SCSI_HPTR);                  
void SCSIhAICBayonetUpdateDataFifoThrsh(SCSI_HHCB SCSI_HPTR);
void SCSIhSwappingClearAllFast20Reg(SCSI_HHCB SCSI_HPTR);
void SCSIhStandard64ClearAllFast20Reg(SCSI_HHCB SCSI_HPTR);
void SCSIhAIC78XXIgnoreWideResCalc(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
void SCSIhAICBayonetIgnoreWideResCalc(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
void SCSIhStandardAdvUpdateSGLength(SCSI_HHCB SCSI_HPTR,SCSI_QUADLET);
void SCSIhStandardAdvUpdateSGAddress(SCSI_HHCB SCSI_HPTR,SCSI_QUADLET);
void SCSIhSwappingAdvUpdateSGLength(SCSI_HHCB SCSI_HPTR,SCSI_QUADLET);
void SCSIhSwappingAdvUpdateSGAddress(SCSI_HHCB SCSI_HPTR,SCSI_QUADLET);
void SCSIhStandard160mUpdateSGLength(SCSI_HHCB SCSI_HPTR,SCSI_QUADLET);
void SCSIhStandard160mUpdateSGAddress(SCSI_HHCB SCSI_HPTR,SCSI_QUADLET);
void SCSIhSwapping160mUpdateSGLength(SCSI_HHCB SCSI_HPTR,SCSI_QUADLET);
void SCSIhSwapping160mUpdateSGAddress(SCSI_HHCB SCSI_HPTR,SCSI_QUADLET);

#if SCSI_TARGET_OPERATION && SCSI_MULTIPLEID_SUPPORT 
SCSI_UEXACT16 SCSIhMultipleHostIdGetOption(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT8);
void SCSIhMultipleHostIdXferOptAssign(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT8,SCSI_UEXACT16); 
void SCSIhMultipleHostIdCommonXferAssign(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT8,SCSI_UEXACT16);
#endif /* SCSI_TARGET_OPERATION && SCSI_MULTIPLEID_SUPPORT */ 
#if SCSI_PAC_NSX_REPEATER || SCSI_NSX_REPEATER
SCSI_UEXACT8 SCSIhNsxRepeaterCommunication(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
#endif /* SCSI_PAC_NSX_REPEATER || SCSI_NSX_REPEATER */


/* HWMHRST.C */
void SCSIhResetChannelHardware(SCSI_HHCB SCSI_HPTR);
void SCSIhAbortChannel(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIhStandard64ResetSoftware(SCSI_HHCB SCSI_HPTR);
void SCSIhSwappingResetSoftware(SCSI_HHCB SCSI_HPTR); 
void SCSIhStandardAdvResetSoftware(SCSI_HHCB SCSI_HPTR);
void SCSIhSwappingAdvResetSoftware(SCSI_HHCB SCSI_HPTR); 
void SCSIhStandard160mResetSoftware(SCSI_HHCB SCSI_HPTR);
void SCSIhSwapping160mResetSoftware(SCSI_HHCB SCSI_HPTR); 
void SCSIhCommonResetSoftware(SCSI_HHCB SCSI_HPTR);
void SCSIhSetAddressScratchBA32(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,void SCSI_HPTR);
void SCSIhSetAddressScratchBA64(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,void SCSI_HPTR);
void SCSIhStandardResetBTA(SCSI_HHCB SCSI_HPTR);
void SCSIhSwappingResetBTA(SCSI_HHCB SCSI_HPTR); 
void SCSIhStandardTargetClearBusy(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwappingTargetClearBusy(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); 
void SCSIhStandardIndexClearBusy(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIhSwappingIndexClearBusy(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8); 
SCSI_UEXACT8 SCSIhResetScsi(SCSI_HHCB SCSI_HPTR);
void SCSIhScsiBusReset(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhBusDeviceReset(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhStandard64AbortHIOB(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
void SCSIhStandardAdvAbortHIOB(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
void SCSIhSwappingAbortHIOB(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8); 
void SCSIhSwappingAdvAbortHIOB(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8); 
void SCSIhSwapping160mAbortHIOB(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR, SCSI_UEXACT8);
void SCSIhStandard160mAbortHIOB(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhStandardSearchSeqDoneQ(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhStandardAdvSearchSeqDoneQ(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhStandard160mSearchSeqDoneQ(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhStandardSearchDoneQ(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIhStandardSearchNewQ(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhSwappingSearchNewQ(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8); 
SCSI_UEXACT8 SCSIhStandard64ObtainNextScbNum(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);                                                                 
SCSI_UEXACT8 SCSIhSwapping32ObtainNextScbNum(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIhSwapping64ObtainNextScbNum(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);                                                                 
SCSI_UEXACT8 SCSIhStandardAdvObtainNextScbNum(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIhSwappingAdvObtainNextScbNum(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIhStandard160mObtainNextScbNum(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIhSwapping160mObtainNextScbNum(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhStandard64UpdateNextScbNum(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);                                                                 
void SCSIhSwapping32UpdateNextScbNum(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
void SCSIhSwapping64UpdateNextScbNum(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);                                                                 
void SCSIhStandardAdvUpdateNextScbNum(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
void SCSIhSwappingAdvUpdateNextScbNum(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
void SCSIhStandard160mUpdateNextScbNum(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
void SCSIhSwapping160mUpdateNextScbNum(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
SCSI_BUS_ADDRESS SCSIhStandard64ObtainNextScbAddress(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);                                                                 
SCSI_BUS_ADDRESS SCSIhStandardAdvObtainNextScbAddress(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_BUS_ADDRESS SCSIhStandard160mObtainNextScbAddress(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhStandard64UpdateNextScbAddress(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_BUS_ADDRESS);
void SCSIhStandardAdvUpdateNextScbAddress(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_BUS_ADDRESS);
void SCSIhStandard160mUpdateNextScbAddress(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_BUS_ADDRESS);
SCSI_UEXACT8 SCSIhStandard64SearchExeQ(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8,SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhSwappingSearchExeQ(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8,SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhStandardAdvSearchExeQ(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8,SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhStandard160mSearchExeQ(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8,SCSI_UEXACT8);
void SCSIhStandard64UpdateAbortBitHostMem(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); 
void SCSIhStandardAdvUpdateAbortBitHostMem(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); 
void SCSIhStandard160mUpdateAbortBitHostMem(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwapping32UpdateAbortBitHostMem(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); 
void SCSIhSwapping64UpdateAbortBitHostMem(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); 
void SCSIhSwappingAdvUpdateAbortBitHostMem(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); 
void SCSIhSwapping160mUpdateAbortBitHostMem(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhStandard64SetupBDR(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwapping32SetupBDR(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwapping64SetupBDR(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); 
void SCSIhStandardAdvSetupBDR(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwappingAdvSetupBDR(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhStandard160mSetupBDR(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwapping160mSetupBDR(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhStandard64StackBugFix(SCSI_HHCB SCSI_HPTR);
void SCSIhSwapping32FreezeHWQueue(SCSI_HHCB SCSI_HPTR, SCSI_HIOB SCSI_IPTR);
void SCSIhStandardAdvFreezeHWQueue(SCSI_HHCB SCSI_HPTR, SCSI_HIOB SCSI_IPTR);
void SCSIhSwappingAdvFreezeHWQueue(SCSI_HHCB SCSI_HPTR, SCSI_HIOB SCSI_IPTR);
void SCSIhStandard160mFreezeHWQueue(SCSI_HHCB SCSI_HPTR, SCSI_HIOB SCSI_IPTR);
void SCSIhSwapping160mFreezeHWQueue(SCSI_HHCB SCSI_HPTR, SCSI_HIOB SCSI_IPTR);

/* HWMINIT.C */                           
int SCSIhStandardConfigureScbRam(SCSI_HHCB SCSI_HPTR);
void SCSIhSetupEnvironment(SCSI_HHCB SCSI_HPTR);
void SCSIhPrepareConfig(SCSI_HHCB SCSI_HPTR);
void SCSIhGetCapability(SCSI_HHCB SCSI_HPTR);
void SCSIhStandardGetConfig(SCSI_HHCB SCSI_HPTR);
void SCSIhNonStandardGetConfig(SCSI_HHCB SCSI_HPTR);
int SCSIhGetMaxHardwareScbs(SCSI_HHCB SCSI_HPTR);
void SCSIhGetCommonConfig(SCSI_HHCB SCSI_HPTR);
void SCSIhGetHardwareConfiguration(SCSI_HHCB SCSI_HPTR);
void SCSIhGetIDBasedConfiguration(SCSI_HHCB SCSI_HPTR);
int SCSIhStandard64SetupSequencer(SCSI_HHCB SCSI_HPTR);
int SCSIhSwapping32SetupSequencer(SCSI_HHCB SCSI_HPTR);
int SCSIhSwapping64SetupSequencer(SCSI_HHCB SCSI_HPTR);
int SCSIhStandardAdvSetupSequencer(SCSI_HHCB SCSI_HPTR);
int SCSIhSwappingAdvSetupSequencer(SCSI_HHCB SCSI_HPTR);
int SCSIhStandard160mSetupSequencer(SCSI_HHCB SCSI_HPTR);
int SCSIhSwapping160mSetupSequencer(SCSI_HHCB SCSI_HPTR);
void SCSIhInitializeHardware(SCSI_HHCB SCSI_HPTR);
void SCSIhStartToRunSequencer(SCSI_HHCB SCSI_HPTR);
void SCSIhAIC78XXProcessAutoTerm(SCSI_HHCB SCSI_HPTR);
void SCSIhAICBayonetProcessAutoTerm(SCSI_HHCB SCSI_HPTR);
void SCSIhAIC78XXUpdateExtTerm(SCSI_HHCB SCSI_HPTR);
void SCSIhAICBayonetUpdateExtTerm(SCSI_HHCB SCSI_HPTR);
                  
/* HWMINTR.C */
int SCSIhCmdComplete(SCSI_HHCB SCSI_HPTR);
void SCSIhIntSrst(SCSI_HHCB SCSI_HPTR);
void SCSIhIntIOError(SCSI_HHCB SCSI_HPTR);
void SCSIhCheckLength(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhCdbAbort(SCSI_HHCB SCSI_HPTR);
void SCSIhCheckCondition(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhBadSeq(SCSI_HHCB SCSI_HPTR);
void SCSIhAbortTarget(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhIntSelto(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhScamSelection(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhIntFree(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhParityError(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhStandard64RequestSense(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwapping32RequestSense(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwapping64RequestSense(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); 
void SCSIhStandardAdvRequestSense(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); 
void SCSIhSwappingAdvRequestSense(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); 
void SCSIhStandard160mRequestSense(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); 
void SCSIhSwapping160mRequestSense(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); 
void SCSIhTerminateCommand(SCSI_HIOB SCSI_IPTR);
void SCSIhSetStat(SCSI_HIOB SCSI_IPTR);
void SCSIhBreakInterrupt(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhStandard64ActiveAbort(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwapping32ActiveAbort(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwapping64ActiveAbort(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); 
void SCSIhStandardAdvActiveAbort(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwappingAdvActiveAbort(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhStandard160mActiveAbort(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwapping160mActiveAbort(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhStandardRemoveActiveAbort(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwappingRemoveActiveAbort(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT32 SCSIhStandard64ResidueCalc(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT32 SCSIhSwapping32ResidueCalc(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT32 SCSIhSwapping64ResidueCalc(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT32 SCSIhStandardAdvResidueCalc(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT32 SCSIhSwappingAdvResidueCalc(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT32 SCSIhStandard160mResidueCalc(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT32 SCSIhSwapping160mResidueCalc(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIhSwappingRemoveAbortHIOBFromNewQ(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhBusWasDead(SCSI_HHCB SCSI_HPTR);
#if SCSI_CHECK_PCI_ERROR
void SCSIhClearPCIError(SCSI_HHCB SCSI_HPTR);
#endif /* #if SCSI_CHECK_PCI_ERROR */

/* HWMSCAM.C */
SCSI_UEXACT8   SCSIhScamLevel1(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8   SCSIhScamLevel2(SCSI_HHCB SCSI_HPTR);
void           SCSIhScamIdStringInit(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8   SCSIhScamLevel1Initiate(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8   SCSIhScamLevel2Initiate(SCSI_HHCB SCSI_HPTR);
void           SCSIhScamIssueDIC(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8   SCSIhScamDetectDIC(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8   SCSIhScamIsolation(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8   SCSIhScamDominantInit(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8   SCSIhScamSubordinateInit(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT32  SCSIhScamAssignID(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT16);
SCSI_UEXACT16  SCSIhScamChooseID(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT16,SCSI_UEXACT8,SCSI_UEXACT8);
SCSI_UEXACT16  SCSIhScamFindNonScamDevices(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8   SCSIhScamSelectDevice(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
SCSI_UEXACT8   SCSIhScamInquiry(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
SCSI_UEXACT8   SCSIhScamSubordinateIDAssign(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8   SCSIhScamSupportCheck(register SCSI_REGISTER);
SCSI_UEXACT8   SCSIhScamDataOut(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
SCSI_UEXACT8   SCSIhScamDataIn(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8   SCSIhScamWaitForRelease(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT8);
SCSI_UEXACT8   SCSIhScamWaitForAssertion(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT8);
                               
/* HWMSE2.C */
SCSI_UEXACT16 SCSIhReadE2Register(SCSI_UINT16,SCSI_REGISTER,SCSI_UINT);
void SCSIhWriteE2Register(SCSI_UINT16,SCSI_UINT16,SCSI_REGISTER,SCSI_UINT);
void SCSIhSendE2Address(SCSI_UINT16,SCSI_INT,SCSI_REGISTER);
void SCSIhWait2usec(SCSI_UEXACT8,SCSI_REGISTER );
#if SCSI_AICTRIDENT
SCSI_INT SCSIhTridentReadSEEPROM (SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8 SCSI_SPTR,SCSI_SINT16,SCSI_SINT16);
SCSI_INT SCSIhTridentWriteSEEPROM (SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8 SCSI_SPTR,SCSI_SINT16,SCSI_SINT16);
#endif

/* HWMTRGT.C */
#if SCSI_TARGET_OPERATION
void SCSIhTargetIntSelIn(SCSI_HHCB SCSI_HPTR);
void SCSIhTargetATNIntr(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIhTargetSendMsgByte(SCSI_UEXACT8,SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIhTargetCmdPhase(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8); 
void  SCSIhSetTargetMode(SCSI_HIOB SCSI_IPTR); 
SCSI_UEXACT8 SCSIhTargetHandleATN(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhTargetDecodeMsgs(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8 SCSI_SPTR,SCSI_UEXACT8);
void SCSIhTargetSendHiob(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);   
SCSI_UEXACT8 SCSIhTargetRespondToWide(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8 SCSI_SPTR);
void  SCSIhTargetModeEnable(SCSI_HHCB SCSI_HPTR);                               
SCSI_UEXACT8 SCSIhTargetRespondToSync(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8 SCSI_SPTR);
SCSI_UEXACT8 SCSIhTargetSendMessage(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8 SCSI_SPTR,int);
SCSI_UEXACT8 SCSIhTargetGetMessage(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8 SCSI_SPTR,int);
SCSI_UEXACT8 SCSIhTargetGetMsgBytes(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8 SCSI_SPTR,int);
SCSI_UEXACT8 SCSIhTargetGetOneMsgByte(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhWaitForSPIORDY(SCSI_HHCB SCSI_HPTR);                             
SCSI_NODE SCSI_NPTR SCSIhTargetGetNODE(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIhTargetScsiBusFree(SCSI_HHCB SCSI_HPTR);
void SCSIhTerminateTargetCommand(SCSI_HIOB SCSI_IPTR);
void SCSIhTargetScsiBusHeld(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhTargetBusDeviceReset(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
void SCSIhTargetResetTarget(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8 SCSIhTargetCompareNexus(SCSI_NEXUS SCSI_XPTR,SCSI_NEXUS SCSI_XPTR);
SCSI_UEXACT8 SCSIhTargetAbortNexus(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
void SCSIhTargetHoldOnToScsiBus(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8,SCSI_UEXACT8); 
void SCSIhTargetIntSelto(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhTargetParityError(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIhTargetTerminateDMA(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhTargetAbortNexusSet(SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIhTargetGoToPhase(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT8);
void SCSIhTargetGoToPhaseManualPIO(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT8);
void SCSIhTargetSetStat(SCSI_HIOB SCSI_IPTR);
void SCSIhTargetSetTaskMngtFunction(SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
void SCSIhTargetHandleSpecialFunction(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhTargetHandleIgnoreWideResidue(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhTargetRedirectSequencer(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8 SCSIhTargetGetOneCmdByte(SCSI_HHCB SCSI_HPTR);
void  SCSIhTargetClearSelection(SCSI_HHCB SCSI_HPTR);
void SCSIhTargetInitializeNodeTbl(SCSI_HHCB  SCSI_HPTR); 
void SCSIhTargetInitiateWide(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhTargetInitiateSync(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
void SCSIhTargetVerifyWideResponse(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8 SCSI_SPTR);
void SCSIhTargetVerifySyncResponse(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8 SCSI_SPTR);
void SCSIhTargetNegotiate(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
void SCSIhTargetEnableId(SCSI_HIOB SCSI_IPTR);
void SCSIhTargetSetupDisableId(SCSI_HIOB SCSI_IPTR);
void SCSIhTargetDisableId(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8 SCSIhTargetRespondToPPR(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8 SCSI_SPTR);
void SCSIhTargetVerifyPPRResponse(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8 SCSI_SPTR);
/* may no longer need these as only used in hwmtrgt.c */
SCSI_UEXACT8 SCSIhTargetCommandState(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_HPTR, SCSI_NODE SCSI_NPTR);
SCSI_UEXACT8 SCSIhTargetDisconnectState(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_HPTR,SCSI_NODE SCSI_NPTR);
SCSI_UEXACT8 SCSIhTargetNoOp(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_HPTR,SCSI_NODE SCSI_NPTR);
SCSI_UEXACT8 SCSIhTargetSelectionState(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_HPTR,SCSI_NODE SCSI_NPTR);
SCSI_UEXACT8 SCSIhTargetIdentifyState(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_HPTR,SCSI_NODE SCSI_NPTR);
SCSI_UEXACT8 SCSIhTargetMessageOutState(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_HPTR,SCSI_NODE SCSI_NPTR); 
SCSI_UEXACT8 SCSIhTargetNegotiated(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIhTargetSetNegotiatedState(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT8);
void SCSIhTargetForceTargetNegotiation(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIhTargetSwappingSendHiobSpecial(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
/* Swapping Advanced routines */
void SCSIhTargetSwappingAdvPatchXferRate(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhTargetSwappingAdvSetIgnoreWideMsg(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhTargetSwappingAdvGetEstScbFields(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
void SCSIhTargetSwappingAdvDeliverEstScb(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhTargetSwappingAdvUpdateScbRamXferOption(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT8,SCSI_UEXACT16);
/* Swapping 160m routines */
void SCSIhTargetSwapping160mSetIgnoreWideMsg(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhTargetSwapping160mDeliverEstScb(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhTargetSwapping160mGetEstScbFields(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
/* Target Mode Bayonet routines */
void SCSIhTargetAICBayonetLoadTargetIDs(SCSI_HHCB SCSI_HPTR);
#endif /* SCSI_TARGET_OPERATION */
void SCSIhTargetGetConfiguration(SCSI_HHCB SCSI_HPTR);

#if SCSI_LOOPBACK_OPERATION
void SCSIhSetXferRate(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
#endif /* SCSI_LOOPBACK_OPERATION */

/*$Header: /home/CVSROOT/sw/srmconsole/5.8/SRC/AIC78XX_HIM_HWM.H,v 1.2 2001/01/13 05:23:41 chris Exp $*/
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
*  Module Name:   HWMDEF.H
*
*  Description:   Definitions for hwarware management layer
*    
*  Owners:        ECX IC Firmware Team
*    
*  Notes:         1. This file is referenced by hardware management layer
*                    and application to hardware management layer
*
***************************************************************************/

/***************************************************************************
* Function prototype as interface to hardware management layer 
***************************************************************************/
/* HWMINIT.C */
int SCSIHGetConfiguration (SCSI_HHCB SCSI_HPTR);
void SCSIHGetMemoryTable(SCSI_UEXACT8,SCSI_UEXACT8,SCSI_MEMORY_TABLE SCSI_HPTR);
SCSI_INT SCSIHApplyMemoryTable (SCSI_HHCB SCSI_HPTR,SCSI_MEMORY_TABLE SCSI_HPTR);
int SCSIHInitialize(SCSI_HHCB SCSI_HPTR);
void SCSIHSetupHardware(SCSI_HHCB SCSI_HPTR);

/* HWMTASK.C */
void SCSIHSetUnitHandle(SCSI_HHCB SCSI_HPTR, SCSI_UNIT_CONTROL SCSI_UPTR,SCSI_UEXACT8,SCSI_UEXACT8);
void SCSIHFreeUnitHandle (SCSI_UNIT_HANDLE);
void SCSIHDisableIRQ (SCSI_HHCB SCSI_HPTR);
void SCSIHEnableIRQ (SCSI_HHCB SCSI_HPTR);
int SCSIHPollIRQ (SCSI_HHCB SCSI_HPTR);
void SCSIHQueueHIOB (SCSI_HIOB SCSI_IPTR);
void SCSIHQueueSpecialHIOB (SCSI_HIOB SCSI_IPTR);
void SCSIHSaveState (SCSI_HHCB SCSI_HPTR,SCSI_STATE SCSI_HPTR);
void SCSIHRestoreState (SCSI_HHCB SCSI_HPTR,SCSI_STATE SCSI_HPTR);
SCSI_UINT SCSIHPowerManagement( SCSI_HHCB SCSI_HPTR,SCSI_UINT powerMode);
int SCSIHReadNVRAM (SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8 SCSI_SPTR,
                                       SCSI_SINT16,SCSI_SINT16);
int SCSIHWriteNVRAM(SCSI_HHCB SCSI_HPTR,SCSI_SINT16,SCSI_UEXACT8 SCSI_SPTR,
                                       SCSI_SINT16);
int SCSIHApplyDeviceTable(SCSI_HHCB SCSI_HPTR);
int SCSIHCheckHostIdle(SCSI_HHCB SCSI_HPTR);
int SCSIHCheckDeviceIdle (SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIHSuppressNegotiation(SCSI_UNIT_CONTROL SCSI_UPTR);
void SCSIHForceNegotiation(SCSI_UNIT_CONTROL SCSI_UPTR);
void SCSIHGetHWInformation(SCSI_HW_INFORMATION SCSI_LPTR,SCSI_HHCB SCSI_HPTR);
void SCSIHPutHWInformation(SCSI_HW_INFORMATION SCSI_LPTR,SCSI_HHCB SCSI_HPTR);
void SCSIHEnableExpStatus (SCSI_HHCB SCSI_HPTR);
void SCSIHDisableExpStatus (SCSI_HHCB SCSI_HPTR);
#if SCSI_PAC_NSX_REPEATER
void SCSIHSuppressNegoActiveTarget(SCSI_UNIT_CONTROL SCSI_UPTR);
void SCSIHForceNegoActiveTarget(SCSI_HHCB SCSI_HPTR);
#endif /* SCSI_PAC_NSX_REPEATER */

/* HWMINTR.C */                        
int SCSIHFrontEndISR (SCSI_HHCB SCSI_HPTR);
void SCSIHBackEndISR(SCSI_HHCB SCSI_HPTR);

/* HWMSE2.C */
SCSI_INT SCSIHSizeSEEPROM(SCSI_HHCB SCSI_HPTR);
SCSI_INT SCSIHReadSEEPROM (SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8 SCSI_SPTR,SCSI_SINT16,SCSI_SINT16);
SCSI_INT SCSIHWriteSEEPROM( SCSI_HHCB SCSI_HPTR,SCSI_SINT16,SCSI_SINT16,SCSI_UEXACT8 SCSI_SPTR);
                   
/* HWMUTIL.C */
int SCSIHEnableExtScbRam(SCSI_HHCB SCSI_HPTR);
int SCSIHDisableExtScbRam(SCSI_HHCB SCSI_HPTR);
SCSI_INT SCSIHExtScbRAMAvail(SCSI_HHCB SCSI_HPTR);
void SCSIHReadScbRAM(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8 SCSI_SPTR,SCSI_UEXACT8,SCSI_UEXACT8,SCSI_UEXACT8);
void SCSIHWriteScbRAM(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT8,SCSI_UEXACT8,SCSI_UEXACT8 SCSI_SPTR);
SCSI_INT SCSIHHardwareInResetState(SCSI_HHCB SCSI_HPTR);
void SCSIHReadScratchRam(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8 SCSI_SPTR,int,int);
void SCSIHWriteScratchRam(SCSI_HHCB SCSI_HPTR,int,int,SCSI_UEXACT8 SCSI_SPTR);
void SCSIHReadHADDR(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8 SCSI_SPTR);
SCSI_UINT8 SCSIHCheckSigSCB2(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8 SCSI_SPTR);
#if SCSI_PAC_NSX_REPEATER
SCSI_UEXACT8 SCSIHSingleEndBus(SCSI_HHCB SCSI_HPTR);
#endif /* SCSI_PAC_NSX_REPEATER */
#if SCSI_DOMAIN_VALIDATION
SCSI_UEXACT8 SCSIHDataBusHang(SCSI_HHCB SCSI_HPTR);
#endif
#if SCSI_TARGET_OPERATION

/* HWMTRGT.C */
void  SCSIHTargetModeDisable(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8 SCSIHTargetSelInPending(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8 SCSIHTargetClearNexus(SCSI_NEXUS  SCSI_XPTR,SCSI_HHCB SCSI_HPTR);
#endif /* SCSI_TARGET_OPERATION */

/* RSM*.C */                     
void SCSIrCompleteHIOB(SCSI_HIOB SCSI_IPTR);
void SCSIrCompleteSpecialHIOB(SCSI_HIOB SCSI_IPTR);
void SCSIrAsyncEvent(SCSI_HHCB SCSI_HPTR,SCSI_UINT16,...);
SCSI_SCB_DESCRIPTOR SCSI_HPTR SCSIRGetFreeHead(SCSI_HHCB SCSI_HPTR);
void SCSIrAsyncEventCommand(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UINT16,...);

#if SCSI_TARGET_OPERATION
SCSI_SCB_DESCRIPTOR SCSI_HPTR SCSIRGetFreeEstHead(SCSI_HHCB SCSI_HPTR);
SCSI_NODE SCSI_NPTR SCSIrAllocateNode (SCSI_HHCB SCSI_HPTR);
#endif /* SCSI_TARGET_OPERATION */
