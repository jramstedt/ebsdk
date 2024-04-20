/*++

Copyright (c) 1995  Digital Equipment Corporation

Module Name:

    iic_devices.h

Abstract:

    Private definitions for the CMOSIRQ module.

Author:

    Chris Gearing              10-Jun-1999

  
Revision History:

    10-Jun-1999         Chris Gearing
        Created.

--*/

#ifndef _IIC_DEVICES_
#define _IIC_DEVICES_

typedef struct _LM75 {
	short temp;
	unsigned char config;
	short hyst;
	short os;
} LM75, *PLM75;

typedef struct _ADMFANDIV {
	unsigned char vid:4;						/* The VID[3:0] inputs from processor core power supplies to indicate the operating voltage (e.g., 1.3 V to 3.5 V). */
	unsigned char fan1Div:2;						/* Sets Counter Prescaler for FAN1 Speed Measurement. Divisor = 2^fan1Div */
	unsigned char fan2Div:2;						/* Sets Counter Prescaler for FAN2 Speed Measurement. Divisor = 2^fan1Div */
} ADMFANDIV, *PADMFANDIV;

typedef struct _PCA8550 {
#if SHARK
	unsigned char notConnected:1;
	unsigned char systemFailLED:1;
	unsigned char normalSROM:1;
	unsigned char nvRamWakeUp:1;
#else
	unsigned char muxDataA:1;
	unsigned char muxDataB:1;
	unsigned char muxDataC:1;
	unsigned char muxDataD:1;
#endif
	unsigned char latchDataA:1;
	unsigned char latchDataB:1;					/* Must be zero */
	unsigned char latchDataC:1;					/* Must be zero */
	unsigned char latchDataD:1;					/* Must be zero */
} PCA8550, *PPCA8550;

typedef struct _PCF8574 {
	unsigned char value;
} PCF8574, *PPCF8574;

