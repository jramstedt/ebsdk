/*
 xlminit.c
 xlmtask.c
 xlmutil.c
*/

/*$Header:   Y:/source/chimscsi/src/XLMINIT.CV_   1.56.8.8   13 Apr 1998 21:50:26   THU  $*/
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
*  Module Name:   XLMINIT.C
*
*  Description:
*                 Codes to implement Common HIM interface for its 
*                 initialization
*
*  Owners:        ECX IC Firmware Team
*    
*  Notes:         NONE
*
*  Entry Point(s):
*                 SCSIGetFunctionPointers
*                 SCSIGetNextHostDeviceType
*                 SCSICreateInitializationTSCB
*                 SCSIGetConfiguration
*                 SCSISetConfiguration
*                 SCSISizeAdapterTSCB
*                 SCSICreateAdapterTSCB
*                 SCSISetupAdapterTSCB
*                 SCSICheckMemoryNeeded
*                 SCSISetMemoryPointer
*                 SCSIVerifyAdapter
*                 SCSIInitialize
*                 SCSISizeTargetTSCB
*                 SCSICheckTargetTSCBNeeded
*                 SCSICreateTargetTSCB
*
***************************************************************************/

#define  SCSI_XLM_DEFINE

#include "scsi.h"
#include "xlm.h"

/*********************************************************************
*
*  SCSIGetFunctionPointers
*
*     Get default configuration information
*
*  Return Value:  none
*                  
*  Parameters:    himFunctions
*                 length
*
*  Remarks:       This routine will fill function pointer as interface
*                 to CHIM. This is the first routine get called by OSM.
*
*********************************************************************/
void SCSIGetFunctionPointers (HIM_FUNC_PTRS HIM_PTR himFunctions,
                              HIM_UINT16 length)
{
   /* copy scsi chim function pointer table over */
   OSDmemcpy(himFunctions,&SCSIFuncPtrs,length);
}

/*********************************************************************
*
*  SCSIGetNextHostDeviceType
*
*     This routine returns the next host device type supported by
*     SCSI CHIM.
*
*  Return Value:  0 - No more device type
*                 host id
*                  
*  Parameters:    index
*                 hostBusType
*                 mask
*
*  Remarks:       SCSI CHIM will make a host device type table built
*                 at compile time. Parameter index will be used to
*                 index into that table. The table itself should be
*                 static at initialization/run time. The table is
*                 maintained at CHIM layer.
*
*********************************************************************/
HIM_HOST_ID SCSIGetNextHostDeviceType (HIM_UINT16 index,
                                       HIM_UINT8 HIM_PTR hostBusType,
                                       HIM_HOST_ID HIM_PTR mask)
{
   HIM_HOST_ID hostID = 0;

   /* make sure initialization TSCB is large enough */
   if ((HIM_SCSI_INIT_TSCB_SIZE >= sizeof(SCSI_INITIALIZATION_TSCB)) &&
       (sizeof(SCSI_NVM_LAYOUT) == 64) &&
       (sizeof(SCSI_BIOS_SCB2) == 32))
   {
      /* make sure index is valid to index into the table */
      /* and the entry is not the delimeter.              */
      if ((index < (sizeof(SCSIHostType) / sizeof(SCSI_HOST_TYPE))) &&
          (SCSIHostType[index].idHost != 0))
      {
         *hostBusType = HIM_HOST_BUS_PCI;
         hostID = SCSIHostType[index].idHost;
         *mask = SCSIHostType[index].idMask;
      }
   }

   return(hostID);
}

/*********************************************************************
*
*  SCSICreateInitializationTSCB
*
*     This routine pass the initializationTSCB from OSM to CHIM.
*
*  Return Value:  initialization TSH
*                  
*  Parameters:    pointer to initialization TSCB
*
*  Remarks:       This routine will basically format/prepare the
*                 memory (pointed by initializationTSCB) ready to
*                 continue the initialization process. The pointer
*                 to initialization TSCB will be returned as
*                 initialization TSH. 
*
*********************************************************************/
HIM_TASK_SET_HANDLE SCSICreateInitializationTSCB (void HIM_PTR pHIMInitTSCB)
{
   SCSI_MEMORY_TABLE HIM_PTR memoryTable = SCSI_INITSH(pHIMInitTSCB)->memoryTable;

   /* set TSCB type */
   SCSI_INITSH(pHIMInitTSCB)->typeTSCB = SCSI_TSCB_TYPE_INITIALIZATION;

   /* copy default configuration setup */
   OSDmemcpy(&SCSI_INITSH(pHIMInitTSCB)->configuration,
            &SCSIConfiguration,sizeof(HIM_CONFIGURATION));

   /* get memory table for all sequencer modes */
   /* assume maximum number of scbs required */

   /* Maximum memory cannot be determined for target mode */
   /* operation until SCSISetConfiguration is invoked.    */
   /* Therefore, just set the nexusTSH and nodeTSH        */
   /* parameters to 0.                                    */

#if SCSI_SWAPPING32_MODE
   SCSI_GET_MEMORY_TABLE(SCSI_FMODE_SWAPPING32,SCSI_MAXSCBS,0,
                         0,&memoryTable[0]);
#else
   OSDmemset(&memoryTable[0],0,sizeof(SCSI_MEMORY_TABLE));
#endif
 
#if SCSI_STANDARD64_MODE
   SCSI_GET_MEMORY_TABLE(SCSI_FMODE_STANDARD64,SCSI_MAXSCBS,0,
                         0,&memoryTable[1]);
#else
   OSDmemset(&memoryTable[1],0,sizeof(SCSI_MEMORY_TABLE));
#endif

   SCSIxMaximizeMemory(&memoryTable[1],&memoryTable[0]);

#if SCSI_SWAPPING64_MODE
   SCSI_GET_MEMORY_TABLE(SCSI_FMODE_SWAPPING64,SCSI_MAXSCBS,0,
                         0,&memoryTable[2]);
#else
   OSDmemset(&memoryTable[2],0,sizeof(SCSI_MEMORY_TABLE));
#endif

   SCSIxMaximizeMemory(&memoryTable[1],&memoryTable[2]); /* keep standard mode in position 1 */

#if SCSI_STANDARD_ADVANCED_MODE
   SCSI_GET_MEMORY_TABLE(SCSI_FMODE_STANDARD_ADVANCED,SCSI_MAXSCBS,0,
                         0,&memoryTable[3]);
#else
   OSDmemset(&memoryTable[3],0,sizeof(SCSI_MEMORY_TABLE));
#endif

#if SCSI_SWAPPING_ADVANCED_MODE
   SCSI_GET_MEMORY_TABLE(SCSI_FMODE_SWAPPING_ADVANCED,SCSI_MAXSCBS,0,
                         0,&memoryTable[4]);
#else
   OSDmemset(&memoryTable[4],0,sizeof(SCSI_MEMORY_TABLE));
#endif

   SCSIxMaximizeMemory(&memoryTable[3],&memoryTable[4]); /* keep standard mode in position 1 */

#if SCSI_STANDARD_160M_MODE
   SCSI_GET_MEMORY_TABLE(SCSI_FMODE_STANDARD_160M,SCSI_MAXSCBS,0,
                         0,&memoryTable[5]);
#else
   OSDmemset(&memoryTable[5],0,sizeof(SCSI_MEMORY_TABLE));
#endif

#if SCSI_SWAPPING_160M_MODE
   SCSI_GET_MEMORY_TABLE(SCSI_FMODE_SWAPPING_160M,SCSI_MAXSCBS,0,
                         0,&memoryTable[6]);
#else
   OSDmemset(&memoryTable[6],0,sizeof(SCSI_MEMORY_TABLE));
#endif

   SCSIxMaximizeMemory(&memoryTable[5],&memoryTable[6]); /* keep standard mode in position 1 */

   return((HIM_TASK_SET_HANDLE) pHIMInitTSCB);
}

/*********************************************************************
*
*  SCSIxMaximizeMemory
*
*     Get the worst memory requirement of both memory table
*
*  Return Value:  none
*                  
*  Parameters:    memoryTable 1
*                 memoryTable 2
*
*  Remarks:       memoryTable 2 should never be overwritten!
*
*********************************************************************/
void SCSIxMaximizeMemory (SCSI_MEMORY_TABLE HIM_PTR memoryTable1,
                          SCSI_MEMORY_TABLE HIM_PTR memoryTable2)
{
   int i;

   /* assume the worst memory requirements */
   for (i = 0; i < SCSI_MAX_MEMORY; i++)
   {
      /* Take the worst size */
      if (memoryTable1->memory[i].memorySize <
         memoryTable2->memory[i].memorySize)
      {
         memoryTable1->memory[i].memorySize =
            memoryTable2->memory[i].memorySize;
      }

      /* take the worst alignment */
      if (memoryTable1->memory[i].memoryAlignment <
         memoryTable2->memory[i].memoryAlignment)
      {
         memoryTable1->memory[i].memoryAlignment =
            memoryTable2->memory[i].memoryAlignment;
      }
   }
}

/*********************************************************************
*
*  SCSIGetConfiguration
*
*     Get the default configuration information
*
*  Return Value:  none
*                  
*  Parameters:    initialization TSH
*                 config structure
*                 product id
*
*  Remarks:       This routine will get the default configuration based
*                 on product id available. A particular hardware
*                 device has not been identified so there is no way
*                 to access to the hardware at the moment.
*
*********************************************************************/
void SCSIGetConfiguration (HIM_TASK_SET_HANDLE initializationTSH,
                           HIM_CONFIGURATION HIM_PTR pConfig,     
                           HIM_HOST_ID productID)
{
   /* copy the current configuration setup over */
   OSDmemcpy(pConfig,&SCSI_INITSH(initializationTSH)->configuration,
      sizeof(HIM_CONFIGURATION));
}

/*********************************************************************
*
*  SCSISetConfiguration
*
*     Apply the configuration based on associaed config structure
*
*  Return Value:  HIM_SUCCESS - updates applied
*                 HIM_FAILURE - productID was invalid
*                 HIM_ILLEGAL_CHANGE - illegal change
*                  
*  Parameters:    initialization TSH
*                 config structure
*                 product id
*
*  Remarks:       This routine will apply the configuration specified
*                 with config structure and keep it in the
*                 initialization TSCB. Hardware is still not available
*                 the moment. 
*
*********************************************************************/
HIM_UINT8 SCSISetConfiguration (HIM_TASK_SET_HANDLE initializationTSH,
                                HIM_CONFIGURATION HIM_PTR pConfig,     
                                HIM_HOST_ID productID)
{
   SCSI_MEMORY_TABLE HIM_PTR memoryTable = SCSI_INITSH(initializationTSH)->memoryTable;
   HIM_UINT8 status = HIM_ILLEGAL_CHANGE;
   SCSI_UEXACT16 nexusHandles = 0;
   SCSI_UEXACT8 nodeHandles = 0; 
#if (SCSI_STANDARD_ADVANCED_MODE + SCSI_SWAPPING_ADVANCED_MODE +\
     SCSI_STANDARD_160M_MODE + SCSI_SWAPPING_160M_MODE)
   SCSI_UEXACT32 blocks;
#endif

   /* Check the following for a valid request:                      */
   /*     make sure number of scbs is within the valid range        */
   /*     memoryMapped <= HIM_MIXED_RANGES                          */
   /*     targetMode = 1 OR/AND initiatorMode = 1                   */
   /*     if targetMode = 1 then                                    */
   /*             0 < targetMaxNexusHandles <= SCSI_MAXNEXUSHANDLES */      
   /*             0 < targetMaxNodeHandles <= SCSI_MAXNODES         */
   /*             0 < targetNumIDs <= HIM_MAX_SCSI_ADAPTER_IDS      */
   if ((pConfig->maxInternalIOBlocks <= SCSI_MAXSCBS) &&
       (pConfig->maxInternalIOBlocks >= SCSI_MINSCBS) &&
       (pConfig->iobReserveSize == SCSIConfiguration.iobReserveSize) &&
       (pConfig->memoryMapped <= HIM_MIXED_RANGES) && 
       (pConfig->initiatorMode || pConfig->targetMode) &&
#if SCSI_INITIATOR_OPERATION && !SCSI_TARGET_OPERATION
       (pConfig->targetMode == HIM_FALSE) &&
#endif
#if SCSI_TARGET_OPERATION && !SCSI_INITIATOR_OPERATION
       (pConfig->initiatorMode == HIM_FALSE) &&
#endif
       (
#if SCSI_TARGET_OPERATION
        (pConfig->targetMode && 
         (pConfig->targetNumNexusTaskSetHandles > 0) &&
         (pConfig->targetNumNexusTaskSetHandles <= SCSI_MAXNEXUSHANDLES) &&
#if SCSI_MULTIPLEID_SUPPORT
         (pConfig->targetNumIDs > 0) &&
         (pConfig->targetNumIDs <= HIM_MAX_SCSI_ADAPTER_IDS) &&
#else
         (pConfig->targetNumIDs == 1) &&
#endif /* SCSI_MULTIPLEID_SUPPORT */
         (pConfig->targetNumNodeTaskSetHandles > 0) &&
         (pConfig->targetNumNodeTaskSetHandles <= SCSI_MAXNODES)) ||
#endif /* SCSI_TARGET_OPERATION */
         ((!pConfig->targetMode))))
   {
#if SCSI_TARGET_OPERATION
      if (pConfig->targetMode == HIM_TRUE)
      {
         nexusHandles = (SCSI_UEXACT16)pConfig->targetNumNexusTaskSetHandles;
         nodeHandles = (SCSI_UEXACT8)pConfig->targetNumNodeTaskSetHandles;
      } 
#endif /* SCSI_TARGET_OPERATION */
       
      /* update memory allocation table if maxInternalIOBlocks,     */
      /* targetMaxNexusHandles, or targetMaxNodeHandles get changed */
     
      if ((pConfig->maxInternalIOBlocks !=
           SCSI_INITSH(initializationTSH)->configuration.maxInternalIOBlocks) ||
          (nexusHandles != 
           SCSI_INITSH(initializationTSH)->configuration.targetNumNexusTaskSetHandles) ||
          (nodeHandles !=
           SCSI_INITSH(initializationTSH)->configuration.targetNumNodeTaskSetHandles))
      {
         /* assume maximum number of scbs required */
         /* and figure out the worst combinations */
#if SCSI_SWAPPING32_MODE
         SCSI_GET_MEMORY_TABLE(SCSI_FMODE_SWAPPING32,
                  (SCSI_UEXACT8)pConfig->maxInternalIOBlocks,
                  nexusHandles,
                  nodeHandles,
                  &memoryTable[0]);
#else
         OSDmemset(&memoryTable[0],0,sizeof(SCSI_MEMORY_TABLE));
#endif

         /* even if we have identified Excalibur it may still */
         /* operate either swapping 32 or standard 64 mode */
#if SCSI_STANDARD64_MODE
         SCSI_GET_MEMORY_TABLE(SCSI_FMODE_STANDARD64,
                  (SCSI_UEXACT8)pConfig->maxInternalIOBlocks,
                  nexusHandles,
                  nodeHandles,
                  &memoryTable[1]);
#else
         OSDmemset(&memoryTable[1],0,sizeof(SCSI_MEMORY_TABLE));
#endif

         SCSIxMaximizeMemory(&memoryTable[1],&memoryTable[0]);

         
         /* keep standard mode in position 1 */
#if SCSI_SWAPPING64_MODE
         SCSI_GET_MEMORY_TABLE(SCSI_FMODE_SWAPPING64,
                  (SCSI_UEXACT8)pConfig->maxInternalIOBlocks,
                  nexusHandles,
                  nodeHandles,
                  &memoryTable[2]);
#else
         OSDmemset(&memoryTable[2],0,sizeof(SCSI_MEMORY_TABLE));
#endif

         SCSIxMaximizeMemory(&memoryTable[1],&memoryTable[2]);
         
#if SCSI_STANDARD_ADVANCED_MODE
         /* maxInternalIOBlocks needs to be rounded to its lower multiple of  */
         /* powerof(2), as SCB_QSIZE in QOFF_CTLSTA register for bayonet      */
         /* takes values in terms of powerof(2).                              */
         blocks = (SCSI_UEXACT32)pConfig->maxInternalIOBlocks+1;
         while (blocks)
         {
            pConfig->maxInternalIOBlocks = (HIM_UINT32)(blocks - 1);
            blocks &= (blocks-1);
         }

         SCSI_GET_MEMORY_TABLE(SCSI_FMODE_STANDARD_ADVANCED,
                  (SCSI_UEXACT8) pConfig->maxInternalIOBlocks,
                  nexusHandles,
                  nodeHandles,
                  &memoryTable[3]);
#else
         OSDmemset(&memoryTable[3],0,sizeof(SCSI_MEMORY_TABLE));
#endif

         /* keep standard mode in position 1 */
#if SCSI_SWAPPING_ADVANCED_MODE
         /* maxInternalIOBlocks needs to be rounded to its lower multiple of  */
         /* powerof(2), as SCB_QSIZE in OFF_CTLSTA register for bayonet       */
         /* takes values in terms of powerof(2).                              */
         blocks = (SCSI_UEXACT32)pConfig->maxInternalIOBlocks+1;
         while (blocks)
         {
            pConfig->maxInternalIOBlocks = (HIM_UINT32)(blocks - 1);
            blocks &= (blocks-1);
         }

         SCSI_GET_MEMORY_TABLE(SCSI_FMODE_SWAPPING_ADVANCED,
                  (SCSI_UEXACT8) pConfig->maxInternalIOBlocks,
                  nexusHandles,
                  nodeHandles,
                  &memoryTable[4]);
#else
         OSDmemset(&memoryTable[4],0,sizeof(SCSI_MEMORY_TABLE));
#endif

         SCSIxMaximizeMemory(&memoryTable[3],&memoryTable[4]);

#if SCSI_STANDARD_160M_MODE
         /* maxInternalIOBlocks needs to be rounded to its lower multiple of  */
         /* powerof(2), as SCB_QSIZE in QOFF_CTLSTA register for trident      */
         /* takes values in terms of powerof(2).                              */
         blocks = (SCSI_UEXACT32)pConfig->maxInternalIOBlocks+1;
         while (blocks)
         {
            pConfig->maxInternalIOBlocks = (HIM_UINT32)(blocks - 1);
            blocks &= (blocks-1);
         }

         SCSI_GET_MEMORY_TABLE(SCSI_FMODE_STANDARD_160M,
                  (SCSI_UEXACT8) pConfig->maxInternalIOBlocks,
                  nexusHandles,
                  nodeHandles,
                  &memoryTable[5]);
#else
         OSDmemset(&memoryTable[5],0,sizeof(SCSI_MEMORY_TABLE));
#endif

#if SCSI_SWAPPING_160M_MODE
         /* maxInternalIOBlocks needs to be rounded to its lower multiple of  */
         /* powerof(2), as SCB_QSIZE in OFF_CTLSTA register for bayonet takes */
         /* values in terms of powerof(2).                                    */
         blocks = (SCSI_UEXACT32)pConfig->maxInternalIOBlocks+1;
         while (blocks)
         {
            pConfig->maxInternalIOBlocks = (HIM_UINT32)(blocks - 1);
            blocks &= (blocks-1);
         }

         SCSI_GET_MEMORY_TABLE(SCSI_FMODE_SWAPPING_160M,
                  (SCSI_UEXACT8) pConfig->maxInternalIOBlocks,
                  nexusHandles,
                  nodeHandles,
                  &memoryTable[6]);
#else
         OSDmemset(&memoryTable[6],0,sizeof(SCSI_MEMORY_TABLE));
#endif
         /* keep standard mode in position 1 */
         SCSIxMaximizeMemory(&memoryTable[5],&memoryTable[6]);
      }

      /* keep configuration setup up to date */
      OSDmemcpy(&SCSI_INITSH(initializationTSH)->configuration,
               pConfig,sizeof(HIM_CONFIGURATION));

      SCSI_INITSH(initializationTSH)->configuration.targetNumNexusTaskSetHandles =
          nexusHandles;
      SCSI_INITSH(initializationTSH)->configuration.targetNumNodeTaskSetHandles =
          nodeHandles;

      /* Doesn't affect memory requirements so just copy passed in value. */ 
      SCSI_INITSH(initializationTSH)->configuration.targetNumIDs =
          pConfig->targetNumIDs;

      status = HIM_SUCCESS;
   }

   return(status);
}

/*********************************************************************
*
*  SCSISizeAdapterTSCB
*
*     Get the size requirement for adapter TSCB
*
*  Return Value:  size required for adapter TSCB
*                  
*  Parameters:    initialization TSH
*                 product id
*
*  Remarks:       This routine may take advantage of configuration
*                 recorded at SCSISetConfiguration call to optimize
*                 the memory required for adapter TSCB.
*
*********************************************************************/
HIM_UINT32 SCSISizeAdapterTSCB (HIM_TASK_SET_HANDLE initializationTSH,
                                HIM_HOST_ID productID)
{
   return(sizeof(SCSI_ADAPTER_TSCB));
}

/*********************************************************************
*
*  SCSICreateAdapterTSCB
*
*     Create and initialize adapter TSCB
*
*  Return Value:  adapter TSH
*                 0 - failure with adapter TSCB initialization
*                  
*  Parameters:    initialization TSH
*                 pointer to adapter TSCB
*                 pointer to osm adapter context
*                 host device address 
*                 product id
*
*  Remarks:       Information kept at initialization TSCB should be
*                 copied over to adapter TSCB. HHCB information 
*                 prepared in initialization TSCB is a typical example
*                 to be copied over to adapter TSCB.
*
*********************************************************************/
HIM_TASK_SET_HANDLE SCSICreateAdapterTSCB (HIM_TASK_SET_HANDLE initializationTSH,
                                           void HIM_PTR tscbPointer,
                                           void HIM_PTR osmAdapterContext,
                                           HIM_HOST_ADDRESS hostAddress,
                                           HIM_HOST_ID productID)
{
   SCSI_HHCB HIM_PTR hhcb = &SCSI_ADPTSH(tscbPointer)->hhcb;
   HIM_UINT16 nexusHandles = (HIM_UINT16)
      SCSI_INITSH(initializationTSH)->configuration.targetNumNexusTaskSetHandles;

#if SCSI_TARGET_OPERATION
   HIM_UINT8 nodeHandles = (HIM_UINT8)
      SCSI_INITSH(initializationTSH)->configuration.targetNumNodeTaskSetHandles; 
#endif /* SCSI_TARGET_OPERATION */      

   /* set type of TSCB */
   SCSI_ADPTSH(tscbPointer)->typeTSCB = SCSI_TSCB_TYPE_ADAPTER;
   SCSI_ADPTSH(tscbPointer)->hhcb.numberScbs = (SCSI_UEXACT8)
         SCSI_INITSH(initializationTSH)->configuration.maxInternalIOBlocks;

   SCSI_ADPTSH(tscbPointer)->memoryMapped = 
         SCSI_INITSH(initializationTSH)->configuration.memoryMapped;
   SCSI_ADPTSH(tscbPointer)->osmAdapterContext = osmAdapterContext;

   /* store the number of nexus and node TSH's to be allocated,
      initiatorMode, and targetMode settings */
   SCSI_ADPTSH(tscbPointer)->hhcb.SCSI_HF_initiatorMode = 
         SCSI_INITSH(initializationTSH)->configuration.initiatorMode;

   if (SCSI_INITSH(initializationTSH)->configuration.targetMode && 
       nexusHandles > 0)
   {     
      SCSI_ADPTSH(tscbPointer)->hhcb.SCSI_HF_targetMode = 1;
   }
   else
   {
      SCSI_ADPTSH(tscbPointer)->hhcb.SCSI_HF_targetMode = 0;
      nexusHandles = 0;
#if SCSI_TARGET_OPERATION      
      nodeHandles = 0;      
#endif /* SCSI_TARGET_OPERATION */      
   }

#if SCSI_TARGET_OPERATION
   SCSI_ADPTSH(tscbPointer)->hhcb.SCSI_HF_targetNumberNexusHandles =
       (SCSI_UEXACT16)nexusHandles;
   SCSI_ADPTSH(tscbPointer)->hhcb.SCSI_HF_targetNumberNodeHandles =
       (SCSI_UEXACT8)nodeHandles;
   SCSI_ADPTSH(tscbPointer)->hhcb.SCSI_HF_targetNumIDs =
       (SCSI_UEXACT8)SCSI_INITSH(initializationTSH)->configuration.targetNumIDs;
#endif /* SCSI_TARGET_OPERATION */

   hhcb->hardwareMode = SCSI_HMODE_AIC78XX;
   /* copy memory table over depending on product id */
   if (SCSI_IS_EXCALIBUR_ID(productID))
   {
#if SCSI_STANDARD64_MODE
      hhcb->firmwareMode = SCSI_FMODE_STANDARD64;
      OSDmemcpy(&SCSI_ADPTSH(tscbPointer)->memoryTable,
               &SCSI_INITSH(initializationTSH)->memoryTable[1],
               sizeof(SCSI_MEMORY_TABLE));
#else  /* !SCSI_STANDARD64_MODE */
#if OSM_BUS_ADDRESS_SIZE == 64
      hhcb->firmwareMode = SCSI_FMODE_SWAPPING64;
      OSDmemcpy(&SCSI_ADPTSH(tscbPointer)->memoryTable,
               &SCSI_INITSH(initializationTSH)->memoryTable[2],
               sizeof(SCSI_MEMORY_TABLE));
#else  /* OSM_BUS_ADDRESS_SIZE != 64 */
      hhcb->firmwareMode = SCSI_FMODE_SWAPPING32;
      OSDmemcpy(&SCSI_ADPTSH(tscbPointer)->memoryTable,
               &SCSI_INITSH(initializationTSH)->memoryTable[0],
               sizeof(SCSI_MEMORY_TABLE));
#endif /* OSM_BUS_ADDRESS_SIZE */
#endif /* SCSI_STANDARD64_MODE */
   }
   else if (SCSI_IS_BAYONET_ID(productID))
   {
      hhcb->hardwareMode = SCSI_HMODE_AICBAYONET;
#if SCSI_STANDARD_ADVANCED_MODE
      hhcb->firmwareMode = SCSI_FMODE_STANDARD_ADVANCED;
      OSDmemcpy(&SCSI_ADPTSH(tscbPointer)->memoryTable,
               &SCSI_INITSH(initializationTSH)->memoryTable[3],
               sizeof(SCSI_MEMORY_TABLE));
#else  /* !SCSI_STANDARD_ADVANCED_MODE */
      hhcb->firmwareMode = SCSI_FMODE_SWAPPING_ADVANCED;
      OSDmemcpy(&SCSI_ADPTSH(tscbPointer)->memoryTable,
               &SCSI_INITSH(initializationTSH)->memoryTable[4],
               sizeof(SCSI_MEMORY_TABLE));
#endif /* SCSI_STANDARD_ADVANCED_MODE */
   }
   else if (SCSI_IS_TRIDENT_ID(productID))
   {
      hhcb->hardwareMode = SCSI_HMODE_AICTRIDENT;
#if SCSI_STANDARD_160M_MODE
      hhcb->firmwareMode = SCSI_FMODE_STANDARD_160M;
      OSDmemcpy(&SCSI_ADPTSH(tscbPointer)->memoryTable,
               &SCSI_INITSH(initializationTSH)->memoryTable[5],
               sizeof(SCSI_MEMORY_TABLE));
#elif SCSI_SWAPPING_160M_MODE
      hhcb->firmwareMode = SCSI_FMODE_SWAPPING_160M;
      OSDmemcpy(&SCSI_ADPTSH(tscbPointer)->memoryTable,
               &SCSI_INITSH(initializationTSH)->memoryTable[6],
               sizeof(SCSI_MEMORY_TABLE));
#elif SCSI_STANDARD_ADVANCED_MODE
      hhcb->firmwareMode = SCSI_FMODE_STANDARD_ADVANCED;
      OSDmemcpy(&SCSI_ADPTSH(tscbPointer)->memoryTable,
               &SCSI_INITSH(initializationTSH)->memoryTable[3],
               sizeof(SCSI_MEMORY_TABLE));
#else
      hhcb->firmwareMode = SCSI_FMODE_SWAPPING_ADVANCED;
      OSDmemcpy(&SCSI_ADPTSH(tscbPointer)->memoryTable,
               &SCSI_INITSH(initializationTSH)->memoryTable[4],
               sizeof(SCSI_MEMORY_TABLE));
#endif /* SCSI_STANDARD_160M_MODE */
   }
   else  /* Non-Excalibur, Non-Bayonet and Non-Trident HW */
   {
#if OSM_BUS_ADDRESS_SIZE == 64
      hhcb->firmwareMode = SCSI_FMODE_SWAPPING64;
      OSDmemcpy(&SCSI_ADPTSH(tscbPointer)->memoryTable,
               &SCSI_INITSH(initializationTSH)->memoryTable[2],
               sizeof(SCSI_MEMORY_TABLE));
#else  /* OSM_BUS_ADDRESS_SIZE != 64 */
      hhcb->firmwareMode = SCSI_FMODE_SWAPPING32;
      OSDmemcpy(&SCSI_ADPTSH(tscbPointer)->memoryTable,
               &SCSI_INITSH(initializationTSH)->memoryTable[0],
               sizeof(SCSI_MEMORY_TABLE));
#endif /* OSM_BUS_ADDRESS_SIZE */
   }

   SCSI_ADPTSH(tscbPointer)->productID = productID;
   SCSI_ADPTSH(tscbPointer)->hostAddress = hostAddress;

#if SCSI_DOMAIN_VALIDATION
   hhcb->SCSI_HF_dvInProgress = 0;
#endif /* SCSI_DOMAIN_VALIDATION */

   return((HIM_TASK_SET_HANDLE) tscbPointer);
}

/*********************************************************************
*
*  SCSISetupAdapterTSCB
*
*     Setup adapter TSCB ready to access OSM support routines
*
*  Return Value:  HIM_SUCCESS - adapter setup successful
*                 HIM_FAILURE - Unexpected value of
*                               osmFuncLength (not implemented).
*                  
*  Parameters:    adapter TSH
*                 osmRoutines
*                 osmFuncLength
*                 
*
*  Remarks:       Not until call to this routine the OSM support routines 
*                 are available for access. Those OSM prepared function
*                 pointers should be copied over to adapter TSCB for 
*                 future usage.
*
*********************************************************************/
HIM_UINT8 SCSISetupAdapterTSCB (HIM_TASK_SET_HANDLE adapterTSH,
                                HIM_OSM_FUNC_PTRS HIM_PTR osmRoutines,
                                HIM_UINT16 osmFuncLength)
{
   /* copy the osm function pointers over */
   OSDmemcpy(&SCSI_ADPTSH(adapterTSH)->osmFuncPtrs,osmRoutines,osmFuncLength);

   return((HIM_UINT8)(HIM_SUCCESS));
}

/*********************************************************************
*
*  SCSICheckMemoryNeeded
*
*     Check if more memory required by CHIM
*
*  Return Value:  size in bytes of memory required by CHIM
*                 zero indicates no more memory required 
*                  
*  Parameters:    initialization TSCB
*                 adapter TSH
*                 product id
*                 index for memory requirement 
*                 pointer to memory category
*                 minimum bytes required
*                 granularity for less memory allowed
*                 aiignment mask
*
*  Remarks:       This routine may be called either before or after
*                 host device has been identified. It should make sure
*                 maximum memory size get returned to support all
*                 possible host devices. The only information this
*                 routine can take advantage of is configuration 
*                 information setup with call to SCSISetConfiguration.
*
*********************************************************************/
HIM_UINT32 SCSICheckMemoryNeeded (HIM_TASK_SET_HANDLE initializationTSH,
                                  HIM_TASK_SET_HANDLE  adapterTSH,
                                  HIM_HOST_ID productID,    
                                  HIM_UINT16 index,    
                                  HIM_UINT8 HIM_PTR category,    
                                  HIM_UINT32 HIM_PTR minimumBytes,    
                                  HIM_UINT32 HIM_PTR granularity,    
                                  HIM_UEXACT32 HIM_PTR alignmentMask)
{
   SCSI_HHCB HIM_PTR hhcb;
   SCSI_MEMORY_TABLE HIM_PTR memoryTable;
   HIM_UINT32 size = 0;
   SCSI_UEXACT16 nexusHandles = 0;
   SCSI_UEXACT8 nodeHandles = 0;

   if (adapterTSH)
   {
      /* adapterTSH is available */
      /* we should be able to further optimized memory requirement */
      memoryTable = &SCSI_ADPTSH(adapterTSH)->memoryTable;
      if (!index)
      {
         hhcb = &SCSI_ADPTSH(adapterTSH)->hhcb;

#if SCSI_TARGET_OPERATION          
         if (hhcb->SCSI_HF_targetMode)
         {
            nexusHandles = hhcb->SCSI_HF_targetNumberNexusHandles;
            nodeHandles = hhcb->SCSI_HF_targetNumberNodeHandles;
         }   
#endif /* SCSI_TARGET_OPERATION */               

         OSDmemset(memoryTable,0,sizeof(SCSI_MEMORY_TABLE));
#if OSM_BUS_ADDRESS_SIZE == 64
         if (hhcb->firmwareMode == SCSI_FMODE_SWAPPING64)
         {
            /* swapping 64 mode */
            SCSI_GET_MEMORY_TABLE(SCSI_FMODE_SWAPPING64,hhcb->numberScbs,
                                  nexusHandles,nodeHandles,memoryTable);
         }
#else
         if (hhcb->firmwareMode == SCSI_FMODE_SWAPPING32)
         {
            /* swapping 32 mode */
            SCSI_GET_MEMORY_TABLE(SCSI_FMODE_SWAPPING32,hhcb->numberScbs,
                                  nexusHandles,nodeHandles,memoryTable);
         }
#endif
         else if (hhcb->firmwareMode == SCSI_FMODE_STANDARD_ADVANCED)
         {
            /* standard advanced mode */
            SCSI_GET_MEMORY_TABLE(SCSI_FMODE_STANDARD_ADVANCED,hhcb->numberScbs,
                                  nexusHandles,nodeHandles,memoryTable);
         }
         else if (hhcb->firmwareMode == SCSI_FMODE_SWAPPING_ADVANCED)
         {
            /* standard advanced mode */
            SCSI_GET_MEMORY_TABLE(SCSI_FMODE_SWAPPING_ADVANCED,hhcb->numberScbs,
                                  nexusHandles,nodeHandles,memoryTable);
         }
         else if (hhcb->firmwareMode == SCSI_FMODE_STANDARD_160M)
         {
            /* standard ultra 160m mode */
            SCSI_GET_MEMORY_TABLE(SCSI_FMODE_STANDARD_160M,hhcb->numberScbs,
                                  nexusHandles,nodeHandles,memoryTable);
         }
         else if (hhcb->firmwareMode == SCSI_FMODE_SWAPPING_160M)
         {
            /* swapping ultra 160m mode */
            SCSI_GET_MEMORY_TABLE(SCSI_FMODE_SWAPPING_160M,hhcb->numberScbs,
                                  nexusHandles,nodeHandles,memoryTable);
         }
         else 
         {
            /* must be standard 64 mode */
            SCSI_GET_MEMORY_TABLE(SCSI_FMODE_STANDARD64,hhcb->numberScbs,
                                  nexusHandles,nodeHandles,memoryTable);
         }
      }
   }
   else
   {
      /* memory table must be from initializationTSH */
      /* copy memory table over depending on product id */
      if (SCSI_IS_EXCALIBUR_ID(productID))
      {
#if SCSI_STANDARD64_MODE
         memoryTable = &SCSI_INITSH(initializationTSH)->memoryTable[1];
#else  /* !SCSI_STANDARD64_MODE */
#if OSM_BUS_ADDRESS_SIZE == 64
         memoryTable = &SCSI_INITSH(initializationTSH)->memoryTable[2];
#else  /* OSM_BUS_ADDRESS_SIZE != 64 */
         memoryTable = &SCSI_INITSH(initializationTSH)->memoryTable[0];
#endif /* OSM_BUS_ADDRESS_SIZE */
#endif /* SCSI_STANDARD64_MODE */
      }
      else if (SCSI_IS_BAYONET_ID(productID))
      {
#if SCSI_STANDARD_ADVANCED_MODE
         memoryTable = &SCSI_INITSH(initializationTSH)->memoryTable[3];
#else  /* !SCSI_STANDARD_ADVANCED_MODE */
         memoryTable = &SCSI_INITSH(initializationTSH)->memoryTable[4];
#endif /* SCSI_STANDARD_ADVANCED_MODE */
      }
      else if (SCSI_IS_TRIDENT_ID(productID))
      {
#if SCSI_STANDARD_160M_MODE
         memoryTable = &SCSI_INITSH(initializationTSH)->memoryTable[5];
#elif SCSI_SWAPPING_160M_MODE
         memoryTable = &SCSI_INITSH(initializationTSH)->memoryTable[6];
#elif SCSI_STANDARD_ADVANCED_MODE
         memoryTable = &SCSI_INITSH(initializationTSH)->memoryTable[3];
#else  /* SCSI_SWAPPING_ADVANCED_MODE */
         memoryTable = &SCSI_INITSH(initializationTSH)->memoryTable[4];
#endif /* SCSI_STANDARD_160M_MODE */
      }
      else  /* Non-Excalibur, Non-Bayonet and Non-Trident HW */
      {
#if OSM_BUS_ADDRESS_SIZE == 64
         memoryTable = &SCSI_INITSH(initializationTSH)->memoryTable[2];
#else  /* OSM_BUS_ADDRESS_SIZE != 64 */
         memoryTable = &SCSI_INITSH(initializationTSH)->memoryTable[0];
#endif /* OSM_BUS_ADDRESS_SIZE */
      }
   }

   if (index < SCSI_MAX_MEMORY)
   {
      *category = memoryTable->memory[index].memoryCategory;
      *granularity = (HIM_UINT32) memoryTable->memory[index].granularity;

      if (memoryTable->memory[index].memorySize)
      {
         /* return memory requirement information */
         size = (SCSI_UINT32) memoryTable->memory[index].memorySize;
         *minimumBytes = (SCSI_UINT32) memoryTable->memory[index].minimumSize;
         *alignmentMask = (HIM_UEXACT32) memoryTable->memory[index].memoryAlignment;
      }
      else
      {
         /* fake a size value so that no zero size returned */
         size = 4;
         *alignmentMask = 0;
         *minimumBytes = 0;
      }
   }
   else if (index == SCSI_INDEX_MORE_LOCKED)
   {
      *category = HIM_MC_LOCKED;
      *granularity = 0;
      *minimumBytes = size = SCSI_MORE_LOCKED;
      *alignmentMask = SCSI_MORE_LOCKED_ALIGN;
   }
   else if (index == SCSI_INDEX_MORE_UNLOCKED)
   {
      *category = HIM_MC_UNLOCKED;
      *granularity = 0;
      *minimumBytes = size = SCSI_MORE_UNLOCKED;
      *alignmentMask = SCSI_MORE_UNLOCKED_ALIGN;
   }

   return(size);
}

/*********************************************************************
*
*  SCSISetMemoryPointer
*
*     Set memory pointer for CHIM usage
*
*  Return Value:  HIM_SUCCESS, indicate memory pointer has been applied
*                 HIM_FAILURE, indicate memory pointer is not usable
*                  
*  Parameters:    adapter TSH
*                 index for memory requirement 
*                 pointer to memory category
*                 pointer to memory block
*                 size of memory block 
*
*  Remarks:       This routine should verify the memory pointer
*                 supplied by OSM is valid or not. OSM should 
*                 call this routine repetitively based on the
*                 index get used with call to SCSICheckMemoryNeeded.
*
*********************************************************************/
HIM_UINT8 SCSISetMemoryPointer (HIM_TASK_SET_HANDLE adapterTSH,
                                HIM_UINT16 index,
                                HIM_UINT8 category,
                                void HIM_PTR pMemory,
                                HIM_UINT32 size)
{
   SCSI_MEMORY_TABLE HIM_PTR memoryTable =
                              &SCSI_ADPTSH(adapterTSH)->memoryTable;
   HIM_UINT8 status = HIM_SUCCESS;

   if (index < SCSI_MAX_MEMORY)
   {
      memoryTable->memory[index].ptr.hPtr = pMemory;
      memoryTable->memory[index].memorySize = (SCSI_UEXACT16) size;
   }
   else if (index == SCSI_INDEX_MORE_LOCKED)
   {
      SCSI_ADPTSH(adapterTSH)->moreLocked = pMemory;
   }
   else if (index == SCSI_INDEX_MORE_UNLOCKED)
   {
      SCSI_ADPTSH(adapterTSH)->moreUnlocked = pMemory;
   }
   else
   {
      status = HIM_FAILURE;
   }

   return(status);   
}

/*********************************************************************
*
*  SCSIVerifyAdapter
*
*     Verify the existance of host adapter
*
*  Return Value:  HIM_SUCCESS - adapter verification successful
*                 HIM_FAILURE - adapter verification failed
*                 HIM_ADAPTER_NOT_SUPPORTED - adapter not supported
*                  
*  Parameters:    adapter TSCB
*
*  Remarks:       This routine should get all the io handles required
*                 through call to OSMMapIOHandle. With io handles 
*                 available host device can be verified.
*
*********************************************************************/
HIM_UINT8 SCSIVerifyAdapter (HIM_TASK_SET_HANDLE adapterTSH)
{
   SCSI_HHCB HIM_PTR hhcb = &SCSI_ADPTSH(adapterTSH)->hhcb;
   HIM_UINT8 status = HIM_SUCCESS;
   HIM_UINT8 i = 0;
   HIM_HOST_ID hostID;
   SCSI_UEXACT8 numberScbs;
   SCSI_UINT8 rangeIndex;
   HIM_HOST_ID subSystemSubVendorID;
   HIM_HOST_ID supportedHostID;
   HIM_HOST_ID filteredHostID;
   HIM_HOST_ID mask;
#if SCSI_PCI_COMMAND_REG_CHECK
   HIM_UEXACT32 statCmd;
#endif
   SCSI_UEXACT8 fwMode;

#if SCSI_PCI_COMMAND_REG_CHECK
   /* get status and command from hardware */
   statCmd = OSMxReadPCIConfigurationDword(SCSI_ADPTSH(adapterTSH)->osmAdapterContext,
                     SCSI_STATUS_CMD_REG);
#endif
   /* I/O mapped */
   if (!SCSI_ADPTSH(adapterTSH)->memoryMapped)
   {
#if SCSI_PCI_COMMAND_REG_CHECK
      /* Return failure if I/O Space access is not enabled. */
      if (!(statCmd & SCSI_ISPACEEN))
      {
         return((HIM_UINT8)(HIM_FAILURE)); /* should this be HIM_ADAPTER_NOT_SUPPORTED?? */
      }
#endif
      rangeIndex = 0;
   }
   else   /* Memory mapped */
   {
#if SCSI_PCI_COMMAND_REG_CHECK
      /* Return failure if Memory Space access is not enabled. */
      if (!(statCmd & SCSI_MSPACEEN))
      {
         return((HIM_UINT8)(HIM_FAILURE)); /* should this be HIM_ADAPTER_NOT_SUPPORTED?? */
      }
#endif
      rangeIndex = 1;
   }

#if SCSI_PCI_COMMAND_REG_CHECK
   /* Check if bus master enabled.  And enable it if it's not. */
   if (!(statCmd & SCSI_MASTEREN))
   {
      OSMxWritePCIConfigurationDword(SCSI_ADPTSH(adapterTSH)->osmAdapterContext,
               SCSI_STATUS_CMD_REG,(statCmd | SCSI_MASTEREN));
   }
#endif

   /* validate io handle */
   if (!OSMxMapIOHandle(SCSI_ADPTSH(adapterTSH)->osmAdapterContext,rangeIndex,0,256,0,
                        HIM_IO_LITTLE_ENDIAN,
                        &SCSI_ADPTSH(adapterTSH)->scsiHandle.ioHandle))
   {
      /* prepare scsiHandle for internal HIM access */
      SCSI_ADPTSH(adapterTSH)->scsiHandle.OSMReadUExact8 = OSMxReadUExact8;
      SCSI_ADPTSH(adapterTSH)->scsiHandle.OSMWriteUExact8 = OSMxWriteUExact8;
#if !SCSI_IO_SYNCHRONIZATION_GUARANTEED
      SCSI_ADPTSH(adapterTSH)->scsiHandle.OSMSynchronizeRange = OSMxSynchronizeRange;
#else /* SCSI_IO_SYNCHRONIZATION_GUARANTEED */
      SCSI_ADPTSH(adapterTSH)->scsiHandle.OSMSynchronizeRange = 0;
#endif

      /* get device id from hardware */
      hostID = OSMxReadPCIConfigurationDword(SCSI_ADPTSH(adapterTSH)->osmAdapterContext,
                                             SCSI_ID_REG);

      subSystemSubVendorID = 
         OSMxReadPCIConfigurationDword(SCSI_ADPTSH(adapterTSH)->osmAdapterContext,
                                       SCSI_SUBSYS_SUBVEND);

      /* If Excalibur then the device id is fixed at 0x7895 and the subsystem */
      /* ID is programable and is initially implemented as 0x789X.  If Talon, */
      /* device ID in what the HW guys call "CHIM" mode is fixed at 0x7860 and*/
      /* the subsystem id is programmable (lo nybble) to 0x786x. Similar to   */
      /* the CHIM call HIMGetNextHostDeviceType(), the function call of       */
      /* SCSIxGetNextSubSystemVendorID is called to determine which subsystem */
      /* IDs are supported.                                                   */
      if ( ((hostID == SCSI_EXCALIBUR_EXACT) && ((hhcb->hardwareRevision & 0x000000FF) < 5))
#if SCSI_2930CVAR_SUPPORT   
            || (hostID == SCSI_2930CVAR)
#endif            
            || (hostID == SCSI_TALON_CHIM_MODE_EXACT) )
      {
         while(1)
         {
            if ((supportedHostID = 
               SCSIxGetNextSubSystemSubVendorID(i,&mask)) == 0)
            {
               /* If reach the end w/o match, then fail. */
               /* Return HA is not supported. */
               return((HIM_UINT8)HIM_ADAPTER_NOT_SUPPORTED);
            }
            i++;
            if ((mask & subSystemSubVendorID) == supportedHostID)
            {
               break;      /* Device found, continue with rest of routine. */
            }
         }
      }

      /* If Bayonet type then the subsystem ID is programable according to    */
      /* the new PCI device ID spec.  This subsystem ID value can be various  */
      /* combination depending on the feature of the hardware.  We need to    */
      /* have a way to filter out a device in which we don't want to support  */
      /* this hardware.  SCSIxBayonetGetNextSubSystemSubVendorID is called to */
      /* determine which subsystem IDs are not supported.                     */
      i = 0;
      if (SCSI_IS_BAYONET_ID(hostID))
      {
         while(1)
         {
            if ((filteredHostID = 
               SCSIxBayonetGetNextSubSystemSubVendorID(i,&mask)) == 0)
            {
               break;      /* If reach the end w/o match, continue with rest of routine. */
            }
            i++;
            if ((mask & subSystemSubVendorID) == filteredHostID)
            {
               /* Device found, then return HA is not supported. */
               return((HIM_UINT8)HIM_ADAPTER_NOT_SUPPORTED);
            }
         }
      }

      /* If Trident type then the subsystem ID is programable according to    */
      /* the new PCI device ID spec.  This subsystem ID value can be various  */
      /* combination depending on the feature of the hardware.  We need to    */
      /* have a way to filter out a device in which we don't want to support  */
      /* this hardware.  SCSIxTridentGetNextSubSystemSubVendorID is called to */
      /* determine which subsystem IDs are not supported.                     */
      i = 0;
      if (SCSI_IS_TRIDENT_ID(hostID))
      {
         while(1)
         {
            if ((filteredHostID = 
               SCSIxTridentGetNextSubSystemSubVendorID(i,&mask)) == 0)
            {
               break;      /* If reach the end w/o match, continue with rest of routine. */
            }
            i++;
            if ((mask & subSystemSubVendorID) == filteredHostID)
            {
               /* Device found, then return HA is not supported. */
               return((HIM_UINT8)HIM_ADAPTER_NOT_SUPPORTED);
            }
         }
      }

      /* make sure device id matched */
      if (SCSI_ADPTSH(adapterTSH)->productID == hostID)
      {
         /* setup hhcb before it's ready to be used */
         hhcb->scsiRegister = &SCSI_ADPTSH(adapterTSH)->scsiHandle;

         /* setup build SCB function pointer depending on operating mode */
         /* we will operate in standard 64 mode or swapping 32 mode based */
         /* on the return value of SCSIxDetermineFirmwareMode */
         
         fwMode = SCSIxDetermineFirmwareMode(SCSI_ADPTSH(adapterTSH));

         if (fwMode == (SCSI_UEXACT8)SCSI_FMODE_STANDARD64)
         {
         /* Standard 64 mode */
#if SCSI_STANDARD64_MODE
            hhcb->firmwareMode = SCSI_FMODE_STANDARD64;
            SCSI_ADPTSH(adapterTSH)->OSDBuildSCB = OSDStandard64BuildSCB;
            SCSI_ENABLE_DAC(adapterTSH);
            SCSI_ADPTSH(adapterTSH)->allocBusAddressSize = 64; 
#else
            status = HIM_ADAPTER_NOT_SUPPORTED;
#endif
         }
         else if (fwMode == (SCSI_UEXACT8)SCSI_FMODE_STANDARD_ADVANCED)
         {
         /* Standard Advanced mode */
#if SCSI_STANDARD_ADVANCED_MODE
            hhcb->firmwareMode = SCSI_FMODE_STANDARD_ADVANCED;
            SCSI_ADPTSH(adapterTSH)->OSDBuildSCB = OSDStandardAdvBuildSCB;
            SCSI_ENABLE_DAC(adapterTSH);
            SCSI_ADPTSH(adapterTSH)->allocBusAddressSize = 64; 
#else
            status = HIM_ADAPTER_NOT_SUPPORTED;
#endif
         }
         else if (fwMode == (SCSI_UEXACT8)SCSI_FMODE_SWAPPING_ADVANCED)
         {
         /* Swapping Advanced mode */
#if SCSI_SWAPPING_ADVANCED_MODE
            hhcb->firmwareMode = SCSI_FMODE_SWAPPING_ADVANCED;
            SCSI_ADPTSH(adapterTSH)->OSDBuildSCB = OSDSwappingAdvBuildSCB;
#if SCSI_TARGET_OPERATION
            /* setup build SCB routine function pointer for target operation */
            SCSI_ADPTSH(adapterTSH)->OSDBuildTargetSCB = OSDSwappingAdvBuildTargetSCB;
#endif /* SCSI_TARGET_OPERATION */
            SCSI_ENABLE_DAC(adapterTSH);
            SCSI_ADPTSH(adapterTSH)->allocBusAddressSize = 64; 
#else
            status = HIM_ADAPTER_NOT_SUPPORTED;
#endif
         }
         else if (fwMode == (SCSI_UEXACT8)SCSI_FMODE_STANDARD_160M)
         {
         /* Standard Ultra 160m mode */
#if SCSI_STANDARD_160M_MODE
            hhcb->firmwareMode = SCSI_FMODE_STANDARD_160M;
            SCSI_ADPTSH(adapterTSH)->OSDBuildSCB = OSDStandard160mBuildSCB;
            SCSI_ENABLE_DAC(adapterTSH);
            SCSI_ADPTSH(adapterTSH)->allocBusAddressSize = 64; 
#else
            status = HIM_ADAPTER_NOT_SUPPORTED;
#endif
         }
         else if (fwMode == (SCSI_UEXACT8)SCSI_FMODE_SWAPPING_160M)
         {
         /* Swapping Ultra 160m mode */
#if SCSI_SWAPPING_160M_MODE
            hhcb->firmwareMode = SCSI_FMODE_SWAPPING_160M;
            SCSI_ADPTSH(adapterTSH)->OSDBuildSCB = OSDSwapping160mBuildSCB;
#if SCSI_TARGET_OPERATION
            /* setup build SCB routine function pointer for target operation */
            SCSI_ADPTSH(adapterTSH)->OSDBuildTargetSCB = OSDSwapping160mBuildTargetSCB;
#endif /* SCSI_TARGET_OPERATION */
            SCSI_ENABLE_DAC(adapterTSH);
            SCSI_ADPTSH(adapterTSH)->allocBusAddressSize = 64;
            /* setup the DMA targetTSCB */
            SCSI_xSETUP_DMA_TSCB(SCSI_ADPTSH(adapterTSH)); 
 
#else
            status = HIM_ADAPTER_NOT_SUPPORTED;
#endif
         }
         else if (fwMode == (SCSI_UEXACT8)SCSI_FMODE_NOT_ASSIGNED)
         {
            /* Invalid mode */
            status = HIM_ADAPTER_NOT_SUPPORTED;
         }
         else
         {
#if SCSI_SWAPPING32_MODE
            hhcb->firmwareMode = SCSI_FMODE_SWAPPING32;
            SCSI_ADPTSH(adapterTSH)->OSDBuildSCB = OSDSwapping32BuildSCB;
            SCSI_ADPTSH(adapterTSH)->allocBusAddressSize = 32; 
#elif SCSI_SWAPPING64_MODE
            hhcb->firmwareMode = SCSI_FMODE_SWAPPING64;
            SCSI_ADPTSH(adapterTSH)->OSDBuildSCB = OSDSwapping64BuildSCB;
            SCSI_ENABLE_DAC(adapterTSH);
            SCSI_ADPTSH(adapterTSH)->allocBusAddressSize = 32; 
#else
            status = HIM_ADAPTER_NOT_SUPPORTED;
#endif
         }

         for (i = 0; i < SCSI_MAXDEV; i++)
         {
            SCSI_ADPTSH(adapterTSH)->NumberLuns[i] = SCSI_MAXLUN;
         }

#if SCSI_NSX_REPEATER + SCSI_PAC_NSX_REPEATER + SCSI_PAC_SEND_SSU
         SCSI_ADPTSH(adapterTSH)->SCSI_AF_ssuInProgress = 0;
#endif /* SCSI_NSX_REPEATER + SCSI_PAC_NSX_REPEATER + SCSI_PAC_SEND_SSU */

#if SCSI_DOMAIN_VALIDATION
         if ((hhcb->hardwareMode == SCSI_HMODE_AICBAYONET) ||
             (hhcb->hardwareMode == SCSI_HMODE_AICTRIDENT))
         {
#if SCSI_DOMAIN_VALIDATION_ENHANCED
            SCSI_ADPTSH(adapterTSH)->domainValidationMethod = SCSI_DV_ENHANCED;
#else /* SCSI_DOMAIN_VALIDATION_ENHANCED */
            SCSI_ADPTSH(adapterTSH)->domainValidationMethod = SCSI_DV_BASIC;
#endif /* SCSI_DOMAIN_VALIDATION_ENHANCED */
         }
         else
         {
            SCSI_ADPTSH(adapterTSH)->domainValidationMethod = SCSI_DV_DISABLE;
         }
#else
         SCSI_ADPTSH(adapterTSH)->domainValidationMethod = SCSI_DV_DISABLE;
#endif

         /* get current configuration */
         hhcb->osdHandle = SCSI_ADPTSH(adapterTSH)->osmAdapterContext;
         numberScbs = hhcb->numberScbs;
         SCSI_GET_CONFIGURATION(hhcb);

         /* get bios information */
#if SCSI_BIOS_ASPI8DOS
         SCSIxGetBiosInformation(adapterTSH);
#endif /* SCSI_BIOS_ASPI8DOS */

         /* numberScbs must be less than the value OSM configured */
         if (hhcb->numberScbs > numberScbs)
         {
            hhcb->numberScbs = numberScbs;
     
            /* need to recalculate number of est scbs */
            SCSI_hTARGETGETCONFIG(hhcb); 
#if SCSI_TARGET_OPERATION
            if (hhcb->SCSI_HF_targetMode && 
                ((hhcb->SCSI_HF_targetNumberEstScbs == 0) ||
                 (hhcb->SCSI_HF_targetNumberEstScbs >= (hhcb->numberScbs - 3))))
            {
               /* Make sure we have a valid number of establish connection SCBs */
               /* We need some scbs for initiator mode and/or reestablish SCBs */
               /* Note; at some point this test may become firmware mode specific */   
               status = HIM_ADAPTER_NOT_SUPPORTED;
            }
#endif /* SCSI_TARGET_OPERATION */    
         }
      }
      else
      {
         status = HIM_ADAPTER_NOT_SUPPORTED;
      }
   }
   else
   {
      status = HIM_ADAPTER_NOT_SUPPORTED;
   }

#if SCSI_TARGET_OPERATION
   /* Initialize hhcb fields. Profiling can now occur.                   */
   /* Note that; initiatorMode, targetMode, targetNumNodeTaskSetHandles, */
   /* and targetNumNexusTaskSetHandles                                   */
   /* were initialized in SCSICreateAdapterTSCB.                         */                     
   
   hhcb->SCSI_HF_targetGroup6CDBsz = SCSI_Group6CDBDefaultSize;
   hhcb->SCSI_HF_targetGroup7CDBsz = SCSI_Group7CDBDefaultSize;
   hhcb->SCSI_HF_targetHostTargetVersion = HIM_SCSI_2;
   hhcb->SCSI_HF_targetDisconnectAllowed = 1;
   hhcb->SCSI_HF_targetTagEnable = 1;
   hhcb->SCSI_HF_targetOutOfOrderTransfers = 1;
   hhcb->SCSI_HF_targetScsi2IdentifyMsgRsv = 1; 
   hhcb->SCSI_HF_targetScsi2RejectLuntar = 0;
   hhcb->SCSI_HF_targetInitNegotiation = 0;
   hhcb->SCSI_HF_targetIgnoreWideResidMsg = 1;
   hhcb->SCSI_HF_targetEnableScsi1Selection = 0;
   hhcb->SCSI_HF_targetNexusThreshold = 0;
   hhcb->SCSI_HF_targetHiobQueueThreshold = 0;      
   hhcb->SCSI_HF_targetTaskMngtOpts = 0;
   hhcb->SCSI_HF_targetAbortTask = 1;
   hhcb->SCSI_HF_targetClearTaskSet = 1;
   hhcb->SCSI_HF_targetAdapterIDMask =
      (SCSI_UEXACT16) (1 << hhcb->hostScsiID);  

#endif /* SCSI_TARGET_OPERATION */

   return(status);
}

/*********************************************************************
*
*  SCSIxEnableDAC
*
*     Enable dual address cycle
*
*  Return Value:  none
*                  
*  Parameters:    adapter TSCB
*
*  Remarks:       This routine should be referenced only if 64 bits
*                 addressing has been enabled through OSM_DMA_SWAP_WIDTH
*                 defined as 64.
*
*********************************************************************/
#if OSM_BUS_ADDRESS_SIZE == 64
void SCSIxEnableDAC (HIM_TASK_SET_HANDLE adapterTSH)
{
   HIM_TASK_SET_HANDLE osmAdapterContext =
                           SCSI_ADPTSH(adapterTSH)->osmAdapterContext;
   SCSI_UEXACT32 uexact32Value;

   if (!((uexact32Value = OSMxReadPCIConfigurationDword(osmAdapterContext,
                                          SCSI_DEVCONFIG)) & SCSI_DACEN))
   {
      OSMxWritePCIConfigurationDword(osmAdapterContext,SCSI_DEVCONFIG,
                                          uexact32Value | SCSI_DACEN);
   }
}
#endif

/*********************************************************************
*
*  SCSI
*
*     Initialize specified host device (hardware)
*
*  Return Value:  HIM_SUCCESS, initialization successfully
*                 HIM_FAILURE, adapter initialization failure
*                  
*  Parameters:    adapter TSCB
*
*  Remarks:       This routine can call SCSIrGetConfiguration (if it's 
*                 not been called yet) and SCSIrInitialize to initialize 
*                 host adapter. Protocol automatic configuration 
*                 procedure must be executed before normal io request
*                 can be executes. SCSIQueueIOB can be invoked to
*                 execute protocol automatic configuration.
*
*********************************************************************/
HIM_UINT8 SCSIInitialize (HIM_TASK_SET_HANDLE adapterTSH)
{
   SCSI_HHCB HIM_PTR hhcb = &SCSI_ADPTSH(adapterTSH)->hhcb;
#if !SCSI_TRIDENT_PROTO
   SCSI_UEXACT8 xferOption[16];
   SCSI_UEXACT8 disconnectDisable[2];
   SCSI_UEXACT8 fast20[2];
   SCSI_UEXACT8 currentXferOption;
#if (SCSI_AICBAYONET || SCSI_AICTRIDENT)
   SCSI_UEXACT8 bayScsiOffset[16];
   

#endif
#endif /* !SCSI_TRIDENT_PROTO */
   SCSI_DEVICE HIM_PTR deviceTable;
   HIM_UINT8 return_value;
   SCSI_UEXACT8 i;
#if SCSI_ACCESS_RAM
   int scbState;
#endif /* SCSI_ACCESS_RAM */
   SCSI_UEXACT8 databuffer[sizeof(SCSI_BIOS_SCB2)];

   for (i = 0; i < sizeof(SCSI_BIOS_SCB2); i++)
      databuffer[i] = 0;

   /* apply memory table to internal HIM */
   /* numberScbs may get adjusted with following call as well */
   if (SCSI_APPLY_MEMORY_TABLE(hhcb,&SCSI_ADPTSH(adapterTSH)->memoryTable))
   {
      return((HIM_UINT8)(HIM_FAILURE));
   }

#if !SCSI_TRIDENT_PROTO
   /* update the hhcb per nvram configuration */
   SCSIxApplyNVData(SCSI_ADPTSH(adapterTSH));

   /* reconfigure the hhcb based on the previous hardware state */
   if (SCSIxBiosExist(adapterTSH) || SCSIxOptimaExist(adapterTSH))
   {
#if SCSI_OEM1_SUPPORT
      /* If OEM1 hardware is detected, don't use xfer option, etc */
      /* from BIOS. Instead use setting in NVRAM or use defaults  */
      /* if no NVRAM.                                             */
      /* Note that we still don't want to reset SCSI bus if BIOS  */
      /* is present.                                              */
      if (hhcb->OEMHardware!=SCSI_OEMHARDWARE_OEM1)
      {
#endif
         /* we must apply the existing configuration in scratch */
         /* get transfer option */
         SCSIHReadScratchRam(hhcb,xferOption,SCSI_BIOS_XFER_OPTION,16);

         /* get disconnect option */
         SCSIHReadScratchRam(hhcb,disconnectDisable,SCSI_BIOS_DISCON_OPTION,2);

         /* get the fast 20 options */
         SCSIHReadScratchRam(hhcb,fast20,SCSI_BIOS_FAST20_OPTION,2);

#if (SCSI_AICBAYONET || SCSI_AICTRIDENT)
         /* get the scsi offset for bayonet and trident bios */
         if ((hhcb->hardwareMode == SCSI_HMODE_AICBAYONET) ||
             (hhcb->hardwareMode == SCSI_HMODE_AICTRIDENT))
         {
            SCSIHReadScratchRam(hhcb,bayScsiOffset,SCSI_BIOS_BAY_OFFSET,16);

            
         }
#endif /* SCSI_AICBAYONET || SCSI_AICTRIDENT */

         /* If target mode is enabled we don't want to 
          * overwrite the default xfer option values as
          * the BIOS sets fast20 and xferOption to 0 if 
          * device is not found in a scan, which would 
          * result in the following:
          *    operating in target mode only our adapter
          *    SCSI ID is used for default negotiation rates
          *    therefore a BIOS scan would not find our ID
          *    and result in us running in async and narrow.
          *    Until the BIOS interface provides a bit indicating
          *    which device IDs where NOT found in the scan
          *    we cannot rely on the BIOS values.
          *    Same applies when operating in initiator and target
          *    mode, however we do need to make sure that the
          *    current xfer options match the BIOS values.
          */
         for (i = 0; i < 16; i++)
         {
            /* apply transfer option */
            deviceTable = &SCSI_DEVICE_TABLE(hhcb)[i];

            /* AIC-78XX BIOS */
            if (hhcb->hardwareMode == SCSI_HMODE_AIC78XX)
            {
               currentXferOption = (SCSI_UEXACT8)
                              (xferOption[i] & (SCSI_WIDEXFER + SCSI_SXFR));

               if (xferOption[i] & SCSI_SOFS)
               {
                  currentXferOption |= SCSI_SYNC_MODE;
                  deviceTable->SCSI_DF_wasSyncXfer = 1;  /* was running sync. xfer */
               }
            
               /* Only update if target mode not enabled */
               if (!hhcb->SCSI_HF_targetMode)
               {
                  deviceTable->scsiOption = currentXferOption;
               }
               else
               {
                  /* may need to set the current rate to what we
                   * received from the BIOS
                   */
               }  
            }
            /* Bayonet BIOS */
#if (SCSI_AICBAYONET || SCSI_AICTRIDENT)
            else
            {
               currentXferOption = (SCSI_UEXACT8) 
                              (xferOption[i] & SCSI_WIDEXFER);

               if (bayScsiOffset[i] & SCSI_BAYONET_OFFSET)
               {
                  currentXferOption |= SCSI_SYNC_MODE;
                  deviceTable->SCSI_DF_wasSyncXfer = 1;  /* was running sync. xfer */
               }

               /* Only update if target mode not enabled */
               if (!hhcb->SCSI_HF_targetMode)
               {
                  deviceTable->scsiOption = currentXferOption;
                  deviceTable->bayScsiRate = (SCSI_UEXACT8) xferOption[i];
                  deviceTable->bayScsiOffset = (SCSI_UEXACT8) bayScsiOffset[i];
                  deviceTable->bayDefaultRate = deviceTable->bayScsiRate;
                  deviceTable->bayDefaultOffset = deviceTable->bayScsiOffset;

                  deviceTable->xferOptionHost = (SCSI_UEXACT16)bayScsiOffset[i];
                  deviceTable->xferOptionHost <<= 8;
                  deviceTable->xferOptionHost |= (SCSI_UEXACT16)xferOption[i];
               }
               else
               {
                  /* may need to set the current rate to what we
                   * received from the BIOS
                   */
               }
            }
#endif /* SCSI_AICBAYONET || SCSI_AICTRIDENT */

            /* Was running wide xfer??? */
            deviceTable->SCSI_DF_wasWideXfer = (xferOption[i] & (SCSI_WIDEXFER));

            /* apply disconnect and fast20 option */
            if (i < 8)
            {
               deviceTable->SCSI_DF_disconnectEnable =
                                 ((disconnectDisable[0] & (1 << i)) == 0);

               if (!hhcb->SCSI_HF_targetMode)
               {    
                  deviceTable->SCSI_DF_ultraEnable =
                                    ((fast20[0] & (1 << i)) != 0);
               }
               else
               {
                  /* not sure if I need to do something here */
               } 
            }
            else
            {
               deviceTable->SCSI_DF_disconnectEnable =
                           ((disconnectDisable[1] & (1 << (i - 8))) == 0);
               if (!hhcb->SCSI_HF_targetMode)
               {    
                  deviceTable->SCSI_DF_ultraEnable =
                                 ((fast20[1] & (1 << (i - 8))) != 0);
               }
               else
               {
                  /* not sure if I need to do something here */
               } 
            }
         }

#if SCSI_OEM1_SUPPORT
      }
#endif      
      
      /* do not invoke scam protocol if BIOS exist */
      hhcb->SCSI_HF_skipScam = 1;

      /* do not reset the SCSI bus */
      hhcb->SCSI_HF_resetSCSI = 0;

      for (i = 0; i < 16; i++)
      {
         SCSI_DEVICE_TABLE(hhcb)[i].SCSI_DF_resetSCSI = 0;
      }
   }
   else
#endif /* !SCSI_TRIDENT_PROTO */
   {
      if (hhcb->SCSI_HF_targetMode) 
      {
         /* do not invoke scam protocol if target mode enabled */
         hhcb->SCSI_HF_skipScam = 1;

         if (!hhcb->SCSI_HF_initiatorMode)
         {    
            /* Do not reset the SCSI bus if operating in Target Mode ONLY */
            hhcb->SCSI_HF_resetSCSI = 0;

            for (i = 0; i < 16; i++)
            {
               SCSI_DEVICE_TABLE(hhcb)[i].SCSI_DF_resetSCSI = 0;
            }
         }
      }

      if (hhcb->SCSI_HF_initiatorMode)
      {
#if SCSI_TRIDENT_PROTO
         hhcb->SCSI_HF_resetSCSI = 1;  /* TLUU -  for trident debug */
#endif
         /* Reset the SCSI bus if Initiator mode enabled and requested */
         if (hhcb->SCSI_HF_resetSCSI == 1)
         {
            for (i = 0; i < 16; i++)
            {
               SCSI_DEVICE_TABLE(hhcb)[i].SCSI_DF_resetSCSI = 1;
            }
         }
      }
   }

   /* set flag for initialized */
   SCSI_ADPTSH(adapterTSH)->SCSI_AF_initialized = 1;

   /* fast20 further qualified with EXTVALID - external resister setting */
#if SCSI_AIC78XX
   if ((hhcb->hardwareMode == SCSI_HMODE_AIC78XX) &&
    (!(OSD_READ_PCICFG_EXACT32(hhcb,SCSI_DEVCONFIG) & SCSI_REXTVALID)))
   {
      for (i = 0; i < 16; i++)
      {
         deviceTable = &SCSI_DEVICE_TABLE(hhcb)[i];
         deviceTable->SCSI_DF_ultraEnable = 0;
      }
   }
#endif /* SCSI_AIC78XX */

#if SCSI_NEGOTIATION_PER_IOB
   for (i = 0; i < 16; i++)
   {
      /* apply transfer option */
      deviceTable = &SCSI_DEVICE_TABLE(hhcb)[i];

      /* AIC-78XX BIOS */
      if (hhcb->hardwareMode == SCSI_HMODE_AIC78XX)
      {
         if (deviceTable->scsiOption & SCSI_WIDE_XFER)
         {
            deviceTable->origOffset = SCSI_WIDE_OFFSET;
         }
         else
         {
            deviceTable->origOffset = SCSI_NARROW_OFFSET;
         }
      }
      /* Bayonet BIOS */
#if (SCSI_AICBAYONET || SCSI_AICTRIDENT)
      else
      {
         deviceTable->origOffset = SCSI_BAYONET_OFFSET;
      }
#endif /* SCSI_AICBAYONET || SCSI_AICTRIDENT */
   }
#endif /* SCSI_NEGOTIATION_PER_IOB */

   /* initialize the host adapter */
   return_value = SCSI_INITIALIZE(hhcb);

#if SCSI_PAC_NSX_REPEATER
   /* Initialize OEM1's parameters. */
   /* Must call here because it requires the accurate termination settings. */
   SCSI_OEM1_INITIALIZE(adapterTSH);
#endif /* SCSI_PAC_NSX_REPEATER */

#if SCSI_ACCESS_RAM
   scbState = SCSIHDisableExtScbRam(hhcb);

   SCSIHWriteScbRAM(hhcb,(SCSI_UEXACT8) 2,
         (SCSI_UEXACT8) 0,
         (SCSI_UEXACT8) sizeof(SCSI_BIOS_SCB2),
         (SCSI_UEXACT8 HIM_PTR) databuffer);
            
   

   if (scbState)
   {
      SCSIHEnableExtScbRam(hhcb);
   }
#endif /* SCSI_ACCESS_RAM */
   return(return_value);
}
/*********************************************************************
*
*  SCSISizeTargetTSCB
*
*     Get target TSCB size
*
*  Return Value:  target TSCB size in bytes 
*                  
*  Parameters:    adapter TSCB
*
*  Remarks:       As far as SCSI CHIM concern the associated target
*                 TSCB size can be found with sizeof(SCSI_DEVICE_CONTROL).
*                 CHIM layer does not have to know how SCSI_DEVICE_CONTROL
*                 get structured. CHIM layer may add other memory 
*                 required into size for its own convenience.
*
*********************************************************************/
HIM_UINT32 SCSISizeTargetTSCB (HIM_TASK_SET_HANDLE adapterTSH)
{
   return(sizeof(SCSI_TARGET_TSCB));
}

/*********************************************************************
*
*  SCSICheckTargetTSCBNeeded
*
*     Check target TSCB needed
*
*  Return Value:  HIM_NO_NEW_DEVICES, no more new TSCB needed
*                 HIM_NEW_DEVICE_DETECTED, new target device detected
*                  
*  Parameters:    adapter TSH
*                 index
*
*  Remarks:       The return value from this routine will tell OSM if
*                 there is new target device encountered by CHIM. New 
*                 target TSCB should be allocated for each target device
*                 found. SCSICreateTargetTSCB should be called to
*                 make new TSCB availabe for CHIM to use.
*
*********************************************************************/
HIM_UINT8 SCSICheckTargetTSCBNeeded (HIM_TASK_SET_HANDLE adapterTSH,
                                     HIM_UINT16 index)
{
#if SCSI_INITIATOR_OPERATION
   SCSI_UEXACT8 HIM_PTR lunExist = SCSI_ADPTSH(adapterTSH)->lunExist;
   SCSI_UEXACT8 HIM_PTR tshExist = SCSI_ADPTSH(adapterTSH)->tshExist;
   HIM_UINT8 i;
   HIM_UINT8 j;
   SCSI_HHCB HIM_PTR hhcb = (SCSI_HHCB HIM_PTR) &SCSI_ADPTSH(adapterTSH)->hhcb;

   if (!hhcb->SCSI_HF_initiatorMode)
   {
      return((HIM_UINT8)(HIM_NO_NEW_DEVICES));
   }

#if !SCSI_DISABLE_PROBE_SUPPORT
   if (index == HIM_PROBED_TARGET)
   {
      i = SCSI_ADPTSH(adapterTSH)->lastScsiIDProbed;
      j = SCSI_ADPTSH(adapterTSH)->lastScsiLUNProbed;
      if (SCSIxChkLunExist(lunExist,i,j) != SCSIxChkLunExist(tshExist,i,j))
      {
         /* lun exist but no task set handle available yet */
         return((HIM_UINT8)(HIM_NEW_DEVICE_DETECTED));
      }
   }
   else
#endif /* !SCSI_DISABLE_PROBE_SUPPORT */
   {
      for (i = 0; i < SCSI_MAXDEV; i++)
      {
         for (j = 0; j < SCSI_ADPTSH(adapterTSH)->NumberLuns[i]; j++)
         {
            if (SCSIxChkLunExist(lunExist,i,j) != SCSIxChkLunExist(tshExist,i,j))
            {
               /* lun exist but no task set handle available yet */
               return((HIM_UINT8)(HIM_NEW_DEVICE_DETECTED));
            }
         }
      }
   }

#endif /* SCSI_INITIATOR_OPERATION */
   return((HIM_UINT8)(HIM_NO_NEW_DEVICES));
}

/*********************************************************************
*
*  SCSICreateTargetTSCB
*
*     Check target TSCB needed
*
*  Return Value:  target task set handle
*                 0, no more new TSCB needed
*                 
*                  
*  Parameters:    target TSH
*                 index
*                 target TSCB
*                 
*
*  Remarks:       The returned target TSH will be available for OSM
*                 to send request to associated target device.
*
*********************************************************************/
HIM_TASK_SET_HANDLE SCSICreateTargetTSCB (HIM_TASK_SET_HANDLE adapterTSH,
                                          HIM_UINT16 index,
                                          void HIM_PTR targetTSCB)
{
#if SCSI_INITIATOR_OPERATION
   SCSI_HHCB HIM_PTR hhcb = (SCSI_HHCB HIM_PTR) &SCSI_ADPTSH(adapterTSH)->hhcb;
   SCSI_UEXACT8 HIM_PTR lunExist = SCSI_ADPTSH(adapterTSH)->lunExist;
   SCSI_UEXACT8 HIM_PTR tshExist = SCSI_ADPTSH(adapterTSH)->tshExist;
   HIM_UEXACT8 i;
   HIM_UEXACT8 j;

   if (!hhcb->SCSI_HF_initiatorMode)
   {
      return(SCSI_TRGTSH(0));
   }

#if !SCSI_DISABLE_PROBE_SUPPORT
   if (index == HIM_PROBED_TARGET)
   {
      i = (HIM_UEXACT8) SCSI_ADPTSH(adapterTSH)->lastScsiIDProbed;
      j = (HIM_UEXACT8) SCSI_ADPTSH(adapterTSH)->lastScsiLUNProbed;
      if (SCSIxChkLunExist(lunExist,i,j) != SCSIxChkLunExist(tshExist,i,j))
      {
         /* lun exist but no task set handle available yet */
         SCSIxSetLunExist(tshExist,i,j);

         /* assign scsi id and lun id */
         SCSI_TRGTSH(targetTSCB)->scsiID = i;
         SCSI_TRGTSH(targetTSCB)->lunID = j;
         SCSI_TRGTSH(targetTSCB)->adapterTSH = SCSI_ADPTSH(adapterTSH);

         SCSI_TRGTSH(targetTSCB)->targetAttributes.tagEnable =
            (((SCSI_DEVICE_TABLE(hhcb)[i].SCSI_DF_disconnectEnable << i) &
              SCSI_ADPTSH(adapterTSH)->tagEnable) != 0);

         /* setup target's TSCB type */
         SCSI_TRGTSH(targetTSCB)->typeTSCB = SCSI_TSCB_TYPE_TARGET;

         /* validate unit control */
         SCSI_SET_UNIT_HANDLE(hhcb,&SCSI_TRGTSH(targetTSCB)->unitControl,i,j);
            
         return(SCSI_TRGTSH(targetTSCB));
      }
   }
   else
#endif /* !SCSI_DISABLE_PROBE_SUPPORT */
   {
      for (i = 0; i < SCSI_MAXDEV; i++)
      {
         for (j = 0; j < SCSI_ADPTSH(adapterTSH)->NumberLuns[i]; j++)
         {
            if (SCSIxChkLunExist(lunExist,i,j) != SCSIxChkLunExist(tshExist,i,j))
            {
               /* lun exist but no task set handle available yet */
               SCSIxSetLunExist(tshExist,i,j);

               /* assign scsi id and lun id */
               SCSI_TRGTSH(targetTSCB)->scsiID = i;
               SCSI_TRGTSH(targetTSCB)->lunID = j;
               SCSI_TRGTSH(targetTSCB)->adapterTSH = SCSI_ADPTSH(adapterTSH);

               SCSI_TRGTSH(targetTSCB)->targetAttributes.tagEnable =
                  (((SCSI_DEVICE_TABLE(hhcb)[i].SCSI_DF_disconnectEnable << i) &
                    SCSI_ADPTSH(adapterTSH)->tagEnable) != 0);

               /* setup target's TSCB type */
               SCSI_TRGTSH(targetTSCB)->typeTSCB = SCSI_TSCB_TYPE_TARGET;

               /* validate unit control */
               SCSI_SET_UNIT_HANDLE(hhcb,&SCSI_TRGTSH(targetTSCB)->unitControl,i,j);

               return(SCSI_TRGTSH(targetTSCB));
            }
         }
      }
   }

#endif /* SCSI_INITIATOR_OPERATION */

   return(SCSI_TRGTSH(0));
}

/*$Header:   Y:/source/chimscsi/src/XLMTASK.CV_   1.56.2.7   30 Mar 1998 09:59:16   MCGE1234  $*/
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
*  Module Name:   XLMTASK.C
*
*  Description:
*                 Codes to implement run time Common HIM interface 
*
*  Owners:        ECX IC Firmware Team
*    
*  Notes:         NONE
*
*  Entry Point(s):
*                 SCSIDisableIRQ
*                 SCSIEnableIRQ
*                 SCSIPollIRQ
*                 SCSIFrontEndISR
*                 SCSIBackEndISR
*                 SCSIQueueIOB
*                 SCSIPowerEvent
*                 SCSIValidateTargetTSH
*                 SCSIClearTargetTSH
*                 SCSISaveState
*                 SCSIRestoreState
*                 SCSIProfileAdapter
*                 SCSIReportAdjustableAdapterProfile
*                 SCSIAdjustAdapterProfile
*                 SCSIGetNVSize
*                 SCSIGetNVOSMSegment
*                 SCSIPutNVData
*                 SCSIGetNVData
*                 SCSIClearNexusTSH 
*                 SCSIProfileNexus 
*                 SCSIProfileNode
*                 SCSIReportAdjustableNodeProfile
*                 SCSIAdjustNodeProfile
*                 SCSISetOSMNodeContext
*
***************************************************************************/

#include "scsi.h"
#include "xlm.h"

/*********************************************************************
*
*  SCSIDisableIRQ
*
*     Disable hardware interrupt
*
*  Return Value:  none
*                  
*  Parameters:    adapter TSH
*
*  Remarks:
*
*********************************************************************/
void SCSIDisableIRQ (HIM_TASK_SET_HANDLE adapterTSH)
{
   SCSI_DISABLE_IRQ(&SCSI_ADPTSH(adapterTSH)->hhcb);
}

/*********************************************************************
*
*  SCSIEnableIRQ
*
*     Enable hardware interrupt
*
*  Return Value:  none
*                  
*  Parameters:    adapter TSH
*
*  Remarks:
*
*********************************************************************/
void SCSIEnableIRQ (HIM_TASK_SET_HANDLE adapterTSH)
{
   SCSI_ENABLE_IRQ(&SCSI_ADPTSH(adapterTSH)->hhcb);
}

/*********************************************************************
*
*  SCSIPollIRQ
*
*     Poll hardware interrupt
*
*  Return Value:  HIM_NOTHING_PENDING, interrupt not asserted
*                 HIM_INTERRUPT_PENDING, interrupt asserted
*                  
*  Parameters:    adapter TSH
*
*  Remarks:
*
*********************************************************************/
HIM_UINT8 SCSIPollIRQ (HIM_TASK_SET_HANDLE adapterTSH)
{
   if(SCSI_POLL_IRQ(&SCSI_ADPTSH(adapterTSH)->hhcb))
   {
      return((HIM_UINT8)(HIM_INTERRUPT_PENDING));
   }
   else
   {
      return((HIM_UINT8)(HIM_NOTHING_PENDING));
   }
}

/*********************************************************************
*
*  SCSIFrontEndISR
*
*     Execute front end interrupt service rutine
*
*  Return Value:  HIM_NOTHING_PENDING, no interrupt pending
*                 HIM_INTERRUPT_PENDING, interrupt pending
*                 HIM_LONG_INTERRUPT_PENDING, lengthy interupt pending
*                  
*  Parameters:    adapter TSH
*
*  Remarks:       This rotuine should be very fast in execution.
*                 The lengthy processing of interrupt should be
*                 handled in SCSIBackEndISR.
*
*********************************************************************/
#if !SCSI_STREAMLINE_QIOPATH
HIM_UINT8 SCSIFrontEndISR (HIM_TASK_SET_HANDLE adapterTSH)
{
   if(SCSI_FRONT_END_ISR(&SCSI_ADPTSH(adapterTSH)->hhcb))
   {
      return((HIM_UINT8)(HIM_INTERRUPT_PENDING));
   }
   else
   {
      return((HIM_UINT8)(HIM_NOTHING_PENDING));
   }
}
#endif /* !SCSI_STREAMLINE_QIOPATH */

/*********************************************************************
*
*  SCSIBackEndISR
*
*     Execute back end interrupt service rutine
*
*  Return Value:  none
*                  
*  Parameters:    adapter TSH
*
*  Remarks:       It's OSM's decision to execute this routine at
*                 interrupt context or not.
*
*********************************************************************/
#if !SCSI_STREAMLINE_QIOPATH
void SCSIBackEndISR (HIM_TASK_SET_HANDLE adapterTSH)
{
   SCSI_BACK_END_ISR(&SCSI_ADPTSH(adapterTSH)->hhcb);
}
#endif /* !SCSI_STREAMLINE_QIOPATH */

/*********************************************************************
*
*  SCSIQueueIOB
*
*     Queue the IOB for execution
*
*  Return Value:  void
*                  
*  Parameters:    adapter TSH
*
*  Remarks:       It's OSM's decision to execute this routine at
*                 interrupt context or not.
*
*********************************************************************/
#if !SCSI_STREAMLINE_QIOPATH
void SCSIQueueIOB (HIM_IOB HIM_PTR iob)
{
   HIM_TASK_SET_HANDLE taskSetHandle = iob->taskSetHandle;
   SCSI_HIOB HIM_PTR hiob = &SCSI_IOBRSV(iob)->hiob;
   HIM_TS_SCSI HIM_PTR transportSpecific = (HIM_TS_SCSI HIM_PTR)iob->transportSpecific;
#if SCSI_TARGET_OPERATION 
   SCSI_UEXACT8 HIM_PTR scsiStatus;
   SCSI_HHCB HIM_PTR hhcb; 
#endif /* SCSI_TARGET_OPERATION */

   /* setup pointer to iob for translation from hiob to iob */
   /* it will be referenced when building SCB and posting to OSM */
   SCSI_IOBRSV(iob)->iob = iob;
                               
   if (iob->function == HIM_INITIATE_TASK)
   {
      /* translate iob into hiob */
      hiob->unitHandle.targetUnit = &SCSI_TRGTSH(taskSetHandle)->unitControl;

      hiob->cmd = SCSI_CMD_INITIATE_TASK;
      hiob->SCSI_IF_noUnderrun = 0;

      if (transportSpecific)
      {
         hiob->SCSI_IF_tagEnable = ((SCSI_TRGTSH(taskSetHandle)->targetAttributes.tagEnable) &&
                                    (transportSpecific->forceUntagged == HIM_FALSE) &&
                                    /* HQ
                                    (transportSpecific->disallowDisconnect == HIM_FALSE) &&
                                    */
                                    (iob->taskAttribute != HIM_TASK_RECOVERY));
         hiob->SCSI_IF_disallowDisconnect =
               (transportSpecific->disallowDisconnect == HIM_TRUE) ? 1 : 0;
#if SCSI_NEGOTIATION_PER_IOB
         hiob->SCSI_IF_forceSync =
               (transportSpecific->forceSync == HIM_TRUE) ? 1 : 0;
         hiob->SCSI_IF_forceAsync =
               (transportSpecific->forceAsync == HIM_TRUE) ? 1 : 0;
         hiob->SCSI_IF_forceWide =
               (transportSpecific->forceWide == HIM_TRUE) ? 1 : 0;
         hiob->SCSI_IF_forceNarrow =
               (transportSpecific->forceNarrow == HIM_TRUE) ? 1 : 0;
         hiob->SCSI_IF_forceReqSenseNego =
               (transportSpecific->forceReqSenseNego == HIM_TRUE) ? 1 : 0;
#endif /* SCSI_NEGOTIATION_PER_IOB */
#if SCSI_PARITY_PER_IOB
         hiob->SCSI_IF_parityEnable = 
               (transportSpecific->parityEnable == HIM_TRUE) ? 1 : 0;
#endif
#if SCSI_PAC_NSX_REPEATER || SCSI_NSX_REPEATER
         hiob->SCSI_IF_nsxCommunication =
               (transportSpecific->nsxCommunication == HIM_TRUE) ? 1 : 0;
#endif /* SCSI_PAC_NSX_REPEATER || SCSI_NSX_REPEATER */
#if SCSI_DOMAIN_VALIDATION
         hiob->SCSI_IF_dvIOB = (transportSpecific->dvIOB == HIM_TRUE) ? 1 : 0;
#endif /* SCSI_DOMAIN_VALIDATION */
      }
      else
      {
         hiob->SCSI_IF_tagEnable = ((SCSI_TRGTSH(taskSetHandle)->targetAttributes.tagEnable) &&
                                    (iob->taskAttribute != HIM_TASK_RECOVERY));
         hiob->SCSI_IF_disallowDisconnect = 0;
#if SCSI_NEGOTIATION_PER_IOB
         hiob->SCSI_IF_forceSync = 0;
         hiob->SCSI_IF_forceAsync = 0;
         hiob->SCSI_IF_forceWide = 0;
         hiob->SCSI_IF_forceNarrow = 0;
         hiob->SCSI_IF_forceReqSenseNego = 0;
#endif
#if SCSI_PARITY_PER_IOB
         hiob->SCSI_IF_parityEnable = 0;
#endif
#if SCSI_PAC_NSX_REPEATER || SCSI_NSX_REPEATER
         hiob->SCSI_IF_nsxCommunication = 0;
#endif /* SCSI_PAC_NSX_REPEATER || SCSI_NSX_REPEATER */
#if SCSI_DOMAIN_VALIDATION
         hiob->SCSI_IF_dvIOB = 0;
#endif /* SCSI_DOMAIN_VALIDATION */
      }

      hiob->SCSI_IF_autoSense = iob->flagsIob.autoSense;
      hiob->SCSI_IF_freezeOnError = iob->flagsIob.freezeOnError;
      hiob->priority = iob->priority;
      hiob->snsBuffer = iob->errorData;

      if (iob->errorDataLength <= 255)
      {
         hiob->snsLength = (SCSI_UEXACT8) iob->errorDataLength;
      }
      else
      {
         hiob->snsLength = (SCSI_UEXACT8) 255;
      }

      /* send request to internal HIM */
      SCSI_QUEUE_HIOB(hiob);
   }
   else
   {
      /* process other special iob */
      switch(iob->function)
      {
         case HIM_TERMINATE_TASK:
         case HIM_ABORT_TASK:
            hiob->cmd = SCSI_CMD_ABORT_TASK;
            hiob->unitHandle.relatedHiob = &SCSI_IOBRSV(iob->relatedIob)->hiob;
            break;

         case HIM_ABORT_TASK_SET:
            hiob->cmd = SCSI_CMD_ABORT_TASK_SET;
            hiob->unitHandle.targetUnit = &SCSI_TRGTSH(taskSetHandle)->unitControl;
            break;

         case HIM_RESET_BUS_OR_TARGET:
            if (SCSI_TRGTSH(taskSetHandle)->typeTSCB == SCSI_TSCB_TYPE_TARGET)
            {
               /* reset target */
               hiob->cmd =  SCSI_CMD_RESET_TARGET;
               hiob->unitHandle.targetUnit = &SCSI_TRGTSH(taskSetHandle)->unitControl;
            }
            else if (SCSI_TRGTSH(taskSetHandle)->typeTSCB == SCSI_TSCB_TYPE_ADAPTER)
            {
               /* reset scsi bus */
               hiob->cmd = SCSI_CMD_RESET_BUS;
               hiob->unitHandle.initiatorUnit = &SCSI_ADPTSH(taskSetHandle)->hhcb;
            }
            else
            {
               /* illegal request */
               iob->taskStatus = HIM_IOB_TSH_INVALID;
               iob->postRoutine(iob);
               return ;
            }
            break;

         case HIM_RESET_HARDWARE:
            hiob->cmd = SCSI_CMD_RESET_HARDWARE;
            hiob->unitHandle.initiatorUnit = &SCSI_ADPTSH(taskSetHandle)->hhcb;
            break;

         case HIM_PROTOCOL_AUTO_CONFIG:
            hiob->cmd = SCSI_CMD_PROTO_AUTO_CFG;
            hiob->unitHandle.initiatorUnit = &SCSI_ADPTSH(taskSetHandle)->hhcb;
            SCSI_xSAVE_CURRENT_WWIDS(SCSI_ADPTSH(taskSetHandle));
            break;

#if !SCSI_DISABLE_PROBE_SUPPORT
         case HIM_PROBE:
            if ((transportSpecific) &&
#if !SCSI_NULL_SELECTION
                (transportSpecific->scsiID != 
                 SCSI_ADPTSH(taskSetHandle)->hhcb.hostScsiID) &&
#endif /* !SCSI_NULL_SELECTION */
                !(transportSpecific->LUN[0]))
            {
               SCSI_ADPTSH(iob->taskSetHandle)->iobProtocolAutoConfig = iob;
               SCSIxSetupLunProbe(SCSI_ADPTSH(iob->taskSetHandle),
                  transportSpecific->scsiID,
                  transportSpecific->LUN[1]);
               SCSIxQueueBusScan(SCSI_ADPTSH(iob->taskSetHandle));
               SCSI_ADPTSH(iob->taskSetHandle)->lastScsiIDProbed = 
                  transportSpecific->scsiID;
               SCSI_ADPTSH(iob->taskSetHandle)->lastScsiLUNProbed = 
                  transportSpecific->LUN[1];
   
            }
            else
            {
               /* invalid probe */
               iob->taskStatus = HIM_IOB_INVALID;
               iob->postRoutine(iob);
            }
            return;
#endif /* !SCSI_DISABLE_PROBE_SUPPORT */

         case HIM_SUSPEND:
         case HIM_QUIESCE:
            if (SCSI_CHECK_HOST_IDLE(&SCSI_ADPTSH(taskSetHandle)->hhcb))
            {
               /* host device is idle and nothing has to be done */
               iob->taskStatus = HIM_IOB_GOOD;
            }
            else
            {
               /* indicate host adapter is not idle */
               iob->taskStatus = HIM_IOB_ADAPTER_NOT_IDLE;
            }

#if SCSI_TARGET_OPERATION
            hhcb = &SCSI_ADPTSH(taskSetHandle)->hhcb;
            
         if (hhcb->SCSI_HF_targetMode)
            
         {
               /* Disable selection-in */
               SCSI_DISABLE_SELECTION_IN(hhcb);
               
               /* Need to check if the SCSI bus is held or */
               /* any pending selection-in interrupts      */
               if ((hhcb->SCSI_HF_targetScsiBusHeld) ||
                   (SCSI_CHECK_SELECTION_IN_INTERRUPT_PENDING(hhcb))) 
               {
                  /* indicate that adapter is not idle */ 
                  iob->taskStatus = HIM_IOB_ADAPTER_NOT_IDLE;
               }
            }
#endif /* SCSI_TARGET_OPERATION */
            iob->postRoutine(iob);
            return ;

         case HIM_RESUME:
#if SCSI_TARGET_OPERATION
            /* Target Mode enabled then this function
             * enables selection-in
             */
            hhcb = &SCSI_ADPTSH(taskSetHandle)->hhcb;
            SCSI_hTARGETMODEENABLE(hhcb);
#endif /* SCSI_TARGET_OPERATION */            
            /* just return good status and nothing has to be done */
            iob->taskStatus = HIM_IOB_GOOD;
            iob->postRoutine(iob);
            return ;

         case HIM_CLEAR_XCA:
            /* not supported for this implementation */
            iob->taskStatus = HIM_IOB_UNSUPPORTED;
            iob->postRoutine(iob);
            return ;

         case HIM_UNFREEZE_QUEUE:
            /* unfreeze device queue */
            hiob->cmd = SCSI_CMD_UNFREEZE_QUEUE;
            hiob->unitHandle.targetUnit = &SCSI_TRGTSH(taskSetHandle)->unitControl;
            break;

#if SCSI_DMA_SUPPORT
         case HIM_INITIATE_DMA_TASK:
            /* use specially reserved dma TSCB */ 
            hiob->unitHandle.targetUnit =
               &(SCSI_TRGTSH(&SCSI_ADPTSH(taskSetHandle)->dmaTargetTSCB)->unitControl);
            hiob->cmd = SCSI_CMD_INITIATE_DMA_TASK;
            /* clear all hiob flags */
            hiob->SCSI_IF_flags = 0;
            break;    
#endif /* SCSI_DMA_SUPPORT */

#if SCSI_TARGET_OPERATION
         case HIM_ESTABLISH_CONNECTION:
#if SCSI_RESOURCE_MANAGEMENT
            hiob->cmd = SCSI_CMD_ESTABLISH_CONNECTION;
            hiob->unitHandle.initiatorUnit = &SCSI_ADPTSH(taskSetHandle)->hhcb;
            /* snsBuffer contains the address of where the 
               received command or task management request 
               is to be stored. */
            hiob->snsBuffer = iob->targetCommand;
            hiob->snsBufferSize = (SCSI_UEXACT16) iob->targetCommandBufferSize;
            break;
#else
            iob->taskStatus = HIM_IOB_INVALID;
            iob->postRoutine(iob);
            return;
  
#endif /* SCSI_RESOURCE_MANAGEMENT */
         case HIM_REESTABLISH_AND_COMPLETE:
            if ( (iob->targetCommandLength != 1) || 
                 (iob->flagsIob.outOfOrderTransfer) ) 
            {
               /* SCSI protocol expects one byte of status */
               /* outOfOrderTransfer not supported */ 
               iob->taskStatus = HIM_IOB_INVALID;
               iob->postRoutine(iob);
               return; 
            }
            hiob->cmd = SCSI_CMD_REESTABLISH_AND_COMPLETE;
            SCSI_NEXUS_UNIT(hiob) = SCSI_NEXUSTSH(taskSetHandle);
            hiob->ioLength = iob->ioLength;
            hiob->snsBufferSize = (SCSI_UEXACT16) iob->targetCommandBufferSize;
            scsiStatus = (SCSI_UEXACT8 HIM_PTR) iob->targetCommand;
            hiob->initiatorStatus = scsiStatus[0];
            if (iob->flagsIob.targetRequestType == HIM_REQUEST_TYPE_TMF)
            {
               hiob->SCSI_IF_taskManagementResponse = 1;
            }
            else
            {
               hiob->SCSI_IF_taskManagementResponse = 0;
            }  
            break;
             
         case HIM_REESTABLISH_INTERMEDIATE:
            if (iob->flagsIob.outOfOrderTransfer) 
            {
               /* Not supported */
               iob->taskStatus = HIM_IOB_INVALID;
               iob->postRoutine(iob);
               return; 
            }
            hiob->cmd = SCSI_CMD_REESTABLISH_INTERMEDIATE;
            SCSI_NEXUS_UNIT(hiob) = SCSI_NEXUSTSH(taskSetHandle);
            hiob->ioLength = iob->ioLength;
            /* hiob->snsBuffer = iob->errorData;  */
            hiob->snsBufferSize = (SCSI_UEXACT16)iob->targetCommandBufferSize;
            if (transportSpecific)
            {
               hiob->SCSI_IF_disallowDisconnect = transportSpecific->disallowDisconnect;
            }
            else
            {
               hiob->SCSI_IF_disallowDisconnect = 0; 
            }
            break;

         case HIM_ABORT_NEXUS:
            hiob->cmd = SCSI_CMD_ABORT_NEXUS;
            SCSI_NEXUS_UNIT(hiob) = SCSI_NEXUSTSH(taskSetHandle);
            break;

#if SCSI_MULTIPLEID_SUPPORT
         case HIM_ENABLE_ID:
            if (!SCSIxEnableID(iob)) 
            {
               iob->taskStatus = HIM_IOB_INVALID;
               iob->postRoutine(iob);
               return;
            }
            else
            {
               hiob->cmd = SCSI_CMD_ENABLE_ID;
               hiob->unitHandle.initiatorUnit = &SCSI_ADPTSH(taskSetHandle)->hhcb;
               hiob->snsBufferSize =
                  ((HIM_TS_ID_SCSI HIM_PTR) iob->transportSpecific)->scsiIDMask;
            }
            break;

         case HIM_DISABLE_ID:
            if (!SCSIxDisableID(iob))
            {
               iob->taskStatus = HIM_IOB_INVALID;
               iob->postRoutine(iob);
               return;
            }
            else
            {
               hiob->cmd = SCSI_CMD_DISABLE_ID;
               hiob->unitHandle.initiatorUnit = &SCSI_ADPTSH(taskSetHandle)->hhcb;
               hiob->snsBufferSize =
                  ((HIM_TS_ID_SCSI HIM_PTR) iob->transportSpecific)->scsiIDMask;
            }
            break;
          
#endif /* SCSI_MULTIPLEID_SUPPORT */
#endif /* SCSI_TARGET_OPERATION */

         default:
            /* indicate iob invalid and post back immediately */
            iob->taskStatus = HIM_IOB_INVALID;
            iob->postRoutine(iob);
            return ;
      }

      /* send request to internal HIM */
      SCSI_QUEUE_SPECIAL_HIOB(hiob);
   }
}
#endif /* !SCSI_STREAMLINE_QIOPATH */

/*********************************************************************
*
*  SCSIPowerEvent
*
*     Queue the IOB for execution
*
*  Return Value:  0, no outstanding tasks. hibernation successful
*                 others, outstanding tasks. hibernation uncessful
*                  
*  Parameters:    adapter TSH
*                 severity
*
*  Remarks:       The implementation of this routine wil just
*                 call SCSIHPowerManagement routine to handle the
*                 actual power management function.
*                 
*
*********************************************************************/
HIM_UINT8 SCSIPowerEvent (HIM_TASK_SET_HANDLE adapterTSH,
                          HIM_UINT8 severity)
{
   return((HIM_UINT8)SCSI_POWER_MANAGEMENT(&SCSI_ADPTSH(adapterTSH)->hhcb,
            (SCSI_UINT) severity));
}

/*********************************************************************
*
*  SCSIValidateTargetTSH
*
*     Validate if the specified target TSH is still valid. 
*
*  Return Value:  HIM_TARGET_VALID, target TSH is still valid
*                 HIM_TARGET_CHANGED, target TSH is still valid 
*                                     but the scsi id changed
*                 HIM_TARGET_INVALID, target TSH is not valid any more
*                  
*  Parameters:    target TSH
*
*  Remarks:       This routine will have to remember what the device 
*                 table was and compare it with the device table looks
*                 like after the protocol automatic configuration.
*
*********************************************************************/
HIM_UINT8 SCSIValidateTargetTSH (HIM_TASK_SET_HANDLE targetTSH)
{
#if SCSI_INITIATOR_OPERATION
   return(SCSI_xVALIDATE_LUN((SCSI_TARGET_TSCB HIM_PTR) targetTSH));
#else
   return((HIM_UINT8)(HIM_TARGET_INVALID));
#endif 
}

/*********************************************************************
*
*  SCSIxScamValidateLun
*
*     Validate if the specified target TSH is still valid with
*     scam enabled. 
*
*  Return Value:  HIM_TARGET_VALID, target TSH is still valid
*                 HIM_TARGET_CHANGED, target TSH is still valid 
*                                     but the scsi id changed
*                 HIM_TARGET_INVALID, target TSH is not valid any more
*                  
*  Parameters:    target TSH
*
*  Remarks:
*
*********************************************************************/
#if SCSI_SCAM_ENABLE
HIM_UINT8 SCSIxScamValidateLun (SCSI_TARGET_TSCB HIM_PTR targetTSH)
{
   SCSI_ADAPTER_TSCB HIM_PTR adapterTSH =
               ((SCSI_TARGET_TSCB HIM_PTR)targetTSH)->adapterTSH;
   SCSI_HHCB HIM_PTR hhcb = &((SCSI_ADAPTER_TSCB HIM_PTR) adapterTSH)->hhcb;
   int i;

   /* check previous devices information */
   if (adapterTSH->prevScamDevice & (1 << targetTSH->scsiID))
   {
      /* was a scam devices. check current device table */
      for (i = 0; i < (int) hhcb->maxDevices; i++)
      {
         if (SCSI_DEVICE_TABLE(hhcb)[i].SCSI_DF_scamDevice &&
            !OSDmemcmp(adapterTSH->prevScamWWID[targetTSH->scsiID],
            SCSI_DEVICE_TABLE(hhcb)[i].scamWWID,SCSI_SIZEWWID))
         {
            if (i == (int) targetTSH->scsiID)
            {
               /* nothing changed */
               return((HIM_UINT8)(HIM_TARGET_VALID));
            }
            else
            {
               /* scsi id changed */
               targetTSH->scsiID = i;
               SCSIxSetLunExist(adapterTSH->tshExist,(SCSI_UINT8) i,targetTSH->lunID);
               return((HIM_UINT8)(HIM_TARGET_CHANGED));
            }
         }
      }

      /* device has been removed */
      return((HIM_UINT8)(HIM_TARGET_INVALID));
   }
   else
   {
      /* was a non scam device */
      if (!SCSI_DEVICE_TABLE(hhcb)[targetTSH->scsiID].SCSI_DF_scamDevice)
      {
         /* it's still not a scam device */
         if (SCSIxChkLunExist(adapterTSH->lunExist,targetTSH->scsiID,0))
         {
            /* assume it's still valid as long as device exist */ 
            return((HIM_UINT8)(HIM_TARGET_VALID));
         }
      }

      /* device must have been removed */
      return((HIM_UINT8)(HIM_TARGET_INVALID));
   }
}     
#endif

/*********************************************************************
*
*  SCSIxNonScamValidateLun
*
*     Validate if the specified target TSH is still valid with
*     scam disabled. 
*
*  Return Value:  HIM_TARGET_VALID, target TSH is still valid
*                 HIM_TARGET_CHANGED, target TSH is still valid 
*                                     but the scsi id changed
*                 HIM_TARGET_INVALID, target TSH is not valid any more
*                  
*  Parameters:    target TSH
*
*  Remarks:
*
*********************************************************************/
#if !SCSI_SCAM_ENABLE
HIM_UINT8 SCSIxNonScamValidateLun (SCSI_TARGET_TSCB HIM_PTR targetTSH)
{
   SCSI_ADAPTER_TSCB HIM_PTR adapterTSH =
               ((SCSI_TARGET_TSCB HIM_PTR)targetTSH)->adapterTSH;
   
   /* for non scam device we just assume it's valid if */
   /* device exist */
   if (SCSIxChkLunExist(adapterTSH->lunExist,targetTSH->scsiID,0))
   {
      return((HIM_UINT8)(HIM_TARGET_VALID));
   }
   else
   {
      return((HIM_UINT8)(HIM_TARGET_INVALID));
   }
}
#endif

/*********************************************************************
*
*  SCSIClearTargetTSH
*
*     Invalidate target TSH
*
*  Return Value:  HIM_SUCCESS, target TSH is cleared and the 
*                              associated TSCB memory can be freed
*                 HIM_FAILURE, target TSH can not be invalidated
*                              (e.g. initiator mode not enabled) 
*                 HIM_TARGET_NOT_IDLE, target TSH is not idle and 
*                                      can not be invalidated
*                  
*  Parameters:    target TSH
*
*  Remarks:       OSM can call this routine to free unused target TSH
*                 for other usage at any time after the call to
*                 execute protocol automatic configuration.
*
*********************************************************************/
HIM_UINT8 SCSIClearTargetTSH (HIM_TASK_SET_HANDLE targetTSH)
{
#if SCSI_INITIATOR_OPERATION
    
   SCSI_ADAPTER_TSCB HIM_PTR adapterTSH =
               ((SCSI_TARGET_TSCB HIM_PTR)targetTSH)->adapterTSH;
   SCSI_HHCB HIM_PTR hhcb = &((SCSI_ADAPTER_TSCB HIM_PTR) adapterTSH)->hhcb;
   
   if (!hhcb->SCSI_HF_initiatorMode) 
   {
      return((HIM_UINT8)(HIM_FAILURE));
   }

   /* check if associated target is busy */
   if (!SCSI_CHECK_DEVICE_IDLE(&adapterTSH->hhcb,
                               (SCSI_UEXACT8)SCSI_TRGTSH(targetTSH)->scsiID))
   {
      return((HIM_UINT8)(HIM_TARGET_NOT_IDLE));
   }

   /* invalidate task set handle (and associated lun) */
   SCSIxClearLunExist(adapterTSH->lunExist,SCSI_TRGTSH(targetTSH)->scsiID,
      SCSI_TRGTSH(targetTSH)->lunID);
   SCSIxClearLunExist(adapterTSH->tshExist,SCSI_TRGTSH(targetTSH)->scsiID,
      SCSI_TRGTSH(targetTSH)->lunID);

   /* free the unit handle */
   SCSI_FREE_UNIT_HANDLE(*((SCSI_UNIT_HANDLE HIM_PTR)
                           &SCSI_TRGTSH(targetTSH)->unitControl));

   /* indicate target cleared for now */
   return((HIM_UINT8)(HIM_SUCCESS));
#else
   return((HIM_UINT8)(HIM_FAILURE));
#endif
}

/*********************************************************************
*
*  SCSISaveState
*
*     Save current hardware state
*
*  Return Value:  none
*                  
*  Parameters:    target TSH
*                 pointer to state memory
*
*  Remarks:       OSM must make enough memory get allocated for
*                 state saving. The size of state memory requirement
*                 can be found through adapter profile.
*
*********************************************************************/
void SCSISaveState (HIM_TASK_SET_HANDLE adapterTSH,
                    void HIM_PTR pState)
{
#if SCSI_SAVE_RESTORE_STATE
   if (SCSIxBiosExist(adapterTSH) || SCSIxOptimaExist(adapterTSH))
   {
      ((SCSI_STATE HIM_PTR) pState)->SCSI_SF_optMode = 1;
   }
   else
   {
      ((SCSI_STATE HIM_PTR) pState)->SCSI_SF_optMode = 0;

   }
   SCSI_SAVE_STATE(&SCSI_ADPTSH(adapterTSH)->hhcb,(SCSI_STATE HIM_PTR) pState);
#endif /* SCSI_SAVE_RESTORE_STATE */
}

/*********************************************************************
*
*  SCSIRestoreState
*
*     Restore state from the state memory
*
*  Return Value:  none
*                  
*  Parameters:    target TSH
*                 pointer to state memory
*
*  Remarks:       Typically the state memory has information saved
*                 with previous call to SCSISave.
*
*********************************************************************/
void SCSIRestoreState (HIM_TASK_SET_HANDLE adapterTSH,
                       void HIM_PTR pState)
{
#if SCSI_SAVE_RESTORE_STATE
   SCSI_RESTORE_STATE(&SCSI_ADPTSH(adapterTSH)->hhcb,(SCSI_STATE HIM_PTR) pState);
#endif /* SCSI_SAVE_RESTORE_STATE */
}

/*********************************************************************
*
*  SCSIProfileAdapter
*
*     Get adapter profile information
*
*  Return Value:  HIM_SUCCESS, normal return
*                 HIM_FAILURE, operation failure
*                  
*  Parameters:    adapter TSH
*                 profile memory
*
*  Remarks:       Some of the profile information are transport
*                 specific and OSM should either avoid or isolate
*                 accessing profile information if possible.
*                 Pretty much CHIM will have to maintain the
*                 profile information itself.
*
*********************************************************************/
HIM_UINT8 SCSIProfileAdapter (HIM_TASK_SET_HANDLE adapterTSH,
                              HIM_ADAPTER_PROFILE HIM_PTR profile)         
{
#if SCSI_PROFILE_INFORMATION
   SCSI_HHCB HIM_PTR hhcb = (SCSI_HHCB HIM_PTR) &SCSI_ADPTSH(adapterTSH)->hhcb;
   SCSI_UINT16 i;
   SCSI_HW_INFORMATION hwInformation;
#if SCSI_TARGET_OPERATION 
   SCSI_DEVICE SCSI_DPTR deviceTable; 
#endif /* SCSI_TARGET_OPERATION */

   SCSI_GET_HW_INFORMATION(&hwInformation, hhcb);
   
   /* copy the default profile for now */
   OSDmemcpy(profile,&SCSIDefaultAdapterProfile,sizeof(HIM_ADAPTER_PROFILE));

   /* update fields which are adapter specific */
   /* must calculate the WWID for the HA because there is no guarantee that   */
   /* protocol auto config has been called yet which would leave the prev     */
   /* calculated WWID for the HA invalid.                                     */
   for (i=0;i<profile->AP_WWIDLength;i++)
   {
      profile->AP_WorldWideID[i] = hwInformation.WWID[i];
   }
#if SCSI_BIOS_ASPI8DOS
   profile->AP_BiosActive =
            (SCSI_ADPTSH(adapterTSH)->biosInformation.biosFlags.biosActive) ?
             HIM_TRUE : HIM_FALSE;
   profile->AP_ExtendedTrans = SCSI_ADPTSH(adapterTSH)->
               biosInformation.biosFlags.extendedTranslation;
#endif /* SCSI_BIOS_ASPI8DOS */
               
   profile->AP_MaxTargets = hhcb->maxDevices;               

   profile->AP_InitiatorMode = (hhcb->SCSI_HF_initiatorMode) ? HIM_TRUE : HIM_FALSE;
   profile->AP_TargetMode = (hhcb->SCSI_HF_targetMode) ? HIM_TRUE : HIM_FALSE;
   profile->AP_OverrideOSMNVRAMRoutines = (hhcb->SCSI_HF_OverrideOSMNVRAMRoutines) ? HIM_TRUE : HIM_FALSE;
   
#if SCSI_TARGET_OPERATION
   if (hhcb->SCSI_HF_targetMode)
   {
      profile->AP_TargetNumNexusTaskSetHandles = hhcb->SCSI_HF_targetNumberNexusHandles; 
      profile->AP_TargetNumNodeTaskSetHandles = hhcb->SCSI_HF_targetNumberNodeHandles;
      profile->AP_TargetDisconnectAllowed = 
         (hhcb->SCSI_HF_targetDisconnectAllowed) ? HIM_TRUE : HIM_FALSE;
      profile->AP_TargetTagEnable = (hhcb->SCSI_HF_targetTagEnable) ? HIM_TRUE : HIM_FALSE;
      /* profile->AP_OutofOrderTransfers = (hhcb->SCSI_HF_targetOutOfOrderTransfers) ? HIM_TRUE : HIM_FALSE;  */
      profile->AP_TargetNumIDs = hhcb->SCSI_HF_targetNumIDs;
      profile->AP_TargetInternalEstablishConnectionIOBlocks = hhcb->SCSI_HF_targetNumberEstScbs;
#if SCSI_RESOURCE_MANAGEMENT
/* May move into SCSI_GET_HW_INFORMATION */
      profile->AP_NexusHandleThreshold = hhcb->SCSI_HF_targetNexusThreshold;
      profile->AP_EC_IOBThreshold = hhcb->SCSI_HF_targetHiobQueueThreshold;      
      profile->AP_TargetAvailableEC_IOBCount = hhcb->SCSI_HF_targetHiobQueueCnt; 
      profile->AP_TargetAvailableNexusCount = hhcb->SCSI_HF_targetNexusQueueCnt;
#endif /* SCSI_RESOURCE_MANAGEMENT */

      profile->AP_SCSITargetOptTmFunctions.AP_SCSITargetAbortTask =
         (hhcb->SCSI_HF_targetAbortTask) ? HIM_TRUE : HIM_FALSE;  
      profile->AP_SCSITargetOptTmFunctions.AP_SCSITargetClearTaskSet =
         (hhcb->SCSI_HF_targetClearTaskSet) ? HIM_TRUE : HIM_FALSE;
      profile->AP_SCSITargetOptTmFunctions.AP_SCSITargetTerminateTask =
         (hhcb->SCSI_HF_targetTerminateTask) ? HIM_TRUE : HIM_FALSE;
      profile->AP_SCSITargetOptTmFunctions.AP_SCSI3TargetClearACA =
         (hhcb->SCSI_HF_targetClearACA) ? HIM_TRUE : HIM_FALSE;
      profile->AP_SCSITargetOptTmFunctions.AP_SCSI3TargetLogicalUnitReset =
         (hhcb->SCSI_HF_targetLogicalUnitReset) ? HIM_TRUE : HIM_FALSE;
             
      profile->himu.TS_SCSI.AP_SCSIHostTargetVersion = 
         hhcb->SCSI_HF_targetHostTargetVersion;
      profile->himu.TS_SCSI.AP_SCSI2_IdentifyMsgRsv = 
         (hhcb->SCSI_HF_targetScsi2IdentifyMsgRsv) ? HIM_TRUE : HIM_FALSE;
      profile->himu.TS_SCSI.AP_SCSI2_TargetRejectLuntar = 
         (hhcb->SCSI_HF_targetScsi2RejectLuntar) ? HIM_TRUE : HIM_FALSE;
      profile->himu.TS_SCSI.AP_SCSIGroup6CDBSize =
         hhcb->SCSI_HF_targetGroup6CDBsz;
      profile->himu.TS_SCSI.AP_SCSIGroup7CDBSize = 
         hhcb->SCSI_HF_targetGroup7CDBsz;
      profile->himu.TS_SCSI.AP_SCSITargetInitNegotiation =
         (hhcb->SCSI_HF_targetInitNegotiation) ? HIM_TRUE : HIM_FALSE;
      profile->himu.TS_SCSI.AP_SCSITargetIgnoreWideResidue =
         (hhcb->SCSI_HF_targetIgnoreWideResidMsg) ? HIM_TRUE : HIM_FALSE;
      profile->himu.TS_SCSI.AP_SCSITargetEnableSCSI1Selection =
         (hhcb->SCSI_HF_targetEnableScsi1Selection) ? HIM_TRUE : HIM_FALSE;

      /* get device table entry for this adapter's SCSI ID */ 
      deviceTable =  &SCSI_DEVICE_TABLE(hhcb)[(HIM_UINT8) hwInformation.hostScsiID];
      /* Note that; if NVRAM/EEPROM was not present all device table entries          */
      /* get SCSI_DF_ultraEnable set to the same value - the max supported by the h/w */  
   
    
      /* Revisit this area */ 
      profile->himu.TS_SCSI.AP_SCSITargetMaxSpeed = 
         hwInformation.targetInfo[hwInformation.hostScsiID].SCSIMaxSpeed;
      profile->himu.TS_SCSI.AP_SCSITargetDefaultSpeed =
         hwInformation.targetInfo[hwInformation.hostScsiID].SCSIDefaultSpeed;

      profile->himu.TS_SCSI.AP_SCSITargetMaxOffset = 
         hwInformation.targetInfo[hwInformation.hostScsiID].SCSIMaxOffset;
      profile->himu.TS_SCSI.AP_SCSITargetDefaultOffset =
         hwInformation.targetInfo[hwInformation.hostScsiID].SCSIDefaultOffset;

      profile->himu.TS_SCSI.AP_SCSITargetMaxWidth = 
         hwInformation.targetInfo[hwInformation.hostScsiID].SCSIMaxWidth;
      profile->himu.TS_SCSI.AP_SCSITargetDefaultWidth = 
         hwInformation.targetInfo[hwInformation.hostScsiID].SCSIDefaultWidth;

      profile->himu.TS_SCSI.AP_SCSITargetAdapterIDMask =
         hwInformation.targetIDMask;

   }  /* end of hhcb->SCSI_HF_targetMode */
   
   
#endif /* SCSI_TARGET_OPERATION */

   /* Getting the threshold is from the hhcb and changing it changes */
   /* the hhcb and the HA hardware if the host is idle. */
   profile->AP_FIFOSeparateRWThreshold = 
      (hwInformation.SCSI_PF_separateRWThreshold) ? HIM_TRUE : HIM_FALSE;      
   profile->AP_FIFOSeparateRWThresholdEnable = 
      (hwInformation.SCSI_PF_separateRWThresholdEnable) ? HIM_TRUE : HIM_FALSE;      

   if (hhcb->hardwareMode == SCSI_HMODE_AIC78XX)
   {
      switch (hwInformation.threshold)
      {
         case 0x00:
            profile->AP_FIFOThreshold = 13;
            break;
         case 0x01:
            profile->AP_FIFOThreshold = 50;
            break;
         case 0x02:
            profile->AP_FIFOThreshold = 75;
            break;
         case 0x03:
            profile->AP_FIFOThreshold = 100;
            break;
         default:
            break; 
      }   
   }
   else
   {
      if ((hwInformation.SCSI_PF_separateRWThresholdEnable))
      {
         /* Determine AP_FIFOWriteThreshold value */
         switch (hwInformation.writeThreshold)
         {
            case 0x00:
               profile->AP_FIFOWriteThreshold = 16;
               break;
            case 0x01:
               profile->AP_FIFOWriteThreshold = 25;
               break;
            case 0x02:
               profile->AP_FIFOWriteThreshold = 50;
               break;
            case 0x03:
               profile->AP_FIFOWriteThreshold = 62;
               break;
            case 0x04:
               profile->AP_FIFOWriteThreshold = 75;
               break;
            case 0x05:
               profile->AP_FIFOWriteThreshold = 85;
               break;
            case 0x06:
               profile->AP_FIFOWriteThreshold = 90;
               break;
            case 0x07:
               profile->AP_FIFOWriteThreshold = 100;
            default:
               break;
         }

         /* Determine AP_FIFOReadThreshold value */
         switch (hwInformation.readThreshold)
         {
            case 0x00:
               profile->AP_FIFOReadThreshold = 16;
               break;
            case 0x01:
               profile->AP_FIFOReadThreshold = 25;
               break;
            case 0x02:
               profile->AP_FIFOReadThreshold = 50;
               break;
            case 0x03:
               profile->AP_FIFOReadThreshold = 62;
               break;
            case 0x04:
               profile->AP_FIFOReadThreshold = 75;
               break;
            case 0x05:
               profile->AP_FIFOReadThreshold = 85;
               break;
            case 0x06:
               profile->AP_FIFOReadThreshold = 90;
               break;
            case 0x07:
               profile->AP_FIFOReadThreshold = 100;
           default:
               break;
         }
      }
      else 
      {
         /* Determine AP_FIFOThreshold field value */ 
         switch (hwInformation.threshold)
         {
            case 0x00:
               profile->AP_FIFOThreshold = 16;
               break;
            case 0x01:
               profile->AP_FIFOThreshold = 25;
               break;
            case 0x02:
               profile->AP_FIFOThreshold = 50;
               break;
            case 0x03:
               profile->AP_FIFOThreshold = 62;
               break;
            case 0x04:
               profile->AP_FIFOThreshold = 75;
               break;
            case 0x05:
               profile->AP_FIFOThreshold = 85;
               break;
            case 0x06:
               profile->AP_FIFOThreshold = 90;
               break;
            case 0x07:
               profile->AP_FIFOThreshold = 100;
            default:
               break;
         }
      }
   }
   
#if SCSI_BIOS_ASPI8DOS   
   /* Get the first drive supported by the bios for this adapter */
   if (SCSI_ADPTSH(adapterTSH)->biosInformation.biosFlags.biosActive)
   {
      profile->AP_LowestScanTarget = 
         (HIM_UINT16) SCSI_ADPTSH(adapterTSH)->biosInformation.firstBiosDrive;
   }
   else
#endif /* SCSI_BIOS_ASPI8DOS */  
   {
      profile->AP_LowestScanTarget = (HIM_UINT16) 0xffff;
   }

   profile->AP_AllocBusAddressSize = SCSI_ADPTSH(adapterTSH)->allocBusAddressSize; 

   profile->AP_ResetDelay = (HIM_UINT32)hhcb->resetDelay;
   /* Not supported at this time
   profile->himu.TS_SCSI.AP_CleanSG = ??;
   profile->himu.TS_SCSI.AP_SCSIForceWide = ??;
   profile->himu.TS_SCSI.AP_SCSIForceNoWide = ??;
   profile->himu.TS_SCSI.AP_SCSIForceSynch = ??;
   profile->himu.TS_SCSI.AP_SCSIForceNoSynch = ??;*/

   /* Getting the Adapter SCSI ID */
   profile->himu.TS_SCSI.AP_SCSIAdapterID = (HIM_UINT8) hwInformation.hostScsiID;

   /* Report the speed supported by the adapter */
   profile->himu.TS_SCSI.AP_SCSISpeed = HIM_SCSI_NORMAL_SPEED;
   for (i=0; i<SCSI_MAXDEV; i++)
   {
      if (hwInformation.targetInfo[i].SCSIMaxSpeed == 800)
      {
         profile->himu.TS_SCSI.AP_SCSISpeed = HIM_SCSI_ULTRA160M_SPEED;
         break;
      }
      if (hwInformation.targetInfo[i].SCSIMaxSpeed == 400)
      {
         profile->himu.TS_SCSI.AP_SCSISpeed = HIM_SCSI_ULTRA2_SPEED;
      }
      else if (hwInformation.targetInfo[i].SCSIMaxSpeed == 200)
      {
         if (profile->himu.TS_SCSI.AP_SCSISpeed != HIM_SCSI_ULTRA2_SPEED)
         {
            profile->himu.TS_SCSI.AP_SCSISpeed = HIM_SCSI_ULTRA_SPEED;
         }
      }
       else if (hwInformation.targetInfo[i].SCSIMaxSpeed == 100)
      {
         if ((profile->himu.TS_SCSI.AP_SCSISpeed != HIM_SCSI_ULTRA_SPEED) &&
             (profile->himu.TS_SCSI.AP_SCSISpeed != HIM_SCSI_ULTRA2_SPEED) )
         {
            profile->himu.TS_SCSI.AP_SCSISpeed = HIM_SCSI_FAST_SPEED;
         }
      }
   }
   
   if (hwInformation.SCSI_PF_wideSupport)
   {
      profile->himu.TS_SCSI.AP_SCSIWidth = 16;
   }
   else
   {
      profile->himu.TS_SCSI.AP_SCSIWidth = 8;
   }
      
   for (i = 0; i < SCSI_MAXDEV; i++)
   {
      /* Currently, our HAs only support up to 16 SCSI devices */
      profile->himu.TS_SCSI.AP_SCSINumberLuns[i] = (HIM_UINT8)SCSI_ADPTSH(adapterTSH)->NumberLuns[i];
   }

   profile->AP_CacheLineStreaming = 
            (hwInformation.SCSI_PF_cacheThEn) ? HIM_TRUE : HIM_FALSE;   
            
   if(hhcb->SCSI_HF_expSupport)
   {
      profile->himu.TS_SCSI.AP_SCSIExpanderDetection = HIM_TRUE;
   }
   else
   {
      profile->himu.TS_SCSI.AP_SCSIExpanderDetection = HIM_FALSE;
   }
   
   /* Get the current address mapped: I/O or memory */
   profile->AP_MemoryMapped = SCSI_ADPTSH(adapterTSH)->memoryMapped;

   profile->himu.TS_SCSI.AP_SCSIDisableParityErrors =
            (hwInformation.SCSI_PF_scsiParity) ? HIM_FALSE : HIM_TRUE;

   /* Selection Timeout will be either 256, 128, 64 or 32 and    */
   /* equivalent to binary number value as 00b, 01b, 10b or 11b. */
   profile->himu.TS_SCSI.AP_SCSISelectionTimeout =
                     ((HIM_UINT16) 256) >> hwInformation.SCSI_PF_selTimeout;

   switch (hwInformation.SCSI_PF_transceiverMode)
   {
      case SCSI_UNKNOWN_MODE:
         profile->himu.TS_SCSI.AP_SCSITransceiverMode = HIM_SCSI_UNKNOWN_MODE;
         break;

      case SCSI_SE_MODE:
         profile->himu.TS_SCSI.AP_SCSITransceiverMode = HIM_SCSI_SE_MODE; 
         break;

      case SCSI_LVD_MODE:
         profile->himu.TS_SCSI.AP_SCSITransceiverMode = HIM_SCSI_LVD_MODE;
         break;

      case SCSI_HVD_MODE:
         profile->himu.TS_SCSI.AP_SCSITransceiverMode = HIM_SCSI_HVD_MODE;
         break;       
   }

   profile->himu.TS_SCSI.AP_SCSIDomainValidationMethod =
         SCSI_ADPTSH(adapterTSH)->domainValidationMethod;

#if SCSI_PPR_ENABLE
   if (hhcb->hardwareMode == SCSI_HMODE_AICTRIDENT)
   {
      profile->himu.TS_SCSI.AP_SCSIPPRSupport = HIM_TRUE;
   }
#endif

   profile->AP_indexWithinGroup = (HIM_UINT8) hhcb->indexWithinGroup;

   if (!hwInformation.intrThresholdSupport || hwInformation.intrThreshold==0)
   {
      /* CmdCmpltIntr reduction logic is NOT enabled in these 2 cases:  */
      /*  a. F/W does not support it (hwinfo.intrThresholdSupport==0)   */
      /*  b. Current hwinfo.intrThreshold value is set to zero          */
      profile->AP_CmdCompleteIntrThresholdSupport = HIM_FALSE;
   }
   else
   {
      profile->AP_CmdCompleteIntrThresholdSupport = HIM_TRUE;
   }
                  
   profile->AP_IntrThreshold = (HIM_UINT8)hwInformation.intrThreshold;
   profile->AP_SaveRestoreSequencer = 
             (hhcb->SCSI_HF_dontSaveSeqInState) ? HIM_FALSE : HIM_TRUE; 

   if (profile->AP_SaveRestoreSequencer == HIM_FALSE)
   {
      /* adjust AP_StateSize as we will not save sequencer in SCSI_STATE */ 
      profile->AP_StateSize = (HIM_UINT32)OSMoffsetof(SCSI_STATE,seqRam[0]);
   }

   profile->AP_ClusterEnabled =
             (hhcb->SCSI_HF_clusterEnable) ? HIM_TRUE : HIM_FALSE; 

   return((HIM_UINT8)(HIM_SUCCESS));
#else
   return((HIM_UINT8)(HIM_FAILURE));

#endif /* SCSI_PROFILE_INFORMATION */
 }

/*********************************************************************
*
*  SCSIReportAdjustableAdapterProfile
*
*     Get information about which adapter profile information are
*     adjustable
*
*  Return Value:  HIM_SUCCESS, normal return
*                 HIM_FAILURE, operation failure
*                  
*  Parameters:    adapter TSH
*                 profile adjust mask memory
*
*  Remarks:       
*                 
*********************************************************************/
HIM_UINT8 SCSIReportAdjustableAdapterProfile (HIM_TASK_SET_HANDLE adapterTSH,
                                     HIM_ADAPTER_PROFILE HIM_PTR profileMask)
{
#if SCSI_PROFILE_INFORMATION
    SCSI_HHCB HIM_PTR hhcb = (SCSI_HHCB HIM_PTR) &SCSI_ADPTSH(adapterTSH)->hhcb; 
    SCSI_HW_INFORMATION hwInformation;    

   /* copy the adjustable adapter profile for now */
   OSDmemcpy(profileMask,&SCSIAdjustableAdapterProfile,sizeof(HIM_ADAPTER_PROFILE));

   SCSI_GET_HW_INFORMATION(&hwInformation, hhcb);

   if (hhcb->hardwareMode == SCSI_HMODE_AIC78XX)
   {
      profileMask->AP_FIFOSeparateRWThresholdEnable = HIM_FALSE;
      profileMask->AP_FIFOWriteThreshold = 0;
      profileMask->AP_FIFOReadThreshold = 0;
   }
   
   /* Interrupt reduction logic is not supported in legacy firmware */
   if (
       (hhcb->firmwareMode == SCSI_FMODE_STANDARD64)||
       (hhcb->firmwareMode == SCSI_FMODE_STANDARD32)||
       (hhcb->firmwareMode == SCSI_FMODE_SWAPPING64)||
       (hhcb->firmwareMode == SCSI_FMODE_SWAPPING32)||
       (hhcb->firmwareMode == SCSI_FMODE_DOWNSHIFT)
      )
   {
      profileMask->AP_CmdCompleteIntrThresholdSupport = HIM_FALSE;
      profileMask->AP_IntrThreshold                   = 0;      
   }
   
   profileMask->AP_CacheLineStreaming = 
      (hwInformation.SCSI_PF_cacheThEnAdjustable) ? HIM_TRUE : HIM_FALSE;
   
   return((HIM_UINT8)(HIM_SUCCESS));
#else
   return((HIM_UINT8)(HIM_FAILURE));

#endif /* SCSI_PROFILE_INFORMATION */
}

/*********************************************************************
*
*  SCSIAdjustAdapterProfile
*
*     Apply the adjusted adapter profile information
*
*  Return Value:  HIM_SUCCESS, normal return
*                 HIM_FAILURE, HIM was unable to make the legal changes
*                 requested by the OSM.
*                 HIM_ILLEGAL_CHANGE, illegal change
*                 HIM_ADAPTER_NOT_IDLE, profile adjusted is not allowed
*                  
*  Parameters:    adapter TSH
*                 profile memory
*
*  Remarks:       The profile mask should be acquired before
*                 adjusting adapter profile.
*                 
*********************************************************************/
HIM_UINT8 SCSIAdjustAdapterProfile (HIM_TASK_SET_HANDLE adapterTSH,
                                    HIM_ADAPTER_PROFILE HIM_PTR profile)
                                    
{                              
#if SCSI_PROFILE_INFORMATION
   SCSI_UINT16 i;
   SCSI_HHCB HIM_PTR hhcb = (SCSI_HHCB HIM_PTR) &SCSI_ADPTSH(adapterTSH)->hhcb;
   SCSI_HW_INFORMATION hwInformation;
   SCSI_UEXACT8 hostScsiID; 
#if SCSI_TARGET_OPERATION 
#if SCSI_MULTIPLEID_SUPPORT
   SCSI_UEXACT16 targetAdapterIDMask;
   SCSI_UINT8 count;
#endif /* SCSI_MULTIPLEID_SUPPORT */
#endif /* SCSI_TARGET_OPERATION */

   /* Return if the host adapter's already initialized and not idle */
   if ((SCSI_ADPTSH(adapterTSH)->SCSI_AF_initialized) &&
       (!SCSI_CHECK_HOST_IDLE(hhcb)))
   {
      return((HIM_UINT8)(HIM_ADAPTER_NOT_IDLE));
   }

   SCSI_GET_HW_INFORMATION(&hwInformation, hhcb);

   /* Not supported at this time
   ?? = profile->himu.TS_SCSI.AP_CleanSG;
   ?? = profile->himu.TS_SCSI.AP_SCSIForceWide;
   ?? = profile->himu.TS_SCSI.AP_SCSIForceNoWide;
   ?? = profile->himu.TS_SCSI.AP_SCSIForceSynch;
   ?? = profile->himu.TS_SCSI.AP_SCSIForceNoSynch;*/

   hhcb->SCSI_HF_OverrideOSMNVRAMRoutines = profile->AP_OverrideOSMNVRAMRoutines;
   
   /* Only support AP_ResetDelay values up to SCSI_MAX_RESET_DELAY */ 

   if ((SCSI_UEXACT32)profile->AP_ResetDelay > SCSI_MAX_RESET_DELAY)
   {
      hhcb->resetDelay = SCSI_MAX_RESET_DELAY;
   }
   else
   { 
      hhcb->resetDelay = (SCSI_UEXACT32)profile->AP_ResetDelay;
   } 

   hostScsiID = hwInformation.hostScsiID;

   if ((SCSI_ADPTSH(adapterTSH)->SCSI_AF_initialized &&
       (hostScsiID != (SCSI_UEXACT8)profile->himu.TS_SCSI.AP_SCSIAdapterID)) ||
       (profile->AP_FIFOSeparateRWThreshold == HIM_FALSE &&
        profile->AP_FIFOSeparateRWThresholdEnable == HIM_TRUE))
   {
      return((HIM_UINT8)(HIM_ILLEGAL_CHANGE)); 
   }
   
   if (!SCSI_ADPTSH(adapterTSH)->SCSI_AF_initialized)
   {
      /* Can only change these fields prior to HIMInitialize */ 
      hhcb->hostScsiID = (SCSI_UEXACT8)profile->himu.TS_SCSI.AP_SCSIAdapterID;
#if SCSI_TARGET_OPERATION && SCSI_MULTIPLEID_SUPPORT
      if (hhcb->SCSI_HF_targetMode)
      {
         targetAdapterIDMask = profile->himu.TS_SCSI.AP_SCSITargetAdapterIDMask;

         if (hhcb->SCSI_HF_targetAdapterIDMask !=
             targetAdapterIDMask)
         {
            count = 0;
            for (i = 0; i < 16; i++)
            {
               /* count the mask bits */
               if (targetAdapterIDMask & (1 << i))
               {
                  count++;
               }
            }

            /* If the number of bits set in the mask is greater 
             * than the number of IDs supported or if the adapter
             * SCSI ID bit is not set in the mask when the mask is
             * non-zero return an error.
             */ 
            if ((count > hhcb->SCSI_HF_targetNumIDs) ||
                (((targetAdapterIDMask & 
                   (1 << hhcb->hostScsiID)) == 0) && 
                 (targetAdapterIDMask != 0)))
            {
               return((HIM_UINT8)(HIM_ILLEGAL_CHANGE)); 
            }
         }

         /* update mask */
         hhcb->SCSI_HF_targetAdapterIDMask = targetAdapterIDMask;
      }
#endif /* SCSI_TARGET_OPERATION && SCSI_MULTIPLEID_SUPPORT */
   }    

#if SCSI_TARGET_OPERATION
   if (hhcb->SCSI_HF_targetMode)
   {
      hhcb->SCSI_HF_targetDisconnectAllowed = profile->AP_TargetDisconnectAllowed;
      hhcb->SCSI_HF_targetTagEnable = profile->AP_TargetTagEnable;
      hhcb->SCSI_HF_targetHostTargetVersion = 
         (SCSI_UEXACT8)profile->himu.TS_SCSI.AP_SCSIHostTargetVersion;
      hhcb->SCSI_HF_targetScsi2IdentifyMsgRsv =
         profile->himu.TS_SCSI.AP_SCSI2_IdentifyMsgRsv;
      hhcb->SCSI_HF_targetScsi2RejectLuntar = 
         profile->himu.TS_SCSI.AP_SCSI2_TargetRejectLuntar; 
      hhcb->SCSI_HF_targetGroup6CDBsz = 
         (SCSI_UEXACT8)profile->himu.TS_SCSI.AP_SCSIGroup6CDBSize;
      hhcb->SCSI_HF_targetGroup7CDBsz =
         (SCSI_UEXACT8)profile->himu.TS_SCSI.AP_SCSIGroup7CDBSize;
      hhcb->SCSI_HF_targetNexusThreshold = 
         (SCSI_UEXACT16)profile->AP_NexusHandleThreshold;
      hhcb->SCSI_HF_targetHiobQueueThreshold = 
         (SCSI_UEXACT16)profile->AP_EC_IOBThreshold;      
      hhcb->SCSI_HF_targetInitNegotiation =
         profile->himu.TS_SCSI.AP_SCSITargetInitNegotiation;
      hhcb->SCSI_HF_targetIgnoreWideResidMsg =
         profile->himu.TS_SCSI.AP_SCSITargetIgnoreWideResidue;
      hhcb->SCSI_HF_targetEnableScsi1Selection = 
         profile->himu.TS_SCSI.AP_SCSITargetEnableSCSI1Selection;
   
      hhcb->SCSI_HF_targetAbortTask =
         profile->AP_SCSITargetOptTmFunctions.AP_SCSITargetAbortTask;  
      hhcb->SCSI_HF_targetClearTaskSet =
         profile->AP_SCSITargetOptTmFunctions.AP_SCSITargetClearTaskSet;
      hhcb->SCSI_HF_targetTerminateTask =
         profile->AP_SCSITargetOptTmFunctions.AP_SCSITargetTerminateTask;
      if (hhcb->SCSI_HF_targetHostTargetVersion == HIM_SCSI_3) 
      {
         hhcb->SCSI_HF_targetClearACA =
            profile->AP_SCSITargetOptTmFunctions.AP_SCSI3TargetClearACA;
         hhcb->SCSI_HF_targetLogicalUnitReset =
            profile->AP_SCSITargetOptTmFunctions.AP_SCSI3TargetLogicalUnitReset;               
      }
      
#if SCSI_MAILBOX_ENABLE
      if (profile->AP_TargetInternalEstablishConnectionIOBlocks >
          SCSI_MAX_MAILBOX_EST_SCBS)
      {
         return((HIM_UINT8)(HIM_ILLEGAL_CHANGE));
      }
#endif /* SCSI_MAILBOX_ENABLE */
      if (profile->AP_TargetInternalEstablishConnectionIOBlocks == 0 || 
          profile->AP_TargetInternalEstablishConnectionIOBlocks >= 
          (HIM_UINT32)(hhcb->numberScbs - 3))
      {
         /* Make sure we have a valid number of establish connection SCBs */
         /* We need some scbs for initiator mode and/or reestablish SCBs */
         /* Note; at some point this test may become firmware mode specific */   
         return((HIM_UINT8)(HIM_ILLEGAL_CHANGE));
      }
      else
      {
         hhcb->SCSI_HF_targetNumberEstScbs = 
            (SCSI_UEXACT8)profile->AP_TargetInternalEstablishConnectionIOBlocks;
      }

      /* Target Mode Negotiation rates */ 
      /* For now assume only changeable before HIMInitialize */ 
      /* Significant changes would be required to this code  */
      /* if these fields are changable after HimInitialize.  */ 
      if (!SCSI_ADPTSH(adapterTSH)->SCSI_AF_initialized)
      {
         /********************************************************
          * WARNING: THIS HAS NOT BEEN IMPLEMENTED YET
          ********************************************************/
         /* Check if change in Target negotiation rates requested */
         /* If so, change all device table entries.               */      
         /* Get a current device table entry - use host adapter's */
         /* ID entry.                                             */ 

      } /* !SCSI_ADPTSH(adapterTSH)->SCSI_AF_initialized */
  
      hwInformation.targetIDMask = hhcb->SCSI_HF_targetAdapterIDMask;
   } /* hhcb->SCSI_HF_targetMode */
#endif /* SCSI_TARGET_OPERATION */

   for (i = 0; i < SCSI_MAXDEV; i++)
   {
      /* Update to the maximum SCSI devices currently support by HIM */
      SCSI_ADPTSH(adapterTSH)->NumberLuns[i] = 
           (SCSI_UEXACT8)profile->himu.TS_SCSI.AP_SCSINumberLuns[i];
   }

   /* Getting the threshold is from the hardware and changing it */
   /* changes the hhcb and the HA hardware if the host is idle. */
   if (hhcb->hardwareMode == SCSI_HMODE_AIC78XX) 
   {
      if (profile->AP_FIFOThreshold < 32) /* 13% */
      {
         hwInformation.threshold = 0x00;
      }
      else if (profile->AP_FIFOThreshold < 63) /* 50% */
      {
         hwInformation.threshold = 0x01;
      }
      else if (profile->AP_FIFOThreshold < 88) /* 75% */
      {
         hwInformation.threshold = 0x02;
      }
      else if (profile->AP_FIFOThreshold >= 88) /* 100% */
      {
         hwInformation.threshold = 0x03;
      }
   }
   else
   {
      hwInformation.SCSI_PF_separateRWThresholdEnable =
         profile->AP_FIFOSeparateRWThresholdEnable;
      
      /* Separate read/write thresholds */
      if (profile->AP_FIFOSeparateRWThresholdEnable)
      {
         /* write threshold */
         if (profile->AP_FIFOWriteThreshold < 20)       /* 16% */
         {
            hwInformation.writeThreshold = 0x00;
         } 
         else if (profile->AP_FIFOWriteThreshold < 37)  /* 25% */
         {
            hwInformation.writeThreshold = 0x01;
         }
         else if (profile->AP_FIFOWriteThreshold < 56)  /* 50% */
         {
            hwInformation.writeThreshold = 0x02;
         } 
         else if (profile->AP_FIFOWriteThreshold < 69)  /* 62.5% */
         {
            hwInformation.writeThreshold = 0x03;
         } 
         else if (profile->AP_FIFOWriteThreshold < 80)  /* 75% */
         {
            hwInformation.writeThreshold = 0x04;
         }
         else if (profile->AP_FIFOWriteThreshold < 88)  /* 85% */
         {
            hwInformation.writeThreshold = 0x05;
         } 
         else if (profile->AP_FIFOWriteThreshold < 95)  /* 90% */
         {
            hwInformation.writeThreshold = 0x06;
         } 
         else if (profile->AP_FIFOWriteThreshold >=95)  /* 100% */
         {
            hwInformation.writeThreshold = 0x07;
         }

         /* read threshold */
         if (profile->AP_FIFOReadThreshold < 20)       /* 16% */
         {
            hwInformation.readThreshold = 0x00;
         }
         else if (profile->AP_FIFOReadThreshold < 37)  /* 25% */
         {
            hwInformation.readThreshold = 0x01;
         } 
         else if (profile->AP_FIFOReadThreshold < 56)  /* 50% */
         {
            hwInformation.readThreshold = 0x02;
         }
         else if (profile->AP_FIFOReadThreshold < 69)  /* 62.5% */
         {
            hwInformation.readThreshold = 0x03;
         }
         else if (profile->AP_FIFOReadThreshold < 80)  /* 75% */
         {
            hwInformation.readThreshold = 0x04;
         }
         else if (profile->AP_FIFOReadThreshold < 88)  /* 85% */
         {
            hwInformation.readThreshold = 0x05;
         }
         else if (profile->AP_FIFOReadThreshold < 95)  /* 90% */
         {
            hwInformation.readThreshold = 0x06;
         }
         else if (profile->AP_FIFOReadThreshold >=95)  /* 100% */
         {
            hwInformation.readThreshold = 0x07;
         } 
      }
      else 
      {
         /* Use AP_FIFOThreshold field */
         if (profile->AP_FIFOThreshold < 20)       /* 16% */
         {
            hwInformation.threshold = 0x00;
         } 
         else if (profile->AP_FIFOThreshold < 37)  /* 25% */
         {
            hwInformation.threshold = 0x01;
         } 
         else if (profile->AP_FIFOThreshold < 56)  /* 50% */
         {
            hwInformation.threshold = 0x02;
         }
         else if (profile->AP_FIFOThreshold < 69)  /* 62.5% */
         {
            hwInformation.threshold = 0x03;
         }
         else if (profile->AP_FIFOThreshold < 80)  /* 75% */
         {
            hwInformation.threshold = 0x04;
         }
         else if (profile->AP_FIFOThreshold < 88)  /* 85% */
         { 
            hwInformation.threshold = 0x05;
         }
         else if (profile->AP_FIFOThreshold < 95)  /* 90% */
         {
            hwInformation.threshold = 0x06;
         } 
         else if (profile->AP_FIFOThreshold >=95)  /* 100% */
         {
            hwInformation.threshold = 0x07;
         }
      }
   }

   /* Getting the Adapter (SCSI) ID and changes the hhcb */
   /* as well as the HA hardware if the host is idle. */
   hwInformation.hostScsiID = (SCSI_UEXACT8)profile->himu.TS_SCSI.AP_SCSIAdapterID;

   /* Getting Parity Error flag */
   hwInformation.SCSI_PF_scsiParity = 
      (profile->himu.TS_SCSI.AP_SCSIDisableParityErrors == HIM_TRUE) ? 0 : 1;

   /* Getting selection timeout by converting the decimal value to */
   /* a binary number value that will be programmed into HW register. */
   if (profile->himu.TS_SCSI.AP_SCSISelectionTimeout > 192)       /* 256 ms */
   {
      hwInformation.SCSI_PF_selTimeout = 0;
   }
   else if (profile->himu.TS_SCSI.AP_SCSISelectionTimeout > 96)   /* 128 ms */
   {
      hwInformation.SCSI_PF_selTimeout = 1;
   }
   else if (profile->himu.TS_SCSI.AP_SCSISelectionTimeout > 48)   /* 64 ms */
   {
      hwInformation.SCSI_PF_selTimeout = 2;
   }
   else                                                           /* 32 ms */
   {
      hwInformation.SCSI_PF_selTimeout = 3;
   }

#if SCSI_DOMAIN_VALIDATION
   if (profile->himu.TS_SCSI.AP_SCSIDomainValidationMethod > 2)
   {
      return((HIM_UINT8)(HIM_ILLEGAL_CHANGE));
   }
   else
   {
#if SCSI_DOMAIN_VALIDATION_ENHANCED
      SCSI_ADPTSH(adapterTSH)->domainValidationMethod =
            profile->himu.TS_SCSI.AP_SCSIDomainValidationMethod;
#else /* SCSI_DOMAIN_VALIDATION_ENHANCED */
      if (profile->himu.TS_SCSI.AP_SCSIDomainValidationMethod > 1)
      {
         return((HIM_UINT8)(HIM_ILLEGAL_CHANGE));
      }
      else
      {
         SCSI_ADPTSH(adapterTSH)->domainValidationMethod =
               profile->himu.TS_SCSI.AP_SCSIDomainValidationMethod;
      }
#endif /* SCSI_DOMAIN_VALIDATION_ENHANCED */
   }
#else /* SCSI_DOMAIN_VALIDATION */
   if (profile->himu.TS_SCSI.AP_SCSIDomainValidationMethod)
   {
      return((HIM_UINT8)(HIM_ILLEGAL_CHANGE));
   }
#endif /* SCSI_DOMAIN_VALIDATION */

#if SCSI_ARO_SUPPORT
   /* Allow OSM to change index within group for ARO only */
   hhcb->indexWithinGroup = (SCSI_UEXACT8) profile->AP_indexWithinGroup;
#endif /* SCSI_ARO_SUPPORT */

   if (hwInformation.intrThresholdSupport)
   {
      /* This is only for f/w that support CmdCmpltIntr reduction logic */ 

      if (profile->AP_CmdCompleteIntrThresholdSupport == HIM_TRUE)
      {
         if (profile->AP_IntrThreshold > 0x7F)
         {
            hwInformation.intrThreshold = 0x7F;
         }
         else
         {
            hwInformation.intrThreshold = (SCSI_UEXACT8)profile->AP_IntrThreshold;
         }
      }
      else                                        
      {  /* Disable Command Complete Interrupt Reduction logic */
         hwInformation.intrThreshold = 0; 
      }
   }

   if (profile->AP_SaveRestoreSequencer == HIM_TRUE)
   {
      hhcb->SCSI_HF_dontSaveSeqInState = 0;
   }
   else
   {
      hhcb->SCSI_HF_dontSaveSeqInState = 1;
   }  

   if (((profile->AP_CacheLineStreaming==HIM_TRUE)  && !hwInformation.SCSI_PF_cacheThEn)||
       ((profile->AP_CacheLineStreaming==HIM_FALSE) &&  hwInformation.SCSI_PF_cacheThEn))
   {
      /* Lets check if the requested change is valid */
      if (hwInformation.SCSI_PF_cacheThEnAdjustable)
      {
         hwInformation.SCSI_PF_cacheThEn =
            (profile->AP_CacheLineStreaming==HIM_TRUE) ? 1 : 0;
      }
      else
      {   /* Default setting of CacheLineStreaming on this adapter cannot be changed */
          return((HIM_UINT8)(HIM_ILLEGAL_CHANGE));
      }    
   }

   SCSI_PUT_HW_INFORMATION(&hwInformation, hhcb);
   
   return((HIM_UINT8)(HIM_SUCCESS));
#else
   return((HIM_UINT8)(HIM_FAILURE));

#endif /* SCSI_PROFILE_INFORMATION */
}

/*********************************************************************
*
*  SCSIProfileTarget
*
*     Get target profile information
*
*  Return Value:  HIM_SUCCESS, normal return
*                 HIM_FAILURE, operation failure
*                  
*  Parameters:    target TSH
*                 profile memory
*
*  Remarks:       Some of the profile information are transport
*                 specific and OSM should either avoid or isolate
*                 accessing profile information if possible.
*                 Pretty much CHIM will have to maintain the
*                 profile information itself.
*                 
*********************************************************************/
HIM_UINT8 SCSIProfileTarget (HIM_TASK_SET_HANDLE targetTSH,
                             HIM_TARGET_PROFILE HIM_PTR profile)
{
#if SCSI_PROFILE_INFORMATION && SCSI_INITIATOR_OPERATION
#if SCSI_SCAM_ENABLE + SCSI_BIOS_ASPI8DOS
   SCSI_UINT16 i;
#endif /* SCSI_SCAM_ENABLE + SCSI_BIOS_ASPI8DOS */
   SCSI_ADAPTER_TSCB HIM_PTR adapterTSH =
               ((SCSI_TARGET_TSCB HIM_PTR)targetTSH)->adapterTSH;
   SCSI_HHCB HIM_PTR hhcb = (SCSI_HHCB HIM_PTR) &SCSI_ADPTSH(adapterTSH)->hhcb;
   SCSI_UEXACT8 scsiID = (SCSI_UEXACT8)SCSI_TRGTSH(targetTSH)->scsiID;
   SCSI_DEVICE SCSI_DPTR deviceTable = &SCSI_DEVICE_TABLE(hhcb)[scsiID];
   SCSI_HW_INFORMATION hwInformation;

   if (!hhcb->SCSI_HF_initiatorMode)
   {
      return((HIM_UINT8)(HIM_FAILURE));
   }

   SCSI_GET_HW_INFORMATION(&hwInformation, hhcb);
   
   /* copy the default target profile */
   OSDmemcpy(profile,&SCSIDefaultTargetProfile,sizeof(HIM_TARGET_PROFILE));
   
   /* update fields which are adapter/target specific */
#if SCSI_SCAM_ENABLE
   for (i = 0; i < profile->TP_WWIDLength; i++)
   {
      profile->TP_WorldWideID[i] = deviceTable->scamWWID[i];
   }
   if (deviceTable->SCSI_DF_scamDevice)
   {
      profile->himu.TS_SCSI.TP_SCSIScamSupport = HIM_SCAM1;
   }
   else
   {
      if (deviceTable->SCSI_DF_scamTolerant)
      {
         profile->himu.TS_SCSI.TP_SCSIScamSupport = HIM_SCAM_TOLERANT;
      }
      else
      {
         profile->himu.TS_SCSI.TP_SCSIScamSupport = HIM_SCAM_INTOLERANT;
      }
   }
#else
   profile->himu.TS_SCSI.TP_SCSIScamSupport = HIM_SCAM_INTOLERANT;
#endif /* SCSI_SCAM_ENABLE */
                                
   /* Get the scan order from the bios information. Only if BIOS is active */
   profile->TP_ScanOrder = 0xffff;
#if SCSI_BIOS_ASPI8DOS
   if (SCSI_ADPTSH(adapterTSH)->biosInformation.biosFlags.biosActive)
   {
      for (i=0;i<SCSI_ADPTSH(adapterTSH)->biosInformation.numberDrives;i++)
      {
         if (SCSI_ADPTSH(adapterTSH)->biosInformation.biosDrive[i].targetID == scsiID)
         {
            profile->TP_ScanOrder = i + 
                  (HIM_UINT16) SCSI_ADPTSH(adapterTSH)->biosInformation.firstBiosDrive;
         }
      }
   }
#endif /* SCSI_BIOS_ASPI8DOS */
   
   /* MaxActiveCommands calculated from hwInformation structure which gets */
   /* the parameter from the unit control structure internally. The field */
   /* used is maxTagScbs since it is not possible to know tagged or */
   /* nontagged since tagged is set on an IOB basis. */
   profile->TP_MaxActiveCommands = hwInformation.targetInfo[scsiID].maxTagScbs;

   profile->TP_TaggedQueuing = 
            (SCSI_ADPTSH(adapterTSH)->tagEnable & (1 << scsiID)) ?
             HIM_TRUE : HIM_FALSE;

   profile->TP_HostManaged =
            (deviceTable->SCSI_DF_hostManaged) ? HIM_TRUE : HIM_FALSE;

   profile->himu.TS_SCSI.TP_SCSI_ID = scsiID;
   profile->himu.TS_SCSI.TP_SCSILun = SCSI_TRGTSH(targetTSH)->lunID;

   profile->himu.TS_SCSI.TP_SCSIMaxSpeed = hwInformation.targetInfo[scsiID].SCSIMaxSpeed;
   profile->himu.TS_SCSI.TP_SCSIDefaultSpeed = hwInformation.targetInfo[scsiID].SCSIDefaultSpeed;
   profile->himu.TS_SCSI.TP_SCSICurrentSpeed = hwInformation.targetInfo[scsiID].SCSICurrentSpeed;
   profile->himu.TS_SCSI.TP_SCSIDefaultOffset = hwInformation.targetInfo[scsiID].SCSIDefaultOffset;
   profile->himu.TS_SCSI.TP_SCSICurrentOffset = hwInformation.targetInfo[scsiID].SCSICurrentOffset;
   profile->himu.TS_SCSI.TP_SCSIMaxOffset = hwInformation.targetInfo[scsiID].SCSIMaxOffset;
   profile->himu.TS_SCSI.TP_SCSIMaxWidth = hwInformation.targetInfo[scsiID].SCSIMaxWidth;
   profile->himu.TS_SCSI.TP_SCSIDefaultWidth = hwInformation.targetInfo[scsiID].SCSIDefaultWidth;
   profile->himu.TS_SCSI.TP_SCSICurrentWidth = hwInformation.targetInfo[scsiID].SCSICurrentWidth;
         
   /* Retrieve device TransitionClocking capability */
   if (deviceTable->SCSI_DF_stcSupport && deviceTable->SCSI_DF_dtcSupport)
   {
      profile->himu.TS_SCSI.TP_SCSITransitionClocking = HIM_SCSI_ST_DT_CLOCKING;  /* both ST & DT */
   }
   else
   {
      if (!deviceTable->SCSI_DF_stcSupport && deviceTable->SCSI_DF_dtcSupport)
         profile->himu.TS_SCSI.TP_SCSITransitionClocking = HIM_SCSI_DT_CLOCKING;  /* DT only */
      else                                                                       
         profile->himu.TS_SCSI.TP_SCSITransitionClocking = HIM_SCSI_ST_CLOCKING;  /* ST only */
   }

   profile->himu.TS_SCSI.TP_SCSIDefaultProtocolOption =
      hwInformation.targetInfo[scsiID].SCSIDefaultProtocolOption;

   profile->himu.TS_SCSI.TP_SCSICurrentProtocolOption =
      hwInformation.targetInfo[scsiID].SCSICurrentProtocolOption;

   /* double check current neg. rates if SCSI_NEEDNEGO then return unknown */
   if (hwInformation.targetInfo[scsiID].xferOption  == SCSI_NEEDNEGO)
   {
      profile->himu.TS_SCSI.TP_SCSICurrentSpeed  = HIM_SCSI_SPEED_UNKNOWN;
      profile->himu.TS_SCSI.TP_SCSICurrentOffset = HIM_SCSI_OFFSET_UNKNOWN;
      profile->himu.TS_SCSI.TP_SCSICurrentWidth  = HIM_SCSI_WIDTH_UNKNOWN;
      profile->himu.TS_SCSI.TP_SCSICurrentProtocolOption = HIM_SCSI_PROTOCOL_OPTION_UNKNOWN;      
   }
   profile->himu.TS_SCSI.TP_SCSIDisconnectAllowed =
            (deviceTable->SCSI_DF_disconnectEnable) ? HIM_TRUE : HIM_FALSE;

#if SCSI_DOMAIN_VALIDATION
   if (adapterTSH->SCSI_AF_dvFallBack(scsiID))
   {
      profile->himu.TS_SCSI.TP_SCSIDomainValidationFallBack = HIM_TRUE;
   }
   else
   {
      profile->himu.TS_SCSI.TP_SCSIDomainValidationFallBack = HIM_FALSE;
   }
   profile->himu.TS_SCSI.TP_SCSIDomainValidationMethod = 
         adapterTSH->SCSI_AF_dvLevel(scsiID);
#else /* SCSI_DOMAIN_VALIDATION */
   profile->himu.TS_SCSI.TP_SCSIDomainValidationFallBack = HIM_FALSE;
   profile->himu.TS_SCSI.TP_SCSIDomainValidationMethod = SCSI_DV_DISABLE;
#endif /* SCSI_DOMAIN_VALIDATION */

   if(hhcb->SCSI_HF_expSupport)
   {
      profile->himu.TS_SCSI.TP_SCSIConnectedViaExpander = 
               (deviceTable->SCSI_DF_behindExp) ? HIM_TRUE : HIM_FALSE;
   }
   else
   {
      profile->himu.TS_SCSI.TP_SCSIConnectedViaExpander = HIM_FALSE;
   }

   return((HIM_UINT8)(HIM_SUCCESS));
#else
   return((HIM_UINT8)(HIM_FAILURE));

#endif /* SCSI_PROFILE_INFORMATION && SCSI_INITIATOR_OPERATION */   
}

/*********************************************************************
*
*  SCSIReportAdjustableTargetProfile
*
*     Get information about which target profile information are
*     adjustable
*
*  Return Value:  HIM_SUCCESS, normal return
*                 HIM_FAILURE, operation failure
*                  
*  Parameters:    target TSH
*                 profile adjust mask memory
*
*  Remarks:       
*                 
*********************************************************************/
HIM_UINT8 SCSIReportAdjustableTargetProfile (HIM_TASK_SET_HANDLE targetTSH,
                                    HIM_TARGET_PROFILE HIM_PTR profileMask)
{

#if SCSI_PROFILE_INFORMATION && SCSI_INITIATOR_OPERATION && !SCSI_DISABLE_ADJUST_TARGET_PROFILE

   /* copy the adjustable target profile */
   OSDmemcpy(profileMask,&SCSIAdjustableTargetProfile,sizeof(HIM_TARGET_PROFILE));
   
   return((HIM_UINT8)(HIM_SUCCESS));
#else
   return((HIM_UINT8)(HIM_FAILURE));

#endif /* SCSI_PROFILE_INFORMATION && SCSI_INITIATOR_OPERATION && !SCSI_DISABLE_ADJUST_TARGET_PROFILE */
}

/*********************************************************************
*
*  SCSIAdjustTargetProfile
*
*     Apply the adjusted adapter profile information
*
*  Return Value:  HIM_SUCCESS, normal return
*                 HIM_FAILURE, operation failure
*                 HIM_ILLEGAL_CHANGE, illegal change
*                 HIM_TARGET_NOT_IDLE, profile adjusted is not allowed
*                  
*  Parameters:    target TSH
*                 profile memory
*
*  Remarks:       The profile mask should be acquired before
*                 adjusting target profile.
*                 
*********************************************************************/
HIM_UINT8 SCSIAdjustTargetProfile (HIM_TASK_SET_HANDLE targetTSH,
                                   HIM_TARGET_PROFILE HIM_PTR profile)    
                                    
{
#if SCSI_PROFILE_INFORMATION && SCSI_INITIATOR_OPERATION && !SCSI_DISABLE_ADJUST_TARGET_PROFILE
   SCSI_ADAPTER_TSCB HIM_PTR adapterTSH =
               ((SCSI_TARGET_TSCB HIM_PTR)targetTSH)->adapterTSH;
   SCSI_HHCB HIM_PTR hhcb = (SCSI_HHCB HIM_PTR) &SCSI_ADPTSH(adapterTSH)->hhcb;
   SCSI_UEXACT8 scsiID = (SCSI_UEXACT8)SCSI_TRGTSH(targetTSH)->scsiID;
   SCSI_DEVICE SCSI_DPTR deviceTable = &SCSI_DEVICE_TABLE(hhcb)[scsiID];
#if SCSI_STANDARD64_MODE 
   SCSI_UEXACT8 disconnectDisable;
#endif
   SCSI_HW_INFORMATION hwInformation;
   HIM_UINT16 speed;
   HIM_UINT8 offset;
   HIM_UINT8 width;
   HIM_UINT8 scsiOptChanged;
   
   /* Return if initiator mode not enabled */
   if (!hhcb->SCSI_HF_initiatorMode)
   {
      return ((HIM_UINT8)(HIM_FAILURE));
   }  
 
   /* Return if the device's not idle */
   if (!SCSI_CHECK_DEVICE_IDLE(hhcb, scsiID))
   {
      return((HIM_UINT8)(HIM_TARGET_NOT_IDLE));
   }

   SCSI_GET_HW_INFORMATION(&hwInformation, hhcb);

   /* Get the current default width and offset from scsiOption */
   offset = hwInformation.targetInfo[scsiID].SCSIDefaultOffset;
   width = hwInformation.targetInfo[scsiID].SCSIDefaultWidth;

   /* Get the current default speed from scsiOption */
   speed = hwInformation.targetInfo[scsiID].SCSIDefaultSpeed;

   scsiOptChanged = 0;     /* Assume there is no change in scsiOption */

   /* We should not allow the OSM to modify the width */
   /* if device is not a wide device. */
   if (deviceTable->SCSI_DF_wideDevice)
   {
      /* Do nothing if the width is the same */
      if (profile->himu.TS_SCSI.TP_SCSIDefaultWidth != width)
      {
         scsiOptChanged = 1;  /* Indicate force negotiation is needed */
         
         if (profile->himu.TS_SCSI.TP_SCSIDefaultWidth >= 16)
         {
            width = 16;
            deviceTable->SCSI_DF_setForWide = 1;  /* Set setForWide flag */
         }
         else
         {
            width = 8;          /* Assume target is running in narrow mode */
            deviceTable->SCSI_DF_setForWide = 0;  /* Clear setForWide flag */

            /* If the device's running in wide mode, we need to clear       */
            /* the setForWide flag and set the width to 16 so that the wide */
            /* xfer bit will be set when the new scsiOption get calucated.  */
            /* This way when the next IOB for this target start execute,    */
            /* CHIM will intiate wide negotiation. By then, it will clear   */
            /* the wide xfer bit because of the setForWide was clear.       */
            if (hwInformation.targetInfo[scsiID].xferOption & SCSI_WIDEXFER)
            {
               width = 16;
            }
         }
      }
   }

   /* We should not allow the OSM to modify speed or offset */
   /* of a device if it does not support synchronous xfer. */
   if (deviceTable->SCSI_DF_syncDevice)
   {
      /* Do nothing if the speed and offset are the same */
      if ((profile->himu.TS_SCSI.TP_SCSIDefaultSpeed != speed) ||
          (profile->himu.TS_SCSI.TP_SCSIDefaultOffset != offset))
      {
         speed = profile->himu.TS_SCSI.TP_SCSIDefaultSpeed;
         offset = profile->himu.TS_SCSI.TP_SCSIDefaultOffset;
         scsiOptChanged = 1;  /* Indicate force negotiation is needed */

         /* If user wants a non-zero offset and speed above async range, */
         /* this means do sync neg. with hardware default offset */
         if ((profile->himu.TS_SCSI.TP_SCSIDefaultOffset) &&
             (profile->himu.TS_SCSI.TP_SCSIDefaultSpeed >=
                hwInformation.minimumSyncSpeed))
         {
            deviceTable->SCSI_DF_setForSync = 1;   /* Set setForSync flag */
         }
         else
         {
            deviceTable->SCSI_DF_setForSync = 0;   /* Clear setForSync flag */
            /* Since the default offset is zero and/or speed is less than   */
            /* 36, which means do async. xfer, we need to clear the         */
            /* setForSync flag and set the offset to the current offset.    */
            /* This way when the next IOB for this target start execute,    */
            /* CHIM will intiate sync. negotiation if the current offset is */
            /* nonzero. By then, it will set the offset to zero because of  */
            /* the setForSync was clear.                                    */
            offset = hwInformation.targetInfo[scsiID].SCSICurrentOffset;
         }
      }
   }  /* end of if syncDevice */

   /* If the scsiOption has been changed then we should force negotiation. */
   if (scsiOptChanged)
   {
      /* Put the changed xfer options back into the hwInformation structure */
      /* and indicate the change to the hardware layer (using the           */
      /* scsiOptChanged flag).                                              */
      hwInformation.targetInfo[scsiID].SCSI_TF_scsiOptChanged = 1;
      hwInformation.targetInfo[scsiID].SCSIDefaultSpeed = (SCSI_UEXACT16)speed;
      hwInformation.targetInfo[scsiID].SCSIDefaultOffset = (SCSI_UEXACT8)offset;
      hwInformation.targetInfo[scsiID].SCSIDefaultWidth = (SCSI_UEXACT8)width;
   }

#if SCSI_NULL_SELECTION
   if (hwInformation.hostScsiID != scsiID)
#endif /* SCSI_NULL_SELECTION */
   {
      /* If the target SCSI ID = the host adapter SCSI ID then this */
      /* must be a target TSCB for a "NULL selection device therefore */
      /* don't allow disconnect allowed to be set. */      
      deviceTable->SCSI_DF_disconnectEnable = 
            (profile->himu.TS_SCSI.TP_SCSIDisconnectAllowed == HIM_TRUE) ? 1 : 0;
   }

   /* The target's tagEnable flag is on / off will depend on the OSM set */
   /* TP_TaggedQueueing field to TRUE / FALSE (assume target supports tag). */
   if (profile->TP_TaggedQueuing == HIM_TRUE)
   {
      /* The target's tagEnable flag is further qualified with the */
      /* disconnectEnable flag is on / off and the target itself */
      /* supports tag or not. */
      SCSI_TRGTSH(targetTSH)->targetAttributes.tagEnable =
                  (((deviceTable->SCSI_DF_disconnectEnable << scsiID) &
                   SCSI_ADPTSH(adapterTSH)->tagEnable) != 0);
   }
   else
   {
      /* OSM request tag queueing is disable */
      SCSI_TRGTSH(targetTSH)->targetAttributes.tagEnable = 0;
   }
         
   /* Update disconnect option in scratch RAM for standard 64 */
#if SCSI_STANDARD64_MODE 
   if (SCSI_hDISCON_OPTION(hhcb) != SCSI_NULL_ENTRY)
   {
      if (scsiID < 8)
      {
         SCSIHReadScratchRam(hhcb, &disconnectDisable,
                             SCSI_hDISCON_OPTION(hhcb) - SCSI_SCRATCH0, 1);
         disconnectDisable |= (SCSI_UEXACT8) (1 << scsiID);
         disconnectDisable &= ~((SCSI_UEXACT8)
                   (deviceTable->SCSI_DF_disconnectEnable << scsiID));
         SCSIHWriteScratchRam(hhcb, SCSI_hDISCON_OPTION(hhcb) - SCSI_SCRATCH0,
                              1, &disconnectDisable);
      }
      else
      {
         SCSIHReadScratchRam(hhcb, &disconnectDisable,
                             (SCSI_hDISCON_OPTION(hhcb) - SCSI_SCRATCH0)+1, 1);
         disconnectDisable |= (SCSI_UEXACT8) (1 << (scsiID - 8));
         disconnectDisable &= ~((SCSI_UEXACT8)
                   (deviceTable->SCSI_DF_disconnectEnable << (scsiID - 8)));
         SCSIHWriteScratchRam(hhcb, (SCSI_hDISCON_OPTION(hhcb) - SCSI_SCRATCH0)+1,
                              1, &disconnectDisable);
      }
   }
#endif

   /* MaxActiveCommands calculated from hwInformation structure which */
   /* gets the parameter from the unit control structure internally. */
   /* The field used is maxTagScbs since it is not possible to know */
   /* tagged or nontagged since tagged is set on an IOB basis. */
   hwInformation.targetInfo[scsiID].maxTagScbs = 
          (SCSI_UEXACT8)profile->TP_MaxActiveCommands;
   
   SCSI_PUT_HW_INFORMATION(&hwInformation, hhcb);

   return((HIM_UINT8)(HIM_SUCCESS));
#else

   return((HIM_UINT8)(HIM_FAILURE));

#endif /* SCSI_PROFILE_INFORMATION && SCSI_INITIATOR_OPERATION && !SCSI_DISABLE_ADJUST_TARGET_PROFILE */
}

/*********************************************************************
*
*  SCSIGetNVSize
*
*     Get size of NVRAM associated with the specified adapter TSH
*
*  Return Value:  size of NVRAM associated
*                  
*  Parameters:    adapter TSH
*
*  Remarks:
*                 
*********************************************************************/
HIM_UINT32 SCSIGetNVSize (HIM_TASK_SET_HANDLE adapterTSH)
{
   SCSI_UINT32 size = 0;
#if SCSI_SEEPROM_ACCESS
   SCSI_INT seepromSize;
   SCSI_HHCB HIM_PTR hhcb = (SCSI_HHCB HIM_PTR) &SCSI_ADPTSH(adapterTSH)->hhcb;
#endif /* SCSI_SEEPROM_ACCESS */
#if !SCSI_SEEPROM_ACCESS
      size = OSMxGetNVSize(SCSI_ADPTSH(adapterTSH)->osmAdapterContext);
#else

   if (!hhcb->SCSI_HF_OverrideOSMNVRAMRoutines)

      size = OSMxGetNVSize(SCSI_ADPTSH(adapterTSH)->osmAdapterContext);

   if (size == 0)
   {
      /* OSM NVM not available */
      /* read from SEEPROM attached to host device */
      
      /* seepromSize is in word */
      seepromSize = SCSI_SIZE_SEEPROM(hhcb);

      switch(hhcb->SCSI_SUBID_scsiChannels)
      {
         case SCSI_ONE_CHNL:
            size = seepromSize * 2;
            return (size);

         case SCSI_TWO_CHNL:
            size = (seepromSize * 4);
            return (size);

         case SCSI_THREE_CHNL:
            size = (seepromSize * 6);
            return (size);

         case SCSI_FOUR_CHNL:
            size = (seepromSize * 8);
            return (size);
      }

   }

   if ((size == 0) && (hhcb->SCSI_HF_OverrideOSMNVRAMRoutines))
      size = OSMxGetNVSize(SCSI_ADPTSH(adapterTSH)->osmAdapterContext);
   
#endif /* SCSI_SEEPROM_ACCESS */

   return(size);
   
}

/*********************************************************************
*
*  SCSIGetNVOSMSegment
*
*     Get segment information for the NVRAM controlled by OSM. 
*
*  Return Value:  HIM_SUCCESS, normal completion
*                 HIM_FAILURE, hardware or other failure
*                 HIM_NO_OSM_SEGMENT, no OSM segment available
*                  
*  Parameters:    adapter TSH
*                 osmOffset
*                 osmCount
*
*  Remarks:
*                 
*********************************************************************/
HIM_UINT8 SCSIGetNVOSMSegment (HIM_TASK_SET_HANDLE adapterTSH, 
                               HIM_UINT32 HIM_PTR osmOffset, 
                               HIM_UINT32 HIM_PTR osmCount)
{
#if SCSI_SEEPROM_ACCESS
   SCSI_INT seepromSize;
   SCSI_HHCB HIM_PTR hhcb = (SCSI_HHCB HIM_PTR) &SCSI_ADPTSH(adapterTSH)->hhcb;
   SCSI_NVM_LAYOUT nvmData;
   SCSI_UINT16 seepromBase;
#endif /* SCSI_SEEPROM_ACCESS */
#if SCSI_SEEPROM_ACCESS

      /* OSM NVM not available */
      /* read from SEEPROM attached to host device */

      /* figure out the section base within the seeprom */
      seepromBase = 0;
      switch(hhcb->SCSI_SUBID_scsiChannels)
      {
         case SCSI_TWO_CHNL:
            if (hhcb->SCSI_SUBID_multifunction == 1)
            {
               if (SCSI_ADPTSH(adapterTSH)->hostAddress.pciAddress.functionNumber != 0)
               {
                  seepromBase = sizeof(SCSI_NVM_LAYOUT) / 2;
               }
            }
            else
            {
               if (SCSI_ADPTSH(adapterTSH)->hostAddress.pciAddress.deviceNumber != 4)
               {
                  seepromBase = sizeof(SCSI_NVM_LAYOUT) / 2;
               }
            }
            break;

         case SCSI_THREE_CHNL:
            switch (SCSI_ADPTSH(adapterTSH)->hostAddress.pciAddress.deviceNumber)
            {
               case 1:
               case 4:
                  /* based at the first section */
                  break;

               case 2:
               case 8:
                  /* based at the second section */
                  seepromBase = sizeof(SCSI_NVM_LAYOUT) / 2;
                  break;

               case 3:
               case 12:
                  /* based at the third section */
                  seepromBase = sizeof(SCSI_NVM_LAYOUT) / 2 * 2;
                  break;
            }
            break;

         case SCSI_FOUR_CHNL:
            seepromBase = sizeof(SCSI_NVM_LAYOUT) / 2 *
               (SCSI_ADPTSH(adapterTSH)->hostAddress.pciAddress.deviceNumber - 4);
            break;
      }

      /* read from seeprom */
      if (SCSIHReadSEEPROM(hhcb,(SCSI_UEXACT8 SCSI_SPTR) &nvmData,
         (SCSI_SINT16) seepromBase,(SCSI_SINT16) sizeof(SCSI_NVM_LAYOUT)/2))
      {
         /* seeprom not available */
         return((HIM_UINT8)(HIM_NO_OSM_SEGMENT)); 
      }

      /* seepromSize is in word */
      seepromSize = SCSI_SIZE_SEEPROM(hhcb);

#if SCSI_SCSISELECT_SUPPORT
      switch(hhcb->SCSI_SUBID_scsiChannels)
      {
         case SCSI_ONE_CHNL:
            *osmOffset = sizeof(SCSI_NVM_LAYOUT);
            *osmCount = (seepromSize * 2) /2;
            return((HIM_UINT8)(HIM_SUCCESS));

         case SCSI_TWO_CHNL:
            *osmOffset = 0;
            *osmCount = 0;
            return((HIM_UINT8)(HIM_NO_OSM_SEGMENT));

         case SCSI_THREE_CHNL:
            switch (SCSI_ADPTSH(adapterTSH)->hostAddress.pciAddress.deviceNumber)
            {
               case 1:
               case 4:
                  /* based at the first section */
                  *osmOffset = sizeof(SCSI_NVM_LAYOUT) * 3;
                  *osmCount = sizeof(SCSI_NVM_LAYOUT) /3;
                  return((HIM_UINT8)(HIM_SUCCESS));

               case 2:
               case 8:
                  /* based at the second section */
                  *osmOffset = sizeof(SCSI_NVM_LAYOUT) * 3 + sizeof(SCSI_NVM_LAYOUT) /3;
                  *osmCount = sizeof(SCSI_NVM_LAYOUT) /3;
                  return((HIM_UINT8)(HIM_SUCCESS));

               case 3:
               case 12:
                  /* based at the third section */
                  *osmOffset = sizeof(SCSI_NVM_LAYOUT) * 3 + (sizeof(SCSI_NVM_LAYOUT) /3) * 2;
                  *osmCount = sizeof(SCSI_NVM_LAYOUT) /3;
                  return((HIM_UINT8)(HIM_SUCCESS));
            }

         case SCSI_FOUR_CHNL:
            *osmOffset = 0;
            *osmCount = 0;
            return((HIM_UINT8)(HIM_NO_OSM_SEGMENT));

         default:
            /* should never come here - this will prevent a compile warning */
            *osmOffset = 0;
            *osmCount = 0;
            return((HIM_UINT8)(HIM_NO_OSM_SEGMENT));
      }

#else /* !SCSI_SCSISELECT_SUPPORT */
      switch(hhcb->SCSI_SUBID_scsiChannels)
      {
         case SCSI_ONE_CHNL:
            *osmOffset = seepromBase * 2;
            *osmCount = seepromSize * 2;
            return((HIM_UINT8)(HIM_SUCCESS));

         case SCSI_TWO_CHNL:
            *osmOffset = seepromBase * 2;
            *osmCount = (seepromSize * 2) /2;
            return((HIM_UINT8)(HIM_SUCCESS));

         case SCSI_THREE_CHNL:
            *osmOffset = seepromBase * 2 + (seepromBase * 2) /3;
            *osmCount = (seepromSize * 2) /3;
            return((HIM_UINT8)(HIM_SUCCESS));

         case SCSI_FOUR_CHNL:
            *osmOffset = seepromBase * 2;
            *osmCount = (seepromSize * 2) /4;
            return((HIM_UINT8)(HIM_SUCCESS));

         default:
            /* should never come here - this will prevent a compile warning */
            *osmOffset = 0;
            *osmCount = 0;
            return((HIM_UINT8)(HIM_NO_OSM_SEGMENT));
      }

#endif /* SCSI_SCSISELECT_SUPPORT */


#else /* !SCSI_SEEPROM_ACCESS */
   
   return((HIM_UINT8)(HIM_NO_OSM_SEGMENT));
#endif /* SCSI_SEEPROM_ACCESS */
}

/*********************************************************************
*
*  SCSIPutNVData
*
*     Write to NVRAM
*
*  Return Value:  HIM_SUCCESS, write successfully
*                 HIM_FAILURE, write failed
*                 HIM_WRITE_NOT_SUPPORTED, write to NVRAM not supported
*                 HIM_WRITE_PROTECTED, NVRAM is write-protected
*                  
*  Parameters:    adapter TSH
*                 destination NVRAM offset
*                 source of memory buffer
*                 length to be written to NVRAM
*
*  Remarks:       
*                 
*********************************************************************/
HIM_UINT8 SCSIPutNVData (HIM_TASK_SET_HANDLE adapterTSH,
                         HIM_UINT32 destinationOffset,
                         void HIM_PTR source,
                         HIM_UINT32 length) 
{
   HIM_UINT32 OSMReturn;
#if SCSI_SEEPROM_ACCESS
   SCSI_HHCB HIM_PTR hhcb = (SCSI_HHCB HIM_PTR) &SCSI_ADPTSH(adapterTSH)->hhcb;
#endif

#if !SCSI_SEEPROM_ACCESS
   
      OSMReturn = OSMxPutNVData(SCSI_ADPTSH(adapterTSH)->osmAdapterContext,destinationOffset,source,length);
      return((HIM_UINT8) OSMReturn);
#else   
   if (!hhcb->SCSI_HF_OverrideOSMNVRAMRoutines)
   
   {
   
      OSMReturn = OSMxPutNVData(SCSI_ADPTSH(adapterTSH)->osmAdapterContext,destinationOffset,source,length);

      if (OSMReturn != HIM_WRITE_NOT_SUPPORTED)
      {
         return((HIM_UINT8) OSMReturn);
      }
   
   }
   
   if (SCSIHWriteSEEPROM(hhcb,(SCSI_SINT16) (destinationOffset/2),
      (SCSI_SINT16) (length/2),(SCSI_UEXACT8 SCSI_SPTR) source) == 0)
   {
      return((HIM_UINT8)(HIM_SUCCESS)); 
   }
   else
   {
   
      if (!hhcb->SCSI_HF_OverrideOSMNVRAMRoutines)
   
         return((HIM_UINT8)(HIM_FAILURE)); 
      else
     {

         OSMReturn = OSMxPutNVData(SCSI_ADPTSH(adapterTSH)->osmAdapterContext,destinationOffset,source,length);
         return((HIM_UINT8) OSMReturn);
   
     }
   }



#endif /* SCSI_SEEPROM_ACCESS */

}

/*********************************************************************
*
*  SCSIGetNVData
*
*     Retrieve data from NVRAM
*
*  Return Value:  HIM_SUCCESS, NVRAM retrieval succussful
*                 HIM_FAILURE, NVRAM retrieval failed
*                  
*  Parameters:    adapter TSH
*                 destination buffer
*                 source offset of NVRAM
*                 length to be read from NVRAM
*
*  Remarks:       
*                 
*********************************************************************/
HIM_UINT8 SCSIGetNVData (HIM_TASK_SET_HANDLE adapterTSH,
                         void HIM_PTR destination,
                         HIM_UINT32 sourceOffset,
                         HIM_UINT32 length)
{
   HIM_UINT32 OSMReturn;

#if SCSI_SEEPROM_ACCESS

   SCSI_HHCB HIM_PTR hhcb = (SCSI_HHCB HIM_PTR) &SCSI_ADPTSH(adapterTSH)->hhcb;
#endif

#if !SCSI_SEEPROM_ACCESS

      OSMReturn = OSMxGetNVData(SCSI_ADPTSH(adapterTSH)->osmAdapterContext,destination,sourceOffset,length);
      return((HIM_UINT8) OSMReturn); 
#else

   if (!hhcb->SCSI_HF_OverrideOSMNVRAMRoutines)
   {
      OSMReturn = OSMxGetNVData(SCSI_ADPTSH(adapterTSH)->osmAdapterContext,destination,sourceOffset,length);

      if (OSMReturn == HIM_SUCCESS)
      {
         return((HIM_UINT8)(HIM_SUCCESS)); 
      }

   }

   if (SCSIHReadSEEPROM(hhcb,(SCSI_UEXACT8 SCSI_SPTR) destination,
         (SCSI_SINT16) (sourceOffset/2),(SCSI_SINT16) (length/2)) == 0)
   {
      return((HIM_UINT8)(HIM_SUCCESS)); 
   }
   else
   {
   
      if (!hhcb->SCSI_HF_OverrideOSMNVRAMRoutines)
         return((HIM_UINT8)(HIM_FAILURE));
      else
      {
         OSMReturn = OSMxGetNVData(SCSI_ADPTSH(adapterTSH)->osmAdapterContext,destination,sourceOffset,length);
         return((HIM_UINT8)OSMReturn); 
    
      }
   }


#endif /* SCSI_SEEPROM_ACCESS */


}

/*********************************************************************
*
*  SCSIProfileNode
*
*     Get node profile information
*
*  Return Value:  HIM_SUCCESS, normal return
*                 HIM_FAILURE, target mode was not enabled
*                  
*  Parameters:    node TSH
*                 profile memory
*
*  Remarks:       Some of the profile information are transport
*                 specific and OSM should either avoid or isolate
*                 accessing profile information if possible.
*                 Pretty much CHIM will have to maintain the
*                 profile information itself.
*                 
*********************************************************************/
HIM_UINT8 SCSIProfileNode (HIM_TASK_SET_HANDLE nodeTSH,
                           HIM_NODE_PROFILE HIM_PTR profile)         
{
#if SCSI_TARGET_OPERATION && SCSI_PROFILE_INFORMATION
   SCSI_NODE SCSI_NPTR node = SCSI_NODETSH(nodeTSH);
   SCSI_HHCB HIM_PTR hhcb = (SCSI_HHCB HIM_PTR)node->hhcb;
   SCSI_UEXACT8 scsiID = (SCSI_UEXACT8)node->scsiID;
   SCSI_HW_INFORMATION hwInformation;

   /* Check target mode enabled */
   if (!hhcb->SCSI_HF_targetMode)
   {
      return ((HIM_UINT8)(HIM_FAILURE));
   }
   profile->NP_Version = HIM_VERSION_NODE_PROFILE;
   profile->NP_Transport = HIM_TRANSPORT_SCSI;
   profile->NP_Protocol = HIM_PROTOCOL_SCSI;
   profile->NP_BusNumber = 0;
   
   profile->himu.TS_SCSI.NP_SCSI_ID = node->scsiID;
   
   /* Max speed etc is obtained from adapter information */

   SCSI_GET_HW_INFORMATION(&hwInformation, hhcb);
      
   profile->himu.TS_SCSI.NP_SCSIMaxSpeed =
      hwInformation.targetInfo[scsiID].SCSIMaxSpeed;
   profile->himu.TS_SCSI.NP_SCSIDefaultSpeed = 
      hwInformation.targetInfo[scsiID].SCSIDefaultSpeed;
   profile->himu.TS_SCSI.NP_SCSICurrentSpeed = 
       hwInformation.targetInfo[scsiID].SCSICurrentSpeed;
   
   profile->himu.TS_SCSI.NP_SCSIMaxOffset =
       hwInformation.targetInfo[scsiID].SCSIMaxOffset;
   profile->himu.TS_SCSI.NP_SCSIDefaultOffset = 
       hwInformation.targetInfo[scsiID].SCSIDefaultOffset;
   profile->himu.TS_SCSI.NP_SCSICurrentOffset = 
       hwInformation.targetInfo[scsiID].SCSICurrentOffset; 
   
   profile->himu.TS_SCSI.NP_SCSIMaxWidth = 
      hwInformation.targetInfo[scsiID].SCSIMaxWidth;
   profile->himu.TS_SCSI.NP_SCSIDefaultWidth =
        hwInformation.targetInfo[scsiID].SCSIDefaultWidth;
   profile->himu.TS_SCSI.NP_SCSICurrentWidth =
        hwInformation.targetInfo[scsiID].SCSICurrentWidth;
  
   /* double check current neg. rates if SCSI_NEEDNEGO then return unknown */
   /* should probably return narrow, async etc */
   if (hwInformation.targetInfo[scsiID].xferOption  == SCSI_NEEDNEGO)
   {
      profile->himu.TS_SCSI.NP_SCSICurrentSpeed = HIM_SCSI_SPEED_UNKNOWN;
      profile->himu.TS_SCSI.NP_SCSICurrentOffset = HIM_SCSI_OFFSET_UNKNOWN;
      profile->himu.TS_SCSI.NP_SCSICurrentWidth = HIM_SCSI_WIDTH_UNKNOWN;
   }

   return((HIM_UINT8)(HIM_SUCCESS));
#else

   return ((HIM_UINT8)(HIM_FAILURE)); 
#endif /* SCSI_TARGET_OPERATION && SCSI_PROFILE_INFORMATION */
   
}

/*********************************************************************
*
*  SCSIReportAdjustableNodeProfile
*
*     Get information about which node profile information are
*     adjustable
*
*  Return Value:  HIM_SUCCESS, normal return
*                 HIM_FAILURE, target mode was not enabled
*                  
*  Parameters:    node TSH
*                 profile adjust mask memory
*
*  Remarks:       This is for target mode operation only
*                 
*********************************************************************/
HIM_UINT8 SCSIReportAdjustableNodeProfile (HIM_TASK_SET_HANDLE nodeTSH,
                                     HIM_NODE_PROFILE HIM_PTR profileMask)
{
#if SCSI_TARGET_OPERATION && SCSI_PROFILE_INFORMATION
   /* copy the adjustable node profile */
   OSDmemcpy(profileMask,&SCSIAdjustableNodeProfile,sizeof(HIM_NODE_PROFILE));
  
   return((HIM_UINT8)(HIM_SUCCESS));
#else
   return((HIM_UINT8)(HIM_FAILURE));
#endif /* SCSI_TARGET_OPERATION && SCSI_PROFILE_OPERATION */
}

/*********************************************************************
*
*  SCSIAdjustNodeProfile
*
*     Apply the adjusted node profile information
*
*  Return Value:  HIM_SUCCESS, normal return
*                 HIM_FAILURE, operation failure
*                 HIM_ILLEGAL_CHANGE, illegal change
*                 HIM_NODE_NOT_IDLE, node TSH is not idle
*                  
*  Parameters:    node TSH
*                 profile memory
*
*  Remarks:       This is for target mode operation only
*                 
*********************************************************************/
HIM_UINT8 SCSIAdjustNodeProfile (HIM_TASK_SET_HANDLE nodeTSH,
                                 HIM_NODE_PROFILE HIM_PTR profile)

{
#if SCSI_TARGET_OPERATION && SCSI_PROFILE_INFORMATION   
    /* Currently none of the profile fields are adjustable */
   return((HIM_UINT8)(HIM_ILLEGAL_CHANGE));

#else
   return((HIM_UINT8)(HIM_FAILURE));   
#endif /* SCSI_TARGET_OPERATION && SCSI_PROFILE_INFORMATION */

}

/*********************************************************************
*
*  SCSISetOSMNodeContext
*
*     Set OSM context for a node TSH
*
*  Return Value:  HIM_SUCCESS, normal return
*                 HIM_FAILURE, target mode was not enabled
*                  
*  Parameters:    node TSH
*                 OSM context
*  Remarks:       The osmContext is the OSM's handle to the same node
*                 as nodeTSH. It typically points to an OSM structure 
*                 associated with this node. This osmContext handle is 
*                 passed to the OSM via the nexus profile. It is possible
*                 that OSM processing of requests is dependent on the state 
*                 of the node (initiator). 
*                 
*********************************************************************/
HIM_UINT8 SCSISetOSMNodeContext(HIM_TASK_SET_HANDLE nodeTSH,
                                 void HIM_PTR osmContext)    
                                    
{
#if SCSI_TARGET_OPERATION 

   SCSI_NODE SCSI_NPTR node = SCSI_NODETSH(nodeTSH);
   SCSI_HHCB SCSI_HPTR hhcb = node->hhcb;

   /* Check if target mode enabled */
   if (!hhcb->SCSI_HF_targetMode)
   {
      return ((HIM_UINT8)(HIM_FAILURE));
   }
   node->osmContext = osmContext;
   return((HIM_UINT8)(HIM_SUCCESS));
#else
   return((HIM_UINT8)(HIM_FAILURE));
#endif /* SCSI_TARGET_OPERATION */      
   
} 

/*********************************************************************
*
*  SCSIProfileNexus
*
*     Get nexus profile information
*
*  Return Value:  HIM_SUCCESS, normal return
*                 HIM_FAILURE, target mode is not enabled
*                  
*  Parameters:    nexus TSH
*                 profile memory
*
*  Remarks:       Some of the profile information are transport
*                 specific and OSM should either avoid or isolate
*                 accessing profile information if possible.
*                 Pretty much CHIM will have to maintain the
*                 profile information itself.
*                 
*********************************************************************/
HIM_UINT8 SCSIProfileNexus(HIM_TASK_SET_HANDLE nexusTSH,
                           HIM_NEXUS_PROFILE HIM_PTR profile)
{
#if SCSI_TARGET_OPERATION
   SCSI_NEXUS SCSI_XPTR nexus = SCSI_NEXUSTSH(nexusTSH); 
   SCSI_HHCB SCSI_HPTR hhcb = nexus->hhcb; 
#if !SCSI_ESS_SUPPORT
   SCSI_NODE SCSI_NPTR node = nexus->node;
#endif /* SCSI_ESS_SUPPORT */
   
   /* Check if target mode enabled */
   if (!hhcb->SCSI_HF_targetMode)
   {
      return ((HIM_UINT8)(HIM_FAILURE));
   }

#if !SCSI_ESS_SUPPORT
   /* ESS does not require these fields */
   profile->XP_Version = HIM_VERSION_NEXUS_PROFILE;
   profile->XP_Transport = HIM_TRANSPORT_SCSI;
   profile->XP_Protocol = HIM_PROTOCOL_SCSI;
   profile->XP_BusNumber = 0;
   profile->XP_NodeTSH = node;
   profile->XP_OSMNodeContext = node->osmContext;
#endif /* !SCSI_ESS_SUPPORT */

   profile->XP_AdapterTSH = SCSI_GETADP(hhcb);    
   profile->XP_LastResource = (nexus->SCSI_XF_lastResource) ? HIM_TRUE : HIM_FALSE;
   profile->himu.TS_SCSI.XP_SCSI_ID = nexus->scsiID;
   profile->himu.TS_SCSI.XP_SCSILun = nexus->lunID; 
   if (nexus->SCSI_XF_tagRequest)
   {/* Tagged queuing */
      switch (nexus->queueType)   
     {
         case SCSI_SIMPLE_QUEUE_TAG:
            profile->himu.TS_SCSI.XP_SCSIQueueType = HIM_TASK_SIMPLE;
            break; 

         case SCSI_HEAD_OF_QUEUE_TAG:
            profile->himu.TS_SCSI.XP_SCSIQueueType = HIM_TASK_HEAD_OF_QUEUE;
            break;

         case SCSI_ORDERED_QUEUE_TAG:
            profile->himu.TS_SCSI.XP_SCSIQueueType = HIM_TASK_ORDERED;
            break;

         case SCSI_ACA_QUEUE_TAG:
            profile->himu.TS_SCSI.XP_SCSIQueueType = HIM_TASK_ACA;
            break;

         default:
            profile->himu.TS_SCSI.XP_SCSIQueueType = HIM_TASK_UNKNOWN;
            break;
      }
 
      profile->himu.TS_SCSI.XP_SCSIQueueTag = nexus->queueTag;
   }           
   else
   {/* No tag */
      profile->himu.TS_SCSI.XP_SCSIQueueType = HIM_TASK_NO_TAG;
   }

   profile->himu.TS_SCSI.XP_SCSILunTar = (nexus->SCSI_XF_lunTar) ? HIM_TRUE : HIM_FALSE;

#if !SCSI_ESS_SUPPORT
   profile->himu.TS_SCSI.XP_SCSIBusHeld = (nexus->SCSI_XF_busHeld) ? HIM_TRUE : HIM_FALSE;
   profile->himu.TS_SCSI.XP_SCSI1Selection = (nexus->SCSI_XF_scsi1Selection) ? HIM_TRUE : HIM_FALSE;   
   profile->himu.TS_SCSI.XP_SCSIDisconnectAllowed = (nexus->SCSI_XF_disconnectAllowed) ? HIM_TRUE : HIM_FALSE;
#endif /* ! SCSI_ESS_SUPPORT */
#if SCSI_MULTIPLEID_SUPPORT
   profile->himu.TS_SCSI.XP_SCSISelectedID = nexus->selectedID;  
#endif /* SCSI_MULTIPLEID_SUPPORT */

   return((HIM_UINT8)(HIM_SUCCESS)); 
#else
   return((HIM_UINT8)(HIM_FAILURE));
#endif /* SCSI_TARGET_OPERATION */   
}

/*********************************************************************
*
*  SCSIClearNexusTSH
*
*     Invalidate nexus TSH and return to available Nexus pool 
*
*  Return Value:  HIM_SUCCESS, nexus TSH is cleared and the associated TSCB 
*                 memory is returned to nexus available pool.
*
*                 HIM_NEXUS_IOBUS_HELD, nexus TSH cannot be cleared due to
*                 the I/O bus being held by this nexus. OSM must issue a
*                 HIM_CLEAR_NEXUS or HIM_REESTABLISH_INTERMEDIATE IOB function
*                 with no data to release the bus.  
*
*                 HIM_NEXUS_NOT_IDLE, nexus TSH cannot be cleared due to 
*                 pending IOBs for this nexus. OSM must issue a HIM_CLEAR_NEXUS
*                 IOB function or await completion of pending IOB(s).
*
*                 HIM_FAILURE, some undefined error occured or 
*                              target mode not enabled.  
*
*                 Other non-zero return status values may be 
*                 added later for future expansion.
*                  
*  Parameters:    nexus TSH
*
*  Remarks:       Whenever the OSM has finished with a particular nexus handle,
*                 it can invalidate the handle by calling this routine. After
*                 the handle is invalidated, the HIM returns the handle and
*                 it's storage space to the pool of available nexus handles.
*                 If the OSM calls this routine while the HIM is processing
*                 an IOB for this nexus OR if the I/O bus is held for this nexus,
*                 the call is rejected.
*
*                 It is not an error to invoke HIMClearNexusTSH for a nexus
*                 which has been returned to the available nexus pool, if and only if, 
*                 the call which release the nexus and subsequent calls are made
*                 within the same HIMBackEndISR context or while selection (IN)
*                 is disabled.
*
*********************************************************************/
HIM_UINT8 SCSIClearNexusTSH (HIM_TASK_SET_HANDLE nexusTSH)
{
#if SCSI_TARGET_OPERATION   
   SCSI_NEXUS HIM_PTR nexus = SCSI_NEXUSTSH(nexusTSH);
   SCSI_HHCB SCSI_XPTR hhcb = nexus->hhcb;
   HIM_UINT8 result;
   
   /* Check if target mode enabled */
   if (!hhcb->SCSI_HF_targetMode)
   {
      return ((HIM_UINT8)(HIM_FAILURE));
   } 

   result = (HIM_UINT8)SCSI_CLEAR_NEXUS(nexus,hhcb);
   if (!result)
   {
      return((HIM_UINT8)(HIM_SUCCESS));
   }
   else if (result == 1)
   {
      return((HIM_UINT8)(HIM_NEXUS_HOLDING_IOBUS));
   }
   else if (result == 2)
   {
      return((HIM_UINT8)(HIM_NEXUS_NOT_IDLE));     
   }
   else
   {         
      return ((HIM_UINT8)(HIM_FAILURE));
   }
   return((HIM_UINT8)(HIM_SUCCESS));
#else
   return((HIM_UINT8)(HIM_FAILURE));
#endif /* SCSI_TARGET_OPERATION */

}

/*********************************************************************
*
*  SCSIQueueIOB
*
*     Queue the IOB for execution
*
*  Return Value:  void
*                  
*  Parameters:    adapter TSH
*
*  Remarks:       It's OSM's decision to execute this routine at
*                 interrupt context or not.
*
*********************************************************************/
#if SCSI_STREAMLINE_QIOPATH
void SCSIQueueIOB (HIM_IOB HIM_PTR iob)
{
   HIM_TASK_SET_HANDLE taskSetHandle = iob->taskSetHandle;
   SCSI_HIOB HIM_PTR hiob = &SCSI_IOBRSV(iob)->hiob;
   HIM_TS_SCSI HIM_PTR transportSpecific = (HIM_TS_SCSI HIM_PTR)iob->transportSpecific;
#if SCSI_TARGET_OPERATION 
   SCSI_UEXACT8 HIM_PTR scsiStatus;
#endif /* SCSI_TARGET_OPERATION */

/************************************************
   new locals start 
*************************************************/
   SCSI_HHCB SCSI_HPTR hhcb;  /* SCSIRQueueHIOB,SCSIHQueueHIOB,SCSIrAllocateScb */
                              /* SCSIrGetScb, SCSIDeliverScb */
   SCSI_TARGET_CONTROL SCSI_HPTR targetControl; /* SCSIrCheckCriteria */
   SCSI_UINT16 maxScbs;  /* SCSIrCheckCriteria */
   register SCSI_REGISTER scsiRegister; /* SCSIHQueueHIOB,SCSIhDeliverScb */

/************************************************
   new locals end 
*************************************************/

   /* setup pointer to iob for translation from hiob to iob */
   /* it will be referenced when building SCB and posting to OSM */
   SCSI_IOBRSV(iob)->iob = iob;
                               
   if (iob->function == HIM_INITIATE_TASK)
   {
      /* translate iob into hiob */
      hiob->unitHandle.targetUnit = &SCSI_TRGTSH(taskSetHandle)->unitControl;

      hiob->cmd = SCSI_CMD_INITIATE_TASK;
      hiob->SCSI_IF_noUnderrun = 0;

      if (transportSpecific)
      {
         hiob->SCSI_IF_tagEnable = ((SCSI_TRGTSH(taskSetHandle)->targetAttributes.tagEnable) &&
                                    (transportSpecific->forceUntagged == HIM_FALSE) &&
                                    /*HQ
                              (transportSpecific->disallowDisconnect == HIM_FALSE) &&
                                     */
                             (iob->taskAttribute != HIM_TASK_RECOVERY));
         hiob->SCSI_IF_disallowDisconnect =
               (transportSpecific->disallowDisconnect == HIM_TRUE) ? 1 : 0;
#if SCSI_NEGOTIATION_PER_IOB
         hiob->SCSI_IF_forceSync =
               (transportSpecific->forceSync == HIM_TRUE) ? 1 : 0;
         hiob->SCSI_IF_forceAsync =
               (transportSpecific->forceAsync == HIM_TRUE) ? 1 : 0;
         hiob->SCSI_IF_forceWide =
               (transportSpecific->forceWide == HIM_TRUE) ? 1 : 0;
         hiob->SCSI_IF_forceNarrow =
               (transportSpecific->forceNarrow == HIM_TRUE) ? 1 : 0;
         hiob->SCSI_IF_forceReqSenseNego =
               (transportSpecific->forceReqSenseNego == HIM_TRUE) ? 1 : 0;
#endif /* SCSI_NEGOTIATION_PER_IOB */
#if SCSI_PARITY_PER_IOB
         hiob->SCSI_IF_parityEnable = 
               (transportSpecific->parityEnable == HIM_TRUE) ? 1 : 0;
#endif
#if SCSI_PAC_NSX_REPEATER || SCSI_NSX_REPEATER
         hiob->SCSI_IF_nsxCommunication =
               (transportSpecific->nsxCommunication == HIM_TRUE) ? 1 : 0;
#endif /* SCSI_PAC_NSX_REPEATER || SCSI_NSX_REPEATER */
#if SCSI_DOMAIN_VALIDATION
         hiob->SCSI_IF_dvIOB = (transportSpecific->dvIOB == HIM_TRUE) ? 1 : 0;
#endif /* SCSI_DOMAIN_VALIDATION */
      }
      else
      {
         hiob->SCSI_IF_tagEnable = ((SCSI_TRGTSH(taskSetHandle)->targetAttributes.tagEnable) &&
                                    (iob->taskAttribute != HIM_TASK_RECOVERY));
         hiob->SCSI_IF_disallowDisconnect = 0;
#if SCSI_NEGOTIATION_PER_IOB
         hiob->SCSI_IF_forceSync = 0;
         hiob->SCSI_IF_forceAsync = 0;
         hiob->SCSI_IF_forceWide = 0;
         hiob->SCSI_IF_forceNarrow = 0;
         hiob->SCSI_IF_forceReqSenseNego = 0;
#endif
#if SCSI_PARITY_PER_IOB
         hiob->SCSI_IF_parityEnable = 0;
#endif
#if SCSI_PAC_NSX_REPEATER || SCSI_NSX_REPEATER
         hiob->SCSI_IF_nsxCommunication = 0;
#endif /* SCSI_PAC_NSX_REPEATER || SCSI_NSX_REPEATER */
#if SCSI_DOMAIN_VALIDATION
         hiob->SCSI_IF_dvIOB = 0;
#endif /* SCSI_DOMAIN_VALIDATION */
      }

      hiob->SCSI_IF_autoSense = iob->flagsIob.autoSense;
      hiob->SCSI_IF_freezeOnError = iob->flagsIob.freezeOnError;
      hiob->priority = iob->priority;
      hiob->snsBuffer = iob->errorData;

      if (iob->errorDataLength <= 255)
      {
         hiob->snsLength = (SCSI_UEXACT8) iob->errorDataLength;
      }
      else
      {
         hiob->snsLength = (SCSI_UEXACT8) 255;
      }

      /* send request to internal HIM */
      /* call to SCSI_QUEUE_HIOB(hiob); */
/************************************************
   SCSIRQueueHIOB() start 
*************************************************/
      hhcb = SCSI_TARGET_UNIT(hiob)->hhcb;

      /* fresh the hiob status and working area */
      SCSI_rFRESH_HIOB(hiob);

      /* start with checking criteria (max tags/nontags) */
/************************************************
   SCSIrCheckCriteria() start 
*************************************************/
      /* call to SCSIrCheckCriteria(hhcb,hiob); */
         targetControl = SCSI_TARGET_UNIT(hiob)->targetControl;

         /* get the criteria factor */
         maxScbs = ((hiob->SCSI_IF_tagEnable) ? targetControl->maxTagScbs :
                                             targetControl->maxNonTagScbs);

#if SCSI_NEGOTIATION_PER_IOB
         /* If HIOB sets the flags to force negotiation, try to do the  */
         /* negotiation */
         if (hiob->SCSI_IF_forceSync || hiob->SCSI_IF_forceAsync ||
             hiob->SCSI_IF_forceWide || hiob->SCSI_IF_forceNarrow)
         {
            if (!SCSIHCheckDeviceIdle(hhcb, SCSI_TARGET_UNIT(hiob)->scsiID))
            {
               maxScbs = 0;
            }
            else
            {
               SCSI_hCHANGEXFEROPTION(hiob);
            }
         }
         /* If a negotiation is pending, queue the incoming hiob into the    */
         /* device queue, so that the command associated with this hiob will */
         /* be executed after the negotiation. */
         if (targetControl->pendingNego)
         {
            maxScbs = 0;
         }
#endif /* SCSI_NEGOTIATION_PER_IOB */
        if (++targetControl->activeScbs <= maxScbs)
         {
            /* criteria met, continue with allocating scb */
            /* call to SCSIrAllocateScb(hhcb,hiob); */
/************************************************
   SCSIrAllocateScb() start 
*************************************************/
            /* call SCSIrGetScb if ((hiob->scbNumber = (SCSI_UEXACT8) 
                  SCSI_rGETSCB(hhcb,hiob)) != SCSI_NULL_SCB) */
            if (hhcb->SCSI_FREE_SCB_AVAIL >= 2) /* this is more efficient - no scb available */
            {
/************************************************
   SCSIrGetScb() start 
*************************************************/
               /* no scb available */
               /*if (hhcb->SCSI_FREE_SCB_AVAIL < 2)
               {
                  return(SCSI_NULL_SCB);
               }
               else
               {
                  hhcb->SCSI_FREE_SCB_AVAIL--;
                  hiob->scbDescriptor = hhcb->SCSI_FREE_HEAD;
                  hhcb->SCSI_FREE_HEAD = hhcb->SCSI_FREE_HEAD->queueNext;
                  return(hiob->scbDescriptor->scbNumber);
               } */
               /* this is more efficient */
               hhcb->SCSI_FREE_SCB_AVAIL--;
               hiob->scbDescriptor = hhcb->SCSI_FREE_HEAD;
               hhcb->SCSI_FREE_HEAD = hhcb->SCSI_FREE_HEAD->queueNext;
               hiob->scbNumber = hiob->scbDescriptor->scbNumber;
/************************************************
   SCSIrGetScb() end 
*************************************************/
               /* scb available, queue to hardware management layer */
               /* call SCSIHQueueHIOB(hiob); */
/************************************************
   SCSIHQueueHIOB() start 
*************************************************/
               scsiRegister = hhcb->scsiRegister;

               /* fresh the hiob status and working area */
               SCSI_hFRESH_HIOB(hiob);

               /* setup active pointer */
               SCSI_ACTPTR[hiob->scbNumber] = hiob;

               /* deliver it to sequencer */
               /* call to SCSI_hDELIVERSCB(hhcb, hiob); */
/************************************************
   SCSIhDeliverScb() start 
*************************************************/
               /* setup scb buffer */
               OSD_BUILD_SCB(hiob);  /* mode dependent so can't inline */

               SCSI_hPATCH_XFER_OPT(hhcb,hiob); /* mode dependent so can't inline */

               /* bump q new head */
               (SCSI_UEXACT8)hhcb->SCSI_HP_qNewHead++;

               OSD_OUTEXACT8(SCSI_AICREG(SCSI_hQ_NEW_HEAD(hhcb)),
                  hhcb->SCSI_HP_qNewHead);
/************************************************
   SCSIhDeliverScb() end
*************************************************/

               OSD_SYNCHRONIZE_IOS(hhcb);
/************************************************
   SCSIHQueueHIOB() end 
*************************************************/
            }
            else
            {
               /* queue it to host queue */
               SCSIrHostQueueAppend(hhcb,hiob);
            }
/************************************************
   SCSIrAllocateScb() end
*************************************************/
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
/************************************************
   SCSIrCheckCriteria() end 
*************************************************/
/************************************************
   SCSIRQueueHIOB() end 
*************************************************/
   }

   else
   {
      /* process other special iob */
      switch(iob->function)
      {
         case HIM_TERMINATE_TASK:
         case HIM_ABORT_TASK:
            hiob->cmd = SCSI_CMD_ABORT_TASK;
            hiob->unitHandle.relatedHiob = &SCSI_IOBRSV(iob->relatedIob)->hiob;
            break;

         case HIM_ABORT_TASK_SET:
            hiob->cmd = SCSI_CMD_ABORT_TASK_SET;
            hiob->unitHandle.targetUnit = &SCSI_TRGTSH(taskSetHandle)->unitControl;
            break;

         case HIM_RESET_BUS_OR_TARGET:
            if (SCSI_TRGTSH(taskSetHandle)->typeTSCB == SCSI_TSCB_TYPE_TARGET)
            {
               /* reset target */
               hiob->cmd =  SCSI_CMD_RESET_TARGET;
               hiob->unitHandle.targetUnit = &SCSI_TRGTSH(taskSetHandle)->unitControl;
            }
            else if (SCSI_TRGTSH(taskSetHandle)->typeTSCB == SCSI_TSCB_TYPE_ADAPTER)
            {
               /* reset scsi bus */
               hiob->cmd = SCSI_CMD_RESET_BUS;
               hiob->unitHandle.initiatorUnit = &SCSI_ADPTSH(taskSetHandle)->hhcb;
            }
            else
            {
               /* illegal request */
               iob->taskStatus = HIM_IOB_TSH_INVALID;
               iob->postRoutine(iob);
               return ;
            }
            break;

         case HIM_RESET_HARDWARE:
            hiob->cmd = SCSI_CMD_RESET_HARDWARE;
            hiob->unitHandle.initiatorUnit = &SCSI_ADPTSH(taskSetHandle)->hhcb;
            break;

         case HIM_PROTOCOL_AUTO_CONFIG:
            hiob->cmd = SCSI_CMD_PROTO_AUTO_CFG;
            hiob->unitHandle.initiatorUnit = &SCSI_ADPTSH(taskSetHandle)->hhcb;
            SCSI_xSAVE_CURRENT_WWIDS(SCSI_ADPTSH(taskSetHandle));
            break;

#if !SCSI_DISABLE_PROBE_SUPPORT
         case HIM_PROBE:
            if ((transportSpecific) &&
#if !SCSI_NULL_SELECTION
                (transportSpecific->scsiID != 
                 SCSI_ADPTSH(taskSetHandle)->hhcb.hostScsiID) &&

#endif /* !SCSI_NULL_SELECTION */
                !(transportSpecific->LUN[0]))
            {
               SCSI_ADPTSH(iob->taskSetHandle)->iobProtocolAutoConfig = iob;
               SCSIxSetupLunProbe(SCSI_ADPTSH(iob->taskSetHandle),
                  transportSpecific->scsiID,
                  transportSpecific->LUN[1]);
               SCSIxQueueBusScan(SCSI_ADPTSH(iob->taskSetHandle));
               SCSI_ADPTSH(iob->taskSetHandle)->lastScsiIDProbed = 
                  transportSpecific->scsiID;
               SCSI_ADPTSH(iob->taskSetHandle)->lastScsiLUNProbed = 
                  transportSpecific->LUN[1];
   
            }
            else
            {
               /* invalid probe */
               iob->taskStatus = HIM_IOB_INVALID;
               iob->postRoutine(iob);
            }
            return;
#endif /* !SCSI_DISABLE_PROBE_SUPPORT */

         case HIM_SUSPEND:
         case HIM_QUIESCE:
            if (SCSI_CHECK_HOST_IDLE(&SCSI_ADPTSH(taskSetHandle)->hhcb))
            {
               /* host device is idle and nothing has to be done */
               iob->taskStatus = HIM_IOB_GOOD;
            }
            else
            {
               /* indicate host adapter is not idle */
               iob->taskStatus = HIM_IOB_ADAPTER_NOT_IDLE;
            }
#if SCSI_TARGET_OPERATION
            hhcb = &SCSI_ADPTSH(taskSetHandle)->hhcb;
            if (hhcb->SCSI_HF_targetMode)
            {
               /* Disable selection-in */
               SCSI_DISABLE_SELECTION_IN(hhcb);
               
               /* Need to check if the SCSI bus is held or */
               /* any pending selection-in interrupts      */
               if ((hhcb->SCSI_HF_targetScsiBusHeld) ||
                   (SCSI_CHECK_SELECTION_IN_INTERRUPT_PENDING(hhcb))) 
               {
                  /* indicate lost adapter is not idle */ 
                  iob->taskStatus = HIM_IOB_ADAPTER_NOT_IDLE;
               }
            }
#endif /* SCSI_TARGET_OPERATION */
            iob->postRoutine(iob);
            return ;

         case HIM_RESUME:
#if SCSI_TARGET_OPERATION
            /* Target Mode enabled then this function
             * enables selection-in
             */
            hhcb = &SCSI_ADPTSH(taskSetHandle)->hhcb;
            SCSI_hTARGETMODEENABLE(hhcb);
#endif /* SCSI_TARGET_OPERATION */            
            /* just return good status and nothing has to be done */
            iob->taskStatus = HIM_IOB_GOOD;
            iob->postRoutine(iob);
            return ;

         case HIM_CLEAR_XCA:
            /* not supported for this implementation */
            iob->taskStatus = HIM_IOB_UNSUPPORTED;
            iob->postRoutine(iob);
            return ;

         case HIM_UNFREEZE_QUEUE:
            /* unfreeze device queue */
            hiob->cmd = SCSI_CMD_UNFREEZE_QUEUE;
            hiob->unitHandle.targetUnit = &SCSI_TRGTSH(taskSetHandle)->unitControl;
            break;

#if SCSI_DMA_SUPPORT
         case HIM_INITIATE_DMA_TASK:
            /* use specially reserved dma TSCB */ 
            hiob->unitHandle.targetUnit =
               &(SCSI_TRGTSH(&SCSI_ADPTSH(taskSetHandle)->dmaTargetTSCB)->unitControl);
            hiob->cmd = SCSI_CMD_INITIATE_DMA_TASK;
            /* clear all hiob flags */
            hiob->SCSI_IF_flags = 0;
            break;    
#endif /* SCSI_DMA_SUPPORT */

#if SCSI_TARGET_OPERATION
         case HIM_ESTABLISH_CONNECTION:
#if SCSI_RESOURCE_MANAGEMENT
            hiob->cmd = SCSI_CMD_ESTABLISH_CONNECTION;
            hiob->unitHandle.initiatorUnit = &SCSI_ADPTSH(taskSetHandle)->hhcb;
            /* snsBuffer contains the address of where the 
               received command or task management request 
               is to be stored. */
            hiob->snsBuffer = iob->targetCommand;
            hiob->snsBufferSize = (SCSI_UEXACT16) iob->targetCommandBufferSize;
            break;
#else
            iob->taskStatus = HIM_IOB_INVALID;
            iob->postRoutine(iob);
            return;
  
#endif /* SCSI_RESOURCE_MANAGEMENT */
         case HIM_REESTABLISH_AND_COMPLETE:
            if ( (iob->targetCommandLength != 1) || 
                 (iob->flagsIob.outOfOrderTransfer) ) 
            {
               /* SCSI protocol expects one byte of status */
               /* outOfOrderTransfer not supported */ 
               iob->taskStatus = HIM_IOB_INVALID;
               iob->postRoutine(iob);
               return; 
            }
            hiob->cmd = SCSI_CMD_REESTABLISH_AND_COMPLETE;
            SCSI_NEXUS_UNIT(hiob) = SCSI_NEXUSTSH(taskSetHandle);
            hiob->ioLength = iob->ioLength;
            hiob->snsBufferSize = (SCSI_UEXACT16) iob->targetCommandBufferSize;
            scsiStatus = (SCSI_UEXACT8 HIM_PTR) iob->targetCommand;
            hiob->initiatorStatus = scsiStatus[0];
            if (iob->flagsIob.targetRequestType == HIM_REQUEST_TYPE_TMF)
            {
               hiob->SCSI_IF_taskManagementResponse = 1;
            }
            else
            {
               hiob->SCSI_IF_taskManagementResponse = 0;
            }  
            break;
             
         case HIM_REESTABLISH_INTERMEDIATE:
            if (iob->flagsIob.outOfOrderTransfer) 
            {
               /* Not supported */
               iob->taskStatus = HIM_IOB_INVALID;
               iob->postRoutine(iob);
               return; 
            }
            hiob->cmd = SCSI_CMD_REESTABLISH_INTERMEDIATE;
            SCSI_NEXUS_UNIT(hiob) = SCSI_NEXUSTSH(taskSetHandle);
            hiob->ioLength = iob->ioLength;
            /* hiob->snsBuffer = iob->errorData;  */
            hiob->snsBufferSize = (SCSI_UEXACT16)iob->targetCommandBufferSize;
            if (transportSpecific)
            {
               hiob->SCSI_IF_disallowDisconnect = transportSpecific->disallowDisconnect;
            }
            else
            {
               hiob->SCSI_IF_disallowDisconnect = 0; 
            }
            break;

         case HIM_ABORT_NEXUS:
            hiob->cmd = SCSI_CMD_ABORT_NEXUS;
            SCSI_NEXUS_UNIT(hiob) = SCSI_NEXUSTSH(taskSetHandle);
            break;

#if SCSI_MULTIPLEID_SUPPORT
         case HIM_ENABLE_ID:
            if (!SCSIxEnableID(iob)) 
            {
               iob->taskStatus = HIM_IOB_INVALID;
               iob->postRoutine(iob);
               return;
            }
            else
            {
               hiob->cmd = SCSI_CMD_ENABLE_ID;
               hiob->unitHandle.initiatorUnit = &SCSI_ADPTSH(taskSetHandle)->hhcb;
               hiob->snsBufferSize =
                  ((HIM_TS_ID_SCSI HIM_PTR) iob->transportSpecific)->scsiIDMask;
            }
            break;

         case HIM_DISABLE_ID:
            if (!SCSIxDisableID(iob))
            {
               iob->taskStatus = HIM_IOB_INVALID;
               iob->postRoutine(iob);
               return;
            }
            else
            {
               hiob->cmd = SCSI_CMD_DISABLE_ID;
               hiob->unitHandle.initiatorUnit = &SCSI_ADPTSH(taskSetHandle)->hhcb;
               hiob->snsBufferSize =
                  ((HIM_TS_ID_SCSI HIM_PTR) iob->transportSpecific)->scsiIDMask;
            }
            break;
          
#endif /* SCSI_MULTIPLEID_SUPPORT */
#endif /* SCSI_TARGET_OPERATION */

         default:
            /* indicate iob invalid and post back immediately */
            iob->taskStatus = HIM_IOB_INVALID;
            iob->postRoutine(iob);
            return ;
      }

      /* send request to internal HIM */
      SCSI_QUEUE_SPECIAL_HIOB(hiob);
   }
}
#endif /* SCSI_STREAMLINE_QIOPATH */

/*********************************************************************
*
*  SCSIFrontEndISR
*
*     Execute front end interrupt service rutine
*
*  Return Value:  HIM_NOTHING_PENDING, no interrupt pending
*                 HIM_INTERRUPT_PENDING, interrupt pending
*                 HIM_LONG_INTERRUPT_PENDING, lengthy interupt pending
*                  
*  Parameters:    adapter TSH
*
*  Remarks:       This rotuine should be very fast in execution.
*                 The lengthy processing of interrupt should be
*                 handled in SCSIBackEndISR.
*
*********************************************************************/
#if SCSI_STREAMLINE_QIOPATH
HIM_UINT8 SCSIFrontEndISR (HIM_TASK_SET_HANDLE adapterTSH)
{
/************************************************
   new locals start 
*************************************************/
   SCSI_HHCB SCSI_HPTR hhcb; /* SCSIHFrontEndISR,SCSIhStandardQoutcnt */
   register SCSI_REGISTER scsiRegister; /* SCSIHFrontEndISR */
   SCSI_UEXACT8 intstat; /* SCSIHFrontEndISR */
   int i; /* SCSIHFrontEndISR */
   SCSI_UEXACT8 qoutcnt; /* SCSIHFrontEndISR */
   SCSI_UEXACT8 qDonePass; /* SCSIhStandardQoutcnt */
   SCSI_UEXACT8 qDoneElement; /* SCSIhStandardQoutcnt */
   SCSI_QOUTFIFO_NEW SCSI_HPTR qOutPtr; /* SCSIhStandardQoutcnt */
   SCSI_UEXACT8 quePassCnt; /* SCSIhStandardQoutcnt */
#if SCSI_CHECK_PCI_ERROR
   SCSI_UEXACT8 error; /* SCSIHFrontEndISR */
#endif /* #if SCSI_CHECK_PCI_ERROR */

/************************************************
   new locals end 
*************************************************/

/************************************************
   SCSIHFrontEndISR() start 
*************************************************/
   hhcb = &SCSI_ADPTSH(adapterTSH)->hhcb;
   scsiRegister = hhcb->scsiRegister;

   /* check if there is any command complete interrupt */
   for (i = 0;; i++)
   {
/************************************************
   SCSIhStandardQoutcnt() start 
*************************************************/
      /* SCSI_UEXACT8 n = 0; */
      qoutcnt = 0; /* use qoutcnt since cant return(n) */
      qDonePass = hhcb->SCSI_HP_qDonePass;
      qDoneElement = hhcb->SCSI_HP_qDoneElement;
      qOutPtr = SCSI_QOUT_PTR_ARRAY_NEW(hhcb) + qDoneElement;
                                       
      while(1)
      {
         /* invalidate cache */
         SCSI_INVALIDATE_CACHE(((SCSI_UEXACT8 SCSI_HPTR)SCSI_QOUT_PTR_ARRAY_NEW(hhcb)) +
            ((qDoneElement * sizeof(SCSI_QOUTFIFO_NEW)) /
            OSD_DMA_SWAP_WIDTH) * OSD_DMA_SWAP_WIDTH, OSD_DMA_SWAP_WIDTH);

         /* check if there is any valid entry */
         SCSI_GET_LITTLE_ENDIAN8(hhcb,&quePassCnt,qOutPtr,
            OSDoffsetof(SCSI_QOUTFIFO_NEW,quePassCnt));

         if (quePassCnt == qDonePass)
         {
            /* bump the number of outstanding entries in qoutfifo */
            /* ++n; */
            ++qoutcnt; /* use qoutcnt since cant return(n) */

            /* increment index */
            if (++qDoneElement != (SCSI_UEXACT8) (hhcb->numberScbs + 1))
            {
               ++qOutPtr;
            }
            else
            {
               /* wrap around */
               ++qDonePass;
               qDoneElement = 0;
               qOutPtr = SCSI_QOUT_PTR_ARRAY_NEW(hhcb);
            }
         }
         else
         {
            break;
         }
      }

      /* return(n); */
/************************************************
   SCSIhStandardQoutcnt() end 
*************************************************/
      /* qoutcnt = SCSI_hQOUTCNT(hhcb); - must delete since no function call */
      if (hhcb->SCSI_HP_qoutcnt != qoutcnt)
      {
         /* remember the interrupt status (command complete) */
         hhcb->SCSI_HP_qoutcnt = qoutcnt;
         hhcb->SCSI_HP_intstat |= (intstat = SCSI_CMDCMPLT);

         /* clear the interrupt and assume this is the only */
         /* interrupt we have so far */
         OSD_OUTEXACT8(SCSI_AICREG(SCSI_CLRINT),SCSI_CLRCMDINT);
         break;
      }
      else
      {
         /* just read interrupt status from hardware */
         intstat = SCSI_hREAD_INTSTAT(hhcb,scsiRegister);

         /* For Cardbus (APA-1480x), the instat register value is 0xFF */
         /* when the card is hot removed from the bus.  For this case, */
         /* CHIM returns with no interrupt pending.  This value is not */
         /* going to happen on the scsi host adapter. */
         if (intstat == 0xFF)
         {
            hhcb->SCSI_HP_intstat = intstat = 0;
            break;
         }
         
         if (intstat & SCSI_CMDCMPLT)
         {
            if (i)
            {
               /* clear the processed but not cleared */
               /* command complete interrupt */
               OSD_OUTEXACT8(SCSI_AICREG(SCSI_CLRINT),SCSI_CLRCMDINT);
            }
            else
            {
               /* go back to check if it's processed but not cleared */
               /* command complete interrupt */
               continue;
            }
         }

#if SCSI_CHECK_PCI_ERROR
         /* only check pci error for now */
         if((hhcb->hardwareMode != SCSI_HMODE_AIC78XX) && 
           (error = (OSD_INEXACT8(SCSI_AICREG(SCSI_ERROR)) & SCSI_PCIERRSTAT)))
         {
            /* svae error status */
            hhcb->SCSI_HP_error = error;
            hhcb->SCSI_HP_hcntrl = OSD_INEXACT8(SCSI_AICREG(SCSI_HCNTRL));
            
            /* disable hardware interrupt with pause */
            SCSI_hWRITE_HCNTRL(hhcb,scsiRegister,SCSI_PAUSE);
         }
#endif /* #if SCSI_CHECK_PCI_ERROR */

         if (intstat & (SCSI_BRKINT | SCSI_SCSIINT | SCSI_SEQINT))
         {
            if (intstat & SCSI_SEQINT)
            {
               /* keep sequencer interrupt code */
               hhcb->SCSI_HP_intstat |= (intstat & SCSI_INTCODE);
            }

            /* keep interrupt status */
            hhcb->SCSI_HP_intstat |= (intstat & (SCSI_BRKINT | SCSI_SCSIINT | SCSI_CMDCMPLT | SCSI_SEQINT));
            hhcb->SCSI_HP_hcntrl = OSD_INEXACT8(SCSI_AICREG(SCSI_HCNTRL));

            /* disable hardware interrupt with pause */
            SCSI_hWRITE_HCNTRL(hhcb,scsiRegister,SCSI_PAUSE);

            /* workaround for hanging of sequencer immediately after a pause */
            /* due to hardware stretch                                       */
            SCSI_hSEQSTRETCHWORKAROUND(hhcb);
         }

         /* ignore the upper nibble */
         /* there is no point to record sparrious interrupt */
         intstat &= (SCSI_BRKINT | SCSI_SCSIINT | SCSI_CMDCMPLT | SCSI_SEQINT);
         break;
      }
   }

   OSD_SYNCHRONIZE_IOS(hhcb);

   /* return(intstat); */
/************************************************
   SCSIHFrontEndISR() end
*************************************************/
   /* if(SCSI_FRONT_END_ISR(&SCSI_ADPTSH(adapterTSH)->hhcb)) */
#if SCSI_CHECK_PCI_ERROR
   if (intstat|error) /* must do this way since no function call */
#else
   if (intstat) /* must do this way since no function call */
#endif /* #if SCSI_CHECK_PCI_ERROR */
   {
      return(HIM_INTERRUPT_PENDING);
   }
   else
   {
      return(HIM_NOTHING_PENDING);
   }
}
#endif /* SCSI_STREAMLINE_QIOPATH */

/*********************************************************************
*
*  SCSIBackEndISR
*
*     Execute back end interrupt service rutine
*
*  Return Value:  none
*                  
*  Parameters:    adapter TSH
*
*  Remarks:       It's OSM's decision to execute this routine at
*                 interrupt context or not.
*
*********************************************************************/
#if SCSI_STREAMLINE_QIOPATH
void SCSIBackEndISR (HIM_TASK_SET_HANDLE adapterTSH)
{

/************************************************
   new locals start 
*************************************************/
   SCSI_HHCB SCSI_HPTR hhcb = &SCSI_ADPTSH(adapterTSH)->hhcb; /* SCSIHBackEndISR */
      /* SCSIhCmdComplete, SCSIhStandardRetrieveScb, SCSIhTerminateCommand */
      /* SCSIrCompleteHIOB,SCSIrFreeScb,SCSIrReturnScb,SCSIrFreeCriteria */
      /* SCSIrPostDoneQueue */
   register SCSI_REGISTER scsiRegister = hhcb->scsiRegister; /* SCSIHBackEndISR */
      /* SCSIhCmdComplete */
   SCSI_HIOB SCSI_IPTR hiob; /* SCSIHBackEndISR, SCSIhCmdComplete,SCSIhTerminateCommand */
      /* SCSIhSetStat,SCSIrCompleteHIOB,SCSIrFreeScb,SCSIrReturnScb,SCSIrFreeCriteria */
      /* OSDCompleteHIOB,SCSIxTranslateError,SCSIrPostDoneQueue */
   SCSI_UEXACT8 intstat; /* SCSIHBackEndISR */
   SCSI_UEXACT8 sstat1; /* SCSIHBackEndISR */
   SCSI_UEXACT8 scbNumber; /* SCSIHBackEndISR,SCSIhCmdComplete,SCSIhStandardRetrieveScb */
   SCSI_UEXACT8 intcode; /* SCSIHBackEndISR */
   SCSI_QOUTFIFO_NEW SCSI_HPTR qOutPtr; /* SCSIhStandardRetrieveScb */
   SCSI_UEXACT8 qPassCnt; /* SCSIhStandardRetrieveScb */
   SCSI_UNIT_CONTROL SCSI_IPTR targetUnit; /* SCSIhTerminateCommand,SCSIrFreeCriteria */
   SCSI_TARGET_CONTROL SCSI_HPTR targetControl; /* SCSIrCompleteHIOB,SCSIrFreeCriteria */
   SCSI_HIOB SCSI_IPTR hiobNew; /* SCSIrFreeScb,SCSIrFreeCriteria */
   SCSI_UINT16 maxScbs; /* SCSIrFreeCriteria */
   SCSI_UEXACT8 origTail; /* SCSIrPostDoneQueue */
   HIM_IOB HIM_PTR iob; /* OSDCompleteHIOB,SCSIxTranslateError */
   SCSI_UEXACT8 hcntrl;
#if SCSI_CHECK_PCI_ERROR
   SCSI_UEXACT8 error; /* SCSIHBackEndISR */
#endif /* #if SCSI_CHECK_PCI_ERROR */
   SCSI_UEXACT8 exceptionCount;
#if SCSI_TARGET_OPERATION  
   SCSI_UEXACT8 sstat0; 
#endif  /* SCSI_TARGET_OPERATION */  

/************************************************
   new locals end 
*************************************************/

   /* call to SCSI_BACK_END_ISR(&SCSI_ADPTSH(adapterTSH)->hhcb); */
/************************************************
   SCSIRBackEndISR() start
*************************************************/

/************************************************
   SCSIHBackEndISR() start
*************************************************/
   /* call to SCSIHBackEndISR(hhcb); */

   intstat = hhcb->SCSI_HP_intstat;
   hhcb->SCSI_HP_intstat = 0;

#if SCSI_CHECK_PCI_ERROR
   error = hhcb->SCSI_HP_error;
   hhcb->SCSI_HP_error = 0;
#endif /* #if SCSI_CHECK_PCI_ERROR */

   if (intstat & SCSI_CMDCMPLT)
   {
      /* handle command complete interrupt */
      /* call to SCSIhCmdComplete(hhcb); */
/************************************************
   SCSIhCmdComplete() start
*************************************************/
      /* int retValue = 0; dont need a return value */

      while (1)
      {
         /* service all command complete outstanding */

         /* dont call in while loop - */
         /* while ((scbNumber = (SCSI_UEXACT8) SCSI_hRETRIEVESCB(hhcb)) != SCSI_NULL_SCB) */
         while (1)
         {
/************************************************
   SCSIhStandardRetrieveScb() start
*************************************************/
            qOutPtr = SCSI_QOUT_PTR_ARRAY_NEW(hhcb) + hhcb->SCSI_HP_qDoneElement;

            /* invalidate cache */
            SCSI_INVALIDATE_CACHE(((SCSI_UEXACT8 SCSI_HPTR)SCSI_QOUT_PTR_ARRAY_NEW(hhcb)) +
               ((hhcb->SCSI_HP_qDoneElement * sizeof(SCSI_QOUTFIFO_NEW)) /
               OSD_DMA_SWAP_WIDTH) * OSD_DMA_SWAP_WIDTH, OSD_DMA_SWAP_WIDTH);

            /* check if there is any valid entry */
            SCSI_GET_LITTLE_ENDIAN8(hhcb,&qPassCnt,qOutPtr,
               OSDoffsetof(SCSI_QOUTFIFO_NEW,quePassCnt));
            if (qPassCnt == hhcb->SCSI_HP_qDonePass)
            {
               /* found valid entry, get scb number and update */
               SCSI_GET_LITTLE_ENDIAN8(hhcb,&scbNumber,qOutPtr,
                  OSDoffsetof(SCSI_QOUTFIFO_NEW,scbNumber));

               if (++hhcb->SCSI_HP_qDoneElement == (SCSI_UEXACT8) (hhcb->numberScbs + 1))
               {
                  hhcb->SCSI_HP_qDoneElement = 0;
                  ++hhcb->SCSI_HP_qDonePass;                  
               }
            }
            else
            {
               /* set scb number to null */
               /*scbNumber = SCSI_NULL_SCB;*/
               break; /* break, instead of while(scbNumber != NULL_SCB) */
            }

            /* return(scbNumber); dont need to return a value */ 
/************************************************
   SCSIhStandardRetrieveScb() end
*************************************************/
            /* get associated hiob */
            hiob = SCSI_ACTPTR[scbNumber];

            /* If there is no associated hiob, assume it as fatal */
            if (hiob == SCSI_NULL_HIOB)
            {
               /* Something is wrong with the device or sequencer */
               /* if there is no hiob reference */
               /* save the HCNTRL value */
               hcntrl = OSD_INEXACT8(SCSI_AICREG(SCSI_HCNTRL));

               /* pause the chip if necessary */
               if (!(hcntrl & SCSI_PAUSEACK))
               {
                  SCSIhPauseAndWait(hhcb);
               }

               SCSIhBadSeq(hhcb);

               /* restore HCNTRL if necessary */
               if (!(hcntrl & SCSI_PAUSEACK))
               {
                  SCSI_hWRITE_HCNTRL(hhcb,scsiRegister,hcntrl);
               }
               break;
            }

            /* Due to the active abort implementation, a HIOB to be aborted */
            /* might completed before the active abort message has ever */
            /* started.  Therefore, we will not post back any HIOB that */
            /* associated with the abort already in progress status. */
            if (hiob->SCSI_IP_mgrStat != SCSI_MGR_ABORTINPROG)
            {
               /* indicate command complete was active */
               /* ++retValue; dont need return value */

               /* terminate the command */
#if SCSI_TARGET_OPERATION
               if ((hiob)->SCSI_IP_targetMode)
               {
                  /* call to SCSIhTerminateTargetCommand */
                  SCSIhTerminateTargetCommand(hiob);
               }
               else
               {
#endif /* SCSI_TARGET_OPERATION */
                  /* call to SCSIhTerminateCommand(hiob); */
/************************************************
   SCSIhTerminateCommand() start
*************************************************/
                  targetUnit = SCSI_TARGET_UNIT(hiob);

                  /* free the associated entry in active pointer array */
                  SCSI_ACTPTR[hiob->scbNumber] = SCSI_NULL_HIOB;

                  /* make sure proper status get passed back */
/************************************************
   SCSIhSetStat() start
*************************************************/
                  /* call to SCSIhSetStat(hiob); */
                  if (hiob->haStat || ((hiob->trgStatus != SCSI_UNIT_GOOD) &&
                                       (hiob->trgStatus != SCSI_UNIT_MET) &&
                                       (hiob->trgStatus != SCSI_UNIT_INTERMED) &&
                                       (hiob->trgStatus != SCSI_UNIT_INTMED_GD)))
                  {
                     if (hiob->stat != SCSI_SCB_ABORTED)
                     {
                        /* The HIOB is really completed.  But the haStat was set during */
                        /* active abort process.  And again check for the trgStatus. */
                        if (((hiob->haStat == SCSI_HOST_ABT_HOST) ||
                             (hiob->haStat == SCSI_HOST_ABT_TRG_RST)) &&
                            ((hiob->trgStatus == SCSI_UNIT_GOOD) ||
                             (hiob->trgStatus == SCSI_UNIT_MET) ||
                             (hiob->trgStatus == SCSI_UNIT_INTERMED) ||
                             (hiob->trgStatus == SCSI_UNIT_INTMED_GD)))
                        {
                           hiob->stat = SCSI_SCB_COMP;   /* HIOB completed without error */
                        }
                        else
                        {
                           hiob->stat = SCSI_SCB_ERR;    /* HIOB completed with error */
                        }
                     }
                     else
                     {
                        /* Check for the terminate HIOB due to the selection time-out, */
                        /* unexpected busfree, or other target errors: queue full etc. */
                        if ((hiob->haStat != SCSI_HOST_ABT_HOST) &&
                            (hiob->haStat != SCSI_HOST_ABT_TRG_RST))
                        {
                           hiob->stat = SCSI_SCB_ERR;    /* HIOB completed with error */
                        }
                        else if ((hiob->trgStatus != SCSI_UNIT_GOOD) &&
                                 (hiob->trgStatus != SCSI_UNIT_MET) &&
                                 (hiob->trgStatus != SCSI_UNIT_INTERMED) &&
                                 (hiob->trgStatus != SCSI_UNIT_INTMED_GD))
                        {
                           /* Need to clear haStat because the error is the trgStatus */
                           hiob->haStat = 0;
                           hiob->stat = SCSI_SCB_ERR;    /* HIOB completed with error */
                        }

                     }
                  }
                  else
                  {
                     hiob->stat = SCSI_SCB_COMP;           /* Update status */
                  }
/************************************************
   SCSISetStat() end
*************************************************/

                  if (hiob->cmd == SCSI_CMD_INITIATE_TASK)
                  {
                     /* post it back to upper layer code */
                     /* call SCSI_COMPLETE_HIOB(hiob); */
/************************************************
   SCSIrCompleteHIOB() start
*************************************************/
                     if (hiob->stat != SCSI_SCB_FROZEN)
                     { 
                        if (hiob->trgStatus == SCSI_UNIT_QUEFULL)
                        {
                           targetControl = SCSI_TARGET_UNIT(hiob)->targetControl; 
                           /* The queue has already been frozen by the */
                           /* hardware layer; just set the             */
                           /* SCSI_FREEZE_QFULL bit in the freezemap   */
                           /* variable                                 */
                           SCSIrFreezeDeviceQueue(targetControl, SCSI_FREEZE_QFULL);
                        }
                     }
                     /* free scb associated */
                     /* call to SCSIrFreeScb(hhcb,hiob); */
/************************************************
   SCSIrFreeScb() start
*************************************************/
                     /* less efficient, plus hard if no fn call - if ((hiobNew = */
                     /* SCSI_rHOSTQUEUEREMOVE(hhcb,hiob->scbNumber)) != */
                     /* SCSI_NULL_HIOB) */
                     if (hhcb->SCSI_WAIT_HEAD != SCSI_NULL_HIOB) /* new way */
                     {
                        /* must get hiobNew here since not in the if() */
                        hiobNew = SCSI_rHOSTQUEUEREMOVE(hhcb,hiob->scbNumber);

                        /* must return scb to linked list then grab off top to */
                        /* ensure list is linked properly */
                        SCSI_rRETURNSCB(hhcb,hiob); /* return completed scb */
                        /* dont need to check return value of GETSCB since we know */
                        /* there will be an scb available since we just returned one */
                        hiobNew->scbNumber = SCSI_rGETSCB(hhcb,hiobNew);

                        /* there are some special HIOBs, e.g. Bus Device Reset, */
                        /* are queueing just like a normal HIOB */
                        if (hiobNew->cmd == SCSI_CMD_INITIATE_TASK)
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
/************************************************
   SCSIrReturnScb() start
*************************************************/
                        /* call to SCSI_rRETURNSCB(hhcb,hiob); */
                        hhcb->SCSI_FREE_SCB_AVAIL++;

                        hhcb->SCSI_FREE_TAIL->queueNext = hiob->scbDescriptor;
                        hhcb->SCSI_FREE_TAIL = hiob->scbDescriptor;
/************************************************
   SCSIrReturnScb() end
*************************************************/
                     }

                     /* continue to free criteria */
                     /* call to SCSIrFreeCriteria(hiob); */
/************************************************
   SCSIrFreeCriteria() start
*************************************************/
                     targetUnit = SCSI_TARGET_UNIT(hiob);
                     targetControl = targetUnit->targetControl;

                     /* get checking criteria */
                     maxScbs = ((hiob->SCSI_IF_tagEnable) ? 
                                 targetControl->maxTagScbs :
                                 targetControl->maxNonTagScbs);

                     if (!targetControl->freezeMap)
                     {
#if SCSI_NEGOTIATION_PER_IOB
                        /* Some hiobs are left in the device queue if there are    */
                        /* more scbs waiting in the rsm layer than maximum allowed */
                        /* tags, OR there is negotiation pending; so just check to */
                        /* see if any HIOBs are sitting in the device queue. */
                        targetControl->activeScbs--;
                        while (targetControl->deviceQueueHiobs)
#else /* SCSI_NEGOTIATION_PER_IOB */
                        if (--targetControl->activeScbs >= maxScbs)
#endif /* SCSI_NEGOTIATION_PER_IOB */
                        {
                           /* some one must be waiting for criteria free */
                           /* dont have ret value if inline */
                           /* if ((hiobNew = SCSIrDeviceQueueRemove(targetControl)) != */
                           /*     SCSI_NULL_HIOB) */
/************************************************
   SCSIrDeviceQueueRemove() start
*************************************************/
                           hiobNew = targetControl->queueHead;

                           if (hiobNew != SCSI_NULL_HIOB)
                           {
                              /* Get the criteria factor */
                              maxScbs = ((hiobNew->SCSI_IF_tagEnable) ?
                                          targetControl->maxTagScbs :
                                          targetControl->maxNonTagScbs);

                              /* If criteria is not met, simply return NULL_HIOB */
                              if (maxScbs <= (targetControl->activeScbs -
                                              targetControl->deviceQueueHiobs))
                              {
                                 hiobNew = SCSI_NULL_HIOB;
                              }
                              else
                              {
#if SCSI_NEGOTIATION_PER_IOB
                                 /* If HIOB sets the flags to force negotiation, */
                                 /* try to do the negotiation */
                                 if (hiobNew->SCSI_IF_forceSync ||
                                     hiobNew->SCSI_IF_forceAsync ||
                                     hiobNew->SCSI_IF_forceWide ||
                                     hiobNew->SCSI_IF_forceNarrow)
                                 {
                                    /* If the device is not idle, do not take the   */
                                    /* hiob out of the device queue as more commands*/
                                    /* are pending with the device, else change the */
                                    /* xfer option parameters to do the negotiation */
                                    /* on the next command. */
                                    if (!SCSIHCheckDeviceIdle(hhcb, targetUnit->scsiID))
                                    {
                                       hiobNew = SCSI_NULL_HIOB;
                                    }
                                    else
                                    {
                                       SCSI_hCHANGEXFEROPTION(hiobNew);
                                       targetControl->pendingNego--;
                                    }
                                 }

                                 /* Remove the hiob from the device queue only if */
                                 /* allowed to */
                                 if (hiobNew != SCSI_NULL_HIOB)
                                 {
                                    if (targetControl->queueHead == 
                                        targetControl->queueTail)
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
                                 if (targetControl->queueHead ==
                                     targetControl->queueTail)
                                 {
                                    /* queue empty */
                                    targetControl->queueHead =
                                       targetControl->queueTail = SCSI_NULL_HIOB;
                                 }
                                 else
                                 {
                                    /* queue won't be empty */
                                    targetControl->queueHead =
                                       targetControl->queueHead->queueNext;
                                 }

                                 targetControl->deviceQueueHiobs--;
#endif /* SCSI_NEGOTIATION_PER_IOB */
                              }
                           }

                           /* return(hiobNew); */
/************************************************
   SCSIrDeviceQueueRemove() end
*************************************************/
                           if (hiobNew != SCSI_NULL_HIOB) /* new way since
                                                             dont have a ret value */
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
                  
                        /* if no scbs outstanding for device then */
                        /* can finish up queue full handling */
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
                              /* Unfreeze the device queue and dispatch any queued */
                              /* HIOBs if the queue is not frozen. */
                              SCSIrUnFreezeDeviceQueue(
                                 SCSI_TARGET_UNIT(hiob)->targetControl,
                                 SCSI_FREEZE_HIPRIEXISTS);
                              SCSIrDispatchQueuedHIOB(hiob);
                           }
                        }
                     }
/************************************************
   SCSIrFreeCriteria() end
*************************************************/

/************************************************
   SCSIrFreeScb() end
*************************************************/
                     if (hiob->stat == SCSI_SCB_FROZEN)
                        /* Queue the hiob back in the Device Queue */
                     {
                        targetControl = SCSI_TARGET_UNIT(hiob)->targetControl; 
                        SCSIrDeviceQueueAppend(targetControl,hiob);
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
/************************************************
   SCSIrCompletHIOB() end
*************************************************/
                  }
                  else
                  {
                     /* Check and set if scsi negotiation is needed for */
                     /* a particular target after the bus device reset executed */
                     if (hiob->cmd == SCSI_CMD_RESET_TARGET)
                     {
#if !SCSI_NEGOTIATION_PER_IOB
                        /* save the HCNTRL value */
                        hcntrl = OSD_INEXACT8(SCSI_AICREG(SCSI_HCNTRL));
  
                        /* pause the chip if necessary */
                        if (!(hcntrl & SCSI_PAUSEACK))
                        {
                           SCSIhPauseAndWait(hhcb);
                        }

                        if (targetUnit->deviceTable->scsiOption &
                            (SCSI_WIDE_XFER | SCSI_SYNC_XFER))
                        {
                           /*  Set negotiation needed */
                           SCSI_hXFEROPTASSIGN(hhcb, targetUnit->scsiID, SCSI_NEEDNEGO);
                        }
                        else
                        {
                           SCSI_hXFEROPTASSIGN(hhcb, targetUnit->scsiID, 0x00);
                        }
 
                        /* restore HCNTRL if necessary */
                        if (!(hcntrl & SCSI_PAUSEACK))
                        {
                           SCSI_hWRITE_HCNTRL(hhcb,scsiRegister,hcntrl);
                        }
#endif /* !SCSI_NEGOTIATION_PER_IOB */
                     }

                     /* post back special HIOB to upper layer code */
                     SCSI_COMPLETE_SPECIAL_HIOB(hiob);
                  }
/************************************************
   SCSIhTerminateCommand() end
*************************************************/
#if SCSI_TARGET_OPERATION
               }
#endif /* SCSI_TARGET_OPERATION */
            }

            --hhcb->SCSI_HP_qoutcnt;

#if SCSI_BACKENDISR_OUTSIDE_INTRCONTEXT
            /* There is a problem when the BackEndISR() was called outside    */
            /* interrupt context.  The problem would occur when the sequencer */
            /* post more completed SCBs in the done queue between the time    */
            /* the FrontEndISR() executed and the last completed SCB          */
            /* (accounted by HIM in FrontEndISR()) was post back to upper     */
            /* layer in this routine.  CHIM will continues to post back all   */
            /* new completed SCBs and cleared the CMDCMPLT bit.  However, the */
            /* hardware interrupt is still pending out there.  Then when the  */
            /* pending interrupt interrupted, the FrontEndISR() will report   */
            /* back a value of 0 to the OSM saying this interrupt wasn't for  */
            /* us.  This will create a spurious interrupt and for some OSes   */
            /* like OS/2, will disable that interrupt in the PIC until the    */
            /* next system boot. */
            if (!hhcb->SCSI_HP_qoutcnt)
            {
               /* return(retValue); cant return because inline */
               break; /* break instead of return because inline */
            }
#endif
         }

         if (hhcb->SCSI_HP_qoutcnt)
         {
            /* we have serviced more command complete than we should */
            /* better clear the clear command interrupt */
            scsiRegister = hhcb->scsiRegister;
            OSD_OUTEXACT8(SCSI_AICREG(SCSI_CLRINT),SCSI_CMDCMPLT);
            hhcb->SCSI_HP_qoutcnt = 0;
         }
         else
         {
            break;
         }
      }

      /* return(retValue); dont need a return value */
/************************************************
   SCSIhCmdComplete() end
*************************************************/
   }

   /* check if there was any exceptional interrupt */
#if SCSI_CHECK_PCI_ERROR
   if(error)
   {
      SCSIhClearPCIError(hhcb);
      /* unpause chip (restore interrupt enable bit) */
      SCSI_hWRITE_HCNTRL(hhcb,scsiRegister,
                         (SCSI_UEXACT8)(hhcb->SCSI_HP_hcntrl & SCSI_INTEN));
   }
#endif /* #if SCSI_CHECK_PCI_ERROR */

   if (intstat & (SCSI_BRKINT | SCSI_SCSIINT | SCSI_SEQINT))
   {
      exceptionCount = 0;

      while ((intstat = SCSI_hREAD_INTSTAT(hhcb,scsiRegister)) &
         (SCSI_BRKINT | SCSI_SCSIINT | SCSI_SEQINT))
      {
         /* If the number of exceptions exceeds 10, then assume that it is */
         /* spurious and recover from it. */
         exceptionCount++;
         if (exceptionCount > 10)
         {
            exceptionCount = 0;
            SCSIhBadSeq(hhcb);
            break;
         }

         /* For Cardbus (APA-1480x), the instat register value is 0xFF */
         /* when the card is hot removed from the bus.  For this case, */
         /* CHIM must break out the loop.  This value is not going to  */
         /* happen on the scsi host adapter. */
         if (intstat == 0xFF)
         {
            break;
         }
      
         SCSI_hSEQSTRETCHWORKAROUND(hhcb); /* sequencer stretch workaround */

         /* Since there is exceptional interrupt, CHIM might already process  */
         /* all completed HIOB.  It is good idea to clear the SCB_DONE_AVAIL  */
         /* bit. So, the sequencer should not generate the cmdcmplt interrupt.*/
         /* Note that this is meaningful only for swapping_advanced_enhanced  */
         /* sequencer. */
         SCSI_hSETINTRTHRESHOLD(hhcb, SCSI_hGETINTRTHRESHOLD(hhcb));

         if ((sstat1 = OSD_INEXACT8(SCSI_AICREG(SCSI_SSTAT1))) & SCSI_SCSIRSTI)
         {
            /* scsi reset interrupt */
            SCSIhIntSrst(hhcb);
#if SCSI_TARGET_OPERATION  
/* Need to revisit this change. SCSI_CLRSCSIRSTI is cleared in
 * SCSIhIntSrst.
 */
            /* clear SCSI interrupt */
            OSD_OUTEXACT8(SCSI_AICREG(SCSI_CLRSINT1),SCSI_CLRSCSIRSTI);
#endif /* SCSI_TARGET_OPERATION */
            scbNumber = SCSI_NULL_SCB;
            continue;
         }
#if (SCSI_AICBAYONET || SCSI_AICTRIDENT)
         else if (OSD_INEXACT8(SCSI_AICREG(SCSI_SSTAT0)) & SCSI_BAYONET_IOERR)
         {
            /* io error interrupt */
            SCSIhIntIOError(hhcb);
            scbNumber = SCSI_NULL_SCB;
            continue;
         }
#endif /* SCSI_AICBAYONET || SCSI_AICTRIDENT */
         else
         {
            if (sstat1 & SCSI_SELTO)      /* handle SCSI selection timout */
            {
               scbNumber = OSD_INEXACT8(SCSI_AICREG(SCSI_hWAITING_SCB(hhcb)));
            }
            else
            {
               scbNumber = OSD_INEXACT8(SCSI_AICREG(SCSI_hACTIVE_SCB(hhcb)));
            }
         }

         if (scbNumber == SCSI_NULL_SCB)
         {
            hiob = SCSI_NULL_HIOB;
         }
         else
         {
            hiob = SCSI_ACTPTR[scbNumber];
         }

         if (intstat & SCSI_SEQINT)
         {
            /* Process sequencer interrupt */
            intstat &= ~SCSI_SEQINT;
            intcode = intstat & SCSI_INTCODE;
            if ((scbNumber == SCSI_NULL_SCB) || 
                ((hiob == SCSI_NULL_HIOB) && 
                 ((intcode != SCSI_CDB_XFER_PROBLEM) &&
                  (intcode != SCSI_AUTO_RATE_OFFSET))))
            {
               /* If hiob is SCSI_NULL_HIOB then don't enter any routine     */
               /* which doesn't have protection on hiob parameter.           */
               /* Abort only if it is not a break point interrupt at idle    */
               /* loop, as an scb can be null in this case                   */
               if (intcode != SCSI_IDLE_LOOP_BREAK)
               {
                  intcode = SCSI_ABORT_TARGET;
               }
            }

            /* process all the sequencer interrupts */
            switch (intcode)
            {
               case SCSI_DATA_OVERRUN:         /* data overrun/underrun */
                  SCSIhCheckLength(hhcb,hiob);
                  break;

               case SCSI_CDB_XFER_PROBLEM:     /* cdb bad transfer */
                  SCSIhCdbAbort(hhcb);
                  break;

               case SCSI_HANDLE_MSG_OUT:       /* send msg out */
                  SCSIhHandleMsgOut(hhcb,hiob);
                  break;

               case SCSI_SYNC_NEGO_NEEDED:
                  SCSI_hNEGOTIATE(hhcb,hiob);
                  break;

               case SCSI_CHECK_CONDX:
                  SCSIhCheckCondition(hhcb,hiob);
                  break;

               case SCSI_PHASE_ERROR:
                  SCSIhBadSeq(hhcb);
                  break;

               case SCSI_EXTENDED_MSG:
                  SCSIhExtMsgi(hhcb,hiob);
                  break;

               case SCSI_UNKNOWN_MSG:
                  SCSIhHandleMsgIn(hhcb,hiob);
                  break;

               case SCSI_ABORT_TARGET:
                  SCSIhAbortTarget(hhcb,hiob);
                  break;

               case SCSI_NO_ID_MSG:
                  SCSIhAbortTarget(hhcb,hiob);
                  break;

               case SCSI_IDLE_LOOP_BREAK:
               case SCSI_EXPANDER_BREAK:
                  SCSIhBreakInterrupt(hhcb,hiob);
                  break;

#if SCSI_TARGET_OPERATION  
               case SCSI_TARGET_BUS_HELD:
                  SCSIhTargetScsiBusHeld(hhcb,hiob);
                  break;

               case SCSI_TARGET_SPECIAL_FUNC: 
                  SCSIhTargetHandleSpecialFunction(hhcb,hiob);
                  break;

#endif  /* SCSI_TARGET_OPERATION */

               case SCSI_AUTO_RATE_OFFSET:
                  /* workaround for Auto Rate/Offset issue where */
                  /* the scsi rate/offset does not take affect   */
                  /* immediately for current connection. */
                  SCSI_hENABLEAUTORATEOPTION(hhcb);
                  break;

               default:
                  /* if we obtain an unknown sequencer interrupt
                   * assume something wrong.
                   */
                  SCSIhBadSeq(hhcb);
                  break; 
            }                    /* end of sequencer interrupt handling */

            /* clear SEQ interrupt */
            OSD_OUTEXACT8(SCSI_AICREG(SCSI_CLRINT),SCSI_CLRSEQINT);
         }

         /* process scsi interrupt */
         else if (intstat & SCSI_SCSIINT)
         {
            intstat &= ~SCSI_SCSIINT;

#if SCSI_SCAM_ENABLE == 2
            /* SCAM selection interrupt handled here:                   */
            /*                                                          */
            /* Must check SCAM selection before selection timeout since */
            /* selection timeout will be set during SCAM selection.     */
            if ((OSD_INEXACT8(SCSI_AICREG(SCSI_SCAMSTAT)) & SCSI_SCAMSELD) &&
                (hhcb->SCSI_HF_scamLevel == 2))
            {
               /* Need to clear selection timeout */
               OSD_OUTEXACT8(SCSI_AICREG(SCSI_CLRSINT1), SCSI_CLRSELTIMO);
               SCSIhScamSelection(hhcb,hiob);
            }
#if SCSI_TARGET_OPERATION  
            else if (((sstat0 = OSD_INEXACT8(SCSI_AICREG(SCSI_SSTAT0)))& SCSI_TARGET) &&
                     (sstat0 & SCSI_SELDI) && 
                     (OSD_INEXACT8(SCSI_AICREG(SCSI_SIMODE0)) & SCSI_ENSELDI))
            {    
                SCSIhTargetIntSelIn(hhcb);
            }
            else if ((sstat0 & SCSI_TARGET) && (sstat1 & SCSI_ATNTARG) &&
                     (OSD_INEXACT8(SCSI_AICREG(SCSI_SIMODE1)) & SCSI_ENATNTARG))
            {
                SCSIhTargetATNIntr(hhcb,hiob);
            }
            else if (hiob != SCSI_NULL_HIOB && hiob->SCSI_IP_targetMode)
            {
                if ((sstat1 & SCSI_SELTO))
                { 
                   SCSIhTargetIntSelto(hhcb,hiob);  /* selection time out */
                }
                else if (sstat1 & SCSI_SCSIPERR)
                {
                   SCSIhTargetParityError(hhcb,hiob); /* parity error */
                } 
            }
#endif /* SCSI_TARGET_OPERATION */
            /* Selection timeout must be checked before bus free since */
            /* bus free status is set due to a selection timeout. */
            else if (sstat1 & SCSI_SELTO)
               SCSIhIntSelto(hhcb,hiob);        /* selection time out */
#else
#if SCSI_TARGET_OPERATION  
            if (((sstat0=OSD_INEXACT8(SCSI_AICREG(SCSI_SSTAT0)))& SCSI_TARGET) &&
                (sstat0 & SCSI_SELDI) && 
                (OSD_INEXACT8(SCSI_AICREG(SCSI_SIMODE0)) & SCSI_ENSELDI)) 
            {    
               SCSIhTargetIntSelIn(hhcb);
            }
            else if ((sstat0 & SCSI_TARGET) && (sstat1 & SCSI_ATNTARG) &&
                     (OSD_INEXACT8(SCSI_AICREG(SCSI_SIMODE1)) & SCSI_ENATNTARG))
            {
               SCSIhTargetATNIntr(hhcb,hiob);
            } 
            else if (hiob != SCSI_NULL_HIOB && hiob->SCSI_IP_targetMode)
            {
                if ((sstat1 & SCSI_SELTO))
                   SCSIhTargetIntSelto(hhcb,hiob);  /* selection time out */
                else if (sstat1 & SCSI_SCSIPERR)
                   SCSIhTargetParityError(hhcb,hiob); /* parity error */
            }
            else 
#endif /* SCSI_TARGET_OPERATION */
            if (sstat1 & SCSI_SELTO)
               SCSIhIntSelto(hhcb,hiob);        /* selection time out */

#endif   /* (#ifdef   SCSI_SCAM_ENABLE) */

            else if (sstat1 & SCSI_BUSFREE)
               SCSIhIntFree(hhcb,hiob); /* unexpected BUSFREE interrupt */

            else if (sstat1 & SCSI_SCSIPERR)
               SCSIhParityError(hhcb,hiob);     /* parity error */
            else if (sstat1 & SCSI_PHASEMIS)
            {
               OSD_OUTEXACT8(SCSI_AICREG(SCSI_SCSISIG),
                  (SCSI_UEXACT8)(OSD_INEXACT8(SCSI_AICREG(SCSI_SCSISIG)) & SCSI_BUSPHASE));
               SCSIhBadSeq(hhcb);
            }
            else if (sstat1 & SCSI_PHASECHG)
            {
               OSD_OUTEXACT8(SCSI_AICREG(SCSI_CLRSINT1),SCSI_CLRPHASECHG);
            }

            /* clear SCSI interrupt */
            OSD_OUTEXACT8(SCSI_AICREG(SCSI_CLRINT),SCSI_CLRSCSINT);
         }

         /* process sequencer breakpoint */
         else if (intstat & SCSI_BRKINT)
         {
            intstat &= ~SCSI_BRKINT;

            SCSIhBreakInterrupt(hhcb,hiob);          /* just for debug  */

            /* clear SCSI interrupt */
            OSD_OUTEXACT8(SCSI_AICREG(SCSI_CLRINT),SCSI_CLRBRKINT);
         }

         /* unpause chip (restore interrupt enable bit) */
         SCSI_hWRITE_HCNTRL(hhcb,scsiRegister,(SCSI_UEXACT8)(hhcb->SCSI_HP_hcntrl & SCSI_INTEN));
      }
   }

   OSD_SYNCHRONIZE_IOS(hhcb);
/************************************************
   SCSIHBackEndISR() end
*************************************************/

   /* call to SCSIrPostDoneQueue(hhcb); */
/************************************************
   SCSIrPostDoneQueue() start
*************************************************/
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
      /* call to OSD_COMPLETE_HIOB(hhcb->SCSI_DONE_QUEUE[origTail]); */
/************************************************
   OSDCompleteHIOB() start
*************************************************/
      hiob = hhcb->SCSI_DONE_QUEUE[origTail];

      iob = SCSI_GETIOB(hiob);

      /* process normal command first */
      if (hiob->stat == SCSI_SCB_COMP)
      {
         /* process successful status */
         iob->taskStatus = HIM_IOB_GOOD;
      }
      else
      {
         /* translate error status */
         /* call to - SCSIxTranslateError(iob,hiob); */
/************************************************
   SCSIxTranslateError() start
*************************************************/
         /* interpret stat of hiob */ 
         switch(hiob->stat)
         {
            case SCSI_SCB_ERR:
               /* interpret haStat of hiob */
               switch(hiob->haStat)
               {
                  case SCSI_HOST_SEL_TO:
                     iob->taskStatus = HIM_IOB_NO_RESPONSE;
                     break;

                  case SCSI_HOST_BUS_FREE:
                     iob->taskStatus = HIM_IOB_CONNECTION_FAILED;
                     break;

                  case SCSI_HOST_PHASE_ERR:
                  case SCSI_HOST_HW_ERROR:    
                     iob->taskStatus = HIM_IOB_HOST_ADAPTER_FAILURE;
                     break;

                  case SCSI_HOST_SNS_FAIL:
                     iob->taskStatus = HIM_IOB_ERRORDATA_FAILED;
                     break;

                  case SCSI_HOST_DETECTED_ERR:
                     iob->taskStatus = HIM_IOB_PARITY_ERROR;
                     break;

                  case SCSI_HOST_DU_DO:
                  case SCSI_HOST_NO_STATUS:
                     /* interpret trgStatus of hiob */
                     switch(hiob->trgStatus)
                     {
                        case SCSI_UNIT_CHECK:
                           if (iob->flagsIob.autoSense)
                           {
                              /* Set return status */
                              iob->taskStatus = HIM_IOB_ERRORDATA_VALID;

                              /* Get residual sense length */
                              iob->residualError = (HIM_UEXACT32) hiob->snsResidual;

                              if (iob->errorDataLength > 255)
                              {
                                 iob->residualError += (iob->errorDataLength - 255);
                              }
                           }
                           else
                           {
                              iob->taskStatus = HIM_IOB_ERRORDATA_REQUIRED;
                           }

                           if (hiob->haStat == SCSI_HOST_DU_DO)
                           {
                              iob->residual = hiob->residualLength;
                           }
                           break;

                        case SCSI_UNIT_BUSY:
                           iob->taskStatus = HIM_IOB_BUSY;
                           break;

                        case SCSI_UNIT_RESERV:
                           iob->taskStatus = HIM_IOB_TARGET_RESERVED;
                           break;

                        case SCSI_UNIT_QUEFULL:
                           iob->taskStatus = HIM_IOB_TASK_SET_FULL;
                           break;

                        case SCSI_UNIT_GOOD:
                        case SCSI_UNIT_MET:
                        case SCSI_UNIT_INTERMED:
                        case SCSI_UNIT_INTMED_GD:
                           if (hiob->haStat == SCSI_HOST_DU_DO)
                           {
                              iob->taskStatus = HIM_IOB_DATA_OVERUNDERRUN;
                              iob->residual = hiob->residualLength;
                           }
                           else
                           {
                              iob->taskStatus = HIM_IOB_GOOD;
                           }
                           break;

                        default:
                           /* should never come to here */
                           iob->taskStatus = HIM_IOB_PROTOCOL_SPECIFIC;
                           break;
                     }
                     break;

                  case SCSI_HOST_TAG_REJ:
                  case SCSI_HOST_ABT_FAIL:
                  case SCSI_HOST_RST_HA:   
                  case SCSI_HOST_RST_OTHER:
                     /* not implemented error */
                     /* should never come to here */
                     break;

                  case SCSI_HOST_ABT_NOT_FND:
                     iob->taskStatus = HIM_IOB_ABORT_NOT_FOUND;
                     break;

                  case SCSI_HOST_ABT_CMDDONE:
                     iob->taskStatus = HIM_IOB_ABORT_ALREADY_DONE;
                     break;

                  case SCSI_HOST_NOAVL_INDEX: 
                  case SCSI_HOST_INV_LINK:
                     /* not implemented error */
                     /* should never come to here */
                     break;

#if SCSI_TARGET_OPERATION
                  case SCSI_INITIATOR_PARITY_MSG:
                     /* Initiator message parity message */ 
                     iob->taskStatus = HIM_IOB_INITIATOR_DETECTED_PARITY_ERROR;
                     break;
                
                  case SCSI_INITIATOR_PARITY_ERR_MSG:
                     /* Initiator detected error message*/ 
                     iob->taskStatus = HIM_IOB_INITIATOR_DETECTED_ERROR;
                     break;        

                  case SCSI_HOST_MSG_REJECT: 
                     /* Initiator rejected a message which we expected to be OK. */
                     iob->taskStatus = HIM_IOB_INVALID_MESSAGE_REJECT;
                     break; 
                                         
                  case SCSI_INITIATOR_INVALID_MSG:
                     /* Invalid message recieved from initiator. After the */
                     /* HIM issued a Message Reject message in response    */
                     /* to an unrecognized or unsupported message the      */
                     /* initiator released the ATN signal.                 */ 
                     iob->taskStatus = HIM_IOB_INVALID_MESSAGE_RCVD;
                     break;
                            
                  case SCSI_HOST_IDENT_RSVD_BITS:
                     /* Identify Message has reserved bits set. XLM should */
                     /* never get this error code. HWM uses this code to   */
                     /* indicate that a reserved bit is set in the         */
                     /* Identify message and the adapter profile field,    */
                     /* AP_SCSI2_IdentifyMsgRsv, is set to 0, indicating   */
                     /* that the HWM is to issue a message reject. Only    */
                     /* applies to SCSI2.                                  */
                     break;

                  case SCSI_HOST_IDENT_LUNTAR_BIT:
                     /* Identify Message has the luntar bit set. XLM       */
                     /* should never get this error code. HWM uses this    */
                     /* code to indicate that the luntar bit is set in the */
                     /* Identify message and the adapter profile field,    */
                     /* AP_SCSI2_TargetRejectLuntar, is set to 1,          */
                     /* indicating that the HWM is to issue a message      */
                     /* reject. Only applies to SCSI2.                     */
                     break; 

                  case SCSI_HOST_TRUNC_CMD: 
                     /* Truncated SCSI Command */
                     iob->taskStatus = HIM_IOB_TARGETCOMMANDBUFFER_OVERRUN; 
                     break;
#endif /* SCSI_TARGET_OPERATION */

                  case SCSI_HOST_ABT_CHANNEL_FAILED:
                     iob->taskStatus = HIM_IOB_ABORTED_CHANNEL_FAILED;
                     break;

                  default:
                     /* should never come to here */
                     break;
               }
               break;

            case SCSI_SCB_ABORTED:
               /* interpret haStat of hiob */
               switch(hiob->haStat)
               {
                  case SCSI_HOST_ABT_HOST:
                     iob->taskStatus = HIM_IOB_ABORTED_ON_REQUEST;
                     break;

                  case SCSI_HOST_ABT_HA:
                     iob->taskStatus = HIM_IOB_ABORTED_CHIM_RESET;
                     break;

                  case SCSI_HOST_ABT_BUS_RST:
                     iob->taskStatus = HIM_IOB_ABORTED_REQ_BUS_RESET;
                     break;

                  case SCSI_HOST_ABT_3RD_RST:
                     iob->taskStatus = HIM_IOB_ABORTED_3RD_PARTY_RESET;
                     break;

                  case SCSI_HOST_ABT_IOERR:
                     iob->taskStatus = HIM_IOB_ABORTED_TRANSPORT_MODE_CHANGE;
                     break;

                  case SCSI_HOST_ABT_TRG_RST:
                     iob->taskStatus = HIM_IOB_ABORTED_REQ_TARGET_RESET;
                     break;

                  case SCSI_HOST_PHASE_ERR:
                  case SCSI_HOST_HW_ERROR:    
                     iob->taskStatus = HIM_IOB_HOST_ADAPTER_FAILURE;
                     break;

#if SCSI_TARGET_OPERATION
                  /* Target mode HIOB Abort reasons */
                  case SCSI_HOST_TARGET_RESET:
                     iob->taskStatus = HIM_IOB_ABORTED_TR_RECVD;
                     break;

                  case SCSI_HOST_ABORT_TASK:
                     iob->taskStatus = HIM_IOB_ABORTED_ABT_RECVD;
                     break;

                  case SCSI_HOST_ABORT_TASK_SET:
                     iob->taskStatus = HIM_IOB_ABORTED_ABTS_RECVD;
                     break;

                  case SCSI_HOST_CLEAR_TASK_SET:
                     iob->taskStatus = HIM_IOB_ABORTED_CTS_RECVD;
                     break;
            
                  case SCSI_HOST_TERMINATE_TASK:
                     iob->taskStatus = HIM_IOB_ABORTED_TT_RECVD;
                     break;
            
#endif /* SCSI_TARGET_OPERATION */

                  default:
                     /* should never come to here */
                     break;
               }
               break;

            case SCSI_SCB_PENDING:
               if (hiob->haStat == SCSI_HOST_ABT_STARTED)
               {
                  iob->taskStatus = HIM_IOB_ABORT_STARTED;
               }
               break;

            default:
               /* SCSI_SCB_COMP is not processed here */
               /* SCSI_TASK_CMD_COMP is not processed here */
               /* SCSI_SCB_INV_CMD should never happen for this implementation */
               /* we should never get to here */
               break;
         }
/************************************************
   SCSIxTranslateError() end
*************************************************/

      }

      iob->postRoutine(iob);
/************************************************
   OSDCompleteHIOB() end
*************************************************/
   }
/************************************************
   SCSIrPostDoneQueue() end
*************************************************/
/************************************************
   SCSIRBackEndISR() end
*************************************************/

}
#endif /* SCSI_STREAMLINE_QIOPATH */

/***************************************************************************
*                                                                          *
* Copyright 1995,1996,1997,1998,1999 Adaptec, Inc., All Rights Reserved.   *
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
*  Module Name:   XLMUTIL.C
*
*  Description:
*                 Codes to implement run time Translation Management Layer
*                 protocol.
*
*  Owners:        ECX IC Firmware Team
*    
*  Notes:         NONE
*
*  Entry Point(s):
*
***************************************************************************/

#include "scsi.h"
#include "xlm.h"

/*********************************************************************
*
*  OSDBuildSCB
*
*     Build scb buffer
*
*  Return Value:  void
*                  
*  Parameters:    exercise
*
*  Remarks:       This routine is defined for internal HIM reference.
*
*********************************************************************/
void OSDBuildSCB (SCSI_HIOB SCSI_IPTR hiob)
{
   HIM_IOB HIM_PTR iob = SCSI_GETIOB(hiob);
   SCSI_TARGET_TSCB HIM_PTR targetTSH;
   SCSI_ADAPTER_TSCB HIM_PTR adapterTSH;

#if SCSI_DMA_SUPPORT
   if (iob->function == HIM_INITIATE_DMA_TASK)
   {
      /* the iob->taskSetHandle is an adapterTSH */
      /* need to get the specially reserved targetTSCB */
      adapterTSH = SCSI_ADPTSH(iob->taskSetHandle);
      targetTSH = &adapterTSH->dmaTargetTSCB;
   }
   else
#endif /* SCSI_DMA_SUPPORT */
   {
      targetTSH = SCSI_TRGTSH(iob->taskSetHandle);
      adapterTSH = SCSI_ADPTSH(targetTSH->adapterTSH);
   }

   /* invoke the proper routine to build SCB */
   adapterTSH->OSDBuildSCB(hiob,iob,targetTSH,adapterTSH);
}

/*********************************************************************
*
*  OSDStandard64BuildSCB
*
*     Build scb buffer for standard 64 mode 
*
*  Return Value:  void
*                  
*  Parameters:    hiob
*
*  Remarks:       This routine is defined for internal HIM reference
*
*********************************************************************/
#if SCSI_STANDARD64_MODE
void OSDStandard64BuildSCB (SCSI_HIOB HIM_PTR hiob,
                            HIM_IOB HIM_PTR iob,
                            SCSI_TARGET_TSCB HIM_PTR targetTSH,
                            SCSI_ADAPTER_TSCB HIM_PTR adapterTSH)
{
   SCSI_IOB_RESERVE HIM_PTR iobReserve =
                              (SCSI_IOB_RESERVE HIM_PTR)iob->iobReserve.virtualAddress;
   SCSI_SCB_STANDARD64 HIM_PTR scbBuffer =
         (SCSI_SCB_STANDARD64 HIM_PTR) hiob->scbDescriptor->scbBuffer.virtualAddress;
   SCSI_BUS_ADDRESS busAddress;
   SCSI_UEXACT32 sgData;
   SCSI_UEXACT8 cdbLength;
   SCSI_UEXACT8 scontrol;

   /* clear the chain control */
   HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
       OSMoffsetof(SCSI_SCB_STANDARD64,chain_control),0);

   scontrol = 0;     /* Assume disconnect is disabled */
   if (!hiob->SCSI_IF_disallowDisconnect)
   {
      scontrol = SCSI_DISCENB;   /* Enable disconnect */

      /* We'll set tag to enable only if the disconnect was enable */
      if (hiob->SCSI_IF_tagEnable)
      {
         scontrol |= SCSIscontrol[iob->taskAttribute];
      }
   }

   HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
       OSMoffsetof(SCSI_SCB_STANDARD64,starget),
       (SCSI_UEXACT8) targetTSH->scsiID);
   HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
       OSMoffsetof(SCSI_SCB_STANDARD64,slun),
       (SCSI_UEXACT8) targetTSH->lunID);

   cdbLength = (SCSI_UEXACT8) iob->targetCommandLength;

   if (!iob->data.bufferSize)
   {
      /* indicate no data transfer */
      cdbLength |= SCSI_S64_NODATA;
   }
   else
   {
      /* get segment length */
      HIM_GET_LITTLE_ENDIAN32(adapterTSH->osmAdapterContext,&sgData,
          iob->data.virtualAddress,sizeof(SCSI_BUS_ADDRESS));

      /* to handle one segment and zero in length passed in */
      if (sgData == (SCSI_UEXACT32)0x80000000)
      {
         /* indicate no data transfer */
         cdbLength |= SCSI_S64_NODATA;
      }
      else
      {
         /* 1st s/g element should be done like this */
         /* setup the embedded s/g segment (the first s/g segment) */
         if (sgData & (SCSI_UEXACT32)0x80000000)
         {
            cdbLength |= SCSI_S64_ONESGSEG;
         }
         else
         {
            /* more than one s/g segments and need to setup s/g pointer */
            SCSI_hPUTBUSADDRESS(&adapterTSH->hhcb,scbBuffer,
                 OSMoffsetof(SCSI_SCB_STANDARD64,SCSI_S64_sg_pointer_SCB0),
                 iob->data.busAddress);

            /* reject modify data pointer setting */
            /* HQ remove this 
            scontrol |= SCSI_S64_RJTMDP;
            */     
         }

         /* write segment length */
         HIM_PUT_LITTLE_ENDIAN24(adapterTSH->osmAdapterContext,scbBuffer,
             OSMoffsetof(SCSI_SCB_STANDARD64,slength0),sgData);

         /* write segment address */
         SCSI_hGETBUSADDRESS(&adapterTSH->hhcb,&busAddress,
              iob->data.virtualAddress,0);
         SCSI_hPUTBUSADDRESS(&adapterTSH->hhcb,scbBuffer,
              OSMoffsetof(SCSI_SCB_STANDARD64,saddress0),busAddress);
                                       
         HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
             OSMoffsetof(SCSI_SCB_STANDARD64,SCSI_S64_sg_cache_SCB),
             2 * sizeof(SCSI_BUS_ADDRESS));
      }    
   }

   HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
       OSMoffsetof(SCSI_SCB_STANDARD64,scdb_length),cdbLength);

   if (iob->targetCommandLength <= 12)
   {
      /* cdb embedded in scb buffer */
      HIM_PUT_BYTE_STRING(adapterTSH->osmAdapterContext,scbBuffer,
          OSMoffsetof(SCSI_SCB_STANDARD64,SCSI_S64_scdb0),
          iob->targetCommand,iob->targetCommandLength);
   }
   else
   {
      /* cdb pointer embedded in scb buffer */
      HIM_PUT_BYTE_STRING(adapterTSH->osmAdapterContext,iobReserve->cdb,
          0,iob->targetCommand,iob->targetCommandLength);
      busAddress = iob->iobReserve.busAddress;
      OSMxAdjustBusAddress(&busAddress,OSMoffsetof(SCSI_IOB_RESERVE,cdb[0]));
      SCSI_hPUTBUSADDRESS(&adapterTSH->hhcb,scbBuffer,
           OSMoffsetof(SCSI_SCB_STANDARD64,SCSI_S64_scdb0),busAddress);
   }

   HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
       OSMoffsetof(SCSI_SCB_STANDARD64,scontrol),scontrol);

   HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
       OSMoffsetof(SCSI_SCB_STANDARD64,SCSI_S64_array_site_next),hiob->scbDescriptor->queueNext->scbNumber);
   SCSI_hPUTBUSADDRESS(&adapterTSH->hhcb,scbBuffer,
        OSMoffsetof(SCSI_SCB_STANDARD64,next_SCB_addr0),hiob->scbDescriptor->queueNext->scbBuffer.busAddress);

   /* flush the cache ready for dma */
   SCSI_FLUSH_CACHE(scbBuffer,SCSI_S64_SIZE_SCB);
}
#endif

/*********************************************************************
*
*  OSDSwapping32BuildSCB
*
*     Build scb buffer for test mode 
*
*  Return Value:  void
*                  
*  Parameters:    hiob
*
*  Remarks:       This routine is defined for internal HIM reference
*
*********************************************************************/
#if SCSI_SWAPPING32_MODE
void OSDSwapping32BuildSCB (SCSI_HIOB HIM_PTR hiob,
                            HIM_IOB HIM_PTR iob,
                            SCSI_TARGET_TSCB HIM_PTR targetTSH,
                            SCSI_ADAPTER_TSCB HIM_PTR adapterTSH)
{
   SCSI_IOB_RESERVE HIM_PTR iobReserve =
                  (SCSI_IOB_RESERVE HIM_PTR)iob->iobReserve.virtualAddress;
   SCSI_HHCB HIM_PTR hhcb = &adapterTSH->hhcb;
   SCSI_SCB_SWAPPING32 HIM_PTR scbBuffer =
         (SCSI_SCB_SWAPPING32 HIM_PTR) hiob->scbDescriptor->scbBuffer.virtualAddress;
   SCSI_UEXACT32 sgData;
   SCSI_BUS_ADDRESS busAddress;
   SCSI_UEXACT8 cdbLength;
   SCSI_UEXACT8 scontrol;
   SCSI_DEVICE SCSI_DPTR deviceTable = 
         &SCSI_DEVICE_TABLE(hhcb)[(SCSI_UEXACT8)SCSI_TRGTSH(targetTSH)->scsiID];

   /* clear the chain control */
   HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
       OSMoffsetof(SCSI_SCB_SWAPPING32,chain_control),0);

   scontrol = 0;     /* Assume disconnect is disabled */
   if ((deviceTable->SCSI_DF_disconnectEnable) &&
       (!hiob->SCSI_IF_disallowDisconnect))
   {
      scontrol = SCSI_DISCENB;   /* Enable disconnect */

      /* We'll turn on tagged command only if the disconnect is enable */
      if (hiob->SCSI_IF_tagEnable)
      {
         scontrol |= SCSIscontrol[iob->taskAttribute];
      }
   }

   HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
       OSMoffsetof(SCSI_SCB_SWAPPING32,starget),(SCSI_UEXACT8) targetTSH->scsiID);

   HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
       OSMoffsetof(SCSI_SCB_SWAPPING32,slun),(SCSI_UEXACT8) targetTSH->lunID);

   cdbLength = (SCSI_UEXACT8) iob->targetCommandLength;

   if (!iob->data.bufferSize)
   {
      /* indicate no data transfer */
      cdbLength |= SCSI_W32_NODATA;
   }
   else
   {
      /* get segment length */
      HIM_GET_LITTLE_ENDIAN32(adapterTSH->osmAdapterContext,&sgData,
          iob->data.virtualAddress,sizeof(SCSI_BUS_ADDRESS));
          
      /* to handle one segment and zero in length passed in */
      if (sgData == (SCSI_UEXACT32)0x80000000)
      {
         /* indicate no data transfer */
         cdbLength |= SCSI_W32_NODATA;
      }
      else
      {
         /* 1st s/g element should be done like this */
         /* setup the embedded s/g segment (the first s/g segment) */
         if (sgData & (SCSI_UEXACT32)0x80000000)
         {
            /* there is only one sg element in sg list */
            cdbLength |= SCSI_W32_ONESGSEG;
         }
         else
         {
            /* setup the sg pointer */
            busAddress = iob->data.busAddress;
            OSMxAdjustBusAddress(&busAddress,8);
                        
            HIM_PUT_LITTLE_ENDIAN32(adapterTSH->osmAdapterContext,scbBuffer,
                OSMoffsetof(SCSI_SCB_SWAPPING32,SCSI_W32_sg_pointer_SCB0),busAddress);
            
            
            /* reject modify data pointer setting */
            /* HQ remove this
         scontrol |= SCSI_W32_RJTMDP;
            */          
         }

         /* setup embedded sg segment length */
         HIM_PUT_LITTLE_ENDIAN24(adapterTSH->osmAdapterContext,scbBuffer,
             OSMoffsetof(SCSI_SCB_SWAPPING32,slength0),sgData);

         /* setup embedded sg segment address */
         HIM_GET_LITTLE_ENDIAN32(adapterTSH->osmAdapterContext,&sgData,
             iob->data.virtualAddress,0);
         HIM_PUT_LITTLE_ENDIAN32(adapterTSH->osmAdapterContext,scbBuffer,
             OSMoffsetof(SCSI_SCB_SWAPPING32,saddress0),sgData);
      }    
   }

   HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
       OSMoffsetof(SCSI_SCB_SWAPPING32,scdb_length),cdbLength);

   if (iob->targetCommandLength <= 12)
   {
      /* cdb embedded in scb buffer */
      HIM_PUT_BYTE_STRING(adapterTSH->osmAdapterContext,scbBuffer,
          OSMoffsetof(SCSI_SCB_SWAPPING32,SCSI_W32_scdb0),
          iob->targetCommand,iob->targetCommandLength);
   }
   else
   {
      /* copy cdb and have cdb pointer embedded in scb buffer */
      HIM_PUT_BYTE_STRING(adapterTSH->osmAdapterContext,iobReserve->cdb,0,
          iob->targetCommand,iob->targetCommandLength);
      busAddress = iob->iobReserve.busAddress;
      OSMxAdjustBusAddress(&busAddress,OSMoffsetof(SCSI_IOB_RESERVE,cdb[0]));
                  
      HIM_PUT_LITTLE_ENDIAN32(adapterTSH->osmAdapterContext,scbBuffer,
          OSMoffsetof(SCSI_SCB_SWAPPING32,SCSI_W32_scdb_pointer0),
          busAddress);
      
   }

   HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
       OSMoffsetof(SCSI_SCB_SWAPPING32,next_SCB),hiob->scbDescriptor->queueNext->scbNumber);

#if SCSI_PARITY_PER_IOB
   if (hiob->SCSI_IF_parityEnable)
   {
      scontrol |= SCSI_W32_ENPAR;
   }
#endif

   HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
       OSMoffsetof(SCSI_SCB_SWAPPING32,scontrol),scontrol);

   /* flush the cache ready for dma */
   SCSI_FLUSH_CACHE(scbBuffer,sizeof(SCSI_SCB_SWAPPING32));
}
#endif


/*********************************************************************
*
*  OSDSwapping64BuildSCB
*
*     Build scb buffer for test mode 
*
*  Return Value:  void
*                  
*  Parameters:    hiob
*
*  Remarks:       This routine is defined for internal HIM reference
*
*********************************************************************/
#if SCSI_SWAPPING64_MODE
void OSDSwapping64BuildSCB (SCSI_HIOB HIM_PTR hiob,
                            HIM_IOB HIM_PTR iob,
                            SCSI_TARGET_TSCB HIM_PTR targetTSH,
                            SCSI_ADAPTER_TSCB HIM_PTR adapterTSH)
{
   SCSI_IOB_RESERVE HIM_PTR iobReserve =
                  (SCSI_IOB_RESERVE HIM_PTR)iob->iobReserve.virtualAddress;
   SCSI_HHCB HIM_PTR hhcb = &adapterTSH->hhcb;
   SCSI_SCB_SWAPPING64 HIM_PTR scbBuffer =
         (SCSI_SCB_SWAPPING64 HIM_PTR) hiob->scbDescriptor->scbBuffer.virtualAddress;
   SCSI_UEXACT32 sgData;
   SCSI_BUS_ADDRESS busAddress;
   SCSI_UEXACT8 cdbLength;
   SCSI_UEXACT8 scontrol;
   SCSI_DEVICE SCSI_DPTR deviceTable = 
         &SCSI_DEVICE_TABLE(hhcb)[(SCSI_UEXACT8)SCSI_TRGTSH(targetTSH)->scsiID];

   /* clear the chain control */
   HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
       OSMoffsetof(SCSI_SCB_SWAPPING64,chain_control),0);

   scontrol = 0;     /* Assume disconnect is disabled */
   if ((deviceTable->SCSI_DF_disconnectEnable) &&
       (!hiob->SCSI_IF_disallowDisconnect))
   {
      scontrol = SCSI_DISCENB;   /* Enable disconnect */

      /* We'll turn on tagged command only if the disconnect is enable */
      if (hiob->SCSI_IF_tagEnable)
      {
         scontrol |= SCSIscontrol[iob->taskAttribute];
      }
   }

   HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
       OSMoffsetof(SCSI_SCB_SWAPPING64,starget),(SCSI_UEXACT8) targetTSH->scsiID);

   HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
       OSMoffsetof(SCSI_SCB_SWAPPING64,slun),(SCSI_UEXACT8) targetTSH->lunID);

   cdbLength = (SCSI_UEXACT8) iob->targetCommandLength;

   if (!iob->data.bufferSize)
   {
      /* indicate no data transfer */
      cdbLength |= SCSI_W64_NODATA;
   }
   else
   {
      /* get segment length */
      HIM_GET_LITTLE_ENDIAN32(adapterTSH->osmAdapterContext,&sgData,
          iob->data.virtualAddress,sizeof(SCSI_BUS_ADDRESS));
          
      /* to handle one segment and zero in length passed in */
      if (sgData == (SCSI_UEXACT32)0x80000000)
      {
         /* indicate no data transfer */
         cdbLength |= SCSI_W64_NODATA;
      }
      else
      {
         /* 1st s/g element should be done like this */
         /* setup the embedded s/g segment (the first s/g segment) */
         if (sgData & (SCSI_UEXACT32)0x80000000)
         {
            /* there is only one sg element in sg list */
            cdbLength |= SCSI_W64_ONESGSEG;
         }
         else
         {
            /* setup the sg pointer */
            busAddress = iob->data.busAddress;
            OSMxAdjustBusAddress(&busAddress,16);
            HIM_PUT_LITTLE_ENDIAN64(adapterTSH->osmAdapterContext,scbBuffer,
                OSMoffsetof(SCSI_SCB_SWAPPING64,SCSI_W64_sg_pointer_SCB0),busAddress);

            /* reject modify data pointer setting */
            /* HQ remove this logic
         scontrol |= SCSI_W64_RJTMDP;
            */          
         }

         /* setup embedded sg segment length */
         HIM_PUT_LITTLE_ENDIAN24(adapterTSH->osmAdapterContext,scbBuffer,
             OSMoffsetof(SCSI_SCB_SWAPPING64,SCSI_W64_slength0),sgData);

         /* setup embedded sg segment address */
         HIM_GET_LITTLE_ENDIAN64(adapterTSH->osmAdapterContext,&busAddress,
             iob->data.virtualAddress,0);
         HIM_PUT_LITTLE_ENDIAN64(adapterTSH->osmAdapterContext,scbBuffer,
             OSMoffsetof(SCSI_SCB_SWAPPING64,SCSI_W64_saddress0),busAddress);
      }    
   }

   HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
       OSMoffsetof(SCSI_SCB_SWAPPING64,scdb_length),cdbLength);

   if (iob->targetCommandLength <= 12)
   {
      /* cdb embedded in scb buffer */
      HIM_PUT_BYTE_STRING(adapterTSH->osmAdapterContext,scbBuffer,
          OSMoffsetof(SCSI_SCB_SWAPPING64,SCSI_W64_scdb0),
          iob->targetCommand,iob->targetCommandLength);
   }
   else
   {
      /* copy cdb and have cdb pointer embedded in scb buffer */
      HIM_PUT_BYTE_STRING(adapterTSH->osmAdapterContext,iobReserve->cdb,0,
          iob->targetCommand,iob->targetCommandLength);
      busAddress = iob->iobReserve.busAddress;
      OSMxAdjustBusAddress(&busAddress,OSMoffsetof(SCSI_IOB_RESERVE,cdb[0]));
      HIM_PUT_LITTLE_ENDIAN64(adapterTSH->osmAdapterContext,scbBuffer,
          OSMoffsetof(SCSI_SCB_SWAPPING64,SCSI_W64_scdb_pointer0),
          busAddress);
   }

   HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
       OSMoffsetof(SCSI_SCB_SWAPPING64,scontrol),scontrol);

   HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
       OSMoffsetof(SCSI_SCB_SWAPPING64,next_SCB),hiob->scbDescriptor->queueNext->scbNumber);

   /* flush the cache ready for dma */
   SCSI_FLUSH_CACHE(scbBuffer,sizeof(SCSI_SCB_SWAPPING64));
}
#endif

/*********************************************************************
*
*  OSDStandardAdvBuildSCB
*
*     Build scb buffer for standard advanced mode 
*
*  Return Value:  void
*                  
*  Parameters:    hiob
*
*  Remarks:       This routine is defined for internal HIM reference
*
*********************************************************************/
#if SCSI_STANDARD_ADVANCED_MODE
void OSDStandardAdvBuildSCB (SCSI_HIOB HIM_PTR hiob,
                            HIM_IOB HIM_PTR iob,
                            SCSI_TARGET_TSCB HIM_PTR targetTSH,
                            SCSI_ADAPTER_TSCB HIM_PTR adapterTSH)
{
   SCSI_IOB_RESERVE HIM_PTR iobReserve =
                     (SCSI_IOB_RESERVE HIM_PTR)iob->iobReserve.virtualAddress;
   SCSI_SCB_STANDARD_ADVANCED HIM_PTR scbBuffer =
      (SCSI_SCB_STANDARD_ADVANCED HIM_PTR) hiob->scbDescriptor->scbBuffer.virtualAddress;
   SCSI_BUS_ADDRESS busAddress;
   SCSI_UEXACT32 sgData;
   SCSI_UEXACT8 sg_cache_SCB;
   SCSI_UEXACT8 scontrol;
   SCSI_UEXACT8 slun;

   /* clear the chain control */
   HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
       OSMoffsetof(SCSI_SCB_STANDARD_ADVANCED,chain_control),0);

   scontrol = 0;     /* Assume disconnect is disabled */
   slun = (SCSI_UEXACT8) targetTSH->lunID;
   if (!hiob->SCSI_IF_disallowDisconnect)
   {
      slun |= SCSI_DISCENB;   /* Enable disconnect */
      /* We'll set tag to enable only if the disconnect was enable */
      if (hiob->SCSI_IF_tagEnable)
      {
         scontrol |= SCSIscontrol[iob->taskAttribute];
      }
   }

   HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
       OSMoffsetof(SCSI_SCB_STANDARD_ADVANCED,starget),
       (SCSI_UEXACT8) targetTSH->scsiID);
   HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
       OSMoffsetof(SCSI_SCB_STANDARD_ADVANCED,slun),
       (SCSI_UEXACT8) slun);

   sg_cache_SCB = 2 * sizeof(SCSI_BUS_ADDRESS);

   if (!iob->data.bufferSize)
   {
      /* indicate no data transfer */
      sg_cache_SCB |= SCSI_SADV_NODATA;
   }
   else
   {
      /* get segment length */
      HIM_GET_LITTLE_ENDIAN32(adapterTSH->osmAdapterContext,&sgData,
          iob->data.virtualAddress,sizeof(SCSI_BUS_ADDRESS));

      /* to handle one segment and zero in length passed in */
      if (sgData == (SCSI_UEXACT32)0x80000000)
      {
         /* indicate no data transfer */
         sg_cache_SCB |= SCSI_SADV_NODATA;
      }
      else
      {
         /* 1st s/g element should be done like this */
         /* setup the embedded s/g segment (the first s/g segment) */
         if (sgData & (SCSI_UEXACT32)0x80000000)
         {
            sg_cache_SCB |= SCSI_SADV_ONESGSEG;
         }
         else
         {
            /* more than one s/g segments and need to setup s/g pointer */
            SCSI_hPUTBUSADDRESS(&adapterTSH->hhcb,scbBuffer,
                 OSMoffsetof(SCSI_SCB_STANDARD_ADVANCED,
                 SCSI_SADV_sg_pointer_SCB0),
                 iob->data.busAddress);

            /* reject modify data pointer setting */
            /* HQ remove this logic
         scontrol |= SCSI_SADV_RJTMDP;
            */     
         }

         /* write segment length */
         HIM_PUT_LITTLE_ENDIAN24(adapterTSH->osmAdapterContext,scbBuffer,
             OSMoffsetof(SCSI_SCB_STANDARD_ADVANCED,slength0),sgData);

         /* write segment address */
         SCSI_hGETBUSADDRESS(&adapterTSH->hhcb,&busAddress,
              iob->data.virtualAddress,0);
         SCSI_hPUTBUSADDRESS(&adapterTSH->hhcb,scbBuffer,
              OSMoffsetof(SCSI_SCB_STANDARD_ADVANCED,saddress0),busAddress);
      }    
   }

   HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
       OSMoffsetof(SCSI_SCB_STANDARD_ADVANCED,sg_cache_SCB),
       sg_cache_SCB);

   HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
       OSMoffsetof(SCSI_SCB_STANDARD_ADVANCED,scdb_length),
       (SCSI_UEXACT8) iob->targetCommandLength);

   if (iob->targetCommandLength <= 12)
   {
      /* cdb embedded in scb buffer */
      HIM_PUT_BYTE_STRING(adapterTSH->osmAdapterContext,scbBuffer,
          OSMoffsetof(SCSI_SCB_STANDARD_ADVANCED,SCSI_SADV_scdb0),
          iob->targetCommand,iob->targetCommandLength);
   }
   else
   {
      /* cdb pointer embedded in scb buffer */
      HIM_PUT_BYTE_STRING(adapterTSH->osmAdapterContext,iobReserve->cdb,
          0,iob->targetCommand,iob->targetCommandLength);
      busAddress = iob->iobReserve.busAddress;
      OSMxAdjustBusAddress(&busAddress,OSMoffsetof(SCSI_IOB_RESERVE,cdb[0]));
      SCSI_hPUTBUSADDRESS(&adapterTSH->hhcb,scbBuffer,
           OSMoffsetof(SCSI_SCB_STANDARD_ADVANCED,SCSI_SADV_scdb0),busAddress);
   }

   HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
       OSMoffsetof(SCSI_SCB_STANDARD_ADVANCED,scontrol),scontrol);

   HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
       OSMoffsetof(SCSI_SCB_STANDARD_ADVANCED,SCSI_SADV_array_site_next),
         hiob->scbDescriptor->queueNext->scbNumber);

   SCSI_hPUTBUSADDRESS(&adapterTSH->hhcb,scbBuffer,
        OSMoffsetof(SCSI_SCB_STANDARD_ADVANCED,next_SCB_addr0),
         hiob->scbDescriptor->queueNext->scbBuffer.busAddress);

   /* flush the cache ready for dma */
   SCSI_FLUSH_CACHE(scbBuffer,SCSI_SADV_SIZE_SCB);
}
#endif


/*********************************************************************
*
*  OSDSwappingAdvBuildSCB
*
*     Build scb buffer for swapping advanced mode 
*
*  Return Value:  void
*                  
*  Parameters:    hiob
*
*  Remarks:       This routine is defined for internal HIM reference
*
*********************************************************************/
#if SCSI_SWAPPING_ADVANCED_MODE
void OSDSwappingAdvBuildSCB (SCSI_HIOB HIM_PTR hiob,
                            HIM_IOB HIM_PTR iob,
                            SCSI_TARGET_TSCB HIM_PTR targetTSH,
                            SCSI_ADAPTER_TSCB HIM_PTR adapterTSH)
{
   SCSI_IOB_RESERVE HIM_PTR iobReserve =
                  (SCSI_IOB_RESERVE HIM_PTR)iob->iobReserve.virtualAddress;
   SCSI_HHCB HIM_PTR hhcb = &adapterTSH->hhcb;
   SCSI_SCB_SWAPPING_ADVANCED HIM_PTR scbBuffer =
         (SCSI_SCB_SWAPPING_ADVANCED HIM_PTR) hiob->scbDescriptor->scbBuffer.virtualAddress;
   SCSI_UEXACT32 sgData;
   SCSI_BUS_ADDRESS busAddress;
   SCSI_UEXACT8 sg_cache_SCB;
   SCSI_UEXACT8 scontrol;
   SCSI_DEVICE SCSI_DPTR deviceTable = 
         &SCSI_DEVICE_TABLE(hhcb)[(SCSI_UEXACT8)SCSI_TRGTSH(targetTSH)->scsiID];
   SCSI_UEXACT8 slun;

   scontrol = 0;     /* Assume disconnect is disabled */
   slun = (SCSI_UEXACT8) targetTSH->lunID;
   if ((deviceTable->SCSI_DF_disconnectEnable) &&
       (!hiob->SCSI_IF_disallowDisconnect))
   {
      slun |= SCSI_DISCENB;   /* Enable disconnect */

      /* We'll turn on tagged command only if the disconnect is enable */
      if (hiob->SCSI_IF_tagEnable)
      {
         scontrol |= SCSIscontrol[iob->taskAttribute];
      }
   }

   HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
       OSMoffsetof(SCSI_SCB_SWAPPING_ADVANCED,starget),
         (SCSI_UEXACT8) targetTSH->scsiID);

   HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
       OSMoffsetof(SCSI_SCB_SWAPPING_ADVANCED,slun),
         (SCSI_UEXACT8) slun);

   sg_cache_SCB = 2 * sizeof(SCSI_BUS_ADDRESS);

   if (!iob->data.bufferSize)
   {
      /* indicate no data transfer */
      sg_cache_SCB |= SCSI_WADV_NODATA;
   }
   else
   {
      /* get segment length */
      HIM_GET_LITTLE_ENDIAN32(adapterTSH->osmAdapterContext,&sgData,
          iob->data.virtualAddress,sizeof(SCSI_BUS_ADDRESS));
          
      /* to handle one segment and zero in length passed in */
      if (sgData == (SCSI_UEXACT32)0x80000000)
      {
         /* indicate no data transfer */
         sg_cache_SCB |= SCSI_WADV_NODATA;
      }
      else
      {
         /* 1st s/g element should be done like this */
         /* setup the embedded s/g segment (the first s/g segment) */
         if (sgData & (SCSI_UEXACT32)0x80000000)
         {
            /* there is only one sg element in sg list */
            sg_cache_SCB |= SCSI_WADV_ONESGSEG;
         }
         else
         {
            /* setup the sg pointer */
            SCSI_hPUTBUSADDRESS(&adapterTSH->hhcb,scbBuffer,
                 OSMoffsetof(SCSI_SCB_SWAPPING_ADVANCED,
                 SCSI_WADV_sg_pointer_SCB0),
                 iob->data.busAddress);

            /* reject modify data pointer setting */
            /* HQ remove this logic 
         scontrol |= SCSI_WADV_RJTMDP;
            */          
         }

         /* setup embedded sg segment length */
         HIM_PUT_LITTLE_ENDIAN24(adapterTSH->osmAdapterContext,scbBuffer,
             OSMoffsetof(SCSI_SCB_SWAPPING_ADVANCED,slength0),sgData);

         /* setup embedded sg segment address */
         SCSI_hGETBUSADDRESS(&adapterTSH->hhcb,&busAddress,
              iob->data.virtualAddress,0);
         SCSI_hPUTBUSADDRESS(&adapterTSH->hhcb,scbBuffer,
              OSMoffsetof(SCSI_SCB_SWAPPING_ADVANCED,saddress0),busAddress);
      }    
   }

   HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
       OSMoffsetof(SCSI_SCB_SWAPPING_ADVANCED,sg_cache_SCB),
       sg_cache_SCB);

   HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
       OSMoffsetof(SCSI_SCB_SWAPPING_ADVANCED,scdb_length),
       (SCSI_UEXACT8) iob->targetCommandLength);

   if (iob->targetCommandLength <= 12)
   {
      /* cdb embedded in scb buffer */
      HIM_PUT_BYTE_STRING(adapterTSH->osmAdapterContext,scbBuffer,
          OSMoffsetof(SCSI_SCB_SWAPPING_ADVANCED,SCSI_WADV_scdb0),
          iob->targetCommand,iob->targetCommandLength);
   }
   else
   {
      /* copy cdb and have cdb pointer embedded in scb buffer */
      HIM_PUT_BYTE_STRING(adapterTSH->osmAdapterContext,iobReserve->cdb,0,
          iob->targetCommand,iob->targetCommandLength);
      busAddress = iob->iobReserve.busAddress;
      OSMxAdjustBusAddress(&busAddress,OSMoffsetof(SCSI_IOB_RESERVE,cdb[0]));
      SCSI_hPUTBUSADDRESS(&adapterTSH->hhcb,scbBuffer,
           OSMoffsetof(SCSI_SCB_SWAPPING_ADVANCED,SCSI_WADV_scdb0),busAddress);
   }

#if SCSI_PARITY_PER_IOB
   if (hiob->SCSI_IF_parityEnable)
   {
      scontrol |= SCSI_WADV_ENPAR;
   }
#endif

   HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
       OSMoffsetof(SCSI_SCB_SWAPPING_ADVANCED,scontrol),scontrol);

   HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
       OSMoffsetof(SCSI_SCB_SWAPPING_ADVANCED,next_SCB),
         hiob->scbDescriptor->queueNext->scbNumber);

   /* flush the cache ready for dma */
   SCSI_FLUSH_CACHE(scbBuffer,sizeof(SCSI_SCB_SWAPPING_ADVANCED));
}
#endif

/*********************************************************************
*
*  OSDStandard160mBuildSCB
*
*     Build scb buffer for standard Ultra 160m mode
*
*  Return Value:  void
*                  
*  Parameters:    hiob
*
*  Remarks:       This routine is defined for internal HIM reference
*
*********************************************************************/
#if SCSI_STANDARD_160M_MODE
void OSDStandard160mBuildSCB (SCSI_HIOB HIM_PTR hiob,
                              HIM_IOB HIM_PTR iob,
                              SCSI_TARGET_TSCB HIM_PTR targetTSH,
                              SCSI_ADAPTER_TSCB HIM_PTR adapterTSH)
{
   SCSI_IOB_RESERVE HIM_PTR iobReserve =
                     (SCSI_IOB_RESERVE HIM_PTR)iob->iobReserve.virtualAddress;
   SCSI_SCB_STANDARD_160M HIM_PTR scbBuffer =
      (SCSI_SCB_STANDARD_160M HIM_PTR) hiob->scbDescriptor->scbBuffer.virtualAddress;
   SCSI_BUS_ADDRESS busAddress;
   SCSI_UEXACT32 sgData;
   SCSI_UEXACT8 sg_cache_SCB;
   SCSI_UEXACT8 scontrol;
   SCSI_UEXACT8 slun;

   /* clear the chain control */
   HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
       OSMoffsetof(SCSI_SCB_STANDARD_160M,chain_control),0);

   scontrol = 0;     /* Assume disconnect is disabled */
   slun = (SCSI_UEXACT8) targetTSH->lunID;
   if (!hiob->SCSI_IF_disallowDisconnect)
   {
      slun |= SCSI_DISCENB;   /* Enable disconnect */
      /* We'll set tag to enable only if the disconnect was enable */
      if (hiob->SCSI_IF_tagEnable)
      {
         scontrol |= SCSIscontrol[iob->taskAttribute];
      }
   }

   HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
       OSMoffsetof(SCSI_SCB_STANDARD_160M,starget),
       (SCSI_UEXACT8) targetTSH->scsiID);
   HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
       OSMoffsetof(SCSI_SCB_STANDARD_160M,slun),
       (SCSI_UEXACT8) slun);

   sg_cache_SCB = 2 * sizeof(SCSI_BUS_ADDRESS);

   if (!iob->data.bufferSize)
   {
      /* indicate no data transfer */
      sg_cache_SCB |= SCSI_S160M_NODATA;
   }
   else
   {
      /* get segment length */
      HIM_GET_LITTLE_ENDIAN32(adapterTSH->osmAdapterContext,&sgData,
          iob->data.virtualAddress,sizeof(SCSI_BUS_ADDRESS));

      /* to handle one segment and zero in length passed in */
      if (sgData == (SCSI_UEXACT32)0x80000000)
      {
         /* indicate no data transfer */
         sg_cache_SCB |= SCSI_S160M_NODATA;
      }
      else
      {
         /* 1st s/g element should be done like this */
         /* setup the embedded s/g segment (the first s/g segment) */
         if (sgData & (SCSI_UEXACT32)0x80000000)
         {
            sg_cache_SCB |= SCSI_S160M_ONESGSEG;
         }
         else
         {
            /* more than one s/g segments and need to setup s/g pointer */
            SCSI_hPUTBUSADDRESS(&adapterTSH->hhcb,scbBuffer,
                 OSMoffsetof(SCSI_SCB_STANDARD_160M,
                 SCSI_S160M_sg_pointer_SCB0),
                 iob->data.busAddress);

            /* reject modify data pointer setting */
            /* HQ remove this logic
            scontrol |= SCSI_S160M_RJTMDP;
            */     
         }

         /* write segment length */
         HIM_PUT_LITTLE_ENDIAN24(adapterTSH->osmAdapterContext,scbBuffer,
             OSMoffsetof(SCSI_SCB_STANDARD_160M,slength0),sgData);

         /* write segment address */
         SCSI_hGETBUSADDRESS(&adapterTSH->hhcb,&busAddress,
              iob->data.virtualAddress,0);
         SCSI_hPUTBUSADDRESS(&adapterTSH->hhcb,scbBuffer,
              OSMoffsetof(SCSI_SCB_STANDARD_160M,saddress0),busAddress);
      }    
   }

   HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
       OSMoffsetof(SCSI_SCB_STANDARD_160M,sg_cache_SCB),
       sg_cache_SCB);

   HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
       OSMoffsetof(SCSI_SCB_STANDARD_160M,scdb_length),
       (SCSI_UEXACT8) iob->targetCommandLength);

   if (iob->targetCommandLength <= 12)
   {
      /* cdb embedded in scb buffer */
      HIM_PUT_BYTE_STRING(adapterTSH->osmAdapterContext,scbBuffer,
          OSMoffsetof(SCSI_SCB_STANDARD_160M,SCSI_S160M_scdb0),
          iob->targetCommand,iob->targetCommandLength);
   }
   else
   {
      /* cdb pointer embedded in scb buffer */
      HIM_PUT_BYTE_STRING(adapterTSH->osmAdapterContext,iobReserve->cdb,
          0,iob->targetCommand,iob->targetCommandLength);
      busAddress = iob->iobReserve.busAddress;
      OSMxAdjustBusAddress(&busAddress,OSMoffsetof(SCSI_IOB_RESERVE,cdb[0]));
      SCSI_hPUTBUSADDRESS(&adapterTSH->hhcb,scbBuffer,
           OSMoffsetof(SCSI_SCB_STANDARD_160M,SCSI_S160M_scdb0),busAddress);
   }

#if SCSI_PARITY_PER_IOB
   if (hiob->SCSI_IF_parityEnable)
   {
      scontrol |= SCSI_S160M_ENPAR;
   }
#endif

   HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
       OSMoffsetof(SCSI_SCB_STANDARD_160M,scontrol),scontrol);

   HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
       OSMoffsetof(SCSI_SCB_STANDARD_160M,SCSI_S160M_array_site_next),
         hiob->scbDescriptor->queueNext->scbNumber);

   SCSI_hPUTBUSADDRESS(&adapterTSH->hhcb,scbBuffer,
        OSMoffsetof(SCSI_SCB_STANDARD_160M,next_SCB_addr0),
         hiob->scbDescriptor->queueNext->scbBuffer.busAddress);

   /* flush the cache ready for dma */
   SCSI_FLUSH_CACHE(scbBuffer,SCSI_S160M_SIZE_SCB);
}
#endif /* SCSI_STANDARD_160M_MODE */


/*********************************************************************
*
*  OSDSwapping160mBuildSCB
*
*     Build scb buffer for swapping Ultra 160m mode
*
*  Return Value:  void
*                  
*  Parameters:    hiob
*
*  Remarks:       This routine is defined for internal HIM reference
*
*********************************************************************/
#if SCSI_SWAPPING_160M_MODE
void OSDSwapping160mBuildSCB (SCSI_HIOB HIM_PTR hiob,
                              HIM_IOB HIM_PTR iob,
                              SCSI_TARGET_TSCB HIM_PTR targetTSH,
                              SCSI_ADAPTER_TSCB HIM_PTR adapterTSH)
{
   SCSI_IOB_RESERVE HIM_PTR iobReserve =
                  (SCSI_IOB_RESERVE HIM_PTR)iob->iobReserve.virtualAddress;
   SCSI_HHCB HIM_PTR hhcb = &adapterTSH->hhcb;
   SCSI_SCB_SWAPPING_160M HIM_PTR scbBuffer =
         (SCSI_SCB_SWAPPING_160M HIM_PTR) hiob->scbDescriptor->scbBuffer.virtualAddress;
   SCSI_UEXACT32 sgData;
   SCSI_BUS_ADDRESS busAddress;
   SCSI_UEXACT8 sg_cache_SCB;
   SCSI_UEXACT8 scontrol;
   SCSI_DEVICE SCSI_DPTR deviceTable; 
   SCSI_UEXACT8 slun;

   /* clear the aborted bit */
   HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
       OSMoffsetof(SCSI_SCB_SWAPPING_160M,scontrol),0);

   scontrol = 0;     /* Assume disconnect is disabled */

#if SCSI_DMA_SUPPORT
   if (hiob->cmd == SCSI_CMD_INITIATE_DMA_TASK)
   {
      /* build scb for memory to memory DMA operation */
      /* set scontrol bits */
      scontrol |= SCSI_W160M_BLOCK_MOVE;

      /* set up sg_cache_SCB */
      sg_cache_SCB = 2 * sizeof(SCSI_BUS_ADDRESS);

      HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
         OSMoffsetof(SCSI_SCB_SWAPPING_160M,sg_cache_SCB),
         sg_cache_SCB);

      /* set up s/g elements */

      /* for this IOB there must only be two elements */
      /* 1st element - source */  

      /* get segment length */
      HIM_GET_LITTLE_ENDIAN32(adapterTSH->osmAdapterContext,&sgData,
          iob->data.virtualAddress,sizeof(SCSI_BUS_ADDRESS));

      /* setup embedded sg segment length */
      HIM_PUT_LITTLE_ENDIAN24(adapterTSH->osmAdapterContext,scbBuffer,
          OSMoffsetof(SCSI_SCB_SWAPPING_160M,slength0),sgData);

      /* setup embedded sg segment address */
      SCSI_hGETBUSADDRESS(&adapterTSH->hhcb,&busAddress,
           iob->data.virtualAddress,0);
      SCSI_hPUTBUSADDRESS(&adapterTSH->hhcb,scbBuffer,
           OSMoffsetof(SCSI_SCB_SWAPPING_160M,saddress0),busAddress);

      /* only the address is required for the 2nd element - assumes */
      /* length is the same.                                        */
      SCSI_hGETBUSADDRESS(&adapterTSH->hhcb,&busAddress,
           iob->data.virtualAddress,(2 * sizeof(SCSI_BUS_ADDRESS)));
      SCSI_hPUTBUSADDRESS(&adapterTSH->hhcb,scbBuffer,
           OSMoffsetof(SCSI_SCB_SWAPPING_160M,SCSI_W160M_scdb0),busAddress);        
   }
   else
   { 
#endif /* SCSI_DMA_SUPPORT */
      deviceTable = &SCSI_DEVICE_TABLE(hhcb)[(SCSI_UEXACT8)SCSI_TRGTSH(targetTSH)->scsiID];
      slun = (SCSI_UEXACT8) targetTSH->lunID;
      if ((deviceTable->SCSI_DF_disconnectEnable) &&
          (!hiob->SCSI_IF_disallowDisconnect))
      {
         slun |= SCSI_W160M_DISCENB;   /* Enable disconnect */

         /* We'll turn on tagged command only if the disconnect is enable */
         if (hiob->SCSI_IF_tagEnable)
         {
            scontrol |= SCSIscontrol[iob->taskAttribute];
         }
      }

      HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
          OSMoffsetof(SCSI_SCB_SWAPPING_160M,starget),
            (SCSI_UEXACT8) targetTSH->scsiID);

      HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
          OSMoffsetof(SCSI_SCB_SWAPPING_160M,slun),
            (SCSI_UEXACT8) slun);

      sg_cache_SCB = 2 * sizeof(SCSI_BUS_ADDRESS);

      if (!iob->data.bufferSize)
      {
         /* indicate no data transfer */
         sg_cache_SCB |= SCSI_W160M_NODATA;
      }
      else
      {
         /* get segment length */
         HIM_GET_LITTLE_ENDIAN32(adapterTSH->osmAdapterContext,&sgData,
             iob->data.virtualAddress,sizeof(SCSI_BUS_ADDRESS));
          
         /* to handle one segment and zero in length passed in */
         if (sgData == (SCSI_UEXACT32)0x80000000)
         {
            /* indicate no data transfer */
            sg_cache_SCB |= SCSI_W160M_NODATA;
         }
         else
         {
            /* 1st s/g element should be done like this */
            /* setup the embedded s/g segment (the first s/g segment) */
            if (sgData & (SCSI_UEXACT32)0x80000000)
            {
               /* there is only one sg element in sg list */
               sg_cache_SCB |= SCSI_W160M_ONESGSEG;
            }
            else
            {
               /* setup the sg pointer */
               SCSI_hPUTBUSADDRESS(&adapterTSH->hhcb,scbBuffer,
                    OSMoffsetof(SCSI_SCB_SWAPPING_160M,
                    SCSI_W160M_sg_pointer_SCB0),
                    iob->data.busAddress);

               /* reject modify data pointer setting */
               /* HQ remove this logic 
               scontrol |= SCSI_W160M_RJTMDP;          
               */
            }

            /* setup embedded sg segment length */
            HIM_PUT_LITTLE_ENDIAN24(adapterTSH->osmAdapterContext,scbBuffer,
                OSMoffsetof(SCSI_SCB_SWAPPING_160M,slength0),sgData);

            /* setup embedded sg segment address */
            SCSI_hGETBUSADDRESS(&adapterTSH->hhcb,&busAddress,
                 iob->data.virtualAddress,0);
            SCSI_hPUTBUSADDRESS(&adapterTSH->hhcb,scbBuffer,
                 OSMoffsetof(SCSI_SCB_SWAPPING_160M,saddress0),busAddress);
         }    
      }

      HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
          OSMoffsetof(SCSI_SCB_SWAPPING_160M,sg_cache_SCB),
          sg_cache_SCB);

      HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
          OSMoffsetof(SCSI_SCB_SWAPPING_160M,scdb_length),
          (SCSI_UEXACT8) iob->targetCommandLength);

      if (iob->targetCommandLength <= 12)
      {
         /* cdb embedded in scb buffer */
         HIM_PUT_BYTE_STRING(adapterTSH->osmAdapterContext,scbBuffer,
             OSMoffsetof(SCSI_SCB_SWAPPING_160M,SCSI_W160M_scdb0),
             iob->targetCommand,iob->targetCommandLength);
      }
      else
      {
         /* copy cdb and have cdb pointer embedded in scb buffer */
         HIM_PUT_BYTE_STRING(adapterTSH->osmAdapterContext,iobReserve->cdb,0,
             iob->targetCommand,iob->targetCommandLength);
         busAddress = iob->iobReserve.busAddress;
         OSMxAdjustBusAddress(&busAddress,OSMoffsetof(SCSI_IOB_RESERVE,cdb[0]));
         SCSI_hPUTBUSADDRESS(&adapterTSH->hhcb,scbBuffer,
              OSMoffsetof(SCSI_SCB_SWAPPING_160M,SCSI_W160M_scdb0),busAddress);
      }

#if SCSI_PARITY_PER_IOB
      if (hiob->SCSI_IF_parityEnable)
      {
         scontrol |= SCSI_W160M_ENPAR;
      }
#endif

#if SCSI_PAC_NSX_REPEATER || SCSI_NSX_REPEATER
      if (hiob->SCSI_IF_nsxCommunication)
      {
         /* request NSX communication */
         scontrol |= SCSI_W160M_NSXENB;
      }
#endif /* SCSI_PAC_NSX_REPEATER || SCSI_NSX_REPEATER */

#if SCSI_DMA_SUPPORT
   }
#endif /* SCSI_DMA_SUPPORT */

   HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
       OSMoffsetof(SCSI_SCB_SWAPPING_160M,scontrol),scontrol);

   HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
       OSMoffsetof(SCSI_SCB_SWAPPING_160M,next_SCB),
         hiob->scbDescriptor->queueNext->scbNumber);

#if SCSI_DOMAIN_VALIDATION
   if (hiob->SCSI_IF_dvIOB)
   {
      HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
          OSMoffsetof(SCSI_SCB_SWAPPING_160M,dv_control),SCSI_W160M_DV_ENABLE);
   }
   else
   {
      HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
          OSMoffsetof(SCSI_SCB_SWAPPING_160M,dv_control),0);
   }
#endif /* SCSI_DOMAIN_VALIDATION */
   /* flush the cache ready for dma */
   SCSI_FLUSH_CACHE(scbBuffer,sizeof(SCSI_SCB_SWAPPING_160M));
}
#endif /* SCSI_SWAPPING_160M_MODE */

/*********************************************************************
*
*  OSDGetSGList
*
*     Get the Scatter/Gather List
*
*  Return Value:  SCSI_BUS_ADDRESS
*                  
*  Parameters:    hiob
*
*  Remarks:       This routine is defined for internal HIM reference
*
*********************************************************************/
SCSI_BUFFER_DESCRIPTOR HIM_PTR OSDGetSGList (SCSI_HIOB SCSI_IPTR hiob)
{
   HIM_IOB HIM_PTR iob = SCSI_GETIOB(hiob);

   return(&(iob->data));
}

/*********************************************************************
*
*  OSDCompleteHIOB
*
*     Process notmal command complete
*
*  Return Value:  void
*                  
*  Parameters:    hiob
*
*  Remarks:       This routine is defined for internal HIM reference
*
*********************************************************************/
void OSDCompleteHIOB (SCSI_HIOB SCSI_IPTR hiob)
{
   HIM_IOB HIM_PTR iob = SCSI_GETIOB(hiob);

   /* process normal command first */
   if (hiob->stat == SCSI_SCB_COMP)
   {
      /* process successful status */
      iob->taskStatus = HIM_IOB_GOOD;
   }
   else
   {
      /* translate error status */
      SCSIxTranslateError(iob,hiob);
   }

   iob->postRoutine(iob);
}

/*********************************************************************
*
*  OSDCompleteSpecialHIOB
*
*     Process special command complete
*
*  Return Value:  void
*                  
*  Parameters:    hiob
*
*  Remarks:       This routine is defined for internal HIM reference
*
*********************************************************************/
void OSDCompleteSpecialHIOB (SCSI_HIOB SCSI_IPTR hiob)
{
   HIM_IOB HIM_PTR      iob = SCSI_GETIOB(hiob);
   HIM_IOB HIM_PTR      activeIob;
   SCSI_HIOB SCSI_IPTR  saveHiob;
   SCSI_HHCB HIM_PTR    hhcb;
   SCSI_ADAPTER_TSCB HIM_PTR adapterTSH= SCSI_ADPTSH(iob->taskSetHandle);


   /* process special command */
   switch(iob->function)
   {
      case HIM_RESET_BUS_OR_TARGET:
         /* Reset SCSI Bus Iob */
         if (SCSI_TRGTSH(iob->taskSetHandle)->typeTSCB == SCSI_TSCB_TYPE_ADAPTER)
         {
            /* Must use exact "hiob" word for the macro SCSI_GETIOB. */
            /* Otherwise, the compiler flags the error: not a member of */
            /* SCSI_IOB_RESERVE. See SCSI_GETIOB macro for details. */
            saveHiob = hiob;
            hiob = (SCSI_HIOB SCSI_IPTR) SCSI_ACTIVE_HIOB(hiob);

            if (hiob != SCSI_NULL_HIOB)
            {
               /* The snsBuffer of the special Bus Reset HIOB contained */
               /* the HIOB of the active target at the time of reset.   */
               activeIob = SCSI_GETIOB(hiob);
               iob->errorData = (void HIM_PTR) activeIob->taskSetHandle;
            }

            hiob = saveHiob;
         }

         if (hiob->stat == SCSI_SCB_COMP)
         {
            /* process successful status */
            iob->taskStatus = HIM_IOB_GOOD;
         }
         else
         {
            /* translate error status */
            SCSIxTranslateError(iob, hiob);
         }

         break;

      case HIM_ABORT_TASK:
      case HIM_ABORT_TASK_SET:
      case HIM_UNFREEZE_QUEUE:
      case HIM_RESET_HARDWARE:
      case HIM_TERMINATE_TASK:
#if SCSI_DMA_SUPPORT
      case HIM_INITIATE_DMA_TASK:
#endif /* SCSI_DMA_SUPPORT */
         if (hiob->stat == SCSI_SCB_COMP)
         {
            /* process successful status */
            iob->taskStatus = HIM_IOB_GOOD;
         }
         else
         {
            /* translate error status */
            SCSIxTranslateError(iob, hiob);
         }

         break;

      case HIM_PROTOCOL_AUTO_CONFIG:
         hhcb = &SCSI_ADPTSH(iob->taskSetHandle)->hhcb;
#if SCSI_TARGET_OPERATION && !SCSI_FAST_PAC
         /* Only perform bus scan if initiator mode enabled */ 
         if (hhcb->SCSI_HF_initiatorMode)
#endif /* SCSI_TARGET_OPERATION && !SCSI_FAST_PAC */

#if !SCSI_FAST_PAC
         {
            SCSI_ADPTSH(iob->taskSetHandle)->iobProtocolAutoConfig = iob;
            if (SCSIxSetupBusScan(SCSI_ADPTSH(iob->taskSetHandle)))
            {
               /* need to perform bus scan after protocol auto config */
               SCSIxQueueBusScan(SCSI_ADPTSH(iob->taskSetHandle));
               /* don't post until procedure is done */
            }
            else
            {
               /* SCSIxSetupBusScan indicates that there is nothing to scan */
               /* We'll just post for PAC completion here & return.         */

               /* Clear resetSCSI flag after the PAC */
               hhcb->SCSI_HF_resetSCSI = 0;

#if (SCSI_AICBAYONET || SCSI_AICTRIDENT)
               if (hhcb->SCSI_HF_expSupport)
               {
                  SCSI_DISABLE_EXP_STATUS(hhcb);
               }
#endif /* SCSI_AICBAYONET || SCSI_AICTRIDENT */

               adapterTSH->iobProtocolAutoConfig->taskStatus = HIM_IOB_GOOD;

               adapterTSH->iobProtocolAutoConfig->postRoutine(adapterTSH->iobProtocolAutoConfig);

               /* If OSM is frozen then unfreeze the OSM because the OSMEvent */
               /* handling is complete */
               if(adapterTSH->SCSI_AF_osmFrozen)
               {
                  OSDAsyncEvent(hhcb, SCSI_AE_OSMUNFREEZE);
               }

               return;
            }   
         }   
#if SCSI_TARGET_OPERATION
         else
#endif /* SCSI_TARGET_OPERATION */

#endif /* !SCSI_FAST_PAC */

#if SCSI_TARGET_OPERATION || SCSI_FAST_PAC
         {
            /* post protocol auto config back to osm */
            iob->taskStatus = HIM_IOB_GOOD;
            iob->postRoutine(iob);

            /* If OSM is frozen then unfreeze the OSM because the OSMEvent handling */
            /* is complete */
            if (adapterTSH->SCSI_AF_osmFrozen)
            {
               OSDAsyncEvent(&adapterTSH->hhcb, SCSI_AE_OSMUNFREEZE);         
            }
         }

#endif /* SCSI_TARGET_OPERATION || SCSI_FAST_PAC */

         return;
   
#if SCSI_TARGET_OPERATION         
      
      case HIM_ESTABLISH_CONNECTION:
        /* Copy the command length and the nexusTSH from the HIOB */  
         iob->targetCommandLength = SCSI_CMD_LENGTH(hiob);
         iob->taskSetHandle = (HIM_TASK_SET_HANDLE) SCSI_NEXUS_UNIT(hiob);
         
         /* Set request type to device command initially */
         iob->flagsIob.targetRequestType = HIM_REQUEST_TYPE_CMND;

         if (hiob->stat == SCSI_SCB_COMP)
         {
            /* process successful status */
            
            iob->taskStatus = HIM_IOB_GOOD;
         }
         else
         if (hiob->stat == SCSI_TASK_CMD_COMP)
         {
            /* Task management function */
            SCSIxTargetTaskManagementRequest(iob,hiob);
         }
         else
         {
            /* translate error status */
            SCSIxTranslateError(iob,hiob);
         }

         break;
                      
      case HIM_REESTABLISH_INTERMEDIATE:
      case HIM_REESTABLISH_AND_COMPLETE:
      case HIM_ABORT_NEXUS:
#if SCSI_MULTIPLEID_SUPPORT
      case HIM_ENABLE_ID:
      case HIM_DISABLE_ID:
#endif /* SCSI_MULTIPLEID_SUPPORT */          
         if (hiob->stat == SCSI_SCB_COMP)
         {
            /* process successful status */
            iob->taskStatus = HIM_IOB_GOOD;
         }
         else
         if (hiob->stat == SCSI_TASK_CMD_COMP)
         {
            /* Task management function */
            SCSIxTargetTaskManagementRequest(iob,hiob);
         }
         else
         {
            /* translate error status */
            SCSIxTranslateError(iob,hiob);
         }

         break;
#endif  /* SCSI_TARGET_OPERATION */

      default:
         /* indicate iob invalid and post back immediately */
         iob->taskStatus = HIM_IOB_INVALID;
         break;
   }

   iob->postRoutine(iob);
}

/*********************************************************************
*
*  SCSIxTranslateError
*
*     Translate hiob error status to iob error status
*
*  Return Value:  none 
*                  
*  Parameters:    iob
*                 hiob
*
*  Remarks:
*
*********************************************************************/
void SCSIxTranslateError (HIM_IOB HIM_PTR iob,
                          SCSI_HIOB HIM_PTR hiob)
{
   /* interpret stat of hiob */ 
   switch(hiob->stat)
   {
      case SCSI_SCB_ERR:
         /* interpret haStat of hiob */
         switch(hiob->haStat)
         {
            case SCSI_HOST_SEL_TO:
               iob->taskStatus = HIM_IOB_NO_RESPONSE;
               break;

            case SCSI_HOST_BUS_FREE:
               iob->taskStatus = HIM_IOB_CONNECTION_FAILED;
               break;

            case SCSI_HOST_PHASE_ERR:
            case SCSI_HOST_HW_ERROR:    
               iob->taskStatus = HIM_IOB_HOST_ADAPTER_FAILURE;
               break;

            case SCSI_HOST_SNS_FAIL:
               iob->taskStatus = HIM_IOB_ERRORDATA_FAILED;
               break;

            case SCSI_HOST_DETECTED_ERR:
               iob->taskStatus = HIM_IOB_PARITY_ERROR;
               break;

            case SCSI_HOST_DU_DO:
            case SCSI_HOST_NO_STATUS:
               /* interpret trgStatus of hiob */
               switch(hiob->trgStatus)
               {
                  case SCSI_UNIT_CHECK:
                     if (iob->flagsIob.autoSense)
                     {
                        /* Set return status */
                        iob->taskStatus = HIM_IOB_ERRORDATA_VALID;

                        /* Get residual sense length */
                        iob->residualError = (HIM_UEXACT32) hiob->snsResidual;

                        if (iob->errorDataLength > 255)
                        {
                           iob->residualError += (iob->errorDataLength - 255);
                        }
                     }
                     else
                     {
                        iob->taskStatus = HIM_IOB_ERRORDATA_REQUIRED;
                     }

                     if (hiob->haStat == SCSI_HOST_DU_DO)
                     {
                        iob->residual = hiob->residualLength;
                     }
                     break;

                  case SCSI_UNIT_BUSY:
                     iob->taskStatus = HIM_IOB_BUSY;
                     break;

                  case SCSI_UNIT_RESERV:
                     iob->taskStatus = HIM_IOB_TARGET_RESERVED;
                     break;

                  case SCSI_UNIT_QUEFULL:
                     iob->taskStatus = HIM_IOB_TASK_SET_FULL;
                     break;

                  case SCSI_UNIT_GOOD:
                  case SCSI_UNIT_MET:
                  case SCSI_UNIT_INTERMED:
                  case SCSI_UNIT_INTMED_GD:
                     if (hiob->haStat == SCSI_HOST_DU_DO)
                     {
                        iob->taskStatus = HIM_IOB_DATA_OVERUNDERRUN;
                        iob->residual = hiob->residualLength;
                     }
                     else
                     {
                        iob->taskStatus = HIM_IOB_GOOD;
                     }
                     break;

                  default:
                     /* should never come to here */
                     iob->taskStatus = HIM_IOB_PROTOCOL_SPECIFIC;
                     break;
               }
               break;

            case SCSI_HOST_TAG_REJ:
            case SCSI_HOST_ABT_FAIL:
            case SCSI_HOST_RST_HA:   
            case SCSI_HOST_RST_OTHER:
               /* not implemented error */
               /* should never come to here */
               break;

            case SCSI_HOST_ABT_NOT_FND:
               iob->taskStatus = HIM_IOB_ABORT_NOT_FOUND;
               break;

            case SCSI_HOST_ABT_CMDDONE:
               iob->taskStatus = HIM_IOB_ABORT_ALREADY_DONE;
               break;

            case SCSI_HOST_NOAVL_INDEX: 
            case SCSI_HOST_INV_LINK:
               /* not implemented error */
               /* should never come to here */
               break;

#if SCSI_TARGET_OPERATION
            case SCSI_INITIATOR_PARITY_MSG:     /* Initiator message parity message  */ 
               iob->taskStatus = HIM_IOB_INITIATOR_DETECTED_PARITY_ERROR;
               break;
                
            case SCSI_INITIATOR_PARITY_ERR_MSG: /* Initiator detected error message*/ 
               iob->taskStatus = HIM_IOB_INITIATOR_DETECTED_ERROR;
               break;        

            case SCSI_HOST_MSG_REJECT:          /* Initiator rejected a message which  */
                                                /* we expected to be OK.               */
               iob->taskStatus = HIM_IOB_INVALID_MESSAGE_REJECT;
               break; 
                                         
            case SCSI_INITIATOR_INVALID_MSG:    /* Invalid message recieved from       */
                                                /* initiator. After the HIM issued a   */
                                                /* Message Reject message in response  */
                                                /* to an unrecognized or unsupported   */
                                                /* message the initiator released the  */
                                                /* ATN signal.                         */ 
               iob->taskStatus = HIM_IOB_INVALID_MESSAGE_RCVD;
               break;
                            
            case SCSI_HOST_IDENT_RSVD_BITS:     /* Identify Message has reserved bits  */
               break;                           /* set. XLM should never get this      */
                                                /* error code. HWM uses this code to   */
                                                /* indicate that a reserved bit is set */
                                                /* in the Identify message and the     */
                                                /* adapter profile field               */
                                                /* AP_SCSI2_IdentifyMsgRsv is set to   */
                                                /* 0, indicating that the HWM is to    */
                                                /* issue a message reject. Only        */
                                                /* applies to SCSI2.                   */
            case SCSI_HOST_IDENT_LUNTAR_BIT:    /* Identify Message has the luntar bit */
               break;                           /* set. XLM should never get this      */
                                                /* error code. HWM uses this code to   */
                                                /* indicate that the luntar bit is set */
                                                /* in the Identify message and the     */
                                                /* adapter profile field               */
                                                /* AP_SCSI2_TargetRejectLuntar is set  */
                                                /* to 1, indicating that the HWM is to */
                                                /* issue a message reject. Only        */
                                                /* applies to SCSI2.                   */

            case SCSI_HOST_TRUNC_CMD:           /* Truncated SCSI Command              */
               iob->taskStatus = HIM_IOB_TARGETCOMMANDBUFFER_OVERRUN; 
               break;
#endif /* SCSI_TARGET_OPERATION */

            case SCSI_HOST_ABT_CHANNEL_FAILED:
               iob->taskStatus = HIM_IOB_ABORTED_CHANNEL_FAILED;
               break;

            default:
               /* should never come to here */
               break;
         }
         break;

      case SCSI_SCB_ABORTED:
         /* interpret haStat of hiob */
         switch(hiob->haStat)
         {
            case SCSI_HOST_ABT_HOST:
               iob->taskStatus = HIM_IOB_ABORTED_ON_REQUEST;
               break;

            case SCSI_HOST_ABT_HA:
               iob->taskStatus = HIM_IOB_ABORTED_CHIM_RESET;
               break;

            case SCSI_HOST_ABT_BUS_RST:
               iob->taskStatus = HIM_IOB_ABORTED_REQ_BUS_RESET;
               break;

            case SCSI_HOST_ABT_3RD_RST:
               iob->taskStatus = HIM_IOB_ABORTED_3RD_PARTY_RESET;
               break;

            case SCSI_HOST_ABT_IOERR:
               iob->taskStatus = HIM_IOB_ABORTED_TRANSPORT_MODE_CHANGE;
               break;

            case SCSI_HOST_ABT_TRG_RST:
               iob->taskStatus = HIM_IOB_ABORTED_REQ_TARGET_RESET;
               break;

            case SCSI_HOST_PHASE_ERR:
            case SCSI_HOST_HW_ERROR:    
               iob->taskStatus = HIM_IOB_HOST_ADAPTER_FAILURE;
               break;

#if SCSI_TARGET_OPERATION
            /* Target mode HIOB Abort reasons */
            case SCSI_HOST_TARGET_RESET:
               iob->taskStatus = HIM_IOB_ABORTED_TR_RECVD;
               break;

            case SCSI_HOST_ABORT_TASK:
               iob->taskStatus = HIM_IOB_ABORTED_ABT_RECVD;
               break;

            case SCSI_HOST_ABORT_TASK_SET:
               iob->taskStatus = HIM_IOB_ABORTED_ABTS_RECVD;
               break;

            case SCSI_HOST_CLEAR_TASK_SET:
               iob->taskStatus = HIM_IOB_ABORTED_CTS_RECVD;
               break;
            
            case SCSI_HOST_TERMINATE_TASK:
               iob->taskStatus = HIM_IOB_ABORTED_TT_RECVD;
               break;
            
#endif /* SCSI_TARGET_OPERATION */

            default:
               /* should never come to here */
               break;
         }
         break;

      case SCSI_SCB_PENDING:
         if (hiob->haStat == SCSI_HOST_ABT_STARTED)
         {
            iob->taskStatus = HIM_IOB_ABORT_STARTED;
         }
         break;

      default:
         /* SCSI_SCB_COMP is not processed here */
         /* SCSI_TASK_CMD_COMP is not processed here */
         /* SCSI_SCB_INV_CMD should never happen for this implementation */
         /* we should never get to here */
         break;
   }
}

/*********************************************************************
*
*  OSDGetBusAddress
*
*     Get bus address
*
*  Return Value:  bus address
*                  
*  Parameters:    hhcb
*                 memory category
*                 virtual address
*
*  Remarks:       This routine is defined for internal HIM reference
*
*********************************************************************/
SCSI_BUS_ADDRESS OSDGetBusAddress (SCSI_HHCB SCSI_HPTR hhcb,
                                   SCSI_UEXACT8 category,
                                   void SCSI_IPTR virtualAddress)
{
   SCSI_ADAPTER_TSCB HIM_PTR adapterTSH = SCSI_GETADP(hhcb);

   return(OSMxGetBusAddress(SCSI_ADPTSH(adapterTSH)->osmAdapterContext,category,virtualAddress));   
}

/*********************************************************************
*
*  OSDAdjustBusAddress
*
*     Adjust bus address
*
*  Return Value:  none
*                  
*  Parameters:    hhcb
*                 bus address
*                 value for adjustment
*
*  Remarks:       This routine is defined for internal HIM reference
*
*********************************************************************/
void OSDAdjustBusAddress (SCSI_HHCB SCSI_HPTR hhcb,
                          SCSI_BUS_ADDRESS SCSI_IPTR busAddress,
                          SCSI_UINT value)
{
   SCSI_ADAPTER_TSCB HIM_PTR adapterTSH = SCSI_GETADP(hhcb);

   OSMxAdjustBusAddress(busAddress,value);
}

/*********************************************************************
*
*  OSDTimer
*
*     Delays for  5 microseconds
*
*  Return Value:  none
*                  
*  Parameters:    hhcb
*                 
*
*  Remarks:       This routine is defined for internal HIM reference
*
*********************************************************************/
void OSDTimer (SCSI_HHCB SCSI_HPTR hhcb)
{
   SCSI_ADAPTER_TSCB HIM_PTR adapterTSH = SCSI_GETADP(hhcb);
   
   OSMxDelay (SCSI_ADPTSH(adapterTSH)->osmAdapterContext,(SCSI_UINT32) 5);
}

/*********************************************************************
*
*  OSDReadPciConfiguration
*
*     Read dword value from pci configuration space
*
*  Return Value:  
*                  
*  Parameters:    hhcb
*                 registerNumber
*
*  Remarks:       This routine is defined for internal HIM reference
*
*********************************************************************/
SCSI_UEXACT32 OSDReadPciConfiguration (SCSI_HHCB SCSI_HPTR hhcb,
                                       SCSI_UEXACT8 registerNumber)
{
   SCSI_ADAPTER_TSCB HIM_PTR adapterTSH = SCSI_GETADP(hhcb);

   return(OSMxReadPCIConfigurationDword(SCSI_ADPTSH(adapterTSH)->osmAdapterContext,registerNumber));
}

/*********************************************************************
*
*  OSDWritePciConfiguration
*
*     Write dword value to pci configuration space
*
*  Return Value:  none
*                  
*  Parameters:    hhcb
*                 registerNumber
*                 value
*
*  Remarks:       This routine is defined for internal HIM reference
*
*********************************************************************/
void OSDWritePciConfiguration (SCSI_HHCB SCSI_HPTR hhcb,
                               SCSI_UEXACT8 regNumber,
                               SCSI_UEXACT32 value)
{
   SCSI_ADAPTER_TSCB HIM_PTR adapterTSH = SCSI_GETADP(hhcb);

   OSMxWritePCIConfigurationDword(SCSI_ADPTSH(adapterTSH)->osmAdapterContext,regNumber,value);
}

/*********************************************************************
*
*  OSDAsyncEvent
*
*     Handle asynch event from SCSI
*
*  Return Value:  none
*                  
*  Parameters:    hhcb
*                 event
*                 others...
*
*  Remarks:       This routine is defined for internal HIM reference
*
*********************************************************************/
void OSDAsyncEvent (SCSI_HHCB SCSI_HPTR hhcb,
                    SCSI_UINT16 event, ...)
{
   SCSI_ADAPTER_TSCB HIM_PTR adapterTSH = SCSI_GETADP(hhcb);
   SCSI_UINT8 eventCHIM = 0;  /* Init to 0 to take care warning message */

   /* translate the event per chim spec */
   switch(event)
   {
      case SCSI_AE_3PTY_RESET:
         eventCHIM = HIM_EVENT_IO_CHANNEL_RESET;
         break;

      /*case SCSI_AE_HAID_CHANGE: */ /* do nothing for now */
#if SCSI_SCAM_ENABLE == 2
      case SCSI_AE_SCAM_SELD:
         eventCHIM = HIM_EVENT_AUTO_CONFIG_REQUIRED;
         break;
#endif /* SCSI_SCAM_ENABLE == 2 */

      case SCSI_AE_HA_RESET:
         eventCHIM = HIM_EVENT_HA_FAILED;
         break;

      case SCSI_AE_IOERROR:
         eventCHIM = HIM_EVENT_TRANSPORT_MODE_CHANGE;
         break;

#if SCSI_CHECK_PCI_ERROR
      /* receive pci target adort error */
      case SCSI_AE_PCI_RTA:
         eventCHIM = HIM_EVENT_PCI_RTA;
         break;
      
      /* receive other pci error */
      case SCSI_AE_PCI_ERROR:
         eventCHIM = HIM_EVENT_PCI_ERROR;
         break;
#endif /* #if SCSI_CHECK_PCI_ERROR */

      case SCSI_AE_OSMFREEZE:
         SCSI_ADPTSH(adapterTSH)->SCSI_AF_osmFrozen = 1;   /* Set OSM is Frozen flag */
         eventCHIM = HIM_EVENT_OSMFREEZE;
         break;

      case SCSI_AE_OSMUNFREEZE:
         SCSI_ADPTSH(adapterTSH)->SCSI_AF_osmFrozen = 0;   /* Set OSM is Unfrozen flag */
         eventCHIM = HIM_EVENT_OSMUNFREEZE;
         break;

#if SCSI_TARGET_OPERATION
      case SCSI_AE_NEXUS_TSH_THRESHOLD:
         eventCHIM = HIM_EVENT_NEXUSTSH_THRESHOLD;  
         break;

      case SCSI_AE_EC_IOB_THRESHOLD:
         eventCHIM = HIM_EVENT_EC_IOB_THRESHOLD;
         break;
#endif /* SCSI_TARGET_OPERATION */

      case SCSI_AE_IO_CHANNEL_FAILED:
         eventCHIM = HIM_EVENT_IO_CHANNEL_FAILED;
         break;
   }

   /* inform the OSM about the event */
   OSMxEvent(SCSI_ADPTSH(adapterTSH)->osmAdapterContext,eventCHIM,(void HIM_PTR) 0);
}

/*********************************************************************
*
*  OSDGetHostAddress
*
*     Get address of the host device
*
*  Return Value:  pointer to host address
*                  
*  Parameters:    hhcb
*
*  Remarks:       The host address aquired from this routine can
*                 be used to either group the host devices or
*                 select scb memory bank (Excalibur)
*
*********************************************************************/
SCSI_HOST_ADDRESS SCSI_LPTR OSDGetHostAddress (SCSI_HHCB SCSI_HPTR hhcb)
{
   SCSI_ADAPTER_TSCB HIM_PTR adapterTSH = SCSI_GETADP(hhcb);

   return((SCSI_HOST_ADDRESS HIM_PTR)&(adapterTSH->hostAddress));
}

/*********************************************************************
*
*  SCSIxApplyNVData
*
*     Apply NV RAM configuration information to the adapter
*
*  Return Value:  0 - NV data applied
*                 1 - No NV ram available
*                  
*  Parameters:    adapterTSH
*
*  Remarks:
*
*********************************************************************/
SCSI_UINT8 SCSIxApplyNVData (HIM_TASK_SET_HANDLE adapterTSH)
{

#if SCSI_SCSISELECT_SUPPORT

   SCSI_HHCB HIM_PTR hhcb = &SCSI_ADPTSH(adapterTSH)->hhcb;
   
   SCSI_NVM_LAYOUT nvmData;

/*  SCSI_UEXACT16 multipleLun;  Temporary comments out */
   SCSI_UINT ultraEnable;
   SCSI_UINT16 seepromBase;
   HIM_BOOLEAN validscb2data = HIM_FALSE;
   SCSI_UINT i;

#endif /* SCSI_SCSISELECT_SUPPORT */

   /* For right now, by default, the multiple LUN support will be */
   /* turned on for all targets until we come up with a new decision. */
   /* Set here to guarantee that the multiple LUN always support */
   /* even if the checksum of the nvram is bad. */
   SCSI_ADPTSH(adapterTSH)->multipleLun = 0xFFFF;

#if SCSI_SCSISELECT_SUPPORT
   /* try to get from OSM non-volatile memory */
   if (OSMxGetNVData(SCSI_ADPTSH(adapterTSH)->osmAdapterContext,&nvmData,0,sizeof(SCSI_NVM_LAYOUT)))
   {
      /* OSM NVM not available */
      /* read from SEEPROM attached to host device */
      
      /* figure out the section base within the seeprom */
      seepromBase = 0;
      switch(hhcb->SCSI_SUBID_scsiChannels)
      {
         case SCSI_TWO_CHNL:
            if (hhcb->SCSI_SUBID_multifunction == 1)
            {
               if (SCSI_ADPTSH(adapterTSH)->hostAddress.pciAddress.functionNumber != 0)
               {
                  seepromBase = sizeof(SCSI_NVM_LAYOUT) / 2;
               }
            }
            else
            {
               if (hhcb->indexWithinGroup)
               {
                  seepromBase = sizeof(SCSI_NVM_LAYOUT) / 2;
               }
            }
            break;

         case SCSI_THREE_CHNL:
            switch (hhcb->indexWithinGroup)
            {
               case 0:
                  /* based at the first section */
                  break;

               case 1:
                  /* based at the second section */
                  seepromBase = sizeof(SCSI_NVM_LAYOUT) / 2;
                  break;

               case 2:
                  /* based at the third section */
                  seepromBase = sizeof(SCSI_NVM_LAYOUT) / 2 * 2;
                  break;
            }
            break;


         case SCSI_FOUR_CHNL:
            seepromBase = sizeof(SCSI_NVM_LAYOUT) / 2 *
               (SCSI_ADPTSH(adapterTSH)->hostAddress.pciAddress.deviceNumber - 4);
            break;
      }
 
      /* read from seeprom */
      if (SCSIHReadSEEPROM(hhcb,(SCSI_UEXACT8 SCSI_SPTR) &nvmData,
           (SCSI_SINT16) seepromBase,(SCSI_SINT16) sizeof(SCSI_NVM_LAYOUT)/2))
         
      {
        if (!SCSIHCheckSigSCB2(hhcb,(SCSI_UEXACT8 SCSI_SPTR) &nvmData))
       
           /* seeprom not available and no SCB2 information */
           return(1);
        else
           if (!SCSIxDetermineValidNVData((SCSI_NVM_LAYOUT HIM_PTR) &nvmData))
           return(1);
         else
           validscb2data = HIM_TRUE;
      }
   }

   /* if default seeprom fails, change type and retry process.   */
   /* present values for seeproms are 1k(typical default) and 2k */
   if ((!validscb2data) && (!SCSIxDetermineValidNVData((SCSI_NVM_LAYOUT HIM_PTR) &nvmData))) 
   {
      /* default seeprom fails, change type */
      if(hhcb->SCSI_HP_se2Type == SCSI_EETYPE_C06C46) 
       {  
         hhcb->SCSI_HP_se2Type = SCSI_EETYPE_C56C66;
       }
      else
       {  
         hhcb->SCSI_HP_se2Type = SCSI_EETYPE_C06C46;   
       }
      
      /* read from seeprom */
      if (SCSIHReadSEEPROM(hhcb,(SCSI_UEXACT8 SCSI_SPTR) &nvmData,
         (SCSI_SINT16) seepromBase,(SCSI_SINT16) sizeof(SCSI_NVM_LAYOUT)/2))
      {
         if (!SCSIHCheckSigSCB2(hhcb,(SCSI_UEXACT8 SCSI_SPTR) &nvmData))
            /* no serial eeprom and no valid SCB2 information */
            return(1);      
         else
            if (!SCSIxDetermineValidNVData((SCSI_NVM_LAYOUT HIM_PTR) &nvmData))
               return(1);
            else
               validscb2data = HIM_TRUE;         
      }

      if ((!validscb2data) && (!SCSIxDetermineValidNVData((SCSI_NVM_LAYOUT HIM_PTR) &nvmData)))
         if (!SCSIHCheckSigSCB2(hhcb,(SCSI_UEXACT8 SCSI_SPTR) &nvmData))
            /* serial eeprom not available and no valid SCB2 data */
            return(1);
         else
            if (!SCSIxDetermineValidNVData((SCSI_NVM_LAYOUT HIM_PTR) &nvmData))
               return(1);
         
    }


   /* make sure the max. targets in NVRAM matching with the default setting */
   /* value which based on the HA capability. */
   if (hhcb->maxDevices != (SCSI_UEXACT8)(nvmData.word20 & SCSI_WORD20_MAXTARGETS))
   {
      return(1);
   }
   
   /* interpret and apply nvm information */
   /* apply per target configuration */

   /* apply per target configuration */
   ultraEnable = 0;
/*   multipleLun = 0;   Temporary comments out */
   /* AIC-78XX NVRAM targets layout */
   if (hhcb->hardwareMode == SCSI_HMODE_AIC78XX)
   {
      for (i = 0; i < (SCSI_UINT)(nvmData.word20 & SCSI_WORD20_MAXTARGETS); i++)
      {
         /* load scsiOption */
         SCSI_DEVICE_TABLE(hhcb)[i].scsiOption = (SCSI_UEXACT8)
               (((nvmData.targetControl.aic78xxTC[i] & SCSI_AIC78XXTC_SYNCHRONOUSRATE) << 4) |
                ((nvmData.targetControl.aic78xxTC[i] & SCSI_AIC78XXTC_INITIATESYNC) >> 3) |
                ((nvmData.targetControl.aic78xxTC[i] & SCSI_AIC78XXTC_INITIATEWIDE) << 2));

         /* build disconnectEnable */
         SCSI_DEVICE_TABLE(hhcb)[i].SCSI_DF_disconnectEnable =
               (nvmData.targetControl.aic78xxTC[i] & SCSI_AIC78XXTC_DISCONNECTENABLE) >> 4;

         /* build ultraEnable */
         SCSI_DEVICE_TABLE(hhcb)[i].SCSI_DF_ultraEnable =
               (nvmData.targetControl.aic78xxTC[i] & SCSI_AIC78XXTC_ULTRAENABLE) >> 6;
         ultraEnable |= (nvmData.targetControl.aic78xxTC[i] & SCSI_AIC78XXTC_ULTRAENABLE) >> 6;

         /* build multiple lun support */
/*----------------------- Temporary comments out ----------------
         multipleLun |= ((nvmData.targetControl.aic78xxTC[i] & SCSI_AIC78XXTC_MULTIPLELUN) >> 11) << i;
---------------------------------------------------------------*/
      }
   }
#if (SCSI_AICBAYONET || SCSI_AICTRIDENT)
   /* Bayonet & Trident NVRAM targets layout */
   else
   {
      for (i = 0; i < (SCSI_UINT)(nvmData.word20 & SCSI_WORD20_MAXTARGETS); i++)
      {
         /* load scsiOption */
         SCSI_DEVICE_TABLE(hhcb)[i].scsiOption = (SCSI_UEXACT8)
               (((nvmData.targetControl.bayonetTC[i]& SCSI_BAYONETTC_INITIATESYNC) >> 3) |
                ((nvmData.targetControl.bayonetTC[i] & SCSI_BAYONETTC_INITIATEWIDE) << 2));

         /* load scsi rate option and wide bit */
         SCSI_DEVICE_TABLE(hhcb)[i].bayScsiRate = (SCSI_UEXACT8)
               ((nvmData.targetControl.bayonetTC[i] & SCSI_BAYONETTC_LOWBITSYNCRATE) |
                ((nvmData.targetControl.bayonetTC[i] & SCSI_BAYONETTC_HIGHBITSYNCRATE) >> 3) |
                ((nvmData.targetControl.bayonetTC[i] & SCSI_BAYONETTC_INITIATEWIDE) << 2));

         /* update CRC enable bit for Trident hardware */
         if(hhcb->hardwareMode == SCSI_HMODE_AICTRIDENT)
         {
            if(SCSI_DEVICE_TABLE(hhcb)[i].bayScsiRate & SCSI_SINGLE_EDGE)
            {
               /* set for single edge xfer, turn off CRC */
               SCSI_DEVICE_TABLE(hhcb)[i].bayScsiRate &= ~SCSI_ENABLE_CRC;
            }
            else
            {
               /* set for dual edge xfer, turn on CRC */
               SCSI_DEVICE_TABLE(hhcb)[i].bayScsiRate |= SCSI_ENABLE_CRC;
            }
         }

         /* Initialize the default rate */
         SCSI_DEVICE_TABLE(hhcb)[i].bayDefaultRate = 
            SCSI_DEVICE_TABLE(hhcb)[i].bayScsiRate;

         /* load scsi offset option */
         SCSI_DEVICE_TABLE(hhcb)[i].bayDefaultOffset = 
            SCSI_DEVICE_TABLE(hhcb)[i].bayScsiOffset = (SCSI_UEXACT8)
               ((nvmData.targetControl.bayonetTC[i] & SCSI_BAYONETTC_INITIATESYNC) ? 127 : 0);

         /* build disconnectEnable */
         SCSI_DEVICE_TABLE(hhcb)[i].SCSI_DF_disconnectEnable =
               (nvmData.targetControl.bayonetTC[i] & SCSI_BAYONETTC_DISCONNECTENABLE) >> 4;

         /* build multiple lun support */
/*----------------------- Temporary comments out ----------------
         multipleLun |= ((nvmData.targetControl.bayonetTC[i] & SCSI_BAYONETTC_MULTIPLELUN) >> 11) << i;
---------------------------------------------------------------*/

        /* build hostManaged setting - NTC specific field */
        SCSI_DEVICE_TABLE(hhcb)[i].SCSI_DF_hostManaged =
              (nvmData.targetControl.bayonetTC[i] & SCSI_BAYONETTC_HOSTMANAGED) >> 9;
      }
   }
#endif   /* SCSI_AICBAYONET */

/*----------------------- Temporary comments out ----------------
   SCSI_ADPTSH(adapterTSH)->multipleLun = multipleLun;

   if ((!multipleLun) && (nvmData.generalControl & SCSI_GENERALCONTROL_MULTIPLELUN))
   {
      SCSI_ADPTSH(adapterTSH)->multipleLun = 0xFFFF;
   }
----------------------------------------------------------------*/

   /* apply bios control configuration if SCAM enabled */
#if SCSI_SCAM_ENABLE
   hhcb->SCSI_HF_scamLevel = (nvmData.biosControl & SCSI_BIOSCONTROL_SCAMENABLE) >> 8;
#else
   hhcb->SCSI_HF_scamLevel = 0;
#endif

#if SCSI_DOMAIN_VALIDATION
   /* apply bios control configuration if DOMAIN VALIDATION enabled, for */
   /* bayonet or trident only */
   if ((hhcb->hardwareMode == SCSI_HMODE_AICBAYONET) ||
       (hhcb->hardwareMode == SCSI_HMODE_AICTRIDENT))
   {
      if ((nvmData.generalControl & SCSI_GENERALCONTROL_DOMAINVALIDATION))
      {
#if SCSI_DOMAIN_VALIDATION_ENHANCED
            SCSI_ADPTSH(adapterTSH)->domainValidationMethod = SCSI_DV_ENHANCED;
#else /* SCSI_DOMAIN_VALIDATION_ENHANCED */
            SCSI_ADPTSH(adapterTSH)->domainValidationMethod = SCSI_DV_BASIC;
#endif /* SCSI_DOMAIN_VALIDATION_ENHANCED */
      }
      else
      {
         SCSI_ADPTSH(adapterTSH)->domainValidationMethod = SCSI_DV_DISABLE;
      }
   }
   else
   {
      SCSI_ADPTSH(adapterTSH)->domainValidationMethod = SCSI_DV_DISABLE;
   }
#else
   SCSI_ADPTSH(adapterTSH)->domainValidationMethod = SCSI_DV_DISABLE;
#endif

   /* apply general control configuration */ 
#if (!SCSI_ARO_SUPPORT)
   hhcb->SCSI_HF_terminationLow = (nvmData.generalControl & SCSI_GENERALCONTROL_TERMINATIONLOW) >> 2;
   hhcb->SCSI_HF_terminationHigh = (nvmData.generalControl & SCSI_GENERALCONTROL_TERMINATIONHIGH) >> 3;
   hhcb->SCSI_HF_autoTermination = (nvmData.generalControl & SCSI_GENERALCONTROL_AUTOTERMINATION);

#if (SCSI_AICBAYONET || SCSI_AICTRIDENT)
   /* Need to check for the legacy bit of the Subsystem ID Values */
   if (hhcb->SCSI_SUBID_legacyConnector &&
       ((hhcb->hardwareMode == SCSI_HMODE_AICBAYONET) ||
        (hhcb->hardwareMode == SCSI_HMODE_AICTRIDENT)))
   {
      /* apply secondary terminations */
      hhcb->SCSI_HF_secondaryTermLow = (nvmData.generalControl & SCSI_GENERALCONTROL_SECONDARYTERMLOW) >> 11;
      hhcb->SCSI_HF_secondaryTermHigh = (nvmData.generalControl & SCSI_GENERALCONTROL_SECONDARYTERMHIGH) >> 12;
      hhcb->SCSI_HF_secondaryAutoTerm = (nvmData.generalControl & SCSI_GENERALCONTROL_SECONDARYAUTOTERM) >> 10;
   }
#endif /* SCSI_AICBAYONET || SCSI_AICTRIDENT */

#endif /* !SCSI_ARO_SUPPORT */

   /* Need to check for the valid signature */
   if (nvmData.signature == SCSI_NVM_SIGNATURE)
   {
      /* set updateTermLevel and get termination power level (STPWLEVEL) bit */
      hhcb->SCSI_HF_updateTermLevel = 1;
      hhcb->SCSI_HF_terminationLevel = (nvmData.biosControl & SCSI_BIOSCONTROL_STPWLEVEL) >> 4;
   }

#if SCSI_PARITY_PER_IOB
   /* ParityEnable in NVRAM no longer used for per iob parity checking */ 
   hhcb->SCSI_HF_scsiParity = 0;
#else
   hhcb->SCSI_HF_scsiParity = (nvmData.generalControl & SCSI_GENERALCONTROL_PARITYENABLE) >> 4;
#endif
   hhcb->SCSI_HF_resetSCSI = (nvmData.generalControl & SCSI_GENERALCONTROL_RESETSCSI) >> 6;
   hhcb->SCSI_HF_clusterEnable = (nvmData.generalControl & SCSI_GENERALCONTROL_CLUSTERENABLE) >> 7;

   if (hhcb->hardwareMode == SCSI_HMODE_AIC78XX)
   {
      if ((!ultraEnable) && (nvmData.generalControl & SCSI_GENERALCONTROL_ULTRAENABLE))
      {
         for (i = 0; i < (SCSI_UINT)(nvmData.word20 & SCSI_WORD20_MAXTARGETS); i++)
         {
            SCSI_DEVICE_TABLE(hhcb)[i].SCSI_DF_ultraEnable = 1;
         }
      }
   }

   /* apply word 19, 20 configuration */
   /* scsiID, maxTargets, bootLun, bootID ??? */
   hhcb->busRelease = (SCSI_UEXACT8)((nvmData.word19 & SCSI_WORD19_BUSRELEASE) >> 8);

   /* To cover the case where the host SCSI id might get changed */
   /* in the scam level 2 environment. */
   if ((!SCSIxBiosExist(adapterTSH)) && (!SCSIxOptimaExist(adapterTSH)))
   {
#if SCSI_TARGET_OPERATION && SCSI_MULTIPLEID_SUPPORT
      if (hhcb->SCSI_HF_targetMode) 
      {
         /* revisit - if the hostScsiID changes then we need
          * to alter the mask removing the old hostScsiID
          * value from the mask and adding the new one to
          * the mask.
          */
         if (hhcb->hostScsiID != ((SCSI_UEXACT8)(nvmData.word19 & SCSI_WORD19_SCSIID))
         {
            hhcb->SCSI_HF_targetAdapterIDMask &= (~(1 << (hhcb->hostScsiID)));
            hhcb->hostScsiID = (SCSI_UEXACT8)(nvmData.word19 & SCSI_WORD19_SCSIID);
            hhcb->SCSI_HF_targetAdapterIDMask |= (1 << (hhcb->hostScsiID));  
         }
      } 
#else
      hhcb->hostScsiID = (SCSI_UEXACT8)(nvmData.word19 & SCSI_WORD19_SCSIID);
#endif /* SCSI_TARGET_OPERATION && SCSI_MULTIPLEID_SUPPORT */
   }

#if SCSI_TARGET_OPERATION
   if ((hhcb->SCSI_HF_targetMode) && (!hhcb->SCSI_HF_initiatorMode))
   {
      /* Set the following device table entries based on contents */
      /* of the device table entry of the SCSI ID of the adapter  */
      /* SCSI_TARGET_OPERATION ONLY.                              */  
      /* AIC-78XX NVRAM targets layout */
      if (hhcb->hardwareMode == SCSI_HMODE_AIC78XX)
      {
         for (i = 0; i < hhcb->maxDevices; i++)
         {
            /* load scsiOption */
            SCSI_DEVICE_TABLE(hhcb)[i].scsiOption = 
               SCSI_DEVICE_TABLE(hhcb)[hhcb->hostScsiID].scsiOption;
         
            /* wide setting */
            SCSI_DEVICE_TABLE(hhcb)[i].SCSI_DF_setForWide = 
                ((SCSI_DEVICE_TABLE(hhcb)[i].scsiOption & (SCSI_WIDEXFER)) !=0);

            /* sync setting */
            SCSI_DEVICE_TABLE(hhcb)[i].SCSI_DF_setForSync = 
                ((SCSI_DEVICE_TABLE(hhcb)[i].scsiOption & (SCSI_SOFS0)) !=0);

            /* build disconnectEnable */
            SCSI_DEVICE_TABLE(hhcb)[i].SCSI_DF_disconnectEnable =
                SCSI_DEVICE_TABLE(hhcb)[hhcb->hostScsiID].SCSI_DF_disconnectEnable;

            /* build ultraEnable */
            SCSI_DEVICE_TABLE(hhcb)[i].SCSI_DF_ultraEnable = 
                SCSI_DEVICE_TABLE(hhcb)[hhcb->hostScsiID].SCSI_DF_ultraEnable;
         }
      }
#if (SCSI_AICBAYONET || SCSI_AICTRIDENT)
      /* Bayonet NVRAM targets layout */
      else
      {
         for (i = 0; i < (SCSI_UINT)(nvmData.word20 & SCSI_WORD20_MAXTARGETS); i++)
         {
            /* load scsiOption */
            SCSI_DEVICE_TABLE(hhcb)[i].scsiOption = 
               SCSI_DEVICE_TABLE(hhcb)[hhcb->hostScsiID].scsiOption;

            /* wide setting */
            SCSI_DEVICE_TABLE(hhcb)[i].SCSI_DF_setForWide = 
                ((SCSI_DEVICE_TABLE(hhcb)[i].scsiOption & (SCSI_WIDEXFER)) !=0);

            /* sync setting */
            SCSI_DEVICE_TABLE(hhcb)[i].SCSI_DF_setForSync = 
                ((SCSI_DEVICE_TABLE(hhcb)[i].scsiOption & (SCSI_SOFS0)) !=0);
 
            /* load scsi rate option and wide bit */
            SCSI_DEVICE_TABLE(hhcb)[i].bayScsiRate = 
               SCSI_DEVICE_TABLE(hhcb)[hhcb->hostScsiID].bayScsiRate;

            /* load scsi offset option */
            SCSI_DEVICE_TABLE(hhcb)[i].bayScsiOffset = 
               SCSI_DEVICE_TABLE(hhcb)[hhcb->hostScsiID].bayScsiOffset;

            /* build disconnectEnable */
            SCSI_DEVICE_TABLE(hhcb)[i].SCSI_DF_disconnectEnable =
               SCSI_DEVICE_TABLE(hhcb)[hhcb->hostScsiID].SCSI_DF_disconnectEnable;

         }
      }
#endif /* SCSI_AICBAYONET || SCSI_AICTRIDENT */      
   }
#endif /* SCSI_TARGET_OPERATION */

   return(0);   
#else /* !SCSI_SCSISELECT_SUPPORT */
   return(1);
#endif /* SCSI_SCSISELECT_SUPPORT */
}

/*********************************************************************
*
*  SCSIxBiosExist
*
*     Check to see whether BIOS is exist or not
*
*  Return Value:  1 if exist
*                 0 if not exist
*                  
*  Parameters:    adapterTSH
*
*  Remarks:
*
*********************************************************************/
SCSI_UINT8 SCSIxBiosExist (HIM_TASK_SET_HANDLE adapterTSH)
{
   SCSI_UINT8  retval = 0;    /* assuming BIOS does not exist */
#if SCSI_BIOS_ASPI8DOS
   SCSI_HHCB HIM_PTR hhcb = &SCSI_ADPTSH(adapterTSH)->hhcb;
   SCSI_UEXACT32 bios13vect, biosintvect;
   int scbState;

   if (!hhcb->SCSI_HF_chipReset)
   {
      scbState = SCSIHDisableExtScbRam(hhcb);

      SCSIHReadScbRAM(hhcb,(SCSI_UEXACT8 HIM_PTR) &bios13vect,
            (SCSI_UEXACT8) 2,
            (SCSI_UEXACT8)OSMoffsetof(SCSI_BIOS_SCB2,bios13Vector),
            (SCSI_UEXACT8) 4);
      SCSIHReadScbRAM(hhcb,(SCSI_UEXACT8 HIM_PTR) &biosintvect,
            (SCSI_UEXACT8) 2,
            (SCSI_UEXACT8)OSMoffsetof(SCSI_BIOS_SCB2,biosIntVector),
            (SCSI_UEXACT8) 4);

      if (bios13vect && (bios13vect != (SCSI_UEXACT32)0xFFFFFFFF) &&
          biosintvect && (biosintvect != (SCSI_UEXACT32)0xFFFFFFFF))
      {
         retval = 1;
      }

      if (scbState)
      {
         SCSIHEnableExtScbRam(hhcb);
      }
   }
#endif /* SCSI_BIOS_ASPI8DOS */
   return (retval);
}

/*********************************************************************
*
*  SCSIxOptimaExist
*
*     Check to see whether old optima mode is exist or not
*
*  Return Value:  1 if exist
*                 0 if not exist
*                  
*  Parameters:    adapterTSH
*
*  Remarks:
*
*********************************************************************/
SCSI_UINT8 SCSIxOptimaExist (HIM_TASK_SET_HANDLE adapterTSH)
{
#if SCSI_BIOS_ASPI8DOS
   SCSI_HHCB HIM_PTR hhcb = &SCSI_ADPTSH(adapterTSH)->hhcb;
   SCSI_UEXACT8 nextScbArray[16];
   SCSI_UEXACT32 optimaQinArray;
   SCSI_UEXACT32 optimaQoutArray;
#if SCSI_AIC78XX
   SCSI_UEXACT32 haddr;
#endif /* SCSI_AIC78XX */
   int i;

   if (!hhcb->SCSI_HF_chipReset)
   {
      /* load contents of next scb array */
      SCSIHReadScratchRam(hhcb,nextScbArray,SCSI_OPTIMA_NEXT_SCB_ARRAY,16);

      /* check whether next scb array are 0x7F */
      for (i = 0; i < 16; i++)
      {
         if (nextScbArray[i] != 0x7F)
         {
            return(0);
         }
      }
      
      /* load contents of optima QIN pointer array */
      SCSIHReadScratchRam(hhcb,(SCSI_UEXACT8 HIM_PTR) &optimaQinArray,
                          SCSI_OPTIMA_QIN_PTR_ARRAY,4);

      /* load contents of optima QOUT pointer array */
      SCSIHReadScratchRam(hhcb,(SCSI_UEXACT8 HIM_PTR) &optimaQoutArray,
                          SCSI_OPTIMA_QOUT_PTR_ARRAY,4);

#if SCSI_AIC78XX
      if (hhcb->hardwareMode == SCSI_HMODE_AIC78XX)
      {
         /* load contents of haddr */
         SCSIHReadHADDR(hhcb,(SCSI_UEXACT8 HIM_PTR) &haddr);

         if (optimaQinArray && (optimaQinArray != (SCSI_UEXACT32)0xFFFFFFFF) &&
             optimaQoutArray && (optimaQoutArray != (SCSI_UEXACT32)0xFFFFFFFF) &&
             haddr)
         {
            return(1);
         }
      }
#endif /* SCSI_AIC78XX */

#if (SCSI_AICBAYONET || SCSI_AICTRIDENT)
      if ((hhcb->hardwareMode == SCSI_HMODE_AICBAYONET) ||
          (hhcb->hardwareMode == SCSI_HMODE_AICTRIDENT))
      {
         /* Do not need to check for HADDR because the BIOS always reset */
         /* the chip when it's not installed. */
         if (optimaQinArray && (optimaQinArray != (SCSI_UEXACT32)0xFFFFFFFF) &&
             optimaQoutArray && (optimaQoutArray != (SCSI_UEXACT32)0xFFFFFFFF))
         {
             return(1);
         }
      }
#endif /* SCSI_AICBAYONET || SCSI_AICTRIDENT */
   }
#endif /* SCSI_BIOS_ASPI8DOS */
   return (0);
}

/*********************************************************************
*
*  SCSIxGetBiosInformation
*
*     Get bios information
*
*  Return Value:  none
*                  
*  Parameters:    adapterTSH
*
*  Remarks:
*
*********************************************************************/
#if SCSI_BIOS_ASPI8DOS
void SCSIxGetBiosInformation (HIM_TASK_SET_HANDLE adapterTSH)
{
   SCSI_HHCB HIM_PTR hhcb = &SCSI_ADPTSH(adapterTSH)->hhcb;
   SCSI_BIOS_INFORMATION HIM_PTR biosInformation =
                                 &SCSI_ADPTSH(adapterTSH)->biosInformation;
   SCSI_BIOS_SCB2 biosScb2;
   SCSI_UINT8 i;
      
   /* check if bios exist */
   if (SCSIxBiosExist(adapterTSH))
   {
      SCSIHReadScbRAM(hhcb,(SCSI_UEXACT8 HIM_PTR) &biosScb2,
         (SCSI_UEXACT8)2,(SCSI_UEXACT8)0,(SCSI_UEXACT8)32);

      biosInformation->biosFlags.biosActive = 1;
      biosInformation->biosFlags.dos5orAfter = (SCSI_UINT8)((biosScb2.biosGlobal & SCSI_BIOSGLOBAL_DOS5) >> 6);
      biosInformation->biosFlags.extendedTranslation = (SCSI_UINT8)((biosScb2.biosGlobal & SCSI_BIOSGLOBAL_GIGA) >> 8);
      biosInformation->firstBiosDrive = (biosScb2.driveRange & SCSI_DRIVERANGE_FIRSTDRIVE);
      biosInformation->lastBiosDrive = (biosScb2.driveRange & SCSI_DRIVERANGE_LASTDRIVE) >> 4;
      biosInformation->numberDrives = (SCSI_UINT8)
                                    (biosInformation->lastBiosDrive -
                                    biosInformation->firstBiosDrive + 1);
         
      /* load drive scsi/lun id */
      for (i = 0; i < biosInformation->numberDrives; i++)
      {
         biosInformation->biosDrive[i].targetID = (biosScb2.biosDrive[i] & SCSI_BIOSDRIVE_TARGETID);
         biosInformation->biosDrive[i].lunID = (biosScb2.biosDrive[i] & SCSI_BIOSDRIVE_LUNID) >> 4;
      }

      /* apply bios control configuration for SCAM */
#if SCSI_SCAM_ENABLE
      biosInformation->biosFlags.scamenable = 
                              (biosScb2.biosGlobal & SCSI_BIOSGLOBAL_SCAMENABLE) ? 1 : 0;
      hhcb->SCSI_HF_scamLevel = (biosScb2.biosGlobal & SCSI_BIOSGLOBAL_SCAMENABLE) >> 13;
#endif
   }
}
#endif /* SCSI_BIOS_ASPI8DOS */

/*********************************************************************
*
*  SCSIxChkLunExist
*
*     Check if the specified lun exist in the lun/tsk exist table
*
*  Return Value:  1 - lun/target exist
*                 0 - not exist
*                  
*  Parameters:    lunExist table
*                 target id
*                 lun id
*
*  Remarks:
*
*********************************************************************/
SCSI_UINT8 SCSIxChkLunExist (SCSI_UEXACT8 HIM_PTR lunExist,
                             SCSI_UINT8 target,
                             SCSI_UINT8 lun)
{
   SCSI_UINT index = (SCSI_UINT)(target * SCSI_MAXLUN + lun);

   return((SCSI_UINT8)((lunExist[index/8] & (1 << (index % 8))) != 0));
}

/*********************************************************************
*
*  SCSIxSetLunExist
*
*     Set the lun exist in lun/tsh exist table
*
*  Return Value:  none
*                  
*  Parameters:    lunExist table
*                 target id
*                 lun id
*
*  Remarks:
*
*********************************************************************/
void SCSIxSetLunExist (SCSI_UEXACT8 HIM_PTR lunExist,
                       SCSI_UINT8 target,
                       SCSI_UINT8 lun)
{
   SCSI_UINT index = (SCSI_UINT)(target * SCSI_MAXLUN + lun);

   lunExist[index/8] |= (SCSI_UEXACT8) (1 << (index % 8));
}

/*********************************************************************
*
*  SCSIxClearLunExist
*
*     Clear the lun exist in lun/tsh exist table
*
*  Return Value:  none
*                  
*  Parameters:    lunExist table
*                 target id
*                 lun id
*
*  Remarks:
*
*********************************************************************/
void SCSIxClearLunExist (SCSI_UEXACT8 HIM_PTR lunExist,
                         SCSI_UINT8 target,
                         SCSI_UINT8 lun)
{
   SCSI_UINT index = (SCSI_UINT)(target * SCSI_MAXLUN + lun);

   lunExist[index/8] &= (SCSI_UEXACT8)(~(1 << (index % 8)));
}

/*********************************************************************
*
*  SCSIxSetupBusScan
*
*     Setup for SCSI bus scan
*
*  Return Value:  0 - All targets are marked with 0 luns.
*                     No need to call SCSIxQueueBusScan.
*                 1 - It is ok to proceed with SCSIxQueueBusScan.
*
*  Parameters:    adapterTSH
*
*  Remarks:
*
*********************************************************************/
SCSI_UINT8 SCSIxSetupBusScan (SCSI_ADAPTER_TSCB HIM_PTR adapterTSH)
{
   SCSI_TARGET_TSCB HIM_PTR targetTSCB = &adapterTSH->targetTSCBBusScan;
#if (SCSI_AICBAYONET || SCSI_AICTRIDENT || SCSI_NULL_SELECTION)
   SCSI_HHCB SCSI_HPTR hhcb = &adapterTSH->hhcb;
#endif /* SCSI_AICBAYONET || SCSI_AICTRIDENT || SCSI_NULL_SELECTION */
#if (SCSI_AICBAYONET || SCSI_AICTRIDENT || SCSI_DOMAIN_VALIDATION) 
   int i;
#endif /* (SCSI_AICBAYONET || SCSI_AICTRIDENT || SCSI_DOMAIN_VALIDATION) */ 

   /* fresh the lunExist table */
   OSDmemset(adapterTSH->lunExist,0,SCSI_MAXDEV*SCSI_MAXLUN/8);

   /* fresh the bus scan state machine (start with target 0, lun 0) */
   OSDmemset(targetTSCB,0,sizeof(SCSI_TARGET_TSCB));

   /* setup targetTSCB */
   targetTSCB->typeTSCB   = SCSI_TSCB_TYPE_TARGET;
   targetTSCB->adapterTSH = adapterTSH;

   /* The following loop skip target(s) with NumberLuns == 0   */
   for (;targetTSCB->scsiID < adapterTSH->hhcb.maxDevices;)
   {
      if (adapterTSH->NumberLuns[targetTSCB->scsiID] == 0)
      {
         /* If NumberLuns specified for the target is 0, skip this target */
         ++targetTSCB->scsiID;
         continue;
      }

      if (targetTSCB->scsiID == adapterTSH->hhcb.hostScsiID)
      {
#if !SCSI_NULL_SELECTION
         /* skip the host scsi id */
         ++targetTSCB->scsiID;
         continue;
#else
         /* Don't skip host scsi id - attempt Null Selection */
         /* Can't disconnect on Null Selection */
         SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].SCSI_DF_disconnectEnable = 0;
         break;
#endif /* !SCSI_NULL_SELECTION */
      }
      
      break;
   }

   if (targetTSCB->scsiID >= adapterTSH->hhcb.maxDevices)
   {
      /* This may happen if all targets are specified with NumberLuns==0 */
      /* For this case no actual scanning need to be done.               */
      return 0;
   }

   /* reset tag enable */
   adapterTSH->tagEnable = 0;

   adapterTSH->retryBusScan = 0;

#if (SCSI_AICBAYONET || SCSI_AICTRIDENT)
   if (hhcb->SCSI_HF_expSupport)
   {
      /* invalidate the bus expander status */
      for (i = 0; i<SCSI_MAXDEV; i++)
      {
         hhcb->deviceTable[i].SCSI_DF_behindExp = 0;
      }
   }
#endif /* SCSI_AICBAYONET || SCSI_AICTRIDENT */

#if SCSI_DOMAIN_VALIDATION
   for (i = 0; i < 16; i++)
   {
      /* disable domain validation as default */
      adapterTSH->SCSI_AF_dvLevel(i) = SCSI_DV_DISABLE;
   }
#endif   /* SCSI_DOMAIN_VALIDATION */

#if SCSI_PAC_NSX_REPEATER
   /* initialize SPI device index and spi_speed_controller.devices */
   adapterTSH->speedDeviceIndex = 0;
   adapterTSH->speedController.devices = HIM_NULL;
#endif /* SCSI_PAC_NSX_REPEATER */

   /* This indicates that its ok to proceed with bus scan process */
   return 1;
}

/*********************************************************************
*
*  SCSIxSetupLunProbe
*
*     Setup to probe a LUN.
*
*  Return Value:  none
*                  
*  Parameters:    adapterTSH
*
*  Remarks:
*
*********************************************************************/
#if !SCSI_DISABLE_PROBE_SUPPORT
void SCSIxSetupLunProbe (SCSI_ADAPTER_TSCB HIM_PTR adapterTSH, 
                         SCSI_UEXACT8 target,
                         SCSI_UEXACT8 lun)
{
   SCSI_TARGET_TSCB HIM_PTR targetTSCB = &adapterTSH->targetTSCBBusScan;
#if SCSI_DOMAIN_VALIDATION
   SCSI_UINT16 i;
#endif /* SCSI_DOMAIN_VALIDATION */

   /* fresh the lunExist entry */
   SCSIxClearLunExist(adapterTSH->lunExist, (SCSI_UINT8)target, (SCSI_UINT8)lun);

   /* @dm nt5 - TSH invalid workaround for NT 5.0 start */
   /* This is a workaround for nt 5.0.  With NT 5.0, the targetTSH and TSCB is*/
   /* stored in the LU extension. The problem is that with NT 5.0, the OS can */
   /* clear the LU extension memory at any given time without warning.  If    */
   /* this occurs, then the OSM has no choice, but to probe for the target/lun*/
   /* for the new request when the LU extension clearing is detected.  If this*/
   /* occurs, then CHIM rules say that you cant probe for a device that has a */
   /* preexisting tsh.  This code is added to clear the tsh exist flag for the*/
   /* target/lun (in other words, dont error check this case) to workaround   */
   /* NT 5.0 problem.                                                         */
   /* fresh the lunExist entry */
   SCSIxClearLunExist(adapterTSH->tshExist, (SCSI_UINT8)target, (SCSI_UINT8)lun);
   /* @dm nt5 - TSH invalid workaround for NT 5.0 end */

   /* fresh the target TSCB */
   OSDmemset(targetTSCB,0,sizeof(SCSI_TARGET_TSCB));
   targetTSCB->scsiID = target;
   targetTSCB->lunID = lun;

   /* setup targetTSCB */
   targetTSCB->typeTSCB = SCSI_TSCB_TYPE_TARGET;
   targetTSCB->adapterTSH = adapterTSH;

   if (targetTSCB->lunID == 0)
   {
      /* reset tag enable */
      adapterTSH->tagEnable &= (SCSI_UEXACT16)(~(1 << targetTSCB->scsiID));

#if SCSI_PAC_NSX_REPEATER
      /* Need to search SPI device table and remove entry if it already exists */
      SCSI_OEM1_REMOVE_SPI_ENTRY(adapterTSH, targetTSCB->scsiID);
#endif /* SCSI_PAC_NSX_REPEATER */        
   }

   adapterTSH->retryBusScan = 0;

#if SCSI_DOMAIN_VALIDATION
   for (i = 0; i < 16; i++)
   {
      /* disable domain validation as default */
      adapterTSH->SCSI_AF_dvLevel(i) = SCSI_DV_DISABLE;
   }
#endif   /* SCSI_DOMAIN_VALIDATION */

#if SCSI_NULL_SELECTION
   if (targetTSCB->scsiID == adapterTSH->hhcb.hostScsiID)
   {
      /* Can't disconnect on Null Selection */
      SCSI_DEVICE_TABLE(&adapterTSH->hhcb)[targetTSCB->scsiID].SCSI_DF_disconnectEnable = 0;
   }  
#endif /* SCSI_NULL_SELECTION */

}
#endif /* !SCSI_DISABLE_PROBE_SUPPORT */

#if SCSI_NSX_REPEATER + SCSI_PAC_NSX_REPEATER + SCSI_PAC_SEND_SSU
/*******************************************************************************
*
*  SCSIxQueueSSU
*
*     Queue Start/Stop Unit request. It is assumed that this routine is called
*     only during PAC (not HIM_PROBE). Also it is assumed that this routine is
*     called only for LUN 0.
*
*  Return Value:  none
*                  
*  Parameters:    adapterTSH
*
*  Remarks:
*
*******************************************************************************/
void SCSIxQueueSSU (SCSI_ADAPTER_TSCB HIM_PTR adapterTSH)
{
   SCSI_TARGET_TSCB HIM_PTR targetTSCB = &adapterTSH->targetTSCBBusScan;
   SCSI_HHCB HIM_PTR hhcb = &adapterTSH->hhcb;
   HIM_IOB HIM_PTR iob;
   HIM_UEXACT8 HIM_PTR uexact8Pointer;

   /* setup next iob request (inquiry) for bus scan */
   /* get iob memory */
   iob = (HIM_IOB HIM_PTR)(((SCSI_UEXACT8 HIM_PTR)adapterTSH->moreUnlocked) + SCSI_MORE_IOB);
   OSDmemset(iob,0,sizeof(HIM_IOB));

   /* setup iob reserved memory */
   iob->iobReserve.virtualAddress = (void HIM_PTR) (((SCSI_UEXACT8 HIM_PTR)
               adapterTSH->moreLocked) + SCSI_MORE_IOBRSV);
   iob->iobReserve.bufferSize = sizeof(SCSI_IOB_RESERVE);
   iob->iobReserve.busAddress = OSMxGetBusAddress(SCSI_ADPTSH(adapterTSH)->osmAdapterContext,
                  HIM_MC_LOCKED,iob->iobReserve.virtualAddress);
   OSDmemset(iob->iobReserve.virtualAddress,0,sizeof(SCSI_IOB_RESERVE));

   /* setup/validate temporary target task set handle */
   SCSI_SET_UNIT_HANDLE(&SCSI_ADPTSH(adapterTSH)->hhcb,
         &targetTSCB->unitControl,(SCSI_UEXACT8)targetTSCB->scsiID,
         (SCSI_UEXACT8)targetTSCB->lunID);

   /* setup iob for inquiry */
   iob->function = HIM_INITIATE_TASK;
   iob->taskSetHandle = (HIM_TASK_SET_HANDLE) targetTSCB;
   iob->flagsIob.autoSense = 1;
   iob->postRoutine = (HIM_POST_PTR) SCSIxPostSSU;
   iob->targetCommand = (void HIM_PTR) (((SCSI_UEXACT8 HIM_PTR) adapterTSH->moreUnlocked) + SCSI_MORE_INQCDB);
   iob->targetCommandLength = 6;

   /* setup sense data */
   iob->errorData = (void HIM_PTR) (((SCSI_UEXACT8 HIM_PTR) adapterTSH->moreLocked) + SCSI_MORE_SNSDATA);
   iob->errorDataLength = 32;
   iob->taskAttribute = HIM_TASK_SIMPLE;

   /* Disable tagging */
   targetTSCB->targetAttributes.tagEnable = 0;
 
   /* setup cdb */
   uexact8Pointer = (SCSI_UEXACT8 HIM_PTR) iob->targetCommand;
   uexact8Pointer[0] = (SCSI_UEXACT8) 0x1B;
   uexact8Pointer[1] = (SCSI_UEXACT8) 0;
   uexact8Pointer[2] = (SCSI_UEXACT8) 0;
   uexact8Pointer[3] = (SCSI_UEXACT8) 0;
   uexact8Pointer[4] = (SCSI_UEXACT8) 0x01;
   uexact8Pointer[5] = (SCSI_UEXACT8) 0;

   /* execute it */
   SCSIQueueIOB(iob);
}

/*********************************************************************
*
*  SCSIxPostSSU
*
*     Verify SSU status
*
*  Return Value:  status
*                  
*  Parameters:    adapterTSH
*
*  Remarks:
*
*********************************************************************/
HIM_UINT32 SCSIxPostSSU (HIM_IOB HIM_PTR iob)
{
   SCSI_TARGET_TSCB HIM_PTR targetTSCB =
                              (SCSI_TARGET_TSCB HIM_PTR) iob->taskSetHandle;
   SCSI_UNIT_CONTROL HIM_PTR unitHandle = &targetTSCB->unitControl;
   SCSI_ADAPTER_TSCB HIM_PTR adapterTSH = targetTSCB->adapterTSH;
   SCSI_HHCB HIM_PTR hhcb = &adapterTSH->hhcb;

   /* check iob status to verify if device exist */
   switch(iob->taskStatus)
   {
      case HIM_IOB_GOOD:
      case HIM_IOB_DATA_OVERUNDERRUN:
         /* TUR was successful; Handle INQ */
         SCSI_FREE_UNIT_HANDLE(*((SCSI_UNIT_HANDLE HIM_PTR)&unitHandle));
         SCSIxQueueTUR(adapterTSH);
         return(HIM_SUCCESS);

      case HIM_IOB_ABORTED_REQ_BUS_RESET:
         /* abort protocol auto config because of a catastrophic event */
         /* post protocol auto config back to osm */
         adapterTSH->iobProtocolAutoConfig->taskStatus = HIM_IOB_ABORTED_REQ_BUS_RESET;
         adapterTSH->iobProtocolAutoConfig->postRoutine(adapterTSH->iobProtocolAutoConfig);
         return(HIM_SUCCESS);

      case HIM_IOB_ABORTED_CHIM_RESET:

         /* abort protocol auto config because of a catastrophic event */
         /* post protocol auto config back to osm */
         adapterTSH->iobProtocolAutoConfig->taskStatus = HIM_IOB_ABORTED_CHIM_RESET;
         adapterTSH->iobProtocolAutoConfig->postRoutine(adapterTSH->iobProtocolAutoConfig);
         return(HIM_SUCCESS);
         
      case HIM_IOB_ABORTED_TRANSPORT_MODE_CHANGE:

         /* abort protocol auto config because of a catastrophic event */
         /* post protocol auto config back to osm */
         adapterTSH->iobProtocolAutoConfig->taskStatus = HIM_IOB_ABORTED_TRANSPORT_MODE_CHANGE;
         adapterTSH->iobProtocolAutoConfig->postRoutine(adapterTSH->iobProtocolAutoConfig);
         return(HIM_SUCCESS);

      case HIM_IOB_ABORTED_3RD_PARTY_RESET:
         /* abort protocol auto config because of a catastrophic event */
         /* post protocol auto config back to osm */
         adapterTSH->iobProtocolAutoConfig->taskStatus = HIM_IOB_ABORTED_3RD_PARTY_RESET;
         adapterTSH->iobProtocolAutoConfig->postRoutine(adapterTSH->iobProtocolAutoConfig);
         return(HIM_SUCCESS);

      case HIM_IOB_HOST_ADAPTER_FAILURE:
         /* abort protocol auto config because of a catastrophic event */
         /* post protocol auto config back to osm */
         adapterTSH->iobProtocolAutoConfig->taskStatus = HIM_IOB_HOST_ADAPTER_FAILURE;
         adapterTSH->iobProtocolAutoConfig->postRoutine(adapterTSH->iobProtocolAutoConfig);
         return(HIM_SUCCESS);

      case HIM_IOB_BUSY:
         /* must free the current unit handle */
         SCSI_FREE_UNIT_HANDLE(*((SCSI_UNIT_HANDLE HIM_PTR)&unitHandle));
      
         SCSIxQueueSpecialIOB(adapterTSH, HIM_UNFREEZE_QUEUE);
         
         /* if not HIM_PROBE, will do retry once */
         if (adapterTSH->retryBusScan <= 30000)
         { 
            /* must free the current unit handle */
            SCSI_FREE_UNIT_HANDLE(*((SCSI_UNIT_HANDLE HIM_PTR)&unitHandle));
            ++adapterTSH->retryBusScan;
            SCSIxQueueSSU(adapterTSH);
            return(HIM_SUCCESS);
         }   
         break;

      default:
         /* treat device exist */
         /* must free the current unit handle */
         SCSI_FREE_UNIT_HANDLE(*((SCSI_UNIT_HANDLE HIM_PTR)&unitHandle));
         SCSIxQueueBusScan(adapterTSH);
         return(HIM_SUCCESS);
   }
}

/*********************************************************************
*
*  SCSIxQueueTUR
*
*     Queue Test Unit Ready request. It is assumed that this routine is called
*     only during PAC (not HIM_PROBE). Also it is assumed that this routine is
*     called only for LUN 0.
*
*  Return Value:  none
*                  
*  Parameters:    adapterTSH
*
*  Remarks:
*
*********************************************************************/
void SCSIxQueueTUR (SCSI_ADAPTER_TSCB HIM_PTR adapterTSH)
{
   SCSI_TARGET_TSCB HIM_PTR targetTSCB = &adapterTSH->targetTSCBBusScan;
   SCSI_HHCB HIM_PTR hhcb = &adapterTSH->hhcb;
   HIM_IOB HIM_PTR iob;
   HIM_UEXACT8 HIM_PTR uexact8Pointer;

   /* setup next iob request (inquiry) for bus scan */
   /* get iob memory */
   iob = (HIM_IOB HIM_PTR)(((SCSI_UEXACT8 HIM_PTR)adapterTSH->moreUnlocked) + SCSI_MORE_IOB);
   OSDmemset(iob,0,sizeof(HIM_IOB));

   /* setup iob reserved memory */
   iob->iobReserve.virtualAddress = (void HIM_PTR) (((SCSI_UEXACT8 HIM_PTR)
               adapterTSH->moreLocked) + SCSI_MORE_IOBRSV);
   iob->iobReserve.bufferSize = sizeof(SCSI_IOB_RESERVE);
   iob->iobReserve.busAddress = OSMxGetBusAddress(SCSI_ADPTSH(adapterTSH)->osmAdapterContext,
                  HIM_MC_LOCKED,iob->iobReserve.virtualAddress);
   OSDmemset(iob->iobReserve.virtualAddress,0,sizeof(SCSI_IOB_RESERVE));

   /* setup/validate temporary target task set handle */
   SCSI_SET_UNIT_HANDLE(&SCSI_ADPTSH(adapterTSH)->hhcb,
         &targetTSCB->unitControl,(SCSI_UEXACT8)targetTSCB->scsiID,
         (SCSI_UEXACT8)targetTSCB->lunID);

   /* setup iob for inquiry */
   iob->function = HIM_INITIATE_TASK;
   iob->taskSetHandle = (HIM_TASK_SET_HANDLE) targetTSCB;
   iob->flagsIob.autoSense = 1;
   iob->postRoutine = (HIM_POST_PTR) SCSIxPostTUR;
   iob->targetCommand = (void HIM_PTR) (((SCSI_UEXACT8 HIM_PTR) adapterTSH->moreUnlocked) + SCSI_MORE_INQCDB);
   iob->targetCommandLength = 6;

   /* setup sense data */
   iob->errorData = (void HIM_PTR) (((SCSI_UEXACT8 HIM_PTR) adapterTSH->moreLocked) + SCSI_MORE_SNSDATA);
   iob->errorDataLength = 32;
   iob->taskAttribute = HIM_TASK_SIMPLE;

   /* Disable tagging */
   targetTSCB->targetAttributes.tagEnable = 0;
 
   /* setup cdb */
   uexact8Pointer = (SCSI_UEXACT8 HIM_PTR) iob->targetCommand;
   uexact8Pointer[0] = (SCSI_UEXACT8) 0;
   uexact8Pointer[1] = (SCSI_UEXACT8) 0;
   uexact8Pointer[2] = (SCSI_UEXACT8) 0;
   uexact8Pointer[3] = (SCSI_UEXACT8) 0;
   uexact8Pointer[4] = (SCSI_UEXACT8) 0;
   uexact8Pointer[5] = (SCSI_UEXACT8) 0;

   /* execute it */
   SCSIQueueIOB(iob);
}

/*********************************************************************
*
*  SCSIxPostTUR
*
*     Verify TUR status
*
*  Return Value:  status
*                  
*  Parameters:    adapterTSH
*
*  Remarks:
*
*********************************************************************/
HIM_UINT32 SCSIxPostTUR (HIM_IOB HIM_PTR iob)
{
   SCSI_TARGET_TSCB HIM_PTR targetTSCB =
                              (SCSI_TARGET_TSCB HIM_PTR) iob->taskSetHandle;
   SCSI_UNIT_CONTROL HIM_PTR unitHandle = &targetTSCB->unitControl;
   SCSI_ADAPTER_TSCB HIM_PTR adapterTSH = targetTSCB->adapterTSH;
   SCSI_HHCB HIM_PTR hhcb = &adapterTSH->hhcb;

   /* check iob status to verify if device exist */
   switch(iob->taskStatus)
   {
      case HIM_IOB_GOOD:
      case HIM_IOB_DATA_OVERUNDERRUN:
         /* TUR was successful; Handle INQ */
         SCSI_FREE_UNIT_HANDLE(*((SCSI_UNIT_HANDLE HIM_PTR)&unitHandle));
         SCSIxQueueBusScan(adapterTSH);
         return(HIM_SUCCESS);

      case HIM_IOB_ABORTED_REQ_BUS_RESET:
         /* abort protocol auto config because of a catastrophic event */
         /* post protocol auto config back to osm */
         adapterTSH->iobProtocolAutoConfig->taskStatus = HIM_IOB_ABORTED_REQ_BUS_RESET;
         adapterTSH->iobProtocolAutoConfig->postRoutine(adapterTSH->iobProtocolAutoConfig);
         return(HIM_SUCCESS);

      case HIM_IOB_ABORTED_CHIM_RESET:

         /* abort protocol auto config because of a catastrophic event */
         /* post protocol auto config back to osm */
         adapterTSH->iobProtocolAutoConfig->taskStatus = HIM_IOB_ABORTED_CHIM_RESET;
         adapterTSH->iobProtocolAutoConfig->postRoutine(adapterTSH->iobProtocolAutoConfig);
         return(HIM_SUCCESS);
         
      case HIM_IOB_ABORTED_TRANSPORT_MODE_CHANGE:

         /* abort protocol auto config because of a catastrophic event */
         /* post protocol auto config back to osm */
         adapterTSH->iobProtocolAutoConfig->taskStatus = HIM_IOB_ABORTED_TRANSPORT_MODE_CHANGE;
         adapterTSH->iobProtocolAutoConfig->postRoutine(adapterTSH->iobProtocolAutoConfig);
         return(HIM_SUCCESS);

      case HIM_IOB_ABORTED_3RD_PARTY_RESET:
         /* abort protocol auto config because of a catastrophic event */
         /* post protocol auto config back to osm */
         adapterTSH->iobProtocolAutoConfig->taskStatus = HIM_IOB_ABORTED_3RD_PARTY_RESET;
         adapterTSH->iobProtocolAutoConfig->postRoutine(adapterTSH->iobProtocolAutoConfig);
         return(HIM_SUCCESS);

      case HIM_IOB_HOST_ADAPTER_FAILURE:
         /* abort protocol auto config because of a catastrophic event */
         /* post protocol auto config back to osm */
         adapterTSH->iobProtocolAutoConfig->taskStatus = HIM_IOB_HOST_ADAPTER_FAILURE;
         adapterTSH->iobProtocolAutoConfig->postRoutine(adapterTSH->iobProtocolAutoConfig);
         return(HIM_SUCCESS);

      case HIM_IOB_BUSY:
         /* must free the current unit handle */
         SCSI_FREE_UNIT_HANDLE(*((SCSI_UNIT_HANDLE HIM_PTR)&unitHandle));
      
         SCSIxQueueSpecialIOB(adapterTSH, HIM_UNFREEZE_QUEUE);
         
         /* if not HIM_PROBE, will do retry once */
         if (adapterTSH->retryBusScan <= 30000)
         { 
            /* must free the current unit handle */
            SCSI_FREE_UNIT_HANDLE(*((SCSI_UNIT_HANDLE HIM_PTR)&unitHandle));
            ++adapterTSH->retryBusScan;
            SCSIxQueueTUR(adapterTSH);
            return(HIM_SUCCESS);
         }   
         break;

      default:
         /* treat device exist */
         /* must free the current unit handle */
         SCSI_FREE_UNIT_HANDLE(*((SCSI_UNIT_HANDLE HIM_PTR)&unitHandle));
         SCSIxQueueSSU(adapterTSH);
         return(HIM_SUCCESS);
   }
}
#endif /* SCSI_NSX_REPEATER + SCSI_PAC_NSX_REPEATER + SCSI_PAC_SEND_SSU */

/*********************************************************************
*
*  SCSIxQueueBusScan
*
*     Queue request for bus scan operation
*
*  Return Value:  none
*                  
*  Parameters:    adapterTSH
*
*  Remarks:
*
*********************************************************************/
void SCSIxQueueBusScan (SCSI_ADAPTER_TSCB HIM_PTR adapterTSH)
{
   SCSI_TARGET_TSCB HIM_PTR targetTSCB = &adapterTSH->targetTSCBBusScan;
   SCSI_HHCB HIM_PTR hhcb = &adapterTSH->hhcb;
   HIM_IOB HIM_PTR iob;
   HIM_BUS_ADDRESS busAddress;
   HIM_UEXACT32 uexact32Value;
   HIM_UEXACT8 HIM_PTR uexact8Pointer;
   SCSI_UINT8 probeFlag = (adapterTSH->iobProtocolAutoConfig->function == HIM_PROBE);
#if SCSI_PAC_NSX_REPEATER
   HIM_BOOLEAN enableNsxCommunication = HIM_TRUE;
#else
#if SCSI_NSX_REPEATER
   HIM_BOOLEAN enableNsxCommunication = HIM_FALSE;
#endif /* SCSI_NSX_REPEATER */
#endif /* SCSI_PAC_NSX_REPEATER */
   HIM_BOOLEAN suppressNego = HIM_FALSE;
   HIM_TS_SCSI HIM_PTR transportSpecific;
#if SCSI_PAC_NSX_REPEATER || SCSI_NSX_REPEATER
   HIM_BOOLEAN useOsmNsxBuffer = HIM_FALSE;
#endif /* SCSI_PAC_NSX_REPEATER || SCSI_NSX_REPEATER */

#if !SCSI_FAST_PAC
   transportSpecific =
      (HIM_TS_SCSI HIM_PTR)adapterTSH->iobProtocolAutoConfig->transportSpecific;
   
   if (transportSpecific)
   {
      /*
         Record the TargetID & Lun of current device being scanned.
         OSM can use this information to identify the device that failed during
         scsi bus scan which caused PAC completed with an error.
         This is not useful for SCSI_FAST_PAC implemetation.
      */
      transportSpecific->lastScannedScsiID = targetTSCB->scsiID;
      transportSpecific->lastScannedLun    = targetTSCB->lunID;
   }
#endif   

   /* setup next iob request (inquiry) for bus scan */
   /* get iob memory */
   iob = (HIM_IOB HIM_PTR)(((SCSI_UEXACT8 HIM_PTR)adapterTSH->moreUnlocked) + SCSI_MORE_IOB);
   OSDmemset(iob,0,sizeof(HIM_IOB));

   /* setup iob reserved memory */
   iob->iobReserve.virtualAddress = (void HIM_PTR) (((SCSI_UEXACT8 HIM_PTR)
               adapterTSH->moreLocked) + SCSI_MORE_IOBRSV);
   iob->iobReserve.bufferSize = sizeof(SCSI_IOB_RESERVE);
   iob->iobReserve.busAddress = OSMxGetBusAddress(SCSI_ADPTSH(adapterTSH)->osmAdapterContext,
                  HIM_MC_LOCKED,iob->iobReserve.virtualAddress);
   OSDmemset(iob->iobReserve.virtualAddress,0,sizeof(SCSI_IOB_RESERVE));

   /* setup/validate temporary target task set handle */
   SCSI_SET_UNIT_HANDLE(&SCSI_ADPTSH(adapterTSH)->hhcb,
         &targetTSCB->unitControl,(SCSI_UEXACT8)targetTSCB->scsiID,
         (SCSI_UEXACT8)targetTSCB->lunID);

   /* setup iob for inquiry */
   iob->function = HIM_INITIATE_TASK;
   iob->taskSetHandle = (HIM_TASK_SET_HANDLE) targetTSCB;
   iob->flagsIob.autoSense = 1;
   iob->postRoutine = (HIM_POST_PTR) SCSIxPostBusScan;
   iob->targetCommand = (void HIM_PTR) (((SCSI_UEXACT8 HIM_PTR) adapterTSH->moreUnlocked) + SCSI_MORE_INQCDB);
   iob->targetCommandLength = 6;
#if SCSI_NEGOTIATION_PER_IOB
   if (probeFlag)
   {
      iob->transportSpecific =
         adapterTSH->iobProtocolAutoConfig->transportSpecific;
   }
#endif /* SCSI_NEGOTIATION_PER_IOB */

   /* setup sense data */
   iob->errorData = (void HIM_PTR) (((SCSI_UEXACT8 HIM_PTR) adapterTSH->moreLocked) + SCSI_MORE_SNSDATA);
   iob->errorDataLength = 32;
   iob->taskAttribute = HIM_TASK_SIMPLE;

   if (targetTSCB->lunID == 0)
   { 
      targetTSCB->targetAttributes.tagEnable = 0;
#if SCSI_NSX_REPEATER
      if (adapterTSH->iobProtocolAutoConfig->transportSpecific && 
          probeFlag)
      {

         transportSpecific =
            (HIM_TS_SCSI HIM_PTR)adapterTSH->iobProtocolAutoConfig->transportSpecific;
         if (enableNsxCommunication == HIM_FALSE)
         {
            /* we don't have SCSI_PAC_NSX_REPEATER enabled */
            /* obtain from transport specific */
            enableNsxCommunication = transportSpecific->nsxCommunication;
            if (enableNsxCommunication == HIM_TRUE)
            {
               /* use OSM supplied transportSpecific */
               useOsmNsxBuffer = HIM_TRUE;
               iob->transportSpecific =
                  adapterTSH->iobProtocolAutoConfig->transportSpecific;
            }
         }
         else
         {
            /* SCSI_PAC_NSX_REPEATER is enabled */
            useOsmNsxBuffer = transportSpecific->nsxCommunication;
            if (useOsmNsxBuffer == HIM_TRUE)
            {
                /* use OSM supplied transportSpecific */
               iob->transportSpecific =
                  adapterTSH->iobProtocolAutoConfig->transportSpecific;
            }
         }           
      }
#if SCSI_PAC_NSX_REPEATER
      else
      if (adapterTSH->iobProtocolAutoConfig->transportSpecific && 
          !probeFlag)
      {
         /* must be PAC */
         /* obtain determination of HIM required NSX from transport specific */
         transportSpecific =
            (HIM_TS_SCSI HIM_PTR)adapterTSH->iobProtocolAutoConfig->transportSpecific;
         if (transportSpecific->nsxCommunication == HIM_TRUE)
         {
            /* disable NSX communication to be interpreted by CHIM for PAC */ 
            enableNsxCommunication = HIM_FALSE;
         }
      } 
#endif /* SCSI_PAC_NSX_REPEATER */  
#endif /* SCSI_NSX_REPEATER */
 
#if SCSI_PAC_NSX_REPEATER || SCSI_NSX_REPEATER
      if ((enableNsxCommunication == HIM_TRUE) && (useOsmNsxBuffer == HIM_FALSE))
      {
         /* set up transport specific buffer for nsx information */
         iob->transportSpecific =
           (void HIM_PTR) (((SCSI_UEXACT8 HIM_PTR) adapterTSH->moreUnlocked) +
                           SCSI_MORE_TRANSPORT_SPECIFIC);
         /* zero area */ 
         OSDmemset(iob->transportSpecific,0,sizeof(HIM_TS_SCSI));

         transportSpecific = (HIM_TS_SCSI HIM_PTR)iob->transportSpecific;
         transportSpecific->nsxData.virtualAddress = (void HIM_PTR)
            (((SCSI_UEXACT8 HIM_PTR) adapterTSH->moreUnlocked) + SCSI_MORE_NSX_DATA);

         transportSpecific->nsxCommunication = HIM_TRUE;

         /* zero buffer */
         OSDmemset(transportSpecific->nsxData.virtualAddress,0,MAX_NSX_REPEATER_DATA);
         transportSpecific->nsxData.busAddress = HIM_NULL;
         transportSpecific->nsxData.bufferSize = (HIM_UEXACT32)MAX_NSX_REPEATER_DATA;
      }
#endif /* SCSI_PAC_NSX_REPEATER || SCSI_NSX_REPEATER */
   }
 
   /* setup cdb */
   uexact8Pointer = (SCSI_UEXACT8 HIM_PTR) iob->targetCommand;
   uexact8Pointer[0] = (SCSI_UEXACT8) 0x12;
   uexact8Pointer[1] = (SCSI_UEXACT8) 0;
   uexact8Pointer[2] = (SCSI_UEXACT8) 0;
   uexact8Pointer[3] = (SCSI_UEXACT8) 0;
   uexact8Pointer[5] = (SCSI_UEXACT8) 0;

   /* This is to avoid some SCSI 2 devices that didn't setup I_T_L nexus properly */
   /* through identify message. So LUN needs to be specified in inquiry cdb.      */ 
   if ((targetTSCB->lunID > 0) && (targetTSCB->lunID < 8) &&
       ((adapterTSH->scsi1OrScsi2Device & 
        ((SCSI_UEXACT16)(1 << targetTSCB->scsiID))) != 0))
   {
      uexact8Pointer[1] |= (SCSI_UEXACT8)(targetTSCB->lunID << 5);
   }

#if (SCSI_AICBAYONET || SCSI_AICTRIDENT)
   /* expander checking only enable for LUN 0 */
   if ((hhcb->SCSI_HF_expSupport) && (targetTSCB->lunID == 0))
   {
      SCSI_ENABLE_EXP_STATUS(hhcb);
   }
#endif /* (SCSI_AICBAYONET || SCSI_AICTRIDENT) */

   /* if HIM_PROBE, need to setup the OSM request data size if it is greater than default */ 
   if((adapterTSH->iobProtocolAutoConfig->data.bufferSize > 
       SCSI_SIZE_INQUIRY) && probeFlag)
   {
      uexact8Pointer[4] = (SCSI_UEXACT8) adapterTSH->iobProtocolAutoConfig->data.bufferSize;
      busAddress = adapterTSH->iobProtocolAutoConfig->data.busAddress;
      uexact32Value = (SCSI_UEXACT32) (0x80000000 + adapterTSH->iobProtocolAutoConfig->data.bufferSize);
   }
   else
   {
      uexact8Pointer[4] = (SCSI_UEXACT8) SCSI_SIZE_INQUIRY;

      busAddress = OSMxGetBusAddress(SCSI_ADPTSH(adapterTSH)->osmAdapterContext,
                     HIM_MC_LOCKED,(void HIM_PTR)
                     (((SCSI_UEXACT8 HIM_PTR)adapterTSH->moreLocked) +
                     SCSI_MORE_INQDATA));

      uexact32Value = (SCSI_UEXACT32)(0x80000000 + SCSI_SIZE_INQUIRY);
   }

   /* setup sg list */
   iob->data.virtualAddress = (void HIM_PTR)
      (((SCSI_UEXACT8 HIM_PTR) adapterTSH->moreLocked) + SCSI_MORE_INQSG);
   iob->data.bufferSize = 2 * sizeof(HIM_BUS_ADDRESS);
   iob->data.busAddress = OSMxGetBusAddress(SCSI_ADPTSH(adapterTSH)->osmAdapterContext,
                              HIM_MC_LOCKED,iob->data.virtualAddress);

   /* setup sg element */
   SCSI_hPUTBUSADDRESS(hhcb,iob->data.virtualAddress,0,busAddress);
   HIM_PUT_LITTLE_ENDIAN32(adapterTSH->osmAdapterContext,iob->data.virtualAddress,
                           sizeof(HIM_BUS_ADDRESS),uexact32Value);

   HIM_FLUSH_CACHE(iob->data.virtualAddress,2*OSM_BUS_ADDRESS_SIZE);

   if (probeFlag)
   {
      /* must be HIM_PROBE */
      /* obtain suppressNegotiation from transport specific */
      transportSpecific =
         (HIM_TS_SCSI HIM_PTR)adapterTSH->iobProtocolAutoConfig->transportSpecific;
      suppressNego = transportSpecific->suppressNego;
   }

#if SCSI_PAC_NSX_REPEATER
   /* If it is a HIM_PROBE or first LUN scanned in a PAC then fall into the */
   /* if statement */
   if (((targetTSCB->lunID == 0) && (!probeFlag) &&
          ((hhcb->SCSI_HF_resetSCSI) ||
            ((hhcb->SCSI_HF_scamLevel == 1) && (!hhcb->SCSI_HF_skipScam)))) 
       ||
        (probeFlag &&
          ((suppressNego == HIM_TRUE) ||
            SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].SCSI_DF_resetSCSI)))
   {
      /* Clear wasSync. and wasWide xfer flags because SCSI bus has */
      /* been or will be reset.  These flags will be updated during */
      /* negotation protocol with the target. */
      SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].SCSI_DF_wasSyncXfer = 0;
      SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].SCSI_DF_wasWideXfer = 0;

      /* suppress negotiation only after SCSI reset or suppressNego flag */
      /* in HIM_PROBE set */
      SCSI_SUPPRESS_NEGO_ACTIVE_TARGET(&targetTSCB->unitControl);
   }

#else /* !SCSI_PAC_NSX_REPEATER */

   /* If it is a HIM_PROBE or first LUN scanned in a PAC then fall into the */
   /* if statement */
   if (((targetTSCB->lunID == 0) && !probeFlag) || probeFlag)
   {
      if ((!probeFlag && 
            ((hhcb->SCSI_HF_resetSCSI) ||
               ((hhcb->SCSI_HF_scamLevel == 1) && 
               (!hhcb->SCSI_HF_skipScam)))) 
          ||
          (probeFlag && 
            ((suppressNego == HIM_TRUE) ||
            SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].SCSI_DF_resetSCSI)))
      {
         /* Clear wasSync. and wasWide xfer flags because SCSI bus has */
         /* been or will be reset.  These flags will be updated during */
         /* negotation protocol with the target. */
         SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].SCSI_DF_wasSyncXfer = 0;
         SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].SCSI_DF_wasWideXfer = 0;

         /* suppress negotiation only after SCSI reset */
         SCSI_SUPPRESS_NEGOTIATION(&targetTSCB->unitControl);
      }
      else
      {
         /* force negotiation if there is no bus reset. */
         /* This make an assumption that if a device is replaced, */
         /* the new device should have the same characteristic */
         /* as a replaced one e.g. wide or sync support. */
#if SCSI_DOMAIN_VALIDATION
         if (adapterTSH->domainValidationMethod && (targetTSCB->lunID == 0))
         {
            /* Force async/narrow to exercise Domain Validation */
            SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].bayScsiRate = SCSI_SINGLE_EDGE;
            SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].bayScsiOffset = 0;
            SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].SCSI_DF_setForWide = 0;
            SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].SCSI_DF_setForSync = 0;
         }
#endif

#if (!SCSI_NEGOTIATION_PER_IOB) + SCSI_DOMAIN_VALIDATION
         SCSI_FORCE_NEGOTIATION(&targetTSCB->unitControl);
#endif /* !SCSI_NEGOTIATION_PER_IOB */
      }
   }
#endif /* SCSI_PAC_NSX_REPEATER */  

   /* execute it */
   SCSIQueueIOB(iob);
}

/*********************************************************************
*
*  SCSIxPostBusScan
*
*     Verify bus scan operation
*
*  Return Value:  status
*                  
*  Parameters:    adapterTSH
*
*  Remarks:
*
*********************************************************************/
HIM_UINT32 SCSIxPostBusScan (HIM_IOB HIM_PTR iob)
{
   SCSI_TARGET_TSCB HIM_PTR targetTSCB =
                              (SCSI_TARGET_TSCB HIM_PTR) iob->taskSetHandle;
   SCSI_UNIT_CONTROL HIM_PTR unitHandle = &targetTSCB->unitControl;
   SCSI_ADAPTER_TSCB HIM_PTR adapterTSH = targetTSCB->adapterTSH;
   SCSI_HHCB HIM_PTR hhcb = &adapterTSH->hhcb;
   SCSI_UEXACT8 uexact8Value;
   SCSI_UEXACT8 maxTo8Lun = 0;
   SCSI_UEXACT8 HIM_PTR lunExist = SCSI_ADPTSH(adapterTSH)->lunExist;
   SCSI_UEXACT8 HIM_PTR tshExist = SCSI_ADPTSH(adapterTSH)->tshExist;
#if !SCSI_DISABLE_PROBE_SUPPORT
   HIM_UINT8 i;
   HIM_UINT8 j;
#endif /* !SCSI_DISABLE_PROBE_SUPPORT */
   int residual = 0;
   void HIM_PTR inquiryData;
   SCSI_UEXACT32 inquiryDataSize;
   SCSI_UEXACT8 peripheralDeviceType;
   SCSI_UINT8 probeFlag = (adapterTSH->iobProtocolAutoConfig->function == HIM_PROBE);
   HIM_TS_SCSI HIM_PTR transportSpecific;
#if SCSI_DOMAIN_VALIDATION
   SCSI_UEXACT8 doDV = 0;
#endif /* SCSI_DOMAIN_VALIDATION */

#if (SCSI_AICBAYONET || SCSI_AICTRIDENT)
   /* Disable expander checking if it is LUN 0. It will be       */
   /* re-enabled when scanning the next ID LUN 0 in QueueBusScan */
   if ((hhcb->SCSI_HF_expSupport) && (targetTSCB->lunID == 0))
   {
      SCSI_DISABLE_EXP_STATUS(hhcb);
   }
#endif /* (SCSI_AICBAYONET || SCSI_AICTRIDENT) */

   /* check iob status to verify if device exist */
   switch(iob->taskStatus)
   {
      case HIM_IOB_NO_RESPONSE:
         /* indicate device does not exist by setting the lunID to max. */
         /* LUN number so that CHIM continues scanning next scsi ID. */
         if(targetTSCB->lunID==0)
         {
            targetTSCB->lunID = (SCSI_UINT8)adapterTSH->NumberLuns[targetTSCB->scsiID];
         }  
         break;

      case HIM_IOB_DATA_OVERUNDERRUN:
         residual = (int)iob->residual;
            /* Fall through */

      case HIM_IOB_GOOD:

         if((adapterTSH->iobProtocolAutoConfig->data.bufferSize > 
             SCSI_SIZE_INQUIRY) && probeFlag)
         {
            inquiryData = adapterTSH->iobProtocolAutoConfig->data.virtualAddress;
            inquiryDataSize = adapterTSH->iobProtocolAutoConfig->data.bufferSize;
         }
         else
         {
            inquiryData = ((SCSI_UEXACT8 HIM_PTR)adapterTSH->moreLocked) + SCSI_MORE_INQDATA;
            inquiryDataSize = SCSI_SIZE_INQUIRY;
         }
         /* for PAC if LUN is 0 or for HIM_PROBE probed then fall into the */
         /* if statement */
         if (((targetTSCB->lunID == 0) && !probeFlag) || probeFlag)
         {
#if SCSI_DOMAIN_VALIDATION
            if (adapterTSH->domainValidationMethod && (targetTSCB->lunID == 0))
            {
#if SCSI_DOMAIN_VALIDATION_ENHANCED
               /* If Inquiry Data was less than 56 bytes, */
               if ((SCSI_SIZE_INQUIRY - residual) < 56)
               {
                  /* then target does not support double-transition clocking. */
                  adapterTSH->SCSI_AF_dvLevel(targetTSCB->scsiID) = SCSI_DV_BASIC;
               }
               else  /* Inquiry Data return was greater 56 bytes */
               {
                  /* check CLOCKING field to determine level of Domain Validation */
                  HIM_GET_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,&uexact8Value,
                     inquiryData, 56);

                  switch ((uexact8Value >> 2) & 0x03)
                  {
                     case 0:  /* Support Single-Transition Clocking only */
                     case 2:  /* Reserved - we treat it like STC mode */
                        adapterTSH->SCSI_AF_dvLevel(targetTSCB->scsiID) = SCSI_DV_BASIC;
                        break;

                     case 1:  /* Support Dual-Transition Clocking only */
                     case 3:  /* Support BOTH DT & ST Clocking */
                        adapterTSH->SCSI_AF_dvLevel(targetTSCB->scsiID) = SCSI_DV_ENHANCED;
                        break;
                  }
               }
#else /* SCSI_DOMAIN_VALIDATION_ENHANCED */
               adapterTSH->SCSI_AF_dvLevel(targetTSCB->scsiID) = SCSI_DV_BASIC;
#endif /* SCSI_DOMAIN_VALIDATION_ENHANCED */

               /* save the inquiry data for later reference */
               OSDmemcpy(((SCSI_UEXACT8 HIM_PTR )adapterTSH->moreUnlocked) +
                  SCSI_MORE_REFDATA + targetTSCB->scsiID * SCSI_SIZE_INQUIRY,
                  ((SCSI_UEXACT8 HIM_PTR )adapterTSH->moreLocked) + SCSI_MORE_INQDATA,
                  SCSI_SIZE_INQUIRY - residual);
            }
#endif   /* SCSI_DOMAIN_VALIDATION */

            /* invalidate cache */
            HIM_INVALIDATE_CACHE(inquiryData, inquiryDataSize);

            /* copy inquiry data to iob data buffer, this is to comply with */
            /* CHIM spec. v1.10 errata (2/2/99)                             */
            if((adapterTSH->iobProtocolAutoConfig->data.bufferSize <= 
                SCSI_SIZE_INQUIRY) && probeFlag)
            {
               for(i=0; i < adapterTSH->iobProtocolAutoConfig->data.bufferSize; i++)
               {
                  HIM_GET_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,
                      ((SCSI_UEXACT8 *)(adapterTSH->iobProtocolAutoConfig->data.virtualAddress) +i ),
                      (((SCSI_UEXACT8 HIM_PTR)adapterTSH->moreLocked) + SCSI_MORE_INQDATA),i);

               }
               adapterTSH->iobProtocolAutoConfig->data.bufferSize -= i;
            }

            /* Is the Logical Unit supported? */
            HIM_GET_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,&peripheralDeviceType,
                inquiryData, 0);

            if (probeFlag && (peripheralDeviceType == 0x7F))
            {
               targetTSCB->lunID = (SCSI_UINT8)adapterTSH->NumberLuns[targetTSCB->scsiID];
            }
            else
            {
               /* set device exist */
               SCSIxSetLunExist(adapterTSH->lunExist,targetTSCB->scsiID,targetTSCB->lunID);

               if (!probeFlag)
               {
                  /* Clear resetSCSI flag during the regular PAC so that when */
                  /* the subsequent HIM_PROBE invoked for the same id,        */
                  /* suppress negotiation will not get called. */
                  SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].SCSI_DF_resetSCSI = 0;
               }
            }

            /* adjust transfer option based on inquiry data */
            HIM_GET_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,&uexact8Value,
                  inquiryData, 7);

            /* Update scsiOption only if the target is not a wide device */
            /* and previously it was not running in wide mode. */
            if ((!(uexact8Value & 0x20)) && 
                (!SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].SCSI_DF_wasWideXfer))
            {
               /* non wide */
               SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].scsiOption &= ~SCSI_WIDEXFER;
               SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].SCSI_DF_wideDevice = 0;
            }

            /* Update scsiOption only if the target is not a sync. device */
            /* and previously it was not running in sync. mode. */
            /* Compatibility issue, a target reports as async. device in */
            /* inquiry data but it initiates sync. nego. to HA. */
            if ((!(uexact8Value & 0x10)) &&
                (!SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].SCSI_DF_wasSyncXfer))
            {
               /* non sync */
               SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].scsiOption &= ~SCSI_SOFS;
               SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].SCSI_DF_syncDevice = 0;
            }

#if SCSI_DOMAIN_VALIDATION
            if (adapterTSH->domainValidationMethod && (targetTSCB->lunID == 0))
            {
               if (!SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].SCSI_DF_syncDevice && !SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].SCSI_DF_wideDevice)
               {
                  adapterTSH->SCSI_AF_dvLevel(targetTSCB->scsiID) = SCSI_DV_DISABLE;
               }
            }
#endif /* SCSI_DOMAIN_VALIDATION

            /* adjust scontrol based on tag enable */
            if (uexact8Value & 0x02)
            {
               adapterTSH->tagEnable |= (1 << targetTSCB->scsiID);
            }

#if SCSI_PPR_ENABLE
            /* If Inquiry Data was less than 56 bytes, */
            if ((SCSI_SIZE_INQUIRY - residual) < 56)
            {
               /* then target does not support double-transition clocking. */
               SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].SCSI_DF_dtcSupport = 0;
            }
            else  /* Inquiry Data return was greater 56 bytes */
            {
               /* udpate DTC based on inquiry data (byte 56) */
               HIM_GET_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,&uexact8Value,
                     inquiryData, 56);

               switch ((uexact8Value >> 2) & 0x03)
               {
                  case 2:  /* reserved - we assume STC mode */
                  case 0:
                     /* Support single-transition clocking ONLY */
                     SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].SCSI_DF_stcSupport = 1;
                     SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].SCSI_DF_dtcSupport = 0;
                     break;

                  case 1:

                     /* Support double-transition clocking ONLY */
                     SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].SCSI_DF_stcSupport = 0;
                     SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].SCSI_DF_dtcSupport = 1;
                     break;
                  
                  case 3:
                     /* Support BOTH double & single transition clocking */
                     SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].SCSI_DF_stcSupport = 1;                     
                     SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].SCSI_DF_dtcSupport = 1;
                     break;
               }
            }
#endif /* SCSI_PPR_ENABLE */

#if SCSI_PAC_NSX_REPEATER && SCSI_NSX_REPEATER 
            if (iob->transportSpecific ==
                ((void HIM_PTR) (((SCSI_UEXACT8 HIM_PTR) adapterTSH->moreUnlocked) +
                                 SCSI_MORE_TRANSPORT_SPECIFIC)))
#endif /* SCSI_PAC_NSX_REPEATER && SCSI_NSX_REPEATER */
#if SCSI_PAC_NSX_REPEATER
            {
               /* Internal NSX request if the transportSpecific buffer is */
               /* PAC/PROBE internal buffer. (i.e. not an OSM NSX request) */               
               if (iob->transportSpecific)
               {
                  transportSpecific = (HIM_TS_SCSI HIM_PTR)iob->transportSpecific;

                  /* for PAC or for HIM_PROBE to LUN 0 then fall into the */
                  /* if statement */
                  if ((transportSpecific->nsxCommunication == HIM_TRUE) &&
                      (!probeFlag || (probeFlag && (targetTSCB->lunID == 0))))
                  {
                     SCSI_OEM1_ADD_SPI_ENTRY(adapterTSH,transportSpecific,targetTSCB->scsiID,residual);
                  }
               }
            }          
#endif /* SCSI_PAC_NSX_REPEATER */
         }
         else
         {
            /* invalidate cache */
            HIM_INVALIDATE_CACHE(inquiryData, inquiryDataSize);

            /* adjust transfer option based on inquiry data */
            HIM_GET_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,&peripheralDeviceType,
                inquiryData, 0);

            /* If the Logical Unit not support, advance to next target */
            if ( (peripheralDeviceType != 0x7F) && (peripheralDeviceType != 0x20) )
          /*  { when trgtTSCB->lunID == max_luns then we don't skip for any more */
                         /*             on this specific target  */
                         /*             i.e. in all cases,  do full lun scan, no short cuts */
          /*     targetTSCB->lunID = (SCSI_UINT8)adapterTSH->NumberLuns[targetTSCB->scsiID]; */
          /*  }    */
          /*  else */
            {
               /* set device exist */
               SCSIxSetLunExist(adapterTSH->lunExist,targetTSCB->scsiID,targetTSCB->lunID);
            }
         }

         /* obtain ANSI-approved version: SCSI-1, SCSI-2 or so on */
         HIM_GET_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,&uexact8Value,
             inquiryData, 2);

         /* adjust max. lun support up to 8 for SCSI 1 or 2 device */
         if ((uexact8Value & 0x07) < 3)
         {
            maxTo8Lun = 1;
         
            if (targetTSCB->lunID == 0)
            {
               /* indicate pre SCSI-3 device */ 
               adapterTSH->scsi1OrScsi2Device |=
                 (SCSI_UEXACT16) (1 << targetTSCB->scsiID);
            }

            /* to turn-off tag queueing for SCSI-1 device */
            if(uexact8Value & 0x01)
            {
               adapterTSH->tagEnable &= ~(1 << targetTSCB->scsiID);
            }
         }
         else
         {
            if (targetTSCB->lunID == 0)
            {
               /* indicate SCSI-3 device */ 
               adapterTSH->scsi1OrScsi2Device &=
                  (SCSI_UEXACT16) (~(1 << targetTSCB->scsiID));
            }
         }                                 

#if SCSI_NSX_REPEATER + SCSI_PAC_NSX_REPEATER + SCSI_PAC_SEND_SSU
         if(hhcb->OEMHardware == SCSI_OEMHARDWARE_OEM1)
         {
            /* Attempt to spin up disk drive type device during PAC */
            if ((targetTSCB->lunID == 0) && !probeFlag)
            {
               HIM_GET_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,
                     &uexact8Value, inquiryData, 0);
               if (((uexact8Value & 0x1F) == 0) &&
                     !(adapterTSH->SCSI_AF_ssuInProgress))
               {
                  adapterTSH->SCSI_AF_ssuInProgress = 1;
                  /* Inquiry was successful on a disk device; Handle TUR/SSU */
                  SCSI_FREE_UNIT_HANDLE(*((SCSI_UNIT_HANDLE HIM_PTR)&unitHandle));
                  SCSIxQueueTUR(adapterTSH);
                  return(HIM_SUCCESS);
               }
            }
         }
#endif /* SCSI_NSX_REPEATER + SCSI_PAC_NSX_REPEATER + SCSI_PAC_SEND_SSU */

         /* Support certain boards that do not allow the hard disk drive */
         /* to be disconnected on any SCSI command. */
         if (hhcb->SCSI_HP_disconnectOffForHDD)
         {
            /* Obtain Peripheral device qualifier and type */
            HIM_GET_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,&uexact8Value,
                inquiryData, 0);

            /* If direct-access device type (ie. hard disk) disable disconnect */
            if ((uexact8Value & 0x1F) == 0)
            {
               /* Obtain the second byte to get RMB bit */
               HIM_GET_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,
                   &uexact8Value, inquiryData, 1);

               if (!(uexact8Value & 0x80))
               {
                  SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].SCSI_DF_disconnectEnable = 0;
               }
            }
         }

         break;

      case HIM_IOB_ABORTED_REQ_BUS_RESET:
         /* abort protocol auto config because of a catastrophic event */
         /* post protocol auto config back to osm */
         adapterTSH->iobProtocolAutoConfig->taskStatus = HIM_IOB_ABORTED_REQ_BUS_RESET;
         adapterTSH->iobProtocolAutoConfig->postRoutine(adapterTSH->iobProtocolAutoConfig);
#if SCSI_NSX_REPEATER + SCSI_PAC_NSX_REPEATER + SCSI_PAC_SEND_SSU
         adapterTSH->SCSI_AF_ssuInProgress = 0;
#endif /* SCSI_NSX_REPEATER + SCSI_PAC_NSX_REPEATER + SCSI_PAC_SEND_SSU */
         return(HIM_SUCCESS);

      case HIM_IOB_ABORTED_CHIM_RESET:

         /* abort protocol auto config because of a catastrophic event */
         /* post protocol auto config back to osm */
         adapterTSH->iobProtocolAutoConfig->taskStatus = HIM_IOB_ABORTED_CHIM_RESET;
         adapterTSH->iobProtocolAutoConfig->postRoutine(adapterTSH->iobProtocolAutoConfig);
#if SCSI_NSX_REPEATER + SCSI_PAC_NSX_REPEATER + SCSI_PAC_SEND_SSU
         adapterTSH->SCSI_AF_ssuInProgress = 0;
#endif /* SCSI_NSX_REPEATER + SCSI_PAC_NSX_REPEATER + SCSI_PAC_SEND_SSU */
         return(HIM_SUCCESS);
         
      case HIM_IOB_ABORTED_TRANSPORT_MODE_CHANGE:

         /* abort protocol auto config because of a catastrophic event */
         /* post protocol auto config back to osm */
         adapterTSH->iobProtocolAutoConfig->taskStatus = HIM_IOB_ABORTED_TRANSPORT_MODE_CHANGE;
         adapterTSH->iobProtocolAutoConfig->postRoutine(adapterTSH->iobProtocolAutoConfig);
#if SCSI_NSX_REPEATER + SCSI_PAC_NSX_REPEATER + SCSI_PAC_SEND_SSU
         adapterTSH->SCSI_AF_ssuInProgress = 0;
#endif /* SCSI_NSX_REPEATER + SCSI_PAC_NSX_REPEATER + SCSI_PAC_SEND_SSU */
         return(HIM_SUCCESS);

      case HIM_IOB_ABORTED_3RD_PARTY_RESET:
         /* abort protocol auto config because of a catastrophic event */
         /* post protocol auto config back to osm */
         adapterTSH->iobProtocolAutoConfig->taskStatus = HIM_IOB_ABORTED_3RD_PARTY_RESET;
         adapterTSH->iobProtocolAutoConfig->postRoutine(adapterTSH->iobProtocolAutoConfig);
#if SCSI_NSX_REPEATER + SCSI_PAC_NSX_REPEATER + SCSI_PAC_SEND_SSU
         adapterTSH->SCSI_AF_ssuInProgress = 0;
#endif /* SCSI_NSX_REPEATER + SCSI_PAC_NSX_REPEATER + SCSI_PAC_SEND_SSU */
         return(HIM_SUCCESS);

      case HIM_IOB_HOST_ADAPTER_FAILURE:
         /* abort protocol auto config because of a catastrophic event */
         /* post protocol auto config back to osm */
         adapterTSH->iobProtocolAutoConfig->taskStatus = HIM_IOB_HOST_ADAPTER_FAILURE;
         adapterTSH->iobProtocolAutoConfig->postRoutine(adapterTSH->iobProtocolAutoConfig);
         return(HIM_SUCCESS);

      case HIM_IOB_BUSY:
         /* must free the current unit handle */
         SCSI_FREE_UNIT_HANDLE(*((SCSI_UNIT_HANDLE HIM_PTR)&unitHandle));
      
         SCSIxQueueSpecialIOB(adapterTSH, HIM_UNFREEZE_QUEUE);
         
         /* if not HIM_PROBE, will do retry once */
         if ( (adapterTSH->iobProtocolAutoConfig->function != HIM_PROBE) && 
              (adapterTSH->retryBusScan <= 30000))
         { 
            /* must free the current unit handle */
            SCSI_FREE_UNIT_HANDLE(*((SCSI_UNIT_HANDLE HIM_PTR)&unitHandle));
            ++adapterTSH->retryBusScan;
            SCSIxQueueBusScan(adapterTSH);
#if SCSI_NSX_REPEATER + SCSI_PAC_NSX_REPEATER + SCSI_PAC_SEND_SSU
            adapterTSH->SCSI_AF_ssuInProgress = 0;
#endif /* SCSI_NSX_REPEATER + SCSI_PAC_NSX_REPEATER + SCSI_PAC_SEND_SSU */
            return(HIM_SUCCESS);
         }   
         break;
        

      default:
         /* treat device exist */
         if (!adapterTSH->retryBusScan)
         {
            /* must free the current unit handle */
            SCSI_FREE_UNIT_HANDLE(*((SCSI_UNIT_HANDLE HIM_PTR)&unitHandle));

            ++adapterTSH->retryBusScan;
            SCSIxQueueBusScan(adapterTSH);
#if SCSI_NSX_REPEATER + SCSI_PAC_NSX_REPEATER + SCSI_PAC_SEND_SSU
            adapterTSH->SCSI_AF_ssuInProgress = 0;
#endif /* SCSI_NSX_REPEATER + SCSI_PAC_NSX_REPEATER + SCSI_PAC_SEND_SSU */
            return(HIM_SUCCESS);
         }
         break;
   }

   /* free the current unit handle */
   SCSI_FREE_UNIT_HANDLE(*((SCSI_UNIT_HANDLE HIM_PTR)&unitHandle));

#if SCSI_DOMAIN_VALIDATION
   /* We should do domain validation only for LUN 0 */
   if (targetTSCB->lunID == 0)
   {
      if (probeFlag)
      {
         transportSpecific =
            (HIM_TS_SCSI HIM_PTR)adapterTSH->iobProtocolAutoConfig->transportSpecific;
         if (transportSpecific->dvIOB == HIM_TRUE)
         {
            doDV = 1;
         }
      }
      else
      {
         doDV = 1;
      }
   }
#endif /* SCSI_DOMAIN_VALIDATION */

   /* work on next lun */
   ++targetTSCB->lunID;

   if (!probeFlag)
   {
      if ((!(adapterTSH->multipleLun & (1 << targetTSCB->scsiID))) ||
          (maxTo8Lun && (targetTSCB->lunID > 7)) ||
          (targetTSCB->lunID >= (SCSI_UINT8)adapterTSH->NumberLuns[targetTSCB->scsiID]))
      {
#if SCSI_DOMAIN_VALIDATION
         if (adapterTSH->domainValidationMethod)
         {
            /* restore deafult xfer speed and width before exercising */
            /* domain validation */
            SCSIxRestoreDefaultXfer(adapterTSH, targetTSCB);
         }
         else
#endif

#if (!SCSI_PAC_NSX_REPEATER)
#if (!SCSI_NEGOTIATION_PER_IOB)+SCSI_DOMAIN_VALIDATION
         /* force negotiation */
         if (hhcb->SCSI_HF_resetSCSI ||
             ((hhcb->SCSI_HF_scamLevel == 1) && (!hhcb->SCSI_HF_skipScam)))
         {
            /* restores negotiation if it's needed for the current target */
            SCSI_FORCE_NEGOTIATION(&targetTSCB->unitControl);
         }
#endif /* !SCSI_NEGOTIATION_PER_IOB */
#endif /* !SCSI_PAC_NSX_REPEATER */

         /* next lun not available and need to work on next target instead */
         for (;++targetTSCB->scsiID < adapterTSH->hhcb.maxDevices;)
         {
            if (adapterTSH->NumberLuns[targetTSCB->scsiID] == 0)
            {
               /* If NumberLuns specified for the target is 0, skip this target */            
               continue;
            }   
            else
            {
               targetTSCB->lunID = 0;
               if (targetTSCB->scsiID == adapterTSH->hhcb.hostScsiID)
               {
#if !SCSI_NULL_SELECTION
                  /* skip the host scsi id */
                  continue;
#else
                  /* Don't skip host scsi id - attempt Null Selection */
                  /* Can't disconnect on Null Selection */ 
                  SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].SCSI_DF_disconnectEnable = 0;
                  break;
#endif /* !SCSI_NULL_SELECTION */
               }
               break;
            }
         }
      }

      if (targetTSCB->scsiID >= adapterTSH->hhcb.maxDevices)
      {
         /* Clear resetSCSI flag after the bus scan is done so that */
         /* the subsequence PROTO_AUTO_CONFIG invokes, the suppress */
         /* negotiation will not get call. */
         hhcb->SCSI_HF_resetSCSI = 0;

         /* post protocol auto config back to osm */

         adapterTSH->iobProtocolAutoConfig->taskStatus = HIM_IOB_GOOD;

#if SCSI_DOMAIN_VALIDATION
         if (adapterTSH->domainValidationMethod)
         {
            SCSIxSetupDomainValidation(adapterTSH);
            SCSIxQueueDomainValidation(adapterTSH);
         }
         else
#endif
         {
#if SCSI_PAC_NSX_REPEATER
            /* Invoke SPI speed determination at the end of PAC. */
            SCSI_OEM1_SET_SPI_SPEED(adapterTSH);
#endif /* SCSI_PAC_NSX_REPEATER */

            adapterTSH->iobProtocolAutoConfig->postRoutine(adapterTSH->iobProtocolAutoConfig);
         }

         /* If OSM is frozen then unfreeze the OSM because the OSMEvent handling */
         /* is complete */
         if(adapterTSH->SCSI_AF_osmFrozen)
         {
            OSDAsyncEvent(hhcb, SCSI_AE_OSMUNFREEZE);         
         }
      }
      else
      {
         /* continue with bus scan process */
         adapterTSH->retryBusScan = 0;
         SCSIxQueueBusScan(adapterTSH);
      }
   }
#if !SCSI_DISABLE_PROBE_SUPPORT
   else
   {
#if SCSI_DOMAIN_VALIDATION
      if (adapterTSH->domainValidationMethod && doDV)
      {
         /* restore deafult xfer speed and width before exercising */
         /* domain validation */
         SCSIxRestoreDefaultXfer(adapterTSH, targetTSCB);
      }
      else
#endif
      {
         /* must be HIM_PROBE; obtain dvIOB from transport specific */
         transportSpecific =
            (HIM_TS_SCSI HIM_PTR)adapterTSH->iobProtocolAutoConfig->transportSpecific;
#if (!SCSI_NEGOTIATION_PER_IOB) + SCSI_DOMAIN_VALIDATION
         if (SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].SCSI_DF_resetSCSI && (transportSpecific->suppressNego == HIM_FALSE))
         {
            /* restores negotiation if it's needed for the current target */
            SCSI_FORCE_NEGOTIATION(&targetTSCB->unitControl);
         }
#endif /* !SCSI_NEGOTIATION_PER_IOB */
      }

      /* post protocol auto config back to osm */
      if (iob->taskStatus == HIM_IOB_GOOD || iob->taskStatus == HIM_IOB_DATA_OVERUNDERRUN)
      {
         /* Clear resetSCSI flag after the HIM_PROBE is done so that when */
         /* the subsequent HIM_PROBE invoked for the same id, the suppress */
         /* negotiation will not get called. */
         SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].SCSI_DF_resetSCSI = 0;

         i = SCSI_ADPTSH(adapterTSH)->lastScsiIDProbed;
         j = SCSI_ADPTSH(adapterTSH)->lastScsiLUNProbed;
         if (SCSIxChkLunExist(lunExist,i,j) != SCSIxChkLunExist(tshExist,i,j))
         {
            /* if there is any residual, taskStatus= HIM_IOB_DATA_OVERUNDERRUN */
            if(adapterTSH->iobProtocolAutoConfig->data.bufferSize)
            {
               adapterTSH->iobProtocolAutoConfig->taskStatus = HIM_IOB_DATA_OVERUNDERRUN;
            }
            else
            {
               adapterTSH->iobProtocolAutoConfig->taskStatus = HIM_IOB_GOOD;
            }
         }
         else
         {
            if (peripheralDeviceType == 0x7F)
            {
               adapterTSH->iobProtocolAutoConfig->taskStatus = HIM_IOB_INVALID_LUN;
            }
            else
            {
               /* This is still an question whether we might go here??? */
               adapterTSH->iobProtocolAutoConfig->taskStatus = HIM_IOB_NO_RESPONSE;
            }
         }
      }
      else
      {
         adapterTSH->iobProtocolAutoConfig->taskStatus = iob->taskStatus;
      }
#if SCSI_DOMAIN_VALIDATION
      if (adapterTSH->domainValidationMethod && doDV)
      {
         SCSIxSetupDomainValidation(adapterTSH);
         SCSIxQueueDomainValidation(adapterTSH);
      }
      else
#endif
      {
         adapterTSH->iobProtocolAutoConfig->postRoutine(adapterTSH->iobProtocolAutoConfig);
      }
   }
#endif /* !SCSI_DISABLE_PROBE_SUPPORT */

#if SCSI_NSX_REPEATER + SCSI_PAC_NSX_REPEATER + SCSI_PAC_SEND_SSU
   adapterTSH->SCSI_AF_ssuInProgress = 0;
#endif /* SCSI_NSX_REPEATER + SCSI_PAC_NSX_REPEATER + SCSI_PAC_SEND_SSU */

   return(HIM_SUCCESS);
}

/*********************************************************************
*
*  SCSIxRestoreDefaultXfer
*
*     Restore deafult xfer speed and width to prepare for
*     domain validation
*
*  Return Value:  none
*                  
*  Parameters:    adapterTSH
*                 targetTSCB
*
*  Remarks:
*
*********************************************************************/
#if SCSI_DOMAIN_VALIDATION
void SCSIxRestoreDefaultXfer( SCSI_ADAPTER_TSCB HIM_PTR adapterTSH,
                              SCSI_TARGET_TSCB HIM_PTR targetTSCB)
{
   SCSI_HHCB HIM_PTR hhcb = &adapterTSH->hhcb;

   /* restore deafult xfer speed and width before exercising */
   /* domain validation */
   SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].bayDvScsiRate =
            SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].bayDefaultRate;
   SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].bayDvScsiOffset =
            SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].bayDefaultOffset;
}
#endif

/*********************************************************************
*
*  SCSIxDvForceNegotiation
*
*  Force Negotiation with the given xfer parameters for domain validation    
*
*  Return Value:  none
*                  
*  Parameters:    adapterTSH
*                 targetTSCB
*
*  Remarks:
*
*********************************************************************/
#if SCSI_DOMAIN_VALIDATION
void SCSIxDvForceNegotiation( SCSI_ADAPTER_TSCB HIM_PTR adapterTSH,
                              SCSI_TARGET_TSCB HIM_PTR targetTSCB)
{
   SCSI_HHCB HIM_PTR hhcb = &adapterTSH->hhcb;

   SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].bayScsiRate =
            SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].bayDvScsiRate;
   SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].bayScsiOffset =
            SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].bayDvScsiOffset;

   SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].SCSI_DF_setForWide = 
            ((SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].bayScsiRate & SCSI_WIDE_XFER) != 0);
   SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].SCSI_DF_setForSync = 
            (SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].bayScsiOffset != 0);

   if (SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].SCSI_DF_wideDevice)
   {
      SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].scsiOption |= SCSI_WIDE_XFER;
   }
   else
   {
      SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].scsiOption &= ~SCSI_WIDE_XFER;
   }

   if (SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].SCSI_DF_syncDevice)
   {
      SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].scsiOption |= SCSI_SYNC_XFER;
   }
   else
   {
      SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].scsiOption &= ~SCSI_SYNC_XFER;
   }

   /* force renegotiation */
   SCSI_FORCE_NEGOTIATION(&targetTSCB->unitControl);
}
#endif

/*********************************************************************
*
*  SCSIxSetupDomainValidation
*
*     Setup for Domain Validation
*
*  Return Value:  none
*                  
*  Parameters:    adapterTSH
*
*  Remarks:       1. For PAC all target/lun 0 found in bus scan will 
*                    exercise Domain Validation at end of PAC.
*                 2. For probe the specified target will exercise
*                    Domain Validation only if it's lun 0.
*
*********************************************************************/
#if SCSI_DOMAIN_VALIDATION
void SCSIxSetupDomainValidation (SCSI_ADAPTER_TSCB HIM_PTR adapterTSH)
{
   SCSI_TARGET_TSCB HIM_PTR targetTSCB = &adapterTSH->targetTSCBBusScan;
   SCSI_HHCB HIM_PTR hhcb = &adapterTSH->hhcb;
   SCSI_UINT16 i;

   hhcb->SCSI_HF_dvInProgress = 1;

   /* setup ready for Domain Validation */
   for (i = 0; i < 16; ++i)
   {
      if (adapterTSH->SCSI_AF_dvLevel(i) != SCSI_DV_DISABLE)
      {
         /* set dv state to ready */
         adapterTSH->SCSI_AF_dvState(i) = SCSI_DV_READY;
         adapterTSH->SCSI_AF_dvPassed(i) = 0;
         adapterTSH->SCSI_AF_dvFallBack(i) = 0;

         if (SCSI_DEVICE_TABLE(hhcb)[i].bayScsiRate & SCSI_ENABLE_CRC)
         {
            adapterTSH->SCSI_AF_dvThrottle(i) = SCSI_DE_WIDE;
         }
         else
         {
            if (SCSI_DEVICE_TABLE(hhcb)[i].bayScsiRate & SCSI_WIDE_XFER)
            {
               adapterTSH->SCSI_AF_dvThrottle(i) = SCSI_SE_WIDE;
            }
            else
            {
               adapterTSH->SCSI_AF_dvThrottle(i) = SCSI_SE_NARROW_REPEAT;

            }
         }
      }
   }

   /* Reset the retry counter */
   adapterTSH->retryBusScan = 0;

   /* refresh the bus scan state machine (start with target 0, lun 0) */
   OSDmemset(targetTSCB,0,sizeof(SCSI_TARGET_TSCB));

   /* setup targetTSCB */
   targetTSCB->typeTSCB = SCSI_TSCB_TYPE_TARGET;
   targetTSCB->adapterTSH = adapterTSH;

   /* start with scsi id 0 */
   targetTSCB->scsiID = 0;
}
#endif   /* SCSI_DOMAIN_VALIDATION */

/*********************************************************************
*
*  SCSIxQueueDomainValidation
*
*     Queue request for Domain Validation
*
*  Return Value:  none
*
*  Parameters:    adapterTSH
*
*  Remarks:
*
*********************************************************************/
#if SCSI_DOMAIN_VALIDATION
void SCSIxQueueDomainValidation (SCSI_ADAPTER_TSCB HIM_PTR adapterTSH)
{
   SCSI_TARGET_TSCB HIM_PTR targetTSCB = &adapterTSH->targetTSCBBusScan;
   SCSI_HHCB HIM_PTR hhcb = &adapterTSH->hhcb;

   /* Check out where we are in Domain Validation exercise */
   for (; targetTSCB->scsiID < 16; ++targetTSCB->scsiID)
   {
      if (adapterTSH->SCSI_AF_dvLevel(targetTSCB->scsiID) != SCSI_DV_DISABLE &&
         adapterTSH->SCSI_AF_dvState(targetTSCB->scsiID) != SCSI_DV_DONE)
      {
         if (adapterTSH->SCSI_AF_dvLevel(targetTSCB->scsiID) == SCSI_DV_BASIC)
         {
            /* execute level 1 domain validation */
            SCSIxQueueDomainValidationBasic(adapterTSH);
         }

#if SCSI_DOMAIN_VALIDATION_ENHANCED
         if (adapterTSH->SCSI_AF_dvLevel(targetTSCB->scsiID) == SCSI_DV_ENHANCED)
         {
            /* must be level 2 domain validation */
            SCSIxQueueDomainValidationEnhanced(adapterTSH);
         }
#endif   /* SCSI_DOMAIN_VALIDATION_ENHANCED */
         break;
      }
      else if (adapterTSH->SCSI_AF_dvLevel(targetTSCB->scsiID) != SCSI_DV_DISABLE &&
               adapterTSH->SCSI_AF_dvState(targetTSCB->scsiID) == SCSI_DV_DONE)
      {

         /* Clear resetSCSI flag after the Domain Validation is done so that */
         /* when the subsequent HIM_PROBE invoked for the same id, the       */
         /* suppress negotiation will not get called. */
         SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].SCSI_DF_resetSCSI = 0;
      }
   }

   /* all devices has been exercised with Domain Validation, it's done */
   if (targetTSCB->scsiID == 16)
   {
      adapterTSH->iobProtocolAutoConfig->postRoutine(adapterTSH->iobProtocolAutoConfig);
      hhcb->SCSI_HF_dvInProgress = 0;
   }
}
#endif   /* SCSI_DOMAIN_VALIDATION */

/*********************************************************************
*
*  SCSIxQueueDomainValidationBasic
*
*     Queue request for Domain Validation level 1
*
*  Return Value:  none
*
*  Parameters:    adapterTSH
*  Parameters:    targetTSCB
*
*  Remarks:
*
*********************************************************************/
#if SCSI_DOMAIN_VALIDATION
void SCSIxQueueDomainValidationBasic (SCSI_ADAPTER_TSCB HIM_PTR adapterTSH)
{
   SCSI_TARGET_TSCB HIM_PTR targetTSCB = &adapterTSH->targetTSCBBusScan;
   SCSI_HHCB HIM_PTR hhcb = &adapterTSH->hhcb;
   HIM_IOB HIM_PTR iob;
   HIM_BUS_ADDRESS busAddress;
   HIM_UEXACT32 uexact32Value;
   HIM_UEXACT8 HIM_PTR uexact8Pointer;
   HIM_UINT8 i;

   /* setup next iob request (inquiry) for inquiry */
   /* get iob memory */
   iob = (HIM_IOB HIM_PTR)(((SCSI_UEXACT8 HIM_PTR)adapterTSH->moreUnlocked) + SCSI_MORE_IOB);
   OSDmemset(iob,0,sizeof(HIM_IOB));

   /* setup iob reserved memory */
   iob->iobReserve.virtualAddress = (void HIM_PTR) (((SCSI_UEXACT8 HIM_PTR)
               adapterTSH->moreLocked) + SCSI_MORE_IOBRSV);
   iob->iobReserve.bufferSize = sizeof(SCSI_IOB_RESERVE);
   iob->iobReserve.busAddress = OSMxGetBusAddress(SCSI_ADPTSH(adapterTSH)->osmAdapterContext,
                  HIM_MC_LOCKED,iob->iobReserve.virtualAddress);
   OSDmemset(iob->iobReserve.virtualAddress,0,sizeof(SCSI_IOB_RESERVE));

   /* Setup transport specific area for Domain Validation */
   iob->transportSpecific = (void HIM_PTR) (((SCSI_UEXACT8 HIM_PTR) adapterTSH->moreUnlocked) + SCSI_MORE_DV_TRANSPORT_SPECIFIC);
   ((HIM_TS_SCSI HIM_PTR)(iob->transportSpecific))->dvIOB = HIM_TRUE;

   /* setup/validate temporary target task set handle */
   SCSI_SET_UNIT_HANDLE(&SCSI_ADPTSH(adapterTSH)->hhcb,
         &targetTSCB->unitControl,(SCSI_UEXACT8)targetTSCB->scsiID,0);

   /* setup iob for inquiry */
   iob->function = HIM_INITIATE_TASK;
   iob->taskSetHandle = (HIM_TASK_SET_HANDLE) targetTSCB;
   iob->flagsIob.autoSense = 1;
   iob->postRoutine = (HIM_POST_PTR) SCSIxPostDomainValidationBasic;
   iob->targetCommand = (void HIM_PTR) (((SCSI_UEXACT8 HIM_PTR) adapterTSH->moreUnlocked) + SCSI_MORE_INQCDB);
   iob->targetCommandLength = 6;

   /* setup sense data */
   iob->errorData = (void HIM_PTR) (((SCSI_UEXACT8 HIM_PTR) adapterTSH->moreLocked) + SCSI_MORE_SNSDATA);
   iob->errorDataLength = 32;
   iob->taskAttribute = HIM_TASK_SIMPLE;
   targetTSCB->targetAttributes.tagEnable = 0;

   /* setup inquiry cdb */
   uexact8Pointer = (SCSI_UEXACT8 HIM_PTR) iob->targetCommand;
   for (i = 0; i < 6; i++)
   {
      uexact8Pointer[i] = (SCSI_UEXACT8) 0x00;
   }
   uexact8Pointer[0] = (SCSI_UEXACT8) 0x12;
   uexact8Pointer[4] = (SCSI_UEXACT8) SCSI_SIZE_INQUIRY;

   /* clear inquiry data buffer */
   for (i = 0; i < SCSI_SIZE_INQUIRY; i++)
   {
      ((SCSI_UEXACT8 HIM_PTR)adapterTSH->moreLocked)[SCSI_MORE_INQDATA+i] = 0;
   }

   /* setup sg list */
   iob->data.virtualAddress = (void HIM_PTR)
      (((SCSI_UEXACT8 HIM_PTR) adapterTSH->moreLocked) + SCSI_MORE_INQSG);
   iob->data.bufferSize = 2 * sizeof(HIM_BUS_ADDRESS);
   iob->data.busAddress = OSMxGetBusAddress(SCSI_ADPTSH(adapterTSH)->osmAdapterContext,
                              HIM_MC_LOCKED,iob->data.virtualAddress);

   /* setup sg element */
   busAddress = OSMxGetBusAddress(SCSI_ADPTSH(adapterTSH)->osmAdapterContext,
                  HIM_MC_LOCKED,(void HIM_PTR)
                  (((SCSI_UEXACT8 HIM_PTR)adapterTSH->moreLocked) +
                  SCSI_MORE_INQDATA));
   SCSI_hPUTBUSADDRESS(hhcb,iob->data.virtualAddress,0,busAddress);
   uexact32Value = (SCSI_UEXACT32) (0x80000000 | SCSI_SIZE_INQUIRY);
   HIM_PUT_LITTLE_ENDIAN32(adapterTSH->osmAdapterContext,iob->data.virtualAddress,
                           sizeof(HIM_BUS_ADDRESS),uexact32Value);

   HIM_FLUSH_CACHE(iob->data.virtualAddress,2*OSM_BUS_ADDRESS_SIZE);

   /* update state before executing it */
   adapterTSH->SCSI_AF_dvState(targetTSCB->scsiID) = SCSI_DV_INQUIRY;

   /* Force Negotiation */
   SCSIxDvForceNegotiation(adapterTSH, targetTSCB);

   /* execute it */
   SCSIQueueIOB(iob);
}
#endif   /* SCSI_DOMAIN_VALIDATION */

/*********************************************************************
*
*  SCSIxQueueDomainValidationEnhanced
*
*     Queue request for Domain Validation level 2
*
*  Return Value:  none
*
*  Parameters:    adapterTSH
*  Parameters:    targetTSCB
*
*  Remarks:
*
*********************************************************************/
#if SCSI_DOMAIN_VALIDATION_ENHANCED
void SCSIxQueueDomainValidationEnhanced (SCSI_ADAPTER_TSCB HIM_PTR adapterTSH)
{
   SCSI_TARGET_TSCB HIM_PTR targetTSCB = &adapterTSH->targetTSCBBusScan;
   SCSI_HHCB HIM_PTR hhcb = &adapterTSH->hhcb;
   HIM_IOB HIM_PTR iob;
   HIM_BUS_ADDRESS busAddress;
   HIM_UEXACT32 uexact32Value;
   HIM_UEXACT8 HIM_PTR uexact8Pointer;
   HIM_UINT8 i;
   SCSI_UEXACT16 bitPattern = (SCSI_UEXACT16)0x0001;

   /* setup next iob request (inquiry) for bus scan */
   /* get iob memory */
   iob = (HIM_IOB HIM_PTR)(((SCSI_UEXACT8 HIM_PTR)adapterTSH->moreUnlocked) + SCSI_MORE_IOB);
   OSDmemset(iob,0,sizeof(HIM_IOB));

   /* setup iob reserved memory */
   iob->iobReserve.virtualAddress = (void HIM_PTR) (((SCSI_UEXACT8 HIM_PTR)
               adapterTSH->moreLocked) + SCSI_MORE_IOBRSV);
   iob->iobReserve.bufferSize = sizeof(SCSI_IOB_RESERVE);
   iob->iobReserve.busAddress = OSMxGetBusAddress(SCSI_ADPTSH(adapterTSH)->osmAdapterContext,
                  HIM_MC_LOCKED,iob->iobReserve.virtualAddress);
   OSDmemset(iob->iobReserve.virtualAddress,0,sizeof(SCSI_IOB_RESERVE));

   /* Setup transport specific area for Domain Validation */
   iob->transportSpecific = (void HIM_PTR) (((SCSI_UEXACT8 HIM_PTR) adapterTSH->moreUnlocked) + SCSI_MORE_DV_TRANSPORT_SPECIFIC);
   ((HIM_TS_SCSI HIM_PTR)(iob->transportSpecific))->dvIOB = HIM_TRUE;

   /* setup/validate temporary target task set handle */
   SCSI_SET_UNIT_HANDLE(&SCSI_ADPTSH(adapterTSH)->hhcb,
         &targetTSCB->unitControl,(SCSI_UEXACT8)targetTSCB->scsiID,0);

   /* setup iob for inquiry */
   iob->function = HIM_INITIATE_TASK;
   iob->taskSetHandle = (HIM_TASK_SET_HANDLE) targetTSCB;
   iob->flagsIob.autoSense = 1;
   iob->postRoutine = (HIM_POST_PTR) SCSIxPostDomainValidationEnhanced;
   iob->targetCommand = (void HIM_PTR) (((SCSI_UEXACT8 HIM_PTR) adapterTSH->moreUnlocked) + SCSI_MORE_INQCDB);
   iob->targetCommandLength = 10;

   /* setup sense data */
   iob->errorData = (void HIM_PTR) (((SCSI_UEXACT8 HIM_PTR) adapterTSH->moreLocked) + SCSI_MORE_SNSDATA);
   iob->errorDataLength = 32;
   iob->taskAttribute = HIM_TASK_SIMPLE;
   targetTSCB->targetAttributes.tagEnable = 0;

   /* setup cdb */
   uexact8Pointer = (SCSI_UEXACT8 HIM_PTR) iob->targetCommand;
   if (adapterTSH->SCSI_AF_dvState(targetTSCB->scsiID) == SCSI_DV_READY)
   {
      /* setup write buffer cdb */
      uexact8Pointer[0] = (SCSI_UEXACT8) 0x3B;

      /* set data pattern to incremenatl for write buffer */
      for (i = 0; i < 32; i++)
      {
         ((SCSI_UEXACT8 HIM_PTR)adapterTSH->moreLocked)[SCSI_MORE_RWBDATA+i] = (SCSI_UEXACT8) i;
      }
      for (i = 32; i < 48; i++)
      {
         ((SCSI_UEXACT8 HIM_PTR)adapterTSH->moreLocked)[SCSI_MORE_RWBDATA+i] = ((i & 0x02) ? (SCSI_UEXACT8)0xFF : (SCSI_UEXACT8)0x00);
         ((SCSI_UEXACT8 HIM_PTR)adapterTSH->moreLocked)[SCSI_MORE_RWBDATA+16+i] = ((i & 0x02) ? (SCSI_UEXACT8)0xAA : (SCSI_UEXACT8)0x55);
      }
      for (i = 64; i < 128; i+=2)
      {
         ((SCSI_UEXACT8 HIM_PTR)adapterTSH->moreLocked)[SCSI_MORE_RWBDATA+i] = ((i & 0x02) ? (SCSI_UEXACT8)((bitPattern & 0xFF00) >> 8) : (SCSI_UEXACT8)0xFF);
         ((SCSI_UEXACT8 HIM_PTR)adapterTSH->moreLocked)[SCSI_MORE_RWBDATA+i+1] = ((i & 0x02) ? (SCSI_UEXACT8)(bitPattern & 0xFF) : (SCSI_UEXACT8)0xFF);
         if (i & 0x02) bitPattern <<= 1;
      }

      /* update state to write buffer */
      adapterTSH->SCSI_AF_dvState(targetTSCB->scsiID) = SCSI_DV_WRITEBUFFER;
   }
   else
   {
      /* setup read buffer cdb */
      uexact8Pointer[0] = (SCSI_UEXACT8) 0x3C;

      /* clear data buffer for read buffer */
      for (i = 0; i < 128; i++)
      {
         ((SCSI_UEXACT8 HIM_PTR)adapterTSH->moreLocked)[SCSI_MORE_RWBDATA+i] = (SCSI_UEXACT8) ~i;
      }

      /* update state to read buffer */
      adapterTSH->SCSI_AF_dvState(targetTSCB->scsiID) = SCSI_DV_READBUFFER;
   }

   /* @TBD should be logical buffer for Ultra 160m */
   uexact8Pointer[1] = (SCSI_UEXACT8) 0x0A;
   uexact8Pointer[2] = (SCSI_UEXACT8) 0;
   uexact8Pointer[3] = (SCSI_UEXACT8) 0;
   uexact8Pointer[4] = (SCSI_UEXACT8) 0;
   uexact8Pointer[5] = (SCSI_UEXACT8) 0;
   uexact8Pointer[6] = (SCSI_UEXACT8) 0;
   uexact8Pointer[7] = (SCSI_UEXACT8) 0;
   uexact8Pointer[8] = (SCSI_UEXACT8) SCSI_SIZE_RWB;
   uexact8Pointer[9] = (SCSI_UEXACT8) 0;

   /* setup sg list */
   iob->data.virtualAddress = (void HIM_PTR)
      (((SCSI_UEXACT8 HIM_PTR) adapterTSH->moreLocked) + SCSI_MORE_RWBSG);
   iob->data.bufferSize = 2 * sizeof(HIM_BUS_ADDRESS);
   iob->data.busAddress = OSMxGetBusAddress(SCSI_ADPTSH(adapterTSH)->osmAdapterContext,
                              HIM_MC_LOCKED,iob->data.virtualAddress);

   /* setup sg element */
   busAddress = OSMxGetBusAddress(SCSI_ADPTSH(adapterTSH)->osmAdapterContext,
                  HIM_MC_LOCKED,(void HIM_PTR)
                  (((SCSI_UEXACT8 HIM_PTR)adapterTSH->moreLocked) +
                  SCSI_MORE_RWBDATA));
   SCSI_hPUTBUSADDRESS(hhcb,iob->data.virtualAddress,0,busAddress);
   uexact32Value = 0x80000000 | SCSI_SIZE_RWB;
   HIM_PUT_LITTLE_ENDIAN32(adapterTSH->osmAdapterContext,iob->data.virtualAddress,
                           sizeof(HIM_BUS_ADDRESS),uexact32Value);

   HIM_FLUSH_CACHE(iob->data.virtualAddress,2*OSM_BUS_ADDRESS_SIZE);

   /* Force Negotiation */
   SCSIxDvForceNegotiation(adapterTSH, targetTSCB);

   /* execute it */
   SCSIQueueIOB(iob);
}
#endif   /* SCSI_DOMAIN_VALIDATION_ENHANCED */

/*********************************************************************
*
*  SCSIxPostDomainValidationBasic
*
*     Post routine for Domain Validation level 1
*
*  Return Value:  status
*                  
*  Parameters:    adapterTSH
*
*  Remarks:
*
*********************************************************************/
#if SCSI_DOMAIN_VALIDATION
HIM_UINT32 SCSIxPostDomainValidationBasic (HIM_IOB HIM_PTR iob)
{
   SCSI_TARGET_TSCB HIM_PTR targetTSCB =
                              (SCSI_TARGET_TSCB HIM_PTR) iob->taskSetHandle;
   SCSI_UNIT_CONTROL HIM_PTR unitHandle = &targetTSCB->unitControl;
   SCSI_ADAPTER_TSCB HIM_PTR adapterTSH = targetTSCB->adapterTSH;
   SCSI_HHCB HIM_PTR hhcb = &adapterTSH->hhcb;
   HIM_UINT8 i, residual = 0, oddCount = 0, evenCount = 0;
   SCSI_UEXACT8 senseKey;

   if (iob->taskStatus == HIM_IOB_ERRORDATA_VALID)
   {
      HIM_GET_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,&senseKey,iob->errorData,2);
     /* If UNIT_ATTENTION, or NOT_READY condition... */
      if (((senseKey & 0x0F) == 2) || ((senseKey & 0x0F) == 6))
      {
         /* Retry twice and then give up */
         if (adapterTSH->retryBusScan < 2)
         {
            adapterTSH->retryBusScan++;
            adapterTSH->SCSI_AF_dvState(targetTSCB->scsiID) = SCSI_DV_READY;
            SCSIxQueueDomainValidation(adapterTSH);
            return(HIM_SUCCESS);
         }
         else
         {
            adapterTSH->retryBusScan = 0;
            adapterTSH->SCSI_AF_dvState(targetTSCB->scsiID) = SCSI_DV_DONE;
            SCSIxQueueDomainValidation(adapterTSH);
            return(HIM_SUCCESS);
         }
      }
      else
      {
         /* Just give up */
         adapterTSH->retryBusScan = 0;
         adapterTSH->SCSI_AF_dvState(targetTSCB->scsiID) = SCSI_DV_DONE;
         SCSIxQueueDomainValidation(adapterTSH);
         return(HIM_SUCCESS);
      }
   }
   else if (iob->taskStatus == HIM_IOB_BUSY)
   {
      /* must free the current unit handle */
      SCSI_FREE_UNIT_HANDLE(*((SCSI_UNIT_HANDLE HIM_PTR)&unitHandle));
      
      /* Unfreeze the queues */
      SCSIxQueueSpecialIOB(adapterTSH, HIM_UNFREEZE_QUEUE);
         
      /* Retry twice and then give up */
      if (adapterTSH->retryBusScan < 2)
      {
         adapterTSH->retryBusScan++;
         adapterTSH->SCSI_AF_dvState(targetTSCB->scsiID) = SCSI_DV_READY;
         SCSIxQueueDomainValidation(adapterTSH);
         return(HIM_SUCCESS);
      }
      else
      {
         adapterTSH->retryBusScan = 0;
         adapterTSH->SCSI_AF_dvState(targetTSCB->scsiID) = SCSI_DV_DONE;
         SCSIxQueueDomainValidation(adapterTSH);
         return(HIM_SUCCESS);
      }
   }
   else
   {
      adapterTSH->retryBusScan = 0;
   }

   /* check IOB status */
   switch( iob->taskStatus)
   {
      case HIM_IOB_DATA_OVERUNDERRUN:
         residual = (HIM_UINT8)iob->residual;
         /* fall thru !!! */

      case HIM_IOB_GOOD:
         /* inquiry is done, we have to verify inquiry data */
         for (i = 0; i < SCSI_SIZE_INQUIRY - residual; i++)
         {
            if ((((SCSI_UEXACT8 HIM_PTR )adapterTSH->moreUnlocked) + SCSI_MORE_REFDATA + targetTSCB->scsiID * SCSI_SIZE_INQUIRY)[i] !=
               (((SCSI_UEXACT8 HIM_PTR )adapterTSH->moreLocked) + SCSI_MORE_INQDATA)[i])
            {
               if (i & 1)
               {
                  ++oddCount;
               }
               else
               {
                  ++evenCount;
               }
            }
         }

         if (!(oddCount + evenCount))
         {
            /* device passed with domain validation */
            adapterTSH->SCSI_AF_dvPassed(targetTSCB->scsiID) = 1;
            adapterTSH->SCSI_AF_dvState(targetTSCB->scsiID) = SCSI_DV_DONE;
            break;
         }
         goto dv_retry_basic;

      case HIM_IOB_ABORTED_REQ_BUS_RESET:
      case HIM_IOB_ABORTED_CHIM_RESET:
      case HIM_IOB_ABORTED_TRANSPORT_MODE_CHANGE:
      case HIM_IOB_ABORTED_3RD_PARTY_RESET:
         /* abort protocol auto config because of a catastrophic event */
         /* post protocol auto config back to osm */
         adapterTSH->iobProtocolAutoConfig->taskStatus = iob->taskStatus;
         adapterTSH->iobProtocolAutoConfig->postRoutine(adapterTSH->iobProtocolAutoConfig);
         hhcb->SCSI_HF_dvInProgress = 0;
         return(HIM_SUCCESS);

      case HIM_IOB_HOST_ADAPTER_FAILURE:
         /* Just throttle down the xfer, and continue the domain validation */
         /* Falling thru !!! */

      default:
dv_retry_basic:
         /* retry with throttling speed/width */
         if (!SCSIxThrottleTransfer(adapterTSH,targetTSCB))
         {
            /* go ahead retry it */
            adapterTSH->SCSI_AF_dvState(targetTSCB->scsiID) = SCSI_DV_READY;
         }
         else
         {
            /* we are done with this device */
            /* ??? TBD */
            adapterTSH->SCSI_AF_dvState(targetTSCB->scsiID) = SCSI_DV_DONE;
         }
         break;
   }

   /* proceed with Domain Validation */ 
   SCSIxQueueDomainValidation(adapterTSH);

   return(HIM_SUCCESS);
}
#endif   /* SCSI_DOMAIN_VALIDATION */

/*********************************************************************
*
*  SCSIxPostDomainValidationEnhanced
*
*     Post routine for Domain Validation level 2
*
*  Return Value:  status
*                  
*  Parameters:    adapterTSH
*
*  Remarks:
*
*********************************************************************/
#if SCSI_DOMAIN_VALIDATION_ENHANCED
HIM_UINT32 SCSIxPostDomainValidationEnhanced (HIM_IOB HIM_PTR iob)
{
   SCSI_TARGET_TSCB HIM_PTR targetTSCB =
                              (SCSI_TARGET_TSCB HIM_PTR) iob->taskSetHandle;
   SCSI_UNIT_CONTROL HIM_PTR unitHandle = &targetTSCB->unitControl;
   SCSI_ADAPTER_TSCB HIM_PTR adapterTSH = targetTSCB->adapterTSH;
   HIM_UINT8 i, oddCount = 0, evenCount = 0;
   SCSI_UEXACT8 senseKey;
   SCSI_UEXACT16 bitPattern = (SCSI_UEXACT16)0x0001;
   SCSI_HHCB HIM_PTR hhcb = &adapterTSH->hhcb;

   if (iob->taskStatus == HIM_IOB_ERRORDATA_VALID)
   {
      HIM_GET_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,&senseKey,iob->errorData,2);
      if ((senseKey & 0x0F) == 5)
      {
         /* illegal request may be caused by no echo buffer support */
         /* retry with level 1 which does not require echo buffer instead */
         adapterTSH->retryBusScan = 0;
         adapterTSH->SCSI_AF_dvLevel(targetTSCB->scsiID) = SCSI_DV_BASIC;
         adapterTSH->SCSI_AF_dvState(targetTSCB->scsiID) = SCSI_DV_READY;
         SCSIxQueueDomainValidation(adapterTSH);
         return(HIM_SUCCESS);
      }

      /* If UNIT_ATTENTION, or NOT_READY condition... */
      else if (((senseKey & 0x0F) == 2) || ((senseKey & 0x0F) == 6))
      {
         /* Retry twice and then give up */
         if (adapterTSH->retryBusScan < 2)
         {
            adapterTSH->retryBusScan++;
            adapterTSH->SCSI_AF_dvState(targetTSCB->scsiID) = SCSI_DV_READY;
            SCSIxQueueDomainValidation(adapterTSH);
            return(HIM_SUCCESS);
         }
         else
         {
            /* retry with level 1 which does not require echo buffer instead */
            adapterTSH->retryBusScan = 0;
            adapterTSH->SCSI_AF_dvLevel(targetTSCB->scsiID) = SCSI_DV_BASIC;
            adapterTSH->SCSI_AF_dvState(targetTSCB->scsiID) = SCSI_DV_READY;
            SCSIxQueueDomainValidation(adapterTSH);
            return(HIM_SUCCESS);
         }
      }
      else
      {
         /* Throttle down and retry */
         adapterTSH->retryBusScan = 0;
      }
   }
   else if (iob->taskStatus == HIM_IOB_BUSY)
   {
      /* must free the current unit handle */
      SCSI_FREE_UNIT_HANDLE(*((SCSI_UNIT_HANDLE HIM_PTR)&unitHandle));
      
      /* Unfreeze the queues */
      SCSIxQueueSpecialIOB(adapterTSH, HIM_UNFREEZE_QUEUE);
         
      /* Retry twice and then give up */
      if (adapterTSH->retryBusScan < 2)
      {
         adapterTSH->retryBusScan++;
         adapterTSH->SCSI_AF_dvState(targetTSCB->scsiID) = SCSI_DV_READY;
         SCSIxQueueDomainValidation(adapterTSH);
         return(HIM_SUCCESS);
      }
      else
      {
         /* retry with level 1 which does not require echo buffer instead */
         adapterTSH->retryBusScan = 0;
         adapterTSH->SCSI_AF_dvLevel(targetTSCB->scsiID) = SCSI_DV_BASIC;
         adapterTSH->SCSI_AF_dvState(targetTSCB->scsiID) = SCSI_DV_READY;
         SCSIxQueueDomainValidation(adapterTSH);
         return(HIM_SUCCESS);
      }
   }
   else
   {
      adapterTSH->retryBusScan = 0;
   }

   /* check IOB status */
   switch( iob->taskStatus)
   {
      case HIM_IOB_GOOD:
         /* check if it's write buffer */
         if (adapterTSH->SCSI_AF_dvState(targetTSCB->scsiID) == SCSI_DV_WRITEBUFFER)
         {
            /* write buffer done, continue with read buffer */
            /* clear data buffer */
            for (i = 0; i < SCSI_SIZE_RWB; i++)
            {
               ((SCSI_UEXACT8 HIM_PTR)adapterTSH->moreLocked)[SCSI_MORE_RWBDATA+i] = (SCSI_UEXACT8) ~i;
            }
            break;
         }
         else
         {
            /* must be post for read buffer */
            /* check if data pattern is good */
            for (i = 0; i < 32; i++)
            {
               if (((SCSI_UEXACT8 HIM_PTR)adapterTSH->moreLocked)[SCSI_MORE_RWBDATA+i] != i)
               {
                  if (i & 1)
                  {
                     ++oddCount;
                  }
                  else
                  {
                     ++evenCount;
                  }
               }
            }
            for (i = 32; i < 48; i++)
            {
               if ((((SCSI_UEXACT8 HIM_PTR)adapterTSH->moreLocked)[SCSI_MORE_RWBDATA+i] != ((i & 0x02) ? (SCSI_UEXACT8)0xFF : (SCSI_UEXACT8)0x00)) ||
                   (((SCSI_UEXACT8 HIM_PTR)adapterTSH->moreLocked)[SCSI_MORE_RWBDATA+16+i] != ((i & 0x02) ? (SCSI_UEXACT8)0xAA : (SCSI_UEXACT8)0x55)))
               {
                  if (i & 1)
                  {
                     ++oddCount;
                  }
                  else
                  {
                     ++evenCount;
                  }
               }
            }
            for (i = 64; i < 128; i+=2)
            {
               if (((SCSI_UEXACT8 HIM_PTR)adapterTSH->moreLocked)[SCSI_MORE_RWBDATA+i] != ((i & 0x02) ? (SCSI_UEXACT8)((bitPattern & 0xFF00) >> 8) : (SCSI_UEXACT8)0xFF))
               {
                  ++evenCount;
               }

               if (((SCSI_UEXACT8 HIM_PTR)adapterTSH->moreLocked)[SCSI_MORE_RWBDATA+i+1] != ((i & 0x02) ? (SCSI_UEXACT8)(bitPattern & 0xFF) : (SCSI_UEXACT8)0xFF))
               {
                  ++oddCount;
               }
               if (i & 0x02) bitPattern <<= 1;
            }
   
            if (!(oddCount + evenCount))
            {
               /* we are done with this device */
               adapterTSH->SCSI_AF_dvPassed(targetTSCB->scsiID) = 1;
               adapterTSH->SCSI_AF_dvState(targetTSCB->scsiID) = SCSI_DV_DONE;
               break;
            }
         }
         goto dv_retry_enhanced;

      case HIM_IOB_ABORTED_REQ_BUS_RESET:
      case HIM_IOB_ABORTED_CHIM_RESET:
      case HIM_IOB_ABORTED_TRANSPORT_MODE_CHANGE:
      case HIM_IOB_ABORTED_3RD_PARTY_RESET:
         /* abort protocol auto config because of a catastrophic event */
         /* post protocol auto config back to osm */
         adapterTSH->iobProtocolAutoConfig->taskStatus = iob->taskStatus;
         adapterTSH->iobProtocolAutoConfig->postRoutine(adapterTSH->iobProtocolAutoConfig);
         hhcb->SCSI_HF_dvInProgress = 0;
         return(HIM_SUCCESS);

      case HIM_IOB_HOST_ADAPTER_FAILURE:
         /* Just throttle down the xfer, and continue the domain validation */
         /* Falling thru !!! */

      default:
dv_retry_enhanced:
         if (!SCSIxThrottleTransfer(adapterTSH, targetTSCB))
         {
            /* throttle down transfer successfully */
            /* go ahead retry it */
            adapterTSH->SCSI_AF_dvState(targetTSCB->scsiID) = SCSI_DV_READY;
         }
         else
         {
            /* can't throttle down anymore */
            /* just give up */
            adapterTSH->SCSI_AF_dvState(targetTSCB->scsiID) = SCSI_DV_DONE;
         }
         break;
   }

   /* proceed with Domain Validation */ 
   SCSIxQueueDomainValidation(adapterTSH);

   return(HIM_SUCCESS);
}
#endif   /* SCSI_DOMAIN_VALIDATION_ENHANCED */

/*********************************************************************
*
*  SCSIxThrottleTransfer
*
*     Throttle transfer speed and width for domain validation retry 
*
*  Return Value:  0 - transfer can be throttled down
*                 -1 - Can't throttle transfer anymore
*                  
*  Parameters:    adapterTSH
*                 targetTSCB
*
*  Remarks:       There are six possible stages in the process of throttling
*                 which is hold by SCSI_AF_dvThrottle under adapterTSH
*                 1. SCSI_DE_WIDE - DE/wide xfer (initial stage)
*                 2. SCSI_SE_WIDE - SE/wide xfer (initial stage)
*                 3. SCSI_SE_NARROW - SE/narrow xfer 
*                 4. SCSI_SE_WIDE_REPEAT - SE/wide xfer with lower speed (repeat to async)
*                 5. SCSI_SE_NARROW_REPEAT - SE/narrow xfer with lower speed (repeast to async) (initial stage)
*                 6. SCSI_ASYNC_NARROW - async/narrow xfer 
*
*********************************************************************/
#if SCSI_DOMAIN_VALIDATION
HIM_UINT32 SCSIxThrottleTransfer (SCSI_ADAPTER_TSCB HIM_PTR adapterTSH,
                                     SCSI_TARGET_TSCB HIM_PTR targetTSCB)
{
   static SCSI_UEXACT8 se_rate[] = { 0, 0, 0x03, 0x05, 0x05, 0x08, 0x08, 0x08, 0x0C, 0x0C, 0x0C,
                  0x0C, 0, 0, 0, 0 };
   SCSI_HHCB HIM_PTR hhcb = &adapterTSH->hhcb;
   int index, scsiRate, scsiOffset;
   HIM_BOOLEAN xferDE, xferWide;
   
   /* get current transfer rate and offset */
   scsiRate = SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].bayDvScsiRate;
   scsiOffset = SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].bayDvScsiOffset;

   /* check SE/DE, wide/narrow */
   xferDE = (((scsiRate & SCSI_ENABLE_CRC) && !(scsiRate & SCSI_SINGLE_EDGE)) != 0);
   xferWide = ((scsiRate & SCSI_WIDE_XFER) != 0);

   /* set index for throttle speed */
   index = scsiRate & 0x0F;

   if (xferDE)
   {
      /* must be DE/wide, just make it SE instead */
      if (index == 2)
      {
         /* Throttle down from 80MHz DE to 40MHz SE */
         scsiRate = SCSI_WIDE_XFER | SCSI_SINGLE_EDGE | se_rate[index];
         adapterTSH->SCSI_AF_dvThrottle(targetTSCB->scsiID) = SCSI_SE_WIDE;
      }
      else if (index < 5)
      {
         /* Throttle down from 40MHz/26.67MHz DE to 40MHz SE */
         scsiRate = SCSI_WIDE_XFER | SCSI_SINGLE_EDGE | se_rate[2];
         adapterTSH->SCSI_AF_dvThrottle(targetTSCB->scsiID) = SCSI_SE_WIDE;
      }
      else if (index < 8)
      {
         /* Throttle down from 20/16/13.33MHz DE to 20MHz SE */
         scsiRate = SCSI_WIDE_XFER | SCSI_SINGLE_EDGE | se_rate[3];
         adapterTSH->SCSI_AF_dvThrottle(targetTSCB->scsiID) = SCSI_SE_WIDE;
      }
      else if (index < 12)
      {
         /* Throttle down from 10/8/6.7/5.7MHz DE to 10MHz SE */
         scsiRate = SCSI_WIDE_XFER | SCSI_SINGLE_EDGE | se_rate[5];
         adapterTSH->SCSI_AF_dvThrottle(targetTSCB->scsiID) = SCSI_SE_WIDE;
      }
   }
   else
   {
      if (xferWide)
      {
         /* it's wide transfer */
         if (adapterTSH->SCSI_AF_dvThrottle(targetTSCB->scsiID) == SCSI_SE_WIDE ||
            adapterTSH->SCSI_AF_dvThrottle(targetTSCB->scsiID) == SCSI_DE_WIDE)
         {
            /* it's SE/wide, just make it SE/narrow */
            scsiRate &= ~SCSI_WIDE_XFER;
            adapterTSH->SCSI_AF_dvThrottle(targetTSCB->scsiID) = SCSI_SE_NARROW;
         }
         else
         {
            /* it's must be SE/wide repeat, just lower transfer rate */
            if (scsiOffset != 0)
            {
               /* lower the transfer rate */
               scsiRate = SCSI_WIDE_XFER | SCSI_SINGLE_EDGE | se_rate[index];
            }
            else
            {
               /* can't lower transfer rate anymore, make it narrow instead */
               scsiRate &= ~SCSI_WIDE_XFER;
               adapterTSH->SCSI_AF_dvThrottle(targetTSCB->scsiID) = SCSI_ASYNC_NARROW;
            }
         }
      }
      else
      {
         /* it's narrow transfer */
         if (scsiOffset == 0)
         {
            /* it's async/narrow we can't throttle anymore */
            return((HIM_UINT32)-1);
         }
         else
         {
            if (adapterTSH->SCSI_AF_dvThrottle(targetTSCB->scsiID) == SCSI_SE_NARROW)
            {
               /* it's SE/narrow, we will restore wide with lower transfer rate */
               scsiRate = SCSI_WIDE_XFER | SCSI_SINGLE_EDGE | se_rate[index];
               adapterTSH->SCSI_AF_dvThrottle(targetTSCB->scsiID) = SCSI_SE_WIDE_REPEAT;
            }
            else
            {
               /* it's must be SE/narrow repeat, we will just lower trasfer  */
               scsiRate = SCSI_SINGLE_EDGE | se_rate[index];
            }
         }
      }
   }

   if (! (scsiRate & 0x0F))
   {
      /* assume async xfer */
      scsiOffset = 0;
      scsiRate &= SCSI_WIDE_XFER;
      scsiRate |= SCSI_SINGLE_EDGE | 0x05;
   }

   /* Remember the xfer parameters */
   SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].bayDvScsiRate = scsiRate;
   SCSI_DEVICE_TABLE(hhcb)[targetTSCB->scsiID].bayDvScsiOffset = scsiOffset;

   /* Remember that we have fallen back from default xfer rate for this target*/
   adapterTSH->SCSI_AF_dvFallBack(targetTSCB->scsiID) = 1;

   return(0);
}
#endif   /* SCSI_DOMAIN_VALIDATION */

/*********************************************************************
*
*  SCSIxQueueSpecialIOB
*
*     Queue request for special function operation
*
*  Return Value:  none
*                  
*  Parameters:    adapterTSH
*                 iob special function type
*
*  Remarks:
*
*********************************************************************/
void SCSIxQueueSpecialIOB (SCSI_ADAPTER_TSCB HIM_PTR adapterTSH,
                           SCSI_UINT8 iobFunctionType)
{
   SCSI_TARGET_TSCB HIM_PTR targetTSCB = &adapterTSH->targetTSCBBusScan;
   HIM_IOB HIM_PTR iob;

   /* setup special iob request (unfreezeinquiry) for bus scan */
   /* get iob memory */
   iob = (HIM_IOB HIM_PTR) (((SCSI_UEXACT8 HIM_PTR)adapterTSH->moreUnlocked)+SCSI_MORE_SPECIAL_IOB);
   OSDmemset(iob,0,sizeof(HIM_IOB));

   /* setup iob reserved memory */
   iob->iobReserve.virtualAddress = (void HIM_PTR) (((SCSI_UEXACT8 HIM_PTR)
               adapterTSH->moreLocked) + SCSI_MORE_IOBRSV);
   iob->iobReserve.bufferSize = sizeof(SCSI_IOB_RESERVE);
   iob->iobReserve.busAddress = OSMxGetBusAddress(SCSI_ADPTSH(adapterTSH)->osmAdapterContext,
                  HIM_MC_LOCKED,iob->iobReserve.virtualAddress);
   OSDmemset(iob->iobReserve.virtualAddress,0,sizeof(SCSI_IOB_RESERVE));

   /* setup/validate temporary target task set handle */
   SCSI_SET_UNIT_HANDLE(&SCSI_ADPTSH(adapterTSH)->hhcb,
         &targetTSCB->unitControl,(SCSI_UEXACT8)targetTSCB->scsiID,
         (SCSI_UEXACT8)targetTSCB->lunID);

   /* setup iob for special function */
   iob->function = (HIM_UINT8) iobFunctionType;
   iob->taskSetHandle = (HIM_TASK_SET_HANDLE) targetTSCB;
   iob->postRoutine = (HIM_POST_PTR) SCSIxPostQueueSpecialIOB;

   /* execute it */
   SCSIQueueIOB(iob);
}

/*********************************************************************
*
*  SCSIxPostQueueSpecialIOB
*
*     Handling post completion for special function operation
*
*  Return Value:   status
*                  
*  Parameters:    iob
*
*  Remarks:
*
*********************************************************************/
HIM_UINT32 SCSIxPostQueueSpecialIOB (HIM_IOB HIM_PTR iob)
{

   switch(iob->taskStatus)
   {

      case HIM_IOB_GOOD:
         break;

      default:
         return(HIM_FAILURE);
   }
   return(HIM_SUCCESS); 
}


/*********************************************************************
*
*  SCSIxSaveCurrentScamWWIDs
*
*     Save the current scamWWIDs to previous scamWWIDs
*
*  Return Value:  none
*                  
*  Parameters:    adapterTSH
*
*  Remarks:
*
*********************************************************************/
#if SCSI_SCAM_ENABLE
void SCSIxSaveCurrentScamWWIDs (SCSI_ADAPTER_TSCB HIM_PTR adapterTSH)
{
   SCSI_HHCB HIM_PTR hhcb = &adapterTSH->hhcb;
   int i;

   /* fresh prevScamDevice bit-mapped */
   adapterTSH->prevScamDevice = 0;

   /* copy the current WWIDs and current scamDevice bit-mapped over as previous */
   for (i = 0; i < (int) hhcb->maxDevices; i++)
   {
      OSDmemcpy(adapterTSH->prevScamWWID[i],
                     SCSI_DEVICE_TABLE(hhcb)[i].scamWWID,SCSI_SIZEWWID);
      adapterTSH->prevScamDevice |= 
            ((SCSI_UEXACT16)SCSI_DEVICE_TABLE(hhcb)[i].SCSI_DF_scamDevice << i);
   }
}
#endif

/*********************************************************************
*
*  SCSIxDetermineFirmwareMode
*
*     Determines which firmware mode.
*
*  Return Value:  Firmware Mode
*
*  Parameters:    adapterTSH - Adapter Task Set Handle
*
*  Remarks:       None.
*
*********************************************************************/
SCSI_UEXACT8 SCSIxDetermineFirmwareMode (SCSI_ADAPTER_TSCB HIM_PTR adapterTSH)
{
   SCSI_HHCB HIM_PTR hhcb = &adapterTSH->hhcb;
   SCSI_UEXACT8 fwMode = SCSI_FMODE_NOT_ASSIGNED;
   SCSI_UEXACT8 hwType;
  
   /* We need to setup hardware descriptor to point to the right */
   /* hardware before we determine the firmware mode, due to the */
   /* movement of some SCB RAM control bits from DEVCONFIG to DSCOMMAND */
   /* register in Bayonet/Scimitar/Trident. */
   SCSIHSetupHardware(hhcb);
   
   /* determine hardware type */
   if (SCSI_IS_TRIDENT_ID(adapterTSH->productID))
   {
      hwType = 0;      /* trident hardware */
   }

   else if (SCSI_IS_BAYONET_ID(adapterTSH->productID))
   {
      hwType = 1;      /* bayonet/scimitar hardware */
   }
   else if (SCSI_IS_EXCALIBUR_ID(adapterTSH->productID)) 
   {  
      hwType = 2;     /* excalibur hardware */
   }  
   else
   {
      hwType = 3;    /* all other AIC78xx hardware */ 
   }
    
   /* setting of firmware mode */
   switch (hwType)
   {
      case 0:
#if SCSI_STANDARD_160M_MODE
         if (SCSIHExtScbRAMAvail(hhcb))
         {
#if SCSI_TARGET_OPERATION
            /* only swapping 160m sequencer can be 
             * used if target mode enabled for the adapter
             */ 
            if (!hhcb->SCSI_HF_targetMode)
#endif /* SCSI_TARGET_OPERATION */
               fwMode = SCSI_FMODE_STANDARD_160M;
         }
#endif
#if SCSI_SWAPPING_160M_MODE
         if (fwMode == SCSI_FMODE_NOT_ASSIGNED)
         {
            fwMode = SCSI_FMODE_SWAPPING_160M;
         }
#endif
#if SCSI_STANDARD_160M_MODE
         if (fwMode == SCSI_FMODE_NOT_ASSIGNED)
         {
#if SCSI_TARGET_OPERATION
            /* only swapping 160m sequencer can be 
             * used if target mode enabled for the adapter
             */ 
            if (!hhcb->SCSI_HF_targetMode)
#endif /* SCSI_TARGET_OPERATION */
               /* Setup Trident to run on standard ultra 160m mode */
               /* using 16 internal SCBs */
               fwMode = SCSI_FMODE_STANDARD_160M;
         }
#endif
         /* Let's try to run Trident with Bayonet compatible mode */
#if SCSI_STANDARD_ADVANCED_MODE
         if ((fwMode == SCSI_FMODE_NOT_ASSIGNED) && (SCSIHExtScbRAMAvail(hhcb)))
         {
#if SCSI_TARGET_OPERATION
            /* only swapping advanced sequencer can be 
             * used if target mode enabled for the adapter
             */ 
            if (!hhcb->SCSI_HF_targetMode)
#endif /* SCSI_TARGET_OPERATION */
               fwMode = SCSI_FMODE_STANDARD_ADVANCED;
         }
#endif
#if SCSI_SWAPPING_ADVANCED_MODE
         if (fwMode == SCSI_FMODE_NOT_ASSIGNED)
         {
            fwMode = SCSI_FMODE_SWAPPING_ADVANCED;
         }
#endif
#if SCSI_STANDARD_ADVANCED_MODE
         if (fwMode == SCSI_FMODE_NOT_ASSIGNED)
         {
#if SCSI_TARGET_OPERATION
            /* only swapping advanced sequencer can be 
             * used if target mode enabled for the adapter
             */ 
            if (!hhcb->SCSI_HF_targetMode)
#endif /* SCSI_TARGET_OPERATION */
               /* Setup Trident to run on standard advanced mode */
               /* using 16 internal SCBs */
               fwMode = SCSI_FMODE_STANDARD_ADVANCED;
         }
#endif
         break;

      case 1:
#if SCSI_STANDARD_ADVANCED_MODE
         if (SCSIHExtScbRAMAvail(hhcb))
         {
#if SCSI_TARGET_OPERATION
            /* only swapping advanced sequencer can be 
             * used if target mode enabled for the adapter
             */ 
            if (!hhcb->SCSI_HF_targetMode)
#endif /* SCSI_TARGET_OPERATION */
            fwMode = SCSI_FMODE_STANDARD_ADVANCED;
         }
#endif /* SCSI_STANDARD_ADVANCED_MODE */
#if SCSI_SWAPPING_ADVANCED_MODE
         if (fwMode == SCSI_FMODE_NOT_ASSIGNED)
         {
            fwMode = SCSI_FMODE_SWAPPING_ADVANCED;
         }
#endif
#if SCSI_STANDARD_ADVANCED_MODE
         if (fwMode == SCSI_FMODE_NOT_ASSIGNED)
         {
#if SCSI_TARGET_OPERATION
            /* only swapping advanced sequencer can be 
             * used if target mode enabled for the adapter
             */ 
            if (!hhcb->SCSI_HF_targetMode)
#endif /* SCSI_TARGET_OPERATION */
            fwMode = SCSI_FMODE_STANDARD_ADVANCED;
         }
#endif /* SCSI_STANDARD_ADVANCED_MODE */
         break;
       
      case 2:
#if SCSI_FORCE_STANDARD64_MODE
#if SCSI_STANDARD64_MODE
         fwMode = SCSI_FMODE_STANDARD64;
#endif
#else
#if SCSI_STANDARD64_MODE
         if (SCSIHExtScbRAMAvail(hhcb))
         {
            fwMode = SCSI_FMODE_STANDARD64;
         }
#endif
#if (SCSI_SWAPPING32_MODE  && (OSM_BUS_ADDRESS_SIZE == 32))
         if (fwMode == SCSI_FMODE_NOT_ASSIGNED)
         {
            fwMode = SCSI_FMODE_SWAPPING32;
         }
#endif
#if (SCSI_SWAPPING64_MODE && (OSM_BUS_ADDRESS_SIZE == 64))
         if (fwMode == SCSI_FMODE_NOT_ASSIGNED)
         {
            fwMode = SCSI_FMODE_SWAPPING64;
         }
#endif
#if SCSI_STANDARD64_MODE
         if (fwMode == SCSI_FMODE_NOT_ASSIGNED)
         {
             fwMode = SCSI_FMODE_STANDARD64;
         }
#endif
#endif
#if SCSI_TARGET_OPERATION
         /* target mode is not supported for this hardware */ 
         if (hhcb->SCSI_HF_targetMode)
         {
            /* target mode operation not supported - yet */
            fwMode = SCSI_FMODE_NOT_ASSIGNED;
         }
#endif /* SCSI_TARGET_OPERATION */
         break;
 
      case 3:
#if SCSI_SWAPPING32_MODE
         fwMode = SCSI_FMODE_SWAPPING32;
#endif         
#if (SCSI_SWAPPING64_MODE && (OSM_BUS_ADDRESS_SIZE == 64))
         if (fwMode == SCSI_FMODE_NOT_ASSIGNED)
         {
            fwMode = SCSI_FMODE_SWAPPING64;
         }  
#endif
#if SCSI_TARGET_OPERATION
         /* target mode is not supported for this hardware */ 
         if (hhcb->SCSI_HF_targetMode)
         {
            /* target mode operation not supported - yet */
            fwMode = SCSI_FMODE_NOT_ASSIGNED;
         }
#endif /* SCSI_TARGET_OPERATION */
         break;
   }

   return(fwMode);
}

/*********************************************************************
*
*  SCSIxGetNextSubSystemSubVendorID
*
*     This routine returns the next host device type supported by
*     SCSI CHIM.
*
*  Return Value:  0 - No more device type
*                 host id
*                  
*  Parameters:    index
*                 hostBusType
*                 mask
*
*  Remarks:       SCSI CHIM will make a subsystem/subvendor id table built
*                 at compile time. Parameter index will be used to
*                 index into that table. The table itself should be
*                 static at initialization/run time. The table is
*                 maintained at CHIM layer.
*
*********************************************************************/
HIM_HOST_ID SCSIxGetNextSubSystemSubVendorID (HIM_UINT16 index,
                                              HIM_HOST_ID HIM_PTR mask)
{
   HIM_HOST_ID hostID = 0;
   if (index < (sizeof(SCSISubSystemSubVendorType) / sizeof(SCSI_HOST_TYPE)))
   {
      hostID = SCSISubSystemSubVendorType[index].idHost;
      *mask = SCSISubSystemSubVendorType[index].idMask;
   }
   
   return(hostID);
}

/*********************************************************************
*
*  SCSIxBayonetGetNextSubSystemSubVendorID
*
*     This routine returns the next host device type not supported by
*     SCSI CHIM.
*
*  Return Value:  0 - No more device type
*                 host id
*                  
*  Parameters:    index
*                 mask
*
*  Remarks:       SCSI CHIM will make a subsystem/subvendor id table built
*                 at compile time. Parameter index will be used to
*                 index into that table. The table itself should be
*                 static at initialization/run time. The table is
*                 maintained at CHIM layer.
*
*********************************************************************/
HIM_HOST_ID SCSIxBayonetGetNextSubSystemSubVendorID (HIM_UINT16 index,
                                                     HIM_HOST_ID HIM_PTR mask)
{
   HIM_HOST_ID hostID = 0;
   if (index < (sizeof(SCSIBayonetSubSystemSubVendorType) / sizeof(SCSI_HOST_TYPE)))
   {
      hostID = SCSIBayonetSubSystemSubVendorType[index].idHost;
      *mask = SCSIBayonetSubSystemSubVendorType[index].idMask;
   }
   
   return(hostID);
}

/*********************************************************************
*
*  SCSIxTridentGetNextSubSystemSubVendorID
*
*     This routine returns the next host device type not supported by
*     SCSI CHIM.
*
*  Return Value:  0 - No more device type
*                 host id
*                  
*  Parameters:    index
*                 mask
*
*  Remarks:       SCSI CHIM will make a subsystem/subvendor id table built
*                 at compile time. Parameter index will be used to
*                 index into that table. The table itself should be
*                 static at initialization/run time. The table is
*                 maintained at CHIM layer.
*
*********************************************************************/
HIM_HOST_ID SCSIxTridentGetNextSubSystemSubVendorID (HIM_UINT16 index,
                                                     HIM_HOST_ID HIM_PTR mask)
{
   HIM_HOST_ID hostID = 0;
   if (index < (sizeof(SCSITridentSubSystemSubVendorType) / sizeof(SCSI_HOST_TYPE)))
   {
      hostID = SCSITridentSubSystemSubVendorType[index].idHost;
      *mask = SCSITridentSubSystemSubVendorType[index].idMask;
   }
   
   return(hostID);
}

#if SCSI_SCSISELECT_SUPPORT
/*********************************************************************
*
*  SCSIxDetermineValidNVData
*
*     This routine determines if the NV RAM is valid. 
*     
*
*  Return Value:  0 - NV RAM check failed
*                 1-  NV RAM check successful
*                  
*  Parameters:    dataBuffer
*                 
*
*  Remarks:       
*                 
*********************************************************************/
SCSI_UEXACT8 SCSIxDetermineValidNVData (SCSI_NVM_LAYOUT HIM_PTR nvmdataptr)

{

   SCSI_UEXACT16 sum;
   SCSI_UEXACT8 seepromAllzero = 1;
   SCSI_UEXACT8 seepromAllff = 1;
   SCSI_UEXACT8 seepromdefaultchk = 1;
   SCSI_UINT i;
   SCSI_UEXACT16 HIM_PTR databuffer = (SCSI_UEXACT16 HIM_PTR) nvmdataptr;
      
   /* check to see if seeprom are all 0s or all 0xFFFFs */
   for (i = 0; i < (SCSI_SINT16) sizeof(SCSI_NVM_LAYOUT)/2; i++)
   {   
      if (databuffer[i] != 0)

      {
         seepromAllzero = 0;
         break;
      }
   }

   if (seepromAllzero)
   {
     seepromdefaultchk = 0;
   }

   for (i = 0; i < (SCSI_SINT16) sizeof(SCSI_NVM_LAYOUT)/2; i++)
   {   
   
      if (databuffer[i] != 0xFFFF)
   
      {
         seepromAllff = 0;
         break;
      }
   }
   
   if (seepromAllff)
   {
     seepromdefaultchk = 0;
   }

   /* calculate check sum */
   sum = 0;
   for (i = 0; i < OSMoffsetof(SCSI_NVM_LAYOUT,checkSum)/2; i++)
   {
      sum += databuffer[i];
   }

   /* make sure check sum matched */
   if (sum != (nvmdataptr->checkSum))
   {
     seepromdefaultchk = 0;
   }

   return(seepromdefaultchk);
}
#endif

#if SCSI_TARGET_OPERATION
/*********************************************************************
*
*  OSDBuildTargetSCB
*
*     Build scb buffer
*
*  Return Value:  void
*                  
*  Parameters:    exercise
*
*  Remarks:       This routine is defined for internal HIM reference
*
*********************************************************************/
void OSDBuildTargetSCB (SCSI_HIOB SCSI_IPTR hiob)
{
   HIM_IOB HIM_PTR iob = SCSI_GETIOB(hiob);
   SCSI_NEXUS SCSI_XPTR nexusTSH = SCSI_NEXUS_UNIT(hiob);
   SCSI_HHCB SCSI_HPTR hhcb = nexusTSH->hhcb;
   SCSI_ADAPTER_TSCB HIM_PTR adapterTSH = SCSI_GETADP(hhcb);
  
   /* invoke the proper routine to build SCB */
   adapterTSH->OSDBuildTargetSCB(hiob,iob,nexusTSH,adapterTSH);
}

/*********************************************************************
*
*  OSDSwapping160mBuildTargetSCB
*
*  Build SCB for target request
*
*  Return Value:  void
*                  
*  Parameters:    hiob
*
*  Remarks: This routine is used for establish connection scbs also.
*
*********************************************************************/ 
#if SCSI_SWAPPING_160M_MODE
void OSDSwapping160mBuildTargetSCB(SCSI_HIOB HIM_PTR hiob,
                                   HIM_IOB HIM_PTR iob,
                                   SCSI_NEXUS SCSI_XPTR nexusTSH,
                                   SCSI_ADAPTER_TSCB HIM_PTR adapterTSH)
{
   SCSI_IOB_RESERVE HIM_PTR iobReserve =
                  (SCSI_IOB_RESERVE HIM_PTR)iob->iobReserve.virtualAddress;
   SCSI_HHCB HIM_PTR hhcb = &adapterTSH->hhcb;
   SCSI_SCB_SWAPPING_160M HIM_PTR scbBuffer =
         (SCSI_SCB_SWAPPING_160M HIM_PTR) hiob->scbDescriptor->scbBuffer.virtualAddress;
   SCSI_EST_SCB_SWAPPING_160M HIM_PTR estscbBuffer =
      (SCSI_EST_SCB_SWAPPING_160M HIM_PTR) hiob->scbDescriptor->scbBuffer.virtualAddress;
   SCSI_UEXACT32 sgData;
   SCSI_BUS_ADDRESS busAddress;
   SCSI_UEXACT8 sg_cache_SCB;
   SCSI_UEXACT8 scontrol;
   SCSI_UEXACT8 starget;
   SCSI_UEXACT8 estScontrol;   /* scontrol for establish scb */ 

   /* build scb buffer */
   /* Use this routine to build establish connection SCBs also. */
   if (hiob->cmd == SCSI_CMD_ESTABLISH_CONNECTION)
   {
      /* Code to build establish connection SCB */
      /* Moving these to scratch ram to save build time */
      if (!hiob->SCSI_IF_disallowDisconnect && hhcb->SCSI_HF_targetDisconnectAllowed)
      {
         estScontrol = SCSI_EST_W160M_DISCON_EN;
      }
      else
      {
         estScontrol = 0; /* initialize */
      }

      if (hhcb->SCSI_HF_targetEnableScsi1Selection)
      {
         estScontrol |= SCSI_EST_W160M_SCSI1_EN;
      }

      if (hhcb->SCSI_HF_targetHostTargetVersion == SCSI_VERSION_3)
      {
         estScontrol |= SCSI_EST_W160M_SCSI3_EN;
      }

      if (hhcb->SCSI_HF_targetScsi2IdentifyMsgRsv)
      {
         estScontrol |= SCSI_EST_W160M_CHK_RSVD_BITS;
      }

      if (hhcb->SCSI_HF_targetScsi2RejectLuntar)
      {
         estScontrol |= SCSI_EST_W160M_LUNTAR_EN;
      }

      if (hhcb->SCSI_HF_targetTagEnable)
      {
         estScontrol |= SCSI_EST_W160M_TAG_ENABLE;
      }
      
      HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,estscbBuffer,
         OSMoffsetof(SCSI_EST_SCB_SWAPPING_160M,scontrol),estScontrol);  

      /* scb # */
      HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,estscbBuffer,
         OSMoffsetof(SCSI_EST_SCB_SWAPPING_160M,current_SCB),hiob->scbDescriptor->scbNumber);  
      
      /* next_SCB */
      HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,estscbBuffer,
         OSMoffsetof(SCSI_EST_SCB_SWAPPING_160M,next_SCB),hiob->scbDescriptor->queueNext->scbNumber);  
   
      /* PCI address of this SCB */
      busAddress = hiob->scbDescriptor->scbBuffer.busAddress;
      SCSI_hPUTBUSADDRESS(adapterTSH->hhcb,estscbBuffer,
         OSMoffsetof(SCSI_EST_SCB_SWAPPING_160M,scbaddress0),
         busAddress);

      /* flush the cache ready for dma */
      SCSI_FLUSH_CACHE(estscbBuffer,sizeof (SCSI_EST_SCB_SWAPPING_160M));
   }
   else
   {
      /* Reestablish SCBs */ 

      starget = 0;
      starget = nexusTSH->scsiID << 4;    /* other ID */
#if SCSI_MULTIPLEID_SUPPORT
      starget |= nexusTSH->selectedID & 0x0f; /* own ID */
#else
      starget |= hhcb->hostScsiID & 0x0f; /* own ID */
#endif /* SCSI_MULTIPLEID_SUPPORT */
      HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
         OSMoffsetof(SCSI_SCB_SWAPPING_160M,starget),starget);  
  
      if (nexusTSH->SCSI_XF_tagRequest) 
      {
         scontrol = 
            SCSI_W160M_DISCENB|SCSI_W160M_SIMPLETAG|SCSI_W160M_TAGENB|SCSI_W160M_TARGETENB;
         
         HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
            OSMoffsetof(SCSI_SCB_SWAPPING_160M,SCSI_W160M_starg_tagno),nexusTSH->queueTag);  
      }
      else
      {    
         scontrol = SCSI_W160M_DISCENB|SCSI_W160M_TARGETENB; 
      }   

      if (hhcb->SCSI_HF_targetHostTargetVersion == HIM_SCSI_2 &&
          !hhcb->SCSI_HF_targetScsi2RejectLuntar)
      {
         /* Need to include luntar bit in response */
         HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
            OSMoffsetof(SCSI_SCB_SWAPPING_160M,slun),
            ((nexusTSH->lunID) | (nexusTSH->SCSI_XF_lunTar<<5)));  
      }
      else
      {
         HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
            OSMoffsetof(SCSI_SCB_SWAPPING_160M,slun),nexusTSH->lunID);  
      }
      if (iob->function == HIM_REESTABLISH_AND_COMPLETE)
      { 
         scontrol &= ~SCSI_W160M_DISCENB;
         /* put in SCSI status ( assume 1 byte) */ 
         /* if (iob->targetCommandLength > 1)
            {
               HIM_PUT_BYTE_STRING(adapterTSH->osmAdapterContext,scbBuffer,
                  OSMoffsetof(SCSI_SCB_SWAPPING_160M,SCSI_WADV_starg_status),iob->targetCommand,1);    
            }
            else
            { 
               HIM_PUT_BYTE_STRING(adapterTSH->osmAdapterContext,scbBuffer,
                  OSMoffsetof(SCSI_SCB_SWAPPING_160M,SCSI_WADV_starg_status),iob->targetCommand,iob->targetCommandLength);    
            }
          */
         HIM_PUT_BYTE_STRING(adapterTSH->osmAdapterContext,scbBuffer,
            OSMoffsetof(SCSI_SCB_SWAPPING_160M,SCSI_W160M_starg_status),iob->targetCommand,1);    
      }
      else if ( (!nexusTSH->SCSI_XF_disconnectAllowed) ||
                (nexusTSH->SCSI_XF_lastResource) ||
                (hiob->SCSI_IF_disallowDisconnect) )
      {
         /* Hold the SCSI bus when                       */
         /*    1) disconnect privilege is not granted    */
         /*    2) the last resource is being used        */
         /*    3) the OSM specified not to disconect     */ 
         scontrol |= SCSI_W160M_HOLDONBUS;
         scontrol &= ~SCSI_W160M_DISCENB;
      }

      if (!iob->data.bufferSize)
      {
         /* If the data buffer is not set up then don't 
          * attempt to transfer data. */
         sg_cache_SCB = SCSI_W160M_NODATA;
      }
      else if (iob->flagsIob.outboundData)
      {
         scontrol |= SCSI_W160M_DATAOUT;
         sg_cache_SCB = 0;
      }
      else if (iob->flagsIob.inboundData)
      {
         scontrol &= ~SCSI_W160M_DATAOUT;
         sg_cache_SCB = 0;
      }
      else
      {
         sg_cache_SCB = SCSI_W160M_NODATA;
      }

      HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
         OSMoffsetof(SCSI_SCB_SWAPPING_160M,scontrol),scontrol);

      /* Data Transfer */
      if (!(sg_cache_SCB & SCSI_W160M_NODATA))
      {
         /* setup the embedded s/g segment (the first s/g segment) */
         HIM_GET_LITTLE_ENDIAN32(adapterTSH->osmAdapterContext,&sgData,iob->data.virtualAddress,
            sizeof(SCSI_BUS_ADDRESS));

         /* Special code to handle one segment with a zero length */ 
         if (sgData == (SCSI_UEXACT32)0x80000000)
         {
            /* indicate no data transfer */
            sg_cache_SCB |= SCSI_W160M_NODATA;
         }
         else
         {
            if (sgData & (SCSI_UEXACT32)0x80000000)
            {
               /* There is only one s/g element in s/g list */
               sg_cache_SCB |= SCSI_W160M_ONESGSEG;
            }
            else
            {
               /* more than one s/g segments and need to setup s/g pointer */
               SCSI_hPUTBUSADDRESS(adapterTSH->hhcb,scbBuffer,OSMoffsetof(
                  SCSI_SCB_SWAPPING_160M,SCSI_W160M_sg_pointer_SCB0),
                  iob->data.busAddress);
            }

            /* setup embedded sg segment length */
            HIM_PUT_LITTLE_ENDIAN24(adapterTSH->osmAdapterContext,scbBuffer,OSMoffsetof(
               SCSI_SCB_SWAPPING_160M,slength0),sgData);

            /* setup embedded sg segment address */
            SCSI_hGETBUSADDRESS(&adapterTSH->hhcb,&busAddress,
               iob->data.virtualAddress,0);
            SCSI_hPUTBUSADDRESS(adapterTSH->hhcb,scbBuffer,
               OSMoffsetof(SCSI_SCB_SWAPPING_160M,saddress0),busAddress);
                                       
         } 
      }

      HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
         OSMoffsetof(SCSI_SCB_SWAPPING_160M,sg_cache_SCB),
            (sg_cache_SCB | (2 * sizeof(SCSI_BUS_ADDRESS))));
    
      HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
         OSMoffsetof(SCSI_SCB_SWAPPING_160M,next_SCB),
            hiob->scbDescriptor->queueNext->scbNumber);

      /* flush the cache ready for dma */
      SCSI_FLUSH_CACHE(scbBuffer,sizeof (SCSI_SCB_SWAPPING_160M));
   }
}
#endif /* SCSI_SWAPPING_160M_MODE */

/*********************************************************************
*
*  OSDSwappingAdvBuildTargetSCB
*
*  Build SCB for target request
*
*  Return Value:  void
*                  
*  Parameters:    hiob
*
*  Remarks: This routine is used for establish connection scbs also.
*
*********************************************************************/ 
#if SCSI_SWAPPING_ADVANCED_MODE
void OSDSwappingAdvBuildTargetSCB(SCSI_HIOB HIM_PTR hiob,
                                  HIM_IOB HIM_PTR iob,
                                  SCSI_NEXUS SCSI_XPTR nexusTSH,
                                  SCSI_ADAPTER_TSCB HIM_PTR adapterTSH)
{

   SCSI_IOB_RESERVE HIM_PTR iobReserve =
                  (SCSI_IOB_RESERVE HIM_PTR)iob->iobReserve.virtualAddress;
   SCSI_HHCB HIM_PTR hhcb = &adapterTSH->hhcb;
   SCSI_SCB_SWAPPING_ADVANCED HIM_PTR scbBuffer =
         (SCSI_SCB_SWAPPING_ADVANCED HIM_PTR) hiob->scbDescriptor->scbBuffer.virtualAddress;
   SCSI_EST_SCB_SWAPPING_ADVANCED HIM_PTR estscbBuffer =
      (SCSI_EST_SCB_SWAPPING_ADVANCED HIM_PTR) hiob->scbDescriptor->scbBuffer.virtualAddress;
   SCSI_UEXACT32 sgData;
   SCSI_BUS_ADDRESS busAddress;
   SCSI_UEXACT8 sg_cache_SCB;
   SCSI_UEXACT8 scontrol;
   SCSI_UEXACT8 starget;
   SCSI_UEXACT8 estScontrol;   /* scontrol for establish scb */ 

   /* build scb buffer */
   /* Use this routine to build establish connection SCBs also. */
   if (hiob->cmd == SCSI_CMD_ESTABLISH_CONNECTION)
   {
      /* Code to build establish connection SCB */
      /* Moving these to scratch ram to save build time */
      if (!hiob->SCSI_IF_disallowDisconnect && hhcb->SCSI_HF_targetDisconnectAllowed)
      {
         estScontrol = SCSI_EST_WADV_DISCON_EN;
      }
      else
      {
         estScontrol = 0; /* initialize */
      }

      if (hhcb->SCSI_HF_targetEnableScsi1Selection)
      {
         estScontrol |= SCSI_EST_WADV_SCSI1_EN;
      }

      if (hhcb->SCSI_HF_targetHostTargetVersion == SCSI_VERSION_3)
      {
         estScontrol |= SCSI_EST_WADV_SCSI3_EN;
      }

      if (hhcb->SCSI_HF_targetScsi2IdentifyMsgRsv)
      {
         estScontrol |= SCSI_EST_WADV_CHK_RSVD_BITS;
      }

      if (hhcb->SCSI_HF_targetScsi2RejectLuntar)
      {
         estScontrol |= SCSI_EST_WADV_LUNTAR_EN;
      }

      if (hhcb->SCSI_HF_targetTagEnable)
      {
         estScontrol |= SCSI_EST_WADV_TAG_ENABLE;
      }
      
      HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,estscbBuffer,
         OSMoffsetof(SCSI_EST_SCB_SWAPPING_ADVANCED,scontrol),estScontrol);  

      /* scb # */
      HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,estscbBuffer,
         OSMoffsetof(SCSI_EST_SCB_SWAPPING_ADVANCED,current_SCB),hiob->scbDescriptor->scbNumber);  
      
      /* next_SCB */
      HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,estscbBuffer,
         OSMoffsetof(SCSI_EST_SCB_SWAPPING_ADVANCED,next_SCB),hiob->scbDescriptor->queueNext->scbNumber);  
   
      /* PCI address of this SCB */
      busAddress = hiob->scbDescriptor->scbBuffer.busAddress;
      SCSI_hPUTBUSADDRESS(adapterTSH->hhcb,estscbBuffer,
         OSMoffsetof(SCSI_EST_SCB_SWAPPING_ADVANCED,scbaddress0),
         busAddress);

      /* flush the cache ready for dma */
      SCSI_FLUSH_CACHE(estscbBuffer,sizeof (SCSI_EST_SCB_SWAPPING_ADVANCED));
   }
   else
   {
      /* Reestablish SCBs */ 

      starget = 0;
      starget = nexusTSH->scsiID << 4;    /* other ID */
#if SCSI_MULTIPLEID_SUPPORT
      starget |= nexusTSH->selectedID & 0x0f; /* own ID */
#else
      starget |= hhcb->hostScsiID & 0x0f; /* own ID */
#endif /* SCSI_MULTIPLEID_SUPPORT */
      HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
         OSMoffsetof(SCSI_SCB_SWAPPING_ADVANCED,starget),starget);  
  
      if (nexusTSH->SCSI_XF_tagRequest) 
      {
         scontrol = 
            SCSI_WADV_DISCENB|SCSI_WADV_SIMPLETAG|SCSI_WADV_TAGENB|SCSI_WADV_TARGETENB;
         
         HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
            OSMoffsetof(SCSI_SCB_SWAPPING_ADVANCED,SCSI_WADV_starg_tagno),nexusTSH->queueTag);  
      }
      else
      {    
         scontrol = SCSI_WADV_DISCENB|SCSI_WADV_TARGETENB; 
      }   

      if (hhcb->SCSI_HF_targetHostTargetVersion == HIM_SCSI_2 &&
          !hhcb->SCSI_HF_targetScsi2RejectLuntar)
      {
         /* Need to include luntar bit in response */
         HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
            OSMoffsetof(SCSI_SCB_SWAPPING_ADVANCED,slun),
            ((nexusTSH->lunID) | (nexusTSH->SCSI_XF_lunTar<<5)));  
      }
      else
      {
         HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
            OSMoffsetof(SCSI_SCB_SWAPPING_ADVANCED,slun),nexusTSH->lunID);  
      }
      if (iob->function == HIM_REESTABLISH_AND_COMPLETE)
      { 
         scontrol &= ~SCSI_WADV_DISCENB;
         /* put in SCSI status ( assume 1 byte) */ 
         /* if (iob->targetCommandLength > 1)
            {
               HIM_PUT_BYTE_STRING(adapterTSH->osmAdapterContext,scbBuffer,
                  OSMoffsetof(SCSI_SCB_SWAPPING_ADVANCED,SCSI_WADV_starg_status),iob->targetCommand,1);    
            }
            else
            { 
               HIM_PUT_BYTE_STRING(adapterTSH->osmAdapterContext,scbBuffer,
                  OSMoffsetof(SCSI_SCB_SWAPPING_ADVANCED,SCSI_WADV_starg_status),iob->targetCommand,iob->targetCommandLength);    
            }
          */
         HIM_PUT_BYTE_STRING(adapterTSH->osmAdapterContext,scbBuffer,
            OSMoffsetof(SCSI_SCB_SWAPPING_ADVANCED,SCSI_WADV_starg_status),iob->targetCommand,1);    
      }
      else if ( (!nexusTSH->SCSI_XF_disconnectAllowed) ||
                (nexusTSH->SCSI_XF_lastResource) ||
                (hiob->SCSI_IF_disallowDisconnect) )
      {
         /* Hold the SCSI bus when                       */
         /*    1) disconnect privilege is not granted    */
         /*    2) the last resource is being used        */
         /*    3) the OSM specified not to disconect     */ 
         scontrol |= SCSI_WADV_HOLDONBUS;
         scontrol &= ~SCSI_WADV_DISCENB;
      }

      if (!iob->data.bufferSize)
      {
         /* If the data buffer is not set up then don't 
          * attempt to transfer data. */
         sg_cache_SCB = SCSI_WADV_NODATA;
      }
      else if (iob->flagsIob.outboundData)
      {
         scontrol |= SCSI_WADV_DATAOUT;
         sg_cache_SCB = 0;
      }
      else if (iob->flagsIob.inboundData)
      {
         scontrol &= ~SCSI_WADV_DATAOUT;
         sg_cache_SCB = 0;
      }
      else
      {
         sg_cache_SCB = SCSI_WADV_NODATA;
      }

      HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
         OSMoffsetof(SCSI_SCB_SWAPPING_ADVANCED,scontrol),scontrol);

      /* Data Transfer */
      if (!(sg_cache_SCB & SCSI_WADV_NODATA))
      {
         /* setup the embedded s/g segment (the first s/g segment) */
         HIM_GET_LITTLE_ENDIAN32(adapterTSH->osmAdapterContext,&sgData,iob->data.virtualAddress,
            sizeof(SCSI_BUS_ADDRESS));

         /* Special code to handle one segment with a zero length */ 
         if (sgData == (SCSI_UEXACT32)0x80000000)
         {
            /* indicate no data transfer */
            sg_cache_SCB |= SCSI_WADV_NODATA;
         }
         else
         {
            if (sgData & (SCSI_UEXACT32)0x80000000)
            {
               /* There is only one s/g element in s/g list */
               sg_cache_SCB |= SCSI_WADV_ONESGSEG;
            }
            else
            {
               /* more than one s/g segments and need to setup s/g pointer */
               SCSI_hPUTBUSADDRESS(adapterTSH->hhcb,scbBuffer,OSMoffsetof(
                  SCSI_SCB_SWAPPING_ADVANCED,SCSI_WADV_sg_pointer_SCB0),
                  iob->data.busAddress);
            }

            /* setup embedded sg segment length */
            HIM_PUT_LITTLE_ENDIAN24(adapterTSH->osmAdapterContext,scbBuffer,OSMoffsetof(
               SCSI_SCB_SWAPPING_ADVANCED,slength0),sgData);

            /* setup embedded sg segment address */
            SCSI_hGETBUSADDRESS(&adapterTSH->hhcb,&busAddress,
               iob->data.virtualAddress,0);
            SCSI_hPUTBUSADDRESS(adapterTSH->hhcb,scbBuffer,
               OSMoffsetof(SCSI_SCB_SWAPPING_ADVANCED,saddress0),busAddress);
                                       
         } 
      }

      HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
         OSMoffsetof(SCSI_SCB_SWAPPING_ADVANCED,sg_cache_SCB),
            (sg_cache_SCB | (2 * sizeof(SCSI_BUS_ADDRESS))));
    
      HIM_PUT_LITTLE_ENDIAN8(adapterTSH->osmAdapterContext,scbBuffer,
         OSMoffsetof(SCSI_SCB_SWAPPING_ADVANCED,next_SCB),
            hiob->scbDescriptor->queueNext->scbNumber);

      /* flush the cache ready for dma */
      SCSI_FLUSH_CACHE(scbBuffer,sizeof (SCSI_SCB_SWAPPING_ADVANCED));
   }
}
#endif /* SCSI_SWAPPING_ADVANCED_MODE */

/*********************************************************************
*
*  SCSIxTargetTaskManagementRequest
*
*     Target mode handling of a Task Management function
*
*  Return Value:  none 
*                  
*  Parameters:    iob
*                 hiob
*
*  Remarks:  This routine is invoked when a Task management function 
*            has been received from an initiator (indicated by an hiob 
*            stat value of SCSI_TASK_CMND_COMP).
*            The task management function (indicated by the contents of
*            the haStat field of the hiob) is translated.
*        
*********************************************************************/
void SCSIxTargetTaskManagementRequest (HIM_IOB HIM_PTR iob,
                          SCSI_HIOB HIM_PTR hiob)
{
   SCSI_NEXUS SCSI_XPTR nexus = SCSI_NEXUS_UNIT(hiob);
   SCSI_HHCB SCSI_XPTR hhcb = nexus->hhcb;
   SCSI_UEXACT8 SCSI_SPTR cmndBuffer;
   
   /* Check if TMF can be stored in the buffer */
   if (iob->targetCommandBufferSize >= 1)
   {
      iob->targetCommandLength = 1;
      cmndBuffer = iob->targetCommand;
      
      /* interpret haStat of hiob */
      switch(hiob->haStat)
      {
         case SCSI_HOST_TARGET_RESET:
            /* SCSI-2 Bus device reset or SCSI-3 Target Reset */
            cmndBuffer[0] = HIM_TARGET_RESET;
            break;

         case SCSI_HOST_ABORT_TASK_SET:
            /* SCSI-2 Abort or SCSI-3 Abort Task Set */
            cmndBuffer[0] = HIM_ABORT_TASK_SET;
            break;

         case SCSI_HOST_ABORT_TASK:
            /* SCSI-2 Abort Tag or SCSI-3 AbortTask */
            cmndBuffer[0] = HIM_ABORT_TASK;
            break; 
             
         case SCSI_HOST_CLEAR_TASK_SET:
            /* SCSI-2 Clear Queue or SCSI-3 ClearTask Set */
            cmndBuffer[0] = HIM_CLEAR_TASK_SET;
            break;

         case SCSI_HOST_TERMINATE_TASK:
            /* SCSI-2 Terminate I/O process or SCSI-3 Terminate Task */
            cmndBuffer[0] = HIM_TERMINATE_TASK;
            break;

         case SCSI_HOST_CLEAR_ACA:
            /* SCSI-3 Clear ACA */
            cmndBuffer[0] = HIM_CLEAR_XCA;
            break;

         case SCSI_HOST_LUN_RESET:
            /* SCSI-3 Logical Unit Reset */  
            cmndBuffer[0] = HIM_LOGICAL_UNIT_RESET;
            break;
           
         default:
            /* should never come to here */
            break;
     
      }  

      if (iob->function == HIM_ESTABLISH_CONNECTION)
      {
         iob->flagsIob.targetRequestType = HIM_REQUEST_TYPE_TMF;
         iob->taskStatus = HIM_IOB_GOOD;
      }
      else
      {
         iob->taskStatus = HIM_IOB_ABORTED_TMF_RECVD;
      }
   }   
   else
   {
      /* No room in the command buffer */   
      iob->targetCommandLength = 0;
      iob->taskStatus = HIM_IOB_TARGETCOMMANDBUFFER_OVERRUN;
      if (iob->function == HIM_ESTABLISH_CONNECTION)
      {
         iob->flagsIob.targetRequestType = HIM_REQUEST_TYPE_TMF;
      }
   }
     
}

#if SCSI_MULTIPLEID_SUPPORT
/*********************************************************************
*
*  SCSIxEnableID
*
*     Validate HIM_ENABLE_ID IOB function
*
*  Return Value:  HIM_TRUE if HIM_ENABLE_ID passed validation
*                  
*  Parameters:    iob
*
*  Remarks:
*
*********************************************************************/
HIM_BOOLEAN SCSIxEnableID (HIM_IOB HIM_PTR iob)
{
   HIM_TASK_SET_HANDLE taskSetHandle = iob->taskSetHandle;
   SCSI_HHCB HIM_PTR hhcb = &SCSI_ADPTSH(taskSetHandle)->hhcb;
   HIM_TS_ID_SCSI HIM_PTR transportSpecific = iob->transportSpecific;
   SCSI_UINT8 count;
   SCSI_UINT8 i;

   if (transportSpecific)
   { 
      count = 0; /*  use this variable as a counter */
      /* 16 bit field */
      for (i = 0; i < 16; i++)
      {
         /* count the bits */
         if (transportSpecific->scsiIDMask & (1 << i))
         {
            count++;
         }
         if (hhcb->SCSI_HF_targetAdapterIDMask & (1 << i))
         {
            count++;
         }
      }
   } 

   if (!transportSpecific ||
       count > hhcb->SCSI_HF_targetNumIDs ||
       !transportSpecific->scsiIDMask ||
       !hhcb->SCSI_HF_targetMode ||
       hhcb->SCSI_HF_initiatorMode)
   {
      return (HIM_FALSE);
   }

   return(HIM_TRUE);

}

/*********************************************************************
*
*  SCSIxDisableID
*
*     validate HIM_DISABLE_ID IOB function
*
*  Return Value:  HIM_TRUE if HIM_DISABLE_IOB passed validation
*                  
*  Parameters:    iob
*
*  Remarks:
*
*********************************************************************/
HIM_BOOLEAN SCSIxDisableID (HIM_IOB HIM_PTR iob)
{
   HIM_TASK_SET_HANDLE taskSetHandle = iob->taskSetHandle;
   SCSI_HHCB HIM_PTR hhcb = &SCSI_ADPTSH(taskSetHandle)->hhcb;
   HIM_TS_ID_SCSI HIM_PTR transportSpecific = iob->transportSpecific;

   if (!transportSpecific ||
       !hhcb->SCSI_HF_targetMode ||
       hhcb->SCSI_HF_initiatorMode)
   {
      return (HIM_FALSE);
   }

   return(HIM_TRUE);

}
#endif /* SCSI_MULTIPLEID_SUPPORT */
#endif /* SCSI_TARGET_OPERATION */

/*********************************************************************
*
*  OSDGetNSXBuffer
*
*     Obtain NSX buffer from IOB
*
*  Return Value:  void
*                  
*  Parameters:    hiob
*
*  Remarks:       This routine is defined for internal HIM reference.
*                 Returns a void pointer and size of data buffer to be
*                 used for NSX data received. Note that this saves
*                 allocating HIOB fields.                  
*
*********************************************************************/
#if SCSI_PAC_NSX_REPEATER || SCSI_NSX_REPEATER
SCSI_BUFFER_DESCRIPTOR SCSI_LPTR OSDGetNSXBuffer (SCSI_HIOB SCSI_IPTR hiob)
{
   HIM_IOB HIM_PTR iob = SCSI_GETIOB(hiob);
   HIM_TS_SCSI HIM_PTR transportSpecific = (HIM_TS_SCSI HIM_PTR)iob->transportSpecific;
   
   return((SCSI_BUFFER_DESCRIPTOR SCSI_LPTR)&(transportSpecific->nsxData));
}
#endif /* SCSI_PAC_NSX_REPEATER || SCSI_NSX_REPEATER */

/*********************************************************************
*
*  OSDReturnNSXStatus
*
*     Obtain NSX buffer from IOB
*
*  Return Value:  void
*                  
*  Parameters:    hiob
*                 status   0 = good  1 = overrun      
*                 count    number of bytes in data nsx buffer
*
*  Remarks:       This routine is defined for internal HIM reference.
*                 Sets the nsxStatus and nsxDataLength fields of the 
*                 transport specific area of the IOB.
*                 Note that this saves allocating HIOB fields.                  
*
*********************************************************************/
#if SCSI_PAC_NSX_REPEATER || SCSI_NSX_REPEATER
void OSDReturnNSXStatus (SCSI_HIOB HIM_PTR hiob, SCSI_UEXACT8 status, SCSI_UINT16 count)
{
   HIM_IOB HIM_PTR iob = SCSI_GETIOB(hiob);
   HIM_TS_SCSI HIM_PTR transportSpecific = (HIM_TS_SCSI HIM_PTR)iob->transportSpecific;

   if (status == SCSI_NSX_GOOD)
   {
      transportSpecific->nsxStatus = HIM_IOB_GOOD;
   }
   else
   if (status == SCSI_NSX_OVERRUN)
   {
      transportSpecific->nsxStatus = HIM_IOB_DATA_OVERUNDERRUN;
   }
   else
   if (status == SCSI_NSX_PROTOCOL_ERROR)
   {
      transportSpecific->nsxStatus = HIM_IOB_PROTOCOL_ERROR;
   }
   /* these are the only errors supported */   

   transportSpecific->nsxDataLength = count;

}

#endif /* SCSI_PAC_NSX_REPEATER || SCSI_NSX_REPEATER */

/*********************************************************************
*
*  SCSIxSetupDmaTSCB
*
*     Initialize Target TSCB to be used for DMA I/Os 
*
*  Return Value:  void
*                  
*  Parameters:    adapterTSH
*
*  Remarks:       This routine initializes the special dmaTargetTSCB
*                 which is used for HIM_INITIATE_DMA_TASK IOBs. 
*                 This routine is called during initialization and
*                 initializes the scsiID field of the Target TSCB
*                 to SCSI_DMA_ID (a value of 16) which cannot
*                 conflict with real SCSI IDs. 
*
*
*********************************************************************/
#if SCSI_DMA_SUPPORT
void SCSIxSetupDmaTSCB(SCSI_ADAPTER_TSCB HIM_PTR adapterTSH)
{
   SCSI_TARGET_TSCB HIM_PTR targetTSCB = &adapterTSH->dmaTargetTSCB;

   /* fresh the dma targetTSCB */
   OSDmemset(targetTSCB,0,sizeof(SCSI_TARGET_TSCB));

   /* setup targetTSCB */
   targetTSCB->typeTSCB = SCSI_TSCB_TYPE_TARGET;
   targetTSCB->adapterTSH = adapterTSH;
   targetTSCB->scsiID = SCSI_DMA_ID;
   targetTSCB->lunID = 0;
   SCSI_SET_UNIT_HANDLE(&SCSI_ADPTSH(adapterTSH)->hhcb,
                        &targetTSCB->unitControl,(SCSI_UEXACT8)targetTSCB->scsiID,
                        (SCSI_UEXACT8)targetTSCB->lunID);
}
#endif /* SCSI_DMA_SUPPORT */
