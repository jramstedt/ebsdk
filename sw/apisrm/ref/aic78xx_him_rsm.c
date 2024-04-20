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
 * Abstract:	Adaptec AIC-78xx HIM Layer
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *	sfs	27-Mar-1998	Initial entry.
 */

#include "cp$src:aic78xx_def.h"
#include "cp$src:aic78xx_scsi.h"
#include "cp$src:aic78xx_him_hwm.h"
#include "cp$src:aic78xx_him_rsm.h"

/*$Header:   Y:/source/chimscsi/src/himscsi/RSMINIT.CV_   1.16.14.0   13 Nov 1997 18:07:48   MAKI1985  $*/
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
*  Module Name:   RSMINIT.C
*
*  Description:
*                 Codes to initialize resource management layer
*
*  Owners:        ECX IC Firmware Team
*    
*  Notes:         NONE
*
*  Entry Point(s):
*                 SCSIRGetConfiguration
*                 SCSIRGetMemoryTable
*                 SCSIRApplyMemoryTable
*                 SCSIRInitialize
*
***************************************************************************/

#if 0
#include "scsi.h"	/* exclude */
#include "rsm.h"	/* exclude */
#endif

/*********************************************************************
*
*  SCSIRGetConfiguration
*
*     Get default configuration information
*
*  Return Value:  status
*                 0 - configuration available
*                  
*  Parameters:    hhcb
*
*  Remarks:       THis routine requires SCSIRegister valid in
*                 order to access the hardware. This routine
*                 may also access pci configuration space in 
*                 order to collect configuration information.
*
*********************************************************************/
int SCSIRGetConfiguration (SCSI_HHCB SCSI_HPTR hhcb)
{
   /* get configuration information filled by hardware management */
   SCSIHGetConfiguration(hhcb);

   hhcb->maxNonTagScbs = SCSI_rMAX_NONTAG_SCBS(hhcb);
   hhcb->maxTagScbs = 4;

   return(0);
}

/*********************************************************************
*
*  SCSIRGetMemoryTable
*
*     This routine will collect memory requirement information and 
*     fill the memory table.
*
*  Return Value:  none
*                  
*  Parameters:    firmwareMode
*                 numberScbs
*                 numberNexusHandles
*                 numberNodeHandles
*                 memoryTable
*
*  Remarks:       This routine may get called before/after 
*                 SCSIRGetConfiguration. It requires firmwareMode set
*                 with intended operating mode before calling
*                 this routine.
*
*                 This routine should not assume harwdare can be
*                 accessed. (e.g. SCSIRegister may not be valid
*                 when this routine get called.
*                 
*********************************************************************/
void SCSIRGetMemoryTable (SCSI_UEXACT8 firmwareMode,
                          SCSI_UEXACT8 numberScbs,
                          SCSI_UEXACT16 numberNexusHandles,
                          SCSI_UEXACT8 numberNodeHandles,
                          SCSI_MEMORY_TABLE SCSI_HPTR memoryTable)
{
   /* get memory table filled by hardware management */
   SCSIHGetMemoryTable(firmwareMode,numberScbs,memoryTable);

   memoryTable->memory[SCSI_HM_FREEQUEUE].memoryType = SCSI_MT_HPTR;
   memoryTable->memory[SCSI_HM_FREEQUEUE].memoryCategory = SCSI_MC_UNLOCKED;
   memoryTable->memory[SCSI_HM_FREEQUEUE].memoryAlignment = (SCSI_UEXACT8)0x03;
   memoryTable->memory[SCSI_HM_FREEQUEUE].minimumSize = 
      memoryTable->memory[SCSI_HM_FREEQUEUE].memorySize =
         sizeof(SCSI_SCB_DESCRIPTOR) * numberScbs;
   memoryTable->memory[SCSI_HM_FREEQUEUE].granularity = 0;

   memoryTable->memory[SCSI_HM_DONEQUEUE].memoryType = SCSI_MT_HPTR;
   memoryTable->memory[SCSI_HM_DONEQUEUE].memoryCategory = SCSI_MC_UNLOCKED;
   memoryTable->memory[SCSI_HM_DONEQUEUE].memoryAlignment = (SCSI_UEXACT8)0x03;
   memoryTable->memory[SCSI_HM_DONEQUEUE].minimumSize = 
      memoryTable->memory[SCSI_HM_DONEQUEUE].memorySize =
         sizeof(SCSI_HIOB SCSI_IPTR) * numberScbs;
   memoryTable->memory[SCSI_HM_DONEQUEUE].granularity = 0;

#if SCSI_TARGET_OPERATION  
   memoryTable->memory[SCSI_HM_NEXUSQUEUE].memoryType = SCSI_MT_HPTR; /*SCSI_MT_XPTR??*/;
   memoryTable->memory[SCSI_HM_NEXUSQUEUE].memoryCategory = SCSI_MC_UNLOCKED;
   memoryTable->memory[SCSI_HM_NEXUSQUEUE].memoryAlignment = (SCSI_UEXACT8)0x03;
   memoryTable->memory[SCSI_HM_NEXUSQUEUE].minimumSize = 
      memoryTable->memory[SCSI_HM_NEXUSQUEUE].memorySize = 
         sizeof(SCSI_NEXUS) * numberNexusHandles; 
   memoryTable->memory[SCSI_HM_NEXUSQUEUE].granularity = 0;
  
   memoryTable->memory[SCSI_HM_NODEQUEUE].memoryType = SCSI_MT_HPTR; /*SCSI_MT_NPTR??*/;
   memoryTable->memory[SCSI_HM_NODEQUEUE].memoryCategory = SCSI_MC_UNLOCKED;
   memoryTable->memory[SCSI_HM_NODEQUEUE].memoryAlignment = (SCSI_UEXACT8)0x03;
   memoryTable->memory[SCSI_HM_NODEQUEUE].minimumSize = 
      memoryTable->memory[SCSI_HM_NODEQUEUE].memorySize = 
         sizeof(SCSI_NODE) * numberNodeHandles; 
   memoryTable->memory[SCSI_HM_NODEQUEUE].granularity = 0; /* still to add the node table stuff */
#endif /* SCSI_TARGET_OPERATION */ 
}

/*********************************************************************
*
*  SCSIRApplyMemoryTable
*
*     This routine will apply memory pointers described in memory table.
*
*  Return Value:  0 - memory table get applied
*                 1 - memory table not applicable
*                  
*  Parameters:    hhcb
*                 memoryTable
*
*  Remarks:       Memory pointers will be applied and saved in 
*                 associated hhcb. After this call the memory table 
*                 is not required (e.g. can be free) from now on.
*                 
*                 Memory pointers in memory table must be setup
*                 properly to satify the memory requirement 
*                 before this routine get called.
*
*********************************************************************/
SCSI_INT SCSIRApplyMemoryTable (SCSI_HHCB SCSI_HPTR hhcb,
                                SCSI_MEMORY_TABLE SCSI_HPTR memoryTable)
{
   hhcb->SCSI_DONE_QUEUE = (SCSI_HIOB SCSI_IPTR SCSI_HPTR)
                        memoryTable->memory[SCSI_HM_DONEQUEUE].ptr.hPtr;

   /* set memory pointer for free scb queue */
   hhcb->SCSI_FREE_QUEUE = (SCSI_SCB_DESCRIPTOR SCSI_HPTR)
                        memoryTable->memory[SCSI_HM_FREEQUEUE].ptr.hPtr;
#if SCSI_TARGET_OPERATION 
   /* Assign Nexus Queue */
   hhcb->SCSI_HF_targetNexusQueue = (SCSI_NEXUS SCSI_XPTR) 
                        memoryTable->memory[SCSI_HM_NEXUSQUEUE].ptr.hPtr;
   /* Assign Node Queue */
   hhcb->SCSI_HF_targetNodeQueue = (SCSI_NODE SCSI_NPTR)
                        memoryTable->memory[SCSI_HM_NODEQUEUE].ptr.hPtr;
#endif /* SCSI_TARGET_OPERATION */

   return(SCSIHApplyMemoryTable(hhcb,memoryTable));
}

/*********************************************************************
*
*  SCSIRInitialize
*
*     Initialize hardware based on configuration information 
*     described 
*
*  Return Value:  return status
*                 0 - initialization successful
*                  
*  Parameters:    hhcb
*
*  Remarks:       Usually this routine is called after SCSIRGetConfiguration,
*                 SCSIRGetMemoryTable and SCSIRSetMemoryTable. It requires 
*                 both configuration and memory available in order to 
*                 initialize hardware properly.
*                 
*********************************************************************/
int SCSIRInitialize (SCSI_HHCB SCSI_HPTR hhcb)
{
   int status;

   /* general reset */
   SCSIrReset(hhcb);

   /* initialize hardware management layer */
   if ((status = SCSIHInitialize(hhcb)))
   {
      return(status);
   }
   return(0);
}
                     
/*********************************************************************
*
*  SCSIrReset
*
*     Reset for resource management layer software
*
*  Return Value:  none
*                  
*  Parameters:    hhcb
*
*  Remarks:
*                  
*********************************************************************/
void SCSIrReset (SCSI_HHCB SCSI_HPTR hhcb)
{
   SCSI_INT i;
   SCSI_TARGET_CONTROL SCSI_HPTR targetControl;
   SCSI_UEXACT8 SCSI_HPTR virtual_Address;
   SCSI_BUS_ADDRESS busAddress;
#if SCSI_DMA_SUPPORT
   SCSI_INT maxDev = SCSI_MAXDEV+1;
#else
   SCSI_INT maxDev = SCSI_MAXDEV;
#endif /* SCSI_DMA_SUPPORT */


#if SCSI_TARGET_OPERATION
   if (hhcb->SCSI_HF_targetMode)
   {
      /* Initialize some target mode structures */
      /* Initialize nexus pointers */ 
      SCSIrInitializeNexusQueue(hhcb);
      /* Initialize node pointers */
      SCSIrInitializeNodeQueue(hhcb);
   }
#endif /* SCSI_TARGET_OPERATION */

   /* initialize done queue */
   hhcb->SCSI_DONE_HEAD = hhcb->SCSI_DONE_TAIL = 0;

   /* initialize scb waiting queue */
   hhcb->SCSI_WAIT_HEAD = hhcb->SCSI_WAIT_TAIL = SCSI_NULL_HIOB;

   hhcb->SCSI_FREE_SCB_AVAIL = hhcb->numberScbs;
   hhcb->SCSI_FREE_HEAD = &hhcb->SCSI_FREE_QUEUE[0];
   hhcb->SCSI_FREE_TAIL = &hhcb->SCSI_FREE_QUEUE[hhcb->numberScbs - 1];
   for (i=0;i<(SCSI_INT)(hhcb->numberScbs);i++)
   {
      hhcb->SCSI_FREE_QUEUE[i].scbNumber = i;
      virtual_Address = (SCSI_UEXACT8 SCSI_HPTR)hhcb->SCSI_SCB_BUFFER.virtualAddress;
      hhcb->SCSI_FREE_QUEUE[i].scbBuffer.virtualAddress = &(virtual_Address[i * 
            SCSI_hSIZEOFSCBBUFFER(hhcb->firmwareMode)]);
      busAddress = OSD_GET_BUS_ADDRESS(hhcb,
            SCSI_MC_LOCKED,hhcb->SCSI_SCB_BUFFER.virtualAddress);
      OSD_ADJUST_BUS_ADDRESS(hhcb,&(busAddress),
            (i * SCSI_hSIZEOFSCBBUFFER(hhcb->firmwareMode)));
      hhcb->SCSI_FREE_QUEUE[i].scbBuffer.busAddress = busAddress;
      hhcb->SCSI_FREE_QUEUE[i].scbBuffer.bufferSize = 
            SCSI_hSIZEOFSCBBUFFER(hhcb->firmwareMode);
      if (i+1 != hhcb->numberScbs)
      {
         hhcb->SCSI_FREE_QUEUE[i].queueNext = &hhcb->SCSI_FREE_QUEUE[i+1];
      }
   }
   hhcb->SCSI_FREE_QUEUE[hhcb->numberScbs - 1].queueNext = 
         &hhcb->SCSI_FREE_QUEUE[0];

   SCSI_rREMOVESCBS(hhcb);

   /* initialize the target control table */
   for (i = 0,targetControl = hhcb->SCSI_HP_targetControl; i < maxDev;
      i++,targetControl++)
   {
      targetControl->maxNonTagScbs = (SCSI_UINT16) hhcb->maxNonTagScbs;
      targetControl->maxTagScbs = (SCSI_UINT16) hhcb->maxTagScbs;
      targetControl->queueHead = targetControl->queueTail = SCSI_NULL_HIOB;
      targetControl->activeScbs = 0;
      targetControl->origMaxNonTagScbs = 0;
      targetControl->origMaxTagScbs = 0;
      targetControl->activeBDRs = 0;
      targetControl->activeABTTSKSETs = 0;
      targetControl->freezeMap = 0;
      targetControl->freezeTail = SCSI_NULL_HIOB;
      targetControl->activeHiPriScbs = 0;
      targetControl->deviceQueueHiobs = 0;
#if SCSI_NEGOTIATION_PER_IOB
      targetControl->pendingNego = 0;
#endif
   }
#if SCSI_DMA_SUPPORT
   /* initialize targetControl reserved for */
   /* DMA I/Os                              */ 
   targetControl--;
   /* targetControl is at the last unit     */ 
   /* allow as many active I/Os as scbs available to DMA unit */
   targetControl->maxNonTagScbs = (SCSI_UINT16) hhcb->SCSI_FREE_SCB_AVAIL;
   targetControl->maxTagScbs = (SCSI_UINT16) hhcb->SCSI_FREE_SCB_AVAIL;
#endif /* SCSI_DMA_SUPPORT */

}

/*********************************************************************
*
*  SCSIrStandardRemoveScbs
*
*     Remove Scbs for standard modes
*
*  Return Value:  none
*                  
*  Parameters:    hhcb
*
*  Remarks:
*                  
*********************************************************************/
#if SCSI_STANDARD_MODE
void SCSIrStandardRemoveScbs (SCSI_HHCB SCSI_HPTR hhcb)
{

#if SCSI_ARO_SUPPORT
   /* remove scb #0 from linked list since xor chim will use scb #0 */
   hhcb->SCSI_FREE_SCB_AVAIL--;
   hhcb->SCSI_FREE_HEAD = &hhcb->SCSI_FREE_QUEUE[1];
   hhcb->SCSI_FREE_QUEUE[hhcb->numberScbs - 1].queueNext = 
         &hhcb->SCSI_FREE_QUEUE[1];
#endif /* SCSI_ARO_SUPPORT */

   /* remove scb #2 from linked list */
   hhcb->SCSI_FREE_QUEUE[1].queueNext = &hhcb->SCSI_FREE_QUEUE[3];
   hhcb->SCSI_FREE_SCB_AVAIL--;

}
#endif /* SCSI_STANDARD_MODE */