typedef struct _ADM9240 {
	unsigned char reserved1[0x15];
	unsigned char testRegister;					/* 15h Test Register 0000 0000 Setting Bit 0 of this register to 1 selects shutdown mode. Caution: Do Not write to any other bits in this register. */
	unsigned char reserved2[0x19-0x16];
	unsigned char analogOutput;					/* 19h Programmed Value of Analog Output 1111 1111# */
	unsigned char reserved3[0x20-0x1a];
	unsigned char measured25;					/* 20h +2.5 V Measured Value / Read Only */
	unsigned char measuredVCCP1;					/* 21h +VCCP1 Measured Value / Read Only */
	unsigned char measuredP33;					/* 22h +3.3 V Measured Value / Read Only */
	unsigned char measuredP5;					/* 23h +5 V Measured Value / Read Only */
	unsigned char measuredP12;					/* 24h +12 V Measured Value / Read Only */
	unsigned char measuredVCCP2;					/* 25h VCCP2 Measured Value / Read Only */
	unsigned char reserved4[0x27-0x26];			/* 26h Reserved */
	unsigned char temperature;					/* 27h Temperature Reading / Read Only */
	unsigned char fan1;							/* 28h FAN1 Reading / Read Only */
	unsigned char fan2;							/* 29h FAN2 Reading / Read Only */
	unsigned char reserved5[0x2B-0x2A];			/* 2Ah Reserved */
	unsigned char highLimitP25;					/* 2Bh +2.5 V High Limit */
	unsigned char lowLimitP25;					/* 2Ch +2.5 V Low Limit */
	unsigned char highLimitVCPP1;					/* 2Dh +VCCP1 High Limit */
	unsigned char lowLimitVCPP1;					/* 2Eh +VCCP1 Low Limit */
	unsigned char highLimitP33;					/* 2Fh +3.3 V High Limit */
	unsigned char lowLimitP33;					/* 30h +3.3 V Low Limit */
	unsigned char highLimitP5;					/* 31h +5 V High Limit */
	unsigned char lowLimitP5;					/* 32h +5 V Low Limit */
	unsigned char highLimitP12;					/* 33h +12 V High Limit */
	unsigned char lowLimitP12;					/* 34h +12V Low Limit */
	unsigned char highLimitVCCP2;					/* 35h VCCP2 High Limit */
	unsigned char lowLimitVCCP2;					/* 36h VCCP2 Low Limit */
	unsigned char reserved6[0x39-0x37];			/* 37h - 38h Reserved */
	unsigned char hotLimit;						/* 39h Hot Temperature Limit (High) */
	unsigned char hystLimit;						/* 3Ah Hot Temperature Hysteresis Limit (Low) */
	unsigned char fan1Limit;						/* 3Bh FAN1 Fan Count Limit */
	unsigned char fan2Limit;						/* 3Ch FAN2 Fan Count Limit */
	unsigned char reserved7[0x3e-0x3d];			/* 3Dh Reserved */
	unsigned char companyIdNumber;				/* 3Eh Company ID Number 0010 0011 This location will contain the company identification number (Read Only). */
	unsigned char dieRevision;					/* 3Fh Revision Number Die Revision This location will contain the revision number of the part. (Read Only). */
	unsigned char configuration;					/* 40h Configuration Register 0000 1000 See Table VII */
	unsigned char intStatus1;					/* 41h Interrupt INT Status Register 1 0000 0000 See Table VIII */
	unsigned char intStatus2;					/* 42h Interrupt INT Status Register 2 0000 0000 See Table IX */
	unsigned char intMask1;						/* 43h INT Mask Register 1 0000 0000 See Table X */
	unsigned char intMask2;						/* 44h INT Mask Register 2 0000 0000 See Table XI */
	unsigned char compatibility;					/* 45h Compatibility Register 0000 0000 See Table XII */
	unsigned char chassisIntrusion;				/* 46h Chassis Intrusion Clear Register 0000 0000 See Table XIII */
	ADMFANDIV fanDivisor;						/* 47h VID0-3/Fan Divisor Register 0101 (VID3-VID0) See Table XIV */
	unsigned char serialAddress;					/* 48h Serial Address Register 0010 11(A1)(A0) See Table XV */
	unsigned char vid4;							/* 49h VID4 Register 1000 000(VID4) See Table XVI */
	unsigned char reserved8[0x4b-0x4a];			/* 4ah Reserved */
	unsigned char tempConfiguration;				/* 4bh Temperature Configuration Register 0000 0001 See Table XVII */
} ADM9240, *PADM9240;

typedef struct _DIMMSPD {
	unsigned char bytesWrittenInmem;
	unsigned char totalBytesSpc;
	unsigned char memoryType;
	unsigned char rowsOfAddresses;
	unsigned char columnsOfAddresses;
	unsigned char moduleBanks;
	unsigned char dataWidth1;
	unsigned char dataWidth2;
	unsigned char voltageRef;
	unsigned char cycleTime;
	unsigned char accessTime;
	unsigned char dimmConfig;
	unsigned char refreshRate;
	unsigned char moduleAttributes;
	unsigned char deviceAttributes;
	unsigned char minClockDelay;
	unsigned char burstLength;
	unsigned char banksPerDram;
	unsigned char cacLatency;
	unsigned char rasLatency;
	unsigned char writeLatency;
} DIMMSPD, *PDIMMSPD;


/*
// I2C Node Values
*/
#define	M1543_BUS			0x0100
#define	CCHIP_BUS			0x0200
#define	PCF8584_BUS			0x0400

