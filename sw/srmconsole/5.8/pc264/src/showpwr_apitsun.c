/* file:    ShowPwr_ApiTsun.c
 *
 * Copyright (C) 1990 by
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
 *      Shark Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *		Implements the show power command.
 *
 *  AUTHORS:
 *
 *  Chris Gearing
 *
 *  CREATION DATE:
 *  
 *  7-Sep-2000
 *
 *
 *  MODIFICATION HISTORY:
 * 
 *
 *--
 */

/*Include files*/

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$src:prdef.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:ansi_def.h"
#include "cp$src:stddef.h"
#include "cp$src:srom_def.h"
#include "cp$src:msg_def.h"
#include "cp$inc:platform_io.h"
#include "cp$inc:kernel_entry.h"
#include "cp$inc:prototypes.h"
#include "cp$src:ev_def.h"
#include "cp$src:parse_def.h"

#include "cp$src:iic_devices.h"
#include "cp$src:Nt_Types.h"

#define ADM_FAN1			0x00000001
#define ADM_FAN2			0x00000002
#define ADM_25V				0x00000004
#define ADM_VCCP1			0x00000008
#define ADM_33V				0x00000010
#define ADM_5V				0x00000020
#define ADM_12V				0x00000040
#define ADM_VCCP2			0x00000080
#define ADM_THERMAL			0x00000100
#define ADM_INTRUSTION		0x00000200
#define ADM_REV				0x00000400
#define ADM_STATUS1			0x00000800
#define ADM_STATUS2			0x00001000
#define LM75_TEMPERATURE	0x00002000
#define LM75_CONFIGREG		0x00004000
#define LM75_TEMPHYST		0x00008000
#define LM75_TEMPOS			0x00010000

typedef struct _LINEINFO {
	char *text;
	int item;
} LINEINFO;


extern struct set_param_table ac_action_table[];

/*+
 * ============================================================================
 * = show power - Display power supply and fan status.                        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine will display the power supply and fan status.
 *
 * COMMAND FORM:
 *
 *	show power ( )
 *                    
 * COMMAND ARGUMENT(S):
 *          
 *	None
 *
 * COMMAND OPTION(S):
 *
 *	None                                 
 *
 * COMMAND EXAMPLE(S):
 *
 *~
 *	P00>>>show power
 *
 *                          Status
 *	Power Supply 0       good
 *	Power Supply 1	     good
 *	System Fans          good
 *	CPU Fans             good
 *	Temperature          good
 *~
 *
 * FORM OF CALL:
 *
 *	show_power( argc, *argv[] )
 *
 * RETURNS:
 *         
 *	None.
 *
 * ARGUMENTS:
 *
 *	int argc - number of command line arguments passed by the shell
 *	char *argv [] - array of pointers to arguments
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/
void show_power( int argc, char *argv[] )
{
	static LINEINFO lines[] = {{"Fan1 Status",	ADM_FAN1},
							   {"Fan2 Status",	ADM_FAN2},
							   {"2.5V",			ADM_25V},
							   {"Vccp1",		ADM_VCCP1},
							   {"P3.3V",		ADM_33V},
							   {"P5V",			ADM_5V},
							   {"P12V",			ADM_12V},
							   {"Vccp2",		ADM_VCCP2},
							   {"Temperature",	ADM_THERMAL},
							   {"Status 1",		ADM_STATUS1},
							   {"Status 2",		ADM_STATUS2},
							   {"Temperature",	LM75_TEMPERATURE},
							   {"LM75 Config",	LM75_CONFIGREG},
							   {"Temp Hyst",	LM75_TEMPHYST},
							   {"Temp Pos",		LM75_TEMPOS}};

#if SHARK
	static char *names[] = {"Proc 1","Proc 2","Dimm","Riser","PSU"};
	static char *adm_devices[ARRAYSIZE(names)] = {"adm9240_primary","adm9240_secondary","adm9240_dimm","adm9240_riser","adm9240_psu"};
	static char *lm_devices[ARRAYSIZE(names)] = {NULL,NULL,NULL,NULL,NULL};
	static int dispInfo[ARRAYSIZE(names)] = {
		ADM_STATUS1|ADM_STATUS2|ADM_THERMAL|ADM_5V|ADM_12V|ADM_25V|ADM_33V|ADM_VCCP1|ADM_VCCP2|ADM_FAN1|ADM_FAN2|ADM_INTRUSTION|ADM_REV,/* Primary CPU */
		ADM_STATUS1|ADM_STATUS2|ADM_THERMAL|ADM_5V|ADM_12V|ADM_25V|ADM_33V|ADM_VCCP1|ADM_VCCP2|ADM_FAN1|ADM_FAN2|ADM_REV,				/* Seconday CPU */
		ADM_STATUS1|ADM_STATUS2|ADM_THERMAL|ADM_5V|ADM_12V|ADM_25V|ADM_33V|ADM_VCCP1|ADM_VCCP2|ADM_FAN1|ADM_FAN2|ADM_REV,				/* DIMM Module */
		ADM_STATUS1|ADM_STATUS2|ADM_THERMAL|ADM_5V|ADM_12V|ADM_33V|ADM_FAN1|ADM_FAN2|ADM_REV,											/* Primary CPU */
		ADM_STATUS1|ADM_STATUS2|ADM_THERMAL|ADM_5V|ADM_12V|ADM_FAN1|ADM_FAN2|ADM_REV};													/* PSU Module */
	int lastDevice = ARRAYSIZE(names);
