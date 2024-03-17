/*
// The following definitions are used to convert ULONG addresses
// to Adaptec's 3 byte address format.
*/

typedef unsigned char UCHAR;

typedef struct _THREE_BYTE {
    UCHAR Msb;
    UCHAR Mid;
    UCHAR Lsb;
} THREE_BYTE, *PTHREE_BYTE;

typedef struct _FOUR_BYTE {
    UCHAR Byte0;
    UCHAR Byte1;
    UCHAR Byte2;
    UCHAR Byte3;
} FOUR_BYTE, *PFOUR_BYTE;


/*//////////////////////////////////////////////////////////////////////////////
//
// CCB - Adaptec SCSI Command Control Block
//
//    The CCB is a superset of the CDB (Command Descriptor Block)
//    and specifies detailed information about a SCSI command.
//
//////////////////////////////////////////////////////////////////////////////*/

/*
//    Byte 0    Command Control Block Operation Code
*/

#define SCSI_INITIATOR_COMMAND    0x00
#define TARGET_MODE_COMMAND       0x01
#define SCATTER_GATHER_COMMAND    0x02
#define SCSI_INITIATOR_RET_LEN    0x03
#define SCSI_BUS_DEV_RESET	  0x81

/*
//    Byte 1    Address and Direction Control
*/

#define CCB_TARGET_ID_SHIFT       0x06            /* CCB Op Code = 00, 02 */
#define CCB_INITIATOR_ID_SHIFT    0x06            /* CCB Op Code = 01 */
#define CCB_DATA_XFER_OUT         0x10            /* Write */
#define CCB_DATA_XFER_IN          0x08            /* Read */
#define CCB_LUN_MASK              0x07            /* Logical Unit Number */

/*
//    Byte 2    SCSI_Command_Length - Length of SCSI CDB
//
//    Byte 3    Request Sense Allocation Length
*/

#define FOURTEEN_BYTES            0x00            /* Request Sense Buffer size */
#define NO_AUTO_REQUEST_SENSE     0x01            /* No Request Sense Buffer */

/*
//    Bytes 4, 5 and 6    Data Length             // Data transfer byte count
//
//    Bytes 7, 8 and 9    Data Pointer            // SGD List or Data Buffer
//
//    Bytes 10, 11 and 12 Link Pointer            // Next CCB in Linked List
//
//    Byte 13   Command Link ID                   // TBD (I don't know yet)
//
//    Byte 14   Host Status                       // Host Adapter status
*/

#define CCB_COMPLETE              0x00            /* CCB completed without error */
#define CCB_LINKED_COMPLETE       0x0A            /* Linked command completed */
#define CCB_LINKED_COMPLETE_INT   0x0B            /* Linked complete with interrupt */
#define CCB_SELECTION_TIMEOUT     0x11            /* Set SCSI selection timed out */
#define CCB_DATA_OVER_UNDER_RUN   0x12
#define CCB_UNEXPECTED_BUS_FREE   0x13            /* Target dropped SCSI BSY */
#define CCB_PHASE_SEQUENCE_FAIL   0x14            /* Target bus phase sequence failure */
#define CCB_BAD_MBO_COMMAND       0x15            /* MBO command not 0, 1 or 2 */
#define CCB_INVALID_OP_CODE       0x16            /* CCB invalid operation code */
#define CCB_BAD_LINKED_LUN        0x17            /* Linked CCB LUN different from first */
#define CCB_INVALID_DIRECTION     0x18            /* Invalid target direction */
#define CCB_DUPLICATE_CCB         0x19            /* Duplicate CCB */
#define CCB_INVALID_CCB           0x1A            /* Invalid CCB - bad parameter */

/*
//    Byte 15   Target Status
//
//    See SCSI.H files for these statuses.
*/

/*
//    Bytes 16 and 17   Reserved (must be 0)
*/

/*
//    Bytes 18 through 18+n-1, where n=size of CDB  Command Descriptor Block
*/

/*
//    Bytes 18+n through 18+m-1, where m=buffer size Allocated for Sense Data
*/

#define REQUEST_SENSE_BUFFER_SIZE 18

/*
// I/O Port Interface
*/

typedef struct _BASE_REGISTER {
    UCHAR StatusRegister;
    UCHAR CommandRegister;
    UCHAR InterruptRegister;
} BASE_REGISTER, *PBASE_REGISTER;

/*
//    Base+0    Write: Control Register
*/

#define IOP_HARD_RESET            0x80            /* bit 7 */
#define IOP_SOFT_RESET            0x40            /* bit 6 */
#define IOP_INTERRUPT_RESET       0x20            /* bit 5 */
#define IOP_SCSI_BUS_RESET        0x10            /* bit 4 */

/*
//    Base+0    Read: Status
*/

#define IOP_SELF_TEST             0x80            /* bit 7 */
#define IOP_INTERNAL_DIAG_FAILURE 0x40            /* bit 6 */
#define IOP_MAILBOX_INIT_REQUIRED 0x20            /* bit 5 */
#define IOP_SCSI_HBA_IDLE         0x10            /* bit 4 */
#define IOP_COMMAND_DATA_OUT_FULL 0x08            /* bit 3 */
#define IOP_DATA_IN_PORT_FULL     0x04            /* bit 2 */
#define IOP_INVALID_COMMAND       0X01            /* bit 1 */

/*
//    Base+1    Write: Command/Data Out
*/

/*
//    Base+1    Read: Data In
*/

/*
//    Base+2    Read: Interrupt Flags
*/

