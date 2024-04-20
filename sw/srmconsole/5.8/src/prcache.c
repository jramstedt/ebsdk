/* file:	prcache.c
 *
 * Copyright (C) 1993, 1997 by
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
 *      Cobra Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *	Commands to support the NVRAM disk cache on Cobra.
 *
 *  AUTHORS:
 *
 *      Harold Buckingham
 *
 *  CREATION DATE:
 *  
 *      04-Mar-1993
 *
 *  MODIFICATION HISTORY:
 *
 *      hcb     22-Jul-1994	Check for multiple NVRAM cards and prompt
 *				for bus type. Always display status(-f)
 *
 *      hcb     25-May-1994	Initial entry based on original prcache
 *				for Cobra.
 *
 *--
 */
                            

#include "cp$src:platform.h"
#include "cp$src:common.h"			/*Kernel definitions*/
#if 0
#include "cp$src:kernel_def.h"			/*Kernel definitions*/
#endif
#include "cp$src:ev_def.h"
#include "cp$src:parse_def.h"			/*Parser definitions*/
#include "cp$src:msg_def.h"			/*Message definitions*/
#include "cp$inc:kernel_entry.h"
#include "cp$inc:prototypes.h"

#define	QZ		0
#define	QF		1
#define QU		2
#define NUM_QUALS	3

#define DEBUG 0

int  prcache (int argc, char *argv[]);

#if !(TURBO || RAWHIDE)
extern int eisa_nvram_find();
extern int eisa_nvram_test_status();
extern int eisa_nvram_configured();
extern int eisa_nvram_zero();
extern int eisa_nvram_bat_enable();
extern int eisa_nvram_disp_status();
#endif

#if !(TURBO || RAWHIDE)
extern int cbus_nvram_find();
extern int cbus_nvram_test_status();
extern int cbus_nvram_configured();
extern int cbus_nvram_zero();
extern int cbus_nvram_bat_enable();
extern int cbus_nvram_disp_status();
#endif

extern int pci_nvram_find();
extern int pci_nvram_test_status();
extern int pci_nvram_configured();
extern int pci_nvram_zero();
extern int pci_nvram_bat_enable();
extern int pci_nvram_disp_status();


extern int err_printf();
extern int printf();
extern int qscan(int *argc, char *argv [], const char *qintro,
		 const char *flist, struct QSTRUCT *qvb);
extern int read_with_prompt(char *prompt, int maxbuf, char *buf, 
			    struct FILE *fp_in, struct FILE *fp_out, int echo);


#if (TURBO || RAWHIDE)
int eisa_nvram_find(){return msg_failure;};
void eisa_nvram_test_status(){};
void eisa_nvram_configured(){};
void eisa_nvram_zero(){};
void eisa_nvram_bat_enable(){};
void eisa_nvram_disp_status(){};

int cbus_nvram_find(){return msg_failure;};
void cbus_nvram_test_status(){};
void cbus_nvram_configured(){};
void cbus_nvram_zero(){};
void cbus_nvram_bat_enable(){};
void cbus_nvram_disp_status(){};
#endif

