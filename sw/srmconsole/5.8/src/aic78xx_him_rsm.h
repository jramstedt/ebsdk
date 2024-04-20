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

/*$Header: /home/CVSROOT/sw/srmconsole/5.8/SRC/AIC78XX_HIM_RSM.H,v 1.2 2001/01/13 05:23:43 chris Exp $*/
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
*  Module Name:   RSM.C
*
*  Description:   Include files for resource management 
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
#include "hwmref.h"	/* exclude */
#include "hwmdef.h"	/* exclude */
#endif

/*$Header: /home/CVSROOT/sw/srmconsole/5.8/SRC/AIC78XX_HIM_RSM.H,v 1.2 2001/01/13 05:23:43 chris Exp $*/
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
*  Module Name:   RSMREF.H
*
*  Description:   Definitions internal to resource management layer
*    
*  Owners:        ECX IC Firmware Team
*    
*  Notes:         1. This file should only be referenced by resource
*                    management layer.
*
***************************************************************************/

#define  SCSI_DOMAIN_VALIDATION  (SCSI_DOMAIN_VALIDATION_BASIC+SCSI_DOMAIN_VALIDATION_ENHANCED)

/***************************************************************************
* Macro definitions for resource management layer
***************************************************************************/
#if SCSI_TARGET_OPERATION
#define  SCSI_rFRESH_HIOB(hiob)  (hiob)->stat = 0;\
                                 (hiob)->haStat = 0;\
                                 (hiob)->trgStatus = 0;\
                                 (hiob)->SCSI_IP_mgrStat = 0;\
                                 (hiob)->SCSI_IP_targetMode = 0;\
                                 (hiob)->SCSI_IP_estScb = 0;\
                                 (hiob)->SCSI_IP_negoState = 0;

#define  SCSI_rTARGETMODE_HIOB(hiob)  (hiob)->SCSI_IP_targetMode = 1; 
#define  SCSI_rABORTHIOBINHIOBQ(hiob,haStat)\
            SCSIrAbortHIOBInHiobQ((hiob),(haStat))
#define  SCSI_rCLEARALLDEVICEQUEUES(hhcb,haStat)\
            if ((hhcb)->SCSI_HF_initiatorMode)\
            {\
               SCSIrClearAllDeviceQueues((hhcb),(haStat));\
            }
/* Attempt to queue Establish Connection HIOBs which 
 * may have been queued to HIOB queue during reset.
 */
#define  SCSI_rQUEUEESTHIOBS(hhcb)\
            if ((hhcb)->SCSI_HF_targetMode && \
                (!((hhcb)->SCSI_HF_targetHoldEstHiobs)))\
            {\
               SCSIrQueueEstHiobs((hhcb));\
            }
/* Don't call SCSIrFreeCriteria for target mode hiobs */ 
#define  SCSI_rFREECRITERIA(hiob)\
            if (!(hiob)->SCSI_IP_targetMode)\
            {\
               SCSIrFreeCriteria((hiob));\
            }
#define  SCSI_rFREEZEALLDEVICEQUEUES(hhcb)\
            if ((hhcb)->SCSI_HF_initiatorMode)\
            {\
               SCSIrFreezeAllDeviceQueues((hhcb));\
            }
#define  SCSI_rUNFREEZEALLDEVICEQUEUES(hhcb)\
            if ((hhcb)->SCSI_HF_initiatorMode)\
            {\
               SCSIrUnFreezeAllDeviceQueues((hhcb));\
            }
#else
#define  SCSI_rFRESH_HIOB(hiob)  (hiob)->stat = 0;\
                                 (hiob)->haStat = 0;\
                                 (hiob)->trgStatus = 0;\
                                 (hiob)->SCSI_IP_mgrStat = 0;\
                                 (hiob)->SCSI_IP_negoState = 0;
#define  SCSI_rABORTHIOBINHIOBQ(hiob,haStat) (SCSI_FALSE)
#define  SCSI_rCLEARALLDEVICEQUEUES(hhcb,haStat)\
            SCSIrClearAllDeviceQueues((hhcb),(haStat));
