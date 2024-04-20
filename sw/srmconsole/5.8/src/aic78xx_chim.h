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

/* $Header: /home/CVSROOT/sw/srmconsole/5.8/SRC/AIC78XX_CHIM.H,v 1.2 2001/01/13 05:23:33 chris Exp $ */
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
*  Module Name:   CHIMDEF.H
*
*  Description:   Main data structures of Common HIM Interface
*                 (Particularly those that use basic CHIM types)
*
*  Owners:        ECX IC Firmware Team
*
*  Notes:         Contains the following:
*                 Main, complicated CHIM structures:
*                    HIM_IOB, HIM_GRAPH_NODE, 
*                    HIM_CONFIGURATION, profile structures
*                 #defines associated with IOB:
*                    taskStatus, function codes, etc.
*                 function-specific defines
*                     event types for OSMevent, HIMPowerEvent, etc.
*                 him_func_ptrs, him_osm_func_ptrs
*                     function prototypes in handy structures
*                     for him-osm passoff
*
*  History:
*      6/3/97  ACS    Updated to match Chim specification rev 1.10
*      3/20/97 ACS    Added Adapter profile field AP_SCSI2_TargetRejectLuntar.
*
*      9/31/96 ACS    Added Target Mode implementation. 
*                     Includes the following:
*                        Version 4 of HIM_CONFIGURATION
*                           targetNumNexusTaskSetHandles
*                           initiatorMode
*                           targetMode.
*                        New IOB fields
*                           outOfOrderTransfer (flagsIob field)
*                           targetCdbOrTmf (flagsIob field)
*                           ioLength
*                           relativeOffset
*                        New function definitions
*                           HIM_ESTABLISH_CONNECTION
*                           HIM_REESTABLISH_AND_COMPLETE
*                           HIM_REESTABLISH_INTERMEDIATE
*                           HIM_ABORT_NEXUS
*                        New task attribute definitions
*                           HIM_TASK_ACA
*                           HIM_TASK_NO_TAG
*                           HIM_TASK_UNKNOWN
*                        New task status definitions
*                           HIM_IOB_OUT_OF_ORDER_TRANSFER_REJECTED
*                           <others TBD>
*                        Version 5 of HIM_ADAPTER_PROFILE
*                           AP_TargetNumNexusTaskSetHandles
*                        Version 1 of HIM_NEXUS_PROFILE
*                        Version 4 of HIM_FUNC_PTRS
*                           HIMProfileNexus
*                           HIMClearNexusTSH                               
*
*      9/09/96 DRB    Add AP_LowestScanTarget to adapter profile
*                     Add freeze/unfreeze event codes, change bus-reset
*                         event code name.
*                     Enforced 80-character line width.
*      8/14/96 DRB    Move HIM_IOB_QUEUE_SUSPENDED from 0x33 to 0x16.
*                     Added AP_MaxIOHandles to adapter profile.  I didn't bump
*                         the version number, because this value's exclusion
*                         was an oversight.
*                     Changed argument in getconfig/setconfig to 'pConfig'
*                         to match documentation.
*      6/24/96 DHM    Update the HIM_CONFIGURATION version number from 2 to 3. 
*      6/24/96 DRB    Add HIM_ADAPTER_NOT_IDLE, HIM_ILLEGAL_CHANGE
*                         profile return calls.
*      6/24/96 DRB    Fix typo: diallowDisconnect->disallowDisconnect
*      6/21/96 DRB    Restore HIM function pointers version # to 3.
*      6/20/96 DRB    Change HIM_TASK_RECOVERY_IO to HIM_TASK_RECOVERY,
*                         collapse taskAttribute numbers to be consecutive.
*                     Drop misleading "_TASKS" from HIM_SUSPEND, HIM_RESUME.
*                     Rework errordata and xca statuses to render obsolete the
*                         iob flag errorDataUnavailable, and allow for
*                         autosense overrun and underrun.
*                     Add 'unknown' values to current scsi negotiation 
*                         parameters.
*      6/14/96 DRB    change him_clear_aca to _xca
*                     Add statuses: HIM_IOB_ERROR_XCA, 
*                         HIM_IOB_NO_XCA_PENDING
*                     IOB: drop suspendOnComplete, change
*                         suspendOnError to freezeOnError
*                     Add transport-specific scsi struct
*                     Add selection timeout to adapter profile.
*                     Reset delay changed to 32 bits.
*                     Add HIM_UNFREEZE_QUEUE IOB function
*      6/04/96 DRB    Remove unusable MoveTargetTSCB.
*                     Add to adapter profile:
*                         memoryMapped
*                         HIM SW version, hardware version
*                     Move from target to adapter profile:
*                         SCSI - disable parity errors
*                     Add to him configuration:
*                         (hardware) stateSize
*                         memoryMapped        
*      5/31/96 DRB    Correct him_entryanme prototypes (previously
*                         added, but incorrect)
*      5/28/96 DRB    Add statuses:
*                         HIM_IOB_TSH_NOT_VALIDATED
*                         HIM_IOB_ADAPTER_NOT_IDLE
*                         HIM_IOB_ABORT_STARTED
*                         HIM_IOB_ABORT_ALREADY_DONE
*                     Changed status #'s for:
*                         HIM_IOB_TERMINATED
*                         HIM_IOB_ABORT_NOT_FOUND
*                     Drop HIM_NO_TAG, change HIM_TASK_ACA to
*                         HIM_TASK_RECOVERY_IO
*                     Add HIMMoveTArgetTSCB
*      5/09/96 DRB    Profile union name: u->himu.  Avoids Unixware
*                         conflict.
*      4/26/96 DRB    Add protocol auto config typedef for
*                         targetCommand
*                     Add trailing underscore to 'struct x_'
*                         for C++-compiler friendliness.
*                     Change HIM_EVENT_ID_REASSIGNED to 
*                         HIM_AUTO_CONFIG_REQUIRED.  Also changed
*                         corresponding auto config reason code.
*                     Add LUN to SSA and FC target profiles.
*      4/02/96 DRB    Version 3 of HIM_TARGET_PROFILE, adding 
*                         TP_TaggedQueuing
*      3/20/96 DRB    Version 2 of HIM_FUNC_PTRS, adding adapterTSH
*                         to HIMCheckMemoryNeeded.
*      3/19/96 DRB    Version 1 of HIM_CONFIGURATION, changing
*                         iobReserveSize to 32 bits (was 16).
*                     Add HIM_IOB_QUEUE_SUSPENDED status.
*                     Add TP_MaxActiveCommands, and shared flag
*                         to target profile.
*                     Add AP_SCSIAdapterID to adapter profile.
*                     Add AP_FIFOThreshold to adapter profile.
*                     Version 2 of adapter, target profiles.
*      3/06/96 DRB    Add version number #defines to profile, 
*                         configuration, function-pointer structs.
*                     Version number is 0 if unchanged from 1.0 spec,
*                         1 otherwise.  
*      3/01/96 DRB    Rip out HIMGetContextFromTSH/HimSaveContextToTSH 
*                     Add pOSMContext to HIMCreateAdapterTSCB
*                     Alter OSMEvent, etc. to use pOSMContext
*                     Add pEventContext to OSMEvent.
*                     Add #defines for types of HIM_PROTOCOL_AUTO_CONFIG
*
*      2/08/96 DRB    HIM_POST_PTR returns HIM_UINT8->HIM_UINT32
*                     Add AP_ExtendedTrans and AP_BiosActive to
*                         adapter profile.
*                     Name unions within profiles as 'u'.
*                     Put HIM_ in front of SCSI-specific structs
*                         within profile unions.
*      1/30/96 DRB    Header keyword
*                     HIMSetIOHandle->HIMVerifyAdapter
*                     HIM_POST_PTR: returns void->HIM_UINT8
*                     No more IOP_ shortcuts for iobflags
*                     HIM_PROTOCOL: SSA and FC removed
*      1/29/96        Initial version.
*
*
*
***************************************************************************/
/***************************************************************************
* Miscellaneous definitions
***************************************************************************/
typedef void    HIM_PTR         HIM_TASK_SET_HANDLE;
typedef struct  HIM_IOB_        HIM_IOB;
typedef struct  HIM_GRAPH_NODE_ HIM_GRAPH_NODE;


/***************************************************************************
* Definitions for HIM_CONFIGURATION
* Used for HIMGetConfiguration()
***************************************************************************/
#define HIM_VERSION_CONFIGURATION 5
typedef struct HIM_CONFIGURATION_
{
    HIM_UINT32   versionNumber;       /* version # of this structure. */
    /* modifiable parameters */
    HIM_UINT32   maxInternalIOBlocks; /* Max I/O blocks per adapter. 
                                       * upper bound on number of 
                                       * simultaneously active I/Os */
    HIM_UINT32   maxTargets;          /* The maximum number of targets per 
                                       * adapter */
    HIM_UINT32   maxSGDescriptors;    /* Max number of elements in 
                                       * scatter/gather list */
    HIM_UINT32   maxTransferSize;     /* maximum bytes in a single IOB */
    HIM_UINT8    memoryMapped;        /* Allows OSM to choose:
                                       *   HIM_IOSPACE - I/O mapped 
                                       *   HIM_MEMORYSPACE - memory mapped
                                       *   HIM_NO_CHOICE_AVAILABLE - HIM does not offer 
                                       *                             choice */
    HIM_UINT8    targetNumIDs;        /* Number of physical addresses that
                                       * this adapter can respond as a target.
                                       */
    HIM_UINT32   targetNumNexusTaskSetHandles;
                                      /* The number of nexus task set handles 
                                       * that can be used by the adapter */  
    HIM_UINT32   targetNumNodeTaskSetHandles; 
                                      /* The number of node task set handles 
                                       * that ccan be used by the adapter. */   
    HIM_BOOLEAN  initiatorMode;       /* Default based on compile option. 
                                       * When set to 1, the OSM may perform
                                       * Initiator functions. */  
    HIM_BOOLEAN  targetMode;          /* Default based on compile option.
                                       * When set to 1 the OSM may perform
                                       * target functions (i.e. receive requests
                                       * from an Initiator). */    
                                      /* Both may be set to 1 but both cannot
                                       * be set to 0 */
    /* unmodifiable parameters */
    HIM_UINT32   iobReserveSize;      /* size of IOB reserve field */
    HIM_UINT32   stateSize;           /* worst-case size for HIMSaveState 
                                       * and HIMRestoreState */
    HIM_UINT16   maxIOHandles;        /* maximum I/O handles HIM will need */  
    HIM_BOOLEAN  virtualDataAccess;   /* HIM uses virtual addresses to access
                                       * data buffers itself */
    HIM_BOOLEAN  needPhysicalAddr;    /* HIM uses physical addresses */
    HIM_BOOLEAN  busMaster;           /* Adapter is a bus master device */
   
    HIM_UINT8    allocBusAddressSize; /* Maximum bus address size allowed for */
                                      /* locked (DMAable) memory that is */
                                      /* allocated in HIMSetMemoryPointer. */
                                      /* (i.e. memory of category HIM_MC_LOCKED) */
   
} HIM_CONFIGURATION;


