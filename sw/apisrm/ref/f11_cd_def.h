/********************************************************************************************************************************/
/* Created 30-APR-1993 11:03:22 by VAX SDL V3.2-12     Source:  2-DEC-1991 20:57:56 VMS_REF$:[F11CD]F11CD.SDL;1 */
/********************************************************************************************************************************/
 
/*** MODULE $F11CD IDENT X-1 ***/
/*                                                                          */
/* File sections shall be recorded in the Data Area. The following types of */
/* descriptors shall be record in the data area of a volume.                */
/*                                                                          */
/*	o Volume Descriptors                                                */
/*	    o Boot Record (BOOT)                                            */
/*	    o Volume Descriptor Set Terminator (VDST)                       */
/*	    o Primary Volume Descriptor (PVD)                               */
/*	    o Supplementary Volume Descriptor (SVD)                         */
/*	    o Volume Partition Descriptor (VPD)                             */
/*	o File Descriptors                                                  */
/*	o Directory Descriptors                                             */
/*	o Path Tables                                                       */
/*                                                                          */
/*                                                                          */
/* ISO 9660 standard constants                                              */
/*                                                                          */
#define LOGICAL_BLOCK_SIZE 2048         /* Current Volume Descriptor Block Size */
#define SYSTEM_AREA 0                   /* Logical Block number of System Area */
#define DATA_AREA 16                    /* Logical Block number of Data Area */
#define ISO_9660_VERSION_1988 1         /* Volume Descriptor Version # for ISO 9660 (1988) */
/*                                                                          */
/* Character set coding                                                     */
/*                                                                          */
#define ISO_9660$K_SPACE 32             /* ' ' fill character               */
#define ISO_9660$K_SEPARATOR_1 46       /* '.'-filename.extension break     */
#define ISO_9660$K_SEPARATOR_2 59       /* ';'-extension;version break      */
#define ISO_9660$K_FILE_ID 95           /* '_' file identifier character    */
#define ISO_9660$K_ROOT_DIRECTORY_ID 0  /* Root Directory Identifier        */
#define ISO_9660$K_PARENT_DIRECTORY_ID 1 /* Parent Directory Identifier     */
/*+                                                                         */
/* FILE_ID                                                                  */
/*                                                                          */
/*  A File Identifier consist of the following sequence:                    */
/*	- File name : A sequence of 0 -> 30 'D' or 'D1' characters          */
/*	- SEPARATOR_1                                                       */
/*	- Extension : A sequence of 0 -> 30 'D' or 'D1' characters          */
/*	- SEPARATOR_2                                                       */
/*	- Version # : A sequence of 1 to 5 digits; up to "32767"            */
/*  If 'File Name' length equals 0, then Extension must be 1 or greater     */
/*  If 'Extension' length equals 0, then File Name must be 1 or greater     */
/*  The length of 'File Name' + 'Extension' must not exceed 30.             */
/*-                                                                         */
struct FILE_ID {
    char FNAME$T_FILE_NAME_EXT [37];    /* filename.ext;version             */
    } ;
/*+                                                                         */
/* Identifier                                                               */
/*                                                                          */
/*  This descriptor shall specify an identifier. If the first byte is equal */
/*  to a %X05F, the remaining bytes of this field shall specific an         */
/*  identifier for a file containing the identification text. This file     */
/*  shall be described in the Root Directory. The File Name shall not       */
/*  contain more than 8 D_CHARACTERS and the File Extension shall not       */
/*  contain more than 3 D_CHARACTERS. If all bytes in this field are set to */
/*  %X20, it shall mean that no such identifier is defined.                 */
/*-                                                                         */
union IDENTIFIER {
    variant_struct  {
        unsigned char ID$B_FLAG;        /* File specific flag               */
        char ID$T_FILE_NAME_EXT [37];   /* File Identifier (filename.ext;version) */
        } ID$R_ENCODED_ID;
    char ID$B_DATA [128];               /* Text Identifier                  */
    } ;
