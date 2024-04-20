/*++

Copyright (c) 1997  Digital Equipment Corporation

Module Name:

    SBridge.h 

Abstract:

    Interface to m5229ide.c

Author:

    Dick Bissen     21-July-1998

Revision History:

--*/

#ifndef _SBRIDGE_H_
#define _SBRIDGE_H_

#include "cp$src:nt_types.h"
#include "cp$src:m1543c_def.h"

#if SHARK													/* Shark PCI ints don't got thru southbridge. */
#define PCISLOTIRQ_THRU_SBRIDGE	0
#else
#define PCISLOTIRQ_THRU_SBRIDGE	1
#endif

/*
// Define Structures
*/

typedef struct _STEERABLEIRQ {
	UCHAR deviceId;			/* Id of device whose Irq is being steered. */
	UCHAR regOffset;		/* Offset of config reg in ISA bridge config space. */
	UCHAR regShift;			/* Shift of 4 bits in config reg. */
	UCHAR deviceLine;		/* The line we are steering. */
	char *handle;			/* Handle for looking up in USB table */
} STEERABLEIRQ, *PSTEERABLEIRQ;

#define STEERABLE_PMU_A_INDEX		0
#define STEERABLE_USB_A_INDEX		1
#define STEERABLE_SLOT0_A_INDEX	2
#define STEERABLE_SLOT1_A_INDEX	3
#define STEERABLE_SLOT2_A_INDEX	4
#define STEERABLE_SLOT3_A_INDEX	5

/*
// Some ISA config stuff
*/
#define ISA_IRQS				0x10				/* ISA IRQs */
#define IRQFORISA(variable,irqNumber)				(variable[irqNumber] == IRQFORISACHAR)
#define IRQFORPNP(variable,irqNumber)				(variable[irqNumber] == IRQFORPNPCHAR)
#define IRQFORSYS(variable,irqNumber)				(variable[irqNumber] == IRQFORSYSCHAR)
#define IRQFORISACHAR	'I'
#define IRQFORPNPCHAR	'P'
#define IRQFORSYSCHAR	'S'
#define IRQRESERVEDEFAULT		"SSSSSSSSSSSSSSSS"

#if PCISLOTIRQ_THRU_SBRIDGE
#define STEERABLE_IRQS			6				/* Steerable IRQs. */
#define STEERABLEIRQIRQDEFAULT	"??????"
#else
#define STEERABLE_IRQS			2				/* Steerable IRQs. */
#define STEERABLEIRQIRQDEFAULT	"??"
#endif

#define STEERABLEIRQAUTO(variable,deviceId)			(variable[deviceId] == STEERABLEIRQAUTOCHAR)
#define STEERABLEIRQDISABLED(variable,deviceId)		(variable[deviceId] == STEERABLEIRQDISABLEDCHAR)
#define STEERABLEIRQVALUE(variable,deviceId)		((variable[deviceId] > '9')?(variable[deviceId]-('A'-10)):(variable[deviceId]-'0'))
#define STEERABLEIRQAUTOCHAR		'?'
#define STEERABLEIRQDISABLEDCHAR	'0'

#define M1543C_PCIISA_DEVICE_ID			0x07
#define M1543C_IDE_DEVICE_ID			0x10
#define M1543C_PMU_DEVICE_ID			0x11
#define M1543C_USB_DEVICE_ID			0x14
#define M1535D_AUDIO_DEVICE_ID          0x06

/*
// Default device address, interrupts
*/
#define COM1_BASE               SP0_ISA_PORT_ADDRESS
#define COM1_INTERRUPT          (SERIAL1_VECTOR-ISA_BASE_VECTOR)
#define COM2_BASE               SP1_ISA_PORT_ADDRESS
#define COM2_INTERRUPT          (SERIAL2_VECTOR-ISA_BASE_VECTOR)

#define PARALLEL_BASE           PARALLEL_ISA_PORT_ADDRESS
#define PARALLEL_INTERRUPT      (PARALLEL_VECTOR-ISA_BASE_VECTOR)

#define FDC_BASE                FLOPPY_ISA_PORT_ADDRESS
#define FDC_INTERRUPT           (FLOPPY_VECTOR-ISA_BASE_VECTOR)
#define FDC_DMA_CHANNEL         0x02

#define KBD_INTERRUPT           KEYBOARD_VECTOR-ISA_BASE_VECTOR)
#define PS2_INTERRUPT           (MOUSE_VECTOR-ISA_BASE_VECTOR)

#define IDE0_INTERRUPT			(IDE0_VECTOR-ISA_BASE_VECTOR)
#define IDE1_INTERRUPT			(IDE1_VECTOR-ISA_BASE_VECTOR)

#define SMB_INTERRUPT			(SMB_VECTOR-ISA_BASE_VECTOR)

/*
// Define symbols for standard PC ISA port addresses.
*/
#define KEYBOARD_ISA_PORT_ADDRESS       0x060
#define MOUSE_ISA_PORT_ADDRESS          0x060
#define POST_LED_PORT_ADDRESS           0x080
#define FLOPPY_ISA_PORT_ADDRESS         0x3f0
#define PARALLEL_ISA_PORT_ADDRESS       0x3bc
#define SP0_ISA_PORT_ADDRESS            0x3f8
#define SP1_ISA_PORT_ADDRESS            0x2f8
#define RTC_ISA_PORT_ADDRESS            0x070


/*
// Soft reset port.
*/
#define SOFT_RESET_PHYS			0x92

/*
// Configuration space addresses of the M1543C registers accessed here.  See the
// Chip Specification for details.
*/


extern void M1543CPciIsaInitialize();
extern void M7101PmuInitialize(void);

#define M1543PciIsaWriteConfig(offset,uLongVal,size)	SBridgeWritePciConfig(M1543C_PCIISA_DEVICE_ID,offset,uLongVal,size)
#define M1543PciIsaReadConfig(offset,size)				SBridgeReadPciConfig(M1543C_PCIISA_DEVICE_ID,offset,size)
#define M7101PmuWriteConfig(offset,uLongVal,size)		SBridgeWritePciConfig(M1543C_PMU_DEVICE_ID,offset,uLongVal,size)
#define M7101PmuReadConfig(offset,size)					SBridgeReadPciConfig(M1543C_PMU_DEVICE_ID,offset,size)
#define M1543UsbWriteConfig(offset,uLongVal,size)		SBridgeWritePciConfig(M1543C_USB_DEVICE_ID,offset,uLongVal,size)
#define M1543UsbReadConfig(offset,size)					SBridgeReadPciConfig(M1543C_USB_DEVICE_ID,offset,size)
#define M5229IdeWriteConfig(offset,uLongVal,size)		SBridgeWritePciConfig(M1543C_IDE_DEVICE_ID,offset,uLongVal,size)
#define M5229IdeReadConfig(offset,size)					SBridgeReadPciConfig(M1543C_IDE_DEVICE_ID,offset,size)
#define M5451AudioWriteConfig(offset,uLongVal,size)		SBridgeWritePciConfig(M1535D_AUDIO_DEVICE_ID,offset,uLongVal,size)
#define M5451AudioReadConfig(offset,size)				SBridgeReadPciConfig(M1535D_AUDIO_DEVICE_ID,offset,size)

void SBridgeWritePciConfig(
IN ULONG deviceNumber,
IN ULONG offset,
IN ULONG uLongVal,
IN ULONG size);

ULONG SBridgeReadPciConfig(
IN ULONG deviceNumber,
IN ULONG offset,
IN ULONG size);

static struct TABLE_ENTRY *IsaDeviceInfo(
char *handle);

#endif

