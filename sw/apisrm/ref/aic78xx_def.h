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

/* $Header:   Y:/source/chimscsi/src/chim.hV_   1.4.20.0   30 Jan 1998 19:26:44   SPAL3094  $ */
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
*  Module Name:   CHIM.H
*
*  Description:   Shortcut front-end to include all other CHIM .h's.
*
*  Owners:        ECX IC Firmware Team
*
***************************************************************************/
#ifndef  CHIM_INCLUDED
#define  CHIM_INCLUDED

/*$Header:   Y:/source/chimscsi/src/chimhw.hV_   1.3.20.0   31 Jan 1998 10:26:16   SPAL3094  $*/
/***************************************************************************
*                                                                          *
* Copyright 1995, 1996, 1997, 1998 Adaptec, Inc.,  All Rights Reserved.    *
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
*  Module Name:   CHIMHW.H
*
*  Description:   Basic hardware 
*
*  Owners:        ECX IC Firmware Team
*
*  Notes:         Contains very basic #defines:
*                    Endianness of hardware.
*                    Scatter-gather format
*                    Host bus (PCI, etc.)
*                 All HIM's compiled together must agree on
*                    the same chimhw.h
*
***************************************************************************/
#define HIM_HA_LITTLE_ENDIAN 1

#define HIM_SG_LIST_TYPE     0    /* phys addr, then length, with       */
                                  /*    end-of-list delimiter           */
                                  /* currently, no other types defined  */

#define HIM_HOST_BUS  HIM_HOST_BUS_PCI  /* possible types in chimcom.h  */

/*$Header:   Y:/source/chimscsi/src/CHIMOSM.HVT   1.9.14.0   13 Nov 1997 18:07:38   MAKI1985  $*/
/***************************************************************************
*                                                                          *
* Copyright 1995, 1996, 1997, 1998 Adaptec, Inc.,  All Rights Reserved.    *
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
*  Module Name:   CHIMOSM.H
*
*  Description:   Definitions for customization by OSM writer
*
*  Owners:        Template provided by ECX IC Firmware Team
*
*  Notes:         Contains the following, as required by the 
*                 CHIM Specification (See Section 12.1 if desired):
*                    him_include_xxx: Enables him-specific .h's.
*                    system-specific #defines
*                       OSM_CPU_LITTLE_ENDIAN, 
*                       OSM_BUS_ADDRESS_SIZE, etc.
*                    OSM-specified types
*                       HIM_PTR (near or far)
*                       HIM_UINT8, HIM_UEXACT16, etc.
*                       HIM_BUS_ADDRESS
*                       HIM_BOOLEAN
*                       HIM_IO_HANDLE
*                    macros
*                       HIM_FLUSH_CACHE, etc.
*                       OSMmemset, etc.
*                       OSMoffsetof, OSMAssert, OSMDebugPrint,
*                           OSMEnterDebugger   
*                       HIM_PUT_BIG_ENDIAN, etc.
*
***************************************************************************/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* HIM's to be included                                                   */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#define HIM_INCLUDE_SCSI

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Endian options                                                         */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#define OSM_CPU_LITTLE_ENDIAN  1 /*  0 - big    endian                  */
                                 /*  1 - little endian                  */
#define OSM_DMA_SWAP_ENDIAN    0 /*  0 - no swapping                    */
                                 /*  1 - swap every osm_swap_width      */
                                 /* -1 - swap by other means            */
#define OSM_DMA_SWAP_WIDTH    32 /* Number of swapping bits.  Also      */
                                 /* used by the HIM as a DMA-alignment  */
                                 /* equirement                          */
#define OSM_BUS_ADDRESS_SIZE  32 /* Number of bits in bus address       */

/***************************************************************************
* Definition for memory pointer reference
***************************************************************************/
#define  HIM_PTR    *  /* The pointer passed to all HIM calls and structs */

