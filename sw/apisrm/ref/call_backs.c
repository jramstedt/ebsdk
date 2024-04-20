/* file:	Call_Backs.c
 *
 * Copyright (C) 1990, 1992 by
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
 *      Console Call Back Routines - As defined in the SRM
 *
 *  AUTHORS:
 *
 *      David Baird
 *
 *  CREATION DATE:
 *  
 *      29-Jun-1990
 *
 *  MODIFICATION HISTORY:
 *
 *      tna	11-Nov-1998     added YukonA conditionals
 *
 *	jhs	7-MAY-1997	conditionalized with SECURE to fix builds
 *
 *      jhs     30-Apr-1997     added OS password callback support
 *
 *	jje	16-feb-1996	Initial Cortex support.  Added CORTEX
 * 				conditional in cb_get_ttfp 
 *				                         
 *	dch	01-Feb-1995	added opt_tga.h
 *
 *	noh	07-Nov-1995	BURNS behavior is now like AVMIN.
 *				Added BURNS conditional.
 *
 *	dtr	8-aug-1995	backed previous changes out until I understand
 *				why they didn't work
 *
 *	dtr	 7-Aug-1995	Corrected the call to rxon/txon to pass port
 *				block and not line number.  All calls are now
 *				consistant in the way port blocks are handled.
 *
 *	noh	 8-Jun-1995	AVMIN: Size of malloc block used in
 *				cb_read and cb_write was 32K.  Changed
 *				to 2K due to small heap size.
 *
 *	jrk	12-Apr-1995	removed obsolete documentation
 *--
 */

#define MAX_IO_SIZE	64 * 512

#include	"cp$src:platform.h"
#include	"cp$src:common.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:ev_def.h"
#include	"cp$src:hwrpb_def.h"
#include	"cp$src:impure_def.h"
#include	"cp$src:pal_def.h"
#include	"cp$src:tt_def.h"
#include	"cp$src:platform_cpu.h"
#include	"cp$src:wwid.h"
#include	"cp$inc:kernel_entry.h"
#include	"cp$inc:prototypes.h"
#include	"cp$src:ni_dl_def.h"
#include        "cp$src:pb_def.h"
#include        "cp$src:ub_def.h"
#include	"cp$inc:platform_io.h"

#if AVMIN
#undef MAX_IO_SIZE
#define MAX_IO_SIZE	4 * 512
#endif

#if MODULAR
extern struct TTPB  *serial_ttpbs;
extern int boot_cpu_slot;	/* boot hwrpb slot pointer */
#endif

#if TURBO
#include "cp$src:turbo_eeprom.h"
#include "cp$src:turbo_mxpr.h"
extern struct SEMAPHORE update_eeprom_sem;
extern int save_env_area_ptr;
extern int eeprom_cb_change_flag;
#endif

#define DEBUG 0

#if MODULAR
#define CALLBACK_DEBUG 1
#else
#define CALLBACK_DEBUG 1
#endif
#define MAX_PWD_LEN 30

/*
 * 1 = open, close
 * 2 = read, write
 * 4 = getenv, setenv, saveenv
 * 8 = getc, puts, set_term_int
 * 10 = pswitch
 * 20 = eerom read, write
 */
#if CALLBACK_DEBUG
int callback_debug_flag = 0;
#endif

int cb_getc();
int cb_puts();
int cb_set_term_int();
int cb_set_term_ctl();
int cb_process_keycode();
int cb_open();
int cb_close();
int cb_read();
int cb_write();
int cb_ioctl();
#if SECURE
int cb_reset_env();
#endif
int cb_set_env();
int cb_get_env();
int cb_save_env();
void cb_pswitch();
#if GALAXY_CALLBACKS
int cb_galaxy();
#endif
#if EEROM_CALLBACKS
int cb_read_eerom();
int cb_write_eerom();
#endif

/* Table of pointers to callback routines */
int (*cb_table[])() = {
	0,
	cb_getc,
	cb_puts,
	0,
	cb_set_term_int,
	cb_set_term_ctl,
	cb_process_keycode,
	0,
	0,
    	0,
	0,
	0,
	0,
	0,
	0,
	0,
	cb_open,
	cb_close,
	cb_ioctl,
	cb_read,
	cb_write,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	cb_set_env,
#if SECURE
	cb_reset_env,
#else
	0,
#endif
	cb_get_env,
	cb_save_env,
	0,
	0,
	0,
	0,
#if GALAXY_CALLBACKS
	cb_galaxy,		/* 0x28 */
#else
	0,
#endif
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	cb_pswitch,
	0,
#if GALAXY_CALLBACKS
	cb_galaxy,		/* 0x32 */
#else
	0,
#endif
#if EEROM_CALLBACKS
	cb_write_eerom,		/* 0x33 */
	cb_read_eerom		/* 0x34 */
#else
	0,
	0
#endif
	};

extern char SL_ICR_COPY[8];
char *sl_icr_copy_ptr[] = SL_ICR_COPY;
extern int graphics_console;
extern int cb_ref;
extern int cb_fp[];
extern int cb_tt_fp[];
extern struct HWRPB *hwrpb;
extern struct TTPB *console_ttpb;
extern struct TTPB  serial_ttpb;
extern struct env_table *env_table_ptr;
extern int primary_cpu;
extern int timer_cpu;
extern int spl_kernel;
extern int main_shell_pid;
extern struct QUEUE _align (QUADWORD) pcbq;
			/* linked list of all known process blocks */

#if MODULAR
extern struct DDB *dk_ddb_ptr;
extern struct DDB *mk_ddb_ptr;
#else
extern struct DDB dk_ddb;
extern struct DDB mk_ddb;
#endif

#if SECURE
extern int secure;
extern int loginfail;
int valid_save_pwd(char *pwd,char *new_pwd, int save);
#endif

#if FIBRE_CHANNEL
extern struct SEMAPHORE wwids_lock;
#endif


void krn$_callback( struct impure *impure )
{
int index;
int (*callback)();

#if TL6
    write_leds(0x99);
#endif
#if TURBO
    krn$_wait(&update_eeprom_sem);
#endif
    index = impure->cns$gpr[0][2*16];
    if( index < sizeof( cb_table ) / 4 )
	callback = cb_table[index];
    else
	callback = 0;
    if( callback )
	callback( impure );
#if TURBO
    krn$_post(&update_eeprom_sem);
#endif
#if TL6
    write_leds(0x00);
#endif
}

#if AVANTI || MIKASA || ALCOR || K2 || MTU || TAKARA || APC_PLATFORM || REGATTA || WILDFIRE || MONET || EIGER

extern int TgaCount;

int cb_get_ttfp (int unit) {
    char *string;
    struct FILE *ttfp;

    /* Get the saved fp, and return it if it's valid. */
    ttfp = cb_tt_fp[unit];

    if (ttfp) return ttfp;

    /* If graphics is primary console:			
	    unit 0 (output=graphics,input=keyboard)
	    unit 1 (output=tta0,input=tta0  COM1 )	
	    unit 2 (output=kbd,input=kbd-no translation)	

       Else If serial is primary console:			
	    unit 0 (output=tta0 ,input=serial tta0 COM1 )	
	    unit 2 (output=kbd,input=kbd-no translation)	
	    unit 3 (output=mouse,input=mouse)	
	
       To Allow for portability...I split this to allow
       for differences when the unit designators differ
       when going between serial and graphics. This 
       currently does not support unit 3,4 or 5    */

       if ( graphics_console) 
        {
	/* Graphics Head Primary Console	*/
	switch (unit)
   	 {
	 case 0:
		/* This will be vga0 or tga0    */
		string=console_ttpb->ip->name;
		break;
	 case 1:
		string="tta0";		/* COM1 */
		break;
#if 0	/* keyboard and mouse are not supported. */
	 case 2:
		/* kbd should be here */
		string="tta1";		/* COM2 */
		break;
#endif
	 default:
		string = "NO_SUCH_DEVICE";
	 }
 	}	/* End if graphics console	*/

       else 
        {
	/* Serial Line Primary Console		*/
	switch (unit)
   	 {
	 case 0:
		string = console_ttpb->ip->name;
		break;

#if 0	/* keyboard and mouse not currently supported */
	 case 2:
		/* kbd should be here */
		string="tta1";		/* COM2 */
		break;
	 case 3:
		/* mouse should be here */
		string="tta1";		/* COM2 */
		break;
#endif
	default:
		string = "NO_SUCH_DEVICE";
	 }
 	}	/* End if serial console	*/

 /* Now that we know what device driver		*/
 /* Open the device for silent,permanent,write  */
 /* and Immediate access..sipw...sure		*/

 ttfp = fopen (string, "sipr+");

 /* And then fill in the file pointer for later	*/
 /* Access by devices				*/
 cb_tt_fp[unit] = ttfp;
 return ttfp;
}

#else

int cb_get_ttfp (int unit) {
    char *string;
    struct FILE *ttfp;

    /*
     * Make sure the unit is valid.
     */
    if ((unit < 0) || (unit > *hwrpb->NUM_CTBS))
	return 0;

    /*
     * Get the saved fp, and return it if it's valid.
     */
    ttfp = cb_tt_fp[unit];
    if (ttfp)
	return ttfp;

    /*
     * If the fp is not valid, try to open the unit specified.
     */
    switch (unit) {
#if MEDULLA || CORTEX || YUKONA
    case 0:
#if TGA
	string = console_ttpb->ip->name;
	break;
    case 1:
	string = "tta0";
	break;
#else
	string = "tta0";
	break;
#endif
#endif
#if SABLE || TURBO || RAWHIDE
    case 0:
	string = console_ttpb->ip->name;
	break;
    case 1:
	string = "tta0";
	break;
#endif
    default:
	return 0;
    }
    ttfp = fopen (string, "sipw");
    cb_tt_fp[unit] = ttfp;
    return ttfp;
}

#endif

/*+
 * ============================================================================
 * = getc - get character from console terminal                               =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Refer to SRM.
 *  
 * FORM OF CALL:
 *  
 * RETURNS:
 *
 * ARGUMENTS:
 *
 * SIDE EFFECTS:
 *
-*/
void cb_getc(struct impure *IMPURE) {
    uint64 src;
    unsigned char string[1];
    int unit;
    int i;
    struct FILE *ttfp;
    
    unit = IMPURE->cns$gpr[0][2*17];

    ttfp = cb_get_ttfp (unit);

    /*
     * If the fp is valid, read a character.  Otherwise, return an error.
     */
    if (ttfp) {
	/*
	 * Try to get a character.
	 */
	i = fread (string, 1, 1, ttfp);

	if (i) {

#if CALLBACK_DEBUG
    if (callback_debug_flag & 8)
	qprintf("cb_getc: unit %x fread %x %x\n", unit, i, string[0]);
#endif
	    IMPURE->cns$gpr[0][2*0] = string[0];
	    IMPURE->cns$gpr[0][2*0+1] = 0;
	} else {
	    IMPURE->cns$gpr[0][2*0] = 0;
	    IMPURE->cns$gpr[0][2*0+1] = 0x80000000;
	}
#if !(MEDULLA || CORTEX || YUKONA)

    } else if (unit == 2) {

	i = kbd_get ();	    		/* hard coded for now */

#if CALLBACK_DEBUG
    if (callback_debug_flag & 8)
	qprintf("cb_getc: kbd_get %x\n", i);
#endif
	/* -1 says error */
	if (i == -1) {
	    IMPURE->cns$gpr[0][2*0] = 0;
	    IMPURE->cns$gpr[0][2*0+1] = 0x80000000;
	} else {
	    IMPURE->cns$gpr[0][2*0] = i;
	    IMPURE->cns$gpr[0][2*0+1] = 0;
	}

    } else if (unit == 3) {

	i = mouse_get();

#if CALLBACK_DEBUG
    if (callback_debug_flag & 8)
	qprintf("cb_getc: mouse_get %x\n", i);
#endif
	/* -1 says error */
	if (i == -1) {
	    IMPURE->cns$gpr[0][2*0] = 0;
	    IMPURE->cns$gpr[0][2*0+1] = 0x80000000;
	} else {
	    IMPURE->cns$gpr[0][2*0] = i;
	    IMPURE->cns$gpr[0][2*0+1] = 0;
	}
#endif
    } else {
	IMPURE->cns$gpr[0][2*0] = 0;
	IMPURE->cns$gpr[0][2*0+1] = 0xc0000000;
    }
    IMPURE->cns$gpr[0][2*0+1] |= unit;
}

