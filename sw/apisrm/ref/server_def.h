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
 * Abstract:	Server Definitions
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *	sfs	03-Jan-1991	Initial entry.
 */
 
#define max_servers 2

struct server {
    unsigned char *src_proc;
    unsigned char *src_data;
    int (*notify)();
    } ;

#define directory_k_normal 1
#define directory_k_nomatch 10

volatile struct directory_req {
    unsigned short int form;
    unsigned short int entry;
    unsigned char proc [16];
    } ;

volatile struct directory_rsp {
    unsigned short int status;
    unsigned short int entry;
    unsigned char proc [16];
    unsigned char data [16];
    } ;
