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

/***************************************************************************
* $Header:   Y:/source/chdkscsi/CHIMTEST.HV_   1.0.1.0   11 Aug 1997 18:00:58   MAKI1985  $                                                                          *
* Copyright 1995,1996 Adaptec, Inc.,  All Rights Reserved.                 *
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
*  Module Name:   CHIMTEST.H
*
*  Description:
*                 Codes to exercise and test hardware management module
*
*  Owners:        ECX IC Firmware Team
*    
*  Notes:         NONE
*
***************************************************************************/

/***************************************************************************
*  Macros for accessing HIM routines
***************************************************************************/
#define  HIMtGetNextHostDeviceType   SCSIRoutines.HIMGetNextHostDeviceType
#define  HIMtCreateInitializationTSCB \
             SCSIRoutines.HIMCreateInitializationTSCB
#define  HIMtGetConfiguration SCSIRoutines.HIMGetConfiguration
#define  HIMtSetConfiguration SCSIRoutines.HIMSetConfiguration
#define  HIMtSizeAdapterTSCB SCSIRoutines.HIMSizeAdapterTSCB
#define  HIMtCreateAdapterTSCB SCSIRoutines.HIMCreateAdapterTSCB
#define  HIMtSetupAdapterTSCB SCSIRoutines.HIMSetupAdapterTSCB
#define  HIMtCheckMemoryNeeded SCSIRoutines.HIMCheckMemoryNeeded
#define  HIMtSetMemoryPointer SCSIRoutines.HIMSetMemoryPointer
#define  HIMtVerifyAdapter SCSIRoutines.HIMVerifyAdapter
#define  HIMtInitialize SCSIRoutines.HIMInitialize
#define  HIMtSizeTargetTSCB SCSIRoutines.HIMSizeTargetTSCB
#define  HIMtCheckTargetTSCBNeeded SCSIRoutines.HIMCheckTargetTSCBNeeded
#define  HIMtCreateTargetTSCB SCSIRoutines.HIMCreateTargetTSCB
#define  HIMtDisableIRQ SCSIRoutines.HIMDisableIRQ
#define  HIMtEnableIRQ SCSIRoutines.HIMEnableIRQ
#define  HIMtPollIRQ SCSIRoutines.HIMPollIRQ
#define  HIMtFrontEndISR SCSIRoutines.HIMFrontEndISR
#define  HIMtBackEndISR SCSIRoutines.HIMBackEndISR
#define  HIMtQueueIOB SCSIRoutines.HIMQueueIOB
#define  HIMtPowerEvent SCSIRoutines.HIMPowerEvent
#define  HIMtValidateTargetTSH SCSIRoutines.HIMValidateTargetTSH
#define  HIMtClearTargetTSH SCSIRoutines.HIMClearTargetTSH
#define  HIMtSaveState SCSIRoutines.HIMSaveState
#define  HIMtRestoreState SCSIRoutines.HIMRestoreState
#define  HIMtProfileAdapter SCSIRoutines.HIMProfileAdapter
#define  HIMtReportAdjustableAdapterProfile \
             SCSIRoutines.HIMReportAdjustableAdapterProfile
#define  HIMtAdjustAdapterProfile SCSIRoutines.HIMAdjustAdapterProfile
#define  HIMtProfileTarget SCSIRoutines.HIMProfileTarget
#define  HIMtReportAdjustableTargetProfile \
             SCSIRoutines.HIMReportAdjustableTargetProfile
#define  HIMtAdjustTargetProfile SCSIRoutines.HIMAdjustTargetProfile
#define  HIMtSaveContextToTSH SCSIRoutines.HIMSaveContextToTSH
#define  HIMtGetContextFromTSH SCSIRoutines.HIMGetContextFromTSH
#define  HIMtGetNVSize SCSIRoutines.HIMGetNVSize
#define  HIMtGetNVOSMSegment SCSIRoutines.HIMGetNVOSMSegment
#define  HIMtPutNVData SCSIRoutines.HIMPutNVData
#define  HIMtGetNVData SCSIRoutines.HIMGetNVData
#define  HIMtMoveTargetTSCB SCSIRoutines.HIMMoveTargetTSCB