#endif
#if SWORDFISH
	static char *names[] = {"Proc 1","Proc 2","Third Fan","CS20 PSU"};
	static char *adm_devices[ARRAYSIZE(names)] = {"adm9240_primary","adm9240_secondary","adm9240_secondary","cs20_psu"};
	static char *lm_devices[ARRAYSIZE(names)] = {"lm75_primary","lm75_secondary",NULL,NULL};
	static int dispInfo[ARRAYSIZE(names)] = {
		ADM_5V|ADM_12V|ADM_25V|ADM_33V|ADM_VCCP1|ADM_VCCP2|ADM_REV|LM75_TEMPERATURE|LM75_TEMPHYST|LM75_TEMPOS,	/* Primary CPU */
		ADM_5V|ADM_12V|ADM_25V|ADM_33V|ADM_VCCP1|ADM_VCCP2|ADM_REV|LM75_TEMPERATURE|LM75_TEMPHYST|LM75_TEMPOS,	/* Seconday CPU */
		ADM_FAN1|ADM_FAN2,																						/* Secondary ADM when CS20 PSU read */
		ADM_STATUS1|ADM_STATUS2|ADM_THERMAL|ADM_5V|ADM_12V|ADM_FAN1|ADM_FAN2|ADM_REV};							/* CS20 PSU Module */
	int lastDevice = (ARRAYSIZE(names)-2);				/* -1 because adm9240_cs20psu is not normally shown */

#define QSTRING	"cs20psu"
#define	CS20PSU		0		/* New Value Present */
#define	QMAX		1

#endif
	DIMMSPD dimmSpd;
	struct FILE *fp;
	char dimmName[10];
	ADM9240 adm9240[ARRAYSIZE(names)];
	LM75 lm75[ARRAYSIZE(names)];
	int error[ARRAYSIZE(names)];
	PCA8550 pca8550;
	int device, line, value, dimm;
	unsigned short nRAS, nCAS, nBS, wid, nChip, sum;
	unsigned long meg;
	int firstDevice = 0;

#if SWORDFISH
	int status;
	struct QSTRUCT qual[QMAX];

	status = qscan (&argc, argv, "-", QSTRING, qual);

	if (status != msg_success)
	{
	    err_printf(status);
		return;
	}

	if (qual[CS20PSU].present)
	{
		firstDevice = 2;
		lastDevice = ARRAYSIZE(names);
	}