/*+                                                                         */
/* ASCII_DATE_TIME                                                          */
/*                                                                          */
/*  The date and time shall be represented by a 17-byte field.              */
/*  If Byte positions 1-16 are the digit '0', and BP 17 equals 0, then      */
/*  it shall mean that date and time are not specified.                     */
/*-                                                                         */
struct ASCII_DATE_TIME {
    char AUTC$T_YEAR [4];               /* year (1-9999)                    */
    char AUTC$T_MONTH [2];              /* month (1-12)                     */
    char AUTC$T_DAY [2];                /* day (1-31)                       */
    char AUTC$T_HOUR [2];               /* hour (0-23)                      */
    char AUTC$T_MINUTE [2];             /* minute (0-59)                    */
    char AUTC$T_SECOND [2];             /* second (0-59)                    */
    char AUTC$T_HUNDREDTH [2];          /* hundredth (0-99)                 */
    char AUTC$B_OFFSET_GMT;             /* ¼ hr. intervals (-48:+52))       */
    } ;
/*+                                                                         */
/* BINARY_DATE_TIME                                                         */
/*                                                                          */
/*  The data and time shall be represented by seven 8-bit numbers           */
/*-                                                                         */
struct BINARY_DATE_TIME {
    unsigned char BUTC$B_YEAR;          /* year since 1900 (1-99)           */
    unsigned char BUTC$B_MONTH;         /* month (1-12)                     */
    unsigned char BUTC$B_DAY;           /* day (1-31)                       */
    unsigned char BUTC$B_HOUR;          /* hour (0-23)                      */
    unsigned char BUTC$B_MINUTE;        /* minute (0-59)                    */
    unsigned char BUTC$B_SECOND;        /* second (0-59)                    */
    char BUTC$B_OFFSET_GMT;             /* ¼ hr. intervals from -48(west) to +52(east) */
    } ;
/*+                                                                         */
/*  PATH_TABLE_RECORD                                                       */
/*                                                                          */
/*  A Path Table contain a set of records describing a directory hierarchy  */
/*  for those volume of a Volume Set the sequence numbers of which are less */
/*  than, or equal to, the assigned Volume Set size of the volume.          */
/*                                                                          */
/*  For each directory in the directory hierarchy other then the Root       */
/*  Directory, the Path Table shall contain a record which identifies       */
/*  the directory, its Parent Directory and its location. The records       */
/*  in a Path Table shall be number starting at 1. The first record in      */
/*  the Path Table shall identify the Root Directory and it location.       */
/*                                                                          */
/*-                                                                         */
struct PATH_TABLE_RECORD {
    unsigned char PTBL$B_DIRECTORY_ID_LENGTH; /* Length of directory Identifier */
    unsigned char PTBL$B_XAR_LENGTH;    /* Extended Attribute Record length */
    unsigned long int PTBL$L_EXTENT_LOCATION; /* Location of Extent         */
    unsigned short int PTBL$W_PARENT_DIRECTORY; /* Parent Directory Number  */
    char PTBL$T_DIRECTORY_ID [37];      /* Directory Identifier (dirname)   */
    } ;
/*+                                                                         */
/* DIRECTORY                                                                */
/*  This descriptor shall define a directory record. A directory record     */
/*  contains information to locate a File Section; an Extended Attribute    */
/*  Record associated with a File Section; the identification of a file;    */
/*  attributes of a file and file section.                                  */
/*-                                                                         */
#define DREC$M_EXISTENCE 1
#define DREC$M_DIRECTORY 2
#define DREC$M_ASSOCIATED 4
#define DREC$M_RECORD 8
#define DREC$M_PROTECTION 16
#define DREC$M_RESERVED 96
#define DREC$M_MULTI_EXTENT 128
#define DREC$A_PADDING 34               /* Pad byte                         */
#define DREC$A_SYSTEM_USE 34            /* System use                       */
struct DIRECTORY {
    unsigned char DREC$B_DIRECTORY_LENGTH; /* Length of directory record    */
    unsigned char DREC$B_XAR_LENGTH;    /* Extended Attribute Length        */
    unsigned long int DREC$L_EXTENT_LOCATION; /* Location of Extent (LBN)   */
    unsigned long int DREC$L_EXTENT_LOCATION_M; /* Location of Extent (LBN) */
    unsigned long int DREC$L_DATA_LENGTH; /* Data Length of File Section    */
    unsigned long int DREC$L_DATA_LENGTH_M; /* Data Length of File Section  */
    char DREC$B_FILE_RECORDING [7];     /* Recording Date/Time of extent    */
    variant_union  {
        unsigned char DREC$B_FILE_FLAGS; /* File characteristics            */
        variant_struct  {
            unsigned DREC$V_EXISTENCE : 1; /* If set; nonexistent           */
            unsigned DREC$V_DIRECTORY : 1; /* If set; directory record      */
            unsigned DREC$V_ASSOCIATED : 1; /* If set; associated file      */
            unsigned DREC$V_RECORD : 1; /* If set; record format via XAR.RFM */
            unsigned DREC$V_PROTECTION : 1; /* If set; enforce protection   */
            unsigned DREC$V_RESERVED : 2; /* Reserved                       */
            unsigned DREC$V_MULTI_EXTENT : 1; /* If set; extend record      */
            } DREC$R_FILE_FLAGS_BITS;
        } DREC$R_FILE_FLAGS_OVERLAY;
    unsigned char DREC$B_FILE_UNIT_SIZE; /* Interleave File Unit size       */
    unsigned char DREC$B_INTERLEAVE_GAP; /* Interleave Gap size             */
    unsigned short int DREC$W_VOLUME_NUMBER; /* Volume Sequence # of extent */
    unsigned short int DREC$W_VOLUME_NUMBER_M; /* Volume Sequence # of extent */
    unsigned char DREC$B_FILE_ID_LENGTH; /* File Identifier Field Length    */
    char DREC$B_FILE_ID [1];                /* File Identifier                  */
    } ;
