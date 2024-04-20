/*++

Copyright (c) 1997  Digital Equipment Corporation

Module Name:

    goby.h

Abstract:

    This file defines the structures and definitions for the GOBY ASIC.

Author:

    Eric Nelson    24-Jan-1997

Environment:

    Kernel mode

Revision History:

    Sean Mcgrane  5-May-1998  Various definitions included in support of the
                              first pass Error Logging support in the EV6 code.

    Darren Sawyer 2-Jul-1998  Definitions for interrupt balancing support
                              calls.

    Sean McGrane 13-Jul-1998  Merge in Jeffs server management and PC264 
                              changes and replace Windjammer support with
							  Clipper.

	Will Leslie   28-Oct-1998 Definitions added for DMA mode selection

    Chris Gearing Dec-11-1998 Ported to tinosa
	
--*/

#ifndef __GOBY_H__
#define __GOBY_H__

#include "cp$src:nt_types.h"

/*
// Macros
*/
#define HI32(X) ((ULONG)(X >> 32))
#define LO32(X) ((ULONG)(X & 0xFFFFFFFF))
#define ARRAYSIZE(x)		(sizeof(x)/sizeof(x[0]))

#define ON  (1)
#define OFF (0)

#define BYTE	unsigned char
#define WORD	unsigned short
#define DWORD	unsigned long

/*
// Define Goby Register Classes
*/
typedef enum _GOBY_REGISTER_CLASS {
    GobyPciRegisters = 0x1,
    GobyAgpRegisters = 0x2,
    GobyAllRegisters = 0xFFFFFFFF
} GOBY_REGISTER_CLASS, *PGOBY_REGISTER_CLASS;

/*
// Function protoypes
*/
VOID 
WriteGobyCSR(
    IN ULONG DeviceNumber,
    IN PVOID Buffer,
    IN ULONG Offset,
    IN ULONG Length
    );

VOID 
ReadGobyCSR(
    IN ULONG DeviceNumber,
    IN PVOID Buffer,
    IN ULONG Offset,
    IN ULONG Length
    );

/*
// DMA Window Values.
//
// Goby will be initialized to allow 2 DMA windows.
// The first window will be for the use of legacy devices and DMA slaves
// and therefore must have logical addresses below 16MB.
// The second window will be for bus masters, and so may be
// above 16MB.
//
// The arrangement of the windows will be as follows:
//
// Window         DMA method        Logical Start Address       Window Size
// ------         ----------        ---------------------       -----------
// Legacy         direct-mapped         0MB                          16MB
// BusMaster32    direct-mapped         2GB                           2GB
*/
#define LEGACY_DM_DMA_WINDOW_BASE (0)
#define LEGACY_DM_DMA_WINDOW_SIZE (__16MB)
#define LEGACY_SG_DMA_WINDOW_BASE (__8MB)
#define LEGACY_SG_DMA_WINDOW_SIZE (__8MB)

#define BUS_MASTER_32_DMA_WINDOW_BASE (__2GB)
#define BUS_MASTER_32_DMA_WINDOW_SIZE (DRAM_PHYSICAL_LENGTH)

/*
// QVA
// HAL_MAKE_QVA(
//     UINT PhysicalAddress
//     )
//
// Routine Description:
//
//    This macro returns the Qva for a physical address in system space.
//
// Arguments:
//
//    PhysicalAddress - Supplies a 64-bit physical address.
//
// Return Value:
//
//    The Qva associated with the physical address.
*/
#define HAL_MAKE_QVA(PA)  ((PVOID)(PA))

/*
// Product Naming data. To enable the code to determine which platform it
// is running on, we pass an 8 character string from the firmware in the
// ProductId field of the ARC information. This is used to set up a family
// name and a Platform ID. A numeric value is used for product ID to allow
// compares to be done efficiently. Supported platforms are :-
//
//  Platform           HalpFamilyName    HalpProductName   HalpPlatformID
//
//  CLIPPER             "CLIPPER "         "CLIPPER "           1
//  MONET               "MONET   "         "MONET   "           2
//  DP264               "DP264   "         "DP264   "           3
//  PC264               "PC264   "         "PC264   "           4
//
// Strings passed from firmware.
*/

/*
// These sizes are in halp.h, but the firmware can't include that...
*/
#define __2KB	(0x800)
#define __4KB	(0x1000)
#define __64KB  (0x10000)
#define __1MB   (0x100000)
#define __2MB   (0x200000)
#define __4MB   (0x400000)
#define __32MB  (0x2000000)
#define __64MB  (0x4000000)
#define __128MB (0x8000000)
#define __256MB (0x10000000)
#define __512MB (0x20000000)
#define __768MB (0x30000000)
#define __1GB   (0x40000000)
#define __2GB   (0x80000000)
#define __3GB   (0xc0000000)
#define __4GB   (0x100000000)

/*
// Goby Device IDs
*/
#define GOBY_PCI_DEVICE_ID		0x00
#define GOBY_PCIPCI_DEVICE_ID	0x01

/*
// PCI Address map
//
// NOTE: These are 43-bit addresses. 43rd bit set to go to Alpha IO range.
*/
#define GOBY_RESERVED_1		0x801FF000000
#define GOBY_PCI_CONFIG		0x801FE000000
#define GOBY_PCI_IO			0x801FC000000
#define GOBY_PCI_IACK		0x801F8000000
#define GOBY_RESERVED_2		0x80100000000
#define GOBY_PCI_MEMORY		0x80000000000

#define GOBY_SYSTEM_MEMORY	0x00000000000

#define GOBY_IO_BASE		GOBY_PCI_IO

#define GOBY_PHYS_LDAH		-0x780
#define GOBY_RESERVED_1_LDA	0x1FF0
#define GOBY_PCI_CONFIG_LDA	0x1FE0
#define GOBY_PCI_IO_LDA		0x1FC0
#define GOBY_PCI_IACK_LDA	0x1F80
#define GOBY_RESERVED_2_LDA	0x1000
#define GOBY_PCI_MEMORY_LDA	0x0000
#define ASAXP_HACK_SHIFT		(20)

