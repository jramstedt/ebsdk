/*
 * file:        lfu_startup_driver.c
 *
 * Copyright (C) 1993 by
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
 *
 * Abstract:    LFU startup Driver code
 *
 *		This is really a dummy driver used by the common 
 *		console platforms to autostart LFU.
 *
 * Author:      Bill Clemence
 *
 * Modifications:
 *
 *	wcc	 7-Dec-1993	First release
 *
 */

/*
* All include files here
*/
#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$inc:prototypes.h"
#include "cp$src:msg_def.h"

/*
*  All functions declared here
*/
extern void krn$_sleep(int milliseconds);
extern int cpip;	/* cpu powerup in progress */

int lfu_startup_init(void);


/*+
 * ============================================================================
 * = lfu_startup_init - Startup LFU 					      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Initalizes the lfu program driver, by just starting it.
 *  
 * FORM OF CALL:
 *  
 *	lfu_startup_init()
 *  
 * RETURN CODES:
 *
 *      msg_success
 *       
 * ARGUMENTS:
 *
 *      None
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int lfu_startup_init(void) 
{
   /* Sleep before we start for 10 seconds. This allows error messages to
    * be displayed before the LFU prompt appears.
   */
   krn$_sleep(10 * 1000);

   cpip = 0;

   /* do the meat-and-potatoes */
   lfu(0, "");

   return msg_success;
}