/*+
 * ============================================================================
 * = puts - put string to console terminal                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Refer to SRM.
 *
 * FORM OF CALL:
 *  
 * RETURNS:
 *
 * ARGUMENTS:
 *
 * SIDE EFFECTS:
 *
-*/
void cb_puts(struct impure *IMPURE) {
    unsigned char *string;
    int unit;
    int i;
    struct FILE *ttfp;
    
    unit = IMPURE->cns$gpr[0][2*17];
    string = &IMPURE->cns$gpr[0][2*22];
    i = IMPURE->cns$gpr[0][2*23];
    ttfp = cb_get_ttfp (unit);

    /*
     * If the fp is valid, write the string.  Otherwise, return an error.
     */
    if (ttfp) {
	/*
	 * Write only one to eight characters.
	 */
	i = fwrite (string, 1, i, ttfp);

	IMPURE->cns$gpr[0][2*0] = i;
	IMPURE->cns$gpr[0][2*0+1] = 0;

#if !(MEDULLA || CORTEX || YUKONA)

    } else if (unit == 2) {

	i = kbd_put (string[0], 0x60);	/* hard coded for now */

#if CALLBACK_DEBUG
    if (callback_debug_flag & 8)
	qprintf("cb_puts: unit %x string %x i %x\n", unit, string[0], i);
#endif
	/* -1 says error */
	if (i == -1) {
	    IMPURE->cns$gpr[0][2*0] = 0;
	    IMPURE->cns$gpr[0][2*0+1] = 0x80000000;
	} else {
	    IMPURE->cns$gpr[0][2*0] = 1;
	    IMPURE->cns$gpr[0][2*0+1] = 0;
	}

    } else if (unit == 3) {

	i = mouse_put (0xd4, 0x64);

	i = mouse_put (string[0], 0x60);

#if CALLBACK_DEBUG
    if (callback_debug_flag & 8)
	qprintf("cb_puts: unit %x string %x i %x\n", unit, string[0], i);
#endif
	/* -1 says error */
	if (i == -1) {
	    IMPURE->cns$gpr[0][2*0] = 0;
	    IMPURE->cns$gpr[0][2*0+1] = 0x80000000;
	    i = 0;
	} else {
	    IMPURE->cns$gpr[0][2*0] = 1;
	    IMPURE->cns$gpr[0][2*0+1] = 0;
	}
#endif
    } else {
	IMPURE->cns$gpr[0][2*0] = 0;
	IMPURE->cns$gpr[0][2*0+1] = 0xc0000000;
    }
}

/*+ 
 * ============================================================================ 
 * = process_keycode - processes and translates keycode                       =
 * ============================================================================
 *
 * OVERVIEW: 
 *
 *	Refer to SRM.
 * 
 * FORM OF CALL:
 *
 * RETURNS:
 *
 * ARGUMENTS:
 *
 * SIDE EFFECTS:
 *
-*/
void cb_process_keycode(struct impure *IMPURE) {
    pprintf("process keycode entered\n");
}

/*+
 * ============================================================================
 * = reset_term - reset console terminal to default parameters                =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Refer to SRM.
 *
 * FORM OF CALL:
 *  
 * RETURNS:
 *
 * ARGUMENTS:
 *
 * SIDE EFFECTS:
 *
-*/

/*+
 * ============================================================================
 * = set_term_int - set console terminal interrupts                           =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Refer to SRM.
 *  
 * FORM OF CALL:
 *  
 * RETURNS:
 *
 * ARGUMENTS:
 *
 * SIDE EFFECTS:
 *
-*/
void cb_set_term_int(struct impure *IMPURE) {
    int tie = 0;
    int rie = 0;
    struct FILE *ttfp;
    struct TTPB *ttpb;
    struct CTB_WS *ctb_ws  = (struct CTB_WS *)0;

    int error_status = 0;

    int unit = IMPURE->cns$gpr[0][2*17];
    int rx_req = IMPURE->cns$gpr[0][2*18] & 0xff;
    int tx_req = (IMPURE->cns$gpr[0][2*18] >> 8) & 0xff;

#if CALLBACK_DEBUG
    if (callback_debug_flag & 8)
	qprintf("cb_set_term_int: unit %x, rx_req %x tx_req %x\n",
	  unit, rx_req, tx_req);
#endif

    if (!graphics_console)
    {
	if (!unit)
	{
	    /*	 
	    **  pick up existing value of interrupt enables.
	    */	 
	    ctb_ws = (int)hwrpb + *hwrpb->CTB_OFFSET;
	    tie = ctb_ws->CTB_WS$R_TX_INT.TX$V_IE;
	    rie = ctb_ws->CTB_WS$R_RX_INT.RX$V_IE;
	}
    }

    ttfp = cb_get_ttfp (unit);

#if CALLBACK_DEBUG
    if (callback_debug_flag & 8)
	qprintf("cb_set_term_int: ttfp %x\n", ttfp);
#endif

    /*
     * If the fp is valid, accept the new interrupt enable values.
     */

    /* set up for the exit code */

    tie = (*sl_icr_copy_ptr)[unit*2];
    rie = (*sl_icr_copy_ptr)[(unit*2)+1];
    IMPURE->cns$gpr[0][2*0] = (rie << 1) + (tie << 0);
    IMPURE->cns$gpr[0][(2*0)+1] = 0;

    if (unit <= 3)
    {
	switch (tx_req)
	{
	    case 0:
		tie = 0;
		break;
	    case 1:
		tie = (*sl_icr_copy_ptr)[unit*2];
		break;
	    case 2:
	    case 3:
		tie = 1;
		break;
	    default:
		error_status = 0x80000000;
	}

	switch (rx_req)
	{
	    case 0:
		rie = 0;
		break;
	    case 1:
		rie = (*sl_icr_copy_ptr)[(unit*2)+1];
		break;
	    case 2:
	    case 3:
		rie = 1;
		break;
	    default:
		error_status = 0x80000000;
	}
    } 
    else
    {
	ctb_ws = 0;
	tie = 0;
	rie = 0;
	error_status = 0x80000000;
    }


    (*sl_icr_copy_ptr)[unit*2] = tie;
    (*sl_icr_copy_ptr)[(unit*2)+1] = rie;


    if (ttfp)
    {
	ttpb = ttfp->ip->misc;

	if (tie)
	{
	    ttpb->txon (ttpb->port);
	}
	else
	{
	    ttpb->txoff (ttpb->port);
	}
	if (rie)
	{
	    ttpb->rxon (ttpb->port);

#if TURBO
	    if (graphics_console) {
		mouse_rxon();
	    }
#endif

	}
	else
	{
	    ttpb->rxoff (ttpb->port);
	}

	if (ctb_ws)
	{
	    ctb_ws->CTB_WS$R_TX_INT.TX$V_IE = tie;
	    ctb_ws->CTB_WS$R_RX_INT.RX$V_IE = rie;
	}
    }
    else
    {

#if TURBO
	if (unit == 2)
	{
	    kbd_rxon();
	}	
	if (unit == 3)
	{
	    mouse_rxon();
	}
#endif

#if !(MEDULLA || CORTEX || YUKONA || TURBO)
	if (unit == 2)
	{
	    io_enable_interrupts (0, KEYBD_VECTOR);
	}	
	if (unit == 3)
	{
	    io_enable_interrupts (0, MOUSE_VECTOR);
	}
#endif
	IMPURE->cns$gpr[0][2*0] = 0;
    }
    IMPURE->cns$gpr[0][(2*0)+1] = error_status;
}

/*+
 * ============================================================================
 * = set_term_ctl - set console terminal controls                             =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *     This routine if successful, changes the line parameters on a
 *     terminal device.
 *     The line parameters are specifed by the "CTB_WS$Q_LINE_PARAM" field
 *     of CTB_WS.
 *     
 *     Currently this code can change line params on workstation 
 *     systems (console terminal device type 4).
 *     VMS driver has following restrictions:
 *	VMS allows the chage only if "console" is set to "graphics"
 *	The line parameters change can occur only on tty0 (ser_ttpb)
 *   
 *     If line parameters is successfully changed then upon return R0[63] is
 *     set to 0. On error R0[63] is set to '1' and R0[31:0] set to the
 *     offset of 'line_param' in CTB.
 *
 * FORM OF CALL:
 *  
 *   cb_set_term_ctl(struct impure *IMPURE)
 *  
 * RETURNS:
 *		    on success
 *		R0[63]=0
 *		    on error
 *		R0[63]=1 
 *		R0[31:0] = (offset of 'line_param') in CTB
 *
 * ARGUMENTS:
 *                 R17 = terminal device unit number (always 1 for device type 4)
 *		   R18 = virtual address of CTB
-*/

void cb_set_term_ctl(struct impure *IMPURE) {
/*
    struct FILE *ttfp;
    struct TTPB *ttpb;
*/
    struct TTPB *ser_ttpb;
    struct CTB_WS *ctb_ws  = (struct CTB_WS *)0;

    int error_status = 0;
    int	unit;
    int	line_params;		

   		 /* set up R0 for  'no error' exit code */
    IMPURE->cns$gpr[0][2*0] = 0;
    IMPURE->cns$gpr[1][2*0] = 0;


    unit = IMPURE->cns$gpr[0][2*17];	/* unit is always=1 (tty0) for Device Type 4 */


    /*
     * processing only Console Terminal Device Type 4 (when console is set to graphics)
     * 
     */

    ctb_ws = (struct CTB_WS *) ((int)hwrpb + *hwrpb->CTB_OFFSET); 

    line_params = ctb_ws->CTB_WS$Q_LINE_PARAM[0];	/* get parity, stop bits ,char size and baud*/


/*    qprintf("cb_set_term_ctl: unit: 0x%x, CTB PHAdr: 0x%x, line_param 0x%x\n", unit, ctb_ws, line_params);*/

#if MODULAR
      ser_ttpb = serial_ttpbs;
#else
      ser_ttpb = &serial_ttpb;
#endif

    if (ctb_ws->CTB_WS$Q_TYPE[0] == 4)			/* workstations type */
     {
      error_status = ser_ttpb->lineset(0,line_params);  /* only COM1 (tta0) can be changed*/
     /* qprintf("cb_set_term_ctl: error_status 0x%x\n", error_status);*/
/*      qprintf("cb_set_term_ctl: offset 0x%x\n", (&ctb_ws->CTB_WS$Q_LINE_PARAM[0] - &hwrpb->CTB_OFFSET));*/

      if (error_status != 0)
       {
	/* return error in R0 */
	IMPURE->cns$gpr[0][2*0] = (&ctb_ws->CTB_WS$Q_LINE_PARAM[0] - &hwrpb->CTB_OFFSET);
	IMPURE->cns$gpr[1][2*0] = 0x80000000;
       }	
     }
}