#define IOP_ANY_INTERRUPT         0x80            /* bit 7 */
#define IOP_SCSI_RESET_DETECTED   0x08            /* bit 3 */
#define IOP_COMMAND_COMPLETE      0x04            /* bit 2 */
#define IOP_MBO_EMPTY             0x02            /* bit 1 */
#define IOP_MBI_FULL              0x01            /* bit 0 */

/*/////////////////////////////////////////////////////////////////////////////
//
// Mailbox Definitions
//
//
/////////////////////////////////////////////////////////////////////////////*/

/*
// Mailbox Definition
*/

#define MB_COUNT                  0x04            /* number of mailboxes */

/*
// Mailbox Out
*/

typedef struct _MBO {
    UCHAR Command;
    THREE_BYTE Address;
} MBO, *PMBO;

/*
// MBO Command Values
*/

#define MBO_FREE                  0x00
#define MBO_START                 0x01
#define MBO_ABORT                 0x02

/*
// Mailbox In
*/

typedef struct _MBI {
    UCHAR Status;
    THREE_BYTE Address;
} MBI, *PMBI;

/*
// MBI Status Values
*/

#define MBI_FREE                  0x00
#define MBI_SUCCESS               0x01
#define MBI_ABORT                 0x02
#define MBI_NOT_FOUND             0x03
#define MBI_ERROR                 0x04

/*
// Mailbox Initialization
*/

typedef struct _MAILBOX_INIT {
    UCHAR Count;
    THREE_BYTE Address;
} MAILBOX_INIT, *PMAILBOX_INIT;

#define MAILBOX_UNLOCK      0x00
#define TRANSLATION_LOCK    0x01    /* mailbox locked for extended BIOS */
#define DYNAMIC_SCAN_LOCK   0x02    /* mailbox locked for 154xC */
#define TRANSLATION_ENABLED 0x08    /* extended BIOS translation (1023/64) */

/*
// DMA Transfer Speeds
*/

#define DMA_SPEED_50_MBS          0x00

/*
// Scatter/Gather firmware bug detection
*/

#define BOARD_ID                  0x00
#define HARDWARE_ID               0x01
#define FIRMWARE_ID               0x02
#define OLD_BOARD_ID1             0x00
#define OLD_BOARD_ID2             0x30
#define A154X_BOARD               0x41
#define A154X_BAD_HARDWARE_ID     0x30
#define A154X_BAD_FIRMWARE_ID     0x33
/*
// Host Adapter Command Operation Codes
*/

#define AC_NO_OPERATION           0x00
#define AC_MAILBOX_INITIALIZATION 0x01
#define AC_START_SCSI_COMMAND     0x02
#define AC_START_BIOS_COMMAND     0x03
#define AC_ADAPTER_INQUIRY        0x04
#define AC_ENABLE_MBO_AVAIL_INT   0x05
#define AC_SET_SELECTION_TIMEOUT  0x06
#define AC_SET_BUS_ON_TIME        0x07
#define AC_SET_BUS_OFF_TIME       0x08
#define AC_SET_TRANSFER_SPEED     0x09
#define AC_RET_INSTALLED_DEVICES  0x0A
#define AC_RET_CONFIGURATION_DATA 0x0B
#define AC_ENABLE_TARGET_MODE     0x0C
#define AC_RETURN_SETUP_DATA      0x0D
#define AC_WRITE_CHANNEL_2_BUFFER 0x1A
#define AC_READ_CHANNEL_2_BUFFER  0x1B
#define AC_WRITE_FIFO_BUFFER      0x1C
#define AC_READ_FIFO_BUFFER       0x1D
#define AC_ECHO_COMMAND_DATA      0x1F
#define AC_SET_HA_OPTION          0x21
#define AC_GET_BIOS_INFO          0x28
#define AC_SET_MAILBOX_INTERFACE  0x29
#define AC_EXTENDED_SETUP_INFO    0x8D


typedef struct _CDB6 {
    UCHAR OperationCode;
    UCHAR Reserved1 : 5;
    UCHAR LogicalUnitNumber : 3;
    UCHAR PageCode;
    UCHAR IReserved;
    UCHAR AllocationLength;
    UCHAR Control;
} CDB6, *PCDB6;

typedef struct _CDB6READWRITE {
    UCHAR OperationCode;
    UCHAR LogicalBlockMsb1 : 5;
    UCHAR LogicalUnitNumber : 3;
    UCHAR LogicalBlockMsb0;
    UCHAR LogicalBlockLsb;
    UCHAR TransferBlocks;
    UCHAR Control;
} CDB6READWRITE, *PCDB6READWRITE;

/*
/////////////////////////////////////////////////////////////////////////////
//
// CCB Typedef
//
/////////////////////////////////////////////////////////////////////////////
*/
#define MAXIMUM_CDB_SIZE 	  12
#define REQUEST_SENSE_BUFFER_SIZE 18

typedef struct _CCB {
    UCHAR OperationCode;
    UCHAR ControlByte;
    UCHAR CdbLength;
    UCHAR RequestSenseLength;
    THREE_BYTE DataLength;
    THREE_BYTE DataPointer;
    THREE_BYTE LinkPointer;
    UCHAR LinkIdentifier;
    UCHAR HostStatus;
    UCHAR TargetStatus;
    UCHAR Reserved[2];
    UCHAR Cdb[MAXIMUM_CDB_SIZE];	/* SCSI command block */
#if 0
    PVOID SrbAddress;
    PVOID AbortSrb;
    SDL   Sdl;
#endif
    UCHAR RequestSenseBuffer[REQUEST_SENSE_BUFFER_SIZE];
} CCB, *PCCB;
