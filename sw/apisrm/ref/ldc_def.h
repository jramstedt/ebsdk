/* file:	ldc_def.h
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
 *      Alpha Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *      The ldc_def.h module contains the data constaints returned by the 
 *	check ldc routine
 *
 *  AUTHORS:
 *
 *      David Baird
 *
 *  CREATION DATE:
 *  
 *      03-Aug-1992
 *
 *  MODIFICATION HISTORY:
 *
 *	ini	03-Aug-1992	David Baird
 *	
 *--
 */


#ifndef	ldc_h

#define ldc_h 0		    /* Prevent multiple compiles of this module */

#define LDC_NOT_ENABLED		0x01
#define LDC_FAILED		0x02
#define LDC_NOT_ENB_FAIL	0x03
#define LDC_NOT_PRESENT		0x04

#endif
