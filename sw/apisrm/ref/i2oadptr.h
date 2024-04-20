/*******************************************************************************
 *
 * All software on this website is made available under the following terms and 
 * conditions.  By downloading this software, you agree to abide by these terms
 * and conditions with respect to this software.
 * 
 * I2O SIG All rights reserved.
 * 
 * These header files are provided, pursuant to your I2O SIG membership
 * agreement, free of charge on an as-is basis without warranty of any kind, 
 * either express or implied, including but not limited to, implied warranties 
 * or merchantability and fitness for a particular purpose.  I2O SIG does not 
 * warrant that this program will meet the user's requirements or that the 
 * operation of these programs will be uninterrupted or error-free.  
 * Acceptance and use of this program constitutes the user's understanding 
 * that he will have no recourse to I2O SIG for any actual or consequential 
 * damages including, but not limited to, loss profits arising out of use 
 * or inability to use this program.
 * 
 * Member is permitted to create derivative works to this header-file program.  
 * However, all copies of the program and its derivative works must contain the
 * I2O SIG copyright notice.
 *
 ******************************************************************************/


#if !defined(I2O_ADPTR_HDR)
#define I2O_ADPTR_HDR

#include    "i2omsg.h"          /* Include the Base Message file */

#define I2OADPTR_REV 1_5_1      /* Header file revision string */


/*****************************************************************************
 *
 *    i2oadptr.h -- I2O Adapter Class Message defintion file
 *
 *
 *  Revision History:
 *
 *  1.5.d   03/06/97 - First definition for spec. draft version 1.5d.
 *  1.5.1   05/02/97 - Corrections from review cycle:
 *          1) Remove "SCSI" from function definition comment.
 *          2) Add revision string.
 *          3) Convert tabs to spaces.
 *          4) New disclaimer.
 *
 *              
 *
 *****************************************************************************/

/*
    NOTES:
    
    Gets, reads, receives, etc. are all even numbered functions.
    Sets, writes, sends, etc. are all odd numbered functions.
    Functions that both send and receive data can be either but an attempt is made
    to use the function number that indicates the greater transfer amount.
    Functions that do not send or receive data use odd function numbers.

    Some functions are synonyms like read, receive and send, write.
    
    All common functions will have a code of less than 0x80.
    Unique functions to a class will start at 0x80.
    Executive Functions start at 0xA0.
    
    Utility Message function codes range from 0 - 0x1f
    Base Message function codes range from 0x20 - 0xfe
    Private Message function code is 0xff.
*/


PRAGMA_ALIGN_PUSH

PRAGMA_PACK_PUSH

/*
    Bus Adapter Class specific functions
*/

#define I2O_HBA_ADAPTER_RESET               0x85
#define I2O_HBA_BUS_QUIESCE                 0x8b
#define I2O_HBA_BUS_RESET                   0x87
#define I2O_HBA_BUS_SCAN                    0x89


/*
    Detailed Status Codes for HBA operations

    Note:
    The 16-bit Detailed Status Code field for HBA operations is divided 
    into two separate 8-bit fields.  The lower 8 bits are reserved.  The 
    upper 8 bits are used to report Adapter Status information.  The 
    definitions for these two fields, however, will be consistent with 
    the standard reply message frame structure declaration, which treats 
    this as a single 16-bit field.  In addition, the values used will be
    consistent with the Adapter Status codes defined for the SCSI 
    Peripheral class.  Theses codes are based on CAM-1.  In other words,
    these definitions are a subset of the SCSI peripheral class codes.
    Where applicable, "SCSI" has been removed from the definition.
*/  


#define I2O_HBA_DSC_MASK                        0xFF00

#define I2O_HBA_DSC_SUCCESS                     0x0000
#define I2O_HBA_DSC_ADAPTER_BUSY                0x0500
#define I2O_HBA_DSC_COMMAND_TIMEOUT             0x0B00
#define I2O_HBA_DSC_COMPLETE_WITH_ERROR         0x0400
#define I2O_HBA_DSC_FUNCTION_UNAVAILABLE        0x3A00
#define I2O_HBA_DSC_NO_ADAPTER                  0x1100
#define I2O_HBA_DSC_PARITY_ERROR_FAILURE        0x0F00
#define I2O_HBA_DSC_PATH_INVALID                0x0700
#define I2O_HBA_DSC_PROVIDE_FAILURE             0x1600
#define I2O_HBA_DSC_QUEUE_FROZEN                0x4000
#define I2O_HBA_DSC_REQUEST_ABORTED             0x0200
#define I2O_HBA_DSC_REQUEST_INVALID             0x0600
#define I2O_HBA_DSC_REQUEST_LENGTH_ERROR        0x1500
#define I2O_HBA_DSC_REQUEST_TERMINATED          0x1800
#define I2O_HBA_DSC_RESOURCE_UNAVAILABLE        0x3400
#define I2O_HBA_DSC_BUS_BUSY                    0x3F00
#define I2O_HBA_DSC_BUS_RESET                   0x0E00
#define I2O_HBA_DSC_ID_INVALID                  0x3900
#define I2O_HBA_DSC_SEQUENCE_FAILURE            0x1400
#define I2O_HBA_DSC_UNABLE_TO_ABORT             0x0300
#define I2O_HBA_DSC_UNABLE_TO_TERMINATE         0x0900
#define I2O_HBA_DSC_UNACKNOWLEDGED_EVENT        0x3500
#define I2O_HBA_DSC_UNEXPECTED_BUS_FREE         0x1300



