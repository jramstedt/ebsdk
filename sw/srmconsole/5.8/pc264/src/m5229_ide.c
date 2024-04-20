/*++

Copyright (c) 1997  Digital Equipment Corporation

Module Name:

    m5229ide.c

Abstract:

    Functions for configuring Acer Labs M5229 IDE interface chip.

Author:

    Dick Bissen        20 July,1998

Environment:

    AlphaBIOS

Revision History:

    Jerzy Kielbasinski  08-Dec-1998 Make internal variables static to aviod errors
                                    when linking both cy693u and m5229ide modules.

--*/

#define DEBUG_TXT	0

#include	"cp$src:platform.h"
#include	"cp$inc:platform_io.h"
#include	"cp$src:common.h"
#include	"cp$inc:prototypes.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:m1543c_def.h"
#include	"cp$src:pb_def.h"

#include	"cp$src:nt_types.h"
#include	"cp$src:api_debug.h"


/*  P R I V A T E   M A C R O   D E F I N I T I O N S */

/*
** The following should be put somewhere they would be useful to everybody.
*/

#define BIT(n) (1 << (n))
#define LENGTHOF(array) (sizeof (array) / sizeof ((array)[0]))

/*
** PCI clock rate.  Because there is no standard way to determine this,it is fixed at
** 33 MHz.  The only harm should be a slight loss of performance if it is actually slower.
*/

#define PCI_CLOCK_MHz (33)

/*
** Convert nsec. to PCI clock count.  The PCI clock rate is in MHz,which is
** equivalent to cycles per microsecond,so multiply the PCI clock rate by
** time in nanoseconds,and divide by 1000,and round up to the next PCI clock
** count.  NOTE: may overflow above 1955 nsec.
*/

#define NSEC_TO_PCI_CLOCKS(nsec) ((unsigned short) (((nsec) * PCI_CLOCK_MHz + 999) / 1000))

#define IDE_CHANNELS	2
#define IDE_DRIVES		2

/*
** Configuration space addresses of the M5229 registers accessed here.  See the
** Chip Specification for details.
*/

#define BASE_ADDR_REG_I         (0x10)  /* Primary Base Address */
#define BASE_ADDR_REG_III       (0x18)  /* Secondary Base Address */
#define BUS_MASTER_CONTROL      (0x20)  /* Bus Master Control Register Base */
#define CCAI        (0x43)				/* Class Code Attributes I Register */
#define CCAII       (0x4D)				/* Class Code Attributes II Register */
#define RESRVD      (0x4F)				/* need to clear bit 6 for UDMA */
#define CCAIII      (0x50)				/* Class Code Attributes III Register */
#define FCS         (0x52)				/* Flexible Channel Select Register */
#define CDRC        (0x53)				/* CDROM Control Register */
#define FTHP        (0x54)				/* FIFO Threshold and mode for Primary Channel */
#define FTHS        (0x55)				/* FIFO Threshold and mode for Secondary Channel */
#define UDMAP       (0x56)				/* Ultra DMA Setting for Primary Channel */
#define UDMAS       (0x57)				/* Ultra DMA Setting for Secondary Channel */
#define PCAS        (0x58)				/* Primary Channel Address Setup Timing */
#define PCCB        (0x59)				/* Primary Channel Command Block Timing */
#define PCDT0       (0x5A)				/* Primary Channel Drive0 Data Read/Write Timing */
#define PCDT1       (0x5B)				/* Primary Channel Drive1 Data Read/Write Timing */
#define SCAS        (0x5C)				/* Secondary Channel Address Setup Timing */
#define SCCB        (0x5D)				/* Secondary Channel Command Block Timing */
#define SCDT0       (0x5E)				/* Secondary Channel Drive0 Data Read/Write Timing */
#define SCDT1       (0x5F)				/* Secondary Channel Drive1 Data Read/Write Timing */

/*
** Shorthand for reading and writing configuration space on the IDE interface.
*/

#define ReadIdeConfig(pb,addr,data)		data = SBridgeReadPciConfig((pb)->slot,addr,sizeof(data))
#define WriteIdeConfig(pb,addr,data)		SBridgeWritePciConfig((pb)->slot,addr,data,sizeof(data))


/*  P R I V A T E   V A R I A B L E S */

/*
** Information needed to read and write PCI configuration space to set up the
** IDE interface.
*/

/*static PBUS_HANDLER IdeBusHandler;
static PCI_SLOT_NUMBER IdeSlotNumber;

ULONG BaseAddress[2];*/

/*
** Map clock count to active count and recovery count fields in Primary and
** Secondary timing registers.
*/