/*+
 * ============================================================================
 * = prcache - Manipulate an optional NVRAM disk cache.                       =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Manipulate an optional NVRAM disk cache installed on the CBUS, EISA
 *	and PCI buses. In the absence of options, indicates if an NVRAM 
 *	disk cache is present.
 *  
 *   COMMAND FMT: prcache 0 ZB 0 prcache
 *
 *   COMMAND FORM:
 *  
 *      prcache ( [-z] [-f] [-u] [nvram_bus])
 *  
 *   COMMAND TAG: prcache 0 RXBZ prcache
 *
 *   COMMAND ARGUMENT(S):
 *
 *	None
 *
 *   COMMAND OPTION(S):
 *
 *	-z	- If specified, zero the NVRAM disk cache.
 *
 *	-f	- If specified, display the NVRAM status.
 *
 *	-u	- If specified, disable the NVRAM battery disconnect circuit.
 *
 *	nvram_bus - If specified, perform the action on the NVRAM card
 *		    installed on the bus.  Default: sniff for one NVRAM module
 *		    Error if more than one installed and none specified.
 *
 *   COMMAND EXAMPLE(S):
 *~
 *	>>>prcache -f
 *	PCI NVRAM Disk Cache: passed
 *	Size:   2MB
 *	Base Address: 04000000
 *	System ID: 21000035
 *	State: valid
 *	Battery Status: good
 *	Battery Disconnect Circuit Status: disabled
 *	>>>
 *	>>>prcache -z pci
 *	This command will zero the PCI NVRAM disk cache
 *	Do you really want to continue [Y/N] ? : y
 *	clearing disk cache
 *	>>>prcache -u
 *	>>>prcache
 *	No Disk Cache installed
 *	>>>
 *~
 *   COMMAND REFERENCES:
 *
 *	None
 *
 * FORM OF CALL:
 *
 *	prcache (argc,argv);
 *
 * RETURNS:
 *
 *	int Status - msg_success or msg_failure
 *
 * ARGUMENTS:
 *
 *	int argc - Number of arguments specified.
 *	char *argv[] - List of arguments.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int  prcache (int argc, char *argv[])
{
    struct QSTRUCT qual[NUM_QUALS];
    int status;
    int cbus_presto_installed;
    int eisa_presto_installed;
    int pci_presto_installed;
    int total_installed;

    struct NVR {
    char presto_bus[MAX_NAME];
    int (*presto_test_status)();
    int (*presto_configured)();
    int (*presto_zero)();
    int (*presto_bat_enable)();
    int (*presto_disp_status)();
    int (*presto_installed)();
    } nvr;
    
    char answer[3];


    /* Look at our qualifiers */
    status = qscan (&argc, argv, "-","z f u",qual);
    if (status != msg_success) {
	err_printf (status);
	return(msg_failure);
    }

    cbus_presto_installed = FALSE;
    eisa_presto_installed = FALSE;
    pci_presto_installed  = FALSE;

    /* if the user specified a bus then use it. Otherwise try to find one */
    if (argc < 2) {
	if (cbus_nvram_find() == msg_success)
	    cbus_presto_installed = TRUE;

	if (eisa_nvram_find() == msg_success)
	    eisa_presto_installed = TRUE;

	if (pci_nvram_find() == msg_success)
	    pci_presto_installed  = TRUE;
    }
    else {
	strncpy(nvr.presto_bus,argv[1],MAX_NAME);
	if (!strcmp_nocase(nvr.presto_bus,"cbus"))
	    cbus_presto_installed = TRUE;
	if (!strcmp_nocase(nvr.presto_bus,"eisa"))
	    eisa_presto_installed = TRUE;
	if (!strcmp_nocase(nvr.presto_bus,"pci"))
	    pci_presto_installed = TRUE;
    }

#if DEBUG
    printf("cbus_presto_installed = %x\n",cbus_presto_installed);
    printf("eisa_presto_installed = %x\n",eisa_presto_installed);
    printf("pci_presto_installed  = %x\n",pci_presto_installed);
#endif

    total_installed = 0;
    if (cbus_presto_installed) {
	    total_installed++;
            strcpy(nvr.presto_bus,"CBUS");
	    nvr.presto_installed   = cbus_nvram_find;
	    nvr.presto_test_status = cbus_nvram_test_status;
	    nvr.presto_configured  = cbus_nvram_configured;
	    nvr.presto_zero        = cbus_nvram_zero;
	    nvr.presto_bat_enable  = cbus_nvram_bat_enable;
	    nvr.presto_disp_status = cbus_nvram_disp_status;
    }
    if (eisa_presto_installed) {
	    total_installed++;
            strcpy(nvr.presto_bus,"EISA");
	    nvr.presto_installed   = eisa_nvram_find;
	    nvr.presto_test_status = eisa_nvram_test_status;
	    nvr.presto_configured  = eisa_nvram_configured;
	    nvr.presto_zero        = eisa_nvram_zero;
	    nvr.presto_bat_enable  = eisa_nvram_bat_enable;
	    nvr.presto_disp_status = eisa_nvram_disp_status;
    }
    if (pci_presto_installed) {
	    total_installed++;
            strcpy(nvr.presto_bus,"PCI");
	    nvr.presto_installed   = pci_nvram_find;
	    nvr.presto_test_status = pci_nvram_test_status;
	    nvr.presto_configured  = pci_nvram_configured;
	    nvr.presto_zero        = pci_nvram_zero;
	    nvr.presto_bat_enable  = pci_nvram_bat_enable;
	    nvr.presto_disp_status = pci_nvram_disp_status;
    }

