/*++

Copyright (c) 1996 Digital Equipment Corporation

Module Name:

    m1543c.c

Abstract:

    This file implements support for configuring the Acer m1543c
    Super I/O chip.

Author:

    Dick Bissen         15-Jun-1998

Revision History:

    Dick Bissen         15-Jun-1998 Initial version extracted from smcc93x.c.

    Jerzy Kielbasinski  08-Dec-1998 Added translation for more devices

--*/

#include	"cp$src:platform.h"
#include	"cp$inc:platform_io.h"
#include	"cp$src:common.h"
#include	"cp$inc:prototypes.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:m1543c_def.h"

#define WRITE_PORT_UCHAR( _x_, _y_ )	outportb( NULL, _x_, _y_ )
#define READ_PORT_UCHAR( _x_ )			inportb( NULL, _x_ )

/*++
// Function prototypes - helper functions
// These functions are not involved in the calling
// standard for the combo driver but are used by the
// other combo functions to do low level work.
--*/
VOID
M1543cConfigMode(
    UINT Enable
   );

static
UCHAR
M1543cReadConfig(
    ULONG RegisterIndex
   );

static
VOID
M1543cWriteConfig(
    ULONG RegisterIndex,
    UCHAR Data
   );

static
VOID
M1543cSetLogicalDevice(
    UCHAR DeviceNumber
   );

static
UCHAR
M1543cLogicalDevice(
    ULONG What
   );

/*++
// Function prototypes - combo chip interface functions
--*/

UINT
M1543cDisableDevice(
    ULONG What
   );


UINT
M1543cEnableDevice(
    ULONG What
   );

static
UINT
M1543cIsDeviceEnabled(
    ULONG What
   );

UINT
M1543cConfigureDevice(
    ULONG What,
    LONG Port,
    LONG Irq,
    LONG Dma
   );

static
UINT
M1543cGetDeviceConfig(
    ULONG What,
    PLONG Port,
    PLONG Irq,
    PLONG Dma
   );

#ifdef PM_ACPI_PLATFORM

static
ULONG
M1543cGetDeviceConfigBase(
    VOID
   );

#endif

static
UINT
M1543cSetDeviceOption(
    ULONG What,
    ULONG Option,
    ULONG Value
   );

static
ULONG
M1543cGetDeviceOption(
    ULONG What,
    ULONG Option
   );

/*
// Pointer to the combo chip
*/
static PM1543c_CONFIG_REGS M1543cDevice = NULL;

/*
// List of all possible addresses for the combo chip
*/
static ULONG M1543c_Addresses[] = {
  0x3f0,        /* primary address */
  0x370,        /* secondary address */
  0             /* end of list */
};


extern int null_procedure( void );

int m1543c_init( void );
int m1543c_open( struct FILE *fp, char *info, char *next, char *mode );
int m1543c_read( struct FILE *fp, int size, int number, unsigned char *buf );
int m1543c_write( struct FILE *fp, int size, int number, unsigned char *buf );
int m1543c_close( struct FILE *fp );

/*
// The Acer m1543c COMBO_DRIVER structure
*/
struct DDB m1543c_ddb = {
  L"Acer",				/* how this routine wants to be called	*/
	m1543c_read,		/* read routine				*/
	m1543c_write,		/* write routine			*/
	m1543c_open,		/* open routine				*/
	m1543c_close,		/* close routine			*/
	null_procedure,		/* name expansion routine		*/
	null_procedure,		/* delete routine			*/
	null_procedure,		/* create routine			*/
	null_procedure,		/* setmode				*/
	null_procedure,		/* validation routine			*/
	0,					/* class specific use			*/
	1,					/* allows information			*/
	0,					/* must be stacked			*/
	0,					/* is a flash update driver		*/
	0,					/* is a block device			*/
	0,					/* not seekable				*/
	0,					/* is an ethernet device		*/
	0,					/* is a filesystem driver		*/
};