#define  SCSI_rQUEUEESTHIOBS(hhcb)
#define  SCSI_rFREECRITERIA(hiob)  SCSIrFreeCriteria((hiob));
#define  SCSI_rFREEZEALLDEVICEQUEUES(hhcb)  SCSIrFreezeAllDeviceQueues((hhcb));
#define  SCSI_rUNFREEZEALLDEVICEQUEUES(hhcb)  SCSIrUnFreezeAllDeviceQueues((hhcb));
#endif /* SCSI_TARGET_OPERATION */

#if SCSI_MULTI_MODE
#define  SCSI_rREMOVESCBS(hhcb)\
               if ((hhcb)->SCSI_HP_firmwareDescriptor->SCSIrRemoveScbs)\
               {\
                  (hhcb)->SCSI_HP_firmwareDescriptor->SCSIrRemoveScbs((hhcb));\
               }
#define  SCSI_rGETSCB(hhcb,hiob)       (hhcb)->SCSI_HP_firmwareDescriptor->SCSIrGetScb((hhcb),(hiob))
#define  SCSI_rRETURNSCB(hhcb,hiob) (hhcb)->SCSI_HP_firmwareDescriptor->SCSIrReturnScb((hhcb),(hiob))
#define  SCSI_rHOSTQUEUEREMOVE(hhcb,scbNumber) (hhcb)->SCSI_HP_firmwareDescriptor->SCSIrHostQueueRemove((hhcb),(scbNumber))
#define  SCSI_rMAX_NONTAG_SCBS(hhcb) (hhcb)->SCSI_HP_firmwareDescriptor->MaxNonTagScbs
#if SCSI_TARGET_OPERATION
#define  SCSI_rGETESTSCB(hhcb,hiob)    (hhcb)->SCSI_HP_firmwareDescriptor->SCSIrGetEstScb((hhcb),(hiob))
#define  SCSI_rRETURNESTSCB(hhcb,hiob) (hhcb)->SCSI_HP_firmwareDescriptor->SCSIrReturnEstScb((hhcb),(hiob))
#endif /* SCSI_TARGET_OPERATION */

#else 
#if SCSI_STANDARD64_MODE
#define  SCSI_rREMOVESCBS(hhcb) SCSIrStandardRemoveScbs((hhcb))
#define  SCSI_rGETSCB(hhcb,hiob)       SCSIrGetScb((hhcb),(hiob))
#define  SCSI_rRETURNSCB(hhcb,hiob) SCSIrReturnScb((hhcb),(hiob))
#define  SCSI_rHOSTQUEUEREMOVE(hhcb,scbNumber) SCSIrHostQueueRemove((hhcb),(scbNumber))
#define  SCSI_rMAX_NONTAG_SCBS(hhcb) SCSI_S64_MAX_NONTAG_SCBS
#if SCSI_TARGET_OPERATION
#define  SCSI_rGETESTSCB(hhcb,hiob)  
#define  SCSI_rRETURNESTSCB(hhcb,hiob)               
#endif /* SCSI_TARGET_OPERATION */
#endif

#if SCSI_SWAPPING32_MODE || SCSI_SWAPPING64_MODE
#define  SCSI_rREMOVESCBS(hhcb)
#define  SCSI_rGETSCB(hhcb,hiob)  SCSIrGetScb((hhcb),(hiob))
#define  SCSI_rRETURNSCB(hhcb,hiob) SCSIrReturnScb((hhcb),(hiob))
#define  SCSI_rHOSTQUEUEREMOVE(hhcb,scbNumber) SCSIrHostQueueRemove((hhcb),(scbNumber))
#define  SCSI_rMAX_NONTAG_SCBS(hhcb) SCSI_W_MAX_NONTAG_SCBS
#if SCSI_TARGET_OPERATION
#define  SCSI_rGETESTSCB(hhcb,hiob)  
#define  SCSI_rRETURNESTSCB(hhcb,hiob)
#endif /* SCSI_TARGET_OPERATION */
#endif