/*+
 * ============================================================================
 * = save_term - save console terminal state                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Refer to SRM.
 *  
 * FORM OF CALL:
 *  
 * RETURNS:
 *
 * ARGUMENTS:
 *
 * SIDE EFFECTS:
 *
-*/

/*+
 * ============================================================================
 * = restore_term - restore console terminal state                            =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Refer to SRM.
 *  
 * FORM OF CALL:
 *  
 * RETURNS:
 *
 * ARGUMENTS:
 *
 * SIDE EFFECTS:
 *
-*/

/*+
 * ============================================================================
 * = open - open I/O device for access                                        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Refer to SRM.
 *  
 * FORM OF CALL:
 *  
 * RETURNS:
 *
 * ARGUMENTS:
 *
 * SIDE EFFECTS:
 *
-*/
void cb_open(struct impure *IMPURE) {
    uint64 src;
    int *p;
    int i;
    struct FILE *fp;
    struct FILE *vmemfp;
    char dname[EV_VALUE_LEN];
    char fname[MAX_NAME*2];
    char *c;
    char *ct;
    struct INODE *inode;
    struct DDB *ddb;
    struct SLOT *slot;
    struct pb *pb;

    /*
     * Open up the virtual memory driver
     */
    vmemfp = fopen ("vmem", "sr");

    /*
     *  Copy over the device string.
     */
    src = *(uint64 *)&IMPURE->cns$gpr[0][2*17];
    i = IMPURE->cns$gpr[0][2*18];
    fseek (vmemfp, (uint64) src, SEEK_SET); 
    fread (dname, 1, i, vmemfp);
    fclose (vmemfp);
    dname[i] = 0;

#if FIBRE_CHANNEL
    srm_long2short( dname, dname );
#endif

#if !( STARTSHUT || DRIVERSHUT )
    dev2file_pb( dname, fname, &pb );
    /*
     *  Restart drivers if necessary.
     */
    if( cb_ref == 0 )
	{
	driver_setmode( DDB$K_STOP, 0 );
	reset_hardware( 0 );
	krn$_set_console_mode( INTERRUPT_MODE );
#if !CORTEX && !YUKONA
	if( !pb || !pb->setmode )
#endif
	    {
	    driver_setmode( DDB$K_START, 0 );
	    tt_setmode( DDB$K_STOP );
	    }
	}
#if !CORTEX && !YUKONA
    if( pb && pb->setmode )
	pb->setmode( pb, DDB$K_START );
#endif

#else
    dev2file( dname, fname );
#endif

#if TL6
    write_leds(0x01);
    slot = boot_cpu_slot;
    if (slot->STATE.SLOT$V_BIP == 0)
	pprintf("\n%%%%%%%% %s, %s\n", dname, fname);
#endif

#if CALLBACK_DEBUG
    if (callback_debug_flag & 1)
	pprintf("cb_open: %s, %s\n", dname, fname);
#endif

#if ( STARTSHUT || DRIVERSHUT )
    create_device(fname);	/* In case we're not opening the boot device */
#endif

    /*
     *  Pick off the optional stuff past the end of the standard SRM stuff.
     */
    c = dname;
    for( i = 0; i < 8 && c; i++ )
	c = strchr( ++c, ' ' );
    if( c )
	{
	/*
	 *  We've got something, so see if we're opening a network device,
	 *  and if so, if that network device supports an additional
	 *  station address to receive messages on.
	 */
	if( allocinode( fname, 0, &inode ) == 1 )
	    {
	    ddb = inode->dva;
	    if( ddb->net_device && ddb->allow_info )
		{
		/*
		 *  Keep only one field.
		 */
		ct = strchr( ++c, ' ' );
		if( ct )
		    *ct = 0;
		/*
		 *  If it fits, add on the optional field the OS supplied.
		 */
		if( strlen( c ) <= MAX_NAME )
		    {
		    strcat( fname, ":" );
		    strcat( fname, c );
#if CALLBACK_DEBUG
		    if (callback_debug_flag & 1)
			pprintf("cb_open, new fname: %s\n", fname);
#endif
		    }
		}
	    INODE_UNLOCK( inode );
	    }
	}
	
    /*
     *  Try the open a few times.  
     *  Consider the case of the KGPSA, where the kgpsa_poll process creates
     *  the inode at callback time.   The kgpsa_poll process may not
     *  have completed and created the inode for the device we are about
     *  to fopen, so allow enough time and retries for the device to be found.
     */
#if ( STARTSHUT || DRIVERSHUT )
    for( i = 0; i < 10; i++ )
#else
    for( i = 0; i < 4; i++ )
#endif
	{
	fp = fopen( fname, "psr" );
	if( fp )
	    break;
	krn$_sleep( 500 );
	}
    if( !fp ) {
	pprintf("\ncb_open: failed %s, %s\n", dname, fname);
	IMPURE->cns$gpr[0][2*0] = 0;
	IMPURE->cns$gpr[0][2*0+1] = 0xc0000000;
	if( cb_ref == 0 ) {
#if !( STARTSHUT || DRIVERSHUT )
	    driver_setmode( DDB$K_STOP, 0 );
#endif
	    krn$_set_console_mode( POLLED_MODE );
	}
    } else if( cb_ref >= 16 ) {
	IMPURE->cns$gpr[0][2*0] = 0;
	IMPURE->cns$gpr[0][2*0+1] = 0xc0000000;
	fclose( fp );
    } else {
	IMPURE->cns$gpr[0][2*0] = (int)fp;
	IMPURE->cns$gpr[0][2*0+1] = 0;
	cb_fp[cb_ref++] = fp;
    }
    tt_check_controlp( );

#if TL6
    write_leds(0x02);
#endif
}

 
/*+
 * ============================================================================
 * = close - close I/O device for access                                      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Refer to SRM.
 *
 * FORM OF CALL:
 *  
 * RETURNS:
 *
 * ARGUMENTS:
 *
 * SIDE EFFECTS:
 *
-*/
void cb_close(struct impure *IMPURE) {
    int i, j;
    struct FILE *fp;

#if TL6
    write_leds(0x03);
#endif
#if CALLBACK_DEBUG
    if (callback_debug_flag & 1)
	pprintf("cb_close\n");
#endif

    fp = (struct FILE *)(IMPURE->cns$gpr[0][2*17]);
    if (fclose(fp) != msg_success) {
	IMPURE->cns$gpr[0][2*0] = 0;
	IMPURE->cns$gpr[0][2*0+1] = 0x80000000;
    }
    else  {
	IMPURE->cns$gpr[0][2*0] = 0;
	IMPURE->cns$gpr[0][2*0+1] = 0;
    }	
    /*
     *  Stop drivers if necessary.
     */
    for( i = j = 0; i < cb_ref; i++ )
	if( cb_fp[i] != fp )
	    cb_fp[j++] = cb_fp[i];
    --cb_ref;
#if !( STARTSHUT || DRIVERSHUT )
    if( cb_ref == 0 )
	{
	driver_setmode( DDB$K_STOP, 0 );
	krn$_set_console_mode( POLLED_MODE );
	}
#endif
#if ( STARTSHUT || DRIVERSHUT )

    /*
     * On an open callback the console will be put into interrupt mode
     * by the port driver when the fopen is done. The console remains
     * in interrupt mode until the file is closed via a close callback.
     */

    krn$_set_console_mode(POLLED_MODE);
#endif
    tt_check_controlp( );
#if TL6
    write_leds(0x04);
#endif
}

/*+
 * ============================================================================
 * = ioctl - perform device-specific I/O operations on a device               =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Refer to SRM.
 * 
 * FORM OF CALL:
 *  
 * RETURNS:
 *
 * ARGUMENTS:
 *
 * SIDE EFFECTS:
 *
-*/
void cb_ioctl(struct impure *IMPURE) {

    uint64 dst;
    struct FILE *fp;
    struct FILE *vmemfp;
    struct INODE *ip;
    struct DDB *ddb;
    struct NI_GBL *ni_gbl;
    struct NI_DL *ni_dl;
    struct ub *ub;
    int size, opcode;
    unsigned char *buf;
    int i, k, l;

	
    vmemfp = fopen ("vmem", "sw");
    fp = (struct FILE *)(IMPURE->cns$gpr[0][2*17]);
    ip = fp->ip;
    ddb = ip->dva;

    opcode = IMPURE->cns$gpr[0][2*18];
    dst = *(uint64 *)&IMPURE->cns$gpr[0][2*19];
    size = IMPURE->cns$gpr[0][2*20];
    l = 0;

    if ( ddb->net_device && ddb->allow_info )    /* If network device */
        {
        switch (opcode)
            {

            case 1 :    /* get station address */

            if (ip->misc) 
                {
                ni_gbl = ip->misc;
                ni_dl = ni_gbl->dlp;

                if (size >= 6) 
                    {
                    fseek (vmemfp, (uint64) dst, SEEK_SET);
                    l = fwrite (ni_dl->sa, 1, 6, vmemfp);
                    }
                else pprintf("cb_ioctl: buffer is too small\n");
                }
            break;


            default:
            pprintf("cb_ioctl: case %d is reserved.\n", opcode);
            break;
      
            }

        }    /* IF network device */


#if MODULAR
    if ( (ddb == dk_ddb_ptr) || (ddb==mk_ddb_ptr) ) 
#else
    if ( (ddb == &dk_ddb) || (ddb == &mk_ddb) ) 
#endif
        {
        switch (opcode)
            {
            case 1 :   
	    case 2 :
	    case 3 :
	    case 4 :
	       break;

	    case 5 :
               if (ip->misc) 
                 {
  	          ub = ip->misc;
		  if( ub->allocation_class != 0x80000000 ) {
                    fseek (vmemfp, (uint64) dst, SEEK_SET);
                    l = fwrite (&ub->allocation_class, 4, 1, vmemfp);
		  }
	         } else {
		  pprintf("ip->misc is zero\n");
		 }
               break;

	    case 6 :
               if (ip->misc) 
                 {
  	          ub = ip->misc;
		  if( ub->multibus != 0x80000000) {
                    fseek (vmemfp, (uint64) dst, SEEK_SET);
                    l = fwrite (&ub->multibus, 4, 1, vmemfp);
		  }
	         } else {
		  pprintf("ip->misc is zero\n");
		 }
               break;

            default:
	       break;

            }

        }  /* if mk_ddb or dk_ddb */


    /* Write is complete now return the correct status */
    if( l )
       {
       IMPURE->cns$gpr[0][2*0] = l;
       IMPURE->cns$gpr[0][2*0+1] = 0;
       }
    else
       {
       IMPURE->cns$gpr[0][2*0] = l;
       IMPURE->cns$gpr[0][2*0+1] = 0x80000000;
       }     


    fclose (vmemfp);

}

