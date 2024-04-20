/*
 * file:	from_def.h   - definitions for Flash ROM
 *                                  
 * Copyright (C) 1997, 1998 by
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
 *	Definitions for AlphaPC 164SX Flash Rom
 *
 *  AUTHORS:
 *
 *	Eric Rasmussen
 *
 *  CREATION DATE:
 *  
 *     12-Mar-1997
 *
 *
 *  MODIFICATION HISTORY:
 *
 *	er	18-Feb-1998	Changed flash layout for PC264.
 *      egg	 5-Aug-1997	Added conditionals for PC264 and SX164.
 *
 *	er	18-Jul-1997	Change FWROM offset definitions to allow for
 *				failsafe loader in first 64K sector.
 *--
 */

#ifndef __FROM_DEF_H
#define __FROM_DEF_H

struct FROM
 {
  char		*name;		/* Name of device */
  unsigned int	base;		/* Base offset of device */
  unsigned int	sector_length;	/* Device sector length (in bytes) */
  unsigned int	length;		/* Size of device (in bytes) */
  int		flags;		/* Device attribute mask */
 };

#endif /* __FROM_DEF_H */