/*
// Don't require to shift IO addresses on Goby.
// This def required for the HalpCreateHostBridgeRegistryField routine.
*/

#define IO_BIT_SHIFT 0x00

/*
// PCI bus-relative addresses
//
// NOTE: These are effectively 29-bit addresses
*/
#define PCI_MAX_LINEAR_IO_ADDRESS     (0x1FFFFFF)  /* I/O: __32MB - 1 */
#define PCI_MAX_LINEAR_MEMORY_ADDRESS (0x3FFFFFFF) /* Mem: __1GB - 1  */


/*
// Define the sizes of the Tsunami sections of the Logout frames.
*/
#define SYSTEM_UNCORRECTABLE_FRAME_SIZE  sizeof(TSUNAMI_UNCORRECTABLE_FRAME)
#define SYSTEM_CORRECTABLE_FRAME_SIZE    sizeof(TSUNAMI_CORRECTABLE_FRAME)

/*
// Now define the maximum PCI section required for a Tsunami based system.
// Set this to ten PCI devices, which is the max on Clipper.
*/
#define SYSTEM_MAXIMUM_PCI_EXTENSION_SIZE  (10*sizeof(PCI_COMMON_HEADER))

/*
// Define the maximum number of PCI interrupts supported by Goby.
*/
#define PCI_MAX_INTERRUPT             (4)

/*
// Define the bit values passed to the HalpInitializeDIRInterrupt routine.
*/
#define DIR_ISA_INTERRUPT             (55)
#define TEMP_WARNING_INTERRUPT        (50)
#define FAN_FAULT_INTERRUPT           (48)

/*
// Define the fixed params used for the Error Logging callbacks.
*/
#define ReadFrame                   0x1
#define WriteFrame                  0x2

#define FatalErrorFrame             0x1
#define DoubleErrorFrame            0x2
#define FRUErrorFrame               0x3

/*
// Define the default processor clock frequency used before the actual
// value can be determined.
// PASS1 is 300MHz min. Use this for now.
*/
#define DEFAULT_PROCESSOR_FREQUENCY_MHZ (600)