#if SHARK
#define ADM_9240_PRIMARY		M1543_BUS|0x58		/* ADM 9240 Under the Primary CPU Heatsink */
#define ADM_9240_SECONDARY		M1543_BUS|0x5a		/* ADM 9240 Under the Secondary CPU Heatsink */
#define ADM_9240_DIMMMODULE		M1543_BUS|0x5c		/* ADM 9240 Near the DIMM Module */
#define PCF_8550_MOTHERBOARD	M1543_BUS|0x9c		/* PCA 8550 4-bit multiplexed/1-bit latched 5-bit I2C EEPROM */
#define PCF_8574_MOTHERBOARD	M1543_BUS|0x70		/* PCF 8574 On the Mother Board */
#define PCF_8574_IOBOARD		M1543_BUS|0x76		/* PCF 8574 On the IO Board */

#define ADM_9240_RISERCARD		CCHIP_BUS|0x5c		/* ADM 9240 On the PCI Riser Card */
#define ADM_9240_PSU			CCHIP_BUS|0x5e		/* ADM 9240 On the PSU */
#define DIMM0_EEPROM			CCHIP_BUS|0xa0		/* DIMM 0 */
#define DIMM1_EEPROM			CCHIP_BUS|0xa2		/* DIMM 1 */
#define DIMM2_EEPROM			CCHIP_BUS|0xa4		/* DIMM 2 */
#define DIMM3_EEPROM			CCHIP_BUS|0xa6		/* DIMM 3 */
#define DIMM4_EEPROM			CCHIP_BUS|0xa8		/* DIMM 4 */
#define DIMM5_EEPROM			CCHIP_BUS|0xaa		/* DIMM 5 */
#define DIMM6_EEPROM			CCHIP_BUS|0xac		/* DIMM 6 */
#define DIMM7_EEPROM			CCHIP_BUS|0xae		/* DIMM 7 */

/* Motherboard PCF8574 Bit Values */
#define MB8574_PSUACFAIL_RO			0x01			/* Bit 0 PSU_AC_FAIL# - READ ONLY */
#define MB8574_HALT_RW				0x02			/* Bit 1 I2C_HALT# ( NMI# ) - READ/WRITE */
#define MB8574_RESETREQUEST_RW		0x04			/* Bit 2 I2C_RESET_REQUEST# - READ/WRITE */
#define MB8574_PSUOUTPUTINHIBIT_RW	0x08			/* Bit 3 I2C_PSU_OUTPUT_INH# ( "0" : Disable RACK PSU POWER OUTPUT) - READ/WRITE */
#define MB8574_P0VRMDISABLE_RW		0x10			/* Bit 4 SW_P0_CPU_VRM_DISABLE# */
#define MB8574_P1VRMDISABLE_RW		0x20			/* Bit 5 SW_P1_CPU_VRM_DISABLE# */
#define MB8574_SERIALPORTSELECT_RW	0x40			/* Bit 6 SW_SERIAL_PORT_SEL#("0" : Select CPU UART on COM Port, "1" : Select ISA UART on COM Port) - READ/WRITE */
#define MB8574_PSUACFAIL_RO			0x80			/* Bit 7 NC */


#endif

#if TINOSA
#define ADM_9240_SMB_UNIT		M1543_BUS|0x58		/* ADM 9240(System Management Unit)	0101 100X */
#define I2C_MUX_CONTROLLER		M1543_BUS|0x74		/* PCF8574AT(I2C bus MUX controller) 0111 010X */
#define PCF8582_LED_CONTROLLER	M1543_BUS|0x78		/* PCF8574AT (LED controller) 0111 100X */
#define PROCESSOR0_TEMPERATURE_NODE	M1543_BUS|0x90	/* LM79 (Thermal Dectector on Slot-B) 1001 111X */
#define PCF8582_SB_REVISION		M1543_BUS|0xa0		/* Revision EEPROM on Slot-B 1010 000X */
#define PCF8582_MB_REVISION		M1543_BUS|0xa8		/* PCF8582(MB revisionEEPROM) 1010 100X */
#define DIMM1_EPROM				M1543_BUS|0xa0		/* DIMM1 1010 000X */
#define DIMM2_EPROM				M1543_BUS|0xa2		/* DIMM2 1010 001X */
#define DIMM3_EPROM				M1543_BUS|0xa4		/* DIMM3 1010 010X */
#define ICS9179_DELAY_BUFFER	M1543_BUS|0xb2		/* ICS9179-05(Zero delay clock buffer)	1011 001X */
#endif