/*+                                                                         */
/* XAR_RECORD                                                               */
/*  This descriptor shall define an Extended Attribute Record. An           */
/*  extended attribute record contains addition information which           */
/*  is associated to a File Section.                                        */
/*                                                                          */
/*-                                                                         */
#define XAR$M_SYS_NO_READ 1
#define XAR$M_SYS_NO_EXECUTE 4
#define XAR$M_OWN_NO_READ 16
#define XAR$M_OWN_NO_EXECUTE 64
#define XAR$M_GRP_NO_READ 256
#define XAR$M_GRP_NO_EXECUTE 1024
#define XAR$M_WLD_NO_READ 4096
#define XAR$M_WLD_NO_EXECUTE 16384
#define XAR_RFM$K_UNDEFINED 0
#define XAR_RFM$K_FIXED 1
#define XAR_RFM$K_LSB_VARIABLE 2
#define XAR_RFM$K_MSB_VARIABLE 3
#define XAR_RFM_SYS$K_UNDEFINED 128
#define XAR_RFM_SYS$K_FIXED 129
#define XAR_RFM_SYS$K_VARIABLE 130
#define XAR_RFM_SYS$K_VFC 131
#define XAR_RFM_SYS$K_STREAM 132
#define XAR_RFM_SYS$K_STREAMLF 133
#define XAR_RFM_SYS$K_STREAMCR 134
#define XAR_ATR$K_CRLF 0
#define XAR_ATR$K_FTN 1
#define XAR_ATR$K_STM 2
#define XAR$A_ESCAPE_SEQUENCE 250       /* Escape Sequences                 */
struct XAR_RECORD {
    unsigned short int XAR$W_OWNER_ID;  /* Owner Identification             */
    unsigned short int XAR$W_OWNER_ID_M; /* Owner Identification            */
    unsigned short int XAR$W_GROUP_ID;  /* Group Identification             */
    unsigned short int XAR$W_GROUP_ID_M; /* Group Identification            */
    variant_union  {
        unsigned short int XAR$W_PERMISSIONS; /* Access permission for classes of users */
        variant_struct  {
            unsigned XAR$V_SYS_NO_READ : 1; /* If set; ~(S:R)               */
            unsigned XAR$V_1 : 1;        /* Must be set to 1                 */
            unsigned XAR$V_SYS_NO_EXECUTE : 1; /* If set; ~(S:E)            */
            unsigned XAR$V_2 : 1;        /* Must be set to 1                 */
            unsigned XAR$V_OWN_NO_READ : 1; /* If set; ~(O:R)               */
            unsigned XAR$V_3 : 1;        /* Must be set to 1                 */
            unsigned XAR$V_OWN_NO_EXECUTE : 1; /* If set; ~(O:E)            */
            unsigned XAR$V_4 : 1;        /* Must be set to 1                 */
            unsigned XAR$V_GRP_NO_READ : 1; /* If set; ~(G:R)               */
            unsigned XAR$V_5 : 1;        /* Must be set to 1                 */
            unsigned XAR$V_GRP_NO_EXECUTE : 1; /* If set; ~(G:E)            */
            unsigned XAR$V_6 : 1;        /* Must be set to 1                 */
            unsigned XAR$V_WLD_NO_READ : 1; /* If set; ~(W:R)               */
            unsigned XAR$V_7 : 1;        /* Must be set to 1                 */
            unsigned XAR$V_WLD_NO_EXECUTE : 1; /* If set; ~(W:E)            */
            unsigned XAR$V_8 : 1;        /* Must be set to 1                 */
            } XAR$R_PERMISSIONS_BITS;
        } XAR$R_PERMISSIONS_OVERLAY;
    char XAR$B_FILE_CREATION [17];      /* File Creation Date/Time          */
    char XAR$B_FILE_MODIFICATION [17];  /* File Modification Date/Time      */
    char XAR$B_FILE_EXPIRATION [17];    /* File Expiration Date/Time        */
    char XAR$B_FILE_EFFECTIVE [17];     /* File Effective Date/Time         */
    unsigned char XAR$B_RECORD_FORMAT;  /* Record Format                    */
    unsigned char XAR$B_RECORD_ATTRIBUTES; /* Record Attributes             */
    unsigned short int XAR$W_RECORD_LENGTH; /* Record Length                */
    unsigned short int XAR$W_RECORD_LENGTH_M; /* Record Length              */
    char XAR$T_SYSTEM_ID [32];          /* System Identifier                */
    char XAR$B_SYSTEM_USE [64];         /* System Used                      */
    unsigned char XAR$B_XAR_VERSION;    /* Extended Attribute Version       */
    unsigned char XAR$B_ESCAPE_SEQ_LENGTH; /* Escape Sequence record length */
    char XAR$B_RESERVED [64];           /* Reserved                         */
    unsigned short int XAR$W_APPLICATION_USE; /* Application Use Length     */
    unsigned short int XAR$W_APPLICATION_USE_M; /* Application Use Length   */
    char XAR$B_APPLICATION_USE [262];   /* Application Use                  */
    } ;