/***************************************************************************
* Data types
***************************************************************************/
#if 0   /* The following are examples of the optimum size for each */
    typedef char            HIM_SINT8;    /* signed   int  - min of   8       */
    typedef unsigned char   HIM_UINT8;    /* unsigned int  - min of   8       */
    typedef char            HIM_SEXACT8;  /* signed   byte - exactly  8       */
    typedef unsigned char   HIM_UEXACT8;  /* unsigned byte - exactly  8       */

    typedef short           HIM_SINT16;   /* signed   int  - min of   16      */
    typedef unsigned short  HIM_UINT16;   /* unsigned int  - min of   16      */
    typedef short           HIM_SEXACT16; /* signed   word - exactly  16      */
    typedef unsigned short  HIM_UEXACT16; /* unsigned word - exactly  16      */

    typedef long            HIM_SINT32;   /* signed   int   - min of  32      */
    typedef unsigned long   HIM_UINT32;   /* unsigned int   - min of  32      */
    typedef long            HIM_SEXACT32; /* signed   dword - exactly 32      */
    typedef unsigned long   HIM_UEXACT32; /* unsigned dword - exactly 32      */
#endif

/* Sizes optimized for speed */
typedef int             HIM_SINT8;    /* signed   int  - min of   8       */
typedef unsigned int    HIM_UINT8;    /* unsigned int  - min of   8       */
typedef char            HIM_SEXACT8;  /* signed   byte - exactly  8       */
typedef unsigned char   HIM_UEXACT8;  /* unsigned byte - exactly  8       */

typedef long            HIM_SINT16;   /* signed   int  - min of   16      */
typedef unsigned long   HIM_UINT16;   /* unsigned int  - min of   16      */
typedef short           HIM_SEXACT16; /* signed   word - exactly  16      */
typedef unsigned short  HIM_UEXACT16; /* unsigned word - exactly  16      */

typedef long            HIM_SINT32;   /* signed   int   - min of  32      */
typedef unsigned long   HIM_UINT32;   /* unsigned int   - min of  32      */
typedef int             HIM_SEXACT32; /* signed   dword - exactly 32      */
typedef unsigned int    HIM_UEXACT32; /* unsigned dword - exactly 32      */

typedef unsigned char   HIM_PAD;      /* padding                          */
typedef unsigned int    HIM_PAD_BIT;  /* padding                          */
#define SCSI_PAD HIM_PAD
#define SCSI_PAD_BIT HIM_PAD_BIT

/***************************************************************************
* Define a BUS ADDRESS (Physical address)
***************************************************************************/
typedef void HIM_PTR HIM_BUS_ADDRESS;

/***************************************************************************
* Define a boolean 
***************************************************************************/
typedef enum HIM_BOOLEAN_ENUM { HIM_FALSE, HIM_TRUE } HIM_BOOLEAN; /* boolean */

/***************************************************************************
* Definitions for the I/O handle
***************************************************************************/
typedef struct HIM_IO_HANDLE_
{
    struct pb *pb;
    HIM_UEXACT32    baseAddress; /* raw base address */
    HIM_BOOLEAN     memoryMapped;
    HIM_PAD         unused[3];
} HIM_PTR HIM_IO_HANDLE;

/***************************************************************************
* Definitions for cache flushing and invalidating
***************************************************************************/

#define  HIM_FLUSH_CACHE(pMemory,length)
#define  HIM_INVALIDATE_CACHE(pMemory,length)


/***************************************************************************
* Definitions for standard ANSI-C functions
***************************************************************************/

#define  OSMmemset(buf,value,length)   memset((char HIM_PTR)(buf),(char) (value),(int) (length))
#define  OSMmemcmp(buf1,buf2,length)   memcmp((char HIM_PTR)(buf1),(char HIM_PTR)(buf2),(int) (length))
#define  OSMmemcpy(buf1,buf2,length)   memcpy((char HIM_PTR)(buf1),(char HIM_PTR)(buf2),(int) (length))


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* miscellaneous system utilities */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
extern  HIM_UINT32 OSMDebugMode;
extern  long       ScreenHandle;