#if DEBUG
    printf("total_installed = %x\n",total_installed);
#endif

    /* make sure at least one NVRAM card is installed */
    if (total_installed == 0) {
	printf("No NVRAM Disk Cache installed\n");
	return(msg_failure);
    }

    /* If more than one NVRAM card is installed make the use specify */
    if (total_installed > 1) {
	printf("Multiple NVRAM Disk Caches installed\n");
	printf("Please specify: ");
	if (cbus_presto_installed)
	    printf("CBUS or ");
	if (eisa_presto_installed)
	    printf("EISA or ");
	if (pci_presto_installed)
	    printf("PCI ");

	printf("\n");
	return(msg_failure);
    }

    if (nvr.presto_installed() == msg_failure) {
	printf("No %s NVRAM Disk Cache installed\n",nvr.presto_bus);
	return(msg_failure);
    }

    if ((nvr.presto_test_status() == msg_failure) && !(qual[QF].present)) {
	printf("%s NVRAM Disk Cache failed powerup test\n",nvr.presto_bus);
	return(msg_failure);
    }

    if ((nvr.presto_configured() == msg_failure) && !(qual[QF].present)) {
	printf("%s NVRAM Disk Cache is not configured\n",nvr.presto_bus);
	return(msg_failure);
    }

    /* Make sure we have enough arguments */
    if (!(qual[QZ].present) &&
	!(qual[QU].present) &&
	!(qual[QF].present)) {
	printf("%s NVRAM Disk Cache installed\n",nvr.presto_bus);
	return(msg_failure);
    }           

    /* The -z qualifier */
    if(qual[QZ].present) {
	printf("This command will zero the %s NVRAM Disk Cache\n",
							nvr.presto_bus);
	read_with_prompt(msg_fe_areyousure, sizeof (answer), answer, 0, 0, 1);
	if ((answer[0] == 'y') || (answer[0] == 'Y')) {
#if DEBUG || 1
	    printf("clearing disk cache\n");
#endif
	    nvr.presto_zero();
	}
#if DEBUG
	else
	   printf("%s NVRAM Disk Cache unchanged\n",nvr.presto_bus);
#endif
    }

    /* The -u qualifier */
    if(qual[QU].present) {
#if DEBUG
	printf("disable battery disk cache\n");
#endif
	/* Disable the battery disconnect circuitry.  This allows the battery */
	/* to be used to backup the NVRAM during a powerfail. */
	nvr.presto_bat_enable();
    }

    /* The -f qualifier */
    if(qual[QF].present) {
	nvr.presto_disp_status();
    }

    return(msg_success);
}

/* Determine an unsigned 32 bit unique number from the Cobra system  */
/* serial number in the sys_serial_num environment variable. Convert */
/* the serial number from ascii to a hex number.  Any letter over 'F'*/
/* (or f) is converted to 0xf modulo the letter. */
int trans_presto_ssn() {

    struct EVNODE ev, *evp;
    unsigned int i, csr_base, ssn;
    char *cp;

    ssn = 0;
    evp = &ev;
    if (ev_read( "sys_serial_num", &evp, 0 ) == msg_success) {
	cp = evp->value.string + 9;

	for (i=0; i<8; i++, cp--) {
	    if (*cp < '9')
		ssn += (*cp - '0' ) << (i*4);
	    else if (*cp < 'G')
		ssn += (*cp - 'A' + 0xa ) << (i*4);
	    else if (*cp < 'a')
		ssn += ( *cp % 0xf ) << (i*4);
	    else if (*cp < 'g')
		ssn += (*cp - 'a' + 0xa ) << (i*4);
	    else 
		ssn += ( *cp % 0xf ) << (i*4);
	}
	return(ssn);
    }
    else
	return(0);
}