/*+                                                                         */
/* Volume Descriptor                                                        */
/*                                                                          */
/*  The Volume Descriptor shall Identify the volume, the partitions recorded */
/*  on the volume, the volume creator(s),  certain attributes of the volume, */
/*  the location of other recorded descriptors and the version of the       */
/*  standard which applies to the volume descriptor.                        */
/*                                                                          */
/*-                                                                         */
#define VD$K_BOOT 0                     /*	Boot Record Descriptor      */
#define VD$K_PVD 1                      /*	Primary Volume Descriptor   */
#define VD$K_SVD 2                      /*	Supplementary Volume Descriptor */
#define VD$K_VPD 3                      /*	Volume Partition Descriptor */
/*						(Values 4 to 254 are reserved) */
#define VD$K_VDST 255                   /*	Volume Descriptor Set Terminator */
struct VD {
    unsigned char VD$B_VOLUME_DESCRIPTOR_TYPE; /* Volume Descriptor Type    */
    char VD$T_STANDARD_IDENTIFIER [5];  /* International Standard Id. (CD001) */
    unsigned char VD$B_VOLUME_DESCRIPTOR_VERS; /* Volume Descriptor Version */
    char VD$B_VOLUME_DATA [2041];       /* Volume Descriptor Data           */
    } ;
/*+                                                                         */
/* Boot Record (BOOT)                                                       */
/*                                                                          */
/*  The Boot Record shall Identify a system which can recognize and act upon */
/*  the content of the field reserved for boot system use in the Boot       */
/*  Record, and shall contain information which is used to achieve a        */
/*  specific state for a system or for an application.                      */
/*                                                                          */
/*-                                                                         */
struct BOOT {
    char BOOT$B_BOOT_VOLUME [7];        /* Boot Volume Descriptor           */
    char BOOT$T_SYSTEM_IDENTIFIER [32]; /* Boot System Identifier           */
    char BOOT$T_IDENTIFIER [32];        /* Boot Identifier                  */
    char BOOT$B_SYSTEM_USE [1977];      /* Boot System Use                  */
    } ;
/*+                                                                         */
/* Volume Descriptor Set Terminator                                         */
/*                                                                          */
/*  The recorded set of Volume Descriptors shall be terminated by a sequence */
/*  of one or more Volume Descriptor Set Terminators                        */
/*                                                                          */
/*-                                                                         */
struct VDST {
    char VDST$B_TERMINATOR_VOLUME [7];  /* Volume Descriptor Set            */
    char VDST$B_RESERVED [2041];        /* Reserved                         */
    } ;
