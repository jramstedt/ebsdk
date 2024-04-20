/* file:	secure.c
 *
 * Copyright (C) 1992, 1993 by
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
 *      This includes the console command LOGIN and the supporting 
 *	routines for setting the environment variable PASSWORD and 
 *	global secure.
 *
 *  AUTHORS:
 *
 *      Carl B. Furbeck
 *
 *  CREATION DATE:
 *  
 *      23-Dec-1992
 *
 *  MODIFICATION HISTORY:
 *      jhs     25-april-1997  added OS callback support
 *
 *      cbf     26-Feb-1993     modify set_secure to work with "2" because
 *                              noarg removed from filesys.c
 *
 *      cbf     5-Feb-1993      pwd_init should look for pwd_valid only
 *
 *      cbf     2-Feb-1993      improve headers
 *
 *	cbf	27-Jan-1993	added ev_pwd_init routine
 *
 *      cbf	27-Jan-1993     modified clear pwd to ask for old pwd
 *
 *      cbf     23-Jan-1993     initial release
 *
 *--
 */

#include	"cp$src:platform.h"
#include "cp$src:common.h"
#include	"cp$src:kernel_def.h" 
#include        "cp$src:dynamic_def.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:ev_def.h"
#include        "cp$src:ctype.h"
#include	"cp$inc:kernel_entry.h"
#include "cp$inc:prototypes.h"

#define MIN_PWD_LEN 15   
#define MAX_PWD_LEN 30   
#define PWD_LEN (MAX_PWD_LEN+2)           /* read_with_prompt adds null+cr    */
#define malloc(x) dyn$_malloc(x,DYN$K_SYNC)
#define free(x)   dyn$_free(x,DYN$K_SYNC)
#define FF_PATTERN -1                   /* FF pattern.                      */
#define PWD_MASK       0x0FFFFFFF       /* pwd only 28 bits                 */
#define PWD_VALID_MASK 0x10000000       /* pwd valid bit mask               */
#define CRC_32 79764919                 /* Coefficients of CRC-32 polynomial. */
#define M_BIT0   1                      /* Mask for bit 0.                  */
#define M_BIT31 -2147483648             /* Mask for bit 31.                 */

/* Define driver structures. */
union PTR_UNION_TAG {
    unsigned char *bptr;
    unsigned short int *wptr;
    unsigned int *ptr;
    } ;

#if !MODULAR
extern ev_sev_init();
#endif

extern int read_with_prompt();
extern int ev_initing;
extern int _align (LONGWORD) robust_mode;

int secure;	/* global int */
int loginfail= 0;   /* failed login attempts count.It will be capped at 9(max)*/
extern struct EVNODE evlist;

/* prototypes */
int clear_password();
int login( int argc, char *argv[] );
int set_secure(int a);
int ev_wr_pwd( char *name, struct EVNODE *evptr );
int ev_pwd_init( struct env_table *et, char *value );

static char askpwd[]             = "Please enter the password: ";
static char askpwd2[]            = "Please enter the password again: ";
static char askoldpwd[]          = "Now enter the old password: ";

/*+
 * ============================================================================
 * = SET_SECURE - set global variable secure                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      This routine will set the global variable secure to 1 or 0.
 *	If the routine is called with argument of 0, then the variable
 *   	is set to zero, disabling the secure features.  If the routine
 *   	is called with an argument of 1, the environment variable password 
 *	is read from NVR and bit 28 is checked.  If the bit is set (1), then
 *	the global variable secure is set to 1.  If there is no valid password
 *      stored in NVR,  secure is set to zero.
 *  
 * FORM OF CALL:
 *  
 *	set_secure ( int a )
 *  
 * RETURN CODES:     
 *
 *	msg_success - success
 *
 * ARGUMENTS:
 *
 *	0 - set secure to "0" 
 *	1 - if password_valid bit = 1, set secure to "1" else "0"
 *
 * SIDE EFFECTS:
 *
 *      Global variable secure may be set.
 *                     
-*/

int set_secure( int a )
{
    struct EVNODE ev;                          
    struct EVNODE *evp;                          

	switch ( a ) {
		case 0 : secure = 0;
                         break;
                case 1 : 

			evp = &ev;
            		ev_read ("password", &evp, EV$K_SYSTEM);
            		if (evp->value.integer & PWD_VALID_MASK) {
               			secure = 1;
			} else {
               			secure = 0;
            		}
			break;
		case 2 : secure = a;
                        break;
                }	
    return msg_success;
}