#if SCSI_TARGET_OPERATION
/*********************************************************************
*
*  SCSIrSwappingRemoveScbs
*
*     Remove Scbs for swapping modes
*
*  Return Value:  none
*                  
*  Parameters:    hhcb
*
*  Remarks: 
*    
*     Currently, only used for target mode to remove scbs to be 
*     reserved for Establish Connection SCBs (i.e. for use with 
*     Establish Connection IOBs).
*                  
*********************************************************************/
#if SCSI_SWAPPING_ADVANCED_MODE || SCSI_SWAPPING_160M_MODE
void SCSIrSwappingRemoveScbs (SCSI_HHCB SCSI_HPTR hhcb)
{

   SCSI_UEXACT8 numberOfEstScbs; /* number of SCBs reserved establish connection
                                  * SCBs
                                  */
   SCSI_UEXACT8 numberScbs;      /* total number of scbs available */

   if (hhcb->SCSI_HF_targetMode)
   {
      /* alter the links and create a queue of establish connection SCBs */
      numberOfEstScbs = hhcb->SCSI_HF_targetNumberEstScbs;
      numberScbs = hhcb->numberScbs;

       /* Initialize Establish Connection SCB Pool */
      hhcb->SCSI_FREE_EST_SCB_AVAIL = numberOfEstScbs;
      hhcb->SCSI_FREE_EST_HEAD = &hhcb->SCSI_FREE_QUEUE[numberScbs-numberOfEstScbs];
      hhcb->SCSI_FREE_EST_TAIL = &hhcb->SCSI_FREE_QUEUE[numberScbs - 1];

      /* Adjust the available scb count */
      hhcb->SCSI_FREE_SCB_AVAIL -= numberOfEstScbs;

      /* Adjust the links of available scbs */
      hhcb->SCSI_FREE_TAIL = &hhcb->SCSI_FREE_QUEUE[numberScbs - numberOfEstScbs - 1];
      hhcb->SCSI_FREE_QUEUE[numberScbs - numberOfEstScbs - 1].queueNext = 
         &hhcb->SCSI_FREE_QUEUE[0];    
      
      /* Set up the links of the establish connection scbs */
      hhcb->SCSI_FREE_QUEUE[hhcb->numberScbs - 1].queueNext = 
            &hhcb->SCSI_FREE_QUEUE[numberScbs - numberOfEstScbs];
 
   }

}
#endif /* SCSI_SWAPPING_ADVANCED_MODE || SCSI_SWAPPING_160M_MODE */
/*********************************************************************
*
*  SCSIrInitializeNexusQueue
*
*     Initialize the Nexus Queue  
*
*  Return Value:  none
*                  
*  Parameters:    hhcb
*
*  Remarks:
*                  
*********************************************************************/
void SCSIrInitializeNexusQueue(SCSI_HHCB SCSI_HPTR hhcb)
{

   SCSI_NEXUS SCSI_XPTR nexus;
   SCSI_UEXACT16 i;
   SCSI_UEXACT16 nexusHandleCnt = hhcb->SCSI_HF_targetNumberNexusHandles; 
   
   for (i=1,nexus = hhcb->SCSI_HF_targetNexusQueue; i < nexusHandleCnt;
        i++,nexus++)
   {
      nexus->SCSI_XF_available = 1; /* mark nexus available */
      nexus->nextNexus = nexus+1;
   }

   if (nexusHandleCnt != 0)
   {
      nexus->nextNexus = SCSI_NULL_NEXUS;
   }
   else
   {
      hhcb->SCSI_HF_targetNexusQueue = SCSI_NULL_NEXUS;
   }
       
   /* set available count */
   hhcb->SCSI_HF_targetNexusQueueCnt = nexusHandleCnt;
   
}

/*********************************************************************
*
*  SCSIrInitializeNodeQueue
*
*     Initialize the Node Queue  
*
*  Return Value:  none
*                  
*  Parameters:    hhcb
*
*  Remarks:
*     The node queue contains a pool of available node structures which
*     are requested by the HWM when a selection of a previously unknown
*     initiator occurs. The size of this pool is dependent on the value
*     of targetNumNodeTaskSetHandles.    
*                  
*********************************************************************/
void SCSIrInitializeNodeQueue(SCSI_HHCB SCSI_HPTR hhcb)
{

   SCSI_NODE SCSI_NPTR node;
   SCSI_UEXACT16 i;
   SCSI_UEXACT16 nodeHandleCnt = hhcb->SCSI_HF_targetNumberNodeHandles; 
   
   for (i=1,node = hhcb->SCSI_HF_targetNodeQueue; i < nodeHandleCnt;
        i++,node++)
   {
      node->nextNode = node+1;
   }

   if (nodeHandleCnt != 0)
   {
      node->nextNode = SCSI_NULL_NODE;
   }
   else
   {
      hhcb->SCSI_HF_targetNodeQueue = SCSI_NULL_NODE;
   }
     
}
#endif /* SCSI_TARGET_OPERATION */

/*$Header:   Y:/source/chimscsi/src/himscsi/RSMTASK.CV_   1.32.12.1   25 Nov 1997 19:24:14   RAME3369  $*/
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
*  Module Name:   RSMTASK.C
*
*  Description:
*                 Codes to implement resource management layer
*
*  Owners:        ECX IC Firmware Team
*    
*  Notes:         NONE
*
*  Entry Point(s):
*                 SCSIRSetUnitHandle
*                 SCSIRFreeUnitHandle
*                 SCSIRQueueHIOB
*                 SCSIRQueueSpecialHIOB
*                 SCSIRFrontEndISR
*                 SCSIRBackEndISR
*
***************************************************************************/

#if 0
#include "scsi.h"	/* exclude */
#include "rsm.h"	/* exclude */
#endif

/*********************************************************************
*
*  SCSIRSetUnitHandle
*
*     Validate unit handle for specified device control block
*
*  Return Value:  none
*                  
*  Parameters:    hhcb
*                 target unit control block 
*                 scsi id
*                 logical unit number
*
*  Remarks:       Validated device handle is used to indicate HIOB
*                 destination. After protocol automatic configuration
*                 OSD may examine device table (embedded in SCSI_HHCB)
*                 and find out the device/unit it interested in. This
*                 routine is for OSD to acquire a unit handle which
*                 is required fir building SCSI_HIOB. After protocol
*                 automatic configuration all unit handles become
*                 inlidated automatically.
*                  
*********************************************************************/
void SCSIRSetUnitHandle (SCSI_HHCB SCSI_HPTR hhcb,
                         SCSI_UNIT_CONTROL SCSI_UPTR targetUnit,
                         SCSI_UEXACT8 scsiID,
                         SCSI_UEXACT8 luNumber)
{
   /* inform hardware management */
   SCSIHSetUnitHandle(hhcb,targetUnit,scsiID,luNumber);

   /* setup pointer to target control */
   targetUnit->targetControl = &hhcb->SCSI_HP_targetControl[scsiID];
}

/*********************************************************************
*
*  SCSIRFreeUnitHandle
*
*     Invalidate specified unit handle
*
*  Return Value:  none
*                  
*  Parameters:    unit handle
*
*  Remarks:       After invalidated the unit handle should not
*                 be used in HIOB. Whenever there is a validated unit
*                 handle which is not in OSD's interest it can
*                 invalidate unit handle through this routine.
*                  
*********************************************************************/
void SCSIRFreeUnitHandle (SCSI_UNIT_HANDLE unitHandle)
{
   /* inform hardware management */
   SCSIHFreeUnitHandle(unitHandle);
}

/*********************************************************************
*
*  SCSIRQueueHIOB 
*
*     Queue iob to resource management queues
*
*  Return Value:  none
*                  
*  Parameters:    hiob
*
*  Remarks:
*                  
*********************************************************************/
#if !SCSI_STREAMLINE_QIOPATH
void SCSIRQueueHIOB (SCSI_HIOB SCSI_IPTR hiob)
{
   SCSI_HHCB SCSI_HPTR hhcb = SCSI_TARGET_UNIT(hiob)->hhcb;

   /* fresh the hiob status and working area */
   SCSI_rFRESH_HIOB(hiob);

   /* start with checking criteria (max tags/nontags) */
   SCSIrCheckCriteria(hhcb,hiob);
}
#endif /* !SCSI_STREAMLINE_QIOPATH */

/*********************************************************************
*
*  SCSIRQueueSpecialHIOB
*
*     Queue special iob to special resource management queue
*
*  Return Value:  none
*                  
*  Parameters:    hiob
*
*  Remarks:       This routine will process all special request
*                 through HIOB.
*                  
*********************************************************************/
void SCSIRQueueSpecialHIOB (SCSI_HIOB SCSI_IPTR hiob)
{
#if SCSI_TARGET_OPERATION 
   SCSI_HHCB SCSI_HPTR hhcb;
   SCSI_NEXUS SCSI_XPTR nexus;
#endif /* SCSI_TARGET_OPERATION */
#if SCSI_DMA_SUPPORT
   SCSI_TARGET_CONTROL SCSI_HPTR targetControl;
   SCSI_UINT16 maxScbs;
#endif /* SCSI_DMA_SUPPORT */

   /* fresh the hiob status and working area */
   SCSI_rFRESH_HIOB(hiob);

   switch(hiob->cmd)
   {
      case SCSI_CMD_ABORT_TASK:
         SCSIrAbortTask(hiob);
         break;

      case SCSI_CMD_ABORT_TASK_SET:
         SCSIrAbortTaskSet(hiob);
         break;

      case SCSI_CMD_RESET_BUS:
         SCSIrScsiBusReset(hiob);
         break;

      case SCSI_CMD_RESET_TARGET:
         SCSIrBusDeviceReset(hiob);
         break;

      case SCSI_CMD_RESET_BOARD:
      case SCSI_CMD_RESET_HARDWARE:
         /* We'll not do anything at this time. */
         /* Just set success status and post back. */
         hiob->stat = SCSI_SCB_COMP;
         SCSIrCompleteSpecialHIOB(hiob);
         break;

      case SCSI_CMD_PROTO_AUTO_CFG:
         /* let hardware management layer to take it over */
         SCSIHQueueSpecialHIOB(hiob);
         break;

      case SCSI_CMD_UNFREEZE_QUEUE:
         /* Unfreeze the device queue and dispatch any queued HIOBs */
         /* if the queue is not frozen. */
         SCSIrUnFreezeDeviceQueue(SCSI_TARGET_UNIT(hiob)->targetControl,
                                  SCSI_FREEZE_ERRORBUSY);
         SCSIrDispatchQueuedHIOB(hiob);

         /* Set success status and post back. */
         hiob->stat = SCSI_SCB_COMP;
         SCSIrCompleteSpecialHIOB(hiob);
         break;

#if SCSI_DMA_SUPPORT
      case SCSI_CMD_INITIATE_DMA_TASK:
         /* get the criteria factor */
         targetControl = SCSI_TARGET_UNIT(hiob)->targetControl;
         maxScbs = ((hiob->SCSI_IF_tagEnable) ? targetControl->maxTagScbs :
                                                targetControl->maxNonTagScbs);

         if (++targetControl->activeScbs <= maxScbs)
         {
            /* criteria met, continue with allocating scb */
            SCSIrAllocateScb(SCSI_TARGET_UNIT(hiob)->hhcb,hiob);
         }
         else
         {
            /* criteria not met, just append it to device queue */
            SCSIrDeviceQueueAppend(targetControl,hiob);
         }
         break;
#endif /* SCSI_DMA_SUPPORT */

#if SCSI_TARGET_OPERATION
      case SCSI_CMD_REESTABLISH_AND_COMPLETE:
      case SCSI_CMD_REESTABLISH_INTERMEDIATE:
         SCSI_rTARGETMODE_HIOB(hiob);
         nexus = SCSI_NEXUS_UNIT(hiob);
         hhcb = nexus->hhcb;
         
         /* indicate we have an active IOB for this nexus */
         /* currently we only allow one hiob per nexus */
         /* if we support more then change this to an 8 bit field */ 
         nexus->SCSI_XP_hiobActive = 1;
                  
         if (!(nexus->SCSI_XF_busHeld))
         {
            /* SCB needs to be allocated */
           
            if ((hiob->scbNumber = (SCSI_UEXACT8)SCSI_rGETSCB(hhcb,hiob)) != SCSI_NULL_SCB)  
            { 
               /* scb available, queue to hardware management layer */
               SCSIHQueueSpecialHIOB(hiob);
            }
            else
            {         
               /* queue it to host queue */
               SCSIrHostQueueAppend(hhcb,hiob);
            }      
         }
         else
         {  
            /* SCSI bus was held - just give to HWM */ 
            /* the scb was not freed because the bus was held */ 
            hiob->scbDescriptor = hhcb->SCSI_HP_busHeldScbDescriptor;
            hiob->scbNumber = (SCSI_UEXACT8)hiob->scbDescriptor->scbNumber;
            /* indicator for where to return scb */
            if (hhcb->SCSI_HP_estScb)
            {
               hiob->SCSI_IP_estScb = 1;
            }

            SCSIHQueueSpecialHIOB(hiob);
         }
         break;

      case SCSI_CMD_ESTABLISH_CONNECTION:
         /* Add this hiob (iob) to the available pool */
         SCSI_rTARGETMODE_HIOB(hiob);
         hhcb = SCSI_INITIATOR_UNIT(hiob);
         hiob->queueNext = SCSI_NULL_HIOB;
         if (hhcb->SCSI_HF_targetHoldEstHiobs)
         {
            /* Cannot queue to HWM layer */
            SCSIrQueueEstHiobPool(hhcb,hiob); 
         }
         else
         {
            SCSIrQueueEstHiobToHwm(hhcb,hiob);
         }
         break;

      case SCSI_CMD_ABORT_NEXUS:
         /* Abort any IOBs associated with this nexus and release the SCSI  */
         /* bus - if held.                                                  */
         /* Send request down to HWM layer.                                 */
         SCSIHQueueSpecialHIOB(hiob);
   
         /* Note that; the call to find HIOBs in the Host Q for this */
         /* nexus is made on the completion of HWM request (see      */
         /* SCSIrCompleteSpecialHIOB).                               */
         break;

#if SCSI_MULTIPLEID_SUPPORT
      case SCSI_CMD_ENABLE_ID:
      case SCSI_CMD_DISABLE_ID:
         /* Send request down to HWM layer. */
         SCSIHQueueSpecialHIOB(hiob); 
         break; 
#endif /* SCSI_MULTIPLEID_SUPPORT */

#endif /* SCSI_TARGET_OPERATION */

      default:
         break;
   }
}

/*********************************************************************
*
*  SCSIRBackEndISR
*
*     Back end processing of ISR
*
*  Return Value:  none
*                  
*  Parameters:    hhcb
*
*  Remarks:       This routine will actually handle interrupt event
*                 though it was cleared by SCSIHFrontEndISR.
*
*********************************************************************/
#if !SCSI_STREAMLINE_QIOPATH
int SCSIRBackEndISR (SCSI_HHCB SCSI_HPTR hhcb)
{
   SCSIHBackEndISR(hhcb);

   SCSIrPostDoneQueue(hhcb);

   return(0);
}
#endif /* !SCSI_STREAMLINE_QIOPATH */

/*********************************************************************
*
*  SCSIRCheckHostIdle
*
*     Get the Check host device is idle
*
*  Return Value:  1 - host device is idle
*                 0 - host is in idle busy
*                  
*  Parameters:    hhcb
*
*  Remarks:       This routine will actually handle interrupt event
*                 though it was cleared by SCSIHFrontEndISR.
*
*********************************************************************/
int SCSIRCheckHostIdle (SCSI_HHCB SCSI_HPTR hhcb)
{
   SCSI_UINT i;

   /* check if there is any outstanding request in criteria queue */
   if (hhcb->SCSI_WAIT_HEAD != SCSI_NULL_HIOB)
   {
      return(0);
   }

#if SCSI_TARGET_OPERATION
   if (hhcb->SCSI_HF_initiatorMode)
   {
#endif /* SCSI_TARGET_OPERATION */ 
      /* check if there is any outstanding request in resource queue */
      for (i = 0; i < SCSI_MAXDEV; i++)
      {
         if (hhcb->SCSI_HP_targetControl[i].queueHead != SCSI_NULL_HIOB)
         {
            return(0);
         }
      }
#if SCSI_TARGET_OPERATION
   }
#endif /* SCSI_TARGET_OPERATION */ 

   /* check if there is any outstanding request in hardware */
   /* management layer */
   return(SCSIHCheckHostIdle(hhcb));
}

/*********************************************************************
*
*  SCSIRCheckDeviceIdle
*
*     Get the check device is idle
*
*  Return Value:  1 - device is idle
*                 0 - device is not idle
*                  
*  Parameters:    hhcb
*                 target SCSI ID
*
*  Remarks:       This routine will check for any outstanding
*                 request of particular device ID in the host
*                 queue and device queue.  This routine should be
*                 called by the task that's running outside the
*                 command complete process such as
*                 SCSIAdjustTargetProfile routine.
*
*********************************************************************/
int SCSIRCheckDeviceIdle (SCSI_HHCB SCSI_HPTR hhcb,
                          SCSI_UEXACT8 targetID)
{
   SCSI_HIOB SCSI_IPTR hiob;

   hiob = hhcb->SCSI_WAIT_HEAD;

   /* Check if there is any outstanding request for this target */
   /* in the host queue */
   while (hiob != SCSI_NULL_HIOB)
   {
      if (SCSI_TARGET_UNIT(hiob)->scsiID == targetID)
      {
         return(0);
      }

      /* Next HIOB */
      hiob = hiob->queueNext;
   }

   /* Check if there is any outstanding request for this target */
   /* in the device queue */
   if (hhcb->SCSI_HP_targetControl[targetID].queueHead != SCSI_NULL_HIOB)
   {
      return(0);
   }

   /* Check the hardware management layer */
   return(SCSIHCheckDeviceIdle(hhcb, targetID));
}