/*+                                                                         */
/* Primary Volume Descriptor                                                */
/*                                                                          */
/*  The Primary Volume Descriptor shall Identify the volume, a system which */
/*  can recognize and act upon the content of the Logical Sectors with      */
/*  Logical Sector Number 0 to 15, the size of the Volume Space, the version */
/*  of the  standard which applies to the Volume Descriptor, the version of */
/*  the specification which applies to the Directory Records and the Path   */
/*  Table Records and certain attributes of the volume.                     */
/*                                                                          */
/*-                                                                         */
struct PVD {
    char PVD$B_PRIMARY_VOLUME [7];      /* Primary Volume Descriptor        */
    char PVD$B_UNUSED_FIELD1;            /* Unused field                     */
    char PVD$T_SYSTEM_IDENTIFIER [32];  /* System Identifier                */
    char PVD$T_VOLUME_IDENTIFIER [32];  /* Volume Identifier                */
    char PVD$B_UNUSED_FIELD2 [8];        /* Unused field                     */
    unsigned long int PVD$L_VOLUME_SPACE_SIZE; /* Volume Space Size         */
    unsigned long int PVD$L_VOLUME_SPACE_SIZE_M; /* Volume Space Size       */
    char PVD$B_UNUSED_FIELD3 [32];       /* Unused field                     */
    unsigned short int PVD$W_VOLUME_SET_SIZE; /* Volume Set Size            */
    unsigned short int PVD$W_VOLUME_SET_SIZE_M; /* Volume Set Size          */
    unsigned short int PVD$W_VOLUME_NUMBER; /* Volume Sequence Number       */
    unsigned short int PVD$W_VOLUME_NUMBER_M; /* Volume Sequence Number     */
    unsigned short int PVD$W_LOGICAL_BLOCK_SIZE; /* Logical Block Size      */
    unsigned short int PVD$W_LOGICAL_BLOCK_SIZE_M; /* Logical Block Size    */
    unsigned long int PVD$L_PATH_TABLE_SIZE; /* Path Table Size             */
    unsigned long int PVD$L_PATH_TABLE_SIZE_M; /* Path Table Size           */
    unsigned long int PVD$L_PATH_TABLE; /* Path Table Logical Block #       */
    unsigned long int PVD$L_OPT_PATH_TABLE; /* Optional Path Table Logical Block # */
    unsigned long int PVD$L_PATH_TABLE_M; /* Path Table Logical Block #     */
    unsigned long int PVD$L_OPT_PATH_TABLE_M; /* Optional Path Table Logical Block # */
    char PVD$B_ROOT_DIRECTORY [34];     /* Root Directory Record            */
    char PVD$T_VOLUME_SET_IDENTIFIER [128]; /* Volume Set Identifier        */
    char PVD$B_PUBLISHER_ID [128];      /* Publisher Identifier             */
    char PVD$B_DATA_PREPARER_ID [128];  /* Data Preparer Identifier         */
    char PVD$B_APPLICATION_ID [128];    /* Application Identifier           */
    char PVD$B_COPYRIGHT_FILE_ID [37];  /* Copyright File Identifier        */
    char PVD$B_ABSTRACT_FILE_ID [37];   /* Abstract File Identifier         */
    char PVD$B_BIBLIOGRAPHIC_ID [37];   /* Bibliographic File Identifier    */
    char PVD$B_VOLUME_CREATION [17];    /* Volume Creation Date/Time        */
    char PVD$B_VOLUME_MODIFIY [17];     /* Volume Modification Date/Time    */
    char PVD$B_VOLUME_EXPIRATION [17];  /* Volume Expiration Date/Time      */
    char PVD$B_VOLUME_EFFECTIVE [17];   /* Volume Effective Date/Time       */
    unsigned char PVD$B_FILE_STRUCTURE_VERS; /* File Structure Version      */
    char PVD$B_RESERVED1;                /* Reserved                         */
    char PVD$B_APPLICATION_USE [512];   /* Application Use field            */
    char PVD$B_RESERVED2 [653];          /* Reserved                         */
    } ;