static UCHAR active_count[] = {
    0x10,0x10,0x20,0x30,0x40,0x50,0x60,0x70,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

static UCHAR recovery_count[] = {
    0x01,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
    0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x00,0x00
};

static struct {
	unsigned short setup;
    unsigned short active;
	unsigned short recovery;
} taskFileTiming [] = {
    {70,	165,	365},		/* PIO Mode 0 */
    {50,	125,	208},		/* PIO Mode 1 */
    {30,	100,	110},		/* PIO Mode 2 */
    {30,	80,		70},		/* PIO Mode 3 */
    {25,	70,		25},		/* PIO Mode 4 */
    {20,	50,		30}			/* PIO Mode 5 (Last value strange!! CRG) */
};

/* structure returned by HDIO_GET_IDENTITY,as per ANSI ATA2 rev.2f spec */
struct hd_driveid {
	unsigned short	config;			/* lots of obsolete bit flags */
	unsigned short	cyls;			/* "physical" cyls */
	unsigned short	reserved2;		/* reserved (word 2) */
	unsigned short	heads;			/* "physical" heads */
	unsigned short	track_bytes;	/* unformatted bytes per track */
	unsigned short	sector_bytes;	/* unformatted bytes per sector */
	unsigned short	sectors;		/* "physical" sectors per track */
	unsigned short	vendor0;		/* vendor unique */
	unsigned short	vendor1;		/* vendor unique */
	unsigned short	vendor2;		/* vendor unique */
	unsigned char	serial_no[20];	/* 0 = not_specified */
	unsigned short	buf_type;
	unsigned short	buf_size;		/* 512 byte increments; 0 = not_specified */
	unsigned short	ecc_bytes;		/* for r/w long cmds; 0 = not_specified */
	unsigned char	fw_rev[8];		/* 0 = not_specified */
	unsigned char	model[40];		/* 0 = not_specified */
	unsigned char	max_multsect;	/* 0=not_implemented */
	unsigned char	vendor3;		/* vendor unique */
	unsigned short	dword_io;		/* 0=not_implemented; 1=implemented */
	unsigned char	vendor4;		/* vendor unique */
	unsigned char	capability;		/* bits 0:DMA 1:LBA 2:IORDYsw 3:IORDYsup*/
	unsigned short	reserved50;		/* reserved (word 50) */
	unsigned char	vendor5;		/* vendor unique */
	unsigned char	tPIO;			/* 0=slow,1=medium,2=fast */
	unsigned char	vendor6;		/* vendor unique */
	unsigned char	tDMA;			/* 0=slow,1=medium,2=fast */
	unsigned short	field_valid;	/* bits 0:cur_ok 1:eide_ok */
	unsigned short	cur_cyls;		/* logical cylinders */
	unsigned short	cur_heads;		/* logical heads */
	unsigned short	cur_sectors;	/* logical sectors per track */
	unsigned short	cur_capacity0;	/* logical total sectors on drive */
	unsigned short	cur_capacity1;	/*  (2 words,misaligned int)     */
	unsigned char	multsect;		/* current multiple sector count */
	unsigned char	multsect_valid;	/* when (bit0==1) multsect is ok */
	unsigned int	lba_capacity;	/* total number of sectors */
	unsigned short	dma_1word;		/* single-word dma info */
	unsigned short	dma_mword;		/* multiple-word dma info */
	unsigned short  eide_pio_modes; /* bits 0:mode3 1:mode4 */
	unsigned short  eide_dma_min;	/* min mword dma cycle time (ns) */
	unsigned short  eide_dma_time;	/* recommended mword dma cycle time (ns) */
	unsigned short  eide_pio;       /* min cycle time (ns),no IORDY  */
	unsigned short  eide_pio_iordy; /* min cycle time (ns),with IORDY */
	unsigned short  word69;
	unsigned short  word70;
	/* HDIO_GET_IDENTITY currently returns only words 0 through 70 */
	unsigned short  word71;
	unsigned short  word72;
	unsigned short  word73;
	unsigned short  word74;
	unsigned short  word75;
	unsigned short  word76;
	unsigned short  word77;
	unsigned short  word78;
	unsigned short  word79;
	unsigned short  word80;
	unsigned short  word81;
	unsigned short  command_sets;	/* bits 0:Smart 1:Security 2:Removable 3:PM */
	unsigned short  word83;			/* bits 14:Smart Enabled 13:0 zero */
	unsigned short  word84;
	unsigned short  word85;
	unsigned short  word86;
	unsigned short  word87;
	unsigned short  dma_ultra;
	unsigned short	word89;			/* reserved (word 89) */
	unsigned short	word90;			/* reserved (word 90) */
	unsigned short	word91;			/* reserved (word 91) */
	unsigned short	word92;			/* reserved (word 92) */
	unsigned short	word93;			/* reserved (word 93) */
	unsigned short	word94;			/* reserved (word 94) */
	unsigned short	word95;			/* reserved (word 95) */
	unsigned short	word96;			/* reserved (word 96) */
	unsigned short	word97;			/* reserved (word 97) */
	unsigned short	word98;			/* reserved (word 98) */
	unsigned short	word99;			/* reserved (word 99) */
	unsigned short	word100;		/* reserved (word 100) */
	unsigned short	word101;		/* reserved (word 101) */
	unsigned short	word102;		/* reserved (word 102) */
	unsigned short	word103;		/* reserved (word 103) */
	unsigned short	word104;		/* reserved (word 104) */
	unsigned short	word105;		/* reserved (word 105) */
	unsigned short	word106;		/* reserved (word 106) */
	unsigned short	word107;		/* reserved (word 107) */
	unsigned short	word108;		/* reserved (word 108) */
	unsigned short	word109;		/* reserved (word 109) */
	unsigned short	word110;		/* reserved (word 110) */
	unsigned short	word111;		/* reserved (word 111) */
	unsigned short	word112;		/* reserved (word 112) */
	unsigned short	word113;		/* reserved (word 113) */
	unsigned short	word114;		/* reserved (word 114) */
	unsigned short	word115;		/* reserved (word 115) */
	unsigned short	word116;		/* reserved (word 116) */
	unsigned short	word117;		/* reserved (word 117) */
	unsigned short	word118;		/* reserved (word 118) */
	unsigned short	word119;		/* reserved (word 119) */
	unsigned short	word120;		/* reserved (word 120) */
	unsigned short	word121;		/* reserved (word 121) */
	unsigned short	word122;		/* reserved (word 122) */
	unsigned short	word123;		/* reserved (word 123) */
	unsigned short	word124;		/* reserved (word 124) */
	unsigned short	word125;		/* reserved (word 125) */
	unsigned short	word126;		/* reserved (word 126) */
	unsigned short	word127;		/* reserved (word 127) */
	unsigned short	security;		/* bits 0:support 1:enabled 2:locked 3:frozen */
	unsigned short	reserved[127];
};

#define DMA_CAPABLE			0x01
#define LBA_CAPABLE			0x02
#define USEIORDY_CAPABLE	0x08

/*  P R I V A T E   F U N C T I O N   P R O T O T Y P E S */



/*  P U B L I C   F U N C T I O N   D E F I N I T I O N S */

void
M5229Initialize(
struct pb *pb)
/*++

Routine Description:

    This function is called when the built-in Acer Labs M5229 IDE interface is
    detected during the PCI configuration process.  It sets the timing
    registers to very conservative (read: slow) values which are likely
    to work with any attached device.  It also records the adapter,bus,and
    device for use when finally configuring optimal timing in M5229Configure().

Arguments:

    Adapter - the M5229 IDE adapter

    Bus - the bus it is on

    Device - the M5229 IDE device

Return Value:

    None.

--*/

{
    UCHAR byteVal;
    ULONG ulongVal;

#if SHARK
	if (!api_debug_value(APIDB_IDEPIO))
		return;
#endif

    /*
    ** Erase the IdeDrive[] array.  This is almost always redundant; see discussion below.
    */
    /*memset(IdeDrive,0,sizeof IdeDrive);*/

    /*
    ** Set up the Acer M5229 interface for very conservative (slow) access to
    ** the drives.  This duplicates the state of timing registers after a
    ** hardware reset; this is almost always redundant,as either a hardware
    ** reset just occurred,or a PCI reset was done on a restart,except in the
    ** case where AlphaBIOS has just been upgraded and has been restarted
    ** without having seen a hardware reset yet.
    **
    ** NOTE: the "hardware reset" timing values used here were determined
    ** experimentally,they are not documented in the Acer Labs M1543C Chip Specification.
    */

    ReadIdeConfig(pb,CDRC,byteVal);
    byteVal = byteVal | 0x01; 
    WriteIdeConfig(pb,CDRC,byteVal);
    byteVal = 0x00;                     /* Set PIO mode with FIFO disabled */
    WriteIdeConfig(pb,FTHP,byteVal);
    WriteIdeConfig(pb,FTHS,byteVal);
    byteVal = 0x00;                     /* Disable Ultra DMA mode */
    WriteIdeConfig(pb,UDMAP,byteVal);
    WriteIdeConfig(pb,UDMAS,byteVal);
    byteVal = 0x00;                     /* 8 clock address setup timming,8clks/16clks  */
                                        /* command block timing. */
    WriteIdeConfig(pb,PCAS,byteVal);
    WriteIdeConfig(pb,PCCB,byteVal);
    WriteIdeConfig(pb,PCDT0,byteVal);
    WriteIdeConfig(pb,PCDT1,byteVal);
    WriteIdeConfig(pb,SCAS,byteVal);
    WriteIdeConfig(pb,SCCB,byteVal);
    WriteIdeConfig(pb,SCDT0,byteVal);
    WriteIdeConfig(pb,SCDT1,byteVal);
    byteVal = 0x1A; 
    WriteIdeConfig(pb,RESRVD,byteVal);

    /*
    ** Get the base address which is needed for UDMA setup.
    */
    /*ReadIdeConfig(pb,BASE_ADDR_REG_I,ulongVal);
    BaseAddress[0] = ulongVal  & ~PCI_ADDRESS_IO_SPACE;
    ReadIdeConfig(pb,BASE_ADDR_REG_III,ulongVal);
    BaseAddress[1] = ulongVal  & ~PCI_ADDRESS_IO_SPACE;*/

    /*
    ** Task file timing affects all attached devices; use a fixed value for default.
    */
    M5229SetTaskFileTiming(pb,290,600,0);		     /* 290 nsec. minimum active,600 nsec. minimum cycle */
    M5229SetTaskFileTiming(pb,290,600,1);		     /* 290 nsec. minimum active,600 nsec. minimum cycle */
}


/*  P R I V A T E   F U N C T I O N   D E F I N I T I O N S */

void M5229Configure(
struct pb *pb,
int channel,
int drive,
struct hd_driveid *driveId)
/*++

Routine Description:

    Configure the ACER M5229 PCI IDE interface to operate optimally with the drives
    attached to it.  Set IDE interface timing parameters according to the modes
    supported by the drives on each channel,and set the DMA Capable bits in the Bus
    Master IDE Status registers for drives which support DMA modes.


Arguments:

    None.

Return Value:

    None.

--*/

{
	static unsigned short setup[IDE_CHANNELS] = {0,0};			/* per-channel address setup time */
	static ULONG pioMode[IDE_CHANNELS] = {6,6};
	unsigned short thisPioMode;
	unsigned short cycleTime = 0;

#ifdef ACER_REV_E_UDMA
	PCONFIGURATION_COMPONENT Component;
	CHAR PathName[16] = "scsi(0)disk(0)";
	unsigned short UltraDMASupportSave;
#endif

	if (
	    (channel >= IDE_CHANNELS) ||
		(drive >= IDE_DRIVES)
	   )
		return;

#if SHARK
	if (!api_debug_value(APIDB_IDEPIO))
		return;
#endif

	DbgPrint(("IdeConfigureInterface (ACER M5229),%d,%d\r\n"));

#ifdef ACER_REV_E_UDMA
    if (channel != 0)
	{
        if (IdeDrive[channel-1][0].Present || IdeDrive[channel-1][1].Present)
		{
			PathName[5] += 1;
        }
    }
#endif

	thisPioMode = driveId->tPIO;

	if (thisPioMode > 2)			/* 2 is maximum allowed tPIO value */
		thisPioMode = 2;

	if (driveId->field_valid & 2)							/* drive implements ATA2? */
	{
		if (driveId->capability & USEIORDY_CAPABLE)			/* drive supports use_iordy? */
		{
			/*use_iordy = 1;*/
			cycleTime = driveId->eide_pio_iordy;

			if (driveId->eide_pio_modes & 7)
			{
				/*overridden = 0;*/

				if (driveId->eide_pio_modes & 4)
					thisPioMode = 5;
				else if (driveId->eide_pio_modes & 2)
					thisPioMode = 4;
				else
					thisPioMode = 3;
			}
		}
		else
		{
			cycleTime = driveId->eide_pio;
		}
	}

	if (cycleTime == 0)
		cycleTime = taskFileTiming[thisPioMode].setup+taskFileTiming[thisPioMode].active+taskFileTiming[thisPioMode].recovery;

	DbgPrint(("Channel %d,device %d\r\n",channel,drive));
	DbgPrint(("Capability: %x\n",driveId->capability));
	DbgPrint(("PIO Mode: %d\n",driveId->tPIO));
	DbgPrint(("This PIO Mode: %d\n",thisPioMode));

	/*DbgPrint(("DMAXmitMode: %d\n",driveId->DMAXmitMode));
	DbgPrint(("DMAXmitSpeedMode: %d\n",driveId->DMAXmitSpeedMode));
	DbgPrint(("DMAActiveTime: %d\n",driveId->DMAActiveTime));
	DbgPrint(("DMACycleTime: %d\n",driveId->DMACycleTime));
	DbgPrint(("PIOSetupTime: %d\n",driveId->PIOSetupTime));
	DbgPrint(("PIOActiveTime: %d\n",driveId->PIOActiveTime));
	DbgPrint(("PIOCycleTime: %d\n",driveId->PIOCycleTime));
	DbgPrint(("PIOCycleTimeIORDY: %d\n",driveId->PIOCycleTimeIORDY));
	DbgPrint(("UltraDMASupport: %d\n",driveId->UltraDMASupport));*/

    /*
    ** Get worst case PIOmode
    */
    if (pioMode[channel] > thisPioMode)
	{
        pioMode[channel] = thisPioMode;
    }

    /*
    ** If the drive supports a DMA mode,set its DMA Capable bit.
    */
    if (driveId->capability & DMA_CAPABLE)
	{
#ifdef ACER_REV_E_UDMA
		if (driveId->UltraDMASupport)
		{
			UltraDMASupportSave = driveId->UltraDMASupport;
			driveId->UltraDMASupport = 0;
			PathName[12] = '0' + (CHAR)device;
			Component = FwGetComponent( PathName );
			if (Component != NULL)
			{
				DbgPrint(("***** Component(%s) Class(%x) Type(%x) IdentifierLength(%x)\r\n",PathName,Component->Class,Component->Type,Component->IdentifierLength));
    
				if ((Component->Class == ControllerClass) && (Component->Type == DiskController))
				{
					if (Component->IdentifierLength > 8 )
					{
						if (strncmp(Component->Identifier,"QUANTUM ",8) == 0)
						{
							driveId->UltraDMASupport = UltraDMASupportSave;
						}
					}
				}
			}
			else
			{
				DbgPrint(("***** Component(%s) NULL\r\n",PathName));
			}

			if (driveId->UltraDMASupport == 0)
			{
				DbgPrint("***** UltraDMASupport DISABLED on %s\r\n",PathName));
			}
		}
#endif
		/*M5229EnableDMA(channel,drive,driveId);*/
	}
                
	/*
	** Record the maximum setup time of the drives attached to this channel.
	*/
	if (taskFileTiming[thisPioMode].setup > setup[channel])
	{
		setup[channel] = taskFileTiming[thisPioMode].setup;
	}

	/*
	** Calculate and set read/write or DACK timing.
	*/
	/*M5229SetRWTiming(pb,channel,drive,driveId);*/

	/*
	** Address setup time requirements of both devices are known.  Set the
	** address setup time for the channel.
	*/
	M5229SetAddressSetup(pb,channel,setup[channel]);

	/*
	** Task file timing affects all attached devices
	*/
	if (pioMode[channel] != 6)
	{
		M5229SetTaskFileTiming(pb,taskFileTiming[pioMode[channel]].active,taskFileTiming[pioMode[channel]].recovery,channel);
	}
}