/*********************************************************************
*
*  SCSIrCompleteHIOB
*
*     Process HIOB posted from hardware management layer
*
*  Return Value:  none
*                  
*  Parameters:    hiob
*
*  Remarks:
*
*********************************************************************/
void SCSIrCompleteHIOB (SCSI_HIOB SCSI_IPTR hiob)
{
   SCSI_HHCB SCSI_HPTR hhcb = SCSI_TARGET_UNIT(hiob)->hhcb;
   SCSI_TARGET_CONTROL SCSI_HPTR targetControl;
   
   targetControl = SCSI_TARGET_UNIT(hiob)->targetControl;

   if (hiob->stat == SCSI_SCB_FROZEN)
   {
      /* free scb associated */
      SCSIrFreeScb(hhcb,hiob);

      /* Queue the SCB back in the Device Queue */
      SCSIrDeviceQueueAppend(targetControl,hiob);
   }

   else
   {
      if (hiob->trgStatus == SCSI_UNIT_QUEFULL)
      {
         /* The queue has been already frozen by the hardware layer; just  */
         /* set the SCSI_FREEZE_QFUL bit in the freezemap variable         */
         SCSIrFreezeDeviceQueue(targetControl, SCSI_FREEZE_QFULL);
      }

      /* free scb associated */
      SCSIrFreeScb(hhcb,hiob);

      /* Put HIOB into Done queue and adjust Done queue pointer */
      hhcb->SCSI_DONE_QUEUE[hhcb->SCSI_DONE_HEAD++] = hiob;
      if (hhcb->SCSI_DONE_HEAD == hhcb->numberScbs)
      {
         hhcb->SCSI_DONE_HEAD = 0;
      }
   }
}

/*********************************************************************
*
*  SCSIrCompleteSpecialHIOB
*
*     Process special HIOB posted from hardware management layer
*
*  Return Value:  none
*                  
*  Parameters:    hiob
*
*  Remarks:
*
*********************************************************************/
void SCSIrCompleteSpecialHIOB (SCSI_HIOB SCSI_IPTR hiob)
{
   SCSI_HHCB SCSI_HPTR hhcb;
   SCSI_TARGET_CONTROL SCSI_HPTR targetControl;
#if SCSI_TARGET_OPERATION
   SCSI_UEXACT8 haStatus;
   SCSI_NEXUS SCSI_XPTR nexus;
   SCSI_HIOB SCSI_IPTR newHiob;
#endif /* SCSI_TARGET_OPERATION */

   switch (hiob->cmd)
   {
      case SCSI_CMD_ABORT_TASK:
         hhcb = SCSI_TARGET_UNIT(SCSI_RELATED_HIOB(hiob))->hhcb;
         SCSIrPostDoneQueue(hhcb);           /* Post back all HIOB(s) */
         break;

      case SCSI_CMD_ABORT_TASK_SET:
         hhcb = SCSI_TARGET_UNIT(hiob)->hhcb;
         targetControl = SCSI_TARGET_UNIT(hiob)->targetControl;

         /* Try to unfreeze the queue by clearing the ABTTSKSET freeze-bit */
         /* only if this is the last Abort_Task_Set returned from the HWM layer. */
         if (!(--targetControl->activeABTTSKSETs))
         {
            SCSIrUnFreezeDeviceQueue(targetControl, SCSI_FREEZE_ABTTSKSET);
            SCSIrDispatchQueuedHIOB(hiob);
         }

         SCSIrPostDoneQueue(hhcb);           /* Post back all HIOB(s) */
         break;

      case SCSI_CMD_RESET_BUS:
         hhcb = SCSI_INITIATOR_UNIT(hiob);
         SCSI_rUNFREEZEALLDEVICEQUEUES(hhcb);/* Unfreeze RSM Layer */
         /* queue Establish Connection HIOBs which 
          * may have been queued to HIOB queue during reset.
          */
         SCSI_rQUEUEESTHIOBS(hhcb);
         SCSIrPostDoneQueue(hhcb);           /* Post back all HIOB(s) */

         break;

      case SCSI_CMD_RESET_TARGET:
         hhcb = SCSI_TARGET_UNIT(hiob)->hhcb;
         targetControl = SCSI_TARGET_UNIT(hiob)->targetControl;

         /* Free scb associated.  This call will clear the queue full */
         /* condition as well if any.  But, the queue is still be frozen */
         /* due to the BDR freeze-bit has not been cleared.  Therefore, */
         /* the call to unfreeze the queue below will dispatch any queued */
         /* HIOB(s).  Normally, BDR is the last one return from HWM layer. */
         SCSIrFreeScb(hhcb,hiob);

         /* Try to unfreeze the queue by clearing the BDR freeze-bit */
         /* only if this is the last BDR returned from the HWM layer. */
         if (!(--targetControl->activeBDRs))
         {
            SCSIrUnFreezeDeviceQueue(targetControl, SCSI_FREEZE_BDR);
            SCSIrDispatchQueuedHIOB(hiob);
         }

         SCSIrPostDoneQueue(hhcb);           /* Post back all HIOB(s) */
         break;

#if SCSI_DMA_SUPPORT
      case SCSI_CMD_INITIATE_DMA_TASK:
         hhcb = SCSI_TARGET_UNIT(hiob)->hhcb;
         /* free scb associated */
         SCSIrFreeScb(hhcb,hiob);

         /* Put HIOB into Done queue and adjust Done queue pointer */
         hhcb->SCSI_DONE_QUEUE[hhcb->SCSI_DONE_HEAD++] = hiob;
         if (hhcb->SCSI_DONE_HEAD == hhcb->numberScbs)
         {
            hhcb->SCSI_DONE_HEAD = 0;
         }
         return;
#endif /* SCSI_DMA_SUPPORT */

#if SCSI_TARGET_OPERATION
      case SCSI_CMD_ESTABLISH_CONNECTION:
         
         /* All establish connection completions are posted to OSM     */
         /* immediately (Note; DONE queue size is equal to numberScbs, */
         /* and an SCB is not allocated for an establish connection    */
         /* IOB, so queueing to DONE queue could result in overflow.)  */ 
         hhcb = SCSI_NEXUS_UNIT(hiob)->hhcb;

         /* Was there an exception */     
         if (hiob->stat != SCSI_SCB_COMP &&
             hiob->stat != SCSI_TASK_CMD_COMP && 
             hiob->haStat != SCSI_HOST_TRUNC_CMD)
         {
            
            /* return est scb to available pool */
            SCSI_rRETURNESTSCB(hhcb,hiob);

            /* If an Establish Connection HIOB completes with an exception
             * other than stat = SCSI_TASK_CMD_COMP or haStat SCSI_HOST_TRUNC_CMD
             * then requeue an don't post to OSM.
             */
            if (hhcb->SCSI_HF_targetHoldEstHiobs)
            {
               SCSIrFreeNexus(hhcb,SCSI_NEXUS_UNIT(hiob));
               SCSI_rFRESH_HIOB(hiob);
               SCSI_rTARGETMODE_HIOB(hiob);
               SCSIrQueueEstHiobPool(hhcb,hiob);
            }
            else
            {
               SCSI_rFRESH_HIOB(hiob);
               SCSI_rTARGETMODE_HIOB(hiob);
               hiob->queueNext = SCSI_NULL_HIOB;
               /* assume est scb available as we just queued one */
               hiob->scbNumber = SCSI_rGETESTSCB(hhcb,hiob);
               SCSIHQueueSpecialHIOB(hiob); 
            }
         }
         else
         {
            if (SCSI_NEXUS_UNIT(hiob)->SCSI_XF_busHeld)
            {    
               /* Don't free establish connection scb - reuse for response. */ 
               /* save scbNumber and est scb indicator in hhcb */
               hhcb->SCSI_HP_busHeldScbDescriptor = hiob->scbDescriptor;
               hhcb->SCSI_HP_estScb = 1;
            }
            else
            {
               /* return est scb to available pool */
               SCSI_rRETURNESTSCB(hhcb,hiob);

               if (!hhcb->SCSI_HF_targetHoldEstHiobs)
               {
                  /* An Est SCB is returned so see if we have an
                   * Est HIOB to queue to HWM layer.
                   */
                  /* no need to call SCSI_rFRESH_HIOB etc - done before
                   * put on queue.
                   */
                  if ((newHiob = SCSIrHiobPoolRemove(hhcb)) != SCSI_NULL_HIOB)
                  { 
                     SCSIrQueueEstHiobToHwm(hhcb,newHiob);
                  }
               }   
            }

            if (hiob->stat == SCSI_TASK_CMD_COMP)
            {
               /* Task management command received */
               /* Pass the IOB to the OSM then perform request */  
               haStatus = hiob->haStat;
               nexus = SCSI_NEXUS_UNIT(hiob);
               OSD_COMPLETE_SPECIAL_HIOB(hiob);
               SCSIrTargetTaskManagementRequest(hhcb,haStatus,nexus);
            } 
            else
            {
               /* Maintain IOB completion order - very important for SCSI for 
                  normal IOB processing as OSM needs to be able to determine if
                  overlapped command etc. */ 
               SCSIrPostDoneQueue(hhcb);    /* Post back all HIOB(s) */
               OSD_COMPLETE_SPECIAL_HIOB(hiob);
            }
         }
         return;      
         
      case SCSI_CMD_REESTABLISH_AND_COMPLETE:
      case SCSI_CMD_REESTABLISH_INTERMEDIATE:
         /* Target mode Iob completion */
         /* free scb associated */
         nexus = SCSI_NEXUS_UNIT(hiob); 
         hhcb = nexus->hhcb;
 
         if (nexus->SCSI_XF_busHeld)
         {
            /* save the scb descriptor */
            hhcb->SCSI_HP_busHeldScbDescriptor = hiob->scbDescriptor;
            hhcb->SCSI_HP_estScb = hiob->SCSI_IP_estScb;
            /* Special handling for completing an IOB when the */
            /* SCSI bus is held.                               */
            /* The IOB is completed immediately instead of     */
            /* queueing in the DONE queue.                     */
            haStatus = hiob->haStat;
            nexus->SCSI_XP_hiobActive = 0;

            OSD_COMPLETE_SPECIAL_HIOB(hiob);
            if (hiob->stat == SCSI_TASK_CMD_COMP)
            {
               /* Task management command received */
               SCSIrTargetTaskManagementRequest(hhcb,haStatus,nexus);
            } 
         }
         else
         {
            if (!hiob->SCSI_IP_estScb)
            {
               /* free the scb */ 
               SCSIrFreeScb(hhcb,hiob);
            }
            else
            {
               /* return est scb to available pool */
               SCSI_rRETURNESTSCB(hhcb,hiob);
               if (!hhcb->SCSI_HF_targetHoldEstHiobs)
               {
                  /* An Est SCB is returned so see if we have an
                   * Est HIOB to queue to HWM layer.
                   */
                  if ((newHiob = SCSIrHiobPoolRemove(hhcb)) != SCSI_NULL_HIOB)
                  { 
                     SCSIrQueueEstHiobToHwm(hhcb,newHiob);
                  }
                } 
            }
            
            if (hiob->SCSI_IF_taskManagementResponse)
            {
               /* Task Management Response must call OSM post routine 
                  as an Interrupt is not generated */
               nexus->SCSI_XP_hiobActive = 0;
               OSD_COMPLETE_SPECIAL_HIOB(hiob);
            }  
            else
            {
               /* Put HIOB into Done queue and adjust Done queue pointer */
               hhcb->SCSI_DONE_QUEUE[hhcb->SCSI_DONE_HEAD++] = hiob;
               if (hhcb->SCSI_DONE_HEAD == hhcb->numberScbs)
               {
                  hhcb->SCSI_DONE_HEAD = 0;
               }
            }
         }
         return;

      case SCSI_CMD_ABORT_NEXUS:
         hhcb = SCSI_NEXUS_UNIT(hiob)->hhcb;
         nexus = SCSI_NEXUS_UNIT(hiob);
         /* Note that; order is important - if the HIOB in the host queue */
         /* was returned before calling the HWM then a HIMClearNexusTSH   */
         /* call in the post routine of the aborted HIOB would fail if    */
         /* the SCSI bus was held.                                          */
    
         /* Abort non-active HIOB(s) associated to the nexus  */
         /* in Host queue. */
         SCSIrFindAndAbortTargetHIOBs(hhcb, nexus, SCSI_HOST_ABT_HOST);
         SCSIrPostDoneQueue(hhcb);           /* Post back all HIOB(s) */
         break;

#if SCSI_MULTIPLEID_SUPPORT
      case SCSI_CMD_ENABLE_ID:
      case SCSI_CMD_DISABLE_ID:
         /* nothing required */
         break;
#endif /* SCSI_MULTIPLEID_SUPPORT */

#endif /* SCSI_TARGET_OPERATION */ 


      default:
         break;
   }

   /* Post back the completion of special HIOB */
   OSD_COMPLETE_SPECIAL_HIOB(hiob);
}

/*********************************************************************
*
*  SCSIrAsyncEvent
*
*     Asynchronous event notification
*
*  Return Value:  none
*                  
*  Parameters:    hhcb
*                 event code
*                 others...
*
*  Remarks:
*
*********************************************************************/
void SCSIrAsyncEvent (SCSI_HHCB SCSI_HPTR hhcb,
                      SCSI_UINT16 event, ...)
{
   switch (event)
   {
      case SCSI_AE_3PTY_RESET:
         /* All non-active HIOB(s) in Host queue and Device queues will */
         /* be aborted with the abort status caused by 3rd party reset */
         SCSIrClearHostQueue(hhcb, SCSI_HOST_ABT_3RD_RST);
         SCSI_rCLEARALLDEVICEQUEUES(hhcb, SCSI_HOST_ABT_3RD_RST);
         SCSI_rQUEUEESTHIOBS(hhcb);
         SCSIrPostDoneQueue(hhcb);           /* Post back all HIOB(s) */
         break;

      case SCSI_AE_IOERROR:
         /* All non-active HIOB(s) in Host queue and Device queues will */
         /* be aborted with the abort status caused by io error */
         SCSIrClearHostQueue(hhcb, SCSI_HOST_ABT_IOERR);
         SCSI_rCLEARALLDEVICEQUEUES(hhcb, SCSI_HOST_ABT_IOERR); 
         SCSI_rQUEUEESTHIOBS(hhcb);
         SCSIrPostDoneQueue(hhcb);           /* Post back all HIOB(s) */
         break;

      case SCSI_AE_HA_RESET:
         /* All non-active HIOB(s) in Host queue and Device queues will */
         /* be aborted with the abort status caused by phase error */
         SCSIrClearHostQueue(hhcb, SCSI_HOST_PHASE_ERR);
         SCSI_rCLEARALLDEVICEQUEUES(hhcb, SCSI_HOST_PHASE_ERR);
         SCSI_rQUEUEESTHIOBS(hhcb);
         SCSIrPostDoneQueue(hhcb);           /* Post back all HIOB(s) */
         break;

#if SCSI_SCAM_ENABLE == 2
      case SCSI_AE_SCAM_SELD:
         /* All non-active HIOB(s) in Host queue and Device queues will */
         /* be aborted with the abort status caused by host adapter */
         SCSIrClearHostQueue(hhcb, SCSI_HOST_ABT_HA);
         SCSI_rCLEARALLDEVICEQUEUES(hhcb, SCSI_HOST_ABT_HA);
         SCSI_rQUEUEESTHIOBS(hhcb);
         SCSIrPostDoneQueue(hhcb);           /* Post back all HIOB(s) */
         break;
#endif /* SCSI_SCAM_ENABLE == 2 */

      case SCSI_AE_OSMFREEZE:
         /* Freeze RSM Layer */
         SCSI_rFREEZEALLDEVICEQUEUES(hhcb);
         break;

      case SCSI_AE_OSMUNFREEZE:
         /* Unfreeze RSM Layer */
         SCSI_rUNFREEZEALLDEVICEQUEUES(hhcb);
         break;

#if SCSI_TARGET_OPERATION 
      case SCSI_AE_NEXUS_TSH_THRESHOLD:
      case SCSI_AE_EC_IOB_THRESHOLD:
         /* Do nothing */
         break;
#endif /* SCSI_TARGET_OPERATION */

      default:
         break;
   }

   /* Due to the Protocol Auto Config can be called within the event */
   /* context, we need to notify the OSM after we have aborted all */
   /* pending IOB(s) if it's the case for the 3rd party reset. */
   /* Otherwise, we will accidently abort the PAC as well. */

   /* OSMUNFREEZE will go to the upper layer at the end of protocol */
   /* auto config.  The Unfreeze that comes from the lower layer    */
   /* only needs to be handled if there is no RSM layer i.e. programming */
   /* directly to the HWM layer. */
#if SCSI_DOMAIN_VALIDATION
   if (!hhcb->SCSI_HF_dvInProgress)
   {
      if (event != SCSI_AE_OSMUNFREEZE)
      { 
         /* AEN call to CHIM / XLM layer */
         OSD_ASYNC_EVENT(hhcb, event);
      }
   }
#else /* SCSI_DOMAIN_VALIDATION */
   if (event != SCSI_AE_OSMUNFREEZE)
   { 
      /* AEN call to CHIM / XLM layer */
      OSD_ASYNC_EVENT(hhcb, event);
   }
#endif /* SCSI_DOMAIN_VALIDATION */
}