#endif

	if (!Swordfish())
	{
		printf("Processor Module Information\n\n");
		printf("%12s","");
		
		for (device = firstDevice; device < lastDevice; device++)
		{
			printf("%9s",names[device]);
			error[device] = FALSE;

			if (adm_devices[device])
			{
				if (
					((fp = fopen(adm_devices[device],"r")) == NULL) ||
					(fread(&adm9240[device],sizeof(ADM9240),1,fp) != sizeof(ADM9240))
				   )
				{
					error[device] = TRUE;
				}

				if (fp)
					fclose(fp);
			}

			if (lm_devices[device])
			{
				if (
					((fp = fopen(lm_devices[device],"r")) == NULL) ||
					(fread(&lm75[device],sizeof(LM75),1,fp) != sizeof(LM75))
				   )
				{
					error[device] = TRUE;
				}

				if (fp)
					fclose(fp);
			}
		}

		printf("\n");

		for (line = 0; line < ARRAYSIZE(lines); line++)
		{
			/* See if this item is read, if not don't print.*/
			for (device = firstDevice; device < lastDevice; device++)
			{
				if (dispInfo[device] & lines[line].item)
					break;
			}

			if (device < lastDevice)
			{
				printf("%12s",lines[line].text);

				for (device = firstDevice; device < lastDevice; device++)
				{
					if (dispInfo[device] & lines[line].item)
					{
						if (error[device])
						{
							printf("%9s","???");
						}
						else
						{
							value = -1;

							switch(lines[line].item)
							{
								case ADM_FAN1:
									printf("%9s",((adm9240[device].fan1 < 230) && (adm9240[device].fan1 > 10))?"OK":"FAIL");
									break;
								case ADM_FAN2:
									printf("%9s",((adm9240[device].fan2 < 230) && (adm9240[device].fan2 > 10))?"OK":"FAIL");
									break;
								case ADM_25V:
									value = (adm9240[device].measured25*100)/(1920/25);
									break;
								case ADM_VCCP1:
									value = (adm9240[device].measuredVCCP1*100)/(1920/50);
									break;
								case ADM_33V:
									value = (adm9240[device].measuredP33*100)/(1920/33);
									break;
								case ADM_5V:
									value = (adm9240[device].measuredP5*100)/(1920/50);
									break;
								case ADM_12V:
									value = (adm9240[device].measuredP12*100)/(1920/120);
									break;
								case ADM_VCCP2:
									value = (adm9240[device].measuredVCCP2*100)/(1920/50);
									break;
								case ADM_THERMAL:
									value = adm9240[device].temperature*2;
									break;
								case ADM_STATUS1:
									printf("     0x%02x",adm9240[device].intStatus1);
									break;
								case ADM_STATUS2:
									printf("     0x%02x",adm9240[device].intStatus2);
									break;
								case ADM_INTRUSTION:
									printf("     0x%02x",adm9240[device].vid4);
									break;
								case ADM_REV:
									break;
								case LM75_TEMPERATURE:
									value = lm75[device].temp*100;
									break;
								case LM75_CONFIGREG:
									printf("     0x%02x",lm75[device].config);
									break;
								case LM75_TEMPHYST:
									value = lm75[device].hyst*100;
									break;
								case LM75_TEMPOS:
									value = lm75[device].os*100;
									break;
							}

							if (value != -1)
								printf("%6d.%02d",value / 100, value % 100);
						}
					}
					else
					{
						printf("%9s","");
					}
				}

				printf("\n");
			}
		}

		printf("\n");

#if !SHARK
		if (!qual[CS20PSU].present)
#endif
		{
			printf("Dimm         1    2    3    4    5    6    7    8\n");
			printf("Megabytes");

			for (dimm = 0; dimm < 8; dimm++)
			{
				sprintf(dimmName,"dimm%d_spd",dimm);

				if (
					((fp = fopen(dimmName,"r")) == NULL) ||
					(fread(&dimmSpd,1,sizeof(dimmSpd), fp) != sizeof(dimmSpd))
				   )
				{
					printf("%5s","X");
				}
				else
				{
					/* Presume everything is ok to start with */
					nRAS  = dimmSpd.rowsOfAddresses;
					nCAS  = dimmSpd.columnsOfAddresses;
					nChip = dimmSpd.moduleBanks;
					wid   = dimmSpd.dataWidth1;

					for (nBS = 0; (1<<nBS) < dimmSpd.banksPerDram; ++nBS);
						nCAS += nBS;
					
					meg = (1l << (nRAS+nCAS));
					meg *= nChip;
					meg *= (wid >> 3) & ~3;
					meg >>= 20;

					if (meg)
						printf("%5d",meg);
					else
						printf("%5s","??");
				}

				if (fp)
					fclose(fp);
			}

			printf("\n\n");
		}

#if SHARK
		if (
			((fp = fopen("pca8550","r")) == NULL) ||
			(fread(&pca8550,1,sizeof(pca8550), fp) != sizeof(pca8550))
		   )
		{
			printf("PCA8550 read failure\n",dimm);
		}
		else
		{
			printf("System fail LED %s\n",pca8550.systemFailLED?"Extinguished":"Illuminated");
			printf("System %s on AC power\n",pca8550.nvRamWakeUp?"enters standby":"boots");
			printf("Normal SROM %s\n\n",pca8550.normalSROM?"selected":"deselected");
		}

		if (fp)
			fclose(fp);
#endif
	}
	else
	{
		printf("Unavailable on UP2000\n");
	}
}

#ifdef SHARK
int ev_ac_action(
char *name,
struct EVNODE *ev)
{
	struct FILE *fp;
	PCA8550 pca8550;
	int i;

	for (i=0; ac_action_table[i].param != 0; i++)
	{
		if (!strcmp(ev->value.string, ac_action_table[i].param))
		{
			/*if (
				((fp = fopen("pca8550","r+")) != NULL) &&
				(fread(&pca8550,1,sizeof(pca8550), fp) == sizeof(pca8550))
			   )
			{
				pca8550.nvRamWakeUp = ac_action_table[i].code;

				krn$_sleep(100);

				fseek(fp,0,SEEK_SET);
				if (fwrite(&pca8550,1,sizeof(pca8550), fp) != sizeof(pca8550))
					printf("Error writing I2C device\n");

			}
			else
			{
				printf("Error accessing I2C device\n");
			}

			if (fp)
				fclose(fp);*/

			return(ev_sev(name,ev));
		}
	}

	return(msg_failure);	
}
#endif