/***************************************************************************
* Definition for the function called by the OSMWatchdog routine
***************************************************************************/
typedef void (* HIM_WATCHDOG_FUNC)(HIM_TASK_SET_HANDLE adapterTSH);

/***************************************************************************
* Definition for the HIM_IOB post routine
***************************************************************************/
typedef HIM_UINT32 (* HIM_POST_PTR)(HIM_IOB HIM_PTR pIob);

/***************************************************************************
* Definitions for HIM_GRAPH_NODE
***************************************************************************/
struct HIM_GRAPH_NODE_
{
    HIM_IOB HIM_PTR        iob;             /* pointer to associated iob    */
    HIM_GRAPH_NODE HIM_PTR nextNode[2];     /* provide next execution path  */
    HIM_UEXACT8            pathType[2];     /* provide type for next path   */
    HIM_UEXACT8            timeStamp;       /* execution time stamp         */
    HIM_UEXACT8            numPreceedingTasks; /* enable node for execution */
};


/***************************************************************************
* Definitions for HIM_IOB 
***************************************************************************/
struct HIM_IOB_ {
    HIM_BUFFER_DESCRIPTOR   iobReserve;
    HIM_UINT8               function;
    HIM_UINT8               priority;
    /* A bit field structure must not exceed 16 bits. */
    struct {
        HIM_UEXACT16        autoSense:1;
        HIM_UEXACT16        inboundData:1;
        HIM_UEXACT16        outboundData:1;
        HIM_UEXACT16        disableDma:1;
        HIM_UEXACT16        disableNotification:1;
        HIM_UEXACT16        freezeOnError:1; 
        HIM_UEXACT16        outOfOrderTransfer:1;
        HIM_UEXACT16        targetRequestType:2;
    } flagsIob;
    HIM_PAD                 unused[2];

    HIM_GRAPH_NODE HIM_PTR  graphNode;
    void HIM_PTR            transportSpecific;
    void HIM_PTR            osRequestBlock;
    HIM_IOB HIM_PTR         relatedIob;
    HIM_TASK_SET_HANDLE     taskSetHandle;
    volatile HIM_UINT16     taskStatus;
    HIM_UINT32              sortTag;
    HIM_POST_PTR            postRoutine;
    void HIM_PTR            targetCommand;
    HIM_UINT16              targetCommandLength;
    HIM_UINT16              targetCommandBufferSize; 
    HIM_BUFFER_DESCRIPTOR   data;
    HIM_UINT32              ioLength;
    HIM_UINT32              relativeOffset;
    void HIM_PTR            errorData;
    HIM_UINT16              errorDataLength;
    HIM_UEXACT32            residual;
    HIM_UEXACT32            residualError;
    HIM_UINT8               taskAttribute;
};


/* function definitions */
#define  HIM_INITIATE_TASK           0
#define  HIM_ABORT_TASK              1
#define  HIM_ABORT_TASK_SET          2
#define  HIM_CLEAR_XCA               3
#define  HIM_RESET_BUS_OR_TARGET     4
#define  HIM_RESET_HARDWARE          5
#define  HIM_PROTOCOL_AUTO_CONFIG    6
#define  HIM_RESUME                  7
#define  HIM_SUSPEND                 8
#define  HIM_QUIESCE                 9 
#define  HIM_TERMINATE_TASK          10
#define  HIM_UNFREEZE_QUEUE          11
#define  HIM_ESTABLISH_CONNECTION    12
#define  HIM_REESTABLISH_AND_COMPLETE 13
#define  HIM_REESTABLISH_INTERMEDIATE 14
#define  HIM_TARGET_RESET            15
#define  HIM_CLEAR_TASK_SET          16
#define  HIM_LOGICAL_UNIT_RESET      17
#define  HIM_ABORT_NEXUS             18 
#define  HIM_PROBE                   19
#define  HIM_ENABLE_ID               20
#define  HIM_DISABLE_ID              21
#define  HIM_INITIATE_DMA_TASK       22

/* task management response definitions */
#define  HIM_TM_RESPONSE_COMPLETE       0
#define  HIM_TM_RESPONSE_NOT_SUPPORTED  1 
#define  HIM_TM_RESPONSE_FAILED         2    

/* flagsIob targetRequestType definitions */
#define HIM_REQUEST_TYPE_CMND           0   /* Device Command */ 
#define HIM_REQUEST_TYPE_TMF            1   /* Task Management Function */

/* task attribute definitions */
#define  HIM_TASK_SIMPLE             0
#define  HIM_TASK_ORDERED            1
#define  HIM_TASK_HEAD_OF_QUEUE      2
#define  HIM_TASK_RECOVERY           3
#define  HIM_TASK_ACA                4
#define  HIM_TASK_NO_TAG             5
#define  HIM_TASK_UNKNOWN            9

/***************************************************************************
*  taskStatus definitions
***************************************************************************/

/* Normal cases and error-related failures:  */
#define HIM_IOB_GOOD                 0x01  /* IO/function is successful.  */

#define HIM_IOB_ERRORDATA_VALID      0x02  /* I/O finished with an error or
                                           unusual status.  error data is
                                           valid.                         */

#define HIM_IOB_ERRORDATA_REQUIRED   0x03  /* Same as HIM_IOB_ERRORDATA_VALID,
                                           but autosense was not enabled, 
                                           and this error data has not
                                           yet been collected.            */
#define HIM_IOB_ERRORDATA_OVERUNDERRUN   0x04  /* Overrun or underrun occurred 
                                           during autosense command.      */
#define HIM_IOB_ERRORDATA_FAILED         0x05  /* Automatic request for error 
                                           data failed.                   */
#define HIM_IOB_XCA_ERRORDATA_VALID      0x06   /* persistent contingent  */
#define HIM_IOB_XCA_ERRORDATA_REQUIRED   0x07   /* allegiance versions of */
#define HIM_IOB_XCA_ERRORDATA_OVERUNDERRUN 0x08 /* above                  */
#define HIM_IOB_XCA_ERRORDATA_FAILED     0x09   /*                        */


/* IOB errors and support issues: */
#define HIM_IOB_INVALID              0x10  /* IOB is incorrectly formed.  */
#define HIM_IOB_UNSUPPORTED          0x11  /* The CHIM implementation
                                           does not support the requested
                                           function. However, IOB form is 
                                           not in error.                  */
#define HIM_IOB_TSH_INVALID          0x12  /* HIM does not recognize TSH  */
#define HIM_IOB_TSH_NOT_VALIDATED    0x13  /* An IOB was sent to a target TSH
                                           in between execution of protocol
                                           auto config and 
                                           HIMValidateTargetTSH */
#define HIM_IOB_ADAPTER_NOT_IDLE     0x14  /* Some functions require the 
                                           adapterTSH in question to have 
                                           no active I/O's.
                                           This is the return code used 
                                           if this rule is violated.     */
#define HIM_IOB_NO_XCA_PENDING       0x15  /* HIM_CLEAR_XCA was called  
                                           when contingent allegiance was
                                           not pending */ 
#define HIM_IOB_QUEUE_SUSPENDED      0x16  /* Adapter received suspend or 
                                           quiesce before this IOB.  Send
                                           resume or auto config,
                                           respectively, before continuing */


/* target errors detected by CHIM: */
#define HIM_IOB_NO_RESPONSE          0x20  /* Requested target does not 
                                           respond. For parallel SCSI, this 
                                           is  Selection Timeout.         */
#define HIM_IOB_PROTOCOL_ERROR       0x21  /* CHIM detects an unrecoverable
                                           protocol error on the part of the
                                           target.  (It is assumed that if the
                                           target detects an error by the host
                                           adapter, it will be reported as
                                           erro data.)                    */
#define HIM_IOB_CONNECTION_FAILED    0x22  /* Connection between CHIM & target
                                           failed prior to delivery of status.
                                           For parallel SCSI, this is an
                                           unexpected bus-free condition. Same
                                           status is to be used for target and
                                           host adapter breaking the
                                           connection.                    */
#define HIM_IOB_PARITY_ERROR         0x23  /* CHIM detects a parity error by
                                           the target.  (It is assumed
                                           that if the target detects an error
                                           by the host adapter, it will be
                                           reported as error data.)       */
#define HIM_IOB_DATA_OVERUNDERRUN    0x24  /* Overrun or underrun has been
                                           detected.  If residual field is 0,
                                           error was overrun.  Otherwise, the
                                           number of bytes NOT transferred is
                                           in the residual field.         */

#define HIM_IOB_INVALID_LUN          0x25  /* Invalid lun was encountered
                                           durning a HIM_PROBE iob.  The target
                                           is present, but the lun is not
                                           supported.                     */



#define HIM_IOB_DOMAIN_VALIDATION_FAILED   0x26  /* Domain Validation
                                           durning a HIM_PROBE iob.  The
                                           target is marked not  present  */