/*+
 * ============================================================================
 * = read - read I/O device                                                   =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Refer to SRM.
 *
 * FORM OF CALL:
 *  
 * RETURNS:
 *
 * ARGUMENTS:
 *
 * SIDE EFFECTS:
 *
-*/
void cb_read(struct impure *IMPURE) {
    uint64 dst;
    struct FILE *fp;
    struct FILE *vmemfp;
    int lbn, size;
    unsigned char *buf;
    int i, k, l;
	
#if TL6
    write_leds(0x05);
#endif
    vmemfp = fopen ("vmem", "sw");
    fp = (struct FILE *)(IMPURE->cns$gpr[0][2*17]);
    size = IMPURE->cns$gpr[0][2*18];
    dst = *(uint64 *)&IMPURE->cns$gpr[0][2*19];
    lbn = IMPURE->cns$gpr[0][2*20];

#if CALLBACK_DEBUG
    if (callback_debug_flag & 2)
	pprintf("cb_read: dst = %016.16x, lbn = %d, size = %d\n",
	  dst, lbn, size);
#endif
#if DEBUG
        pprintf("cb_read, size:%d  lbn:%d  dst:%x  fp:%x\n", size, lbn, dst,fp);
	pprintf("cb_read, fp name:%s\n", fp->ip->name);
#endif

    /*
     *  Get a temporary buffer to do the I/O into.
     */
    buf = malloc_noown( min( size, MAX_IO_SIZE ) );

    /*
     * Read until the entire transfer is complete.
     */
    fseek( fp, (uint64) lbn * 512, SEEK_SET );
    i = size;
    size = 0;    
    while( i > 0 )
	{
	k = min( i, MAX_IO_SIZE );
#if DEBUG
    pprintf("call cb read driver routine, size:%d\n", k);
#endif
#if TL6
    write_leds(0x06);
#endif
	l = fread( buf, 1, k, fp );
#if TL6
    write_leds(0x07);
#endif
    	fseek (vmemfp, (uint64) dst, SEEK_SET);
    	fwrite (buf, 1, min(k,l), vmemfp);
#if TL6
    write_leds(0x08);
#endif
    	size += l;
	if(l != k)
	    break;
  	i -= k;
	dst += k;
	}

    /*
     * Read is complete now return the correct status
     */
     if( l )
	{
	IMPURE->cns$gpr[0][2*0] = size;
	IMPURE->cns$gpr[0][2*0+1] = 0;
	}
     else
	{
	IMPURE->cns$gpr[0][2*0] = size;
	IMPURE->cns$gpr[0][2*0+1] = 0x80000000;
	}     

    /*
     * Do some cleanup then return.
     */
    free( buf );
    fclose (vmemfp);
#if CALLBACK_DEBUG
    if (callback_debug_flag & 2)
	pprintf("cb_read exit: size = %x(%d)\n", size, size);
#endif
#if DEBUG
    pprintf("cb_read exit, :%d\n", l);
#endif
    tt_check_controlp( );

#if TL6
    write_leds(0x09);
#endif
}

/*+
 * ============================================================================
 * = write - write I/O device                                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Refer to SRM.
 *  
 * FORM OF CALL:
 *  
 * RETURNS:
 *
 * ARGUMENTS:
 *
 * SIDE EFFECTS:
 *
-*/
void cb_write(struct impure *IMPURE) {
    uint64 src;
    struct FILE *fp;
    struct FILE *vmemfp;
    int lbn, size;
    unsigned char *buf;
    int i, k;

#if TL6
    write_leds(0x10);
#endif
    vmemfp = fopen ("vmem", "sr");
    fp = (struct FILE *)(IMPURE->cns$gpr[0][2*17]);
    size = IMPURE->cns$gpr[0][2*18];
    src = *(uint64 *)&IMPURE->cns$gpr[0][2*19];
    lbn = IMPURE->cns$gpr[0][2*20];

    /*
     *  Get a temporary buffer to do the I/O into.
     */
    buf = malloc_noown( min( size, MAX_IO_SIZE ) );

    fseek( fp, (uint64) lbn * 512, SEEK_SET );
    i = size;

#if CALLBACK_DEBUG
    if (callback_debug_flag & 2)
	pprintf("cb_write: src = %016.16x, lbn = %d, size = %d\n",
	  src, lbn, size);
#endif
#if DEBUG
    pprintf("cb_write, size:%d  fp:%x  lbn:%d  src:%x\n", size, fp, lbn, src);
    pprintf("cb_write, fp name:%s\n", fp->ip->name);
#endif
    while( i > 0 )
	{
	k = min( i, MAX_IO_SIZE );
	fseek (vmemfp, (uint64) src, SEEK_SET);
	fread (buf, 1, k, vmemfp);
#if TL6
    write_leds(0x11);
#endif
	if( fwrite( buf, 1, k, fp ) != k )
	    {
#if TL6
    write_leds(0x12);
#endif
	    IMPURE->cns$gpr[0][2*0] = size - i;
	    IMPURE->cns$gpr[0][2*0+1] = 0x80000000;
	    free( buf );
	    fclose (vmemfp);
	    return;
	    }
#if TL6
    write_leds(0x13);
#endif
	i -= k;
	src += k;
	}
    IMPURE->cns$gpr[0][2*0] = size;
    IMPURE->cns$gpr[0][2*0+1] = 0;
    free( buf );
    fclose (vmemfp);
#if DEBUG
    pprintf("cb_write exit\n");
#endif
    tt_check_controlp( );
#if TL6
    write_leds(0x14);
#endif
}

/*+
 *============================================================================
 * = setenv - sets an environment variable                                   =
 *============================================================================
 * 
 * OVERVIEW:
 * 
 *	Refer to SRM.
 *
 * FORM OF CALL:
 *  
 * RETURNS:
 *
 * ARGUMENTS:
 *
 * SIDE EFFECTS:
 *
-*/
void cb_set_env( struct impure *IMPURE )
    {
    char name[EV_NAME_LEN+1];
    char value[EV_VALUE_LEN+1];
    int size;
    int n;
    uint64 va;
    struct FILE *fp;
    char first_arg[EV_VALUE_LEN+1];
    char sec_arg[EV_VALUE_LEN+1];
    int status;                     /*JS no compile without*/

    fp = fopen( "vmem", "sr" );

/* For the security related callback EVs, the callback macro passes control
 * here to handle EV IDs 0x4c,0x4d,0x4e. Since implementing password encryption
 * in the macro was too time consuming , I am using the available security
 * functions in secure.c . For these 3 EVs, r17 will contain the EV IDs in hex
 * and not as a pointer to a EV string. - Sri Dhati.
 */
#if SECURE

        if ( IMPURE->cns$gpr[0][2*17] == 0x4c )
        {
            va = *(uint64 *)&IMPURE->cns$gpr[0][2*18];
            fseek( fp, va, SEEK_SET );
            fread( value, 1, EV_VALUE_LEN, fp );
            strcpy(&first_arg[0],&value[0]);
            strcpy(&sec_arg[0],&value[strlen(first_arg)+1]);
            if (!valid_save_pwd (first_arg ,sec_arg , 1 ) )
                {
                      loginfail = 0;
                      IMPURE->cns$gpr[0][2*0+1] = 0x00000000; /* success */
                }
            else
                {
                      if (loginfail < 9) /* peg loginfail at 9 */
                         loginfail += 1;
                      IMPURE->cns$gpr[0][2*0+1] = 0x80000000;
                      IMPURE->cns$gpr[0][2*0] = 0x00000000;
                }
            fclose(fp);
            return;
        }

        if ( IMPURE->cns$gpr[0][2*17] == 0x4d )
           {
                va = *(uint64 *)&IMPURE->cns$gpr[0][2*18];
                fseek( fp, va, SEEK_SET );
                fread( value, 1, EV_VALUE_LEN, fp );
                strcpy(&first_arg[0],&value[0]);
                strcpy(&sec_arg[0],&value[strlen(first_arg)+1]);
                status = valid_save_pwd (&first_arg[0],"\0",0);
                if (strcmp (&sec_arg[0],"on")&& (status != msg_no_vldpsswd))
                   {
                        secure = 1 ;
                        IMPURE->cns$gpr[0][2*0] = 0x00000000;
                        IMPURE->cns$gpr[0][2*0+1] = 0x00000000;
                   }
                else if ( strcmp (&sec_arg[0],"off") && (status == msg_success))
                        {
                              secure = 0;
                              loginfail = 0;
                              IMPURE->cns$gpr[0][2*0] = 0x00000000;
                              IMPURE->cns$gpr[0][2*0+1] = 0x00000000;
                        }
                     else
                        {
                              if (loginfail < 9 ) /* peg loginfail at 9 */
                                loginfail += 1;
                              IMPURE->cns$gpr[0][2*0+1] = 0x80000000;
                              IMPURE->cns$gpr[0][2*0] = 0x00000000;
                        }
              fclose(fp);
              return;
           }
#endif      /* endif for SECURE */

    /*
     *  Copy over the EV name string.
     */
    va = *(uint64 *)&IMPURE->cns$gpr[0][2*17];
    fseek( fp, va, SEEK_SET );
    n = fread( name, 1, EV_NAME_LEN, fp );
    if( n )
	{
	name[n] = 0;
	/*
	 *  Write the EV.
	 */
	va = *(uint64 *)&IMPURE->cns$gpr[0][2*18];
	fseek( fp, va, SEEK_SET );
	fread( value, 1, EV_VALUE_LEN, fp );
#if CALLBACK_DEBUG
	if (callback_debug_flag & 4)
	    pprintf("set_env: %s, %s\n", name, value);
#endif
	if( ev_write( name, value, EV$K_SYSTEM ) == msg_success )
	    {
	    size = *(uint64 *)&IMPURE->cns$gpr[0][2*19];
	    if( size > EV_VALUE_LEN )
		{
		size = EV_VALUE_LEN;
		IMPURE->cns$gpr[0][2*0+1] = 0xe0000000;
		}
	    else
		IMPURE->cns$gpr[0][2*0+1] = 0x00000000;
	    IMPURE->cns$gpr[0][2*0] = size;
	    }
	else
	    {
	    IMPURE->cns$gpr[0][2*0] = 0;
	    IMPURE->cns$gpr[0][2*0+1] = 0xc0000000;
	    }
	}
    else
	{
	IMPURE->cns$gpr[0][2*0] = 0;
	IMPURE->cns$gpr[0][2*0+1] = 0xc0000000;
	}
    fclose( fp );
    }

/*+
 * ============================================================================
 * = clearenv - delete an environment variable from the namespace             =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Refer to SRM.
 *  
 * FORM OF CALL:
 *  
 * RETURNS:
 *
 * ARGUMENTS:
 *
 * SIDE EFFECTS:
 *
-*/

