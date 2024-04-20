/* file:	smcc669.c
 *
 * Copyright (C) 1997 by
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
 * and  should  not  be  construed  as a commitment by digital equipment
 * corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by digital.
 */

/*
 *++
 *  FACILITY:
 *
 *      Alpha SRM Console Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *	SMC37c669 Super I/O controller configuration routines.
 *
 *  AUTHORS:
 *
 *	Eric Rasmussen
 *
 *  CREATION DATE:
 *  
 *	28-Jan-1997
 *
 *  MODIFICATION HISTORY:
 *
 *	dtr	30-Sep-1997	Added exteranl function call for drq/irq
 *				assignments.
 *	
 *	er	01-May-1997	Fixed pointer conversion errors in 
 *
 *				SMC37c669_get_device_config().
 *      er	28-Jan-1997	Initial version.
 *
 *
 *	tna	05-Apr-1999	Conditionalized for Yukona
 *--
 */
/* $INCLUDE_OPTIONS$ */
#include    "cp$inc:platform_io.h"
/* $INCLUDE_OPTIONS_END$ */
#include    "cp$src:common.h"
#include    "cp$inc:prototypes.h"
#include    "cp$src:kernel_def.h"
#include    "cp$src:msg_def.h"
#include    "cp$src:smcc669_def.h"
/* Platform-specific includes */
#include    "cp$src:platform.h"

#define wb( _x_, _y_ )	outportb( NULL, _x_, _y_ )
#define rb( _x_ )	inportb( NULL, _x_ )

/*
** Local storage for device configuration information.
**
** Since the SMC37c669 does not provide an explicit
** mechanism for enabling/disabling individual device 
** functions, other than unmapping the device, local 
** storage for device configuration information is 
** allocated here for use in implementing our own 
** function enable/disable scheme.
*/
static struct DEVICE_CONFIG {
    unsigned int port1;
    unsigned int port2;
    unsigned int irq;
    unsigned int drq;
} local_config [NUM_FUNCS];

/*
** List of all possible addresses for the Super I/O chip
*/
static unsigned int SMC37c669_Addresses[] =
    {
	0x3f0,	    /* Primary address	    */
	0x370,	    /* Secondary address    */
	0	    /* End of list	    */
    };

/*
** Global Pointer to the Super I/O device
*/
static SMC37c669_CONFIG_REGS *SMC37c669 = NULL;

/*
** IRQ Translation Table
**
** The IRQ translation table is a list of SMC37c669 device 
** and standard ISA IRQs.
**
*/
SMC37c669_IRQ_TRANSLATION_ENTRY *SMC37c669_irq_table; 

/*
** The following definition is for the default IRQ 
** translation table.
*/

static SMC37c669_IRQ_TRANSLATION_ENTRY SMC37c669_default_irq_table[ ] = 
    { 
	{ SMC37c669_DEVICE_IRQ_A, -1 }, 
	{ SMC37c669_DEVICE_IRQ_B, -1 }, 
	{ SMC37c669_DEVICE_IRQ_C, 7 }, 
	{ SMC37c669_DEVICE_IRQ_D, 6 }, 
	{ SMC37c669_DEVICE_IRQ_E, 4 }, 
	{ SMC37c669_DEVICE_IRQ_F, 3 }, 
	{ SMC37c669_DEVICE_IRQ_H, -1 }, 
	{ -1, -1 } /* End of table */
    };

/*
** DRQ Translation Table
**
** The DRQ translation table is a list of SMC37c669 device and
** ISA DMA channels.
**
*/
SMC37c669_DRQ_TRANSLATION_ENTRY *SMC37c669_drq_table; 

/*
** The following definition is the default DRQ
** translation table.
*/
static SMC37c669_DRQ_TRANSLATION_ENTRY SMC37c669_default_drq_table[ ] = 
    { 
	{ SMC37c669_DEVICE_DRQ_A, 2 }, 
	{ SMC37c669_DEVICE_DRQ_B, 3 }, 
	{ SMC37c669_DEVICE_DRQ_C, -1 }, 
	{ -1, -1 } /* End of table */
    };

/*
** Local Function Prototype Declarations
*/

static unsigned int SMC37c669_is_device_enabled( 
    unsigned int func 
);

static unsigned int SMC37c669_get_device_config( 
    unsigned int func, 
    int *port, 
    int *irq, 
    int *drq 
);

void SMC37c669_config_mode( 
    unsigned int enable 
);

static unsigned char SMC37c669_read_config( 
    unsigned char index 
);

void SMC37c669_write_config( 
    unsigned char index, 
    unsigned char data 
);

static void SMC37c669_init_local_config( void );

static struct DEVICE_CONFIG *SMC37c669_get_config(
    unsigned int func
);

static int SMC37c669_xlate_irq(
    unsigned int irq 
);

static int SMC37c669_xlate_drq(
    unsigned int drq 
);

int SMC37c669_init_irq_drq_tables(SMC37c669_IRQ_TRANSLATION_ENTRY **SMC37c669_default_irq_table,
	SMC37c669_DRQ_TRANSLATION_ENTRY **SMC37c669_default_drq_table);
/*
** External Data Declarations
*/

extern struct LOCK spl_atomic;

/*
** External Function Prototype Declarations
*/

/* From kernel_alpha.mar */
extern spinlock( 
    struct LOCK *spl 
);

extern spinunlock( 
    struct LOCK *spl 
);

/* From printf.c */
extern int qprintf( 
    char *format, ... 
);

/* From filesys.c */
int allocinode(
    char *name, 
    int can_create, 
    struct INODE **ipp
);