#if 0
void M5229IdeEnableDeviceDMA(
unsigned short channel,
unsigned short device)
/*++

Routine Description:

    Enable DMA operation on a drive.

Arguments:

    Channel - the IDE channel number (0 or 1) the drive is attached to.

    Device - the IDE device number (0 or 1) of the drive on the channel.

Return Value:

    None.

--*/

{
    PUCHAR busMasterStatusAddr;
    M5229_BUS_MASTER_STATUS busMasterStatusValue;
    static struct {								/* lookup table indexed by [Channel] to get: */
        unsigned short Offset;					/* status register offset */
    } busMasterStatus[2] = {					/* for controlling read-ahead */
            PRIMARY_BUS_MASTER_STATUS,
            SECONDARY_BUS_MASTER_STATUS												
    };

    /*
    ** Enable the DMA device in the SFF-8038i Bus master IDE status register.
    */
    busMasterStatusAddr = (PUCHAR)((ULONG)ideBusMasterControlBase + busMasterStatus[Channel].Offset);

    busMasterStatusValue.AsUCHAR = READ_PORT_UCHAR(busMasterStatusAddr);

    if (device == 0) {
        busMasterStatusValue.Drive0DMACapable = 1;
    } else {
        busMasterStatusValue.Drive1DMACapable = 1;
    }

    WRITE_PORT_UCHAR(busMasterStatusAddr,busMasterStatusValue.AsUCHAR);
    
}