/*+
 * ============================================================================
 * = resetenv - delete an environment variable from the namespace             =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      Refer to SRM.
 *  
 * FORM OF CALL:
 *  
 * RETURNS:
 *
 * ARGUMENTS:
 *
 * SIDE EFFECTS:
 *
-*/
#if SECURE
void cb_reset_env( struct impure *IMPURE )
    {
    int size;
    int n;
    uint64 va;
    struct FILE *fp;
    char name[EV_NAME_LEN+1];
    char value[EV_VALUE_LEN+1];
    char first_arg[EV_VALUE_LEN+1];
    char sec_arg[EV_VALUE_LEN+1];

    fp = fopen( "vmem", "sr" );

/* For the security related callback EVs, the callback macro passes control
 * here to handle EV IDs 0x4c,0x4d,0x4e. Since implementing password encryption
 * in the macro was too time consuming , I am using the available security
 * functions in secure.c . For these 3 EVs, r17 will contain the EV IDs in hex
 * and not as a pointer to a EV string. - Sri Dhati.
 */

        if ( IMPURE->cns$gpr[0][2*17] == 0x4c )
        {
            va = *(uint64 *)&IMPURE->cns$gpr[0][2*18];
            fseek( fp, va, SEEK_SET );
            fread( value, 1, EV_VALUE_LEN, fp );
            strcpy(&first_arg[0],&value[0]);
            strcpy(&sec_arg[0],"\0");
            if (!valid_save_pwd (&first_arg[0] ,&sec_arg[0] , 1 ) )
                {
                      loginfail = 0;
                      IMPURE->cns$gpr[0][2*0+1] = 0x00000000; /* success */
                }
            else
                {
                      if (loginfail < 9) /* peg loginfail at 9 */
                         loginfail += 1;
                      IMPURE->cns$gpr[0][2*0+1] = 0x80000000;
                      IMPURE->cns$gpr[0][2*0] = 0x00000000;
                }
            fclose(fp);
            return;
        }

        if ( IMPURE->cns$gpr[0][2*17] == 0x4d )
           {
                va = *(uint64 *)&IMPURE->cns$gpr[0][2*18];
                fseek( fp, va, SEEK_SET );
                fread( value, 1, EV_VALUE_LEN, fp );
                strcpy(&first_arg[0],&value[0]);
                strcpy(&sec_arg[0],"\0");
                if (! valid_save_pwd (&first_arg[0],&sec_arg[0],0) )
                   {
                        secure = 0 ;
                        loginfail = 0;
                        IMPURE->cns$gpr[0][2*0] = 0x00000000;
                        IMPURE->cns$gpr[0][2*0+1] = 0x00000000;
                   }
                else 
                        {
                              if (loginfail < 9 ) /* peg loginfail at 9 */
                                loginfail += 1;
                              IMPURE->cns$gpr[0][2*0+1] = 0x80000000;
                              IMPURE->cns$gpr[0][2*0] = 0x00000000;
                        }
              fclose(fp);
              return;
           }
        if ( IMPURE->cns$gpr[0][2*17] == 0x4e )
           {
                va = *(uint64 *)&IMPURE->cns$gpr[0][2*18];
                fseek( fp, va, SEEK_SET );
                fread( value, 1, EV_VALUE_LEN, fp );
                strcpy(&first_arg[0],&value[0]);
                strcpy(&sec_arg[0],"\0");
                if (! valid_save_pwd (&first_arg[0],&sec_arg[0],0) )
                   {
                        loginfail = 0;
                        IMPURE->cns$gpr[0][2*0] = 0x00000000;
                        IMPURE->cns$gpr[0][2*0+1] = 0x00000000;
                   }
                else 
                        {
                              if (loginfail < 9 ) /* peg loginfail at 9 */
                                loginfail += 1;
                              IMPURE->cns$gpr[0][2*0+1] = 0x80000000;
                              IMPURE->cns$gpr[0][2*0] = 0x00000000;
                        }
              fclose(fp);
              return;
           }
    /*
     *  Copy over the EV name string.
     */
    va = *(uint64 *)&IMPURE->cns$gpr[0][2*17];
    fseek( fp, va, SEEK_SET );
    n = fread( name, 1, EV_NAME_LEN, fp );
    if( n )
        {
        name[n] = 0;
        /*
         *  Write the EV.
         */
        va = *(uint64 *)&IMPURE->cns$gpr[0][2*18];
        fseek( fp, va, SEEK_SET );
        fread( value, 1, EV_VALUE_LEN, fp );
#if CALLBACK_DEBUG
        if (callback_debug_flag & 4)
            pprintf("set_env: %s, %s\n", name, value);
#endif
        if( ev_write( name, value, EV$K_SYSTEM ) == msg_success )
            {
            size = *(uint64 *)&IMPURE->cns$gpr[0][2*19];
            if( size > EV_VALUE_LEN )
                {
                size = EV_VALUE_LEN;
                IMPURE->cns$gpr[0][2*0+1] = 0xe0000000;
                }
            else
                IMPURE->cns$gpr[0][2*0+1] = 0x00000000;
            IMPURE->cns$gpr[0][2*0] = size;
            }
        else
            {
            IMPURE->cns$gpr[0][2*0] = 0;
            IMPURE->cns$gpr[0][2*0+1] = 0xc0000000;
            }
        }
    else
        {
        IMPURE->cns$gpr[0][2*0] = 0;
        IMPURE->cns$gpr[0][2*0+1] = 0xc0000000;
        }
    fclose( fp );
    }
#endif    /*  endif for SECURE */


/*+
 * ============================================================================
 * = getenv - gets an environment variable and writes it to memory            =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Refer to SRM.
 *  
 * FORM OF CALL:
 *  
 * RETURNS:
 *
 * ARGUMENTS:
 *
 * SIDE EFFECTS:
 *
-*/
void cb_get_env( struct impure *IMPURE )
    {
    char name[EV_NAME_LEN+1];
    int size;
    int n;
    int count;   
    uint64 va;
    struct FILE *fp;
    struct EVNODE *ev;
    char first_arg[EV_VALUE_LEN+1];
    char sec_arg[EV_VALUE_LEN+1];

    fp = fopen( "vmem", "sr" );
/* Below we check for 2 special conditions , secure (0x4d) and loginfail (0x4e)
 * both of which are not really a EV. It has been decided not to change the
 * them to an EV but use as is in srm - Sri Dhati
 */
#if SECURE

    if ( IMPURE->cns$gpr[0][2*17] == 0x4d )
      {

        va = *(uint64 *)&IMPURE->cns$gpr[0][2*18];
        fseek( fp, va, SEEK_SET );
        if (secure)
            fwrite( "on", 1, 2, fp );
        else
            fwrite( "off", 1, 3, fp );
        IMPURE->cns$gpr[0][2*0]  = (secure) ? 0x2 : 0x3;  /* len could be 2(on)/3(off)*/
        IMPURE->cns$gpr[0][2*0+1]  = 0x00000000;
        fclose(fp);
        return;
      }

    if ( IMPURE->cns$gpr[0][2*17] == 0x4e )
      {
        va = *(uint64 *)&IMPURE->cns$gpr[0][2*18];
        fseek( fp, va, SEEK_SET );
        count = loginfail+'0';          /* convert number to equiv char */
        fwrite( &count, 1, 1, fp );
        IMPURE->cns$gpr[0][2*0] = 0x1;         /* just one character */
        IMPURE->cns$gpr[0][2*0+1] = 0x00000000;
        fclose(fp);
        return;
      }

#endif     /* end for secure */
    /*
     *  Copy over the EV name string.
     */
    va = *(uint64 *)&IMPURE->cns$gpr[0][2*17];
    fseek( fp, va, SEEK_SET );
    n = fread( name, 1, EV_NAME_LEN, fp );
    if( n )
	{
	name[n] = 0;
	/*
	 *  Read the EV.
	 */
	ev = malloc_noown( sizeof( *ev ) );
	if( ev_read( name, &ev, EV$K_SYSTEM ) == msg_success )
	    {
#if CALLBACK_DEBUG
	    if (callback_debug_flag & 4)
		pprintf("get_env: %s, %s\n", name, &ev->value);
#endif
	    size = *(uint64 *)&IMPURE->cns$gpr[0][2*19];
	    if( size >= ev->size )
		{
		size = ev->size;
		IMPURE->cns$gpr[0][2*0+1] = 0x00000000;
		}
	    else
		IMPURE->cns$gpr[0][2*0+1] = 0x20000000;
	    va = *(uint64 *)&IMPURE->cns$gpr[0][2*18];
	    fseek( fp, va, SEEK_SET );
	    fwrite( &ev->value, 1, size, fp );
	    IMPURE->cns$gpr[0][2*0] = ev->size;
	    }
	else
	    {
#if CALLBACK_DEBUG
	    if (callback_debug_flag & 4)
		pprintf("get_env: %s failed\n", name);
#endif
	    IMPURE->cns$gpr[0][2*0] = 0;
	    IMPURE->cns$gpr[0][2*0+1] = 0xc0000000;
	    }
	free( ev );
	}
    else
	{
	IMPURE->cns$gpr[0][2*0] = 0;
	IMPURE->cns$gpr[0][2*0+1] = 0xc0000000;
	}
    fclose( fp );
    }

/*+
 * ============================================================================
 * = saveenv - saves an environment variable.				      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Refer to SRM.
 *  
 * FORM OF CALL:
 *  
 * RETURNS:
 *
 * ARGUMENTS:
 *
 * SIDE EFFECTS:
 *
-*/

#if !TURBO
void cb_save_env( struct impure *IMPURE )
    {
    struct EVNODE *ev;
    struct env_table *et;

#if CALLBACK_DEBUG
    if (callback_debug_flag & 4)
	pprintf("save_env:\n");
#endif
#if LX164
    IMPURE->cns$gpr[0][2*0] = 0;
    IMPURE->cns$gpr[0][2*0+1] = 0xe0000000;
#else
    ev = malloc_noown( sizeof( *ev ) );
    for( et = env_table_ptr; et->ev_name ; et++ )
	if( ( ( et->ev_options & EV$M_SRM ) == EV$K_SRM_DEF )
		&& ( ( et->ev_options & EV$M_VOLATILE ) == EV$K_NONVOLATILE ) )
	    if( ev_read( et->ev_name, &ev, EV$K_SYSTEM ) == msg_success )
		if( ( ev->attributes & EV$M_MODIFIED ) == EV$K_MODIFIED )
		    if( ( ev->attributes & EV$M_DEFAULT ) == EV$K_DEF_STRING )
			ev_write( ev->name, ev->value.string, EV$K_SYSTEM );
		    else
			ev_write( ev->name, ev->value.integer, EV$K_SYSTEM );
    free( ev );
    IMPURE->cns$gpr[0][2*0] = 0;
    IMPURE->cns$gpr[0][2*0+1] = 0;
#endif
    }
#endif

#if TURBO
void cb_save_env( struct impure *IMPURE )
    {
    struct EVNODE *ev;
    struct env_table *et;
    struct FILE *fp;
    struct AREA_HEADER *area_ptr;
    struct EEPROM_S_DEF *eptr;

#if CALLBACK_DEBUG
    if (callback_debug_flag & 4)
	pprintf("save_env:\n");
#endif

    save_env_area_ptr = 0;

/* Make a copy of the eeprom */

    if (!(fp = eeprom_file_open("r+")))
	goto save_env_error;

    area_ptr = eeprom_find_area(eeprom_k_environ, fp);
    if (!area_ptr) {
	eeprom_file_close(fp, FALSE);
	goto save_env_error;
    }

    eptr = dyn$_malloc(sizeof(struct EEPROM_S_DEF), DYN$K_NOOWN | DYN$K_SYNC);
    memcpy(eptr, fp->ip->loc, sizeof(struct EEPROM_S_DEF));
    save_env_area_ptr = (struct AREA_HEADER *) ((int *) eptr + ((int *) area_ptr - (int *) fp->ip->loc));
    eeprom_file_close(fp, FALSE);

    ev = malloc_noown( sizeof( *ev ) );

    for( et = env_table_ptr; et->ev_name ; et++ ) {

	if( ( ( et->ev_options & EV$M_SRM ) == EV$K_SRM_DEF )
		&& ( ( et->ev_options & EV$M_VOLATILE ) == EV$K_NONVOLATILE ) )
	    if( ev_read( et->ev_name, &ev, EV$K_SYSTEM ) == msg_success )
		if( ( ev->attributes & EV$M_MODIFIED ) == EV$K_MODIFIED )
		    if( ( ev->attributes & EV$M_DEFAULT ) == EV$K_DEF_STRING )
			ev_write( ev->name, ev->value.string, EV$K_SYSTEM );
		    else
			ev_write( ev->name, ev->value.integer, EV$K_SYSTEM );
    }

    eeprom_cb_change_flag = 1;
    eeprom_update(eptr);		/* only do requesting cpu */

    save_env_area_ptr = 0;
    free(eptr);
    free( ev );
    IMPURE->cns$gpr[0][2*0] = 0;
    IMPURE->cns$gpr[0][2*0+1] = 0;
    return;

save_env_error:
    save_env_area_ptr = 0;
    IMPURE->cns$gpr[0][2*0] = 0;
    IMPURE->cns$gpr[0][2*0+1] = 0xe0000000;
}
#endif



