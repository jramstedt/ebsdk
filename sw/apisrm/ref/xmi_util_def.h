/* file:    xmi_util_def.h
 *
 * Copyright (C) 1991 by
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
 * Abstract:	XMI utility macros for the TURBO console.
 *
 * Author:	jds - Jim Sawin
 *
 */

#include	"cp$src:mbx_def.h"

#define XMI_WRITE_L(reg, buf, hose, mbx) \
   mbx_read_write_reg(MBX_XMI, 'w', reg, sizeof(U_INT_32), buf, hose, mbx)
#define XMI_WRITE_W(reg, buf, hose, mbx) \
   mbx_read_write_reg(MBX_XMI, 'w', reg, sizeof(U_INT_16), buf, hose, mbx)
#define XMI_WRITE_B(reg, buf, hose, mbx) \
   mbx_read_write_reg(MBX_XMI, 'w', reg, sizeof(U_INT_8), buf, hose, mbx)

#define XMI_READ_L(reg, buf, hose, mbx) \
   mbx_read_write_reg(MBX_XMI, 'r', reg, sizeof(U_INT_32), buf, hose, mbx)
#define XMI_READ_W(reg, buf, hose, mbx) \
   mbx_read_write_reg(MBX_XMI, 'r', reg, sizeof(U_INT_16), buf, hose, mbx)
#define XMI_READ_B(reg, buf, hose, mbx) \
   mbx_read_write_reg(MBX_XMI, 'r', reg, sizeof(U_INT_8), buf, hose, mbx)

#define XMI_NODE_BASE(node_id) (0x21800000 + 0x80000*node_id)
