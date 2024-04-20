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
 * Abstract:	World Wide ID Definitions
 *
 * Author:	Dennis Mazur
 *
 * Below are the WWID formats as defined by the UNIX supplied structs.
 * However, for the FC adapter - the only SCSI3 console adapter, we
 * currently have a fixed set of hardware to be supported (Seagates and
 * HSG). Thus, only the following WWID types/lens need to be supported:
 *
 * type                          len             max WWID size
 * ---------------------         ---------       ----------------
 * SCSI_WWID_PG83_IEEE64         N/A             12 bytes
 * SCSI_WWID_PG80_SERIALNO       0-16 bytes      28-44 bytes
 *
 * Therefore each record in the console alias table must be sized at
 * a maximum of 44 bytes.
 *
 * The header (wwid_hdr)is laid out to occupy a single 4 byte area at the 
 * beginning of the WWID:
 *
 * The "type" shall indicate the manner in which the WWID was generated:
 *
 * The "ascii" field shall be set to '1' if the identifier is
 * "ascii" based, and '0' if binary.
 *  
 *  The "len" field shall be set to the length, in bytes, of the
 *  variable length identifier data in the WWID. For example, if the
 *  type is SCSI_WWID_PG80_SERIALNO, "len" shall contain the length
 *  (or number of valid bytes) in the "sn[]" field of the WWID structure.
 *  Note: "len" can encode the full 0-255 byte length of any variable
 *    length data element.
 *  Note: Although 248 bytes of variable information is available, most
 *   devices use less than 20 bytes.
 *
 * Modifications:
 *
 *	dm	16-May-1997	Initial entry.
 */

#if !defined(WWID_HDR)
#define WWID_HDR

#define MAX_FC_ALIAS        5
#define MAX_PORTNAME_ALIAS  4
#define MAX_FC_TEMP_ALIAS   200
#define WWID_NVRAM_SZ       44
#define WWID_ASCII_SZ       2*WWID_NVRAM_SZ
#define WWID_VID_SZ         8
#define WWID_PID_SZ         16
#define MAX_WWID_ALIAS_EV          4
#define MAX_PORTNAME_ALIAS_EV      4

/*
 * Filter masks for search selection.  Used to select which list(s) of 
 * wwid's to look thru or to  register a wwid.
 */
#define WWID$K_USERFILTER 0
#define WWID$K_TEMPFILTER 1


/*
 * WWID structs in common with UNIX
 */

#if !defined(UNIX_TYPEDEFS)
#define UNIX_TYPEDEFS

typedef	unsigned ubit;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;

#endif

#if RAWHIDE
typedef unsigned int uint;
#endif



#define SCSI_WWID_PG83_IEEE128          1
#define SCSI_WWID_PG83_IEEE64           2
#define SCSI_WWID_PG83_VENDOR           3
#define SCSI_WWID_PG80_SERIALNO         4
#define SCSI_WWID_SERIALNO              5       /* inq derived serial/no */
#define SCSI_WWID_TGT_IDENT             6
#define SCSI_WWID_BTL                   7
#define SCSI_WWID_DEC_UNIQUE            8
#define SCSI_WWID_MAX_TYPE              8


typedef struct wwid_hdr {
 ulong len   : 20,		/* Length of WWID (not incl prefix) */
       ascii :  1,		/* WWID is ASCII string if set */
       rsvd  :  3,		/* Reserved flags, MBZ */
       type  :  8;		/* WWID type */
} wwid_hdr_t;

typedef struct wwid_p83_ieee64 {
    wwid_hdr_t        hdr;
    uchar             ieee[8];
} wwid_p83_ieee64_t;

typedef struct wwid_p83_ieee128 {
    wwid_hdr_t        hdr;
    uchar             ieee[16];
} wwid_p83_ieee128_t;

typedef struct wwid_p83_vendor {
    wwid_hdr_t        hdr;
    uchar             pid[16];
    uchar             ieee[248];      /* variable size */
} wwid_p83_vendor_t;

typedef struct wwid_serial_num {
    wwid_hdr_t        hdr;
    uchar             vid[8];
    uchar             pid[16];
    uchar             sn[248];        /* variable size */
} wwid_serial_num_t;

typedef struct wwid_tgt_ident {
    wwid_hdr_t        hdr;
    uchar             vid[8];
    uchar             pid[16];
    uchar             ident[248];     /* variable size */
} wwid_tgt_ident_t;

typedef struct wwid_btl {
    wwid_hdr_t        hdr;
    uchar             vid[8];
    uchar             pid[16];
    uint              bus_no;
    ulong             target_no;
    ulong             lun_no;
} wwid_btl_t;
    
typedef union wwid {
    wwid_hdr_t                hdr;
    wwid_p83_ieee64_t         ieee64;  
    wwid_p83_ieee128_t        ieee128;  
    wwid_p83_vendor_t         vendor;
    wwid_serial_num_t         serialno;
    wwid_tgt_ident_t          tgt_ident;
    wwid_btl_t                physaddr;
} wwid_t;


/*
 * registered_wwid is the structure stored in the console uses.  It has the
 * additional fields that VMS want for a user specified unit number and
 * collision number.  There are also fields to support the wwidmgr.
 */

#define MULTIPATH_MAX 16
struct registered_wwid {
   int		  unit;
   int		  collision;
   wwid_t    	  wwid;
   int	          evnum;
   int            udid;                   /* user defined id from device */
   struct sb	  *sbs[MULTIPATH_MAX];
   int		  lun[MULTIPATH_MAX];
   int		  online[MULTIPATH_MAX];
};


struct registered_portname {
    char	portname[17];
    int		sb_allowed;
    int         evnum;
    struct sb	*sbs[MULTIPATH_MAX];
};
#endif