/*+
 * ============================================================================
 * = cb_pswitch - switch the primary processor.				      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This callback allows the operating system to designate a new
 *	primary processor from among those marked as "primary eligible".
 *  
 * FORM OF CALL:
 *  
 *	status = DISPATCH( PSWITCH, action, id)
 *
 * RETURNS:
 *
 *	    0 for success,
 *	    R0<63:63> = 1 for error, with specific error in R0<62:0>
 *
 * ARGUMENTS:
 *
 *	    action<1:0> is either:
 *		'01' transition from primary
 *		'10' transition to primary
 *		'11' switch primary
 *
 *	    id is the CPU ID to transition to or from primary
 *
 * SIDE EFFECTS:
 *
 *	    Processes which had set affinity to the old processor
 *	    will be moved to the new primary.
 *
-*/

void cb_pswitch( struct impure *IMPURE )
    {
    int		    action, id;
    struct QUEUE    *p;
    struct PCB	    *pcb;
    struct SLOT	    *slot;
    int		    old_affinity;
    int		    new_affinity;

    action = IMPURE->cns$gpr[0][2*17];
    id = IMPURE->cns$gpr[0][2*18];

#if CALLBACK_DEBUG
    if (callback_debug_flag & 0x10)
	pprintf("cb_pswitch: %x, %x\n", action, id);
#endif

    if (id != whoami()) {		/* must run on cpu being changed */
	IMPURE->cns$gpr[0][2*0] = 0x87654321;
	IMPURE->cns$gpr[0][2*0+1] = 0x80000000;
	return;
    }

    slot = (void *)((int)hwrpb + *(uint64 *)hwrpb->SLOT_OFFSET + 
      (id * sizeof(struct SLOT)));

    if ((slot->CPU_VAR[0] & 4) == 0) {    /* must specify a primary eligible cpu */
	IMPURE->cns$gpr[0][2*0] = 0x87654320;
	IMPURE->cns$gpr[0][2*0+1] = 0x80000000;
	return;
    }

    if (action == 0x1) {    /* transition from primary */
	if (id != primary_cpu) {
	    IMPURE->cns$gpr[0][2*0] = 0x87654300;
	    IMPURE->cns$gpr[0][2*0+1] = 0x80000000;
	    return;
	}

	/* any cleanup necessary? */

    } else if (action == 0x2) {	/* transition to primary */

	/* change the primary_cpu & timer_cpu */
	old_affinity = (1 << primary_cpu);
	new_affinity = (1 << id);
	primary_cpu = id;
	timer_cpu = id;

#if RAWHIDE
	/* change PAL's idea of the primary */
	*((int *) PAL$PRIMARY) = id;
#endif
	/* move any processes with primary affinity */

	spinlock (&spl_kernel);	    /* lock the database */
	p = pcbq.flink;
	while (p != &pcbq) {
	    pcb = (struct PCB *) ((int) p - offsetof (struct PCB, pcb$r_pq));
	    if ((pcb->pcb$l_affinity == old_affinity) &&
		(strcmp(pcb->pcb$b_name, "idle") != 0) &&
		((strncmp(pcb->pcb$b_name, "shell", 5) != 0) ||
		 (pcb->pcb$l_pid == main_shell_pid))) {
		pcb->pcb$l_affinity = new_affinity;
	    }
	    p = p->flink;
	}
	spinunlock(&spl_kernel);

    } else {
	IMPURE->cns$gpr[0][2*0] = 0x87654000;
	IMPURE->cns$gpr[0][2*0+1] = 0x80000000;
	return;
    }

    IMPURE->cns$gpr[0][2*0] = 0;
    IMPURE->cns$gpr[0][2*0+1] = 0;
}

/*+
 * ============================================================================
 * = cb_GetPeer - returns handle to peer in configuration tree                =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      cb_GetPeer operates on the firmware configuration tree.  The routine 
 *	writes the CONFIGURATION_COMPONENT data of the peer (i.e. the sibling 
 *	node) of handle current into comp_buffer.  It writes the associated 
 *	identifier, if any, to ident_buffer.  The size of the 
 *	CONFIGURATION_COMPONENT is 36 bytes. This value should be written into 
 *	comp_size.  The recommended size for the ident buffer is 64 bytes.  
 *	This value should be written into ident_size.  A handle to the
 *	peer of current is returned in R0[31:0].  The number of bytes written 
 *	into each of the buffers, comp and ident, is returned in R0[39:32] and 
 *	R0[47:40], respectively.  R0[63:61] contain the status of the call. 
 *     
 *
 * FORM OF CALL:
 *
 *	void *out_handle cb_GetPeer(void *current, void *comp_buffer, 
 *				unsigned int comp_size, void *ident_buffer,
 *				unsigned int ident_size);
 *
 * RETURNS:
 *
 *      R0 - out_handle and status
 *#o
 *              o R0[63:61] 
 *#o		   
 *		       o '000', success, handle to peer received
 *                     o '001', success, node current has no peer
 *		       o '101', failure, comp data byte stream truncated
 *		       o '110', failure, ident data byte stream truncated
 *#
 *		o R0[48:60] SBZ
 *		o R0[47:40] number of bytes written into ident_buffer
 *		o R0[39:32] number of bytes written into comp_buffer
 *		o R0[31:0] handle to peer of node current.  Will be 0 if no
 *		          peer was found.
 *#
 *       
 * ARGUMENTS:
 *
 *      current  - R16; handle to component to get peer of.
 *	comp_buffer - R17; address of buffer to copy CONFIGURATION_COMPONENT 
 *			data to.
 *	comp_size - R18; number of bytes of CONFIGURATION_COMPONENT data to 
 *			 copy.
 *	ident_buffer - R19; address if buffer to copy Identifier string to.
 *	ident_size - R20; number of bytes of Identifier data to copy
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

/*+
 * ============================================================================
 * = cb_GetChild - returns handle to child in configuration tree              =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      cb_GetChild operates on the firmware configuration tree.  The routine 
 *	writes the CONFIGURATION_COMPONENT data of the child of handle current 
 *	into comp_buffer.  It writes the associated identifier, if any, to 
 *	ident_buffer.  The size of the CONFIGURATION_COMPONENT is 36 bytes. 
 *	This value should be written into comp_size.  The recommended size for 
 *	the ident buffer is 64 bytes.  This value should be written into 
 *	ident_size.  A handle to the child of current is returned in R0[31:0].  
 *	The number of bytes written into each of the buffers, comp and ident, 
 *	is returned in R0[39:32] and R0[47:40], respectively.  R0[63:61] 
 *	contain the status of the call.    
 *
 * FORM OF CALL:
 *
 *	void *out_handle cb_GetChild(void *current, void *comp_buffer, 
 *				unsigned int comp_size, void *ident_buffer,
 *				unsigned int ident_size);
 *
 * RETURNS:
 *
 *      R0 - out_handle and status
 *#o
 *              o R0[63:61] 
 *#o		   
 *		       o '000', success, handle to child received
 *                     o '001', success, node current has no child
 *		       o '101', failure, comp data byte stream truncated
 *		       o '110', failure, ident data byte stream truncated
 *#
 *		o R0[48:60] SBZ
 *		o R0[47:40] number of bytes written into ident_buffer
 *		o R0[39:32] number of bytes written into comp_buffer
 *		o R0[31:0] handle to child of node current.  Will be 0 if no
 *		          child was found.
 *#
 *       
 * ARGUMENTS:
 *
 *      current  - R16; handle to component to get child of.
 *	comp_buffer - R17; address of buffer to copy CONFIGURATION_COMPONENT 
 *			data to.
 *	comp_size - R18; number of bytes of CONFIGURATION_COMPONENT data to 
 *			 copy.
 *	ident_buffer - R19; address if buffer to copy Identifier string to.
 *	ident_size - R20; number of bytes of Identifier data to copy
 *
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

/*+
 * ============================================================================
 * = cb_GetParent - returns handle to parent in configuration tree            =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      cb_GetParent operates on the firmware configuration tree.  The routine 
 *	writes the CONFIGURATION_COMPONENT data of the parent of handle current 
 *	into comp_buffer.  It writes the associated identifier, if any, to 
 *	ident_buffer.  The size of the CONFIGURATION_COMPONENT is 36 bytes. 
 *	This value should be written into comp_size.  The recommended size for 
 *	the ident buffer is 64 bytes.  This value should be written into 
 *	ident_size.  A handle to the parent of current is returned in R0[31:0].
 *	The number of bytes written into each of the buffers, comp and ident, 
 *	is returned in R0[39:32] and R0[47:40], respectively.  R0[63:61] 
 *	contain the status of the call.    
 *                                             
 *
 * FORM OF CALL:
 *
 *	void *out_handle cb_GetParent(void *current, void *comp_buffer, 
 *				unsigned int comp_size, void *ident_buffer,
 *				unsigned int ident_size);
 *
 * RETURNS:
 *
 *      R0 - out_handle and status
 *#o
 *              o R0[63:61] 
 *#o		   
 *		       o '000', success, handle to parent received
 *                     o '001', success, node current has no parent
 *		       o '101', failure, comp data byte stream truncated
 *		       o '110', failure, ident data byte stream truncated
 *#
 *		o R0[48:60] SBZ
 *		o R0[47:40] number of bytes written into ident_buffer
 *		o R0[39:32] number of bytes written into comp_buffer
 *		o R0[31:0] handle to parent of node current.  Will be 0 if no
 *		          parent was found.
 *#
 *       
 * ARGUMENTS:
 *
 *      current  - R16; handle to component to get parent of.
 *	comp_buffer - R17; address of buffer to copy CONFIGURATION_COMPONENT 
 *			data to.
 *	comp_size - R18; number of bytes of CONFIGURATION_COMPONENT data to 
 *			 copy.
 *	ident_buffer - R19; address if buffer to copy Identifier string to.
 *	ident_size - R20; number of bytes of Identifier data to copy
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

/*+
 * ============================================================================
 * = cb_GetComponent - returns handle to component in config tree 	      =
 * ============================================================================
 *
 * OVERVIEW:
 *      cb_GetComponent returns a pointer to the component in the firmware
 *	configuration tree specified by the path.  The path must be ARC-valid.
 *	The routine writes the CONFIGURATION_COMPONENT data of the component
 *	specified by the path into comp_buffer.  It writes the associated 
 *	identifier, if any, to ident_buffer.  The size of the 
 *	CONFIGURATION_COMPONENT is 36 bytes. This value should be written into 
 *	comp_size.  The recommended size for the ident buffer is 64 bytes.  
 *	This value should be written into ident_size.  A handle to the parent 
 *	of current is returned in R0[31:0].  The number of bytes written into 
 *	each of the buffers, comp and ident, is returned in R0[39:32] and 
 *	R0[47:40], respectively.  R0[63:61] contain the status of the call.    
 *     
 *
 * FORM OF CALL:
 *
 *	void *out_handle cb_GetComponent(CHAR *path, void *comp_buffer, 
 *			unsigned int comp_size, void *ident_buffer, 
 *			unsigned int ident_size);
 *
 *
 * RETURNS:
 *
 *      R0 - out_handle and status
 *#o
 *              o R0[63:61] 
 *#o		   
 *		       o '000', success, handle to component received
 *                     o '100', failure, invalid path or component not found
 *		       o '101', failure, comp data byte stream truncated
 *		       o '110', failure, ident data byte stream truncated
 *#
 *		o R0[48:60] SBZ
 *		o R0[47:40] number of bytes written into ident_buffer
 *		o R0[39:32] number of bytes written into comp_buffer
 *		o R0[31:0] handle to node specified by path.  Will be 0 if node
 *		          wasn't found.
 *#       
 * ARGUMENTS:
 *
 *      path  - R16; pointer to path representation of node to find.
 *	comp_buffer - R17; address of buffer to copy CONFIGURATION_COMPONENT 
 *			   data to.
 *	comp_size - R18; number of bytes of CONFIGURATION_COMPONENT data to 
 *			 copy.
 *	ident_buffer - R19; address if buffer to copy Identifier string to.
 *	ident_size - R20; number of bytes of Identifier data to copy
 *
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

/*+
 * ============================================================================
 * = cb_GetConfigurationData - writes any configuration data into a buffer    =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	cb_GetConfigurationData operates on the firmware configuration tree.
 *	Any configuration data associated with Component is copied into the 
 *	buffer ConfigurationData.  The size of the buffer is determined by 
 *	examining the ConfigurationDataSize field in the 
 *	CONFIGURATION_COMPONENT.  This value should be written into size.
 *	The number of bytes written into buffer ConfigurationData is returned
 * 	in R0[39:32].  R0[63:61] contain the status of the call.
 *
 *
 * FORM OF CALL:
 *
 *	status cb_GetConfigurationData(void *Component,void *ConfigurationData, 
 *			unsigned int size);
 *
 * RETURNS:
 *
 *      R0 - status
 *#o
 *              o R0[63:61] 
 *#o		   
 *		       o '000', success, configuration data copied
 *                     o '100', failure, no configuration data is available 
 *		       o '101', failure, configuration data byte stream truncated
 *#
 *		o R0[40:60] SBZ
 *		o R0[39:32]number of bytes written into ConfigurationData 
 *		o R0[31:0] SBZ
 *#
 *       
 *   
 * ARGUMENTS:
 *
 *      Component  - R16; handle of node to obtain ConfigurationData from
 *	ConfigurationData - R17; buffer to store obtained configuration data
 *	size - R18; number of bytes of data to copy.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

/*+
 * ============================================================================
 * = Read_PCI_Config_Register - Read a register in PCI configuration space.   =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      This SRM callback provides a generic facility for an SRM compliant
 *	operating system to perform PCI configuration space read operations
 *	in a platform independent way.
 *	
 *	The OS requires access to PCI configuration space at system
 *	initialization and during runtime.  In the absence of other
 *	mechanisms, the OS must scan configuration space at initialization
 *	time to determine which PCI devices are present.  At runtime, the OS
 *	must read and write configuration space registers when responding to
 *	hardware errors.
 *	
 *	The PCI Specification does not dictate how a configuration space
 *	address is mapped to a particular PCI device on the bus.  The mapping
 *	is determined by the host-to-PCI bridge.  The way in which the address
 *	is passed to the bridge, and the way in which an address maps to a PCI
 *	device can vary from bridge to bridge.  An OS must have code that
 *	knows how to access configuration space for each supported bridge.  To
 *	reduce this burden on the OS, console should provide a standard
 *	interface to configuration space which hides the quirks of each
 *	bridge.  Note the PCI community addresses this issue on PC platforms
 *	by specifying PCI BIOS calls to read and write configuration space.
 *	
 *	Two console callbacks are needed.  The parameters identify a PCI
 *	device using terminology and limits from the PCI Specification.
 *	
 * RESTRICTIONS:
 *
 *	When using these callbacks, the operating system must provide 
 *	synchronization (IPL/locks, etc) to prevent modification of the
 *	PCI device registers and the host-to-PCI bridge register.
 *	
 *	The operating system will receive any hardware error notification that
 *	occurs as a result of the access.  For example, most bridges trigger a
 *	machine check when an non-existent PCI device is addresses.
 *	
 *  ??  (Is this feasible/consistent etc?  An alternative would be for
 *	console to trap and clean up any errors, returning a status value.
 *	Is that workable?)
 *	
 *	In a multiprocessor system, the operating system must make these
 *	callbacks only from the primary processor.
 *  ??  (Is this really a requirement, or is it overly restrictive?
 *	I think all current callbacks are restricted to the primary?)
 *	
 * FORM OF CALL:
 *  
 *	Read_PCI_Config_Register ( int *result,
 *			int size, int bridge, int bus,
 *			int device, int function, int register )
 *
 * RETURN CODES:
 *
 *	01 - if the data was successfully read
 *	00 - if an invalid parameter was supplied
 *
 * ARGUMENTS:
 *
 *	int *result - The virtual address of the buffer to receive the 
 *		 register contents.  This virtual address must be 
 *		 currently mapped by the operating system.
 *
 *	int size - The size of the register to be read in bytes.
 *		 Legal values are 1, 2, and 4.
 *
 *	int bridge - The number of the host-to-PCI bridge being accessed.
 *		 Legal values are from 0 to one less than the number of
 *		 bridges on the system.
 *		 (If an ARC configuration tree is used, these could be
 *		  numbered in the order of traversing the tree.
 *
 * 	int bus - The PCI bus to be accessed through the bridge.
 *		 Legal values are 0 (the primary PCI), and 1-255
 *		 (busses accesses through PCI-to-PCI bridges).
 *
 *	int device - The PCI device on the bus.  Legal values are 0-31.
 *
 *	int function - For multi-function PCI devices, the function on the
 *		 device.  Legal values are 0 (the first or only function
 *		 on the device) to 7.
 *
 *	int register - The offset of the register to read.  Legal values are
 *		   0-255.
 *
 * SIDE EFFECTS:
 *
 *	PCI Configuration Space read access generated.
 *
-*/