/* Activity-related unusual conditions: */
#define HIM_IOB_BUSY                 0x30  /* target reports busy status. */
#define HIM_IOB_TASK_SET_FULL        0x31  /* target cannot accept this command
                                           until at least one current command
                                           completes.  For parallel SCSI, this
                                           is Queue Full status.          */
#define HIM_IOB_TARGET_RESERVED      0x32  /* target has been reserved by
                                           another initiator.             */
                                                                                   

/* Abort statues: */
#define HIM_IOB_ABORTED_ON_REQUEST       0x40 /* Normal status for requested 
                                              aborts. */
#define HIM_IOB_ABORTED_REQ_BUS_RESET    0x41 /* A bus or hardware reset
                                              required the CHIM to return this 
                                              IOB to the OSM.  Protocol auto 
                                              config is needed before retry */
#define HIM_IOB_ABORTED_CHIM_RESET       0x42 /* CHIM decision to reset bus 
                                              or target required the return 
                                              of this IOB to OSM.         */
#define HIM_IOB_ABORTED_3RD_PARTY_RESET  0x43 /* A bus reset from another 
                                              inititator or target forced 
                                              the return of this IOB.     */
#define HIM_IOB_ABORTED_BY_CHIM          0x44 /* CHIM decided to abort command
                                              for internal reasons other than
                                              bus or target reset.        */
#define HIM_IOB_ABORTED_REQ_TARGET_RESET 0x45 /* IOB was aborted due to target 
                                              reset requested by OSM.  No Need 
                                              for protocol auto config before
                                              retry                         */
#define HIM_IOB_ABORT_FAILED             0x46 /* target did not respond 
                                              properly to abort sequence.   */
#define HIM_IOB_TERMINATED               0x47 /* Only used for 
                                              HIM_TERMINATE_TASK function to 
                                              HIMQueueIOB.                  */
/* statuses for IOB's with function HIM_ABORT_TASK */
#define HIM_IOB_ABORT_NOT_FOUND          0x48 /* Related IOB to be aborted
                                              was not found in CHIM queues.  
                                              Command has probably completed 
                                              just before abort
                                              request was received.         */
#define HIM_IOB_ABORT_STARTED            0x49 /* HIM has found the IOB to be 
                                              aborted and started the abort
                                              procedure. */
#define HIM_IOB_ABORT_ALREADY_DONE       0x4A /* HIM has found the IOB to be 
                                              aborted in a 'done' queue 
                                              awaiting final interrupt 
                                              processing.  Original IOB will
                                              be returned with status 
                                              reflecting outcome of original
                                              I/O  */
#define HIM_IOB_ABORTED_TRANSPORT_MODE_CHANGE 0x4B /* An I/O operating mode
                                              change forced the return of 
                                              this IOB.                     */

/* Miscellaneous: */
#define HIM_IOB_HOST_ADAPTER_FAILURE     0x50 /* Failure within host adapter HW
                                              caused command to fail.       */
#define HIM_IOB_TARGET_RESET_FAILED      0x51 /* Attempt to reset target 
                                              failed, or target did not respond
                                              to the reset request */
                                              
#define HIM_IOB_PROTOCOL_SPECIFIC        0x60 /* status is in protocol-specific    
                                              portion of IOB                */

/* Target Mode Abort Statuses */
#define HIM_IOB_ABORTED_ABTS_RECVD       0x70 /* IOB was aborted due to an
                                              Abort Task Set request received
                                              from an Initiator.            */  
#define HIM_IOB_ABORTED_ABT_RECVD        0x71 /* IOB was aborted due to an Abort 
                                              Task request received from an 
                                              Initiator.                    */   
#define HIM_IOB_ABORTED_TR_RECVD         0x72 /* IOB was aborted due to a Target
                                              Reset request received from an
                                              Initiator.                    */    
#define HIM_IOB_ABORTED_CTS_RECVD        0x73 /* IOB was aborted due to a Clear
                                              Task Set request received from an 
                                              Initiator.                    */
#define HIM_IOB_ABORTED_TT_RECVD         0x74 /* IOB was aborted due to a Terminate
                                              Task request received from an 
                                              Initiator.                    */
#define HIM_IOB_ABORTED_TMF_RECVD        0x75 /* A Task Management Function was
                                              received while processing a
                                              HIM_REESTABLISH_STATUS or 
                                              HIM_REESTABLISH_INTERMEDIATE 
                                              IOB.                          */
/* Target Mode Errors */
#define HIM_IOB_TARGETCOMMANDBUFFER_OVERRUN  0x80 /* Information received from an 
                                                  Initiator into the targetCommand
                                                  buffer was truncated (due to value of
                                                  IOB field targetCommandBufferSize).  */
#define HIM_IOB_OUT_OF_ORDER_TRANSFER_REJECTED  0x81
                                                  /* A HIM_REESTABLISH_STATUS or
                                                  HIM_REESTABLISH_INTERMEDIATE IOB 
                                                  with the outOfOrderTransfer flagsIob
                                                  set to 1 was rejected by an Initiator,
                                                  implying the Initiator does not 
                                                  support this type of request.
                                                                            */   
#define HIM_IOB_INITIATOR_DETECTED_PARITY_ERROR 0x82
                                                 /* A parity error was detected by
                                                 the Initiator. This error is returned 
                                                 for IOB function codes 
                                                 HIM_REESTABLISH_STATUS or 
                                                 HIM_REESTABLISH_INTERMEDIATE only.
                                                                            */
#define HIM_IOB_INITIATOR_DETECTED_ERROR        0x83
                                                 /* Some error other than parity
                                                 error was detected by the Initiator.
                                                 This error is returned for IOB function
                                                 codes HIM_REESTABLISH_STATUS or 
                                                 HIM_REESTABLISH_INTERMEDIATE only.  
                                                                           */
                                                                                                                                              
#define HIM_IOB_INVALID_MESSAGE_REJECT          0x84
                                                 /* HIM received a message reject in
                                                 response to a mandatory message.
                                                                           */
#define HIM_IOB_INVALID_MESSAGE_RCVD            0x85
                                                 /* HIM received an unrecognized or 
                                                 unsupported message and issued a message 
                                                 reject message and the initiator 
                                                 responded by deasserting ATN.
                                                                           */  
#define HIM_IOB_ABORTED_CHANNEL_FAILED          0x86  /* IOB was aborted due to unrecoverable SCSI bus error */

/***************************************************************************
* Definitions for HIM_ADAPTER_PROFILE
***************************************************************************/
#define HIM_MAX_WWID_SIZE 32
#define HIM_VERSION_ADAPTER_PROFILE 10

