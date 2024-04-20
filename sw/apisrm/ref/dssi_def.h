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
 * Abstract:	Digital Small Storage Interface (DSSI) Definitions
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *	sfs	30-May-1990	Initial entry.
 */
 
#define dssi_k_max_pkt 4114

#define dssi_k_cmd_dssi 224

#define dssi_k_sts_bad 30
#define dssi_k_sts_good 97

volatile struct dssi_header {
    unsigned char opcode;
    unsigned mbz : 4;
    unsigned req_ack : 4;
    unsigned char dst_port;
    unsigned char src_port;
    unsigned short int size;
    } ;