#define OSMoffsetof(structDef,member)  (HIM_UEXACT16)((HIM_UEXACT32)&(((structDef *)0)->member))
#define OSMAssert(x)                   if(!(x)) EnterDebugger();
#define OSMDebugPrint(priority,string) if ((priority) >= OSMDebugMode) \
                                          OutputToScreen(ScreenHandle,string);
#define OSMEnterDebugger()             EnterDebugger();

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/* Endian-transformation macros    */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
#define HIM_PUT_BYTE_STRING(pContext,dmaStruct,offset,pSrc,length)\
        OSMmemcpy(((HIM_UEXACT8 *)(dmaStruct))+(offset),    \
                  (HIM_UEXACT8 *)pSrc,                      \
                  length)

#define HIM_PUT_LITTLE_ENDIAN8(adapterTSH,dmaStruct,offset,value)  \
            *(((HIM_UEXACT8 *)(dmaStruct))+(offset)) = (HIM_UEXACT8)(value)

#define HIM_PUT_LITTLE_ENDIAN16(adapterTSH,dmaStruct,offset,value) \
            *((HIM_UEXACT16 *)(((HIM_UEXACT8 *)\
               (dmaStruct))+(offset))) = (HIM_UEXACT16)(value)

#define HIM_PUT_LITTLE_ENDIAN24(adapterTSH,dmaStruct,offset,value) \
            *((HIM_UEXACT32 *)(((HIM_UEXACT8 *)(dmaStruct))+(offset))) &= \
                        0xFF000000; \
            *((HIM_UEXACT32 *)(((HIM_UEXACT8 *)(dmaStruct))+(offset))) |= \
                        ((HIM_UEXACT32)(value)) & 0x00FFFFFF

#define HIM_PUT_LITTLE_ENDIAN32(adapterTSH,dmaStruct,offset,value) \
            *((HIM_UEXACT32 *)(((HIM_UEXACT8 *)\
               (dmaStruct))+(offset))) = (HIM_UEXACT32)(value)
                                       
#define HIM_PUT_LITTLE_ENDIAN64(adapterTSH,dmaStruct,offset,value) \
            HIM_PUT_LITTLE_ENDIAN32((adapterTSH),(dmaStruct),(offset),\
                                       *((HIM_UEXACT32 *)&value));\
            HIM_PUT_LITTLE_ENDIAN32((adapterTSH),(dmaStruct),(offset)+4,\
                                       *(((HIM_UEXACT32 *)&value)+1))

#define HIM_GET_BYTE_STRING(pContext,pDest,dmaStruct,offset,length)\
        OSMmemcpy((HIM_UEXACT8 *)pDest,                      \
                  ((HIM_UEXACT8 *)(dmaStruct))+(offset),     \
                  length);

#define HIM_GET_LITTLE_ENDIAN8(adapterTSH,pValue,dmaStruct,offset)  \
            *((HIM_UEXACT8 *)pValue) = *(((HIM_UEXACT8 *)(dmaStruct))+(offset));

#define HIM_GET_LITTLE_ENDIAN16(adapterTSH,pValue,dmaStruct,offset) \
            *((HIM_UEXACT16 *)pValue) =\
               *((HIM_UEXACT16 *)(((HIM_UEXACT8 *)(dmaStruct))+(offset)))
                                                            

#define HIM_GET_LITTLE_ENDIAN24(adapterTSH,pValue,dmaStruct,offset) \
            *((HIM_UEXACT32 *)pValue) = *((HIM_UEXACT32 *)\
                  (((HIM_UEXACT8 *)(dmaStruct))+(offset))) & 0x00FFFFFF
                        
#define HIM_GET_LITTLE_ENDIAN32(adapterTSH,pValue,dmaStruct,offset) \
            *((HIM_UEXACT32 *)pValue) =  *((HIM_UEXACT32 *)\
                                 (((HIM_UEXACT8 *)(dmaStruct))+(offset)))