/*+
 * ============================================================================
 * = Write_PCI_Config_Register - Write a register in PCI configuration space. =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *      This SRM callback provides a generic facility for an SRM compliant
 *	operating system to perform PCI configuration space write operations
 *	in a platform independent way.
 *
 *	See Read_PCI_Config_Register overview for more details.
 *  
 * FORM OF CALL:
 *  
 *	Write_PCI_Config_Register ( int data,
 *			int size, int bridge, int bus,
 *			int device, int function, int register )
 *
 * RETURN CODES:
 *
 *	01 - if the data was successfully written
 *	00 - if an invalid parameter was supplied
 *
 * ARGUMENTS:
 *
 *	int data - The value to be written to a configuration space register.
 *
 *	int size - The size of the register to be read in bytes.
 *		 Legal values are 1, 2, and 4.
 *
 *	int bridge - The number of the host-to-PCI bridge being accessed.
 *		 Legal values are from 0 to one less than the number of
 *		 bridges on the system.
 *		 (If an ARC configuration tree is used, these could be
 *		  numbered in the order of traversing the tree.
 *
 * 	int bus - The PCI bus to be accessed through the bridge.
 *		 Legal values are 0 (the primary PCI), and 1-255
 *		 (busses accesses through PCI-to-PCI bridges).
 *
 *	int device - The PCI device on the bus.  Legal values are 0-31.
 *
 *	int function - For multi-function PCI devices, the function on the
 *		 device.  Legal values are 0 (the first or only function
 *		 on the device) to 7.
 *
 *	int register - The offset of the register to read.  Legal values are
 *		   0-255.
 *
 * SIDE EFFECTS:
 *
 *	PCI Configuration Space write access generated and any side effects
 *	of the particular write.
 *
-*/



#if FIBRE_CHANNEL
/*+
 * ============================================================================
 * = srm_long2short - take an OS long SRM specifier and turn it into short    =
 * ============================================================================
 *
 *  Currently this is only used for the wwid mechanism used in fibre channel.
 *  But in the future, it could be used whenever the OS has some information
 *  about a srm specifier to pass us that is not already described in an
 *  srm specifier.  Or to be unincumbered by the indirection to an ev that
 *  gives extended data as in the case of wwids.
 *
-*/
int srm_long2short( char *srmlong, char *srmshort )
{
  int i;
  int status = msg_success;

  if( strncmp_nocase("SCSI3", srmlong, 5) != 0 )
    return msg_failure;

  status = fc_long2short( srmlong, srmshort );

  return status;
}


