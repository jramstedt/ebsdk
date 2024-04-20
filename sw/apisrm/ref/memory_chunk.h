/*
 * Copyright (C) 1998, 1998 by
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
 * Abstract:	Structure definitions that are shared between the
 *		generic bootstrap program and bootstrap front ends.
 *
 * Author:	Harol Buckingham
 *
 * Modifications:
 *
 *      hcb	20-jul-1998	Initial entry
 */
struct CHUNK_MAP
    {
    uint64 base;
    uint64 size;
    uint64 bitmap;
    uint32 flags;
    uint32 owner;
    };

#define CHUNK_FLAG_BASE 0x1
#define CHUNK_FLAG_OWNED 0x2
#define CHUNK_FLAG_SHARED 0x4
#define CHUNK_FLAG_BITMAP 0x8