/*********************************************************************
*
*  SCSIrScsiBusReset
*
*     Process SCSI_Bus_Reset HIOB
*
*  Return Value:  none
*                  
*  Parameters:    hiob
*
*  Remarks:       This routine will clear any HIOB(s) in Host queue
*                 and all Device queues.
*                  
*********************************************************************/
void SCSIrScsiBusReset (SCSI_HIOB SCSI_IPTR hiob)
{
   SCSI_HHCB SCSI_HPTR hhcb = SCSI_INITIATOR_UNIT(hiob);

   /* Freeze RSM Layer */
   SCSI_rFREEZEALLDEVICEQUEUES(hhcb);

   /* Clear non-active HIOB(s) in Host queue and Device queues. */
   /* Start with the Host queue first. */
   SCSIrClearHostQueue(hhcb, SCSI_HOST_ABT_BUS_RST);

   /* The Device queues, if any */
   SCSI_rCLEARALLDEVICEQUEUES(hhcb, SCSI_HOST_ABT_BUS_RST);

   /* Pass SCSI_Bus_Reset HIOB down to Hardware layer */
   SCSIHQueueSpecialHIOB(hiob);
}

/*********************************************************************
*
*  SCSIrBusDeviceReset
*
*     Process Bus_Device_Reset HIOB
*
*  Return Value:  none
*                  
*  Parameters:    hiob
*
*  Remarks:       This routine will clear any HIOB(s) associated to
*                 particular target in Host queue and Device queues.
*                  
*********************************************************************/
void SCSIrBusDeviceReset (SCSI_HIOB SCSI_IPTR hiob)
{
   SCSI_UNIT_CONTROL SCSI_UPTR targetUnit = SCSI_TARGET_UNIT(hiob);
   SCSI_HHCB SCSI_HPTR hhcb = targetUnit->hhcb;
   SCSI_TARGET_CONTROL SCSI_HPTR targetControl = targetUnit->targetControl;

   /* Freeze the device queue if this is the first BDR entering HWM layer. */
   if (!targetControl->activeBDRs)
   {
      SCSIrFreezeDeviceQueue(targetControl, SCSI_FREEZE_BDR);
   }

   /* Abort non-active HIOB(s) associated to the particular target  */
   /* in Host queue and Device queue.  Start with Host queue first. */
   SCSIrFindAndAbortHIOB(hhcb, targetUnit, SCSI_HOST_ABT_TRG_RST);

   /* Then Device queue ... */
   SCSIrClearDeviceQueue(targetControl, SCSI_HOST_ABT_TRG_RST);
   
   /* Tracking number of active BDR already sent to this target. */
   ++targetControl->activeBDRs;
   
   /* Increment the number of active SCBs for this target. */
   /* BDR does not need to qualify with the criteria. */ 
   ++targetControl->activeScbs;

   /* Now check if SCB is available in FREE pool */
   if ((hiob->scbNumber = SCSI_rGETSCB(hhcb, hiob)) == SCSI_NULL_SCB)
   {
      /* No SCB available, queue HIOB into Host queue */
      SCSIrHostQueueAppend(hhcb, hiob);
   }
   else
   {
      SCSIHQueueSpecialHIOB(hiob);
   }
}

/*********************************************************************
*
*  SCSIRPowerManagement
*
*     Execute power management function
*
*  Return Value:  0 - function execute successfully
*                 1 - function not supported
*                 2 - host device not idle         
*                  
*  Parameters:    hhcb
*                 powerMode
*
*  Remarks:       There should be no active hiob when this routine
*                 get called.
*                  
*********************************************************************/
SCSI_UINT SCSIRPowerManagement (SCSI_HHCB SCSI_HPTR hhcb,
                                SCSI_UINT powerMode)
{
   if (SCSIRCheckHostIdle(hhcb))
   {
      return(SCSIHPowerManagement(hhcb,powerMode));
   }
   else
   {
      return(2);
   }
}

/*********************************************************************
*
*  SCSIrAbortTask
*
*     Abort task associated
*
*  Return Value:  none
*                  
*  Parameters:    hiob
*
*  Remarks:       There is no guarantee the aborted hiob is still
*                 in resource management layer.
*
*********************************************************************/
void SCSIrAbortTask (SCSI_HIOB SCSI_IPTR hiob)
{
   SCSI_HIOB SCSI_IPTR hiobAbort = SCSI_RELATED_HIOB(hiob);

   if (SCSIrAbortHIOBInHostQ(hiobAbort, SCSI_HOST_ABT_HOST) ||
       SCSIrAbortHIOBInDevQ(hiobAbort, SCSI_HOST_ABT_HOST) || 
       SCSI_rABORTHIOBINHIOBQ(hiobAbort, SCSI_HOST_ABT_HOST))   
   {
      /* Post back Abort_Task HIOB with successful completion */
      hiob->stat = SCSI_SCB_COMP;
      SCSIrCompleteSpecialHIOB(hiob);
   }

   else
   {
      /* The aborting HIOB was not in the RSM layer. */
      /* So send down the Abort_Task HIOB to HWM layer. */
      SCSIHQueueSpecialHIOB(hiob);
   }
}

/*********************************************************************
*
*  SCSIrAbortTaskSet
*
*     Abort task set associated
*
*  Return Value:  none
*                  
*  Parameters:    hiob
*
*  Remarks:       There is no guarantee the aborted hiob is still
*                 in resource management layer.
*
*********************************************************************/
void SCSIrAbortTaskSet (SCSI_HIOB SCSI_IPTR hiob)
{
   SCSI_UNIT_CONTROL SCSI_UPTR targetUnit = SCSI_TARGET_UNIT(hiob);
   SCSI_HHCB SCSI_HPTR hhcb = targetUnit->hhcb;
   SCSI_TARGET_CONTROL SCSI_HPTR targetControl = targetUnit->targetControl;

   /* Freeze the device queue if this is the first Abort_Task_Set */
   /* entering HWM layer. */
   if (!targetControl->activeABTTSKSETs)
   {
      SCSIrFreezeDeviceQueue(targetControl, SCSI_FREEZE_ABTTSKSET);
   }

   /* Abort non-active HIOB(s) associated to the target's task set  */
   /* in Host queue and Device queue.  Start with Host queue first. */
   SCSIrFindAndAbortHIOB(hhcb, targetUnit, SCSI_HOST_ABT_HOST);

   /* Then Device queue ... */
   SCSIrClearDeviceQueue(targetControl, SCSI_HOST_ABT_HOST);

   /* Tracking number of active Abort_Task_Set already sent to this target. */
   ++targetControl->activeABTTSKSETs;
   
   /* Send request down to HWM layer */
   SCSIHQueueSpecialHIOB(hiob);
}

/*********************************************************************
*
*  SCSIRGetHWInformation
*
*     Gets information from the hardware to put into the 
*  SCSI_HW_INFORMATION structure.  An example usage is to update 
*  adapter/target profile information for the CHIM interface.
*
*  Return Value:  none.
*                  
*  Parameters:    hwInfo      - SCSI_HW_INFORMATION structure.
*                 hhcb        - HHCB structure for the adapter.
*
*  Remarks:       None.
*                 
*********************************************************************/
#if SCSI_PROFILE_INFORMATION
void SCSIRGetHWInformation (SCSI_HW_INFORMATION SCSI_LPTR hwInfo, 
                            SCSI_HHCB SCSI_HPTR hhcb)                  
{
   SCSI_INT i;
   SCSI_TARGET_CONTROL SCSI_HPTR targetControl;

   for (i = 0,targetControl = hhcb->SCSI_HP_targetControl; i < SCSI_MAXDEV;
        i++,targetControl++)
   {
      /* If the device queue is frozen ... */
      if (targetControl->freezeMap)
      {
         /* ... Then get the value from the origMaxTagScbs. */
         hwInfo->targetInfo[i].maxTagScbs = targetControl->origMaxTagScbs;
      }
      else
      {
         /* Otherwise, get it from the maxTagScbs. */
         hwInfo->targetInfo[i].maxTagScbs = targetControl->maxTagScbs;
      }
   }
   
   SCSIHGetHWInformation(hwInfo, hhcb);
}
#endif /* SCSI_PROFILE_INFORMATION */

/*********************************************************************
*
*  SCSIRPutHWInformation
*
*     Updates the the hardware with information from the SCSI_HW_INFORMATION
*  structure.  An example usage is to update adapter/target profile
*  information for the CHIM interface.
*
*  Return Value:  none.
*                  
*  Parameters:    hwInfo      - SCSI_HW_INFORMATION structure.
*                 hhcb        - HHCB structure for the adapter.
*
*  Remarks:       None.
*                 
*********************************************************************/
#if SCSI_PROFILE_INFORMATION && !SCSI_DISABLE_ADJUST_TARGET_PROFILE
void SCSIRPutHWInformation (SCSI_HW_INFORMATION SCSI_LPTR hwInfo, 
                            SCSI_HHCB SCSI_HPTR hhcb)                  
{
   SCSI_INT i;
   SCSI_TARGET_CONTROL SCSI_HPTR targetControl;

   for (i = 0,targetControl = hhcb->SCSI_HP_targetControl; i < SCSI_MAXDEV;
        i++,targetControl++)
   {
      /* If the device queue is frozen ... */
      if (targetControl->freezeMap)
      {
         /* ... Then put the value into the origMaxTagScbs. */
         targetControl->origMaxTagScbs = hwInfo->targetInfo[i].maxTagScbs;
      }
      else
      {
         /* Otherwise, put it into the maxTagScbs. */
         targetControl->maxTagScbs = hwInfo->targetInfo[i].maxTagScbs;
      }
   }
   
   SCSIHPutHWInformation(hwInfo, hhcb);
}
#endif /* SCSI_PROFILE_INFORMATION && !SCSI_DISABLE_ADJUST_TARGET_PROFILE */

/*********************************************************************
*
*  SCSIRGetFreeHead
*
*     Gets the Head of the Free SCB Queue.  An example usage is to 
*  initialize the HW scratch RAM next SCB pointer for the sequencer
*  linked list "one dma" delivery scheme.
*
*  Return Value:  SCSI_SCB_DESCRIPTOR SCSI_HPTR.
*                  
*  Parameters:    hhcb        - HHCB structure for the adapter.
*
*  Remarks:       None.
*                 
*********************************************************************/
SCSI_SCB_DESCRIPTOR SCSI_HPTR SCSIRGetFreeHead(SCSI_HHCB SCSI_HPTR hhcb)
{                 
   return(hhcb->SCSI_FREE_HEAD);
}

/*********************************************************************
*
*  SCSIrAsyncEventCommand
*
*     Asynchronous event notification for a command
*
*  Return Value:  none
*                  
*  Parameters:    hhcb
*                 event code
*                 others...
*
*  Remarks:
*
*********************************************************************/
void SCSIrAsyncEventCommand (SCSI_HHCB SCSI_HPTR hhcb,
                             SCSI_HIOB SCSI_IPTR hiob,
                             SCSI_UINT16 event, ...)
{
   SCSI_TARGET_CONTROL SCSI_HPTR targetControl;
   SCSI_UNIT_CONTROL SCSI_UPTR targetUnit = SCSI_TARGET_UNIT(hiob);
   SCSI_HIOB SCSI_IPTR hiobCurr;
   SCSI_HIOB SCSI_IPTR hiobPrev;
   SCSI_UEXACT8 scsiId;

   switch (event)
   {
      case SCSI_AE_FREEZEONERROR_START:
         targetControl = targetUnit->targetControl;

         /* Save the number of active SCBs */
         targetControl->origActiveScbs = targetControl->activeScbs;

         /* Freeze the device queue */
         SCSIrFreezeDeviceQueue(targetControl, SCSI_FREEZE_ERRORBUSY);

         /* Remember the Device Queue Tail, as the queue has to be    */
         /* adjusted after all the HIOBs with SCSI_SCB_FROZEN status  */
         /* have been returned by the hardware layer.                 */
         targetControl->freezeTail = targetControl->queueTail;

         /* Get the target id */
         scsiId = (SCSI_UEXACT8) targetUnit->scsiID;

         /* Remove HIOBs for this target from the host queue */
         targetControl->freezeHostHead = targetControl->freezeHostTail =
                    SCSI_NULL_HIOB;
         hiobPrev = hiobCurr = hhcb->SCSI_WAIT_HEAD;

         while (hiobCurr != SCSI_NULL_HIOB)
         {
            if ((SCSI_UEXACT8)(SCSI_TARGET_UNIT(hiobCurr)->scsiID) == scsiId)
            {
               /* Remove the hiob from the host queue */
               if (hiobPrev == hhcb->SCSI_WAIT_TAIL)
               {
                  /* queue is going to be empty */
                  hhcb->SCSI_WAIT_HEAD = hhcb->SCSI_WAIT_TAIL = SCSI_NULL_HIOB;
               }
               else
               {
                  /* queue won't be empty */
                  hiobPrev->queueNext = hiobCurr->queueNext;
                  hiobCurr = hiobCurr->queueNext;
               }

               /* Insert the hiob into the temporary host queue used during */
               /* the freeze logic.                                         */
               if (targetControl->freezeHostHead == SCSI_NULL_HIOB)
               {
                  targetControl->freezeHostHead =
                     targetControl->freezeHostTail = hiobCurr;
               }
               else
               {
                  targetControl->freezeHostTail->queueNext = hiobCurr;
                  targetControl->freezeHostTail = hiobCurr;
               }

#if SCSI_NEGOTIATION_PER_IOB
               /* If the hiob has any of the 'forceNego' flags set, increment */
               /* the pendingNego field to indicate the one more negotiation  */
               /* is needed. */
               if (hiob->SCSI_IF_forceSync || hiob->SCSI_IF_forceAsync ||
                   hiob->SCSI_IF_forceWide || hiob->SCSI_IF_forceNarrow)
               {
                  targetControl->pendingNego++;
               }
#endif
            }
            else
            {
               /* Continue the search by moving to the next hiob in host q */
               hiobPrev = hiobCurr;
               hiobCurr = hiobCurr->queueNext;
            }
         }
         
         break;

      case SCSI_AE_FREEZEONERROR_END:
         targetControl = targetUnit->targetControl;

         /* Link the temporary host queue (which was built during   */
         /* FREEZEONERROR_START) behind the device queue            */
         if (targetControl->freezeHostTail != SCSI_NULL_HIOB)
         {
            targetControl->queueTail->queueNext = targetControl->freezeHostHead;
            targetControl->queueTail = targetControl->freezeHostTail;
         }

         /* Mend the device queue in such a way that the chronological order */
         /* of the commands are maintained                                   */
         if (targetControl->freezeTail != SCSI_NULL_HIOB)
         {
            targetControl->queueTail->queueNext = targetControl->queueHead;
            targetControl->queueHead = targetControl->freezeTail->queueNext;
            targetControl->freezeTail->queueNext = SCSI_NULL_HIOB;
            targetControl->queueTail = targetControl->freezeTail;
            targetControl->freezeTail = SCSI_NULL_HIOB;
         }

         /* Restore the number of active SCBs */
         targetControl->activeScbs = targetControl->origActiveScbs;

         break;

      default:
         break;
   }
}

