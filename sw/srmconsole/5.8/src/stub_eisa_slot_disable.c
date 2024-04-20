/* file:	stub_eisa_slot_disable.c
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
 */

/*
 *++
 *  FACILITY:
 *
 *      Alpha Firmware for APS Platforms
 *
 *  MODULE DESCRIPTION:     
 *
 *	Stub routine for eisa_slot_cfg_disable routine.
 *
 *  AUTHORS:
 *
 *      David Baird
 *
 *  CREATION DATE:
 *  
 *      22-Nov-1994
 *
 *  MODIFICATION HISTORY:
 *
 *	dwb	22-Nov-1994	Initial entry 
 *
 *--
 */                                                

#include	"cp$src:alpha_arc.h"

/*+
 * ============================================================================
 * = eisa_slot_cfg_disable - Disables a device if no info available  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	If not port, Memory, or IRQ data is available for a slot zero function
 *	and it exists in the pc87312 chips, disable the device within the chip 
 *	to allow it's resources to be used by other devices.
 *  
 * FORM OF CALL:
 *      
 *	VOID eisa_slot_cfg_disable(IN PUCHAR EisaFuncInfo, IN ULONG BusNumber,
 *			           IN ULONG SlotNumber )
 *  
 * RETURNS:
 *
 *	NULL
 *          
 * ARGUMENTS:
 *
 *	IN PUCHAR EisaFuncInfo - function info pointer
 *	IN ULONG BusNumber - EISA bus number
 *	IN ULONG SlotNumber - EISA slot number
 *
 * SIDE EFFECTS:
 *
 *	
 *
-*/
void eisa_slot_cfg_disable (UCHAR *p_config,ULONG BusNumber,ULONG SlotNumber)
{
        return;
}