void
M5229EnableDMA(
unsigned short channel,
unsigned short device,
PIDE_DRIVE_INFO Drive)

/*++

Routine Description:

    Enable DMA operation on a drive.

Arguments:

    Channel - the IDE channel number (0 or 1) the drive is attached to.

    Device - the IDE device number (0 or 1) of the drive on the channel.

    Drive - pointer to the IDE_DRIVE_INFO containing the timing requirements for the device.

Return Value:

    None.

--*/

{
    UCHAR byteVal;
    static struct {         /* lookup table indexed by [Channel][Device] to get: */
        UCHAR addr;         /*   configuration register address */
        UCHAR data;         /*   bit data */
        UCHAR mask;         /*   bit mask */
    } dma[2][2] = {         /* for controlling read-ahead */
        {
            { FTHP,BIT(3)|BIT(0),0x0F },
            { FTHP,BIT(7)|BIT(4),0xF0 }
        },
        {
            { FTHS,BIT(3)|BIT(0),0x0F },
            { FTHS,BIT(7)|BIT(4),0xF0 }
        }
    };

	DbgPrint(("Enable DMA\r\n"));

    ReadIdeConfig(pb,dma[Channel][device].Addr,byteVal);
    byteVal &= ~dma[channel][device].Mask;						/* Clear field */
    byteVal |= dma[channel][device].Data;						/* Enable DMA mode */
    WriteIdeConfig(pb,dma[channel][device].Addr,byteVal);

    M5229IdeEnableDeviceDMA(channel,device);

    if (driveId->UltraDMASupport)
	{
		ULONG UDMAMode = 0;

		if (driveId->UltraDMASupport & 4)
		{
			/*
			** The Quantum drive that I have been testing fails to operate
			** in UDMA mode 2.  For now limit to a maximum of mode 1.
			*/
			UDMAMode = 1;
		}
		else if (driveId->UltraDMASupport & 2)
		{
			UDMAMode = 1;
		}

		DD( DbgPrint("Enable Ultra DMA Mode %x\r\n",UDMAMode); )
		IdeUltraDMA(Channel,Device,UDMAMode);
		AcerUltraDMAMode(Channel,Device,UDMAMode);
    }
}