typedef struct HIM_ADAPTER_PROFILE_ {
    HIM_UINT32  AP_Version;             /* Profile version #. */
    HIM_UINT8   AP_Transport;            /* SCSI, SSA, etc. */
    HIM_UEXACT8 AP_WorldWideID[HIM_MAX_WWID_SIZE];  /* Unique ID of adapter */
    HIM_UINT8   AP_WWIDLength;          /* number of valid bytes in wwid */
    HIM_UINT8   AP_NumBuses;            /* # of physical channels, usually 1 */
    HIM_BOOLEAN AP_VirtualDataAccess;   /* HIM uses virt addr to access data */
    HIM_BOOLEAN AP_NeedPhysicalAddr;    /* HIM uses OSMGetBusAddress, etc. */
    HIM_BOOLEAN AP_BusMaster;           /* Adapter is a bus master device */ 
    HIM_UEXACT32 AP_AlignmentMask;      /* Align. req's for s/g lists, iob
                                         * reserve, and buffer descriptors */
    HIM_UINT8   AP_AddressableRange;    /* # of bits adapter can address */
    HIM_UINT8   AP_GroupNum;            /* Used by RAID-driver re-entrancy */
    HIM_UINT16  AP_AutoConfigTimeout;   /* HIM recommendedation, in seconds */
    HIM_UINT16  AP_MaxIOHandles;        /* HIM's # of I/O handles needed */
    HIM_BOOLEAN AP_TargetMode;          /* If 1, Target Mode enabled */
    HIM_BOOLEAN AP_InitiatorMode;       /* Default 1 for normal operation */
    HIM_BOOLEAN AP_CleanSG;             /* If 1, HIM returns s/g list 
                                         * unchanged */
    HIM_BOOLEAN AP_Graphing;            /* If 1, HIM supports graph node */
    HIM_BOOLEAN AP_CannotCross4G;       /* If 1, DMA cannot cross 32-bit 
                                         * boundary */
    HIM_BOOLEAN AP_BiosActive;          /* If 1, BIOS is active on adapter */
    HIM_BOOLEAN AP_CacheLineStreaming;  /* If 1, cache threshold enable */
    HIM_UINT8   AP_ExtendedTrans;       /* If 1, uses has selected extended 
                                         * translation for new drives */
    HIM_UINT8   AP_MemoryMapped;        /* I/O or memory mapped */
    HIM_UINT32  AP_MaxTargets;          /* Max targets adapter can support */
    HIM_UINT32  AP_MaxInternalIOBlocks; /* Max internal I/O control blocks */
    HIM_UINT32  AP_MaxSGDescriptors;    /* Max SG list elements allowed */
    HIM_UINT32  AP_MaxTransferSize;     /* Max transfer, in bytes, per IOB */
    HIM_UINT32  AP_StateSize;           /* Size of swappable hardware state */
    HIM_UINT32  AP_IOBReserveSize;      /* Size of buffer iobRserve */
    HIM_BOOLEAN AP_FIFOSeparateRWThreshold; /* If HIM_TRUE, the adapter (and the
                                             * HIM) support a separate data FIFO
                                             * threshold control for each direction 
                                             * (write to main memory or read from 
                                             * main memory). 
                                             */
    HIM_BOOLEAN AP_FIFOSeparateRWThresholdEnable; /* This field may be used by 
                                                   * the OSM to select the method
                                                   * of adjusting the FIFO threshold.
                                                   * It is only meaningful if
                                                   * AP_FIFOSeparateRWThreshold returns
                                                   * a value of HIM_TRUE. 
                                                   * If HIM_TRUE, then
                                                   * AP_FIFOWriteThreshold and
                                                   * AP_FIFOReadThreshold are used to
                                                   * adjust the FIFO threshold. If
                                                   * HIM_FALSE, the default, the
                                                   * AP_FIFOThreshold field is used
                                                   * to adjust the FIFO threshold.
                                                   */ 
    HIM_UINT8   AP_FIFOWriteThreshold;  /* The percentage of FIFO available to
                                         * trigger a transfer from the adapter
                                         * to system memory.
                                         */      
    HIM_UINT8   AP_FIFOReadThreshold;   /* The percentage of FIFO available to
                                         * trigger a transfer from main memory to
                                         * the adapter.
                                         */
    HIM_UINT8   AP_FIFOThreshold;       /* Percentage of FIFO available 
                                         * for DMA to trigger, has meaning only when 
                                         * AP_FIFOSeparateRWThreshold is false
                                         */
    HIM_UINT32  AP_ResetDelay;          /* In milliseconds */
    HIM_UINT16  AP_HIMVersion;          /* version # of HIM implementation */
    HIM_UINT8   AP_HardwareVersion;     /* main hardware version number */
    HIM_UINT8   AP_HardwareVariant;     /* minor hardware version number */
    HIM_UINT16  AP_LowestScanTarget;    /* Lowest TP_ScanOrder on this
                                         * adapter */

    HIM_UINT8   AP_AllocBusAddressSize; /* Maximum bus address size allowed for */
                                        /* locked (DMAable) memory that is */
                                        /* allocated in HIMSetMemoryPointer. */
                                        /* (i.e. memory of category HIM_MC_LOCKED) */
    HIM_UINT8   AP_indexWithinGroup;    /* adapter's index within a group */
   
    HIM_BOOLEAN AP_CmdCompleteIntrThresholdSupport;
                               /* In supported F/W : */
                               /*   HIM_TRUE =Enable Interrupt Reduction Logic  */
                               /*   HIM_FALSE=Disable Interrupt Reduction Logic  */                                        
    HIM_UINT8   AP_IntrThreshold;       /* Threshold count for interrupt posting */
    HIM_BOOLEAN AP_SaveRestoreSequencer;/* If HIM_TRUE, the default, the 
                                         * and sequencer will be saved to 
                                         * and restored from the pState area
                                         * provided by the OSM in HIMSaveState
                                         * and HIMRestoreState. If HIM_FALSE,
                                         * sequencer will not be saved on 
                                         * HIMSaveState and will be
                                         * reinitialized on HIMRestoreState.
                                         */
    HIM_BOOLEAN AP_ClusterEnabled;      /* This field is specific to AAC (Adapter
                                         * Array Controller) products.
                                         * If this value is HIM_TRUE the adapter
                                         * may be used in an Adaptec specific 
                                         * cluster environment.
                                         */       
    HIM_BOOLEAN AP_InitializeIOBus;     /* Condition I/O bus (e.g. SCSI bus reset) */

    /* The following are Target Mode profiles, i.e. these profiles only
     * have meaning when AP_TargetMode is set to 1. */
    HIM_UINT32  AP_TargetNumNexusTaskSetHandles;
                                        /* Number of nexus task set handles  
                                         * that can be used by the adapter */
    HIM_UINT32  AP_TargetNumNodeTaskSetHandles;
                                        /* Number of node task set handles
                                           taht can be used by the adapter */     

    HIM_BOOLEAN AP_TargetDisconnectAllowed;
                                        /* 1=HIM will disconnect from
                                         * the bus on receipt of a new
                                         * command (if Identify msg
                                         * DiscPriv=1).
                                         * 0=No Disconnect */

    HIM_BOOLEAN AP_TargetTagEnable;     /* 1=tagged requests accepted
                                         * 0=tagged requests rejected */
    HIM_BOOLEAN AP_OutOfOrderTransfers; /* 1=the adapter supports 
                                         * random buffer access, e.g.
                                         * issuing Modify Data pointer
                                         * msg in Parallel SCSI */                                         
    HIM_UINT32  AP_NexusHandleThreshold; 
                                        /* OSMEvent notification is returned
                                         * when this value = number of  
                                         * available NexusTSHs. A value of  
                                         * 0, the default, disables this
                                         * OSMEvent. */
    HIM_UINT32  AP_EC_IOBThreshold;     /* OSMEvent notification is returned
                                         * when this value = number of  
                                         * available Establish Connection IOBs.
                                         * A value of 0, the default, disables this
                                         * OSMEvent. */                         
    HIM_UINT32  AP_TargetAvailableEC_IOBCount;
                                        /* The number of available 
                                         * HIM_ESTABLISH_CONNECTION IOBs 
                                         * queued to the CHIM. */
    HIM_UINT32  AP_TargetAvailableNexusCount;
                                        /* The number of available Nexus Task
                                         * Set Handles. This is the number of 
                                         * nexusTSHs which are not in use by
                                         * the CHIM/OSM (i.e. are available
                                         * for a new connection). */
    struct AP_SCSI_TARGET_OPT_TASKMANAGEMENT_ {
        /* This structure is really protocol specific (SCSI) */
        /* Mask of optional task management
         * functions supported by CHIM */   
        HIM_BOOLEAN  AP_SCSITargetAbortTask;         /* Abort Tag in SCSI-2 */
        HIM_BOOLEAN  AP_SCSITargetClearTaskSet;      /* Clear Queue in SCSI-2 */
        HIM_BOOLEAN  AP_SCSITargetTerminateTask;     /* Terminate I/O Process in SCSI-2 */
        HIM_BOOLEAN  AP_SCSI3TargetClearACA;         /* SCSI-3 only */
        HIM_BOOLEAN  AP_SCSI3TargetLogicalUnitReset; /* SCSI-3 only */
    } AP_SCSITargetOptTmFunctions;
    HIM_UINT8   AP_TargetNumIDs;        /* The number of physical addresses
                                         * which this adapter can respond
                                         * as a target.
                                         */
    HIM_UINT32  AP_TargetInternalEstablishConnectionIOBlocks;
                                        /* The number of Internal I/O blocks
                                         * reserved for receiving new requests
                                         * from Initiators. This value must be
                                         * less than the value of 
                                         * AP_MaxInternalIOBlocks as these
                                         * I/O blocks are allocated from 
                                         * the same available pool. This
                                         * field shall only be modified
                                         * prior to invoking HIMInitialize.
                                         */     
    /* End of Target Mode profiles */                                      
    union {
        struct HIM_SCSI_ADAPTER_PROFILE_ {
            HIM_UEXACT32  AP_SCSIForceWide;     /* Force Wide nego on, by ID */
            HIM_UEXACT32  AP_SCSIForceNoWide;   /* Force Wide nego off, by ID*/
            HIM_UEXACT32  AP_SCSIForceSynch;    /* Force Sync nego on, by ID */
            HIM_UEXACT32  AP_SCSIForceNoSynch;  /* Force Sync nego off, by ID*/
            HIM_UINT8     AP_SCSIAdapterID;     /* SCSI ID of Adapter */
            HIM_UINT8     AP_SCSISpeed;         /* 0-SCSI, 1-Fast, 2-Ultra */
            HIM_UINT8     AP_SCSIWidth;         /* # of bits in data bus: 
                                                 * 8, 16, 32 */
            HIM_UINT8     AP_SCSINumberLuns[32];/* The number of LUN's to scan,
                                                   indexed by SCSI ID */
            HIM_BOOLEAN   AP_SCSIDisableParityErrors;
            HIM_UINT16    AP_SCSISelectionTimeout; 
            HIM_UINT8     AP_SCSITransceiverMode;/* The current transceiver
                                                  * mode of the SCSI bus
                                                  * connected to the adapter.
                                                  * One of:
                                                  *    HIM_SCSI_UNKNOWN_MODE
                                                  *    HIM_SCSI_LVD_MODE
                                                  *    HIM_SCSI_SE_MODE
                                                  *    HIM_SCSI_HVD_MODE
                                                  */
            HIM_UINT8     AP_SCSIDomainValidationMethod; /* Specifies domain */
                                                         /* validation method*/
                                                         /* for PAC/HIM_PROBE*/

            HIM_BOOLEAN   AP_SCSIOSMResetBusUseThrottledDownSpeedforDV;
                                        /* 1=HIM will throttle down 
                                         * so when domain validation occurs
                                         * we will go with throttled rate */
                                         
                        
            HIM_BOOLEAN   AP_SCSIPPRSupport;     /* HIM_TRUE  - Adapter support Parallel
                                                  * Protocol Request (PPR) message.
                                                  * HIM_FALSE - No PPR suppport.
                                                  */
            HIM_BOOLEAN   AP_SCSIExpanderDetection;
                                                 /* HIM_TRUE - The SCSI expander
                                                    detection logic is enable.
                                                    The HIM will report via the
                                                    TP_SCSIConnectedViaExpander
                                                    target profile field, whether
                                                    the target is connected to the
                                                    adapter via a SCSI expander.
                                                  */  
            HIM_UINT8     AP_SCSICurrentSensingStat_PL; /* low byte of primary seqment */ 
            HIM_UINT8     AP_SCSICurrentSensingStat_PH; /* high byte of primary seqment */ 
            HIM_UINT8     AP_SCSICurrentSensingStat_SL; /* low byte of secondary seqment */ 
            HIM_UINT8     AP_SCSICurrentSensingStat_SH; /* high byte of secondary seqment */ 
                                                /* HA termination status retured from
                                                   on board current sensing circuitry
                                                   0 - Termination Okay
                                                   1 - Over Terminated
                                                   2 - Under Terminated
                                                   3 - Invalid
                                                 */

            HIM_BOOLEAN   AP_SCSISuppressNegotiationWithSaveStateRestoreState;
                                                /* HIM_TRUE=No force renegotiate after  
                                                   RestoreState when the compile option
                                                   POWER_MANAGEMENT_SAVE_RESTORE is on 
                                                 */

            /* The following are Target Mode profiles */
            HIM_UINT8     AP_SCSIHostTargetVersion;
                                                /* Either HIM_SCSI_2 or HIM_SCSI_3 */
            HIM_BOOLEAN   AP_SCSI2_IdentifyMsgRsv;
                                                /* 1=HIM responds with Message
                                                 * Reject if SCSI_2 */
            HIM_BOOLEAN   AP_SCSI2_TargetRejectLuntar;
                                                /* HIM handling of the LUNTAR bit set
                                                 * in a SCSI2 Identify message. 
                                                 * 1=HIM responds with Message Reject.
                                                 * 0=HIM passes in Nexus and accepts 
                                                 * request. */
            HIM_UINT8     AP_SCSIGroup6CDBSize; /* Expected number of bytes
                                                 * in a group 6 CDB. Default is
                                                 * 12. */ 
            HIM_UINT8     AP_SCSIGroup7CDBSize; /* Expected number of bytes 
                                                 * in a group 7 CDB. Default is
                                                 * 12. */
            HIM_BOOLEAN   AP_SCSITargetIgnoreWideResidue;
                                                /* If 1, and AP_TargetMode is
                                                 * enabled, the adapter will issue
                                                 * the Ignore Wide Residue message,
                                                 * when appropriate during wide data
                                                 * transfers. */           
            HIM_BOOLEAN   AP_SCSITargetEnableSCSI1Selection;
                                                /* if 1, and AP_TargetMode
                                                 * is enabled, the HIM accepts a 
                                                 * selection without an identify
                                                 * message. If 0, the default, the 
                                                 * HIM transitions the bus to bus
                                                 * free when an identify message is
                                                 * not received with a selection. */   
            HIM_BOOLEAN   AP_SCSITargetInitNegotiation; 
                                                /* If 1, and AP_TargetMode
                                                 * is enabled, the adapter
                                                 * will initiate negotiation.
                                                 * If 0, the default, and
                                                 * AP_TargetMode is enabled, the
                                                 * adapter will rely on the Initiator
                                                 * to initiate negotiation.
                                                 */
            HIM_UINT16    AP_SCSITargetMaxSpeed; 
                                                /* Maximum adapter speed in
                                                 * tenths of Mtransfers/second when 
                                                 * operating in target mode.
                                                 */ 
            HIM_UINT16    AP_SCSITargetDefaultSpeed;
                                                /* Speed to attempt to negotiate
                                                 * when operating in target mode.
                                                 */ 
            HIM_UINT8     AP_SCSITargetMaxOffset;
                                                /* Maximum adapter synchronous offset
                                                 * supported when operating in target
                                                 * mode.
                                                 */    
            HIM_UINT8     AP_SCSITargetDefaultOffset;
                                                /* Offset to attempt to negotiate when 
                                                 * operating in target mode.
                                                 */
            HIM_UINT8     AP_SCSITargetMaxWidth;
                                                /* Maximum adapter data bus width 
                                                 * supported when operating in
                                                 * target mode.
                                                 */
            HIM_UINT8     AP_SCSITargetDefaultWidth;
                                                /* Width to attempt to negotiate
                                                 * when operating in target mode.
                                                 */                                        
            HIM_UEXACT16  AP_SCSITargetAdapterIDMask;
                                                /* Mask of SCSI IDs to which this
                                                 * adapter will respond to selection.
                                                 */ 
            HIM_PAD       AP_SCSITargetUnused[2];

            /* End of Target Mode profiles */

            HIM_BOOLEAN   AP_SCSIForceSTPWLEVELtoOneForEmbeddedChips;
                                                /* HIM_TRUE=Force STPWLEVEL bit to one  
                                                   in DEVCONFIG PCI configuration register
                                                   for embedded chips
                                                 */

        } TS_SCSI;
        /* Currently nothing defined for either SSA or Fibre Channel */
    } himu;
    HIM_BOOLEAN AP_OverrideOSMNVRAMRoutines;
                                        /* 1=HIM will will not call an 
                                         * OSM NVRAM routine prior to accessing
                                         * the local NVRAM */
    /* OEM specific adapter profile fields */
    /* Note; only non-general fields should be placed here. */
    /* Add a separate structure for each OEM. */
    struct HIM_OEM1_ADAPTER_PROFILE_ {
        union {
            struct HIM_OEM1_SCSI_ADAPTER_PROFILE_ {
                HIM_UINT8     AP_SCSICmndPhaseDelay; /* The number of microseconds
                                                      * the adapter will delay in
                                                      * command phase.
                                                      */
                HIM_UINT8     AP_SCSIDataPhaseDelay; /* The number of microseconds
                                                      * the adapter will delay in
                                                      * data phase.
                                                      */
                HIM_UINT8     AP_SCSIStatusPhaseDelay; /* The number of microseconds
                                                      * the adapter will delay in
                                                      * status phase.
                                                      */
            } TS_SCSI;
        } himu; 
    } AP_OEM1Specific;

} HIM_ADAPTER_PROFILE ;

