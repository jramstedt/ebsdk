/*
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
 * and  should  not  be  construed  as a commitment by Digital Equipment
 * Corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by Digital.
 *
 *
 * Abstract:	Unit Block (UB) Definitions
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *	jeg	27-Aug-1993	add id field.
 *
 *	pel	22-Dec-1992	add pointer to inquiry response data
 *
 *	sfs	30-May-1990	Initial entry.
 */
 
#define MAX_INQ_SZ 56
#define MAX_EXT_INQ_SZ 255
#define MAX_SENSE_SZ 18

volatile struct ub {
    struct QUEUE ub;
    unsigned long int ref;
    unsigned long int incarn;
    struct cb *cb;
    struct sb *sb;
    struct pb *pb;
    struct INODE *inode;
    unsigned long int unit;
    unsigned long int flags;
    unsigned char alias [32];
    unsigned char info [32];
    unsigned char version [16];
    unsigned char string [80];
    unsigned char id [8];	/* configuration tree Identifier */
    unsigned char dd [3];
    unsigned char pad [1];
    unsigned char *inq;
    unsigned int inq_len;
    unsigned char *wwid;
    unsigned short int shdw_unit;
    unsigned short int shdw_sts;
    int allocation_class;
    int multibus;
    } ;
