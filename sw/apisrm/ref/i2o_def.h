/*
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
 * and  should  not  be  construed  as a commitment by Digital Equipment
 * Corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by Digital.
 *
 *
 * Abstract:	I2O Console specific definitions
 *              
 *              This contains I2O declarations that are specific to
 *              the console implementation, except for i2o_pb_def.h
 *              which gets its own file historically.   All the other
 *              I2O files should remain in their pure form so that no
 *              editing is required when new versions come down off the
 *              SIG web site.
 *
 * Author:	Dennis Mazur
 *
 * Modifications:
 *
 *	dm 	14-Aug-1997	Initial entry.
 */
 
#if !defined(I2O_DEF_HDR)
#define I2O_DEF_HDR

#define DEBUG 0

#include    "cp$src:i2odep.h"
#include    "cp$src:i2omsg.h"

#if DEBUG
#define dprintf(fmt, args)      \
    pprintf(fmt, args)
#else
#define dprintf(fmt, args)
#endif

#define BASE0         0x10            /* PCI bar */

#define I2O_SUBCLASS_SCSI 1
#define I2O_RESOURCE_NOT_CLAIMED 0xfff
#define I2O_MAX_CONTROLLERS 4

#define I2O_INTERRUPT_STATUS_REG 0x30
#define I2O_INTERRUPT_MASK_REG   0x34
#define I2O_INBOUND_FIFO_OFFSET  0x40
#define I2O_OUTBOUND_FIFO_OFFSET 0x44
#define I2O_OUTBOUND_INTERRUPT   8

#define READ  1
#define WRITE 2
#define INBOUND  I2O_INBOUND_FIFO_OFFSET
#define OUTBOUND I2O_OUTBOUND_FIFO_OFFSET
/*
 *  Redefine these from the host's perspective
 */
#define INBOUND_FIFO    I2O_INBOUND_FIFO_OFFSET
#define OUTBOUND_FIFO   I2O_OUTBOUND_FIFO_OFFSET


#define I2O_CNSL_MF_SIZE    128		/* Max permissable message frame */
#define I2O_CNSL_FIFO_SIZE   16         /* FIFO Queue size */
#define I2O_EMPTY_FIFO      0xFFFFFFFF


/*
 * NOTE: the MFHDR and the I2O request frame are always allocated 
 * contiguously. The address of the I2O request frame is equal to the
 * address of the MFHDR plus the sizeof(  I2O_CNSL_MFHDR ).
 */

typedef struct _I2O_CNSL_MFHDR {
   I2O_MESSAGE_FRAME	    *reply;
   U32			    context_type;
   struct TIMERQ	    rsp_t;
   void                     (*callback)( struct pb *pb, int *mfhdr );
} I2O_CNSL_MFHDR, *PI2O_CNSL_MFHDR;

#define I2O_CNSL_CONTEXT_NONE     	0x00
#define I2O_CNSL_CONTEXT_CALLBACK	0x01
#define I2O_CNSL_CONTEXT_SEMAPHORE	0x02

#define I2O_RESET_TIMEOUT 10000
#define I2O_INBOUND_TIMEOUT 2000
#define I2O_OUTBOUND_TIMEOUT 2000
#define I2O_REQUEST_TIMEOUT 300000
#define I2O_REQUEST_SUCCESS 0x23


/*
 * Extern of the literals from i2o_literals.h
 */
extern char *iopstate_text[];


/*
 * OSM 
 */

struct osm {
    int unit_class;
    int adapter_class;
    int (*new_unit)();
    int (*shutdown)();
    int (*adapter_init)();
};


#define LCT_ENTRY_BLOCK 8
#define I2O_MAX_SB 48
#define NUM_LCT_ENTRIES( x )  ( x->TableSize - (( sizeof( I2O_LCT ) \
	- sizeof( I2O_LCT_ENTRY )) / 4 )) / (sizeof( I2O_LCT_ENTRY ) / 4 )

#define I2O_EVENT_ALL_EVENTS 0xFFFFFFFF

#define SYSADR( x, y ) (( int ) y | io_get_window_base( x ))


/*
 * Define flags for a simple SGL. This is I2O_SGL_FLAGS_SIMPLE_ADDRESS_ELEMENT
 * | I2O_SGL_FLAGS_NO_CONTEXT | FLAGS_END_OF_BUFFER | FLAGS_LAST_ELEMENT ( 
 * ( these are define in i2omsg.h ).
 */

#define I2O_SGL_SIMPLE_FLAGS 0xD0

/*
 * I2O_NO_SUBCLASS used to match OSM's to peripherals
 */

#define I2O_NO_SUBCLASS 0xFFFFFFFF

#endif