#if SCSI_STANDARD_ADVANCED_MODE
#define  SCSI_rREMOVESCBS(hhcb) SCSIrStandardRemoveScbs((hhcb))
#define  SCSI_rGETSCB(hhcb,hiob)       SCSIrGetScb((hhcb),(hiob))
#define  SCSI_rRETURNSCB(hhcb,hiob) SCSIrReturnScb((hhcb),(hiob))
#define  SCSI_rHOSTQUEUEREMOVE(hhcb,scbNumber) SCSIrHostQueueRemove((hhcb),(scbNumber))
#define SCSI_rMAX_NONTAG_SCBS(hhcb) SCSI_SADV_MAX_NONTAG_SCBS
#if SCSI_TARGET_OPERATION
#define  SCSI_rGETESTSCB(hhcb,hiob)  
#define  SCSI_rRETURNESTSCB(hhcb,hiob)
#endif /* SCSI_TARGET_OPERATION */
#endif

#if SCSI_SWAPPING_ADVANCED_MODE
#if SCSI_TARGET_OPERATION
#define  SCSI_rREMOVESCBS(hhcb)   SCSIrSwappingRemoveScbs((hhcb))
#else
#define  SCSI_rREMOVESCBS(hhcb)
#endif
#define  SCSI_rGETSCB(hhcb,hiob)  SCSIrGetScb((hhcb),(hiob))
#define  SCSI_rRETURNSCB(hhcb,hiob) SCSIrReturnScb((hhcb),(hiob))
#define  SCSI_rHOSTQUEUEREMOVE(hhcb,scbNumber) SCSIrHostQueueRemove((hhcb),(scbNumber))
#define SCSI_rMAX_NONTAG_SCBS(hhcb) SCSI_WADV_MAX_NONTAG_SCBS
#if SCSI_TARGET_OPERATION
#define  SCSI_rGETESTSCB(hhcb,hiob)  SCSIrSwappingGetEstScb((hhcb),(hiob))
#define  SCSI_rRETURNESTSCB(hhcb,hiob)  SCSIrSwappingReturnEstScb((hhcb),(hiob))
#endif /* SCSI_TARGET_OPERATION */
#endif

#if SCSI_STANDARD_160M_MODE
#define  SCSI_rREMOVESCBS(hhcb) SCSIrStandardRemoveScbs((hhcb))
#define  SCSI_rGETSCB(hhcb,hiob) SCSIrGetScb((hhcb),(hiob))
#define  SCSI_rRETURNSCB(hhcb,hiob) SCSIrReturnScb((hhcb),(hiob))
#define  SCSI_rHOSTQUEUEREMOVE(hhcb,scbNumber) SCSIrHostQueueRemove((hhcb),(scbNumber))
#define  SCSI_rMAX_NONTAG_SCBS(hhcb) SCSI_S160M_MAX_NONTAG_SCBS
#if SCSI_TARGET_OPERATION
#define  SCSI_rGETESTSCB(hhcb,hiob)  
#define  SCSI_rRETURNESTSCB(hhcb,hiob)
#endif /* SCSI_TARGET_OPERATION */
#endif

#if SCSI_SWAPPING_160M_MODE
#if SCSI_TARGET_OPERATION
#define  SCSI_rREMOVESCBS(hhcb)  SCSIrSwappingRemoveScbs((hhcb))
#else
#define  SCSI_rREMOVESCBS(hhcb)
#endif /* SCSI_TARGET_OPERATION */
#define  SCSI_rGETSCB(hhcb,hiob) SCSIrGetScb((hhcb),(hiob))
#define  SCSI_rRETURNSCB(hhcb,hiob) SCSIrReturnScb((hhcb),(hiob))
#define  SCSI_rHOSTQUEUEREMOVE(hhcb,scbNumber) SCSIrHostQueueRemove((hhcb),(scbNumber))
#define  SCSI_rMAX_NONTAG_SCBS(hhcb) SCSI_W160M_MAX_NONTAG_SCBS
#if SCSI_TARGET_OPERATION
#define  SCSI_rGETESTSCB(hhcb,hiob)  SCSIrSwappingGetEstScb((hhcb),(hiob))
#define  SCSI_rRETURNESTSCB(hhcb,hiob)  SCSIrSwappingReturnEstScb((hhcb),(hiob))
#endif /* SCSI_TARGET_OPERATION */
#endif
#endif