extern int null_procedure( void );

int smcc669_init( void );
int smcc669_open( struct FILE *fp, char *info, char *next, char *mode );
int smcc669_read( struct FILE *fp, int size, int number, unsigned char *buf );
int smcc669_write( struct FILE *fp, int size, int number, unsigned char *buf );
int smcc669_close( struct FILE *fp );

struct DDB smc_ddb = {
	"smc",			/* how this routine wants to be called	*/
	smcc669_read,		/* read routine				*/
	smcc669_write,		/* write routine			*/
	smcc669_open,		/* open routine				*/
	smcc669_close,		/* close routine			*/
	null_procedure,		/* name expansion routine		*/
	null_procedure,		/* delete routine			*/
	null_procedure,		/* create routine			*/
	null_procedure,		/* setmode				*/
	null_procedure,		/* validation routine			*/
	0,			/* class specific use			*/
	1,			/* allows information			*/
	0,			/* must be stacked			*/
	0,			/* is a flash update driver		*/
	0,			/* is a block device			*/
	0,			/* not seekable				*/
	0,			/* is an ethernet device		*/
	0,			/* is a filesystem driver		*/
};


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function detects the presence of an SMC37c669 Super I/O
**	controller.
**
**  FORMAL PARAMETERS:
**
**	None
**
**  RETURN VALUE:
**
**      Returns a pointer to the device if found, otherwise,
**	the NULL pointer is returned.
**
**  SIDE EFFECTS:
**
**      None
**
**--
*/
void *SMC37c669_detect( void )
{
    int i;
    SMC37c669_DEVICE_ID_REGISTER id;

    for ( i = 0;  SMC37c669_Addresses[i] != 0;  i++ ) {
/*
** Initialize the device pointer even though we don't yet know if
** the controller is at this address.  The support functions access
** the controller through this device pointer so we need to set it
** even when we are looking ...
*/
    	SMC37c669 = ( SMC37c669_CONFIG_REGS * )SMC37c669_Addresses[i];

/*
** Enter configuration mode
*/
	SMC37c669_config_mode( TRUE );
/*
** Read the device id
*/
#if 1
	id.as_uchar = SMC37c669_read_config( SMC37c669_DEVICE_ID_INDEX );
#else
	id.as_uchar = SMC37c669_DEVICE_ID;
#endif

/*
** Exit configuration mode
*/
	SMC37c669_config_mode( FALSE );
/*
** Does the device id match?  If so, assume we have found an
** SMC37c669 controller at this address.
*/

	if ( id.by_field.device_id == SMC37c669_DEVICE_ID ) {
/*
** Initialize the IRQ and DRQ translation tables.  Go external if need be.
*/

	    if (!(SMC37c669_init_irq_drq_tables(&SMC37c669_irq_table,
		    &SMC37c669_drq_table)))
	    {
		SMC37c669_irq_table = SMC37c669_default_irq_table;
		SMC37c669_drq_table = SMC37c669_default_drq_table;
	    }
/*
** erfix
**
** If the platform can't use the IRQ and DRQ defaults set up in this 
** file, it should call a platform-specific external routine at this 
** point to reset the IRQ and DRQ translation table pointers to point 
** at the appropriate tables for the platform.  If the defaults are 
** acceptable, then the external routine should do nothing.
*/

/*
** Put the chip back into configuration mode
*/
	    SMC37c669_config_mode( TRUE );
/*
** Initialize local storage for configuration information
*/
	    SMC37c669_init_local_config( );
/*
** Exit configuration mode
*/
	    SMC37c669_config_mode( FALSE );
/*
** SMC37c669 controller found, break out of search loop
*/
	    break;
	}
	else {
/*
** Otherwise, we did not find an SMC37c669 controller at this
** address so set the device pointer to NULL.
*/
	    SMC37c669 = NULL;
	}
    }
    return SMC37c669;
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function enables an SMC37c669 device function.
**
**  FORMAL PARAMETERS:
**
**      func:
**          Which device function to enable
**
**  RETURN VALUE:
**
**      Returns TRUE is the device function was enabled, otherwise, FALSE
**
**  SIDE EFFECTS:
**
**      {@description or none@}
**
**  DESIGN:
**
**      Enabling a device function in the SMC37c669 controller involves
**	setting all of its mappings (port, irq, drq ...).  A local 
**	"shadow" copy of the device configuration is kept so we can
**	just set each mapping to what the local copy says.
**
**	This function ALWAYS updates the local shadow configuration of
**	the device function being enabled, even if the device is always
**	enabled.  To avoid replication of code, functions such as
**	configure_device set up the local copy and then call this 
**	function to the update the real device.
**
**--
*/
unsigned int SMC37c669_enable_device ( unsigned int func )
{
    unsigned int ret_val;
/*
** Put the device into configuration mode
*/
    SMC37c669_config_mode( TRUE );
    switch ( func ) {
    	case SERIAL_0:
	    {
	    	SMC37c669_SERIAL_BASE_ADDRESS_REGISTER base_addr;
		SMC37c669_SERIAL_IRQ_REGISTER irq;
/*
** Enable the serial 1 IRQ mapping
*/
	    	irq.as_uchar = 
		    SMC37c669_read_config( SMC37c669_SERIAL_IRQ_INDEX );

		irq.by_field.uart1_irq =
		    SMC37c669_RAW_DEVICE_IRQ(
			SMC37c669_xlate_irq( local_config[ func ].irq )
		    );

		SMC37c669_write_config( SMC37c669_SERIAL_IRQ_INDEX, irq.as_uchar );
/*
** Enable the serial 1 port base address mapping
*/
		base_addr.as_uchar = 0;
		base_addr.by_field.addr9_3 = local_config[ func ].port1 >> 3;

		SMC37c669_write_config( 
		    SMC37c669_SERIAL0_BASE_ADDRESS_INDEX,
		    base_addr.as_uchar
		);
		ret_val = TRUE;
		break;
	    }
	case SERIAL_1:
	    {
	    	SMC37c669_SERIAL_BASE_ADDRESS_REGISTER base_addr;
		SMC37c669_SERIAL_IRQ_REGISTER irq;
/*
** Enable the serial 2 IRQ mapping
*/
	    	irq.as_uchar = 
		    SMC37c669_read_config( SMC37c669_SERIAL_IRQ_INDEX );

		irq.by_field.uart2_irq =
		    SMC37c669_RAW_DEVICE_IRQ(
			SMC37c669_xlate_irq( local_config[ func ].irq )
		    );

		SMC37c669_write_config( SMC37c669_SERIAL_IRQ_INDEX, irq.as_uchar );
/*
** Enable the serial 2 port base address mapping
*/
		base_addr.as_uchar = 0;
		base_addr.by_field.addr9_3 = local_config[ func ].port1 >> 3;

		SMC37c669_write_config( 
		    SMC37c669_SERIAL1_BASE_ADDRESS_INDEX,
		    base_addr.as_uchar
		);
		ret_val = TRUE;
		break;
	    }
	case PARALLEL_0:
	    {
	    	SMC37c669_PARALLEL_BASE_ADDRESS_REGISTER base_addr;
		SMC37c669_PARALLEL_FDC_IRQ_REGISTER irq;
		SMC37c669_PARALLEL_FDC_DRQ_REGISTER drq;
/*
** Enable the parallel port DMA channel mapping
*/
	    	drq.as_uchar =
		    SMC37c669_read_config( SMC37c669_PARALLEL_FDC_DRQ_INDEX );

		drq.by_field.ppt_drq = 
		    SMC37c669_RAW_DEVICE_DRQ(
			SMC37c669_xlate_drq( local_config[ func ].drq )
		    );

		SMC37c669_write_config(
		    SMC37c669_PARALLEL_FDC_DRQ_INDEX,
		    drq.as_uchar
		);
/*
** Enable the parallel port IRQ mapping
*/
		irq.as_uchar = 
		    SMC37c669_read_config( SMC37c669_PARALLEL_FDC_IRQ_INDEX );

		irq.by_field.ppt_irq =
		    SMC37c669_RAW_DEVICE_IRQ(
			SMC37c669_xlate_irq( local_config[ func ].irq )
		    );

		SMC37c669_write_config( 
		    SMC37c669_PARALLEL_FDC_IRQ_INDEX,
		    irq.as_uchar
		);
/*
** Enable the parallel port base address mapping
*/
		base_addr.as_uchar = 0;
		base_addr.by_field.addr9_2 = local_config[ func ].port1 >> 2;

		SMC37c669_write_config(
		    SMC37c669_PARALLEL0_BASE_ADDRESS_INDEX,
		    base_addr.as_uchar
		);
		ret_val = TRUE;
		break;
	    }
	case FLOPPY_0:
	    {
	    	SMC37c669_FDC_BASE_ADDRESS_REGISTER base_addr;
		SMC37c669_PARALLEL_FDC_IRQ_REGISTER irq;
		SMC37c669_PARALLEL_FDC_DRQ_REGISTER drq;
/*
** Enable the floppy controller DMA channel mapping
*/
	    	drq.as_uchar =
		    SMC37c669_read_config( SMC37c669_PARALLEL_FDC_DRQ_INDEX );
		 
		drq.by_field.fdc_drq =
		    SMC37c669_RAW_DEVICE_DRQ(
			SMC37c669_xlate_drq( local_config[ func ].drq )
		    );
		 
		SMC37c669_write_config( 
		    SMC37c669_PARALLEL_FDC_DRQ_INDEX,
		    drq.as_uchar
		);
/*
** Enable the floppy controller IRQ mapping
*/
		irq.as_uchar =
		    SMC37c669_read_config( SMC37c669_PARALLEL_FDC_IRQ_INDEX );
		 
		irq.by_field.fdc_irq =
		    SMC37c669_RAW_DEVICE_IRQ(
			SMC37c669_xlate_irq( local_config[ func ].irq )
		    );
		 
		SMC37c669_write_config(
		    SMC37c669_PARALLEL_FDC_IRQ_INDEX,
		    irq.as_uchar
		);
/*
** Enable the floppy controller base address mapping
*/
		base_addr.as_uchar = 0;
		base_addr.by_field.addr9_4 = local_config[ func ].port1 >> 4;
		 
		SMC37c669_write_config(
		    SMC37c669_FDC_BASE_ADDRESS_INDEX,
		    base_addr.as_uchar
		);
		ret_val = TRUE;
		break;
	    }
	case IDE_0:
	    {
	    	SMC37c669_IDE_ADDRESS_REGISTER ide_addr;
/*
** Enable the IDE alternate status base address mapping
*/
	    	ide_addr.as_uchar = 0;
		ide_addr.by_field.addr9_4 = local_config[ func ].port2 >> 4;
		 
		SMC37c669_write_config(
		    SMC37c669_IDE_ALTERNATE_ADDRESS_INDEX,
		    ide_addr.as_uchar
		);
/*
** Enable the IDE controller base address mapping
*/
		ide_addr.as_uchar = 0;
		ide_addr.by_field.addr9_4 = local_config[ func ].port1 >> 4;
		 
		SMC37c669_write_config(
		    SMC37c669_IDE_BASE_ADDRESS_INDEX,
		    ide_addr.as_uchar
		);
		ret_val = TRUE;
		break;
	    }
    }
/*
** Exit configuration mode and return
*/
    SMC37c669_config_mode( FALSE );

    return ret_val;
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function disables a device function within the
**	SMC37c669 Super I/O controller.
**
**  FORMAL PARAMETERS:
**
**      func:
**          Which function to disable
**
**  RETURN VALUE:
**
**      Return TRUE if the device function was disabled, otherwise, FALSE
**
**  SIDE EFFECTS:
**
**      {@description or none@}
**
**  DESIGN:
**
**      Disabling a function in the SMC37c669 device involves
**	disabling all the function's mappings (port, irq, drq ...).
**	A shadow copy of the device configuration is maintained
**	in local storage so we won't worry aboving saving the
**	current configuration information.
**
**--
*/
unsigned int SMC37c669_disable_device ( unsigned int func )
{
    unsigned int ret_val;

/*
** Put the device into configuration mode
*/
    SMC37c669_config_mode( TRUE );
    switch ( func ) {
    	case SERIAL_0:
	    {
	    	SMC37c669_SERIAL_BASE_ADDRESS_REGISTER base_addr;
		SMC37c669_SERIAL_IRQ_REGISTER irq;
/*
** Disable the serial 1 IRQ mapping
*/
	    	irq.as_uchar = 
		    SMC37c669_read_config( SMC37c669_SERIAL_IRQ_INDEX );

		irq.by_field.uart1_irq = 0;

		SMC37c669_write_config( SMC37c669_SERIAL_IRQ_INDEX, irq.as_uchar );
/*
** Disable the serial 1 port base address mapping
*/
		base_addr.as_uchar = 0;
		SMC37c669_write_config( 
		    SMC37c669_SERIAL0_BASE_ADDRESS_INDEX,
		    base_addr.as_uchar
		);
		ret_val = TRUE;
		break;
	    }
	case SERIAL_1:
	    {
	    	SMC37c669_SERIAL_BASE_ADDRESS_REGISTER base_addr;
		SMC37c669_SERIAL_IRQ_REGISTER irq;
/*
** Disable the serial 2 IRQ mapping
*/
	    	irq.as_uchar = 
		    SMC37c669_read_config( SMC37c669_SERIAL_IRQ_INDEX );

		irq.by_field.uart2_irq = 0;

		SMC37c669_write_config( SMC37c669_SERIAL_IRQ_INDEX, irq.as_uchar );
/*
** Disable the serial 2 port base address mapping
*/
		base_addr.as_uchar = 0;

		SMC37c669_write_config( 
		    SMC37c669_SERIAL1_BASE_ADDRESS_INDEX,
		    base_addr.as_uchar
		);
		ret_val = TRUE;
		break;
	    }
	case PARALLEL_0:
	    {
	    	SMC37c669_PARALLEL_BASE_ADDRESS_REGISTER base_addr;
		SMC37c669_PARALLEL_FDC_IRQ_REGISTER irq;
		SMC37c669_PARALLEL_FDC_DRQ_REGISTER drq;
/*
** Disable the parallel port DMA channel mapping
*/
	    	drq.as_uchar =
		    SMC37c669_read_config( SMC37c669_PARALLEL_FDC_DRQ_INDEX );

		drq.by_field.ppt_drq = 0;

		SMC37c669_write_config(
		    SMC37c669_PARALLEL_FDC_DRQ_INDEX,
		    drq.as_uchar
		);
/*
** Disable the parallel port IRQ mapping
*/
		irq.as_uchar = 
		    SMC37c669_read_config( SMC37c669_PARALLEL_FDC_IRQ_INDEX );

		irq.by_field.ppt_irq = 0;

		SMC37c669_write_config( 
		    SMC37c669_PARALLEL_FDC_IRQ_INDEX,
		    irq.as_uchar
		);
/*
** Disable the parallel port base address mapping
*/
		base_addr.as_uchar = 0;

		SMC37c669_write_config(
		    SMC37c669_PARALLEL0_BASE_ADDRESS_INDEX,
		    base_addr.as_uchar
		);
		ret_val = TRUE;
		break;
	    }
	case FLOPPY_0:
	    {
	    	SMC37c669_FDC_BASE_ADDRESS_REGISTER base_addr;
		SMC37c669_PARALLEL_FDC_IRQ_REGISTER irq;
		SMC37c669_PARALLEL_FDC_DRQ_REGISTER drq;
/*
** Disable the floppy controller DMA channel mapping
*/
	    	drq.as_uchar =
		    SMC37c669_read_config( SMC37c669_PARALLEL_FDC_DRQ_INDEX );
		 
		drq.by_field.fdc_drq = 0;
		 
		SMC37c669_write_config( 
		    SMC37c669_PARALLEL_FDC_DRQ_INDEX,
		    drq.as_uchar
		);
/*
** Disable the floppy controller IRQ mapping
*/
		irq.as_uchar =
		    SMC37c669_read_config( SMC37c669_PARALLEL_FDC_IRQ_INDEX );
		 
		irq.by_field.fdc_irq = 0;
		 
		SMC37c669_write_config(
		    SMC37c669_PARALLEL_FDC_IRQ_INDEX,
		    irq.as_uchar
		);
/*
** Disable the floppy controller base address mapping
*/
		base_addr.as_uchar = 0;
		 
		SMC37c669_write_config(
		    SMC37c669_FDC_BASE_ADDRESS_INDEX,
		    base_addr.as_uchar
		);
		ret_val = TRUE;
		break;
	    }
	case IDE_0:
	    {
	    	SMC37c669_IDE_ADDRESS_REGISTER ide_addr;
/*
** Disable the IDE alternate status base address mapping
*/
	    	ide_addr.as_uchar = 0;
		 
		SMC37c669_write_config(
		    SMC37c669_IDE_ALTERNATE_ADDRESS_INDEX,
		    ide_addr.as_uchar
		);
/*
** Disable the IDE controller base address mapping
*/
		ide_addr.as_uchar = 0;
		 
		SMC37c669_write_config(
		    SMC37c669_IDE_BASE_ADDRESS_INDEX,
		    ide_addr.as_uchar
		);
		ret_val = TRUE;
		break;
	    }
    }
/*
** Exit configuration mode and return
*/
    SMC37c669_config_mode( FALSE );

    return ret_val;
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function configures a device function within the 
**	SMC37c669 Super I/O controller.
**
**  FORMAL PARAMETERS:
**
**      func:
**          Which device function
**       
**      port:
**          I/O port for the function to use
**	 
**      irq:
**          IRQ for the device function to use
**	 
**      drq:
**          DMA channel for the device function to use
**
**  RETURN VALUE:
**
**      Returns TRUE if the device function was configured, 
**	otherwise, FALSE.
**
**  SIDE EFFECTS:
**
**      {@description or none@}
**
**  DESIGN:
**
**	If this function returns TRUE, the local shadow copy of
**	the configuration is also updated.  If the device function
**	is currently disabled, only the local shadow copy is 
**	updated and the actual device function will be updated
**	if/when is is enabled.
**
**--
*/
unsigned int SMC37c669_configure_device (
    unsigned int func,
    int port,
    int irq,
    int drq )
{
    struct DEVICE_CONFIG *cp;

/*
** Check for a valid configuration
*/
    if ( ( cp = SMC37c669_get_config ( func ) ) != NULL ) {
/*
** Configuration is valid, update the local shadow copy
*/
    	if ( ( drq & ~0xFF ) == 0 ) {
	    cp->drq = drq;
	}
	if ( ( irq & ~0xFF ) == 0 ) {
	    cp->irq = irq;
	}
	if ( ( port & ~0xFFFF ) == 0 ) {
	    cp->port1 = port;
	}
/*
** If the device function is enabled, update the actual
** device configuration.
*/
	if ( SMC37c669_is_device_enabled( func ) ) {
	    SMC37c669_enable_device( func );
	}
	return TRUE;
    }
    return FALSE;
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function determines whether a device function
**	within the SMC37c669 controller is enabled.
**
**  FORMAL PARAMETERS:
**
**      func:
**          Which device function
**
**  RETURN VALUE:
**
**      Returns TRUE if the device function is enabled, otherwise, FALSE
**
**  SIDE EFFECTS:
**
**      {@description or none@}
**
**  DESIGN:
**
**      To check whether a device is enabled we will only look at 
**	the port base address mapping.  According to the SMC37c669
**	specification, all of the port base address mappings are
**	disabled if the addr<9:8> (bits <7:6> of the register) are
**	zero.
**
**--
*/
static unsigned int SMC37c669_is_device_enabled ( unsigned int func )
{
    unsigned char base_addr;
    unsigned int dev_ok = FALSE;
    unsigned int ret_val = FALSE;
/*
** Enter configuration mode
*/
    SMC37c669_config_mode( TRUE );
     
    switch ( func ) {
    	case SERIAL_0:
	    base_addr =
		SMC37c669_read_config( SMC37c669_SERIAL0_BASE_ADDRESS_INDEX );
	    dev_ok = TRUE;
	    break;
	case SERIAL_1:
	    base_addr =
		SMC37c669_read_config( SMC37c669_SERIAL1_BASE_ADDRESS_INDEX );
	    dev_ok = TRUE;
	    break;
	case PARALLEL_0:
	    base_addr =
		SMC37c669_read_config( SMC37c669_PARALLEL0_BASE_ADDRESS_INDEX );
	    dev_ok = TRUE;
	    break;
	case FLOPPY_0:
	    base_addr =
		SMC37c669_read_config( SMC37c669_FDC_BASE_ADDRESS_INDEX );
	    dev_ok = TRUE;
	    break;
	case IDE_0:
	    base_addr =
		SMC37c669_read_config( SMC37c669_IDE_BASE_ADDRESS_INDEX );
	    dev_ok = TRUE;
	    break;
    }
/*
** If we have a valid device, check base_addr<7:6> to see if the
** device is enabled (mapped).
*/
    if ( ( dev_ok ) && ( ( base_addr & 0xC0 ) != 0 ) ) {
/*
** The mapping is not disabled, so assume that the function is 
** enabled.
*/
    	ret_val = TRUE;
    }
/*
** Exit configuration mode 
*/
    SMC37c669_config_mode( FALSE );

    return ret_val;
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function retrieves the configuration information of a 
**	device function within the SMC37c699 Super I/O controller.
**
**  FORMAL PARAMETERS:
**
**      func:
**          Which device function
**       
**      port:
**          I/O port returned
**	 
**      irq:
**          IRQ returned
**	 
**      drq:
**          DMA channel returned
**
**  RETURN VALUE:
**
**      Returns TRUE if the device configuration was successfully
**	retrieved, otherwise, FALSE.
**
**  SIDE EFFECTS:
**
**      The data pointed to by the port, irq, and drq parameters
**	my be modified even if the configuration is not successfully
**	retrieved.
**
**  DESIGN:
**
**      The device configuration is fetched from the local shadow
**	copy.  Any unused parameters will be set to -1.  Any
**	parameter which is not desired can specify the NULL
**	pointer.
**
**--
*/
static unsigned int SMC37c669_get_device_config (
    unsigned int func,
    int *port,
    int *irq,
    int *drq )
{
    struct DEVICE_CONFIG *cp;
    unsigned int ret_val = FALSE;
/*
** Check for a valid device configuration
*/
    if ( ( cp = SMC37c669_get_config( func ) ) != NULL ) {
    	if ( drq != NULL ) {
	    *drq = cp->drq;
	    ret_val = TRUE;
	}
	if ( irq != NULL ) {
	    *irq = cp->irq;
	    ret_val = TRUE;
	}
	if ( port != NULL ) {
	    *port = cp->port1;
	    ret_val = TRUE;
	}
    }
    return ret_val;
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function displays the current state of the SMC37c699
**	Super I/O controller's device functions.
**
**  FORMAL PARAMETERS:
**
**      None
**
**  RETURN VALUE:
**
**      None
**
**  SIDE EFFECTS:
**
**      None
**
**--
*/
void SMC37c669_display_device_info ( void )
{
#if !(YUKONA)
    if ( SMC37c669_is_device_enabled( SERIAL_0 ) ) {
    	qprintf( "  Serial 0:    Enabled [ Port 0x%x, IRQ %d ]\n",
		 local_config[ SERIAL_0 ].port1,
		 local_config[ SERIAL_0 ].irq
	);
    }
    else {
    	qprintf( "  Serial 0:    Disabled\n" );
    }

    if ( SMC37c669_is_device_enabled( SERIAL_1 ) ) {
    	qprintf( "  Serial 1:    Enabled [ Port 0x%x, IRQ %d ]\n",
		 local_config[ SERIAL_1 ].port1,
		 local_config[ SERIAL_1 ].irq
	);
    }
    else {
    	qprintf( "  Serial 1:    Disabled\n" );
    }
#endif
    if ( SMC37c669_is_device_enabled( PARALLEL_0 ) ) {
    	qprintf( "  Parallel:    Enabled [ Port 0x%x, IRQ %d ]\n",
		 local_config[ PARALLEL_0 ].port1,
		 local_config[ PARALLEL_0 ].irq
	);
    }
    else {
    	qprintf( "  Parallel:    Disabled\n" );
    }

    if ( SMC37c669_is_device_enabled( FLOPPY_0 ) ) {
    	qprintf( "  Floppy Ctrl: Enabled [ Port 0x%x, IRQ %d ]\n",
		 local_config[ FLOPPY_0 ].port1,
		 local_config[ FLOPPY_0 ].irq
	);
    }
    else {
    	qprintf( "  Floppy Ctrl: Disabled\n" );
    }

    if ( SMC37c669_is_device_enabled( IDE_0 ) ) {
    	qprintf( "  IDE 0:       Enabled [ Port 0x%x, IRQ %d ]\n",
		 local_config[ IDE_0 ].port1,
		 local_config[ IDE_0 ].irq
	);
    }
#if !(YUKONA)
    else {
    	qprintf( "  IDE 0:       Disabled\n" );
    }
#endif
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function puts the SMC37c669 Super I/O controller into,
**	and takes it out of, configuration mode.
**
**  FORMAL PARAMETERS:
**
**      enable:
**          TRUE to enter configuration mode, FALSE to exit.
**
**  RETURN VALUE:
**
**      None
**
**  SIDE EFFECTS:
**
**      The SMC37c669 controller may be left in configuration mode.
**
**--
*/
void SMC37c669_config_mode( 
    unsigned int enable )
{
    if ( enable ) {
/*
** To enter configuration mode, two writes in succession to the index
** port are required.  If a write to another address or port occurs
** between these two writes, the chip does not enter configuration
** mode.  Therefore, a spinlock is placed around the two writes to 
** guarantee that they complete uninterrupted.
*/
	spinlock( &spl_atomic );
    	wb( &SMC37c669->index_port, SMC37c669_CONFIG_ON_KEY );
    	wb( &SMC37c669->index_port, SMC37c669_CONFIG_ON_KEY );
	spinunlock( &spl_atomic );
    }
    else {
    	wb( &SMC37c669->index_port, SMC37c669_CONFIG_OFF_KEY );
    }
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function reads an SMC37c669 Super I/O controller
**	configuration register.  This function assumes that the
**	device is already in configuration mode.
**
**  FORMAL PARAMETERS:
**
**      index:
**          Index value of configuration register to read
**
**  RETURN VALUE:
**
**      Data read from configuration register
**
**  SIDE EFFECTS:
**
**      None
**
**--
*/
static unsigned char SMC37c669_read_config( 
    unsigned char index )
{
    unsigned char data;

    wb( &SMC37c669->index_port, index );
    data = rb( &SMC37c669->data_port );
    return data;
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function writes an SMC37c669 Super I/O controller
**	configuration register.  This function assumes that the
**	device is already in configuration mode.
**
**  FORMAL PARAMETERS:
**
**      index:
**          Index of configuration register to write
**       
**      data:
**          Data to be written
**
**  RETURN VALUE:
**
**      None
**
**  SIDE EFFECTS:
**
**      None
**
**--
*/
void SMC37c669_write_config( 
    unsigned char index, 
    unsigned char data )
{
    wb( &SMC37c669->index_port, index );
    wb( &SMC37c669->data_port, data );
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function initializes the local device
**	configuration storage.  This function assumes
**	that the device is already in configuration
**	mode.
**
**  FORMAL PARAMETERS:
**
**      None
**
**  RETURN VALUE:
**
**      None
**
**  SIDE EFFECTS:
**
**      Local storage for device configuration information
**	is initialized.
**
**--
*/
static void SMC37c669_init_local_config ( void )
{
    SMC37c669_SERIAL_BASE_ADDRESS_REGISTER uart_base;
    SMC37c669_SERIAL_IRQ_REGISTER uart_irqs;
    SMC37c669_PARALLEL_BASE_ADDRESS_REGISTER ppt_base;
    SMC37c669_PARALLEL_FDC_IRQ_REGISTER ppt_fdc_irqs;
    SMC37c669_PARALLEL_FDC_DRQ_REGISTER ppt_fdc_drqs;
    SMC37c669_FDC_BASE_ADDRESS_REGISTER fdc_base;
    SMC37c669_IDE_ADDRESS_REGISTER ide_base;
    SMC37c669_IDE_ADDRESS_REGISTER ide_alt;

/*
** Get serial port 1 base address 
*/
    uart_base.as_uchar = 
	SMC37c669_read_config( SMC37c669_SERIAL0_BASE_ADDRESS_INDEX );
/*
** Get IRQs for serial ports 1 & 2
*/
    uart_irqs.as_uchar = 
	SMC37c669_read_config( SMC37c669_SERIAL_IRQ_INDEX );
/*
** Store local configuration information for serial port 1
*/
    local_config[SERIAL_0].port1 = uart_base.by_field.addr9_3 << 3;
    local_config[SERIAL_0].irq = 
	SMC37c669_xlate_irq( 
	    SMC37c669_DEVICE_IRQ( uart_irqs.by_field.uart1_irq ) 
	);
/*
** Get serial port 2 base address
*/
    uart_base.as_uchar = 
	SMC37c669_read_config( SMC37c669_SERIAL1_BASE_ADDRESS_INDEX );
/*
** Store local configuration information for serial port 2
*/
    local_config[SERIAL_1].port1 = uart_base.by_field.addr9_3 << 3;
    local_config[SERIAL_1].irq = 
	SMC37c669_xlate_irq( 
	    SMC37c669_DEVICE_IRQ( uart_irqs.by_field.uart2_irq ) 
	);
/*
** Get parallel port base address
*/
    ppt_base.as_uchar =
	SMC37c669_read_config( SMC37c669_PARALLEL0_BASE_ADDRESS_INDEX );
/*
** Get IRQs for parallel port and floppy controller
*/
    ppt_fdc_irqs.as_uchar =
	SMC37c669_read_config( SMC37c669_PARALLEL_FDC_IRQ_INDEX );
/*
** Get DRQs for parallel port and floppy controller
*/
    ppt_fdc_drqs.as_uchar =
	SMC37c669_read_config( SMC37c669_PARALLEL_FDC_DRQ_INDEX );
/*
** Store local configuration information for parallel port
*/
    local_config[PARALLEL_0].port1 = ppt_base.by_field.addr9_2 << 2;
    local_config[PARALLEL_0].irq =
	SMC37c669_xlate_irq(
	    SMC37c669_DEVICE_IRQ( ppt_fdc_irqs.by_field.ppt_irq )
	);
    local_config[PARALLEL_0].drq =
	SMC37c669_xlate_drq(
	    SMC37c669_DEVICE_DRQ( ppt_fdc_drqs.by_field.ppt_drq )
	);
/*
** Get floppy controller base address
*/
    fdc_base.as_uchar = 
	SMC37c669_read_config( SMC37c669_FDC_BASE_ADDRESS_INDEX );
/*
** Store local configuration information for floppy controller
*/
    local_config[FLOPPY_0].port1 = fdc_base.by_field.addr9_4 << 4;
    local_config[FLOPPY_0].irq =
	SMC37c669_xlate_irq(
	    SMC37c669_DEVICE_IRQ( ppt_fdc_irqs.by_field.fdc_irq )
	);
    local_config[FLOPPY_0].drq =
	SMC37c669_xlate_drq(
	    SMC37c669_DEVICE_DRQ( ppt_fdc_drqs.by_field.fdc_drq )
	);
/*
** Get IDE controller base address
*/
    ide_base.as_uchar =
	SMC37c669_read_config( SMC37c669_IDE_BASE_ADDRESS_INDEX );
/*
** Get IDE alternate status base address
*/
    ide_alt.as_uchar =
	SMC37c669_read_config( SMC37c669_IDE_ALTERNATE_ADDRESS_INDEX );
/*
** Store local configuration information for IDE controller
*/
    local_config[IDE_0].port1 = ide_base.by_field.addr9_4 << 4;
    local_config[IDE_0].port2 = ide_alt.by_field.addr9_4 << 4;
    local_config[IDE_0].irq = 14;
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function returns a pointer to the local shadow
**	configuration of the requested device function.
**
**  FORMAL PARAMETERS:
**
**      func:
**          Which device function
**
**  RETURN VALUE:
**
**      Returns a pointer to the DEVICE_CONFIG structure for the
**	requested function, otherwise, NULL.
**
**  SIDE EFFECTS:
**
**      {@description or none@}
**
**--
*/
static struct DEVICE_CONFIG *SMC37c669_get_config( unsigned int func )
{
    struct DEVICE_CONFIG *cp = NULL;

    switch ( func ) {
    	case SERIAL_0:
	    cp = &local_config[ SERIAL_0 ];
	    break;
	case SERIAL_1:
	    cp = &local_config[ SERIAL_1 ];
	    break;
	case PARALLEL_0:
	    cp = &local_config[ PARALLEL_0 ];
	    break;
	case FLOPPY_0:
	    cp = &local_config[ FLOPPY_0 ];
	    break;
	case IDE_0:
	    cp = &local_config[ IDE_0 ];
	    break;
    }
    return cp;
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function translates IRQs back and forth between ISA
**	IRQs and SMC37c669 device IRQs.
**
**  FORMAL PARAMETERS:
**
**      irq:
**          The IRQ to translate
**
**  RETURN VALUE:
**
**      Returns the translated IRQ, otherwise, returns -1.
**
**  SIDE EFFECTS:
**
**      {@description or none@}
**
**--
*/
static int SMC37c669_xlate_irq ( unsigned int irq )
{
    int i, translated_irq = -1;

    if ( SMC37c669_IS_DEVICE_IRQ( (int)irq ) ) {
/*
** We are translating a device IRQ to an ISA IRQ
*/
    	for ( i = 0; ( SMC37c669_irq_table[i].device_irq != -1 ) || ( SMC37c669_irq_table[i].isa_irq != -1 ); i++ ) {
	    if ( irq == SMC37c669_irq_table[i].device_irq ) {
	    	translated_irq = SMC37c669_irq_table[i].isa_irq;
		break;
	    }
	}
    }
    else {
/*
** We are translating an ISA IRQ to a device IRQ
*/
    	for ( i = 0; ( SMC37c669_irq_table[i].isa_irq != -1 ) || ( SMC37c669_irq_table[i].device_irq != -1 ); i++ ) {
	    if ( irq == SMC37c669_irq_table[i].isa_irq ) {
	    	translated_irq = SMC37c669_irq_table[i].device_irq;
		break;
	    }
	}
    }
    return translated_irq;
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function translates DMA channels back and forth between
**	ISA DMA channels and SMC37c669 device DMA channels.
**
**  FORMAL PARAMETERS:
**
**      drq:
**          The DMA channel to translate
**
**  RETURN VALUE:
**
**      Returns the translated DMA channel, otherwise, returns -1
**
**  SIDE EFFECTS:
**
**      {@description or none@}
**
**--
*/
static int SMC37c669_xlate_drq ( unsigned int drq )
{
    int i, translated_drq = -1;

    if ( SMC37c669_IS_DEVICE_DRQ( (int)drq ) ) {
/*
** We are translating a device DMA channel to an ISA DMA channel
*/
    	for ( i = 0; ( SMC37c669_drq_table[i].device_drq != -1 ) || ( SMC37c669_drq_table[i].isa_drq != -1 ); i++ ) {
	    if ( drq == SMC37c669_drq_table[i].device_drq ) {
	    	translated_drq = SMC37c669_drq_table[i].isa_drq;
		break;
	    }
	}
    }
    else {
/*
** We are translating an ISA DMA channel to a device DMA channel
*/
    	for ( i = 0; ( SMC37c669_drq_table[i].isa_drq != -1 ) || ( SMC37c669_drq_table[i].device_drq != -1 ); i++ ) {
	    if ( drq == SMC37c669_drq_table[i].isa_drq ) {
	    	translated_drq = SMC37c669_drq_table[i].device_drq;
		break;
	    }
	}
    }
    return translated_drq;
}

int smcc669_init ( void )
{
    struct INODE *ip;

    allocinode( smc_ddb.name, 1, &ip );
    ip->dva = &smc_ddb;
    ip->attr = ATTR$M_WRITE | ATTR$M_READ;
    ip->len[0] = 0x30;
    ip->misc = 0;
    INODE_UNLOCK( ip );

    return msg_success;
}

int smcc669_open( struct FILE *fp, char *info, char *next, char *mode )
{
    struct INODE *ip;
/*
** Allow multiple readers but only one writer.  ip->misc keeps track
** of the number of writers
*/
    ip = fp->ip;
    INODE_LOCK( ip );
    if ( fp->mode & ATTR$M_WRITE ) {
	if ( ip->misc ) {
	    INODE_UNLOCK( ip );
	    return msg_failure;	    /* too many writers */
	}
	ip->misc++;
    }
/*
** Treat the information field as a byte offset
*/
    *fp->offset = xtoi( info );
    INODE_UNLOCK( ip );

    return msg_success;
}

int smcc669_close( struct FILE *fp )
{
    struct INODE *ip;

    ip = fp->ip;
    if ( fp->mode & ATTR$M_WRITE ) {
	INODE_LOCK( ip );
	ip->misc--;
	INODE_UNLOCK( ip );
    }
    return msg_success;
}

int smcc669_read( struct FILE *fp, int size, int number, unsigned char *buf )
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

    SMC37c669_config_mode( TRUE );
    for ( i = 0; i < length; i++ ) {
	if ( !inrange( *fp->offset, 0, ip->len[0] ) ) 
	    break;
	*buf++ = SMC37c669_read_config( *fp->offset );
	*fp->offset += 1;
	nbytes++;
    }
    SMC37c669_config_mode( FALSE );
    return nbytes;
}

int smcc669_write( struct FILE *fp, int size, int number, unsigned char *buf )
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

    SMC37c669_config_mode( TRUE );
    for ( i = 0; i < length; i++ ) {
	if ( !inrange( *fp->offset, 0, ip->len[0] ) ) 
	    break;
	SMC37c669_write_config( *fp->offset, *buf );
	*fp->offset += 1;
	buf++;
	nbytes++;
    }
    SMC37c669_config_mode( FALSE );
    return nbytes;
}