/*+
 * ============================================================================
 * = CAP_AND_PAD - capitilize string and pad array to MAX_PWD_LEN             =
 * ============================================================================
 * OVERVIEW:
 *  
 *	This command accepts an ASCII string as input and converts all it's 
 *	elements from lower case to upper case and pads the arrary with space 
 *	characters to MAX_PWD_LEN in length.  If an element in the string is 
 *	not an alpha or numeric character, it is replaced with a space.  
 *
 * FORM OF CALL:
 *  
 *      cap_and_pad ( char *string )
 *
 * RETURN CODES:     
 *
 *	msg_success - success
 *
 * ARGUMENTS:
 *
 *	string - pointer to the string to be capitilized and padded
 *
 * SIDE EFFECTS:
 *
 *      argument is capitilized and the array padded to MAX_PWD_LEN characters
 *	with spaces.
 *                     
-*/
int cap_and_pad ( char *string )
{
    int len, i;
    char temp[MAX_PWD_LEN];
    
    len = strlen( string );
    for( i = 0; i < MAX_PWD_LEN; i++ )
    {
        if( i < len )
        {
             if ( isalnum(string[i]) )
                 temp[i] = toupper( string[i] );
             else
                 temp[i] = ' ';
        }
        else
        {
             temp[i] = ' ';
        }
    } 
    strncpy( string, temp, MAX_PWD_LEN );
    return msg_success;
}

/*+
 * ===================================================
 * = PASSWORD_FCS - Calculate the Frame Check Sequence =
 * ===================================================
 *
 * OVERVIEW:
 *
 *	This routine calculates a 32-bit Frame Check Sequence (FCS) 
 *	for the specified data buffer using the CRC-32 generating 
 *	polynomial.  The FCS of the password is used as the stored
 *      value for the secure password.
 *  
 * FORM OF CALL:
 *  
 *     password_fcs(buff_len,buff_addr);
 *
 * RETURN CODES:
 *
 *	Unsigned int - unsigned integer containing the FCS.
 *
 * ARGUMENTS:
 *
 *	buff_len - Unsigned int indicating number of bytes in buffer.
 *	 
 *	buff_addr - Pointer to array of unsigned bytes.
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
 * FUNCTIONAL DESCRIPTION:
 *
 *	This function accepts a pointer to a buffer whose length in bytes
 *	is specified and calculates the 32-bit CRC using the CRC-32
 *	generating polynomial:
 *~	
 *	G(X) = X**32 + X**26 + X**23 + X**22 + X**16 + X**12 + X**11 +
 *	       X**10 + X**8 + X**7 + X**5 + X**4 + X**2 + X + 1
 *~
 *
 *	The FCS is the complement of the longword resulting from the
 *	CRC-32 generating polynomial.
 *
 *	The CRC-32 generating polynomial is implemented as a software 
 *	feedback shift register.  Refer to DEC STD 134, The Digital
 *	Ethernet Specification, section 6.2.4 and appendix C.
 *
 * ALGORITHM:
 *~
 *	sh_reg = %xFFFF.FFFF
 *	cur_byte = buff_addr
 *
 *	FOR count = 0 TO buff_len -1 DO
 *	: cur_byte = cur_byte + 1
 *	: FOR bit = 0 TO 7 DO
 *	: : IF (sh_reg AND M_BIT31)
 *	: : : fdbk = 1
 *	: : ELSE
 *	: : : fdbk = 0
 *	: : ENDIF
 *	: : IF (fdbk XOR (input AND M_BIT0))
 *	: : : sh_reg = ((sh_reg << 1) XOR CRC_32)
 *	: : ELSE
 *	: : : sh_reg = sh_reg << 1
 *	: : ENDIF
 *	: : input = input >> 1
 *	: NEXT bit
 *	NEXT count
 * 
 *	Swap sh_reg bits end for end for crc (crc<0:31> = sh_reg<31:0>)
 *	fcs_value = complemented(crc)
 *	fcs_value & pwd_mask (bits 0-27)
 *	fcs_value | pwd_valid (set bit 28)
 *	Return(fcs_value)
 *~ 
-*/