/***************************************************************************
* Function prototype internal to resource management layer
***************************************************************************/
/* RSMINIT.C */
void SCSIrReset(SCSI_HHCB SCSI_HPTR);
void SCSIrStandardRemoveScbs (SCSI_HHCB SCSI_HPTR);
/* SCSI_TARGET_OPERATION prototypes */
void SCSIrSwappingRemoveScbs(SCSI_HHCB SCSI_HPTR);
void SCSIrInitializeNexusQueue(SCSI_HHCB SCSI_HPTR);
void SCSIrInitializeNodeQueue(SCSI_HHCB SCSI_HPTR);

/* RSMTASK.C */
void SCSIrCompleteHIOB(SCSI_HIOB SCSI_IPTR);
void SCSIrCompleteSpecialHIOB(SCSI_HIOB SCSI_IPTR);
void SCSIrAsyncEvent(SCSI_HHCB SCSI_HPTR,SCSI_UINT16,...);
void SCSIrScsiBusReset(SCSI_HIOB SCSI_IPTR);
void SCSIrBusDeviceReset(SCSI_HIOB SCSI_IPTR);
void SCSIrAbortTask(SCSI_HIOB SCSI_IPTR);
void SCSIrAbortTaskSet(SCSI_HIOB SCSI_IPTR);
SCSI_SCB_DESCRIPTOR SCSI_HPTR SCSIRGetFreeHead(SCSI_HHCB SCSI_HPTR);
void SCSIrAsyncEventCommand(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UINT16,...);