void M5229SetRWTiming(
struct pb *pb,
int channel,
int drive,
struct hd_driveid *driveId)
/*++

Routine Description:

    Calculate and set the Data Read/Write or DACK Timing Register value.  If the device
    supports DMA,both the DMA mode timing and PIO mode timing are considered,with the
    more conservative (slower) timing chosen because the NT ATAPI driver could choose
    either mode.

    NOTE:  There is a simplifying assumption that the PIO mode DIOR-/DIOW- recovery
    time is equal to the cycle time minus the active time; for some modes it is
    actually less than this.  Someone interested in squeezing the last few nanoseconds
    out of a cycle could take advantage of this fact.  Since the number of clocks
    required for the active time is rounded up,this will generally result in an
    active time (and thus cycle time) longer than necessary,which in some cases
    can be compensated for by reducing the recovery time.

Arguments:

    Drive - pointer to the IDE_DRIVE_INFO containing the timing requirements for the device.

    Channel - the IDE channel number (0 or 1) the drive is attached to.

    Device - the IDE device number (0 or 1) of the drive on the channel.

Return Value:

    None.

--*/

{
    unsigned short active,recovery,cycle;			/* DIOR-,DIOW- active and recovery times */
    static UCHAR CDTx[2][2] = {						/* map Channel and Device to P/SCDTx register */
        { PCDT0,PCDT1 },
        { SCDT0,SCDT1 }
    };
    UCHAR byteVal;

    /*
    ** Get timing for PIO mode.
    */

	DbgPrint("PIO timing: %d active,%d cycle\r\n",driveId->PIOActiveTime,driveId->PIOCycleTime));

    active = driveId->PIOActiveTime;
    cycle = driveId->eide_pio;

    /*
    ** If the device supports DMA,check DMA mode timing,adjust active and recovery if
    ** DMA mode requires longer times.
    */

    if (driveId->capability & DMA_CAPABLE)
	{
		DbgPrint(("DMA timing: %d active,%d cycle\r\n",driveId->DMAActiveTime,driveId->DMACycleTime));

        if (driveId->DMAActiveTime > active)
		{
            active = driveId->DMAActiveTime;
        }

        if (driveId->DMACycleTime > cycle)
		{
            cycle = driveId->DMACycleTime;
        }
    }

    /*
    ** Convert nsec to PCI clock count.
    */
    active = NSEC_TO_PCI_CLOCKS(active);
    cycle = NSEC_TO_PCI_CLOCKS(cycle);
    recovery = cycle - active;

	DbgPrint(("Clock counts: %d active,%d recovery\r\n",active,recovery));

    /*
    ** Encode these clock counts into a P/SCDTx register image and write it to
    ** the appropriate register.  Note that the longest time is PIO mode 0
    ** recovery time,which is 15 clocks at 33 MHz,so no range checking is
    ** performed here.
    */

    byteVal = recovery_count[recovery] | active_count[active];
    WriteIdeConfig(pb,CDTx[Channel][Device],byteVal);
}
#endif