#if HALDBG
#ifdef ALPHA_BIOS
#include "NBridge.h"
#define DUMP_IG_CSR(device, offset, size)	DbgPrint("%-30s = %x\n",#offset,NBridgeReadPciConfig(device,offset,size))
#else
#define DUMP_IG_CSR(device, offset, size)			\
	{										\
		ULONG value = 0;						\
		ReadGobyCSR(device,&value,offset,size);	\
		DbgPrint("%-30s = %x\n",#offset,value);		\
     }
#endif
#else
#define DUMP_IG_CSR(device, offset, size)
#endif

/*
// Command Register.
*/
typedef union {
    struct {
	USHORT ioSpace:1;				/* Bit 0 I/O Space (always reads 0) The AMD-751 does not act as an I/O target. */
	USHORT pciMemory:1;				/* Bit 1 Memory Space (RW) 0 =Disable PCI memory space (default) 1 =Responds to PCI memory space accesses */
	USHORT initiatorEnable:1;		/* Bit 2 Initiator Enable (always reads 1) 1 =AMD-751 system controller can behave as bus initiator */
	USHORT specialCycleMonitor:1;	/* Bit 3 Special Cycle Monitoring (always reads 0) 0 =Special cycles not monitored */
	USHORT memoryWriteInvalid:1;	/* Bit 4 Memory Write and Invalidate Command (always reads 0) This feature increases overall performance by eliminating cache writebacks when a PCI initiator writes to the address of a modified line. 0 =The AMD-751 never generates MWI. The AMD-751 responds to MWI commands by generating the appropriate probe on the S2K system interface. */
	USHORT vgaPaletteSnoop:1;		/* Bit 5 VGA Palette Snoop (always reads 0) 0 =Palette accesses generate normal PCI cycles */
	USHORT parityErrorResponse:1;	/* Bit 6 Parity Error Response Enable (always reads 0) 0 =Do not report parity errors in status register offset 06 bit 8 (default) 1 =Enable PCI parity response in status register offset 06 bit 8 (not supported) */
	USHORT addressDataStepping:1;	/* Bit 7 Address/Data Stepping (always reads 0) 0 =Device never uses stepping */
	USHORT serrEnable:1;			/* Bit 8 SERR# Enable (RW) This bit does not affect the setting of bit 14 in offset 07h - 06h. 0 =SERR# output driver disabled (default) 1 =SERR# output driver enabled Note: If a system error is detected, SERR# can be asserted by either the PCI master or the AMD-755 peripheral bus controller. */
	USHORT back2backEnable:1;		/* Bit 9 Fast Back-to-Back Cycle Enable (always reads 0) See 5.3.5 for a discussion of back-to-back cycles. 0 =Fast back-to-back transactions only allowed to the same agent */
	USHORT reserved:6;				/* Bits 15 - 10 Reserved (always reads 0) */
} u;
	USHORT AsUSHORT;
} GOBY_COMMAND, *PGOBY_COMMAND;

/*
// Status Register.
*/
typedef union {
    struct {
	USHORT reserved:4;				/* Bits 3-0 Reserved (always reads 0) */
	USHORT capabilities:1;			/* Bit 4 Capabilities List (always reads 1). This bit indicates that the configuration space of this device contains a capabilities list. */
	USHORT mhz66Bus:1;				/* Bit 5 66 MHz-Capable PCI Bus (always reads 0). The AMD-751 supports only 33MHz PCI bus. 0 = 33 MHz support (default), 1 = 66 MHz support (not supported) */
	USHORT userDefFeatures:1;		/* Bit 6 User-Defined Features (always reads 0). The AMD-751 does not support user-defined features. */
	USHORT fastBackToBack:1;		/* Bit 7 Fast Back-to-Back Capability (always reads 0). The AMD-751 can accept fast back-to-back transactions only if they are from the same agent. */
	USHORT pciParityError:1;		/* Bit 8 PCI Parity Error Detected (always reads 0). The AMD-751 does not support parity checking. 0 =No parity error detected/Not supported */
	USHORT devSel:2;				/* Bits 10-9 DEVSEL# Timing (always reads 01). This field indicates that the slowest DEVSEL# timing is medium. 00 = Fast, 01 = Medium (AMD-751 only implements this timing), 10 = Slow, 11 = Reserved. */
	USHORT signalTargAbort:1;		/* Bit 11 Signaled Target Abort (always reads 0). The AMD-751 does not report target aborts. */
	USHORT receivedTargAbort:1;		/* Bit 12 Received Target Abort (RWC). The target issues a target abort when it detects a fatal error or cannot complete a transaction. This bit is set by simultaneously deasserting DEVSEL# and asserting STOP#. 0 = No abort received. 1 =Transaction aborted by target. */
	USHORT receivedInitAbort:1;		/* Bit 13 Received Initiator Abort (RWC). This bit is set by a PCI initiator when its transaction is terminated with initiator abort. 0 =PCI transactions proceeding normally, 1 =The AMD-751 system controller has detected that a transaction was terminated before completion. */
	USHORT serrError:1;				/* Bit 14 SERR Error (RWC). This bit is set whenever the AMD-751 generates a system error and asserts SERR#. 0 =No error signaled, 1 =SERR# error signaled. The AMD-751 has asserted the SERR# pin. (MBE, GART error, or SERR# assertion on the AGP bus). */
	USHORT dramParityError:1;		/* Bit 15 DRAM Parity Error Detected (always reads 0). The AMD-751 does not support parity checking. */
} u;
	USHORT AsUSHORT;
} GOBY_STATUS, *PGOBY_STATUS;

/*
// AGPPCISTATUS Register
*/
typedef union {
    struct {
	USHORT reserved:5;				/* bitsBits 4-0 Reserved (always reads 0) */
	USHORT mhz66Bus:1;				/* Bit 5 66MHz-Capable PCI Bus (always reads 1). The maximum secondary PCI bus operating speed is 66 MHz. */
	USHORT userDefFeatures:1;		/* Bit 6 User-Defined Features (always reads 0). The AMD-751 does not support */
	USHORT fastBackToBack:1;		/* Bit 7 Fast Back-to-Back Capability (always reads 0). The AMD-751 can accept fast back-to-back transactions only if they are from the same agent. */
	USHORT dataParityError:1;		/* Bit 8 Data Parity Error (always reads 0). The AMD-751 does not support parity checking. */
	USHORT devSel:2;				/* Bits 10-9 DEVSEL# Timing (always reads 01). This field indicates that the slowest DEVSEL# timing is medium. 00 = Fast, 01 = Medium (AMD-751 only implements this timing), 10 = Slow, 11 = Reserved. */
	USHORT signalTargAbort:1;		/* Bit 11 Signaled Target Abort (always reads 0). The AMD-751 does not report target aborts. */
	USHORT receivedTargAbort:1;		/* Bit 12 Received Target Abort (RWC). The target issues a target abort when it detects a fatal error or cannot complete a transaction by simultaneously deasserting DEVSEL# and asserting STOP#. The AMD-751 sets this bit when it detects this condition on the secondary PCI bus. 0 =No abort received. 1 =Transaction aborted by target. */
	USHORT receivedInitAbort:1;		/* Bit 13 Received Initiator Abort (RWC). This bit is set by an AGP initiator whenever its transaction is terminated with initiator abort. 0 =AGP transactions proceeding normally. 1 =The AMD-751, acting as a PCI initiator on the AGP bus, has terminated a transaction before completion */
	USHORT sserrError:1;			/* Bit 14 Signaled System Error (RWC). The AMD-751 sets this bit when ASERR# is sampled asserted by an AGP device. It then asserts SERR# 0 =No error detected (default). 1 =System error on AGP. */
	USHORT detectedParityError:1;	/* Bit 15 Detected Parity Error (always reads 0). The AMD-751 does not support parity checking. */
} u;
	USHORT AsUSHORT;
} GOBY_AGPPCISTATUS, *PGOBY_AGPPCISTATUS;

/*
// PCIARBITRATION Register
*/
typedef union {
    struct {
	ULONG parkPCI:1;				/* Bit 0 Park PCI (RW) This bit controls where the arbiter defaults to when there is no request pending. Sometimes this is referred to as processor-centric (parking on the processor) or memory-centric. Generally, a processor-centric system has improved processor benchmarks and a memory-centric system has improved overall system performance. The philosophy in a memory-centric system is that the last requestor is most likely to be the next requestor. 0 =The arbiter parks on the processor only (default) 1 =Enable parking for the PCI master (recommended) */
	ULONG pciPrefetchEnable:1;		/* Bit 1 PCI Prefetch Enable (RW). This bit enables the AMD-751 to prefetch data from the SDRAM when a PCI master on the standard PCI bus reads from the main memory. 0 =Prefetch disabled (default) 1 =Prefetch enabled */
	ULONG ldtPrefetchEnable:1;		/* Bit 2 LDT Prefetch Enable (RW)—This bit enables the Caspian system controller to prefetch data from the SDRAM when a LDT master reads from the main memory. */
	ULONG targetLatencyTimoutDisable:1;/* Bit 3 Target Latency Timer Disable (RW). When the AMD-751 acts as a PCI target, it has a latency timer that retries the (write) cycle if its buffers are full for more than 8 bus clocks (16 clocks for the first transfer). 0 =Enabled (default) 1 =Disable the target latency timer on both the standard PCI and AGP PCI interfaces */
	ULONG ev6Mode:1;				/* Bit 4 EV6 Mode (RW). When set, this bit indicates that the PCI interface decodes memory hits in the EV6 mode. There are no memory holes and DMA can be performed on any address that lies within the SDRAM map. 0 =x86 mode (default) 1 =EV6 mode */
	ULONG HoleEnable14M:1;			/* Bit 5 14M Hole Enable (RW). When set, this bit creates a ‘hole’ in memory from 14Mbytes to 15Mbytes, and the PCI decode logic does not assert a match for those addresses. */
	ULONG HoleEnable15M:1;			/* Bit 6 15M Hole Enable (RW). When set, this bit creates a ‘hole’ in memory from 15Mbytes to 16Mbytes, and the PCI decode logic does not assert a match for those addresses. */
	ULONG powerManagementEnable:1;	/* Bit 7 Power Management Register Access Enable (RW). When set, this bit allows reading and writing to the power management register (BAR2). */
	ULONG sbLockDisable:1;			/* Bit 8 SB Lock Disable (RW). This bit controls the response of the AMD-751 system controller to a PCI request by the AMD-756 peripheral bus controller. 0 = The AMD-751 makes sure that all previous requests from the BIU and PCI are flushed out before granting the AMD-756 the PCI bus 1 = Disable the flushing of previous requests */
	ULONG undefined:1;				/* Bit 9 Undefined On GOBY */
	ULONG pciEarlyProbeDisable:1;	/* Bit 10 Disable PCI Early Probe (RW). This bit works exactly the same as bit 11, except it affects PCI master requests, rather than APCI requests. */
	ULONG ldtEarlyProbeDisable:1;	/* Bit 11 Disable LDT Early Probe (RW). When set, this bit disables the Caspian system controller from issuing an early cache snoop to the processor when an LDT master requests a memory write cycle. In the default mode, as soon as the Caspian system controller detects a memory write cycle from an external LDT master, it sends a Probe Only request to the MRO, which results in a processor snoop. */
	ULONG disableReadDataError:1;	/* Bit 12 Disable Read Data Error (RW). When set, this bit disables the AMD-751 system controller from reporting a data read error to the processor if the BUI cycle was aborted. Instead, the AMD-751 then returns a data value of all ones. */
	ULONG apciWritePostRetry:1;		/* Bit 13 APCI Write-Post Retry (RW). When set, this bit enables retries on the APCI if there are pending posted writes. */
	ULONG pciWritePostRetry:1;		/* Bit 14 PCI Write-Post Retry (RW). When set, this bit enables retries on the PCI if there are pending posted writes. */
	ULONG reserved:9;				/* Bits 23-15 Reserved (always reads 0) */
	ULONG ldtBiosAddress:8;			/* Bits 31-24 LDT VGA BIOS Addr (RW) This field indicates the corresponding 16-Kbyte segment or segments that contains the VGA BIOS on the LDT bus. */
} u;
	ULONG AsULONG;
} GOBY_PCIARBITRATION, *PGOBY_PCIARBITRATION;

/*
// BIUCONTROLSTATUS Register
*/
typedef union {
    struct {
	ULONG rd2wr:2;				/* Bits 1 - 0 RD2WR Delay (RO). This field specifies the number of S2K interface cycles that are inserted between read and write transfers to allow the data bus to turn around. */
	ULONG wr2rd:1;				/* Bits 2 WR2RD Delay (RO). This field specifies the number of S2K interface cycles that are inserted between write and read transfers to allow the data bus to turn around */
	ULONG dcInDelay:4;			/* Bits 6 - 3 DC in Delay (RO). This field specifies the number of system (100-MHz) clocks from a processor write command and the start of data. 0000= 1 clock - 1111= 16 clocks */
	ULONG dcOutDelay:2;			/* Bits 8 - 7 DC Out Delay (RO). This field specifies the number of system (normally 100-MHz) clock cycles from a processor read command and the start of data. */
	ULONG rihEnabled:1;			/* Bit 9 RIH Enable (RW). When set, the BIU performs RIH instead of RID probes. */
	ULONG ackLimit:4;			/* Bits 13 - 10 Ack Limit (RO). BIOS firmware reads this field to determine how many outstanding ‘un-acked’ S2K interface commands can be sent to the AMD-751 system controller. 0000= 1 un-acked commands - 1111= 16 un-acked commands */
	ULONG probeLimit:3;			/* Bits 16 - 14 Probe Limit (RW). This field is set by the BIOS firmware with the maximum probes that the processor can handle. 000 = 1 probe (default) - 111 = 8 probes */
	ULONG stopGrantDisEnable:1;	/* Bit 17 Stop Grant Disconnect Enable (RW) 0 = No disconnect after a Stop Grant special cycle (default) 1 = Disconnect after a Stop Grant special cycle. */
	ULONG haltDisconnectEnabled:1;/* Bit 18 Halt Disconnect Enable (RW) 0 = No disconnect after a Halt special cycle (default) 1 = Disconnect after a Halt special cycle. */
	ULONG reserved2:9;			/* Bits 27 - 19 Reserved (RO) */
	ULONG reserved1:3;			/* Bits 30 - 28 Reserved (always RW 0). This field must always be written as 000 for maximum performance. */
	ULONG probeEnable:1;		/* Bits 31 Probe Enable (RW). When set, this bit allows probes to be sent to the processor. */
} u;
	ULONG AsULONG;
} GOBY_BIUCONTROLSTATUS, *PGOBY_BIUCONTROLSTATUS;

/*
// MROCONTROL Register
*/
typedef union {
    struct {
	USHORT reorderDisable:1;			/* Bit 0 Reorder Disable (RW) 0 = The MRO (Memory Reorder Buffer) reorders memory requests to optimize memory performance 1 = The MRO does not reorder memory requests (default) */
	USHORT memoryWriteQueueDisable:1;	/* Bit 1 Memory Write Queue Disable (RW). This bit is for testing purposes only. 0 = Enables the request entry in the Memory Write Queue (default) 1 = Disables the request entry in the Memory Write Queue */
	USHORT memoryRequestDisable:4;		/* Bit's 5-2 Memory Request Disable (RW). These bits are for testing purposes only. When set, these bits disable request entries in the Memory Read Queue. (default 000) */
	USHORT reserved1:3;					/* Bit's 8-6 Reserved (RW0) These bits must remain 0 for proper operation. */
	USHORT pciBlockWriteEnable:1;		/* Bit 9 PCI Block Write Enable (RW) */
	USHORT pipelineEnable:1;			/* Bit 10 Pipeline Enable (RW). Pipelined probes to same cacheline as address. */
	USHORT speedUpFeatureDisable:1;		/* Bit 11 Speed-up Feature Disable. Disables speed-up feature in S2K data read/write write/read transition. */
	USHORT memoryRequestEnable:1;		/* Bit 12 Memory Request Enable. Enables faster memory request arbiter. */
	USHORT reserved2:3;					/* Bit's 15-13 Reserved (always reads 0) */
} u;
	USHORT AsUSHORT;
} GOBY_MROCONTROL, *PGOBY_MROCONTROL;

/*
// DRAMECCSTATUS Register
*/
typedef union {
    struct {
	USHORT eccCSStatus:6;		/* Bits 5 - 0 ECC CS Status (RO). This field indicates the Chip Select where the ECC error occured. Bit 0 indicates CS0, bit 1 indicates CS1, etc. */
	USHORT reserved1:2;			/* Bits 7 - 6 Reserved (always reads 0) */
	USHORT eccStatus:2;			/* Bits 9 - 8 ECC Status (RW). This field indicates the status of the ECC Detect logic. An I/O write must clear both bits in this field before any new status can be recorded. 00 = No error (default), 01 = Multiple bit error detected (SERR# asserts), 10 = Single bit error detected, 11 = Single and Multiple bit error detected (SERR# asserts). */
	USHORT reserved2:6;			/* Bits 15 - 10 Reserved (always reads 0) */
} u;
	USHORT AsUSHORT;
} GOBY_DRAMECCSTATUS, *PGOBY_DRAMECCSTATUS;

/*
// BASEADDRESS CS Register
*/
typedef union {
    struct {
	USHORT bankEnable:1;		/* Bit 0 Bank 0 Enable (RW), 0 = Memory bank is disabled (default), 1 = Memory bank enabled. */
	USHORT addressMask:6;		/* Bits 6 - 1 (RW), DIMM 0 Address Mask A[30:25]. These bits are ANDed with A[30:25] from the processor to determine the size of the memory DIMM (32 Mbytes, 64 Mbytes, etc.). */
	USHORT baseAddress:9;		/* Bits 15 - 7 (RW) DIMM 0 Base Address A[33:25]. These bits determine the base or start address of the DIMM 0 Select. The DIMM 0 Address Mask bits determine which of the address bits are to be used by the compare logic to generate a DIMM select, depending on the size of the memory DIMM. */
} u;
	USHORT AsUSHORT;
} GOBY_BASEADDRESSCS, *PGOBY_BASEADDRESSCS;

/*
// Interrupt Control.
*/
typedef union {
    struct {
        USHORT interruptLine:8;	/* Interrupt Line (RW). This field is RW to allow BIOS software to program the required value. This register indicates which input of the system interrupt controller the the Interrupt signal pin is connected to. */
        USHORT interruptPin:8;	/* Interrupt Pin (RW) default 00h. This field indicates which interrupt pin the PCI-to-PCI bridge uses. This field is RW to allow BIOS software to program the required value. */
} u;
	USHORT AsUSHORT;
} GOBY_INTERRUPT_CTRL, *PGOBY_INTERRUPT_CTRL;

/*
// GART control registers.
*/
typedef struct _GOBY_GARTREG {
	union {
		struct {
		USHORT revisionId:8;			/* Bits 7 - 0 Revision ID (always reads 01h) */
		USHORT validBitError:1;			/* Bit 8 Valid Bit Error Capability (always reads 1). This bit is set to indicate that the AMD-751 supports the detection of valid bit errors. The controller detects an access to an invalid page by checking the valid bit for each page of the GART. */
		USHORT multiplePages:1;			/* Bit 9 Multiple Pages Capability (always reads 1). This bit is set to indicate that the AMD-751 supports multiple GART page entries. */
		USHORT pciToPci:1;				/* Bit 10 PCI-to-PCI Capability (RO). This bit is cleared to indicate that the AMD-751 only implements those PCI-to-PCI bridge commands required to implement AGP. (The AMD-751 does not implement a complete PCI 2.2-compliant PCI-to-PCI bridge between the PCI bus and AGP.) */
		USHORT reserved:5;				/* Bits 15 - 11 Reserved (always reads 0) */
		} u0;
		USHORT AsUSHORT;
	} capabilities;
	union {
		struct {
		USHORT validBitErrorEnable:1;	/* Bit 0 Valid Bit Error Enable (RW). Setting this bit enables assertion of SERR# when a graphics device attempts to access a page in AGP memory that is not valid (page fault), generating a valid bit error. 0 =SERR# is not asserted on a valid bit error (default) 1 =SERR# is asserted on a valid bit error */
		USHORT multiplePagesEnable:1;	/* Bit 1 Multiple Pages Enable (RW). 0 =Disable multiple pages per GART entry (default). 1 =Enable multiple pages per GART entry */
		USHORT gartCacheEnable:1;		/* Bit 2 Gart Cache Enable (RW) 0 =GART TLB entry caching disabled (default) 1 =GART TLB entry caching enabled. */
		USHORT pciToPciEnable:1;		/* Bit 3 PCI-to-PCI Enable (always reads 0). This bit is cleared to indicate that the AMD-751 only implements the PCI-to-PCI bridge commands required to implement AGP. (The AMD-751 does not implement a complete PCI 2.2-compliant PCI-to-PCI bridge between the PCI bus and AGP.)  */
		USHORT reserved1:4;				/* Bits 7 - 4 Reserved (always reads 0) */
		USHORT validBitErrorStatus:1;	/* Bit 8 Valid Bit Error Status (RO). The AMD-751 system controller sets this bit when the GART attempts to access an invalid page. If valid bit error signalling is enabled (bit 0 is set), the system also asserts SERR#. 0 =No error detected (default) 1 =Valid bit error detected */
		USHORT multiplePagesStatus:1;	/* Bit 9 Multiple Page Status (RO). This bit indicates the status of bit 1. 0 =Variable number of pages per GDC entry are disabled (each GDC entry cached refers to one 4-Kbyte page only) (default) 1 =Variable number of pages per GDC entry are enabled (each GDC entry cached refers to multiple 4-Kbyte pages) */
		USHORT cacheStatus:1;			/* Bit 10 GART Cache Status (RO) 0 =GART cache is disabled (default) 1 =GART cache has been enabled by software */
		USHORT pciToPciStatus:1;		/* Bit 11 PCI-to-PCI Status (always reads 0). This bit is cleared to indicate that the AMD-751 only implements those PCI-to-PCI bridge commands required to implement AGP. (The AMD-751 does not implement a complete PCI 2.2-compliant PCI-to-PCI bridge between the PCI bus and AGP.) */
		USHORT reserved2:4;				/* Bit 15 - 12 Reserved (always reads 0) */
		} u1;
		USHORT AsUSHORT;
	} enableStatus;

	ULONG baseAddress;					/* Base address 64K aligned. 0 - 11 always 0. */
	ULONG cacheSize;					/* Cache Size (always reads 02h). The AMD-751 system controller implements a GART cache that contains 16 entries, organized as 8-way, two set-associative. */

	union {
		struct {
		ULONG cacheInvalidate:1;		/* Bit 0 GART Cache Invalidate (RW) default 0. The AGP driver sets this bit to invalidate the entire GART cache. When the AMD-751 samples the bit High, it invalidates the cache and clears the bit. */
		ULONG reserved:31;				/* Bits 31 - 1 Reserved (always reads 0) */
		} u2;
		USHORT AsUSHORT;
	} cacheControl;
	union {
		struct {
		ULONG cartEntryInvalidate:1;	/* Bit 0 GART Cache Entry Invalidate (RW) (default 0). Setting this bit forces the AMD-751 to invalidate the GART cache entry specified in bits [31:12] if it is present in the GART cache. The invalidate function is performed immediately following the write to this register. The bit is cleared when the invalidate operation is completed. Note: Bits 1 - 0 must never be set (11b) simultaneously. 00 = No action (default) 01 = Invalidate 10 = Update 11 = Not allowed */
		ULONG cacheEntryUpdate:1;		/* Bit 1 GART Cache Entry Update (RW) (default 0). Setting this bit forces the AMD-751 system controller to update the GART cache entry specified in bits A[31:12] with the current entry in the GART table in system memory. The update function is performed immediately following the write to this register. The bit is cleared when the update operation is completed.  */
		ULONG reserved:10;				/* Bits 11 - 2 Reserved (always reads 00h) */
		ULONG entryOffset:20;			/* Bits 31 - 12 GART Entry Offset (RW) (default 00000h). These bits define the offset of the particular GART entry to be invalidated or updated. The AGP driver derives this offset by multiplying the page offset from the AGP virtual base address by four, which is the number of bytes in a single GART entry. When the AGP driver writes an offset to this register, the referenced GART cache entry is either invalidated or updated, depending on the value of bits 1 - 0. */
		} u3;
		USHORT AsUSHORT;
	} entryControl;
	union {
		struct {
		ULONG arbiterDisable:1;			/* Bit 0 Arbiter Disable(RW). This bit is used to enable and disable the system arbiter. 0 =The system arbiter is enabled and the arbiter can grant bus ownership to other bus masters in the system. (default) 1 =When this bit is High, the system arbiter is disabled and the boot processor (given in WHAMI Device 0, Offset 80, see page 149) has ownership of the system (AGP and PCI masters are not granted ownership of the bus). */
		ULONG reserved:31;				/* Bits 31 - 1 Reserved (always reads 0) */
		} u4;
		USHORT AsUSHORT;
	} pm2;
} GOBY_GARTREG;

typedef GOBY_GARTREG *PGOBY_GARTREG;

/*
// Function 0, Device 1 Registers
*/

typedef struct _GOBY_CONFIG {
	WORD VendorID;						/* 0x00 Vendor ID (API) TBD RO */
	WORD DeviceID;						/* 0x02 Device ID single processor device TBD RO */
	GOBY_COMMAND Command;				/* 0x04 Command 0004h RW */
	GOBY_STATUS Status;					/* 0x06 Status 0210h RWC */
	BYTE Revision;						/* 0x08 Revision ID nn (See Note) RO */
	BYTE ProgIf;						/* 0x09 Program Interface 00h RO */
	BYTE SubClass;						/* 0x0A Sub Class Code 00h RO */
	BYTE BaseClass;						/* 0x0B Base Class Code 06h RO */
	BYTE Reserved1[0x0d-0x0c];			/* 0x0C Reserved 00h RO */
	BYTE LatencyTimer;					/* 0x0D Latency Timer 00h RW */
	BYTE HeaderType;					/* 0x0E Header Type 80h RO */
	BYTE Reserved2[0x10-0x0f];			/* 0x0F Reserved 00h RO */
	DWORD AGPMemoryAddress;				/* 0x10 Base Address 0 (BAR0) (AGP Memory) 0000_0008h RW */
	DWORD GARTRegistersAddress;			/* 0x14 Base Address 1(BAR1) (GARTRegisters) 0000_0008h RW */
	DWORD PM2IOAddress;					/* 0x18 Base Address 2 (BAR2) (PM2) 0000_0001h RW */
	BYTE Reserved3[0x34-0x1c];			/* 0x1c Reserved */
	DWORD Capabilities;					/* 0x34 Capabilities 0000_00A0h RO */
	BYTE Reserved4[0x40-0x38];			/* 0x38 Reserved */
	GOBY_BASEADDRESSCS BaseAddressDimm[8];	/* 0x40 Base Address DIMM Select 0 0000h RW */
	BYTE DramMapping[4];				/* 0x50 DRAM Mapping 1/0 00h RW */
	WORD DramTiming;					/* 0x54 DRAM Timing 0000h RW */
	WORD DramCSStrength;				/* 0x56 DRAM CS Driver Strength 0000h RW */
	WORD DramECCStatus;					/* 0x58 DRAM ECC Status 00h RW */
	WORD DramModeStatus;				/* 0x5A DRAM Mode Status 00h RW */
	WORD DrawModeRegister;				/* 0x5C DRAM Mode Register 00h RW */
	BYTE Reserved5[0x60-0x5E];			/* 0x5E Reserved */
	GOBY_BIUCONTROLSTATUS BIUControlStatus;	/* 0x60 BIU Status/Control 0 0000_0Cxxh RW */
	DWORD BIUSIP;						/* 0x64 BIU SIP 0000_0000h RW */
	WORD BIUStatus;						/* 0x68 BIU Status 1 000xh RW */
	BYTE Reserved6[0x70-0x6a];			/* 0x6A Reserved 0000h RW */
	GOBY_MROCONTROL MROControl;			/* 0x70 MRO Control Register 0001h RW */
	BYTE Reserved7[0x80-0x72];			/* 0x72 Reserved 0000h RW */
	WORD WhoAmi;						/* 0x80 WHAMI and Boot Processor 00xxh RW */
	BYTE Reserved8[0x84-0x82];			/* 0x82 Reserved 0000h RO */
	GOBY_PCIARBITRATION PciArbitrationControl;	/* 0x84 PCI Arbitration Control Register 0000_0000h RW */
	WORD ConfigStatus[2];				/* 0x88 Config/Status #1 0x0xh RW */
	BYTE Reserved9[0xa0-0x8c];			/* 0x8C Reserved 0000_0000h RW */
	DWORD GartDVMARange[2];				/* 0xA0 GART/DVMA Range 0 0100_0002h RW */
	BYTE Reserved10[0xac-0xa8];			/* 0xA8 Reserved 0000_0000h RW */
	DWORD LDTVirtualAddressSpaceSize;	/* 0xAC LDT Virtual Address Space Size 0001_0000h RW */
	BYTE Reserved11[0xc0-0xb0];			/* 0xB0 Reserved 0000h RW */
	BYTE ScratchRegisters[0xdf-0xc0];	/* 0xDE - 0xC0 Scratch registers */
	BYTE SyncSemaphore;					/* 0xDF Processor Sync Semaphore */
	WORD DramTiming3;					/* 0xE0 DRAM Timing 3 */
	BYTE Reserved12[0x100-0xe2];		/* 0xE2 Reserved 0000h RW */
} GOBY_CONFIG;

/*																									     
// Function 0, Device 1 Registers																			     
*/																									     
typedef struct _GOBY_PCIPCI_CONFIG {
	WORD VendorID;						/* 0x00 Vendor ID (API) TBD RO */
	WORD DeviceID;						/* 0x02 Device ID 1 Processor device 7007h RO */
	WORD Command;						/* 0x04 Command 0000h RW */
	WORD Status;						/* 0x06 Status 0220h RO */
	BYTE Revision;						/* 0x08 Revision ID 00h RO */
	BYTE ProgIf;						/* 0x09 Program Interface 00h RO */
	BYTE SubClass;						/* 0x0A Sub Class Code 04h RO */
	BYTE BaseClass;						/* 0x0B Base Class Code 06h RO */
	BYTE Reserved1[0x0d-0x0c];			/* 0x0C Reserved 00h RO */
	BYTE Reserved2[0x0e-0x0d];			/* 0x0D Reserved 00h RO */
	BYTE Headertype;					/* 0x0E Header Type 81h RO */
	BYTE Reserved3[0x10-0x0f];			/* 0x0F Reserved 00h RO */
	BYTE Reserved4[0x18-0x10];			/* 0x10 Reserved 00h RO */
	BYTE PrimaryBusNumber;				/* 0x18 Primary Bus Number 00h RW */
	BYTE SecondaryBusNumber;			/* 0x19 Secondary Bus Number 00h RW */
	BYTE SubordinateBusNumber;			/* 0x1A Subordinate Bus Number 00h RW */
	BYTE SecondaryLatencyTimer;			/* 0x1B Secondary Latency Timer 00h RW */
	BYTE IOBaseRegister;				/* 0x1C I/O Base Register FFh RW */
	BYTE IOLimitRegister;				/* 0x1D I/O Limit Register 0Fh RW */
	WORD AGPPCISecondaryStatus;			/* 0x1E AGP/PCI Secondary Status 0220h RWC */
	WORD MemoryBase;					/* 0x20 Memory Base 0000h RW */
	WORD MemoryLimit;					/* 0x22 Memory Limit 0000h RW */
	WORD LDTAGPPrefetchMemBase;			/* 0x24 LDT/AGP Prefetch Memory Base 0000h RW */
	WORD LDTAGPPrefetchLimit;			/* 0x26 LDT/AGP Prefetch Memory Limit 0000h RW */
	BYTE Reserved5[0x30-0x28];			/* 0x28 Reserved */
	WORD Base;							/* 0x30 I/O Base 0000h RW */
	WORD Limit;							/* 0x32 I/O Limit 0000h RW */
	BYTE Reserved6[0x38-0x34];			/* 0x34 Reserved 0000_0000h RO */
	BYTE Reserved7[0x3c-0x38];			/* 0x38 Reserved 0000_0000h RO */
	WORD Interrupt;						/* 0x3C Interrupt 0000h RW */
	WORD PCIPCIBridgeControl;			/* 0x3E PCI-to-PCI Bridge Control 0000h RW */
	BYTE Reserved8[0x100-0x40];			/* 0x40 pad to end. */
} GOBY_PCIPCI_CONFIG;

#define GARTPAGESIZE		(0x1000)	/* __4K Not always defined at this point. */
#define GARTWINDOWSTART		(BUS_MASTER_32_DMA_WINDOW_BASE)
#define GARTWINDOWSIZE		(BUS_MASTER_32_DMA_WINDOW_SIZE+__256MB)			/* __256MB used for Video. */
#define GARTVASVALUE		(5)			/* 0=32MB, 1=64MB, 2=128MB, 3=256MB, 4=512MB, 5=1GB, 6=2GB */

typedef struct _GARTPDE {
	ULONG present:1;					/* Bit 0. Present bit. 1=Present and Valid. 0=Not Present Page Not Valid */
	ULONG reserved1:4;					/* Bits 4 - 1 Reserved for future use. */
	ULONG tlbLookupMask:3;				/* Bits 7 - 5 . TLB lookup mask bits M[2:0]. A 1 in any bit position masks that bit from being compared during lookup. See Table 13 for the proper use of the mask bits. */
	ULONG reserved2:4;					/* Bits 11 - 8. Reserved for future use */
	ULONG pageBaseAddress:20;			/* Bits 31 - 12. 4-Kbyte Page Base Address */
} GARTPDE;								/* A Directory entry. */

typedef GARTPDE *PGARTPDE;
typedef GARTPDE GARTPTE;				/* A page table entry. */

typedef struct _GARTPDT {
	GARTPDE pde[GARTPAGESIZE/sizeof(GARTPDE)];
} GARTPDT;								/* A page directory table. */

typedef GARTPDT  *PGARTPDT;				/* A pointer to a page table directory. */
typedef GARTPDT  GARTPT;				/* A page table. */
typedef GARTPT  *PGARTPT;				/* A pointer to a page table. */

/*
// Define the machine specific SCB structure for Typhoon based systems.
// SRM folks have asked that we keep this the same as Rawhide.
//
// Define the SCB flags. These indicate the validity of the structure elements.
*/

typedef union _SCB_FLAGS {
    struct {
      UINT CpuMaskValid:1;				/* <0> */
      UINT Reserved:1;					/* <1> */
      UINT IoMaskValid:1;				/* <2> */
      UINT SystemConfigValid:1;			/* <3> */
      UINT MemoryDescriptorValid:1;		/* <4> */
     } u;
     UINT all;
} SCB_FLAGS, *PSCB_FLAGS;

/*
// System Platform Configuration Subpacket
*/
typedef struct _SCB_SYSTEM_CONFIGURATION {
    ULONG ValidBits;					/* (0x00) Valid fields below */
    UCHAR SystemManufacturer[80];		/* (0x04) System Manufacturer */
    UCHAR SystemModel[80];				/* (0x54) Model Name */
    UCHAR SystemSerialNumber[16];		/* (0xA4) SystemSerialNumber */
    UCHAR SystemRevisionLevel[4];		/* (0xB4) SystemRevisionLevel */
    UCHAR SystemVariation[80];			/* (0xB8) SystemVariation */
    UCHAR ConsoleTypeRev[80];			/* (0x108) SRM Console Rev (string) */
} SCB_SYSTEM_CONFIGURATION, *PSCB_SYSTEM_CONFIGURATION;

/*
// SCB Memory information.
*/
typedef enum _SCB_MEMORY_TYPE {
    ScbMemoryFree,						/* Good memory */
    ScbMemoryHole,						/* Hole memory */
    ScbMemoryBad,						/* Bad memory */
    ScbMemoryMaximum
} SCB_MEMORY_TYPE;

typedef struct _SCB_MEMORY_DESCRIPTOR {
    SCB_MEMORY_TYPE MemoryType;
    ULONG BasePage;
    ULONG PageCount;
    ULONG bTested;						/* TRUE if memory has been tested */
} SCB_MEMORY_DESCRIPTOR, *PSCB_MEMORY_DESCRIPTOR;

/*
** 
** Host Bridge System Addressing Definitions
**
** The following constants define base address values that describe the
** mapping of the 40 bit processor physical address space into memory
** and I/O space.
**
** Note that these constants are base references that need to be shifted 
** into their proper bit positions to form addresses.
**                                           
*/
#define PCI_IO_BASE			0x801FC		/* PCI I/O Space */
#define PCI_MEM_BASE		0x80000		/* PCI Memory Space */
#define PCI_CONFIG_BASE		0x801FE		/* PCI Configuration Space */
#define PCI_INTACK			0x801F8		/* PCI Special Cycle/Interrupt Acknowledge */

/*
// Define size of start of GART Window.
*/
#define TINOSA_GART_START			(0)
#define TINOSA_GART_SIZE			(__32MB)
#define TINOSA_VASVALUE				(0)		/* 0=32MB, 1=64MB, 2=128MB, 3=256MB, 4=512MB, 5=1GB, 6=2GB */

/*
// Define size of start of PCI memory space for the Goby.
*/
#define PCI_IO_START				(__64KB)
#define PCI_IO_LENGTH				(__32MB - PCI_IO_START)

/*
// Define size of start of PCI memory space for the Goby.
*/
#define PCI_PREFETCH_START			(__1GB)
#define PCI_PREFETCH_LENGTH			(__2GB - PCI_PREFETCH_START)

#define PCI_NONPREFETCH_START			(__2GB)
#define PCI_NONPREFETCH_LENGTH		(__3GB - PCI_NONPREFETCH_START)

/*
// Define size of start of DRAM memory space for the Goby.
*/
#define DRAM_PHYSICAL_START			(0)
#define DRAM_PHYSICAL_LENGTH		(__512MB+__256MB-DRAM_PHYSICAL_START)

#endif
