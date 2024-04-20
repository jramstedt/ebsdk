/* file:    api_debug.c
 *
 * Copyright (C) 1990 by
 * Digital Equipment Corporation,Maynard,Massachusetts.
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
 *      API Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *  AUTHORS:
 *
 *  Chris Gearing
 *
 *  CREATION DATE:
 *  
 *  Q4 2000
 *
 *
 *  MODIFICATION HISTORY:
  
 *
 *--
 */

/*Include files*/

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$src:common_def.h"
#include "cp$src:parse_def.h"
#include "cp$src:kernel_def.h"
#include "cp$src:ev_def.h"
#include "cp$src:pb_def.h"
#include "cp$src:api_debug.h"

#define DO_MISS_DRIVER		0
#define SIMPLE_COM_DEBUG	0
#define EV_API_DEBUG		0x13
#define API_DRIVER_MISS		0x14

static unsigned char debugValue = 0xff;
extern unsigned int max_hose;
extern struct set_param_table ev_adaptec_table[];
extern char *TYPE_INIT;
extern struct DST {
    int	  (*startup)();
    char  *name;
    int   phase;
} dst[];

ev_api_debug(
char *name,
struct EVNODE *ev)
{
	static char enabled[] = "Enabled";
	static char disabled[] = "Disabled";

	set_api_debug_value(xtoi(ev->value.string),APIDB_SPCBITS);
	sprintf(ev->value.string,"%x",debugValue & ~APIDB_SPCBITS);

	printf("Simple COM1 Debug %s\n",(debugValue&APIDB_SIMCOMDBG)?enabled:disabled);

#if SHARK
	printf("PIO Setup %s\n",(debugValue&APIDB_IDEPIO)?enabled:disabled);
	printf("UART Fix %s\n",(debugValue&APIDB_UARTFIX)?enabled:disabled);
#endif

#if TINOSA
	printf("Scan All PCI %s\n",(debugValue&APIDB_ALLPCI)?enabled:disabled);
#endif

#if SWORDFISH
	printf("Onboard Adaptec %s\n",(debugValue&APIDB_ADPDIS)?disabled:enabled);
#endif

	return(msg_success);
}

ev_api_debug_init(
struct env_table *et,
char *value)
{
	static char data[3];

	sprintf(data,"%x",api_debug_value(0xff) & ~APIDB_SPCBITS);

	return(data);
}

unsigned char set_api_debug_value(unsigned char orValue,unsigned char andValue)
{
	debugValue = (api_debug_value(0xff) & andValue) | orValue;

	/* Ensure magic code bits set OK */
	debugValue = (debugValue & ~APIDB_SPCBITS) | APIDB_SPCVAL;

	rtc_write(EV_API_DEBUG, debugValue);
}

unsigned char api_debug_value(unsigned char value)
{
	if (debugValue == 0xff)
		debugValue = rtc_read(EV_API_DEBUG);

	/* Force to default value it not a valid value */
	if ((debugValue & APIDB_SPCBITS) != APIDB_SPCVAL)
		debugValue = APIDB_SPCVAL | APIDB_DEFBITS;

	return(debugValue & value);
}

int SimpleComDebug(void)
{
#if SIMPLE_COM_DEBUG
	return(TRUE);
#else
	return(api_debug_value(APIDB_SIMCOMDBG) != 0);
#endif
}

#if SWORDFISH
int ev_adaptec(
char *name,
struct EVNODE *ev)
{
	int i;

	for (i=0; ev_adaptec_table[i].param != 0; i++)
	{
		if (!strcmp(ev->value.string, ev_adaptec_table[i].param))
		{
			if (api_debug_value(APIDB_ADPDIS) != ev_adaptec_table[i].code)
				err_printf("Restart system for changes to take effect\n") ;

			set_api_debug_value(ev_adaptec_table[i].code,~APIDB_ADPDIS);
			return(msg_success);
		}
	}

	return(msg_failure);	
}

ev_adaptec_init(
struct env_table *et,
char *value)
{
	int i;

	for (i=0; ev_adaptec_table[i].param != 0; i++)
	{
		if (api_debug_value(APIDB_ADPDIS) == ev_adaptec_table[i].code)
		{
			return(ev_adaptec_table[i].param);
		}
	}

	return (ev_adaptec_table[0].param);
}
#endif

static unsigned int GetMissDriver(void)
{
	static unsigned int missDriver = (unsigned int)-1;

	if (missDriver == -1)
		missDriver = (rtc_read(API_DRIVER_MISS) & 0x00ff);

	return(missDriver);
}

unsigned int BadDriver(unsigned int driverNum)
{
#if DO_MISS_DRIVER
	return(driverNum == GetMissDriver());
#else
	return(FALSE);
#endif
}

void SetMissDriver(unsigned int missDriver)
{
#if DO_MISS_DRIVER
	rtc_write(API_DRIVER_MISS,missDriver);
#endif
}

void ClearMissDriver(int missDriver)
{
	SetMissDriver(GetMissDriver());
}

/*+
 * ============================================================================
 * = clear_bad_driver - Clears the bad driver.
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *   COMMAND TAG: clear_bad_driver 0 RXBZ clear_bad_driver
 *
 *   COMMAND ARGUMENT(S):
 *
 * 	<show>... - Specifies to show the currently excluded drivers(s).
 *
 *   COMMAND OPTION(S):
 *
 * 	None
 *
 *   COMMAND EXAMPLE(S):
 *
 *~
 *      P00>>>clear_bad_driver -show 
 *      Driver ev not loaded due to previous error
 *      type clear_bad_driver to cause reload on next boot
 *		P00>>>clear_bad_driver      
 *      Bad drivers cleared
 *      P00>>>clear_bad_driver -show 
 *      All drivers loaded
 *      P00>>>
 *~
 *
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

#define QSTRING	"show"
#define	SHOW		0		/* New Value Present */
#define	QMAX		1

