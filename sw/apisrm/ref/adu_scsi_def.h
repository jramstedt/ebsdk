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
 * Abstract:	ADU's SCSI Definitions
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *	sfs	20-Nov-1990	Initial entry.
 */

volatile struct scsi_icr
    {
    unsigned ie	: 1;
    unsigned node : 4;
    unsigned irq : 5;
    };

volatile struct scsi_cmd
    {
    unsigned long int flag;
    unsigned long int fill0[7];
    unsigned long int cmdtag[2];
    unsigned long int target;
    unsigned long int bus;
    unsigned long int nmsg;
    unsigned long int ncmd;
    unsigned long int options;
    unsigned long int timeout;
    unsigned long int dataaddr;
    unsigned long int dataaddr_h;
    unsigned long int ndata;
    unsigned long int fill1[5];
    unsigned char msg[8];
    unsigned char cmd[16];
    unsigned long int fill2[2];
    };

volatile struct scsi_rsp
    {
    unsigned long int flag;
    unsigned long int fill0[7];
    unsigned long int cmdtag[2];
    unsigned long int cstatus;
    unsigned long int ndata;
    unsigned long int nsstatus;
    unsigned long int fill1[3];
    unsigned char sstatus[8];
    unsigned long int fill2[14];
    };

volatile struct ring {
    struct scsi_cmd scsi_cmd[32];
    struct scsi_rsp scsi_rsp[32];
    } ;
