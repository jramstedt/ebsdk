/* file:	stub_use_eisa_ecu_data.c
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
 *	Stub routine for use_eisa_ecu_data routine.
 *
 *  AUTHORS:
 *
 *      Eric Rasmussen
 *
 *  CREATION DATE:
 *  
 *      06-May-1997
 *
 *  MODIFICATION HISTORY:
 *
 *	er	06-May-1997	Initial entry 
 *
 *--
 */                                                

#include	"cp$src:kernel_def.h"
#include	"cp$src:pb_def.h"

int use_eisa_ecu_data( struct pb *pb )
{
    return( 0 );
}
