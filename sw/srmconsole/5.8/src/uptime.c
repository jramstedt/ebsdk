/* file:	uptime.c
 *
 * Copyright (C) 1994 by
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
 *      Console Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *      Display the days, hours, minutes and seconds since the console has
 *	been running.  The timer is restarted by an init or reset.
 *
 *  AUTHORS:
 *
 *      Harold Buckingham
 *
 *  CREATION DATE:
 *  
 *      11-May-1994
 *
 *  MODIFICATION HISTORY:
 *
 *      hcb     11-May-1994	Initial entry.
 *
 *--
 */

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$inc:prototypes.h"

extern unsigned int poweron_msec [2];

/*+
 * ============================================================================
 * = uptime - Display the elapsed time since system initialization.           =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      Displays the days, hours, minutes and seconds since the console has
 *	been running.  The timer is restarted by an init or reset.
 *  
 *   COMMAND FMT: uptime 2 0 0 uptime
 *
 *   COMMAND FORM:
 *  
 *	uptime()
 *  
 *   COMMAND TAG: uptime 0 RXBZ uptime
 *
 *   COMMAND ARGUMENT(S):
 *
 *	None
 *
 *   COMMAND OPTION(S):
 *
 *	none
 *
 *   COMMAND EXAMPLE(S):
 *~
 *	>>>uptime 
 *	0 10:29:04
 *	>>>
 *~
 *   COMMAND REFERENCES:
 *
 *	None
 *
 * FORM OF CALL:
 *  
 *	uptime ( int argc, char *argv[] )
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
void uptime (int argc, char *argv[]) {
    unsigned __int64 total_seconds;
    unsigned int days, hours, minutes, seconds;

    total_seconds = (*(unsigned __int64*)&poweron_msec)/1000;

    seconds = (unsigned int)((total_seconds) % 60);
    minutes = (unsigned int)((total_seconds/(60)) % 60);
    hours   = (unsigned int)((total_seconds/(60*60)) % 24);
    days    = (unsigned int)((total_seconds/(60*60*24)));

#if 0
    printf("uptime : %d\n", *(unsigned __int64*)&poweron_msec);
    printf("total_seconds: %d\n", total_seconds);
#endif

    printf("%d %02d:%02d:%02d\n", days, hours, minutes, seconds);
}