/***************************************************************************
* Definitions for HIM_TARGET_PROFILE
***************************************************************************/
#define HIM_VERSION_TARGET_PROFILE 7
typedef struct HIM_TARGET_PROFILE_ {
    HIM_UINT32  TP_Version;         /* Version # of target profile */
    HIM_UINT8   TP_Transport;       /* Duplicate of AP_Transport   */
    HIM_UINT8   TP_Protocol;        /* I/O protocol of target --
                                     * which commands and sense types
                                     * are understood by target    */
     
    HIM_UEXACT8 TP_WorldWideID[HIM_MAX_WWID_SIZE]; /* Unique ID of SSA, FC, 
                                                      or SCAM target */
    HIM_UINT8   TP_WWIDLength;      /* number of valid bytes in wwid */
    HIM_UINT16  TP_ScanOrder;       /* scan order determined by bios or 
                                       config utility */
    HIM_UINT8   TP_BusNumber;       /* for multiple-bus adapters */
    HIM_UINT8   TP_SortMethod;      /* 0 = FIFO, 1 = elevator */
    HIM_UINT16  TP_MaxActiveCommands; /* maximum commands active at the
                                       * target simultaneously */
    HIM_BOOLEAN TP_MaxActiveShared;   /* If 1, max active is per target, 
                                       * If 0, per target/lun (targetTSH)*/
    HIM_BOOLEAN TP_TaggedQueuing;   /* If true, tagged queuing is active */
    HIM_BOOLEAN TP_HostManaged;     /* This field is specific to AAC (Adaptec
                                     * Array Controller) products. 
                                     * If this value is HIM_TRUE, the host
                                     * shall manage the device. Requests 
                                     * issued to the target are passed
                                     * directly to the device without
                                     * processing by the AAC (i.e. pass through
                                     * mode). If this value is HIM_FALSE, the
                                     * target is managed by the AAC.
                                     */
    union {
        struct HIM_SCSI_TARGET_PROFILE_ {
            /* Note: All information in this SCSI section is ID-specific, 
               and is shared across all LUN's for a given ID.  An OSM 
               alteration to any target profile will be reflected 
               in the profiles of all other LUN's on the same ID. */
            HIM_UINT8   TP_SCSI_ID;
            HIM_UINT8   TP_SCSILun;
            HIM_UINT8   TP_SCSIScamSupport;   /* Level of SCAM on target */
            HIM_UINT16  TP_SCSIMaxSpeed;      /* Maximum adapter speed in
                                                 tenths of Mtransfers/second */
            HIM_UINT16  TP_SCSIDefaultSpeed;  /* Speed to attempt to negot. */   
            HIM_UINT16  TP_SCSICurrentSpeed;  /* Current negotiation speed */
            HIM_UINT8   TP_SCSIMaxOffset;       
            HIM_UINT8   TP_SCSIDefaultOffset;
            HIM_UINT8   TP_SCSICurrentOffset;
            HIM_UINT8   TP_SCSIMaxWidth;
            HIM_UINT8   TP_SCSIDefaultWidth;
            HIM_UINT8   TP_SCSICurrentWidth;
            HIM_BOOLEAN TP_SCSIDisconnectAllowed;   
            HIM_UINT8   TP_SCSIDomainValidationMethod;
            HIM_BOOLEAN TP_SCSIDomainValidationFallBack;
            HIM_UINT8   TP_SCSITransitionClocking; /* Indicates modes    */
                                                   /* supported by target*/
            HIM_UINT8   TP_SCSIDefaultProtocolOption;
            HIM_UINT8   TP_SCSICurrentProtocolOption;
            HIM_BOOLEAN TP_SCSIConnectedViaExpander; /* HIM_TRUE - The target is
                                                        connected to the adapter
                                                        via an expander.
                                                      */
        } TS_SCSI;
        struct HIM_SSA_TARGET_PROFILE_ {
            HIM_UINT8   TP_SSALun;
        } TS_SSA;
        struct HIM_FC_TARGET_PROFILE_ {
            HIM_UINT8   TP_FCLun;
        } TS_FC;
    } himu;
} HIM_TARGET_PROFILE;

