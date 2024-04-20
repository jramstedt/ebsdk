/* file:	i2o_ovly.c
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
 *
 * Abstract:	I2O Overlay for Common console.
 *
 * Authors:	Kevin LeMieux
 *
 * Modifications:
 *
 *	kl	 14-Jun-1994	initial entry
 *
 */

#include "cp$src:platform.h"                  
#include "cp$inc:kernel_entry.h"

extern int i2o_shared_adr;

extern i2o_setmode();
extern i2o_poll_units();
extern i2o_establish_connection();
extern i2o_bsa_transfer();

int ovly_rtn_table[] = {
	i2o_bsa_transfer,
	0
};

void startup_overlay()
{
    i2o_shared_adr = &ovly_rtn_table;
    dev_table_add("I2O", "I2O", i2o_setmode, i2o_poll_units, i2o_establish_connection);
}

void shutdown_overlay()
{
    dev_table_remove("I2O", "I2O");
    i2o_shared_adr = 0;
}