#if SCSI_TARGET_OPERATION   
SCSI_SCB_DESCRIPTOR SCSI_HPTR SCSIRGetFreeEstHead(SCSI_HHCB SCSI_HPTR);
void SCSIrAbortNexus(SCSI_HIOB SCSI_IPTR);
SCSI_NODE SCSI_NPTR SCSIrAllocateNode(SCSI_HHCB SCSI_HPTR);
SCSI_HIOB SCSI_IPTR  SCSIrHiobPoolRemove(SCSI_HHCB SCSI_HPTR);
void SCSIrReturnToHiobPool(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_NEXUS SCSI_XPTR SCSIrAllocateNexus(SCSI_HHCB SCSI_HPTR);
void SCSIrFreeNexus(SCSI_HHCB SCSI_HPTR,SCSI_NEXUS SCSI_XPTR);  
SCSI_UINT8 SCSIrClearNexus(SCSI_NEXUS SCSI_XPTR,SCSI_HHCB SCSI_HPTR);
void SCSIrQueueHIOBPool(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIrFindAndAbortTargetHIOBs(SCSI_HHCB SCSI_HPTR,SCSI_NEXUS SCSI_XPTR,SCSI_UEXACT8);
void SCSIrTargetTaskManagementRequest(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_NEXUS SCSI_XPTR);
void SCSIrQueueEstHiobPool(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIrQueueEstHiobToHwm(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIrQueueEstHiobs(SCSI_HHCB SCSI_HPTR);
#endif /* SCSI_TARGET_OPERATION */ 

/* RSMUTIL.C */
void SCSIrCheckCriteria(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIrAllocateScb(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIrGetScb(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIrReturnScb(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIrDeviceQueueAppend(SCSI_TARGET_CONTROL SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_HIOB SCSI_IPTR SCSIrDeviceQueueRemove(SCSI_TARGET_CONTROL SCSI_UPTR);
SCSI_UEXACT8 SCSIrAbortHIOBInDevQ(SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
void SCSIrClearDeviceQueue(SCSI_TARGET_CONTROL SCSI_HPTR,SCSI_UEXACT8);
void SCSIrClearAllDeviceQueues(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIrHostQueueAppend(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_HIOB SCSI_IPTR SCSIrHostQueueRemove(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
SCSI_UEXACT8 SCSIrAbortHIOBInHostQ(SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
void SCSIrFindAndAbortHIOB(SCSI_HHCB SCSI_HPTR,SCSI_UNIT_CONTROL SCSI_UPTR,SCSI_UEXACT8);
void SCSIrClearHostQueue(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIrFreeScb(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIrFreeCriteria(SCSI_HIOB SCSI_IPTR);
void SCSIrPostAbortedHIOB(SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
void SCSIrPostDoneQueue(SCSI_HHCB SCSI_HPTR);
void SCSIrAdjustQueue(SCSI_HIOB SCSI_IPTR SCSI_IPTR,SCSI_HIOB SCSI_IPTR SCSI_IPTR,
                      SCSI_HIOB SCSI_IPTR SCSI_IPTR,SCSI_HIOB SCSI_IPTR);
void SCSIrFinishQueueFull(SCSI_HIOB SCSI_IPTR);
void SCSIrFreezeAllDeviceQueues(SCSI_HHCB SCSI_HPTR);
void SCSIrUnFreezeAllDeviceQueues(SCSI_HHCB SCSI_HPTR);
void SCSIrFreezeDeviceQueue(SCSI_TARGET_CONTROL SCSI_HPTR, SCSI_UINT8);
void SCSIrUnFreezeDeviceQueue(SCSI_TARGET_CONTROL SCSI_HPTR, SCSI_UINT8);
void SCSIrDispatchQueuedHIOB(SCSI_HIOB SCSI_IPTR);

#if SCSI_TARGET_OPERATION
SCSI_UEXACT8 SCSIrAbortHIOBInHiobQ(SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
void SCSIrSwappingReturnEstScb(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIrSwappingGetEstScb(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
#endif /* SCSI_TARGET_OPERATION */

/*$Header: /home/CVSROOT/sw/srmconsole/5.8/SRC/AIC78XX_HIM_RSM.H,v 1.2 2001/01/13 05:23:43 chris Exp $*/
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
*  Module Name:   RSMDEF.H
*
*  Description:   Definitions for resource management layer
*    
*  Owners:        ECX IC Firmware Team
*    
*  Notes:         1. This file must is referenced by resource
*                    management layer and applications to resource
*                    management layer
*
***************************************************************************/

/***************************************************************************
* Function prototype for tools referenced in RSM
***************************************************************************/

/***************************************************************************
* Function prototype as interface to resource management layer 
***************************************************************************/
/* RSMINIT.C */
int SCSIRGetConfiguration (SCSI_HHCB SCSI_HPTR);
void SCSIRGetMemoryTable(SCSI_UEXACT8,SCSI_UEXACT8,SCSI_UEXACT16,SCSI_UEXACT8,SCSI_MEMORY_TABLE SCSI_HPTR);
SCSI_INT SCSIRApplyMemoryTable (SCSI_HHCB SCSI_HPTR,SCSI_MEMORY_TABLE SCSI_HPTR);
int SCSIRInitialize (SCSI_HHCB SCSI_HPTR);

/* RSMTASK.C */
void SCSIRSetUnitHandle (SCSI_HHCB SCSI_HPTR,SCSI_UNIT_CONTROL SCSI_UPTR,SCSI_UEXACT8,SCSI_UEXACT8);
void SCSIRFreeUnitHandle (SCSI_UNIT_HANDLE);
void SCSIRQueueHIOB (SCSI_HIOB SCSI_IPTR);
void SCSIRQueueSpecialHIOB(SCSI_HIOB SCSI_IPTR);
int SCSIRBackEndISR(SCSI_HHCB SCSI_HPTR);
int SCSIRCheckHostIdle(SCSI_HHCB SCSI_HPTR);
int SCSIRCheckDeviceIdle(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
SCSI_UINT SCSIRPowerManagement( SCSI_HHCB SCSI_HPTR,SCSI_UINT);
void SCSIRGetHWInformation(SCSI_HW_INFORMATION SCSI_LPTR,SCSI_HHCB SCSI_HPTR);
void SCSIRPutHWInformation(SCSI_HW_INFORMATION SCSI_LPTR,SCSI_HHCB SCSI_HPTR);
