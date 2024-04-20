/*
 scsi.h
 scsichim.h
 scsiosm.h
 hardequ.h
 harddef.h
 firmequ.h
 firmdef.h
 scsiref.h
 scsidef.h
*/

/*$Header:   Y:/source/chimscsi/src/himscsi/hwmscsi/SCSI.HV_   1.11.7.0   13 Nov 1997 18:08:00   MAKI1985  $*/
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
*  Module Name:   SCSI.H
*
*  Description:   Shortcut front-end to include all other SCSI .h's.
*
*  Owners:        ECX IC Firmware Team
*
***************************************************************************/

typedef  struct SCSI_HIOB_ SCSI_HIOB;
typedef  struct SCSI_HHCB_ SCSI_HHCB;
typedef  struct SCSI_UNIT_CONTROL_ SCSI_UNIT_CONTROL;
typedef  struct SCSI_SCB_DESCRIPTOR_ SCSI_SCB_DESCRIPTOR;
typedef  struct SCSI_HW_INFORMATION_ SCSI_HW_INFORMATION;
/* SCSI_TARGET_OPERATION */
typedef  struct SCSI_NODE_ SCSI_NODE;
/* SCSI_TARGET_OPERATION */

#if !defined(SCSI_INTERNAL)
#include "chimhw.h"
#include "chimosm.h"
#include "chimcom.h"
#include "chimscsi.h"
#include "scsichim.h"
#include "scsiosm.h"
#else
typedef  union  SCSI_HOST_ADDRESS_ SCSI_HOST_ADDRESS;
typedef  struct  SCSI_BUFFER_DESCRIPTOR_ SCSI_BUFFER_DESCRIPTOR ;
#include "scsihw.h"
#include "scsiosd.h"
#include "scsicom.h"
#endif

/*$Header:   Y:/source/chimscsi/src/SCSICHIM.HV_   1.24.14.1   21 Nov 1997 17:23:16   LUU  $*/
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
*  Module Name:   SCSICHIM.H
*
*  Description:   Shortcut front-end to include all other SCSI .h's.
*
*  Owners:        ECX IC Firmware Team
*
***************************************************************************/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  Definitions originated from chimhw.h
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
#define  SCSI_HA_LITTLE_ENDIAN   HIM_HA_LITTLE_ENDIAN
#define  SCSI_SG_LIST_TYPE       HIM_SG_LIST_TYPE
#define  SCSI_HOST_BUS           HIM_HOST_BUS

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  Definitions originated from chimcom.h implementation
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
typedef HIM_DOUBLET SCSI_DOUBLET;
typedef HIM_QUADLET SCSI_QUADLET;
typedef HIM_OCTLET SCSI_OCTLET;
typedef HIM_BUFFER_DESCRIPTOR SCSI_BUFFER_DESCRIPTOR;
typedef HIM_HOST_ID SCSI_HOST_ID;
typedef HIM_HOST_ADDRESS SCSI_HOST_ADDRESS;
#define SCSI_HOST_BUS_PCI     HIM_HOST_BUS_PCI     /* PCI bus            */
#define SCSI_HOST_BUS_EISA    HIM_HOST_BUS_EISA    /* EISA bus           */
#define SCSI_MC_UNLOCKED      HIM_MC_UNLOCKED      /* unlocked memory    */
#define SCSI_MC_LOCKED        HIM_MC_LOCKED        /* locked memory      */

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  Definitions originated from chimosm.h
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
#define OSD_CPU_LITTLE_ENDIAN    OSM_CPU_LITTLE_ENDIAN
#define OSD_DMA_SWAP_ENDIAN      OSM_DMA_SWAP_ENDIAN  
#define OSD_DMA_SWAP_WIDTH       OSM_DMA_SWAP_WIDTH       
#define OSD_BUS_ADDRESS_SIZE     OSM_BUS_ADDRESS_SIZE 

#define SCSI_HPTR       HIM_PTR 
#define SCSI_IPTR       HIM_PTR 
#define SCSI_FPTR       HIM_PTR 
#define SCSI_SPTR       HIM_PTR 
#define SCSI_LPTR       HIM_PTR
#define SCSI_UPTR       HIM_PTR
#define SCSI_DPTR       HIM_PTR
#define SCSI_XPTR       HIM_PTR     /* Nexus */
#define SCSI_NPTR       HIM_PTR     /* Node  */

#define SCSI_SINT8      HIM_SINT8
#define SCSI_UINT8      HIM_UINT8   
#define SCSI_SEXACT8    HIM_SEXACT8 
#define SCSI_UEXACT8    HIM_UEXACT8 
#define SCSI_SINT16     HIM_SINT16  
#define SCSI_UINT16     HIM_UINT16  
#define SCSI_SEXACT16   HIM_SEXACT16
#define SCSI_UEXACT16   HIM_UEXACT16
#define SCSI_SINT32     HIM_SINT32
#define SCSI_UINT32     HIM_UINT32  
#define SCSI_SEXACT32   HIM_SEXACT32
#define SCSI_UEXACT32   HIM_UEXACT32

typedef HIM_BUS_ADDRESS SCSI_BUS_ADDRESS;
typedef HIM_BOOLEAN     SCSI_BOOLEAN;
#define SCSI_FALSE      HIM_FALSE
#define SCSI_TRUE       HIM_TRUE

typedef struct 
{
   HIM_IO_HANDLE ioHandle;
   HIM_UEXACT8 (*OSMReadUExact8)(HIM_IO_HANDLE,HIM_UINT32);
   void (*OSMWriteUExact8)(HIM_IO_HANDLE,HIM_UINT32,HIM_UEXACT8);
   void (*OSMSynchronizeRange)(HIM_IO_HANDLE,HIM_UINT32,HIM_UINT32);
} SCSI_IO_HANDLE;


#define SCSI_FLUSH_CACHE      HIM_FLUSH_CACHE
#define SCSI_INVALIDATE_CACHE HIM_INVALIDATE_CACHE

#define OSDmemset(buf,value,len) OSMmemset((void HIM_PTR)(buf),(SCSI_UEXACT8)(value),(int)(len))
#define OSDmemcmp(buf1,buf2,len) OSMmemcmp((void HIM_PTR)(buf1),(void HIM_PTR)(buf2),(int)(len))
#define OSDmemcpy(buf1,buf2,len) OSMmemcpy((void HIM_PTR)(buf1),(void HIM_PTR)(buf2),(int)(len))

#define OSDDebugMode       OSMDebugMode
#define OSDScreenHandle    OSMScreenHandle

#define OSDoffsetof        OSMoffsetof
#define OSDAssert          OSMAssert
#define OSDDebugPrint      OSMDebugPrint
#define OSDEnterDebugger   OSMEnterDebugger


#define SCSI_PUT_BYTE_STRING(hhcb,dmaStruct,offset,pSrc,length)\
            HIM_PUT_BYTE_STRING((hhcb)->osdHandle,dmaStruct,offset,pSrc,length)
#define SCSI_PUT_LITTLE_ENDIAN8(hhcb,dmaStruct,offset,value)   \
            HIM_PUT_LITTLE_ENDIAN8((hhcb)->osdHandle,(dmaStruct),(offset),(value))
#define SCSI_PUT_LITTLE_ENDIAN16(hhcb,dmaStruct,offset,value)  \
            HIM_PUT_LITTLE_ENDIAN16((hhcb)->osdHandle,(dmaStruct),(offset),(value))
#define SCSI_PUT_LITTLE_ENDIAN24(hhcb,dmaStruct,offset,value)  \
             HIM_PUT_LITTLE_ENDIAN24((hhcb)->osdHandle,(dmaStruct),(offset),(value))
#define SCSI_PUT_LITTLE_ENDIAN32(hhcb,dmaStruct,offset,value)  \
            HIM_PUT_LITTLE_ENDIAN32((hhcb)->osdHandle,(dmaStruct),(offset),(value))
#define SCSI_GET_BYTE_STRING(hhcb,pDest,dmaStruct,offset,length)\
            HIM_GET_BYTE_STRING((hhcb)->osdHandle,pDest,dmaStruct,offset,length)
#define SCSI_PUT_LITTLE_ENDIAN64(hhcb,dmaStruct,offset,value)  \
            HIM_PUT_LITTLE_ENDIAN64((hhcb)->osdHandle,(dmaStruct),(offset),(value))
#define SCSI_GET_LITTLE_ENDIAN8(hhcb,pValue,dmaStruct,offset)   \
            HIM_GET_LITTLE_ENDIAN8((hhcb)->osdHandle,(pValue),(dmaStruct),(offset))
#define SCSI_GET_LITTLE_ENDIAN16(hhcb,pValue,dmaStruct,offset)  \
            HIM_GET_LITTLE_ENDIAN16((hhcb)->osdHandle,(pValue),(dmaStruct),(offset))
#define SCSI_GET_LITTLE_ENDIAN24(hhcb,pValue,dmaStruct,offset)  \
            HIM_GET_LITTLE_ENDIAN24((hhcb)->osdHandle,(pValue),(dmaStruct),(offset))
#define SCSI_GET_LITTLE_ENDIAN32(hhcb,pValue,dmaStruct,offset)  \
            HIM_GET_LITTLE_ENDIAN32((hhcb)->osdHandle,(pValue),(dmaStruct),(offset))
#define SCSI_GET_LITTLE_ENDIAN64(hhcb,pValue,dmaStruct,offset)  \
            HIM_GET_LITTLE_ENDIAN64((hhcb)->osdHandle,(pValue),(dmaStruct),(offset))

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Customization unique to implementation                                 */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
typedef int             SCSI_INT;      /* signed int                       */
typedef unsigned int    SCSI_UINT;     /* unsigned int                     */
typedef long            SCSI_LONG;     /* signed long                      */
typedef unsigned long   SCSI_ULONG;    /* unsigned long                    */

typedef struct SCSI_SG_DESCRIPTOR_
{
    SCSI_BUS_ADDRESS busAddress;
    SCSI_UEXACT32    segmentLength;
} SCSI_SG_DESCRIPTOR;

/***************************************************************************
* Definitions for hardware register access
***************************************************************************/
#define SCSI_REGISTER               SCSI_IO_HANDLE HIM_PTR
#define SCSI_AICREG(a)              (a)
#define OSD_INEXACT8(reg) (HIM_UEXACT8) scsiRegister->OSMReadUExact8(scsiRegister->ioHandle,(HIM_UINT32)(reg))
#define OSD_OUTEXACT8(reg,value) scsiRegister->OSMWriteUExact8(scsiRegister->ioHandle,(HIM_UINT32)(reg),(HIM_UEXACT8)(value))
#define OSD_SYNCHRONIZE_IOS(hhcb)   if ((hhcb)->scsiRegister->OSMSynchronizeRange)\
                                    {\
                                       (hhcb)->scsiRegister->OSMSynchronizeRange((hhcb)->scsiRegister->ioHandle,(HIM_UINT32)0,(HIM_UINT32)256);\
                                    }

/***************************************************************************
* Definitions for OSM support routines
***************************************************************************/
#define  OSD_GET_BUS_ADDRESS(hhcb,category,virtualAddress) \
                  OSDGetBusAddress((hhcb),(category),(virtualAddress))
#define  OSD_ADJUST_BUS_ADDRESS(hhcb,busAddress,value) \
                  OSDAdjustBusAddress((hhcb),(busAddress),(value))
#define  OSD_READ_PCICFG_EXACT32(hhcb,registerNumber) \
                  OSDReadPciConfiguration((hhcb),(registerNumber))
#define  OSD_WRITE_PCICFG_EXACT32(hhcb,registerNumber,value) \
                  OSDWritePciConfiguration((hhcb),(registerNumber),(value))
#define  OSD_COMPLETE_HIOB(hiob) OSDCompleteHIOB((hiob))
#define  OSD_COMPLETE_SPECIAL_HIOB(hiob) OSDCompleteSpecialHIOB((hiob))
#define  OSD_ASYNC_EVENT(hhcb,event) OSDAsyncEvent((hhcb),(event))
#define  OSD_BUILD_SCB(hiob) OSDBuildSCB((hiob))
#define  OSD_GET_SG_LIST(hiob) OSDGetSGList((hiob))
#define  OSD_GET_HOST_ADDRESS(hhcb) OSDGetHostAddress((hhcb))
#define  OSD_TIMER(hhcb) OSDTimer((hhcb))

/* NSX Repeater routines */
#define  OSD_GET_NSX_BUFFER(hiob) OSDGetNSXBuffer((hiob))
#define  OSD_RETURN_NSX_STATUS(hiob,status,count) OSDReturnNSXStatus((hiob),(status),(count));


/***************************************************************************
* Definitions for Target Mode OSM support routines
***************************************************************************/
#define  OSD_BUILD_TARGET_SCB(hiob) OSDBuildTargetSCB((hiob))
#define  OSD_ALLOCATE_NODE(hhcb) OSDAllocateNODE((hhcb))
#define  OSD_GET_FREE_EST_HEAD(hhcb) OSDGetFreeEstHead((hhcb))

/***************************************************************************
* Prototype for functions defined by OSM
***************************************************************************/
SCSI_BUS_ADDRESS OSDGetBusAddress(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,void SCSI_HPTR);
void OSDTimer(SCSI_HHCB SCSI_HPTR);
void OSDAdjustBusAddress(SCSI_HHCB SCSI_HPTR,SCSI_BUS_ADDRESS SCSI_HPTR,SCSI_UINT);
SCSI_UEXACT32 OSDReadPciConfiguration(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void OSDWritePciConfiguration(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT32);
void OSDCompleteHIOB(SCSI_HIOB SCSI_IPTR);
void OSDCompleteSpecialHIOB(SCSI_HIOB SCSI_IPTR);
void OSDAsyncEvent(SCSI_HHCB SCSI_HPTR,SCSI_UINT16,...);
void OSDBuildSCB(SCSI_HIOB SCSI_IPTR);
SCSI_BUFFER_DESCRIPTOR HIM_PTR OSDGetSGList (SCSI_HIOB SCSI_IPTR);
SCSI_HOST_ADDRESS SCSI_LPTR OSDGetHostAddress(SCSI_HHCB SCSI_HPTR hhcb);
void OSDSynchronizeIOs(SCSI_HHCB SCSI_HPTR);
SCSI_BUFFER_DESCRIPTOR SCSI_LPTR OSDGetNSXBuffer(SCSI_HIOB HIM_PTR);
void OSDReturnNSXStatus(SCSI_HIOB HIM_PTR,SCSI_UEXACT8,SCSI_UINT16);

/***************************************************************************
* Prototype for Target Mode functions defined by OSM
***************************************************************************/
SCSI_SCB_DESCRIPTOR SCSI_HPTR OSDGetFreeHead(SCSI_HHCB SCSI_HPTR);
void OSDBuildTargetSCB(SCSI_HIOB SCSI_IPTR);
SCSI_NODE SCSI_NPTR OSDAllocateNODE(SCSI_HHCB SCSI_HPTR);

/*$Header:   Y:/source/chimscsi/src/SCSIOSM.HVT   1.14.8.8   17 Apr 1998 10:20:38   lin1239  $*/
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
*  Module Name:   SCSIOSM.H
*
*  Description:   SCSI implementation specific customization
*
*  Owners:        ECX IC Firmware Team
*
***************************************************************************/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Customization unique to implementation */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
#define SCSI_RESOURCE_MANAGEMENT    1        /* enable resource management */
#define SCSI_SWAPPING32_MODE        1        /* enable swapping 32 mode */
#define SCSI_SWAPPING64_MODE        0        /* enable swapping 64 mode */
#define SCSI_STANDARD32_MODE        0        /* enable standard 32 mode */
#define SCSI_STANDARD64_MODE        1        /* enable standard 64 mode */
#define SCSI_STANDARD_ADVANCED_MODE 1        /* enable standard bayonet mode */
#define SCSI_SWAPPING_ADVANCED_MODE 1        /* enable swapping bayonet mode */
#define SCSI_STANDARD_160M_MODE     1        /* enable standard trident mode */
#define SCSI_SWAPPING_160M_MODE     1        /* enable swapping trident mode */
#define SCSI_DOWNSHIFT_MODE         0        /* enable diwn shift mode */
#define SCSI_GROUP_DEVICES          0        /* enable group devices for raid */
#define SCSI_SCAM_ENABLE            0        /* scam level support 0, 1, 2 */
#define SCSI_AUTO_TERMINATION       1        /* auto termination en/disable   */
#define SCSI_UPDATE_TERMINATION_ENABLE  1    /* update termination setting en/disable   */
#define SCSI_CUSTOM_TERMINATION     0        /* custom termination code for an internal 
                                              * customer
                                              */ 
#define SCSI_SCBBFR_BUILTIN         1        /* scb buffer built in hardware management */
#define SCSI_AIC785X                1        /* support Dagger type devices */
#define SCSI_AIC786X                1        /* support talon type device     */
#define SCSI_AIC787X                1        /* support Lance type device */
#define SCSI_AIC788X                1        /* support Katana type device */
#define SCSI_AIC789X                1        /* support Excalibur type device */
#define SCSI_AICBAYONET             1        /* support Bayonet type device */
#define SCSI_AICTRIDENT             1        /* support Trident type device */
#define SCSI_PPR_ENABLE             1        /* support Parallel Protocol Request */
#define SCSI_SEEPROM_ACCESS         1        /* support SEEPROM access     */
#define SCSI_BIOS_ASPI8DOS          1        /* access to scratch ram that BIOS/ASPI8DOS have */
#define SCSI_SCSISELECT_SUPPORT     1        /* SCSISelect information existing in SEEPROM */
#define SCSI_TARGET_OPERATION       0        /* target operation mode */
#define SCSI_INITIATOR_OPERATION    1        /* initiator operation mode */
#define SCSI_SAVE_RESTORE_STATE     1        /* save/restore state */
#define SCSI_PROFILE_INFORMATION    1        /* support CHIM adapter and target profiles */
#define SCSI_DISABLE_ADJUST_TARGET_PROFILE   0 /* disable code associated with
                                                * HIMAdjustTargetProfile.
                                                */ 
#define SCSI_FORCE_STANDARD64_MODE  0        /* force s64 mode regardless off ext scb presence */
#define SCSI_BACKENDISR_OUTSIDE_INTRCONTEXT  0    /* defer HIMBackEndISR call outside of interrupt context */
#define SCSI_ARO_SUPPORT            0        /* support ARO design */
#define SCSI_4944IDFIX_ENABLE       0        /* enable 4944 id support for 783B and 78EC */
#define SCSI_STREAMLINE_QIOPATH     1        /* streamline the normal qiob/command complete path */
#define SCSI_IO_SYNCHRONIZATION_GUARANTEED   1  /* PIO's are guaranteed to be in order and not posted */
#define SCSI_PCI2_0_WORKAROUND      0        /* enable the fix for Bayonet/Mace DMA hang problem */
#define SCSI_DOMAIN_VALIDATION_BASIC    0     /* enable Basic Domain Validation */
#define SCSI_DOMAIN_VALIDATION_ENHANCED 0     /* enable Enhanced Domain Validation */

#define SCSI_PCI_COMMAND_REG_CHECK  1        /* enable checking I/O, Memory and Bus Master */
#define SCSI_RESET_DELAY_DEFAULT    2000     /* the default value for adapter profile field 
                                              * AP_ResetDelay in milliseconds when 
                                              * AP_InitiatorMode is HIM_TRUE. 
                                              */
#define SCSI_FAST_PAC  0                     /* skip SCSI bus scan to search for new targets during PAC */
#define SCSI_EXCALIBUR_DAC_WORKAROUND  1     /* Excalibur DAC problem workaround 0- no workaround, */
                                             /* 1- cachesize=0, 2- cachesize=0 && MRDCEN=1         */
#define SCSI_2930UW_SUPPORT         0        /* 0 - no support for 2930UW, 1 - support 2930UW */
#define SCSI_2930U2_SUPPORT         0        /* 0 - no support for 2930U2, 1 - support 2930U2 */
#define SCSI_2930CVAR_SUPPORT       0        /* 0 - no support for 2930CVAR, 1 - support 2930CVAR */

#define SCSI_CLEAR_PCI_ERROR        0        /* Able to clear PCI errors through config space */

#if SCSI_AICBAYONET && SCSI_CLEAR_PCI_ERROR 
#define SCSI_CHECK_PCI_ERROR        0        /* enable checking PCI target abort error which may come from Mace DRAM */
#endif
#define SCSI_NEGOTIATION_PER_IOB    0        /* enable/disable controling negotiation through IOB */
#define SCSI_PARITY_PER_IOB         0        /* enable/disable parity per iob  */   

#define SCSI_OEM1_SUPPORT           0        /* support OEM1 customer */

#define SCSI_NULL_SELECTION         0        /* When set to 1 the CHIM will issue
                                              * a SCSI NULL Selection (peforming
                                              * a selection using only the host
                                              * adapter ID) during a bus scan (PAC).
                                              * Also allows the scsiID field in the
                                              * transportSpecific area of the IOB
                                              * to be set to the host adapter SCSI ID.  
                                              */
#define SCSI_PAC_NSX_REPEATER       0        /* When set to 1 the CHIM will determine
                                              * if targets are connected through an NSX
                                              * repeater during PAC or HIM_PROBE of Lun 
                                              * 0 and adjust transfer rate to an 
                                              * optimal value.
                                              */
#define SCSI_NSX_REPEATER           0        /* When set to 1 enables OSM interfaces
                                              * to request NSX repeater information
                                              * via HIM_PROBE or HIM_INITIATE_TASK
                                              * IOB functions.
                                              */ 
#define SCSI_FOOT_BRIDGE            0        /* Enable swapping advanced
                                              * sequencer workaround to avoid
                                              * more than 32 byte bursts from
                                              * PCI to bridge. Only applicable
                                              * if
                                              * SCSI_SWAPPING_ADVANCED_SEQUENCER
                                              * = 1.
                                              */ 

#define SCSI_MAILBOX_ENABLE         0        /* Use Mailbox for Establish
                                              * Connection SCBs 
                                              */
#define SCSI_LOOPBACK_OPERATION     0        /* support loopback operation */
#define SCSI_ESS_SUPPORT            0        /* support ESS design */      
#define SCSI_MULTIPLEID_SUPPORT     0        /* support Multiple IDs for target
                                              * mode
                                              */ 
#define SCSI_TRIDENT_PROTO          0        /* To enable firmware workarounds*/
                                             /* for Trident prototype board */
 
#define SCSI_DISABLE_INTERRUPTS     0        /* Does not enable the HA interrupt */
                                             /* when SCSIInitialize() is called. */
#define SCSI_DISABLE_PROBE_SUPPORT  0        /* disable support of HIM_PROBE IOB */
                                             /* function */ 
#define SCSI_ESTABLISH_CONNECTION_SCBS  0    /* This field only has meaning when
                                              * SCSI_TARGET_OPERATION = 1 and 
                                              * target mode is enabled for an
                                              * adapter. This value represents
                                              * the default number of internal
                                              * I/O blocks(SCBS) reserved for
                                              * receiving new target mode 
                                              * requests from initiators.
                                              * If this value is 0 an adapter
                                              * specific default will be used.   
                                              */ 
/* check operating modes validity */
#if (SCSI_STANDARD32_MODE + SCSI_SWAPPING32_MODE) && OSD_BUS_ADDRESS_SIZE == 64
   ****** 64 bits address cannot be enabled for Swapping 32 or standard 32 modes ******
#endif

#define POWER_MANAGEMENT_SAVE_RESTORE  0      /* save and restore for Power Management */

/*$Header:   Y:/source/chimscsi/src/himscsi/hwmscsi/HARDEQU.HV_   1.34.6.3   09 Apr 1998 10:49:16   lin1239  $*/
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
*  Module Name:   HARDEQU.H
*
*  Description:   Definitions for hardware device register 
*    
*  Owners:        ECX IC Firmware Team
*    
*  Notes:         
*                 
***************************************************************************/

#define  SCSI_AIC78XX (SCSI_AIC785X + SCSI_AIC786X + SCSI_AIC787X +\
                       SCSI_AIC788X + SCSI_AIC789X)

#define  SCSI_MULTI_HARDWARE (SCSI_AIC78XX + SCSI_AICBAYONET +\
                              SCSI_AICTRIDENT) > 1

/****************************************************************************

 Equates and definitions to be used exclusively by the Lance Driver modules

****************************************************************************/

#define  SCSI_NARROW_OFFSET  15       /* maximum 8-bit sync transfer offset     */
#define  SCSI_WIDE_OFFSET    8        /* maximum 16-bit sync transfer offset    */
#define  SCSI_WIDE_WIDTH     1        /* maximum tarnsfer width = 16 bits       */
#if SCSI_TARGET_OPERATION       
#define  SCSI_FAST20_THRESHOLD  0x12  /* fast 20 threshold                      */
#else
#define  SCSI_FAST20_THRESHOLD  0x19  /* Any smaller will be double speed dev   */
#endif  /* SCSI_TARGET_OPERATION */

/****************************************************************************
*  Definitions for message protocol
****************************************************************************/
#define  SCSI_MSG00          0x00     /*   - command complete                   */
#define  SCSI_MSG01          0x01     /*   - extended message                   */
#define  SCSI_MSGMDP         0x00     /*       - modify data pointer msg        */
#define  SCSI_MSGSYNC        0x01     /*       - synchronous data transfer msg  */
#define  SCSI_MSGWIDE        0x03     /*       - wide data transfer msg         */
#define  SCSI_MSGPPR         0x04     /*       - parallel protocol request msg  */
#define  SCSI_MSG02          0x02     /*   - save data pointers                 */
#define  SCSI_MSG03          0x03     /*   - restore data pointers              */
#define  SCSI_MSG04          0x04     /*   - disconnect                         */
#define  SCSI_MSG05          0x05     /*   - initiator detected error           */
#define  SCSI_MSG06          0x06     /*   - abort                              */
#define  SCSI_MSG07          0x07     /*   - message reject                     */
#define  SCSI_MSG08          0x08     /*   - nop                                */
#define  SCSI_MSG09          0x09     /*   - message parity error               */
#define  SCSI_MSG0A          0x0a     /*   - linked command complete            */
#define  SCSI_MSG0B          0x0b     /*   - linked command complete            */
#define  SCSI_MSG0C          0x0c     /*   - bus device reset                   */
#define  SCSI_MSG0D          0x0d     /*   - abort tag                          */
#define  SCSI_MSGTAG         0x20     /*   - tag queuing                        */
#define  SCSI_MSG23          0x23     /*   - ignore wide residue                */
#define  SCSI_MSGID          0x80     /* identify message, no disconnection     */
#define  SCSI_MSGID_D        0xc0     /* identify message, disconnection        */

/****************************************************************************

 LANCE SCSI REGISTERS DEFINED AS A STRUCTURE

****************************************************************************/

#define  SCSI_SCSISEQ        0x00     /* scsi sequence control     (read/write) */

#define     SCSI_TEMODEO     0x80     /* target enable mode out                 */
#define     SCSI_ENSELO      0x40     /* enable selection out                   */
#define     SCSI_ENSELI      0x20     /* enable selection in                    */
#define     SCSI_ENRSELI     0x10     /* enable reselection in                  */
#define     SCSI_ENAUTOATNO  0x08     /* enable auto attention out              */
#define     SCSI_ENAUTOATNI  0x04     /* enable auto attention in               */
#define     SCSI_ENAUTOATNP  0x02     /* enable auto attention parity           */
#define     SCSI_SCSIRSTO    0x01     /* scsi reset out                         */


#define  SCSI_SXFRCTL0       0x01     /* scsi transfer control 0   (read/write) */

#define     SCSI_DFON        0x80     /* Digital Filter On                      */
#define     SCSI_DFPEXP      0x40     /* Digital Filter Period                  */
#define     SCSI_FAST20      0x20     /* Fast20 hardware enabled                */
#define     SCSI_CLRSTCNT    0x10     /* clear Scsi Transfer Counter            */
#define     SCSI_SPIOEN      0x08     /* enable auto scsi pio                   */
#define     SCSI_SCAMEN      0x04     /* Enable SCAM protocol                   */
#define     SCSI_CLRCHN      0x02     /* clear Channel n                        */


#define  SCSI_SXFRCTL1       0x02     /* scsi transfer control 1   (read/write) */
                                 
#define     SCSI_BITBUCKET   0x80     /* enable bit bucket mode                 */
#define     SCSI_SWRAPEN     0x40     /* enable wrap-around                     */
#define     SCSI_ENSPCHK     0x20     /* enable scsi parity checking            */
#define     SCSI_STIMESEL1   0x10     /* select selection timeout:  00 - 256 ms,*/
#define     SCSI_STIMESEL0   0x08     /* 01 - 128 ms, 10 - 64 ms, 11 - 32 ms    */
#define     SCSI_ENSTIMER    0x04     /* enable selection timer                 */
#define     SCSI_ACTNEGEN    0x02     /* enable active negation                 */
#define     SCSI_STPWEN      0x01     /* enable termination power               */
#define     SCSI_STIMESEL    (SCSI_STIMESEL0 | SCSI_STIMESEL1)
            
#define  SCSI_SCSISIG        0x03     /* actual scsi bus signals   (write/read) */

#define     SCSI_CDI         0x80     /* c/d                                    */
#define     SCSI_IOI         0x40     /* i/o                                    */
#define     SCSI_MSGI        0x20     /* msg                                    */
#define     SCSI_ATNI        0x10     /* atn                                    */
#define     SCSI_SELI        0x08     /* sel                                    */
#define     SCSI_BSYI        0x04     /* bsy                                    */
#define     SCSI_REQI        0x02     /* req                                    */
#define     SCSI_ACKI        0x01     /* ack                                    */

#define     SCSI_CDO         0x80     /* expected c/d  (initiator mode)         */
#define     SCSI_IOO         0x40     /* expected i/o  (initiator mode)         */
#define     SCSI_MSGO        0x20     /* expected msg  (initiator mode)         */
#define     SCSI_ATNO        0x10     /* set atn                                */
#define     SCSI_SELO        0x08     /* set sel                                */
#define     SCSI_BSYO        0x04     /* set busy                               */
#define     SCSI_REQO        0x02     /* not functional in initiator mode       */
#define     SCSI_ACKO        0x01     /* set ack                                */

#define     SCSI_BUSPHASE    (SCSI_CDO+SCSI_IOO+SCSI_MSGO) /* scsi bus phase -  */
#define     SCSI_DOPHASE     0x00           /*  data out                        */
#define     SCSI_DIPHASE     SCSI_IOO       /*  data in                         */
#define     SCSI_CMDPHASE    SCSI_CDO       /*  command                         */
#define     SCSI_MIPHASE     (SCSI_CDO+SCSI_IOO+SCSI_MSGO) /*  message in       */
#define     SCSI_MOPHASE     (SCSI_CDO+SCSI_MSGO)     /*  message out           */
#define     SCSI_STPHASE     (SCSI_CDO+SCSI_IOO)      /*  status                */
#define     SCSI_ERRPHASE    0xff                     /* error                  */

#define  SCSI_SCSIRATE       0x04     /* scsi rate control      (write)         */

#define     SCSI_WIDEXFER    0x80     /* ch 0 wide scsi bus                     */
#define     SCSI_SXFR2       0x40     /* synchronous scsi transfer rate         */
#define     SCSI_SXFR1       0x20     /*                                        */
#define     SCSI_SXFR0       0x10     /*                                        */
#define     SCSI_SXFR        (SCSI_SXFR2+SCSI_SXFR1+SCSI_SXFR0)     /*          */
#define     SCSI_SOFS3       0x08     /* synchronous scsi offset                */
#define     SCSI_SOFS2       0x04     /*                                        */
#define     SCSI_SOFS1       0x02     /*                                        */
#define     SCSI_SOFS0       0x01     /*                                        */
#define     SCSI_SOFS        (SCSI_SOFS3+SCSI_SOFS2+SCSI_SOFS1+SCSI_SOFS0)
#define     SCSI_SYNC_MODE   0x01     /* synchronous xfer mode                  */

#define  SCSI_SCSIID         0x05     /* scsi id       (read/write)             */

#define     SCSI_TID3        0x80     /* other scsi device id                   */
#define     SCSI_TID2        0x40     /*                                        */
#define     SCSI_TID1        0x20     /*                                        */
#define     SCSI_TID0        0x10     /*                                        */
#define     SCSI_OID3        0x08     /* my id                                  */
#define     SCSI_OID2        0x04     /*                                        */
#define     SCSI_OID1        0x02     /*                                        */
#define     SCSI_OID0        0x01     /*                                        */
#define     SCSI_TID         (SCSI_TID3+SCSI_TID2+SCSI_TID1+SCSI_TID0)   /* scsi device id mask     */
#define     SCSI_OID         (SCSI_OID3+SCSI_OID2+SCSI_OID1+SCSI_OID0)   /* our scsi device id mask */

#define  SCSI_SCSIDATL       0x06     /* scsi latched data, lo     (read/write) */
#define  SCSI_SCSIDATH       0x07     /* scsi latched data, hi     (read/write) */

#define  SCSI_STCNT0         0x08     /* scsi transfer count, lsb  (read/write) */
#define  SCSI_STCNT1         0x09     /*                    , mid  (read/write) */
#define  SCSI_STCNT2         0x0a     /*                    , msb  (read/write) */
         
#define  SCSI_CLRSINT0       0x0b     /* clear scsi interrupts 0   (write)      */

#define     SCSI_CLRSELDO    0x40     /* clear seldo interrupt & status         */
#define     SCSI_CLRSELDI    0x20     /* clear seldi interrupt & status         */
#define     SCSI_CLRSELINGO  0x10     /* clear selingo interrupt & status       */
#define     SCSI_CLRSWRAP    0x08     /* clear swrap interrupt & status         */
#define     SCSI_BAYONET_CLRIOERR   0x08  /* clear ioerr int & status (bayonet only) */
#define     SCSI_CLRSDONE    0x04     /* clear sdone interrupt & status         */
#define     SCSI_CLRSPIORDY  0x02     /* clear spiordy interrupt & status       */
#define     SCSI_CLRDMADONE  0x01     /* clear dmadone interrupt & status       */


#define  SCSI_SSTAT0         0x0b     /* scsi status 0       (read)             */

#define     SCSI_TARGET      0x80     /* mode = target                          */
#define     SCSI_SELDO       0x40     /* selection out completed                */
#define     SCSI_SELDI       0x20     /* have been reselected                   */
#define     SCSI_SELINGO     0x10     /* arbitration won, selection started     */
#define     SCSI_SWRAP       0x08     /* transfer counter has wrapped around    */
#define     SCSI_BAYONET_IOERR   0x08 /* ioerr status (bayonet only)            */
#define     SCSI_SDONE       0x04     /* not used in mode = initiator           */
#define     SCSI_SPIORDY     0x02     /* auto pio enabled & ready to xfer data  */
#define     SCSI_DMADONE     0x01     /* transfer completely done               */


#define  SCSI_CLRSINT1       0x0c     /* clear scsi interrupts 1   (write)      */

#define     SCSI_CLRSELTIMO  0x80     /* clear selto interrupt & status         */
#define     SCSI_CLRATNO     0x40     /* clear atno control signal              */
#define     SCSI_CLRSCSIRSTI 0x20     /* clear scsirsti interrupt & status      */
#define     SCSI_CLRBUSFREE  0x08     /* clear busfree interrupt & status       */
#define     SCSI_CLRSCSIPERR 0x04     /* clear scsiperr interrupt & status      */
#define     SCSI_CLRPHASECHG 0x02     /* clear phasechg interrupt & status      */
#define     SCSI_CLRREQINIT  0x01     /* clear reqinit interrupt & status       */


#define  SCSI_SSTAT1         0x0c     /* scsi status 1       (read)             */

#define     SCSI_SELTO       0x80     /* selection timeout                      */
#define     SCSI_ATNTARG     0x40     /* mode = target:  initiator set atn      */
#define     SCSI_SCSIRSTI    0x20     /* other device asserted scsi reset       */
#define     SCSI_PHASEMIS    0x10     /* actual scsi phase <> expected          */
#define     SCSI_BUSFREE     0x08     /* bus free occurred                      */
#define     SCSI_SCSIPERR    0x04     /* scsi parity error                      */
#define     SCSI_PHASECHG    0x02     /* scsi phase change                      */
#define     SCSI_REQINIT     0x01     /* latched req                            */


#define  SCSI_SSTAT2         0x0d     /* scsi status 2       (read)             */

#define     SCSI_SFCNT4      0x10     /* scsi fifo count                        */
#define     SCSI_SFCNT3      0x08     /*                                        */
#define     SCSI_SFCNT2      0x04     /*                                        */
#define     SCSI_SFCNT1      0x02     /*                                        */
#define     SCSI_SFCNT0      0x01     /*                                        */
#define     SCSI_BAYONET_OVERRUN    0x80  /* overrun                            */
#define     SCSI_BAYONET_SHVALID    0x40  /* shadow valid                       */
#define     SCSI_BAYONET_WIDE_RES   0x20  /* wide residue                       */
#define     SCSI_BAYONET_EXP_ACTIVE 0x10  /* expander active                    */
#define     SCSI_AIC78XX_EXPACTIVE_DEFAULT 0 /* default aic78xx expactive       */
#define     SCSI_TRIDENT_CRCVALERR  0x08  /* CRC value error                    */
#define     SCSI_TRIDENT_CRCENDERR  0x04  /* CRC end error                      */
#define     SCSI_TRIDENT_CRCREQERR  0x02  /* CRC REG error                      */
#define     SCSI_TRIDENT_DUAL_EDGE_ERR 0x01  /* Dual-Edge phase error           */

#define  SCSI_SSTAT3         0x0e     /* scsi status 3       (read)             */

#define     SCSI_SCSICNT3    0x80     /*                                        */
#define     SCSI_SCSICNT2    0x40     /*                                        */
#define     SCSI_SCSICNT1    0x20     /*                                        */
#define     SCSI_SCSICNT0    0x10     /*                                        */
#define     SCSI_OFFCNT3     0x08     /* current scsi offset count              */
#define     SCSI_OFFCNT2     0x04     /*                                        */
#define     SCSI_OFFCNT1     0x02     /*                                        */
#define     SCSI_OFFCNT0     0x01     /*                                        */
            

#define  SCSI_SCSITEST       0x0f     /* scsi test control   (read/write)       */

#define     SCSI_CNTRTEST    0x02     /*                                        */
#define     SCSI_CMODE       0x01     /*                                        */


#define  SCSI_SIMODE0        0x10     /* scsi interrupt mask 0     (read/write) */

#define     SCSI_ENSELDO     0x40     /* enable seldo status to assert int      */
#define     SCSI_ENSELDI     0x20     /* enable seldi status to assert int      */
#define     SCSI_ENSELINGO   0x10     /* enable selingo status to assert int    */
#define     SCSI_ENSWRAP     0x08     /* enable swrap status to assert int      */
#define     SCSI_BAYONET_ENIOERR 0x08 /* enable ioerr status to assert int      */
#define     SCSI_ENSDONE     0x04     /* enable sdone status to assert int      */
#define     SCSI_ENSPIORDY   0x02     /* enable spiordy status to assert int    */
#define     SCSI_ENDMADONE   0x01     /* enable dmadone status to assert int    */


#define  SCSI_SIMODE1        0x11     /* scsi interrupt mask 1     (read/write) */

#define     SCSI_ENSELTIMO   0x80     /* enable selto status to assert int      */
#define     SCSI_ENATNTARG   0x40     /* enable atntarg status to assert int    */
#define     SCSI_ENSCSIRST   0x20     /* enable scsirst status to assert int    */
#define     SCSI_ENPHASEMIS  0x10     /* enable phasemis status to assert int   */
#define     SCSI_ENBUSFREE   0x08     /* enable busfree status to assert int    */
#define     SCSI_ENSCSIPERR  0x04     /* enable scsiperr status to assert int   */
#define     SCSI_ENPHASECHG  0x02     /* enable phasechg status to assert int   */
#define     SCSI_ENREQINIT   0x01     /* enable reqinit status to assert int    */


#define  SCSI_SCSIBUSL       0x12     /* scsi data bus, lo  direct (read)       */

#define  SCSI_SCSIBUSH       0x13     /* scsi data bus, hi  direct (read)       */

#define  SCSI_SHADDR0        0x14     /* scsi/host address      (read)          */
#define  SCSI_SHADDR1        0x15     /*                                        */
#define  SCSI_SHADDR2        0x16     /* host address incremented by scsi ack   */
#define  SCSI_SHADDR3        0x17     /*                                        */

#define  SCSI_TARGIDIN       0x18     /* target ID in              (read)       */

#define     SCSI_CLKOUT      0x80     /* clock out provides 102.4 us period     */

#define  SCSI_SELID          0x19     /* selection/reselection id  (read)       */

#define     SCSI_SELID3      0x80     /* binary id of other device              */
#define     SCSI_SELID2      0x40     /*                                        */
#define     SCSI_SELID1      0x20     /*                                        */
#define     SCSI_SELID0      0x10     /*                                        */
#define     SCSI_ONEBIT      0x08     /* non-arbitrating selection detection    */

#define  SCSI_SCAMCTRL       0x1a     /* scam control         (write)           */

#define  SCSI_ENSCAMSELO     0x80     /* scam selection enable out              */
#define  SCSI_CLRSCAMSELD    0x40     /* clear scam selection done              */
#define  SCSI_ALTSTIM        0x20     /* alternate selection time-out           */
#define  SCSI_DFLTID         0x10     /* default scsi id                        */
                                      /* bits 2-3 are reserved                  */
#define  SCSI_SCAMLVL2       0x02     /* scam level 2                           */
#define  SCSI_SCAMLVL1       0x01     /* scam level 1                           */
#define  SCSI_SCAMLVL     (SCSI_SCAMLVL1 + SCSI_SCAMLVL2)  /* scam level mask   */

#define  SCSI_SCAMSTAT       0x1a     /* scam status          (read)            */

#define  SCSI_SCAMSELOSTA    0x80     /* scam selection out status              */
#define  SCSI_SCAMSELD       0x40     /* scam selection done                    */
#define  SCSI_ALTSTIMSTA     0x20     /* alternate selection time-out status    */
#define  SCSI_DFLTIDSTA      0x10     /* default scsi id status                 */
                                      /* bits 2-3 are reserved                  */
#define  SCSI_SCAMLVL2STA    0x02     /* scam level 2                           */
#define  SCSI_SCAMLVL1STA    0x01     /* scam level 1                           */
#define  SCSI_SCAMLVLSTA  (SCAMLVL1STA+SCAMLVL2STA)  /* scam level status       */

#define  SCSI_BRDCTL         0x1d

#define     SCSI_BRDDAT7     0x80     /* data bit 2                             */
#define     SCSI_BRDDAT6     0x40     /* data bit 1                             */
#define     SCSI_BRDDAT5     0x20     /* data bit 0                             */
#define     SCSI_BRDSTB      0x10     /* board strobe                           */
#define     SCSI_BRDCS       0x08     /* board chip select                      */
#define     SCSI_BRDRW       0x04     /* board read/write                       */

                                      /* BRDCTL definition for Bayonet          */
#define     SCSI_BAYONET_BRDDAT7 0x80 /* data bit 5                             */
#define     SCSI_BAYONET_BRDDAT6 0x40 /* data bit 4                             */
#define     SCSI_BAYONET_BRDDAT5 0x20 /* data bit 3                             */
#define     SCSI_BAYONET_BRDDAT4 0x10 /* data bit 2                             */
#define     SCSI_BAYONET_BRDDAT3 0x08 /* data bit 1                             */
#define     SCSI_BAYONET_BRDDAT2 0x04 /* data bit 0                             */
#define     SCSI_BAYONET_BRDRW   0x02 /* board read/write                       */
#define     SCSI_BAYONET_BRDSTB  0x01 /* board strobe                           */

#define  SCSI_SEEPROM        0x1e     /* Serial EEPROM (read/write)             */

#define     SCSI_EXTARBACK   0x80     /* external arbitration acknowledge       */
#define     SCSI_EXTARBREQ   0x40     /* external arbitration request           */
#define     SCSI_SEEMS       0x20     /* serial EEPROM mode select              */
#define     SCSI_SEERDY      0x10     /* serial EEPROM ready                    */
#define     SCSI_SEECS       0x08     /* serial EEPROM chip select              */
#define     SCSI_SEECK       0x04     /* serial EEPROM clock                    */
#define     SCSI_SEEDO       0x02     /* serial EEPROM data out                 */
#define     SCSI_SEEDI       0x01     /* serial EEPROM data in                  */
            
#define  SCSI_SBLKCTL        0x1f     /* scsi block control     (read/write)    */

#define     SCSI_DIAGLEDEN      0x80  /* diagnostic led enable                  */
#define     SCSI_DIAGLEDON      0x40  /* diagnostic led on                      */
#define     SCSI_AUTOFLUSHDIS   0x20  /* disable automatic flush                */
#define     SCSI_SELBUS1        0x08  /* select scsi channel 1                  */
#define     SCSI_SELWIDE        0x02  /* scsi wide hardware configure           */

#define     SCSI_BAYONET_ENAB40 0x08  /* enable 40 Mtransfer Mode               */
#define     SCSI_BAYONET_ENAB20 0x04  /* enable 20 Mtransfer Mode               */
#define     SCSI_BAYONET_XCVR   0x01  /* external transceiver                   */

/****************************************************************************

 LANCE SEQUENCER REGISTERS

****************************************************************************/

#define  SCSI_SEQCTL         0x60     /* sequencer control      (read/write)    */

#define     SCSI_PERRORDIS   0x80     /* enable sequencer parity errors         */
#define     SCSI_PAUSEDIS    0x40     /* disable pause by driver                */
#define     SCSI_FAILDIS     0x20     /* disable illegal opcode/address int     */
#define     SCSI_FASTMODE    0x10     /* sequencer clock select                 */
#define     SCSI_BRKINTEN    0x08     /* break point interrupt enable           */
#define     SCSI_STEP        0x04     /* single step sequencer program          */
#define     SCSI_LOADRAM     0x01     /* enable sequencer ram loading           */

#define     SCSI_AIC78XX_SEQCODE_SIZE  2048 /* aic-78xx sequencer ram size      */
#define     SCSI_BAYONET_SEQCODE_SIZE  3072 /* bayonet sequencer ram size       */
#define     SCSI_TRIDENT_SEQCODE_SIZE  4096 /* trident sequencer ram size       */

#define  SCSI_SEQRAM         0x61     /* sequencer ram data        (read/write) */

#define  SCSI_SEQADDR0       0x62     /* sequencer address 0       (read/write) */

#define  SCSI_SEQADDR1       0x63     /* sequencer address 1       (read/write) */

#define  SCSI_ACCUM          0x64     /* accumulator               (read/write) */

#define  SCSI_SINDEX         0x65     /* source index register     (read/write) */

#define  SCSI_DINDEX         0x66     /* destination index register(read/write) */

#define  SCSI_BRKADDR0       0x67     /* break address, lo         (read/write) */

#define  SCSI_BRKADDR1       0x68     /* break address, hi         (read/write) */

#define     SCSI_BRKDIS      0x80     /* disable breakpoint                     */
#define     SCSI_BRKADDR08   0x01     /* breakpoint addr, bit 8                 */


#define  SCSI_ALLONES        0x69     /* all ones, src reg = 0ffh  (read)       */

#define  SCSI_ALLZEROS       0x6a     /* all zeros, src reg = 00h  (read)       */

#define  SCSI_NONE           0x6a     /* no destination, No reg altered (write) */

#define  SCSI_FLAGS          0x6b     /* flags            (read)                */

#define     SCSI_ZERO        0x02     /* sequencer 'zero' flag                  */
#define     SCSI_CARRY       0x01     /* sequencer 'carry' flag                 */


#define  SCSI_SINDIR         0x6c     /* source index reg, indirect(read)       */

#define  SCSI_DINDIR         0x6d     /* destination index reg, indirect(read)  */

#define  SCSI_FUNCTION1      0x6e     /* funct: bits 6-4 -> 1-of-8 (read/write) */

#define  SCSI_STACK          0x6f     /* stack, for subroutine returns  (read)  */


/****************************************************************************

 LANCE HOST REGISTERS

****************************************************************************/

#define  SCSI_VENDID0        0x80     /* PCI id                    (read/write) */
#define     SCSI_HA_ID_HI    0x04

#define  SCSI_VENDID1        0x81     /* PCI id                    (read/write) */
#define     SCSI_HA_ID_LO    0x90

#define  SCSI_DEVID0         0x82     /* PCI id                    (read/write) */
#define     SCSI_HA_PROD_HI  0x78

#define  SCSI_DEVID1         0x83     /* PCI id                    (read/write) */
#define     SCSI_HA_PROD_LO  0x70

#define  SCSI_COMMAND        0x84     /* PCI command               (read/write) */

#define     SCSI_CACHETHEN   0x80     /*                                        */
#define     SCSI_DPARCKEN    0x40     /*                                        */
#define     SCSI_MPARCKEN    0x20     /*                                        */
#define     SCSI_EXTREQLCK   0x10     /*                                        */
#define     SCSI_DSSERRESPEN 0x08     /* (read only)                            */
#define     SCSI_DSPERRESPEN 0x04     /* (read only)                            */
#define     SCSI_DSMWRICEN   0x02     /* (read only)                            */
#define     SCSI_DSMASTEREN  0x01     /* (read only)                            */
#define     SCSI_AIC78XX_CACHETHEEN_DEFAULT 0 /* default aic78xx cachethen      */

#define     SCSI_AIC78XX_MAX_SYNC_PERIOD 68 /* max xfer period to go sync       */
#define  SCSI_LATTIME        0x85     /* latency timer             (read/write) */

#define     SCSI_LATT7       0x80     /* latency timer time        (read)       */
#define     SCSI_LATT6       0x40     /*                                        */
#define     SCSI_LATT5       0x20     /*                                        */
#define     SCSI_LATT4       0x10     /*                                        */
#define     SCSI_LATT3       0x08     /*                                        */
#define     SCSI_LATT2       0x04     /*                                        */
#define     SCSI_HADDLDSEL1  0x02     /* host address load select               */
#define     SCSI_HADDLDSEL0  0x01     /*                                        */


#define  SCSI_PCISTATUS      0x86     /* PCI status                (read/write) */

#define     SCSI_DFTHRSH1    0x80     /* data fifo threshold control            */
#define     SCSI_DFTHRSH0    0x40     /*                                        */
#define     SCSI_DSDPR       0x20     /*                                        */
#define     SCSI_DSDPE       0x10     /*                                        */
#define     SCSI_DSSSE       0x08     /*                                        */
#define     SCSI_DSRMA       0x04     /*                                        */
#define     SCSI_DSRTA       0x02     /*                                        */
#define     SCSI_DSSTA       0x01     /*                                        */


#define  SCSI_HCNTRL         0x87     /* host control              (read/write) */

#define     SCSI_POWRDN      0x40     /* power down                             */
#define     SCSI_BANKSEL     0x20     /* scratch bank select                    */
#define     SCSI_SWINT       0x10     /* force interrupt                        */
#define     SCSI_IRQMS       0x08     /* 0 = high true edge, 1 = low true level */
#define     SCSI_PAUSE       0x04     /* pause sequencer            (write)     */
#define     SCSI_PAUSEACK    0x04     /* sequencer is paused        (read)      */
#define     SCSI_INTEN       0x02     /* enable hardware interrupt              */
#define     SCSI_CHIPRESET   0x01     /* device hard reset                      */


#define  SCSI_HADDR0         0x88     /* host address 0            (read/write) */
#define  SCSI_HADDR1         0x89     /* host address 1            (read/write) */
#define  SCSI_HADDR2         0x8a     /* host address 2            (read/write) */
#define  SCSI_HADDR3         0x8b     /* host address 3            (read/write) */

#define  SCSI_HCNT0          0x8c     /* host count 0              (read/write) */
#define  SCSI_HCNT1          0x8d     /* host count 1              (read/write) */
#define  SCSI_HCNT2          0x8e     /* host count 2              (read/write) */

#define  SCSI_SCBPTR         0x90     /* scb pointer               (read/write) */

#define     SCSI_SCBVAL2     0x04     /* label of element in scb array          */
#define     SCSI_SCBVAL1     0x02     /*                                        */
#define     SCSI_SCBVAL0     0x01     /*                                        */


#define  SCSI_INTSTAT        0x91     /* interrupt status          (read)       */

#define     SCSI_INTCODE3    0x80     /* seqint interrupt code                  */
#define     SCSI_INTCODE2    0x40     /*                                        */
#define     SCSI_INTCODE1    0x20     /*                                        */
#define     SCSI_INTCODE0    0x10     /*                                        */
#define     SCSI_BRKINT      0x08     /* program count = breakpoint             */
#define     SCSI_SCSIINT     0x04     /* scsi event interrupt                   */
#define     SCSI_CMDCMPLT    0x02     /* scb command complete w/ no error       */
#define     SCSI_SEQINT      0x01     /* sequencer paused itself                */
#define     SCSI_INTCODE     (SCSI_INTCODE3+SCSI_INTCODE2+SCSI_INTCODE1+SCSI_INTCODE0) /* intr code */
#define     SCSI_ANYINT      (SCSI_BRKINT+SCSI_SCSIINT+SCSI_CMDCMPLT+SCSI_SEQINT) /* any interrupt  */
#define     SCSI_ANYPAUSE    (SCSI_BRKINT+SCSI_SCSIINT+SCSI_SEQINT)   /* any intr that pauses  */

/* ;  Bits 7-4 are written to '0' or '1' by sequencer.
   ;  Bits 3-0 can only be written to '1' by sequencer.  Previous '1's are
   ;  preserved by the write. */


#define  SCSI_CLRINT         0x92     /* clear interrupt status    (write)      */

#define     SCSI_CLRBRKINT   0x08     /* clear breakpoint interrupt  (brkint)   */
#define     SCSI_CLRSCSINT   0x04     /* clear scsi interrupt  (scsiint)        */
#define     SCSI_CLRCMDINT   0x02     /* clear command complete interrupt       */
#define     SCSI_CLRSEQINT   0x01     /* clear sequencer interrupt  (seqint)    */


#define  SCSI_ERROR          0x92     /* hard error                (read)       */

#define     SCSI_PCIERRSTAT  0x40     /* detect pci error                       */
#define     SCSI_PARERR      0x08     /* status = sequencer ram parity error    */
#define     SCSI_ILLOPCODE   0x04     /* status = illegal command line          */
#define     SCSI_ILLSADDR    0x02     /* status = illegal sequencer address     */
#define     SCSI_ILLHADDR    0x01     /* status = illegal host address          */


#define  SCSI_DFCNTRL        0x93     /* data fifo control register(read/write) */

#define     SCSI_WIDEODD     0x40     /* prevent flush of odd last byte         */
#define     SCSI_SCSIEN      0x20     /* enable xfer: scsi <-> sfifo    (write) */
#define     SCSI_SCSIENACK   0x20     /* SCSIEN clear acknowledge       (read)  */
#define     SCSI_SDMAEN      0x10     /* enable xfer: sfifo <-> dfifo   (write) */
#define     SCSI_SDMAENACK   0x10     /* SDMAEN clear acknowledge       (read)  */
#define     SCSI_HDMAEN      0x08     /* enable xfer: dfifo <-> host    (write) */
#define     SCSI_HDMAENACK   0x08     /* HDMAEN clear acknowledge       (read)  */
#define     SCSI_DIRECTION   0x04     /* transfer direction = write             */
#define     SCSI_FIFOFLUSH   0x02     /* flush data fifo to host                */
#define     SCSI_FIFORESET   0x01     /* reset data fifo                        */


#define  SCSI_DFSTATUS       0x94     /* data fifo status          (read)       */

#define     SCSI_MREQPEND    0x10     /* master request pending                 */
#define     SCSI_HDONE       0x08     /* host transfer done:                    */
                                      /*  hcnt=0 & bus handshake done           */
#define     SCSI_DFTHRSH     0x04     /* threshold reached                      */
#define     SCSI_FIFOFULL    0x02     /* data fifo full                         */
#define     SCSI_FIFOEMP     0x01     /* data fifo empty                        */


#define  SCSI_DFWADDR0       0x95     /* data fifo write address   (read/write) */
#define  SCSI_DFWADDR1       0x96     /* reserved         (read/write)          */

#define  SCSI_DFRADDR0       0x97     /* data fifo read address    (read/write) */
#define  SCSI_DFRADDR1       0x98     /* reserved         (read/write)          */
         
#define  SCSI_DFDAT          0x99     /* data fifo data register   (read/write) */

#define  SCSI_SCBCNT         0x9a     /* SCB count register        (read/write) */

#define     SCSI_SCBAUTO     0x80     /*                                        */


#define  SCSI_QINFIFO        0x9b     /* queue in fifo             (read/write) */

#define  SCSI_QINCNT         0x9c     /* queue in count            (read)       */

#define     SCSI_QINCNT2     0x04     /*                                        */
#define     SCSI_QINCNT1     0x02     /*                                        */
#define     SCSI_QINCNT0     0x01     /*                                        */


#define  SCSI_QOUTFIFO       0x9d     /* queue out fifo            (read/write) */

#define  SCSI_QOUTCNT        0x9e     /* queue out count           (read)       */

#define  SCSI_SFUNCT         0x9f     /* test chip                 (read/write) */

#define     SCSI_TESTHOST    0x08     /* select Host module for testing         */
#define     SCSI_TESTSEQ     0x04     /* select Sequencer module for testing    */
#define     SCSI_TESTFIFO    0x02     /* select Fifo module for testing         */
#define     SCSI_TESTSCSI    0x01     /* select Scsi module for testing         */
#define     SCSI_CS_ON       0x0e     /* enabled PCI config space               */


/****************************************************************************

 LANCE HOST REGISTERS

****************************************************************************/
#define  SCSI_SCB_BASE       0xa0

/****************************************************************************

 LANCE CONFIGURATION REGISTERS

****************************************************************************/

#define  SCSI_CONFIG_ADDRESS 0x0CF8      /* System Config Double word address 1 */

#define  SCSI_FORWARD_REG    0x0CFA      /* System Config Forward Address       */

#define  SCSI_CONFIG_DATA    0x0CFC      /* System Config Double word address 2 */

#define  SCSI_ID_REG         0x0000

#define     SCSI_VIKING_SABRE   0x7810   /* Id for Sabre chip                   */
#define     SCSI_BAYONET_LANCE  0x7876   /* Id for Lance on Bayonet board       */
#define     SCSI_VIKING_LANCE   0x7873   /* Id for Lance on Viking board        */
#define     SCSI_VLIGHT_LANCE   0x7872   /* Id for Sabre on Viking board        */
#define     SCSI_BAYONET_KATANA 0x7886   /* Id for Lance on Bayonet board       */
#define     SCSI_VIKING_KATANA  0x7883   /* Id for Lance on Viking board        */
#define     SCSI_VLIGHT_KATANA  0x7882   /* Id for Sabre on Viking board        */
#define     SCSI_DAGGER_PLUS    0x7855   /* id for Dagger plus                  */
#define     SCSI_ARO_EXCALIBUR  0x7893   /* Id for Excalibur on ARO board       */
#define     SCSI_ARO_BAYONET    0x0013   /* Id for Bayonet on ARO&AAA board     */

#define     SCSI_EXCALIBUR_BASE 0x7890   /* Id for excalibur (WW-will change to 9578)*/
#define     SCSI_KATANA_BASE    0x7880   /* Id for Lance                        */
#define     SCSI_KATANA_2930UW  0x7888   /* Id for 2930UW                       */ 
#define     SCSI_LANCE_BASE     0x7870   /* Id for Lance                        */
#define     SCSI_TALON_BASE     0x7860   /* Id for Talon                        */
#define     SCSI_TALON_2930CVAR 0x3860   /* Id for 2930CVAR                     */ 
#define     SCSI_DAGGER2_BASE   0x7850   /* Id for Dagger 2                     */
#define     SCSI_DAGGER1_BASE   0x7550   /* Id for Dagger 1                     */
#define     SCSI_2940AUW        0x7887   /* Id for 2940AUW                      */
#define     SCSI_19160_DISTI    0x0081   /* Id for 19160 Disti                  */
#define     SCSI_ID_MASK        0xFFF0   /* Id mask for multiple devices        */

#define     OLD_VENDOR_ID       0x00009004  /* Mask for old 9004 vendor id      */
#define     BAYONET_ID          0x00009005  /* Mask for Bayonet chip            */
#define     TRIDENT_ID          0x00809005  /* Mask for Trident chip            */

#define  SCSI_STATUS_CMD_REG 0x0004      /* Status/Command registers            */

#define     SCSI_DPE         0x80000000  /* Detected parity error               */
#define     SCSI_DPE_MASK    0x8000FFFF  /*                                     */
#define     SCSI_SSE         0x40000000  /* Signal system error                 */
#define     SCSI_SSE_MASK    0x4000FFFF  /*                                     */
#define     SCSI_RMA         0x20000000  /* Received master abort               */
#define     SCSI_RMA_MASK    0x2000FFFF  /*                                     */
#define     SCSI_RTA         0x10000000  /* Received target abort               */
#define     SCSI_RTA_MASK    0x1000FFFF  /*                                     */
#define     SCSI_STA         0x08000000  /* Signal target abort                 */
#define     SCSI_STA_MASK    0x0800FFFF  /*                                     */
#define     SCSI_DPR         0x01000000  /* Data parity reported                */
#define     SCSI_DPR_MASK    0x0100FFFF  /*                                     */
#define     SCSI_MWRICEN     0x00000010  /* MWIC enable                         */
#define     SCSI_MASTEREN    0x00000004  /* Master enable                       */
#define     SCSI_MSPACEEN    0x00000002  /* Memory space enable                 */
#define     SCSI_ISPACEEN    0x00000001  /* IO space enable                     */
#define     SCSI_ENABLE_MASK (SCSI_MASTEREN + SCSI_ISPACEEN)

#define  SCSI_STATUS_HI_REG  0x07        /* High byte of status register        */

#define  SCSI_DEV_REV_ID     0x0008      /* Device revision id                  */
#define     SCSI_BAYONET_REV_A 0x00      /* Bayonet rev A device rev ID         */

#define  SCSI_BASE_ADDR_REG  0x0010      /* Base Port Address for I/O space     */

#define  SCSI_SUBSYS_SUBVEND  0x002C     /* Base Port Address for I/O space     */

#define     SCSI_OEM1_SUBVID  0x0E11     /* SubVendor ID for OEM1               */

#define  SCSI_INTR_LEVEL_REG 0x003c      /* Interrupt Level (low 8 bits)        */

#define  SCSI_BUS_NUMBER     0x0018      /* bus number etc                      */

#define  SCSI_HDR_TYPE_REG  0x000C      /* Header Type                          */
#define  SCSI_CDWDSIZE      0x0000003C  /* Cache Line Size in units of 32-bit   */

#define  SCSI_DEVCONFIG      0x0040      /* Device configuration register       */

#define     SCSI_DIFACTNEGEN 0x00000001  /* Differential active negation enable */
#define     SCSI_STPWLEVEL   0x00000002  /* SCSI termination power down mode sel*/
#define     SCSI_DACEN       0x00000004  /* Dual address cycle enable           */
#define     SCSI_BERREN      0x00000008  /* Byte parity error enable            */
#define     SCSI_EXTSCBPEN   0x00000010  /* External SCB parity enable          */
#define     SCSI_EXTSCBTIME  0x00000020  /* External SCB time                   */
#define     SCSI_MRDCEN      0x00000040  
#define     SCSI_SCBRAMSEL   0x00000080  /* External SCB RAM select             */
#define     SCSI_64BITS_PCI  0x00000080  /* 64 bits pci                         */
#define     SCSI_VOLSENSE    0x00000100  /* Voltage sense                       */
#define     SCSI_RAMPSM      0x00000200  /* RAM present                         */
#define     SCSI_MPORTMODE   0x00000400
#define     SCSI_TESTBITEN   0x00000800  /* Test Bit Enable                     */
#define     SCSI_REXTVALID   0x00001000  /* External Resister Valid - FAST20    */
#define     SCSI_SCBSIZE32   0x00010000  /* 32 byte SCB page size enable        */

#define     SCSI_ULTRA_2     0x00        /* Ultra 2 / LVD  00b                  */
#define     SCSI_ULTRA       0x01        /* Ultra          01b                  */
#define     SCSI_ULTRA_160   0x02        /* Ultra 160      10b                  */
#define     SCSI_FAST        0x03        /* Fast           11b                  */

#define     SCSI_NO_SEEPROM  0x00        /* No SEEPROM     00b                  */
#define     SCSI_SEE_TYPE0   0x01        /* SEEPROM type 0 01b                  */
#define     SCSI_SEE_TYPE1   0x02        /* SEEPROM type 1 10b                  */
#define     SCSI_SEE_TYPE2   0x03        /* SEEPROM type 2 11b                  */

#define     SCSI_ONE_CHNL    0x00        /* One channel    00b                  */
#define     SCSI_TWO_CHNL    0x01        /* Two channels   01b                  */
#define     SCSI_THREE_CHNL  0x02        /* Three channels 10b                  */
#define     SCSI_FOUR_CHNL   0x03        /* Four channels  11b                  */

/****************************************************************************

 Driver - Sequencer interface

****************************************************************************/

/* INTCODES  - */

/* Seqint Driver interrupt codes identify action to be taken by the Driver.*/

#define  SCSI_SYNC_NEGO_NEEDED  0x00     /* initiate synchronous negotiation    */
#define  SCSI_ATN_TIMEOUT       0x00     /* timeout in atn_tmr routine          */
#define  SCSI_CDB_XFER_PROBLEM  0x10     /* possible parity error in cdb:  retry*/
#define  SCSI_HANDLE_MSG_OUT    0x20     /* handle Message Out phase            */
#define  SCSI_DATA_OVERRUN      0x30     /* data overrun detected               */
#define  SCSI_UNKNOWN_MSG       0x40     /* handle the message in from target   */
#define  SCSI_CHECK_CONDX       0x50     /* Check Condition from target         */
#define  SCSI_PHASE_ERROR       0x60     /* unexpected scsi bus phase           */
#define  SCSI_EXTENDED_MSG      0x70     /* handle Extended Message from target */
#define  SCSI_ABORT_TARGET      0x80     /* abort connected target              */
#define  SCSI_NO_ID_MSG         0x90     /* reselection with no id message      */
#define  SCSI_IDLE_LOOP_BREAK   0xA0     /* idle loop breakpoint                */
#define  SCSI_EXPANDER_BREAK    0xB0     /* expander breakpoint                 */
#if SCSI_TARGET_OPERATION  
#define  SCSI_TARGET_BUS_HELD   0xC0     /* target mode: SCSI BUS is held       */
#define  SCSI_TARGET_SPECIAL_FUNC  0xD0  /* target mode: special msg function   */
#endif  /* SCSI_TARGET_OPERATION */   
#define  SCSI_AUTO_RATE_OFFSET  0xE0     /* auto rate/offset for Seq. W160m and */
                                         /* Seq. S160m mode.                    */

/* Delay factor based on sequencer instruction speed */
#define  SCSI_DELAY_FACTOR_20MIPS 4000   /* delay for 20 mips sequencer (bayonet) */
#define  SCSI_DELAY_FACTOR_10MIPS 2000   /* delay for 10 mips sequencer (aic78xx) */

/****************************************************************************

EEPROM information definitions

****************************************************************************/
#define SCSI_EEPROM_SIZE        64       /* EEPROM size */

/* offset for EEPROM information */

#define  SCSI_EE_TARGET0        0        /* target 0 */
#define  SCSI_EE_TARGET1        2        /* target 1 */
#define  SCSI_EE_TARGET2        4        /* target 2 */
#define  SCSI_EE_TARGET3        6        /* target 3 */
#define  SCSI_EE_TARGET4        8        /* target 4 */
#define  SCSI_EE_TARGET5        10       /* target 5 */
#define  SCSI_EE_TARGET6        12       /* target 6 */
#define  SCSI_EE_TARGET7        14       /* target 7 */
#define  SCSI_EE_TARGET8        16       /* target 8 */
#define  SCSI_EE_TARGET9        18       /* target 9 */
#define  SCSI_EE_TARGET10       20       /* target 10 */
#define  SCSI_EE_TARGET11       22       /* target 11 */
#define  SCSI_EE_TARGET12       24       /* target 12 */
#define  SCSI_EE_TARGET13       26       /* target 13 */
#define  SCSI_EE_TARGET14       28       /* target 14 */
#define  SCSI_EE_TARGET15       30       /* target 15 */
#define  SCSI_EE_BIOSFLAG       32       /* bios flags */
#define  SCSI_EE_CONTROL        34       /* scsi channel control */
#define  SCSI_EE_ID             36       /* scsi id */
#define  SCSI_EE_BUSRLS         37       /* bus release */
#define  SCSI_EE_MAX_TARGET     38       /* max no of targets */ 

/* definitions for scsi channel control (SCSI_EE_CONTROL) */

#define SCSI_E2C_FAST20         0x80     /* enable/disable fast20 scsi */
#define SCSI_E2C_RESET          0x40     /* enable/disable scsi reset */
#define SCSI_E2C_PARITY         0x10     /* enable/disable parity */
#define SCSI_E2C_TERM_LOW       0x04     /* enable/disable low byte termination */
#define SCSI_E2C_TERM_HIGH      0x08     /* enable/disable high byte termination */
#define SCSI_E2C_TERMINATION    (SCSI_E2C_TERM_LOW | SCSI_E2C_TERM_HIGH)
#define SCSI_E2C_SEC_TERM_LO    0x0800   /* enable/disable secondary low byte termination */
#define SCSI_E2C_SEC_TERM_HI    0x1000   /* enable/disable secondary high byte termination */
#define SCSI_E2C_SEC_TERM       (E2C_SEC_TERM_LO | E2C_SEC_TERM_HI)

#if SCSI_AUTO_TERMINATION
#define SCSI_E2C_AUTOTERM       0x0001   /* autoterminatin bit */
#define SCSI_E2C_SEC_AUTOTERM   0x0400   /* secondary autotermination bit */
#endif

#if SCSI_SCAM_ENABLE
#define SCSI_E2C_SCAMENABLE     0x0100   /* scam enable bit */
#endif   /* of _SCAM */

/* definitions for target control flags (EE_TARGET?) */

#define SCSI_E2T_SYNCRATE       0x07     /* synchronous transfer rate */
#define SCSI_E2T_SYNCMODE       0x08     /* synchronous enable */
#define SCSI_E2T_DISCONNECT     0x10     /* disconnect enable */
#define SCSI_E2T_WIDE           0x20     /* intiate wide negotiation */
        
/* SCB usage for External Access method */

#define SCSI_HWSCB_BIOS      0           /* BIOS uses SCB 0 */
#define SCSI_HWSCB_DRIVER    1           /* Driver uses SCB 1 */
#define SCSI_HWSCB_STORAGE   2           /* SCB 2 used for parameter storage */

/* Definition for SEEPROM type */
#define SCSI_EETYPE_C06C46   0           /* NM93C06 or NM93C46 type SEEPROM */
#define SCSI_EETYPE_C56C66   1           /* NM93C56 or NM93C66 type SEEPROM */

/****************************************************************************
* SCAM operation definitions
****************************************************************************/
#if SCSI_SCAM_ENABLE || SCSI_PROFILE_INFORMATION
/* Definitions for scam control */
#define SCSI_DB7          0x80
#define SCSI_DB6          0x40
#define SCSI_DB5          0x20
#define SCSI_DB1          0x02
#define SCSI_DB0          0x01

#define SCSI_PRIORI_CODE  0xC0     /* bit 7 & 6 for the prority code */
                              /* or dominance preferrence code  */
#define SCSI_SCAM1_INIT      0x00        /* A level 1 SCAM initiator */
#define SCSI_SCAM2_INIT      0x40        /* A level 2 SCAM initiator only */
#define SCSI_DOM_SCAM2_INIT  0xC0        /* A Dominant level 2 SCAM initiator */

#define SCSI_MAX_ID_CODE  0x30   /* bit 5 & 4 for the Max ID code */
#define    SCSI_UPTO_1F   0x00        /* Maximum assignable ID - 0x1f */
#define    SCSI_UPTO_0F   0x10        /* Maximum assignable ID - 0x0f */
#define    SCSI_UPTO_07   0x20        /* Maximum assignable ID - 0x07 */

#define SCSI_ID_VALID     0x06   /* bit 2 & 1 for the ID valid info */
#define    SCSI_NOT_VALID          0x00     /* ID field bit 4-0 not valid */
#define    SCSI_VALID_BUT_UNASSIGN 0x02     /* ID field bit 4-0 valid but not assigned */
#define    SCSI_VALID_AND_ASSIGN   0x04     /* ID field bit 4-0 valid and assigned */

#define SCSI_SNA         0x01     /* bit 0 for Serial Number Available */

#define SCSI_SCAM_ID     0x001f   /* ID field for current ID of device */

#define SCSI_SYNC_PATTERN 0x1f   

#define SCSI_DOMINIT_CONT 0x0f    /* Dominant initiator contention */

#define SCSI_CONFIG_DONE  0x03    /* Configuration process complete */

#define SCSI_ASSIGN       0x00    /* function - ISOLATE & ASSIGN */
#define    SCSI_ISOLATE_TERMINATE  0x00     /* db(0-1) both zero - terminate  */

#define    SCSI_ASSIGN_00ID        0x18     /* action code - Assign ID 00nnnb */
#define    SCSI_ASSIGN_01ID        0x11     /* action code - Assign ID 01nnnb */
#define    SCSI_ASSIGN_10ID        0x12     /* action code - Assign ID 10nnnb */
#define    SCSI_ASSIGN_11ID        0x13     /* action code - Assign ID 11nnnb */

#define    SCSI_CONFIG_DONE        0x03     /* Configuration process complete */
           
#define  SCSI_ASSIGN_INPROG        0x0001
#define  SCSI_ASSIGN_DONE          0x0000
#define  SCSI_ASSIGN_FAIL          0xffff
#endif      /* of (#if SCSI_SCAM_ENABLE || SCSI_PROFILE_INFORMATION) */

/****************************************************************************
* AUTOTERMINATION operation definitions
****************************************************************************/
/* Definitions for SPIO for Samurai chip */
#define SCSI_SPIOCAP     0x1b   /* SPIO Capability register */
#define SCSI_SOFTCMDEN   0x20   /* Soft commands enable bit */
#define SCSI_EXT_BRDCTL  0x10   /* External board control */
#define SCSI_BRDCTL      0x1d    /* Board Control register */
#define SCSI_BRDCS       0x08    /* Board Chip Select */
#define SCSI_BRDRW       0x04    /* Board Read Write */

#define  SCSI_CABLE_INT50  0x01  /* internal 50-pin cable installed */
#define  SCSI_CABLE_INT68  0x02  /* internal 68-pin cable installed */
#define  SCSI_CABLE_EXT    0x04  /* external cable installed */

/* AutoTermination definitions for Bayonet based host adapters */
#define  SCSI_TERMINATION_LOW       0x08  /* setting on primary low termination */
#define  SCSI_TERMINATION_HIGH      0x10  /* setting on primary high termination */
#define  SCSI_SECONDARY_TERM_LOW    0x20  /* setting on secondary low termination */
#define  SCSI_SECONDARY_TERM_HIGH   0x40  /* setting on secondary high termination */

#define  SCSI_AUTOTERM_MODE0_CABLE_SENSING   0x00  /* cable sensing mode 0 */
#define  SCSI_AUTOTERM_MODE1_CABLE_SENSING   0x01  /* cable sensing mode 1 */
#define  SCSI_AUTOTERM_MODE2_CABLE_SENSING   0x02  /* cable sensing mode 2 */

/****************************************************************************
* Excalibur command channel register  
****************************************************************************/
#define  SCSI_CCLHADDR0      0xE0        /* Command channel low host address */
#define  SCSI_CCLHADDR1      0xE1
#define  SCSI_CCLHADDR2      0xE2
#define  SCSI_CCLHADDR3      0xE3
#define  SCSI_CCHHADDR0      0xE4        /* Command channel high host address */
#define  SCSI_CCHHADDR1      0xE5
#define  SCSI_CCHHADDR2      0xE6
#define  SCSI_CCHHADDR3      0xE7
#define  SCSI_CCHCNT         0xE8        /* Command channel host count */

#define  SCSI_CCSGRAM        0xE9        /* Command channel S/G ram data port */
#define  SCSI_CCSGADR        0xEA        /* Command channel S/G ram adr pointer */
#define  SCSI_CCSGCTL        0xEB        /* Command channel S/G control */
#define     SCSI_CCSGDONE       0x80     /* Command channel s/g prefetch done */
#define     SCSI_CCSGEN         0x08     /* Command channel s/g xfer enable */
#define     SCSI_CCSGENACK      0x08     /* Command channel s/g xfer ack */
#define     SCSI_FLAG           0x02     /* read/write bit only */
#define     SCSI_CCSGRESET      0x01     /* Command channel s/g reset */

#define  SCSI_CCSCBRAM       0xEC        /* Command channel SCB ram data port */
#define  SCSI_CCSCBADR       0xED        /* Command channel SCB ram adr pointer */
#define  SCSI_CCSCBCTL       0xEE        /* Command channel SCB control */
#define     SCSI_CCSCBDONE      0x80              /* SCB command channel done */        
#define     SCSI_ARRDONE        0x40           /* The scb array prefetch done */
#define     SCSI_CCARREN        0x10              /* The scb array dma enable */
#define     SCSI_CCARRENACK     0x10              /* scb array dma enable ack */
#define     SCSI_CCSCBEN        0x08  /* command channel scb ram  xfer enable */
#define     SCSI_CCSCBENACK     0x08   /* cmd channel scb ram xfer enable ack */
#define     SCSI_CCSCBDIR       0x04    /* Command channel SCB SRAM direction */
#define     SCSI_CCSCBDIRACK    0x04    /* Cmd channel SCB SRAM direction sck */
#define     SCSI_CCSCBRESET     0x01        /* Command channel SCB SRAM reset */

#define  SCSI_CCSCBCNT       0xEF           /* Command channel SCB count */
#define  SCSI_SCBBADDR       0xF0    /* Command channel SCB base address */
#define  SCSI_CCSCBPTR       0xF1         /* Command channel SCB pointer */

#define  SCSI_SCRATCH0                0x20           /* scratch base address  */
#define  SCSI_SCRATCH0_SIZE           64             /* scratch 0 size        */
#define  SCSI_SCRATCH1                0x70           /* scratch base address  */
#define  SCSI_SCRATCH1_SIZE           16             /* scratch 1 size        */
#define  SCSI_MAX_SCBSIZE             64             /* sequencer code size   */

/****************************************************************************
* Bayonet/Scimitar/Trident registers
****************************************************************************/
#define  SCSI_BAYONET_SCSIRATE      0x04  /* scsi rate control          (write) */

#define     SCSI_BAYONET_WIDEXFER   0x80  /* ch 0 wide scsi bus                 */
#define     SCSI_BAYONET_XFERRATE6  0x40  /* synchronous scsi transfer rate     */
#define     SCSI_BAYONET_XFERRATE5  0x20  /*                                    */
#define     SCSI_BAYONET_XFERRATE4  0x10  /*                                    */
#define     SCSI_BAYONET_XFERRATE3  0x08  /*                                    */
#define     SCSI_BAYONET_XFERRATE2  0x04  /*                                    */
#define     SCSI_BAYONET_XFERRATE1  0x02  /*                                    */
#define     SCSI_BAYONET_XFERRATE0  0x01  /*                                    */
#define     SCSI_BAYONET_XFERRATE   (SCSI_BAYONET_XFERRATE6+SCSI_BAYONET_XFERRATE5\
                                    +SCSI_BAYONET_XFERRATE4+SCSI_BAYONET_XFERRATE3\
                                    +SCSI_BAYONET_XFERRATE2+SCSI_BAYONET_XFERRATE1\
                                    +SCSI_BAYONET_XFERRATE0)

#define  SCSI_BAYONET_SCSIOFFSET    0x05  /* scsi offset control        (write) */

#define     SCSI_BAYONET_OFFSET6    0x40  /* synchronous scsi transfer rate     */
#define     SCSI_BAYONET_OFFSET5    0x20  /*                                    */
#define     SCSI_BAYONET_OFFSET4    0x10  /*                                    */
#define     SCSI_BAYONET_OFFSET3    0x08  /*                                    */
#define     SCSI_BAYONET_OFFSET2    0x04  /*                                    */
#define     SCSI_BAYONET_OFFSET1    0x02  /*                                    */
#define     SCSI_BAYONET_OFFSET0    0x01  /*                                    */
#define     SCSI_BAYONET_OFFSET     (SCSI_BAYONET_OFFSET6+SCSI_BAYONET_OFFSET5\
                                    +SCSI_BAYONET_OFFSET4+SCSI_BAYONET_OFFSET3\
                                    +SCSI_BAYONET_OFFSET2+SCSI_BAYONET_OFFSET1\
                                    +SCSI_BAYONET_OFFSET0)

#define  SCSI_BAYONET_SSTAT2        0x0D  /* scsi status 2         (read/write) */
#define  SCSI_BAYONET_EXPACTIVE_DEFAULT 1 /* default bayonet expander active    */

#define  SCSI_BAYONET_SCSIID        0x0F  /* scsi id               (read/write) */

#define  SCSI_BAYONET_TARGIDL       0x1B  /* Used for Multi-id feature          */
                                          /* This register pair contains        */
#define  SCSI_BAYONET_TARGIDH       0x1C  /* a mask of selection-in IDs         */

#define  SCSI_HNSCB_QOFF            0xF4  /* host new scb queue offset          */
#define  SCSI_SNSCB_QOFF            0xF6  /* sequencer new scb queue offset     */
#define  SCSI_SDSCB_QOFF            0xF8  /* sequencer done scb queue offset    */
#define  SCSI_QOFF_CTLSTA           0xFA  /* queue offset control & status      */
#define     SCSI_SCB_AVAIL          0x40  /* scb available bit                  */
#define     SCSI_SNSCB_ROLLOVER     0x20  /* sequencer new scb queue roll-over bit */
#define     SCSI_SDSCB_ROLLOVER     0x10  /* sequencer done scb queue roll-over bit */
#define     SCSI_SCB_QSIZE2         0x04  /* scb queue size                     */
#define     SCSI_SCB_QSIZE1         0x02  /*                                    */
#define     SCSI_SCB_QSIZE0         0x01  /*                                    */
#define     SCSI_SCB_QSIZE          (SCSI_SCB_QSIZE2+SCSI_SCB_QSIZE1 \
                                    +SCSI_SCB_QSIZE0)

#define  SCSI_BAYONET_COMMAND0      0x84  /* DSCOMMAND0 register                */

#define     SCSI_BAYONET_EXTREQLCK     0x10  /*  */
#define     SCSI_BAYONET_INTSCBRAMSEL  0x08  /*  */
#define     SCSI_BAYONET_RAMPSM        0x04  /*  */
#define     SCSI_BAYONET_SCBSIZE32     0x02  /*  */

#define  SCSI_BAYONET_MAILBOX       0x86  /* MailBox register                   */  
#define     SCSI_BAYONET_HNMB_MASK  0xF0  /* Host Mail Box                      */
#define     SCSI_BAYONET_SNMB_MASK  0x0F  /* Sequencer Mail box                 */

#define  SCSI_BAYONET_FIFO_THRSH    0xFB  /* register for data fifo threshold control */

#define     SCSI_BAYONET_WR_DFTHRSH 0x70  /* write threshold */ 
#define     SCSI_BAYONET_RD_DFTHRSH 0x07  /* read threshold  */

#define  SCSI_BAYONET_WR_DFT_DEFAULT 0x70 /* write threshold default */
#define  SCSI_BAYONET_RD_DFT_DEFAULT 0x07 /* read threshold default */

#define  SCSI_BAYONET_CACHETHEEN_DEFAULT 1/* default bayonet cachethen */
#define  SCSI_BAYONET_MAX_SYNC_PERIOD 50  /* max xfer period to go sync */

/****************************************************************************
* Trident registers
****************************************************************************/
/* Trident scsi rate definition in standard mode */
#define  SCSI_TRIDENT_SCSIRATE      0x04  /* scsi rate control     (read/write) */

#define     SCSI_TRIDENT_WIDEXFER   0x80  /* wide scsi xfer                     */
#define     SCSI_ENABLE_CRC         0x40  /* crc protocol                       */
#define     SCSI_SINGLE_EDGE        0x10  /* single-edge/dual-edge select       */
#define     SCSI_TRIDENT_XFERRATE3  0x08  /* scsi data xfer rate                */
#define     SCSI_TRIDENT_XFERRATE2  0x04  /*                                    */
#define     SCSI_TRIDENT_XFERRATE1  0x02  /*                                    */
#define     SCSI_TRIDENT_XFERRATE0  0x01  /*                                    */
#define     SCSI_TRIDENT_XFERRATE   (SCSI_TRIDENT_XFERRATE3+SCSI_TRIDENT_XFERRATE2+\
                                     SCSI_TRIDENT_XFERRATE1+SCSI_TRIDENT_XFERRATE0)

#define  SCSI_TRIDENT_SCSIOFFSET    0x05  /* scsi offset control   (read/write) */

/* Trident scsi rate/offset definition in enhanced mode */
#define  SCSI_AUTORATE_PNTR         0x04  /* scsi auto rate pntr   (read/write) */
#define     SCSI_RATE_DATA          0x10  /* auto rate data/offset data select  */
#define     SCSI_DEV_ID3            0x08  /* device id select                   */
#define     SCSI_DEV_ID2            0x04  /*                                    */
#define     SCSI_DEV_ID1            0x02  /*                                    */
#define     SCSI_DEV_ID0            0x01  /*                                    */

#define  SCSI_AUTORATE_DATA         0x05  /* scsi auto rate data   (read/write) */
#define     SCSI_RATE_OFFSET7       0x80  /* scsi rate/offset data              */
#define     SCSI_RATE_OFFSET6       0x40  /*                                    */
#define     SCSI_RATE_OFFSET5       0x20  /*                                    */
#define     SCSI_RATE_OFFSET4       0x10  /*                                    */
#define     SCSI_RATE_OFFSET3       0x08  /*                                    */
#define     SCSI_RATE_OFFSET2       0x04  /*                                    */
#define     SCSI_RATE_OFFSET1       0x02  /*                                    */
#define     SCSI_RATE_OFFSET0       0x01  /*                                    */
#define     SCSI_RATE_OFFSET        (SCSI_RATE_OFFSET7+SCSI_RATE_OFFSET6+\
                                     SCSI_RATE_OFFSET5+SCSI_RATE_OFFSET4+\
                                     SCSI_RATE_OFFSET3+SCSI_RATE_OFFSET2+\
                                     SCSI_RATE_OFFSET1+SCSI_RATE_OFFSET0)

#define  SCSI_SCSIDATL_IMG          0x9C  /* scsidatl image        (read/write) */

#define  SCSI_TRIDENT_SFUNCT        0x9F  /* special function      (read/write) */
#define     SCSI_ALT_MODE           0x80  /* alternate mode                     */

#define  SCSI_OPTIONMODE            0x08  /* option mode           (read/write) */
#define     SCSI_AUTORATEEN         0x80  /* automatic rate & offset enable     */
#define     SCSI_AUTOACKEN          0x40  /* automatic ack enable               */
#define     SCSI_ATNMGMNTEN         0x20  /* ATN management enable              */
#define     SCSI_BUSFREEREV         0x10  /* bus free interrupt revision enable */
#define     SCSI_EXPPHASEDIS        0x08  /* expected phase disable             */
#define     SCSI_SCSIDATL_IMGEN     0x04  /* scsidatl image reg. enable         */
#define     SCSI_AUTO_MSGOUT_DE     0x02  /* auto. msg-out in dual edge mode    */
#define     SCSI_DIS_MSGIN_DE       0x01  /* disable msg-in in dual edge mode   */

#define  SCSI_TRIDENT_MAX_STSYNC_PERIOD 50  /* max xfer period to go ST sync    */
#define  SCSI_TRIDENT_MAX_DTSYNC_PERIOD 100 /* max xfer period to go DT sync    */
/*  Trident SEEPROM accessing state machine registers definitions */
#define  SCSI_SE2CST                0xFD  /* SEEPROM status register            */
#define     SCSI_SE2_INIT_DONE      0x80
#define     SCSI_SE2_LDALTID_L      0x08
#define     SCSI_SE2_DISSEE_WPEN    0x04
#define     SCSI_SE2_BUSY           0x02
#define     SCSI_SE2_START          0x01
/* SE2OPCODE */
#define     SCSI_SE2_READ           0x60  /* read data                          */
#define     SCSI_SE2_ERASE          0x70  /* erase memory                       */
#define     SCSI_SE2_WRITE          0x50  /* write memory                       */
#define     SCSI_SE2_ERAL           0x40  /* erase all                          */
#define     SCSI_SE2_WRAL           0x40  /* write all                          */
#define     SCSI_SE2_EWDS           0x40  /* disable write                      */
#define     SCSI_SE2_EWEN           0x40  /* enable write                       */

#define     SCSI_SE2DAT0            0xF2  /* SEEPROM data register 0            */
#define     SCSI_SE2DAT1            0xF3  /* SEEPROM data register 1            */
#define     SCSI_SE2ADR0            0xF9  /* SEEPROM address register           */

#if SCSI_TRIDENT_PROTO
#define  SCSI_TRIDENT_IOPDNCTL      0x14  /* SCSI I/O cell powerdown control (alternate) */
#define     SCSI_DIS_FORCE_BC       0x20  /* Disable automatic Bias cancellation */
#endif /* SCSI_TRIDENT_PROTO */

/*$Header:   Y:/source/chimscsi/src/himscsi/hwmscsi/HARDDEF.HV_   1.17.3.1   22 Jan 1998 20:58:46   THU  $*/
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
*  Module Name:   HARDDEF.H
*
*  Description:   Definitions for hardware device definitions which
*                 are devices specific (e.g. cross Lance, Sabre and
*                 Mace)
*    
*  Owners:        ECX IC Firmware Team
*    
*  Notes:         
*
***************************************************************************/

#if SCSI_MULTI_HARDWARE
typedef struct SCSI_HARDWARE_DESCRIPTOR_
{
   void (*SCSIhLoadScsiRate)(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT16); /* load scsi rate */
   void (*SCSIhResetDFifo)(SCSI_HHCB SCSI_HPTR); /* reset dfifo */
   void (*SCSIhEnableIOErr)(SCSI_HHCB SCSI_HPTR); /* enable IO Error */
   SCSI_UEXACT8 (*SCSIhGetWideScsiOffset)(SCSI_HIOB SCSI_IPTR); /* SCSI wide offset   */
   SCSI_UEXACT8 (*SCSIhGetNarrowScsiOffset)(SCSI_HIOB SCSI_IPTR); /* SCSI narrow offset */
   SCSI_UEXACT8 (*SCSIhGetXferPeriod)(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); /* get xfer period */
   SCSI_UEXACT16 (*SCSIhCalcScsiRate)(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT8,SCSI_UEXACT8); /* calc scsi rate */
   void (*SCSIhClearScsiOffset)(SCSI_HIOB SCSI_IPTR); /* clear scsi offset */
   SCSI_UEXACT8 (*SCSIhWasSyncXfer)(SCSI_UEXACT8); /* was sync xfer */
   void (*SCSIhProcessAutoTerm)(SCSI_HHCB SCSI_HPTR);  /* process auto termination */
   void (*SCSIhUpdateExtTerm)(SCSI_HHCB SCSI_HPTR);   /* update external termination */
   SCSI_UEXACT32 (*SCSIhGetScbRamCtrls)(SCSI_HHCB SCSI_HPTR);  /* get SCB RAM control bits */
   void (*SCSIhPutScbRamCtrls)(SCSI_HHCB SCSI_HPTR, SCSI_UEXACT32);  /* put SCB RAM control bits */
   SCSI_UEXACT8 (*SCSIhSetDataFifoThrshDefault)(SCSI_HHCB SCSI_HPTR); /* set default fifo threshold */ 
   SCSI_UEXACT8 (*SCSIhReadDataFifoThrsh)(SCSI_HHCB SCSI_HPTR); /* read fifo threshold */
   void (*SCSIhUpdateDataFifoThrsh)(SCSI_HHCB SCSI_HPTR); /* update fifo threshold */   
   SCSI_UEXACT8 (*SCSIhGetCacheThEn)(SCSI_HHCB SCSI_HPTR); /* get cachethen */
   void (*SCSIhUpdateCacheThEn)(SCSI_HHCB SCSI_HPTR); /* update cachethen */
   void (*SCSIhGetProfileParameters)(SCSI_HW_INFORMATION SCSI_LPTR,SCSI_HHCB SCSI_HPTR); /* get profile parameters from hwm layer into hw_info structure */
   void (*SCSIhPutProfileParameters)(SCSI_HW_INFORMATION SCSI_LPTR,SCSI_HHCB SCSI_HPTR); /* put profile parameters from hw_info structure into hwm layer */
   void (*SCSIhCheckExtArbAckEnBit)(SCSI_HHCB SCSI_HPTR); /* check external arbitration enable bit */
   SCSI_UEXACT8 (*SCSIhGetQNewTail)(SCSI_HHCB SCSI_HPTR); /* get q_new_tail value */
   void (*SCSIhWaitForBusStable)(SCSI_HHCB SCSI_HPTR); /* wait for SCSI bus stable */
   void (*SCSIhSeqStretchWorkaround)(SCSI_HHCB SCSI_HPTR); /* perform workaround to clear stretch signal */
   void (*SCSIhIgnoreWideResCalc)(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8); /* ignore wide residue calculation */
   void (*SCSIhTargetLoadTargetIds)(SCSI_HHCB SCSI_HPTR); /* load Target IDs */
   void (*SCSIhResetScsiRate)(SCSI_HHCB SCSI_HPTR); /* reset scsi rate/offset table */
   void (*SCSIhClearScsiRate)(SCSI_HHCB SCSI_HPTR); /* clear scsi rate/offset */
   SCSI_UEXACT16 (*SCSIhMaxSyncPeriod)(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8); /* max xfer period to go sync xfer */
#if SCSI_NEGOTIATION_PER_IOB
   void (*SCSIhChangeXferOption)(SCSI_HIOB SCSI_IPTR);
#endif
   SCSI_UEXACT32 RAMPSM;  /* bit position for RAM Present Mode */
   SCSI_UEXACT32 SCBRAMSEL;  /* bit position for SCB RAM Select */
   SCSI_UEXACT32 SCBSIZE32;  /* bit position for SCB RAM Size */
   SCSI_UEXACT16 SeqCodeSize; /* sequencer code size */
   SCSI_UEXACT8 defaultCacheThEn; /* cache threshold enable flag */
   SCSI_UEXACT8 defaultExpActive; /* expander active flag */
   SCSI_UEXACT8 ScsiID;
   SCSI_UEXACT16 delayFactor; /* minimum speed to go sync xfer */
} SCSI_HARDWARE_DESCRIPTOR;


#if SCSI_AIC785X
#endif

#if SCSI_AIC787X
#endif

#if SCSI_AIC788X
#endif

#if SCSI_AIC789X
#endif

/***************************************************************************
*  SCSI_HARDWARE_DESCRIPTOR for AIC-78XX devices
***************************************************************************/
#if SCSI_AIC78XX && defined(SCSI_DATA_DEFINE)
void SCSIhAIC78XXLoadScsiRate(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT16);
void SCSIhAIC78XXResetDFifo(SCSI_HHCB SCSI_HPTR);
void SCSIhAIC78XXEnableIOErr(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8 SCSIhAIC78XXGetXferPeriod(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIhAIC78XXGetWideScsiOffset(SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIhAIC78XXGetNarrowScsiOffset(SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT16 SCSIhAIC78XXCalcScsiRate(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT8,SCSI_UEXACT8);
void SCSIhAIC78XXClearScsiOffset(SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIhAIC78XXWasSyncXfer(SCSI_UEXACT8);
void SCSIhAIC78XXProcessAutoTerm(SCSI_HHCB SCSI_HPTR);
void SCSIhAIC78XXUpdateExtTerm(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT32 SCSIhAIC78XXGetScbRamCtrls(SCSI_HHCB SCSI_HPTR);
void SCSIhAIC78XXPutScbRamCtrls(SCSI_HHCB SCSI_HPTR, SCSI_UEXACT32);
SCSI_UEXACT8 SCSIhAIC78XXSetDataFifoThrshDefault(SCSI_HHCB SCSI_HPTR);  
SCSI_UEXACT8 SCSIhAIC78XXReadDataFifoThrsh(SCSI_HHCB SCSI_HPTR); 
void SCSIhAIC78XXUpdateDataFifoThrsh(SCSI_HHCB SCSI_HPTR);   
SCSI_UEXACT8 SCSIhGetCacheThEn(SCSI_HHCB SCSI_HPTR);
void SCSIhAIC78XXUpdateCacheThEn(SCSI_HHCB SCSI_HPTR);
void SCSIhAIC78XXGetProfileParameters(SCSI_HW_INFORMATION SCSI_LPTR, SCSI_HHCB SCSI_HPTR);
void SCSIhAIC78XXPutProfileParameters(SCSI_HW_INFORMATION SCSI_LPTR, SCSI_HHCB SCSI_HPTR);
void SCSIhAIC78XXCheckExtArbAckEnBit(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8 SCSIhAIC78XXGetQNewTail(SCSI_HHCB SCSI_HPTR);
void SCSIhAIC78XXIgnoreWideResCalc(SCSI_HHCB SCSI_HPTR, SCSI_HIOB SCSI_IPTR, SCSI_UEXACT8);
void SCSIhAIC78XXResetScsiRate(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT16 SCSIhAIC78XXMaxSyncPeriod(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
#if SCSI_NEGOTIATION_PER_IOB
void SCSIhAIC78XXChangeXferOption(SCSI_HIOB SCSI_IPTR);
#endif
SCSI_HARDWARE_DESCRIPTOR SCSIHardwareAIC78XX = 
{
   SCSIhAIC78XXLoadScsiRate,           /* load scsi rate                */
   SCSIhAIC78XXResetDFifo,             /* reset dfifo                   */
   SCSIhAIC78XXEnableIOErr,            /* enable IO Error               */
   SCSIhAIC78XXGetWideScsiOffset,      /* SCSI wide offset              */
   SCSIhAIC78XXGetNarrowScsiOffset,    /* SCSI narrow offset            */
   SCSIhAIC78XXGetXferPeriod,          /* get xfer period               */
   SCSIhAIC78XXCalcScsiRate,           /* calc scsi rate                */
   SCSIhAIC78XXClearScsiOffset,        /* clear scsi offset             */
   SCSIhAIC78XXWasSyncXfer,            /* was sync xfer                 */
#if SCSI_AUTO_TERMINATION
   SCSIhAIC78XXProcessAutoTerm,        /* process auto termination      */
#else
   0,                                  /* process auto termination      */
#endif
   SCSIhAIC78XXUpdateExtTerm,          /* update external termination   */
   SCSIhAIC78XXGetScbRamCtrls,         /* get SCB RAM control bits      */
   SCSIhAIC78XXPutScbRamCtrls,         /* put SCB RAM control bits      */
   SCSIhAIC78XXSetDataFifoThrshDefault,/* set data fifo default         */  
   SCSIhAIC78XXReadDataFifoThrsh,      /* read data fifo threshold      */
   SCSIhAIC78XXUpdateDataFifoThrsh,    /* update data fifo threshold    */
   SCSIhGetCacheThEn,                  /* get cachethen                 */
   SCSIhAIC78XXUpdateCacheThEn,        /* update cachethen              */
   SCSIhAIC78XXGetProfileParameters,   /* get profile parameters        */
   SCSIhAIC78XXPutProfileParameters,   /* put profile parameters        */
   SCSIhAIC78XXCheckExtArbAckEnBit,    /* check external arbitration enable bit */
   SCSIhAIC78XXGetQNewTail,            /* get q_new_tail value          */
   0,                                  /* wait for SCSI bus stable      */
   0,                                  /* sequencer stretch workaround  */
   SCSIhAIC78XXIgnoreWideResCalc,      /* ignore wide residue calculation */
   0,                                  /* load TARGIDL/H registers      */
   SCSIhAIC78XXResetScsiRate,          /* reset scsi rate/offset table  */
   SCSIhAIC78XXResetScsiRate,          /* clear scsi rate/offset        */
   SCSIhAIC78XXMaxSyncPeriod,          /* max xfer period to go sync xfer */
#if SCSI_NEGOTIATION_PER_IOB
   SCSIhAIC78XXChangeXferOption,       /* Change xfer option form IOB   */
#endif
   SCSI_RAMPSM,                        /* SCB RAM Present bit           */
   SCSI_SCBRAMSEL,                     /* SCB RAM Select bit            */
   SCSI_SCBSIZE32,                     /* SCB RAM Size bit              */
   SCSI_AIC78XX_SEQCODE_SIZE,          /* Sequencer ram size            */
   SCSI_AIC78XX_CACHETHEEN_DEFAULT,    /* default cachethen             */
   SCSI_AIC78XX_EXPACTIVE_DEFAULT,     /* default expactive             */
   SCSI_SCSIID,                        /* scsi id                       */
   SCSI_DELAY_FACTOR_10MIPS            /* delay factor                  */
};
#endif /* SCSI_AIC78XX && defined(SCSI_DATA_DEFINE) */

#if SCSI_AICBAYONET && defined(SCSI_DATA_DEFINE)
void SCSIhAICBayonetLoadScsiRate(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT16);
void SCSIhAICBayonetResetDFifo(SCSI_HHCB SCSI_HPTR);
void SCSIhAICBayonetEnableIOErr(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8 SCSIhAICBayonetGetXferPeriod(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIhAICBayonetGetScsiOffset(SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT16 SCSIhAICBayonetCalcScsiRate(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT8,SCSI_UEXACT8);
void SCSIhAICBayonetClearScsiOffset(SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIhAICBayonetWasSyncXfer(SCSI_UEXACT8);
void SCSIhAICBayonetProcessAutoTerm(SCSI_HHCB SCSI_HPTR);
void SCSIhAICBayonetUpdateExtTerm(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT32 SCSIhAICBayonetGetScbRamCtrls(SCSI_HHCB SCSI_HPTR);
void SCSIhAICBayonetPutScbRamCtrls(SCSI_HHCB SCSI_HPTR, SCSI_UEXACT32);
SCSI_UEXACT8 SCSIhAICBayonetSetDataFifoThrshDefault(SCSI_HHCB SCSI_HPTR);  
SCSI_UEXACT8 SCSIhAICBayonetReadDataFifoThrsh(SCSI_HHCB SCSI_HPTR); 
void SCSIhAICBayonetUpdateDataFifoThrsh(SCSI_HHCB SCSI_HPTR);   
SCSI_UEXACT8 SCSIhGetCacheThEn(SCSI_HHCB SCSI_HPTR);
void SCSIhAICBayonetUpdateCacheThEn(SCSI_HHCB SCSI_HPTR);
void SCSIhAICBayonetGetProfileParameters(SCSI_HW_INFORMATION SCSI_LPTR, SCSI_HHCB SCSI_HPTR);
void SCSIhAICBayonetPutProfileParameters(SCSI_HW_INFORMATION SCSI_LPTR, SCSI_HHCB SCSI_HPTR);
void SCSIhAICBayonetCheckExtArbAckEnBit(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8 SCSIhAICBayonetGetQNewTail(SCSI_HHCB SCSI_HPTR);
void SCSIhAICBayonetWaitForBusStable(SCSI_HHCB SCSI_HPTR);
void SCSIhAICBayonetSeqStretchWorkaround(SCSI_HHCB SCSI_HPTR);
void SCSIhAICBayonetIgnoreWideResCalc(SCSI_HHCB SCSI_HPTR, SCSI_HIOB SCSI_IPTR, SCSI_UEXACT8);
void SCSIhTargetAICBayonetLoadTargetIDs(SCSI_HHCB SCSI_HPTR);
void SCSIhAICBayonetResetScsiRate(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT16 SCSIhAICBayonetMaxSyncPeriod(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
#if SCSI_NEGOTIATION_PER_IOB
void SCSIhAICBayonetChangeXferOption(SCSI_HIOB SCSI_IPTR);
#endif
SCSI_HARDWARE_DESCRIPTOR SCSIHardwareAICBayonet = 
{
   SCSIhAICBayonetLoadScsiRate,        /* load scsi rate                */
   SCSIhAICBayonetResetDFifo,          /* reset dfifo                   */
   SCSIhAICBayonetEnableIOErr,         /* enable IO Error               */
   SCSIhAICBayonetGetScsiOffset,       /* SCSI wide offset              */
   SCSIhAICBayonetGetScsiOffset,       /* SCSI narrow offset            */
   SCSIhAICBayonetGetXferPeriod,       /* get xfer period               */
   SCSIhAICBayonetCalcScsiRate,        /* calc scsi rate                */
   SCSIhAICBayonetClearScsiOffset,     /* clear scsi offset             */
   SCSIhAICBayonetWasSyncXfer,         /* was sync xfer                 */
#if SCSI_AUTO_TERMINATION
   SCSIhAICBayonetProcessAutoTerm,     /* process auto termination      */
#else
   0,                                  /* process auto termination      */
#endif
   SCSIhAICBayonetUpdateExtTerm,       /* update external termination   */
   SCSIhAICBayonetGetScbRamCtrls,      /* get SCB RAM control bits      */
   SCSIhAICBayonetPutScbRamCtrls,      /* put SCB RAM control bits      */
   SCSIhAICBayonetSetDataFifoThrshDefault,/* set data fifo default      */  
   SCSIhAICBayonetReadDataFifoThrsh,   /* read data fifo threshold      */
   SCSIhAICBayonetUpdateDataFifoThrsh, /* update data fifo threshold    */   
   SCSIhGetCacheThEn,                  /* get cachethen                 */
   SCSIhAICBayonetUpdateCacheThEn,     /* update cachethen              */
   SCSIhAICBayonetGetProfileParameters,/* get profile parameters        */
   SCSIhAICBayonetPutProfileParameters,/* put profile parameters        */
   SCSIhAICBayonetCheckExtArbAckEnBit, /* check external arbitration enable bit */
   SCSIhAICBayonetGetQNewTail,         /* get q_new_tail value          */
   SCSIhAICBayonetWaitForBusStable,    /* wait for SCSI bus stable      */
   SCSIhAICBayonetSeqStretchWorkaround,/* sequencer stretch workaround  */
   SCSIhAICBayonetIgnoreWideResCalc,   /* ignore wide residue calculation */
#if SCSI_TARGET_OPERATION && SCSI_MULTIPLEID_SUPPORT
   SCSIhTargetAICBayonetLoadTargetIDs, /* load TARGIDL/H registers      */
#else
   0,
#endif /* SCSI_TARGET_OPERATION && SCSI_MULTIPLEID_SUPPORT */
   SCSIhAICBayonetResetScsiRate,       /* reset scsi rate/offset table  */
   SCSIhAICBayonetResetScsiRate,       /* clear scsi rate/offset        */
   SCSIhAICBayonetMaxSyncPeriod,       /* max xfer period to go sync xfer */
#if SCSI_NEGOTIATION_PER_IOB
   SCSIhAICBayonetChangeXferOption,    /* Change xfer option form IOB   */
#endif
   SCSI_BAYONET_RAMPSM,                /* SCB RAM Present bit           */
   SCSI_BAYONET_INTSCBRAMSEL,          /* SCB RAM Select bit            */
   SCSI_BAYONET_SCBSIZE32,             /* SCB RAM Size bit              */
   SCSI_BAYONET_SEQCODE_SIZE,          /* Sequencer ram size            */
   SCSI_BAYONET_CACHETHEEN_DEFAULT,    /* default cachethen             */
   SCSI_BAYONET_EXPACTIVE_DEFAULT,     /* default cachethen             */
   SCSI_BAYONET_SCSIID,                /* scsi id                       */
   SCSI_DELAY_FACTOR_20MIPS            /* delay factor                  */
};
#endif /* SCSI_AICBAYONET && defined(SCSI_DATA_DEFINE) */

#if SCSI_AICTRIDENT && defined(SCSI_DATA_DEFINE)
void SCSIhAICTridentLoadScsiRate(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT16);
void SCSIhAICBayonetResetDFifo(SCSI_HHCB SCSI_HPTR);
void SCSIhAICBayonetEnableIOErr(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8 SCSIhAICTridentGetXferPeriod(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIhAICBayonetGetScsiOffset(SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT16 SCSIhAICTridentCalcScsiRate(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT8,SCSI_UEXACT8);
void SCSIhAICBayonetClearScsiOffset(SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIhAICBayonetWasSyncXfer(SCSI_UEXACT8);
void SCSIhAICBayonetProcessAutoTerm(SCSI_HHCB SCSI_HPTR);
void SCSIhAICBayonetUpdateExtTerm(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT32 SCSIhAICBayonetGetScbRamCtrls(SCSI_HHCB SCSI_HPTR);
void SCSIhAICBayonetPutScbRamCtrls(SCSI_HHCB SCSI_HPTR, SCSI_UEXACT32);
SCSI_UEXACT8 SCSIhAICBayonetSetDataFifoThrshDefault(SCSI_HHCB SCSI_HPTR);  
SCSI_UEXACT8 SCSIhAICBayonetReadDataFifoThrsh(SCSI_HHCB SCSI_HPTR); 
void SCSIhAICBayonetUpdateDataFifoThrsh(SCSI_HHCB SCSI_HPTR);   
SCSI_UEXACT8 SCSIhGetCacheThEn(SCSI_HHCB SCSI_HPTR);
void SCSIhAICTridentUpdateCacheThEn(SCSI_HHCB SCSI_HPTR);
void SCSIhAICTridentGetProfileParameters(SCSI_HW_INFORMATION SCSI_LPTR, SCSI_HHCB SCSI_HPTR);
void SCSIhAICTridentPutProfileParameters(SCSI_HW_INFORMATION SCSI_LPTR, SCSI_HHCB SCSI_HPTR);
void SCSIhAICBayonetCheckExtArbAckEnBit(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8 SCSIhAICBayonetGetQNewTail(SCSI_HHCB SCSI_HPTR);
void SCSIhAICBayonetWaitForBusStable(SCSI_HHCB SCSI_HPTR);
void SCSIhAICBayonetSeqStretchWorkaround(SCSI_HHCB SCSI_HPTR);
void SCSIhAICBayonetIgnoreWideResCalc(SCSI_HHCB SCSI_HPTR, SCSI_HIOB SCSI_IPTR, SCSI_UEXACT8);
void SCSIhAICTridentResetScsiRate(SCSI_HHCB SCSI_HPTR);
void SCSIhAICTridentClearScsiRate(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT16 SCSIhAICTridentMaxSyncPeriod(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
#if SCSI_NEGOTIATION_PER_IOB
void SCSIhAICTridentChangeXferOption(SCSI_HIOB SCSI_IPTR);
#endif
SCSI_HARDWARE_DESCRIPTOR SCSIHardwareAICTrident = 
{
   SCSIhAICTridentLoadScsiRate,        /* load scsi rate                */
   SCSIhAICBayonetResetDFifo,          /* reset dfifo                   */
   SCSIhAICBayonetEnableIOErr,         /* enable IO Error               */
   SCSIhAICBayonetGetScsiOffset,       /* SCSI wide offset              */
   SCSIhAICBayonetGetScsiOffset,       /* SCSI narrow offset            */
   SCSIhAICTridentGetXferPeriod,       /* get xfer period               */
   SCSIhAICTridentCalcScsiRate,        /* calc scsi rate                */
   SCSIhAICBayonetClearScsiOffset,     /* clear scsi offset             */
   SCSIhAICBayonetWasSyncXfer,         /* was sync xfer                 */
#if SCSI_AUTO_TERMINATION
   SCSIhAICBayonetProcessAutoTerm,     /* process auto termination      */
#else
   0,                                  /* process auto termination      */
#endif
   SCSIhAICBayonetUpdateExtTerm,       /* update external termination   */
   SCSIhAICBayonetGetScbRamCtrls,      /* get SCB RAM control bits      */
   SCSIhAICBayonetPutScbRamCtrls,      /* put SCB RAM control bits      */
   SCSIhAICBayonetSetDataFifoThrshDefault,/* set data fifo default      */  
   SCSIhAICBayonetReadDataFifoThrsh,   /* read data fifo threshold      */
   SCSIhAICBayonetUpdateDataFifoThrsh, /* update data fifo threshold    */   
   SCSIhGetCacheThEn,                  /* get cachethen                 */
   SCSIhAICTridentUpdateCacheThEn,     /* update cachethen              */
   SCSIhAICTridentGetProfileParameters,/* get profile parameters        */
   SCSIhAICTridentPutProfileParameters,/* put profile parameters        */
   SCSIhAICBayonetCheckExtArbAckEnBit, /* check external arbitration enable bit */
   SCSIhAICBayonetGetQNewTail,         /* get q_new_tail value          */
   SCSIhAICBayonetWaitForBusStable,    /* wait for SCSI bus stable      */
#if (SCSI_STANDARD_160M_MODE || SCSI_SWAPPING_160M_MODE)
   0,                                  /* sequencer stretch workaround  */
#else
   SCSIhAICBayonetSeqStretchWorkaround,/* sequencer stretch workaround  */
#endif
   SCSIhAICBayonetIgnoreWideResCalc,   /* ignore wide residue calculation */
   0,                                  /* load TARGIDL/H registers      */
   SCSIhAICTridentResetScsiRate,       /* reset scsi rate/offset table  */
   SCSIhAICTridentClearScsiRate,       /* clear scsi rate/offset        */
   SCSIhAICTridentMaxSyncPeriod,       /* max xfer period to go sync xfer */
#if SCSI_NEGOTIATION_PER_IOB
   SCSIhAICTridentChangeXferOption,    /* Change xfer option form IOB   */
#endif
   SCSI_BAYONET_RAMPSM,                /* SCB RAM Present bit           */
   SCSI_BAYONET_INTSCBRAMSEL,          /* SCB RAM Select bit            */
   SCSI_BAYONET_SCBSIZE32,             /* SCB RAM Size bit              */
#if (SCSI_STANDARD_160M_MODE || SCSI_SWAPPING_160M_MODE)
   SCSI_TRIDENT_SEQCODE_SIZE,          /* Sequencer ram size            */
#else
   SCSI_BAYONET_SEQCODE_SIZE,          /* Sequencer ram size            */
#endif
   SCSI_BAYONET_CACHETHEEN_DEFAULT,    /* default cachethen             */
   SCSI_BAYONET_EXPACTIVE_DEFAULT,     /* default cachethen             */
   SCSI_BAYONET_SCSIID,                /* scsi id                       */
   SCSI_DELAY_FACTOR_20MIPS            /* delay factor                  */
};
#endif /* SCSI_AICTRIDENT && defined(SCSI_DATA_DEFINE) */


/***************************************************************************
*  SCSI_HARDWARE_DESCRIPTOR table for indexing
***************************************************************************/
#define  SCSI_MAX_HW    4
#if  defined(SCSI_DATA_DEFINE)
SCSI_HARDWARE_DESCRIPTOR SCSI_LPTR SCSIHardware[SCSI_MAX_HW] =
{
#if SCSI_AIC78XX
   &SCSIHardwareAIC78XX,
#else  /* SCSI_AIC78XX */
   (SCSI_HARDWARE_DESCRIPTOR SCSI_LPTR) 0,
#endif /* SCSI_AIC78XX */
#if SCSI_AICBAYONET
   &SCSIHardwareAICBayonet,
#else  /* !SCSI_AICBAYONET */
   (SCSI_HARDWARE_DESCRIPTOR SCSI_LPTR) 0,
#endif /* SCSI_AICBAYONET */
#if SCSI_AICTRIDENT
   &SCSIHardwareAICTrident,
#else  /* !SCSI_AICTRIDENT */
   (SCSI_HARDWARE_DESCRIPTOR SCSI_LPTR) 0,
#endif /* SCSI_AICTRIDENT */
   (SCSI_HARDWARE_DESCRIPTOR SCSI_LPTR) 0
};
#else
extern SCSI_HARDWARE_DESCRIPTOR SCSI_LPTR SCSIHardware[SCSI_MAX_HW];
#endif
#endif

/*$Header:   Y:/source/chimscsi/src/himscsi/hwmscsi/FIRMEQU.HV_   1.43.2.11   30 Mar 1998 10:01:40   MCGE1234  $*/
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
*  Module Name:   FIRMEQU.H
*
*  Description:   Definitions for firmware equates
*    
*  Owners:        ECX IC Firmware Team
*    
*  Notes:         1. This file is referenced by hardware management layer
*                    only
*
***************************************************************************/

/* define selected modes */
#define SCSI_MULTI_MODE (SCSI_STANDARD64_MODE+SCSI_STANDARD32_MODE+\
                         SCSI_SWAPPING64_MODE+SCSI_SWAPPING32_MODE+\
                         SCSI_STANDARD_ADVANCED_MODE+\
                         SCSI_SWAPPING_ADVANCED_MODE+\
                         SCSI_STANDARD_160M_MODE+\
                         SCSI_SWAPPING_160M_MODE) > 1

#define SCSI_NEW_MODE   (SCSI_STANDARD64_MODE+SCSI_STANDARD32_MODE+\
                         SCSI_SWAPPING64_MODE+SCSI_SWAPPING32_MODE+\
                         SCSI_STANDARD_ADVANCED_MODE+\
                         SCSI_SWAPPING_ADVANCED_MODE+\
                         SCSI_STANDARD_160M_MODE+\
                         SCSI_SWAPPING_160M_MODE)

#define SCSI_STANDARD_MODE (SCSI_STANDARD64_MODE+SCSI_STANDARD32_MODE+\
                            SCSI_STANDARD_ADVANCED_MODE+\
                            SCSI_STANDARD_160M_MODE)

#define SCSI_SWAPPING_MODE (SCSI_SWAPPING64_MODE+SCSI_SWAPPING32_MODE+\
                            SCSI_SWAPPING_ADVANCED_MODE+\
                            SCSI_SWAPPING_160M_MODE)

/****************************************************************************
* Version number
****************************************************************************/
#define  SCSI_SOFTWARE_VERSION   0

/****************************************************************************
* Miscellaneous
****************************************************************************/
#define  SCSI_NOT_APPLIED       -1             /* entry not applied       */
#define  SCSI_NULL_SCB          ((SCSI_UEXACT8) -1)   /* null scb pointer        */
#define  SCSI_NEEDNEGO          0x8f           /* need negotiation ind    */
#define  SCSI_WIDE_XFER         0x80           /* wide xfer ind           */
#define  SCSI_SYNC_XFER         0x01           /* sync xfer ind           */
#define  SCSI_SYNC_RATE         0x70           /* sync rate mask          */
#if SCSI_TARGET_OPERATION       
#define  SCSI_FAST20_THRESHOLD  0x12           /* fast 20 threshold       */
#else
#define  SCSI_FAST20_THRESHOLD  0x19           /* fast 20 threshold       */
#endif  /* SCSI_TARGET_OPERATION */
#define  SCSI_NULL_ENTRY        0xff           /* invalid entry indicator */

#define  ENTRY_IDLE_LOOP        0x01           /* bit to set/clear idle loop break point */
#define  ENTRY_EXPANDER_BREAK   0x02           /* bit to set/clear expander break break point */

#if SCSI_STANDARD64_MODE
/****************************************************************************
* Definitions for standard 64 mode firmware
* This section must be checked whenever there is any sequencer code update
****************************************************************************/
#define SCSI_S64_SIZE_SCB              64
#define SCSI_S64_ALIGNMENT_SCB         0x003F
#define SCSI_S64_FIRMWARE_VERSION      0
#define SCSI_S64_BTATABLE              0x00DF
#define SCSI_S64_RESCNT_BASE           0x00A0

#define SCSI_S64_SEQUENCER_PROGRAM     0x0000
#define SCSI_S64_IDLE_LOOP_ENTRY       0x0000
#define SCSI_S64_START_LINK_CMD_ENTRY  0x0004
#define SCSI_S64_SIOSTR3_ENTRY         0x0008
#define SCSI_S64_SIO204_ENTRY          0x000c
#define SCSI_S64_ATN_TMR_ENTRY         0x0010
#define SCSI_S64_IDLE_LOOP             0x002c
#define SCSI_S64_IDLE_LOOP0            0x0030
#if SCSI_TARGET_OPERATION   
/* Sequencer entry points specific to target mode */     
#define SCSI_S64_TARGET_DATA_ENTRY     SCSI_NULL_ENTRY    /* Not used */ 
#define SCSI_S64_TARGET_HELD_BUS_ENTRY SCSI_NULL_ENTRY    /* Not used */
#endif  /* SCSI_TARGET_OPERATION */
#define SCSI_S64_XFER_OPTION           0x0020
#define SCSI_S64_DISCON_OPTION         0x0030
#define SCSI_S64_PASS_TO_DRIVER        0x0033
#define SCSI_S64_WAITING_SCB           0x0034
#define SCSI_S64_ACTIVE_SCB            0x0035
#define SCSI_S64_Q_NEW_HEAD            0x003a
#define SCSI_S64_Q_NEW_TAIL            0x003b
#define SCSI_S64_Q_NEW_POINTER         0x003c
#define SCSI_S64_Q_NEXT_SCB            0x0044
#define SCSI_S64_Q_EXE_HEAD            0x005e
#define SCSI_S64_Q_EXE_TAIL            0x005f
#define SCSI_S64_Q_DONE_ELEMENT        0x004c
#define SCSI_S64_Q_DONE_PASS           0x004d
#define SCSI_S64_Q_DONE_BASE           0x004e
#define SCSI_S64_Q_DONE_HEAD           0x0070
#define SCSI_S64_FAST20_LOW            0x0076
#define SCSI_S64_FAST20_HIGH           0x0077
#define SCSI_S64_Q_MASK                0x0078
#define SCSI_S64_MWI_CACHE_MASK        0x0079
#define SCSI_S64_ENT_PT_BITMAP         SCSI_NULL_ENTRY
#define SCSI_S64_AUTO_RATE_ENABLE      SCSI_NULL_ENTRY
#if SCSI_TRIDENT_PROTO
#define SCSI_S64_SCSISEQ_COPY          0x0000      /* TLUU - trident debug */
#endif /* SCSI_TRIDENT_PROTO */
#define SCSI_S64_INTR_THRSH_TIMEOUT1   SCSI_NULL_ENTRY
#define SCSI_S64_INTR_THRSH_TIMEOUT2   SCSI_NULL_ENTRY
#define SCSI_S64_CHECK_ABORT           0x0000      /* flag sequencer to check abort in scontrol */
#define SCSI_S64_MAX_NONTAG_SCBS       2    

#if (OSD_BUS_ADDRESS_SIZE == 64)
#define SCSI_S64_SIO215                0x040c
#else
#define SCSI_S64_SIO215                0x03f0
#endif
#define SCSI_S64_TARG_LUN_MASK0        SCSI_NULL_ENTRY   /* future for RAID */ 
#define SCSI_S64_ARRAY_PARTITION0      SCSI_NULL_ENTRY   /* future for RAID */
#define SCSI_S64_EXPANDER_BREAK        SCSI_NULL_ENTRY   /* not for Excalibur */
#if SCSI_TARGET_OPERATION
/* Scratch RAM entries specific to target mode */
#define SCSI_S64_Q_EST_HEAD            SCSI_NULL_ENTRY   /* Not used */
#define SCSI_S64_Q_EST_TAIL            SCSI_NULL_ENTRY   /* Not used */
#define SCSI_S64_Q_EST_NEXT_SCB        SCSI_NULL_ENTRY   /* Not used */
#define SCSI_S64_HESCB_QOFF            SCSI_NULL_ENTRY   /* Not used */
#define SCSI_S64_SESCB_QOFF            SCSI_NULL_ENTRY   /* Not used */
#endif /* SCSI_TARGET_OPERATION */
#endif

/****************************************************************************
* Definitions for standard 32 mode firmware
* This section must be checked whenever there is any sequencer code update
****************************************************************************/
#if SCSI_STANDARD32_MODE
#endif


#if SCSI_SWAPPING_MODE
/****************************************************************************
* Definitions for swapping mode firmware
* This section must be checked whenever there is any sequencer code update
* These macros are shared by both swapping 32 and swapping 64.
****************************************************************************/
#define SCSI_W_PASS_TO_DRIVER        0x0055
#define SCSI_W_WAITING_SCB           0x0056
#define SCSI_W_ACTIVE_SCB            0x0057
#define SCSI_W_Q_NEXT_SCB            0x0040
#define SCSI_W_Q_NEW_HEAD            0x0058
#define SCSI_W_Q_NEW_TAIL            0x0059
#define SCSI_W_Q_DONE_BASE           0x0048
#define SCSI_W_Q_DONE_POINTER        0x004C
#define SCSI_W_Q_DONE_ELEMENT        0x005A
#define SCSI_W_Q_DONE_PASS           0x005B
#define SCSI_W_Q_MASK                0x005C
#define SCSI_W_MWI_CACHE_MASK        0x005E
#define SCSI_W_MAX_NONTAG_SCBS       1
/*#define SCSI_W_TARG_LUN_MASK0        SCSI_NULL_ENTRY*/   /* future for RAID */
/*#define SCSI_W_ARRAY_PARTITION0      SCSI_NULL_ENTRY*/   /* future for RAID */
/*#define SCSI_W_SCBBTA                SCSI_NULL_ENTRY*/   /* Not used */
/*#define SCSI_W_DISCON_OPTION         SCSI_NULL_ENTRY*/   /* Not used */
/*#define SCSI_W_XFER_OPTION           SCSI_NULL_ENTRY*/   /* Not used */
/*#define SCSI_W_Q_EXE_HEAD            SCSI_NULL_ENTRY*/   /* Not used */
/*#define SCSI_W_Q_DONE_HEAD           SCSI_NULL_ENTRY*/   /* Not used */
/*#define SCSI_W_FAST20_LOW            SCSI_NULL_ENTRY*/   /* Not used */
/*#define SCSI_W_FAST20_HIGH           SCSI_NULL_ENTRY*/   /* Not used */
/*#define SCSI_W_EXPANDER_BREAK        SCSI_NULL_ENTRY*/   /* Not used */
#if SCSI_TARGET_OPERATION
/* Scratch RAM entries specific to target mode */
#define SCSI_W_Q_EST_HEAD            SCSI_NULL_ENTRY       /* Not used */
#define SCSI_W_Q_EST_TAIL            SCSI_NULL_ENTRY       /* Not used */
#define SCSI_W_Q_EST_NEXT_SCB        SCSI_NULL_ENTRY       /* Not used */
#define SCSI_W_HESCB_QOFF            SCSI_NULL_ENTRY       /* Not used */
#define SCSI_W_SESCB_QOFF            SCSI_NULL_ENTRY       /* Not used */
#endif /* SCSI_TARGET_OPERATION */
/* definitions unique to swapping modes */
#define SCSI_W_BUSY_TARGET_SCRATCH   0x20        
#define SCSI_W_SCB_PTR_SCRATCH       0x50
#endif

#if SCSI_SWAPPING64_MODE
/****************************************************************************
* Definitions for swapping 64 mode firmware
* This section must be checked whenever there is any sequencer code update
****************************************************************************/
#define SCSI_W64_SIZE_SCB              64
#define SCSI_W64_ALIGNMENT_SCB         0x003F
#define SCSI_W64_FIRMWARE_VERSION      0
#define SCSI_W64_RESCNT_BASE           0x00A0

#define SCSI_W64_SEQUENCER_PROGRAM     0x0000
#define SCSI_W64_IDLE_LOOP_ENTRY       0x0000
#define SCSI_W64_IDLE_LOOP0            SCSI_NULL_ENTRY   /* Not used */
#define SCSI_W64_START_LINK_CMD_ENTRY  0x0004
#define SCSI_W64_SIOSTR3_ENTRY         0x0008
#define SCSI_W64_SIO204_ENTRY          0x000C
#define SCSI_W64_ATN_TMR_ENTRY         0x0010
#define SCSI_W64_SIO215                0x02ac
#if SCSI_TARGET_OPERATION  
/* Sequencer entry points specific to target mode */      
#define SCSI_W64_TARGET_DATA_ENTRY     SCSI_NULL_ENTRY   /* Not used */
#define SCSI_W64_TARGET_HELD_BUS_ENTRY SCSI_NULL_ENTRY   /* Not used */
#endif  /* SCSI_TARGET_OPERATION */
#define SCSI_W64_PASS_TO_DRIVER        SCSI_W_PASS_TO_DRIVER
#define SCSI_W64_WAITING_SCB           SCSI_W_WAITING_SCB   
#define SCSI_W64_ACTIVE_SCB            SCSI_W_ACTIVE_SCB    
#define SCSI_W64_Q_NEXT_SCB            SCSI_W_Q_NEXT_SCB
#define SCSI_W64_Q_NEW_HEAD            SCSI_W_Q_NEW_HEAD    
#define SCSI_W64_Q_NEW_TAIL            SCSI_W_Q_NEW_TAIL    
#define SCSI_W64_Q_DONE_BASE           SCSI_W_Q_DONE_BASE   
#define SCSI_W64_Q_DONE_POINTER        SCSI_W_Q_DONE_POINTER
#define SCSI_W64_Q_DONE_ELEMENT        SCSI_W_Q_DONE_ELEMENT
#define SCSI_W64_Q_DONE_PASS           SCSI_W_Q_DONE_PASS   
#define SCSI_W64_Q_MASK                SCSI_W_Q_MASK        
#define SCSI_W64_MWI_CACHE_MASK        SCSI_W_MWI_CACHE_MASK
#define SCSI_W64_ENT_PT_BITMAP         SCSI_NULL_ENTRY   
#define SCSI_W64_AUTO_RATE_ENABLE      SCSI_NULL_ENTRY
#if SCSI_TRIDENT_PROTO
#define SCSI_W64_SCSISEQ_COPY          0x0000      /* TLUU - trident debug */
#endif /* SCSI_TRIDENT_PROTO */
#define SCSI_W64_INTR_THRSH_TIMEOUT1   SCSI_NULL_ENTRY
#define SCSI_W64_INTR_THRSH_TIMEOUT2   SCSI_NULL_ENTRY
#define SCSI_W64_CHECK_ABORT           0x0000      /* flag sequencer to check abort in scontrol */
#define SCSI_W64_TARG_LUN_MASK0        SCSI_NULL_ENTRY   /* future for RAID */
#define SCSI_W64_ARRAY_PARTITION0      SCSI_NULL_ENTRY   /* future for RAID */
#define SCSI_W64_BTATABLE              0x0020            /* Not used */
#define SCSI_W64_DISCON_OPTION         SCSI_NULL_ENTRY   /* Not used */
#define SCSI_W64_XFER_OPTION           SCSI_NULL_ENTRY   /* Not used */
#define SCSI_W64_Q_EXE_HEAD            SCSI_NULL_ENTRY   /* Not used */
#define SCSI_W64_Q_DONE_HEAD           SCSI_NULL_ENTRY   /* Not used */
#define SCSI_W64_FAST20_LOW            SCSI_NULL_ENTRY   /* Not used */
#define SCSI_W64_FAST20_HIGH           SCSI_NULL_ENTRY   /* Not used */
#define SCSI_W64_EXPANDER_BREAK        SCSI_NULL_ENTRY   /* Not used */
#define SCSI_W64_MAX_NONTAG_SCBS       SCSI_W_MAX_NONTAG_SCBS     
#if SCSI_TARGET_OPERATION
/* Scratch RAM entries specific to target mode */
#define SCSI_W64_Q_EST_HEAD            SCSI_NULL_ENTRY   /* Not used */
#define SCSI_W64_Q_EST_TAIL            SCSI_NULL_ENTRY   /* Not used */     
#define SCSI_W64_Q_EST_NEXT_SCB        SCSI_NULL_ENTRY   /* Not used */   
#define SCSI_W64_HESCB_QOFF            SCSI_NULL_ENTRY   /* Not used */
#define SCSI_W64_SESCB_QOFF            SCSI_NULL_ENTRY   /* Not used */
#endif /* SCSI_TARGET_OPERATION */

/* definitions unique to swapping 64 mode */
#define SCSI_W64_BUSY_TARGET_SCRATCH   SCSI_W_BUSY_TARGET_SCRATCH        
#define SCSI_W64_SCB_PTR_SCRATCH       SCSI_W_SCB_PTR_SCRATCH    
#endif



#if SCSI_SWAPPING32_MODE
/****************************************************************************
* Definitions for swapping 32 mode firmware
* This section must be checked whenever there is any sequencer code update
****************************************************************************/
#define SCSI_W32_SIZE_SCB              32
#define SCSI_W32_ALIGNMENT_SCB         0x001F
#define SCSI_W32_FIRMWARE_VERSION      0
#define SCSI_W32_RESCNT_BASE           0x00A8

#define SCSI_W32_SEQUENCER_PROGRAM     0x0000
#define SCSI_W32_IDLE_LOOP_ENTRY       0x0000
#define SCSI_W32_IDLE_LOOP0            SCSI_NULL_ENTRY   /* Not used */
#define SCSI_W32_START_LINK_CMD_ENTRY  0x0004
#define SCSI_W32_SIOSTR3_ENTRY         0x0008
#define SCSI_W32_SIO204_ENTRY          0x000C
#define SCSI_W32_ATN_TMR_ENTRY         0x0010
#define SCSI_W32_SIO215                0x0298
#if SCSI_TARGET_OPERATION     
/* Sequencer entry points specific to target mode */   
#define SCSI_W32_TARGET_DATA_ENTRY     SCSI_NULL_ENTRY   /* Not used */
#define SCSI_W32_TARGET_HELD_BUS_ENTRY SCSI_NULL_ENTRY   /* Not used */
#endif  /* SCSI_TARGET_OPERATION */ 

#define SCSI_W32_PASS_TO_DRIVER        SCSI_W_PASS_TO_DRIVER
#define SCSI_W32_WAITING_SCB           SCSI_W_WAITING_SCB   
#define SCSI_W32_ACTIVE_SCB            SCSI_W_ACTIVE_SCB    
#define SCSI_W32_Q_NEXT_SCB            SCSI_W_Q_NEXT_SCB     
#define SCSI_W32_Q_NEW_HEAD            SCSI_W_Q_NEW_HEAD    
#define SCSI_W32_Q_NEW_TAIL            SCSI_W_Q_NEW_TAIL    
#define SCSI_W32_Q_DONE_BASE           SCSI_W_Q_DONE_BASE   
#define SCSI_W32_Q_DONE_POINTER        SCSI_W_Q_DONE_POINTER
#define SCSI_W32_Q_DONE_ELEMENT        SCSI_W_Q_DONE_ELEMENT
#define SCSI_W32_Q_DONE_PASS           SCSI_W_Q_DONE_PASS   
#define SCSI_W32_Q_MASK                SCSI_W_Q_MASK        
#define SCSI_W32_MWI_CACHE_MASK        SCSI_W_MWI_CACHE_MASK
#define SCSI_W32_ENT_PT_BITMAP         SCSI_NULL_ENTRY   
#define SCSI_W32_AUTO_RATE_ENABLE      SCSI_NULL_ENTRY
#if SCSI_TRIDENT_PROTO
#define SCSI_W32_SCSISEQ_COPY          0x0000      /* TLUU - trident debug */
#endif /* SCSI_TRIDENT_PROTO */
#define SCSI_W32_INTR_THRSH_TIMEOUT1   SCSI_NULL_ENTRY
#define SCSI_W32_INTR_THRSH_TIMEOUT2   SCSI_NULL_ENTRY
#define SCSI_W32_CHECK_ABORT           0x0000      /* flag sequencer to check abort in scontrol */


#define SCSI_W32_TARG_LUN_MASK0        SCSI_NULL_ENTRY   /* future for RAID */
#define SCSI_W32_ARRAY_PARTITION0      SCSI_NULL_ENTRY   /* future for RAID */
#define SCSI_W32_BTATABLE              0x0020            /* Not used */
#define SCSI_W32_DISCON_OPTION         SCSI_NULL_ENTRY   /* Not used */
#define SCSI_W32_XFER_OPTION           SCSI_NULL_ENTRY   /* Not used */
#define SCSI_W32_Q_EXE_HEAD            SCSI_NULL_ENTRY   /* Not used */
#define SCSI_W32_Q_DONE_HEAD           SCSI_NULL_ENTRY   /* Not used */
#define SCSI_W32_FAST20_LOW            SCSI_NULL_ENTRY   /* Not used */
#define SCSI_W32_FAST20_HIGH           SCSI_NULL_ENTRY   /* Not used */
#define SCSI_W32_EXPANDER_BREAK        SCSI_NULL_ENTRY   /* Not used */
#define SCSI_W32_MAX_NONTAG_SCBS       SCSI_W_MAX_NONTAG_SCBS     
#if SCSI_TARGET_OPERATION
/* Scratch RAM entries specific to target mode */
#define SCSI_W32_Q_EST_HEAD            SCSI_NULL_ENTRY   /* Not used */
#define SCSI_W32_Q_EST_TAIL            SCSI_NULL_ENTRY   /* Not used */
#define SCSI_W32_Q_EST_NEXT_SCB        SCSI_NULL_ENTRY   /* Not used */
#define SCSI_W32_HESCB_QOFF            SCSI_NULL_ENTRY   /* Not used */
#define SCSI_W32_SESCB_QOFF            SCSI_NULL_ENTRY   /* Not used */
#endif /* SCSI_TARGET_OPERATION */

/* definitions unique to swapping 32 mode */
#define SCSI_W32_BUSY_TARGET_SCRATCH   SCSI_W_BUSY_TARGET_SCRATCH        
#define SCSI_W32_SCB_PTR_SCRATCH       SCSI_W_SCB_PTR_SCRATCH    
#endif


#if SCSI_STANDARD_ADVANCED_MODE
/****************************************************************************
* Definitions for standard advanced mode firmware
* This section must be checked whenever there is any sequencer code update
****************************************************************************/
#define SCSI_SADV_SIZE_SCB             64
#define SCSI_SADV_ALIGNMENT_SCB        0x003F
#define SCSI_SADV_FIRMWARE_VERSION     0
#define SCSI_SADV_BTATABLE             0x00DF
#define SCSI_SADV_RESCNT_BASE          0x00B0
#define SCSI_SADV_ARB_HUNG             0x01

#define SCSI_SADV_SEQUENCER_PROGRAM    0x0000
#define SCSI_SADV_IDLE_LOOP_ENTRY      0x0000
#define SCSI_SADV_START_LINK_CMD_ENTRY 0x0004
#define SCSI_SADV_SIOSTR3_ENTRY        0x0008
#define SCSI_SADV_SIO204_ENTRY         0x000c
#define SCSI_SADV_ATN_TMR_ENTRY        0x0010
#define SCSI_SADV_SIO215               SCSI_NULL_ENTRY   /* Not used */
#define SCSI_SADV_IDLE_LOOP            0x002c
#define SCSI_SADV_DISCON_OPTION        SCSI_NULL_ENTRY
#define SCSI_SADV_IDLE_LOOP0           0x0020
#if SCSI_TARGET_OPERATION   
/* Sequencer entry points specific to target mode */     
#define SCSI_SADV_TARGET_DATA_ENTRY    SCSI_NULL_ENTRY   /* Not used */
#define SCSI_SADV_TARGET_HELD_BUS_ENTRY SCSI_NULL_ENTRY  /* Not used */
#endif  /* SCSI_TARGET_OPERATION */
#define SCSI_SADV_XFER_OPTION          SCSI_NULL_ENTRY   /* Not used */
#define SCSI_SADV_PASS_TO_DRIVER       0x0035
#define SCSI_SADV_WAITING_SCB          0x003C
#define SCSI_SADV_ACTIVE_SCB           0x003D
#define SCSI_SADV_Q_NEW_POINTER        0x0020
#define SCSI_SADV_Q_NEW_HEAD           SCSI_HNSCB_QOFF
#define SCSI_SADV_Q_NEW_TAIL           SCSI_SNSCB_QOFF
#define SCSI_SADV_Q_NEXT_SCB           0x003A
#define SCSI_SADV_Q_EXE_HEAD           0x0036 
#define SCSI_SADV_Q_EXE_TAIL           0x0037 
#define SCSI_SADV_Q_DONE_PASS          0x0034 
#define SCSI_SADV_Q_DONE_BASE          0x0028
#define SCSI_SADV_Q_DONE_POINTER       0x0030
#define SCSI_SADV_Q_DONE_ELEMENT       SCSI_NULL_ENTRY   /* Not used */
#define SCSI_SADV_Q_DONE_HEAD          0x0038
#define SCSI_SADV_FAST20_LOW           SCSI_NULL_ENTRY   /* Not used */
#define SCSI_SADV_FAST20_HIGH          SCSI_NULL_ENTRY   /* Not used */
#define SCSI_SADV_Q_MASK               SCSI_QOFF_CTLSTA
#define SCSI_SADV_MWI_CACHE_MASK       SCSI_NULL_ENTRY   /* Not used */
#define SCSI_SADV_ENT_PT_BITMAP        0x004A
#define SCSI_SADV_AUTO_RATE_ENABLE     SCSI_NULL_ENTRY
#if SCSI_TRIDENT_PROTO
#define SCSI_SADV_SCSISEQ_COPY         0x0000      /* TLUU - trident debug */
#endif /* SCSI_TRIDENT_PROTO */
#define SCSI_SADV_INTR_THRSH_TIMEOUT1  0x004B
#define SCSI_SADV_INTR_THRSH_TIMEOUT2  0x004C
#define SCSI_SADV_CHECK_ABORT          0x0000      /* flag sequencer to check abort in scontrol */
#define SCSI_SADV_WW_FLAG              0x004D
#define SCSI_SADV_MAX_NONTAG_SCBS      2     
#define SCSI_SADV_TARG_LUN_MASK0       SCSI_NULL_ENTRY   /* future for RAID */ 
#define SCSI_SADV_ARRAY_PARTITION0     SCSI_NULL_ENTRY   /* future for RAID */
#if (OSD_BUS_ADDRESS_SIZE == 32)
#define SCSI_SADV_EXPANDER_BREAK       0x02B0
#else
#define SCSI_SADV_EXPANDER_BREAK       0x02B8
#endif
#if SCSI_TARGET_OPERATION
/* Scratch RAM entries specific to target mode */
#define SCSI_SADV_Q_EST_HEAD           SCSI_NULL_ENTRY   /* Not used */
#define SCSI_SADV_Q_EST_TAIL           SCSI_NULL_ENTRY   /* Not used */
#define SCSI_SADV_Q_EST_NEXT_SCB       SCSI_NULL_ENTRY   /* Not used */
#define SCSI_SADV_HESCB_QOFF           SCSI_NULL_ENTRY   /* Not used */
#define SCSI_SADV_SESCB_QOFF           SCSI_NULL_ENTRY   /* Not used */
#endif /* SCSI_TARGET_OPERATION */

#endif /* SCSI_STANDARD_ADVANCED_MODE */

#if SCSI_SWAPPING_ADVANCED_MODE
/****************************************************************************
* Definitions for swapping advanced mode firmware
* This section must be checked whenever there is any sequencer code update
****************************************************************************/
#define SCSI_WADV_BUSY_TARGET_SCRATCH  0x70        
#define SCSI_WADV_SCB_PTR_SCRATCH      0x20

#define SCSI_WADV_SIZE_SCB             64
#define SCSI_WADV_ALIGNMENT_SCB        0x003F
#define SCSI_WADV_FIRMWARE_VERSION     0
#define SCSI_WADV_BTATABLE             0x0070           /* Not used */
#define SCSI_WADV_RESCNT_BASE          0x00B8
#define SCSI_WADV_ARB_HUNG             0x01

#define SCSI_WADV_SEQUENCER_PROGRAM    0x0000
#define SCSI_WADV_IDLE_LOOP_ENTRY      0x0000
#define SCSI_WADV_START_LINK_CMD_ENTRY 0x0004
#define SCSI_WADV_SIOSTR3_ENTRY        0x0008
#define SCSI_WADV_SIO204_ENTRY         0x000c
#define SCSI_WADV_ATN_TMR_ENTRY        0x0010
#define SCSI_WADV_SIO215               SCSI_NULL_ENTRY   /* Not used */
#if SCSI_TARGET_OPERATION
/* Sequencer entry points specific to target mode */     
#define SCSI_WADV_TARGET_DATA_ENTRY    0x0014   
#define SCSI_WADV_TARGET_HELD_BUS_ENTRY 0x0018
#endif /* SCSI_TARGET_OPERATION */

#if SCSI_TARGET_OPERATION
#define SCSI_WADV_IDLE_LOOP0           0x0028
#else
#if SCSI_TRIDENT_PROTO
#define SCSI_WADV_IDLE_LOOP0           0x0024
#else
#define SCSI_WADV_IDLE_LOOP0           0x0020            
#endif /* SCSI_TRIDENT_PROTO */
#endif /* SCSI_TARGET_OPERATION */

#define SCSI_WADV_DISCON_OPTION        SCSI_NULL_ENTRY   /* Not used */
#define SCSI_WADV_XFER_OPTION          SCSI_NULL_ENTRY   /* Not used */
#define SCSI_WADV_PASS_TO_DRIVER       0x0038
#define SCSI_WADV_WAITING_SCB          0x0039
#define SCSI_WADV_ACTIVE_SCB           0x003A
#define SCSI_WADV_Q_NEW_POINTER        SCSI_NULL_ENTRY   /* Not used */
#define SCSI_WADV_Q_NEW_HEAD           SCSI_HNSCB_QOFF
#define SCSI_WADV_Q_NEW_TAIL           SCSI_SNSCB_QOFF
#define SCSI_WADV_Q_NEXT_SCB           0x0028
#define SCSI_WADV_Q_EXE_HEAD           SCSI_NULL_ENTRY   /* Not used */ 
#define SCSI_WADV_Q_EXE_TAIL           SCSI_NULL_ENTRY   /* Not used */ 
#define SCSI_WADV_Q_DONE_PASS          0x0035
#define SCSI_WADV_Q_DONE_BASE          0x0029
#define SCSI_WADV_Q_DONE_ELEMENT       SCSI_NULL_ENTRY   /* Not used */
#define SCSI_WADV_Q_DONE_POINTER       0x0031
#define SCSI_WADV_Q_DONE_HEAD          SCSI_NULL_ENTRY   /* Not used */
#define SCSI_WADV_FAST20_LOW           SCSI_NULL_ENTRY   /* Not used */
#define SCSI_WADV_FAST20_HIGH          SCSI_NULL_ENTRY   /* Not used */
#define SCSI_WADV_Q_MASK               SCSI_QOFF_CTLSTA
#define SCSI_WADV_MWI_CACHE_MASK       SCSI_NULL_ENTRY   /* Not used */
#define SCSI_WADV_ENT_PT_BITMAP        0x0049
#define SCSI_WADV_AUTO_RATE_ENABLE     SCSI_NULL_ENTRY
#if SCSI_TRIDENT_PROTO
#define SCSI_WADV_SCSISEQ_COPY         0x004A      /* TLUU - trident debug */
#endif /* SCSI_TRIDENT_PROTO */

#define SCSI_WADV_INTR_THRSH_TIMEOUT1  0x004B
#define SCSI_WADV_INTR_THRSH_TIMEOUT2  0x004C
#define SCSI_WADV_CHECK_ABORT          0x0000      /* flag sequencer to check abort in scontrol */
#define SCSI_WADV_WW_FLAG              0x004D
#define SCSI_WADV_MAX_NONTAG_SCBS      1     

#define SCSI_WADV_TARG_LUN_MASK0       SCSI_NULL_ENTRY   /* future for RAID */ 
#define SCSI_WADV_ARRAY_PARTITION0     SCSI_NULL_ENTRY   /* future for RAID */

#if SCSI_TARGET_OPERATION
#if (OSD_BUS_ADDRESS_SIZE == 32)
#define SCSI_WADV_EXPANDER_BREAK       0x0270
#else
#define SCSI_WADV_EXPANDER_BREAK       0x0278
#endif
#else
#if SCSI_TRIDENT_PROTO
#if (OSD_BUS_ADDRESS_SIZE == 32)
#define SCSI_WADV_EXPANDER_BREAK       0x028C
#else
#define SCSI_WADV_EXPANDER_BREAK       0x0270
#endif
#else /* !SCSI_TRIDENT_PROTO */
#if (OSD_BUS_ADDRESS_SIZE == 32)
#define SCSI_WADV_EXPANDER_BREAK       0x0278
#else
#define SCSI_WADV_EXPANDER_BREAK       0x0294
#endif
#endif /* SCSI_TRIDENT_PROTO */
#endif /* SCSI_TARGET_OPERATION */

#if SCSI_TARGET_OPERATION
/* Scratch RAM entries specific to target mode */
#if SCSI_MAILBOX_ENABLE
#define SCSI_WADV_Q_EST_HEAD           SCSI_BAYONET_MAILBOX
#else
#define SCSI_WADV_Q_EST_HEAD           SCSI_WADV_HESCB_QOFF /* host est scb queue offset */
#endif /* SCSI_MAILBOX_ENABLE */
#define SCSI_WADV_Q_EST_TAIL           SCSI_WADV_SESCB_QOFF /* sequencer est scb queue 
                                                             * offset
                                                             */
#define SCSI_WADV_Q_EST_NEXT_SCB       0x0050            /* first est scb to be delivered */
#define SCSI_WADV_HESCB_QOFF           0x004F            /* host establish scb queue
                                                          * offset
                                                          */
#define SCSI_WADV_SESCB_QOFF           0x004E            /* sequencer establish scb queue
                                                          * offset
                                                          */
/* other sequencer globals required by target mode */
#define SCSI_WADV_SCB_SITE0            0x00              /* One of two scb sram sites
                                                          * used for reestablish and
                                                          * initiator scbs.
                                                          */
#define SCSI_WADV_SCB_SITE1            0x01              /* The other scb sram site used
                                                          * for reestablish and initiator
                                                          * scbs.
                                                          */ 
#define SCSI_WADV_EST_SCB_SITE         0x07              /* A special scb site reserved
                                                          * for establish connection scb 
                                                          * processing. The establish 
                                                          * connection scb is stored in
                                                          * this site prior to DMAing to
                                                          * the host.
                                                          */
#endif /* SCSI_TARGET_OPERATION */

#endif /* SCSI_SWAPPING_ADVANCED_MODE */

#if SCSI_STANDARD_160M_MODE
/****************************************************************************
* Definitions for standard Ultra 160m mode firmware
* This section must be checked whenever there is any sequencer code update
****************************************************************************/
#define SCSI_S160M_SIZE_SCB              64
#define SCSI_S160M_ALIGNMENT_SCB         0x003F
#define SCSI_S160M_FIRMWARE_VERSION      0
#define SCSI_S160M_BTATABLE              0x00DF
#define SCSI_S160M_RESCNT_BASE           0x00B0
#define SCSI_S160M_SEQUENCER_PROGRAM     0x0000
#define SCSI_S160M_IDLE_LOOP_ENTRY       0x0000
#define SCSI_S160M_START_LINK_CMD_ENTRY  0x0004
#define SCSI_S160M_SIOSTR3_ENTRY         0x0008
#define SCSI_S160M_SIO204_ENTRY          0x000C
#define SCSI_S160M_ATN_TMR_ENTRY         0x0010
#define SCSI_S160M_SIO215                SCSI_NULL_ENTRY   /* Not used */
#define SCSI_S160M_IDLE_LOOP             SCSI_NULL_ENTRY   /* Not used */
#define SCSI_S160M_DISCON_OPTION         SCSI_NULL_ENTRY   /* Not used */
#define SCSI_S160M_IDLE_LOOP0            0x0018
#if SCSI_TARGET_OPERATION   
/* Sequencer entry points specific to target mode */     
#define SCSI_S160M_TARGET_DATA_ENTRY     SCSI_NULL_ENTRY   /* Not used */ 
#define SCSI_S160M_TARGET_HELD_BUS_ENTRY SCSI_NULL_ENTRY   /* Not used */
#endif  /* SCSI_TARGET_OPERATION */

#define SCSI_S160M_XFER_OPTION           SCSI_NULL_ENTRY   /* Not used */
#define SCSI_S160M_PASS_TO_DRIVER        0x0035
#define SCSI_S160M_SCSIDATL_IMAGE        SCSI_SCSIDATL_IMG
#define SCSI_S160M_WAITING_SCB           0x003C
#define SCSI_S160M_ACTIVE_SCB            0x003D
#define SCSI_S160M_Q_NEW_POINTER         0x0020
#define SCSI_S160M_Q_NEW_HEAD            SCSI_HNSCB_QOFF
#define SCSI_S160M_Q_NEW_TAIL            SCSI_SNSCB_QOFF
#define SCSI_S160M_Q_NEXT_SCB            0x003A            /* corresponding to array site */
#define SCSI_S160M_Q_EXE_HEAD            0x0036 
#define SCSI_S160M_Q_EXE_TAIL            0x0037 
#define SCSI_S160M_Q_DONE_PASS           0x0034 
#define SCSI_S160M_Q_DONE_BASE           0x0028
#define SCSI_S160M_Q_DONE_POINTER        0x0030
#define SCSI_S160M_Q_DONE_ELEMENT        SCSI_NULL_ENTRY   /* Not used */
#define SCSI_S160M_Q_DONE_HEAD           0x0038
#define SCSI_S160M_FAST20_LOW            SCSI_NULL_ENTRY   /* Not used */
#define SCSI_S160M_FAST20_HIGH           SCSI_NULL_ENTRY   /* Not used */
#define SCSI_S160M_Q_MASK                SCSI_QOFF_CTLSTA
#define SCSI_S160M_MWI_CACHE_MASK        SCSI_NULL_ENTRY   /* Not used */

#define SCSI_S160M_ENT_PT_BITMAP         0x004D            
#define SCSI_S160M_AUTO_RATE_ENABLE      0x004A
#if SCSI_TRIDENT_PROTO
#define SCSI_S160M_SCSISEQ_COPY          0x0000      /* TLUU - trident debug */
#endif /* SCSI_TRIDENT_PROTO */
#define SCSI_S160M_INTR_THRSH_TIMEOUT1   0x004B
#define SCSI_S160M_INTR_THRSH_TIMEOUT2   0x004C
#define SCSI_S160M_CHECK_ABORT           0x0000      /* flag sequencer to check abort in scontrol */
#define SCSI_S160M_WW_FLAG               SCSI_NULL_ENTRY   /* Not used */
#define SCSI_S160M_MAX_NONTAG_SCBS       2

#define SCSI_S160M_TARG_LUN_MASK0        SCSI_NULL_ENTRY   /* future for RAID */ 
#define SCSI_S160M_ARRAY_PARTITION0      SCSI_NULL_ENTRY   /* future for RAID */

#if (OSD_BUS_ADDRESS_SIZE == 32)
#define SCSI_S160M_EXPANDER_BREAK        0x0248
#else
#define SCSI_S160M_EXPANDER_BREAK        0x0250
#endif

#if SCSI_TARGET_OPERATION   
/* Scratch RAM entries specific to target mode */
#define SCSI_S160M_Q_EST_HEAD            SCSI_NULL_ENTRY   /* Not used */
#define SCSI_S160M_Q_EST_TAIL            SCSI_NULL_ENTRY   /* Not used */
#define SCSI_S160M_Q_EST_NEXT_SCB        SCSI_NULL_ENTRY   /* Not used */
#define SCSI_S160M_HESCB_QOFF            SCSI_NULL_ENTRY   /* Not used */
#define SCSI_S160M_SESCB_QOFF            SCSI_NULL_ENTRY   /* Not used */
#endif /* SCSI_TARGET_OPERATION */

#endif /* SCSI_STANDARD_160M_MODE */

#if SCSI_SWAPPING_160M_MODE
/****************************************************************************
* Definitions for swapping U160m mode firmware
* This section must be checked whenever there is any sequencer code update
****************************************************************************/



#define SCSI_W160M_BUSY_TARGET_SCRATCH   0x70        
#define SCSI_W160M_SCB_PTR_SCRATCH       0x20

#define SCSI_W160M_SIZE_SCB              64
#define SCSI_W160M_ALIGNMENT_SCB         0x003F
#define SCSI_W160M_FIRMWARE_VERSION      0
#define SCSI_W160M_BTATABLE              0x0070            /* Not used */
#define SCSI_W160M_RESCNT_BASE           0x00B8

#define SCSI_W160M_SEQUENCER_PROGRAM     0x0000
#define SCSI_W160M_IDLE_LOOP_ENTRY       0x0000
#define SCSI_W160M_START_LINK_CMD_ENTRY  0x0004
#define SCSI_W160M_SIOSTR3_ENTRY         0x0008
#define SCSI_W160M_SIO204_ENTRY          0x000c
#define SCSI_W160M_ATN_TMR_ENTRY         0x0010
#define SCSI_W160M_SIO215                SCSI_NULL_ENTRY   /* Not used */

#if SCSI_TARGET_OPERATION
#define SCSI_W160M_IDLE_LOOP0            0x0020
#else
#define SCSI_W160M_IDLE_LOOP0            0x0018
#endif /* SCSI_TARGET_OPERATION */

#if SCSI_TARGET_OPERATION   
/* Sequencer entry points specific to target mode */     
#define SCSI_W160M_TARGET_DATA_ENTRY     0x0014
#define SCSI_W160M_TARGET_HELD_BUS_ENTRY 0x0018
#endif /* SCSI_TARGET_OPERATION */

#define SCSI_W160M_DISCON_OPTION         SCSI_NULL_ENTRY   /* Not used */
#define SCSI_W160M_XFER_OPTION           SCSI_NULL_ENTRY   /* Not used */
#define SCSI_W160M_PASS_TO_DRIVER        0x0038
#define SCSI_W160M_SCSIDATL_IMAGE        SCSI_SCSIDATL_IMG
#define SCSI_W160M_WAITING_SCB           0x0039
#define SCSI_W160M_ACTIVE_SCB            0x003A
#define SCSI_W160M_Q_NEW_POINTER         SCSI_NULL_ENTRY   /* Not used */
#define SCSI_W160M_Q_NEW_HEAD            SCSI_HNSCB_QOFF
#define SCSI_W160M_Q_NEW_TAIL            SCSI_SNSCB_QOFF
#define SCSI_W160M_Q_NEXT_SCB            0x0028
#define SCSI_W160M_Q_EXE_HEAD            SCSI_NULL_ENTRY   /* Not used */ 
#define SCSI_W160M_Q_EXE_TAIL            SCSI_NULL_ENTRY   /* Not used */ 
#define SCSI_W160M_Q_DONE_PASS           0x0035
#define SCSI_W160M_Q_DONE_BASE           0x0029
#define SCSI_W160M_Q_DONE_ELEMENT        SCSI_NULL_ENTRY   /* Not used */
#define SCSI_W160M_Q_DONE_POINTER        0x0031
#define SCSI_W160M_Q_DONE_HEAD           SCSI_NULL_ENTRY   /* Not used */
#define SCSI_W160M_FAST20_LOW            SCSI_NULL_ENTRY   /* Not used */
#define SCSI_W160M_FAST20_HIGH           SCSI_NULL_ENTRY   /* Not used */
#define SCSI_W160M_Q_MASK                SCSI_QOFF_CTLSTA
#define SCSI_W160M_MWI_CACHE_MASK        SCSI_NULL_ENTRY   /* Not used */
#define SCSI_W160M_ENT_PT_BITMAP         0x004C
#define SCSI_W160M_AUTO_RATE_ENABLE      0x0049


#if SCSI_TRIDENT_PROTO
#define SCSI_W160M_SCSISEQ_COPY          0x0000      /* TLUU - trident debug */
#endif /* SCSI_TRIDENT_PROTO */
#define SCSI_W160M_INTR_THRSH_TIMEOUT1   0x004A
#define SCSI_W160M_INTR_THRSH_TIMEOUT2   0x004B

#if SCSI_TARGET_OPERATION
#define SCSI_W160M_CHECK_ABORT           0x0056      /* flag sequencer to check abort in scontrol */
#else
#define SCSI_W160M_CHECK_ABORT           0x004D      /* flag sequencer to check abort in scontrol */
#endif
#define SCSI_W160M_WW_FLAG               SCSI_NULL_ENTRY   /* Not used */
#define SCSI_W160M_MAX_NONTAG_SCBS       1     

#define SCSI_W160M_TARG_LUN_MASK0        SCSI_NULL_ENTRY   /* future for RAID */ 
#define SCSI_W160M_ARRAY_PARTITION0      SCSI_NULL_ENTRY   /* future for RAID */

#if SCSI_TARGET_OPERATION

#if (OSD_BUS_ADDRESS_SIZE == 32)
#define SCSI_W160M_EXPANDER_BREAK        0x0284
#else
#define SCSI_W160M_EXPANDER_BREAK        0x028C
#endif
#else
#if (OSD_BUS_ADDRESS_SIZE == 32)
#define SCSI_W160M_EXPANDER_BREAK        0x0250
#else
#define SCSI_W160M_EXPANDER_BREAK        0x0258
#endif
#endif /* SCSI_TARGET_OPERATION */

#if SCSI_TARGET_OPERATION
/* Scratch RAM entries specific to target mode */
#define SCSI_W160M_Q_EST_HEAD            SCSI_W160M_HESCB_QOFF   /* host est scb
                                                                  * queue offset
                                                                  */
#define SCSI_W160M_Q_EST_TAIL            SCSI_W160M_SESCB_QOFF   /* sequencer est
                                                                  * scb queue offset
                                                                  */
#define SCSI_W160M_Q_EST_NEXT_SCB        0x004F            /* first est scb to be delivered */
#define SCSI_W160M_HESCB_QOFF            0x004E            /* host establish scb
                                                            * queue offset
                                                            */
#define SCSI_W160M_SESCB_QOFF            0x004D            /* sequencer establish scb queue
                                                            * offset
                                                            */
#define SCSI_W160M_TARGET_FLAG           0x0050            /* a scratch RAM location for
                                                            * target sequencer usage
                                                            */
/* other sequencer globals required by target mode */
#define SCSI_W160M_SCB_SITE0             0x00              /* One of two scb sram sites
                                                            * used for reestablish and
                                                            * initiator scbs.
                                                            */
#define SCSI_W160M_SCB_SITE1             0x01              /* The other scb sram site used
                                                            * for reestablish and initiator
                                                            * scbs.
                                                            */
#define SCSI_W160M_EST_SCB_SITE          0x07              /* A special scb site reserved
                                                            * for establish connection scb
                                                            * processing. The establish
                                                            * connection scb is stored in
                                                            * this site prior to DMAing to
                                                            * the host.
                                                            */
#endif /* SCSI_TARGET_OPERATION */

#endif /* SCSI_SWAPPING_160M_MODE */

/*$Header:   Y:/source/chimscsi/src/himscsi/hwmscsi/FIRMDEF.HV_   1.54.2.7   07 Apr 1998 20:50:34   THU  $*/
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
*  Module Name:   FIRMDEF.H
*
*  Description:   Definitions for firmware dependent structures 
*    
*  Owners:        ECX IC Firmware Team
*    
*  Notes:         1. Definitions for SCB layout
*                 2. Depending on hardware or mode of operation the layout
*                    of SCB may be different. The layout is defined in
*                    hardware management layer. It's translation management
*                    layer's responsibility to make sure SCB information
*                    get filled properly.
*                 3. Modify CUSTOM.H to enable/disable a particular scb
*                    format
*
***************************************************************************/

#if defined(SCSI_DATA_DEFINE)
#if SCSI_STANDARD64_MODE
#include "seq_s64.h"
#endif
#if SCSI_STANDARD32_MODE
#include "seq_s32.h"
#endif
#if SCSI_SWAPPING64_MODE
#include "seq_w64.h"
#endif
#if SCSI_SWAPPING32_MODE
#include "seq_w32.h"
#endif
#if SCSI_STANDARD_ADVANCED_MODE
#include "seq_sadv.h"
#endif
#if SCSI_SWAPPING_ADVANCED_MODE
#include "seq_wadv.h"
#endif
#if SCSI_STANDARD_160M_MODE
#include "seqs160m.h"
#endif
#if SCSI_SWAPPING_160M_MODE
#include "seqw160m.h"
#endif
#if SCSI_DOWNSHIFT_MODE
#include "seq_dwn.h"
#endif
#endif

/***************************************************************************
*  Standard 64 bytes SCB format layout
***************************************************************************/
#if SCSI_STANDARD64_MODE

/* structure definition */
typedef struct SCSI_SCB_STANDARD64_
{
   union
   {
      SCSI_UEXACT8    scdb[12];            /* SCSI CDB                      */
      struct 
      {
         SCSI_UEXACT8     scdb0;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb1;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb2;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb3;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb4;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb5;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb6;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb7;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb8;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb9;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb10;              /* SCSI CDB                      */
         SCSI_UEXACT8     scdb11;              /* SCSI CDB                      */
      } scbCDB;
      SCSI_UEXACT8 scdb_pointer[8];            /* cdb pointer                   */
      struct 
      {
         SCSI_UEXACT8     scdb_pointer0;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer1;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer2;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer3;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer4;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer5;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer6;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer7;       /* cdb pointer                   */   
      } scbCDBPTR;
      struct 
      {
         SCSI_UEXACT8     sdata_residue0;      /* total # of bytes not xfer     */
         SCSI_UEXACT8     sdata_residue1;      /* total # of bytes not xfer     */
         SCSI_UEXACT8     sdata_residue2;      /* total # of bytes not xfer     */
         SCSI_UEXACT8     sdata_residue3;      /* total # of bytes not xfer     */
         SCSI_UEXACT8     sg_flags_work;       /* working flags                 */
         SCSI_UEXACT8     sg_cache_work;       /* working cache                 */
         SCSI_UEXACT8     sg_pointer_work0;    /* working s/g list pointer      */
         SCSI_UEXACT8     sg_pointer_work1;    /* working s/g list pointer      */
         SCSI_UEXACT8     sg_pointer_work2;    /* working s/g list pointer      */
         SCSI_UEXACT8     sg_pointer_work3;    /* working s/g list pointer      */
      } scbResidualAndWork;
   } scb0to11;

   SCSI_UEXACT8     scontrol;                  /* scb flags                     */
   SCSI_UEXACT8     starget;                   /* target                        */
   SCSI_UEXACT8     scdb_length;               /* cdb length                    */
   union 
   {
      struct 
      {
         SCSI_UEXACT8     sg_cache_SCB;        /* # of SCSI_UEXACT8s for each element   */
         SCSI_UEXACT8     sg_pointer_SCB0;     /* pointer to s/g list           */
         SCSI_UEXACT8     sg_pointer_SCB1;     /* pointer to s/g list           */
         SCSI_UEXACT8     sg_pointer_SCB2;     /* pointer to s/g list           */
         SCSI_UEXACT8     sg_pointer_SCB3;     /* pointer to s/g list           */
         SCSI_UEXACT8     sg_pointer_SCB4;     /* pointer to s/g list           */
         SCSI_UEXACT8     sg_pointer_SCB5;     /* pointer to s/g list           */
         SCSI_UEXACT8     sg_pointer_SCB6;     /* pointer to s/g list           */
         SCSI_UEXACT8     sg_pointer_SCB7;     /* pointer to s/g list           */
      } scbSG;
      struct 
      {
         SCSI_UEXACT8     special_opcode;      /* opcode for special scb func   */
         SCSI_UEXACT8     special_info;        /* special info                  */
      } scbSpecial;
   } scb15to23;

#if (OSD_BUS_ADDRESS_SIZE == 64)
   SCSI_UEXACT8     saddress0;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress1;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress2;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress3;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress4;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress5;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress6;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress7;                 /* 1st s/g segment               */
   SCSI_UEXACT8     slength0;                  /* 1st s/g segment               */
   SCSI_UEXACT8     slength1;                  /* 1st s/g segment               */
   SCSI_UEXACT8     slength2;                  /* 1st s/g segment               */
#else
   SCSI_UEXACT8     saddress0;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress1;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress2;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress3;                 /* 1st s/g segment               */
   SCSI_UEXACT8     slength0;                  /* 1st s/g segment               */
   SCSI_UEXACT8     slength1;                  /* 1st s/g segment               */
   SCSI_UEXACT8     slength2;                  /* 1st s/g segment               */
   SCSI_UEXACT8     padding0;                  /* 1st s/g segment               */
   SCSI_UEXACT8     padding1;                  /* 1st s/g segment               */
   SCSI_UEXACT8     padding2;                  /* 1st s/g segment               */
   SCSI_UEXACT8     padding3;                  /* 1st s/g segment               */
#endif /* OSD_BUS_ADDRESS_SIZE */

   SCSI_UEXACT8     chain_control;             /* chain control                 */
   SCSI_UEXACT8     next_SCB;                  /* pointer to next chained scb   */
   SCSI_UEXACT8     offshoot;                  /* pointer to offshoot scb       */
   union
   {
      SCSI_UEXACT8     q_next;                 /* point to next for execution   */
      SCSI_UEXACT8     array_site_next;             /* SCB array # (SCB destination) */
   } scb38;
   SCSI_UEXACT8     slun;                      /* lun                           */
   SCSI_UEXACT8     next_SCB_addr0;            /* next SCB address              */
   SCSI_UEXACT8     next_SCB_addr1;            /* next SCB address              */
   SCSI_UEXACT8     next_SCB_addr2;            /* next SCB address              */
   SCSI_UEXACT8     next_SCB_addr3;            /* next SCB address              */
   SCSI_UEXACT8     next_SCB_addr4;            /* next SCB address              */
   SCSI_UEXACT8     next_SCB_addr5;            /* next SCB address              */
   SCSI_UEXACT8     next_SCB_addr6;            /* next SCB address              */
   SCSI_UEXACT8     next_SCB_addr7;            /* next SCB address              */
   SCSI_UEXACT8     reserved[14];              /* reserved                      */
   SCSI_UEXACT8     xfer_opt;                  /* save xfer option that uses during reselection */
   SCSI_UEXACT8     busy_target;               /* busy target                   */
} SCSI_SCB_STANDARD64;

#define  SCSI_S64_sg_cache_SCB              scb15to23.scbSG.sg_cache_SCB
#define  SCSI_S64_sg_pointer_SCB0           scb15to23.scbSG.sg_pointer_SCB0
#define  SCSI_S64_sg_pointer_SCB1           scb15to23.scbSG.sg_pointer_SCB1
#define  SCSI_S64_sg_pointer_SCB2           scb15to23.scbSG.sg_pointer_SCB2
#define  SCSI_S64_sg_pointer_SCB3           scb15to23.scbSG.sg_pointer_SCB3
#define  SCSI_S64_sg_pointer_SCB4           scb15to23.scbSG.sg_pointer_SCB4
#define  SCSI_S64_sg_pointer_SCB5           scb15to23.scbSG.sg_pointer_SCB5
#define  SCSI_S64_sg_pointer_SCB6           scb15to23.scbSG.sg_pointer_SCB6
#define  SCSI_S64_sg_pointer_SCB7           scb15to23.scbSG.sg_pointer_SCB7

#define  SCSI_S64_special_opcode            scb15to23.scbSpecial.special_opcode
#define  SCSI_S64_special_info              scb15to23.scbSpecial.special_info

#define  SCSI_S64_scdb                      scb0to11.scdb
#define  SCSI_S64_scdb0                     scb0to11.scbCDB.scdb0 
#define  SCSI_S64_scdb1                     scb0to11.scbCDB.scdb1
#define  SCSI_S64_scdb2                     scb0to11.scbCDB.scdb2
#define  SCSI_S64_scdb3                     scb0to11.scbCDB.scdb3
#define  SCSI_S64_scdb4                     scb0to11.scbCDB.scdb4
#define  SCSI_S64_scdb5                     scb0to11.scbCDB.scdb5
#define  SCSI_S64_scdb6                     scb0to11.scbCDB.scdb6
#define  SCSI_S64_scdb7                     scb0to11.scbCDB.scdb7
#define  SCSI_S64_scdb8                     scb0to11.scbCDB.scdb8
#define  SCSI_S64_scdb9                     scb0to11.scbCDB.scdb9
#define  SCSI_S64_scdb10                    scb0to11.scbCDB.scdb10
#define  SCSI_S64_scdb11                    scb0to11.scbCDB.scdb11

#define  SCSI_S64_scdb_pointer              scb0to11.scdb_pointer

#define  SCSI_S64_scdb_pointer0             scb0to11.scbCDBPTR.scdb_pointer0
#define  SCSI_S64_scdb_pointer1             scb0to11.scbCDBPTR.scdb_pointer1
#define  SCSI_S64_scdb_pointer2             scb0to11.scbCDBPTR.scdb_pointer2
#define  SCSI_S64_scdb_pointer3             scb0to11.scbCDBPTR.scdb_pointer3
#define  SCSI_S64_scdb_pointer4             scb0to11.scbCDBPTR.scdb_pointer4
#define  SCSI_S64_scdb_pointer5             scb0to11.scbCDBPTR.scdb_pointer5
#define  SCSI_S64_scdb_pointer6             scb0to11.scbCDBPTR.scdb_pointer6
#define  SCSI_S64_scdb_pointer7             scb0to11.scbCDBPTR.scdb_pointer7

#define  SCSI_S64_sdata_residue0            scb0to11.scbResidualAndWork.sdata_residue0
#define  SCSI_S64_sdata_residue1            scb0to11.scbResidualAndWork.sdata_residue1
#define  SCSI_S64_sdata_residue2            scb0to11.scbResidualAndWork.sdata_residue2
#define  SCSI_S64_sdata_residue3            scb0to11.scbResidualAndWork.sdata_residue3
#define  SCSI_S64_sg_flags_work             scb0to11.scbResidualAndWork.sg_flags_work  
#define  SCSI_S64_sg_cache_work             scb0to11.scbResidualAndWork.sg_cache_work  
#define  SCSI_S64_sg_pointer_work0          scb0to11.scbResidualAndWork.sg_pointer_work0
#define  SCSI_S64_sg_pointer_work1          scb0to11.scbResidualAndWork.sg_pointer_work1
#define  SCSI_S64_sg_pointer_work2          scb0to11.scbResidualAndWork.sg_pointer_work2
#define  SCSI_S64_sg_pointer_work3          scb0to11.scbResidualAndWork.sg_pointer_work3

#define  SCSI_S64_mwiResidual               SCSI_S64_scdb11

#define  SCSI_S64_q_next                    scb38.q_next
#define  SCSI_S64_array_site_next           scb38.array_site_next

/* definitions for scontrol   */
#define  SCSI_S64_SIMPLETAG  0x00           /* simple tag                 */
#define  SCSI_S64_HEADTAG    0x01           /* head of queue tag          */
#define  SCSI_S64_ORDERTAG   0x02           /* ordered tag                */
#define  SCSI_S64_SPECFUN    0x08           /* special function           */
#define  SCSI_S64_TAGENB     0x20           /* tag enable                 */
#define  SCSI_S64_DISCENB    0x40           /* disconnect enable          */
#define  SCSI_S64_TAGMASK    0x03           /* mask for tags              */

/* definitions dor starg_lun */
#define  SCSI_S64_TARGETID   0xF0           /* targte id                  */
#define  SCSI_S64_TARGETLUN  0x07           /* target lun                 */

/* definitions for scdb_length */
#define  SCSI_S64_CDBLENMSK  0x1F           /* CDB length                 */
#define  SCSI_S64_NODATA     0x40           /* no data transfer involoved */
#define  SCSI_S64_ONESGSEG   0x80           /* only one sg segment        */

/* definitions for chain_control flags */
#define  SCSI_S64_HOLDOFF    0x01           /* hold off flag              */
#define  SCSI_S64_CONCURRENT 0x02           /* concurrent flag            */
#define  SCSI_S64_ABORTED    0x08           /* aborted flag               */
#define  SCSI_S64_PROGMASK   0xF0           /* masm for progress count    */

/* definitions for special function opcode */
#define  SCSI_S64_MSGTOTARG  0x00           /* special opcode, msg_to_targ */
#endif

/***************************************************************************
*  Standard 32 bytes SCB format layout
***************************************************************************/
#if SCSI_STANDARD32_MODE
#endif


#if SCSI_SWAPPING_MODE
#define  SCSI_W_FAST20           0x04
#define  SCSI_W_THRESHOLD        30
#endif


/***************************************************************************
*  Swapping 64 bytes SCB format layout
***************************************************************************/
#if SCSI_SWAPPING64_MODE

/* structure definition */
typedef struct SCSI_SCB_SWAPPING64_
{
   union
   {
      SCSI_UEXACT8    scdb[12];            /* SCSI CDB                      */
      struct 
      {
         SCSI_UEXACT8     scdb0;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb1;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb2;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb3;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb4;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb5;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb6;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb7;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb8;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb9;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb10;              /* SCSI CDB                      */
         SCSI_UEXACT8     scdb11;              /* SCSI CDB                      */
      } scbCDB;
      SCSI_UEXACT8 scdb_pointer[8];            /* cdb pointer                   */
      struct 
      {
         SCSI_UEXACT8     scdb_pointer0;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer1;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer2;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer3;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer4;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer5;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer6;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer7;       /* cdb pointer                   */   
      } scbCDBPTR;
      struct 
      {
         SCSI_UEXACT8     sdata_residue0;      /* total # of bytes not xfer     */
         SCSI_UEXACT8     sdata_residue1;      /* total # of bytes not xfer     */
         SCSI_UEXACT8     sdata_residue2;      /* total # of bytes not xfer     */
         SCSI_UEXACT8     sdata_residue3;      /* total # of bytes not xfer     */
         SCSI_UEXACT8     sg_flags_work;       /* working flags                 */
         SCSI_UEXACT8     sg_pointer_work0;    /* working s/g list pointer      */
         SCSI_UEXACT8     sg_pointer_work1;    /* working s/g list pointer      */
         SCSI_UEXACT8     sg_pointer_work2;    /* working s/g list pointer      */
         SCSI_UEXACT8     sg_pointer_work3;    /* working s/g list pointer      */
         SCSI_UEXACT8     sg_pointer_work4;    /* working s/g list pointer      */
         SCSI_UEXACT8     sg_pointer_work5;    /* working s/g list pointer      */
         SCSI_UEXACT8     sg_pointer_work6;    /* working s/g list pointer      */
         SCSI_UEXACT8     sg_pointer_work7;    /* working s/g list pointer      */
      } scbResidualAndWork;
   } scb0to12;
   SCSI_UEXACT8     starget;                   /* target                        */
   SCSI_UEXACT8     slun;                      /* lun                           */
   SCSI_UEXACT8     scdb_length;               /* cdb length                    */
   union 
   {
      struct 
      {
         SCSI_UEXACT8     saddress0;           /* 1st s/g segment               */
         SCSI_UEXACT8     saddress1;           /* 1st s/g segment               */
         SCSI_UEXACT8     saddress2;           /* 1st s/g segment               */
         SCSI_UEXACT8     saddress3;           /* 1st s/g segment               */
         SCSI_UEXACT8     saddress4;           /* 1st s/g segment               */
         SCSI_UEXACT8     saddress5;           /* 1st s/g segment               */
         SCSI_UEXACT8     saddress6;           /* 1st s/g segment               */
         SCSI_UEXACT8     saddress7;           /* 1st s/g segment               */
         SCSI_UEXACT8     slength0;            /* 1st s/g segment               */
         SCSI_UEXACT8     slength1;            /* 1st s/g segment               */
         SCSI_UEXACT8     slength2;            /* 1st s/g segment               */
      } scbSGElement;
      struct 
      {
         SCSI_UEXACT8     special_opcode;      /* opcode for special scb func   */
         SCSI_UEXACT8     special_info;        /* special info                  */
      } scbSpecial;
   } scb16to27;
   SCSI_UEXACT8     next_SCB;                  /* next SCB number               */
   SCSI_UEXACT8     padding2;                  /* reserved                      */
   SCSI_UEXACT8     xfer_option;               /* SCSI transfer option          */
   SCSI_UEXACT8     scontrol;                  /* scb flags                     */
   SCSI_UEXACT8     chain_control;             /* chain control                 */
   SCSI_UEXACT8     sg_pointer_SCB0;           /* pointer to s/g list           */
   SCSI_UEXACT8     sg_pointer_SCB1;           /* pointer to s/g list           */
   SCSI_UEXACT8     sg_pointer_SCB2;           /* pointer to s/g list           */
   SCSI_UEXACT8     sg_pointer_SCB3;           /* pointer to s/g list           */
   SCSI_UEXACT8     sg_pointer_SCB4;           /* pointer to s/g list           */
   SCSI_UEXACT8     sg_pointer_SCB5;           /* pointer to s/g list           */
   SCSI_UEXACT8     sg_pointer_SCB6;           /* pointer to s/g list           */
   SCSI_UEXACT8     sg_pointer_SCB7;           /* pointer to s/g list           */
} SCSI_SCB_SWAPPING64;

#define  SCSI_W64_scdb                      scb0to12.scdb
#define  SCSI_W64_scdb0                     scb0to12.scbCDB.scdb0 
#define  SCSI_W64_scdb1                     scb0to12.scbCDB.scdb1
#define  SCSI_W64_scdb2                     scb0to12.scbCDB.scdb2
#define  SCSI_W64_scdb3                     scb0to12.scbCDB.scdb3
#define  SCSI_W64_scdb4                     scb0to12.scbCDB.scdb4
#define  SCSI_W64_scdb5                     scb0to12.scbCDB.scdb5
#define  SCSI_W64_scdb6                     scb0to12.scbCDB.scdb6
#define  SCSI_W64_scdb7                     scb0to12.scbCDB.scdb7
#define  SCSI_W64_scdb8                     scb0to12.scbCDB.scdb8
#define  SCSI_W64_scdb9                     scb0to12.scbCDB.scdb9
#define  SCSI_W64_scdb10                    scb0to12.scbCDB.scdb10
#define  SCSI_W64_scdb11                    scb0to12.scbCDB.scdb11

#define  SCSI_W64_scdb_pointer              scb0to12.scdb_pointer

#define  SCSI_W64_scdb_pointer0             scb0to12.scbCDBPTR.scdb_pointer0
#define  SCSI_W64_scdb_pointer1             scb0to12.scbCDBPTR.scdb_pointer1
#define  SCSI_W64_scdb_pointer2             scb0to12.scbCDBPTR.scdb_pointer2
#define  SCSI_W64_scdb_pointer3             scb0to12.scbCDBPTR.scdb_pointer3
#define  SCSI_W64_scdb_pointer4             scb0to12.scbCDBPTR.scdb_pointer0
#define  SCSI_W64_scdb_pointer5             scb0to12.scbCDBPTR.scdb_pointer1
#define  SCSI_W64_scdb_pointer6             scb0to12.scbCDBPTR.scdb_pointer2
#define  SCSI_W64_scdb_pointer7             scb0to12.scbCDBPTR.scdb_pointer3

#define  SCSI_W64_sdata_residue0            scb0to12.scbResidualAndWork.sdata_residue0
#define  SCSI_W64_sdata_residue1            scb0to12.scbResidualAndWork.sdata_residue1
#define  SCSI_W64_sdata_residue2            scb0to12.scbResidualAndWork.sdata_residue2
#define  SCSI_W64_sdata_residue3            scb0to12.scbResidualAndWork.sdata_residue3
#define  SCSI_W64_sg_flags_work             scb0to12.scbResidualAndWork.sg_flags_work  
#define  SCSI_W64_sg_pointer_work0          scb0to12.scbResidualAndWork.sg_pointer_work0
#define  SCSI_W64_sg_pointer_work1          scb0to12.scbResidualAndWork.sg_pointer_work1
#define  SCSI_W64_sg_pointer_work2          scb0to12.scbResidualAndWork.sg_pointer_work2
#define  SCSI_W64_sg_pointer_work3          scb0to12.scbResidualAndWork.sg_pointer_work3
#define  SCSI_W64_sg_pointer_work4          scb0to12.scbResidualAndWork.sg_pointer_work4
#define  SCSI_W64_sg_pointer_work5          scb0to12.scbResidualAndWork.sg_pointer_work5
#define  SCSI_W64_sg_pointer_work6          scb0to12.scbResidualAndWork.sg_pointer_work6
#define  SCSI_W64_sg_pointer_work7          scb0to12.scbResidualAndWork.sg_pointer_work7

#define  SCSI_W64_mwiResidual               padding2

#define  SCSI_W64_saddress0                 scb16to27.scbSGElement.saddress0
#define  SCSI_W64_saddress1                 scb16to27.scbSGElement.saddress1
#define  SCSI_W64_saddress2                 scb16to27.scbSGElement.saddress2
#define  SCSI_W64_saddress3                 scb16to27.scbSGElement.saddress3
#define  SCSI_W64_saddress4                 scb16to27.scbSGElement.saddress4
#define  SCSI_W64_saddress5                 scb16to27.scbSGElement.saddress5
#define  SCSI_W64_saddress6                 scb16to27.scbSGElement.saddress6
#define  SCSI_W64_saddress7                 scb16to27.scbSGElement.saddress7
#define  SCSI_W64_slength0                  scb16to27.scbSGElement.slength0
#define  SCSI_W64_slength1                  scb16to27.scbSGElement.slength1
#define  SCSI_W64_slength2                  scb16to27.scbSGElement.slength2

#define  SCSI_W64_special_opcode            scb16to27.scbSpecial.special_opcode
#define  SCSI_W64_special_info              scb16to27.scbSpecial.special_info

#define  SCSI_W64_sg_pointer_SCB0           sg_pointer_SCB0
#define  SCSI_W64_sg_pointer_SCB1           sg_pointer_SCB1
#define  SCSI_W64_sg_pointer_SCB2           sg_pointer_SCB2
#define  SCSI_W64_sg_pointer_SCB3           sg_pointer_SCB3
#define  SCSI_W64_sg_pointer_SCB4           sg_pointer_SCB4
#define  SCSI_W64_sg_pointer_SCB5           sg_pointer_SCB5
#define  SCSI_W64_sg_pointer_SCB6           sg_pointer_SCB6
#define  SCSI_W64_sg_pointer_SCB7           sg_pointer_SCB7


/* definitions for scontrol   */
#define  SCSI_W64_SIMPLETAG  0x00           /* simple tag                 */
#define  SCSI_W64_HEADTAG    0x01           /* head of queue tag          */
#define  SCSI_W64_ORDERTAG   0x02           /* ordered tag                */
#define  SCSI_W64_FAST20     SCSI_W_FAST20  /* fast20 enable              */
#define  SCSI_W64_SPECFUN    0x08           /* special function           */
#define  SCSI_W64_TAGENB     0x20           /* tag enable                 */
#define  SCSI_W64_DISCENB    0x40           /* disconnect enable          */
#define  SCSI_W64_TAGMASK    0x03           /* mask for tags              */

/* definitions dor starg_lun */
#define  SCSI_W64_TARGETID   0x0F           /* targte id                  */
#define  SCSI_W64_TARGETLUN  0x3F           /* target lun                 */

/* definitions for slun */
#define  SCSI_W64_MWI_FIX    0x80           /* mwi fix has been done      */

/* definitions for scdb_length */
#define  SCSI_W64_CDBLENMSK  0x1F           /* CDB length                 */
#define  SCSI_W64_NODATA     0x40           /* no data transfer involoved */
#define  SCSI_W64_ONESGSEG   0x80           /* only one sg segment        */

/* definitions for chain_control flags */
#define  SCSI_W64_HOLDOFF    0x01           /* hold off flag              */
#define  SCSI_W64_CONCURRENT 0x02           /* concurrent flag            */
#define  SCSI_W64_ABORTED    0x08           /* aborted flag               */
#define  SCSI_W64_PROGMASK   0xF0           /* masm for progress count    */
#define  SCSI_W64_THRESHOLD  SCSI_W_THRESHOLD /* threshold for scb manage   */

/* definitions for special function opcode */
#define  SCSI_W64_MSGTOTARG  0x00           /* special opcode, msg_to_targ */

#endif

/***************************************************************************
*  Swapping 32 bytes SCB format layout
***************************************************************************/
#if SCSI_SWAPPING32_MODE

/* structure definition */
typedef struct SCSI_SCB_SWAPPING32_
{
   SCSI_UEXACT8     chain_control;             /* chain control                 */
   SCSI_UEXACT8     scontrol;                  /* scb flags                     */
   SCSI_UEXACT8     starget;                   /* target                        */
   SCSI_UEXACT8     scdb_length;               /* cdb length                    */
   union 
   {
      struct 
      {
         SCSI_UEXACT8     sg_pointer_SCB0;     /* pointer to s/g list           */
         SCSI_UEXACT8     sg_pointer_SCB1;     /* pointer to s/g list           */
         SCSI_UEXACT8     sg_pointer_SCB2;     /* pointer to s/g list           */
         SCSI_UEXACT8     sg_pointer_SCB3;     /* pointer to s/g list           */
      } scbSG;
      struct 
      {
         SCSI_UEXACT8     special_opcode;      /* opcode for special scb func   */
         SCSI_UEXACT8     special_info;        /* special info                  */
      } scbSpecial;
   } scb4to7;
   union
   {
      SCSI_UEXACT8    scdb[12];            /* SCSI CDB                      */
      struct 
      {
         SCSI_UEXACT8     scdb0;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb1;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb2;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb3;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb4;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb5;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb6;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb7;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb8;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb9;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb10;              /* SCSI CDB                      */
         SCSI_UEXACT8     scdb11;              /* SCSI CDB                      */
      } scbCDB;
      SCSI_UEXACT8 scdb_pointer[4];            /* cdb pointer                   */
      struct 
      {
         SCSI_UEXACT8     scdb_pointer0;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer1;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer2;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer3;       /* cdb pointer                   */   
      } scbCDBPTR;
      struct 
      {
         SCSI_UEXACT8     sdata_residue0;      /* total # of bytes not xfer     */
         SCSI_UEXACT8     sdata_residue1;      /* total # of bytes not xfer     */
         SCSI_UEXACT8     sdata_residue2;      /* total # of bytes not xfer     */
         SCSI_UEXACT8     sdata_residue3;      /* total # of bytes not xfer     */
         SCSI_UEXACT8     sg_flags_work;       /* working flags                 */
         SCSI_UEXACT8     sg_pointer_work0;    /* working s/g list pointer      */
         SCSI_UEXACT8     sg_pointer_work1;    /* working s/g list pointer      */
         SCSI_UEXACT8     sg_pointer_work2;    /* working s/g list pointer      */
         SCSI_UEXACT8     sg_pointer_work3;    /* working s/g list pointer      */
      } scbResidualAndWork;
   } scb8to19;

   SCSI_UEXACT8     saddress0;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress1;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress2;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress3;                 /* 1st s/g segment               */
   SCSI_UEXACT8     slength0;                  /* 1st s/g segment               */
   SCSI_UEXACT8     slength1;                  /* 1st s/g segment               */
   SCSI_UEXACT8     slength2;                  /* 1st s/g segment               */
   SCSI_UEXACT8     padding0;                  /* reserved                      */
   SCSI_UEXACT8     next_SCB;                  /* next SCB number               */
   SCSI_UEXACT8     xfer_option;               /* SCSI transfer option          */
   SCSI_UEXACT8     padding2;                  /* 1st s/g segment               */
   SCSI_UEXACT8     slun;                      /* lun                           */
} SCSI_SCB_SWAPPING32;

#define  SCSI_W32_sg_pointer_SCB0           scb4to7.scbSG.sg_pointer_SCB0
#define  SCSI_W32_sg_pointer_SCB1           scb4to7.scbSG.sg_pointer_SCB1
#define  SCSI_W32_sg_pointer_SCB2           scb4to7.scbSG.sg_pointer_SCB2
#define  SCSI_W32_sg_pointer_SCB3           scb4to7.scbSG.sg_pointer_SCB3

#define  SCSI_W32_special_opcode            scb4to7.scbSpecial.special_opcode
#define  SCSI_W32_special_info              scb4to7.scbSpecial.special_info

#define  SCSI_W32_scdb                      scb8to19.scdb
#define  SCSI_W32_scdb0                     scb8to19.scbCDB.scdb0 
#define  SCSI_W32_scdb1                     scb8to19.scbCDB.scdb1
#define  SCSI_W32_scdb2                     scb8to19.scbCDB.scdb2
#define  SCSI_W32_scdb3                     scb8to19.scbCDB.scdb3
#define  SCSI_W32_scdb4                     scb8to19.scbCDB.scdb4
#define  SCSI_W32_scdb5                     scb8to19.scbCDB.scdb5
#define  SCSI_W32_scdb6                     scb8to19.scbCDB.scdb6
#define  SCSI_W32_scdb7                     scb8to19.scbCDB.scdb7
#define  SCSI_W32_scdb8                     scb8to19.scbCDB.scdb8
#define  SCSI_W32_scdb9                     scb8to19.scbCDB.scdb9
#define  SCSI_W32_scdb10                    scb8to19.scbCDB.scdb10
#define  SCSI_W32_scdb11                    scb8to19.scbCDB.scdb11

#define  SCSI_W32_scdb_pointer              scb8to19.scdb_pointer

#define  SCSI_W32_scdb_pointer0             scb8to19.scbCDBPTR.scdb_pointer0
#define  SCSI_W32_scdb_pointer1             scb8to19.scbCDBPTR.scdb_pointer1
#define  SCSI_W32_scdb_pointer2             scb8to19.scbCDBPTR.scdb_pointer2
#define  SCSI_W32_scdb_pointer3             scb8to19.scbCDBPTR.scdb_pointer3

#define  SCSI_W32_sdata_residue0            scb8to19.scbResidualAndWork.sdata_residue0
#define  SCSI_W32_sdata_residue1            scb8to19.scbResidualAndWork.sdata_residue1
#define  SCSI_W32_sdata_residue2            scb8to19.scbResidualAndWork.sdata_residue2
#define  SCSI_W32_sdata_residue3            scb8to19.scbResidualAndWork.sdata_residue3
#define  SCSI_W32_sg_flags_work             scb8to19.scbResidualAndWork.sg_flags_work  
#define  SCSI_W32_sg_pointer_work0          scb8to19.scbResidualAndWork.sg_pointer_work0
#define  SCSI_W32_sg_pointer_work1          scb8to19.scbResidualAndWork.sg_pointer_work1
#define  SCSI_W32_sg_pointer_work2          scb8to19.scbResidualAndWork.sg_pointer_work2
#define  SCSI_W32_sg_pointer_work3          scb8to19.scbResidualAndWork.sg_pointer_work3

#define  SCSI_W32_mwiResidual               SCSI_W32_scdb10

/* definitions for scontrol   */
#define  SCSI_W32_SIMPLETAG  0x00           /* simple tag                 */
#define  SCSI_W32_HEADTAG    0x01           /* head of queue tag          */
#define  SCSI_W32_ORDERTAG   0x02           /* ordered tag                */
#define  SCSI_W32_FAST20     SCSI_W_FAST20  /* fast20 enable              */ 
#define  SCSI_W32_SPECFUN    0x08           /* special function           */
#define  SCSI_W32_TAGENB     0x20           /* tag enable                 */
#define  SCSI_W32_DISCENB    0x40           /* disconnect enable          */
#define  SCSI_W32_TAGMASK    0x03           /* mask for tags              */
#if SCSI_PARITY_PER_IOB
#define  SCSI_W32_ENPAR      0x10           /* disable parity checking    */
#endif

/* definitions dor starg_lun */
#define  SCSI_W32_TARGETID   0x0F           /* targte id                  */
#define  SCSI_W32_TARGETLUN  0x3F           /* target lun                 */

/* definitions for slun */
#define  SCSI_W32_MWI_FIX    0x80           /* mwi fix has been done      */

/* definitions for scdb_length */
#define  SCSI_W32_CDBLENMSK  0x1F           /* CDB length                 */
#define  SCSI_W32_NODATA     0x40           /* no data transfer involoved */
#define  SCSI_W32_ONESGSEG   0x80           /* only one sg segment        */

/* definitions for chain_control flags */
#define  SCSI_W32_HOLDOFF    0x01           /* hold off flag              */
#define  SCSI_W32_CONCURRENT 0x02           /* concurrent flag            */
#define  SCSI_W32_ABORTED    0x08           /* aborted flag               */
#define  SCSI_W32_PROGMASK   0xF0           /* masm for progress count    */
#define  SCSI_W32_THRESHOLD  SCSI_W_THRESHOLD /* threshold for scb manage   */

/* definitions for special function opcode */
#define  SCSI_W32_MSGTOTARG  0x00           /* special opcode, msg_to_targ */
#endif

/***************************************************************************
*  Standard bayonet SCB format layout
***************************************************************************/
#if SCSI_STANDARD_ADVANCED_MODE

/* structure definition */
typedef struct SCSI_SCB_STANDARD_ADVANCED_
{
   SCSI_UEXACT8     scontrol;                  /* scb flags                     */
   SCSI_UEXACT8     starget;                   /* target                        */
   SCSI_UEXACT8     slun;                      /* lun                           */
   SCSI_UEXACT8     xfer_option0;              /* xfer option                   */
   SCSI_UEXACT8     xfer_option1;              /* xfer option                   */
   union
   {
      SCSI_UEXACT8     q_next;                 /* point to next for execution   */
      SCSI_UEXACT8     array_site_next;             /* SCB array # (SCB destination) */
   } scb5;
   SCSI_UEXACT8     scdb_length;               /* cdb length                    */
   SCSI_UEXACT8     sg_cache_SCB;              /* # of SCSI_UEXACT8s for each element   */
   union 
   {
      struct 
      {
         SCSI_UEXACT8     sg_pointer_SCB0;     /* pointer to s/g list           */
         SCSI_UEXACT8     sg_pointer_SCB1;     /* pointer to s/g list           */
         SCSI_UEXACT8     sg_pointer_SCB2;     /* pointer to s/g list           */
         SCSI_UEXACT8     sg_pointer_SCB3;     /* pointer to s/g list           */
         SCSI_UEXACT8     sg_pointer_SCB4;     /* pointer to s/g list           */
         SCSI_UEXACT8     sg_pointer_SCB5;     /* pointer to s/g list           */
         SCSI_UEXACT8     sg_pointer_SCB6;     /* pointer to s/g list           */
         SCSI_UEXACT8     sg_pointer_SCB7;     /* pointer to s/g list           */
      } scbSG;
      struct 
      {
         SCSI_UEXACT8     special_opcode;      /* opcode for special scb func   */
         SCSI_UEXACT8     special_info;        /* special info                  */
      } scbSpecial;
   } scb8to15;
   union
   {
      SCSI_UEXACT8    scdb[12];            /* SCSI CDB                      */
      struct 
      {
         SCSI_UEXACT8     scdb0;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb1;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb2;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb3;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb4;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb5;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb6;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb7;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb8;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb9;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb10;              /* SCSI CDB                      */
         SCSI_UEXACT8     scdb11;              /* SCSI CDB                      */
      } scbCDB;
      SCSI_UEXACT8 scdb_pointer[8];            /* cdb pointer                   */
      struct 
      {
         SCSI_UEXACT8     scdb_pointer0;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer1;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer2;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer3;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer4;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer5;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer6;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer7;       /* cdb pointer                   */   
      } scbCDBPTR;
      struct 
      {
         SCSI_UEXACT8     sdata_residue0;      /* total # of bytes not xfer     */
         SCSI_UEXACT8     sdata_residue1;      /* total # of bytes not xfer     */
         SCSI_UEXACT8     sdata_residue2;      /* total # of bytes not xfer     */
         SCSI_UEXACT8     sdata_residue3;      /* total # of bytes not xfer     */
         SCSI_UEXACT8     reserved;            /* working flags                 */
         SCSI_UEXACT8     sg_cache_work;       /* working cache                 */
         SCSI_UEXACT8     sg_pointer_work0;    /* working s/g list pointer      */
         SCSI_UEXACT8     sg_pointer_work1;    /* working s/g list pointer      */
         SCSI_UEXACT8     sg_pointer_work2;    /* working s/g list pointer      */
         SCSI_UEXACT8     sg_pointer_work3;    /* working s/g list pointer      */
      } scbResidualAndWork;
   } scb16to27;
   SCSI_UEXACT8     chain_control;             /* chain control                 */
   SCSI_UEXACT8     next_SCB;                  /* pointer to next chained scb   */
   SCSI_UEXACT8     offshoot;                  /* pointer to offshoot scb       */
   SCSI_UEXACT8     reserved0;                 /* reserved                      */
   SCSI_UEXACT8     next_SCB_addr0;            /* next SCB address              */
   SCSI_UEXACT8     next_SCB_addr1;            /* next SCB address              */
   SCSI_UEXACT8     next_SCB_addr2;            /* next SCB address              */
   SCSI_UEXACT8     next_SCB_addr3;            /* next SCB address              */
   SCSI_UEXACT8     next_SCB_addr4;            /* next SCB address              */
   SCSI_UEXACT8     next_SCB_addr5;            /* next SCB address              */
   SCSI_UEXACT8     next_SCB_addr6;            /* next SCB address              */
   SCSI_UEXACT8     next_SCB_addr7;            /* next SCB address              */
#if (OSD_BUS_ADDRESS_SIZE == 64)
   SCSI_UEXACT8     saddress0;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress1;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress2;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress3;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress4;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress5;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress6;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress7;                 /* 1st s/g segment               */
   SCSI_UEXACT8     slength0;                  /* 1st s/g segment               */
   SCSI_UEXACT8     slength1;                  /* 1st s/g segment               */
   SCSI_UEXACT8     slength2;                  /* 1st s/g segment               */
#else
   SCSI_UEXACT8     saddress0;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress1;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress2;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress3;                 /* 1st s/g segment               */
   SCSI_UEXACT8     slength0;                  /* 1st s/g segment               */
   SCSI_UEXACT8     slength1;                  /* 1st s/g segment               */
   SCSI_UEXACT8     slength2;                  /* 1st s/g segment               */
   SCSI_UEXACT8     padding0;                  /* 1st s/g segment               */
   SCSI_UEXACT8     padding1;                  /* 1st s/g segment               */
   SCSI_UEXACT8     padding2;                  /* 1st s/g segment               */
   SCSI_UEXACT8     padding3;                  /* 1st s/g segment               */
#endif /* OSD_BUS_ADDRESS_SIZE */
   SCSI_UEXACT8     reserved[12];              /* reserved                      */
   SCSI_UEXACT8     busy_target;               /* busy target                   */
} SCSI_SCB_STANDARD_ADVANCED;

#define  SCSI_SADV_sg_pointer_SCB0          scb8to15.scbSG.sg_pointer_SCB0
#define  SCSI_SADV_sg_pointer_SCB1          scb8to15.scbSG.sg_pointer_SCB1
#define  SCSI_SADV_sg_pointer_SCB2          scb8to15.scbSG.sg_pointer_SCB2
#define  SCSI_SADV_sg_pointer_SCB3          scb8to15.scbSG.sg_pointer_SCB3
#define  SCSI_SADV_sg_pointer_SCB4          scb8to15.scbSG.sg_pointer_SCB4
#define  SCSI_SADV_sg_pointer_SCB5          scb8to15.scbSG.sg_pointer_SCB5
#define  SCSI_SADV_sg_pointer_SCB6          scb8to15.scbSG.sg_pointer_SCB6
#define  SCSI_SADV_sg_pointer_SCB7          scb8to15.scbSG.sg_pointer_SCB7

#define  SCSI_SADV_special_opcode           scb8to15.scbSpecial.special_opcode
#define  SCSI_SADV_special_info             scb8to15.scbSpecial.special_info

#define  SCSI_SADV_scdb                     scb16to27.scdb
#define  SCSI_SADV_scdb0                    scb16to27.scbCDB.scdb0 
#define  SCSI_SADV_scdb1                    scb16to27.scbCDB.scdb1
#define  SCSI_SADV_scdb2                    scb16to27.scbCDB.scdb2
#define  SCSI_SADV_scdb3                    scb16to27.scbCDB.scdb3
#define  SCSI_SADV_scdb4                    scb16to27.scbCDB.scdb4
#define  SCSI_SADV_scdb5                    scb16to27.scbCDB.scdb5
#define  SCSI_SADV_scdb6                    scb16to27.scbCDB.scdb6
#define  SCSI_SADV_scdb7                    scb16to27.scbCDB.scdb7
#define  SCSI_SADV_scdb8                    scb16to27.scbCDB.scdb8
#define  SCSI_SADV_scdb9                    scb16to27.scbCDB.scdb9
#define  SCSI_SADV_scdb10                   scb16to27.scbCDB.scdb10
#define  SCSI_SADV_scdb11                   scb16to27.scbCDB.scdb11

#define  SCSI_SADV_scdb_pointer             scb16to27.scdb_pointer

#define  SCSI_SADV_scdb_pointer0            scb16to27.scbCDBPTR.scdb_pointer0
#define  SCSI_SADV_scdb_pointer1            scb16to27.scbCDBPTR.scdb_pointer1
#define  SCSI_SADV_scdb_pointer2            scb16to27.scbCDBPTR.scdb_pointer2
#define  SCSI_SADV_scdb_pointer3            scb16to27.scbCDBPTR.scdb_pointer3
#define  SCSI_SADV_scdb_pointer4            scb16to27.scbCDBPTR.scdb_pointer4
#define  SCSI_SADV_scdb_pointer5            scb16to27.scbCDBPTR.scdb_pointer5
#define  SCSI_SADV_scdb_pointer6            scb16to27.scbCDBPTR.scdb_pointer6
#define  SCSI_SADV_scdb_pointer7            scb16to27.scbCDBPTR.scdb_pointer7

#define  SCSI_SADV_sdata_residue0           scb16to27.scbResidualAndWork.sdata_residue0
#define  SCSI_SADV_sdata_residue1           scb16to27.scbResidualAndWork.sdata_residue1
#define  SCSI_SADV_sdata_residue2           scb16to27.scbResidualAndWork.sdata_residue2
#define  SCSI_SADV_sdata_residue3           scb16to27.scbResidualAndWork.sdata_residue3
#define  SCSI_SADV_sg_flags_work            scb16to27.scbResidualAndWork.sg_flags_work  
#define  SCSI_SADV_sg_cache_work            scb16to27.scbResidualAndWork.sg_cache_work  
#define  SCSI_SADV_sg_pointer_work0         scb16to27.scbResidualAndWork.sg_pointer_work0
#define  SCSI_SADV_sg_pointer_work1         scb16to27.scbResidualAndWork.sg_pointer_work1
#define  SCSI_SADV_sg_pointer_work2         scb16to27.scbResidualAndWork.sg_pointer_work2
#define  SCSI_SADV_sg_pointer_work3         scb16to27.scbResidualAndWork.sg_pointer_work3

#define  SCSI_SADV_mwiResidual              SCSI_SADV_scdb11

#define  SCSI_SADV_q_next                   scb5.q_next
#define  SCSI_SADV_array_site_next          scb5.array_site_next

/* definitions for scontrol   */
#define  SCSI_SADV_SIMPLETAG  0x00          /* simple tag                 */
#define  SCSI_SADV_HEADTAG    0x01          /* head of queue tag          */
#define  SCSI_SADV_ORDERTAG   0x02          /* ordered tag                */
#define  SCSI_SADV_SPECFUN    0x08          /* special function           */
#define  SCSI_SADV_TAGENB     0x20          /* tag enable                 */
#define  SCSI_SADV_DISCENB    0x40          /* disconnect enable          */
#define  SCSI_SADV_TAGMASK    0x03          /* mask for tags              */

/* definitions dor starg_lun */
#define  SCSI_SADV_TARGETID   0xF0          /* targte id                  */
#define  SCSI_SADV_TARGETLUN  0x07          /* target lun                 */

/* definitions for scdb_length */
#define  SCSI_SADV_CDBLENMSK  0x1F          /* CDB length                 */
#define  SCSI_SADV_CACHE_ADDR_MASK  0x7C    /* Mask out rollover & lo 2 bits */

/* definitions for sg_cache_SCB */
#define  SCSI_SADV_NODATA     0x01          /* no data transfer involoved */
#define  SCSI_SADV_ONESGSEG   0x02          /* only one sg segment        */

/* definitions for chain_control flags */
#define  SCSI_SADV_HOLDOFF    0x01          /* hold off flag              */
#define  SCSI_SADV_CONCURRENT 0x02          /* concurrent flag            */

#define  SCSI_SADV_ABORTED    0x10          /* aborted flag               */

#define  SCSI_SADV_PROGMASK   0xF0          /* masm for progress count    */

/* definitions for special function opcode */
#define  SCSI_SADV_MSGTOTARG  0x00          /* special opcode, msg_to_targ */
#endif

/***************************************************************************
*  Swapping bayonet SCB format layout
***************************************************************************/
#if SCSI_SWAPPING_ADVANCED_MODE

/* structure definition */
typedef struct SCSI_SCB_SWAPPING_ADVANCED_
{
#if (OSD_BUS_ADDRESS_SIZE == 64)
   SCSI_UEXACT8     saddress0;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress1;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress2;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress3;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress4;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress5;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress6;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress7;                 /* 1st s/g segment               */
   SCSI_UEXACT8     slength0;                  /* 1st s/g segment               */
   SCSI_UEXACT8     slength1;                  /* 1st s/g segment               */
   SCSI_UEXACT8     slength2;                  /* 1st s/g segment               */
#else
   SCSI_UEXACT8     saddress0;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress1;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress2;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress3;                 /* 1st s/g segment               */
   SCSI_UEXACT8     slength0;                  /* 1st s/g segment               */
   SCSI_UEXACT8     slength1;                  /* 1st s/g segment               */
   SCSI_UEXACT8     slength2;                  /* 1st s/g segment               */
   SCSI_UEXACT8     padding0;                  /* 1st s/g segment               */
   SCSI_UEXACT8     padding1;                  /* 1st s/g segment               */
   SCSI_UEXACT8     padding2;                  /* 1st s/g segment               */
   SCSI_UEXACT8     padding3;                  /* 1st s/g segment               */
#endif /* OSD_BUS_ADDRESS_SIZE */
   SCSI_UEXACT8     next_SCB;                  /* next SCB                      */
   SCSI_UEXACT8     starget;                   /* target (lower 4 bits)         */
                                               /* if target scb then this byte  */
                                               /* contains Other Id (7:4) and   */
                                               /* Own Id (3:0)                  */
   SCSI_UEXACT8     slun;                      /* lun                           */
   SCSI_UEXACT8     scdb_length;               /* cdb length                    */
   SCSI_UEXACT8     sg_cache_SCB;              /* # of SCSI_UEXACT8s for each element   */
   union 
   {
      struct 
      {
         SCSI_UEXACT8     sg_pointer_SCB0;     /* pointer to s/g list           */
         SCSI_UEXACT8     sg_pointer_SCB1;     /* pointer to s/g list           */
         SCSI_UEXACT8     sg_pointer_SCB2;     /* pointer to s/g list           */
         SCSI_UEXACT8     sg_pointer_SCB3;     /* pointer to s/g list           */
         SCSI_UEXACT8     sg_pointer_SCB4;     /* pointer to s/g list           */
         SCSI_UEXACT8     sg_pointer_SCB5;     /* pointer to s/g list           */
         SCSI_UEXACT8     sg_pointer_SCB6;     /* pointer to s/g list           */
         SCSI_UEXACT8     sg_pointer_SCB7;     /* pointer to s/g list           */
      } scbSG;
      struct 
      {
         SCSI_UEXACT8     special_opcode;      /* opcode for special scb func   */
         SCSI_UEXACT8     special_info;        /* special info                  */
      } scbSpecial;
   } scb16to23;
   union
   {
      SCSI_UEXACT8    scdb[12];            /* SCSI CDB                      */
      struct 
      {
         SCSI_UEXACT8     scdb0;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb1;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb2;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb3;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb4;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb5;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb6;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb7;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb8;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb9;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb10;              /* SCSI CDB                      */
         SCSI_UEXACT8     scdb11;              /* SCSI CDB                      */
      } scbCDB;
      SCSI_UEXACT8 scdb_pointer[8];            /* cdb pointer                   */
      struct 
      {
         SCSI_UEXACT8     scdb_pointer0;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer1;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer2;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer3;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer4;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer5;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer6;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer7;       /* cdb pointer                   */   
      } scbCDBPTR;
      struct 
      {
         SCSI_UEXACT8     sdata_residue0;      /* total # of bytes not xfer     */
         SCSI_UEXACT8     sdata_residue1;      /* total # of bytes not xfer     */
         SCSI_UEXACT8     sdata_residue2;      /* total # of bytes not xfer     */
         SCSI_UEXACT8     sdata_residue3;      /* total # of bytes not xfer     */
         SCSI_UEXACT8     sg_cache_work;       /* working cache                 */
         SCSI_UEXACT8     sg_pointer_work0;    /* working s/g list pointer      */
         SCSI_UEXACT8     sg_pointer_work1;    /* working s/g list pointer      */
         SCSI_UEXACT8     sg_pointer_work2;    /* working s/g list pointer      */
         SCSI_UEXACT8     sg_pointer_work3;    /* working s/g list pointer      */
      } scbResidualAndWork;
#if SCSI_TARGET_OPERATION
      struct
      {
         SCSI_UEXACT8     sdata_residue0;      /* total # of bytes not xfer     */
         SCSI_UEXACT8     sdata_residue1;      /* total # of bytes not xfer     */
         SCSI_UEXACT8     sdata_residue2;      /* total # of bytes not xfer     */
         SCSI_UEXACT8     sdata_residue3;      /* total # of bytes not xfer     */
         SCSI_UEXACT8     sg_cache_work;       /* working cache                 */
         SCSI_UEXACT8     sg_pointer_work0;    /* working s/g list pointer      */
         SCSI_UEXACT8     sg_pointer_work1;    /* working s/g list pointer      */
         SCSI_UEXACT8     sg_pointer_work2;    /* working s/g list pointer      */
         SCSI_UEXACT8     sg_pointer_work3;    /* working s/g list pointer      */
         SCSI_UEXACT8     reserved;
         SCSI_UEXACT8     starg_tagno;         /* target tag for target mode    */
         SCSI_UEXACT8     starg_status;        /* target status for target mode */
      } scbResidualAndWorkAndTarget;    
#endif /* SCSI_TARGET_OPERATION */      
   } scb24to35;
   SCSI_UEXACT8     xfer_option0;              /* xfer option                   */
   SCSI_UEXACT8     xfer_option1;              /* xfer option                   */
   SCSI_UEXACT8     scontrol;                  /* scb flags                     */
   SCSI_UEXACT8     reserved[25];              /* reserved                      */
} SCSI_SCB_SWAPPING_ADVANCED;

#define  SCSI_WADV_sg_pointer_SCB0          scb16to23.scbSG.sg_pointer_SCB0
#define  SCSI_WADV_sg_pointer_SCB1          scb16to23.scbSG.sg_pointer_SCB1
#define  SCSI_WADV_sg_pointer_SCB2          scb16to23.scbSG.sg_pointer_SCB2
#define  SCSI_WADV_sg_pointer_SCB3          scb16to23.scbSG.sg_pointer_SCB3
#define  SCSI_WADV_sg_pointer_SCB4          scb16to23.scbSG.sg_pointer_SCB4
#define  SCSI_WADV_sg_pointer_SCB5          scb16to23.scbSG.sg_pointer_SCB5
#define  SCSI_WADV_sg_pointer_SCB6          scb16to23.scbSG.sg_pointer_SCB6
#define  SCSI_WADV_sg_pointer_SCB7          scb16to23.scbSG.sg_pointer_SCB7
              
#define  SCSI_WADV_special_opcode           scb16to23.scbSpecial.special_opcode
#define  SCSI_WADV_special_info             scb16to23.scbSpecial.special_info
              
#define  SCSI_WADV_scdb                     scb24to35.scdb
#define  SCSI_WADV_scdb0                    scb24to35.scbCDB.scdb0 
#define  SCSI_WADV_scdb1                    scb24to35.scbCDB.scdb1
#define  SCSI_WADV_scdb2                    scb24to35.scbCDB.scdb2
#define  SCSI_WADV_scdb3                    scb24to35.scbCDB.scdb3
#define  SCSI_WADV_scdb4                    scb24to35.scbCDB.scdb4
#define  SCSI_WADV_scdb5                    scb24to35.scbCDB.scdb5
#define  SCSI_WADV_scdb6                    scb24to35.scbCDB.scdb6
#define  SCSI_WADV_scdb7                    scb24to35.scbCDB.scdb7
#define  SCSI_WADV_scdb8                    scb24to35.scbCDB.scdb8
#define  SCSI_WADV_scdb9                    scb24to35.scbCDB.scdb9
#define  SCSI_WADV_scdb10                   scb24to35.scbCDB.scdb10
#define  SCSI_WADV_scdb11                   scb24to35.scbCDB.scdb11
                                                   
#define  SCSI_WADV_scdb_pointer             scb24to35.scdb_pointer
                                                   
#define  SCSI_WADV_scdb_pointer0            scb24to35.scbCDBPTR.scdb_pointer0
#define  SCSI_WADV_scdb_pointer1            scb24to35.scbCDBPTR.scdb_pointer1
#define  SCSI_WADV_scdb_pointer2            scb24to35.scbCDBPTR.scdb_pointer2
#define  SCSI_WADV_scdb_pointer3            scb24to35.scbCDBPTR.scdb_pointer3
#define  SCSI_WADV_scdb_pointer4            scb24to35.scbCDBPTR.scdb_pointer4
#define  SCSI_WADV_scdb_pointer5            scb24to35.scbCDBPTR.scdb_pointer5
#define  SCSI_WADV_scdb_pointer6            scb24to35.scbCDBPTR.scdb_pointer6
#define  SCSI_WADV_scdb_pointer7            scb24to35.scbCDBPTR.scdb_pointer7
                                                   
#define  SCSI_WADV_sdata_residue0           scb24to35.scbResidualAndWork.sdata_residue0
#define  SCSI_WADV_sdata_residue1           scb24to35.scbResidualAndWork.sdata_residue1
#define  SCSI_WADV_sdata_residue2           scb24to35.scbResidualAndWork.sdata_residue2
#define  SCSI_WADV_sdata_residue3           scb24to35.scbResidualAndWork.sdata_residue3
#define  SCSI_WADV_sg_flags_work            scb24to35.scbResidualAndWork.sg_flags_work  
#define  SCSI_WADV_sg_cache_work            scb24to35.scbResidualAndWork.sg_cache_work  
#define  SCSI_WADV_sg_pointer_work0         scb24to35.scbResidualAndWork.sg_pointer_work0
#define  SCSI_WADV_sg_pointer_work1         scb24to35.scbResidualAndWork.sg_pointer_work1
#define  SCSI_WADV_sg_pointer_work2         scb24to35.scbResidualAndWork.sg_pointer_work2
#define  SCSI_WADV_sg_pointer_work3         scb24to35.scbResidualAndWork.sg_pointer_work3
#if SCSI_TARGET_OPERATION
#define  SCSI_WADV_starg_tagno              scb24to35.scbResidualAndWorkAndTarget.starg_tagno
#define  SCSI_WADV_starg_status             scb24to35.scbResidualAndWorkAndTarget.starg_status
#endif /* SCSI_TARGET_OPERATION */

#define  SCSI_WADV_mwiResidual              SCSI_WADV_scdb11

/* definitions for scontrol   */
#define  SCSI_WADV_SIMPLETAG  0x00          /* simple tag                 */
#define  SCSI_WADV_HEADTAG    0x01          /* head of queue tag          */
#define  SCSI_WADV_ORDERTAG   0x02          /* ordered tag                */
#define  SCSI_WADV_SPECFUN    0x08          /* special function           */
#define  SCSI_WADV_TAGENB     0x20          /* tag enable                 */
#define  SCSI_WADV_DISCENB    0x40          /* disconnect enable          */
#define  SCSI_WADV_TAGMASK    0x03          /* mask for tags              */
#if SCSI_TARGET_OPERATION
#define  SCSI_WADV_TARGETENB  0x04          /* target mode enable         */
#define  SCSI_WADV_DATAOUT    0x01          /* data-out to initiator      */
#define  SCSI_WADV_HOLDONBUS  0x02          /* hold-on to SCSI BUS        */ 
#endif  /* SCSI_TARGET_OPERATION */
#if SCSI_PARITY_PER_IOB
#define  SCSI_WADV_ENPAR      0x80          /* disable parity checking    */
#endif

#if SCSI_TARGET_OPERATION
/* definitions for starget */
#define  SCSI_WADV_OWNID_MASK 0x0F          /* Mask of starget field      */
#define  SCSI_WADV_OTHERID_MASK 0xF0        /* Mask of starget field      */  
#endif /* SCSI_TARGET_OPERATION */

/* definitions for scdb_length */
#define  SCSI_WADV_CDBLENMSK  0x1F          /* CDB length                 */
#define  SCSI_WADV_CACHE_ADDR_MASK  0x7C    /* Mask out rollover & lo 2 bits */

/* definitions for sg_cache_SCB */
#define  SCSI_WADV_NODATA     0x01          /* no data transfer involoved */
#define  SCSI_WADV_ONESGSEG   0x02          /* only one sg segment        */

/* definitions for chain_control flags */
#define  SCSI_WADV_ABORTED    0x10          /* aborted flag               */

/* definitions for special function opcode */
#define  SCSI_WADV_MSGTOTARG  0x00          /* special opcode, msg_to_targ */
#endif

/***************************************************************************
*  Swapping bayonet Establish Connection SCB format layout
***************************************************************************/
#if SCSI_SWAPPING_ADVANCED_MODE && SCSI_TARGET_OPERATION
/* structure definition */
typedef struct SCSI_EST_SCB_SWAPPING_ADVANCED_ 
{
   SCSI_UEXACT8     next_SCB;              /* next SCB                      */
   SCSI_UEXACT8     current_SCB;           /* current SCB                   */            
   SCSI_UEXACT8     scontrol;              /* scb flags                     */ 
   SCSI_UEXACT8     reserve0;

   SCSI_UEXACT8     scbaddress0;           /* PCI address of this scb       */
   SCSI_UEXACT8     scbaddress1;           /* PCI address of this scb       */
   SCSI_UEXACT8     scbaddress2;           /* PCI address of this scb       */
   SCSI_UEXACT8     scbaddress3;           /* PCI address of this scb       */
#if (OSD_BUS_ADDRESS_SIZE == 64)
   SCSI_UEXACT8     scbaddress4;           /* PCI address of this scb       */
   SCSI_UEXACT8     scbaddress5;           /* PCI address of this scb       */
   SCSI_UEXACT8     scbaddress6;           /* PCI address of this scb       */
   SCSI_UEXACT8     scbaddress7;           /* PCI address of this scb       */
#else
   SCSI_UEXACT8     padding0;
   SCSI_UEXACT8     padding1;
   SCSI_UEXACT8     padding2;
   SCSI_UEXACT8     padding3;
#endif /* OSD_BUS_ADDRESS_SIZE */
   SCSI_UEXACT8     est_iid;              /* other ID 7:3, own ID 3:0       */
   SCSI_UEXACT8     tag_type;             /* queue tag type received from initiator */
   SCSI_UEXACT8     tag_num;              /* tag number received from initiator     */
   SCSI_UEXACT8     scdb_len;             /* cbd length and flags                   */
   SCSI_UEXACT8     scdb0;                /* SCSI CDB                               */
   SCSI_UEXACT8     scdb1;                /* SCSI CDB                               */
   SCSI_UEXACT8     scdb2;                /* SCSI CDB                               */ 
   SCSI_UEXACT8     scdb3;                /* SCSI CDB                               */
   SCSI_UEXACT8     scdb4;                /* SCSI CDB                               */
   SCSI_UEXACT8     scdb5;                /* SCSI CDB                               */
   SCSI_UEXACT8     scdb6;                /* SCSI CDB                               */
   SCSI_UEXACT8     scdb7;                /* SCSI CDB                               */
   SCSI_UEXACT8     scdb8;                /* SCSI CDB                               */
   SCSI_UEXACT8     scdb9;                /* SCSI CDB                               */
   SCSI_UEXACT8     scdb10;               /* SCSI CDB                               */
   SCSI_UEXACT8     scdb11;               /* SCSI CDB                               */
   SCSI_UEXACT8     scdb12;               /* SCSI CDB                               */
   SCSI_UEXACT8     scdb13;               /* SCSI CDB                               */
   SCSI_UEXACT8     scdb14;               /* SCSI CDB                               */
   SCSI_UEXACT8     scdb15;               /* SCSI CDB                               */
   SCSI_UEXACT8     id_msg;               /* identify msg byte recvd from initiator */
   SCSI_UEXACT8     last_byte;            /* last byte received from initiator      */
   SCSI_UEXACT8     flags;                /* misc flags related to connection       */
   SCSI_UEXACT8     eststatus;            /* status returned from sequencer         */
   SCSI_UEXACT8     reserved[28];
} SCSI_EST_SCB_SWAPPING_ADVANCED;

/* defines for scontrol */ 
#define SCSI_EST_WADV_SCSI1_EN          0x20
#define SCSI_EST_WADV_DISCON_EN         0x10
#define SCSI_EST_WADV_SCSI3_EN          0x08
#define SCSI_EST_WADV_TAG_ENABLE        0x04
#define SCSI_EST_WADV_CHK_RSVD_BITS     0x02
#define SCSI_EST_WADV_LUNTAR_EN         0x01

/* defines for flags */
#define SCSI_EST_WADV_TAG_RCVD          0x80  /* queue tag message received */
#define SCSI_EST_WADV_SCSI1_SEL         0x40  /* scsi1 selection received */
#define SCSI_EST_WADV_BUS_HELD          0x20  /* bus held */
#define SCSI_EST_WADV_RSVD_BITS         0x10  /* reserved bit set in SCSI-2 
                                               * identify message and 
                                               * CHK_RSVD_BITS set
                                               */
#define SCSI_EST_WADV_LUNTAR            0x08  /* luntar bit set in SCSI-2
                                               * identify message and
                                               * LUNTAR_EN = 0
                                               */
/* defines for last state */
/* are these still relevant ???? */
#define SCSI_EST_WADV_CMD_PHASE         0x02
#define SCSI_EST_WADV_MSG_OUT_PHASE     0x06  
#define SCSI_EST_WADV_MSG_IN_PHASE      0x07

/* definitions for eststatus */
#define SCSI_EST_WADV_GOOD_STATE    0x00 /* command received without exception */ 
#define SCSI_EST_WADV_SEL_STATE     0x01 /* exception during selection phase */
#define SCSI_EST_WADV_ID_STATE      0x02 /*     "        "   identifying phase */
#define SCSI_EST_WADV_MSGOUT_STATE  0x03 /*     "        "   message out phase */
#define SCSI_EST_WADV_CMD_STATE     0x04 /*     "        "   command phase     */
#define SCSI_EST_WADV_DISC_STATE    0x05 /*     "        "   disconnect phase  */
#define SCSI_EST_WADV_VENDOR_CMD    0x08 /* Vendor unique command */
/* These statuses are stored in the upper nibble of the returned status 
 * so we can handle multiple conditions at once.
 */
#define SCSI_EST_WADV_LAST_RESOURCE 0x40  /* last resource used */
#define SCSI_EST_WADV_PARITY_ERR    0x80  /* parity error       */ 

/* defines for estiid */
#define SCSI_EST_WADV_SELID_MASK    0x0F  /* our selected ID */  
#endif /* SCSI_SWAPPING_ADVANCED_MODE && SCSI_TARGET_OPERATION */

/***************************************************************************
*  Standard Ultra 160m SCB format layout
***************************************************************************/
#if SCSI_STANDARD_160M_MODE

/* structure definition */
typedef struct SCSI_SCB_STANDARD_160M_
{
   SCSI_UEXACT8     scontrol;                  /* scb flags                     */
   SCSI_UEXACT8     starget;                   /* target                        */
   SCSI_UEXACT8     slun;                      /* lun                           */
   SCSI_UEXACT8     atn_length;                /* attention management          */
   SCSI_UEXACT8     reserved0;                 /* reserved                      */
   union
   {
      SCSI_UEXACT8     q_next;                 /* point to next for execution   */
      SCSI_UEXACT8     array_site_next;        /* SCB array # (SCB destination) */
   } scb5;
   SCSI_UEXACT8     scdb_length;               /* cdb length                    */
   SCSI_UEXACT8     sg_cache_SCB;              /* # of SCSI_UEXACT8s for each element   */
   union 
   {
      struct 
      {
         SCSI_UEXACT8     sg_pointer_SCB0;     /* pointer to s/g list           */
         SCSI_UEXACT8     sg_pointer_SCB1;     /* pointer to s/g list           */
         SCSI_UEXACT8     sg_pointer_SCB2;     /* pointer to s/g list           */
         SCSI_UEXACT8     sg_pointer_SCB3;     /* pointer to s/g list           */
         SCSI_UEXACT8     sg_pointer_SCB4;     /* pointer to s/g list           */
         SCSI_UEXACT8     sg_pointer_SCB5;     /* pointer to s/g list           */
         SCSI_UEXACT8     sg_pointer_SCB6;     /* pointer to s/g list           */
         SCSI_UEXACT8     sg_pointer_SCB7;     /* pointer to s/g list           */
      } scbSG;
      struct 
      {
         SCSI_UEXACT8     special_opcode;      /* opcode for special scb func   */
         SCSI_UEXACT8     special_info;        /* special info                  */
      } scbSpecial;
   } scb8to15;
   union
   {
      SCSI_UEXACT8    scdb[12];                /* SCSI CDB                      */
      struct 
      {
         SCSI_UEXACT8     scdb0;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb1;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb2;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb3;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb4;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb5;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb6;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb7;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb8;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb9;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb10;              /* SCSI CDB                      */
         SCSI_UEXACT8     scdb11;              /* SCSI CDB                      */
      } scbCDB;
      SCSI_UEXACT8 scdb_pointer[8];            /* cdb pointer                   */
      struct 
      {
         SCSI_UEXACT8     scdb_pointer0;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer1;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer2;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer3;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer4;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer5;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer6;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer7;       /* cdb pointer                   */   
      } scbCDBPTR;
      struct 
      {
         SCSI_UEXACT8     sdata_residue0;      /* total # of bytes not xfer     */
         SCSI_UEXACT8     sdata_residue1;      /* total # of bytes not xfer     */
         SCSI_UEXACT8     sdata_residue2;      /* total # of bytes not xfer     */
         SCSI_UEXACT8     sdata_residue3;      /* total # of bytes not xfer     */
         SCSI_UEXACT8     reserved;            /* working flags                 */
         SCSI_UEXACT8     sg_cache_work;       /* working cache                 */
         SCSI_UEXACT8     sg_pointer_work0;    /* working s/g list pointer      */
         SCSI_UEXACT8     sg_pointer_work1;    /* working s/g list pointer      */
         SCSI_UEXACT8     sg_pointer_work2;    /* working s/g list pointer      */
         SCSI_UEXACT8     sg_pointer_work3;    /* working s/g list pointer      */
      } scbResidualAndWork;
   } scb16to27;
   SCSI_UEXACT8     chain_control;             /* chain control                 */
   SCSI_UEXACT8     next_SCB;                  /* pointer to next chained scb   */
   SCSI_UEXACT8     offshoot;                  /* pointer to offshoot scb       */
   SCSI_UEXACT8     reserved1;                 /* reserved                      */
   SCSI_UEXACT8     next_SCB_addr0;            /* next SCB address              */
   SCSI_UEXACT8     next_SCB_addr1;            /* next SCB address              */
   SCSI_UEXACT8     next_SCB_addr2;            /* next SCB address              */
   SCSI_UEXACT8     next_SCB_addr3;            /* next SCB address              */
   SCSI_UEXACT8     next_SCB_addr4;            /* next SCB address              */
   SCSI_UEXACT8     next_SCB_addr5;            /* next SCB address              */
   SCSI_UEXACT8     next_SCB_addr6;            /* next SCB address              */
   SCSI_UEXACT8     next_SCB_addr7;            /* next SCB address              */
#if (OSD_BUS_ADDRESS_SIZE == 64)
   SCSI_UEXACT8     saddress0;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress1;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress2;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress3;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress4;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress5;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress6;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress7;                 /* 1st s/g segment               */
   SCSI_UEXACT8     slength0;                  /* 1st s/g segment               */
   SCSI_UEXACT8     slength1;                  /* 1st s/g segment               */
   SCSI_UEXACT8     slength2;                  /* 1st s/g segment               */
#else
   SCSI_UEXACT8     saddress0;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress1;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress2;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress3;                 /* 1st s/g segment               */
   SCSI_UEXACT8     slength0;                  /* 1st s/g segment               */
   SCSI_UEXACT8     slength1;                  /* 1st s/g segment               */
   SCSI_UEXACT8     slength2;                  /* 1st s/g segment               */
   SCSI_UEXACT8     padding0;                  /* 1st s/g segment               */
   SCSI_UEXACT8     padding1;                  /* 1st s/g segment               */
   SCSI_UEXACT8     padding2;                  /* 1st s/g segment               */
   SCSI_UEXACT8     padding3;                  /* 1st s/g segment               */
#endif /* OSD_BUS_ADDRESS_SIZE */
   SCSI_UEXACT8     reserved[12];              /* reserved                      */
   SCSI_UEXACT8     busy_target;               /* busy target                   */
} SCSI_SCB_STANDARD_160M;

#define  SCSI_S160M_sg_pointer_SCB0         scb8to15.scbSG.sg_pointer_SCB0
#define  SCSI_S160M_sg_pointer_SCB1         scb8to15.scbSG.sg_pointer_SCB1
#define  SCSI_S160M_sg_pointer_SCB2         scb8to15.scbSG.sg_pointer_SCB2
#define  SCSI_S160M_sg_pointer_SCB3         scb8to15.scbSG.sg_pointer_SCB3
#define  SCSI_S160M_sg_pointer_SCB4         scb8to15.scbSG.sg_pointer_SCB4
#define  SCSI_S160M_sg_pointer_SCB5         scb8to15.scbSG.sg_pointer_SCB5
#define  SCSI_S160M_sg_pointer_SCB6         scb8to15.scbSG.sg_pointer_SCB6
#define  SCSI_S160M_sg_pointer_SCB7         scb8to15.scbSG.sg_pointer_SCB7

#define  SCSI_S160M_special_opcode          scb8to15.scbSpecial.special_opcode
#define  SCSI_S160M_special_info            scb8to15.scbSpecial.special_info

#define  SCSI_S160M_scdb                    scb16to27.scdb
#define  SCSI_S160M_scdb0                   scb16to27.scbCDB.scdb0 
#define  SCSI_S160M_scdb1                   scb16to27.scbCDB.scdb1
#define  SCSI_S160M_scdb2                   scb16to27.scbCDB.scdb2
#define  SCSI_S160M_scdb3                   scb16to27.scbCDB.scdb3
#define  SCSI_S160M_scdb4                   scb16to27.scbCDB.scdb4
#define  SCSI_S160M_scdb5                   scb16to27.scbCDB.scdb5
#define  SCSI_S160M_scdb6                   scb16to27.scbCDB.scdb6
#define  SCSI_S160M_scdb7                   scb16to27.scbCDB.scdb7
#define  SCSI_S160M_scdb8                   scb16to27.scbCDB.scdb8
#define  SCSI_S160M_scdb9                   scb16to27.scbCDB.scdb9
#define  SCSI_S160M_scdb10                  scb16to27.scbCDB.scdb10
#define  SCSI_S160M_scdb11                  scb16to27.scbCDB.scdb11

#define  SCSI_S160M_scdb_pointer            scb16to27.scdb_pointer

#define  SCSI_S160M_scdb_pointer0           scb16to27.scbCDBPTR.scdb_pointer0
#define  SCSI_S160M_scdb_pointer1           scb16to27.scbCDBPTR.scdb_pointer1
#define  SCSI_S160M_scdb_pointer2           scb16to27.scbCDBPTR.scdb_pointer2
#define  SCSI_S160M_scdb_pointer3           scb16to27.scbCDBPTR.scdb_pointer3
#define  SCSI_S160M_scdb_pointer4           scb16to27.scbCDBPTR.scdb_pointer4
#define  SCSI_S160M_scdb_pointer5           scb16to27.scbCDBPTR.scdb_pointer5
#define  SCSI_S160M_scdb_pointer6           scb16to27.scbCDBPTR.scdb_pointer6
#define  SCSI_S160M_scdb_pointer7           scb16to27.scbCDBPTR.scdb_pointer7

#define  SCSI_S160M_sdata_residue0          scb16to27.scbResidualAndWork.sdata_residue0
#define  SCSI_S160M_sdata_residue1          scb16to27.scbResidualAndWork.sdata_residue1
#define  SCSI_S160M_sdata_residue2          scb16to27.scbResidualAndWork.sdata_residue2
#define  SCSI_S160M_sdata_residue3          scb16to27.scbResidualAndWork.sdata_residue3
#define  SCSI_S160M_sg_cache_work           scb16to27.scbResidualAndWork.sg_cache_work  
#define  SCSI_S160M_sg_pointer_work0        scb16to27.scbResidualAndWork.sg_pointer_work0
#define  SCSI_S160M_sg_pointer_work1        scb16to27.scbResidualAndWork.sg_pointer_work1
#define  SCSI_S160M_sg_pointer_work2        scb16to27.scbResidualAndWork.sg_pointer_work2
#define  SCSI_S160M_sg_pointer_work3        scb16to27.scbResidualAndWork.sg_pointer_work3

#define  SCSI_S160M_q_next                  scb5.q_next
#define  SCSI_S160M_array_site_next         scb5.array_site_next

/* definitions for scontrol   */
#define  SCSI_S160M_SIMPLETAG  0x00         /* simple tag                 */
#define  SCSI_S160M_HEADTAG    0x01         /* head of queue tag          */
#define  SCSI_S160M_ORDERTAG   0x02         /* ordered tag                */
#define  SCSI_S160M_SPECFUN    0x08         /* special function           */
#define  SCSI_S160M_TAGENB     0x20         /* tag enable                 */
#define  SCSI_S160M_DISCENB    0x40         /* disconnect enable          */
#define  SCSI_S160M_TAGMASK    0x03         /* mask for tags              */
#if SCSI_PARITY_PER_IOB
#define  SCSI_S160M_ENPAR      0x80         /* disable parity checking    */
#endif

/* definitions for starg_lun */
#define  SCSI_S160M_TARGETID   0xF0         /* target id                  */
#define  SCSI_S160M_TARGETLUN  0x07         /* target lun                 */

/* definitions for scdb_length */
#define  SCSI_S160M_CDBLENMSK  0x1F         /* CDB length                 */
#define  SCSI_S160M_CACHE_ADDR_MASK  0x7C   /* Mask out rollover & lo 2 bits */

/* definitions for sg_cache_SCB */
#define  SCSI_S160M_NODATA     0x01         /* no data transfer involoved */
#define  SCSI_S160M_ONESGSEG   0x02         /* only one sg segment        */

/* definitions for chain_control flags */
#define  SCSI_S160M_HOLDOFF    0x01         /* hold off flag              */
#define  SCSI_S160M_CONCURRENT 0x02         /* concurrent flag            */

#define  SCSI_S160M_ABORTED    0x10         /* aborted flag               */

#define  SCSI_S160M_PROGMASK   0xF0         /* masm for progress count    */

/* definitions for special function opcode */
#define  SCSI_S160M_MSGTOTARG  0x00         /* special opcode, msg_to_targ */
#endif /* SCSI_STANDARD_160M_MODE */

/***************************************************************************
*  Swapping Ultra 160m SCB format layout
***************************************************************************/
#if SCSI_SWAPPING_160M_MODE

/* structure definition */
typedef struct SCSI_SCB_SWAPPING_160M_
{
#if (OSD_BUS_ADDRESS_SIZE == 64)
   SCSI_UEXACT8     saddress0;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress1;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress2;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress3;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress4;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress5;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress6;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress7;                 /* 1st s/g segment               */
   SCSI_UEXACT8     slength0;                  /* 1st s/g segment               */
   SCSI_UEXACT8     slength1;                  /* 1st s/g segment               */
   SCSI_UEXACT8     slength2;                  /* 1st s/g segment               */
#else
   SCSI_UEXACT8     saddress0;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress1;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress2;                 /* 1st s/g segment               */
   SCSI_UEXACT8     saddress3;                 /* 1st s/g segment               */
   SCSI_UEXACT8     slength0;                  /* 1st s/g segment               */
   SCSI_UEXACT8     slength1;                  /* 1st s/g segment               */
   SCSI_UEXACT8     slength2;                  /* 1st s/g segment               */
   SCSI_UEXACT8     padding0;                  /* 1st s/g segment               */
   SCSI_UEXACT8     padding1;                  /* 1st s/g segment               */
   SCSI_UEXACT8     padding2;                  /* 1st s/g segment               */
   SCSI_UEXACT8     padding3;                  /* 1st s/g segment               */
#endif /* OSD_BUS_ADDRESS_SIZE */
   SCSI_UEXACT8     next_SCB;                  /* next SCB                      */
   SCSI_UEXACT8     starget;                   /* target                        */
   SCSI_UEXACT8     slun;                      /* lun                           */
   SCSI_UEXACT8     scdb_length;               /* cdb length                    */
   SCSI_UEXACT8     sg_cache_SCB;              /* # of SCSI_UEXACT8s for each element   */
   union 
   {
      struct 
      {
         SCSI_UEXACT8     sg_pointer_SCB0;     /* pointer to s/g list           */
         SCSI_UEXACT8     sg_pointer_SCB1;     /* pointer to s/g list           */
         SCSI_UEXACT8     sg_pointer_SCB2;     /* pointer to s/g list           */
         SCSI_UEXACT8     sg_pointer_SCB3;     /* pointer to s/g list           */
         SCSI_UEXACT8     sg_pointer_SCB4;     /* pointer to s/g list           */
         SCSI_UEXACT8     sg_pointer_SCB5;     /* pointer to s/g list           */
         SCSI_UEXACT8     sg_pointer_SCB6;     /* pointer to s/g list           */
         SCSI_UEXACT8     sg_pointer_SCB7;     /* pointer to s/g list           */
      } scbSG;
      struct 
      {
         SCSI_UEXACT8     special_opcode;      /* opcode for special scb func   */
         SCSI_UEXACT8     special_info;        /* special info                  */
      } scbSpecial;
   } scb16to23;
   union
   {
      SCSI_UEXACT8    scdb[12];                /* SCSI CDB                      */
      struct 
      {
         SCSI_UEXACT8     scdb0;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb1;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb2;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb3;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb4;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb5;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb6;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb7;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb8;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb9;               /* SCSI CDB                      */
         SCSI_UEXACT8     scdb10;              /* SCSI CDB                      */
         SCSI_UEXACT8     scdb11;              /* SCSI CDB                      */
      } scbCDB;
      SCSI_UEXACT8 scdb_pointer[8];            /* cdb pointer                   */
      struct 
      {
         SCSI_UEXACT8     scdb_pointer0;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer1;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer2;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer3;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer4;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer5;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer6;       /* cdb pointer                   */   
         SCSI_UEXACT8     scdb_pointer7;       /* cdb pointer                   */   
      } scbCDBPTR;
      struct 
      {
         SCSI_UEXACT8     sdata_residue0;      /* total # of bytes not xfer     */
         SCSI_UEXACT8     sdata_residue1;      /* total # of bytes not xfer     */
         SCSI_UEXACT8     sdata_residue2;      /* total # of bytes not xfer     */
         SCSI_UEXACT8     sdata_residue3;      /* total # of bytes not xfer     */
         SCSI_UEXACT8     sg_cache_work;       /* working cache                 */
         SCSI_UEXACT8     sg_pointer_work0;    /* working s/g list pointer      */
         SCSI_UEXACT8     sg_pointer_work1;    /* working s/g list pointer      */
         SCSI_UEXACT8     sg_pointer_work2;    /* working s/g list pointer      */
         SCSI_UEXACT8     sg_pointer_work3;    /* working s/g list pointer      */
      } scbResidualAndWork;
#if SCSI_TARGET_OPERATION
      struct
      {
         SCSI_UEXACT8     sdata_residue0;      /* total # of bytes not xfer     */
         SCSI_UEXACT8     sdata_residue1;      /* total # of bytes not xfer     */
         SCSI_UEXACT8     sdata_residue2;      /* total # of bytes not xfer     */
         SCSI_UEXACT8     sdata_residue3;      /* total # of bytes not xfer     */
         SCSI_UEXACT8     sg_cache_work;       /* working cache                 */
         SCSI_UEXACT8     sg_pointer_work0;    /* working s/g list pointer      */
         SCSI_UEXACT8     sg_pointer_work1;    /* working s/g list pointer      */
         SCSI_UEXACT8     sg_pointer_work2;    /* working s/g list pointer      */
         SCSI_UEXACT8     sg_pointer_work3;    /* working s/g list pointer      */
         SCSI_UEXACT8     reserved;
         SCSI_UEXACT8     starg_tagno;         /* target tag for target mode    */
         SCSI_UEXACT8     starg_status;        /* target status for target mode */
      } scbResidualAndWorkAndTarget;
#endif /* SCSI_TARGET_OPERATION */
   } scb24to35;
   SCSI_UEXACT8     atn_length;                /* attention management          */
#if SCSI_DOMAIN_VALIDATION_BASIC+SCSI_DOMAIN_VALIDATION_ENHANCED
   SCSI_UEXACT8     dv_control;                 /* reserved                      */
#else /* SCSI_DOMAIN_VALIDAITON */
   SCSI_UEXACT8     reserved0;                 /* reserved                      */
#endif /* SCSI_DOMAIN_VALIDATION */
   SCSI_UEXACT8     scontrol;                  /* scb flags                     */
   SCSI_UEXACT8     reserved[25];              /* reserved                      */
} SCSI_SCB_SWAPPING_160M;

#define  SCSI_W160M_sg_pointer_SCB0        scb16to23.scbSG.sg_pointer_SCB0
#define  SCSI_W160M_sg_pointer_SCB1        scb16to23.scbSG.sg_pointer_SCB1
#define  SCSI_W160M_sg_pointer_SCB2        scb16to23.scbSG.sg_pointer_SCB2
#define  SCSI_W160M_sg_pointer_SCB3        scb16to23.scbSG.sg_pointer_SCB3
#define  SCSI_W160M_sg_pointer_SCB4        scb16to23.scbSG.sg_pointer_SCB4
#define  SCSI_W160M_sg_pointer_SCB5        scb16to23.scbSG.sg_pointer_SCB5
#define  SCSI_W160M_sg_pointer_SCB6        scb16to23.scbSG.sg_pointer_SCB6
#define  SCSI_W160M_sg_pointer_SCB7        scb16to23.scbSG.sg_pointer_SCB7
              
#define  SCSI_W160M_special_opcode         scb16to23.scbSpecial.special_opcode
#define  SCSI_W160M_special_info           scb16to23.scbSpecial.special_info
              
#define  SCSI_W160M_scdb                   scb24to35.scdb
#define  SCSI_W160M_scdb0                  scb24to35.scbCDB.scdb0 
#define  SCSI_W160M_scdb1                  scb24to35.scbCDB.scdb1
#define  SCSI_W160M_scdb2                  scb24to35.scbCDB.scdb2
#define  SCSI_W160M_scdb3                  scb24to35.scbCDB.scdb3
#define  SCSI_W160M_scdb4                  scb24to35.scbCDB.scdb4
#define  SCSI_W160M_scdb5                  scb24to35.scbCDB.scdb5
#define  SCSI_W160M_scdb6                  scb24to35.scbCDB.scdb6
#define  SCSI_W160M_scdb7                  scb24to35.scbCDB.scdb7
#define  SCSI_W160M_scdb8                  scb24to35.scbCDB.scdb8
#define  SCSI_W160M_scdb9                  scb24to35.scbCDB.scdb9
#define  SCSI_W160M_scdb10                 scb24to35.scbCDB.scdb10
#define  SCSI_W160M_scdb11                 scb24to35.scbCDB.scdb11
                                                   
#define  SCSI_W160M_scdb_pointer           scb24to35.scdb_pointer
                                                   
#define  SCSI_W160M_scdb_pointer0          scb24to35.scbCDBPTR.scdb_pointer0
#define  SCSI_W160M_scdb_pointer1          scb24to35.scbCDBPTR.scdb_pointer1
#define  SCSI_W160M_scdb_pointer2          scb24to35.scbCDBPTR.scdb_pointer2
#define  SCSI_W160M_scdb_pointer3          scb24to35.scbCDBPTR.scdb_pointer3
#define  SCSI_W160M_scdb_pointer4          scb24to35.scbCDBPTR.scdb_pointer4
#define  SCSI_W160M_scdb_pointer5          scb24to35.scbCDBPTR.scdb_pointer5
#define  SCSI_W160M_scdb_pointer6          scb24to35.scbCDBPTR.scdb_pointer6
#define  SCSI_W160M_scdb_pointer7          scb24to35.scbCDBPTR.scdb_pointer7
                                                   
#define  SCSI_W160M_sdata_residue0         scb24to35.scbResidualAndWork.sdata_residue0
#define  SCSI_W160M_sdata_residue1         scb24to35.scbResidualAndWork.sdata_residue1
#define  SCSI_W160M_sdata_residue2         scb24to35.scbResidualAndWork.sdata_residue2
#define  SCSI_W160M_sdata_residue3         scb24to35.scbResidualAndWork.sdata_residue3
#define  SCSI_W160M_sg_cache_work          scb24to35.scbResidualAndWork.sg_cache_work  
#define  SCSI_W160M_sg_pointer_work0       scb24to35.scbResidualAndWork.sg_pointer_work0
#define  SCSI_W160M_sg_pointer_work1       scb24to35.scbResidualAndWork.sg_pointer_work1
#define  SCSI_W160M_sg_pointer_work2       scb24to35.scbResidualAndWork.sg_pointer_work2
#define  SCSI_W160M_sg_pointer_work3       scb24to35.scbResidualAndWork.sg_pointer_work3
#if SCSI_TARGET_OPERATION
#define  SCSI_W160M_starg_tagno            scb24to35.scbResidualAndWorkAndTarget.starg_tagno
#define  SCSI_W160M_starg_status           scb24to35.scbResidualAndWorkAndTarget.starg_status
#endif /* SCSI_TARGET_OPERATION */
              
/* definitions for scontrol   */
#define  SCSI_W160M_SIMPLETAG  0x00        /* simple tag                 */
#define  SCSI_W160M_HEADTAG    0x01        /* head of queue tag          */
#define  SCSI_W160M_ORDERTAG   0x02        /* ordered tag                */
#define  SCSI_W160M_SPECFUN    0x08        /* special function           */
#define  SCSI_W160M_TAGENB     0x20        /* tag enable                 */
#define  SCSI_W160M_TAGMASK    0x03        /* mask for tags              */
#if SCSI_PARITY_PER_IOB
#define  SCSI_W160M_ENPAR      0x80        /* disable parity checking    */
#endif
#if SCSI_PAC_NSX_REPEATER || SCSI_NSX_REPEATER
#define  SCSI_W160M_NSXENB     0x40        /* enable NSX communication   */
#endif /* SCSI_PAC_NSX_REPEATER || SCSI_NSX_REPEATER */
#if SCSI_DMA_SUPPORT
#define  SCSI_W160M_BLOCK_MOVE 0x04        /* DMA host-to-host block     */
                                           /* move request               */
#endif /* SCSI_DMA_SUPPORT */

#if SCSI_DOMAIN_VALIDATION_BASIC+SCSI_DOMAIN_VALIDATION_ENHANCED
/* definitions for dv_control */
#define  SCSI_W160M_DV_ENABLE  0x01        /* Identify domain validation SCB */
#endif /* SCSI_DOMAIN_VALIDATION_BASIC+SCSI_DOMAIN_VALIDATION_ENHANCED */

#if SCSI_TARGET_OPERATION
#define  SCSI_W160M_TARGETENB  0x04        /* target mode enable         */
#define  SCSI_W160M_DATAOUT    0x01        /* data-out to initiator      */
#define  SCSI_W160M_HOLDONBUS  0x02        /* hold-on to SCSI BUS        */
#define  SCSI_W160M_SENDDISC   0x40        /* send disconnect msg        */
#endif /* SCSI_TARGET_OPERATION */

/* definitions for slun */
#define  SCSI_W160M_DISCENB    0x40        /* disconnect enable          */

#if SCSI_TARGET_OPERATION
/* definitions for starget */
#define  SCSI_W160M_OWNID_MASK 0x0F        /* Mask of starget field      */
#define  SCSI_W160M_OTHERID_MASK 0xF0      /* Mask of starget field      */
#endif /* SCSI_TARGET_OPERATION */

/* definitions for starg_lun */
#define  SCSI_W160M_TARGETID   0xF0        /* target id                  */
#define  SCSI_W160M_TARGETLUN  0x07        /* target lun                 */

/* definitions for scdb_length */
#define  SCSI_W160M_CDBLENMSK  0x1F        /* CDB length                 */
#define  SCSI_W160M_CACHE_ADDR_MASK  0x7C  /* Mask out rollover & lo 2 bits */

/* definitions for sg_cache_SCB */
#define  SCSI_W160M_NODATA     0x01        /* no data transfer involoved */
#define  SCSI_W160M_ONESGSEG   0x02        /* only one sg segment        */

/* definitions for chain_control flags */
#define  SCSI_W160M_ABORTED    0x10        /* aborted flag               */

/* definitions for special function opcode */
#define  SCSI_W160M_MSGTOTARG  0x00        /* special opcode, msg_to_targ */

/***************************************************************************
*  Swapping Ultra 160m Establish Connection SCB format layout
***************************************************************************/
#if SCSI_TARGET_OPERATION
/* structure definition */
typedef struct SCSI_EST_SCB_SWAPPING_160M_
{
   SCSI_UEXACT8     next_SCB;              /* next SCB                      */
   SCSI_UEXACT8     current_SCB;           /* current SCB                   */
   SCSI_UEXACT8     scontrol;              /* scb flags                     */
   SCSI_UEXACT8     reserve0;

   SCSI_UEXACT8     scbaddress0;           /* PCI address of this scb       */
   SCSI_UEXACT8     scbaddress1;           /* PCI address of this scb       */
   SCSI_UEXACT8     scbaddress2;           /* PCI address of this scb       */
   SCSI_UEXACT8     scbaddress3;           /* PCI address of this scb       */
#if (OSD_BUS_ADDRESS_SIZE == 64)
   SCSI_UEXACT8     scbaddress4;           /* PCI address of this scb       */
   SCSI_UEXACT8     scbaddress5;           /* PCI address of this scb       */
   SCSI_UEXACT8     scbaddress6;           /* PCI address of this scb       */
   SCSI_UEXACT8     scbaddress7;           /* PCI address of this scb       */
#else
   SCSI_UEXACT8     padding0;
   SCSI_UEXACT8     padding1;
   SCSI_UEXACT8     padding2;
   SCSI_UEXACT8     padding3;
#endif /* OSD_BUS_ADDRESS_SIZE */
   SCSI_UEXACT8     est_iid;              /* other ID 7:3, own ID 3:0       */
   SCSI_UEXACT8     tag_type;             /* queue tag type received from initiator */
   SCSI_UEXACT8     tag_num;              /* tag number received from initiator     */
   SCSI_UEXACT8     scdb_len;             /* cbd length and flags                   */
   SCSI_UEXACT8     scdb0;                /* SCSI CDB                               */
   SCSI_UEXACT8     scdb1;                /* SCSI CDB                               */
   SCSI_UEXACT8     scdb2;                /* SCSI CDB                               */
   SCSI_UEXACT8     scdb3;                /* SCSI CDB                               */
   SCSI_UEXACT8     scdb4;                /* SCSI CDB                               */
   SCSI_UEXACT8     scdb5;                /* SCSI CDB                               */
   SCSI_UEXACT8     scdb6;                /* SCSI CDB                               */
   SCSI_UEXACT8     scdb7;                /* SCSI CDB                               */
   SCSI_UEXACT8     scdb8;                /* SCSI CDB                               */
   SCSI_UEXACT8     scdb9;                /* SCSI CDB                               */
   SCSI_UEXACT8     scdb10;               /* SCSI CDB                               */
   SCSI_UEXACT8     scdb11;               /* SCSI CDB                               */
   SCSI_UEXACT8     scdb12;               /* SCSI CDB                               */
   SCSI_UEXACT8     scdb13;               /* SCSI CDB                               */
   SCSI_UEXACT8     scdb14;               /* SCSI CDB                               */
   SCSI_UEXACT8     scdb15;               /* SCSI CDB                               */
   SCSI_UEXACT8     id_msg;               /* identify msg byte recvd from initiator */
   SCSI_UEXACT8     last_byte;            /* last byte received from initiator      */
   SCSI_UEXACT8     flags;                /* misc flags related to connection       */
   SCSI_UEXACT8     eststatus;            /* status returned from sequencer         */
   SCSI_UEXACT8     reserved[28];
} SCSI_EST_SCB_SWAPPING_160M;

/* defines for scontrol */
#define SCSI_EST_W160M_SCSI1_EN         0x20  /* SCSI-1 Selection Enable */
#define SCSI_EST_W160M_DISCON_EN        0x10  /* Disconnect enable       */
#define SCSI_EST_W160M_SCSI3_EN         0x08  /* SCSI-3 enable           */
#define SCSI_EST_W160M_TAG_ENABLE       0x04  /* Tagged commands enable  */
#define SCSI_EST_W160M_CHK_RSVD_BITS    0x02  /* Check reserved bits in  */
                                              /* identify message        */
#define SCSI_EST_W160M_LUNTAR_EN        0x01  /* LunTar bit in identify  */
                                              /* message enable          */

/* defines for flags */
#define SCSI_EST_W160M_TAG_RCVD         0x80  /* queue tag message received */
#define SCSI_EST_W160M_SCSI1_SEL        0x40  /* scsi1 selection received */
#define SCSI_EST_W160M_BUS_HELD         0x20  /* scsi bus held */
#define SCSI_EST_W160M_RSVD_BITS        0x10  /* reserved bit set in SCSI-2
                                               * identify message and
                                               * CHK_RSVD_BITS set
                                               */
#define SCSI_EST_W160M_LUNTAR           0x08  /* luntar bit set in SCSI-2
                                               * identify message and
                                               * LUNTAR_EN = 0
                                               */
/* defines for last state */
/* are these still relevant ???? */
#define SCSI_EST_W160M_CMD_PHASE        0x02
#define SCSI_EST_W160M_MSG_OUT_PHASE    0x06
#define SCSI_EST_W160M_MSG_IN_PHASE     0x07

/* definitions for eststatus */
#define SCSI_EST_W160M_GOOD_STATE       0x00 /* command received without exception */
#define SCSI_EST_W160M_SEL_STATE        0x01 /* exception during selection phase */
#define SCSI_EST_W160M_ID_STATE         0x02 /*     "        "   identifying phase */
#define SCSI_EST_W160M_MSGOUT_STATE     0x03 /*     "        "   message out phase */
#define SCSI_EST_W160M_CMD_STATE        0x04 /*     "        "   command phase     */
#define SCSI_EST_W160M_DISC_STATE       0x05 /*     "        "   disconnect phase  */
#define SCSI_EST_W160M_VENDOR_CMD       0x08 /* Vendor unique command */
/* These statuses are stored in the upper nibble of the returned status 
 * so we can handle multiple conditions at once.
 */
#define SCSI_EST_W160M_LAST_RESOURCE 0x40    /* last resource used */
#define SCSI_EST_W160M_PARITY_ERR    0x80    /* parity error       */
/* defines for estiid */
#define SCSI_EST_W160M_SELID_MASK    0x0F    /* our selected ID */
#endif /* SCSI_TARGET_OPERATION */
#endif /* SCSI_SWAPPING_160M_MODE */

/***************************************************************************
* New mode SCSI_QOUTFIFO element structure
***************************************************************************/
typedef struct SCSI_QOUTFIFO_NEW_
{
   SCSI_UEXACT8 scbNumber;                 /* scb number                    */
   SCSI_UEXACT8 padding[6];                /* padding to make DWORD allign  */
   SCSI_UEXACT8 quePassCnt;                /* queue pass count              */
} SCSI_QOUTFIFO_NEW;

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*  All Firmware Descriptor defintions should be referenced only if
*  SCSI_MULTI_MODE is enabled
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
#if SCSI_MULTI_MODE
/***************************************************************************
* FIRMWARE_DESCRIPTOR definition
***************************************************************************/
typedef struct SCSI_FIRMWARE_DESCRIPTOR_
{
   SCSI_INT firmwareVersion;               /* firmware version              */
   SCSI_INT ScbSize;                       /* SCB size                      */
   SCSI_UINT QoutfifoSize;                 /* QOUTFIFO size                 */
   SCSI_UINT reserved;                     /* for alignment and future use  */
   SCSI_UINT16  Siostr3Entry;              /* siostr3 entry                 */
   SCSI_UINT16  AtnTmrEntry;               /* sequencer timer entry address */
   SCSI_UINT16  TargLunMask0;              /* target lun mask               */
   SCSI_UINT16  StartLinkCmdEntry;         /* start link command entry addr */
   SCSI_UINT16  IdleLoopEntry;             /* idle loop entry               */
   SCSI_UINT16  IdleLoop0;                 /* idle loop0                    */
   SCSI_UINT16  ArrayPartition0;           /* array partition 0             */
   SCSI_UINT16  Sio204Entry;               /* sio 204 entry                 */
   SCSI_UINT16  Sio215;                    /* sio 215                       */
   SCSI_UINT16  ExpanderBreak;             /* bus expander break address     */
#if SCSI_TARGET_OPERATION          
   SCSI_UINT16  TargetDataEntry;           /* target data entry             */
   SCSI_UINT16  TargetHeldBusEntry;        /* target held scsi bus entry    */
#endif  /* SCSI_TARGET_OPERATION */  
   SCSI_UEXACT8 BtaTable;                  /* busy target array             */
   SCSI_UEXACT8 RescntBase;                /* residual length base          */
   SCSI_UEXACT8 PassToDriver;              /* pass to driver from sequencer */
   SCSI_UEXACT8 ScsiDatlImage;             /* scsidatl image                */
   SCSI_UEXACT8 ActiveScb;                 /* active scb                    */
   SCSI_UEXACT8 WaitingScb;                /* waiting scb                   */
   SCSI_UEXACT8 XferOption;                /* transfer option               */
   SCSI_UEXACT8 DisconOption;              /* disconnect option             */
   SCSI_UEXACT8 QNewBase;                  /* base address of new SCB queue */
   SCSI_UEXACT8 QNewHead;                  /* queue new head                */
   SCSI_UEXACT8 QNewTail;                  /* queue new tail                */
   SCSI_UEXACT8 QNextScb;                  /* next SCB to be delivered      */
   SCSI_UEXACT8 QDoneBase;                 /* queue done base               */
   SCSI_UEXACT8 QDoneHead;                 /* queue done head               */
   SCSI_UEXACT8 QDoneElement;              /* queue done element            */
   SCSI_UEXACT8 QDonePass;                 /* queue done pass               */
   SCSI_UEXACT8 QExeHead;                  /* queue exe head                */
   SCSI_UEXACT8 QMask;                     /* Qin/Qout mask                 */
   SCSI_UEXACT8 Fast20Low;                 /* fast 20 low                   */
   SCSI_UEXACT8 Fast20High;                /* fast 20 high                  */
   SCSI_UEXACT8 MwiCacheMask;              /* Mwi Cache Mask                */
   SCSI_UEXACT8 EntPtBitmap;               /* bitmap for various ent points */
   SCSI_UEXACT8 AutoRateEnable;            /* workaround for Trident's Auto Rate */
#if SCSI_TRIDENT_PROTO
   SCSI_UEXACT8 ScsiSeqCopy;               /* copy of scsiseq register      */
   SCSI_UEXACT8 reserved1;                 /* for alignment and future use  */
#else /* SCSI_TRIDENT_PROTO */
   SCSI_UEXACT8 reserved1[2];              /* for alignment and future use  */
#endif /* SCSI_TRIDENT_PROTO */
#if SCSI_TARGET_OPERATION 
   /* Scratch RAM entries specific to target mode */
   SCSI_UEXACT8 QEstHead;                  /* host est scb queue offset     */
   SCSI_UEXACT8 QEstTail;                  /* sequencer est scb queue offset */
   SCSI_UEXACT8 QEstNextScb;               /* first est scb to be delivered */
   SCSI_UEXACT8 reserved3[1];              /* for alignment and future use  */
#endif /* SCSI_TARGET_OPERATION */
#if SCSI_RESOURCE_MANAGEMENT
   SCSI_UEXACT8 MaxNonTagScbs;             /* Max Non Tag SCBs              */
   SCSI_UEXACT8 reserved2[3];              /* for alignment and future use  */
#endif
   int (*SCSIhSetupSequencer)(SCSI_HHCB SCSI_HPTR); /* set driver sequencer          */
   void (*SCSIhResetSoftware)(SCSI_HHCB SCSI_HPTR); /* reset software       */
   void (*SCSIhDeliverScb)(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); /* deliver scb */
   void (*SCSIhPatchXferOpt)(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); /* patch xfer option */
   void (*SCSIhEnqueHeadBDR)(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
   SCSI_UEXACT8 (*SCSIhRetrieveScb)(SCSI_HHCB SCSI_HPTR); /* retrieve scb */
   SCSI_UEXACT8 (*SCSIhQoutcnt)(SCSI_HHCB SCSI_HPTR);  /* get qoutcnt value */
   void (*SCSIhTargetClearBusy)(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); /* target clear busy */
   void (*SCSIhRequestSense)(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); /* request sense setup */
   void (*SCSIhResetBTA)(SCSI_HHCB SCSI_HPTR);     /* reset busy target array */
   void (*SCSIhGetConfig)(SCSI_HHCB SCSI_HPTR);              /* get configuration */
   int (*SCSIhConfigureScbRam)(SCSI_HHCB SCSI_HPTR);        /* configure external scb ram */
   void (*SCSIhSetupAssignScbBuffer)(SCSI_HHCB SCSI_HPTR);
   void (*SCSIhAssignScbBuffer)(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
   void (*SCSIhChkCondXferAssign)(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT16); /* assign xfer_option during check condition */
   void (*SCSIhXferAssign)(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT16);
   void (*SCSIhCommonXferAssign)(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT16); /* assign xfer option common     */ 
   void (*SCSIhUpdateXferOptionHostMem)(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT16); /* assign xfer opt host mem */
   SCSI_UEXACT16 (*SCSIhGetXfer)(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
   SCSI_UEXACT8 (*SCSIhGetFast20Reg)(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
   void (*SCSIhClearFast20Reg)(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
   void (*SCSIhLogFast20Map)(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT8);
   void (*SCSIhUpdateFast20HostMem)(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8); /* update fast 20 bit in scb in host mem */ 
   void (*SCSIhUpdateFast20HW)(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8); /* update fast 20 bit in scb in hw */ 
   void (*SCSIhClearAllFast20Reg)(SCSI_HHCB SCSI_HPTR); /* clear all fast 20 registers */
   void (*SCSIhModifyDataPtr)(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
   void (*SCSIhActiveAbort)(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
   void (*SCSIhAbortHIOB)(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
   SCSI_UEXACT8 (*SCSIhSearchSeqDoneQ)(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
   SCSI_UEXACT8 (*SCSIhSearchDoneQ)(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
   SCSI_UEXACT8 (*SCSIhSearchNewQ)(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
   SCSI_UEXACT8 (*SCSIhSearchExeQ)(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8,SCSI_UEXACT8);
   void (*SCSIhUpdateAbortBitHostMem)(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); 
   SCSI_UEXACT8 (*SCSIhObtainNextScbNum)(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);   
   void (*SCSIhUpdateNextScbNum)(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);   
   SCSI_BUS_ADDRESS (*SCSIhObtainNextScbAddress)(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);   
   void (*SCSIhUpdateNextScbAddress)(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_BUS_ADDRESS);   
   void (*SCSIhResetCCCtl)(SCSI_HHCB SCSI_HPTR);
   void (*SCSIhInitCCHHAddr)(SCSI_HHCB SCSI_HPTR);
   void (*SCSIhRemoveActiveAbort)(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
   void (*SCSIhStackBugFix)(SCSI_HHCB SCSI_HPTR);
   void (*SCSIhSetupBDR)(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
   SCSI_UEXACT32 (*SCSIhResidueCalc)(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
   void (*SCSIhSetBreakPoint)(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
   void (*SCSIhClearBreakPoint)(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
   void (*SCSIhUpdateSGLength)(SCSI_HHCB SCSI_HPTR,SCSI_QUADLET);
   void (*SCSIhUpdateSGAddress)(SCSI_HHCB SCSI_HPTR,SCSI_QUADLET);
   void (*SCSIhFreezeHWQueue)(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
   void (*SCSIhSetIntrThreshold)(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
   SCSI_UEXACT8 (*SCSIhGetIntrThreshold)(SCSI_HHCB SCSI_HPTR);
   void (*SCSIhEnableAutoRateOption)(SCSI_HHCB SCSI_HPTR);
   void (*SCSIhDisableAutoRateOption)(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT16);
#if SCSI_RESOURCE_MANAGEMENT
   void (*SCSIrReset)(SCSI_HHCB SCSI_HPTR);   /* reset */
   SCSI_UEXACT8 (*SCSIrGetScb)(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); /* get scb */
   void (*SCSIrReturnScb)(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); /* return scb */
   SCSI_HIOB SCSI_IPTR (*SCSIrHostQueueRemove)(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);   /* remove from host queue */
   void (*SCSIrRemoveScbs)(SCSI_HHCB SCSI_HPTR); /* remove scb */
#if SCSI_TARGET_OPERATION
   SCSI_UEXACT8 (*SCSIrGetEstScb)(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); /* get est scb */  
   void (*SCSIrReturnEstScb)(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); /* return est scb */ 
#endif /* SCSI_TARGET_OPERATION */
#endif /* SCSI_RESOURCE_MANAGEMENT */
#if SCSI_TARGET_OPERATION 
   void (*SCSIhTargetPatchXferRate)(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
   void (*SCSIhTargetSetIgnoreWideMsg)(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
   void (*SCSIhTargetSendHiobSpecial)(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);    /* send hiob special */
   void (*SCSIhTargetGetEstScbFields)(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8); /* get est scb fields */
   void (*SCSIhTargetDeliverEstScb)(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR hiob); /* deliver est scb */
#endif  /* SCSI_TARGET_OPERATION */   
} SCSI_FIRMWARE_DESCRIPTOR;

/***************************************************************************
*  SCSI_FIRMWARE_DESCRIPTOR for standard 64 mode
***************************************************************************/
#if SCSI_STANDARD64_MODE && defined(SCSI_DATA_DEFINE)
int SCSIhStandard64SetupSequencer(SCSI_HHCB SCSI_HPTR);
void SCSIhStandard64ResetSoftware(SCSI_HHCB SCSI_HPTR);
void SCSIhDeliverScb (SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhStandard64QHeadBDR(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIhStandardRetrieveScb(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8 SCSIhStandardQoutcnt(SCSI_HHCB SCSI_HPTR);
void SCSIhStandardTargetClearBusy(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhStandard64RequestSense (SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhStandardResetBTA(SCSI_HHCB SCSI_HPTR);
void SCSIhStandardGetConfig(SCSI_HHCB SCSI_HPTR);
int SCSIhStandardConfigureScbRam(SCSI_HHCB SCSI_HPTR);
void SCSIhSetupAssignScbBuffer(SCSI_HHCB SCSI_HPTR);
void SCSIhAssignScbBuffer(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIrGetScb(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIrReturnScb(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_HIOB SCSI_IPTR SCSIrHostQueueRemove(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIrStandardRemoveScbs (SCSI_HHCB SCSI_HPTR);
void SCSIhStandardXferOptAssign(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT16);
SCSI_UEXACT16 SCSIhStandardGetOption(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhStandardGetFast20Reg(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIhStandardClearFast20Reg(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIhStandardLogFast20Map(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT8);
void SCSIhStandard64ClearAllFast20Reg(SCSI_HHCB SCSI_HPTR);
void SCSIhStandard64ModifyDataPtr(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhStandard64ActiveAbort(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhStandard64AbortHIOB(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhStandardSearchSeqDoneQ(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhStandardSearchDoneQ(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIhStandardSearchNewQ(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhStandard64SearchExeQ(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8,SCSI_UEXACT8);
void SCSIhStandard64UpdateAbortBitHostMem(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); 
SCSI_UEXACT8 SCSIhStandard64ObtainNextScbNum(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);                                                                 
void SCSIhStandard64UpdateNextScbNum(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8); 
SCSI_BUS_ADDRESS SCSIhStandard64ObtainNextScbAddress(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);                                                                 
void SCSIhStandard64UpdateNextScbAddress(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_BUS_ADDRESS);
void SCSIhStandardResetCCCtl(SCSI_HHCB SCSI_HPTR);
void SCSIhStandardInitCCHHAddr(SCSI_HHCB SCSI_HPTR);
void SCSIhStandardRemoveActiveAbort(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhStandard64StackBugFix(SCSI_HHCB SCSI_HPTR);
void SCSIhStandard64SetupBDR(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT32 SCSIhStandard64ResidueCalc(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhStandard64SetBreakPoint(SCSI_HHCB SCSI_HPTR, SCSI_UEXACT8);
void SCSIhStandard64ClearBreakPoint(SCSI_HHCB SCSI_HPTR, SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhStandard64GetIntrThreshold(SCSI_HHCB SCSI_HPTR);
SCSI_FIRMWARE_DESCRIPTOR SCSIFirmwareStandard64 = 
{
   SCSI_S64_FIRMWARE_VERSION,          /* firmware version              */
   SCSI_S64_SIZE_SCB,                  /* SCB size                      */
   sizeof(SCSI_QOUTFIFO_NEW),          /* qoutfifo element size         */
   0,                                  /* for alignment and future use  */
   SCSI_S64_SIOSTR3_ENTRY,             /* siostr3_entry                 */
   SCSI_S64_ATN_TMR_ENTRY,             /* atn_tmr_entry                 */
   SCSI_S64_TARG_LUN_MASK0,            /* targ_lun_mask                 */
   SCSI_S64_START_LINK_CMD_ENTRY,      /* start_link_cmd_entry          */
   SCSI_S64_IDLE_LOOP_ENTRY,           /* idle_loop_entry               */
   SCSI_S64_IDLE_LOOP0,                /* idle_loop0                    */
   SCSI_S64_ARRAY_PARTITION0,          /* array partition0              */
   SCSI_S64_SIO204_ENTRY,              /* sio204_entry                  */
   SCSI_S64_SIO215,                    /* sio215                        */
   SCSI_S64_EXPANDER_BREAK,            /* expander_break                */
#if SCSI_TARGET_OPERATION          
   SCSI_S64_TARGET_DATA_ENTRY,         /* target data entry             */
   SCSI_S64_TARGET_HELD_BUS_ENTRY,     /* target held scsi bus entry    */
#endif  /* SCSI_TARGET_OPERATION */      
   SCSI_S64_BTATABLE,                  /* busy target array table       */
   SCSI_S64_RESCNT_BASE,               /* residual length base          */
   SCSI_S64_PASS_TO_DRIVER,            /* pass_to_driver                */
   SCSI_S64_PASS_TO_DRIVER,            /* scsidatl image                */
   SCSI_S64_ACTIVE_SCB,                /* waiting scb                   */
   SCSI_S64_WAITING_SCB,               /* active scb                    */
   SCSI_S64_XFER_OPTION,               /* transfer option               */
   SCSI_S64_DISCON_OPTION,             /* disconnect option             */
   SCSI_S64_Q_NEW_POINTER,             /* base address of new SCB queue */
   SCSI_S64_Q_NEW_HEAD,                /* queue new head                */
   SCSI_S64_Q_NEW_TAIL,                /* queue new tail                */
   SCSI_S64_Q_NEXT_SCB,                /* next SCB to be delivered      */
   SCSI_S64_Q_DONE_BASE,               /* queue done base               */
   SCSI_S64_Q_DONE_HEAD,               /* queue done head               */
   SCSI_S64_Q_DONE_ELEMENT,            /* queue done element            */
   SCSI_S64_Q_DONE_PASS,               /* queue done pass               */
   SCSI_S64_Q_EXE_HEAD,                /* queue exe head                */
   SCSI_S64_Q_MASK,                    /* queue masks for in/out        */
   SCSI_S64_FAST20_LOW,                /* fast 20 low byte              */
   SCSI_S64_FAST20_HIGH,               /* fast 20 high byte             */
   SCSI_S64_MWI_CACHE_MASK,            /* Mwi Cache Mask                */
   SCSI_S64_ENT_PT_BITMAP,             /* bitmap for various brk points */
   SCSI_S64_AUTO_RATE_ENABLE,          /* workaround for Trident's auto rate */
#if SCSI_TRIDENT_PROTO
   SCSI_S64_SCSISEQ_COPY,              /* copy of scsiseq register      */
   0,                                  /* reserved1                     */
#else /* SCSI_TRIDENT_PROTO */
   {
      0,0                              /* reserved1                     */
   },
#endif /* SCSI_TRIDENT_PROTO */
#if SCSI_TARGET_OPERATION
   SCSI_S64_Q_EST_HEAD,                /* Not used                      */
   SCSI_S64_Q_EST_TAIL,                /* Not used                      */
   SCSI_S64_Q_EST_NEXT_SCB,            /* Not used                      */
   0,                                  /* reserved for future use       */
#endif /* SCSI_TARGET_OPERATION */
#if SCSI_RESOURCE_MANAGEMENT
   SCSI_S64_MAX_NONTAG_SCBS,           /* Max Non Tag SCBs              */
   {
      0,0,0                            /* reserved2                     */
   },
#endif
   SCSIhStandard64SetupSequencer,      /* setup sequencer               */
   SCSIhStandard64ResetSoftware,       /* reset software                */
   SCSIhDeliverScb,                    /* deliver scb                   */
   0,                                  /* patch xfer option             */
   SCSIhStandard64QHeadBDR,            /* enqueue at head of exe. queue */
   SCSIhStandardRetrieveScb,           /* retrieve scb                  */
   SCSIhStandardQoutcnt,               /* check qoutcnt value           */
   SCSIhStandardTargetClearBusy,       /* clear busy target             */
   SCSIhStandard64RequestSense,        /* request sense setup           */
   SCSIhStandardResetBTA,              /* reste busy target array       */
   SCSIhStandardGetConfig,             /* get driver config info        */
   SCSIhStandardConfigureScbRam,       /* configure scb ram             */
#if SCSI_SCBBFR_BUILTIN
   SCSIhSetupAssignScbBuffer,          /* set assign scb buffer         */
   0                                   /* assign scb buffer (SCSIhAssignScbBuffer) */
#else
   0,
   0
#endif
   ,
   SCSIhStandardXferOptAssign,         /* assign xfer option during check condition */
   SCSIhStandardXferOptAssign,         /* assign xfer option            */
   0,                                  /* assign xfer option common     */ 
   0,                                  /* assign xfer option host mem   */ 
   SCSIhStandardGetOption,             /* get xfer option               */
   SCSIhStandardGetFast20Reg,          /* get fast20 registers          */
   SCSIhStandardClearFast20Reg,        /* clear fast20 registers        */
   SCSIhStandardLogFast20Map,          /* log fast20 registers          */
   0,                                  /* update fast 20 bit in scb in host mem */ 
   0,                                  /* update fast 20 bit in scb in hw */  
   SCSIhStandard64ClearAllFast20Reg,   /* clear all fast 20 registers */ 
   SCSIhStandard64ModifyDataPtr,       /* modify data pointer message   */
   SCSIhStandard64ActiveAbort,         /* aborting active HIOB          */
   SCSIhStandard64AbortHIOB,           /* aborting HIOB in queues       */
   SCSIhStandardSearchSeqDoneQ,        /* search seq done Q. for abort HIOB */
   SCSIhStandardSearchDoneQ,           /* search Done Q. for abort HIOB */
   SCSIhStandardSearchNewQ,            /* search New Q. for abort HIOB */
   SCSIhStandard64SearchExeQ,          /* search Exe. Q. for abort HIOB */
   SCSIhStandard64UpdateAbortBitHostMem, /* update abort bit in host mem */
   SCSIhStandard64ObtainNextScbNum,    /* obtain next SCB number in SCB */ 
   SCSIhStandard64UpdateNextScbNum,    /* update next SCB number in SCB */ 
   SCSIhStandard64ObtainNextScbAddress, /* obtain next SCB address in SCB */ 
   SCSIhStandard64UpdateNextScbAddress, /* update next SCB address in SCB */ 
   SCSIhStandardResetCCCtl,            /* reset CCSCBCTL and CCSGCTL    */
   SCSIhStandardInitCCHHAddr,          /* initialize CCHHAddr           */
   SCSIhStandardRemoveActiveAbort,     /* remove active abort           */
   SCSIhStandard64StackBugFix,         /* fix sequencer stack problems  */
   SCSIhStandard64SetupBDR,            /* setup bus device reset scb    */
   SCSIhStandard64ResidueCalc,         /* calculate residual length     */
   SCSIhStandard64SetBreakPoint,       /* set breakpoint                */
   SCSIhStandard64ClearBreakPoint,     /* clear breakpoint              */
   0,                                  /* update SCB slength0-2         */
   0,                                  /* update SCB saddress0-3        */
   0,                                  /* freeze h/w queue              */
   0,                                  /* set intr threshold value      */
   SCSIhStandard64GetIntrThreshold,    /* get intr threshold value      */
   0,                                  /* enable auto rate feature      */
   0                                   /* disable auto rate feature     */
#if SCSI_RESOURCE_MANAGEMENT
   ,
   0,                                  /* reset                         */
   SCSIrGetScb,                        /* get scb                       */
   SCSIrReturnScb,                     /* return scb                    */
   SCSIrHostQueueRemove,               /* remove from host queue        */
   SCSIrStandardRemoveScbs             /* remove scbs                   */
#if SCSI_TARGET_OPERATION
   ,
   0,                                  /* get est scb                   */              
   0                                   /* return est scb                */
#endif /* SCSI_TARGET_OPERATION */
#endif /* SCSI_RESOURCE_MANAGEMENT */
#if SCSI_TARGET_OPERATION              
   , 
   0,                                  /* patch transfer rate into SCB  */
   0,                                  /* set ignore wide residue       */ 
   0,                                  /* send hiob special             */
   0,                                  /* get est scb fields            */
   0                                   /* deliver est scb               */
#endif  /* SCSI_TARGET_OPERATION */  
};
#endif

/***************************************************************************
*  SCSI_FIRMWARE_DESCRIPTOR for standard 32 mode
***************************************************************************/
#if SCSI_STANDARD32_MODE && defined(SCSI_DATA_DEFINE)
SCSI_FIRMWARE_DESCRIPTOR SCSIFirmwareStandard32 = 
{
};
#endif

/***************************************************************************
*  SCSI_FIRMWARE_DESCRIPTOR for swapping 64 mode
***************************************************************************/
#if SCSI_SWAPPING64_MODE && defined(SCSI_DATA_DEFINE)

int SCSIhSwapping64SetupSequencer(SCSI_HHCB SCSI_HPTR);
void SCSIhSwappingResetSoftware(SCSI_HHCB SCSI_HPTR); 
void SCSIhDeliverScb (SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwapping64PatchXferOpt(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwapping64QHeadBDR(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIhStandardRetrieveScb(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8 SCSIhStandardQoutcnt(SCSI_HHCB SCSI_HPTR);
void SCSIhSwappingTargetClearBusy(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwapping64RequestSense (SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwappingResetBTA(SCSI_HHCB SCSI_HPTR);
void SCSIhNonStandardGetConfig(SCSI_HHCB SCSI_HPTR);
void SCSIhSetupAssignScbBuffer(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8 SCSIrGetScb(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIrReturnScb(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_HIOB SCSI_IPTR SCSIrHostQueueRemove(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIhChkCondXferAssign(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT16);
void SCSIhHostXferOptAssign(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT16);
void SCSIhSwapping64CommonXferAssign(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT16);
void SCSIhSwapping64UpdateXferOptionHostMem (SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT16);
SCSI_UEXACT16 SCSIhHostGetOption(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhSwappingGetFast20Reg(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIhSwappingClearFast20Reg(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIhSwappingLogFast20Map(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT8);
void SCSIhSwapping64UpdateFast20HostMem (SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
void SCSIhSwapping64UpdateFast20HW (SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIhSwappingClearAllFast20Reg(SCSI_HHCB SCSI_HPTR);
void SCSIhSwapping64ModifyDataPtr(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwapping64ActiveAbort(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwappingAbortHIOB(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR, SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhStandardSearchDoneQ(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIhSwappingSearchNewQ(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR, SCSI_UEXACT8);
void SCSIhSwapping64UpdateAbortBitHostMem(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); 
SCSI_UEXACT8 SCSIhSwapping64ObtainNextScbNum(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);                                                                 
void SCSIhSwapping64UpdateNextScbNum(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8); 
void SCSIhSwappingRemoveActiveAbort(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwapping64SetupBDR(SCSI_HHCB SCSI_HPTR, SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT32 SCSIhSwapping64ResidueCalc(SCSI_HHCB SCSI_HPTR, SCSI_HIOB SCSI_IPTR);
void SCSIhSwapping64SetBreakPoint(SCSI_HHCB SCSI_HPTR, SCSI_UEXACT8);
void SCSIhSwapping64ClearBreakPoint(SCSI_HHCB SCSI_HPTR, SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhSwappingGetIntrThreshold(SCSI_HHCB SCSI_HPTR);
SCSI_FIRMWARE_DESCRIPTOR SCSIFirmwareSwapping64 = 
{
   SCSI_W64_FIRMWARE_VERSION,          /* firmware version              */
   SCSI_W64_SIZE_SCB,                  /* SCB size                      */
   sizeof(SCSI_QOUTFIFO_NEW),          /* qoutfifo element size         */
   0,                                  /* for alignment and future use  */
   SCSI_W64_SIOSTR3_ENTRY,             /* siostr3_entry                 */
   SCSI_W64_ATN_TMR_ENTRY,             /* atn_tmr_entry                 */
   SCSI_W64_TARG_LUN_MASK0,            /* targ_lun_mask                 */
   SCSI_W64_START_LINK_CMD_ENTRY,      /* start_link_cmd_entry          */
   SCSI_W64_IDLE_LOOP_ENTRY,           /* idle_loop_entry               */
   SCSI_W64_IDLE_LOOP0,                /* idle_loop0                    */
   SCSI_W64_ARRAY_PARTITION0,          /* array partition0              */
   SCSI_W64_SIO204_ENTRY,              /* sio204_entry                  */
   SCSI_W64_SIO215,                    /* sio215                        */
   SCSI_W64_EXPANDER_BREAK,            /* expander_break                */
#if SCSI_TARGET_OPERATION          
   SCSI_W64_TARGET_DATA_ENTRY,         /* target data entry             */
   SCSI_W64_TARGET_HELD_BUS_ENTRY,     /* target held scsi bus entry    */
#endif  /* SCSI_TARGET_OPERATION */  
   SCSI_W64_BTATABLE,                  /* busy target array table       */
   SCSI_W64_RESCNT_BASE,               /* residual length base          */
   SCSI_W64_PASS_TO_DRIVER,            /* pass_to_driver                */
   SCSI_W64_PASS_TO_DRIVER,            /* scsidatl image                */
   SCSI_W64_ACTIVE_SCB,                /* waiting scb                   */
   SCSI_W64_WAITING_SCB,               /* active scb                    */
   SCSI_W64_XFER_OPTION,               /* transfer option               */
   SCSI_W64_DISCON_OPTION,             /* disconnect option             */
   0,                                  /* base address of new SCB queue */
   SCSI_W64_Q_NEW_HEAD,                /* queue new head                */
   SCSI_W64_Q_NEW_TAIL,                /* queue new tail                */
   SCSI_W64_Q_NEXT_SCB,                /* next SCB to be delivered      */
   SCSI_W64_Q_DONE_BASE,               /* queue done base               */
   SCSI_W64_Q_DONE_HEAD,               /* queue done head               */
   SCSI_W64_Q_DONE_ELEMENT,            /* queue done element            */
   SCSI_W64_Q_DONE_PASS,               /* queue done pass               */
   SCSI_W64_Q_EXE_HEAD,                /* queue exe head                */
   SCSI_W64_Q_MASK,                    /* queue masks for in/out        */
   SCSI_W64_FAST20_LOW,                /* fast 20 low byte              */
   SCSI_W64_FAST20_HIGH,               /* fast 20 high byte             */
   SCSI_W64_MWI_CACHE_MASK,            /* Mwi Cache Mask                */
   SCSI_W64_ENT_PT_BITMAP,             /* bitmap for various brk points */
   SCSI_W64_AUTO_RATE_ENABLE,          /* workaround for Trident's auto rate */
#if SCSI_TRIDENT_PROTO
   SCSI_W64_SCSISEQ_COPY,              /* copy of scsiseq register      */
   0,                                  /* reserved1                     */
#else /* SCSI_TRIDENT_PROTO */
   {
      0,0                              /* reserved1                     */
   },
#endif /* SCSI_TRIDENT_PROTO */
#if SCSI_TARGET_OPERATION
   SCSI_W64_Q_EST_HEAD,                /* Not used                      */
   SCSI_W64_Q_EST_TAIL,                /* Not used                      */     
   SCSI_W64_Q_EST_NEXT_SCB,            /* Not used                      */   
   0,                                  /* reserved for future use       */
#endif /* SCSI_TARGET_OPERATION */
#if SCSI_RESOURCE_MANAGEMENT
   SCSI_W64_MAX_NONTAG_SCBS,           /* Max Non Tag SCBs              */
   {
      0,0,0                            /* reserved2                     */
   },
#endif
   SCSIhSwapping64SetupSequencer,      /* setup sequencer               */
   SCSIhSwappingResetSoftware,         /* reset software                */
   SCSIhDeliverScb,                    /* deliver scb                   */
   SCSIhSwapping64PatchXferOpt,        /* patch xfer option             */
   SCSIhSwapping64QHeadBDR,            /* enqueue at head of new queue  */
   SCSIhStandardRetrieveScb,           /* retrieve scb                  */
   SCSIhStandardQoutcnt,               /* check qoutcnt value           */
   SCSIhSwappingTargetClearBusy,       /* clear busy target             */
   SCSIhSwapping64RequestSense,        /* setup request sense  ???      */
   SCSIhSwappingResetBTA,              /* reset busy target array       */
   SCSIhNonStandardGetConfig,          /* get driver config info        */
   0,                                  /* configure scb ram             */
   SCSIhSetupAssignScbBuffer,          /* setup assign scb buffer       */
   0,                                  /* assign scb buffer (SCSIhAssignScbBuffer) */
   SCSIhChkCondXferAssign,             /* assign xfer option during check condition */
   SCSIhHostXferOptAssign,             /* assign xfer option            */
   SCSIhSwapping64CommonXferAssign,    /* assign xfer option common     */
   SCSIhSwapping64UpdateXferOptionHostMem,   /* assign xfer opt host mem */
   SCSIhHostGetOption,                 /* get xfer option               */
   SCSIhSwappingGetFast20Reg,          /* get fast20 registers          */
   SCSIhSwappingClearFast20Reg,        /* clear fast20 registers        */
   SCSIhSwappingLogFast20Map,          /* log fast20 registers          */
   SCSIhSwapping64UpdateFast20HostMem, /* update fast 20 bit in scb in host mem */
   SCSIhSwapping64UpdateFast20HW,      /* update fast 20 bit in scb in hw */
   SCSIhSwappingClearAllFast20Reg,     /* clear all fast 20 registers    */
   SCSIhSwapping64ModifyDataPtr,       /* modify data pointer message   */
   SCSIhSwapping64ActiveAbort,         /* aborting active HIOB          */
   SCSIhSwappingAbortHIOB,             /* aborting HIOB in queues       */
   0,                                  /* search seq done Q. for abort HIOB */
   SCSIhStandardSearchDoneQ,           /* search Done Q. for abort HIOB */
   SCSIhSwappingSearchNewQ,            /* search New Q. for abort HIOB  */
   0,                                  /* search Exe. Q. for abort HIOB */
   SCSIhSwapping64UpdateAbortBitHostMem, /* update abort bit in host mem */
   SCSIhSwapping64ObtainNextScbNum,    /* obtain next SCB number in SCB */ 
   SCSIhSwapping64UpdateNextScbNum,    /* update next SCB number in SCB */ 
   0,                                  /* obtain next SCB address in SCB */ 
   0,                                  /* update next SCB address in SCB */ 
   0,                                  /* reset CCSCBCTL and CCSGCTL    */
   0,                                  /* initialize CCHHAddr           */
   SCSIhSwappingRemoveActiveAbort,     /* remove active abort           */
   0,                                  /* fix sequencer stack problems  */
   SCSIhSwapping64SetupBDR,            /* setup bus device reset scb    */
   SCSIhSwapping64ResidueCalc,         /* calculate residual length     */
   SCSIhSwapping64SetBreakPoint,       /* set breakpoint                */
   SCSIhSwapping64ClearBreakPoint,     /* clear breakpoint              */
   0,                                  /* update SCB slength0-2         */
   0,                                  /* update SCB saddress0-3        */
   0,                                  /* freeze h/w queue              */
   0,                                  /* set intr threshold value      */
   SCSIhSwappingGetIntrThreshold,      /* get intr threshold value      */
   0,                                  /* enable auto rate feature      */
   0                                   /* disable auto rate feature     */
#if SCSI_RESOURCE_MANAGEMENT
   ,
   0,                                  /* reset                         */
   SCSIrGetScb,                        /* get scb                       */
   SCSIrReturnScb,                     /* return scb                    */
   SCSIrHostQueueRemove,               /* remove from host queue        */
   0                                   /* remove scbs                   */
#if SCSI_TARGET_OPERATION
   ,
   0,                                  /* get est scb                   */              
   0                                   /* return est scb                */
#endif /* SCSI_TARGET_OPERATION */
#endif /* SCSI_RESOURCE_MANAGEMENT */
#if SCSI_TARGET_OPERATION              
   , 
   0,                                  /* patch transfer rate into SCB  */
   0,                                  /* set ignore wide residue       */ 
   0,                                  /* send hiob special             */
   0,                                  /* get est scb fields            */
   0                                   /* deliver est scb               */
#endif  /* SCSI_TARGET_OPERATION */ 
};

#endif

/***************************************************************************
*  SCSI_FIRMWARE_DESCRIPTOR for swapping 32 mode
***************************************************************************/
#if SCSI_SWAPPING32_MODE && defined(SCSI_DATA_DEFINE)
int SCSIhSwapping32SetupSequencer(SCSI_HHCB SCSI_HPTR);
void SCSIhSwappingResetSoftware(SCSI_HHCB SCSI_HPTR); 
void SCSIhDeliverScb (SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwapping32PatchXferOpt(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwapping32QHeadBDR(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIhStandardRetrieveScb(SCSI_HHCB SCSI_HPTR); 
SCSI_UEXACT8 SCSIhStandardQoutcnt(SCSI_HHCB SCSI_HPTR);
void SCSIhSwappingTargetClearBusy(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); 
void SCSIhSwapping32RequestSense (SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwappingResetBTA(SCSI_HHCB SCSI_HPTR); 
void SCSIhNonStandardGetConfig(SCSI_HHCB SCSI_HPTR);
void SCSIhSetupAssignScbBuffer(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8 SCSIrGetScb(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIrReturnScb(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_HIOB SCSI_IPTR SCSIrHostQueueRemove(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8); 
void SCSIhChkCondXferAssign(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT16);
void SCSIhHostXferOptAssign(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT16);
void SCSIhSwapping32CommonXferAssign(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT16);
void SCSIhSwapping32UpdateXferOptionHostMem (SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT16);
SCSI_UEXACT16 SCSIhHostGetOption(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhSwappingGetFast20Reg(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8); 
void SCSIhSwappingClearFast20Reg(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8); 
void SCSIhSwappingLogFast20Map(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT8); 
void SCSIhSwapping32UpdateFast20HostMem (SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8); 
void SCSIhSwapping32UpdateFast20HW (SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8); 
void SCSIhSwappingClearAllFast20Reg(SCSI_HHCB SCSI_HPTR);
void SCSIhSwapping32ModifyDataPtr(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwapping32ActiveAbort(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwappingAbortHIOB(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR, SCSI_UEXACT8); 
SCSI_UEXACT8 SCSIhStandardSearchDoneQ(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIhSwappingSearchNewQ(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR, SCSI_UEXACT8);
void SCSIhSwapping32UpdateAbortBitHostMem(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); 
SCSI_UEXACT8 SCSIhSwapping32ObtainNextScbNum(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwapping32UpdateNextScbNum(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);    
void SCSIhSwappingRemoveActiveAbort(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwapping32SetupBDR(SCSI_HHCB SCSI_HPTR, SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT32 SCSIhSwapping32ResidueCalc(SCSI_HHCB SCSI_HPTR, SCSI_HIOB SCSI_IPTR);
void SCSIhSwapping32SetBreakPoint(SCSI_HHCB SCSI_HPTR, SCSI_UEXACT8);
void SCSIhSwapping32ClearBreakPoint(SCSI_HHCB SCSI_HPTR, SCSI_UEXACT8);
void SCSIhSwapping32FreezeHWQueue(SCSI_HHCB SCSI_HPTR, SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIhSwappingGetIntrThreshold(SCSI_HHCB SCSI_HPTR);
SCSI_FIRMWARE_DESCRIPTOR SCSIFirmwareSwapping32 = 
{
   SCSI_W32_FIRMWARE_VERSION,          /* firmware version              */
   SCSI_W32_SIZE_SCB,                  /* SCB size                      */
   sizeof(SCSI_QOUTFIFO_NEW),          /* qoutfifo element size         */
   0,                                  /* for alignment and future use  */
   SCSI_W32_SIOSTR3_ENTRY,             /* siostr3_entry                 */
   SCSI_W32_ATN_TMR_ENTRY,             /* atn_tmr_entry                 */
   SCSI_W32_TARG_LUN_MASK0,            /* targ_lun_mask                 */
   SCSI_W32_START_LINK_CMD_ENTRY,      /* start_link_cmd_entry          */
   SCSI_W32_IDLE_LOOP_ENTRY,           /* idle_loop_entry               */
   SCSI_W32_IDLE_LOOP0,                /* idle_loop0                    */
   SCSI_W32_ARRAY_PARTITION0,          /* array partition0              */
   SCSI_W32_SIO204_ENTRY,              /* sio204_entry                  */
   SCSI_W32_SIO215,                    /* sio215                        */
   SCSI_W32_EXPANDER_BREAK,            /* expander_break                */
#if SCSI_TARGET_OPERATION          
   SCSI_W32_TARGET_DATA_ENTRY,         /* target data entry             */
   SCSI_W32_TARGET_HELD_BUS_ENTRY,     /* target held scsi bus entry    */
#endif  /* SCSI_TARGET_OPERATION */  
   SCSI_W32_BTATABLE,                  /* busy target array table       */
   SCSI_W32_RESCNT_BASE,               /* residual length base          */
   SCSI_W32_PASS_TO_DRIVER,            /* pass_to_driver                */
   SCSI_W32_PASS_TO_DRIVER,            /* scsidatl image                */
   SCSI_W32_ACTIVE_SCB,                /* waiting scb                   */
   SCSI_W32_WAITING_SCB,               /* active scb                    */
   SCSI_W32_XFER_OPTION,               /* transfer option               */
   SCSI_W32_DISCON_OPTION,             /* disconnect option             */
   0,                                  /* base address of new SCB queue */
   SCSI_W32_Q_NEW_HEAD,                /* queue new head                */
   SCSI_W32_Q_NEW_TAIL,                /* queue new tail                */
   SCSI_W32_Q_NEXT_SCB,                /* next SCB to be delivered      */
   SCSI_W32_Q_DONE_BASE,               /* queue done base               */
   SCSI_W32_Q_DONE_HEAD,               /* queue done head               */
   SCSI_W32_Q_DONE_ELEMENT,            /* queue done element            */
   SCSI_W32_Q_DONE_PASS,               /* queue done pass               */
   SCSI_W32_Q_EXE_HEAD,                /* queue exe head                */
   SCSI_W32_Q_MASK,                    /* queue masks for in/out        */
   SCSI_W32_FAST20_LOW,                /* fast 20 low byte              */
   SCSI_W32_FAST20_HIGH,               /* fast 20 high byte             */
   SCSI_W32_MWI_CACHE_MASK,            /* Mwi Cache Mask                */
   SCSI_W32_ENT_PT_BITMAP,             /* bitmap for various brk points */
   SCSI_W32_AUTO_RATE_ENABLE,          /* workaround for Trident's auto rate */
#if SCSI_TRIDENT_PROTO
   SCSI_W32_SCSISEQ_COPY,              /* copy of scsiseq register      */
   0,                                  /* reserved1                     */
#else /* SCSI_TRIDENT_PROTO */
   {
      0,0                              /* reserved1                     */
   },
#endif /* SCSI_TRIDENT_PROTO */
#if SCSI_TARGET_OPERATION
   SCSI_W_Q_EST_HEAD,                  /* Not used                      */
   SCSI_W_Q_EST_TAIL,                  /* Not used                      */
   SCSI_W_Q_EST_NEXT_SCB,              /* Not used                      */
   0,                                  /* reserved for future use       */
#endif /* SCSI_TARGET_OPERATION */
#if SCSI_RESOURCE_MANAGEMENT
   SCSI_W32_MAX_NONTAG_SCBS,           /* Max Non Tag SCBs              */
   {
      0,0,0                            /* reserved2                     */
   },
#endif
   SCSIhSwapping32SetupSequencer,      /* setup sequencer               */
   SCSIhSwappingResetSoftware,         /* reset software                */ 
   SCSIhDeliverScb,                    /* deliver scb                   */
   SCSIhSwapping32PatchXferOpt,        /* patch xfer option             */
   SCSIhSwapping32QHeadBDR,            /* enqueue at head of new queue  */ 
   SCSIhStandardRetrieveScb,           /* retrieve scb                  */
   SCSIhStandardQoutcnt,               /* check qoutcnt value           */
   SCSIhSwappingTargetClearBusy,       /* clear busy target             */ 
   SCSIhSwapping32RequestSense,        /* setup request sense  ???      */
   SCSIhSwappingResetBTA,              /* reset busy target array       */ 
   SCSIhNonStandardGetConfig,          /* get driver config info        */
   0,                                  /* configure scb ram             */
   SCSIhSetupAssignScbBuffer,          /* setup assign scb buffer       */
   0,                                  /* assign scb buffer (SCSIhAssignScbBuffer) */
   SCSIhChkCondXferAssign,             /* assign xfer option during check condition */
   SCSIhHostXferOptAssign,             /* assign xfer option            */
   SCSIhSwapping32CommonXferAssign,    /* assign xfer option common     */ 
   SCSIhSwapping32UpdateXferOptionHostMem,   /* assign xfer opt host mem */ 
   SCSIhHostGetOption,                 /* get xfer option               */
   SCSIhSwappingGetFast20Reg,          /* get fast20 registers          */
   SCSIhSwappingClearFast20Reg,        /* clear fast20 registers        */
   SCSIhSwappingLogFast20Map,          /* log fast20 registers          */
   SCSIhSwapping32UpdateFast20HostMem, /* update fast 20 bit in scb in host mem */ 
   SCSIhSwapping32UpdateFast20HW,      /* update fast 20 bit in scb in hw */ 
   SCSIhSwappingClearAllFast20Reg,     /* clear all fast 20 registers     */
   SCSIhSwapping32ModifyDataPtr,       /* modify data pointer message   */
   SCSIhSwapping32ActiveAbort,         /* aborting active HIOB          */
   SCSIhSwappingAbortHIOB,             /* aborting HIOB in queues       */ 
   0,                                  /* search seq done Q. for abort HIOB */
   SCSIhStandardSearchDoneQ,           /* search Done Q. for abort HIOB */
   SCSIhSwappingSearchNewQ,            /* search New Q. for abort HIOB  */ 
   0,                                  /* search Exe. Q. for abort HIOB */
   SCSIhSwapping32UpdateAbortBitHostMem, /* update abort bit in host mem */ 
   SCSIhSwapping32ObtainNextScbNum,    /* obtain next SCB number in SCB */ 
   SCSIhSwapping32UpdateNextScbNum,    /* update next SCB number in SCB */ 
   0,                                  /* obtain next SCB address in SCB */ 
   0,                                  /* update next SCB address in SCB */ 
   0,                                  /* Reset CCSCBCTL and CCSGCTL    */
   0,                                  /* initialize CCHHAddr           */
   SCSIhSwappingRemoveActiveAbort,     /* remove active abort           */
   0,                                  /* fix sequencer stack problems  */
   SCSIhSwapping32SetupBDR,            /* setup bus device reset scb    */
   SCSIhSwapping32ResidueCalc,         /* calculate residual length     */
   SCSIhSwapping32SetBreakPoint,       /* set breakpoint                */
   SCSIhSwapping32ClearBreakPoint,     /* clear breakpoint              */
   0,                                  /* update SCB slength0-2         */
   0,                                  /* update SCB saddress0-3        */
   SCSIhSwapping32FreezeHWQueue,       /* freeze h/w queue              */
   0,                                  /* set intr threshold value      */
   SCSIhSwappingGetIntrThreshold,      /* get intr threshold value      */
   0,                                  /* enable auto rate feature      */
   0                                   /* disable auto rate feature     */
#if SCSI_RESOURCE_MANAGEMENT
   ,
   0,                                  /* reset                         */
   SCSIrGetScb,                        /* get scb                       */
   SCSIrReturnScb,                     /* return scb                    */
   SCSIrHostQueueRemove,               /* remove from host queue        */
   0                                   /* remove scbs                   */
#if SCSI_TARGET_OPERATION
   ,
   0,                                  /* get est scb                   */              
   0                                   /* return est scb                */
#endif /* SCSI_TARGET_OPERATION */
#endif /* SCSI_RESOURCE_MANAGEMENT */
#if SCSI_TARGET_OPERATION              
   , 
   0,                                  /* patch transfer rate into SCB  */
   0,                                  /* set ignore wide residue       */ 
   0,                                  /* send hiob special             */
   0,                                  /* get est scb fields            */
   0                                   /* deliver est scb               */
#endif  /* SCSI_TARGET_OPERATION */ 
};
#endif

/***************************************************************************
*  SCSI_FIRMWARE_DESCRIPTOR for standard bayonet mode 
***************************************************************************/
#if SCSI_STANDARD_ADVANCED_MODE && defined(SCSI_DATA_DEFINE)
int SCSIhStandardAdvSetupSequencer(SCSI_HHCB SCSI_HPTR);
void SCSIhStandardAdvResetSoftware(SCSI_HHCB SCSI_HPTR);
void SCSIhDeliverScb (SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhStandardAdvPatchXferOpt(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhStandardAdvQHeadBDR(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIhStandardRetrieveScb(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8 SCSIhStandardQoutcnt(SCSI_HHCB SCSI_HPTR);
void SCSIhStandardTargetClearBusy(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhStandardAdvRequestSense (SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhStandardResetBTA(SCSI_HHCB SCSI_HPTR);
void SCSIhStandardGetConfig(SCSI_HHCB SCSI_HPTR);
int SCSIhStandardConfigureScbRam(SCSI_HHCB SCSI_HPTR);
void SCSIhSetupAssignScbBuffer(SCSI_HHCB SCSI_HPTR);
void SCSIhStandardAssignScbBuffer(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIrGetScb(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIrReturnScb(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_HIOB SCSI_IPTR SCSIrHostQueueRemove(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIrStandardRemoveScbs (SCSI_HHCB SCSI_HPTR);
void SCSIhHostXferOptAssign(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT16);
void SCSIhChkCondXferAssign(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT16);
void SCSIhStandardAdvCommonXferAssign(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT16);
void SCSIhStandardAdvUpdateXferOptionHostMem (SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT16);
SCSI_UEXACT16 SCSIhHostGetOption(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhStandardAdvGetFast20Reg(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIhStandardAdvModifyDataPtr(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); 
void SCSIhStandardAdvActiveAbort(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhStandardAdvAbortHIOB(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhStandardAdvSearchSeqDoneQ(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhStandardSearchDoneQ(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIhStandardSearchNewQ(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhStandardAdvSearchExeQ(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8,SCSI_UEXACT8);
void SCSIhStandardAdvUpdateAbortBitHostMem(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); 
SCSI_UEXACT8 SCSIhStandardAdvObtainNextScbNum(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhStandardAdvUpdateNextScbNum(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);  
SCSI_BUS_ADDRESS SCSIhStandardAdvObtainNextScbAddress(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhStandardAdvUpdateNextScbAddress(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_BUS_ADDRESS);
void SCSIhStandardResetCCCtl(SCSI_HHCB SCSI_HPTR);
void SCSIhStandardInitCCHHAddr(SCSI_HHCB SCSI_HPTR);
void SCSIhStandardRemoveActiveAbort(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhStandardAdvSetupBDR(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT32 SCSIhStandardAdvResidueCalc(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhStandardAdvSetBreakPoint(SCSI_HHCB SCSI_HPTR, SCSI_UEXACT8);
void SCSIhStandardAdvClearBreakPoint(SCSI_HHCB SCSI_HPTR, SCSI_UEXACT8);
void SCSIhStandardAdvUpdateSGLength(SCSI_HHCB SCSI_HPTR, SCSI_QUADLET);
void SCSIhStandardAdvUpdateSGAddress(SCSI_HHCB SCSI_HPTR, SCSI_QUADLET);
void SCSIhStandardAdvFreezeHWQueue(SCSI_HHCB SCSI_HPTR, SCSI_HIOB SCSI_IPTR);
void SCSIhStandardAdvSetIntrThreshold(SCSI_HHCB SCSI_HPTR, SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhStandardAdvGetIntrThreshold(SCSI_HHCB SCSI_HPTR);
SCSI_FIRMWARE_DESCRIPTOR SCSIFirmwareStandardAdv = 
{
   SCSI_SADV_FIRMWARE_VERSION,         /* firmware version              */
   SCSI_SADV_SIZE_SCB,                 /* SCB size                      */
   sizeof(SCSI_QOUTFIFO_NEW),          /* qoutfifo element size         */
   0,                                  /* for alignment and future use  */
   SCSI_SADV_SIOSTR3_ENTRY,            /* siostr3_entry                 */
   SCSI_SADV_ATN_TMR_ENTRY,            /* atn_tmr_entry                 */
   SCSI_SADV_TARG_LUN_MASK0,           /* targ_lun_mask                 */
   SCSI_SADV_START_LINK_CMD_ENTRY,     /* start_link_cmd_entry          */
   SCSI_SADV_IDLE_LOOP_ENTRY,          /* idle_loop_entry               */
   SCSI_SADV_IDLE_LOOP0,               /* idle_loop0                    */
   SCSI_SADV_ARRAY_PARTITION0,         /* array partition0              */
   SCSI_SADV_SIO204_ENTRY,             /* sio204_entry                  */
   SCSI_SADV_SIO215,                   /* sio215                        */
   SCSI_SADV_EXPANDER_BREAK,           /* expander_break                */
#if SCSI_TARGET_OPERATION          
   SCSI_SADV_TARGET_DATA_ENTRY,        /* target data entry             */
   SCSI_SADV_TARGET_HELD_BUS_ENTRY,    /* target held scsi bus entry    */
#endif  /* SCSI_TARGET_OPERATION */  
   SCSI_SADV_BTATABLE,                 /* busy target array table       */
   SCSI_SADV_RESCNT_BASE,              /* residual length base          */
   SCSI_SADV_PASS_TO_DRIVER,           /* pass_to_driver                */
   SCSI_SADV_PASS_TO_DRIVER,           /* scsidatl image                */
   SCSI_SADV_ACTIVE_SCB,               /* waiting scb                   */
   SCSI_SADV_WAITING_SCB,              /* active scb                    */
   SCSI_SADV_XFER_OPTION,              /* transfer option               */
   SCSI_SADV_DISCON_OPTION,            /* disconnect option             */
   SCSI_SADV_Q_NEW_POINTER,            /* base address of new SCB queue */
   SCSI_SADV_Q_NEW_HEAD,               /* queue new head                */
   SCSI_SADV_Q_NEW_TAIL,               /* queue new tail                */
   SCSI_SADV_Q_NEXT_SCB,               /* next SCB to be delivered      */
   SCSI_SADV_Q_DONE_BASE,              /* queue done base               */
   SCSI_SADV_Q_DONE_HEAD,              /* queue done head               */
   SCSI_SADV_Q_DONE_ELEMENT,           /* queue done element            */
   SCSI_SADV_Q_DONE_PASS,              /* queue done pass               */
   SCSI_SADV_Q_EXE_HEAD,               /* queue exe head                */
   SCSI_SADV_Q_MASK,                   /* queue masks for in/out        */
   SCSI_SADV_FAST20_LOW,               /* fast 20 low byte              */
   SCSI_SADV_FAST20_HIGH,              /* fast 20 high byte             */
   SCSI_SADV_MWI_CACHE_MASK,           /* Mwi Cache Mask                */
   SCSI_SADV_ENT_PT_BITMAP,            /* bitmap for various brk points */
   SCSI_SADV_AUTO_RATE_ENABLE,         /* workaround for Trident's auto rate */
#if SCSI_TRIDENT_PROTO
   SCSI_SADV_SCSISEQ_COPY,             /* copy of scsiseq register      */
   0,                                  /* reserved1                     */
#else /* SCSI_TRIDENT_PROTO */
   {
      0,0                              /* reserved1                     */
   },
#endif /* SCSI_TRIDENT_PROTO */
#if SCSI_TARGET_OPERATION
   SCSI_SADV_Q_EST_HEAD,               /* Not used                      */
   SCSI_SADV_Q_EST_TAIL,               /* Not used                      */
   SCSI_SADV_Q_EST_NEXT_SCB,           /* Not used                      */
   0,                                  /* reserved for future use       */
#endif /* SCSI_TARGET_OPERATION */
#if SCSI_RESOURCE_MANAGEMENT
   SCSI_SADV_MAX_NONTAG_SCBS,          /* Max Non Tag SCBs              */
   {
      0,0,0                            /* reserved2                     */
   },
#endif
   SCSIhStandardAdvSetupSequencer,     /* setup sequencer               */
   SCSIhStandardAdvResetSoftware,      /* reset software                */
   SCSIhDeliverScb,                    /* deliver scb                   */
   SCSIhStandardAdvPatchXferOpt,       /* patch xfer option             */
   SCSIhStandardAdvQHeadBDR,           /* enqueue at head of exe. queue */
   SCSIhStandardRetrieveScb,           /* retrieve scb                  */
   SCSIhStandardQoutcnt,               /* check qoutcnt value           */
   SCSIhStandardTargetClearBusy,       /* clear busy target             */
   SCSIhStandardAdvRequestSense,       /* request sense setup           */
   SCSIhStandardResetBTA,              /* reste busy target array       */
   SCSIhStandardGetConfig,             /* get driver config info        */
   SCSIhStandardConfigureScbRam,       /* configure scb ram             */
#if SCSI_SCBBFR_BUILTIN
   SCSIhSetupAssignScbBuffer,          /* set assign scb buffer         */
   0                                   /* assign scb buffer (SCSIhAssignScbBuffer) */
#else
   0,
   0
#endif
   ,
   SCSIhChkCondXferAssign,             /* assign xfer option for check cond. */ 
   SCSIhHostXferOptAssign,             /* assign xfer option            */
   SCSIhStandardAdvCommonXferAssign,   /* assign xfer option common     */
   SCSIhStandardAdvUpdateXferOptionHostMem,  /* assign xfer option host mem   */
   SCSIhHostGetOption,                 /* get xfer option               */
   SCSIhStandardAdvGetFast20Reg,       /* get fast20 registers          */
   0,                                  /* clear fast20 registers (SCSIhStandardClearFast20Reg) */
   0,                                  /* log fast20 registers          */
   0,                                  /* update fast 20 bit in scb in host mem */
   0,                                  /* update fast 20 bit in scb in hw */
   0,                                  /* clear all fast 20 registers     */
   SCSIhStandardAdvModifyDataPtr,      /* modify data pointer message   */
   SCSIhStandardAdvActiveAbort,        /* aborting active HIOB          */
   SCSIhStandardAdvAbortHIOB,          /* aborting HIOB in queues       */
   SCSIhStandardAdvSearchSeqDoneQ,     /* search seq done Q. for abort HIOB */
   SCSIhStandardSearchDoneQ,           /* search Done Q. for abort HIOB */
   SCSIhStandardSearchNewQ,            /* search New Q. for abort HIOB */
   SCSIhStandardAdvSearchExeQ,         /* search Exe. Q. for abort HIOB */
   SCSIhStandardAdvUpdateAbortBitHostMem, /* update abort bit in host mem */
   SCSIhStandardAdvObtainNextScbNum,   /* obtain next SCB number in SCB */   
   SCSIhStandardAdvUpdateNextScbNum,   /* update next SCB number in SCB */   
   SCSIhStandardAdvObtainNextScbAddress, /* obtain next SCB address in SCB */ 
   SCSIhStandardAdvUpdateNextScbAddress, /* update next SCB address in SCB */ 
   SCSIhStandardResetCCCtl,            /* reset CCSCBCTL and CCSGCTL    */
   SCSIhStandardInitCCHHAddr,          /* initialize CCHHAddr           */
   SCSIhStandardRemoveActiveAbort,     /* remove active abort           */
   0,                                  /* fix sequencer stack problems  */
   SCSIhStandardAdvSetupBDR,           /* setup bus device reset scb    */
   SCSIhStandardAdvResidueCalc,        /* calculate residual length     */
   SCSIhStandardAdvSetBreakPoint,      /* set breakpoint                */
   SCSIhStandardAdvClearBreakPoint,    /* clear breakpoint              */
   SCSIhStandardAdvUpdateSGLength,     /* update SCB slength0-2         */
   SCSIhStandardAdvUpdateSGAddress,    /* update SCB saddress0-3        */
   SCSIhStandardAdvFreezeHWQueue,      /* freeze h/w queue              */
   SCSIhStandardAdvSetIntrThreshold,   /* set intr threshold value      */
   SCSIhStandardAdvGetIntrThreshold,   /* get intr threshold value      */
   0,                                  /* enable auto rate feature      */
   0                                   /* disable auto rate feature     */
#if SCSI_RESOURCE_MANAGEMENT
   ,
   0,                                  /* reset */
   SCSIrGetScb,                        /* get scb */
   SCSIrReturnScb,                     /* return scb */
   SCSIrHostQueueRemove,               /* remove from host queue */
   SCSIrStandardRemoveScbs             /* remove scbs                   */
#if SCSI_TARGET_OPERATION
   ,
   0,                                  /* get est scb                   */              
   0                                   /* return est scb                */
#endif /* SCSI_TARGET_OPERATION */
#endif /* SCSI_RESOURCE_MANAGEMENT */
#if SCSI_TARGET_OPERATION              
   , 
   0,                                 /* patch transfer rate into SCB   */
   0,                                 /* set ignore wide residue        */ 
   0,                                 /* send hiob special              */
   0,                                 /* get est scb fields             */
   0                                  /* deliver est scb                */
#endif  /* SCSI_TARGET_OPERATION */
};
#endif /* SCSI_STANDARD_ADVANCED_MODE && defined(SCSI_DATA_DEFINE) */

/***************************************************************************
*  SCSI_FIRMWARE_DESCRIPTOR for Swapping Advanced mode
***************************************************************************/
#if SCSI_SWAPPING_ADVANCED_MODE && defined(SCSI_DATA_DEFINE)

int SCSIhSwappingAdvSetupSequencer(SCSI_HHCB SCSI_HPTR);
void SCSIhSwappingAdvResetSoftware(SCSI_HHCB SCSI_HPTR);
void SCSIhDeliverScb (SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwappingAdvPatchXferOpt(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwappingAdvQHeadBDR(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIhStandardRetrieveScb(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8 SCSIhStandardQoutcnt(SCSI_HHCB SCSI_HPTR);
void SCSIhSwappingTargetClearBusy(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); 
void SCSIhSwappingAdvRequestSense (SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwappingResetBTA(SCSI_HHCB SCSI_HPTR); 
void SCSIhNonStandardGetConfig(SCSI_HHCB SCSI_HPTR);
void SCSIhSetupAssignScbBuffer(SCSI_HHCB SCSI_HPTR);
void SCSIhStandardAssignScbBuffer(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIrGetScb(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIrReturnScb(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_HIOB SCSI_IPTR SCSIrHostQueueRemove(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIhHostXferOptAssign(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT16);
void SCSIhChkCondXferAssign(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT16);
void SCSIhSwappingAdvXferOptAssign(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT16);
void SCSIhSwappingAdvCommonXferAssign(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT16);
void SCSIhSwappingAdvUpdateXferOptionHostMem (SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT16);
SCSI_UEXACT16 SCSIhHostGetOption(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhSwappingAdvGetFast20Reg(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIhSwappingAdvModifyDataPtr(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwappingAdvActiveAbort(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwappingAdvAbortHIOB(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR, SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhStandardSearchDoneQ(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIhSwappingSearchNewQ(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR, SCSI_UEXACT8);
void SCSIhSwappingAdvUpdateAbortBitHostMem(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);  
SCSI_UEXACT8 SCSIhSwappingAdvObtainNextScbNum(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwappingAdvUpdateNextScbNum(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);  
void SCSIhStandardResetCCCtl(SCSI_HHCB SCSI_HPTR);
void SCSIhStandardInitCCHHAddr(SCSI_HHCB SCSI_HPTR);
void SCSIhSwappingRemoveActiveAbort(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwappingAdvSetupBDR(SCSI_HHCB SCSI_HPTR, SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT32 SCSIhSwappingAdvResidueCalc(SCSI_HHCB SCSI_HPTR, SCSI_HIOB SCSI_IPTR);
void SCSIhSwappingAdvSetBreakPoint(SCSI_HHCB SCSI_HPTR, SCSI_UEXACT8);
void SCSIhSwappingAdvClearBreakPoint(SCSI_HHCB SCSI_HPTR, SCSI_UEXACT8);
void SCSIhSwappingAdvUpdateSGLength(SCSI_HHCB SCSI_HPTR, SCSI_QUADLET);
void SCSIhSwappingAdvUpdateSGAddress(SCSI_HHCB SCSI_HPTR, SCSI_QUADLET);
void SCSIhSwappingAdvFreezeHWQueue(SCSI_HHCB SCSI_HPTR, SCSI_HIOB SCSI_IPTR);
void SCSIhSwappingAdvSetIntrThreshold(SCSI_HHCB SCSI_HPTR, SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhSwappingAdvGetIntrThreshold(SCSI_HHCB SCSI_HPTR);
#if SCSI_TARGET_OPERATION
void SCSIrSwappingRemoveScbs(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8 SCSIrSwappingGetEstScb(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIrSwappingReturnEstScb(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhTargetSwappingAdvPatchXferRate(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR hiob);
void SCSIhTargetSwappingAdvSetIgnoreWideMsg(SCSI_HHCB SCSI_HPTR, SCSI_HIOB SCSI_IPTR);
void SCSIhTargetSwappingSendHiobSpecial(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhTargetSwappingAdvGetEstScbFields(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,
                                           SCSI_UEXACT8);
void SCSIhTargetSwappingAdvDeliverEstScb(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
#endif /* SCSI_TARGET_OPERATION */
SCSI_FIRMWARE_DESCRIPTOR SCSIFirmwareSwappingAdv = 
{
   SCSI_WADV_FIRMWARE_VERSION,         /* firmware version              */
   SCSI_WADV_SIZE_SCB,                 /* SCB size                      */
   sizeof(SCSI_QOUTFIFO_NEW),          /* qoutfifo element size         */
   0,                                  /* for alignment and future use  */
   SCSI_WADV_SIOSTR3_ENTRY,            /* siostr3_entry                 */
   SCSI_WADV_ATN_TMR_ENTRY,            /* atn_tmr_entry                 */
   SCSI_WADV_TARG_LUN_MASK0,           /* targ_lun_mask                 */
   SCSI_WADV_START_LINK_CMD_ENTRY,     /* start_link_cmd_entry          */
   SCSI_WADV_IDLE_LOOP_ENTRY,          /* idle_loop_entry               */
   SCSI_WADV_IDLE_LOOP0,               /* idle_loop0                    */
   SCSI_WADV_ARRAY_PARTITION0,         /* array partition0              */
   SCSI_WADV_SIO204_ENTRY,             /* sio204_entry                  */
   SCSI_WADV_SIO215,                   /* sio215                        */
   SCSI_WADV_EXPANDER_BREAK,           /* expander_break                */
#if SCSI_TARGET_OPERATION          
   SCSI_WADV_TARGET_DATA_ENTRY,        /* target data entry             */
   SCSI_WADV_TARGET_HELD_BUS_ENTRY,    /* target held scsi bus entry    */
#endif  /* SCSI_TARGET_OPERATION */  
   SCSI_WADV_BTATABLE,                 /* busy target array table       */
   SCSI_WADV_RESCNT_BASE,              /* residual length base          */
   SCSI_WADV_PASS_TO_DRIVER,           /* pass_to_driver                */
   SCSI_WADV_PASS_TO_DRIVER,           /* scsidatl image                */
   SCSI_WADV_ACTIVE_SCB,               /* waiting scb                   */
   SCSI_WADV_WAITING_SCB,              /* active scb                    */
   SCSI_WADV_XFER_OPTION,              /* transfer option               */
   SCSI_WADV_DISCON_OPTION,            /* disconnect option             */
   SCSI_WADV_Q_NEW_POINTER,            /* base address of new SCB queue */
   SCSI_WADV_Q_NEW_HEAD,               /* queue new head                */
   SCSI_WADV_Q_NEW_TAIL,               /* queue new tail                */
   SCSI_WADV_Q_NEXT_SCB,               /* next SCB to be delivered      */
   SCSI_WADV_Q_DONE_BASE,              /* queue done base               */
   SCSI_WADV_Q_DONE_HEAD,              /* queue done head               */
   SCSI_WADV_Q_DONE_ELEMENT,           /* queue done element            */
   SCSI_WADV_Q_DONE_PASS,              /* queue done pass               */
   SCSI_WADV_Q_EXE_HEAD,               /* queue exe head                */
   SCSI_WADV_Q_MASK,                   /* queue masks for in/out        */
   SCSI_WADV_FAST20_LOW,               /* fast 20 low byte              */
   SCSI_WADV_FAST20_HIGH,              /* fast 20 high byte             */
   SCSI_WADV_MWI_CACHE_MASK,           /* Mwi Cache Mask                */
   SCSI_WADV_ENT_PT_BITMAP,            /* bitmap for various brk points */
   SCSI_WADV_AUTO_RATE_ENABLE,         /* workaround for Trident's auto rate */
#if SCSI_TRIDENT_PROTO
   SCSI_WADV_SCSISEQ_COPY,             /* copy of scsiseq register      */
   0,                                  /* reserved1                     */
#else /* SCSI_TRIDENT_PROTO */
   {
      0,0                              /* reserved1                     */
   },
#endif /* SCSI_TRIDENT_PROTO */
#if SCSI_TARGET_OPERATION
   SCSI_WADV_Q_EST_HEAD,               /* host est scb queue offset */
   SCSI_WADV_Q_EST_TAIL,               /* sequencer est scb queue offset */
   SCSI_WADV_Q_EST_NEXT_SCB,           /* first est scb to be delivered */
   0,                                  /* reserved for future use       */
#endif /* SCSI_TARGET_OPERATION */
#if SCSI_RESOURCE_MANAGEMENT
   SCSI_WADV_MAX_NONTAG_SCBS,          /* Max Non Tag SCBs              */
   {
      0,0,0                            /* reserved2                     */
   },
#endif
   SCSIhSwappingAdvSetupSequencer,     /* setup sequencer               */
   SCSIhSwappingAdvResetSoftware,      /* reset software                */
   SCSIhDeliverScb,                    /* deliver scb                   */
   SCSIhSwappingAdvPatchXferOpt,       /* patch xfer option             */
   SCSIhSwappingAdvQHeadBDR,           /* enqueue at head of new queue  */
   SCSIhStandardRetrieveScb,           /* retrieve scb                  */
   SCSIhStandardQoutcnt,               /* check qoutcnt value           */
   SCSIhSwappingTargetClearBusy,       /* clear busy target             */
   SCSIhSwappingAdvRequestSense,       /* setup request sense  ???      */
   SCSIhSwappingResetBTA,              /* reset busy target array       */
   SCSIhNonStandardGetConfig,          /* get driver config info        */
   0,                                  /* configure scb ram             */
   SCSIhSetupAssignScbBuffer,          /* setup assign scb buffer       */
   0,                                  /* assign scb buffer (SCSIhAssignScbBuffer) */
   SCSIhChkCondXferAssign,             /* assign xfer option during check condition */
   SCSIhHostXferOptAssign,             /* assign xfer option            */
   SCSIhSwappingAdvCommonXferAssign,   /* assign xfer option common     */
   SCSIhSwappingAdvUpdateXferOptionHostMem,   /* assign xfer opt host mem */
   SCSIhHostGetOption,                 /* get xfer option               */
   SCSIhSwappingAdvGetFast20Reg,       /* get fast20 registers          */
   0,                                  /* clear fast20 registers (SCSIhSwappingClearFast20Reg) */
   0,                                  /* log fast20 registers          */
   0,                                  /* update fast 20 bit in scb in host mem */
   0,                                  /* update fast 20 bit in scb in hw */
   0,                                  /* clear all fast 20 registers     */
   SCSIhSwappingAdvModifyDataPtr,      /* modify data pointer message   */
   SCSIhSwappingAdvActiveAbort,        /* aborting active HIOB          */
   SCSIhSwappingAdvAbortHIOB,          /* aborting HIOB in queues       */
   0,                                  /* search seq done Q. for abort HIOB */
   SCSIhStandardSearchDoneQ,           /* search Done Q. for abort HIOB */
   SCSIhSwappingSearchNewQ,            /* search New Q. for abort HIOB  */
   0,                                  /* search Exe. Q. for abort HIOB */
   SCSIhSwappingAdvUpdateAbortBitHostMem, /* update abort bit in host mem */
   SCSIhSwappingAdvObtainNextScbNum,   /* obtain next SCB number in SCB */   
   SCSIhSwappingAdvUpdateNextScbNum,   /* update next SCB number in SCB */   
   0,                                  /* obtain next SCB address in SCB */ 
   0,                                  /* update next SCB address in SCB */ 
   SCSIhStandardResetCCCtl,            /* reset CCSCBCTL and CCSGCTL    */
   SCSIhStandardInitCCHHAddr,          /* initialize CCHHAddr           */
   SCSIhSwappingRemoveActiveAbort,     /* remove active abort           */
   0,                                  /* fix sequencer stack problems  */
   SCSIhSwappingAdvSetupBDR,           /* setup bus device reset scb    */
   SCSIhSwappingAdvResidueCalc,        /* calculate residual length     */
   SCSIhSwappingAdvSetBreakPoint,      /* set breakpoint                */
   SCSIhSwappingAdvClearBreakPoint,    /* clear breakpoint              */
   SCSIhSwappingAdvUpdateSGLength,     /* update SCB slength0-2         */
   SCSIhSwappingAdvUpdateSGAddress,    /* update SCB saddress0-3        */
   SCSIhSwappingAdvFreezeHWQueue,      /* freeze h/w queue              */
   SCSIhSwappingAdvSetIntrThreshold,   /* set intr threshold value      */
   SCSIhSwappingAdvGetIntrThreshold,   /* get intr threshold value      */
   0,                                  /* enable auto rate feature      */
   0                                   /* disable auto rate feature     */
#if SCSI_RESOURCE_MANAGEMENT
   ,
   0,                                  /* reset                         */
   SCSIrGetScb,                        /* get scb                       */     
   SCSIrReturnScb,                     /* return scb                    */  
   SCSIrHostQueueRemove,               /* remove from host queue        */
#if SCSI_TARGET_OPERATION
   SCSIrSwappingRemoveScbs,            /* remove scbs                   */  
   SCSIrSwappingGetEstScb,             /* get est scb                   */              
   SCSIrSwappingReturnEstScb           /* return est scb                */
#else
   0                                   /* remove scbs                   */
#endif /* SCSI_TARGET_OPERATION */
#endif /* SCSI_RESOURCE_MANAGEMENT */
#if SCSI_TARGET_OPERATION              
   , 
   SCSIhTargetSwappingAdvPatchXferRate,/* patch transfer rate into SCB  */
   SCSIhTargetSwappingAdvSetIgnoreWideMsg,  /* set ignore wide residue  */ 
   SCSIhTargetSwappingSendHiobSpecial,      /* send hiob special        */
   SCSIhTargetSwappingAdvGetEstScbFields,   /* get est scb fields       */
   SCSIhTargetSwappingAdvDeliverEstScb      /* deliver est scb          */
#endif  /* SCSI_TARGET_OPERATION */
};
#endif /* SCSI_SWAPPING_ADVANCED_MODE && defined(SCSI_DATA_DEFINE) */

/***************************************************************************
*  SCSI_FIRMWARE_DESCRIPTOR for Standard Ultra 160m mode 
***************************************************************************/
#if SCSI_STANDARD_160M_MODE && defined(SCSI_DATA_DEFINE)
int SCSIhStandard160mSetupSequencer(SCSI_HHCB SCSI_HPTR);
void SCSIhStandard160mResetSoftware(SCSI_HHCB SCSI_HPTR);
void SCSIhDeliverScb(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhStandard160mPatchXferOpt(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhStandard160mQHeadBDR(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIhStandardRetrieveScb(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8 SCSIhStandardQoutcnt(SCSI_HHCB SCSI_HPTR);
void SCSIhStandardTargetClearBusy(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhStandard160mRequestSense(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhStandardResetBTA(SCSI_HHCB SCSI_HPTR);
void SCSIhStandardGetConfig(SCSI_HHCB SCSI_HPTR);
int SCSIhStandardConfigureScbRam(SCSI_HHCB SCSI_HPTR);
void SCSIhSetupAssignScbBuffer(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8 SCSIrGetScb(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIrReturnScb(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_HIOB SCSI_IPTR SCSIrHostQueueRemove(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIrStandardRemoveScbs(SCSI_HHCB SCSI_HPTR);
void SCSIhHostXferOptAssign(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT16);
void SCSIhChkCondXferAssign(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT16);
void SCSIhStandard160mCommonXferAssign(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT16);
void SCSIhStandard160mUpdateXferOptionHostMem(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT16);
SCSI_UEXACT16 SCSIhHostGetOption(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhStandardAdvGetFast20Reg(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIhStandard160mModifyDataPtr(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); 
void SCSIhStandard160mActiveAbort(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhStandard160mAbortHIOB(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhStandard160mSearchSeqDoneQ(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhStandardSearchDoneQ(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIhStandardSearchNewQ(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhStandard160mSearchExeQ(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8,SCSI_UEXACT8);
void SCSIhStandard160mUpdateAbortBitHostMem(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); 
SCSI_UEXACT8 SCSIhStandard160mObtainNextScbNum(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhStandard160mUpdateNextScbNum(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);  
SCSI_BUS_ADDRESS SCSIhStandard160mObtainNextScbAddress(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhStandard160mUpdateNextScbAddress(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_BUS_ADDRESS);
void SCSIhStandardResetCCCtl(SCSI_HHCB SCSI_HPTR);
void SCSIhStandardInitCCHHAddr(SCSI_HHCB SCSI_HPTR);
void SCSIhStandardRemoveActiveAbort(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhStandard160mSetupBDR(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT32 SCSIhStandard160mResidueCalc(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhStandard160mSetBreakPoint(SCSI_HHCB SCSI_HPTR, SCSI_UEXACT8);
void SCSIhStandard160mClearBreakPoint(SCSI_HHCB SCSI_HPTR, SCSI_UEXACT8);
void SCSIhStandard160mUpdateSGLength(SCSI_HHCB SCSI_HPTR, SCSI_QUADLET);
void SCSIhStandard160mUpdateSGAddress(SCSI_HHCB SCSI_HPTR, SCSI_QUADLET);
void SCSIhStandard160mFreezeHWQueue(SCSI_HHCB SCSI_HPTR, SCSI_HIOB SCSI_IPTR);
void SCSIhStandard160mSetIntrThreshold(SCSI_HHCB SCSI_HPTR, SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhStandard160mGetIntrThreshold(SCSI_HHCB SCSI_HPTR);
void SCSIhEnableAutoRateOption(SCSI_HHCB SCSI_HPTR);
void SCSIhDisableAutoRateOption(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT16);
SCSI_FIRMWARE_DESCRIPTOR SCSIFirmwareStandard160m = 
{
   SCSI_S160M_FIRMWARE_VERSION,        /* firmware version              */
   SCSI_S160M_SIZE_SCB,                /* SCB size                      */
   sizeof(SCSI_QOUTFIFO_NEW),          /* qoutfifo element size         */
   0,                                  /* for alignment and future use  */
   SCSI_S160M_SIOSTR3_ENTRY,           /* siostr3_entry                 */
   SCSI_S160M_ATN_TMR_ENTRY,           /* atn_tmr_entry                 */
   SCSI_S160M_TARG_LUN_MASK0,          /* targ_lun_mask                 */
   SCSI_S160M_START_LINK_CMD_ENTRY,    /* start_link_cmd_entry          */
   SCSI_S160M_IDLE_LOOP_ENTRY,         /* idle_loop_entry               */
   SCSI_S160M_IDLE_LOOP0,              /* idle_loop0                    */
   SCSI_S160M_ARRAY_PARTITION0,        /* array partition0              */
   SCSI_S160M_SIO204_ENTRY,            /* sio204_entry                  */
   SCSI_S160M_SIO215,                  /* sio215                        */
   SCSI_S160M_EXPANDER_BREAK,          /* expander_break                */
#if SCSI_TARGET_OPERATION          
   SCSI_S160M_TARGET_DATA_ENTRY,       /* target data entry             */
   SCSI_S160M_TARGET_HELD_BUS_ENTRY,   /* target held scsi bus entry    */
#endif  /* SCSI_TARGET_OPERATION */
   SCSI_S160M_BTATABLE,                /* busy target array table       */
   SCSI_S160M_RESCNT_BASE,             /* residual length base          */
   SCSI_S160M_PASS_TO_DRIVER,          /* pass_to_driver                */
   SCSI_S160M_SCSIDATL_IMAGE,          /* scsidatl image                */
   SCSI_S160M_ACTIVE_SCB,              /* waiting scb                   */
   SCSI_S160M_WAITING_SCB,             /* active scb                    */
   SCSI_S160M_XFER_OPTION,             /* transfer option               */
   SCSI_S160M_DISCON_OPTION,           /* disconnect option             */
   SCSI_S160M_Q_NEW_POINTER,           /* base address of new SCB queue */
   SCSI_S160M_Q_NEW_HEAD,              /* queue new head                */
   SCSI_S160M_Q_NEW_TAIL,              /* queue new tail                */
   SCSI_S160M_Q_NEXT_SCB,              /* next SCB to be delivered      */
   SCSI_S160M_Q_DONE_BASE,             /* queue done base               */
   SCSI_S160M_Q_DONE_HEAD,             /* queue done head               */
   SCSI_S160M_Q_DONE_ELEMENT,          /* queue done element            */
   SCSI_S160M_Q_DONE_PASS,             /* queue done pass               */
   SCSI_S160M_Q_EXE_HEAD,              /* queue exe head                */
   SCSI_S160M_Q_MASK,                  /* queue masks for in/out        */
   SCSI_S160M_FAST20_LOW,              /* fast 20 low byte              */
   SCSI_S160M_FAST20_HIGH,             /* fast 20 high byte             */
   SCSI_S160M_MWI_CACHE_MASK,          /* Mwi Cache Mask                */
   SCSI_S160M_ENT_PT_BITMAP,           /* bitmap for various brk points */
   SCSI_S160M_AUTO_RATE_ENABLE,        /* workaround for Trident's auto rate */
#if SCSI_TRIDENT_PROTO
   SCSI_S160M_SCSISEQ_COPY,            /* copy of scsiseq register      */
   0,                                  /* reserved1                     */
#else /* SCSI_TRIDENT_PROTO */
   {
      0,0                              /* reserved1                     */
   },
#endif /* SCSI_TRIDENT_PROTO */
#if SCSI_TARGET_OPERATION
   SCSI_S160M_Q_EST_HEAD,              /* Not used                      */
   SCSI_S160M_Q_EST_TAIL,              /* Not used                      */
   SCSI_S160M_Q_EST_NEXT_SCB,          /* Not used                      */
   0,                                  /* reserved for future use       */
#endif /* SCSI_TARGET_OPERATION */
#if SCSI_RESOURCE_MANAGEMENT
   SCSI_S160M_MAX_NONTAG_SCBS,         /* Max Non Tag SCBs              */
   {
      0,0,0                            /* reserved2                     */
   },
#endif
   SCSIhStandard160mSetupSequencer,    /* setup sequencer               */
   SCSIhStandard160mResetSoftware,     /* reset software                */
   SCSIhDeliverScb,                    /* deliver scb                   */
   SCSIhStandard160mPatchXferOpt,      /* patch xfer option             */
   SCSIhStandard160mQHeadBDR,          /* enqueue at head of exe. queue */
   SCSIhStandardRetrieveScb,           /* retrieve scb                  */
   SCSIhStandardQoutcnt,               /* check qoutcnt value           */
   SCSIhStandardTargetClearBusy,       /* clear busy target             */
   SCSIhStandard160mRequestSense,      /* request sense setup           */
   SCSIhStandardResetBTA,              /* reste busy target array       */
   SCSIhStandardGetConfig,             /* get driver config info        */
   SCSIhStandardConfigureScbRam,       /* configure scb ram             */
#if SCSI_SCBBFR_BUILTIN
   SCSIhSetupAssignScbBuffer,          /* set assign scb buffer         */
   0                                   /* assign scb buffer (SCSIhAssignScbBuffer) */
#else
   0,
   0
#endif
   ,
   SCSIhChkCondXferAssign,             /* assign xfer option for check cond. */ 
   SCSIhHostXferOptAssign,             /* assign xfer option            */
   SCSIhStandard160mCommonXferAssign,  /* assign xfer option common     */
   SCSIhStandard160mUpdateXferOptionHostMem,  /* assign xfer option host mem   */
   SCSIhHostGetOption,                 /* get xfer option               */
   SCSIhStandardAdvGetFast20Reg,       /* get fast20 registers          */
   0,                                  /* clear fast20 registers (SCSIhStandardClearFast20Reg) */
   0,                                  /* log fast20 registers          */
   0,                                  /* update fast 20 bit in scb in host mem */
   0,                                  /* update fast 20 bit in scb in hw */
   0,                                  /* clear all fast 20 registers     */
   SCSIhStandard160mModifyDataPtr,     /* modify data pointer message   */
   SCSIhStandard160mActiveAbort,       /* aborting active HIOB          */
   SCSIhStandard160mAbortHIOB,         /* aborting HIOB in queues       */
   SCSIhStandard160mSearchSeqDoneQ,    /* search seq done Q. for abort HIOB */
   SCSIhStandardSearchDoneQ,           /* search Done Q. for abort HIOB */
   SCSIhStandardSearchNewQ,            /* search New Q. for abort HIOB  */
   SCSIhStandard160mSearchExeQ,        /* search Exe. Q. for abort HIOB */
   SCSIhStandard160mUpdateAbortBitHostMem, /* update abort bit in host mem */
   SCSIhStandard160mObtainNextScbNum,  /* obtain next SCB number in SCB */   
   SCSIhStandard160mUpdateNextScbNum,  /* update next SCB number in SCB */   
   SCSIhStandard160mObtainNextScbAddress, /* obtain next SCB address in SCB */ 
   SCSIhStandard160mUpdateNextScbAddress, /* update next SCB address in SCB */ 
   SCSIhStandardResetCCCtl,            /* reset CCSCBCTL and CCSGCTL    */
   SCSIhStandardInitCCHHAddr,          /* initialize CCHHAddr           */
   SCSIhStandardRemoveActiveAbort,     /* remove active abort           */
   0,                                  /* fix sequencer stack problems  */
   SCSIhStandard160mSetupBDR,          /* setup bus device reset scb    */
   SCSIhStandard160mResidueCalc,       /* calculate residual length     */
   SCSIhStandard160mSetBreakPoint,     /* set breakpoint                */
   SCSIhStandard160mClearBreakPoint,   /* clear breakpoint              */
   SCSIhStandard160mUpdateSGLength,    /* update SCB slength0-2         */
   SCSIhStandard160mUpdateSGAddress,   /* update SCB saddress0-3        */
   SCSIhStandard160mFreezeHWQueue,     /* freeze h/w queue              */
   SCSIhStandard160mSetIntrThreshold,  /* set intr threshold value      */
   SCSIhStandard160mGetIntrThreshold,  /* get intr threshold value      */
   SCSIhEnableAutoRateOption,          /* enable auto rate feature      */
   SCSIhDisableAutoRateOption          /* disable auto rate feature     */
#if SCSI_RESOURCE_MANAGEMENT
   ,
   0,                                  /* reset */
   SCSIrGetScb,                        /* get scb */
   SCSIrReturnScb,                     /* return scb */
   SCSIrHostQueueRemove,               /* remove from host queue */
   SCSIrStandardRemoveScbs             /* remove scbs                   */
#if SCSI_TARGET_OPERATION
   ,
   0,                                  /* get est scb                   */              
   0                                   /* return est scb                */
#endif /* SCSI_TARGET_OPERATION */
#endif /* SCSI_RESOURCE_MANAGEMENT */
#if SCSI_TARGET_OPERATION              
   , 
   0,                                  /* patch transfer rate into SCB  */
   0,                                  /* set ignore wide residue       */ 
   0,                                  /* send hiob special             */
   0,                                  /* get est scb fields            */
   0                                   /* deliver est scb               */
#endif  /* SCSI_TARGET_OPERATION */
};
#endif /* SCSI_STANDARD_160M_MODE && defined(SCSI_DATA_DEFINE) */

/***************************************************************************
*  SCSI_FIRMWARE_DESCRIPTOR for Swapping Ultra 160m mode
***************************************************************************/
#if SCSI_SWAPPING_160M_MODE && defined(SCSI_DATA_DEFINE)
int SCSIhSwapping160mSetupSequencer(SCSI_HHCB SCSI_HPTR);
void SCSIhSwapping160mResetSoftware(SCSI_HHCB SCSI_HPTR);
void SCSIhDeliverScb(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwapping160mPatchXferOpt(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwapping160mQHeadBDR(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIhStandardRetrieveScb(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8 SCSIhStandardQoutcnt(SCSI_HHCB SCSI_HPTR);
void SCSIhSwappingTargetClearBusy(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR); 
void SCSIhSwapping160mRequestSense(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwappingResetBTA(SCSI_HHCB SCSI_HPTR); 
void SCSIhNonStandardGetConfig(SCSI_HHCB SCSI_HPTR);
void SCSIhSetupAssignScbBuffer(SCSI_HHCB SCSI_HPTR);
SCSI_UEXACT8 SCSIrGetScb(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIrReturnScb(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_HIOB SCSI_IPTR SCSIrHostQueueRemove(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIhHostXferOptAssign(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT16);
void SCSIhChkCondXferAssign(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT16);
void SCSIhSwapping160mXferOptAssign(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT16);
void SCSIhSwapping160mCommonXferAssign(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8,SCSI_UEXACT16);
void SCSIhSwapping160mUpdateXferOptionHostMem(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT16);
SCSI_UEXACT16 SCSIhHostGetOption(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhSwappingAdvGetFast20Reg(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT8);
void SCSIhSwapping160mModifyDataPtr(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwapping160mActiveAbort(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwapping160mAbortHIOB(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR, SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhStandardSearchDoneQ(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIhSwappingSearchNewQ(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR, SCSI_UEXACT8);
void SCSIhSwapping160mUpdateAbortBitHostMem(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);  
SCSI_UEXACT8 SCSIhSwapping160mObtainNextScbNum(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwapping160mUpdateNextScbNum(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);  
void SCSIhStandardResetCCCtl(SCSI_HHCB SCSI_HPTR);
void SCSIhStandardInitCCHHAddr(SCSI_HHCB SCSI_HPTR);
void SCSIhSwappingRemoveActiveAbort(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhSwapping160mSetupBDR(SCSI_HHCB SCSI_HPTR, SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT32 SCSIhSwapping160mResidueCalc(SCSI_HHCB SCSI_HPTR, SCSI_HIOB SCSI_IPTR);
void SCSIhSwapping160mSetBreakPoint(SCSI_HHCB SCSI_HPTR, SCSI_UEXACT8);
void SCSIhSwapping160mClearBreakPoint(SCSI_HHCB SCSI_HPTR, SCSI_UEXACT8);
void SCSIhSwapping160mUpdateSGLength(SCSI_HHCB SCSI_HPTR, SCSI_QUADLET);
void SCSIhSwapping160mUpdateSGAddress(SCSI_HHCB SCSI_HPTR, SCSI_QUADLET);
void SCSIhSwapping160mFreezeHWQueue(SCSI_HHCB SCSI_HPTR, SCSI_HIOB SCSI_IPTR);
void SCSIhSwapping160mSetIntrThreshold(SCSI_HHCB SCSI_HPTR, SCSI_UEXACT8);
SCSI_UEXACT8 SCSIhSwapping160mGetIntrThreshold(SCSI_HHCB SCSI_HPTR);
void SCSIhEnableAutoRateOption(SCSI_HHCB SCSI_HPTR);
void SCSIhDisableAutoRateOption(SCSI_HHCB SCSI_HPTR,SCSI_UEXACT16);
#if SCSI_TARGET_OPERATION
void SCSIrSwappingRemoveScbs(SCSI_HHCB SCSI_HPTR);
void SCSIrSwappingReturnEstScb(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
SCSI_UEXACT8 SCSIrSwappingGetEstScb(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhTargetSwapping160mDeliverEstScb(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhTargetSwapping160mSetIgnoreWideMsg(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
void SCSIhTargetSwapping160mGetEstScbFields(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR,SCSI_UEXACT8);
void SCSIhTargetSwappingSendHiobSpecial(SCSI_HHCB SCSI_HPTR,SCSI_HIOB SCSI_IPTR);
#endif /* SCSI_TARGET_OPERATION */
SCSI_FIRMWARE_DESCRIPTOR SCSIFirmwareSwapping160m = 
{
   SCSI_W160M_FIRMWARE_VERSION,        /* firmware version              */
   SCSI_W160M_SIZE_SCB,                /* SCB size                      */
   sizeof(SCSI_QOUTFIFO_NEW),          /* qoutfifo element size         */
   0,                                  /* for alignment and future use  */
   SCSI_W160M_SIOSTR3_ENTRY,           /* siostr3_entry                 */
   SCSI_W160M_ATN_TMR_ENTRY,           /* atn_tmr_entry                 */
   SCSI_W160M_TARG_LUN_MASK0,          /* targ_lun_mask                 */
   SCSI_W160M_START_LINK_CMD_ENTRY,    /* start_link_cmd_entry          */
   SCSI_W160M_IDLE_LOOP_ENTRY,         /* idle_loop_entry               */
   SCSI_W160M_IDLE_LOOP0,              /* idle_loop0                    */
   SCSI_W160M_ARRAY_PARTITION0,        /* array partition0              */
   SCSI_W160M_SIO204_ENTRY,            /* sio204_entry                  */
   SCSI_W160M_SIO215,                  /* sio215                        */
   SCSI_W160M_EXPANDER_BREAK,          /* expander_break                */
#if SCSI_TARGET_OPERATION          
   SCSI_W160M_TARGET_DATA_ENTRY,       /* target data entry             */
   SCSI_W160M_TARGET_HELD_BUS_ENTRY,   /* target held scsi bus entry    */
#endif  /* SCSI_TARGET_OPERATION */
   SCSI_W160M_BTATABLE,                /* busy target array table       */
   SCSI_W160M_RESCNT_BASE,             /* residual length base          */
   SCSI_W160M_PASS_TO_DRIVER,          /* pass_to_driver                */
   SCSI_W160M_SCSIDATL_IMAGE,          /* scsidatl image                */
   SCSI_W160M_ACTIVE_SCB,              /* waiting scb                   */
   SCSI_W160M_WAITING_SCB,             /* active scb                    */
   SCSI_W160M_XFER_OPTION,             /* transfer option               */
   SCSI_W160M_DISCON_OPTION,           /* disconnect option             */
   SCSI_W160M_Q_NEW_POINTER,           /* base address of new SCB queue */
   SCSI_W160M_Q_NEW_HEAD,              /* queue new head                */
   SCSI_W160M_Q_NEW_TAIL,              /* queue new tail                */
   SCSI_W160M_Q_NEXT_SCB,              /* next SCB to be delivered      */
   SCSI_W160M_Q_DONE_BASE,             /* queue done base               */
   SCSI_W160M_Q_DONE_HEAD,             /* queue done head               */
   SCSI_W160M_Q_DONE_ELEMENT,          /* queue done element            */
   SCSI_W160M_Q_DONE_PASS,             /* queue done pass               */
   SCSI_W160M_Q_EXE_HEAD,              /* queue exe head                */
   SCSI_W160M_Q_MASK,                  /* queue masks for in/out        */
   SCSI_W160M_FAST20_LOW,              /* fast 20 low byte              */
   SCSI_W160M_FAST20_HIGH,             /* fast 20 high byte             */
   SCSI_W160M_MWI_CACHE_MASK,          /* Mwi Cache Mask                */
   SCSI_W160M_ENT_PT_BITMAP,           /* bitmap for various brk points */
   SCSI_W160M_AUTO_RATE_ENABLE,        /* workaround for Trident's auto rate */
#if SCSI_TRIDENT_PROTO
   SCSI_W160M_SCSISEQ_COPY,            /* copy of scsiseq register      */
   0,                                  /* reserved1                     */
#else /* SCSI_TRIDENT_PROTO */
   {
      0,0                              /* reserved1                     */
   },
#endif /* SCSI_TRIDENT_PROTO */
#if SCSI_TARGET_OPERATION
   SCSI_W160M_Q_EST_HEAD,              /* host est scb queue offset     */
   SCSI_W160M_Q_EST_TAIL,              /* sequencer est scb queue offset*/
   SCSI_W160M_Q_EST_NEXT_SCB,          /* first est scb to be delivered */
   0,                                  /* reserved for future use       */
#endif /* SCSI_TARGET_OPERATION */
#if SCSI_RESOURCE_MANAGEMENT
   SCSI_W160M_MAX_NONTAG_SCBS,         /* Max Non Tag SCBs              */
   {
      0,0,0                            /* reserved2                     */
   },
#endif
   SCSIhSwapping160mSetupSequencer,    /* setup sequencer               */
   SCSIhSwapping160mResetSoftware,     /* reset software                */
   SCSIhDeliverScb,                    /* deliver scb                   */
   SCSIhSwapping160mPatchXferOpt,      /* patch xfer option             */
   SCSIhSwapping160mQHeadBDR,          /* enqueue at head of new queue  */
   SCSIhStandardRetrieveScb,           /* retrieve scb                  */
   SCSIhStandardQoutcnt,               /* check qoutcnt value           */
   SCSIhSwappingTargetClearBusy,       /* clear busy target             */
   SCSIhSwapping160mRequestSense,      /* setup request sense  ???      */
   SCSIhSwappingResetBTA,              /* reset busy target array       */
   SCSIhNonStandardGetConfig,          /* get driver config info        */
   0,                                  /* configure scb ram             */
   SCSIhSetupAssignScbBuffer,          /* setup assign scb buffer       */
#if SCSI_RESOURCE_MANAGEMENT 
   0,                                  /* assign scb buffer (SCSIhAssignScbBuffer) */
#else
   SCSIhSwappingAssignScbBuffer,
#endif /* SCSI_RESOURCE_MANAGEMENT */
   SCSIhChkCondXferAssign,             /* assign xfer option during check condition */
   SCSIhHostXferOptAssign,             /* assign xfer option            */
   SCSIhSwapping160mCommonXferAssign,  /* assign xfer option common     */
   SCSIhSwapping160mUpdateXferOptionHostMem,   /* assign xfer opt host mem */
   SCSIhHostGetOption,                 /* get xfer option               */
   SCSIhSwappingAdvGetFast20Reg,       /* get fast20 registers          */
   0,                                  /* clear fast20 registers (SCSIhSwappingClearFast20Reg) */
   0,                                  /* log fast20 registers          */
   0,                                  /* update fast 20 bit in scb in host mem */
   0,                                  /* update fast 20 bit in scb in hw */
   0,                                  /* clear all fast 20 registers     */
   SCSIhSwapping160mModifyDataPtr,     /* modify data pointer message   */
   SCSIhSwapping160mActiveAbort,       /* aborting active HIOB          */
   SCSIhSwapping160mAbortHIOB,         /* aborting HIOB in queues       */
   0,                                  /* search seq done Q. for abort HIOB */
   SCSIhStandardSearchDoneQ,           /* search Done Q. for abort HIOB */
   SCSIhSwappingSearchNewQ,            /* search New Q. for abort HIOB  */
   0,                                  /* search Exe. Q. for abort HIOB */
   SCSIhSwapping160mUpdateAbortBitHostMem, /* update abort bit in host mem */
   SCSIhSwapping160mObtainNextScbNum,  /* obtain next SCB number in SCB */   
   SCSIhSwapping160mUpdateNextScbNum,  /* update next SCB number in SCB */   
   0,                                  /* obtain next SCB address in SCB */ 
   0,                                  /* update next SCB address in SCB */ 
   SCSIhStandardResetCCCtl,            /* reset CCSCBCTL and CCSGCTL    */
   SCSIhStandardInitCCHHAddr,          /* initialize CCHHAddr           */
   SCSIhSwappingRemoveActiveAbort,     /* remove active abort           */
   0,                                  /* fix sequencer stack problems  */
   SCSIhSwapping160mSetupBDR,          /* setup bus device reset scb    */
   SCSIhSwapping160mResidueCalc,       /* calculate residual length     */
   SCSIhSwapping160mSetBreakPoint,     /* set breakpoint                */
   SCSIhSwapping160mClearBreakPoint,   /* clear breakpoint              */
   SCSIhSwapping160mUpdateSGLength,    /* update SCB slength0-2         */
   SCSIhSwapping160mUpdateSGAddress,   /* update SCB saddress0-3        */
   SCSIhSwapping160mFreezeHWQueue,     /* freeze h/w queue              */
   SCSIhSwapping160mSetIntrThreshold,  /* set intr threshold value      */
   SCSIhSwapping160mGetIntrThreshold,  /* get intr threshold value      */
   SCSIhEnableAutoRateOption,          /* enable auto rate feature      */
   SCSIhDisableAutoRateOption          /* disable auto rate feature     */
#if SCSI_RESOURCE_MANAGEMENT
   ,
   0,                                  /* reset                         */
   SCSIrGetScb,                        /* get scb                       */     
   SCSIrReturnScb,                     /* return scb                    */  
   SCSIrHostQueueRemove,               /* remove from host queue        */
#if SCSI_TARGET_OPERATION
   SCSIrSwappingRemoveScbs,            /* remove scbs                   */
   SCSIrSwappingGetEstScb,             /* get est scb                   */
   SCSIrSwappingReturnEstScb           /* return est scb                */
#else
   0                                   /* remove scbs                   */
#endif /* SCSI_TARGET_OPERATION */
#endif /* SCSI_RESOURCE_MANAGEMENT */
#if SCSI_TARGET_OPERATION
   ,
   0,                                 /* patch transfer rate into SCB   */
   SCSIhTargetSwapping160mSetIgnoreWideMsg, /* set ignore wide residue  */
   SCSIhTargetSwappingSendHiobSpecial,      /* send hiob special        */
   SCSIhTargetSwapping160mGetEstScbFields,  /* get est scb fields       */
   SCSIhTargetSwapping160mDeliverEstScb     /* deliver est scb          */
#endif /* SCSI_TARGET_OPERATION */
};
#endif   /* SCSI_SWAPPING_160M_MODE && defined(SCSI_DATA_DEFINE) */

/***************************************************************************
*  SCSI_FIRMWARE_DESCRIPTOR table for indexing
***************************************************************************/
#define  SCSI_MAX_MODES    9
#if  defined(SCSI_DATA_DEFINE)
SCSI_FIRMWARE_DESCRIPTOR SCSI_LPTR SCSIFirmware[SCSI_MAX_MODES] =
{
#if SCSI_STANDARD64_MODE
   &SCSIFirmwareStandard64,
#else
   (SCSI_FIRMWARE_DESCRIPTOR SCSI_LPTR) 0,
#endif
#if SCSI_STANDARD32_MODE
   &SCSIFirmwareStandard32,
#else
   (SCSI_FIRMWARE_DESCRIPTOR SCSI_LPTR) 0,
#endif
#if SCSI_SWAPPING64_MODE
   &SCSIFirmwareSwapping64,
#else
   (SCSI_FIRMWARE_DESCRIPTOR SCSI_LPTR) 0,
#endif
#if SCSI_SWAPPING32_MODE
   &SCSIFirmwareSwapping32,
#else
   (SCSI_FIRMWARE_DESCRIPTOR SCSI_LPTR) 0,
#endif
#if SCSI_STANDARD_ADVANCED_MODE
   &SCSIFirmwareStandardAdv,
#else
   (SCSI_FIRMWARE_DESCRIPTOR SCSI_LPTR) 0,
#endif
#if SCSI_SWAPPING_ADVANCED_MODE
   &SCSIFirmwareSwappingAdv,
#else
   (SCSI_FIRMWARE_DESCRIPTOR SCSI_LPTR) 0,
#endif
#if SCSI_STANDARD_160M_MODE
   &SCSIFirmwareStandard160m,
#else
   (SCSI_FIRMWARE_DESCRIPTOR SCSI_LPTR) 0,
#endif
#if SCSI_SWAPPING_160M_MODE
   &SCSIFirmwareSwapping160m,
#else
   (SCSI_FIRMWARE_DESCRIPTOR SCSI_LPTR) 0,
#endif
   (SCSI_FIRMWARE_DESCRIPTOR SCSI_LPTR) 0
};
#else
extern SCSI_FIRMWARE_DESCRIPTOR SCSI_LPTR SCSIFirmware[SCSI_MAX_MODES];
#endif
#endif

/*$Header:   Y:/source/chimscsi/src/himscsi/hwmscsi/SCSIREF.HV_   1.41.3.2   24 Mar 1998 17:16:48   NGUY2129  $*/
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
*  Module Name:   SCSIREF.H
*
*  Description:   Definitions as interface to HIM internal layers
*    
*  Owners:        ECX IC Firmware Team
*    
*  Notes:         1. This file is included from HIMDEF.H
*                 2. This file should be modified to fit a particular
*                    Common HIM implementation.
*
***************************************************************************/

#define  SCSI_DOMAIN_VALIDATION  SCSI_DOMAIN_VALIDATION_BASIC+SCSI_DOMAIN_VALIDATION_ENHANCED

/***************************************************************************
* Miscellaneous
***************************************************************************/
#define  SCSI_MAXSCBS      255         /* Maximum number of possible SCBs  */
#define  SCSI_MINSCBS      4           /* minimum number of possible SCBS  */
#define  SCSI_MAXDEV     16            /* maximum number of target devs    */
                                       /* maximum value for AP_ResetDelay  */
                                       /* as uses a 500usec counter        */                       
#define  SCSI_MAX_RESET_DELAY   ((SCSI_UEXACT32) 0x7FFFFFFF)
typedef struct SCSI_DEVICE_ SCSI_DEVICE;

#if SCSI_TARGET_OPERATION 
#if SCSI_MAILBOX_ENABLE
#define  SCSI_MAX_MAILBOX_EST_SCBS  15 /* Maximum number of SCBs allocated */
                                       /* for Establish Connections        */
#endif /* SCSI_MAILBOX_ENABLE */
#if SCSI_MULTIPLEID_SUPPORT
#define  SCSI_MAXADAPTERIDS 15         /* Maximum number of Adapter IDs    */
#define  SCSI_MAXNODES      (15 * SCSI_MAXADAPTERIDS) /* Maximum number of Nodes  */ 
#else
#define  SCSI_MAXNODES      15         /* Maximum number of Nodes          */ 
#endif /* SCSI_MULTIPLEID_SUPPORT */

/* Maximum number of nexus task set handles */
#define  SCSI_MAXNEXUSHANDLES ((0xFFFFFFFF)/(sizeof(SCSI_NEXUS))) 
typedef struct SCSI_NEXUS_ SCSI_NEXUS; 
#endif /* SCSI_TARGET_OPERATION */

#if SCSI_DMA_SUPPORT
/* DMA I/O defines */
#define  SCSI_DMA_ID       16   /* SCSI ID used for DMA device */
#endif /* SCSI_DMA_SUPPORT */

/***************************************************************************
* Definitions for memory management 
***************************************************************************/
#if !SCSI_DOWNSHIFT_MODE
#define  SCSI_HM_QOUTPTRARRAY       0  /* for not down shift mode       */
#define  SCSI_HM_SCBBFRARRAY        1  /* for swapping                  */
#define  SCSI_HM_ACTIVEPTRARRAY     2  /* for not down shift mode       */
#define  SCSI_HM_DONEQUEUE          3  /* for not down shift mode       */
#define  SCSI_HM_SHARECONTROL       4  /* for group devices only        */
#define  SCSI_HM_FREEQUEUE          5  /*                               */
#if SCSI_TARGET_OPERATION
#define  SCSI_HM_NEXUSQUEUE         6  /* for target mode only          */
#define  SCSI_HM_NODEQUEUE          7  /* for target mode only          */
#define  SCSI_MAX_MEMORY            8
#else 
#define  SCSI_MAX_MEMORY            6
#endif /* SCSI_TARGET_OPERATION */
#endif

/***************************************************************************
* SCSI_STATE structures definitions
***************************************************************************/
typedef struct SCSI_STATE_
{
   SCSI_UEXACT8 scb2[SCSI_MAX_SCBSIZE];
   SCSI_UEXACT8 scb0[SCSI_MAX_SCBSIZE];
   SCSI_UEXACT8 scratch0[SCSI_SCRATCH0_SIZE];
   SCSI_UEXACT8 scratch1[SCSI_SCRATCH1_SIZE];
   SCSI_UEXACT8 hAddr[4];                          /* for save/restore state */
   SCSI_UEXACT8 Bta[SCSI_MAXSCBS+1];               /* busy target array      */
   union
   {
      SCSI_UEXACT16 u16;
      struct 
      {
         SCSI_UEXACT16 scbRamSel:1;            /* status of SCBRAMSEL bit in DEVCONFIG   */
         SCSI_UEXACT16 scbSize32:1;            /* status of SCBSIZE32 bit in DEVCONFIG   */
         SCSI_UEXACT16 chipPause:1;            /* chip was in pause state                */
         SCSI_UEXACT16 interruptEnable:1;      /* status if interrupt enable bit         */
         SCSI_UEXACT16 mwiBit:1;               /* mwicen bit in confg space for workarnd */
         SCSI_UEXACT16 optMode:1;              /* =1, the state is for optima HIM */
#if SCSI_TARGET_OPERATION
         SCSI_UEXACT16 selectionEnable:1;      /* status if selection enable bit         */
#endif /* SCSI_TARGET_OPERATION */
      } bits;
   } flags;
   SCSI_UEXACT8 hostScsiId;

#if (SCSI_AICBAYONET || SCSI_AICTRIDENT)
   SCSI_UEXACT8 qNewHead;
   SCSI_UEXACT8 qNewTail;
#if SCSI_AICTRIDENT
   SCSI_UEXACT8 optionMode;
#if POWER_MANAGEMENT_SAVE_RESTORE
#if !(SCSI_STANDARD_MODE || SCSI_SWAPPING_ADVANCED_MODE || SCSI_SWAPPING_160M_MODE)
   SCSI_UEXACT8 alignment[2];
#endif
#else
   SCSI_UEXACT8 alignment[1];
#endif
#else
#if POWER_MANAGEMENT_SAVE_RESTORE
#if (SCSI_STANDARD_MODE || SCSI_SWAPPING_ADVANCED_MODE || SCSI_SWAPPING_160M_MODE)

   SCSI_UEXACT8 alignment[1];
#else
   SCSI_UEXACT8 alignment[3];
#endif
#else
   SCSI_UEXACT8 alignment[2];
#endif  
#endif /* SCSI_AICTRIDENT */
#else

#if POWER_MANAGEMENT_SAVE_RESTORE
#if (SCSI_STANDARD_MODE || SCSI_SWAPPING_ADVANCED_MODE || SCSI_SWAPPING_160M_MODE)
   SCSI_UEXACT8 alignment[3];
#else
   SCSI_UEXACT8 alignment[1];
#endif
#endif
#endif /* (SCSI_AICBAYONET || SCSI_AICTRIDENT) */

#if POWER_MANAGEMENT_SAVE_RESTORE

   SCSI_UEXACT8 scsiseq;
   SCSI_UEXACT8 sxfrctl0;
   SCSI_UEXACT8 sxfrctl1;
   SCSI_UEXACT8 simode0;
   SCSI_UEXACT8 simode1;
   SCSI_UEXACT8 sblkctl;
   SCSI_UEXACT8 hcntrl;
   SCSI_UEXACT8 dfcntrl;
   SCSI_UEXACT8 scamctrl;
   SCSI_UEXACT8 scsisig;
   SCSI_UEXACT8 pcistatus;

#if (SCSI_STANDARD_MODE || SCSI_SWAPPING_ADVANCED_MODE || SCSI_SWAPPING_160M_MODE)

   SCSI_UEXACT8 cchhaddr0;
   SCSI_UEXACT8 cchhaddr1;
   SCSI_UEXACT8 cchhaddr2;
   SCSI_UEXACT8 cchhaddr3;
   SCSI_UEXACT8 ccscbctl; 
   SCSI_UEXACT8 ccsgctl;
#endif
   SCSI_UEXACT32 devconfig;
   SCSI_UEXACT32 status_cmd_register;
   SCSI_UEXACT32 hdr_type;
  
#endif
   /* WARNING: seqRam must always be the last element in this structure for    */        
   /* AP_SaveRestoreSequencer to work correctly.                               */
   SCSI_UEXACT8 seqRam[SCSI_TRIDENT_SEQCODE_SIZE]; /* use largest sequence ram size */

} SCSI_STATE;
#define  SCSI_SF_scbRamSel  flags.bits.scbRamSel
#define  SCSI_SF_scbSize32  flags.bits.scbSize32
#define  SCSI_SF_chipPause  flags.bits.chipPause
#define  SCSI_SF_interruptEnable  flags.bits.interruptEnable
#define  SCSI_SF_mwiBit     flags.bits.mwiBit
#define  SCSI_SF_optMode    flags.bits.optMode
#if SCSI_TARGET_OPERATION
#define  SCSI_SF_selectionEnable  flags.bits.selectionEnable
#endif /* SCSI_TARGET_OPERATION */

/***************************************************************************
* SCSI_TARGET_CONTROL structures definitions
***************************************************************************/
typedef struct SCSI_TARGET_CONTROL_ SCSI_TARGET_CONTROL;
struct SCSI_TARGET_CONTROL_
{
   SCSI_HIOB SCSI_IPTR queueHead;            /* head of waiting queue         */
   SCSI_HIOB SCSI_IPTR queueTail;            /* head of waiting queue         */
   SCSI_UINT16 maxNonTagScbs;                /* maximum non tagged scbs       */
   SCSI_UINT16 maxTagScbs;                   /* maximum tagged scbs           */
   SCSI_UINT16 activeScbs;                   /* scbs associated in use        */
   SCSI_UINT16 origMaxNonTagScbs;            /* save original maxNonTagScbs   */
   SCSI_UINT16 origMaxTagScbs;               /* save original maxTagScbs      */
   SCSI_UINT16 origActiveScbs;               /* scbs associated in use        */
   SCSI_UINT8  activeBDRs;                   /* number of active BDRs         */
   SCSI_UINT8  activeABTTSKSETs;             /* number of active ABTTSKSETs   */
   SCSI_UINT8  freezeMap;                    /* freeze device queue bit map   */
#if SCSI_NEGOTIATION_PER_IOB
   SCSI_UINT8  pendingNego;                  /* negotiation is pending        */
#else
   SCSI_UINT8  alignment;                    /* padded for alignment          */
#endif
   SCSI_HIOB SCSI_IPTR freezeTail;           /* to remember device queue tail */
                                             /* during freeze logic           */
   SCSI_HIOB SCSI_IPTR freezeHostHead;       /* to remember head of host queue*/
   SCSI_HIOB SCSI_IPTR freezeHostTail;       /* to remember tail of host queue*/
   SCSI_UINT16 activeHiPriScbs;              /* hi priority scbs in rsm layer */
   SCSI_UINT16 deviceQueueHiobs;             /* number of hiobs in device q   */
};

/* definitions for freezeMap */
#define  SCSI_FREEZE_QFULL          0x01     /* queue full                    */
#define  SCSI_FREEZE_BDR            0x02     /* bus device reset              */
#define  SCSI_FREEZE_ERRORBUSY      0x04     /* freezeOnError flag or busy    */
#define  SCSI_FREEZE_EVENT          0x08     /* freeze/unfreeze async event   */
#define  SCSI_FREEZE_ABTTSKSET      0x10     /* abort task set                */
#define  SCSI_FREEZE_HIPRIEXISTS    0x20     /* hi priority scb handling      */

/***************************************************************************
* SCSI_UNIT_CONTROL structures definitions
***************************************************************************/
struct SCSI_UNIT_CONTROL_
{
   SCSI_HHCB SCSI_HPTR hhcb;                 /* host control block         */
   SCSI_UEXACT8 scsiID;                      /* device scsi id             */
   SCSI_UEXACT8 lunID;                       /* logical unit number        */
   SCSI_UEXACT8 alignment1;                  /* padding for alignment      */
   SCSI_UEXACT8 alignment2;                  /* padding for alignment      */
   SCSI_DEVICE SCSI_DPTR deviceTable;        /* device table               */
   SCSI_TARGET_CONTROL SCSI_HPTR targetControl; /* ptr to target control   */
};

#define  SCSI_NULL_UNIT_CONTROL    ((SCSI_UNIT_CONTROL SCSI_UPTR)-1)

/***************************************************************************
* SCSI_SHARE structures definitions
***************************************************************************/
typedef struct SCSI_SHARE_
{
   union                               /* pointer to array of active */
   {                                   /* pointer to HIOB            */
      SCSI_UINT32 u32;
      SCSI_HIOB SCSI_IPTR SCSI_HPTR activePointer;
   } activePtrArray;
   union
   {
      SCSI_UINT32 u32;                 /* free scb queue   */
      SCSI_SCB_DESCRIPTOR SCSI_HPTR freePointer;
   } freeQueue;
   union                               /* done queue                 */
   {                                   /* each array element will    */
      SCSI_UINT32 u32;                 /* be one byte (scb number)   */
      SCSI_HIOB SCSI_IPTR SCSI_HPTR donePointer;
   } doneQueue;
#if SCSI_RESOURCE_MANAGEMENT
   SCSI_UEXACT8 headDone;                     /* head of done queue           */
   SCSI_UEXACT8 tailDone;                     /* tail of done queue           */
   SCSI_UEXACT8 freeLow;                      /* low of free scb              */
   SCSI_UEXACT8 freeHigh;                     /* high of free scb             */
   SCSI_UEXACT8 freeScbAvail;                 /* number of free scbs available */
#if SCSI_TARGET_OPERATION
   SCSI_UEXACT8 freeEstScbAvail;              /* number of free scbs          */
                                              /* available for establish      */
                                              /* connections                  */   
   SCSI_UEXACT8 alignment[2];
#else
   SCSI_UEXACT8 alignment[3];
#endif /* SCSI_TARGET_OPERATION */
   SCSI_SCB_DESCRIPTOR SCSI_HPTR freeHead;   /* head of free scb queue        */
   SCSI_SCB_DESCRIPTOR SCSI_HPTR freeTail;   /* tail of free scb queue        */
   SCSI_HIOB SCSI_IPTR waitHead;             /* head of waiting queue         */
   SCSI_HIOB SCSI_IPTR waitTail;             /* tail of waiting queue         */
#if SCSI_TARGET_OPERATION
   SCSI_SCB_DESCRIPTOR SCSI_HPTR freeEstHead; /* head of free est scb queue   */
   SCSI_SCB_DESCRIPTOR SCSI_HPTR freeEstTail; /* tail of free est scb queue   */
#endif /* SCSI_TARGET_OPERATION */
#endif
} SCSI_SHARE;

#if SCSI_GROUP_DEVICES
#define  SCSI_ACTIVE_PTR_ARRAY     hhcbReserve.shareControl->activePtrArray.activePointer
#define  SCSI_FREE_SCB_AVAIL       hhcbReserve.shareControl->freeScbAvail
#define  SCSI_FREE_HEAD            hhcbReserve.shareControl->freeHead
#define  SCSI_FREE_TAIL            hhcbReserve.shareControl->freeTail
#define  SCSI_FREE_QUEUE           hhcbReserve.shareControl->freeQueue.freePointer
#define  SCSI_DONE_QUEUE           hhcbReserve.shareControl->doneQueue.donePointer
#define  SCSI_FREE_LO_SCB          hhcbReserve.shareControl->freeLow
#define  SCSI_FREE_HI_SCB          hhcbReserve.shareControl->freeHigh
#define  SCSI_DONE_HEAD            hhcbReserve.shareControl->headDone
#define  SCSI_DONE_TAIL            hhcbReserve.shareControl->tailDone
#define  SCSI_WAIT_HEAD            hhcbReserve.shareControl->waitHead
#define  SCSI_WAIT_TAIL            hhcbReserve.shareControl->waitTail
#if SCSI_TARGET_OPERATION
#define  SCSI_FREE_EST_SCB_AVAIL   hhcbReserve.shareControl->freeEstScbAvail
#define  SCSI_FREE_EST_HEAD        hhcbReserve.shareControl->freeEstHead
#define  SCSI_FREE_EST_TAIL        hhcbReserve.shareControl->freeEstTail
#endif /* SCSI_TARGET_OPERATION */
#else
#define  SCSI_ACTIVE_PTR_ARRAY     hhcbReserve.shareControl.activePtrArray.activePointer
#define  SCSI_FREE_SCB_AVAIL       hhcbReserve.shareControl.freeScbAvail
#define  SCSI_FREE_HEAD            hhcbReserve.shareControl.freeHead
#define  SCSI_FREE_TAIL            hhcbReserve.shareControl.freeTail
#define  SCSI_FREE_QUEUE           hhcbReserve.shareControl.freeQueue.freePointer
#define  SCSI_DONE_QUEUE           hhcbReserve.shareControl.doneQueue.donePointer
#define  SCSI_FREE_LO_SCB          hhcbReserve.shareControl.freeLow
#define  SCSI_FREE_HI_SCB          hhcbReserve.shareControl.freeHigh
#define  SCSI_DONE_HEAD            hhcbReserve.shareControl.headDone
#define  SCSI_DONE_TAIL            hhcbReserve.shareControl.tailDone
#define  SCSI_WAIT_HEAD            hhcbReserve.shareControl.waitHead
#define  SCSI_WAIT_TAIL            hhcbReserve.shareControl.waitTail
#if SCSI_TARGET_OPERATION
#define  SCSI_FREE_EST_SCB_AVAIL   hhcbReserve.shareControl.freeEstScbAvail
#define  SCSI_FREE_EST_HEAD        hhcbReserve.shareControl.freeEstHead
#define  SCSI_FREE_EST_TAIL        hhcbReserve.shareControl.freeEstTail
#endif /* SCSI_TARGET_OPERATION */
#endif

#define  SCSI_FREE_SCB             SCSI_FREE_HI_SCB

#define  SCSI_ACTPTR               hhcb->SCSI_ACTIVE_PTR_ARRAY

/***************************************************************************
* SCSI_HHCB_RESERVE structures definitions
***************************************************************************/
typedef struct SCSI_HHCB_RESERVE_
{
   union                               /* queue in pointer array     */
   {                                   /* element depending on       */
      SCSI_UINT32 u32;                 /* operating mode             */
      void SCSI_HPTR qinPointer;
   } qinPtrArray;
   union                               /* queue out pointer array     */
   {                                   /* element depending on       */
      SCSI_UINT32 u32;                 /* operating mode             */
      void SCSI_HPTR qoutPointer;
   } qoutPtrArray;
#if SCSI_SWAPPING_MODE + SCSI_SCBBFR_BUILTIN
   SCSI_BUFFER_DESCRIPTOR scbBuffer;   /* scb buffer                 */
#endif
   union                               
   {
      SCSI_UEXACT16 u16;
      struct
      {
         SCSI_UEXACT16 initNeeded:1;         /* initialization needed         */
         SCSI_UEXACT16 scratch1Exist:1;      /* scratch 1 area exists */
         SCSI_UEXACT16 scamSelectDetected:1; /* scam selection detected       */
         SCSI_UEXACT16 scamDominantInit:1;   /* scam dominant initiator(last invocation */
         SCSI_UEXACT16 scamSubordinateInit:1; /* scam subordinate initiator    */
         SCSI_UEXACT16 scamIDRetention:1;    /* scam ID retention             */
         SCSI_UEXACT16 autoTerminationMode:2;/* auto termination op mode      */
         SCSI_UEXACT16 scbSize32BitExist:1;  /* SCBSIZE32 bit in DEVCONFIG register exists */
         SCSI_UEXACT16 scsi3rdPartyReset:1;  /* 3rd party reset detected      */
         SCSI_UEXACT16 wwWriteHCNTRL:1;      /* workaround for write hcntrl   */
         SCSI_UEXACT16 wwReadINTSTAT:1;      /* workaround for read intstat   */
         SCSI_UEXACT16 respondToWideOrSynch:1; /* Responding to wide/synch nego */
         SCSI_UEXACT16 aicBayonet:1;         /* Bayonet ASIC detected */
         SCSI_UEXACT16 expRequest:1;         /* expander status requested  */
         SCSI_UEXACT16 nonExpBreak:1;        /* non expander bread int enabled */
      } bit;
   } flags;
   union                               
   {
      SCSI_UEXACT16 u16;
      struct
      {
         SCSI_UEXACT16 wwArbitorHung:1;      /* non expander bread int enabled */
         SCSI_UEXACT16 ultraCapable:1;       /* set if hardware supports ultra */ 
#if SCSI_TARGET_OPERATION && SCSI_RESOURCE_MANAGEMENT
         SCSI_UEXACT16 estScb:1;             /* set if est SCB is used for     */
                                             /* bus held case                  */
         SCSI_UEXACT16 reserved:13;          /* reserved for expansion */
#else
         SCSI_UEXACT16 disconnectOffForHDD:1;/* disable disconnect for HDD     */
         SCSI_UEXACT16 reserved:13;          /* reserved for expansion */
#endif /* SCSI_TARGET_OPERATION && SCSI_RESOURCE_MANAGEMENT */
      } bitMore;
   } flagsMore;
#if SCSI_NEW_MODE
   SCSI_UEXACT8 qNewHead;                    /* q_new_head copy for HIM       */
   SCSI_UEXACT8 qDoneElement;                /* q_done_element for HIM        */
   SCSI_UEXACT8 qDonePass;                   /* q_done_pass copy for HIM      */
#if SCSI_TARGET_OPERATION
   SCSI_UEXACT8 qEstHead;                    /* q_est_head copy for HIM       */
#else
   SCSI_UEXACT8 alignment1;                  /* padding for alignment         */
#endif /* SCSI_TARGET_OPERATION */
#endif /* SCSI_NEW_MODE */
   SCSI_UEXACT8 intstat;                     /* interrupt state saved         */
   SCSI_UEXACT8 hcntrl;                      /* host control register         */
   SCSI_UEXACT8 qoutcnt;                     /* qoutcnt saved                 */
   SCSI_UEXACT8 se2Type;                     /* type of seeprom attached      */
#if SCSI_MULTI_MODE
   SCSI_FIRMWARE_DESCRIPTOR SCSI_LPTR firmwareDescriptor; /* firmware descriptor  */
#endif
#if SCSI_MULTI_HARDWARE
   SCSI_HARDWARE_DESCRIPTOR SCSI_LPTR hardwareDescriptor;  /* hardware descriptor  */
#endif
#if SCSI_RESOURCE_MANAGEMENT
#if SCSI_DMA_SUPPORT
   /* Need to see if we could use a separate structure */
   SCSI_TARGET_CONTROL targetControl[SCSI_MAXDEV+1]; /* target control table  */
                                                     /* one extra for DMA I/O */
                                                     /* target.               */
   SCSI_UNIT_CONTROL unitControl;            /* host unit control             */
#else
   SCSI_TARGET_CONTROL targetControl[SCSI_MAXDEV]; /* target control table    */
#endif /* SCSI_DMA_SUPPORT */
#endif
#if SCSI_GROUP_DEVICES
   SCSI_SHARE SCSI_HPTR shareControl;/* shared control              */
#else
   SCSI_SHARE shareControl;/* shared control              */
#endif
#if SCSI_CHECK_PCI_ERROR
   SCSI_UEXACT8 error;                        /* error status register saved   */
#endif
#if SCSI_TARGET_OPERATION
   SCSI_NODE SCSI_NPTR nodeTable[SCSI_MAXDEV];   /* Node table                 */
   SCSI_NEXUS  SCSI_XPTR nexusHoldingBus;        /* Nexus Holding the SCSI BUS */
#if SCSI_MULTIPLEID_SUPPORT
   SCSI_HIOB SCSI_IPTR disableHiob;              /* HIOB associated with a     */
                                                 /* DISABLE_ID request         */ 
#endif /* SCSI_MULTIPLEID_SUPPORT */
#if SCSI_RESOURCE_MANAGEMENT
   SCSI_SCB_DESCRIPTOR SCSI_HPTR busHeldScbDescriptor; /* scb being used for a */
                                                       /* target mode bus held */
                                                       /* case                 */
#endif /* SCSI_RESOURCE_MANAGEMENT */
#endif /* SCSI_TARGET_OPERATION */
} SCSI_HHCB_RESERVE;

#define  SCSI_HP_firmwareDescriptor hhcbReserve.firmwareDescriptor
#define  SCSI_HP_hardwareDescriptor hhcbReserve.hardwareDescriptor
#define  SCSI_HP_qNewHead           hhcbReserve.qNewHead    
#define  SCSI_HP_qDoneElement       hhcbReserve.qDoneElement
#define  SCSI_HP_qDonePass          hhcbReserve.qDonePass   
#define  SCSI_HP_intstat            hhcbReserve.intstat  
#define  SCSI_HP_hcntrl             hhcbReserve.hcntrl   
#define  SCSI_HP_qoutcnt            hhcbReserve.qoutcnt  
#define  SCSI_HP_se2Type            hhcbReserve.se2Type
#define  SCSI_HP_targetControl      hhcbReserve.targetControl
#define  SCSI_HP_initNeeded         hhcbReserve.flags.bit.initNeeded
#define  SCSI_HP_scratch1Exist      hhcbReserve.flags.bit.scratch1Exist
#define  SCSI_HP_scamSelectDetected hhcbReserve.flags.bit.scamSelectDetected
#define  SCSI_HP_scamDominantInit   hhcbReserve.flags.bit.scamDominantInit
#define  SCSI_HP_scamSubordinateInit hhcbReserve.flags.bit.scamSubordinateInit
#define  SCSI_HP_scamIDRetention    hhcbReserve.flags.bit.scamIDRetention
#define  SCSI_HP_autoTerminationMode hhcbReserve.flags.bit.autoTerminationMode
#define  SCSI_HP_scbSize32BitExist  hhcbReserve.flags.bit.scbSize32BitExist
#define  SCSI_HP_scsi3rdPartyReset  hhcbReserve.flags.bit.scamIDRetention
#define  SCSI_HP_wwWriteHCNTRL      hhcbReserve.flags.bit.wwWriteHCNTRL
#define  SCSI_HP_wwReadINTSTAT      hhcbReserve.flags.bit.wwReadINTSTAT
#define  SCSI_HP_respondToWideOrSynch hhcbReserve.flags.bit.respondToWideOrSynch
#define  SCSI_HP_aicBayonet         hhcbReserve.flags.bit.aicBayonet
#define  SCSI_HP_expRequest         hhcbReserve.flags.bit.expRequest
#define  SCSI_HP_nonExpBreak        hhcbReserve.flags.bit.nonExpBreak
#define  SCSI_HP_wwArbitorHung      hhcbReserve.flagsMore.bitMore.wwArbitorHung
#define  SCSI_HP_ultraCapable       hhcbReserve.flagsMore.bitMore.ultraCapable
#define  SCSI_HP_disconnectOffForHDD hhcbReserve.flagsMore.bitMore.disconnectOffForHDD
#define  SCSI_QOUT_PTR_ARRAY        hhcbReserve.qoutPtrArray.qoutPointer
#define  SCSI_QOUT_PTR_ARRAY_NEW(hhcb) ((SCSI_QOUTFIFO_NEW SCSI_HPTR) (hhcb)->SCSI_QOUT_PTR_ARRAY)
#define  SCSI_SCB_BUFFER            hhcbReserve.scbBuffer
#if SCSI_CHECK_PCI_ERROR
#define  SCSI_HP_error              hhcbReserve.error  
#endif /* SCSI_CHECK_PCI_ERROR */
#if SCSI_DMA_SUPPORT
#define  SCSI_HP_unitControl        hhcbReserve.unitControl
#endif /* SCSI_DMA_SUPPORT */
#if SCSI_TARGET_OPERATION
#define  SCSI_HP_qEstHead           hhcbReserve.qEstHead
#define  SCSI_HP_nodeTable          hhcbReserve.nodeTable
#define  SCSI_HP_nexusHoldingBus    hhcbReserve.nexusHoldingBus 
#if SCSI_MULTIPLEID_SUPPORT
#define  SCSI_HP_disableHiob        hhcbReserve.disableHiob 
#endif /* SCSI_MULTIPLEID_SUPPORT */
#if SCSI_RESOURCE_MANAGEMENT
#define  SCSI_HP_busHeldScbDescriptor hhcbReserve.busHeldScbDescriptor
#define  SCSI_HP_estScb             hhcbReserve.flagsMore.bitMore.estScb
#endif /* SCSI_RESOURCE_MANAGEMENT */
#endif /* SCSI_TARGET_OPERATION */

/***************************************************************************
* SCSI_HIOB_RESERVE structures definitions
***************************************************************************/
#define  SCSI_MAX_WORKAREA   10             /* maximum working area length   */
typedef struct SCSI_HIOB_RESERVE_
{
   SCSI_UEXACT8 mgrStat;                      /* manager status                */
   SCSI_UEXACT8 negoState;                    /* state of negotiation process  */
   SCSI_UEXACT8 ackLastMsg;                   /* flag to check if ack is needed*/
   SCSI_UEXACT8 workArea[SCSI_MAX_WORKAREA];  /* protocol working area         */
#if SCSI_TARGET_OPERATION
   union 
   {
      SCSI_UEXACT8 u8;
      struct
      {
         SCSI_UEXACT8 targetMode:1;           /* when set to 1 indicates that  */
                                              /* this is a target mode HIOB    */
         SCSI_UEXACT8 estScb:1;               /* when set to 1 indicates that  */
                                              /* the scb being used for this   */
                                              /* HIOB is an establish          */
                                              /* connection scb. An est scbd   */
                                              /* is used when an establish     */
                                              /* connection completes holding  */
                                              /* the SCSI bus. The purpose     */
                                              /* is to know which queue to     */
                                              /* return the scb when the HIOB  */
                                              /* is complete.                  */  
      } bits;
   } flags;
   SCSI_UEXACT8 alignment[2];                 /* padded for alignment          */
#else
   SCSI_UEXACT8 alignment[3];                 /* padded for alignment          */
#endif /* SCSI_TARGET_OPERATION */         
} SCSI_HIOB_RESERVE;

/* definitions for negoState */
#define  SCSI_NOT_NEGOTIATING   0           /* not in the state of nego      */
#define  SCSI_INITIATE_WIDE     1           /* initiate wide negotiation     */
#define  SCSI_INITIATE_SYNC     2           /* initiate sync negotiation     */
#define  SCSI_RESPONSE_WIDE     3           /* respond to wide negotiation   */
#define  SCSI_RESPONSE_SYNC     4           /* respond to sync negotiation   */
#define  SCSI_INITIATE_PPR      5     /* initiate Parallel Protocol Request  */
#define  SCSI_RESPONSE_PPR      6     /* respond to Parallel Protocol Request*/
#define  SCSI_NEGOTIATE_NEEDED  7           /* negotiation needed            */

/* definitions for ackLastMsg */
#define  SCSI_ACK_NOT_NEEDED    0
#define  SCSI_ACK_NEEDED        1

#define  SCSI_IP_mgrStat       hiobReserve.mgrStat
#define  SCSI_IP_negoState     hiobReserve.negoState
#define  SCSI_IP_ackLastMsg    hiobReserve.ackLastMsg
#define  SCSI_IP_workArea      hiobReserve.workArea
#if SCSI_TARGET_OPERATION
#define  SCSI_IP_targetMode    hiobReserve.flags.bits.targetMode
#define  SCSI_IP_estScb        hiobReserve.flags.bits.estScb 


/* definitions for Establish Connection fields saved in */
/* hiob workarea                                        */
#define  SCSI_EST_IID         0
#define  SCSI_EST_IDMSG       1
#define  SCSI_EST_FLAGS       2
#define  SCSI_EST_LASTBYTE    3 
#define  SCSI_EST_STATUS      4 

/* definitions for SCSI_EST_STATUS */
#define SCSI_EST_GOOD_STATE    0x00
#define SCSI_EST_SEL_STATE     0x01 /* exception during selection phase   */
#define SCSI_EST_ID_STATE      0x02 /*     "        "   identifying phase */
#define SCSI_EST_MSGOUT_STATE  0x03 /*     "        "   message out phase */
#define SCSI_EST_CMD_STATE     0x04 /*     "        "   command phase     */
#define SCSI_EST_DISC_STATE    0x05 /*     "        "   disconnect phase  */
#define SCSI_EST_VENDOR_CMD    0x08 /* Vendor unique command              */

/* definition of status mask */
#define SCSI_EST_STATUS_MASK   0x0F
 
/* definitions for flag fields of SCSI_EST_STATUS */
#define SCSI_EST_LAST_RESOURCE 0x40  /* last resource used */
#define SCSI_EST_PARITY_ERR    0x80  /* parity error detected */
 
#endif /* SCSI_TARGET_MODE */ 

/****************************************************************************
* definitions for mgrStat of HIOB
****************************************************************************/
#define  SCSI_MGR_PROCESS     0           /* SCB needs to be processed           */
#define  SCSI_MGR_DONE        1           /* SCB finished without error (hwm)    */
#define  SCSI_MGR_DONE_ABT    2           /* SCB finished due to abort from host */
#define  SCSI_MGR_DONE_ERR    3           /* SCB finished with error             */
#define  SCSI_MGR_DONE_ILL    4           /* SCB finished due to illegal command */
#define  SCSI_MGR_ABORTINPROG 5           /* Abort special function in progress  */
#define  SCSI_MGR_BDR         6           /* Bus Device Reset special function   */
#define  SCSI_MGR_AUTOSENSE   7           /* SCB w/autosense in progress         */
#define  SCSI_MGR_ABORTED     8           /* aborted                             */
#define  SCSI_MGR_ABORTINREQ  9           /* Abort HIOB in request               */
#define  SCSI_MGR_ENQUE_BDR   10          /* Enqueue Bus Device Reset flag       */
#if SCSI_TARGET_OPERATION
#define  SCSI_MGR_IGNOREWIDEINPROG 11     /* Ignore Wide Residue msg in progress */
#endif /* SCSI_TARGET_OPERATION */

/***************************************************************************
* SCSI_SCB_DESCRIPTOR structure definition
***************************************************************************/
struct SCSI_SCB_DESCRIPTOR_
{
   SCSI_UEXACT8 scbNumber;                /* scb Number                    */
   SCSI_UEXACT8 reserved[3];              /* for alignment                 */
   SCSI_BUFFER_DESCRIPTOR scbBuffer;      /* scb buffer descriptor         */
   SCSI_SCB_DESCRIPTOR SCSI_HPTR queueNext;    /* next scb queue element   */
};

#if SCSI_TARGET_OPERATION
/***************************************************************************
* SCSI_NEXUS_RESERVE structures definitions
***************************************************************************/
typedef struct SCSI_NEXUS_RESERVE_
{
   union
   {
      SCSI_UEXACT8 u8;
      struct
      {
         SCSI_UEXACT8 abortInProgress:1;      /* abort in progress             */  
         SCSI_UEXACT8 deviceResetInProgress:1;/* device reset in progress      */ 
         SCSI_UEXACT8 nexusActive:1;          /* nexus active i.e. not avail   */   
         SCSI_UEXACT8 parityErr:1;    
         SCSI_UEXACT8 taskManagementFunction:1;
#if SCSI_RESOURCE_MANAGEMENT
         SCSI_UEXACT8 hiobActive:1;           /* used by SCSIClearNexus for    */
                                              /* clearing HIOBs                */ 
#endif
      } bits;
   } nexusFlags;
   SCSI_UEXACT8 lunMask;
   SCSI_UEXACT8 alignment[2];                 /* padded for alignment          */
 } SCSI_NEXUS_RESERVE;

#define SCSI_XP_abortInProgress       nexusReserve.nexusFlags.bits.abortInProgress
#define SCSI_XP_deviceResetInProgress nexusReserve.nexusFlags.bits.deviceResetInProgress
#define SCSI_XP_parityErr             nexusReserve.nexusFlags.bits.parityErr
#define SCSI_XP_nexusActive           nexusReserve.nexusFlags.bits.nexusActive
#define SCSI_XP_taskManagementFunction  nexusReserve.nexusFlags.bits.taskManagementFunction
#define SCSI_XP_lunMask               nexusReserve.lunMask
#if SCSI_RESOURCE_MANAGEMENT
#define SCSI_XP_hiobActive            nexusReserve.nexusFlags.bits.hiobActive
#endif /* SCSI_RESOURCE_MANAGEMENT */

/* lunmask field defines */
#define  SCSI_3BIT_LUN_MASK      0x07
#define  SCSI_5BIT_LUN_MASK      0x1f
#define  SCSI_6BIT_LUN_MASK      0x3f

/***************************************************************************
* SCSI_NODE_RESERVE structures definitions
***************************************************************************/
typedef struct SCSI_NODE_RESERVE_
{
   union 
   {
      SCSI_UEXACT8 u8;
      struct 
      {
         SCSI_UEXACT8 forceNegotiation:1; /* used only if initiator  */ 
                                          /* mode is enabled. Force  */
                                          /* target negotiation.     */
                                          /* Special bit used when a */
                                          /* check condition is      */
                                          /* received on an          */
                                          /* Initiator I/O.          */
         SCSI_UEXACT8 negotiated:1;     /* negotiated with initiator */ 
         SCSI_UEXACT8 retry:1;          /* retry the SCSI phase      */
      } bits;
   } flags;
   SCSI_UEXACT8  scsiState;             /* current SCSI state        */
   SCSI_UEXACT8  scsiAtnState;          /* last SCSI state after ATN */
   SCSI_UEXACT8  scsiMsgSize;           /* last message in size      */
   SCSI_UEXACT8  scsiMsgIn[8];          /* last entire message       */
 } SCSI_NODE_RESERVE;

#define  SCSI_NP_negotiated           nodeReserve.flags.bits.negotiated
#define  SCSI_NP_forceTargetNegotiation nodeReserve.flags.bits.forceNegotiation
#define  SCSI_NP_retry                nodeReserve.flags.bits.retry
#define  SCSI_NP_scsiState            nodeReserve.scsiState
#define  SCSI_NP_scsiAtnState         nodeReserve.scsiAtnState
#define  SCSI_NP_scsiMsgSize          nodeReserve.scsiMsgSize
#define  SCSI_NP_scsiMsgIn            nodeReserve.scsiMsgIn

#endif /* SCSI_TARGET_OPERATION */

/*$Header:   Y:/source/chimscsi/src/himscsi/hwmscsi/SCSIDEF.HV_   1.55.3.4   06 Apr 1998 18:36:34   MAKI1985  $*/
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
*  Module Name:   SCSIDEF.H
*
*  Description:   Definitions as interface to HIM internal layers
*    
*  Owners:        ECX IC Firmware Team
*    
*  Notes:         1. This file is referenced by all three layers of Common
*                    HIM implementation. 
*
***************************************************************************/

/***************************************************************************
* Miscellaneous
***************************************************************************/
#ifndef SCSI_ACCESS_RAM
#define SCSI_ACCESS_RAM SCSI_BIOS_ASPI8DOS
#endif /* SCSI_ACCESS_RAM */
#define  SCSI_MAXLUN       64

/***************************************************************************
* SCSI_DEVICE structures definitions
***************************************************************************/
#define SCSI_SIZEWWID   32
struct SCSI_DEVICE_
{
   struct
   {
      SCSI_UEXACT16 ultraEnable:1;           /* ultra speed enable            */
      SCSI_UEXACT16 disconnectEnable:1;      /* disconnect enable             */
#if SCSI_SWAPPING32_MODE || SCSI_SWAPPING64_MODE
      SCSI_UEXACT16 fast20EnableW:1;         /* bitmap for fast20 enable      */
#else
      SCSI_UEXACT16 reserved1:1;
#endif
#if SCSI_SCAM_ENABLE
      SCSI_UEXACT16 scamDevice:1;            /* scam device identified        */
      SCSI_UEXACT16 scamTolerant:1;          /* scam tolerant device          */
#else
      SCSI_UEXACT16 reserved2:2;
#endif
      SCSI_UEXACT16 setForSync:1;            /* currently set for sync. xfer  */
      SCSI_UEXACT16 setForWide:1;            /* currently set for wide xfer   */
      SCSI_UEXACT16 syncDevice:1;            /* synchronous device            */
      SCSI_UEXACT16 wideDevice:1;            /* wide device                   */
      SCSI_UEXACT16 wasSyncXfer:1;           /* was synchronous xfer          */
      SCSI_UEXACT16 wasWideXfer:1;           /* was wide xfer                 */
      SCSI_UEXACT16 behindExp:1;             /* target is behind bus expander */
      SCSI_UEXACT16 reserved4:1;
      SCSI_UEXACT16 resetSCSI:1;             /* target been reset?            */
#if SCSI_PPR_ENABLE
      SCSI_UEXACT16 dtcSupport:1;            /* target supports DT clocking   */
      SCSI_UEXACT16 stcSupport:1;            /* target supports ST clocking   */      
#else
      SCSI_UEXACT16 reserved3:2;
#endif
   } flags;

#if (SCSI_SWAPPING_MODE || SCSI_STANDARD_ADVANCED_MODE || SCSI_STANDARD_160M_MODE)
#if SCSI_MULTIPLEID_SUPPORT && SCSI_TARGET_OPERATION
   SCSI_UEXACT16 xferOptionHost[SCSI_MAXDEV];/* resulted transfer option      */
                                             /* indexed by our ID             */
   SCSI_UEXACT16 reserved1;                  /* padded for alignment          */
#else
   SCSI_UEXACT16 xferOptionHost;             /* resulted transfer option      */
#endif /* SCSI_MULTIPLEID_SUPPORT && SCSI_TARGET_OPERATION */
#else
   SCSI_UEXACT16 reserved1;                  /* padded for alignment          */
#endif
   SCSI_UEXACT8 scsiOption;                  /* options for SCSI transfer     */
#if (SCSI_AICBAYONET || SCSI_AICTRIDENT)
   SCSI_UEXACT8 bayScsiRate;                 /* options for SCSI transfer     */
   SCSI_UEXACT8 bayScsiOffset;               /* options for SCSI transfer     */
   SCSI_UEXACT8 bayDefaultRate;              /* default rate at Domain Validation */
   SCSI_UEXACT8 bayDefaultOffset;            /* default offset at Domain Validation */
   SCSI_UEXACT8 bayDvScsiRate;               /* rate during Domain Validation */
   SCSI_UEXACT8 bayDvScsiOffset;             /* offset during Domain Validation */
#endif
#if SCSI_NEGOTIATION_PER_IOB
   SCSI_UEXACT8 origOffset;
   struct
   {
      SCSI_UEXACT16 forceSync:1;
      SCSI_UEXACT16 forceAsync:1;
      SCSI_UEXACT16 forceWide:1;
      SCSI_UEXACT16 forceNarrow:1;
      SCSI_UEXACT16 reserved:12;             /* for future use                */
   } negoFlags;
   SCSI_UEXACT8 reserved2[3];                /* padded for alignment          */
#else
   SCSI_UEXACT8 reserved2[1];                /* padded for alignment          */
#endif
   struct
   {
      SCSI_UEXACT16 hostManaged:1;           /* host managed (NTC)            */
      SCSI_UEXACT16 suppressNego:1;          /* suppress negotiation          */
#if SCSI_PPR_ENABLE
      SCSI_UEXACT16 negotiateDtc:1;          /* DTC negotiation allowed       */
      SCSI_UEXACT16 reserved1:13;            /* for future use                */
#else
      SCSI_UEXACT16 reserved1:14;            /* for future use                */
#endif
   } flags2;
#if SCSI_SCAM_ENABLE
   SCSI_UEXACT8 scamWWID[SCSI_SIZEWWID];     /* world wide id                 */
#endif
};

#define  SCSI_DF_ultraEnable        flags.ultraEnable
#define  SCSI_DF_disconnectEnable   flags.disconnectEnable
#define  SCSI_DF_fast20EnableW      flags.fast20EnableW
#define  SCSI_DF_scamDevice         flags.scamDevice
#define  SCSI_DF_scamTolerant       flags.scamTolerant
#define  SCSI_DF_setForSync         flags.setForSync
#define  SCSI_DF_setForWide         flags.setForWide
#define  SCSI_DF_syncDevice         flags.syncDevice
#define  SCSI_DF_wideDevice         flags.wideDevice
#define  SCSI_DF_wasSyncXfer        flags.wasSyncXfer
#define  SCSI_DF_wasWideXfer        flags.wasWideXfer
#define  SCSI_DF_behindExp          flags.behindExp
#define  SCSI_DF_resetSCSI          flags.resetSCSI
#define  SCSI_DF_dtcSupport         flags.dtcSupport
#define  SCSI_DF_stcSupport         flags.stcSupport
#if SCSI_NEGOTIATION_PER_IOB
#define  SCSI_DNF_forceSync         negoFlags.forceSync
#define  SCSI_DNF_forceAsync        negoFlags.forceAsync
#define  SCSI_DNF_forceNarrow       negoFlags.forceNarrow
#define  SCSI_DNF_forceWide         negoFlags.forceWide
#endif

#define  SCSI_DF_hostManaged        flags2.hostManaged
#define  SCSI_DF_suppressNego       flags2.suppressNego
#if SCSI_PPR_ENABLE
#define  SCSI_DF_negotiateDtc       flags2.negotiateDtc
#endif

/***************************************************************************
* SCSI_UINT_HANDLE structures definitions
***************************************************************************/
typedef union SCSI_UNIT_HANDLE_
{
   SCSI_UNIT_CONTROL SCSI_UPTR targetUnit;      /* as target unit handle   */
   SCSI_HHCB SCSI_HPTR initiatorUnit;           /* as hhcb                 */
   SCSI_HIOB SCSI_IPTR relatedHiob;             /* related (aborted) hiob  */
#if SCSI_TARGET_OPERATION
   SCSI_NEXUS SCSI_XPTR nexus;                  /* as nexus (target mode)  */
#endif /* SCSI_TARGET_OPERATION */
} SCSI_UNIT_HANDLE;

/***************************************************************************
* SCSI_MEMORY_TABLE structures definitions
***************************************************************************/
#if !SCSI_DOWNSHIFT_MODE
typedef struct SCSI_MEMORY_TABLE_
{
   struct
   {
      SCSI_UEXACT8 memoryCategory;              /* memory category         */
      SCSI_UEXACT8 memoryAlignment;             /* alignment offset        */
      SCSI_UEXACT8 memoryType;                  /* memoryCategory          */
      SCSI_UEXACT8 granularity;                 /* alignment               */
      SCSI_UEXACT16 memorySize;                 /* size of memory required */
      SCSI_UEXACT16 minimumSize;                /* size of memory required */
      union
      {
         void SCSI_HPTR hPtr;          
         void SCSI_IPTR iPtr;
         void SCSI_SPTR sPtr;
         void SCSI_FPTR fPtr;
         void SCSI_LPTR lPtr;
         void SCSI_DPTR dPtr;
         void SCSI_UPTR uPtr;
#if SCSI_TARGET_OPERATION
         void SCSI_XPTR xPtr;
         void SCSI_NPTR nPtr;
#endif /* SCSI_TARGET_OPERATION */
      } ptr;
   } memory[SCSI_MAX_MEMORY];
} SCSI_MEMORY_TABLE;

/* memory types */
#define  SCSI_MT_HPTR   0                       /* ptr to hhcb */
#define  SCSI_MT_IPTR   1                       /* ptr to hiob */
#define  SCSI_MT_SPTR   2                       /* ptr to stack */
#define  SCSI_MT_FPTR   3                       /* ptr to function space */
#define  SCSI_MT_LPTR   4                       /* ptr to local data */
#define  SCST_MT_DPTR   5                       /* ptr to device table */
#define  SCSI_MT_UPTR   6                       /* ptr to target unit */
#if SCSI_TARGET_OPERATION 
#define  SCSI_MT_XPTR   7                       /* ptr to nexus memory (target mode) */
#define  SCSI_MT_NPTR   8                       /* ptr to node memory (target mode)  */ 
#endif /* SCSI_TARGET_OPERATION */
#endif

/***************************************************************************
* Target Mode specific HHCB structures definitions
***************************************************************************/
#if SCSI_TARGET_OPERATION
typedef struct SCSI_HHCB_TARGET_MODE_
{
#if SCSI_RESOURCE_MANAGEMENT
   /* Target Mode fields */
   SCSI_UEXACT16 numberNexusHandles;      /* number of nexus task set handles          */
   SCSI_UEXACT8  numberNodeHandles;       /* number of node task set handles           */
   SCSI_UEXACT8  numberEstScbs;           /* number of Establish Connection Scbs       */
   SCSI_UEXACT16 nexusQueueCnt;           /* number of available nexus handles         */
   SCSI_UEXACT16 nexusThreshold;          /* nexus threshold for generating an event   */
   SCSI_NEXUS SCSI_XPTR nexusQueue;       /* head of nexus queue                       */  
   SCSI_UEXACT16 hiobQueueCnt;            /* number of Establish Connection HIOBs
                                           * queued
                                           */
   SCSI_UEXACT16 hiobQueueThreshold;      /* hiob threshold for generating an event    */
   SCSI_HIOB SCSI_IPTR hiobQueue;         /* Establish Connection HIOB available queue */ 
   SCSI_NODE SCSI_NPTR nodeQueue;         /* Node available queue                      */
#endif
   SCSI_UEXACT8  group6CDBsz;             /* # CDB bytes for a group 6 vendor unique   */
                                          /* command                                   */
   SCSI_UEXACT8  group7CDBsz;             /* # CDB bytes for a group 7 vendor unique   */
                                          /* command                                   */
   SCSI_UEXACT8  hostTargetVersion;       /* ANSI version for target mode operation.   */
                                          /* SCSI_2 or SCSI_3                          */
   union
   {
      SCSI_UEXACT8 u8;
      struct
      {
         SCSI_UEXACT8  abortTask:1;       /* Abort Tag/Task message supported          */ 
         SCSI_UEXACT8  clearTaskSet:1;    /* Clear Queue/Task Set message supported    */
         SCSI_UEXACT8  terminateTask:1;   /* Terminate I/O Process/Task message        */
                                          /* supported                                 */
         SCSI_UEXACT8  clearACA:1;        /* Clear ACA message supported               */
         SCSI_UEXACT8  logicalUnitReset:1;/* Logical Unit Reset message supported      */
      } bits;
   } targetTaskMngtOpts;
   union
   {
      SCSI_UEXACT16 u16;
      struct
      {
         SCSI_UEXACT16  disconnectAllowed:1;       /* Disconnect Allowed enable/disable*/
         SCSI_UEXACT16  tagEnable:1;               /* Tagged requests enable/disable   */
         SCSI_UEXACT16  outOfOrderTransfers:1;     /* Out of order transfers enable/   */
                                                   /* disable                          */
         SCSI_UEXACT16  scsi2IdentifyMsgRsv:1;     /* Generate message reject when     */
                                                   /* reserved bit set in identify msg */ 
         SCSI_UEXACT16  scsi2RejectLuntar:1;       /* Generate message reject when     */
                                                   /* LunTar bit set in identify msg   */ 
         SCSI_UEXACT16  ignoreWideResidueMsg:1;    /* Generate Ignore Wide Residue     */
                                                   /* on wide odd transfer             */ 
         SCSI_UEXACT16  scsiBusHeld:1;             /* SCSI bus is currently held       */
         SCSI_UEXACT16  initNegotiation:1;         /* Target initiated negotiation     */
                                                   /* enabled/disabled                 */
         SCSI_UEXACT16  scsi1Selection:1;          /* Accept a SCSI-1 selection        */
         SCSI_UEXACT16  holdEstHiobs:1;            /* Don't queue establish connection */
                                                   /* HIOBs to HWM layer               */  
      } bits;
   } targetFlags;
   SCSI_UEXACT16 targetIDMask;            /* mask of enabled SCSI IDs when operating   */
                                          /* as a target                               */
   SCSI_UEXACT8 targetNumIDs;             /* number of IDs supported when operating    */
                                          /* as a target                               */
   SCSI_UEXACT8 reserved1[3];             /* alignment                                 */
} SCSI_HHCB_TARGET_MODE;

/* Definitions for target HHCB fields */
#if SCSI_RESOURCE_MANAGEMENT
#define SCSI_HF_targetNumberNexusHandles   targetMode.numberNexusHandles
#define SCSI_HF_targetNumberNodeHandles    targetMode.numberNodeHandles
#define SCSI_HF_targetNumberEstScbs        targetMode.numberEstScbs
#define SCSI_HF_targetNexusQueueCnt        targetMode.nexusQueueCnt
#define SCSI_HF_targetNexusThreshold       targetMode.nexusThreshold
#define SCSI_HF_targetNexusQueue           targetMode.nexusQueue
#define SCSI_HF_targetHiobQueueCnt         targetMode.hiobQueueCnt
#define SCSI_HF_targetHiobQueueThreshold   targetMode.hiobQueueThreshold
#define SCSI_HF_targetHiobQueue            targetMode.hiobQueue
#define SCSI_HF_targetNodeQueue            targetMode.nodeQueue
#endif /* SCSI_RESOURCE_MANAGEMENT */

#define SCSI_HF_targetGroup6CDBsz          targetMode.group6CDBsz
#define SCSI_HF_targetGroup7CDBsz          targetMode.group7CDBsz 
#define SCSI_HF_targetHostTargetVersion    targetMode.hostTargetVersion
#define SCSI_HF_targetTaskMngtOpts         targetMode.targetTaskMngtOpts.u8
#define SCSI_HF_targetNumIDs               targetMode.targetNumIDs
#define SCSI_HF_targetAdapterIDMask        targetMode.targetIDMask
 
/* Definitions for target HHCB Flags bits */
#define  SCSI_HF_targetDisconnectAllowed   targetMode.targetFlags.bits.disconnectAllowed
#define  SCSI_HF_targetTagEnable           targetMode.targetFlags.bits.tagEnable 
#define  SCSI_HF_targetOutOfOrderTransfers targetMode.targetFlags.bits.outOfOrderTransfers
#define  SCSI_HF_targetScsi2IdentifyMsgRsv targetMode.targetFlags.bits.scsi2IdentifyMsgRsv
#define  SCSI_HF_targetIgnoreWideResidMsg  targetMode.targetFlags.bits.ignoreWideResidueMsg
#define  SCSI_HF_targetScsiBusHeld         targetMode.targetFlags.bits.scsiBusHeld
#define  SCSI_HF_targetInitNegotiation     targetMode.targetFlags.bits.initNegotiation
#define  SCSI_HF_targetEnableScsi1Selection targetMode.targetFlags.bits.scsi1Selection 
#define  SCSI_HF_targetScsi2RejectLuntar   targetMode.targetFlags.bits.scsi2RejectLuntar
#define  SCSI_HF_targetHoldEstHiobs        targetMode.targetFlags.bits.holdEstHiobs

/* Definitions for target HHCB Task Management Flags bits */
#define  SCSI_HF_targetAbortTask   targetMode.targetTaskMngtOpts.bits.abortTask
#define  SCSI_HF_targetClearTaskSet   targetMode.targetTaskMngtOpts.bits.clearTaskSet
#define  SCSI_HF_targetTerminateTask   targetMode.targetTaskMngtOpts.bits.terminateTask
#define  SCSI_HF_targetClearACA   targetMode.targetTaskMngtOpts.bits.clearACA
#define  SCSI_HF_targetLogicalUnitReset   targetMode.targetTaskMngtOpts.bits.logicalUnitReset

/* Definitions for default values */
#define  SCSI_Group6CDBDefaultSize      12
#define  SCSI_Group7CDBDefaultSize      12

/* Definitions for hostTargetVersion */
#define  SCSI_VERSION_2                 2
#define  SCSI_VERSION_3                 3
    
#endif /* SCSI_TARGET_OPERATION */

/***************************************************************************
* HHCB structures definitions
***************************************************************************/
struct SCSI_HHCB_
{
   SCSI_UEXACT16 deviceID;                      /* device id                     */
   SCSI_UEXACT16 productID;                     /* product id                    */
   SCSI_UEXACT8 hardwareRevision;               /* hardware revision             */
   SCSI_UEXACT8 firmwareVersion;                /* firmware version              */
   SCSI_UEXACT8 softwareVersion;                /* software version              */
   SCSI_UEXACT8 numberScbs;                     /* number of scbs configured     */
   SCSI_REGISTER scsiRegister;                  /* register access               */
   union
   {
      SCSI_UEXACT16 u16;
      struct 
      {
         SCSI_UEXACT16 scsiParity:1;            /* scsi parity check enable      */
         SCSI_UEXACT16 resetSCSI:1;             /* reset SCSI at initialization  */
         SCSI_UEXACT16 multiTaskLun:1;          /* multitasking target/lun       */
         SCSI_UEXACT16 terminationLow:1;        /* termination low enable        */
         SCSI_UEXACT16 terminationHigh:1;       /* termination high enable       */
         SCSI_UEXACT16 autoTermination:1;       /* auto termination              */
         SCSI_UEXACT16 secondaryTermLow:1;      /* termination low enable        */
         SCSI_UEXACT16 secondaryTermHigh:1;     /* termination high enable       */
         SCSI_UEXACT16 secondaryAutoTerm:1;     /* auto termination              */
         SCSI_UEXACT16 scamLevel:2;             /* level of scam supported       */
         SCSI_UEXACT16 skipScam:1;              /* skip Scam protocol            */
         SCSI_UEXACT16 selTimeout:2;            /* selection time out            */
         SCSI_UEXACT16 expSupport:1;            /* bus expander supported        */
         SCSI_UEXACT16 cacheThEn:1;             /* cache threshold enable state  */
      } bits;
   } flags;
   union
   {
      SCSI_UEXACT16 u16;
      struct 
      {
         SCSI_UEXACT16 cacheThEnAdjustable:1;   /* Set to 0 to indicate that the */
                                                /* default cacheThEn state bit   */
                                                /* set after init should NOT be  */
                                                /* modified (typically due to hw */
                                                /* bug or not supported feature).*/
         SCSI_UEXACT16 chipReset:1;             /* chiprst bit in HCNTRL reg.    */
         SCSI_UEXACT16 autoFlushHWProblem:1;    /* auto flush hw bug in bayonet  */
                                                /* rev 0                         */
         SCSI_UEXACT16 separateRWThreshold:1;   /* hardware supports separate    */
                                                /* FIFO thresholds for read and  */
                                                /* write.                        */
         SCSI_UEXACT16 separateRWThresholdEnable:1; /* separate read/write       */
                                                /* threshold enable              */
         SCSI_UEXACT16 initiatorMode:1;         /* initiator Mode of operation   */
         SCSI_UEXACT16 targetMode:1;            /* target Mode of operation      */
         SCSI_UEXACT16 OverrideOSMNVRAMRoutines:1; /* override NVRAM routines    */
         SCSI_UEXACT16 dontSaveSeqInState:1;    /* dont save/restore the sequencer      */
                                                /* in/from SCSI_STATE. Note this is     */
                                                /* a "dontSave" to maintain consistency */
                                                /* with existing HWM layer interface    */
         SCSI_UEXACT16 clusterEnable:1;         /* cluster enable bit from NVRAM (NTC)  */
         SCSI_UEXACT16 updateTermLevel:1;       /* update termination level             */
         SCSI_UEXACT16 terminationLevel:1;      /* termination level                    */
#if SCSI_DOMAIN_VALIDATION
         SCSI_UEXACT16 dvInProgress:1;              /* reserved for future use              */
         SCSI_UEXACT16 reserved:3;              /* reserved for future use              */
#else /* SCSI_DOMAIN_VALIDATION */
         SCSI_UEXACT16 reserved:4;              /* reserved for future use              */
#endif /* SCSI_DOMAIN_VALIDATION */
      } bits;
   } flags2;
   SCSI_UEXACT8 busRelease;                     /* bus release                   */
   SCSI_UEXACT8 threshold;                      /* threshold                     */
   SCSI_UEXACT8 firmwareMode;                   /* firmware mode of operation    */
   SCSI_UEXACT8 hardwareMode;                   /* hardware mode of operation    */
   SCSI_UEXACT8 OEMHardware;                    /* OEM hardware support flag     */
   SCSI_UEXACT8 reserved0[2];                   /* reserved for more flags       */
   SCSI_UEXACT8 indexWithinGroup;               /* index within group            */
#if SCSI_RESOURCE_MANAGEMENT
   SCSI_UEXACT8 maxNonTagScbs;                  /* maximum non tagged scbs       */
   SCSI_UEXACT8 maxTagScbs;                     /* maximum tagged scbs           */
   SCSI_UEXACT8 reserved1[2];                   /* alignment              */
#endif
#if SCSI_GROUP_DEVICES
   SCSI_HHCB SCSI_HPTR groupHead;               /* head hhcb of group     */
   SCSI_HHCB SCSI_HPTR groupNext;               /* next hhcb in group     */
#endif
   SCSI_UEXACT8 maxDevices;                     /* max number of devices allowed */
   SCSI_UEXACT8 maxLU;                          /* max lun allowed               */
   SCSI_UEXACT8 hostScsiID;                     /* host scsi id                  */
   SCSI_UEXACT8 numberHWScbs;                   /* number of hw scbs present     */

   SCSI_UEXACT32 resetDelay;                    /* number of milliseconds to     */
                                                /* delay after a SCSI reset      */
#if !SCSI_DOWNSHIFT_MODE
   SCSI_DEVICE deviceTable[SCSI_MAXDEV];        /* device configuration table    */
   SCSI_HHCB_RESERVE hhcbReserve;               /* reserved for internal usage   */
#else 
   SCSI_DEVICE SCSI_DPTR deviceTable[SCSI_MAXDEV]; /* device configuration table    */
#endif
   void SCSI_HPTR osdHandle;                    /* handle for OSD's convenience    */
   union
   {
      SCSI_UEXACT16 u16;
#if OSM_CPU_LITTLE_ENDIAN
      struct 
      {
         SCSI_UEXACT16 typeField:4;             /* type field                    */
         SCSI_UEXACT16 maxScsiRate:2;           /* max SCSI transfer rate        */
         SCSI_UEXACT16 autoTerm:1;              /* auto termination enable       */
         SCSI_UEXACT16 legacyConnector:1;       /* legacy connector present      */
         SCSI_UEXACT16 seepromType:2;           /* SEEPROM type                  */
         SCSI_UEXACT16 scsiChannels:2;          /* number of SCSI channels       */
         SCSI_UEXACT16 multifunction:1;         /* multi-function enable         */
         SCSI_UEXACT16 scsiBusWidth:1;          /* width of SCSI bus             */
         SCSI_UEXACT16 pciBusWidth:1;           /* PCI address width             */
         SCSI_UEXACT16 seDiff:1;                /* differential support          */
      } bits;
#else
      struct 
      {
         SCSI_UEXACT16 seDiff:1;                /* differential support          */
         SCSI_UEXACT16 pciBusWidth:1;           /* PCI address width             */
         SCSI_UEXACT16 scsiBusWidth:1;          /* width of SCSI bus             */
         SCSI_UEXACT16 multifunction:1;         /* multi-function enable         */
         SCSI_UEXACT16 scsiChannels:2;          /* number of SCSI channels       */
         SCSI_UEXACT16 seepromType:2;           /* SEEPROM type                  */
         SCSI_UEXACT16 legacyConnector:1;       /* legacy connector present      */
         SCSI_UEXACT16 autoTerm:1;              /* auto termination enable       */
         SCSI_UEXACT16 maxScsiRate:2;           /* max SCSI transfer rate        */
         SCSI_UEXACT16 typeField:4;             /* type field                    */
      } bits;
#endif /*OSM_CPU_LITTLE_ENDIAN */
   } SubSystemIDValues;
#if SCSI_TARGET_OPERATION
   SCSI_HHCB_TARGET_MODE targetMode;            /* Specific for Target Mode      */
#endif /* SCSI_TARGET_OPERATION */ 
   SCSI_UEXACT32 numberSCBAborts;               /* number of SCB Aborts     */

};

#if !SCSI_DOWNSHIFT_MODE
#define  SCSI_DEVICE_TABLE(hhcb)   (hhcb)->deviceTable
#else
#define  SCSI_DEVICE_TABLE(hhcb)   (*(hhcb)->deviceTable)
#endif

/* definitions for firmware mode */
#define  SCSI_FMODE_STANDARD64  0           /* standard 64 bytes scb mode    */
#define  SCSI_FMODE_STANDARD32  1           /* standard 32 bytes scb mode    */
#define  SCSI_FMODE_SWAPPING64  2           /* standard 64 bytes scb mode    */
#define  SCSI_FMODE_SWAPPING32  3           /* swapping 32 bytes scb mode    */
#define  SCSI_FMODE_STANDARD_ADVANCED  4    /* standard advanced scb mode    */
#define  SCSI_FMODE_SWAPPING_ADVANCED  5    /* swapping advanced scb mode    */
#define  SCSI_FMODE_STANDARD_160M      6    /* standard ultra 160m scb mode  */
#define  SCSI_FMODE_SWAPPING_160M      7    /* swapping ultra 160m scb mode  */
#define  SCSI_FMODE_DOWNSHIFT   8           /* down shift mode (bios)        */
#define  SCSI_FMODE_NOT_ASSIGNED 9          /* no firmware mode assigned     */

/* definitions for hardware mode */
#define  SCSI_HMODE_AIC78XX      0           /* aic-78xx hardware             */
#define  SCSI_HMODE_AICBAYONET   1           /* bayonet hardware              */
#define  SCSI_HMODE_AICTRIDENT   2           /* trident hardware              */

/* definitions for OEM customized hardware support flag */
#define  SCSI_OEMHARDWARE_NONE   0           /* Not applicable                */
#define  SCSI_OEMHARDWARE_OEM1   1           /* OEM hardware support          */

#define  SCSI_NULL_HHCB         ((SCSI_HHCB SCSI_HPTR) -1)

#define  SCSI_HF_scsiParity            flags.bits.scsiParity
#define  SCSI_HF_resetSCSI             flags.bits.resetSCSI
#define  SCSI_HF_multiTaskLun          flags.bits.multiTaskLun
#define  SCSI_HF_terminationLow        flags.bits.terminationLow
#define  SCSI_HF_terminationHigh       flags.bits.terminationHigh
#define  SCSI_HF_autoTermination       flags.bits.autoTermination
#define  SCSI_HF_secondaryTermLow      flags.bits.secondaryTermLow
#define  SCSI_HF_secondaryTermHigh     flags.bits.secondaryTermHigh
#define  SCSI_HF_secondaryAutoTerm     flags.bits.secondaryAutoTerm
#define  SCSI_HF_scamLevel             flags.bits.scamLevel
#define  SCSI_HF_skipScam              flags.bits.skipScam
#define  SCSI_HF_selTimeout            flags.bits.selTimeout
#define  SCSI_HF_expSupport            flags.bits.expSupport
#define  SCSI_HF_cacheThEn             flags.bits.cacheThEn

#define  SCSI_HF_cacheThEnAdjustable   flags2.bits.cacheThEnAdjustable
#define  SCSI_HF_chipReset             flags2.bits.chipReset
#define  SCSI_HF_autoFlushHWProblem    flags2.bits.autoFlushHWProblem
#define  SCSI_HF_separateRWThreshold   flags2.bits.separateRWThreshold
#define  SCSI_HF_separateRWThresholdEnable flags2.bits.separateRWThresholdEnable
#define  SCSI_HF_OverrideOSMNVRAMRoutines  flags2.bits.OverrideOSMNVRAMRoutines
#define  SCSI_HF_dontSaveSeqInState    flags2.bits.dontSaveSeqInState
#define  SCSI_HF_clusterEnable         flags2.bits.clusterEnable
#define  SCSI_HF_targetMode            flags2.bits.targetMode
#define  SCSI_HF_initiatorMode         flags2.bits.initiatorMode
#define  SCSI_HF_updateTermLevel       flags2.bits.updateTermLevel
#define  SCSI_HF_terminationLevel      flags2.bits.terminationLevel
#if SCSI_DOMAIN_VALIDATION
#define  SCSI_HF_dvInProgress          flags2.bits.dvInProgress
#endif /* SCSI_DOMAIN_VALIDATION */

#define  SCSI_SUBID_typeField          SubSystemIDValues.bits.typeField
#define  SCSI_SUBID_maxScsiRate        SubSystemIDValues.bits.maxScsiRate
#define  SCSI_SUBID_seepromType        SubSystemIDValues.bits.seepromType
#define  SCSI_SUBID_scsiChannels       SubSystemIDValues.bits.scsiChannels
#define  SCSI_SUBID_autoTerm           SubSystemIDValues.bits.autoTerm
#define  SCSI_SUBID_multifunction      SubSystemIDValues.bits.multifunction
#define  SCSI_SUBID_scsiBusWidth       SubSystemIDValues.bits.scsiBusWidth
#define  SCSI_SUBID_legacyConnector    SubSystemIDValues.bits.legacyConnector
#define  SCSI_SUBID_pciBusWidth        SubSystemIDValues.bits.pciBusWidth
#define  SCSI_SUBID_seDiff             SubSystemIDValues.bits.seDiff

/***************************************************************************
* HIOB structures definitions
***************************************************************************/
struct SCSI_HIOB_
{
   SCSI_UNIT_HANDLE unitHandle;        /* device task set handle        */
   SCSI_UEXACT8 cmd;                   /* command code                  */
   SCSI_UEXACT8 stat;                  /* state                         */
   SCSI_UEXACT8 haStat;                /* host adapter status           */
   SCSI_UEXACT8 trgStatus;             /* target status                 */
   SCSI_UEXACT8 residualSense;         /* length of valid sense data ret*/
   SCSI_UEXACT8 scbNumber;             /* scb number assigned           */
   union
   {
      SCSI_UEXACT16 u16;
      struct
      {
         SCSI_UEXACT16 noUnderrun:1;
         SCSI_UEXACT16 tagEnable:1;
         SCSI_UEXACT16 disallowDisconnect:1;
         SCSI_UEXACT16 autoSense:1;  
         SCSI_UEXACT16 freezeOnError:1;
#if SCSI_TARGET_OPERATION  
         SCSI_UEXACT16 taskManagementResponse:1;
#endif /* SCSI_TARGET_OPERATION */
#if SCSI_DOWNSHIFT_MODE
         SCSI_UEXACT16 overlaySns:1;
#endif
#if SCSI_NEGOTIATION_PER_IOB
         SCSI_UEXACT16 forceSync:1;
         SCSI_UEXACT16 forceAsync:1;
         SCSI_UEXACT16 forceWide:1;
         SCSI_UEXACT16 forceNarrow:1;
         SCSI_UEXACT16 forceReqSenseNego:1;
#endif
#if SCSI_PARITY_PER_IOB
         SCSI_UEXACT16 parityEnable:1;
#endif
#if SCSI_PAC_NSX_REPEATER || SCSI_NSX_REPEATER
         SCSI_UEXACT16 nsxCommunication:1;
#endif /* SCSI_PAC_NSX_REPEATER || SCSI_NSX_REPEATER */
#if SCSI_DOMAIN_VALIDATION_BASIC+SCSI_DOMAIN_VALIDATION_ENHANCED
         SCSI_UEXACT16 dvIOB:1;
#endif /* SCSI_DOMAIN_VALIDATION_BASIC+SCSI_DOMAIN_VALIDATION_ENHANCED */
      } bits;
   } flags;
   void SCSI_IPTR snsBuffer;              /* sense buffer                  */
   SCSI_UEXACT8 snsLength;                /* sense length/residual         */
   SCSI_UEXACT8 snsResidual;              /* sense residual                */
#if SCSI_TARGET_OPERATION  
/* Protect by compile option so that size of initiator mode HIOB is not    */
/* increased unnecessarily.                                                */  
/* Target Mode fields */ 
   SCSI_UEXACT8 initiatorStatus;          /* scsi status for initiator     */
                                          /* reestablish and complete      */
                                          /* target mode I/O.              */
   SCSI_UEXACT8 reserved2[3];             /* reserved for alignment        */
   SCSI_UEXACT16 snsBufferSize;           /* size of the snsBuffer, which  */
                                          /* is used to store the command  */
                                          /* for target mode establish     */
                                          /* connection Iobs.              */  
   SCSI_UEXACT32 ioLength;                /* length of the I/O request.    */
                                          /* For target mode this is the   */
                                          /* length of the I/O for this    */ 
                                          /* SCB only.                     */ 
   SCSI_UEXACT32 relativeOffset;          /* offset (in bytes) from the    */
                                          /* start of the client buffer    */ 
#else                                            
   SCSI_UEXACT8 reserved2[2];             /* reserved for alignment        */
#endif /* SCSI_TARGET_OPERATION */   
   SCSI_UEXACT32 residualLength;          /* residual length               */
   SCSI_SCB_DESCRIPTOR SCSI_HPTR scbDescriptor; /* scb descriptor          */
#if SCSI_GROUP_DEVICES
   SCSI_HIOB SCSI_IPTR groupHead;         /* pointer to head of group      */
   SCSI_HIOB SCSI_IPTR groupNext;         /* pointer to next in group      */
#endif
#if SCSI_RESOURCE_MANAGEMENT
   SCSI_HIOB SCSI_IPTR queueNext;         /* pointer to next in queue      */
#endif
   SCSI_HIOB_RESERVE hiobReserve;         /* reserved for internal usage   */
   SCSI_UINT8 priority;
};

/* definitions for flags */
#define  SCSI_IF_noUnderrun         flags.bits.noUnderrun  
#define  SCSI_IF_tagEnable          flags.bits.tagEnable   
#define  SCSI_IF_disallowDisconnect flags.bits.disallowDisconnect
#define  SCSI_IF_overlaySns         flags.bits.overlaySns  
#define  SCSI_IF_autoSense          flags.bits.autoSense   
#define  SCSI_IF_freezeOnError      flags.bits.freezeOnError
#if SCSI_TARGET_OPERATION
/* This flag is used to indicate if the SCSI_CMD_REESTABLISH_AND_COMPLETE HIOB
   is a response to a task management function. Responses to TMF's do NOT
   result in an interrupt (no SCB used - the scsi bus is simply released)
   therefore this flag indcates that the IOB post routine must be called
   receipt of the HIOB from the HWM. */
#define  SCSI_IF_taskManagementResponse flags.bits.taskManagementResponse
#endif /* SCSI_TARGET_OPERATION */
#if SCSI_NEGOTIATION_PER_IOB
#define  SCSI_IF_forceSync          flags.bits.forceSync   
#define  SCSI_IF_forceAsync         flags.bits.forceAsync   
#define  SCSI_IF_forceWide          flags.bits.forceWide
#define  SCSI_IF_forceNarrow        flags.bits.forceNarrow
#define  SCSI_IF_forceReqSenseNego  flags.bits.forceReqSenseNego
#endif /* SCSI_NEGOTIATION_PER_IOB */
#if SCSI_PARITY_PER_IOB
#define  SCSI_IF_parityEnable       flags.bits.parityEnable
#endif
#if SCSI_PAC_NSX_REPEATER || SCSI_NSX_REPEATER
#define  SCSI_IF_nsxCommunication   flags.bits.nsxCommunication
#endif /* SCSI_PAC_NSX_REPEATER || SCSI_NSX_REPEATER */
#if SCSI_DOMAIN_VALIDATION_BASIC+SCSI_DOMAIN_VALIDATION_ENHANCED
#define  SCSI_IF_dvIOB              flags.bits.dvIOB
#endif /* SCSI_DOMAIN_VALIDATION_BASIC+SCSI_DOMAIN_VALIDATION_ENHANCED */
#if SCSI_DMA_SUPPORT
#define  SCSI_IF_flags              flags.u16    /* all the flag bits */
#endif /* SCSI_DMA_SUPPORT */


#define  SCSI_TARGET_UNIT(hiob)     (hiob)->unitHandle.targetUnit
#define  SCSI_INITIATOR_UNIT(hiob)  (hiob)->unitHandle.initiatorUnit
#define  SCSI_RELATED_HIOB(hiob)    (hiob)->unitHandle.relatedHiob
#if SCSI_TARGET_OPERATION
#define  SCSI_NEXUS_UNIT(hiob)      (hiob)->unitHandle.nexus
#endif /* SCSI_TARGET_OPERATION */

/* use snsBuffer to store the active HIOB for special Bus Reset request */
#define  SCSI_ACTIVE_HIOB(hiob)     (hiob)->snsBuffer
#if  SCSI_TARGET_OPERATION 
#define  SCSI_CMD_BUFFER(hiob)      ((hiob)->snsBuffer)
#define  SCSI_CMD_LENGTH(hiob)      ((hiob)->snsLength)
#define  SCSI_CMD_SIZE(hiob)        ((hiob)->snsBufferSize)
#define  SCSI_DATA_IOLENGTH(hiob)   ((hiob)->ioLength)
#define  SCSI_DATA_OFFSET(hiob)     ((hiob)->relativeOffset)
#endif /* SCSI_TARGET_OPERATION */

#define  SCSI_NULL_HIOB             ((SCSI_HIOB SCSI_IPTR) -1)

/****************************************************************************
* definitions for cmd of HIOB 
****************************************************************************/
#define  SCSI_CMD_INITIATE_TASK     0  /* Standard SCSI command               */
#define  SCSI_CMD_ABORT_TASK        1  /* abort task                          */
#define  SCSI_CMD_RESET_BUS         2  /* reset protocol channel/bus          */
#define  SCSI_CMD_RESET_TARGET      3  /* reset target device                 */
#define  SCSI_CMD_RESET_HARDWARE    4  /* reset hardware                      */
#define  SCSI_CMD_PROTO_AUTO_CFG    6  /* protocol auto config                */
#define  SCSI_CMD_ABORT_TASK_SET    7  /* abort task set                      */
#define  SCSI_CMD_RESET_BOARD       8  /* reset board                         */
#define  SCSI_CMD_UNFREEZE_QUEUE    9  /* unfreeze device queue               */
#if SCSI_TARGET_OPERATION
#define  SCSI_CMD_ESTABLISH_CONNECTION 10      /* establish connection iob    */
#define  SCSI_CMD_REESTABLISH_INTERMEDIATE 11  /* reestablish intermediate    */
#define  SCSI_CMD_REESTABLISH_AND_COMPLETE 12  /* reestablish and complete    */
#define  SCSI_CMD_ABORT_NEXUS      13  /* abort nexus                         */  
#if SCSI_MULTIPLEID_SUPPORT
#define  SCSI_CMD_ENABLE_ID        14  /* enable target id(s)                 */        
#define  SCSI_CMD_DISABLE_ID       15  /* disable target id(s)                */
#endif /* SCSI_MULTIPLEID_SUPPORT */
#endif /* SCSI_TARGET_OPERATION */
#if SCSI_DMA_SUPPORT
#define  SCSI_CMD_INITIATE_DMA_TASK 16 /* special DMA task                   */
#endif /* SCSI_DMA_SUPPORT */

/****************************************************************************
* definitions for stat of HIOB
****************************************************************************/
#define  SCSI_SCB_PENDING           0  /* SCSI request in progress            */
#define  SCSI_SCB_COMP              1  /* SCSI request completed no error     */
#define  SCSI_SCB_ABORTED           2  /* SCSI request aborted                */
#define  SCSI_SCB_ERR               3  /* SCSI request completed with error   */
#define  SCSI_SCB_INV_CMD           4  /* Invalid SCSI request                */
#define  SCSI_SCB_FROZEN            5  /* SCSI request pulled out from Queue  */
#if SCSI_TARGET_OPERATION
#define  SCSI_TASK_CMD_COMP         6  /* Task Command Complete               */
#endif /* SCSI_TARGET_OPERATION */

/***************************************************************************
* definitions for haStat of HIOB
****************************************************************************/
#define  SCSI_HOST_NO_STATUS    0x00     /* No adapter status available         */
#define  SCSI_HOST_ABT_HOST     0x04     /* Command aborted by host             */
#define  SCSI_HOST_ABT_HA       0x05     /* Command aborted by host adapter     */
#define  SCSI_HOST_ABT_BUS_RST  0x06     /* Command aborted by bus reset        */
#define  SCSI_HOST_ABT_3RD_RST  0x07     /* Command aborted by 3rd party reset  */
#define  SCSI_HOST_ABT_TRG_RST  0x08     /* Command aborted by target reset     */
#define  SCSI_HOST_ABT_IOERR    0x09     /* Command aborted by io error         */
#define  SCSI_HOST_SEL_TO       0x11     /* Selection timeout                   */
#define  SCSI_HOST_DU_DO        0x12     /* Data overrun/underrun error         */
#define  SCSI_HOST_BUS_FREE     0x13     /* Unexpected bus free                 */
#define  SCSI_HOST_PHASE_ERR    0x14     /* Target bus phase sequence error     */
#define  SCSI_HOST_INV_LINK     0x17     /* Invalid SCSI linking operation      */
#define  SCSI_HOST_SNS_FAIL     0x1b     /* Auto-request sense failed           */
#define  SCSI_HOST_TAG_REJ      0x1c     /* Tagged Queuing rejected by target   */
#define  SCSI_HOST_HW_ERROR     0x20     /* Host adpater hardware error         */
#define  SCSI_HOST_ABT_FAIL     0x21     /* Target did'nt respond to ATN (RESET)*/
#define  SCSI_HOST_RST_HA       0x22     /* SCSI bus reset by host adapter      */
#define  SCSI_HOST_RST_OTHER    0x23     /* SCSI bus reset by other device      */
#define  SCSI_HOST_ABT_NOT_FND  0x24     /* Command to be aborted not found     */
#define  SCSI_HOST_ABT_STARTED  0x25     /* Abort is in progress                */
#define  SCSI_HOST_ABT_CMDDONE  0x26     /* Command to be aborted has already   */
                                         /* completed on the I/O channel        */
#define  SCSI_HOST_NOAVL_INDEX  0x30     /* SCSI bus reset by other device      */
#define  SCSI_HOST_DETECTED_ERR 0x48     /* Initiator detected error (parity)   */

#if SCSI_TARGET_OPERATION
#define  SCSI_INITIATOR_PARITY_MSG  0x49 /* Initiator message parity message    */ 
#define  SCSI_INITIATOR_PARITY_ERR_MSG  0x4a  /* Initiator detected parity message     */ 
#define  SCSI_INITIATOR_INVALID_MSG 0x4b    /* Invalid message recieved from initiator */
#define  SCSI_HOST_MSG_REJECT       0x50    /* Initiator message reject         */ 
#define  SCSI_HOST_IDENT_RSVD_BITS  0x51    /* Reserved bits set in Identify    */
                                            /* message - msg reject issued      */
#define  SCSI_HOST_TRUNC_CMD        0x52    /* Truncated SCSI Command           */
#define  SCSI_HOST_ABORT_TASK       0x53    /* Abort Task message recieved      */
#define  SCSI_HOST_TERMINATE_TASK   0x54    /* Terminate I/O  message recieved  */
#define  SCSI_HOST_ABORT_TASK_SET   0x55    /* Abort Task Set message recieved  */
#define  SCSI_HOST_LUN_RESET        0x56    /* Reset Port message recieved      */
#define  SCSI_HOST_CLEAR_ACA        0x57    /* Clear ACA message recieved       */ 
#define  SCSI_HOST_TARGET_RESET     0x58    /* Reset Target message recieved    */
#define  SCSI_HOST_CLEAR_TASK_SET   0x59    /* Clear Task Set message recieved  */
#define  SCSI_HOST_IDENT_LUNTAR_BIT 0x60    /* Luntar bit set in identify       */
                                            /* message - msg reject issued      */ 
#endif /* SCSI_TARGET_OPERATION */

#define  SCSI_HOST_ABT_CHANNEL_FAILED  0x61 /* Aborted due to unrecoverable SCSI bus failure */

/****************************************************************************
* Definitions for target status
****************************************************************************/
#define  SCSI_UNIT_GOOD      0x00        /* Good status or none available       */
#define  SCSI_UNIT_CHECK     0x02        /* Check Condition                     */
#define  SCSI_UNIT_MET       0x04        /* Condition met                       */
#define  SCSI_UNIT_BUSY      0x08        /* Target busy                         */
#define  SCSI_UNIT_INTERMED  0x10        /* Intermediate command good           */
#define  SCSI_UNIT_INTMED_GD 0x14        /* Intermediate condition met          */
#define  SCSI_UNIT_RESERV    0x18        /* Reservation conflict                */
#define  SCSI_UNIT_QUEFULL   0x28        /* Queue Full                          */

#if SCSI_TARGET_OPERATION
/****************************************************************************
* Definitions for SCSI messages
****************************************************************************/
#define  SCSI_SIMPLE_QUEUE_TAG    0x20   /* Simple Task Queuing Message         */
#define  SCSI_HEAD_OF_QUEUE_TAG   0x21   /* Head Of Queue Task Queuing Message  */
#define  SCSI_ORDERED_QUEUE_TAG   0x22   /* Ordered Task Queuing Message        */
#define  SCSI_ACA_QUEUE_TAG       0x24   /* ACA Task Queuing Message            */
#define  SCSI_QUEUE_TAG_MASK      0x0F   /* Mask for scontrol                   */

/****************************************************************************
* Definitions for SCSI CDB Fields and masks
****************************************************************************/
#define  SCSI_CDB_LUNFIELD_MASK   0xE0   /* Mask for byte 1 Lun field  */ 

#endif /* SCSI_TARGET_OPERATION */

/****************************************************************************
* SCSI_HW_INFORMATION structure
****************************************************************************/
struct SCSI_HW_INFORMATION_
{
   SCSI_UEXACT8 WWID[SCSI_SIZEWWID];         /* World Wide ID                 */
   SCSI_UEXACT8 hostScsiID;                  /* Host adapter SCSI ID          */
   SCSI_UEXACT8 threshold;                   /* Host adapter DFIFO threshold  */
   SCSI_UEXACT8 writeThreshold;              /* FIFO write data threshold     */
   SCSI_UEXACT8 readThreshold;               /* FIFO read data threshold      */
   union
   {
      SCSI_UEXACT16 u16;
      struct 
      {
         SCSI_UEXACT16 wideSupport:1;          /* Adapter supports wide xfers    */
         SCSI_UEXACT16 scsiParity:1;           /* SCSI parity error              */
         SCSI_UEXACT16 selTimeout:2;           /* Selection timeout              */
         SCSI_UEXACT16 ultraCapable:1;         /* Ultra Capable adapter          */
         SCSI_UEXACT16 transceiverMode:2;      /* Transceiver mode               */
         SCSI_UEXACT16 separateRWThresholdEnable:1; /* Separate threshold enable */
         SCSI_UEXACT16 separateRWThreshold:1;  /* Adapter supports separate FIFO */
                                               /* thresholds                     */ 
         SCSI_UEXACT16 cacheThEn:1;            /* Current state of CACHETEN bit  */
         SCSI_UEXACT16 cacheThEnAdjustable:1;  /* Indicate CACHETHEN adjustment  */
                                               /* is supported properly by hw    */
      } bits;
   } flags;
   struct
   {
      SCSI_UINT16 maxTagScbs;                /* Max tagged scbs per target    */
      SCSI_UEXACT16 xferOption;              /* Transfer Option for the target*/
      union
      {
         SCSI_UEXACT16 u16;
         struct 
         {
            SCSI_UEXACT16 ultraEnable:1;     /* Ultra enabled for the target  */
            SCSI_UEXACT16 scsiOptChanged:1;  /* Scsi option changed using adjust profile */
            SCSI_UEXACT16 dtcEnable:1;       /* DTC with CRC xfer enable */
         } bits;
      } flags;
      SCSI_UEXACT16 SCSIMaxSpeed;            /* Maximum speed for profile  */
      SCSI_UEXACT16 SCSICurrentSpeed;        /* Current speed for profile  */
      SCSI_UEXACT16 SCSIDefaultSpeed;        /* Default speed for profile  */
      SCSI_UEXACT8 SCSIMaxOffset;            /* Maximum offset for profile */
      SCSI_UEXACT8 SCSICurrentOffset;        /* Current offset for profile */
      SCSI_UEXACT8 SCSIDefaultOffset;        /* Default offset for profile */
      SCSI_UEXACT8 SCSIMaxWidth;             /* Maximum width for profile  */
      SCSI_UEXACT8 SCSICurrentWidth;         /* Current width for profile  */
      SCSI_UEXACT8 SCSIDefaultWidth;         /* Default width for profile  */
      SCSI_UEXACT8 SCSICurrentProtocolOption;/* Current Protocol for profile */
      SCSI_UEXACT8 SCSIDefaultProtocolOption;/* Default Protocol for profile */
   } targetInfo[SCSI_MAXDEV];   
   SCSI_UEXACT16 minimumSyncSpeed;           /* Minimum speed for sync xfer */
   SCSI_UEXACT8 intrThresholdSupport;        /* intr threshold is supported or not  */
   SCSI_UEXACT8 intrThreshold;               /* threshold for intr posting  */
#if SCSI_TARGET_OPERATION
   SCSI_UEXACT16 targetIDMask;               /* Mask of SCSI IDs to which this */
                                             /* adapter will respond as a      */
                                             /* target.                        */
   SCSI_UEXACT16 alignment1;                 /* padded for alignment           */
#endif /* SCSI_TARGET_OPERATION */
};

#define  SCSI_PF_wideSupport                 flags.bits.wideSupport
#define  SCSI_PF_scsiParity                  flags.bits.scsiParity
#define  SCSI_PF_selTimeout                  flags.bits.selTimeout
#define  SCSI_PF_ultraCapable                flags.bits.ultraCapable
#define  SCSI_PF_transceiverMode             flags.bits.transceiverMode 
#define  SCSI_PF_separateRWThresholdEnable   flags.bits.separateRWThresholdEnable
#define  SCSI_PF_separateRWThreshold         flags.bits.separateRWThreshold
#define  SCSI_PF_cacheThEn                   flags.bits.cacheThEn
#define  SCSI_PF_cacheThEnAdjustable         flags.bits.cacheThEnAdjustable

#define  SCSI_TF_ultraEnable                 flags.bits.ultraEnable
#define  SCSI_TF_scsiOptChanged              flags.bits.scsiOptChanged
#define  SCSI_TF_dtcEnable                   flags.bits.dtcEnable

/****************************************************************************
* Definitions for transceiverMode
****************************************************************************/
#define  SCSI_UNKNOWN_MODE       0           /* Mode unknown              */
#define  SCSI_LVD_MODE           1           /* Low Voltage Differential  */
#define  SCSI_SE_MODE            2           /* Single Ended              */
#define  SCSI_HVD_MODE           3           /* High Voltage Differential */

/****************************************************************************
* Definitions for Protocol Option
****************************************************************************/
#define SCSI_PROTOCOL_OPTION_UNKNOWN  0
#define SCSI_NO_PROTOCOL_OPTION       1
#define SCSI_ST_DATA                  2
#define SCSI_DT_DATA_WITH_CRC         3

/****************************************************************************
* Definitions for SCAM operation structures
****************************************************************************/
#if SCSI_SCAM_ENABLE >= 1 
#define  SCSI_SCAM_DEVICE_MASK   0xffff0000
#define  SCSI_SCAM_STATUS_MASK   0x0000ffff
#define  SCSI_SCAM_FAILURE       0xff
#endif

/****************************************************************************
* Definitions for Asynchronous Event
****************************************************************************/
#define  SCSI_AE_3PTY_RESET  1           /* 3rd party SCSI bus reset         */
#define  SCSI_AE_HAID_CHANGE 2           /* Host ID change due to SCAM       */
#define  SCSI_AE_HA_RESET    3           /* Host Adapter SCSI bus reset      */
#define  SCSI_AE_SELECTED    4           /* Host Adapter selected as target  */
#define  SCSI_AE_SCAM_SELD   5           /* SCAM selection detected          */
#define  SCSI_AE_OSMFREEZE   6           /* OSM Freeze                       */
#define  SCSI_AE_OSMUNFREEZE 7           /* OSM Unfreeze                     */
#define  SCSI_AE_IOERROR     8           /* IO Error                         */
#if SCSI_CHECK_PCI_ERROR
#define  SCSI_AE_PCI_RTA     9           /* receive PCI target adort error   */
#define  SCSI_AE_PCI_ERROR   10          /* receive other PCI errors         */
#endif
#if SCSI_TARGET_OPERATION   
/* Target Mode Event defines */
#define  SCSI_AE_NEXUS_TSH_THRESHOLD 11  /* Target mode event. NexusTsh      */    
                                         /* threshold exceeded.              */  
#define  SCSI_AE_EC_IOB_THRESHOLD    12  /* Target mode event. Establish     */
                                         /* connection threshold exceeded.   */ 
#endif /* SCSI_TARGET_OPERATION */
#define  SCSI_AE_IO_CHANNEL_FAILED   13  /* Unrecoverable SCSI bus failure   */

#if SCSI_PAC_NSX_REPEATER || SCSI_NSX_REPEATER
/****************************************************************************
* Definitions for NSX status
****************************************************************************/
#define  SCSI_NSX_GOOD            0x00   /* Good status                      */
#define  SCSI_NSX_OVERRUN         0x01   /* More data was received than      */
                                         /* could be stored in nsx buffer    */
#define  SCSI_NSX_PROTOCOL_ERROR  0x02   /* Some protocol error occurred     */ 
#endif /* SCSI_PAC_NSX_REPEATER || SCSI_NSX_REPEATER */

#if SCSI_TARGET_OPERATION
/***************************************************************************
* SCSI_NEXUS structures definitions
***************************************************************************/
struct SCSI_NEXUS_
{
   SCSI_HHCB SCSI_HPTR hhcb;                 /* host control block          */
   SCSI_NODE SCSI_NPTR node;                 /* node handle associated with */ 
                                             /* this nexus                  */ 
   SCSI_UEXACT8 scsiID;                      /* scsi id of initiator        */
   SCSI_UEXACT8 lunID;                       /* target lun id               */
   SCSI_UEXACT8 queueTag;                    /* queue tag                   */
   SCSI_UEXACT8 queueType;                   /* type of queuing             */
   union
   {
      SCSI_UEXACT8 u8;
      struct 
      {  
         SCSI_UEXACT8 lunTar:1;              /* value of LunTar bit in      */
                                             /* identify message received   */
         SCSI_UEXACT8 disconnectAllowed:1;   /* value of discPriv bit in    */
                                             /* identify message received   */
         SCSI_UEXACT8 busHeld:1;             /* if 1, the SCSI bus is held  */
         SCSI_UEXACT8 tagRequest:1;          /* if 1, this is a tagged      */
                                             /* request                     */
         SCSI_UEXACT8 lastResource:1;        /* if 1 the last resource for  */
                                             /* receiving new requests has  */
                                             /* been used.                  */
         SCSI_UEXACT8 scsi1Selection:1;      /* if 1, this is a SCSI-1      */
                                             /* selection. I.e. there was   */
                                             /* no identify message         */
                                             /* received with the request   */     
#if SCSI_RESOURCE_MANAGEMENT
         SCSI_UEXACT8 available:1;           /* nexus is in the available   */ 
                                             /* pool */
#endif /* SCSI_RESOURCE_MANAGEMENT */
      } bits;
   } nexusAttributes;
#if SCSI_MULTIPLEID_SUPPORT
   SCSI_UEXACT8 selectedID;                  /* the scsi id that we were    */ 
                                             /* selected as a target - used */
                                             /* when our adapter/chip is    */
                                             /* set up to respond to        */
                                             /* selection as multiple scsi  */
                                             /* ids.                        */     
   SCSI_UEXACT8 reserved1[2];                /* padded for alignment        */
#else
   SCSI_UEXACT8 reserved1[3];                /* padded for alignment        */
#endif /* SCSI_MULTIPLEID_SUPPORT */
   SCSI_NEXUS_RESERVE nexusReserve;
#if SCSI_RESOURCE_MANAGEMENT
   SCSI_NEXUS SCSI_XPTR nextNexus;           /* next nexus pointer          */  
#endif
};   

#define SCSI_XF_lunTar             nexusAttributes.bits.lunTar
#define SCSI_XF_disconnectAllowed  nexusAttributes.bits.disconnectAllowed
#define SCSI_XF_busHeld            nexusAttributes.bits.busHeld
#define SCSI_XF_tagRequest         nexusAttributes.bits.tagRequest
#define SCSI_XF_lastResource       nexusAttributes.bits.lastResource
#define SCSI_XF_available          nexusAttributes.bits.available
#define SCSI_XF_scsi1Selection     nexusAttributes.bits.scsi1Selection

#define  SCSI_NULL_NEXUS    ((SCSI_NEXUS SCSI_XPTR)-1)
#define  SCSI_GET_NODE(nexusHandle)   ((nexusHandle)->node)

/***************************************************************************
* SCSI_NODE structures definitions
***************************************************************************/
struct SCSI_NODE_
{
   SCSI_DEVICE SCSI_DPTR deviceTable;
   void SCSI_NPTR osmContext;           /* OSM set context        */ 
   SCSI_HHCB SCSI_HPTR hhcb;            /* host control block     */
   SCSI_UEXACT8 scsiID;                 /* node scsi id           */
   SCSI_UEXACT8 reserved0[3];           /* alignment              */   
   SCSI_NODE_RESERVE nodeReserve;   
#if SCSI_RESOURCE_MANAGEMENT
   SCSI_NODE SCSI_NPTR nextNode;        /* next link              */ 
#endif
 };

#define  SCSI_NULL_NODE               ((SCSI_NODE SCSI_NPTR) -1)
#define  SCSI_NODE_TABLE(hhcb)   (hhcb)->SCSI_HP_nodeTable;
 
#endif /* SCSI_TARGET_OPERATION */

/****************************************************************************
* Definitions for Asynchronous Event for command
****************************************************************************/
#define  SCSI_AE_FREEZEONERROR_START  1   /* H/W queue is frozen for target   */
#define  SCSI_AE_FREEZEONERROR_END    2   /* H/W queue is frozen for target   */

/****************************************************************************
* Definitions as generic SCSI HIM interface
****************************************************************************/
#if SCSI_RESOURCE_MANAGEMENT
#define  SCSI_GET_CONFIGURATION(hhcb)  SCSIRGetConfiguration((hhcb))
#define  SCSI_GET_MEMORY_TABLE(firmwareMode,numberScbs,numberNexusHandles,numberNodeHandles,memoryHandle) \
            SCSIRGetMemoryTable((firmwareMode),(numberScbs),(numberNexusHandles),(numberNodeHandles),(memoryHandle))
#define  SCSI_APPLY_MEMORY_TABLE(hhcb,memoryTable) SCSIRApplyMemoryTable((hhcb),(memoryTable))
#define  SCSI_INITIALIZE(hhcb)         SCSIRInitialize((hhcb))
#define  SCSI_SET_UNIT_HANDLE(hhcb,unitControl,scsiID,luNumber) SCSIRSetUnitHandle((hhcb),(unitControl),(scsiID),(luNumber))
#define  SCSI_FREE_UNIT_HANDLE(unitControl) SCSIRFreeUnitHandle((unitControl))
#define  SCSI_QUEUE_HIOB(hiob)         SCSIRQueueHIOB((hiob))
#define  SCSI_QUEUE_SPECIAL_HIOB(hiob) SCSIRQueueSpecialHIOB((hiob))
#define  SCSI_BACK_END_ISR(hhcb)       SCSIRBackEndISR((hhcb))
#define  SCSI_COMPLETE_HIOB(hiob)      SCSIrCompleteHIOB((hiob))
#define  SCSI_COMPLETE_SPECIAL_HIOB(hiob)       SCSIrCompleteSpecialHIOB((hiob))
#define  SCSI_ASYNC_EVENT(hhcb,event)  SCSIrAsyncEvent((hhcb),(event))
#define  SCSI_POWER_MANAGEMENT(hhcb,powerMode)  SCSIRPowerManagement((hhcb),(powerMode))
#define  SCSI_CHECK_HOST_IDLE(hhcb)    SCSIRCheckHostIdle((hhcb))
#define  SCSI_CHECK_DEVICE_IDLE(hhcb,targetID)  SCSIRCheckDeviceIdle((hhcb),(targetID))
#define  SCSI_GET_HW_INFORMATION(hwInformation,hhcb) SCSIRGetHWInformation((hwInformation),(hhcb))
#if !SCSI_DISABLE_ADJUST_TARGET_PROFILE
#define  SCSI_PUT_HW_INFORMATION(hwInformation,hhcb) SCSIRPutHWInformation((hwInformation),(hhcb))
#else
#define  SCSI_PUT_HW_INFORMATION(hwInformation,hhcb) SCSIHPutHWInformation((hwInformation),(hhcb))
#endif /* !SCSI_DISABLE_ADJUST_TARGET_PROFILE */
#define  SCSI_GET_FREE_HEAD(hhcb)      SCSIRGetFreeHead((hhcb))
#define  SCSI_ASYNC_EVENT_COMMAND(hhcb,hiob,event)  SCSIrAsyncEventCommand((hhcb),(hiob),(event))
#if SCSI_TARGET_OPERATION
#define  SCSI_GET_FREE_EST_HEAD(hhcb)  SCSIRGetFreeEstHead((hhcb))
#define  SCSI_CLEAR_NEXUS(nexus,hhcb)  SCSIrClearNexus((nexus),(hhcb))
#define  SCSI_ALLOCATE_NODE(hhcb)      SCSIrAllocateNode((hhcb))
#endif /* SCSI_TARGET_OPERATION */     
#else
#define  SCSI_GET_CONFIGURATION(hhcb)  SCSIHGetConfiguration((hhcb))
#define  SCSI_GET_MEMORY_TABLE(firmwareMode,numberScbs,memoryTable)   SCSIHGetMemoryTable((firmwareMode),(numberScbs),(memoryTable))
#define  SCSI_APPLY_MEMORY_TABLE(hhcb,memoryTable) SCSIHApplyMemoryTable((hhcb),(memoryTable))
#define  SCSI_INITIALIZE(hhcb)         SCSIHInitialize((hhcb))
#define  SCSI_SET_UNIT_HANDLE(hhcb,unitControl,scsiID,luNumber) SCSIHSetUnitHandle((hhcb),(unitControl),(scsiID),(luNumber))
#define  SCSI_FREE_UNIT_HANDLE(unitControl)     SCSIHFreeUnitHandle((unitControl))
#define  SCSI_QUEUE_HIOB(hiob)         SCSIHQueueHIOB((hiob))
#define  SCSI_QUEUE_SPECIAL_HIOB(hiob) SCSIHQueueSpecialHIOB((hiob))
#define  SCSI_BACK_END_ISR(hhcb)       SCSIHBackEndISR((hhcb))
#define  SCSI_COMPLETE_HIOB(hiob)      OSD_COMPLETE_HIOB((hiob))
#define  SCSI_COMPLETE_SPECIAL_HIOB(hiob)       OSD_COMPLETE_SPECIAL_HIOB((hiob))
#define  SCSI_ASYNC_EVENT(hhcb,event)  OSD_ASYNC_EVENT((hhcb),(event))
#define  SCSI_POWER_MANAGEMENT(hhcb,powerMode)  SCSIHPowerManagement((hhcb),(powerMode))
#define  SCSI_CHECK_HOST_IDLE(hhcb)    SCSIHCheckHostIdle((hhcb))
#define  SCSI_CHECK_DEVICE_IDLE(hhcb,targetID)  SCSIHCheckDeviceIdle((hhcb),(targetID))
#define  SCSI_GET_HW_INFORMATION(hwInformation,hhcb) SCSIHGetHWInformation((hwInformation),(hhcb))
#define  SCSI_PUT_HW_INFORMATION(hwInformation,hhcb) SCSIHPutHWInformation((hwInformation),(hhcb))
#define  SCSI_GET_FREE_HEAD(hhcb)      OSD_GET_FREE_HEAD((hhcb))
#if SCSI_TARGET_OPERATION
#define  SCSI_GET_FREE_EST_HEAD(hhcb)  OSD_GET_FREE_EST_HEAD((hhcb))
#define  SCSI_CLEAR_NEXUS(nexus,hhcb)  SCSIHTargetClearNexus((nexus),(hhcb))
#define  SCSI_ALLOCATE_NODE(hhcb)      OSD_ALLOCATE_NODE((hhcb))
#endif /* SCSI_TARGET_OPERATION */  
#endif                                 
                                       
#define  SCSI_ENABLE_IRQ(hhcb)         SCSIHEnableIRQ((hhcb))
#define  SCSI_DISABLE_IRQ(hhcb)        SCSIHDisableIRQ((hhcb))
#define  SCSI_POLL_IRQ(hhcb)           SCSIHPollIRQ((hhcb))
#define  SCSI_FRONT_END_ISR(hhcb)      SCSIHFrontEndISR((hhcb))
#define  SCSI_SAVE_STATE(hhcb,state)         SCSIHSaveState((hhcb),(state))
#define  SCSI_RESTORE_STATE(hhcb,state)      SCSIHRestoreState((hhcb),(state))
#define  SCSI_SIZE_SEEPROM(hhcb)       SCSIHSizeSEEPROM((hhcb))
#define  SCSI_READ_SEEPROM(hhcb,dataBuffer,wordAddress,wordLength) SCSIHReadSEEPROM((hhcb),(dataBuffer),(wordAddress),(wordLength))
#define  SCSI_WRITE_SEEPROM(hhcb,dataBuffer,wordAddress,wordLength) SCSIHWriteSEEPROM((hhcb),(wordAddress),(wordLength),(dataBuffer))
#define  SCSI_FORCE_NEGOTIATION(unitHandle)     SCSIHForceNegotiation((unitHandle))
#define  SCSI_SUPPRESS_NEGOTIATION(unitHandle)  SCSIHSuppressNegotiation((unitHandle))
#define  SCSI_ENABLE_EXP_STATUS(hhcb)  SCSIHEnableExpStatus((hhcb))
#define  SCSI_DISABLE_EXP_STATUS(hhcb) SCSIHDisableExpStatus((hhcb))
#if SCSI_PAC_NSX_REPEATER
#define  SCSI_SINGLE_END_BUS(hhcb)     SCSIHSingleEndBus((hhcb))
#define  SCSI_FORCE_NEGO_ACTIVE_TARGET(hhcb)    SCSIHForceNegoActiveTarget((hhcb))
#define  SCSI_SUPPRESS_NEGO_ACTIVE_TARGET(unitHandle) SCSIHSuppressNegoActiveTarget((unitHandle))
#endif /* SCSI_PAC_NSX_REPEATER */

#if SCSI_TARGET_OPERATION
#define SCSI_DISABLE_SELECTION_IN(hhcb)                  SCSIHTargetModeDisable((hhcb))
#define SCSI_CHECK_SELECTION_IN_INTERRUPT_PENDING(hhcb)  SCSIHTargetSelInPending((hhcb))
#endif /* SCSI_TARGET_OPERATION */
#if SCSI_PAC_NSX_REPEATER || SCSI_NSX_REPEATER
#define SCSI_GET_NSX_BUFFER(hiob)   OSD_GET_NSX_BUFFER((hiob))
#define SCSI_RETURN_NSX_STATUS(hiob,status,count) OSD_RETURN_NSX_STATUS((hiob),(status),(count))
#endif /* SCSI_PAC_NSX_REPEATER || SCSI_NSX_REPEATER */