#if NAUTILUS || GLXYTRAIN
#define ADM_9240_SMB_UNIT		M1543_BUS|0x58		/* ADM 9240(System Management Unit)	0101 100X */
#define I2C_MUX_CONTROLLER		M1543_BUS|0x74		/* PCF8574AT(I2C bus MUX controller) 0111 010X */
#define PCF8582_LED_CONTROLLER	M1543_BUS|0x78		/* PCF8574AT (LED controller) 0111 100X */
#define PROCESSOR0_TEMPERATURE_NODE	M1543_BUS|0x90	/* LM79 (Thermal Dectector on Slot-B) 1001 111X */
#define PCF8582_SB_REVISION		M1543_BUS|0xa0		/* Revision EEPROM on Slot-B 1010 000X */
#define PCF8582_MB_REVISION		M1543_BUS|0xa8		/* PCF8582(MB revisionEEPROM) 1010 100X */
#define DIMM1_EPROM				M1543_BUS|0xa0		/* DIMM1 1010 000X */
#define DIMM2_EPROM				M1543_BUS|0xa2		/* DIMM2 1010 001X */
#define DIMM3_EPROM				M1543_BUS|0xa4		/* DIMM3 1010 010X */
#define ICS9179_DELAY_BUFFER	M1543_BUS|0xb2		/* ICS9179-05(Zero delay clock buffer) 1011 001X */
#endif

#if SWORDFISH
/* I2C bus from UP2000 tech ref */
#define LM_75_PRIMARY			PCF8584_BUS|0x94	/* ADM 9240 Under the Primary CPU Heatsink */
#define LM_75_SECONDARY			PCF8584_BUS|0x9c	/* ADM 9240 Under the Secondary CPU Heatsink */
#define ADM_9240_PRIMARY		PCF8584_BUS|0x58	/* ADM 9240 Under the Primary CPU Heatsink */
#define ADM_9240_SECONDARY		PCF8584_BUS|0x5a	/* ADM 9240 Under the Secondary CPU Heatsink */
#define PCF_8574_MOTHERBOARD	PCF8584_BUS|0x70	/* PCF 8574 On the Mother Board */
#define PCF_8574_MOTHERBOARDSEC	PCF8584_BUS|0x72	/* PCF 8574 On the Mother Board (Secondary!) */
#define ADM_9240_CS20PSU		PCF8584_BUS|0x5e	/* ADM 9240 On the CS20 PSU! */
#define DIMM0_EEPROM			CCHIP_BUS|0xa0		/* DIMM 0 */
#define DIMM1_EEPROM			CCHIP_BUS|0xa2		/* DIMM 1 */
#define DIMM2_EEPROM			CCHIP_BUS|0xa4		/* DIMM 2 */
#define DIMM3_EEPROM			CCHIP_BUS|0xa6		/* DIMM 3 */
#define DIMM4_EEPROM			CCHIP_BUS|0xa8		/* DIMM 4 */
#define DIMM5_EEPROM			CCHIP_BUS|0xaa		/* DIMM 5 */
#define DIMM6_EEPROM			CCHIP_BUS|0xac		/* DIMM 6 */
#define DIMM7_EEPROM			CCHIP_BUS|0xae		/* DIMM 7 */

/* Motherboard PCF8574 Bit Values */
#define MB8574_P0_PRESENT		0x01
#define MB8574_P1_PRESENT		0x02
#define MB8574_P0_CORE			0x04
#define MB8574_P1_CORE			0x08
#define MB8574_P0_SRAM			0x10
#define MB8574_P1_SRAM			0x20
#define MB8574_HALT_ASSRT		0x40
#define MB8574_RESETREQUEST_RW	0x80
#endif

#endif
