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
 * Abstract:	System Block (SB) Definitions
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *	dtr	5-Feb-1999	Added block size to ata_sb for support of iomega
 *				zip drives which have 512 byte blocks
 *
 *	sfs	30-May-1990	Initial entry.
 */
 
#define sb_k_closed 0
#define sb_k_start_out 1
#define sb_k_start_in 2
#define sb_k_open 3
#define sb_k_stall 4

#define include_struct_sb \
    struct pb *pb; \
    struct ub *first_ub; \
    unsigned long int state; \
    unsigned char name [32]; \
    unsigned char alias [16]; \
    unsigned char string [80]; \
    unsigned int class;

#define include_struct_mscp_sb \
    include_struct_sb \
    struct QUEUE cb; \
    int (*connect)(); \
    int (*accept)(); \
    int (*alloc)(); \
    int (*map)(); \
    int (*unmap)(); \
    int (*send_data)(); \
    int (*request_data)(); \
    int (*break_vc)(); \
    int (*send_scs_msg)(); \
    int (*conn_open)(); \
    int (*conn_close)(); \
    unsigned long int novc; \
    unsigned long int vc; \
    unsigned long int local; \
    unsigned long int port_type; \
    unsigned long int poll_count; \
    unsigned long int poll_delay; \
    unsigned long int poll_delay2; \
    unsigned long int poll_active; \
    unsigned long int poll_in_prog; \
    unsigned long int node_id; \
    unsigned char info [16]; \
    unsigned char version [16]; \
    unsigned char node_name [9]; \
    unsigned char system_id [6]; \
    unsigned char pad [1];

#define include_struct_ext_mscp_sb \
    struct SEMAPHORE ready_s; \
    struct SEMAPHORE disk_s; \
    struct SEMAPHORE tape_s; \
    unsigned long int ok; \
    unsigned long int stallvc; \
    unsigned long int dqi; \
    unsigned long int dssi_disk; \
    unsigned long int dssi_tape; \
    unsigned long int verbose; \
    unsigned long int pdevice; \
    unsigned long int ackrec; \
    unsigned long int conn_count; \
    unsigned long int send_active; \
    unsigned long int xport_fcn_ext[2]; \
    unsigned long int swtype; \
    unsigned long int ref;

#define include_struct_scsi_sb \
    include_struct_sb \
    struct QUEUE ub; \
    int (*command)(); \
    int (*send_inquiry)(); \
    unsigned long int ok; \
    unsigned long int local; \
    unsigned long int sdtr; \
    unsigned long int node_id; \
    unsigned long int lun_map;

#define include_struct_ext_scsi_sb \
    struct SEMAPHORE ready_s; \
    struct SEMAPHORE disk_s; \
    struct SEMAPHORE tape_s; \
    unsigned long int verbose; \
    unsigned long int pdevice; \
    unsigned long int ref;

/*
 *  The structure below has some fibre channel specific fields.  I hate
 *  to do that, but since devices dont all give wwid's, the contrived
 *  wwid will come from the portname possibly.  I'll get back here.
 */
#define include_struct_ext_scsi3_sb     \
    int		    lun_ext[8];         \
    char	nodename[20];		\
    char	portname[20];		\
   

volatile struct sb {
    include_struct_sb
    } ;

volatile struct mscp_sb {
    include_struct_mscp_sb
#if MODULAR || STARTSHUT || DRIVERSHUT
    include_struct_ext_mscp_sb
#endif
    } ;

volatile struct scsi_sb {
    include_struct_scsi_sb
#if MODULAR || STARTSHUT || DRIVERSHUT
    include_struct_ext_scsi_sb
#endif
    } ;

volatile struct scsi3_sb {
    include_struct_scsi_sb
#if MODULAR || STARTSHUT || DRIVERSHUT
    include_struct_ext_scsi_sb
#endif
    include_struct_ext_scsi3_sb
    } ;



volatile struct cipca_sb {
    include_struct_mscp_sb
#if MODULAR || STARTSHUT || DRIVERSHUT
    include_struct_ext_mscp_sb
#else
    struct SEMAPHORE ready_s;
    struct SEMAPHORE disk_s;
    struct SEMAPHORE tape_s;
    unsigned long int ok;
    unsigned long int stallvc;
    unsigned long int dqi;
    unsigned long int verbose;
    unsigned long int pdevice;
    unsigned long int ackrec;
    unsigned long int send_active;
    unsigned long int xport_fcn_ext[2];
    unsigned long int swtype;
    unsigned long int ref;
#endif
    } ;

volatile struct pue_sb {
    include_struct_mscp_sb
#if MODULAR || STARTSHUT || DRIVERSHUT
    include_struct_ext_mscp_sb
#endif
    unsigned long int ns;
    unsigned long int nr;
    } ;

volatile struct put_sb {
    include_struct_mscp_sb
#if MODULAR || STARTSHUT || DRIVERSHUT
    include_struct_ext_mscp_sb
#else
    unsigned long int ok;
#endif
    unsigned long int ns;
    unsigned long int nr;
    } ;

volatile struct ata_sb {
    include_struct_scsi_sb
#if MODULAR || STARTSHUT || DRIVERSHUT
    include_struct_ext_scsi_sb
#endif
    unsigned long int atapi;
    void *identify;
    unsigned int block_size;	/* needed by atapi devices as some */
				/* devices are 512 byte blocks and */
				/* note the 2048 of the cdrom */

    } ;