int password_fcs(register unsigned int buff_len, 
	     register unsigned char *buff_addr)
{
    union PTR_UNION_TAG cur;		/* Pointer to current byte 
					    in message buffer. */
    unsigned int count;			/* Position/number of current
					    byte in buffer. */
    unsigned int input;			/* Input "shift reg" for current
					    byte in buffer. */
    unsigned int bit;			/* Current bit in current byte. */
    unsigned int sh_reg;		/* Software CRC "shift register".*/
    unsigned int fdbk;			/* Feedback bit (sh_reg<31>). */
    unsigned int crc;			/* CRC-32. */
    unsigned int fcs_value;		/* Complemented CRC-32 or FCS. */
    unsigned char temp_value;

    /* Initialize shift register, pointer. */

    sh_reg = FF_PATTERN;		    /* Init. shift register to ones.*/
    cur.bptr = buff_addr;		    /* Init. current byte ptr. */

    /* While message buffer not empty, read byte and process it. */
    for (count = 0;  count < buff_len;  count++)
    {
        temp_value = (*cur.bptr);
        input = (unsigned int)temp_value;
	cur.bptr++;			    /* Point at next byte in buffer.*/

	for (bit = 0;  bit < 8;  bit++) /* Serialize input byte. */
	{
            /* Generate feedback bit. */
	    if (sh_reg & M_BIT31)
	        fdbk = 1;
	    else
	        fdbk = 0;

            /* Generate feed forward bit and shift CRC reg. */
            if (fdbk ^ (input & M_BIT0))
		sh_reg = (sh_reg << 1) ^ CRC_32;
	    else
		sh_reg = sh_reg << 1;

	    input = input >> 1;	    /* Next input bit. */
	}
    }

    /* Compose CRC by emptying sh_reg one bit at a time; sh_reg<31:0> becomes
       CRC<0:31>.  Complement CRC and return. */

    crc = 0;
    for (bit = 0;  bit < 32;  bit++)
    {
	crc = crc << 1;
	if (sh_reg & M_BIT0)
	    crc = crc | M_BIT0;
	sh_reg = sh_reg >> 1;
    }
    /* 
    ** compliment, truncate to 28 bits and set password valid bit 
    */
    fcs_value = ~crc;
    fcs_value &= PWD_MASK;
    fcs_value |= PWD_VALID_MASK;

    return(fcs_value);
}

/*+
 * ============================================================================
 * = EV_WR_PWD - write password and password valid bit in NVR                 =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine is specified as the write routine for the environment
 *	varaiable, password.  This routine is passed the name of the 
 *	environment variable to be modified (always password) and a pointer 
 *	to the evronmental table.
 *
 *	If this write is the result of the command, clear password, then
 *	the password_valid bit (28) will be 0 while the password field
 *	(bits 0-27) should be non-zero.  If these conditions are met, the
 *	password field is written with zero (cleared).
 *
 *	If the above condition is not met, then the code will prompt the
 *	user to enter a new password.  The password will be checked for
 * 	length.  If the password is within the specified range, the code
 *	will prompt the user to enter the password again to verify the
 *	change.  If there is no password in NVR, then the new password
 *	is written.  If there is a password stored in NVR, then the user
 *	must enter it correctly before the new password is written to NVR.
 *  
 * FORM OF CALL:
 *  
 *	ev_wr_pwd( char *name, struct EVNODE *evptr )
 *  
 * RETURN CODES:     
 *
 *	msg_success  	   - success
 *	msg_curr_psswd_err - user did not know the correct current password
 *      msg_val_err        - error validating new password
 *      msg_psswd_len_err  - err in password length
 *
 * ARGUMENTS:
 *
 *	name - name of varible being written (always password)
 *      evptr - pointer to evironmental table.
 *
 * SIDE EFFECTS:
 *
 *      Environment variable "password" is modified in NVR
 *                     
-*/