PM1543c_CONFIG_REGS *M1543c_detect(void)
/*++

Routine Description:

    This function checks for the Acer m1543c Super I/O and 
    if it is found sets up the combo chip function pointers.

Arguments:

    None

Return Value:

    Pointer to M1543cDriver if chip found, else NULL.

--*/
{
	UCHAR DeviceId;
	int i;

	/*
	// Try to find the device
	*/
	for(i = 0; M1543c_Addresses[i] != 0; i++)
	{
		/*
		// Translate the address
		//
		// Note that we put the result in M1543cDevice even
		// though we don't yet know if the device is at this address.
		// The access functions access the device through 
		// M1543cDevice so we need to set it even when we are 
		// looking...
		*/
		M1543cDevice = (PM1543c_CONFIG_REGS)M1543c_Addresses[i];

		/*
		// Enter config mode
		*/
		M1543cConfigMode(TRUE);

		/*
		// Read the device id
		*/
		DeviceId = M1543cReadConfig(M1543c_DEVICE_ID_INDEX);

		/*
		// Leave config mode
		*/
		M1543cConfigMode(FALSE);

		/*
		// Does the device id match?
		*/
#if M1543C
		if (DeviceId == M1543c_DEVICE_ID) {
#endif
#if M1535C
		if (DeviceId == M1535d_DEVICE_ID) {
#endif
			/*
			// We found it - break out of the search loop
			*/
			break;
		}

		/*
		// If we get here, we did not find a 37c93x at this address
		// so NULL out the device pointer.
		*/
		M1543cDevice = NULL;
	}

	/*
	// Return the driver
	*/
	return M1543cDevice;
}

VOID
M1543cConfigMode(
    UINT Enable
   )
/*++

Routine Description:

    This function puts the Super I/O into and takes it out of config mode.

Arguments:

    Enable      - TRUE to enter config mode, FALSE to leave config mode

Return Value:

    None

--*/
{
  if (Enable) {
    WRITE_PORT_UCHAR((PUCHAR)&M1543cDevice->ConfigurationIndexRegister,
                     M1543c_CONFIG_ON_KEY1);
    WRITE_PORT_UCHAR((PUCHAR)&M1543cDevice->ConfigurationIndexRegister,
                     M1543c_CONFIG_ON_KEY2);
  } else {
    WRITE_PORT_UCHAR((PUCHAR)&M1543cDevice->ConfigurationIndexRegister,
                     M1543c_CONFIG_OFF_KEY);
  }
}

static
UCHAR
M1543cReadConfig(
    ULONG RegisterIndex
   )
/*++

Routine Description:

    This function reads an Super I/O configuration register. It assumes
    that the device is already in config mode.

Arguments:

    RegisterIndex       - Which register

Return Value:

    Returns the data read from the config register.

--*/
{
  UCHAR DataByte;

  /*
  // Select the register
  */
  WRITE_PORT_UCHAR((PUCHAR)&M1543cDevice->ConfigurationIndexRegister,
                   (UCHAR)RegisterIndex);

  /*
  // Read the data
  */
  DataByte = 
    READ_PORT_UCHAR((PUCHAR)&M1543cDevice->ConfigurationDataRegister);

  /*
  // Return it
  */
  return DataByte;
}

static
VOID
M1543cWriteConfig(
    ULONG RegisterIndex,
    UCHAR Data
   )
/*++

Routine Description:

    This function writes an Super I/O configuration register. It assumes
    that the device is already in config mode.

Arguments:

    RegisterIndex       - Which register
    Data                - Data to write to register

Return Value:

    None.

--*/
{
  /*
  // Select the register
  */
  WRITE_PORT_UCHAR((PUCHAR)&M1543cDevice->ConfigurationIndexRegister,
                   (UCHAR)RegisterIndex);

  /*
  // Write the data
  */
  WRITE_PORT_UCHAR((PUCHAR)&M1543cDevice->ConfigurationDataRegister,
                   Data);
}

static
VOID
M1543cSetLogicalDevice(
    UCHAR DeviceNumber
   )
/*++

Routine Description:

    This function sets the Super I/O up to talk to a specific
    logical device. It assumes that the device is already in 
    config mode.

Arguments:

    DeviceNumber        - Which device

Return Value:

    None.

--*/
{
  M1543cWriteConfig(M1543c_LOGICAL_DEVICE_NUMBER_INDEX,
                       DeviceNumber);
}

static
UCHAR
M1543cLogicalDevice(
    ULONG What
   )
{
	UCHAR LogicalDevice = M1543c_INVALID_DEVICE;

	/*
	// Translate the requested device
	*/
	switch(What)
	{
		case SERIAL_0:
			LogicalDevice = M1543c_SERIAL_1;
			break;
		case SERIAL_1:
			LogicalDevice = M1543c_SERIAL_2;
			break;
		case PARALLEL_0:
			LogicalDevice = M1543c_PARALLEL_PORT;
			break;
		case FLOPPY_0:
			LogicalDevice = M1543c_FLOPPY_DISK;
			break;
		case KBD_CTRL_0:
			LogicalDevice = M1543c_KEYBOARD;
			break;
		
		/*case SERIAL2:
		LogicalDevice = M1543c_SERIAL_3;
		break;
		case HOTKEY:
		LogicalDevice = M1543c_HOTKEY;
		break;
		case COMMONIO:
		LogicalDevice = M1543c_COMMONIO;
		break;*/
	}

	return LogicalDevice;
}

UINT
M1543cDisableDevice(
    ULONG What
   )
/*++

Routine Description:

    This function disables a device function within the Super I/O.

Arguments:

    What        - Which device function.

Return Value:

    TRUE if the device function was disabled, else FALSE.

--*/
{
  UINT RetVal = FALSE;

  /*
  // Set the Super I/O into config mode
  */
  M1543cConfigMode(TRUE);


  /*
  // Find out which device to check and set up to talk to 
  // that logical device.
  */
  if (M1543cLogicalDevice(What) != M1543c_INVALID_DEVICE) {
    /*
    // Set up to talk to the device
    */
    M1543cSetLogicalDevice(M1543cLogicalDevice(What));

    /*
    // Disable it
    */
    M1543cWriteConfig(M1543c_ACTIVATE_INDEX,
                         M1543c_DEVICE_OFF);
    /*
    // Remember that we disabled it
    */
    RetVal = TRUE;
  }

  /*    
  // Take it out of config mode
  */
  M1543cConfigMode(FALSE);

  return RetVal;
}

UINT
M1543cEnableDevice(
    ULONG What
   )
/*++

Routine Description:

    This function disables a device function within the Super I/O.

Arguments:

    What        - Which device function.

Return Value:

    TRUE if the device function was enabled, else FALSE.

--*/
{
  UINT RetVal = FALSE;

  /*
  // Set the Super I/O into config mode
  */
  M1543cConfigMode(TRUE);

  /*
  // Find out which device to check and set up to talk to 
  // that logical device.
  */
  if (M1543cLogicalDevice(What) != M1543c_INVALID_DEVICE) {
    /*
    // Set up to talk to the device
    */
    M1543cSetLogicalDevice(M1543cLogicalDevice(What));

    /*
    // Enable it
    */
    M1543cWriteConfig(M1543c_ACTIVATE_INDEX,
                         M1543c_DEVICE_ON);
    /*
    // Remember that we enabled it
    */
    RetVal = TRUE;
  }

  /*    
  // Take it out of config mode
  */
  M1543cConfigMode(FALSE);

  return RetVal;
}

static
UINT
M1543cIsDeviceEnabled(
    ULONG What
   )
/*++

Routine Description:

    This function determines whether a device function within 
    the Super I/O is enabled.

Arguments:

    What        - Which device function.

Return Value:

    TRUE if the device function is enabled, else FALSE.
    Note that this function returns FALSE under two conditions.
    It returns FALSE if the device is disabled and it returns
    FALSE if we cannot talk to the requested device (doesn't exist?)

--*/
{
  UINT RetVal = FALSE;

  /*
  // Set the Super I/O into config mode
  */
  M1543cConfigMode(TRUE);

  /*
  // Find out which device to check and set up to talk to 
  // that logical device.
  */
  if (M1543cLogicalDevice(What) != M1543c_INVALID_DEVICE) {
    /*
    // Set up to talk to the device
    */
    M1543cSetLogicalDevice(M1543cLogicalDevice(What));

    /*
    // Get it's state
    */
    RetVal = M1543cReadConfig(M1543c_ACTIVATE_INDEX);
  }

  /*    
  // Take it out of config mode
  */
  M1543cConfigMode(FALSE);

  return RetVal;
}

UINT
M1543cConfigureDevice(
    ULONG What,
    LONG Port,
    LONG Irq,
    LONG Dma
   )
/*++

Routine Description:

    This function configures a device function within the Super I/O.
    Any parameter which is not UNUSED_OPTION will be configured. It 
    is the responsibility of the caller to set parameters which should
    not be configured or don't make sense for the device to 
    UNUSED_OPTION.

Arguments:

    What        - Which device function.
    Port        - I/O port for the device to use (UNUSED_OPTION if none).
    Irq         - IRQ for the device to use (UNUSED_OPTION if none).
    Dma         - DMA Channel for the device to use (UNUSED_OPTION if none).

Return Value:

    TRUE if the device function was configured, else FALSE.

--*/
{
  UINT RetVal = FALSE;

  /*
  // Set the Super I/O into config mode
  */
  M1543cConfigMode(TRUE);

  /*
  // Find out which device to check and set up to talk to 
  // that logical device.
  */
  if (M1543cLogicalDevice(What) != M1543c_INVALID_DEVICE) {
    /*
    // Set up to talk to the device
    */
    M1543cSetLogicalDevice(M1543cLogicalDevice(What));

    /*
    // Configure the port
    */
    if ((Port & ~0xffff) == 0) {
      M1543cWriteConfig(M1543c_ADDR_LOW_INDEX,
                           (UCHAR)(Port & 0xff));
      M1543cWriteConfig(M1543c_ADDR_HI_INDEX,
                           (UCHAR)((Port >> 8) & 0xff));
    }

    /*
    // Configure the Irq
    */
    if ((Irq & ~0xff) == 0) {
      M1543cWriteConfig(M1543c_INTERRUPT_INDEX,
                           (UCHAR)(Irq & 0xff));
    }

    /*
    // Configure the DMA Channel
    */
    if ((Dma & ~0xff) == 0) {
      M1543cWriteConfig(M1543c_DMA_CHANNEL_INDEX,
                           (UCHAR)(Dma & 0xff));
    }

    /*
    // Remember that the device was configured
    */
    RetVal = TRUE;
  }

  /*    
  // Take it out of config mode
  */
  M1543cConfigMode(FALSE);

  return RetVal;
}

static
UINT
M1543cGetDeviceConfig(
    ULONG What,
    PLONG Port,
    PLONG Irq,
    PLONG Dma
   )
/*++

Routine Description:

    This function retrieves the configuration of a device function within 
    the Super I/O. Any unused parameters will be set to UNUSED_OPTION. Any
    parameter which is not desired can be a NULL pointer.

Arguments:

    What        - Which device function.
    Port        - Pointer to receive the I/O port.
    Irq         - Pointer to receive the IRQ.
    Dma         - Pointer to receive the DMA Channel.

Return Value:

    TRUE if the device configuration was retreived, else FALSE.
    The data pointed to by Port, Irq, and Dma may be modified even if
    the configuration is not retreived.

--*/
{
  UINT RetVal = FALSE;
  UINT GetConfiguration = FALSE;

  /*
  // Initialize all requested parameters to 0 and remember if any are
  // requested.
  */
  if (Port != NULL) {
    GetConfiguration = TRUE;
    *Port = 0;
  }

  if (Irq != NULL) {
    GetConfiguration = TRUE;
    *Irq = 0;
  }

  if (Dma != NULL) {
    GetConfiguration = TRUE;
    *Dma = 0;
  }

  /*
  // Only look at the configuration if any parameters were requested
  */
  if (GetConfiguration) {
    /*
    // Set the Super I/O into config mode
    */
    M1543cConfigMode(TRUE);

    /*
    // Find out which device to check and set up to talk to 
    // that logical device.
    */
    if (M1543cLogicalDevice(What) != M1543c_INVALID_DEVICE) {
      /*
      // Set up to talk to the device
      */
      M1543cSetLogicalDevice(M1543cLogicalDevice(What));

      /*
      // If the device is NOT the floppy, set Dma to UNUSED_OPTION
      // as only the floppy controller gets configured for DMA
      */
      if ((Dma != NULL) && (What != FLOPPY_0)) {
        *Dma = UNUSED_OPTION;
      }

      /*
      // Get the configuration - only look at items which have valid
      // pointers and have not been set to UNUSED_OPTION
      //
      // First the Port
      */
      if ((Port != NULL) && (*Port != UNUSED_OPTION)) {
        *Port = (ULONG)M1543cReadConfig(M1543c_ADDR_LOW_INDEX);
        *Port = *Port |
          ((ULONG)M1543cReadConfig(M1543c_ADDR_HI_INDEX) << 8);
      }

      /*
      // Then the Irq
      */
      if ((Irq != NULL) && (*Irq != UNUSED_OPTION)) {
        *Irq = (ULONG)M1543cReadConfig(M1543c_INTERRUPT_INDEX);
      }

      /*
      // And finally the DMA Channel
      */
      if ((Dma != NULL) && (*Dma != UNUSED_OPTION)) {
        *Dma = (ULONG)M1543cReadConfig(M1543c_DMA_CHANNEL_INDEX);
      }

      /*
      // And remember that we were able to read the configuration
      */
      RetVal = TRUE;
    }

    /*
    // Take it out of config mode
    */
    M1543cConfigMode(FALSE);
  }

  return RetVal;
}

UINT
M1543cSetDeviceOption(
    ULONG What,
    ULONG Option,
    ULONG Value
   )
/*++

Routine Description:

    This function sets an option for an Super I/O device function.

Arguments:

    What        - Which device function.
    Option      - Which option.
    Value       - Value to set option to.

Return Value:

    TRUE if the option was set, else FALSE.

--*/
{
  UINT RetVal = FALSE;

  /*
  // Set the Super I/O into config mode
  */
  M1543cConfigMode(TRUE);

  /*
  // Find out which device to check and set up to talk to 
  // that logical device.
  */
  if (M1543cLogicalDevice(What) != M1543c_INVALID_DEVICE) {
    /*
    // Set up to talk to the device
    */
    M1543cSetLogicalDevice(M1543cLogicalDevice(What));

    /*
    // Set the option
    */
    M1543cWriteConfig((UCHAR)(Option & 0xff),
                         (UCHAR)(Value & 0xff));

    /*
    // And remember that we set it
    */
    RetVal = TRUE;
  }

  /*    
  // Take it out of config mode
  */
  M1543cConfigMode(FALSE);
  
  return RetVal;
}

static
ULONG
M1543cGetDeviceOption(
    ULONG What,
    ULONG Option
   )
/*++

Routine Description:

    This function reads an option for an Super I/O device function.

Arguments:

    What        - Which device function.
    Option      - Which option.

Return Value:

    The option value. If the device does not exist, UNUSED_OPTION is returned.

--*/
{
  UINT DeviceOK = FALSE;
  ULONG RetVal = UNUSED_OPTION;

  /*
  // Set the Super I/O into config mode
  */
  M1543cConfigMode(TRUE);

  /*
  // Find out which device to check and set up to talk to 
  // that logical device.
  */
  if (M1543cLogicalDevice(What) != M1543c_INVALID_DEVICE) {
    /*
    // Set up to talk to the device
    */
    M1543cSetLogicalDevice(M1543cLogicalDevice(What));

    /*
    // Read the option
    */
    RetVal = (ULONG)M1543cReadConfig((UCHAR)(Option & 0xff));

  }

  /*    
  // Take it out of config mode
  */
  M1543cConfigMode(FALSE);
  
  return RetVal;
}

int m1543c_init ( void )
{
    struct INODE *ip;

    allocinode( m1543c_ddb.name, 1, &ip );
    ip->dva = &m1543c_ddb;
    ip->attr = ATTR$M_WRITE | ATTR$M_READ;
    ip->len[0] = 0x30;
    ip->misc = 0;
    INODE_UNLOCK( ip );

    return msg_success;
}

int m1543c_open(struct FILE *fp, char *info, char *next, char *mode)
{
	struct INODE *ip;

	/*
	** Allow multiple readers but only one writer.  ip->misc keeps track
	** of the number of writers
	*/
	ip = fp->ip;

	INODE_LOCK(ip);

	if (fp->mode & ATTR$M_WRITE)
	{
		if (ip->misc)
		{
			INODE_UNLOCK(ip);
			return msg_failure;	    /* too many writers */
		}
	
		ip->misc++;
	}

	/*
	** Treat the information field as a byte offset
	*/
	*fp->offset = xtoi(info);
	INODE_UNLOCK(ip);

	return msg_success;
}

int m1543c_close(struct FILE *fp)
{
    struct INODE *ip;

    ip = fp->ip;

    if (fp->mode & ATTR$M_WRITE)
	{
		INODE_LOCK(ip);
		ip->misc--;
		INODE_UNLOCK(ip);
    }

    return msg_success;
}

int m1543c_read(struct FILE *fp, int size, int number, unsigned char *buf)
{
	int i;
	int length;
	int nbytes;
	struct INODE *ip;

	/*
	** Always access a byte at a time
	*/
	ip = fp->ip;
	length = size * number;
	nbytes = 0;

	M1543cConfigMode(TRUE);

	for (i = 0; i < length; i++)
	{
		if (!inrange(*fp->offset, 0, ip->len[0]))
			break;

		*buf++ = M1543cReadConfig(*fp->offset);
		*fp->offset += 1;
		nbytes++;
	}
	M1543cConfigMode(FALSE);

	return nbytes;
}

int m1543c_write(struct FILE *fp, int size, int number, unsigned char *buf)
{
	int i;
	int length;
	int nbytes;
	struct INODE *ip;

	/*
	** Always access a byte at a time
	*/
	ip = fp->ip;
	length = size * number;
	nbytes = 0;

	M1543cConfigMode(TRUE);

	for (i = 0; i < length; i++)
	{
		if (!inrange(*fp->offset, 0, ip->len[0])) 
			break;

		M1543cWriteConfig(*fp->offset, *buf);
		*fp->offset += 1;
		buf++;
		nbytes++;
	}

	M1543cConfigMode(FALSE);

	return nbytes;
}

/*+
 * ============================================================================
 * = M1543cDisplayDeviceInfo - Displays M1543C Southbridge info               =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *    COMMAND FMT: southbridge_info 3 S 0 southbridge_info
 *
 *    COMMAND FORM:
 *
 *	southbridge_info
 *
 *   COMMAND TAG: M1543cDisplayDeviceInfo 0 RXBZ southbridge_info
 *
 *   COMMAND ARGUMENT(S):
 *
 * 	 None
 *
 *   COMMAND OPTION(S):
 *
 * 	 None
 *
 *   COMMAND EXAMPLE(S):
 *~
 *      >>>southbridge_info
 *      Serial 0:    Enabled [ Port 0x3f8, IRQ 4 ] 
 *      Serial 1:    Enabled [ Port 0x2f8, IRQ 3 ] 
 *      Parallel:    Enabled [ Port 0x3bc, IRQ 7 ] 
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

M1543cDisplayDeviceInfo(
int argc,
char *argv[])
/*++

Routine Description:

    This function displays the state of the Super I/O device functions.

Arguments:

    None

Return Value:

    TRUE.

--*/
{
	struct PCB *pcb;
	struct {
		unsigned int Enabled;
		ULONG Addr;
		ULONG Irq;
		ULONG Irq2;
		ULONG Dma;
	} SuperIODevice[13];


	pcb = getpcb();
	

	/* Set the Super I/O into config mode */
	M1543cConfigMode(TRUE);

	/* Get the status of onboard com1 */
	M1543cGetDeviceConfig(SERIAL_0,
						  &SuperIODevice[M1543c_SERIAL_1].Addr,
						  &SuperIODevice[M1543c_SERIAL_1].Irq,
						  &SuperIODevice[M1543c_SERIAL_1].Dma);
	SuperIODevice[M1543c_SERIAL_1].Enabled = M1543cIsDeviceEnabled(SERIAL_0);

	/* Get the status of onboard com2 */
	M1543cGetDeviceConfig(SERIAL_1,
						  &SuperIODevice[M1543c_SERIAL_2].Addr,
						  &SuperIODevice[M1543c_SERIAL_2].Irq,
						  &SuperIODevice[M1543c_SERIAL_2].Dma);
	SuperIODevice[M1543c_SERIAL_2].Enabled = M1543cIsDeviceEnabled(SERIAL_1);

	/* Get the status of onboard com3 */
	/*M1543cGetDeviceConfig(SERIAL_2,
						  &SuperIODevice[M1543c_SERIAL_3].Addr,
						  &SuperIODevice[M1543c_SERIAL_3].Irq,
						  &SuperIODevice[M1543c_SERIAL_3].Dma);
	SuperIODevice[M1543c_SERIAL_3].Enabled = M1543cIsDeviceEnabled(SERIAL_2);*/

	/* Get the status of onboard parallel port */
	M1543cGetDeviceConfig(PARALLEL_0,
						  &SuperIODevice[M1543c_PARALLEL_PORT].Addr,
						  &SuperIODevice[M1543c_PARALLEL_PORT].Irq,
						  &SuperIODevice[M1543c_PARALLEL_PORT].Dma);
	SuperIODevice[M1543c_PARALLEL_PORT].Enabled = M1543cIsDeviceEnabled(PARALLEL_0);

	/* Get the status of onboard fdc */
	M1543cGetDeviceConfig(FLOPPY_0,
						  &SuperIODevice[M1543c_FLOPPY_DISK].Addr,
						  &SuperIODevice[M1543c_FLOPPY_DISK].Irq,
						  &SuperIODevice[M1543c_FLOPPY_DISK].Dma);
	SuperIODevice[M1543c_FLOPPY_DISK].Enabled = M1543cIsDeviceEnabled(FLOPPY_0);

	/* Get the status of onboard keyboard/mouse controller */
	M1543cGetDeviceConfig(KBD_CTRL_0,
						  &SuperIODevice[M1543c_KEYBOARD].Addr,
						  &SuperIODevice[M1543c_KEYBOARD].Irq,
						  &SuperIODevice[M1543c_KEYBOARD].Dma);
	SuperIODevice[M1543c_KEYBOARD].Addr = 0x60; 
	SuperIODevice[M1543c_KEYBOARD].Irq2 = M1543cGetDeviceOption(KBD_CTRL_0, M1543c_SECOND_INTERRUPT_INDEX);
	SuperIODevice[M1543c_KEYBOARD].Enabled = M1543cIsDeviceEnabled(KBD_CTRL_0);

	/* Turn off Super I/O config */
	M1543cConfigMode(FALSE);

	/* Display the info */
	if (SuperIODevice[M1543c_SERIAL_1].Enabled)
	{
		fprintf(pcb->pcb$a_stdout,"  Serial 0:    Enabled [ Port 0x%x, IRQ %d ] \r\n",SuperIODevice[M1543c_SERIAL_1].Addr,SuperIODevice[M1543c_SERIAL_1].Irq);
	}
	else
	{
		fprintf(pcb->pcb$a_stdout,"  Serial 0:    Disabled\r\n");
	}

	if (SuperIODevice[M1543c_SERIAL_2].Enabled)
	{
		fprintf(pcb->pcb$a_stdout,"  Serial 1:    Enabled [ Port 0x%x, IRQ %d ] \r\n",SuperIODevice[M1543c_SERIAL_2].Addr,SuperIODevice[M1543c_SERIAL_2].Irq);
	}
	else
	{
		fprintf(pcb->pcb$a_stdout,"  Serial 1:    Disabled\r\n");
	}

	/*if (SuperIODevice[M1543c_SERIAL_3].Enabled)
	{
		fprintf(pcb->pcb$a_stdout,"  Serial 2:    Enabled [ Port 0x%x, IRQ %d ] \r\n",SuperIODevice[M1543c_SERIAL_3].Addr,SuperIODevice[M1543c_SERIAL_3].Irq);
	}
	else
	{
		fprintf(pcb->pcb$a_stdout,"  Serial 2:    Disabled\r\n");
	}*/

	if (SuperIODevice[M1543c_PARALLEL_PORT].Enabled)
	{
		fprintf(pcb->pcb$a_stdout,"  Parallel:    Enabled [ Port 0x%x, IRQ %d ] \r\n",SuperIODevice[M1543c_PARALLEL_PORT].Addr,SuperIODevice[M1543c_PARALLEL_PORT].Irq);
	}
	else
	{
		fprintf(pcb->pcb$a_stdout,"  Parallel:    Disabled\r\n");
	}

#if !SHARK
	if (SuperIODevice[M1543c_FLOPPY_DISK].Enabled)
	{
		fprintf(pcb->pcb$a_stdout,"  Floppy Ctrl: Enabled [ Port 0x%x, IRQ %d, DMA %d ] \r\n",SuperIODevice[M1543c_FLOPPY_DISK].Addr,SuperIODevice[M1543c_FLOPPY_DISK].Irq,SuperIODevice[M1543c_FLOPPY_DISK].Dma);
	}
	else
	{
		fprintf(pcb->pcb$a_stdout,"  Floppy Ctrl: Disabled\r\n");
	}

	if (SuperIODevice[M1543c_KEYBOARD].Enabled)
	{
		fprintf(pcb->pcb$a_stdout,"  Kbd / Mouse: Enabled [ Port 0x%x, KBD IRQ %d, MOUSE IRQ %d ]\r\n",SuperIODevice[M1543c_KEYBOARD].Addr,SuperIODevice[M1543c_KEYBOARD].Irq,SuperIODevice[M1543c_KEYBOARD].Irq2);
	}
	else
	{
		fprintf(pcb->pcb$a_stdout,"  Kbd / Mouse: Disabled\r\n");
	}
#endif

	return TRUE;
}