/*+
 * ============================================================================
 * = fc_long2short - convert a long form SRM specifier into the short form    =
 * ============================================================================
 *
 *  Currently, the only devices the fibre channel driver will configure are those
 *  that are "registered" via the wwidmgr.  Registering a device means setting
 *  a wwid<n> ev to a devices WWID, and setting any paths (i.e. n_port ports)
 *  to the port World Wide Name (WWN).
 *  
 *  There are 4 wwid evs and 4 N evs.  When the port driver probes the fibre
 *  channel loop or fabric, it will only build up data structures (sb's) and
 *  actively login (e.g. an FC PLOGI and PRLI) with those n_ports that correlate
 *  to N evs.
 *  
 *  When the SCSI class driver polls the n_port for devices, it checks the
 *  devices wwid against the wwid ev, and only builds data structures (ub's and
 *  inode's) for devices that correspond to the wwid evs.
 *  
 *  On any one adapter, at most 16 device inodes will be created (4 devices
 *  possibly accessed by 4 different n_ports, as in the case of the HSG80).
 *  Each adapter could be connected to the same loop or fabric, and since the
 *  adapter is part of the device name, each adapter could generate 16 devices.
 *  It is possible in FC to have many paths to the same device, and the console
 *  makes a different device name for each of those.
 *  
 *  There is a new request from UNIX to allow callbacks to access devices that
 *  are not registered.  
 *  
 *  This can be accomplished if the port WWN and device WWID are fully specified
 *  in the SRM string passed on the cb_open.  Below is a sample long form
 *  SRM specifier :
 *  
 *   SCSI3 0 6 0 0 0 0 0 PN:a1040000e11f0050 WWID:01000010:6000-1fe1-0000-04a0-ffff-fffe-0005-0000
 *  
 *  The PN is the port WWN that is used by the console port driver to allow
 *  access to that n_port, establish a virtual circuit with that n_port, and
 *  build up the system block (sb) data structure.   The WWID field is the
 *  information the class driver needs to allow an inode to be built to the
 *  device.
 *  
 *  This long form SRM specifier is only created by the operating system.  It
 *  is never stored bootdef_dev nor dump_dev nor in any non-volatile backing like
 *  the NVRAM.   It is passed into cb_open, converted by fc_long2short and 
 *  then discarded.   dev2file and file2dev will never manipulate the long
 *  form SRM specifier, only the short form that fc_long2short creates.  The
 *  long form SRM specifier is therefore short lived.
 *  
 *  
 *  Impact statement:
 *  -------------------
 *     In call_backs.c, prior to drivers starting (and prior to create_device)
 *  on Turbo, a new routine fc_long2short will convert that long srm specifier
 *  into the short form that the console is equipped to handle today.  The
 *  short form SRM specifier looks like :
 *  
 *          SCSI3 0 3 0 1 0 0 0 @wwid1  
 *  
 *  where the node field ( a 1 in this case ) identifies the port ev and the
 *  wwid1 indicates the WWID ev of the device.
 *  
 *   The new console routine fc_long2short will allocate new volatile port and 
 *  wwid data structures, add them to the already existing table of 4 non-volatile
 *  port and wwid data structures.   When the port driver is started, it will
 *  use all available port and wwid information, both volatile and non-volatile
 *  and make sb's and inodes.
 *  
 *   The fc_long2short also has to contrive the value used for the wwid<n> and
 *  the SRM specifier node field.  These two values can be different.  I have 
 *  chosen to fabricate this number to be 900+num_portnames, which is a value
 *  well outside of where any pre-registered number will be.
 *  
 *   These changes are isolated to the kgpsa_driver.c and to call_backs.c.
 *  
 *   The callback registered portname and wwid will only be erased with a
 *  console init, reset, or power cycle.
 *  
 *  
 *  
 *  Issues
 *  ---------
 *  
 *         - How long can the SRM string be?  
 *         - How many of these can you open at the same time ( I can see
 *           a problem with more than one right now )
 *         - Are my fabricated device names good enough since 
 *           they are meaningless
 *  
 *  
 *  
 *   The restriction of only one device being open at a time is imposed because
 *  the loop/fabric probing is only done at driver startup or loop initialization.
 *  A newly registered port will not get probed if the driver is already started.
 *  With difficulty, it is possible to work around this restriction, but I 
 *  chose not to do that at this time.
 *  
 *
-*/
#define FAB_OFFSET 900
#define FC_NO_SWAP_BYTES 0
#define FC_SWAP_BYTES    1
int fc_long2short( char *srmlong, char *srmshort )
{
  extern struct registered_portname **portnames;
  extern unsigned int num_portnames;
  extern unsigned int num_wwids;
 
  int i;
  int status = msg_success;
  struct registered_portname *cb_p;
  struct registered_wwid *cb_w;
  char buf[128];		    /* Should be long enough - no dev lists */
  char ascii_node[10], *pascii_node = &ascii_node[0];
  char *c, *pn, *ww, *node, *cont;
  int node_len;
  char space;
  unsigned __int64 p1;
  union wwid *w;
  int unit, collision;

  if( strncmp_nocase("SCSI3", srmlong, 5) != 0 )
    return msg_failure;

  pn = strstr( srmlong, "PN:" );
  if ( !pn )
    return msg_failure;

  ww = strstr( srmlong, "WWID:" );
  if (!ww)
    return msg_failure;

  /*
   * A PN: label was found, assume that port specifier after that is valid.
   * Create a new portname structure and add it to the list of portnames that
   * is scanned by each fibre channel port driver.  When the port driver
   * exchanges the PLOGI service parameters, which contain the port name, 
   * the portnames data structure is used to limit the n_ports we create vc's
   * with. The creation of the file name uses the evnum field as an alias
   * to the long WWN portname.  The user specifies that value with the wwidmgr
   * assignment.  On a callback, we just fabricate an evnum that is well
   * outside the range of any portname evnum by adding 900 to the evnum.
   * We cant just use the num_portnames because we reserve num_portnames < 4
   * to be backed by real ev's N1, N2, N3, and N4.
   *
   * NOTE: During wwidmgr mode, node alias are fabricated from the algorithm
   *
   *                  node_id = pb->node_count+10
   *
   *       which can be a value like 900+num_portnames when the node count
   *       is over 900.  The problem would occur when the OS created a
   *       portname, assigned by the console to say 905.   Then if the user
   *       ran the wwidmgr, the 905th n_port would conflict.   
   *       
   */

  p1 = xtoi(&pn[3]);
  if( (cb_p = fc_find_portname( &p1, &i, FC_NO_SWAP_BYTES )) == 0 ) {
    pprintf("registering new portname - %s\n", pn );
    cb_p = dyn$_malloc( sizeof(struct registered_portname), DYN$K_SYNC | DYN$K_NOOWN );;
    portnames[num_portnames++] = cb_p;
    strncpy( cb_p->portname, &pn[3], 16 );
    cb_p->sb_allowed = 1;
    cb_p->evnum = FAB_OFFSET+num_portnames;
  } else {
    pprintf("portname already registered at index %d\n", i);
  }

  /*
   * A WWID label was found. Assume the WWID specifier is correct.  Convert
   * the WWID string into the registered_wwid format, and then fill out the
   * structures fields. Insert it into the wwids table so that the class driver
   * will use this information on its next poll cycle to allow building an
   * inode for this wwid.  Since there is no real ev backing this wwids
   * structure, fabricate an evnum that could be used in the device name
   * node field.  There are only a small number of real ev's (wwid0, wwid1,
   * wwid2, wwid3).  We only have to avoid the node ids of 0,1,2,3.  The
   * same value as the portname evnum works, so we use that.  
   *
   */
  w = dyn$_malloc( sizeof(union wwid), DYN$K_SYNC | DYN$K_NOOWN );
  sprintf( buf, "0 0 %s", ww );
  wwev2wwid( buf, &unit, &collision, w );
  krn$_wait(&wwids_lock);
  if( (cb_w = find_wwidfilter_from_wwids( w, &i)) == 0 ) {
    pprintf("registering new wwid - %s\n", buf );
    cb_w = dyn$_malloc( sizeof(struct registered_wwid), DYN$K_SYNC | DYN$K_NOOWN );
    memcpy(&cb_w->wwid, w, sizeof(union wwid) );
    cb_w->evnum = FAB_OFFSET+num_wwids;
    insert_wwid( cb_w );
  } else {
    pprintf("wwid already registered at %d\n", i );
  }
  krn$_post(&wwids_lock);
  dyn$_free(w, DYN$K_SYNC|DYN$K_FLOOD);

  /*
   ** We have completed registering the portname and wwid.  The 2nd step
   ** of this operation is to convert the SRM string the OS passed in into
   ** the short form that the console already knows how to use.  We will
   ** pretend there is a wwid9xx.  dev2file never goes after the ev, just
   ** its in-memory structure which was created above.
   */

  /*
   * The SRM string is usable as is up to but not including the node field,
   * and then usable again after the node field up to the PN.  Find where
   * the node field starts and where the field after the node field starts.
   */
  node = srmlong;
  for(i=0; i<4; i++ ) {
    node = strchr(node+1,' ');
  }
  cont = strchr(node+1, ' ');

  /*
   * Copy the long form up to but not including the node field.
   */
  i = 0;
  c = srmlong;
  memset( buf, 0, 128 );
  while( c < node ) {
    buf[i++] = *c++;
  }
   
  /*
   * Calculate the node id field that points to the portname data
   * structure where we stored the OS information.  We'll use that data
   * in the port driver, but now we need to alias it ala short form that
   * dev2file uses.
   */
  sprintf( ascii_node, "%d", cb_p->evnum );
  buf[i++] = ' ';

  /*
   * Slip the new node value into the short form SRM we are creating.
   */
  while( *pascii_node != '\0' ) {
    buf[i++] = *pascii_node++;
  }

  /*
   * Take the rest of the long SRM string up to but not including the PN field.
   */
  c = cont;
  while( c < pn ) {
    buf[i++] = *c++;
  }

  /*
   * Alias the WWID information that was handed to us in the form that 
   * dev2file likes.  The info was stored away above, and we just indirectly
   * point to it here.
   */
  sprintf( srmshort, "%s@wwid%d ", buf, cb_w->evnum );

#if DEBUG
  pprintf("srmshort: %s \n", srmshort );
#endif
  return status;
}
#endif

#if EEROM_CALLBACKS
void cb_write_eerom( struct impure *IMPURE )
    {
    uint64 buffer;
    uint64 length;
    uint64 fru_id;
    uint64 operation_code;
    uint64 status;
    struct FILE *vmemfp;
    unsigned char *buf;
    int l;

#if CALLBACK_DEBUG && 0
    if (callback_debug_flag & 0x20)
	pprintf("calling cb_write_eerom\n" );
#endif
    buffer = *(uint64 *)&IMPURE->cns$gpr[0][2*17];
    length = *(uint64 *)&IMPURE->cns$gpr[0][2*18];
    fru_id = *(uint64 *)&IMPURE->cns$gpr[0][2*19];
    operation_code = *(uint64 *)&IMPURE->cns$gpr[0][2*20];

#if CALLBACK_DEBUG
    if (callback_debug_flag & 0x20)
	pprintf("cb_write_eerom: buffer = %016.16x, length = %x, fru_id = %x, operation_code = %x\n",
		buffer, length, fru_id, operation_code );
#endif

    vmemfp = fopen( "vmem", "sr" );
    if( vmemfp == NULL )
	{
	*(uint64 *)&IMPURE->cns$gpr[0][2*0] = (uint64)4 << 60;
	return;
	}
    buf = malloc_noown( 256 );

    fseek( vmemfp, (uint64)buffer, SEEK_SET );
    l = fread( buf, 1, length, vmemfp );
    if( l != length )
	{
	*(uint64 *)&IMPURE->cns$gpr[0][2*0] = (uint64)4 << 60;
	free( buf );
	fclose( vmemfp );
	return;
	}

    status = fru_write_eerom( buf, length, fru_id, operation_code );

    IMPURE->cns$gpr[0][2*0+1] = status << 28;
    if( status == msg_success )
	IMPURE->cns$gpr[0][2*0] = length;
    else
	IMPURE->cns$gpr[0][2*0] = 0;
#if CALLBACK_DEBUG
    if (callback_debug_flag & 0x20)
	pprintf("cb_write_eerom: status: %016x\n", *(uint64 *)&IMPURE->cns$gpr[0][2*0] );
#endif

    free( buf );
    fclose( vmemfp );
    }

void cb_read_eerom( struct impure *IMPURE )
    {
    uint64 buffer;
    uint64 length;
    uint64 fru_id;
    uint64 operation_code;
    uint64 status;
    struct FILE *vmemfp;
    unsigned char *buf;
    int l;

#if CALLBACK_DEBUG && 0
    if (callback_debug_flag & 0x20)
	pprintf("calling cb_read_eerom\n" );
#endif
    buffer = *(uint64 *)&IMPURE->cns$gpr[0][2*17];
    length = *(uint64 *)&IMPURE->cns$gpr[0][2*18];
    fru_id = *(uint64 *)&IMPURE->cns$gpr[0][2*19];
    operation_code = *(uint64 *)&IMPURE->cns$gpr[0][2*20];

    vmemfp = fopen( "vmem", "sw" );
    if( vmemfp == NULL )
	{
	*(uint64 *)&IMPURE->cns$gpr[0][2*0] = (uint64)4 << 60;
	return;
	}
    buf = malloc_noown( 256 );

    status = fru_read_eerom( buf, length, fru_id, operation_code );

    IMPURE->cns$gpr[0][2*0+1] = ( status & 0xf ) << 28;
    if( ( status & 0xf ) == msg_success )
	IMPURE->cns$gpr[0][2*0] = status >> 8;
    else
	IMPURE->cns$gpr[0][2*0] = 0;

#if CALLBACK_DEBUG
    if (callback_debug_flag & 0x20)
	pprintf("cb_read_eerom: buffer = %016x, length = %x, fru_id = %x, operation_code = %x\n",
		buffer, length, fru_id, operation_code );
#endif

    fseek( vmemfp, (uint64)buffer, SEEK_SET );
    l = fwrite( buf, 1, length, vmemfp );
    if( l != length )
	*(uint64 *)&IMPURE->cns$gpr[0][2*0] = (uint64)4 << 60;

#if CALLBACK_DEBUG
    if (callback_debug_flag & 0x20)
	pprintf("cb_read_eerom: status: %016x\n", *(uint64 *)&IMPURE->cns$gpr[0][2*0] );
#endif
    free( buf );
    fclose( vmemfp );
    }
#endif