void
M5229SetAddressSetup(
struct pb *pb,
unsigned short channel,
unsigned short setupTime)
/*++

Routine Description:

    Calculate and set the address setup time to be used on a channel.

Arguments:

    Channel - the IDE channel number (0 or 1).

    SetupTime - the minimum address setup time allowed by the device(s) on the channel.

Return Value:

    None.

--*/

{
    static UCHAR setup_count[] = {							/* map clock count 2-5 to ARTTIMx register */
        0x01,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x00,0x00
    };
    UCHAR byteVal;

	DbgPrint(("Channel %d address setup time %d\r\n",channel,setupTime));

    setupTime = NSEC_TO_PCI_CLOCKS(setupTime);

	DbgPrint(("Address setup time clock count: %d\r\n",setupTime));

    /*
    ** Encode the clock count and program the appropriate register.  Both
    ** channels has only one register for both drives,which also has some 
    ** unrelated bits in it.
    */

    if (channel == 0)
	{
		WriteIdeConfig(pb,PCAS,setup_count[setupTime]);
    }
    else
	{
		WriteIdeConfig(pb,SCAS,setup_count[setupTime]);
    }
}

#if 0
#define IDE_COMMAND_SET_FEATURES (0xEF)
void
IdeUltraDMA(
    ULONG Channel,
    ULONG Device,
    ULONG Mode
    )