/****************************************************************************/

/* Bus Adapter Parameter Groups */

/****************************************************************************/


#define     I2O_HBA_CONTROLLER_INFO_GROUP_NO            0x0000
#define     I2O_HBA_HISTORICAL_STATS_GROUP_NO           0x0100
#define     I2O_HBA_SCSI_CONTROLLER_INFO_GROUP_NO       0x0200
#define     I2O_HBA_SCSI_BUS_PORT_INFO_GROUP_NO         0x0201
#define     I2O_HBA_FCA_CONTROLLER_INFO_GROUP_NO        0x0300
#define     I2O_HBA_FCA_PORT_INFO_GROUP_NO              0x0301


/* - 0000h - HBA Controller Information Parameter Group */

/* Bus Type */

#define     I2O_HBA_BUS_TYPE_GENERIC        0x00
#define     I2O_HBA_BUS_TYPE_SCSI           0x01
#define     I2O_HBA_BUS_TYPE_FCA            0x10


typedef struct _I2O_HBA_CONTROLLER_INFO_SCALAR {
    U8          BusType;
    U8          BusState;
    U16         Reserved2;
    U8          BusName[12];
} I2O_HBA_CONTROLLER_INFO_SCALAR, *PI2O_HBA_CONTROLLER_INFO_SCALAR;


/* - 0100h - HBA Historical Stats Parameter Group */

typedef struct _I2O_HBA_HIST_STATS_SCALAR {
    U32         TimeLastPoweredUp;
    U32         TimeLastReset;
} I2O_HBA_HIST_STATS_SCALAR, *PI2O_HBA_HIST_STATS_SCALAR;


/* - 0200h - HBA SCSI Controller Information Parameter Group */

/* SCSI Type */

#define I2O_SCSI_TYPE_UNKNOWN               0x00
#define I2O_SCSI_TYPE_SCSI_1                0x01
#define I2O_SCSI_TYPE_SCSI_2                0x02
#define I2O_SCSI_TYPE_SCSI_3                0x03

/* Protection Management */

#define     I2O_SCSI_PORT_PROT_OTHER        0x00
#define     I2O_SCSI_PORT_PROT_UNKNOWN      0x01
#define     I2O_SCSI_PORT_PROT_UNPROTECTED  0x02
#define     I2O_SCSI_PORT_PROT_PROTECTED    0x03
#define     I2O_SCSI_PORT_PROT_SCC          0x04

/* Settings */

#define     I2O_SCSI_PORT_PARITY_FLAG       0x01
#define     I2O_SCSI_PORT_PARITY_DISABLED   0x00
#define     I2O_SCSI_PORT_PARITY_ENABLED    0x01

#define     I2O_SCSI_PORT_SCAN_ORDER_FLAG   0x02
#define     I2O_SCSI_PORT_SCAN_LOW_TO_HIGH  0x00
#define     I2O_SCSI_PORT_SCAN_HIGH_TO_LOW  0x02

#define     I2O_SCSI_PORT_IID_FLAG          0x04
#define     I2O_SCSI_PORT_IID_DEFAULT       0x00
#define     I2O_SCSI_PORT_IID_SPECIFIED     0x04

#define     I2O_SCSI_PORT_SCAM_FLAG         0x08
#define     I2O_SCSI_PORT_SCAM_DISABLED     0x00
#define     I2O_SCSI_PORT_SCAM_ENABLED      0x08

#define     I2O_SCSI_PORT_TYPE_FLAG         0x80
#define     I2O_SCSI_PORT_TYPE_PARALLEL     0x00
#define     I2O_SCSI_PORT_TYPE_SERIAL       0x80

typedef struct _I2O_HBA_SCSI_CONTROLLER_INFO_SCALAR {
    U8          SCSIType;
    U8          ProtectionManagement;
    U8          Settings;
    U8          Reserved1;
    U32         InitiatorID;
    U64         ScanLun0Only;
    U16         DisableDevice;
    U8          MaxOffset;
    U8          MaxDataWidth;
    U64         MaxSyncRate;
} I2O_HBA_SCSI_CONTROLLER_INFO_SCALAR, *PI2O_HBA_SCSI_CONTROLLER_INFO_SCALAR;