#if SCSI_TARGET_OPERATION
/*********************************************************************
*
*  SCSIRGetFreeEstHead
*
*  Gets the Head of the Free Establish SCB Queue. An example usage is
*  to initialize the HW scratch RAM next Establish Connection SCB
*  pointer for the swapping advanced sequencer linked list "one dma"
*  delivery scheme.
*
*  Return Value:  SCSI_SCB_DESCRIPTOR SCSI_HPTR.
*                  
*  Parameters:    hhcb        - HHCB structure for the adapter.
*
*  Remarks:       None.
*                 
*********************************************************************/
SCSI_SCB_DESCRIPTOR SCSI_HPTR SCSIRGetFreeEstHead(SCSI_HHCB SCSI_HPTR hhcb)
{                 
   return((SCSI_SCB_DESCRIPTOR SCSI_HPTR)hhcb->SCSI_FREE_EST_HEAD);
}

/*********************************************************************
*
*  SCSIrHiobPoolRemove
*
*     Get an Establish Connection IOB from the hiobQueue  
*
*  Return Value:  hiob
*                  
*  Parameters:    hhcb
*
*  Remarks:
*
*********************************************************************/
SCSI_HIOB SCSI_IPTR  SCSIrHiobPoolRemove(SCSI_HHCB SCSI_HPTR  hhcb)
{ 
    SCSI_HIOB SCSI_IPTR hiob;
    SCSI_UEXACT16 queueCnt = hhcb->SCSI_HF_targetHiobQueueCnt; 

    if (queueCnt == 0 )
    { 
       /* send Alert to OSM?? */
       return( SCSI_NULL_HIOB );
    }
    
    hiob = hhcb->SCSI_HF_targetHiobQueue; 
    hhcb->SCSI_HF_targetHiobQueue = hiob->queueNext;
    
    hhcb->SCSI_HF_targetHiobQueueCnt = --queueCnt;
    
    if (hhcb->SCSI_HF_targetHiobQueueThreshold)
    {
       /* Check if OSMEvent to be sent */   
       if( queueCnt == hhcb->SCSI_HF_targetHiobQueueThreshold )
       {
          /* send a Alert to OSM */
          SCSIrAsyncEvent(hhcb,SCSI_AE_EC_IOB_THRESHOLD);
       }
    }

    return(hiob);
}
              
/*********************************************************************
*
*  SCSIrAllocateNexus
*
*     Get a nexus (SCSI_NEXUS structure) from the nexus pool.  
*
*  Return Value:  nexus
*                  
*  Parameters:    hhcb
*
*  Remarks:
*
*********************************************************************/
SCSI_NEXUS SCSI_XPTR SCSIrAllocateNexus (SCSI_HHCB SCSI_HPTR hhcb)
{
   SCSI_NEXUS SCSI_XPTR nexus;
   SCSI_UEXACT16 queueCnt = hhcb->SCSI_HF_targetNexusQueueCnt;
         
   if (queueCnt == 0)
   {
     /* send alert to OSM - or we should never get here?? */
     return(SCSI_NULL_NEXUS);
   }
   
   hhcb->SCSI_HF_targetNexusQueueCnt = --queueCnt;
   
   if (hhcb->SCSI_HF_targetNexusThreshold)
   {
      if (queueCnt == hhcb->SCSI_HF_targetNexusThreshold)
      {
         /* Send alert to OSM */
         SCSIrAsyncEvent(hhcb,SCSI_AE_NEXUS_TSH_THRESHOLD);                                    
      }
   } 
   nexus = hhcb->SCSI_HF_targetNexusQueue;
   hhcb->SCSI_HF_targetNexusQueue = nexus->nextNexus;
  
   /* Nexus in use now */
   nexus->SCSI_XF_available = 0;

   return(nexus);
     
}

/*********************************************************************
*
*  SCSIrFreeNexus
*
*     Return a Nexus (SCSI_NEXUS) to the available nexus queue
*
*  Return Value:  none
*                  
*  Parameters:    hhcb, nexus
*
*  Remarks:
*
*********************************************************************/
void SCSIrFreeNexus (SCSI_HHCB SCSI_HPTR hhcb, SCSI_NEXUS SCSI_XPTR nexus)
{
   /* always add to the head of the queue (we don't care about order) */     
   nexus->SCSI_XF_available = 1;
   nexus->nextNexus = hhcb->SCSI_HF_targetNexusQueue;
   hhcb->SCSI_HF_targetNexusQueue = nexus;
   hhcb->SCSI_HF_targetNexusQueueCnt++;
    
}

/*********************************************************************
*
*  SCSIrClearNexus
*
*     Clear a Nexus
*
*  Return Value:  0 - nexus cleared or nexus already
*                     cleared (i.e. returned to the available nexus pool)
*                 1 - I/O Bus held, nexus NOT cleared  
*                 2 - nexus was not idle, HIOBs pending,                   nexus NOT cleared  
*                  
*  Parameters:    nexus, hhcb
*
*  Remarks:       Returns the nexus to the available nexus pool, if and
*                 only if, there are no HIOBs active for this nexus, and
*                 the SCSI bus is NOT held by this nexus.
*                 If the nexus is already in the available pool then return 
*                 a good result (not an error). 
*
*********************************************************************/
SCSI_UINT8 SCSIrClearNexus (SCSI_NEXUS SCSI_XPTR nexus, SCSI_HHCB SCSI_HPTR hhcb)
{
   SCSI_UINT8 result = 0;

   if (nexus->SCSI_XF_available)
   {
      /* nexus is in available pool - just return successful result */
   }
   else if (nexus->SCSI_XP_hiobActive)
   {
      /* HIOBs pending */
      result = 2;
   }
   else if (SCSIHTargetClearNexus(nexus,hhcb))
   {
      /* I/O Bus held by this nexus */
      result = 1;
   }
   else
   {
      /* Return the nexus to the available pool */ 
      SCSIrFreeNexus(hhcb,nexus);
   }
  
   /* Return HIM_SUCCESS if nexus cleared. */ 
   return(result);
   
}
                         
/*********************************************************************
*
*  SCSIrQueueHIOBPool
*
*     Queue the Establish Connection HIOB to the Hardware layer if
*     resources are available, otherwise queue to the pool of
*     available HIOBs (Establish Connection HIOBs).  
*
*  Return Value:  none
*                  
*  Parameters:    hiob, hhcb
*
*  Remarks:
*
*********************************************************************/
void SCSIrQueueHIOBPool (SCSI_HHCB SCSI_HPTR hhcb,SCSI_HIOB SCSI_IPTR hiob)
{      
   SCSI_NEXUS SCSI_XPTR nexusUnit;

   /* get the Nexus & establish connection scb */
   if ((nexusUnit = SCSIrAllocateNexus(hhcb)) != SCSI_NULL_NEXUS) 
   {
      if ((hiob->scbNumber = SCSI_rGETESTSCB(hhcb,hiob)) != SCSI_NULL_SCB)
      {
         SCSI_NEXUS_UNIT(hiob) = nexusUnit;
         nexusUnit->hhcb = hhcb;
         SCSIHQueueSpecialHIOB(hiob);
         return;
      }
      else
      {
         /* free the nexus */
         SCSIrFreeNexus(hhcb,nexusUnit);
      }
   }

   /* if no Nexus or scb available then queue the hiob */
   SCSIrQueueEstHiobPool(hhcb,hiob);
   return; 
}

/*********************************************************************
*
*  SCSIrQueueEstHiobPool
*
*     Queue an Establish Connection hiob to the Establish hiob pool.
*
*  Return Value:  none
*                  
*  Parameters:    hiob, hhcb
*
*  Remarks:
*
*********************************************************************/
void SCSIrQueueEstHiobPool (SCSI_HHCB SCSI_HPTR hhcb, SCSI_HIOB SCSI_IPTR hiob)
{
   /* always add to the head of the queue (we don't care about order) */                                 
   hiob->queueNext = hhcb->SCSI_HF_targetHiobQueue;
   hhcb->SCSI_HF_targetHiobQueue = hiob; 
   hhcb->SCSI_HF_targetHiobQueueCnt++;
   return; 
}

/*********************************************************************
*
*  SCSIrQueueEstHiobToHwm
*
*     Queue an Establish Connection hiob to the HWM layer if resources.
*
*  Return Value:  none
*                  
*  Parameters:    hiob, hhcb
*
*  Remarks:
*
*********************************************************************/
void SCSIrQueueEstHiobToHwm (SCSI_HHCB SCSI_HPTR hhcb, SCSI_HIOB SCSI_IPTR hiob)
{
   SCSI_NEXUS SCSI_XPTR nexusUnit;

   /* get the Nexus & establish connection scb */
   if ((nexusUnit = SCSIrAllocateNexus(hhcb)) != SCSI_NULL_NEXUS) 
   {
      if ((hiob->scbNumber = SCSI_rGETESTSCB(hhcb,hiob)) != SCSI_NULL_SCB)
      {
         SCSI_NEXUS_UNIT(hiob) = nexusUnit;
         nexusUnit->hhcb = hhcb;
         SCSIHQueueSpecialHIOB(hiob);
         return;
      }
      else
      {
         /* free the nexus */
         SCSIrFreeNexus(hhcb,nexusUnit);
      }
   }
   /* Queue to Est HIOB queue */
   SCSIrQueueEstHiobPool(hhcb,hiob);  
   return; 
}


#if SCSI_TARGET_OPERATION
/*********************************************************************
*
*  SCSIrAllocateNode
*
*     Get a node (SCSI_NODE structure) from the node pool.  
*
*  Return Value:  node
*                  
*  Parameters:    hhcb
*
*  Remarks:
*
*********************************************************************/
SCSI_NODE SCSI_NPTR SCSIrAllocateNode (SCSI_HHCB SCSI_HPTR hhcb)
                     
{
   SCSI_NODE SCSI_NPTR node;
   SCSI_UEXACT16 queueCnt = hhcb->SCSI_HF_targetNexusQueueCnt;
         
   node = hhcb->SCSI_HF_targetNodeQueue;
   if (node != SCSI_NULL_NODE)
   {
      hhcb->SCSI_HF_targetNodeQueue = node->nextNode;
   }
   
   return(node);
     
}
#endif /* SCSI_TARGET_OPERATION */

/*********************************************************************
*
*  SCSIrTargetTaskManagementRequest
*
*     Process a Task Management request received from an initiator  
*
*  Return Value:  none
*                  
*  Parameters:    hhcb, haStatus, nexus
*
*  Remarks:  This routine assumes that information in nexus is valid
*            even though the IOB associated with this nexus has been
*            returned to the OSM and the OSM may have issued a 
*            HIMClearNexus. I.e. HIMClearNexus must NOT reset any 
*            connection information fields in the nexus.
*
*********************************************************************/
void SCSIrTargetTaskManagementRequest (SCSI_HHCB SCSI_HPTR hhcb, SCSI_UEXACT8 haStatus,
                                          SCSI_NEXUS SCSI_XPTR nexus)
                     
{    
        
   switch (haStatus)
   {
      case SCSI_HOST_TARGET_RESET:
         SCSIrFindAndAbortTargetHIOBs(hhcb,SCSI_NULL_NEXUS,haStatus);
         break;
             
      case SCSI_HOST_ABORT_TASK_SET:
      case SCSI_HOST_ABORT_TASK:
      case SCSI_HOST_CLEAR_TASK_SET:
         /* These requests involve aborting HIOBs based on */
         /* nexus information */ 
         SCSIrFindAndAbortTargetHIOBs(hhcb,nexus,haStatus);
         break;

      case SCSI_HOST_CLEAR_ACA:
         /* SCSI-3 Clear ACA */
      case SCSI_HOST_TERMINATE_TASK:
         /* SCSI-2 Terminate I/O Process or SCSI-3 Terminate Task */
      case SCSI_HOST_LUN_RESET:
         /* SCSI-3 Logical Unit Reset */

         /* Do nothing */
         break;
      
      default:
         /* Should never get here */
         break;

   }
}

#endif /* SCSI_TARGET_OPERATION */

/*$Header:   Y:/source/chimscsi/src/himscsi/RSMUTIL.CV_   1.26.14.3   25 Nov 1997 19:24:10   RAME3369  $*/
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
*  Module Name:   RSMUTIL.C
*
*  Description:
*                 Utilities for resource management layer
*
*  Owners:        ECX IC Firmware Team
*    
*  Notes:         NONE
*
*  Entry Point(s):
*
***************************************************************************/

#if 0
#include "scsi.h"	/* exclude */
#include "rsm.h"	/* exclude */
#endif

/*********************************************************************
*
*  SCSIrCheckCriteria
*
*     Continue scb processing with checking criteria
*
*  Return Value:  none
*                  
*  Parameters:    hhcb
*                 hiob
*
*  Remarks:       This routine will check if max tags/nontags
*                 criteria meet. If it's meet then continue
*                 HIOB execution process else just queue it.
*                  
*********************************************************************/
#if !SCSI_STREAMLINE_QIOPATH
void SCSIrCheckCriteria (SCSI_HHCB SCSI_HPTR hhcb,
                         SCSI_HIOB SCSI_IPTR hiob)
{
   SCSI_TARGET_CONTROL SCSI_HPTR targetControl = SCSI_TARGET_UNIT(hiob)->targetControl;
   SCSI_UINT16 maxScbs;

   /* get the criteria factor */
   maxScbs = ((hiob->SCSI_IF_tagEnable) ? targetControl->maxTagScbs :
                                       targetControl->maxNonTagScbs);
#if SCSI_NEGOTIATION_PER_IOB
   /* If HIOB sets the flags to force negotiation, try to do the negotiation */
   if (hiob->SCSI_IF_forceSync || hiob->SCSI_IF_forceAsync ||
       hiob->SCSI_IF_forceWide || hiob->SCSI_IF_forceNarrow)
   {
      /* If the device is not idle, simply put the hiob into the device queue */
      /* and remember the fact that negotiation is pending, else change the   */
      /* xfer option parameters to do the negotiation on the next command. */
      if (!SCSIHCheckDeviceIdle(hhcb, SCSI_TARGET_UNIT(hiob)->scsiID))
      {
         maxScbs = 0;
      }
      else
      {
         SCSI_hCHANGEXFEROPTION(hiob);
      }
   }
   /* If a negotiation is pending, queue the incoming hiob into the device   */
   /* queue, so that the command associated with this hiob will be executed  */
   /* after the negotiation. */
   if (targetControl->pendingNego)
   {
      maxScbs = 0;
   }
#endif /* SCSI_NEGOTIATION_PER_IOB */

   if (++targetControl->activeScbs <= maxScbs)
   {
      /* criteria met, continue with allocating scb */
      SCSIrAllocateScb(hhcb,hiob);
   }
   else if ((targetControl->freezeMap) && (hiob->priority >= 0x80))
   {
      targetControl->activeHiPriScbs++;
      targetControl->freezeMap |= SCSI_FREEZE_HIPRIEXISTS;
      SCSIrAllocateScb(hhcb,hiob);
   }
   else
   {
      /* criteria not met, just append it to device queue */
      SCSIrDeviceQueueAppend(targetControl,hiob);
   }
}
#endif /* !SCSI_STREAMLINE_QIOPATH */

/*********************************************************************
*
*  SCSIrAllocateScb
*
*     Continue scb processing with allocating scb
*
*  Return Value:  none
*                  
*  Parameters:    hhcb
*                 hiob
*
*  Remarks:       This routine will try to allocate scb for
*                 specified hiob. if it's available then 
*                 continue HIOB execution process else just
*                 queue it.
*                  
*********************************************************************/
void SCSIrAllocateScb (SCSI_HHCB SCSI_HPTR hhcb,
                       SCSI_HIOB SCSI_IPTR hiob)
{
   if ((hiob->scbNumber = SCSI_rGETSCB(hhcb,hiob)) != SCSI_NULL_SCB)
   {
      /* scb available, queue to hardware management layer */
      SCSIHQueueHIOB(hiob);
   }
   else
   {
      /* queue it to host queue */
      SCSIrHostQueueAppend(hhcb,hiob);
   }
}

/*********************************************************************
*
*  SCSIrGetScb
*
*     Get free scb if available
*
*  Return Value:  none
*                  
*  Parameters:    hhcb
*
*  Remarks:
*                  
*********************************************************************/
SCSI_UEXACT8 SCSIrGetScb (SCSI_HHCB SCSI_HPTR hhcb,
                          SCSI_HIOB SCSI_IPTR hiob)
{
   /* no scb available */
   if (hhcb->SCSI_FREE_SCB_AVAIL < 2)
   {
      return(SCSI_NULL_SCB);
   }
   else
   {
      hhcb->SCSI_FREE_SCB_AVAIL--;
      hiob->scbDescriptor = hhcb->SCSI_FREE_HEAD;
      hhcb->SCSI_FREE_HEAD = hhcb->SCSI_FREE_HEAD->queueNext;
      return(hiob->scbDescriptor->scbNumber);
   }
}