/***************************************************************************
* Definitions for HIM_NEXUS_PROFILE
***************************************************************************/
#define HIM_VERSION_NEXUS_PROFILE 2
typedef struct HIM_NEXUS_PROFILE_ {
    HIM_UINT32  XP_Version;         /* Version # of nexus profile */
    HIM_UINT8   XP_Transport;       /* Duplicate of AP_Transport   */
    HIM_UINT8   XP_Protocol;        /* I/O protocol of target --
                                     * which commands and sense types
                                     * are understood by target    */
    HIM_UINT8   XP_BusNumber;       /* for multiple bus adapters   */
    HIM_TASK_SET_HANDLE
                XP_AdapterTSH;      /* TSH of adapter which received the
                                       request */
   HIM_TASK_SET_HANDLE
                XP_NodeTSH;         /* TSH of node associated with this 
                                       request */
   void HIM_PTR 
                XP_OSMNodeContext;  /* OSM reference passed to node TSH */
   HIM_BOOLEAN  XP_LastResource;    /* When set to 1, the HIM has used the 
                                     * last available nexus TSH or 
                                     * Establish Connection IOB. */                                                  
    union {
        struct HIM_SCSI_NEXUS_PROFILE_ {
            /* Note: All information in this SCSI section is ID-specific */ 
            HIM_UINT8   XP_SCSI_ID;
            HIM_UINT8   XP_SCSILun;
            HIM_UINT8   XP_SCSIQueueTag;      /* The received queue tag */
            HIM_UINT8   XP_SCSIQueueType;     /* The type of queuing requested */
            HIM_BOOLEAN XP_SCSILunTar;        /* 0=Lun, 1=Target routine. SCSI_2
                                                 only  */   
            HIM_BOOLEAN XP_SCSIDisconnectAllowed;
                                              /* 0=No Disconnect,
                                                 1=Disconnect Allowed */
            HIM_BOOLEAN XP_SCSIBusHeld;       /* 1=HIM has not relinquished 
                                                 the SCSI bus */
            HIM_BOOLEAN XP_SCSI1Selection;    /* 1 = SCSI1 selection - 
                                                 no identify message */
            HIM_UINT8   XP_SCSISelectedID;    /* The SCSI ID that the adapter
                                               * was selected as a target for
                                               * this nexus.
                                               */
      } TS_SCSI;
      /* Nothing defined for SSA or Fibre Channel transports */
        
    } himu;
} HIM_NEXUS_PROFILE;

/***************************************************************************
* Definitions for HIM_NODE_PROFILE
***************************************************************************/
#define HIM_VERSION_NODE_PROFILE 1
typedef struct HIM_NODE_PROFILE_ {
    HIM_UINT32  NP_Version;         /* Version # of target profile */
    HIM_UINT8   NP_Transport;       /* Duplicate of AP_Transport   */
    HIM_UINT8   NP_Protocol;        /* I/O protocol of target --
                                     * which commands and sense types
                                     * are understood by target    */
    HIM_UINT8   NP_BusNumber;       /* for multiple-bus adapters */
    union {
        struct HIM_SCSI_NODE_PROFILE_ {
            /* Note: All information in this SCSI section is ID-specific, 
               and is shared across all LUN's for a given ID.  An OSM 
               alteration to any target profile will be reflected 
               in the profiles of all other LUN's on the same ID. */
            HIM_UINT8   NP_SCSI_ID;
            HIM_UINT16  NP_SCSIMaxSpeed;      /* Maximum adapter speed in
                                                 tenths of Mtransfers/second */
            HIM_UINT16  NP_SCSIDefaultSpeed;  /* Speed to attempt to negotiate */   
            HIM_UINT16  NP_SCSICurrentSpeed;  /* Current negotiation speed */
            HIM_UINT8   NP_SCSIMaxOffset;       
            HIM_UINT8   NP_SCSIDefaultOffset;
            HIM_UINT8   NP_SCSICurrentOffset;
            HIM_UINT8   NP_SCSIMaxWidth;
            HIM_UINT8   NP_SCSIDefaultWidth;
            HIM_UINT8   NP_SCSICurrentWidth;
        } TS_SCSI;
    } himu;
} HIM_NODE_PROFILE;

/***************************************************************************
*  #defines used by adapter and target profiles
***************************************************************************/
/* AP_Transport, TP_Transport */
#define HIM_TRANSPORT_SCSI    1
#define HIM_TRANSPORT_SSA     2
#define HIM_TRANSPORT_FC      3

/* AP_Memorymapped           */
#define HIM_IOSPACE           0
#define HIM_MEMORYSPACE       1
#define HIM_MIXED_RANGES      2

/* AP_ExtendedTrans          */
#define HIM_STANDARD_TRANS    0
#define HIM_EXTENDED_TRANS    1
#define HIM_UNKNOWN_TRANS     2
     
/* AP_SCSISpeed              */
#define HIM_SCSI_NORMAL_SPEED    0
#define HIM_SCSI_FAST_SPEED      1
#define HIM_SCSI_ULTRA_SPEED     2
#define HIM_SCSI_ULTRA2_SPEED    3
#define HIM_SCSI_ULTRA160M_SPEED 4

/* AP_SCSITransceiverMode */
#define HIM_SCSI_UNKNOWN_MODE 0 
#define HIM_SCSI_LVD_MODE     1
#define HIM_SCSI_SE_MODE      2
#define HIM_SCSI_HVD_MODE     3

/* AP_SCSIDomainValidationMethod */
#define HIM_SCSI_DISABLE_DOMAIN_VALIDATION  0
#define HIM_SCSI_BASIC_DOMAIN_VALIDATION    1
#define HIM_SCSI_ENHANCED_DOMAIN_VALIDATION 2
#define HIM_SCSI_MARGINED_DOMAIN_VALIDATION 3

/* AP_SCSICurrentSensingStat_XX */
#define HIM_SCSI_TERMINATION_OK      0
#define HIM_SCSI_OVER_TERMINATION    1
#define HIM_SCSI_UNDER_TERMINATION   2
#define HIM_SCSI_INVALID_TERMINATION 3

/* AP_SCSIHostTargetVersion  */
#define HIM_SCSI_2            2
#define HIM_SCSI_3            3

/* AP_Protocol               */
#define HIM_PROTOCOL_SCSI     1

/* AP_TargetNumIDs */
#define HIM_MAX_SCSI_ADAPTER_IDS   15

/* TP_SCSIScamsupport        */
#define HIM_SCAM_TOLERANT     0
#define HIM_SCAM1             1
#define HIM_SCAM2             2
#define HIM_SCAM_INTOLERANT   0xff

/* TP_SortMethod             */
#define HIM_FIFO              0
#define HIM_SORT_ELEVATOR     1

/* tp_scsicurrent speed/offset/width */
#define HIM_SCSI_OFFSET_UNKNOWN 0xff
#define HIM_SCSI_WIDTH_UNKNOWN  0xff
#define HIM_SCSI_SPEED_UNKNOWN  0xffff

/* TP_SCSITransitionClocking */
#define HIM_SCSI_ST_CLOCKING     0
#define HIM_SCSI_DT_CLOCKING     1
#define HIM_SCSI_ST_DT_CLOCKING  2

/* TP_SCSIDefault/CurrentProtocolOption */
#define HIM_SCSI_PROTOCOL_OPTION_UNKNOWN  0
#define HIM_SCSI_NO_PROTOCOL_OPTION       1
#define HIM_SCSI_ST_DATA                  2
#define HIM_SCSI_DT_DATA_WITH_CRC         3

/* Null Pointer define -used in XP_OSMNodeContext */
#define  HIM_NULL   0
/****************************************************************************/
/* specialized return codes and parameters                                  */
/****************************************************************************/
/* General results           */
#define  HIM_SUCCESS          0
#define  HIM_FAILURE          1

/* HIMVerifyAdapter          */
#define  HIM_ADAPTER_NOT_SUPPORTED    5 

/* HIMCheckTargetTSCBNeeded  */
#define  HIM_NO_NEW_DEVICES       0
#define  HIM_NEW_DEVICE_DETECTED  1

/* index for HIMCreateTargetTSCB */
#define  HIM_PROBED_TARGET    0xffff

/* HIMValidateTargetTSH      */
#define  HIM_TARGET_VALID     0
#define  HIM_TARGET_CHANGED   1
#define  HIM_TARGET_INVALID   2

/* HIMClearTargetTSH, HIMAdjustTargetProfile  */
#define  HIM_TARGET_NOT_IDLE  6

/* HIMPollIRQ, HIMFrontEndISR    */
#define  HIM_NOTHING_PENDING     0
#define  HIM_INTERRUPT_PENDING   1
#define  HIM_LONG_INTERRUPT_PENDING 2

/* HIMPowerEvent             */
#define  HIM_APM_SUSPEND      1
#define  HIM_APM_STANDBY      2
#define  HIM_APM_RESUME       3

/* HIMAdjustAdapterProfile   */
#define  HIM_ADAPTER_NOT_IDLE 2

/* HIMAdjustAdapterProfile, HIMAdjustTargetProfile,
 * HIMAdjustNodeProfile, HIMSetConfiguration
 */
#define  HIM_ILLEGAL_CHANGE   7

/* HIMAdjustNodeProfile       */
#define  HIM_NODE_NOT_IDLE    9 

/* HIMClearNexusTSH specific results */
#define  HIM_NEXUS_HOLDING_IOBUS 2
#define  HIM_NEXUS_NOT_IDLE      3

/* HIMGetNVOSMSegment              */
#define  HIM_NO_OSM_SEGMENT        2

/* HIMPutNVData                    */
#define  HIM_WRITE_NOT_SUPPORTED   3
#define  HIM_WRITE_PROTECTED       8


/* HIMQueueIOB: targetCommand points to this structure
 *              if HIM_PROTOCOL_AUTO_CONFIG */
typedef struct HIM_AUTO_CONFIG_
{
    HIM_UEXACT32    reason; /* see below for values */
    void HIM_PTR    pEventContext; /* previously passed to OSM via OSMEvent, 
                                    * else 0 */
} HIM_AUTO_CONFIG;

/* HIMQueueIOB: errorData points to this union in special cases */
typedef union 
{
    struct resetBus_
    {
        HIM_TASK_SET_HANDLE targetTSH;
    } resetBus;
} HIM_SPECIAL_ERRORDATA;