/*+                                                                         */
/* Supplementary Volume Descriptor                                          */
/*                                                                          */
/*  The Supplementary Volume Descriptor shall Identify the volume, a system */
/*  which can recognize and act upon the content of the Logical Sectors with */
/*  Logical Sector Number 0 to 15, the size of the Volume Space, the version */
/*  of the  standard which applies to the Volume Descriptor, the version of */
/*  the specification which applies to the Directory Records and the Path   */
/*  Table Records, certain attributes of the volume and the coded graphic   */
/*  character sets used to interpret descriptor fields that contain         */
/*  characters.                                                             */
/*                                                                          */
/*-                                                                         */
#define SVD$M_NON_ISO_2375 1
struct SVD {
    char SVD$B_SUPPLEMENTARY_VOLUME [7]; /* Supplementary Volume Descriptor */
    variant_union  {
        unsigned char SVD$B_VOLUME_FLAGS; /* Volume characteristics         */
        variant_struct  {
            unsigned SVD$V_NON_ISO_2375 : 1; /* If set; Escape Sequence is non ISO-2375 compliant */
            unsigned SVD$V_fill_0 : 7;
            } SVD$R_VOLUME_FLAGS_BITS;
        } SVD$R_VOLUME_FLAGS_OVERLAY;
    char SVD$T_SYSTEM_IDENTIFIER [32];  /* System Identifier                */
    char SVD$T_VOLUME_IDENTIFIER [32];  /* Volume Identifier                */
    char SVD$B_UNUSED [8];              /* Unused Field                     */
    unsigned long int SVD$L_VOLUME_SPACE_SIZE; /* Volume Space Size         */
    unsigned long int SVD$L_VOLUME_SPACE_SIZE_M; /* Volume Space Size       */
    char SVD$B_ESCAPE_SEQUENCES [32];   /* Escape Sequences ISO 2022 for G0, G1 */
    unsigned short int SVD$W_VOLUME_SET_SIZE; /* Volume Set Size            */
    unsigned short int SVD$W_VOLUME_SET_SIZE_M; /* Volume Set Size          */
    unsigned short int SVD$W_VOLUME_NUMBER; /* Volume Sequence Number       */
    unsigned short int SVD$W_VOLUME_NUMBER_M; /* Volume Sequence Number     */
    unsigned short int SVD$W_LOGICAL_BLOCK_SIZE; /* Logical Block Size      */
    unsigned short int SVD$W_LOGICAL_BLOCK_SIZE_M; /* Logical Block Size    */
    unsigned long int SVD$L_PATH_TABLE_SIZE; /* Path Table Size             */
    unsigned long int SVD$L_PATH_TABLE_SIZE_M; /* Path Table Size           */
    unsigned long int SVD$L_PATH_TABLE; /* Path Table Logical Block #       */
    unsigned long int SVD$L_OPT_PATH_TABLE; /* Optional Path Table Logical Block # */
    unsigned long int SVD$L_PATH_TABLE_M; /* Path Table Logical Block #     */
    unsigned long int SVD$L_OPT_PATH_TABLE_M; /* Optional Path Table Logical Block # */
    char SVD$B_ROOT_DIRECTORY [34];     /* Root Directory Record            */
    char SVD$T_VOLUME_SET_IDENTIFIER [128]; /* Volume Set Identifier        */
    char SVD$B_PUBLISHER_ID [128];      /* Publisher Identifier             */
    char SVD$B_DATA_PREPARER_ID [128];  /* Data Preparer Identifier         */
    char SVD$B_APPLICATION_ID [128];    /* Application Identifier           */
    char SVD$B_COPYRIGHT_FILE_ID [37];  /* Copyright File Identifier        */
    char SVD$B_ABSTRACT_FILE_ID [37];   /* Abstract File Identifier         */
    char SVD$B_BIBLIOGRAPHIC_ID [37];   /* Bibliographic File Identifier    */
    char SVD$B_VOLUME_CREATION [17];    /* Volume Creation Date/Time        */
    char SVD$B_VOLUME_MODIFIY [17];     /* Volume Modification Date/Time    */
    char SVD$B_VOLUME_EXPIRATION [17];  /* Volume Expiration Date/Time      */
    char SVD$B_VOLUME_EFFECTIVE [17];   /* Volume Effective Date/Time       */
    unsigned char SVD$B_FILE_STRUCTURE_VERS; /* File Structure Version      */
    char SVD$B_RESERVED1;                /* Reserved                         */
    char SVD$B_APPLICATION_USE [512];   /* Application Use field            */
    char SVD$B_RESERVED2 [653];          /* Reserved                         */
    } ;