/***************************************************************************
*  OSM routines function prototype
***************************************************************************/
HIM_UINT8 OSMMapIOHandle(void HIM_PTR,HIM_UINT8,HIM_UINT32,HIM_UINT32,
                         HIM_UINT32,HIM_UINT16,HIM_IO_HANDLE HIM_PTR);
HIM_UINT8 OSMReleaseIOHandle(void HIM_PTR,HIM_IO_HANDLE);
void OSMEvent(void HIM_PTR,HIM_UINT16, void HIM_PTR,...);
HIM_BUS_ADDRESS OSMGetBusAddress(void HIM_PTR,HIM_UINT8,void HIM_PTR);
void OSMAdjustBusAddress(HIM_BUS_ADDRESS HIM_PTR,int);
HIM_UINT32 OSMGetNVSize(void HIM_PTR);
HIM_UINT8 OSMPutNVData(void HIM_PTR,HIM_UINT32,void HIM_PTR,HIM_UINT32);
HIM_UINT8 OSMGetNVData(void HIM_PTR,void HIM_PTR,HIM_UINT32,HIM_UINT32);
HIM_UEXACT8 OSMReadUExact8(HIM_IO_HANDLE,HIM_UINT32);
HIM_UEXACT16 OSMReadUExact16(HIM_IO_HANDLE,HIM_UINT32);
HIM_UEXACT32 OSMReadUExact32(HIM_IO_HANDLE,HIM_UINT32);
void OSMReadStringUExact8(HIM_IO_HANDLE,HIM_UINT32,HIM_UEXACT8 HIM_PTR,
                          HIM_UINT32,HIM_UINT8);
void OSMReadStringUExact16(HIM_IO_HANDLE,HIM_UINT32,HIM_UEXACT16 HIM_PTR,
                          HIM_UINT32,HIM_UINT8);
void OSMReadStringUExact32(HIM_IO_HANDLE,HIM_UINT32,HIM_UEXACT32 HIM_PTR, 
                          HIM_UINT32,HIM_UINT8);
void OSMWriteUExact8(HIM_IO_HANDLE,HIM_UINT32,HIM_UEXACT8);
void OSMWriteUExact16(HIM_IO_HANDLE,HIM_UINT32,HIM_UEXACT16);
void OSMWriteUExact32(HIM_IO_HANDLE,HIM_UINT32,HIM_UEXACT32);
void OSMWriteStringUExact8(HIM_IO_HANDLE,HIM_UINT32,HIM_UEXACT8 HIM_PTR,
                           HIM_UINT32,HIM_UINT8);
void OSMWriteStringUExact16(HIM_IO_HANDLE,HIM_UINT32,HIM_UEXACT16 HIM_PTR,
                            HIM_UINT32,HIM_UINT8);
void OSMWriteStringUExact32(HIM_IO_HANDLE,HIM_UINT32,HIM_UEXACT32 HIM_PTR,
                            HIM_UINT32,HIM_UINT8);
void OSMSynchronizeRange(HIM_IO_HANDLE,HIM_UINT32,HIM_UINT32);
void OSMWatchdog(void HIM_PTR,HIM_WATCHDOG_FUNC,HIM_UINT32);
HIM_UINT8 OSMSaveInterruptState();
void  OSMSetInterruptState(HIM_UINT8);
HIM_UEXACT32 OSMReadPCIConfigurationDword(void HIM_PTR,HIM_UINT8);
HIM_UEXACT16 OSMReadPCIConfigurationWord(void HIM_PTR,HIM_UINT8);
HIM_UEXACT8 OSMReadPCIConfigurationByte(void HIM_PTR,HIM_UINT8);
void OSMWritePCIConfigurationDword(void HIM_PTR,HIM_UINT8,HIM_UEXACT32);
void OSMWritePCIConfigurationWord(void HIM_PTR,HIM_UINT8,HIM_UEXACT16);
void OSMWritePCIConfigurationByte(void HIM_PTR,HIM_UINT8,HIM_UEXACT8);
void OSMDelay(void HIM_PTR,HIM_UINT32);