/* HIMQueueIOB: transport-specific, if OSM wishes to include it */
typedef struct HIM_TS_SCSI_ {
    HIM_UEXACT8 protocolStatus;
    HIM_PAD unused1[3];
    HIM_BOOLEAN forceUntagged;
    HIM_BOOLEAN disallowDisconnect;
    HIM_BOOLEAN forceSync;
    HIM_BOOLEAN forceAsync;
    HIM_BOOLEAN forceWide;
    HIM_BOOLEAN forceNarrow;
    HIM_BOOLEAN forceReqSenseNego;
    HIM_BOOLEAN parityEnable;
    HIM_UEXACT8 scsiID;
    HIM_PAD unused2[3];
    HIM_UEXACT8 LUN[8];
    HIM_BOOLEAN nsxCommunication;  /* When set to HIM_TRUE indicates that NSX
                                    * repeater information is to be obtained.
                                    */ 
    HIM_BUFFER_DESCRIPTOR nsxData; /* NSX data buffer */
    HIM_UINT16 nsxStatus;          /* taskStatus result for NSX communication */
                                   /* currently only x results:
                                    *    HIM_IOB_DATA_OVERUNDERRUN
                                    *    HIM_IOB_GOOD
                                    */
    HIM_UINT16 nsxDataLength;      /* number of bytes of NSX data received in buffer */     
    HIM_BOOLEAN suppressNego;      /* When set to HIM_TRUE indicates that 
                                    * negotiation should be suppressed for
                                    * given target.
                                    */
    HIM_BOOLEAN dvIOB;             /* When set to HIM_TRUE indicates this is a
                                    * domain validation IOB.
                                    */ 
} HIM_TS_SCSI;

/* Transport specific structure for HIM_IOB functions HIM_ENABLE_ID
 * and HIM_DISABLE_ID
 */
typedef struct HIM_TS_ID_SCSI_ {
    HIM_UEXACT16 scsiIDMask;
    HIM_PAD unused[2];
} HIM_TS_ID_SCSI;
 
/* Reason codes */
#define  HIM_AUTO_INIT        0  /* called during initialization */        
#define  HIM_AUTO_EXTERNAL    1  /* external rescan request from os */     
#define  HIM_AUTO_OSM_RESET   2  /* after osm-requested reset */           
#define  HIM_AUTO_BUS_RESET   3  /* after HIM_EVENT_IO_BUS_RESET */        
#define  HIM_AUTO_CONFIG_REQ  4  /* after HIM_EVENT_AUTO_CONFIG_REQUIRED */
#define  HIM_AUTO_HA_FAILED   5  /* after HIM_EVENT_HA_FAILED     */      
#define  HIM_AUTO_POWER       6  /* after power resume event, or     
                                  * any other use of HIM_QUIESCE  */
#define  HIM_AUTO_RETRY       7  /* HIM returned bad status to last attempt */
#define  HIM_AUTO_TRANSPORT_MODE_CHANGE  8  /* after HIM_EVENT_TRANSPORT_MODE_CHANGE */

/* Event codes for OSMEvent() */
#define HIM_EVENT_IO_CHANNEL_RESET     1
#define HIM_EVENT_AUTO_CONFIG_REQUIRED 2
#define HIM_EVENT_HA_FAILED            3
#define HIM_EVENT_OSMFREEZE            4
#define HIM_EVENT_OSMUNFREEZE          5
#define HIM_EVENT_NEXUSTSH_THRESHOLD   6
#define HIM_EVENT_EC_IOB_THRESHOLD     7
#define HIM_EVENT_TRANSPORT_MODE_CHANGE 8
#define HIM_EVENT_PCI_RTA              9
#define HIM_EVENT_PCI_ERROR            10
#define HIM_EVENT_IO_CHANNEL_FAILED    13

/* OSMSave/SetInterruptState */
#define HIM_DISABLED                 0
#define HIM_ENABLED                  1

/* Attributes of OSMMapIOHandle */
#define HIM_IO_BIG_ENDIAN            1
#define HIM_IO_LITTLE_ENDIAN         2
#define HIM_IO_STRICTORDER           4 /* Register accesses must be in order */
#define HIM_IO_MERGING_OK            8 /* Merge consecutive loads/stores in  
                                        * single, larger operation           */
#define HIM_IO_LOAD_CACHING_OK       16 /* Loads can be cached and reused until
                                         * next store                        */
#define HIM_IO_STORE_CACHING_OK      32 /* Writes can be cached, and written 
                                         * at a later time                   */
/****************************************************************************/
/*  function pointer packets                                                */
/****************************************************************************/
/* ----- OSM -> HIM functions ----- */
#define HIM_VERSION_OSM_FUNC_PTRS 1
typedef struct HIM_OSM_FUNC_PTRS_ {
    HIM_UINT32  versionNumber;  /* currently 0 */

    HIM_UINT8   (HIM_PTR OSMMapIOHandle)(void HIM_PTR   osmAdapterContext,
                                         HIM_UINT8      rangeIndex,
                                         HIM_UINT32     offset,
                                         HIM_UINT32     length,
                                         HIM_UINT32     pacing,
                                         HIM_UINT16     attributes,
                                         HIM_IO_HANDLE HIM_PTR handle);

    HIM_UINT8   (HIM_PTR OSMReleaseIOHandle)
                                        (void HIM_PTR   osmAdapterContext,
                                         HIM_IO_HANDLE  handle);

    void            (HIM_PTR OSMEvent)(  void HIM_PTR   osmAdapterContext,
                                         HIM_UINT16     event, 
                                         void HIM_PTR   pEventContext,
                                         ...);

    HIM_BUS_ADDRESS (HIM_PTR OSMGetBusAddress)(void HIM_PTR  osmAdapterContext,
                                         HIM_UINT8      category,
                                         void HIM_PTR   virtualAddress );

    void            (HIM_PTR OSMAdjustBusAddress)
                                        (HIM_BUS_ADDRESS HIM_PTR busAddress,
                                         int value);

    HIM_UINT32      (HIM_PTR OSMGetNVSize)(void HIM_PTR osmAdapterContext);
    HIM_UINT8       (HIM_PTR OSMPutNVData)(void HIM_PTR osmAdapterContext,
                                         HIM_UINT32     destinationOffset, 
                                         void HIM_PTR   source, 
                                         HIM_UINT32     length);
    HIM_UINT8       (HIM_PTR OSMGetNVData)(void HIM_PTR osmAdapterContext,
                                         void HIM_PTR   destination, 
                                         HIM_UINT32     sourceOffset, 
                                         HIM_UINT32     length);

    HIM_UEXACT8     (HIM_PTR OSMReadUExact8)(HIM_IO_HANDLE ioBase, 
                                         HIM_UINT32     ioOffset);
    HIM_UEXACT16    (HIM_PTR OSMReadUExact16)(HIM_IO_HANDLE ioBase,
                                         HIM_UINT32     ioOffset);
    HIM_UEXACT32    (HIM_PTR OSMReadUExact32)(HIM_IO_HANDLE ioBase,
                                         HIM_UINT32     ioOffset);

    void            (HIM_PTR OSMReadStringUExact8)(HIM_IO_HANDLE ioBase, 
                                         HIM_UINT32     ioOffset,
                                         HIM_UEXACT8 HIM_PTR destBuffer, 
                                         HIM_UINT32     count, 
                                         HIM_UINT8      stride);
    void            (HIM_PTR OSMReadStringUExact16)(HIM_IO_HANDLE ioBase,
                                         HIM_UINT32     ioOffset,
                                         HIM_UEXACT16 HIM_PTR destBuffer, 
                                         HIM_UINT32     count, 
                                         HIM_UINT8      stride);
    void            (HIM_PTR OSMReadStringUExact32)(HIM_IO_HANDLE ioBase,
                                         HIM_UINT32     ioOffset,
                                         HIM_UEXACT32 HIM_PTR destBuffer, 
                                         HIM_UINT32     count, 
                                         HIM_UINT8      stride);

    void            (HIM_PTR OSMWriteUExact8)(HIM_IO_HANDLE ioBase, 
                                         HIM_UINT32     ioOffset, 
                                         HIM_UEXACT8    value);
    void            (HIM_PTR OSMWriteUExact16)(HIM_IO_HANDLE ioBase,
                                         HIM_UINT32     ioOffset, 
                                         HIM_UEXACT16   value);
    void            (HIM_PTR OSMWriteUExact32)(HIM_IO_HANDLE ioBase,
                                         HIM_UINT32     ioOffset, 
                                         HIM_UEXACT32   value);

    void            (HIM_PTR OSMWriteStringUExact8)(HIM_IO_HANDLE ioBase, 
                                         HIM_UINT32     ioOffset,
                                         HIM_UEXACT8 HIM_PTR sourceBuffer, 
                                         HIM_UINT32     count, 
                                         HIM_UINT8      stride);
    void            (HIM_PTR OSMWriteStringUExact16)(HIM_IO_HANDLE ioBase,
                                         HIM_UINT32     ioOffset,
                                         HIM_UEXACT16 HIM_PTR sourceBuffer, 
                                         HIM_UINT32     count, 
                                         HIM_UINT8      stride);
    void            (HIM_PTR OSMWriteStringUExact32)(HIM_IO_HANDLE ioBase,
                                         HIM_UINT32     ioOffset,
                                         HIM_UEXACT32 HIM_PTR sourceBuffer, 
                                         HIM_UINT32     count, 
                                         HIM_UINT8      stride);

    void            (HIM_PTR OSMSynchronizeRange)(HIM_IO_HANDLE ioBase, 
                                         HIM_UINT32     ioOffset, 
                                         HIM_UINT32     length);

    void            (HIM_PTR OSMWatchdog)(void HIM_PTR  osmAdapterContext,
                                         HIM_WATCHDOG_FUNC watchdogProcedure,
                                         HIM_UINT32     microSeconds);

    HIM_UINT8       (HIM_PTR OSMSaveInterruptState)();

    void            (HIM_PTR OSMSetInterruptState)(HIM_UINT8 interruptState);

    HIM_UEXACT32    (HIM_PTR OSMReadPCIConfigurationDword)
                                        (void HIM_PTR   osmAdapterContext,
                                         HIM_UINT8      registerNumber);
    HIM_UEXACT16    (HIM_PTR OSMReadPCIConfigurationWord)
                                        (void HIM_PTR   osmAdapterContext,
                                         HIM_UINT8      registerNumber);
    HIM_UEXACT8     (HIM_PTR OSMReadPCIConfigurationByte)
                                        (void HIM_PTR   osmAdapterContext,
                                         HIM_UINT8      registerNumber);

    void            (HIM_PTR OSMWritePCIConfigurationDword)
                                        (void HIM_PTR   osmAdapterContext,
                                         HIM_UINT8      registerNumber,
                                         HIM_UEXACT32   registerValue);
    void            (HIM_PTR OSMWritePCIConfigurationWord)
                                        (void HIM_PTR   osmAdapterContext,
                                         HIM_UINT8      registerNumber,
                                         HIM_UEXACT16   registerValue);
    void            (HIM_PTR OSMWritePCIConfigurationByte)
                                        (void HIM_PTR   osmAdapterContext,
                                         HIM_UINT8      registerNumber,
                                         HIM_UEXACT8    registerValue);

    void            (HIM_PTR OSMDelay)(void HIM_PTR  osmAdapterContext,
                                       HIM_UINT32 microSeconds);

} HIM_OSM_FUNC_PTRS;