#define HIM_GET_LITTLE_ENDIAN64(adapterTSH,pValue,dmaStruct,offset) \
            HIM_GET_LITTLE_ENDIAN32((adapterTSH),(pValue),(dmaStruct),(offset));\
            HIM_GET_LITTLE_ENDIAN32((adapterTSH),((HIM_UEXACT32 *)\
                                          (pValue))+1,(dmaStruct),offset+4)

#define HIM_PUT_BIG_ENDIAN8(adapterTSH,dmaStruct,offset,value)\
            *(((HIM_UEXACT8 *)(dmaStruct))+(offset)) = \
                                       (HIM_UEXACT8)(value)

#define HIM_PUT_BIG_ENDIAN16(adapterTSH,dmaStruct,offset,value)\
            HIM_PUT_BIG_ENDIAN8((adapterTSH),(dmaStruct),(offset),\
                                      (HIM_UEXACT8)(value>>8));\
            HIM_PUT_BIG_ENDIAN8((adapterTSH),(dmaStruct),(offset)+1,\
                                      (HIM_UEXACT8)(value))

#define HIM_PUT_BIG_ENDIAN24(adapterTSH,dmaStruct,offset,value)\
            HIM_PUT_BIG_ENDIAN8((adapterTSH),(dmaStruct),(offset),\
                                   (HIM_UEXACT8)(value>>16));\
            HIM_PUT_BIG_ENDIAN8((adapterTSH),(dmaStruct),(offset)+1,\
                                   (HIM_UEXACT8)(value>>8));\
            HIM_PUT_BIG_ENDIAN8((adapterTSH),(dmaStruct),(offset)+2,\
                                   (HIM_UEXACT8)(value))

#define HIM_PUT_BIG_ENDIAN32(adapterTSH,dmaStruct,offset,value)\
            HIM_PUT_BIG_ENDIAN16(adapterTSH,dmaStruct,offset,\
                                  (HIM_UEXACT16)(value>>16));\
            HIM_PUT_BIG_ENDIAN16(adapterTSH,dmaStruct,offset+2,\
                                  (HIM_UEXACT16)(value))

#define HIM_PUT_BIG_ENDIAN64(adapterTSH,dmaStruct,offset,value) \
            HIM_PUT_BIG_ENDIAN32((adapterTSH),(dmaStruct),(offset),\
                              *(((HIM_UEXACT32 *)&value)+1));\
            HIM_PUT_BIG_ENDIAN32((adapterTSH),(dmaStruct),(offset)+4,\
                              *(((HIM_UEXACT32 *)&value)+0))

#define HIM_GET_BIG_ENDIAN8(adapterTSH,pValue,dmaStruct,offset)\
            *((HIM_UEXACT8 *)pValue) = *(((HIM_UEXACT8 *)(dmaStruct))+\
                                                            (offset))

#define HIM_GET_BIG_ENDIAN16(adapterTSH,pValue,dmaStruct,offset)\
            HIM_GET_BIG_ENDIAN8((adapterTSH),((HIM_UEXACT8 *)\
                                    (pValue))+1,(dmaStruct),(offset));\
            HIM_GET_BIG_ENDIAN8((adapterTSH),(pValue),(dmaStruct),(offset)+1);

#define HIM_GET_BIG_ENDIAN24(adapterTSH,pValue,dmaStruct,offset)\
            HIM_GET_BIG_ENDIAN8((adapterTSH),((HIM_UEXACT8 *)(pValue))+2,\
                                                (dmaStruct),(offset));\
            HIM_GET_BIG_ENDIAN8((adapterTSH),((HIM_UEXACT8 *)(pValue))+1,\
                                                (dmaStruct),(offset)+1);\
            HIM_GET_BIG_ENDIAN8((adapterTSH),(pValue),(dmaStruct),(offset)+2)