/*+                                                                         */
/* Volume Partition Descriptor                                              */
/*                                                                          */
/*  The Volume Partition Descriptor shall identify a volume partition with  */
/*  the Volume Space, a system which can recognize and act upon the content */
/*  of fields reserved for system use in the Volume Descriptor, the position */
/*  and size of the volume partition, the version of the standard which     */
/*  applies to the Volume Descriptor.                                       */
/*                                                                          */
/*-                                                                         */
struct VPD {
    char VPD$B_VOLUME_PARTITION [7];    /* Volume Partition Descriptor      */
    char VPD$B_UNUSED;                  /* Unused field                     */
    char VPD$T_SYSTEM_IDENTIFIER [32];  /* System Identifier                */
    char VPD$T_PARTITION_IDENTIFIER [32]; /* Volume Partition Identifier    */
    unsigned long int VPD$L_PARTITION_LOCATION; /* Location of Partition (LBN) */
    unsigned long int VPD$L_PARTITION_LOCATION_M; /* Location of Partition (LBN) */
    unsigned long int VPD$L_PARTITION_SIZE; /* Volume Partition Size        */
    unsigned long int VPD$L_PARTITION_SIZE_M; /* Volume Partition Size      */
    char VPD$B_SYSTEM_USE [1960];       /* System Used                      */
    } ;
/* Digital System Identifier - (defacto standard)                           */
/*                                                                          */
/* An ISO 9660 CD-ROM, may contain information written according to DIGITAL */
/* specifications. Theses specifications would be identified by a DIGITAL System */
/* Identifier (DSI) record in the XAR record associated with some or all files.  */
/*                                                                          */
/* The DIGITAL System Identifier, may be used to indicate that there are  specific */
/* system use fields which may contain VMS specific information such as RMS File */
/* Attribute Blocks, User Identification Codes and file protection masks.   */
/*                                                                          */
/*-                                                                         */
struct DSI {
    char DSI$T_VENDOR_IDENTIFIER [12];  /* Vendor Identifier                */
    char DSI$T_SOFTWARE_IDENTIFIER [12]; /* Operating System Identifier     */
    char DSI$T_FILE_SYSTEM_IDENTIFIER [4]; /* File System Identifier        */
    char DSI$T_FILE_SYSTEM_VERSION [4]; /* File System Version              */
    } ;
/*                                                                          */
/* Well Known "Digital System Identifiers"                                  */
/*	                                                                    */
/*	00000000011111111112222222222333  [Position]                        */
/*	12345678901234567890123456789012                                    */
/*      --------------------------------                                    */
/*     'DEC         VMS         0   0   '  ! Known ID, no RMS FAT block     */
/*     'DEC         VMS         0   1   '  ! Known ID, binary RMS FAT block */
/*     'DEC         VMS         0   2   '  ! Known ID, hexadecimal encoded RMS FAT block */
/*                                                                          */
/* ASCII encoded RMS file attribute block                                   */
/*                                                                          */
/* 	f$fao("!1XB!1XB,!2XB,!4XW,!8XL,!8XL,!4XW,!2XB,!2XB,!4XW,!4XW,!4XW,!4XW",- */
/*	        RTYPE,RATTRIB,RSIZE,HIBLK,EFBLK,FFBYTE,BKTSIZE,VFCSIZ,MAXREC,DEFEXT,GBC,VERSIONS) */
/*	FAT$B_RTYPE	! Record type,, file organization ( FAT$V_RTYPE,,FAT$V_FILEORG )  */
/*	FAT$B_RATTRIB	!  record attributes                                */
/*	FAT$W_RSIZE	!  record size in bytes                             */
/*	FAT$L_HIBLK	!  highest allocated VBN (FAT$W_HIBLKH,,FAT$W_HIBLKL) */
/*	FAT$L_EFBLK	!  end of file VBN (FAT$W_EFBLKH,,FAT$W_EFBLKL)     */
/*	FAT$W_FFBYTE	!  first free byte in EFBLK                         */
/*	FAT$B_BKTSIZE	!  bucket size in blocks                            */
/*	FAT$B_VFCSIZE	!  size in bytes of fixed length control for VFC records  */
/*	FAT$W_MAXREC	!  maximum record size in bytes                     */
/*	FAT$W_DEFEXT	!  default extend quantity                          */
/*	FAT$W_GBC	!  global buffer count                              */
/*	FAT$W_VERSIONS	!  default version limit for directory file         */
/*	                                                                    */