/*********************************************************************
*
*  SCSIrReturnScb
*
*     Standard mode return scb to free pool
*
*  Return Value:  none
*                  
*  Parameters:    hhcb
*                 scb number to be returned
*
*  Remarks:
*                  
*********************************************************************/
void SCSIrReturnScb (SCSI_HHCB SCSI_HPTR hhcb,
                     SCSI_HIOB SCSI_IPTR hiob)
{
   hhcb->SCSI_FREE_SCB_AVAIL++;
   hhcb->SCSI_FREE_TAIL->queueNext = hiob->scbDescriptor;
   hhcb->SCSI_FREE_TAIL = hiob->scbDescriptor;
}

/*********************************************************************
*
*  SCSIrDeviceQueueAppend
*
*     Get free scb if available
*
*  Return Value:  none
*                  
*  Parameters:    target unit control
*                 hiob
*                 
*  Remarks:
*                  
*********************************************************************/
void SCSIrDeviceQueueAppend (SCSI_TARGET_CONTROL SCSI_HPTR targetControl,
                             SCSI_HIOB SCSI_IPTR hiob)
{
#if SCSI_NEGOTIATION_PER_IOB
   /* If the hiob has any of the 'forceNego' flags set, increment   */
   /* the pendingNego field to indicate the one more negotiation is */
   /* needed. */
   if (hiob->SCSI_IF_forceSync || hiob->SCSI_IF_forceAsync ||
       hiob->SCSI_IF_forceWide || hiob->SCSI_IF_forceNarrow)
   {
      targetControl->pendingNego++;
   }
#endif

   hiob->queueNext = SCSI_NULL_HIOB;

   if (targetControl->queueHead == SCSI_NULL_HIOB)
   {
      /* queue empty, start queue */
      targetControl->queueHead = targetControl->queueTail = hiob;
   }
   else
   {
      /* append at end */
      targetControl->queueTail->queueNext = hiob;
      targetControl->queueTail = hiob;
   }

   targetControl->deviceQueueHiobs++;
}

/*********************************************************************
*
*  SCSIrDeviceQueueRemove
*
*     Remove hiob from device queue
*
*  Return Value:  hiob removed from device queue head
*                  
*  Parameters:    target unit control
*                 
*  Remarks:       This routine assumes that there is at least one
*                 hiob in device queue
*                  
*********************************************************************/
SCSI_HIOB SCSI_IPTR SCSIrDeviceQueueRemove (SCSI_TARGET_CONTROL SCSI_HPTR targetControl)
{
   SCSI_HIOB SCSI_IPTR hiob = targetControl->queueHead;
   SCSI_UINT16 maxScbs;
#if SCSI_NEGOTIATION_PER_IOB
   SCSI_UNIT_CONTROL SCSI_UPTR targetUnit;
   SCSI_HHCB SCSI_HPTR hhcb;
#endif /* SCSI_NEGOTIATION_PER_IOB */

   if (hiob != SCSI_NULL_HIOB)
   {
      /* Get the criteria factor */
      maxScbs = ((hiob->SCSI_IF_tagEnable) ? targetControl->maxTagScbs :
                                             targetControl->maxNonTagScbs);

      /* If criteria is not met, simply return NULL_HIOB */
      if (maxScbs <= (targetControl->activeScbs - targetControl->deviceQueueHiobs))
      {
         hiob = SCSI_NULL_HIOB;
      }
      else
      {
#if SCSI_NEGOTIATION_PER_IOB
         targetUnit = SCSI_TARGET_UNIT(hiob);
         hhcb = targetUnit->hhcb;
         
         /* If HIOB sets the flags to force negotiation, try to do the   */
         /* negotiation */
         if (hiob->SCSI_IF_forceSync || hiob->SCSI_IF_forceAsync ||
             hiob->SCSI_IF_forceWide || hiob->SCSI_IF_forceNarrow)
         {
            /* If the device is not idle, do not take the hiob out of the   */
            /* device queue as more commands are pending with the device,   */
            /* else change the xfer option parameters to do the negotiation */
            /* on the next command. */
            if (!SCSIHCheckDeviceIdle(hhcb, targetUnit->scsiID))
            {
               hiob = SCSI_NULL_HIOB;
            }
            else
            {
               SCSI_hCHANGEXFEROPTION(hiob);
               targetControl->pendingNego--;
            }
         }

         /* Remove the hiob from the device queue only if allowed to */
         if (hiob != SCSI_NULL_HIOB)
         {
            if (targetControl->queueHead == targetControl->queueTail)
            {
               /* queue empty */
               targetControl->queueHead = targetControl->queueTail = SCSI_NULL_HIOB;
            }
            else
            {
               /* queue won't be empty */
               targetControl->queueHead = targetControl->queueHead->queueNext;
            }

            targetControl->deviceQueueHiobs--;
         }
#else /* SCSI_NEGOTIATION_PER_IOB */
         if (targetControl->queueHead == targetControl->queueTail)
         {
            /* queue empty */
            targetControl->queueHead = targetControl->queueTail = SCSI_NULL_HIOB;
         }
         else
         {
            /* queue won't be empty */
            targetControl->queueHead = targetControl->queueHead->queueNext;
         }

         targetControl->deviceQueueHiobs--;
#endif /* SCSI_NEGOTIATION_PER_IOB */
      }
   }

   return(hiob);
}

/*********************************************************************
*
*  SCSIrAbortHIOBInDevQ
*
*     Abort the aborting HIOB in Device queue if it found
*
*  Return Value:  0 - aborting HIOB found
*                 1 - aborting HIOB not found
*                  
*  Parameters:    aborting hiob
*                 host adapter status
*                 
*  Remarks:
*                  
*********************************************************************/
SCSI_UEXACT8 SCSIrAbortHIOBInDevQ (SCSI_HIOB SCSI_IPTR hiob,
                                   SCSI_UEXACT8 haStat)
{
   SCSI_TARGET_CONTROL SCSI_HPTR targetControl;
   SCSI_HIOB SCSI_IPTR hiobCurr;
   SCSI_HIOB SCSI_IPTR hiobPrev;
   SCSI_UEXACT8 retStat = 0;

   targetControl = SCSI_TARGET_UNIT(hiob)->targetControl;
   hiobCurr = hiobPrev = targetControl->queueHead;

   while (hiobCurr != SCSI_NULL_HIOB)
   {
      /* Abort current HIOB if it matches */
      if (hiobCurr == hiob)
      {
#if SCSI_NEGOTIATION_PER_IOB
         /* If the hiob has any of the 'forceNego' flags set, decrement   */
         /* the pendingNego field to indicate the one less negotiation is */
         /* needed. */
         if (hiob->SCSI_IF_forceSync || hiob->SCSI_IF_forceAsync ||
             hiob->SCSI_IF_forceWide || hiob->SCSI_IF_forceNarrow)
         {
            targetControl->pendingNego--;
         }
#endif
         /* Mending Device queue */
         SCSIrAdjustQueue(&targetControl->queueHead, &targetControl->queueTail,
                          &hiobPrev, hiobCurr);

         /* Post aborted HIOB */
         SCSIrPostAbortedHIOB(hiobCurr, haStat);

         targetControl->deviceQueueHiobs--;

         retStat = 1;
         break;
      }
      else
      {
         /* Aborting HIOB have not found yet, advance to the next HIOB */
         hiobPrev = hiobCurr;
         hiobCurr = hiobCurr->queueNext;
      }
   }  /* end of while loop */

   return(retStat);
}

/*********************************************************************
*
*  SCSIrClearDeviceQueue
*
*     Clear any non-active HIOB(s) in particular target's Device queue.
*
*  Return Value:  none
*                  
*  Parameters:    target unit control
*                 host adapter status
*                 
*  Remarks:       This routine will remove any HIOB(s) in a target's
*                 Device queue and post aborted HIOB back to OSM.
*                  
*********************************************************************/
void SCSIrClearDeviceQueue (SCSI_TARGET_CONTROL SCSI_HPTR targetControl,
                            SCSI_UEXACT8 haStatus)
{
   SCSI_HIOB SCSI_IPTR hiob = targetControl->queueHead;
   SCSI_HIOB SCSI_IPTR qTail = targetControl->queueTail;

   /* If the device queue is not empty */
   if (hiob != SCSI_NULL_HIOB)
   {
      while (hiob != qTail)
      {
         /* Due to the HIOB got post back to OSM immediately, we need to */
         /* save the next HIOB.  Otherwise, the queue will be broken. */
         targetControl->queueHead = targetControl->queueHead->queueNext;

#if SCSI_NEGOTIATION_PER_IOB
         /* If the hiob has any of the 'forceNego' flags set, decrement   */
         /* the pendingNego field to indicate the one less negotiation is */
         /* needed. */
         if (hiob->SCSI_IF_forceSync || hiob->SCSI_IF_forceAsync ||
             hiob->SCSI_IF_forceWide || hiob->SCSI_IF_forceNarrow)
         {
            targetControl->pendingNego--;
         }
#endif
         /* Post aborted HIOB */
         SCSIrPostAbortedHIOB(hiob, haStatus);

         targetControl->deviceQueueHiobs--;

         /* Next HIOB in queue */
         hiob = targetControl->queueHead;
      }

      /* Advance to past the last HIOB in the device queue */
      targetControl->queueHead = targetControl->queueHead->queueNext;
      
#if SCSI_NEGOTIATION_PER_IOB
      /* If the hiob has any of the 'forceNego' flags set, decrement   */
      /* the pendingNego field to indicate the one less negotiation is */
      /* needed. */
      if (hiob->SCSI_IF_forceSync || hiob->SCSI_IF_forceAsync ||
          hiob->SCSI_IF_forceWide || hiob->SCSI_IF_forceNarrow)
      {
         targetControl->pendingNego--;
      }
#endif
      /* Post the last aborted HIOB */
      SCSIrPostAbortedHIOB(hiob, haStatus);

      targetControl->deviceQueueHiobs--;

      /* If the queue head is null, then update the queue tail as well */
      if (targetControl->queueHead == SCSI_NULL_HIOB)
      {
         targetControl->queueTail = SCSI_NULL_HIOB;
      }
   }
}

/*********************************************************************
*
*  SCSIrClearAllDeviceQueues
*
*     Clear any non-active HIOB(s) in all Device queues.
*
*  Return Value:  none
*                  
*  Parameters:    hhcb
*                 host adapter status
*                 
*  Remarks:
*                  
*********************************************************************/
void SCSIrClearAllDeviceQueues (SCSI_HHCB SCSI_HPTR hhcb,
                                SCSI_UEXACT8 haStatus)
{
   SCSI_TARGET_CONTROL SCSI_HPTR targetControl;
   SCSI_INT i;
   SCSI_INT maxDev = SCSI_MAXDEV;

#if SCSI_DMA_SUPPORT
   /* Need to handle the special DMA queue */
   maxDev++;
#endif /* SCSI_DMA_SUPPORT */

   /* Search through all Device queues for any existing HIOB(s) */
   for (i = 0, targetControl = hhcb->SCSI_HP_targetControl; i < maxDev;
        i++, targetControl++)
   {
      /* Clear any HIOB(s) in a device queue */
      SCSIrClearDeviceQueue(targetControl, haStatus);
   }
}

/*********************************************************************
*
*  SCSIrHostQueueAppend
*
*     Append to host queue
*
*  Return Value:  none
*                  
*  Parameters:    hhcb
*                 hiob to be appended
*                 
*  Remarks:
*                  
*********************************************************************/
void SCSIrHostQueueAppend (SCSI_HHCB SCSI_HPTR hhcb,
                           SCSI_HIOB SCSI_IPTR hiob)
{
   hiob->queueNext = SCSI_NULL_HIOB;

   if (hhcb->SCSI_WAIT_HEAD == SCSI_NULL_HIOB)
   {
      /* queue was empty */
      hhcb->SCSI_WAIT_HEAD = hhcb->SCSI_WAIT_TAIL = hiob;
   }
   else
   {
      /* append at end */
      hhcb->SCSI_WAIT_TAIL->queueNext = hiob;
      hhcb->SCSI_WAIT_TAIL = hiob;
   }
}

/*********************************************************************
*
*  SCSIrHostQueueRemove
*
*     Remove from host queue
*
*  Return Value:  hiob removed from host queue
*                 scbNumber available for reuse
*                  
*  Parameters:    hhcb
*                 
*  Remarks:       This routine assumes that there is at least one
*                 hiob in host queue
*                  
*********************************************************************/
SCSI_HIOB SCSI_IPTR SCSIrHostQueueRemove (SCSI_HHCB SCSI_HPTR hhcb,
                                                  SCSI_UEXACT8 scbNumber)
{
   SCSI_HIOB SCSI_IPTR hiob = hhcb->SCSI_WAIT_HEAD;

   if (hiob != SCSI_NULL_HIOB)
   {
      /* the available scb number can be used for either tagged */
      /* nontagged request. we just remove the head from queue */
      if (hiob == hhcb->SCSI_WAIT_TAIL)
      {
         /* queue is going to be empty */
         hhcb->SCSI_WAIT_HEAD = hhcb->SCSI_WAIT_TAIL = SCSI_NULL_HIOB;
      }
      else
      {
         /* queue won't be empty */
         hhcb->SCSI_WAIT_HEAD = hhcb->SCSI_WAIT_HEAD->queueNext;
      }
   }

   return(hiob);
}

/*********************************************************************
*
*  SCSIrAbortHIOBInHostQ
*
*     Abort the aborting HIOB in Host queue if it found
*
*  Return Value:  0 - aborting HIOB found
*                 1 - aborting HIOB not found
*                  
*  Parameters:    aborting hiob
*                 host adapter status
*                 
*  Remarks:
*                  
*********************************************************************/
SCSI_UEXACT8 SCSIrAbortHIOBInHostQ (SCSI_HIOB SCSI_IPTR hiob,
                                    SCSI_UEXACT8 haStat)
{
   SCSI_HHCB SCSI_HPTR hhcb = SCSI_TARGET_UNIT(hiob)->hhcb;
   SCSI_HIOB SCSI_IPTR hiobCurr;
   SCSI_HIOB SCSI_IPTR hiobPrev;
   SCSI_UEXACT8 retStat = 0;

   hiobCurr = hiobPrev = hhcb->SCSI_WAIT_HEAD;

   while (hiobCurr != SCSI_NULL_HIOB)
   {
      /* Abort current HIOB if it matches */
      if (hiobCurr == hiob)
      {
         /* Mending Host queue */
         SCSIrAdjustQueue(&hhcb->SCSI_WAIT_HEAD, &hhcb->SCSI_WAIT_TAIL,
                          &hiobPrev, hiobCurr);

         /* Post aborted HIOB */
         SCSIrPostAbortedHIOB(hiobCurr, haStat);

         retStat = 1;
         break;
      }
      else
      {
         /* Aborting HIOB have not found yet, advance to the next HIOB */
         hiobPrev = hiobCurr;
         hiobCurr = hiobCurr->queueNext;
      }
   }  /* end of while loop */

   return(retStat);
}

