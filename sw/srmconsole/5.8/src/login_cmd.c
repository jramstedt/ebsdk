/* file:	login_cmd.c
 *
 * Copyright (C) 1995 by
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
 *      Alpha Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *      This module implements the console secure features.  
 *      This includes the console command LOGIN.
 *
 *  AUTHORS:
 *
 *      Console Firmware Team
 *
 *  CREATION DATE:
 *  
 *      21-Nov-1995
 *
 *--
 */

#include	"cp$src:platform.h" 
#include	"cp$src:common.h"
#include	"cp$inc:prototypes.h"
#include	"cp$src:kernel_def.h" 
#include        "cp$src:dynamic_def.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:ev_def.h"
#include        "cp$src:ctype.h"
#include "cp$inc:kernel_entry.h"

#define MIN_PWD_LEN 15   
#define MAX_PWD_LEN 30   
#define PWD_LEN (MAX_PWD_LEN+2)		/* read_with_prompt adds null+cr */
#define PWD_MASK       0x0FFFFFFF       /* pwd only 28 bits */
#define PWD_VALID_MASK 0x10000000       /* pwd valid bit mask */

char askpwd[]             = "Please enter the password: ";
char askpwd2[]            = "Please enter the password again: ";
char askoldpwd[]          = "Now enter the old password: ";

/*+
 * ============================================================================
 * = login - Login to a secure console.                                       =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *   	The login command is part of the secure features for the console.
 *   	The command will prompt the user for a password, which will be
 *	compared to the current password stored in NVRAM.
 *
 *	If the user enters the current password, then the global variable,
 *	"secure" will be set to "0" and the console will be unsecure;
 *	that is, the user will have access to all console commands.
 * 
 *	If there is no valid password stored in NVRAM, then the user will
 *	be notified that the console has no valid password.
 *	No command line arguments are required.  
 *	The password will not be echoed to the terminal.
 *
 *   COMMAND FMT: login 0 S 0 login 
 *  
 *   COMMAND FORM:
 *  
 *	login (
 *             Please enter the password: <password> )
 *  
 *   COMMAND TAG: login 0 RXBP login 
 *  
 *   COMMAND ARGUMENT(S):
 *
 *   	None
 *
 *   COMMAND OPTION(S):
 *
 *	None
 *
 *   COMMAND EXAMPLE(S):
 *
 *	The following is an example of a successful login attempt:
 *~
 *	>>> login
 *	Please enter the password:(input not echoed to terminal) 
 *	>>>
 *~ 	
 *	Here is a failed login attempt:
 *~ 	
 *	>>> login
 *	Please enter the password:(input not echoed to terminal) 
 *	Validation error
 *	>>>
 *~ 	
 *   COMMAND REFERENCES:
 * 
 *	set password, set secure, clear password
 *
 * FORM OF CALL:
 *  
 *	login ( int argc, char *argv[] )
 *  
 * RETURN CODES:
 *
 *	msg_success - Indicates normal completion.
 *      msg_failure - Indicates the user did not know the current password.
 *       
 * ARGUMENTS:
 *
 * 	None - There are no arguments other than being prompted for the current
 *	password as stored in NVR.  Values passed in argv are not used.
 *
 * SIDE EFFECTS:
 *
 *      Global variable secure is set to "0" (false).
 *
-*/

int login( int argc, char *argv[] )
{
   struct EVNODE *evp;

   unsigned int fcs, halt_in; 
   unsigned int validate;
   char pwd[PWD_LEN];
   unsigned char reject=1;
   unsigned char rstat;

   set_secure( 0 );

   /*
   ** read in old password from NVR 
   */
   evp = (void *)malloc(sizeof(struct EVNODE));
   ev_read ("password", &evp, EV$K_SYSTEM);
   validate = evp->value.integer;

   if ( validate & PWD_VALID_MASK )
   {
       /* see if they made the call w/halt in */
       /* if they did, we'll reject the password */
       reject = halt_switch_in( TRUE );
       /*
       ** Ask the user for the password.  Change password to all capitals 
       ** and pad to MAX_PWD_LEN characters then encrypt it for comparing with 
       ** stored value
       */
       read_with_prompt( askpwd, PWD_LEN, pwd ,0,0,0);

       cap_and_pad( pwd );
       fcs = password_fcs( (unsigned int)MAX_PWD_LEN, pwd );

       halt_in = halt_switch_in( TRUE );

       if( ( fcs == validate ) || (  halt_in  && !reject ) ) 
       {
          /* set secure off */
          set_secure( 0 );
	  if( halt_in  && !reject ) {
       	      evp->value.integer &= PWD_MASK;  /* clear PWD_VALID */
              rstat = ev_write("password",evp->value.integer, 
                                EV$K_INTEGER | EV$K_NOREAD | EV$K_NODELETE | 
                                EV$K_ROUTINE | EV$K_NONVOLATILE );
	  }	
          rstat =  msg_success;
       }
       else
       {
          /* they didn't know the password */
          set_secure( 1 );
          err_printf( msg_invld_psswd );
          rstat = msg_failure;
       }
   }
   else
   { 
          err_printf( msg_no_vldpsswd );
	  rstat = msg_success;
   }
   free (evp);
   return rstat;
}