#define HIM_GET_BIG_ENDIAN32(adapterTSH,pValue,dmaStruct,offset)\
            HIM_GET_BIG_ENDIAN16((adapterTSH),((HIM_UEXACT16 *)(pValue))+1,\
                                                 (dmaStruct),(offset));\
            HIM_GET_BIG_ENDIAN16((adapterTSH),((HIM_UEXACT16 *)(pValue)),\
                                                (dmaStruct),(offset)+2);

#define HIM_GET_BIG_ENDIAN64(adapterTSH,pValue,dmaStruct,offset)\
            HIM_GET_BIG_ENDIAN32((adapterTSH),((HIM_UEXACT32 *)(pValue))+1,\
                                                (dmaStruct),(offset));\
            HIM_GET_BIG_ENDIAN32((adapterTSH),((HIM_UEXACT32 *)(pValue)),\
                                                (dmaStruct),(offset)+4)

#define OSMDEBUGPRINT(string)   printf string

#endif	/* _CHIMOSM_H_ */

/* $Header:   Y:/source/chimscsi/src/chimcom.hV_   1.6.20.0   31 Jan 1998 10:30:28   SPAL3094  $ */
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
*  Module Name:   CHIMCOM.H
*
*  Description:   Definitions which are common to all implementations
*
*  Owners:        ECX IC Firmware Team
*
*  Notes:         Contains basic data structures 
*                    (quadlet, him_bus_address, him_host_id, etc.)
*                 Contains #defines for pseudo-enum values
*                    (All s/g defines, host bus types, 
*                     memory categories, etc.)
*
*  History:
*    08/14/96     DRB   Changed pci.architecture->pci.treeNumber
*    04/26/96     DRB   Added trailing underscore to 'struct x_'
*                           to be C++-compiler friendly.
*    02/08/96     DRB   changed pci.architect -> pci.architecture
*
***************************************************************************/


/***************************************************************************
* Definitions for two byte entity
***************************************************************************/
#if OSM_CPU_LITTLE_ENDIAN
    typedef union HIM_DOUBLET_                  /* two orders entity              */
    {
        struct
        {
            HIM_UEXACT8  order0;
            HIM_UEXACT8  order1;
        } u8;
    } HIM_DOUBLET;
#else
    typedef union HIM_DOUBLET_                  /* two orders entity              */
    {
        struct
        {
            HIM_UEXACT8  order1;
            HIM_UEXACT8  order0;
        } u8;
    } HIM_DOUBLET;
#endif


/***************************************************************************
* Definitions for four byte entity
***************************************************************************/
#if OSM_CPU_LITTLE_ENDIAN
    typedef union HIM_QUADLET_                  /* four orders entity             */
    {
        struct
        {
            HIM_UEXACT8  order0;
            HIM_UEXACT8  order1;
            HIM_UEXACT8  order2;
            HIM_UEXACT8  order3;
        } u8;
        struct
        {
            HIM_UEXACT16 order0;
            HIM_UEXACT16 order1;
        } u16;
    } HIM_QUADLET;
#else
    typedef union HIM_QUADLET_                  /* four orders entity             */
    {
        struct
        {
            HIM_UEXACT8  order3;
            HIM_UEXACT8  order2;
            HIM_UEXACT8  order1;
            HIM_UEXACT8  order0;
        } u8;
        struct
        {
            HIM_UEXACT16 order1;
            HIM_UEXACT16 order0;
        } u16;
    } HIM_QUADLET;
#endif


/***************************************************************************
* Definitions for eight byte entity
***************************************************************************/
#if OSM_CPU_LITTLE_ENDIAN
    typedef union HIM_OCTLET_                  /* 8 orders entity                */
    {
        struct
        {
            HIM_UEXACT8 order0;
            HIM_UEXACT8 order1;
            HIM_UEXACT8 order2;
            HIM_UEXACT8 order3;
            HIM_UEXACT8 order4;
            HIM_UEXACT8 order5;
            HIM_UEXACT8 order6;
            HIM_UEXACT8 order7;
        } u8;
        struct
        {
            HIM_UEXACT16 order0;
            HIM_UEXACT16 order1;
            HIM_UEXACT16 order2;
            HIM_UEXACT16 order3;
        } u16;
        struct
        {
            HIM_UEXACT32 order0;
            HIM_UEXACT32 order1;
        } u32;
    } HIM_OCTLET;