/*********************************************************************
*
*  SCSIrFindAndAbortHIOB
*
*     Find and abort any HIOB(s) in Host queue that matched scsi id
*     of the aborting HIOB.
*
*  Return Value:  none
*                  
*  Parameters:    hhcb
*                 target unit control
*                 host adapter status
*                 
*  Remarks:
*                  
*********************************************************************/
void SCSIrFindAndAbortHIOB (SCSI_HHCB SCSI_HPTR hhcb,
                            SCSI_UNIT_CONTROL SCSI_UPTR targetUnit,
                            SCSI_UEXACT8 haStat)
{
   SCSI_HIOB SCSI_IPTR hiobCurr;
   SCSI_HIOB SCSI_IPTR hiobPrev;
   SCSI_HIOB SCSI_IPTR qNext;
   SCSI_HIOB SCSI_IPTR qTail = hhcb->SCSI_WAIT_TAIL;

   hiobCurr = hiobPrev = hhcb->SCSI_WAIT_HEAD;

   /* If the host queue is not empty */
   if (hiobCurr != SCSI_NULL_HIOB)
   {
      while (hiobCurr != qTail)
      {
         /* Due to the HIOB got post back to OSM immediately, we need to */
         /* save the next HIOB.  Otherwise, the queue will be broken. */
         qNext = hiobCurr->queueNext;

         /* We'll abort any HIOB that has a SCSI id matching the new HIOB */
         if (SCSI_TARGET_UNIT(hiobCurr)->scsiID == targetUnit->scsiID)
         {
            /* Mending Host queue */
            SCSIrAdjustQueue(&hhcb->SCSI_WAIT_HEAD, &hhcb->SCSI_WAIT_TAIL,
                             &hiobPrev, hiobCurr);
     
            /* Post aborted HIOB back to OSM */
            SCSIrPostAbortedHIOB(hiobCurr, haStat);
         }

         else
         {
            hiobPrev = hiobCurr;
         }

         /* Next HIOB in queue */
         hiobCurr = qNext;
      }

      /* We'll abort any HIOB that has a SCSI id matching the new HIOB */
      if (SCSI_TARGET_UNIT(hiobCurr)->scsiID == targetUnit->scsiID)
      {
         /* Mending Host queue */
         SCSIrAdjustQueue(&hhcb->SCSI_WAIT_HEAD, &hhcb->SCSI_WAIT_TAIL,
                          &hiobPrev, hiobCurr);
     
         /* Post aborted HIOB back to OSM */
         SCSIrPostAbortedHIOB(hiobCurr, haStat);
      }
   }
}

/*********************************************************************
*
*  SCSIrClearHostQueue
*
*     Clear any non-active HIOB(s) in Host queue
*
*  Return Value:  none
*                  
*  Parameters:    hhcb
*                 host adapter status
*                 
*  Remarks:       This routine will remove any HIOB(s) in Host queue
*                 and put aborted HIOB into Done queue.
*                  
*********************************************************************/
void SCSIrClearHostQueue (SCSI_HHCB SCSI_HPTR hhcb,
                          SCSI_UEXACT8 haStatus)
{
   SCSI_HIOB SCSI_IPTR hiob = hhcb->SCSI_WAIT_HEAD;
   SCSI_HIOB SCSI_IPTR qTail = hhcb->SCSI_WAIT_TAIL;

   /* If the host queue is not empty */
   if (hiob != SCSI_NULL_HIOB)
   {
      while (hiob != qTail)
      {
         /* Due to the HIOB got post back to OSM immediately, we need to */
         /* save the next HIOB.  Otherwise, the queue will be broken. */
         hhcb->SCSI_WAIT_HEAD = hhcb->SCSI_WAIT_HEAD->queueNext;

         /* Post aborted HIOB back to OSM */
         SCSIrPostAbortedHIOB(hiob, haStatus);

         /* Next HIOB in queue */
         hiob = hhcb->SCSI_WAIT_HEAD;
      }

      /* Advance to past the last HIOB in the host queue */
      hhcb->SCSI_WAIT_HEAD = hhcb->SCSI_WAIT_HEAD->queueNext;

      /* Post the last aborted HIOB back to OSM */
      SCSIrPostAbortedHIOB(hiob, haStatus);

      /* If the queue head is null, then update the queue tail as well */
      if (hhcb->SCSI_WAIT_HEAD == SCSI_NULL_HIOB)
      {
         hhcb->SCSI_WAIT_TAIL = SCSI_NULL_HIOB;
      }
   }
}

/*********************************************************************
*
*  SCSIrFreeScb
*
*     Free scb associated
*
*  Return Value:  none
*                  
*  Parameters:    hhcb
*                 hiob
*
*  Remarks:       This routine will free scb specified. if there was
*                 any hiob queued for waiting scb available then this
*                 new available scb will be used to continue queued hiob
*                 processing.
*                  
*********************************************************************/
void SCSIrFreeScb (SCSI_HHCB SCSI_HPTR hhcb,
                   SCSI_HIOB SCSI_IPTR hiob)
{
   SCSI_HIOB SCSI_IPTR hiobNew;

   if ((hiobNew = SCSI_rHOSTQUEUEREMOVE(hhcb,hiob->scbNumber)) !=
      SCSI_NULL_HIOB)
   {
      /* must return scb to linked list then grab off top to ensure list is */
      /* linked properly */
      SCSI_rRETURNSCB(hhcb,hiob); /* return completed scb */
      /* don't need to check return value of GETSCB since we know there will */
      /* be an scb available since we just returned one */
      hiobNew->scbNumber = SCSI_rGETSCB(hhcb,hiobNew);

      /* there are some special HIOBs, e.g. Bus Device Reset, */
      /* are queueing just like a normal HIOB */
      if ((hiobNew->cmd == SCSI_CMD_INITIATE_TASK)
#if SCSI_DMA_SUPPORT
          || (hiobNew->cmd == SCSI_CMD_INITIATE_DMA_TASK)
#endif /* SCSI_DMA_SUPPORT */
         )
      {
         SCSIHQueueHIOB(hiobNew);
      }
      else
      {
         SCSIHQueueSpecialHIOB(hiobNew);
      }
   }
   else
   {
      /* return it to free pool */
      SCSI_rRETURNSCB(hhcb,hiob);
   }

   /* continue to free criteria */
   SCSI_rFREECRITERIA(hiob);
}

/*********************************************************************
*
*  SCSIrFreeCriteria
*
*     Free criteria associated specified
*
*  Return Value:  none
*                  
*  Parameters:    hiob
*
*  Remarks:       This routine will free scb specified. if there was
*                 any hiob queued for waiting scb available then this
*                 new available scb will be used to continue queued hiob
*                 processing.
*                  
*********************************************************************/
void SCSIrFreeCriteria (SCSI_HIOB SCSI_IPTR hiob)
{
   SCSI_UNIT_CONTROL SCSI_UPTR targetUnit = SCSI_TARGET_UNIT(hiob);
   SCSI_HHCB SCSI_HPTR hhcb = targetUnit->hhcb;
   SCSI_TARGET_CONTROL SCSI_HPTR targetControl = targetUnit->targetControl;
   SCSI_HIOB SCSI_IPTR hiobNew;
   SCSI_UINT16 maxScbs;

   /* get checking criteria */
   maxScbs = ((hiob->SCSI_IF_tagEnable) ? targetControl->maxTagScbs :
                                 targetControl->maxNonTagScbs);

   if (!targetControl->freezeMap)
   {
#if SCSI_NEGOTIATION_PER_IOB
      /* Some hiobs are left in the device queue if there are more scbs     */
      /* waiting in the rsm layer than maximum allowed tags, OR there is    */
      /* negotiation pending; so just check to see if any HIOBs are sitting */
      /* in the device queue. */
      targetControl->activeScbs--;
      while (targetControl->deviceQueueHiobs)
#else /* SCSI_NEGOTIATION_PER_IOB */
      if (--targetControl->activeScbs >= maxScbs)
#endif /* SCSI_NEGOTIATION_PER_IOB */
      {
         /* some one must be waiting for criteria free */
         if ((hiobNew = SCSIrDeviceQueueRemove(targetControl)) !=
             SCSI_NULL_HIOB)
         {
            /* continue hiob processing */
            SCSIrAllocateScb(targetUnit->hhcb,hiobNew);
         }
#if SCSI_NEGOTIATION_PER_IOB
         else
         {
            break;
         }
#endif
      }
   }
   else
   {                                 
      /* Free the criteria */
      --targetControl->activeScbs;
      
      /* if no scbs outstanding for device then can finish up queue */
      /* full handling */
      if (SCSIHCheckDeviceIdle(hhcb, targetUnit->scsiID) &&
          (targetControl->freezeMap & SCSI_FREEZE_QFULL))
      {
         SCSIrFinishQueueFull(hiob);
      }

      if ((targetControl->freezeMap) && (hiob->priority >= 0x80))
      {
         --targetControl->activeHiPriScbs;
         if (!targetControl->activeHiPriScbs)
         {
            /* Unfreeze the device queue and dispatch any queued HIOBs */
            /* if the queue is not frozen. */
            SCSIrUnFreezeDeviceQueue(SCSI_TARGET_UNIT(hiob)->targetControl,
                                     SCSI_FREEZE_HIPRIEXISTS);
            SCSIrDispatchQueuedHIOB(hiob);
         }
      }
   }
}

/*********************************************************************
*
*  SCSIrPostAbortedHIOB
*
*     Setup abort status and post back aborted HIOB to OSM
*
*  Return Value:  none
*                  
*  Parameters:    hiob to be aborted
*                 host adapter status
*                 
*  Remarks:
*                  
*********************************************************************/
void SCSIrPostAbortedHIOB (SCSI_HIOB SCSI_IPTR hiob,
                           SCSI_UEXACT8 haStatus)
{
#if SCSI_TARGET_OPERATION 
   if (!hiob->SCSI_IP_targetMode)
      /* if not a target mode HIOB */
#endif /* SCSI_TARGET_OPERATION */   
      /* Decrement number of active Scbs for particular target */
      --(SCSI_TARGET_UNIT(hiob)->targetControl->activeScbs);

   /* Set up abort status */
   hiob->stat = SCSI_SCB_ABORTED;
   hiob->haStat = haStatus;

   /* Post back aborted HIOB to OSM */
   OSD_COMPLETE_HIOB(hiob);
}

/*********************************************************************
*
*  SCSIrPostDoneQueue
*
*     Posting all hiobs in done queue
*
*  Return Value:  none
*                  
*  Parameters:    hhcb
*
*  Remarks:
*
*********************************************************************/
void SCSIrPostDoneQueue (SCSI_HHCB SCSI_HPTR hhcb)
{
   SCSI_UEXACT8 origTail;
#if SCSI_TARGET_OPERATION
   SCSI_HIOB SCSI_IPTR hiob;
#endif /* SCSI_TARGET_OPERATION */

   /* post done queue if not empty */
   while (hhcb->SCSI_DONE_HEAD != hhcb->SCSI_DONE_TAIL)
   {
      /* origTail is assigned hhcb->SCSI_DONE_TAIL (since post increment) */
      origTail = hhcb->SCSI_DONE_TAIL++;

      if (hhcb->SCSI_DONE_TAIL == (hhcb->numberScbs))
      {
         hhcb->SCSI_DONE_TAIL = 0;
      }
#if SCSI_TARGET_OPERATION
      if (hhcb->SCSI_HF_targetMode)
      {
         /* reset the hiobActive flag for the nexus */
         hiob = hhcb->SCSI_DONE_QUEUE[origTail];
         if (hiob->SCSI_IP_targetMode)
         {
            SCSI_NEXUS_UNIT(hiob)->SCSI_XP_hiobActive = 0;
         }
      }
#endif /* SCSI_TARGET_OPERATION */
      OSD_COMPLETE_HIOB(hhcb->SCSI_DONE_QUEUE[origTail]);
   }
}

/*********************************************************************
*
*  SCSIrAdjustQueue
*
*     Adjust queue as the result of aborting a HIOB
*
*  Return Value:  none
*                  
*  Parameters:    pointer to queue head
*                 pointer to queue tail
*                 pointer to previous hiob
*                 current hiob
*                 
*  Remarks:
*                  
*********************************************************************/
void SCSIrAdjustQueue (SCSI_HIOB SCSI_IPTR SCSI_IPTR pqHead,
                       SCSI_HIOB SCSI_IPTR SCSI_IPTR pqTail,
                       SCSI_HIOB SCSI_IPTR SCSI_IPTR phiobPrev,
                       SCSI_HIOB SCSI_IPTR hiobCurr)
{
   /* Aborted HIOB is the only one in the queue */
   if (*pqHead == *pqTail)
   {
      /* Queue empty */
      *pqHead = *pqTail = SCSI_NULL_HIOB;
   }
   else if (hiobCurr == *pqHead)
   {
      /* Aborted HIOB is at the head of the queue */
      *pqHead = (*pqHead)->queueNext;
   }
   else
   {
      /* Aborted HIOB is either at the middle or tail of the queue */
      (*phiobPrev)->queueNext = hiobCurr->queueNext;
      if (hiobCurr == *pqTail)
      {
         /* Adjust tail */
         *pqTail = *phiobPrev;
      }
   }
}

/*********************************************************************
*
*  SCSIrFinishQueueFull
*
*     Handle Queue Full condition
*
*  Return Value:  none
*                  
*  Parameters:    hiob
*
*  Remarks:       This routine will handle queue full status for a
*                 device.  If the device has no scbs active, then 
*                 any hiob queued for waiting scb available then this
*                 new available scb will be used to continue queued hiob
*                 processing.
*                  
*********************************************************************/
void SCSIrFinishQueueFull (SCSI_HIOB SCSI_IPTR hiob)
{
   SCSI_UNIT_CONTROL SCSI_UPTR targetUnit = SCSI_TARGET_UNIT(hiob);
   SCSI_TARGET_CONTROL SCSI_HPTR targetControl = targetUnit->targetControl;

   /* Cut the original MaxTagScbs in half */
   /* if the original MaxTagScbs is greater than 1. */
   if (targetControl->origMaxTagScbs > 1)
   {
      targetControl->origMaxTagScbs = targetControl->origMaxTagScbs/2;
   }
   
   /* Clear queue full condition and dispatch all queued HIOBs. */
   SCSIrUnFreezeDeviceQueue(targetControl, SCSI_FREEZE_QFULL);
   SCSIrDispatchQueuedHIOB(hiob);
}

/*********************************************************************
*
*  SCSIrFreezeAllDeviceQueues
*
*     Freeze all device queues.
*
*  Return Value:  none
*                  
*  Parameters:    hhcb
*                 
*  Remarks:
*                  
*********************************************************************/
void SCSIrFreezeAllDeviceQueues (SCSI_HHCB SCSI_HPTR hhcb)
{
   SCSI_TARGET_CONTROL SCSI_HPTR targetControl;
   SCSI_INT i;
   SCSI_INT maxDev = SCSI_MAXDEV;

#if SCSI_DMA_SUPPORT
   /* Need to handle the special DMA queue */
   maxDev++;
#endif /* SCSI_DMA_SUPPORT */

   for (i = 0, targetControl = hhcb->SCSI_HP_targetControl; i < maxDev;
        i++, targetControl++)
   {
      /* Freeze this device queue */
      SCSIrFreezeDeviceQueue(targetControl, SCSI_FREEZE_EVENT);
   }
}

/*********************************************************************
*
*  SCSIrUnFreezeAllDeviceQueues
*
*     UnFreeze all device queues.
*
*  Return Value:  none
*                  
*  Parameters:    hhcb
*                 
*  Remarks:
*                  
*********************************************************************/
void SCSIrUnFreezeAllDeviceQueues (SCSI_HHCB SCSI_HPTR hhcb)
{
   SCSI_TARGET_CONTROL SCSI_HPTR targetControl;
   SCSI_HIOB SCSI_IPTR hiob;
   SCSI_INT i;
   SCSI_INT maxDev = SCSI_MAXDEV;

#if SCSI_DMA_SUPPORT
   /* Need to handle the special DMA queue */
   maxDev++;
#endif /* SCSI_DMA_SUPPORT */

   for (i = 0, targetControl = hhcb->SCSI_HP_targetControl; i < maxDev;
        i++, targetControl++)
   {
      /* Unfreeze this device queue */
      SCSIrUnFreezeDeviceQueue(targetControl, SCSI_FREEZE_EVENT);
      
      /* Rule of thumb, we should dispatch any pending HIOB(s) */
      /* if any after the queue unfrozen. */
      hiob = targetControl->queueHead;
      if (hiob != SCSI_NULL_HIOB)
      {
         SCSIrDispatchQueuedHIOB(hiob);
      }
   }
}