/*++

Routine Description:

    Set the Ultra DMA mode

Arguments:

HwDeviceExtension - ATAPI driver storage.

--*/

{
    PIDE_REGISTERS_1    baseIoAddress;
    UCHAR statusByte,errorByte;

    baseIoAddress = (PIDE_REGISTERS_1)BaseAddress[Channel];

    /*
    ** set mode
    */
    ScsiPortWritePortUchar(&baseIoAddress->DriveSelect,
                           (UCHAR)((Device << 4U) | 0xA0U));
    ScsiPortWritePortUchar((PUCHAR)baseIoAddress + 1,(UCHAR)0x03);
    ScsiPortWritePortUchar(&baseIoAddress->BlockCount,(UCHAR)(0x40U + Mode));

    ScsiPortWritePortUchar(&baseIoAddress->Command,
                           IDE_COMMAND_SET_FEATURES);

}

void
AcerUltraDMAMode(
    ULONG Channel,
    ULONG deviceNumber,
    ULONG Mode
    )
/*++

Routine Description:

    Set the Ultra DMA mode

Arguments:

HwDeviceExtension - ATAPI driver storage.

--*/

{
    static UCHAR UDMA[2] = {						/* map Channel to UDMAP/S register */
        UDMAP,UDMAS
    };
    UCHAR byteVal;
    UCHAR udmaData;

    ReadIdeConfig(pb,UDMA[Channel],byteVal);
    switch (Mode) {
    case 0:
        udmaData = 0xC;
        break;
    case 1:
        udmaData = 0xB;
        break;
    case 2:
        udmaData = 0xA;
        break;
    }
    byteVal = byteVal & ~(0xF<<(deviceNumber*4));
    byteVal |= udmaData<<(deviceNumber*4);
    WriteIdeConfig(pb,UDMA[Channel],byteVal);

}
#endif

void
M5229SetTaskFileTiming(
struct pb *pb,
unsigned short active,
unsigned short recovery,
unsigned short channel)

/*++

Routine Description:

    Set the timing to be used when accessing any drive's task file.

Arguments:

    Active - DIOW-/DIOR- active time in nsec.

    Cycle - DIOW-/DIOR- cycle time in nsec.

    Channel 

Return Value:

    None.

--*/

{
    UCHAR byteVal;
    static UCHAR CCBs[] = {
        PCCB,SCCB 
    };


	DbgPrint(("Task file timing clock values: %d active,%d recovery\r\n",active,recovery));

    /*
    ** Calculate active and recovery times in PCI clocks.
    */
    active = NSEC_TO_PCI_CLOCKS(active);
    recovery = NSEC_TO_PCI_CLOCKS(recovery);

	DbgPrint(("Task file timing clock counts: %d active,%d recovery\r\n",active,recovery));

    /*
    ** Encode these times into the P/SCCB register format,and store this
    ** in the register.
    */
    byteVal = recovery_count[recovery] | active_count[active];
    WriteIdeConfig(pb,CCBs[channel],byteVal);
}