/* - 0201h - HBA SCSI Bus Port Information Parameter Group */

/*  NOTE:   Refer to the SCSI Peripheral Class Bus Port Information Parameter
            Group field definitions for HBA SCSI Bus Port field definitions.
 */

typedef struct _I2O_HBA_SCSI_BUS_PORT_INFO_SCALAR {
    U8          PhysicalInterface;
    U8          ElectricalInterface;
    U8          Isochronous;
    U8          ConnectorType;
    U8          ConnectorGender;
    U8          Reserved1;
    U16         Reserved2;
    U32         MaxNumberDevices;
    U32         DeviceIdBegin;
    U32         DeviceIdEnd;
    U8          LunBegin[8];
    U8          LunEnd[8];
} I2O_HBA_SCSI_BUS_PORT_INFO_SCALAR, *PI2O_HBA_SCSI_BUS_PORT_INFO_SCALAR;


/* - 0300h - HBA FCA Controller Information Parameters Group defines */

/* SCSI Type */

#define I2O_FCA_TYPE_UNKNOWN                0x00
#define I2O_FCA_TYPE_FCAL                   0x01

typedef struct _I2O_HBA_FCA_CONTROLLER_INFO_SCALAR {
    U8          FcaType;
    U8          Reserved1;
    U16         Reserved2;
} I2O_HBA_FCA_CONTROLLER_INFO_SCALAR, *PI2O_HBA_FCA_CONTROLLER_INFO_SCALAR;


/* - 0301h - HBA FCA Port Information Parameters Group defines */

typedef struct _I2O_HBA_FCA_PORT_INFO_SCALAR {
    U32         Reserved4;
} I2O_HBA_FCA_PORT_INFO_SCALAR, *PI2O_HBA_FCA_PORT_INFO_SCALAR;


/****************************************************************************/

/* I2O Bus Adapter Class Specific Message Definitions */

/****************************************************************************/


/****************************************************************************/

/* I2O Bus Adapter Class Reply Message Frame */

typedef struct _I2O_HBA_REPLY_MESSAGE_FRAME {
    I2O_SINGLE_REPLY_MESSAGE_FRAME StdReplyFrame;
} I2O_HBA_REPLY_MESSAGE_FRAME, *PI2O_HBA_REPLY_MESSAGE_FRAME;


/****************************************************************************/

/* I2O HBA Adapter Reset Message Frame */

typedef struct _I2O_HBA_ADAPTER_RESET_MESSAGE {
    I2O_MESSAGE_FRAME       StdMessageFrame;
    I2O_TRANSACTION_CONTEXT TransactionContext;
} I2O_HBA_ADAPTER_RESET_MESSAGE, *PI2O_HBA_ADAPTER_RESET_MESSAGE;


/****************************************************************************/

/* I2O HBA Bus Quiesce Message Frame */

typedef U32     I2O_HBQ_FLAGS;

#define I2O_HBQ_FLAG_NORMAL             0x0000
#define I2O_HBQ_FLAG_QUIESCE            0x0001

typedef struct _I2O_HBA_BUS_QUIESCE_MESSAGE {
    I2O_MESSAGE_FRAME       StdMessageFrame;
    I2O_TRANSACTION_CONTEXT TransactionContext;
    I2O_HBQ_FLAGS           Flags;
} I2O_HBA_BUS_QUIESCE_MESSAGE, *PI2O_HBA_BUS_QUIESCE_MESSAGE;


/****************************************************************************/

/* I2O HBA Bus Reset Message Frame */

typedef struct _I2O_HBA_BUS_RESET_MESSAGE {
    I2O_MESSAGE_FRAME       StdMessageFrame;
    I2O_TRANSACTION_CONTEXT TransactionContext;
} I2O_HBA_BUS_RESET_MESSAGE, *PI2O_HBA_BUS_RESET_MESSAGE;


/****************************************************************************/

/* I2O HBA Bus Scan Message Frame */

/* NOTE: SCSI-2 8-bit scalar LUN goes into offset 1 of Lun arrays */

typedef struct _I2O_HBA_BUS_SCAN_MESSAGE {
    I2O_MESSAGE_FRAME       StdMessageFrame;
    I2O_TRANSACTION_CONTEXT TransactionContext;
} I2O_HBA_BUS_SCAN_MESSAGE, *PI2O_HBA_BUS_SCAN_MESSAGE;


PRAGMA_PACK_POP

PRAGMA_ALIGN_POP

#endif      /* I2O_ADPTR_HDR */