/*********************************************************************
*
*  SCSIrFreezeDeviceQueue
*
*     Freeze the device queue if the condition met
*
*  Return Value:  none
*                  
*  Parameters:    targetControl
*                 freeze-bit value
*
*  Remarks:       If the queue is not frozen then hold off all new
*                 commands for the device by assigning maxNonTagScbs
*                 and maxTagScbs to 0.  The original values are saved
*                 in origMaxNonTagScbs and origMaxTagScbs respectively.
*                  
*********************************************************************/
void SCSIrFreezeDeviceQueue (SCSI_TARGET_CONTROL SCSI_HPTR targetControl,
                             SCSI_UINT8 freezeVal)
{
   /* If the queue is not frozen, */
   /* then set the maxNonTagScbs and maxTagScbs to zero. */
   if (!targetControl->freezeMap)
   {
      if (targetControl->maxNonTagScbs > 0)
      {
         targetControl->origMaxNonTagScbs = targetControl->maxNonTagScbs;
         targetControl->maxNonTagScbs = 0;
      }

      if (targetControl->maxTagScbs > 0)
      {
         targetControl->origMaxTagScbs = targetControl->maxTagScbs;
         targetControl->maxTagScbs = 0;
      }
   }
   
   /* Set requested freeze-bit: */
   targetControl->freezeMap |= freezeVal;
}

/*********************************************************************
*
*  SCSIrUnFreezeDeviceQueue
*
*     Unfreeze the device queue if the condition met.
*
*  Return Value:  none
*                  
*  Parameters:    targetControl
*                 freeze-bit value
*
*  Remarks:
*                  
*********************************************************************/
void SCSIrUnFreezeDeviceQueue (SCSI_TARGET_CONTROL SCSI_HPTR targetControl,
                               SCSI_UINT8 freezeVal)
{
   /* If the device queue is already unfreeze, do nothing. */
   /* This is to prevent an unexpected call to unfreeze the queue. */
   if (targetControl->freezeMap)
   {
      /* Clear requested freeze-bit */
      targetControl->freezeMap &= ~freezeVal;

      /* If the queue is not frozen, */
      if (!targetControl->freezeMap)
      {
         /* then restore the original maxNonTagScbs and maxTagScbs. */
         /* Also reset both origMaxNonTagScbs and origMaxTagScbs to 0. */
         targetControl->maxNonTagScbs = targetControl->origMaxNonTagScbs;
         targetControl->origMaxNonTagScbs = 0;
         targetControl->maxTagScbs = targetControl->origMaxTagScbs;
         targetControl->origMaxTagScbs = 0;
      }
   }
}

/*********************************************************************
*
*  SCSIrDispatchQueuedHIOB
*
*     Dispatch any queued HIOB(s) that meet the criteria
*     after the device queue unfroze.
*
*  Return Value:  none
*                  
*  Parameters:    hiob
*                 freeze-bit value
*
*  Remarks:
*                  
*********************************************************************/
void SCSIrDispatchQueuedHIOB (SCSI_HIOB SCSI_IPTR hiob)
{
   SCSI_UNIT_CONTROL SCSI_UPTR targetUnit = SCSI_TARGET_UNIT(hiob);
   SCSI_TARGET_CONTROL SCSI_HPTR targetControl = targetUnit->targetControl;
   SCSI_HIOB SCSI_IPTR hiobNew;
   SCSI_UINT16 maxScbs;
   SCSI_UINT16 cnt = 0;

   /* Dispatch if the queue is not frozen, */
   if ((!targetControl->freezeMap) &&
         ((hiobNew = SCSIrDeviceQueueRemove(targetControl)) != SCSI_NULL_HIOB))
   {
      /* Get the criteria factor */
      maxScbs = ((hiobNew->SCSI_IF_tagEnable) ? targetControl->maxTagScbs :
                                                targetControl->maxNonTagScbs);
      SCSIrAllocateScb(targetUnit->hhcb,hiobNew);
      maxScbs--;

      /* Walk down device queue and execute until new criteria is reached     */
      /* or device queue is empty.  To enter this function, the all active    */
      /* cmds for the device must have been completed.  Therefore, the cnt    */
      /* in the loop below can start at zero. Also, there is no need to       */
      /* adjust targetControl->activeScbs since it was already updated when   */
      /* the scb was put into the device queue in the first place.            */
      while(1)
      {
         if (++cnt > maxScbs)
         {
            break;
         }
         if ((hiobNew = SCSIrDeviceQueueRemove(targetControl)) != SCSI_NULL_HIOB)
         {
            SCSIrAllocateScb(targetUnit->hhcb,hiobNew);
         }
         else
         {
            break;
         }
      }
   }
}

#if SCSI_TARGET_OPERATION 
/*********************************************************************
*
*  SCSIrAbortTargetHIOB
*
*     Setup abort status and call OSD_COMPLETE_SPECIAL_HIOB
*     for Target mode HIOB 
*
*  Return Value:  none
*                  
*  Parameters:    hiob to be aborted
*                 host adapter status
*                 
*  Remarks:
*                  
*********************************************************************/
void SCSIrAbortTargetHIOB (SCSI_HIOB SCSI_IPTR hiob,
                     SCSI_UEXACT8 haStatus)
{
   SCSI_HHCB SCSI_HPTR hhcb = SCSI_NEXUS_UNIT(hiob)->hhcb;
   
   SCSI_NEXUS_UNIT(hiob)->SCSI_XP_hiobActive = 0;

   /* Set up abort status */
   hiob->stat = SCSI_SCB_ABORTED;
   hiob->haStat = haStatus;
   
   /* Complete HIOB */
   OSD_COMPLETE_SPECIAL_HIOB(hiob);
}

/*********************************************************************
*
*  SCSIrFindAndAbortTargetHIOBs
*
*     Find and abort Target Mode HIOB(s) in Host queue that match the
*     conditions specified by the haStat parameters. 
*
*  Return Value:  the number of HIOBs aborted
*                  
*  Parameters:    hhcb
*                 nexus
*                 haStat - contains abort reason 
*                                 
*  Remarks:
*                  
*********************************************************************/
SCSI_UEXACT8 SCSIrFindAndAbortTargetHIOBs (SCSI_HHCB SCSI_HPTR hhcb,
                            SCSI_NEXUS  SCSI_XPTR nexus,
                            SCSI_UEXACT8 haStat)
{
   SCSI_HIOB SCSI_IPTR hiobCurr;
   SCSI_HIOB SCSI_IPTR hiobPrev;  
   SCSI_HIOB SCSI_IPTR hiobNext;
   SCSI_BOOLEAN conditionMet = 0;
   SCSI_UEXACT8 numAborted = 0;

   hiobCurr = hiobPrev = hhcb->SCSI_WAIT_HEAD;

   while (hiobCurr != SCSI_NULL_HIOB)
   {
      if (hiobCurr->SCSI_IP_targetMode)
      {
         /* Only consider Target Mode IOBs in the queue */

         switch (haStat)
         {
            case SCSI_HOST_TARGET_RESET:
               /* SCSI-2 Bus device reset or SCSI-3 Target Reset */
               /* abort all target mode IOBs */
               conditionMet = 1; 
               break;

            case SCSI_HOST_ABORT_TASK_SET:
               /* SCSI-2 Abort or SCSI-3 Abort Task Set */
               /* Abort IOBs identified by SCSI ID and Lun */ 
            
               if ((SCSI_NEXUS_UNIT(hiobCurr)->scsiID == nexus->scsiID) &&
                   (SCSI_NEXUS_UNIT(hiobCurr)->lunID == nexus->lunID ))
               {
                  conditionMet = 1; 
               }
               break;

            case SCSI_HOST_ABORT_TASK:
               /* SCSI-2 Abort Tag or SCSI-3 AbortTask */
               /* Aborts IOBs identified by SCSI ID, Lun, and Tag */
               if ((SCSI_NEXUS_UNIT(hiobCurr)->scsiID == nexus->scsiID) &&
                   (SCSI_NEXUS_UNIT(hiobCurr)->lunID == nexus->lunID) &&
                   (SCSI_NEXUS_UNIT(hiobCurr)->queueTag = nexus->queueTag))
               {
                  conditionMet = 1; 
               }
               break;
         
            case SCSI_HOST_CLEAR_TASK_SET:
               /* SCSI-2 Clear Queue or SCSI-3 ClearTask Set */
               /* Abort IOBs identified by lun */ 
               if (SCSI_NEXUS_UNIT(hiobCurr)->lunID == nexus->lunID)
               {
                  conditionMet = 1;
               }
               break;

            case SCSI_HOST_TERMINATE_TASK:
               /* SCSI-2 Terminate I/O process or SCSI-3 Terminate Task */
               /* Do nothing for now */
               break;

            case SCSI_HOST_ABT_HOST:
               /* Abort due to a SCSI_ABORT_NEXUS  */
               if (SCSI_NEXUS_UNIT(hiobCurr) == nexus)
               {
                  conditionMet = 1;
               }
               break;
                   
            default:
               /* should never get here */
               break;

         } 
      }
      
      /* save the next HIOB as hiobCurr may be posted back to OSM */
      hiobNext = hiobCurr->queueNext; 
                
      if (conditionMet)
      {
                  
         /* Mending Host queue */
         SCSIrAdjustQueue(&hhcb->SCSI_WAIT_HEAD, &hhcb->SCSI_WAIT_TAIL,
                          &hiobPrev, hiobCurr);
                          
         /* Post aborted HIOB back to OSM */
         SCSIrAbortTargetHIOB(hiobCurr, haStat);
         numAborted++;       
         conditionMet = 0;
      }
      else
      {
         hiobPrev = hiobCurr; 
      }
      
      /* Next HIOB in queue */
      hiobCurr = hiobNext;
   }

   return(numAborted);

}

/*********************************************************************
*
*  SCSIrSwappingReturnEstScb
*
*     Swapping return est scb to free pool. If there are
*     resources available (HIOB and NEXUS) then requeue to HWM layer. 
*     
*
*  Return Value:  none
*                  
*  Parameters:    hhcb
*                 hiob containing scbDescriptor to be returned
*
*  Remarks:
*                  
*********************************************************************/
#if SCSI_SWAPPING_ADVANCED_MODE || SCSI_SWAPPING_160M_MODE
void SCSIrSwappingReturnEstScb (SCSI_HHCB SCSI_HPTR hhcb,
                                SCSI_HIOB SCSI_IPTR hiob)
{

   /* return est SCB to available list so that linked properly */
   hhcb->SCSI_FREE_EST_SCB_AVAIL++;
   hhcb->SCSI_FREE_EST_TAIL->queueNext = hiob->scbDescriptor;
   hhcb->SCSI_FREE_EST_TAIL = hiob->scbDescriptor;

}
#endif /* SCSI_SWAPPING_ADVANCED_MODE || SCSI_SWAPPING_160M_MODE */

/*********************************************************************
*
*  SCSIrSwappingGetEstScb
*
*     Get free establish connection scb if available
*
*  Return Value:  none
*                  
*  Parameters:    hhcb
*
*  Remarks:
*                  
*********************************************************************/
#if SCSI_SWAPPING_ADVANCED_MODE || SCSI_SWAPPING_160M_MODE
SCSI_UEXACT8 SCSIrSwappingGetEstScb (SCSI_HHCB SCSI_HPTR hhcb,
                                     SCSI_HIOB SCSI_IPTR hiob)
{

   /* no est scb available */
   if (hhcb->SCSI_FREE_EST_SCB_AVAIL < 2)
   {
      if (hhcb->SCSI_HF_targetNumberEstScbs != 1)
      {
         return(SCSI_NULL_SCB);
      }
      else
      if (hhcb->SCSI_FREE_EST_SCB_AVAIL < 1)
      {   
         /* if hhcb->SCSI_HF_targetNumberEstScbs == 1 then one Establish
          * connection SCB may be used.
          */
         return(SCSI_NULL_SCB);
      }   
   }
   
   /* allocate as est SCB */    
   hhcb->SCSI_FREE_EST_SCB_AVAIL--;
   hiob->scbDescriptor = hhcb->SCSI_FREE_EST_HEAD;
   hhcb->SCSI_FREE_EST_HEAD = hhcb->SCSI_FREE_EST_HEAD->queueNext;
   
   return(hiob->scbDescriptor->scbNumber);
   
}
#endif /* SCSI_SWAPPING_ADVANCED_MODE || SCSI_SWAPPING_160M_MODE */

/*********************************************************************
*
*  SCSIrQueueEstHiobs
*
*     Queue establish Connection HIOBs from HIOB queue
*
*  Return Value:  none
*                  
*  Parameters:    hhcb, hiob
*
*  Remarks:
*                  
*********************************************************************/
void SCSIrQueueEstHiobs(SCSI_HHCB SCSI_HPTR hhcb)
{

   SCSI_HIOB SCSI_IPTR hiob;
   SCSI_NEXUS SCSI_XPTR nexus;

   while ((hiob = SCSIrHiobPoolRemove(hhcb)) != SCSI_NULL_HIOB)
   {
      if ((nexus = SCSIrAllocateNexus(hhcb)) == SCSI_NULL_NEXUS)
      {
         SCSIrQueueEstHiobPool(hhcb,hiob);
         break;
      }
      else
      {
         if ((hiob->scbNumber = SCSI_rGETESTSCB(hhcb,hiob)) != SCSI_NULL_SCB)
         {
            SCSI_NEXUS_UNIT(hiob) = nexus;
            nexus->hhcb = hhcb;
            hiob->queueNext = SCSI_NULL_HIOB;
            SCSIHQueueSpecialHIOB(hiob);
         }  
         else
         {
            /* free the nexus */
            SCSIrFreeNexus(hhcb,nexus);
            /* Queue to Est HIOB queue */
            SCSIrQueueEstHiobPool(hhcb,hiob);  
            break;
         }
      }    
   }      

}


/*********************************************************************
*
*  SCSIrAbortHIOBInHiobQ
*
*     Abort the aborting HIOB in HIOB Pool queue if it found
*
*  Return Value:  0 - aborting HIOB not found
*                 1 - aborting HIOB found
*                  
*  Parameters:    aborting hiob
*                 host adapter status
*                 
*  Remarks:
*                 This routine is only used in Target mode, but is 
*                 compiled in initiator mode.             
*********************************************************************/
SCSI_UEXACT8 SCSIrAbortHIOBInHiobQ (SCSI_HIOB SCSI_IPTR hiob,
                                   SCSI_UEXACT8 haStat)
{
   SCSI_UEXACT8 retStat = 0;
   SCSI_HHCB SCSI_HPTR hhcb;
   SCSI_HIOB SCSI_IPTR hiobCurr;
   SCSI_HIOB SCSI_IPTR hiobPrev;
   SCSI_UEXACT16 queueCnt;

   /* Determine if related HIOB is target mode Establish    */
   /* Connection HIOB. If not then don't search this queue. */
   if (hiob->SCSI_IP_targetMode &&
       hiob->cmd == SCSI_CMD_ESTABLISH_CONNECTION) 
   {
      hhcb = SCSI_INITIATOR_UNIT(hiob);
      /* if there are no HIOBs in queue then return. */
      if (hhcb->SCSI_HF_targetHiobQueueCnt == 0)
      {
         return(retStat);
      }
   }
   else
   {
      return(retStat);
   }

   hiobCurr = hiobPrev = hhcb->SCSI_HF_targetHiobQueue;
   
   while (hiobCurr != SCSI_NULL_HIOB)
   {
      /* Abort current HIOB if it matches */
      if (hiobCurr == hiob)
      {
         if (hiobCurr = hiobPrev)
         {
            /* Aborted HIOB is at the head of queue */
            hhcb->SCSI_HF_targetHiobQueue = hiobCurr->queueNext;
         }
         else
         {
            /* Aborted HIOB is in the middle or tail of the queue */ 
            hiobPrev->queueNext = hiobCurr->queueNext;
         }
         queueCnt = hhcb->SCSI_HF_targetHiobQueueCnt;
         queueCnt--;
         hhcb->SCSI_HF_targetHiobQueueCnt = queueCnt;
         
         /* Check threshold */
         if (hhcb->SCSI_HF_targetHiobQueueThreshold)
         {
            /* Check if OSMEvent to be sent */   
            if (queueCnt == hhcb->SCSI_HF_targetHiobQueueThreshold)
            {
               /* send a Alert to OSM */
               SCSIrAsyncEvent(hhcb,SCSI_AE_EC_IOB_THRESHOLD);
            }
         } 
         
         /* Complete HIOB */
         hiobCurr->stat = SCSI_SCB_ABORTED;
         hiobCurr->haStat = haStat;
         OSD_COMPLETE_HIOB(hiobCurr);                  
         retStat = 1;
         break;
      }
      else
      {
         /* Aborting HIOB have not found yet, advance to the next HIOB */
         hiobPrev = hiobCurr;
         hiobCurr = hiobCurr->queueNext;
      }
   }  /* end of while loop */

   return(retStat);
}
#endif /* SCSI_TARGET_OPERATION */
