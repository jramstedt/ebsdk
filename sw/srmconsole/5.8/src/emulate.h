/*
 * file:	emulate.h
 *
 * Copyright (C) 1990, 1991, 1992, 1993 by
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
 * Abstract:	Definitions for emulation
 *              
 *
 * Author:	Kevin LeMieux
 *
 * Modifications:
 *	
 *	kl	28-Jun-1993	Initial. 
 */

#define ISP_MODEL 1
#define RUBY_HW 2
#define TURBO_HW 3

#define HOST_TO_HOST 1			/* Emulate host on host */
#define TARGET_TO_HOST 2		/* Emulate target on host */
#define TARGET_TO_TARGET 3		/* Emulate target on target */
#define LOCAL_TYPE 0x1234

#define FAKE_LEP  0xF001
#define FAKE_MIC  0xF002
#define FAKE_IOP  0xF003   
#define FAKE_TIOP 0xF004

struct emulation {
    unsigned int code;				 /* which way to emulate */
    unsigned int target_offset;			 /* target CSR offset */
    unsigned int host_offset;			 /* host CSR offset */
    unsigned int (*emulate_read_operation)();	 /* routine to emulate CSR read access from target/host to host */
    unsigned int (*emulate_write_operation)();	 /* routine to emulate CSR write access from target/host to host */
    char *register_name;			 /* Name of register */
}; 

struct emulation_address_table {
    unsigned int address_type;			/* address type */
    struct emulation *emulation_table;		/* emulation table associated with the device */
    char *address_space_name;			/* Name of address space */
}; 

typedef struct {
	int modtype;	
	int dev;
	int spare;
} FAKEMODULE;

#define TARGET_SLOT(x) (x & 0x03c00000) >> 22			/* get the slot */
#define TARGET_OFFSET(x) x & 0x0000ffff				/* get the offset */
#define SYS_BUS_ADDRESS(x) ((x >> 28) == 8)			/* hmmm */
#define HOST_BUS_ADDRESS(x) (0xF8000000 + (x * 0x400000))	/* host bus address */
#define TARGET_BUS_ADDRESS(x) (0x88000000 + (x * 0x400000))	/* host bus address */