int ev_wr_pwd( char *name, struct EVNODE *evptr )
{

    struct EVNODE *evp;

    int i;
    int len;
    int match;
    unsigned int fcs;
    unsigned int oldpwdfcs;
    unsigned int validate;
    char pwd[PWD_LEN];
    char pwd2[PWD_LEN];
    char pwdvld[PWD_LEN];
    char oldpwd[PWD_LEN];

   if( ev_initing )
	return( msg_success );

   /* 
   ** read the password.  If pwd_valid clear and pwd != 0 then
   ** clear the password.  The next time in, you can set the password.
   */
   evp = (void *)malloc(sizeof(struct EVNODE));
   ev_read ("password", &evp, EV$K_SYSTEM);
   validate = evp->value.integer;
   free (evp);
 
   if ( (validate & PWD_MASK ) && !(validate & PWD_VALID_MASK) ) 
   {
       evptr->value.integer = (int)0;
       ev_sev( name, evptr );	/* write pwd to non-volatile RAM */
       return msg_success;
   }
   /*
   ** Ask for the new password and change the password to all
   ** capitals and pad to MAX_PWD_LEN characters.   Len must be
   ** minus 1 for newline put on by read_with_prompt.
   */
   len = ( (read_with_prompt( askpwd, PWD_LEN, pwd, 0, 0, 0 ) - 1 ) );
   cap_and_pad( pwd );        

   if ( ( len < MIN_PWD_LEN ) || ( len > MAX_PWD_LEN ) )
   {
       err_printf( msg_psswd_len_err, MIN_PWD_LEN, MAX_PWD_LEN );
       return msg_failure;
   }
 
   /*
   ** Ask to validate the new password and change the password to all
   ** capitals and pad to MAX_PWD_LEN characters
   */
   read_with_prompt( askpwd2, PWD_LEN, pwd2 ,0,0,0);
   cap_and_pad( pwd2 );

   match = strncmp( pwd, pwd2, MAX_PWD_LEN );      /* return 0 if equal */
   if ( match )
   { 
       err_printf( msg_val_err );
       return msg_failure;
   }

   if (validate & PWD_VALID_MASK)
   {
       match = 1;  
       /*
       ** Ask for the old password and change the password to all
       ** capitals and pad to MAX_PWD_LEN characters
       */
       read_with_prompt( askoldpwd, PWD_LEN, oldpwd ,0,0,0);
       cap_and_pad( oldpwd );
       oldpwdfcs = password_fcs( (unsigned int)MAX_PWD_LEN, oldpwd );
   }

   /* encrypt the password for storage in NVR */
   fcs = password_fcs( (unsigned int)MAX_PWD_LEN, pwd );

   /* if old passwords match or if password_valid 0, write to NVR */
   if( ( oldpwdfcs == validate ) || ( !match ) )
   {
       evptr->value.integer = fcs;
       ev_sev( name, evptr );	/* write pwd to non-volatile RAM */
   }
   else
   {
       err_printf( msg_curr_psswd_err );
       return msg_failure;
   }
   return msg_success;
}  /* end wr_pwd */

/*+
 * ============================================================================
 * = VALId_SAVE_PWD - Validate the password passed in argument
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      This routine will validate the password passed in as an argument
 *      with the existing password EV. If there is no password it will return
 *      error if no password exists and a password is still passed in. This
 *      routine will be called only in the callbacks routine to verify
 *      if the requested callback is valid.
 *
 *      If the save argument is set, then use the second argument as the new
 *      password and save it
 * FORM OF CALL:
 *
 *      valid_save_pwd( char *pwd , char *new_pwd , int save)
 *
 * RETURN CODES:
 *
 *      msg_success        - success
 *      msg_no_vldpsswd     - user did not know the correct current password
 *      msg_val_err        - error validating new password
 *      msg_psswd_len_err  - err in password length
 *
 * ARGUMENTS:
 *
 *      pwd - pointer to varible containing password string.
 *
 * SIDE EFFECTS:
 *
 *
-*/

int valid_save_pwd( char *pwd , char * new_pwd, int save )
{

    struct EVNODE *evptr;

    unsigned int pwdfcs;
    unsigned int validate;

    ev_locate(&evlist, "password",&evptr);
    validate = evptr->value.integer;

    if (!validate && !save)
        return msg_no_vldpsswd;

    if ((!validate) && (*pwd)) /* no password in EV but pwd had something?? */
    {
      if (loginfail < 9 )
          loginfail += 1;
    return msg_invld_psswd;
    }

    cap_and_pad( pwd );
    pwdfcs = password_fcs( (unsigned int)MAX_PWD_LEN, (unsigned char *)pwd );

    /* if old passwords match  */
    if(  pwdfcs != validate  )
    {
      if (loginfail < 9 )
          loginfail += 1;
    return msg_invld_psswd;
    }
    if (save)           /* save new password passed as second arg */
     {
        if (*new_pwd) {
             cap_and_pad( new_pwd );
             pwdfcs = password_fcs( (unsigned int)MAX_PWD_LEN,
                                 (unsigned char *)new_pwd );
             evptr->value.integer = pwdfcs;
         }
        else
             evptr->value.integer = 0;
	 evptr->attributes |= EV$M_MODIFIED; 
     }

    return msg_success;
}   /* end valid_save_pwd */


/*+
 * ============================================================================
 * = CLEAR_PASSWORD - clear the password and password_valid bit(s)            =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This module is called as a special case to the clear command in
 *	the module, setshow.c.  This commands clears only the password
 *	valid bit in environment variable, password.  When the write
 *	routine (ev_wr_pwd) is called from ev_write(), the environment
 *	variable password is set to zero (0).  There must be a way to 
 *	clear the password if it is forgotten.  This method MUST include
 *      access to the hardware.
 *  
 * FORM OF CALL:
 *  
 *	clear_password ( )
 *  
 * RETURN CODES:     
 *
 *	msg_success  	- success
 *	msg_failure	- user did not know the correct current password
 *
 * ARGUMENTS:
 *
 *	name - name of variable being written (always password)
 *      evptr - pointer to evironmental table w/values.
 *
 * SIDE EFFECTS:
 *#o
 *      Environment variable "password" is modified in NVR
 *      Global secure set.
 *#                     
-*/