/*+
 * ============================================================================
 * = pci_registers - Display/Edits PCI Registers.                             =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *   COMMAND TAG: show_ide 0 RXBZ show_ide
 *
 *   COMMAND ARGUMENT(S):
 *
 *
 *   COMMAND OPTION(S):
 *
 * 	None
 *
 *   COMMAND EXAMPLE(S):
 *
 *      >>>show_ide
 *
 *~
 *   COMMAND REFERENCES:
 *  
 *      None
 *  
 * RETURNS:
 *
 *	0 - success
 *       
 * ARGUMENTS:
 *
 * 	argc	- number of words present on command line
 *	argv	- array of pointers to words on command line
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/

#define ABLED(x) ((x) ? "en" : "dis")
#define CLOCKS_TO_NSEC(n) (((n) * 1000) / PCI_CLOCK_MHz)

void show_ide (int argc,char *argv[])
{
	struct pb pb;
    UCHAR byteVal;
    static UCHAR active[] =   { 8,1,2,3,4,5,6,7 };
    static UCHAR recovery[] = { 16,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 };
    static UCHAR setup[] = { 8,1,2,3,4,5,6,7 };

	pb.slot = 0x10;

    printf("\r\nACER M5229: IDE Status\r\n");

    ReadIdeConfig(&pb,UDMAP,byteVal);
    printf("  UDMAP  %02X\n\r",byteVal);
    printf("         Primary Drive 0 Ultra DMA %sabled\r\n",ABLED((byteVal & BIT(3))));
    printf("         Primary Drive 1 Ultra DMA %sabled\r\n",ABLED((byteVal & BIT(7))));

    ReadIdeConfig(&pb,UDMAS,byteVal);
    printf("  UDMAS  %02X\n\r",byteVal);
    printf("         Secondary Drive 0 Ultra DMA %sabled\r\n",ABLED((byteVal & BIT(3))));
    printf("         Secondary Drive 1 Ultra DMA %sabled\r\n",ABLED((byteVal & BIT(7))));

    ReadIdeConfig(&pb,PCAS,byteVal);
    printf("  PCAS   %02X: active %d clocks (%d nsec)\r\n",
                byteVal,
                active[byteVal & 7],CLOCKS_TO_NSEC(active[byteVal & 7]));

    ReadIdeConfig(&pb,PCCB,byteVal);
    printf("  PCCB   %02X: active %d clocks (%d nsec),recovery %d clocks (%d nsec)\r\n",
                byteVal,
                active[byteVal >> 4],CLOCKS_TO_NSEC(active[byteVal >> 4]),
                recovery[byteVal & 0xF],CLOCKS_TO_NSEC(recovery[byteVal & 0xF]));

    ReadIdeConfig(&pb,PCDT0,byteVal);
    printf("  PCDT0  %02X: active %d clocks (%d nsec),recovery %d clocks (%d nsec)\r\n",
                byteVal,
                active[byteVal >> 4],CLOCKS_TO_NSEC(active[byteVal >> 4]),
                recovery[byteVal & 0xF],CLOCKS_TO_NSEC(recovery[byteVal & 0xF]));

    ReadIdeConfig(&pb,PCDT1,byteVal);
    printf("  PCDT1  %02X: active %d clocks (%d nsec),recovery %d clocks (%d nsec)\r\n",
                byteVal,
                active[byteVal >> 4],CLOCKS_TO_NSEC(active[byteVal >> 4]),
                recovery[byteVal & 0xF],CLOCKS_TO_NSEC(recovery[byteVal & 0xF]));

    ReadIdeConfig(&pb,SCAS,byteVal);
    printf("  SCAS   %02X: active %d clocks (%d nsec)\r\n",
                byteVal,
                active[byteVal & 7],CLOCKS_TO_NSEC(active[byteVal & 7]));

    ReadIdeConfig(&pb,SCCB,byteVal);
    printf("  SCCB   %02X: active %d clocks (%d nsec),recovery %d clocks (%d nsec)\r\n",
                byteVal,
                active[byteVal >> 4],CLOCKS_TO_NSEC(active[byteVal >> 4]),
                recovery[byteVal & 0xF],CLOCKS_TO_NSEC(recovery[byteVal & 0xF]));

    ReadIdeConfig(&pb,SCDT0,byteVal);
    printf("  SCDT0  %02X: active %d clocks (%d nsec),recovery %d clocks (%d nsec)\r\n",
                byteVal,
                active[byteVal >> 4],CLOCKS_TO_NSEC(active[byteVal >> 4]),
                recovery[byteVal & 0xF],CLOCKS_TO_NSEC(recovery[byteVal & 0xF]));

    ReadIdeConfig(&pb,SCDT1,byteVal);
    printf("  SCDT1  %02X: active %d clocks (%d nsec),recovery %d clocks (%d nsec)\r\n",
                byteVal,
                active[byteVal >> 4],CLOCKS_TO_NSEC(active[byteVal >> 4]),
                recovery[byteVal & 0xF],CLOCKS_TO_NSEC(recovery[byteVal & 0xF]));

#if 0
    ReadIdeConfig(&pb,BMIDESR0,byteVal);
    printf("  BMIDESR0 %02X: DMA on drive 0: %s,drive 1: %s\r\n",
             byteVal,(byteVal & BIT(5)) ? "yes" : "no",(byteVal & BIT(6)) ? "yes" : "no"));
    ReadIdeConfig(&pb,BMIDESR1,byteVal);
    printf("  BMIDESR1 %02X: DMA on drive 2: %s,drive 3: %s\r\n\n",
             byteVal,(byteVal & BIT(5)) ? "yes" : "no",(byteVal & BIT(6)) ? "yes" : "no"));
#endif
}