void clear_bad_driver (int argc, char *argv[])
{
#if DO_MISS_DRIVER
	struct QSTRUCT qual[QMAX];
	struct PCB *pcb;
	int status;

	status = qscan (&argc, argv, "-", QSTRING, qual);

	if (status != msg_success)
	{
	    err_printf(status);
		return;
	}

	pcb = getpcb();

	if (qual[SHOW].present)
	{
		if (GetMissDriver() != 0xff)
		{
			fprintf(pcb->pcb$a_stdout,"Driver %s not loaded due to previous error\n",dst[GetMissDriver()].name);
			fprintf(pcb->pcb$a_stdout,"type clear_bad_driver to cause reload on next boot\n");
		}
		else
		{
			fprintf(pcb->pcb$a_stdout,"All drivers loaded\n");
		}
	}
	else
	{
		SetMissDriver(0xff);

		fprintf(pcb->pcb$a_stdout,"Bad drivers cleared\n");
	}
#endif
}


/*+
 * ============================================================================
 * = pci_registers - Display/Edits PCI Registers.                             =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *   COMMAND TAG: pci_registers 0 RXBZ pci_registers
 *
 *   COMMAND ARGUMENT(S):
 *
 * 	<hose...> - Specfies the host adaptor to show
 * 	<bus...> - Specfies the bus number to show.
 * 	<device...> - Specfies the device number to show.
 * 	<function...> - Specfies the function number to show.
 * 	<register...> - Specfies the register number to show.
 * 	<value...> - Specfies the new value to write to value.
 *
 *   COMMAND OPTION(S):
 *
 * 	None
 *
 *   COMMAND EXAMPLE(S):
 *~
 *      >>>pci_registers
 *
 *      >>>pci_register -hose 0
 *		....{ALL DEVICES INFO PRINTED}
 *
 *      >>>pci_register -hose 0 -device 7
 *		....{DEVICES INFO FOR BUS HOSE ZERO DEVICE 7 PRINTED}
 *
 *      >>>pci_register -hose 0 -device 7 -function 0 -reg 0
 *		00,00,00,00,10228086
 *
 *      >>>pci_register -hose 0 -device 7 -function 0 -reg 0 -value cceeccee
 *		00,00,00,00,10228086 now cceeccee
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

#define HOSE		0		/* Hose Present */
#define BUS			1		/* Bus Present */
#define DEVICE		2		/* Device Present */
#define	FUNCTION	3		/* Function Present	*/
#define	REGISTER	4		/* Register Present	*/
#define	NEWVAL		5		/* New Value Present */
#define	QMAX		6
#define QSTRING	"%xhose %xbus %xdevice %xfunction %xregister %xvalue"

void pci_registers (int argc, char *argv[])
{
    struct pb pb;
	int reg, checkDevice;
	int firstHose = 0, lastHose = max_hose;
	int firstBus = 0, lastBus = 0;
	int firstDevice = 0, lastDevice = 31;
	int firstFunction = 0, lastFunction = 7;
	int firstReg = 0, lastReg = 252;
	int status;
	struct QSTRUCT qual[QMAX];
	struct PCB *pcb;

	/*
	 * Pick off qualifiers
	 */
	status = qscan (&argc, argv, "-", QSTRING, qual);

	if (status != msg_success)
	{
	    err_printf(status);
		return;
	}

	pcb = getpcb();
	

	if (qual[HOSE].present)
	{
	    firstHose = qual[HOSE].value.integer;
	    lastHose = qual[HOSE].value.integer;
	}

	if (qual[BUS].present)
	{
	    firstBus = qual[BUS].value.integer;
	    lastBus = qual[BUS].value.integer;
	}

	if (qual[DEVICE].present)
	{
	    firstDevice = qual[DEVICE].value.integer;
	    lastDevice = qual[DEVICE].value.integer;
	}

	if (qual[FUNCTION].present)
	{
	    firstFunction = qual[FUNCTION].value.integer;
	    lastFunction = qual[FUNCTION].value.integer;
	}

	if (qual[REGISTER].present)
	{
	    firstReg = qual[REGISTER].value.integer & 0xfc;
	    lastReg = qual[REGISTER].value.integer & 0xfc;
	}

	for (pb.hose = firstHose; pb.hose <= lastHose; pb.hose++)
	{
		for (pb.bus = firstBus; pb.bus <= lastBus; pb.bus++)
		{
			for (pb.slot = firstDevice; pb.slot <= lastDevice; pb.slot++)
			{
				for (pb.function = firstFunction; pb.function <= lastFunction; pb.function++)
				{
					if (incfgl(&pb, 0) != 0xffffffff)
					{
						for (reg = firstReg; reg <= lastReg; reg+=4)
						{
							fprintf(pcb->pcb$a_stdout,"%02x,%02x,%02x,%02x,%08x",pb.hose,pb.slot,pb.function,reg,incfgl(&pb,reg));

							if (qual[NEWVAL].present)
							{
								outcfgl(&pb, reg, qual[NEWVAL].value);
								fprintf(pcb->pcb$a_stdout," now %08x\n",incfgl(&pb,reg));
							}

							fprintf(pcb->pcb$a_stdout,"\n");
						}
					}
				}
			}
		}
	}
}