int clear_password()
{
   struct EVNODE *evp;
   unsigned int fcs;
   int rstat, reject;
   char pwd[PWD_LEN];

   evp = (void *)malloc(sizeof(struct EVNODE)); 
   ev_read ("password", &evp, EV$K_SYSTEM);
   if( evp->value.integer & (PWD_VALID_MASK | PWD_MASK ) ) 
   {
       if (!robust_mode) {
           read_with_prompt( askpwd, PWD_LEN, pwd ,0,0,0);
           cap_and_pad( pwd );
           fcs = password_fcs( (unsigned int)MAX_PWD_LEN, pwd );
           if ( fcs != evp->value.integer ) {
               err_printf( msg_curr_psswd_err );
               return msg_failure;
           }
       }

       evp->value.integer &= PWD_MASK;  /* clear PWD_VALID */
       rstat = ev_write("password",evp->value.integer, 
                         EV$K_INTEGER | EV$K_NOREAD | EV$K_NODELETE | 
                         EV$K_ROUTINE | EV$K_NONVOLATILE );

       if( !rstat )
           err_printf( msg_pwd_cleared );
   }
   free (evp);  
   return msg_success;
}

int fsb_clear_password()
{
   struct EVNODE *evp;
   unsigned int fcs;
   int rstat, reject;

   evp = (void *)malloc(sizeof(struct EVNODE)); 
   ev_read ("password", &evp, EV$K_SYSTEM);
   if( evp->value.integer & (PWD_VALID_MASK | PWD_MASK ) ) 
   {
       evp->value.integer &= PWD_MASK;  /* clear PWD_VALID */
       rstat = ev_write("password",evp->value.integer, 
                         EV$K_INTEGER | EV$K_NOREAD | EV$K_NODELETE | 
                         EV$K_ROUTINE | EV$K_NONVOLATILE );

       if( !rstat )
           err_printf( msg_pwd_cleared );
   }
   free (evp);  
   return msg_success;
}

/*+
 * ============================================================================
 * = EV_PWD_INIT - set ctrl-X enble asap if no password stored in NVR         =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine is specified as the init routine for the environment
 *	varaiable, password.  When the NVR is being initialized during
 *	powerup, this routine is called to enable CTRL_X if there is no
 *      password stored in NVR.  If there is a valid password, then the
 *	CTRL_X function will remain disabled.
 *  
 * FORM OF CALL:
 *  
 *	ev_pwd_init ( struct env_table *et, char *value )
 *  
 * RETURN CODES:     
 *
 *      *et - pointer to the environment table 
 *      value - value found in nvr for password.
 *
 * ARGUMENTS:
 *
 *      et - pointer to env_table (in ev_action)
 *      value - pointer to value to be updated
 *
 * SIDE EFFECTS:
 *
 *	If there is not a valid password in NVR, turn CNTRL_X on.
 *
-*/

int ev_pwd_init( struct env_table *et, char *value )
{
    unsigned val;

    val = (unsigned)ev_sev_init( et, value );

    /*
    ** if the password is not set (cleared), enable ctrl_x
    */
    if ( !( val & PWD_VALID_MASK ) ) {
        val = (unsigned)0;

    /* if it is set, then we have to set secure */
    } else {
	secure = 1;
    }

    return( val );
} 

/*
 * The following headers are dummies. Their only purpose is for the on-line
 * help text file that is created by BUILD_HELPTEXT.AWK.
 */
/*+
 * ============================================================================
 * = set secure - Place the console in secure mode.                           =
 * ============================================================================
 *
 *  
 *   COMMAND FORM:
 *  
 *      set secure ( )
 *  
 *  
 *   COMMAND ARGUMENT(S):
 *
 *      None
-*/
/*+
 * ============================================================================
 * = set password - Set the password for the secure function.                 =
 * ============================================================================
 *
 *
 *  
 *   COMMAND FORM:
 *  
 *      set password (
 *                    Please enter the password: <15-30 characters>
 *                    Please enter the password again: <verify password> )
 *  
 *  
 *   COMMAND ARGUMENT(S):
 *
 *   	None
-*/
/*+
 * ============================================================================
 * = clear password - Clear the password for the secure function.             =
 * ============================================================================
 *
 *  
 *   COMMAND FORM:
 *  
 *      clear password (
 *                    Please enter the password: <password> )
 *  
 *  
 *   COMMAND ARGUMENT(S):
 *
 *      None
 *
-*/
