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
 * Abstract:	Port Block (PB) Definitions
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *	jeg	 7-Jan-1994	add e_memq.
 *
 *	joes	12-Oct-1993	add bus and function fields.
 *
 *	jeg	27-Aug-1993	add id field.
 *
 *	jrk	12-May-1992	new driver scheme.
 *
 *	jds	13-Feb-1992	Added ref and state.
 *
 *	sfs	15-Oct-1990	Initial entry.
 */

typedef struct _E_MEMQ {
    struct _E_MEMQ *flink;
    struct _E_MEMQ *blink;
    unsigned long addr;
    unsigned long size;
    } E_MEMQ;
 
volatile struct pb {
    int (*setmode)();
    unsigned long int ref;
    unsigned long int state;
    unsigned long int mode;
    unsigned long int desired_mode;
    unsigned long int hose;
    unsigned long int bus;
    unsigned long int slot;
    unsigned long int function;
    unsigned long int channel;
    unsigned char *protocol;
    unsigned long int controller;
    unsigned char controller_id [4];
    unsigned char port_name [32];
    unsigned long int type;
    unsigned long int csr;
    unsigned long int vector;
    unsigned char *device;
    unsigned long int *letter;
    void *misc;
    void *pdm;
    void *config_device;
    unsigned long int order;
    struct sb **sb;
    unsigned long int num_sb;
    unsigned char name [32];
    unsigned char alias [8];
    unsigned char info [24];
    unsigned char version [16];
    unsigned char string [80];
    unsigned char id [20];
    unsigned char *hardware_name;
    E_MEMQ e_memq;
    int status;
    struct POLLQ pq;
    struct pb *parent;
#if DRIVERSHUT || RAWHIDE
    struct io_device *tbl;
    struct pb *dpb;
#endif
    } ;