#else
    typedef union HIM_OCTLET_                  /* 8 orders entity                */
    {
        struct
        {
            HIM_UEXACT8 order7;
            HIM_UEXACT8 order6;
            HIM_UEXACT8 order5;
            HIM_UEXACT8 order4;
            HIM_UEXACT8 order3;
            HIM_UEXACT8 order2;
            HIM_UEXACT8 order1;
            HIM_UEXACT8 order0;
        } u8;
        struct
        {
            HIM_UEXACT16 order3;
            HIM_UEXACT16 order2;
            HIM_UEXACT16 order1;
            HIM_UEXACT16 order0;
        } u16;
        struct
        {
            HIM_UEXACT32 order1;
            HIM_UEXACT32 order0;
        } u32;
    } HIM_OCTLET;
#endif


/***************************************************************************
* Buffer descriptor
***************************************************************************/
typedef struct HIM_BUFFER_DESCRIPTOR_
{
    HIM_BUS_ADDRESS busAddress;          /* bus address                   */
    HIM_UEXACT32    bufferSize;          /* scatter/gather count          */
    void HIM_PTR    virtualAddress;      /* virtual address for iob       */
} HIM_BUFFER_DESCRIPTOR;


/***************************************************************************
* Miscellaneous definitions
***************************************************************************/
typedef HIM_UEXACT32 HIM_HOST_ID;

typedef union HIM_HOST_ADDRESS_
{
    struct HIM_PCI_ADDRESS_
    {  /* PCI address */
        HIM_UINT8 treeNumber;               /* hierarchy                     */
        HIM_UINT8 busNumber;                /* PCI bus number                */
        HIM_UINT8 deviceNumber;             /* PCI device number             */
        HIM_UINT8 functionNumber;           /* PCI function number           */
    } pciAddress;

    struct HIM_EISA_ADDRESS_
    {  /* EISA address */
        HIM_UINT8 slotNumber;
    } eisaAddress;
} HIM_HOST_ADDRESS;

/***************************************************************************
 * host bus type definitions 
 ***************************************************************************/
#define HIM_HOST_BUS_PCI     0              /* PCI bus                       */
#define HIM_HOST_BUS_EISA    1              /* EISA bus                      */

/***************************************************************************
* Memory category code definition
***************************************************************************/
#define  HIM_MC_UNLOCKED   0        /* unlocked, not DMAable, cached */
#define  HIM_MC_LOCKED     1        /* locked, DMAable memory, non-cached */

/* him-specific .h's - one per him */
#ifdef HIM_INCLUDE_SSA /* to be included by OSM writer in chimosm.h */
#include "chimssa.h"	/* exclude */
#endif
#ifdef HIM_INCLUDE_SCSI

/*$Header:   Y:/source/chimscsi/src/CHIMSCSI.HV_   1.5.14.0   13 Nov 1997 18:07:14   MAKI1985  $*/
/***************************************************************************
*                                                                          *
* Copyright 1995,1996,1997,1998  Adaptec, Inc.,  All Rights Reserved.      *
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
*  Module Name:   CHIMSCSI.H
*
*  Description:   SCSI-specific #defines
*
*  Owners:        Milpitas SCSI HIM Team
*
*  Notes:         Includes
*                     Entry point to get other function pointers
*                     Required size of InitTSCB
*                     HIM_SSA_INCLUDED for OSM convenience
*
***************************************************************************/
#ifndef HIM_CHIMSCSI_INCLUDED
#define HIM_CHIMSCSI_INCLUDED
#define HIM_ENTRYNAME_SCSI      SCSIGetFunctionPointers
#define HIM_SCSI_INIT_TSCB_SIZE 1000
#endif

#endif
/* end of him-specific .h's */

#if 0
#include "chimdef.h"	/* exclude */
#endif
