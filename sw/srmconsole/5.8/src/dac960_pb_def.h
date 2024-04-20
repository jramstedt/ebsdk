/*
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
 * and  should  not  be  construed  as a commitment by Digital Equipment
 * Corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by Digital.
 *
 *
 * Abstract:	Port Block (PB) Definitions
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *	sfs	14-Oct-1993	Initial entry.
 */
 
volatile struct dac960_pb {
    struct pb pb;
    struct SEMAPHORE owner_s;
    unsigned long int old;
    unsigned long int board_status;
    unsigned long int csr;
    unsigned long int num_drives;
    unsigned char (*inb)();
    unsigned long (*inl)();
    void (*outb)();
    void (*outl)();
    unsigned long int ldb_xor;
    unsigned long int LDB;
    unsigned long int SDB;
    unsigned long int MBX;
    unsigned long int MBX_STATUS;
    unsigned long int TGT;
    unsigned long int CHN;
    unsigned long int INT;
    unsigned long int new;
    unsigned long int max_channels;
    } ;