/* ----- HIM -> OSM functions ----- */

#define HIM_VERSION_FUNC_PTRS 4
typedef struct HIM_FUNC_PTRS_ {
    HIM_UINT32  versionNumber;  /* currently 0 */
    HIM_HOST_ID (HIM_PTR HIMGetNextHostDeviceType)(HIM_UINT16 index,
                                         HIM_UINT8   HIM_PTR  hostBusType,
                                         HIM_HOST_ID HIM_PTR  mask);

    HIM_TASK_SET_HANDLE (HIM_PTR HIMCreateInitializationTSCB)
                                        (void HIM_PTR   pHIMInitTSCB );

    void        (HIM_PTR HIMGetConfiguration)
                                        (HIM_TASK_SET_HANDLE initializationTSH,
                                         HIM_CONFIGURATION HIM_PTR pConfig,
                                         HIM_HOST_ID         productID);

    HIM_UINT8   (HIM_PTR HIMSetConfiguration)
                                        (HIM_TASK_SET_HANDLE initializationTSH,
                                         HIM_CONFIGURATION HIM_PTR pConfig,
                                         HIM_HOST_ID         productID);

    HIM_UINT32  (HIM_PTR HIMSizeAdapterTSCB)
                                        (HIM_TASK_SET_HANDLE initializationTSH,
                                         HIM_HOST_ID         productID);

    HIM_TASK_SET_HANDLE (HIM_PTR HIMCreateAdapterTSCB)
                                        (HIM_TASK_SET_HANDLE initializationTSH,
                                         void HIM_PTR        tscbPointer,
                                         void HIM_PTR        osmAdapterContext,
                                         HIM_HOST_ADDRESS    hostAddress,
                                         HIM_HOST_ID         productID);

    HIM_UINT8 (HIM_PTR HIMSetupAdapterTSCB)
                                        (HIM_TASK_SET_HANDLE adapterTSH,
                                         HIM_OSM_FUNC_PTRS HIM_PTR osmRoutines,
                                         HIM_UINT16          osmFuncLength);

    HIM_UINT32  (HIM_PTR HIMCheckMemoryNeeded)
                                        (HIM_TASK_SET_HANDLE initializationTSH,
                                         HIM_TASK_SET_HANDLE  adapterTSH,
                                         HIM_HOST_ID          productID,
                                         HIM_UINT16           index,
                                         HIM_UINT8    HIM_PTR category,
                                         HIM_UINT32   HIM_PTR minimumBytes,
                                         HIM_UINT32   HIM_PTR granularity,
                                         HIM_UEXACT32 HIM_PTR alignmentMask);

    HIM_UINT8   (HIM_PTR HIMSetMemoryPointer)
                                        (HIM_TASK_SET_HANDLE  adapterTSH,
                                         HIM_UINT16           index,
                                         HIM_UINT8            category,
                                         void     HIM_PTR     pMemory,
                                         HIM_UINT32           size);

    HIM_UINT8   (HIM_PTR HIMVerifyAdapter)(HIM_TASK_SET_HANDLE  adapterTSH);

    HIM_UINT8   (HIM_PTR HIMInitialize )(HIM_TASK_SET_HANDLE    adapterTSH);

    HIM_UINT32  (HIM_PTR HIMSizeTargetTSCB)(HIM_TASK_SET_HANDLE adapterTSH);

    HIM_UINT8   (HIM_PTR HIMCheckTargetTSCBNeeded)
                                        (HIM_TASK_SET_HANDLE    adapterTSH,
                                         HIM_UINT16             index);

    HIM_TASK_SET_HANDLE (HIM_PTR HIMCreateTargetTSCB )
                                        (HIM_TASK_SET_HANDLE    adapterTSH,
                                         HIM_UINT16             index,
                                         void HIM_PTR           targetTSCB);

/* run time interfaces */

    void        (HIM_PTR HIMDisableIRQ  )(HIM_TASK_SET_HANDLE   adapterTSH);

    void        (HIM_PTR HIMEnableIRQ   )(HIM_TASK_SET_HANDLE   adapterTSH);

    HIM_UINT8   (HIM_PTR HIMPollIRQ     )(HIM_TASK_SET_HANDLE   adapterTSH);

    HIM_UINT8   (HIM_PTR HIMFrontEndISR )(HIM_TASK_SET_HANDLE   adapterTSH);

    void        (HIM_PTR HIMBackEndISR  )(HIM_TASK_SET_HANDLE   adapterTSH);

    void        (HIM_PTR HIMQueueIOB    )(HIM_IOB HIM_PTR       iob);

    HIM_UINT8   (HIM_PTR HIMPowerEvent  )(HIM_TASK_SET_HANDLE   adapterTSH,
                                         HIM_UINT8              severity);

    HIM_UINT8   (HIM_PTR HIMValidateTargetTSH)(HIM_TASK_SET_HANDLE  targetTSH);

    HIM_UINT8   (HIM_PTR HIMClearTargetTSH)(HIM_TASK_SET_HANDLE targetTSH);

    void        (HIM_PTR HIMSaveState  )(HIM_TASK_SET_HANDLE    adapterTSH,
                                         void HIM_PTR           pState);

    void        (HIM_PTR HIMRestoreState)(HIM_TASK_SET_HANDLE   adapterTSH,
                                         void HIM_PTR           pState);

    HIM_UINT8   (HIM_PTR HIMProfileAdapter)
                                    (HIM_TASK_SET_HANDLE          adapterTSH,
                                     HIM_ADAPTER_PROFILE HIM_PTR  profile);

    HIM_UINT8   (HIM_PTR HIMReportAdjustableAdapterProfile)
                                    (HIM_TASK_SET_HANDLE          adapterTSH,
                                     HIM_ADAPTER_PROFILE HIM_PTR  profileMask);

    HIM_UINT8   (HIM_PTR HIMAdjustAdapterProfile )
                                    (HIM_TASK_SET_HANDLE          adapterTSH,
                                     HIM_ADAPTER_PROFILE HIM_PTR  profile);

    HIM_UINT8   (HIM_PTR HIMProfileTarget)(HIM_TASK_SET_HANDLE    targetTSH,
                                     HIM_TARGET_PROFILE HIM_PTR   profile);

    HIM_UINT8   (HIM_PTR HIMReportAdjustableTargetProfile)
                                    (HIM_TASK_SET_HANDLE          targetTSH,
                                     HIM_TARGET_PROFILE HIM_PTR   profileMask);

    HIM_UINT8   (HIM_PTR HIMAdjustTargetProfile)
                                    (HIM_TASK_SET_HANDLE          targetTSH,
                                     HIM_TARGET_PROFILE HIM_PTR   profile);

    HIM_UINT32  (HIM_PTR HIMGetNVSize  )(HIM_TASK_SET_HANDLE     adapterTSH);

    HIM_UINT8   (HIM_PTR HIMGetNVOSMSegment)(HIM_TASK_SET_HANDLE adapterTSH, 
                                     HIM_UINT32 HIM_PTR      osmOffset, 
                                     HIM_UINT32 HIM_PTR      osmCount);

    HIM_UINT8   (HIM_PTR HIMPutNVData  )(HIM_TASK_SET_HANDLE adapterTSH,
                                     HIM_UINT32              destinationOffset,
                                     void HIM_PTR            source,
                                     HIM_UINT32              length);

    HIM_UINT8   (HIM_PTR HIMGetNVData  )(HIM_TASK_SET_HANDLE adapterTSH,
                                     void HIM_PTR            destination,
                                     HIM_UINT32              sourceOffset,
                                     HIM_UINT32              length);
    
    HIM_UINT8   (HIM_PTR HIMProfileNexus)(HIM_TASK_SET_HANDLE    nexusTSH,
                                     HIM_NEXUS_PROFILE HIM_PTR   profile);

    HIM_UINT8   (HIM_PTR HIMClearNexusTSH)(HIM_TASK_SET_HANDLE nexusTSH);

    HIM_UINT8   (HIM_PTR HIMProfileNode)(HIM_TASK_SET_HANDLE    nodeTSH,
                                     HIM_NODE_PROFILE HIM_PTR   profile);

    HIM_UINT8   (HIM_PTR HIMReportAdjustableNodeProfile)
                                    (HIM_TASK_SET_HANDLE          nodeTSH,
                                     HIM_NODE_PROFILE HIM_PTR   profileMask);

    HIM_UINT8   (HIM_PTR HIMAdjustNodeProfile)
                                    (HIM_TASK_SET_HANDLE        nodeTSH,
                                     HIM_NODE_PROFILE HIM_PTR   profile);
    
    HIM_UINT8   (HIM_PTR HIMSetOSMNodeContext) 
                                    (HIM_TASK_SET_HANDLE        nodeTSH,
                                     void HIM_PTR               osmContext);

} HIM_FUNC_PTRS;

/****************************************************************************/
/*  entryname function prototypes                                           */
/****************************************************************************/
#ifdef HIM_INCLUDE_SCSI
    void HIM_ENTRYNAME_SCSI(HIM_FUNC_PTRS HIM_PTR scsiFuncPointers,
                            HIM_UINT16 length);
#endif
#ifdef HIM_INCLUDE_FC
    void HIM_ENTRYNAME_FC(HIM_FUNC_PTRS HIM_PTR fcFuncPointers,
                          HIM_UINT16 length);
#endif
